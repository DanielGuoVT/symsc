/*
 * CacheDriver.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: sjguo
 */

#include <stdio.h>
#include "CacheDriver.h"

#define SOL_THRESHOLD 500
#define SIZE_OF_SECRET 4 /* find a better solution */
#define SIZE_OF_BYTE 8	/* find a better solution */
#define OBS_START 0
#define OBS_END 400
#define WINDOW_SIZE 0

std::map<unsigned long, ref<Expr> > CacheDriver::missToState;
std::map< std::pair<unsigned long,bool>, ref<Expr> > CacheDriver::missSeqToState;

// constrain one byte with a specific value
ref<Expr> CacheDriver::constrainOneByte(ExecutionState& state, int nbyte, int value) {
	ref<Expr> ret = NULL;

	for (unsigned i = 0; i < state.symbolics.size(); i++) {
		UpdateList ul(state.symbolics[i].second, 0);
		ref<Expr> read = ReadExpr::create(ul, ConstantExpr::create(nbyte, Expr::Int32));
		ref<Expr> valueEx = ConstantExpr::alloc(value, read->getWidth());
		if (ret.isNull())
			ret = EqExpr::create(read, valueEx);
		else
			ret = AndExpr::create(ret, EqExpr::create(read, valueEx));
	}

	assert(ret.isNull()==0 && "constraining byte of a key went wrong");
	return ret;
}

// constrain one bit at a time for a secret input
ref<Expr> CacheDriver::constrainOneBit(ExecutionState& state, int nbit, bool trueorfalse) {
	ref<Expr> ret = NULL;

	for (unsigned SI = 0; SI < state.symbolics.size(); SI++) {
			UpdateList ul(state.symbolics[SI].second, 0);

			/* get the byte position from the bit number */
			int nbyte = nbit/SIZE_OF_BYTE;
			int posbyte = nbit % SIZE_OF_BYTE;
			ref<Expr> read = ReadExpr::create(ul, ConstantExpr::create(nbyte, Expr::Int32));

			/* TODO: change to a more efficient implementation with bit-level read? */
			for (int num = 0x00; num <= 0xFF; num++) {
				bool check = (trueorfalse) ? ((num & (1 << posbyte)) != 0) : ((num & (1 << posbyte)) == 0);
				if (check) {
					ref<Expr> value = ConstantExpr::alloc(num, read->getWidth());
					if (ret.isNull())
						ret = EqExpr::create(read, value);
					else
						ret = OrExpr::create(ret, EqExpr::create(read, value));
				}
			}
		}

		assert(!ret.isNull() && "bit constraint checking went wrong");

		return ret;
}

// add cache-conflict and/or cold-miss related constraints
void CacheDriver::addConflictConstraints(ExecutionState& state, logT& constraintLog) {
	int count = 0;

	/* adding cold miss constraint */
	for (addrT::iterator II = allConstraintLog.begin(); II != allConstraintLog.end(); II++) {
		/* for getting model of the solution */
		state.addConstraint(*II);
		fprintf(stdout, "adding constraint %d\n", count++);
	}

	/* sudiptac: experimenting a different order of constraints for insertion */
	for (unsigned II = 0; II < state.memAddrs.size(); II++) {
		if (constraintLog.count(II)) {
			state.addConstraint(constraintLog[II].first);
			state.addConstraint(constraintLog[II].second);
			fprintf(stdout, "adding constraint(c) %d\n", count++);
			/* only for set-associative caches */
			if (nassoc > 1) {
				assert(setAssocConflictCnstrLog.count(II) && "something went wrong in processing \
					constraints for set-associative caches");
				/* add bounds for cache conflicts */
				for (unsigned JJ = 0; JJ < setAssocConflictCnstrLog[II].size(); JJ++) {
					state.addConstraint(setAssocConflictCnstrLog[II][JJ].first);
					state.addConstraint(setAssocConflictCnstrLog[II][JJ].second);
				}
			}
		}
	}

	fprintf(stdout, "\n\n########## calling the solver with constraint size %d ##########\n\n", count + 1);
}

