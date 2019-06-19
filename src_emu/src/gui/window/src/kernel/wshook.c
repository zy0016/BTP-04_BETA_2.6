/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements hook support functions.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "wsownd.h"
#include "wsthread.h"

#ifndef NOWH
/*
typedef struct tagHOOKOBJ
{
    struct tagHOOKOBJ* pNext;   // Next hook node
    HOOKPROC    pfnHookProc;    // Hook proc of the hook
    HINSTANCE   hMod;           // handle of application instance 
    DWORD       dwThreadId;     // identity of thread to install hook
} HOOKOBJ, *PHOOKOBJ;

typedef struct tagHOOKCHAIN
{
    PHOOKOBJ pHead;
} HOOKCHAIN;
*/
// Default keyboard hook proc and default mouse hook proc
static PHOOKOBJ AllocHookObj(void);
static void FreeHookObj(PHOOKOBJ pHook);

static DWORD SystemHookThreadId = INVALID_THREADID;
static HOOKCHAIN SystemHookChains[WH_MAX + 1];
//static HOOKCHAIN HookChains[WH_MAX + 1];

/*
**  Function : SetWindowsHookEx
**  Purpose  :
**      Installs an application-defined hook procedure into a hook chain. 
**      You would install a hook procedure to monitor the system for 
**      certain types of events. These events are associated either with 
**      a specific thread or with all threads in the system. 
**  Params   :
**      idHook      : Specifies the type of hook procedure to be installed.
**                    This parameter can be one of the following values: 
**                        WH_KEYBOARD : Installs a hook procedure that 
**                                      monitors keystroke messages.
**                        WH_MOUSE    : Installs a hook procedure that 
**                                      monitors mouse messages. 
**      pfcHookProc : Points to the hook procedure. If the dwThreadId 
**                    parameter is zero or specifies the identifier of a 
**                    thread created by a different process, the pfnHookProc
**                    parameter must point to a hook procedure in a 
**                    dynamic-link library (DLL). Otherwise, lpfn can point
**                    to a hook procedure in the code associated with the 
**                    current process. 
**      hMod        : Identifies the DLL containing the hook procedure 
**                    pointed to by the pfnHookProc parameter. The hMod 
**                    parameter must be set to NULL if the dwThreadId 
**                    parameter specifies a thread created by the current 
**                    process and if the hook procedure is within the code 
**                    associated with the current process. 
**      dwThreadID  : Specifies the identifier of the thread with which the 
**                    hook procedure is to be associated. If this parameter
**                    is zero, the hook procedure is associated with all 
**                    existing threads. 
**  Return   :
**      If the function succeeds, return the handle of the hook procedure. 
**      If the function fails, the return value is NULL. 
**  Remarks  :
**      An error may occur if the hMod parameter is NULL and the dwThreadId
**      parameter is zero or specifies the identifier of a thread created 
**      by another process. 
**      Calling the CallNextHookEx function to chain to the next hook 
**      procedure is optional, but it is highly recommended; otherwise, 
**      other applications that have installed hooks will not receive hook 
**      notifications and may behave incorrectly as a result. You should 
**      call CallNextHookEx unless you absolutely need to prevent the 
**      notification from being seen by other applications. 
**      Before terminating, an application must call the UnhookWindowsHookEx
**      function to free system resources associated with the hook. 
**      For a specified hook type, thread hooks are called first, then 
**      system hooks. 
**      The system hooks are a shared resource, and installing one affects
**      all applications. All system hook functions must be in libraries. 
**      System hooks should be restricted to special-purpose applications 
**      or to use as a development aid during application debugging. 
**      Libraries that no longer need a hook should remove the hook 
**      procedure. 
*/
#define HOOK_SYSTEM 0x8000
HHOOK WINAPI SetWindowsHookEx(int idHook, HOOKPROC pfnHookProc, 
                              HINSTANCE hMod, DWORD dwThreadId)
{
    PHOOKOBJ pHook;
    PTHREADINFO pThreadInfo;
    BOOL bSystem;

    if ((idHook & HOOK_SYSTEM) != 0)
    {
        if (SystemHookThreadId == INVALID_THREADID)
        {
            SystemHookThreadId = WS_GetCurrentThreadId();
        }
        else if (SystemHookThreadId != WS_GetCurrentThreadId())
        {
            return NULL;
        }
        
        idHook &= ~HOOK_SYSTEM;
        bSystem = TRUE;
    }
    else
    {
        bSystem = FALSE;
    }

    // Invalid hook id, return NULL
    if (idHook < WH_MIN || idHook > WH_MAX || !pfnHookProc)
    {
        SetLastError(1);
        return NULL;
    }

    idHook -= WH_MIN;

    ENTERMONITOR;

    pThreadInfo = WS_GetThreadInfo(dwThreadId);
    if (pThreadInfo == NULL)
    {
        LEAVEMONITOR;
        SetLastError(1);
        return NULL;
    }
    
    pHook = AllocHookObj();
    if (!pHook)
    {
        LEAVEMONITOR;

        SetLastError(1);
        return NULL;
    }

    pHook->pfnHookProc = pfnHookProc;
    pHook->hMod = NULL;
    pHook->dwThreadId = dwThreadId;

    if (bSystem)
    {
        pHook->pNext = SystemHookChains[idHook].pHead;
        SystemHookChains[idHook].pHead = pHook;
    }
    else
    {
        pHook->pNext = pThreadInfo->HookChains[idHook].pHead;
        pThreadInfo->HookChains[idHook].pHead = pHook;
    }

//    pHook->pNext = HookChains[idHook].pHead;
//    HookChains[idHook].pHead = pHook;

    LEAVEMONITOR;

    return (HHOOK)pHook;
}

