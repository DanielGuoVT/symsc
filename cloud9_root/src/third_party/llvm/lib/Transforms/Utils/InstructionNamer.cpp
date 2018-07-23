//===- InstructionNamer.cpp - Give anonymous instructions names -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is a little utility pass that gives instructions names, this is mostly
// useful when diffing the effect of an optimization because deleting an
// unnamed instruction can change all other instruction numbering, making the
// diff very noisy.
//
//===----------------------------------------------------------------------===//

/*
 #include "llvm/Transforms/Scalar.h"
 #include "llvm/Function.h"
 #include "llvm/Pass.h"
 #include "llvm/Type.h"

 using namespace llvm;
 namespace {
 struct InstNamer : public FunctionPass {
 static char ID; // Pass identification, replacement for typeid
 InstNamer() : FunctionPass(ID) {
 initializeInstNamerPass(*PassRegistry::getPassRegistry());
 }

 void getAnalysisUsage(AnalysisUsage &Info) const {
 Info.setPreservesAll();
 }

 bool runOnFunction(Function &F) {
 for (Function::arg_iterator AI = F.arg_begin(), AE = F.arg_end();
 AI != AE; ++AI)
 if (!AI->hasName() && !AI->getType()->isVoidTy())
 AI->setName("arg");

 for (Function::iterator BB = F.begin(), E = F.end(); BB != E; ++BB) {
 if (!BB->hasName())
 BB->setName("bb");

 for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I)
 if (!I->hasName() && !I->getType()->isVoidTy())
 I->setName("tmp");
 }
 return true;
 }
 };

 char InstNamer::ID = 0;
 }

 INITIALIZE_PASS(InstNamer, "instnamer",
 "Assign names to anonymous instructions", false, false)
 char &llvm::InstructionNamerID = InstNamer::ID;
 //===----------------------------------------------------------------------===//
 //
 // InstructionNamer - Give any unnamed non-void instructions "tmp" names.
 //
 FunctionPass *llvm::createInstructionNamerPass() {
 return new InstNamer();
 }
 */

#define DEBUG_TYPE "insert-Inst-Id"

#include "llvm/Transforms/Scalar.h"
#include "llvm/Module.h"
#include "llvm/Instructions.h"
#include "llvm/IntrinsicInst.h"
#include "llvm/Function.h"
#include "llvm/Pass.h"
#include "llvm/Type.h"

// sjguo
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CallSite.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace llvm;
using namespace std;

STATISTIC(InstCounter, "Counts number of instructions greeted");

namespace {
struct InstNamer: public ModulePass {
  static char ID;  // Pass identification, replacement for typeid
  InstNamer()
      : ModulePass(ID) {
    initializeInstNamerPass(*PassRegistry::getPassRegistry());
  }

  bool runOnModule(Module &M) {

    int arg_counter = 1;
    int inst_counter = 1;
    int bb_counter = 1;

    for (Module::iterator F = M.begin(); F != M.end(); F++) {
#if 1 // add the clap_id
      for (Function::iterator B = F->begin(); B != F->end(); B++) {
        for (BasicBlock::iterator I = B->begin(); I != B->end(); I++) {
          LLVMContext &C = B->getParent()->getParent()->getContext();
          Value* elts[] = { ConstantInt::get(C, APInt(32, ++InstCounter)) };
          MDNode* md_node = MDNode::get(C, elts);
          I->setMetadata("clap", md_node);
        }
      }
#endif


      bool flag = false;

      for (Function::arg_iterator AI = F->arg_begin(), AE = F->arg_end();
          AI != AE; ++AI) {
        if (!AI->getType()->isVoidTy()) {
          if (!AI->hasName()) {
            flag = true;
          } else if (!AI->getName().startswith("wp_arg")) {
            flag = true;
          }
          if (flag) {
            std::string s = static_cast<ostringstream*>(&(ostringstream()
                << arg_counter++))->str();
            std::string name = "wp_arg" + s;
            AI->setName(name);
            flag = false;
          }
        }
      }

      for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
        if (!BB->hasName()) {
          flag = true;
        } else {
          if (!BB->getName().startswith("wp_bb")) {
            flag = true;
          }
        }
        if (flag) {
          std::string s = static_cast<ostringstream*>(&(ostringstream()
              << bb_counter++))->str();
          std::string name = "wp_bb" + s;
          BB->setName(name);
          flag = false;
        }

        for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I) {
          if (!I->getType()->isVoidTy()) {
            if (!I->hasName()) {
              flag = true;
            } else if (!I->getName().startswith("wp_inst")) {
              flag = true;
            }

            if (flag) {
              std::string s = static_cast<ostringstream*>(&(ostringstream()
                  << inst_counter++))->str();
              std::string name = "wp_inst" + s;
              I->setName(name);
            }
          }
        }
      }

    }
    return true;
  }
};

char InstNamer::ID = 0;
}

INITIALIZE_PASS(InstNamer, "instnamer",
                "Assign names to anonymous instructions", false, false)
char &llvm::InstructionNamerID = InstNamer::ID;
//===----------------------------------------------------------------------===//
//
// InstructionNamer - Give any unnamed non-void instructions "tmp" names.
//
ModulePass *llvm::createInstructionNamerPass() {
  return new InstNamer();
}

