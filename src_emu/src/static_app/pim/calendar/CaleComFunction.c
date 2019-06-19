/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleComFunction.c
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleComFunction.h"

SCROLLINFO GloScroll;

int CALE_CmpOnlyDate(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate)
{
    char  DateBuf1[11];
    char  DateBuf2[11];
    
    unsigned long Value1, Value2;
    
    memset(DateBuf1, 0x0, 11);
    memset(DateBuf2, 0x0, 11);
    
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
        return Equality;  
}

int CALE_CmpOnlyTime(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate)
{
    char  DateBuf1[11];
    char  DateBuf2[11];

    unsigned long Value1, Value2;
    
    memset(DateBuf1, 0, 11);
    memset(DateBuf2, 0, 11);
    
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

//first<second = early
int CALE_CmpCaleDate(CaleDATE *pFirstDate, CaleDATE *pSecondDate)
{
    char  DateBuf1[11];
    char  DateBuf2[11];
    
    unsigned long Value1, Value2;
    
    memset(DateBuf1, 0x0, 11);
    memset(DateBuf2, 0x0, 11);
    
    sprintf(DateBuf1, "%04d%02d%02d", (int)pFirstDate->bYear,
                                      (int)pFirstDate->bMonth, 
                                      (int)pFirstDate->bDay
        );
    
    sprintf(DateBuf2, "%04d%02d%02d", (int)pSecondDate->bYear,
                                      (int)pSecondDate->bMonth, 
                                      (int)pSecondDate->bDay
        );
    
    Value1 = atol(DateBuf1);
    Value2 = atol(DateBuf2);
    
    if (Value1 < Value2)
        return Early;
    else if (Value1 > Value2)
        return Late;
    else
    {
        memset(DateBuf1, 0, 11);
        memset(DateBuf2, 0, 11);
        
        sprintf(DateBuf1, "%02d%02d", (int)pFirstDate->bHour,    (int)pFirstDate->bMinute);
        sprintf(DateBuf2, "%02d%02d", (int)pSecondDate->bHour,   (int)pSecondDate->bMinute);
        
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

//provide function to log
int CALE_CmpSysDate(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate)
{
    CaleDATE pFirstCale;
    CaleDATE pSecondCale;

    pFirstCale.bYear    = pFirstDate->wYear;
    pFirstCale.bMonth   = (BYTE)pFirstDate->wMonth;
    pFirstCale.bDay     = (BYTE)pFirstDate->wDay;
    pFirstCale.bHour    = (BYTE)pFirstDate->wHour;
    pFirstCale.bMinute  = (BYTE)pFirstDate->wMinute;
    pFirstCale.bSecond  = (BYTE)pFirstDate->wSecond;

    pSecondCale.bYear   = pSecondDate->wYear;
    pSecondCale.bMonth  = (BYTE)pSecondDate->wMonth;
    pSecondCale.bDay    = (BYTE)pSecondDate->wDay;
    pSecondCale.bHour   = (BYTE)pSecondDate->wHour;
    pSecondCale.bMinute = (BYTE)pSecondDate->wMinute;
    pSecondCale.bSecond = (BYTE)pSecondDate->wSecond;

    return (CALE_CmpCaleDate(&pFirstCale, &pSecondCale));
}

//Startdate is early
int CALE_IntervalSysDate(SYSTEMTIME *pStartDate, SYSTEMTIME *pEndDate)
{
    WORD wYear;
    DWORD nDaysDiff = 0;
    int nMonth, nDays;

    for(wYear = pStartDate->wYear; wYear < pEndDate->wYear; wYear++)
    {
        if(((wYear % 4 == 0) && (wYear % 100 != 0)) || (wYear % 400 == 0))
        {
            nDays = 366;
        }
        else
        {
            nDays = 365;
        }
        nDaysDiff += nDays;
    }
    for(nMonth = 1; nMonth < pEndDate->wMonth; nMonth ++)
    {
        SYSTEMTIME Date;

        Date.wYear = pEndDate->wYear;
        Date.wMonth = (WORD)nMonth;
        nDaysDiff += CALE_AlmDaysOfMonth(&Date);
    }
    nDaysDiff += pEndDate->wDay;
    for(nMonth = 1; nMonth < pStartDate->wMonth; nMonth ++)
    {
        SYSTEMTIME Date;
        
        Date.wYear = pStartDate->wYear;
        Date.wMonth = (WORD)nMonth;
        nDaysDiff -= CALE_AlmDaysOfMonth(&Date);
    }
    nDaysDiff -= pStartDate->wDay;

    return nDaysDiff;
}

int CALE_DaynCmp(SYSTEMTIME systime1, SYSTEMTIME systime2, unsigned int numday)
{
    if(!(CALE_DayIncrease(&systime2, numday)))
    {
        return -1;//error
    }
    return CALE_CmpSysDate(&systime1, &systime2);
}

BOOL CALE_DayIncrease(SYSTEMTIME *psystime, unsigned int day)
{
    int outday;
    WORD	   wOldYear;
    WORD       wOldMonth;
    WORD	   wOldDay;

    //save date
    wOldYear  = psystime->wYear;
    wOldMonth = psystime->wMonth;
    wOldDay   = psystime->wDay;
    
    psystime->wDay += day;
    outday = CALE_AlmDaysOfMonth(psystime);
    while(psystime->wDay > outday)
    {
        psystime->wMonth ++;
        psystime->wDay -= outday;

        if (psystime->wMonth > 12)
        {
            psystime->wYear ++;
            psystime->wMonth = 1;

            if (psystime->wYear > MAX_YEAR)
            {
                psystime->wYear  = wOldYear;
                psystime->wMonth = wOldMonth;
                psystime->wDay   = wOldDay;
                return FALSE;
            }
        }
        outday = CALE_AlmDaysOfMonth(psystime);
    }
    return TRUE;
}

BOOL CALE_DayReduce(SYSTEMTIME *psystime, unsigned int day)
{
    int outday;
    WORD	   wOldYear;
    WORD       wOldMonth;
    WORD	   wOldDay;
    
    //save date
    wOldYear  = psystime->wYear;
    wOldMonth = psystime->wMonth;
    wOldDay   = psystime->wDay;

    while(psystime->wDay <= day)
    {
        psystime->wMonth --;
        if (psystime->wMonth < 1)
        {
            psystime->wYear --;
            psystime->wMonth = 12;
            
            if (psystime->wYear < MIN_YEAR)
            {
                psystime->wYear  = wOldYear;
                psystime->wMonth = wOldMonth;
                psystime->wDay   = wOldDay;
                return FALSE;
            }
        }
        outday = CALE_AlmDaysOfMonth(psystime);
        psystime->wDay += outday;
        
    }
    psystime->wDay -= day;
    
    return TRUE;
}

int CALE_AlmDaysOfMonth(SYSTEMTIME *pDate)
{
    return CALE_GetDaysOfMonth(pDate->wYear, pDate->wMonth);
}

BOOL CALE_IsLeapYear(WORD wYear)
{
    if ((wYear % 4 == 0 && wYear % 100 != 0) || (wYear % 400 == 0))
        return TRUE;
    else
        return FALSE;
}

BOOL CALE_MonthIncrease(SYSTEMTIME *pSystime)
{
    WORD wOldYear;
    WORD wOldMonth;
    WORD wOldDay;

    wOldYear = pSystime->wYear;
    wOldMonth = pSystime->wMonth;
    wOldDay = pSystime->wDay;

    pSystime->wMonth ++;
    if(pSystime->wMonth > 12)
    {
        pSystime->wYear ++;
        pSystime->wMonth = 1;
        if(pSystime->wYear > MAX_YEAR)
        {
            pSystime->wYear = wOldYear;
            pSystime->wMonth = wOldMonth;
            pSystime->wDay = wOldDay;
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CALE_YearIncrease(SYSTEMTIME *pSystime)
{
    WORD wOldYear;
    WORD wOldMonth;
    WORD wOldDay;
    
    wOldYear = pSystime->wYear;
    wOldMonth = pSystime->wMonth;
    wOldDay = pSystime->wDay;
    
    pSystime->wYear ++;
    if(pSystime->wYear > MAX_YEAR)
    {
        pSystime->wYear = wOldYear;
        pSystime->wMonth = wOldMonth;
        pSystime->wDay =wOldDay;
        return FALSE;
    }
    
    return TRUE;
}

int CALE_CalcWkday(SYSTEMTIME* pSysInfo)
{
    int Year, Month, Day, Century;
    int nDayOfWeek;
    
    Year  = pSysInfo->wYear;
    Month = pSysInfo->wMonth;
    Day   = pSysInfo->wDay;
    
    if (Month < 3)
    {
        Month += 10;
        Year--;
    }
    else
    {
        Month -= 2;
    }
    
    Century = Year / 100;
    Year = Year % 100;
    
    nDayOfWeek = (((26 * Month - 2) / 10) + Day + Year + (Year / 4) + (Century / 4) - (2 * Century)) % 7;
    
    if (nDayOfWeek < 0)
    {
        nDayOfWeek += 7;
    }
    
    return nDayOfWeek;
}

BOOL CALE_SetScheduleFlag(CaleMonthSch *pSched)
{
    SYSTEMTIME StartSys;
    SYSTEMTIME EndSys;
    SYSTEMTIME CurDateStart;
    SYSTEMTIME CurDateEnd;
    SYSTEMTIME tmpDate;
    
    DWORD nItemID;
    int CmpRes;
    int DayInter;
    int i;
    int outday;
    int iOpen;

    CaleMeetingNode CurMeetDate;
    CaleAnniNode CurAnniDate;
    CaleEventNode CurEventDate;

    StartSys.wYear = pSched[0].wYear;
    StartSys.wMonth = pSched[0].wMonth;
    StartSys.wDay = pSched[0].wDay;
    StartSys.wHour = 0;
    StartSys.wMinute = 0;

    EndSys.wYear = pSched[MAX_SCH - 1].wYear;
    EndSys.wMonth = pSched[MAX_SCH - 1].wMonth;
    EndSys.wDay = pSched[MAX_SCH - 1].wDay;
    EndSys.wHour = 23;
    EndSys.wMinute = 59;

    if((iOpen = CALE_OpenApi(CALE_MEETING)) == CALE_FAIL)
    {
        return FALSE;
    }
    memset(&CurMeetDate, 0x0, sizeof(CaleMeetingNode));
    while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurMeetDate, 
        sizeof(CaleMeetingNode))) != CALE_FAIL)
    {
        if(CurMeetDate.LocalFlag == CALE_LOCAL)
        {
            CurDateStart.wYear = CurMeetDate.MeetStartYear;
            CurDateStart.wMonth = CurMeetDate.MeetStartMonth;
            CurDateStart.wDay = CurMeetDate.MeetStartDay;
            CurDateStart.wHour = CurMeetDate.MeetStartHour;
            CurDateStart.wMinute = CurMeetDate.MeetStartMinute;

			CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
			if(CmpRes == Late)
			{
				continue;
			}
			
            CurDateEnd.wYear = CurMeetDate.MeetEndYear;
            CurDateEnd.wMonth = CurMeetDate.MeetEndMonth;
            CurDateEnd.wDay = CurMeetDate.MeetEndDay;
            CurDateEnd.wHour = 23;
            CurDateEnd.wMinute = 59;
            
            if(CurMeetDate.MeetRepeatFlag == 0 || CurMeetDate.MeetRepeatStyle == CALE_REP_NONE)
            {
                CmpRes = CALE_CmpSysDate(&CurDateEnd, &StartSys);
                if(CmpRes == Early)
                {
                    continue;
                }

				CmpRes = CALE_CmpSysDate(&CurDateStart, &StartSys);
				if(Early == CmpRes)
				{
					memcpy(&CurDateStart, &StartSys, sizeof(SYSTEMTIME));
					DayInter = 0;
				}
				else
	                DayInter = CALE_IntervalSysDate(&StartSys, &CurDateStart);

				CmpRes = CALE_CmpSysDate(&CurDateEnd, &EndSys);
				if(Late == CmpRes)
					memcpy(&CurDateEnd, &EndSys, sizeof(SYSTEMTIME));

				i = CALE_IntervalSysDate(&CurDateStart, &CurDateEnd);
				while(i >= 0)
				{
					pSched[DayInter+i].bFlag |= CALE_SCHED_MEET;
					i --;
				}
            }
            else
            {
				switch(CurMeetDate.MeetRepeatStyle)
				{
				case CALE_REP_NONE:
					break;

				case CALE_REP_DAY:
					CmpRes = CALE_CmpSysDate(&CurDateStart, &StartSys);
					if(CmpRes != Late)
					{
						DayInter = 0;
					}
					else
					{
						DayInter = CALE_IntervalSysDate(&StartSys, &CurDateStart);
					}
					if((DayInter < 0) || (DayInter >= MAX_SCH))
					{
						continue;
					}
					
					for(i = DayInter; i < MAX_SCH; i ++)
					{
//						tmpDate.wYear = pSched[i].wYear;
//						tmpDate.wMonth = pSched[i].wMonth;
//						tmpDate.wDay = pSched[i].wDay;
//						tmpDate.wHour = 23;
//						tmpDate.wMinute = 59;
//						
//						CmpRes = CALE_CmpSysDate(&tmpDate, &CurDateEnd);
//						if((CmpRes == Early) || (CmpRes ==Equality))
						{
							pSched[i].bFlag |= CALE_SCHED_MEET;
						}
//						else
//						{
//							break;
//						}
					}
					break;

				case CALE_REP_WEEK:
					while(Early == CALE_CmpSysDate(&CurDateEnd, &StartSys))
					{
						CALE_DayIncrease(&CurDateStart, 7);
						CALE_DayIncrease(&CurDateEnd, 7);
					}
					while(Late != CALE_CmpSysDate(&CurDateStart, &EndSys))
					{
						if(Late !=	CALE_CmpSysDate(&CurDateStart, &StartSys))
							DayInter = 0;
						else
							DayInter = CALE_IntervalSysDate(&StartSys, &CurDateStart);

						outday = min(MAX_SCH, CALE_IntervalSysDate(&StartSys, &CurDateEnd)+1);

						for(i=DayInter; i<outday; i++)
							pSched[i].bFlag |= CALE_SCHED_MEET;

						CALE_DayIncrease(&CurDateStart, 7);
						CALE_DayIncrease(&CurDateEnd, 7);
					}
					break;

				case CALE_REP_TWOWEEK:
					while(Early == CALE_CmpSysDate(&CurDateEnd, &StartSys))
					{
						CALE_DayIncrease(&CurDateStart, 7*2);
						CALE_DayIncrease(&CurDateEnd, 7*2);
					}
					while(Late != CALE_CmpSysDate(&CurDateStart, &EndSys))
					{
						if(Late !=	CALE_CmpSysDate(&CurDateStart, &StartSys))
							DayInter = 0;
						else
							DayInter = CALE_IntervalSysDate(&StartSys, &CurDateStart);

						outday = min(MAX_SCH, CALE_IntervalSysDate(&StartSys, &CurDateEnd)+1);

						for(i=DayInter; i<outday; i++)
							pSched[i].bFlag |= CALE_SCHED_MEET;

						CALE_DayIncrease(&CurDateStart, 7*2);
						CALE_DayIncrease(&CurDateEnd, 7*2);
					}
					break;

				case CALE_REP_MONTH:
					{
						SYSTEMTIME dStart, dEnd;
						
						while(Early == CALE_CmpSysDate(&CurDateEnd, &StartSys))
						{
							CALE_MonthIncrease(&CurDateStart);
							CALE_MonthIncrease(&CurDateEnd);
						}
						while(Late != CALE_CmpSysDate(&CurDateStart, &EndSys))
						{
							if(Late !=	CALE_CmpSysDate(&CurDateStart, &StartSys))
								memcpy(&dStart, &StartSys, sizeof(SYSTEMTIME));
							else
								memcpy(&dStart, &CurDateStart, sizeof(SYSTEMTIME));

							outday = CALE_AlmDaysOfMonth(&dStart);
							if(dStart.wDay > outday)
							{
								CALE_MonthIncrease(&dStart);
								dStart.wDay = 1;
								dStart.wHour = 0;
								dStart.wMinute = 0;
							}

							if(Early !=	CALE_CmpSysDate(&CurDateEnd, &EndSys))
								memcpy(&dEnd, &EndSys, sizeof(SYSTEMTIME));
							else
								memcpy(&dEnd, &CurDateEnd, sizeof(SYSTEMTIME));

							outday = CALE_AlmDaysOfMonth(&dEnd);
							if(dEnd.wDay > outday)
							{
								dEnd.wDay = outday;
								dEnd.wHour = 23;
								dEnd.wMinute = 59;
							}

							if(CALE_CmpSysDate(&dStart, &dEnd) == Late)
							{
								CALE_MonthIncrease(&CurDateStart);
								CALE_MonthIncrease(&CurDateEnd);
								continue;
							}

							DayInter = CALE_IntervalSysDate(&StartSys, &dStart);

							outday = min(MAX_SCH, CALE_IntervalSysDate(&StartSys, &dEnd)+1);

							for(i=DayInter; i<outday; i++)
								pSched[i].bFlag |= CALE_SCHED_MEET;

							CALE_MonthIncrease(&CurDateStart);
							CALE_MonthIncrease(&CurDateEnd);
						}
					}
					break;

				case CALE_REP_YEAR:
					{
						SYSTEMTIME dStart, dEnd;
						
						while(Early == CALE_CmpSysDate(&CurDateEnd, &StartSys))
						{
							CALE_YearIncrease(&CurDateStart);
							CALE_YearIncrease(&CurDateEnd);
						}
						while(Late != CALE_CmpSysDate(&CurDateStart, &EndSys))
						{
							if(Late !=	CALE_CmpSysDate(&CurDateStart, &StartSys))
								memcpy(&dStart, &StartSys, sizeof(SYSTEMTIME));
							else
								memcpy(&dStart, &CurDateStart, sizeof(SYSTEMTIME));

							outday = CALE_AlmDaysOfMonth(&dStart);
							if(dStart.wDay > outday)
							{
								CALE_MonthIncrease(&dStart);
								dStart.wDay = 1;
								dStart.wHour = 0;
								dStart.wMinute = 0;
							}

							if(Early !=	CALE_CmpSysDate(&CurDateEnd, &EndSys))
								memcpy(&dEnd, &EndSys, sizeof(SYSTEMTIME));
							else
								memcpy(&dEnd, &CurDateEnd, sizeof(SYSTEMTIME));

							outday = CALE_AlmDaysOfMonth(&dEnd);
							if(dEnd.wDay > outday)
							{
								dEnd.wDay = outday;
								dEnd.wHour = 23;
								dEnd.wMinute = 59;
							}

							if(CALE_CmpSysDate(&dStart, &dEnd) == Late)
							{
								CALE_YearIncrease(&CurDateStart);
								CALE_YearIncrease(&CurDateEnd);
								continue;
							}

							DayInter = CALE_IntervalSysDate(&StartSys, &dStart);

							outday = min(MAX_SCH, CALE_IntervalSysDate(&StartSys, &dEnd)+1);

							for(i=DayInter; i<outday; i++)
								pSched[i].bFlag |= CALE_SCHED_MEET;

							CALE_YearIncrease(&CurDateStart);
							CALE_YearIncrease(&CurDateEnd);
						}
					}
					break;

				default:
					break;
				}
			}//end else if(CurMeetDate.MeetRepeatFlag == 0)
		}//end if(CurMeetDate.LocalFlag == CALE_LOCAL)
	}//end while
	CALE_CloseApi(iOpen);

	if((iOpen = CALE_OpenApi(CALE_ANNI)) == CALE_FAIL)
	{
		return FALSE;
	}
	memset(&CurAnniDate, 0x0, sizeof(CaleAnniNode));
	while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurAnniDate, 
		sizeof(CaleAnniNode))) != CALE_FAIL)
	{
		if(CurAnniDate.LocalFlag == CALE_LOCAL)
		{
			CurDateStart.wYear = CurAnniDate.AnniDisYear;
			CurDateStart.wMonth = CurAnniDate.AnniDisMonth;
			CurDateStart.wDay = CurAnniDate.AnniDisDay;
			CurDateStart.wHour = CurAnniDate.AnniHour;
			CurDateStart.wMinute = CurAnniDate.AnniMinute;

			CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
			if(CmpRes == Late)
			{
				continue;
			}

			CmpRes = CALE_CmpSysDate(&CurDateStart, &StartSys);
			if(CmpRes == Early)
			{
				while(1)
				{
					CmpRes = CALE_YearIncrease(&CurDateStart);
					if(CmpRes == FALSE)
					{
						break;
					}
					if((CALE_CmpSysDate(&StartSys, &CurDateStart)) != Late)
					{
						break;
					}
				}
			}
			if(CmpRes == FALSE)
			{
				continue;
			}
						
			//exceed days of current month
			outday = CALE_AlmDaysOfMonth(&CurDateStart);
			if(CurDateStart.wDay > outday)
			{
				CurDateStart.wDay = outday;
			}

			DayInter = CALE_IntervalSysDate(&StartSys, &CurDateStart);
			if((DayInter < 0) || (DayInter >= MAX_SCH))
			{
				continue;
			}
			pSched[DayInter].bFlag |= CALE_SCHED_ANNI;
		}//end if(CurMeetDate.LocalFlag == CALE_LOCAL)
	}
	CALE_CloseApi(iOpen);

	if((iOpen = CALE_OpenApi(CALE_EVENT)) == CALE_FAIL)
	{
		return FALSE;
	}
	memset(&CurEventDate, 0x0, sizeof(CaleEventNode));
	while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurEventDate, 
		sizeof(CaleEventNode))) != CALE_FAIL)
	{
		if(CurEventDate.LocalFlag == CALE_LOCAL)
		{
			CurDateStart.wYear = CurEventDate.EventStartYear;
			CurDateStart.wMonth = CurEventDate.EventStartMonth;
			CurDateStart.wDay = CurEventDate.EventStartDay;
			CurDateStart.wHour = CurEventDate.EventStartHour;
			CurDateStart.wMinute = CurEventDate.EventStartMinute;

			CurDateEnd.wYear = CurEventDate.EventEndYear;
			CurDateEnd.wMonth = CurEventDate.EventEndMonth;
			CurDateEnd.wDay = CurEventDate.EventEndDay;
			CurDateEnd.wHour = 23;
			CurDateEnd.wMinute = 59;
			
			CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
			if(CmpRes == Late)
			{
				continue;
			}
			CmpRes = CALE_CmpSysDate(&CurDateEnd, &StartSys);
			if(CmpRes == Early)
			{
				continue;
			}

			CmpRes = CALE_CmpSysDate(&CurDateStart, &StartSys);
			if(CmpRes != Late)
			{
				DayInter = 0;
			}
			else
			{
				DayInter = CALE_IntervalSysDate(&StartSys, &CurDateStart);
			}
			if((DayInter < 0) || (DayInter >= MAX_SCH))
			{
				continue;
			}
			
			for(i = DayInter; i < MAX_SCH; i ++)
			{
				tmpDate.wYear = pSched[i].wYear;
				tmpDate.wMonth = pSched[i].wMonth;
				tmpDate.wDay = pSched[i].wDay;
				tmpDate.wHour = 23;
				tmpDate.wMinute = 59;

				CmpRes = CALE_CmpSysDate(&tmpDate, &CurDateEnd);
				if((CmpRes == Early) || (CmpRes ==Equality))
				{
					pSched[i].bFlag |= CALE_SCHED_EVENT;
				}
				else
				{
					break;
				}
			}
		}//end if(CurEventDate.LocalFlag == CALE_LOCAL)
	}
	CALE_CloseApi(iOpen);

	return TRUE;
}

