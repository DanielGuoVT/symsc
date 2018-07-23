//===-- Context.cpp -------------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Context.h"

#include "klee/Expr.h"

#include "llvm/Type.h"
#include "llvm/DerivedTypes.h"

#include <cassert>

using namespace klee;

static bool Initialized = false;
static Context TheContext;

void Context::initialize(bool IsLittleEndian, Expr::Width PointerWidth) {
	Initialized = false; // Daniel
	assert(!Initialized && "Duplicate context initialization!");
	TheContext = Context(IsLittleEndian, PointerWidth);
	Initialized = true;
}

const Context &Context::get() {
	assert(Initialized && "Context has not been initialized!");
	return TheContext;
}

// FIXME: This is a total hack, just to avoid a layering issue until this stuff
// moves out of Expr.

klee::ref<Expr> Expr::createCoerceToPointerType(klee::ref<Expr> e) {
	return ZExtExpr::create(e, Context::get().getPointerWidth());
}

klee::ref<ConstantExpr> Expr::createPointer(uint64_t v) {
	return ConstantExpr::create(v, Context::get().getPointerWidth());
}