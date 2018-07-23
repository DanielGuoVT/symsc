//===-- Expr.cpp ----------------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Expr.h"
#include "klee/util/Ref.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/ADT/Hashing.h"
// FIXME: We shouldn't need this once fast constant support moves into
// Core. If we need to do arithmetic, we probably want to use APInt.
#include "klee/Internal/Support/IntEvaluation.h"
#include "klee/util/ExprPPrinter.h"

#include <iostream>
#include <sstream>
#include <stack>

using namespace klee;
using namespace llvm;

namespace {
cl::opt<bool> ConstArrayOpt("const-array-opt", cl::init(false), cl::desc("Enable various optimizations involving all-constant arrays."));
}

/***/

unsigned Expr::count = 0;

ref<Expr> Expr::createTempRead(const Array *array, Expr::Width w) {
	UpdateList ul(array, 0);

	switch (w) {
	default:
		assert(0 && "invalid width");
	case Expr::Bool:
		return ZExtExpr::create(ReadExpr::create(ul, ConstantExpr::alloc(0, Expr::Int32)), Expr::Bool);
	case Expr::Int8:
		return ReadExpr::create(ul, ConstantExpr::alloc(0, Expr::Int32));
	case Expr::Int16:
		return ConcatExpr::create(ReadExpr::create(ul, ConstantExpr::alloc(1, Expr::Int32)),
				ReadExpr::create(ul, ConstantExpr::alloc(0, Expr::Int32)));
	case Expr::Int32:
		return ConcatExpr::create4(ReadExpr::create(ul, ConstantExpr::alloc(3, Expr::Int32)),
				ReadExpr::create(ul, ConstantExpr::alloc(2, Expr::Int32)), ReadExpr::create(ul, ConstantExpr::alloc(1, Expr::Int32)),
				ReadExpr::create(ul, ConstantExpr::alloc(0, Expr::Int32)));
	case Expr::Int64:
		return ConcatExpr::create8(ReadExpr::create(ul, ConstantExpr::alloc(7, Expr::Int32)),
				ReadExpr::create(ul, ConstantExpr::alloc(6, Expr::Int32)), ReadExpr::create(ul, ConstantExpr::alloc(5, Expr::Int32)),
				ReadExpr::create(ul, ConstantExpr::alloc(4, Expr::Int32)), ReadExpr::create(ul, ConstantExpr::alloc(3, Expr::Int32)),
				ReadExpr::create(ul, ConstantExpr::alloc(2, Expr::Int32)), ReadExpr::create(ul, ConstantExpr::alloc(1, Expr::Int32)),
				ReadExpr::create(ul, ConstantExpr::alloc(0, Expr::Int32)));
	}
}

// returns 0 if b is structurally equal to *this
int Expr::compare(const Expr &b) const {
	if (this == &b) return 0;

	Kind ak = getKind(), bk = b.getKind();
	if (ak != bk) return (ak < bk) ? -1 : 1;

	if (hashValue != b.hashValue) return (hashValue < b.hashValue) ? -1 : 1;

	if (int res = compareContents(b)) return res;

	unsigned aN = getNumKids();
	for (unsigned i = 0; i < aN; i++)
		if (int res = getKid(i).compare(b.getKid(i))) return res;

	return 0;
}

void Expr::printKind(std::ostream &os, Kind k) {
	switch (k) {
#define X(C) case C: os << #C; break
	X(Constant);
	X(NotOptimized);
//	X(WPRegular);
//	X(WPLoad);
	X(Read);
	X(Select);
	X(Concat);
	X(Extract);
	X(ZExt);
	X(SExt);
	X(Add);
	X(Sub);
	X(Mul);
	X(UDiv);
	X(SDiv);
	X(URem);
	X(SRem);
	X(Not);
	X(And);
	X(Or);
	X(Xor);
	X(Shl);
	X(LShr);
	X(AShr);
	X(Eq);
	X(Ne);
	X(Ult);
	X(Ule);
	X(Ugt);
	X(Uge);
	X(Slt);
	X(Sle);
	X(Sgt);
	X(Sge);
#undef X
	default:
		assert(0 && "invalid kind");
	}
}

////////
//
// Simple hash functions for various kinds of Exprs
//
///////

unsigned Expr::computeHash() {
	unsigned res = getKind() * Expr::MAGIC_HASH_CONSTANT;

	int n = getNumKids();
	for (int i = 0; i < n; i++) {
		res <<= 1;
		res ^= getKid(i)->hash() * Expr::MAGIC_HASH_CONSTANT;
	}

	hashValue = res;
	return hashValue;
}

//unsigned WPRegularExpr::computeHash() {
//	hashValue = getKind() * Expr::MAGIC_HASH_CONSTANT;
//	return hashValue;
//}
//
//unsigned WPLoadExpr::computeHash() {
//	hashValue = getKind() * Expr::MAGIC_HASH_CONSTANT;
//	return hashValue;
//}

unsigned ConstantExpr::computeHash() {
	hashValue = hash_value(value) ^ (getWidth() * MAGIC_HASH_CONSTANT);
	return hashValue;
}

unsigned CastExpr::computeHash() {
	unsigned res = getWidth() * Expr::MAGIC_HASH_CONSTANT;
	hashValue = res ^ src->hash() * Expr::MAGIC_HASH_CONSTANT;
	return hashValue;
}

unsigned ExtractExpr::computeHash() {
	unsigned res = offset * Expr::MAGIC_HASH_CONSTANT;
	res ^= getWidth() * Expr::MAGIC_HASH_CONSTANT;
	hashValue = res ^ expr->hash() * Expr::MAGIC_HASH_CONSTANT;
	return hashValue;
}

unsigned ReadExpr::computeHash() {
	unsigned res = index->hash() * Expr::MAGIC_HASH_CONSTANT;
	res ^= updates.hash();
	hashValue = res;
	return hashValue;
}

unsigned NotExpr::computeHash() {
	unsigned hashValue = expr->hash() * Expr::MAGIC_HASH_CONSTANT * Expr::Not;
	return hashValue;
}

