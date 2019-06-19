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


#ifndef RTC_TOOLS_H
#define RTC_TOOLS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hmrtc.h"

void RTC_IncDay(RTCTIME *pRtctime);   // add one day to specified time by pRtctime
void RTC_IncMonth(RTCTIME *pRtctime); // add one month to specified time by pRtctime
void RTC_IncYear(RTCTIME *pRtctime);  // add one year to specified time by pRtctime

int RTC_DaysOfYear(int nYear);        // get days of specific year
int RTC_DaysOfMonth(int nYear, int nMonth); // get days of specified year and month

int RTC_IsLeapYear(int year);         // whether the year is leap 
int RTC_GetWkday(int nYear, int nMonth, int nDay);  // get week day of the day
int RTC_CompareTime(const RTCTIME* pTime1, const RTCTIME* pTime2); // compare time 
int RTC_CompareDate(const RTCTIME* pDate1, const RTCTIME* pDate2); // compare only date
void RTC_AddMinutes(RTCTIME *pRtcTime, int nPreMin);               // add minutes to specified time
void RTC_AddSeconds(PRTCTIME pTime, unsigned long dwSeconds);      // add seconds to specified time


#ifdef __cplusplus
}
#endif

#endif  // RTC_TOOLS_H
