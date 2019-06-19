/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for Operating System abstract layer.
 *            
\**************************************************************************/

#ifndef __OSAL_H
#define __OSAL_H

int PLXOS_GetTicksPerSecond(void);
unsigned long PLXOS_GetSysClock(void);

void PLXOS_CreateTimerTask(void);
unsigned long PLXOS_GetElapsedTime(BOOL bUpdate);

BOOL PLXOS_InitTimerEvent(void);
void PLXOS_SetTimerEvent(DWORD dwNewEscape);
void PLXOS_SetTimerout(DWORD dwEscape);
void PLXOS_WaitTimerEvent(void);

/* Define the mutual object with various of OS */
#if ((_HOPEN_VER <=200) && !defined(_LINUX_OS_))
typedef OSHANDLE SEMAPHOREOBJ;
typedef MUTEX MUTEXOBJ;

#define SEMAPHORE_CREATE(sem)       {sem = OS_CreateSemaphore(NULL, 1);}
#define SEMAPHORE_DESTROY(sem)      OS_DestroySemaphore(sem)
#define SEMAPHORE_OPEN(sem, name)   {sem = OS_OpenSemaphore(name);}
#define SEMAPHORE_WAIT(sem)         OS_WaitSemaphore(sem)
#define SEMAPHORE_RELEASE(sem)      OS_ReleaseSemaphore(sem, 1)

#define MUTEX_CREATE(mut)        OS_CreateMutex(&(mut), 0)
#define MUTEX_DESTROY(mut)       OS_DestroyMutex(&(mut))
#define MUTEX_WAIT(mut)          OS_WaitMutex(&(mut))
#define MUTEX_RELEASE(mut)       OS_ReleaseMutex(&(mut))

#elif ((_HOPEN_VER >= 300) || defined(_LINUX_OS_))
#include "unistd.h"
#include <sys/ipc.h>
#include <pthread.h>

#if (__MP_PLX_GUI)
#ifdef _EMULATE_
#include "importms.h"

typedef HANDLE SEMAPHOREOBJ;
typedef HANDLE MUTEXOBJ;

#define SEMAPHORE_CREATE(sem, name) {sem = EMU_CreateSemaphore(name, 1);}
#define SEMAPHORE_DESTROY(sem)      EMU_DestroySemaphore(sem)
#define SEMAPHORE_OPEN(sem, name)   {sem = EMU_OpenSemaphore(name);}
#define SEMAPHORE_WAIT(sem)         EMU_WaitSemaphore(sem, INFINITE)
#define SEMAPHORE_RELEASE(sem)      EMU_ReleaseSemaphore(sem, 1)

#define MUTEX_CREATE(mut)           {mut = EMU_CreateMutex();}
#define MUTEX_DESTROY(mut)          EMU_DestroyMutex(mut)
#define MUTEX_WAIT(mut)             EMU_WaitMutex(mut)
#define MUTEX_RELEASE(mut)          EMU_ReleaseMutex(mut)

#else //_EMULATE_
typedef int SEMAPHOREOBJ;
typedef pthread_mutex_t MUTEXOBJ;

#define SEMAPHORE_CREATE(sem)       {sem = CreateSemaphore(NULL, 1);}
#define SEMAPHORE_DESTROY(sem)      DestroySemaphore(sem)
#define SEMAPHORE_OPEN(sem, key)    {sem = OpenSemaphore(key);}
#define SEMAPHORE_WAIT(sem)         WaitSemaphore(sem, -1)
#define SEMAPHORE_RELEASE(sem)      ReleaseSemaphore(sem, 1)

#define MUTEX_CREATE(mut)        pthread_mutex_init(&(mut), NULL)
#define MUTEX_DESTROY(mut)       pthread_mutex_destroy(&(mut))
#define MUTEX_WAIT(mut)          pthread_mutex_lock(&(mut))
#define MUTEX_RELEASE(mut)       pthread_mutex_unlock(&(mut))

#endif //_EMULATE_
#else //__MP_PLX_GUI
typedef int SEMAPHOREOBJ;
typedef pthread_mutex_t MUTEXOBJ;
typedef int EVENTOBJ;

#define SEMAPHORE_CREATE(sem)       {sem = CreateSemaphore(0, 1);}
#define SEMAPHORE_DESTROY(sem)      DestroySemaphore(sem)
#define SEMAPHORE_OPEN(sem, key)    {sem = OpenSemaphore(key);}
#define SEMAPHORE_WAIT(sem)         WaitSemaphore(sem, -1)
#define SEMAPHORE_RELEASE(sem)      ReleaseSemaphore(sem, 1)

#define MUTEX_CREATE(mut)        pthread_mutex_init(&(mut), NULL)
#define MUTEX_DESTROY(mut)       pthread_mutex_destroy(&(mut))
#define MUTEX_WAIT(mut)          pthread_mutex_lock(&(mut))
#define MUTEX_RELEASE(mut)       pthread_mutex_unlock(&(mut))

/*
#if defined(_LINUX_OS_)
#define EVENT_CREATE(evt)        {evt = OS_CreateEvent(0, 0);} //无名自动复位
#define EVENT_DESTROY(evt)       OS_DestroyEvent(evt)
#define EVENT_WAIT(evt, timeout) OS_WaitEvent(evt, timeout)
#define EVENT_SET(evt)           OS_SignalEvent(evt, 1) //事件置位,唤醒线程
#else*/

#define EVENT_CREATE(evt)        {evt = CreateEvent(0, 0);} //无名自动复位
#define EVENT_DESTROY(evt)       DestroyEvent(evt)
#define EVENT_WAIT(evt, timeout) WaitEvent(evt, timeout)
#define EVENT_SET(evt)           SetEvent(evt, 1) //事件置位,唤醒线程
//#endif
#endif // __MP_PLX_GUI
#endif // _HOPEN_VER

#define MAX_ULONG    0xFFFFFFFFL
#define MAX_LONG     0x7FFFFFFFL
#if (_C166)
#define MAX_UINT     0xFFFFL
#define MAX_INT      0x7FFFL
#else
#define MAX_UINT     0xFFFFFFFFL
#define MAX_INT      0x7FFFFFFFL
#endif

#endif // __OSAL_H
