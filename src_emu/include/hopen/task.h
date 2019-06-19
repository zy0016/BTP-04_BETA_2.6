/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
 * Author  :     WangXinshe
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/hopen/task.h,v $
 * $Name:  $
 *
 * $Revision: 1.4 $     $Date: 2004/12/13 06:20:18 $
 * 
\**************************************************************************/

#ifndef _HOPEN_TASK_H_
#define	_HOPEN_TASK_H_

#include <hal/hal.h>
#include <sys/types.h>
#include <hopen/timer.h>

struct proc;
struct wait_queue;
struct monitor;

/* PCB */
typedef struct task {
	struct task *	pLink;		
	pid_t		taskid;		/* task ID */
	pid_t		procid;		/* Process ID */
	pid_t		parent;         /* Parent task ID */
	pid_t		waitpid;	/* waiting task */
	unsigned char	state;		/* task state */
	unsigned char	baseprio;	/* basal prior */
	unsigned short	flags;		/* flag */

	struct proc *	proc;		/* pointer to PCB */
	char *		pKernelStack;	/* kernel stack*/

	unsigned long	signal;		/* signal */
	unsigned long   sigMask;	/* signal mask */

	signed	int	currprio;	/* current prior */
	signed	int	timeslice;	/* timeslice */

	/* These four fields are only for debug */
	unsigned long	cputimes;	/* cpu time */
	unsigned long   userFrame;	/* user stack for interrupt and syscall */
	unsigned long	userPC;		/* user PC for interrupt and syscall */ 
	void *		waitAddr;	/* waiting queue for this process */

	struct monitor *monitor;	/* If this thread is in monitor, point to
					 * monitor structure */
	unsigned long   it_interval;	/* Interval timer value */
	struct timer_list itimer;	/* Alarm timer of this task */

	pid_t		tracer;		/* Debuger task */
	char		addrCheck;	/* check copyin,copyout address? */
	char		unused_1;
	short		error;		/* Error number of system call. */
	short		exit_code;	/* Exit code of this task. */

	/* Temp variables used by kernel */
	union {
		int	i;
		void *	p;
	} temp[4];
	hal_context	hal_context;	/* context for task */
} task;

/* value for task state */
#define TS_READY	        1
#define	TS_POLL			2	/* Task is in poll mode */
#define	TS_INTERRUPTABLE	4	/* Sleeping task can be interrupted by SIGNAL */
//#define TS_TRACED               5       /* Task is in trace mode */
#define TS_STOPED               6       /* Task is stoped by SIGSTOP */
#define TS_SLEEP	        7	/* Sleeping task can not be interrupted by SIGNAL */
#define TS_DEAD			8	/* dead tatk */

/* value for task flags */
#define TF_REALTIME     0x0200		/* real time task */
#define TF_KERNEL       0x0400          /* Kernel task */
#define TF_NOPREEMPT	0x0800		/* Tasks within same process no preempt. */
#define TF_TRACE	0x1000		/* Task is traced */
#define TF_FLAGMASK     0x7F00          /* Fixed task flag mask */

#define TF_SIGSUSPEND	0x0010		/* Task is doning sigsuspend */

/* for PTRACE flags*/
#define PF_PTRACED	TF_TRACE	/* 0x1000, means Task is traced */
#define PF_TRACESYS	0x2000		/* tracing system calls */
#define PF_DTRACE	0x4000		/* delayed trace*/


/* The task enters the sleep state */
#define	SLEEP_NOINTERRUPT	0
#define	SLEEP_INTERRUPTABLE	TS_INTERRUPTABLE
#define SLEEP_STOPED            TS_STOPED

int _KERN_CreateTask (void (*entry)(void), int flags, int prio);

struct task * _KERN_FindTask(int taskid );

int  _KERN_CondSleep (struct wait_queue **, int (*)(void * param), void *, int);
int  _KERN_Wakeup (struct wait_queue **, int, int);

void _KERN_EnqueueTask (struct task * pTask);
void _KERN_DequeueTask (struct task * pTask);

extern struct task * current;

#endif
