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

#include "stdio.h"
#include "errno.h"
#include "string.h"
#include "unistd.h"
#include "fcntl.h"
#include "hopen/devmajor.h"
#include "sys/task.h"
#include "sys/poll.h"
#include "sys/ioctl.h"

//#include "virtkey.h"
#define BTP04

#define KS_DOWN         0x01
#define KS_NUMLOCK      0x02
#define KS_CAPSLOCK     0x04
#define KS_SHIFT        0x08
#define KS_ALT          0x10
#define KS_CTRL         0x20
#define KS_INSERT       0x40
#define KS_EXTENDKEY    0x80

#define LED_SCROLLLOCK  0x01
#define LED_NUMLOCK     0x02
#define LED_CAPSLOCK    0x04

#define BTN_LEFTDOWN    0x0004
#define BTN_MIDDLEDOWN  0x0002
#define BTN_RIGHTDOWN   0x0001
#define MOUBUFFER       6

#define KDSKBLED        0x4B65
#define STACKSIZE       4096

//static void event_capture(void* param);
static int  event_capture(void* param);
static int  UpdateKeyboard(void);
static int  UpdateMouse(void);
static void PostKeyEvent(unsigned char scancode, unsigned char vkey, 
                         int keydown);
static void ChangeFgThread(DWORD dwThreadId, int nActive);

PWINOBJ WND_FromPointEx(int x, int y);

// Define mouse message and non client mouse message base value
#define MOUSE_MESSAGE_BASE      WM_MOUSEMOVE
#define NCMOUSE_MESSAGE_BASE    WM_NCMOUSEMOVE
//static BOOL IsSysModalWindowDescendant(PWINOBJ pWin);

// Define mouse capture mode
#define CAPTURE_NULL        0   // Mouse isn't captured
#define CAPTURE_CLIENT      1   // Mouse is client capture
#define CAPTURE_NCLIENT     2   // Mouse is non client capture

#define	SCALE   3       /* default scaling factor for acceleration */
#define	THRESH  5       /* default threshhold for acceleration */

static int  xpos = 0;   /* current x position of mouse */
static int  ypos = 0;   /* current y position of mouse */
static int  minx;       /* minimum allowed x position  */
static int  maxx;       /* maximum allowed x position  */
static int  miny;       /* minimum allowed y position  */
static int  maxy;       /* maximum allowed y position  */
static int  scale;      /* acceleration scale factor   */
static int  thresh;     /* acceleration threshhold     */

static int  ledstate    = 0;
static int  keystate    = 0;

static int  mouse_fd    = 0;
static int  kbd_fd      = 0;

//static int  buttons[4]  = {0, 4, 1, 5}; 
static WORD btn_stat    = 0xffff;

#define MOUSE_EVENT MouseEvent
#define KEYBD_EVENT KeyboardEvent

typedef struct
{
    PTHREADINFO pFgThreadInfo;  // foreground thread
    PWINOBJ     pFgWin;

    WORD    wKeyState;          // Special key state
    WORD    wBtnState;          // Mouse button state 

    WORD    wKeyAfterSpecKey;   // the pressed key after control key
    WORD    wLastChar;          // last char code
//    PWINOBJ pPenCalibrationWin; // The window used to calibrate the pen
} INPUTDEVDATA, *PINPUTDEVDATA;

static INPUTDEVDATA InputDevData;

/*
**  Function : INPUTMGR_Init
*/
BOOL INPUTMGR_Init(void)
{
    int scr_width, scr_height;
    struct taskcreate cdata;

    InputDevData.pFgThreadInfo = WS_GetThreadInfo(g_pRootWin->dwThreadId);

#if (!NOKEYBOARDDEV)
    kbd_fd = opendev(KBD_MAJOR, 0, O_RDONLY|O_NONBLOCK);
    if (kbd_fd < 0)
    {
        TRACE("Keyboard device open failed!\r\n");
        return FALSE;
    }
    TRACE("Keyboard device open success!\r\n");
#endif

#ifndef BTP04
    mouse_fd = opendev(TOUCHPEN_MAJOR, 0, O_RDONLY|O_NONBLOCK);
    if (mouse_fd < 0)
    {
        TRACE("Mouse device open failed!\r\n");
        close(kbd_fd);
        return FALSE;
    }
#endif   
    if (!InitLongKey())
    {
        TRACE("LongKey init failed!\r\n");
    }
    else
    {
        TRACE("LongKey init success!\r\n");
    }

    TRACE("Mouse device open success!\r\n");

    scr_width   = DISPDEV_GetCaps(HORZRES);
    scr_height  = DISPDEV_GetCaps(VERTRES);
    TRACE("screen width = %d  \theight = %d\r\n", scr_width, scr_height);

    xpos        = scr_width / 2;
    ypos        = scr_height / 2;
    minx        = 0;
    maxx        = scr_width - 1;
    miny        = 0;
    maxy        = scr_height - 1;
    scale       = SCALE;
    thresh      = THRESH;

    // start keyboard and mouse event capturing thread 
    memset(&cdata, 0, sizeof(cdata));
    //strncpy(cdata.name, (char *)"kbd&mou", 8);
    cdata.prio = 255;
    cdata.flags = 0;
    cdata.entry = event_capture;
    cdata.param = 0;
    cdata.stkTop = (void *)LocalAlloc(LMEM_FIXED, STACKSIZE);
    if (cdata.stkTop == NULL)
    {
        return FALSE;
    }
    cdata.stkTop = (char *)cdata.stkTop + STACKSIZE;

    if (CreateTaskEx(&cdata) <= 0)
    {
        LocalFree((char *)cdata.stkTop - STACKSIZE);
        return FALSE;
    }

    return TRUE;
}

