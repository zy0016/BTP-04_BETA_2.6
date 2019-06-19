/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Bluetooth module
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "stdio.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "time.h"

#include "BtMsg.h"

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

UINT BtGetMsgSize(char* filename)
{
	UINT size;
    struct stat fd_stat;
	
	stat(filename,&fd_stat);
	
	size=fd_stat.st_size;
	
	return size;	
}

BOOL BtGetMsgTime(char *filename, PBTMSGTIME pBtMsgTime)
{
	struct stat fd_stat;
   	SYSTEMTIME    systime;
	struct tm     tme, *ptime;
	
	stat(filename, &fd_stat);
				
	ptime = &tme;
	
	ptime = localtime(&(fd_stat.st_mtime));
	
	systime.wDay = (WORD)ptime->tm_mday;
	systime.wDayOfWeek = (WORD)ptime->tm_wday;
	systime.wHour = (WORD)ptime->tm_hour;
	systime.wMilliseconds = 0;
	systime.wMinute = (WORD)ptime->tm_min;
	systime.wMonth = (WORD)ptime->tm_mon + 1;
	systime.wSecond = (WORD)ptime->tm_sec;
	systime.wYear = (WORD)ptime->tm_year + 1900;
	
	BtSTtoFT(&systime, pBtMsgTime);
	
	return TRUE;
}

void BtFTtoST(PBTMSGTIME pBtMsgTime, SYSTEMTIME* pBtMsgSysTime)
{
	DWORD	time = pBtMsgTime->dwLowDateTime;
	
	pBtMsgSysTime->wYear   = (WORD)((( time & YEAR_MASK ) >> YEAR_OFFSET ) + BASE_YEAR);
	pBtMsgSysTime->wMonth  = (WORD)(( time & MONTH_MASK ) >> MONTH_OFFSET );
	pBtMsgSysTime->wDay	   = (WORD)(( time & DAY_MASK ) >> DAY_OFFSET );
	pBtMsgSysTime->wHour   = (WORD)(( time & HOUR_MASK ) >> HOUR_OFFSET );
	pBtMsgSysTime->wMinute = (WORD)(( time & MINUTE_MASK ) >> MINUTE_OFFSET );
	pBtMsgSysTime->wSecond = (WORD)(( time & SECOND_MASK ) << 1);
	pBtMsgSysTime->wDayOfWeek= 0;
	pBtMsgSysTime->wMilliseconds = 0;
}

void BtSTtoFT(SYSTEMTIME* pBtMsgSysTime, PBTMSGTIME pBtMsgTime)
{
	pBtMsgTime->dwLowDateTime =  ((pBtMsgSysTime->wYear-BASE_YEAR) << YEAR_OFFSET) & YEAR_MASK;
	pBtMsgTime->dwLowDateTime |= (pBtMsgSysTime->wMonth << MONTH_OFFSET) & MONTH_MASK;
	pBtMsgTime->dwLowDateTime |= (pBtMsgSysTime->wDay << DAY_OFFSET) & DAY_MASK;
	pBtMsgTime->dwLowDateTime |= (pBtMsgSysTime->wHour << HOUR_OFFSET) & HOUR_MASK;
	pBtMsgTime->dwLowDateTime |= (pBtMsgSysTime->wMinute << MINUTE_OFFSET) & MINUTE_MASK;
	pBtMsgTime->dwLowDateTime |= (pBtMsgSysTime->wSecond >> 1) & SECOND_MASK;
	
	pBtMsgTime->dwHighDateTime = 0;
}

#define BUF_SIZE 1024
BOOL BtCopyFile(PCSTR pOldFileName, PCSTR pNewFileName)
{
	int  hfold,hfnew;
	char buf[BUF_SIZE];
	DWORD nBytes, dwPointer = 0;
	
	hfold = open(pOldFileName, O_RDONLY);
	
	if(hfold == -1)
		return 0;
	
	hfnew = open(pNewFileName, O_RDWR|O_CREAT, S_IRWXU);
	
	if(hfnew == -1)
	{
		close(hfold);
		return 0;
	}
	nBytes = read(hfold, buf, BUF_SIZE);
	
	while (nBytes != 0 && nBytes != -1)
	{
		int ret;
		
		lseek(hfnew, dwPointer, SEEK_SET);
		ret = write(hfnew, buf, nBytes);
		
		if(ret != (int)nBytes)
		{
			close(hfnew);
			close(hfold);
			return 0;
		}
		dwPointer += nBytes; 
		lseek(hfold, dwPointer, SEEK_SET);
		nBytes = read(hfold, buf, BUF_SIZE);
		
	}
	close(hfold);
	close(hfnew);
	return 1;
	
}

BOOL BtMoveFile(PCSTR pOldFileName, PCSTR pNewFileName)
{
	
	BtCopyFile(pOldFileName, pNewFileName);
	remove(pOldFileName);
	
	return TRUE;
}
