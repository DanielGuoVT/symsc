//===-- Expr.h --------------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_EXPR_H
#define KLEE_EXPR_H

#include "klee/util/Bits.h"
#include "klee/util/Ref.h"

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/SmallVector.h"

#include "cloud9/Utils.h"

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <iosfwd> // FIXME: Remove this!!!

using namespace std;
using namespace klee;

namespace llvm {
class Type;
}

namespace klee {

class Array;
class ConstantExpr;
class ObjectState;

template<class T> class ref;

/// Class representing symbolic expressions.
/**

 <b>Expression canonicalization</b>: we define certain rules for
 canonicalization rules for Exprs in order to simplify code that
 pattern matches Exprs (since the number of forms are reduced), to open
 up further chances for optimization, and to increase similarity for
 caching and other purposes.

 The general rules are:
 <ol>
 <li> No Expr has all constant arguments.</li>

 <li> Booleans:
 <ol type="a">
 <li> \c Ne, \c Ugt, \c Uge, \c Sgt, \c Sge are not used </li>
 <li> The only acceptable operations with boolean arguments are
 \c Not \c And, \c Or, \c Xor, \c Eq,
 as well as \c SExt, \c ZExt,
 \c Select and \c NotOptimized. </li>
 <li> The only boolean operation which may involve a constant is boolean not (<tt>== false</tt>). </li>
 </ol>
 </li>

 <li> Linear Formulas:
 <ol type="a">
 <li> For any subtree representing a linear formula, a constant
 term must be on the LHS of the root node of the subtree.  In particular,
 in a BinaryExpr a constant must always be on the LHS.  For example, subtraction
 by a constant c is written as <tt>add(-c, ?)</tt>.  </li>
 </ol>
 </li>


 <li> Chains are unbalanced to the right </li>

 </ol>


 <b>Steps required for adding an expr</b>:0
 -# Add case to printKind
 -# Add to ExprVisitor
 -# Add to IVC (implied value concretization) if possible

 Todo: Shouldn't bool \c Xor just be written as not equal?

 */

class Expr {
public:
	static unsigned count;
	static const unsigned MAGIC_HASH_CONSTANT = 39;

	/// The type of an expression is simply its width, in bits.
	typedef unsigned Width;

	static const Width InvalidWidth = 0;
	static const Width Bool = 1;
	static const Width Int8 = 8;
	static const Width Int16 = 16;
	static const Width Int32 = 32;
	static const Width Int64 = 64;
	static const Width Fl80 = 80;

	enum Kind {
		InvalidKind = -1,

		// Primitive

		Constant = 0,

		// Special

		/// Prevents optimization below the given expression.  Used for
		/// testing: make equality constraints that KLEE will not use to
		/// optimize to concretes.
		NotOptimized,

		//// Skip old varexpr, just for deserialization, purge at some point
		Read = NotOptimized + 2,
		Select,
		Concat,
		Extract,

		// Casting,
		ZExt,
		SExt,

		// All subsequent kinds are binary.

		// Arithmetic
		Add,
		Sub,
		Mul,
		UDiv,
		SDiv,
		URem,
		SRem,

		// Bit
		Not,
		And,
		Or,
		Xor,
		Shl,
		LShr,
		AShr,

		// Compare
		Eq,
		Ne, ///< Not used in canonical form
		Ult,
		Ule,
		Ugt, ///< Not used in canonical form
		Uge, ///< Not used in canonical form
		Slt,
		Sle,
		Sgt, ///< Not used in canonical form
		Sge, ///< Not used in canonical form

		/// for wp expr usage, sjguo
//		WP,
//		WPRegular,
//		WPLoad,

		LastKind = Sge,

		CastKindFirst = ZExt,
		CastKindLast = SExt,
		BinaryKindFirst = Add,
		BinaryKindLast = Sge,
		CmpKindFirst = Eq,
		CmpKindLast = Sge
	};

	unsigned refCount;

protected:
	unsigned hashValue;

public:
	Expr() :
			refCount(0) {
		Expr::count++;
		this->instructionID = -1;
		this->moAddr = 0;
		this->instName = "";
		this->readWidth = 0;
	}
	virtual ~Expr() {
		Expr::count--;
	}

	virtual Kind getKind() const = 0;
	virtual Width getWidth() const = 0;

	virtual unsigned getNumKids() const = 0;
	virtual klee::ref<Expr> getKid(unsigned i) const = 0;

	virtual void print(std::ostream &os) const;

	/// dump - Print the expression to stderr.
	void dump() const;

	/// Returns the pre-computed hash of the current expression
	virtual unsigned hash() const {
		return hashValue;
	}

	/// (Re)computes the hash of the current expression.
	/// Returns the hash value.
	virtual unsigned computeHash();

	/// Returns 0 iff b is structuraly equivalent to *this
	int compare(const Expr &b) const;
	virtual int compareContents(const Expr &b) const {
		return 0;
	}

	// Given an array of new kids return a copy of the expression
	// but using those children.
	virtual klee::ref<Expr> rebuild(klee::ref<Expr> kids[/* getNumKids() */]) const = 0;

	//

	/// isZero - Is this a constant zero.
	bool isZero() const;

	/// isTrue - Is this the true expression.
	bool isTrue() const;

	/// isFalse - Is this the false expression.
	bool isFalse() const;

	/* Static utility methods */

	static void printKind(std::ostream &os, Kind k);
	static void printWidth(std::ostream &os, Expr::Width w);

	/// returns the smallest number of bytes in which the given width fits
	static inline unsigned getMinBytesForWidth(Width w) {
		return (w + 7) / 8;
	}