klee::ref<Expr> Expr::createFromKind(Kind k, std::vector<CreateArg> args) {
	unsigned numArgs = args.size();
	(void) numArgs;

	switch (k) {
	case Constant:
	case Extract:
	case Read:
	default:
		assert(0 && "invalid kind");

	case NotOptimized:
		assert(numArgs == 1 && args[0].isExpr() && "invalid args array for given opcode");
		return NotOptimizedExpr::create(args[0].expr);

	case Select:
		assert(numArgs == 3 && args[0].isExpr() && args[1].isExpr() && args[2].isExpr() && "invalid args array for Select opcode");
		return SelectExpr::create(args[0].expr, args[1].expr, args[2].expr);

	case Concat: {
		assert(numArgs == 2 && args[0].isExpr() && args[1].isExpr() && "invalid args array for Concat opcode");

		return ConcatExpr::create(args[0].expr, args[1].expr);
	}

#define CAST_EXPR_CASE(T)                                    \
      case T:                                                \
        assert(numArgs == 2 &&             					 \
               args[0].isExpr() && args[1].isWidth() &&      \
               "invalid args array for given opcode");       \
      return T ## Expr::create(args[0].expr, args[1].width); \
      
#define BINARY_EXPR_CASE(T)                                 \
      case T:                                               \
        assert(numArgs == 2 &&                              \
               args[0].isExpr() && args[1].isExpr() &&      \
               "invalid args array for given opcode");      \
      return T ## Expr::create(args[0].expr, args[1].expr); \

	CAST_EXPR_CASE(ZExt);
	CAST_EXPR_CASE(SExt);

	BINARY_EXPR_CASE(Add);
	BINARY_EXPR_CASE(Sub);
	BINARY_EXPR_CASE(Mul);
	BINARY_EXPR_CASE(UDiv);
	BINARY_EXPR_CASE(SDiv);
	BINARY_EXPR_CASE(URem);
	BINARY_EXPR_CASE(SRem);
	BINARY_EXPR_CASE(And);
	BINARY_EXPR_CASE(Or);
	BINARY_EXPR_CASE(Xor);
	BINARY_EXPR_CASE(Shl);
	BINARY_EXPR_CASE(LShr);
	BINARY_EXPR_CASE(AShr);

	BINARY_EXPR_CASE(Eq);
	BINARY_EXPR_CASE(Ne);
	BINARY_EXPR_CASE(Ult);
	BINARY_EXPR_CASE(Ule);
	BINARY_EXPR_CASE(Ugt);
	BINARY_EXPR_CASE(Uge);
	BINARY_EXPR_CASE(Slt);
	BINARY_EXPR_CASE(Sle);
	BINARY_EXPR_CASE(Sgt);
	BINARY_EXPR_CASE(Sge);
	}
}

void Expr::printWidth(std::ostream &os, Width width) {
	switch (width) {
	case Expr::Bool:
		os << "Expr::Bool";
		break;
	case Expr::Int8:
		os << "Expr::Int8";
		break;
	case Expr::Int16:
		os << "Expr::Int16";
		break;
	case Expr::Int32:
		os << "Expr::Int32";
		break;
	case Expr::Int64:
		os << "Expr::Int64";
		break;
	case Expr::Fl80:
		os << "Expr::Fl80";
		break;
	default:
		os << "<invalid type: " << (unsigned) width << ">";
		break;
	}
}

klee::ref<Expr> Expr::createImplies(klee::ref<Expr> hyp, klee::ref<Expr> conc) {
	return OrExpr::create(Expr::createIsZero(hyp), conc);
}

klee::ref<Expr> Expr::createIsZero(klee::ref<Expr> e) {
	return EqExpr::create(e, ConstantExpr::create(0, e->getWidth()));
}

void Expr::print(std::ostream &os) const {
	ExprPPrinter::printSingleExpr(os, const_cast<Expr*>(this));
}

void Expr::dump() const {
	this->print(std::cerr);
	std::cerr << std::endl;
}

void Expr::replaceArray(ref<Expr> target, std::vector<std::pair<const Array *, const Array*> > &arrayPairs) {
	if (target->getKind() == Expr::Read) {
		ref<ReadExpr> e = dyn_cast<ReadExpr>(target);
		std::string name = e->updates.root->getName();
		for (size_t i = 0; i < arrayPairs.size(); i++) {
			if (arrayPairs[i].first->getName() == name) {
				e->updates.root = arrayPairs[i].second;
				return;
			}
		}
	}
	for (size_t i = 0; i < target->getNumKids(); i++) {
		replaceArray(target->getKid(i), arrayPairs);
	}
}

klee::ref<Expr> Expr::deepCopy(klee::ref<Expr> expr, bool usc) {
	klee::ref<Expr> ret = NULL;
	switch (expr->getKind()) {
	case Expr::InvalidKind: {
		assert(0);
		break;
	}
	case Expr::Constant: {
		ret = ConstantExpr::create(dyn_cast<ConstantExpr>(expr)->getZExtValue(), expr->getWidth());
		break;
	}
	case Expr::NotOptimized: {
		ret = NotOptimizedExpr::create(expr);
		break;
	}
	case Expr::Read: {
		ref<ReadExpr> read = cast<ReadExpr>(expr);
		ref<Expr> idx = deepCopy(read->index);
		const Array *array = read->updates.root;
		std::string arrayName = array->name + "_1";
		UpdateList *ul;
		if (Array::updateListSingletonMap.find(arrayName) == Array::updateListSingletonMap.end()) {
			const Array *array_1 = Array::CreateArray(arrayName, array->size);
			ul = new UpdateList(array_1, read->updates.head);
			Array::updateListSingletonMap[arrayName] = ul;
		} else {
			ul = Array::updateListSingletonMap[arrayName];
		}
		ret = ReadExpr::create(*ul, idx);
		break;
	}
	case Expr::Select: {
		ret = SelectExpr::create(deepCopy(expr->getKid(0)), deepCopy(expr->getKid(1)), deepCopy(expr->getKid(2)));
		break;
	}
	case Expr::Concat: {
		ret = ConcatExpr::create(deepCopy(expr->getKid(0)), deepCopy(expr->getKid(1)));
		break;
	}
	case Expr::Extract: {
		ret = ExtractExpr::create(deepCopy(expr->getKid(0)), (dyn_cast<ExtractExpr>(expr)->offset), (dyn_cast<ExtractExpr>(expr)->width));
		break;
	}
	case Expr::ZExt: {
		ret = ZExtExpr::create(deepCopy(expr->getKid(0)), expr->getWidth());
		break;
	}
	case Expr::SExt: {
		ret = SExtExpr::create(deepCopy(expr->getKid(0)), expr->getWidth());
		break;
	}
	case Expr::Not: {
		ret = NotExpr::create(deepCopy(expr->getKid(0)));
		break;
	}
#define BinaryExpr(_type)  						\
	case Expr::_type:{      					\
	    ret=_type##Expr::create(deepCopy(expr->getKid(0)),deepCopy(expr->getKid(1)));    \
	    break;                                  \
	}
	BinaryExpr(Add);
	BinaryExpr(Sub);
	BinaryExpr(Mul);
	BinaryExpr(UDiv);
	BinaryExpr(SDiv);
	BinaryExpr(URem);
	BinaryExpr(SRem);
	BinaryExpr(And);
	BinaryExpr(Or);
	BinaryExpr(Xor);
	BinaryExpr(Shl);
	BinaryExpr(LShr);
	BinaryExpr(AShr);

		//compare
	BinaryExpr(Eq);
	BinaryExpr(Ne);
	BinaryExpr(Ult);
	BinaryExpr(Ule);
	BinaryExpr(Ugt);
	BinaryExpr(Uge);
	BinaryExpr(Slt);
	BinaryExpr(Sle);
	BinaryExpr(Sgt);
	BinaryExpr(Sge);
	default:
		ret = NULL;
		break;
	}
	return ret;
}

