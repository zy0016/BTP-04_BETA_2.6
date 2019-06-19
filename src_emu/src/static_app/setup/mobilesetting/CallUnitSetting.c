 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setting
 *
 * Purpose  : Unit setting
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "CallUnitSetting.h"
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


static HWND hWndApp = NULL;
static HWND hFrameWin = NULL;
static HWND hUnitListWndApp;
static char * pClassName = "UnitsSettingClass";
static char * pUnitListClassName = "UnitsListSettingClass";

static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static UNIT_TYPE UnitType;

static BOOL CreateControl(HWND hWnd, HWND * hDistance, HWND * hSpeed, HWND * hElevation);
static void LoadUnitList(HWND * hList, UNIT_TYPE unittype);
static BOOL UnitListCreateControl(HWND hWnd, HWND * hList);
static BOOL CallUnitListWnd(HWND hFatherWnd, UNIT_TYPE unittype);
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT UnitListAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);
void SetUnitDistance(UNIT_DISTANCE DisUnit);
void SetUnitSpeed(UNIT_SPEED SpeedUnit);
void SetUnitElevation(UNIT_ELEVATION EleUnit);


BOOL CallUnitSetting(HWND hFatherWnd)
{
    WNDCLASS    wc;
    RECT rClient;
    
    hFrameWin = hFatherWnd;
    
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
        hFrameWin, NULL, NULL, NULL);
    
    if (NULL == hWndApp)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");    
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK,(LPARAM)ML("Back") ); 
        
    SetWindowText(hFrameWin,ML("Units"));   
    
    SetFocus(hWndApp);
    
    return (TRUE);
    
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static      HWND        hDistance, hSpeed, hElevation;
    static      HWND        hFocus = 0;
    static      int         iCurSel,iButtonJust;
    LRESULT     lResult;
    
    lResult = (LRESULT)TRUE;
    
    switch ( wMsgCmd )
    {   
    case PWM_SHOWWINDOW:  
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");    
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK,(LPARAM)ML("Back") ); 
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        
        SetWindowText(hFrameWin,ML("Units"));   
        SendMessage(hDistance, SSBM_SETCURSEL, GetUnitDistance(), 0);
        SendMessage(hSpeed, SSBM_SETCURSEL, GetUnitSpeed(), 0);
        SendMessage(hElevation, SSBM_SETCURSEL, GetUnitElevation(), 0);
        break;
        
    case WM_CREATE :
        CreateControl(hWnd, &hDistance, &hSpeed, &hElevation);
        SendMessage(hDistance, SSBM_ADDSTRING, 0, (LPARAM)ML("Meters"));
        SendMessage(hDistance, SSBM_ADDSTRING, 0, (LPARAM)ML("Imperial miles"));
        SendMessage(hDistance, SSBM_ADDSTRING, 0, (LPARAM)ML("Imperial yards"));
        SendMessage(hDistance, SSBM_ADDSTRING, 0, (LPARAM)ML("Nautical miles"));
        
        SendMessage(hSpeed, SSBM_ADDSTRING, 0, (LPARAM)ML("Kilometersslhour"));
        SendMessage(hSpeed, SSBM_ADDSTRING, 0, (LPARAM)ML("Metersslsecond"));
        SendMessage(hSpeed, SSBM_ADDSTRING, 0, (LPARAM)ML("Milesslhour"));
        SendMessage(hSpeed, SSBM_ADDSTRING, 0, (LPARAM)ML("Knots"));
        
        SendMessage(hElevation, SSBM_ADDSTRING, 0, (LPARAM)ML("Meters"));
        SendMessage(hElevation, SSBM_ADDSTRING, 0, (LPARAM)ML("Imperial feet"));
        
        SendMessage(hDistance, SSBM_SETCURSEL, GetUnitDistance(), 0);
        SendMessage(hSpeed, SSBM_SETCURSEL, GetUnitSpeed(), 0);
        SendMessage(hElevation, SSBM_SETCURSEL, GetUnitElevation(), 0);       
        
        hFocus=hDistance;
        break;
        
        
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;
        
    case WM_DESTROY : 
        hWndApp = NULL;
        UnregisterClass(pClassName,NULL);
        break;
	case PWM_ACTIVATE:
		if (wParam == WA_INACTIVE)
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
			wKeyCode   = 0;
		}
		break;
        
    case WM_KEYDOWN:
		if (RepeatFlag > 0 && wKeyCode != LOWORD(wParam))
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
		}
		
		wKeyCode = LOWORD(wParam);
		RepeatFlag++;
        
		switch(LOWORD(wParam))
        {            
        case VK_F10:           
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        case VK_DOWN:
            SettListProcessKeyDown(hWnd, &hFocus);            
			if ( 1 == RepeatFlag )
			{
	    		SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
			}
            break;
            
        case VK_UP:
            SettListProcessKeyUp(hWnd, &hFocus);            
			if ( 1 == RepeatFlag )
			{
	    		SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
			}
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;  
    
	case WM_TIMER:
	    switch(wParam)
		{
		case TIMER_ID:
            if (1 == RepeatFlag)
            {
                KillTimer(hWnd, TIMER_ID);
                SetTimer(hWnd, TIMER_ID, ET_REPEAT_LATER, NULL);
            }
			keybd_event(wKeyCode, 0, 0, 0);
			break;
       default:
			KillTimer(hWnd, wParam);
            break;
		}
		break;

    case WM_KEYUP:
		RepeatFlag = 0;
        switch(LOWORD(wParam))
        {
        case VK_UP:
        case VK_DOWN:
			KillTimer(hWnd, TIMER_ID);
            break;
        default:
            break;
        }        
        break;
        
    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET:
            switch(iButtonJust)
            {
            case IDC_DISTANCE:
                CallUnitListWnd(hFrameWin, DIS_UNIT_LIST);
                break;
            case IDC_SPEED:
                CallUnitListWnd(hFrameWin, SPID_UNIT_LIST);                   
                break;
            case IDC_ELEVATION:
                CallUnitListWnd(hFrameWin, ELE_UNIT_LIST);
                break;
            }
            break;
            
            case IDC_DISTANCE:
                if(HIWORD( wParam ) == SSBN_CHANGE)
                {
                    iCurSel = SendMessage(hDistance, SSBM_GETCURSEL, 0, 0);
                    SetUnitDistance(iCurSel);
                }
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                {
                    iButtonJust = IDC_DISTANCE;
                    hFocus = hDistance;
                }
                break;
            case IDC_SPEED:
                if(HIWORD( wParam ) == SSBN_CHANGE)
                {
                    iCurSel = SendMessage(hSpeed, SSBM_GETCURSEL, 0, 0);
                    SetUnitSpeed(iCurSel);
                }
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                {
                    iButtonJust = IDC_SPEED;
                    hFocus = hSpeed;
                }
                break;
            case IDC_ELEVATION:
                if(HIWORD( wParam ) == SSBN_CHANGE)
                {
                    iCurSel = SendMessage(hElevation, SSBM_GETCURSEL, 0, 0);
                    SetUnitElevation(iCurSel);
                }
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                {
                    iButtonJust = IDC_ELEVATION;
                    hFocus = hElevation;
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
static BOOL CreateControl(HWND hWnd, HWND * hDistance, HWND * hSpeed, HWND * hElevation)
{
    int xzero=0,yzero=0;
    int iControlH,iControlW;
    RECT rect;
    GetClientRect(hWnd, &rect);
    iControlH = rect.bottom/3;
    iControlW = rect.right;
    
    
    * hDistance = CreateWindow( "SPINBOXEX", ML("Distance"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST  | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hWnd, (HMENU)IDC_DISTANCE, NULL, NULL);
    
    if (* hDistance == NULL)
        return FALSE;
    * hSpeed = CreateWindow( "SPINBOXEX", ML("Speed"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST  | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH, iControlW, iControlH, 
        hWnd, (HMENU)IDC_SPEED, NULL, NULL);
    
    if (* hSpeed == NULL)
        return FALSE;
    * hElevation = CreateWindow( "SPINBOXEX", ML("Elevation"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST  | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 2, iControlW, iControlH, 
        hWnd, (HMENU)IDC_ELEVATION, NULL, NULL);
    
    if (* hElevation == NULL)
        return FALSE;
    return TRUE;
    
}

static BOOL CallUnitListWnd(HWND hFatherWnd, UNIT_TYPE unittype)
{
    WNDCLASS    wc;
    RECT rClient;
    
    hFrameWin = hFatherWnd;
    UnitType = unittype;
    
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = UnitListAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pUnitListClassName;
    
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    
    GetClientRect(hFrameWin, &rClient);
    hUnitListWndApp = CreateWindow(pUnitListClassName,"", 
        WS_VISIBLE | WS_CHILD,  
        
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        
        
        hFrameWin, NULL, NULL, NULL);
    
    if (NULL == hUnitListWndApp)
    {
        UnregisterClass(pUnitListClassName,NULL);
        return FALSE;
    }
    switch(UnitType)
    {
    case DIS_UNIT_LIST:
        SetWindowText(hFrameWin, ML("Distance"));
        break;
    case SPID_UNIT_LIST:
        SetWindowText(hFrameWin, ML("Speed"));
        break;
    case ELE_UNIT_LIST:
        SetWindowText(hFrameWin, ML("Elevation"));
        break;
    }
    
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK,(LPARAM)ML("Cancel"));        
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");   
    
    SetFocus(hUnitListWndApp);
    
    return (TRUE);   
    
}
static LRESULT UnitListAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
    static  HWND    hUnitList;
	HDC hdc;
    LRESULT lResult;
    static  int     nItemNum, iIndex;
    lResult = TRUE;   

    switch (wMsgCmd)
	{
		case WM_CREATE :
            UnitListCreateControl (hWnd, &hUnitList);
		    hdc         = GetDC(hWnd);
            hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		    ReleaseDC(hWnd,hdc);		
            
            switch(UnitType)
            {
            case DIS_UNIT_LIST:
                LoadUnitList(&hUnitList, DIS_UNIT_LIST);
                iIndex = GetUnitDistance();
                nItemNum = 4;
                break;
            case SPID_UNIT_LIST:
                LoadUnitList(&hUnitList, SPID_UNIT_LIST);
                iIndex = GetUnitSpeed();
                nItemNum = 4;
                break;
            case ELE_UNIT_LIST:
                LoadUnitList(&hUnitList, ELE_UNIT_LIST);
                iIndex = GetUnitElevation();
                nItemNum = 2;
                break;
            }

            SendMessage(hUnitList,LB_SETCURSEL,iIndex,0);
		    Load_Icon_SetupList(hUnitList,hIconNormal,hIconSel,nItemNum,iIndex);

		    break;

		case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_UNITLIST));
            break;

        case PWM_SHOWWINDOW:
            SetFocus(hWnd);
            switch(UnitType)
            {
            case DIS_UNIT_LIST:
                SetWindowText(hFrameWin, ML("Distance"));
                break;
            case SPID_UNIT_LIST:
                SetWindowText(hFrameWin, ML("Speed"));
                break;
            case ELE_UNIT_LIST:
                SetWindowText(hFrameWin, ML("Elevation"));
                break;
            }
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK,(LPARAM)ML("Cancel"));        
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");   
            break;
            
		case WM_KEYDOWN:
            switch(LOWORD(wParam))
            {       
		        case VK_F10:
                    SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;
                case VK_F5:
                    SendMessage(hWnd,WM_COMMAND, IDC_BUTTON_OK, 0);
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
		    hUnitListWndApp = NULL;
            UnregisterClass(pUnitListClassName,NULL);
            break;

	   case WM_COMMAND :    	 
           
           switch (LOWORD(wParam))
		    {
                case IDC_BUTTON_CANCEL:
                    DestroyWindow(hWnd);
                    break;      
                case IDC_BUTTON_OK:   
                    iIndex = SendMessage(hUnitList, LB_GETCURSEL, 0, 0);              //get the current selected item number
                    switch(UnitType)
                    {
                    case DIS_UNIT_LIST:
                        SetUnitDistance(iIndex);
                        break;
                    case SPID_UNIT_LIST:
                        SetUnitSpeed(iIndex);
                        break;
                    case ELE_UNIT_LIST:
                        SetUnitElevation(iIndex);
                        break;
                    }
		            Load_Icon_SetupList(hUnitList,hIconNormal,hIconSel,nItemNum,iIndex);
                         //change the radio button of the current selected item		

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
static void LoadUnitList(HWND * hList, UNIT_TYPE unittype)
{
    switch(unittype)
    {
    case DIS_UNIT_LIST:
        SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Meters"));
        SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Imperial miles"));
        SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Imperial yards"));
        SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Nautical miles"));
        break;
    case SPID_UNIT_LIST:
        SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Kilometersslhour"));
        SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Metersslsecond"));
        SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Milesslhour"));
        SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Knots"));
        break;
    case ELE_UNIT_LIST:
        SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Meters"));
        SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Imperial feet"));
        break;
        
    }

}
static BOOL UnitListCreateControl(HWND hWnd, HWND * hList)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    
    * hList = CreateWindow("LISTBOX", 0, 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0,0,rect.right - rect.left, rect.bottom - rect.top,
        hWnd, (HMENU)IDC_UNITLIST, NULL, NULL);

    if (* hList == NULL )
        return FALSE;

    return TRUE;

}
