/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : AlmMain.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "pollex.h"
#include "window.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "plx_pdaex.h"
#include "pubapp.h"
#include "winpda.h"
#include "hpimage.h"
#include "hpdef.h"

#include "str_public.h"
#include "str_plx.h"
#include "mullang.h"
#include "setting.h"

#include "Almglobal.h"
#include "AlmMain.h"

#include "PMALARM.H"		//程序管理器负责管理闹钟的到时提醒



static HINSTANCE	hInstance = NULL;
static HWND			hWndAlarmList = NULL;
static HWND			hFocus = NULL;
static HBITMAP		hBmp_Selected_Pane;
static HBITMAP		hBmp_Selected_HLight_Pane;
static HBITMAP		hBmp_NoSelected_Pane;
static HBITMAP		hBmp_NoSelected_HLight_Pane;
static HBITMAP		hBmp_Ring;
static HBITMAP		hBmp_HLight_Ring;
static int nTimerID[ALARM_MAX] ={0,0,0};
static int nAlarmID;
static char *cDayOfWeek[7] =
{
	"Su",
	"Mo",
	"Tu",
	"We",
	"Th",
	"Fr",
	"Sa",
};

extern DWORD AlarmSetWnd(int AlarmID, HWND hWnd);
						
static LRESULT AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static HWND CreateMainWnd();
static void GetTimeStr(char *cTime,int nHour, int nMinute);
static void CreateListBox(HWND hWnd);
static void ResetDataInlist();
static void SelchangeInList(HWND hWnd);


DWORD AlarmMain()
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
	wc.lpszClassName = CLASSNAME_MAIN;
	
	if (!RegisterClass(&wc))
		return -1;
	
	hWndAlarmMain = CreateMainWnd();
	
	ShowWindow(hClockFrameWnd, SW_SHOW);
	UpdateWindow(hClockFrameWnd);

    return dwRet;
}

