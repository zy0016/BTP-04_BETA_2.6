/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 *    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
 *                       All Rights Reserved
 *
 * Module   :  timeedit.c
 *
 * Purpose  :  Time Edit Control Implementation
 *
\**************************************************************************/

#include "window.h"
#include "pubapp.h"

static WNDPROC g_pfnBaseWndProc = NULL;
static LRESULT CALLBACK TIMEEDIT_WndProc(HWND hWnd, UINT uMsgCmd,
                                         WPARAM wParam, LPARAM lParam);
static void OnCreate(HWND hWnd, LPARAM lParam);

/**********************************************************************
 * Function     TIMEEDIT_RegisterClass
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/

BOOL TIMEEDIT_RegisterClass(void)
{
    WNDCLASS wc;

    GetClassInfo(NULL, "UNITEDIT", &wc);
    g_pfnBaseWndProc = wc.lpfnWndProc;

    wc.lpszClassName = "TIMEEDIT";
    wc.lpfnWndProc   = TIMEEDIT_WndProc;

    return RegisterClass(&wc);
}

/**********************************************************************
 * Function     TIMEEDIT_WndProc
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT CALLBACK TIMEEDIT_WndProc(HWND hWnd, UINT uMsgCmd,
                                         WPARAM wParam, LPARAM lParam)
{
    switch (uMsgCmd)
    {
    case WM_CREATE:
        OnCreate(hWnd, lParam);
        return 0;

    default:
        return DefWindowProc(hWnd, uMsgCmd, wParam, lParam);
    }
}

/**********************************************************************
 * Function     OnCreate
 * Purpose      
 * Params       hWnd
 * Return       
 * Remarks      
 **********************************************************************/

static void OnCreate(HWND hWnd, LPARAM lParam)
{
    LPCREATESTRUCT  lpcs = NULL;
    DWORD           dwStyle = 0;

    lpcs = (LPCREATESTRUCT)lParam;
    dwStyle = LOWORD(lpcs->style);

    if (dwStyle & TES_DATE_YMD || dwStyle & TES_DATE_MDY
        || dwStyle & TES_DATE_DMY)
    {
        lpcs->style |= UES_DATE;
    }
    else
    {
        lpcs->style |= UES_TIME;
    }

    CallWindowProc(g_pfnBaseWndProc, hWnd, WM_CREATE, 0, lParam);
    SetWindowLong(hWnd, GWL_WNDPROC, (LONG)g_pfnBaseWndProc);
}
