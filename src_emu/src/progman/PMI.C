                                     /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : $ version 3.0
 *
 * Purpose  : 
 *
\**************************************************************************/
#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"

#include <hopen/rtc.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <errno.h>
#include <ioctl.h>

#include "pmi.h"
#include "string.h"
#include "window.h"
#include "plx_pdaex.h"
#include <project/pmalarm.h>

//  MACRO DEFINE HERE
#define PMIS_MMC_PATH   "/mnt/fat16/"
#define PMIA_INI_MMC    0x0001
#define PMIA_HAS_MMC    0x0002
#define MAX_RTCTIMERS	10

/* Type of timer */
#define GENERAL_TIMER   1       // General timer
#define SYSTEM_TIMER    2       // System timer

#define MINIMUM_INTERVAL 5000	// minimum interval between two rtc timer 5s
#define MAXIMUM_INTERVAL 600000	// maximum interval between two rtc timer 10min

// RTCTimer Structure
typedef struct tagTIMERSTRUCT
{
    UINT        uIDEvent;        /* The id of the timer in the system */
    UINT        bUsed;          /* 0 : not used; 1 : general timer; 
                                ** 2 : system timer
                                */
    HWND       hWnd;     /* The thread id of the owner thread */
    DWORD        uMiliSeconds;   
    DWORD       uElapse;
} TIMERSTRUCT, *PTIMERSTRUCT;

// RTCTimer Global variable
static TIMERSTRUCT RTCTimerList[MAX_RTCTIMERS];		// RTC Timer
static unsigned int minimum_interval=0;

//  GLOBAL VARIABLE DEFINE HERE
static  HWND    g_hWnd;
static  UINT    g_uMsg;
static  DWORD   g_nAttrib;

static pthread_t pAlarm_thread;
static int       hRTC = -1;

static void * Thread_Alarm(void *pvoid);

