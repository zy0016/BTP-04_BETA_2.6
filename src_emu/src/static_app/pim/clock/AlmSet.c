  /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : AlmSet.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "plx_pdaex.h"
#include "pubapp.h"
#include "winpda.h"
#include "mullang.h"
#include "setting.h"
#include "PMALARM.H"

#include "Almglobal.h"
#include "AlmSet.h"

static HWND hWndAlarmSwitch;
static HWND hWndAlarmTime;
static HWND hWndAlarmRepeat;
static HWND hWndAlarmSnooze;
static int nAlarmID = 0;
static HWND hWndFocus = NULL;
static int  nCurFocus;
static int iTimerUp = 0;
static int iTimerDown = 0;

//static int AlarmSet_Get(AlarmSetting *pSetting);
static LRESULT AlarmSet_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL AlarmSet_RegisterClass(void);
static void AlarmSet_CreateMain(HWND hWndParent);
static void CreateAlarmSwitch(HWND hWnd);
static void CreateTimeView(HWND hWnd);
static void CreateRepeat(HWND hWnd);
static void CreateSnooze(HWND hWnd);
static void RenewControlsData();
static void OnVScroll(HWND hWnd,  UINT wParam);
static void InitVScroll(HWND hWnd,int iItemNum);

static void OnDownProcess(HWND hWnd);
static void OnUpProcess(HWND hWnd);
static void OnTimeProcess(HWND hWnd, WPARAM wParam);

extern DWORD AlarmSwitch(int nID, HWND hWnd);
extern DWORD AlarmTime(int nID, HWND hWnd);
extern DWORD AlarmRepeat(int nID, HWND hWnd);
extern DWORD AlarmSnooze(int nID, HWND hWnd);



extern AlarmSetting	 Alarm_Settings[ALARM_MAX];

DWORD AlarmSetWnd(int nID, HWND hWndParent)
{
	if (!AlarmSet_RegisterClass())
		return -1;
	
	nAlarmID = nID;
	AlarmSet_CreateMain(hWndParent);
	ShowWindow(hWndParent, SW_SHOW);
	UpdateWindow(hWndParent);
	return 0;
}

static BOOL AlarmSet_RegisterClass(void)
{
	WNDCLASS wc;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AlarmSet_WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = CLASSNAME_ALARMSET;

	return RegisterClass(&wc);
}

static void AlarmSet_CreateMain(HWND hWndParent)
{
	RECT	rClient;

	GetClientRect(hWndParent, &rClient);
	hWndAlarmSetMain = CreateWindow(
		CLASSNAME_ALARMSET,
		"",//"Alarm",
		WS_VISIBLE | WS_CHILD |WS_VSCROLL,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hWndParent,
		NULL,
		NULL,
		NULL);
}
static void CreateAlarmSwitch(HWND hWndParent)
{	
	hWndAlarmSwitch = CreateWindow(
        "SPINBOXEX",
        ML("Alarm:"),
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT |CS_NOSYSCTRL,
		ALARM_SWITCH_POSITION,
        hWndParent,
        (HMENU)ID_AlarmSwitch,
        NULL,
        NULL);
	
	SendMessage(hWndAlarmSwitch, SSBM_ADDSTRING, NULL, (LPARAM)ML("Off"));
	SendMessage(hWndAlarmSwitch, SSBM_ADDSTRING, NULL, (LPARAM)ML("On"));
	
	if (Alarm_Settings[nAlarmID].bIsOpen)
		SendMessage(hWndAlarmSwitch, SSBM_SETCURSEL, 1, 0);
	else
		SendMessage(hWndAlarmSwitch, SSBM_SETCURSEL, 0, 0);

	hWndFocus = hWndAlarmSwitch;
}
static void CreateTimeView(HWND hWndParent)
{
	SYSTEMTIME systime;
	char cTime[10];
	char cDate[20];

	hWndAlarmTime = CreateWindow(
        "SPINBOXEX",
        ML("Alarm time:"),
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | SSBS_LEFT |SSBS_ARROWRIGHT | CS_NOSYSCTRL,
		ALARM_TIME_POSITION,
        hWndParent,
        (HMENU)ID_AlarmTime,
        NULL,
        NULL);

	GetLocalTime(&systime);
	systime.wHour = Alarm_Settings[nAlarmID].nHour;
	systime.wMinute = Alarm_Settings[nAlarmID].nMinute;
	GetTimeDisplay(systime, cTime, cDate);
	SendMessage(hWndAlarmTime, SSBM_ADDSTRING, NULL, (LPARAM)cTime);	
}
static void  CreateRepeat(HWND hWndParent)
{
	char cWeek[25];

	hWndAlarmRepeat = CreateWindow(
        "SPINBOXEX",
        ML("Repeat:"),
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
		ALARM_REPEAT_POSTION,
        hWndParent,
        (HMENU)ID_AlarmRepeat,
        NULL,
        NULL);		

	switch(Alarm_Settings[nAlarmID].nRepeat) 
	{
	case Repeat_Once:
		SendMessage(hWndAlarmRepeat, SSBM_ADDSTRING, NULL, (LPARAM)ML("Once"));
		break;
	case Repeat_Daily:
		SendMessage(hWndAlarmRepeat, SSBM_ADDSTRING, NULL, (LPARAM)ML("Daily"));
		break;
	case Repeat_SelectDays:
		Alarm_GetWeekStr(cWeek, Alarm_Settings[nAlarmID].nSelectedDay);
		SendMessage(hWndAlarmRepeat, SSBM_ADDSTRING, NULL, (LPARAM)ML(cWeek));
		break;
	default:
		break;
	}
}

