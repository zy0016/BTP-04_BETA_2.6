/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : connectivity setting
 *
 * Purpose  : GSM radio part setting (switch on or off)
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "GSMRadioSetting.h"
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

static char* pClassName = "GSMRadioSettingClass";

static HWND hAppWnd;
static HWND hFrameWin;
static BOOL   CreateControl (HWND hWnd, HWND * hList);

void SetGSMRadioPartMode(SWITCHTYPE OnOrOff);

static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

BOOL    CallGSMRadioSetting(HWND hFatherWnd)
{
	WNDCLASS wc;
    RECT rect;
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

    hFrameWin = hFatherWnd;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rect);
        
    hAppWnd = CreateWindow(pClassName,NULL, 
            WS_VISIBLE |WS_CHILD,  
            rect.left,
            rect.top,
            rect.right-rect.left,
            rect.bottom-rect.top,
            hFatherWnd, NULL, NULL, NULL);
        
        if (NULL == hAppWnd)
        {
            UnregisterClass(pClassName,NULL);
            return FALSE;
        }
    SetWindowText(hFrameWin, ML("GSM radio part"));
            
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
                (WPARAM)IDC_CANCEL,(LPARAM)ML("Cancel") );
    
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");

    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;


    SetFocus ( hAppWnd );	

    return TRUE;
    
}

static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
    static  HWND    hList;
    static  int     iIndex, iDefIndex;
    static int iTmpIndex = 0;
    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
	{
		case WM_CREATE :
            CreateControl (hWnd, &hList);
		    hdc         = GetDC(hWnd);
            hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		    ReleaseDC(hWnd,hdc);
            iDefIndex = GetGSMRadioPartMode();
            Load_Icon_SetupList(hList,hIconNormal,hIconSel,2,iDefIndex);		    
            SendMessage(hList,LB_SETCURSEL,iDefIndex,0);                      
		    break;

        case PWM_SHOWWINDOW:
            SetWindowText(hFrameWin, ML("GSM radio part"));
            SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_CANCEL, (LPARAM)ML("Cancel") );
            SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select"); 
            SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
            SetFocus(hWnd);
            break;
            
        case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_LIST));
            break;

/*
        case WM_ACTIVATE:
            
		    if (WA_INACTIVE != LOWORD(wParam))
                SetFocus(GetDlgItem(hWnd,IDC_LIST));
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            InvalidateRect(hWnd,NULL,TRUE);
            break;
*/
        
		case WM_KEYDOWN:
            switch(LOWORD(wParam))
            {       
		        case VK_F10:
                    SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;
                case VK_F5:
                    SendMessage(hWnd,WM_COMMAND,IDC_OK,0);
                    break;
                default:
                    lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            }
            break;  
         
		case WM_DESTROY : 
	        DeleteObject(hIconNormal);
	        DeleteObject(hIconSel);
		    KillTimer(hWnd, TIMER_ASURE);
		    hAppWnd = NULL;
            UnregisterClass(pClassName,NULL);
            break;

	    case WM_COMMAND :	    	    
            iIndex = SendMessage(hList,LB_GETCURSEL,0,0);              //get the current selected item number           
            switch (LOWORD(wParam))
		    {
                case IDC_CANCEL:
                    DestroyWindow(hWnd);
                    break;      
                case IDC_OK:
		    	    Load_Icon_SetupList(hList,hIconNormal,hIconSel,2,iIndex);     //change the radio button of the current selected item		
                    iTmpIndex = iIndex;
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
                    if(0 == iTmpIndex) //select "On"
                        PLXConfirmWinEx(hFrameWin, hWnd, ML("GSM radio part \n will be switched on. \n Continue?"), Notify_Request, ML("GSM radio part"), ML("Yes"), ML("No"), CALLBACK_SETGSMON);
                    else // select off
                        PLXConfirmWinEx(hFrameWin, hWnd,ML("GSM radio part \n will be switched off. \n Continue?"), Notify_Request, ML("GSM radio part"), ML("Yes"), ML("No"), CALLBACK_SETGSMOFF);
			        break;
                
                default:
                    break;
		    }
	    	break;
        
        case CALLBACK_SETGSMON:
            if(lParam)
            {
                SetGSMRadioPartMode(SWITCH_ON);
                PLXTipsWin(hFrameWin, hWnd, NULL,ML("GSM radio part \n is switched on"), ML("GSM radio part"), Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
                DestroyWindow(hWnd);
                break; // yes, sure
            }
            break;
        case CALLBACK_SETGSMOFF:
            if(lParam)
            {
                SetGSMRadioPartMode(SWITCH_OFF);
                PLXTipsWin(hFrameWin, hWnd, NULL,ML("GSM radio part \n is switched off"), ML("GSM radio part"), Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
                DestroyWindow(hWnd);
                break; // yes, sure
            }
            break;
        
        default :
            lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
            break;
	}
    return lResult;
}

static BOOL   CreateControl (HWND hWnd, HWND * hList)
{
	RECT rect;
	GetClientRect(hWnd, &rect);
    * hList = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
		rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
        hWnd, (HMENU)IDC_LIST, NULL, NULL);
    if (* hList == NULL )
        return FALSE;

    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("On"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Off"));

    return TRUE;
}


SWITCHTYPE GetGSMRadioPartMode(void)
{
    return FS_GetPrivateProfileInt(SN_CONNECTIVITY, KN_GSMRADIO, SWITCH_ON, SETUPFILENAME);
}

void SetGSMRadioPartMode(SWITCHTYPE OnOrOff)
{
    FS_WritePrivateProfileInt(SN_CONNECTIVITY, KN_GSMRADIO, OnOrOff, SETUPFILENAME);
}
