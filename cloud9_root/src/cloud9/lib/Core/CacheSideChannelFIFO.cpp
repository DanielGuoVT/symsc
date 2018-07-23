/*
 * CacheSideChannelFIFO.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: sjguo
 */

#include <stdio.h>
#include "CacheSideChannel.h"

logT CacheChannelFIFO::seqConstraintLogFIFO;

void CacheChannelFIFO::processFIFOConflictMissCnstr(unsigned II, ExecutionState& state, TimingSolver* solver) {

	fprintf(stdout, "\n\n......Start processing constraints for set-associative FIFO caches......\n\n");

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
		/* log symbolic miss constraints */
		std::pair<ref<Expr>, ref<Expr> > cnstr(NULL, NULL);
		missConstraintLog[II] = cnstr;
		fprintf(stdout, "Cold cache miss logged in FIFO\n");
		fflush(stdout);
		return;
	}

	if (setAssocReloadCnstrLog.count(II))
		premise = (premise.isNull()) ? setAssocReloadCnstrLog[II] : OrExpr::create(premise, setAssocReloadCnstrLog[II]);

	if (premise.isNull()) {
		return;
	}

	/* create symbolic cache conflict variable */
	std::ostringstream ss;
	std::string missStr("conflictFIFO");
	ss << II;
	missStr += ss.str();
	const Array* array = Array::CreateArray(missStr.c_str(), 1);
	ref<Expr> read = Expr::createTempRead(array, 8);
	/* add newly generated symbolic cache miss variable to the symbolic variable log */
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
	pairT conflictCnstrFIFO(evictCnstr, notEvictCnstr);
	conflictConstraintLogFIFO[II] = conflictCnstrFIFO;

	/* sudiptac: for checking sequence-based cache attacks */
	pairT seqCnstrFIFO(premise, notPremise);
	seqConstraintLogFIFO[II] = seqCnstrFIFO;

	/* log symbolic miss constraints */
	std::pair<ref<Expr>, ref<Expr> > cnstr(zeroConflict, oneConflict);
	missConstraintLog[II] = cnstr;

	fprintf(stdout, "\n\n......End processing constraints for set-associative FIFO caches......\n\n");
}

