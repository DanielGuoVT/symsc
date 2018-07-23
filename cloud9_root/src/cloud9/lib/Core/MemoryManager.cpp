//===-- MemoryManager.cpp -------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Memory.h"
#include "MemoryManager.h"

#include "klee/Executor.h"
#include "klee/ExecutionState.h"
#include "klee/Expr.h"
#include "klee/CoreStats.h"
#include "klee/Solver.h"

#include "llvm/Support/CommandLine.h"

using namespace klee;

/***/

MemoryManager::~MemoryManager() {
	while (!objects.empty()) {
		MemoryObject *mo = objects.back();
		objects.pop_back();
		delete mo;
	}
}

MemoryObject *MemoryManager::allocate(ExecutionState *state, uint64_t size, bool isLocal, bool isGlobal, const llvm::Value *allocSite) {
	if (size > 10 * 1024 * 1024) {
		LOG(WARNING) << "Failing large alloc: " << size << " bytes at " << *state;
		return 0;
	}
	uint64_t address = (uint64_t) state->addressPool.allocate(size);
	if (!address) return 0;

	++stats::allocations;
	MemoryObject *res = new MemoryObject(address, size, isLocal, isGlobal, false, allocSite);
	objects.push_back(res);
	return res;
}

MemoryObject *MemoryManager::allocate(ExecutionState *state, uint64_t size, bool isLocal, bool isGlobal, const llvm::Value *allocSite,
		uint64_t binaryAddr) {
	if (size > 10 * 1024 * 1024) {
		LOG(WARNING) << "Failing large alloc: " << size << " bytes at " << *state;
		return 0;
	}
	assert(binaryAddr && "binaryAddr should not be zero !");
	uint64_t startAddr = (uint64_t) state->addressPool.allocate(size + Executor::cacheSetNum * Executor::cacheLineSize * Executor::nway);

	if (!startAddr) {
		std::cerr << "failed to malloc memory in address pool, now return 0." << std::endl;
		return 0;
	}
	++stats::allocations;
	int shift = 0;
	int binaryCacheSet = 0;
	int cloud9CacheSet = 0;
	int newCloud9CacheSet = 0;
	uint64_t newAddr = 0;

	if (state->executor->nway == 1) {
		binaryCacheSet = state->executor->getCacheSet(binaryAddr);
		cloud9CacheSet = state->executor->getCacheSet(startAddr);
		shift = cloud9CacheSet < binaryCacheSet ? (binaryCacheSet - cloud9CacheSet) : (binaryCacheSet - cloud9CacheSet + Executor::cacheSetNum);
		newAddr = startAddr + Executor::cacheLineSize * shift;
		newCloud9CacheSet = state->executor->getCacheSet(newAddr);
		std::cerr << "Global var: " << allocSite->getName().str() << ", binary addr: " << binaryAddr << ", size: " << size << ", cache set: "
				<< binaryCacheSet << std::endl;
		std::cerr << "KLEE original address: " << startAddr << ", re-mapped address: " << newAddr << std::endl;
		std::cerr << "KLEE original cache set: " << cloud9CacheSet << ", re-mapped cache Set: " << newCloud9CacheSet << std::endl;
		assert((newCloud9CacheSet == binaryCacheSet) && "address rewriting failed");
	} else {
		binaryCacheSet = (binaryAddr >> 6) & 0xff;
		cloud9CacheSet = (startAddr >> 6) & 0xff;
		uint64_t offset = 0;

		if (binaryCacheSet - cloud9CacheSet < 0) {
			offset = (binaryCacheSet + Executor::cacheSetNum - cloud9CacheSet) << 6;
		} else {
			offset = (binaryCacheSet - cloud9CacheSet) << 6;
		}

		newAddr = startAddr + offset;
		newCloud9CacheSet = (newAddr >> 6) & 0xff;

		std::cerr << "Global var: " << allocSite->getName().str() << ", binary addr: " << binaryAddr << ", size: " << size << ", cache set: "
				<< binaryCacheSet << std::endl;
		std::cerr << "KLEE original address: " << startAddr << ", re-mapped address: " << newAddr << std::endl;
		std::cerr << "KLEE original cache set: " << cloud9CacheSet << ", re-mapped cache set: " << newCloud9CacheSet << std::endl;
		assert((newCloud9CacheSet == binaryCacheSet) && "address rewriting failed");
	}

	MemoryObject *res = new MemoryObject(newAddr, size, isLocal, isGlobal, false, allocSite);
	res->shift = Executor::cacheLineSize * shift;
	objects.push_back(res);
	return res;
}

MemoryObject *MemoryManager::allocateFixed(uint64_t address, uint64_t size, const llvm::Value *allocSite) {
#ifndef NDEBUG
	for (objects_ty::iterator it = objects.begin(), ie = objects.end(); it != ie; ++it) {
		MemoryObject *mo = *it;
		if (address + size > mo->address && address < mo->address + mo->size)
		LOG(FATAL) << "Trying to allocate an overlapping object";
	}
#endif

	++stats::allocations;
	MemoryObject *res = new MemoryObject(address, size, false, true, true, allocSite);
	objects.push_back(res);
	return res;
}

void MemoryManager::deallocate(const MemoryObject *mo) {
	assert(0);
}

