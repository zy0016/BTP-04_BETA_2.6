         /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : sound profile
 *
 * Purpose  : select the set for rings
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "callringsetwindow.h"
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
#include    "PreBrow.h"
#include	"sndmgr.h"


#define  pClassName     "CallRingSetWindowClass"

static  HWND hAppWnd = NULL;
static  HWND hFrameWin = NULL;
static  int          iCurMode;
RINGFOR              iRingFor;
static  int  nCurVol;
static SCENEMODE g_sm = {0};

static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl (HWND hWnd,HWND *hTone,HWND *hVolLev,HWND *hMode);
static void SetRingMode(RINGFOR iRingFor, RINGTYPE iRingType);

extern  BOOL CallLevelAdjuster(HWND hFrame, HWND hcallWnd, ADJTYPE adjType, int nlevel,UINT wCallbackMsg, char* szTitle);
extern  void IntToString( int nValue, char *buf );
extern  BOOL CallRingMode(HWND hWnd, RINGFOR iringfor, int icurmode);
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);
BOOL Sett_GetNowPlayRing(char* cMusicFile);

/***************************************************************
* Function  CallRingSetWindow 
* Purpose           
* Params
* Return    
* Remarks
***************************************************************/
BOOL CallRingSetWindow(HWND hwndApp,int icurMode,RINGFOR iringFor)
{  
	WNDCLASS    wc;
    RECT rClient;
	iCurMode = icurMode;
    iRingFor = iringFor;

    hFrameWin = hwndApp;
    
    GetSM(&g_sm,iCurMode);
 
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

    hAppWnd = CreateWindow(pClassName,NULL, 
        WS_VISIBLE | WS_CHILD,          
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
	SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Back"));//RSK       

    SetFocus(hAppWnd);

    return (TRUE);
}
/***************************************************************
* Function  AppWndProc 
* Purpose           
* Params
* Return    
* Remarks
***************************************************************/
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT     lResult;   
	static HWND hTone,hVolLev,hMode;
    static HWND hFocus = 0;
    static int  iButtonJust,iCurIndex, iIndex;
    char * Tmp;
    char ToneFileName[RINGNAMEMAXLEN+1], TmpName[RINGNAMEMAXLEN+1];
	HWND hRingWnd;
    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
    case WM_CREATE:
        memset(ToneFileName, 0, RINGNAMEMAXLEN+1);
        memset(TmpName, 0, RINGNAMEMAXLEN+1);
        CreateControl (hWnd,&hTone,&hVolLev,&hMode);
        hFocus = hTone;
        switch(iRingFor)
        {
        case INCOMCALL:
            SetWindowText(hFrameWin,ML("Incoming call"));
            Tmp = strrchr(g_sm.rIncomeCall.cMusicFileName, '/');
            nCurVol = g_sm.rIncomeCall.iRingVolume;
            SendMessage(hMode,SSBM_SETCURSEL,(WPARAM)g_sm.rIncomeCall.iRingType,0);
            break;
        case MESSAGES :
            SetWindowText(hFrameWin,ML("Messages"));
            Tmp = strrchr(g_sm.rMessage.cMusicFileName, '/');
            nCurVol = g_sm.rMessage.iRingVolume;
            break;
        case TMESSAGES:
            SetWindowText(hFrameWin,ML("Telematic messages"));
            Tmp = strrchr(g_sm.rTelematics.cMusicFileName, '/');
            nCurVol = g_sm.rTelematics.iRingVolume;
            break;
        case ALARMCLK:
            SetWindowText(hFrameWin,ML("Alarm clock"));
            Tmp = strrchr(g_sm.rAlarmClk.cMusicFileName, '/');
            nCurVol = g_sm.rAlarmClk.iRingVolume;
            SendMessage(hMode,SSBM_SETCURSEL,(WPARAM)g_sm.rAlarmClk.iRingType,0);
            break;
        case CALENDAR:
            SetWindowText(hFrameWin,ML("Calendar alarm"));
            Tmp = strrchr(g_sm.rCalendar.cMusicFileName, '/');
            nCurVol = g_sm.rCalendar.iRingVolume;
            SendMessage(hMode,SSBM_SETCURSEL,(WPARAM)g_sm.rCalendar.iRingType,0);
            break;
        }
        SendMessage(hVolLev,SSBM_SETCURSEL,nCurVol,0);
        if(Tmp != 0)
        {
            Tmp++;
//			UTF8ToMultiByte(CP_ACP,0,Tmp,strlen(Tmp),TmpName,RINGNAMEMAXLEN+1,NULL,NULL);
            SendMessage(hTone,SSBM_ADDSTRING,0,(LPARAM)/*TmpName*/Tmp);
        }
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case WM_DESTROY :
        hAppWnd = NULL;
        UnregisterClass(pClassName,NULL);
        break;

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Back"));//RSK 
        GetSM(&g_sm, iCurMode);
        switch(iRingFor)
        {
        case INCOMCALL:
            SetWindowText(hFrameWin,ML("Incoming call"));
            Tmp = strrchr(g_sm.rIncomeCall.cMusicFileName, '/');
            iIndex = g_sm.rIncomeCall.iRingType;
            break;
        case MESSAGES:
            SetWindowText(hFrameWin,ML("Messages"));
            Tmp = strrchr(g_sm.rMessage.cMusicFileName, '/');
            iIndex = g_sm.rMessage.iRingType ;
            break;
        case TMESSAGES:
            SetWindowText(hFrameWin,ML("Telematic messages"));
            Tmp = strrchr(g_sm.rTelematics.cMusicFileName, '/');
            iIndex = g_sm.rTelematics.iRingType;
            break;
        case ALARMCLK:
            SetWindowText(hFrameWin,ML("Alarm clock"));
            Tmp = strrchr(g_sm.rAlarmClk.cMusicFileName, '/');
            iIndex = g_sm.rAlarmClk.iRingType ;
            break;
        case CALENDAR:
            SetWindowText(hFrameWin,ML("Calendar alarm"));
            Tmp = strrchr(g_sm.rCalendar.cMusicFileName, '/');
            iIndex = g_sm.rCalendar.iRingType;
            break;
        }         
        if(Tmp != 0)
        {
            Tmp++;			
			memset(TmpName, 0, RINGNAMEMAXLEN+1);
			strncpy(TmpName, Tmp, strlen(Tmp));
//			UTF8ToMultiByte(CP_ACP,0,Tmp,strlen(Tmp),TmpName,RINGNAMEMAXLEN+1,NULL,NULL);
            SendMessage(hTone, SSBM_DELETESTRING, 0, 0);
            SendMessage(hTone, SSBM_ADDSTRING, 0, (LPARAM)TmpName);
        }
        
        SendMessage(hMode, SSBM_SETCURSEL, iIndex, 0);        
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
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_OK,0);
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
        switch(LOWORD(wParam))
        {
        case IDC_BUTTON_OK:
            switch(iButtonJust)
            {
            case IDC_TONE:
                hRingWnd = PreviewSoundEx(hFrameWin,hWnd, CALLBACK_SETRING, (PSTR)ML("Tone"), TRUE);
				memset(ToneFileName, 0, sizeof(ToneFileName));
                Sett_GetNowPlayRing(ToneFileName);
				SendMessage(hRingWnd, SPM_SETCURSEL, 0, (LPARAM)ToneFileName);
				break;
            case IDC_VOLLEV:
/*
				if (iRingFor == INCOMCALL)
*/
					CallLevelAdjuster(hFrameWin, hWnd,LAS_RING,nCurVol,CALLBACK_SETVOL,(char*)ML("Volume level"));
/*
				else
					CallLevelAdjuster(hFrameWin, hWnd,LAS_VOL,nCurVol,CALLBACK_SETVOL,(char*)ML("Volume level"));
*/
                break;
            case IDC_MODE:
                CallRingMode(hFrameWin, iRingFor, iCurMode);
                break;
            }
            break;

        case IDC_TONE:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
            {
                iButtonJust = IDC_TONE;
                hFocus = hTone;
            }
            break;
        
        case IDC_VOLLEV:
            if(HIWORD( wParam ) == SSBN_CHANGE)
            {
                nCurVol = SendMessage(hVolLev,SSBM_GETCURSEL,0,0);
                switch(iRingFor)
                {
                case INCOMCALL:
                    g_sm.rIncomeCall.iRingVolume = nCurVol;
                    break;
                case MESSAGES:
                    g_sm.rMessage.iRingVolume = nCurVol;
                    break;
                case TMESSAGES:
                    g_sm.rTelematics.iRingVolume = nCurVol;
                    break;
                case ALARMCLK:
                    g_sm.rAlarmClk.iRingVolume = nCurVol;
                    break;
                case CALENDAR:
                    g_sm.rCalendar.iRingVolume = nCurVol;
                    break;
                }
                SetSM(&g_sm,iCurMode);                               
            }
            if(HIWORD(wParam) == SSBN_SETFOCUS)
            {
                iButtonJust = IDC_VOLLEV;
                hFocus = hVolLev;
            }
            break;

        case IDC_MODE:
            if(HIWORD( wParam ) == SSBN_CHANGE)
            {
                iCurIndex = SendMessage(hMode,SSBM_GETCURSEL,0,0);
                SetRingMode(iRingFor, iCurIndex);
            }
            if(HIWORD(wParam) == SSBN_SETFOCUS)
            {
                iButtonJust = IDC_MODE;
                hFocus = hMode;
            }
            break;
        }
        break;	

    case CALLBACK_SETVOL:
        if ( 1 == wParam )
        {
            nCurVol = lParam;
            
            switch(iRingFor)
            {
            case INCOMCALL:
                g_sm.rIncomeCall.iRingVolume = nCurVol;
                break;
            case MESSAGES:
                g_sm.rMessage.iRingVolume = nCurVol;
                break;
            case TMESSAGES:
                g_sm.rTelematics.iRingVolume = nCurVol;
                break;
            case ALARMCLK:
                g_sm.rAlarmClk.iRingVolume = nCurVol;
                break;
            case CALENDAR:
                g_sm.rCalendar.iRingVolume = nCurVol;
                break;
            }
            SetSM(&g_sm,iCurMode);
            
            SendMessage(hVolLev,SSBM_SETCURSEL,nCurVol,0);
            
        }
        break;

    case CALLBACK_SETRING:
        switch(iRingFor)
        {
        case INCOMCALL:
            strcpy(g_sm.rIncomeCall.cMusicFileName, (const char*)lParam);
            break;
        case MESSAGES:
            strcpy(g_sm.rMessage.cMusicFileName, (const char*)lParam);
            break;
        case TMESSAGES:
            strcpy(g_sm.rTelematics.cMusicFileName, (const char*)lParam);
            break;
        case ALARMCLK:
            strcpy(g_sm.rAlarmClk.cMusicFileName, (const char*)lParam);
            break;
        case CALENDAR:
            strcpy(g_sm.rCalendar.cMusicFileName, (const char*)lParam);
            break;
        }
        SetSM(&g_sm,iCurMode);
        break;

    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
	}
