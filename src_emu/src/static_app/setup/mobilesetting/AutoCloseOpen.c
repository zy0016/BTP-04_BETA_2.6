 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setting / auto switch on and off
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

#include    "AutoCloseOpen.h"

static const char * pClassName  = "AutoWndClass";

static const char * pAutoOnClassName = "AutoOnWndClass";
static const char * pAutoOnOffClassName = "AutoOnOffWndClass";
static const char * pAutoOnOffTimeClassName = "AutoOnOffTimeWndClass";
static HWND         hFrameWin = NULL;
static HWND         hWndApp = NULL;
static HWND         hTimeParentWindow = NULL;

static BOOL         OnOffFlag;
static BOOL         EnableOnOffFlag;
static BOOL         AutoOnOffTimeFlag;

static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static AUTO_CLOSE   g_AutoOCState;
static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

static BOOL  CallAutoOnTimeSetting(HWND hWnd, BOOL onoffFlag);

static BOOL CallAutoOnWindow(HWND hWnd, BOOL onoffFlag);
static BOOL CallAutoOnSetting(HWND hWnd, BOOL onoffFlag);
static LRESULT AutoOnAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  LRESULT AutoOnOffAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  LRESULT AutoOnOffTimeAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

static void SetAutoOpenState(SWITCHTYPE OnOrOff);
static void SetAutoCloseState(SWITCHTYPE OnOrOff);
static void SetAutoOpenTime(SYSTEMTIME * st);
static void SetAutoCloseTime(SYSTEMTIME * st);
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);


