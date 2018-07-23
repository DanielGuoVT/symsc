//===-- Executor.cpp ------------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Executor.h"

#include "ExternalDispatcher.h"
#include "ImpliedValue.h"
#include "Memory.h"
#include "MemoryManager.h"
#include "PTree.h"
#include "SpecialFunctionHandler.h"
#include "StatsTracker.h"
#include "TimingSolver.h"
#include "UserSearcher.h"
#include "klee/CoreStats.h"
#include "klee/LoggingSolvers.h"
#include "klee/data/ExprSerializer.h"
#include "klee/data/ExprDeserializer.h"
#include "klee/ExprBuilder.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/util/ExprPPrinter.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Signals.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Support/CFG.h"
#include "llvm/IntrinsicInst.h"

#include <glog/logging.h>

#include <iomanip>
#include <string>
#include <sstream>

#include <boost/lexical_cast.hpp>

using namespace klee;
using namespace boost;

namespace {

cl::opt<bool> NoPreferCex("no-prefer-cex", cl::init(false));

cl::opt<bool> DebugPrintInstructions("debug-print-instructions", cl::desc("Print instructions during execution."));

cl::opt<bool> DebugCheckForImpliedValues("debug-check-for-implied-values");

cl::opt<bool> DebugValidateSolver("debug-validate-solver", cl::init(false));

cl::opt<bool> UseFastCexSolver("use-fast-cex-solver", cl::init(false));

cl::opt<bool> UseIndependentSolver("use-independent-solver", cl::init(true), cl::desc("Use constraint independence"));

cl::opt<bool> UseCexCache("use-cex-cache", cl::init(true), cl::desc("Use counterexample caching"));

cl::opt<bool> UseQueryPCLog("use-query-pc-log", cl::init(false));

cl::opt<bool> UseSTPQueryPCLog("use-stp-query-pc-log", cl::init(false));

cl::opt<bool> UseCache("use-cache", cl::init(true), cl::desc("Use validity caching"));

cl::opt<double> MaxInstructionTime("max-instruction-time", cl::desc("Only allow a single instruction to take this much time (default=0 (off))"),
		cl::init(0));

cl::opt<double> MaxSTPTime("max-stp-time", cl::desc("Maximum amount of time for a single query (default=120s)"), cl::init(120.0));

cl::opt<unsigned int> StopAfterNInstructions("stop-after-n-instructions", cl::desc("Stop execution after specified number of instructions (0=off)"),
		cl::init(0));

cl::opt<bool> UseForkedSTP("use-forked-stp", cl::desc("Run STP in forked process"), cl::init(false));

cl::opt<bool> STPOptimizeDivides("stp-optimize-divides", cl::desc("Optimize constant divides into add/shift/multiplies before passing to STP"),
		cl::init(true));

cl::opt<bool> UseBTORSolver("use-btor", cl::desc("Use BTOR solver"), cl::init(false));

///////////////////////////////////options from sjguo//////////////////////////////////

cl::opt<bool> DPOR("dpor", cl::desc("enable dynamic partial-order-reduction (default=disabled)"), cl::init(false));

cl::opt<bool> FLOG("flog", cl::desc("wirte running info to cerr.txt (default=disabled"), cl::init(false));

cl::opt<bool> RUI("rui", cl::desc("print runtime information to std::cerr (default=disabled"), cl::init(false));

cl::opt<bool> PRIORITY("prio", cl::desc("priority based preemption"), cl::init(false));

cl::opt<bool> PERIOD("period", cl::desc("enable the period-based checking for co-enabled events"), cl::init(false));

cl::opt<bool> PLCStateCheck("psc", cl::desc("use the plc state check"), cl::init(false));

cl::opt<bool> NoOutput("no-output", cl::desc("Don't generate test files"));

cl::opt<unsigned int> MaxIter("max", cl::desc("The iteration bound number"), cl::init(10));

cl::opt<bool> UseSideChannel("usc", cl::desc("enable the side channel analysis"), cl::init(false));

cl::opt<bool> FIXED("fixed", cl::desc("use a random memory address in thread T2 for tapsc analysis"), cl::init(false));

cl::opt<bool> PRECISE("precise", cl::desc("use the precise formula computation for tapsc analysis"), cl::init(false));

cl::opt<bool> TWOSTEP("twostep", cl::desc("use the two-step analysis for tapsc analysis"), cl::init(false));

cl::opt<bool> REGULAR("regular", cl::desc("just run and collect statistical data"), cl::init(false));
///////////////////////////////////end of the options////////////////////////////////////////////
}

