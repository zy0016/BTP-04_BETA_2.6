 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : alarm manager $ version 3.0
 *
 * Purpose  : Implements  alarm for hopen PDA .
 *            
\**************************************************************************/

#ifndef	HAVE_PMALARM_H
#define	HAVE_PMALARM_H

#include "window.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef	signed		char	i_INT1;
typedef	signed		short	i_INT2;
typedef	signed		long	i_INT4;
typedef	unsigned	char	u_INT1;
typedef	unsigned	short	u_INT2;
typedef	unsigned	long	u_INT4;
typedef	int					HFILE;

//	TYPE DEFINE HERE
typedef	struct	rtctime_s 
{
    u_INT2	v_nYear;	
	u_INT1	v_nMonth;
	u_INT1	v_nDay;
	u_INT1	v_nHour;
	u_INT1	v_nMinute;
	u_INT1	v_nSecond;
	u_INT1	v_nDayOfWeek;

}	RTCTIME, *PRTCTIME;
//	MACRO DEFINE HERE
#define ALMN_TIMEOUT		0x01
#define ALMN_TIMECHANGED	0x02
#define ALMN_SETERROR		0x04
#define ALMN_SETOK			0x08
#define ALMN_SYSTIME_CHANGE	0x10

#define	ALMF_REPLACE		0x01
#define	ALMF_AUTONEXTDAY	0x02
#define	ALMF_POWEROFF		0x04
#define	ALMF_RELATIVE		0x08

#define ALMN_TIME_OUT		ALMN_TIMEOUT
#define ALMN_TIME_CHANGE	ALMN_TIMECHANGED

#define ALMN_AUTOSWITCHON_CHANGE  0x0003

#define ALM_ID_AUTO_POWER_ON	0
#define ALM_ID_AUTO_POWER_OFF	1

#define PMS_FILE_NAME	"/mnt/flash/pmalarm.ini"
#define AUTO_POWER_ON	"AutoPowerOn"
#define AUTO_POWER_OFF	"AutoPowerOff"

/*copy from old*/
//#define ALMF_REPLACE			0x0001
//#define ALMF_ENABLE_POWOFF		0x0002
//#define ALMF_RELATIVE_TIME		0x0004
//#define ALMF_SNOOZE             0x0010
//
//#define ALMN_TIME_OUT			0x0000
//#define ALMN_TIME_CHANGE		0x0001
//#define ALMN_AUTOSWITCHON_CHANGE  0x0002
// end

//	OUTPUT FUNCTION DECLARE
int	RTC_Create(void);
int	RTC_Release(void);
int RTC_Check(void);
int RTC_GetTime(PRTCTIME pTime);
int RTC_SetTime(PRTCTIME pTime);
int RTC_SetAlarms(i_INT1 *pAppSrc, u_INT2 nID, u_INT1 nflags, 
				  i_INT1 *pAppDst, PRTCTIME pTime);
int RTC_KillAlarms(i_INT1 *pApp, u_INT2 nID);
int RTC_SetTimer (i_INT1 *pApp, u_INT2 nID, u_INT4 nElapse);
int RTC_KillTimer(i_INT1 *pApp, u_INT2 nID);

int	RTC_GetFirstAppAlarm(signed char *pAppDst,PRTCTIME pTime,unsigned short* nID);
int RTC_AlarmPowerOn(void);
BOOL RTC_RegisterApp(u_INT1* AppName);
BOOL RTC_UnRegisterApp(u_INT1* AppName);
void pmalarm_debug(void);

#if (DEVF_HAS_RTC < 1)
//	TYPE DEFINE HERE
struct	rtc_dtime 
{
    unsigned long date;
    unsigned long time;
};

int	RTCGetDayTime(struct rtc_dtime * daytime);
int	RTCSetDayTime(struct rtc_dtime * daytime);
int	RTCSetAlarm(int enable, PRTCTIME pTime, HWND hWnd, UINT uMsgCmd);
#endif

//#define DEBUG_LINE(x)	{\
//	char	abuf[512];\
//	sprintf(abuf, "\r\nFILE:%s, LINE:%ld, X:0x%lx", __FILE__, __LINE__, x);\
//	printf(abuf);\
//}
//#define	DEBUG_TEXT(x)	PlxTrace(x);
#define DEBUG_LINE(x)
#define	DEBUG_TEXT(x)
#else
#define DEBUG_LINE(x)
#define	DEBUG_TEXT(x)

#ifdef __cplusplus
}
#endif

#endif //HAVE_PMRTC_H