/*
**  Function : UnhookWindowsHookEx
**  Purpose  :
**      Removes a hook procedure installed in a hook chain by the 
**      SetWindowsHookEx function. 
**  Params   :
**      hHook : Identifies the hook to be removed. This parameter is a hook
**              handle obtained by a previous call to SetWindowsHookEx. 
**  Return   :
**      If the function succeeds, return TRUE.
**      If the function fails, return FALSE.
**  Remarks  :
**      The hook procedure can be in the state of being called by another 
**      thread even after UnhookWindowsHookEx returns. If the hook 
**      procedure is not being called concurrently, the hook procedure is 
**      removed immediately before UnhookWindowsHookEx returns. 
*/
BOOL WINAPI UnhookWindowsHookEx(HHOOK hHook)
{
    int i;
    PHOOKOBJ pHook, pPrev;
    PTHREADINFO pThreadInfo;

    if (!hHook)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    pThreadInfo = WS_GetThreadInfo(((PHOOKOBJ)hHook)->dwThreadId);
    if (pThreadInfo == NULL)
    {
        LEAVEMONITOR;
        SetLastError(1);
        return FALSE;
    }

    for (i = 0; i <= WH_MAX - WH_MIN; i++)
    {
        pHook = pThreadInfo->HookChains[i].pHead;
        pPrev = NULL;

        while (pHook)
        {
            if (pHook == (PHOOKOBJ)hHook)
            {
                if (pPrev)
                    pPrev->pNext = pHook->pNext;
                else
                    pThreadInfo->HookChains[i].pHead = pHook->pNext;

                pHook->pfnHookProc = NULL;
                pHook->pNext = NULL;

                FreeHookObj(pHook);

                LEAVEMONITOR;

                return TRUE;
            }

            pPrev = pHook;
            pHook = pHook->pNext;
        }

        if (pThreadInfo->dwThreadId == SystemHookThreadId)
        {
            pHook = SystemHookChains[i].pHead;
            pPrev = NULL;

            while (pHook)
            {
                if (pHook == (PHOOKOBJ)hHook)
                {
                    if (pPrev)
                        pPrev->pNext = pHook->pNext;
                    else
                        SystemHookChains[i].pHead = pHook->pNext;

                    pHook->pfnHookProc = NULL;
                    pHook->pNext = NULL;

                    FreeHookObj(pHook);

                    LEAVEMONITOR;

                    return TRUE;
                }

                pPrev = pHook;
                pHook = pHook->pNext;
            }
        }
    }

    LEAVEMONITOR;
    return FALSE;
}