// checker to detect timing-based side-channel attack
void CacheDriver::checkForTimingAttack(ExecutionState& state, TimingSolver* solver) {

	fprintf(stdout, "\n\n##### Number of (input)-independent cache misses = %lu #####\n\n", fixedColdMisses + fixedConflictMisses);

	/* fast path: when all addresses are constant */
	if (state.symbolicAddrNum == 0) {
		return;
	}

	ExecutionState cState(state);

	/* add cache-conflict and/or cold-miss related constraints */
	addConflictConstraints(cState, conflictConstraintLog);

	/* check for different timing constraints */
	for(int omiss = OBS_START; omiss < OBS_END; omiss++) {
		/* add miss count constraint */
		ref<Expr> missCnstr = CacheChannel::processMissCountCnstr(omiss);
		observedCacheMiss = fixedColdMisses + fixedConflictMisses + omiss;
		bool rs = CacheChannel::sanityCheck(state, fixedColdMisses+fixedConflictMisses, observedCacheMiss);
		if (rs) continue;
		if (missCnstr.isNull()) continue;

		/* we come here if there exists, at least one symbolic address */
		int nsolution = CacheChannel::solveAllConstraints(cState, solver, missCnstr, SOL_THRESHOLD);

		fprintf(stdout, "\n\n..... #solutions for %lu cache misses = %d .....\n", observedCacheMiss, nsolution);
		fprintf(stdout, "..... probability to infer input = (1/%d) .....\n\n", nsolution);
	}

	fprintf(stdout, "\n\n########## Solver terminates, we are done here ##########\n\n");
	fflush(stdout);
}

// checker to detect sequence-based side-channel attack
void CacheDriver::checkForSeqAttack(ExecutionState& state, TimingSolver* solver) {
	/* for each observation cache access, check how many keys could lead to hit/miss */

	for (unsigned II = 0; II < state.memAddrs.size(); II++) {
		if (seqConstraintLog.count(II)) {
			int nsolutionHit = solveAllConstraints(state, solver, seqConstraintLog[II].second, SOL_THRESHOLD);
			fprintf(stdout, "\n\n..... #solutions for #%u access hit = %d .....\n", II+1 , nsolutionHit);
			fprintf(stdout, "..... probability to infer input = (1/%d) .....\n", nsolutionHit);
			if (nsolutionHit == 0) continue;
			int nsolutionMiss = solveAllConstraints(state, solver, seqConstraintLog[II].first, SOL_THRESHOLD);
			fprintf(stdout, "\n\n..... #solutions for #%u access miss = %d .....\n", II+1 , nsolutionMiss);
			fprintf(stdout, "..... probability to infer input = (1/%d) .....\n", nsolutionMiss);
		}
	}

	fprintf(stdout, "\n\n########## Solver terminates, we are done here ##########\n\n");
	fflush(stdout);
}

