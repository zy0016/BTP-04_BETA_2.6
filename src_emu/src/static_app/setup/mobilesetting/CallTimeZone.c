/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setting / time zone setting
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
#include    "CallTimeZone.h"
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

static HWND hTimeZoneWnd;
static HWND hFrameWin = NULL;
static const char * pTimeStr   = "%02d/%02d/%02d,%02d:%02d:00";
char cdate [20] = "";
char szHomezoneStr[10];
char szVisitzoneStr[11];
BOOL btzsProcessing = FALSE;

static unsigned long RepeatFlag = 0;
static WORD wKeyCode;

static LRESULT  AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL CreateControl(HWND hwnd, HWND *hZoneUsed, HWND *hHomeZone, HWND *hVisitZone);
static int tzs_Month_Days(int year, int month);
void SetUsingTimeZone(TIMEZONE tz, SYSTEMTIME* st);
char* GetHomeZoneStr(void);
char* GetVisitZoneStr(void);

float tzs_TimeStrToFloat(const char *szTimeStr); //format of szTimeStr:(e.g.) "UTC+11:30"
void tzs_IncreaseDay(SYSTEMTIME *lpTD);
void tzs_DecreaseDay(SYSTEMTIME *lpTD);
void GetNewZoneTime(SYSTEMTIME *st_old,const char *szNewZone,const char *szOldZone);

extern BOOL CallSelZoneUsedWindow(HWND hFrame);
extern BOOL CallEditHomeZoneWindow(HWND hFrame);
extern BOOL CallEditVisitZoneWindow(HWND hFrame);
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);

