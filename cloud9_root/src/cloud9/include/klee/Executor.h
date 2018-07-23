//===-- Executor.h ----------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Class to perform actual execution, hides implementation details from external
// interpreter.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_EXECUTOR_H
#define KLEE_EXECUTOR_H

#include "klee/ExecutionState.h"
#include "klee/Interpreter.h"
#include "klee/Expr.h"
#include "klee/ForkTag.h"
#include "klee/util/Ref.h"
#include "klee/Internal/Module/Cell.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/KModule.h"
#include "llvm/Support/CallSite.h"
#include "cloud9/worker/SymbolicEngine.h"
#include <vector>
#include <string>
#include <map>
#include <set>

//add by shengjianguo
#include "llvm/Support/CommandLine.h"
#include "klee/data/ExprSerializer.h"
#include "klee/data/ExprDeserializer.h"

struct KTest;

namespace llvm {
class BasicBlock;
class BranchInst;
class CallInst;
class Constant;
class ConstantExpr;
class Function;
class GlobalValue;
class Instruction;
class TargetData;
class Twine;
class Value;
}

namespace klee {
class Array;
struct Cell;
class ExecutionState;
class ExternalDispatcher;
class Expr;
class ExprSerializer;
class InstructionInfoTable;
struct KFunction;
struct KInstruction;
class KInstIterator;
class KModule;
class MemoryManager;
class MemoryObject;
class ObjectState;
class PTree;
class Searcher;
class SeedInfo;
class SpecialFunctionHandler;
struct StackFrame;
class StatsTracker;
class TimingSolver;
class TreeStreamWriter;
class WP;

/// \todo Add a context object to keep track of data only live
/// during an instruction step. Should contain addedStates,
/// removedStates, and haltExecution, among others.

class Executor: public Interpreter, public ::cloud9::worker::SymbolicEngine {
	friend class BumpMergingSearcher;
	friend class MergingSearcher;
	friend class RandomPathSearcher;
	friend class OwningSearcher;
	friend class WeightedRandomSearcher;
	friend class SpecialFunctionHandler;
	friend class StatsTracker;
	friend class MemoryObject;
	friend class ObjectState;

public:
	class Timer {
	public:
		Timer();
		virtual ~Timer();

		/// The event callback.
		virtual void run() = 0;
	};

	typedef std::pair<ExecutionState*, ExecutionState*> StatePair;

private:
	class TimerInfo;

	KModule *kmodule;
	InterpreterHandler *interpreterHandler;
	Searcher *searcher;

	ExternalDispatcher *externalDispatcher;
	TimingSolver *solver;
	MemoryManager *memory;
	std::set<ExecutionState*> states;
	StatsTracker *statsTracker;

private:
	TreeStreamWriter *pathWriter, *symPathWriter;
public:
	SpecialFunctionHandler *specialFunctionHandler;
private:
	std::vector<TimerInfo*> timers;
	PTree *processTree;
	ExecutionState *activeState;

/// Used to track states that have been added during the current
/// instructions step.
/// \invariant \ref addedStates is a subset of \ref states.
/// \invariant \ref addedStates and \ref removedStates are disjoint.
	std::set<ExecutionState*> addedStates;
	/// Used to track states that have been removed during the current
	/// instructions step.
	/// \invariant \ref removedStates is a subset of \ref states.
	/// \invariant \ref addedStates and \ref removedStates are disjoint.
	std::set<ExecutionState*> removedStates;

	/// Map of globals to their representative memory object.
	std::map<const llvm::GlobalValue*, MemoryObject*> globalObjects;

	/// Map of globals to their bound address. This also includes
	/// globals that have no representative object (i.e. functions).
	std::map<const llvm::GlobalValue*, klee::ref<ConstantExpr> > globalAddresses;

	/// The set of legal function addresses, used to validate function
	/// pointers. We use the actual Function* address as the function address.
	std::set<uint64_t> legalFunctions;

