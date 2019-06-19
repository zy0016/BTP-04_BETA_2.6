               /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : alarm manager $ version 3.0
 *
 * Purpose  : Implements  alarm for PDA .
 *            
\**************************************************************************/

#include	"pmalarm.h"
#include	"progman.h"
#include	"sys/types.h"
#include	"sys/stat.h"
#include	"ctype.h"
#include	"fcntl.h"
#include	"stdlib.h"
#include	"stdio.h"
#include	"string.h"
#include	"dirent.h"
#include	"unistd.h"

#include	"pmi.h"
#include	"malloc.h"
#include	<pm/AppFile.h>

#define	USE_DUMMY	1

#define ALARM_ME		0
#define ALARM_CPU		1

#define WM_ALARM_ON			WM_USER + 1011
#define WM_ALARM_SET		WM_USER + 1012
#define WM_ALARM_POWERON	WM_USER + 1013
#define WM_ALARM_ME_TIME	WM_USER + 1014
#define WM_ALARM_TIME_CHANGE	(WM_USER + 1015)
/**************************************************************************
*	Purpose:	DEFINE INLINE MACRO CONST   
*	Remarks:	   
*
**************************************************************************/
#define MAX_CHAR		23
#define MAX_TIMER		0x000A
#define	MAX_ALARM		0x0020
#define MAX_RUBBISH		0x000A
#define MAX_APP			0x05
#define MAX_YEAR        2099
#define BASE_YEAR       1980
#define	TIME_PRECISION	0x10 // delay next alarm in seconds when set alarm to MC55
#define	TIME_DELAY		0x20 // delay next alarm in seconds when the phone is under power off process

#define PMALARM_CLASS	"AlarmBaseClass"


#define	IS_CODE			0xBB	// item flag, 0xBB:validate item, 0x00:deleted item, other:error
#define	IS_ALARM		0x8000
#define	IS_TIMER		0x1000

#define	HAS_INFILE		0x0001
#define HAS_AUTONEXTDAY	0x0002
#define	HAS_RELATIVE	0x0008 

/**************************************************************************
*	Purpose:	DEFINE INLINE MACRO CONST   
*	Remarks:	   
*
**************************************************************************/
#define SECOND_MASK         0x00003F00
#define MINUTE_MASK         0x000FC000
#define HOUR_MASK           0x01F00000
    
#define DAY_MASK            0x0000001F
#define MONTH_MASK          0x000001E0
#define YEAR_MASK           0x001FFE00
    
#define DAY_SHIFT_BITS      0
#define MONTH_SHIFT_BITS    5
#define YEAR_SHIFT_BITS     9

#define SECOND_SHIFT_BITS   8
#define MINUTE_SHIFT_BITS   14
#define HOUR_SHIFT_BITS     20 
    
#define GETDAY(d)           (BYTE)(((DWORD)(d)&DAY_MASK)>>DAY_SHIFT_BITS)
#define GETMONTH(d)         (BYTE)(((DWORD)(d)&MONTH_MASK)>>MONTH_SHIFT_BITS)
#define GETYEAR(d)          (WORD)(((DWORD)(d)&YEAR_MASK)>>YEAR_SHIFT_BITS)

#define GETSECOND(t)		(BYTE)(((DWORD)(t)&SECOND_MASK)>>SECOND_SHIFT_BITS)
#define GETMINUTE(t)        (BYTE)(((DWORD)(t)&MINUTE_MASK)>>MINUTE_SHIFT_BITS)
#define GETHOUR(t)          (BYTE)(((DWORD)(t)&HOUR_MASK)>>HOUR_SHIFT_BITS)

#define MAKETIME(h, m, s)   (((DWORD)(h)) << HOUR_SHIFT_BITS) | \
    (((DWORD)(m)) << MINUTE_SHIFT_BITS) | (((DWORD)(s)) << SECOND_SHIFT_BITS)
#define MAKEDATE(y, m, d)   (((DWORD)(y)) << YEAR_SHIFT_BITS) | \
    (((DWORD)(m)) << MONTH_SHIFT_BITS) | (((DWORD)(d)) << DAY_SHIFT_BITS)
/**************************************************************************
 *	purpose	:	TYPE DEFINE HERE
 *	remark	:
 *
 **************************************************************************/
typedef	struct registeredapp_s
{
	i_INT1	v_AppItem[MAX_CHAR+1];
}	REGISTEREDAPP,*PREGISTEREDAPP;

typedef	struct	timeritem_s 
{	
	u_INT4	v_nTimeBase;	
	u_INT4	v_nTimeElapse;
	u_INT2	v_nID;			//
	u_INT2	v_nNext;

	i_INT1	v_aAppExec[MAX_CHAR+1];			// the name of the app

}	TIMERITEM, *PTIMERITEM;

typedef	struct	alarmdata_s 
{		
	struct	rtctime_s	v_aTime;	
	u_INT2	v_nID;	
	u_INT2	v_nAttrib;						// has_infile or has_relative or has_autonextday

	i_INT1	v_aAppItem[MAX_CHAR+1];			//the name of app which set alarm
	i_INT1	v_aAppExec[MAX_CHAR+1];		    // the name of the app when timeout to run 

}	ALARMDATA, *PALARMDATA;

typedef	struct	alarmitem_s 
{		
	struct	alarmdata_s v_aData;		
	
	u_INT2	v_nFilePos;					// item location in file
	u_INT2	v_nNext;					// next item of the chain

}	ALARMITEM, *PALARMITEM;

typedef struct	alarm_s	
{
	HWND		v_hWnd;
	PTIMERITEM	v_pTimer;
	PALARMITEM	v_pAlarm;

	PREGISTEREDAPP	v_pRegisteredApp;

	u_INT2	v_iAlarm;				// the index to the first used alarm item
	u_INT2	v_iTimer;
	u_INT2	v_iAlarmRoom;			// the index to unused alarm
	u_INT2	v_iTimerRoom;
	
	u_INT1	v_nAlarmRoom;			// the space number of all
	u_INT1	v_nTimerRoom;
	u_INT1	v_nAlarmRubbish;
	u_INT1	v_nAlarmSource;				// alarm source device
	
}	ALARM, *HALARM;
/*************************************************************************
 *	purpose	:	DEFINE GLOBAL VARIABLE
 *	remark	:
 *
 **************************************************************************/