//main window of auto switch on/off
BOOL    CallAutoOSWindow(HWND hwndCall)
{
    WNDCLASS wc;
    RECT rClient;
    hFrameWin = hwndCall;

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
        return FALSE;

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
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back") ); 
            
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");

    SetWindowText(hFrameWin, ML("Auto switch onsloff"));
    SetFocus(hWndApp);
    return TRUE;
}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static      HWND    hFocus = 0, hAutoOn, hAutoOff;
    static      RECT    rect;
    static      int     iButtonJust;
    AUTO_CLOSE  AutoOnOff;
    SYSTEMTIME  AutoTime;
    char        szTimeStr[8];
                LRESULT lResult;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        GetClientRect(hWnd, &rect);
        
        hAutoOn = CreateWindow( "SPINBOXEX", ML("Auto switch on"), 
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
            0, 0, rect.right - rect.left, ( rect.bottom - rect.top ) / 3, 
            hWnd, (HMENU)IDC_AUTOON, NULL, NULL);
        
        if(hAutoOn == NULL)
        {
            lResult = FALSE;
            break;
        }
        
        hAutoOff = CreateWindow( "SPINBOXEX", ML("Auto switch off"), 
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
            0, ( rect.bottom - rect.top ) / 3, rect.right - rect.left, ( rect.bottom - rect.top ) / 3, 
            hWnd, (HMENU)IDC_AUTOOFF, NULL, NULL);
        
        if(hAutoOff == NULL)
        {
            lResult = FALSE;
            break;
        }
        GetAutoOSOption(&AutoOnOff);
        if(SWITCH_ON == AutoOnOff.OpenState)
        {
            AutoTime.wHour = AutoOnOff.OpenHour;
            AutoTime.wMinute = AutoOnOff.OpenMin;
            GetTimeDisplay(AutoTime, szTimeStr, NULL);
            SendMessage(hAutoOn, SSBM_DELETESTRING, 0, 0);
            SendMessage(hAutoOn, SSBM_ADDSTRING, 0, (LPARAM)szTimeStr);
        }   
        else
        {
            SendMessage(hAutoOn, SSBM_DELETESTRING, 0, 0);
            SendMessage(hAutoOn, SSBM_ADDSTRING, 0, (LPARAM)ML("No"));
        }
        
        if(SWITCH_ON == AutoOnOff.CloseState)
        {
            AutoTime.wHour = AutoOnOff.CloseHour;
            AutoTime.wMinute = AutoOnOff.CloseMin;
            GetTimeDisplay(AutoTime, szTimeStr, NULL);
            SendMessage(hAutoOff, SSBM_DELETESTRING, 0, 0);
            SendMessage(hAutoOff, SSBM_ADDSTRING, 0, (LPARAM)szTimeStr);
        }
        else
        {
            SendMessage(hAutoOff, SSBM_DELETESTRING, 0, 0);
            SendMessage(hAutoOff, SSBM_ADDSTRING, 0, (LPARAM)ML("No"));
        }      
        
        hFocus = hAutoOn;


        break;
        
    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back") ); 
        
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        
        SetWindowText(hFrameWin, ML("Auto switch onsloff"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif

        GetAutoOSOption(&AutoOnOff);
        if(SWITCH_ON == AutoOnOff.OpenState)
        {
            AutoTime.wHour = AutoOnOff.OpenHour;
            AutoTime.wMinute = AutoOnOff.OpenMin;
            GetTimeDisplay(AutoTime, szTimeStr, NULL);
            SendMessage(hAutoOn, SSBM_DELETESTRING, 0, 0);
            SendMessage(hAutoOn, SSBM_ADDSTRING, 0, (LPARAM)szTimeStr);
        }   
        else
        {
            SendMessage(hAutoOn, SSBM_DELETESTRING, 0, 0);
            SendMessage(hAutoOn, SSBM_ADDSTRING, 0, (LPARAM)ML("No"));
        }
        
        if(SWITCH_ON == AutoOnOff.CloseState)
        {
            AutoTime.wHour = AutoOnOff.CloseHour;
            AutoTime.wMinute = AutoOnOff.CloseMin;
            GetTimeDisplay(AutoTime, szTimeStr, NULL);
            SendMessage(hAutoOff, SSBM_DELETESTRING, 0, 0);
            SendMessage(hAutoOff, SSBM_ADDSTRING, 0, (LPARAM)szTimeStr);
        }
        else
        {
            SendMessage(hAutoOff, SSBM_DELETESTRING, 0, 0);
            SendMessage(hAutoOff, SSBM_ADDSTRING, 0, (LPARAM)ML("No"));
        }
        
        break;
        
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case WM_DESTROY :
        hWndApp = NULL;
        UnregisterClass(pClassName,NULL);
        break;
       
	case WM_KILLFOCUS:
		KillTimer(hWnd, TIMER_ID);
		RepeatFlag = 0;
		wKeyCode   = 0;
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
            case IDC_AUTOON:
                CallAutoOnWindow(hFrameWin, TRUE); //call auto switch on window
                break;
            case IDC_AUTOOFF:
                CallAutoOnWindow(hFrameWin, FALSE); //call auto switch off window
                break;
            }
            break;
        case IDC_AUTOON:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
            {
                iButtonJust = IDC_AUTOON;
                hFocus = hAutoOn;
            }
            break;
        case IDC_AUTOOFF:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
            {
                iButtonJust = IDC_AUTOOFF;
                hFocus = hAutoOff;
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


//call the window to select from "Enable/Disable the auto switch on/off"  and "auto switch on/off time"
static BOOL CallAutoOnWindow(HWND hWnd, BOOL onoffFlag)
{
    HWND hwnd;
    WNDCLASS wc;
    RECT rClient;
    hFrameWin = hWnd;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AutoOnAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pAutoOnClassName;

    if (!RegisterClass(&wc))
        return FALSE;

    OnOffFlag = onoffFlag;
    GetClientRect(hFrameWin, &rClient);

    hwnd = CreateWindow(pAutoOnClassName,NULL, 
        WS_VISIBLE | WS_CHILD,  
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, NULL, NULL, NULL);

    if (NULL == hwnd)
    {
        UnregisterClass(pAutoOnClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back"));
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");

	if(OnOffFlag)
		SetWindowText(hFrameWin, ML("Auto switch on"));
	else
		SetWindowText(hFrameWin, ML("Auto switch off"));

    SetFocus(hwnd);
    return TRUE;
}
static LRESULT AutoOnAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static      HWND    hFocus = 0, hAutoSwitchOn, hSwitchOnTime;
    static      RECT    rect;
    static      int     iButtonJust,iCurIndex;
    AUTO_CLOSE  AutoCO;
    SYSTEMTIME  TmpTime;
    char        szTimeStr[8];
                LRESULT lResult;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        GetClientRect(hWnd, &rect);
        
        if(OnOffFlag)
        {
            hAutoSwitchOn = CreateWindow( "SPINBOXEX", ML("Auto switch on"), 
                WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
                0, 0, rect.right - rect.left, ( rect.bottom - rect.top ) / 3, 
                hWnd, (HMENU)IDC_AUTOSWITCHON, NULL, NULL);
            hSwitchOnTime = CreateWindow( "SPINBOXEX", ML("Switch on time"), 
                WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT| CS_NOSYSCTRL,
                0, ( rect.bottom - rect.top ) / 3, rect.right - rect.left, ( rect.bottom - rect.top ) / 3, 
                hWnd, (HMENU)IDC_SWITCHONTIME, NULL, NULL);
        }
        
        else
        {
            hAutoSwitchOn = CreateWindow( "SPINBOXEX", ML("Auto switch off"), 
                WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT| CS_NOSYSCTRL,
                0, 0, rect.right - rect.left, ( rect.bottom - rect.top ) / 3, 
                hWnd, (HMENU)IDC_AUTOSWITCHON, NULL, NULL);
        
            hSwitchOnTime = CreateWindow( "SPINBOXEX", ML("Switch off time"), 
                WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT| CS_NOSYSCTRL,
                0, ( rect.bottom - rect.top ) / 3, rect.right - rect.left, ( rect.bottom - rect.top ) / 3, 
                hWnd, (HMENU)IDC_SWITCHONTIME, NULL, NULL);
        }

        if(hAutoSwitchOn == NULL)
        {
            lResult = FALSE;
            break;
        }
        
        SendMessage(hAutoSwitchOn, SSBM_ADDSTRING, 0, (LPARAM)ML("Yes"));
        SendMessage(hAutoSwitchOn, SSBM_ADDSTRING, 0, (LPARAM)ML("No"));
        

        if(hSwitchOnTime == NULL)
        {
            lResult = FALSE;
            break;
        }
        GetAutoOSOption(&AutoCO);

        if(OnOffFlag) //auto switch on
        {
            TmpTime.wHour = AutoCO.OpenHour;
            TmpTime.wMinute = AutoCO.OpenMin;
            SendMessage(hAutoSwitchOn, SSBM_SETCURSEL, AutoCO.OpenState, 0);
        }
        else
        {        
            TmpTime.wHour = AutoCO.CloseHour;
            TmpTime.wMinute = AutoCO.CloseMin;
            SendMessage(hAutoSwitchOn, SSBM_SETCURSEL, AutoCO.CloseState, 0);
        }
        GetTimeDisplay(TmpTime, szTimeStr, NULL);       

        SendMessage(hSwitchOnTime, SSBM_ADDSTRING, 0, (LPARAM)szTimeStr);
        hFocus = hAutoSwitchOn;
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0,(LPARAM)ML("Back"));
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        
		if(OnOffFlag)
			SetWindowText(hFrameWin, ML("Auto switch on"));
		else
			SetWindowText(hFrameWin, ML("Auto switch off"));
        GetAutoOSOption(&AutoCO);

        if(OnOffFlag) //auto switch on
        {
            TmpTime.wHour = AutoCO.OpenHour;
            TmpTime.wMinute = AutoCO.OpenMin;
            SendMessage(hAutoSwitchOn, SSBM_SETCURSEL, AutoCO.OpenState, 0);
        }
        else
        {        
            TmpTime.wHour = AutoCO.CloseHour;
            TmpTime.wMinute = AutoCO.CloseMin;
            SendMessage(hAutoSwitchOn, SSBM_SETCURSEL, AutoCO.CloseState, 0);
        }
        GetTimeDisplay(TmpTime, szTimeStr, NULL);
        
        SendMessage(hSwitchOnTime, SSBM_DELETESTRING, 0, 0);

        SendMessage(hSwitchOnTime, SSBM_ADDSTRING, 0, (LPARAM)szTimeStr);

        break;

    case WM_DESTROY :
        hFocus = NULL;
        UnregisterClass(pAutoOnClassName,NULL);
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
	case WM_KILLFOCUS:
		KillTimer(hWnd, TIMER_ID);
		RepeatFlag = 0;
		wKeyCode   = 0;
		break;
	case PWM_ACTIVATE:
		if (wParam == WA_INACTIVE)
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
			wKeyCode   = 0;
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
        case IDC_BUTTON_SET://
            switch(iButtonJust)
            {
            case IDC_AUTOSWITCHON:
                CallAutoOnSetting(hFrameWin, OnOffFlag);
                break;
            case IDC_SWITCHONTIME:
                CallAutoOnTimeSetting(hFrameWin, OnOffFlag);
                break;
            }
            break;
        case IDC_AUTOSWITCHON:
           	if(HIWORD( wParam ) == SSBN_CHANGE)
            {
                iCurIndex = SendMessage(hAutoSwitchOn,SSBM_GETCURSEL,0,0);
                if(OnOffFlag)
				{
					SetAutoOpenState(iCurIndex);
					DlmNotify(PS_AUTOPOWERON, NULL);
				}
                else
                    SetAutoCloseState(iCurIndex);
            }
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_AUTOSWITCHON;
            break;
        case IDC_SWITCHONTIME:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_SWITCHONTIME;
            break;
        }
        break;
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}


//call the window for setting if the switch is "Yes" or "No"
static BOOL CallAutoOnSetting(HWND hWnd, BOOL onoffFlag)
{
    HWND hwnd;
    WNDCLASS wc;
    RECT rClient;

    EnableOnOffFlag = onoffFlag;
    hFrameWin = hWnd;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AutoOnOffAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pAutoOnOffClassName;

    if (!RegisterClass(&wc))
        return FALSE;   
    
    GetClientRect(hFrameWin, &rClient);

    hwnd = CreateWindow(pAutoOnOffClassName, NULL, 
        WS_VISIBLE | WS_CHILD,  
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hwnd)
    {
        UnregisterClass(pAutoOnOffClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");              

    if(EnableOnOffFlag)
        SetWindowText(hFrameWin, ML("Auto switch on"));
    else
        SetWindowText(hFrameWin, ML("Auto switch off"));
    SetFocus(hwnd);

    return TRUE;

}
static  LRESULT AutoOnOffAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static      HWND    hAutoOnOffList;
    static      RECT    rect;
    static      int     iIndex;
                LRESULT lResult;
    static      HBITMAP hIconNormal;
    static      HBITMAP hIconSel;
    AUTO_CLOSE          AutoState;
    HDC         hdc;
    
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        GetClientRect(hWnd, &rect);        
        
        hAutoOnOffList = CreateWindow("LISTBOX",0, 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
            0,0,rect.right - rect.left, rect.bottom - rect.top,
            hWnd, (HMENU)IDC_AUTOONOFFLIST, NULL, NULL);

        if(NULL == hAutoOnOffList)
        {
            lResult = FALSE;
            break;
        }

        SendMessage(hAutoOnOffList, LB_ADDSTRING, 0, (LPARAM)ML("Yes"));
        SendMessage(hAutoOnOffList, LB_ADDSTRING, 0, (LPARAM)ML("No"));

        hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        ReleaseDC(hWnd,hdc);
		
        GetAutoOSOption(&AutoState);
        
        if(EnableOnOffFlag)
            iIndex = AutoState.OpenState;
        else
            iIndex = AutoState.CloseState;
        
        Load_Icon_SetupList(hAutoOnOffList,hIconNormal,hIconSel,2,iIndex);
        SendMessage(hAutoOnOffList, LB_SETCURSEL, iIndex, 0);  
        break;
        
    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_AUTOONOFFLIST));
        break;

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0,(LPARAM)ML("Cancel") );
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");              
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        
        if(EnableOnOffFlag)
            SetWindowText(hFrameWin, ML("Auto switch on"));
        else
            SetWindowText(hFrameWin, ML("Auto switch off"));
        
        break;
        
    case WM_DESTROY :
        DeleteObject(hIconNormal);
        DeleteObject(hIconSel);
        KillTimer(hWnd, TIMER_ASURE);
        UnregisterClass(pAutoOnOffClassName,NULL);
        break;
       
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET:
            iIndex = SendMessage(hAutoOnOffList, LB_GETCURSEL, 0, 0);
	         
            Load_Icon_SetupList(hAutoOnOffList,hIconNormal,hIconSel,2,iIndex);     //change the radio button of the current selected item		

            if(EnableOnOffFlag)
			{
				SetAutoOpenState(iIndex);
				DlmNotify(PS_AUTOPOWERON, NULL);
			}
            else
                SetAutoCloseState(iIndex);
                
            SetTimer(hWnd, TIMER_ASURE, 500, NULL);

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
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}



