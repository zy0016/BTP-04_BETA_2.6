/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : PushStrct.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _PUSHSTRCT_H_
#define _PUSHSTRCT_H_

#define PUSH_INBOX			100

#ifdef _EMULATE_
#define FILE_PUSH_INBOX		"D:/flash/app/mobile/unibox/PushInbox.dat"
#define FILE_PUSH_COPY		"D:/flash/app/mobile/unibox/PushCopy.dat"
#else
#define FILE_PUSH_INBOX		"/mnt/flash/app/mobile/unibox/PushInbox.dat"
#define FILE_PUSH_COPY		"/mnt/flash/app/mobile/unibox/PushCopy.dat"
#endif // _EMULATE_

typedef struct tagPushFileReord
{
	int  offset;
	BOOL status;
	int  type;
	int  TitleLen;
	char *pszTitle;
	int  UrlLen;
	char *pszUri;
}PUSHFILERECORD, *PPUSHFILERECORD;

#define YEAR_MASK		0xFE000000		/*(11111110000000000000000000000000b)*/
#define MONTH_MASK		0x01E00000		/*(00000001111000000000000000000000b)*/
#define DAY_MASK		0x001F0000		/*(00000000000111110000000000000000b)*/
#define HOUR_MASK		0x0000F800		/*(00000000000000001111100000000000b)*/
#define MINUTE_MASK		0x000007E0		/*(00000000000000000000011111100000b)*/
#define SECOND_MASK		0x0000001F		/*(00000000000000000000000000011111b)*/

#define YEAR_OFFSET		25
#define MONTH_OFFSET	21
#define DAY_OFFSET		16
#define HOUR_OFFSET		11
#define MINUTE_OFFSET	5
#define SECOND_OFFSET	0

#define INTYEAR(dt)		(((dt) & YEAR_MASK) >> YEAR_OFFSET)
#define INTMONTH(dt)	(((dt) & MONTH_MASK) >> MONTH_OFFSET)
#define INTDAY(dt)		(((dt) & DAY_MASK) >> DAY_OFFSET)
#define INTHOUR(dt)		(((dt) & HOUR_MASK) >> HOUR_OFFSET)
#define INTMINUTE(dt)	(((dt) & MINUTE_MASK) >> MINUTE_OFFSET)
#define INTSECOND(dt)	(((dt) & SECOND_MASK) << 1)

#define Datetime2INT(year, month, day, hour, minute, second)	\
	 (DWORD)((((DWORD)(year) << YEAR_OFFSET) | (((DWORD)month) << MONTH_OFFSET) | (((DWORD)day) << DAY_OFFSET)	\
	   | (((DWORD)hour) << HOUR_OFFSET) | (((DWORD)minute) << MINUTE_OFFSET) | (((DWORD)second) >> 1)))

#endif // _PUSHSTRCT_H_
