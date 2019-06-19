/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements input device manage functions.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "devdisp.h"
#include "wsobj.h"
#include "wscursor.h"
#include "wssysmsg.h"

#include "wsthread.h"
#include "msgqueue.h"
#include "inputmgr.h"
#include "devinput.h"
#include "devqueue.h"
#include "wshook.h"
#include "string.h"

#include "stdio.h"

#ifndef _DDI_VER       // 没有定义_DDI_VER使用旧的设备驱动程序接口

#define GUI_CALL

#include "pointdev.h"

static POINTDRV         PointDrv;       // Point driver interface
static PDEV             pPointDev;      // Point device pointer

static int PenCallbackFunc(int32 para); // Pen dev callback function

#if (!NOKEYBOARDDEV)

#include "keyboard.h"

static KEYBOARDDRV      KbdDrv;         // Point driver interface
static PDEV             pKbdDev;        // Point device pointer

static int KbdCallbackFunc(int32 para); // Keyboard dev callback function

#endif // NOKEYBOARDDEV

#endif // _DDI_VER

// Define control key mask

#define CKS_SHIFT           (1 << 0)
#define CKS_CONTROL         (1 << 1)
#define CKS_ALT             (1 << 2)
#define CKS_CAPS            (1 << 3)

#define CKS_MASK            (CKS_SHIFT | CKS_CONTROL | CKS_ALT | CKS_CAPS)

#ifdef KEYBOARD_GERMANY
#define CKS_ALTGR           (1 << 4)
#endif

// Define mouse capture mode
#define CAPTURE_NULL        0   // Mouse isn't captured
#define CAPTURE_CLIENT      1   // Mouse is client capture
#define CAPTURE_NCLIENT     2   // Mouse is non client capture

// Define struct for input device management
/*
typedef struct
{
    PWINOBJ pActiveWin;         // Current active window

    PWINOBJ pFocusWin;          // The window which has keyboard focus
    PWINOBJ pKeyGrabWin;        // The window which grab keyboard input

    PWINOBJ pCaptureWin;        // The window which capture the mouse input
    int     nCaptureMode;       // Mouse capture mode

    WORD    wKeyState;          // Special key state
    WORD    wBtnState;          // Mouse button state 

    WORD    wKeyAfterSpecKey;   // the pressed key after control key
    WORD    wLastChar;          // last char code

    PWINOBJ pPenCalibrationWin; // The window used to calibrate the pen
} INPUTDEVDATA;

// Varibel used to store current input device state
static INPUTDEVDATA InputDevData;
*/

//static PTHREADINFO pOldThreadInfo = NULL;
static PWINOBJ g_pPenCalibrationWin = NULL; // The window used to calibrate the pen

#ifdef KEYBOARD_GERMANY

#define DOUBLEDOT_A     VK_QUOTA
#define DOUBLEDOT_O     VK_SEMICOLON
#define DOUBLEDOT_U     VK_LBRACKET

#define ISALPHA(wVirtKey) \
    ((wVirtKey >= VK_A && wVirtKey <= VK_Z) || wVirtKey == DOUBLEDOT_A || \
    wVirtKey == DOUBLEDOT_O || wVirtKey == DOUBLEDOT_U)

#define ISEXTALPHA(wVirtKey) (wVirtKey == DOUBLEDOT_A || \
    wVirtKey == DOUBLEDOT_O || wVirtKey == DOUBLEDOT_U)

typedef struct
{
    WORD wVirtKey;          // virtual key
    WORD wCharCode;         // char code when shift key isn't pressed
    WORD wShiftCharCode;    // char code when shift key is pressed
    WORD wAltgrCharCode;    // char code when altgr key is pressed
} KEYCODE;

typedef struct
{
    WORD wCharCode;         // virtual key
    WORD w136CharCode;      // 136 = '^' 
    WORD w145CharCode;      // 145 = '‘'
    WORD w146CharCode;      // 146 = '’'
} EXTKEYCODE;

static const KEYCODE KeyCodeTable[] =
{
//   wVirtKey   wCharCode  shift      Altgr   

    {VK_SPACE,  VK_SPACE,  VK_SPACE,  0      },
    {VK_RETURN, VK_RETURN, VK_RETURN, 0      }, 
    {VK_TAB,    VK_TAB,    VK_TAB,    VK_TAB },
    {VK_BACK,   VK_BACK,   VK_BACK,   0      }, 

    {VK_F13,    '<',       '>',       '|'    }, 

    {'1',       '1',       '!',       0      }, 
    {'2',       '2',       '"',       178    }, 
    {'3',       '3',       167,       179    }, 
    {'4',       '4',       '$',       0      }, 
    {'5',       '5',       '%',       0      }, 
    {'6',       '6',       '&',       0      }, 
    {'7',       '7',       '/',       '{'    }, 
    {'8',       '8',       '(',       '['    }, 
    {'9',       '9',       ')',       ']'    }, 
    {'0',       '0',       '=',       '}'    }, 
    {VK_MINUS,  223,       '?',       '\\'   },    // 'β'
    {VK_EQUAL,  146,       145,       0      },    // '’'

    {'A',       'a',       'A',       0      }, 
    {'B',       'b',       'B',       0      }, 
    {'C',       'c',       'C',       0      }, 
    {'D',       'd',       'D',       0      }, 
    {'E',       'e',       'E',       0      }, 
    {'F',       'f',       'F',       0      }, 
    {'G',       'g',       'G',       0      }, 
    {'H',       'h',       'H',       0      }, 
    {'I',       'i',       'I',       0      }, 
    {'J',       'j',       'J',       0      }, 
    {'K',       'k',       'K',       0      }, 
    {'L',       'l',       'L',       0      }, 
    {'M',       'm',       'M',       181    }, 
    {'N',       'n',       'N',       0      }, 
    {'O',       'o',       'O',       0      }, 
    {'P',       'p',       'P',       0      }, 
    {'Q',       'q',       'Q',       '@'    }, 
    {'R',       'r',       'R',       0      }, 
    {'S',       's',       'S',       0      }, 
    {'T',       't',       'T',       0      }, 
    {'U',       'u',       'U',       0      }, 
    {'V',       'v',       'V',       0      }, 
    {'W',       'w',       'W',       0      }, 
    {'X',       'x',       'X',       0      }, 
    {'Y',       'y',       'Y',       0      }, 
    {'Z',       'z',       'Z',       0      }, 

    {VK_LBRACKET,   252,   220,       0      },    // '..U'
    {VK_SEMICOLON,  246,   214,       0      },    // '..O'
    {VK_QUOTA,      228,   196,       0      },    // '..A'
    {VK_RBRACKET,   '+',   '*',       '~'    }, 
    {VK_SLASH,      '#',   '\'',      0      }, 
    {VK_COMMA,      ',',   ';',       0      }, 
    {VK_PERIOD,     '.',   ':',       0      }, 
    {VK_QUESTION,   '-',   '_',       0      }, 
    {VK_WAVE,       136,   176,       0      }     // '^'
};

static const EXTKEYCODE ExtKeyCodeTable[] =
{
//   code   '^'     '‘'     '’'
    {' ',   136,    145,    146}, 
    {'A',   194,    192,    193}, 
    {'E',   202,    200,    201}, 
    {'I',   206,    204,    205}, 
    {'O',   212,    210,    211}, 
    {'U',   219,    217,    218}, 
    {'Y',   'Y',    'Y',    221},
    {'a',   226,    224,    225}, 
    {'e',   234,    232,    233}, 
    {'i',   238,    236,    237}, 
    {'o',   244,    242,    243}, 
    {'u',   251,    249,    250}, 
    {'y',   'y',    'y',    253}
};

#else

#define ISALPHA(wVirtKey) (wVirtKey >= VK_A && wVirtKey <= VK_Z)

typedef struct
{
    WORD    wVirtKey;
    WORD    wCharCode;
    WORD    wShiftCharCode;
} KEYCODE;

static const KEYCODE KeyCodeTable[] =
{
    {VK_SPACE,      VK_SPACE,   VK_SPACE    },
    {VK_RETURN,     VK_RETURN,  VK_RETURN   }, 
    {VK_TAB,        VK_TAB,     VK_TAB      },
    {VK_BACK,       VK_BACK,    VK_BACK     }, 
    {VK_1,          '1',        '!'         }, 
    {VK_2,          '2',        '@'         }, 
    {VK_3,          '3',        '#'         }, 
    {VK_4,          '4',        '$'         }, 
    {VK_5,          '5',        '%'         }, 
    {VK_6,          '6',        '^'         }, 
    {VK_7,          '7',        '&'         }, 
    {VK_8,          '8',        '*'         },
    {VK_9,          '9',        '('         }, 
    {VK_0,          '0',        ')'         }, 
    {VK_WAVE,       '`',        '~'         },
    {VK_MINUS,      '-',        '_'         },
    {VK_EQUAL,      '=',        '+'         },
    {VK_LBRACKET,   '[',        '{'         },
    {VK_RBRACKET,   ']',        '}'         },
    {VK_SEMICOLON,  ';',        ':'         },
    {VK_QUOTA,      '\'',       '"'         },
    {VK_COMMA,      ',',        '<'         },        
    {VK_PERIOD,     '.',        '>'         },
    {VK_QUESTION,   '/',        '?'         },
    {VK_SLASH,      '\\',       '|'         },
    {VK_DIVIDE,     '/',        '/'         },
    {VK_MULTIPLY,   '*',        '*'         },
    {VK_SUBTRACT,   '-',        '-'         },
    {VK_ADD,        '+',        '+'         },
    {VK_DECIMAL,    '.',        '.'         }
};

#endif

typedef struct tagCONTROLKEYCODE
{
    WORD    wVirtKey;       // Virtual key code
    WORD    wCharCode;      // WM_CHAR code when control is pressed
} CONTROLKEYCODE;

static const CONTROLKEYCODE ControlKeyCodeTable[] = 
{
    {VK_RETURN,     10  }, 
    {VK_LBRACKET,   27  }, 
    {VK_SLASH,      28  }, 
    {VK_RBRACKET,   29  }, 
    {VK_BACK,       127 }
};

// Define mouse event id
#define ME_MOUSEMOVE        0
#define ME_LBUTTONDOWN      1
#define ME_LBUTTONUP        2
#define ME_LBUTTONDBLCLK    3
#define ME_RBUTTONDOWN      4
#define ME_RBUTTONUP        5
#define ME_RBUTTONDBLCLK    6
#define ME_MBUTTONDOWN      7
#define ME_MBUTTONUP        8
#define ME_MBUTTONDBLCLK    9
#define ME_WHEEL            10

