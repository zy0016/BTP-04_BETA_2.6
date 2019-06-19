/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Do the basic routine of default window.
 *            
\**************************************************************************/

#include "hpwin.h"

/*
**  Function : DEFWND_ProcBase
**  Purpose  :
**      Window proc for default kernel window.
*/
LRESULT DEFWND_ProcBase(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_PAINT:

        BeginPaint(hWnd, NULL);
        EndPaint(hWnd, NULL);

        break;

    default:

        break;
    }

    return TRUE;
}

#if (BASEWINDOW)
/*
**  Function : RegisterDefWindowClass
**  Purpose  :
**      Registers default window class.
*/
BOOL DEFWND_RegisterClass(void)
{
    WNDCLASS wc;

    wc.style            = 0;
    wc.lpfnWndProc      = DEFWND_ProcBase;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = DEFWINDOWCLASSNAME;

    if (!RegisterClassBase(&wc))
        return FALSE;

    return TRUE;
}
#endif

/*
* Function	: DefWindowProcBase   
* Purpose   :
        The basic DefWindowProc routine of the window kernel  
* Params	   
* Return	 	   
* Remarks	   
*/
LRESULT WINAPI DefWindowProcBase(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                             LPARAM lParam)
{
    return DEFWND_ProcBase(hWnd, wMsgCmd, wParam, lParam);
}
