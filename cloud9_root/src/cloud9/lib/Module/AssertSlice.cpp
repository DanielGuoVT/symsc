/**
 * Author: Markus Kusano
 */

#include "llvm/Constants.h"
#include "llvm/Module.h"
#include "llvm/Instruction.h"
#include "llvm/Instructions.h"
#include "llvm/Value.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/PassSupport.h"
#include "llvm/Support/CommandLine.h"

// if defined, the pass will use the provided alias analysis (must be run with
// the pass) to build up the alias sets of the slice
#define STATIC_ALIAS

#ifdef STATIC_ALIAS
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Pass.h"
#endif

#include "ControlDependence.h"
#include "FileInfo.h"

#include "Passes.h"

#include <stack>
#include <map>
#include <set>

#define MK_DEBUG

// Skip function calls added by Klee (see skipFunction())
#define SKIP_KLEE

#ifdef MK_DEBUG
#define DEBUG_MSG(str) do { errs() << str; } while( false )
#else
#define DEBUG_MSG(str) do { } while (false)
#endif

#ifdef MK_DEBUG
#define DEBUG_MSG_RED(str) \
do { \
	if (1) {  \
      errs().changeColor(raw_ostream::RED); \
    } \
    errs() << str;  \
    errs().resetColor(); } while (false)
#else
#define DEBUG_MSG_RED(str) do { } while (false)
#endif

using namespace llvm;

cl::opt<bool> ExploreIntraCD("intra-cd", cl::desc("expore intra-procedural control dependence and data dependence"), cl::init(false));
cl::opt<unsigned int> RoundsOfRuns("rounds", cl::desc("the rounds of runs"), cl::init(0));

cl::opt<bool> AS("assert-slicing", cl::desc("do assert-slicing"), cl::init(false));

