/*
 * CacheSideChannel.cpp
 *
 *  Created on: Jun 14, 2017
 *      Author: sjguo
 */

#include "CacheSideChannel.h"
#define CACHE_CONFIG_FILE "cache.cfg"

#include <fstream>
#include <iostream>
#include <stdio.h>
using namespace std;

#include <llvm/Support/raw_ostream.h>

#include "Memory.h"
#include "TimingSolver.h"
using namespace klee;

typedef std::vector<ref<Expr> > addrT;
typedef std::pair<ref<Expr>, ref<Expr> > pairT;
typedef std::map<pairT, ref<Expr> > hashT;
typedef std::map<unsigned, pairT> logT;
typedef std::map<unsigned, addrT> seqT;
typedef std::map<unsigned, std::vector<pairT> > seqPairT;

// observer model: miss_count or miss_sequence
CacheChannel::Observer_t CacheChannel::observer;

// to cache and reuse cache-tag related expressions
hashT CacheChannel::tagPairCache;
cacheT CacheChannel::tagCache;

// to cache and reuse cache-set related expressions
hashT CacheChannel::setPairCache;
cacheT CacheChannel::setCache;

// for test generation
std::vector<const Array *> CacheChannel::missVars;

int CacheChannel::nset;
int CacheChannel::line;
int CacheChannel::nassoc;
char CacheChannel::policy[8];

int CacheChannel::fixedColdMisses = 0;
unsigned long CacheChannel::fixedConflictMisses = 0;
unsigned long CacheChannel::observedCacheMiss;
unsigned long CacheChannel::atomicCnstr;

// data structures for constraint logging
logT CacheChannel::conflictConstraintLog;
logT CacheChannel::conflictConstraintLogLRU;
logT CacheChannel::conflictConstraintLogFIFO;
logT CacheChannel::seqConstraintLog;
logT CacheChannel::missConstraintLog;
logT CacheChannel::coldSeqConstraintLog;

// member variables to log cache hit/miss sequence
std::vector<bool> CacheChannel::cMissLog;
// log of all generated constraints
addrT CacheChannel::allConstraintLog;
// holds all symbolic cache miss variables
addrT CacheChannel::symCacheMissLog;
// holds sequence to symbolic variable mapping
std::map<unsigned, ref<Expr> > CacheChannel::symCacheMissMap;

// data structures related to set-associative caches
std::map<unsigned, ref<Expr> > CacheChannel::setAssocReloadCnstrLog;
seqT CacheChannel::setAssocCnstrLog;
seqPairT CacheChannel::setAssocConflictCnstrLog;

// cache mapping routines for constant address expressions
#define CACHE_SET(x,nset,line) (((x) >> log_base2(line)) & (nset - 1))
#define CACHE_TAG(x,nset,line) ((x) >> (log_base2(line) + log_base2(nset)))

// stage threshold for setting probe points
#define STAGE_THRESHOLD 10000000

// to make the compiler happy
class CacheDriver;

// record the memory address of load/store insts, invoked in executeMemoryOperation function
void CacheChannel::logMemoryAddr(ExecutionState& state, ref<Expr> address) {
	if (state.symbolicAddrNum + state.concreteAddrNum <= STAGE_THRESHOLD) {
		state.memAddrs.push_back(address);
		if (address->getKind() == Expr::Constant) {
			state.concreteAddrNum++;
		} else {
			state.symbolicAddrNum++;
		}
	}
}

/* Analyze memory addresses to build the constraint system */
void CacheChannel::processMemoryAddr(ExecutionState& state, TimingSolver* solver) {

#if 0 /* for testing */

	for (unsigned II = 0; II < state.memAddrs.size(); II++) {
		ref<Expr> address = state.memAddrs[II];

		if (address->getKind() == Expr::Constant) {
			ConstantExpr* CE = dyn_cast<ConstantExpr>(address);
			uint64_t addrRaw = CE->getZExtValue();
			fprintf(stdout, ".....   Address (concrete) id = %u, cache set = %llu   .....\n", II + 1, CACHE_SET(addrRaw, nset, line));
		} else {
			ref<Expr> blAddr = LShrExpr::create(address, ConstantExpr::alloc(log_base2(line), address->getWidth()));
			ref<Expr> setAddr = AndExpr::create(blAddr, ConstantExpr::alloc(nset - 1, address->getWidth()));
			for (unsigned set = 0; set < (unsigned) nset; set++) {
				ref<Expr> eqSet = EqExpr::create(setAddr, ConstantExpr::alloc(set, setAddr->getWidth()));
				bool mayBeTrue = true;
				solver->mayBeTrue(state, eqSet, mayBeTrue);
				if (set == 0)
				fprintf(stdout, ".....   Address (symbolic) id = %u, cache set = ", II + 1);
				if (mayBeTrue)
				fprintf(stdout, "%u   ", set);
				else
				fprintf(stdout, "(null)   ");
			}
			fprintf(stdout, ".....\n");
		}
	}

	return;

#endif

	fprintf(stdout, "\n\n");
	fprintf(stdout, "########## Now building the constraint system ##########\n\n");

	/* clean up tasks before processing each execution state */
	atomicCnstr = 0;

	fixedColdMisses = 0;
	fixedConflictMisses = 0;

	allConstraintLog.clear();
	conflictConstraintLog.clear();
	seqConstraintLog.clear();
	missConstraintLog.clear();
	coldSeqConstraintLog.clear();

	conflictConstraintLogLRU.clear();
	conflictConstraintLogFIFO.clear();

	symCacheMissLog.clear();
	symCacheMissMap.clear();

	setAssocReloadCnstrLog.clear();
	setAssocCnstrLog.clear();
	setAssocConflictCnstrLog.clear();

	missVars.clear();

	CacheChannelLRU::seqConstraintLogLRU.clear();
	CacheChannelFIFO::seqConstraintLogFIFO.clear();

//	fprintf(stdout, "\n\n########## after path processing, constraint size %lu ##########\n\n", state.forks);
	fflush(stdout);

	/**
	 * process cold miss constraints
	 * a crude approximation is to remove all cold-miss related constraints
	 * this will derive a bound on the number of eviction misses and a loose interval
	 * on total number of misses ---> [e+1, e+|set|]
	 */

#define __CACHE_CHECK_TIMING_LEAK // Daniel.
#ifdef __CACHE_CHECK_TIMING_LEAK
	if (nassoc == 1) {
		/* for direct-mapped caches */
		processColdMissCnstr(state, solver);
		fprintf(stdout, "\n\n########## after cold miss processing, constraint-log size %lu ##########\n\n", allConstraintLog.size());
		fflush(stdout);
	}
#endif
	/* process conflict miss constraints */
	processConflictMissCnstr(state, solver);
	fprintf(stdout, "\n\n########## after conflict miss processing, constraint-log size %lu ##########\n\n", seqConstraintLog.size());
	fflush(stdout);

	if (nassoc == 1) {
		/* for direct-mapped caches */

#define __CACHE_CHECK_SEQ_LEAK // Daniel.
#ifdef __CACHE_CHECK_SEQ_LEAK
		/* generate cold-miss constraints for detecting leaks in sequence-based attacks */
		processColdMissCnstrForSeqAttack(state, solver);
		fprintf(stdout, "\n\n########## after cold miss processing, constraint-log size %lu ##########\n\n", coldSeqConstraintLog.size());
		fprintf(stdout, "########## Successfully built the constraint system ##########");
		fprintf(stdout, "\n\n");
		fflush(stdout);
#endif
	} else {
		/* for set associative caches */
		if (strncasecmp(policy, "l", 1) == 0) CacheChannelLRU::processLRUConflictMissCnstr(state, solver);
		else if (strncasecmp(policy, "f", 1) == 0) ;/* do nothing, FIFO is processed _online_ -- while building constraints */
		else
		assert(0 && "fatal: replacement policy is not supported");
	}

	fprintf(stdout, "Number of symbolic cache conflicts = %lu\n", symCacheMissLog.size() + coldSeqConstraintLog.size());
	fprintf(stdout, "Number of atomic constraints = %lu\n", atomicCnstr);
	fflush(stdout);


#define __LEAK_DETECTION // Daniel.
#ifdef __LEAK_DETECTION
	/* entry level routine to detect information leak in a given execution */
	CacheDriver::checkForAttackAndLeak(state, solver);
#endif

#define __TEST_DRIVER // Daniel.

#ifdef __TEST_DRIVER
	/* entry level routine to generate tests for each possible side-channel observation */
	CacheTestDriver::generateAllTestBasedOnObserverModel(state, solver);
#endif
}

