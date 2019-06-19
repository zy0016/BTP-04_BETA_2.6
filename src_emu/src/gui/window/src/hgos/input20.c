/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Manages the input device.
 *            
\**************************************************************************/

#if defined(_DDI_VER)

#if (_DDI_VER >= 200)

#include "ddi.h"
#include "devman.h"

static PDEVICE pMouseDev;
static PDEVICE pKbdDev;
static PDEVICE pRControlDev;

// Internal function prototype
static int KbdCallBack(LONG lUserData, DWORD dwEvent, DWORD param1, 
                       LONG param2);
static int MouseCallBack(LONG lUserData, DWORD dwEvent, DWORD param1, 
                         LONG param2);
static int RControlCallBack(LONG lUserData, DWORD dwEvent, DWORD param1, 
                            LONG param2);

#else // _DDI_VER >= 200

#include "fileapi.h"
#include "device.h"

static int keyboard;
static int mouse;

typedef struct
{
    unsigned short event_mask;
    unsigned short unused;
	void (*callback_func)(void* para, int event);
	void* callback_para;
} IOCTLDATA;

// Internal function prototype
static void KbdCallBack(void* para, int event);
static void MouseCallBack(void* para, int event);

#endif  // _DDI_VER >= 200

typedef struct
{
    unsigned char   vkey;
    unsigned char   ascii;
    unsigned char   scancode;
    unsigned char   shiftstate;
    unsigned long   time;
} KEYBOARDEVENT;

typedef struct
{
    short   dx;
    short   dy;
    unsigned short  btn_stat;
    unsigned short  time;
} MOUSEEVENT;

typedef struct
{
    int type;
    union
    {
        struct RMEVENT
        {
            short   dx;
            short   dy;
            unsigned short  btn_stat;
        }mouse;
        struct RKEVENT
        {
            BYTE    vkey;
            BYTE    shiftstate;
            BYTE    scancode;   
        }kbd;
    }e;
} RCONTROLEVENT;

#define BTN_LEFTDOWN    0x0001
#define BTN_RIGHTDOWN   0x0002
#define BTN_MIDDLEDOWN  0x0004

static WORD btn_stat;

typedef struct
{
    PTHREADINFO pFgThreadInfo;  // ǰ���߳�
    PWINOBJ     pFgWin;

    WORD    wKeyState;          // Special key state
    WORD    wBtnState;          // Mouse button state 

    WORD    wKeyAfterSpecKey;   // the pressed key after control key
    WORD    wLastChar;          // last char code
//    PWINOBJ pPenCalibrationWin; // The window used to calibrate the pen
} INPUTDEVDATA, *PINPUTDEVDATA;

static INPUTDEVDATA InputDevData;

// ���滷��ʹ�þ�������, y��������ʵ�����෴
#ifdef _EMULATE_
#define MOUSE_EVENT(dwFlags, dx, dy, dwData, dwExtraInfo) \
    MouseEvent((dwFlags) | MOUSEEVENTF_ABSOLUTE, dx, -(dy), dwData, \
        dwExtraInfo)
#else   // _EMULATE_
#define MOUSE_EVENT MouseEvent
#endif   // _EMULATE_

static void ChangeFgThread(DWORD dwThreadId, int nActive);
PWINOBJ WND_FromPointEx(int x, int y);

