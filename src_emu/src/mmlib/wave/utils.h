/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : utils.h
 *
 * Purpose  : Declare os compatible module
 *
\**************************************************************************/

#ifndef HAVE_UTILS_H
#define HAVE_UTILS_H

#include <window.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

//	TODO	:	DEFINE DATA STRUCT
#ifdef WIN32_OS
#include <winbase.h>

typedef	HANDLE              HTHREAD;
typedef	HANDLE              HMUTEX;
typedef	HANDLE              HSEMP;

typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(
    LPVOID lpThreadParameter
    );
typedef PTHREAD_START_ROUTINE THREAD_FUNC;

#endif // WIN32_OS

#ifdef POLLEX_OS
#include "sys/ipc.h"
#include "pthread.h"
#include "unistd.h"

typedef	pthread_t           HTHREAD;
typedef	pthread_mutex_t     HMUTEX;
typedef void*               THREAD_FUNC;
typedef	int                 HSEMP;

#endif // POLLEX_OS

#ifdef LINUX_OS
#include "pthread.h"
#include "semaphore.h"
#include "unistd.h"

typedef	pthread_t           HTHREAD;
typedef	pthread_mutex_t     HMUTEX;
typedef void*               THREAD_FUNC;
typedef	sem_t               HSEMP;

#endif // LINUX_OS

typedef struct ut_entry_s
{
	BOOL (*f_Operate )( struct ut_entry_s* hThis, int nCode,
		unsigned long nOne, unsigned long nTwo );

} UT_ENTRY, *HUT_ENTRY, **PHUT_ENTRY;

//	TODO	: DEFINE MODULE PROPER MACRO
#define	OSM_USER            0x1000  // 对象定义操作
#define	OSM_RELEASE         0x0001  // 对象被释放
#define	OSM_INITIALIZE      0x0002  // 对象被初始化

#define	MKTAG(a,b,c,d)      (a | (b << 8) | (c << 16) | (d << 24))

//	TODO	:	DECLARE OUTPUT FUNCTION
#ifdef __cplusplus
extern "C" {
#endif

void*	mmi_Malloc	( unsigned int nSize );
void*	mmi_Realloc	( void* pd, unsigned int nSize );
void	mmi_Free	( void* pVoid );
void*	mmi_Set		( void *pd, int n, unsigned int size_t );
void*   mmi_Copy	( void *pd, const void *ps, unsigned int size_t );
void*	mmi_Move	( void *pd, const void *ps, unsigned int size_t );

DWORD	f_GetTick	( void );
void	f_Sleep		( long nTime );

#ifdef __cplusplus
}
#endif

#include "uthread.h"
#include "umutex.h"
#include "usemp.h"

#endif // HAVE_UTILS_H
