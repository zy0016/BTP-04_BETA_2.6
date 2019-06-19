/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Device event queue management.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "wsthread.h"
#include "devqueue.h"
#include "devdisp.h"
#include "wscursor.h"

//static DEVEVENTQUEUE DevEventQueue;     // 设备事件队列
//static USEREVENTQUEUE UserEventQueue;   // 用户事件队列

// 设备事件队列中键盘事件的个数
static int volatile nKbdEventCount;

// 队列中鼠标一个按键事件的存在可能
#define ME_NODOWN       0   // 表示队列中没有鼠标按下事件
#define ME_DOWN         1   // 表示队列中有鼠标按下事件
#define ME_DOWNUP       2   // 表示队列中有鼠标按下事件和抬起事件

// 用来保存鼠标左键、右键和中键事件的状态
//static int volatile nLeftDown;
//static int volatile nRightDown;
//static int volatile nMiddleDown;

static BOOL bMoveMerge = TRUE;

/*
**  Function : DEVQ_GetInputEvent
**  Purpose  :
**      Retrieves a event from the device input event queue tail.
*/
BOOL DEVQ_GetInputEvent(PTHREADINFO pThreadInfo, PDEVEVENT pDevEvent, BOOL bRemove)
{
    PUSEREVENTQUEUE pUser;
    PDEVEVENTQUEUE pDev;

    ASSERT(pDevEvent != NULL);

    pUser = &(pThreadInfo->InputEventQue.UserEventQueue);
    pDev = &(pThreadInfo->InputEventQue.DevEventQueue);

    // 如果用户事件队列中有事件, 从用户事件队列中取一个事件
    if (pUser->head != pUser->tail)
    {
        *pDevEvent = pUser->events[pUser->head];
        
        if (bRemove)
        {
            pUser->head = (pUser->head + 1) % USER_QUEUE_SIZE;
        }

        if (pUser->head == pUser->tail && pDev->head == pDev->tail)
        {
            pThreadInfo->dwState &= ~QS_INPUT;
        }

        return TRUE;
    }

    // 用户事件队列中没有事件, 从设备事件队列中取一个事件, 设备事件队列空, 
    // 直接返回FALSE
    if (pDev->head == pDev->tail)
    {
        // 键盘事件计数清0, 以避免计数由于中断产生的误差积累
        nKbdEventCount = 0;

        pThreadInfo->dwState &= ~QS_INPUT;
        return FALSE;
    }

    // 设备事件队列中有事件, 从设备事件队列中取一个事件

    *pDevEvent = pDev->events[pDev->head];
    
    if (bRemove)
    {
        pDev->head = (pDev->head + 1) % DEV_QUEUE_SIZE;

        // 取走的是键盘事件, 减少键盘事件计数
        if (pDevEvent->type == DE_KEYBOARD && nKbdEventCount > 0)
            nKbdEventCount--;
    }
    
    if (pDev->head == pDev->tail)
    {
        pThreadInfo->dwState &= ~QS_INPUT;
    }
    
    return TRUE;
}