void INPUTMGR_Exit(void)
{
    close(kbd_fd);
#ifndef BTP04
    close(mouse_fd);
#endif
}

#if (!NOKEYBOARDDEV)
//static void event_capture(void* param)
static int event_capture(void* param)
{
    int nRet;
#ifndef BTP04
    struct pollfd fds[2] = 
#else
	struct pollfd fds[1] = 
#endif
    {
        {0, 0, POLLIN, 0}, 
#ifndef BTP04
        {0, 0, POLLIN, 0}, 
#endif
    };

    fds[0].fd = kbd_fd;
#ifndef BTP04
    fds[1].fd = mouse_fd;
#endif
//    printf("event capture, kbd_fd: %d, mouse_fd: %d\n", kbd_fd, mouse_fd);
    //fcntl(kbd_fd,   F_SETFL, O_NONBLOCK);
    //fcntl(mouse_fd, F_SETFL, O_NONBLOCK);
    TRACE("event_capture is running...\r\n");

    while (1)
    {
#ifndef BTP04
        nRet = poll(fds, 2, -1);
#else
        nRet = poll(fds, 1, -1);
#endif

        if (nRet == -1)
        {
            TRACE("poll error\r\n");
            break;
        }

//      TRACE("Event is coming from poll\r\n");

        if (fds[0].revents & POLLIN)
        {
//            TRACE("Keyboard event is coming!\r\n");
            UpdateKeyboard();   //handle keybord input
        }
#ifndef BTP04
        if (fds[1].revents & POLLIN)
        {
//            TRACE("Mouse event is coming!\r\n");
            UpdateMouse();  //handle mouse input
        }
#endif
    }

    EndTask(0, 0);
    return 0;
//    OS_EndTask(0);
}
#else
//static void event_capture(void* param)
static int event_capture(void* param)
{
    int nRet;
    struct pollfd fds[1] = 
    {
        {0, 0, POLLIN, 0}, 
    };

    fds[0].fd = mouse_fd;
//    printf("event capture, kbd_fd: %d, mouse_fd: %d\n", kbd_fd, mouse_fd);
    //fcntl(kbd_fd,   F_SETFL, O_NONBLOCK);
    //fcntl(mouse_fd, F_SETFL, O_NONBLOCK);
    TRACE("event_capture is running...\r\n");
    while (1)
    {
        nRet = poll(fds, 1, -1);

        if (nRet == -1)
        {
            TRACE("poll error\r\n");
            break;
        }

//        TRACE("Event is coming from poll\r\n");

        if (fds[0].revents & POLLIN)
        {
//            TRACE("Mouse event is coming!\r\n");
            UpdateMouse();  //handle mouse input
        }
    }

    TRACE("event_capture is breaken!\r\n");
    EndTask(0, 0);
    return 0;
//    OS_EndTask(0);
}

#endif //NOKEYBOARDDEV


/**************************************************************************/
/*          Low Level Input Operations                                    */
/**************************************************************************/