// call the window for setting the time(hour : minute) of switch on/off

static BOOL  CallAutoOnTimeSetting(HWND hWnd, BOOL onoffFlag)
{
    HWND hwnd;
    WNDCLASS wc;
    RECT rClient;
    AutoOnOffTimeFlag = onoffFlag;
    hFrameWin = hWnd;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AutoOnOffTimeAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pAutoOnOffTimeClassName;

    if (!RegisterClass(&wc))
        return FALSE;

    GetClientRect(hFrameWin, &rClient);
    
    hwnd = CreateWindow(pAutoOnOffTimeClassName, NULL, 
        WS_VISIBLE | WS_CHILD,  
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hwnd)
    {
        UnregisterClass(pAutoOnOffTimeClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );
                
    SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BUTTON_SAVE,1),(LPARAM)ML("Save") ) ;
    //set the left soft key
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");               

    if(AutoOnOffTimeFlag)
        SetWindowText(hFrameWin, ML("Auto switch on"));
    else
        SetWindowText(hFrameWin, ML("Auto switch off"));
    SetFocus(hwnd);

    return TRUE;
}

static  LRESULT AutoOnOffTimeAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static      HWND    hTime, hFocus;
    static      RECT    rect;
                LRESULT lResult;
    AUTO_CLOSE          AutoState;
    SYSTEMTIME  st = {0}, systime = {0};
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        GetClientRect(hWnd, &rect);       
         
        GetAutoOSOption(&AutoState);

        if(TRUE == AutoOnOffTimeFlag) //auto switch on
        {
            st.wHour = AutoState.OpenHour;
            st.wMinute = AutoState.OpenMin;
        if (TF_24 == GetTimeFormt() )
            hTime = CreateWindow (           //create the 24HR "time" editor;  
                "TIMEEDIT",ML("Switch on timecolon"), 
                WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_TIME_24HR | TES_TITLE,
                0, 0, rect.right - rect.left, (rect.bottom - rect.top)/3,
                hWnd,(HMENU)IDC_TIME,NULL,(LPVOID)&st);
    
        else
            hTime = CreateWindow (           //create the 12HR "time" editor;  
                "TIMEEDIT",ML("Switch on timecolon"), 
                WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_TIME_12HR | TES_TITLE,
                0, 0, rect.right - rect.left, (rect.bottom - rect.top)/3,
                hWnd,(HMENU)IDC_TIME,NULL,(LPVOID)&st);
        }
        else
        {
            st.wHour = AutoState.CloseHour;
            st.wMinute = AutoState.CloseMin;
        if (TF_24 == GetTimeFormt() )
            hTime = CreateWindow (           //create the 24HR "time" editor;  
                "TIMEEDIT",ML("Switch off timecolon"), 
                WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_TIME_24HR | TES_TITLE,
                0, 0, rect.right - rect.left, (rect.bottom - rect.top)/3,
                hWnd,(HMENU)IDC_TIME,NULL,(LPVOID)&st);
    
        else
            hTime = CreateWindow (           //create the 12HR "time" editor;  
                "TIMEEDIT",ML("Switch off timecolon"), 
                WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_TIME_12HR | TES_TITLE,
                0, 0, rect.right - rect.left, (rect.bottom - rect.top)/3,
                hWnd,(HMENU)IDC_TIME,NULL,(LPVOID)&st);
        }      
	    hFocus = hTime;	        
        break;
        
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0,(LPARAM)ML("Cancel") );
        
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)ML("Save") ) ;
        //set the left soft key
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");               
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        
        if(AutoOnOffTimeFlag)
            SetWindowText(hFrameWin, ML("Auto switch on"));
        else
            SetWindowText(hFrameWin, ML("Auto switch off"));
        break;

    case WM_DESTROY :
        UnregisterClass(pAutoOnOffTimeClassName,NULL);
        break;
       
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:

        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET:
			SendMessage(hTime, TEM_GETTIME, 0, (LPARAM)&systime);				  

	        if(TRUE == AutoOnOffTimeFlag) //for auto switch on
            {
                SetAutoOpenTime(&systime);
				DlmNotify(PS_AUTOPOWERON, NULL);

                SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                PostMessage(hWnd, WM_CLOSE, 0, 0);
                break;
            }
            else //for auto switch off
            {
                SetAutoCloseTime(&systime);
                SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                PostMessage(hWnd, WM_CLOSE, 0, 0);
                break;
            }
        }
        break;


    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}