// Define mouse message and non client mouse message base value
#define MOUSE_MESSAGE_BASE      WM_MOUSEMOVE
#define NCMOUSE_MESSAGE_BASE    WM_NCMOUSEMOVE

// Internal function prototypes
static BOOL ProcessMouseEvent(PMSG pMsg, PMSEV pMouseEvent, int nEvent);
static BOOL ProcessKeyboardInput(PMSG pMsg, PKBDEV pKbdEvent);
static BOOL IsSysModalWindowDescendant(PWINOBJ pWin);

/*
**  Function : INPUTDEV_Init
**  Purpose  :
**      Initializes input device management data struct. This function 
**      must be called when window system being initialized.
**  Params   :
**      None.
**  Return   :
**      If the function succeeds, return TRUE; If the function fails, 
**      return FALSE.
*/
BOOL INPUTDEV_Init(void)
{
#ifndef _DDI_VER       // 没有定义_DDI_VER使用旧的设备驱动程序接口

    int nRet;    
    SCALEDATA ScaleData;

    /*********************************************************************/
    /*  Get point driver interface and initialize Point device           */
    /*********************************************************************/

    // Get Point driver interface
    EnablePointDev(&PointDrv);

    // Get size of Point device struct
    nRet = PointDrv.OpenDevice(NULL, 1);

    // Ensure the return value is valid
    ASSERT(nRet > 0);
    if (nRet <= 0)
    {
        SetLastError(-nRet);
        return FALSE;
    }

    // Allocate memory for Point device struct pointer
    pPointDev = (PDEV)MemAlloc(nRet);

    // Ensure memory is allocated successfully
    ASSERT(pPointDev != NULL);
    if (!pPointDev)
        return FALSE;

    // Open the point device
    PointDrv.OpenDevice(pPointDev, 1);

    // Set callback function for point device
    PointDrv.SetCallbackFunc(pPointDev, PenCallbackFunc, 0);

    // Set original scale : 1 : 1
    ScaleData.log_x1 = 0;       /* X1 的逻辑坐标 */
    ScaleData.log_y1 = 0;       /* Y1 的逻辑坐标 */
    ScaleData.phy_x1 = 0;       /* X1 的测量数值 */
    ScaleData.phy_y1 = 0;       /* Y1 的测量数值 */
    ScaleData.log_x2 = 1;       /* X2 的逻辑坐标 */
    ScaleData.log_y2 = 1;       /* Y2 的逻辑坐标 */
    ScaleData.phy_x2 = 1;       /* X2 的测量数值 */
    ScaleData.phy_y2 = 1;       /* Y2 的测量数值 */

    PointDrv.SetScale(pPointDev, &ScaleData);

#if (!NOKEYBOARDDEV)

    /*********************************************************************/
    /*  Get keyboard driver interface and initialize keyboard device     */
    /*********************************************************************/

    // Get keyboard driver interface
    EnableKeyboardDev(&KbdDrv);

    // Get size of keyboard device struct
    nRet = KbdDrv.OpenDevice(NULL, 1);

    // Ensure the return value is valid
    ASSERT(nRet > 0);
    if (nRet <= 0)
    {
        SetLastError(-nRet);
        return FALSE;
    }

    // Allocate memory for Kbd device struct KbdDev
    pKbdDev = (PDEV)MemAlloc(nRet);

    // Ensure memory is allocated successfully
    if (!pKbdDev)
        return FALSE;

    // Open the Kbd device
    KbdDrv.OpenDevice(pKbdDev, 1);

    // Set callback function for Kbd device
    KbdDrv.SetCallbackFunc(pKbdDev, KbdCallbackFunc, 0);

#endif  //  NOKEYBOARDDEV

#endif  // _DDI_VER

    // Initializes the management state for input device
/*
    InputDevData.pActiveWin = NULL;

    InputDevData.pFocusWin = NULL;
    InputDevData.pKeyGrabWin = NULL;

    InputDevData.pCaptureWin = NULL;
    InputDevData.nCaptureMode = CAPTURE_NULL;

    InputDevData.wKeyState = 0;
    InputDevData.wBtnState = 0;
    InputDevData.wKeyAfterSpecKey = 0;

    InputDevData.pPenCalibrationWin = NULL;
*/
    return TRUE;
}

void INPUTDEV_Pause(void)
{
    return;
}

void INPUTDEV_Resume(void)
{
    INPUTMGR_GetFgThread()->wKeyState &= CKS_CAPS;
    INPUTMGR_GetFgThread()->wBtnState = 0;
    INPUTMGR_GetFgThread()->wKeyAfterSpecKey = 0;

    return;
}

/*
**  Function : INPUTDEV_GetInputMessage
**  Purpose  :
**      Gets a input message from input event queue
*/
BOOL INPUTDEV_GetInputMessage(PMSG pMsg)
{
    DEVEVENT DevEvent;
    BOOL bDone;
    int nMouseEvent;
    POINT ptMouse;
    PTHREADINFO pThreadInfo; 

    ASSERT(pMsg != NULL);

    pThreadInfo = WS_GetCurrentThreadInfo();

    // Gets current mouse position
    WS_GetCursorPos(&ptMouse);

    bDone = FALSE;
    while (!bDone)
    {
#if (__ST_PLX_GUI)
        WAITMSGQUEOBJ;
#endif 
        if (!DEVQ_GetInputEvent(pThreadInfo, &DevEvent, TRUE))
        {
#if (__ST_PLX_GUI)
            RELEASEMSGQUEOBJ;
#endif 
            return FALSE;
        }

#if (__ST_PLX_GUI)
        RELEASEMSGQUEOBJ;
#endif 

        switch (DevEvent.type)
        {
        case DE_MOUSE :

//            TRACE("Get mouse event: x= %d, y= %d\r\n", 
//                DevEvent.event.mouse.pt.x, DevEvent.event.mouse.pt.y);
            // Converts relative coordinates to absolute coordiantes
            // if necessary
            if (!(DevEvent.event.mouse.flags & MOUSEEVENTF_ABSOLUTE))
            {
                DevEvent.event.mouse.pt.x += ptMouse.x;
                DevEvent.event.mouse.pt.y += ptMouse.y;
            }

            // If pPenCalibrationWin isn't NULL, indicates that the window
            // system is doing the pen calibration, just send WM_LBUTTONDOWN 
            // message to pen calibration window.
            if (g_pPenCalibrationWin && 
                g_pPenCalibrationWin->dwThreadId == pThreadInfo->dwThreadId)
            {
                if (DevEvent.event.mouse.flags & MOUSEEVENTF_LEFTDOWN)
                {
                    pMsg->hwnd = (HWND)WOT_GetHandle((PWSOBJ)g_pPenCalibrationWin);

                    pMsg->message = WM_LBUTTONDOWN;
                    pMsg->wParam = MK_LBUTTON;
                    pMsg->lParam = MAKELPARAM(DevEvent.event.mouse.pt.x, 
                        DevEvent.event.mouse.pt.y);

                    bDone = TRUE;
                }

                break;
            }
            // Clips the mouse position by the cursor clip rectangle
            WS_ClipCursorPoint(&DevEvent.event.mouse.pt);

            // Now the mouse position is in the cursor clip rectangle

            nMouseEvent = ME_MOUSEMOVE;

            if (DevEvent.event.mouse.flags & MOUSEEVENTF_LEFTDOWN)
            {
                if (!(pThreadInfo->wBtnState & MK_LBUTTON) || 
                    (DevEvent.event.mouse.flags & MOUSEEVENTF_NOMERGEMOVE))
                {
                    pThreadInfo->wBtnState |= MK_LBUTTON;
                    nMouseEvent = ME_LBUTTONDOWN;
                }
                
                // If the mouse left button is already down, convert
                // the mouse button down event to mouse move event
            }
            else if (DevEvent.event.mouse.flags & MOUSEEVENTF_LEFTUP)
            {
                if ((pThreadInfo->wBtnState & MK_LBUTTON) || 
                    (DevEvent.event.mouse.flags & MOUSEEVENTF_NOMERGEMOVE))
                {
                    pThreadInfo->wBtnState &= (WORD)~MK_LBUTTON;
                    nMouseEvent = ME_LBUTTONUP;
                }
                
                // If the mouse left button isn't down, convert
                // the mouse up event to mouse move event
            }
            else if (DevEvent.event.mouse.flags & MOUSEEVENTF_RIGHTDOWN)
            {
                if (!(pThreadInfo->wBtnState & MK_RBUTTON) || 
                    (DevEvent.event.mouse.flags & MOUSEEVENTF_NOMERGEMOVE))
                {
                    pThreadInfo->wBtnState |= MK_RBUTTON;
                    nMouseEvent = ME_RBUTTONDOWN;
                }
                
                // If the mouse right button is already down, convert
                // the mouse down event to mouse move event
            }
            else if (DevEvent.event.mouse.flags & MOUSEEVENTF_RIGHTUP)
            {
                if ((pThreadInfo->wBtnState & MK_RBUTTON) || 
                    (DevEvent.event.mouse.flags & MOUSEEVENTF_NOMERGEMOVE))
                {
                    pThreadInfo->wBtnState &= (WORD)~MK_RBUTTON;
                    nMouseEvent = ME_RBUTTONUP;
                }
                
                // If the mouse right button isn't down, convert
                // the mouse up event to mouse move event
            }
            else if (DevEvent.event.mouse.flags & MOUSEEVENTF_MIDDLEDOWN)
            {
                if (!(pThreadInfo->wBtnState & MK_MBUTTON) || 
                    (DevEvent.event.mouse.flags & MOUSEEVENTF_NOMERGEMOVE))
                {
                    pThreadInfo->wBtnState |= MK_MBUTTON;
                    nMouseEvent = ME_MBUTTONDOWN;
                }
                
                // If the mouse middle button is already down, convert
                // the mouse down event to mouse move event
            }
            else if (DevEvent.event.mouse.flags & MOUSEEVENTF_MIDDLEUP)
            {
                if ((pThreadInfo->wBtnState & MK_MBUTTON) || 
                    (DevEvent.event.mouse.flags & MOUSEEVENTF_NOMERGEMOVE))
                {
                    pThreadInfo->wBtnState &= (WORD)~MK_MBUTTON;
                    nMouseEvent = ME_MBUTTONUP;
                }
                
                // If the mouse middle button isn't down, convert
                // the mouse up event to mouse move event
            }
            else if (DevEvent.event.mouse.flags & MOUSEEVENTF_WHEEL)
            {
                nMouseEvent = ME_WHEEL;
            }
            else if (!(DevEvent.event.mouse.flags & MOUSEEVENTF_MOVE))
                break;
            
            // 必须在调用WS_SetCursorPos函数前调用ProcessMouseEvent, 因为
            // ProcessMouseEvent函数中要调用WS_GetCursorPos获得原来的光标
            // 位置.
            bDone = ProcessMouseEvent(pMsg, &DevEvent.event.mouse, 
                nMouseEvent);

            // Sets cursor position if necessary
            if (ptMouse.x != DevEvent.event.mouse.pt.x ||
                ptMouse.y != DevEvent.event.mouse.pt.y)
            {
                ptMouse.x = DevEvent.event.mouse.pt.x;
                ptMouse.y = DevEvent.event.mouse.pt.y;

                // Move the cursor to new position
                WS_SetCursorPos(ptMouse.x, ptMouse.y);
            }

            break;

        case DE_KEYBOARD :

            bDone = ProcessKeyboardInput(pMsg, &DevEvent.event.kbd);
            break;

        default :

            break;
        }
    }

    return TRUE;
}