// simple sanity checker
bool CacheChannel::sanityCheck(ExecutionState& state, unsigned long fixedMisses, unsigned long obsMisses) {

	/* fast path: when the number of constant misses is bigger than observed misses */
	if (fixedMisses > obsMisses) {
		fprintf(stdout, "\n\n..... Number of constant misses %lu exceeds number of observed miss %lu .....\n", fixedMisses, obsMisses);
		fprintf(stdout, "\n\n..... %lu cache misses have no possible solutions .....\n\n", obsMisses);

		return true;
	}

	/* fast path: when the number of observed misses is bigger than the number of accesses */
	if (obsMisses > state.memAddrs.size()) {
		fprintf(stdout, "\n\n..... Number of observed misses %lu exceeds number of cache access %lu .....\n", obsMisses, state.memAddrs.size());
		fprintf(stdout, "\n\n..... %lu cache misses have no possible solutions .....\n\n", obsMisses);

		return true;
	}

	/* sanity check */
	assert((obsMisses <= state.memAddrs.size()) && (obsMisses >= fixedMisses) && "fast-path checking went wrong");

	/* to make the compiler happy */
	return false;
}

// solve constraints to get all tests according to side-channel observations
int CacheChannel::getAllTests(ExecutionState& state, TimingSolver* solver, long bound) {

	ExecutionState cState(state);
	int nsolution = 0;

	// sanity check (cache performance does not depend on the input)
	if (missVars.size() == 0) return 1;

	// get the models for cache miss constraints
	{
		// 1. add all symbolic inputs
		std::vector<std::vector<unsigned char> > values;
		std::vector<const Array*> objects;
		for (unsigned i = 0; i != state.symbolics.size(); i++) {
			llvm::errs() << "\n\n -> Found symbolic memory object, name: \"" << state.symbolics[i].first->name << "\"\n";
			llvm::errs() << "address = " << state.symbolics[i].first->address << "\n";
			objects.push_back(state.symbolics[i].second);
		}
		// 2. now add all symbolic variables
		for (unsigned i = 0; i != missVars.size(); i++) {
			llvm::errs() << "\n\n -> Found symbolic hit/miss variable..." << missVars[i]->name << "\n";
			objects.push_back(missVars[i]);
		}

		bool success = true;

		/******* KEY Test Genration Loop to get tests, each of which leads to a unique observation *******/
		while (success) {
			/* get a model (solution) for the observed cache misses */
			success = solver->getInitialValues(data::SIDE_CHANNEL_ANALYSIS, cState, objects, values);
			solver->setTimeout(0);
			/* no solution exists */
			if (!success) {
				llvm::errs() << "\n\n..... test generation finished (no more observations possible) .....\n\n";
			} else /* we have a solution for observed cache misses */{
				//ExprPPrinter::printQuery(llvm::errs(), cState.constraints,
				//                    	ConstantExpr::alloc(0, Expr::Bool));
				llvm::errs() << "\n\n..... printing one test for the chosen observer model .....\n\n";

				/* print test input for the respective solution */
				for (unsigned SI = 0; SI < state.symbolics.size(); SI++) {
					llvm::errs() << state.symbolics[SI].first->name << " = ";
					llvm::errs() << "(size " << state.symbolics[SI].second->size << ", little-endian) ";
					for (unsigned II = 0; II < values[SI].size(); II++)
						fprintf(stdout, "%d ", values[SI][II]);
					fprintf(stdout, "\n");
				}
				/* print observation for the respective solution */
				unsigned nSymbolic = state.symbolics.size();
				for (unsigned i = 0; i < missVars.size(); i++) {
					/* print the name of hit/miss variable */
					llvm::errs() << missVars[i]->getName() << " = ";
					llvm::errs() << "(size " << missVars[i]->getSize() << ", little-endian) ";
					for (unsigned j = 0; j < missVars[i]->getSize(); j++)
						fprintf(stdout, "%d ", values[nSymbolic + i][j]);
					fprintf(stdout, "\n");
				}
				nsolution++;
				llvm::errs() << "..... #tests generated so far = " << nsolution << ".....\n\n";

				/* sudiptac: only get as many as "bound" tests */
				if (nsolution >= bound) break;
				/* constrain the state space to get a different solution next time */
				if (CacheChannel::observer == MISS_COUNT) getNextTestForObsCount(state, cState, values);
				else if (CacheChannel::observer == MISS_SEQUENCE) getNextTestForObsSeq(state, cState, values);
				else
				assert(0 && "fatal: observer model is not supported");
			}
		}
		/********** END of test generation loop **********/
	}

	return nsolution;
}