klee::ref<Expr> Expr::copy1(klee::ref<Expr> expr, bool usc) {
	klee::ref<Expr> ret = NULL;
	switch (expr->getKind()) {
	case Expr::InvalidKind: {
		assert(0);
		break;
	}
	case Expr::Constant: {
		ret = ConstantExpr::create(dyn_cast<ConstantExpr>(expr)->getZExtValue(), expr->getWidth());
		break;
	}
	case Expr::NotOptimized: {
		ret = NotOptimizedExpr::create(expr);
		break;
	}
	case Expr::Read: {
		ref<Expr> idx = deepCopy((dyn_cast<ReadExpr>(expr)->index));
		UpdateList updates = (dyn_cast<ReadExpr>(expr))->updates;
		ret = ReadExpr::create(updates, idx);
		break;
	}
	case Expr::Select: {
		ret = SelectExpr::create(0, 0, 0);
		break;
	}
	case Expr::Concat: {
		ret = ConcatExpr::create(0, 0);
		break;
	}
	case Expr::Extract: {
		ret = ExtractExpr::create(0, (dyn_cast<ExtractExpr>(expr)->offset), (dyn_cast<ExtractExpr>(expr)->width));
		break;
	}
	case Expr::ZExt: {
		ret = ZExtExpr::create(0, expr->getWidth());
		break;
	}
	case Expr::SExt: {
		ret = SExtExpr::create(0, expr->getWidth());
		break;
	}
	case Expr::Not: {
		ret = NotExpr::create(0);
		break;
	}
#define BinaryExpr(_type)  						\
	case Expr::_type:{      					\
	    ret=_type##Expr::create(0,0);    \
	    break;                                  \
	}
	BinaryExpr(Add);
	BinaryExpr(Sub);
	BinaryExpr(Mul);
	BinaryExpr(UDiv);
	BinaryExpr(SDiv);
	BinaryExpr(URem);
	BinaryExpr(SRem);
	BinaryExpr(And);
	BinaryExpr(Or);
	BinaryExpr(Xor);
	BinaryExpr(Shl);
	BinaryExpr(LShr);
	BinaryExpr(AShr);

		//compare
	BinaryExpr(Eq);
	BinaryExpr(Ne);
	BinaryExpr(Ult);
	BinaryExpr(Ule);
	BinaryExpr(Ugt);
	BinaryExpr(Uge);
	BinaryExpr(Slt);
	BinaryExpr(Sle);
	BinaryExpr(Sgt);
	BinaryExpr(Sge);
	default:
		ret = NULL;
		break;
	}
	return ret;
}

klee::ref<Expr> Expr::copyExpr(klee::ref<Expr> e, bool usc) {
	if (e.isNull()) return NULL;
//	ref<Expr> src = NULL;
//	ref<Expr> target = NULL;
//	ref<Expr> tmp = NULL;
//	std::stack<ref<Expr> > stack;
//	stack.push(e);
//
//	while (!stack.empty()) {
//		src = stack.top();
//		stack.pop();
//
//		tmp = copy1(src);
//
//
//		for (unsigned i = 0; i < src->getNumKids(); i++) {
//			if (!tmp->getKid(i).isNull()) {
//				stack.push(src->getKid(i));
//			}
//		}
//
//	}

	return NULL;
}

//klee::ref<Expr> Expr::copyExpr(klee::ref<Expr> e, bool usc) {
//	klee::ref<Expr> ret;
//	if (e.get() == NULL) {
//		return NULL;
//	}
//	switch (e->getKind()) {
//	case Expr::InvalidKind: {
//		ret = NULL;
//		break;
//	}
//	case Expr::Constant: {
//		ret = ConstantExpr::create(dyn_cast<ConstantExpr>(e)->getZExtValue(), e->getWidth());
//		break;
//	}
//	case Expr::NotOptimized: {
//		klee::ref<Expr> src = copyExpr(e->getKid(0));
//		ret = NotOptimizedExpr::create(src);
//		break;
//	}
//	case Expr::Read: {
//		klee::ref<Expr> index = copyExpr(dyn_cast<ReadExpr>(e)->index);
//		UpdateList updates = (dyn_cast<ReadExpr>(e)->updates);
//		ret = ReadExpr::alloc(updates, index);
//		break;
//	}
//	case Expr::Select: {
//		klee::ref<Expr> cond = copyExpr(e->getKid(0));
//		klee::ref<Expr> trueExpr = copyExpr(e->getKid(1));
//		klee::ref<Expr> falseExpr = copyExpr(e->getKid(2));
//		ret = SelectExpr::alloc(cond, trueExpr, falseExpr);
////		ret = SelectExpr::create(cond, trueExpr, falseExpr);
//		break;
//	}
//	case Expr::Concat: {
//		klee::ref<Expr> left = copyExpr(e->getKid(0));
//		klee::ref<Expr> right = copyExpr(e->getKid(1));
////		ret = ConcatExpr::create(left, right);
//		ret = ConcatExpr::alloc(left, right);
//		break;
//	}
//	case Expr::Extract: {
//		klee::ref<Expr> expr = copyExpr(e->getKid(0));
////		ret = ExtractExpr::create(expr, dyn_cast<ExtractExpr>(e)->offset, dyn_cast<ExtractExpr>(e)->width);
//		ret = ExtractExpr::alloc(expr, dyn_cast<ExtractExpr>(e)->offset, dyn_cast<ExtractExpr>(e)->width);
//		break;
//	}
//	case Expr::ZExt: {
//		klee::ref<Expr> src = copyExpr(e->getKid(0));
//		ret = ZExtExpr::create(src, e->getWidth());
//		break;
//	}
//	case Expr::SExt: {
//		klee::ref<Expr> src = copyExpr(e->getKid(0));
//		ret = SExtExpr::create(src, e->getWidth());
//		ret->instName = e->instName;
//		ret->instructionID = e->getInstID();
////		ret->moAddr = e->getMOAddr();
//		break;
//	}
//	case Expr::Not: {
//		klee::ref<Expr> expr = copyExpr(e->getKid(0));
//		ret = NotExpr::create(expr);
//		ret->instName = e->instName;
//		ret->instructionID = e->getInstID();
////		ret->moAddr = e->getMOAddr();
//		break;
//	}
//	case Expr::Add: {
//		klee::ref<Expr> left = copyExpr(e->getKid(0));
//		klee::ref<Expr> right = copyExpr(e->getKid(1));
//
////		ret = AddExpr::create(left, right);
//		ret = AddExpr::alloc(left, right);
//		ret->instName = e->instName;
//		ret->instructionID = e->getInstID();
////		ret->moAddr = e->getMOAddr();
//		break;
//	}
//	case Expr::Eq: {
//		klee::ref<Expr> left = copyExpr(e->getKid(0));
//		klee::ref<Expr> right = copyExpr(e->getKid(1));
//		if (left->getWidth() != right->getWidth()) {
//			std::cerr << "width mismatch." << std::endl;
//			std::cerr << "left: " << left << ", name: " << left->getName() << ", width: " << left->getWidth() << std::endl;
//			std::cerr << "right: " << right << ", name: " << right->getName() << ", width: " << right->getWidth() << std::endl;
//		}
////		ret = EqExpr::create(left, right);
//		ret = EqExpr::alloc(left, right);
//		break;
//	}
//	case Expr::Ult: {
//		klee::ref<Expr> left = copyExpr(e->getKid(0));
//		klee::ref<Expr> right = copyExpr(e->getKid(1));
//		if (right.isNull()) {
//			std::cerr << "right is null, e: " << e << std::endl;
//			std::cerr << "right kid: " << e->getKid(1) << std::endl;
//			std::cerr << "kind of right kid: " << e->getKid(1)->getKind() << std::endl;
//		}
//		if (left->getWidth() != right->getWidth()) {
//			std::cerr << "width mismatch." << std::endl;
//			std::cerr << "left: " << left << ", name: " << left->getName() << ", width: " << left->getWidth() << std::endl;
//			std::cerr << "right: " << right << ", name: " << right->getName() << ", width: " << right->getWidth() << std::endl;
//		}
//
////		ret = UltExpr::create(left, right);
//		ret = UltExpr::alloc(left, right);
//		break;
//	}
//	case Expr::Sle: {
//		klee::ref<Expr> left = copyExpr(e->getKid(0));
//		klee::ref<Expr> right = copyExpr(e->getKid(1));
//
//		if (right.isNull()) {
//			std::cerr << "right is null, e: " << e << std::endl;
//			std::cerr << "right kid: " << e->getKid(1) << std::endl;
//			std::cerr << "kind of right kid: " << e->getKid(1)->getKind() << std::endl;
//		}
//		if (left->getWidth() != right->getWidth()) {
//			std::cerr << "width mismatch." << std::endl;
//			std::cerr << "left: " << left << ", name: " << left->getName() << ", width: " << left->getWidth() << std::endl;
//			std::cerr << "right: " << right << ", name: " << right->getName() << ", width: " << right->getWidth() << std::endl;
//		}
//
////		ret = SleExpr::create(left, right);
//		ret = SleExpr::alloc(left, right);
//		break;
//	}
//	case Expr::Xor: {
//		klee::ref<Expr> left = copyExpr(e->getKid(0));
//		klee::ref<Expr> right = copyExpr(e->getKid(1));
//		ret = XorExpr::alloc(left, right);
//		break;
//	}
//	case Expr::And: {
//		klee::ref<Expr> left = copyExpr(e->getKid(0));
//		klee::ref<Expr> right = copyExpr(e->getKid(1));
//		ret = AndExpr::alloc(left, right);
//		break;
//	}
//
//#define BinaryExpr(_type)  						\
//	case Expr::_type:{      					\
//		klee::ref<Expr> left=copyExpr(e->getKid(0));  \
//	    klee::ref<Expr> right=copyExpr(e->getKid(1)); \
//	    ret=_type##Expr::alloc(left,right);    \
//	    break;                                  \
//	}
//	BinaryExpr(Sub);
//	BinaryExpr(Mul);
//	BinaryExpr(UDiv);
//	BinaryExpr(SDiv);
//	BinaryExpr(URem);
//	BinaryExpr(SRem);
////	BinaryExpr(And);
//	BinaryExpr(Or);
////	BinaryExpr(Xor);
//	BinaryExpr(Shl);
//	BinaryExpr(LShr);
//	BinaryExpr(AShr);
//
//		//compare
////	BinaryExpr(Eq);
//	BinaryExpr(Ne);
////	BinaryExpr(Ult);
//	BinaryExpr(Ule);
//	BinaryExpr(Ugt);
//	BinaryExpr(Uge);
//	BinaryExpr(Slt);
////	BinaryExpr(Sle);
//	BinaryExpr(Sgt);
//	BinaryExpr(Sge);
//	default:
//		ret = NULL;
//		break;
//	}
//	return ret;
//}

