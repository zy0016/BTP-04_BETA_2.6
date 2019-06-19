/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setting
 *
 * Purpose  : create the date on setup window
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "CallDateOnOptions.h"
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "str_plx.h"
#include    "me_wnd.h"
#include    "plx_pdaex.h"
#include    "str_public.h"
#include    "setting.h"
#include    "setup.h"
#include    "mullang.h"
#include    "pubapp.h"
/*
#include    "LocApi.h"
#include    "GPSSettingsApi.h"

*/
static  HWND        hwndApp = NULL;
static  HWND        hFrameWin = NULL;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd,HWND * hListContent);
extern  BOOL    SetTimeSource(TIMESOURCE timesource);

/***************************************************************
* Function  CallDateOnOptions
* Purpose   create the main window
* Params
* Return
* Remarks
***************************************************************/
BOOL CallDateOnOptions(HWND hwndCall)
{    
	WNDCLASS    wc;

    RECT rClient;

    hFrameWin = hwndCall;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    GetClientRect(hFrameWin, &rClient);

    hwndApp = CreateWindow(pClassName, NULL,  
                WS_VISIBLE | WS_CHILD,                    
                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin,
                NULL, NULL, NULL);

    if (NULL == hwndApp)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT, (LPARAM)ML("Cancel"));
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SetWindowText(hFrameWin, ML("Time source"));
    SetFocus(hwndApp);

    return (TRUE);
}
/***************************************************************
* Function  
* Purpose    the main window process
* Params
* Return
* Remarks
***************************************************************/
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
    static  HWND    hListContent,hFocus;
	static  int     iIndex,i;
    LRESULT lResult;
//    UTCTIME myUTCTime;
//    SYSTEMTIME st;
    lResult = TRUE;   

    switch ( wMsgCmd )
	{
		case WM_CREATE :
            CreateControl (hWnd, &hListContent);
		    hdc         = GetDC(hWnd);
            hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		    ReleaseDC(hWnd,hdc);
            iIndex = GetTimeSource();
		    Load_Icon_SetupList(hListContent,hIconNormal,hIconSel,2,iIndex);
		    SendMessage(hListContent,LB_SETCURSEL,iIndex,0);
		    break;

		case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_LISTCONTENT));
            break;

        case PWM_SHOWWINDOW:
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT, (LPARAM)ML("Cancel"));
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            SetWindowText(hFrameWin, ML("Time source"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            SetFocus(hWnd);
            break;

		case WM_KEYDOWN:
            switch(LOWORD(wParam))
            {       
		        case VK_F10:
                    SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;
                case VK_F5:
                    SendMessage(hWnd,WM_COMMAND,IDM_OK,0);
                    break;
                default:
                    lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            }
            break;  
         
		case WM_DESTROY : 
	        DeleteObject(hIconNormal);
	        DeleteObject(hIconSel);
		    hwndApp = NULL;
            UnregisterClass(pClassName,NULL);
            break;

	    case WM_COMMAND :
            switch (LOWORD(wParam))
		    {
                case IDM_FILEEXIT:
                    DestroyWindow(hWnd);
                    break;      
                case IDM_OK:
	    	        iIndex = SendMessage(hListContent,LB_GETCURSEL,0,0);              //get the current selected item number
		    	    Load_Icon_SetupList(hListContent,hIconNormal,hIconSel,2,iIndex);     //change the radio button of the current selected item		
                    SetTimeSource(iIndex);
                    SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
                    break;
                default:
                    break;                 
		    }
            break;

        case WM_TIMER:
	        switch(wParam)
		    {
		        case TIMER_ASURE:
		            KillTimer(hWnd, TIMER_ASURE);
                    SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
			        break;
                default:
                    break;
		    }
	    	break;
        default :
            lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
            break;
	}
    return lResult;
}
/***************************************************************
* Function  CreateControl
* Purpose   create the control components of the window         
* Params
* Return    
* Remarks
***************************************************************/
static  BOOL    CreateControl(HWND hWnd,HWND * hListContent)
{
	RECT rect;

	GetClientRect(hWnd, &rect);
    * hListContent = CreateWindow("LISTBOX", 0, 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL/*|CS_NOSYSCTRL*/,   
        rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
        hWnd, (HMENU)IDC_LISTCONTENT, NULL, NULL);
    if (* hListContent == NULL )
        return FALSE;
	SendMessage(* hListContent,LB_ADDSTRING,0,(LPARAM)ML("GPS"));
	SendMessage(* hListContent,LB_ADDSTRING,0,(LPARAM)ML("User defined"));

    return TRUE;
}