// constrain state space to get a new test each time the test generator is called
void CacheChannel::getNextTestForObsCount(ExecutionState& state, ExecutionState& cState, std::vector<std::vector<unsigned char> >& values) {
	ref<Expr> counterEx = NULL;
	ref<Expr> sumEx = NULL;
	uint64_t valueEx = 0;
	unsigned nSymbolic = state.symbolics.size();

	for (unsigned SI = 0; SI < missVars.size(); SI++) {
		UpdateList ul(missVars[SI], 0);
		for (unsigned II = 0; II < values[nSymbolic + SI].size(); II++) {
			ref<Expr> read = ReadExpr::create(ul, ConstantExpr::create(II, Expr::Int32));
			/* sudiptac: this is a crazy hack */
			valueEx += values[nSymbolic + SI][II];
			/* get add expression for all symbolic variables */
			(sumEx.isNull()) ? (sumEx = read) : (sumEx = AddExpr::create(sumEx, read));
		}
	}

	if (sumEx.isNull() == false) {
		ref<Expr> assignedValue = ConstantExpr::alloc(valueEx, sumEx->getWidth());
		counterEx = EqExpr::create(sumEx, assignedValue);

		/* add negation to get a different solution in the next CEGAR iteration */
		cState.addConstraint(NotExpr::create(counterEx));
	}
}

// constrain state space to get a new test each time the test generator is called
void CacheChannel::getNextTestForObsSeq(ExecutionState& state, ExecutionState& cState, std::vector<std::vector<unsigned char> >& values) {
	ref<Expr> counterEx = NULL;
	ref<Expr> allEx = NULL;
	uint64_t valueEx = 0;
	unsigned nSymbolic = state.symbolics.size();

	for (unsigned SI = 0; SI < missVars.size(); SI++) {
		UpdateList ul(missVars[SI], 0);
		for (unsigned II = 0; II < values[nSymbolic + SI].size(); II++) {
			ref<Expr> read = ReadExpr::create(ul, ConstantExpr::create(II, Expr::Int32));
			/* sudiptac: this is a crazy hack */
			valueEx = values[nSymbolic + SI][II];
			ref<Expr> assignedValue = ConstantExpr::alloc(valueEx, read->getWidth());
			counterEx = EqExpr::create(read, assignedValue);
			/* get aggregate expression for all symbolic variables */
			(allEx.isNull()) ? (allEx = counterEx) : (allEx = AndExpr::create(allEx, counterEx));
		}
	}

	/* add negation to get a different solution in the next CEGAR iteration */
	cState.addConstraint(NotExpr::create(allEx));
}

/**
 * Solve all constraints to get models of secret keys. Each solution is a model for the secret key.
 * The observational constraint is added first (e.g. count-based or seq-based) and the solver is
 * called iteratively
 */
int CacheChannel::solveAllConstraints(ExecutionState& state, TimingSolver* solver, ref<Expr>& obsCnstr, long bound) {

	ExecutionState cState(state);
	// add the observational constraint
	cState.addConstraint(obsCnstr);

	int nsolution = 0;

	// get the models for cache miss constraints
	{
		// get values of symbolic objects
		std::vector<std::vector<unsigned char> > values;
		std::vector<const Array*> objects;
		for (unsigned SI = 0; SI != state.symbolics.size(); SI++) {
			llvm::errs() << "\n\n -> encountered symbolic object \"" << state.symbolics[SI].first->name << "\"\n";
			llvm::errs() << "address = " << state.symbolics[SI].first->address << "\n";
			objects.push_back(state.symbolics[SI].second);
		}

		bool success = true;
		/********** the KEY CEGAR loop to get different solutions for a given cache miss **********/
		while (success) {
			/* get a model (solution) for the observed cache misses */
			success = solver->getInitialValues(data::SIDE_CHANNEL_ANALYSIS, cState, objects, values);
			solver->setTimeout(0);
			/* no solution exists */
			if (!success) {
				fprintf(stdout, "\n\n..... observation constraint have no possible solutions .....\n\n");
			} else /* we have a solution for observed cache misses */{
				//ExprPPrinter::printQuery(llvm::errs(), cState.constraints,
				//                    	ConstantExpr::alloc(0, Expr::Bool));
				fprintf(stdout, "\n\n..... printing one solution for observation constraint .....\n\n");

				for (unsigned SI = 0; SI < state.symbolics.size(); SI++) {
					llvm::errs() << state.symbolics[SI].first->name << " = ";
					llvm::errs() << "(size " << state.symbolics[SI].second->size << ", little-endian) ";
					for (unsigned II = 0; II < values[SI].size(); II++)
						fprintf(stdout, "%d ", values[SI][II]);
					fprintf(stdout, "\n");
					fflush(stdout);
				}
				nsolution++;
				fprintf(stdout, "..... #keys discovered so far = %d .....\n\n", nsolution);
				/* sudiptac: only get as many as "bound" solutions */
				if (nsolution >= bound) break;
				/* implement features to get a different solution each time
				 * "getInitialValues" is called	*/
				naiveCEGAR(state, cState, values);
			}
		}
		/********** END of KEY CEGAR loop **********/
	}

	return nsolution;
}

// a naive CEGAR based approach to get all possible inputs for a given (observed) cache miss
void CacheChannel::naiveCEGAR(ExecutionState& state, ExecutionState& cState, std::vector<std::vector<unsigned char> >& values) {
	ref<Expr> counterEx = NULL;

	for (unsigned SI = 0; SI < state.symbolics.size(); SI++) {
		UpdateList ul(state.symbolics[SI].second, 0);
		for (unsigned II = 0; II < values[SI].size(); II++) {
			ref<Expr> read = ReadExpr::create(ul, ConstantExpr::create(II, Expr::Int32));
			ref<Expr> assignedValue = ConstantExpr::alloc(values[SI][II], read->getWidth());
			ref<Expr> eqExpr = EqExpr::create(read, assignedValue);
			/* add more constraints to the cache state encoding */
			(counterEx.isNull()) ? (counterEx = eqExpr) : (counterEx = AndExpr::create(counterEx, eqExpr));
		}
	}

	// add negation to get a different solution in the next CEGAR iteration
	cState.addConstraint(NotExpr::create(counterEx));
}

// read cache hit/miss sequence from a file (currently, this function is not used)
void CacheChannel::readConcreteHitMissSequence() {
	/* FIXME: file name for cache hit/miss sequence is hardcoded */
	std::ifstream seqFile("hit_miss_seq.log");
	bool miss;

	fprintf(stdout, "\n\n####### reading cache hit/miss sequence file ######\n\n");
	while (seqFile >> miss)
		cMissLog.push_back(miss);
	fprintf(stdout, "\n\n####### end reading cache hit/miss sequence file ######\n\n");
}

// process miss sequence constraints
void CacheChannel::processMissSequenceCnstr() {
	ref<Expr> seqExpr = NULL;

	/* read concrete hit/miss sequence from file */
	readConcreteHitMissSequence();

	/* go through the concrete cache miss sequence */
	for (unsigned II = 0; II < cMissLog.size(); II++) {
		if (symCacheMissMap.count(II)) {
			ref<Expr> eqExpr = EqExpr::create(symCacheMissMap[II], ConstantExpr::alloc(cMissLog[II], symCacheMissMap[II]->getWidth()));
			pairT missCnstr(eqExpr, NULL);
			missConstraintLog[II] = missCnstr;
		}
	}
}

