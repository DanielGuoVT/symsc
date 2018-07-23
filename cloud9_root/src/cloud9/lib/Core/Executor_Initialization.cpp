//===-- Executor_Initialization.cpp ---------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Executor.h"
#include "klee/Utils.h"

#include "ExternalDispatcher.h"
#include "Memory.h"
#include "MemoryManager.h"
#include "PTree.h"
#include "StatsTracker.h"
#include "klee/util/GetElementPtrTypeIterator.h"

#include "llvm/Constants.h"
#include "llvm/Module.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include <glog/logging.h>

#include <boost/lexical_cast.hpp>

extern void *__dso_handle; //__attribute__ ((__weak__));

using namespace llvm;

namespace {

cl::opt<bool> UseAsmAddresses("use-asm-addresses", cl::init(false));

}

namespace klee {

void Executor::initializeGlobalObject(ExecutionState &state, ObjectState *os, const Constant *c, unsigned offset, MemoryObject* mo) {
	TargetData *targetData = kmodule->targetData;

	if (const ConstantVector *cp = dyn_cast<ConstantVector>(c)) {
		unsigned elementSize = targetData->getTypeStoreSize(cp->getType()->getElementType());
		for (unsigned i = 0, e = cp->getNumOperands(); i != e; ++i)
			initializeGlobalObject(state, os, cp->getOperand(i), offset + i * elementSize);
	} else if (isa<ConstantAggregateZero>(c)) {
		unsigned i, size = targetData->getTypeStoreSize(c->getType());
		for (i = 0; i < size; i++)
			os->write8(offset + i, (uint8_t) 0);
	} else if (const ConstantArray *ca = dyn_cast<ConstantArray>(c)) {
		unsigned elementSize = targetData->getTypeStoreSize(ca->getType()->getElementType());
		for (unsigned i = 0, e = ca->getNumOperands(); i != e; ++i)
			initializeGlobalObject(state, os, ca->getOperand(i), offset + i * elementSize);
	} else if (const ConstantStruct *cs = dyn_cast<ConstantStruct>(c)) {
		const StructLayout *sl = targetData->getStructLayout(cast<StructType>(cs->getType()));
		for (unsigned i = 0, e = cs->getNumOperands(); i != e; ++i)
			initializeGlobalObject(state, os, cs->getOperand(i), offset + sl->getElementOffset(i));
	} else if (const ConstantDataSequential *cds = dyn_cast<ConstantDataSequential>(c)) {
		unsigned elementSize = targetData->getTypeStoreSize(cds->getElementType());
		for (unsigned i = 0, e = cds->getNumElements(); i != e; ++i)
			initializeGlobalObject(state, os, cds->getElementAsConstant(i), offset + i * elementSize);
	} else {
		unsigned StoreBits = targetData->getTypeStoreSizeInBits(c->getType());
//  LOG(INFO) << "ValueID: " << c->getValueID();
//  LOG(INFO) << "Global object size: " << c->getType()->getScalarSizeInBits() << " (store size: " << StoreBits << ")";

		klee::ref<ConstantExpr> C = evalConstant(c);

		// Extend the constant if necessary;
		assert(StoreBits >= C->getWidth() && "Invalid store size!");
		if (StoreBits > C->getWidth()) C = C->ZExt(StoreBits);

		os->write(offset, C);
	}
}

MemoryObject * Executor::addExternalObject(ExecutionState &state, void *addr, unsigned size, bool isReadOnly) {
	MemoryObject *mo = memory->allocateFixed((uint64_t) (unsigned long) addr, size, 0);
	ObjectState *os = bindObjectInState(state, mo, false);
	for (unsigned i = 0; i < size; i++)
		os->write8(i, ((uint8_t*) addr)[i]);
	if (isReadOnly) os->setReadOnly(true);
	return mo;
}

void Executor::initializeGlobals(ExecutionState &state) {
	Module *m = kmodule->module;

	if (m->getModuleInlineAsm() != "")
	LOG(WARNING) << "executable has module level assembly (ignoring)";

	//assert(m->lib_begin() == m->lib_end() &&
	//       "XXX do not support dependent libraries");

	// represent function globals using the address of the actual llvm function
	// object. given that we use malloc to allocate memory in states this also
	// ensures that we won't conflict. we don't need to allocate a memory object
	// since reading/writing via a function pointer is unsupported anyway.
	for (Module::iterator i = m->begin(), ie = m->end(); i != ie; ++i) {
		Function *f = i;
		klee::ref<ConstantExpr> addr(0);

		// If the symbol has external weak linkage then it is implicitly
		// not defined in this module; if it isn't resolvable then it
		// should be null.
		if (f->hasExternalWeakLinkage() && !externalDispatcher->resolveSymbol(f->getName())) {
			addr = Expr::createPointer(0);
		} else {
			addr = Expr::createPointer((unsigned long) (void*) f);
			legalFunctions.insert((uint64_t) (unsigned long) (void*) f);
		}

		globalAddresses.insert(std::make_pair(f, addr));
	}

	// Disabled, we don't want to promote use of live externals.
#ifdef HAVE_CTYPE_EXTERNALS
#ifndef WINDOWS
#ifndef DARWIN
	/* From /usr/include/errno.h: it [errno] is a per-thread variable. */
	int *errno_addr = __errno_location();
	addExternalObject(state, (void *) errno_addr, sizeof *errno_addr, false);

	/* from /usr/include/ctype.h:
	 These point into arrays of 384, so they can be indexed by any `unsigned
	 char' value [0,255]; by EOF (-1); or by any `signed char' value
	 [-128,-1).  ISO C requires that the ctype functions work for `unsigned */
	const uint16_t **addr = __ctype_b_loc();
	addExternalObject(state, (void *) (*addr - 128), 384 * sizeof **addr, true);
	addExternalObject(state, addr, sizeof(*addr), true);

	const int32_t **lower_addr = __ctype_tolower_loc();
	addExternalObject(state, (void *) (*lower_addr - 128), 384 * sizeof **lower_addr, true);
	addExternalObject(state, lower_addr, sizeof(*lower_addr), true);

	const int32_t **upper_addr = __ctype_toupper_loc();
	addExternalObject(state, (void *) (*upper_addr - 128), 384 * sizeof **upper_addr, true);
	addExternalObject(state, upper_addr, sizeof(*upper_addr), true);
#endif
#endif
#endif

	// allocate and initialize globals, done in two passes since we may
	// need address of a global in order to initialize some other one.

	// allocate memory objects for all globals
	for (Module::const_global_iterator i = m->global_begin(), e = m->global_end(); i != e; ++i) {
		if (i->isDeclaration()) {
			// FIXME: We have no general way of handling unknown external
			// symbols. If we really cared about making external stuff work
			// better we could support user definition, or use the EXE style
			// hack where we check the object file information.

			Type *ty = i->getType()->getElementType();
			uint64_t size = kmodule->targetData->getTypeStoreSize(ty);

			// XXX - DWD - hardcode some things until we decide how to fix.
#ifndef WINDOWS
			if (i->getName() == "_ZTVN10__cxxabiv117__class_type_infoE") {
				size = 0x2C;
			} else if (i->getName() == "_ZTVN10__cxxabiv120__si_class_type_infoE") {
				size = 0x2C;
			} else if (i->getName() == "_ZTVN10__cxxabiv121__vmi_class_type_infoE") {
				size = 0x2C;
			}
#endif

			std::string varName = i->getName().str();
			if (size == 0) {
				llvm::errs() << "Unable to find size for global variable: " << varName << " (use will result in out of bounds access)\n";
			}

			MemoryObject *mo = 0;
			if (USC && scGlobalVars.count(i->getName().str())) {
				mo = memory->allocate(&state, size, false, true, i, scGlobalVars[i->getName().str()]);

				if (sboxGlobalMOs.count(i->getName().str())) {
					sboxGlobalMOs[i->getName().str()] = mo;
					if (size > Executor::cacheLineSize) {
						smallSboxes = false;
					}
				}
				std::cerr << std::endl;
			} else {
				mo = memory->allocate(&state, size, false, true, i);
			}
			ObjectState *os = bindObjectInState(state, mo, false);
			globalObjects.insert(std::make_pair(i, mo));
			globalAddresses.insert(std::make_pair(i, mo->getBaseExpr()));

			// Program already running = object already initialized.  Read
			// concrete value and write it to our copy.
			if (size) {
				void *addr;
				if (i->getName() == "__dso_handle") {
					addr = &__dso_handle; // wtf ?
					LOG(INFO) << "__dso_handle we found is at " << addr;
				} else {
					addr = externalDispatcher->resolveSymbol(i->getName());
				}
				if (!addr) {
					LOG(FATAL) << "Unable to load symbol (" << i->getName().data() << ") while initializing globals.";
				}

				for (unsigned offset = 0; offset < mo->size; offset++)
					os->write8(offset, ((unsigned char*) addr)[offset]);
			}
		} else {
			Type *ty = i->getType()->getElementType();
			uint64_t size = kmodule->targetData->getTypeStoreSize(ty);
			MemoryObject *mo = 0;

			if (UseAsmAddresses && i->getName()[0] == '\01') {
				char *end;
				uint64_t address = ::strtoll(i->getName().str().c_str() + 1, &end, 0);

				if (end && *end == '\0') {
					LOG(INFO) << "Allocated global at asm specified address: " << std::hex << std::uppercase << (long long) address << " ("
							<< size << " bytes)";
					mo = memory->allocateFixed(address, size, &*i);
					mo->isUserSpecified = true; // XXX hack;
				}
			}

			if (!mo) {
				if (USC && scGlobalVars.count(i->getName().str())) {
					mo = memory->allocate(&state, size, false, true, i, scGlobalVars[i->getName().str()]);
					if (sboxGlobalMOs.count(i->getName().str())) {
						sboxGlobalMOs[i->getName().str()] = mo;
						if (size > Executor::cacheLineSize) smallSboxes = false;
					}
					std::cerr << std::endl;
				} else {
					mo = memory->allocate(&state, size, false, true, &*i);
				}
			}

			if (!mo)
			LOG(INFO) << "Cannot allocate memory for global " << i->getName().str();
			assert(mo && "out of memory");
			ObjectState *os = bindObjectInState(state, mo, false);
			globalObjects.insert(std::make_pair(i, mo));
			globalAddresses.insert(std::make_pair(i, mo->getBaseExpr()));

			if (!i->hasInitializer()) os->initializeToRandom();
		}
	}

	if (USC && smallSboxes) {
		std::cerr << "[Exit] Sbox size is less than cache line size, no leakage under symsc." << std::endl;
		if (!regular) {
			if (precise) {
				freopen("./log.Inter", "w", stderr);
				inter++;
				std::cerr << inter;
				freopen("./log.Test", "w", stderr);
				std::cerr << test;
			} else {
				freopen("./log.Inter", "w", stderr);
				inter++;
				std::cerr << inter;
				freopen("./log.Fail", "w", stderr);
				fail++;
				std::cerr << fail;
				freopen("./log.Test", "w", stderr);
				std::cerr << test;
			}
			fclose(stderr);
			exit(0);
		}
	}

	// link aliases to their definitions (if bound)
	for (Module::alias_iterator i = m->alias_begin(), ie = m->alias_end(); i != ie; ++i) {
		// Map the alias to its aliasee's address. This works because we have
		// addresses for everything, even undefined functions.
		globalAddresses.insert(std::make_pair(i, evalConstant(i->getAliasee())));
	}

	// once all objects are allocated, do the actual initialization
	for (Module::const_global_iterator i = m->global_begin(), e = m->global_end(); i != e; ++i) {
		if (i->hasInitializer()) {
			MemoryObject *mo = globalObjects.find(i)->second;
			const ObjectState *os = state.addressSpace().findObject(mo);
			assert(os);
			ObjectState *wos = state.addressSpace().getWriteable(mo, os);

			initializeGlobalObject(state, wos, i->getInitializer(), 0, mo);
		}
	}
}

void Executor::parseGlobalInfo(ExecutionState& state, Module::const_global_iterator& global, MemoryObject* mo) {
	const string name = global->getName().str();
	mo->setName(name);

	std::string moduleFullName = kmodule->module->getModuleIdentifier();
	std::string moduleShortName = moduleFullName.substr(18, moduleFullName.size() - 18);

	if (moduleShortName.substr(0, sizeof("nxt.") - 1) == "nxt.") {
		if (name.substr(0, sizeof("PLC_CELL1__") - 1) != "PLC_CELL1__") {
			return;
		}
	}

	std::cerr << "global var, name: " << name << std::endl;

	StructType *st = dyn_cast<StructType>(global->getType()->getElementType());
	if (st) {
		uint64_t address = mo->address;
		this->parseGlobalStructInfo(state, st, name, address);
	} else {
		Type* ty = global->getType()->getElementType();
		unsigned width = kmodule->targetData->getTypeStoreSizeInBits(ty);
		std::pair<uint64_t, unsigned> pair = std::make_pair(mo->address, width);
		this->globalStatefulVars[name] = pair;
	}
}

void Executor::parseGlobalStructInfo(ExecutionState& state, StructType* st, string name, uint64_t baseAddr) {
	const StructLayout *sl = kmodule->targetData->getStructLayout(st);
	std::string moduleFullName = kmodule->module->getModuleIdentifier();
	std::string moduleShortName = moduleFullName.substr(18, moduleFullName.size() - 18);

	for (unsigned i = 0; i < st->getNumElements(); i++) {
		if (st->getName() == "struct.PROGA") {
			if (moduleShortName == "g4ltl1.bc") {
				if (i <= 6) continue;
			} else if (moduleShortName == "g4ltl7.bc") {
				if (i == 0) continue;
				if (i == 1) continue;
			} else if (moduleShortName == "g4ltl9.bc") {
				if (i < 9) continue;
			} else if (moduleShortName == "g4ltl10.bc") {
				if (i < 9) continue;
			} else if (moduleShortName == "ex10.bc") {
				if (i < 9) continue;
			} else if (moduleShortName == "iec2.bc") {
				if (i == 7) continue;
				if (i == 8) continue;
				if (i == 9) continue;
			} else if (moduleShortName == "iec3.bc") {
				if (i > 10) continue;
			} else if (moduleShortName == "iec4.bc") {
				if (i == 3) continue;
				if (i == 4) continue;
			} else if (moduleShortName == "iec7.bc") {
				if (i > 11) continue;
			} else if (moduleShortName == "ld-prog2.bc") {
				if (i < 3) continue;
				if (i > 13) continue;
			} else if (moduleShortName == "ld-prog4.bc") {
				if (i == 19) continue;
			} else if (moduleShortName == "mixer.bc") {
				if (i < 13) continue;
				if (i > 53) continue;
			} else if (moduleShortName == "evaporator.bc") {
				if (i < 13) continue;
			} else if (moduleShortName == "lift.bc") {
				if (i < 10) continue;
			} else if (moduleShortName == "plastic.bc") {
				if (i <= 10) continue;
				if (i == 30) continue;
				if (i == 31) continue;
				if (i == 32) continue;
				if (i == 33) continue;
			} else if (moduleShortName == "traffic.bc") {
				if (i < 5) continue;
				if (i == 8) continue;
				if (i == 9) continue;
			} else if (moduleShortName == "bargraph.bc") {
				if (i <= 1) continue;
				if (i == 4) continue;
				if (i == 5) continue;
				if (i == 6) continue;
			} else if (moduleShortName == "startstop.bc") {
				if (i > 4) continue;
			} else if (moduleShortName == "shutter.bc") {
				if (i <= 13) continue;
			} else if (moduleShortName == "alarm.bc") {
				if (i < 5) continue;
			}
		} else if (st->getName() == "struct.FB_G4LTL") {
			if (moduleShortName == "g4ltl1.bc") {
				if (i < 9) continue;
			} else if (moduleShortName == "g4ltl7.bc") {
				if (i == 2) continue;
				if (i == 3) continue;
			} else if (moduleShortName == "g4ltl9.bc") {
				if (i < 11) continue;
			} else if (moduleShortName == "g4ltl10.bc") {
				if (i < 11) continue;
			} else if (moduleShortName == "ex10.bc") {
				if (i < 11) continue;
				if (i > 21) continue;
			}
		} else if (st->getName() == "struct.TON") {
			if (i != 4) continue;
		} else if (st->getName() == "struct.SR") {
			continue;
		} else if (st->getName() == "struct.CTU") {
			continue;
		} else if (st->getName() == "struct.CMD_MONITOR") {
			if (i == 0) continue;
			if (i == 1) continue;
			if (moduleShortName == "iec2.bc") {
				if (i == 9) continue;
				if (i == 10) continue;
			} else if (moduleShortName == "iec3.bc") {
				if (i == 9) continue;
				if (i == 10) continue;
			}
		} else if (st->getName() == "struct.FWD_REV_MON") {
			if (i == 0) continue;
			if (i == 1) continue;
			if (i > 13) continue;
		} else if (st->getName() == "struct.STACK_INT") {
			if (i < 13) continue;
		} else if (st->getName() == "struct.RAMPP") {
			continue;
		} else if (st->getName() == "struct.TRANSFERR") {
			if (i == 1) continue;
			if (i == 2) continue;
			if (i == 3) continue;
			if (i > 7) continue;
		}

		Type* elementType = st->getElementType(i);
		uint64_t offset = sl->getElementOffset(i);
		uint64_t base = baseAddr;

		if (dyn_cast<StructType>(elementType)) {
			StructType* st1 = cast<StructType>(elementType);
			if (this->iec_basic_types.count(st->getName())) {
				string elementName = name + "_" + boost::lexical_cast<std::string>(0);
				base += offset;
				unsigned width = kmodule->targetData->getTypeStoreSizeInBits(st1->getElementType(0));
				this->globalStatefulVars[elementName] = std::make_pair(baseAddr, width);
			} else {
				string elementName = name + "_" + boost::lexical_cast<std::string>(i);
				base += offset;
				this->parseGlobalStructInfo(state, st1, name + "_" + boost::lexical_cast<std::string>(i), base);
			}
		} else {
			string elementName = name + "_" + boost::lexical_cast<std::string>(i);
			base += offset;
			unsigned width = kmodule->targetData->getTypeStoreSizeInBits(elementType);
			this->globalStatefulVars[elementName] = std::make_pair(base, width);
		}
	}
}

template<typename TypeIt>
void Executor::computeOffsets(KGEPInstruction *kgepi, TypeIt ib, TypeIt ie) {
	klee::ref<ConstantExpr> constantOffset = ConstantExpr::alloc(0, Context::get().getPointerWidth());
	uint64_t index = 1;
	for (TypeIt ii = ib; ii != ie; ++ii) {
		if (StructType *st = (dyn_cast<StructType>(*ii))) {
			const StructLayout *sl = kmodule->targetData->getStructLayout(st);
			const ConstantInt *ci = cast<ConstantInt>(ii.getOperand());
			uint64_t addend = sl->getElementOffset((unsigned) ci->getZExtValue());
			constantOffset = constantOffset->Add(ConstantExpr::alloc(addend, Context::get().getPointerWidth()));
		} else {
			const SequentialType *set = cast<SequentialType>(*ii);
			uint64_t elementSize = kmodule->targetData->getTypeStoreSize(set->getElementType());
			Value *operand = ii.getOperand();
			if (Constant *c = (dyn_cast<Constant>(operand))) {
				klee::ref<ConstantExpr> index = evalConstant(c)->SExt(Context::get().getPointerWidth());
				klee::ref<ConstantExpr> addend = index->Mul(ConstantExpr::alloc(elementSize, Context::get().getPointerWidth()));
				constantOffset = constantOffset->Add(addend);
			} else {
				kgepi->indices.push_back(std::make_pair(index, elementSize));
			}
		}
		index++;
	}
	kgepi->offset = constantOffset->getSExtValue();
}

void Executor::bindInstructionConstants(KInstruction *KI) {
	KGEPInstruction *kgepi = static_cast<KGEPInstruction*>(KI);

	if (GetElementPtrInst *gepi = (dyn_cast<GetElementPtrInst>(KI->inst))) {
		computeOffsets(kgepi, gep_type_begin(gepi), gep_type_end(gepi));
	} else if (InsertValueInst *ivi = (dyn_cast<InsertValueInst>(KI->inst))) {
		computeOffsets(kgepi, iv_type_begin(ivi), iv_type_end(ivi));
		assert(kgepi->indices.empty() && "InsertValue constant offset expected");
	} else if (ExtractValueInst *evi = (dyn_cast<ExtractValueInst>(KI->inst))) {
		computeOffsets(kgepi, ev_type_begin(evi), ev_type_end(evi));
		assert(kgepi->indices.empty() && "ExtractValue constant offset expected");
	}
}

void Executor::bindModuleConstants() {
	for (std::vector<KFunction*>::iterator it = kmodule->functions.begin(), ie = kmodule->functions.end(); it != ie; ++it) {
		KFunction *kf = *it;
		for (unsigned i = 0; i < kf->numInstructions; ++i)
			bindInstructionConstants(kf->instructions[i]);
	}

	kmodule->constantTable = new Cell[kmodule->constants.size()];
	for (unsigned i = 0; i < kmodule->constants.size(); ++i) {
		Cell &c = kmodule->constantTable[i];
		c.value = evalConstant(kmodule->constants[i]);

		/* Store the name-value mapping of a module-level constant, Daniel*/
		llvm::Constant* constant = kmodule->constants[i];
		if (constant->hasName()) {
			const std::string &name = constant->getName().str();
			kmodule->constantNameValueMap.insert(std::make_pair(name, c.value));
		}
	}
}

ExecutionState *Executor::createRootState(llvm::Function *f) {
	ExecutionState *state = new ExecutionState(this, kmodule->functionMap[f]);

	return state;
}

void Executor::initRootState(ExecutionState *state, int argc, char **argv, char **envp) {
	llvm::Function *f = state->stack().back().kf->function;

	std::vector<klee::ref<Expr> > arguments;

	// force deterministic initialization of memory objects
	srand(1);
	srandom(1);

	MemoryObject *argvMO = 0;

	// In order to make uclibc happy and be closer to what the system is
	// doing we lay out the environments at the end of the argv array
	// (both are terminated by a null). There is also a final terminating
	// null that uclibc seems to expect, possibly the ELF header?

	int envc;
	for (envc = 0; envp[envc]; ++envc)
		;

	unsigned NumPtrBytes = Context::get().getPointerWidth() / 8;
	KFunction *kf = kmodule->functionMap[f];
	assert(kf);
	Function::arg_iterator ai = f->arg_begin(), ae = f->arg_end();
	if (ai != ae) {
		arguments.push_back(ConstantExpr::alloc(argc, Expr::Int32));

		if (++ai != ae) {
			argvMO = memory->allocate(state, (argc + 1 + envc + 1 + 1) * NumPtrBytes, false, true, f->begin()->begin());

			arguments.push_back(argvMO->getBaseExpr());

			if (++ai != ae) {
				uint64_t envp_start = argvMO->address + (argc + 1) * NumPtrBytes;
				arguments.push_back(Expr::createPointer(envp_start));

				if (++ai != ae)
				LOG(FATAL) << "Invalid main function (expect 0-3 arguments)";
			}
		}
	}

	if (pathWriter) state->pathOS = pathWriter->open();
	if (symPathWriter) state->symPathOS = symPathWriter->open();

	if (statsTracker) statsTracker->framePushed(*state, 0);

	assert(arguments.size() == f->arg_size() && "wrong number of arguments");
	for (unsigned i = 0, e = f->arg_size(); i != e; ++i)
		bindArgument(kf, i, *state, arguments[i]);

	if (argvMO) {
		ObjectState *argvOS = bindObjectInState(*state, argvMO, false);

		for (int i = 0; i < argc + 1 + envc + 1 + 1; i++) {
			MemoryObject *arg;

			if (i == argc || i >= argc + 1 + envc) {
				arg = 0;
			} else {
				char *s = i < argc ? argv[i] : envp[i - (argc + 1)];
				int j, len = strlen(s);

				arg = memory->allocate(state, len + 1, false, true, state->pc()->inst);
				ObjectState *os = bindObjectInState(*state, arg, false);
				for (j = 0; j < len + 1; j++)
					os->write8(j, s[j]);
			}

			if (arg) {
				argvOS->write(i * NumPtrBytes, arg->getBaseExpr());
			} else {
				argvOS->write(i * NumPtrBytes, Expr::createPointer(0));
			}
		}
	}
	if (USC) {
		std::cerr << std::endl << "Now map global vars' binary addrs to KLEE's addrs: " << std::endl << std::endl;
		initializeGlobals(*state);
		std::cerr << "============================End Preprocessing===============================" << std::endl << std::endl;
	} else {
		initializeGlobals(*state);
	}

	processTree = new PTree(state);
	state->ptreeNode = processTree->root;

	bindModuleConstants();

	// Delay init till now so that ticks don't accrue during
	// optimization and such.
	initTimers();

	states.insert(state);
}

}