/*                
**  Function : UpdateMouse
**  Purpose  :
**      Read mouse, generate event and put it into system event queue.
**  Params   :
**  
**  Return   :
**      If the function successes, return 1.
**      If there is no data to read from mouse, return 0.
**      If there is an error, return -1.
*/
#if 1
static int UpdateMouse(void)
{
    static unsigned char buf[MOUBUFFER];
    int n, err;
    int button;
    static int nbytes = 0;
    static int xprev = 0;
    static int yprev = 0;

    // read mouse input data
    while (nbytes < MOUBUFFER && 
        (n = read(mouse_fd, &buf[nbytes], MOUBUFFER - nbytes)) != 0)
    {
        if (n < 0)
        {
            err = errno; //OS_GetError();
            if ((err == EINTR) || (err == EAGAIN))
                return 0;
            else
                return -1;
        }

        nbytes += n;
    }

    if (n == 0)
    {
        int a;
        a = 0;
    }
    nbytes = 0;

    button = buf[0];
    xpos = (int)(((unsigned short)buf[2] << 8) + buf[3]);
    ypos = (int)(((unsigned short)buf[4] << 8) + buf[5]);

//    TRACE("button state = %d\r\n", button);

    if (btn_stat == button)
    {
        if (xpos != xprev || ypos != yprev)
        {
            MOUSE_EVENT(MOUSEEVENTF_MOVE | MOUSEEVENTF_FROMDEVICE | 
                MOUSEEVENTF_ABSOLUTE, xpos, ypos, 0, 0);
            
            xprev = xpos;
            yprev = ypos;
        }
    }
//    if (btn_stat != button) // 笔的按键状态改变, 发送相应笔事件
    else
    {
        if (button == 0)
        {
            // 笔按下
            MOUSE_EVENT(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_FROMDEVICE |
                MOUSEEVENTF_ABSOLUTE, xpos, ypos, 0, 0);
        }
        else
        {
            // 笔抬起
            MOUSE_EVENT(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_FROMDEVICE | 
                MOUSEEVENTF_ABSOLUTE, xpos, ypos, 0, 0);
        }

        btn_stat = button;
    }

    return 1;
}
#else
static int UpdateMouse(void)
{
    static unsigned char buf[MOUBUFFER];
    int sign;
    int dx, dy;
    int n, err;
    int button;
    static int nbytes = 0;
    static int xprev = 0;
    static int yprev = 0;

    // read mouse input data
    while (nbytes < MOUBUFFER && 
        (n = read(mouse_fd, &buf[nbytes], MOUBUFFER - nbytes)) != 0)
    {
        if (n < 0)
        {
            err = errno; //OS_GetError();
            if ((err == EINTR) || (err == EAGAIN))
                return 0;
            else
                return -1;
        }

        nbytes += n;
    }
    // check header byte
    if ((buf[0] & 0xc0) != 0)
    {
        buf[0] = buf[1];
        buf[1] = buf[2];
        nbytes = 2;

        return -1;
    }
    nbytes = 0;

    button = buf[0];
    xpos = (int)(((unsigned short)buf[2] << 8) + buf[3]);
    ypos = (int)(((unsigned short)buf[4] << 8) + buf[5]);
    dx = xpos - xprev;
    dy = ypos - yprev;
    xprev = xpos;
    yprev = ypos;
/*    
    button = buttons[(buf[0] & 0x03)];
    dx = (buf[0] & 0x10) ? buf[1] - 256 : buf[1];
    dy = (buf[0] & 0x20) ? -(buf[2] - 256) : -buf[2];

    sign = 1;
    if (dx < 0)
    {
        sign = -1;
        dx = -dx;
    }
    if (dx > thresh)
        dx = thresh + (dx - thresh) * scale;
    dx *= sign;
    xpos += dx;
    if (xpos < minx)
        xpos = minx;
    if (xpos > maxx)
        xpos = maxx;

    sign = 1;
    if (dy < 0)
    {
        sign = -1;
        dy = -dy;
    }
    if (dy > thresh)
        dy = thresh + (dy - thresh) * scale;
    dy *= sign;
    ypos += dy;
    if (ypos < miny)
        ypos = miny;
    if (ypos > maxy)
        ypos = maxy;
*/
    if (btn_stat == button)
    {
        // 发送鼠标移动事件
        if (dx != 0 || dy != 0)
        {
            MOUSE_EVENT(MOUSEEVENTF_MOVE | MOUSEEVENTF_FROMDEVICE | 
                MOUSEEVENTF_ABSOLUTE, xpos, ypos, 0, 0);
        }
    }
    else
    {
        // 鼠标的按键状态改变, 发送相应鼠标事件
        if ((btn_stat & BTN_LEFTDOWN) != (button & BTN_LEFTDOWN))
        {
            if (button & BTN_LEFTDOWN)
            {
                MOUSE_EVENT(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_FROMDEVICE |
                    MOUSEEVENTF_ABSOLUTE, xpos, ypos, 0, 0);
            }
            else
            {
               // 如果鼠标抬起事件同时又移动, 产生鼠标移动事件和鼠标
                // 抬起事件
                if (dx != 0 || dy != 0)
                {
                    MOUSE_EVENT(MOUSEEVENTF_MOVE | MOUSEEVENTF_FROMDEVICE | 
                        MOUSEEVENTF_ABSOLUTE, xpos, ypos, 0, 0);
                }

                MOUSE_EVENT(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_FROMDEVICE | 
                    MOUSEEVENTF_ABSOLUTE, xpos, ypos, 0, 0);
            }

            btn_stat ^= BTN_LEFTDOWN;
        }
        if ((btn_stat & BTN_RIGHTDOWN) != (button & BTN_RIGHTDOWN))
        {
            if (button & BTN_RIGHTDOWN)
            {
                MOUSE_EVENT(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_FROMDEVICE |
                    MOUSEEVENTF_ABSOLUTE, xpos, ypos, 0, 0);
            }
            else
            {
                // 如果鼠标抬起事件同时又移动, 产生鼠标事件和鼠标抬起
                // 移动事件
                if (dx != 0 || dy != 0)
                {
                    MOUSE_EVENT(MOUSEEVENTF_MOVE | MOUSEEVENTF_FROMDEVICE |
                        MOUSEEVENTF_ABSOLUTE, xpos, ypos, 0, 0);
                }

                MOUSE_EVENT(MOUSEEVENTF_RIGHTUP | MOUSEEVENTF_FROMDEVICE |  
                    MOUSEEVENTF_ABSOLUTE, xpos, ypos, 0, 0);
            }
            
            btn_stat ^= BTN_RIGHTDOWN;                    
        }

        if ((btn_stat & BTN_MIDDLEDOWN) != (button & BTN_MIDDLEDOWN))
        {
            if (button & BTN_MIDDLEDOWN)
            {
                MOUSE_EVENT(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_FROMDEVICE | 
                    MOUSEEVENTF_ABSOLUTE, xpos, ypos, 0, 0);
            }
            else
            {
                // 如果鼠标抬起事件同时又移动, 产生鼠标移动事件和鼠标
                // 抬起事件

                if (dx != 0 || dy != 0)
                {
                    MOUSE_EVENT(MOUSEEVENTF_MOVE | MOUSEEVENTF_FROMDEVICE |
                        MOUSEEVENTF_ABSOLUTE ,xpos, ypos, 0, 0);
                }

                MOUSE_EVENT(MOUSEEVENTF_MIDDLEUP | MOUSEEVENTF_FROMDEVICE |
                    MOUSEEVENTF_ABSOLUTE, xpos, ypos, 0, 0);
            }
            
            btn_stat ^= BTN_MIDDLEDOWN;                    
        }
    }

    return 1;
}
#endif

