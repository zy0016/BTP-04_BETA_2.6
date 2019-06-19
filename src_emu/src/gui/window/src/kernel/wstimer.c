/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements timer support function :
 *            SetTimer
 *            KillTimer
 *            
\**************************************************************************/

/*
** to realize the multithread window system.
*/
#include "hpwin.h"
#include "osal.h"
#include "wsobj.h"
#include "wsownd.h"
#include "wsthread.h"
#include "wstimer.h"
#include "wscaret.h"
#include "wssysmsg.h"

static COMMONMUTUALOBJ sem_timer;

#if (__MP_PLX_GUI)
#ifdef _EMULATE_

#define SEMNAME_TIMER       "$$PLXGUI_TIMER$$"
#define TIMER_CREATEOBJ(sem)        CREATEOBJ(sem, SEMNAME_TIMER)
#define TIMER_DESTROYOBJ(sem)       DESTROYOBJ(sem, SEMNAME_TIMER)
#define TIMER_WAITOBJ(sem)          WAITOBJ(sem, SEMNAME_TIMER)
#define TIMER_RELEASEOBJ(sem)       RELEASEOBJ(sem, SEMNAME_TIMER)
#else
#define TIMER_CREATEOBJ(sem)        CREATEOBJ(sem)
#define TIMER_DESTROYOBJ(sem)       DESTROYOBJ(sem)
#define TIMER_WAITOBJ(sem)          WAITOBJ(sem)
#define TIMER_RELEASEOBJ(sem)       RELEASEOBJ(sem)
#endif
#else
#define TIMER_CREATEOBJ(sem)        CREATEOBJ(sem)
#define TIMER_DESTROYOBJ(sem)       DESTROYOBJ(sem)
#define TIMER_WAITOBJ(sem)          WAITOBJ(sem)
#define TIMER_RELEASEOBJ(sem)       RELEASEOBJ(sem)
#endif

typedef struct tagTIMERSTRUCT
{
    UINT        uTimerId;        /* The id of the timer in the system */
    BYTE        bUsed;          /* 0 : not used; 1 : general timer; 
                                ** 2 : system timer
                                */
    BYTE        nSlot;          /* The slot number in the thread info */
    DWORD       dwThreadId;     /* The thread id of the owner thread */
    DWORD        uMiliSeconds;   
    DWORD       uElapse;
} TIMERSTRUCT, *PTIMERSTRUCT;

static TIMERSTRUCT TimerList[MAX_TIMERS];

// Internal function prototypes
static UINT CreateTimer(DWORD dwThreadId, DWORD uElapse, BOOL bSystem, 
                        int nSlot);
static UINT UpdateTimer(UINT uTimerId, DWORD uElapse, BOOL bSystem, int nSlot);
static BOOL DestroyTimer(UINT uTimerId);

#ifdef WINDEBUG
#define OBJ_TIMER    12      /* Menu Object          */
extern void WINDEBUG_AddObj(int uType);
extern void WINDEBUG_RemoveObj(int uType);
#endif

/*
**  Function : SetTimer
**  Purpose  :
**      Creates a timer with the specified time-out value. If this 
**      parameter is NULL, no window is associated with the timer and the
**      nIDEvent parameter is ignored. 
**  Params   :
**      hWnd       : Identifies the window to be associated with the timer. 
**      uIDEvent   : Specifies a nonzero timer identifier. If the hWnd 
**                   parameter is NULL, this parameter is ignored. 
**      uElapse    : Specifies the time-out value, in milliseconds. 
**      pTimerFunc : Points to the function to be notified when the time-out
**                   value elapses. 
**  Return   :
**      If the function succeeds, return an integer identifying the new 
**      timer. An application can pass this value, or the string 
**      identifier, if it exists, to the KillTimer function to destroy 
**      the timer. If the function fails to create a timer, return zero.
*/
UINT WINAPI SetTimer(HWND hWnd, UINT uIDEvent, DWORD uElapse, 
                     TIMERPROC pTimerFunc)
{
    UINT uRet;
    PWINOBJ pWin;

    if (hWnd != NULL)
    {
        pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
        if (pWin == NULL)
        {
            SetLastError(1);
            return 0;
        }
    }
    else
        pWin = NULL;

    ENTERMONITOR;

    uRet = WS_CreateTimer(pWin, uIDEvent, uElapse, pTimerFunc, FALSE);

    LEAVEMONITOR;

    return uRet;
}