static void SetAutoCloseState(SWITCHTYPE OnOrOff)
{ 
	int openstate;
	RTCTIME rtcTime;
	SYSTEMTIME stime;

	openstate = FS_GetPrivateProfileInt(SN_AUTO_CLOSE,KN_AUTO_OPEN_STATE,
        SWITCH_OFF,SETUPFILENAME);
    
	FS_WritePrivateProfileInt(SN_AUTO_CLOSE,KN_AUTO_CLOSE_STATE,
        OnOrOff,SETUPFILENAME);
    DlmNotify(PS_POWEROFF,ICON_CANCEL);
    DlmNotify(PS_POWERON,ICON_CANCEL);
    DlmNotify(PS_POWERONOFF,ICON_CANCEL);

    if(openstate == SWITCH_ON  && OnOrOff == SWITCH_ON)
    {
        DlmNotify(PS_POWERONOFF,ICON_SET);
    }
	else  if(openstate == SWITCH_ON && OnOrOff == SWITCH_OFF)
    {
        DlmNotify(PS_POWERON,ICON_SET);
    }
	else  if(openstate == SWITCH_OFF && OnOrOff == SWITCH_ON)
    {
        DlmNotify(PS_POWEROFF,ICON_SET);
    }
	
	if (g_AutoOCState.CloseState != OnOrOff)
	{
		if (OnOrOff == SWITCH_ON)
		{
			memset(&rtcTime, 0, sizeof(RTCTIME));
			GetLocalTime(&stime);
			rtcTime.v_nDay = (u_INT1)(stime.wDay);
			rtcTime.v_nMonth = (u_INT1)(stime.wMonth);
			rtcTime.v_nYear = (u_INT2)(stime.wYear);
			rtcTime.v_nHour = (u_INT1)(g_AutoOCState.CloseHour);
			rtcTime.v_nMinute = (u_INT1)(g_AutoOCState.CloseMin);
			rtcTime.v_nSecond = 30;
			RTC_SetAlarms(AUTO_POWER_OFF, ALM_ID_AUTO_POWER_OFF, ALMF_REPLACE|ALMF_AUTONEXTDAY, AUTO_POWER_OFF, &rtcTime);

		}
		else if (OnOrOff == SWITCH_OFF)
		{
			RTC_KillAlarms(AUTO_POWER_OFF, ALM_ID_AUTO_POWER_OFF);
		}
	}

	g_AutoOCState.CloseState = OnOrOff;
}