struct key_table
{
    unsigned char vkey;
    unsigned char basecode;
    unsigned char shiftcode;
    unsigned char unused;
};

static struct key_table table_1[] = {
    {VK_SPACE,      ' ',    ' '},
    {VK_1,          '1',    '!'}, 
    {VK_2,          '2',    '@'}, 
    {VK_3,          '3',    '#'}, 
    {VK_4,          '4',    '$'}, 
    {VK_5,          '5',    '%'}, 
    {VK_6,          '6',    '^'}, 
    {VK_7,          '7',    '&'}, 
    {VK_8,          '8',    '*'},
    {VK_9,          '9',    '('}, 
    {VK_0,          '0',    ')'}, 
    {VK_WAVE,       '`',    '~'},
    {VK_MINUS,      '-',    '_'},
    {VK_EQUAL,      '=',    '+'},
    {VK_LBRACKET,   '[',    '{'},   
    {VK_RBRACKET,   ']',    '}'},     
    {VK_SEMICOLON,  ';',    ':'},
    {VK_QUOTA,      '\'',   '"'},
    {VK_COMMA,      ',',    '<'},        
    {VK_PERIOD,     '.',    '>'},
    {VK_QUESTION,   '/',    '?'},
    {VK_SLASH,      '\\',   '|'},
    {VK_ADD,        '+',    '+'},
    {VK_SUBTRACT,   '-',    '-'},
    {0, 0, 0}
};