// check for byte-level information leak from path constraints (miss-sequence-based observers)
void CacheDriver::checkForByteLeakSeqFromPathConstraints(ExecutionState& state, TimingSolver* solver) {
	fprintf(stdout, "\n\n#### Now checking information leak through path constraints and miss sequence ####\n\n");
	fflush(stdout);

	std::map< std::pair<unsigned long, bool>, ref<Expr> >::iterator it;

	/* clean the initial constraints */
	state.constraints().clean();

	/* iterate over all possible observations */
	for (it = missSeqToState.begin(); it != missSeqToState.end(); it++) {
		/* check initial constraints (fast path) */
		if ((*it).first.second) {
			std::pair<unsigned long, bool> test((*it).first.first, false);
			if (!missSeqToState.count(test)) {
				fprintf(stdout, "\n\n##### NO information leak detected for access %lu\n\n", (*it).first.first);
				fflush(stdout);
				continue;
			}
		} else {
			std::pair<unsigned long, bool> test((*it).first.first, true);
			if (!missSeqToState.count(test)) {
				fprintf(stdout, "\n\n##### NO information leak detected for access %lu\n\n", (*it).first.first);
				fflush(stdout);
				continue;
			}
		}

		/* create initial state vector for the respective observation */
		ExecutionState cState(state);
		cState.addConstraint((*it).second);

		/* check whether the solution is different for different value of each byte (total 256*16 calls) */
		for (int nbyte = 0; nbyte < SIZE_OF_SECRET; nbyte++) {
			fprintf(stdout, "\n\n###### checking potential information leak via byte %d, access %lu\n\n", nbyte, (*it).first.first);
			bool leak = false;
			for (int value = 0x00; value <= 0xFF; value++) {
				ref<Expr> byteCnstr = constrainOneByte(cState, nbyte, value);
				int nsolutionD = CacheChannel::solveAllConstraints(cState, solver, byteCnstr, 1);
				if (1 != nsolutionD) {
					fprintf(stdout, "\n\n##### (**) information leak detected in byte %d, access %lu\n", nbyte, (*it).first.first);
					leak = true;
				}
			}
			if (leak == false) {
				fprintf(stdout, "\n\n##### NO information leak found via byte %d, access %lu\n", nbyte, (*it).first.first);
			}
		}
	}
}

// check for byte-level information leak from path constraints (miss-count-based observers)
void CacheDriver::checkForByteLeakFromPathConstraints(ExecutionState& state, TimingSolver* solver) {
	fprintf(stdout, "\n\n#### Now checking information leak through path constraints ####\n\n");
	fflush(stdout);

	std::map<unsigned long, ref<Expr> >::iterator it;

	/* clean the initial constraints */
	state.constraints().clean();

	/* iterate over all possible observations */
	for (it = missToState.begin(); it != missToState.end(); it++) {
		/* create initial state vector for the respective observation */
		ExecutionState cState(state);
		cState.addConstraint((*it).second);

		/* check whether the solution is different for different value of each byte (total 256*16 calls) */
		for (int nbyte = 0; nbyte < SIZE_OF_SECRET; nbyte++) {
			fprintf(stdout, "\n\n###### checking potential information leak via byte %d, observation %lu\n\n", nbyte, (*it).first);
			bool leak = false;
			for (int value = 0x00; value <= 0xFF; value++) {
				ref<Expr> byteCnstr = constrainOneByte(cState, nbyte, value);
				int nsolutionD = CacheChannel::solveAllConstraints(cState, solver, byteCnstr, 1);
				if (1 != nsolutionD) {
					fprintf(stdout, "\n\n##### (**) information leak detected in byte %d, observation %lu\n", nbyte, (*it).first);
					leak = true;
				}
			}
			if (leak == false) {
				fprintf(stdout, "\n\n##### NO information leak found via byte %d, observation %lu\n", nbyte, (*it).first);
			}
		}
	}
}

