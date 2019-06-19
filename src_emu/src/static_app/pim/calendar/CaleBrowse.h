/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleBrowse.h
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleHeader.h"

//delete submenu
MENUITEMTEMPLATE CaleDelMenuItem[] =
{
    {MF_STRING, IDM_BUTTON_DELSELECT, NULL, NULL},
    {MF_STRING, IDM_BUTTON_DELDATE, NULL, NULL},
    {MF_STRING, IDM_BUTTON_DELALL, NULL, NULL},
    {MF_END, 0, NULL, NULL}
};

const MENUTEMPLATE CaleDelMenuTemplate =
{
    0,
        CaleDelMenuItem
};

//send submenu
MENUITEMTEMPLATE CaleSendMenuItem[] =
{
    {MF_STRING, IDM_BUTTON_BYSMS, NULL, NULL},
    {MF_STRING, IDM_BUTTON_BYMMS, NULL, NULL},
    {MF_STRING, IDM_BUTTON_BYEMAIL, NULL, NULL},
    {MF_STRING, IDM_BUTTON_BYBLUE, NULL, NULL},
    {MF_END, 0, NULL, NULL}
};

const MENUTEMPLATE CaleSendMenuTemplate =
{
    0,
        CaleSendMenuItem
};

static LRESULT CaleAPPWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
HWND CALE_GetFrame(void);
HWND CALE_GetAppWnd(void);

static DWORD RegisterCaleJumpClass(void);
static void UnRegisterCaleJumpClass(void);
static BOOL CreateJumpWnd(HWND hFrameWnd, HWND hwnd, SYSTEMTIME *NowTime);
static LRESULT CaleJumpWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

extern void CALE_AlarmOut(SYSTEMTIME *CurAlarmSys, /*char *AlarmName*/long lSysAlmID);
extern int CALE_RegisterAlarm(int nFlag);
extern BOOL CALE_RegisterMonthClass(void* hInst);
extern BOOL CALE_UnRegisterMonthClass();
extern BOOL CALE_RegisterDayClass(void* hInst);
extern BOOL CALE_UnRegisterDayClass();
extern BOOL CALE_RegisterWeekClass(void* hInst);
extern BOOL CALE_UnRegisterWeekClass();
extern int CALE_GetDefaultView(void);
extern void CALE_DateSysToChar(SYSTEMTIME *pSys, char *pChar);
extern BOOL CALE_DelSchedule(int DelFlag, SYSTEMTIME *pSys);
extern BOOL CreateConfigWnd(HWND hwnd);
extern int CALE_CalcWkday(SYSTEMTIME* pSysInfo);
extern BOOL CreateDelSelWnd(HWND hwnd);
extern BOOL CALE_CleanApi(void);
extern BOOL CALE_NeedClean(void);
extern BOOL CALE_CleanAlarm(void);

extern void CALE_MonthViewDisplay(SYSTEMTIME *pSys, char *pChar);
extern void CALE_WeekViewDisplay(SYSTEMTIME *pSys, char *pChar);
extern void CALE_DayViewDisplay(SYSTEMTIME *pSys, char *pChar);