	/// Disables forking, instead a random path is chosen. Enabled as
	/// needed to control memory usage. \see fork()
	bool atMemoryLimit;

	/// Disables forking, set by client. \see setInhibitForking()
	bool inhibitForking;

	/// Signals the executor to halt execution at the next instruction
	/// step.
	bool haltExecution;

	/// Whether implied-value concretization is enabled. Currently
	/// false, it is buggy (it needs to validate its writes).
	bool ivcEnabled;

	/// The maximum time to allow for a single stp query.
	double stpTimeout;
	double instrTime;

	llvm::Function* getCalledFunction(llvm::CallSite &cs, ExecutionState &state);

	void executeInstruction(ExecutionState &state, KInstruction *ki);

	void printFileLine(ExecutionState &state, KInstruction *ki);

	void run(ExecutionState &initialState);

	// Given a concrete object in our [klee's] address space, add it to
	// objects checked code can reference.
	MemoryObject *addExternalObject(ExecutionState &state, void *addr, unsigned size, bool isReadOnly);

	void initializeGlobalObject(ExecutionState &state, ObjectState *os, const llvm::Constant *c, unsigned offset, MemoryObject* mo = 0);
	void initializeGlobals(ExecutionState &state);

	void stepInstruction(ExecutionState &state);
	void updateStates(ExecutionState *current);
	void finalizeRemovedStates();

	void transferToBasicBlock(llvm::BasicBlock *dst, llvm::BasicBlock *src, ExecutionState &state);

public:
	void callExternalFunction(ExecutionState &state, KInstruction *target, llvm::Function *function, std::vector<klee::ref<Expr> > &arguments);

private:
	void callUnmodelledFunction(ExecutionState &state, KInstruction *target, llvm::Function *function, std::vector<klee::ref<Expr> > &arguments);

public:
	ObjectState *bindObjectInState(ExecutionState &state, const MemoryObject *mo, bool isLocal, const Array *array = 0);

private:
	/// Resolve a pointer to the memory objects it could point to the
	/// start of, forking execution when necessary and generating errors
	/// for pointers to invalid locations (either out of bounds or
	/// address inside the middle of objects).
	///
	/// \param results[out] A list of ((MemoryObject,ObjectState),
	/// state) pairs for each object the given address can point to the
	/// beginning of.
	typedef std::vector<std::pair<std::pair<const MemoryObject*, const ObjectState*>, ExecutionState*> > ExactResolutionList;

	void resolveExact(ExecutionState &state, klee::ref<Expr> p, ExactResolutionList &results, const std::string &name);

	/// Allocate and bind a new object in a particular state. NOTE: This
	/// function may fork.
	///
	/// \param isLocal Flag to indicate if the object should be
	/// automatically deallocated on function return (this also makes it
	/// illegal to free directly).
	///
	/// \param target Value at which to bind the base address of the new
	/// object.
	///
	/// \param reallocFrom If non-zero and the allocation succeeds,
	/// initialize the new object from the given one and unbind it when
	/// done (realloc semantics). The initialized bytes will be the
	/// minimum of the size of the old and new objects, with remaining
	/// bytes initialized as specified by zeroMemory.
	void executeAlloc(ExecutionState &state, klee::ref<Expr> size, bool isLocal, KInstruction *target, bool zeroMemory = false,
			const ObjectState *reallocFrom = 0, bool dynAllocated = false);

	/// Free the given address with checking for errors. If target is
	/// given it will be bound to 0 in the resulting states (this is a
	/// convenience for realloc). Note that this function can cause the
	/// state to fork and that \ref state cannot be safely accessed
	/// afterwards.
	void executeFree(ExecutionState &state, klee::ref<Expr> address, KInstruction *target = 0);

	void executeCall(ExecutionState &state, KInstruction *ki, llvm::Function *f, std::vector<klee::ref<Expr> > &arguments);

public:
	// do address resolution / object binding / out of bounds checking
	// and perform the operation
	void executeMemoryOperation(ExecutionState &state, bool isWrite, klee::ref<Expr> address, klee::ref<Expr> value /* undef if read */,
			KInstruction *target /* undef if write */, bool usc = false);

private:
	void executeMakeSymbolic(ExecutionState &state, const MemoryObject *mo, bool shared = false);

