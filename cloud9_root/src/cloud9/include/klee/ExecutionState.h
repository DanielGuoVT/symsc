//===-- ExecutionState.h ----------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_EXECUTIONSTATE_H
#define KLEE_EXECUTIONSTATE_H

#include "klee/Constraints.h"
#include "klee/Expr.h"
#include "klee/Internal/ADT/TreeStream.h"

// FIXME: We do not want to be exposing these? :(
#include "../../lib/Core/AddressSpace.h"
#include "klee/Internal/Module/KInstIterator.h"
#include "klee/Internal/Module/KInstruction.h"

#include "llvm/Support/TimeValue.h"

#include "klee/Threading.h"
#include "klee/MultiProcess.h"
#include "klee/AddressPool.h"
#include "klee/StackTrace.h"

#include <map>
#include <set>
#include <vector>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

//add by shengjian guo
#include<boost/unordered_map.hpp>
using boost::unordered_map;

#include <glog/logging.h>

using namespace llvm;

namespace cloud9 {
namespace worker {
class SymbolicState;
}
}

namespace llvm {
class Instruction;
class Function;
class BasicBlock;
}

namespace klee {
class Array;
class CallPathNode;
struct Cell;
struct KFunction;
struct KInstruction;
class MemoryObject;
class PTreeNode;
struct InstructionInfo;
class Executor;

class ExecutionState;

namespace c9 {
std::ostream &printStateStack(std::ostream &os, const ExecutionState &state);
std::ostream &printStateConstraints(std::ostream &os, const ExecutionState &state);
std::ostream &printStateMemorySummary(std::ostream &os, const ExecutionState &state);
}

std::ostream &operator<<(std::ostream &os, const ExecutionState &state); // XXX Cloud9 hack
std::ostream &operator<<(std::ostream &os, const MemoryMap &mm);

typedef uint64_t wlist_id_t;

//thread id, transition type and variable address
struct TransitionEvent {
	thread_uid_t thread_uid;
	int transition_type;
	uint64_t global_addr;
	std::string ipp_id;
	std::vector<uint64_t> lockset;
	std::string prefix;
	uint64_t period;
	int crtPeriodIndex;
	thread_priority_t priority;
	std::string name;
};

class ExecutionState {
	friend class ObjectState;

public:
	typedef std::vector<StackFrame> stack_ty;

	typedef std::map<thread_uid_t, Thread> threads_ty;
	typedef std::map<process_id_t, Process> processes_ty;
	typedef std::map<wlist_id_t, std::set<thread_uid_t> > wlists_ty;

private:
	// unsupported, use copy constructor
	ExecutionState &operator=(const ExecutionState&);
	std::map<std::string, std::string> fnAliases;

	cloud9::worker::SymbolicState *c9State;

	void setupMain(KFunction *kf);
	void setupTime();
	void setupAddressPool();
public:
	/* System-level parameters */
	Executor *executor;

	unsigned depth;

	/// Disables forking, set by user code.
	bool forkDisabled;

	mutable double queryCost;
	double weight;

	TreeOStream pathOS, symPathOS;
	unsigned instsSinceCovNew;

	bool coveredNew;
	sys::TimeValue lastCoveredTime;

	void setCoveredNew() {
		coveredNew = true;
		lastCoveredTime = sys::TimeValue::now();
	}

	std::map<const std::string*, std::set<unsigned> > coveredLines;

	PTreeNode *ptreeNode;

	int crtForkReason;
	Instruction *crtSpecialFork;

	/// ordered list of symbolics: used to generate test cases.
	//
	// FIXME: Move to a shared list structure (not critical).
	std::vector<std::pair<const MemoryObject*, const Array*> > symbolics;

	ConstraintManager globalConstraints;

	// For a multi threaded ExecutionState
	threads_ty threads;
	processes_ty processes;

	wlists_ty waitingLists;
	wlist_id_t wlistCounter;

	uint64_t stateTime;

	AddressPool addressPool;
	AddressSpace::cow_domain_t cowDomain;

	Thread& createThread(thread_id_t tid, KFunction *kf);
	Process& forkProcess(process_id_t pid);
	void terminateThread(threads_ty::iterator it);
	void terminateProcess(processes_ty::iterator it);

	threads_ty::iterator nextThread(threads_ty::iterator it) {
		if (it == threads.end()) it = threads.begin();
		else {
			it++;
			if (it == threads.end()) it = threads.begin();
		}

		crtProcessIt = processes.find(crtThreadIt->second.getPid());

		return it;
	}

	void scheduleNext(threads_ty::iterator it) {
		assert(it != threads.end());

		crtThreadIt = it;
		crtProcessIt = processes.find(crtThreadIt->second.getPid());
	}

	wlist_id_t getWaitingList() {
		return wlistCounter++;
	}
	void sleepThread(wlist_id_t wlist);
	void notifyOne(wlist_id_t wlist, thread_uid_t tid);
	void notifyAll(wlist_id_t wlist);

	threads_ty::iterator crtThreadIt;
	processes_ty::iterator crtProcessIt;

	unsigned int preemptions;

	/* Shortcut methods */

	Thread &crtThread() {
		return crtThreadIt->second;
	}
	const Thread &crtThread() const {
		return crtThreadIt->second;
	}

	Process &crtProcess() {
		return crtProcessIt->second;
	}
	const Process &crtProcess() const {
		return crtProcessIt->second;
	}

