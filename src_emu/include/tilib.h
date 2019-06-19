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

#ifndef _TILIB_H
#define _TILIB_H

#include <pthread.h>

#ifndef NULL
#define NULL	((void* )0)
#endif

#ifndef TRUE
#define FALSE	0
#define TRUE	1
#endif

#ifndef SYSERR
#define OK		0
#define SYSERR	-1
#endif

#define IPADDRLEN	4

typedef struct apptimer 
{
		struct apptimer * pNext;
		struct apptimer * pPrev;
		char	state;
		char	flags;
		short	unused;
		int		expire;
		void (* func)(void * para);
		void  * para;
		int		value;
} APPTIMER, * PAPPTIMER;

#define INVALID_SOCKET	-1

#define GMEM_FIXED		1

typedef struct mutex MUTEX;

#ifdef __cplusplus
extern "C" {
#endif

int  LIB_CreateTimer( PAPPTIMER pTimer ); 
int  LIB_StartTimer( PAPPTIMER pTimer );
int  LIB_StopTimer( PAPPTIMER pTimer );
int  LIB_SetTimerFunc( PAPPTIMER pTimer, void (*func)(void * para), void * para );
int  LIB_SetTimerExpire( PAPPTIMER pTimer, int value );
int  LIB_QueryTimerEx( PAPPTIMER pTimer );
int  LIB_DestroyTimer( PAPPTIMER pTimer );

int LIB_CreateMutex( MUTEX* pMu );
int LIB_WaitMutex( MUTEX* pMu );
int LIB_SignalMutex( MUTEX* pMu );
int LIB_DestroyMutex( MUTEX* pMu );
int LIB_PostMessage( void* hd, unsigned long msg, unsigned long wP, unsigned long lP );
void* M_GlobalAlloc( int att, int size );
int M_GlobalFree( void* p );
int LIB_GetSysClock();
int LIB_GetTicksPerSecond();
int LIB_GetTaskID();
int LIB_SetError( int err );
int LIB_CreateEvent();
int LIB_WaitEvent( int handle );
int LIB_SignalEvent( int handle, int mode );
int LIB_DestroyEvent( int handle );
int LIB_CreateTask(char * pname, unsigned int prio,
		void* (*entry)(void *), unsigned long stksize, void * para );
unsigned short  get16(unsigned char *cp);
unsigned long  get32(unsigned char *cp);
unsigned char * put16(unsigned char *cp, unsigned short x);
unsigned char * put32(unsigned char *cp, unsigned long x);

#ifdef __cplusplus
}
#endif
#endif
