//===-- Executor_States.cpp -----------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Executor.h"

#include "Memory.h"
#include "MemoryManager.h"
#include "PTree.h"
#include "StatsTracker.h"
#include "TimingSolver.h"
#include "klee/CoreStats.h"
#include "klee/data/ExprSerializer.h"
#include "klee/data/ExprDeserializer.h"
#include "klee/ExprBuilder.h"
#include "klee/Searcher.h"
#include "klee/TimerStatIncrementer.h"
#include "klee/Internal/ADT/RNG.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/System/Time.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_os_ostream.h"
#include "klee/util/ExprPPrinter.h"
#include "klee/data/ExprDeserializer.h"

#include "llvm/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Process.h"

#include "llvm/Constants.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Support/raw_ostream.h"

#include <sys/mman.h>
#include <glog/logging.h>

#include <algorithm>
#include <sstream>

#include <boost/lexical_cast.hpp>

using namespace llvm;

namespace {

cl::opt<bool> DumpStatesOnHalt("dump-states-on-halt", cl::init(false));

cl::opt<bool> EmitAllErrors("emit-all-errors", cl::init(false), cl::desc("Generate tests cases for all errors "
		"(default=one per (error,instruction) pair)"));

cl::opt<unsigned> MaxForks("max-forks", cl::desc("Only fork this many times (-1=off)"), cl::init(~0u));

cl::opt<unsigned> MaxMemory("max-memory", cl::desc("Refuse to fork when more above this about of memory (in MB, 0=off)"), cl::init(0));

cl::opt<bool> MaxMemoryInhibit("max-memory-inhibit", cl::desc("Inhibit forking at memory cap (vs. random terminate)"), cl::init(true));

cl::opt<double> MaxStaticForkPct("max-static-fork-pct", cl::init(1.));
cl::opt<double> MaxStaticSolvePct("max-static-solve-pct", cl::init(1.));
cl::opt<double> MaxStaticCPForkPct("max-static-cpfork-pct", cl::init(1.));
cl::opt<double> MaxStaticCPSolvePct("max-static-cpsolve-pct", cl::init(1.));
cl::opt<bool> OnlyOutputStatesCoveringNew("only-output-states-covering-new", cl::init(false));

void *theMMap = 0;
unsigned theMMapSize = 0;

}

