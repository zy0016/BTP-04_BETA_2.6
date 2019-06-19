/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements message management functions.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "wssysmsg.h"
#include "devdisp.h"

#include "wsobj.h"
#include "wsownd.h"
#include "wsthread.h"
#include "msgqueue.h"

/**************************************************************************/
/*          Prototype for some external function                          */
/**************************************************************************/
BOOL INPUTDEV_GetInputMessage(PMSG pMsg);
BOOL INPUTDEV_TranslateMessage(const MSG* pMsg);
BOOL WND_GetPaintMessage(PMSG pMsg);
BOOL WS_GetTimerMessage(PMSG pMsg);

extern struct pollfd fds[2];
extern int  UpdateKeyboard(void);
extern int  UpdateMouse(void);
extern void KickDog(void);

/**************************************************************************/
/*          Prototype for internal functions                              */
/**************************************************************************/
static void WS_IdleTask(void);
static BOOL CALLBACK DispatchMessageToWindow(HWND hWnd, LPARAM lParam);
static BOOL CALLBACK DispatchSendMessageToWindow(HWND hWnd, LPARAM lParam);
static BOOL CALLBACK DispatchSendNotifyMessageToWindow(HWND hWnd, LPARAM lParam);
static BOOL CALLBACK DispatchPostMessageToWindow(HWND hWnd, LPARAM lParam);

#if (__ST_PLX_GUI || __XMT_PLX_GUI)
MSGQUEOBJ sem_msgque;
#endif
/*
**  Function : WS_DestroyMsgQueue
**  Purpose  :
**      Destroys message queue, free relative resources.
**  Params   :
**      None.
**  Return   :
**      None.
*/
void WS_DestroyMsgQueue(void)
{
}

static BOOL WS_GetMessage(PMSG pMsg, HWND hWnd, UINT wMsgFilterMin, 
                       UINT wMsgFilterMax, UINT wRemoveMsg, BOOL bMsgBlock)
{
    PTHREADINFO pThreadInfo;
#if (!__ST_PLX_GUI)
    PSENDMSG pSendMsg;
#endif
    PPOSTMSG pPostMsg;

    if (!pMsg || (hWnd != NULL && !IsWindow(hWnd)))
        return -1;

    pThreadInfo = WS_GetCurrentThreadInfo();

    ENTERMONITOR;

    KickDog();

    while (1)
    {
#if (!__ST_PLX_GUI)
//        ASSERT(pThreadInfo->bBlock == BLOCK_NO);
        if (pThreadInfo->dwState & QS_SENDMESSAGE)
        {
            /* QS_SENDMESSAGE标志由MSQ_GetSendMsg在内部判断是否重置 */
            pSendMsg = MSQ_GetSendMsg(pThreadInfo, TRUE);
            ASSERT(pSendMsg != NULL && 
                (pSendMsg->type == SENDMSG_SEND || pSendMsg->type == SENDMSG_NOTIFY));
            if (pSendMsg != NULL)
            {
                /* Send消息由发送方从消息池得到，还应该由发送方释放 */
                MSQ_ReplyMessage(pSendMsg);
                
                continue;
            }
            else
                continue;
        }
#endif
        if (pThreadInfo->dwState & QS_POSTMESSAGE)
        {
#if (__ST_PLX_GUI)
            WAITMSGQUEOBJ;
#endif 
            /* QS_POSTMESSAGE标志由MSQ_GetPostMsg在内部判断是否重置 */
            pPostMsg = MSQ_GetPostMsg(pThreadInfo, TRUE);

            ASSERT(pPostMsg != NULL);
            if (pPostMsg != NULL)
            {
                *pMsg = pPostMsg->msg;

                /* Post消息结构有发送线程从消息池中得到，应该由处理方线程释放 */
                ReleaseMsgObj(pPostMsg, MSGOBJ_POST);

#if (__ST_PLX_GUI)
                RELEASEMSGQUEOBJ;
#endif 
                LEAVEMONITOR;

                return TRUE;
            }
        }
        if (pThreadInfo->dwState & QS_QUIT)
        {
            pMsg->message = WM_QUIT;
            pMsg->hwnd = NULL;
            pMsg->wParam = pThreadInfo->nExitCode;
            pMsg->lParam = 0;

            pThreadInfo->dwState &= ~QS_QUIT;
            if (!WS_ThreadExit(pThreadInfo->dwThreadId))
                continue;

            LEAVEMONITOR;

            return FALSE;
        }
        // Gets message from the system input event queue
        if (pThreadInfo->dwState & QS_INPUT)
        {
            if (INPUTDEV_GetInputMessage(pMsg))
            {
                LEAVEMONITOR;

                return TRUE;
            }
            else
                continue;
        }
        if (pThreadInfo->dwState & QS_PAINT)
        {
            if (WND_GetPaintMessage(pMsg))
            {
                LEAVEMONITOR;

                return TRUE;
            }
        }
        if (pThreadInfo->dwState & QS_TIMER)
        {
            if (WS_GetTimerMessage(pMsg))
            {
                LEAVEMONITOR;

                return TRUE;
            }
        }

        //ASSERT(pThreadInfo->dwState == 0);

        // Now the message queue has no message, executes the idle task
        WS_IdleTask();

        if (!bMsgBlock)
        {
            LEAVEMONITOR;
            return FALSE;
        }

//        if (pThreadInfo->dwState != 0)
//            printf("ThreadId = %d, *************pThreadInfo->dwState = %x\n", pThreadInfo->dwThreadId, pThreadInfo->dwState);
//        ASSERT(pThreadInfo->dwState == 0);
//        printf("Thread %d:Want WAIT%s,%d\n", GetCurrentThreadId(),__FILE__, __LINE__);
        pThreadInfo->bBlock = BLOCK_INGETMESSAGE;
        LEAVEMONITOR;

        BLOCK(pThreadInfo);
//        printf("Thread %d:Out of WAIT%s,%d\n", GetCurrentThreadId(),__FILE__, __LINE__);

        ENTERMONITOR;
//        printf("Thread %d:Enter monitor %s,%d\n", GetCurrentThreadId(),__FILE__, __LINE__);

//        if (pThreadInfo->dwState == 0)
//            printf("ThreadId = %d, *************pThreadInfo->dwState = %x\n", pThreadInfo->dwThreadId, pThreadInfo->dwState);
//        ASSERT(pThreadInfo->dwState != 0);
    }

    return TRUE;
}


