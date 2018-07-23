//===-- Passes.h ------------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_PASSES_H
#define KLEE_PASSES_H

#include "klee/Config/config.h"
#include "klee/Internal/Module/KModule.h"

#include "llvm/Constants.h"
#include "llvm/Instructions.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/IntrinsicLowering.h"

#include <map>
#include <vector>
#include <set>

#include "ControlDependence.h"

namespace llvm {
class Function;
class Instruction;
class Module;
class TargetData;
class TargetLowering;
class Type;
}

using namespace llvm;
using namespace std;

namespace klee {

/// RaiseAsmPass - This pass raises some common occurences of inline
/// asm which are used by glibc into normal LLVM IR.
class RaiseAsmPass: public llvm::ModulePass {
	static char ID;

	const llvm::TargetLowering *TLI;

	llvm::Function *getIntrinsic(llvm::Module &M, unsigned IID, llvm::Type **Tys, unsigned NumTys);
	llvm::Function *getIntrinsic(llvm::Module &M, unsigned IID, llvm::Type *Ty0) {
		return getIntrinsic(M, IID, &Ty0, 1);
	}

	bool runOnInstruction(llvm::Module &M, llvm::Instruction *I);

public:
	RaiseAsmPass() :
			llvm::ModulePass(ID) {
	}

	virtual bool runOnModule(llvm::Module &M);
};

// This is a module pass because it can add and delete module
// variables (via intrinsic lowering).
class IntrinsicCleanerPass: public llvm::ModulePass {
	static char ID;
	const llvm::TargetData &TargetData;
	llvm::IntrinsicLowering *IL;
	bool LowerIntrinsics;

	bool runOnBasicBlock(llvm::BasicBlock &b);
public:
	IntrinsicCleanerPass(const llvm::TargetData &TD, bool LI = true) :
			llvm::ModulePass(ID), TargetData(TD), IL(new llvm::IntrinsicLowering(TD)), LowerIntrinsics(LI) {
	}
	~IntrinsicCleanerPass() {
		delete IL;
	}

	virtual bool runOnModule(llvm::Module &M);
};

class ThrowCleanerPass: public llvm::ModulePass {
	static char ID;

	bool runOnBasicBlock(llvm::BasicBlock &b);
public:
	ThrowCleanerPass() :
			llvm::ModulePass(ID) {
	}

	virtual bool runOnModule(llvm::Module &M);
};

class LowerSSEPass: public llvm::ModulePass {
	static char ID;

	bool runOnBasicBlock(llvm::BasicBlock &b);
public:
	LowerSSEPass() :
			llvm::ModulePass(ID) {
	}

	virtual bool runOnModule(llvm::Module &M);
};

class SIMDInstrumentationPass: public llvm::ModulePass {
	static char ID;

	bool runOnBasicBlock(llvm::BasicBlock &b);
public:
	SIMDInstrumentationPass() :
			llvm::ModulePass(ID) {
	}

	virtual bool runOnModule(llvm::Module &M);
};

// performs two transformations which make interpretation
// easier and faster.
//
// 1) Ensure that all the PHI nodes in a basic block have
//    the incoming block list in the same order. Thus the
//    incoming block index only needs to be computed once
//    for each transfer.
//
// 2) Ensure that no PHI node result is used as an argument to
//    a subsequent PHI node in the same basic block. This allows
//    the transfer to execute the instructions in order instead
//    of in two passes.
class PhiCleanerPass: public llvm::FunctionPass {
	static char ID;

public:
	PhiCleanerPass() :
			llvm::FunctionPass(ID) {
	}

	virtual bool runOnFunction(llvm::Function &f);
};

class DivCheckPass: public llvm::ModulePass {
	static char ID;
public:
	DivCheckPass() :
			ModulePass(ID) {
	}
	virtual bool runOnModule(llvm::Module &M);
};

/// LowerSwitchPass - Replace all SwitchInst instructions with chained branch
/// instructions.  Note that this cannot be a BasicBlock pass because it
/// modifies the CFG!
class LowerSwitchPass: public llvm::FunctionPass {
public:
	static char ID; // Pass identification, replacement for typeid
	LowerSwitchPass() :
			FunctionPass(ID) {
	}

