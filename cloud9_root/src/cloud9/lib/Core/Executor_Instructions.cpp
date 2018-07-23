//===-- Executor_Instructions.cpp -----------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Executor.h"

#include "StatsTracker.h"
#include "TimingSolver.h"
#include "klee/Expr.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/klee.h"

#include "llvm/Constants.h"
#include "llvm/GlobalValue.h"
#include "llvm/IntrinsicInst.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CFG.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Operator.h"

#include "../../runtime/POSIX/multiprocess.h"

#include <glog/logging.h>
#include <sstream>
#include <string.h>
#include <stdio.h>

#include "Memory.h"
#include "MemoryManager.h"

#include <boost/lexical_cast.hpp>
#include "klee/data/ExprSerializer.h"

using namespace llvm;

namespace {

using namespace klee;

inline const llvm::fltSemantics * fpWidthToSemantics(unsigned width) {
	switch (width) {
	case Expr::Int32:
		return &llvm::APFloat::IEEEsingle;
	case Expr::Int64:
		return &llvm::APFloat::IEEEdouble;
	case Expr::Fl80:
		return &llvm::APFloat::x87DoubleExtended;
	default:
		return 0;
	}
}

bool isDebugIntrinsic(const Function *f, KModule *KM) {
	return false;
}

}

namespace klee {
class MemoryManager;
class MemoryObject;

void Executor::executeInstruction(ExecutionState &state, KInstruction *ki) {
	if (state.isRedundant) {
		terminateRedundantState(state, true);
		return;
	}
	Instruction *i = ki->inst;
	Function *parentFunc = i->getParent()->getParent();
	std::string parentFuncName = parentFunc->hasName() ? parentFunc->getName().str() : "";
#if 1
	switch (i->getOpcode()) {
	// Control flow
	case Instruction::Ret: {
		ReturnInst *ri = cast<ReturnInst>(i);
		KInstIterator kcaller = state.stack().back().caller;
		Instruction *caller = kcaller ? kcaller->inst : 0;
		bool isVoidReturn = (ri->getNumOperands() == 0);
		klee::ref<Expr> result = ConstantExpr::alloc(0, Expr::Bool);

		fireControlFlowEvent(&state, ::cloud9::worker::RETURN);

		if (!isVoidReturn) {
			result = eval(ki, 0, state).value;
		}

		if (state.stack().size() <= 1) {
			assert(!caller && "caller set on initial stack frame");
			if (state.threads.size() == 1) {
				//main exit
				terminateStateOnExit(state);
			} else if (state.crtProcess().threads.size() == 1) {
				// Invoke exit()
				Function *f = kmodule->module->getFunction("exit");
				std::vector<klee::ref<Expr> > arguments;
				arguments.push_back(result);
				executeCall(state, NULL, f, arguments);
			} else {
				// Invoke pthread_exit()
				Function *f = kmodule->module->getFunction("pthread_exit");
				std::vector<klee::ref<Expr> > arguments;
				arguments.push_back(result);
				executeCall(state, NULL, f, arguments);
			}
		} else {
			StackFrame &sf = state.stack().back();
			state.popFrame();

			if (statsTracker) {
				statsTracker->framePopped(state);
			}

			if (InvokeInst * ii = (dyn_cast<InvokeInst>(caller))) {
				transferToBasicBlock(ii->getNormalDest(), caller->getParent(), state);
			} else {
				state.pc() = kcaller;
				++state.pc();
			}

			if (!isVoidReturn) {
				Type *t = caller->getType();
				if (t != Type::getVoidTy(getGlobalContext())) {
					// may need to do coercion due to bitcasts
					Expr::Width from = result->getWidth();
					Expr::Width to = getWidthForLLVMType(t);

					if (from != to) {
						CallSite cs = (isa<InvokeInst>(caller) ? CallSite(cast<InvokeInst>(caller)) : CallSite(cast<CallInst>(caller)));

						// XXX need to check other param attrs ?
						if (cs.paramHasAttr(0, llvm::Attribute::SExt)) {
							result = SExtExpr::create(result, to);
						} else {
							result = ZExtExpr::create(result, to);
						}
					}
					bindLocal(kcaller, state, result);
				}
			} else {

				// We check that the return value has no users instead of
				// checking the type, since C defaults to returning int for
				// undeclared functions.

				if (!caller->use_empty()) {
					terminateStateOnExecError(state, "return void when caller expected a result");
				}
			}
		}

		break;
	}

	case Instruction::Invoke:
	case Instruction::Call: {
		CallSite cs(i);

		unsigned numArgs = cs.arg_size();
		Function *f = getCalledFunction(cs, state);

		// Skip debug intrinsics, we can't evaluate their metadata arguments.
		if (f && isDebugIntrinsic(f, kmodule)) {
			break;
		}
		// evaluate arguments
		std::vector<klee::ref<Expr> > arguments;
		arguments.reserve(numArgs);

		for (unsigned j = 0; j < numArgs; ++j) {
			arguments.push_back(eval(ki, j + 1, state).value);
		}

		if (!f) {
			// special case the call with a bitcast case
			Value *fp = cs.getCalledValue();
			llvm::ConstantExpr *ce = dyn_cast<llvm::ConstantExpr>(fp);

			if (ce && ce->getOpcode() == Instruction::BitCast) {
				if (GlobalAlias * target = (dyn_cast<GlobalAlias>(ce->getOperand(0)))) {
					GlobalValue *GV = const_cast<GlobalValue*>(target->resolveAliasedGlobal(false));
					f = dyn_cast<Function>(GV);
				} else {
					f = dyn_cast<Function>(ce->getOperand(0));
				}

				assert(f && "XXX unrecognized constant expression in call");
				const FunctionType *fType = dyn_cast<FunctionType>(cast<PointerType>(f->getType())->getElementType());
				const FunctionType *ceType = dyn_cast<FunctionType>(cast<PointerType>(ce->getType())->getElementType());
				assert(fType && ceType && "unable to get function type");

				// XXX check result coercion

				// XXX this really needs thought and validation
				unsigned i = 0;
				for (std::vector<klee::ref<Expr> >::iterator ai = arguments.begin(), ie = arguments.end(); ai != ie; ++ai) {
					Expr::Width to, from = (*ai)->getWidth();

					if (i < fType->getNumParams()) {
						to = getWidthForLLVMType(fType->getParamType(i));

						if (from != to) {
							// XXX need to check other param attrs ?
							if (cs.paramHasAttr(i + 1, llvm::Attribute::SExt)) {
								arguments[i] = SExtExpr::create(arguments[i], to);
							} else {
								arguments[i] = ZExtExpr::create(arguments[i], to);
							}
						}
					}

					i++;
				}
			} else if (isa<InlineAsm>(fp)) {
				terminateStateOnExecError(state, "inline assembly is unsupported");
				break;
			}
		}

		if (f) {
			executeCall(state, ki, f, arguments);
		} else {
			klee::ref<Expr> v = eval(ki, 0, state).value;

			ExecutionState *free = &state;
			bool hasInvalid = false, first = true;

			/* XXX This is wasteful, no need to do a full evaluate since we
			 have already got a value. But in the end the caches should
			 handle it for us, albeit with some overhead. */
			do {
				klee::ref<ConstantExpr> value;
				bool success = solver->getValue(data::FUNCTION_RESOLUTION, *free, v, value);
				assert(success && "FIXME: Unhandled solver failure");
				(void) success;
				//C9HACK_DEBUG("Fork requested: " << (true ? "internal" : "external"), state);
				StatePair res = fork(*free, EqExpr::create(v, value), true, KLEE_FORK_INTERNAL);
				if (res.first) {
					uint64_t addr = value->getZExtValue();
					if (legalFunctions.count(addr)) {
						f = (Function*) addr;

						// Don't give warning on unique resolution
						if (res.second || !first)
						LOG(WARNING) << "Resolved symbolic function pointer to: " << f->getName().data();

						executeCall(*res.first, ki, f, arguments);
					} else {
						if (!hasInvalid) {
							terminateStateOnExecError(state, "invalid function pointer");
							hasInvalid = true;
						}
					}
				}

				first = false;
				free = res.second;
			} while (free);
		}
		break;
	}
	case Instruction::Load: {
		klee::ref<Expr> base = eval(ki, 0, state).value;
		Value* op = i->getOperand(0);

		if (regular) {
			if (white_list_functions.count(parentFuncName)) state.memAccs++;
			executeMemoryOperation(state, false, base, 0, ki);
			break;
		}

		bool no_SCA = false;
		if (!USC) no_SCA = true;
		else if (state.crtThread().getTid() == 0) no_SCA = true;
		else if (state.totalWorkerThreads == 0) no_SCA = true;
		else if (!white_list_functions.count(parentFuncName)) no_SCA = true;
		else if (!state.isSCAddress(base, ki)) no_SCA = true;
		else if (smallSboxes) no_SCA = true;

		if (white_list_functions.count(parentFuncName)) state.memAccs++;

		if (no_SCA) {
			if (state.startShadowExecution) {
				assert(state.crtThread().getTid() == 0);
				assert(precise);
				if (shadow_functions.count(parentFuncName) && ki->sbox != "" && base->getKind() != Expr::Constant) {
					state.shadowMemAddrs.push_back(base);
				}
			}
			executeMemoryOperation(state, false, base, 0, ki);
			break;
		}

		if (precise) {
			state.memAddrs.push_back(base);
			state.memInsts.push_back(ki);
			executeMemoryOperation(state, false, base, 0, ki);
			break;
		}

		// two-step mode
		if (state.allReachInterleavingPoint()) {
			state.memAddrs.push_back(base);
			state.memInsts.push_back(ki);
			if (!state.memAddrs.empty()) {
				// check leakage here
				if (state.executedT2) {
					std::cerr << "-------------------------------------------------------" << std::endl << std::endl;
					std::cerr << "Start csc analysis in interleaving " << &state << ", sym addrs: " << state.memAddrs.size() << std::endl;
					processCacheAccessTwoStep(state, solver);
					std::cerr << "Terminate interleaving " << &state << " after csc analysis." << std::endl;
					std::cerr << "-------------------------------------------------------" << std::endl;
					inter++;
					terminateState(state, true);
				}
			}
			executeMemoryOperation(state, false, base, 0, ki);
			state.crtThread().reachInterleavingPoint = false; // reset the flag
		} else {
			if (white_list_functions.count(parentFuncName)) state.memAccs--;
			updateStateBeforeSchedule(state, ki, ExecutionState::load);
			schedule(state, true, ExecutionState::load); // schedule
		}

		break;
	}
	case Instruction::Store: {
		klee::ref<Expr> base = eval(ki, 1, state).value;
		klee::ref<Expr> value = eval(ki, 0, state).value;
		Value* op = i->getOperand(1);

		if (regular) {
			executeMemoryOperation(state, true, base, value, 0);
			break;
		}

		bool no_SCA = false;
		if (!USC) no_SCA = true;
		else if (state.crtThread().getTid() == 0) no_SCA = true;
		else if (state.totalWorkerThreads == 0) no_SCA = true;
		else if (!white_list_functions.count(parentFuncName) && parentFuncName != "thread2") no_SCA = true;
		else if (!state.isSCAddress(base, ki)) no_SCA = true;
		else if (smallSboxes) no_SCA = true;

		if (white_list_functions.count(parentFuncName)) state.memAccs++;

		if (no_SCA) {
			if (state.startShadowExecution) {
				assert(state.crtThread().getTid() == 0);
				assert(precise);
				if (shadow_functions.count(parentFuncName) && ki->sbox != "" && base->getKind() != Expr::Constant) {
					state.shadowMemAddrs.push_back(base);
				}
			}
			executeMemoryOperation(state, true, base, value, 0);
			break;
		}

		if (precise) {
			if (state.crtThread().getTid() == 2) {
				assert(parentFuncName == "thread2");
				fixed_addr = (dyn_cast<ConstantExpr>(value))->getZExtValue();
			} else {
				state.memAddrs.push_back(base);
				state.memInsts.push_back(ki);
			}
			executeMemoryOperation(state, true, base, value, 0);
			break;
		}

		// two-step mode
		if (state.allReachInterleavingPoint()) {
			executeMemoryOperation(state, true, base, value, 0);
			state.crtThread().reachInterleavingPoint = false; // reset the flag
			if (state.crtThread().getTid() == 2) {
				assert(parentFuncName == "thread2");
				state.executedT2 = true;
				fixed_addr = (dyn_cast<ConstantExpr>(value))->getZExtValue();
			} else {
				state.memAddrs.push_back(base);
				state.memInsts.push_back(ki);
			}
		} else {
			if (white_list_functions.count(parentFuncName)) state.memAccs--;
			updateStateBeforeSchedule(state, ki, ExecutionState::store);
			schedule(state, true, ExecutionState::load); // schedule
		}
		break;
	}

	case Instruction::Br: {
		BranchInst *bi = cast<BranchInst>(i);
		int reason = KLEE_FORK_DEFAULT;
		if (state.crtSpecialFork == i) {
			reason = state.crtForkReason;
			state.crtSpecialFork = NULL;
		} else {
			assert( !state.crtForkReason && "another branching instruction between a klee_branch and its corresponding 'if'");
		}
		state.totalBranches++;

		if (bi->isUnconditional()) {
			transferToBasicBlock(bi->getSuccessor(0), bi->getParent(), state);
		} else {
			// FIXME: Find a way that we don't have this hidden dependency.
			assert( bi->getCondition() == bi->getOperand(0) && "Wrong operand index!");
			klee::ref<Expr> cond = eval(ki, 0, state).value;
			Executor::StatePair branches = fork(state, cond, false, reason);
			if (branches.first) {
				fireControlFlowEvent(branches.first, ::cloud9::worker::BRANCH_TRUE);
			}
			if (branches.second) {
				fireControlFlowEvent(branches.second, ::cloud9::worker::BRANCH_FALSE);
			}

			// NOTE: There is a hidden dependency here, markBranchVisited
			// requires that we still be in the context of the branch
			// instruction (it reuses its statistic id). Should be cleaned
			// up with convenient instruction specific data.
			if (statsTracker) {
				statsTracker->markBranchVisited(branches.first, branches.second);
			}
			if (branches.first) {
				transferToBasicBlock(bi->getSuccessor(0), bi->getParent(), *branches.first);
			}
			if (branches.second) {
				transferToBasicBlock(bi->getSuccessor(1), bi->getParent(), *branches.second);
			}
		}
		break;
	}
	case Instruction::Switch: {
		SwitchInst *si = cast<SwitchInst>(i);
		klee::ref<Expr> cond = eval(ki, 0, state).value;
		BasicBlock *bb = si->getParent();

		cond = toUnique(state, cond);

		if (ConstantExpr * CE = (dyn_cast<ConstantExpr>(cond))) {
			// Somewhat gross to create these all the time, but fine till we
			// switch to an internal rep.
			llvm::IntegerType *Ty = cast<IntegerType>(si->getCondition()->getType());
			ConstantInt *ci = ConstantInt::get(Ty, CE->getZExtValue());
			SwitchInst::CaseIt caseIt = si->findCaseValue(ci);
			transferToBasicBlock(caseIt.getCaseSuccessor(), si->getParent(), state);
		} else {
			std::vector<std::pair<BasicBlock*, klee::ref<Expr> > > options;

			options.push_back(std::make_pair(si->getDefaultDest(), ConstantExpr::alloc(1, Expr::Bool)));

			for (SwitchInst::CaseIt caseIt = si->case_begin(), caseIe = si->case_end(); caseIt != caseIe; ++caseIt) {
				options.push_back(std::make_pair(caseIt.getCaseSuccessor(), evalConstant(caseIt.getCaseValue())));
			}

			typedef std::vector<std::pair<BasicBlock*, ExecutionState*> > branches_ty;
			branches_ty branches;

			branch(state, cond, options, branches, KLEE_FORK_DEFAULT);

			for (branches_ty::iterator bit = branches.begin(), bie = branches.end(); bit != bie; ++bit) {
				ExecutionState *es = bit->second;
				transferToBasicBlock(bit->first, bb, *es);
			}
		}
		break;
	}
	case Instruction::Unreachable:
		// Note that this is not necessarily an internal bug, llvm will
		// generate unreachable instructions in cases where it knows the
		// program will crash. So it is effectively a SEGV or internal
		// error.
		terminateStateOnExecError(state, "reached \"unreachable\" instruction");
		break;
	case Instruction::PHI: {
		klee::ref<Expr> result = eval(ki, state.crtThread().incomingBBIndex, state).value;
		bindLocal(ki, state, result);
		break;
	}
		// Special instructions
	case Instruction::Select: {
		SelectInst *SI = cast<SelectInst>(ki->inst);
		assert( SI->getCondition() == SI->getOperand(0) && "Wrong operand index!");
		klee::ref<Expr> cond = eval(ki, 0, state).value;
		klee::ref<Expr> tExpr = eval(ki, 1, state).value;
		klee::ref<Expr> fExpr = eval(ki, 2, state).value;
		klee::ref<Expr> result = SelectExpr::create(cond, tExpr, fExpr);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::VAArg:
		terminateStateOnExecError(state, "unexpected VAArg instruction");
		break;

		// Arithmetic / logical

	case Instruction::Add: {
		klee::ref<Expr> left = eval(ki, 0, state).value;
		klee::ref<Expr> right = eval(ki, 1, state).value;
		klee::ref<Expr> result = AddExpr::create(left, right);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::Sub: {
		klee::ref<Expr> left = eval(ki, 0, state).value;
		klee::ref<Expr> right = eval(ki, 1, state).value;
		klee::ref<Expr> result = SubExpr::create(left, right);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::Mul: {
		klee::ref<Expr> left = eval(ki, 0, state).value;
		klee::ref<Expr> right = eval(ki, 1, state).value;
		klee::ref<Expr> result = MulExpr::create(left, right);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::UDiv: {
		klee::ref<Expr> left = eval(ki, 0, state).value;
		klee::ref<Expr> right = eval(ki, 1, state).value;
		klee::ref<Expr> result = UDivExpr::create(left, right);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::SDiv: {
		klee::ref<Expr> left = eval(ki, 0, state).value;
		klee::ref<Expr> right = eval(ki, 1, state).value;
		klee::ref<Expr> result = SDivExpr::create(left, right);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::URem: {
		klee::ref<Expr> left = eval(ki, 0, state).value;
		klee::ref<Expr> right = eval(ki, 1, state).value;
		klee::ref<Expr> result = URemExpr::create(left, right);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::SRem: {
		klee::ref<Expr> left = eval(ki, 0, state).value;
		klee::ref<Expr> right = eval(ki, 1, state).value;
		klee::ref<Expr> result = SRemExpr::create(left, right);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::And: {
		klee::ref<Expr> left = eval(ki, 0, state).value;
		klee::ref<Expr> right = eval(ki, 1, state).value;
		klee::ref<Expr> result = AndExpr::create(left, right);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::Or: {
		klee::ref<Expr> left = eval(ki, 0, state).value;
		klee::ref<Expr> right = eval(ki, 1, state).value;
		klee::ref<Expr> result = OrExpr::create(left, right);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::Xor: {
		klee::ref<Expr> left = eval(ki, 0, state).value;
		klee::ref<Expr> right = eval(ki, 1, state).value;
		klee::ref<Expr> result = XorExpr::create(left, right);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::Shl: {
		klee::ref<Expr> left = eval(ki, 0, state).value;
		klee::ref<Expr> right = eval(ki, 1, state).value;
		klee::ref<Expr> result = ShlExpr::create(left, right);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::LShr: {
		klee::ref<Expr> left = eval(ki, 0, state).value;
		klee::ref<Expr> right = eval(ki, 1, state).value;
		klee::ref<Expr> result = LShrExpr::create(left, right);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::AShr: {
		klee::ref<Expr> left = eval(ki, 0, state).value;
		klee::ref<Expr> right = eval(ki, 1, state).value;
		klee::ref<Expr> result = AShrExpr::create(left, right);
		bindLocal(ki, state, result);
		break;
	}

		// Compare
	case Instruction::ICmp: {
		CmpInst *ci = cast<CmpInst>(i);
		ICmpInst *ii = cast<ICmpInst>(ci);

		switch (ii->getPredicate()) {
		case ICmpInst::ICMP_EQ: {
			klee::ref<Expr> left = eval(ki, 0, state).value;
			klee::ref<Expr> right = eval(ki, 1, state).value;
			klee::ref<Expr> result = EqExpr::create(left, right);
			bindLocal(ki, state, result);
			break;
		}

		case ICmpInst::ICMP_NE: {
			klee::ref<Expr> left = eval(ki, 0, state).value;
			klee::ref<Expr> right = eval(ki, 1, state).value;
			klee::ref<Expr> result = NeExpr::create(left, right);
			bindLocal(ki, state, result);
			break;
		}

		case ICmpInst::ICMP_UGT: {
			klee::ref<Expr> left = eval(ki, 0, state).value;
			klee::ref<Expr> right = eval(ki, 1, state).value;
			klee::ref<Expr> result = UgtExpr::create(left, right);
			bindLocal(ki, state, result);
			break;
		}

		case ICmpInst::ICMP_UGE: {
			klee::ref<Expr> left = eval(ki, 0, state).value;
			klee::ref<Expr> right = eval(ki, 1, state).value;
			klee::ref<Expr> result = UgeExpr::create(left, right);
			bindLocal(ki, state, result);
			break;
		}

		case ICmpInst::ICMP_ULT: {
			klee::ref<Expr> left = eval(ki, 0, state).value;
			klee::ref<Expr> right = eval(ki, 1, state).value;
			klee::ref<Expr> result = UltExpr::create(left, right);
			bindLocal(ki, state, result);
			break;
		}

		case ICmpInst::ICMP_ULE: {
			klee::ref<Expr> left = eval(ki, 0, state).value;
			klee::ref<Expr> right = eval(ki, 1, state).value;
			klee::ref<Expr> result = UleExpr::create(left, right);
			bindLocal(ki, state, result);
			break;
		}

		case ICmpInst::ICMP_SGT: {
			klee::ref<Expr> left = eval(ki, 0, state).value;
			klee::ref<Expr> right = eval(ki, 1, state).value;
			klee::ref<Expr> result = SgtExpr::create(left, right);
			bindLocal(ki, state, result);
			break;
		}

		case ICmpInst::ICMP_SGE: {
			klee::ref<Expr> left = eval(ki, 0, state).value;
			klee::ref<Expr> right = eval(ki, 1, state).value;
			klee::ref<Expr> result = SgeExpr::create(left, right);
			bindLocal(ki, state, result);
			break;
		}

		case ICmpInst::ICMP_SLT: {
			klee::ref<Expr> left = eval(ki, 0, state).value;
			klee::ref<Expr> right = eval(ki, 1, state).value;
			klee::ref<Expr> result = SltExpr::create(left, right);
			bindLocal(ki, state, result);
			break;
		}

		case ICmpInst::ICMP_SLE: {
			klee::ref<Expr> left = eval(ki, 0, state).value;
			klee::ref<Expr> right = eval(ki, 1, state).value;
			klee::ref<Expr> result = SleExpr::create(left, right);
			bindLocal(ki, state, result);
			break;
		}

		default:
			terminateStateOnExecError(state, "invalid ICmp predicate");
		}
		break;
	}

		// Memory instructions...
	case Instruction::Alloca: {
		AllocaInst *ai = cast<AllocaInst>(i);
		unsigned elementSize = kmodule->targetData->getTypeStoreSize(ai->getAllocatedType());
		klee::ref<Expr> size = Expr::createPointer(elementSize);
		if (ai->isArrayAllocation()) {
			klee::ref<Expr> count = eval(ki, 0, state).value;
			count = Expr::createCoerceToPointerType(count);
			size = MulExpr::create(size, count);
		}
		executeAlloc(state, size, true, ki);
		break;
	}

	case Instruction::GetElementPtr: {
		KGEPInstruction *kgepi = static_cast<KGEPInstruction*>(ki);
		klee::ref<Expr> base = eval(ki, 0, state).value;

		for (std::vector<std::pair<unsigned, uint64_t> >::iterator it = kgepi->indices.begin(), ie = kgepi->indices.end(); it != ie; ++it) {
			uint64_t elementSize = it->second;
			klee::ref<Expr> index = eval(ki, it->first, state).value;
			base = AddExpr::create(base, MulExpr::create(Expr::createCoerceToPointerType(index), Expr::createPointer(elementSize)));
		}
		if (kgepi->offset) {
			base = AddExpr::create(base, Expr::createPointer(kgepi->offset));
		}
		bindLocal(ki, state, base);

		static unsigned count = 0;
		if (USC) {
			if (white_list_functions.count(parentFuncName) || shadow_functions.count(parentFuncName)) {
				Value *v = i->getOperand(0);
				if (const llvm::ConstantExpr *ce = dyn_cast<llvm::ConstantExpr>(v)) {
					if (ce->getOpcode() == Instruction::GetElementPtr) {
						if (ce->getOperand(0)->hasName()) {
							string name = ce->getOperand(0)->getName().str();
							if (sboxGlobalMOs.count(name)) {
								state.pc()->sbox = name;
								break;
							}
						}
					}
				}
				if (v->hasName()) {
					string name = v->getName().str();
					if (sboxGlobalMOs.count(name)) {
						state.pc()->sbox = name;
					}
				}
			}
		}
		break;
	}

		// Conversion
	case Instruction::Trunc: {
		CastInst *ci = cast<CastInst>(i);
		klee::ref<Expr> result = ExtractExpr::create(eval(ki, 0, state).value, 0, getWidthForLLVMType(ci->getType()));
		bindLocal(ki, state, result);
		break;
	}
	case Instruction::ZExt: {
		CastInst *ci = cast<CastInst>(i);
		klee::ref<Expr> result = ZExtExpr::create(eval(ki, 0, state).value, getWidthForLLVMType(ci->getType()));
		bindLocal(ki, state, result);
		break;
	}
	case Instruction::SExt: {
		CastInst *ci = cast<CastInst>(i);
		klee::ref<Expr> result = SExtExpr::create(eval(ki, 0, state).value, getWidthForLLVMType(ci->getType()));
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::IntToPtr: {
		CastInst *ci = cast<CastInst>(i);
		Expr::Width pType = getWidthForLLVMType(ci->getType());
		klee::ref<Expr> arg = eval(ki, 0, state).value;
		klee::ref<Expr> result = ZExtExpr::create(arg, pType);
		bindLocal(ki, state, result);
		break;
	}
	case Instruction::PtrToInt: {
		CastInst *ci = cast<CastInst>(i);
		Expr::Width iType = getWidthForLLVMType(ci->getType());
		klee::ref<Expr> arg = eval(ki, 0, state).value;
		klee::ref<Expr> result = ZExtExpr::create(arg, iType);
		bindLocal(ki, state, result);
		break;
	}

	case Instruction::BitCast: {
		klee::ref<Expr> result = eval(ki, 0, state).value;
		bindLocal(ki, state, result);
		break;
	}

		// Floating point instructions

	case Instruction::FAdd: {
		klee::ref<ConstantExpr> left = toConstant(state, eval(ki, 0, state).value, "floating point");
		klee::ref<ConstantExpr> right = toConstant(state, eval(ki, 1, state).value, "floating point");
		if (!fpWidthToSemantics(left->getWidth()) || !fpWidthToSemantics(right->getWidth()))
			return terminateStateOnExecError(state, "Unsupported FAdd operation");

		llvm::APFloat Res(left->getAPValue());
		Res.add(APFloat(right->getAPValue()), APFloat::rmNearestTiesToEven);
		bindLocal(ki, state, ConstantExpr::alloc(Res.bitcastToAPInt()));
		break;
	}

	case Instruction::FSub: {
		klee::ref<ConstantExpr> left = toConstant(state, eval(ki, 0, state).value, "floating point");
		klee::ref<ConstantExpr> right = toConstant(state, eval(ki, 1, state).value, "floating point");
		if (!fpWidthToSemantics(left->getWidth()) || !fpWidthToSemantics(right->getWidth()))
			return terminateStateOnExecError(state, "Unsupported FSub operation");

		llvm::APFloat Res(left->getAPValue());
		Res.subtract(APFloat(right->getAPValue()), APFloat::rmNearestTiesToEven);
		bindLocal(ki, state, ConstantExpr::alloc(Res.bitcastToAPInt()));
		break;
	}

	case Instruction::FMul: {
		klee::ref<ConstantExpr> left = toConstant(state, eval(ki, 0, state).value, "floating point");
		klee::ref<ConstantExpr> right = toConstant(state, eval(ki, 1, state).value, "floating point");
		if (!fpWidthToSemantics(left->getWidth()) || !fpWidthToSemantics(right->getWidth()))
			return terminateStateOnExecError(state, "Unsupported FMul operation");

		llvm::APFloat Res(left->getAPValue());
		Res.multiply(APFloat(right->getAPValue()), APFloat::rmNearestTiesToEven);
		bindLocal(ki, state, ConstantExpr::alloc(Res.bitcastToAPInt()));
		break;
	}

	case Instruction::FDiv: {
		klee::ref<ConstantExpr> left = toConstant(state, eval(ki, 0, state).value, "floating point");
		klee::ref<ConstantExpr> right = toConstant(state, eval(ki, 1, state).value, "floating point");
		if (!fpWidthToSemantics(left->getWidth()) || !fpWidthToSemantics(right->getWidth()))
			return terminateStateOnExecError(state, "Unsupported FDiv operation");

		llvm::APFloat Res(left->getAPValue());
		Res.divide(APFloat(right->getAPValue()), APFloat::rmNearestTiesToEven);
		bindLocal(ki, state, ConstantExpr::alloc(Res.bitcastToAPInt()));
		break;
	}

	case Instruction::FRem: {
		klee::ref<ConstantExpr> left = toConstant(state, eval(ki, 0, state).value, "floating point");
		klee::ref<ConstantExpr> right = toConstant(state, eval(ki, 1, state).value, "floating point");
		if (!fpWidthToSemantics(left->getWidth()) || !fpWidthToSemantics(right->getWidth()))
			return terminateStateOnExecError(state, "Unsupported FRem operation");

		llvm::APFloat Res(left->getAPValue());
		Res.mod(APFloat(right->getAPValue()), APFloat::rmNearestTiesToEven);
		bindLocal(ki, state, ConstantExpr::alloc(Res.bitcastToAPInt()));
		break;
	}

	case Instruction::FPTrunc: {
		FPTruncInst *fi = cast<FPTruncInst>(i);
		Expr::Width resultType = getWidthForLLVMType(fi->getType());
		klee::ref<ConstantExpr> arg = toConstant(state, eval(ki, 0, state).value, "floating point");
		if (!fpWidthToSemantics(arg->getWidth()) || resultType > arg->getWidth())
			return terminateStateOnExecError(state, "Unsupported FPTrunc operation");

		llvm::APFloat Res(arg->getAPValue());
		bool losesInfo = false;
		Res.convert(*fpWidthToSemantics(resultType), llvm::APFloat::rmNearestTiesToEven, &losesInfo);
		bindLocal(ki, state, ConstantExpr::alloc(Res));
		break;
	}

	case Instruction::FPExt: {
		FPExtInst *fi = cast<FPExtInst>(i);
		Expr::Width resultType = getWidthForLLVMType(fi->getType());
		klee::ref<ConstantExpr> arg = toConstant(state, eval(ki, 0, state).value, "floating point");
		if (!fpWidthToSemantics(arg->getWidth()) || arg->getWidth() > resultType)
			return terminateStateOnExecError(state, "Unsupported FPExt operation");

		llvm::APFloat Res(arg->getAPValue());
		bool losesInfo = false;
		Res.convert(*fpWidthToSemantics(resultType), llvm::APFloat::rmNearestTiesToEven, &losesInfo);
		bindLocal(ki, state, ConstantExpr::alloc(Res));
		break;
	}

	case Instruction::FPToUI: {
		FPToUIInst *fi = cast<FPToUIInst>(i);
		Expr::Width resultType = getWidthForLLVMType(fi->getType());
		klee::ref<ConstantExpr> arg = toConstant(state, eval(ki, 0, state).value, "floating point");
		if (!fpWidthToSemantics(arg->getWidth()) || resultType > 64) return terminateStateOnExecError(state, "Unsupported FPToUI operation");

		llvm::APFloat Arg(arg->getAPValue());
		uint64_t value = 0;
		bool isExact = true;
		Arg.convertToInteger(&value, resultType, false, llvm::APFloat::rmTowardZero, &isExact);
		bindLocal(ki, state, ConstantExpr::alloc(value, resultType));
		break;
	}

	case Instruction::FPToSI: {
		FPToSIInst *fi = cast<FPToSIInst>(i);
		Expr::Width resultType = getWidthForLLVMType(fi->getType());
		klee::ref<ConstantExpr> arg = toConstant(state, eval(ki, 0, state).value, "floating point");
		if (!fpWidthToSemantics(arg->getWidth()) || resultType > 64) return terminateStateOnExecError(state, "Unsupported FPToSI operation");

		llvm::APFloat Arg(arg->getAPValue());
		uint64_t value = 0;
		bool isExact = true;
		Arg.convertToInteger(&value, resultType, false, llvm::APFloat::rmTowardZero, &isExact);
		bindLocal(ki, state, ConstantExpr::alloc(value, resultType));
		break;
	}

	case Instruction::UIToFP: {
		UIToFPInst *fi = cast<UIToFPInst>(i);
		Expr::Width resultType = getWidthForLLVMType(fi->getType());
		klee::ref<ConstantExpr> arg = toConstant(state, eval(ki, 0, state).value, "floating point");
		const llvm::fltSemantics *semantics = fpWidthToSemantics(resultType);
		if (!semantics) return terminateStateOnExecError(state, "Unsupported UIToFP operation");
		llvm::APFloat f(*semantics, 0);
		f.convertFromAPInt(arg->getAPValue(), false, llvm::APFloat::rmNearestTiesToEven);
		bindLocal(ki, state, ConstantExpr::alloc(f));
		break;
	}

	case Instruction::SIToFP: {
		SIToFPInst *fi = cast<SIToFPInst>(i);
		Expr::Width resultType = getWidthForLLVMType(fi->getType());
		klee::ref<ConstantExpr> arg = toConstant(state, eval(ki, 0, state).value, "floating point");
		const llvm::fltSemantics *semantics = fpWidthToSemantics(resultType);
		if (!semantics) return terminateStateOnExecError(state, "Unsupported SIToFP operation");
		llvm::APFloat f(*semantics, 0);
		f.convertFromAPInt(arg->getAPValue(), true, llvm::APFloat::rmNearestTiesToEven);
		bindLocal(ki, state, ConstantExpr::alloc(f));
		break;
	}

	case Instruction::FCmp: {
		FCmpInst *fi = cast<FCmpInst>(i);
		klee::ref<ConstantExpr> left = toConstant(state, eval(ki, 0, state).value, "floating point");
		klee::ref<ConstantExpr> right = toConstant(state, eval(ki, 1, state).value, "floating point");
		if (!fpWidthToSemantics(left->getWidth()) || !fpWidthToSemantics(right->getWidth()))
			return terminateStateOnExecError(state, "Unsupported FCmp operation");

		APFloat LHS(left->getAPValue());
		APFloat RHS(right->getAPValue());
		APFloat::cmpResult CmpRes = LHS.compare(RHS);

		bool Result = false;
		switch (fi->getPredicate()) {
		// Predicates which only care about whether or not the operands are NaNs.
		case FCmpInst::FCMP_ORD:
			Result = CmpRes != APFloat::cmpUnordered;
			break;

		case FCmpInst::FCMP_UNO:
			Result = CmpRes == APFloat::cmpUnordered;
			break;

			// Ordered comparisons return false if either operand is NaN.  Unordered
			// comparisons return true if either operand is NaN.
		case FCmpInst::FCMP_UEQ:
			if (CmpRes == APFloat::cmpUnordered) {
				Result = true;
				break;
			}
		case FCmpInst::FCMP_OEQ:
			Result = CmpRes == APFloat::cmpEqual;
			break;

		case FCmpInst::FCMP_UGT:
			if (CmpRes == APFloat::cmpUnordered) {
				Result = true;
				break;
			}
		case FCmpInst::FCMP_OGT:
			Result = CmpRes == APFloat::cmpGreaterThan;
			break;

		case FCmpInst::FCMP_UGE:
			if (CmpRes == APFloat::cmpUnordered) {
				Result = true;
				break;
			}
		case FCmpInst::FCMP_OGE:
			Result = CmpRes == APFloat::cmpGreaterThan || CmpRes == APFloat::cmpEqual;
			break;

		case FCmpInst::FCMP_ULT:
			if (CmpRes == APFloat::cmpUnordered) {
				Result = true;
				break;
			}
		case FCmpInst::FCMP_OLT:
			Result = CmpRes == APFloat::cmpLessThan;
			break;

		case FCmpInst::FCMP_ULE:
			if (CmpRes == APFloat::cmpUnordered) {
				Result = true;
				break;
			}
		case FCmpInst::FCMP_OLE:
			Result = CmpRes == APFloat::cmpLessThan || CmpRes == APFloat::cmpEqual;
			break;

		case FCmpInst::FCMP_UNE:
			Result = CmpRes == APFloat::cmpUnordered || CmpRes != APFloat::cmpEqual;
			break;
		case FCmpInst::FCMP_ONE:
			Result = CmpRes != APFloat::cmpUnordered && CmpRes != APFloat::cmpEqual;
			break;

		default:
			assert(0 && "Invalid FCMP predicate!");
		case FCmpInst::FCMP_FALSE:
			Result = false;
			break;
		case FCmpInst::FCMP_TRUE:
			Result = true;
			break;
		}

		bindLocal(ki, state, ConstantExpr::alloc(Result, Expr::Bool));
		break;
	}
	case Instruction::InsertValue: {

		KGEPInstruction *kgepi = static_cast<KGEPInstruction*>(ki);

		klee::ref<Expr> agg = eval(ki, 0, state).value;
		klee::ref<Expr> val = eval(ki, 1, state).value;

		klee::ref<Expr> l = NULL, r = NULL;
		unsigned lOffset = kgepi->offset * 8, rOffset = kgepi->offset * 8 + val->getWidth();

		if (lOffset > 0) l = ExtractExpr::create(agg, 0, lOffset);
		if (rOffset < agg->getWidth()) r = ExtractExpr::create(agg, rOffset, agg->getWidth() - rOffset);

		klee::ref<Expr> result;
		if (!l.isNull() && !r.isNull()) result = ConcatExpr::create(r, ConcatExpr::create(val, l));
		else if (!l.isNull()) result = ConcatExpr::create(val, l);
		else if (!r.isNull()) result = ConcatExpr::create(r, val);
		else result = val;

		bindLocal(ki, state, result);
		break;
	}
	case Instruction::ExtractValue: {

		KGEPInstruction *kgepi = static_cast<KGEPInstruction*>(ki);

		klee::ref<Expr> agg = eval(ki, 0, state).value;

		klee::ref<Expr> result = ExtractExpr::create(agg, kgepi->offset * 8, getWidthForLLVMType(i->getType()));

		bindLocal(ki, state, result);
		break;
	}

	case Instruction::ExtractElement: {

		ExtractElementInst *eei = cast<ExtractElementInst>(i);
		klee::ref<Expr> vec = eval(ki, 0, state).value;
		klee::ref<Expr> idx = eval(ki, 1, state).value;

		assert(isa<ConstantExpr>(idx) && "symbolic index unsupported");
		ConstantExpr *cIdx = cast<ConstantExpr>(idx);
		uint64_t iIdx = cIdx->getZExtValue();

		const llvm::VectorType *vt = eei->getVectorOperandType();
		unsigned EltBits = getWidthForLLVMType(vt->getElementType());

		klee::ref<Expr> result = ExtractExpr::create(vec, EltBits * iIdx, EltBits);

		bindLocal(ki, state, result);
		break;
	}
	case Instruction::InsertElement: {

		InsertElementInst *iei = cast<InsertElementInst>(i);
		klee::ref<Expr> vec = eval(ki, 0, state).value;
		klee::ref<Expr> newElt = eval(ki, 1, state).value;
		klee::ref<Expr> idx = eval(ki, 2, state).value;

		assert(isa<ConstantExpr>(idx) && "symbolic index unsupported");
		ConstantExpr *cIdx = cast<ConstantExpr>(idx);
		uint64_t iIdx = cIdx->getZExtValue();

		const llvm::VectorType *vt = iei->getType();
		unsigned EltBits = getWidthForLLVMType(vt->getElementType());

		unsigned ElemCount = vt->getNumElements();
		klee::ref<Expr> *elems = new klee::ref<Expr>[vt->getNumElements()];
		for (unsigned i = 0; i < ElemCount; ++i)
			elems[ElemCount - i - 1] = i == iIdx ? newElt : ExtractExpr::create(vec, EltBits * i, EltBits);

		klee::ref<Expr> result = ConcatExpr::createN(ElemCount, elems);
		delete[] elems;
		bindLocal(ki, state, result);
		break;
	}
	case Instruction::ShuffleVector: {
		ShuffleVectorInst *svi = cast<ShuffleVectorInst>(i);

		klee::ref<Expr> vec1 = eval(ki, 0, state).value;
		klee::ref<Expr> vec2 = eval(ki, 1, state).value;
		const llvm::VectorType *vt = svi->getType();
		unsigned EltBits = getWidthForLLVMType(vt->getElementType());

		unsigned ElemCount = vt->getNumElements();
		klee::ref<Expr> *elems = new klee::ref<Expr>[vt->getNumElements()];
		for (unsigned i = 0; i < ElemCount; ++i) {
			int MaskValI = svi->getMaskValue(i);
			klee::ref<Expr> &el = elems[ElemCount - i - 1];
			if (MaskValI < 0) el = ConstantExpr::alloc(0, EltBits);
			else {
				unsigned MaskVal = (unsigned) MaskValI;
				if (MaskVal < ElemCount) el = ExtractExpr::create(vec1, EltBits * MaskVal, EltBits);
				else el = ExtractExpr::create(vec2, EltBits * (MaskVal - ElemCount), EltBits);
			}
		}

		klee::ref<Expr> result = ConcatExpr::createN(ElemCount, elems);
		delete[] elems;
		bindLocal(ki, state, result);
		break;
	}
	default:
		terminateStateOnExecError(state, "illegal instruction");
		break;
	}

#endif
}

std::string Executor::generatePrefix(ExecutionState &state) {
	string ipp_id = state.ipp_list.back();
	string prefix;
	if (state.transition_sequence.empty()) {
		prefix = "[" + ipp_id + "]";
	} else {
		for (int i = state.transition_sequence.size() - 1; i >= 0; i--) {
			TransitionEvent &e = state.transition_sequence[i];
			if (e.transition_type == ExecutionState::boundaryEnd) continue;
			if (e.transition_type == ExecutionState::boundaryStart) continue;
			if (e.transition_type == ExecutionState::boundary) continue;
			prefix = e.prefix + "[" + ipp_id + "]";
			break;
		}
		if (prefix.empty()) {
			prefix = "[" + ipp_id + "]";
		}
	}
	assert(!prefix.empty());
	return prefix;
}

void Executor::addTransitionEvent(ExecutionState &state, int transition_type, uint64_t mem_addr, std::string name) {
	if (DEBUG) {
		std::cerr << "[DBG]: Enter addTransitionEvent in state: " << &state << std::endl;
		std::cerr << "[DBG]: event type: " << transition_type << std::endl;
		std::cerr << "[DBG]: thread tid: " << state.crtThread().getTid() << std::endl;
	}
	TransitionEvent ti;
	Thread &thread = state.crtThread();
	ti.thread_uid = thread.getUid();
	ti.period = thread.period;
	ti.crtPeriodIndex = thread.crtPeriodIndex;
	ti.priority = thread.priority;
	ti.transition_type = transition_type;
	if (transition_type == ExecutionState::load || transition_type == ExecutionState::store) {
		ti.ipp_id = state.ipp_list.back();
		ti.prefix = generatePrefix(state);
		assert((state.ipp_prefix_map.insert(std::make_pair(ti.ipp_id, ti.prefix))).second);
	}
	ti.global_addr = mem_addr;
	ti.name = name;
	state.transition_sequence.push_back(ti);
	if (DEBUG) {
		std::cerr << "[DBG]: Leave addTransitionEvent in state: " << &state << std::endl;
	}
}

void Executor::updateBacktrackSet(ExecutionState &state) {
	assert(this->UseDPOR);
	if (DEBUG) {
		std::cerr << "[DBG]: Enter updateBacktrackInfo in state: " << &state << " at iPP: " << state.ipp_list.back() << std::endl;
	}

	//reach the first iPP, here the sequence is empty,just return
	if (state.transition_sequence.empty()) {
		if (DEBUG) {
			std::cerr << "[DBG]: transition_sequence is empty" << std::endl;
			std::cerr << "[DBG]: Leave updateBacktrackInfo in state: " << &state << " at iPP: " << state.ipp_list.back() << std::endl;
		}
		return;
	}

	//iterate all the active threads to check the co-enable relationship
	ExecutionState::threads_ty::iterator tit = state.threads.begin();
	for (; tit != state.threads.end(); tit++) {
		if (!tit->second.enabled || tit->second.getTid() == 0) continue;
		Thread& thread = tit->second;
		uint64_t transition_type = thread.lastTransitionType;
		uint64_t mem_addr = thread.lastTransitionAddr;
		int index = state.transition_sequence.size() - 1;
		// backtrack existing event sequence
		for (int i = index; i >= 0; i--) {
			TransitionEvent& event = state.transition_sequence[i];
			// At least one Store operation
			if (event.transition_type != ExecutionState::store && transition_type != ExecutionState::store) continue;
			// Two events should be dependent
			if (event.global_addr != mem_addr) continue;
			// If two events are from the same thread
			if (event.thread_uid == thread.getUid()) {
				if (event.transition_type == ExecutionState::store) break;
				else continue;
			}
			// Check if the thread id is already in backtrack_set or done_set
			std::string ipp_id = event.ipp_id;
			std::string prefix = event.prefix;
			thread_uid_t uid = thread.getUid();
			if (this->backtrack_set[prefix].count(uid)) {
				if (DEBUG) std::cerr << "[DBG]: tid " << uid.first << " already in backtrack_set at " << ipp_id << std::endl;
			} else {
				if (this->UsePriorityPreemption) {
					if (thread.priority > event.priority) {
						this->backtrack_set[prefix].insert(uid);
						if (this->DEBUG) std::cerr << "[DBG]: insert tid " << uid.first << " into backtrack_set at: " << ipp_id << std::endl;
					} else if (this->done_set[prefix].count(uid)) {
						if (DEBUG) std::cerr << "[DBG]: tid " << uid.first << " already in done_set at: " << ipp_id << std::endl;
					} else {
						if (this->DEBUG)
							std::cerr << "[DBG]: cannot insert lower-priority tid " << uid.first << " into backtrack_set at: " << ipp_id << std::endl;
					}
				} else {
					if (this->done_set[prefix].count(uid)) {
						if (DEBUG) std::cerr << "[DBG]: tid " << uid.first << " already in done_set at: " << ipp_id << std::endl;
					} else {
						this->backtrack_set[prefix].insert(uid);
						if (DEBUG) std::cerr << "[DBG]: insert tid " << uid.first << " into backtrack_set at: " << ipp_id << std::endl;
					}
				}
			}
			break;
		}
	}
	if (DEBUG) {
		std::cerr << "[DBG]: Leave updateBacktrackInfo in state: " << &state << " at iPP: " << state.ipp_list.back() << std::endl;
	}
}

void Executor::updateDoneSet(ExecutionState &state) { // add the latest transition info into done_set
	if (DEBUG) std::cerr << "[DBG]: Enter updateDoneSetInfo in state: " << &state << std::endl;

	TransitionEvent &e = state.transition_sequence.back();
	std::string prefix = e.prefix;
	thread_uid_t tuid = state.crtThread().getUid();
	if (this->done_set[prefix].count(tuid)) {
		if (DEBUG) std::cerr << "[DBG]: tuid " << tuid.first << " already in done_set" << std::endl;
	} else {
		done_set[prefix].insert(tuid);
		if (DEBUG) {
			std::cerr << "[DBG]: Insert tuid " << tuid.first << " to done_set at ipp " << e.ipp_id << std::endl;
			std::cerr << "[DBG]: Leave updateDoneSetInfo in state: " << &state << std::endl;
		}
	}
}

void Executor::updateStateBeforeSchedule(ExecutionState &state, KInstruction* ki, uint64_t lastTransitionType, uint64_t lastTransitionAddr) {
	// set the reach_ipp flag
	state.crtThread().reachInterleavingPoint = true;
	// reset program counter
	state.pc() = state.prevPC();
}

std::string Executor::generateIPP(ExecutionState &state) {
	if (DEBUG) std::cerr << "[DBG]: Enter generateIPP at state " << &state << std::endl;

	std::stringstream ss;
	uint64_t counter = 1;
	std::map<thread_uid_t, Thread>::iterator mit = state.threads.begin();
	mit++; //skip thread 0
	while (counter <= state.totalWorkerThreads) {
		if (mit != state.threads.end()) {
			while (mit->second.uniqueID > counter) {
				ss << "T" << counter << "@" << -1 << "_";
				counter++;
			}
			ss << "T" << counter << "@" << mit->second.locationStr << "_";
			mit++;
			counter++;
			continue;
		}
		ss << "T" << counter << "@" << -1 << "_";
		counter++;
	}

	std::string ipp_id = ss.str();
	state.ipp_list.push_back(ipp_id);
	if (DEBUG) {
		std::cerr << "[DBG]: iPP generated: " << ipp_id << std::endl;
		std::cerr << "[DBG]: Leave generateIPP at state " << &state << std::endl;
	}
	return ipp_id;
}

void Executor::sleepThread(ExecutionState&state, thread_uid_t tuid, wlist_id_t wlist) {
	Thread &thread = state.threads.find(tuid)->second;
	if (!thread.enabled) {
		this->terminateState(state, true);
	}
	assert(wlist > 0);
//	std::cerr << "thread will go sleeping for exclusive mutex: " << tuid.first << std::endl;

	thread.sleeping = true;
	thread.enabled = false;
	thread.waitingList = wlist;
	std::set<thread_uid_t> &wl = state.waitingLists[wlist];

	wl.insert(tuid);
// state.activeThreads--;
}

}
