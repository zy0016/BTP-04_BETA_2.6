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
#include <project/msgpublic.h>

void MSG_FTtoST(FILETIME* pFileTime, SYSTEMTIME* pSysTime)
{
	DWORD	time = pFileTime->dwLowDateTime;

	pSysTime->wYear		= (WORD)((( time & YEAR_MASK ) >> YEAR_OFFSET ) + BASE_YEAR);
	pSysTime->wMonth	= (WORD)(( time & MONTH_MASK ) >> MONTH_OFFSET );
	pSysTime->wDay		= (WORD)(( time & DAY_MASK ) >> DAY_OFFSET );
	pSysTime->wHour		= (WORD)(( time & HOUR_MASK ) >> HOUR_OFFSET );
	pSysTime->wMinute	= (WORD)(( time & MINUTE_MASK ) >> MINUTE_OFFSET );
	pSysTime->wSecond	= (WORD)(( time & SECOND_MASK ) << 1);
	pSysTime->wDayOfWeek= 0;
	pSysTime->wMilliseconds = 0;
}

void MSG_STtoFT(SYSTEMTIME* pSysTime, FILETIME* pFileTime)
{
	pFileTime->dwLowDateTime =  ((pSysTime->wYear-BASE_YEAR) << YEAR_OFFSET) & YEAR_MASK;
	pFileTime->dwLowDateTime |= (pSysTime->wMonth << MONTH_OFFSET) & MONTH_MASK;
	pFileTime->dwLowDateTime |= (pSysTime->wDay << DAY_OFFSET) & DAY_MASK;
	pFileTime->dwLowDateTime |= (pSysTime->wHour << HOUR_OFFSET) & HOUR_MASK;
	pFileTime->dwLowDateTime |= (pSysTime->wMinute << MINUTE_OFFSET) & MINUTE_MASK;
	pFileTime->dwLowDateTime |= (pSysTime->wSecond >> 1) & SECOND_MASK;

	pFileTime->dwHighDateTime = 0;
}