/*
**  Function : DEVQ_PutKeyboardEvent
**  Purpose  :
**      Puts a specified keyboard event into the device input event queue.
*/
BOOL DEVQ_PutKeyboardEvent(PTHREADINFO pThreadInfo, BYTE byVirtKey, 
                           BYTE byScan, DWORD dwFlags, DWORD dwExtraInfo)
{
    PDEVEVENT pEvent;
    PUSEREVENTQUEUE pUser;
    PDEVEVENTQUEUE pDev;


    if (!(dwFlags & EVENTF_FROMDEVICE))
    {
        pUser = &(pThreadInfo->InputEventQue.UserEventQueue);
        // 用户键盘事件, 加到设备事件队列中

        // 用户事件队列满, 返回FALSE
        if ((pUser->tail + 1) % USER_QUEUE_SIZE == pUser->head)
        {
            ASSERT(0);
            return FALSE;
        }

        // 将新的事件插入到队列尾

        pEvent = &pUser->events[pUser->tail];

        pEvent->type                  = DE_KEYBOARD;
        pEvent->event.kbd.vkCode      = byVirtKey;
        pEvent->event.kbd.scanCode    = byScan;
        pEvent->event.kbd.flags       = dwFlags;
        pEvent->event.kbd.time        = GetTickCount();
        pEvent->event.kbd.dwExtraInfo = dwExtraInfo;

        pUser->tail = (pUser->tail + 1) % USER_QUEUE_SIZE;
    }
    else
    {
        pDev = &(pThreadInfo->InputEventQue.DevEventQueue);
        // 设备键盘事件, 加到设备事件队列中

        // 设备事件队列满, 返回FALSE
        // if (nKbdEventCount >= MAX_KBD_EVENTS)
        if ((pDev->tail + 1) % DEV_QUEUE_SIZE == pDev->head)
            return FALSE;

        // 是键盘按下事件并且设备事件队列不空, 判断是否前一个键盘事件是同一
        // 个键盘按下事件, 若是, 合并
        if ((pDev->head != pDev->tail) && !(dwFlags & KEYEVENTF_KEYUP))
        {
            int i;
            
            // 寻找最后一个键盘事件
            i = (pDev->tail + DEV_QUEUE_SIZE - 1) % DEV_QUEUE_SIZE;
            while (i != pDev->head)
            {
                // 找到鼠标事件, 退出循环
                if (pDev->events[i].type == DE_KEYBOARD)
                    break;
                
                i = (i + DEV_QUEUE_SIZE - 1) % DEV_QUEUE_SIZE;
            }
            
            // 找到键盘事件
            pEvent = &pDev->events[i];
            if (pEvent->type == DE_KEYBOARD)
            {
                // 当前键盘按下事件与找到的上一个键盘事件相同, 忽略当前
                // 键盘按下事件直接返回
                if (pEvent->event.kbd.vkCode == byVirtKey &&
                    pEvent->event.kbd.flags == dwFlags)
                {
                    ASSERT(pThreadInfo->dwState & QS_INPUT);
                    return TRUE;
                }
            }
        }

        // 将新的事件插入到队列尾

        pEvent = &pDev->events[pDev->tail];

        pEvent->type                  = DE_KEYBOARD;
        pEvent->event.kbd.vkCode      = byVirtKey;
        pEvent->event.kbd.scanCode    = byScan;
        pEvent->event.kbd.flags       = dwFlags;
        pEvent->event.kbd.time        = GetTickCount();
        pEvent->event.kbd.dwExtraInfo = dwExtraInfo;

        pDev->tail = (pDev->tail + 1) % DEV_QUEUE_SIZE;

        // 增加键盘事件计数
        nKbdEventCount++;
    }

    // 如果窗口任务有处于阻塞状态, 发送消息唤醒窗口任务
    pThreadInfo->dwState |= QS_INPUT;
    if (ISBLOCKINGET(pThreadInfo))
    {
        pThreadInfo->bBlock = BLOCK_NO;
        WAKEUP(pThreadInfo);
    }

    return TRUE;
}

