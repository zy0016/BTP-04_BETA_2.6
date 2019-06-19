/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements threadinfo support functions.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "string.h"
#include "winmem.h"
#include "wsownd.h"
#include "wsobj.h"
#include "wsthread.h"

#if (__MP_PLX_GUI || __MT_PLX_GUI)
MONITOROBJ sem_monitor;
#endif

static PTHREADINFO ThreadInfoHead = NULL;
static BOOL     InsertThreadInfo(PTHREADINFO pThreadInfo);
static BOOL     DeleteThreadInfo(PTHREADINFO pThreadInfo);
static void     InitThreadInfo(PTHREADINFO pThreadInfo);

DWORD WINAPI GetCurrentThreadId(void)
{
    return WS_GetCurrentThreadId();
}

DWORD WINAPI GetCurrentProcessId(void)
{
    return WS_GetCurrentProcessId();
}

DWORD WS_GetProcessIdFromThreadId(DWORD dwThreadId)
{
    return 0;
}

DWORD WS_GetCurrentProcessId(void)
{
#if(__MP_PLX_GUI)
#ifdef _EMULATE_
    return EMU_GetCurrentProcessId();
#endif
#endif

#if(_HOPEN_VER <= 200)
    return 0;

#elif(_HOPEN_VER >= 300)
    return (DWORD)getpid();
#endif //_HOPEN_VER
}

DWORD WS_GetCurrentThreadId(void)
{
#if(__MP_PLX_GUI)

#ifdef _EMULATE_
    return EMU_GetCurrentThreadId();
#endif

#elif(__MT_PLX_GUI || __XMT_PLX_GUI)

#if(_HOPEN_VER <= 200)
    return OS_GetTaskID();
#elif(_HOPEN_VER >= 300)
    return pthread_self();
#endif //_HOPEN_VER

#else //__ST_PLX_GUI
    return 0;

#endif
}

PTHREADINFO WS_GetThreadInfo(DWORD dwThreadId)
{
    PTHREADINFO pCur;
    
    pCur = ThreadInfoHead;
    while (pCur != NULL && pCur->dwThreadId != dwThreadId)
        pCur = pCur->pNext;

    return pCur;
}

PTHREADINFO WS_GetCurrentThreadInfo(void)
{
    return WS_GetThreadInfo(WS_GetCurrentThreadId());
}

BOOL WS_ThreadInit(void)
{
    return TRUE;
}

BOOL WS_ThreadExit(DWORD dwThreadId)
{
    PTHREADINFO pThreadInfo;

    ASSERT(!THREAD_ISDESTROYED(dwThreadId));

    pThreadInfo = WS_GetThreadInfo(dwThreadId);
    ASSERT(pThreadInfo != NULL);

    /* 删除该线程创建的所有窗口,消息队列将在窗口销毁过程中被释放 */
    if (!WND_DestroyWindowsByThreadId(dwThreadId))
    {
        ASSERT(0);
        SetLastError(1);
        return FALSE;
    }

    /* 删除线程信息结构 */
    if (!THREADINFO_Destroy(pThreadInfo))
    {
        ASSERT(0);
        SetLastError(1);
        return FALSE;
    }

    return TRUE;
    
    
}


BOOL THREADINFO_Init(void)
{
    return TRUE;
}

void THREADINFO_Exit(void)
{
    return;
}

PTHREADINFO THREADINFO_Create(void)
{
    PTHREADINFO pThreadInfo;

    pThreadInfo = MemAlloc(sizeof(THREADINFO));
    if (pThreadInfo == NULL)
    {
        SetLastError(1);
        return NULL;
    }

    InitThreadInfo(pThreadInfo);

    if (!InsertThreadInfo(pThreadInfo))
    {
        MemFree(pThreadInfo);
        return NULL;
    }

    return pThreadInfo;
}

BOOL THREADINFO_Destroy(PTHREADINFO pThreadInfo)
{
    if (!DeleteThreadInfo(pThreadInfo))
    {
        return FALSE;
    }

    DESTROYBLOCKOBJ(pThreadInfo->sem_message);
    MemFree(pThreadInfo);
    return TRUE;
}

/* 内部函数 */
static BOOL InsertThreadInfo(PTHREADINFO pThreadInfo)
{
    PTHREADINFO pCur;

    pThreadInfo->pNext = NULL;
    if (ThreadInfoHead == NULL)
    {
        ThreadInfoHead = pThreadInfo;
        return TRUE;
    }

    pCur = ThreadInfoHead;
    while (pCur->pNext != NULL)
    {
        pCur = pCur->pNext;
    }

    pCur->pNext = pThreadInfo;
    return TRUE;
}

static BOOL DeleteThreadInfo(PTHREADINFO pThreadInfo)
{
    PTHREADINFO pCur, pPrev;

    if (ThreadInfoHead == pThreadInfo)
    {
        ThreadInfoHead = ThreadInfoHead->pNext;
        return TRUE;
    }

    pCur = ThreadInfoHead;
    pPrev = pCur;
    while (pCur != NULL && pCur != pThreadInfo)
    {
        pPrev = pCur;
        pCur = pCur->pNext;
    }

    if (pCur == NULL)
        return FALSE;

    pPrev->pNext = pCur->pNext;

    return TRUE;
}

static void InitThreadInfo(PTHREADINFO pThreadInfo)
{
#if (__XMT_PLX_GUI)
    static BLOCKOBJ semblock;

    MemSet(pThreadInfo, 0, sizeof(THREADINFO));
    INITBLOCKOBJ(semblock);
    pThreadInfo->sem_message = semblock;

#else
    MemSet(pThreadInfo, 0, sizeof(THREADINFO));
    INITBLOCKOBJ(pThreadInfo->sem_message);
    /* The first block is just to get the object. And the block later is to
    ** block the thread
    */
#endif
    pThreadInfo->dwThreadId = WS_GetCurrentThreadId();
    BLOCK(pThreadInfo);
    return;
}