/***/

klee::ref<Expr> ConstantExpr::fromMemory(void *address, Width width) {
	switch (width) {
	case Expr::Bool:
		return ConstantExpr::create(*((uint8_t*) address), width);
	case Expr::Int8:
		return ConstantExpr::create(*((uint8_t*) address), width);
	case Expr::Int16:
		return ConstantExpr::create(*((uint16_t*) address), width);
	case Expr::Int32:
		return ConstantExpr::create(*((uint32_t*) address), width);
	case Expr::Int64:
		return ConstantExpr::create(*((uint64_t*) address), width);
		// FIXME: what about machines without x87 support?
	default:
		return ConstantExpr::alloc(llvm::APInt(width, (width + llvm::integerPartWidth - 1) / llvm::integerPartWidth, (const uint64_t*) address));
	}
}

void ConstantExpr::toMemory(void *address) {
	switch (getWidth()) {
	default:
		assert(0 && "invalid type");
	case Expr::Bool:
		*((uint8_t*) address) = getZExtValue(1);
		break;
	case Expr::Int8:
		*((uint8_t*) address) = getZExtValue(8);
		break;
	case Expr::Int16:
		*((uint16_t*) address) = getZExtValue(16);
		break;
	case Expr::Int32:
		*((uint32_t*) address) = getZExtValue(32);
		break;
	case Expr::Int64:
		*((uint64_t*) address) = getZExtValue(64);
		break;
		// FIXME: what about machines without x87 support?
	case Expr::Fl80:
		*((long double*) address) = *(long double*) value.getRawData();
		break;
	}
}

void ConstantExpr::toString(std::string &Res) const {
	Res = value.toString(10, false);
}

klee::ref<ConstantExpr> ConstantExpr::Concat(const klee::ref<ConstantExpr> &RHS) {
	Expr::Width W = getWidth() + RHS->getWidth();
	APInt Tmp(value);
	Tmp = Tmp.zext(W);
	Tmp <<= RHS->getWidth();
	Tmp |= APInt(RHS->value).zext(W);

	return ConstantExpr::alloc(Tmp);
}

klee::ref<ConstantExpr> ConstantExpr::Extract(unsigned Offset, Width W) {
	return ConstantExpr::alloc(APInt(value.ashr(Offset)).zextOrTrunc(W));
}

klee::ref<ConstantExpr> ConstantExpr::ZExt(Width W) {
	return ConstantExpr::alloc(APInt(value).zextOrTrunc(W));
}

klee::ref<ConstantExpr> ConstantExpr::SExt(Width W) {
	return ConstantExpr::alloc(APInt(value).sextOrTrunc(W));
}

klee::ref<ConstantExpr> ConstantExpr::Add(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value + RHS->value);
}

klee::ref<ConstantExpr> ConstantExpr::Neg() {
	return ConstantExpr::alloc(-value);
}

klee::ref<ConstantExpr> ConstantExpr::Sub(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value - RHS->value);
}

klee::ref<ConstantExpr> ConstantExpr::Mul(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value * RHS->value);
}

klee::ref<ConstantExpr> ConstantExpr::UDiv(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.udiv(RHS->value));
}

klee::ref<ConstantExpr> ConstantExpr::SDiv(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.sdiv(RHS->value));
}

klee::ref<ConstantExpr> ConstantExpr::URem(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.urem(RHS->value));
}

klee::ref<ConstantExpr> ConstantExpr::SRem(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.srem(RHS->value));
}

klee::ref<ConstantExpr> ConstantExpr::And(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value & RHS->value);
}

klee::ref<ConstantExpr> ConstantExpr::Or(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value | RHS->value);
}

klee::ref<ConstantExpr> ConstantExpr::Xor(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value ^ RHS->value);
}

klee::ref<ConstantExpr> ConstantExpr::Shl(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.shl(RHS->value));
}

klee::ref<ConstantExpr> ConstantExpr::LShr(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.lshr(RHS->value));
}

klee::ref<ConstantExpr> ConstantExpr::AShr(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.ashr(RHS->value));
}

klee::ref<ConstantExpr> ConstantExpr::Not() {
	return ConstantExpr::alloc(~value);
}

klee::ref<ConstantExpr> ConstantExpr::Eq(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value == RHS->value, Expr::Bool);
}

klee::ref<ConstantExpr> ConstantExpr::Ne(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value != RHS->value, Expr::Bool);
}