// RTC Timer static Function
static UINT CreateRTCTimer(HWND hWnd, DWORD uIDEvent, DWORD uElapse, TIMERPROC pTimerFunc, BOOL bSystem)
{
	int i,j;
	unsigned int current;

	if ( !hWnd || (-1==hRTC))
	{
		printf("RTCTimer:RTC device is not opened or hWnd is NULL.\r\n");
		return FALSE;
	}

	// Determines whether the specified window handle identifies an existing 
    // window. If not, return 0.
    if (hWnd && !IsWindow(hWnd))
	{
		printf("RTCTimer:hWnd is not valid.\r\n");
        return FALSE;
	}

	if ( MINIMUM_INTERVAL > uElapse )
	{
		printf("RTCTimer:you set a small timeout timer.\r\n");
		return FALSE;
	}

    j = -1;

    // Determines whether the timer is already setted, if already setted, just
    // return the existing timer
    for (i = 0; i < MAX_RTCTIMERS; i++)
    {
        if (!RTCTimerList[i].bUsed)
        {
            if (j == -1)
                j = i;
        }
        else
        {
            if (RTCTimerList[i].hWnd == hWnd && RTCTimerList[i].uIDEvent == uIDEvent)
                return uIDEvent;
        }
    }

	// RTC Timer slots have been used up, return zero.
    if (j == -1)
	{
		printf("RTCTimer:no slot for new timer.\r\n");
        return FALSE;
	}

	ioctl(hRTC,RTC_READ_TIMER,&current);		// get current time

    RTCTimerList[j].hWnd         = hWnd;
    RTCTimerList[j].uIDEvent     = uIDEvent;
    RTCTimerList[j].uElapse      = current;		// current time, in second
    RTCTimerList[j].uMiliSeconds = uElapse;		// timeout milisecond

    // bUsed has to be set at the end
    RTCTimerList[j].bUsed        = bSystem ? SYSTEM_TIMER : GENERAL_TIMER;

	if ( 0==minimum_interval )					// first time
	{
		minimum_interval = uElapse;
		ioctl(hRTC,RTC_SET_TIMER,&uElapse);		// set rtc timer
	}
	if ( uElapse < minimum_interval )
	{
		minimum_interval = uElapse;
		ioctl(hRTC,RTC_SET_TIMER,&uElapse);		// set rtc timer when there is a small interval timeout
	}

    return RTCTimerList[j].uIDEvent;
}
static BOOL DestroyRTCTimer(HWND hWnd, UINT uIDEvent, BOOL bSystem)
{
	int i,j;

	if ( !hWnd )
		return FALSE;

    if (hWnd && !IsWindow(hWnd))
        return FALSE;

    for (i = 0,j=0; i < MAX_RTCTIMERS; i++)
    {
		if ( 0 == RTCTimerList[i].bUsed )
			j++;

        if (RTCTimerList[i].bUsed == (UINT)(bSystem ? SYSTEM_TIMER : GENERAL_TIMER))
        {
            if (RTCTimerList[i].hWnd == hWnd && 
                RTCTimerList[i].uIDEvent == uIDEvent)
                break;
        }
    }

	if ( MAX_RTCTIMERS == j )
	{
		ioctl(hRTC,RTC_DEL_TIMER);	// no rtc timer, delete rtc timer from device
		minimum_interval = 0;
	}

    if ( MAX_RTCTIMERS == i )
        return FALSE;

	// find a rtc timer and set delete flag
    RTCTimerList[i].bUsed = 0;

	// find a minimal interval timeout and set it again
	if ( minimum_interval==RTCTimerList[i].uMiliSeconds )
	{
		unsigned int tmpval = 0xffffffff;
		for ( j=0; j< MAX_RTCTIMERS; j++ )
		{
			if ( RTCTimerList[j].bUsed )
			{
				tmpval = ( tmpval>RTCTimerList[j].uMiliSeconds )?RTCTimerList[j].uMiliSeconds:tmpval;
			}
		}

		// reset rtc timer
		if ( tmpval>=MINIMUM_INTERVAL && tmpval<=MAXIMUM_INTERVAL )
		{
			minimum_interval = tmpval;
			ioctl(hRTC,RTC_SET_TIMER,&minimum_interval);
		}
	}

    for (i=0,j=0; i < MAX_RTCTIMERS; i++)
	{
		if ( 0 == RTCTimerList[i].bUsed )
			j++;
	}

	if ( MAX_RTCTIMERS == j )
	{
		ioctl(hRTC,RTC_DEL_TIMER);	// no rtc timer, delete rtc timer from device
		minimum_interval = 0;
	}

    return TRUE;
}

// RTC Timer interface
UINT WINAPI SetRTCTimer(HWND hWnd, UINT uIDEvent, DWORD uElapse, TIMERPROC pTimerFunc)
{
	return CreateRTCTimer(hWnd, uIDEvent, uElapse, pTimerFunc, 0);
}
BOOL WINAPI KillRTCTimer(HWND hWnd, UINT uIDEvent)
{
	return DestroyRTCTimer(hWnd, uIDEvent, 0);
}
/*************************************************************************

 * Function		:   DHI_SetRTC

 * Description	: 这个函数被窗口系统调用，在此函数中调用 PMALARM 设置时间的函数

 *				: PSYSTEMTIME pSystemTime

 * Return type	: BOOL  

 **************************************************************************/
BOOL    DHI_SetRTC(PSYSTEMTIME pSystemTime)
{	// notify pmalarm system time has be changed.
	RTCTIME pTime;

	pTime.v_nYear = (WORD)pSystemTime->wYear;
	pTime.v_nMonth = (unsigned char)pSystemTime->wMonth;
	pTime.v_nDay = (unsigned char)pSystemTime->wDay;
	pTime.v_nHour = (unsigned char)pSystemTime->wHour;
	pTime.v_nMinute = (unsigned char)pSystemTime->wMinute;
	pTime.v_nSecond = (unsigned char)pSystemTime->wSecond;
	pTime.v_nDayOfWeek = (unsigned char)pSystemTime->wDayOfWeek;

	RTC_SetTime(&pTime);

	return TRUE;
}

/*
 *	这个函数被 PMALARM 模块调用，将时间设置到设备中
 */
