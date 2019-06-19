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
 * $Source: /cvs/hopencvs/src/include/hopen/mutex.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2004/12/13 06:19:23 $
 * 
\**************************************************************************/

#ifndef _HOPEN_MUTEX_H
#define	_HOPEN_MUTEX_H

#include <hal/atomic.h>
#include <hopen/waitque.h>

typedef struct mutex {
	atomic_t		m_lock;		/* lock? */
	unsigned char	m_count;	/* recursion count */
	unsigned char	m_attrib;	/* attribute,for example,whether recurse*/
	unsigned char	m_wait;		/* Some thread is waiting for mutex */
	unsigned char	m_unused;
	struct task *	m_pTask;	/* owner thread */
	struct wait_queue * m_queue;	/* waiting queue */
} mutex;

#define	MUTEX_VALID		0x80
#define	MUTEX_NOREENT		0x02

extern int _KERN_CreateMutex (struct mutex * pMutex);
extern int _KERN_WaitMutex (struct mutex * pMutex);
extern int _KERN_ReleaseMutex(struct mutex * pMutex);

#define	init_mutex(m)		_KERN_CreateMutex(m)
#define	wait_mutex(m)		_KERN_WaitMutex(m)
#define release_mutex(m)	_KERN_ReleaseMutex(m)

#endif