static void CreateSnooze(HWND hWndParent)
{
	hWndAlarmSnooze = CreateWindow(
        "SPINBOXEX",
        ML("Snooze:"),
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT |CS_NOSYSCTRL,
		ALARM_SNOOZE_POSIOTN,
        hWndParent,
        (HMENU)ID_AlarmSnooze,
        NULL,
        NULL);
	SendMessage(hWndAlarmSnooze, SSBM_ADDSTRING, 0, (LPARAM)ML("Off"));
	SendMessage(hWndAlarmSnooze, SSBM_ADDSTRING, 1, (LPARAM)ML("5 Min"));
	SendMessage(hWndAlarmSnooze, SSBM_ADDSTRING, 2, (LPARAM)ML("10 Min"));
	SendMessage(hWndAlarmSnooze, SSBM_ADDSTRING, 3, (LPARAM)ML("30 Min"));

	switch(Alarm_Settings[nAlarmID].nSnooze)
	{
	case Snooze_Off:
		SendMessage(hWndAlarmSnooze, SSBM_SETCURSEL, (WPARAM)0, NULL);
		break;
	case Snooze_5Min:
		SendMessage(hWndAlarmSnooze, SSBM_SETCURSEL, (WPARAM)1, NULL);
		break;
	case Snooze_10Min:
		SendMessage(hWndAlarmSnooze, SSBM_SETCURSEL, (WPARAM)2, NULL);
		break;
	case Snooze_30Min:
		SendMessage(hWndAlarmSnooze, SSBM_SETCURSEL, (WPARAM)3, NULL);
		break;
	default:
		break;
	}
}