	/* Kind utilities */

	/* Utility creation functions */
	static klee::ref<Expr> createCoerceToPointerType(klee::ref<Expr> e);
	static klee::ref<Expr> createImplies(klee::ref<Expr> hyp, klee::ref<Expr> conc);
	static klee::ref<Expr> createIsZero(klee::ref<Expr> e);

	/// Create a little endian read of the given type at offset 0 of the
	/// given object.
	static klee::ref<Expr> createTempRead(const Array *array, Expr::Width w);

	static klee::ref<ConstantExpr> createPointer(uint64_t v);

	struct CreateArg;
	static klee::ref<Expr> createFromKind(Kind k, std::vector<CreateArg> args);

	static bool isValidKidWidth(unsigned kid, Width w) {
		return true;
	}
	static bool needsResultType() {
		return false;
	}

	static bool classof(const Expr *) {
		return true;
	}

	//utility variables and functions, sjguo
public:
	// the name of an instruction
	std::string instName;
	// the position of an instruction
	int instructionID;
	// the address of a MemoryObject
	uint64_t moAddr;
	// the width of read operation, if the expr is a WPLoadExpr
	Width readWidth;

	klee::ref<Expr> parent;

	unsigned size;

	std::string getName() {
		return instName;
	}
	void setName(std::string name) {
		instName = name;
	}
	int getInstID() {
		return instructionID;
	}
	void setInstID(int id) {
		instructionID = id;
	}
	uint64_t getMOAddr() {
		return moAddr;
	}
	void setMOAddr(uint64_t id) {
		moAddr = id;
	}
	void setReadWidth(Width type) {
		readWidth = type;
	}
	Width getReadWidth() {
		return readWidth;
	}

	virtual void setKid(unsigned i, klee::ref<Expr> e) {
	}

	static ref<Expr> deepCopy(ref<Expr> e, bool usc = false);
	static ref<Expr> copy1(ref<Expr> e, bool usc = false);
	static ref<Expr> copyExpr(ref<Expr> e, bool usc = false);

	static void replaceArray(ref<Expr> e, std::vector<std::pair<const Array*, const Array*> > &arrayPairs);

	uint64_t threadID;

	uint64_t getThreadID() {
		return threadID;
	}
};

struct Expr::CreateArg {
	klee::ref<Expr> expr;
	Width width;

	CreateArg(Width w = Bool) :
			expr(0), width(w) {
	}
	CreateArg(klee::ref<Expr> e) :
			expr(e), width(Expr::InvalidWidth) {
	}

	bool isExpr() {
		return !isWidth();
	}
	bool isWidth() {
		return width != Expr::InvalidWidth;
	}
};

// Comparison operators

inline bool operator==(const Expr &lhs, const Expr &rhs) {
	return lhs.compare(rhs) == 0;
}

inline bool operator<(const Expr &lhs, const Expr &rhs) {
	return lhs.compare(rhs) < 0;
}

inline bool operator!=(const Expr &lhs, const Expr &rhs) {
	return !(lhs == rhs);
}

inline bool operator>(const Expr &lhs, const Expr &rhs) {
	return rhs < lhs;
}

inline bool operator<=(const Expr &lhs, const Expr &rhs) {
	return !(lhs > rhs);
}

inline bool operator>=(const Expr &lhs, const Expr &rhs) {
	return !(lhs < rhs);
}

// Printing operators

inline std::ostream &operator<<(std::ostream &os, const Expr &e) {
	e.print(os);
	return os;
}

inline std::ostream &operator<<(std::ostream &os, const Expr::Kind kind) {
	Expr::printKind(os, kind);
	return os;
}

// Terminal Exprs

class ConstantExpr: public Expr {
public:
	static const Kind kind = Constant;
	static const unsigned numKids = 0;

private:
	llvm::APInt value;

	ConstantExpr(const llvm::APInt &v) :
			value(v) {
		size = 1;
	}

public:
	~ConstantExpr() {
	}

	Width getWidth() const {
		return value.getBitWidth();
	}
	Kind getKind() const {
		return Constant;
	}

	unsigned getNumKids() const {
		return 0;
	}
	klee::ref<Expr> getKid(unsigned i) const {
		return 0;
	}

	/// getAPValue - Return the arbitrary precision value directly.
	///
	/// Clients should generally not use the APInt value directly and instead use
	/// native ConstantExpr APIs.
	const llvm::APInt &getAPValue() const {
		return value;
	}

	/// getZExtValue - Return the constant value for a limited number of bits.
	///
	/// This routine should be used in situations where the width of the constant
	/// is known to be limited to a certain number of bits.
	uint64_t getZExtValue(unsigned bits = 64) const {
		assert(getWidth() <= bits && "Value may be out of range!");
		return value.getZExtValue();
	}

	uint64_t getSExtValue(unsigned bits = 64) const {
		assert(getWidth() <= bits && "Value may be out of range!");
		return value.getSExtValue();
	}

	/// getLimitedValue - If this value is smaller than the specified limit,
	/// return it, otherwise return the limit value.
	uint64_t getLimitedValue(uint64_t Limit = ~0ULL) const {
		return value.getLimitedValue(Limit);
	}

	/// toString - Return the constant value as a decimal string.
	void toString(std::string &Res) const;

	int compareContents(const Expr &b) const {
		const ConstantExpr &cb = static_cast<const ConstantExpr&>(b);
		if (getWidth() != cb.getWidth()) return getWidth() < cb.getWidth() ? -1 : 1;
		if (value == cb.value) return 0;
		return value.ult(cb.value) ? -1 : 1;
	}

