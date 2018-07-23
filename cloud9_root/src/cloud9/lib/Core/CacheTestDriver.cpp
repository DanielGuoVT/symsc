/*
 * CacheTestDriver.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: sjguo
 */

#include <stdio.h>
#include "CacheTestDriver.h"

#define SOL_THRESHOLD 500
#define SIZE_OF_SECRET 8 /* find a better solution */
#define SIZE_OF_BYTE 8	/* find a better solution */
#define OBS_START 0
#define OBS_END 400
#define WINDOW_SIZE 0

/* check for byte-level information leak through observing cache-miss count */
void CacheTestDriver::generateTestBasedOnMissCount(ExecutionState& state, TimingSolver* solver) {

	fprintf(stdout, "\n\n##### Number of (input)-independent cache misses = %lu #####\n\n", \
		fixedColdMisses + fixedConflictMisses);
	fprintf(stdout, "\n\n##### Number of (input)-independent cold misses = %d #####\n\n", \
		fixedColdMisses);
	fprintf(stdout, "\n\n##### Number of (input)-independent conflict misses = %lu #####\n\n", \
		fixedConflictMisses);

	/* fast path: when all addresses are constant */
	if (state.symbolicAddrNum == 0) {
		return;
	}

	ExecutionState cState(state);

	/* add cache-conflict and/or cold-miss related constraints */
	/* the constraint log depends on the type of cache (direct-mapped or set-associative) */
	if (nassoc == 1)
		CacheDriver::addConflictConstraints(cState, conflictConstraintLog);
	else {
		if (strncasecmp(policy, "l", 1) == 0) {
			fprintf(stdout, "set-associative LRU cache detected");
			CacheDriver::addConflictConstraints(cState, conflictConstraintLogLRU);
		}
		else if (strncasecmp(policy, "f", 1) == 0) {
			fprintf(stdout, "set-associative FIFO cache detected");
			CacheDriver::addConflictConstraints(cState, conflictConstraintLogFIFO);
		}
		else
			assert(0 && "fatal: replacement policy is not supported");
	}

	/* generate all tests for each side-channel observation */
	CacheChannel::getAllTests(cState, solver, SOL_THRESHOLD);

}

/* check for byte-level information leak through a (sequence) of cache misses */
void CacheTestDriver::generateTestBasedOnMissSequence(ExecutionState& state, TimingSolver* solver, logT& cnstrLog) {

	/* fast path: when all addresses are constant */
	if (state.symbolicAddrNum == 0) {
		return;
	}

	ExecutionState cState(state);
	std::string missStr("test_sym_miss");

	/* clean the list of symbolic miss variables */
	missVars.clear();

	/* check for every memory access and possible information leaks through each of them */
	for (unsigned II = 1; II < state.memAddrs.size(); II++) {
		ref<Expr> checkCnstr = NULL;
		ref<Expr> checkCnstrNeg = NULL;

		if (cnstrLog.count(II)) {
				checkCnstr = cnstrLog[II].first;
				checkCnstrNeg = cnstrLog[II].second;
		}
		/* do this optimization  only for direct-mapped caches */
		if (coldSeqConstraintLog.count(II) && nassoc == 1) {
			if (checkCnstr.isNull()) {
				checkCnstr = coldSeqConstraintLog[II].first;
				checkCnstrNeg = coldSeqConstraintLog[II].second;
			}
			else {
				checkCnstr = OrExpr::create(checkCnstr, coldSeqConstraintLog[II].first);
				checkCnstrNeg = AndExpr::create(checkCnstrNeg, coldSeqConstraintLog[II].second);
			}
		}

		/* cache hit/miss statistics is independent of data flow, so skip this access */
		if (checkCnstr.isNull()) continue;

		/* perform the following step only for set-associative caches */
		/* add intermediary conflict constraints for set-associative caches */
		if (setAssocConflictCnstrLog.count(II) && nassoc > 1) {
			fprintf(stdout, "logging %lu intermediate conflicts for access %u\n", setAssocConflictCnstrLog.size(), II);
			for (unsigned JJ = 0; JJ < setAssocConflictCnstrLog[II].size(); JJ++) {
				checkCnstr = AndExpr::create(checkCnstr, setAssocConflictCnstrLog[II][JJ].first);
				checkCnstr = AndExpr::create(checkCnstr, setAssocConflictCnstrLog[II][JJ].second);
				checkCnstrNeg = AndExpr::create(checkCnstrNeg, setAssocConflictCnstrLog[II][JJ].first);
				checkCnstrNeg = AndExpr::create(checkCnstrNeg, setAssocConflictCnstrLog[II][JJ].second);
			}
		}
    /* create symbolic cache conflict variable */
    std::ostringstream ss;
    ss << II;
    missStr += ss.str();
    const Array* array = Array::CreateArray(missStr.c_str(), 1);
    ref<Expr> read = Expr::createTempRead(array, 8);

    /* add newly generate symbolic cache miss variable to the symbolic variable log */
    missVars.push_back(array);

    /* set and reset of symbolic cache conflict variable */
    ref<Expr> zero = EqExpr::create(read, ConstantExpr::alloc(0, read->getWidth()));
    ref<Expr> one = EqExpr::create(read, ConstantExpr::alloc(1, read->getWidth()));

    ref<Expr> miss = Expr::createImplies(checkCnstr, one);
    ref<Expr> hit = Expr::createImplies(checkCnstrNeg, zero);

		/* add constraints into the execution state */
		cState.addConstraint(miss);
		cState.addConstraint(hit);
	}

	/* generate all tests for each side-channel observation */
	CacheChannel::getAllTests(cState, solver, SOL_THRESHOLD);
}

/* entry level wrapper to generate all tests based on side-channel observations */
void CacheTestDriver::generateAllTestBasedOnObserverModel(ExecutionState& state, TimingSolver* solver) {

#define __CACHE_CHECK_TIMING_LEAK // Daniel.
#ifdef __CACHE_CHECK_TIMING_LEAK
	CacheChannel::observer = MISS_COUNT;
	generateTestBasedOnMissCount(state, solver);
#endif

#define __CACHE_CHECK_SEQ_LEAK // Daniel.
#ifdef __CACHE_CHECK_SEQ_LEAK
	CacheChannel::observer = MISS_SEQUENCE;
	generateTestBasedOnMissSequence(state, solver, seqConstraintLog);
#endif

#ifdef __CACHE_CHECK_SEQ_LEAK_LRU
	CacheChannel::observer = MISS_SEQUENCE;
	generateTestBasedOnMissSequence(state, solver, seqConstraintLogLRU);
#endif
}