klee::ref<ConstantExpr> ConstantExpr::Ult(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.ult(RHS->value), Expr::Bool);
}

klee::ref<ConstantExpr> ConstantExpr::Ule(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.ule(RHS->value), Expr::Bool);
}

klee::ref<ConstantExpr> ConstantExpr::Ugt(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.ugt(RHS->value), Expr::Bool);
}

klee::ref<ConstantExpr> ConstantExpr::Uge(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.uge(RHS->value), Expr::Bool);
}

klee::ref<ConstantExpr> ConstantExpr::Slt(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.slt(RHS->value), Expr::Bool);
}

klee::ref<ConstantExpr> ConstantExpr::Sle(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.sle(RHS->value), Expr::Bool);
}

klee::ref<ConstantExpr> ConstantExpr::Sgt(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.sgt(RHS->value), Expr::Bool);
}

ref<ConstantExpr> ConstantExpr::Sge(const klee::ref<ConstantExpr> &RHS) {
	return ConstantExpr::alloc(value.sge(RHS->value), Expr::Bool);
}

/***/

klee::ref<Expr> NotOptimizedExpr::create(klee::ref<Expr> src) {
	return NotOptimizedExpr::alloc(src);
}

/***/

extern "C" void vc_DeleteExpr(void*);

Array::~Array() {
	// FIXME: This shouldn't be necessary.
	if (stpInitialArray) {
		::vc_DeleteExpr(stpInitialArray);
		stpInitialArray = 0;
	}
}

unsigned Array::computeHash() {
	unsigned res = 0;
	for (unsigned i = 0, e = name.size(); i != e; ++i)
		res = (res * Expr::MAGIC_HASH_CONSTANT) + name[i];
	res = (res * Expr::MAGIC_HASH_CONSTANT) + size;
	hashValue = res;
	return hashValue;
}

std::map<unsigned, std::vector<const Array *> *> Array::symbolicArraySingletonMap;

std::map<std::string, UpdateList*> Array::updateListSingletonMap;

const Array * Array::CreateArray(const std::string &_name, uint64_t _size, const ref<ConstantExpr> *constantValuesBegin,
		const ref<ConstantExpr> *constantValuesEnd) {

	const Array * array = new Array(_name, _size, constantValuesBegin, constantValuesEnd);
	if (array->constantValues.size() == 0) { // symbolic array
		unsigned hash = array->hash();
		std::vector<const Array *> * bucket = Array::symbolicArraySingletonMap[hash];
		if (bucket) {
			for (std::vector<const Array*>::const_iterator it = bucket->begin(); it != bucket->end(); it++) {
				const Array* prospect = *it;
				if (prospect->size == array->size && prospect->name == array->name) {
					delete array;
					return prospect;
				}
			}
			bucket->push_back(array);
			return array;
		} else {
			bucket = new std::vector<const Array *>();
			bucket->push_back(array);
			Array::symbolicArraySingletonMap[hash] = bucket;
			return array;
		}
	} else { // concrete array
		return array;
	}
}

/***/

klee::ref<Expr> ReadExpr::create(const UpdateList &ul, klee::ref<Expr> index) {
	// rollback index when possible...

	// XXX this doesn't really belong here... there are basically two
	// cases, one is rebuild, where we want to optimistically try various
	// optimizations when the index has changed, and the other is
	// initial creation, where we expect the ObjectState to have constructed
	// a smart UpdateList so it is not worth rescanning.

	const UpdateNode *un = ul.head;
	for (; un; un = un->next) {
		klee::ref<Expr> cond = EqExpr::create(index, un->index);

		if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(cond))) {
			if (CE->isTrue()) {
				return un->value;
			}
		} else {
			break;
		}
	}

	return ReadExpr::alloc(ul, index);
}

int ReadExpr::compareContents(const Expr &b) const {
	return updates.compare(static_cast<const ReadExpr&>(b).updates);
}

klee::ref<Expr> SelectExpr::create(klee::ref<Expr> c, klee::ref<Expr> t, klee::ref<Expr> f) {
	Expr::Width kt = t->getWidth();

	assert(c->getWidth()==Bool && "type mismatch");
	assert(kt==f->getWidth() && "type mismatch");

	if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(c))) {
		return CE->isTrue() ? t : f;
	} else if (t == f) {
		return t;
	} else if (kt == Expr::Bool) { // c ? t : f  <=> (c and t) or (not c and f)
		if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(t))) {
			if (CE->isTrue()) {
				return OrExpr::create(c, f);
			} else {
				return AndExpr::create(Expr::createIsZero(c), f);
			}
		} else if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(f))) {
			if (CE->isTrue()) {
				return OrExpr::create(Expr::createIsZero(c), t);
			} else {
				return AndExpr::create(c, t);
			}
		}
	}

	return SelectExpr::alloc(c, t, f);
}

/***/

klee::ref<Expr> ConcatExpr::create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	Expr::Width w = l->getWidth() + r->getWidth();

	// Fold concatenation of constants.
	//
	// FIXME: concat 0 x -> zext x ?
	if (ConstantExpr * lCE = (dyn_cast<ConstantExpr>(l))) if (ConstantExpr * rCE = (dyn_cast<ConstantExpr>(r))) return lCE->Concat(rCE);

	// Merge contiguous Extracts
	if (ExtractExpr * ee_left = (dyn_cast<ExtractExpr>(l))) {
		if (ExtractExpr * ee_right = (dyn_cast<ExtractExpr>(r))) {
			if (ee_left->expr == ee_right->expr && ee_right->offset + ee_right->width == ee_left->offset) {
				return ExtractExpr::create(ee_left->expr, ee_right->offset, w);
			}
		}
	}

	return ConcatExpr::alloc(l, r);
}

/// Shortcut to concat N kids.  The chain returned is unbalanced to the right
klee::ref<Expr> ConcatExpr::createN(unsigned n_kids, const klee::ref<Expr> kids[]) {
	assert(n_kids > 0);
	if (n_kids == 1) return kids[0];

	klee::ref<Expr> r = ConcatExpr::create(kids[n_kids - 2], kids[n_kids - 1]);
	for (int i = n_kids - 3; i >= 0; i--)
		r = ConcatExpr::create(kids[i], r);
	return r;
}

/// Shortcut to concat 4 kids.  The chain returned is unbalanced to the right
klee::ref<Expr> ConcatExpr::create4(const klee::ref<Expr> &kid1, const klee::ref<Expr> &kid2, const klee::ref<Expr> &kid3,
		const klee::ref<Expr> &kid4) {
	return ConcatExpr::create(kid1, ConcatExpr::create(kid2, ConcatExpr::create(kid3, kid4)));
}

/// Shortcut to concat 8 kids.  The chain returned is unbalanced to the right
klee::ref<Expr> ConcatExpr::create8(const klee::ref<Expr> &kid1, const klee::ref<Expr> &kid2, const klee::ref<Expr> &kid3,
		const klee::ref<Expr> &kid4, const klee::ref<Expr> &kid5, const klee::ref<Expr> &kid6, const klee::ref<Expr> &kid7,
		const klee::ref<Expr> &kid8) {
	return ConcatExpr::create(kid1,
			ConcatExpr::create(kid2, ConcatExpr::create(kid3, ConcatExpr::create(kid4, ConcatExpr::create4(kid5, kid6, kid7, kid8)))));
}

