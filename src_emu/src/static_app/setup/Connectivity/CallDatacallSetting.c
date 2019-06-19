/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : connectivity setting
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
#include    "CallDatacallSetting.h"
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

static char* pClassName = "DatacallSettingClass";
static char* pAutoDisconClassName = "AutoDisconSettingClass";
static HWND hAppWnd = NULL;
static HWND hAutoDisconAppWnd = NULL;
static HWND hFrameWin = NULL;
static int iIndex_datacall = 0;
static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  AutoDisconAppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

static BOOL CreateControl(HWND hWnd, HWND * hAutoDiscon);
static BOOL AutoDisconCreateControl (HWND hWnd, HWND *hList);
static BOOL    CallAutoDisconList(HWND hWnd);

static int GetAutoDisconnCSD(void);
static void SetAutoDisconnCSD(int index);


BOOL    CallDatacallSetting(HWND hWnd)
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

    hFrameWin = hWnd;
    
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
        
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_BACK, (LPARAM)ML("Back"));//RSK
    SetWindowText(hFrameWin, ML("Data call"));

    GetClientRect(hFrameWin, &rect);

    hAppWnd = CreateWindow(pClassName,NULL, 
        WS_VISIBLE| WS_CHILD,  
        rect.left,
        rect.top,
        rect.right-rect.left,
        rect.bottom-rect.top,
        hFrameWin, NULL, NULL, NULL);    

    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    SetFocus ( hAppWnd );	
    
    return TRUE;
    
}