// check for byte-level information leak through observing cache-miss count
void CacheDriver::checkForByteLeakFromMissCount(ExecutionState& state, TimingSolver* solver) {

	fprintf(stdout, "\n\n##### Number of (input)-independent cache misses = %lu #####\n\n", fixedColdMisses + fixedConflictMisses);
	fprintf(stdout, "\n\n##### Number of (input)-independent cold misses = %d #####\n\n", fixedColdMisses);
	fprintf(stdout, "\n\n##### Number of (input)-independent conflict misses = %lu #####\n\n", fixedConflictMisses);
	fflush(stdout);

	/* fast path: when all addresses are constant */
	if (state.symbolicAddrNum == 0) {
		unsigned long nmisses = fixedColdMisses+fixedConflictMisses;
		ConstraintManager cm = state.constraints();
		ref<Expr> pathCnstr = NULL;
		for (ConstraintManager::constraint_iterator ci = cm.begin(); ci != cm.end(); ci++) {
			if (pathCnstr.isNull())
				pathCnstr = *ci;
			else
				pathCnstr = AndExpr::create(pathCnstr, *ci);
		}
		if (pathCnstr.isNull() == 0) {
#ifdef _DEBUG
			pathCnstr->dump();
#endif
			if (missToState.count(nmisses)) {
				missToState[nmisses] = OrExpr::create(missToState[nmisses], pathCnstr);
			} else {
				missToState[nmisses] = pathCnstr;
			}
		}
		return;
	}

	ExecutionState cState(state);

	/* add cache-conflict and/or cold-miss related constraints */
	/* the constraint log depends on the type of cache (direct-mapped or set-associative) */
	if (nassoc == 1)
		addConflictConstraints(cState, conflictConstraintLog);
	else {
		if (strncasecmp(policy, "l", 1) == 0) {
			fprintf(stdout, "set-associative LRU cache detected");
			addConflictConstraints(cState, conflictConstraintLogLRU);
		}
		else
			assert(0 && "fatal: replacement policy is not supported");
	}

	/* check for different timing constraints and possible information leaks through them */
	for (int omiss = OBS_START; omiss < OBS_END; omiss++) {
		ref<Expr> missCnstr = CacheChannel::processMissCountCnstr(omiss);
		observedCacheMiss = fixedColdMisses + fixedConflictMisses + omiss;
		bool rs = CacheChannel::sanityCheck(state, fixedColdMisses+fixedConflictMisses, observedCacheMiss);

		if (rs) continue;
		if (missCnstr.isNull()) continue;

		int nsolution = CacheChannel::solveAllConstraints(cState, solver, missCnstr, 1);
		/* no solution exists, not an observational constraint */
		if (nsolution == 0) continue;
		/* check whether the solution is different for different value of each byte (total 256*16 calls) */
		for (int nbyte = 0; nbyte < SIZE_OF_SECRET; nbyte++) {
			fprintf(stdout, "\n\n###### checking potential information leak via byte %d\n\n", nbyte);
			bool leak = false;
			for (int value = 0x00; value <= 0xFF; value++) {
				ref<Expr> byteCnstr = constrainOneByte(state, nbyte, value);
				ref<Expr> byteLeak = AndExpr::create(byteCnstr, missCnstr);
				int nsolutionD = CacheChannel::solveAllConstraints(cState, solver, byteLeak, 1);
				if (nsolution != nsolutionD) {
					fprintf(stdout, "\n\n##### (**) information leak detected in byte %d\n", nbyte);
					leak = true;
				}
			}
			if (leak == false) {
				fprintf(stdout, "\n\n##### NO information leak found via byte %d\n", nbyte);
			}
		}
		/* sudiptac: remove this */
    exit(0);
	}
}