/***/

klee::ref<Expr> ExtractExpr::create(klee::ref<Expr> expr, unsigned off, Width w) {
	unsigned kw = expr->getWidth();
	assert(w > 0 && off + w <= kw && "invalid extract");

	if (w == kw) {
		return expr;
	} else if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(expr))) {
		return CE->Extract(off, w);
	} else {
		// Extract(Concat)
		if (ConcatExpr * ce = (dyn_cast<ConcatExpr>(expr))) {
			// if the extract skips the right side of the concat
			if (off >= ce->getRight()->getWidth()) return ExtractExpr::create(ce->getLeft(), off - ce->getRight()->getWidth(), w);

			// if the extract skips the left side of the concat
			if (off + w <= ce->getRight()->getWidth()) return ExtractExpr::create(ce->getRight(), off, w);

			// E(C(x,y)) = C(E(x), E(y))
			return ConcatExpr::create(ExtractExpr::create(ce->getKid(0), 0, w - ce->getKid(1)->getWidth() + off),
					ExtractExpr::create(ce->getKid(1), off, ce->getKid(1)->getWidth() - off));
		}
	}

	return ExtractExpr::alloc(expr, off, w);
}

/***/

klee::ref<Expr> NotExpr::create(const klee::ref<Expr> &e) {
	if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(e))) return CE->Not();

	return NotExpr::alloc(e);
}

/***/

klee::ref<Expr> ZExtExpr::create(const klee::ref<Expr> &e, Width w) {
	unsigned kBits = e->getWidth();
	if (w == kBits) {
		return e;
	} else if (w < kBits) { // trunc
		return ExtractExpr::create(e, 0, w);
	} else if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(e))) {
		return CE->ZExt(w);
	} else {
		return ZExtExpr::alloc(e, w);
	}
}

klee::ref<Expr> SExtExpr::create(const klee::ref<Expr> &e, Width w) {
	unsigned kBits = e->getWidth();
	if (w == kBits) {
		return e;
	} else if (w < kBits) { // trunc
		return ExtractExpr::create(e, 0, w);
	} else if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(e))) {
		return CE->SExt(w);
	} else {
		return SExtExpr::alloc(e, w);
	}
}

/***/

static klee::ref<Expr> AndExpr_create(Expr *l, Expr *r);
static klee::ref<Expr> XorExpr_create(Expr *l, Expr *r);

static klee::ref<Expr> EqExpr_createPartial(Expr *l, const klee::ref<ConstantExpr> &cr);
static klee::ref<Expr> AndExpr_createPartialR(const klee::ref<ConstantExpr> &cl, Expr *r);
static klee::ref<Expr> SubExpr_createPartialR(const klee::ref<ConstantExpr> &cl, Expr *r);
static klee::ref<Expr> XorExpr_createPartialR(const klee::ref<ConstantExpr> &cl, Expr *r);

static klee::ref<Expr> AddExpr_createPartialR(const klee::ref<ConstantExpr> &cl, Expr *r) {
	Expr::Width type = cl->getWidth();
	if (type == Expr::Bool) {
		return XorExpr_createPartialR(cl, r);
	} else if (cl->isZero()) {
		return r;
	} else {
		Expr::Kind rk = r->getKind();
		if (rk == Expr::Add && isa<ConstantExpr>(r->getKid(0))) { // A + (B+c) == (A+B) + c
			return AddExpr::create(AddExpr::create(cl, r->getKid(0)), r->getKid(1));
		} else if (rk == Expr::Sub && isa<ConstantExpr>(r->getKid(0))) { // A + (B-c) == (A+B) - c
			return SubExpr::create(AddExpr::create(cl, r->getKid(0)), r->getKid(1));
		} else {
			return AddExpr::alloc(cl, r);
		}
	}
}
static klee::ref<Expr> AddExpr_createPartial(Expr *l, const klee::ref<ConstantExpr> &cr) {
	return AddExpr_createPartialR(cr, l);
}
static klee::ref<Expr> AddExpr_create(Expr *l, Expr *r) { // this function is called when both l and r are not ConstantExpr
	Expr::Width type = l->getWidth();

	if (type == Expr::Bool) {
		return XorExpr_create(l, r);
	} else {
		Expr::Kind lk = l->getKind(), rk = r->getKind();
		if (lk == Expr::Add && isa<ConstantExpr>(l->getKid(0))) { // (k+a)+b = k+(a+b)
			return AddExpr::create(l->getKid(0), AddExpr::create(l->getKid(1), r));
		} else if (lk == Expr::Sub && isa<ConstantExpr>(l->getKid(0))) { // (k-a)+b = k+(b-a)
			return AddExpr::create(l->getKid(0), SubExpr::create(r, l->getKid(1)));
		} else if (rk == Expr::Add && isa<ConstantExpr>(r->getKid(0))) { // a + (k+b) = k+(a+b)
			return AddExpr::create(r->getKid(0), AddExpr::create(l, r->getKid(1)));
		} else if (rk == Expr::Sub && isa<ConstantExpr>(r->getKid(0))) { // a + (k-b) = k+(a-b)
			return AddExpr::create(r->getKid(0), SubExpr::create(l, r->getKid(1)));
		} else {
			return AddExpr::alloc(l, r);
		}
	}
}

static klee::ref<Expr> SubExpr_createPartialR(const klee::ref<ConstantExpr> &cl, Expr *r) {
	Expr::Width type = cl->getWidth();

	if (type == Expr::Bool) {
		return XorExpr_createPartialR(cl, r);
	} else {
		Expr::Kind rk = r->getKind();
		if (rk == Expr::Add && isa<ConstantExpr>(r->getKid(0))) { // A - (B+c) == (A-B) - c
			return SubExpr::create(SubExpr::create(cl, r->getKid(0)), r->getKid(1));
		} else if (rk == Expr::Sub && isa<ConstantExpr>(r->getKid(0))) { // A - (B-c) == (A-B) + c
			return AddExpr::create(SubExpr::create(cl, r->getKid(0)), r->getKid(1));
		} else {
			return SubExpr::alloc(cl, r);
		}
	}
}
static klee::ref<Expr> SubExpr_createPartial(Expr *l, const klee::ref<ConstantExpr> &cr) {
	// l - c => l + (-c)
	return AddExpr_createPartial(l, ConstantExpr::alloc(0, cr->getWidth())->Sub(cr));
}
static klee::ref<Expr> SubExpr_create(Expr *l, Expr *r) {
	Expr::Width type = l->getWidth();

	if (type == Expr::Bool) {
		return XorExpr_create(l, r);
	} else if (*l == *r) {
		return ConstantExpr::alloc(0, type);
	} else {
		Expr::Kind lk = l->getKind(), rk = r->getKind();
		if (lk == Expr::Add && isa<ConstantExpr>(l->getKid(0))) { // (k+a)-b = k+(a-b)
			return AddExpr::create(l->getKid(0), SubExpr::create(l->getKid(1), r));
		} else if (lk == Expr::Sub && isa<ConstantExpr>(l->getKid(0))) { // (k-a)-b = k-(a+b)
			return SubExpr::create(l->getKid(0), AddExpr::create(l->getKid(1), r));
		} else if (rk == Expr::Add && isa<ConstantExpr>(r->getKid(0))) { // a - (k+b) = (a-c) - k
			return SubExpr::create(SubExpr::create(l, r->getKid(1)), r->getKid(0));
		} else if (rk == Expr::Sub && isa<ConstantExpr>(r->getKid(0))) { // a - (k-b) = (a+b) - k
			return SubExpr::create(AddExpr::create(l, r->getKid(1)), r->getKid(0));
		} else {
			return SubExpr::alloc(l, r);
		}
	}
}