/*
**  Function : INPUTMGR_Init
*/
BOOL INPUTMGR_Init(void)
{
#if (_DDI_VER >= 200)

    // ��ü����豸����ָ��, ��ʼ�������豸
    if (GetDeviceInterface("KEYBOARD", (void**)&pKbdDev))
    {
        ASSERT(pKbdDev != NULL);
        
        // ���ü����豸���¼�����ͻص�����
        IDevice_SysControl(pKbdDev, SYS_SETEVENTMASK, (DWORD)-1, 0);
        IDevice_SysControl(pKbdDev, SYS_SETCALLBACK, (DWORD)KbdCallBack, 
            0);
        
        // �򿪼����豸
        IDevice_Open(pKbdDev);
    }

    // �������豸����ָ��, ��ʼ������豸
    if (GetDeviceInterface("MOUSE", (void**)&pMouseDev))
    {
        ASSERT(pMouseDev != NULL);
        
        // ��������豸���¼�����ͻص�����
        IDevice_SysControl(pMouseDev, SYS_SETEVENTMASK, (DWORD)-1, 0);
        IDevice_SysControl(pMouseDev, SYS_SETCALLBACK, 
            (DWORD)MouseCallBack, 0);
        
        // ������豸
        IDevice_Open(pMouseDev);
    }

    // ���ң���豸����ָ��, ��ʼ��ң���豸
    if (GetDeviceInterface("RCONTROL", (void**)&pRControlDev))
    {
        ASSERT(pRControlDev != NULL);
        
        // ����ң���豸���¼�����ͻص�����
        IDevice_SysControl(pRControlDev, SYS_SETEVENTMASK, (DWORD)-1, 0);
        IDevice_SysControl(pRControlDev, SYS_SETCALLBACK, 
            (DWORD)RControlCallBack, 0);
        
        // ��ң���豸
        IDevice_Open(pRControlDev);
    }

#else // _DDI_VER >= 200

    IOCTLDATA ioctl_data;

    keyboard = OpenFile("DEV:/KEYBOARD", 0, 0, 0);
    mouse = OpenFile("DEV:/MOUSE", 0, 0, 0);

    if (keyboard > 0)
    {
        ioctl_data.event_mask = (unsigned short)0xFFFF;
        ioctl_data.unused = 0;
        ioctl_data.callback_func = KbdCallBack;
        ioctl_data.callback_para = 0;
        FILE_ioctl(keyboard, IO_SETCALLBACK, &ioctl_data, sizeof(IOCTLDATA));
    }

    if (mouse > 0)
    {
        ioctl_data.event_mask = (unsigned short)0xFFFF;
        ioctl_data.unused = 0;
        ioctl_data.callback_func = MouseCallBack;
        ioctl_data.callback_para = 0;
        FILE_ioctl(mouse, IO_SETCALLBACK, &ioctl_data, sizeof(IOCTLDATA));
    }

#endif

    InputDevData.pFgThreadInfo = WS_GetThreadInfo(g_pRootWin->dwThreadId);

    return TRUE;
}

/*
**  Function : INPUTMGR_Exit
*/
void INPUTMGR_Exit(void)
{
}

#if (_DDI_VER >= 200)
static int KbdCallBack(LONG lUserData, DWORD dwEvent, DWORD param1, 
                       LONG param2)
#else // _DDI_VER >= 200   
static void KbdCallBack(void* para, int event)
#endif// _DDI_VER >= 200   
{
    KEYBOARDEVENT kb_event;

#if (_DDI_VER >= 200)
    while (IDevice_Read(pKbdDev, &kb_event, sizeof(KEYBOARDEVENT)) > 0)
#else // _DDI_VER >= 200   
    while (ReadFile(keyboard, &kb_event, sizeof(KEYBOARDEVENT)) > 0 )
#endif // _DDI_VER >= 200   
    {
        KeyboardEvent(kb_event.vkey, kb_event.scancode, 
            (kb_event.shiftstate & 0x01) ? KEYEVENTF_FROMDEVICE : 
            KEYEVENTF_KEYUP | KEYEVENTF_FROMDEVICE, 0);
    }

#if (_DDI_VER >= 200)
    return 1;
#endif // _DDI_VER >= 200
}

#if (_DDI_VER >= 200)
static int MouseCallBack(LONG lUserData, DWORD dwEvent, DWORD param1, 
                         LONG param2)
