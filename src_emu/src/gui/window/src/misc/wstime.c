  /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Functions about time.
 *            
\**************************************************************************/



#include "hpwin.h"
#include "osal.h"
#include <project\pmi.h>

#include "string.h"

#ifndef NOSYSTEMTIME

#if defined(_EMULATE_) && ((_HOPEN_VER >= 300) || defined(_LINUX_OS_))
#include "time.h"

static long lOffset = 0;
static long lBias = 0;
#else   // _HOPEN_VER < 300
static long lBias = -480;
#endif  // _HOPEN_VER >= 300 || _LINUX_OS_

// record the number of day of every months
static int nDayOfMonth[] = 
{
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

// functions used for converting time
static void ConvertTimeByPositiveBias(const SYSTEMTIME* pTime1, 
                                      PSYSTEMTIME pTime2, long lBias);
static void ConvertTimeByNegativeBias(const SYSTEMTIME* pTime1, 
                                      PSYSTEMTIME pTime2, long lBias);
static BOOL IsLeapYear(WORD wYear);
static int CompareTime(const SYSTEMTIME* pTime1, const SYSTEMTIME* pTime2);
static BOOL IsTimeValid(const SYSTEMTIME* pSystemTime);

#ifdef _EMULATE_


#if defined(_HOPEN_VER) && (_HOPEN_VER < 300)
void _EMU_GetLocalTime(PSYSTEMTIME pSystemTime);
#endif  // _HOPEN_VER < 300

#else

#if (RTC)

#define GetRTC Alarm_GetRTC
#define SetRTC Alarm_SetRTC

void GetRTC(PSYSTEMTIME pSystemTime);
void SetRTC(const SYSTEMTIME* pSystemTime);

#else   // RTC


#endif  // RTC

#endif  // _EMULATE_
// 用于保存最后一次调用SetLocalTime设置的时间和当时的时钟中断计数
static SYSTEMTIME SettingTime = 
{
    2000, 1, 6, 1, 0, 0, 0, 0   // 2000/1/1 00:00:00:0000
};
static DWORD dwSettingTick;

static int GetDayOfWeek(const SYSTEMTIME* pSystemTime);
static void AddSpanToTime(PSYSTEMTIME pSystemTime, DWORD dwMilliseconds);


/*
**  Function : GetSystemTime
**  Purpose  :
**      The GetSystemTime function retrieves the current system date and time.
**      The system time is expressed in Coordinated Universal Time (UTC). 
**  Params   :
**      pSystemTime : Points to a SYSTEMTIME structure to receive the current
**                    system date and time
**  Return   :
**      This function does not return a value. 
*/
void WINAPI GetSystemTime(PSYSTEMTIME pSystemTime)
{
    SYSTEMTIME localTime;

    if (!pSystemTime)
    {
        SetLastError(1);
        return;
    }

    // get the local time from the system
    GetLocalTime(&localTime);
    
    // convert the local time to the system time
    if (lBias > 0)
        ConvertTimeByPositiveBias(&localTime, pSystemTime, lBias);
    else if (lBias < 0)
        ConvertTimeByNegativeBias(&localTime, pSystemTime, -lBias);
    else
    {
        pSystemTime->wYear = localTime.wYear;
        pSystemTime->wMonth = localTime.wMonth;
        pSystemTime->wDayOfWeek = localTime.wDayOfWeek;
        pSystemTime->wDay = localTime.wDay;
        pSystemTime->wHour = localTime.wHour;
        pSystemTime->wMinute = localTime.wMinute;
        pSystemTime->wSecond = localTime.wSecond;
        pSystemTime->wMilliseconds = localTime.wMilliseconds;
    }
}

/*
**  Function : SetSystemTime
**  Purpose  :
**      The SetSystemTime function sets the current system time and date. 
**      The system time is expressed in Coordinated Universal Time (UTC). 
**  Params   :
**      pSystemTime   : Points to a SYSTEMTIME structure that contains the 
**                      current system date and time. 
**                      The wDayOfWeek member of the SYSTEMTIME structure is ignored. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. 
**      To get extended error information, call GetLastError. 
*/
BOOL WINAPI SetSystemTime(const SYSTEMTIME *pSystemTime)
{
    SYSTEMTIME localTime;

    if (!pSystemTime)
    {
        SetLastError(1);
        return FALSE;
    }

    // convert UTC to the local time and set the local time
    if (lBias > 0)
    {
        ConvertTimeByNegativeBias(pSystemTime, &localTime, lBias);
        SetLocalTime(&localTime);
    }
    else if (lBias < 0)
    {
        ConvertTimeByPositiveBias(pSystemTime, &localTime, -lBias);
        SetLocalTime(&localTime);
    }
    else
        SetLocalTime(pSystemTime);

    return TRUE;
}

/*
**  Function : GetLocalTime
**  Purpose  :
**      The GetLocalTime function retrieves the current local date and time. 
**  Params   :
**      pSystemTime   : Points to a SYSTEMTIME structure to receive the current 
**                      local date and time
**  Return   :
**      This function does not return a value. 
*/
void WINAPI GetLocalTime(PSYSTEMTIME pSystemTime)
{
#if defined(_EMULATE_) && ((_HOPEN_VER >= 300) || defined(_LINUX_OS_))
//    time_t tNow;
//    struct tm *tmNow;
#endif  // _HOPEN_VER >= 300 || _LINUX_OS_

    if (!pSystemTime)
    {
        SetLastError(1);
		//printf("============GetLocalTime() error!!");
        return;
    }

    // get the local time from the system, add this function at here.

#if 0// _EMULATE_

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    tNow = time(NULL);
    if (tNow == (time_t)-1)
    {
        SetLastError(1);
        return;
    }
    tNow += lOffset;

    tmNow = localtime(&tNow);
    if (tmNow == NULL)
    {
        SetLastError(1);
        return;
    }

    pSystemTime->wYear          = tmNow->tm_year + 1900;
    pSystemTime->wMonth         = tmNow->tm_mon + 1;
    pSystemTime->wDayOfWeek     = tmNow->tm_wday;
    pSystemTime->wDay           = tmNow->tm_mday;
    pSystemTime->wHour          = tmNow->tm_hour;// - 1;
    pSystemTime->wMinute        = tmNow->tm_min;
    pSystemTime->wSecond        = tmNow->tm_sec;
    pSystemTime->wMilliseconds  = 0;

#else   // _HOPEN_VER < 300

    _EMU_GetLocalTime(pSystemTime);

#endif  // _HOPEN_VER >= 300 || _LINUX_OS_

#else   // _EMULATE_


	DHI_GetRTC(pSystemTime); //DHI_GetRTC implementation in progman/PMI.c ---changed by axli
	pSystemTime->wDayOfWeek = GetDayOfWeek(pSystemTime);

//#if (RTC)
//
//    GetRTC(pSystemTime);
//    pSystemTime->wDayOfWeek = GetDayOfWeek(pSystemTime);
//
//#else
//
//    {
//        DWORD dwMilliseconds;
//
//        // 计算从上一次设置系统时间到现在的好秒数
//        dwMilliseconds = PLXOS_GetSysClock() - dwSettingTick;
//        dwMilliseconds = 1000 * dwMilliseconds / PLXOS_GetTicksPerSecond();
//        
//        *pSystemTime = SettingTime;
//        AddSpanToTime(pSystemTime, dwMilliseconds);
//    }

// #endif  // RTC

#endif  // _EMULATE_
}

/*
**  Function : SetLocalTime
**  Purpose  :
**      The SetLocalTime function sets the current local time and date. 
**  Params   :
**      pSystemTime   : Points to a SYSTEMTIME structure that contains 
**                      the current local date and time. 
**                      The wDayOfWeek member of the SYSTEMTIME structure
**                      is ignored. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended error 
**      information, call GetLastError
*/
BOOL WINAPI SetLocalTime(const SYSTEMTIME* pSystemTime)
{
/*#if defined(_EMULATE_) && ((_HOPEN_VER >= 300) || defined(_LINUX_OS_))
    time_t tNow;
    time_t tSet;
    struct tm tmSet;
#endif  // _HOPEN_VER >= 300 || _LINUX_OS_*/
    SYSTEMTIME NewSystemTime;

    if (!pSystemTime)
    {
        SetLastError(1);
        return FALSE;
    }

    if (!IsTimeValid(pSystemTime))
    {
        SetLastError(1);
        return FALSE;
    }

//#ifndef _EMULATE_
#if 1

/*
#if (RTC)

    SetRTC((PSYSTEMTIME)pSystemTime);

#else   // RTC

#else   // RTC
    // 没有实时钟时，记下当前设置的时间和时钟中断计数
    SettingTime = *pSystemTime;
    dwSettingTick = PLXOS_GetSysClock();

    // Ignore the wDayOfWeek member of the SYSTEMTIME structure, calculate
    // it using the date
    SettingTime.wDayOfWeek = GetDayOfWeek(&SettingTime);



#endif  // RTC
*/
    memcpy(&NewSystemTime, pSystemTime, sizeof(SYSTEMTIME));
    NewSystemTime.wDayOfWeek = GetDayOfWeek(pSystemTime);

    DHI_SetRTC((PSYSTEMTIME)&NewSystemTime);    //DHI_SetRTC implementation in progman/PMI.c----changed by axli
	

#else   // _EMULATE_

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    if (pSystemTime->wYear < 1900)
    {
        SetLastError(1);
        return FALSE;
    }

    tNow = time(NULL);
    if (tNow == (time_t)-1)
    {
        SetLastError(1);
        return FALSE;
    }

    tmSet.tm_year   = pSystemTime->wYear - 1900;
    tmSet.tm_mon    = pSystemTime->wMonth - 1;
    tmSet.tm_wday   = pSystemTime->wDayOfWeek;
    tmSet.tm_mday   = pSystemTime->wDay;
    tmSet.tm_hour   = pSystemTime->wHour;
    tmSet.tm_min    = pSystemTime->wMinute;
    tmSet.tm_sec    = pSystemTime->wSecond;
    tmSet.tm_isdst  = 0;

    tSet = mktime(&tmSet);
    if (tSet == (time_t)-1)
    {
        SetLastError(1);
        return FALSE;
    }

    lOffset = tSet - tNow;

#else   // _HOPEN_VER < 300
#endif  // _HOPEN_VER >= 300 || _LINUX_OS_

#endif  // _EMULATE_
    
    PostMessage(HWND_BROADCAST, WM_TIMECHANGE, 0, 0);

    return TRUE;
}

#ifndef NOTIMEZONE

static TIME_ZONE_INFORMATION TimeZoneInfo =
{
    -480,
    "",
    {0,0,0,0,0,0,0,0},
    0,
    "",
    {0,0,0,0,0,0,0,0},
    0
};

/*
**  Function : SystemTimeToTzSpecificLocalTime
**  Purpose  :
**      TThe SystemTimeToTzSpecificLocalTime function converts a Coordinated
**      Universal Time (UTC) to a specified time zone’s corresponding local time. 
**  Params   :
**      pTimeZoneInfo   : Pointer to a TIME_ZONE_INFORMATION structure 
**                               that specifies the time zone of interest. 
**                               If lpTimeZoneInfo is NULL, the function uses
**                               the currently active time zone. 
**      pUniversalTime  :   Pointer to a SYSTEMTIME structure that specifies a UTC.
**                          The function converts this universal time to the 
**                          specified time zone’s corresponding local time. 
**      pLocalTime      :   Pointer to a SYSTEMTIME structure that receives the
**                          local time information. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended error
**      information, call GetLastError
*/
BOOL WINAPI SystemTimeToTzSpecificLocalTime(
                    PTIME_ZONE_INFORMATION pTimeZoneInfo,
                    PSYSTEMTIME pUniversalTime,
                    PSYSTEMTIME pLocalTime)
{
    long lBias;

    if (!pUniversalTime || !pLocalTime)
    {
        SetLastError(1);
        return FALSE;
    }

    if (!pTimeZoneInfo)
        lBias = TimeZoneInfo.Bias;
    else
        lBias = pTimeZoneInfo->Bias;

    if (lBias > 0)
        ConvertTimeByNegativeBias(pUniversalTime, pLocalTime, lBias);
    else if (lBias < 0)
        ConvertTimeByPositiveBias(pUniversalTime, pLocalTime, -lBias);
    else
    {
        pLocalTime->wYear = pUniversalTime->wYear;
        pLocalTime->wMonth = pUniversalTime->wMonth;
        pLocalTime->wDayOfWeek = pUniversalTime->wDayOfWeek;
        pLocalTime->wDay = pUniversalTime->wDay;
        pLocalTime->wHour = pUniversalTime->wHour;
        pLocalTime->wMinute = pUniversalTime->wMinute;
        pLocalTime->wSecond = pUniversalTime->wSecond;
        pLocalTime->wMilliseconds = pUniversalTime->wMilliseconds;
    }

    return TRUE;
}

/*
**  Function : GetTimeZoneInformation
**  Purpose  :
**      The GetTimeZoneInformation function retrieves the current time-zone parameters.
**      These parameters control the translations between Coordinated Universal 
**      Time (UTC) and local time
**  Params   :
**      pTimeZoneInfo   : 
**                      Points to a TIME_ZONE_INFORMATION structure to receive the
**                      current time-zone parameters. 
**  Return   :
**      If the function succeeds, the return value is one of the following values: 
**      TIME_ZONE_ID_UNKNOWN    :The operating system cannot determine the current
**                               time zone. This is usually because a previous call
**                               to the SetTimeZoneInformation function supplied only
**                               the bias (and no transition dates). 
**      TIME_ZONE_ID_STANDARD   :The operating system is operating in the range
**                               covered by the StandardDate member of the structure
**                               pointed to by the lpTimeZoneInfo parameter
**      TIME_ZONE_ID_DAYLIGHT   :The operating system is operating in the range covered
**                               by the DaylightDate member of the structure pointed
**                               to by the lpTimeZoneInfo parameter
**
**      If the function fails, the return value is 0xFFFFFFFF. To get extended
**      error information, call GetLastError
*/
DWORD WINAPI GetTimeZoneInformation(PTIME_ZONE_INFORMATION pTimeZoneInfo)
{
    if (!pTimeZoneInfo)
    {
        SetLastError(1);
        return (DWORD)-1;
    }

    pTimeZoneInfo->Bias = TimeZoneInfo.Bias;
    strcpy(pTimeZoneInfo->StandardName, TimeZoneInfo.StandardName);

    pTimeZoneInfo->StandardDate.wYear = TimeZoneInfo.StandardDate.wYear;
    pTimeZoneInfo->StandardDate.wMonth = TimeZoneInfo.StandardDate.wMonth;
    pTimeZoneInfo->StandardDate.wDayOfWeek = TimeZoneInfo.StandardDate.wDayOfWeek;
    pTimeZoneInfo->StandardDate.wDay = TimeZoneInfo.StandardDate.wDay;
    pTimeZoneInfo->StandardDate.wHour = TimeZoneInfo.StandardDate.wHour;
    pTimeZoneInfo->StandardDate.wMinute = TimeZoneInfo.StandardDate.wMinute;
    pTimeZoneInfo->StandardDate.wSecond =TimeZoneInfo.StandardDate.wSecond;
    pTimeZoneInfo->StandardDate.wMilliseconds = TimeZoneInfo.StandardDate.wMilliseconds;

    pTimeZoneInfo->StandardBias = TimeZoneInfo.StandardBias;
    strcpy(pTimeZoneInfo->DaylightName, TimeZoneInfo.DaylightName);

    pTimeZoneInfo->DaylightDate.wYear = TimeZoneInfo.DaylightDate.wYear;
    pTimeZoneInfo->DaylightDate.wMonth = TimeZoneInfo.DaylightDate.wMonth;
    pTimeZoneInfo->DaylightDate.wDayOfWeek = TimeZoneInfo.DaylightDate.wDayOfWeek;
    pTimeZoneInfo->DaylightDate.wDay = TimeZoneInfo.DaylightDate.wDay;
    pTimeZoneInfo->DaylightDate.wHour = TimeZoneInfo.DaylightDate.wHour;
    pTimeZoneInfo->DaylightDate.wMinute = TimeZoneInfo.DaylightDate.wMinute;
    pTimeZoneInfo->DaylightDate.wSecond = TimeZoneInfo.DaylightDate.wSecond;
    pTimeZoneInfo->DaylightDate.wMilliseconds = TimeZoneInfo.DaylightDate.wMilliseconds;

    pTimeZoneInfo->DaylightBias = TimeZoneInfo.DaylightBias;

    /*
    if (TimeZoneInfo.StandardDate.wMonth != 0 &&
        TimeZoneInfo.DaylightDate.wMonth != 0)
    {
        // get the local time
        GetLocalTime(&localTime);

        if (CompareTime(&localTime, &(TimeZoneInfo.DaylightDate)) >= 0 &&
            CompareTime(&localTime, &(TimeZoneInfo.StandardDate)) < 0)
            return TIME_ZONE_ID_DAYLIGHT;
        else
            return TIME_ZONE_ID_STANDARD;
    }
    */

    return TIME_ZONE_ID_UNKNOWN;
}

/*
**  Function : SetTimeZoneInformation
**  Purpose  :
**      The SetTimeZoneInformation function sets the current time-zone 
**      parameters. These parameters control translations from Coordinated 
**      Universal Time (UTC) to local time. 
**  Params   :
**      pTimeZoneInfo : Points to a TIME_ZONE_INFORMATION structure that
**                      contains the time-zone parameters to set
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended 
**      error information, call GetLastError
*/
BOOL WINAPI SetTimeZoneInformation(const TIME_ZONE_INFORMATION* pTimeZoneInfo)
{
    if (!pTimeZoneInfo)
    {
        SetLastError(1);
        return FALSE;
    }

    TimeZoneInfo.Bias = pTimeZoneInfo->Bias;
    strcpy(TimeZoneInfo.StandardName, pTimeZoneInfo->StandardName);

    TimeZoneInfo.StandardDate.wYear = pTimeZoneInfo->StandardDate.wYear;
    TimeZoneInfo.StandardDate.wMonth = pTimeZoneInfo->StandardDate.wMonth;
    TimeZoneInfo.StandardDate.wDayOfWeek = pTimeZoneInfo->StandardDate.wDayOfWeek;
    TimeZoneInfo.StandardDate.wDay = pTimeZoneInfo->StandardDate.wDay;
    TimeZoneInfo.StandardDate.wHour = pTimeZoneInfo->StandardDate.wHour;
    TimeZoneInfo.StandardDate.wMinute = pTimeZoneInfo->StandardDate.wMinute;
    TimeZoneInfo.StandardDate.wSecond = pTimeZoneInfo->StandardDate.wSecond;
    TimeZoneInfo.StandardDate.wMilliseconds = pTimeZoneInfo->StandardDate.wMilliseconds;

    TimeZoneInfo.StandardBias = pTimeZoneInfo->StandardBias;
    strcpy(TimeZoneInfo.DaylightName, pTimeZoneInfo->DaylightName);

    TimeZoneInfo.DaylightDate.wYear = pTimeZoneInfo->DaylightDate.wYear;
    TimeZoneInfo.DaylightDate.wMonth = pTimeZoneInfo->DaylightDate.wMonth;
    TimeZoneInfo.DaylightDate.wDayOfWeek = pTimeZoneInfo->DaylightDate.wDayOfWeek;
    TimeZoneInfo.DaylightDate.wDay = pTimeZoneInfo->DaylightDate.wDay;
    TimeZoneInfo.DaylightDate.wHour = pTimeZoneInfo->DaylightDate.wHour;
    TimeZoneInfo.DaylightDate.wMinute = pTimeZoneInfo->DaylightDate.wMinute;
    TimeZoneInfo.DaylightDate.wSecond = pTimeZoneInfo->DaylightDate.wSecond;
    TimeZoneInfo.DaylightDate.wMilliseconds = pTimeZoneInfo->DaylightDate.wMilliseconds;

    TimeZoneInfo.DaylightBias = pTimeZoneInfo->DaylightBias;

    return TRUE;
}

#endif // NOTIMEZONE

//*************************************************************************
//                           内 部 函 数
//*************************************************************************

/*
*   private function used to convert time by positive bias
*   formula is time2 = time1 + bias
*/
static void ConvertTimeByPositiveBias(const SYSTEMTIME* pTime1, PSYSTEMTIME pTime2, long lBias)
{
    int     nCount, i;
    WORD    wTime1Days;
    long    lTime1Min, lTime2Min;
    long    lDay, lReminderMin;

    if (IsLeapYear(pTime1->wYear))      // is leap year
        nDayOfMonth[1] = 29;

    wTime1Days = 0;
    for (i=0; i<pTime1->wMonth - 1; i++)
        wTime1Days += nDayOfMonth[i];

    wTime1Days += pTime1->wDay;

    // caculate minutes according to the pLocalTime
    lTime1Min = (long)(wTime1Days * 24 * 60 + 
                        pTime1->wHour * 60 + 
                        pTime1->wMinute);

    // get minutes of the pTime2
    lTime2Min = lTime1Min + lBias;

    /* 
     * convert the minutes to contents in the struature SYSTEMTIME
    */

    // caculate day numbers and the reminder
    lDay = lTime2Min / (24 * 60);
    lReminderMin = lTime2Min % (24 * 60);

    nCount = 0;
    while (1)
    {
        if (IsLeapYear((WORD)(pTime1->wYear + nCount))) // is leap year
        {
            if (lDay - 366 < 0)
                break;
            else
                lDay -= 366;
        }
        else
        {
            if (lDay - 365 < 0)
                break;
            else
                lDay -= 365;
        }
        nCount++;
    }

    pTime2->wYear = pTime1->wYear + nCount;

    if (IsLeapYear(pTime2->wYear))
        nDayOfMonth[1] = 29;
    else
        nDayOfMonth[1] = 28;

    for (i=0; i<12; i++)
    {
        if (lDay - nDayOfMonth[i] > 0)
            lDay -= nDayOfMonth[i];
        else
            break;
    }

    pTime2->wMonth = i + 1;
    pTime2->wDay = (WORD)lDay;

    // caculate the day of a week
    pTime2->wDayOfWeek = (WORD)(((lBias + 
        pTime1->wHour * 60 +
        pTime1->wMinute) / (24 * 60) + 
        pTime1->wDayOfWeek) % 7);

    // set the other members of pTime2
    pTime2->wHour = (WORD)(lReminderMin / 60);
    pTime2->wMinute = (WORD)(lReminderMin % 60);
    pTime2->wSecond = pTime1->wSecond;
    pTime2->wMilliseconds = pTime1->wMilliseconds;
}

/*
*   private function used to convert time by negative bias
*   formula is time2 = time1 - bias
*/
static void ConvertTimeByNegativeBias(const SYSTEMTIME* pTime1, PSYSTEMTIME pTime2, long lBias)
{
    int     nCount, i;
    WORD    wTime1Days;
    long    lTime1Min, lTime2Min;
    long    lDay, lReminderMin, lMinOfYear, lTmp;

    if (IsLeapYear(pTime1->wYear))      // is leap year
        nDayOfMonth[1] = 29;

    wTime1Days = 0;
    for (i=0; i<pTime1->wMonth - 1; i++)
        wTime1Days += nDayOfMonth[i];

    wTime1Days += pTime1->wDay;

    // caculate minutes according to the pTime2
    lTime1Min = (long)(wTime1Days * 24 * 60 + 
                    pTime1->wHour * 60 + 
                    pTime1->wMinute);

    // get minutes of the pTime2
    if (lTime1Min > lBias)
    {
        lTime2Min = lTime1Min - lBias;

        // caculate day numbers and the reminder
        lDay = lTime2Min / (24 * 60);
        lReminderMin = lTime2Min % (24 * 60);

        pTime2->wYear = pTime1->wYear;
    }
    else
    {
        lTime2Min = lBias - lTime1Min;

        nCount = 1;
        while (1)
        {
            if (IsLeapYear((WORD)(pTime1->wYear - nCount))) // is leap year
                lMinOfYear = 366L * 24 * 60;
            else
                lMinOfYear = 365L * 24 * 60;

            if (lTime2Min - lMinOfYear < 0)
            {
                lTime2Min = lMinOfYear - lTime2Min;
                break;
            }
            else
                lTime2Min -= lMinOfYear;

            nCount++;
        }

        // caculate day numbers and the reminder
        lDay = lTime2Min / (24 * 60);
        lReminderMin = lTime2Min % (24 * 60);

        pTime2->wYear = pTime1->wYear - nCount;

        if (IsLeapYear(pTime2->wYear))
            nDayOfMonth[1] = 29;
        else
            nDayOfMonth[1] = 28;
    }

    for (i=0; i<12; i++)
    {
        if (lDay - nDayOfMonth[i] > 0)
            lDay -= nDayOfMonth[i];
        else
            break;
    }

    pTime2->wMonth = i + 1;
    pTime2->wDay = (WORD)lDay;

    pTime2->wHour = (WORD)(lReminderMin / 60);
    pTime2->wMinute = (WORD)(lReminderMin % 60);

    pTime2->wSecond = pTime1->wSecond;
    pTime2->wMilliseconds = pTime1->wMilliseconds;

    // caculate the day of a week
    lTmp = lBias - (pTime1->wHour * 60 + pTime1->wMinute);
    if (lTmp <= 0)
        pTime2->wDayOfWeek = pTime1->wDayOfWeek;
    else
    {
        if (lTmp % (24 * 60) == 0)
            pTime2->wDayOfWeek = (WORD)((pTime1->wDayOfWeek + 7 -
                ((lTmp / (24 * 60)) % 7)) % 7);
        else
            pTime2->wDayOfWeek = (WORD)((pTime1->wDayOfWeek + 7 -
                ((lTmp / (24 * 60) + 1) % 7)) % 7);
    }
}

/*
*   private function used to judge whether is a leap year
*/
static BOOL IsLeapYear(WORD wYear)
{
    return (wYear % 400 == 0 || (wYear % 4 == 0 && wYear % 100 != 0));
}

/*
*   private function used to compare two time. If the first is larger( or smallar)
*   than the second.
*   the function return 1(or -1). If the two time are equal, the function return 0.
*/
static int CompareTime(const SYSTEMTIME* pTime1, const SYSTEMTIME* pTime2)
{
    int wCompareArray[7];
    int i = 0;

    wCompareArray[0] = pTime1->wYear - pTime2->wYear;
    wCompareArray[1] = pTime1->wMonth - pTime2->wMonth;
    wCompareArray[2] = pTime1->wDay - pTime2->wDay;
    wCompareArray[3] = pTime1->wHour - pTime2->wHour;
    wCompareArray[4] = pTime1->wMinute - pTime2->wMinute;
    wCompareArray[5] = pTime1->wSecond - pTime2->wSecond;
    wCompareArray[6] = pTime1->wMilliseconds - pTime2->wMilliseconds;

    do
    {
        if (wCompareArray[i] > 0)
            return 1;
        else if (wCompareArray[i] < 0)
            return -1;
        else
            i++;
    }while(i < 7);

    return 0;
}

/*
**  Function : IsTimeValid
**  Purpose  :
**      Retrieves whether a specified time is valid.
*/
static BOOL IsTimeValid(const SYSTEMTIME* pTime)
{
    if (pTime->wHour >= 24 || pTime->wMinute >= 60 ||
        pTime->wSecond >= 60 || pTime->wMilliseconds >= 1000 ||
        pTime->wMonth < 1 || pTime->wMonth >12 || pTime->wDay < 1)
        return FALSE;
    
    if (IsLeapYear(pTime->wYear) && (pTime->wMonth == 2))
        return (pTime->wDay <= 29);
    
    return (pTime->wDay <= (WORD)nDayOfMonth[pTime->wMonth - 1]);
}

//#ifndef _EMULATE_
#if 1
//#if (!RTC)

/*
**  Function : GetDayOfWeek
**  Purpose  :
**      Get the day of week for a speicified system time
*/
static int GetDayOfWeek(const SYSTEMTIME* pTime)
{
    int year, month, day, century;
    int nDayOfWeek;

    year = pTime->wYear;
    month = pTime->wMonth;
    day = pTime->wDay;

    if (month < 3)
    {
        month += 10;
        year--;
    }
    else
        month -= 2;

    century = year / 100;
    year = year % 100;

    nDayOfWeek = (((26 * month - 2) / 10) + day + year + (year / 4) + 
        (century / 4) - (2 * century)) % 7;

    if (nDayOfWeek < 0)
       nDayOfWeek += 7;

    return nDayOfWeek;
}

/*
**  Function : AddSpanToTime
**  Purpose  :
**      Adds a specified time span to a specified time
*/
static void AddSpanToTime(PSYSTEMTIME pTime, DWORD dwMilliseconds)
{
    int nCarry;
    int nMonthDays;
    int nDays;

    pTime->wMilliseconds += (WORD)(dwMilliseconds % 1000);
    if (pTime->wMilliseconds >= 1000)
    {
        pTime->wMilliseconds -= 1000;
        nCarry = 1;
    }
    else
        nCarry = 0;
    dwMilliseconds = dwMilliseconds / 1000;

    pTime->wSecond += (WORD)(dwMilliseconds % 60 + nCarry);
    if (pTime->wSecond >= 60)
    {
        pTime->wSecond -= 60;
        nCarry = 1;
    }
    else
        nCarry = 0;
    dwMilliseconds = dwMilliseconds / 60;

    pTime->wMinute += (WORD)(dwMilliseconds % 60 + nCarry);
    if (pTime->wMinute >= 60)
    {
        pTime->wMinute -= 60;
        nCarry = 1;
    }
    else
        nCarry = 0;
    dwMilliseconds = dwMilliseconds / 60;

    pTime->wHour += (WORD)(dwMilliseconds % 24 + nCarry);
    if (pTime->wHour >= 24)
    {
        pTime->wHour -= 24;
        nCarry = 1;
    }
    else
        nCarry = 0;
    dwMilliseconds = dwMilliseconds / 24;

    // Now dwMilliseconds is days in origin dwMilliseconds
    nDays = (int)(dwMilliseconds + nCarry + pTime->wDay - 1);
    pTime->wDay = 1;

    if (IsLeapYear(pTime->wYear) && pTime->wMonth == 2)
        nMonthDays = 29;
    else
        nMonthDays = nDayOfMonth[pTime->wMonth - 1];

    while (nDays >= nMonthDays)
    {
        pTime->wMonth++;
        if (pTime->wMonth > 12)
        {
            pTime->wMonth -= 12;
            pTime->wYear++;
        }
        nDays -= nMonthDays;

        if (IsLeapYear(pTime->wYear) && pTime->wMonth == 2)
            nMonthDays = 29;
        else
            nMonthDays = nDayOfMonth[pTime->wMonth - 1];
    }

    pTime->wDay += (WORD)nDays;
}

//#endif  // RTC
#endif // _EMULATE_

#endif // NOSYSTEMTIME

/*
**  Function : GetTickCount
**  Purpose  :
**      Retrieves the number of milliseconds that have elapsed since 
**      Windows was started. 
**  Params   :
**      None.
**  Return   :
**      If the function succeeds, the return value is the number of 
**      milliseconds that have elapsed since Windows was started. 
**  Remark   :
**      The elapsed time is stored as a DWORD value. Therefore, the time 
**      will wrap around to zero if Windows is run continuously for 49.7 
**      days. 
*/
DWORD WINAPI GetTickCount(void)
{
    return 1000 * PLXOS_GetSysClock() / PLXOS_GetTicksPerSecond();
}