//namespace klee {

unsigned Executor::cacheSetNum = 1024;
unsigned Executor::nway = 1;
unsigned Executor::cacheLineSize = 64; // in byte

Executor::Executor(const InterpreterOptions &opts, InterpreterHandler *ih) :
		Interpreter(opts), kmodule(0), interpreterHandler(ih), searcher(0), externalDispatcher(new ExternalDispatcher()), statsTracker(0), exprRecorder(
				0), pathWriter(0), symPathWriter(0), specialFunctionHandler(0), processTree(0), activeState(0), atMemoryLimit(false), inhibitForking(
				false), haltExecution(false), ivcEnabled(false), stpTimeout(
				MaxSTPTime != 0 && MaxInstructionTime != 0 ? std::min(MaxSTPTime, MaxInstructionTime) : std::max(MaxSTPTime, MaxInstructionTime)), instrTime(
				MaxInstructionTime) {

	STPSolver *stpSolver;
	Solver *baseSolver;

	stpSolver = new STPSolver(UseForkedSTP, STPOptimizeDivides);
	baseSolver = stpSolver;

	Solver *solver = baseSolver;

	if (UseSTPQueryPCLog) solver = createPCLoggingSolver(solver, interpreterHandler->getOutputFilename("stp-queries.qlog"));

	if (UseFastCexSolver) solver = createFastCexSolver(solver);

	if (UseCexCache) solver = createCexCachingSolver(solver);

	if (UseCache) solver = createCachingSolver(solver);

	if (UseIndependentSolver) solver = createIndependentSolver(solver);

	if (DebugValidateSolver) solver = createValidatingSolver(solver, baseSolver);

	if (UseQueryPCLog) solver = createPCLoggingSolver(solver, interpreterHandler->getOutputFilename("queries.pc"));

	UseDPOR = DPOR ? true : false;
	DEBUG = RUI ? true : false;

	UsePriorityPreemption = PRIORITY ? true : false;
	CheckJobPeriod = PERIOD ? true : false;

	UsePLCStateCheck = PLCStateCheck ? true : false;
	crtScan = 0;
	UseNoOutput = NoOutput ? true : false;

	USC = UseSideChannel ? true : false;
	smallSboxes = true;
	maxMemAccs = 0;
	fixed_exec = FIXED ? true : false;
	fixed_addr = 0;
	precise = PRECISE ? true : false;
	twostep = TWOSTEP ? true : false;
	regular = REGULAR ? true : false;
	flog = FLOG ? true : false;

	if (FLOG) {
		if (regular) {
			freopen("./log.regular", "w", stderr);
		} else if (precise) {
			if (fixed_exec) freopen("./log.precise-fixed", "w", stderr);
			else freopen("./log.precise-symbolic", "w", stderr);
		} else {
			if (fixed_exec) freopen("./log.twostep-fixed", "w", stderr);
			else freopen("./log.twostep-symbolic", "w", stderr);
		}
	}

	this->solver = new TimingSolver(solver, stpSolver, statsTracker);
	memory = new MemoryManager();

	PrintDumpOnErrorSignal();
}

