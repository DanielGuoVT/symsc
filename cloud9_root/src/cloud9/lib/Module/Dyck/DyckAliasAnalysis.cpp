/*
 * DyckAliasAnalysis.cpp
 *
 *  Created on: Jun 17, 2014
 *      Author: sjguo
 */

#include "DyckGraph.h"
#include "AAAnalyzer.h"
#include "DyckAliasAnalysis.h"

#include <stdio.h>
#include <algorithm>
#include <stack>

#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"

using namespace llvm;
using namespace std;

namespace klee {

static const Function *getParent(const Value *V) {
	if (const Instruction * inst = dyn_cast<Instruction>(V))
		return inst->getParent()->getParent();

	if (const Argument * arg = dyn_cast<Argument>(V))
		return arg->getParent();

	return NULL;
}

static bool notDifferentParent(const Value *O1, const Value *O2) {

	const Function *F1 = getParent(O1);
	const Function *F2 = getParent(O2);

	return !F1 || !F2 || F1 == F2;
}

/// DyckAliasAnalysis - This is the primary alias analysis implementation.

struct DyckAliasAnalysis: public ModulePass, public AliasAnalysis {
	static char ID; // Class identification, replacement for typeinfo

	DyckAliasAnalysis() :
			ModulePass(ID) {
		dyck_graph = new DyckGraph;
	}

	~DyckAliasAnalysis() {
		delete dyck_graph;
	}

	virtual void initializePass() {
		InitializeAliasAnalysis(this);
	}

	virtual bool runOnModule(Module &M);

	virtual void getAnalysisUsage(AnalysisUsage &AU) const {
//		AliasAnalysis::getAnalysisUsage(AU);
		AU.addRequired<AliasAnalysis>();
		AU.addRequired<TargetLibraryInfo>();
//		AU.addRequired<DataLayout>();
		//AU.setPreservesAll();
	}

	virtual AliasResult alias(const Location &LocA, const Location &LocB) {
		if (notDifferentParent(LocA.Ptr, LocB.Ptr)) {
			AliasResult ret = AliasAnalysis::alias(LocA, LocB);
			if (ret == NoAlias) {
				return ret;
			}

			if (ret == MustAlias) {
				return ret;
			}

			if (ret == PartialAlias) {
				return ret;
			}
		}

		pair<DyckVertex*, bool> retpair = dyck_graph->retrieveDyckVertex(const_cast<Value*>(LocA.Ptr));
		DyckVertex * VA = retpair.first->getRepresentative();

		retpair = dyck_graph->retrieveDyckVertex(const_cast<Value*>(LocB.Ptr));
		DyckVertex * VB = retpair.first->getRepresentative();

		if (VA == VB) {
			return MayAlias;
		} else {
			if (isPartialAlias(VA, VB)) {
				return PartialAlias;
			} else {
				return NoAlias;
			}
		}
	}

//	AliasResult alias(const Value *V1, uint64_t V1Size, const Value *V2, uint64_t V2Size) {
//		return alias(Location(V1, V1Size), Location(V2, V2Size));
//	}
//
//	AliasResult alias(const Value *V1, const Value *V2) {
//		return alias(V1, UnknownSize, V2, UnknownSize);
//	}

	virtual ModRefResult getModRefInfo(ImmutableCallSite CS, const Location &Loc) {
		return AliasAnalysis::getModRefInfo(CS, Loc);
	}

	virtual ModRefResult getModRefInfo(ImmutableCallSite CS1, ImmutableCallSite CS2) {
		// The AliasAnalysis base class has some smarts, lets use them.
		return AliasAnalysis::getModRefInfo(CS1, CS2);
	}

	/// pointsToConstantMemory - Chase pointers until we find a (constant
	/// global) or not.
	virtual bool pointsToConstantMemory(const Location &Loc, bool OrLocal) {
		return AliasAnalysis::pointsToConstantMemory(Loc, OrLocal);
	}

	/// getModRefBehavior - Return the behavior when calling the given
	/// call site.
	virtual ModRefBehavior getModRefBehavior(ImmutableCallSite CS) {
		return AliasAnalysis::getModRefBehavior(CS);
	}

	/// getModRefBehavior - Return the behavior when calling the given function.
	/// For use when the call site is not known.

