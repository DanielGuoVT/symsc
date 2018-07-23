/*
 * Cloud9 Parallel Symbolic Execution Engine
 *
 * Copyright (c) 2011, Dependable Systems Laboratory, EPFL
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Dependable Systems Laboratory, EPFL nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE DEPENDABLE SYSTEMS LABORATORY, EPFL BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * All contributors are listed in CLOUD9-AUTHORS file.
 *
 */

#include "cloud9/worker/CoreStrategies.h"
#include "cloud9/worker/TreeObjects.h"
#include "cloud9/worker/TreeNodeInfo.h"
#include "cloud9/worker/WorkerCommon.h"
#include "cloud9/worker/SymbolicEngine.h"
#include "cloud9/worker/JobManager.h"

#include "klee/CoreStats.h"
#include "klee/Internal/ADT/RNG.h"
#include "klee/Searcher.h"
#include "klee/Statistics.h"
#include "klee/Executor.h"
#include "klee/ExecutionState.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
///XXX: ugly, remove this dependency
#include "../../Core/CallPathManager.h"
#include "../../Core/StatsTracker.h"

using namespace klee;

namespace cloud9 {

namespace worker {

static ExecutionJob *selectRandomPathJob(WorkerTree *tree) {
	WorkerTree::Node *node = tree->selectRandomLeaf(WORKER_LAYER_JOBS, tree->getRoot(), theRNG);
	ExecutionJob *job = (**node).getJob();

	assert(job != NULL || node == tree->getRoot());

	return job;
}

static SymbolicState *selectRandomPathState(WorkerTree *tree) {
	WorkerTree::Node *node = tree->selectRandomLeaf(WORKER_LAYER_STATES, tree->getRoot(), theRNG, WORKER_LAYER_JOBS);
	SymbolicState *state = (**node).getSymbolicState();

	return state;
}

ExecutionJob *BasicStrategy::selectJob(WorkerTree *tree, SymbolicState* state) {
	WorkerTree::Node *node = state->getNode().get();
	if (!node->layerExists(WORKER_LAYER_JOBS)) {
		dumpSymbolicTree(node);
	}

	assert(node->layerExists(WORKER_LAYER_JOBS));

	// Take the easy way first
	if ((**node).getJob() != NULL) {
		return (**node).getJob();
	}

	// OK, so it's an inner state - select one job at random for replay
	WorkerTree::Node *jobNode = tree->selectRandomLeaf(WORKER_LAYER_JOBS, node, theRNG);
	ExecutionJob *job = (**jobNode).getJob();

	return job;
}

void BasicStrategy::dumpSymbolicTree(WorkerTree::Node *highlight) {
	jobManager->dumpSymbolicTree(NULL, WorkerNodeDecorator(highlight));
}

////////////////////////////////////////////////////////////////////////////////
// Random-Job-From-State Strategy
////////////////////////////////////////////////////////////////////////////////

void RandomJobFromStateStrategy::onStateActivated(SymbolicState *state) {
	stateStrat->onStateActivated(state);
}

void RandomJobFromStateStrategy::onStateUpdated(SymbolicState *state, WorkerTree::Node *oldNode) {
	stateStrat->onStateUpdated(state, oldNode);
}

void RandomJobFromStateStrategy::onStateDeactivated(SymbolicState *state) {
	stateStrat->onStateDeactivated(state);
}

void RandomJobFromStateStrategy::onStateStepped(SymbolicState *state) {
	stateStrat->onStateStepped(state);
}

ExecutionJob* RandomJobFromStateStrategy::onNextJobSelection() {
	SymbolicState *state = stateStrat->onNextStateSelection();

	if (!state) return NULL;

	return selectJob(tree, state);
}

ExecutionJob* RandomJobFromStateStrategy::onNextJobSelectionEx(bool &canBatch, uint32_t &batchDest) {
	SymbolicState *state = stateStrat->onNextStateSelectionEx(canBatch, batchDest);

	if (!state) return NULL;

	return selectJob(tree, state);
}

////////////////////////////////////////////////////////////////////////////////
// Random Strategy
////////////////////////////////////////////////////////////////////////////////

SymbolicState* RandomStrategy::onNextStateSelection() {
	if (states.empty()) {
		return NULL;
	}

	int index = klee::theRNG.getInt32() % states.size();
	return states[index];
}

void RandomStrategy::onStateActivated(SymbolicState *state) {
	indices[state] = states.size();
	states.push_back(state);
}

void RandomStrategy::onStateDeactivated(SymbolicState *state) {
	unsigned i = indices[state];
	states[i] = states.back();
	indices[states[i]] = i;
	states.pop_back();
}

////////////////////////////////////////////////////////////////////////////////
// DFS State Selection Strategy, sjguo
////////////////////////////////////////////////////////////////////////////////
SymbolicState* DFSStrategy::onNextStateSelection() {
	if (states.empty()) {
		return NULL;
	}

	static unsigned max = 0;
	if (states.size() > max) max = states.size();

	unsigned index = states.size() - 1;
//	std::cerr << states.size() << std::endl;

	ExecutionState *root = states[0]->getKleeState();
	Executor *executor = root->executor;

	// if POR is enabled
	if (executor->UseDPOR) {
		for (int i = index; i > 0; i--) {
			if (!states[i]->getKleeState()->reserved_for_POR) {
				index = i;
				break;
			}
		}

		if (index == 0) {
			index = states.size() - 1;
		}
	}

	if (executor->DEBUG) {
		std::cerr << "[DBG]: selected state for execute: " << states[index]->getKleeState() << std::endl;
	}

	return states[index];
}

void DFSStrategy::onStateActivated(SymbolicState *state) {
	indices[state] = states.size();
	states.push_back(state);
}

void DFSStrategy::onStateDeactivated(SymbolicState *state) {
	unsigned i = indices[state];
	int size = states.size();
	for (int index = i; index < size - 1; index++) {
		states[index] = states[index + 1];
		indices[states[index]] = index;
	}
	states.pop_back();
}

////////////////////////////////////////////////////////////////////////////////
// State Selection Strategy for PLC programs, sjguo
////////////////////////////////////////////////////////////////////////////////
SymbolicState* PLCFirstStrategy::onNextStateSelection() {
	if (states.empty()) {
		return NULL;
	}

//	std::cerr << states.size() << std::endl;
	ExecutionState *root = states[0]->getKleeState();
	Executor *executor = root->executor;

	unsigned size = states.size();
	unsigned index = 0;

	for (unsigned i = size - 1; i > 0; i--) {
		ExecutionState *state = states[i]->getKleeState();
		if (state->isRedundant) continue;
		if (state->reserved_for_POR) continue;
		if (states[i]->getKleeState()->scan == executor->crtScan) {
			index = i;
			break;
		}
	}

	if (index == 0) {
		// check if some states can be terminated
		if (size > 1) {
			executor->crtScan++;
			for (unsigned i = size - 1; i > 0; i--) {
				ExecutionState *state = states[i]->getKleeState();
				if (state->isRedundant) continue;
				if (state->reserved_for_POR) continue;
				if (states[i]->getKleeState()->scan == executor->crtScan) {
					index = i;
					break;
				}
			}
		}
	}

	if (executor->DEBUG) {
		std::cerr << "[DBG]: selected state for execute: " << states[index]->getKleeState() << std::endl;
	}
	return states[index];
}

void PLCFirstStrategy::onStateActivated(SymbolicState *state) {
	indices[state] = states.size();
	states.push_back(state);
}

void PLCFirstStrategy::onStateDeactivated(SymbolicState *state) {
	unsigned i = indices[state];
	int size = states.size();
	for (int index = i; index < size - 1; index++) {
		states[index] = states[index + 1];
		indices[states[index]] = index;
	}
	states.pop_back();
}

////////////////////////////////////////////////////////////////////////////////
// Random Path Strategy
////////////////////////////////////////////////////////////////////////////////

SymbolicState* RandomPathStrategy::onNextStateSelection() {
	SymbolicState *state = selectRandomPathState(tree);

	return state;
}

////////////////////////////////////////////////////////////////////////////////
// Clustered Random Path Strategy
////////////////////////////////////////////////////////////////////////////////

SymbolicState* ClusteredRandomPathStrategy::onNextStateSelection() {
	if (states.empty()) return NULL;

	std::vector<WorkerTree::Node*> nodes;
	nodes.reserve(states.size());

// TODO: Make this more efficient by implementing a custom iterator
	for (state_set_t::iterator it = states.begin(); it != states.end(); it++) {
		SymbolicState *state = *it;
		nodes.push_back(state->getNode().get());
	}

	WorkerTree::Node *selNode = tree->selectRandomLeaf(WORKER_LAYER_STATES, tree->getRoot(), theRNG, nodes.begin(), nodes.end());

	SymbolicState *state = (**selNode).getSymbolicState();

	assert(state != NULL);

	return state;
}

void ClusteredRandomPathStrategy::onStateActivated(SymbolicState *state) {
	states.insert(state);
}

void ClusteredRandomPathStrategy::onStateDeactivated(SymbolicState *state) {
	states.erase(state);
}

////////////////////////////////////////////////////////////////////////////////
// Limited Flow Strategy
////////////////////////////////////////////////////////////////////////////////

void LimitedFlowStrategy::onStateActivated(SymbolicState *state) {
	underStrat->onStateActivated(state);
}

void LimitedFlowStrategy::onStateDeactivated(SymbolicState *state) {
	underStrat->onStateDeactivated(state);

	if (activeStates.count(state) > 0) {
		workingStrat->onStateDeactivated(state);
		activeStates.erase(state);
	}
}

SymbolicState* LimitedFlowStrategy::onNextStateSelection() {
// First, ask the underlying strategy...
	SymbolicState *candidate = underStrat->onNextStateSelection();

	if (!candidate) {
		assert(activeStates.size() == 0);
		return NULL;
	}

	if (activeStates.count(candidate) > 0) return candidate;

	if (activeStates.size() < maxCount) {
		activeStates.insert(candidate);
		workingStrat->onStateActivated(candidate);
		return candidate;
	}

	SymbolicState *state = workingStrat->onNextStateSelection();
	assert(state != NULL);

	return state;
}

////////////////////////////////////////////////////////////////////////////////
// Klee Imported Strategy
////////////////////////////////////////////////////////////////////////////////

KleeStrategy::KleeStrategy(WorkerTree *_tree) :
		tree(_tree), searcher(NULL) {

}

KleeStrategy::KleeStrategy(WorkerTree *_tree, klee::Searcher *_searcher) :
		tree(_tree), searcher(_searcher) {

}

KleeStrategy::~KleeStrategy() {

}

void KleeStrategy::onStateActivated(SymbolicState *state) {
	std::set<klee::ExecutionState*> added;
	added.insert(&(**state));

	searcher->update(NULL, added, std::set<klee::ExecutionState*>());
}

void KleeStrategy::onStateUpdated(SymbolicState *state, WorkerTree::Node *oldNode) {
	searcher->update(&(**state), std::set<klee::ExecutionState*>(), std::set<klee::ExecutionState*>());
}

void KleeStrategy::onStateDeactivated(SymbolicState *state) {
	std::set<klee::ExecutionState*> removed;
	removed.insert(&(**state));

	searcher->update(NULL, std::set<klee::ExecutionState*>(), removed);
}

SymbolicState* KleeStrategy::onNextStateSelection() {
	if (searcher->empty()) return NULL;

	klee::ExecutionState &kState = searcher->selectState();
	SymbolicState *state = kState.getCloud9State();

	return state;
}

////////////////////////////////////////////////////////////////////////////////
// Weighted Random Strategy
////////////////////////////////////////////////////////////////////////////////

WeightedRandomStrategy::WeightedRandomStrategy(WeightType _type, WorkerTree *_tree, SymbolicEngine *_engine) :
		KleeStrategy(_tree) {

	klee::Executor *executor = dynamic_cast<klee::Executor*>(_engine); // XXX I should be ashamed of this
	searcher = new WeightedRandomSearcher(*executor, static_cast<klee::WeightedRandomSearcher::WeightType>(_type)); // XXX This is truly ugly

}

WeightedRandomStrategy::~WeightedRandomStrategy() {
	delete searcher;
}

}

}