// process miss count constraint
ref<Expr> CacheChannel::processMissCountCnstr(int nmiss) {
	ref<Expr> addMissExpr = NULL;

	/* sum up all the symbolic cache miss variables */
	for (addrT::iterator II = symCacheMissLog.begin(); II != symCacheMissLog.end(); II++) {
		if (addMissExpr.isNull()) {
			addMissExpr = *II;
		} else addMissExpr = AddExpr::create(addMissExpr, *II);
	}

	if (!addMissExpr.isNull()) {
		int symbolicMisses = nmiss;

		fprintf(stdout, "Number of symbolic cache conflicts = %lu\n", symCacheMissLog.size());
		fprintf(stdout, "Number of atomic constraints = %lu\n", 2 * symCacheMissLog.size() + atomicCnstr);

#if 1
		fprintf(stdout, "Number of symbolic cache misses = %d\n", symbolicMisses);
#endif

		/* now add the constraint to encode the desired number of cache misses i.e. (miss == K)	*/
		ref<Expr> missCountCnstr = EqExpr::create(addMissExpr, ConstantExpr::alloc(symbolicMisses, addMissExpr->getWidth()));

		return missCountCnstr;
	}

	return NULL;
}

// generate tag inequality constraints for constant address pairs
bool CacheChannel::generateTagCnstrCC(ref<Expr>& addressI, ref<Expr>& addressJ) {

	ConstantExpr* CE = dyn_cast < ConstantExpr > (addressI);
	uint64_t addrRaw = CE->getZExtValue();
	int tagI = CACHE_TAG(addrRaw, nset, line);
	CE = dyn_cast < ConstantExpr > (addressJ);
	addrRaw = CE->getZExtValue();
	int tagJ = CACHE_TAG(addrRaw, nset, line);

	return (tagI != tagJ);
}

// generate set equality constraints for constant address pairs
bool CacheChannel::generateSetCnstrCC(ref<Expr>& addressI, ref<Expr>& addressJ) {
	ConstantExpr* CE = dyn_cast < ConstantExpr > (addressI);
	uint64_t addrRaw = CE->getZExtValue();
	int setI = CACHE_SET(addrRaw, nset, line);
	CE = dyn_cast < ConstantExpr > (addressJ);
	addrRaw = CE->getZExtValue();
	int setJ = CACHE_SET(addrRaw, nset, line);

	return (setI == setJ);
}

// generate set equality constraints for <symbolic,constant> address pairs
ref<Expr> CacheChannel::generateSetCnstrSC(ref<Expr>& addressI, ref<Expr>& addressJ) {
	int set;
	ref<Expr> setAddr;
	pairT setPair(addressI, addressJ);

	atomicCnstr++;

	/* optimization to reduce memory consumption */
	{
		if (setPairCache.count(setPair)) {
#ifdef _CACHE_CHANNEL_DEBUG
			fprintf(stdout, "cached result found.....\n");
			fflush(stdout);
#endif
			return setPairCache[setPair];
		}
	}
	/* end optimization branch */

	/* otherwise allocate new memory */
	if (addressI->getKind() == Expr::Constant) {
		ConstantExpr* CE = dyn_cast < ConstantExpr > (addressI);
		uint64_t addrRaw = CE->getZExtValue();
		set = CACHE_SET(addrRaw, nset, line);
		/* for optimizing memory consumption */
		if (setCache.count(addressJ)) setAddr = setCache[addressJ];
		else { /* allocate new memory and store in the cache */
			ref<Expr> blAddr = LShrExpr::create(addressJ, ConstantExpr::alloc(log_base2(line), addressJ->getWidth()));
			setAddr = AndExpr::create(blAddr, ConstantExpr::alloc(nset - 1, addressJ->getWidth()));
			setCache[addressJ] = setAddr;
		}
	} else {
		ConstantExpr* CE = dyn_cast < ConstantExpr > (addressJ);
		uint64_t addrRaw = CE->getZExtValue();
		set = CACHE_SET(addrRaw, nset, line);
		/* for optimizing memory consumption */
		if (setCache.count(addressI)) setAddr = setCache[addressI];
		else { /* allocate new memory and store in the cache */
			ref<Expr> blAddr = LShrExpr::create(addressI, ConstantExpr::alloc(log_base2(line), addressI->getWidth()));
			setAddr = AndExpr::create(blAddr, ConstantExpr::alloc(nset - 1, addressI->getWidth()));
			setCache[addressI] = setAddr;
		}
	}

	setPairCache[setPair] = EqExpr::create(setAddr, ConstantExpr::alloc(set, setAddr->getWidth()));

	return setPairCache[setPair];
}

// generate tag inequality constraints for <symbolic,constant> address pairs
ref<Expr> CacheChannel::generateTagCnstrSC(ref<Expr>& addressI, ref<Expr>& addressJ) {
	int tag;
	ref<Expr> tagAddr;
	pairT tagPair(addressI, addressJ);

	atomicCnstr++;

	/* optimization to reduce memory consumption */
	{
		if (tagPairCache.count(tagPair)) {
#ifdef _CACHE_CHANNEL_DEBUG
			fprintf(stdout, "cached result found.....\n");
			fflush(stdout);
#endif
			return tagPairCache[tagPair];
		}
	}
	/* end optimization branch */

	/* otherwise, allocate new memory */
	if (addressI->getKind() == Expr::Constant) {
		ConstantExpr* CE = dyn_cast < ConstantExpr > (addressI);
		uint64_t addrRaw = CE->getZExtValue();
		tag = CACHE_TAG(addrRaw, nset, line);
		/* for optimizing memory consumption */
		if (tagCache.count(addressJ)) tagAddr = tagCache[addressJ];
		else { /* allocate new memory and store in the cache */
			tagAddr = LShrExpr::create(addressJ, ConstantExpr::alloc(log_base2(line) + log_base2(nset), addressJ->getWidth()));
			tagCache[addressJ] = tagAddr;
		}
	} else {
		ConstantExpr* CE = dyn_cast < ConstantExpr > (addressJ);
		uint64_t addrRaw = CE->getZExtValue();
		tag = CACHE_TAG(addrRaw, nset, line);
		/* for optimizing memory consumption */
		if (tagCache.count(addressI)) tagAddr = tagCache[addressI];
		else { /* allocate new memory and store in the cache */
			tagAddr = LShrExpr::create(addressI, ConstantExpr::alloc(log_base2(line) + log_base2(nset), addressI->getWidth()));
			tagCache[addressI] = tagAddr;
		}
	}

	ref<Expr> eqExpr = EqExpr::create(tagAddr, ConstantExpr::alloc(tag, tagAddr->getWidth()));

	tagPairCache[tagPair] = NotExpr::create(eqExpr);

	return tagPairCache[tagPair];
}