/*                
**  Function : GetMessage
**  Purpose  :
**      Retrieves a message from the calling thread's message queue. The 
**      function retrieves messages that lie within a specified range of 
**      message values. 
**  Params   :
**      pMsg          : Points to an MSG structure that receives message 
**                      information.
**      hWnd          : Identifies the window whose messages are to be 
**                      retrieved. If the window is NULL, this function 
**                      retrieves messages for any window.
**      wMsgFilterMin : Specifies the integer value of the lowest message
**                      value to be retrieved. 
**      wMsgFilterMax : Specifies the integer value of the highest message
**                      value to be retrieved. 
**  Return   :
**      If the function retrieves a message other than WM_QUIT,return 
**      nonzero. If the function retrieves the WM_QUIT message, return zero.
**      If there is an error, return -1. For example, the function fails if 
**      hWnd is an invalid window handle. 
*/
BOOL WINAPI GetMessage(PMSG pMsg, HWND hWnd, UINT wMsgFilterMin, 
                       UINT wMsgFilterMax)
{
    return WS_GetMessage(pMsg, hWnd, wMsgFilterMin, wMsgFilterMax, 
        PM_REMOVE, TRUE);
}

/*                
**  Function : PeekMessage
**  Purpose  :
**      The PeekMessage function dispatches incoming sent messages, 
**      checks the thread message queue for a posted message, and 
**      retrieves the message (if any exist). 
**  Params   :
**      pMsg          : Points to an MSG structure that receives message 
**                      information.
**      hWnd          : Identifies the window whose messages are to be 
**                      retrieved. If the window is NULL, this function 
**                      retrieves messages for any window.
**      wMsgFilterMin : Specifies the integer value of the lowest message
**                      value to be retrieved. 
**      wMsgFilterMax : Specifies the integer value of the highest message
**                      value to be retrieved. 
**      wRemoveMsg    : [in] Specifies how messages are handled. This 
**                      parametercan be one of the following values. 
**                Value                     Meaning 
**                PM_NOREMOVE     Messages are not removed from the 
**                                queue after processing by PeekMessage. 
**                PM_REMOVE       Messages are removed from the queue 
**                                after processing by PeekMessage. 
**  Return   :
**      If a message is available, the return value is nonzero.
**      If no messages are available, the return value is zero. 
*/
BOOL WINAPI PeekMessage(PMSG pMsg, HWND hWnd, UINT wMsgFilterMin, 
                       UINT wMsgFilterMax, UINT wRemoveMsg)