/*
**  Function : SetSystemTimer
**  Purpose  :
**      Creates a timer with the specified time-out value. If this 
**      parameter is NULL, no window is associated with the timer and the
**      nIDEvent parameter is ignored. 
**  Params   :
**      hWnd       : Identifies the window to be associated with the timer. 
**      uIDEvent   : Specifies a nonzero timer identifier. If the hWnd 
**                   parameter is NULL, this parameter is ignored. 
**      uElapse    : Specifies the time-out value, in milliseconds. 
**      pTimerFunc : Points to the function to be notified when the time-out
**                   value elapses. 
**  Return   :
**      If the function succeeds, return an integer identifying the new 
**      timer. An application can pass this value, or the string 
**      identifier, if it exists, to the KillTimer function to destroy 
**      the timer. If the function fails to create a timer, return zero.
*/
UINT WINAPI SetSystemTimer(HWND hWnd, UINT uIDEvent, DWORD uElapse, 
                           TIMERPROC pTimerFunc)
{
    UINT uRet;
    PWINOBJ pWin;

    if (hWnd != NULL)
    {
        pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
        if (pWin == NULL)
        {
            SetLastError(1);
            return 0;
        }
    }
    else
        pWin = NULL;
    
    ENTERMONITOR;

    uRet = WS_CreateTimer(pWin, uIDEvent, uElapse, pTimerFunc, TRUE);

    LEAVEMONITOR;

    return uRet;
}