	virtual klee::ref<Expr> rebuild(klee::ref<Expr> kids[]) const {
		assert(0 && "rebuild() on ConstantExpr");
		return (Expr*) this;
	}

	virtual unsigned computeHash();

	static klee::ref<Expr> fromMemory(void *address, Width w);
	void toMemory(void *address);

	static klee::ref<ConstantExpr> alloc(const llvm::APInt &v) {
		klee::ref<ConstantExpr> r(new ConstantExpr(v));
		r->computeHash();
		return r;
	}

	static klee::ref<ConstantExpr> alloc(const llvm::APFloat &f) {
		return alloc(f.bitcastToAPInt());
	}

	static klee::ref<ConstantExpr> alloc(uint64_t v, Width w) {
		return alloc(llvm::APInt(w, v));
	}

	static klee::ref<ConstantExpr> create(uint64_t v, Width w) {
		assert(v == bits64::truncateToNBits(v, w) && "invalid constant");
		return alloc(v, w);
	}

	static bool classof(const Expr *E) {
		return E->getKind() == Expr::Constant;
	}
	static bool classof(const ConstantExpr *) {
		return true;
	}

	/* Utility Functions */

	/// isZero - Is this a constant zero.
	bool isZero() const {
		return value.isMinValue();
	}

	/// isOne - Is this a constant one.
	bool isOne() const {
		return getLimitedValue() == 1;
	}

	/// isTrue - Is this the true expression.
	bool isTrue() const {
		return getZExtValue(1) == 1;
	}

	/// isFalse - Is this the false expression.
	bool isFalse() const {
		return getZExtValue(1) == 0;
	}

	/// isAllOnes - Is this constant all ones.
	bool isAllOnes() const {
		return value.isAllOnesValue();
	}

	/* Constant Operations */

