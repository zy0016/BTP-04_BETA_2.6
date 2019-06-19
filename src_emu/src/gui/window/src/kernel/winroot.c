/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements root window mangament functions.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "string.h"

#include "wsobj.h"
#include "wsownd.h"
#include "clstbl.h"
#include "wsgraph.h"
#include "wscursor.h"

#include "wsthread.h"
static CONST char achRootWindowClassName[] = "WS_ROOTWINDOWCLASS";

// Define root window
PWINOBJ g_pRootWin;

// Internal function prototypes
static LRESULT DefaultRootWindowProc(HWND hWnd, UINT wMsgCmd, 
                                     WPARAM wParam, LPARAM lParam);

/*
**  Function : ROOTWIN_Init
**  Purpsoe  :
**      Creates the root window. This function must be called in WSO_Init
**      function.
*/
BOOL ROOTWIN_Init(void)
{
    WNDCLASS wc;
    PCLSOBJ pClsObj;
    PBRUSHOBJ pWhiteBrush;
    int nScrW, nScrH;
    PTHREADINFO pThreadInfo;

    if (!g_pRootWin)
    {
        pWhiteBrush = (PBRUSHOBJ)WOT_GetStockObj(WHITE_BRUSH);
        
        // Register the root window class
        
        wc.style            = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc      = DefaultRootWindowProc;
        wc.cbClsExtra       = 0;
        wc.cbWndExtra       = 0;
        wc.hInstance        = NULL;
        wc.hIcon            = NULL;
        wc.hCursor          = NULL;
        wc.hbrBackground    = (HBRUSH)WOT_GetHandle((PGDIOBJ)pWhiteBrush);
        wc.lpszMenuName     = NULL;
        wc.lpszClassName    = achRootWindowClassName;
        
        pClsObj = CLSTBL_Insert(&wc);
        
        if (!pClsObj)
            return FALSE;
        
        g_pRootWin = (PWINOBJ)MemAlloc(sizeof(WINOBJ) + wc.cbWndExtra);
        
        if (!g_pRootWin)
            return FALSE;
        
        memset(g_pRootWin, 0, sizeof(WINOBJ) + wc.cbWndExtra);

        pThreadInfo = WS_GetCurrentThreadInfo();
        if (pThreadInfo == NULL)
        {
            pThreadInfo = THREADINFO_Create();
            if (pThreadInfo == NULL)
            {
                SetLastError(1);
                return FALSE;
            }
        }

        g_pRootWin->dwThreadId = WS_GetCurrentThreadId();
        
        WOT_RegisterObj((PWSOBJ)g_pRootWin, OBJ_WINDOW, FALSE);
//        g_pRootWin->header.objhead.handle = (HANDLE)HWND_ROOT;
        g_pRootWin->pWndClass = &pClsObj->wc;
        g_pRootWin->pfnWndProc = DefaultRootWindowProc;
        g_pRootWin->dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;
        g_pRootWin->dwExStyle = 0;

        g_pRootWin->rcWindow.right = 0;
        g_pRootWin->rcWindow.bottom = 0;
        g_pRootWin->rcWindow.right = DISPDEV_GetCaps(HORZRES);
        g_pRootWin->rcWindow.bottom = DISPDEV_GetCaps(VERTRES);

        g_pRootWin->rcClient = g_pRootWin->rcWindow;
        
        g_pRootWin->pDC = DC_CreateWindowDC(g_pRootWin, TRUE);
        
        ASSERT(g_pRootWin->pDC != NULL);
        
        WND_Map(g_pRootWin);
    }
    else
    {
        nScrW = DISPDEV_GetCaps(HORZRES);
        nScrH = DISPDEV_GetCaps(VERTRES);

        // Moves the root window to new size if the resolution changed
        if ((g_pRootWin->rcWindow.right != nScrW) || 
            (g_pRootWin->rcWindow.bottom != nScrH))
            WND_Move(g_pRootWin, 0, 0, nScrW, nScrH, TRUE);
    }

    return TRUE;
}

/*
**  Function : DefaultRootWindowProc
**  Purpose  :
**      Implements root Window window proc.
*/
static LRESULT DefaultRootWindowProc(HWND hWnd, UINT wMsgCmd, 
                                     WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult;
    PDC pDC;
    PAINTSTRUCT ps;

    lResult = 0;

    switch ( wMsgCmd )
    {
    case WM_PAINT :

        ENTERMONITOR;

        pDC = WND_BeginPaint(g_pRootWin, &ps);
        //WS_ClearRect(pDC, &ps.rcPaint, RGB(255, 255, 255));
        WND_EndPaint(g_pRootWin, &ps);

        LEAVEMONITOR;

        break;

    case WM_NCHITTEST :

        lResult = HTCLIENT;
        break;

    case WM_SETCURSOR :

        if ((HWND)wParam != hWnd)
            return FALSE;

        SetCursor(LoadCursor(NULL, IDC_ARROW));
        lResult = TRUE;

        break;

    default :

        break;
    }

    return lResult;
}