/*
**  Function : CallNextHookEx
**  Purpose  :
**      Passes the hook information to the next hook procedure in the 
**      current hook chain. A hook procedure can call this function either
**      before or after processing the hook information. 
**  Params   :
**      hHook   : Identifies the current hook. An application receives this
**                handle as a result of a previous call to the 
**                SetWindowsHookEx function. 
**      nCode   : Specifies the hook code passed to the current hook 
**                procedure. The next hook procedure uses this code to 
**                determine how to process the hook information. 
**      wParam  : Specifies the wParam value passed to the current hook 
**                procedure. The meaning of this parameter depends on the 
**                type of hook associated with the current hook chain. 
**      lParam  : Specifies the lParam value passed to the current hook 
**                procedure. The meaning of this parameter depends on the 
**                type of hook associated with the current hook chain. 
**  Return   :
**      If the function succeeds, return the value returned by the next 
**      hook procedure in the chain. The current hook procedure must 
**      also return this value. The meaning of the return value depends 
**      on the hook type. For more information, see the descriptions of 
**      the individual hook procedures. 
**  Remarks  :
**      Hook procedures are installed in chains for particular hook types.
**      CallNextHookEx calls the next hook in the chain. 
**      Calling CallNextHookEx is optional, but it is highly recommended; 
**      otherwise, other applications that have installed hooks will not 
**      receive hook notifications and may behave incorrectly as a result.
**      You should call CallNextHookEx unless you absolutely need to 
**      prevent the notification from being seen by other applications. 
*/
LRESULT WINAPI CallNextHookEx(HHOOK hHook, int nCode, WPARAM wParam, 
                              LPARAM lParam)
{
    PHOOKOBJ pHook;

    if (!hHook)
    {
        SetLastError(1);
        return 0;
    }

    pHook = (PHOOKOBJ)hHook;

    // If the the hook is the last hook of the hook chain, return 0 to
    // continue process the event or the message
    if (!pHook->pNext)
        return 0;

    return pHook->pNext->pfnHookProc(nCode, wParam, lParam);
}

/*
**  Function : WH_CallHookProc
**  Purpose  :
**      Calls the specified hook proc, this function will be called
**      in DevInput.c.
*/
LRESULT WH_CallHookProc(int nHookId, int nCode, WPARAM wParam, 
                        LPARAM lParam)
{
    PTHREADINFO pThreadInfo;
    LRESULT lRet = 0;
    BOOL bSystem, bThread;

    ASSERT(nHookId >= WH_MIN && nHookId <= WH_MAX);

    pThreadInfo = WS_GetCurrentThreadInfo();

    nHookId -= WH_MIN;

    if (SystemHookChains[nHookId].pHead == NULL)
    {
        bSystem = FALSE;
    }
    else
    {
        bSystem = TRUE;
    }

    if (pThreadInfo->HookChains[nHookId].pHead == NULL)
    {
        if (!bSystem)
            return lRet;

        bThread = FALSE;
    }
    else
    {
        bThread = TRUE;
    }

//    RELEASEMSGQUEOBJ;

    if (bSystem)
    {
        lRet = SystemHookChains[nHookId].pHead->pfnHookProc(nCode, wParam, lParam);
    }

    if ((lRet == 0) && bThread)
    {
        lRet = pThreadInfo->HookChains[nHookId].pHead->pfnHookProc(nCode, wParam, lParam);
    }

//    WAITMSGQUEOBJ;

    return lRet;
}

/*************************************************************************/
/*          Internal functions                                           */
/*************************************************************************/

#define HOOK_CACHE_SIZE 10

static HOOKOBJ HookCache[HOOK_CACHE_SIZE];

/*
**  Function : AllocHookObj
**  Purpose  :
**      The last hook proc in keyboard hook chain.
*/
static PHOOKOBJ AllocHookObj(void)
{
    int i;

    for (i = 0; i < HOOK_CACHE_SIZE; i++)
    {
        if (!HookCache[i].pfnHookProc)
            return &HookCache[i];
    }

    return MemAlloc(sizeof(HOOKOBJ));
}

/*
**  Function : FreeHookObj
**  Purpose  :
*/
static void FreeHookObj(PHOOKOBJ pHook)
{
    ASSERT(!pHook->pfnHookProc);

    if ((pHook >= HookCache) && (pHook < HookCache + HOOK_CACHE_SIZE))
        return;

    MemFree(pHook);
}

#endif  // NOWH