static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hAutoDiscon;
    static HWND hFocus;
    static int  iButtonJust, iCurIndex;
    
    LRESULT     lResult;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:
        CreateControl(hWnd, &hAutoDiscon);
    	hFocus = hAutoDiscon;
        iIndex_datacall = GetAutoDisconnCSD();
        SendMessage(hAutoDiscon, SSBM_SETCURSEL, iIndex_datacall, 0);
        break;
        
    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_BACK, (LPARAM)ML("Back"));//RSK
        SetWindowText(hFrameWin, ML("Data call"));
        iIndex_datacall = GetAutoDisconnCSD();
        SendMessage(hAutoDiscon, SSBM_SETCURSEL, iIndex_datacall, 0);
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
    	break;
 
    case WM_DESTROY:
        hAppWnd = NULL;
        UnregisterClass(pClassName,NULL);             
        break;

    case WM_KEYDOWN:
		switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_F5:
            SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_SEL, 0);
            break;
		default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
        }
        break;
    case WM_COMMAND:

        switch(LOWORD( wParam )) 
        {
        case IDC_BUTTON_SEL:
            switch(iButtonJust)
            {
            case IDC_AUTODISCON:
                CallAutoDisconList(hFrameWin);
                break;
            }
            break;
        case IDC_AUTODISCON:
            if(HIWORD( wParam ) == SSBN_CHANGE)
            {
                iIndex_datacall = SendMessage(hAutoDiscon,SSBM_GETCURSEL,0,0);
                SetAutoDisconnCSD(iIndex_datacall);
            }
            if(HIWORD(wParam) == SSBN_SETFOCUS)
            {
                iButtonJust = IDC_AUTODISCON;
                hFocus = hAutoDiscon;
            }
            break;
        }
        break;
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static BOOL CreateControl(HWND hWnd, HWND * hAutoDiscon)
{
    int iControlH,iControlW;
	int xzero=0,yzero=0;

	RECT    rect;
    GetClientRect(hWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;
    
    * hAutoDiscon = CreateWindow( "SPINBOXEX", ML("Auto disconnect"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT,
        xzero, yzero, iControlW, iControlH, 
        hWnd, (HMENU)IDC_AUTODISCON, NULL, NULL);

    if (* hAutoDiscon == NULL)
        return FALSE;  
    SendMessage(* hAutoDiscon, SSBM_ADDSTRING, 0, (LPARAM)ML("Off"));
    SendMessage(* hAutoDiscon, SSBM_ADDSTRING, 0, (LPARAM)ML("1 min"));
    SendMessage(* hAutoDiscon, SSBM_ADDSTRING, 0, (LPARAM)ML("2 min"));
    SendMessage(* hAutoDiscon, SSBM_ADDSTRING, 0, (LPARAM)ML("5 min"));
    SendMessage(* hAutoDiscon, SSBM_ADDSTRING, 0, (LPARAM)ML("10 min"));
    SendMessage(* hAutoDiscon, SSBM_ADDSTRING, 0, (LPARAM)ML("20 min"));
    SendMessage(* hAutoDiscon, SSBM_ADDSTRING, 0, (LPARAM)ML("30 min"));
    return TRUE;
    
}

static BOOL    CallAutoDisconList(HWND hWnd)
{
	WNDCLASS wc;
    RECT rect;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AutoDisconAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pAutoDisconClassName;

    hFrameWin = hWnd;
    
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
        
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_BACK, (LPARAM)ML("Cancel"));//RSK
    SetWindowText(hFrameWin, ML("Auto disconnect"));

    GetClientRect(hFrameWin, &rect);

    hAutoDisconAppWnd = CreateWindow(pAutoDisconClassName,NULL, 
        WS_VISIBLE| WS_CHILD,  
        rect.left,
        rect.top,
        rect.right-rect.left,
        rect.bottom-rect.top,
        hFrameWin, NULL, NULL, NULL);    

    if (NULL == hAutoDisconAppWnd)
    {
        UnregisterClass(pAutoDisconClassName,NULL);
        return FALSE;
    }
    SetFocus ( hAutoDisconAppWnd );	
    
    return TRUE;

}
static LRESULT  AutoDisconAppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;

	HDC                 hdc;

	static  int     iIndex=0;
    static  HWND    hList;
    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
    {

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_BACK, (LPARAM)ML("Cancel"));//RSK
        SetWindowText(hFrameWin, ML("Auto disconnect"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        break;
   
    
    case WM_CREATE :
        AutoDisconCreateControl (hWnd, &hList);

		hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);

        iIndex = iIndex_datacall;

		Load_Icon_SetupList(hList,hIconNormal,hIconSel,7,iIndex);
		SendMessage(hList,LB_SETCURSEL,iIndex,0);
  
		break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_LIST));
        break;
        
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
       
		case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SEL,0);
             break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;  

   case WM_COMMAND :
       switch (LOWORD(wParam))
        {
        case IDC_BUTTON_SEL:   
             iIndex = SendMessage(hList,LB_GETCURSEL,0,0);              //get the current selected item number
			 Load_Icon_SetupList(hList,hIconNormal,hIconSel,7,iIndex);     //change the radio button of the current selected item		
	    	 iIndex_datacall = iIndex;
             SetAutoDisconnCSD(iIndex_datacall);
             SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
             break;
        }
        break;

    case WM_DESTROY : //通知程序管理器此程序退出;
        DeleteObject(hIconNormal);
        DeleteObject(hIconSel);
        hAutoDisconAppWnd = NULL;
        UnregisterClass(pAutoDisconClassName,NULL);
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
	case WM_TIMER:
	    switch(wParam)
		{
		case TIMER_ASURE:
            KillTimer(hWnd, TIMER_ASURE);
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    } 
    return lResult;
}
static BOOL AutoDisconCreateControl (HWND hWnd, HWND *hList)
{

    RECT rect;
    GetClientRect(hWnd, &rect);

    * hList = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0,0,rect.right - rect.left,rect.bottom - rect.top,
        hWnd, (HMENU)IDC_LIST, NULL, NULL);
    if (* hList == NULL )
        return FALSE;
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Off"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("1 min"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("2 min"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("5 min"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("10 min"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("20 min"));
     SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("30 min"));
   return TRUE;
}

static void SetAutoDisconnCSD(int index)
{
    FS_WritePrivateProfileInt(SN_CSDSETT, KN_DISCONNTIME, index, SETUPFILENAME);
}
static int GetAutoDisconnCSD(void)
{
    return  FS_GetPrivateProfileInt(SN_CSDSETT, KN_DISCONNTIME, 0, SETUPFILENAME);
}

int Sett_GetDataCallAutoDisconn(void)	//return how long (minute) the data call disconnect.
{
	int nTime;
	nTime = GetAutoDisconnCSD();
	switch(nTime)
	{
	case 0:
		return 0;
	case 1:
		return 1;
	case 2:
		return 2;
	case 3:
		return 5;
	case 4:
		return 10;
	case 5:
		return 20;
	case 6:
		return 30;
	default:
		return 0;
	}
}