namespace klee {

#define PH 0.6
#define CH 0.78
#define OH 0.84
#define RH 0.72
#define SH 0.55
#define GH 0.62

void AssertSlice::runAssertSlicing(Module &M) {
	DEBUG_MSG("[MK DEBUG] in AssertSlice::runOnModule()\n");

	std::vector<CallInst *> assertCalls;

	assertCalls = findAssertFail(M); // find the assert calls

	assertCalls = filterAsserts(assertCalls);

#ifdef MK_DEBUG
	if (assertCalls.size() > 0) {
		DEBUG_MSG_RED("[MK DEBUG] __assert_fail calls found:\n");
		for (size_t i = 0; i < assertCalls.size(); ++i) {
			errs() << *assertCalls[i] << '\n';
		}
	}
#endif

	// slice back on each assertion
	for (size_t i = 0; i < assertCalls.size(); ++i) {
		// Values who's def-use chains still need to be traversed
		std::stack<Value *> defUseBacktrack;

		CallInst *assertc = assertCalls[i];
		Function *currFunc = assertc->getParent()->getParent();

		// get control dependencies of the function containing the assert call
		ControlDependence cdep;
		PostDominatorTree &PDT = getAnalysis<PostDominatorTree>(*currFunc);

		cdep.getControlDependencies(*currFunc, PDT);

		// vector of Values encountered in this functions CDG which should have
		// their data dependencies checked
		std::vector<Value *> dataCheck;
		dataCheck = addCdepToSlice(assertc, cdep, assertc->getParent());
		if (dataCheck.size()) {
			addDdepToSlice(assertc, dataCheck);
		}

		// check if any of the items in the slice could be influenced by the
		// parameters of the function
		std::set<unsigned> leakyArguments = argumentLeak(assertc, currFunc);

		// If the slice could be influenced by parameters of the function then we
		// need to update the slice with each callsite of the function as well as
		// any transitive function callsites.
		if (leakyArguments.size()) {
			addFuncCallToSlice(assertc, leakyArguments, currFunc);
		}

		DEBUG_MSG_RED("Finished Slice\n");
	} // for (assertCalls.size())

	clapInstIDOut(M.getModuleIdentifier());

#ifdef STATIC_ALIAS
	// This is done on the fly in addDDepToSlize()
	//	DEBUG_MSG_RED("Building alias sets statically\n");
	//	SliceMap_ = extendWithAlias(SliceMap_, M);
#endif

#ifdef MK_DEBUG
	DEBUG_MSG_RED("Finished All Slices\n");
	// output to file
	std::string filename;
	filename = M.getModuleIdentifier() + ".slices";
	DEBUG_MSG("Output filename: " << filename << '\n');
	std::string errInfo;
	raw_fd_ostream out(filename.c_str(), errInfo);

	if (!errInfo.empty()) {
		errs() << "[ERROR] Error opening file: " << filename << '\n' << errInfo << '\n';
		exit(EXIT_FAILURE);
	}

	out << AssertSlice::slMapToString(SliceMap_);
#endif

#ifdef MK_DEBUG
#if 0
	// Print out CDG  for each function
	for (Module::iterator mi = M.begin(); mi != M.end(); ++mi) {
		// get post-dom tree+control dependencies for function that actually have
		// bodies
		if (!(mi->isDeclaration())) {
			PostDominatorTree &PDT = getAnalysis<PostDominatorTree>(*mi);

			errs() << "[MK DEBUG] calling getControlDependences()\n";

			ControlDep.getControlDependencies(*mi, PDT);
			ControlDep.toDot(mi->getName(), ControlDep.controlDeps_);
			std::string name = mi->getName();
			name += ".rev";
			ControlDep.toDot(name, ControlDep.reverseControlDeps_);
		}
	}
#endif
#endif

	// nothing modified in IR
}

bool AssertSlice::runOnModule(Module &M) {
	if (AS) {
		runAssertSlicing(M);
	}
	return false;
}

std::vector<Value *> AssertSlice::addCdepToSlice(const CallInst *assertCall, const ControlDependence &cdep, BasicBlock *Sb) {
	assert(assertCall != NULL);
	assert(Sb != NULL && "NULL starting basic block passed");

	// Return value. Vector of Values which should have their data dependencies
	// checked and added to the slice.
	std::vector<Value *> dataCheck;

	// keep track of which basicblocks we have already checked
	static std::set<BasicBlock *> paramVisited;
	if (paramVisited.count(Sb)) {
		// already visited this basic block
		return dataCheck;
	}

	// mark the basicblock we're about to check to the visited set
	paramVisited.insert(Sb);

	DEBUG_MSG_RED("[MK DEBUG] Examining Control Dependencies of:\n");
	DEBUG_MSG("Address: " << Sb << '\n');
	DEBUG_MSG(*Sb << '\n');

	std::set<Instruction *> &slice = SliceMap_[assertCall];
	std::stack<BasicBlock *> backtrack;

	// Set of already visited basicblocks
	std::set<BasicBlock *> visited;

	// The entry basicblock for the CDG traversal is considered slightly
	// different compared to other blocks encountered in the traversal. It is
	// assumed that the entry block contains some instruction of interest
	// (potentially the terminator) but in this function we don't know which.
	// As such, we don't add the terminator to the slice. We simply start
	// searching to see which other basicblocks control the function reacing
	// the entry (traversal of the reverse CDG).

	// Get the control dependencies of the entry block
	ControlDependence::depmap::const_iterator cdepi = cdep.reverseControlDeps_.find(Sb);

	if (cdepi == cdep.reverseControlDeps_.end()) {
		// This is a case where basicblock which, in the current function is not
		// controlled by anything (the entry block). This can happen when we are
		// calculating interprocedural control dependencies and we end up with a
		// call instruction in the entry block. We don't need to do anything, in
		// terms of control dependences. We simply need to following the _data_
		// dependencies of the argument of interest in the current function
		assert(dataCheck.size() == 0);
		DEBUG_MSG("\tNo Control dependencies found\n");
		return dataCheck;
	}

	assert(cdepi->second.size() != 0 && "size 0 dependence set found for node");

	// add all the blocks which control the execution of the entry block and
	// start searching
	for (std::set<BasicBlock *>::iterator cdi = cdepi->second.begin(), cde = cdepi->second.end(); cdi != cde; ++cdi) {
		backtrack.push(*cdi);
	}

	DEBUG_MSG("\tstarting backtrack size: " << backtrack.size() << '\n');

	while (!backtrack.empty()) {
		// copy over the top of the stack
		BasicBlock *curBlock = backtrack.top();
		// remove the top item
		backtrack.pop();

		// mark the current BasicBlock as visited
		visited.insert(curBlock);

		DEBUG_MSG("\tVisiting Node: " << *curBlock << '\n');

		ControlDependence::depmap::const_iterator cdepi = cdep.reverseControlDeps_.find(curBlock);

		// Add the terminator of the block to the slice
		TerminatorInst *ti = curBlock->getTerminator();
		DEBUG_MSG("[MK DEBUG] Adding to slice: " << *ti << '\n');
		sliceInsert(slice, ti);
		// Get which should have its data dependencies checked from the
		// terminator
		Value *cond = getCond(ti);
		if (cond != NULL) {
			DEBUG_MSG("\tAdding Terminator Cond: " << cond << '\n');
			dataCheck.push_back(cond);
		}

		if (cdepi == cdep.reverseControlDeps_.end()) {
			DEBUG_MSG("\tVisiting node with no rev control deps\n");
			continue;
		}

		// Get the basicblock which curBlock is control dependent on
		std::set<BasicBlock *> depSet;
		depSet = cdepi->second;

		for (std::set<BasicBlock *>::iterator si = depSet.begin(), se = depSet.end(); si != se; ++si) {
			BasicBlock *bb;
			bb = *si;
			DEBUG_MSG("Dependent BB:\n" << *bb << '\n');

			// Add the basicblock to the backtrack stack. This will ensure that the
			// transitive control dependencies are added to the slice
			if (visited.find(bb) == visited.end()) {
				// Not visited, it will be marked as visited on entry to the while
				// loop
				DEBUG_MSG("[MK DEBUG] adding BasicBlock to CDG backtrack set:\n" << *bb << '\n');
				backtrack.push(bb);
			}
		} // for (std::set)
	} // while (!backtrack.empty())

	return dataCheck;
}

void AssertSlice::addDdepToSlice(const CallInst *assertCall, std::vector<Value *> values) {
	assert(assertCall);
	assert(values.size() && "Vector size 0 passed");

	DEBUG_MSG_RED("[MK DEBUG] Examining data dependence chain\n");
	DEBUG_MSG("In Function: " << *(assertCall->getParent()->getParent()) << '\n');

	if (skipFunction(assertCall->getParent()->getParent())) {
		return;
	}

	// keep track of what has already been visited
	static std::set<std::pair<const CallInst *, const Value *> > paramVisited;

	// Vector of the values that need to be checked
	std::vector<Value *> vals;

	for (std::vector<Value *>::iterator i = values.begin(), e = values.end(); i != e; ++i) {
		std::pair<const CallInst *, const Value *> param;
		param = std::make_pair(assertCall, *i);

		if (!paramVisited.count(param)) {
			// not visited, so we need to check it and mark it as visited
			vals.push_back(*i);
			paramVisited.insert(param);
		}
	}

	if (vals.size() == 0) {
		// all parameters already visited
		return;
	}

	std::stack<Value *> backtrack;

	// Assumption: all the instructions we are iterating over are in the same
	// function. This is set to that function when necessary (when we need to find
	// the callsites of the function).
	Function *currFunc = NULL;

	// Values which have already been visited
	std::set<Value *> visited;

	// Function who's return values have already been traversed
	std::set<Function *> visitedRets;

	// Slice for the current assertion
	std::set<Instruction *> &slice = SliceMap_[assertCall];

	// Add the entire passed vector to the backtrack set and starting searching
	for (size_t i = 0; i < vals.size(); ++i) {
		visited.insert(vals[i]);
		backtrack.push(vals[i]);
	}

	while (!backtrack.empty()) {
		// copy top element and pop
		Value *curr = backtrack.top();
		backtrack.pop();

		DEBUG_MSG("[MK DEBUG] examining def: " << *curr << '\n');

		// iterate over uses of instruction
		Instruction *inst = NULL;
		if ((inst = dyn_cast<Instruction>(curr))) {
			if (isa<InvokeInst>(inst) || isa<CallInst>(inst)) {
				DEBUG_MSG("\tCallSite found on dependence chain\n");

				// as an over-approximation, assume that all the arguments of the
				// function influence the return value.
				CallSite cs = CallSite(inst);

				DEBUG_MSG("\tnum args: " << cs.arg_size() << '\n');

				for (CallSite::arg_iterator i = cs.arg_begin(), e = cs.arg_end(); i != e; ++i) {
					DEBUG_MSG("\tCallSite arg: " << *i << '\n');
					if (!visited.count(*i)) {
						visited.insert(*i);
						backtrack.push(*i);
					}
				}

				Function *calledFunc = cs.getCalledFunction();

				if (!calledFunc) {
					assert(0 && "indirect function call encountered");
				}

				if (!visited.count(cs.getCalledFunction())) {
					visitedRets.insert(cs.getCalledFunction());
					if (!ignoreInst(cs.getInstruction())) {
						// skip clap_ function calls
						DEBUG_MSG("\tcalled function: " << *cs.getCalledFunction() << '\n');
						addFuncReturnToSlice(assertCall, cs.getCalledFunction());
					}
				}
			} else if (LoadInst * load = dyn_cast<LoadInst>(inst)) {
				DEBUG_MSG("\tLoadInst found on use-def chain\n");
				// follow the address being loaded
				Value *loadAddr = load->getOperand(0);
				// dependeing on what the value is, it will be added to the slice (or
				// not) on the next run through the loop
				if (!visited.count(loadAddr)) {
					visited.insert(loadAddr);
					backtrack.push(loadAddr);
				}
				loadAndStoreInsts.insert(load);
			} else if (StoreInst * store = dyn_cast<StoreInst>(inst)) {
				DEBUG_MSG("\tStoreInst found on use-def chain\n");
				// follow the value being stored
				Value *stored = store->getOperand(0);
				// dependeing on what the value is, it will be added to the slice (or
				// not) on the next run through the loop
				if (!visited.count(stored)) {
					visited.insert(stored);
					backtrack.push(stored);
				}
				loadAndStoreInsts.insert(store);
			}
#if 1
			// TODO: Do we need to follow the def-use chains of Alloca
			// instructions?
			else if (AllocaInst * allo = dyn_cast<AllocaInst>(inst)) {
				currFunc = inst->getParent()->getParent();
				DEBUG_MSG("\tAllocaInst on use-def chain\n");
				std::vector<Value *> useCheck;
				useCheck = handleAlloca(assertCall, allo);
				//useCheck = pruneHappensAfter(useCheck, inst);
				for (size_t i = 0; i != useCheck.size(); ++i) {
					Value *v = useCheck[i];
					DEBUG_MSG("\tChecking from handleAlloca: " << *v << '\n');
					if (visited.find(v) == visited.end()) {
						DEBUG_MSG("\tadding to backtrack: " << *v << '\n');
						visited.insert(v);
						backtrack.push(v);
					}
				}
#if 0
				// for Alloca instructions, also added all the uses of the def. This
				// captures stores to the alloca instruction
				std::vector<Instruction *> useCheck;
				useCheck = addDefUseToSlice(assertCall, inst);

				for (size_t i = 0; i != useCheck.size(); ++i) {
					Instruction *in = useCheck[i];
					DEBUG_MSG("\tChecking from addDefUse: " << *inst << '\n');
					for (User::op_iterator i = in->op_begin(), e = in->op_end();
							i != e; ++i) {
						// on the next iteration this will be added to the slice
						Value *v = *i;
						if (visited.find(v) == visited.end()) {
							DEBUG_MSG("\tadding to backtrack: " << *v << '\n');
							visited.insert(v);
							backtrack.push(v);
						}
					}
				}
#endif
			} // dyn_cast<AllocaInst>

#endif

			DEBUG_MSG("\t def added to slice\n");
			sliceInsert(slice, inst);
			for (User::op_iterator i = inst->op_begin(), e = inst->op_end(); i != e; ++i) {
				// on the next iteration this will be added to the slice
				Value *v = *i;
				if (visited.find(v) == visited.end()) {
					DEBUG_MSG("\tadding to backtrack: " << *v << '\n');
					visited.insert(v);
					backtrack.push(v);
				}
			}
		} // inst = dyn_cast<Instruction>
		else if (isa<Constant>(curr)) {
			continue;
		} else if (Argument * arg = dyn_cast<Argument>(curr)) {
			// follow argument out of the function
			currFunc = arg->getParent();
			assert(currFunc != NULL && "current function not set");
			std::set<unsigned> pos;
			pos.insert(argumentPosition(arg, currFunc));
			addFuncCallToSlice(assertCall, pos, currFunc);

		} else {
			// TODO: Probably have to handle more uses as needed
			errs() << "[ERROR] Examining Value: " << *curr << '\n';
			assert(0 && "unhandled value encountered");
		}
	} // while (!backtrack.empty())

	// Add all the control/data dependencies of the alias set of each load/store instruction we have visited
	if (ExploreIntraCD) {
		if (loadAndStoreInsts.size()) {
			addLoadAndStoreAlias(assertCall, loadAndStoreInsts);
		}
	} //if (ExploreIntraCD)
} //addDdepToSlice

std::vector<CallInst *> AssertSlice::findAssertFail(Module &M) const {
	DEBUG_MSG("Enter findAssertFail()\n");
	std::vector<CallInst *> assertCalls;
	// iterate over functions
	for (Module::iterator mi = M.begin(), me = M.end(); mi != me; ++mi) {
		if (skipFunction(&*mi)) {
			continue;
		}
		// iterate over insturctions in function
		for (inst_iterator ii = inst_begin(*mi), ie = inst_end(*mi); ii != ie; ++ii) {
			Instruction &curr_inst = *ii;
			// search for calls. My assumption is that Invokes to __assert_fail()
			// should never exist since a call to __assert_fail() will never return
			if (CallInst * call = dyn_cast<CallInst>(&curr_inst)) {
				// check for indirect function calls, they are not supported by this
				// pass
				Function *called_func = call->getCalledFunction();
				if (called_func == NULL) {
					errs() << "Instruction : " << curr_inst << '\n' << "In function: " << mi->getName() << "()\n";
					errs() << "[ERROR] Indirect function calls are not supported\n";
					errs() << "LineNumber: " << getDebugLineNum(&curr_inst) << '\n';
					errs() << "Filename: " << getDebugFilename(&curr_inst) << '\n';
					exit(EXIT_FAILURE);
					// continue;
				}
				if (called_func->getName() == "__assert_fail" || called_func->getName() == "clap___assert_fail") {
					assertCalls.push_back(call);
				}
			} else if (InvokeInst * invoke = dyn_cast<InvokeInst>(&curr_inst)) {
				// check for indirect function calls, they are not supported by this
				// pass
				Function *called_func = invoke->getCalledFunction();
				if (called_func == NULL) {
					DEBUG_MSG("Instruction : " << curr_inst << '\n' << "In function: " << mi->getName() << "()\n");
					errs() << "[ERROR] Indirect function invokes are not supported\n";
					exit(EXIT_FAILURE);
				}

				if (called_func->getName() == "__assert_fail") {
					errs() << "[ERROR] __assert_fail invoke inst encountered\n";
					exit(EXIT_FAILURE);
				}
			}
		} // for (const_isnt_iteratr)
	} // for (Module::const_iterator)

	return assertCalls;
}

Value *AssertSlice::getCond(TerminatorInst *tinst) {
	// check all the possible types of terminator instructions
	//if (ReturnInst *ret = dyn_cast<ReturnInst>(tinst)) {
	if (isa<ReturnInst>(tinst)) {
		// In our traversal up the CDG, we should not encounter return
		// instructions
		assert(0 && "return terminator encountered in CDG traversal");
		// A return instruction itself has no data dependencies attached to it
		// to worry about. The value returned will only be of interest if it
		// can affect some other data values controling the assertion.
		//return NULL;
	} else if (BranchInst * br = (dyn_cast<BranchInst>(tinst))) {
		// The condition of the branch is the only value of interest. The other
		// two operands are simply the labels
		if (br->isConditional())
			return br->getCondition();
		return NULL;
	} else if (SwitchInst * swt = (dyn_cast<SwitchInst>(tinst))) {
		return swt->getCondition();
	} else if (IndirectBrInst * ibr = (dyn_cast<IndirectBrInst>(tinst))) {
		return ibr->getAddress();
	} else if (isa<InvokeInst>(tinst)) {
		// Invoke instructions have no conditions associated with them. If the
		// parameters of the invoke instructions would affect the instructions on
		// the slice then they should show up else where on the data dependence
		// chains
		return NULL;
	} else if (ResumeInst * res = (dyn_cast<ResumeInst>(tinst))) {
		// The value associated with a resume instruction is the exception type.
		return res->getValue();
	} else if (isa<UnreachableInst>(tinst)) {
		// unreachable instructions have no values associated. This should really
		// only happen on the basic block containing __assert_fail().
		return NULL;
	} else {
		llvm_unreachable("unknown terminator encountered");
	}
}

std::set<unsigned> AssertSlice::argumentLeak(const CallInst *assertCall, Function *func) {
	assert(assertCall && "NULL passed");
	assert(func && "NULL passed");

	// Positions in the functions argument list that leak into the slice. In
	// other words, these are the arguments in the function that we need to
	// trace interprocedurally to callsites of the function.
	std::set<unsigned> paramPos;

	// TODO: DO WE NEED THIS FUNCTION?
	return paramPos;

#if 0
	std::map<const CallInst *, std::set<Instruction *> >::iterator
	mit = SliceMap_.find(assertCall);

	assert(mit != SliceMap_.end() && "no slice found for assert call");

	std::set<Instruction *> &slice = mit->second;

	Function::ArgumentListType &argList = func->getArgumentList();

	// Even if two arguments point to the same instruction, we only need to
	// visit each one once. We are only looking to see if any instruction that
	// is influecned by any of the function arguments is already in the slice;
	// regardless of the function parameter, the def-use chain will be the
	// same.
	std::set<const Value *> visited;

	for (Function::ArgumentListType::iterator ai = argList.begin(),
			ae = argList.end(); ai != ae; ++ ai) {
		DEBUG_MSG("[MK DEBUG] argumentLeak(): checking arg: " << *ai << '\n');
		std::stack<Value *> backtrack;
		backtrack.push(&*ai);

		while (!backtrack.empty()) {
			// copy and pop stack
			Value *curr = backtrack.top();
			backtrack.pop();
			if (Instruction *inst = dyn_cast<Instruction>(curr)) {
				if (slice.count(inst) == 1) {
					DEBUG_MSG("[MK DEBUG] found argument leaking into slice: "
							<< *inst << '\n'
							<< "\tcurrent argument: " << *ai << '\n'
							<< "\targument position: " << argumentPosition(*&ai, func)
							<< '\n');

					// found argmuent leaking into the slice
					paramPos.insert(argumentPosition(*&ai, func));
				}
			}
			else if (isa<Argument>(curr)) {
				// do nothing with arguments except follow their def-use chain
			}
			else if (isa<GlobalVariable>(curr)) {
				// similar to arguments, just follow the def-use chain of globals
			}
			else {
				errs() << "[ERROR] Examining Value: " << *curr << '\n';
				assert(0 && "unsupported value found on def-use chain");
			}
			// follow def-use chain of every function parameter
			for (Value::use_iterator ui = curr->use_begin(),
					ue = curr->use_end(); ui != ue; ++ui) {
				Value *use = *ui;

				if (!visited.count(use)) {
					// not visited
					DEBUG_MSG("\tAdding to backtrack: " << *use << '\n');
					visited.insert(use);
					backtrack.push(use);
				}
				if (StoreInst *str = dyn_cast<StoreInst>(use)) {
					// also track if the memory location being stored into is being
					// used by any instructions in the slice
					Value *strLoc = str->getOperand(1);
					if (!visited.count(strLoc)) {
						DEBUG_MSG("\tAdding to backtrack: " << *strLoc << '\n');
						visited.insert(strLoc);
						backtrack.push(strLoc);
					}
				}
			} // for (use_iterator)
		} // while (!backtrack.empty())
	} // for (ArgumentList)

	return paramPos;
#endif
}
void AssertSlice::addFuncCallToSlice(const CallInst *assertc, std::set<unsigned> argpos, Function *func) {
	assert(assertc && "NULL assert call passed");
	assert(func && "NULL function passed");
	assert(argpos.size() != 0 && "zero aguments of interest in func");

	DEBUG_MSG_RED("[MK DEBUG] Checking callsites\n");

	std::stack<std::pair<Function *, std::set<unsigned> > > backtrack;

	// keep track of which functions have already been visited. This is a map
	// of functions to a set of arguments that have already been visited for
	// this function
	static std::map<Function *, std::set<unsigned> > paramVisited;

	std::map<Function *, std::set<unsigned> >::iterator paramIter;

	paramIter = paramVisited.find(func);
	if (paramIter != paramVisited.end()) {
		for (std::set<unsigned>::iterator i = argpos.begin(), e = argpos.end(); i != e; ++i) {
			if (paramIter->second.count(*i)) {
				// argument already visited for this function
				DEBUG_MSG("\argument visited: " << *i << '\n');
				argpos.erase(i);
			} else {
				// mark the argument as visited
				paramIter->second.insert(*i);
			}
		}
	} else {
		paramVisited.insert(std::make_pair(func, argpos));
	}

	if (argpos.size() == 0) {
		// the function has been visited and all the arguments that are of
		// interest in the current call have also been visited, so we don't need
		// to do anything
		return;
	}

#ifdef MK_DEBUG
	for (std::set<unsigned>::iterator sii = argpos.begin(); sii != argpos.end(); ++sii) {
		DEBUG_MSG("\targpos contains: " << *sii << '\n');
	}
#endif

	// Map of visited functions. The associated set is the set of arguments
	// who's data dependencies have been tracked. Control dependencies only
	// need to be updated for each function once.
	std::map<Function *, std::set<unsigned> > visited;
	backtrack.push(std::make_pair(func, argpos));

	while (!backtrack.empty()) {
		std::pair<Function *, std::set<unsigned> > currPair;

		// copy and pop top element of stack
		currPair = backtrack.top();
		backtrack.pop();

		Function *currFunc = currPair.first;
		std::set<unsigned> currArgs;
		currArgs = currPair.second;
		assert(currArgs.size() && "backtrack pair w/o arguments to check");

		// check if we have visited the function and any number of its arguments
		std::map<Function *, std::set<unsigned> >::iterator mi;
		mi = visited.find(currFunc);

		// only need to check control dependencies once per function
		bool cdepVisited;
		if (mi == visited.end()) {
			// Function not found in visited map. Implicitly, this means that all
			// the arguments have never been visited either
			cdepVisited = false;

			// Update visited information
			visited.insert(std::make_pair(currFunc, currArgs));
		} else {
			// Function found in visited map
			cdepVisited = true;
			// check if we have visited any of the arguments of the function
			// already.
			//
			// Do this by checking if the visited function's set contains any of the
			// items in the current function's set. These arguments do not need to
			// have their data dependencies queried.
			for (std::set<unsigned>::iterator si = currArgs.begin(), se = currArgs.end(); si != se; ++si) {
				if (mi->second.find(*si) != mi->second.end()) {
					// argument already visited, so erase it from the set of arguments
					// to be checked
					DEBUG_MSG("\tcurrArgs erased\n");
					currArgs.erase(*si);
				}
			}
			// Mark new arguments as visited
			if (currArgs.size()) {
				mi->second.insert(currArgs.begin(), currArgs.end());
			}
		}

#ifdef MK_DEBUG
		for (std::set<unsigned>::iterator sii = currArgs.begin(); sii != currArgs.end(); ++sii) {
			DEBUG_MSG("\tcurrArgs contains: " << *sii << '\n');
		}
#endif

		DEBUG_MSG("\tcdepVisited: " << (cdepVisited ? "true" : "false") << '\n');
		if (cdepVisited && (currArgs.size() == 0)) {
			// TODO: OPTIMIZATION: We could cache the control dependence results to
			// use when currArgs.size() != 0 and cdepVisited == true
			DEBUG_MSG("\tcontinuing");
			continue;
		}

		// The execution of the assert call is dependent on the function's
		// arguments. We proceed to find each location where the function is
		// called and perform the same control and data dependency tracking for
		// the argument of interest

		DEBUG_MSG("[MK DEBUG] uses of function: " << currFunc->getName() << '\n');
		for (Value::use_iterator i = currFunc->use_begin(), e = currFunc->use_end(); i != e; ++i) {
			// true if the use of the function is a pthread_create call.
			bool isPthreadCreate = false;
			Value *use = *i;
			DEBUG_MSG("=== ITER ==\n");
			DEBUG_MSG("\t" << *use << '\n');

			if (!(isa<CallInst>(use) || isa<InvokeInst>(use))) {
				continue;
			}
			// TODO: We probably need to handle pthread_create calls to a function
			// and the arguments passed via the pthread_create call
			assert(isa<CallInst>(use) || isa<InvokeInst>(use) && "non callsite function use encountered");
			CallSite site;

			if (CallInst * ci = dyn_cast<CallInst>(use)) {
				site = CallSite(ci);
			} else if (InvokeInst * ii = dyn_cast<InvokeInst>(use)) {
				site = CallSite(ii);
			} else {
				assert(0 && "non callsite function use encountered");
			}

			// add the callsite to the slice
			std::set<Instruction *> &sli = SliceMap_[assertc];
			sliceInsert(sli, site.getInstruction());

			BasicBlock *calleeBb = site->getParent();
			Function *callee = calleeBb->getParent();

			ControlDependence calleeCdep;
			PostDominatorTree &calleePDT = getAnalysis<PostDominatorTree>(*callee);
			calleeCdep.getControlDependencies(*callee, calleePDT);

			// Add controldependencies of function call to slice
			std::vector<Value *> dataDeps = addCdepToSlice(assertc, calleeCdep, calleeBb);

			DEBUG_MSG("\tcurrArgs.size(): " << currArgs.size() << '\n');

			// Modify the arguments incase the use is in pthread_create
			Function *calledFunc = site.getCalledFunction();

			if (calledFunc == NULL) {
				errs() << "[ERROR] Indirect function call encountered\n";
				exit(EXIT_FAILURE);
			} else if (calledFunc->getName() == "pthread_create" || calledFunc->getName() == "clap_thread_create") {
				// For a pthread_create call, the arguments are always in position 3
				// opposed to position 0 (where they actually are in the function)
				assert(currArgs.size() == 1 && "pthread_create func w/o 1 arg operand");
				assert(currArgs.count(0U) == 1 && "checking pthread_create on non 0th argument");
				isPthreadCreate = true;
			} else if (calledFunc->getName() == currFunc->getName()) {
				// do nothing
			} else {
				assert(0 && "function used in unsupported call/invoke");
			}

			// In addition to the data values influencing the control
			// statements we also need to add the argument itself to the slice
			if (!isPthreadCreate) {
				for (std::set<unsigned>::const_iterator i = currArgs.begin(), e = currArgs.end(); i != e; ++i) {
					unsigned argPos = *i;
					assert(argPos < site.arg_size() && "invalid argument for callsite");
					Value *arg = site.getArgument(argPos);
					DEBUG_MSG("\tCallSite Arg: " << *arg << '\n');
					dataDeps.push_back(arg);
				}
			} else {
				// pthread_create argument position is always 3
				assert(3 < site.arg_size() && "invalid argument pthread_createcallsite");
				Value *arg = site.getArgument(3);
				DEBUG_MSG("\tCallSite Arg: " << *arg << '\n');
				dataDeps.push_back(arg);
			}

			// We should always be following at least the data dependencies of
			// one argument, addDdepToSlice will assert fail when
			// dataDeps.size() is zero
			addDdepToSlice(assertc, dataDeps);

			// Check if we need to continue checking dependencies of functions
			// calling the callee
			std::set<unsigned> leakyArgs = argumentLeak(assertc, callee);

			if (leakyArgs.size()) {
				DEBUG_MSG("\tBacktracking Function: " << callee->getName() << '\n');
				backtrack.push(std::make_pair(callee, leakyArgs));
			}
		} // for value::user iterator
	} // while (!backTrack.empty())
}

unsigned AssertSlice::argumentPosition(const Argument *arg, const Function *func) const {
	assert(arg && "NULL argument passed");
	assert(func && "NULL function passed");

	return arg->getArgNo();

	// Get the argument list and iterate over it until you find the argument of
	// interest
#if 0
	const Function::ArgumentListType &argList = func->getArgumentList();
	unsigned pos = 0;
	for (Function::ArgumentListType::const_iterator ai = argList.begin(),
			ae = argList.end(); ai != ae; ++ai) {
		if (arg == &*ai) {
			return pos;
		}
		pos++;
	}
	llvm_unreachable("argument not found in argument list");
#endif
}

std::string AssertSlice::sliceToString(const std::set<Instruction *> &sl) {
	std::string ret;
	raw_string_ostream ss(ret);
	DEBUG_MSG("[MK DEBUG] Stringifying Slice of Size: " << sl.size() << '\n');

	for (std::set<Instruction *>::const_iterator si = sl.begin(), se = sl.end(); si != se; ++si) {
		ss << **si << '\n';
	}

	return ss.str();
}
std::string AssertSlice::slMapToString(const std::map<const CallInst *, std::set<Instruction *> > &slm) {
	std::string ret;
	raw_string_ostream ss(ret);

	for (std::map<const CallInst *, std::set<Instruction *> >::const_iterator mi = slm.begin(), me = slm.end(); mi != me; ++mi) {
		const CallInst *acall = mi->first;

		ss << "==== Slice for: " << '\n' << *acall << '\n';
		const Function *f = acall->getParent()->getParent();
		if (f->hasName()) {
			ss << "in function: " << f->getName() << '\n';
		}
		ss << sliceToString(mi->second);
		ss << "==== End Slice ====\n";
	}

	return ss.str();
}

std::vector<Instruction *> AssertSlice::addDefUseToSlice(const CallInst *assertc, Value *v) {
	assert(v && "NULL value passed");
	std::set<Value *> visited;
	std::stack<Value *> backtrack;
	backtrack.push(v);

	std::vector<Instruction *> ret;

	// Dont visit the same instructions twice
	static std::set<std::pair<const CallInst *, Value *> > paramVisited;
	std::pair<const CallInst *, Value *> params = std::make_pair(assertc, v);

	if (paramVisited.count(params)) {
		return ret;
	}
	paramVisited.insert(params);

	std::set<Instruction *> &sli = SliceMap_[assertc];

	while (!backtrack.empty()) {
		// copy and pop top
		Value *curr = backtrack.top();
		backtrack.pop();
		visited.insert(curr);

		if (StoreInst * str = dyn_cast<StoreInst>(curr)) {
			DEBUG_MSG("\tadding store inst to slice: " << *str << '\n');
			sliceInsert(sli, str);
			// also track if the memory location being stored into is being
			// used by any instructions in the slice
			Value *strLoc = str->getOperand(1);
			if (!visited.count(strLoc)) {
				visited.insert(strLoc);
				backtrack.push(strLoc);
			}

			// the store instruction should have its uses followed since the value
			// being stored (operand 0) should be on the slice
			ret.push_back(str);
		} else if (Instruction * inst = dyn_cast<Instruction>(curr)) {
			sliceInsert(sli, inst);
			DEBUG_MSG("\taddDefUse: inst: " << *inst << '\n');
			if (isa<CallInst>(inst) || isa<InvokeInst>(inst)) {
				CallSite cs = CallSite(inst);
				Function *f = cs.getCalledFunction();
				//assert(f && "NULL function callsite encountered");
				assert(f && "Indirect function calls are not supported");
				// check if the function has a body (ie has a definition we know
				// about) and if it has a NULL return type.
				if (f->getBasicBlockList().size() && !f->getReturnType()->isVoidTy()) {
					addFuncReturnToSlice(assertc, f);
				} else {
					errs() << "[WARNING] Bodyless function found: " << f->getName() << "()\n";
				}
				// Check operands of call instruction too
				for (CallSite::arg_iterator ai = cs.arg_begin(), ae = cs.arg_end(); ai != ae; ++ai) {
					DEBUG_MSG("\adding CallSite arg: " << **ai << '\n');
					if (!visited.count(*ai)) {
						visited.insert(*ai);
						backtrack.push(*ai);
					}
				}
			} // isa<CallInst>
		} else if (isa<Argument>(curr)) {
			// do nothing except follow the uses of the argument
		} else if (isa<Constant>(curr)) {
			// don't even need to follow uses of constants
			continue;// goto next item on backtrack stack
		} else {
			errs() << "[ERROR] Value: " << *curr << '\n';
			assert(0 && "unhandled type on def-use chain");
		}

		for (Value::use_iterator ui = curr->use_begin(), ue = curr->use_end(); ui != ue; ++ui) {
			Value *use = *ui;
			if (!visited.count(use)) {
				backtrack.push(use);
			}
		}
	} // while (!backtrack.empty())
	return ret;
}

void AssertSlice::sliceInsert(std::set<Instruction *> &sli, Instruction *i) const {
	assert(i && "NULL instruction passed");

	if (ignoreInst(i)) {
		return;
	}
	sli.insert(i);

	LLVMContext &C = i->getParent()->getParent()->getContext();
	Value* elts[] = { ConstantInt::get(C, APInt(1, 1)) };
	MDNode* md_node = MDNode::get(C, elts);
	i->setMetadata("AssertSlice", md_node);
}

uint64_t AssertSlice::getInstID(const Instruction *i) const {
	assert(i && "NULL instruction passed");
	MDNode *clap_metadata = i->getMetadata("clap");

	DEBUG_MSG("getInstID() on instruction: " << *i << '\n');
	assert(clap_metadata && "instruction w/o clap inst ID found");

	Value *clap_id = clap_metadata->getOperand(0);

	// Operand 0 should be a constant integer
	if (ConstantInt * ci = dyn_cast<ConstantInt>(clap_id)) {
		// getZExtValue will assert fail if the value is larger than 64 bits
		return ci->getZExtValue();
	} else {
		llvm_unreachable("non-constant-int clap metadata operand 0 found");
	}
	llvm_unreachable("unreachable");
}

void AssertSlice::clapInstIDOut(const std::string &moduleName) const {
	assert(!moduleName.empty() && "empty string passed");

	std::string filename = moduleName;
	filename += ".slices.id";

	std::string errInfo;
	raw_fd_ostream out(filename.c_str(), errInfo);
	if (!errInfo.empty()) {
		errs() << "[ERROR] Error opening file: " << filename << "\n" << errInfo << '\n';
	}

	// Iterate over all the instructions in every slice
	for (std::map<const CallInst *, std::set<Instruction *> >::const_iterator mi = SliceMap_.begin(), me = SliceMap_.end(); mi != me; ++mi) {
		for (std::set<Instruction *>::const_iterator si = mi->second.begin(), se = mi->second.end(); si != se; ++si) {
			out << getInstID(*si) << '\n';
		}
	}
}

void AssertSlice::addFuncReturnToSlice(const CallInst *assertc, Function *f) {
	assert(f != NULL && "NULL function passed");
	DEBUG_MSG_RED("[MK DEBUG] Checking func return dependencies\n");
	DEBUG_MSG("\tfunc: " << *f << '\n');

	// Keep track of what has already been checked
	static std::set<std::pair<const CallInst *, Function *> > paramVisited;

	std::pair<const CallInst *, Function *> param;
	param = std::make_pair(assertc, f);

	// check if function has already been visited
	if (paramVisited.count(param)) {
		return;
	}

	// since it hasn't been visited, mark it as being visited
	paramVisited.insert(param);

	// Assumption: All function arguments may, even if they dont directly
	// influence the return value, alter the global state and should be on the
	// slice
	for (Function::arg_iterator ai = f->arg_begin(), ae = f->arg_end(); ai != ae; ++ai) {
		DEBUG_MSG("\tfollowing def-use of argument: " << *ai << '\n');
		addDefUseToSlice(assertc, &*ai);
	}

	// if the function has no body, then this is all we can do
	if (f->getBasicBlockList().size() == 0) {
		DEBUG_MSG("Function: " << *f << "has no body\n");
		return;
	}

	// backtrack stack for values (data dependencies to check)
	std::stack<Value *> valBacktrack;

	// bactrack stack for functions. This is required since we may find another
	// function call on the use-def chain which we need to also traverse
	std::stack<Function *> funcBacktrack;

	std::set<Value *> valVisited;
	std::set<Function *> funcVisited;

	std::set<Instruction *> &slice = SliceMap_[assertc];

	// add passed function to backtrack set and start searching
	funcBacktrack.push(f);
	while (!funcBacktrack.empty()) {
		Function *func = funcBacktrack.top();
		funcBacktrack.pop();
		funcVisited.insert(func);

		// find return instructions in function
		for (inst_iterator ii = inst_begin(func), ie = inst_end(func); ii != ie; ++ii) {
			Instruction &inst = *ii;
			if (ReturnInst * ret = dyn_cast<ReturnInst>(&inst)) {
				Value *retVal = ret->getReturnValue();
				assert(retVal != NULL && "slicing on NULL return value");
				DEBUG_MSG("\tFound return val: " << *retVal << 'n');
				valBacktrack.push(retVal);
			}
		}

		// get the control dependencies of the function
		ControlDependence cdep;
		PostDominatorTree &PDT = getAnalysis<PostDominatorTree>(*func);
		cdep.getControlDependencies(*func, PDT);

		// trace back the dependencies of all the return instructions in the
		// function
		while (!valBacktrack.empty()) {
			Value *val = valBacktrack.top();
			valBacktrack.pop();
			valVisited.insert(val);

			Instruction *inst = NULL;
			if ((inst = dyn_cast<Instruction>(val))) {
				// get the control dependencies of the return on the slice
				std::vector<Value *> dataDeps;
				dataDeps = addCdepToSlice(assertc, cdep, inst->getParent());

				// add data dependencies of control statements to backtrack
				for (size_t i = 0; i < dataDeps.size(); ++i) {
					valBacktrack.push(dataDeps[i]);
				}

				DEBUG_MSG("\tAdding to slice: " << *inst << '\n');
				sliceInsert(slice, inst);
				if (isa<InvokeInst>(inst) || isa<CallInst>(inst)) {
					DEBUG_MSG("\tCallSite found on return dependence chain\n");

					CallSite cs = CallSite(inst);

					DEBUG_MSG("\tnum args: " << cs.arg_size() << '\n');

					for (CallSite::arg_iterator i = cs.arg_begin(), e = cs.arg_end(); i != e; ++i) {
						DEBUG_MSG("\tCallSite arg: " << *i << '\n');
						valBacktrack.push(*i);
					}

					Function *calledFunc = cs.getCalledFunction();

					if (!calledFunc) {
						assert(0 && "indirect function call encountered");
					}

				} // if (isa<InvokeInst> ... )
				else if (AllocaInst * allo = dyn_cast<AllocaInst>(inst)) {
					std::vector<Value *> useCheck;
					useCheck = handleAlloca(assertc, allo);
					//useCheck = pruneHappensAfter(useCheck, allo);
					for (size_t i = 0; i != useCheck.size(); ++i) {
						Value *v = useCheck[i];
						DEBUG_MSG("\tChecking from handleAlloca: " << *v << '\n');
						if (!valVisited.count(v)) {
							DEBUG_MSG("\tadding to backtrack: " << *v << '\n');
							valVisited.insert(v);
							valBacktrack.push(v);
						}
					}

#if 0
					std::vector<Instruction *> useCheck;
					useCheck = addDefUseToSlice(assertc, inst);

					for (size_t i = 0; i != useCheck.size(); ++i) {
						Instruction *in = useCheck[i];
						DEBUG_MSG("\tChecking from addDefUse: " << *inst << '\n');
						for (User::op_iterator i = in->op_begin(), e = in->op_end();
								i != e; ++i) {
							// on the next iteration this will be added to the slice
							Value *v = *i;
							if (!valVisited.count(v))
							DEBUG_MSG("\tadding to backtrack: " << *v << '\n');
							valVisited.insert(v);
							valBacktrack.push(v);
						}
					}
#endif
				} // dyn_cast<AllocaInst>
				for (User::op_iterator i = inst->op_begin(), e = inst->op_end(); i != e; ++i) {
					Value *v = *i;
					if (!valVisited.count(v)) {
						DEBUG_MSG("\tadding to backtrack: " << *v << '\n');
						valBacktrack.push(v);
					}
				} // for (User::op_iterator)
			} // dyn_cast<Instruction>
			else if (isa<Constant>(val)) {
				continue;
			} else if (isa<Argument>(val)) {
				// for arguments, when checking the return of the function, we do
				// not need to trace out of the function. This function is used
				// when the following is encountered in another function:
				// %call1 = call <func>
				// We already know the location where the function is used so we do
				// not need to to go visit all the uses of the function
			} else {
				errs() << "[ERROR] Unhandled value on use-def chain: " << *val << '\n';
				assert(0 && "unhandled value on use-def chain");
			}

		} // while (!valBacktrack.empty())
	} // while (!funcBacktrack.empty())
	DEBUG_MSG("addFuncReturn complete\n");
}

std::vector<Value *> AssertSlice::handleAlloca(const CallInst *assertc, AllocaInst *ali) {
	assert(assertc && "NULL assert passed");
	assert(ali && "NULL alloca passed");

	DEBUG_MSG_RED("[MK DEBUG] Handling Alloca on use-def\n");

	// Return value: operands from store instructions who's
	std::vector<Value *> useCheck;

	// Keep track of instructions we have already iterated over
	static std::set<std::pair<const CallInst *, AllocaInst *> > paramVisited;
	std::vector<Value *> ret;

	std::pair<const CallInst *, AllocaInst *> param;
	param = std::make_pair(assertc, ali);
	if (paramVisited.count(param)) {
		return useCheck;
	}
	paramVisited.insert(param);

	std::set<Instruction *> &sli = SliceMap_[assertc];

	// We handle GetElementPointer instructions on the def-use chain of the
	// alloca inst in the same way as an alloca instruction itself so we have a
	// recursive process.
	std::stack<Value *> backtrack;
	std::set<Value *> visited;

	backtrack.push(ali);

	while (!backtrack.empty()) {
		Value *val = backtrack.top();
		backtrack.pop();

		// iterate over just the immediate def-use chain: we are only looking for
		// store instructions directly into the address of memory from the alloca
		// instruction.
		// TODO: Probably need to track the def-use chain of getelementpointer
		// instructions, but i'll leave that for when this crashes.
		for (Value::use_iterator vi = val->use_begin(), ve = val->use_end(); vi != ve; ++vi) {
			Value *use = *vi;
			if (StoreInst * str = dyn_cast<StoreInst>(use)) {
				DEBUG_MSG("\t store instruction found: " << *str << '\n');
				if (val == str->getOperand(1)) {
					// alloca instruction on operand 1
					useCheck.push_back(str->getOperand(0));
					sliceInsert(sli, str);
				} else {
					assert(val == str->getOperand(0) && "val not on 0th or 1st operand of store");
					// we don't need to track if the alloca is stored somewhere else.
					// It the value that is being stored into is important then it will
					// be on the slice else where
				}
			}
			// Do nothing for load instructions: these will already be on the use-def
			// chain that got us here; (following data dependencies)
			//else if (LoadInst *load = dyn_cast<LoadInst>(use)) {
			//DEBUG_MSG("Load Inst: " << *load << '\n');
			//assert(0 && "unhandled: load inst used alloca");
			//}
			else if (GetElementPtrInst * gep = dyn_cast<GetElementPtrInst>(use)) {
				// Consider the entire memory allocated by the alloca as a contiguous
				// unit. Basically, we need to follow the stores to the result of the
				// getelement pointer instruction in the same way as we are doing
				// with the alloca
				DEBUG_MSG("GEP found on alloca chain: " << *gep << '\n');
				if (!visited.count(gep)) {
					visited.insert(gep);
					backtrack.push(gep);
				}
			}
		} // for (Value::use_iterator)
	} // while (!backtrack.empty())
	return useCheck;
}

bool AssertSlice::ignoreInst(Instruction *inst) const {
	if (isa<CallInst>(inst) || isa<InvokeInst>(inst)) {
		CallSite cs = CallSite(inst);
		Function *f = cs.getCalledFunction();
		if (f == NULL) {
			return false;
		}
		if (skipFunction(f)) {
			return true;
		}
		std::string name = f->getName();
		//name = name.substr(0, 5);
		if (name.substr(0, 5) == "clap_") {
			return true;
		}
	}
	return false;
}

bool AssertSlice::checkFunction(Function *f) const {

	if (f == NULL) {
		return false;
	}
	if (!f->hasName()) {
		return false;
	}
	std::string name = f->getName().str();

	if (kModule->getUserFunctionNames().count(name)) {
		return true;
	}
	return false;
}

bool AssertSlice::skipFunction(const Function *f) const {
	// ignore indirect functions
	if (f == NULL)
		return false;
	std::string name = f->getName();

#ifdef SKIP_KLEE

	if (name.substr(0, sizeof("__uClibc_init") - 1) == "__uClibc_init") {
		return true;
	}
	if (name.substr(0, sizeof("__uClibc_fini") - 1) == "__uClibc_fini") {
		return true;
	}
	if (name.substr(0, sizeof("__setutent") - 1) == "__setutent") {
		return true;
	}
	if (name.substr(0, sizeof("endutent") - 1) == "endutent") {
		return true;
	}
	if (name.substr(0, sizeof("__getutent") - 1) == "__getutent") {
		return true;
	}
	if (name.substr(0, sizeof("pututline") - 1) == "pututline") {
		return true;
	}
	if (name.substr(0, sizeof("utmpname") - 1) == "utmpname") {
		return true;
	}
	if (name.substr(0, sizeof("__stdio_READ") - 1) == "__stdio_READ") {
		return true;
	}
	if (name.substr(0, sizeof("__stdio_WRITE") - 1) == "__stdio_WRITE") {
		return true;
	}
	if (name.substr(0, sizeof("exit") - 1) == "exit") {
		return true;
	}
	if (name.substr(0, sizeof("_fpmaxtostr") - 1) == "_fpmaxtostr") {
		return true;
	}
	if (name.substr(0, sizeof("pthread_") - 1) == "pthread_") {
		return true;
	}
	if (name.substr(0, sizeof("__close_fds") - 1) == "__close_fds") {
		return true;
	}
	if (name.substr(0, sizeof("sendfile") - 1) == "sendfile") {
		return true;
	}
	if (name.substr(0, sizeof("_read_file") - 1) == "_read_file") {
		return true;
	}
	if (name.substr(0, sizeof("_write_file") - 1) == "_write_file") {
		return true;
	}
	if (name.substr(0, sizeof("_open_symbolic") - 1) == "_open_symbolic") {
		return true;
	}
	if (name.substr(0, sizeof("__klee_model_creat") - 1) == "__klee_model_creat") {
		return true;
	}
	if (name.substr(0, sizeof("mmap") - 1) == "mmap") {
		return true;
	}
	if (name.substr(0, sizeof("_scatter_read") - 1) == "_scatter_read") {
		return true;
	}
	if (name.substr(0, sizeof("_is_blocking") - 1) == "_is_blocking") {
		return true;
	}
	if (name.substr(0, sizeof("_is_blocking_file") - 1) == "_is_blocking_file") {
		return true;
	}
	if (name.substr(0, sizeof("_clean_read") - 1) == "_clean_read") {
		return true;
	}
	if (name.substr(0, sizeof("_gather_write") - 1) == "_gather_write") {
		return true;
	}
	if (name.substr(0, sizeof("_clean_write") - 1) == "_clean_write") {
		return true;
	}
	if (name.substr(0, sizeof("__klee_model_close") - 1) == "__klee_model_close") {
		return true;
	}
	if (name.substr(0, sizeof("__klee") - 1) == "__klee") {
		return true;
	}
	if (name.substr(0, sizeof("_stdio_term") - 1) == "_stdio_term") {
		return true;
	}
	if (name.substr(0, sizeof("_IO_getc") - 1) == "_IO_getc") {
		return true;
	}
	if (name.substr(0, sizeof("_IO_putc") - 1) == "_IO_putc") {
		return true;
	}
	if (name.substr(0, sizeof("_stat_file") - 1) == "_stat_file") {
		return true;
	}
	if (name.substr(0, sizeof("_open_concrete") - 1) == "_open_concrete") {
		return true;
	}
	if (name.substr(0, sizeof("_netlink_handler") - 1) == "_netlink_handler") {
		return true;
	}
	if (name.substr(0, sizeof("__user_main") - 1) == "__user_main") {
		return false;
	}
	if (name.substr(0, sizeof("__argless_main") - 1) == "__argless_main") {
		return false;
	}
//	if (name.substr(0, sizeof("__uClibc_main") - 1) == "__uClibc_main") {
//		return false;
//	}
	if (name.substr(0, sizeof("_") - 1) == "_") {
		return true;
	}
	if (name.substr(0, sizeof("__") - 1) == "__") {
		return true;
	}
	if (name.substr(0, sizeof("_open_symbolic") - 1) == "_open_symbolic") {
		return true;
	}
	if (name.substr(0, sizeof("fflush_unlocked") - 1) == "fflush_unlocked") {
		return true;
	}
	if (name.substr(0, sizeof("getcwd") - 1) == "getcwd") {
		return true;
	}
	if (name.substr(0, sizeof("getutxent") - 1) == "getutxent") {
		return true;
	}
	if (name.substr(0, sizeof("waitid") - 1) == "waitid") {
		return true;
	}
	if (name.substr(0, sizeof("waitpid") - 1) == "waitid") {
		return true;
	}
	if (name.substr(0, sizeof("klee_init_fdt") - 1) == "klee_init_fdt") {
		return true;
	}
	if (name.substr(0, sizeof("munmap") - 1) == "munmap") {
		return true;
	}
	if (name.substr(0, sizeof("getaddrinfo") - 1) == "getaddrinfo") {
		return true;
	}
	if (name.substr(0, sizeof("getnameinfo") - 1) == "getnameinfo") {
		return true;
	}
	if (name.substr(0, sizeof("klee_init_") - 1) == "klee_init_") {
		return true;
	}
	if (name.substr(0, sizeof("_close_socket") - 1) == "_close_socket") {
		return true;
	}
	if (name.substr(0, sizeof("accept") - 1) == "accept") {
		return true;
	}
	if (name.substr(0, sizeof("_register_events_socket") - 1) == "_register_events_socket") {
		return true;
	}
	if (name.substr(0, sizeof("socketpair") - 1) == "socketpair") {
		return true;
	}
	if (name.substr(0, sizeof("connect") - 1) == "connect") {
		return true;
	}
	if (name.substr(0, sizeof("socket") - 1) == "socket") {
		return true;
	}
	if (name.substr(0, sizeof("sendmsg") - 1) == "sendmsg") {
		return true;
	}
	if (name.substr(0, sizeof("recvmsg") - 1) == "recvmsg") {
		return true;
	}
	if (name.substr(0, sizeof("getsockname") - 1) == "getsockname") {
		return true;
	}
	if (name.substr(0, sizeof("recvfrom") - 1) == "recvfrom") {
		return true;
	}
	if (name.substr(0, sizeof("getpeername") - 1) == "getpeername") {
		return true;
	}
	if (name.substr(0, sizeof("shutdown") - 1) == "shutdown") {
		return true;
	}
	if (name.substr(0, sizeof("send") - 1) == "send") {
		return true;
	}
	if (name.substr(0, sizeof("recv") - 1) == "recv") {
		return true;
	}
	if (name.substr(0, sizeof("sendto") - 1) == "sendto") {
		return true;
	}
	if (name.substr(0, sizeof("getsocketopt") - 1) == "getsocketopt") {
		return true;
	}
	if (name.substr(0, sizeof("freeaddrinfo") - 1) == "freeaddrinfo") {
		return true;
	}
	if (name.substr(0, sizeof("klee_range") - 1) == "klee_range") {
		return true;
	}
	if (name.substr(0, sizeof("memmove") - 1) == "memmove") {
		return true;
	}
	if (name.substr(0, sizeof("gnu_dev_makedev112") - 1) == "gnu_dev_makedev112") {
		return true;
	}
	if (name.substr(0, sizeof("getutent") - 1) == "getutent") {
		return true;
	}
	if (name.substr(0, sizeof("getutid") - 1) == "getutid") {
		return true;
	}
	if (name.substr(0, sizeof("getutline") - 1) == "getutline") {
		return true;
	}
	if (name.substr(0, sizeof("realpath") - 1) == "realpath") {
		return true;
	}
	if (name.substr(0, sizeof("fstat") - 1) == "fstat") {
		return true;
	}
	if (name.substr(0, sizeof("memcpy") - 1) == "memcpy") {
		return true;
	}
	if (name.substr(0, sizeof("memset") - 1) == "memset") {
		return true;
	}
	if (name.substr(0, sizeof("readlink") - 1) == "readlink") {
		return true;
	}
	if (name.substr(0, sizeof("strcat") - 1) == "strcat") {
		return true;
	}
	if (name.substr(0, sizeof("strcmp") - 1) == "strcmp") {
		return true;
	}
	if (name.substr(0, sizeof("strdup") - 1) == "strdup") {
		return true;
	}
	if (name.substr(0, sizeof("strlen") - 1) == "strlen") {
		return true;
	}
	if (name.substr(0, sizeof("strncmp") - 1) == "strncmp") {
		return true;
	}
	if (name.substr(0, sizeof("fseek") - 1) == "fseek") {
		return true;
	}
	if (name.substr(0, sizeof("isatty") - 1) == "isatty") {
		return true;
	}
	if (name.substr(0, sizeof("fseeko64") - 1) == "fseeko64") {
		return true;
	}
	if (name.substr(0, sizeof("tcgetattr") - 1) == "tcgetattr") {
		return true;
	}
	if (name.substr(0, sizeof("ioctl") - 1) == "ioctl") {
		return true;
	}
	if (name.substr(0, sizeof("mempcpy") - 1) == "mempcpy") {
		return true;
	}
	if (name.substr(0, sizeof("mkdir") - 1) == "mkdir") {
		return true;
	}
	if (name.substr(0, sizeof("mkfifo") - 1) == "mkfifo") {
		return true;
	}
	if (name.substr(0, sizeof("mknod") - 1) == "mknod") {
		return true;
	}
	if (name.substr(0, sizeof("link") - 1) == "link") {
		return true;
	}
	if (name.substr(0, sizeof("symlink") - 1) == "symlink") {
		return true;
	}
	if (name.substr(0, sizeof("rename") - 1) == "rename") {
		return true;
	}
	if (name.substr(0, sizeof("clock_gettime") - 1) == "clock_gettime") {
		return true;
	}
	if (name.substr(0, sizeof("time") - 1) == "time") {
		return true;
	}
	if (name.substr(0, sizeof("times") - 1) == "times") {
		return true;
	}
	if (name.substr(0, sizeof("srand") - 1) == "srand") {
		return true;
	}
	if (name.substr(0, sizeof("random") - 1) == "random") {
		return true;
	}
	if (name.substr(0, sizeof("vfprintf") - 1) == "vfprintf") {
		return true;
	}
	if (name.substr(0, sizeof("fwrite_unlocked") - 1) == "fwrite_unlocked") {
		return true;
	}
#endif

	return false;
}

std::vector<CallInst *> AssertSlice::filterAsserts(std::vector<CallInst *> asserts) {
	std::vector<CallInst *> ret;
	for (size_t i = 0; i < asserts.size(); ++i) {
		Function *f = asserts[i]->getParent()->getParent();
		if (!skipFunction(f)) {
			ret.push_back(asserts[i]);
		}
		DEBUG_MSG("Assert in function: " << ret[i]->getParent()->getParent()->getName() << '\n');
	}

	return ret;
}

std::vector<Value *> AssertSlice::pruneHappensAfter(std::vector<Value *> vals, Instruction *inst) {
	assert(0 && "unimplimented");
	assert(inst && "NULL inst passed");
	assert(vals.size() && "vector size zero passed");
	std::vector<Value *> ret;

	// Create a set containing all the instructions in the current function up
	// to the passed instruction. Then check if the set contains the passed
	// values.
	std::set<Instruction *> before;
	Function *f = inst->getParent()->getParent();

	assert(f && "instruction not in function");

	DEBUG_MSG("Pruning based on happens-after\n");
	DEBUG_MSG("Base Inst: " << *inst << '\n');

	// Generate before set
	for (inst_iterator I = inst_begin(f), E = inst_end(f); I != E; ++I) {
		Instruction *cur = &(*I);
		before.insert(cur);
		if (cur == inst) {
			break;
		}
	}

	// Only keep values in vals that are in the before set
	for (std::vector<Value *>::iterator I = vals.begin(), E = vals.end(); I != E; ++I) {
		Value *cur = *I;
		if (Instruction * inst = dyn_cast<Instruction>(cur)) {
			if (before.count(inst)) {
				// in set
				ret.push_back(inst);
			} else {
				DEBUG_MSG("\thappens-after pruned!\n");
			}
		} else if (isa<Argument>(cur)) {
			// let the caller deal with arguments being in the passed vector
			ret.push_back(cur);
		} else if (isa<Constant>(cur)) {
			// let the caller deal with constants being in the passed vector
			ret.push_back(cur);
		} else {
			DEBUG_MSG("Value: " << *cur << '\n');
			assert(0 && "Unhandled Value");
		}
	} // end for

	return ret;
}

//#ifdef STATIC_ALIAS
//std::map<const CallInst *, std::set<Instruction *> > AssertSlice::extendWithAlias(std::map<const CallInst *, std::set<Instruction *> > sliceMap, Module &M) {
//	//AliasAnalysis* AA = getAnalysisIfAvailable<AliasAnalysis>();
//	AliasAnalysis* AA = Pass::getAnalysisIfAvailable<AliasAnalysis>();
//	if (!AA) {
//		assert(0 && "Alias analysis not availible");
//	}
//	// compare every instruction in the map to every other instruction and build up the alias sets
//	for (std::map<const CallInst *, std::set<Instruction *> >::iterator mit = sliceMap.begin(); mit != sliceMap.end(); ++mit) {
//		std::set<Instruction *> &slice = mit->second;
//		// iterate over the set of instructions in the slice for this assertion
//		for (std::set<Instruction *>::iterator sit = slice.begin(); sit != slice.end(); ++sit) {
//			Instruction *sliceInst = *sit;
//			// iterate over every function in the program
//			for (Module::iterator fit = M.begin(); fit != M.end(); ++fit) {
//				// iterate over every instruction in the function
//				Function &F = *fit;
//				if (!checkFunction(&F))
//					continue;
//				if (sliceInst->getParent()->getParent() == &F)
//					continue;
//				for (inst_iterator iit = inst_begin(&F); iit != inst_end(&F); ++iit) {
//					Instruction *I = &(*iit);
//					Value *Pointer = NULL;
//					uint64_t ValueSize = AliasAnalysis::UnknownSize;
//					// check if the instruction is a type where aliasing is relevant
//					if (StoreInst * S = dyn_cast<StoreInst>(I)) {
//						Pointer = S->getPointerOperand();
//						Type* ValueType = S->getValueOperand()->getType();
//						ValueSize = AA->getTypeStoreSize(ValueType);
//					} else if (LoadInst * S = dyn_cast<LoadInst>(I)) {
//						Pointer = S->getPointerOperand();
//						Type* ValueType = S->getType();
//						ValueSize = AA->getTypeStoreSize(ValueType);
//					} else if (AtomicCmpXchgInst * S = dyn_cast<AtomicCmpXchgInst>(I)) {
//						Pointer = S->getPointerOperand();
//						Type* ValueType = S->getType();
//						ValueSize = AA->getTypeStoreSize(ValueType);
//					} else if (AtomicRMWInst * S = dyn_cast<AtomicRMWInst>(I)) {
//						Pointer = S->getPointerOperand();
//						Type* ValueType = S->getType();
//						ValueSize = AA->getTypeStoreSize(ValueType);
//					} else {
//						continue;
//					}
//					// check if they alias, update the map if they do
//					AliasAnalysis::AliasResult R = AA->alias(Pointer, ValueSize, sliceInst, ~0U);
//					if (R != AliasAnalysis::NoAlias) {
//						// Since we did the dynamic checks previously, Pointer is an instruction
//						slice.insert(I);
//						LLVMContext &C = I->getParent()->getParent()->getContext();
//						Value* elts[] = { ConstantInt::get(C, APInt(1, 1)) };
//						MDNode* md_node = MDNode::get(C, elts);
//						I->setMetadata("Intra_Procedural_Inst", md_node);
//
//						errs() << "Maybe Alias, func name: " << I->getParent()->getParent()->getName().str() << "\n";
//						I->dump();
//
////						LLVMContext &C = I->getParent()->getParent()->getContext();
////						Value* elts[] = { ConstantInt::get(C, APInt(1, 1)) };
////						MDNode* md_node = MDNode::get(C, elts);
////						if (I->getParent()->getParent()->hasName()) {
////							I->setMetadata(I->getParent()->getParent()->getName(), md_node);
////						}
//					}
//				} // for (inst_iterator ... )
//			} // for (Module::iterator ...)
//		} // for (std::set<Instruction *> ...)
//	} // for (std::map<const CallInst ...)
//	return sliceMap;
//}
//#endif

void AssertSlice::addLoadAndStoreAlias(const CallInst *assertCall, const std::set<Instruction *> loadsAndStores) {
	assert(loadsAndStores.size() != 0);
	assert(assertCall != NULL);

	// Slice for the current assertion
	std::set<Instruction *> &slice = SliceMap_[assertCall];

	static std::set<Instruction *> visited;

	// iterate over each load and store
	for (std::set<Instruction *>::iterator i = loadsAndStores.begin(), ie = loadsAndStores.end(); i != ie; ++i) {
		Instruction *loadStore = *i;

		if (visited.count(loadStore)) {
			continue;
		}
		visited.insert(loadStore);
		std::set<Instruction *> aliasSet = getAliasSet(loadStore);

		// iterate over alias set
		for (std::set<Instruction *>::iterator j = aliasSet.begin(), je = aliasSet.end(); j != je; ++j) {
			Instruction *currInst = *j;
			BasicBlock *currBlock = currInst->getParent();
			// Get the function containing the current alias set item
			Function *currFunc = currBlock->getParent();

			// add the aliasing statement to the slice
			this->sliceInsert(slice, currInst);

			// get control dependencies of the function containing the alias set
			// member
			ControlDependence cdep;
			PostDominatorTree &PDT = getAnalysis<PostDominatorTree>(*currFunc);
			cdep.getControlDependencies(*currFunc, PDT);

			// vector of Values encoutered in this functions CDG which should have
			// their data dependencies checked
			std::vector<Value *> dataCheck;
			dataCheck = addCdepToSlice(assertCall, cdep, currBlock);
			if (dataCheck.size()) {
				addDdepToSlice(assertCall, dataCheck);
			}
		} // for (j)
	} // for (i)
}

std::set<Instruction *> AssertSlice::getAliasSet(Instruction *inst) {
	std::set<Instruction *> ret;
	AliasAnalysis* AA = Pass::getAnalysisIfAvailable<AliasAnalysis>();
	if (!AA) {
		assert(0 && "Alias analysis not availible");
	}

	// Get the pointer value of the passed instruction
	Value *loadStorePtr = NULL;
	uint64_t loadStoreSize = AliasAnalysis::UnknownSize;
	if (StoreInst * S = dyn_cast<StoreInst>(inst)) {
		loadStorePtr = S->getPointerOperand();
		Type* ValueType = S->getValueOperand()->getType();
		loadStoreSize = AA->getTypeStoreSize(ValueType);
	} else if (LoadInst * S = dyn_cast<LoadInst>(inst)) {
		loadStorePtr = S->getPointerOperand();
		Type* ValueType = S->getType();
		loadStoreSize = AA->getTypeStoreSize(ValueType);
	} else if (AtomicCmpXchgInst * S = dyn_cast<AtomicCmpXchgInst>(inst)) {
		loadStorePtr = S->getPointerOperand();
		Type* ValueType = S->getType();
		loadStoreSize = AA->getTypeStoreSize(ValueType);
	} else if (AtomicRMWInst * S = dyn_cast<AtomicRMWInst>(inst)) {
		loadStorePtr = S->getPointerOperand();
		Type* ValueType = S->getType();
		loadStoreSize = AA->getTypeStoreSize(ValueType);
	} else {
		assert(0 && "non load/store passed to getAliasSet");
	}

	int rounds = RoundsOfRuns;
	int counter = 1;
	// iterate over every function in the program
	Module &M = *inst->getParent()->getParent()->getParent();
	for (Module::iterator fit = M.begin(); fit != M.end(); ++fit) {
		// iterate over every instruction in the function
		Function &F = *fit;
		if (!checkFunction(&F)) {
			continue;
		}
		if (inst->getParent()->getParent() == &F) {
			continue;
		}

		if (RoundsOfRuns) {
			if (++counter > rounds) {
				break;
			}
		}

		for (inst_iterator iit = inst_begin(&F); iit != inst_end(&F); ++iit) {
			Instruction *I = &(*iit);
			Value *Pointer = NULL;
			uint64_t ValueSize = AliasAnalysis::UnknownSize;
			// check if the instruction is a type where aliasing is relevant
			if (StoreInst * S = (dyn_cast<StoreInst>(I))) {
				Pointer = S->getPointerOperand();
				Type* ValueType = S->getValueOperand()->getType();
				ValueSize = AA->getTypeStoreSize(ValueType);
			} else if (LoadInst * S = (dyn_cast<LoadInst>(I))) {
				Pointer = S->getPointerOperand();
				Type* ValueType = S->getType();
				ValueSize = AA->getTypeStoreSize(ValueType);
			} else if (AtomicCmpXchgInst * S = (dyn_cast<AtomicCmpXchgInst>(I))) {
				Pointer = S->getPointerOperand();
				Type* ValueType = S->getType();
				ValueSize = AA->getTypeStoreSize(ValueType);
			} else if (AtomicRMWInst * S = (dyn_cast<AtomicRMWInst>(I))) {
				Pointer = S->getPointerOperand();
				Type* ValueType = S->getType();
				ValueSize = AA->getTypeStoreSize(ValueType);
			} else {
				continue;
			}
			// check if they alias
			AliasAnalysis::AliasResult R = AA->alias(Pointer, ValueSize, loadStorePtr, loadStoreSize);
			if (R) {
				ret.insert(I);
			}
		} // for (inst_iter)
	} // for (module)
	return ret;
}

} // namespace

char klee::AssertSlice::ID = 0;
static RegisterPass<klee::AssertSlice> X("assert-slice", "slice on program assertions", true, /* does not modify CFG */
true); /* analysis pass */
