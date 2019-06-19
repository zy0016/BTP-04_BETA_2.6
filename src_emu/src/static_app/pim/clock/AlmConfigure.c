/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : AlmConfigure.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "plx_pdaex.h"
#include "pubapp.h"
#include "winpda.h"
#include "hpimage.h"
#include "unistd.h"
#include "dirent.h"
#include "types.h"
#include "stat.h"
#include "unistd.h"
#include "vfs.h"
#include "fcntl.h"
#include "me_wnd.h"
#include "time.h"
#include "math.h"
#include "pmalarm.h"
#include "Almglobal.h"
#include "AlmSet.h"

static int CalcDaysOfMonth(PSYSTEMTIME pDate);
static BOOL IsLeapYear(WORD wYear);
static int CountIncreasedDaysNum(int nNextDay, int nToDay);
static int GetNextAlarmDay(int *pDayOfWeek, int nTodayOfWeek);
static int Compare(unsigned long time1, unsigned long time2, unsigned long *pMini);

static int AddNode(AlarmLastTime *pLastTime);
static int ClearNode(int nID);


BOOL Configure_Init(void)
{
	int hFile;
	AlarmSetting setting;
	int i;
	int n = 0;
	int size;

	hFile = open(FILENAME_CONFIGURE, O_RDONLY, 0);//judge if there is datfile.
	if (hFile != INVALID_HANDLE_VALUE)
	{
		close(hFile);
		return TRUE;
	}
	

	if(-1 ==chdir(ALARM_DIRNAME))
	{
		mkdir(ALARM_DIRNAME, 0);
	}
	//创建配置文件
	hFile = open(FILENAME_CONFIGURE, O_WRONLY | O_CREAT, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	//配置文件初始化
	setting.bIsOpen = FALSE;
	setting.nHour	= 0;
	setting.nMinute = 0;
	setting.nRepeat = Repeat_Once;
	setting.bSnooze = FALSE;
	for (i = 0; i < 7; i++)
		setting.nSelectedDay[i] = 0; 
	setting.nSnooze	= Snooze_5Min;
	size = sizeof(AlarmSetting);

	for (i = 0; i <ALARM_MAX ; i++)
	{
		lseek(hFile, size * i, SEEK_SET);
		write(hFile, &setting, sizeof(AlarmSetting));
	}
	close(hFile);

	return TRUE;
}

BOOL Configure_Set(int nID, AlarmSetting setting)
{
	int hFile;

	if (nID < 0 || nID > 2)
		return FALSE;

	chdir(ALARM_DIRNAME);

	hFile = open(FILENAME_CONFIGURE, O_WRONLY | O_CREAT, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	lseek(hFile, sizeof(AlarmSetting) * nID, SEEK_SET);
	if (-1 == write(hFile, &setting, sizeof(AlarmSetting)))
	{
		close(hFile);
		return FALSE;
	}

	close(hFile);
//	Configure_SetIcon();
	return TRUE;	
}

BOOL Configure_Get(int nID, AlarmSetting *pSetting)
{
	int hFile;

	if (nID < 0 || nID > 2)
		return FALSE;

	chdir(ALARM_DIRNAME);

	hFile = open(FILENAME_CONFIGURE, O_RDONLY, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	lseek(hFile, sizeof(AlarmSetting) * nID, SEEK_SET);
	if (-1 == read(hFile, pSetting, sizeof(AlarmSetting)))
	{
		close(hFile);
		return FALSE;
	}

	close(hFile);
	return TRUE;
}

BOOL Configure_SetIcon(void)
{
	int hFile;
	int i = 0;
	AlarmSetting setting;

	//打开配置文件失败
	chdir(ALARM_DIRNAME);

	hFile = open(FILENAME_CONFIGURE, O_RDONLY, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		DlmNotify(PS_ALARMCLOCK, ICON_CANCEL);
		return FALSE;
	}

	for (; i<ALARM_MAX; i++)
	{
		lseek(hFile, sizeof(AlarmSetting) * i, SEEK_SET);
		//读取配置文件失败
		if (-1 == read(hFile, &setting, sizeof(AlarmSetting)))
		{
			close(hFile);
			DlmNotify(PS_ALARMCLOCK, ICON_CANCEL);
			return FALSE;
		}

		//闹钟打开状态
		if (setting.bIsOpen)
		{
			close(hFile);
			DlmNotify(PS_ALARMCLOCK, ICON_SET);
			return TRUE;
		}
	}

	//闹钟关闭状态
	close(hFile);
	DlmNotify(PS_ALARMCLOCK, ICON_CANCEL);
	return FALSE;
}



int ResetNode(int nID, BOOL bSnooze, AlarmSetting * psetting)
{
	int nDayNum;
	int nNextDayOfWeek;
	BOOL bTodaySelectedOrNo;
	BOOL bValidTime = 1;
	int i;
	SYSTEMTIME systime;
	int retval;
	RTCTIME rtctime;	
	
	
	if (!psetting->bIsOpen)
	{
		
		RTC_KillAlarms("Clock", (unsigned short)nID);
		Configure_Set(nID, *psetting);
		if(!IsAlarmPowerOn())
			Configure_SetIcon();

		return 1;
		
	}	
	
	GetLocalTime(&systime);   	
	
	if(bSnooze)
	{
		
		systime.wMinute = systime.wMinute + psetting->nSnooze;
		if (systime.wMinute >= 60)
		{
			systime.wMinute = systime.wMinute - 60;
			systime.wHour = systime.wHour + 1;
			if (systime.wHour > 23) 
			{
				systime.wHour = systime.wHour - 24;
				bValidTime = ClockIncreaseDay(1, &systime);
			}
		}
		psetting->bSnooze = TRUE;
		psetting->nFlag = ALMF_REPLACE|ALMF_POWEROFF|ALMF_RELATIVE; // 0x0D
	}
	else
	{
		if (psetting->nRepeat == Repeat_Once)
		{
			if (psetting->nHour < systime.wHour ||
				(psetting->nHour == systime.wHour && psetting->nMinute <= systime.wMinute)) 
				psetting->nFlag  = ALMF_REPLACE|ALMF_POWEROFF|ALMF_AUTONEXTDAY;// 0x07
			else
				psetting->nFlag  = ALMF_REPLACE|ALMF_POWEROFF;		//0x05	
			
		}		
		
		if (psetting->nRepeat == Repeat_Daily) 
		{
			if (psetting->nHour < systime.wHour ||
				(psetting->nHour == systime.wHour && psetting->nMinute <= systime.wMinute)) 
			{
				bValidTime = ClockIncreaseDay(1, &systime);
			}
			psetting->nFlag  = ALMF_REPLACE|ALMF_POWEROFF;
		}
		if (psetting->nRepeat == Repeat_SelectDays)
		{
			for(i = 0 ; i < 7 ; i++)
			{
				if (psetting->nSelectedDay[i] &&i == systime.wDayOfWeek)
				{
					bTodaySelectedOrNo = TRUE;
					break;
				}
				bTodaySelectedOrNo = FALSE;
			}
			if (bTodaySelectedOrNo) 
			{
				if (psetting->nHour < systime.wHour ||
					(psetting->nHour == systime.wHour && psetting->nMinute <= systime.wMinute)) 
				{
					nNextDayOfWeek = GetNextAlarmDay(psetting->nSelectedDay,systime.wDayOfWeek);
					nDayNum = CountIncreasedDaysNum(nNextDayOfWeek, systime.wDayOfWeek);
					bValidTime = ClockIncreaseDay(nDayNum, &systime);
				}
			}
			else
			{
				nNextDayOfWeek = GetNextAlarmDay(psetting->nSelectedDay, systime.wDayOfWeek);
				if (nNextDayOfWeek > -1)
				{
					nDayNum = CountIncreasedDaysNum(nNextDayOfWeek, systime.wDayOfWeek);
					bValidTime = ClockIncreaseDay(nDayNum, &systime);
				}
				
			}
			psetting->nFlag  = ALMF_REPLACE|ALMF_POWEROFF;
		}
		psetting->bSnooze = FALSE;
		systime.wHour = psetting->nHour;
		systime.wMinute = psetting->nMinute;
		systime.wSecond = 0;
	}
	  if(!bValidTime)
	  {	
		  RTC_KillAlarms("Clock", (unsigned short)nID);
		  psetting->bIsOpen = FALSE;
		  Configure_Set(nID, *psetting);	
		  if(!IsAlarmPowerOn())
			  Configure_SetIcon();
		  return 0;
		  
	  }	
	   rtctime.v_nDay = (unsigned char)systime.wDay;
	   rtctime.v_nDayOfWeek = (unsigned char)systime.wDayOfWeek;
	   rtctime.v_nHour = (unsigned char)systime.wHour;
	   rtctime.v_nMinute = (unsigned char)systime.wMinute;
	   rtctime.v_nMonth = (unsigned char)systime.wMonth;
	   rtctime.v_nSecond = (unsigned char)systime.wSecond;
	   rtctime.v_nYear = (unsigned short)systime.wYear;
	   
	   retval = RTC_SetAlarms("Clock", (unsigned short)nID, psetting->nFlag, "Clock", &rtctime);
	  printf("^^^^^^^^^^^^^^ResetNode SetAlarms^^^^^^^^^^^^^^\r\n");
	  printf("%d/%d/%d/,%d:%d\r\n", rtctime.v_nYear, rtctime.v_nMonth, rtctime.v_nDay,  rtctime.v_nHour, rtctime.v_nMinute); 
	  printf("retval = %d\r\n", retval);
	  if (retval == 1)
	  {		
		  Configure_Set(nID, *psetting);	
		  if(!IsAlarmPowerOn())
				Configure_SetIcon();
		 // 	PLXTipsWin(0, 0, 0, "set alarm success!", "clock", Notify_Success, NULL, NULL, WAITTIMEOUT);
	  
	  }
	  else
		   return 0;	   
	   
	   return 1;
}


static int CountIncreasedDaysNum(int nNextDay, int nToDay)
{
	int nDayNum;

	nDayNum = nNextDay - nToDay;
	if(nDayNum > 0)
		return nDayNum;
	if (nDayNum < 0)
		return 7+nDayNum;
	if (nDayNum == 0)
		return 7;
	return 0;
}
static int GetNextAlarmDay(int *pDayOfWeek, int nTodayOfWeek)
{
	int i;
	
	for(i = nTodayOfWeek +1 ; i < 7 ; i++)
	{
		if (pDayOfWeek[i])
			return i;	
	}
	for(i = 0; i < nTodayOfWeek +1; i++)
	{
		if (pDayOfWeek[i])
			return i;
	}
	return -1;
}

BOOL ClockIncreaseDay(int n, SYSTEMTIME* pTime)
{
	SYSTEMTIME stNodeDate;
	WORD	   wOldYear;
	WORD	   wOldMonth;
	WORD	   wOldDay;
	int		   iDayOfMonth;
	

	// 暂存原有日期
	wOldYear  = pTime->wYear;
	wOldMonth = pTime->wMonth;
	wOldDay   = pTime->wDay;

	// 获取结点的日期(年份和月份)
	memset(&stNodeDate, 0, sizeof(SYSTEMTIME));
	stNodeDate.wYear  = pTime->wYear;
	stNodeDate.wMonth = pTime->wMonth;

	// 日递增n
	pTime->wDay += n;

	iDayOfMonth = CalcDaysOfMonth(&stNodeDate);
	// 判断日期是否上溢
	if (pTime->wDay > iDayOfMonth)
	{
		pTime->wMonth++;
		
		pTime->wDay = pTime->wDay - iDayOfMonth;

		// 判断月份是否上溢
		if (pTime->wMonth > 12)
		{
			pTime->wYear++;
			pTime->wMonth = 1;
			pTime->wDay = pTime->wDay - iDayOfMonth;

			// 判断年是否上溢
			if (pTime->wYear > 2037)
			{
				pTime->wYear  = wOldYear;
				pTime->wMonth = wOldMonth;
				pTime->wDay   = wOldDay;
				return FALSE;
			}
		}
	}

	return TRUE;
}

static int CalcDaysOfMonth(PSYSTEMTIME pDate)
{
	int nDaysOfMonth;

	switch (pDate->wMonth)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:	// 1、3、5、7、8、10、腊均为31天
			nDaysOfMonth = 31;
			break;

		case 2:		// 2月28或29天
			if (IsLeapYear(pDate->wYear))
			{
				// 闰年2月29天
				nDaysOfMonth = 29;
			}
			else
			{
				// 平年2月28天
				nDaysOfMonth = 28;
			}
			break;

		default:	// 其余月份均为30天
			nDaysOfMonth = 30;
			break;
	}

	return (nDaysOfMonth);
}

static BOOL IsLeapYear(WORD wYear)
{
	if ((wYear % 4 == 0 && wYear % 100 != 0) || (wYear % 400 == 0))
		return TRUE;
	else
		return FALSE;
}
