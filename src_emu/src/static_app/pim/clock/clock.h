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
#include <pubapp.h>

#include <plx_pdaex.h>
#include <imesys.h>
#include <string.h>
#include <stdlib.h>
#include <window.h>
#include <stdio.h>
#include <mullang.h>
#include <setting.h>
#include <PMALARM.h>
#include <window.h>
#include <dirent.h>
#include <stat.h>
#include <time.h>

#include "Almglobal.h"
#include "pubapp.h"

#define IDS_ALARM			ML("Alarms")
#define IDS_EXIT			ML("Exit")
#define IDS_SETCLOCK		ML("Set clock")


#define ID_ALARM			1
#define ID_EXIT				2
#define ID_SETCLOCK			3
#define ID_TIMEZONE			4
#define ID_TIMER			5

#define TIME_LENTH			10
#define DATE_LENTH			10
#define ZONE_LENTH			20
#define WEEK_LENTH           2
#define WEEKDATE_LENTH      13
#define IDS_OK					ML("Ok")
//#define ICON1				"/rom/pim/clock/icon1.bmp"


//RECT rLocalTime = {64, 5, 150, 28};
RECT rLocalTime = {70, 5, 156, 28};
RECT rLocalZone = {60, 29, 170, 45};
RECT rLocalWeek = {56, 46, 75, 62};
RECT rLocalDate = {75, 46, 170, 62};
RECT rClock		= {50, 3, 170, 70};

RECT rAlarmCaption	= {60, 86, 170, 102};
//RECT rAlarmTime		= {64, 103, 170, 126};
RECT rAlarmTime = {70, 103, 156, 126};
RECT rAlarmWeek		= {56, 127, 75, 143};
RECT rAlarmDate		= {75, 127, 170, 143};
RECT rAlarm			= {50, 80, 170, 143};

RECT rIcon1		= {35, 9, 60, 24};
RECT rIcon2		= {35, 107, 60, 122};

typedef struct tagRegionTime
{
	int   year;
	int   month;
	int   day;
	int   hour;
	int   minute;
	int   week;
	int   zone;
}CLOCK_REGIONTIME;

static char *cWeek[7] =
{
	"SU",
	"MO",
	"TU",
	"We",
	"TH",
	"FR",
	"SA",
};


static LRESULT AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static int	GetZoneValue();
static HMENU CreateMenu_MainWnd();
static void GetLocalTimeStr(char *cTime, char *cDate, char *cZone);
static void GetAlarmDisplayStr(SYSTEMTIME *pTime, char *cTime, char *cDate, int *nDayOfWeek);

static HINSTANCE hInstance;
static HWND hFocus;
static CLOCK_REGIONTIME LocalTimeData;
static DWORD wSecond;
static BOOL bNextAlarm = FALSE;
static HBITMAP hBmpHomeZone, hBmpVisitZone, hBmpRing;

extern BOOL TimeSetting(HWND hwndCall);
extern const char* GetHomeZoneStr(void);//to be continue...
extern const char* GetVisitZoneStr(void);//to be continue...

TIMEZONE GetUsingTimeZone(void);
 