namespace klee {

RNG theRNG;

void Executor::branch(ExecutionState &state, klee::ref<Expr> condition, const std::vector<std::pair<BasicBlock*, klee::ref<Expr> > > &options,
		std::vector<std::pair<BasicBlock*, ExecutionState*> > &branches, int reason, bool flag) {
	std::vector<std::pair<BasicBlock*, klee::ref<Expr> > > targets;

	// sjguo
	std::vector<std::pair<BasicBlock*, klee::ref<Expr> > > wpTargets;
	Instruction *inst = state.prevPC()->inst;
//	klee::ref<Expr> wpCondition = WPRegularExpr::create(inst->getOperand(0)->getName().str(), 0, condition->getWidth(), state.crtThread().getTid());
	klee::ref<Expr> wpIsDefault = options[0].second;

	klee::ref<Expr> isDefault = options[0].second;
	bool success;
	bool result;

	for (unsigned i = 1; i < options.size(); ++i) {
		success = false;

		klee::ref<Expr> match = EqExpr::create(condition, options[i].second);
		isDefault = AndExpr::create(isDefault, Expr::createIsZero(match));

		// sjguo
//		klee::ref<Expr> wpMatch = EqExpr::create(wpCondition, options[i].second);
//		wpIsDefault = AndExpr::create(wpIsDefault, Expr::createIsZero(wpMatch));

// Check the feasibility of this option
		success = solver->mayBeTrue(data::SWITCH_FEASIBILITY, state, match, result);

		assert(success && "FIXME: Unhandled solver failure");

		if (result) {
			// Merge same destinations
			unsigned k;
			for (k = 0; k < targets.size(); k++) {
				if (targets[k].first == options[i].first) {
					targets[k].second = OrExpr::create(match, targets[k].second);
					break;
				}
			}
			// If no same destinations
			if (k == targets.size()) {
				targets.push_back(std::make_pair(options[i].first, match));
			}

			// sjguo
//			if (flag) {
//				for (k = 0; k < wpTargets.size(); k++) {
//					if (wpTargets[k].first == options[i].first) {
//						wpTargets[k].second = OrExpr::create(wpMatch, wpTargets[k].second);
//						break;
//					}
//				}
//				if (k == wpTargets.size()) {
//					wpTargets.push_back(std::make_pair(options[i].first, wpMatch));
//				}
//			}
		}
	}

	// Check the feasibility of the default option
	success = false;
	success = solver->mayBeTrue(data::SWITCH_FEASIBILITY, state, isDefault, result);
	assert(success && "FIXME: Unhandled solver failure");

	if (result) {
		unsigned k;
		for (k = 0; k < targets.size(); k++) {
			if (targets[k].first == options[0].first) {
				targets[k].second = OrExpr::create(isDefault, targets[k].second);
				break;
			}
		}
		// If no same destinations
		if (k == targets.size()) {
			targets.push_back(std::make_pair(options[0].first, isDefault));
		}

		// sjguo
		if (flag) {
			for (k = 0; k < wpTargets.size(); k++) {
				if (wpTargets[k].first == options[0].first) {
					wpTargets[k].second = OrExpr::create(wpIsDefault, wpTargets[k].second);
					break;
				}
			}
			if (k == wpTargets.size()) {
				wpTargets.push_back(std::make_pair(options[0].first, wpIsDefault));
			}
		}
	} else {
		std::cerr << "[DBG]: validity is false here." << std::endl;
	}

	TimerStatIncrementer timer(stats::forkTime);
	assert(!targets.empty());

	stats::forks += targets.size() - 1;
	state.totalForks += targets.size() - 1;

	ForkTag tag = getForkTag(state, reason);

	// XXX do proper balance or keep random?
	for (unsigned i = 0; i < targets.size(); ++i) {
		ExecutionState *es = &state;
		ExecutionState *ns = es;
		if (i > 0) {
			ns = es->branch();
			addedStates.insert(ns);
			if (es->ptreeNode) {
				es->ptreeNode->data = 0;
				std::pair<PTree::Node*, PTree::Node*> res = processTree->split(es->ptreeNode, ns, es, tag);
				ns->ptreeNode = res.first;
				es->ptreeNode = res.second;
			}
			fireStateBranched(ns, es, 0, tag);
		}

		branches.push_back(std::make_pair(targets[i].first, ns));
	}

	// Add constraints at the end, in order to keep the parent state unaltered
	for (unsigned i = 0; i < targets.size(); ++i) {
		addConstraint(*branches[i].second, targets[i].second);
	}
}

Executor::StatePair Executor::fork(ExecutionState &current, klee::ref<Expr> condition, bool isInternal, int reason, bool flag, bool hasSummary) {

	ForkTag tag = getForkTag(current, reason);

	// TODO(bucur): Understand this, or wipe it altogether
	if (!isa<ConstantExpr>(condition) && (MaxStaticForkPct != 1. || MaxStaticSolvePct != 1. || MaxStaticCPForkPct != 1. || MaxStaticCPSolvePct != 1.)
			&& statsTracker->elapsed() > 60.) {
		StatisticManager &sm = *theStatisticManager;
		CallPathNode *cpn = current.stack().back().callPathNode;
		if ((MaxStaticForkPct < 1. && sm.getIndexedValue(stats::forks, sm.getIndex()) > stats::forks * MaxStaticForkPct)
				|| (MaxStaticCPForkPct < 1. && cpn && (cpn->statistics.getValue(stats::forks) > stats::forks * MaxStaticCPForkPct))
				|| (MaxStaticSolvePct < 1 && sm.getIndexedValue(stats::solverTime, sm.getIndex()) > stats::solverTime * MaxStaticSolvePct)
				|| (MaxStaticCPForkPct < 1. && cpn && (cpn->statistics.getValue(stats::solverTime) > stats::solverTime * MaxStaticCPSolvePct))) {
			klee::ref<ConstantExpr> value;
			bool success = solver->getValue(data::BRANCH_CONDITION_CONCRETIZATION, current, condition, value);
			assert(success && "FIXME: Unhandled solver failure");
			(void) success;
			LOG(INFO) << "NONDETERMINISM! New constraint added!";
			addConstraint(current, EqExpr::create(value, condition));
			condition = value;
		}
	}

	Solver::PartialValidity partial_validity;
	Solver::Validity validity;
	bool success = false;

	if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(condition))) {
		success = true;
		validity = CE->isFalse() ? Solver::False : Solver::True;
	} else {
		solver->setTimeout(stpTimeout);
		success = solver->evaluate(data::BRANCH_FEASIBILITY, current, condition, validity);
		solver->setTimeout(0);

		if (!success) {
			current.pc() = current.prevPC();
			terminateStateEarly(current, "query timed out");
			return StatePair((klee::ExecutionState*) NULL, (klee::ExecutionState*) NULL);
		}
	}

	assert(success && "FIXME: Solver failure");

	switch (validity) {
	case Solver::True:
		partial_validity = Solver::MustBeTrue;
		break;
	case Solver::False:
		partial_validity = Solver::MustBeFalse;
		break;
	case Solver::Unknown:
		partial_validity = Solver::TrueOrFalse;
		break;
	}

	if (partial_validity == Solver::TrueOrFalse) {
		if ((MaxMemoryInhibit && atMemoryLimit) || current.forkDisabled || inhibitForking || (MaxForks != ~0u && stats::forks >= MaxForks)) {

			if (MaxMemoryInhibit && atMemoryLimit)
			LOG_EVERY_N(WARNING, 1) << "skipping fork (memory cap exceeded)";
			else if (current.forkDisabled)
			LOG_EVERY_N(WARNING, 1) << "skipping fork (fork disabled on current path)";
			else if (inhibitForking)
			LOG_EVERY_N(WARNING, 1) << "skipping fork (fork disabled globally)";
			else
			LOG_EVERY_N(WARNING, 1) << "skipping fork (max-forks reached)";

			TimerStatIncrementer timer(stats::forkTime);
			if (theRNG.getBool()) {
				addConstraint(current, condition);
				partial_validity = Solver::MustBeTrue;
			} else {
				addConstraint(current, Expr::createIsZero(condition));
				partial_validity = Solver::MustBeFalse;
			}
		}
	}

	// XXX - even if the constraint is provable one way or the other we
	// can probably benefit by adding this constraint and allowing it to
	// reduce the other constraints. For example, if we do a binary
	// search on a particular value, and then see a comparison against
	// the value it has been fixed at, we should take this as a nice
	// hint to just use the single constraint instead of all the binary
	// search ones. If that makes sense.

	switch (partial_validity) {
	case Solver::MustBeTrue: {
		if (!isInternal) {
			if (pathWriter) {
				current.pathOS << "1";
			}
		}

		return StatePair(&current, (klee::ExecutionState*) NULL);
	}
	case Solver::MustBeFalse: {
		if (!isInternal) {
			if (pathWriter) {
				current.pathOS << "0";
			}
		}
		return StatePair((klee::ExecutionState*) NULL, &current);
	}
	case Solver::TrueOrFalse:
	case Solver::MayBeTrue:
	case Solver::MayBeFalse:
	case Solver::None: {

		TimerStatIncrementer timer(stats::forkTime);
		ExecutionState *falseState, *trueState = &current;
		++stats::forks;
		++current.totalForks;

		falseState = trueState->branch();
		addedStates.insert(falseState);

		if (current.ptreeNode) {
			current.ptreeNode->data = 0;
			std::pair<PTree::Node*, PTree::Node*> res = processTree->split(current.ptreeNode, falseState, trueState, tag);
			falseState->ptreeNode = res.first;
			trueState->ptreeNode = res.second;
		}

		if (&current == falseState) fireStateBranched(trueState, falseState, 1, tag);
		else fireStateBranched(falseState, trueState, 0, tag);

		if (!isInternal) {
			if (pathWriter) {
				falseState->pathOS = pathWriter->open(current.pathOS);
				trueState->pathOS << "1";
				falseState->pathOS << "0";
			}
			if (symPathWriter) {
				falseState->symPathOS = symPathWriter->open(current.symPathOS);
				trueState->symPathOS << "1";
				falseState->symPathOS << "0";
			}
		}

		addConstraint(*trueState, condition);
		addConstraint(*falseState, Expr::createIsZero(condition));

		if (USC) {
			inter++;
		}

		return StatePair(trueState, falseState);
	}
	}
}