// check for bit-level information leak through observing cache-miss count
void CacheDriver::checkForBitLeakFromMissCount(ExecutionState& state, TimingSolver* solver) {

	fprintf(stdout, "\n\n##### Number of (input)-independent cache misses = %lu #####\n\n", fixedColdMisses + fixedConflictMisses);

	/* fast path: when all addresses are constant */
	if (state.symbolicAddrNum == 0) {
		return;
	}

	ExecutionState cState(state);

	/* the constraint log depends on the type of cache (direct-mapped or set-associative) */
	if (nassoc == 1)
		addConflictConstraints(cState, conflictConstraintLog);
	else {
		if (strncasecmp(policy, "l", 1) == 0) {
			fprintf(stdout, "\n\n.....set-associative LRU cache detected.....\n\n");
			addConflictConstraints(cState, conflictConstraintLogLRU);
		}
		else
			assert(0 && "fatal: replacement policy is not supported");
	}

	/* check for different timing constraints and possible information leaks through them */
	for (int omiss = OBS_START; omiss < OBS_END; omiss++) {
		/* add observable cache-miss constraint */
		ref<Expr> missCnstr = CacheChannel::processMissCountCnstr(omiss);
		observedCacheMiss = fixedColdMisses + fixedConflictMisses + omiss;

		bool rs = CacheChannel::sanityCheck(state, fixedColdMisses+fixedConflictMisses, observedCacheMiss);

		if (rs) continue;
		if (missCnstr.isNull()) continue;

		for (int nbit = 0; nbit < SIZE_OF_SECRET * SIZE_OF_BYTE; nbit++) {
			ref<Expr> bitTrueCnstr = constrainOneBit(state, nbit, true);
			ref<Expr> bitFalseCnstr = constrainOneBit(state, nbit, false);
			ref<Expr> trueLeak = AndExpr::create(bitTrueCnstr, missCnstr);
			ref<Expr> falseLeak = AndExpr::create(bitFalseCnstr, missCnstr);

			fprintf(stdout, "\n\n###### checking potential information leak via bit %d\n\n", nbit);
			/* check satisfiability when the bit is true */
			int nsolutionTrue = CacheChannel::solveAllConstraints(cState, solver, trueLeak, 1);
			/* check satisfiability when the bit is false */
			int nsolutionFalse = CacheChannel::solveAllConstraints(cState, solver, falseLeak, 1);

			/* potential leak situation */
			if (nsolutionTrue != nsolutionFalse) {
				fprintf(stdout, "\n\n######## (**) potential information leak via bit %d\n", nbit);
			} else {
				fprintf(stdout, "\n\n######## NO potential information leak via bit %d\n", nbit);
			}
		}
	}
}

// check for byte-level information leak through a (sequence) of cache misses
void CacheDriver::checkForByteLeakFromSequence(ExecutionState& state, TimingSolver* solver, logT& cnstrLog) {

	/* fast path: when all addresses are constant */
	if (state.symbolicAddrNum == 0) {
		/* return; */ /* sudiptac: changed due to check multiple paths */
	}

	ExecutionState cState(state);

	ref<Expr> allCnstr = NULL;
	ref<Expr> allCnstrNeg = NULL;
	int wlength = 0;

	/* check for every memory access and possible information leaks through each of them */
	for (unsigned II = 1; II < state.memAddrs.size(); II++) {
	//for (unsigned II = 1; II < 50; II++) {
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

		/* cache hit/miss statistics is independent of data flow, so skip this access, but
		 * but record path constraints */
		if (checkCnstr.isNull()) {
			bool nmisses = false;
			if (missConstraintLog.count(II) && missConstraintLog[II].first.isNull())
				nmisses = true;
			ConstraintManager cm = state.constraints();
			ref<Expr> pathCnstr = NULL;
			for (ConstraintManager::constraint_iterator ci = cm.begin(); ci != cm.end(); ci++) {
				if (pathCnstr.isNull())
					pathCnstr = *ci;
				else
					pathCnstr = AndExpr::create(pathCnstr, *ci);
			}
			if (pathCnstr.isNull() == 0) {
#ifdef _NDEBUG
				pathCnstr->dump();
#endif
				std::pair<unsigned long, bool> missI(II, nmisses);
				if (missSeqToState.count(missI)) {
					missSeqToState[missI] = OrExpr::create(missSeqToState[missI], pathCnstr);
				} else {
					missSeqToState[missI] = pathCnstr;
				}
			}
			continue;
		}

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

		/* end specialized computations for set-associative caches */
		/* accumulate constraints in the current sliding window */
		allCnstr = (allCnstr.isNull()) ? checkCnstr : AndExpr::create(allCnstr, checkCnstr);
		allCnstrNeg = (allCnstrNeg.isNull()) ? checkCnstrNeg : AndExpr::create(allCnstrNeg, checkCnstrNeg);

		/* continue if the length of sliding window has not expired */
		if (wlength < WINDOW_SIZE) {
			wlength++;
			continue;
		}
		wlength = 0;

	  int nsolution = CacheChannel::solveAllConstraints(cState, solver, allCnstr, 1);
	  int nsolutionNeg = CacheChannel::solveAllConstraints(cState, solver, allCnstrNeg, 1);

		/* no solution exists, not an observational constraint */
		if ((nsolution + nsolutionNeg) == 0) continue;

		/* check whether the solution is different for different value of each byte (total 256*16 calls) */
		for (int nbyte = 1; nbyte < SIZE_OF_SECRET; nbyte++) {
			fprintf(stdout, "\n\n###### checking potential information leak via byte %d, access %u\n\n", nbyte, II);
			bool leak = false;
			for (int value = 0x00; value <= 0xFF; value++) {
				ref<Expr> byteCnstr = constrainOneByte(state, nbyte, value);
				ref<Expr> byteLeak = AndExpr::create(byteCnstr, allCnstr);
				ref<Expr> byteLeakNeg = AndExpr::create(byteCnstr, allCnstrNeg);
				int nsolutionD = CacheChannel::solveAllConstraints(cState, solver, byteLeak, 1);
				int nsolutionDN = CacheChannel::solveAllConstraints(cState, solver, byteLeakNeg, 1);
#if 0
				if ((nsolution != nsolutionD) || (nsolutionNeg != nsolutionDN)) {
					fprintf(stdout, "\n\n##### (**) information leak detected in byte %d, access %u\n", nbyte, II);
					leak = true;
				}
#endif
				if (nsolution != nsolutionD) {
					fprintf(stdout, "\n\n##### (**)[++] information leak detected in byte %d, access %u\n", nbyte, II);
					leak = true;
				}
				if (nsolutionNeg != nsolutionDN) {
					fprintf(stdout, "\n\n##### (**)[--] information leak detected in byte %d, access %u\n", nbyte, II);
					leak = true;
				}
			}
			if (leak == false)
				fprintf(stdout, "\n\n##### NO information leak found via byte %d, access %u\n", nbyte, II);
		}
		allCnstr = NULL;
		allCnstrNeg = NULL;
	}

//	CacheChannel::printCacheCnstr(cState);
}