{
    wRemoveMsg = PM_REMOVE;
    memset(pMsg, 0, sizeof(MSG));

    return WS_GetMessage(pMsg, hWnd, wMsgFilterMin, wMsgFilterMax, 
        wRemoveMsg, FALSE);
}

/*
**  Function : PostMessage
**  Purpose  :
**      Places (posts) a message in the message queue associated with the 
**      thread that created the specified window and then returns without 
**      waiting for the thread to process the message. Messages in a 
**      message queue are retrieved by calls to the GetMessage function. 
**  Params   :
**      hWnd    : Identifies the window whose window procedure is to 
**                receive the message.
**      wMsgCmd : Specifies the message to be posted.
**      wParam  : Specifies additional message-specific information. 
**      lParam  : Specifies additional message-specific information. 
**  Returns  :
**      If the function succeeds, the return value is TRUE.
**      If the function fails, the return value is FALSE. To get extended
**      error information, call GetLastError. 
*/
BOOL WINAPI PostMessage(HWND hWnd, UINT uMsgCmd, WPARAM wParam, 
                        LPARAM lParam)
{
    MSG msg;
    PWINOBJ pWin;
    BOOL bRet;

    if (hWnd == HWND_BROADCAST)
    {
        msg.hwnd = hWnd;
        msg.message = uMsgCmd;
        msg.wParam = wParam;
        msg.lParam = lParam;
        
        EnumWindows(DispatchPostMessageToWindow, (LPARAM)&msg);
        
        return TRUE;
    }
    
    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (pWin == NULL)
    {
        return PostThreadMessage(WS_GetCurrentThreadId(), uMsgCmd, wParam, lParam);
    }

    ENTERMONITOR;

#if (__ST_PLX_GUI)
    WAITMSGQUEOBJ;
#endif 

    bRet = MSQ_PostMessage(pWin, uMsgCmd, wParam, lParam);

#if (__ST_PLX_GUI)
    RELEASEMSGQUEOBJ;
#endif

    LEAVEMONITOR;
    
    return bRet;
}

/*
**  Function : PostThreadMessage
*/

BOOL WINAPI PostThreadMessage(DWORD dwThreadId, UINT uMsgCmd, WPARAM wParam, 
                              LPARAM lParam)
{
    BOOL bRet;
    
    ENTERMONITOR;
#if (__ST_PLX_GUI)
    WAITMSGQUEOBJ;
#endif 

    bRet = MSQ_PostThreadMessage(dwThreadId, uMsgCmd, wParam, lParam);

    LEAVEMONITOR;
#if (__ST_PLX_GUI)
    RELEASEMSGQUEOBJ;
#endif

    return bRet;
    
}

void WINAPI PostQuitMessage(int nExitCode)
{
    PTHREADINFO pThreadInfo;

    ENTERMONITOR;

    pThreadInfo = WS_GetCurrentThreadInfo();
    ASSERT(pThreadInfo != NULL);

    pThreadInfo->nExitCode = nExitCode;
    pThreadInfo->dwState |= QS_QUIT;

    LEAVEMONITOR;
}