Executor::StatePair Executor::fork(ExecutionState &current, int reason) {
	ExecutionState *lastState = &current;
	ForkTag tag = getForkTag(current, reason);

	ExecutionState *newState = lastState->branch();
	addedStates.insert(newState);

	if (lastState->ptreeNode) {
		lastState->ptreeNode->data = 0;
		std::pair<PTree::Node*, PTree::Node*> res = processTree->split(lastState->ptreeNode, newState, lastState, tag);
		newState->ptreeNode = res.first;
		lastState->ptreeNode = res.second;
	}

	fireStateBranched(newState, lastState, 0, tag);
	return StatePair(newState, lastState);
}

ForkTag Executor::getForkTag(ExecutionState &current, int forkClass) {
	ForkTag tag((ForkClass) forkClass);
	if (current.crtThreadIt == current.threads.end()) return tag;
	tag.functionName = current.stack().back().kf->function->getName();
	tag.instrID = current.prevPC()->info->id;
	if (tag.forkClass == KLEE_FORK_FAULTINJ) {
		tag.fiVulnerable = false;
		// Check to see whether we are in a vulnerable call

		for (ExecutionState::stack_ty::iterator it = current.stack().begin(); it != current.stack().end(); it++) {
			if (!it->caller) continue;

			KCallInstruction *callInst = dyn_cast<KCallInstruction>((KInstruction*) it->caller);
			assert(callInst);

			if (callInst->vulnerable) {
				tag.fiVulnerable = true;
				break;
			}
		}
	}

	return tag;
}

void Executor::finalizeRemovedStates() {
	// TODO(bucur): Memory management here is quite cumbersome. Consider
	// switching to smart pointers everywhere...

	for (std::set<ExecutionState*>::iterator it = removedStates.begin(), ie = removedStates.end(); it != ie; ++it) {
		ExecutionState *es = *it;

		std::set<ExecutionState*>::iterator it2 = states.find(es);
		assert(it2 != states.end());
		states.erase(it2);

		if (es->ptreeNode) processTree->remove(es->ptreeNode);

		delete es;
	}

	removedStates.clear();
}

void Executor::updateStates(ExecutionState *current) {
	if (searcher) {
		assert(false);
		searcher->update(current, addedStates, removedStates);
	}
	if (statsTracker) {
		statsTracker->updateStates(current, addedStates, removedStates);
	}

	for (std::set<ExecutionState*>::iterator it = removedStates.begin(), ie = removedStates.end(); it != ie; ++it) {
//		fireStateDestroy(*it, false);
		// do not generate test case, sjguo
		fireStateDestroy(*it, true);
	}

	states.insert(addedStates.begin(), addedStates.end());
	addedStates.clear();

	finalizeRemovedStates();
}

bool Executor::terminateState(ExecutionState &state, bool silenced) {
	if (DEBUG) {
		std::cerr << "[DBG]: terminating state: " << &state << std::endl;
	}

	if (precise && !state.isRedundant && !smallSboxes) {
		std::cerr << "-------------------------------------------------------" << std::endl << std::endl;
		std::cerr << "[DBG] Start csc analysis in precise mode: " << &state << ", sym addrs: " << state.memAddrs.size() << std::endl;
		processCacheAccessPrecise(state, solver);
		std::cerr << "Terminate state " << &state << " after csc analysis." << std::endl;
		std::cerr << "-------------------------------------------------------" << std::endl;
	}

	if (state.memAccs > maxMemAccs) maxMemAccs = state.memAccs;
	std::cerr << "maxaddr accs size: " << maxMemAccs << std::endl;
	freopen("./log.maxaccs", "w", stderr);
	std::cerr << maxMemAccs;

	interpreterHandler->incPathsExplored();

	std::set<ExecutionState*>::iterator it = addedStates.find(&state);
	if (it == addedStates.end()) {
		state.pc() = state.prevPC();
		removedStates.insert(&state);
	} else {
		fireStateDestroy(&state, silenced);
		// never reached searcher, just delete immediately
		addedStates.erase(it);
		if (state.ptreeNode) {
			processTree->remove(state.ptreeNode);
		}
		delete &state;
	}
	return true;
}

void Executor::terminateStateEarly(ExecutionState &state, const Twine &message) {
	if (DEBUG) {
		std::cerr << "[DBG]: terminating state early: " << &state << std::endl;
	}
	if (!OnlyOutputStatesCoveringNew || state.coveredNew) {
		interpreterHandler->processTestCase(state, (message + "\n").str().c_str(), "early");
		if (UseNoOutput) {
			terminateState(state, true);
		} else {
			interpreterHandler->processTestCase(state, (message + "\n").str().c_str(), "early");
			terminateState(state, false);
		}
	} else {
		terminateState(state, true);
	}
}