	void executeEvent(ExecutionState &state, unsigned int type, long int value);

	/// Create a new state where each input condition has been added as
	/// a constraint and return the results. The input state is included
	/// as one of the results. Note that the output vector may included
	/// NULL pointers for states which were unable to be created.
	void branch(ExecutionState &state, klee::ref<Expr> condition, const std::vector<std::pair<BasicBlock*, klee::ref<Expr> > > &options,
			std::vector<std::pair<BasicBlock*, ExecutionState*> > &branches, int reason, bool wp = false);

	// Fork current and return states in which condition holds / does
	// not hold, respectively. One of the states is necessarily the
	// current state, and one of the states may be null.
	StatePair fork(ExecutionState &current, klee::ref<Expr> condition, bool isInternal, int reason, bool flag = false, bool hasSummary = false);
public:
	StatePair fork(ExecutionState &current, int reason);
private:
	ForkTag getForkTag(ExecutionState &current, int reason);

	/// Add the given (boolean) condition as a constraint on state. This
	/// function is a wrapper around the state's addConstraint function
	/// which also manages manages propogation of implied values,
	/// validity checks, and seed patching.
	void addConstraint(ExecutionState &state, klee::ref<Expr> condition);

	// Called on [for now] concrete reads, replaces constant with a symbolic
	// Used for testing.
	klee::ref<Expr> replaceReadWithSymbolic(ExecutionState &state, klee::ref<Expr> e);

public:
	const Cell& eval(KInstruction *ki, unsigned index, ExecutionState &state) const;

private:
	Cell& getArgumentCell(ExecutionState &state, KFunction *kf, unsigned index) {
		return state.stack().back().locals[kf->getArgRegister(index)];
	}

	Cell& getArgumentCell(StackFrame &sf, KFunction *kf, unsigned index) {
		return sf.locals[kf->getArgRegister(index)];
	}

public:
	Cell& getDestCell(ExecutionState &state, KInstruction *target) {
		return state.stack().back().locals[target->dest];
	}

	void bindLocal(KInstruction *target, ExecutionState &state, klee::ref<Expr> value, unsigned instID = -1);

public:
	void bindArgument(KFunction *kf, unsigned index, ExecutionState &state, klee::ref<Expr> value);

private:
	klee::ref<klee::ConstantExpr> evalConstantExpr(const llvm::ConstantExpr *ce);

	/// Return a unique constant value for the given expression in the
	/// given state, if it has one (i.e. it provably only has a single
	/// value). Otherwise return the original expression.
	klee::ref<Expr> toUnique(const ExecutionState &state, klee::ref<Expr> &e);

public:
	/// Return a constant value for the given expression, forcing it to
	/// be constant in the given state by adding a constraint if
	/// necessary. Note that this function breaks completeness and
	/// should generally be avoided.
	///
	/// \param purpose An identify string to printed in case of concretization.
	klee::ref<klee::ConstantExpr> toConstant(ExecutionState &state, klee::ref<Expr> e, const char *purpose);

private:
	/// Bind a constant value for e to the given target. NOTE: This
	/// function may fork state if the state has multiple seeds.
	void executeGetValue(ExecutionState &state, klee::ref<Expr> e, KInstruction *target);

	/// Get textual information regarding a memory address.
	std::string getAddressInfo(ExecutionState &state, klee::ref<Expr> address) const;