/*
**  Function : INPUTDEV_SetPointDevScale
*/
void INPUTDEV_SetPointDevScale(int nLogX1, int nLogY1, int nPhyX1, 
                               int nPhyY1, int nLogX2, int nLogY2, 
                               int nPhyX2, int nPhyY2)
{
#ifndef _DDI_VER       // 没有定义_DDI_VER使用旧的设备驱动程序接口

    SCALEDATA ScaleData;

    ScaleData.log_x1 = (int16)nLogX1;      /* X1 的逻辑坐标 */
    ScaleData.log_y1 = (int16)nLogY1;      /* Y1 的逻辑坐标 */
    ScaleData.phy_x1 = (int16)nPhyX1;      /* X1 的测量数值 */
    ScaleData.phy_y1 = (int16)nPhyY1;      /* Y1 的测量数值 */
    ScaleData.log_x2 = (int16)nLogX2;      /* X2 的逻辑坐标 */
    ScaleData.log_y2 = (int16)nLogY2;      /* Y2 的逻辑坐标 */
    ScaleData.phy_x2 = (int16)nPhyX2;      /* X2 的测量数值 */
    ScaleData.phy_y2 = (int16)nPhyY2;      /* Y2 的测量数值 */

    PointDrv.SetScale(pPointDev, &ScaleData);

#endif  // _DDI_VER
}

#ifndef _DDI_VER       // 没有定义_DDI_VER使用旧的设备驱动程序接口

static int PenCallbackFunc(int32 para)
{
    POINTDEVEVENT penEvent;
    DWORD flags;

    if (PointDrv.GetDevEvent(pPointDev, &penEvent))
    {
        if (penEvent.btn_stat & BTN_PENDOWN)
            flags = MOUSEEVENTF_LEFTDOWN;
        else if (penEvent.btn_stat & BTN_PENUP)
        {
            // 为了保证抬起消息与最后一个移动消息的坐标一样，这里强制
            // 发送一个与抬起时坐标一致的移动消息
            DEVQ_PutMouseEvent(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | 
                MOUSEEVENTF_FROMDEVICE, penEvent.x, penEvent.y, 0, 0);

            flags = MOUSEEVENTF_LEFTUP;
        }
        else
            flags = MOUSEEVENTF_MOVE;
        
        flags |=  MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_FROMDEVICE;
        
        DEVQ_PutMouseEvent(flags, penEvent.x, penEvent.y, 0, 0);
    }

    return 0;
}

#if (!NOKEYBOARDDEV)

static int KbdCallbackFunc(int32 para)
{
    KEYBOARDEVENT kbdEvent;

    if (KbdDrv.GetDevEvent(pKbdDev, &kbdEvent))
    {
        if (kbdEvent.shiftstate == KEYDOWN)
        {
            DEVQ_PutKeyboardEvent(kbdEvent.vkey, kbdEvent.scancode, 
                KEYEVENTF_FROMDEVICE, 0);
        }
        else
        {
            DEVQ_PutKeyboardEvent(kbdEvent.vkey, kbdEvent.scancode, 
                KEYEVENTF_KEYUP | KEYEVENTF_FROMDEVICE, 0);
        }
    }

    return 0;
}

#endif // NOKEYBOARDDEV

#endif // _DDI_VER

void INPUTDEV_BeginPenCalibrate(PWINOBJ pWin)
{
    ASSERT(pWin != NULL);

	g_pPenCalibrationWin = pWin;

    pWin = g_pRootWin->pChild;
    while (pWin)
    {
        WND_SendMessage(pWin, WM_BEGINPENCALIBRATE, 0, 0);
        pWin = pWin->pNext;
    }
}

void INPUTDEV_EndPenCalibrate(void)
{
    PWINOBJ pWin;

	g_pPenCalibrationWin = NULL;

    pWin = g_pRootWin->pChild;
    while (pWin)
    {
        WND_SendMessage(pWin, WM_ENDPENCALIBRATE, 0, 0);
        pWin = pWin->pNext;
    }
}

/*
**  Function : INPUTDEV_GetForegroundWindow
**  Purpose  :
**      
*/
PWINOBJ INPUTDEV_GetForegroundWindow(void)
{
    return INPUTMGR_GetFgThread()->pActiveWin;
}

/*
**  Function : INPUTDEV_SetForegroundWindow
**  Purpose  :
**      
*/
BOOL INPUTDEV_SetForegroundWindow(PWINOBJ pWin)
{
    if (pWin->pParent == g_pRootWin || ISROOTWIN(pWin))
    {
        INPUTDEV_SetActiveWindow(pWin, WA_ACTIVE, TRUE);
        return TRUE;
    }
    
    return FALSE;
}

/*
**  Function : INPUTDEV_GetActiveWindow
**  Purpose  :
**      
*/
PWINOBJ INPUTDEV_GetActiveWindow(void)
{
    return WS_GetCurrentThreadInfo()->pActiveWin;
}

/*
**  Function : INPUTDEV_SetActiveWindow
**  Purpose  :
**      Activates the specified window. 
**  Params   :
**      pWin : Identifies the top-level window to be activated. 
**  Return   :
**      If the function succeeds, return the window that was previously 
**      active. 
*/
PWINOBJ INPUTDEV_SetActiveWindow(PWINOBJ pWin, int nActive, BOOL bUpdate)
{
    BOOL bMinimized;
    PWINOBJ pOldActiveWin;
    PTHREADINFO pNewInfo;

    ASSERT(pWin && WND_IsWindow(pWin));

    ASSERT(nActive == WA_ACTIVE || nActive == WA_CLICKACTIVE);
    ASSERT(pWin->pParent == g_pRootWin || pWin == g_pRootWin);

    pOldActiveWin = INPUTMGR_GetFgWindow();
    pNewInfo = WS_GetThreadInfo(pWin->dwThreadId);

    if (pNewInfo->dwThreadId != WS_GetCurrentThreadId() && !bUpdate)
    {
        /* 线程间的活动窗口切换必须指定bUpdate */
        return NULL;
    }
    
    if (pNewInfo != INPUTMGR_GetFgThread())
    {
        if (!bUpdate)
        {
            /* 非前景线程的线程内的活动窗口切换，修改参数后直接返回 */
            pOldActiveWin = pNewInfo->pActiveWin;
            pNewInfo->pActiveWin = pWin;
            return pOldActiveWin;
        }

        INPUTMGR_SetFgThread(pNewInfo->dwThreadId);
    }

    /*  API SetActiveWindow will call this function
    ** with bUpdate=FALSE. Here the bUpdate may be FALSE or TRUE.
    */
/*
    if (!bUpdate)
    {
        ASSERT(0);
        return NULL;
    }

*/
    // 如果当前的active窗口是非法窗口（已经删除的窗口)，可能是由于活动窗口
    // 的转移与窗口删除的配合不当造成的，是异常情况，为保险起见，这里处理
    // 为将活动窗口设置为空，同时在DEBUG状态下使用ASSERT给出警告。因为这种
    // 情况是不应该出现的，以后只需使用ASSERT确保这种情况不能出现。
/*
    if (pOldInfo->pActiveWin && 
        !IsWindow(pOldInfo->pActiveWin->handle))
    {
        ASSERT(0);
        pOldInfo->pActiveWin = NULL;
    }
*/

    // 确保设置为活动状态的窗口不是将要删除的窗口
    ASSERT(!WND_IsDestroying(pWin));

    bMinimized = FALSE;
//    pOldActiveWin = pOldInfo->pActiveWin;

    // New active window must not be a child window
    if (pWin != pOldActiveWin && (!pWin || ISPOPUP(pWin)))
    {
        // Cancel the pen grab if necessary
		if (pNewInfo->pCaptureWin && pNewInfo->pCaptureWin != pWin)
		{
            if (pOldActiveWin == pNewInfo->pCaptureWin)
            {
                if (!strcmp(pOldActiveWin->pWndClass->lpszClassName, "#MENU"))
                    pOldActiveWin = NULL;
            }
			// 给应用程序机会释放对鼠标的捕获
			WND_SendMessage(pNewInfo->pCaptureWin, WM_CANCELMODE, 0, 0);
			
            // 应用程序没有释放对鼠标的捕获, 强制释放
			if (pNewInfo->pCaptureWin)
			{
				pNewInfo->pCaptureWin = NULL;
                pNewInfo->nCaptureMode = CAPTURE_NULL;
			}
            
		}
        // First move the window to top
        if (pWin)
            WND_MoveToTop(pWin);

        // If the current active window is NULL, just send window active
        // message to the new active window.
        if (!pOldActiveWin)
        {
            if (pWin)
            {
                pNewInfo->pActiveWin = pWin;
                
                WND_SendNotifyMessage(pWin, WM_ACTIVATEAPP, (WPARAM)1, 0);
                WND_SendNotifyMessage(pWin, WM_NCACTIVATE, (WPARAM)1, 0);
                WND_SendNotifyMessage(pWin, WM_ACTIVATE, 
                    MAKEWPARAM(nActive, bMinimized), 0);
            }
            INPUTMGR_SetFgWindow(pWin);
        }
        else
        {
            // Sends WM_NCACTIVATE and WM_ACTIVE mesage to the current 
            // active window, indicates that the current active will be 
            // deactived.
            WND_SendNotifyMessage(pOldActiveWin, WM_NCACTIVATE, 
                (WPARAM)0, 0);
            WND_SendNotifyMessage(pOldActiveWin, WM_ACTIVATE, 
                MAKEWPARAM(WA_INACTIVE, bMinimized), 
                (LPARAM)(pWin ? WOT_GetHandle((PWSOBJ)pWin) : NULL));

            // Send WM_ACTIVATEAPP message if the new active window and the 
            // current window isn't belong to a same appliction
            if (pWin && pOldActiveWin->hInst != pWin->hInst)
            {
                // Send WM_ACTIVATEAPP message to current active window
                WND_SendNotifyMessage(pOldActiveWin, WM_ACTIVATEAPP, 
                    (WPARAM)0, (LPARAM)(pWin ? pWin->hInst : NULL));
            }

            // Sets the new window to be the active window
            pNewInfo->pActiveWin = pWin;
            /* 在给新的活动窗口发送ACTIVATE消息前应该
            ** 设置新的前景窗口
            */
            INPUTMGR_SetFgWindow(pWin);

            if (pWin)
            {
                // Send WM_ACTIVATEAPP message to the new active window if
                // the new active window and the old active window isn't 
                // belong to a same appliction
                if (pOldActiveWin->hInst != pWin->hInst)
                {
                    WND_SendNotifyMessage(pWin, WM_ACTIVATEAPP, (WPARAM)1, 
                        (LPARAM)pOldActiveWin->hInst);
                }
                
                // Sends WM_NCACTIVATE and WM_ACTIVE mesage to the new
                // active window, indicates that the new active will be 
                // activated.
                WND_SendNotifyMessage(pWin, WM_NCACTIVATE, (WPARAM)1, 0);
                WND_SendNotifyMessage(pWin, WM_ACTIVATE, MAKEWPARAM(nActive, 
                    bMinimized), (LPARAM)WOT_GetHandle((PWSOBJ)pOldActiveWin));
                {
                    POINT pt;
                    int nHitTest;
                    PWINOBJ pChild, pCurWin;
                    
                    WS_GetCursorPos(&pt);
                    
                    nHitTest = WND_SendMessage(pWin, WM_NCHITTEST, 0, 
                        MAKELPARAM(pt.x, pt.y));
                    
                    pCurWin = pWin;
                    pChild = pWin->pChild;
                    while (pChild)
                    {
                        if (PtInRect(&pWin->rcWindow, pt))
                        {
                            pCurWin = pChild;
                            pChild = pChild->pChild;
                        }
                        else
                        {
                            pChild = pChild->pNext;
                        }
                    }
                    
                    WND_SendMessage(pCurWin, WM_SETCURSOR, 
                        (WPARAM)WOT_GetHandle((PWSOBJ)pCurWin), 
                        MAKELPARAM(nHitTest, MOUSE_MESSAGE_BASE));
                }
            }
        }

//        INPUTMGR_SetFgWindow(pWin);
    }
    else if (pWin == g_pRootWin)
    {
//        ASSERT(pWin->pChild == NULL);
        /* when there is no top window, the activewin maybe the root 
        ** window, the active window of the thread should be NULL.
        ** It is occured in the root window thread.
        */
        pNewInfo->pActiveWin = NULL;
        INPUTMGR_SetFgWindow(pWin);
    }

    return pOldActiveWin;
}

