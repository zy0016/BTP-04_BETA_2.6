/**************************************************************************\
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Model   : traffic log
*
* Purpose : Compare log time
*  
* Author  : 
*
\**************************************************************************/
#include "window.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#define MAX_YEAR 2050
typedef enum
{
    Early = 0,
    Equality,
    Late
}cmpresult;
static BOOL DayIncrease(SYSTEMTIME *psystime, unsigned int day);
static int ALM_CalcDaysOfMonth(SYSTEMTIME *pDate);
static int ALM_CmpDate(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate);
static BOOL ALM_IsLeapYear(WORD wYear);

int DaynCmp(SYSTEMTIME systime1, SYSTEMTIME systime2, unsigned int numday)
{
    if(!(DayIncrease(&systime2, numday)))
    {
        return -1;//错误时间比较
    }
    return ALM_CmpDate(&systime1, &systime2);
}

static BOOL DayIncrease(SYSTEMTIME *psystime, unsigned int day)
{
    int outday;
    WORD	   wOldYear;
    WORD       wOldMonth;
    WORD	   wOldDay;

    // 暂存原有日期
    wOldYear  = psystime->wYear;
    wOldMonth = psystime->wMonth;
    wOldDay   = psystime->wDay;

    psystime->wDay += day;
    outday = ALM_CalcDaysOfMonth(psystime);
    while(psystime->wDay > outday)
    {
        psystime->wMonth ++;
        psystime->wDay -= outday;
        
        // 判断月份是否上溢
        if (psystime->wMonth > 12)
        {
            psystime->wYear ++;
            psystime->wMonth = 1;
//            psystime->wDay = 1;
            
            // 判断年是否上溢
            if (psystime->wYear > MAX_YEAR)
            {
                psystime->wYear  = wOldYear;
                psystime->wMonth = wOldMonth;
                psystime->wDay   = wOldDay;
                return FALSE;
            }
        }
        outday = ALM_CalcDaysOfMonth(psystime);
    }
    return TRUE;
}

static int ALM_CalcDaysOfMonth(SYSTEMTIME *pDate)
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
        if (ALM_IsLeapYear(pDate->wYear))
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

static BOOL ALM_IsLeapYear(WORD wYear)
{
    if ((wYear % 4 == 0 && wYear % 100 != 0) || (wYear % 400 == 0))
        return TRUE;
    else
        return FALSE;
}

static int ALM_CmpDate(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate)
{
    char  DateBuf1[11];
    char  DateBuf2[11];
    
    unsigned long Value1, Value2;
    
    memset(DateBuf1, 0, sizeof(DateBuf1));
    memset(DateBuf2, 0, sizeof(DateBuf2));
    
    sprintf(DateBuf1, "%04d%02d%02d", (int)pFirstDate->wYear,
        (int)pFirstDate->wMonth, 
        (int)pFirstDate->wDay
        );
    
    sprintf(DateBuf2, "%04d%02d%02d", (int)pSecondDate->wYear,
        (int)pSecondDate->wMonth, 
        (int)pSecondDate->wDay
        );
    
    Value1 = atol(DateBuf1);
    Value2 = atol(DateBuf2);
    
    if (Value1 < Value2)
        return Early;
    else if (Value1 > Value2)
        return Late;
    else
    {
        memset(DateBuf1, 0, sizeof(DateBuf1));
        memset(DateBuf2, 0, sizeof(DateBuf2));
        
        sprintf(DateBuf1, "%02d%02d", (int)pFirstDate->wHour,    (int)pFirstDate->wMinute);
        sprintf(DateBuf2, "%02d%02d", (int)pSecondDate->wHour,   (int)pSecondDate->wMinute);
        
        Value1 = atol(DateBuf1);
        Value2 = atol(DateBuf2);
        
        if (Value1 < Value2)
            return Early;
        else if (Value1 > Value2)
            return Late;
        else
            return Equality;
    }
}