static klee::ref<Expr> MulExpr_createPartialR(const klee::ref<ConstantExpr> &cl, Expr *r) {
	Expr::Width type = cl->getWidth();

	if (type == Expr::Bool) {
		return AndExpr_createPartialR(cl, r);
	} else if (cl->isOne()) {
		return r;
	} else if (cl->isZero()) {
		return cl;
	} else {
		return MulExpr::alloc(cl, r);
	}
}
static klee::ref<Expr> MulExpr_createPartial(Expr *l, const klee::ref<ConstantExpr> &cr) {
	return MulExpr_createPartialR(cr, l);
}
static klee::ref<Expr> MulExpr_create(Expr *l, Expr *r) {
	Expr::Width type = l->getWidth();

	if (type == Expr::Bool) {
		return AndExpr::alloc(l, r);
	} else {
		return MulExpr::alloc(l, r);
	}
}

static klee::ref<Expr> AndExpr_createPartial(Expr *l, const klee::ref<ConstantExpr> &cr) {
	if (cr->isAllOnes()) {
		return l;
	} else if (cr->isZero()) {
		return cr;
	} else {
		return AndExpr::alloc(l, cr);
	}
}
static klee::ref<Expr> AndExpr_createPartialR(const klee::ref<ConstantExpr> &cl, Expr *r) {
	return AndExpr_createPartial(r, cl);
}
static klee::ref<Expr> AndExpr_create(Expr *l, Expr *r) {
	return AndExpr::alloc(l, r);
}

static klee::ref<Expr> OrExpr_createPartial(Expr *l, const klee::ref<ConstantExpr> &cr) {
	if (cr->isAllOnes()) {
		return cr;
	} else if (cr->isZero()) {
		return l;
	} else {
		return OrExpr::alloc(l, cr);
	}
}
static klee::ref<Expr> OrExpr_createPartialR(const klee::ref<ConstantExpr> &cl, Expr *r) {
	return OrExpr_createPartial(r, cl);
}
static klee::ref<Expr> OrExpr_create(Expr *l, Expr *r) {
	return OrExpr::alloc(l, r);
}

static klee::ref<Expr> XorExpr_createPartialR(const klee::ref<ConstantExpr> &cl, Expr *r) {
	if (cl->isZero()) {
		return r;
	} else if (cl->getWidth() == Expr::Bool) {
		return EqExpr_createPartial(r, ConstantExpr::create(0, Expr::Bool));
	} else {
		return XorExpr::alloc(cl, r);
	}
}

static klee::ref<Expr> XorExpr_createPartial(Expr *l, const klee::ref<ConstantExpr> &cr) {
	return XorExpr_createPartialR(cr, l);
}
static klee::ref<Expr> XorExpr_create(Expr *l, Expr *r) {
	return XorExpr::alloc(l, r);
}

static klee::ref<Expr> UDivExpr_create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	if (l->getWidth() == Expr::Bool) { // r must be 1
		return l;
	} else {
		return UDivExpr::alloc(l, r);
	}
}

static klee::ref<Expr> SDivExpr_create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	if (l->getWidth() == Expr::Bool) { // r must be 1
		return l;
	} else {
		return SDivExpr::alloc(l, r);
	}
}

static klee::ref<Expr> URemExpr_create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	if (l->getWidth() == Expr::Bool) { // r must be 1
		return ConstantExpr::create(0, Expr::Bool);
	} else {
		return URemExpr::alloc(l, r);
	}
}

static klee::ref<Expr> SRemExpr_create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	if (l->getWidth() == Expr::Bool) { // r must be 1
		return ConstantExpr::create(0, Expr::Bool);
	} else {
		return SRemExpr::alloc(l, r);
	}
}

static klee::ref<Expr> ShlExpr_create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	if (l->getWidth() == Expr::Bool) { // l & !r
		return AndExpr::create(l, Expr::createIsZero(r));
	} else {
		return ShlExpr::alloc(l, r);
	}
}

static klee::ref<Expr> LShrExpr_create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	if (l->getWidth() == Expr::Bool) { // l & !r
		return AndExpr::create(l, Expr::createIsZero(r));
	} else {
		return LShrExpr::alloc(l, r);
	}
}

static klee::ref<Expr> AShrExpr_create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	if (l->getWidth() == Expr::Bool) { // l
		return l;
	} else {
		return AShrExpr::alloc(l, r);
	}
}

#define BCREATE_R(_e_op, _op, partialL, partialR) \
klee::ref<Expr>  _e_op ::create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) { \
  assert(l->getWidth()==r->getWidth() && "type mismatch");              \
  if (ConstantExpr *cl = dyn_cast<ConstantExpr>(l)) {                   \
    if (ConstantExpr *cr = dyn_cast<ConstantExpr>(r)){                  \
      return cl->_op(cr);                             					\
    }																	\
    return _e_op ## _createPartialR(cl, r.get());                       \
  } else if (ConstantExpr *cr = dyn_cast<ConstantExpr>(r)) {            \
    return _e_op ## _createPartial(l.get(), cr);                        \
  }                                                                     \
  return _e_op ## _create(l.get(), r.get());                            \
}

#define BCREATE(_e_op, _op) \
klee::ref<Expr>  _e_op ::create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) { \
  assert(l->getWidth()==r->getWidth() && "type mismatch");          \
  if (ConstantExpr *cl = dyn_cast<ConstantExpr>(l))                 \
    if (ConstantExpr *cr = dyn_cast<ConstantExpr>(r))               \
      return cl->_op(cr);                                           \
  return _e_op ## _create(l, r);                                    \
}

BCREATE_R(AddExpr, Add, AddExpr_createPartial, AddExpr_createPartialR)
BCREATE_R(SubExpr, Sub, SubExpr_createPartial, SubExpr_createPartialR)
BCREATE_R(MulExpr, Mul, MulExpr_createPartial, MulExpr_createPartialR)
BCREATE_R(AndExpr, And, AndExpr_createPartial, AndExpr_createPartialR)
BCREATE_R(OrExpr, Or, OrExpr_createPartial, OrExpr_createPartialR)
BCREATE_R(XorExpr, Xor, XorExpr_createPartial, XorExpr_createPartialR)
BCREATE(UDivExpr, UDiv)
BCREATE(SDivExpr, SDiv)
BCREATE(URemExpr, URem)
BCREATE(SRemExpr, SRem)
BCREATE(ShlExpr, Shl)
BCREATE(LShrExpr, LShr)
BCREATE(AShrExpr, AShr)

#define CMPCREATE(_e_op, _op) \
klee::ref<Expr>  _e_op ::create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) { \
  assert(l->getWidth()==r->getWidth() && "type mismatch");              \
  if (ConstantExpr *cl = dyn_cast<ConstantExpr>(l))                     \
    if (ConstantExpr *cr = dyn_cast<ConstantExpr>(r))                   \
      return cl->_op(cr);                                               \
  return _e_op ## _create(l, r);                                        \
}

