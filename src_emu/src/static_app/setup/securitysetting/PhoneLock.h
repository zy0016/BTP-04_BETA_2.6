 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Setup - security setting		
 *
 * Purpose  : 
 *
\**************************************************************************/
#ifndef PHONELOCK_H
#define PHONELOCK_H 

#include "securitysetting.h"

#define		IDC_TIMEOUT				200
#define     IDC_LOCKTIME_QUIT       201
#define		IDC_LOCKPERIOD			202
#define		IDC_LOCK				203
#define     IDC_PHONELOCK_QUIT      204

#define     SECURITYLOCKPERIOD		"Autolock period"
#define     SECURITYLOCK            "Phone lock code"

#define		CODE_X					5
#define		CODE_Y					5
#define		CODE_WIDTH				150
#define		CODE_HIGHT				30
#define		LOCKTIMEWND_WIDTH		PLX_WIN_WIDTH
#define		LOCKTIMEWND_HIGHT		50
#define     MAX_PERIODTIME			7

#define     CALLBACK_PERIOD			WM_USER	+201

static   char* selecttime[] =
	{
		"Off",
		"1 min",
		"2 min",
		"5 min",
		"10 min",
		"15 min",
		"30 min"
	};

static HWND					hFocusPhoneLock;
static HWND					hLockPeriod;

static HWND                 hCallwnd;
static UINT					CallMsg;

static BOOL					bLockRequst;

static HWND hList, hTimeOutFocus;
static HBITMAP  hIconNormal, hIconSel;
extern HWND hFrameWin;

static int  v_nRepeats_PC;
static unsigned int  iRepeatKeyTimerId_PC = 0;
static DWORD  nKeyCode_PC;

//phone lock view
static LRESULT  AppPhoneLockWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL     PhoneLock_OnCreate(HWND hWnd);
static void     PhoneLock_OnPaint(HWND hWnd);
static void     PhoneLock_OnKeydown(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
static void     PhoneLock_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void     PhoneLock_OnActive(HWND hWnd, WPARAM wParam);
static void     PhoneLock_OnCallBack(HWND hWnd, LPARAM lParam);

//autolock period
static BOOL     CreateLockTimeWnd(HWND hWnd, UINT CallMsg);
static LRESULT  AppLockTimeWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL     LockTime_OnCreate(HWND hWnd, HWND *hList);
static void     LockTime_OnKeydown(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
static void     LockTime_OnCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void     LockTime_OnDestroy(HWND hWnd);

BOOL        GetLockCodePeriod(char* pLockPeriodTime);//get autolock period
void		SetLockCodePeriod(char* pLockPeriod);

static void CALLBACK f_PCTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);

#endif			