static const unsigned char vkey_table[0x80] = {
    0,                  // NONE
    VK_ESCAPE,          // ESC         01
    VK_1,               // 1           02
    VK_2,               // 2           03
    VK_3,               // 3           04
    VK_4,               // 4           05
    VK_5,               // 5           06
    VK_6,               // 6           07
    VK_7,               // 7           08
    VK_8,               // 8           09
    VK_9,               // 9           0A
    VK_0,               // 0           0B
    VK_MINUS,           // -           0C
    VK_EQUAL,           // =           0D
    VK_BACK,            // BACKSPACE   0E
    VK_TAB,             // TAB         0F
    VK_Q,               // Q           10
    VK_W,               // W           11
    VK_E,               // E           12
    VK_R,               // R           13
    VK_T,               // T           14
    VK_Y,               // Y           15
    VK_U,               // U           16
    VK_I,               // I           17
    VK_O,               // O           18
    VK_P,               // P           19
    VK_LBRACKET,        // [           1A
    VK_RBRACKET,        // ]           1B
    VK_RETURN,          // RETURN      1C
    VK_CONTROL,         // L CTRL      1D
    VK_A,               // A           1E
    VK_S,               // S           1F
    VK_D,               // D           20
    VK_F,               // F           21
    VK_G,               // G           22
    VK_H,               // H           23
    VK_J,               // J           24
    VK_K,               // K           25
    VK_L,               // L           26
    VK_SEMICOLON,       // ;           27
    VK_QUOTA,           // '           28
    VK_WAVE,            // ~           29
    VK_SHIFT,           // L SHIFT     2A
    VK_SLASH,           // \           2B
    VK_Z,               // Z           2C
    VK_X,               // X           2D
    VK_C,               // C           2E
    VK_V,               // V           2F
    VK_B,               // B           30
    VK_N,               // N           31
    VK_M,               // M           32
    VK_COMMA,           // ,           33
    VK_PERIOD,          // .           34
    VK_QUESTION,        // /           35
    VK_SHIFT,           // R SHIFT     36
    VK_MULTIPLY,        // KP *        37
    VK_MENU,            // ALT         38
    VK_SPACE,           // Space       39
    VK_CAPITAL,         // CAPS LOCK   3A
    VK_F1,              // F1          3B
    VK_F2,              // F2          3C
    VK_F3,              // F3          3D
    VK_F4,              // F4          3E
    VK_F5,              // F5          3F
    VK_F6,              // F6          40
    VK_F7,              // F7          41
    VK_F8,              // F8          42
    VK_F9,              // F9          43
    VK_F10,             // F10         44
    VK_NUMLOCK,         // NUM LOCK    45
    VK_SCROLL,          // SCROLL LOCK 46
    VK_HOME,            // KP 7        47       
    VK_UP,              // KP 8        48       
    VK_PRIOR,           // KP 9        49       
    VK_SUBTRACT,        // KP -        4A       
    VK_LEFT,            // KP 4        4B       
    VK_CLEAR,           // KP 5        4C       
    VK_RIGHT,           // KP 6        4D       
    VK_ADD,             // KP +        4E       
    VK_END,             // KP 1        4F       
    VK_DOWN,            // KP 2        50       
    VK_NEXT,            // KP 3        51       
    VK_INSERT,          // KP 0        52       
    VK_DELETE,          // KP .        53       
    VK_SC_54,           //             54           
    VK_SC_55,           //             55           
    VK_SC_56,           //             56           
    VK_F11,             // F11         57       
    VK_F12,             // F12         58
    /* Added on 2004/02/18 for user defined vk */
    VK_SC_59,           //             59 
    VK_SC_5A,           //             5A 
    VK_SC_5B,           //             5B 
    VK_SC_5C,           //             5C 
    VK_SC_5D,           //             5D 
    VK_SC_5E,           //             5E 
    VK_SC_5F,           //             5F 
/* Up of 60 is e0. It's a extend scancode. So it can't be used */
    VK_UNDEFINE,        //             60 
    VK_SC_61,           //             61 
    VK_SC_62,           //             62 
    VK_SC_63,           //             63 
    VK_SC_64,           //             64
    VK_SC_65,           //             65
    VK_SC_66,           //             66 
    VK_SC_67,           //             67 
    VK_SC_68,           //             68 
    VK_SC_69,           //             69 
    VK_SC_6A,           //             6A 
    VK_SC_6B,           //             6B 
    VK_SC_6C,           //             6C 
    VK_SC_6D,           //             6D 
    VK_SC_6E,           //             6E 
    VK_SC_6F,           //             6F 
    VK_SC_70,           //             70 
    VK_SC_71,           //             71 
    VK_SC_72,           //             72 
    VK_SC_73,           //             73 
    VK_SC_74,           //             74 
    VK_SC_75,           //             75 
    VK_SC_76,           //             76 
    VK_SC_77,           //             77 
    VK_SC_78,           //             78 
    VK_SC_79,           //             79 
    VK_SC_7A,           //             7A 
    VK_SC_7B,           //             7B 
    VK_SC_7C,           //             7C 
    VK_SC_7D,           //             7D 
    VK_SC_7E,           //             7E 
    VK_SC_7F            //             7F
};

#if (!NOKEYBOARDDEV)

