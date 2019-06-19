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

//static DEVEVENTQUEUE DevEventQueue;     // �豸�¼�����
//static USEREVENTQUEUE UserEventQueue;   // �û��¼�����

// �豸�¼������м����¼��ĸ���
static int volatile nKbdEventCount;

// ���������һ�������¼��Ĵ��ڿ���
#define ME_NODOWN       0   // ��ʾ������û����갴���¼�
#define ME_DOWN         1   // ��ʾ����������갴���¼�
#define ME_DOWNUP       2   // ��ʾ����������갴���¼���̧���¼�

// �����������������Ҽ����м��¼���״̬
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

    // ����û��¼����������¼�, ���û��¼�������ȡһ���¼�
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

    // �û��¼�������û���¼�, ���豸�¼�������ȡһ���¼�, �豸�¼����п�, 
    // ֱ�ӷ���FALSE
    if (pDev->head == pDev->tail)
    {
        // �����¼�������0, �Ա�����������жϲ�����������
        nKbdEventCount = 0;

        pThreadInfo->dwState &= ~QS_INPUT;
        return FALSE;
    }

    // �豸�¼����������¼�, ���豸�¼�������ȡһ���¼�

    *pDevEvent = pDev->events[pDev->head];
    
    if (bRemove)
    {
        pDev->head = (pDev->head + 1) % DEV_QUEUE_SIZE;

        // ȡ�ߵ��Ǽ����¼�, ���ټ����¼�����
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
        // �û������¼�, �ӵ��豸�¼�������

        // �û��¼�������, ����FALSE
        if ((pUser->tail + 1) % USER_QUEUE_SIZE == pUser->head)
        {
            ASSERT(0);
            return FALSE;
        }

        // ���µ��¼����뵽����β

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
        // �豸�����¼�, �ӵ��豸�¼�������

        // �豸�¼�������, ����FALSE
        // if (nKbdEventCount >= MAX_KBD_EVENTS)
        if ((pDev->tail + 1) % DEV_QUEUE_SIZE == pDev->head)
            return FALSE;

        // �Ǽ��̰����¼������豸�¼����в���, �ж��Ƿ�ǰһ�������¼���ͬһ
        // �����̰����¼�, ����, �ϲ�
        if ((pDev->head != pDev->tail) && !(dwFlags & KEYEVENTF_KEYUP))
        {
            int i;
            
            // Ѱ�����һ�������¼�
            i = (pDev->tail + DEV_QUEUE_SIZE - 1) % DEV_QUEUE_SIZE;
            while (i != pDev->head)
            {
                // �ҵ�����¼�, �˳�ѭ��
                if (pDev->events[i].type == DE_KEYBOARD)
                    break;
                
                i = (i + DEV_QUEUE_SIZE - 1) % DEV_QUEUE_SIZE;
            }
            
            // �ҵ������¼�
            pEvent = &pDev->events[i];
            if (pEvent->type == DE_KEYBOARD)
            {
                // ��ǰ���̰����¼����ҵ�����һ�������¼���ͬ, ���Ե�ǰ
                // ���̰����¼�ֱ�ӷ���
                if (pEvent->event.kbd.vkCode == byVirtKey &&
                    pEvent->event.kbd.flags == dwFlags)
                {
                    ASSERT(pThreadInfo->dwState & QS_INPUT);
                    return TRUE;
                }
            }
        }

        // ���µ��¼����뵽����β

        pEvent = &pDev->events[pDev->tail];

        pEvent->type                  = DE_KEYBOARD;
        pEvent->event.kbd.vkCode      = byVirtKey;
        pEvent->event.kbd.scanCode    = byScan;
        pEvent->event.kbd.flags       = dwFlags;
        pEvent->event.kbd.time        = GetTickCount();
        pEvent->event.kbd.dwExtraInfo = dwExtraInfo;

        pDev->tail = (pDev->tail + 1) % DEV_QUEUE_SIZE;

        // ���Ӽ����¼�����
        nKbdEventCount++;
    }

    // ������������д�������״̬, ������Ϣ���Ѵ�������
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
    static BOOL bCursorMove;    // �Ƿ��û�����¼������ƶ����
    PGRAPHDEV   pGraphDev;
#endif

    static POINT ptCursor;      // �����������λ��

    PDEVEVENT pEvent;
    POINT pt;

    PUSEREVENTQUEUE pUser;
    PDEVEVENTQUEUE pDev;


    if (!(dwFlags & EVENTF_FROMDEVICE))
    {
        // �û�����¼�
        pUser = &(pThreadInfo->InputEventQue.UserEventQueue);

        // ������������, ת��Ϊ��������
        if (!(dwFlags & MOUSEEVENTF_ABSOLUTE))
        {
            // ����ʹ�ñ�ģ�鱣��Ĺ���λ��, Ӧʹ�ù�����ģ�鱣���
            // ���λ��
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

        // ���λ�øı�, �ƶ�����λ��
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

        // �û��¼�������, ����FALSE
        if ((pUser->tail + 1) % USER_QUEUE_SIZE == pUser->head)
        {
            ASSERT(0);
            return FALSE;
        }

        // ���µ��¼����뵽����β

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
        // �豸����¼�
        pDev = &(pThreadInfo->InputEventQue.DevEventQueue);

        // ������������, ת��Ϊ��������
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

        // ���λ�øı�, �ƶ�����λ��
        if (pt.x != ptCursor.x || pt.y != ptCursor.y)
        {
            ptCursor.x = pt.x;
            ptCursor.y = pt.y;

//            if (!bCursorMove)
//                g_DisplayDrv.SetCursorPos(g_pDisplayDev, pt.x, pt.y);
        }

#endif // NOCURSOR

        // ������������д�������״̬, ������Ϣ���Ѵ�������, �����ǲ������
        // ���Ժϲ�������ƶ��¼������뷢����Ϣ���Ѵ������񣬷�����ܻᵼ��
        // ���������ӳٻ���

        if (bMoveMerge)
        {
            // ������ƶ��¼������豸�¼����в���, �ж��Ƿ�ǰһ������¼���
            // ����ƶ��¼�, ����, �ϲ�
            if ((pDev->head != pDev->tail) && (dwFlags & MOUSEEVENTF_MOVE))
            {
                int i;
                
                // Ѱ�����һ������¼�
                i = (pDev->tail + DEV_QUEUE_SIZE - 1) % 
                    DEV_QUEUE_SIZE;
                while (i != pDev->head)
                {
                    // �ҵ�����¼�, �˳�ѭ��
                    if (pDev->events[i].type == DE_MOUSE)
                        break;
                    
                    i = (i + DEV_QUEUE_SIZE - 1) % DEV_QUEUE_SIZE;
                }
                
                // �ҵ�����¼�
                pEvent = &pDev->events[i];
                if (pEvent->type == DE_MOUSE)
                {
                    // ��ǰ����ƶ��¼���һ������ƶ��¼�λ����ͬ, ���Ե�ǰ
                    // ����¼�ֱ�ӷ���
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
                    
                    // �ҵ�����ƶ��¼�, ʹ���µ��ƶ��¼����������
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

        // �豸�¼�������, ����FALSE
        if ((pDev->tail + 1) % DEV_QUEUE_SIZE == pDev->head)
            return FALSE;

        // ���µ��¼����뵽����β

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
**      API��������Ӧ�ó�����������Ƿ������꣨�ʣ��ƶ���Ϣ�ĺϲ���ֻ��
**      PDAƽ̨���ã���ʱ�ŵ����
*/
void SetPenMoveMerge(BOOL flag)
{
	bMoveMerge = flag;
}