return lResult;
}

static void SetRingMode(RINGFOR iRingFor, RINGTYPE iRingType)
{
     switch(iRingFor)
        {
            case INCOMCALL:
                g_sm.rIncomeCall.iRingType = iRingType;
                break;
		    case MESSAGES :
                g_sm.rMessage.iRingType = iRingType;
			    break;
		    case TMESSAGES:
                g_sm.rTelematics.iRingType = iRingType;
			    break;
		    case ALARMCLK:
                g_sm.rAlarmClk.iRingType = iRingType;
			    break;
		    case CALENDAR:
		        g_sm.rCalendar.iRingType = iRingType;
                break;                    
        }
    SetSM(&g_sm,iCurMode);
    
}
/***************************************************************
* Function  CreateControl 
* Purpose           
* Params
* Return    
* Remarks
***************************************************************/

static  BOOL    CreateControl (HWND hWnd,HWND *hTone,HWND *hVolLev,HWND *hMode)
{
    int iControlH,iControlW;
	int xzero=0,yzero=0;

	RECT    rect;
    GetClientRect(hWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;
/************************************************************************/

	* hTone = CreateWindow( "SPINBOXEX", ML("Tone"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hWnd, (HMENU)IDC_TONE, NULL, NULL);

    if (* hTone == NULL)
        return FALSE;  
	
	* hVolLev = CreateWindow( "LEVIND", ML("Volume level"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT| CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 1, iControlW, iControlH, 
        hWnd, (HMENU)IDC_VOLLEV, NULL, (PVOID)LAS_VOL);

    if (* hVolLev == NULL)
        return FALSE;  

	if((iRingFor==MESSAGES) || (iRingFor == TMESSAGES))
		*hMode = NULL;
	else
	{
		* hMode = CreateWindow( "SPINBOXEX", ML("Mode"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT| CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 2, iControlW, iControlH, 
        hWnd, (HMENU)IDC_MODE, NULL, NULL);

    
		if (* hMode == NULL)
         return FALSE;  
				  
	    SendMessage(*hMode,SSBM_ADDSTRING,NORMAL,(LPARAM)ML("Normal"));
	    SendMessage(*hMode,SSBM_ADDSTRING,ASCENDING,(LPARAM)ML("Ascending"));
	    SendMessage(*hMode,SSBM_ADDSTRING,BEEPFIRST,(LPARAM)ML("Beep first"));
	}

	return TRUE;
}

BOOL Sett_GetNowPlayRing(char* cMusicFile)
{
	switch(iRingFor)
	{
	case INCOMCALL:
		strncpy(cMusicFile, g_sm.rIncomeCall.cMusicFileName, RINGNAMEMAXLEN+1);
		break;
	case MESSAGES:
		strncpy(cMusicFile, g_sm.rMessage.cMusicFileName, RINGNAMEMAXLEN+1);
		break;
	case TMESSAGES:
		strncpy(cMusicFile, g_sm.rTelematics.cMusicFileName, RINGNAMEMAXLEN+1);
		break;
	case ALARMCLK:
		strncpy(cMusicFile, g_sm.rAlarmClk.cMusicFileName, RINGNAMEMAXLEN+1);
		break;
	case CALENDAR:
		strncpy(cMusicFile, g_sm.rCalendar.cMusicFileName, RINGNAMEMAXLEN+1);
		break;
	}

	return TRUE;
}

