//===-- Memory.h ------------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_MEMORY_H
#define KLEE_MEMORY_H

#include "Context.h"
#include "klee/Expr.h"
#include "klee/Internal/Module/KInstruction.h"
#include "AddressSpace.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/Instruction.h"
#include "llvm/Function.h"

#include <vector>
#include <string>

using namespace llvm;

namespace llvm {
class Value;
}

namespace klee {

class BitArray;
class MemoryManager;
class Solver;

class MemoryObject {
	friend class STPBuilder;

private:
	static int counter;

public:
	unsigned id;
	uint64_t address;

	/// size in bytes
	unsigned size;
	mutable std::string name;

	bool isLocal;
	mutable bool isGlobal;
	bool isFixed;

	/// true if created by us, Daniel
	bool isUserSpecified;

	/// true if is dynamically allocated in execution, daniel.
	bool isDynAllocated;
	/// shift to re-map binary address, daniel.
	unsigned shift;

	/// "Location" for which this memory object was allocated. This
	/// should be either the allocating instruction or the global object
	/// it was allocated for (or whatever else makes sense).
	const llvm::Value *allocSite;

	/// A list of boolean expressions the user has requested be true of
	/// a counterexample. Mutable since we play a little fast and loose
	/// with allowing it to be added to during execution (although
	/// should sensibly be only at creation time).
	mutable std::vector<klee::ref<Expr> > cexPreferences;

	// DO NOT IMPLEMENT
	MemoryObject(const MemoryObject &b);
	MemoryObject &operator=(const MemoryObject &b);

public:

	// XXX this is just a temp hack, should be removed
	explicit MemoryObject(uint64_t _address) :
			id(counter++), address(_address), size(0), isFixed(true), isDynAllocated(false), allocSite(0) {
	}

	MemoryObject(uint64_t _address, unsigned _size, bool _isLocal, bool _isGlobal, bool _isFixed, const llvm::Value *_allocSite) :
			id(counter++), address(_address), size(_size), name("unnamed"), isLocal(_isLocal), isGlobal(_isGlobal), isFixed(_isFixed), isUserSpecified(
					false), isDynAllocated(false), allocSite(_allocSite) {
	}

	~MemoryObject();

	/// Get an identifying string for this allocation.
	template<class OStream>
	void getAllocInfo(OStream &info, bool fast = false) const {
		info << "MO" << id << "[" << size << "]";

		if (allocSite) {
			info << " allocated at ";
			if (const Instruction *i = (dyn_cast<Instruction>(allocSite))) {
				info << i->getParent()->getParent()->getName();
				if (fast) {
					info << "()";
				} else {
					info << "():" << *i;
				}
			} else if (const GlobalValue *gv = dyn_cast<GlobalValue>(allocSite)) {
				info << "global:" << gv->getName();
			} else {
				info << "value:" << *allocSite;
			}
		} else {
			info << " (no allocation info)";
		}
	}

	void getAllocInfo(std::string &result, bool fast = false) const;

	void setName(std::string newName) const {
		name = newName;
	}

	std::string getName() {
		return name;
	}

	void setAllocFlag(bool flag) {
		this->isDynAllocated = flag;
	}

	klee::ref<ConstantExpr> getBaseExpr() const {
		return ConstantExpr::create(address, Context::get().getPointerWidth());
	}
	klee::ref<ConstantExpr> getSizeExpr() const {
		return ConstantExpr::create(size, Context::get().getPointerWidth());
	}
	klee::ref<Expr> getOffsetExpr(klee::ref<Expr> pointer) const {
		return SubExpr::create(pointer, getBaseExpr());
	}
	klee::ref<Expr> getBoundsCheckPointer(klee::ref<Expr> pointer) const {
		return getBoundsCheckOffset(getOffsetExpr(pointer));
	}
	klee::ref<Expr> getBoundsCheckPointer(klee::ref<Expr> pointer, unsigned bytes) const {
		return getBoundsCheckOffset(getOffsetExpr(pointer), bytes);
	}

