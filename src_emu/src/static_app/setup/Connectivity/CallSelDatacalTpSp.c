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
#include    "CallSelDatacalTpSp.h"
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

static  HWND hwndApp = NULL;
static  HWND hspdwndApp = NULL;
static  HWND hFrameWin = NULL;

static int iCurMode;
static int iSpdCurMode;
static UDB_ISPINFO Global_IspInfo;
static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT SpdAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd,HWND * hList);
static  BOOL    SPDCreateControl(HWND hWnd,HWND * hList);

extern BOOL IspSaveInfo (UDB_ISPINFO * uIspInfo ,int iNum);
extern BOOL IspReadInfo (UDB_ISPINFO * uIspInfo ,int iNum);


BOOL CallSelConnDatacalTp(HWND hwndCall, int icurMode)
{    
	WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hwndCall;
    iCurMode = icurMode;
    
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

    IspReadInfo(&Global_IspInfo, iCurMode);

    GetClientRect(hFrameWin, &rClient);

    hwndApp = CreateWindow(pClassName,
        NULL, 
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
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT, (LPARAM)ML("Cancel"));
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");      

    SetWindowText(hFrameWin, ML("Data call type"));
    SetFocus(hwndApp);


    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
	static  int     iIndex=0,i;
    static  HWND    hList,hFocus;
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
        
        iIndex = Global_IspInfo.DatacallType;

        SendMessage(hList,LB_SETCURSEL,iIndex,0);
        Load_Icon_SetupList(hList,hIconNormal,hIconSel,3,iIndex);
        break;
        
    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Cancel"));
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");   
        SetWindowText(hFrameWin, ML("Data call type"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
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
            SendMessage(hWnd,WM_COMMAND,IDM_OK,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;  
        
        case WM_DESTROY : 
            DeleteObject(hIconNormal);
            DeleteObject(hIconSel);
            KillTimer(hWnd, TIMER_ASURE);
            hwndApp = NULL;
            UnregisterClass(pClassName,NULL);
            break;
            
        case WM_COMMAND :
            switch (LOWORD(wParam))
            {
            case IDM_OK:   
                iIndex = SendMessage(hList,LB_GETCURSEL,0,0);//get the current selected item number
                Load_Icon_SetupList(hList,hIconNormal,hIconSel,3,iIndex);//change the radio button of the current selected item		
                Global_IspInfo.DatacallType = iIndex;   

                IspSaveInfo(&Global_IspInfo, iCurMode);
                SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
                break;
            }
            break;
            
        case WM_TIMER:
            switch(wParam)
            {
            case TIMER_ASURE:
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

static  BOOL    CreateControl(HWND hWnd,HWND * hList)
{
    RECT rect;

    GetClientRect(hWnd, &rect);

    * hList = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0,0,rect.right - rect.left, rect.bottom - rect.top,
        hWnd, (HMENU)IDC_LIST, NULL, NULL);
    if (* hList == NULL )
        return FALSE;

	SendMessage(* hList,LB_ADDSTRING,0,(LPARAM)ML("Analogue"));
	SendMessage(* hList,LB_ADDSTRING,0,(LPARAM)ML("ISDN v.110"));
	SendMessage(* hList,LB_ADDSTRING,0,(LPARAM)ML("ISDN v.120"));

   return TRUE;
}

BOOL CallSelConnDatacalSpd(HWND hwndCall, int icurMode)
{    
	WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hwndCall;
    iSpdCurMode = icurMode;
    
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = SpdAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pSpdClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    IspReadInfo(&Global_IspInfo, iSpdCurMode);

    GetClientRect(hFrameWin, &rClient);

    hspdwndApp = CreateWindow(pSpdClassName,
        NULL, 
        WS_VISIBLE | WS_CHILD,                    
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin,
        NULL, NULL, NULL);

    if (NULL == hspdwndApp)
    {
        UnregisterClass(pSpdClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT, (LPARAM)ML("Cancel"));
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");      

    SetWindowText(hFrameWin, ML("Max. data speed"));
    SetFocus(hspdwndApp);


    return (TRUE);
}
static LRESULT SpdAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
	static  int     iIndex=0,i;
    static  HWND    hList,hFocus;
    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
    {
    case WM_CREATE :
        SPDCreateControl (hWnd, &hList);
        
        hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        ReleaseDC(hWnd,hdc);
        
        iIndex = Global_IspInfo.MaxDataSpeed;

        SendMessage(hList,LB_SETCURSEL,iIndex,0);
        Load_Icon_SetupList(hList,hIconNormal,hIconSel,3,iIndex);
        break;
        
    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Cancel"));
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");   
        SetWindowText(hFrameWin, ML("Max. data speed"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        break;
        
    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_SPDLIST));
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
            break;
        }
        break;  
        
        case WM_DESTROY : 
            DeleteObject(hIconNormal);
            DeleteObject(hIconSel);
            KillTimer(hWnd, TIMER_ASURE);
            hspdwndApp = NULL;
            UnregisterClass(pSpdClassName,NULL);
            break;
            
        case WM_COMMAND :
            switch (LOWORD(wParam))
            {
            case IDM_OK:   
                iIndex = SendMessage(hList,LB_GETCURSEL,0,0);//get the current selected item number
                Load_Icon_SetupList(hList,hIconNormal,hIconSel,3,iIndex);//change the radio button of the current selected item		
                Global_IspInfo.MaxDataSpeed = iIndex;   

                IspSaveInfo(&Global_IspInfo, iSpdCurMode);
                SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
                break;
            }
            break;
            
        case WM_TIMER:
            switch(wParam)
            {
            case TIMER_ASURE:
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

static  BOOL    SPDCreateControl(HWND hWnd,HWND * hList)
{
    RECT rect;

    GetClientRect(hWnd, &rect);

    * hList = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0,0,rect.right - rect.left, rect.bottom - rect.top,
        hWnd, (HMENU)IDC_SPDLIST, NULL, NULL);
    if (* hList == NULL )
        return FALSE;

	SendMessage(* hList,LB_ADDSTRING,0,(LPARAM)ML("Automatic"));
	SendMessage(* hList,LB_ADDSTRING,0,(LPARAM)ML("9600 bps"));
	SendMessage(* hList,LB_ADDSTRING,0,(LPARAM)ML("14400 bps"));

   return TRUE;
}
