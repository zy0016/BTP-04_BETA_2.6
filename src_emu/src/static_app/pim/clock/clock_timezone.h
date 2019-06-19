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

#include "almglobal.h"

#define ID_USE				1
#define ID_EXIT				2
#define ID_SETCLOCK			3
#define ID_TIMER			4

#define  CALLBACK_ME		WM_USER + 300

#define IDS_USE					ML("Use")
#define IDS_EXIT				ML("Exit")
#define IDS_SETCLOCK			ML("Set Clock")
#define IDS_SUCCESS				ML("Time zone\nin use:\n")
#define IDS_CONFIRM_TITLE		ML("Clock")
#define HOME					ML("Home")
#define VISITING				ML("Visiting")
#define IDS_OK					ML("Ok")

#define ZONELENGTH	15



static HWND hWndTimeZone;
static HWND hFocus;
static HWND hWndList;
static HINSTANCE	hInstance = NULL;
static HBITMAP hBmpHomeZone;
static HBITMAP hBmpVisitZone;
//static HBITMAP hBmpLeftArrow;
static HBITMAP hBmpActiveZone;

static LRESULT AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static HWND CreateMainWnd();
static HMENU CreateMenu_TimeZoneWnd();
static void UpdateList(HWND hWnd);
static void InitList(HWND hWnd);
static void CreateListBox(HWND hWnd);

extern /*SYSTEMTIME*/void SetUsingTimeZone(TIMEZONE tz, SYSTEMTIME* st);
extern const char* GetHomeZoneStr(void);//to be continue...
extern const char* GetVisitZoneStr(void);//to be continue...
extern BOOL TimeSetting(HWND hwndCall);
extern void GetNewZoneTime(SYSTEMTIME *st_old,const char *szNewZone,const char *szOldZone);