BOOL CALE_SetWeekSchedule(PCaleWeek pWeek)
{
	SYSTEMTIME StartSys;
	SYSTEMTIME EndSys;
	SYSTEMTIME CurDateStart;
	SYSTEMTIME CurDateEnd;
	
	DWORD nItemID;
	int CmpRes;
	int DayInter;
	int i, j;
	int outday;
	int iOpen;
	int iStart, iEnd, iBew;

	CaleMeetingNode CurMeetDate;
	CaleAnniNode CurAnniDate;
	CaleEventNode CurEventDate;

	CaleWeekSchNode *NewSch;

	memcpy(&StartSys, &pWeek->StartTime, sizeof(SYSTEMTIME));
	memcpy(&EndSys, &pWeek->EndTime, sizeof(SYSTEMTIME));

	if((iOpen = CALE_OpenApi(CALE_MEETING)) == CALE_FAIL)
	{
		return FALSE;
	}
	memset(&CurMeetDate, 0x0, sizeof(CaleMeetingNode));
	while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurMeetDate, 
		sizeof(CaleMeetingNode))) != CALE_FAIL)
	{
		if(CurMeetDate.LocalFlag == CALE_LOCAL)
		{
			CurDateStart.wYear = CurMeetDate.MeetStartYear;
			CurDateStart.wMonth = CurMeetDate.MeetStartMonth;
			CurDateStart.wDay = CurMeetDate.MeetStartDay;
			CurDateStart.wHour = CurMeetDate.MeetStartHour;
			CurDateStart.wMinute = CurMeetDate.MeetStartMinute;

			CurDateEnd.wYear = CurMeetDate.MeetEndYear;
			CurDateEnd.wMonth = CurMeetDate.MeetEndMonth;
			CurDateEnd.wDay = CurMeetDate.MeetEndDay;
			CurDateEnd.wHour = CurMeetDate.MeetEndHour;//23;
			CurDateEnd.wMinute = CurMeetDate.MeetEndMinute;//59;
			
			CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
			if(CmpRes == Late)
			{
				continue;
			}
			if(CurMeetDate.MeetRepeatFlag == 0 || CALE_REP_NONE == CurMeetDate.MeetRepeatStyle)
			{
				CmpRes = CALE_CmpSysDate(&CurDateEnd, &StartSys);
				if(CmpRes == Early)
				{
					continue;
				}

				CmpRes = CALE_CmpSysDate(&CurDateStart, &StartSys);
				if(Early == CmpRes)
				{
					DayInter = 0;
					memcpy(&CurDateStart, &StartSys, sizeof(SYSTEMTIME));
				}
				else
				{
					DayInter = CALE_IntervalSysDate(&StartSys, &CurDateStart);
				}

				CmpRes = CALE_CmpSysDate(&CurDateEnd, &EndSys);
				if(Late == CmpRes)
				{
					memcpy(&CurDateEnd, &EndSys, sizeof(SYSTEMTIME));
				}
				
#ifdef _NOKIA_CALE_
				pWeek->TimeRow[CurMeetDate.MeetStartHour][DayInter] |= CALE_SCHED_MEET;
#endif
#ifdef _BENEFON_CALE_
				iStart = CurDateStart.wMinute / 6;
				iEnd   = CurDateEnd.wMinute / 6;
				iBew   = CurDateEnd.wHour - CurDateStart.wHour;
				iBew  += CALE_IntervalSysDate(&CurDateStart, &CurDateEnd)*24;
				j = CurDateStart.wHour;
				if(iBew < 0)
				{
					continue;
				}
				else if((iBew == 0) && (iEnd <= iStart))
				{
					continue;
				}
				else
				{
					NewSch = NULL;
					NewSch = (CaleWeekSchNode *)malloc(sizeof(CaleWeekSchNode));
					if(NewSch == NULL)
					{
						return FALSE;
					}
					memset(NewSch, 0x0, sizeof(CaleWeekSchNode));
					NewSch->pNext = NULL;
					NewSch->nStart = iStart;
					while (iBew)
					{
						NewSch->nEnd = 10;
						NewSch->pNext = NULL;
						CALE_AddWeekSchNode(&pWeek->TimeRow[j][DayInter], NewSch);
						j ++;
						if(j > 23)
						{
							j = 0;
							DayInter ++;
						}
						NewSch = NULL;
						NewSch = (CaleWeekSchNode *)malloc(sizeof(CaleWeekSchNode));
						if(NewSch == NULL)
						{
							return FALSE;
						}
						memset(NewSch, 0x0, sizeof(CaleWeekSchNode));
						NewSch->pNext = NULL;
						NewSch->nStart = 0;
						iBew --;
					}
					NewSch->nEnd = iEnd;
					NewSch->pNext = NULL;
					CALE_AddWeekSchNode(&pWeek->TimeRow[j][DayInter], NewSch);
				}
#endif
			}
			else
			{
				switch(CurMeetDate.MeetRepeatStyle)
				{
				case CALE_REP_DAY:
					{
						SYSTEMTIME curDay;
						CmpRes = CALE_CmpSysDate(&CurDateStart, &StartSys);
						if(CmpRes != Late)
						{
							DayInter = 0;
							memcpy(&curDay, &StartSys, sizeof(SYSTEMTIME));
						}
						else
						{
							DayInter = CALE_IntervalSysDate(&StartSys, &CurDateStart);
							memcpy(&curDay, &CurDateStart, sizeof(SYSTEMTIME));
						}
						if((DayInter < 0) || (DayInter >= MAX_WEEK_LINE))
						{
							continue;
						}
						
						for(i = DayInter; i < MAX_WEEK_LINE; i ++)
						{
							CmpRes = CALE_CmpSysDate(&CurDateStart, &curDay);
							if(Late == CmpRes)
								continue;
							
#ifdef _NOKIA_CALE_
							pWeek->TimeRow[CurMeetDate.MeetStartHour][i] |= CALE_SCHED_MEET;
#endif
#ifdef _BENEFON_CALE_
							iStart = CurMeetDate.MeetStartMinute / 6;
							iEnd = CurMeetDate.MeetEndMinute / 6;
							iBew = CurMeetDate.MeetEndHour - CurMeetDate.MeetStartHour;
							iBew += 24*CALE_IntervalSysDate(&CurDateStart, &CurDateEnd);
							j = CurMeetDate.MeetStartHour;
							if(iBew < 0)
							{
								continue;
							}
							else if((iBew == 0) && (iEnd < iStart))
							{
								continue;
							}
							else
							{
								NewSch = NULL;
								NewSch = (CaleWeekSchNode *)malloc(sizeof(CaleWeekSchNode));
								if(NewSch == NULL)
								{
									return FALSE;
								}
								memset(NewSch, 0x0, sizeof(CaleWeekSchNode));
								NewSch->nStart = iStart;
								while (iBew)
								{
									NewSch->nEnd = 10;
									NewSch->pNext = NULL;
									CALE_AddWeekSchNode(&pWeek->TimeRow[j][i], NewSch);
									j ++;
									if(j > 23)
									{
										if(Equality == CmpRes)
											break;
										else
											j = 0;
									}
									NewSch = NULL;
									NewSch = (CaleWeekSchNode *)malloc(sizeof(CaleWeekSchNode));
									if(NewSch == NULL)
									{
										return FALSE;
									}
									memset(NewSch, 0x0, sizeof(CaleWeekSchNode));
									NewSch->nStart = 0;
									iBew --;
								}
								if(j <= 23)
								{
									NewSch->nEnd = iEnd;
									NewSch->pNext = NULL;
									CALE_AddWeekSchNode(&pWeek->TimeRow[j][i], NewSch);
								}
							}
							CALE_DayIncrease(&curDay, 1);
#endif
						}
					}
					break;
					
				case CALE_REP_WEEK:
					{
						SYSTEMTIME dStart, dEnd;
						while(Early == CALE_CmpSysDate(&CurDateEnd, &StartSys))
						{
							CALE_DayIncrease(&CurDateStart, 7);
							CALE_DayIncrease(&CurDateEnd, 7);
						}
						while(Late != CALE_CmpSysDate(&CurDateStart, &EndSys))
						{
							if(Early == CALE_CmpSysDate(&CurDateStart, &StartSys))
								memcpy(&dStart, &StartSys, sizeof(SYSTEMTIME));
							else
								memcpy(&dStart, &CurDateStart, sizeof(SYSTEMTIME));
							
							if(Late == CALE_CmpSysDate(&CurDateEnd, &EndSys))
								memcpy(&dEnd, &EndSys, sizeof(SYSTEMTIME));
							else
								memcpy(&dEnd, &CurDateEnd, sizeof(SYSTEMTIME));
							
							
							DayInter = CALE_IntervalSysDate(&StartSys, &dStart);
#ifdef _NOKIA_CALE_
							pWeek->TimeRow[CurMeetDate.MeetStartHour][DayInter] |= CALE_SCHED_MEET;
#endif
#ifdef _BENEFON_CALE_
							iStart = dStart.wMinute / 6;
							iEnd   = dEnd.wMinute / 6;
							iBew   = dEnd.wHour - dStart.wHour;
							iBew  += CALE_IntervalSysDate(&dStart, &dEnd) * 24;
							j = dStart.wHour;
							if((iBew > 0) || (iBew == 0 && iEnd > iStart))
							{
								NewSch = NULL;
								NewSch = (CaleWeekSchNode *)malloc(sizeof(CaleWeekSchNode));
								if(NewSch == NULL)
								{
									return FALSE;
								}
								memset(NewSch, 0x0, sizeof(CaleWeekSchNode));
								NewSch->nStart = iStart;
								while (iBew)
								{
									NewSch->nEnd = 10;
									NewSch->pNext = NULL;
									CALE_AddWeekSchNode(&pWeek->TimeRow[j][DayInter], NewSch);
									j ++;
									if(j > 23)
									{
										j = 0;
										DayInter ++;
									}
									NewSch = NULL;
									NewSch = (CaleWeekSchNode *)malloc(sizeof(CaleWeekSchNode));
									if(NewSch == NULL)
									{
										return FALSE;
									}
									memset(NewSch, 0x0, sizeof(CaleWeekSchNode));
									NewSch->nStart = 0;
									iBew --;
								}
								NewSch->nEnd = iEnd;
								NewSch->pNext = NULL;
								CALE_AddWeekSchNode(&pWeek->TimeRow[j][DayInter], NewSch);
							}
							CALE_DayIncrease(&CurDateStart, 7);
							CALE_DayIncrease(&CurDateEnd, 7);
#endif
						}
					}
					break;
					
				case CALE_REP_TWOWEEK:
					{
						SYSTEMTIME dStart, dEnd;
						while(Early == CALE_CmpSysDate(&CurDateEnd, &StartSys))
						{
							CALE_DayIncrease(&CurDateStart, 7*2);
							CALE_DayIncrease(&CurDateEnd, 7*2);
						}
						while(Late != CALE_CmpSysDate(&CurDateStart, &EndSys))
						{
							if(Early == CALE_CmpSysDate(&CurDateStart, &StartSys))
								memcpy(&dStart, &StartSys, sizeof(SYSTEMTIME));
							else
								memcpy(&dStart, &CurDateStart, sizeof(SYSTEMTIME));
							
							if(Late == CALE_CmpSysDate(&CurDateEnd, &EndSys))
								memcpy(&dEnd, &EndSys, sizeof(SYSTEMTIME));
							else
								memcpy(&dEnd, &CurDateEnd, sizeof(SYSTEMTIME));
							
							
							DayInter = CALE_IntervalSysDate(&StartSys, &dStart);
#ifdef _NOKIA_CALE_
							pWeek->TimeRow[CurMeetDate.MeetStartHour][DayInter] |= CALE_SCHED_MEET;
#endif
#ifdef _BENEFON_CALE_
							iStart = dStart.wMinute / 6;
							iEnd   = dEnd.wMinute / 6;
							iBew   = dEnd.wHour - dStart.wHour;
							iBew  += CALE_IntervalSysDate(&dStart, &dEnd) * 24;
							j = dStart.wHour;
							if((iBew > 0) || (iBew == 0 && iEnd > iStart))
							{
								NewSch = NULL;
								NewSch = (CaleWeekSchNode *)malloc(sizeof(CaleWeekSchNode));
								if(NewSch == NULL)
								{
									return FALSE;
								}
								memset(NewSch, 0x0, sizeof(CaleWeekSchNode));
								NewSch->nStart = iStart;
								while (iBew)
								{
									NewSch->nEnd = 10;
									NewSch->pNext = NULL;
									CALE_AddWeekSchNode(&pWeek->TimeRow[j][DayInter], NewSch);
									j ++;
									if(j > 23)
									{
										j = 0;
										DayInter ++;
									}
									NewSch = NULL;
									NewSch = (CaleWeekSchNode *)malloc(sizeof(CaleWeekSchNode));
									if(NewSch == NULL)
									{
										return FALSE;
									}
									memset(NewSch, 0x0, sizeof(CaleWeekSchNode));
									NewSch->nStart = 0;
									iBew --;
								}
								NewSch->nEnd = iEnd;
								NewSch->pNext = NULL;
								CALE_AddWeekSchNode(&pWeek->TimeRow[j][DayInter], NewSch);
							}
							CALE_DayIncrease(&CurDateStart, 7*2);
							CALE_DayIncrease(&CurDateEnd, 7*2);
#endif
						}
					}
					break;
					
				case CALE_REP_MONTH:
					{
						SYSTEMTIME dStart, dEnd;
						while(Early == CALE_CmpSysDate(&CurDateEnd, &StartSys))
						{
							CALE_MonthIncrease(&CurDateStart);
							CALE_MonthIncrease(&CurDateEnd);
						}
						while(Late != CALE_CmpSysDate(&CurDateStart, &EndSys))
						{
							if(Early == CALE_CmpSysDate(&CurDateStart, &StartSys))
								memcpy(&dStart, &StartSys, sizeof(SYSTEMTIME));
							else
								memcpy(&dStart, &CurDateStart, sizeof(SYSTEMTIME));
							
							outday = CALE_AlmDaysOfMonth(&dStart);
							if(outday < dStart.wDay)
							{
								CALE_MonthIncrease(&dStart);
								dStart.wDay = 1;
								dStart.wHour = 0;
								dStart.wMinute = 0;
							}
							
							if(Late == CALE_CmpSysDate(&CurDateEnd, &EndSys))
								memcpy(&dEnd, &EndSys, sizeof(SYSTEMTIME));
							else
								memcpy(&dEnd, &CurDateEnd, sizeof(SYSTEMTIME));
							
							outday = CALE_AlmDaysOfMonth(&dEnd);
							if(outday < dEnd.wDay)
							{
								dEnd.wDay = outday;
								dEnd.wHour = 23;
								dEnd.wMinute = 59;
							}
							
							if(CALE_CmpSysDate(&dStart, &dEnd) == Late)
							{
								CALE_MonthIncrease(&CurDateStart);
								CALE_MonthIncrease(&CurDateEnd);
								continue;
							}
							
							DayInter = CALE_IntervalSysDate(&StartSys, &dStart);
#ifdef _NOKIA_CALE_
							pWeek->TimeRow[CurMeetDate.MeetStartHour][DayInter] |= CALE_SCHED_MEET;
#endif
#ifdef _BENEFON_CALE_
							iStart = dStart.wMinute / 6;
							iEnd   = dEnd.wMinute / 6;
							iBew   = dEnd.wHour - dStart.wHour;
							iBew  += CALE_IntervalSysDate(&dStart, &dEnd) * 24;
							j = dStart.wHour;
							if((iBew > 0) || (iBew == 0 && iEnd > iStart))
							{
								NewSch = NULL;
								NewSch = (CaleWeekSchNode *)malloc(sizeof(CaleWeekSchNode));
								if(NewSch == NULL)
								{
									return FALSE;
								}
								memset(NewSch, 0x0, sizeof(CaleWeekSchNode));
								NewSch->nStart = iStart;
								while (iBew)
								{
									NewSch->nEnd = 10;
									NewSch->pNext = NULL;
									CALE_AddWeekSchNode(&pWeek->TimeRow[j][DayInter], NewSch);
									j ++;
									if(j > 23)
									{
										j = 0;
										DayInter ++;
									}
									NewSch = NULL;
									NewSch = (CaleWeekSchNode *)malloc(sizeof(CaleWeekSchNode));
									if(NewSch == NULL)
									{
										return FALSE;
									}
									memset(NewSch, 0x0, sizeof(CaleWeekSchNode));
									NewSch->nStart = 0;
									iBew --;
								}
								NewSch->nEnd = iEnd;
								NewSch->pNext = NULL;
								CALE_AddWeekSchNode(&pWeek->TimeRow[j][DayInter], NewSch);
							}
							CALE_MonthIncrease(&CurDateStart);
							CALE_MonthIncrease(&CurDateEnd);
#endif
						}
					}
					break;
					
				case CALE_REP_YEAR:
					{
						SYSTEMTIME dStart, dEnd;
						while(Early == CALE_CmpSysDate(&CurDateEnd, &StartSys))
						{
							CALE_YearIncrease(&CurDateStart);
							CALE_YearIncrease(&CurDateEnd);
						}
						while(Late != CALE_CmpSysDate(&CurDateStart, &EndSys))
						{
							if(Early == CALE_CmpSysDate(&CurDateStart, &StartSys))
								memcpy(&dStart, &StartSys, sizeof(SYSTEMTIME));
							else
								memcpy(&dStart, &CurDateStart, sizeof(SYSTEMTIME));
							
							outday = CALE_AlmDaysOfMonth(&dStart);
							if(outday < dStart.wDay)
							{
								CALE_MonthIncrease(&dStart);
								dStart.wDay = 1;
								dStart.wHour = 0;
								dStart.wMinute = 0;
							}

							if(Late == CALE_CmpSysDate(&CurDateEnd, &EndSys))
								memcpy(&dEnd, &EndSys, sizeof(SYSTEMTIME));
							else
								memcpy(&dEnd, &CurDateEnd, sizeof(SYSTEMTIME));
							
							outday = CALE_AlmDaysOfMonth(&dEnd);
							if(outday < dEnd.wDay)
							{
								dEnd.wDay = outday;
								dEnd.wHour = 23;
								dEnd.wMinute = 59;
							}
							
							if(CALE_CmpSysDate(&dStart, &dEnd) == Late)
							{
								CALE_YearIncrease(&CurDateStart);
								CALE_YearIncrease(&CurDateEnd);
								continue;
							}

							DayInter = CALE_IntervalSysDate(&StartSys, &dStart);
#ifdef _NOKIA_CALE_
							pWeek->TimeRow[CurMeetDate.MeetStartHour][DayInter] |= CALE_SCHED_MEET;
#endif
#ifdef _BENEFON_CALE_
							iStart = dStart.wMinute / 6;
							iEnd   = dEnd.wMinute / 6;
							iBew   = dEnd.wHour - dStart.wHour;
							iBew  += CALE_IntervalSysDate(&dStart, &dEnd) * 24;
							j = dStart.wHour;
							if((iBew > 0) || (iBew == 0 && iEnd > iStart))
							{
								NewSch = NULL;
								NewSch = (CaleWeekSchNode *)malloc(sizeof(CaleWeekSchNode));
								if(NewSch == NULL)
								{
									return FALSE;
								}
								memset(NewSch, 0x0, sizeof(CaleWeekSchNode));
								NewSch->nStart = iStart;
								while (iBew)
								{
									NewSch->nEnd = 10;
									NewSch->pNext = NULL;
									CALE_AddWeekSchNode(&pWeek->TimeRow[j][DayInter], NewSch);
									j ++;
									if(j > 23)
									{
										j = 0;
										DayInter ++;
									}
									NewSch = NULL;
									NewSch = (CaleWeekSchNode *)malloc(sizeof(CaleWeekSchNode));
									if(NewSch == NULL)
									{
										return FALSE;
									}
									memset(NewSch, 0x0, sizeof(CaleWeekSchNode));
									NewSch->nStart = 0;
									iBew --;
								}
								NewSch->nEnd = iEnd;
								NewSch->pNext = NULL;
								CALE_AddWeekSchNode(&pWeek->TimeRow[j][DayInter], NewSch);
							}
							CALE_YearIncrease(&CurDateStart);
							CALE_YearIncrease(&CurDateEnd);
#endif
						}
					}
					break;
					
				default:
					break;
				}
			}//end else if(CurMeetDate.MeetRepeatFlag == 0)
		}//end if(CurMeetDate.LocalFlag == CALE_LOCAL)
	}//end while
	CALE_CloseApi(iOpen);

	if((iOpen = CALE_OpenApi(CALE_ANNI)) == CALE_FAIL)
	{
		return FALSE;
	}
	memset(&CurAnniDate, 0x0, sizeof(CaleAnniNode));
	while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurAnniDate, 
		sizeof(CaleAnniNode))) != CALE_FAIL)
	{
		if(CurAnniDate.LocalFlag == CALE_LOCAL)
		{
			CurDateStart.wYear = CurAnniDate.AnniDisYear;
			CurDateStart.wMonth = CurAnniDate.AnniDisMonth;
			CurDateStart.wDay = CurAnniDate.AnniDisDay;
			CurDateStart.wHour = CurAnniDate.AnniHour;
			CurDateStart.wMinute = CurAnniDate.AnniMinute;

			CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
			if(CmpRes == Late)
			{
				continue;
			}

			CmpRes = CALE_CmpSysDate(&CurDateStart, &StartSys);
			if(CmpRes == Early)
			{
				while(1)
				{
					CmpRes = CALE_YearIncrease(&CurDateStart);
					if(CmpRes == FALSE)
					{
						break;
					}
					if((CALE_CmpSysDate(&StartSys, &CurDateStart)) != Late)
					{
						break;
					}
				}
			}
			if(CmpRes == FALSE)
			{
				continue;
			}
						
			//exceed days of current month
			outday = CALE_AlmDaysOfMonth(&CurDateStart);
			if(CurDateStart.wDay > outday)
			{
				CALE_DayIncrease(&CurDateStart, 1);
				CurDateStart.wHour = 0;
				CurDateStart.wMinute = 0;
			}

			DayInter = CALE_IntervalSysDate(&StartSys, &CurDateStart);
			if((DayInter < 0) || (DayInter >= MAX_WEEK_LINE))
			{
				continue;
			}
			pWeek->IconRow[DayInter] |= CALE_SCHED_ANNI;
			pWeek->bAnEv |= CALE_SCHED_ANNI;
		}//end if(CurMeetDate.LocalFlag == CALE_LOCAL)
	}
	CALE_CloseApi(iOpen);

	if((iOpen = CALE_OpenApi(CALE_EVENT)) == CALE_FAIL)
	{
		return FALSE;
	}
	memset(&CurEventDate, 0x0, sizeof(CaleEventNode));
	while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurEventDate, 
		sizeof(CaleEventNode))) != CALE_FAIL)
	{
		if(CurEventDate.LocalFlag == CALE_LOCAL)
		{
			CurDateStart.wYear = CurEventDate.EventStartYear;
			CurDateStart.wMonth = CurEventDate.EventStartMonth;
			CurDateStart.wDay = CurEventDate.EventStartDay;
			CurDateStart.wHour = CurEventDate.EventStartHour;
			CurDateStart.wMinute = CurEventDate.EventStartMinute;

			CurDateEnd.wYear = CurEventDate.EventEndYear;
			CurDateEnd.wMonth = CurEventDate.EventEndMonth;
			CurDateEnd.wDay = CurEventDate.EventEndDay;
			CurDateEnd.wHour = 23;
			CurDateEnd.wMinute = 59;
			
			CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
			if(CmpRes == Late)
			{
				continue;
			}
			CmpRes = CALE_CmpSysDate(&CurDateEnd, &StartSys);
			if(CmpRes == Early)
			{
				continue;
			}

			CmpRes = CALE_CmpSysDate(&CurDateStart, &StartSys);
			if(CmpRes != Late)
			{
				DayInter = 0;
				memcpy(&CurDateStart, &StartSys, sizeof(SYSTEMTIME));
			}
			else
			{
				DayInter = CALE_IntervalSysDate(&StartSys, &CurDateStart);
			}
			if((DayInter < 0) || (DayInter >= MAX_WEEK_LINE))
			{
				continue;
			}
			
			//CurDateStart.wYear = StartSys.wYear;
			//CurDateStart.wMonth = StartSys.wMonth;
			//CurDateStart.wDay = StartSys.wDay;

			for(i = DayInter; i < MAX_WEEK_LINE; i ++)
			{
				CmpRes = CALE_CmpSysDate(&CurDateStart, &CurDateEnd);
				if((CmpRes == Early) || (CmpRes ==Equality))
				{
					pWeek->IconRow[i] |= CALE_SCHED_EVENT;
					pWeek->bAnEv |= CALE_SCHED_EVENT;
					CALE_DayIncrease(&CurDateStart, 1);
				}
				else
				{
					break;
				}
			}
		}//end if(CurEventDate.LocalFlag == CALE_LOCAL)
	}
	CALE_CloseApi(iOpen);

	return TRUE;
}