/*
**  Function : KillTimer
**  Purpose  :
**      Destroys the specified timer. 
**  Params   :
**      hWnd     : Identifies the window associated with the specified 
**                 timer. This value must be the same as the hWnd value
**                 passed to the SetTimer function that created the timer. 
**      uIDEvent : Specifies the timer to be destroyed. If the window 
**                 handle passed to SetTimer is valid, this parameter must
**                 be the same as the uIDEvent value passed to SetTimer. 
**                 If the application calls SetTimer with hWnd set to NULL,
**                 this parameter must be the timer identifier returned by
**                 SetTimer. 
**  Return   :
**      If the function succeeds, return nonzero. If the function fails, 
**      return zero. To get extended error information, call GetLastError. 
**  Remark   :
**      The KillTimer function does not remove WM_TIMER messages already 
**      posted to the message queue. 
*/
BOOL WINAPI KillTimer(HWND hWnd, UINT uIDEvent)
{
    BOOL bRet;
    PWINOBJ pWin;
    
    if (hWnd != NULL)
    {
        pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
        if (pWin == NULL)
        {
            SetLastError(1);
            return FALSE;
        }
    }
    else
        pWin = NULL;
    
    ENTERMONITOR;

    bRet = WS_DestroyTimer(pWin, uIDEvent, FALSE);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : KillSystemTimer
**  Purpose  :
**      Destroys the specified system timer. 
**  Params   :
**      hWnd     : Identifies the window associated with the specified 
**                 timer. This value must be the same as the hWnd value
**                 passed to the SetTimer function that created the timer. 
**      uIDEvent : Specifies the timer to be destroyed. If the window 
**                 handle passed to SetTimer is valid, this parameter must
**                 be the same as the uIDEvent value passed to SetTimer. 
**                 If the application calls SetTimer with hWnd set to NULL,
**                 this parameter must be the timer identifier returned by
**                 SetTimer. 
**  Return   :
**      If the function succeeds, return nonzero. If the function fails, 
**      return zero. To get extended error information, call GetLastError. 
**  Remark   :
**      The KillTimer function does not remove WM_TIMER messages already 
**      posted to the message queue. 
*/
BOOL WINAPI KillSystemTimer(HWND hWnd, UINT uIDEvent)
{
    BOOL bRet;
    PWINOBJ pWin;
    
    if (hWnd != NULL)
    {
        pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
        if (pWin == NULL)
        {
            SetLastError(1);
            return FALSE;
        }
    }
    else
        pWin = NULL;
    
    ENTERMONITOR;

    bRet = WS_DestroyTimer(pWin, uIDEvent, TRUE);

    LEAVEMONITOR;

    return bRet;
}

// Three functions used in internal

/*
**  Function : WS_KillWindowTimer
**  Purpose  :
**      Kills all timer of a specified window, this function will be called
**      by DestroyWindow function.
*/
void WS_KillWindowTimer(PWINOBJ pWin)
{
    HWND hWnd;
    int i, j, k;
    PTHREADINFO pThreadInfo;
    PTIMERMSGQUE pTimerQue;

    ASSERT(pWin != NULL && WND_IsWindow(pWin));

    hWnd = (HWND)WOT_GetHandle((PWSOBJ)pWin);
    pThreadInfo = WS_GetThreadInfo(pWin->dwThreadId);
    pTimerQue = &pThreadInfo->TimerMsgQue;

    for (i = 0; i < MAX_THREAD_TIMERS; i++)
    {
        j = i / 32;
        k = i % 32;
        if (pTimerQue->TimerInfo[i].hWnd == hWnd &&
            (pTimerQue->dwSlotMask[j] & (1L << k)))
        {
            DestroyTimer(pTimerQue->TimerInfo[i].TimerId);

            pTimerQue->dwSlotMask[j] &= ~(1L << k);
            pTimerQue->dwActiveMask[j] &= ~(1L << k);
            pTimerQue->TimerInfo[i].hWnd = NULL;
            pTimerQue->TimerInfo[i].uEventId = 0;
            pTimerQue->TimerInfo[i].TimerId = 0;
        }
    }
}

/*
**  Function : WS_GetTimerMessage
*/
BOOL WS_GetTimerMessage(PMSG pMsg)
{
    BOOL bActive = FALSE;
    int i, j, k;
    PTHREADINFO pThreadInfo;
    PTIMERMSGQUE pTimerQue;

    ASSERT(pMsg != NULL);

    pThreadInfo = WS_GetCurrentThreadInfo();
    pTimerQue = &pThreadInfo->TimerMsgQue;

    for (j = 0; j <= (MAX_THREAD_TIMERS - 1) / 32; j ++)
    {
        if (pTimerQue->dwActiveMask[j] != 0)
        {
            bActive = TRUE;
            break;
        }
    }
    if (!bActive)
    {
        pThreadInfo->dwState &= ~QS_TIMER;
        return FALSE;
    }

    i = pTimerQue->uPreStart;
    while (1)
    {
        j = i / 32;
        k = i % 32;
        if ((pTimerQue->dwSlotMask[j] & (1L << k)) && 
            (pTimerQue->dwActiveMask[j] & (1L << k)))
        {
            pMsg->hwnd    = pTimerQue->TimerInfo[i].hWnd;
            pMsg->wParam  = pTimerQue->TimerInfo[i].uEventId;
            pMsg->lParam  = (LPARAM)pTimerQue->TimerInfo[i].pTimerProc;

            if (pTimerQue->TimerInfo[i].type == SYSTEM_TIMER)
                pMsg->message = WM_SYSTIMER;
            else
                pMsg->message = WM_TIMER;

            pTimerQue->dwActiveMask[j] &= ~(1L << k);
            bActive = FALSE;

            pTimerQue->uPreStart = ++i;
            if (pTimerQue->uPreStart == MAX_THREAD_TIMERS)
                pTimerQue->uPreStart = 0;

            for (j = 0; j <= (MAX_THREAD_TIMERS - 1) / 32; j ++)
            {
                if (pTimerQue->dwActiveMask[j] != 0)
                {
                    bActive = TRUE;
                    break;
                }
            }
            if (!bActive)
                pThreadInfo->dwState &= ~QS_TIMER;

            return TRUE;
        }
        if (++i == MAX_THREAD_TIMERS)
            i = 0;
        if ((UINT)i == pTimerQue->uPreStart)
            break;
    }

    /* 可能被抢占，不一定还为0 */
//    ASSERT(pTimerQue->dwActiveMask == 0);
    for (j = 0; j <= (MAX_THREAD_TIMERS - 1) / 32; j ++)
    {
        if (pTimerQue->dwActiveMask[j] != 0)
            return FALSE;
    }
    
    pThreadInfo->dwState &= ~QS_TIMER;
    return FALSE;
}

extern DWORD LongKeyTask(UINT uTimerTick);

/*
**  Function : WS_TimerTask
**  Purpose  :
**      Timer task function, which is called once 50 milisecond.
*/
#ifdef _WINBOND_

#include "w90210.h"

__asm (".EXPORT WS_TimerTask,ENTRY,PRIV_LEV=3,ARGW0=NO,ARGW1=NO,ARGW2=NO,ARGW3=NO,RTNVAL=NO" );
__asm ( "\nWS_TimerTask" );
__asm ( ".PROC" );
__asm ( ".CALLINFO FRAME=0,NO_CALLS" );
//      "	.ENTRY\n");
ENTER_USERDOMAIN
LEAVE_USERDOMAIN
//__asm (".EXIT");
__asm (".PROCEND");

static void PLXUserHandler(void)

#else

DWORD WS_TimerTask(void)

#endif

{
    DWORD dwTimeout, dwLKTimeout;
    int i, j, k;
    DWORD uTimerTick;
    PTHREADINFO pThreadInfo;
    BOOL bInMonitor = FALSE;

    TIMER_WAITOBJ(sem_timer);

    dwTimeout = MAX_ULONG;
    uTimerTick = PLXOS_GetElapsedTime(TRUE);

    for (i = 0; i < MAX_TIMERS; i++)
    {
        if (TimerList[i].bUsed)
        {
            if (TimerList[i].uMiliSeconds <= uTimerTick)
            {
                j = TimerList[i].nSlot / 32;
                k = TimerList[i].nSlot % 32;
                // 窗口任务处于阻塞状态, 发送消息唤醒窗口任务
                if (!bInMonitor)
                {
                    ENTERMONITOR;
                    bInMonitor = TRUE;
                }

                pThreadInfo = WS_GetThreadInfo(TimerList[i].dwThreadId);
                ASSERT(pThreadInfo != NULL);
                ASSERT(pThreadInfo->TimerMsgQue.dwSlotMask[j] & (1L << k));
                pThreadInfo->TimerMsgQue.dwActiveMask[j] |= (1L << k);

                pThreadInfo->dwState |= QS_TIMER;
                if (ISBLOCKINGET(pThreadInfo))
                {
                    pThreadInfo->bBlock = BLOCK_NO;
                    WAKEUP(pThreadInfo);
                }

                TimerList[i].uMiliSeconds = TimerList[i].uElapse;
            }
            else
            {
                TimerList[i].uMiliSeconds -= uTimerTick;
            }
            
            if (TimerList[i].uMiliSeconds < dwTimeout)
                dwTimeout = TimerList[i].uMiliSeconds;
        }
    }

    if (bInMonitor)
    {
        bInMonitor = FALSE;
        LEAVEMONITOR;
    }

    /* 需要在互斥内设置新的等待时间, 在互斥外进行等待,
    ** 这样在Timer线程互斥外运行时,如果SetTimer重新设置等待时间时,将进行时间比较.
    ** 1.如果Timer线程即将进入等待,那么一定与最新的等待时间比较
    ** 2.如果Timer线程刚出等待,即将执行TimerTask,那么会与上次等待时间比较,但最多
    **   是Timer线程又执行一遍,不会错误.
    */

    /* the long key task should update the timer task */
    dwLKTimeout = LongKeyTask(uTimerTick);
    if (dwLKTimeout < dwTimeout)
        dwTimeout = dwLKTimeout;

    /* 
    ** 如果dwTimerout为0，那么这里会导致一直运行，这里给出一个最小值 
    */
    if (dwTimeout < 50)
        dwTimeout = 50;
    PLXOS_SetTimerout(dwTimeout);
    TIMER_RELEASEOBJ(sem_timer);

    PLXOS_WaitTimerEvent();
    return dwTimeout;
}

/*********************************************************************\
* Function	   WS_InitTimerSem
* Purpose      Initial the mutual object of timer task.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL WS_InitTimerSem(void)
{
    if (!PLXOS_InitTimerEvent())
        return FALSE;

    TIMER_CREATEOBJ(sem_timer);

    return TRUE;
}

/*********************************************************************\
* Function	   WS_DestroyTimerSem
* Purpose      Destroy the mutual object of the timer task.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void WS_DestroyTimerSem(void)
{
    TIMER_DESTROYOBJ(sem_timer);

    return ;
}

/*********************************************************************\
* Function	   WS_CreateTimer
* Purpose      Creates a timer with the specified time-out value. 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
UINT WS_CreateTimer(PWINOBJ pWin, UINT uIDEvent, DWORD uElapse, 
                        TIMERPROC pTimerFunc, BOOL bSystem)
{
    int i, j, k;
    HWND hWnd;
    PTHREADINFO pThreadInfo;
    PTIMERMSGQUE pTimerQue;

    if (pWin != NULL && uIDEvent == 0)
    {
        SetLastError(1);
        return 0;
    }

    /* 如果hWnd为空，uIDEvent参数将被忽略，仍然创建一个Timer，由系统决定ID */
    if (pWin != NULL)
    {
        /* the window must be owned by the calling thread */
        if (!WND_IsWindow(pWin) || pWin->dwThreadId != WS_GetCurrentThreadId())
        {
            SetLastError(1);
            return 0;
        }

        hWnd = WOT_GetHandle((PWSOBJ)pWin);

        pThreadInfo = WS_GetThreadInfo(pWin->dwThreadId);
        pTimerQue = &pThreadInfo->TimerMsgQue;

        for (i = 0; i < MAX_THREAD_TIMERS; i ++)
        {
            if (pTimerQue->TimerInfo[i].hWnd == hWnd && 
                pTimerQue->TimerInfo[i].uEventId == uIDEvent)
            {
                /* 已经创建，使用新的替换旧的 */
                pTimerQue->TimerInfo[i].pTimerProc = pTimerFunc;
                pTimerQue->TimerInfo[i].type = 
                    bSystem ? SYSTEM_TIMER : GENERAL_TIMER;
                UpdateTimer(pTimerQue->TimerInfo[i].TimerId, uElapse, bSystem, i);

                if (uElapse == 0)
                {
                    /* 
                    ** If the escape time equal zero, 
                    ** the timer message is generated immediately.
                    */
                    j = i / 32;
                    k = i % 32;
                    pTimerQue->dwActiveMask[j] |= (1L << k);
                    pThreadInfo->dwState |= QS_TIMER;
                    if (ISBLOCKINGET(pThreadInfo))
                    {
                        pThreadInfo->bBlock = BLOCK_NO;
                        WAKEUP(pThreadInfo);
                    }
                }

                return uIDEvent;
            }
        }
    }
    else
    {
        hWnd = NULL;
        pThreadInfo = WS_GetCurrentThreadInfo();
        pTimerQue = &pThreadInfo->TimerMsgQue;
    }

    for (i = 0; i < MAX_THREAD_TIMERS; i ++)
    {
        j = i / 32;
        k = i % 32;
        if ((pTimerQue->dwSlotMask[j] & (1L << k)) == 0)
            break;
    }

    if (i == MAX_THREAD_TIMERS)
    {
        SetLastError(1);
        return 0;
    }

    /* 这里需要先设置标志，然后在CreateTimer中就启动了Timer。返回后可能直接调度
    ** 到TimerTask，应该能够设置该Timer。该标志是联系系统timer数据结构和线程
    ** Timer数据结构的数据项
    */
    pTimerQue->dwSlotMask[j] |= 1L << k;
    
    pTimerQue->TimerInfo[i].TimerId = 
        CreateTimer(pThreadInfo->dwThreadId, uElapse, bSystem, i);
    if (pTimerQue->TimerInfo[i].TimerId == 0)
    {
        pTimerQue->dwSlotMask[j] &= ~(1L << k);
        SetLastError(1);
        return 0;
    }

    pTimerQue->TimerInfo[i].hWnd = hWnd;
    pTimerQue->TimerInfo[i].pTimerProc = pTimerFunc;
    pTimerQue->TimerInfo[i].type = bSystem ? SYSTEM_TIMER : GENERAL_TIMER;

    if (hWnd == NULL)
    {
        pTimerQue->TimerInfo[i].uEventId = pTimerQue->TimerInfo[i].TimerId;
    }
    else
    {
        pTimerQue->TimerInfo[i].uEventId = uIDEvent;
    }

    if (uElapse == 0)
    {
        /* 
        ** If the escape time equal zero, 
        ** the timer message is generated immediately.
        */
        pTimerQue->dwActiveMask[j] |= (1L << k);
        pThreadInfo->dwState |= QS_TIMER;
        if (ISBLOCKINGET(pThreadInfo))
        {
            pThreadInfo->bBlock = BLOCK_NO;
            WAKEUP(pThreadInfo);
        }
    }
    
    return pTimerQue->TimerInfo[i].uEventId;

}