BOOL	DHI_SetRTC_(PSYSTEMTIME pSystemTime)
{
    struct  rtc_time    rtc_set_time;    
	struct timeval	time;
	struct timezone	zone;
	struct tm		tmset;
	struct tm*		tm;
	int ret;
    
	if(pSystemTime != NULL)
	{ 
		rtc_set_time.tm_year	= (int)(pSystemTime->wYear);
		rtc_set_time.tm_mon	= (int)(pSystemTime->wMonth);   
		rtc_set_time.tm_mday	= (int)(pSystemTime->wDay );
		rtc_set_time.tm_hour	= (int)(pSystemTime->wHour );
		rtc_set_time.tm_min	= (int)(pSystemTime->wMinute);
		rtc_set_time.tm_sec	= (int)(pSystemTime->wSecond);
		rtc_set_time.tm_msec	= 0;
		rtc_set_time.tm_wday	= (int)(pSystemTime->wDayOfWeek);
		
		tmset.tm_year = (int)(pSystemTime->wYear - 1900);	// year from	1900
		tmset.tm_mon  = (int)(pSystemTime->wMonth - 1);		// month		[0-11]
		tmset.tm_mday = (int)(pSystemTime->wDay );			// day			[1-31]
		tmset.tm_hour = (int)(pSystemTime->wHour );			// hour			[0-23]
		tmset.tm_min  = (int)(pSystemTime->wMinute);			// minutes		[0-59]
		tmset.tm_sec  = (int)(pSystemTime->wSecond);			// seconds		[0-60]
		tmset.tm_wday = (int)(pSystemTime->wDayOfWeek);		// day of week	[0-6]
		tmset.tm_isdst= 0;
	}
	else
	{
		printf("Invalid pointer\n");   
	}
	
    //set rtc time
    ret = ioctl( hRTC, RTC_SET_TIME, &rtc_set_time );
	
	if ( -1 == ret )
	{
		printf("Set RTC error!\r\n");
        return FALSE;
	}
	
	//set system time
	gettimeofday(&time,&zone);
	time.tv_sec = mktime(&tmset);
	settimeofday(&time,&zone);
	{
		tm = localtime(&time.tv_sec);
		printf("\r\nDHI_SetRTC:system time is:\r\n");
		printf("year=%4d  month=%2d  day=%2d  dayofweek=%d\r\n",tm->tm_year,tm->tm_mon+1,tm->tm_mday,tm->tm_wday);
		printf("hour=%4d  minutes=%2d  second=%2d \r\n",tm->tm_hour,tm->tm_min,tm->tm_sec);
		printf("timezone is %s\r\n",tm->tm_zone);
	}

	return  TRUE;		
}
/*************************************************************************

 * Function		:   DHI_GetRTC

 * Description	: 这个函数被窗口系统调用，在此函数中调用 PMALARM 设置时间的函数

 *				: PSYSTEMTIME pSystemTime

 * Return type	: BOOL  

 **************************************************************************/
BOOL    DHI_GetRTC(PSYSTEMTIME pSystemTime)
{
	RTCTIME pTime;

	RTC_GetTime(&pTime);

	pSystemTime->wYear		= (WORD)pTime.v_nYear;
	pSystemTime->wMonth		= (WORD)pTime.v_nMonth;
	pSystemTime->wDay		= (WORD)pTime.v_nDay;
	pSystemTime->wHour		= (WORD)pTime.v_nHour;
	pSystemTime->wMinute	= (WORD)pTime.v_nMinute;
	pSystemTime->wSecond	= (WORD)pTime.v_nSecond;
	pSystemTime->wDayOfWeek = (WORD)pTime.v_nDayOfWeek;

	return TRUE;
}

/*
 *	这个函数被 PMALARM 模块调用，将时间设置到设备中
 */
BOOL	DHI_GetRTC_(PSYSTEMTIME pSystemTime)
{
	struct  rtc_time    aTime;
	int ret;
	
    ret=ioctl( hRTC, RTC_RD_TIME, &aTime );
	
	pSystemTime->wYear		= (WORD)(aTime.tm_year) ;
	pSystemTime->wMonth		= (WORD)(aTime.tm_mon);   
	pSystemTime->wDay		= (WORD)(aTime.tm_mday ) ;
	pSystemTime->wHour		= (WORD)(aTime.tm_hour ) ;
	pSystemTime->wMinute	= (WORD)aTime.tm_min  ;
	pSystemTime->wSecond	= (WORD)aTime.tm_sec  ;
	pSystemTime->wMilliseconds = 0;
	pSystemTime->wDayOfWeek = (WORD)aTime.tm_wday ;
    
	return  TRUE;	
}
/*************************************************************************

 * Function		:   DHI_CheckAlarm
 * Description	: 
 *				: void
 * Return type	: BOOL  
 **************************************************************************/