/*
**  Function : INPUTDEV_GetFocus
**  Purpose  :
**      Retrieves the handle of the window that has the keyboard focus.
*/
PWINOBJ INPUTDEV_GetFocus(void)
{
    return WS_GetCurrentThreadInfo()->pFocusWin;
}

/*
**  Function : INPUTDEV_SetFocus
**  Purpose  :
**      Sets the keyboard focus to the specified window. All 
**      subsequent keyboard input is directed to this window. 
**      The window, if any, that previously had the keyboard 
**      focus loses it.
*/
PWINOBJ INPUTDEV_SetFocus(PWINOBJ pWin)
{
    static BOOL bInKillFocus = FALSE;

    PWINOBJ pOldWin = NULL;
    PTHREADINFO pThreadInfo;

    ASSERT(!pWin || WND_IsWindow(pWin));

    pThreadInfo = WS_GetCurrentThreadInfo();

    // 如果当前的focus窗口是非法窗口（已经删除的窗口)，可能是由于焦点窗口
    // 的转移与窗口删除的配合不当造成的，是异常情况，为保险起见，这里处理
    // 为将活动窗口设置为空，同时在DEBUG状态下使用ASSERT给出警告。因为这种
    // 情况是不应该出现的，以后只需使用ASSERT确保这种情况不能出现。
    if (pThreadInfo->pFocusWin && !WND_IsWindow(pThreadInfo->pFocusWin))
    {
        ASSERT(0);
        pThreadInfo->pFocusWin = NULL;
    }

    // 如果要设置的窗口是disabled窗口或是正在删除的窗口，返回NULL表示设置
    // 失败，焦点窗口保持不变。可以设置隐藏窗口为焦点窗口，因此这里不判断
    // 窗口是否处于显示状态
    if (pWin && (!WND_IsEnabled(pWin) || WND_IsDestroying(pWin)))
        return NULL;

    if (pWin == pThreadInfo->pFocusWin)
        return pWin;

    // Set the new window to be the focus window before send WM_KILLFOCUS
    // message to the old focus window
    pOldWin = pThreadInfo->pFocusWin;
    pThreadInfo->pFocusWin = pWin;

    if (bInKillFocus)
        return pOldWin;
    
    // Send WM_KILLFOCUS message to the old focus window.
    if (pOldWin)
    {
        bInKillFocus = TRUE;
        WND_SendNotifyMessage(pOldWin, WM_KILLFOCUS, 
            (WPARAM)(pWin ? WOT_GetHandle((PWSOBJ)pWin) : 0), (LPARAM)0);
//        SendMessage(pOldWin->handle, WM_KILLFOCUS, 
//            (WPARAM)(pWin ? pWin->handle : 0), (LPARAM)0);
        bInKillFocus = FALSE;
    }

    // Send WM_SETFOCUS message
    if (pThreadInfo->pFocusWin)
    {
        WND_SendNotifyMessage(pThreadInfo->pFocusWin, WM_SETFOCUS, 
           (WPARAM)(pOldWin ? WOT_GetHandle((PWSOBJ)pOldWin) : 0), (LPARAM)0);
//        SendMessage(pThreadInfo->pFocusWin->handle, WM_SETFOCUS, 
//           (WPARAM)(pOldWin ? pOldWin->handle : 0), (LPARAM)0);
    }

    return pOldWin;
}

/*
**  Function : INPUTDEV_GetCapture
**  Purpose  :
**      
*/
PWINOBJ INPUTDEV_GetCapture(void)
{
    return WS_GetCurrentThreadInfo()->pCaptureWin;
}

/*
**  Function : INPUTDEV_SetCapture
**  Purpose  :
**      
*/
PWINOBJ INPUTDEV_SetCapture(PWINOBJ pWin, BOOL bClient)
{
    PWINOBJ pOldWin;
    PTHREADINFO pThreadInfo;

    ASSERT(pWin != NULL);

    pThreadInfo = WS_GetThreadInfo(pWin->dwThreadId);
    pOldWin = pThreadInfo->pCaptureWin;

    if (WND_IsDestroying(pWin))
    {
        pThreadInfo->pCaptureWin = NULL;
        pThreadInfo->nCaptureMode = CAPTURE_NULL;
    }
    else
    {
        pThreadInfo->pCaptureWin = pWin;
        pThreadInfo->nCaptureMode = bClient ? CAPTURE_CLIENT : CAPTURE_NCLIENT;
    }

    return pOldWin;
}

/*
**  Function : INPUTDEV_ReleaseCapture
**  Purpose  :
**
*/
BOOL INPUTDEV_ReleaseCapture(void)
{
    PTHREADINFO pThreadInfo;

    pThreadInfo = WS_GetCurrentThreadInfo();

    if (!pThreadInfo->pCaptureWin)
        return FALSE;

    pThreadInfo->pCaptureWin = NULL;
    pThreadInfo->nCaptureMode = CAPTURE_NULL;

    return TRUE;
}

/*
**  Function : INPUTDEV_GrabKeyboard
**  Purpose  :
*/
PWINOBJ INPUTDEV_GrabKeyboard(PWINOBJ pWin)
{
    PWINOBJ pOldWin;
    PTHREADINFO pThreadInfo;

    ASSERT(pWin != NULL);

    pThreadInfo = WS_GetCurrentThreadInfo();
    pOldWin = pThreadInfo->pKeyGrabWin;

    if (WND_IsDestroying(pWin))
        pThreadInfo->pKeyGrabWin = NULL;
    else
        pThreadInfo->pKeyGrabWin = pWin;

    return pOldWin;
}

/*
**  Function : INPUTDEV_UngrabKeyboard
*/
BOOL INPUTDEV_UngrabKeyboard(void)
{
    PTHREADINFO pThreadInfo;

    pThreadInfo = WS_GetCurrentThreadInfo();
    if (!pThreadInfo->pKeyGrabWin)
        return FALSE;

    pThreadInfo->pKeyGrabWin = NULL;

    return TRUE;
}