	// remove state from queue and delete
	bool terminateState(ExecutionState &state, bool silenced);
	// call exit handler and terminate state
	void terminateStateEarly(ExecutionState &state, const llvm::Twine &message);

public:
	// call exit handler and terminate state
	void terminateStateOnExit(ExecutionState &state);
	// call error handler and terminate state
	void terminateStateOnError(ExecutionState &state, const llvm::Twine &message, const char *suffix, const llvm::Twine &longMessage = "");
	// call error handler and terminate state, for execution errors
	// (things that should not be possible, like illegal instruction or
	// unlowered instrinsic, or are unsupported, like inline assembly)
	void terminateStateOnExecError(ExecutionState &state, const llvm::Twine &message, const llvm::Twine &info = "") {
		terminateStateOnError(state, message, "exec.err", info);
	}

private:
	/// bindModuleConstants - Initialize the module constant table.
	void bindModuleConstants();

	template<typename TypeIt>
	void computeOffsets(KGEPInstruction *kgepi, TypeIt ib, TypeIt ie);

	/// bindInstructionConstants - Initialize any necessary per instruction
	/// constant values.
	void bindInstructionConstants(KInstruction *KI);

	void handlePointsToObj(ExecutionState &state, KInstruction *target, const std::vector<klee::ref<Expr> > &arguments);

	void doImpliedValueConcretization(ExecutionState &state, klee::ref<Expr> e, klee::ref<ConstantExpr> value);

	/// Add a timer to be executed periodically.
	///
	/// \param timer The timer object to run on firings.
	/// \param rate The approximate delay (in seconds) between firings.
	void addTimer(Timer *timer, double rate);

	void initTimers();
	void processTimers(ExecutionState *current);
	void resetTimers();

	/// Pthread create needs a specific StackFrame instead of the one of the current state
	void bindArgumentToPthreadCreate(KFunction *kf, unsigned index, StackFrame &sf, klee::ref<Expr> value);

	/// Finds the functions coresponding to an address.
	/// For now, it only support concrete values for the thread and function pointer argument.
	/// Can be extended easily to take care of symbolic function pointers.
	/// \param address address of the function pointer
	KFunction* resolveFunction(klee::ref<Expr> address);

	//pthread handlers
	void executeThreadCreate(ExecutionState &state, thread_id_t tid, klee::ref<Expr> start_function, klee::ref<Expr> arg);

	// Daniel, set the thread priority
	void executeThreadPriority(ExecutionState &state, thread_id_t tid, thread_priority_t priority);

	void executeThreadExit(ExecutionState &state);

	void executeProcessExit(ExecutionState &state);

	void executeProcessFork(ExecutionState &state, KInstruction *ki, process_id_t pid);

	bool schedule(ExecutionState &state, bool yield, int specialReason = 3);

	void executeThreadNotifyOne(ExecutionState &state, wlist_id_t wlist);

	void executeFork(ExecutionState &state, KInstruction *ki, uint64_t reason);

public:
	Executor(const InterpreterOptions &opts, InterpreterHandler *ie);
	virtual ~Executor();

	const InterpreterHandler& getHandler() {
		return *interpreterHandler;
	}

	// XXX should just be moved out to utility module
	klee::ref<ConstantExpr> evalConstant(const llvm::Constant *c);

	virtual void setPathWriter(TreeStreamWriter *tsw) {
		pathWriter = tsw;
	}
	virtual void setSymbolicPathWriter(TreeStreamWriter *tsw) {
		symPathWriter = tsw;
	}

	virtual const llvm::Module *
	setModule(llvm::Module *module, const ModuleOptions &opts);

	const KModule* getKModule() const {
		return kmodule;
	}

	virtual void runFunctionAsMain(llvm::Function *f, int argc, char **argv, char **envp);

	/*** Runtime options ***/

	virtual void setHaltExecution(bool value) {
		haltExecution = value;
	}

	virtual void setInhibitForking(bool value) {
		inhibitForking = value;
	}

	/*** State accessor methods ***/

	virtual unsigned getPathStreamID(const ExecutionState &state);

	virtual unsigned getSymbolicPathStreamID(const ExecutionState &state);

	virtual void getConstraintLog(const ExecutionState &state, std::string &res, bool asCVC = false);

	virtual bool getSymbolicSolution(ExecutionState &state, std::vector<std::pair<std::string, std::vector<unsigned char> > > &res);

