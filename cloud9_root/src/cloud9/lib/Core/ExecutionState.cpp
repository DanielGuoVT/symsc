//===-- ExecutionState.cpp ------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/ExecutionState.h"
#include "klee/Executor.h"

#include "klee/Internal/Module/Cell.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/KModule.h"
#include "klee/util/ExprPPrinter.h"
#include "klee/ForkTag.h"
#include "klee/AddressPool.h"

#include "klee/Expr.h"

#include "Memory.h"

#include "llvm/Function.h"
#include "llvm/Support/CommandLine.h"

#include <iostream>
#include <iomanip>
#include <cassert>
#include <map>
#include <set>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <crypto++/sha.h>

using namespace llvm;
using namespace klee;

namespace {
cl::opt<bool> DebugLogStateMerge("debug-log-state-merge");
}

namespace klee {
class Executor;

/***/

/***/

ExecutionState::ExecutionState(Executor *_executor, KFunction *kf) :
		c9State(NULL), executor(_executor), depth(0), forkDisabled(false), queryCost(0.), weight(1), instsSinceCovNew(0), coveredNew(false), lastCoveredTime(
				sys::TimeValue::now()), ptreeNode(0), crtForkReason(KLEE_FORK_DEFAULT), crtSpecialFork(NULL), wlistCounter(1), preemptions(0), totalInstructions(
				0), totalBranches(0), totalForks(0), totalQueries(0), totalTime(0) {

	totalWorkerThreads = 0;
	is_pruned = false;
	reserved_for_POR = false;
	isRedundant = false;
	scan = 0;
	firPrio = 0;
	secPrio = 0;
	stateIdentifer = _executor->stateCounter++;
	executedT2 = false;
	memAccs = 0;
	startShadowExecution = false;

	memset(state_id, 0, sizeof(state_id));
	memset(parent_id, 0, sizeof(parent_id));
	memset(fork_id, 0, sizeof(fork_id));

	setupMain(kf);
	setupTime();
	setupAddressPool();
}

ExecutionState::ExecutionState(Executor *_executor, const std::vector<klee::ref<Expr> > &assumptions) :
		c9State(NULL), executor(_executor), queryCost(0.), lastCoveredTime(sys::TimeValue::now()), ptreeNode(0), globalConstraints(assumptions), wlistCounter(
				1), preemptions(0), totalInstructions(0), totalBranches(0), totalForks(0), totalQueries(0), totalTime(0) {

	totalWorkerThreads = 0;
	is_pruned = false;
	reserved_for_POR = false;
	isRedundant = false;
	scan = 0;
	firPrio = 0;
	secPrio = 0;
	stateIdentifer = _executor->stateCounter++;
	executedT2 = false;
	memAccs = 0;
	startShadowExecution = false;

	memset(state_id, 0, sizeof(state_id));
	memset(parent_id, 0, sizeof(parent_id));
	memset(fork_id, 0, sizeof(fork_id));

	setupMain(NULL);

}

void ExecutionState::setupTime() {
	stateTime = 1284138206L * 1000000L; // Yeah, ugly, but what else? :)
}

void ExecutionState::setupAddressPool() {
	void *startAddress = mmap((void*) addressPool.getStartAddress(), addressPool.getSize(), PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
	assert(startAddress != MAP_FAILED);

	VLOG(2) << "Address pool starts at " << startAddress << " although it was requested at " << addressPool.getStartAddress();

	addressPool = AddressPool((uint64_t) startAddress, addressPool.getSize()); // Correct the address
}

void ExecutionState::setupMain(KFunction *kf) {
	Process mainProc = Process(2, 1);
	Thread mainThread = Thread(0, 2, kf);

	mainProc.threads.insert(mainThread.tuid);

	threads.insert(std::make_pair(mainThread.tuid, mainThread));
	processes.insert(std::make_pair(mainProc.pid, mainProc));

	crtThreadIt = threads.begin();
	crtProcessIt = processes.find(crtThreadIt->second.getPid());

	cowDomain.push_back(&crtProcessIt->second.addressSpace);

	crtProcessIt->second.addressSpace.cowDomain = &cowDomain;
}

Thread& ExecutionState::createThread(thread_id_t tid, KFunction *kf) {
	totalWorkerThreads++;
	Thread newThread = Thread(tid, crtProcess().pid, kf);
	newThread.uniqueID = totalWorkerThreads;
	std::cerr << "[DBG]: Creating thread, unique_id: " << newThread.uniqueID << std::endl;

	crtProcess().threads.insert(newThread.tuid);
	threads.insert(std::make_pair(newThread.tuid, newThread));

//	if (totalWorkerThreads > 1) this->executedT2 = true;

	return threads.find(newThread.tuid)->second;
}

Process& ExecutionState::forkProcess(process_id_t pid) {
	for (processes_ty::iterator it = processes.begin(); it != processes.end(); it++) {
		it->second.addressSpace.cowKey++;
	}

	Process forked = Process(crtProcess());

	forked.pid = pid;
	forked.ppid = crtProcess().pid;
	forked.threads.clear();
	forked.children.clear();

	forked.forkPath.push_back(1); // Child
	crtProcess().forkPath.push_back(0); // Parent

	Thread forkedThread = Thread(crtThread());
	forkedThread.tuid = std::make_pair(0, forked.pid);

	forked.threads.insert(forkedThread.tuid);

	crtProcess().children.insert(forked.pid);

	threads.insert(std::make_pair(forkedThread.tuid, forkedThread));
	processes.insert(std::make_pair(forked.pid, forked));

	cowDomain.push_back(&processes.find(forked.pid)->second.addressSpace);
	processes.find(forked.pid)->second.addressSpace.cowDomain = &cowDomain;

	return processes.find(forked.pid)->second;
}

void ExecutionState::terminateThread(threads_ty::iterator thrIt) {
	VLOG(1) << "Terminating thread...";

	if (this->executor->DEBUG) {
		std::cerr << "[DBG]: Terminating thread: " << thrIt->second.uniqueID << std::endl;
	}

	Process &proc = processes.find(thrIt->second.getPid())->second;

	assert(proc.threads.size() > 1);
	assert(thrIt != crtThreadIt);
	// We assume the scheduler found a new thread first
	assert(!thrIt->second.enabled);
	assert(thrIt->second.waitingList == 0);

	proc.threads.erase(thrIt->second.tuid);
	threads.erase(thrIt);
}

void ExecutionState::terminateProcess(processes_ty::iterator procIt) {
	VLOG(1) << "Terminating process " << procIt->second.pid;

	assert(processes.size() > 1);

	// Delete all process threads
	for (std::set<thread_uid_t>::iterator it = procIt->second.threads.begin(); it != procIt->second.threads.end(); it++) {
		threads_ty::iterator thrIt = threads.find(*it);
		assert(thrIt != crtThreadIt);
		assert(!thrIt->second.enabled);
		assert(thrIt->second.waitingList == 0);

		threads.erase(thrIt);
	}

	// Update the process hierarchy
	if (procIt->second.ppid != 1) {
		Process &parent = processes.find(procIt->second.ppid)->second;

		parent.children.erase(procIt->second.pid);
	}

	if (procIt->second.children.size() > 0) {
		// Reassign the children to process 1
		for (std::set<process_id_t>::iterator it = procIt->second.children.begin(); it != procIt->second.children.end(); it++) {
			processes.find(*it)->second.ppid = 1;
		}
	}

	// Update the state COW domain
	AddressSpace::cow_domain_t::iterator it = std::find(cowDomain.begin(), cowDomain.end(), &procIt->second.addressSpace);
	assert(it != cowDomain.end());
	cowDomain.erase(it);

	assert(procIt != crtProcessIt);

	processes.erase(procIt);
}

void ExecutionState::sleepThread(wlist_id_t wlist) {
	//assert(crtThread().enabled);
	if (!crtThread().enabled) {
		this->executor->terminateRedundantState(*this, true);
	}
	assert(wlist > 0);
	if (this->executor->DEBUG) {
		std::cerr << "[DBG]: current thread will go sleeping: " << crtThread().getTid() << " for wlist: " << wlist << std::endl;
	}

	crtThread().sleeping = true;
	crtThread().enabled = false;
	crtThread().waitingList = wlist;
	std::set<thread_uid_t> &wl = waitingLists[wlist];
	wl.insert(crtThread().tuid);
	// activeThreads--;
}

void ExecutionState::notifyOne(wlist_id_t wlist, thread_uid_t tuid) {
	assert(wlist > 0);

	std::set<thread_uid_t> &wl = waitingLists[wlist];

	if (wl.erase(tuid) != 1) {
		assert(0 && "thread was not waiting");
	}

	Thread &thread = threads.find(tuid)->second;
	assert(!thread.enabled);
	thread.enabled = true;
	thread.sleeping = false;
	thread.waitingList = 0;
	std::cerr << "thread will be enabled in notifyOne: " << thread.getTid() << std::endl;

	if (wl.size() == 0) {
		waitingLists.erase(wlist);
	}
	// activeThreads++;
}

void ExecutionState::notifyAll(wlist_id_t wlist) {
	assert(wlist > 0);

	//std::cerr << "wlist in notifyAll: " << wlist << std::endl;

	std::set<thread_uid_t> &wl = waitingLists[wlist];

	if (wl.size() > 0) {
		for (std::set<thread_uid_t>::iterator it = wl.begin(); it != wl.end(); it++) {
			Thread &thread = threads.find(*it)->second;
			thread.enabled = true;
			thread.sleeping = false;
			thread.waitingList = 0;
			if (this->executor->DEBUG) {
				std::cerr << "[DBG]: in notifyAll, thread: " << thread.getTid() << " will be enabled for wlist " << wlist << std::endl;
			}
			// activeThreads++;
		}
		wl.clear();
	}

	waitingLists.erase(wlist);
}

ExecutionState::~ExecutionState() {
	for (threads_ty::iterator it = threads.begin(); it != threads.end(); it++) {
		Thread &t = it->second;
		while (!t.stack.empty())
			popFrame(t);
	}
}

ExecutionState *ExecutionState::branch(bool copy) {
	if (!copy) depth++;

	for (processes_ty::iterator it = processes.begin(); it != processes.end(); it++) {
		it->second.addressSpace.cowKey++;
	}

	ExecutionState *falseState = new ExecutionState(*this);
	falseState->stateIdentifer = executor->stateCounter++;

	if (!copy) {
		falseState->coveredNew = false;
		falseState->coveredLines.clear();
	}

	falseState->c9State = NULL;

	falseState->crtThreadIt = falseState->threads.find(crtThreadIt->second.tuid);
	falseState->crtProcessIt = falseState->processes.find(crtProcessIt->second.pid);

	falseState->cowDomain.clear();

	// Rebuilding the COW domain...

	for (processes_ty::iterator it = falseState->processes.begin(); it != falseState->processes.end(); it++) {
		falseState->cowDomain.push_back(&it->second.addressSpace);
	}

	for (processes_ty::iterator it = falseState->processes.begin(); it != falseState->processes.end(); it++) {
		it->second.addressSpace.cowDomain = &falseState->cowDomain;
	}

	if (!copy) {
		weight *= .5;
		falseState->weight -= weight;

		// Compute IDs
		CryptoPP::SHA1 sha1compute;
		unsigned char message[21];
		::memcpy(message, fork_id, 20);

		message[20] = 1;
		sha1compute.CalculateDigest(fork_id, message, 21);

		message[20] = 0;
		sha1compute.CalculateDigest(falseState->fork_id, message, 21);

		::memcpy(falseState->state_id, falseState->fork_id, 20);
		::memcpy(falseState->parent_id, state_id, 20);
	}

	return falseState;
}

///

std::string ExecutionState::getFnAlias(std::string fn) {
	std::map<std::string, std::string>::iterator it = fnAliases.find(fn);
	if (it != fnAliases.end()) return it->second;
	else return "";
}

void ExecutionState::addFnAlias(std::string old_fn, std::string new_fn) {
	fnAliases[old_fn] = new_fn;
}

void ExecutionState::removeFnAlias(std::string fn) {
	fnAliases.erase(fn);
}

/**/

std::ostream &operator<<(std::ostream &os, const MemoryMap &mm) {
	os << "{";
	MemoryMap::iterator it = mm.begin();
	MemoryMap::iterator ie = mm.end();
	if (it != ie) {
		os << "MO" << it->first->id << ":" << it->second;
		for (++it; it != ie; ++it)
			os << ", MO" << it->first->id << ":" << it->second;
	}
	os << "}";
	return os;
}

std::ostream &operator<<(std::ostream &os, const ExecutionState &state) {
	state.getStackTrace().dumpInline(os);
	return os;
	return klee::c9::printStateStack(os, state);
}

bool ExecutionState::merge(const ExecutionState &b) {
	if (DebugLogStateMerge) std::cerr << "-- attempting merge of A:" << this << " with B:" << &b << "--\n";
	if (pc() != b.pc()) return false;

	// XXX is it even possible for these to differ? does it matter? probably
	// implies difference in object states?
	if (symbolics != b.symbolics) return false;

	{
		std::vector<StackFrame>::const_iterator itA = stack().begin();
		std::vector<StackFrame>::const_iterator itB = b.stack().begin();
		while (itA != stack().end() && itB != b.stack().end()) {
			// XXX vaargs?
			if (itA->caller != itB->caller || itA->kf != itB->kf) return false;
			++itA;
			++itB;
		}
		if (itA != stack().end() || itB != b.stack().end()) return false;
	}

	std::set<klee::ref<Expr> > aConstraints(constraints().begin(), constraints().end());
	std::set<klee::ref<Expr> > bConstraints(b.constraints().begin(), b.constraints().end());
	std::set<klee::ref<Expr> > commonConstraints, aSuffix, bSuffix;
	std::set_intersection(aConstraints.begin(), aConstraints.end(), bConstraints.begin(), bConstraints.end(),
			std::inserter(commonConstraints, commonConstraints.begin()));
	std::set_difference(aConstraints.begin(), aConstraints.end(), commonConstraints.begin(), commonConstraints.end(),
			std::inserter(aSuffix, aSuffix.end()));
	std::set_difference(bConstraints.begin(), bConstraints.end(), commonConstraints.begin(), commonConstraints.end(),
			std::inserter(bSuffix, bSuffix.end()));
	if (DebugLogStateMerge) {
		std::cerr << "\tconstraint prefix: [";
		for (std::set<klee::ref<Expr> >::iterator it = commonConstraints.begin(), ie = commonConstraints.end(); it != ie; ++it)
			std::cerr << *it << ", ";
		std::cerr << "]\n";
		std::cerr << "\tA suffix: [";
		for (std::set<klee::ref<Expr> >::iterator it = aSuffix.begin(), ie = aSuffix.end(); it != ie; ++it)
			std::cerr << *it << ", ";
		std::cerr << "]\n";
		std::cerr << "\tB suffix: [";
		for (std::set<klee::ref<Expr> >::iterator it = bSuffix.begin(), ie = bSuffix.end(); it != ie; ++it)
			std::cerr << *it << ", ";
		std::cerr << "]\n";
	}

	// We cannot merge if addresses would resolve differently in the
	// states. This means:
	//
	// 1. Any objects created since the branch in either object must
	// have been free'd.
	//
	// 2. We cannot have free'd any pre-existing object in one state
	// and not the other

	if (DebugLogStateMerge) {
		std::cerr << "\tchecking object states\n";
		std::cerr << "A: " << addressSpace().objects << "\n";
		std::cerr << "B: " << b.addressSpace().objects << "\n";
	}

	std::set<const MemoryObject*> mutated;
	MemoryMap::iterator ai = addressSpace().objects.begin();
	MemoryMap::iterator bi = b.addressSpace().objects.begin();
	MemoryMap::iterator ae = addressSpace().objects.end();
	MemoryMap::iterator be = b.addressSpace().objects.end();
	for (; ai != ae && bi != be; ++ai, ++bi) {
		if (ai->first != bi->first) {
			if (DebugLogStateMerge) {
				if (ai->first < bi->first) {
					std::cerr << "\t\tB misses binding for: " << ai->first->id << "\n";
				} else {
					std::cerr << "\t\tA misses binding for: " << bi->first->id << "\n";
				}
			}
			return false;
		}
		if (ai->second != bi->second) {
			if (DebugLogStateMerge) std::cerr << "\t\tmutated: " << ai->first->id << "\n";
			mutated.insert(ai->first);
		}
	}
	if (ai != ae || bi != be) {
		if (DebugLogStateMerge) std::cerr << "\t\tmappings differ\n";
		return false;
	}

	// merge stack

	klee::ref<Expr> inA = ConstantExpr::alloc(1, Expr::Bool);
	klee::ref<Expr> inB = ConstantExpr::alloc(1, Expr::Bool);
	for (std::set<klee::ref<Expr> >::iterator it = aSuffix.begin(), ie = aSuffix.end(); it != ie; ++it)
		inA = AndExpr::create(inA, *it);
	for (std::set<klee::ref<Expr> >::iterator it = bSuffix.begin(), ie = bSuffix.end(); it != ie; ++it)
		inB = AndExpr::create(inB, *it);

	// XXX should we have a preference as to which predicate to use?
	// it seems like it can make a difference, even though logically
	// they must contradict each other and so inA => !inB

	std::vector<StackFrame>::iterator itA = stack().begin();
	std::vector<StackFrame>::const_iterator itB = b.stack().begin();
	for (; itA != stack().end(); ++itA, ++itB) {
		StackFrame &af = *itA;
		const StackFrame &bf = *itB;
		for (unsigned i = 0; i < af.kf->numRegisters; i++) {
			klee::ref<Expr> &av = af.locals[i].value;
			const klee::ref<Expr> &bv = bf.locals[i].value;
			if (av.isNull() || bv.isNull()) {
				// if one is null then by implication (we are at same pc)
				// we cannot reuse this local, so just ignore
			} else {
				av = SelectExpr::create(inA, av, bv);
			}
		}
	}

	for (std::set<const MemoryObject*>::iterator it = mutated.begin(), ie = mutated.end(); it != ie; ++it) {
		const MemoryObject *mo = *it;
		const ObjectState *os = addressSpace().findObject(mo);
		const ObjectState *otherOS = b.addressSpace().findObject(mo);
		assert(os && !os->readOnly && "objects mutated but not writable in merging state");
		assert(otherOS);

		ObjectState *wos = addressSpace().getWriteable(mo, os);
		for (unsigned i = 0; i < mo->size; i++) {
			klee::ref<Expr> av = wos->read8(i);
			klee::ref<Expr> bv = otherOS->read8(i);
			wos->write(i, SelectExpr::create(inA, av, bv));
		}
	}

	constraints() = ConstraintManager();
	for (std::set<klee::ref<Expr> >::iterator it = commonConstraints.begin(), ie = commonConstraints.end(); it != ie; ++it)
		constraints().addConstraint(*it);
	constraints().addConstraint(OrExpr::create(inA, inB));

	return true;
}

/***/

StackTrace ExecutionState::getStackTrace() const {
	StackTrace result;

	const KInstruction *target = prevPC();

	for (ExecutionState::stack_ty::const_reverse_iterator it = stack().rbegin(), ie = stack().rend(); it != ie; ++it) {

		const StackFrame &sf = *it;

		StackTrace::position_t position = std::make_pair(sf.kf, target);
		std::vector<klee::ref<Expr> > arguments;

		Function *f = sf.kf->function;
		unsigned index = 0;
		for (Function::arg_iterator ai = f->arg_begin(), ae = f->arg_end(); ai != ae; ++ai) {

			klee::ref<Expr> value = sf.locals[sf.kf->getArgRegister(index++)].value;
			arguments.push_back(value);
		}

		result.contents.push_back(std::make_pair(position, arguments));

		target = sf.caller;
	}

	return result;
}

// XXX: Ugly hack.
// Whitelist external calls that go through the POSIX model.  We look at
// the stack trace of the call, and see if there are any functions whose
// source code is in our POSIX model.
bool ExecutionState::isExternalCallSafe() const {
	StackTrace stack_trace = getStackTrace();

	for (StackTrace::stack_t::iterator it = stack_trace.contents.begin(), ie = stack_trace.contents.end(); it != ie; ++it) {
		const KInstruction *ki = (*it).first.second;
		if (ki && ki->info) {
			if (ki->info->file.find("runtime/POSIX") != std::string::npos) return true;
		}
	}

	return false;
}

void ExecutionState::DoMutexUnlockEnd(uint64_t addr) {

//	std::cerr<<"mutex "<<addr<<" is released by crtThread "<<crtThread().getTid()<<std::endl;
	uint64_t mutex = addr;
	// 1. remove current lock from locksets
	assert(mutex && "locksets should contain current lock");
	assert(crtThread().lockset.count(mutex));
	crtThread().lockset.erase(mutex);
	// 2. remove mutex-holding info
	mutex_holding_list.erase(mutex);
	// 3. notify all sleeping threads that are waiting for the mutex
	std::set<thread_uid_t> &wl = waitingLists[mutex];
	if (wl.size() > 0) {
		for (std::set<thread_uid_t>::iterator it = wl.begin(); it != wl.end(); it++) {
			Thread &thread = threads.find(*it)->second;
			if (this->executor->DEBUG) {
				std::cerr << "[DBG]: crtThread " << crtThread().getTid() << " makes thread " << thread.getTid() << " awake for unlocking mutex "
						<< addr << std::endl;
			}
		}
	}
	notifyAll(mutex);
}

/*
 * check if a global variable is symbolic
 */
bool ExecutionState::isSymbolic(uint64_t variable_addr) {
	bool isSymbolic = false;
	std::vector<std::pair<const MemoryObject*, const Array*> >::iterator vit = this->symbolics.begin();
	while (vit != symbolics.end()) {
		uint64_t tmp_addr = vit->first->address;
		if (tmp_addr == variable_addr) {
			isSymbolic = true;
			break;
		}
		vit++;
	}
	return isSymbolic;
}

bool ExecutionState::isGlobalAddress(uint64_t addr) {
	MemoryObject hack(addr);
	if (const MemoryMap::value_type *res = addressSpace().objects.lookup_previous(&hack)) {
		const MemoryObject *mo = res->first;
		if ((mo->size == 0 && addr == mo->address) || (addr - mo->address < mo->size)) {
			if (mo->isGlobal) {
				return true;
			} else if (!mo->isLocal && mo->isDynAllocated) return true;
		}
	}
	return false;
}

bool ExecutionState::isSCAddress(const ref<Expr> &addr, KInstruction *ki) {
	bool result = false;
	assert(executor->USC);
	if (crtThread().getTid() == 1) { // T1
		if (addr->getKind() == Expr::Constant) result = false;
		else if (ki->sbox != "") {
			result = true;
		}
	} else if (crtThread().getTid() == 2) {
		result = true;
	}
	return result;
}

bool ExecutionState::isGlobalAddress(const ref<Expr> &addr, KInstruction *ki) {
	bool result = false;
	if (addr->getKind() == Expr::Constant) {
		result = isGlobalAddress((dyn_cast<ConstantExpr>(addr))->getZExtValue());
	} else {
		result = true; // we treat all symbolic addresses as global now
	}
	return result;
}

bool ExecutionState::allReachInterleavingPoint() {
	bool flag = true;
	for (threads_ty::iterator tit = this->threads.begin(); tit != this->threads.end(); tit++) {
		if (tit->second.getTid() != 0 && tit->second.enabled && !tit->second.reachInterleavingPoint) {
			flag = false;
			break;
		}
	}
	return flag;
}

}