/*
**  Function : INPUTDEV_TranslateMessage
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

/* 管程外部函数 */
BOOL INPUTDEV_TranslateMessage(const MSG* pMsg)
{
    WORD wVirtKey;
    int nKeyState;
    WORD wCharCode = 0;
    int i;

    ASSERT(pMsg != NULL);
    ASSERT(pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN);

    wVirtKey = (WORD)pMsg->wParam;
    nKeyState = WS_GetCurrentThreadInfo()->wKeyState;

	// Do not send WM_CHAR message when ALT key is pressed
    if (nKeyState & CKS_ALT)
		return FALSE;

    // Sends special WM_CAHR message when CTRL key is pressed
    if (nKeyState & CKS_CONTROL)
    {
        if (wVirtKey >= VK_A && wVirtKey <= VK_Z)
            wCharCode = wVirtKey - 64;
        else
        {
            for (i = 0; i < sizeof(ControlKeyCodeTable) / 
                sizeof(CONTROLKEYCODE); i++)
            {
                if (ControlKeyCodeTable[i].wVirtKey == wVirtKey)
                {
                    wCharCode = ControlKeyCodeTable[i].wCharCode;
                    break;
                }
            }
            
            // 不是字母, 又不在ControlKeyCodeTable中, 不产生WM_CHAR消息
            if (i == sizeof(ControlKeyCodeTable) / sizeof(CONTROLKEYCODE))
                return FALSE;
        }

        PostMessage(pMsg->hwnd, WM_CHAR, (WPARAM)wCharCode, nKeyState);

        return TRUE;
    }

    if (pMsg->message == WM_SYSKEYDOWN)
    {
        // Translates system virtual-key messages to WM_SYSCHAR
        // ...

        return TRUE;
    }

    // Translate WM_KEYDOWN message to WM_CHAR message

    wCharCode = 0;

#ifdef KEYBOARD_GERMANY

    // 首先处理ALTGR键按下的情况
    if (nKeyState & CKS_ALTGR)
    {
        for (i = 0; i < sizeof(KeyCodeTable) / sizeof(KEYCODE); i++)
        {
            if (KeyCodeTable[i].wVirtKey == wVirtKey)
            {
                wCharCode = KeyCodeTable[i].wAltgrCharCode;
                break;
            }
        }

        if (wCharCode == 0)
            return FALSE;

        PostMessage(pMsg->hwnd, WM_CHAR, (WPARAM)wCharCode, nKeyState);
        return TRUE;
    }

#endif // KEYBOARD_GERMANY

    if (ISALPHA(wVirtKey))
    {
#ifdef KEYBOARD_GERMANY
        // 单独处理德文键盘的三个特殊字母..A, ..O, ..U
        if (ISEXTALPHA(wVirtKey))
        {
            for (i = 0; i < sizeof(KeyCodeTable) / sizeof(KEYCODE); i++)
            {
                if (KeyCodeTable[i].wVirtKey == wVirtKey)
                {
                    wVirtKey = KeyCodeTable[i].wShiftCharCode;
                    break;
                }
            }
        }
#endif // KEYBOARD_GERMANY

        if (((nKeyState & CKS_SHIFT) && 
            !(nKeyState & CKS_CAPS)) ||
            (!(nKeyState & CKS_SHIFT) && 
            (nKeyState & CKS_CAPS)))
            wCharCode = wVirtKey;
        else
            wCharCode = wVirtKey + 32;
    }
    else if (wVirtKey >= VK_NUMPAD0 && wVirtKey <= VK_NUMPAD9)
    {
       wCharCode = wVirtKey - 0x30;
    }
    else
    {
        for (i = 0; i < sizeof(KeyCodeTable) / sizeof(KEYCODE); i++)
        {
            if (KeyCodeTable[i].wVirtKey == wVirtKey)
            {
                if (nKeyState & CKS_SHIFT)
                    wCharCode = KeyCodeTable[i].wShiftCharCode;
                else
                    wCharCode = KeyCodeTable[i].wCharCode;
                
                break;
            }
        }
    }
    
    if (wCharCode == 0)
        return FALSE;

    PostMessage(pMsg->hwnd, WM_CHAR, (WPARAM)wCharCode, nKeyState);
    return TRUE;
}

/*
**  Function : INPUTDEV_SendChar
**  Purpose  :
**      Sends WM_CHAR message to the specified window or the focus window.
*/
/* 管程内部函数 */
BOOL INPUTDEV_SendChar(PWINOBJ pWin, WORD wCharCode)
{
    PTHREADINFO pThreadInfo;

    pThreadInfo = WS_GetCurrentThreadInfo();

    if (!pWin)
    {
        pWin = pThreadInfo->pFocusWin;
        
        if (pWin == pThreadInfo->pKeyGrabWin)
            return FALSE;
    }

    if (pWin)
        MSQ_PostMessage(pWin, WM_CHAR, (WPARAM)wCharCode, (LPARAM)0);

    return TRUE;
}

/*
**  Function : INPUTDEV_SendKeyDown
**  Purpose  :
**      Sends WM_KEYDOWN message to the specified window or the 
**      focus window.
*/
/*  管程内部函数 */
BOOL INPUTDEV_SendKeyDown(PWINOBJ pWin, WORD wVirtKey, WORD wKeyState)
{
    PTHREADINFO pThreadInfo;

    pThreadInfo = WS_GetCurrentThreadInfo();

    if (!pWin)
    {
        pWin = pThreadInfo->pFocusWin;
        
        if (pWin == pThreadInfo->pKeyGrabWin)
            return FALSE;
    }

    if (pWin)
    {
        MSQ_PostMessage(pWin, WM_KEYDOWN, (WPARAM)wVirtKey, 
            (LPARAM)wKeyState);
    }

    return TRUE;
}

/*
**  Function : INPUTDEV_SendKeyUp
**  Purpose  :
**      Sends WM_KEYUP message to the specified window or the 
**      focus window.
*/
/* 管程内部函数 */
BOOL INPUTDEV_SendKeyUp(PWINOBJ pWin, WORD wVirtKey, WORD wKeyState)
{
    PTHREADINFO pThreadInfo;

    pThreadInfo = WS_GetCurrentThreadInfo();
    
    if (!pWin)
    {
        pWin = pThreadInfo->pFocusWin;
        
        if (pWin == pThreadInfo->pKeyGrabWin)
            return FALSE;
    }

    if (pWin)
    {
        MSQ_PostMessage(pWin, WM_KEYUP, (WPARAM)wVirtKey, 
            (LPARAM)wKeyState);
    }

    return TRUE;
}

/*
**  Function : INPUTDEV_ResendMouseMessage
**  Purpose  :
**      Resends the specified mouse message
*/
/* 管程内部函数 */
BOOL INPUTDEV_ResendMouseMessage(PWINOBJ pWin, UINT wMsgCmd, WPARAM wParam,
                                 LPARAM lParam)
{
    int x, y;
    DWORD dwFlags;
    DWORD dwExtraInfo;
    PTHREADINFO pThreadInfo;

    pThreadInfo = WS_GetThreadInfo(pWin->dwThreadId);

    dwFlags = MOUSEEVENTF_NOMERGEMOVE;

    // 取1用以区别重发的鼠标事件和正常的鼠标事件，正常鼠标事件取0，PDA产品
    // 实现全屏手写功能需要
    dwExtraInfo = 1;

    x = (int)(short)LOWORD(lParam);
    y = (int)(short)HIWORD(lParam);

    if (wMsgCmd >= MOUSE_MESSAGE_BASE)
        WND_ClientToScreen(pWin, &x, &y);

    switch (wMsgCmd)
    {
    case WM_MOUSEMOVE :
    case WM_NCMOUSEMOVE :

        dwFlags |= MOUSEEVENTF_MOVE;
        break;

    case WM_LBUTTONDOWN :
    case WM_NCLBUTTONDOWN :

        // 必须设置当前的按键状态为抬起状态，否则重发的鼠标按键点下事件会
        // 被当作鼠标移动事件处理；另外输入队列中遗留的鼠标按键抬起事件将
        // 会被当作鼠标移动事件处理
//        InputDevData.wBtnState &= ~MK_LBUTTON;
//        pThreadInfo->wBtnState &= ~MK_LBUTTON;

        // 重发鼠标按键按下消息
        dwFlags |= MOUSEEVENTF_LEFTDOWN;
        if (!DEVQ_PutMouseEvent(pThreadInfo, dwFlags | MOUSEEVENTF_ABSOLUTE, x, y, 0, 
            dwExtraInfo, (HWND)WOT_GetHandle((PWSOBJ)pWin)))
            return FALSE;

        // 补发鼠标按键抬起消息，否则后续的正常鼠标按键输入事件的处理会受到
        // 影响
        dwFlags = MOUSEEVENTF_LEFTUP;

        break; 

    case WM_LBUTTONUP :
    case WM_NCLBUTTONUP :
        
        dwFlags = MOUSEEVENTF_LEFTUP;
//        InputDevData.wBtnState |= MK_LBUTTON;
        pThreadInfo->wBtnState |= MK_LBUTTON;
        break; 
        
    case WM_RBUTTONDOWN :
    case WM_NCRBUTTONDOWN :
        
        // 必须设置当前的按键状态为抬起状态，否则重发的鼠标按键点下事件会
        // 被当作鼠标移动事件处理；另外输入队列中遗留的鼠标按键抬起事件将
        // 会被当作鼠标移动事件处理
//        InputDevData.wBtnState &= ~MK_RBUTTON;
//        pThreadInfo->wBtnState &= ~MK_RBUTTON;

        // 重发鼠标按键按下消息
        dwFlags |= MOUSEEVENTF_RIGHTDOWN;
        if (!DEVQ_PutMouseEvent(pThreadInfo, dwFlags | MOUSEEVENTF_ABSOLUTE, x, y, 0, 
            dwExtraInfo, (HWND)WOT_GetHandle((PWSOBJ)pWin)))
            return FALSE;

        // 补发鼠标按键抬起消息，否则后续的正常鼠标按键输入事件的处理会受到
        // 影响
        dwFlags = MOUSEEVENTF_RIGHTUP;

        break; 
        
    case WM_RBUTTONUP :
    case WM_NCRBUTTONUP :

        dwFlags = MOUSEEVENTF_RIGHTUP;
//        InputDevData.wBtnState |= MK_RBUTTON;
        pThreadInfo->wBtnState |= MK_RBUTTON;
        break; 
        
    case WM_MBUTTONDOWN :
    case WM_NCMBUTTONDOWN :
        
        // 必须设置当前的按键状态为抬起状态，否则重发的鼠标按键点下事件会
        // 被当作鼠标移动事件处理；另外输入队列中遗留的鼠标按键抬起事件将
        // 会被当作鼠标移动事件处理
//        InputDevData.wBtnState &= ~MK_MBUTTON;
//        pThreadInfo->wBtnState &= ~MK_MBUTTON;

        // 重发鼠标按键按下消息
        dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
        if (!DEVQ_PutMouseEvent(pThreadInfo, dwFlags | MOUSEEVENTF_ABSOLUTE, x, y, 0, 
            dwExtraInfo, (HWND)WOT_GetHandle((PWSOBJ)pWin)))
            return FALSE;

        // 补发鼠标按键抬起消息，否则后续的正常鼠标按键输入事件的处理会受到
        // 影响
        dwFlags = MOUSEEVENTF_MIDDLEUP;

        break; 
        
    case WM_MBUTTONUP :
    case WM_NCMBUTTONUP :
        
        dwFlags = MOUSEEVENTF_MIDDLEUP;
//        InputDevData.wBtnState |= MK_MBUTTON;
        pThreadInfo->wBtnState |= MK_MBUTTON;
        break; 
        
    default : 

        return FALSE;
    }

    return DEVQ_PutMouseEvent(pThreadInfo, 
        dwFlags | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_NOMERGEMOVE, x, y, 0, 
        dwExtraInfo, (HWND)WOT_GetHandle((PWSOBJ)pWin));
}

