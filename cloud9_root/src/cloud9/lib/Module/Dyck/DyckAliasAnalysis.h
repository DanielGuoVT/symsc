/*
 * DyckAliasAnalysis.h
 *
 *  Created on: Jun 17, 2014
 *      Author: sjguo
 */

#ifndef DYCKALIASANALYSIS_H_
#define DYCKALIASANALYSIS_H_

#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/CaptureTracking.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/InstructionSimplify.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Target/TargetLibraryInfo.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/GetElementPtrTypeIterator.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

namespace klee{

llvm::ModulePass *createDyckAliasAnalysisPass();

}

#endif /* DYCKALIASANALYSIS_H_ */
