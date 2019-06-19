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
#include "string.h"

#include "pubapp.h"
#include "plx_pdaex.h"
#include "mullang.h"
#include "setting.h"

#include "Almglobal.h"

#define IDS_CANCEL	ML("Cancel")
#define IDS_SAVE	ML("Save")

#define ID_CANCEL			1
#define ID_RADIO1			2
#define ID_RADIO2			3
#define ID_RADIO3			4
#define ID_RADIO4			5
#define ID_DAYSLIST			6
#define ID_ALARMTIME		7
#define ID_SAVE				8
#define NOSELECT_OK			WM_USER+ 301

#define TIMERID_DOWN			1
#define TIMERID_UP				2

#define RadioHeight			29
#define RadioWidth			174

#define Time_Separator			":"

static HWND hWndAlarmSwitchMain;
static HWND hWndAlarmRepeatMain;
static HWND hWndAlarmSnoozeMain;
static HWND hWndAlarmSelectDaysMain;
static HWND hWndAlarmTimeMain;
static HWND hWndAlarmDaysList;
static HWND hWndRadio_Off;
static HWND hWndRadio_On;
static HWND hWndRadio_Once;
static HWND hWndRadio_SelectDays;
static HWND hWndRadio_Daily;
static HWND hWndRadio_5Min;
static HWND hWndRadio_10Min;
static HWND hWndRadio_30Min;
static HWND hWndTimeEdit;

static char *cWeek[7]=
{
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday",
};

static int nAlarmID;
static int iTimerDown = 0;
static int iTimerUp = 0;


extern AlarmSetting	 Alarm_Settings[ALARM_MAX];

DWORD AlarmSwitch(int nID, HWND hWnd);
DWORD AlarmRepeat(int nID, HWND hWnd);
DWORD AlarmSnooze(int nID, HWND hWnd);
DWORD AlarmSelectDays(int nID, HWND hWnd);
DWORD AlarmTime(int nID, HWND hWnd);
static  LRESULT AlarmSwitch_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static  LRESULT AlarmRepeat_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static  LRESULT AlarmSnooze_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static  LRESULT AlarmSelectDays_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static  LRESULT AlarmTime_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static HWND CreateAlarmSwitch(HWND hWndParent);
static HWND CreateAlarmRepeat(HWND hWndParent);
static HWND CreateAlarmSnooze(HWND hWndParent);
static HWND CreateAlarmSelectDays(HWND hWndParent);
static void CreateAlarmTime(HWND hWndParent);

static void OnDownProcess(HWND hWnd);
static void OnUpProcess(HWND hWnd);
static void OnTimeProcess(HWND hWnd, WPARAM wParam);


DWORD AlarmSwitch(int nID, HWND hWnd)
{
	WNDCLASS	wc;
	RECT		rClient; 

	nAlarmID = nID;
    wc.style         = 0;
    wc.lpfnWndProc   = AlarmSwitch_WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "AlarmSwitch_WndClass";


	if (!RegisterClass(&wc))
	{
		UnregisterClass("AlarmSwitch_WndClass", NULL);
		return FALSE;
	}
	
	GetClientRect(hWnd, &rClient);
	hWndAlarmSwitchMain = CreateWindow(
		"AlarmSwitch_WndClass",
		"",//ML("Alarm"),
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hClockFrameWnd, NULL, NULL, NULL);
	
	if (!hWndAlarmSwitchMain)
		return FALSE;
	return TRUE;
}