bool Executor::isStateContained(ExecutionState& state, std::map<std::string, ref<Expr> >& memMap) {
	if (!UsePLCStateCheck) return false;
	if (this->visitedStates.empty()) {
		return false;
	}
	bool contained = false;
	// check the containing relationship
	std::vector<std::map<std::string, ref<Expr> > >::iterator vit = this->visitedStates.begin();
	for (; vit != this->visitedStates.end(); vit++) {
		ref<Expr> stateExpr = ConstantExpr::create(1, Expr::Bool);
		bool flag = false;
		// Now check each recorded state
		std::map<std::string, ref<Expr> >::iterator mit = vit->begin();
		for (; mit != vit->end(); mit++) {
			assert(this->globalStatefulVars.count(mit->first));
			ref<Expr> result = memMap[mit->first];

			ref<Expr> partial = EqExpr::create(result, mit->second);
			if (ConstantExpr * CE1 = (dyn_cast<ConstantExpr>(partial))) {
				if (CE1->isFalse()) {
					flag = true;
					break;
				}
			}
			stateExpr = AndExpr::create(stateExpr, partial);
		}
		if (flag) continue;
		stateExpr = Expr::createIsZero(stateExpr);
		if (ConstantExpr * CE1 = (dyn_cast<ConstantExpr>(stateExpr))) {
			if (CE1->isFalse()) {
				contained = true;
				break;
			}
		} else {
			solver->setTimeout(stpTimeout);
			Solver::Validity validity1 = Solver::Unknown;
			bool success = solver->evaluate(data::OTHER, state, stateExpr, validity1);
			solver->setTimeout(0);
			if (validity1 == Solver::False) {
				// current state is contained
				contained = true;
				break;
			}
		}
	}
	return contained;
}

// for tapPLC, sjguo

void Executor::printTrace(ExecutionState &state) {
	std::vector<TransitionEvent> &trace = state.transition_sequence;
	unsigned size = trace.size();
	std::cerr << "[DBG]: event trace of current state: " << &state << std::endl;

	for (unsigned i = 0; i < size; i++) {
		TransitionEvent &e1 = trace[i];
		std::cerr << e1.thread_uid.first;
	}
	std::cerr << std::endl;
	for (unsigned i = 0; i < size; i++) {
		TransitionEvent &e1 = trace[i];
		std::cerr << "thread: " << e1.thread_uid.first << " , type: " << e1.transition_type << " , addr: ";
		if (e1.transition_type == ExecutionState::load || e1.transition_type == ExecutionState::store) std::cerr << e1.global_addr;
		else std::cerr << "000000000000000";
		std::cerr << " , period: [" << e1.crtPeriodIndex * e1.period << ", " << (e1.crtPeriodIndex + 1) * e1.period << "]";
		std::cerr << " , priority: " << e1.priority;
		std::cerr << " , ipp: " << e1.ipp_id << std::endl;
	}
}