/*                
**  Function : UpdateKeyboard
**  Purpose  :
**      Read keyboard, generate event and put it into system event queue.
**  Params   :
**  
**  Return   :
**      If the function successes, return 1.
**      If there is no data to read from mouse, return 0.
**      If there is an error, return -1.
*/
static int UpdateKeyboard(void)
{
    unsigned char vkey, scancode;
    static int enhanced = 0;
    int ret;

readagain:
    ret = read(kbd_fd, &scancode, 1);
//    TRACE("scancode: %x, enhanced: %d\r\n", scancode, enhanced);
//    return 1;

    if ((ret == -1) || (ret == 0))
    {
        TRACE("keybd read error!\r\n");
        TRACEERROR();
        return 0;
    }

    if (scancode == 0xe0)
    {
        enhanced = 1;
        goto readagain;
    }
    else //else if ((scancode & 0x7f) <= 0x58)
    {   
        if (enhanced)
            keystate |= KS_EXTENDKEY;
        else
            keystate &= ~KS_EXTENDKEY;       
                
        enhanced = 0;
    }
/*    else
    {
        enhanced = 0;
        return 1;
    }*/

#ifndef _EMULATE_
    vkey =scancode &0x7f; // vkey_table[scancode & 0x7f];
#else
	vkey=vkey_table[scancode & 0x7f];
#endif

    // Check if this key is shift key 
    switch (scancode)
    {
    case 0x1D:   // control key
        keystate |= KS_CTRL;
        break;
                
    case 0x9D:   // control key
        keystate &= ~KS_CTRL;
        break;
                
    case 0x2A:
    case 0x36:  // Shift key
        keystate |= KS_SHIFT;
        break;
                
    case 0xAA:
    case 0xB6:  // Shift key
        keystate &= ~KS_SHIFT;
        break;
                
    case 0x38:  // Alt Key
        keystate |= KS_ALT;
        break;

    case 0xB8:  // Alt Key
        keystate &= ~KS_ALT;
        break;

    case 0x3A:  // Caps lock key
        if ((keystate & KS_EXTENDKEY))
        {
            keystate ^= KS_CAPSLOCK;
            ledstate ^= LED_CAPSLOCK;
            printf("set CAPSLOCK\n");
            ioctl(KDSKBLED, ledstate);
        }
        break;

    case 0x46:  // Scroll Lock key
        if ((keystate & KS_EXTENDKEY))
        {
            ledstate ^= LED_SCROLLLOCK;
            printf("set SCROLLLOCK\n");
            ioctl(KDSKBLED, ledstate);
        }
        break;

    case 0x45:  // Num lock key
        if ((keystate & KS_EXTENDKEY))
        {
            keystate ^= KS_NUMLOCK;
            ledstate ^= LED_NUMLOCK;
            printf("set NUMLOCK\n");
            ioctl(KDSKBLED, ledstate);
        }
        break;
    
    case 0x47: 
    case 0xc7: // KeyPad 7
        if ((keystate & KS_NUMLOCK) && !(keystate & KS_EXTENDKEY))
			vkey = VK_7;
        break;
    
    case 0x48: 
    case 0xc8: // KeyPad 8
        if ((keystate & KS_NUMLOCK) &&!(keystate & KS_EXTENDKEY))
			vkey = VK_8;
        break;

    case 0x49: 
    case 0xc9: // KeyPad 9
        if ((keystate & KS_NUMLOCK) && !(keystate & KS_EXTENDKEY))
			vkey = VK_9;
        break;

    case 0x4a: 
    case 0xca: // KeyPad -
        if (!(keystate & KS_SHIFT))
			vkey = VK_MINUS;
        break;

    case 0x4b: 
    case 0xcb: // KeyPad 4
        if ((keystate & KS_NUMLOCK) && !(keystate & KS_EXTENDKEY))
			vkey = VK_4;
        break;

    case 0x4c: 
    case 0xcc: // KeyPad 5
        if ((keystate & KS_NUMLOCK) &&!(keystate & KS_EXTENDKEY))
			vkey = VK_5;
        break;

    case 0x4d: 
    case 0xcd: // KeyPad 6
        if ((keystate & KS_NUMLOCK) && !(keystate & KS_EXTENDKEY))
			vkey = VK_6;
        break;

    case 0x4f: 
    case 0xcf: // KeyPad 1
        if ((keystate & KS_NUMLOCK) && !(keystate & KS_EXTENDKEY))
			vkey = VK_1;
        break;

    case 0x50: 
    case 0xd0: // KeyPad 2
        if ((keystate & KS_NUMLOCK) && !(keystate & KS_EXTENDKEY))
			vkey = VK_2;
        break;

    case 0x51: 
    case 0xd1: // KeyPad 3
        if ((keystate & KS_NUMLOCK) &&!(keystate & KS_EXTENDKEY))
			vkey = VK_3;
        break;

    case 0x52: 
    case 0xd2: // KeyPad 0
        if ((keystate & KS_NUMLOCK) && !(keystate & KS_EXTENDKEY))
			vkey = VK_0;
        break;

    case 0x53: 
    case 0xd3: // KeyPad .
        if ((keystate & KS_NUMLOCK) &&!(keystate & KS_EXTENDKEY) && 
			!(keystate & KS_SHIFT))
			vkey = VK_PERIOD;
        break;

    default:
        break;
    }

    if (scancode & 0x80)
    {
        DWORD dwRet;

        dwRet = EndLongKey((WORD)(scancode & 0x7F), (WORD)vkey);
        if (dwRet & SHORTKEY_DOWN)
            PostKeyEvent((BYTE)(scancode & 0x7F), vkey, 1);

        if (dwRet & SHORTKEY_UP)
            PostKeyEvent(scancode, vkey, 0);
    }
    else
    {
        DWORD dwRet;

        dwRet = StartLongKey((WORD)(scancode & 0x7F), (WORD)vkey);
        if (dwRet & SHORTKEY_DOWN)
            PostKeyEvent(scancode, vkey, 1);
    }

    return 1;
}