	virtual bool runOnFunction(llvm::Function &F);

	struct SwitchCase {
		llvm::Value *value;
		llvm::BasicBlock *block;

		SwitchCase() :
				value(0), block(0) {
		}
		SwitchCase(llvm::Value *v, llvm::BasicBlock *b) :
				value(v), block(b) {
		}
	};

	typedef std::vector<SwitchCase> CaseVector;
	typedef std::vector<SwitchCase>::iterator CaseItr;

private:
	void processSwitchInst(llvm::SwitchInst *SI);
	void switchConvert(CaseItr begin, CaseItr end, llvm::Value *value, llvm::BasicBlock *origBlock, llvm::BasicBlock *defaultBlock);
};

//// struct AssertSlice

// This pass is a module pass for now. I chose this with the idea that the
// pass could be asked the question: "What are the dependencies of this
// instruction?"
struct AssertSlice: public ModulePass {
	static char ID;

	AssertSlice() :
			ModulePass(ID) {
	}

	KModule *kModule;
	void setKModule(KModule* km) {
		kModule = km;
	}

	// ControlDependence object. Holds and calculates the control dependency
	// information
	//ControlDependence ControlDep;

	// Map of __assert_fail() calls to the set of instructions in the
	// interprocedural slice of the assert call. These are the control
	// instructions that control the execution of the __assert_fail() call and
	// the instructions flow dependent on the condition of the control
	// instructions
	std::map<const CallInst *, std::set<Instruction *> > SliceMap_;

	// Helper printing functions
	static std::string sliceToString(const std::set<Instruction *> &sl);
	static std::string slMapToString(const std::map<const CallInst *, std::set<Instruction *> > &slm);

	virtual bool runOnModule(Module &M);

	virtual void getAnalysisUsage(AnalysisUsage &AU) const {
		// For control dependence analysis
		AU.addRequired<PostDominatorTree>();

		AU.setPreservesAll();
	}

	// return a vector of all calls to __assert_fail found in the passed module
	std::vector<CallInst *> findAssertFail(Module &M) const;

	// add the passed control dependence information to the slice (see
	// SliceMap_) of the passed __assert_fail call. The passed BasicBlock Sb is
	// the starting block to traverse from in the control dependence graph.
	//
	// Returns all of the Values which should have their data dependencies
	// checked (e.g. conditions in branch statements)
	std::vector<Value *> addCdepToSlice(const CallInst *assertCall, const ControlDependence &cdep, BasicBlock *Sb);

	// Follow the use-def chains of each value passed and add them to the
	// slice (SliceMap_) of the passed assert call
	void addDdepToSlice(const CallInst *assertCall, std::vector<Value *> vals);

	// Return a vector which is vals except removing Values which occur after
	// inst. By happening after, I mean the sequential order of instructions in
	// the current function.
	std::vector<Value *> pruneHappensAfter(std::vector<Value *> vals, Instruction *inst);

	// Return the value that should have its data dependencies checked
	// associated with the passed terminator instruction. This could be, for
	// example, the condition of a branch instruction.
	//
	// Return NULL for terminator instructions which have no Values of
	// interest.
	Value *getCond(TerminatorInst *tinst);

	// Checkif any of the items in the slice of assertCall could be influenced
	// by a parameter of the function. This is done by walking the def-use
	// chain of every parameter of the passed function.
	//
	// A set of argument positions is returned. Each position represents an
	// argument of the function that leaks into the slice. If none are found, a
	// defaul constructed set is returned (i.e. size == 0)
	//
	// This takes care of situations like the following:
	//
	// define void @A(i32 %a) nounwind uwtable {
	//  %a.addr = alloca i32, align 4
	//  store i32 %a, i32* %a.addr, align 4
	//  %0 = load i32* %a.addr, align 4, !dbg !21
	//
	// if the load is being examined, then the proper entry on the use-def
	// chain of a.addr is the alloca instruction. The store instruction does
	// not _define_ a.addr but simply _uses_ it. By walking the def-use chain
	// of `a` we can see that it intersects with a.addr so the slice needs to
	// be interprocedural.
	std::set<unsigned> argumentLeak(const CallInst *assertCall, Function *func);