int GetScrollPos(HWND hWnd, int nBar)
{
    memset(&GloScroll, 0x0, sizeof(SCROLLINFO));
    GloScroll.fMask = SIF_ALL;
    GloScroll.cbSize = sizeof(GloScroll);
    GetScrollInfo(hWnd, nBar, &GloScroll);
    
    return GloScroll.nPos;
}

int SetScrollPos(HWND hWnd, int nBar, int nPos, BOOL bRedraw)
{
    int tmp;

    tmp = GloScroll.nPos;
    GloScroll.cbSize = sizeof(GloScroll);
    GloScroll.fMask = SIF_POS;
    GloScroll.nPos = nPos;
    SetScrollInfo(hWnd, nBar, &GloScroll, bRedraw);

    return tmp;
}

void CALE_TimeSysToChar(SYSTEMTIME *pSys, char *pChar)
{
    char    szHour[10] = {'\0'};
    char    szMinute[10] = {'\0'};

    if((pSys->wHour -10) < 0)
        sprintf(szHour, "0%d", pSys->wHour);
    else
        sprintf(szHour, "%2d", pSys->wHour);
    
    if((pSys->wMinute -10) < 0)
        sprintf(szMinute, "0%d", pSys->wMinute);
    else
        sprintf(szMinute, "%2d", pSys->wMinute);

    sprintf(pChar, "%s:%s", szHour, szMinute);

    return;
}