bool Executor::isTraceFeasible(ExecutionState &state) {
	if (this->DEBUG) {
		std::cerr << "[DBG]: Enter isTraceFeasible in state " << &state << std::endl;
	}
	bool flag = true;
	std::vector<TransitionEvent> &trace = state.transition_sequence;
	unsigned size = trace.size();
	if (size == 0) {
		return true;
	}
	// First, check priority
	if (UsePriorityPreemption) {
		// high-preempt-low rule
		for (unsigned i = 0; i < size - 1; i++) {
			TransitionEvent &e1 = trace[i];
			TransitionEvent &e2 = trace[i + 1];
			if (e1.thread_uid == e2.thread_uid) continue;
			if (e1.transition_type == ExecutionState::boundary) continue;
			if (e1.transition_type == ExecutionState::boundaryStart) continue;
			if (e1.transition_type == ExecutionState::boundaryEnd) continue;
			if (e2.transition_type == ExecutionState::boundaryStart) continue;
			if (e2.transition_type == ExecutionState::boundary) continue;
			if (e2.transition_type == ExecutionState::boundaryEnd) continue;
			if (e1.priority >= e2.priority) {
				if (this->DEBUG) std::cerr << "[DBG]: false flag in prio checking 1" << std::endl;
				flag = false;
				return flag;
			}
		}

		// special cases
		for (unsigned i = 0; i < size; i++) {
			// the events in first segment must be from the first-prio task
			TransitionEvent &e = trace[i];
			if (e.transition_type == ExecutionState::boundaryStart) continue;
			if (e.transition_type == ExecutionState::boundary) break;
			if (e.transition_type == ExecutionState::boundaryEnd) break;
			if (e.priority != state.firPrio) {
				if (this->DEBUG) std::cerr << "[DBG]: false flag in prio checking 2 (special case 1)" << std::endl;
				flag = false;
				return flag;
			}
		}

		for (unsigned i = 0; i < size - 1; i++) {
			// the first event after first bound must be from the second-prio task
			TransitionEvent &e = trace[i];
			if (e.transition_type == ExecutionState::boundary) {
				TransitionEvent e1 = trace[i + 1];
				if (e1.priority != state.secPrio) {
					if (this->DEBUG) std::cerr << "[DBG]: false flag in prio checking 3 (special case 2)" << std::endl;
					flag = false;
					return flag;
				}
				break;
			}
		}
	}

	// Second, check period
	if (flag && CheckJobPeriod) {
		for (unsigned i = 0; i < size - 1; i++) {
			TransitionEvent &e1 = trace[i];
			TransitionEvent &e2 = trace[i + 1];
			if (e1.thread_uid == e2.thread_uid) continue;

			uint64_t left1 = e1.crtPeriodIndex * e1.period;
			uint64_t right1 = (e1.crtPeriodIndex + 1) * e1.period;
			uint64_t left2 = e2.crtPeriodIndex * e2.period;

			if (e1.transition_type == ExecutionState::boundaryStart) continue;
			if (e1.transition_type == ExecutionState::boundary) {
				if (right1 < left2) {
					if (this->DEBUG) std::cerr << "[DBG]: false flag in period checking 1 (bound-next)" << std::endl;
					flag = false;
					return flag;
				}
				continue;
			}
			if (e1.transition_type == ExecutionState::boundaryEnd) continue;
			if (e2.transition_type == ExecutionState::boundaryStart) continue;
			if (e2.transition_type == ExecutionState::boundary) continue;
			if (e2.transition_type == ExecutionState::boundaryEnd) continue;

			// If job A is preempted by higher-prio job B, then the start time of B must be in (A.start, A.end)
			if (UsePriorityPreemption) {
				assert(e1.priority < e2.priority);
				if (left1 >= left2 || right1 < left2) {
					if (this->DEBUG) std::cerr << "[DBG]: false flag in period checking 2 (period-preemption)" << std::endl;
					flag = false;
					return flag;
				}
			}
		}

		for (unsigned i = 0; i < size - 1; i++) {
			TransitionEvent &e1 = trace[i];
			if (e1.transition_type == ExecutionState::boundary) continue;
			if (e1.transition_type == ExecutionState::boundaryStart) continue;
			if (e1.transition_type == ExecutionState::boundaryEnd) continue;
			uint64_t left1 = e1.crtPeriodIndex * e1.period;
			uint64_t right1 = (e1.crtPeriodIndex + 1) * e1.period;

			for (unsigned j = i + 1; j < size; j++) {
				TransitionEvent &e3 = trace[j];
				if (e1.thread_uid == e3.thread_uid) continue;
				if (e3.transition_type == ExecutionState::boundary) continue;
				if (e3.transition_type == ExecutionState::boundaryStart) continue;
				if (e3.transition_type == ExecutionState::boundaryEnd) continue;
				uint64_t left3 = e3.crtPeriodIndex * e3.period;
				uint64_t right3 = (e3.crtPeriodIndex + 1) * e3.period;

				// events should respect the happens-before rule in terms of period
				if (left1 >= right3) {
					if (this->DEBUG) std::cerr << "[DBG]: false flag in period checking 3 (happens-before)" << std::endl;
					flag = false;
					return flag;
				}

				// if job A has higher priority than job B, and its start time is earlier than that of B,
				// then all events from A must happen before all events from B
				if (UsePriorityPreemption) {
					if (left1 > left3 && e1.priority < e3.priority) {
						if (this->DEBUG) std::cerr << "[DBG]: false flag in period checking 4 (higher-prio finishes first)" << std::endl;
						flag = false;
						return flag;
					}
				}
			}
		}
	}

	if (this->DEBUG) {
		std::cerr << "[DBG]: Leave isTraceFeasible in state " << &state << std::endl;
	}
	return flag;
}

bool Executor::isStateVisited(ExecutionState &state) {
	bool flag = false;
	if (this->UsePLCStateCheck) {
		std::vector<std::pair<std::string, std::vector<unsigned char> > > res;
		this->getSymbolicSolution(state, res);
		// 1. construct the var-value map for current state
		GlobalVarMap &globalVarInfo = this->globalStatefulVars;
		GlobalVarMap::iterator mit = globalVarInfo.begin();
		std::map<std::string, ref<Expr> > crtStateMemMap;
		for (; mit != globalVarInfo.end(); mit++) {
			ref<Expr> addr = Expr::createPointer(mit->second.first);
			Expr::Width width = mit->second.second;
			ref<Expr> result = this->loadMemoryObject(state, addr, width);
			// comment for tapsc, sjguo
//			crtStateMemMap[mit->first] = Expr::copyExpr(result);
		}
		// 2. get corresponding solved values for current scan
		std::map<std::string, ref<Expr> > solvedMap;
		for (unsigned i = 0; i < res.size(); i++) {
			std::string &name = res[i].first;
			if (name.substr(0, 4) == "scan") {
				unsigned num = boost::lexical_cast<unsigned>(name.substr(4, 1));
				if (num == state.scan) {
					std::vector<unsigned char> &v = res[i].second;
					uint64_t value = 0;
					for (unsigned j = 0; j < v.size(); j++) {
						value |= (v[j] << (j * 8));
					}
					const MemoryObject* mo = state.symbolics[i].first;
					string symbolicName = mo->name;
					ref<Expr> solvedValue = ConstantExpr::create(value, mo->size * 8);
					solvedMap[name] = solvedValue;
				}
			}
		}
		// 3. use the solved value to simplify the symbolic expr
		std::map<std::string, ref<Expr> >::iterator mit1 = crtStateMemMap.begin();
		for (; mit1 != crtStateMemMap.end(); mit1++) {
			ref<Expr>& expr = mit1->second;
			replaceSolvedSymbolics(expr, solvedMap);
		}
		flag = this->isStateContained(state, crtStateMemMap);
		if (!flag) {
			std::cerr << "[DBG]: not visited" << std::endl;
			this->visitedStates.push_back(crtStateMemMap);
		} else {
			std::cerr << "[DBG]: visited" << std::endl;
		}
	}

	return flag;
}