static void SetAutoOpenState(SWITCHTYPE OnOrOff)
{
	int closestate;
//	RTCTIME rtcTime;
//	SYSTEMTIME sTime;

	closestate =  FS_GetPrivateProfileInt(SN_AUTO_CLOSE,KN_AUTO_CLOSE_STATE,
        SWITCH_OFF,SETUPFILENAME);

    FS_WritePrivateProfileInt(SN_AUTO_CLOSE,KN_AUTO_OPEN_STATE,
        OnOrOff,SETUPFILENAME);
    DlmNotify(PS_POWEROFF,ICON_CANCEL);
    DlmNotify(PS_POWERON,ICON_CANCEL);
    DlmNotify(PS_POWERONOFF,ICON_CANCEL);

    if(OnOrOff == SWITCH_ON  && closestate == SWITCH_ON)
    {
        DlmNotify(PS_POWERONOFF,ICON_SET);
    }
    else if(OnOrOff == SWITCH_ON && closestate == SWITCH_OFF)
    {
        DlmNotify(PS_POWERON,ICON_SET);
    }
    else if(OnOrOff == SWITCH_OFF && closestate == SWITCH_ON)
    {
        DlmNotify(PS_POWEROFF,ICON_SET);
    }

//	if (g_AutoOCState.OpenState != OnOrOff)
//	{
//		if (OnOrOff == SWITCH_ON)
//		{
//			GetLocalTime(&sTime);
//			memset(&rtcTime, 0, sizeof(RTCTIME));
//			rtcTime.v_nHour = (u_INT1)(g_AutoOCState.OpenHour);
//			rtcTime.v_nMinute = (u_INT1)(g_AutoOCState.OpenMin);
//			rtcTime.v_nYear = (u_INT2)(sTime.wYear);
//			rtcTime.v_nMonth = (u_INT1)(sTime.wMonth);
//			rtcTime.v_nDay = (u_INT1)(sTime.wDay);
//			rtcTime.v_nSecond = 30;
//			RTC_SetAlarms(AUTO_POWER_ON, ALM_ID_AUTO_POWER_ON, ALMF_REPLACE|ALMF_POWEROFF|ALMF_AUTONEXTDAY, AUTO_POWER_ON, &rtcTime);
//
//		}
//		else if (OnOrOff == SWITCH_OFF)
//		{
//			RTC_KillAlarms(AUTO_POWER_ON, ALM_ID_AUTO_POWER_ON);
//		}
//	}
	
	g_AutoOCState.OpenState = OnOrOff;
}