/*********************************************************************\
* Function	   DestroyTimer
* Purpose      Destroys the specified timer. 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL WS_DestroyTimer(PWINOBJ pWin, UINT uIDEvent, BOOL bSystem)
{
    HWND hWnd;
    PTHREADINFO pThreadInfo;
    PTIMERMSGQUE pTimerQue;
    int i, j, k;

    if (pWin != NULL)
    {
        if (!WND_IsWindow(pWin))
        {
            SetLastError(1);
            return FALSE;
        }

        hWnd = WOT_GetHandle((PWSOBJ)pWin);
        pThreadInfo = WS_GetThreadInfo(pWin->dwThreadId);
    }
    else
    {
        hWnd = NULL;
        pThreadInfo = WS_GetCurrentThreadInfo();
    }

    pTimerQue = &pThreadInfo->TimerMsgQue;

    for (i = 0; i < MAX_THREAD_TIMERS; i ++)
    {
        if (pTimerQue->TimerInfo[i].hWnd == hWnd && 
            pTimerQue->TimerInfo[i].uEventId == uIDEvent)
        {
            ASSERT(hWnd != NULL || pTimerQue->TimerInfo[i].TimerId == uIDEvent);
            if (DestroyTimer(pTimerQue->TimerInfo[i].TimerId))
            {
                j = i / 32;
                k = i % 32;
                pTimerQue->dwSlotMask[j] &= ~(1L << k);
                pTimerQue->dwActiveMask[j] &= ~(1L << k);
                pTimerQue->TimerInfo[i].hWnd = NULL;
                pTimerQue->TimerInfo[i].uEventId = 0;
                pTimerQue->TimerInfo[i].TimerId = 0;
                return TRUE;
            }
        }
    }

    return FALSE;

}

// Internal function
#define MAKETIMERID(i)      (((DWORD)(i + 1)) << 16)
#define GETTIMERINDEX(id)   (((id) >> 16) - 1)
/*********************************************************************\
* Function	    CreateTimer
* Purpose       Create a timer in the timer task.
* Params	   
* Return	 	The timer id various in the global system.
* Remarks	   The timer is registered in the global timer task. All the
               timers in the system should be created with this routine.
**********************************************************************/
static UINT CreateTimer(DWORD dwThreadId, DWORD uElapse, BOOL bSystem, 
                        int nSlot)
{
    int i;

    // Determines whether the timer is already setted, if already setted, just
    // return the existing timer
    TIMER_WAITOBJ(sem_timer);

    for (i = 0; i < MAX_TIMERS; i++)
    {
        if (!TimerList[i].bUsed)
            break;
    }

    if (i == MAX_TIMERS)
    {
        TIMER_RELEASEOBJ(sem_timer);
        return 0;
    }

    TimerList[i].uTimerId       = MAKETIMERID(i);
    TimerList[i].uElapse        = uElapse;
    TimerList[i].uMiliSeconds   = uElapse + PLXOS_GetElapsedTime(FALSE);
    TimerList[i].dwThreadId     = dwThreadId;
    TimerList[i].nSlot          = nSlot;
    TimerList[i].bUsed          = bSystem ? SYSTEM_TIMER : GENERAL_TIMER;

    PLXOS_SetTimerEvent(TimerList[i].uMiliSeconds);
    TIMER_RELEASEOBJ(sem_timer);

#ifdef WINDEBUG
    WINDEBUG_AddObj(OBJ_TIMER);
#endif

    return TimerList[i].uTimerId;
}