static	const	u_INT2	g_nMonthDays[] = 
{
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};
static	const	u_INT1	g_nDayOfMonth[] = 
{
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
static	HALARM			g_hObj;

/**************************************************************************
 *	purpose	:	PROGRAM INLINE FUNCTION DECLARE
 *	remark	:
 *
 **************************************************************************/
static	BOOL	f_Create(VOID);
static	VOID	f_Release(VOID);

static	void	f_FileCheck( VOID );
static	void	f_FileClean( VOID );
static	void	f_FileCheckIn ( u_INT1 nIndex );
static	void	f_FileCheckOut( u_INT1 nIndex );

static	void	f_AlarmCheckIn( u_INT1 nIndex );
static	u_INT2	f_AlarmCheckOut(i_INT1* pApp, u_INT2 nID );
static	void	f_TimerCheckIn (u_INT1 nIndex );
static	u_INT2	f_TimerCheckOut(i_INT1* pApp, u_INT2 nID );

static	i_INT1	f_CompareTime( PRTCTIME pSrc, PRTCTIME pDst );
static	u_INT1	f_GetDayOfWeek(PRTCTIME pTime);
static	u_INT4	f_CalcSeconds(PRTCTIME pTime);
static	BOOL	f_IsLeapYear( unsigned short nYear );
static	BOOL	f_IsTimeValid(PRTCTIME pTime);
static	void	f_AddSpanToTime( PRTCTIME pTime, u_INT4 nSeconds);

static	void	f_RTCSetAlarm(void);
static	void	f_RTCCallBack(void * nParam, int nEvent);
static	void	f_RTCChangeTime(PRTCTIME pPrevTime);

static	LRESULT	CALLBACK f_WndProc(HWND hWnd, UINT wMsgCmd, 
								   WPARAM wParam, LPARAM lParam);
extern	BOOL	PM_IsPowerOff(void);
extern	BOOL	IsAlarmPowerOn(void);
extern	PAPPADM	GetAppMain(void);
extern	void	ChargeOnlyState(void);
extern	BOOL	MobileInit2(void);
extern	BOOL	Sett_GetAutoCloseInfo(int *nHour, int *nMinute);
/**************************************************************************
 * Function		: pmalarm_debug
 * Description	: print information of pmalarm
 *				: VOID
 * Return type	: 
 **************************************************************************/
void pmalarm_debug(void)
{
	u_INT1 iAlarm,iAlarmRoom;
	int nAlarm,nAlarmRoom;
	nAlarm = 0;
	nAlarmRoom = 0;
	
	printf("\r\nPMAlarm debug information...\r\n");
	printf("Max alarm number = %d\r\n",MAX_ALARM);
	printf("Max timer number = %d\r\n",MAX_TIMER);
	printf("Max rubbish      = %d\r\n",MAX_RUBBISH);
	printf("PMAlarm file is %s\r\n",PMS_FILE_NAME);

	if (!g_hObj)
	{
		printf("PMAlarm is not initialized!\r\n");
		return;
	}

	if ( ALARM_ME == g_hObj->v_nAlarmSource )
		printf("PMAlarm source device is MC55\r\n");
	else if ( ALARM_CPU == g_hObj->v_nAlarmSource )
		printf("PMAlarm source device is PXA271\r\n");
	else
		printf("Error! No PMAlarm source device!!!\r\n");

	if ( 0!=g_hObj->v_iAlarm )
	{
		RTCTIME rtctime;
		// alarm chain is not empty
		iAlarm = g_hObj->v_iAlarm&0xff;
		nAlarm++;
		printf("alarm[%d]'s id is %d\r\n",iAlarm,g_hObj->v_pAlarm[iAlarm].v_aData.v_nID);
		printf("alarm[%d] is belone to %s\r\n",iAlarm,g_hObj->v_pAlarm[iAlarm].v_aData.v_aAppExec);
		rtctime.v_nYear		= g_hObj->v_pAlarm[iAlarm].v_aData.v_aTime.v_nYear;
		rtctime.v_nMonth	= g_hObj->v_pAlarm[iAlarm].v_aData.v_aTime.v_nMonth;
		rtctime.v_nDay		= g_hObj->v_pAlarm[iAlarm].v_aData.v_aTime.v_nDay;
		rtctime.v_nHour		= g_hObj->v_pAlarm[iAlarm].v_aData.v_aTime.v_nHour;
		rtctime.v_nMinute	= g_hObj->v_pAlarm[iAlarm].v_aData.v_aTime.v_nMinute;
		rtctime.v_nSecond	= g_hObj->v_pAlarm[iAlarm].v_aData.v_aTime.v_nSecond;
		printf("Year=%4d Month=%2d Day=%2d\r\n",rtctime.v_nYear,rtctime.v_nMonth,rtctime.v_nDay);
		printf("Hour=%2d   Minut=%2d Sec=%2d\r\n",rtctime.v_nHour,rtctime.v_nMinute,rtctime.v_nSecond);
		while( 0!=g_hObj->v_pAlarm[iAlarm].v_nNext )
		{
			nAlarm++;	// total number of alarm items
			iAlarm = g_hObj->v_pAlarm[iAlarm].v_nNext&0xff;
			printf("alarm[%d]'s id is %d\r\n",iAlarm,g_hObj->v_pAlarm[iAlarm].v_aData.v_nID);
			printf("alarm[%d] is belone to %s\r\n",iAlarm,g_hObj->v_pAlarm[iAlarm].v_aData.v_aAppExec);
			rtctime.v_nYear		= g_hObj->v_pAlarm[iAlarm].v_aData.v_aTime.v_nYear;
			rtctime.v_nMonth	= g_hObj->v_pAlarm[iAlarm].v_aData.v_aTime.v_nMonth;
			rtctime.v_nDay		= g_hObj->v_pAlarm[iAlarm].v_aData.v_aTime.v_nDay;
			rtctime.v_nHour		= g_hObj->v_pAlarm[iAlarm].v_aData.v_aTime.v_nHour;
			rtctime.v_nMinute	= g_hObj->v_pAlarm[iAlarm].v_aData.v_aTime.v_nMinute;
			rtctime.v_nSecond	= g_hObj->v_pAlarm[iAlarm].v_aData.v_aTime.v_nSecond;
			printf("Year=%4d Month=%2d Day=%2d\r\n",rtctime.v_nYear,rtctime.v_nMonth,rtctime.v_nDay);
			printf("Hour=%2d   Minut=%2d Sec=%2d\r\n",rtctime.v_nHour,rtctime.v_nMinute,rtctime.v_nSecond);
		}
		printf("total alarm number is %d\r\n",nAlarm);
		if ( nAlarm!=(MAX_ALARM-g_hObj->v_nAlarmRoom) )
			printf("there are some error in calculating alarm item,please check it.\r\n");
	}
	else
	{
		printf("no alarm item.\r\n");
	}

	if ( 0!=g_hObj->v_iAlarmRoom )
	{
		// available chain is not empty
		iAlarmRoom = g_hObj->v_iAlarmRoom&0xff;
		nAlarmRoom++;
		printf("alarm[%d] is available\r\n",iAlarmRoom);
		while( 0!=g_hObj->v_pAlarm[iAlarmRoom].v_nNext )
		{
			nAlarmRoom++;
			iAlarmRoom = g_hObj->v_pAlarm[iAlarmRoom].v_nNext&0xff;
			printf("alarm[%d] is available\r\n",iAlarmRoom);
		}
		
		printf("total available number is %d",nAlarmRoom);
		if ( nAlarmRoom!=g_hObj->v_nAlarmRoom )
			printf("there are some error in calculating available number, please check it.\r\n");
	}
	else
	{
		printf("no available item.\r\n");
	}

	printf("\r\n");
}

/**************************************************************************
 * Function		: f_Create
 * Description	: 
 *				: VOID
 * Return type	: 
 **************************************************************************/
static	BOOL	f_Create(VOID)
{
	WNDCLASS ws;
	register u_INT1 i;

	g_hObj = (HALARM)malloc(sizeof(ALARM));
	if (NULL == g_hObj)
		return	FALSE;

	memset(g_hObj, 0, sizeof(ALARM));

	if ( !GetClassInfo(NULL, PMALARM_CLASS, &ws) )
	{		
		ws.style			= 0;	
		ws.lpfnWndProc		= (WNDPROC)f_WndProc;
		ws.cbClsExtra		= 0;
		ws.cbWndExtra		= 0;
		ws.hInstance		= NULL;
		ws.hIcon			= NULL;
		ws.hCursor			= NULL;
		ws.hbrBackground	= NULL;
		ws.lpszMenuName		= NULL;
		ws.lpszClassName	= PMALARM_CLASS;	
		
		if ( !RegisterClass( &ws ) )
		{
			printf("PMAlarm: Register class error!\r\n");
			goto class_error;
		}
	}
	g_hObj->v_hWnd = CreateWindow ( PMALARM_CLASS, "",
		0, 0, 0,0, 0, NULL, NULL, NULL, NULL );
	if (NULL == g_hObj->v_hWnd)
	{
		printf("PMAlarm: Create window error!\r\n");
		goto window_error;
	}

	g_hObj->v_pAlarm = (PALARMITEM)malloc(sizeof(ALARMITEM)*MAX_ALARM);
	if (NULL == g_hObj->v_pAlarm)
	{
		printf("PMAlarm: malloc alarm space error!\r\n");
		goto malloc_alarm_error;
	}
	memset(g_hObj->v_pAlarm,0,sizeof(ALARMITEM)*MAX_ALARM);

	g_hObj->v_pTimer = (PTIMERITEM)malloc(sizeof(TIMERITEM)*MAX_TIMER);
	if (NULL == g_hObj->v_pTimer)
	{
		printf("PMAlarm: malloc timer space error!\r\n");
		goto malloc_timer_error;
	}
	memset(g_hObj->v_pTimer,0,sizeof(TIMERITEM)*MAX_TIMER);

	g_hObj->v_pRegisteredApp = (PREGISTEREDAPP)malloc(sizeof(REGISTEREDAPP)*MAX_APP);
	if (NULL == g_hObj->v_pRegisteredApp )
	{
		printf("PMAlarm: malloc app space error!\r\n");
		goto malloc_app_error;
	}
	memset(g_hObj->v_pRegisteredApp,0,sizeof(REGISTEREDAPP)*MAX_APP);
	
	for (i = 0; i < MAX_ALARM - 1; i++)
		g_hObj->v_pAlarm[i].v_nNext = (i+1)|IS_ALARM;

	g_hObj->v_pAlarm[i].v_nNext = 0;
	g_hObj->v_iAlarmRoom = IS_ALARM;	// available item chian
	g_hObj->v_nAlarmRoom = MAX_ALARM;	// available item number

	for (i = 0; i < MAX_TIMER - 1; i++)
		g_hObj->v_pTimer[i].v_nNext = (i+1)|IS_TIMER;

	g_hObj->v_pTimer[i].v_nNext = 0;
	g_hObj->v_iTimerRoom = IS_TIMER;
	g_hObj->v_nTimerRoom = MAX_TIMER;

	f_FileCheck();

	// register Me urc
	if ( -1==ME_RegisterIndication(ME_URC_CALA, IND_PRIORITY, g_hObj->v_hWnd, WM_ALARM_ON) )
	{
		printf("\r\npmalarm register URC error!\r\n");
		// register urc error, just use alarm of cpu
		g_hObj->v_nAlarmSource = ALARM_CPU;
	}
	else
		g_hObj->v_nAlarmSource = ALARM_ME;

	return	TRUE;
	
malloc_app_error:
	free(g_hObj->v_pTimer);
malloc_timer_error:
	free(g_hObj->v_pAlarm);
malloc_alarm_error:
	DestroyWindow(g_hObj->v_hWnd);
window_error:
	UnregisterClass(PMALARM_CLASS, NULL);
class_error:
	free(g_hObj);
	g_hObj = NULL;
	return	FALSE;
}

/**************************************************************************
 * Function		: f_Release
 * Description	: 
 *				: VOID
 * Return type	: 
 **************************************************************************/
static	VOID	f_Release(VOID)
{
	if (NULL == g_hObj)
		return;

	if (IsWindow(g_hObj->v_hWnd))
		DestroyWindow(g_hObj->v_hWnd);

	//	reset clock
	if (0 != g_hObj->v_iAlarm)
	{
		g_hObj->v_iTimer = 0;
//		g_hObj->v_iAlarm = 0;
		printf("\r\npmalarm: RTC_Release is called, we reset alarm to MC55.\r\n");
		f_RTCSetAlarm();
	}
	if (NULL != g_hObj->v_pTimer)
		free(g_hObj->v_pTimer);

	if (NULL != g_hObj->v_pAlarm)
		free(g_hObj->v_pAlarm);

	free(g_hObj);
	g_hObj = NULL;

	UnregisterClass(PMALARM_CLASS, NULL);
}

/**************************************************************************
 * Function		: f_FileCheck
 * Description	: Read saved alarm&timer node from file
 *				: 
 * Return type	: VOID
 **************************************************************************/
static	void	f_FileCheck( VOID )
{
	HFILE	hf;
	int		nRead = 1;
	u_INT2	nPos, nSize;
	u_INT1	nByte, nIndex;	
	
	hf = open( PMS_FILE_NAME, O_RDONLY);
	if (-1 == hf)
		return;

	nSize = sizeof(ALARMDATA);
	
	while (0 != nRead)
	{
		nPos	= (u_INT2)lseek( hf, 0, SEEK_CUR );	// set read pointor at current location
		nRead	= read( hf, &nByte, 1 );
		if (1 != nRead)
			break;

		//	the node has been deleted
		if ( 0 == nByte )
		{
			g_hObj->v_nAlarmRubbish++;
			lseek(hf, nSize, SEEK_CUR);
			continue;
		}
		else if (IS_CODE != nByte)//	error happened
		{
			printf("\r\npmalarm: we find error in file, so f_FileClean was called.\r\n");
			close(hf);
			f_FileClean();
			return;
		}
		nIndex	= (u_INT1)(g_hObj->v_iAlarmRoom&0xff);		// get an available item
		nRead	= read(hf, &g_hObj->v_pAlarm[nIndex].v_aData, nSize);
		if (nRead != nSize)
		{
			// there are must some error, so we have to clean the file
			printf("\r\npmalarm: we read error from file, so f_FileClean was called.\r\n");
			close(hf);
			f_FileClean();
			return;
		}
		g_hObj->v_iAlarmRoom = g_hObj->v_pAlarm[nIndex].v_nNext;
		g_hObj->v_nAlarmRoom--;
		g_hObj->v_pAlarm[nIndex].v_nFilePos = nPos;			// save item location in the file
		// HAS_INFILE has been recorded in file , we just read out it
		//g_hObj->v_pAlarm[nIndex].v_aData.v_nAttrib = HAS_INFILE;	// mark the item is in file
		{
			PRTCTIME pTime;
			pTime = &g_hObj->v_pAlarm[nIndex].v_aData.v_aTime;

			printf("\r\npmalarm: add alarm item to alarm chain:\r\n");
			printf("alarm belong to %s\r\n",g_hObj->v_pAlarm[nIndex].v_aData.v_aAppExec);
			printf("Year:%4d,Month:%2d,Day:%2d\r\n",pTime->v_nYear,pTime->v_nMonth,pTime->v_nDay);
			printf("Hour:%4d,Minut:%2d,Sec:%2d\r\n",pTime->v_nHour,pTime->v_nMinute,pTime->v_nSecond);
		}

		f_AlarmCheckIn(nIndex);								// add a new alarm item into alarm chain
	}
	close( hf );
}

/**************************************************************************
 * Function		: f_FileClean
 * Description	: clean up the file and write new data into it
 *				: 
 * Return type	: VOID
 **************************************************************************/
static	void	f_FileClean( VOID )
{
	HFILE	hf;
	u_INT2	nID, nWrite, nSize, nPos = 0;
	u_INT1	nIndex, nByte = IS_CODE;	
	
	g_hObj->v_nAlarmRubbish = 0;

	hf = open(PMS_FILE_NAME, O_RDWR|O_TRUNC, S_IRWXU);	// delete file if exist
	if (-1 == hf)
		return;

	nID		= g_hObj->v_iAlarm;
	nSize	= sizeof(ALARMDATA);
	
	// re-write alarm data to file
	while (0 != nID)
	{
		nIndex = (u_INT1)(nID&0xff);
		if (g_hObj->v_pAlarm[nIndex].v_aData.v_nAttrib & HAS_INFILE)
		{
			nWrite = write( hf, &nByte, 1 );
			if (1 != nWrite)
				break;

			g_hObj->v_pAlarm[nIndex].v_nFilePos = nPos;
			nWrite = write(hf, &g_hObj->v_pAlarm[nIndex].v_aData, nSize);
			if (nWrite != nSize)
				break;

			nPos += nSize+1;
		}
		nID = g_hObj->v_pAlarm[nIndex].v_nNext;
	}
	close( hf );	
}

/**************************************************************************
 * Function		: f_FileCheckIn
 * Description	: add a item into the end of file
 *				: u_INT1 nIndex
 * Return type	: 
 **************************************************************************/
static	void	f_FileCheckIn(u_INT1 nIndex )
{
	HFILE	hf;
	u_INT1	nByte = IS_CODE;
	
	hf = open(PMS_FILE_NAME, O_WRONLY, S_IRWXU);	
	if (-1 == hf)
	{
		hf = open( PMS_FILE_NAME, O_RDWR|O_CREAT, S_IRWXU);
		if (-1 == hf)
			return;
	}

	// move file pointer to the end of the file
	g_hObj->v_pAlarm[nIndex].v_nFilePos = 
		(u_INT2)lseek(hf, 0, SEEK_END);
	g_hObj->v_pAlarm[nIndex].v_aData.v_nAttrib |= HAS_INFILE;

	write(hf, &nByte, 1 );	// validate flag of an item
	write(hf, &g_hObj->v_pAlarm[nIndex].v_aData, sizeof(ALARMDATA));	
	close(hf);
}

/**************************************************************************
 * Function		: f_FileCheckOut
 * Description	: mark an item deleted in the file
 *				: u_INT1 nIndex
 * Return type	: 
 **************************************************************************/
static	void	f_FileCheckOut( u_INT1 nIndex )
{
	HFILE	hf;
	u_INT1	nByte = 0;

	// specified alarm item is not in file , just return
	if (!(g_hObj->v_pAlarm[nIndex].v_aData.v_nAttrib & HAS_INFILE))
		return;

	g_hObj->v_pAlarm[nIndex].v_aData.v_nAttrib &= ~HAS_INFILE;
	g_hObj->v_nAlarmRubbish++;

	hf = open(PMS_FILE_NAME, O_RDWR|O_CREAT, S_IRWXU);	
	if (-1 == hf)
		return;

	// move file pointer to the start location of specified item
	lseek(hf, g_hObj->v_pAlarm[nIndex].v_nFilePos, SEEK_SET );
	write(hf, &nByte, 1 );
	close(hf);
}

/**************************************************************************
 * Function		: f_AlarmCheckIn
 * Description	: add a new alarm into pmalarm
 *				: nIndex is in neither alarm chain nor available chain
 *				: u_INT1 nIndex
 * Return type	: 
 **************************************************************************/
static	void	f_AlarmCheckIn( u_INT1 nIndex )
{
	u_INT1	i, iNext;

	if ( 0 == g_hObj->v_iAlarm )	// alarm item chain is empty
	{
		g_hObj->v_pAlarm[nIndex].v_nNext = 0;	// first alarm item, its next item is NULL
		g_hObj->v_iAlarm = nIndex|IS_ALARM;		
		return;
	}
	i = (u_INT1)(g_hObj->v_iAlarm&0xff);		// get first item index

	// insert new item at the beginning of the alarm chain
	if ( f_CompareTime( &g_hObj->v_pAlarm[i].v_aData.v_aTime, 
		&g_hObj->v_pAlarm[nIndex].v_aData.v_aTime) <= 0 )
	{
		// 新节点的时间在第一个节点时间之前
		g_hObj->v_pAlarm[nIndex].v_nNext = g_hObj->v_iAlarm;
		g_hObj->v_iAlarm = nIndex|IS_ALARM;	
		return;
	}

	// insert new item at a proper place of the alarm chain
	while ( 0 != g_hObj->v_pAlarm[i].v_nNext )
	{
		iNext = (u_INT1)(g_hObj->v_pAlarm[i].v_nNext&0xff);

		// 当 iNext 的时间在 nIndex 的时间之后，退出循环
		if ( f_CompareTime( &g_hObj->v_pAlarm[iNext].v_aData.v_aTime, 
			&g_hObj->v_pAlarm[nIndex].v_aData.v_aTime) <= 0 ) 
			break;
		
		i = iNext;
	}
	g_hObj->v_pAlarm[nIndex].v_nNext = g_hObj->v_pAlarm[i].v_nNext;
	g_hObj->v_pAlarm[i].v_nNext = nIndex|IS_ALARM;
}

/**************************************************************************
 * Function		: f_AlarmCheckOut
 * Description	: delete an alarm from alarm chain
 * Argument		: pApp
 *				: u_INT2 nID
 * Return type	: 
 **************************************************************************/
static	u_INT2	f_AlarmCheckOut(i_INT1* pApp, u_INT2 nID )
{
	u_INT2	nRet;
	u_INT1	i, iNext;	

	if (0 == g_hObj->v_iAlarm)	// alarm chain is empty
		return	0;

	nRet = g_hObj->v_iAlarm;	// beginning of the chain
	i = (u_INT1)(nRet&0xff);

	if (nID == g_hObj->v_pAlarm[i].v_aData.v_nID && 
		0 == strcmp((const char*)pApp, 
		(const char*)g_hObj->v_pAlarm[i].v_aData.v_aAppItem))
	{
		// 将第一个节点从报警链表中去掉
		g_hObj->v_iAlarm = g_hObj->v_pAlarm[i].v_nNext;		// point to next alarm item
		return	nRet;
	}
	nRet = g_hObj->v_pAlarm[i].v_nNext;
	while ( nRet > 0)
	{
		iNext = (u_INT1)(nRet&0xff);

		if (nID == g_hObj->v_pAlarm[iNext].v_aData.v_nID && 
			0 == strcmp((const char*)pApp, 
			(const char*)g_hObj->v_pAlarm[iNext].v_aData.v_aAppItem))
		{
			g_hObj->v_pAlarm[i].v_nNext = g_hObj->v_pAlarm[iNext].v_nNext;
			return	nRet;
		}		
		i = iNext;
		nRet = g_hObj->v_pAlarm[i].v_nNext;
	}
	return	0;
}

/**************************************************************************
 * Function		: f_TimerCheckIn
 * Description	: add a new timer into pmalarm
 *				: u_INT1 nIndex
 * Return type	: 
 **************************************************************************/
static	void	f_TimerCheckIn(u_INT1 nIndex )
{
	u_INT1	i, iNext;

	if ( 0 == g_hObj->v_iTimer )
	{
		g_hObj->v_pTimer[nIndex].v_nNext = 0;
		g_hObj->v_iTimer = nIndex|IS_TIMER;		
		return;
	}
	i = (u_INT1)(g_hObj->v_iTimer&0xff);
	if (g_hObj->v_pTimer[nIndex].v_nTimeBase + 
		g_hObj->v_pTimer[nIndex].v_nTimeElapse < 
		g_hObj->v_pTimer[i].v_nTimeBase + g_hObj->v_pTimer[i].v_nTimeElapse)
	{
		g_hObj->v_pTimer[nIndex].v_nNext = g_hObj->v_iTimer;
		g_hObj->v_iTimer = nIndex|IS_TIMER;	
		return;
	}
	while ( 0 != g_hObj->v_pTimer[i].v_nNext )
	{
		iNext = (u_INT1)(g_hObj->v_pTimer[i].v_nNext&0xff);
		if (g_hObj->v_pTimer[nIndex].v_nTimeBase + 
			g_hObj->v_pTimer[nIndex].v_nTimeElapse < 
			g_hObj->v_pTimer[iNext].v_nTimeBase + 
			g_hObj->v_pTimer[iNext].v_nTimeElapse)
			break;
		
		i = iNext;
	}
	g_hObj->v_pTimer[nIndex].v_nNext = g_hObj->v_pTimer[i].v_nNext;
	g_hObj->v_pTimer[i].v_nNext = nIndex|IS_TIMER;
}

/**************************************************************************
 * Function		: f_TimerCheckOut
 * Description	: delete a timer from pmalarm
 * Argument		: pApp
 *				: u_INT2 nID
 * Return type	: 
 **************************************************************************/
static	u_INT2	f_TimerCheckOut(i_INT1* pApp, u_INT2 nID)
{
	u_INT2	nRet;
	u_INT1	i, iNext;	

	if (0 == g_hObj->v_iTimer)
		return	0;

	nRet = g_hObj->v_iTimer;
	i = (u_INT1)(nRet&0xff);

	if (nID == g_hObj->v_pTimer[i].v_nID && 
		0 == strcmp((const char*)pApp, 
		(const char*)g_hObj->v_pTimer[i].v_aAppExec))
	{
		g_hObj->v_iTimer = g_hObj->v_pTimer[i].v_nNext;
		return	nRet;
	}
	nRet = g_hObj->v_pTimer[i].v_nNext;
	while ( nRet > 0 )
	{
		iNext = (u_INT1)(nRet&0xff);

		if (nID == g_hObj->v_pTimer[iNext].v_nID && 
			0 == strcmp((const char*)pApp, 
			(const char*)g_hObj->v_pTimer[iNext].v_aAppExec))
		{
			g_hObj->v_pTimer[i].v_nNext = g_hObj->v_pTimer[iNext].v_nNext;
			return	nRet;
		}
		i = iNext;
		nRet = g_hObj->v_pTimer[i].v_nNext;
	}
	return	0;
}
/**************************************************************************
 * Function		: f_CompareTime
 * Description	: 
 * Argument		: PRTCTIME pSrc
 *				: PRTCTIME pDst
 * Return type	: 1 : src time less then dst time
				 -1 : src time exceed dst time
				  0 : the same time
					dst-time
	1			src		|
	-1					|	src
	0					src
 **************************************************************************/
static	i_INT1	f_CompareTime( PRTCTIME pSrc, PRTCTIME pDst )
{
	if ( pDst->v_nYear > pSrc->v_nYear )
		return	1;
	else if ( pDst->v_nYear < pSrc->v_nYear )
		return	-1;

	if ( pDst->v_nMonth > pSrc->v_nMonth )
		return	1;
	else if ( pDst->v_nMonth < pSrc->v_nMonth )
		return	-1;

	if ( pDst->v_nDay > pSrc->v_nDay )
		return	1;
	else if ( pDst->v_nDay < pSrc->v_nDay )
		return	-1;

	if ( pDst->v_nHour > pSrc->v_nHour )
		return	1;
	else if ( pDst->v_nHour < pSrc->v_nHour )
		return	-1;

	if ( pDst->v_nMinute > pSrc->v_nMinute )
		return	1;
	else if ( pDst->v_nMinute < pSrc->v_nMinute )
		return	-1;

	if ( pDst->v_nSecond > pSrc->v_nSecond )
		return	1;
	else if ( pDst->v_nSecond < pSrc->v_nSecond )
		return	-1;

	return	0;
}

/**************************************************************************
 * Function		: f_GetDayOfWeek
 * Description	: 
 *				: PRTCTIME pTime
 * Return type	: u_INT1
 **************************************************************************/
static u_INT1 f_GetDayOfWeek(PRTCTIME pTime)
{
	i_INT2	nYear;
    i_INT1	nMonth, nDay, nCentury, nDayOfWeek;

    nYear	= (i_INT2)pTime->v_nYear;
    nMonth	= (i_INT1)pTime->v_nMonth;
    nDay	= (i_INT1)pTime->v_nDay;

    if (nMonth < 3)
    {
        nMonth += 10;
        nYear--;
    }
    else
	{
        nMonth -= 2;
	}
    nCentury = (i_INT1)(nYear / 100);
    nYear = (i_INT1)(nYear % 100);

    nDayOfWeek = (((26 * nMonth - 2) / 10) + nDay + nYear + (nYear / 4) + 
        (nCentury / 4) - (2 * nCentury)) % 7;

    if (nDayOfWeek < 0)
       nDayOfWeek += 7;

    return	nDayOfWeek;
}

/**************************************************************************
* Function	f_CalcSeconds(PRTCTIME pTime)
* Purpose	count the interval from BASE_YEAR to defined time
			the unit is second
* Params	pTime		: absolute time
* Return	seconds
* Remarks	  
 **************************************************************************/
static u_INT4 f_CalcSeconds(PRTCTIME pTime)
{
    u_INT4	nTimeSpan;
    u_INT2	nYear;
	
    if(pTime->v_nYear < BASE_YEAR && pTime->v_nYear > MAX_YEAR)
		return	0;
    
    // Calculate elapsed days since base date (midnight, 1/1/1980)
    // 365 days for each elapsed year since 1980, plus one more day for
    // each elapsed leap year. no danger of overflow because of the range
    // check (above) on tmptm1.
    nTimeSpan = (pTime->v_nYear - BASE_YEAR) * 365L;
    for (nYear = BASE_YEAR; nYear < pTime->v_nYear; nYear++)
        if (f_IsLeapYear(nYear))
            nTimeSpan++;
	
    // Calculate days elapsed minus one, in the given year, to the given
    // month. Check for leap year and adjust if necessary.
    nTimeSpan += g_nMonthDays[pTime->v_nMonth - 1];
    if ( f_IsLeapYear(pTime->v_nYear) && (pTime->v_nMonth > 2) )
        nTimeSpan++;
	
    // elapsed days to current date
    nTimeSpan += pTime->v_nDay - 1;
    
    // Calculate elapsed hours since base date
    nTimeSpan = nTimeSpan * 24L + pTime->v_nHour;
    
    // Calculate elapsed minutes since base date
    nTimeSpan = nTimeSpan * 60L + pTime->v_nMinute;
    
    // Calculate elapsed seconds since base date
    nTimeSpan = nTimeSpan * 60L + pTime->v_nSecond;
	
    return	nTimeSpan;
}

/*
 *	get the date and time from calculate the seconds base on BASE_YEAR
 */
static	BOOL f_CalcTime(u_INT4 nSeconds,PRTCTIME pTime)
{
	int i;
	int iLeftSeconds;

	if ( nSeconds>(((u_INT4)(MAX_YEAR-BASE_YEAR))*365L*24L*3600L) )
		return FALSE;

	pTime->v_nYear	= BASE_YEAR;
	pTime->v_nMonth	= 1;
	pTime->v_nDay	= 1;
	pTime->v_nHour	= 0;
	pTime->v_nMinute= 0;
	pTime->v_nSecond= 0;
	pTime->v_nDayOfWeek = 0;

	iLeftSeconds = nSeconds;
	// calculate year
	for ( i=BASE_YEAR; i<=MAX_YEAR; i++ )
	{
		int tmp;
		if ( f_IsLeapYear(i) )
			tmp = 366*24*3600;
		else
			tmp = 365*24*3600;
		iLeftSeconds -= tmp;
		if ( iLeftSeconds<0 )
		{
			pTime->v_nYear = i;
			iLeftSeconds += tmp;
			break;
		}
	}
//	ASSERT(i<=MAX_YEAR);

	// calculate month
	if ( f_IsLeapYear(pTime->v_nYear) )
	{
		for ( i=0;i<12;i++ )
		{
			int tmp;

			if ( i==1 )	// Febrary
				tmp = g_nDayOfMonth[i]+1;
			else
				tmp = g_nDayOfMonth[i];

			iLeftSeconds -= tmp*24*3600;
			if ( iLeftSeconds<0 )
			{
				pTime->v_nMonth = i+1;
				iLeftSeconds += tmp*24*3600;
				break;
			}
		}
	}
	else
	{
		for ( i=0;i<12;i++ )
		{
			iLeftSeconds -= g_nDayOfMonth[i]*24*3600;
			if ( iLeftSeconds<0 )
			{
				pTime->v_nMonth = i+1;
				iLeftSeconds += g_nDayOfMonth[i]*24*3600;
				break;
			}
		}
	}
//	ASSERT(i<12);

	// calculate day of month
	i = 1;
	while ( 1 )
	{
		iLeftSeconds -= 24*3600;
		if ( iLeftSeconds<0 )
		{
			pTime->v_nDay = i;
			iLeftSeconds += 24*3600;
			break;
		}
		i++;
	}
//	ASSERT(i<32);

	// Hour
	i = 0;
	while (1)
	{
		iLeftSeconds -= 3600;
		if ( iLeftSeconds<0 )
		{
			pTime->v_nHour = i;
			iLeftSeconds += 3600;
			break;
		}
		i++;
	}
//	ASSERT(i<24);

	// Minute
	i = 0;
	while (1)
	{
		iLeftSeconds -= 60;
		if ( iLeftSeconds<0 )
		{
			pTime->v_nMinute = i;
			iLeftSeconds += 60;
			break;
		}
		i++;
	}
//	ASSERT(i<60);
//	ASSERT(iLeftSeconds<60);

	// Seconds
	pTime->v_nSecond = iLeftSeconds;

	return TRUE;
}
/**************************************************************************
 * Function		: f_IsLeapYear
 * Description	: 
 *				: u_INT2 nYear
 * Return type	: 
 **************************************************************************/
static	BOOL	f_IsLeapYear( u_INT2 nYear )
{
    return (nYear % 400 == 0 || (nYear % 4 == 0 && nYear % 100 != 0));
}

/**************************************************************************
 * Function		: f_IsTimeValid
 * Description	: 
 *				: PRTCTIME pTime
 * Return type	: 
 **************************************************************************/
static	BOOL	f_IsTimeValid(PRTCTIME pTime)
{
	if (NULL == pTime)
		return	FALSE;

	if (pTime->v_nHour >= 24 || pTime->v_nMinute >= 60 || 
		pTime->v_nSecond >= 60 || pTime->v_nMonth < 1 || 
		pTime->v_nMonth >12	|| pTime->v_nDay < 1 ||
		pTime->v_nYear < BASE_YEAR||pTime->v_nYear > MAX_YEAR)
        return	FALSE;
    
    if ( f_IsLeapYear(pTime->v_nYear) && (2 == pTime->v_nMonth) && 
		(pTime->v_nDay > 29) )
        return	FALSE;
    
    if (pTime->v_nDay > g_nDayOfMonth[pTime->v_nMonth-1])
		return	FALSE;

	return	TRUE;
}

/**************************************************************************
 * Function		: f_AddSpanToTime
 * Description	: Adds a specified time span to a specified time
 * Argument		: PRTCTIME pTime
 *				: unsigned long nSeconds
 * Return type	: static void 
 **************************************************************************/
static void f_AddSpanToTime( PRTCTIME pTime, u_INT4 nSeconds )
{
	i_INT4 nDays;
    i_INT1 nCarry, nMonthDays; 
	
    pTime->v_nSecond += (u_INT1)(nSeconds % 60);
    if (pTime->v_nSecond >= 60)
    {
        pTime->v_nSecond -= 60;
        nCarry = 1;
    }
	else
		nCarry = 0;

    nSeconds = nSeconds / 60;	
    pTime->v_nMinute += (u_INT2)(nSeconds % 60 + nCarry);
    if (pTime->v_nMinute >= 60)
    {
        pTime->v_nMinute -= 60;
        nCarry = 1;
    }
    else
        nCarry = 0;
    
	nSeconds = nSeconds / 60;	
    pTime->v_nHour += (u_INT2)(nSeconds % 24 + nCarry);
    if (pTime->v_nHour >= 24)
    {
        pTime->v_nHour -= 24;
        nCarry = 1;
    }
    else
        nCarry = 0;

    nSeconds = nSeconds / 24;	
    // Now nSeconds is days in origin nSeconds
    nDays = (i_INT4)(nSeconds + nCarry + pTime->v_nDay - 1);
    pTime->v_nDay = 1;
	
    if ( f_IsLeapYear(pTime->v_nYear) && pTime->v_nMonth == 2)
        nMonthDays = 29;
    else
        nMonthDays = g_nDayOfMonth[pTime->v_nMonth - 1];
	
    while (nDays >= nMonthDays)
    {
        pTime->v_nMonth++;
        if (pTime->v_nMonth > 12)
        {
            pTime->v_nMonth -= 12;
            pTime->v_nYear++;
        }
        nDays -= nMonthDays;
		
        if ( f_IsLeapYear(pTime->v_nYear) && pTime->v_nMonth == 2)
            nMonthDays = 29;
        else
            nMonthDays = g_nDayOfMonth[pTime->v_nMonth - 1];
    }	
    pTime->v_nDay += (u_INT1)nDays;
}

/*
Excute a app
*/
static void PM_ExcuApp( LPSTR pDstAppName, DWORD nCmd, WPARAM wParam, LPARAM lParam)
{
	APPENTRY	hApp;

	if (NULL == g_hObj || NULL == pDstAppName)
		return;

	hApp.hApp = NULL;
	hApp.pAppName = pDstAppName;
	hApp.nCode = nCmd;
	hApp.wParam = wParam;
	hApp.lParam = lParam;

	printf("\r\nNotify Application %s , wParma is %d\r\n",pDstAppName,wParam);
	CallAppFile(GetAppMain(),&hApp);
}
/**************************************************************************
 * Function		: f_RTCSetAlarm
 * Description	: 
 *				: void
 * Return type	: 
 **************************************************************************/
static	void	f_RTCSetAlarm(void)
{
	PRTCTIME	pTime = NULL;
	RTCTIME		aMaxTime, aTime, aMinTime;
	u_INT4		nSecs;
	u_INT1		i;

	if ( 0 != g_hObj->v_iTimer )
	{
		RTC_GetTime(&aTime);
		nSecs = f_CalcSeconds ( &aTime );

		i = (u_INT1)(g_hObj->v_iTimer&0xff);
		f_AddSpanToTime ( &aTime, g_hObj->v_pTimer[i].v_nTimeBase- nSecs 
			+ g_hObj->v_pTimer[i].v_nTimeElapse );

		pTime = &aTime;
		if ( 0 == g_hObj->v_iAlarm )
			goto RTC_SETALARM;

		i = (u_INT1)(g_hObj->v_iAlarm&0xff);	
		if (f_CompareTime(pTime, &g_hObj->v_pAlarm[i].v_aData.v_aTime) < 0 )
			pTime = &g_hObj->v_pAlarm[i].v_aData.v_aTime;

		goto RTC_SETALARM;
	}
	else if ( 0 != g_hObj->v_iAlarm )
	{
		i = (u_INT1)(g_hObj->v_iAlarm&0xff);	// get the first alarm index	
		pTime = &g_hObj->v_pAlarm[i].v_aData.v_aTime;	// first alarm item
	}	
RTC_SETALARM:
	DEBUG_LINE(pTime)
	if (NULL == pTime)	// no alarm item
	{
#if USE_DUMMY
		RTCSetAlarm(0, NULL, NULL, 0);	// cancel me alarm
#else
		RTCSetAlarm(0, 0, NULL, NULL);
#endif		
		return;
	}
	RTC_GetTime(&aMaxTime);		// get current time
	memcpy(&aMinTime, &aMaxTime, sizeof(RTCTIME));

	if (PM_IsPowerOff())
	{
		DEBUG_LINE(TIME_DELAY)
		f_AddSpanToTime(&aMinTime, TIME_DELAY);	// wait for finishing power off process
	}
	else
	{
		DEBUG_LINE(TIME_PRECISION)
		f_AddSpanToTime(&aMinTime, TIME_PRECISION);	// wait for sending AT command to MC55
	}

/* MC55 support infinite time scope , so this codes are useless
	f_AddSpanToTime(&aMaxTime, 3*24 * 60 * 60 - TIME_PRECISION); 
    if (f_CompareTime(pTime, &aMaxTime) < 0)		// 设置的时间大于设备可以接受的最长时间
        pTime = &aMaxTime;    
	else if (f_CompareTime(pTime, &aMinTime) > 0)	// 增加了 TIME_PRECISION 后，有可能使得设置的时间小于可以设置的合理的时间
		pTime = &aMinTime;	
*/
	// 使用 MC55 ，只需要考虑一种情况
	if (f_CompareTime(pTime, &aMinTime) > 0)
		pTime = &aMinTime;

	DEBUG_LINE(pTime->v_nHour)
	DEBUG_LINE(pTime->v_nMinute)
	DEBUG_LINE(pTime->v_nSecond)

	// set next alarm time
#if USE_DUMMY
	RTCSetAlarm(3, pTime, g_hObj->v_hWnd, WM_ALARM_SET);
#else
	nSecs = MAKETIME(pTime->v_nHour, pTime->v_nMinute, pTime->v_nSecond);
	RTCSetAlarm(3, nSecs, f_RTCCallBack, NULL);
#endif    
}

/**************************************************************************
 * Function		: f_RTCCallBack
 * Description	: 
 * Argument		: * nParam
 *				: int nEvent
 * Return type	: 
 **************************************************************************/
static	void	f_RTCCallBack(void * nParam, int nEvent)
{
	if (NULL != g_hObj)
		if (IsWindow(g_hObj->v_hWnd))
			PostMessage(g_hObj->v_hWnd, WM_TIMER, 0, 0);
}

/**************************************************************************
 * Function		: f_RTCChangeTime
 * Description	: 
 *				: PRTCTIME pPrevTime
 * Return type	: void 
 **************************************************************************/
static	void	f_RTCChangeTime(PRTCTIME pPrevTime)
{
	RTCTIME	aTime, *pTime;
	i_INT4	nPrevSeconds, nCurrSeconds, nTmp4;
	u_INT2	nID, iAlarm;
	u_INT1	i, bFile;

	if (NULL == g_hObj)
		return;

	// we do nothing when in alarm mode and charge only mode
	if ( IsAlarmPowerOn() )
	{
		printf("\r\npmalarm: time is changed but we will not re-arrange alarm chain, because we are in alarm mode.\r\n");
		return;
	}

	iAlarm = g_hObj->v_iAlarm;	// save header of alarm chain
	g_hObj->v_iAlarm = 0;		// set alarm chain header as NULL, this mean there is no alarm item in the chain

	nPrevSeconds = f_CalcSeconds(pPrevTime );
	RTC_GetTime(&aTime);
	nCurrSeconds = f_CalcSeconds(&aTime );

	while(0 != iAlarm)
	{
		// deal with each alarm item of the alarm chain
		i = (u_INT1)(iAlarm&0xff);	
		
		if (g_hObj->v_pAlarm[i].v_aData.v_nAttrib & HAS_INFILE)
		{
			f_FileCheckOut(i);
			bFile = 1;
		}
		else
		{
			bFile = 0;
		}
		
		//	delete
		nID = iAlarm;
		iAlarm = g_hObj->v_pAlarm[i].v_nNext;	// record next alarm item
		
		pTime = &g_hObj->v_pAlarm[i].v_aData.v_aTime;

		if (g_hObj->v_pAlarm[i].v_aData.v_nAttrib & HAS_RELATIVE)
		{
			nTmp4 = f_CalcSeconds(pTime );//the time to ring

			memcpy(pTime, &aTime, sizeof(RTCTIME));	
			//the time to ring = current time + interval time
			f_AddSpanToTime(pTime, nTmp4-nPrevSeconds);
			f_AlarmCheckIn(i);
			if (0 != bFile)
				f_FileCheckIn(i);
			continue;
		}
		
		if ( 0 < f_CompareTime(&aTime,pTime) )
		{
			// alarm is not time out , just add it again
			if ( g_hObj->v_pAlarm[i].v_aData.v_nAttrib & HAS_AUTONEXTDAY )
			{
				// we should assure auto next day alarm should not exceed current date 1 day 
				u_INT4	nAlarmSec;
				int		iAlarmCurr;

				// set to the same date
				pTime->v_nYear	= aTime.v_nYear;
				pTime->v_nMonth	= aTime.v_nMonth;
				pTime->v_nDay	= aTime.v_nDay;

				nAlarmSec = f_CalcSeconds(pTime);			// get seconds of alarm
				iAlarmCurr = (nAlarmSec-nCurrSeconds);		// seconds between alarm time and current time

				if ( iAlarmCurr<TIME_DELAY )
				{
					f_AddSpanToTime(pTime, 86400);
				}
				else
				{
					;// do nothing when alarm time advance current time
				}
			}
			f_AlarmCheckIn(i);
			if ( 0!=bFile )
				f_FileCheckIn(i);
			continue;
		}
		else
		{
			// alarm is time out due to system time change ...
			if ( g_hObj->v_pAlarm[i].v_aData.v_nAttrib & HAS_AUTONEXTDAY )
			{
				u_INT4	nAlarmSec,nAddSec,nCurrAlarm;

				nAlarmSec = f_CalcSeconds(pTime);			// get seconds of alarm
				nCurrAlarm = (nCurrSeconds-nAlarmSec);		// seconds between current time and alarm time

				if ( nCurrAlarm<=(86400-TIME_DELAY) )
				{
					nAddSec = 86400;
				}
				else
				{
					nAddSec = (nCurrAlarm+TIME_DELAY) + 86400 - ((nCurrAlarm+TIME_DELAY)%86400);
				}

				printf("\r\npmalarm: time is changed, we add span to autonextday item.\r\n");
				f_AddSpanToTime(pTime, nAddSec);	//modify to the second day
				f_AlarmCheckIn(i);
				if (0 != bFile)
					f_FileCheckIn(i);
				continue;
			}
		}

		// now, an alarm item is deleted due to system time changing, we should notify application
		{
			//	attemper
			PM_ExcuApp((LPSTR)g_hObj->v_pAlarm[i].v_aData.v_aAppExec, 
				APP_ALARM, ALMN_TIMECHANGED, 
				g_hObj->v_pAlarm[i].v_aData.v_nID);
			//	reclaim
			// put the reclaimed item at the head of the available chain
			g_hObj->v_pAlarm[i].v_nNext = g_hObj->v_iAlarmRoom;
			g_hObj->v_pAlarm[i].v_aData.v_nAttrib = 0;
			g_hObj->v_iAlarmRoom = nID;
			g_hObj->v_nAlarmRoom++;	
		}		
	}
	
	nID = g_hObj->v_iTimer;
	while (0 != nID)
	{	
		i = (u_INT1)(nID&0xff);
		g_hObj->v_pTimer[i].v_nTimeBase +=  nCurrSeconds-nPrevSeconds;
		//base time = current time-elapse time(nPrevSeconds-g_hObj->v_pTimer[i].v_nTimeBase)
		nID = g_hObj->v_pTimer[i].v_nNext;
	}

	// application will not re-set a new alarm, we have to set the new alarm time to device
	printf("\r\npmalarm: time has be changed, so we should reset alarm to MC55.\r\n");
	f_RTCSetAlarm();
}

/**************************************************************************
 * Function		: f_WndProc
 * Description	: 
 * Argument		: hWnd
 *				: UINT wMsgCmd
 *				: WPARAM wParam
 *				: LPARAM lParam
 * Return type	: CALLBACK 
 **************************************************************************/
static	LRESULT	CALLBACK 
f_WndProc(HWND hWnd, UINT uMsgCmd, WPARAM wParam, LPARAM lParam)
{
	static	char	aAppExec[MAX_CHAR+1];
	u_INT2	nError;
	u_INT2	nPowerOnItem;
	u_INT2	nID;
	i_INT1	i;
	i_INT1	iFirst;

	switch(uMsgCmd) {
	case WM_ALARM_ON:
		printf("\r\nReceive MC55 +CALA URC...\r\n");
		return RTC_Check();

	// WM_ALARM_POWERON and WM_ALARM_ME_TIME are useful only when alarm or charger power on
	case WM_ALARM_POWERON:
		printf("\r\npmalarm begin get time from MC55...\r\n");
		if ( -1==ME_GetClockTime ( hWnd, WM_ALARM_ME_TIME ) )
		{
			// Get time from Me error, just power on as normal
			printf("\r\nAlarm call ME_GetClockTime error.\r\n");
			MobileInit2();
		}
		break;
	case WM_ALARM_ME_TIME:
		switch(wParam)
		{
		case ME_RS_SUCCESS:
			{
				SYSTEMTIME  aTime;
				RTCTIME		rtctime;
				RTCTIME		alarmTime;
				char		aStr[32] = "\0";	
				int			nYear = 0;

				printf("\r\npmalarm get time from MC55 successfully.\r\n");
				printf("\r\nwe will check the reason of power on.\r\n");
				// Get time from Me
				if ( ME_GetResult ( aStr, ME_SMS_TIME_LEN ) < 0 )
				{
					printf("\r\nPMAlarm Get Me time result error.\r\n");
					return	FALSE;
				}
				printf("\r\nPMAlarm Get time is %s\r\n",aStr);

			    aStr[2]	= 0;
				nYear = atoi(aStr);

				memset(&aTime,0,sizeof(SYSTEMTIME));
				if(nYear > 50)
					aTime.wYear	= (WORD)(atoi( aStr ) + 1900);
				else
					aTime.wYear	= (WORD)(atoi( aStr ) + 2000);
    
				aStr[5]	= 0;
				aTime.wMonth= (WORD)atoi( &aStr[3] );
				aStr[8]	= 0;
				aTime.wDay	= (WORD)atoi( &aStr[6] );
				aStr[11]= 0;
				aTime.wHour	= (WORD)atoi( &aStr[9] );
				aStr[14]= 0;
				aTime.wMinute = (WORD)atoi( &aStr[12] );
				aStr[17]= 0;
				aTime.wSecond = (WORD)atoi( &aStr[15] );
    
				printf("\r\npmalarm set local time without change alarm chain.\r\n");
				DHI_SetRTC_( &aTime );

				rtctime.v_nYear		= (u_INT2)aTime.wYear;
				rtctime.v_nMonth	= (u_INT1)aTime.wMonth;
				rtctime.v_nDay		= (u_INT1)aTime.wDay;
				rtctime.v_nHour		= (u_INT1)aTime.wHour;
				rtctime.v_nMinute	= (u_INT1)aTime.wMinute;
				rtctime.v_nSecond	= (u_INT1)aTime.wSecond;

				// there are alarms in the chain, get the first item
				nPowerOnItem = g_hObj->v_iAlarm;
				if ( 0==nPowerOnItem )
				{
					printf("\r\nNo alarm item, there should be some problems.\r\n");
					MobileInit2();//ChargeOnlyState();
					break;
				}

				iFirst = nPowerOnItem&0xff;

				memcpy(&alarmTime,&g_hObj->v_pAlarm[iFirst].v_aData.v_aTime,sizeof(RTCTIME));

				printf("\r\nthe first alarm belong to %s it's time is:\r\n",g_hObj->v_pAlarm[iFirst].v_aData.v_aAppExec);
				printf("Year:%4d,Month:%2d,Day:%2d\r\n",alarmTime.v_nYear,alarmTime.v_nMonth,alarmTime.v_nDay);
				printf("Hour:%4d,Minut:%2d,Sec:%2d\r\n",alarmTime.v_nHour,alarmTime.v_nMinute,alarmTime.v_nSecond);
/*
				if ( 0 < f_CompareTime(&rtctime,&alarmTime) )
				{
					// the first alarm is not time out , maybe there are some error or charging power on
					// now , we deal with it as charging power on
					printf("\r\nno alarm is time out, enter charge only mode.\r\n");
					ChargeOnlyState();
					break;
				}
				f_AddSpanToTime(&alarmTime,600);
				if ( 0 < f_CompareTime(&alarmTime,&rtctime) )
				{
					// the first alarm is early 30 before current time , we deal with it as charging power on
					printf("\r\nalarm is time out more then 30 seconds, enter charge only mode.\r\n");
					ChargeOnlyState();
					break;
				}
*/
				// now we think it's alarm power on
				// save alarm item information
				strncpy(aAppExec,(char*)g_hObj->v_pAlarm[iFirst].v_aData.v_aAppExec,MAX_CHAR);

				if ( 0==strcmp(aAppExec,AUTO_POWER_ON) )
				{
					// automatically opower on
					printf("\r\nautomatically power on...\r\n");
					MobileInit2();
				}
				else
					// we should check if the first is not auto power on
					RTC_Check();

				// auto power off time will be set at APP_INIT by application
				//printf("\r\npmalarm: get automatically power off time at alarm mode.\r\n");
				//RTC_CheckPowerOff(&rtctime);
			}
			break;
		default:
			// Get time from Me error, just power on as normal
			printf("\r\nAlarm get time from Me error.\r\n");
			MobileInit2();
			break;
		}

		break;

	case WM_ALARM_SET:
		if ( wParam==0 )
		{
			printf("\r\nSet alarm successfully.\r\n");

			if ( 0!=g_hObj->v_iAlarm )
			{
				if ( 0==g_hObj->v_iAlarm )
					break;

				i = g_hObj->v_iAlarm&0xff;
				// save alarm item information
				strncpy(aAppExec,g_hObj->v_pAlarm[i].v_aData.v_aAppExec,MAX_CHAR);
				nID = g_hObj->v_pAlarm[i].v_aData.v_nID;

				// notify success to application
				PM_ExcuApp((LPSTR)aAppExec,APP_ALARM,ALMN_SETOK,nID);
			}
		}
		else
		{
			printf("\r\nSet alarm error.\r\n");

			// we receive this means the first alarm item is not been set to MC55, we should delete it and notify application
			if ( 0!=g_hObj->v_iAlarm )
			{
				// there are alarms in the chain, get the first item
				nError = g_hObj->v_iAlarm;
				i = nError&0xff;

				// save alarm item information
				strncpy(aAppExec,g_hObj->v_pAlarm[i].v_aData.v_aAppExec,MAX_CHAR);
				nID = g_hObj->v_pAlarm[i].v_aData.v_nID;

				// delete the first alarm when set alarm error
				g_hObj->v_iAlarm = g_hObj->v_pAlarm[i].v_nNext;		// point to next alarm item
				f_FileCheckOut(i);
				//	reclaim
				g_hObj->v_pAlarm[i].v_nNext = g_hObj->v_iAlarmRoom;
				g_hObj->v_pAlarm[i].v_aData.v_nAttrib = 0;
				g_hObj->v_iAlarmRoom = nError;
				g_hObj->v_nAlarmRoom++;

				if ( 0!= g_hObj->v_iAlarm)		// set next alarm item if the chain is not empty
				{
					printf("\r\npmalarm: alarm set error, the wrong item has been deleted, we should reset alarm to MC55.\r\n");
					f_RTCSetAlarm();
				}

				// notify error to application
				PM_ExcuApp((LPSTR)aAppExec,APP_ALARM,ALMN_SETERROR,nID);
			}
		}
		break;
	case WM_ALARM_TIME_CHANGE:
		// send message to all registered application
		{
			int i;

			for ( i=0; i<MAX_APP; i++ )
			{
				if ( NULL != g_hObj->v_pRegisteredApp[i].v_AppItem[0] )
				{
					PM_ExcuApp((LPSTR)g_hObj->v_pRegisteredApp[i].v_AppItem,
						APP_ALARM,ALMN_SYSTIME_CHANGE,NULL);
				}
			}
		}
		break;
	default:
		return	DefWindowProc( hWnd, uMsgCmd, wParam, lParam );
	}
	
	return	DefWindowProc( hWnd, uMsgCmd, wParam, lParam );
}

/**************************************************************************
 * Function		: RTC_Create
 * Description	: 
 *				: void
 * Return type	: 
 **************************************************************************/
int	RTC_Create(void)
{
	if (NULL != g_hObj)
		return	1;
	if (f_Create())
		return	1;

	f_Release();
	return	0;
}

/**************************************************************************
 * Function		: RTC_Release
 * Description	: 
 *				: void
 * Return type	: 
 **************************************************************************/
int	RTC_Release(void)
{
	if (NULL == g_hObj)
		return	0;

	f_Release();

	return	1;
}

/**************************************************************************
 * Function		: RTC_Check
 * Description	: 
 *				: void
 * Return type	: int 
 **************************************************************************/
int RTC_Check(void)
{
	RTCTIME	aTime;	
	PRTCTIME pTime;
	BOOL	bFile;
	u_INT4	nSeconds;
	u_INT2	iTimeOut;
	u_INT1	i, nCount = 0;

	if (NULL == g_hObj)
		if (0 == RTC_Create())
			return	-1;

	DEBUG_LINE(g_hObj)
	if (PM_IsPowerOff())
	{
		printf("\r\npmalarm: in the process of power off, we should reset alarm to MC55.\r\n");
		f_RTCSetAlarm();	// on power off ...
		return	-1;
	}
	
	while(0 != g_hObj->v_iAlarm)
	{
		// There are alarm item in the alarm chain
		RTC_GetTime(&aTime);
		i = (u_INT1)(g_hObj->v_iAlarm&0xff);

		// Do nothing when the first alarm item aTime<AlarmTime 
		if (f_CompareTime(&aTime, &g_hObj->v_pAlarm[i].v_aData.v_aTime) > 0)
			break;

		// time is out
		printf("\r\nalarm item %s is time out.\r\n",g_hObj->v_pAlarm[i].v_aData.v_aAppExec);
		pTime = &g_hObj->v_pAlarm[i].v_aData.v_aTime;

		if (g_hObj->v_pAlarm[i].v_aData.v_nAttrib & HAS_INFILE)
		{
			f_FileCheckOut(i);
			bFile = 1;
		}
		else
		{
			bFile = 0;
		}

		// delete
		iTimeOut = g_hObj->v_iAlarm;
		g_hObj->v_iAlarm = g_hObj->v_pAlarm[i].v_nNext;	// point to the next alarm item

		if ( g_hObj->v_pAlarm[i].v_aData.v_nAttrib&HAS_AUTONEXTDAY )
		{
			if ( 0==strcmp(g_hObj->v_pAlarm[i].v_aData.v_aAppExec,AUTO_POWER_OFF) )
			{
				printf("\r\n alarm notify power off!");
				DlmNotify(PMM_NEWS_ENABLE, PMF_AUTO_SHUT_DOWN);
				DlmNotify(PMM_NEWS_SHUT_DOWN,(LPARAM)0);
			}
			else if ( 0==strcmp(g_hObj->v_pAlarm[i].v_aData.v_aAppExec,AUTO_POWER_ON) )
			{
				// do nothing when the phone has been power on, just modify to the next day
				printf("\r\nAutomatically Power on alarm is comming...\r\n");
			}
			else
			{
				// notify application
				PM_ExcuApp((LPSTR)g_hObj->v_pAlarm[i].v_aData.v_aAppExec,
				APP_ALARM, ALMN_TIMEOUT, g_hObj->v_pAlarm[i].v_aData.v_nID);
			}
			// do not delete it , move to next day
			pTime->v_nYear	= aTime.v_nYear;
			pTime->v_nMonth	= aTime.v_nMonth;
			pTime->v_nDay	= aTime.v_nDay;
			f_AddSpanToTime(pTime, 86400);	//modify to the next day
			f_AlarmCheckIn(i);
			if (0 != bFile)
				f_FileCheckIn(i);
			
			if ( 0==strcmp(g_hObj->v_pAlarm[i].v_aData.v_aAppExec,AUTO_POWER_ON) )
			{
				// start the phone when in alarm or charge only mode and auto power on is come
				if ( IsAlarmPowerOn() )
					MobileInit2();
			}
		}
		else
		{
			// notify application
			PM_ExcuApp((LPSTR)g_hObj->v_pAlarm[i].v_aData.v_aAppExec,
				APP_ALARM, ALMN_TIMEOUT, g_hObj->v_pAlarm[i].v_aData.v_nID);
			//	reclaim
			g_hObj->v_pAlarm[i].v_nNext = g_hObj->v_iAlarmRoom;
			g_hObj->v_pAlarm[i].v_aData.v_nAttrib = 0;
			g_hObj->v_iAlarmRoom = iTimeOut;
			g_hObj->v_nAlarmRoom++;
			nCount++;
		}
	}
	while (0 != g_hObj->v_iTimer )
	{
		// There are timer item in timer chain
		RTC_GetTime(&aTime);
		nSeconds = f_CalcSeconds( &aTime );
		i = (u_INT1)(g_hObj->v_iTimer&0xff);
		
		if (nSeconds < g_hObj->v_pTimer[i].v_nTimeBase +
			g_hObj->v_pTimer[i].v_nTimeElapse)
			break;

		//	delete
		g_hObj->v_iTimer = g_hObj->v_pTimer[i].v_nNext;
		//	add newly
		g_hObj->v_pTimer[i].v_nTimeBase = nSeconds;
		f_TimerCheckIn(i);
		//	attemper
		PM_ExcuApp((LPSTR)g_hObj->v_pTimer[i].v_aAppExec, 
			APP_TIMER, ALMN_TIMEOUT, g_hObj->v_pTimer[i].v_nID);		
		
		nCount++;
	}

	printf("\r\npmalarm: RTC_Check has been called, we should reset alarm to MC55.\r\n");
	f_RTCSetAlarm();

	return	nCount;
}

/**************************************************************************
 * Function		: RTC_GetTime
 * Description	: 
 *				: PRTCTIME pTime
 * Return type	: int 
 **************************************************************************/
int RTC_GetTime(PRTCTIME pTime)
{
	struct rtc_dtime aTime;

    if (NULL == pTime)
        return	0;
    
    RTCGetDayTime(&aTime);
    
    pTime->v_nYear		= GETYEAR(aTime.date);
    pTime->v_nMonth		= GETMONTH(aTime.date);
    pTime->v_nDay		= GETDAY(aTime.date);
    pTime->v_nHour		= GETHOUR(aTime.time);
    pTime->v_nMinute	= GETMINUTE(aTime.time);
    pTime->v_nSecond	= GETSECOND(aTime.time);
	pTime->v_nDayOfWeek = f_GetDayOfWeek(pTime);

    return	1;
}

/**************************************************************************
 * Function		: RTC_SetTime
 * Description	: 
 *				: PRTCTIME pTime
 * Return type	: int 
 **************************************************************************/
int RTC_SetTime(PRTCTIME pTime)
{
	RTCTIME aPrevTime;
    struct	rtc_dtime aTime;

	if ( !g_hObj )	// 闹钟管理模块初始化之前有可能设置时间，此时返回 0
		return 0;

    if (!f_IsTimeValid(pTime))
        return	0;

    RTC_GetTime(&aPrevTime);

    // call the bottom? interface to modify time
    aTime.date = MAKEDATE(pTime->v_nYear, pTime->v_nMonth, pTime->v_nDay);
    aTime.time = MAKETIME(pTime->v_nHour, pTime->v_nMinute, pTime->v_nSecond);
    if (0 == RTCSetDayTime(&aTime))
	{
		// notify RTC Alarm manage module that the time changed
        f_RTCChangeTime(&aPrevTime);
//		PM_AppNotify(APP_CHANGETIME, 0, 0);
		//DlmNotify(PM_TIMECHANGED, TRUE);
		DlmNotify(1, TRUE);
		PostMessage(g_hObj->v_hWnd,WM_ALARM_TIME_CHANGE,NULL,NULL);
		return	1;
	}	
    return	0;
}

/**************************************************************************
 * Function		: RTC_SetAlarms
 * Description	: 
 * Argument		: *pAppSrc
 *				: u_INT2 nID
 *				: u_INT1 nflags
 *				: i_INT1 *pAppDst
 *				: PRTCTIME pTime
 * Return type	: int 
 **************************************************************************/
int RTC_SetAlarms(i_INT1 *pAppSrc, u_INT2 nID, u_INT1 nflags, 
				  i_INT1 *pAppDst, PRTCTIME pTime)
{
	RTCTIME	aTime;
	u_INT2	nGet, iAlarm;
	u_INT1	i;

	if (NULL == pAppDst || NULL == pAppSrc || NULL == pTime || NULL == g_hObj)
		return	0;

	if (!f_IsTimeValid(pTime))
		return	0;

	iAlarm = g_hObj->v_iAlarm;
	nGet = f_AlarmCheckOut(pAppSrc, nID);	// delete nID from alarm chain	

	if (0 != nGet)
	{
		// alarm item is already in alarm chain
		i = (u_INT1)(nGet&0xff);
		if (!(nflags & ALMF_REPLACE))
		{
			// can not delete alarm item, add it again
			f_AlarmCheckIn(i);			// should not replace, insert again
			return	0;
		}
		f_FileCheckOut(i);
	}
	else
	{
		// a new alarm item
		if (0 == g_hObj->v_nAlarmRoom)	// no available alarm space
			return	0;

		nGet = g_hObj->v_iAlarmRoom;	// get a available item
		i = (u_INT1)(nGet&0xff);
		g_hObj->v_iAlarmRoom = g_hObj->v_pAlarm[i].v_nNext;
		g_hObj->v_nAlarmRoom--;
	}

	RTC_GetTime(&aTime);

	if (f_CompareTime(&aTime, pTime) <= 0)	// if time of alarm item less then current time
	{
		// set time early then current time
		if (nflags & ALMF_AUTONEXTDAY)		// alarm repeat every day.
		{
			f_AddSpanToTime(pTime, 86400);	//modify to the second day
		}
		else
		{
			if (nGet == iAlarm)				// first item is changed, set next alarm to MC55
			{
				printf("\r\npmalarm: in RTC_Alarms, we call f_RTCSetAlarm.\r\n");
				f_RTCSetAlarm();
			}

			// notify application right now
			PM_ExcuApp((LPSTR)pAppDst, APP_ALARM, ALMN_TIMEOUT, nID);
			// reclaim, add i to the head of available chain
			g_hObj->v_pAlarm[i].v_nNext = g_hObj->v_iAlarmRoom;
			g_hObj->v_pAlarm[i].v_aData.v_nAttrib = 0;
			g_hObj->v_iAlarmRoom = nGet;
			g_hObj->v_nAlarmRoom++;	
			return	1;
		}		
	}

	strncpy((char*)g_hObj->v_pAlarm[i].v_aData.v_aAppExec, 
		(char*)pAppDst, MAX_CHAR);
	strncpy((char*)g_hObj->v_pAlarm[i].v_aData.v_aAppItem, 
		(char*)pAppSrc, MAX_CHAR);
	g_hObj->v_pAlarm[i].v_aData.v_aAppItem[MAX_CHAR] = 0;
	g_hObj->v_pAlarm[i].v_aData.v_aAppExec[MAX_CHAR] = 0;
	memcpy(&g_hObj->v_pAlarm[i].v_aData.v_aTime, pTime, sizeof(RTCTIME));
	
	g_hObj->v_pAlarm[i].v_aData.v_nAttrib = 0;	// first set alarm item, clear attribute in advance
	if (nflags & ALMF_RELATIVE)
		g_hObj->v_pAlarm[i].v_aData.v_nAttrib |= HAS_RELATIVE;	// relative alarm for snooze
	if ( nflags & ALMF_AUTONEXTDAY )
		g_hObj->v_pAlarm[i].v_aData.v_nAttrib |= HAS_AUTONEXTDAY;	// autonextday alarm

	g_hObj->v_pAlarm[i].v_aData.v_nID	= nID;	// do not change application specified id
	
	f_AlarmCheckIn(i);							// add new alarm into alarm chain
	if (nflags & ALMF_POWEROFF)
		f_FileCheckIn(i);	
	if (nGet == g_hObj->v_iAlarm)				// first alarm has been changed, reset alarm again
	{
		printf("\r\npmalarm: in RTC_SetAlarms, we finally set alarm to MC55.\r\n");
		f_RTCSetAlarm();
	}
	if (g_hObj->v_nAlarmRubbish >= MAX_RUBBISH)
		f_FileClean();

	return	1;
}

/**************************************************************************
 * Function		: RTC_KillAlarms
 * Description	: 
 * Argument		: *pApp
 *				: u_INT2 nID
 * Return type	: int 
 **************************************************************************/
int RTC_KillAlarms(i_INT1 *pApp, u_INT2 nID)
{
	u_INT2	nGet, iAlarm;
	u_INT1	i;

	if (NULL == pApp || NULL == g_hObj)
		return	0;

	iAlarm = g_hObj->v_iAlarm;
	nGet = f_AlarmCheckOut(pApp, nID);
	if (0 == nGet)
		return	0;

	i = (u_INT1)(nGet & 0xff);
	f_FileCheckOut(i);
	//	reclaim
	g_hObj->v_pAlarm[i].v_nNext = g_hObj->v_iAlarmRoom;
	g_hObj->v_pAlarm[i].v_aData.v_nAttrib = 0;
	g_hObj->v_iAlarmRoom = nGet;
	g_hObj->v_nAlarmRoom++;
	//	reset
	if (iAlarm == nGet)	// if remove the first alarm item, reset all alarm again
	{
		printf("\r\npmalarm: first alarm item has been killed by user, reset alarm to MC55.\r\n");
		f_RTCSetAlarm();
	}

	// add code for delete alarm when there is no any item
	{
	}

	return	1;
}

/**************************************************************************
 * Function		: RTC_SetTimer
 * Description	: 
 * Argument		: *pApp
 *				: u_INT2 nID
 *				: u_INT4 nElapse
 * Return type	: int 
 **************************************************************************/
int RTC_SetTimer(i_INT1 *pApp, u_INT2 nID, u_INT4 nElapse)
{
	RTCTIME	aTime;
	u_INT2	nGet;
	u_INT1	i;

	if (NULL == pApp || NULL == g_hObj || nElapse < TIME_PRECISION )
		return	0;
	if (0 == g_hObj->v_iTimerRoom)
		return	0;

	nGet = f_TimerCheckOut(pApp, nID);
	if (0 != nGet)
	{
		i = (u_INT1)(nGet & 0xff);
		f_TimerCheckIn(i);
		return	0;
	}
	nGet = g_hObj->v_iTimerRoom;
	i = (u_INT1)(nGet & 0xff);
	g_hObj->v_iTimerRoom = g_hObj->v_pTimer[i].v_nNext;
	g_hObj->v_nTimerRoom--;

	strncpy((char*)g_hObj->v_pTimer[i].v_aAppExec, (char*)pApp, MAX_CHAR);
	g_hObj->v_pTimer[i].v_aAppExec[MAX_CHAR] = 0;
	g_hObj->v_pTimer[i].v_nID = nID;

	RTC_GetTime(&aTime);
	g_hObj->v_pTimer[i].v_nTimeBase = f_CalcSeconds(&aTime);
	g_hObj->v_pTimer[i].v_nTimeElapse = nElapse;

	f_TimerCheckIn(i);
	if (nGet == g_hObj->v_iTimer)
		f_RTCSetAlarm();

	return	1;
}

/**************************************************************************
 * Function		: RTC_KillTimer
 * Description	: 
 * Argument		: *pApp
 *				: unsigned short nID
 * Return type	: int 
 **************************************************************************/
int RTC_KillTimer(i_INT1 *pApp, u_INT2 nID)
{
	u_INT2	nGet, iTimer;
	u_INT1	i;

	if (NULL == pApp || NULL == g_hObj)
		return	0;

	iTimer = g_hObj->v_iTimer;
	nGet = f_TimerCheckOut(pApp, nID);
	if (0 == nGet)
		return	0;

	i = (u_INT1)(nGet & 0xff);
	//	reclaim
	g_hObj->v_pTimer[i].v_nNext = g_hObj->v_iTimerRoom;
	g_hObj->v_iTimerRoom = nGet;
	g_hObj->v_nTimerRoom++;
	//	reset
	if (iTimer == nGet)
		f_RTCSetAlarm();

	return	1;
}

#if (DEVF_HAS_RTC < 1)
/**************************************************************************
 * Function		: RTCGetDayTime
 * Description	: 
 *				: struct rtc_dtime * daytime
 * Return type	: 
 **************************************************************************/
int		RTCGetDayTime(struct rtc_dtime * daytime)
{
	SYSTEMTIME	atm;

	DHI_GetRTC_(&atm);
	
	daytime->date = (atm.wYear << 9)+(atm.wMonth << 5)+atm.wDay;
    daytime->time = (atm.wHour << 20)+(atm.wMinute << 14)+(atm.wSecond << 8);

	return	0;
}

/**************************************************************************
 * Function		: RTCSetDayTime
 * Description	: 
 *				: struct rtc_dtime * daytime
 * Return type	: 
 **************************************************************************/
int		RTCSetDayTime(struct rtc_dtime * daytime)
{
	SYSTEMTIME	atm;

	atm.wHour	= (WORD)(daytime->time >> 20);
    atm.wMinute	= (WORD)(daytime->time >> 14) & 0x3F;
    atm.wSecond = (WORD)(daytime->time >> 8) & 0x3F;
    if (!(atm.wHour >= 0 && atm.wHour < 24 && atm.wMinute < 60 && 
		atm.wSecond < 60))
        return	-1;

	atm.wYear	= (WORD)(daytime->date >> 9);
    atm.wMonth	= (WORD)(daytime->date >> 5) & 0x0F;
    atm.wDay	= (WORD)daytime->date & 0x1F;
    if ( !(atm.wYear >= BASE_YEAR && atm.wYear <= MAX_YEAR) ||
         !(atm.wMonth >= 1 && atm.wMonth <= 12) )
        return	-1;

	atm.wDayOfWeek		= 0;
	atm.wMilliseconds	= 500;

	if (DHI_SetRTC_(&atm))
		return	0;

	return	-1;
}

BOOL    DHI_KillAlarm(void);
/**************************************************************************
 * Function		: RTCSetAlarm
 * Description	: 
 * Argument		: enable
 *				: PRTCTIME pTime
 *				: HWND hWnd
 *				: UINT uMsgCmd
 * Return type	: 
 **************************************************************************/
int		RTCSetAlarm(int enable, PRTCTIME pTime, HWND hWnd, UINT uMsgCmd)
{
	ME_ALARMTIME	atm;
	ALARMTIME		alarmtime;

	if ( 0==enable )	// cancel alarm
	{
		if ( ALARM_ME==g_hObj->v_nAlarmSource )
		{
			memset(&atm, 0, sizeof(ME_ALARMTIME));

			atm.year	= (unsigned char)(2099%100);
			atm.month	= (unsigned char)1;
			atm.day		= (unsigned char)1;
			atm.hour	= (unsigned char)0;
			atm.minute	= (unsigned char)0;
			atm.second	= (unsigned char)0;

			if ( ME_SetAlarm(hWnd, uMsgCmd, &atm, 0)<0 )
			{
				printf("\r\nMe_SetAlarm return error!\r\n");
				return -1;
			}
		}
		if ( ALARM_CPU==g_hObj->v_nAlarmSource )
		{
			DHI_KillAlarm();
		}

		return 0;
	}

	if (NULL == pTime || NULL == hWnd)
		return	-1;

	if ( ALARM_ME==g_hObj->v_nAlarmSource )
	{
		memset(&atm, 0, sizeof(ME_ALARMTIME));

		atm.day	= pTime->v_nDay;
		atm.hour	= pTime->v_nHour;
		atm.minute	= pTime->v_nMinute;
		atm.month	= pTime->v_nMonth;
		atm.second	= pTime->v_nSecond;
		atm.year	= (unsigned char)(pTime->v_nYear%100);

		if ( ME_SetAlarm(hWnd, uMsgCmd, &atm, 0)<0 )
		{
			// notify application
			printf("\r\nMe_SetAlarm return error!\r\n");
			PostMessage(g_hObj->v_hWnd,WM_ALARM_SET,-1,0);
			return -1;
		}

		return 0;
	}

	if ( ALARM_CPU==g_hObj->v_nAlarmSource )
	{
		memset(&alarmtime, 0, sizeof(ALARMTIME));

		alarmtime.wYear		= (WORD)pTime->v_nYear;
		alarmtime.wMonth	= (WORD)pTime->v_nMonth;
		alarmtime.wDay		= (WORD)pTime->v_nDay;
		alarmtime.wHour		= (WORD)pTime->v_nHour;
		alarmtime.wMinute	= (WORD)pTime->v_nMinute;
		alarmtime.wSecond	= (WORD)pTime->v_nSecond;
		
		if ( TRUE!=DHI_SetAlarm(hWnd,WM_ALARM_ON,&alarmtime) )
		{
			// notify application
			printf("\r\nDHI_SetAlarm return error!\r\n");
			PostMessage(g_hObj->v_hWnd,WM_ALARM_SET,-1,0);
			return -1;
		}

		return 0;
	}

	return -1;
}
#endif

/**************************************************************************
 * Function		: RTC_GetFirstAppAlarm
 * Description	: 
 * Argument		: pAppDst	: Destinate Application name
 *				: pTime		: Point to RTCTIME to receive time
 *				: nID		: Point to a unsigned short variable to receive alarm id
 * Return type	: 0			: Find an alarm item
 *				: -1		: Error parameters
 *				: -2		: PMAlarm is not created
 *				: -5		: Does not find an alarm item of an application
 **************************************************************************/
int	RTC_GetFirstAppAlarm(signed char *pAppDst,PRTCTIME pTime,unsigned short* nID)
{
	signed char		i;
	unsigned short	v_iAlarm;

	if ( NULL==pAppDst || NULL==pTime || NULL==nID )
		return -1;

	if (NULL == g_hObj)
		return -2;

	if (0 != g_hObj->v_iAlarm)
	{
		// there are alarm items in the chain

		v_iAlarm = g_hObj->v_iAlarm;
		while ( 0!=v_iAlarm )
		{
			i = v_iAlarm&0xff;
			if ( 0 == strcmp((const char*)pAppDst, (const char*)g_hObj->v_pAlarm[i].v_aData.v_aAppExec) )
			{
				memcpy(pTime,&g_hObj->v_pAlarm[i].v_aData.v_aTime,sizeof(RTCTIME));
				*nID = g_hObj->v_pAlarm[i].v_aData.v_nID;
				return 0;
			}
			v_iAlarm = g_hObj->v_pAlarm[i].v_nNext;
		}		
	}
	
	return -5;
}

/*
 *	Called at start up when alarm power on in progman.c
 */
int RTC_AlarmPowerOn(void)
{
	if ( !g_hObj )
	{
		printf("\r\nAlarm power on before pmalarm is created!!!\r\n");
		return -1;
	}

	if ( 0==g_hObj->v_iAlarm )
	{
		printf("\r\nNo alarm item in the chain, please check it!!!\r\n");
		return -2;
	}

	// post message to pmalarm window proc, pmalarm will notify application in this message
	PostMessage(g_hObj->v_hWnd,WM_ALARM_POWERON,NULL,NULL);
	return 0;
}

/*
 *	application register for receive message when time change
 */
BOOL RTC_RegisterApp(u_INT1* AppName)
{
	int i;

	if ( !g_hObj )
	{
		printf("\r\nPMAlarm: register app before create pmalarm module!\r\n");
		return FALSE;
	}
	
	// same application can not register more time
	for ( i=0;i<MAX_APP;i++ )
	{
		if ( 0 == strcmp(g_hObj->v_pRegisteredApp[i].v_AppItem,AppName) )
		{
			printf("\r\nPMAlarm: application %s has registered.\r\n",AppName);
			return FALSE;
		}
	}

	for ( i=0;i<MAX_APP;i++ )
	{
		if ( NULL == g_hObj->v_pRegisteredApp[i].v_AppItem[0] )
			break;
	}

	if ( i>=MAX_APP )
	{
		printf("\r\nPMAlarm: no space for new registeration.\r\n");
		return FALSE;
	}

	strncpy(g_hObj->v_pRegisteredApp[i].v_AppItem,AppName,MAX_CHAR);
	g_hObj->v_pRegisteredApp[i].v_AppItem[MAX_CHAR] = '\0';

	return TRUE;
}

/*
 *	unregister function
 */
BOOL RTC_UnRegisterApp(u_INT1* AppName)
{
	int i;

	if ( !g_hObj )
	{
		printf("\r\nPMAlarm: unregister app before create pmalarm module!\r\n");
		return FALSE;
	}

	for ( i=0;i<MAX_APP;i++ )
	{
		if ( 0==strcmp(g_hObj->v_pRegisteredApp[i].v_AppItem,AppName) )
		{
			// find it
			g_hObj->v_pRegisteredApp[i].v_AppItem[0] = '\0';
			return TRUE;
		}
	}

	printf("\r\nPMAlarm: application haven't registered.\r\n");
	return FALSE;
}
/*
void TestDateTime(void)
{
	int	nYear,nMonth,nDay,nHour,nMinute,nSecond;
	RTCTIME	TestRtc,CalcRtc;
	u_INT4	nSecondsOfDateTime;

	for ( nYear=BASE_YEAR;nYear<=MAX_YEAR;nYear++ )
	{
		BOOL bLeapYear;

		if ( f_IsLeapYear(nYear) )
			bLeapYear =TRUE;
		else
			bLeapYear = FALSE;

		TestRtc.v_nYear = nYear;

		for ( nMonth=0;nMonth<12;nMonth++ )
		{
			int iDaysOfMonth;

			if ( bLeapYear )
			{
				if ( nMonth==1 )
					iDaysOfMonth = g_nDayOfMonth[nMonth]+1;
				else
					iDaysOfMonth = g_nDayOfMonth[nMonth];
			}
			else
			{
				iDaysOfMonth = g_nDayOfMonth[nMonth];
			}

			TestRtc.v_nMonth = nMonth+1;

			for ( nDay=0;nDay<iDaysOfMonth;nDay++ )
			{
				TestRtc.v_nDay = nDay+1;

				for ( nHour=0;nHour<24;nHour++ )
				{
					TestRtc.v_nHour = nHour;

					for ( nMinute=0;nMinute<60;nMinute++ )
					{
						TestRtc.v_nMinute = nMinute;

						for ( nSecond=0;nSecond<60;nSecond++ )
						{
							TestRtc.v_nSecond = nSecond;

							nSecondsOfDateTime = f_CalcSeconds(&TestRtc);
							f_CalcTime(nSecondsOfDateTime,&CalcRtc);
							if ( f_CompareTime(&TestRtc,&CalcRtc)!=0 )
							{
								printf("\r\nCalculate Date:Time error!\r\n");
								printf("Original:%4d-%2d-%2d : %2d-%2d-%2d\r\n",TestRtc.v_nYear,TestRtc.v_nMonth,TestRtc.v_nDay,
									TestRtc.v_nHour,TestRtc.v_nMinute,TestRtc.v_nSecond);
								printf("Calculat:%4d-%2d-%2d : %2d-%2d-%2d\r\n",CalcRtc.v_nYear,CalcRtc.v_nMonth,CalcRtc.v_nDay,
									CalcRtc.v_nHour,CalcRtc.v_nMinute,CalcRtc.v_nSecond);
								return;
							}
						}
					}
				}
			}

		}
	}
	printf("\r\nCalculate date:time OK!\r\n");
}

void RTC_CheckPowerOff(PRTCTIME pCurrentTime)
{
	BOOL	bTimeIsOk = FALSE;
	int		nHour,nMinute;
	RTCTIME	rtctime;

	// get time of automatically power off
	if ( Sett_GetAutoCloseInfo(&nHour,&nMinute) )
	{
		printf("\r\nget automatically power off time from settings.\r\n");
		memcpy(&rtctime,pCurrentTime,sizeof(RTCTIME));
		// user has set automatically power off, we get the time and set to pmalarm
		if ( (u_INT1)nHour < rtctime.v_nHour )
		{
			rtctime.v_nHour		= (u_INT1)nHour;
			rtctime.v_nMinute	= (u_INT1)nMinute;
			f_AddSpanToTime(&rtctime, 86400);	//modify to the next day
			bTimeIsOk=TRUE;
		}
		else if ( (u_INT1)nHour == rtctime.v_nHour )
		{
			if ( (u_INT1)nMinute < rtctime.v_nMinute )
			{
				rtctime.v_nMinute = (u_INT1)nMinute;
				f_AddSpanToTime(&rtctime, 86400);	//modify to the next day
				bTimeIsOk=TRUE;
			}
		}
		
		if ( !bTimeIsOk )
		{
			// auto power off time is not time out
			rtctime.v_nHour		= (u_INT1)nHour;
			rtctime.v_nMinute	= (u_INT1)nMinute;
			rtctime.v_nSecond	= (u_INT1)0;
			f_AddSpanToTime(&rtctime, TIME_DELAY);
		}

		// set alarm to support automatically power off
		printf("\r\npmalarm: we set automatically power off alarm to MC55 now.\r\n");
		RTC_SetAlarms(AUTO_POWER_OFF,ALM_ID_AUTO_POWER_OFF,
			ALMF_REPLACE|ALMF_AUTONEXTDAY,AUTO_POWER_OFF,&rtctime);
	}
}
*/
