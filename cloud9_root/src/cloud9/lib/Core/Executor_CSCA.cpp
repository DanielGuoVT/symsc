/*
 * Executor.cpp
 *
 *  Created on: Jun 29, 2017
 *      Author: sjguo
 */

#include <stdio.h>
#include <cmath>
#include <map>
#include <iostream>
#include "klee/Expr.h"
#include "klee/ExecutionState.h"
#include "klee/Executor.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "Memory.h"
#include "TimingSolver.h"

#include "llvm/Instruction.h"

using namespace std;
using namespace klee;
using namespace llvm;

namespace klee {

static int t2idx = 0;

int Executor::log_base2(int n) {
	int power = 0;
	if (n <= 0 || (n & (n - 1)) != 0)
	assert(0 && "log2() only works for positive power of two values");
	while (n >>= 1)
		power++;
	return power;
}

unsigned Executor::getCacheSet(uint64_t address) {
	return ((address) >> log_base2(Executor::cacheLineSize)) & (Executor::cacheSetNum - 1);
}

ref<Expr> Executor::getCacheSet(ref<Expr> address) {
	ref<Expr> base_addr = LShrExpr::create(address, ConstantExpr::alloc(log_base2(Executor::cacheLineSize), address->getWidth()));
	return AndExpr::create(base_addr, ConstantExpr::alloc(cacheSetNum - 1, address->getWidth()));
}

unsigned Executor::getCacheTag(uint64_t address) {
	return (address >> (log_base2(Executor::cacheLineSize) + log_base2(Executor::cacheSetNum)));
}

ref<Expr> Executor::getCacheTag(ref<Expr> address) {
	return LShrExpr::create(address, ConstantExpr::create(log_base2(Executor::cacheLineSize) + log_base2(Executor::cacheSetNum), address->getWidth()));
}

ref<Expr> Executor::createCacheSetCompareConstraint(ref<Expr> &addr1, ref<Expr> &addr2) {
	if (addr1->getKind() == Expr::Constant) {
		if (addr2->getKind() == Expr::Constant) {
			return Executor::createCacheSetCompareConstraintCC(addr1, addr2);
		} else {
			return Executor::createCacheSetCompareConstraintSC(addr1, addr2);
		}
	} else {
		if (addr2->getKind() == Expr::Constant) {
			return Executor::createCacheSetCompareConstraintSC(addr1, addr2);
		} else {
			return Executor::createCacheSetCompareConstraintSS(addr1, addr2);
		}
	}
}

ref<Expr> Executor::createCacheTagCompareConstraint(ref<Expr> &addr1, ref<Expr> &addr2) {
	if (addr1->getKind() == Expr::Constant) {
		if (addr2->getKind() == Expr::Constant) {
			return Executor::createCacheTagCompareConstraintCC(addr1, addr2);
		} else {
			return Executor::createCacheTagCompareConstraintSC(addr1, addr2);
		}
	} else {
		if (addr2->getKind() == Expr::Constant) {
			return Executor::createCacheTagCompareConstraintSC(addr1, addr2);
		} else {
			return Executor::createCacheTagCompareConstraintSS(addr1, addr2);
		}
	}
}

void Executor::previousMethod(ExecutionState &state) {
	bool flag = false;
	unsigned size = state.memAddrs.size();
	for (unsigned idx = 1; idx < size; idx++) {
		if (state.memAddrs[idx]->threadID == 2) {
			t2idx = idx;
			flag = true;
			std::cerr << std::endl;
			std::cerr << "---------------------------------------------------------" << std::endl;
			std::cerr << "idx: " << idx << std::endl;
		}
		if (flag) {
			computeIndividualConstraint(state, solver, idx, state.memAddrs[idx]);
		}
	}
}

ref<Expr> Executor::createCacheSetCompareConstraintCC(ref<Expr> &addr1, ref<Expr> &addr2) {
	ref<ConstantExpr> CE1 = dyn_cast<ConstantExpr>(addr1);
	uint64_t mem_addr1 = CE1->getZExtValue();
	unsigned set1 = getCacheSet(mem_addr1);

	ref<ConstantExpr> CE2 = dyn_cast<ConstantExpr>(addr2);
	uint64_t mem_addr2 = CE2->getZExtValue();
	unsigned set2 = getCacheSet(mem_addr2);

	if (set1 == set2) return ConstantExpr::create(1, Expr::Bool);
	else return ConstantExpr::create(0, Expr::Bool);
}

ref<Expr> Executor::createCacheTagCompareConstraintCC(ref<Expr> &addr1, ref<Expr> &addr2) {
	ConstantExpr* CE1 = dyn_cast<ConstantExpr>(addr1);
	uint64_t mem_addr1 = CE1->getZExtValue();
	unsigned tag1 = getCacheTag(mem_addr1);

	ConstantExpr* CE2 = dyn_cast<ConstantExpr>(addr2);
	uint64_t mem_addr2 = CE2->getZExtValue();
	unsigned tag2 = getCacheTag(mem_addr2);

	if (tag1 == tag2) return ConstantExpr::create(1, Expr::Bool);
	else return ConstantExpr::create(0, Expr::Bool);
}
//
// generate set equality constraints for <symbolic, constant> address pairs
ref<Expr> Executor::createCacheSetCompareConstraintSC(ref<Expr> &addr1, ref<Expr> &addr2) {
	unsigned set;
	klee::ref<Expr> set_addr;
	std::pair<klee::ref<Expr>, klee::ref<Expr> > setPair(addr1, addr2);

	// try to look up in cache
	if (setPairCache.count(setPair)) {
		return setPairCache[setPair];
	}

	// either addr1 or addr2 is constant expr
	if (addr1->getKind() == Expr::Constant) {
		ConstantExpr* CE = dyn_cast<ConstantExpr>(addr1);
		uint64_t addr_mem = CE->getZExtValue();
		set = getCacheSet(addr_mem);

		if (setCache.count(addr2)) set_addr = setCache[addr2];
		else {
			set_addr = getCacheSet(addr2);
			setCache[addr2] = set_addr;
		}
	} else {
		ConstantExpr* CE = dyn_cast<ConstantExpr>(addr2);
		uint64_t addr_mem = CE->getZExtValue();
		set = getCacheSet(addr_mem);
		if (setCache.count(addr1)) set_addr = setCache[addr1];
		else {
			set_addr = getCacheSet(addr1);
			setCache[addr1] = set_addr;
		}
	}

	setPairCache[setPair] = EqExpr::create(set_addr, ConstantExpr::alloc(set, set_addr->getWidth()));
	return setPairCache[setPair];
}

// generate tag inequality constraints for <symbolic, constant> address pairs
ref<Expr> Executor::createCacheTagCompareConstraintSC(ref<Expr> &addr1, ref<Expr> &addr2) {
	int tag;
	ref<Expr> tag_addr;
	std::pair<ref<Expr>, ref<Expr> > tagPair(addr1, addr2);

	// look up the cache
	if (tagPairCache.count(tagPair)) {
		return tagPairCache[tagPair];
	}

	// either addr1 or addr2 is constant expr
	if (addr1->getKind() == Expr::Constant) {
		ConstantExpr* CE = dyn_cast<ConstantExpr>(addr1);
		uint64_t addr_mem = CE->getZExtValue();
		tag = getCacheTag(addr_mem);
		if (tagCache.count(addr2)) tag_addr = tagCache[addr2];
		else { /* allocate new memory and store in the cache */
			tag_addr = this->getCacheTag(addr2);
			tagCache[addr2] = tag_addr;
		}
	} else {
		ConstantExpr* CE = dyn_cast<ConstantExpr>(addr2);
		uint64_t addr_mem = CE->getZExtValue();
		tag = getCacheTag(addr_mem);
		if (tagCache.count(addr1)) tag_addr = tagCache[addr1];
		else { /* allocate new memory and store in the cache */
			tag_addr = getCacheTag(addr1);
			tagCache[addr1] = tag_addr;
		}
	}

	tagPairCache[tagPair] = EqExpr::create(tag_addr, ConstantExpr::alloc(tag, tag_addr->getWidth()));
	return tagPairCache[tagPair];
}

// generate tag inequality constraints for <symbolic, symbolic> address pairs
ref<Expr> Executor::createCacheSetCompareConstraintSS(ref<Expr> &addr1, ref<Expr> &addr2) {
	std::pair<ref<Expr>, ref<Expr> > setPair(addr1, addr2);
	ref<Expr> set_addr1, set_addr2;

	if (setPairCache.count(setPair)) {
		return setPairCache[setPair];
	}

	if (setCache.count(addr1)) set_addr1 = setCache[addr1];
	else {
		set_addr1 = getCacheSet(addr1);
		setCache[addr1] = set_addr1;
	}
	if (setCache.count(addr2)) set_addr2 = setCache[addr2];
	else {
		set_addr2 = getCacheSet(addr2);
		setCache[addr2] = set_addr2;
	}

	setPairCache[setPair] = EqExpr::create(set_addr1, set_addr2);
	return setPairCache[setPair];
}

// generate tag inequality constraints for <symbolic,symbolic> constraint pairs
ref<Expr> Executor::createCacheTagCompareConstraintSS(ref<Expr> &addr1, ref<Expr> &addr2) {
	std::pair<ref<Expr>, ref<Expr> > tagPair(addr1, addr2);
	ref<Expr> tag_addr1, tag_addr2;

	if (tagPairCache.count(tagPair)) {
		return tagPairCache[tagPair];
	}
	if (tagCache.count(addr1)) tag_addr1 = tagCache[addr1];
	else {
		tag_addr1 = getCacheTag(addr1);
		tagCache[addr1] = tag_addr1;
	}
	if (tagCache.count(addr2)) tag_addr2 = tagCache[addr2];
	else {
		tag_addr2 = getCacheTag(addr2);
		tagCache[addr2] = tag_addr2;
	}
	tagPairCache[tagPair] = EqExpr::create(tag_addr1, tag_addr2);
	return tagPairCache[tagPair];
}

ref<Expr> Executor::createNotReloadConstraint(unsigned crtIdx, ref<Expr> &target, ExecutionState &state) {
	ref<Expr> crt = state.memAddrs[crtIdx];
	ref<Expr> setCmp = Executor::createCacheSetCompareConstraint(target, crt);

	if (setCmp->getKind() == Expr::Constant) {
		if (setCmp->isTrue()) return ConstantExpr::create(0, Expr::Bool);
		else return ConstantExpr::create(1, Expr::Bool);
	}
	return NotExpr::create(setCmp);
}

void Executor::processCacheAccessTwoStep(ExecutionState &state, TimingSolver *solver) {
	/**
	 * reset the cache
	 */
//	setPairCache.clear();
//	cmpFalseCache.clear();
//	tagPairCache.clear();
//	setCache.clear();
//	tagCache.clear();
	if (state.memAddrs.size() <= 1) {
		std::cerr << "less than two memory addresses in trace, return." << std::endl;
	}

	std::vector<ref<Expr> > &addrs = state.memAddrs;
	std::vector<KInstruction*> &insts = state.memInsts;
	std::cerr << "sym memory  accs size: " << addrs.size() << std::endl;

	/**
	 * double check the sym addresses
	 */
	for (unsigned i = 0; i < addrs.size(); i++) {
		ref<Expr> &addr = addrs[i];
		if (dyn_cast<ConstantExpr>(addr)) assert(0);
		KInstruction *ki = state.memInsts[i];
		if (ki->sbox == "") assert(0);
		std::cerr << "sym addr at idx: " << i << ", sbox: " << ki->sbox << std::endl;
	}

	/**
	 * get the last addr of crt trace
	 */
	int i = addrs.size() - 1;
	ref<Expr> &addr0 = addrs[i];
	KInstruction *ki = insts[i];
	std::cerr << "source line: " << ki->info->line << ", idx: " << i << ", sbox: " << ki->sbox << std::endl;

	/**
	 * pair i and j for may-equal analysis
	 */
	int result = 0;
	for (int j = i - 1; j >= 0; j--) {
		ref<Expr> &addr1 = addrs[j];
		KInstruction *kj = insts[j];
		if (ki->sbox != kj->sbox) continue;
		std::cerr << "source line: " << kj->info->line << ", idx: " << j << ", sbox: " << kj->sbox << std::endl;

		if (addr0 == addr1) {
			std::cerr << "find two identical addrs, start step 1, i: " << i << ", j: " << j << std::endl;
			result = computeSolutionTwoStep(state, solver, i, j, true);
			break;
		} else {
			// check if two memory accesses may point to same address
			std::cerr << std::endl << "Start step 1, i: " << i << ", j: " << j << ", sbox: " << state.memInsts[j]->sbox << std::endl;
			result = computeSolutionTwoStep(state, solver, i, j);
			if (result) {
				test++;
				break;
			} else {
				fail++;
			}
		}
	}
}

int Executor::computeSolutionTwoStep(ExecutionState &state, TimingSolver *solver, unsigned idx1, unsigned idx2, bool identical) {
	ExecutionState *copy = fork(state, KLEE_FORK_CACHE_SIDE_CHANNEL).first;
	// construct the constraint
	std::vector<ref<Expr> > &addrs = copy->memAddrs;
	ref<Expr> &addr0 = addrs[idx1];
	ref<Expr> &addr1 = addrs[idx2];

	std::vector<const Array*> objects;
	for (unsigned i = 3, size = copy->symbolics.size(); i < size; ++i) {
		objects.push_back(copy->symbolics[i].second);
	}

	ref<Expr> symAddr;
	if (fixed_exec) {
		symAddr = ConstantExpr::create(fixed_addr, Expr::Int64);
		std::cerr << "fixed_addr: " << symAddr << ", cache set: " << getCacheSet(fixed_addr) << std::endl;
	} else {
		const Array *syn = Array::CreateArray("sym_addr", Expr::Int8);
		objects.push_back(syn);
		symAddr = Expr::createTempRead(syn, Expr::Int64);
	}

	if (!identical) {
		copy->addConstraint(EqExpr::create(addr0, addr1));
	}

	copy->addConstraint(createCacheSetCompareConstraint(addr1, symAddr));
	// synthesized address should not be addr0 or addr1 (may not needed)
	if (!fixed_exec) {
		copy->addConstraint(SgtExpr::create(symAddr, addr1));
	}

	std::vector<vector<uint8_t> > solvedValues;
	solver->setTimeout(0);
	bool success = false;
	success = solver->getInitialValues(data::SIDE_CHANNEL_ANALYSIS, *copy, objects, solvedValues);

	if (success) {
		std::cerr << "Find possible leak point, now start step 2." << std::endl;
		std::stringstream ss1;
		std::vector<klee::ref<ConstantExpr> > synAddrBytes;
		uint64_t addr = 0; // addr is the concrete synthesized address in step 1

		for (unsigned i = 0; i < objects.size(); i++) {
			std::string name = objects[i]->getName();
			if (!name.compare("dummy")) continue;
			ss1 << name << ": 0x";
			for (size_t j = 0, size = solvedValues[i].size(); j < size; j++) {
				char buf[8];
				sprintf(buf, "%02x", solvedValues[i][size - j - 1]);
				ss1 << buf;
				if (name == "sym_addr") {
					synAddrBytes.push_back(ConstantExpr::create(solvedValues[i][j], Expr::Int8));
					addr |= (solvedValues[i][j] << (j * 8));
				}
			}
			ss1 << "\n";
		}

		ExecutionState *copy1 = fork(state, KLEE_FORK_CACHE_SIDE_CHANNEL).first;
		std::vector<const Array*> objects1;
		for (unsigned i = 3, size = state.symbolics.size(); i < size; i++) {
			objects1.push_back(copy1->symbolics[i].second);
		}

		// copy the synthesized address from step1
		ref<Expr> symAddr1;
		if (fixed_exec) {
			symAddr1 = symAddr;
			ss1 << "sym_addr maps to cache set: " << getCacheSet(symAddr1) << "\n";
		} else {
			const Array *syn1 = Array::CreateArray("sym_addr1", Expr::Int8, &synAddrBytes[0], &synAddrBytes[0] + synAddrBytes.size());
			symAddr1 = Expr::createTempRead(syn1, Expr::Int64);
			ss1 << "sym_addr maps to cache set: " << getCacheSet(addr) << "\n";
		}

		// addr1 = addr2 && they cannot map to same cache line with syn1
		addr0 = copy1->memAddrs[idx1];
		addr1 = copy1->memAddrs[idx2];
		if (!identical) {
			copy1->addConstraint(EqExpr::create(addr0, addr1));
		}
		copy1->addConstraint(NotExpr::create(createCacheSetCompareConstraint(addr1, symAddr1)));

		std::vector<vector<uint8_t> > solvedValues1;
		solver->setTimeout(0);
		bool success1 = false;
		success1 = solver->getInitialValues(data::SIDE_CHANNEL_ANALYSIS, *copy1, objects1, solvedValues1);

		if (success1) {
			ss1 << "find a solution in step 2\n";
			for (unsigned i = 0; i < objects1.size(); i++) {
				std::string name = objects1[i]->getName();
				if (!name.compare("dummy")) continue;
				ss1 << name << ": 0x";
				for (unsigned j = 0, size = solvedValues1[i].size(); j < size; j++) {
					char buf[8];
					sprintf(buf, "%02x", solvedValues1[i][size - j - 1]);
					ss1 << buf;
				}
				ss1 << "\n";
			}
			copy->isRedundant = true;
			terminateRedundantState(*copy);
			copy1->isRedundant = true;
			terminateRedundantState(*copy1);
			std::cerr << ss1.str() << std::endl;
			return 1;
		} else {
			copy->isRedundant = true;
			terminateRedundantState(*copy);
			copy1->isRedundant = true;
			terminateRedundantState(*copy1);
			std::cerr << "no solution in step 2." << std::endl;
			return 0;
		}
	} else {
		std::cerr << "No solution in step 1 for i: " << idx1 << ", j: " << idx2 << std::endl << std::endl;
		copy->isRedundant = true;
		terminateRedundantState(*copy);
		return 0;
	}
}

void Executor::processCacheAccessPrecise(ExecutionState &state, TimingSolver *solver) {
	/**
	 * reset the cache
	 */
	setPairCache.clear();
	cmpFalseCache.clear();
	tagPairCache.clear();
	setCache.clear();
	tagCache.clear();

	if (state.memAddrs.size() <= 1) {
		std::cerr << "less than two memory addresses in trace, return." << std::endl;
	}
	std::cerr << "-------------------------------------------------------" << std::endl;
	std::vector<ref<Expr> > &addrs = state.memAddrs;
	std::vector<ref<Expr> > &shadows = state.shadowMemAddrs;
	if (addrs.size() != shadows.size()) {
		std::cerr << "addrs size: " << addrs.size() << std::endl;
		std::cerr << "shadow size: " << shadows.size() << std::endl;
		assert(0);
	}

	std::vector<KInstruction*> &insts = state.memInsts;
	/**
	 * double check the sym addresses
	 */
	for (unsigned i = 0; i < addrs.size(); i++) {
		ref<Expr> &addr = addrs[i];
		if (dyn_cast<ConstantExpr>(addr)) assert(0);
		KInstruction *ki = state.memInsts[i];
		if (ki->sbox == "") assert(0);
		std::cerr << "sym addr at idx: " << i << ", sbox: " << ki->sbox << std::endl;
	}

	/**
	 * Loop
	 */
	for (unsigned i = 0; i < addrs.size(); i++) {
		KInstruction *ki = insts[i];
		if (this->fixed_exec) {
			MemoryObject *mo = sboxGlobalMOs[ki->sbox];
			unsigned t2set = getCacheSet(fixed_addr);
			unsigned t1SetBeg = getCacheSet(mo->address);
			unsigned t1SetEnd = getCacheSet(mo->address + mo->size);
			std::cerr << "T2's fixed_addr: " << fixed_addr << " maps to cache set: " << getCacheSet(fixed_addr) << std::endl;
			if (t2set < t1SetBeg || t2set > t1SetEnd) {
				std::cerr << "Sbox access to " << ki->sbox << " maps to cache set " << t1SetBeg << " to " << t1SetEnd << std::endl << std::endl;
				continue;
			} else {
				std::cerr << std::endl << "-------------------------------------------------------" << std::endl;
				std::cerr << "Find matching sbox access: " << ki->sbox << " maps to set " << t1SetBeg << " to " << t1SetEnd << std::endl << std::endl;
			}
		}
		std::cerr << "source line: " << ki->info->line << ", idx: " << i << ", sbox: " << ki->sbox << std::endl;
		unsigned result = 0;
		for (unsigned j = i + 1; j < addrs.size(); j++) {
			KInstruction *kj = insts[j];
			if (ki->sbox != kj->sbox) continue;
			std::cerr << "source line: " << kj->info->line << ", idx: " << j << ", sbox: " << kj->sbox << std::endl;
			/**
			 * pair i and j for may-equal analysis
			 */
			result = computeSolutionPrecise(state, solver, i, j);
			inter++;
			std::cerr << "-------------------------------------------------------" << std::endl << std::endl;
			if (result) {
				test++;
				break;
			}
		}
	}
}

int Executor::computeSolutionPrecise(ExecutionState &state, TimingSolver *solver, unsigned idx1, unsigned idx2, bool identical) {
	/**
	 * fork a new state for constraint solving
	 */
	ExecutionState *state1 = fork(state, KLEE_FORK_CACHE_SIDE_CHANNEL).first;

	/**
	 *	clone the symbolic objects from state to copy
	 */
	std::vector<const Array*> objects;
	for (unsigned i = 3, size = state1->symbolics.size(); i < size; ++i) {
		objects.push_back(state1->symbolics[i].second);
	}

	/**
	 * create the address variable in thread T2
	 */
	ref<Expr> symAddr;
	if (fixed_exec) { // fixed-address approach
		symAddr = ConstantExpr::create(fixed_addr, Expr::Int64);
		std::cerr << "fixed_addr: " << symAddr << ", cache set: " << getCacheSet(fixed_addr) << std::endl;
	} else { // symbolic approach
		const Array *sym = Array::CreateArray("sym_addr", Expr::Int8);
		objects.push_back(sym);
		symAddr = Expr::createTempRead(sym, Expr::Int64);
	}

	/**
	 * build the constraints
	 */
	std::vector<ref<Expr> > &addrs1 = state1->memAddrs;
	std::vector<ref<Expr> > &addrs2 = state1->shadowMemAddrs;

	ref<Expr> addr1 = addrs1[idx1];
	ref<Expr> addr2 = addrs1[idx2];
	if (!identical) state1->addConstraint(EqExpr::create(addr1, addr2));
	state1->addConstraint(createCacheSetCompareConstraint(addr1, symAddr));

	ref<Expr> addr3 = addrs2[idx1];
	ref<Expr> addr4 = addrs2[idx2];
	if (!identical) state1->addConstraint(EqExpr::create(addr3, addr4));
	state1->addConstraint(NotExpr::create(createCacheSetCompareConstraint(addr3, symAddr)));

	state1->addConstraint(UgtExpr::create(symAddr, addr3));
	state1->addConstraint(UgtExpr::create(symAddr, addr4));

	std::vector<vector<uint8_t> > solvedValues;
	solver->setTimeout(0);
	bool success = false;
	success = solver->getInitialValues(data::SIDE_CHANNEL_ANALYSIS, *state1, objects, solvedValues);

	if (success) {
		std::cerr << "find leak point in precise approach, T2's address: " << (fixed_exec ? "fixed" : "symbolic") << std::endl << std::endl;
		std::stringstream ss1;

		std::vector<klee::ref<ConstantExpr> > synAddrBytes;
		uint64_t addr = 0; // addr is the concretized synthesizing address

		for (unsigned i = 0; i < objects.size(); i++) {
			std::string name = objects[i]->getName();

			if (name.compare("dummy") == 0) {
				if (fixed_exec) {
					continue;
				} else {
					ss1 << "sym_addr: 0x";
				}
			} else {
				ss1 << name << ": 0x";
			}

			for (size_t j = 0, size = solvedValues[i].size(); j < size; j++) {
				char buf[8];
				sprintf(buf, "%02x", solvedValues[i][size - j - 1]);
				ss1 << buf;
				if (name == "dummy") {
					synAddrBytes.push_back(ConstantExpr::create(solvedValues[i][j], Expr::Int8));
					addr |= (solvedValues[i][j] << (j * 8));
				}
			}
			ss1 << "\n";
		}
		std::cerr << ss1.str();
//		if (!fixed_exec) {
//			std::cerr << "sym_addr maps to cache line: " << getCacheSet(addr) << std::endl << std::endl;
//		}
	} else {
		std::cerr << "No solution in precise approach for i: " << idx1 << ", j: " << idx2 << std::endl << std::endl;
	}

	state1->isRedundant = true;
	terminateRedundantState(*state1);
	return (success ? 1 : 0);
}

int Executor::computeSolution(ExecutionState &state, TimingSolver *solver, bool identical) {
	if (state.symbolicHitVars.empty()) {
		return 0;
	}

	std::vector<const Array *> allObjects;
	std::vector<std::pair<unsigned, const Array*> > &vars = state.symbolicHitVars;

	for (size_t i = 0; i < state.symbolics.size(); i++) {
		allObjects.push_back(state.symbolics[i].second);
	}
	for (size_t i = 0; i < vars.size(); i++) {
		allObjects.push_back(vars[i].second);
	}

	ExecutionState::ConstraintsMap &cm = state.constraintsMap;
	ExecutionState::ConstraintsMap::iterator mit = cm.begin();
	for (; mit != cm.end(); mit++) {
		state.addConstraint(mit->second.first);
		state.addConstraint(mit->second.second);
		std::cerr << "constraint: " << mit->second.first << std::endl;
	}

	bool success = true;
	std::vector<vector<unsigned char> > solvedValues;

	while (success) {
		solver->setTimeout(0);
		success = solver->getInitialValues(data::SIDE_CHANNEL_ANALYSIS, state, allObjects, solvedValues);
		if (!success) {
			std::cerr << "[DBG] finish solving ... " << std::endl;
		} else {
			std::vector<std::pair<const MemoryObject*, const Array*> > &symbolics = state.symbolics;
			std::stringstream ss1;
			std::stringstream ss2;
			std::stringstream ss3;
			for (size_t i = 3; i < symbolics.size(); i++) {
				std::string name = symbolics[i].second->getName();
				ss1 << name << ": ";
				for (size_t j = 0; j < solvedValues[i].size(); j++) {
					char buf[8];
					sprintf(buf, "%hhu", solvedValues[i][j]);
					ss1 << buf;
					ss3 << buf;
				}
			}

			unsigned offset = symbolics.size();
			for (size_t i = 0; i < vars.size(); i++) {
				std::string name = vars[i].second->getName();
				//	ss2 << name;
				for (size_t j = 0; j < solvedValues[offset + i].size(); j++) {
					char buf[8];
					sprintf(buf, "%hhu", solvedValues[offset + i][j]);
					ss2 << buf;
				}
				if (i != vars.size() - 1) {
					ss2 << "-";
				}
			}

			string input_seq = ss1.str();
			string cache_seq = ss2.str();

			if (!cacheBehavInputMap.count(cache_seq)) {
				if (cacheBehavInputMap.empty()) {
					std::cerr << "empty cacheBehaInputMap, now push back the first item ..." << std::endl;
				} else {
					std::cerr << "find new cache sequence ..." << std::endl;
				}
				std::cerr << "input sequence : " << input_seq << std::endl;
				std::cerr << "chache sequence: " << cache_seq << std::endl;

				cacheBehavInputMap[cache_seq] = input_seq;

#if 0 // use the solved value to construct the concrete addresses to check the cache-mapping sequence
				uint64_t tmp;
				ss3 >> tmp;
				ref<Expr> k = ConstantExpr::create(tmp, Expr::Int8);
				std::vector<ref<Expr> > addrs;
				for (size_t i = 0; i < state.memAddrs.size(); i++) {
					addrs.push_back(Expr::copyExpr(state.memAddrs[i]));
				}

				for (size_t i = 0; i < addrs.size(); i++) {
					if (addrs[i]->getKind() == Expr::Constant) {
						uint64_t addr = dyn_cast<ConstantExpr>(addrs[i])->getZExtValue();
						std::cerr << "tid: " << state.tids[i] << ", addr: " << addr << ", cset: "
						<< this->getCacheSet(addr, cacheSetNum, cacheLineSize) << std::endl;
						continue;
					}
					ref<Expr> candidate = addrs[i];
					substituteSymVar(candidate, k);
					uint64_t addr = computeExpr(candidate);
					std::cerr << "tid: " << state.tids[i] << ", addr: " << addr << ", cset: " << this->getCacheSet(addr, cacheSetNum, cacheLineSize)
					<< std::endl;
				}
#endif
			} else {
//				std::cerr << "cache seq exists" << std::endl;
			}

			// now add constraint to compute next possible solution
			ref<Expr> extra = NULL;
			for (size_t i = 3; i < symbolics.size(); i++) {
				UpdateList ul(symbolics[i].second, 0);
				std::vector<unsigned char> &solvedValue = solvedValues[i];
				for (unsigned j = 0; j < solvedValue.size(); j++) {
					ref<Expr> tmpRead = ReadExpr::alloc(ul, ConstantExpr::create(j, Expr::Int8));
					ref<Expr> tmpValue = ConstantExpr::create(solvedValue[j], tmpRead->getWidth());
					if (extra.isNull()) extra = EqExpr::create(tmpRead, tmpValue);
					else extra = AndExpr::create(extra, EqExpr::create(tmpRead, tmpValue));
				}
			}
			if (!extra.isNull()) {
				state.addConstraint(NotExpr::create(extra));
			}
		}
	}
	return 0;
}

void Executor::computeIndividualConstraint(ExecutionState &state, TimingSolver *solver, int idx, ref<Expr> addr) {
	ref<Expr> premise = NULL;
	ref<Expr> miss = NULL;
	ref<Expr> not_reload = NULL;

	ExecutionState bakState(state);

	bool must_miss;
	bool must_hit;
//	bool sym_reload;

	for (int i = idx - 1; i >= t2idx; i--) {
		ref<Expr> &prev = bakState.memAddrs[i];
		ref<Expr> setCmp = Executor::createCacheSetCompareConstraint(addr, prev);
		ref<Expr> tagCmp = NULL;

		if (prev->getKind() == Expr::Constant) {
			ref<ConstantExpr> tmp = dyn_cast<ConstantExpr>(prev);
//			std::cerr << "i: " << i << ", set:" << this->getCacheSet(tmp->getZExtValue(), cacheSetNum, cacheLineSize) << std::endl;
		} else {
//			std::cerr << "i: " << i << std::endl;
		}

		must_hit = false;
		must_miss = false;

		if (i < idx - 1) {
			ref<Expr> tmp = Executor::createNotReloadConstraint(i + 1, addr, bakState);
			if (tmp->getKind() == Expr::Constant) {
				// should stop backtracking if previously reloaded
				// can only be not-reload (true)
				assert(tmp->isTrue());
				if (not_reload.isNull()) not_reload = tmp;
			} else {
				not_reload = not_reload.isNull() ? tmp : AndExpr::create(not_reload, tmp);
			}
		}

		if (setCmp->getKind() == Expr::Constant) { // constant cmp constraint
			if (setCmp->isFalse()) {
				if (not_reload.isNull()) {
					assert(premise.isNull());
				} else if (dyn_cast<ConstantExpr>(not_reload)) {
					assert(dyn_cast<ConstantExpr>(not_reload)->isTrue());
					assert(premise.isNull());
				}
				must_miss = true;
				continue; // different cache set
			}
			tagCmp = Executor::createCacheTagCompareConstraint(addr, prev);
			assert(tagCmp->getKind() == Expr::Constant);
			if (tagCmp->isTrue()) {
				// a hard hit
				if (not_reload.isNull()) {
					assert(premise.isNull());
					must_hit = true;
				} else if (dyn_cast<ConstantExpr>(not_reload)) {
					assert((dyn_cast<ConstantExpr>(not_reload))->isTrue());
					assert(premise.isNull());
					must_hit = true;
				} else {
					premise = premise.isNull() ? not_reload : OrExpr::create(premise, not_reload);
				}
			} else { // same cache set, different tag
				if (not_reload.isNull()) {
					assert(premise.isNull());
					must_miss = true;
				} else if (dyn_cast<ConstantExpr>(not_reload)) {
					assert(dyn_cast<ConstantExpr>(not_reload)->isTrue());
					assert(premise.isNull());
					must_miss = true;
				}
			}
			break;
		} else { //symbolic cmp constraint
			bool mustBeFalse = false;
			bool mustBeTrue = false;

			solver->mustBeFalse(data::SIDE_CHANNEL_ANALYSIS, bakState, setCmp, mustBeFalse);
			if (mustBeFalse) {
				if (not_reload.isNull()) assert(premise.isNull());
				must_miss = true;
				continue; // cannot cause either conflict or hit, do nothing
			}

			tagCmp = Executor::createCacheTagCompareConstraint(addr, prev);
			solver->mustBeFalse(data::SIDE_CHANNEL_ANALYSIS, bakState, tagCmp, mustBeFalse);
			if (mustBeFalse) {
				if (not_reload.isNull()) assert(premise.isNull());
				must_miss = true;
				continue; // cannot cause either conflict or hit, do nothing
			}

			ref<Expr> current = not_reload.isNull() ? AndExpr::create(setCmp, tagCmp) : AndExpr::create(not_reload, AndExpr::create(setCmp, tagCmp));

			if (premise.isNull()) premise = current;
			else premise = OrExpr::create(premise, current);
			assert(!premise.isNull());

			if (mustBeTrue) { // Must stop here
				if (not_reload.isNull()) {
					assert(premise.isNull());
					must_hit = true;
				}
				break;
			}
		}
	}

	if (premise.isNull()) {
		if (must_miss) {
			bakState.symbolicHitLog[idx] = ConstantExpr::create(0, Expr::Bool);
			bakState.constraintsMap[idx] = std::make_pair(ConstantExpr::create(0, Expr::Bool), ConstantExpr::create(0, Expr::Bool));
		} else if (must_hit) {
			bakState.symbolicHitLog[idx] = ConstantExpr::create(0, Expr::Bool);
			bakState.constraintsMap[idx] = std::make_pair(ConstantExpr::create(1, Expr::Bool), ConstantExpr::create(1, Expr::Bool));
		} else {
			assert(0 && "should not reach here.");
		}
	} else {
		// now compute possible k1, k2
		ref<Expr> copy = Expr::copyExpr(premise, true);
		Expr::replaceArray(copy, bakState.arrayPairs);

		ref<Expr> cond = XorExpr::create(premise, copy);
		std::vector<const Array *> usefulSymbolics;

		std::vector<std::pair<const Array*, const Array*> > &arrayPairs = bakState.arrayPairs;
		for (size_t i = 0; i < arrayPairs.size(); i++) {
			usefulSymbolics.push_back(arrayPairs[i].first);
			usefulSymbolics.push_back(arrayPairs[i].second);
		}

		bakState.addConstraint(cond);

		bool success = true;
		std::vector<vector<unsigned char> > solvedValues;
		while (success) {
			solver->setTimeout(0);
			success = solver->getInitialValues(data::SIDE_CHANNEL_ANALYSIS, bakState, usefulSymbolics, solvedValues);
			std::cerr << "success: " << success << std::endl;
			if (!success) {
				std::cerr << "[DBG] finish solving ... " << std::endl;
			} else {
				std::stringstream ss1;
				std::stringstream ss2;

				for (size_t i = 0; i < arrayPairs.size(); i++) {
					std::string name1 = arrayPairs[i].first->getName();
					std::string name2 = arrayPairs[i].second->getName();

					std::vector<unsigned char> &solved1 = solvedValues[i * 2];
					for (size_t j = 0; j < solved1.size(); j++) {
						char buf1[8];
						sprintf(buf1, "%02x", solved1[j]);
						ss1 << buf1;
					}

					std::vector<unsigned char> &solved2 = solvedValues[i * 2 + 1];
					for (size_t j = 0; j < solved2.size(); j++) {
						char buf2[8];
						sprintf(buf2, "%02x", solved2[j]);
						ss2 << buf2;
					}
				}

				string input_seq1 = ss1.str();
				string input_seq2 = ss2.str();
				std::cerr << "input1: 0x" << input_seq1 << std::endl;
				std::cerr << "input2: 0x" << input_seq2 << std::endl;

				// now add constraint to compute next possible solution
				ref<Expr> extra1 = NULL;
				ref<Expr> extra2 = NULL;
				for (size_t i = 0; i < arrayPairs.size(); i++) {
					UpdateList ul1(arrayPairs[i].first, 0);
					UpdateList ul2(arrayPairs[i].second, 0);
					std::vector<unsigned char> &solved1 = solvedValues[i * 2];
					std::vector<unsigned char> &solved2 = solvedValues[i * 2 + 1];
					for (size_t j = 0; j < solved1.size(); j++) {
						ref<Expr> read1 = ReadExpr::alloc(ul1, ConstantExpr::create(j, Expr::Int32));
						ref<Expr> value1 = ConstantExpr::create(solved1[j], read1->getWidth());
						bakState.addConstraint(NotExpr::create(EqExpr::create(read1, value1)));
					}
					for (size_t j = 0; j < solved2.size(); j++) {
						ref<Expr> read2 = ReadExpr::alloc(ul2, ConstantExpr::create(j, Expr::Int32));
						ref<Expr> value2 = ConstantExpr::create(solved2[j], read2->getWidth());
						extra2 = extra2.isNull() ? EqExpr::create(read2, value2) : AndExpr::create(extra2, EqExpr::create(read2, value2));
						bakState.addConstraint(NotExpr::create(EqExpr::create(read2, value2)));
					}
				}
			}
		}
		if (!copy.isNull()) {
			free(copy.get());
		}
	}
}

//static bool substituteSymVar(ref<Expr> target, ref<Expr> newValue) {
//	assert(!target.isNull());
//	bool found = false;
//	if (target->getKind() == Expr::Read) {
//		ref<ReadExpr> read = dyn_cast<ReadExpr>(target);
//		if (read->updates.root->getName() == "k") {
//			for (size_t i = 0; i < target->parent->getNumKids(); i++) {
//				if (target->parent->getKid(i) == target) {
//					target->parent->setKid(i, newValue);
//					found = true;
//					break;
//				}
//			}
//		}
//	}
//	if (found) return found;
//	for (size_t i = 0; i < target->getNumKids(); i++) {
//		if (substituteSymVar(target->getKid(i), newValue)) {
//			found = true;
//			break;
//		}
//	}
//	return found;
//}

//static uint64_t computeExpr(ref<Expr> source) {
//	uint64_t result = 0;
//	if (source->getKind() == Expr::Add) {
//		ref<AddExpr> add = dyn_cast<AddExpr>(source);
//
//		if (add->getKid(0)->getKind() == Expr::Constant) {
//			ref<ConstantExpr> left = dyn_cast<ConstantExpr>(add->getKid(0));
//			result += left->getZExtValue();
//		}
//
//		if (add->getKid(1)->getKind() == Expr::ZExt) {
//			ref<ConstantExpr> right = dyn_cast<ConstantExpr>(add->getKid(1)->getKid(0));
//			result += right->getZExtValue();
//		} else if (add->getKid(1)->getKind() == Expr::SExt) {
//			ref<ConstantExpr> left = dyn_cast<ConstantExpr>(add->getKid(1)->getKid(0)->getKid(0));
//			ref<ConstantExpr> right = dyn_cast<ConstantExpr>(add->getKid(1)->getKid(0)->getKid(1)->getKid(0));
//			result += left->getZExtValue();
//			result += right->getZExtValue();
//		}
//	}
//	return result;
//}

}