static void SetAutoCloseTime(SYSTEMTIME * st)
{
	RTCTIME rtcTime;
	SYSTEMTIME stime;

    FS_WritePrivateProfileInt(SN_AUTO_CLOSE, KN_AUTO_CLOSE_HOUR,
        st->wHour, SETUPFILENAME);
    FS_WritePrivateProfileInt(SN_AUTO_CLOSE, KN_AUTO_CLOSE_MINUTE,
        st->wMinute, SETUPFILENAME);

	g_AutoOCState.CloseHour = st->wHour;
	g_AutoOCState.CloseMin  = st->wMinute;
	
	if (g_AutoOCState.CloseState == SWITCH_ON)
	{
		memset(&rtcTime, 0, sizeof(RTCTIME));
		GetLocalTime(&stime);
		rtcTime.v_nDay = (u_INT1)(stime.wDay);
		rtcTime.v_nMonth = (u_INT1)(stime.wMonth);
		rtcTime.v_nYear = (u_INT2)(stime.wYear);
		rtcTime.v_nHour = (u_INT1)(g_AutoOCState.CloseHour);
		rtcTime.v_nMinute = (u_INT1)(g_AutoOCState.CloseMin);
		rtcTime.v_nSecond = 30;
		RTC_SetAlarms(AUTO_POWER_OFF, ALM_ID_AUTO_POWER_OFF, ALMF_REPLACE|ALMF_AUTONEXTDAY, AUTO_POWER_OFF, &rtcTime);		
	}

}

