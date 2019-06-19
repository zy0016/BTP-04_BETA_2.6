/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Operating System abstract layer for PLX GUI System.
 *            
\**************************************************************************/

#include "hpwin.h"

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)
#include "string.h"
#include "unistd.h"
#if defined(_LINUX_OS_)
#include "pthread.h"
#else   // _LINUX_OS_
#include "sys/task.h"
#endif  // _LINUX_OS_
#else   // _HOPEN_VER < 300
#include "hp_kern.h"
#endif  // _HOPEN_VER

//#include "wstimer.h"
#include "osal.h"

#if defined(_LINUX_OS_)
#include "sys/times.h"
/*unsigned long GetSysClock(void)
{
    struct tms buf;
    
    return times(&buf);
}*/

int	GetTicksPerSecond( void )
{
    return sysconf(_SC_CLK_TCK);

}
#endif

#if (_HOPEN_VER >= 300)
#define OS_GetSysClock			GetSysClock
#define OS_GetTicksPerSecond	GetTicksPerSecond

unsigned long OS_GetSysClock( void );
int	OS_GetTicksPerSecond( void );
#endif

extern void WS_TimerTask(void);


/***************************************************************
 * var definition and func declaration.
 ***************************************************************/
#if defined(_LINUX_OS_)
#define SLEEP_MILLS     100
static pthread_t timer_thr;
static unsigned long lSysClock;
static void* timer_task(void* param);

#elif (_HOPEN_VER >= 300)

#if !defined(NO_TIMER_THREAD)
#define SLEEP_MILLS     100
static unsigned long lSysClock;
static int timer_task(void* param);
#endif  // NO_TIMER_THREAD

#endif  // _LINUX_OS_


/***************************************************************
 * Create a thread to run timer task periodically.
 ***************************************************************/
void PLXOS_CreateTimerTask(void)
{
#if defined(_LINUX_OS_)

    pthread_create(&timer_thr, NULL, timer_task, NULL);

#elif (_HOPEN_VER >= 300)

#if !defined(NO_TIMER_THREAD)
    #define STACKSIZE   4096
    struct taskcreate   cdata;

    memset(&cdata, 0, sizeof(cdata));
    //strncpy(cdata.name, (char *)"timer_t", 8);
    cdata.prio = 254;
    cdata.flags = 0;
    cdata.entry = timer_task;
    cdata.param = 0;
    cdata.stkTop = (void *)MemAlloc(STACKSIZE);
    if (cdata.stkTop == NULL)
    {
        return;
    }
    cdata.stkTop = (BYTE*)cdata.stkTop + STACKSIZE;

    if (CreateTaskEx(&cdata) <= 0)
    {
        MemFree((char *)(cdata.stkTop) - STACKSIZE);
    }
#endif  // NO_TIMER_THREAD

#else   // _HOPEN_VER < 300

    OS_SetTickProc(WS_TimerTask, 1);

#endif  // _HOPEN_VER
}


/***************************************************************
 * Thread entry for timer-task thread.
 ***************************************************************/
#if defined(_LINUX_OS_)
static void* timer_task(void* param)
{
    lSysClock = 0;

    while (1)
	{
        usleep(SLEEP_MILLS * 1000);
        lSysClock++;

        WS_TimerTask();
    }
}
#elif (_HOPEN_VER >= 300)
#if !defined(NO_TIMER_THREAD)
static int timer_task(void* param)
{
    while (1)
    {
        WS_TimerTask();
    }

    EndTask(0, 0);
}
#endif  // NO_TIMER_THREAD & _HOPEN_VER
#endif  // _LINUX_OS_


/***************************************************************
 * Emulation for GetTicksPerSecond & GetSysClock.
 ***************************************************************/
int PLXOS_GetTicksPerSecond(void)
{
#if defined(_LINUX_OS_)
    return GetTicksPerSecond();
//    return (1000 / SLEEP_MILLS);
#elif (_HOPEN_VER >= 300)
    return GetTicksPerSecond();
#else   // _HOPEN_VER < 300
    return OS_GetTicksPerSecond();
#endif  // _LINUX_OS_
}

unsigned long PLXOS_GetSysClock(void)
{
#if defined(_LINUX_OS_)
    return GetSysClock();
//    return lSysClock;
#elif (_HOPEN_VER >= 300)
    return GetSysClock();
#else   // _HOPEN_VER < 300
    return OS_GetSysClock();
#endif  // _LINUX_OS_
}


/***************************************************************
 * Get elapsed time since last timer-task-running.
 ***************************************************************/
unsigned long PLXOS_GetElapsedTime(BOOL bUpdate)
{
    unsigned long lTimeElapsed;

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    int nTicksPerSecond;
    unsigned long lTickCur;
    unsigned long lTickElapsed;
    static unsigned long lTickPrev;
    static BOOL bFirst = TRUE;

    lTickCur = GetSysClock();
    if (bFirst)
    {
        lTickPrev = lTickCur - 1;
        bFirst = FALSE;
    }
    lTickElapsed = lTickCur - lTickPrev;

    if (bUpdate)
        lTickPrev = lTickCur;

    if (lTickElapsed < 1)
        return 0;

    nTicksPerSecond = GetTicksPerSecond();
    lTimeElapsed = (lTickElapsed * 1000 + nTicksPerSecond / 2) 
        / nTicksPerSecond;

#else   // _HOPEN_VER < 300

    int nTicksPerSecond;

    nTicksPerSecond = OS_GetTicksPerSecond();
    lTimeElapsed = (1000 + nTicksPerSecond / 2) / nTicksPerSecond;

#endif  // _HOPEN_VER

    return lTimeElapsed;
}

#if ((_HOPEN_VER >= 300) || defined(_LINUX_OS_))
static EVENTOBJ timer_event;
static LONG timer_timeout;
#elif (_VME)
static EVENTOBJ timer_event;
static LONG timer_timeout;
#endif
BOOL PLXOS_InitTimerEvent(void)
{
#if ((_HOPEN_VER >= 300) || defined(_LINUX_OS_))
    EVENT_CREATE(timer_event);
#elif (_VME)
#endif
    return TRUE;
}
void PLXOS_SetTimerEvent(DWORD dwNewEscape)
{
    if (dwNewEscape > (DWORD)timer_timeout)
        return;

#if ((_HOPEN_VER >= 300) || defined(_LINUX_OS_))
    EVENT_SET(timer_event);
#elif (_VME)
    VME_StopTimer(timer_event);
    timer_timeout = WS_TimerTask();
    timer_event = VME_StartTimerMilliSecond(timer_timeout, SYS_TIMER_SUB_EVENT);
#endif
}

void PLXOS_SetTimerout(DWORD dwEscape)
{
    timer_timeout = (LONG)dwEscape;
    if (timer_timeout < 0)
        timer_timeout = MAX_LONG;
}

void PLXOS_WaitTimerEvent(void)
{
#if ((_HOPEN_VER >= 300) || defined(_LINUX_OS_))
    EVENT_WAIT(timer_event, timer_timeout);
#elif (_VME)
    timer_event = VME_StartTimerMilliSecond(timer_timeout, SYS_TIMER_SUB_EVENT);
#endif
}