	// Return the position the passed argument is located in the argument list
	// of the passed function. This corresponds to the position of the argument
	// in a CallInst when using getArgOperand().
	unsigned argumentPosition(const Argument *arg, const Function *func) const;

	// Update the slice of assertc with the callsites of func using the
	// arguments specified in argpos.
	void addFuncCallToSlice(const CallInst *assertc, std::set<unsigned> argpos, Function *func);

	// Follow the def-use chain of the passed value and add it to the slice of
	// the passed assertion. Returns a vector of instructions who's uses should
	// be followed (right now, this is only store instructions)
	std::vector<Instruction *> addDefUseToSlice(const CallInst *assertc, Value *v);

	// Add the passed instruction to the slice. This also updates the metadata
	// of the instruction to mark it as being on an assert slice
	void sliceInsert(std::set<Instruction *> &sli, Instruction *i) const;

	// Output the instruction IDs on the slices to a file. This requires that
	// each instruction has a "clap" metadata integer which is assumed to be
	// the unique instruction ID.
	//
	// The output will have one instruction ID per line. The filename will be:
	//
	// <module name>.slices.id
	//
	void clapInstIDOut(const std::string &moduleName) const;

	// Helper function: returns the clap instruction ID of the passed
	// instruction. Assumes that the ID exists and is valid.
	uint64_t getInstID(const Instruction *i) const;

	// Follow the return statements(s) of the passed function and add any
	// dependencies to the slice of the passed assertion. This does not follow
	// the chains out of the function when the reach the function parameters;
	// this is to be used when a function call is found in another function
	// (i.e., you already know the exact location where the function is being
	// use)
	void addFuncReturnToSlice(const CallInst *assertc, Function *f);

	// Returns true if the passed instruction is one that should be ignroed.
	// Currently, this is:
	//
	// Calls to clap_ instructions.
	// Calls to functions without bodies
	// Also skips calls to functions specified in skipFunction (see below)
	bool ignoreInst(Instruction *inst) const;

	// skip certain functions
	// Currently, these are:
	// __uClibc*
	bool skipFunction(const Function *f) const;

	// remove assert calls from the passed vector if they are in a function
	// that should be skipped
	std::vector<CallInst *> filterAsserts(std::vector<CallInst *> asserts);

	// Handle an Alloca instruction found on the use-def chain of the passed
	// assertion. This will do the following:
	//
	// 1. Add any stores to the address of the alloca instruction to the slice
	// 2. Return a vector of  stored into the address of the alloca inst
	// (these should have their use-def chain's added to the slice)
	// TODO: Handle getelementpointer instructions
	std::vector<Value *> handleAlloca(const CallInst *assertc, AllocaInst *ali);

//	// Attempt to obtain an alias analysis and then extend the passed map to
//	// statically build the alias results
//	std::map<const CallInst *, std::set<Instruction *> >
//	extendWithAlias(std::map<const CallInst *, std::set<Instruction *> > sliceMap, Module &M);

	// not skip certain functions
	bool checkFunction(Function *f) const;

	// For each load/store instruction, add all the control/data dependencies
	// to the slice of the passed assertion
	void addLoadAndStoreAlias(const CallInst * assertCall, const std::set<Instruction *> loadsAndStores);

	// returns the alias set of the passed instruction (assumes it is some kind
	// of memory accessing instruction (e.g., load,store,atomic cmpxchng))
	std::set<Instruction *> getAliasSet(Instruction *inst);

	// Load/Store instructions which need to have any aliasing statements (and
	// their control/data dependencies) added to the slice
	std::set<Instruction *> loadAndStoreInsts;

	void runAssertSlicing(Module &M);

};
// struct AssertSlice

}

#endif
