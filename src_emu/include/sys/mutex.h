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
 * $Source: /cvs/hopencvs/src/include/sys/mutex.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2004/02/27 06:16:06 $
 * 
\**************************************************************************/

#ifndef _SYS_MUTEX_H
#define	_SYS_MUTEX_H

typedef struct mutex {
	unsigned int   	m_lock;
	signed char		m_count;
	unsigned char	m_attrib;
    unsigned char   m_wait;
	unsigned char	m_unused;
	unsigned short  m_taskid;
} mutex;

#define	MUTEX_REENT		0x02

#ifdef __cplusplus
extern "C" {
#endif

int __WaitMutex(struct mutex * pMutex, int tryflag);
int __ReleaseMutex(struct mutex * pMutex);

void WaitCondVar (int * condvar, struct mutex * mutex);
void WakeupCondVar ( int * condvar );

int CreateMutex(struct mutex * pMutex, int attrib);
int ReleaseMutex(struct mutex * pMutex);
int DestroyMutex(struct mutex * pMutex);

#ifdef __cplusplus
}
#endif

#define TryWaitMutex(pMutex)	__WaitMutex(pMutex, 1)
#define WaitMutex(pMutex)	__WaitMutex(pMutex, 0)

#endif  // _SYS_MUTEX
