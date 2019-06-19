/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef	_POSIX_OPT_H
#define	_POSIX_OPT_H	1

/* Job control is supported.  */
#undef	_POSIX_JOB_CONTROL

/* Processes have a saved set-user-ID and a saved set-group-ID.  */
#undef	_POSIX_SAVED_IDS

/* Priority scheduling is supported.  */
#undef	_POSIX_PRIORITY_SCHEDULING

/* Synchronizing file data is supported.  */
#undef	_POSIX_SYNCHRONIZED_IO

/* The fsync function is present.  */
#define	_POSIX_FSYNC	1

/* Mapping of files to memory is supported.  */
#undef	_POSIX_MAPPED_FILES

/* Locking of all memory is supported.  */
#undef	_POSIX_MEMLOCK

/* Locking of ranges of memory is supported.  */
#undef	_POSIX_MEMLOCK_RANGE

/* Setting of memory protections is supported.  */
#undef	_POSIX_MEMORY_PROTECTION

/* Implementation supports `poll' function.  */
#undef	_POSIX_POLL

/* Implementation supports `select' and `pselect' functions.  */
#undef	_POSIX_SELECT

/* Only root can change owner of file.  */
#undef	_POSIX_CHOWN_RESTRICTED

/* `c_cc' member of 'struct termios' structure can be disabled by
   using the value _POSIX_VDISABLE.  */
#undef	_POSIX_VDISABLE

/* Filenames are not silently truncated.  */
#define	_POSIX_NO_TRUNC	1

/* X/Open realtime support is available.  */
#undef	_XOPEN_REALTIME

/* X/Open realtime thread support is available.  */
#undef	_XOPEN_REALTIME_THREADS

/* XPG4.2 shared memory is supported.  */
#undef	_XOPEN_SHM

/* Tell we have POSIX threads.  */
#define _POSIX_THREADS	1

/* We have the reentrant functions described in POSIX.  */
#define _POSIX_THREAD_SAFE_FUNCTIONS	1

/* We provide priority scheduling for threads.  */
#define	_POSIX_THREAD_PRIORITY_SCHEDULING	1

/* We support user-defined stack sizes.  */
#define _POSIX_THREAD_ATTR_STACKSIZE	1

/* We support user-defined stacks.  */
#define _POSIX_THREAD_ATTR_STACKADDR	1

/* We support POSIX.1b semaphores, but only the non-shared form for now.  */
#undef _POSIX_SEMAPHORES

/* Real-time signals are supported.  */
#define _POSIX_REALTIME_SIGNALS	1

/* We support asynchronous I/O.  */
#undef _POSIX_ASYNCHRONOUS_IO

/* POSIX shared memory objects are implemented.  */
#undef _POSIX_SHARED_MEMORY_OBJECTS

/* CPU-time clocks supported.  */
#undef _POSIX_CPUTIME

/* We support the clock also in threads.  */
#undef _POSIX_THREAD_CPUTIME

/* GNU libc provides regular expression handling.  */
#undef _POSIX_REGEXP

/* Reader/Writer locks are available.  */
#undef _POSIX_READER_WRITER_LOCKS

/* We have a POSIX shell.  */
#undef _POSIX_SHELL

/* We support the Timeouts option.  */
#undef _POSIX_TIMEOUTS

/* We support spinlocks.  */
#undef _POSIX_SPIN_LOCKS

/* The `spawn' function family is supported.  */
#undef _POSIX_SPAWN

/* We have POSIX timers.  */
#define _POSIX_TIMERS	1

/* The barrier functions are available.  */
#undef _POSIX_BARRIERS

/* POSIX message queues are not yet supported.  */
#undef	_POSIX_MESSAGE_PASSING

#endif /* posix_opt.h */
