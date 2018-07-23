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

#include "klee/StackTrace.h"

#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/KModule.h"

#include "llvm/Function.h"

#include <iomanip>

using namespace llvm;

namespace klee {

void StackTrace::dump(std::ostream &out) const {
  unsigned idx = 0;

  for (stack_t::const_iterator it = contents.begin(); it != contents.end(); it++) {
    Function *f = it->first.first->function;
    const InstructionInfo &ii = *it->first.second->info;

    out << "\t#" << idx++
        << " " << std::setw(8) << std::setfill('0') << ii.assemblyLine
        << " in " << f->getName().str() << " (";

    unsigned index = 0;
    for (Function::arg_iterator ai = f->arg_begin(), ae = f->arg_end();
         ai != ae; ++ai) {
      if (ai!=f->arg_begin()) out << ", ";

      out << ai->getName().str();
      // XXX should go through function
      klee::ref<Expr> value = it->second[index++];
      if (isa<ConstantExpr>(value))
        out << "=" << value;
    }
    out << ")";
    if (ii.file != "")
      out << " at " << ii.file << ":" << ii.line;
    out << "\n";
  }
}

void StackTrace::dumpInline(std::ostream &out) const {
  for (stack_t::const_iterator it = contents.begin(), ie = contents.end();
      it != ie; ++it) {
    Function *f = it->first.first->function;
    const InstructionInfo &ii = *it->first.second->info;

    if (it != contents.begin()) {
      out << '(' << ii.assemblyLine << ',' << ii.file << ':' << ii.line << ')';
      out << "]/[";
    } else {
      out << "[";
    }
    out << f->getName().str();
  }

  out << "]";

  // os << '(' << state.pc()->info->assemblyLine << ',' << state.pc()->info->file << ':' << state.pc()->info->line << ')';
}


//std::ostream &printStateConstraints(std::ostream &os, const ExecutionState &state) {
//  ExprPPrinter::printConstraints(os, state.constraints());
//
//  return os;
//}
//
//std::ostream &printStateMemorySummary(std::ostream &os,
//    const ExecutionState &state) {
//  const MemoryMap &mm = state.addressSpace().objects;
//
//  os << "{";
//  MemoryMap::iterator it = mm.begin();
//  MemoryMap::iterator ie = mm.end();
//  if (it != ie) {
//    os << "MO" << it->first->id << ":" << it->second;
//    for (++it; it != ie; ++it)
//      os << ", MO" << it->first->id << ":" << it->second;
//  }
//  os << "}";
//  return os;
//}

}