static void PostKeyEvent(unsigned char scancode, unsigned char vkey, 
                         int keydown)
{
    unsigned char ascii;
    int i;
    DWORD flags;

//    TRACE("vkCode = 0x%0X, scanCode = 0x%0X\r\n", vkey, scancode);

    scancode &= 0x7f;
    
    // Change VKEY to ASCII  
    if (vkey >= VK_A && vkey <= VK_Z)
    {
        ascii = vkey;
        //Check if is lower case  
        if (!((keystate & (KS_SHIFT|KS_CAPSLOCK)) == KS_SHIFT ||
            (keystate & (KS_SHIFT|KS_CAPSLOCK)) == KS_CAPSLOCK))
            ascii += 32;
    }
    else // Check for other ascii key  
    {
        for (i = 0; table_1[i].vkey != 0; i++)
        {
            if (vkey == table_1[i].vkey)
            {
                ascii = (keystate & KS_SHIFT) ?
                    table_1[i].shiftcode : table_1[i].basecode;
                break;
            }
        }   
    }
    // Modify keyboard + state  
    if ((scancode == 0x4e) && (keystate && KS_NUMLOCK))
    {
        ascii = '+';
        vkey  = VK_EQUAL;
    }

    flags = KEYEVENTF_FROMDEVICE;
    if (!keydown)
        flags |= KEYEVENTF_KEYUP;
    if (keystate & KS_EXTENDKEY)
        flags |= KEYEVENTF_EXTENDEDKEY;

    KEYBD_EVENT(vkey, scancode, flags, 0);
}
#endif

/************************************************************************************
**  Function : KeyboardEvent
**  Purpose  : 
************************************************************************************/
void KeyboardEvent(int byVirtKey, int bScan, DWORD dwFlags, DWORD dwExtraInfo)
{
    {
        /*处理热键*/
    }

    WAITMSGQUEOBJ;

    DEVQ_PutKeyboardEvent(InputDevData.pFgThreadInfo, 
        (BYTE)byVirtKey, (BYTE)bScan, dwFlags, dwExtraInfo);

    RELEASEMSGQUEOBJ;
}

