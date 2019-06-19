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

#ifndef _CALENDAR_H_
#define _CALENDAR_H_

#define WM_CALENDAR		WM_USER + 100

extern BOOL DisplayCalendar(HWND hwnd, SYSTEMTIME* caltime, BOOL bCallerClose);
extern int MYear_CalcLunarDate(PSYSTEMTIME pDate, PSYSTEMTIME pLunarDate);
extern void MYear_FormatLunar(PSYSTEMTIME pDate, char* pBuffer);

#endif // _CALENDAR_H_
