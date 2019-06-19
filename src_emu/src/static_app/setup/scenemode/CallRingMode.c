 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : sound profile
 *
 * Purpose  : editing the mode of ring
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
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
#include	"imesys.h"
#include    "CallRingMode.h"

static HWND hWndApp = NULL;
static HWND hFrameWin = NULL;
static RINGFOR iRingFor;
static int  iCurMode;
static SCENEMODE sm;
static  LRESULT  AppWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  LRESULT  HeadPhoneAdjWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

/*******************************************************************/
static BOOL OnCreate(HWND hWnd, HWND* hControl, int nInputLev);
static void OnKeyDown(HWND hWnd, UINT wMsgCmd,WPARAM wParam, LPARAM lParam);
static void	OnDestroy(HWND hWnd);
static void ProcessKeyLeftRight(HWND hWnd, WPARAM wParam, LPARAM lParam);
void IntToString ( int nValue, char *buf );

void ReadRingVolume(unsigned long * ivalue);
void SetRingVolume(int ivalue);

/*******************************************************************/
BOOL CallRingMode(HWND hWnd, RINGFOR iringfor, int icurmode)
{
    WNDCLASS    wc;
    RECT rClient;

    iRingFor = iringfor;
    iCurMode = icurmode;
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
    
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel")); //RSK
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");         
    
    SetWindowText(hFrameWin, ML("Mode"));
    SetFocus(hWndApp);


    return (TRUE);
    
}
static  LRESULT  AppWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
	static  int     iIndex;
    static  HWND    hModeList;
    RECT  rect;
    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
	{
    case WM_CREATE :
        GetClientRect(hWnd, &rect);
        GetSM(&sm, iCurMode);
        
        hModeList = CreateWindow("LISTBOX", 0, 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
            0,0,rect.right - rect.left, rect.bottom - rect.top,
            hWnd, (HMENU)IDC_MODELIST, NULL, NULL);
        SendMessage(hModeList, LB_ADDSTRING, 0, (LPARAM)ML("Normal"));
        SendMessage(hModeList, LB_ADDSTRING, 0, (LPARAM)ML("Ascending"));
        SendMessage(hModeList, LB_ADDSTRING, 0, (LPARAM)ML("Beep first"));
        
        hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        ReleaseDC(hWnd,hdc);
        
        switch(iRingFor)
        {
        case INCOMCALL:
            iIndex = sm.rIncomeCall.iRingType;
            break;
        case ALARMCLK:
            iIndex = sm.rAlarmClk.iRingType;
            break;
        case CALENDAR:
            iIndex = sm.rCalendar.iRingType;
            break;
        }
        
        
        SendMessage(hModeList,LB_SETCURSEL,iIndex,0);
        Load_Icon_SetupList(hModeList, hIconNormal,hIconSel,3,iIndex);
        break;
        
    case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_MODELIST));
            break;
            
    case PWM_SHOWWINDOW:
            SetFocus(hWnd);
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel")); //RSK
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");         
            
            SetWindowText(hFrameWin, ML("Mode"));            
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
            SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_OK, 0);
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
            iIndex = SendMessage(hModeList,LB_GETCURSEL,0,0);              //get the current selected item number
            Load_Icon_SetupList(hModeList,hIconNormal,hIconSel,3,iIndex);     //change the radio button of the current selected item		
            switch(iRingFor)
            {
            case INCOMCALL:
                sm.rIncomeCall.iRingType = iIndex;
                break;
            case ALARMCLK:
                sm.rAlarmClk.iRingType = iIndex;
                break;
            case CALENDAR:
                sm.rCalendar.iRingType = iIndex;
                break;
            }
            SetSM(&sm, iCurMode);
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

BOOL CallheadphoneWindow(HWND hFrame, int i)
{
	return TRUE;
}
/*
    WNDCLASS    wc;
	HWND hWnd;
    
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = HeadPhoneAdjWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = HeadPhoneAdjClass;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    hWnd = CreateWindow(HeadPhoneAdjClass,ML("Volume"), 
        PWS_STATICBAR | WS_CAPTION,          
		PLX_WIN_POSITION, 
		NULL, 
        NULL, NULL, NULL);

    if (NULL == hWnd)
    {
        UnregisterClass(HeadPhoneAdjClass,NULL);
        return FALSE;
    }
    
    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel")); //RSK
    SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Save"));         
    return (TRUE);
*/

/*
static  LRESULT  HeadPhoneAdjWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT     lResult;   
	HWND hControl;
	int nLev = 0;
    lResult = TRUE;
    switch(wMsgCmd)
    {
    case WM_CREATE:
        OnCreate(hWnd, &hControl, nLev);
		SetFocus(hControl);
    	break;

    case WM_KEYDOWN:
        OnKeyDown(hWnd,wMsgCmd,wParam,lParam);
        break;

    case WM_DESTROY:
		OnDestroy(hWnd);
        break;

    default:
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    }
    return lResult;	
}

static BOOL OnCreate(HWND hWnd, HWND* hControl, int nInputLev)
{
	RECT rc;
	char cLev[1];

	IntToString(nInputLev, cLev);
	GetClientRect(hWnd, &rc);
	
	*hControl = CreateWindow("LEVADJ", cLev, 
        WS_VISIBLE | WS_CHILD | CS_NOSYSCTRL,
        rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, 
        hWnd, (HMENU)IDC_LEVADJ, NULL, (PVOID)LAS_VOL);

    if (*hControl == NULL)
        return FALSE;

	return TRUE;
}
static void OnKeyDown(HWND hWnd, UINT wMsgCmd,WPARAM wParam, LPARAM lParam)
{}
static void	OnDestroy(HWND hWnd)
{}*/

