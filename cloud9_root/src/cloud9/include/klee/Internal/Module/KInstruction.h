//===-- KInstruction.h ------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_KINSTRUCTION_H
#define KLEE_KINSTRUCTION_H

#include "klee/Config/config.h"
#include "klee/Expr.h"
#include "klee/Internal/Module/KModule.h"

#include <vector>

namespace llvm {
class Instruction;
}

namespace klee {
class Executor;
struct InstructionInfo;
class KModule;

/// KInstruction - Intermediate instruction representation used
/// during execution.
struct KInstruction {
	llvm::Instruction *inst;
	const InstructionInfo *info;

	/// Value numbers for each operand. -1 is an invalid value,
	/// otherwise negative numbers are indices (negated and offset by
	/// 2) into the module constant table and positive numbers are
	/// register indices.
	int *operands;
	/// Destination register index.
	unsigned dest;

	/// KInstruction id in its parent function, sjguo
	unsigned index;

	// Daniel, to count assert proof/alarm
	bool assertBr;
	bool assert;

	// Daniel, mark a sbox-related instruction
	std::string sbox;
public:
	virtual ~KInstruction();
};

struct KGEPInstruction: KInstruction {
	/// indices - The list of variable sized adjustments to add to the pointer
	/// operand to execute the instruction. The first element is the operand
	/// index into the GetElementPtr instruction, and the second element is the
	/// element size to multiple that index by.
	std::vector<std::pair<unsigned, uint64_t> > indices;

	/// offset - A constant offset to add to the pointer operand to execute the
	/// instruction.
	uint64_t offset;
};

struct KCallInstruction: KInstruction {
	bool vulnerable; // Whether the result of this call is unchecked, and
					 // thus may lead to further errors
	static bool classof(const KInstruction *) {
		return true;
	}
};

struct ShadowInstruction {
	uint64_t thread_id;
	KFunction *kf;
	KInstruction *kInst;
	std::string bpp_id;
	ShadowInstruction *prev;
	ShadowInstruction *next;
	klee::ref<Expr> base;

	Expr::Width width;
	std::string caller;

	std::string ipp_id;
	std::string bpp_ID;
	unsigned direction;

	std::string callee;

	bool constantCondition;
	unsigned retDest;

	int incomingBBIndex;

	std::vector<std::pair<int, int> > operandLocations;

	std::vector<std::string> operandNames;
	std::vector<Expr::Width> operandWidth;
	std::map<std::string, klee::ref<Expr> > constantParaMap;

	// if a func is declared and has void return value
	bool noRetDeclaration;

	// record the return value of a function
	klee::ref<Expr> retValue;

	// value for GetElementPtr instruction
	klee::ref<Expr> gepValue;

	// just skip this instruction during wp computation, if this flag is set to true
	bool skipInst;

	// If the corresponding instruction is accessing a global variable
	bool isGlobal;

public:
	ShadowInstruction(KFunction *kf, KInstruction *kInst, string shadowID, uint64_t tid) :
			kf(kf), kInst(kInst), bpp_id(shadowID) {
		thread_id = tid;
		prev = NULL;
		next = NULL;

		width = 0;

		constantCondition = false;
		direction = -1;

		/* for Instruction::PHI */
		incomingBBIndex = -1;

		noRetDeclaration = false;
		retValue = NULL;

		skipInst = false;
		isGlobal = false;
	}

	ShadowInstruction(KFunction *kf, KInstruction *kInst, uint64_t tid) :
			kf(kf), kInst(kInst) {
		thread_id = tid;
		prev = NULL;
		next = NULL;

		width = 0;
		constantCondition = false;
		direction = -1;

		/* for Instruction::PHI */
		incomingBBIndex = -1;

		noRetDeclaration = false;
		retValue = NULL;

		skipInst = false;
		isGlobal = false;
	}
};
}

#endif