static BOOL DHI_CheckAlarm(void)
{  
	UINT current;
	int i;
	UINT flag=0;

	ioctl(hRTC,RTC_READ_EVENT,&flag);	// read event flag, whether alarm or timer

	if ( flag & 0x01 )					// alarm
	{
#ifndef _EMULATE_
		/* Disable alarm interrupts */
		if ( -1 == ioctl(hRTC,RTC_AIE_OFF,0)) 
			perror("rtc ioctl error turn off alarm!\r\n");
#endif
		if ( NULL == g_hWnd )   
			return  FALSE;

		PostMessage( g_hWnd, g_uMsg, 0, 0 );
    
		g_hWnd = NULL;
	}

	if ( flag & 0x02 )					// timer
	{
		ioctl(hRTC,RTC_READ_TIMER,&current);

		for ( i=0;i<MAX_RTCTIMERS;i++ )
		{
			if ( RTCTimerList[i].bUsed && 
				(RTCTimerList[i].uElapse+RTCTimerList[i].uMiliSeconds/1000 <= current) )	// there is a event of timeout
			{
				printf("\r\n post wm_rtctimer message !");
				PostMessage(RTCTimerList[i].hWnd,WM_RTCTIMER,RTCTimerList[i].uIDEvent,0);
				RTCTimerList[i].uElapse = current;
			}
		}
	}
    
    return  TRUE;
}
/*************************************************************************

 * Function		: DHI_SetAlarm
 * Description	: 
 * Argument		: hWnd
 *				: UINT uMsgCmd
 *				: PALARMTIME  pAlarmTime
 * Return type	: BOOL  
 **************************************************************************/
BOOL    DHI_SetAlarm(HWND hWnd, UINT uMsgCmd, PALARMTIME  pAlarmTime)
{
	
	struct  rtc_time    aTime;
	int     retval;	
		
	g_hWnd  = hWnd;
	g_uMsg  = uMsgCmd;

	aTime.tm_year   = (int)(pAlarmTime->wYear);
	aTime.tm_mon    = (int)(pAlarmTime->wMonth);   
	aTime.tm_mday   = (int)(pAlarmTime->wDay );
	aTime.tm_hour   = (int)(pAlarmTime->wHour );
	aTime.tm_min    = (int)pAlarmTime->wMinute;
	aTime.tm_sec    = (int)pAlarmTime->wSecond;
	
	// set alarm time
	retval = ioctl( hRTC, RTC_ALM_SET, &aTime );

	if (retval == -1) 	
	{
		printf("set rtc error!\r\n");
		return FALSE;
	}
		
//	printf("before open interrupt ********\r\n");
	
#ifndef _EMULATE_
	retval = ioctl(hRTC, RTC_AIE_ON, 0);
	
	if (retval == -1) 	
	{
		printf("turn on alarm error!\r\n");
		return FALSE;
	}
#endif		
		
    return  TRUE;
}

/*
 *	
 */
BOOL    DHI_KillAlarm(void)
{
	struct  rtc_time    aTime;

	memset(&aTime,0,sizeof(aTime));
	if ( 0<hRTC )
	{
		printf("\r\nRTC is not open\r\n");
		return -1;
	}
#ifndef _EMULATE_
		/* Disable alarm interrupts */
	if ( -1 == ioctl(hRTC,RTC_AIE_OFF,0)) 
		perror("rtc ioctl error turn off alarm!\r\n");
#else
	ioctl( hRTC, RTC_ALM_SET, &aTime );
#endif
	return 0;
}
/*************************************************************************
 * Function		:   MMC_CheckCardStatus
 * Description	: 
 *				: void
 * Return type	: BOOL  
 **************************************************************************/