static  LRESULT AlarmSwitch_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;
	static HWND hFocus;

	switch (message)
    {
    case WM_CREATE:
		hFocus = CreateAlarmSwitch(hWnd);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Alarm"));
		SetFocus(hFocus);
		break;
		
	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;

	case PWM_SHOWWINDOW:
		SetFocus(hFocus);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Alarm"));
		
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case ID_CANCEL:
				{
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					SendMessage(hClockFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				}
				break;

			default:
				break;
		}
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

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{	
			case VK_F10:
				SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				break;
			case VK_F5:
				{
//					SYSTEMTIME systime;
					hFocus = GetFocus();
					SendMessage(hFocus, BM_SETCHECK, BST_CHECKED, 0);
					if (hFocus == hWndRadio_Off) 
						SendMessage(hWndRadio_On, BM_SETCHECK, BST_UNCHECKED, 0);
					else
						SendMessage(hWndRadio_Off, BM_SETCHECK, BST_UNCHECKED, 0);
					
					if(BST_CHECKED == SendMessage(hWndRadio_Off, BM_GETCHECK, 0, 0))
					{
						Alarm_Settings[nAlarmID].bIsOpen = FALSE;
					}
					if (BST_CHECKED == SendMessage(hWndRadio_On, BM_GETCHECK, 0, 0)) 
					{
						Alarm_Settings[nAlarmID].bIsOpen = TRUE;
					}

				
					SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				}
				break;
			case VK_DOWN:
				{
					OnDownProcess(hWnd);
					iTimerDown = SetTimer(hWnd, TIMERID_DOWN, TIMER_FIRST_ELAPSE, NULL);
				}
				break;
			case VK_UP:
				{
					OnUpProcess(hWnd);
					iTimerUp = SetTimer(hWnd, TIMERID_UP, TIMER_FIRST_ELAPSE, NULL);
				}
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
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

    case WM_DESTROY:
		iTimerDown = 0;
		iTimerUp = 0;
		UnregisterClass("AlarmSwitch_WndClass", NULL);
		hFocus = NULL;
		break;
	default:
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
	}

	return lResult;
}



static void OnDownProcess(HWND hWnd)
{
	HWND hFocus = NULL;
	HWND hNextFocus = NULL;

	hFocus = GetFocus();
	
	hNextFocus = GetNextDlgTabItem(hWnd, hFocus, FALSE);

	SetFocus(hNextFocus);

}
static void OnUpProcess(HWND hWnd)
{
	HWND hFocus = NULL;
	HWND hPreFocus = NULL;

	hFocus = GetFocus();
	
	hPreFocus = GetNextDlgTabItem(hWnd, hFocus, TRUE);

	SetFocus(hPreFocus);
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

			break;
		case TIMERID_UP:

			KillTimer(hWnd, TIMERID_UP);
			iTimerUp = 0;
			iTimerUp = SetTimer(hWnd, TIMERID_UP, TIMER_SECOND_ELAPSE, NULL);
			OnUpProcess(hWnd);

			break;
       default:
			KillTimer(hWnd, wParam);
            break;
		}

}
static HWND CreateAlarmSwitch(HWND hWndParent)
{
	RECT rc;
	HWND hFocus;

	GetClientRect(hWndParent, &rc);
	hWndRadio_On = CreateWindow(
		"BUTTON",
		ML("On"),
		BS_AUTORADIOBUTTON | WS_VISIBLE | WS_CHILD | WS_TABSTOP |CS_NOSYSCTRL,
		0, 0, RadioWidth, RadioHeight,
		hWndParent,
		(HMENU)ID_RADIO1,                                                                                       //控件标识号 
		NULL,
		NULL);

	hWndRadio_Off = CreateWindow(
		"BUTTON",
		ML("Off"),
		BS_AUTORADIOBUTTON | WS_VISIBLE | WS_CHILD | WS_TABSTOP |CS_NOSYSCTRL,
		0,RadioHeight, RadioWidth, RadioHeight,
		hWndParent,
		(HMENU)ID_RADIO2,
		NULL, 
		NULL);
	if(Alarm_Settings[nAlarmID].bIsOpen)
	{
		SendMessage(hWndRadio_On, BM_SETCHECK, BST_CHECKED, 0);
		hFocus = hWndRadio_On;
	}
	else
	{
		SendMessage(hWndRadio_Off, BM_SETCHECK, BST_CHECKED, 0);
		hFocus = hWndRadio_Off;
	}
	return hFocus;
}

DWORD AlarmRepeat(int nID, HWND hWnd)
{
	WNDCLASS wc;
	RECT rClient;

	nAlarmID = nID;

    wc.style         = 0;
    wc.lpfnWndProc   = AlarmRepeat_WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "AlarmRepeat_WndClass";


	if (!RegisterClass(&wc))
	{
		UnregisterClass("AlarmRepeat_WndClass", NULL);
		return FALSE;
	}
	GetClientRect(hWnd, &rClient);
	hWndAlarmRepeatMain = CreateWindow(
		"AlarmRepeat_WndClass",
		"",//ML("Alarm"),
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hClockFrameWnd, NULL, NULL, NULL);
	
	if (!hWndAlarmRepeatMain)
		return FALSE;
	
	return TRUE;
}