	klee::ref<Expr> getBoundsCheckOffset(klee::ref<Expr> offset) const {
		if (size == 0) {
			return EqExpr::create(offset, ConstantExpr::alloc(0, Context::get().getPointerWidth()));
		} else {
			return UltExpr::create(offset, getSizeExpr());
		}
	}
	klee::ref<Expr> getBoundsCheckOffset(klee::ref<Expr> offset, unsigned bytes) const {
		if (bytes <= size) {
			return UltExpr::create(offset, ConstantExpr::alloc(size - bytes + 1, Context::get().getPointerWidth()));
		} else {
			return ConstantExpr::alloc(0, Expr::Bool);
		}
	}
};

class ObjectState {
private:
	friend class AddressSpace;
	unsigned copyOnWriteOwner; // exclusively for AddressSpace

	friend class ObjectHolder;
	unsigned refCount;

	const MemoryObject *object;

	uint8_t *concreteStore;
	// XXX cleanup name of flushMask (its backwards or something)
	BitArray *concreteMask;

	// mutable because may need flushed during read of const
	mutable BitArray *flushMask;

	klee::ref<Expr> *knownSymbolics;

	// mutable because we may need flush during read of const
	mutable UpdateList updates;

public:
	unsigned size;

	bool readOnly;

	bool isShared; // The object is shared among addr. spaces within the same state

public:
	/// Create a new object state for the given memory object with concrete
	/// contents. The initial contents are undefined, it is the callers
	/// responsibility to initialize the object contents appropriately.
	ObjectState(const MemoryObject *mo);

	/// Create a new object state for the given memory object with symbolic
	/// contents.
	ObjectState(const MemoryObject *mo, const Array *array);

	ObjectState(const ObjectState &os);
	~ObjectState();

	const MemoryObject *getObject() const {
		return object;
	}

	void setReadOnly(bool ro) {
		readOnly = ro;
	}

	// make contents all concrete and zero
	void initializeToZero();
	// make contents all concrete and random
	void initializeToRandom();

	klee::ref<Expr> read(klee::ref<Expr> offset, Expr::Width width) const;
	klee::ref<Expr> read(unsigned offset, Expr::Width width) const;
	klee::ref<Expr> read8(unsigned offset) const;

	// return bytes written.
	void write(unsigned offset, klee::ref<Expr> value);
	void write(klee::ref<Expr> offset, klee::ref<Expr> value);

	void write8(unsigned offset, uint8_t value);
	void write16(unsigned offset, uint16_t value);
	void write32(unsigned offset, uint32_t value);
	void write64(unsigned offset, uint64_t value);

	const UpdateList readAll() const;

	// for tapPLC, reset symbolic
	void resetSymbolic();

private:
	const UpdateList &getUpdates() const;

	void makeConcrete();

	void makeSymbolic();

	klee::ref<Expr> read8(klee::ref<Expr> offset) const;
	void write8(unsigned offset, klee::ref<Expr> value);
	void write8(klee::ref<Expr> offset, klee::ref<Expr> value);

	void fastRangeCheckOffset(klee::ref<Expr> offset, unsigned *base_r, unsigned *size_r) const;
	void flushRangeForRead(unsigned rangeBase, unsigned rangeSize) const;
	void flushRangeForWrite(unsigned rangeBase, unsigned rangeSize);

	bool isByteConcrete(unsigned offset) const;
	bool isByteFlushed(unsigned offset) const;
	bool isByteKnownSymbolic(unsigned offset) const;

	void markByteConcrete(unsigned offset);
	void markByteSymbolic(unsigned offset);
	void markByteFlushed(unsigned offset);
	void markByteUnflushed(unsigned offset);
	void setKnownSymbolic(unsigned offset, Expr *value);

	void print();
};

} // End klee namespace

#endif