void * Thread_Alarm(void *pvoid)
{
//	SYSTEMTIME	systime;
    int retval;
	struct pollfd fds = {0, 0, POLLIN, 0};

	fds.fd = hRTC;

	while (1)
	{
		printf("rtc:before poll.........\r\n");

		retval = poll(&fds, 1, -1);	// wait until timeout

		read(hRTC,&retval,4);

		if (retval == -1)
		{
			printf("poll error\r\n");
		}

		if (fds.revents & POLLIN)
			printf("rtc event is coming!\r\n");

		DHI_CheckAlarm();
	}
    return (void*)pAlarm_thread;
}
/*************************************************************************
  open rtc dev
 **************************************************************************/
int DHI_OpenRTC(void)
{
	int retval;
	
    hRTC = open("/dev/rtc", O_RDONLY);
    if(hRTC < 0)
	{
        printf("open /dev/rtc failed\r\n");
		return -1;
	}

	retval = pthread_create(&pAlarm_thread, NULL, Thread_Alarm, (void *)0);

	if (retval != 0) 	
	{
		printf("create alarm thread error!\r\n");
		return -1;
	}

    return hRTC;
}
/*************************************************************************
  close rtc dev
 **************************************************************************/
void DHI_CloseRTC(void)
{

    close(hRTC);
}

void KickDog(void)
{
	static kickdogerrorflag;
#ifndef _EMULATE_
#define CLEAR_1SECOND	3250000
	if ( hRTC<0 )
	{
		if ( !kickdogerrorflag )
		{
			// maybe the rtc device is not be opened, open it again
			hRTC = open("/dev/rtc",O_RDWR);
			if ( hRTC<0 )
			{
				kickdogerrorflag = 1;
				printf("\r\nKickDog error.\r\n");
				return;
			}
			ioctl(hRTC, WATCHDOG_TIMEOUT_VALUE, 30 * CLEAR_1SECOND);
			close(hRTC);
			hRTC = -1;
		}
		return;
	}
	ioctl(hRTC, WATCHDOG_TIMEOUT_VALUE, 30 * CLEAR_1SECOND);
#endif
}

void EnableWatchDog(void)
{
#ifndef _EMULATE_
	if ( hRTC<0 )
	{
		// maybe the rtc device is not be opened, open it again
		hRTC = open("/dev/rtc",O_RDWR);
		if ( hRTC<0 )
		{
			printf("\r\EnableWatchDog error.\r\n");
			return;
		}
		ioctl(hRTC, ENABLE_WATCHDOG, 0);
		close(hRTC);
		hRTC = -1;
		return;
	}
	ioctl(hRTC, ENABLE_WATCHDOG, 0);
#endif
}

void DisableWatchDog(void)
{
#ifndef _EMULATE_
	if ( hRTC<0 )
	{
		// maybe the rtc device is not be opened, open it again
		hRTC = open("/dev/rtc",O_RDWR);
		if ( hRTC<0 )
		{
			printf("\r\DisableWatchDog error.\r\n");
			return;
		}
		ioctl(hRTC, DISABLE_WATCHDOG, 0);
		close(hRTC);
		hRTC = -1;
		return;
	}
	ioctl(hRTC, DISABLE_WATCHDOG, 0);
#endif
}

/*
 *	return value: 
 *	TRUE: watchdog is opened
 *	FALSE:watchdog is closed
 */
BOOL GetWatchDogState(void)
{
#ifndef _EMULATE_
	BOOL state = FALSE;

	if ( hRTC<0 )
	{
		// maybe the rtc device is not be opened, open it again
		hRTC = open("/dev/rtc",O_RDWR);
		if ( hRTC<0 )
		{
			printf("\r\GetWatchDogState error.\r\n");
			return FALSE;
		}
		ioctl(hRTC, GET_WATCHDOG_STATE, &state);
		close(hRTC);
		hRTC = -1;
		return state;
	}
	ioctl(hRTC, GET_WATCHDOG_STATE, &state);
	return state;
#else
	static BOOL bState = FALSE;
	if ( bState )
		bState = FALSE;
	else
		bState = TRUE;
	return bState;
#endif
}



