/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : sound profile
 *
 * Purpose  : Group alert setting
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "CallAlertGrp.h"
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "plx_pdaex.h"
#include	"fcntl.h"
#include    "setting.h"
#include    "pubapp.h"
#include    "setup.h"
#include    "mullang.h"

static   int   iCurMode;
static   HWND   hWndApp = NULL;
static   HWND   hFrameWin = NULL;
static   SCENEMODE   sm;
static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL CreateControl(HWND hwnd, HWND * hItemList);
extern  BOOL CallSelAlertGrp(HWND hFrame, HWND hFather, int iIndex);

BOOL     CallAlertGrp(HWND hcallApp,int icurmode)
{

	WNDCLASS wc;
    RECT rClient;

    hFrameWin = hcallApp;
    iCurMode = icurmode;

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
        
    GetSM(&sm,iCurMode);

    GetClientRect(hFrameWin, &rClient);
    hWndApp = CreateWindow(pClassName,NULL, 
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
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));        
    
    SetWindowText(hFrameWin, ML("Alerting groups"));
    SetFocus(hWndApp);
    return TRUE;

}

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;
    static      int iIndex;
	HDC             hdc;
    LRESULT     lResult;
    static      HWND        hItemList;
    lResult = TRUE;   

    switch ( wMsgCmd )
    {
    case WM_CREATE:
        CreateControl(hWnd,&hItemList);
		hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);		
        SendMessage(hItemList,LB_SETCURSEL,(WPARAM)sm.iAlertFor,0);
        Load_Icon_SetupList(hItemList,hIconNormal,hIconSel,3,(LPARAM)sm.iAlertFor);

        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_ITEMLIST));
        break;

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));        
        
        SetWindowText(hFrameWin, ML("Alerting groups"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        break;
        
    case WM_KEYDOWN:
        switch(LOWORD(wParam)) 
        {
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        	break;
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_OK,0);
        	break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_BUTTON_OK:
            iIndex = SendMessage(hItemList,LB_GETCURSEL,0,0);
		    Load_Icon_SetupList(hItemList,hIconNormal,hIconSel,3,iIndex);     //change the radio button of the current selected item		
            SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
            break;

        }
        break;
    case WM_DESTROY:
         hWndApp = NULL;
	     DeleteObject(hIconNormal);
	     DeleteObject(hIconSel);
         UnregisterClass(pClassName,NULL);
         break;
   
    case WM_TIMER:
	    switch(wParam)
		{
		case TIMER_ASURE:
            if(iIndex == 1)//selected
            {
                KillTimer(hWnd, TIMER_ASURE);
                CallSelAlertGrp(hFrameWin, hWnd, iCurMode);
            }
            else 
            {
                if(iIndex == 0)
                {
                    KillTimer(hWnd, TIMER_ASURE);
                    sm.iAlertFor = ALERTALL;
					memset(&sm.iAlertGrpID, 0, sizeof(SETT_MAX_ABGRP_NUM));
                }
                if(iIndex == 2)
                {
                    KillTimer(hWnd, TIMER_ASURE);
                    sm.iAlertFor = ALERTNO;
					memset(&sm.iAlertGrpID, 0, sizeof(SETT_MAX_ABGRP_NUM));
                }
                SetSM(&sm, iCurMode);
                SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
            break;
		}
		break;
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;    
    }
    return lResult;
}

static BOOL CreateControl(HWND hwnd, HWND * hItemList)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
    * hItemList = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY,   
        rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
        hwnd, (HMENU)IDC_ITEMLIST, NULL, NULL);
    if (* hItemList == NULL )
        return FALSE;
    SendMessage(* hItemList, LB_RESETCONTENT, 0, 0);

    SendMessage(* hItemList, LB_ADDSTRING, 0, (LPARAM)ML("All calls"));
    SendMessage(* hItemList, LB_ADDSTRING, 1, (LPARAM)ML("Selected"));
    SendMessage(* hItemList, LB_ADDSTRING, 2, (LPARAM)ML("None"));

   return TRUE;
}