// generate set equality constraints for <symbolic,symbolic> constraint pairs
ref<Expr> CacheChannel::generateSetCnstrSS(ref<Expr>& addressI, ref<Expr>& addressJ) {
	pairT setPair(addressI, addressJ);
	ref<Expr> setAddrI, setAddrJ;

	atomicCnstr++;

	/* optimization to reduce memory consumption */
	{
		if (setPairCache.count(setPair)) {
#ifdef _CACHE_CHANNEL_DEBUG
			fprintf(stdout, "cached result found.....\n");
			fflush(stdout);
#endif
			return tagPairCache[setPair];
		}
	}
	/* end optimization branch */

	/* more optimization */
	if (setCache.count(addressI)) setAddrI = setCache[addressI];
	else {
		ref<Expr> blAddr = LShrExpr::create(addressI, ConstantExpr::alloc(log_base2(line), addressI->getWidth()));
		setAddrI = AndExpr::create(blAddr, ConstantExpr::alloc(nset - 1, addressI->getWidth()));
		setCache[addressI] = setAddrI;
	}
	if (setCache.count(addressJ)) setAddrJ = setCache[addressJ];
	else {
		ref<Expr> blAddr = LShrExpr::create(addressJ, ConstantExpr::alloc(log_base2(line), addressJ->getWidth()));
		setAddrJ = AndExpr::create(blAddr, ConstantExpr::alloc(nset - 1, addressJ->getWidth()));
		setCache[addressJ] = setAddrJ;
	}
	/* more optimization ends */

	setPairCache[setPair] = EqExpr::create(setAddrI, setAddrJ);

	return setPairCache[setPair];
}

// generate tag inequality constraints for <symbolic,symbolic> constraint pairs
ref<Expr> CacheChannel::generateTagCnstrSS(ref<Expr>& addressI, ref<Expr>& addressJ) {
	pairT tagPair(addressI, addressJ);
	ref<Expr> tagAddrI, tagAddrJ;

	atomicCnstr++;

	/* optimization to reduce memory consumption */
	{
		if (tagPairCache.count(tagPair)) {
#ifdef _CACHE_CHANNEL_DEBUG
			fprintf(stdout, "cached result found.....\n");
			fflush(stdout);
#endif
			return tagPairCache[tagPair];
		}
	}
	/* end optimization branch */

	/* more optimization */
	if (tagCache.count(addressI)) tagAddrI = tagCache[addressI];
	else {
		tagAddrI = LShrExpr::create(addressI, ConstantExpr::alloc(log_base2(line) + log_base2(nset), addressI->getWidth()));
		tagCache[addressI] = tagAddrI;
	}
	if (tagCache.count(addressJ)) tagAddrJ = tagCache[addressJ];
	else {
		ref<Expr> tagAddrJ = LShrExpr::create(addressJ, ConstantExpr::alloc(log_base2(line) + log_base2(nset), addressJ->getWidth()));
		tagCache[addressJ] = tagAddrJ;
	}
	/* more optimization ends */

	ref<Expr> eqExpr = EqExpr::create(tagAddrI, tagAddrJ);

	tagPairCache[tagPair] = NotExpr::create(eqExpr);

	return tagPairCache[tagPair];
}

// generate constraints in order to capture that an access "II" occurs for the first time
ref<Expr> CacheChannel::generateNeverBeforeCnstr(ExecutionState& state, TimingSolver* solver, unsigned II) {
	ref<Expr> ret = NULL;
	ref<Expr> addressI = state.memAddrs[II];

	for (unsigned JJ = 0; JJ < II; JJ++) {
		ref<Expr> addressJ = state.memAddrs[JJ];
		ref<Expr> setCnstr = NULL;
		ref<Expr> tagCnstr = NULL;

		if (addressI->getKind() == Expr::Constant) {
			if (addressJ->getKind() == Expr::Constant) {
				int setR = generateSetCnstrCC(addressI, addressJ);
				/* make sure that the pair <addressI,addressJ> may conflict in the cache */
				if (!setR) continue;
				break; /* this cannot be a never-before situation */
			} else {
				setCnstr = generateSetCnstrSC(addressI, addressJ);
			}
		} else {
			if (addressJ->getKind() == Expr::Constant) setCnstr = generateSetCnstrSC(addressI, addressJ);
			else setCnstr = generateSetCnstrSS(addressI, addressJ);
		}
		assert(!setCnstr.isNull() && "cold miss constraint processing went wrong");
		ref<Expr> notConflictJJandII = NotExpr::create(setCnstr);
		bool mustBeTrue = true;
		solver->mustBeTrue(data::SIDE_CHANNEL_ANALYSIS, state, notConflictJJandII, mustBeTrue);
		if (mustBeTrue) continue;

		if (ret.isNull()) ret = notConflictJJandII;
		else ret = AndExpr::create(ret, notConflictJJandII);
	}

	return ret;
}

// generate constraints in order to detect vulnerabilities against sequence-based attacks
void CacheChannel::processColdMissCnstrForSeqAttack(ExecutionState& state, TimingSolver* solver) {

	for (unsigned II = 1; II < state.memAddrs.size(); II++) {
		ref<Expr> addressI = state.memAddrs[II];
		ref<Expr> neverBeforeI = generateNeverBeforeCnstr(state, solver, II);

		if (neverBeforeI.isNull()) continue;

		ref<Expr> notNeverBeforeI = NotExpr::create(neverBeforeI);
		pairT coldSeqCnstr(notNeverBeforeI, neverBeforeI);

		coldSeqConstraintLog[II] = coldSeqCnstr;
	}
}

/**
 * generate constraints to formulate reloading of memory blocks
 * reload constraints need to be modified for the FIFO replacement policy
 */