void CALE_TimeCharToSys(char *pChar, SYSTEMTIME *pSys)
{
    pSys->wHour = (WORD)atoi(pChar);
    pSys->wMinute = (WORD)atoi(pChar + 3);
    
    return;
}
/*
void CALE_DateSysToChar(SYSTEMTIME *pSys, char *pChar)
{
    DATEFORMAT forDate = GetDateFormt();

	switch(forDate)
	{
	case DF_DMY_DOT:
		sprintf(pChar, "%02d.%02d.%04d", pSys->wDay, pSys->wMonth, pSys->wYear);
		break;

	case DF_DMY_SLD:
		sprintf(pChar, "%02d/%02d/%04d", pSys->wDay, pSys->wMonth, pSys->wYear);
		break;
		
	case DF_DMY_DSH:
		sprintf(pChar, "%02d-%02d-%04d", pSys->wDay, pSys->wMonth, pSys->wYear);
		break;

	case DF_MDY_DOT:
		sprintf(pChar, "%02d.%02d.%04d", pSys->wMonth,  pSys->wDay, pSys->wYear);
		break;

	case DF_MDY_SLD:
		sprintf(pChar, "%02d/%02d/%04d", pSys->wMonth,  pSys->wDay, pSys->wYear);
		break;
		
	case DF_MDY_DSH:
		sprintf(pChar, "%02d-%02d-%04d", pSys->wMonth,  pSys->wDay, pSys->wYear);
		break;

	default:
		sprintf(pChar, "%02d-%02d-%04d", pSys->wDay, pSys->wMonth, pSys->wYear);
		break;
	}
}
//*/

