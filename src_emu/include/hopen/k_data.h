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
 * $Source: /cvs/hopencvs/src/include/hopen/k_data.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2004/12/13 06:19:25 $
 * 
\**************************************************************************/

#ifndef _HOPEN_KDATA_H_
#define	_HOPEN_KDATA_H_

#include <sys/types.h>
#include <hal/atomic.h>

struct task;
struct proc;
struct page_head;
struct addr_wait;

#define	KERNEL_SPACE		0
#define	WAKEUP_ALL		0

#define	MEMCHK_COPYIN		/* check CopyIn address */
#define	MEMCHK_COPYOUT		/* check CopyOut address */

/* Define variables in kernel data structure */
struct kernel_data {
	unsigned long	systimer;	// The system clock is from starting the machine to present time 
	unsigned long	basetime;	// basetime+systime/TICKS_PERSECOND = realtime.
	pid_t		taskid;		// current task id
	char		started;	// Kernel started.
	char		SchedLock;	// If sched locked.

	struct task *	rtQueue;	// realtime task queue
	struct task *	nrQueue;	// common task queue

	int		max_tasks;	// Mumber of items in task table.
	int		max_proces;	// Number of process.

	void *		MemStart;	// Start of kernel memory.
	unsigned long	memsize;	// Total memory size.

	char *		kmem_base;
	int		kmem_pages;
	int		kmem_free_pages;
	int		kmem_max_pageindex;

	/* On 32 bit CPU, the max kernel memory is 4M, use 32 long words
	 * for alloction bitmap.
	 * On 16 bit CPU, the max kernel memory is 256K, use 4 16 bit
	 * words for allocation bitmap. */
#ifndef CPU_16BIT
	unsigned kmem_pagemap[32];
#else
	unsigned kmem_pagemap[4];
#endif
	struct page_head * kmem_free_blks[12];

	struct addr_wait * sleep_hash[SLEEP_HASH_TBLSIZE];	/* sleeping task table  */
};

extern struct kernel_data K_Data;
extern struct proc * proc_table;
extern struct task * task_table;

#endif