	Expr::Width getWidthForLLVMType(llvm::Type *type) const;

	void PrintDump(std::ostream &os);
	void PrintDumpOnErrorSignal();

	/*** Cloud9 symbolic execution engine methods ***/

	virtual ExecutionState *createRootState(llvm::Function *f);
	virtual void initRootState(ExecutionState *state, int argc, char **argv, char **envp);

	virtual void stepInState(ExecutionState *state);

	virtual void destroyStates();

	virtual void destroyState(klee::ExecutionState *state);

	virtual klee::Searcher *initSearcher(klee::Searcher *base);

	virtual klee::KModule *getModule() {
		return kmodule;
	}

	//Hack for dynamic cast in CoreStrategies, TODO Solve it as soon as possible
	static bool classof(const SymbolicEngine* engine) {
		return true;
	}

	void sleepThread(ExecutionState& state, thread_uid_t tuid, wlist_id_t wlist);
	void terminateRedundantState(ExecutionState &state, bool silenced = true);

public:
	bool UseDPOR;
	bool DEBUG;
	bool UsePriorityPreemption;
	bool CheckJobPeriod;

	//for CS & DPOR, sjguo
	enum PivotPointType {
		bPP = 1, iPP = 2
	};
	std::set<std::string> ignoredVariables;
	std::set<std::string> global_rw_white_list;
	std::set<std::string> white_list_functions; // record the direct instruction in these functions
	std::set<std::string> shadow_functions;

	//for DPOR, sjguo
	typedef boost::unordered_map<std::string, std::set<thread_uid_t> > Backtrack_set;
	typedef boost::unordered_map<std::string, std::set<thread_uid_t> > Done_set;
	Backtrack_set backtrack_set;
	Done_set done_set;
	std::map<std::string, ExecutionState*> POR_states;

	std::string generateIPP(ExecutionState &state);
	void backupStateOnIPP(ExecutionState &state);
	void addTransitionEvent(ExecutionState &state, int transition_type, uint64_t memoryAddr, std::string name = "");
	std::string generatePrefix(ExecutionState &state);
	void updateBacktrackSet(ExecutionState &state);
	void updateDoneSet(ExecutionState &state);
	void updateStateBeforeSchedule(ExecutionState &state, KInstruction* ki, uint64_t lastTransitonType = 0, uint64_t lastTransitonAddr = 0);

	void printInstructionInfo(Instruction* inst);

	/// Functions for wp computation, sjguo
	void updateTraceLines(KInstruction *ki, ExecutionState &state);
	int getMOID(ExecutionState &state, klee::ref<Expr> address);
	klee::ref<Expr> getOffset(ExecutionState &state, klee::ref<Expr> address);
	MemoryManager *getMemoryManager() {
		return memory;
	}

	// use the runtime memory value to update the wp
	void concretizeWP(ExecutionState &state, klee::ref<Expr> &wp);
	klee::ref<Expr> loadMemoryObject(ExecutionState &state, klee::ref<Expr> address, Expr::Width type);

	// for tapPLC, sjguo
	bool isTraceFeasible(ExecutionState &state);
	bool isStateVisited(ExecutionState &state);
	void printTrace(ExecutionState &state);
	void concretizePLCStateExpr(std::string key, klee::ref<Expr> value, klee::ref<Expr> visitedStates);
	bool isStateContained(ExecutionState&state, std::map<std::string, ref<Expr> >& memMap);
	void parseGlobalInfo(ExecutionState& state, Module::const_global_iterator& global, MemoryObject* mo);
	void parseGlobalStructInfo(ExecutionState& state, StructType* st, string name, uint64_t baseAddr);
	void replaceSolvedSymbolics(ref<Expr>& expr, std::map<std::string, ref<Expr> >& solved);

	enum ScheduleReason {
		PrioritySharedVariableAccess = 1, JobBoundary = 2, RegularSharedVariableAccess = 3
	};

	std::set<std::string> cov_functions;
	std::set<std::string> sc_functions;

