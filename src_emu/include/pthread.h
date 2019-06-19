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

#ifndef _PTHREAD_H
#define _PTHREAD_H

#include <sys/types.h>
#include <sys/mutex.h>


#define PTHREAD_KEYS_MAX	16
#define PTHREAD_STACK_MIN	4096

#define PTHREAD_READY		1
#define PTHREAD_DEAD		2


/* Data structure to describe a process' schedulability.  */
struct sched_param
{
	int __sched_priority;
};

/* Scheduling algorithms.  */
#define SCHED_OTHER	0
#define SCHED_FIFO	1
#define SCHED_RR	2


/* Attributes for threads.  */
typedef struct __pthread_attr_s
{
	int __detachstate;
	int __schedpolicy;
	int __schedparam;
	void *__stackaddr;
	size_t __stacksize;
} pthread_attr_t;

/* Values for pthread attributes.  */
#define PTHREAD_CREATE_JOINABLE		0
#define PTHREAD_CREATE_DETACHED		1

#define PTHREAD_CANCELED ((void *) -1)


/* Conditions (not abstract because of PTHREAD_COND_INITIALIZER */
typedef int pthread_cond_t;


/* Attribute for conditionally variables.  */
typedef struct
{
	int __dummy;
} pthread_condattr_t;


/* Keys for thread-specific data */
typedef unsigned int pthread_key_t;


/* Mutexes (not abstract because of PTHREAD_MUTEX_INITIALIZER).  */
/* (The layout is unnatural to maintain binary compatibility
with earlier releases of LinuxThreads.) */
typedef struct mutex pthread_mutex_t;


/* Attribute for mutex.  */
typedef struct
{
	int __mutexkind;
} pthread_mutexattr_t;

/* Values for pthread mutex attributes.  */
#define	PTHREAD_MUTEX_FAST_NP		0
#define	PTHREAD_MUTEX_RECURSIVE_NP	MUTEX_REENT


/* Thread identifiers */
typedef int pthread_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Function for handling threads.  */

/* Create a thread with given attributes ATTR (or default attributes
if ATTR is NULL), and call function START_ROUTINE with given
arguments ARG.  */
int pthread_create (pthread_t *thread,
			   const pthread_attr_t *attr,
			   void *(*start_routine) (void *),
			   void *arg);

/* Obtain the identifier of the current thread.  */
pthread_t pthread_self (void);

/* Compare two thread identifiers.  */
int pthread_equal (pthread_t thread1, pthread_t thread2);

/* Terminate calling thread.  */
void pthread_exit (void *retval);

/* Make calling thread wait for termination of the thread TH.  The
exit status of the thread is stored in *THREAD_RETURN, if THREAD_RETURN
is not NULL.  */
int pthread_join (pthread_t th, void **thread_return);

/* Indicate that the thread TH is never to be joined with PTHREAD_JOIN.
   The resources of TH will therefore be freed immediately when it
   terminates, instead of waiting for another thread to perform PTHREAD_JOIN
   on it.  */
int pthread_detach(pthread_t th);


/* Functions for handling attributes.  */

/* Initialize thread attribute *ATTR with default attributes
(detachstate is PTHREAD_JOINABLE, scheduling policy is SCHED_OTHER,
no user-provided stack).  */
int pthread_attr_init (pthread_attr_t *attr);

/* Destroy thread attribute *ATTR.  */
int pthread_attr_destroy (pthread_attr_t *attr);

/* Set the `detachstate' attribute in *ATTR according to DETACHSTATE.  */
int pthread_attr_setdetachstate (pthread_attr_t *attr,
					int detachstate);

/* Return in *DETACHSTATE the `detachstate' attribute in *ATTR.  */
int pthread_attr_getdetachstate (const pthread_attr_t *attr,
					int *detachstate);

/* Set scheduling parameters (priority, etc) in *ATTR according to PARAM.  */
int pthread_attr_setschedparam (pthread_attr_t *attr,
				       const struct sched_param *param);

/* Return in *PARAM the scheduling parameters of *ATTR.  */
int pthread_attr_getschedparam (const pthread_attr_t *attr,
				       struct sched_param * param);

/* Set the starting address of the stack of the thread to be created.
Depending on whether the stack grows up or down the value must either
be higher or lower than all the address in the memory block.  The
minimal size of the block must be PTHREAD_STACK_SIZE.  */
int pthread_attr_setstackaddr (pthread_attr_t *attr,
				      void *stackaddr);

