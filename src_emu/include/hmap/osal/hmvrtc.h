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

#ifndef _HMVRTC_H_
#define _HMVRTC_H_

#ifdef __cplusplus
extern "C" {
#endif

struct rtc_dtime {
    unsigned long date;
    unsigned long time;
};

#define TIME_ELAPSED     0
#define TIME_CHANGED     1

long RTCGetTime (void);
long RTCGetDate (void);
int  RTCGetDayTime (struct rtc_dtime * daytime);
long RTCSetTime (unsigned long time);
long RTCSetDate (unsigned long date);
int  RTCSetDayTime (struct rtc_dtime * daytime);
int  RTCSetAlarm (int enable, struct rtc_dtime * daytime, void (*func)(void * param, int event), void *param);
int  RTCGetAlarm (struct rtc_dtime * daytime);

#ifdef __cplusplus
}
#endif

#endif // _HMVRTC_H_