/*************************************************************************/
/*      Functions to support system modal dialog                         */
/*************************************************************************/

// Define macro and variable used to manage the modal window stack
#define MAX_MODAL_WIN   11
static PWINOBJ SysModalWinStack[MAX_MODAL_WIN];
static int nCurrentSysModalWin = 0;

// 注意：为编程方便，这里不使用0位置存储system modal win，0位置固定为NULL，
// 这样可以不考虑边界条件，因此，实际可以设置的最大数目为MAX_MODAL_WIN - 1

/*
**  Function : INPUTDEV_SetSysModalWindow
**  Purpose  :
**      Notifies that the specified window will be system modal state.
*/
/* 管程内部函数 */
PWINOBJ INPUTDEV_SetSysModalWindow(PWINOBJ pWin)
{
    PTHREADINFO pThreadInfo;

    ASSERT(pWin != NULL);       // Can't be NULL
    ASSERT(!ISROOTWIN(pWin));   // Can't be root window

    // Can't be child window
    if (ISCHILD(pWin))
        return NULL;

    if (nCurrentSysModalWin == MAX_MODAL_WIN - 1)
        return NULL;

    pThreadInfo = WS_GetCurrentThreadInfo();

    nCurrentSysModalWin++;
    SysModalWinStack[nCurrentSysModalWin] = pWin;

    // Cancel the pen grab if necessary
    if (pThreadInfo->pCaptureWin)
    {
        // 给应用程序机会释放对鼠标的捕获
        WND_SendNotifyMessage(pThreadInfo->pCaptureWin, WM_CANCELMODE, 0, 0);

        // 应用程序没有释放对鼠标的捕获, 强制释放
        if (pThreadInfo->pCaptureWin)
        {
            pThreadInfo->pCaptureWin = NULL;
            pThreadInfo->nCaptureMode = CAPTURE_NULL;
        }
    }

    return SysModalWinStack[nCurrentSysModalWin - 1];
}

/*
**  Function : INPUTDEV_GetSysModalWindow
**  Purpose  :
**      Retrieves the current system modal window.
*/
/* 管程内部函数 */
PWINOBJ INPUTDEV_GetSysModalWindow(void)
{
    return SysModalWinStack[nCurrentSysModalWin];
}

/*
**  Function : IsSysModalWindowDescendant
**  Purpose  :
**      Retrieves the specified window is the current system modal window.
*/
/* 管程内部函数 */
static BOOL IsSysModalWindowDescendant(PWINOBJ pWin)
{
    // If there is no system modal window, return TRUE
    if (SysModalWinStack[nCurrentSysModalWin] == NULL)
        return TRUE;

    return WND_IsDescendant(pWin, SysModalWinStack[nCurrentSysModalWin]);
}

/*************************************************************************/
/*      Function called by window destroy function                       */
/*************************************************************************/

/*
**  Function : INPUTDEV_HideWindowNotify
**  Purpose  :
**      Notifies the input device management module that a window is being
**      hided. This function must be called when a window is being hided.
*/
/* 管程内部函数 */
void INPUTDEV_HideWindowNotify(PWINOBJ pWin)
{
    PWINOBJ pNewWin;
    int i, j;
    PTHREADINFO pThreadInfo;

    ASSERT(pWin != NULL);
    ASSERT(!ISROOTWIN(pWin));

    pThreadInfo = WS_GetThreadInfo(pWin->dwThreadId);

    // 注意，指定的窗口将被隐藏，窗口的后代窗口也将被隐藏，如果InputDevData
    // 中的当前窗口是要隐藏窗口的后代窗窗口时需要处理。

    if (WND_IsDescendant(pThreadInfo->pKeyGrabWin, pWin))
        pThreadInfo->pKeyGrabWin = NULL;

    if (WND_IsDescendant(pThreadInfo->pCaptureWin, pWin))
    {
        pThreadInfo->pCaptureWin = NULL;
        pThreadInfo->nCaptureMode = CAPTURE_NULL;
    }

    // If the window to be deleted is in the current system modal window, 
    // end the system modal state
    if (ISPOPUP(pWin))
    {
        for (i = nCurrentSysModalWin; i > 0; i--)
        {
            if (SysModalWinStack[i] == pWin)
            {
                for (j = i; j < nCurrentSysModalWin; j++)
                    SysModalWinStack[j] = SysModalWinStack[j + 1];

                SysModalWinStack[nCurrentSysModalWin] = NULL;
                nCurrentSysModalWin--;
            }
        }
    }

    // The window to be destroyed is the activate window itself or owns the
    // active window, finds a new active window, 
    /* 前景窗口是不会成为pWin的后代的 */ 
    if (INPUTDEV_GetForegroundWindow() == pWin)
    {
        int nActivate;

        pNewWin = pWin->pParent->pChild;
        while (pNewWin != NULL)
        {
            if (pNewWin != pWin && ISMAPPED(pNewWin) && 
                WND_IsEnabled(pNewWin))
            {
                nActivate = WND_SendMessage(pNewWin, WM_MOUSEACTIVATE, 
                    0, 0);
                if (nActivate == MA_ACTIVATE || 
                    nActivate == MA_ACTIVATEANDEAT)
                break;
            }
            
            pNewWin = pNewWin->pNext;
        }

        if (pNewWin == NULL)
            INPUTDEV_SetForegroundWindow(g_pRootWin);
        else
            INPUTDEV_SetForegroundWindow(pNewWin);
    }
    else if (pThreadInfo->pActiveWin == pWin)
    {
        int nActivate;
        /* 活动窗口是不会成为pWin的后代的 */ 
        /* 线程内的活动窗口切换,搜索另一个顶级窗口 */
        pNewWin = g_pRootWin->pChild;
        while (pNewWin != NULL)
        {
            if (pNewWin != pWin && ISMAPPED(pNewWin) && 
                WND_IsEnabled(pNewWin) && 
                pNewWin->dwThreadId == pWin->dwThreadId)
            {
                nActivate = WND_SendMessage(pNewWin, WM_MOUSEACTIVATE, 
                    0, 0);
                if (nActivate == MA_ACTIVATE || 
                    nActivate == MA_ACTIVATEANDEAT)
                break;
            }
            
            pNewWin = pNewWin->pNext;
        }

        if (pNewWin == NULL)
            pThreadInfo->pActiveWin = NULL;
        else
            INPUTDEV_SetActiveWindow(pNewWin, WA_ACTIVE, FALSE);
    }

    // 当要隐藏的窗口是active窗口时，active窗口的转移会引起focus窗口的转移,
    // 所以对focus窗口的重新设置应该放到处理active窗口之后

    // 如果当前的focus窗口是要隐藏窗口的后代窗口或自身，设置focus窗口为要隐
    // 藏窗口的父窗口；设置以后，必须再次检查当前的focus窗口是不是要隐藏窗
    // 口的后代窗口或自身，因为调用INPUTDEV_SetFocus会发送WM_SETFOCUS和
    // WM_KILLFOCUS消息，应用程序响应这些消息时，可能会调用SetFocus函数重新
    // 设置焦点窗口，这里必须确保新的focus窗口不是将要被删除的窗口，否则会
    // 由于已经释放的窗口对象指针引起无法预料的错误
    if (WND_IsDescendant(pThreadInfo->pFocusWin, pWin))
    {
        PWINOBJ pUnusable;
        BOOL bFromChild = FALSE;
        pUnusable = pWin;
        pNewWin = pWin;
        while (pNewWin != NULL)
        {
            if (!bFromChild && pNewWin->pParent != NULL)
            {
                pNewWin = pNewWin->pParent->pChild;
                bFromChild = TRUE;
                continue;
            }

            if (pNewWin != pUnusable && ISMAPPED(pNewWin) && 
                WND_IsEnabled(pNewWin) && !WND_IsDestroying(pNewWin) && 
                pNewWin->dwThreadId == pUnusable->dwThreadId)
            {
                break; 
            }

            if (pNewWin->pNext != NULL)
            {
                pNewWin = pNewWin->pNext;
            }
            else if (pNewWin->pParent != NULL)
            {
                pNewWin = pNewWin->pParent;
                if (ISMAPPED(pNewWin) && WND_IsEnabled(pNewWin) && 
                    !WND_IsDestroying(pNewWin) && 
                    pNewWin->dwThreadId == pUnusable->dwThreadId)
                    break;

                pUnusable = pNewWin;
                bFromChild = FALSE;
            }
            else
            {
                pNewWin = NULL;
            }
        }

        if (pNewWin == NULL)
        {
            pThreadInfo->pFocusWin = NULL;
        }
        else
        {
            INPUTDEV_SetFocus(pNewWin);
        }

/*
        pNewWin = pWin->pParent;
        while (pNewWin->pParent != NULL)
        {
            if (ISMAPPED(pNewWin) && WND_IsEnabled(pNewWin) && 
                !WND_IsDestroying(pNewWin) && 
                pNewWin->dwThreadId == pWin->dwThreadId)
                break;
            
            pNewWin = pNewWin->pParent;
        }

        if (pNewWin->pParent == NULL)
        {
            pNewWin = NULL;
            pThreadInfo->pFocusWin = NULL;
        }
        else
        {
            INPUTDEV_SetFocus(pNewWin);
        }
*/

        ASSERT(!WND_IsDescendant(pThreadInfo->pFocusWin, pWin));
    }
}

/*
**  Function : INPUTDEV_DestroyWindowNotify
**  Purpose  :
**      Notifies the input device management module that a window
**      is being destroyed. This function must be called when a
**      window is being destroyed.
*/
/* 管程内部函数 */
void INPUTDEV_DestroyWindowNotify(PWINOBJ pWin)
{
    // 与窗口隐藏同样处理
    INPUTDEV_HideWindowNotify(pWin);
}

/*
**  Function : INPUTDEV_GetKeyboardState
**  Purpose  :
**      Copies the status of the 256 virtual keys to the specified buffer.
*/
/* 管程内部函数 */
void INPUTDEV_GetKeyboardState(PBYTE pKeyState)
{
    PTHREADINFO pThreadInfo;

    pThreadInfo = WS_GetCurrentThreadInfo();
    memset(pKeyState, 0, 256);

    if (pThreadInfo->wKeyState & CKS_SHIFT)
        pKeyState[VK_SHIFT] = 0x80;

    if (pThreadInfo->wKeyState & CKS_CONTROL)
        pKeyState[VK_CONTROL] = 0x80;

    if (pThreadInfo->wKeyState & CKS_ALT)
        pKeyState[VK_ALT] = 0x80;

    if (pThreadInfo->wKeyState & CKS_CAPS)
        pKeyState[VK_CAPITAL] = 0x01;
}