void CALE_DateSysToChar(SYSTEMTIME *pSys, char *pChar)
{
    char    szMonth[10] = {'\0'};
    char    szDay[10] = {'\0'};
    
    if((pSys->wMonth -10) < 0)
        sprintf(szMonth, "0%d", pSys->wMonth);
    else
        sprintf(szMonth, "%2d", pSys->wMonth);
    
    if((pSys->wDay -10) < 0)
        sprintf(szDay, "0%d", pSys->wDay);
    else
        sprintf(szDay, "%2d", pSys->wDay);
    
    sprintf(pChar, "%d-%s-%s", pSys->wYear, szMonth, szDay);
    
    return;
}

void CALE_DateCharToSys(char *pChar, SYSTEMTIME *pSys)
{
    pSys->wYear = (WORD)atoi(pChar);
    pSys->wMonth = (WORD)atoi(pChar + 5);
    pSys->wDay = (WORD)atoi(pChar + 8);
    
    return;
}

//* get date string with system setting format
BOOL CALE_GetFormatedDate(SYSTEMTIME *pSys, char *pChar)
{

    DATEFORMAT forDate = GetDateFormt();

	if(NULL == pSys || NULL == pChar)
		return FALSE;

	switch(forDate)
	{
	case DF_DMY_DOT:
		sprintf(pChar, "%02d.%02d.%04d", 
			pSys->wDay, pSys->wMonth, pSys->wYear);
		break;

	case DF_DMY_SLD:
		sprintf(pChar, "%02d/%02d/%04d", 
			pSys->wDay, pSys->wMonth, pSys->wYear);
		break;
		
	case DF_DMY_DSH:
		sprintf(pChar, "%02d-%02d-%04d", 
			pSys->wDay, pSys->wMonth, pSys->wYear);
		break;

	case DF_MDY_DOT:
		sprintf(pChar, "%02d.%02d.%04d",
			pSys->wMonth,  pSys->wDay, pSys->wYear);
		break;

	case DF_MDY_SLD:
		sprintf(pChar, "%02d/%02d/%04d",
			pSys->wMonth,  pSys->wDay, pSys->wYear);
		break;
		
	case DF_MDY_DSH:
		sprintf(pChar, "%02d-%02d-%04d",
			pSys->wMonth,  pSys->wDay, pSys->wYear);
		break;

	case DF_YMD_DOT:
		sprintf(pChar, "%04d.%02d.%02d", 
			pSys->wYear, pSys->wMonth, pSys->wDay);
		break;

	case DF_YMD_SLD:
		sprintf(pChar, "%04d/%02d/%02d", 
			pSys->wYear, pSys->wMonth, pSys->wDay);
		break;
		
	case DF_YMD_DSH:
		sprintf(pChar, "%04d-%02d-%02d", 
			pSys->wYear, pSys->wMonth, pSys->wDay);
		break;

	default:
		sprintf(pChar, "%02d.%02d.%04d", 
			pSys->wDay, pSys->wMonth, pSys->wYear);
		break;
	}
    
    return TRUE;
}
//*/