static LRESULT AlarmSet_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;

	switch (message)
	{
	case WM_CREATE:
		CreateAlarmSwitch(hWnd);
		CreateTimeView(hWnd);
		CreateRepeat(hWnd);
		CreateSnooze(hWnd);
		InitVScroll(hWnd, 4);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Alarm"));
		SetFocus(hWndFocus);

		break;

	case PWM_SHOWWINDOW:
		
		SetFocus(hWndFocus);
		RenewControlsData();
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Alarm"));
		
		break;

	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;


	case WM_SETFOCUS:
		SetFocus(hWndFocus);
		break;
	case WM_KILLFOCUS:
		if (iTimerDown)
		{
			KillTimer(hWnd, TIMERID_DOWN);
			iTimerDown = 0;
		}
		if (iTimerUp)
		{
			KillTimer(hWnd, TIMERID_UP);
			iTimerUp = 0;
		}
		break;

	case WM_VSCROLL:
	    OnVScroll(hWnd,wParam);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F10:
			SendMessage(hWnd, WM_COMMAND, ID_BACK, 0);
            break;
		case VK_F5:
			{
				HWND hFocus;
				hFocus = GetFocus();
				hFocus = GetParent(hFocus);
				if (hWndAlarmSwitch == hFocus) 
				{
					AlarmSwitch(nAlarmID, hClockFrameWnd);
					break;
				}
				if (hWndAlarmTime == hFocus) 
				{
					AlarmTime(nAlarmID, hClockFrameWnd);
					break;
				}
				if (hWndAlarmRepeat == hFocus) 
				{
					AlarmRepeat(nAlarmID, hClockFrameWnd);
					break;
				}
				if (hWndAlarmSnooze == hFocus) 
				{
					AlarmSnooze(nAlarmID, hClockFrameWnd);
					break;
				}
			}
			break;
		case VK_DOWN:
			OnDownProcess(hWnd);
			SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
			iTimerDown = SetTimer(hWnd, TIMERID_DOWN, TIMER_FIRST_ELAPSE, NULL);

			break;
		case VK_UP:
			OnUpProcess(hWnd);
			SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
			iTimerUp = SetTimer(hWnd, TIMERID_UP, TIMER_FIRST_ELAPSE, NULL);
			
			break;
        default:
			lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
			break;
        }
		break;
	case WM_KEYUP:
		switch(LOWORD(wParam))
		{
		case VK_UP:
			if (iTimerUp)
			{
				KillTimer(hWnd, TIMERID_UP);
				iTimerUp = 0;
			}
			break;
		case VK_DOWN:
			if (iTimerDown)
			{
				KillTimer(hWnd, TIMERID_DOWN);
				iTimerDown = 0;
			}
			break;
		default:
			break;
		}        
		break;
	case WM_TIMER:
		OnTimeProcess(hWnd, wParam);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_BACK:
			{
				 SYSTEMTIME systime;
				GetLocalTime(&systime);		
				ResetNode(nAlarmID, 0, &Alarm_Settings[nAlarmID]);
				SendMessage(hWndAlarmMain, WM_COMMAND, ID_Set_Back, 0);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;			
		}

		switch(HIWORD(wParam)) 
		{
		case SSBN_CHANGE:
			{
				HWND hFocus;
				int  nIndex_Switch = 0;
				int  nIndex_Repeat = 0;
				int	 nIndex_Snooze = 0;
		
				
				hFocus = GetFocus();
				if (hWndAlarmTime != hFocus) 
				{
					hFocus = GetParent(hFocus);
					if (hWndAlarmSwitch == hFocus)
					{
						nIndex_Switch = SendMessage(hWndAlarmSwitch, SSBM_GETCURSEL, NULL, NULL);
						
						if (nIndex_Switch)
							Alarm_Settings[nAlarmID].bIsOpen = TRUE;
						else
							Alarm_Settings[nAlarmID].bIsOpen = FALSE;						
				
										
					}			
				
					if (hWndAlarmSnooze == hFocus)
					{
						nIndex_Snooze = SendMessage(hWndAlarmSnooze, SSBM_GETCURSEL, 0, 0);
						switch (nIndex_Snooze)
						{
						case 0:
							Alarm_Settings[nAlarmID].nSnooze = Snooze_Off;
							break;
						case 1:
							Alarm_Settings[nAlarmID].nSnooze = Snooze_5Min;
							break;
						case 2:
							Alarm_Settings[nAlarmID].nSnooze = Snooze_10Min;
							break;
						case 3:
							Alarm_Settings[nAlarmID].nSnooze = Snooze_30Min;
							break;
						default:
							break;
						}				
					
					}
				}
				break;
			}
		default:
			break;
		}
			
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		iTimerDown = 0;
		iTimerUp = 0;
		UnregisterClass(CLASSNAME_ALARMSET, NULL);
		hWndAlarmSetMain = NULL;
		break;

	default:
		lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return lResult;
}



static void RenewControlsData()
{
	SYSTEMTIME systime;
	char cTime[10];
	char cDate[20];
	char cWeek[20];

	if (Alarm_Settings[nAlarmID].bIsOpen)
		SendMessage(hWndAlarmSwitch, SSBM_SETCURSEL, 1, 0);
	else
		SendMessage(hWndAlarmSwitch, SSBM_SETCURSEL, 0, 0);

	GetLocalTime(&systime);
	systime.wHour = Alarm_Settings[nAlarmID].nHour;
	systime.wMinute = Alarm_Settings[nAlarmID].nMinute;
	GetTimeDisplay(systime, cTime, cDate);
	
	SendMessage(hWndAlarmTime, SSBM_SETTEXT, 0, (LPARAM)cTime);
	
	switch(Alarm_Settings[nAlarmID].nSnooze)
	{
	case Snooze_Off:
		SendMessage(hWndAlarmSnooze, SSBM_SETCURSEL, (WPARAM)0, NULL);
		break;
	case Snooze_5Min:
		SendMessage(hWndAlarmSnooze, SSBM_SETCURSEL, (WPARAM)1, NULL);
		break;
	case Snooze_10Min:
		SendMessage(hWndAlarmSnooze, SSBM_SETCURSEL, (WPARAM)2, NULL);
		break;
	case Snooze_30Min:
		SendMessage(hWndAlarmSnooze, SSBM_SETCURSEL, (WPARAM)3, NULL);
		break;
	default:
		break;
	}
	
	switch(Alarm_Settings[nAlarmID].nRepeat) 
	{
	case Repeat_Once:
		SendMessage(hWndAlarmRepeat, SSBM_SETTEXT, 0, (LPARAM)ML("Once"));
		break;
	case Repeat_Daily:
		SendMessage(hWndAlarmRepeat, SSBM_SETTEXT, 0, (LPARAM)ML("Daily"));
		break;
	case Repeat_SelectDays:
		Alarm_GetWeekStr(cWeek, Alarm_Settings[nAlarmID].nSelectedDay);
		SendMessage(hWndAlarmRepeat, SSBM_SETTEXT, 0, (LPARAM)ML(cWeek));
		break;
	default:
		break;
	}
}