#else // _DDI_VER >= 200
static void MouseCallBack(void* para, int event)
#endif // _DDI_VER >= 200   
{
    MOUSEEVENT ms_event;
    int x, y;

#if (_DDI_VER >= 200)
    while (IDevice_Read(pMouseDev, &ms_event, sizeof(MOUSEEVENT)) > 0 )
#else // _DDI_VER >= 200
    while (ReadFile(mouse, &ms_event, sizeof(MOUSEEVENT)) > 0 )
#endif // _DDI_VER >= 200   
    {
        ms_event.btn_stat &= 0x0007;
        if (btn_stat == ms_event.btn_stat)
        {
            // ��������ƶ��¼�
            if (ms_event.dx != 0 || ms_event.dy != 0)
            {
                MOUSE_EVENT(MOUSEEVENTF_MOVE | MOUSEEVENTF_FROMDEVICE, 
                    ms_event.dx, -ms_event.dy, 0, 0);
            }
        }
        else
        {
            // ���İ���״̬�ı�, ������Ӧ����¼�
            
            if ((btn_stat & BTN_LEFTDOWN) != 
                (ms_event.btn_stat & BTN_LEFTDOWN))
            {
                if (ms_event.btn_stat & BTN_LEFTDOWN)
                {
                    MOUSE_EVENT(
                        MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_FROMDEVICE, 
                        ms_event.dx, -ms_event.dy, 0, 0);
                }
                else
                {
                    // ������̧���¼�ͬʱ���ƶ�, ��������ƶ��¼������
                    // ̧���¼�
                    if (ms_event.dx != 0 || ms_event.dy != 0)
                    {
                        MOUSE_EVENT(
                            MOUSEEVENTF_MOVE | MOUSEEVENTF_FROMDEVICE, 
                            ms_event.dx, -ms_event.dy, 0, 0);
                    }

#ifdef _EMULATE_
                    x = ms_event.dx;
                    y = -ms_event.dy;
#else // _EMULATE_
                    x = y = 0;
#endif // _EMULATE_

                    MOUSE_EVENT(
                        MOUSEEVENTF_LEFTUP | MOUSEEVENTF_FROMDEVICE, 
						x, y, 0, 0);
                }
                
                btn_stat ^= BTN_LEFTDOWN;
            }

            if ((btn_stat & BTN_RIGHTDOWN) != 
                (ms_event.btn_stat & BTN_RIGHTDOWN))
            {
                if (ms_event.btn_stat & BTN_RIGHTDOWN)
                {
                    MOUSE_EVENT(
                        MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_FROMDEVICE, 
                        ms_event.dx, -ms_event.dy, 0, 0);
                }
                else
                {
                    // ������̧���¼�ͬʱ���ƶ�, ��������¼������̧��
                    // �ƶ��¼�

                    if (ms_event.dx != 0 || ms_event.dy != 0)
                    {
                        MOUSE_EVENT(
                            MOUSEEVENTF_MOVE | MOUSEEVENTF_FROMDEVICE, 
                            ms_event.dx, -ms_event.dy, 0, 0);
                    }

#ifdef _EMULATE_
                    x = ms_event.dx;
                    y = -ms_event.dy;
#else // _EMULATE_
                    x = y = 0;
#endif // _EMULATE_

                    MOUSE_EVENT(
                        MOUSEEVENTF_RIGHTUP | MOUSEEVENTF_FROMDEVICE, 
						x, y, 0, 0);

                }
                
                btn_stat ^= BTN_RIGHTDOWN;                    
            }

            if ((btn_stat & BTN_MIDDLEDOWN) != 
                (ms_event.btn_stat & BTN_MIDDLEDOWN))
            {
                if (ms_event.btn_stat & BTN_MIDDLEDOWN)
                {
                    MOUSE_EVENT(
                        MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_FROMDEVICE, 
                        ms_event.dx, -ms_event.dy, 0, 0);
                }
                else
                {
                    // ������̧���¼�ͬʱ���ƶ�, ��������ƶ��¼������
                    // ̧���¼�

                    if (ms_event.dx != 0 || ms_event.dy != 0)
                    {
                        MOUSE_EVENT(
                            MOUSEEVENTF_MOVE | MOUSEEVENTF_FROMDEVICE, 
                            ms_event.dx, -ms_event.dy, 0, 0);
                    }

#ifdef _EMULATE_
                    x = ms_event.dx;
                    y = -ms_event.dy;
#else // _EMULATE_
                    x = y = 0;
#endif // _EMULATE_

                    MOUSE_EVENT(
                        MOUSEEVENTF_MIDDLEUP | MOUSEEVENTF_FROMDEVICE, 
						x, y, 0, 0);
                }
                
                btn_stat ^= BTN_MIDDLEDOWN;                    
            }
        }
    }

#if (_DDI_VER >= 200)
    return 1;
#endif // _DDI_VER >= 200
}

