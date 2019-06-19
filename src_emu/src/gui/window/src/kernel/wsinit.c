/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : 实现窗口系统初始化函数和终止函数。
 *            
\**************************************************************************/

#include "hpwin.h"
#include "osal.h"

#include "wsobj.h"

#include "wsownd.h"
#include "wsthread.h"
#include "wsinit.h"
#include "devinput.h"
#include "wscursor.h"


#if (PRINTERSUPPORT)
#include "devprn.h"
#endif

// 窗口系统初始化标志
#ifdef WINDEBUG
BOOL g_bInit;
#endif // WINDEBUG

//Memory routine of window system 
BOOL WINMEM_Init(int type);
void WINMEM_Exit(void);

//Message pool initialization 
BOOL MessagePoolInit(void);
void MessagePoolExit(void);

// System cache DC initialization function, implemented in wsodc.c
BOOL DC_Init(void);

//System semaphore initialization, create the only semaphore
BOOL WS_InitTimerSem(void);
BOOL WS_InitClipboardSem(void);

// Root window initialization function, implemented in winroot.c
BOOL ROOTWIN_Init(void);

// System metrics table initialization function, implemented in sysmetri.c
void WS_InitSystemMetrics(void);


// Messge queue initialize&close function, implemented in message.c
//BOOL WS_CreateMsgQueue(void);
//void WS_DestroyMsgQueue(void);

// Input device driver initialize&close function, implemented in inputmgr.c
#if defined(_DDI_VER)
BOOL INPUTMGR_Init(void);
void INPUTMGR_Exit(void);
#endif

BOOL WS_Init2(void);

/*
**  Function : WS_Init
**  Purpose  :
**      Initializes window system.
*/
BOOL WS_Init(void)
{
    INITMSGQUEOBJ;
//    INITMONITOR;

    ENTERMONITOR;
    if (!WINMEM_Init(1))
    {
        TRACE("Window memory initial failed!!\r\n");
        return FALSE;
    }
    TRACE("Window memory initial success!!\r\n");

    /*********************************************************************/
    /*  Enable font driver                                               */
    /*********************************************************************/
    if (!FONTDEV_Init())
    {
        TRACE("Font device initial failed!!\r\n");
        return FALSE;
    }
    TRACE("Font device initial success!!\r\n");

    if (!GRAPHDEV_Init())
    {
        TRACE("Display device initial failed!!\r\n");
        return FALSE;
    }
    TRACE("Display device initial success!!\r\n");

    /*********************************************************************/
    /*  Get display driver interface and initialize display device       */
    /*********************************************************************/
/*
    if (!DISPDEV_Init())
        return FALSE;
*/

#if (PRINTERSUPPORT)
    /*********************************************************************/
    /*  Get printer driver interface and initialize display device       */
    /*********************************************************************/
/*
    if (!PRNDEV_Init())
        return FALSE;
*/
#endif

    /*********************************************************************/
    /*  Initialize system metrics table, must do after display device    */
    /*  initialization.                                                  */
    /*********************************************************************/
    WS_InitSystemMetrics();
    TRACE("SystemMetrics initial success!!\r\n");

    /*********************************************************************/
    /*  Initialize window object table                                   */
    /*********************************************************************/
    WOT_Init();
    TRACE("WOT initial success!!\r\n");

    /*********************************************************************/
    /*  Initialize window cache DC                                       */
    /*********************************************************************/
    DC_Init();
    TRACE("DC initial success!!\r\n");

    /*********************************************************************/
    /*  Initialize message pool                                          */
    /*********************************************************************/
    if (!MessagePoolInit())
    {
        TRACE("Message pool initial failed!!\r\n");
        return FALSE;
    }
    TRACE("Message pool initial success!!\r\n");


    /*********************************************************************/
    /*  Creates message queue for window system                          */
    /*********************************************************************/
//    if (!WS_CreateMsgQueue())
//        return FALSE;

    /*********************************************************************/
    /*  Initialize root window, cursor etc, which is depended on the     */
    /*  display resolution                                               */
    /*********************************************************************/
    if (!WS_Init2())
    {
        TRACE("Root window initial failed!!\r\n");
        return FALSE;
    }
    TRACE("Root window initial success!!\r\n");

    /*********************************************************************/
    /*  Initialize input device management module                        */
    /*********************************************************************/
    if (!INPUTDEV_Init())
    {
        TRACE("Input device initial failed!!\r\n");
        return FALSE;
    }
    TRACE("Input device initial success!!\r\n");

#ifdef _DDI_VER
    if (!INPUTMGR_Init())
    {
        TRACE("Input manage initial failed!!\r\n");
        return FALSE;
    }
    TRACE("Input manage initial success!!\r\n");
#endif

    /*********************************************************************/
    /*  Set window system initialization flag                            */
    /*********************************************************************/
#ifdef WINDEBUG
    g_bInit = TRUE;
#endif

    WS_InitClipboardSem();
    WS_InitTimerSem();

    LEAVEMONITOR;

    // 设置窗口系统时钟任务
    PLXOS_CreateTimerTask();
    TRACE("Timer task initial success\r\n");

    TRACE("Timer task initial success!!!!!!!!!!!!!\r\n");
    return TRUE;
}

/*
**  Function : WS_Init2
**  Purpose  :
**      进行与显示分辨率相关的初始化，在初始化和显示分辨率改变时调用。
*/
BOOL WS_Init2(void)
{
    /*********************************************************************/
    /*  Register root window class and create root window                */
    /*********************************************************************/
    if (!ROOTWIN_Init())
        return FALSE;
    
    /*********************************************************************/
    /*  Initialize cursor management                                     */
    /*********************************************************************/
    
    // Set the maximum cursor clip rectangle and cursor clip rectangle to
    // whole screen area
    WS_SetMaxCursorRect(NULL);
    
#if (!NOCURSOR)
    // Set current cursor to arrow cursor
    WS_SetCursor(NULL);
    WS_SetCursor((PCURSOROBJ)WOT_GetStockCursor((int)IDC_ARROW));
#endif  // NOCURSOR
    
    // Set cursor position to center point of screen. Must set the cursor
    // before this operation, otherwise the position can't be setted 
    // correctly
//    mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, 
//        DISPDEV_GetCaps(HORZRES) / 2, DISPDEV_GetCaps(VERTRES) / 2, 0, 0);
    
#if (!NOCURSOR)
    // Show current cursor
    WS_ShowCursor(TRUE);
#endif  // NOCURSOR

    return TRUE;
}

/*
**  Function : WS_Exit
**  Purpose  :
**      Window system exit function.
*/
void WS_Exit(void)
{
    // Close input device
    INPUTMGR_Exit();

    // Destroy message queue
//    WS_DestroyMsgQueue();

    // Destroy window object table
    WOT_Exit();

    // Close display device
//    DISPDEV_Exit();
    GRAPHDEV_Exit();
}