/*
**  Function : INPUTDEV_ToAscii
**  Purpose  :
**      Translates the specified virtual-key code and keyboard state to 
**      the corresponding Windows character or characters. The function 
**      translates the code using the input language and physical 
**      keyboard layout identified by the given keyboard layout handle. 
*/
/* 管程外部函数 */
int  INPUTDEV_ToAscii(UINT uVirtKey, UINT uScanCode, PBYTE pKeyState,
                      PWORD pChar, UINT uFlags)
{
    int i;
    WORD wCharCode = 0;

    // Control key is down
    if (pKeyState[VK_CONTROL] & 0x80)
    {
        if (uVirtKey >= VK_A && uVirtKey <= VK_Z)
            wCharCode = uVirtKey - 64;
        else
        {
            for (i = 0; i < sizeof(ControlKeyCodeTable) / 
                sizeof(CONTROLKEYCODE); i++)
            {
                if (ControlKeyCodeTable[i].wVirtKey == uVirtKey)
                {
                    wCharCode = ControlKeyCodeTable[i].wCharCode;
                    break;
                }
            }
            
            // 不是字母, 又不在ControlKeyCodeTable中, 不产生WM_CHAR消息
            if (i == sizeof(ControlKeyCodeTable) / sizeof(CONTROLKEYCODE))
                return 0;
        }
    }
    else
    {
        if (uVirtKey >= VK_A && uVirtKey <= VK_Z)
        {
            if (((pKeyState[VK_SHIFT] & 0x80) && 
                !(pKeyState[VK_CAPITAL] & 0x01)) ||
                (!(pKeyState[VK_SHIFT] & 0x80) && 
                (pKeyState[VK_CAPITAL] & 0x01)))
                wCharCode = uVirtKey;
            else
                wCharCode = uVirtKey + 32;
        }
        else
        {
            for (i = 0; i < sizeof(KeyCodeTable) / sizeof(KEYCODE); i++)
            {
                if (KeyCodeTable[i].wVirtKey == uVirtKey)
                {
                    if (pKeyState[VK_SHIFT] & 0x80)
                        wCharCode = KeyCodeTable[i].wShiftCharCode;
                    else
                        wCharCode = KeyCodeTable[i].wCharCode;
                    
                    break;
                }
            }
            
            // 不是字母, 又不在KeyCodeTable中, 不产生WM_CHAR消息
            if (i == sizeof(KeyCodeTable) / sizeof(KEYCODE))
                return 0;
        }
    }

    ASSERT(wCharCode != 0);

    *pChar = (*pChar & 0xFF00) | wCharCode;

    return 1;
}

// Internal functions

/*
**  Function : ProcessMouseEvent
**  Purpose  :
**      Processes mouse button down event which is sended from input device
**      drive program.
*/
/* 管程内部函数 */
static BOOL ProcessMouseEvent(PMSG pMsg, PMSEV pMouseEvent, int nEvent)
{
    PWINOBJ pWin, pTopLevelWin, pTempWin, pChild;
    int nHitTest;
    int nActivate;
    BOOL bClient;
    int x, y;
    MOUSEHOOKSTRUCT mhs;
    UINT message;
    PTHREADINFO pThreadInfo;
    BOOL bModuleWin = FALSE;

    x = pMouseEvent->pt.x;
    y = pMouseEvent->pt.y;

    pThreadInfo = WS_GetCurrentThreadInfo();
    // 获得鼠标事件的目标窗口, 如果有窗口捕获鼠标消息, 则该窗口为鼠标事件
    // 的目标窗口. 
    if (nEvent == ME_WHEEL)
    {
        pWin = pThreadInfo->pFocusWin;
        if (!pWin)
            return FALSE;
#ifndef NOWH
        
        // Calls mouse hook functions
        
        mhs.pt = pMouseEvent->pt;
        mhs.hwnd = (HWND)WOT_GetHandle((PWSOBJ)pWin);
        mhs.wHitTestCode = 0;
        mhs.dwExtraInfo = pMouseEvent->dwExtraInfo;
        
        if (WH_CallHookProc(WH_MOUSE, HC_ACTION, (WPARAM)(nEvent + 
            MOUSE_MESSAGE_BASE), (LPARAM)&mhs))
            return FALSE;
        
#endif  // NOWH
        
        pMsg->hwnd = (HWND)WOT_GetHandle((PWSOBJ)pWin);
        pMsg->message = nEvent + MOUSE_MESSAGE_BASE;
        pMsg->wParam = pThreadInfo->wBtnState;
        pMsg->lParam = MAKELPARAM(x, y);
        
        if (pThreadInfo->wKeyState & CKS_SHIFT)
            pMsg->wParam |= MK_SHIFT;
        
        if (pThreadInfo->wKeyState & CKS_CONTROL)
            pMsg->wParam |= MK_CONTROL;
        
        pMsg->wParam |= pMouseEvent->mouseData << 16;
        
        return TRUE;
    }

    if (pThreadInfo->pCaptureWin && WND_IsEnabled(pThreadInfo->pCaptureWin))
    {
        pWin = pThreadInfo->pCaptureWin;
    }
    else
    {
        // 没有窗口捕获鼠标消息, 则鼠标位置所在的上层窗口为目标窗口, 若该
        // 窗口被禁止, 则以它的父窗口为鼠标事件的目标窗口

        pWin = WND_FromPoint(x, y);
        while (pWin)
        {
            if (WND_IsEnabled(pWin))
                break;
            else if (pWin->byOwnWins != 0) //模式对话框可以接收激活消息
            {
                bModuleWin = TRUE;
                break;
            }
            else
                pWin = pWin->pParent;
        }
        
        ASSERT(pWin != NULL);
    }

    // Sends WM_NCHITTEST message to the window proc to get the hit test 
    // code
    nHitTest = WND_SendMessage(pWin, WM_NCHITTEST, 0, 
        MAKELPARAM(x, y));

    // If the WM_NCHITEST message return value is HTTRANSPARENT, indicates
    // that the current mouse window is transparent to mouse event, finds
    // new mouse window from its brother window or parent window 

    if (nHitTest == HTTRANSPARENT)
    {
        bModuleWin = FALSE;

        // First finds in the brother windows under the current mouse 
        // window
        pTempWin = pWin->pNext;
        while (pTempWin != NULL)
        {
            if (!WND_IsEnabled(pTempWin))
            {
                if (ISCHILD(pTempWin))
                    break;
                //POPUP窗口，且拥有模式对话框
                else if (pTempWin->byOwnWins != 0 && ISMAPPED(pTempWin) && 
                    PtInRectXY(&pTempWin->rcWindow, x, y))
                {
                    nHitTest = WND_SendMessage(pTempWin, WM_NCHITTEST, 0, 
                        MAKELPARAM(x, y));
                    if (nHitTest != HTTRANSPARENT)
                    {
                        bModuleWin = TRUE;
                        break;
                    }
                }
            }
            else if (ISMAPPED(pTempWin) && 
                PtInRectXY(&pTempWin->rcWindow, x, y))
            {
                nHitTest = WND_SendMessage(pTempWin, WM_NCHITTEST, 0, 
                    MAKELPARAM(x, y));
                if (nHitTest != HTTRANSPARENT)
                    break;
            }
            
            pTempWin = pTempWin->pNext;
        }
        
        // If still can't finds the mouse window, finds from its parent 
        // windows
        if (nHitTest == HTTRANSPARENT)
        {
            pTempWin = pWin->pParent;
            while (pTempWin != NULL)
            {
                if (ISMAPPED(pTempWin) && WND_IsEnabled(pWin))
                {
                    nHitTest = WND_SendMessage(pTempWin, WM_NCHITTEST, 0, 
                        MAKELPARAM(x, y));
                    if (nHitTest != HTTRANSPARENT)
                        break;
                }
                
                pTempWin = pTempWin->pParent;
            }
            pWin = pTempWin;
        }
        else if (!bModuleWin)
        {
            pWin = pTempWin;
            do 
            {
                if ((pChild = pWin->pChild) == NULL)
                    break;

                if (PtInRectXY(&pWin->rcClient, x, y))
                {
                    while (pChild)
                    {
                        // 该点在pChild窗口之内
                        if (ISMAPPED(pChild) && WND_IsEnabled(pChild) 
                            && PtInRectXY(&pChild->rcWindow, x, y))
                        {
                            nHitTest = WND_SendMessage(pChild, WM_NCHITTEST, 0, 
                                MAKELPARAM(x, y));
                            if (nHitTest != HTTRANSPARENT)
                            {
                                pWin = pChild;
                                break;
                            }
                        }
                    
                        pChild = pChild->pNext;
                    }
                }
            
            } while (pChild != NULL);
        }
        else //模式对话框
        {
            pWin = pTempWin;
            while (pWin->pPrev != NULL && pWin->byOwnWins != 0)
            {
                if (pWin->pPrev->pOwner != pWin)
                {
                    ASSERT(0);
                    break;
                }
                
                pWin = pWin->pPrev;
                bModuleWin = TRUE;
            }
        }

        ASSERT(pWin != NULL);
    }
    else 
    {
        if(bModuleWin)
        {
            while (pWin->pPrev != NULL && pWin->byOwnWins != 0)
            {
                if (pWin->pPrev->pOwner != pWin)
                {
                    ASSERT(0);
                    break;
                }
                
                pWin = pWin->pPrev;
                bModuleWin = TRUE;
            }
        }
    }

    if (pThreadInfo->dwThreadId != pWin->dwThreadId)
        return FALSE;

#ifndef NOWH

    // Calls mouse hook functions

    mhs.pt = pMouseEvent->pt;
    mhs.hwnd = (HWND)WOT_GetHandle((PWSOBJ)pWin);
    mhs.wHitTestCode = (UINT)nHitTest;
    mhs.dwExtraInfo = pMouseEvent->dwExtraInfo;

    if (WH_CallHookProc(WH_MOUSE, HC_ACTION, (WPARAM)(nEvent + 
        MOUSE_MESSAGE_BASE), (LPARAM)&mhs))
        return FALSE;

#endif  // NOWH

    // If no window capture the mouse, ...
    if (pThreadInfo->pCaptureWin && WND_IsEnabled(pThreadInfo->pCaptureWin))
    {
        bClient = (pThreadInfo->nCaptureMode == CAPTURE_CLIENT);

#if (!NOCURSOR)
        if (bClient)
        {
            WND_SendNotifyMessage(pWin, WM_SETCURSOR, (WPARAM)WOT_GetHandle((PWSOBJ)pWin), 
                MAKELPARAM(nHitTest, nEvent + MOUSE_MESSAGE_BASE));
        }
        else
        {
            WND_SendNotifyMessage(pWin, WM_SETCURSOR, (WPARAM)WOT_GetHandle((PWSOBJ)pWin), 
                MAKELPARAM(nHitTest, nEvent + NCMOUSE_MESSAGE_BASE));
        }
#endif // NOCURSOR
    }
    else
    {
        // If the window isn't descendant window of current system modal 
        // window, just return 
        if (!IsSysModalWindowDescendant(pWin))
        {
#if (!NOCURSOR)
            WND_SendNotifyMessage(pWin, WM_SETCURSOR, (WPARAM)WOT_GetHandle((PWSOBJ)pWin), 
                MAKELPARAM(HTERROR, nEvent + MOUSE_MESSAGE_BASE));
#endif // NOCURSOR

            return FALSE;
        }
        
        // bClient indicates the message is client message or nclient message
        bClient = (nHitTest == HTCLIENT);
        
        message = nEvent + 
            (bClient ? MOUSE_MESSAGE_BASE : NCMOUSE_MESSAGE_BASE);
        
#if (!NOCURSOR)
        // Sends WM_SETCURSOR message to allow window proc to change cursor
        WND_SendNotifyMessage(pWin, WM_SETCURSOR, (WPARAM)WOT_GetHandle((PWSOBJ)pWin), 
            MAKELPARAM(nHitTest, message));
#endif // NOCURSOR

        // 单独处理ME_LBUTTONDOWN和ME_RBUTTONDOWN, 完成活动窗口的切换
        if (nEvent == ME_LBUTTONDOWN || nEvent == ME_RBUTTONDOWN)
        {
            // If the window isn't the root window and the window's top
            // level parent window isn't the active window, send 
            // WM_MOUSEACTIVATE message to the window proc to determine
            // whether set the window's top level parent window to be 
            // the active window
            if (!ISROOTWIN(pWin))
            {
                // Finds the top level window of the window
                pTopLevelWin = pWin;
                while (!ISROOTWIN(pTopLevelWin->pParent))
                    pTopLevelWin = pTopLevelWin->pParent;

                if ((pTopLevelWin != pThreadInfo->pActiveWin && 
                    pTopLevelWin->dwThreadId == pWin->dwThreadId) || 
                    (pTopLevelWin != INPUTMGR_GetFgWindow()))
                {
                    nActivate = WND_SendMessage(pTopLevelWin, WM_MOUSEACTIVATE, 
                        (WPARAM)WOT_GetHandle((PWSOBJ)pTopLevelWin), MAKELPARAM(nHitTest, 
                        bClient ? (nEvent + MOUSE_MESSAGE_BASE) : 
                        (nEvent + NCMOUSE_MESSAGE_BASE)));
                    
                    if (nActivate == MA_ACTIVATE || 
                        nActivate == MA_ACTIVATEANDEAT)
                    {
                        INPUTDEV_SetActiveWindow(pTopLevelWin, 
                            WA_CLICKACTIVE, TRUE);

                        // INPUTDEV_SetActiveWindow可能会导致窗口MoveToTop,
                        // 立即重画窗口及其子窗口, 否则可能引起应用程序绘图
                        // 的异常(比如, 点在窗口边框上需要画移动虚框时)
                        WND_UpdateTree(pTopLevelWin);
                    }
                    
                    if (nActivate == MA_ACTIVATEANDEAT || 
                        nActivate == MA_NOACTIVATEANDEAT)
                        return FALSE;
                }
            }
        }
    }

    /* 对于进行过切换的模式对话框，其Owner窗口不接收鼠标的实际消息 */
    if (bModuleWin)
        return FALSE;
    
    pMsg->hwnd = (HWND)WOT_GetHandle((PWSOBJ)pWin);

    if (bClient)
    {
        WND_ScreenToClient(pWin, &x, &y);

        pMsg->message = nEvent + MOUSE_MESSAGE_BASE;
        pMsg->wParam = pThreadInfo->wBtnState;
        pMsg->lParam = MAKELPARAM(x, y);

        if (pThreadInfo->wKeyState & CKS_SHIFT)
            pMsg->wParam |= MK_SHIFT;
        
        if (pThreadInfo->wKeyState & CKS_CONTROL)
            pMsg->wParam |= MK_CONTROL;
    }
    else
    {
        pMsg->message =  nEvent + NCMOUSE_MESSAGE_BASE;
        pMsg->wParam = (WPARAM)nHitTest;
        pMsg->lParam = MAKELPARAM(x, y);
    }

    return TRUE;
}