static void SetAutoOpenTime(SYSTEMTIME * st)
{
//	RTCTIME rtcTime;
//	SYSTEMTIME sTime;
	
  
	FS_WritePrivateProfileInt(SN_AUTO_CLOSE, KN_AUTO_OPEN_HOUR,
        st->wHour, SETUPFILENAME);
    FS_WritePrivateProfileInt(SN_AUTO_CLOSE, KN_AUTO_OPEN_MINUTE,
        st->wMinute, SETUPFILENAME);
	g_AutoOCState.OpenHour = st->wHour;
	g_AutoOCState.OpenMin  = st->wMinute;

//	if (g_AutoOCState.OpenState == SWITCH_ON)
//	{
//		GetLocalTime(&sTime);
//		memset(&rtcTime, 0, sizeof(RTCTIME));
//		rtcTime.v_nHour = (u_INT1)(g_AutoOCState.OpenHour);
//		rtcTime.v_nMinute = (u_INT1)(g_AutoOCState.OpenMin);
//		rtcTime.v_nYear = (u_INT2)(sTime.wYear);
//		rtcTime.v_nMonth = (u_INT1)(sTime.wMonth);
//		rtcTime.v_nDay = (u_INT1)(sTime.wDay);
//		rtcTime.v_nSecond = 30;
//		RTC_SetAlarms(AUTO_POWER_ON, ALM_ID_AUTO_POWER_ON, ALMF_REPLACE|ALMF_POWEROFF|ALMF_AUTONEXTDAY, AUTO_POWER_ON, &rtcTime);		
//	}
}
/*******************************************************************
* Function      GetAutoOSOption
* Purpose       read the automatic switch on/off info;
* Params        
* Return        
* Remarks      
*********************************************************************/

