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

#ifndef __HMRTC_H
#define __HMRTC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagRTCTIME 
{
    unsigned short  year;
    unsigned char   month;
    unsigned char   day;
    unsigned char   hour;
    unsigned char   minute;
    unsigned char   second;
    unsigned char   unused;
} RTCTIME, *PRTCTIME;

void*   RTC_Open(int mode);
int     RTC_Close(void* pRTC);

#define ALARM_TIMEELAPSED       0
#define ALARM_TIMECHANGED       1

int     RTC_SetAlarm(void* pRTC, int id, const RTCTIME* pRTCTime, 
                     int bPowerOff, const char* pProg, const char* arg);
int     RTC_FindAlarm(void* pRTC, const char* pProg, int index);
void    RTC_KillAlarm(void* pRTC, const char* pProg, int id);

void    RTC_SetNotify(void* pRTC, void* hWnd, unsigned int msg);
int     RTC_SetTimer (void* pRTC, int id, unsigned long seconds);
int     RTC_KillTimer (void* pRTC, int id);

int     RTC_Check(void);

int     RTC_GetTime(RTCTIME* pRTCTime);
int     RTC_SetTime(const RTCTIME* pRTCTime);

void    RTC_ChangeTime(const RTCTIME* pOldTime);

//***  add by yangyoubing for testing  **************************//
RTCTIME *   RTC_CurAlarmTime(PRTCTIME pTime);  // get current alarm time 
int RTC_AlarmTime(PRTCTIME pTime, const char *pProg, int id ); // get specific alarm time

#ifdef __cplusplus
}
#endif

#endif  // __HMRTC_H