/*
**  Function : ProcessKeyboardInput
**  Purpose  :
*/
/* 管程内部函数 */
static BOOL ProcessKeyboardInput(PMSG pMsg, PKBDEV pKbdEvent)
{
    PWINOBJ pWin;
    LPARAM lKeyData;
    static BOOL bCapsDown = FALSE;
    PTHREADINFO pThreadInfo;

    pThreadInfo =WS_GetCurrentThreadInfo();

    switch (pKbdEvent->vkCode)
    {
#ifdef KEYBOARD_GERMANY

    case VK_RALT :

        if (pKbdEvent->flags & KEYEVENTF_KEYUP)
            pThreadInfo->wKeyState &= (WORD)~CKS_ALTGR;
        else
            pThreadInfo->wKeyState |= CKS_ALTGR;

        break;
#endif

    case VK_SHIFT :

        if (pKbdEvent->flags & KEYEVENTF_KEYUP)
            pThreadInfo->wKeyState &= (WORD)~CKS_SHIFT;
        else
            pThreadInfo->wKeyState |= CKS_SHIFT;

        break;

    case VK_CONTROL :

        if (pKbdEvent->flags & KEYEVENTF_KEYUP)
            pThreadInfo->wKeyState &= (WORD)~CKS_CONTROL;
        else
            pThreadInfo->wKeyState |= CKS_CONTROL;

        pThreadInfo->wKeyAfterSpecKey = 0;

        break;

    case VK_ALT :

        if (pKbdEvent->flags & KEYEVENTF_KEYUP)
            pThreadInfo->wKeyState &= (WORD)~CKS_ALT;
        else
            pThreadInfo->wKeyState |= CKS_ALT;

        pThreadInfo->wKeyAfterSpecKey = 0;

        break;

    case VK_CAPITAL :

        if (!(pKbdEvent->flags & KEYEVENTF_KEYUP))
        {
            if (!bCapsDown)
            {
                bCapsDown = TRUE;
                pThreadInfo->wKeyState ^= CKS_CAPS;
            }
        }
        else
            bCapsDown = FALSE;

        break;

    default :

        // STB产品一般使用遥控键盘，键盘抬起事件容易丢失。当control或alt键
        // 的抬起事件丢失时，会造成focus窗口无法收到正常的WM_CHAR消息引起
        // 键盘输入异常。为了避免这种情况的发生，使control或alt键的按下状
        // 态只维持一次，当第二个非控制键键按下时，自动清除control或alt键的
        // 按下状态

        if (pKbdEvent->flags & KEYEVENTF_KEYUP)
            break;

        if ((pThreadInfo->wKeyState & CKS_CONTROL) || 
            (pThreadInfo->wKeyState & CKS_ALT))
        {
            if ((pThreadInfo->wKeyAfterSpecKey != 0) && 
                (pThreadInfo->wKeyAfterSpecKey != pKbdEvent->vkCode))
            {
                pThreadInfo->wKeyState &= 
                    (WORD)~(CKS_CONTROL | CKS_ALT);
            }
            else
                pThreadInfo->wKeyAfterSpecKey = pKbdEvent->vkCode;
        }
    }

    lKeyData = (LPARAM)MAKELPARAM(1, pKbdEvent->scanCode);

    if (pKbdEvent->flags & KEYEVENTF_EXTENDEDKEY)
        lKeyData |= 1L << 24;
    lKeyData |= (DWORD)(pThreadInfo->wKeyState & CKS_MASK) << 25;

    if (pKbdEvent->flags & KEYEVENTF_KEYUP)
        lKeyData |= 0xC0000000;
    else
        lKeyData |= 0x00000000;

#ifndef NOWH

    // Calls keyboard hook function
    if (WH_CallHookProc(WH_KEYBOARD, HC_ACTION, (WPARAM)pKbdEvent->vkCode, 
        lKeyData))
        return FALSE;

#endif  // NOWH
    
    if (pThreadInfo->pKeyGrabWin != NULL)
        pWin = pThreadInfo->pKeyGrabWin;
    else
    {
        pWin = pThreadInfo->pFocusWin;
        if (pWin && !WND_IsEnabled(pWin))
            return FALSE;
    }

    if (pWin && !(pThreadInfo->wKeyState & CKS_ALT) && 
        pKbdEvent->vkCode != VK_ALT)
    {
        if (pKbdEvent->flags & KEYEVENTF_KEYUP)
            pMsg->message = WM_KEYUP;
        else
            pMsg->message = WM_KEYDOWN;
    }
    else
    {
        // The WM_SYSKEYDOWN and WM_SYSKEYUP message is posted to the 
        // window with the keyboard focus when the user releases a key 
        // that was pressed while the ALT key was held down. It also 
        // occurs when no window currently has the keyboard focus; in 
        // this case, the WM_SYSKEYDOWN and WM_SYSKEYUP message is sent 
        // to the active window. The window that receives the message can
        // distinguish between these two contexts by checking the context
        // code in the lKeyData parameter. 

        // If no window currently has the keyboard focus, send 
        // WM_SYSKEYDOWN or WM_SYSKEYUP message to the active window.
        if (!pWin)
        {
            pWin = pThreadInfo->pActiveWin;
            if (!pWin || !WND_IsEnabled(pWin))
                return FALSE;
        }
        else
            lKeyData |= 0x20000000; // Indicates ALT system key message

        if (pKbdEvent->flags & KEYEVENTF_KEYUP)
            pMsg->message = WM_SYSKEYUP;
        else
            pMsg->message = WM_SYSKEYDOWN;
    }

    pMsg->hwnd   = (HWND)WOT_GetHandle((PWSOBJ)pWin);
    pMsg->wParam = (WPARAM)pKbdEvent->vkCode;
    pMsg->lParam = lKeyData;

    return TRUE;
}