/************************************************************************************
**  Function : MouseEvent 
**  Purpose  : 输入事件线程，应该不涉及到窗口的消息分发，否则也需要线程消息结构
************************************************************************************/
void MouseEvent(DWORD dwFlags, LONG dx, LONG dy, DWORD dwData, DWORD dwExtraInfo)
{
    PWINOBJ pWin = NULL;
    PWINOBJ pWinFromPoint = NULL;
    POINT   pt;

    WAITMSGQUEOBJ;

    if (!(dwFlags & MOUSEEVENTF_ABSOLUTE))
    {
        // Gets current mouse position
        WS_GetCursorPos(&pt);

        //convert relative coord to absolute coord
        dx += pt.x;
        dy += pt.y;

        dwFlags |= MOUSEEVENTF_ABSOLUTE;
    }

    /* 已经判断了透明情况 */
    pWinFromPoint = WND_FromPointEx(dx, dy);

    {
        //处理一些高优先级的操作，比如关闭一个窗口线程
    }

    ASSERT(pWinFromPoint != NULL);
    if (pWinFromPoint == NULL)
    {
        RELEASEMSGQUEOBJ;
        return ;
    }

    //find the target win
    pWin = InputDevData.pFgThreadInfo->pCaptureWin;
    if (pWin == NULL)
        pWin = pWinFromPoint;
    ASSERT(pWin != NULL);

    if (dwFlags & MOUSEEVENTF_MOVE)
    {
        /* 只有当活动窗口是捕获窗口时，才能得到任何位置的光标消息 ，否则
        ** 即使有捕获窗口，当不是活动窗口时，也得不到窗口外的消息
        */
        if (InputDevData.pFgThreadInfo->pCaptureWin &&
            ((InputDevData.wBtnState & MK_LBUTTON) ||
            (InputDevData.wBtnState & MK_RBUTTON) ||
            (InputDevData.wBtnState & MK_MBUTTON)))
        {
            /* 前景线程活动窗口捕获鼠标 */
            DEVQ_PutMouseEvent(InputDevData.pFgThreadInfo, dwFlags, dx, dy, 
                dwData, dwExtraInfo, NULL);
        }
        else if ((InputDevData.wBtnState & MK_LBUTTON) ||
            (InputDevData.wBtnState & MK_RBUTTON) ||
            (InputDevData.wBtnState & MK_MBUTTON))
        {
            /* 有键处于按下状态，移动鼠标，则消息只能发向前景线程的活动窗口 */
            DEVQ_PutMouseEvent(InputDevData.pFgThreadInfo, dwFlags, dx, dy, 
                dwData, dwExtraInfo, NULL);
        }
        else
        {
            /* 其他光标移动消息直接发向目标窗口 */
            DEVQ_PutMouseEvent(WS_GetThreadInfo(pWinFromPoint->dwThreadId), 
                dwFlags, dx, dy, dwData, dwExtraInfo, NULL);
        }
    }
    else if ((dwFlags & MOUSEEVENTF_LEFTUP) || 
        (dwFlags & MOUSEEVENTF_RIGHTUP) ||
        (dwFlags & MOUSEEVENTF_MIDDLEUP))
    {
        /* 只有当活动窗口为捕获窗口时，收到消息 */
        if (InputDevData.pFgThreadInfo->pCaptureWin)
        {
            DEVQ_PutMouseEvent(InputDevData.pFgThreadInfo, dwFlags, dx, dy, 
                dwData, dwExtraInfo, NULL);//InputDevData.pFgThreadInfo->pActiveWin);
        }
        else if ((InputDevData.wBtnState & MK_LBUTTON) ||
            (InputDevData.wBtnState & MK_RBUTTON) ||
            (InputDevData.wBtnState & MK_MBUTTON))
        {
            /* 有键处于按下状态，抬起鼠标，则消息应发向前景线程的活动窗口 */
            DEVQ_PutMouseEvent(InputDevData.pFgThreadInfo, dwFlags, dx, dy, 
                dwData, dwExtraInfo, NULL);
        }
        else
        {
            DEVQ_PutMouseEvent(WS_GetThreadInfo(pWinFromPoint->dwThreadId), 
                dwFlags, dx, dy, dwData, dwExtraInfo, WOT_GetHandle((PWSOBJ)pWinFromPoint));
        }
    }
    else //有键按下
    {
//        BOOL bCapture = FALSE, bChange = FALSE;
        DWORD dwFlagsUp;
        /* 如果目标窗口不是当前前景线程，需要切换前景线程，并向新的前景线程
        ** 发消息。
        ** 此时是否应该只转换一下前景线程的状态，而窗口位置、Z序的改变放到
        ** 新的前景线程中去处理。还是说这里就全处理了。
        ** 这需要调试时进一步分析!!!
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
                else// if (dwFlags & MOUSEEVENTF_RIGHTDOWN)
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

    //modify InputDevData.wBtnState修改原始输入的状态
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

    RELEASEMSGQUEOBJ;
    return;
}

/* 管程内部函数 */
static void ChangeFgThread(DWORD dwThreadId, int nActive)
{
    PWINOBJ pOldFocusWin, pNewFocusWin;
    ASSERT (dwThreadId != InputDevData.pFgThreadInfo->dwThreadId);
//    SetNewActiveWindow(pWin, nActive);

    pOldFocusWin = InputDevData.pFgThreadInfo->pFocusWin;
    /* 修改前景线程 */
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

/* 管程内部函数 */
PWINOBJ WND_FromPointEx(int x, int y)
{
    PWINOBJ pWin;

    if (x < 0 || y < 0)
    {

        x = x;
    }
    pWin = WND_FromPoint(x, y);
    
    ASSERT(pWin != NULL);

    return pWin;
}

/* 管程内部函数 */
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
//    ASSERT(ISROOTWIN(pWin->pParent) || (ISROOTWIN(pWin) && pWin->pChild == NULL));
    InputDevData.pFgWin = pWin;

    return TRUE;
}

PWINOBJ INPUTMGR_GetFgWindow(void)
{
    return InputDevData.pFgWin;
}
