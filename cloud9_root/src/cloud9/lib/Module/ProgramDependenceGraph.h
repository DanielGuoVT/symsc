/*
 * ProgramDependenceGraph.h
 *
 *  Created on: Feb 13, 2015
 *      Author: sjguo
 */

#ifndef PROGRAMDEPENDENCEGRAPH_H_
#define PROGRAMDEPENDENCEGRAPH_H_

#include "llvm/DerivedTypes.h"
#include "llvm/Pass.h"
#include "llvm/Instruction.h"
#include "llvm/Instructions.h"
#include "llvm/IntrinsicInst.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"

#include "ControlDependence.h"
#include "ValToStrDB.h"
#include "klee/Utils.h"

#include <sstream>
#include <string>
#include <vector>

using namespace llvm;

namespace klee {

// Context insensitive program dependency pass
struct PDG: llvm::ModulePass {
	static char ID;
	ValToStrDB IDMap;

	// Vector populated with all the __assert_fail calls found during the
	// traversal of every instruction.
	//
	// The vector should only contain CallInsts or InvokeInsts
	std::set<Instruction *> assertCalls;
	std::set<Instruction *> modifiedInsts;

	PDG() :
			ModulePass(ID) {
	}

	void addSpec(raw_fd_ostream &f);

	virtual void getAnalysisUsage(AnalysisUsage &AU) const;

	// Given the passed name of a function, return the name of its return
	// instruction. This allows for other functions to setup alias links to the
	// return value
	std::string getFuncReturnName(StringRef funcName);

	// The passed string should be string IDs from a ValToStrDB. These will be
	// converted to integer IDs (bitvectors)
	std::string createCDep(std::string to, std::string from);

	// Create a control dependence from-->to in the passed ostream.
	std::string createCDep(Value *to, Value *from);

	// Remove one star from the value being dereferenced (from) and create an
	// assignment from-->to.
	//
	// Control dependencies will be created to the derefenced values from the
	// passed instruction
	std::string createStore(Value *to, Value *from, Instruction *i);

	// Create a memcpy from-->to. This will dereference both of the passed values
	std::string createMemCpy(Value *to, Value *from);

	// Create a data dependence from-->to assuming that the values come from a
	// load. This will dereference the from value.
	//
	// The passed instruction is the instruction actually performing the load.
	// The dereferenced values will be come control dependent on the passed
	// instruction. This ensures that the dereferenced values keep their parents
	// control dependencies
	std::string createLoad(Value *to, Value *from, Instruction *i);

	std::string createAssign(std::string to, std::string from);

	std::string createAssign(Value *to, Value *from);

	std::string visitAllocaInst(AllocaInst *I);

	std::string visitAtomicCmpXchg(AtomicCmpXchgInst *i);

	std::string visitGetElementPtrInst(GetElementPtrInst *i);

	std::string visitAtomicRMWInst(AtomicRMWInst *i);

	std::string visitStoreInst(StoreInst *i);

	std::string visitLoadInst(LoadInst *i);

	std::string visitReturnInst(ReturnInst *i);

	// Create a rule:
	// call(callSite, callee)
	// This means that the callSite calls the passed pointer to a callee
	std::string createCall(Value *callSite, Value *callee);

	std::string createCall(std::string cs, std::string callee);

	// returns true if the passed function type has the same types as the passed
	// vector of types. Order matters in the passed vector. This assumes that the
	// number of types in the passed vector and in the parameters of the passed
	// function are the same. Otherwise, this function will crash
	bool paramsMatch(FunctionType *fTy, std::vector<Type *> const tys);

	// Creates a conditional assignment rule:
	//
	// (rule (=> (points-to toPtr fromPtr) (assign to fromStr)))
	//
	// Or, to is assigned fromStr (to <-- fromStr) if toPtr points to fromPtr
	// (toPtr --> fromPtr)
	std::string createCondAssign(Value *toPtr, Value* fromPtr, Value* to, std::string fromStr);

	std::string createCondAssign(Value *toPtr, Value* fromPtr, Value* to, Value *from);

	std::string createCondAssign(std::string toPtr, std::string fromPtr, std::string toStr, std::string fromStr);

	// Create a rule: (points-to to from)
	//
	// Converts to bitvectors
	std::string createPointsTo(std::string to, std::string from);

	// Given a string: (rule (...)) remove the "(rule ) and the last paren
	std::string removeRule(std::string s);

	// Given the passed callsite, return the matching set of functions which
	// could be called from the callsite.
	// If the called value is a literal function (i.e., not a function pointer)
	// then the result will be a vector of size one containing this value.
	//
	// Otherwise, the return will be the set of all functions in the module which
	// have the same number of arguments and the same type of arguments.
	//
	// If no functions can be found, then this function will crash.
	std::vector<Function *> getMatchingFunctions(CallSite cs);

	std::string handleCallSite(CallSite I);

	virtual bool runOnModule(Module &M);

	// Write a query asking: "What is program dependent to the passed instruction?" VarName
	// is the name of the Z3 variable to use in the universal quantification.
	// The variable will first be defined and then used in the query.
	void writeUniversalQuery(Instruction *i, std::string varName, raw_fd_ostream *os);

	void writeFwdQuery(Instruction *i, std::string varName, raw_fd_ostream *os);

};

}

#endif /* PROGRAMDEPENDENCEGRAPH_H_ */