BOOL CallTimeZone(HWND hwndApp)
{
    WNDCLASS    wc;
    RECT rClient;
    hFrameWin = hwndApp;

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

    hTimeZoneWnd = CreateWindow(
        pClassName, 
        NULL,
        WS_VISIBLE | WS_CHILD,                    
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin,
        NULL,
        NULL, 
        NULL
        );
    
    if (NULL == hTimeZoneWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK,(LPARAM)ML("Back"));
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");    
    SetWindowText(hFrameWin, ML("Time zone"));
    SetFocus(hTimeZoneWnd);

    return (TRUE);
}
static LRESULT  AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{    
    static      HWND        hZoneUsed,hHomeZone,hVisitZone;
    static      HWND        hFocus = 0;
    static      int         iButtonJust,iCurIndex;
    static      int         meRet;
	LRESULT     lResult;
    static  SYSTEMTIME  tzs_st;
    char    szTimeZoneStr[10];
    char    *tmp;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE:
        CreateControl(hWnd,&hZoneUsed,&hHomeZone,&hVisitZone);
        SendMessage(hZoneUsed,SSBM_SETCURSEL,GetUsingTimeZone(),0);
        hFocus = hZoneUsed;
        break;

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK,(LPARAM)ML("Back"));
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");    
        SetWindowText(hFrameWin, ML("Time zone"));        
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        
        SendMessage(hZoneUsed,SSBM_SETCURSEL,GetUsingTimeZone(),0);

        strcpy(szTimeZoneStr, GetHomeZoneStr());
        if ( 0 == (tmp = strrchr(szTimeZoneStr, '+')) )
            tmp = strrchr(szTimeZoneStr, '-');
        if ( tmp != 0 )
        SendMessage(hHomeZone, SSBM_DELETESTRING, 0, 0);
        SendMessage(hHomeZone, SSBM_ADDSTRING, 0, (LPARAM)tmp);

        strcpy(szTimeZoneStr, GetVisitZoneStr());
        if ( 0 == (tmp = strrchr(szTimeZoneStr, '+')) )
            tmp = strrchr(szTimeZoneStr, '-');
        if ( tmp != 0 )
        SendMessage(hVisitZone, SSBM_DELETESTRING, 0, 0);
        SendMessage(hVisitZone, SSBM_ADDSTRING, 0, (LPARAM)tmp);
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case WM_DESTROY : 
         hTimeZoneWnd = NULL;
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
             case IDC_ZONEUSED:
                 CallSelZoneUsedWindow(hFrameWin);
                 break;
             case IDC_HOMEZONE:
                 CallEditHomeZoneWindow(hFrameWin);
                 break;
             case IDC_VISITZONE:
                 CallEditVisitZoneWindow(hFrameWin);
                 break;
             }
             break;
         case IDC_ZONEUSED:
             if(HIWORD( wParam ) == SSBN_CHANGE)
             {
                 iCurIndex = SendMessage(hZoneUsed,SSBM_GETCURSEL,0,0);
				 
				 if (GetUsingTimeZone() == iCurIndex)		//re-saving the same setting info;
					 break;

                 memset(&tzs_st,0x00,sizeof(SYSTEMTIME));
                 
                 GetLocalTime(&tzs_st);
                 SetUsingTimeZone(iCurIndex,&tzs_st);
                 sprintf(cdate,pTimeStr,//"%02d/%02d/%02d,%02d:%02d:00"
                     tzs_st.wYear%100,tzs_st.wMonth,tzs_st.wDay,tzs_st.wHour,tzs_st.wMinute);
                 //                 if(btzsProcessing == TRUE)  
                 meRet = ME_SetClockTime(hWnd,CALLBACK_SETDATE,cdate);//set the wierless module first
                 if(meRet < 0)
                 {
					 //PostMessage(hWnd,CALLBACK_SETDATE,0,0);// only for test in emulator
					  PLXTipsWin(NULL, NULL, NULL, "Failed to set time", NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);

				 }
             }
             if(HIWORD(wParam) == SSBN_SETFOCUS)
             {
                 iButtonJust = IDC_ZONEUSED;
                 hFocus = hZoneUsed;
             }
             break; 
         case IDC_HOMEZONE:
             if(HIWORD(wParam) == SSBN_SETFOCUS)
             {
                 iButtonJust = IDC_HOMEZONE;
                 hFocus = hHomeZone;
             }
             break;
         case IDC_VISITZONE:
             if(HIWORD(wParam) == SSBN_SETFOCUS)
             {
                 iButtonJust = IDC_VISITZONE;
                 hFocus = hVisitZone;
             }
             break;
         }
         break;

      case CALLBACK_SETDATE:
          SetLocalTime (&tzs_st);
          DlmNotify((WPARAM)PS_SETTIME, 0);
          break;       

     default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
	return lResult;
}
static BOOL CreateControl(HWND hwnd, HWND *hZoneUsed, HWND *hHomeZone, HWND *hVisitZone)
{
    int     xzero=1,yzero=0;
    int     iControlH,iControlW;
	RECT    rect;
    char    szTimeZoneStr[10];
    char    *tmp;
    GetClientRect(hwnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;

    * hZoneUsed = CreateWindow( "SPINBOXEX", ML("Time zone in use"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hwnd, (HMENU)IDC_ZONEUSED, NULL, NULL);

    if (* hZoneUsed == NULL)
        return FALSE;

    SendMessage(*hZoneUsed,SSBM_ADDSTRING,0,(LPARAM)ML("Home"));
    SendMessage(*hZoneUsed,SSBM_ADDSTRING,0,(LPARAM)ML("Visiting"));

    * hHomeZone = CreateWindow( "SPINBOXEX", ML("Home zone"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH, iControlW, iControlH, 
        hwnd, (HMENU)IDC_HOMEZONE, NULL, NULL);

    if (* hHomeZone == NULL)
        return FALSE;
    
    strcpy(szTimeZoneStr, GetHomeZoneStr());

    tmp = strrchr(szTimeZoneStr, '+');
    
    if ( 0 == tmp )
    {
        tmp = strrchr(szTimeZoneStr, '-');
    }

    if ( tmp != 0 )
        SendMessage(*hHomeZone,SSBM_ADDSTRING,0,(LPARAM)tmp);

    * hVisitZone = CreateWindow( "SPINBOXEX", ML("Visiting zone"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*2, iControlW, iControlH, 
        hwnd, (HMENU)IDC_VISITZONE, NULL, NULL);

    if (* hVisitZone == NULL)
        return FALSE;

    strcpy(szTimeZoneStr, GetVisitZoneStr());

    tmp = strrchr(szTimeZoneStr, '+');
    
    if ( 0 == tmp )
    {
        tmp = strrchr(szTimeZoneStr, '-');
    }
    
    if ( tmp != 0 )
        SendMessage(*hVisitZone,SSBM_ADDSTRING,0,(LPARAM)tmp);
    return TRUE;
}


char* GetHomeZoneStr(void)
{
    FS_GetPrivateProfileString(SN_TIMEZONE, KN_HZSTRING, "UTC+00:00", szHomezoneStr, 10, SETUPFILENAME);
	printf("\r\n To GET home zone string===============> %s\r\n", szHomezoneStr);
    return szHomezoneStr;
}

void SaveHomeZoneStr(const char* szHomeStr)//szHomeStr : "UTC +/- XX:XX"
{
	printf("\r\n To SAVE home zone string===============> %s\r\n", szHomeStr);
    FS_WritePrivateProfileString(SN_TIMEZONE, KN_HZSTRING, (char*)szHomeStr, SETUPFILENAME);
}

void TimeZoneStrToSysTime(SYSTEMTIME * st, const char * szTimeZone)
{
    float timevalue;
    int hour, minute;
    timevalue = tzs_TimeStrToFloat(szTimeZone);
    hour = (int)(timevalue);
    minute = abs((int)((timevalue - hour) * 60));

    if ( (0 == hour) && (timevalue < 0) )
    {
        st->wHour = hour;
        st->wMinute = (WORD)(-minute);
    }
    else
    {
        st->wHour = hour;
        st->wMinute = minute;  
    }    
}

char* GetVisitZoneStr(void)
{
    FS_GetPrivateProfileString(SN_TIMEZONE, KN_VZSTRING, "UTC+00:00", szVisitzoneStr, 10, SETUPFILENAME);
    return szVisitzoneStr;//Finland
}
void SaveVisitZoneStr(const char* szVisitStr) //szVisitStr : "UTC +/- XX:XX"
{
    FS_WritePrivateProfileString(SN_TIMEZONE, KN_VZSTRING,(char*)szVisitStr, SETUPFILENAME);
}

float tzs_TimeStrToFloat(const char *szTimeStr) //format of szTimeStr:(e.g.) "UTC+11:30"
{
    int i;
    char sign;
    float fDecSec;
    float fResult;
    static char szTimeIntSection[3];
    static char szTimeDecSection[3];
    const char ts_Head[4] = {"UTC"};
    if(!strncmp(szTimeStr,ts_Head,3))
        sign = szTimeStr[3];
    for(i=0;i<2;i++)
    {
        szTimeIntSection[i] = szTimeStr[4+i];
        szTimeDecSection[i] = szTimeStr[7+i];
    }
    fDecSec = (float)(atoi(szTimeDecSection)/60.0);
    fResult = atoi(szTimeIntSection) + fDecSec;
    if(sign=='-')
    fResult = -fResult;
    return fResult;
}

void GetNewZoneTime(SYSTEMTIME *st_old,const char *szNewZone,const char *szOldZone)
{
    int ndiff,nTemp,nMinTemp;
    float fOldDiff;
    float fNewDiff;
    int nHour;
 
    fOldDiff = tzs_TimeStrToFloat(szOldZone);

    fNewDiff = tzs_TimeStrToFloat(szNewZone);

    nHour = (int)st_old->wHour;
	nMinTemp = (int)st_old->wMinute;
    
	ndiff = (int)((fNewDiff - fOldDiff)*2);
    nTemp = nHour + ndiff/2;
    
    if (ndiff >= 0)
		nMinTemp = ((ndiff%2)==0)?nMinTemp: nMinTemp+30;
	else
		nMinTemp = ((ndiff%2)==0)?nMinTemp: nMinTemp-30;
	if (nMinTemp >= 60)
	{
		nMinTemp -= 60;
		nTemp += 1;
	}
	if (nMinTemp < 0)
	{
		nMinTemp += 60;
		nTemp -= 1;
	}

	st_old->wMinute = nMinTemp;
	
	//calculate the hour
        while(nTemp >= 24)
        {
            nTemp -= 24;
		    tzs_IncreaseDay(st_old);
        }
	    
        while(nTemp < 0)
        {
            nTemp += 24;
            tzs_DecreaseDay(st_old);
        }
	st_old->wHour = nTemp;

	sprintf(cdate,pTimeStr,//"%02d/%02d/%02d,%02d:%02d:00"
           st_old->wYear%100,st_old->wMonth,st_old->wDay,st_old->wHour,st_old->wMinute);

}

/*======================================================================================================================
*		function name   £º	tzs_IncreaseDay
*		purpose   £º	Increase the day
*		parameter £º	*lpTD         
*		return value   £º	                       
=======================================================================================================================
*/
void tzs_IncreaseDay(SYSTEMTIME *lpTD)
{
	if (lpTD->wDay == tzs_Month_Days(lpTD->wYear, lpTD->wMonth))
	{
		lpTD->wDay = 1;
		if (lpTD->wMonth == 12)
		{
			lpTD->wYear++;
			lpTD->wMonth = 1;
		}
		else lpTD->wMonth++;
	}
	else lpTD->wDay++;
}
/*
/======================================================================================================================
*		function name   £º	tzs_DecreaseDay
*		purpose   £º	Decrease the day value
*		parameter £º	    
*		return value   £º                       
=======================================================================================================================
*/
void tzs_DecreaseDay(SYSTEMTIME *lpTD)
{
	if (lpTD->wDay == 1)
	{
		if (lpTD->wMonth != 1)
		{
			lpTD->wDay = tzs_Month_Days(lpTD->wYear, lpTD->wMonth-1);
			lpTD->wMonth--;
		}
		else
		{
			lpTD->wDay = tzs_Month_Days(lpTD->wYear-1, 12);
			lpTD->wYear--;
			lpTD->wMonth = 12;
		}
	}
	else lpTD->wDay--;
}
/*
*=============================================================================
*		functin Name   £º	Month_Days
*		purpose   £º	Get the days in specific month
*		parameter £º	year      current year
*					month         current month
*		return value   £º	days in the month                      
*=============================================================================
*/
static int tzs_Month_Days(int year, int month)
{
	int days;

    switch (month)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
             days = 31;
             break;

        case 4:
        case 6:
        case 9:
        case 11:
             days = 30;
             break;

        case 2:
             if (((year % 400) == 0) || (((year % 4) == 0) && ((year % 100) != 0)))
                 days = 29;
             else
                 days = 28;
             break;
    }

    return days;
}