#if (_DDI_VER >= 200)

static int RControlCallBack(LONG lUserData, DWORD dwEvent, DWORD param1, 
                            LONG param2)
{
    RCONTROLEVENT event;

    while (IDevice_Read(pRControlDev, &event, sizeof(RCONTROLEVENT)) > 0 )
    {
        if (event.type == 0)
        {
            // �����¼�
            keybd_event(event.e.kbd.vkey, event.e.kbd.scancode, 
                (event.e.kbd.shiftstate & 0x01) ? KEYEVENTF_FROMDEVICE : 
            KEYEVENTF_KEYUP | KEYEVENTF_FROMDEVICE, 0);
        }
        else
        {
            // ����¼�
            event.e.mouse.btn_stat &= 0x0007;
            if (btn_stat == event.e.mouse.btn_stat)
            {
                // ��������ƶ��¼�
                if (event.e.mouse.dx != 0 || event.e.mouse.dy != 0)
                {
                    MOUSE_EVENT(MOUSEEVENTF_MOVE | MOUSEEVENTF_FROMDEVICE, 
                        event.e.mouse.dx, -event.e.mouse.dy, 0, 0);
                }
            }
            else
            {
                // ���İ���״̬�ı�, ������Ӧ����¼�
                
                if ((btn_stat & BTN_LEFTDOWN) != 
                    (event.e.mouse.btn_stat & BTN_LEFTDOWN))
                {
                    if (event.e.mouse.btn_stat & BTN_LEFTDOWN)
                    {
                        MOUSE_EVENT(
                            MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_FROMDEVICE, 
                            event.e.mouse.dx, -event.e.mouse.dy, 0, 0);

                        // ң�����豸������̧���¼�������̧���¼�
                        MOUSE_EVENT(
                            MOUSEEVENTF_LEFTUP | MOUSEEVENTF_FROMDEVICE, 
                            0, 0, 0, 0);
                    }
                    else
                    {
                        // ң�����豸������̧���¼�
                    }
                    
                    // ����Ҫ�ı�������״̬
                }
                
                if ((btn_stat & BTN_RIGHTDOWN) != 
                    (event.e.mouse.btn_stat & BTN_RIGHTDOWN))
                {
                    if (event.e.mouse.btn_stat & BTN_RIGHTDOWN)
                    {
                        MOUSE_EVENT(
                            MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_FROMDEVICE, 
                            event.e.mouse.dx, -event.e.mouse.dy, 0, 0);

                        // ң�����豸������̧���¼�������̧���¼�
                        MOUSE_EVENT(
                            MOUSEEVENTF_RIGHTUP | MOUSEEVENTF_FROMDEVICE, 
                            0, 0, 0, 0);
                    }
                    else
                    {
                        // ң�����豸������̧���¼�
                    }
                    
                    // ����Ҫ�ı�����Ҽ�״̬
                }
            }
        }
    }

    return 1;
}

#endif // _DDI_VER >= 200

#else   // _DDI_VER

static void UnusedFunc(void)
{
}

#endif  // _DDI_VER

/************************************************************************************
**  Function : KeyboardEvent
**  Purpose  : 
************************************************************************************/
void KeyboardEvent(int byVirtKey, int bScan, DWORD dwFlags, DWORD dwExtraInfo)
{
    {
        /*�����ȼ�*/
    }

    ENTERMONITOR;

    DEVQ_PutKeyboardEvent(InputDevData.pFgThreadInfo, 
        (BYTE)byVirtKey, (BYTE)bScan, dwFlags, dwExtraInfo);

    LEAVEMONITOR;
}