/* Return the previously set address for the stack.  */
int pthread_attr_getstackaddr (const pthread_attr_t *attr, void **stackaddr);

/* Add information about the minimum stack size needed for the thread
to be started.  This size must never be less than PTHREAD_STACK_SIZE
and must also not exceed the system limits.  */
int pthread_attr_setstacksize (pthread_attr_t *attr,
				      size_t stacksize);

/* Return the currently used minimal stack size.  */
int pthread_attr_getstacksize (const pthread_attr_t *attr, size_t *stacksize);


/* Functions for scheduling control.  */

#ifdef __USE_GNU
/* Yield the processor to another thread or process.
This function is similar to the POSIX `sched_yield' function but
might be differently implemented in the case of a m-on-n thread
implementation.  */
int pthread_yield (void);
#endif


/* Functions for mutex handling.  */

/* Initialize MUTEX using attributes in *MUTEX_ATTR, or use the
default values if later is NULL.  */
int pthread_mutex_init (pthread_mutex_t *mutex,
			       const pthread_mutexattr_t *mutex_attr);

/* Destroy MUTEX.  */
int pthread_mutex_destroy (pthread_mutex_t *mutex);

/* Try to lock MUTEX.  */
int pthread_mutex_trylock (pthread_mutex_t *mutex);

/* Wait until lock for MUTEX becomes available and lock it.  */
int pthread_mutex_lock (pthread_mutex_t *mutex);

/* Unlock MUTEX.  */
int pthread_mutex_unlock (pthread_mutex_t *mutex);


/* Functions for handling mutex attributes.  */

/* Initialize mutex attribute object ATTR with default attributes
(kind is PTHREAD_MUTEX_TIMED_NP).  */
int pthread_mutexattr_init (pthread_mutexattr_t *attr);

/* Destroy mutex attribute object ATTR.  */
int pthread_mutexattr_destroy (pthread_mutexattr_t *attr);

/* Set the mutex kind attribute in *ATTR to KIND (either PTHREAD_MUTEX_FAST_NP,
or PTHREAD_MUTEX_RECURSIVE_NP).  */
int pthread_mutexattr_setkind_np (pthread_mutexattr_t *attr, int kind);

/* Return in *KIND the mutex kind attribute in *ATTR.  */
int pthread_mutexattr_getkind_np (const pthread_mutexattr_t *attr, int *kind);


/* Functions for handling conditional variables.  */

/* Initialize condition variable COND using attributes ATTR, or use
the default values if later is NULL.  */
int pthread_cond_init (pthread_cond_t *cond,
			      const pthread_condattr_t *cond_attr);

/* Destroy condition variable COND.  */
int pthread_cond_destroy (pthread_cond_t *cond);

/* Wake up one thread waiting for condition variable COND.  */
int pthread_cond_signal (pthread_cond_t *cond);

/* Wake up all threads waiting for condition variables COND.  */
int pthread_cond_broadcast (pthread_cond_t *cond);

/* Wait for condition variable COND to be signaled or broadcast.
MUTEX is assumed to be locked before.  */
int pthread_cond_wait (pthread_cond_t *cond,
			      pthread_mutex_t *mutex);


/* Functions for handling condition variable attributes.  */

/* Initialize condition variable attribute ATTR.  */
int pthread_condattr_init (pthread_condattr_t *attr);

/* Destroy condition variable attribute ATTR.  */
int pthread_condattr_destroy (pthread_condattr_t *attr);


/* Functions for handling thread-specific data.  */

/* Create a key value identifying a location in the thread-specific
   data area.  Each thread maintains a distinct thread-specific data
   area.  DESTR_FUNCTION, if non-NULL, is called with the value
   associated to that key when the key is destroyed.
   DESTR_FUNCTION is not called if the value associated is NULL when
   the key is destroyed.  */
int pthread_key_create (pthread_key_t *key,
			       void (*destr_function) (void *));

/* Destroy KEY.  */
int pthread_key_delete (pthread_key_t key);

/* Store POINTER in the thread-specific data slot identified by KEY. */
int pthread_setspecific (pthread_key_t key,
				const void *pointer);

/* Return current value of the thread-specific data slot identified by KEY.  */
void *pthread_getspecific (pthread_key_t key);


#ifdef __cplusplus
}
#endif

#endif	/* pthread.h */
