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
#include "clock_timezone.h"

DWORD TimeZone()
{
    WNDCLASS	wc;
	DWORD		dwRet = 0;

	
	wc.style         = 0;
	wc.lpfnWndProc   = AppWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ClockTimeZoneClass";
	
	if (!RegisterClass(&wc))
		return -1;
	
	hWndTimeZone = CreateMainWnd();
	
	if (!hWndTimeZone)
		return FALSE;
	ShowWindow(hClockFrameWnd, SW_SHOW);
	UpdateWindow(hClockFrameWnd);

	return dwRet;
}
static LRESULT AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;
	static HMENU hMenu;
	static SYSTEMTIME systime;
	static DWORD wSecond;
	static int iCurIndex;

    switch (message)
    {
    case WM_CREATE:
		{	
			
			HDC hdc;		
			hdc = GetDC(hWnd);
			hBmpHomeZone = LoadImage(NULL, ICON_HOMEZONE, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			hBmpVisitZone = LoadImage(NULL, ICON_VISITZONE, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			hBmpActiveZone = LoadImage(NULL, ICON_ACTIVEZONE, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			ReleaseDC(hWnd, hdc);
		}
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_USE);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)CLOCK_ICON_ARROWLEFT);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Time zones"));
		hMenu = CreateMenu_TimeZoneWnd();
		CreateListBox(hWnd);	
		InitList(hWndList);	
		SetFocus(hWndList);
		SetTimer(hWnd, ID_TIMER, 1000, NULL);
		wSecond = 0;
		break;
	case WM_TIMER:
		wSecond ++;
		if (30 == wSecond) 
		{		
		
			printf("clock_timezone WM_TIMER\r\n");
			UpdateList(hWndList);
			wSecond = 0;
		}
		break;
	case PWM_SHOWWINDOW:
		{		
			SetFocus(hWndList);		
			PDASetMenu(hClockFrameWnd, hMenu);
			SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_USE);
			SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
			SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");	
			SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)CLOCK_ICON_ARROWLEFT);
			SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
			SetWindowText(hClockFrameWnd, ML("Time zones"));		
			printf("clock_timezone PWM_SHOWWINDOW\r\n");
			UpdateList(hWndList);
			
		}
		
		break;
	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_USE:
			{			
				char sTmp[30];
				char cDate[20];
				int meRet;
				TIMEZONE UsingZone;

				GetLocalTime(&systime);
				iCurIndex = SendMessage(hWndList, LB_GETCURSEL, 0, 0 );	
				UsingZone = GetUsingTimeZone();				
		
				if (TZ_HOME == iCurIndex) 
				{
					SetUsingTimeZone(TZ_HOME, &systime);
					strcpy(sTmp, IDS_SUCCESS);
					strcat(sTmp, HOME);
				}
				else if(TZ_VISIT == iCurIndex)
				{
					SetUsingTimeZone(TZ_VISIT, &systime);
					strcpy(sTmp, IDS_SUCCESS);
					strcat(sTmp, VISITING);
				}			
				
				sprintf(cDate, "%02d/%02d/%02d,%02d:%02d:00",
                     systime.wYear%100,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute);
				meRet = ME_SetClockTime(hWnd, CALLBACK_ME,cDate);//set the wierless module first              
                    
				 if(meRet < 0)
				 {
					 printf("clock_timezone ME_SetClockTime failed\r\n");
					 PLXTipsWin(NULL, NULL, NULL, "Failed to set time", NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
					 break;
				 }
				PLXTipsWin(0, 0, 0, sTmp, IDS_CONFIRM_TITLE, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
				printf("clock_timezone ME_SetClockTime succeed\r\n");
		
			
				
			}
			break;
		case ID_EXIT:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			PostMessage(Clock_hWndApp, WM_CLOSE, 0, 0);	
			break;
		case ID_SETCLOCK:
			TimeSetting(hClockFrameWnd);
			break;
		default:
			break;
		}
	
		break;
	case CALLBACK_ME:
		SetLocalTime (&systime);
        DlmNotify((WPARAM)PS_SETTIME, 0);
		printf("clock_timezone CALLBACK_ME succeed\r\n");
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			SendMessage(hWnd, WM_COMMAND, ID_USE, 0);
			break;

		case VK_F10:
			SendMessage(hWnd, WM_COMMAND, ID_EXIT, 0);
			break;
		case VK_LEFT:			
			PostMessage(hWnd, WM_CLOSE, 0, 0);	
			break;
		case VK_F5:
			SendMessage(hClockFrameWnd,WM_KEYDOWN,wParam,lParam);
			break;
		default:
			lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
			break;	
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);	
		break;

	case WM_DESTROY:
		KillTimer(hWnd, ID_TIMER);
		wSecond = 0;
		DeleteObject(hBmpHomeZone);
		DeleteObject(hBmpVisitZone);
		DeleteObject(hBmpActiveZone);
		hBmpHomeZone = NULL;
		hBmpVisitZone = NULL;
		hWndTimeZone = NULL;
		hBmpActiveZone = NULL;
		DestroyMenu(PDAGetMenu(hClockFrameWnd));
		UnregisterClass("ClockTimeZoneClass", hInstance);
		SendMessage(hClockFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		break;

	default:
		lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
		break;
	} 

	return lResult;
}