void MouseEvent(DWORD dwFlags, LONG dx, LONG dy, DWORD dwData, DWORD dwExtraInfo)
{
    PWINOBJ pWin = NULL;
    PWINOBJ pWinFromPoint = NULL;
    POINT   pt;

    ENTERMONITOR;

    if (!(dwFlags & MOUSEEVENTF_ABSOLUTE))
    {
        // Gets current mouse position
        WS_GetCursorPos(&pt);

        //convert relative coord to absolute coord
        dx += pt.x;
        dy += pt.y;

        dwFlags |= MOUSEEVENTF_ABSOLUTE;
    }

    /* �Ѿ��ж���͸����� */
    pWinFromPoint = WND_FromPointEx(dx, dy);

    {
        //����һЩ�����ȼ��Ĳ���������ر�һ�������߳�
    }

    ASSERT(pWinFromPoint != NULL);
    if (pWinFromPoint == NULL)
    {
        LEAVEMONITOR;
        return ;
    }

    //find the target win
    pWin = InputDevData.pFgThreadInfo->pCaptureWin;
    if (pWin == NULL)
        pWin = pWinFromPoint;
    ASSERT(pWin != NULL);

    if (dwFlags & MOUSEEVENTF_MOVE)
    {
        /* ֻ�е�������ǲ��񴰿�ʱ�����ܵõ��κ�λ�õĹ����Ϣ ������
        ** ��ʹ�в��񴰿ڣ������ǻ����ʱ��Ҳ�ò������������Ϣ
        */
        if (InputDevData.pFgThreadInfo->pCaptureWin &&
            ((InputDevData.wBtnState & MK_LBUTTON) ||
            (InputDevData.wBtnState & MK_RBUTTON) ||
            (InputDevData.wBtnState & MK_MBUTTON)))
        {
            /* ǰ���̻߳���ڲ������ */
            DEVQ_PutMouseEvent(InputDevData.pFgThreadInfo, dwFlags, dx, dy, 
                dwData, dwExtraInfo, NULL);//InputDevData.pFgThreadInfo->pCaptureWin->handle);
        }
        else if ((InputDevData.wBtnState & MK_LBUTTON) ||
            (InputDevData.wBtnState & MK_RBUTTON) ||
            (InputDevData.wBtnState & MK_MBUTTON))
        {
            /* �м����ڰ���״̬���ƶ���꣬����Ϣֻ�ܷ���ǰ���̵߳Ļ���� */
            DEVQ_PutMouseEvent(InputDevData.pFgThreadInfo, dwFlags, dx, dy, 
                dwData, dwExtraInfo, NULL);//InputDevData.pFgThreadInfo->pActiveWin->handle);
        }
        else
        {
            /* ��������ƶ���Ϣֱ�ӷ���Ŀ�괰�� */
            DEVQ_PutMouseEvent(WS_GetThreadInfo(pWinFromPoint->dwThreadId), 
                dwFlags, dx, dy, dwData, dwExtraInfo, NULL);//pWinFromPoint->handle);
        }
    }
    else if ((dwFlags & MOUSEEVENTF_LEFTUP) || 
        (dwFlags & MOUSEEVENTF_RIGHTUP) ||
        (dwFlags & MOUSEEVENTF_MIDDLEUP))
    {
        /* ֻ�е������Ϊ���񴰿�ʱ���յ���Ϣ */
        if (InputDevData.pFgThreadInfo->pCaptureWin)
        {
            DEVQ_PutMouseEvent(InputDevData.pFgThreadInfo, dwFlags, dx, dy, 
                dwData, dwExtraInfo, NULL);//InputDevData.pFgThreadInfo->pActiveWin);
        }
        else if ((InputDevData.wBtnState & MK_LBUTTON) ||
            (InputDevData.wBtnState & MK_RBUTTON) ||
            (InputDevData.wBtnState & MK_MBUTTON))
        {
            /* �м����ڰ���״̬��̧����꣬����ϢӦ����ǰ���̵߳Ļ���� */
            DEVQ_PutMouseEvent(InputDevData.pFgThreadInfo, dwFlags, dx, dy, 
                dwData, dwExtraInfo, NULL);//InputDevData.pFgThreadInfo->pActiveWin->handle);
        }
        else
        {
            DEVQ_PutMouseEvent(WS_GetThreadInfo(pWinFromPoint->dwThreadId), 
                dwFlags, dx, dy, dwData, dwExtraInfo, WOT_GetHandle((PWSOBJ)pWinFromPoint));
        }
    }
    else //�м�����
    {
//        BOOL bCapture = FALSE, bChange = FALSE;
        DWORD dwFlagsUp;
        /* ���Ŀ�괰�ڲ��ǵ�ǰǰ���̣߳���Ҫ�л�ǰ���̣߳������µ�ǰ���߳�
        ** ����Ϣ��
        ** ��ʱ�Ƿ�Ӧ��ֻת��һ��ǰ���̵߳�״̬��������λ�á�Z��ĸı�ŵ�
        ** �µ�ǰ���߳���ȥ��������˵�����ȫ�����ˡ�
        ** ����Ҫ����ʱ��һ������!!!
        */
        ASSERT((dwFlags & MOUSEEVENTF_LEFTDOWN) || 
            (dwFlags & MOUSEEVENTF_MIDDLEDOWN) ||
            (dwFlags & MOUSEEVENTF_RIGHTDOWN));
        if (pWinFromPoint->dwThreadId != InputDevData.pFgThreadInfo->dwThreadId)
        {
            if (InputDevData.pFgThreadInfo->pCaptureWin)
            {
                DEVQ_PutMouseEvent(InputDevData.pFgThreadInfo, 
                    dwFlags, dx, dy, dwData, 
                    dwExtraInfo, NULL);
                
                if (dwFlags & MOUSEEVENTF_LEFTDOWN)
                    dwFlagsUp = (dwFlags & ~MOUSEEVENTF_LEFTDOWN) | MOUSEEVENTF_LEFTUP;
                else if (dwFlags & MOUSEEVENTF_MIDDLEDOWN)
                    dwFlagsUp = (dwFlags & ~MOUSEEVENTF_MIDDLEDOWN) | MOUSEEVENTF_MIDDLEUP;
                else if (dwFlags & MOUSEEVENTF_RIGHTDOWN)
                    dwFlagsUp = (dwFlags & ~MOUSEEVENTF_RIGHTDOWN) | MOUSEEVENTF_RIGHTUP;
                
                DEVQ_PutMouseEvent(InputDevData.pFgThreadInfo, 
                    dwFlagsUp, dx, dy, dwData, 
                    dwExtraInfo, NULL);
            }

            ChangeFgThread(pWinFromPoint->dwThreadId, WA_CLICKACTIVE);

//            bChange = TRUE;
        }

//        if (bChange || !bCapture)
        {
            DEVQ_PutMouseEvent(InputDevData.pFgThreadInfo, dwFlags, dx, dy, 
                dwData, dwExtraInfo, WOT_GetHandle((PWSOBJ)pWinFromPoint));
        }
    }

    //modify InputDevData.wBtnState�޸�ԭʼ�����״̬
    if (dwFlags & MOUSEEVENTF_LEFTDOWN)
    {
        if (!(InputDevData.wBtnState & MK_LBUTTON))
            InputDevData.wBtnState |= MK_LBUTTON;
        // If the mouse left button is already down, convert
        // the mouse button down event to mouse move event
    }
    else if (dwFlags & MOUSEEVENTF_LEFTUP)
    {
        if (InputDevData.wBtnState & MK_LBUTTON)
            InputDevData.wBtnState &= (WORD)~MK_LBUTTON;
        // If the mouse left button isn't down, convert
        // the mouse up event to mouse move event
    }
    else if (dwFlags & MOUSEEVENTF_RIGHTDOWN)
    {
        if (!(InputDevData.wBtnState & MK_RBUTTON))
            InputDevData.wBtnState |= MK_RBUTTON;
        // If the mouse right button is already down, convert
        // the mouse down event to mouse move event
    }
    else if (dwFlags & MOUSEEVENTF_RIGHTUP)
    {
        if (InputDevData.wBtnState & MK_RBUTTON)
            InputDevData.wBtnState &= (WORD)~MK_RBUTTON;
        // If the mouse right button isn't down, convert
        // the mouse up event to mouse move event
    }
    else if (dwFlags & MOUSEEVENTF_MIDDLEDOWN)
    {
        if (!(InputDevData.wBtnState & MK_MBUTTON))
            InputDevData.wBtnState |= MK_MBUTTON;
        // If the mouse middle button is already down, convert
        // the mouse down event to mouse move event
    }
    else if (dwFlags & MOUSEEVENTF_MIDDLEUP)
    {
        if (InputDevData.wBtnState & MK_MBUTTON)
            InputDevData.wBtnState &= (WORD)~MK_MBUTTON;
        // If the mouse middle button isn't down, convert
        // the mouse up event to mouse move event
    }
    else if (!(dwFlags & MOUSEEVENTF_MOVE))
    {
    }

    LEAVEMONITOR;
    return;
}