/*
**  Function : SendMessage
**  Purpose  :
**      Sends the specified message to a window or windows. The function
**      calls the window procedure for the specified window and does not 
**      return until the window procedure has processed the message. The 
**      PostMessage function, in contrast, posts a message to a thread's 
**      message queue and returns immediately. 
**  Params   :
**      hWnd    : Identifies the window whose window procedure is to 
**                receive the message. If this parameter is HWND_BROADCAST,
**                the message is sent to all top-level windows in the 
**                system, including disabled or invisible unowned windows,
**                overlapped windows, and pop-up windows; but the message 
**                is not sent to child windows. 
**      wMsgCmd : Specifies the message to be posted.
**      wParam  : Specifies additional message-specific information. 
**      lParam  : Specifies additional message-specific information. 
**  Returns  :
**      The return value specifies the result of the message processing 
**      and depends on the message sent. 
*/
LRESULT WINAPI SendMessage(HWND hWnd, UINT uMsgCmd, WPARAM wParam, 
                           LPARAM lParam)
{
    MSG msg;
    PWINOBJ pWin;
#if (!__ST_PLX_GUI)
    LRESULT nRet;
#endif
    if (hWnd == HWND_BROADCAST)
    {
        msg.hwnd = hWnd;
        msg.message = uMsgCmd;
        msg.wParam = wParam;
        msg.lParam = lParam;
        
        EnumWindows(DispatchSendMessageToWindow, (LPARAM)&msg);

        return 1;
    }

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (pWin == NULL)
    {
        return 0;
    }

#if (!__ST_PLX_GUI)
    if (pWin->dwThreadId == WS_GetCurrentThreadId())
    {
#endif
        msg.hwnd = hWnd;
        msg.message = uMsgCmd;
        msg.wParam = wParam;
        msg.lParam = lParam;

        return DispatchMessage(&msg);
#if (!__ST_PLX_GUI)
    }

    ENTERMONITOR;

    nRet = MSQ_SendMessage(pWin, uMsgCmd, wParam, lParam);

    LEAVEMONITOR;

    return nRet;
#endif
}

BOOL WINAPI SendNotifyMessage(HWND hWnd, UINT uMsgCmd, WPARAM wParam, 
                           LPARAM lParam)
{
    MSG msg;
    PWINOBJ pWin;

    if (hWnd == HWND_BROADCAST)
    {
        msg.hwnd = hWnd;
        msg.message = uMsgCmd;
        msg.wParam = wParam;
        msg.lParam = lParam;
        
        EnumWindows(DispatchSendNotifyMessageToWindow, (LPARAM)&msg);
        
        return 1;
    }

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (pWin == NULL)
    {
        return FALSE;
    }

#if (!__ST_PLX_GUI)
    if (pWin->dwThreadId == WS_GetCurrentThreadId())
    {
#endif
        msg.hwnd = hWnd;
        msg.message = uMsgCmd;
        msg.wParam = wParam;
        msg.lParam = lParam;

        DispatchMessage(&msg);

        return TRUE;
#if (!__ST_PLX_GUI)
    }

    ENTERMONITOR;

    MSQ_SendNotifyMessage(pWin, uMsgCmd, wParam, lParam);

    LEAVEMONITOR;

    return TRUE;
#endif
}
/*
**  Function : TranslateMessage
**  Purpose  :
**      Translates virtual-key messages into character messages. The 
**      character messages are posted to the message queue, to be read the
**      next time the thread calls the GetMessage function. 
**  Params   :
**      pMsg : Points to an MSG structure that contains message information
**             retrieved from the message queue by using the GetMessage 
**             function. 
**  Return   :
**      If the message is translated (that is, a character message is 
**      posted to the message queue), return nonzero. 
**      If the message is not translated (that is, a character message is 
**      not posted to the message queue), return zero. 
**  Remarks  :
**      The TranslateMessage function does not modify the message pointed 
**      to by the pMsg parameter. WM_KEYDOWN and WM_KEYUP combinations 
**      produce a WM_CHAR or WM_DEADCHAR message. WM_SYSKEYDOWN and 
**      WM_SYSKEYUP combinations produce a WM_SYSCHAR or WM_SYSDEADCHAR 
**      message. 
*/
BOOL WINAPI TranslateMessage(const MSG* pMsg)
{
    if (!pMsg)
    {
        SetLastError(1);
        return FALSE;
    }

    if (pMsg->message != WM_KEYDOWN && pMsg->message != WM_SYSKEYDOWN)
        return FALSE;

    return INPUTDEV_TranslateMessage(pMsg);
}

