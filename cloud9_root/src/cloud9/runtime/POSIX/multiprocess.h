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

#ifndef THREADS_H_
#define THREADS_H_

#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <klee/klee.h>

#include "common.h"
#include "signals.h"

typedef uint64_t wlist_id_t;

#define DEFAULT_THREAD  0

#define DEFAULT_PROCESS 2
#define DEFAULT_PARENT  1
#define DEFAULT_UMASK   (S_IWGRP | S_IWOTH)

#define PID_TO_INDEX(pid)   ((pid) - 2)
#define INDEX_TO_PID(idx)   ((idx) + 2)

#define PTHREAD_ONCE_INIT       0

#define STATIC_MUTEX_VALUE      0
#define STATIC_CVAR_VALUE       0
#define STATIC_BARRIER_VALUE    0
#define STATIC_RWLOCK_VALUE     0

#define PTHREAD_BARRIER_SERIAL_THREAD    -1

////////////////////////////////////////////////////////////////////////////////
// System Wide Data Structures
////////////////////////////////////////////////////////////////////////////////

typedef struct {
  wlist_id_t wlist;
  wlist_id_t children_wlist;

  pid_t parent;
  mode_t umask;

  int ret_value;

  char allocated;
  char terminated;
#ifdef HAVE_POSIX_SIGNALS
  char signaled;
  sigset_t blocked;
  struct sighand_struct *sighand;
  struct sigpending pending;
#endif
} proc_data_t;

extern proc_data_t __pdata[MAX_PROCESSES];

typedef struct {
  // Taken from the semaphore specs
  unsigned short semval;
  unsigned short semzcnt;
  unsigned short semncnt;
  pid_t sempid;
} sem_t;

typedef struct {
  struct semid_ds descriptor;
  sem_t *sems;

  char allocated;
} sem_set_t;

extern sem_set_t __sems[MAX_SEMAPHORES];

////////////////////////////////////////////////////////////////////////////////
// Process Specific Data Structures
////////////////////////////////////////////////////////////////////////////////

typedef struct {
  wlist_id_t wlist;

  void *ret_value;

  char allocated;
  char terminated;
  char joinable;
} thread_data_t;

typedef struct {
  wlist_id_t wlist;

  char taken;
  unsigned int owner;
  int count;
  unsigned int queued;

  char allocated;
} mutex_data_t;

typedef struct {
  wlist_id_t wlist;

  mutex_data_t *mutex;
  unsigned int queued;
} condvar_data_t;

typedef struct {
  wlist_id_t wlist;

  unsigned int curr_event;
  unsigned int left;
  unsigned int init_count;
} barrier_data_t;

typedef struct {
  wlist_id_t wlist_readers;
  wlist_id_t wlist_writers;

  unsigned int nr_readers;
  unsigned int nr_readers_queued;
  unsigned int nr_writers_queued;
  int writer;
} rwlock_data_t;
typedef struct {
  wlist_id_t wlist;

  int count;
  char allocated;
  int thread_level;
  pid_t owner;
} sem_data_t;

typedef struct {
  thread_data_t threads[MAX_THREADS];
} tsync_data_t;

extern tsync_data_t __tsync;

// defined by Daniel.
//typedef struct{
//	unsigned tids[MAX_THREADS];
//}id_map_t;
//id_map_t id_map;
//static unsigned tid = 1;

void klee_init_processes(void);
void klee_init_threads(void);

/*
 * Wrapper over the klee_thread_preempt() call.
 * This is done to simulate checking for received
 * signals when being first planned.
 */
static inline void __thread_preempt(int yield) {
  klee_thread_preempt(yield);
#ifdef HAVE_POSIX_SIGNALS
  if((&__pdata[PID_TO_INDEX(getpid())])->signaled)
      __handle_signal();
#endif
}

/*
 * Wrapper over the klee_thread_sleep() call.
 * This is done to simulate checking for received
 * signals when being first planned.
 */
static inline void __thread_sleep(uint64_t wlist) {
  klee_thread_sleep(wlist);
#ifdef HAVE_POSIX_SIGNALS
  if((&__pdata[PID_TO_INDEX(getpid())])->signaled)
      __handle_signal();
#endif
}

static inline void __thread_notify(uint64_t wlist, int all) {
  klee_thread_notify(wlist, all);
}

static inline void __thread_notify_one(uint64_t wlist) {
  __thread_notify(wlist, 0);
}

static inline void __thread_notify_all(uint64_t wlist) {
  __thread_notify(wlist, 1);
}

static inline void __thread_mutex_unlock_end(uint64_t mutex){
	klee_mutex_unlock_end(mutex);
}
#endif /* THREADS_H_ */