void Executor::concretizePLCStateExpr(std::string key, ref<Expr> value, ref<Expr> visited) {
//	for (unsigned i = 0; i < visited->getNumKids(); i++) {
//		ref<Expr> kid = visited->getKid(i);
//		if (dyn_cast<WPRegularExpr>(kid)) {
//			const string &name = kid->getName();
//			if (key == name) {
//				visited->setKid(i, value);
//			}
//		} else {
//			concretizePLCStateExpr(key, value, kid);
//		}
//	}
}

void Executor::terminateStateOnExit(ExecutionState &state) {
	if (DEBUG) {
		std::cerr << "[DBG] now enter terminateStateOnExit in state " << &state << std::endl;
	}

	std::cerr << "-------------------------------------------------------" << std::endl;
	if (!OnlyOutputStatesCoveringNew || state.coveredNew) {
		interpreterHandler->processTestCase(state, 0, 0);
		terminateState(state, false);
	} else {
		terminateState(state, true);
	}
}

void Executor::terminateRedundantState(ExecutionState &state, bool silenced) {
	if (DEBUG) {
		std::cerr << "[DBG]: terminating redundant state: " << &state << std::endl;
	}
	terminateState(state, silenced);
}

void Executor::terminateStateOnError(ExecutionState &state, const llvm::Twine &messaget, const char *suffix, const llvm::Twine &info) {
	if (DEBUG) {
		std::cerr << "[DBG]: terminating state on error: " << &state << std::endl;
		std::cerr << "[DBG]: error message: " << messaget.str() << std::endl;
	}

	std::string message = messaget.str();
	static std::set<std::pair<Instruction*, std::string> > emittedErrors;

	assert(state.crtThreadIt != state.threads.end());

	const InstructionInfo &ii = *state.prevPC()->info;

	if (this->DEBUG) {
		printTrace(state);
	}

	if (isTraceFeasible(state)) {
		if (this->DEBUG) std::cerr << "[DBG]: error trace feasible" << std::endl;
	} else {
		if (this->DEBUG) std::cerr << "[DBG]: error trace infeasible" << std::endl;
		terminateRedundantState(state);
		return;
	}

	if (EmitAllErrors || emittedErrors.insert(std::make_pair(state.prevPC()->inst, message)).second) {
		if (ii.file != "") {
			LOG(ERROR) << ii.file.c_str() << ":" << ii.line << ": " << message.c_str();
		} else {
			LOG(ERROR) << message.c_str();
		}
		if (!EmitAllErrors)
		LOG(INFO) << "Now ignoring this error at this location";

		std::ostringstream msg;
		msg << "Error: " << message << "\n";
		if (ii.file != "") {
			msg << "File: " << ii.file << "\n";
			msg << "Line: " << ii.line << "\n";
		}
		msg << "Stack: \n";
		state.getStackTrace().dump(msg);
		std::string info_str = info.str();
		if (info_str != "") msg << "Info: \n" << info_str;
		interpreterHandler->processTestCase(state, msg.str().c_str(), suffix);
		terminateState(state, false);
	} else {
		terminateState(state, true);
	}
}

void Executor::destroyStates() {
	if (DumpStatesOnHalt && !states.empty()) {
		std::cerr << "KLEE: halting execution, dumping remaining states\n";
		for (std::set<ExecutionState*>::iterator it = states.begin(), ie = states.end(); it != ie; ++it) {
			ExecutionState &state = **it;
			stepInstruction(state); // keep stats rolling
			terminateStateEarly(state, "execution halting");
		}
		updateStates(0);
	}

	delete processTree;
	processTree = 0;

	// hack to clear memory objects
	delete memory;
	memory = new MemoryManager();

	globalObjects.clear();
	globalAddresses.clear();

	if (statsTracker) statsTracker->done();

	if (theMMap) {
		munmap(theMMap, theMMapSize);
		theMMap = 0;
	}
}

void Executor::destroyState(ExecutionState *state) {
	terminateStateEarly(*state, "cancelled");
}

void Executor::addConstraint(ExecutionState &state, klee::ref<Expr> condition) {
	if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(condition))) {
		assert(CE->isTrue() && "attempt to add invalid constraint");
		return;
	}

	state.addConstraint(condition);
	if (ivcEnabled) doImpliedValueConcretization(state, condition, ConstantExpr::alloc(1, Expr::Bool));
}

void Executor::executeEvent(ExecutionState &state, unsigned int type, long int value) {
	if (type == KLEE_EVENT_BREAKPOINT && value == KLEE_BRK_START_TRACING) {
		fireControlFlowEvent(&state, ::cloud9::worker::CHANGE_SHADOW_STATE);
	}

	fireEvent(&state, type, value);
}

void Executor::executeFork(ExecutionState &state, KInstruction *ki, uint64_t reason) {
	int forkClass = reason & 0xFF;
	bool canFork = false;
	// Check to see if we really should fork
	if (forkClass == KLEE_FORK_DEFAULT || fireStateBranching(&state, getForkTag(state, forkClass))) {
		canFork = true;
	}

	if (canFork) {
		StatePair sp = fork(state, reason);

		// Return 0 in the original
		bindLocal(ki, *sp.second, ConstantExpr::create(0, getWidthForLLVMType(ki->inst->getType())));

		// Return 1 otherwise
		bindLocal(ki, *sp.first, ConstantExpr::create(1, getWidthForLLVMType(ki->inst->getType())));
	} else {
		bindLocal(ki, state, ConstantExpr::create(0, getWidthForLLVMType(ki->inst->getType())));
	}
}

static double startTime = klee::util::getUserTime();

