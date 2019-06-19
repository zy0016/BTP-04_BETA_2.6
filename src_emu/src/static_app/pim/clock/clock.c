/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#include "clock.h"
extern BOOL GetAppCallInfo(void);
extern DWORD TempWindow(int nID);
static BOOL bNoRealQuit = FALSE;
static BOOL bCurShortCut = FALSE;
/**********************************************************************
* Function	 Clock_AppControl
* Purpose    application main function
* Params	
* Return	
* Remarks
**********************************************************************/
DWORD Clock_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
	WNDCLASS	    wc;
	DWORD		    dwRet;
	RECT			rClient;

    dwRet = TRUE;
    switch (nCode)
    {
    case APP_INIT:
		{
//			char i;
			hInstance = (HINSTANCE)pInstance;	
			
			RTC_RegisterApp("Clock");
			
			Configure_Init();		
			
			Configure_SetIcon();	

		/*
			for(i=0; i<3; i++)//reset alarm according to setting file when no power or get off battery
					{
						Configure_Get(i, &Alarm_Settings[i]);		
						ResetNode(i, 0, &Alarm_Settings[i]);
					
					}*/
		
		}
        break;

   case APP_GETOPTION:
		switch (wParam)
		{
		case AS_STATE:
			break;
		case AS_HELP:
			break;
		case AS_APPWND:
			dwRet = (DWORD)Clock_hWndApp;
			break;
		}
		break;

	case APP_SETOPTION:
		break;

	case APP_ALARM:
		{
			switch (wParam)
			{
			case ALMN_TIME_OUT:
				{				
					int id;					
					BOOL bAlarmPoweron = FALSE;
					SYSTEMTIME systime;
					
					id = LOWORD(lParam);
					printf(" ********** <clock ID: %d> ALMN_TIME_OUT! *************** \r\n", id);
					GetLocalTime(&systime);
					Configure_Get(id, &Alarm_Settings[id]);
				
					if(GetAppCallInfo())
						TempWindow(id);
					else
						
					{					
						bAlarmPoweron = IsAlarmPowerOn();				
						if(Alarm_Settings[id].nSnooze > Snooze_Off)
							AlarmWindow(id, 1, bAlarmPoweron, 0);
						else
							AlarmWindow(id, 0, bAlarmPoweron, 0);		
						
					}				
				
				}
				
				break;	

			case ALMN_SYSTIME_CHANGE:
				{
					int id;
					printf(" ********** <clock> ALMN_TIME_CHANGE! *************** \n");
					for(id=0; id<3; id++)
					{
						Configure_Get(id, &Alarm_Settings[id]);	
						if(Alarm_Settings[id].bSnooze == FALSE)
							ResetNode(id, 0, &Alarm_Settings[id]);
					}		
				
				}

				break;
		
			case ALMN_SETERROR:
				{	
					int id;						
					id = LOWORD(lParam);
					printf(" ********** <clock> ALMN_SETERROR! *****ID: %d***** \r\n", id);
					PLXTipsWin(0, 0, 0, ML("Set alarm failed!"), ML("Clock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
					Configure_Get(id, &Alarm_Settings[id]);
					Alarm_Settings[id].bIsOpen = FALSE;
					Configure_Set(id, Alarm_Settings[id]);
				
					break;
				}		
					
			}		
		}
		break;



    case APP_ACTIVE:

	
	////////////////////for test/////////////////////
	/*
		{				
						int id;					
						BOOL bAlarmPoweron = FALSE;
						
						printf(" ********** <clock> ALMN_TIME_OUT! *************** \n");
						
						id = 0;
						Configure_Get(id, &Alarm_Settings[id]);
						bAlarmPoweron = IsAlarmPowerOn();
						bAlarmPoweron = TRUE;
					/ *
						if(Alarm_Settings[id].nSnooze > Snooze_Off)
											AlarmWindow(id, 1, bAlarmPoweron, 0);
					
										else
											AlarmWindow(id, 0, bAlarmPoweron, 0);		* /
					
						AlarmWindow(2, 1, bAlarmPoweron, 0);
						AlarmWindow(0, 1, bAlarmPoweron, 0);
						AlarmWindow(1, 1, bAlarmPoweron, 0);
					}
					
					break;	*/
		
		
		
		
	/*
	
			AlarmWindow(1, 1, 1, 0);
				AlarmWindow(2, 0, 1, 0);
				AlarmWindow(3, 1, 1, 0);							
			
				break;*/
	
	
		
		////////////////////for test/////////////////////	
		if ( lParam == 1 )		//favourite & shortcut for logs
		{
			if(bCurShortCut)
			{
				ShowWindow(hClockFrameWnd, SW_SHOW);
				ShowOwnedPopups(hClockFrameWnd, SW_SHOW);
				UpdateWindow(hClockFrameWnd);
			}		
			else
			{
				if (hClockFrameWnd)
				{	
					bNoRealQuit = TRUE;
					DestroyWindow(hClockFrameWnd);				
					hClockFrameWnd = NULL;			
					
				}
				bCurShortCut = TRUE;
				wc.style         = 0;
				wc.lpfnWndProc   = AppWndProc;
				wc.cbClsExtra    = 0;
				wc.cbWndExtra    = 0;
				wc.hInstance     = NULL;
				wc.hIcon         = NULL;
				wc.hCursor       = NULL;
				wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
				wc.lpszMenuName  = NULL;
				wc.lpszClassName = "ClockWndClass";
				if (!RegisterClass(&wc))
					return FALSE;
				
				hClockFrameWnd = CreateFrameWindow( WS_CAPTION | PWS_STATICBAR);
				GetClientRect(hClockFrameWnd,&rClient);
				
				Clock_hWndApp = CreateWindow(
					"ClockWndClass",
					"",//ML("Clock")
					WS_VISIBLE | WS_CHILD,
					rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
					hClockFrameWnd, NULL, NULL, NULL);
				
				if (!Clock_hWndApp)
					return FALSE;
				
				AlarmMain();	
			}
		
			break;
		}
	
		if(IsWindow(hClockFrameWnd))
		{
			ShowWindow(hClockFrameWnd, SW_SHOW);
			ShowOwnedPopups(hClockFrameWnd, SW_SHOW);
			UpdateWindow(hClockFrameWnd);
		}
		else
		{
			wc.style         = 0;
			wc.lpfnWndProc   = AppWndProc;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = NULL;
			wc.hIcon         = NULL;
			wc.hCursor       = NULL;
			wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
			wc.lpszMenuName  = NULL;
			wc.lpszClassName = "ClockWndClass";
			if (!RegisterClass(&wc))
				return FALSE;

			hClockFrameWnd = CreateFrameWindow( WS_CAPTION | PWS_STATICBAR);
			GetClientRect(hClockFrameWnd,&rClient);
			
			Clock_hWndApp = CreateWindow(
				"ClockWndClass",
				"",//ML("Clock")
				WS_VISIBLE | WS_CHILD,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hClockFrameWnd, NULL, NULL, NULL);

			if (!Clock_hWndApp)
				return FALSE;
			
			ShowWindow(hClockFrameWnd, SW_SHOW);
			UpdateWindow(hClockFrameWnd);
			hFocus = Clock_hWndApp;
			SetFocus(hFocus);
		}
        break;

    case APP_INACTIVE:
		ShowOwnedPopups(hClockFrameWnd, SW_HIDE);
        ShowWindow(hClockFrameWnd,SW_HIDE); 
        break;

    default:
        break;
    }

    return dwRet;
}
/**********************************************************************
* Function	MainWndProc
* Purpose   main window function
* Params
* Return	
* Remarks
**********************************************************************/
static LRESULT AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;
	static char cTime[TIME_LENTH];
	static char cDate[DATE_LENTH];
	static char cZone[ZONE_LENTH];
	static char cTime_Alarm[TIME_LENTH];
	static char cDate_Alarm[DATE_LENTH];
	static char cDateWeek[WEEKDATE_LENTH];
	static int nDayOfWeek_Alarm = 0;
	HDC hdc;
	SYSTEMTIME  systime_alarm;
	static HMENU hMenu;

		
	switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			SYSTEMTIME systime;
		
		
			SetWindowText(hClockFrameWnd, ML("Clock"));
			SetFocus(hWnd);
			GetLocalTime(&systime);
			GetLocalTimeStr(cTime, cDate, cZone);
			hMenu = CreateMenu_MainWnd();
			SetTimer(hWnd, ID_TIMER, 1000, NULL);
	
			{
				RTCTIME rtctime;
				unsigned short nID;
				int retval;
				
				retval = RTC_GetFirstAppAlarm("Clock", &rtctime, &nID);
				if(retval == 0)
				{
				
					systime_alarm.wDay = (WORD)rtctime.v_nDay;
					systime_alarm.wDayOfWeek = (WORD)rtctime.v_nDayOfWeek;
					systime_alarm.wHour = (WORD) rtctime.v_nHour;
					systime_alarm.wMinute = (WORD)rtctime.v_nMinute;
					systime_alarm.wMonth = (WORD) rtctime.v_nMonth;
					systime_alarm.wYear = (WORD) rtctime.v_nYear;

					bNextAlarm = TRUE;
					GetAlarmDisplayStr(&systime_alarm, cTime_Alarm, cDate_Alarm, &nDayOfWeek_Alarm);
					
				}
				else
					bNextAlarm = FALSE;
			}
		
				
			{
				HDC hdc;
			/*
				COLORREF Color;
							BOOL bTrans;*/
			
				
				hdc = GetDC(hWnd);
				hBmpHomeZone = LoadImage(NULL, ICON_HOMEZONE, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
				hBmpVisitZone = LoadImage(NULL, ICON_VISITZONE, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
				hBmpRing = LoadImage(NULL, ICON_RING, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			//	hBmpRightArrow = CreateBitmapFromImageFile(hdc, ICON_RIGHTARROW, &Color, &bTrans);
				ReleaseDC(hWnd, hdc);
			}
			
			SendMessage(hClockFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_ALARM, 1), (LPARAM)IDS_ALARM);
			SendMessage(hClockFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_EXIT, 0), (LPARAM)IDS_EXIT);
			SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
			//SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)hBmpRightArrow);		
			SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)CLOCK_ICON_ARROWRIGHT);
			SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		}
		break;
		
	case WM_PAINT:
		{
			HFONT hSmallFont, hOldFont;
			int  OldStyle; 
			RECT rect;
			TIMEZONE UsingZone;
			 COLORREF BkColor;
		
		
			BeginPaint(hWnd, NULL);
			hdc = GetDC(hWnd);
			GetClientRect(hWnd, &rect);
			ClearRect(hdc, &rect, COLOR_TRANSBK);
			OldStyle = SetBkMode(hdc, BM_TRANSPARENT);//不画背景色	
	
		    rLocalTime.left = rect.left;
			rLocalTime.right = rect.right;
			rLocalZone.left = rect.left;
			rLocalZone.right = rect.right;
			rLocalDate.left = rect.left;
			rLocalDate.right = rect.right;
			rAlarmCaption.left = rect.left;
			rAlarmCaption.right = rect.right;
			rAlarmTime.left = rect.left;
			rAlarmTime.right = rect.right;
			rAlarmDate.left = rect.left;
			rAlarmDate.right = rect.right;
			
			DrawText(hdc, ML(cTime), -1, &rLocalTime, DT_CENTER);//Large Font
			if (bNextAlarm) 
				DrawText(hdc, ML(cTime_Alarm), -1, &rAlarmTime, DT_CENTER);//Large Font		
			GetFontHandle(&hSmallFont, SMALL_FONT);
			hOldFont = SelectObject(hdc, hSmallFont);
			DrawText(hdc, ML(cZone), -1, &rLocalZone, DT_CENTER);
			memset(cDateWeek, 0, WEEKDATE_LENTH);
			strcat(cDateWeek, ML(cWeek[LocalTimeData.week]));
			strcat(cDateWeek + 2, " ");
			strcat(cDateWeek, cDate);
			DrawText(hdc, cDateWeek, -1, &rLocalDate, DT_CENTER);		
			if (bNextAlarm)
			{
				DrawText(hdc, ML("Next alarm:"),-1, &rAlarmCaption,DT_CENTER);			
				memset(cDateWeek, 0, WEEKDATE_LENTH);
				strcat(cDateWeek, ML(cWeek[nDayOfWeek_Alarm]));
				strcat(cDateWeek + 2, " ");
				strcat(cDateWeek, cDate_Alarm);						
				DrawText(hdc, cDateWeek, -1, &rAlarmDate, DT_CENTER);
				BitBlt(hdc, rIcon2.left, rIcon2.top, 
					rIcon2.right - rIcon2.left, rIcon2.bottom - rIcon2.top, (HDC)hBmpRing, 0, 0, ROP_SRC);
			}
			else
			{
				DrawText(hdc, ML("No alarms"),-1, &rAlarmCaption, DT_CENTER);
			}
			SelectObject(hdc ,hOldFont);
			SetBkMode(hdc, OldStyle);
		
			OldStyle = SetBkMode(hdc, BM_NEWTRANSPARENT);//由SetBkColor指定的颜色不画，当前是黑色
			BkColor = SetBkColor(hdc, COLOR_BLACK);
			UsingZone = GetUsingTimeZone();
			
			if (UsingZone == TZ_HOME)
			{
				BitBlt(hdc, rIcon1.left, rIcon1.top, 
				rIcon1.right - rIcon1.left, rIcon1.bottom - rIcon1.top, (HDC)hBmpHomeZone, 0, 0, ROP_SRC);
			}
			else
			{
				BitBlt(hdc, rIcon1.left, rIcon1.top, 
				rIcon1.right - rIcon1.left, rIcon1.bottom - rIcon1.top, (HDC)hBmpVisitZone, 0, 0, ROP_SRC);
			}
			
			SetBkMode(hdc, OldStyle);
			SetBkColor(hdc, BkColor);
			ReleaseDC(hWnd, hdc);
			EndPaint(hWnd, NULL);
		
			
		}
		
		break;
		
	case PWM_SHOWWINDOW:	
		{
			RTCTIME rtctime;
			unsigned short nID;
			int retval;
			
			retval = RTC_GetFirstAppAlarm("Clock", &rtctime, &nID);
			if(retval == 0)
			{
				typedef struct _SYSTEMTIME 
				{  
					WORD wYear;     
					WORD wMonth; 
					WORD wDayOfWeek;     
					WORD wDay;     
					WORD wHour;     
					WORD wMinute; 
					WORD wSecond;     
					WORD wMilliseconds; 
				} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME; 
				systime_alarm.wDay = (WORD)rtctime.v_nDay;
				systime_alarm.wDayOfWeek = (WORD)rtctime.v_nDayOfWeek;
				systime_alarm.wHour = (WORD) rtctime.v_nHour;
				systime_alarm.wMinute = (WORD)rtctime.v_nMinute;
				systime_alarm.wMonth = (WORD) rtctime.v_nMonth;
				systime_alarm.wYear = (WORD) rtctime.v_nYear;
				bNextAlarm = TRUE;
				GetAlarmDisplayStr(&systime_alarm, cTime_Alarm, cDate_Alarm, &nDayOfWeek_Alarm);
				
			}
			else
					bNextAlarm = FALSE;
		}
		GetLocalTimeStr(cTime, cDate, cZone);
		SetFocus(hWnd);
		PDASetMenu(hClockFrameWnd, hMenu);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_ALARM);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
	//	SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)hBmpRightArrow);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)CLOCK_ICON_ARROWRIGHT);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SetWindowText(hClockFrameWnd,ML("Clock"));
		
	//	SetTimer(hWnd, ID_TIMER, 1000, NULL);
		break;

	case WM_TIMER:
		{
			static int iSencond = 0;
		
			iSencond ++;
			if (iSencond > 30) 
			{
			//	GetLocalTime(&systime);			
				GetLocalTimeStr(cTime, cDate, cZone);
				InvalidateRect(hWnd, &rClock, FALSE);
				iSencond = 0;
			}
		}
		
		
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case ID_ALARM:
				AlarmMain();			
				break;
			case ID_TIMEZONE:
				TimeZone();			
				break;
			case ID_SETCLOCK:
				TimeSetting(hClockFrameWnd);			
				break;
	
			default:
				break;
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{	
			case VK_F10:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				SendMessage(hClockFrameWnd, PWM_CLOSEWINDOW, (WPARAM) hWnd, 0);
				break;
			case VK_RETURN:
				SendMessage(hWnd, WM_COMMAND, ID_ALARM, 0);
				break;
			case VK_RIGHT:
				SendMessage(hWnd, WM_COMMAND, ID_TIMEZONE, 0);
				break;
			case VK_F5:
				SendMessage(hClockFrameWnd,WM_KEYDOWN,wParam,lParam);
				break;
				
			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
		}
		break;
		
	case WM_CLOSE:
	
		DestroyWindow(hWnd);
	
		break;

    case WM_DESTROY:
		KillTimer(hWnd, ID_TIMER);
		DeleteObject(hBmpRing);
		DeleteObject(hBmpVisitZone);
		DeleteObject(hBmpHomeZone);
	//	DeleteObject(hBmpRightArrow);
		hWndAlarmMain = NULL;
		hWndAlarmSetMain = NULL;
		Clock_hWndApp = NULL;
		hClockFrameWnd = NULL;
		DestroyMenu(PDAGetMenu(hClockFrameWnd));
		UnregisterClass("ClockWndClass", NULL);
		SendMessage(hClockFrameWnd, PWM_CLOSEWINDOW, 0, 0);
		if(!bNoRealQuit)
			DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance); 
		bNoRealQuit = FALSE;
		bCurShortCut = FALSE;


 		break;
	default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

	return lResult;
}