const Module *Executor::setModule(llvm::Module *module, const ModuleOptions &opts) {
	assert(!kmodule && module && "can only register one module");
// XXX gross

	kmodule = new KModule(module);

// Initialize the context.
	TargetData *TD = kmodule->targetData;

	Context::initialize(TD->isLittleEndian(), (Expr::Width) TD->getPointerSizeInBits());

	specialFunctionHandler = new SpecialFunctionHandler(*this);

	specialFunctionHandler->prepare();

	kmodule->prepare(opts, interpreterHandler);

	specialFunctionHandler->bind();

	if (StatsTracker::useStatistics()) {
		statsTracker = new StatsTracker(*this, interpreterHandler->getOutputFilename("assembly.ll"), true);
		solver->statsTracker = statsTracker;
	}

	if (USC) {
		// default cache configuration
		cacheSetNum = 1024;
		nway = 1;
		cacheLineSize = 64; // in byte

		// initialize the statistical variables
		inter = 0;
		fail = 0;
		test = 0;

		// read the layout
		llvm::sys::Path dir(".");
		string path = dir.str() + "/layout";
		ifstream file;
		file.open(path.c_str());
		std::map<unsigned, unsigned> lineMap;
		if (file.fail()) {
			std::cerr << "[ERROR] Unable to read variable layout file !" << std::endl;
		} else {
			std::string line;
			while (std::getline(file, line)) {
				if (line.empty()) continue;

				string addr_hex = line.substr(9, 16);
				string name = line.substr(59, line.length() - 59);

				uint64_t addr;
				std::stringstream ss;
				ss << std::hex << addr_hex;
				ss >> addr;
				scGlobalVars[name] = addr;
			}
			file.close();
		}

		std::cerr << std::endl << "==============================Preprocessing=================================" << std::endl;

		path = dir.str() + "/config";
		file.open(path.c_str());
		std::string key_name;
		if (file.fail()) {
			std::cerr << "[ERROR] Unable to read config file, now exit." << std::endl;
			exit(0);
		} else {
			std::string line;
			unsigned section = 0;
			stringstream ss1;
			while (std::getline(file, line)) {
				if (line.empty()) continue;
				if (line == "[func]") {
					section = 1;
					continue;
				} else if (line == "[sbox]") {
					section = 2;
					continue;
				} else if (line == "[shadow]") {
					section = 3;
					continue;
				} else if (line == "[cache]") {
					section = 4;
					continue;
				} else if (line == "[key]") {
					section = 5;
					continue;
				}

				switch (section) {
				case 1:
					std::cerr << "add function: " << line << std::endl;
					white_list_functions.insert(line);
					break;
				case 2:
					std::cerr << "add sboxGlobalVars: " << line << std::endl;
					sboxGlobalMOs[line] = NULL;
					break;
				case 3:
					std::cerr << "add shadow function: " << line << std::endl;
					shadow_functions.insert(line);
					break;
				case 4:
					if (line.substr(0, 4) == "setN") {
						ss1 << line.substr(5, line.length() - 5);
						ss1 >> cacheSetNum;
						ss1.clear();
						std::cerr << "cacheSetN: " << cacheSetNum << std::endl;
					} else if (line.substr(0, 4) == "nway") {
						ss1 << line.substr(5, line.length() - 5);
						ss1 >> nway;
						ss1.clear();
						std::cerr << "nway: " << nway << std::endl;
					} else if (line.substr(0, 4) == "line") {
						ss1 << line.substr(5, line.length() - 5);
						ss1 >> cacheLineSize;
						ss1.clear();
						std::cerr << "cacheline: " << cacheLineSize << std::endl;
					}
					break;
				case 5:
					assert(!line.empty());
					key_name = line;
					break;
				default:
					break;
				}
			}
			file.close();
		}

		llvm::Module *M = getKModule()->module;
		unsigned memAccs = 0;
		for (Module::iterator F = M->begin(); F != M->end(); F++) {
			if (F->hasName()) {
				if (!white_list_functions.count(F->getName().str())) {
					continue;
				}
			}
			for (Function::iterator B = F->begin(); B != F->end(); B++) {
				for (BasicBlock::iterator I = B->begin(); I != B->end(); I++) {
					if (I->getOpcode() == Instruction::Load || I->getOpcode() == Instruction::Store) {
						memAccs++;
					}
				}
			}
		}
		std::cerr << "Static memory access number: " << memAccs << std::endl;
		if (regular) {
			freopen("./log.accs", "w", stderr);
			std::cerr << memAccs;

			for (Module::const_global_iterator i = module->global_begin(), e = module->global_end(); i != e; ++i) {
				std::string name = i->getName().str();
				if (name == key_name) {
					Type *ty = i->getType()->getElementType();
					uint64_t size = kmodule->targetData->getTypeStoreSize(ty);
					freopen("./log.ks", "w", stderr);
					std::cerr << size;
				}
			}
			exit(0);
		}
	}

	return module;
}

Executor::~Executor() {
	delete memory;
	delete externalDispatcher;
	if (processTree) {
		delete processTree;
	}
	if (specialFunctionHandler) {
		delete specialFunctionHandler;
	}
	if (statsTracker) {
		delete statsTracker;
	}
//	delete exprRecorder;
//	delete builder;
//	delete exprDeserializer;
	delete solver;
	delete kmodule;

	if (FLOG) {
		if (precise) {
			freopen("./log.Inter", "w", stderr);
			std::cerr << inter;
			freopen("./log.Test", "w", stderr);
			std::cerr << test;
		} else {
			freopen("./log.Inter", "w", stderr);
			std::cerr << inter;
			freopen("./log.Fail", "w", stderr);
			if (fail == 0 && test == 0 && inter > 0) {
				fail = inter;
			}
			std::cerr << fail;
			freopen("./log.Test", "w", stderr);
			std::cerr << test;
		}
		fclose(stderr);
	}
}

/***/