	ConstraintManager &constraints() {
		return globalConstraints;
	}
	const ConstraintManager &constraints() const {
		return globalConstraints;
	}

	AddressSpace &addressSpace() {
		return crtProcess().addressSpace;
	}
	const AddressSpace &addressSpace() const {
		return crtProcess().addressSpace;
	}

	KInstIterator& pc() {
		return crtThread().pc;
	}
	const KInstIterator& pc() const {
		return crtThread().pc;
	}

	KInstIterator& prevPC() {
		return crtThread().prevPC;
	}
	const KInstIterator& prevPC() const {
		return crtThread().prevPC;
	}

public:
	KInstIterator& prevPrevPC() {
		return crtThread().prevPrevPC;
	}

	const KInstIterator& prevPrevPC() const {
		return crtThread().prevPrevPC;
	}

public:
	stack_ty& stack() {
		return crtThread().stack;
	}
	const stack_ty& stack() const {
		return crtThread().stack;
	}

	std::string getFnAlias(std::string fn);
	void addFnAlias(std::string old_fn, std::string new_fn);
	void removeFnAlias(std::string fn);

	/* Logging support */
	unsigned char state_id[20];
	unsigned char parent_id[20];
	unsigned char fork_id[20];

	/* Various counters */
	uint64_t totalInstructions;
	uint64_t totalBranches;
	uint64_t totalForks;
	mutable uint64_t totalQueries; // TODO(bucur): Fix this atrocity
	mutable uint64_t totalTime;
public:
	ExecutionState(Executor *_executor, KFunction *kf);

	// XXX total hack, just used to make a state so solver can
	// use on structure
	ExecutionState(Executor *_executor, const std::vector<klee::ref<Expr> > &assumptions);

//	ExecutionState(const ExecutionState &state);

	~ExecutionState();

	ExecutionState *branch(bool copy = false);

	void pushFrame(Thread &t, KInstIterator caller, KFunction *kf) {
		t.stack.push_back(StackFrame(caller, kf));
	}
	void pushFrame(KInstIterator caller, KFunction *kf) {
		pushFrame(crtThread(), caller, kf);
	}

	void popFrame(Thread &t) {
		StackFrame &sf = t.stack.back();
		for (std::vector<const MemoryObject*>::iterator it = sf.allocas.begin(), ie = sf.allocas.end(); it != ie; ++it)
			processes.find(t.getPid())->second.addressSpace.unbindObject(*it);
		t.stack.pop_back();
	}
	void popFrame() {
		popFrame(crtThread());
	}

	void addSymbolic(const MemoryObject *mo, const Array *array) {
		symbolics.push_back(std::make_pair(mo, array));
	}
	void addConstraint(klee::ref<Expr> e) {
		constraints().addConstraint(e);
	}

	bool merge(const ExecutionState &b);

	cloud9::worker::SymbolicState *getCloud9State() const {
		return c9State;
	}
	void setCloud9State(cloud9::worker::SymbolicState *state) {
		c9State = state;
	}

	StackTrace getStackTrace() const;

	bool isExternalCallSafe() const;

public:
	///////////////////////////////variables & funcs from shengjian guo///////////////////////

	std::vector<ShadowInstruction*> WPTrace;

	//for Coverage Summary & POR
	uint64_t totalWorkerThreads;
	bool is_pruned;
	boost::unordered_map<int, boost::unordered_map<int, int> > tid_inst_count;
	std::vector<std::string> ipp_list; // record the id of each ipp point
	boost::unordered_map<std::string, thread_uid_t> ipp_thread_mapping;
	boost::unordered_map<uint64_t, thread_uid_t> mutex_holding_list;

	bool allReachInterleavingPoint();
	bool isGlobalAddress(uint64_t addr);
	bool isGlobalAddress(const ref<Expr> &addr, KInstruction *i = 0);
	bool isSCAddress(const ref<Expr> &addr, KInstruction *i = 0);
	bool isSymbolic(uint64_t variable_addr);

	//for POR
	std::vector<TransitionEvent> transition_sequence; // record the transition trace
	enum transition_type {
		null = 0, load = 2, store = 3, boundary = 4, boundaryStart = 5, boundaryEnd = 6
	};
	bool reserved_for_POR;
	bool isRedundant;

	std::map<std::string, std::string> ipp_prefix_map;

	void DoMutexUnlockEnd(uint64_t addr);

	// for tapPLC
	unsigned scan;

	thread_priority_t firPrio;
	thread_priority_t secPrio;

	/// for tapsc
	long symbolicAddrNum;
	long concreteAddrNum;
	long stateIdentifer;
	std::vector<ref<Expr> > memAddrs;
	std::vector<KInstruction *> memInsts;
	std::vector<ref<Expr> > shadowMemAddrs;
	bool executedT2;
	unsigned memAccs;
	bool startShadowExecution;

	// address idx --> (hit_cond, miss_cond);
	typedef std::map<unsigned, std::pair<ref<Expr>, ref<Expr> > > ConstraintsMap;

	ConstraintsMap constraintsMap;
	std::map<unsigned, ref<Expr> > symbolicHitLog;
	std::vector<std::pair<unsigned, const Array *> > symbolicHitVars;

	std::vector<std::pair<const Array*, const Array*> > arrayPairs;
	void addExtraSymbolic(const Array *org, const Array *copy) {
		arrayPairs.push_back(std::make_pair(org, copy));
	}
};

}

#endif