#define CMPCREATE_T(_e_op, _op, _reflexive_e_op, partialL, partialR) 	\
  klee::ref<Expr>  _e_op ::create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {   \
  assert(l->getWidth()==r->getWidth() && "type mismatch");             	\
  if (ConstantExpr *cl = dyn_cast<ConstantExpr>(l)) {                  	\
    if (ConstantExpr *cr = (dyn_cast<ConstantExpr>(r)))                 \
      return cl->_op(cr);                                              	\
    return partialR(cl, r.get());                                      	\
  } else if (ConstantExpr *cr = (dyn_cast<ConstantExpr>(r))) {          \
    return partialL(l.get(), cr);                                      	\
  } else {                                                             	\
    return _e_op ## _create(l.get(), r.get());                         	\
  }                                                                    	\
}

static klee::ref<Expr> EqExpr_create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	if (l == r) {
		return ConstantExpr::alloc(1, Expr::Bool);
	} else {
		return EqExpr::alloc(l, r);
	}
}

/// Tries to optimize EqExpr cl == rd, where cl is a ConstantExpr and
/// rd a ReadExpr.  If rd is a read into an all-constant array,
/// returns a disjunction of equalities on the index.  Otherwise,
/// returns the initial equality expression. 
static klee::ref<Expr> TryConstArrayOpt(const klee::ref<ConstantExpr> &cl, ReadExpr *rd) {
	if (rd->updates.root->isSymbolicArray() || rd->updates.getSize()) return EqExpr_create(cl, rd);

	// Number of positions in the array that contain value ct.
	unsigned numMatches = 0;

	// for now, just assume standard "flushing" of a concrete array,
	// where the concrete array has one update for each index, in order
	klee::ref<Expr> res = ConstantExpr::alloc(0, Expr::Bool);
	for (unsigned i = 0, e = rd->updates.root->size; i != e; ++i) {
		if (cl == rd->updates.root->constantValues[i]) {
			// Arbitrary maximum on the size of disjunction.
			if (++numMatches > 100) return EqExpr_create(cl, rd);

			klee::ref<Expr> mayBe = EqExpr::create(rd->index, ConstantExpr::alloc(i, rd->index->getWidth()));
			res = OrExpr::create(res, mayBe);
		}
	}

	return res;
}

static klee::ref<Expr> EqExpr_createPartialR(const klee::ref<ConstantExpr> &cl, Expr *r) {
	Expr::Width width = cl->getWidth();

	Expr::Kind rk = r->getKind();
	if (width == Expr::Bool) {
		if (cl->isTrue()) {
			return r;
		} else {
			// 0 == ...

			if (rk == Expr::Eq) {
				const EqExpr *ree = cast<EqExpr>(r);

				// eliminate double negation
				if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(ree->left))) {
					// 0 == (0 == A) => A
					if (CE->getWidth() == Expr::Bool && CE->isFalse()) return ree->right;
				}
			} else if (rk == Expr::Or) {
				const OrExpr *roe = cast<OrExpr>(r);

				// transform not(or(a,b)) to and(not a, not b)
				return AndExpr::create(Expr::createIsZero(roe->left), Expr::createIsZero(roe->right));
			}
		}
	} else if (rk == Expr::SExt) {
		// (sext(a,T)==c) == (a==c)
		const SExtExpr *see = cast<SExtExpr>(r);
		Expr::Width fromBits = see->src->getWidth();
		klee::ref<ConstantExpr> trunc = cl->ZExt(fromBits);

		// pathological check, make sure it is possible to
		// sext to this value *from any value*
		if (cl == trunc->SExt(width)) {
			return EqExpr::create(see->src, trunc);
		} else {
			return ConstantExpr::create(0, Expr::Bool);
		}
	} else if (rk == Expr::ZExt) {
		// (zext(a,T)==c) == (a==c)
		const ZExtExpr *zee = cast<ZExtExpr>(r);
		Expr::Width fromBits = zee->src->getWidth();
		klee::ref<ConstantExpr> trunc = cl->ZExt(fromBits);

		// pathological check, make sure it is possible to
		// zext to this value *from any value*
		if (cl == trunc->ZExt(width)) {
			return EqExpr::create(zee->src, trunc);
		} else {
			return ConstantExpr::create(0, Expr::Bool);
		}
	} else if (rk == Expr::Add) {
		const AddExpr *ae = cast<AddExpr>(r);
		if (isa<ConstantExpr>(ae->left)) {
			// c0 = c1 + b => c0 - c1 = b
			return EqExpr_createPartialR(cast<ConstantExpr>(SubExpr::create(cl, ae->left)), ae->right.get());
		}
	} else if (rk == Expr::Sub) {
		const SubExpr *se = cast<SubExpr>(r);
		if (isa<ConstantExpr>(se->left)) {
			// c0 = c1 - b => c1 - c0 = b
			return EqExpr_createPartialR(cast<ConstantExpr>(SubExpr::create(se->left, cl)), se->right.get());
		}
	} else if (rk == Expr::Read && ConstArrayOpt) {
		return TryConstArrayOpt(cl, static_cast<ReadExpr*>(r));
	}

	return EqExpr_create(cl, r);
}

static klee::ref<Expr> EqExpr_createPartial(Expr *l, const klee::ref<ConstantExpr> &cr) {
	return EqExpr_createPartialR(cr, l);
}

klee::ref<Expr> NeExpr::create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	return EqExpr::create(ConstantExpr::create(0, Expr::Bool), EqExpr::create(l, r));
}

klee::ref<Expr> UgtExpr::create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	return UltExpr::create(r, l);
}
klee::ref<Expr> UgeExpr::create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	return UleExpr::create(r, l);
}

klee::ref<Expr> SgtExpr::create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	return SltExpr::create(r, l);
}
klee::ref<Expr> SgeExpr::create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	return SleExpr::create(r, l);
}

static klee::ref<Expr> UltExpr_create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	Expr::Width t = l->getWidth();
	if (t == Expr::Bool) { // !l && r
		return AndExpr::create(Expr::createIsZero(l), r);
	} else {
		return UltExpr::alloc(l, r);
	}
}

static klee::ref<Expr> UleExpr_create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	if (l->getWidth() == Expr::Bool) { // !(l && !r)
		return OrExpr::create(Expr::createIsZero(l), r);
	} else {
		return UleExpr::alloc(l, r);
	}
}

static klee::ref<Expr> SltExpr_create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	if (l->getWidth() == Expr::Bool) { // l && !r
		return AndExpr::create(l, Expr::createIsZero(r));
	} else {
		klee::ref<Expr> tmp = SltExpr::alloc(l, r);
		return SltExpr::alloc(l, r);
	}
}

static klee::ref<Expr> SleExpr_create(const klee::ref<Expr> &l, const klee::ref<Expr> &r) {
	if (l->getWidth() == Expr::Bool) { // !(!l && r)
		return OrExpr::create(l, Expr::createIsZero(r));
	} else {
		return SleExpr::alloc(l, r);
	}
}

CMPCREATE_T(EqExpr, Eq, EqExpr, EqExpr_createPartial, EqExpr_createPartialR)
CMPCREATE(UltExpr, Ult)
CMPCREATE(UleExpr, Ule)
CMPCREATE(SltExpr, Slt)
CMPCREATE(SleExpr, Sle)