void Executor::stepInstruction(ExecutionState &state) {
	if (DebugPrintInstructions) {
		printFileLine(state, state.pc());
		std::cerr << std::setw(10) << stats::instructions << " ";
		llvm::errs() << *(state.pc()->inst);
	}

	if (statsTracker) statsTracker->stepInstruction(state);

	++state.totalInstructions;
	++stats::instructions;
	state.prevPrevPC() = state.prevPC();
	state.prevPC() = state.pc();
	++state.pc();

	if (stats::instructions == StopAfterNInstructions) haltExecution = true;
}

void Executor::printFileLine(ExecutionState &state, KInstruction *ki) {
	const InstructionInfo &ii = *ki->info;
	if (ii.file != "") std::cerr << "     " << ii.file << ":" << ii.line << ":";
	else std::cerr << "     [no debug info]:";
}

std::string Executor::getAddressInfo(ExecutionState &state, klee::ref<Expr> address) const {
	std::ostringstream info;
	info << "\taddress: " << address << "\n";
	uint64_t example;
	if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(address))) {
		example = CE->getZExtValue();
	} else {
		klee::ref<ConstantExpr> value;
		bool success = solver->getValue(data::OTHER, state, address, value);
		assert(success && "FIXME: Unhandled solver failure");
		(void) success;
		example = value->getZExtValue();
		info << "\texample: " << example << "\n";
		std::pair<klee::ref<Expr>, klee::ref<Expr> > res = solver->getRange(data::OTHER, state, address);
		info << "\trange: [" << res.first << ", " << res.second << "]\n";
	}

	MemoryObject hack((unsigned) example);
	MemoryMap::iterator lower = state.addressSpace().objects.upper_bound(&hack);
	info << "\tnext: ";
	if (lower == state.addressSpace().objects.end()) {
		info << "none\n";
	} else {
		const MemoryObject *mo = lower->first;
		std::string alloc_info;
		mo->getAllocInfo(alloc_info);
		info << "object at " << mo->address << " of size " << mo->size << "\n" << "\t\t" << alloc_info << "\n";
	}
	if (lower != state.addressSpace().objects.begin()) {
		--lower;
		info << "\tprev: ";
		if (lower == state.addressSpace().objects.end()) {
			info << "none\n";
		} else {
			const MemoryObject *mo = lower->first;
			std::string alloc_info;
			mo->getAllocInfo(alloc_info);
			info << "object at " << mo->address << " of size " << mo->size << "\n" << "\t\t" << alloc_info << "\n";
		}
	}

	return info.str();
}

Searcher *Executor::initSearcher(Searcher *base) {
	return constructUserSearcher(*this, base);
}

void Executor::runFunctionAsMain(Function *f, int argc, char **argv, char **envp) {

	ExecutionState *state = createRootState(f);
	initRootState(state, argc, argv, envp);

	run(*state);

	destroyStates();

}

unsigned Executor::getPathStreamID(const ExecutionState &state) {
	assert(pathWriter);
	return state.pathOS.getID();
}

unsigned Executor::getSymbolicPathStreamID(const ExecutionState &state) {
	assert(symPathWriter);
	return state.symPathOS.getID();
}

void Executor::getConstraintLog(const ExecutionState &state, std::string &res, bool asCVC) {
#if 0
	if (asCVC) {
		Query query(state.constraints(), ConstantExpr::alloc(0, Expr::Bool));
		char *log = solver->stpSolver->getConstraintLog(query);
		res = std::string(log);
		free(log);
	} else {
		std::ostringstream info;
		ExprPPrinter::printConstraints(info, state.constraints());
		res = info.str();
	}
#endif
}

void concretizeSymbolicExpr(klee::ref<Expr> &source, klee::ref<Expr> &query, klee::ref<Expr> concrete) {
	for (unsigned i = 0; i < source->getNumKids(); i++) {
		klee::ref<Expr> kid = source->getKid(i);
		if (kid->hash() == query->hash() && kid->getKind() == query->getKind()) {
			std::cerr << "find one" << std::endl;
			std::cerr << "kid: " << kid << std::endl;
			std::cerr << "query : " << query << std::endl;
			std::cerr << "hash: " << kid->hash() << std::endl;
			source->setKid(i, concrete);
		}
		concretizeSymbolicExpr(kid, query, concrete);
	}
}