static HWND CreateMainWnd()
{
	HWND hWnd;
	RECT rClient;


	GetClientRect(hClockFrameWnd,&rClient);
	hWnd = CreateWindow(
		"ClockTimeZoneClass",
		"",//ML("Time zones"),
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hClockFrameWnd,
		NULL,
		NULL,
		NULL
		);

	return hWnd;
}
static void CreateListBox(HWND hWnd)
{
	RECT rccl;

	GetClientRect(hWnd, &rccl);

	hWndList = CreateWindow("LISTBOX",
		"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_MULTILINE | LBS_BITMAP,
		rccl.left, rccl.top,
		rccl.right - rccl.left, rccl.bottom - rccl.top,
		hWnd,
		NULL,
		NULL,
		NULL);
}
static void InitList(HWND hWnd)
{	
	char sHomeZone[ZONELENGTH], sVisitingZone[ZONELENGTH];
	SYSTEMTIME systime;
	char cTime[10];
	char cTmp[30];
	TIMEZONE UsingZone;

	strcpy(sHomeZone, GetHomeZoneStr());
	strcpy(sVisitingZone, GetVisitZoneStr());
	UsingZone = GetUsingTimeZone();  
	SendMessage(hWnd, LB_INSERTSTRING, 0 , (LPARAM)HOME);
	GetLocalTime(&systime);

	if (UsingZone == TZ_VISIT)
	{
		GetNewZoneTime(&systime, sHomeZone, sVisitingZone);
	}
	
	GetTimeDisplay(systime, cTime, NULL);
    strcpy(cTmp, cTime);
	strcat(cTmp, "(");
	strcat(cTmp, sHomeZone);
	strcat(cTmp, ")");
	SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(0, -1), (LPARAM)cTmp);
	SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)hBmpHomeZone);
    printf("InitList cTmpHome = %s\r\n", cTmp);
	SendMessage(hWnd, LB_INSERTSTRING, 1 , (LPARAM)VISITING);
	
	GetLocalTime(&systime);
	if (UsingZone == TZ_HOME)
	{
		GetNewZoneTime(&systime, sVisitingZone, sHomeZone);
	}
	
	GetTimeDisplay(systime, cTime, NULL);
	strcpy(cTmp, cTime);
	strcat(cTmp, "(");
	strcat(cTmp, sVisitingZone);
	strcat(cTmp, ")");
	SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(1, -1), (LPARAM)cTmp);
	SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)hBmpVisitZone);
	  printf("InitList cTmpVisit = %s\r\n", cTmp);
	if (UsingZone == TZ_HOME)
	{	
		SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), 0), (long)hBmpActiveZone);	
	}
	if (UsingZone == TZ_VISIT)
	{		
		SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), 1), (long)hBmpActiveZone);	
	}	

	SendMessage(hWndList, LB_SETCURSEL, 0, 0);
}
static void UpdateList(HWND hWnd)
{	
	char sHomeZone[ZONELENGTH], sVisitingZone[ZONELENGTH];
	SYSTEMTIME systime;
	char cTime[10];
	char cTmp[30];
	TIMEZONE UsingZone;
	
	strcpy(sHomeZone, GetHomeZoneStr());
	strcpy(sVisitingZone, GetVisitZoneStr());
	UsingZone = GetUsingTimeZone();
  
	GetLocalTime(&systime);
	
	if (UsingZone == TZ_VISIT)
	{
		GetNewZoneTime(&systime, sHomeZone, sVisitingZone);
	}
	
	GetTimeDisplay(systime, cTime, NULL);
    strcpy(cTmp, cTime);
	strcat(cTmp, "(");
	strcat(cTmp, sHomeZone);
	strcat(cTmp, ")");
	SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(0, -1), (LPARAM)cTmp);
	SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)hBmpHomeZone);
    printf("UpdateList cTmpHome = %s\r\n", cTmp);	
	GetLocalTime(&systime);
	if (UsingZone == TZ_HOME)
	{
		GetNewZoneTime(&systime, sVisitingZone, sHomeZone);
	}
	
	GetTimeDisplay(systime, cTime, NULL);
	strcpy(cTmp, cTime);
	strcat(cTmp, "(");
	strcat(cTmp, sVisitingZone);
	strcat(cTmp, ")");
	SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(1, -1), (LPARAM)cTmp);
	SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)hBmpVisitZone);
	printf("UpdateList cTmpVisit = %s\r\n", cTmp);
	if (UsingZone == TZ_HOME)
	{	
		SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), 0), (long)hBmpActiveZone);	
		SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), 1), NULL);	
	}
	if (UsingZone == TZ_VISIT)
	{		
		SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), 1), (long)hBmpActiveZone);	
		SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), 0), NULL);	
	}	

}


static HMENU CreateMenu_TimeZoneWnd()
{
	HMENU hMenu;

	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_USE, IDS_USE);
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_SETCLOCK, IDS_SETCLOCK);
	PDASetMenu(hClockFrameWnd, hMenu);
	return hMenu;
}