// check for bit-level information leak through a (sequence) of cache misses
void CacheDriver::checkForBitLeakFromSequence(ExecutionState& state, TimingSolver* solver, logT& cnstrLog) {

	/* fast path: when all addresses are constant */
	if (state.symbolicAddrNum == 0) {
		return;
	}

	ExecutionState cState(state);

	ref<Expr> allCnstr = NULL;
	ref<Expr> allCnstrNeg = NULL;
	int wlength = 0;

	/* check for every memory access and possible information leaks through each of them */
	for (unsigned II = 1; II < state.memAddrs.size(); II++) {
		ref<Expr> checkCnstr = NULL;
		ref<Expr> checkCnstrNeg = NULL;

		if (cnstrLog.count(II)) {
				checkCnstr = cnstrLog[II].first;
				checkCnstrNeg = cnstrLog[II].second;
		}
		/* do this optimization only for direct-mapped caches */
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
			for (unsigned JJ = 0; JJ < setAssocConflictCnstrLog[II].size(); JJ++) {
				checkCnstr = AndExpr::create(checkCnstr, setAssocConflictCnstrLog[II][JJ].first);
				checkCnstr = AndExpr::create(checkCnstr, setAssocConflictCnstrLog[II][JJ].second);
				checkCnstrNeg = AndExpr::create(checkCnstrNeg, setAssocConflictCnstrLog[II][JJ].first);
				checkCnstrNeg = AndExpr::create(checkCnstrNeg, setAssocConflictCnstrLog[II][JJ].second);
			}
		}
		/* end specialized computations for set-associative caches */

		/* accumulate constraints in the current sliding window */
		allCnstr = (allCnstr.isNull()) ? checkCnstr : AndExpr::create(allCnstr, checkCnstr);
		allCnstrNeg = (allCnstrNeg.isNull()) ? checkCnstrNeg : AndExpr::create(allCnstrNeg, checkCnstrNeg);

		/* continue if the length of sliding window has not expired */
		if (wlength < WINDOW_SIZE) {
			wlength++;
			continue;
		}
		wlength = 0;

		/* otherwise, check whether any information leak (bit-level) happen through this access */
		for (int nbit = 0; nbit < SIZE_OF_SECRET * SIZE_OF_BYTE; nbit++) {
			ref<Expr> bitTrueCnstr = constrainOneBit(state, nbit, true);
			ref<Expr> bitFalseCnstr = constrainOneBit(state, nbit, false);
			ref<Expr> trueLeak = AndExpr::create(bitTrueCnstr, allCnstr);
			ref<Expr> falseLeak = AndExpr::create(bitFalseCnstr, allCnstr);

			fprintf(stdout, "\n\n###### (++) checking potential information leak via bit %d, access %u\n\n", nbit, II);
			/* check satisfiability when the bit is true */
			int nsolutionTrue = CacheChannel::solveAllConstraints(cState, solver, trueLeak, 1);
			/* check satisfiability when the bit is false */
			int nsolutionFalse = CacheChannel::solveAllConstraints(cState, solver, falseLeak, 1);

			/* potential leak situation */
			if (nsolutionTrue != nsolutionFalse) {
				fprintf(stdout, "\n\n######## (**) potential information leak via bit %d, access %u\n", nbit, II);
				continue;
			}
			ref<Expr> trueLeakNeg = AndExpr::create(bitTrueCnstr, allCnstrNeg);
			ref<Expr> falseLeakNeg = AndExpr::create(bitFalseCnstr, allCnstrNeg);

			fprintf(stdout, "\n\n###### (--) checking potential information leak via bit %d, access %u\n\n", nbit, II);
			/* check satisfiability when the bit is true */
			int nsolutionTrueNeg = CacheChannel::solveAllConstraints(cState, solver, trueLeakNeg, 1);
			/* check satisfiability when the bit is false */
			int nsolutionFalseNeg = CacheChannel::solveAllConstraints(cState, solver, falseLeakNeg, 1);

			/* potential leak situation */
			if (nsolutionTrueNeg != nsolutionFalseNeg) {
				fprintf(stdout, "\n\n######## (**) potential information leak via bit %d, access %u\n", nbit, II);
				continue;
			} else {
				fprintf(stdout, "\n\n######## NO potential information leak via bit %d, access %u\n", nbit, II);
			}
		}
	}
}

