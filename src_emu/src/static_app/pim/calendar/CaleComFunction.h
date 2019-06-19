/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleComFunction.h
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleHeader.h"

int CALE_CmpCaleDate(CaleDATE *pFirstDate, CaleDATE *pSecondDate);
int CALE_CmpSysDate(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate);
int CALE_DaynCmp(SYSTEMTIME systime1, SYSTEMTIME systime2, unsigned int numday);
BOOL CALE_DayIncrease(SYSTEMTIME *psystime, unsigned int day);
BOOL CALE_DayReduce(SYSTEMTIME *psystime, unsigned int day);
int CALE_AlmDaysOfMonth(SYSTEMTIME *pDate);
BOOL CALE_IsLeapYear(WORD wYear);
BOOL CALE_MonthIncrease(SYSTEMTIME *pSystime);
BOOL CALE_YearIncrease(SYSTEMTIME *pSystime);
int CALE_CalcWkday(SYSTEMTIME* pSysInfo);
BOOL CALE_SetScheduleFlag(CaleMonthSch *pSched);
int GetScrollPos(HWND hWnd, int nBar);
int SetScrollPos(HWND hWnd, int nBar, int nPos, BOOL bRedraw);
void CALE_TimeSysToChar(SYSTEMTIME *pSys, char *pChar);
void CALE_TimeCharToSys(char *pChar, SYSTEMTIME *pSys);
void CALE_DateSysToChar(SYSTEMTIME *pSys, char *pChar);
void CALE_DateCharToSys(char *pChar, SYSTEMTIME *pSys);
void CALE_DateTimeSysToChar(SYSTEMTIME *pSys, char *pChar);
void CALE_DateTimeCharToSys(char *pChar, SYSTEMTIME *pSys);
void CALE_MonthViewDisplay(SYSTEMTIME *pSys, char *pChar);
void CALE_WeekViewDisplay(SYSTEMTIME *pSys, char *pChar);
void CALE_DayViewDisplay(SYSTEMTIME *pSys, char *pChar);
BOOL CALE_DelSchedule(int DelFlag, SYSTEMTIME *pSys);
BOOL CALE_SetWeekSchedule(PCaleWeek pWeek);
int CALE_IntervalSysDate(SYSTEMTIME *pStartDate, SYSTEMTIME *pEndDate);
int CALE_CmpOnlyDate(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate);
int CALE_CmpOnlyTime(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate);
void CALE_ReleaseWeekSchNode(CaleWeekSchHead *pHead);
BOOL CALE_GetWeekSchByInt(CaleWeekSchHead *pHead, CaleWeekSchNode **pTmpNode, int iTmp);
void CALE_FormatDisplay(HWND hWnd, char *szDisplay, char *szString);

BOOL CALE_GetFormatedTime(char *pBuf, int nHour, int nMinute);
BOOL CALE_GetFormatedDate(SYSTEMTIME *pSys, char *pChar);
BOOL CALE_GetFormatedFullDate(SYSTEMTIME *pSys, char *pChar);
void CALE_SetSystemIcons(HWND hFrame);

BOOL CALE_CheckDate(SYSTEMTIME *pDate);
int CALE_GetDaysOfMonth(int year, int month);
BOOL CALE_GetUTCTime(SYSTEMTIME *pTime, PCSTR szTimeZone, BOOL bAdd);

static void CALE_AddWeekSchNode(CaleWeekSchHead *pHead, CaleWeekSchNode *pTmpNode);
static void CALE_DelWeekSchNode(CaleWeekSchHead *pHead, int iTmp);

extern int CALE_OpenApi(int CaleEntry);
extern BOOL CALE_CloseApi(int Cal_OpenFile);
extern int CALE_ReadApi(int Cal_Open, int itemID, BYTE *pRecord, int size);
extern BOOL CALE_DelApi(int CaleEntry, int Cal_Open, int itemID, BYTE *pRecord, int size);
extern int CALE_KillAlarm(int CaleType, int nAlmID);
extern int CALE_GetWeekStart(void);