	// for tapPLC, sjguo
	unsigned crtScan;
	bool UseTapPLC;
	bool UsePLCStateCheck;
	bool UseNoOutput;
	typedef std::map<std::string, std::pair<uint64_t, unsigned> > GlobalVarMap;
	GlobalVarMap globalStatefulVars;

	// name-value map of a state
	std::vector<std::map<std::string, ref<Expr> > > visitedStates;
	std::set<llvm::StringRef> iec_basic_types;

	// for tapsc, sjguo

public:
	ExprSerializer *exprRecorder;
	ExprBuilder *builder;
	ExprDeserializer *exprDeserializer;

	/* begin tapsc variables and functions*/
private:
	// borrowed from sudiptac's Chalice code
	static int log_base2(int n);

	map<std::pair<klee::ref<Expr>, klee::ref<Expr> >, klee::ref<Expr> > setPairCache;
	map<klee::ref<Expr>, bool> cmpFalseCache;
	map<std::pair<ref<Expr>, ref<Expr> >, ref<Expr> > tagPairCache;
	std::map<ref<Expr>, ref<Expr> > setCache;
	std::map<ref<Expr>, ref<Expr> > tagCache;
	std::map<std::string, std::string> cacheBehavInputMap;

	ref<Expr> createCacheSetCompareConstraintCC(ref<Expr> &addr1, ref<Expr> &addr2);
	ref<Expr> createCacheTagCompareConstraintCC(ref<Expr> &addr1, ref<Expr> &addr2);

	ref<Expr> createCacheSetCompareConstraintSC(ref<Expr> &addr1, ref<Expr> &addr2);
	ref<Expr> createCacheTagCompareConstraintSC(ref<Expr> &addr1, ref<Expr> &addr2);

	ref<Expr> createCacheSetCompareConstraintSS(ref<Expr> &addr1, ref<Expr> &addr2);
	ref<Expr> createCacheTagCompareConstraintSS(ref<Expr> &addr1, ref<Expr> &addr2);
public:
	unsigned getCacheSet(uint64_t address);
	ref<Expr> getCacheSet(ref<Expr> address);

	unsigned getCacheTag(uint64_t address);
	ref<Expr> getCacheTag(ref<Expr> address);

	ref<Expr> createCacheSetCompareConstraint(ref<Expr> &addr1, ref<Expr> &addr2);
	ref<Expr> createCacheTagCompareConstraint(ref<Expr> &addr1, ref<Expr> &addr2);

	void computeIndividualConstraint(ExecutionState &state, TimingSolver *solver, int idx, ref<Expr> addr);

	void processCacheAccessTwoStep(ExecutionState &state, TimingSolver *solver);
	void processCacheAccessPrecise(ExecutionState &state, TimingSolver *solver);

	ref<Expr> createNotReloadConstraint(unsigned crtIdx, ref<Expr> &target, ExecutionState &state);
	void previousMethod(ExecutionState &state);

	int computeSolution(ExecutionState &state, TimingSolver *solver, bool identical = false);
	int computeSolutionTwoStep(ExecutionState &state, TimingSolver *solver, unsigned begin, unsigned end, bool identical = false);
	int computeSolutionPrecise(ExecutionState &state, TimingSolver *solver, unsigned begin, unsigned end, bool identical = false);

	/* begin tapsc variables */
	bool USC;
	long stateCounter;
	static unsigned cacheSetNum;
	static unsigned nway;
	static unsigned cacheLineSize;
	std::map<std::string, uint64_t> scGlobalVars;
	std::map<std::string, MemoryObject *> sboxGlobalMOs;
	bool smallSboxes;
	unsigned maxMemAccs;
	bool regular;
	bool flog;
	bool fixed_exec;
	uint64_t fixed_addr;
	bool precise; // precise analysis
	bool twostep;
	unsigned inter;
	unsigned fail;
	unsigned test;
	/* end tapsc variables */
};

} // End klee namespace

#endif