void Executor::stepInState(ExecutionState *state) {
	activeState = state;

	fireControlFlowEvent(state, ::cloud9::worker::STEP);

	VLOG(5)
			<< "Executing instruction: " << state->pc()->info->assemblyLine << " through state " << state << " Src: " << state->pc()->info->file
					<< " Line: " << state->pc()->info->line;

	resetTimers();

	KInstruction *ki = state->pc();

	stepInstruction(*state);
	executeInstruction(*state, ki);
	state->stateTime++; // Each instruction takes one unit of time

	processTimers(state);

	double currentTime = klee::util::getUserTime();

	if (MaxMemory) {
		if (currentTime - startTime > 15.0) {
			startTime = currentTime;
//		if ((stats::instructions & 0xFFFF) == 0) {
			// We need to avoid calling GetMallocUsage() often because it
			// is O(elts on freelist). This is really bad since we start
			// to pummel the freelist once we hit the memory cap.
			unsigned mbs = sys::Process::GetTotalMemoryUsage() >> 20;

			if (mbs > MaxMemory) {
				if (mbs > MaxMemory + 100) {
					// just guess at how many to kill
					unsigned numStates = states.size();
					unsigned toKill = std::max(1U, numStates - numStates * MaxMemory / mbs);

					if (MaxMemoryInhibit)
					LOG(WARNING) << "Killing " << toKill << " states (over memory cap)";

					std::cerr << "to kill: " << toKill << std::endl;
					unsigned num = 0;
					std::vector<ExecutionState*> arr(states.begin(), states.end());
					for (unsigned i = 0, N = arr.size(); N && i < toKill; ++i, --N) {
						unsigned idx = rand() % N;

						// Make two pulls to try and not hit a state that
						// covered new code.
//						if (arr[idx]->coveredNew) idx = rand() % N;
						if (arr[idx]->reserved_for_POR) continue;

						std::swap(arr[idx], arr[N - 1]);

						fireOutOfResources(arr[N - 1]);
						terminateStateEarly(*arr[N - 1], "memory limit");
						num++;
					}
					std::cerr << "killed: " << num << std::endl;
				}
				atMemoryLimit = true;
			} else {
				atMemoryLimit = false;
			}
		}
	}

	updateStates(state);

	activeState = NULL;
}

void Executor::run(ExecutionState &initialState) {
	searcher = initSearcher(NULL);

	searcher->update(0, states, std::set<ExecutionState*>());

	while (!states.empty() && !haltExecution) {
		ExecutionState &state = searcher->selectState();

		stepInState(&state);
	}

	delete searcher;
	searcher = 0;

	if (DumpStatesOnHalt && !states.empty()) {
		std::cerr << "KLEE: halting execution, dumping remaining states\n";
		for (std::set<ExecutionState*>::iterator it = states.begin(), ie = states.end(); it != ie; ++it) {
			ExecutionState &state = **it;
			stepInstruction(state); // keep stats rolling
			terminateStateEarly(state, "execution halting");
		}
		updateStates(0);
	}
}

//void Executor::concretizeWP(ExecutionState &state, klee::ref<Expr> &wp) {
//	for (unsigned i = 0; i < wp->getNumKids(); i++) {
//		klee::ref<Expr> kid = wp->getKid(i);
//		if (dyn_cast<WPRegularExpr>(kid)) {
//			uint64_t thread_id = kid->getThreadID();
//			assert(!kid->getName().empty());
//			const string &name = kid->getName();
//			bool found = false;
//
//			// Let's search the memory for global variables
//			// e.g, @a, its address is fixed during root state initialization
//			std::map<string, ref<Expr> > &map = this->getModule()->constantNameValueMap;
//			if (map.find(name) != map.end()) {
//				found = true;
//				ref<Expr> constAddr = map[name];
//				wp->setKid(i, constAddr);
//			}
//
//			if (found) continue;
//
//			if (name.substr(0, 7) == "wp_inst") {
//				// Not found in Constant Table, let's search the registers of current thread
//				thread_uid_t uid = std::make_pair(thread_id, state.crtProcess().pid);
//				std::vector<StackFrame> &stack = state.threads.find(uid)->second.stack;
//				int index = stack.size() - 1;
//				for (; index >= 0; index--) {
//					KFunction *kf = stack[index].kf;
//					unsigned numInsts = kf->numInstructions;
//					for (unsigned j = 0; j < numInsts; j++) {
//						KInstruction *ki = kf->instructions[j];
//						Instruction *inst = ki->inst;
//						if (inst->hasName() && inst->getName().str().compare(name) == 0) {
//							found = true;
//							ref<Expr> regValue = stack[index].locals[ki->dest].value;
//							wp->setKid(i, regValue);
//							break;
//						}
//					}
//					if (found) {
//						break;
//					}
//				}
//			}
//
//			if (found) continue;
//
//			// Not found in registers, let's search the argument list of current thread
//			if (name.substr(0, 6) == "wp_arg") {
//				thread_uid_t uid = std::make_pair(thread_id, state.crtProcess().pid);
//				std::vector<StackFrame> &stack = state.threads.find(uid)->second.stack;
//				KFunction *kf = stack.back().kf;
//				unsigned numArgs = kf->numArgs;
//				ilist_iterator<Argument> ait = kf->function->getArgumentList().begin();
//
//				for (unsigned j = 0; j < numArgs; j++) {
//					assert(ait->hasName());
//					if (ait->getName().str() == name) {
//						found = true;
//						ref<Expr> argValue = stack.back().locals[kf->getArgRegister(j)].value;
//						wp->setKid(i, argValue);
//						break;
//					}
//					ait++;
//				}
//			}
//
//			if (found) continue;
//
//			// Not found in register, now search the main memory
////			std::vector<MemoryObject*> &mos = state.executor->memory->getMemoryObjects();
////			for (std::vector<MemoryObject*>::iterator vit = mos.begin(); vit != mos.end(); vit++) {
////				MemoryObject *mo = *vit;
////				if (mo->getName() == name) {
////					if (name == "wp_inst56")
////					assert(false);
////					ref<Expr> addr = ConstantExpr::createPointer(mo->address);
////					wp->setKid(i, addr);
////					found = true;
////					break;
////				}
////			}
//
//			if (!found) {
////				ref<Expr> newKid = wp;
////				ref<Expr> parent = wp->parent;
////				while(!parent.isNull()){
////					Expr::Kind kind = parent->getKind();
////					if(kind == Expr::And || kind == Expr::Or){
////						unsigned index = 0;
////						for(unsigned i =0;i<parent->getNumKids();i++){
////							if(parent->getKid(i) == newKid){
////								index = i;
////								break;
////							}
////						}
////						ref<Expr> constantKid = ConstantExpr::create(0, 64);
////						parent->setKid(index, constantKid);
////					}
////					else{
////						newKid = parent;
////						parent = parent->parent;
////					}
////				}
//				// Not found in concrete constant map, and registers, now just print the error information
//				std::cout << "[DBG]: value of kid is not found, kid: " << endl << kid << std::endl;
//				std::cout << "[DBG]: parent: " << endl << wp << std::endl;
//				std::cout << "[DBG]: kid thread id: " << thread_id << std::endl;
//				std::cout << "[DBG]: crt thread id: " << state.crtThread().getTid() << std::endl;
//				std::cout << "[DBG] at line " << state.prevPC()->info->assemblyLine << std::endl;
//				assert(false);
//			}
//		} else if (dyn_cast<WPLoadExpr>(kid)) {
//			assert(kid->getNumKids() == 1);
////			kid = Expr::copyExpr(kid);
//			concretizeWP(state, kid);
//			unsigned type = kid->getReadWidth() ? kid->getReadWidth() : kid->getWidth();
//			klee::ref<Expr> tmp = loadMemoryObject(state, kid->getKid(0), type);
//
//			if (tmp.isNull()) {
//				std::cout << "[DBG]: null value loaded for expr kid: " << kid << std::endl;
//				std::cout << "[DBG]: parent expr of kid: " << wp << std::endl;
//				std::cout << "[DBG] at assembly line " << state.prevPC()->info->assemblyLine << std::endl;
//				assert(false);
//			} else {
//				wp->setKid(i, tmp);
//			}
//		} else {
//			concretizeWP(state, kid);
//		}
//	}
//}

