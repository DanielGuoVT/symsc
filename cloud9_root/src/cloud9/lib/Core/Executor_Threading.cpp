//===-- Executor_Threading.cpp --------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Executor.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "Memory.h"

#include "StatsTracker.h"
#include "TimingSolver.h"

#include "llvm/Function.h"
#include "llvm/Support/CommandLine.h"

#include <glog/logging.h>

#include <sstream>

using namespace llvm;

namespace {

cl::opt<bool> DebugSchedulingHistory("debug-sched-history", cl::init(false));
cl::opt<bool> ForkOnSchedule("fork-on-schedule", cl::desc("fork when various schedules are possible (defaul=disabled)"), cl::init(false));
cl::opt<unsigned int> MaxPreemptions("scheduler-preemption-bound", cl::desc("scheduler preemption bound (default=0)"), cl::init(0));
cl::opt<bool> ForkOnInterleaving("fork-on-interleaving", cl::desc("fork when various interleavings are possible (defaul=disabled)"), cl::init(false));
}

namespace klee {

bool Executor::schedule(ExecutionState &state, bool yield, int specialReason) {
	if (DEBUG) {
		std::cerr << "----------------------Enter Executor::schedule--------------------------------------------" << std::endl;
		std::cerr << "[DBG]: state:" << &state << ", thread size: " << state.threads.size() << std::endl;
	}

	// First, try to check if there exists deadlock situation.
	int enabledCount = 0;
	int enabledWorkerThreads = 0;
	for (ExecutionState::threads_ty::iterator tit = state.threads.begin(); tit != state.threads.end(); tit++) {
		if (tit->second.enabled) {
			enabledCount++;
			if (tit->second.getTid() != 0) {
				enabledWorkerThreads++;
			}
		}
	}
	if (enabledCount == 0) {
		ExecutionState::threads_ty::iterator tit = state.threads.begin();
		for (; tit != state.threads.end(); tit++) {
			std::cerr << "thread unique id: " << tit->second.uniqueID << " ,enabled: " << tit->second.enabled << std::endl;
		}
		terminateStateOnError(state, " ******** hang (possible deadlock?)", "user.err");
		return false;
	} else {
		if (DEBUG) std::cerr << "[DBG]: enabledCount " << enabledCount << std::endl;
		if (DEBUG) std::cerr << "[DBG]: enabledWorkerThreads: " << enabledWorkerThreads << std::endl;
	}

	bool forkSchedule = false;
	ExecutionState::threads_ty::iterator originalIt = state.crtThreadIt;
	ExecutionState::threads_ty::iterator candidateIt = state.crtThreadIt;
	Thread &crtThread = state.crtThread();

	if (!crtThread.enabled) {
		while (true) {
			if (DEBUG) std::cerr << "[DBG]: Thread not enabled: " << candidateIt->second.uniqueID << std::endl;
			candidateIt = state.nextThread(candidateIt);
			if (DEBUG) std::cerr << "[DBG]: checking next thread: " << candidateIt->second.uniqueID << std::endl;
			if (candidateIt->second.enabled) {
				if (DEBUG) std::cerr << "[DBG]: find active thread: " << candidateIt->second.uniqueID << std::endl;
				break;
			}
		}

		// if crtThread is a working thread
		if (crtThread.getTid() != 0) {
			crtThread.reachInterleavingPoint = true;
		}

		if (ForkOnSchedule) {
			forkSchedule = true;
			// Do not fork states if (1) context switch is from main thread; (2) current thread is sleeping;
			// (3) not all threads reach ipp; (4) only one worker thread is enabled;
			if (crtThread.getTid() == 0 || crtThread.sleeping || enabledWorkerThreads <= 1 || !state.allReachInterleavingPoint()) {
				forkSchedule = false;
			}
		}
	}

	if (crtThread.enabled && yield) {
		ExecutionState::threads_ty::iterator tit;

		if (DEBUG) std::cerr << "[DBG]: crtThread " << crtThread.uniqueID << " reaches iPP, try to find candidateIt." << std::endl;

		while (true) {
			// Next eligible thread should not be main thread, should not reach iPP, should be active
			if (candidateIt->second.enabled && candidateIt->first.first != 0 && !candidateIt->second.reachInterleavingPoint) {
				if (DEBUG) std::cerr << "[DBG]: find eligible thread: " << candidateIt->second.uniqueID << std::endl;
				break;
			}

			if (DEBUG)
				std::cerr << "[DBG]: thread not qualified: " << candidateIt->second.uniqueID << ", enabled: " << candidateIt->second.enabled
						<< std::endl;
			candidateIt = state.nextThread(candidateIt);
			if (DEBUG) std::cerr << "[DBG]: check next thread: " << candidateIt->second.uniqueID << std::endl;

			// loop until it==oldIt
			if (candidateIt == originalIt) {
				if (DEBUG) {
					std::cerr << "[DBG]: active working threads: " << enabledWorkerThreads << std::endl;
					std::cerr << "[DBG]: it == oldIt == " << candidateIt->second.uniqueID << std::endl;
					std::cerr << "[DBG]: all threads have reached i-PP Node." << std::endl;
				}
				break;
			}
		}

		if (ForkOnSchedule) {
			forkSchedule = true;
			if (crtThread.getTid() == 0 || enabledWorkerThreads == 1 || !state.allReachInterleavingPoint()) {
				forkSchedule = false;
			} else {
				if (fixed_exec && enabledWorkerThreads > 1 && crtThread.getTid() == 1) {
					ExecutionState::threads_ty::iterator t1 = state.crtThreadIt;
					ExecutionState::threads_ty::iterator t2 = state.threads.find(std::make_pair(2, state.crtProcess().pid));
					assert(t1 != state.threads.end());
					assert(t2 != state.threads.end());

					MemoryObject *mo = sboxGlobalMOs[t1->second.prevPC->sbox];
					ConstantExpr * CE = dyn_cast<ConstantExpr>(eval(t2->second.prevPC, 0, state).value);
					assert(CE);
					unsigned t2set = getCacheSet(CE->getZExtValue());
					fixed_addr = CE->getZExtValue();
					if (t2set < getCacheSet(mo->address) || t2set > getCacheSet(mo->address + mo->size)) {
						forkSchedule = false;
						std::cerr << "[DBG] t1 cache set: " << getCacheSet(mo->address) << " to " << getCacheSet(mo->address + mo->size) << ", sbox: "
								<< t1->second.prevPC->sbox << std::endl;
						std::cerr << "[DBG] t2 cache set: " << getCacheSet(CE->getZExtValue()) << ", skip interleaving." << std::endl;
					} else {
						std::cerr << "[DBG] t1 cache set: " << getCacheSet(mo->address) << " to " << getCacheSet(mo->address + mo->size) << ", sbox: "
								<< t1->second.prevPC->sbox << std::endl;
						std::cerr << "[DBG] t2 cache set: " << getCacheSet(CE->getZExtValue()) << ", start interleaving." << std::endl;
					}
				}
			}
		}
	}
	if (DEBUG) {
		std::cerr << "[DBG]: Context Switch: -- old thread: " << crtThread.uniqueID << " enabled: " << crtThread.enabled << std::endl;
		std::cerr << "[DBG]: Context Switch: -- new thread: " << candidateIt->second.uniqueID << " enabled: " << candidateIt->second.enabled
				<< std::endl;
		std::cerr << "[DBG]: Context Switch: -- forkSchedule: " << forkSchedule << std::endl;
	}

	state.scheduleNext(candidateIt);

	if (forkSchedule) {
		if (UseDPOR) {
			assert(enabledWorkerThreads > 1);
			if (specialReason == ExecutionState::load || specialReason == ExecutionState::store) {
				ExecutionState *lastState = &state;
				ForkClass forkClass = KLEE_FORK_POR;
				StatePair sp = fork(*lastState, forkClass);
				sp.first->reserved_for_POR = true;

				// terminate originalIt after state-forking if originalIt was disabled in the old state.
				if (!originalIt->second.enabled && !originalIt->second.sleeping && originalIt->second.getTid() != 0) {
					Process &proc = sp.first->processes.find(originalIt->second.getPid())->second;
					proc.threads.erase(originalIt->second.tuid);
					ExecutionState::threads_ty &threads = sp.first->threads;
					ExecutionState::threads_ty::iterator tit = threads.begin();
					for (; tit != threads.end(); tit++) {
						if (tit->first == originalIt->first) {
							threads.erase(tit++);
							break;
						}
					}
				}

				std::string ipp_id = state.ipp_list.back();
				if (DEBUG) std::cerr << "[DBG]: new state backed up for dpor: " << sp.first << std::endl;
				std::string prefix = generatePrefix(state);
				POR_states[prefix] = sp.first;
			}
		} else {
			// Regular state forking
			ExecutionState *lastState = &state;
			ForkClass forkClass = KLEE_FORK_SCHEDULE;
			ExecutionState::threads_ty::iterator tit = state.threads.begin();
			for (; tit != state.threads.end(); tit++) {
				if (!tit->second.enabled) continue;
				if (tit->first.first == 0) continue;
				if (tit == candidateIt) continue;
				if (tit == originalIt) continue;

				StatePair sp = fork(*lastState, forkClass);
				sp.first->scheduleNext(sp.first->threads.find(tit->first));

				// terminate originalIt after state-forking if originalIt was disabled in the old state.
				if (!originalIt->second.enabled && !originalIt->second.sleeping && originalIt->second.getTid() != 0) {
					Process &proc = sp.first->processes.find(originalIt->second.getPid())->second;
					proc.threads.erase(originalIt->second.tuid);
					ExecutionState::threads_ty &threads = sp.first->threads;
					ExecutionState::threads_ty::iterator tit = threads.begin();
					for (; tit != threads.end(); tit++) {
						if (tit->first == originalIt->first) {
							threads.erase(tit++);
							break;
						}
					}
				}

				if (DEBUG) {
					std::cerr << "[DBG]: fork new state, crtIt: " << tit->first.first << " ,address: " << sp.first << std::endl;
				}
				if (forkClass == KLEE_FORK_SCHEDULE) {
					forkClass = KLEE_FORK_MULTI; // Avoid appearing like multiple schedules
				}
			}
		}
	}

	if (DEBUG) {
		std::cerr << "[DBG]: reach the end of schedule --- state: " << &state << std::endl;
		std::cerr << "----------------------Leave Executor::schedule--------------------------------------------" << std::endl;
	}

	return true;
}

void Executor::executeThreadCreate(ExecutionState &state, thread_id_t tid, klee::ref<Expr> start_function, klee::ref<Expr> arg) {
	VLOG(1) << "Creating thread...";

	KFunction *kf = resolveFunction(start_function);
	assert(kf && "cannot resolve thread start function");

	Thread &t = state.createThread(tid, kf);

	bindArgumentToPthreadCreate(kf, 0, t.stack.back(), arg);

	if (statsTracker) {
		statsTracker->framePushed(&t.stack.back(), 0);
	}
}

// Daniel, set the thread priority
void Executor::executeThreadPriority(ExecutionState &state, thread_id_t tid, thread_priority_t priority) {
	VLOG(1) << "Set thread priority manually...";
	ExecutionState::threads_ty::iterator tit = state.threads.find(std::make_pair(tid, state.crtProcess().pid));
	assert(tit != state.threads.end());
	tit->second.priority = priority;
}

void Executor::executeThreadExit(ExecutionState &state) {
//terminate this thread and schedule another one
	VLOG(1) << "Exiting thread...";
	if (state.threads.size() == 1) {
		LOG(INFO) << "Terminating state";
		terminateStateOnExit(state);
		return;
	}

	assert(state.threads.size() > 1);
	ExecutionState::threads_ty::iterator thrIt = state.crtThreadIt;
	thrIt->second.enabled = false;
	thrIt->second.sleeping = false;
	if (DEBUG)
		std::cerr << "[DBG]: Executor::executeThreadExit: thread " << thrIt->second.uniqueID << " disabled, call schedule(state, false)" << std::endl;
	if (!schedule(state, false)) return;
	state.terminateThread(thrIt);
}

void Executor::executeThreadNotifyOne(ExecutionState &state, wlist_id_t wlist) {
// Copy the waiting list
	std::set<thread_uid_t> wl = state.waitingLists[wlist];

	if (!ForkOnSchedule || wl.size() <= 1) {
		if (wl.size() == 0) state.waitingLists.erase(wlist);
		else state.notifyOne(wlist, *wl.begin()); // Deterministically pick the first thread in the queue

		return;
	}

	ExecutionState *lastState = &state;

	if (state.allReachInterleavingPoint()) {
		for (std::set<thread_uid_t>::iterator it = wl.begin(); it != wl.end();) {
			thread_uid_t tuid = *it++;

			if (it != wl.end()) {
				StatePair sp = fork(*lastState, KLEE_FORK_SCHEDULE);

				sp.second->notifyOne(wlist, tuid);

				lastState = sp.first;
			} else {
				lastState->notifyOne(wlist, tuid);
			}
		}
	} else {
		state.notifyOne(wlist, *wl.begin());
	}
}

void Executor::executeProcessFork(ExecutionState &state, KInstruction *ki, process_id_t pid) {

	VLOG(1) << "Forking with pid " << pid;

	Thread &pThread = state.crtThread();

	Process &child = state.forkProcess(pid);

	Thread &cThread = state.threads.find(*child.threads.begin())->second;

// Set return value in the child
	state.scheduleNext(state.threads.find(cThread.tuid));
	bindLocal(ki, state, ConstantExpr::create(0, getWidthForLLVMType(ki->inst->getType())));

// Set return value in the parent
	state.scheduleNext(state.threads.find(pThread.tuid));
	bindLocal(ki, state, ConstantExpr::create(child.pid, getWidthForLLVMType(ki->inst->getType())));
}

void Executor::executeProcessExit(ExecutionState &state) {
	if (state.processes.size() == 1) {
		terminateStateOnExit(state);
		return;
	}

	VLOG(1) << "Terminating " << state.crtProcess().threads.size() << " threads of the current process...";

	ExecutionState::processes_ty::iterator procIt = state.crtProcessIt;

// Disable all the threads of the current process
	for (std::set<thread_uid_t>::iterator it = procIt->second.threads.begin(); it != procIt->second.threads.end(); it++) {
		ExecutionState::threads_ty::iterator thrIt = state.threads.find(*it);

		if (thrIt->second.enabled) {
			// Disable any enabled thread
			thrIt->second.enabled = false;
		} else {
			// If the thread is disabled, remove it from any waiting list
			wlist_id_t wlist = thrIt->second.waitingList;

			if (wlist > 0) {
				state.waitingLists[wlist].erase(thrIt->first);
				if (state.waitingLists[wlist].size() == 0) state.waitingLists.erase(wlist);

				thrIt->second.waitingList = 0;
			}
		}
	}

	if (!schedule(state, false)) return;

	state.terminateProcess(procIt);
}

}