static void OnVScroll(HWND hWnd,  UINT wParam)
{
    int  nY;
    RECT rcClient;
    SCROLLINFO      vsi;

    switch(wParam)
    {
    case SB_LINEDOWN:
		memset(&vsi, 0, sizeof(SCROLLINFO));
		
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus++;
		
        if(nCurFocus > vsi.nMax)	
        {
            ScrollWindow(hWnd,0,(vsi.nMax-vsi.nPage+1)*nY,NULL,NULL);	
            UpdateWindow(hWnd);
            nCurFocus = 0;
			vsi.nPos = 0;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
	
        if(((int)(vsi.nPos + vsi.nPage - 1) <= nCurFocus) && nCurFocus != vsi.nMax)	//modified for UISG
        { 
            ScrollWindow(hWnd,0,-nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos++;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
		break;
		
    case SB_LINEUP:
		
        memset(&vsi, 0, sizeof(SCROLLINFO));
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
		
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus--;
		
        if(nCurFocus < vsi.nMin)	
        {
            ScrollWindow(hWnd,0,(vsi.nPage-vsi.nMax-1)*nY,NULL,NULL);
			
            UpdateWindow(hWnd);
			nCurFocus = vsi.nMax;
		    vsi.nPos = vsi.nMax-vsi.nPage+1;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;	
        }
		
        if((int)vsi.nPos == nCurFocus && nCurFocus != vsi.nMin)	//modified for UISG
        { 
            ScrollWindow(hWnd,0,nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos--;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);   
            break;	
        }
		break;
    case SB_PAGEDOWN:
        break;
    case SB_PAGEUP:
        break;
    default:
        break;	
    }	
}

static void InitVScroll(HWND hWnd,int iItemNum)
{
    SCROLLINFO   vsi;

    memset(&vsi, 0, sizeof(SCROLLINFO));

    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 3;
    vsi.nMax   = (iItemNum-1);
    vsi.nPos   = 0;

    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
	nCurFocus =0;
	return;
}

static void OnDownProcess(HWND hWnd)
{
	hWndFocus = GetFocus();
			
	while(GetParent(hWndFocus) != hWnd)
		hWndFocus = GetParent(hWndFocus);
	
	hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, FALSE);
	SetFocus(hWndFocus);
}

static void OnUpProcess(HWND hWnd)
{
	hWndFocus = GetFocus();
	
	while(GetParent(hWndFocus) != hWnd)
		hWndFocus = GetParent(hWndFocus);
	
	hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, TRUE);
	SetFocus(hWndFocus);
}

static void OnTimeProcess(HWND hWnd, WPARAM wParam)
{
	    switch(wParam)
		{
		case TIMERID_DOWN:

			KillTimer(hWnd, TIMERID_DOWN);
			iTimerDown = 0;
			iTimerDown = SetTimer(hWnd, TIMERID_DOWN, TIMER_SECOND_ELAPSE, NULL);
			OnDownProcess(hWnd);
			SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);

			break;
		case TIMERID_UP:

			KillTimer(hWnd, TIMERID_UP);
			iTimerUp = 0;
			iTimerUp = SetTimer(hWnd, TIMERID_UP, TIMER_SECOND_ELAPSE, NULL);
			OnUpProcess(hWnd);
			SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);

			break;
       default:
			KillTimer(hWnd, wParam);
            break;
		}
}