static LRESULT AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	LRESULT		lResult = TRUE;
	

    switch (message)
    {
    case WM_CREATE:
		{
			COLORREF Color;
			BOOL bTrans;
			int i;

			hdc = GetDC(hWnd);		
			hBmp_Ring = CreateBitmapFromImageFile(hdc, IMAGE_RING, &Color, &bTrans);
			hBmp_HLight_Ring = CreateBitmapFromImageFile(hdc, IMAGE_HLIGHT_RING, &Color, &bTrans);
			ReleaseDC(hWnd, hdc);
			
			for(i = 0; i < ALARM_MAX; i++)
				Configure_Get(i, &Alarm_Settings[i]);

			CreateListBox(hWnd);
			SelchangeInList(hWndAlarmList);

			SendMessage(hClockFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_BACK, 1), (LPARAM)"");
			
			SendMessage(hClockFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_BACK, 0), (LPARAM)IDS_BACK);
			SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
			SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
			SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
			SetWindowText(hClockFrameWnd, ML("Alarms"));
			hFocus = hWndAlarmList;
			SetFocus(hWndAlarmList);
		}
		break;
		
	case PWM_SHOWWINDOW:
		
		SetFocus(hWnd);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Alarms"));
		
		break;
	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_EDIT:
			{
				nAlarmID = SendMessage(hWndAlarmList, LB_GETCURSEL, NULL, NULL);
				AlarmSetWnd(nAlarmID ,hClockFrameWnd);
			}
			
			break;
		case ID_BACK:
			{
				int i;
				for(i = 0; i < ALARM_MAX; i++)
				{
					if (Alarm_Settings[i].bIsOpen)
					{
						DlmNotify(PS_ALARMCLOCK, ICON_SET);
						break;
					}
					DlmNotify(PS_ALARMCLOCK, ICON_CANCEL);
				}
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
		
			break;
		case ID_Set_Back:
			ResetDataInlist();
			SelchangeInList(hWndAlarmList);
			break;
		default:
			break;
		}

		switch (HIWORD(wParam))
		{
		case LBN_SELCHANGE:
			SelchangeInList(hWndAlarmList);
			break;
			
		default:
			break;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F5:
			SendMessage(hWnd, WM_COMMAND, ID_EDIT, 0);
			break;
	
		case VK_F10:
			SendMessage(hWnd, WM_COMMAND, ID_BACK, 0);
			break;
		}
		break;

	case WM_CLOSE:
	
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		hWndAlarmMain = NULL;
		nAlarmID = 0;	
		DeleteObject( hBmp_Ring);
		DeleteObject(hBmp_HLight_Ring);
		UnregisterClass(CLASSNAME_MAIN, hInstance);
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

	GetClientRect(hClockFrameWnd, &rClient);
	hWnd = CreateWindow(
		CLASSNAME_MAIN,
		"",//ML("Alarms"),
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
	char cTime[10];
	char cWeek[25];
	int i;

	GetClientRect(hWnd, &rccl);

	hWndAlarmList = CreateWindow("MULTILISTBOX",
		"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_MULTILINE | LBS_BITMAP,
		rccl.left, rccl.top,
		rccl.right - rccl.left, rccl.bottom - rccl.top,
		hWnd,
		(HMENU)ID_ALARMLIST,
		NULL,
		NULL);
	
	for(i = 0; i < ALARM_MAX; i++ )
	{
		GetTimeStr(cTime,Alarm_Settings[i].nHour, Alarm_Settings[i].nMinute);
		SendMessage(hWndAlarmList, LB_INSERTSTRING, i , (LPARAM)cTime);
		SendMessage(hWndAlarmList, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), i), (long)hBmp_Ring);	
		
		
		switch(Alarm_Settings[i].nRepeat) 
		{
		case Repeat_Once:
			SendMessage(hWndAlarmList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)ML("Once"));
			break;
		case Repeat_Daily:
			SendMessage(hWndAlarmList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)ML("Daily"));
			break;
		case Repeat_SelectDays:
			Alarm_GetWeekStr(cWeek, Alarm_Settings[i].nSelectedDay);
			SendMessage(hWndAlarmList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)ML(cWeek));
			break;
		default:
			break;
		}	

	}

	SendMessage(hWndAlarmList, LB_ENDINIT, 0, 0);
	SendMessage(hWndAlarmList, LB_SETCURSEL, 0, 0);
	hFocus = hWndAlarmList;
}
static void GetTimeStr(char *cTime,int nHour, int nMinute)
{
	SYSTEMTIME st;
	char cDate[20];

	memset(&st, 0, sizeof(SYSTEMTIME));
	st.wHour = nHour;
	st.wMinute = nMinute;
	GetTimeDisplay(st, cTime, cDate);
}
void Alarm_GetWeekStr(char *cWeek,int *nSelectedDay)
{
	int i;

	cWeek[0] = '\0';
	
	for(i = 1; i < 7; i++)
	{
		if (nSelectedDay[i])
		{
			if (nSelectedDay[i -1])
			{
				strcat(cWeek, " ");
			}
			
			strcat(cWeek, ML(cDayOfWeek[i]));
		}
		else
			strcat(cWeek, "-");
	}
	if (nSelectedDay[0])
	{
		if (nSelectedDay[6])
		{
			strcat(cWeek, " ");
		}
		strcat(cWeek, ML(cDayOfWeek[0]));
	}
	else
		strcat(cWeek, "-");
}
static void ResetDataInlist()
{
	char cTime[10];
	char cWeek[25];
	
	GetTimeStr(cTime,Alarm_Settings[nAlarmID].nHour, Alarm_Settings[nAlarmID].nMinute);
	SendMessage(hWndAlarmList, LB_DELETESTRING,nAlarmID , 0);
	SendMessage(hWndAlarmList, LB_INSERTSTRING, nAlarmID , (LPARAM)cTime);
	
	if (Alarm_Settings[nAlarmID].nRepeat == Repeat_Once)
	{
		SendMessage(hWndAlarmList, LB_SETAUXTEXT, MAKEWPARAM(nAlarmID, -1), (LPARAM)ML("Once"));
	}
	if (Alarm_Settings[nAlarmID].nRepeat == Repeat_Daily)
	{
		SendMessage(hWndAlarmList, LB_SETAUXTEXT, MAKEWPARAM(nAlarmID, -1), (LPARAM)ML("Daily"));
	}
	if (Alarm_Settings[nAlarmID].nRepeat == Repeat_SelectDays)
	{
		Alarm_GetWeekStr(cWeek, Alarm_Settings[nAlarmID].nSelectedDay);
		SendMessage(hWndAlarmList, LB_SETAUXTEXT, MAKEWPARAM(nAlarmID, -1), (LPARAM)cWeek);
	}	

	if (Alarm_Settings[nAlarmID].bIsOpen)	
		SendMessage(hWndAlarmList, LB_SETSEL, (WPARAM)TRUE, nAlarmID);
	else
		SendMessage(hWndAlarmList, LB_SETSEL, (WPARAM)FALSE, nAlarmID);

	SendMessage(hWndAlarmList, LB_SETCURSEL, nAlarmID, 0);
	SendMessage(hWndAlarmList, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), nAlarmID), (long)hBmp_Ring);


}
static void SelchangeInList(HWND hWnd)
{
	int i;
	int iIndex;
				
	iIndex = SendMessage(hWnd,LB_GETCURSEL,0,0);
	
	for(i = 0; i < ALARM_MAX; i ++)
	{
		if (i == iIndex)
		{
			if (Alarm_Settings[i].bIsOpen)			
				SendMessage(hWnd, LB_SETSEL, (WPARAM)TRUE, i);
			else
				SendMessage(hWnd, LB_SETSEL, (WPARAM)FALSE, i);
			SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), i), (LPARAM)hBmp_HLight_Ring);
		}
		else
		{
			if (Alarm_Settings[i].bIsOpen)			
				SendMessage(hWnd, LB_SETSEL, (WPARAM)TRUE, i);
			else
				SendMessage(hWnd, LB_SETSEL, (WPARAM)FALSE, i);
			SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), i), (LPARAM)hBmp_Ring);

		}
	}
}
