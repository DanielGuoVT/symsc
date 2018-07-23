/*
 * CacheSideChannel.h
 *
 *  Created on: Jun 14, 2017
 *      Author: sjguo
 */

#ifndef CACHESIDECHANNEL_H_
#define CACHESIDECHANNEL_H_

#include <klee/Expr.h>
#include <klee/ExecutionState.h>
using namespace klee;

#include <vector>
#include <map>
using namespace std;

/* Cache parameters. */
#define BLOCK_SIZE 32
/* Cache parameters end. */

typedef std::vector<ref<Expr> > addrT;
typedef std::pair<ref<Expr>, ref<Expr> > pairT;
typedef std::map<pairT, ref<Expr> > hashT;
typedef std::map<unsigned, pairT> logT;
typedef std::map<ref<Expr>, ref<Expr> > cacheT;
typedef std::map<unsigned, addrT> seqT;
typedef std::map<unsigned, std::vector<pairT> > seqPairT;

class CacheChannel {

private:
	// useful data structures to reduce memory consumption when building constraints
	static hashT tagPairCache;
	static hashT setPairCache;
	static cacheT tagCache;
	static cacheT setCache;

	// process the cold miss
	static void processColdMissCnstr(ExecutionState& state, TimingSolver *solver);
	// process the conflict miss
	static void processConflictMissCnstr(ExecutionState& state, TimingSolver *solver);
	// encodes reload constraints
	static ref<Expr> generateCnstrForInterReload(ExecutionState& state, int hStart, int hEnd, int target, bool& reload);
	// special handling for seq-based attack
	static void processColdMissCnstrForSeqAttack(ExecutionState& state, TimingSolver* solver);
	// encodes never-accessed constraints
	static ref<Expr> generateNeverBeforeCnstr(ExecutionState& state, TimingSolver* solver, unsigned II);

	/** routines for the generation of symbolic cache conflict count
	 * 	suffix "CC" stands for <constant,constant> address pairs
	 * 	suffix "SC" stands for <symbolic,constant> or <constant,symbolic> address pairs
	 * 	suffix "SS" stands for <symbolic,symbolic> address pairs
	 */
	static bool generateSetCnstrCC(ref<Expr>& addr1, ref<Expr>& addr2);
	static bool generateTagCnstrCC(ref<Expr>& addr1, ref<Expr>& addr2);
	static ref<Expr> generateSetCnstrSC(ref<Expr>& addr1, ref<Expr>& addr2);
	static ref<Expr> generateTagCnstrSC(ref<Expr>& addr1, ref<Expr>& addr2);
	static ref<Expr> generateSetCnstrSS(ref<Expr>& addr1, ref<Expr>& addr2);
	static ref<Expr> generateTagCnstrSS(ref<Expr>& addr1, ref<Expr>& addr2);

	// for logging constraints related to only set-associative caches
	static void logSetAssociativeCnstr(ref<Expr> constraint, unsigned accessID, bool reload = false);

public:
	// encodes an enumeration type for the observer model
	typedef enum {
		MISS_COUNT, MISS_SEQUENCE
	} Observer_t;
	static Observer_t observer;

	typedef std::map<ExecutionState, long> stateT;

	// encodes cache-miss constraints in terms of count
	static ref<Expr> processMissCountCnstr(int nmiss);
	static void processMissSequenceCnstr();

	// solve cache miss constraints
	static int solveAllConstraints(ExecutionState& state, TimingSolver* solver, ref<Expr>& obsCnstr, long bound = -1);

	// get all tests, each of which generates a unique side-channel observation
	static int getAllTests(ExecutionState& state, TimingSolver* solver, long bound);
	static void getNextTestForObsCount(ExecutionState& state, ExecutionState& cState, std::vector<std::vector<unsigned char> >& values);
	static void getNextTestForObsSeq(ExecutionState& state, ExecutionState& cState, std::vector<std::vector<unsigned char> >& values);

	// implements checkers to detect different types of attacks (1) timing-based attack and (2) seq-based
	// attack. Both checkers call the solver iteratively to get the amount of information leak
	static void checkForSeqAttack(ExecutionState& state, TimingSolver* solver);
	static void checkForTimingAttack(ExecutionState& state, TimingSolver* solver);

	// simple sanity checker to implement fast path (no solver calls)
	static bool sanityCheck(ExecutionState& state, unsigned long fixedMisses, unsigned long obsMisses);

	/* ##### list all tricks and heuristics to discover all solultions here ##### */

	// naive CEGAR base approach
	static void naiveCEGAR(ExecutionState& state, ExecutionState& cState, std::vector<std::vector<unsigned char> >& values);