klee::ref<Expr> Executor::loadMemoryObject(ExecutionState& state, klee::ref<Expr> address, Expr::Width type) {
	unsigned bytes = Expr::getMinBytesForWidth(type);
	ObjectPair op;
	bool resolved = false;

	// fast path: single in-bounds resolution
	solver->setTimeout(stpTimeout);
	if (!state.addressSpace().resolveOne(state, solver, address, op, resolved)) {
		address = toConstant(state, address, "resolveOne failure");
		resolved = state.addressSpace().resolveOne(cast<ConstantExpr>(address), op);
	}
	solver->setTimeout(0);

	if (false) {
		std::cerr << "[DBG]: state: " << &state << std::endl;
		std::cerr << "[DBG]: type: " << type << std::endl;
		std::cerr << "[DBG]: addr in load mo: " << address << std::endl;
		std::cerr << "[DBG]: resolved: " << resolved << std::endl;
	}

	klee::ref<Expr> result;
	if (resolved) {
		const MemoryObject *mo = op.first;
		klee::ref<Expr> offset = mo->getOffsetExpr(address);

		solver->setTimeout(stpTimeout);
		bool success = solver->mustBeTrue(data::SINGLE_ADDRESS_RESOLUTION, state, mo->getBoundsCheckOffset(offset, bytes), resolved);
		solver->setTimeout(0);
		if (!success) {
			state.pc() = state.prevPC();
			terminateStateEarly(state, "query timed out");
			return NULL;
		}

		const ObjectState *os = op.second;
		result = os->read(offset, type);
	}

	if (result.isNull()) {
		const MemoryObject *mo = op.first;
		klee::ref<Expr> offset = mo->getOffsetExpr(address);
//		std::cerr << "[DBG]: offset: " << offset << std::endl;
//		std::cerr << "[DBG]: cannot find the memory object in state: " << &state << std::endl;
//		std::cerr << "[DBG]: the inst is in function: " << state.prevPC()->inst->getParent()->getParent()->getName().str() << std::endl;
//		std::cerr << "[DBG]: address: " << address << std::endl;
		assert(0);
		return result;
	} else {
		return result;
	}

	ResolutionList rl;
	solver->setTimeout(stpTimeout);
	bool incomplete = state.addressSpace().resolve(state, solver, address, rl, 0, stpTimeout);
	solver->setTimeout(0);

	// XXX there is some query wasteage here. who cares?
	ExecutionState *unbound = &state;

	for (ResolutionList::iterator i = rl.begin(), ie = rl.end(); i != ie; ++i) {
		const MemoryObject *mo = i->first;
		const ObjectState *os = i->second;
		klee::ref<Expr> inBounds = mo->getBoundsCheckPointer(address, bytes);

		StatePair branches = fork(*unbound, inBounds, true, KLEE_FORK_INTERNAL);
		ExecutionState *bound = branches.first;

		// bound can be 0 on failure or overlapped
		if (bound) {
			klee::ref<Expr> result = os->read(mo->getOffsetExpr(address), type);
		}

		unbound = branches.second;
		if (!unbound) break;
	}

	// XXX should we distinguish out of bounds and overlapped cases?
	if (unbound) {
		if (incomplete) {
			terminateStateEarly(*unbound, "query timed out (resolve)");
		} else {
			terminateStateOnError(*unbound, "memory error: out of bound pointer", "ptr.err", getAddressInfo(*unbound, address));
		}
	}
	return result;
}

}