	klee::ref<ConstantExpr> Concat(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Extract(unsigned offset, Width W);
	klee::ref<ConstantExpr> ZExt(Width W);
	klee::ref<ConstantExpr> SExt(Width W);
	klee::ref<ConstantExpr> Add(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Sub(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Mul(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> UDiv(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> SDiv(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> URem(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> SRem(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> And(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Or(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Xor(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Shl(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> LShr(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> AShr(const klee::ref<ConstantExpr> &RHS);

	// Comparisons return a constant expression of width 1.

	klee::ref<ConstantExpr> Eq(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Ne(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Ult(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Ule(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Ugt(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Uge(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Slt(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Sle(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Sgt(const klee::ref<ConstantExpr> &RHS);
	klee::ref<ConstantExpr> Sge(const klee::ref<ConstantExpr> &RHS);

	klee::ref<ConstantExpr> Neg();
	klee::ref<ConstantExpr> Not();
};

// Utility classes

class NonConstantExpr: public Expr {
public:
	static bool classof(const Expr *E) {
		return E->getKind() != Expr::Constant;
	}
	static bool classof(const NonConstantExpr *) {
		return true;
	}
};

class BinaryExpr: public NonConstantExpr {
public:
	klee::ref<Expr> left, right;

public:
	unsigned getNumKids() const {
		return 2;
	}
	klee::ref<Expr> getKid(unsigned i) const {
		if (i == 0) return left;
		if (i == 1) return right;
		return 0;
	}

protected:
	BinaryExpr(const klee::ref<Expr> &l, const klee::ref<Expr> &r) :
			left(l), right(r) {
		left->parent = this;
		right->parent = this;
		size = left->size + right->size;
	}

public:
	static bool classof(const Expr *E) {
		Kind k = E->getKind();
		return Expr::BinaryKindFirst <= k && k <= Expr::BinaryKindLast;
	}
	static bool classof(const BinaryExpr *) {
		return true;
	}

	//sjguo
public:
	void setKid(unsigned i, klee::ref<Expr> e) {
		assert(i==0 || i==1);
		if (i == 0) {
			left = e;
		} else {
			right = e;
		}
		e->parent = this;
	}
};

class CmpExpr: public BinaryExpr {

protected:
	CmpExpr(ref<Expr> l, ref<Expr> r) :
			BinaryExpr(l, r) {
	}

public:
	Width getWidth() const {
		return Bool;
	}

	static bool classof(const Expr *E) {
		Kind k = E->getKind();
		return Expr::CmpKindFirst <= k && k <= Expr::CmpKindLast;
	}
	static bool classof(const CmpExpr *) {
		return true;
	}
};

// Special

class NotOptimizedExpr: public NonConstantExpr {
public:
	static const Kind kind = NotOptimized;
	static const unsigned numKids = 1;
	klee::ref<Expr> src;

	static klee::ref<Expr> alloc(const klee::ref<Expr> &src) {
		klee::ref<Expr> r(new NotOptimizedExpr(src));
		r->computeHash();
		return r;
	}

	static klee::ref<Expr> create(klee::ref<Expr> src);

	Width getWidth() const {
		return src->getWidth();
	}
	Kind getKind() const {
		return NotOptimized;
	}

	unsigned getNumKids() const {
		return 1;
	}
	klee::ref<Expr> getKid(unsigned i) const {
		return src;
	}

	virtual klee::ref<Expr> rebuild(klee::ref<Expr> kids[]) const {
		return create(kids[0]);
	}
private:
	NotOptimizedExpr(const klee::ref<Expr> &_src) :
			src(_src) {
		src->parent = this;
		this->size = src->size;
	}

public:
	static bool classof(const Expr *E) {
		return E->getKind() == Expr::NotOptimized;
	}
	static bool classof(const NotOptimizedExpr *) {
		return true;
	}

	//sjguo
public:
	void setKid(unsigned i, klee::ref<Expr> e) {
		src = e;
		e->parent = this;
	}
};

/// Class representing a byte update of an array.
class UpdateNode {
	friend class UpdateList;
	friend class STPBuilder; // for setting STPArray

	mutable unsigned refCount;
	// gross
	mutable void *stpArray;
	// cache instead of recalc
	unsigned hashValue;

public:
	const UpdateNode *next;
	klee::ref<Expr> index, value;

private:
	/// size of this update sequence, including this update
	unsigned size;

public:
	UpdateNode(const UpdateNode *_next, const klee::ref<Expr> &_index, const klee::ref<Expr> &_value);

	unsigned getSize() const {
		return size;
	}

	int compare(const UpdateNode &b) const;
	unsigned hash() const {
		return hashValue;
	}

private:
	UpdateNode() :
			refCount(0), stpArray(0) {
	}
	~UpdateNode();

	unsigned computeHash();
};

/// Class representing a complete list of updates into an array.
class UpdateList {
	friend class ReadExpr; // for default constructor

public:
	const Array *root;

	/// pointer to the most recent update node
	const UpdateNode *head;

public:
	UpdateList(const Array *_root, const UpdateNode *_head);
	UpdateList(const UpdateList &b);
	~UpdateList();

	UpdateList &operator=(const UpdateList &b);

	/// size of this update list
	unsigned getSize() const {
		return (head ? head->getSize() : 0);
	}

	void extend(const klee::ref<Expr> &index, const klee::ref<Expr> &value);

	int compare(const UpdateList &b) const;
	unsigned hash() const;
};

class Array {
public:
	const std::string name;

	// FIXME: Not 64-bit clean.
	unsigned size;

	/// constantValues - The constant initial values for this array, or empty for
	/// a symbolic array. If non-empty, this size of this array is equivalent to
	/// the array size.
	const std::vector<klee::ref<ConstantExpr> > constantValues;

	// FIXME: This does not belong here.
	mutable void *stpInitialArray;

private:
	unsigned hashValue;

	/// The symbolic array singleton map is necessary to allow sharing
	/// of Arrays across states, essentially performing a (limited) form
	/// of alpha renaming.  Some Solvers use maps such as < const *Array,
	/// std::vector<unsigned> >.  This causes problems because stored
	/// answers can't be easily recovered.  Even worse, in read
	/// expressions, such as (read array 32), array is a pointer, and
	/// cached solutions are missed because the two Array instances
	/// aren't recognized as the same.
	static std::map<unsigned, std::vector<const Array *> *> symbolicArraySingletonMap;

public:
	static std::map<std::string, UpdateList*> updateListSingletonMap;

public:

	Expr::Width getDomain() const {
		return Expr::Int32;
	}
	Expr::Width getRange() const {
		return Expr::Int8;
	}

	/// Array - Construct a new array object.
	///
	/// \param _name - The name for this array. Names should generally be unique
	/// across an application, but this is not necessary for correctness, except
	/// when printing expressions. When expressions are printed the output will
	/// not parse correctly since two arrays with the same name cannot be
	/// distinguished once printed.
	Array(const std::string &_name, uint64_t _size, const ref<ConstantExpr> *constantValuesBegin = 0, const ref<ConstantExpr> *constantValuesEnd = 0) :
			name(_name), size(_size), constantValues(constantValuesBegin, constantValuesEnd), stpInitialArray(0) {
		assert((isSymbolicArray() || constantValues.size() == size) && "Invalid size for constant array!");
		computeHash();
#ifndef NDEBUG
		for (const ref<ConstantExpr> *it = constantValuesBegin; it != constantValuesEnd; ++it)
			assert((*it)->getWidth() == getRange() && "Invalid initial constant value!");
#endif //NDEBUG
	}

	~Array();

	bool isSymbolicArray() const {
		return constantValues.empty();
	}
	bool isConstantArray() const {
		return !isSymbolicArray();
	}
	const std::string getName() const {
		return name;
	}
	unsigned getSize() const {
		return size;
	}

	/// ComputeHash must take into account the name, the size, the domain, and the range
	unsigned computeHash();
	unsigned hash() const {
		return hashValue;
	}

	static const Array * CreateArray(const std::string &_name, uint64_t _size, const ref<ConstantExpr> *constantValuesBegin = 0,
			const ref<ConstantExpr> *constantValuesEnd = 0);
};

/// Class representing a one byte read from an array. 
class ReadExpr: public NonConstantExpr {
public:
	static const Kind kind = Read;
	static const unsigned numKids = 1;

public:
	UpdateList updates;
	klee::ref<Expr> index;

public:
	static klee::ref<Expr> alloc(const UpdateList &updates, const klee::ref<Expr> &index) {
		klee::ref<Expr> r(new ReadExpr(updates, index));
		r->computeHash();
		return r;
	}

	static klee::ref<Expr> create(const UpdateList &updates, klee::ref<Expr> i);

	Width getWidth() const {
		return Expr::Int8;
	}
	Kind getKind() const {
		return Read;
	}

	unsigned getNumKids() const {
		return numKids;
	}
	klee::ref<Expr> getKid(unsigned i) const {
		return !i ? index : 0;
	}

	int compareContents(const Expr &b) const;

	virtual klee::ref<Expr> rebuild(klee::ref<Expr> kids[]) const {
		return create(updates, kids[0]);
	}

	virtual unsigned computeHash();

private:
	ReadExpr(const UpdateList &_updates, const klee::ref<Expr> &_index) :
			updates(_updates), index(_index) {
		index->parent = this;
		size = _index->size;
	}

public:
	static bool classof(const Expr *E) {
		return E->getKind() == Expr::Read;
	}
	static bool classof(const ReadExpr *) {
		return true;
	}

	//sjguo
public:
	void setKid(unsigned i, klee::ref<Expr> e) {
		index = e;
		e->parent = this;
	}
};

/// Class representing an if-then-else expression.
class SelectExpr: public NonConstantExpr {
public:
	static const Kind kind = Select;
	static const unsigned numKids = 3;

public:
	klee::ref<Expr> cond, trueExpr, falseExpr;

public:
	static klee::ref<Expr> alloc(const klee::ref<Expr> &c, const klee::ref<Expr> &t, const klee::ref<Expr> &f) {
		klee::ref<Expr> r(new SelectExpr(c, t, f));
		r->computeHash();
		return r;
	}

	static klee::ref<Expr> create(klee::ref<Expr> c, klee::ref<Expr> t, klee::ref<Expr> f);

	Width getWidth() const {
		return trueExpr->getWidth();
	}
	Kind getKind() const {
		return Select;
	}

	unsigned getNumKids() const {
		return numKids;
	}
	klee::ref<Expr> getKid(unsigned i) const {
		switch (i) {
		case 0:
			return cond;
		case 1:
			return trueExpr;
		case 2:
			return falseExpr;
		default:
			return 0;
		}
	}

	void setKid(unsigned i, klee::ref<Expr> kid) {
		assert(i==0 || i==1 || i==2);
		switch (i) {
		case 0:
			cond = kid;
			break;
		case 1:
			trueExpr = kid;
			break;
		case 2:
			falseExpr = kid;
			break;
		default:
			assert(false);
			break;
		}
		kid->parent = this;
	}

	static bool isValidKidWidth(unsigned kid, Width w) {
		if (kid == 0) return w == Bool;
		else return true;
	}

	virtual klee::ref<Expr> rebuild(klee::ref<Expr> kids[]) const {
		return create(kids[0], kids[1], kids[2]);
	}

private:
	SelectExpr(const klee::ref<Expr> &c, const klee::ref<Expr> &t, const klee::ref<Expr> &f) :
			cond(c), trueExpr(t), falseExpr(f) {
		cond->parent = this;
		trueExpr->parent = this;
		falseExpr->parent = this;
		this->size = trueExpr->size + falseExpr->size + cond->size;
	}

public:
	static bool classof(const Expr *E) {
		return E->getKind() == Expr::Select;
	}
	static bool classof(const SelectExpr *) {
		return true;
	}
};

/** Children of a concat expression can have arbitrary widths.  
 Kid 0 is the left kid, kid 1 is the right kid.
 */
class ConcatExpr: public NonConstantExpr {
public:
	static const Kind kind = Concat;
	static const unsigned numKids = 2;

private:
	Width width;
	klee::ref<Expr> left, right;

public:
	static klee::ref<Expr> alloc(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
		klee::ref<Expr> c(new ConcatExpr(l, r));
		c->computeHash();
		return c;
	}

	static klee::ref<Expr> create(const klee::ref<Expr> &l, const klee::ref<Expr> &r);

	Width getWidth() const {
		return width;
	}
	Kind getKind() const {
		return kind;
	}
	klee::ref<Expr> getLeft() const {
		return left;
	}
	klee::ref<Expr> getRight() const {
		return right;
	}

	unsigned getNumKids() const {
		return numKids;
	}
	klee::ref<Expr> getKid(unsigned i) const {
		if (i == 0) return left;
		else if (i == 1) return right;
		else return NULL;
	}

	/// Shortcuts to create larger concats.  The chain returned is unbalanced to the right
	static klee::ref<Expr> createN(unsigned nKids, const klee::ref<Expr> kids[]);
	static klee::ref<Expr> create4(const klee::ref<Expr> &kid1, const klee::ref<Expr> &kid2, const klee::ref<Expr> &kid3,
			const klee::ref<Expr> &kid4);
	static klee::ref<Expr> create8(const klee::ref<Expr> &kid1, const klee::ref<Expr> &kid2, const klee::ref<Expr> &kid3, const klee::ref<Expr> &kid4,
			const klee::ref<Expr> &kid5, const klee::ref<Expr> &kid6, const klee::ref<Expr> &kid7, const klee::ref<Expr> &kid8);

	virtual klee::ref<Expr> rebuild(klee::ref<Expr> kids[]) const {
		return create(kids[0], kids[1]);
	}

private:
	ConcatExpr(const klee::ref<Expr> &l, const klee::ref<Expr> &r) :
			left(l), right(r) {
		width = l->getWidth() + r->getWidth();
		l->parent = this;
		r->parent = this;
		this->size = l->size + r->size;
	}

public:
	static bool classof(const Expr *E) {
		return E->getKind() == Expr::Concat;
	}
	static bool classof(const ConcatExpr *) {
		return true;
	}

	//sjguo
public:
	void setKid(unsigned i, klee::ref<Expr> e) {
		assert(i==0 || i==1);
		if (i == 0) {
			left = e;
		} else {
			right = e;
		}
		e->parent = this;
	}
};

/** This class represents an extract from expression {\tt expr}, at
 bit offset {\tt offset} of width {\tt width}.  Bit 0 is the right most
 bit of the expression.
 */
class ExtractExpr: public NonConstantExpr {
public:
	static const Kind kind = Extract;
	static const unsigned numKids = 1;

public:
	klee::ref<Expr> expr;
	unsigned offset;
	Width width;

public:
	static klee::ref<Expr> alloc(const klee::ref<Expr> &e, unsigned o, Width w) {
		klee::ref<Expr> r(new ExtractExpr(e, o, w));
		r->computeHash();
		return r;
	}

	/// Creates an ExtractExpr with the given bit offset and width
	static klee::ref<Expr> create(klee::ref<Expr> e, unsigned bitOff, Width w);

	Width getWidth() const {
		return width;
	}
	Kind getKind() const {
		return Extract;
	}

	unsigned getNumKids() const {
		return numKids;
	}
	klee::ref<Expr> getKid(unsigned i) const {
		return expr;
	}

	int compareContents(const Expr &b) const {
		const ExtractExpr &eb = static_cast<const ExtractExpr&>(b);
		if (offset != eb.offset) return offset < eb.offset ? -1 : 1;
		if (width != eb.width) return width < eb.width ? -1 : 1;
		return 0;
	}

	virtual klee::ref<Expr> rebuild(klee::ref<Expr> kids[]) const {
		return create(kids[0], offset, width);
	}

	virtual unsigned computeHash();

private:
	ExtractExpr(const klee::ref<Expr> &e, unsigned b, Width w) :
			expr(e), offset(b), width(w) {
		expr->parent = this;
		this->size = expr->size;
	}

public:
	static bool classof(const Expr *E) {
		return E->getKind() == Expr::Extract;
	}
	static bool classof(const ExtractExpr *) {
		return true;
	}

	//sjguo
public:
	void setKid(unsigned i, klee::ref<Expr> kid) {
		assert(i==0);
		expr = kid;
		kid->parent = this;
	}
};

/**
 Bitwise Not
 */
class NotExpr: public NonConstantExpr {
public:
	static const Kind kind = Not;
	static const unsigned numKids = 1;

	klee::ref<Expr> expr;

public:
	static klee::ref<Expr> alloc(const klee::ref<Expr> &e) {
		klee::ref<Expr> r(new NotExpr(e));
		r->computeHash();
		return r;
	}

	static klee::ref<Expr> create(const klee::ref<Expr> &e);

	Width getWidth() const {
		return expr->getWidth();
	}
	Kind getKind() const {
		return Not;
	}

	unsigned getNumKids() const {
		return numKids;
	}
	klee::ref<Expr> getKid(unsigned i) const {
		return expr;
	}

	int compareContents(const Expr &b) const {
		const NotExpr &eb = static_cast<const NotExpr&>(b);
		if (expr != eb.expr) return expr < eb.expr ? -1 : 1;
		return 0;
	}

	virtual klee::ref<Expr> rebuild(klee::ref<Expr> kids[]) const {
		return create(kids[0]);
	}

	virtual unsigned computeHash();

public:
	static bool classof(const Expr *E) {
		return E->getKind() == Expr::Not;
	}
	static bool classof(const NotExpr *) {
		return true;
	}

private:
	NotExpr(const klee::ref<Expr> &e) :
			expr(e) {
		expr->parent = this;
		this->size = expr->size;
	}

	//sjguo
public:
	void setKid(unsigned i, klee::ref<Expr> kid) {
		assert(i == 0);
		expr = kid;
		kid->parent = this;
	}
};

// Casting

class CastExpr: public NonConstantExpr {
public:
	klee::ref<Expr> src;
	Width width;

public:
	CastExpr(const klee::ref<Expr> &e, Width w) :
			src(e), width(w) {
		src->parent = this;
		this->size = src->size;
	}

	Width getWidth() const {
		return width;
	}

	unsigned getNumKids() const {
		return 1;
	}
	klee::ref<Expr> getKid(unsigned i) const {
		return (i == 0) ? src : 0;
	}

	static bool needsResultType() {
		return true;
	}

	int compareContents(const Expr &b) const {
		const CastExpr &eb = static_cast<const CastExpr&>(b);
		if (width != eb.width) return width < eb.width ? -1 : 1;
		return 0;
	}

	virtual unsigned computeHash();

	static bool classof(const Expr *E) {
		Expr::Kind k = E->getKind();
		return Expr::CastKindFirst <= k && k <= Expr::CastKindLast;
	}
	static bool classof(const CastExpr *) {
		return true;
	}

	//sjguo
public:
	void setKid(unsigned i, klee::ref<Expr> e) {
		assert(i==0);
		src = e;
		e->parent = this;
	}
};

#define CAST_EXPR_CLASS(_class_kind)                             \
class _class_kind ## Expr : public CastExpr {                    \
public:                                                          \
  static const Kind kind = _class_kind;                          \
  static const unsigned numKids = 1;                             \
public:                                                          \
    _class_kind ## Expr(klee::ref<Expr> e, Width w) : CastExpr(e,w) {} \
    static klee::ref<Expr> alloc(const klee::ref<Expr> &e, Width w) {        \
      klee::ref<Expr> r(new _class_kind ## Expr(e, w));                \
      r->computeHash();                                          \
      return r;                                                  \
    }                                                            \
    static klee::ref<Expr> create(const klee::ref<Expr> &e, Width w);        \
    Kind getKind() const { return _class_kind; }                 \
    virtual klee::ref<Expr> rebuild(klee::ref<Expr> kids[]) const {          \
      return create(kids[0], width);                             \
    }                                                            \
                                                                 \
    static bool classof(const Expr *E) {                         \
      return E->getKind() == Expr::_class_kind;                  \
    }                                                            \
    static bool classof(const  _class_kind ## Expr *) {          \
      return true;                                               \
    }                                                            \
};                                                               \

CAST_EXPR_CLASS(SExt)
CAST_EXPR_CLASS(ZExt)

// Arithmetic/Bit Exprs

#define ARITHMETIC_EXPR_CLASS(_class_kind)                           \
class _class_kind ## Expr : public BinaryExpr {                      \
public:                                                              \
  static const Kind kind = _class_kind;                              \
  static const unsigned numKids = 2;                                 \
public:                                                              \
    _class_kind ## Expr(const klee::ref<Expr> &l,                          \
                        const klee::ref<Expr> &r) : BinaryExpr(l,r) {}     \
    static klee::ref<Expr> alloc(const klee::ref<Expr> &l, const klee::ref<Expr> &r) { \
      klee::ref<Expr> res(new _class_kind ## Expr (l, r));                 \
      res->computeHash();                                            \
      return res;                                                    \
    }                                                                \
    static klee::ref<Expr> create(const klee::ref<Expr> &l, const klee::ref<Expr> &r); \
    Width getWidth() const { return left->getWidth(); }              \
    Kind getKind() const { return _class_kind; }                     \
    virtual klee::ref<Expr> rebuild(klee::ref<Expr> kids[]) const {              \
      return create(kids[0], kids[1]);                               \
    }                                                                \
                                                                     \
    static bool classof(const Expr *E) {                             \
      return E->getKind() == Expr::_class_kind;                      \
    }                                                                \
    static bool classof(const  _class_kind ## Expr *) {              \
      return true;                                                   \
    }                                                                \
};                                                                   \

ARITHMETIC_EXPR_CLASS(Add)
ARITHMETIC_EXPR_CLASS(Sub)
ARITHMETIC_EXPR_CLASS(Mul)
ARITHMETIC_EXPR_CLASS(UDiv)
ARITHMETIC_EXPR_CLASS(SDiv)
ARITHMETIC_EXPR_CLASS(URem)
ARITHMETIC_EXPR_CLASS(SRem)
ARITHMETIC_EXPR_CLASS(And)
ARITHMETIC_EXPR_CLASS(Or)
ARITHMETIC_EXPR_CLASS(Xor)
ARITHMETIC_EXPR_CLASS(Shl)
ARITHMETIC_EXPR_CLASS(LShr)
ARITHMETIC_EXPR_CLASS(AShr)

// Comparison Exprs

#define COMPARISON_EXPR_CLASS(_class_kind)                           \
class _class_kind ## Expr : public CmpExpr {                         \
public:                                                              \
  static const Kind kind = _class_kind;                              \
  static const unsigned numKids = 2;                                 \
public:                                                              \
    _class_kind ## Expr(const klee::ref<Expr> &l,                          \
                        const klee::ref<Expr> &r) : CmpExpr(l,r) {}        \
    static klee::ref<Expr> alloc(const klee::ref<Expr> &l, const klee::ref<Expr> &r) { \
      klee::ref<Expr> res(new _class_kind ## Expr (l, r));                 \
      res->computeHash();                                            \
      return res;                                                    \
    }                                                                \
    static klee::ref<Expr> create(const klee::ref<Expr> &l, const klee::ref<Expr> &r); \
    Kind getKind() const { return _class_kind; }                     \
    virtual klee::ref<Expr> rebuild(klee::ref<Expr> kids[]) const {              \
      return create(kids[0], kids[1]);                               \
    }                                                                \
                                                                     \
    static bool classof(const Expr *E) {                             \
      return E->getKind() == Expr::_class_kind;                      \
    }                                                                \
    static bool classof(const  _class_kind ## Expr *) {              \
      return true;                                                   \
    }                                                                \
};                                                                   \

COMPARISON_EXPR_CLASS(Eq)
COMPARISON_EXPR_CLASS(Ne)
COMPARISON_EXPR_CLASS(Ult)
COMPARISON_EXPR_CLASS(Ule)
COMPARISON_EXPR_CLASS(Ugt)
COMPARISON_EXPR_CLASS(Uge)
COMPARISON_EXPR_CLASS(Slt)
COMPARISON_EXPR_CLASS(Sle)
COMPARISON_EXPR_CLASS(Sgt)
COMPARISON_EXPR_CLASS(Sge)

//class SleExpr : public CmpExpr {
//public:
//  static const Kind kind = Expr::Sle;
//  static const unsigned numKids = 2;
//public:
//    SleExpr(const klee::ref<Expr> &l,
//                        const klee::ref<Expr> &r) : CmpExpr(l,r) {}
//    static klee::ref<Expr> alloc(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
//      klee::ref<Expr> res(new SleExpr (l, r));
//      res->computeHash();
//      return res;
//    }
//    static klee::ref<Expr> create(const klee::ref<Expr> &l, const klee::ref<Expr> &r);
//    Kind getKind() const { return Expr::Sle; }
//    virtual klee::ref<Expr> rebuild(klee::ref<Expr> kids[]) const {
//      return create(kids[0], kids[1]);
//    }
//
//    static bool classof(const Expr *E) {
//      return E->getKind() == Expr::Sle;
//    }
//    static bool classof(const  SleExpr *) {
//      return true;
//    }
//};

// Implementations

inline bool Expr::isZero() const {
	if (const ConstantExpr *CE = dyn_cast<ConstantExpr>(this)) return CE->isZero();
	return false;
}

inline bool Expr::isTrue() const {
	assert(getWidth() == Expr::Bool && "Invalid isTrue() call!");
	if (const ConstantExpr *CE = dyn_cast<ConstantExpr>(this)) return CE->isTrue();
	return false;
}

inline bool Expr::isFalse() const {
	assert(getWidth() == Expr::Bool && "Invalid isFalse() call!");
	if (const ConstantExpr *CE = (dyn_cast<ConstantExpr>(this))) return CE->isFalse();
	return false;
}

//class WPExpr: public Expr {
//public:
//	static bool classof(const Expr *E) {
//		return E->getKind() == Expr::WP;
//	}
//	static bool classof(const WPExpr *) {
//		return true;
//	}
//};

//class WPRegularExpr: public Expr {
//public:
//	static bool classof(const Expr *E) {
//		return E->getKind() == Expr::WPRegular;
//	}
//	static bool classof(const WPRegularExpr *) {
//		return true;
//	}
//
//public:
//	Width width;
//
//public:
//	WPRegularExpr(std::string name, uint64_t valueID, Width width, uint64_t threadID) {
//		this->instName = name;
//		this->moAddr = valueID;
//		this->width = width;
//		this->threadID = threadID;
//		this->hashValue = 0;
//		computeHash();
//	}
//
//	static klee::ref<Expr> create(std::string name = "", uint64_t valueID = 0, Width width = 0, uint64_t threadID = 0) {
//		return WPRegularExpr::alloc(name, valueID, width, threadID);
//	}
//
//	static klee::ref<Expr> alloc(std::string name, uint64_t valueID = 0, Width width = 0, uint64_t threadID = 0) {
//		return new WPRegularExpr(name, valueID, width, threadID);
//	}
//
//public:
//	Width getWidth() const {
//		return width;
//	}
//
//	unsigned getNumKids() const {
//		return 0;
//	}
//
//	klee::ref<Expr> getKid(unsigned i) const {
//		return 0;
//	}
//
//	int compareContents(const Expr &b) const {
//		const WPRegularExpr &cb = static_cast<const WPRegularExpr&>(b);
//		if (getWidth() != cb.getWidth()) return getWidth() < cb.getWidth() ? -1 : 1;
//		if (instName == cb.instName) {
//			return 0;
//		}
//		return instName < cb.instName ? -1 : 1;
//	}
//
//	klee::ref<Expr> rebuild(klee::ref<Expr> kids[]) const {
//		return 0;
//	}
//
//	Kind getKind() const {
//		return WPRegular;
//	}
//
//	void setWidth(Width w) {
//		width = w;
//	}
//
//	void setKid(unsigned i, klee::ref<Expr> e) {
//		assert(false);
//	}
//
//	virtual unsigned computeHash();
//};

//class WPLoadExpr: public Expr {
//public:
//public:
//	static bool classof(const Expr *E) {
//		return E->getKind() == Expr::WPLoad;
//	}
//
//	static bool classof(const WPLoadExpr *E) {
//		return true;
//	}
//public:
//	klee::ref<Expr> kid;
//	Width width;
//	Width readWidth;
//public:
//	WPLoadExpr(std::string name, uint64_t addr, Width width, uint64_t thread_id) {
//		this->instName = name;
//		this->moAddr = addr;
//		this->width = width;
//		this->readWidth = 0;
//		this->threadID = thread_id;
//		this->hashValue = 0;
//		computeHash();
//	}
//
//	WPLoadExpr(ref<Expr> kid, std::string name, Width width, uint64_t thread_id) {
//		this->kid = kid;
//		this->instName = name;
//		this->width = width;
//		this->threadID = thread_id;
//	}
//
//	static klee::ref<Expr> create(std::string name = "", uint64_t addr = 0, Width width = 0, uint64_t threadID = 0) {
//		return WPLoadExpr::alloc(name, addr, width, threadID);
//	}
//
//	static klee::ref<Expr> alloc(std::string name, uint64_t addr, Width width, uint64_t thread_id) {
//		return new WPLoadExpr(name, addr, width, thread_id);
//	}
//
//	static klee::ref<Expr> alloc(ref<Expr> kid, std::string name, Width width, uint64_t thread_id) {
//		return new WPLoadExpr(kid, name, width, thread_id);
//	}
//
//public:
//	Width getWidth() const {
//		return width;
//	}
//
//	unsigned getNumKids() const {
//		return 1;
//	}
//
//	void setKid(unsigned i, klee::ref<Expr> e) {
//		assert(i==0);
//		kid = e;
//		e->parent = this;
//		this->size = e->size;
//	}
//
//	klee::ref<Expr> getKid(unsigned i = 0) const {
//		assert(i==0 && "WPLoadExpr only have one kid");
//		return kid;
//	}
//
//	int compareContents(const Expr &b) const {
//		const WPLoadExpr &cb = static_cast<const WPLoadExpr&>(b);
//		if (getWidth() != cb.getWidth()) return getWidth() < cb.getWidth() ? -1 : 1;
//		if (instName == cb.instName) {
//			return 0;
//		}
//		return instName < cb.instName ? -1 : 1;
//	}
//
//	klee::ref<Expr> rebuild(klee::ref<Expr> kids[]) const {
//		return 0;
//	}
//
//	Kind getKind() const {
//		return WPLoad;
//	}
//
//	void setWidth(Width w) {
//		width = w;
//	}
//
//	virtual unsigned computeHash();
//};

} // End klee namespace

#endif
