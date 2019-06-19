/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleAlarm.h
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleHeader.h"

BOOL CALE_RegisterAlarm(int nFlag);
BOOL CALE_NewMeet(CaleMeetingNode *CurMeetDate, DWORD nItemID, int nFlag);
BOOL CALE_NewAnni(CaleAnniNode *CurAnniDate, DWORD nItemID, int nFlag);
void CALE_MeetToAlmNode(CaleMeetingNode *pCurCale, CaleAlarmNode *pNode, DWORD dwSaveIndex);
void CALE_AnniToAlmNode(CaleAnniNode *pCurCale, CaleAlarmNode *pNode, DWORD dwSaveIndex);
int CALE_KillAlarm(int CaleType, int nAlmID);
int CALE_ResetAlarm(CaleAlarmNode *pNode, int nFlag);
int CALE_SnoozeAlarm(CaleAlarmNode *pNode, int nMinute);
BOOL CALE_CleanAlarm(void);
void CALE_AlarmOut(SYSTEMTIME *CurAlarmSys, /*char *AlarmName*/long lSysAlmID);
static LRESULT CALLBACK CaleAlarmWndProc(HWND hWnd, UINT message,
                                         WPARAM wParam, LPARAM lParam);
void CALE_AlmWnd(SYSTEMTIME *CurAlarmSys, CaleAlarmNode *CurAlarmNode);
BOOL CALE_AddAlarmNode(CaleAlarmHead *pHead, CaleAlarmNode *pNode);
BOOL CALE_DelAlarmNode(CaleAlarmHead *pHead, int iTmp);
void CALE_CalcAlarmNode(CaleAlarmNode *pNode, int iDay, int iHour, int iMin);
static void CALE_ReleaseAlarmNode(CaleAlarmHead *pHead);
static int CALE_GetAlarmNodeByID(CaleAlarmHead *pHead, CaleAlarmNode **pNode, BYTE bType, int iIndex);
void CALE_LOCK(void);
void CALE_UNLOCK(void);

BOOL CALE_NeedClean(void);

extern int CALE_OpenApi(int CaleEntry);
extern BOOL CALE_CloseApi(int Cal_OpenFile);
extern int CALE_ReadApi(int Cal_Open, int itemID, BYTE *pRecord, int size);
//extern BOOL CALE_AddApi(int Cal_Open, BYTE *pRecord, int size);
//extern BOOL CALE_ModifyApi(int Cal_Open, int itemID, BYTE *pRecord, int size);
//extern BOOL CALE_DelApi(int CaleEntry, int Cal_Open, int itemID, BYTE *pRecord, int size);
extern int CALE_CmpSysDate(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate);
extern int CALE_DaynCmp(SYSTEMTIME systime1, SYSTEMTIME systime2, unsigned int numday);
extern BOOL CALE_DayIncrease(SYSTEMTIME *psystime, unsigned int day);
extern BOOL CALE_MonthIncrease(SYSTEMTIME *pSystime);
extern BOOL CALE_YearIncrease(SYSTEMTIME *pSystime);
extern BOOL CALE_DayReduce(SYSTEMTIME *psystime, unsigned int day);
extern int CALE_AlmDaysOfMonth(SYSTEMTIME *pDate);
extern int CALE_GetSnooze(void);
extern void CALE_FormatDisplay(HWND hWnd, char *szDisplay, char *szString);

//extern BOOL f_sleep_register(int handle);
//extern BOOL f_sleep_unregister(int handle);
