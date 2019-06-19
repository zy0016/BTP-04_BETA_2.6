/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setting / keylock setting
 *
 * Purpose  :  
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "CallKeyLockSetting.h"
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

static HWND hWndApp= NULL;
static HWND hFrameWin = NULL;
static char * pClassName = "KeylockSettingClass";
static  LRESULT  AppWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL CreateControl(HWND hWnd, HWND * hKeyLockModeList);

extern BOOL SetKeyLockMode(KEYLOCKMODE iKeylockmode);

BOOL CallKeyLockSetting(HWND hWnd)
{	
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hWnd;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);

    hWndApp = CreateWindow(pClassName,"", 
                WS_VISIBLE | WS_CHILD,  
                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin, 
                NULL, NULL, NULL);

    if (NULL == hWndApp)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");         

    SetWindowText(hFrameWin, ML("Keylock"));
    SetFocus(hWndApp);
    
    return (TRUE);
}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
    static  HWND    hKeylockModeList;
    static  int     iIndex;
    LRESULT lResult;

    lResult = TRUE;   

    switch (wMsgCmd)
	{
		case WM_CREATE :
            CreateControl (hWnd, &hKeylockModeList);
		    hdc         = GetDC(hWnd);
            hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		    ReleaseDC(hWnd,hdc);		
		    
            iIndex = GetKeyLockMode();

		    SendMessage(hKeylockModeList,LB_SETCURSEL,iIndex,0);
		    Load_Icon_SetupList(hKeylockModeList,hIconNormal,hIconSel,2,iIndex);
            
		    break;

		case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_KEYLOCK));
            break;


        case PWM_SHOWWINDOW:
            SetFocus(hWnd);            
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");         
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            
            SetWindowText(hFrameWin, ML("Keylock"));
            break;
        
		case WM_KEYDOWN:
            switch(LOWORD(wParam))
            {       
		        case VK_F10:
                    SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;
                case VK_F5:
                    SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_OK, 0);
                    break;
                default:
                    lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
             }
             break;  
         
		case WM_DESTROY : 
	        DeleteObject(hIconNormal);
	        DeleteObject(hIconSel);
		    hWndApp = NULL;
            UnregisterClass(pClassName,NULL);
            break;

	   case WM_COMMAND :    	    
           switch (LOWORD(wParam))
		    {
                case IDC_BUTTON_CANCEL:
                    DestroyWindow(hWnd);
                    break;      
                case IDC_BUTTON_OK:   
                    iIndex = SendMessage(hKeylockModeList,LB_GETCURSEL,0,0);              
                    //get the current selected item number
		            Load_Icon_SetupList(hKeylockModeList,hIconNormal,hIconSel,2,iIndex);
                         //change the radio button of the current selected item		
                    SetKeyLockMode(iIndex);
				    SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
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

static BOOL CreateControl(HWND hWnd, HWND * hKeyLockModeList)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    
    * hKeyLockModeList = CreateWindow("LISTBOX", 0, 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0,0,rect.right - rect.left, rect.bottom - rect.top,
        hWnd, (HMENU)IDC_KEYLOCK, NULL, NULL);

    if (* hKeyLockModeList == NULL )
        return FALSE;
    SendMessage(* hKeyLockModeList, LB_ADDSTRING, 0, (LPARAM)ML("Manual"));
    SendMessage(* hKeyLockModeList, LB_ADDSTRING, 0, (LPARAM)ML("Automatic"));
    return TRUE;
    
}