static  LRESULT AlarmRepeat_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;
	static	HWND hFocus;
	switch (message)
    {
    case WM_CREATE:
		hFocus = CreateAlarmRepeat(hWnd);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Repeat"));
		SetFocus(hFocus);
		break;
		
	case PWM_SHOWWINDOW:
		SetFocus(hFocus);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Repeat"));
		
		break;
		
	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case ID_CANCEL:
				{				

					int i;
					
					if(BST_CHECKED == SendMessage(hWndRadio_Once, BM_GETCHECK, 0, 0))	
					{
						Alarm_Settings[nAlarmID].nRepeat = Repeat_Once;
					}
					
					if (BST_CHECKED == SendMessage(hWndRadio_Daily, BM_GETCHECK, 0, 0))
					{
						Alarm_Settings[nAlarmID].nRepeat = Repeat_Daily;
					}
					
					if (Alarm_Settings[nAlarmID].nRepeat < Repeat_SelectDays)
					{
						for(i = 0; i < 7 ; i++)
							Alarm_Settings[nAlarmID].nSelectedDay[i] = 0;
					}				
					
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					SendMessage(hClockFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				}
				
				break;
				
			default:
				break;
		}
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

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{	
			case VK_F10:
				SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				break;
			case VK_F5:
				{					
					hFocus = GetFocus();
					if (hWndRadio_Once == hFocus)
					{
						SendMessage(hFocus, BM_SETCHECK, BST_CHECKED, 0);
						SendMessage(hWndRadio_SelectDays, BM_SETCHECK, BST_UNCHECKED, 0);
						SendMessage(hWndRadio_Daily, BM_SETCHECK, BST_UNCHECKED, 0);
					}
					if (hWndRadio_Daily == hFocus) 
					{
						SendMessage(hFocus, BM_SETCHECK, BST_CHECKED, 0);
						SendMessage(hWndRadio_Once, BM_SETCHECK, BST_UNCHECKED, 0);
						SendMessage(hWndRadio_SelectDays, BM_SETCHECK, BST_UNCHECKED, 0);
					}

					if (hWndRadio_SelectDays == hFocus)
					{
						SendMessage(hFocus,BM_SETCHECK, BST_CHECKED, 0);
						SendMessage(hWndRadio_Once, BM_SETCHECK, BST_UNCHECKED, 0);
						SendMessage(hWndRadio_Daily, BM_SETCHECK, BST_UNCHECKED, 0);
						AlarmSelectDays(nAlarmID, hWnd);
						PostMessage(hWnd, WM_CLOSE, 0, 0);
						SendMessage(hClockFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
						break;

					}
					SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				}
				
				break;
			case VK_DOWN:
				{
					OnDownProcess(hWnd);
					iTimerDown = SetTimer(hWnd, TIMERID_DOWN, TIMER_FIRST_ELAPSE, NULL);
				}
				break;
			case VK_UP:
				{
					OnUpProcess(hWnd);
					iTimerUp = SetTimer(hWnd, TIMERID_UP, TIMER_FIRST_ELAPSE, NULL);
				}
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
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

    case WM_DESTROY:
		iTimerDown = 0;
		iTimerUp = 0;
		UnregisterClass("AlarmRepeat_WndClass", NULL);
		hFocus = NULL;
		break;
	default:
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
	}

	return lResult;
}
static HWND CreateAlarmRepeat(HWND hWndParent)
{
	HWND hFocus;
	
	hWndRadio_Once = CreateWindow(
		"BUTTON",
		ML("Once"),
		BS_AUTORADIOBUTTON | WS_VISIBLE | WS_CHILD |WS_TABSTOP | CS_NOSYSCTRL,// 该按钮窗口为单选框
		0, 0, RadioWidth,RadioHeight,
		hWndParent,
		(HMENU)ID_RADIO1,                                                                                       //控件标识号 
		NULL,
		NULL);

	hWndRadio_SelectDays = CreateWindow(
		"BUTTON",
		ML("Select Days"),
		BS_AUTORADIOBUTTON | WS_VISIBLE | WS_CHILD |WS_TABSTOP| CS_NOSYSCTRL, 
		0,RadioHeight, RadioWidth, RadioHeight,
		hWndParent,
		(HMENU)ID_RADIO2,
		NULL, 
		NULL);

	hWndRadio_Daily = CreateWindow(
		"BUTTON",
		ML("Daily"),
		BS_AUTORADIOBUTTON | WS_VISIBLE | WS_CHILD |WS_TABSTOP| CS_NOSYSCTRL, 
		0, 2*RadioHeight, RadioWidth, RadioHeight,
		hWndParent,
		(HMENU)ID_RADIO3,
		NULL, 
		NULL);
	
	switch(Alarm_Settings[nAlarmID].nRepeat) 
	{
	case Repeat_Once:
		SendMessage(hWndRadio_Once, BM_SETCHECK, BST_CHECKED, 0);
		hFocus = hWndRadio_Once;
		break;
	case Repeat_SelectDays:
		SendMessage(hWndRadio_SelectDays, BM_SETCHECK, BST_CHECKED, 0);
		hFocus = hWndRadio_SelectDays;
		break;

	case Repeat_Daily:
		SendMessage(hWndRadio_Daily, BM_SETCHECK, BST_CHECKED, 0);
		hFocus = hWndRadio_Daily;
		break;
	default:
		break;
	}
	return hFocus;
}

DWORD AlarmSnooze(int nID, HWND hWnd)
{
	WNDCLASS wc;
	RECT rClient;

	nAlarmID = nID;

    wc.style         = 0;
    wc.lpfnWndProc   = AlarmSnooze_WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "AlarmSnooze_WndClass";


	if (!RegisterClass(&wc))
	{
		UnregisterClass("AlarmSnooze_WndClass", NULL);
		return FALSE;
	}

	GetClientRect(hWnd, &rClient);
	hWndAlarmSnoozeMain = CreateWindow(
		"AlarmSnooze_WndClass",
		"",//ML("Alarm"),
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hClockFrameWnd, NULL, NULL, NULL);
	
	if (!hWndAlarmSnoozeMain)
		return FALSE;
	
	return TRUE;
}

static  LRESULT AlarmSnooze_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;
	static HWND hFocus;
	
	switch (message)
    {
    case WM_CREATE:
		hFocus = CreateAlarmSnooze(hWnd);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Snooze"));
		SetFocus(hFocus);
		break;
		
	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;

	case PWM_SHOWWINDOW:
		SetFocus(hFocus);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Snooze"));
		
		break;
		
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case ID_CANCEL:
				{			
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					SendMessage(hClockFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				}
				break;
			default:
				break;
		}
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

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{	
			case VK_F10:
				SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				break;
			case VK_F5:
				{
					hFocus = GetFocus();
					if (hWndRadio_Off == hFocus) 
					{
						SendMessage(hFocus, BM_SETCHECK, BST_CHECKED, 0);
						SendMessage(hWndRadio_5Min,BM_SETCHECK, BST_UNCHECKED, 0 );
						SendMessage(hWndRadio_10Min,BM_SETCHECK, BST_UNCHECKED, 0 );
						SendMessage(hWndRadio_30Min,BM_SETCHECK, BST_UNCHECKED, 0 );
					}
					if (hWndRadio_5Min == hFocus) 
					{
						SendMessage(hFocus, BM_SETCHECK, BST_CHECKED, 0);
						SendMessage(hWndRadio_Off,BM_SETCHECK, BST_UNCHECKED, 0 );
						SendMessage(hWndRadio_10Min,BM_SETCHECK, BST_UNCHECKED, 0 );
						SendMessage(hWndRadio_30Min,BM_SETCHECK, BST_UNCHECKED, 0 );
					}
					
					if (hWndRadio_10Min == hFocus) 
					{
						SendMessage(hFocus, BM_SETCHECK, BST_CHECKED, 0);
						SendMessage(hWndRadio_5Min,BM_SETCHECK, BST_UNCHECKED, 0 );
						SendMessage(hWndRadio_Off,BM_SETCHECK, BST_UNCHECKED, 0 );
						SendMessage(hWndRadio_30Min,BM_SETCHECK, BST_UNCHECKED, 0 );	
					}
					if (hWndRadio_30Min == hFocus) 
					{
						SendMessage(hFocus, BM_SETCHECK, BST_CHECKED, 0);
						SendMessage(hWndRadio_5Min,BM_SETCHECK, BST_UNCHECKED, 0 );
						SendMessage(hWndRadio_10Min,BM_SETCHECK, BST_UNCHECKED, 0 );
						SendMessage(hWndRadio_Off,BM_SETCHECK, BST_UNCHECKED, 0 );
					}
					
					
					if(BST_CHECKED == SendMessage(hWndRadio_Off, BM_GETCHECK, 0, 0))	
						Alarm_Settings[nAlarmID].nSnooze = Snooze_Off;
					if (BST_CHECKED == SendMessage(hWndRadio_5Min, BM_GETCHECK, 0, 0))
						Alarm_Settings[nAlarmID].nSnooze = Snooze_5Min;
					if (BST_CHECKED == SendMessage(hWndRadio_10Min, BM_GETCHECK, 0, 0))
						Alarm_Settings[nAlarmID].nSnooze = Snooze_10Min;
					if (BST_CHECKED == SendMessage(hWndRadio_30Min, BM_GETCHECK, 0, 0))
						Alarm_Settings[nAlarmID].nSnooze = Snooze_30Min;
				

					SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				}
				break;

			case VK_DOWN:
				{
					OnDownProcess(hWnd);
					iTimerDown = SetTimer(hWnd, TIMERID_DOWN, TIMER_FIRST_ELAPSE, NULL);
				}
				break;
			case VK_UP:
				{
					OnUpProcess(hWnd);
					iTimerUp = SetTimer(hWnd, TIMERID_UP, TIMER_FIRST_ELAPSE, NULL);
				}
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
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

    case WM_DESTROY:
		iTimerDown = 0;
		iTimerUp = 0;
		UnregisterClass("AlarmSnooze_WndClass", NULL);
		hFocus = NULL;
		break;
	default:
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
	}

	return lResult;
}
static HWND CreateAlarmSnooze(HWND hWndParent)
{
	HWND hFocus;

	hWndRadio_Off = CreateWindow(
		"BUTTON",
		ML("Off"),
		BS_AUTORADIOBUTTON | WS_VISIBLE | WS_CHILD |WS_TABSTOP | CS_NOSYSCTRL, 
		0,0, RadioWidth, RadioHeight,
		hWndParent,
		(HMENU)ID_RADIO1,                                                                                       //控件标识号 
		NULL,
		NULL);

	hWndRadio_5Min = CreateWindow(
		"BUTTON",
		ML("5 min"),
		BS_AUTORADIOBUTTON | WS_VISIBLE | WS_CHILD |WS_TABSTOP | CS_NOSYSCTRL, 
		0,RadioHeight, RadioWidth, RadioHeight,
		hWndParent,
		(HMENU)ID_RADIO2,
		NULL, 
		NULL);

	hWndRadio_10Min = CreateWindow(
		"BUTTON",
		ML("10 min"),
		BS_AUTORADIOBUTTON | WS_VISIBLE | WS_CHILD|WS_TABSTOP | CS_NOSYSCTRL, 
		0,2*RadioHeight, RadioWidth, RadioHeight,
		hWndParent,
		(HMENU)ID_RADIO3,
		NULL, 
		NULL);
	hWndRadio_30Min = CreateWindow(
		"BUTTON",
		ML("30 min"),
		BS_AUTORADIOBUTTON | WS_VISIBLE | WS_CHILD |WS_TABSTOP | CS_NOSYSCTRL, 
		0,3 * RadioHeight, RadioWidth, RadioHeight,
		hWndParent,
		(HMENU)ID_RADIO4,
		NULL, 
		NULL);

	switch(Alarm_Settings[nAlarmID].nSnooze) 
	{
	case Snooze_Off:
		SendMessage(hWndRadio_Off, BM_SETCHECK, BST_CHECKED, 0);
		hFocus = hWndRadio_Off;
		break;
	case Snooze_5Min:
		SendMessage(hWndRadio_5Min, BM_SETCHECK, BST_CHECKED, 0);
		hFocus = hWndRadio_5Min;
		break;
	case Snooze_10Min:
		SendMessage(hWndRadio_10Min, BM_SETCHECK, BST_CHECKED, 0);
		hFocus = hWndRadio_10Min;
		break;
	case Snooze_30Min:
		SendMessage(hWndRadio_30Min, BM_SETCHECK, BST_CHECKED, 0);
		hFocus = hWndRadio_30Min;
		break;
	default:
		break;
	}

	return hFocus;
}

DWORD AlarmSelectDays(int nID, HWND hWnd)
{
	WNDCLASS wc;
	RECT rClient;

	nAlarmID = nID;

    wc.style         = 0;
    wc.lpfnWndProc   = AlarmSelectDays_WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "AlarmSelectDays_WndClass";


	if (!RegisterClass(&wc))
	{
		UnregisterClass("AlarmSelectDays_WndClass", NULL);
		return FALSE;
	}
	GetClientRect(hWnd, &rClient);
	hWndAlarmSelectDaysMain = CreateWindow(
		"AlarmSelectDays_WndClass",
		"",//ML("Alarm"),
		WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hClockFrameWnd, NULL, NULL, NULL);
	
	if (!hWndAlarmSelectDaysMain)
		return FALSE;
	
	
	return TRUE;
}

static  LRESULT AlarmSelectDays_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;

	switch (message)
    {
    case WM_CREATE:
		CreateAlarmSelectDays(hWnd);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Select days"));
		SetFocus(hWndAlarmDaysList);
		break;
		
	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;

	case PWM_SHOWWINDOW:
		SetFocus(hWndAlarmDaysList);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Select days"));
		break;
		
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case ID_CANCEL:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				SendMessage(hClockFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			
				break;
			case ID_SAVE:
				{
					int i;
//					SYSTEMTIME systime;
					int iIndexItem[7]; 
					BOOL bSelected = FALSE;

					for(i = 0; i < 7; i++)
					{
						iIndexItem[i] = SendMessage(hWndAlarmDaysList, LB_GETSEL, (WPARAM)i, 0);
						if (iIndexItem[i] > 0)
						{
							bSelected = TRUE;
						}
					}
					if (!bSelected) 
					{
						PLXTipsWin(hClockFrameWnd, hWnd, NOSELECT_OK, ML("Please select day"), ML("Select days"), Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
						break;
					}

					if (iIndexItem[6] > 0)
						Alarm_Settings[nAlarmID].nSelectedDay[0] = 1;
					else
						Alarm_Settings[nAlarmID].nSelectedDay[0] = 0;
					
					for(i = 0; i < 6; i++)
					{
						if(iIndexItem[i])
							Alarm_Settings[nAlarmID].nSelectedDay[i +1] = 1;
						else
							Alarm_Settings[nAlarmID].nSelectedDay[i + 1] = 0;
					}
					
					Alarm_Settings[nAlarmID].nRepeat = Repeat_SelectDays;					
			
				
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					SendMessage(hClockFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				}
				break;

			default:
				break;
		}
		break;

		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{	
		case VK_F10:
			SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
			break;
		case VK_RETURN:
			SendMessage(hWnd, WM_COMMAND, ID_SAVE, 0);
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
		UnregisterClass("AlarmSelectDays_WndClass", NULL);
		break;
	default:
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
	}

	return lResult;
}
static HWND CreateAlarmSelectDays(HWND hWndParent)
{
	int i;
	RECT rccl;
	
	GetClientRect(hWndParent, &rccl);
	hWndAlarmDaysList = CreateWindow("MULTILISTBOX",
		ML("Alarm"),
		WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_SORT| LBS_BITMAP,
		rccl.left, rccl.top,
		rccl.right - rccl.left, rccl.bottom - rccl.top,
		hWndParent,
		(HMENU)ID_DAYSLIST,
		NULL,
		NULL);

	for(i = 0; i < 6; i++)
	{
		SendMessage(hWndAlarmDaysList, LB_INSERTSTRING, i,(long)ML(cWeek[i + 1]));
	}
	
	SendMessage(hWndAlarmDaysList, LB_INSERTSTRING, 6,(long)ML(cWeek[0]));
	

	SendMessage(hWndAlarmDaysList, LB_ENDINIT, 0, 0);
	
	for(i = 0; i < 6; i++)
	{
		if (Alarm_Settings[nAlarmID].nSelectedDay[i + 1])
			SendMessage(hWndAlarmDaysList, LB_SETSEL, (WPARAM)1, i);
	}

	if (Alarm_Settings[nAlarmID].nSelectedDay[0])
		SendMessage(hWndAlarmDaysList, LB_SETSEL, (WPARAM)1, 6);

	SendMessage(hWndAlarmDaysList, LB_SETCURSEL, 0, 0);

	return hWndAlarmDaysList;
}
DWORD AlarmTime(int nID, HWND hWnd)
{
	WNDCLASS wc;
	RECT rClient;

	nAlarmID = nID;

    wc.style         = 0;
    wc.lpfnWndProc   = AlarmTime_WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "AlarmTime_WndClass";


	if (!RegisterClass(&wc))
	{
		UnregisterClass("AlarmTime_WndClass", NULL);
		return FALSE;
	}
	GetClientRect(hClockFrameWnd,&rClient);
	hWndAlarmTimeMain = CreateWindow(
		"AlarmTime_WndClass",
		"",//ML("Alarm"),
		WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hClockFrameWnd, NULL, NULL, NULL);
	
	if (!hWndAlarmTimeMain)
		return FALSE;
	
	
	return TRUE;	
}
static  LRESULT AlarmTime_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;
	
	switch (message)
    {
    case WM_CREATE:
		CreateAlarmTime(hWnd);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Alarm"));
		SetFocus(hWndTimeEdit);
		SendMessage(hWndTimeEdit, EM_SETSEL, -1, -1);
		break;
		
	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;

	case PWM_SHOWWINDOW:
		SetFocus(hWndTimeEdit);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SendMessage(hClockFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
		SetWindowText(hClockFrameWnd, ML("Alarm"));
		
		break;
		
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case ID_CANCEL:
				PostMessage(hWnd , WM_CLOSE, 0, 0);
				SendMessage(hClockFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				break;
			case ID_SAVE:
				{
					SYSTEMTIME st;

					memset(&st,0,sizeof(SYSTEMTIME));
					SendMessage(hWndTimeEdit, TEM_GETTIME, 0, (LPARAM)&st);
					Alarm_Settings[nAlarmID].nHour = st.wHour;
					Alarm_Settings[nAlarmID].nMinute = st.wMinute;		

					PostMessage(hWnd , WM_CLOSE, 0, 0);
					SendMessage(hClockFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				}
				break;
			default:
				break;
		}
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
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{	
			case VK_F10:
				SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				break;
			case VK_RETURN:
				SendMessage(hWnd, WM_COMMAND, ID_SAVE, 0);
				break;
			case VK_DOWN:
				{
					OnDownProcess(hWnd);
					iTimerDown = SetTimer(hWnd, TIMERID_DOWN, TIMER_FIRST_ELAPSE, NULL);
				}
				break;
			case VK_UP:
				{
					OnUpProcess(hWnd);
					iTimerUp = SetTimer(hWnd, TIMERID_UP, TIMER_FIRST_ELAPSE, NULL);
				}
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
	case WM_SETRBTNTEXT:
        SendMessage(hClockFrameWnd, PWM_SETBUTTONTEXT, 0, lParam);
		break;
		
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

    case WM_DESTROY:
		iTimerDown = 0;
		iTimerUp = 0;
		UnregisterClass("AlarmTime_WndClass", NULL);
		break;
	default:
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
	}

	return lResult;
}

static void CreateAlarmTime(HWND hWndParent)
{
	SYSTEMTIME st;

	memset((void*)&st, 0, sizeof(SYSTEMTIME));
	
	GetLocalTime(&st);
	st.wHour = Alarm_Settings[nAlarmID].nHour;
	st.wMinute = Alarm_Settings[nAlarmID].nMinute;

	if (TF_24 == GetTimeFormt())
	{	
		hWndTimeEdit = CreateWindow(
			"TIMEEDIT",
			ML("Alarm time:"),
			WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_TIME_24HR | TES_TITLE,
			1,3,176, 50,
			hWndParent,
			(HMENU)ID_ALARMTIME,
			NULL,
			(LPVOID)&st);
	}
	else
	{
		hWndTimeEdit = CreateWindow(
			"TIMEEDIT",
			ML("Alarm time:"),
			WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_TIME_12HR | TES_TITLE,
			1,3,176, 50,
			hWndParent,
			(HMENU)ID_ALARMTIME,
			NULL,
			(LPVOID)&st);
	}
	
}