bool Executor::getSymbolicSolution(ExecutionState &state, std::vector<std::pair<std::string, std::vector<unsigned char> > > &res) {
	solver->setTimeout(stpTimeout);
	if (this->DEBUG) {
		std::cerr << "[DBG]: getSymbolicSolution, state: " << &state << std::endl;
	}

	ExecutionState tmp(state);
	if (!NoPreferCex) {
		for (unsigned i = 0; i != state.symbolics.size(); ++i) {
			const MemoryObject *mo = state.symbolics[i].first;
			std::vector<klee::ref<Expr> >::const_iterator pi = mo->cexPreferences.begin(), pie = mo->cexPreferences.end();
			for (; pi != pie; ++pi) {
				bool mustBeTrue;
				bool success = solver->mustBeTrue(data::TEST_CASE_GENERATION, tmp, Expr::createIsZero(*pi), mustBeTrue);
				if (!success) break;
				if (!mustBeTrue) tmp.addConstraint(*pi);
			}

			if (pi != pie) break;
		}
	}

	std::vector<std::vector<unsigned char> > values;
	std::vector<const Array*> objects;
	for (unsigned i = 0; i != state.symbolics.size(); ++i) {
		objects.push_back(state.symbolics[i].second);
	}
	bool success = solver->getInitialValues(data::TEST_CASE_GENERATION, tmp, objects, values);
	solver->setTimeout(0);
	if (!success) {
		LOG(WARNING) << "Unable to compute initial values (invalid constraints?)!";
		ExprPPrinter::printQuery(std::cerr, state.constraints(), ConstantExpr::alloc(0, Expr::Bool), std::string(), std::string());
		return false;
	}
	for (unsigned i = 0; i != state.symbolics.size(); ++i) {
		res.push_back(std::make_pair(state.symbolics[i].first->name, values[i]));
	}

	return true;
}

void Executor::replaceSolvedSymbolics(ref<Expr>& expr, std::map<std::string, ref<Expr> >& solved) {
	if (expr->getKind() == Expr::Read) {
		ref<ReadExpr> expr1 = cast<ReadExpr>(expr);
		std::string name = expr1->updates.root->name;
		if (solved.count(name)) {
			expr = solved.find(name)->second;
		}
	} else if (expr->getKind() == Expr::Concat) { // ReadLSB
		ref<ConcatExpr> expr1 = cast<ConcatExpr>(expr);
		;
		std::string name = cast<ReadExpr>(expr1->getKid(0))->updates.root->name;
		if (solved.count(name)) {
			expr = solved.find(name)->second;
		}
	} else {
		for (unsigned i = 0; i < expr->getNumKids(); i++) {
			ref<Expr> kid = expr->getKid(i);
			this->replaceSolvedSymbolics(kid, solved);
			expr->setKid(i, kid);
		}
	}
}

void Executor::doImpliedValueConcretization(ExecutionState &state, klee::ref<Expr> e, klee::ref<ConstantExpr> value) {
	abort(); // FIXME: Broken until we sort out how to do the write back.

	if (DebugCheckForImpliedValues) ImpliedValue::checkForImpliedValues(solver->solver, e, value);

	ImpliedValueList results;
	ImpliedValue::getImpliedValues(e, value, results);
	for (ImpliedValueList::iterator it = results.begin(), ie = results.end(); it != ie; ++it) {
		ReadExpr *re = it->first.get();

		if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(re->index))) {
			// FIXME: This is the sole remaining usage of the Array object
			// variable. Kill me.
			const MemoryObject *mo = 0; //re->updates.root->object;
			const ObjectState *os = state.addressSpace().findObject(mo);

			if (!os) {
				// object has been free'd, no need to concretize (although as
				// in other cases we would like to concretize the outstanding
				// reads, but we have no facility for that yet)
			} else {
				assert(!os->readOnly && "not possible? read only object with static read?");
				ObjectState *wos = state.addressSpace().getWriteable(mo, os);
				wos->write(CE, it->second);
			}
		}
	}
}

static void PrintExecutorDump(void *cookie) {
	Executor *executor = (Executor*) cookie;
	executor->PrintDump(std::cerr);
}

void Executor::PrintDump(std::ostream &os) {
	if (activeState) {
		// Print state stack trace
		os << "State trace:" << std::endl;
		activeState->getStackTrace().dump(os);

		// Print memory map
//		os << "Memory map:" << std::endl;
//		activeState->addressSpace().DumpContents(os);
	} else {
		os << "No active state set" << std::endl;
	}
}

void Executor::PrintDumpOnErrorSignal() {
	llvm::sys::AddSignalHandler(PrintExecutorDump, (void*) this);
}

///

Interpreter *Interpreter::create(const InterpreterOptions &opts, InterpreterHandler *ih) {
	return new Executor(opts, ih);
}

//}