/* �ܳ��ڲ����� */
static void ChangeFgThread(DWORD dwThreadId, int nActive)
{
    PWINOBJ pOldFocusWin, pNewFocusWin;
    ASSERT (dwThreadId != InputDevData.pFgThreadInfo->dwThreadId);
//    SetNewActiveWindow(pWin, nActive);

    pOldFocusWin = InputDevData.pFgThreadInfo->pFocusWin;
    /* �޸�ǰ���߳� */
    if (InputDevData.pFgThreadInfo->pCaptureWin != NULL)
    {
/*
        WND_SendNotifyMessage(InputDevData.pFgThreadInfo->pCaptureWin, 
            WM_CANCELMODE, 0, 0);
*/
//        InputDevData.pFgThreadInfo->pCaptureWin = NULL;
    }
    InputDevData.pFgThreadInfo = WS_GetThreadInfo(dwThreadId);

    pNewFocusWin = InputDevData.pFgThreadInfo->pFocusWin;

    if (pOldFocusWin != NULL)
        WND_SendNotifyMessage(pOldFocusWin, WM_KILLFOCUS, 
        (WPARAM)(pNewFocusWin ? WOT_GetHandle((PWSOBJ)pNewFocusWin) : 0), (LPARAM)0);

    if (InputDevData.pFgThreadInfo->pFocusWin != NULL)
        WND_SendNotifyMessage(InputDevData.pFgThreadInfo->pFocusWin, WM_SETFOCUS, 
        (WPARAM)(pOldFocusWin ? WOT_GetHandle((PWSOBJ)pOldFocusWin) : 0), (LPARAM)0);

    return;
}

/* �ܳ��ڲ����� */
PWINOBJ WND_FromPointEx(int x, int y)
{
    LONG nHitTest;
    PWINOBJ pWin, pTempWin;

    if (x < 0 || y < 0)
    {

        x = x;
    }
    pWin = WND_FromPoint(x, y);
    
    ASSERT(pWin != NULL);

    return pWin;
}

/* �ܳ��ڲ����� */
BOOL INPUTMGR_SetFgThread(DWORD dwThreadId)
{
    if (dwThreadId != InputDevData.pFgThreadInfo->dwThreadId)
        ChangeFgThread(dwThreadId, 0);
    
    return TRUE;
}

PTHREADINFO INPUTMGR_GetFgThread(void)
{
    return InputDevData.pFgThreadInfo;
}

BOOL INPUTMGR_SetFgWindow(PWINOBJ pWin)
{
    ASSERT(ISROOTWIN(pWin->pParent));
    InputDevData.pFgWin = pWin;

    return TRUE;
}

PWINOBJ INPUTMGR_GetFgWindow(void)
{
    return InputDevData.pFgWin;
}