//* get time string with system setting format
BOOL CALE_GetFormatedTime(char *pBuf, int nHour, int nMinute)
{
	if(NULL == pBuf)
		return FALSE;

	if(nHour < 0 || nMinute < 0)
		return FALSE;
	if(nHour > 23 || nMinute > 59)
		return FALSE;
	
	if(TF_12 ==  GetTimeFormt())
	{
		if(nHour == 0)
			sprintf(pBuf, "12:%02d AM", nMinute);
		else if(nHour == 12)
			sprintf(pBuf, "12:%02d PM", nMinute);
		else if(nHour > 12)
		{
			nHour -= 12;
			sprintf(pBuf, "%02d:%02d PM", nHour, nMinute);
		}
		else
			sprintf(pBuf, "%02d:%02d AM", nHour, nMinute);
	}
	else
		sprintf(pBuf, "%02d:%02d", nHour, nMinute);

	return TRUE;
}
//*/
//* get date and time with system setting format
BOOL CALE_GetFormatedFullDate(SYSTEMTIME *pSys, char *pChar)
{
	if(!CALE_GetFormatedDate(pSys, pChar))
		return FALSE;

	pChar[10] = ' ';
	
	return CALE_GetFormatedTime(pChar+11, pSys->wHour, pSys->wMinute);
}
//*/
void CALE_DateTimeSysToChar(SYSTEMTIME *pSys, char *pChar)
{
    char    szMonth[10] = {'\0'};
    char    szDay[10] = {'\0'};
    char    szHour[10] = {'\0'};
    char    szMinute[10] = {'\0'};
    
    if((pSys->wMonth -10) < 0)
        sprintf(szMonth, "0%d", pSys->wMonth);
    else
        sprintf(szMonth, "%2d", pSys->wMonth);
    
    if((pSys->wDay -10) < 0)
        sprintf(szDay, "0%d", pSys->wDay);
    else
        sprintf(szDay, "%2d", pSys->wDay);

    if((pSys->wHour -10) < 0)
        sprintf(szHour, "0%d", pSys->wHour);
    else
        sprintf(szHour, "%2d", pSys->wHour);
    
    if((pSys->wMinute -10) < 0)
        sprintf(szMinute, "0%d", pSys->wMinute);
    else
        sprintf(szMinute, "%2d", pSys->wMinute);
    
    sprintf(pChar, "%s-%s-%04d %s:%s", szDay, szMonth, pSys->wYear, szHour, szMinute);
    
    return;
}

