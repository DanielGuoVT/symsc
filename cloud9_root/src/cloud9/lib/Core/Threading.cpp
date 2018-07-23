/*
 * Cloud9 Parallel Symbolic Execution Engine
 *
 * Copyright (c) 2011, Dependable Systems Laboratory, EPFL
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Dependable Systems Laboratory, EPFL nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE DEPENDABLE SYSTEMS LABORATORY, EPFL BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * All contributors are listed in CLOUD9-AUTHORS file.
 *
 */

#include "klee/Threading.h"
#include "klee/ExecutionState.h"
#include "klee/Internal/Module/KModule.h"
#include "klee/Internal/Module/Cell.h"

namespace klee {

/* StackFrame Methods */

StackFrame::StackFrame(KInstIterator _caller, KFunction *_kf) :
		caller(_caller), kf(_kf), callPathNode(0), minDistToUncoveredOnReturn(0), varargs(0) {
	locals = new Cell[kf->numRegisters];
}

StackFrame::StackFrame(const StackFrame &s) :
		caller(s.caller), kf(s.kf), callPathNode(s.callPathNode), allocas(s.allocas), minDistToUncoveredOnReturn(s.minDistToUncoveredOnReturn), varargs(
				s.varargs) {
	locals = new Cell[s.kf->numRegisters];
	for (unsigned i = 0; i < s.kf->numRegisters; i++)
		locals[i] = s.locals[i];
}

StackFrame& StackFrame::operator=(const StackFrame &s) {
	if (this != &s) {
		caller = s.caller;
		kf = s.kf;
		callPathNode = s.callPathNode;
		allocas = s.allocas;
		minDistToUncoveredOnReturn = s.minDistToUncoveredOnReturn;
		varargs = s.varargs;

		if (locals) delete[] locals;

		locals = new Cell[s.kf->numRegisters];
		for (unsigned i = 0; i < s.kf->numRegisters; i++)
			locals[i] = s.locals[i];
	}

	return *this;
}

StackFrame::~StackFrame() {
	delete[] locals;
}

/* Thread class methods */

Thread::Thread(thread_id_t tid, process_id_t pid, KFunction * kf) :
		enabled(true), waitingList(0), reachInterleavingPoint(false), crtPeriodIndex(0), priority(0), period(0), uniqueID(0){

	tuid = std::make_pair(tid, pid);
	reachBoundaryStart = false;
	highestPrio = false;
	lastTransitionType = ExecutionState::null;
	lastTransitionAddr = 0;

	if (kf) {
		stack.push_back(StackFrame(0, kf));
		pc = kf->instructions;
		prevPC = pc;
	}

}

}
