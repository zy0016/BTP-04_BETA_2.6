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
#ifndef _MSGPUBLIC_H
#define _MSGPUBLIC_H

#include "window.h"

/* file time */
#ifndef _FILETIME_
#define _FILETIME_
typedef struct _FILETIME {	// ft 
    DWORD		dwLowDateTime;	//
    DWORD		dwHighDateTime;	// 
}FILETIME, *PFILETIME, *LPFILETIME; 
#endif // _FILETIME_

///* system time */
//#ifndef _SYSTEMTIME_
//#define _SYSTEMTIME_
//typedef struct _SYSTEMTIME 
//{  
//    WORD wYear;     
//    WORD wMonth; 
//    WORD wDayOfWeek;     
//    WORD wDay;     
//    WORD wHour;     
//    WORD wMinute; 
//    WORD wSecond;     
//    WORD wMilliseconds; 
//} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME; 
//#endif // _SYSTEMTIME_

/*---------- datetime ----------------------------------------------*/
#define BASE_YEAR		1980

#define YEAR_MASK		0xFE000000		/*	(1111 1110 0000 0000 0000 0000 00000000b)*/
#define MONTH_MASK		0x01E00000		/*	(0000 0001 1110 0000 0000 0000 00000000b)*/
#define DAY_MASK		0x001F0000		/*	(0000 0000 0001 1111 0000 0000 00000000b)*/
#define HOUR_MASK		0x0000F800		/*	(0000 0000 0000 0000 1111 1000 00000000b)*/
#define MINUTE_MASK		0x000007E0		/*	(0000 0000 0000 0000 0000 0111 11100000b)*/
#define SECOND_MASK		0x0000001F		/*	(0000 0000 0000 0000 0000 0000 00011111b)*/


#define YEAR_OFFSET		25
#define MONTH_OFFSET	21
#define DAY_OFFSET		16
#define HOUR_OFFSET		11
#define MINUTE_OFFSET	5
#define SECOND_OFFSET	0

/* macros for date/time populating*/
#define INTYEAR(dt)		((( (dt) & YEAR_MASK ) >> YEAR_OFFSET) + BASE_YEAR)
#define INTMONTH(dt)	(( (dt) & MONTH_MASK ) >> MONTH_OFFSET)
#define INTDAY(dt)		(( (dt) & DAY_MASK ) >> DAY_OFFSET)
#define INTHOUR(dt)		(( (dt) & HOUR_MASK ) >> HOUR_OFFSET)
#define INTMINUTE(dt)	(( (dt) & MINUTE_MASK) >> MINUTE_OFFSET)
#define INTSECOND(dt)	(( (dt) & SECOND_MASK) << 1)
 
#define Datetime2INT(year, month, day, hour, minute, second)	\
	 (DWORD)( (((DWORD)(year-BASE_YEAR) << YEAR_OFFSET) | (((DWORD)month) << MONTH_OFFSET) | (((DWORD)day) << DAY_OFFSET) \
	   | (((DWORD)hour) << HOUR_OFFSET) | (((DWORD)minute) << MINUTE_OFFSET) | (((DWORD)second) >> 1)) )


extern void MSG_FTtoST(FILETIME* pFileTime, SYSTEMTIME* pSysTime);
extern void MSG_STtoFT(SYSTEMTIME* pSysTime, FILETIME* pFileTime);

extern void MMSNotify(int nMMS,BOOL bFull);
extern void SMSNotify(int nSMS,BOOL bFull);

#endif // _MSGPUBLIC_H