/**********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void GetLocalTimeStr(char *cTime, char *cDate, char *cZone)
{
	SYSTEMTIME	SysTime;

	memset(&SysTime, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&SysTime);	
	LocalTimeData.day = SysTime.wDay;
	LocalTimeData.month = SysTime.wMonth;
	LocalTimeData.year = SysTime.wYear;
	LocalTimeData.hour = SysTime.wHour;
	LocalTimeData.minute = SysTime.wMinute;
	LocalTimeData.week = SysTime.wDayOfWeek;
	LocalTimeData.zone = GetUsingTimeZone();
	
	GetTimeDisplay(SysTime, cTime, cDate);
//	GetZoneStr(LocalTimeData.zone, cZone);
	if (TZ_HOME ==LocalTimeData.zone)
		strcpy(cZone, GetHomeZoneStr());
	if (TZ_VISIT ==LocalTimeData.zone)
		strcpy(cZone, GetVisitZoneStr());
	
	wSecond = SysTime.wSecond;
}

/**********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int GetWeekValue(SYSTEMTIME *pSystime)
{
	
	DWORD   wYear, wMonth, wDay;
	BOOL    bLunar = FALSE;
	short	Days[12] = {31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
	int     iDays, iStart,iYearStart;

    wYear = pSystime->wYear;
	wMonth = pSystime->wMonth;
	wDay = pSystime->wDay;
	
	if((wYear % 4 == 0) && (wYear % 100 != 0) || (wYear % 400 == 0))
	{
		bLunar = 1;
	}

    if(wYear >= 1980)
    {
        iStart = (wYear - 1980) * 365 + (wYear - 1980) / 4 + 2 + 1;
        if((wYear % 4 == 0) && (wYear % 100 != 0) || (wYear % 400 == 0))
        {
            iStart -= 1;
        }
    }
    else
    {
        iStart = (1980 - wYear) * 365 + (1980 - wYear) / 4 + 2 + 1;
        if((wYear % 4 == 0) && (wYear % 100 != 0) || (wYear % 400 == 0))
        {
            iStart -= 1;
        }
    }
    iDays = Days[wMonth - 1];
    iYearStart = (bLunar ? iStart + 1 : iStart) % 7;

    if(wMonth != 1)
    {
        iStart += Days[wMonth - 2];
        iDays = Days[wMonth - 1] - Days[wMonth - 2];
        if(wMonth == 2 && bLunar)
            iDays ++;
    }
    if(bLunar && ((wYear <= 1998) ||
        ((wYear > 1998) && (wMonth > 2))))
    {
		iStart ++;
    }
	iStart +=(wDay -1); 
    iStart %= 7;

	return iStart;
}



/**********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static HMENU CreateMenu_MainWnd()
{
	HMENU hMenu;
	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_ALARM, IDS_ALARM);
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_SETCLOCK, IDS_SETCLOCK);
	PDASetMenu(hClockFrameWnd, hMenu);
	return hMenu;
}
/**********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void GetAlarmDisplayStr(SYSTEMTIME *pTime, char *cTime, char *cDate, int *nDayOfWeek)
{
	
	SYSTEMTIME systime_alarm;
	
	GetLocalTime(&systime_alarm);
	systime_alarm.wYear =(WORD) pTime->wYear;
	systime_alarm.wMonth = (WORD) pTime->wMonth;
	systime_alarm.wDay = (WORD) pTime->wDay;
	systime_alarm.wHour = (WORD) pTime->wHour;
	systime_alarm.wMinute = (WORD) pTime->wMinute;
	
	*nDayOfWeek = GetWeekValue(pTime);

	GetTimeDisplay(systime_alarm, cTime, cDate);
}