void CALE_DateTimeCharToSys(char *pChar, SYSTEMTIME *pSys)
{
    pSys->wDay = (WORD)atoi(pChar);
    pSys->wMonth = (WORD)atoi(pChar + 3);
    pSys->wYear = (WORD)atoi(pChar + 6);
    pSys->wHour = (WORD)atoi(pChar + 11);
    pSys->wMinute = (WORD)atoi(pChar + 14);
    
    return;
}

void CALE_MonthViewDisplay(SYSTEMTIME *pSys, char *pChar)
{
    sprintf(pChar, "%s %d", nMonthEnString[pSys->wMonth - 1], pSys->wYear);
    return;
}

void CALE_WeekViewDisplay(SYSTEMTIME *pSys, char *pChar)
{

    WORD wYear, wMonth, wDay;
    int nYearStart, nWeekNo;
    int nStart, nDays, nTempDays;
    BOOL bLunar = 0;
    int StartDate;
    int betWeek;

    SYSTEMTIME startsys;
    SYSTEMTIME endsys;

    StartDate = CALE_GetWeekStart();
    if(StartDate == START_MONDAY)
    {
        wYear = pSys->wYear;
        wMonth = pSys->wMonth;
        wDay = pSys->wDay;
        
        if((wYear % 4 == 0) && (wYear % 100 != 0) || (wYear % 400 == 0))
        {
            bLunar = 1;
        }
        
        if(wYear >= 1980)
        {
            nStart = (wYear - 1980) * 365 + (wYear - 1980) / 4 + 2 + 1;
            if((wYear % 4 == 0) && (wYear % 100 != 0) || (wYear % 400 == 0))
            {
                nStart -= 1;
            }
        }
        else
        {
            nStart = (1980 - wYear) * 365 + (1980 - wYear) / 4 + 2 + 1;
            if((wYear % 4 == 0) && (wYear % 100 != 0) || (wYear % 400 == 0))
            {
                nStart -= 1;
            }
        }
        nDays = Days[wMonth - 1];
        nYearStart = (bLunar ? nStart + 1 : nStart) % 7;
        
        if(wMonth >= 2)
        {
            nTempDays = Days[wMonth - 2];
			if(wMonth > 2 && bLunar)
				nTempDays ++;
        }
        else
        {
            nTempDays = 0;
        }
        nWeekNo = (nTempDays + (wDay-1) + (nYearStart+7-1) % 7) / 7 + 1;

        sprintf(pChar, "%s %d", IDP_IME_WEEK, nWeekNo);
    }
    else
    {        
        betWeek = pSys->wDayOfWeek - StartDate;
        if(betWeek < 0)
        {
            betWeek += 7;
        }
        memcpy(&startsys, pSys, sizeof(SYSTEMTIME));
        CALE_DayReduce(&startsys, betWeek);
        startsys.wDayOfWeek = StartDate;
        
        memcpy(&endsys, pSys, sizeof(SYSTEMTIME));
        CALE_DayIncrease(&endsys, (7 - betWeek - 1));
        if(StartDate == 0)
        {
            endsys.wDayOfWeek = 6;
        }
        else
        {
            endsys.wDayOfWeek = StartDate - 1;
        }
        
		CALE_GetFormatedDate(&startsys, pChar);
		CALE_GetFormatedDate(&endsys, (pChar+11));
		pChar[10] = '-';
    }

    return;
}

void CALE_DayViewDisplay(SYSTEMTIME *pSys, char *pChar)
{
    sprintf(pChar, "%s ", nWeekEnString[pSys->wDayOfWeek]);
	CALE_GetFormatedDate(pSys, (pChar+3));
    
    return;
}

BOOL CALE_DelSchedule(int DelFlag, SYSTEMTIME *pSys)
{
    int iOpen;
    DWORD nItemID;
    int CmpRes;

    SYSTEMTIME CurDateStart;
    SYSTEMTIME CurStartSys;

    CaleMeetingNode CurMeetDate;
    CaleAnniNode CurAnniDate;
    CaleEventNode CurEventDate;

    CurStartSys.wYear = pSys->wYear;
    CurStartSys.wMonth = pSys->wMonth;
    CurStartSys.wDay = pSys->wDay;
    CurStartSys.wHour = 0;
    CurStartSys.wMinute = 0;

    if((iOpen = CALE_OpenApi(CALE_MEETING)) == CALE_FAIL)
    {
        return FALSE;
    }
    memset(&CurMeetDate, 0x0, sizeof(CaleMeetingNode));
    while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurMeetDate, 
        sizeof(CaleMeetingNode))) != CALE_FAIL)
    {
        if(CurMeetDate.LocalFlag == CALE_LOCAL)
        {
            if(DelFlag == CALE_DEL_ALL)
            {
                if(CurMeetDate.MeetAlarmFlag)
                {
                    CALE_KillAlarm(CALE_MEETING, nItemID);
                }
                CurMeetDate.LocalFlag = CALE_UNLOCAL;
                CALE_DelApi(CALE_MEETING, iOpen, nItemID, (BYTE*)&CurMeetDate,
                    sizeof(CaleMeetingNode));
                
                continue;
            }

            CurDateStart.wYear = CurMeetDate.MeetStartYear;
            CurDateStart.wMonth = CurMeetDate.MeetStartMonth;
            CurDateStart.wDay = CurMeetDate.MeetStartDay;
            CurDateStart.wHour = CurMeetDate.MeetStartHour;
            CurDateStart.wMinute = CurMeetDate.MeetStartMinute;

            CmpRes = CALE_CmpOnlyDate(&CurDateStart, &CurStartSys);
            if((CmpRes == Early) && (DelFlag == CALE_DEL_BEFORE))
            {
                if(CurMeetDate.MeetAlarmFlag)
                {
                    CALE_KillAlarm(CALE_MEETING, nItemID);
                }
                CurMeetDate.LocalFlag = CALE_UNLOCAL;
                CALE_DelApi(CALE_MEETING, iOpen, nItemID, (BYTE*)&CurMeetDate,
                    sizeof(CaleMeetingNode));

                continue;
            }
            
            if((CmpRes == Late) && (DelFlag == CALE_DEL_AFTER))
            {
                if(CurMeetDate.MeetAlarmFlag)
                {
                    CALE_KillAlarm(CALE_MEETING, nItemID);
                }
                CurMeetDate.LocalFlag = CALE_UNLOCAL;
                CALE_DelApi(CALE_MEETING, iOpen, nItemID, (BYTE*)&CurMeetDate,
                    sizeof(CaleMeetingNode));
                
                continue;
            }
            if((CmpRes == Equality) && (DelFlag == CALE_DEL_NOW))
            {
                if(CurMeetDate.MeetAlarmFlag)
                {
                    CALE_KillAlarm(CALE_MEETING, nItemID);
                }
                CurMeetDate.LocalFlag = CALE_UNLOCAL;
                CALE_DelApi(CALE_MEETING, iOpen, nItemID, (BYTE*)&CurMeetDate,
                    sizeof(CaleMeetingNode));
                
                continue;
            }
        }
    }
    CALE_CloseApi(iOpen);
    
    if((iOpen = CALE_OpenApi(CALE_ANNI)) == CALE_FAIL)
    {
        return FALSE;
    }
    memset(&CurAnniDate, 0x0, sizeof(CaleAnniNode));
    while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurAnniDate, 
        sizeof(CaleAnniNode))) != CALE_FAIL)
    {
        if(CurAnniDate.LocalFlag == CALE_LOCAL)
        {
            if(DelFlag == CALE_DEL_ALL)
            {
                if(CurAnniDate.AnniAlarmFlag)
                {
                    CALE_KillAlarm(CALE_ANNI, nItemID);
                }
                CurAnniDate.LocalFlag = CALE_UNLOCAL;
                CALE_DelApi(CALE_ANNI, iOpen, nItemID, (BYTE*)&CurAnniDate,
                    sizeof(CaleAnniNode));
                
                continue;
            }
            
            CurDateStart.wYear = CurAnniDate.AnniDisYear;
            CurDateStart.wMonth = CurAnniDate.AnniDisMonth;
            CurDateStart.wDay = CurAnniDate.AnniDisDay;
            CurDateStart.wHour = 8;
            CurDateStart.wMinute = 0;
            
            CmpRes = CALE_CmpOnlyDate(&CurDateStart, &CurStartSys);
            if((CmpRes == Early) && (DelFlag == CALE_DEL_BEFORE))
            {
                if(CurAnniDate.AnniAlarmFlag)
                {
                    CALE_KillAlarm(CALE_ANNI, nItemID);
                }
                CurAnniDate.LocalFlag = CALE_UNLOCAL;
                CALE_DelApi(CALE_ANNI, iOpen, nItemID, (BYTE*)&CurAnniDate,
                    sizeof(CaleAnniNode));
                
                continue;
            }
            if((CmpRes == Late) && (DelFlag == CALE_DEL_AFTER))
            {
                if(CurAnniDate.AnniAlarmFlag)
                {
                    CALE_KillAlarm(CALE_ANNI, nItemID);
                }
                CurAnniDate.LocalFlag = CALE_UNLOCAL;
                CALE_DelApi(CALE_ANNI, iOpen, nItemID, (BYTE*)&CurAnniDate,
                    sizeof(CaleAnniNode));
                
                continue;
            }
            if((CmpRes == Equality) && (DelFlag == CALE_DEL_NOW))
            {
                if(CurAnniDate.AnniAlarmFlag)
                {
                    CALE_KillAlarm(CALE_ANNI, nItemID);
                }
                CurAnniDate.LocalFlag = CALE_UNLOCAL;
                CALE_DelApi(CALE_ANNI, iOpen, nItemID, (BYTE*)&CurAnniDate,
                    sizeof(CaleAnniNode));
                
                continue;
            }
        }
    }
    CALE_CloseApi(iOpen);
    
    if((iOpen = CALE_OpenApi(CALE_EVENT)) == CALE_FAIL)
    {
        return FALSE;
    }
    memset(&CurEventDate, 0x0, sizeof(CaleEventNode));
    while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurEventDate, 
        sizeof(CaleEventNode))) != CALE_FAIL)
    {
        if(CurEventDate.LocalFlag == CALE_LOCAL)
        {
            if(DelFlag == CALE_DEL_ALL)
            {
                CurEventDate.LocalFlag = CALE_UNLOCAL;
                CALE_DelApi(CALE_EVENT, iOpen, nItemID, (BYTE*)&CurEventDate,
                    sizeof(CaleEventNode));
                
                continue;
            }
            
            CurDateStart.wYear = CurEventDate.EventStartYear;
            CurDateStart.wMonth = CurEventDate.EventStartMonth;
            CurDateStart.wDay = CurEventDate.EventStartDay;
            CurDateStart.wHour = CurEventDate.EventStartHour;
            CurDateStart.wMinute = CurEventDate.EventStartMinute;
            
            CmpRes = CALE_CmpOnlyDate(&CurDateStart, &CurStartSys);
            if((CmpRes == Early) && (DelFlag == CALE_DEL_BEFORE))
            {
                CurEventDate.LocalFlag = CALE_UNLOCAL;
                CALE_DelApi(CALE_EVENT, iOpen, nItemID, (BYTE*)&CurEventDate,
                    sizeof(CaleEventNode));
                
                continue;
            }
            if((CmpRes == Late) && (DelFlag == CALE_DEL_AFTER))
            {
                CurEventDate.LocalFlag = CALE_UNLOCAL;
                CALE_DelApi(CALE_EVENT, iOpen, nItemID, (BYTE*)&CurEventDate,
                    sizeof(CaleEventNode));
                
                continue;
            }
            if((CmpRes == Equality) && (DelFlag == CALE_DEL_NOW))
            {
                CurEventDate.LocalFlag = CALE_UNLOCAL;
                CALE_DelApi(CALE_EVENT, iOpen, nItemID, (BYTE*)&CurEventDate,
                    sizeof(CaleEventNode));
                
                continue;
            }
        }
    }
    CALE_CloseApi(iOpen);

    return TRUE;
}