ref<Expr> CacheChannel::generateCnstrForInterReload(ExecutionState& state, int hStart, int hEnd, int target, bool& reload) {

	if (hStart > hEnd) {
		return NULL;
	}

	ref<Expr> addrTarget = state.memAddrs[target];
	bool set, tag;

	ref<Expr> addrStart = state.memAddrs[hStart];
	ref<Expr> notSetCnstr;
	ref<Expr> tagCnstr;
	ref<Expr> fifoCnstr = NULL;

	if (addrTarget->getKind() == Expr::Constant) {
		if (addrStart->getKind() == Expr::Constant) {
			set = generateSetCnstrCC(addrTarget, addrStart);
			tag = generateTagCnstrCC(addrTarget, addrStart);
			/* memory block might be reloaded */
			if (set && !tag) {
				/* direct-mapped policy */
				if (nassoc == 1) reload = true;
				/* LRU policy */
				else if (strncasecmp(policy, "l", 1) == 0) reload = true;
				/* FIFO policy */
				else if (strncasecmp(policy, "f", 1) == 0) {
					/* the intermediate access is a cache miss */
					if (missConstraintLog.count(hStart) && missConstraintLog[hStart].first.isNull()) reload = true;
					else if (missConstraintLog.count(hStart)) {
						fifoCnstr = missConstraintLog[hStart].first;
					} else {
						/* do nothing */
					}
				} else
				assert(0 && "fatal: cache replacement policy is not supported");
			}

			return fifoCnstr;

		} else {
			ref<Expr> setCnstr = generateSetCnstrSC(addrTarget, addrStart);
			notSetCnstr = NotExpr::create(setCnstr);
			tagCnstr = generateTagCnstrSC(addrTarget, addrStart);
		}
	} else {
		if (addrStart->getKind() == Expr::Constant) {
			ref<Expr> setCnstr = generateSetCnstrSC(addrTarget, addrStart);
			notSetCnstr = NotExpr::create(setCnstr);
			tagCnstr = generateTagCnstrSC(addrTarget, addrStart);
		} else {
			ref<Expr> setCnstr = generateSetCnstrSS(addrTarget, addrStart);
			notSetCnstr = NotExpr::create(setCnstr);
			tagCnstr = generateTagCnstrSS(addrTarget, addrStart);
		}
	}

	ref<Expr> retExpr = OrExpr::create(notSetCnstr, tagCnstr);

	/* sudiptac: for FIFO replacement policy */
	if (nassoc > 1 && strncasecmp(policy, "f", 1) == 0) {
		if (!missConstraintLog.count(hStart)) {
			return NULL;
		} else if (!missConstraintLog[hStart].first.isNull()) {
			retExpr = OrExpr::create(retExpr, missConstraintLog[hStart].first);
		}
	}
	/* end processing FIFO replacement policy */

	return retExpr;
}

// process constraints for conflict cache misses
void CacheChannel::processConflictMissCnstr(ExecutionState& state, TimingSolver* solver) {

	/* starts from an empty cache, hence initialize the first access to be always a miss */
	pairT cnstr(NULL, NULL);
	missConstraintLog[0] = cnstr;

	for (unsigned i = 1; i < state.memAddrs.size(); i++) {
		ref<Expr> addressI = state.memAddrs[i];
		bool conflict = false;
		ref<Expr> premise = NULL;
		ref<Expr> itmCnstr = NULL;
		bool mustBeFalse = true;

		/* sudiptac: switching loop for performance */
		for (int j = (int) (i - 1); j >= 0; j--) {
			ref<Expr> addressJ = state.memAddrs[j];
			bool setR, tagR;
			ref<Expr> setCnstr, tagCnstr;
			bool reload = false;
			bool symbolic = false;
			/* required only for set-associtive caches */
			ref<Expr> uniqueConflict = NULL;

#if 1
			/* disabling the following code fragment would result in a sound approximation */
			/* sudiptac: enable this code fragment for exact solution */
			/* generate constraints to make sure nothing between j and i (exclusive)
			 * access the memory block accessed at i */
			if (j + 1 <= (int) (i - 1)) {
				ref<Expr> notReloadCnstr = generateCnstrForInterReload(state, j + 1, i - 1, i, reload);
				/* if the memory block is reloaded between j and i (exclusive) we skip
				 * other constraints, as j cannot induce cache conflict to i */
				if (reload) {
					break;
				}

				if (!notReloadCnstr.isNull()) {
					if (!itmCnstr.isNull()) itmCnstr = AndExpr::create(itmCnstr, notReloadCnstr);
					else itmCnstr = notReloadCnstr;
				}
				/* start special processing for set-associative caches */
				if (nassoc > 1) {
					/* find unique cache conflicts */
					for (int KK = j + 1; KK <= (int) (i - 1); KK++) {
						ref<Expr> newConflict = generateCnstrForInterReload(state, KK, i - 1, j, reload);
						if (!newConflict.isNull()) {
							uniqueConflict = (uniqueConflict.isNull()) ? newConflict : AndExpr::create(uniqueConflict, newConflict);
						}
					}
				}
				/* end processing for set-associative caches */
			}
#endif
			/* sudiptac: log constraints for set-associative caches */
			/* check whether the access "II" could be a cold miss in a set-associative cache */
			if (j == 0 && nassoc > 1) {
				ref<Expr> newCnstr = generateCnstrForInterReload(state, j, i - 1, i, reload);
				if (!reload) {
					if (itmCnstr.isNull()) logSetAssociativeCnstr(newCnstr, i, true);
					else {
						ref<Expr> reloadCnstr = (newCnstr.isNull()) ? itmCnstr : AndExpr::create(itmCnstr, newCnstr);
						logSetAssociativeCnstr(reloadCnstr, i, true);
					}
				} else {
					/* do nothing */
				}
			}
			/* sudiptac: end logging set-associative constraints */

			/* sudiptac: handling FIFO replacement policy */
			if (nassoc > 1 && strncasecmp(policy, "f", 1) == 0) {
				if (missConstraintLog.count(j) && missConstraintLog[j].second.isNull() == 0) {
					itmCnstr = (itmCnstr.isNull()) ? missConstraintLog[j].second : AndExpr::create(itmCnstr, missConstraintLog[j].second);
				} else if (!missConstraintLog.count(j)) /* cache hits do not generate conflict in FIFO */
				continue;
			}
			/* end handling FIFO replacement policy */

			/* generate \psi_cnf  and \psi_dif constraints in the following code */
			/* suffix "CC" stands for <constant,constant> address pairs */
			/* suffix "SC" stands for <symbolic,constant> or <constant,symbolic> address pairs */
			/* suffix "SS" stands for <symbolic,symbolic> address pairs */
			if (addressI->getKind() == Expr::Constant) {
				if (addressJ->getKind() == Expr::Constant) {
					setR = generateSetCnstrCC(addressI, addressJ);
					/* make sure that the pair <addressI,addressJ> may conflict in the cache */
					if (!setR) continue;
					tagR = generateTagCnstrCC(addressI, addressJ);
					if (!tagR) continue;
					/* optimization to reduce number of constraints, this is a must eviction situation */
					if (itmCnstr.isNull()) { // maybe in all constant cases.
						conflict = true;
						/* sudiptac: this is a must-miss scenario */
						fixedConflictMisses++;
						/* sudiptac: for FIFO replacement policy */
						pairT nullCnstr(NULL, NULL);
						missConstraintLog[i] = nullCnstr;
						break;
					}
				} else {
					symbolic = true;
					setCnstr = generateSetCnstrSC(addressI, addressJ);
					tagCnstr = generateTagCnstrSC(addressI, addressJ);
				}
			} else /* address is symbolic */{
				symbolic = true;
				if (addressJ->getKind() == Expr::Constant) {
					setCnstr = generateSetCnstrSC(addressI, addressJ);
					tagCnstr = generateTagCnstrSC(addressI, addressJ);
				} else {
					setCnstr = generateSetCnstrSS(addressI, addressJ);
					tagCnstr = generateTagCnstrSS(addressI, addressJ);
				}
			}
			/* end generation of \psi_cnf and \psi_dif constraints */

			if (symbolic == false) {
				assert(!itmCnstr.isNull() && "error processing conflict constraints.....exiting.....");
				if (premise.isNull()) premise = itmCnstr;
				else premise = OrExpr::create(premise, itmCnstr);

				/* sudiptac: add support for set-associative caches */
				if (uniqueConflict.isNull()) logSetAssociativeCnstr(itmCnstr, i);
				else logSetAssociativeCnstr(AndExpr::create(uniqueConflict, itmCnstr), i);
			} else {
				/* <caution> ::: the following code has a lot of branching related to optimization of
				 * constraints. Double check that the constraint formulation is correct */
				assert(symbolic == true && "non-symbolic cache access cannot be here");
				ref<Expr> join = AndExpr::create(setCnstr, tagCnstr);
				if (!itmCnstr.isNull()) join = AndExpr::create(join, itmCnstr);
				if (!uniqueConflict.isNull()) join = AndExpr::create(join, uniqueConflict);

				/* optimization: intermediate solver calls to reduce the size of formula */
				/* to check if the two accesses at i and j cannot be conflicting */
				solver->mustBeFalse(data::SIDE_CHANNEL_ANALYSIS, state, join, mustBeFalse);

				/* handle the possibly conflicting case */
				if (!mustBeFalse) {
					if (premise.isNull()) premise = join;
					else premise = OrExpr::create(premise, join);

					/* sudiptac: add support for set-associative caches */
					logSetAssociativeCnstr(join, i);
				}
			}
		}
		/* here ends the history of cache access sequence for access "II" */

		/* the following condition might capture two reasons: (1) all prior conflicting
		 * addresses were constant addresses, or (2) there were no cache conflict */
		if (premise.isNull()) {
			/* no cache conflict for access "II", conflict count should be reset */
			if (!conflict) {
				/* do nothing */
			} else if (!mustBeFalse) {
				fixedConflictMisses++;
				/* sudiptac: for FIFO replacement policy */
				pairT nullCnstr(NULL, NULL);
				missConstraintLog[i] = nullCnstr;
			}
		} else if (nassoc == 1) /* only for direct-mapped caches */{
			/* create symbolic cache conflict variable */
			std::ostringstream ss;
			std::string missStr("conflict");
			ss << i;
			missStr += ss.str();
			const Array* array = Array::CreateArray(missStr.c_str(), 1);
			ref<Expr> read = Expr::createTempRead(array, 8);
			/* add newly generate symbolic cache miss variable to the symbolic variable log */
			symCacheMissLog.push_back(read);
			missVars.push_back(array);

			/* add in the sequence->variable map to compare with hit/miss sequence */
			symCacheMissMap[i] = read;

			/* set and reset of symbolic cache conflict variable */
			ref<Expr> zeroConflict = EqExpr::create(read, ConstantExpr::alloc(0, read->getWidth()));
			ref<Expr> oneConflict = EqExpr::create(read, ConstantExpr::alloc(1, read->getWidth()));

			ref<Expr> notPremise = NotExpr::create(premise);
			ref<Expr> evictCnstr = Expr::createImplies(premise, oneConflict);
			ref<Expr> notEvictCnstr = Expr::createImplies(notPremise, zeroConflict);

			/* <conflict_constraint_log>: add conflict constraints into a global log */
			pairT conflictCnstr(evictCnstr, notEvictCnstr);
			conflictConstraintLog[i] = conflictCnstr;
			/* sudiptac: for checking sequence-based cache attacks */
			pairT seqCnstr(premise, notPremise);
			seqConstraintLog[i] = seqCnstr;
		} else /* for set associative caches */{
			if (strncasecmp(policy, "f", 1) == 0) CacheChannelFIFO::processFIFOConflictMissCnstr(i, state, solver);
		}
	}
}