/*
**  Function : DEVQ_PutMouseEvent
**  Purpose  :
**      Puts a specified mouse event input the device input event queue.
*/
BOOL DEVQ_PutMouseEvent(PTHREADINFO pThreadInfo,  DWORD dwFlags, LONG dx, 
                        LONG dy, DWORD dwData, DWORD dwExtraInfo, HWND hWnd)
{
#if (!NOCURSOR)
    static BOOL bCursorMove;    // 是否用户鼠标事件导致移动光标
    PGRAPHDEV   pGraphDev;
#endif

    static POINT ptCursor;      // 用来保存光标的位置

    PDEVEVENT pEvent;
    POINT pt;

    PUSEREVENTQUEUE pUser;
    PDEVEVENTQUEUE pDev;


    if (!(dwFlags & EVENTF_FROMDEVICE))
    {
        // 用户鼠标事件
        pUser = &(pThreadInfo->InputEventQue.UserEventQueue);

        // 如果是相对坐标, 转换为绝对坐标
        if (!(dwFlags & MOUSEEVENTF_ABSOLUTE))
        {
            // 不能使用本模块保存的光标表位置, 应使用光标管理模块保存的
            // 光标位置
            WS_GetCursorPos(&pt);

            dx += pt.x;
            dy += pt.y;

            dwFlags |= MOUSEEVENTF_ABSOLUTE;
        }
        
#if (!NOCURSOR)

        // Clips the mouse position by the cursor clip rectangle
        pt.x = dx;
        pt.y = dy;
        WS_ClipCursorPoint(&pt);

        // 光标位置改变, 移动光标的位置
        if (pt.x != ptCursor.x || pt.y != ptCursor.y)
        {
            ptCursor.x = pt.x;
            ptCursor.y = pt.y;

            bCursorMove = TRUE;
            pGraphDev = GRAPHDEV_GetCursorScreen(0);
            pGraphDev->drv.SetCursorPos(pGraphDev->pDev, pt.x, pt.y);
            bCursorMove = FALSE;
        }

#endif // NOCURSOR

        // 用户事件队列满, 返回FALSE
        if ((pUser->tail + 1) % USER_QUEUE_SIZE == pUser->head)
        {
            ASSERT(0);
            return FALSE;
        }

        // 将新的事件插入到队列尾

        pEvent = &pUser->events[pUser->tail];

        pEvent->type                    = DE_MOUSE;
        pEvent->event.mouse.pt.x        = dx;
        pEvent->event.mouse.pt.y        = dy;
        pEvent->event.mouse.mouseData   = dwData;
        pEvent->event.mouse.flags       = dwFlags;
        pEvent->event.mouse.time        = GetTickCount();
        pEvent->event.mouse.dwExtraInfo = dwExtraInfo;

        pUser->tail = (pUser->tail + 1) % USER_QUEUE_SIZE;
    }
    else
    {
        // 设备鼠标事件
        pDev = &(pThreadInfo->InputEventQue.DevEventQueue);

        // 如果是相对坐标, 转换为绝对坐标
        if (!(dwFlags & MOUSEEVENTF_ABSOLUTE))
        {
            dx += ptCursor.x;
            dy += ptCursor.y;

            dwFlags |= MOUSEEVENTF_ABSOLUTE;
        }

#if (!NOCURSOR)

        // Clips the mouse position by the cursor clip rectangle
        pt.x = dx;
        pt.y = dy;
        WS_ClipCursorPoint(&pt);

        // 光标位置改变, 移动光标的位置
        if (pt.x != ptCursor.x || pt.y != ptCursor.y)
        {
            ptCursor.x = pt.x;
            ptCursor.y = pt.y;

//            if (!bCursorMove)
//                g_DisplayDrv.SetCursorPos(g_pDisplayDev, pt.x, pt.y);
        }

#endif // NOCURSOR

        // 如果窗口任务有处于阻塞状态, 发送消息唤醒窗口任务, 无论是不是鼠标
        // 可以合并的鼠标移动事件，必须发送消息唤醒窗口任务，否则可能会导致
        // 窗口任务延迟唤醒

        if (bMoveMerge)
        {
            // 是鼠标移动事件并且设备事件队列不空, 判断是否前一个鼠标事件是
            // 鼠标移动事件, 若是, 合并
            if ((pDev->head != pDev->tail) && (dwFlags & MOUSEEVENTF_MOVE))
            {
                int i;
                
                // 寻找最后一个鼠标事件
                i = (pDev->tail + DEV_QUEUE_SIZE - 1) % 
                    DEV_QUEUE_SIZE;
                while (i != pDev->head)
                {
                    // 找到鼠标事件, 退出循环
                    if (pDev->events[i].type == DE_MOUSE)
                        break;
                    
                    i = (i + DEV_QUEUE_SIZE - 1) % DEV_QUEUE_SIZE;
                }
                
                // 找到鼠标事件
                pEvent = &pDev->events[i];
                if (pEvent->type == DE_MOUSE)
                {
                    // 当前鼠标移动事件上一个鼠标移动事件位置相同, 忽略当前
                    // 鼠标事件直接返回
                    if (pEvent->event.mouse.pt.x == dx &&
                        pEvent->event.mouse.pt.y == dy)
                    {
                        pThreadInfo->dwState |= QS_INPUT;
                        if (ISBLOCKINGET(pThreadInfo))
                        {
                            pThreadInfo->bBlock = BLOCK_NO;
                            WAKEUP(pThreadInfo);
                        }
                        return TRUE;
                    }
                    
                    // 找到鼠标移动事件, 使用新的移动事件替代并返回
                    if (pEvent->event.mouse.flags & MOUSEEVENTF_MOVE)
                    {
                        pEvent->event.mouse.pt.x        = dx;
                        pEvent->event.mouse.pt.y        = dy;
                        pEvent->event.mouse.mouseData   = dwData;
                        pEvent->event.mouse.flags       = dwFlags;
                        pEvent->event.mouse.time        = GetTickCount();
                        pEvent->event.mouse.dwExtraInfo = dwExtraInfo;

                        pThreadInfo->dwState |= QS_INPUT;
                        if (ISBLOCKINGET(pThreadInfo))
                        {
                            pThreadInfo->bBlock = BLOCK_NO;
                            WAKEUP(pThreadInfo);
                        }
                        return TRUE;
                    }
                }
            }
        }

        // 设备事件队列满, 返回FALSE
        if ((pDev->tail + 1) % DEV_QUEUE_SIZE == pDev->head)
            return FALSE;

        // 将新的事件插入到队列尾

        pEvent = &pDev->events[pDev->tail];

        pEvent->type                    = DE_MOUSE;
        pEvent->event.mouse.pt.x        = dx;
        pEvent->event.mouse.pt.y        = dy;
        pEvent->event.mouse.mouseData   = dwData;
        pEvent->event.mouse.flags       = dwFlags;
        pEvent->event.mouse.time        = GetTickCount();
        pEvent->event.mouse.dwExtraInfo = dwExtraInfo;

        pDev->tail = (pDev->tail + 1) % DEV_QUEUE_SIZE;
    }

    pThreadInfo->dwState |= QS_INPUT;
    if (ISBLOCKINGET(pThreadInfo))
    {
        pThreadInfo->bBlock = BLOCK_NO;
        WAKEUP(pThreadInfo);
    }
    
    return TRUE;
}

/*
**  Function : DEVQ_RemoveInputEvents
**  Purpose  :
**      Removes all device input events from device input event queue.
*/
void DEVQ_RemoveInputEvents(PTHREADINFO pThreadInfo)
{
    if (pThreadInfo->InputEventQue.DevEventQueue.head != 
        pThreadInfo->InputEventQue.DevEventQueue.tail)
    {
        pThreadInfo->InputEventQue.DevEventQueue.head = 
            pThreadInfo->InputEventQue.DevEventQueue.tail;
        nKbdEventCount = 0;
    }

    if (pThreadInfo->InputEventQue.UserEventQueue.head != 
        pThreadInfo->InputEventQue.UserEventQueue.tail)
    {
        pThreadInfo->InputEventQue.UserEventQueue.head = 
            pThreadInfo->InputEventQue.UserEventQueue.tail;
    }
}

/*
**  Function : SetPenMoveMerge
**  Purpose  :
**      API函数，供应用程序调用设置是否进行鼠标（笔）移动消息的合并，只有
**      PDA平台调用，暂时放到这里。
*/
void SetPenMoveMerge(BOOL flag)
{
	bMoveMerge = flag;
}
