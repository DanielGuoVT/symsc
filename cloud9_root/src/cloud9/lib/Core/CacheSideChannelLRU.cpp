/*
 * CacheSideChannelLRU.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: sjguo
 */

#include <stdio.h>
#include "CacheSideChannel.h"
logT CacheChannelLRU::seqConstraintLogLRU;

void CacheChannelLRU::processLRUConflictMissCnstr(ExecutionState& state, TimingSolver* solver) {

	fprintf(stdout, "\n\n......Now processing constraints for set-associative LRU caches......\n\n");

	/* sudiptac: clean up the array containing symbolic cache conflict variables. this is crucial
	 * for set-associative caches, as the list (if not cleaned) contain also symbolic variables
	 * related to direct-mapped caches */
	symCacheMissLog.clear();
	missVars.clear();

	for (unsigned II = 1; II < state.memAddrs.size(); II++) {
		ref<Expr> premise = NULL;

		if (setAssocCnstrLog.count(II)) {
			for (unsigned JJ = 0; JJ < setAssocCnstrLog[II].size(); JJ++) {
				if (premise.isNull()) premise = setAssocCnstrLog[II][JJ];
				else premise = AddExpr::create(premise, setAssocCnstrLog[II][JJ]);
			}
		}

		/* create constraint of the form conf_1 + conf_2 + ... + conf_n >= Cache_Associativity */
		if (!premise.isNull()) {
			ref<Expr> assocExpr = ConstantExpr::alloc(nassoc, premise->getWidth());
			premise = UgeExpr::create(premise, assocExpr);
		}

		/* let us ignore (must) cold misses */
		if (setAssocReloadCnstrLog.count(II) && setAssocReloadCnstrLog[II].isNull()) {
			continue;
		}

		if (setAssocReloadCnstrLog.count(II))
			premise = (premise.isNull()) ? setAssocReloadCnstrLog[II] : OrExpr::create(premise, setAssocReloadCnstrLog[II]);

		if (premise.isNull()) {
			continue;
		}

		fprintf(stdout, "logging sequence constraints\n");

		/* create symbolic cache conflict variable */
		std::ostringstream ss;
		std::string missStr("conflictLRU");
		ss << II;
		missStr += ss.str();
		const Array* array = Array::CreateArray(missStr.c_str(), 1);
		ref<Expr> read = Expr::createTempRead(array, 8);
		/* add newly generate symbolic cache miss variable to the symbolic variable log */
		symCacheMissLog.push_back(read);
		missVars.push_back(array);

		/* add in the sequence->variable map to compare with hit/miss sequence */
		symCacheMissMap[II] = read;

		/* set and reset of symbolic cache conflict variable */
		ref<Expr> zeroConflict = EqExpr::create(read, ConstantExpr::alloc(0, read->getWidth()));
		ref<Expr> oneConflict = EqExpr::create(read, ConstantExpr::alloc(1, read->getWidth()));

		ref<Expr> notPremise = NotExpr::create(premise);
		ref<Expr> evictCnstr = Expr::createImplies(premise, oneConflict);
		ref<Expr> notEvictCnstr = Expr::createImplies(notPremise, zeroConflict);

		/* <conflict_constraint_log>: add conflict constraints into a global log */
		pairT conflictCnstrLRU(evictCnstr, notEvictCnstr);
		conflictConstraintLogLRU[II] = conflictCnstrLRU;

		/* sudiptac: for checking sequence-based cache attacks */
		pairT seqCnstrLRU(premise, notPremise);
		seqConstraintLogLRU[II] = seqCnstrLRU;
	}

	fprintf(stdout, "\n\n......End processing constraints for set-associative LRU caches......\n\n");
}

