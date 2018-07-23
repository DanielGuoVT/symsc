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

#ifndef THREADING_H_
#define THREADING_H_

#include "klee/Expr.h"
#include "klee/Internal/Module/KInstIterator.h"
#include "../../lib/Core/AddressSpace.h"

#include "llvm/ADT/ilist.h"

#include <map>

namespace klee {

struct KFunction;
struct KInstruction;
class ExecutionState;
class Process;
class CallPathNode;
struct Cell;

typedef uint64_t thread_id_t;
typedef uint64_t thread_priority_t;
typedef uint64_t thread_period_t;
typedef uint64_t process_id_t;
typedef uint64_t wlist_id_t;

typedef std::pair<thread_id_t, process_id_t> thread_uid_t;

struct StackFrame {
	KInstIterator caller;
	KFunction *kf;
	CallPathNode *callPathNode;

	std::vector<const MemoryObject*> allocas;
	Cell *locals;

	/// Minimum distance to an uncovered instruction once the function
	/// returns. This is not a good place for this but is used to
	/// quickly compute the context sensitive minimum distance to an
	/// uncovered instruction. This value is updated by the StatsTracker
	/// periodically.
	unsigned minDistToUncoveredOnReturn;

	// For vararg functions: arguments not passed via parameter are
	// stored (packed tightly) in a local (alloca) memory object. This
	// is setup to match the way the front-end generates vaarg code (it
	// does not pass vaarg through as expected). VACopy is lowered inside
	// of intrinsic lowering.
	MemoryObject *varargs;

	StackFrame(KInstIterator caller, KFunction *kf);
	StackFrame(const StackFrame &s);

	StackFrame& operator=(const StackFrame &sf);
	~StackFrame();
};

class Thread {
	friend class Executor;
	friend class ExecutionState;
	friend class Process;
private:

	KInstIterator pc, prevPC, prevPrevPC;
	unsigned incomingBBIndex;

	std::vector<StackFrame> stack;
public:
	bool enabled;
	thread_priority_t priority;
	thread_period_t period;
	// for tapPLC
	int crtPeriodIndex;
	bool reachBoundaryStart;
	bool highestPrio;
private:
	wlist_id_t waitingList;
	thread_uid_t tuid;

///////////////////////////////variables added by shengjian guo///////////////////////
	//for play out all interleavings
	bool reachInterleavingPoint;
	std::set<uint64_t> lockset;

	std::string locationStr;
	uint64_t uniqueID;
	bool sleeping;
	uint64_t lastTransitionType;
	uint64_t lastTransitionAddr;

///////////////////////////////end of the variables///////////////////////////////////

public:
	Thread(thread_id_t tid, process_id_t pid, KFunction *start_function);

	thread_uid_t getUid() const {
		return tuid;
	}
	;
	thread_id_t getTid() const {
		return tuid.first;
	}
	process_id_t getPid() const {
		return tuid.second;
	}
};

}

#endif /* THREADING_H_ */