	virtual ModRefBehavior getModRefBehavior(const Function *F) {
		return AliasAnalysis::getModRefBehavior(F);
	}

	/// getAdjustedAnalysisPointer - This method is used when a pass implements
	/// an analysis interface through multiple inheritance.  If needed, it
	/// should override this to adjust the this pointer as needed for the
	/// specified pass info.
	virtual void *getAdjustedAnalysisPointer(const void *ID) {
		if (ID == &AliasAnalysis::ID)
			return (AliasAnalysis*) this;
		return this;
	}

private:
	DyckGraph* dyck_graph;

private:
	bool isPartialAlias(DyckVertex *v1, DyckVertex *v2);
	void fromDyckVertexToValue(set<DyckVertex*>& from, set<Value*>& to);
public:
	void getEscapedPointersTo(set<DyckVertex*>* ret, Function * func); // escaped to 'func'
	void getEscapedPointersFrom(set<DyckVertex*>* ret, Value * from); // escaped from 'from'
};

// Register this pass...
char DyckAliasAnalysis::ID = 0;
RegisterPass<DyckAliasAnalysis> X("dyckaa", "Alias Analysis based on Qirun's PLDI 2013 paper", false, true);
RegisterAnalysisGroup<AliasAnalysis> Y(X);

bool DyckAliasAnalysis::isPartialAlias(DyckVertex *v1, DyckVertex *v2) {
	if (v1 == NULL || v2 == NULL)
		return false;

	v1 = v1->getRepresentative();
	v2 = v2->getRepresentative();

	if (v1 == v2)
		return false;

	set<DyckVertex*> visited;
	stack<DyckVertex*> workStack;
	workStack.push(v1);

	while (!workStack.empty()) {
		DyckVertex* top = workStack.top();
		workStack.pop();

		// have visited
		if (visited.find(top) != visited.end()) {
			continue;
		}

		if (top == v2) {
			return true;
		}

		visited.insert(top);

		{ // push out tars
			set<void*>& outlabels = top->getOutLabels();
			set<void*>::iterator olIt = outlabels.begin();
			while (olIt != outlabels.end()) {
				long labelValue = (long) (*olIt);
				if (labelValue >= 0) { /// address offset; @FIXME
					set<DyckVertex*>* tars = top->getOutVertices(*olIt);

					set<DyckVertex*>::iterator tit = tars->begin();
					while (tit != tars->end()) {
						// if it has not been visited
						if (visited.find(*tit) == visited.end()) {
							workStack.push(*tit);
						}
						tit++;
					}

				}
				olIt++;
			}
		}

		{ // push in srcs
			set<void*>& inlabels = top->getInLabels();
			set<void*>::iterator ilIt = inlabels.begin();
			while (ilIt != inlabels.end()) {
				long labelValue = (long) (*ilIt);
				if (labelValue >= 0) { /// address offset; @FIXME
					set<DyckVertex*>* srcs = top->getInVertices(*ilIt);

					set<DyckVertex*>::iterator sit = srcs->begin();
					while (sit != srcs->end()) {
						// if it has not been visited
						if (visited.find(*sit) == visited.end()) {
							workStack.push(*sit);
						}
						sit++;
					}

				}
				ilIt++;
			}
		}

	}

	return false;
}

void DyckAliasAnalysis::fromDyckVertexToValue(set<DyckVertex*>& from, set<Value*>& to) {
	set<DyckVertex*>::iterator svsIt = from.begin();
	while (svsIt != from.end()) {
		Value * val = (Value*) ((*svsIt)->getValue());
		if (val == NULL) {
			set<DyckVertex*>* eset = (*svsIt)->getEquivalentSet();
			set<DyckVertex*>::iterator eit = eset->begin();
			while (eit != eset->end()) {
				val = (Value*) ((*eit)->getValue());
				if (val != NULL) {
					break;
				}
				eit++;
			}
		}

		if (val != NULL) {
			// If A is a partial alias of B, A will not be put in ret, because
			// we will consider them as a pair of may alias during instrumentation.
			bool add = true;
			set<Value*>::iterator tempIt = to.begin();
			while (tempIt != to.end()) {
				Value * existed = *tempIt;

				if (!this->isNoAlias(Location(existed), Location(val))) {
					add = false;
					break;
				}

				tempIt++;
			}
			if (add) {
				to.insert(val);
			}
		}

		svsIt++;
	}
}

void DyckAliasAnalysis::getEscapedPointersFrom(set<DyckVertex*>* ret, Value * from) {
	if (ret == NULL || from == NULL) {
		errs() << "Warning in getEscapingPointers: ret or from are null!\n";
		return;
	}

	set<DyckVertex*> visited;
	stack<DyckVertex*> workStack;

	workStack.push(dyck_graph->retrieveDyckVertex(from).first->getRepresentative());

	while (!workStack.empty()) {
		DyckVertex* top = workStack.top();
		workStack.pop();

		// have visited
		if (visited.find(top) != visited.end()) {
			continue;
		}

		visited.insert(top);

		set<DyckVertex*> tars;
		top->getOutVertices(&tars);
		set<DyckVertex*>::iterator tit = tars.begin();
		while (tit != tars.end()) {
			// if it has not been visited
			if (visited.find(*tit) == visited.end()) {
				workStack.push(*tit);
			}
			tit++;
		}
	}

	ret->insert(visited.begin(), visited.end());
}

void DyckAliasAnalysis::getEscapedPointersTo(set<DyckVertex*>* ret, Function *func) {
	if (ret == NULL || func == NULL) {
		errs() << "Warning in getEscapingPointers: ret or func are null!\n";
		return;
	}

	Module* module = func->getParent();

	set<DyckVertex*> visited;
	stack<DyckVertex*> workStack;

	iplist<GlobalVariable>::iterator git = module->global_begin();
	while (git != module->global_end()) {
		//if (!git->isConstant()) {
		DyckVertex * rt = dyck_graph->retrieveDyckVertex(git).first->getRepresentative();
		workStack.push(rt);
		//}
		git++;
	}

	iplist<Argument>& alt = func->getArgumentList();
	iplist<Argument>::iterator it = alt.begin();
	if (func->hasName() && func->getName() == "pthread_create") {
		it++;
		it++;
		it++;
		DyckVertex * rt = dyck_graph->retrieveDyckVertex(it).first->getRepresentative();
		workStack.push(rt);
	} else {
		while (it != alt.end()) {
			DyckVertex * rt = dyck_graph->retrieveDyckVertex(it).first->getRepresentative();
			workStack.push(rt);
			it++;
		}
	}

	while (!workStack.empty()) {
		DyckVertex* top = workStack.top();
		workStack.pop();

		// have visited
		if (visited.find(top) != visited.end()) {
			continue;
		}

		visited.insert(top);

		set<DyckVertex*> tars;
		top->getOutVertices(&tars);
		set<DyckVertex*>::iterator tit = tars.begin();
		while (tit != tars.end()) {
			// if it has not been visited
			DyckVertex* dv = (*tit)->getRepresentative();
			if (visited.find(dv) == visited.end()) {
				workStack.push(dv);
			}
			tit++;
		}
	}

	ret->insert(visited.begin(), visited.end());
}

bool DyckAliasAnalysis::runOnModule(Module &M) {
	InitializeAliasAnalysis(this);

	AAAnalyzer* aaa = new AAAnalyzer(&M, this, dyck_graph);

	/// step 1: intra-procedure analysis
	aaa->start_intra_procedure_analysis();
	outs() << "Start intra-procedure analysis...\n";
	aaa->intra_procedure_analysis();
	outs() << "Done!\n\n";
	aaa->end_intra_procedure_analysis();

	/// step 2: inter-procedure analysis
	aaa->start_inter_procedure_analysis();
	outs() << "Start inter-procedure analysis...\n";
	int itTimes = 0;
	while (1) {
		bool finished = dyck_graph->qirunAlgorithm();

		if (itTimes != 0 && finished) {
			break;
		}

		outs() << "Iterating... " << ++itTimes << "             \r";

		finished = aaa->inter_procedure_analysis();

		if (finished) {
			break;
		}
	}
	outs() << "\nDone!\n\n";
	aaa->end_inter_procedure_analysis();

	set<Instruction*> unhandled_calls; // Currently, it is used only for canary-record-transformer
	aaa->getUnhandledCallInstructions(&unhandled_calls);

	delete aaa;

	return false;
}

ModulePass *createDyckAliasAnalysisPass() {
	return new DyckAliasAnalysis();
}
}
