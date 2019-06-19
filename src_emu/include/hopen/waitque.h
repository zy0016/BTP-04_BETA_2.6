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
 * $Source: /cvs/hopencvs/src/include/hopen/waitque.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2004/12/13 06:20:24 $
 * 
\**************************************************************************/

#ifndef _HOPEN_WAIT_H
#define _HOPEN_WAIT_H

#ifdef __KERNEL__

#include <hopen/task.h>

struct wait_queue {
    struct wait_queue * next;		/* next sleeping node */
    struct wait_queue * prev;		/* prev sleeping node */
    struct task *	task;
};

typedef struct wait_queue	wait_queue_t;
typedef struct wait_queue *	wait_queue_head_t;

#define	init_waitqueue(q)	*q = 0
#define	waitqueue_active(q)	(*q != 0)

#define DECLARE_WAIT_QUEUE_HEAD(name)	wait_queue_head_t (name) = (void *)0
#define DECLARE_WAITQUEUE(name)		wait_queue_t (name) = {0, 0, 0}
#define	init_waitqueue_head(queue)	*(queue) = 0

extern void add_wait_queue(wait_queue_head_t * queue, wait_queue_t * wait);
extern void remove_wait_queue(wait_queue_head_t * queue, wait_queue_t * wait);

extern int _KERN_CondSleep (wait_queue_head_t *, int (*)(void * param), void *, int);
extern int _KERN_PollSleep (wait_queue_head_t *, int);
extern int _KERN_PollSleepTimeout (wait_queue_head_t *, long timeout);
extern int _KERN_Wakeup (wait_queue_head_t* pQueue, int error, int number);

#define sleep(q,f,p)			_KERN_CondSleep (q, f, p, 0)
#define sleep_interruptable(q,f,p)	_KERN_CondSleep (q, f, p, SLEEP_INTERRUPTABLE)
#define	wake_up(q)			_KERN_Wakeup (q, 0, -1)

#define start_poll()			current->state = TS_POLL
#define end_poll()			current->state = TS_READY
#define	poll_sleep(q)			_KERN_PollSleep (q, 0)
#define poll_sleep_timeout(q,t)		_KERN_PollSleepTimeout (q, t)
#define poll_sleep_interruptable(q)	_KERN_PollSleep (q, SLEEP_INTERRUPTABLE)

#endif /* __KERNEL__ */

#endif	// _HOPEN_WAIT_H