// process constraints for cold cache misses
void CacheChannel::processColdMissCnstr(ExecutionState& state, TimingSolver* solver) {
	std::map<int, bool> coldMissMap;

	/* In the first pass, only handle the constant expressions */
	for (addrT::iterator II = state.memAddrs.begin(); II != state.memAddrs.end(); II++) {
		ref<Expr> address = *II;
		/* address is a constant expression */
		if (address->getKind() == Expr::Constant) {
			/* get the address as a raw 64-bit integer */
			ConstantExpr* CE = dyn_cast < ConstantExpr > (address);
			uint64_t addrRaw = CE->getZExtValue();
			int set = CACHE_SET(addrRaw, nset, line);
			/* compute cold miss for this set */
			if (!coldMissMap.count(set)) {
				coldMissMap[set] = true;
				fixedColdMisses++;
			}
		}
	}

	std::vector<int> remainingSets;

	/* optimization: check cold misses in the symbolic expression only for
	 those cache sets that are not touched by constant expressions */
	for (int II = 0; II < nset; II++) {
		if (!coldMissMap.count(II)) remainingSets.push_back(II);
	}

	/* no constaint is generated if all cache lines are touched by constant
	 * address expressions */
	if (remainingSets.empty()) return;

	std::vector<ref<Expr> > SymSetAddr;

	/* In the second pass, handle the rest of the expressions (i.e. symbolic) */
	for (addrT::iterator II = state.memAddrs.begin(); II != state.memAddrs.end(); II++) {
		ref<Expr> address = *II;

		if (address->getKind() == Expr::Constant) continue;

		ref<Expr> blAddr = LShrExpr::create(address, ConstantExpr::alloc(log_base2(line), address->getWidth()));
		ref<Expr> setAddr = AndExpr::create(blAddr, ConstantExpr::alloc(nset - 1, address->getWidth()));
		SymSetAddr.push_back(setAddr);
	}

	/* if there does not exist any symbolic address, we are done here */
	if (SymSetAddr.empty()) return;

	ref<Expr> setCheckExpr = NULL;

	/* now process the sequence of symbolic addresses set-by-set */

	for (std::vector<int>::iterator II = remainingSets.begin(); II != remainingSets.end(); II++) {
		int set = *II;
		/* symbolic variable, placeholders for cold cache misses */
		std::string missStr("cold");

		for (std::vector<ref<Expr> >::iterator IIR = SymSetAddr.begin(); IIR != SymSetAddr.end(); IIR++) {
			ref<Expr> setExpr = *IIR;
			ref<Expr> EqExpr = EqExpr::create(setExpr, ConstantExpr::alloc(set, setExpr->getWidth()));
			bool mustBeFalse = true;
			solver->mustBeFalse(data::SIDE_CHANNEL_ANALYSIS, state, EqExpr, mustBeFalse);

			if (mustBeFalse) {
				continue;
			}
			if (setCheckExpr.isNull()) setCheckExpr = EqExpr;
			else {
				setCheckExpr = OrExpr::create(setCheckExpr, EqExpr);
			}
			atomicCnstr++;
		}

		/* skip if none of the symbolic addresses could be mapped to cache-set "set" */
		if (setCheckExpr.isNull()) continue;

		/* at this moment setCheckExpr contains ((set(s1) = s) \/ (set(s2) = s) \/ .... \/ (set(sn) = s)) */
		std::ostringstream ss;
		ss << set;
		missStr += ss.str();

		/* create hit and miss effects depending on the conflicts */
		const Array* array = Array::CreateArray(missStr.c_str(), 1);
		ref<Expr> read = Expr::createTempRead(array, 8);

		/* add newly generate symbolic cache miss variable to the symbolic variable log */
		symCacheMissLog.push_back(read);
		missVars.push_back(array);

		ref<Expr> missEff = EqExpr::create(read, ConstantExpr::alloc(1, read->getWidth()));
		ref<Expr> hitEff = EqExpr::create(read, ConstantExpr::alloc(0, read->getWidth()));

		/* Now join the premise and conclusion: (setCheckExpr)=>(hit/miss) */
		ref<Expr> notSetCheckExpr = NotExpr::create(setCheckExpr);
		/* cold miss constraint */
		ref<Expr> coldMissCnstr = Expr::createImplies(setCheckExpr, missEff);
		/* not cold miss constraints */
		ref<Expr> notColdMissCnstr = Expr::createImplies(notSetCheckExpr, hitEff);

		/* <Cache_Constraint_System>: add these constraints in the constraint log */
		/* sudiptac: make sure to add the tag <Cache_Constraint_System> when adding constraints
		 * to allConstraintLog */
		allConstraintLog.push_back(coldMissCnstr);
		allConstraintLog.push_back(notColdMissCnstr);
	}
	/* sudiptac: work with the cold-miss constraint generation is done here */
}

