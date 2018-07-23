//===-- MemoryManager.h -----------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_MEMORYMANAGER_H
#define KLEE_MEMORYMANAGER_H

#include <vector>
#include <stdint.h>

namespace llvm {
  class Value;
}

namespace klee {
  class MemoryObject;

  class MemoryManager {
  private:
    typedef std::vector<MemoryObject*> objects_ty;
    objects_ty objects;

  public:
    MemoryManager() {}
    ~MemoryManager();

    MemoryObject *allocate(ExecutionState *state, uint64_t size, bool isLocal, bool isGlobal,
                           const llvm::Value *allocSite);

    MemoryObject *allocate(ExecutionState *state, uint64_t size, bool isLocal, bool isGlobal,
                               const llvm::Value *allocSite, uint64_t binaryAddr);

    MemoryObject *allocateFixed(uint64_t address, uint64_t size,
                                const llvm::Value *allocSite);
    void deallocate(const MemoryObject *mo);

    objects_ty& getMemoryObjects(){
    	return this->objects;
    }
  };

} // End klee namespace

#endif
