/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : global.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#define ALARM_MAX		3

#define APP_ALARM				100

#define ME_SET					WM_USER+ 300
#define Repeat_Once				0
#define Repeat_Daily			1
#define Repeat_SelectDays		2

#define Snooze_Off			0
#define Snooze_5Min			5
#define Snooze_10Min		10
#define Snooze_30Min		30

typedef struct AlarmSettingtag
{
	BOOL	bIsOpen;
	int		nHour;
	int		nMinute;
	int		nRepeat;
	int		nSelectedDay[7];
	int		nSnooze;
	int     bSnooze;
	unsigned char nFlag;
}AlarmSetting;
/*
typedef struct tagAlarmPRI
{
	int nID;
	int nHour;
	int nMinute;
}ALARM_PRI;*/


typedef struct tagAlarmLastTime
{
	int nID;
	BOOL bSnoozing;
	SYSTEMTIME systime;
}AlarmLastTime;

typedef struct tagAlarmNode
{
	AlarmLastTime LastTime;
	struct tagAlarmNode * pNextNode;
}AlarmNode;

AlarmSetting	 Alarm_Settings[ALARM_MAX];

#define ID_SetDetail_Back	200
#define ID_Set_Back			201
#define ID_Alarm_Back		202
#define ID_TIMEZONE_BACK	203


#define TIMER_FIRST_ELAPSE		300
#define TIMER_SECOND_ELAPSE		100

HWND	hWndAlarmMain;
HWND	hWndAlarmSetMain;
HWND    Clock_hWndApp;
HWND	hClockFrameWnd;
DWORD AlarmWindow(int nID, BOOL bPri, BOOL bAlarmPwrOn, BOOL bAutoPowOn);
DWORD AlarmMain();
DWORD TimeZone();
void Alarm_GetWeekStr(char *cWeek,int *nSelectedDay);

#define ALARM_DIRNAME		"/mnt/flash/clock"
#define FILENAME_CONFIGURE	"/mnt/flash/clock/alarm.dat"
#define FILENAME_PRI		"/mnt/flash/clock/alarm_pri.dat"
#define FILENAME_LASTTIME	"/mnt/flash/clock/alarm_lasttime.dat"
#define INVALID_HANDLE_VALUE -1

#define ICON_HOMEZONE		"/rom/pim/clock/homezone.bmp"
#define ICON_VISITZONE		"/rom/pim/clock/visitingzone.bmp"
#define ICON_ACTIVEZONE    	"/rom/wml/not_available.bmp"
//#define ICON_ACTIVEZONE     "/rom/pim/clock/Not_Available.bmp"
#define ICON_RING			"/rom/pim/clock/icon2.bmp"
#define ICON_RIGHTARROW		"/rom/pim/clock/arrow_right.bmp"
#define ICON_LEFTARROW		"/rom/pim/clock/arrow_left.bmp"
#define CLOCK_ICON_ARROWRIGHT	"/rom/public/arrow_right.ico"
#define CLOCK_ICON_ARROWLEFT	"/rom/public/arrow_left.ico"
BOOL Configure_Init(void);
BOOL Configure_Get(int nID, AlarmSetting *pSetting);
BOOL Configure_Set(int nID, AlarmSetting setting);
BOOL Configure_SetIcon(void);



int ResetNode(int nID, BOOL bSnooze, AlarmSetting * psetting);

void ChangeTimeType(SYSTEMTIME *pDesTime, char *pResTime);
BOOL ClockIncreaseDay(int n, SYSTEMTIME* pTime);
extern BOOL IsAlarmPowerOn(void);