// record constraints related only to set-associative caches
void CacheChannel::logSetAssociativeCnstr(ref<Expr> constraint, unsigned accessID, bool reload) {
	static unsigned ID = 0;

#ifdef _DEBUG
	fprintf(stdout, "logging set-associative cache constraints\n");
#endif

	if (reload) {
		assert(!setAssocReloadCnstrLog.count(accessID) && "reload constaint loaded only once");
		/* check whether it is a (sure) cold miss */
		if (constraint.isNull()) {
			fixedColdMisses++;
			/* sudiptac: for FIFO replacement policy */
			pairT nullCnstr(NULL, NULL);
			missConstraintLog[accessID] = nullCnstr;
		}
		setAssocReloadCnstrLog[accessID] = constraint;
	} else {
		/* create symbolic cache conflict variable */
		std::string missStr("missIJ");
		std::ostringstream ss;
		ss << accessID;
		ss << ID;
		ID++;
		missStr += ss.str();
		const Array* array = Array::CreateArray(missStr.c_str(), 1);
		ref<Expr> read = Expr::createTempRead(array, 8);
		if (setAssocCnstrLog.count(accessID)) setAssocCnstrLog[accessID].push_back(read);
		else {
			std::vector<ref<Expr> > cnstrSet;
			cnstrSet.push_back(read);
			setAssocCnstrLog[accessID] = cnstrSet;
		}
		ref<Expr> zeroConflict = EqExpr::create(read, ConstantExpr::alloc(0, read->getWidth()));
		ref<Expr> oneConflict = EqExpr::create(read, ConstantExpr::alloc(1, read->getWidth()));
		ref<Expr> notConstraint = NotExpr::create(constraint);
		ref<Expr> evictCnstr = Expr::createImplies(constraint, oneConflict);
		ref<Expr> notEvictCnstr = Expr::createImplies(notConstraint, zeroConflict);
		pairT conflictCnstr(evictCnstr, notEvictCnstr);
		if (setAssocConflictCnstrLog.count(accessID)) setAssocConflictCnstrLog[accessID].push_back(conflictCnstr);
		else {
			std::vector<pairT> cnstrSet;
			cnstrSet.push_back(conflictCnstr);
			setAssocConflictCnstrLog[accessID] = cnstrSet;
		}
	}
}

// dump cache constraints into a file
void CacheChannel::printCacheConstraints(ExecutionState& state) {
	printf("\n\n");
	printf("printing cache constraints....\n\n");
	ConstraintManager cm = state.constraints();
//	ExprSMTLIBPrinter printer;
//
//	printer.setOutput(llvm::errs());

#if 0
	for (ConstraintManager::constraint_iterator ci = cm.begin(); ci != cm.end(); ci++) {
		//	std::string Str("cache.smt");
		//	llvm::raw_string_ostream cacheInfo(Str);
		//	printer.printMemoryExpression(*ci);
		cm.addConstraint(*ci);
	}
#endif

	Query query(cm, ConstantExpr::alloc(0, Expr::Bool));
//	printer.setQuery(query);
//	printer.generateOutput();
}

/* prints memory addresses */
void CacheChannel::printMemoryAddr(ExecutionState& state) {
	printf("\n\n");
	printf("Number of symbolic addresses::=> [%ld]\n", state.symbolicAddrNum);
	printf("Number of constant addresses::=> [%ld]\n", state.concreteAddrNum);
	printf("\n\n");
#ifdef _CACHE_CHANNEL_DEBUG
	printf("########## Printing all addresses ##########\n");
	fflush(stdout);

	/* sudiptac: the following code is guarded by a disabled flag due to the large
	 * amount of file I/O being occurred for complex memory expressions. To enable
	 * the following code, enable the _CACHE_CHANNEL_DEBUG flag */
	for (addrT::iterator II = state.memAddrs.begin(); II != state.memAddrs.end(); II++) {
		ref<Expr> address = *II;
		/* sudiptac: additional code to print symbolic memory addresses in SMT2 format */
		if (address->getKind() == Expr::Constant) {
			printf("\n\n");
			printf("Memory address is constant::=>\n");
			fflush(stdout);
			llvm::errs() << address;
			llvm::errs().flush();
		}
		else {
			printf("\n\n");
			printf("Memory address is symbolic::=>\n");
			fflush(stdout);
			ExprSMTLIBPrinter printer;
			printer.setOutput(llvm::errs());
			printer.printMemoryExpression(address);
		}
	}
	printf("\n\n");
	printf("########## end printing all addresses ##########\n");
	fflush(stdout);
	/* printing symbolic addresses end */
#endif
}