/*********************************************************************\
* Function	    UpdateTimer
* Purpose       Modify the properties of an existed timer.
* Params	   
* Return	 	   
* Remarks	    When a thread set a existed timer event id, the system 
                will replace the old timer with the new one and modifies
                the old properties.
**********************************************************************/
static UINT UpdateTimer(UINT uTimerId, DWORD uElapse, BOOL bSystem, int nSlot)
{
    int i;

    i = GETTIMERINDEX(uTimerId);
    if (i >= MAX_TIMERS || i < 0)
    {
        ASSERT(0);
        return 0;
    }

    TIMER_WAITOBJ(sem_timer);

    TimerList[i].uElapse = uElapse;
    TimerList[i].uMiliSeconds = uElapse + PLXOS_GetElapsedTime(FALSE);
    TimerList[i].nSlot = nSlot;
    TimerList[i].bUsed =bSystem ? SYSTEM_TIMER : GENERAL_TIMER;

    PLXOS_SetTimerEvent(TimerList[i].uMiliSeconds);
    TIMER_RELEASEOBJ(sem_timer);

    return TimerList[i].uTimerId;
}

/*********************************************************************\
* Function	    DestroyTimer
* Purpose       Unregister a existed timer.
* Params	   
* Return	 	TRUE when success and FALSE when failed.
* Remarks	   
**********************************************************************/
static BOOL DestroyTimer(UINT uTimerId)
{
    int i;

    i = GETTIMERINDEX(uTimerId);
    if (i < 0 || i >= MAX_TIMERS)
    {
//        ASSERT(0);
        return FALSE;
    }

    TIMER_WAITOBJ(sem_timer);

    TimerList[i].bUsed = 0;

    TIMER_RELEASEOBJ(sem_timer);

#ifdef WINDEBUG
    WINDEBUG_RemoveObj(OBJ_TIMER);
#endif

    return TRUE;
}