/* This is an entry-level wrapper for checking all potential (side-channel) leaks and
 * attacks in the software under test */
void CacheDriver::checkForAttackAndLeak(ExecutionState& state, TimingSolver* solver) {

#define __CACHE_CHECK_COUNTING // Daniel.

#ifdef __CACHE_CHECK_COUNTING
	/* call checkers to detect (1) timing attacks and/or (2) seq-based attack */
	/* **** these are checkers based on counting the potential number of keys */
	checkForTimingAttack(state, solver);
	checkForSeqAttack(state, solver);
#endif


#ifdef __CACHE_CHECK_TIMING_LEAK
	/* check for information leaks at bit-level and byte-level */
	/* **** does not count keys, check whether the observation differs by changing bits */
	//checkForBitLeakFromMissCount(state, solver);
	/* check whether observation differs by changing bytes */
	checkForByteLeakFromMissCount(state, solver);
#endif

#ifdef __CACHE_CHECK_SEQ_LEAK
	/* check whether the observation of hit/miss sequence differs by changing bit */
	//checkForBitLeakFromSequence(state, solver, seqConstraintLog);
	/* check whether the observation of hit/miss sequence differs by changing byte */
	checkForByteLeakFromSequence(state, solver, seqConstraintLog);
#endif

#ifdef __CACHE_CHECK_SEQ_LEAK_LRU
	/* check whether the observation of hit/miss sequence differs by changing bit */
	//checkForBitLeakFromSequence(state, solver, CacheChannelLRU::seqConstraintLogLRU);
	/* check whether the observation of hit/miss sequence differs by changing byte */
	checkForByteLeakFromSequence(state, solver, CacheChannelLRU::seqConstraintLogLRU);
#endif
}