/*
**  Function : DispatchMessage
**  Purpose  :
**      
**  Params   :
**      pMsg : Points to an MSG structure that contains the message.
*/
LONG WINAPI DispatchMessage(const MSG* pMsg)
{
    WNDPROC pfnWndProc;
    LONG lResult;
    TIMERPROC pTimerProc;
    DWORD dwTime;

    if (!pMsg)
        return 0L;

    if ((pMsg->message == WM_TIMER || pMsg->message == WM_SYSTIMER) && 
        pMsg->lParam != 0)
    {
        // Call GetTickCount to get the number of milliseconds that 
        // have elapsed since Windows was started. 
        dwTime = 0;     
        
        // Calls application callback timer function.
        pTimerProc = (TIMERPROC)pMsg->lParam;
        pTimerProc(pMsg->hwnd, pMsg->message, pMsg->wParam, dwTime);

        return 0;
    }

    if (!pMsg->hwnd)
        return 0;

    lResult = 0;

    if (pMsg->hwnd != HWND_BROADCAST)
    {
        pfnWndProc = (WNDPROC)GetWindowLong(pMsg->hwnd, GWL_WNDPROC);
        if (pfnWndProc)
        {
            lResult = pfnWndProc(pMsg->hwnd, pMsg->message, pMsg->wParam, 
                pMsg->lParam);
        }
    }
    else
        EnumWindows(DispatchMessageToWindow, (LPARAM)pMsg);

    return lResult;
}

/*
**  Function : AppRun
**  Purpose  :
**      Message loop function.
*/
void WINAPI AppRun(void)
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

/*
**  Function : WS_IsBlocked
**  Purpose  :
**      Retrieves whether the window task is blocked.
*/
BOOL WS_IsBlocked(void)
{
    PTHREADINFO pThreadInfo;

    pThreadInfo = WS_GetCurrentThreadInfo();
    return ISBLOCK(pThreadInfo);
}
/**************************************************************************/
/*              Internal functions                                        */
/**************************************************************************/

/*
**  Function : WS_IdleTask
**  Purpose  :
**     窗口系统空闲时需要调用的函数，在窗口系统消息队列空时调用。
*/
static void WS_IdleTask(void)
{
    /* 降低优先级，执行应用的Idle任务 */
    if (g_DisplayDrv.func_mask1 & FM_CHECKCURSOR)
        g_DisplayDrv.CheckCursor(g_pDisplayDev);

    if (g_DisplayDrv.func_mask1 & FM_UPDATESCREEN)
        g_DisplayDrv.UpdateScreen(g_pDisplayDev);
}

/*
**  Function : DispatchMessageToWindow
**  Purpose  :
**      Dispatchs a specifie message to the specified window. This 
**      function is a top-level window enumerate function.
*/
static BOOL CALLBACK DispatchMessageToWindow(HWND hWnd, LPARAM lParam)
{
    PMSG pMsg;
    WNDPROC pfnWndProc;

    pMsg = (PMSG)lParam;

    pfnWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
    if (pfnWndProc)
        pfnWndProc(hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
    
    return TRUE;
}

/*
**  Function : DispatchSendMessageToWindow
**  Purpose  :
**      Dispatchs a specifie message to the specified window. This 
**      function is a top-level window enumerate function.
*/
static BOOL CALLBACK DispatchSendMessageToWindow(HWND hWnd, LPARAM lParam)
{
    PMSG pMsg;

    pMsg = (PMSG)lParam;

    SendMessage(hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
   
    return TRUE;
}

/*
**  Function : DispatchSendNotifyMessageToWindow
**  Purpose  :
**      Dispatchs a specifie message to the specified window. This 
**      function is a top-level window enumerate function.
*/
static BOOL CALLBACK DispatchSendNotifyMessageToWindow(HWND hWnd, LPARAM lParam)
{
    PMSG pMsg;

    pMsg = (PMSG)lParam;

    SendNotifyMessage(hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
   
    return TRUE;
}

/*
**  Function : DispatchPostMessageToWindow
**  Purpose  :
**      Dispatchs a specifie message to the specified window. This 
**      function is a top-level window enumerate function.
*/
static BOOL CALLBACK DispatchPostMessageToWindow(HWND hWnd, LPARAM lParam)
{
    PMSG pMsg;

    pMsg = (PMSG)lParam;

    PostMessage(hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
   
    return TRUE;
}