	/* ##### tricks and heuristics end ##### */

	// return log of a number to the base 2
	static int log_base2(int n) {
		int power = 0;
		if (n <= 0 || (n & (n - 1)) != 0)
		assert(0 && "log2() only works for positive power of two values");
		while (n >>= 1)
			power++;
		return power;
	}

public:
	// holds concrete hit/miss sequence
	static std::vector<bool> cMissLog;

	// logging and processing memory addresses
	static void logMemoryAddr(ExecutionState& state, ref<Expr> address);
	static void printMemoryAddr(ExecutionState& state);
	static void printCacheConstraints(ExecutionState& state);
	static void processMemoryAddr(ExecutionState& state, TimingSolver* solver);

	// logging and processing hit/miss sequence (concrete case)
	static void readConcreteHitMissSequence();

	// cache related parameters
	static int nset;
	static int line;
	static int nassoc;
	static char policy[8];
	static unsigned long observedCacheMiss;
	// end cache parameters

	// fixed number of cold misses (independent of input)
	static int fixedColdMisses;
	// fixed number of conflict misses (independent of input)
	static unsigned long fixedConflictMisses;
	// number of atomic constraints in the constraint system
	static unsigned long atomicCnstr;

	// stores all generated constraints in the constraint log
	// sudiptac: use the tag <Cache_Constraint_System> whenever the following log is modified
	static addrT allConstraintLog;
	static logT conflictConstraintLog;
	static logT seqConstraintLog;
	static logT missConstraintLog;
	static logT coldSeqConstraintLog;

	// for set-associative caches
	static logT conflictConstraintLogLRU; /* for LRU replacement policy */
	static logT conflictConstraintLogFIFO; /* for FIFO replacement policy */

	// holds all symbolic variables created to capture cache misses
	static addrT symCacheMissLog;
	static std::map<unsigned, ref<Expr> > symCacheMissMap;

	// data structures related to set-associative caches
	static std::map<unsigned, ref<Expr> > setAssocReloadCnstrLog;
	static seqT setAssocCnstrLog;
	static seqPairT setAssocConflictCnstrLog;

	// hold a log for symbolic hit/miss variables
	static std::vector<const Array *> missVars;
};

// derived class for set-associative caches with LRU replacement policy
class CacheChannelLRU: public CacheChannel {
public:
	static logT seqConstraintLogLRU;
	static void processLRUConflictMissCnstr(ExecutionState& state, TimingSolver* solver);
};

// derived class for set-associative caches with LRU replacement policy
class CacheChannelFIFO: public CacheChannel {
public:
	static logT seqConstraintLogFIFO;
	static void processFIFOConflictMissCnstr(unsigned accessID, ExecutionState& state, TimingSolver* solver);
};

// driver class implementing cache-side-channel attack related routines
class CacheDriver: public CacheChannel {

public:
	static std::map<unsigned long, ref<Expr> > missToState;
	static std::map<std::pair<unsigned long, bool>, ref<Expr> > missSeqToState;

public:
	static void checkForTimingAttack(ExecutionState& state, TimingSolver* solver);
	static void checkForSeqAttack(ExecutionState& state, TimingSolver* solver);
	static void checkForAttackAndLeak(ExecutionState& state, TimingSolver* solver);
	static void addConflictConstraints(ExecutionState& state, logT& constraintLog);
	static void checkForBitLeakFromMissCount(ExecutionState& state, TimingSolver* solver);
	static void checkForByteLeakFromMissCount(ExecutionState& state, TimingSolver* solver);
	static void checkForBitLeakFromSequence(ExecutionState& state, TimingSolver* solver, logT& constraintLog);
	static void checkForByteLeakFromSequence(ExecutionState& state, TimingSolver* solver, logT& constraintLog);
	static ref<Expr> constrainOneBit(ExecutionState& state, int nbit, bool trueorfalse);
	static ref<Expr> constrainOneByte(ExecutionState& state, int nbyte, int value);
	static void checkForByteLeakFromPathConstraints(ExecutionState& state, TimingSolver* solver);
	static void checkForByteLeakSeqFromPathConstraints(ExecutionState& state, TimingSolver* solver);
};

// driver class implementing test generation related routines
class CacheTestDriver: public CacheChannel {

public:
	static void generateTestBasedOnMissCount(ExecutionState& state, TimingSolver* solver);
	static void generateTestBasedOnMissSequence(ExecutionState& state, TimingSolver* solver, logT& cnstrLog);
	static void generateAllTestBasedOnObserverModel(ExecutionState& state, TimingSolver* solver);
};

#endif /* CACHESIDECHANNEL_H_ */