void CALE_ReleaseWeekSchNode(CaleWeekSchHead *pHead)
{
    CaleWeekSchNode *pTemp;
    
    pTemp = pHead->pNext;
    while (pTemp)
    {
        pHead->pNext = pTemp->pNext;
        free(pTemp);
        pTemp = NULL;
        pTemp = pHead->pNext;
    }
    pHead->Num = 0;
    pHead->pNext = NULL;
    
    return;
}

static void CALE_AddWeekSchNode(CaleWeekSchHead *pHead, CaleWeekSchNode *pTmpNode)
{
    CaleWeekSchNode *pTemp;
    
    pTmpNode->pNext = NULL;

    if(pHead->pNext == NULL)
    {
        pHead->pNext = pTmpNode;
    }
    else
    {
        pTemp = pHead->pNext;
        while (pTemp->pNext)
        {
            pTemp = pTemp->pNext;
        }
        pTmpNode->pNext = NULL;
        pTemp->pNext = pTmpNode;
    }
    pHead->Num ++;
    
    return;
}

static void CALE_DelWeekSchNode(CaleWeekSchHead *pHead, int iTmp)
{
    CaleWeekSchNode *pTemp;
    CaleWeekSchNode *pPre;
    int i;
    
    if(pHead->pNext == NULL)
        return;
    
    pTemp = pHead->pNext;
    if(iTmp == 0)
    {
        pHead->pNext = pTemp->pNext;
        free(pTemp);
        pTemp = NULL;
    }
    else
    {
        for(i = 1; i < iTmp; i ++)
        {
            pPre = pTemp;
            pTemp = pPre->pNext;
        }
        pPre->pNext = pTemp->pNext;
        free(pTemp);
        pTemp = NULL;
    }
    pHead->Num --;
    
    return;
}

BOOL CALE_GetWeekSchByInt(CaleWeekSchHead *pHead, CaleWeekSchNode **pTmpNode, int iTmp)
{
    int i;
    CaleWeekSchNode *pTemp;
    
    if(iTmp > (pHead->Num - 1))
        return FALSE;
    
    if(pHead->pNext == NULL)
        return FALSE;
    
    pTemp = pHead->pNext;
    for(i = 0; i < iTmp; i ++)
    {
        pTemp = pTemp->pNext;
    }
    *pTmpNode = pTemp;
    
    return TRUE;
}

void CALE_FormatDisplay(HWND hWnd, char *szDisplay, char *szString)
{
    HDC hdc;
    int nLen, nFit = 0;
    
    hdc = GetDC(hWnd);
    
    nLen = strlen(szString);
    GetTextExtentExPoint(hdc, szString, nLen, CALE_LIST_WIDTH, &nFit, NULL, NULL);
    
    if(nFit == nLen)//add space
    {
        strcpy(szDisplay, szString);
    }
    else
    {
        strncpy(szDisplay, szString, nFit);
    }
    
    ReleaseDC(hWnd, hdc);
    return;
}

//*
static long StatFolder(PCSTR pFolderPath)
{
	struct dirent *dirinfo = NULL;
	struct stat	filestat;

	long nRtnSize;	
	char FullName[128];
	
	DIR  *dirtemp = NULL;

	if (NULL == pFolderPath)
	{
		return 0;
	}

    nRtnSize = 0;

	dirtemp = opendir(pFolderPath);

	if(dirtemp == NULL)
		return 0;
	

	while((dirinfo = readdir(dirtemp))!=NULL)
	{
		memset(FullName, 0, 128);
		strcpy ( FullName, pFolderPath );
		if (FullName[strlen(FullName) - 1] != '/')
			strcat(FullName, "/");

		strcat ( FullName, dirinfo->d_name);
		stat(FullName,&filestat);

		if((strcmp(dirinfo->d_name,".")==0) || (strcmp(dirinfo->d_name,"..")==0))
			continue;
		else if ( (!S_ISLNK(filestat.st_mode)) && S_ISDIR(filestat.st_mode) )	// sub folder
		{			
			nRtnSize += StatFolder(FullName);
		}
		else if(S_ISREG(filestat.st_mode))	// common file
			nRtnSize += filestat.st_size;

		KickDog();	// clear the watch dog
	//	GetMessage(&msg, NULL, 0, 0))
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	}
	closedir(dirtemp);
	return nRtnSize;
}

long CALE_StatFlashFolder()
{
	long lRet = StatFolder(CALE_FILE_PATH);
	chdir(CALE_FILE_PATH);
	return lRet;
}

void CALE_SetSystemIcons(HWND hFrame)
{
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, CALE_LEFTICON), NULL);
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, CALE_RIGHTICON), NULL);
}

int CALE_GetDaysOfMonth(int year, int month)
{
	switch(month)
	{
	case 2:
		if(CALE_IsLeapYear((WORD)year))
			return 29;
		else
			return 28;

	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;

	default:
		return 30;

	}
}

BOOL CALE_CheckDate(SYSTEMTIME *pDate)
{
	if(NULL == pDate)
		return FALSE;

	if(pDate->wDay > CALE_GetDaysOfMonth(pDate->wYear, pDate->wMonth))
		return FALSE;

	if(pDate->wDay < 1)
		return FALSE;

	if(pDate->wYear > MAX_YEAR || pDate->wYear < MIN_YEAR)
		return FALSE;

	if(pDate->wMonth < 1 || pDate->wMonth > 12)
		return FALSE;

	return TRUE;
}

BOOL CALE_GetUTCTime(SYSTEMTIME *pTime, PCSTR szTimeZone, BOOL bAdd)
{
	int  dhour = 0;
	int  dmin = 0;
	char *p = NULL;

	if(szTimeZone && (p = strchr(szTimeZone, ':')))
	{
		ASSERT(strlen(szTimeZone) >= 6);
		ASSERT(p - szTimeZone >= 3);
		p -= 3;
		
		if('-' == *p)
			bAdd = !bAdd;
		
		dhour = atoi(p+1);
		dmin = atoi(p+4);
		
		ASSERT(dhour <= 13);
		ASSERT(dmin < 60);
	}
	
	if(bAdd)
	{
		pTime->wMinute += dmin;
		while(pTime->wMinute >= 60)
		{
			pTime->wMinute -= 60;
			pTime->wHour ++;
		}
		pTime->wHour += dhour;
		while(pTime->wHour >= 24)
		{
			pTime->wHour -= 24;
			CALE_DayIncrease(pTime, 1);
		}
	}
	else
	{
		int hour, minute;
		
		hour = pTime->wHour;
		minute = pTime->wMinute;

		minute -= dmin;
		while(minute < 0)
		{
			minute += 60;
			hour --;
		}

		hour -= dhour;
		while(hour < 0)
		{
			hour += 24;
			CALE_DayReduce(pTime, 1);
		}
		
		pTime->wMinute = minute;
		pTime->wHour = hour;
	}
	return TRUE;
}