BOOL    GetAutoOSOption(AUTO_CLOSE * os)
{
    if (os == NULL)
        return FALSE;

    os->CloseHour   = (WORD)FS_GetPrivateProfileInt(SN_AUTO_CLOSE,KN_AUTO_CLOSE_HOUR,
        0,SETUPFILENAME);
    os->CloseMin    = (WORD)FS_GetPrivateProfileInt(SN_AUTO_CLOSE,KN_AUTO_CLOSE_MINUTE,
        0,SETUPFILENAME);
    os->CloseState  = FS_GetPrivateProfileInt(SN_AUTO_CLOSE,KN_AUTO_CLOSE_STATE,
        SWITCH_OFF,SETUPFILENAME);
    os->OpenHour    = (WORD)FS_GetPrivateProfileInt(SN_AUTO_CLOSE,KN_AUTO_OPEN_HOUR,
        0,SETUPFILENAME);
    os->OpenMin     = (WORD)FS_GetPrivateProfileInt(SN_AUTO_CLOSE,KN_AUTO_OPEN_MINUTE,
        0,SETUPFILENAME);
    os->OpenState   = FS_GetPrivateProfileInt(SN_AUTO_CLOSE,KN_AUTO_OPEN_STATE,
        SWITCH_OFF,SETUPFILENAME);

    if ((os->CloseHour < 0) || (os->CloseHour > 23))
        os->CloseHour = 0;
    if ((os->CloseMin < 0) || (os->CloseMin > 59))
        os->CloseMin = 0;
    if ((os->OpenHour < 0) || (os->OpenHour > 23))
        os->OpenHour = 0;
    if ((os->OpenMin < 0) || (os->OpenMin > 59))
        os->OpenMin = 0;


    if ((os->CloseState != SWITCH_OFF) && (os->CloseState != SWITCH_ON))
        os->CloseState = SWITCH_OFF;
    if ((os->OpenState != SWITCH_OFF) && (os->OpenState != SWITCH_ON))
        os->OpenState = SWITCH_OFF;
	
	memcpy(&g_AutoOCState, os, sizeof(AUTO_CLOSE));
    return TRUE;
}

void FetchAutoOCState(AUTO_CLOSE  *asOC)
{
	memcpy(asOC, &g_AutoOCState, sizeof(AUTO_CLOSE));
}

void InitAutoOCState(void)
{
	AUTO_CLOSE os;
	GetAutoOSOption(&os);
}

void Sett_RestoreRamOCState(void)
{
	g_AutoOCState.CloseHour = 0;
	g_AutoOCState.CloseMin = 0;
	g_AutoOCState.CloseState = SWITCH_OFF;
	g_AutoOCState.OpenHour = 0;
	g_AutoOCState.OpenMin = 0;
	g_AutoOCState.OpenState = SWITCH_OFF;
}

BOOL Sett_GetAutoOpenInfo(int *nHour, int *nMinute)
{
	if (nHour != NULL)
		*nHour	 = g_AutoOCState.OpenHour;
	
	if (nMinute != NULL)
		*nMinute = g_AutoOCState.OpenMin;

	if ( 0 == g_AutoOCState.OpenState ) //0: open, 1: close
		return TRUE;
	else 
		return FALSE;
}

BOOL Sett_GetAutoCloseInfo(int *nHour, int *nMinute)
{
	if (nHour != NULL)
		*nHour	 = FS_GetPrivateProfileInt(SN_AUTO_CLOSE,KN_AUTO_CLOSE_HOUR,
        0,SETUPFILENAME);
	
	if (nMinute != NULL)
		*nMinute = FS_GetPrivateProfileInt(SN_AUTO_CLOSE,KN_AUTO_CLOSE_MINUTE,
        0,SETUPFILENAME);

	if (FS_GetPrivateProfileInt(SN_AUTO_CLOSE,KN_AUTO_CLOSE_STATE,SWITCH_OFF,SETUPFILENAME) == 0 )
         //0: open, 1: close
		return TRUE;
	else 
		return FALSE;

}

int SetAutoUpWhilePowerOff(void)
{
	RTCTIME rtcTime;
	SYSTEMTIME sTime;

	if (g_AutoOCState.OpenState == SWITCH_ON)
	{
		GetLocalTime(&sTime);
		memset(&rtcTime, 0, sizeof(RTCTIME));
		rtcTime.v_nHour = (u_INT1)(g_AutoOCState.OpenHour);
		rtcTime.v_nMinute = (u_INT1)(g_AutoOCState.OpenMin);
		rtcTime.v_nYear = (u_INT2)(sTime.wYear);
		rtcTime.v_nMonth = (u_INT1)(sTime.wMonth);
		rtcTime.v_nDay = (u_INT1)(sTime.wDay);
		rtcTime.v_nSecond = 30;
		return RTC_SetAlarms(AUTO_POWER_ON, ALM_ID_AUTO_POWER_ON, ALMF_REPLACE|ALMF_POWEROFF|ALMF_AUTONEXTDAY, AUTO_POWER_ON, &rtcTime);		
	}
	else
	{
		return -1;
	}	
}
