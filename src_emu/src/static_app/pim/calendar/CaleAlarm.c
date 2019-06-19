/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleAlarm.c
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleAlarm.h"
/*
#ifndef _SNOOZE_BY_TIMER_
#define _SNOOZE_BY_TIMER_	// snooze by timer
#endif
//*/
#undef	_SNOOZE_BY_TIMER_	// snooze by system alarm

//*
#ifndef _REPEAT_AFTER_END_
#define _REPEAT_AFTER_END_	// end date doesn't mean end of repeat
#endif
//*/
//#undef	_REPEAT_AFTER_END_	// no repeat after end date

#define CALE_ALARM_TIMER(n) (100000*((n)->CaleType)+((n)->iSaveIndex))

static void CALE_AlarmSystemOn(BOOL bOn);
static BOOL CALE_GetMeetAlmNode(CaleMeetingNode *CurMeetDate, CaleAlarmNode *pNode, DWORD nItemID);
static BOOL CALE_GetAnniAlmNode(CaleAnniNode *CurAnniDate, CaleAlarmNode *pNode, DWORD nItemID);

static HWND hCaleAlarm = NULL;
static BOOL bChangedLock = FALSE;

BOOL CALE_RegisterAlarm(int nFlag)
{
	DWORD			nItemID;
	CaleMeetingNode CurMeetDate;
	CaleAnniNode	CurAnniDate;
	CaleAlarmNode	AlmNode;
	CaleAlarmNode	*pFirstAlm = NULL;
	SYSTEMTIME		AlmTime;
	SYSTEMTIME		tmpTime;
	
	WORD			dwFirstID = 0;
	int				iOpen = 0;
	
	//////////////////////////////////////////////////////////////////////////
	///////////////// Get the first alarm to be set //////////////////////////
	
	AlmTime.wYear = MAX_YEAR;
	AlmTime.wMonth = 12;
	AlmTime.wDay = 31;
	AlmTime.wHour = 23;
	AlmTime.wMinute = 59;
	AlmTime.wSecond = 59;

	tmpTime.wSecond = 0;

	//meeting alarm
	if((iOpen = CALE_OpenApi(CALE_MEETING)) == CALE_FAIL)
	{
		return FALSE;
	}
	memset(&CurMeetDate, 0x0, sizeof(CaleMeetingNode));
	while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurMeetDate, 
			sizeof(CaleMeetingNode))) != CALE_FAIL)
	{
		if(CALE_GetMeetAlmNode(&CurMeetDate, &AlmNode, nItemID))
		{
			tmpTime.wYear = AlmNode.wAlmYear;
			tmpTime.wMonth = AlmNode.bAlmMonth;
			tmpTime.wDay = AlmNode.bAlmDay;
			tmpTime.wHour = AlmNode.bAlmHour;
			tmpTime.wMinute = AlmNode.bAlmMinute;
			if(Early == CALE_CmpSysDate(&tmpTime, &AlmTime))
			{
				memcpy(&AlmTime, &tmpTime, sizeof(SYSTEMTIME));
				if(NULL == pFirstAlm)
				{
					pFirstAlm = (CaleAlarmNode*)malloc(sizeof(CaleAlarmNode));
					if(NULL == pFirstAlm)
						return FALSE;
				}
				memcpy(pFirstAlm, &AlmNode, sizeof(CaleAlarmNode));
			}
		}
	}//end while (nItemID = CALE_ReadApi != CALE_FAIL)
	CALE_CloseApi(iOpen);

	//anniversary alarm
	if((iOpen = CALE_OpenApi(CALE_ANNI)) == CALE_FAIL)
	{
		return FALSE;
	}
	memset(&CurAnniDate, 0x0, sizeof(CaleAnniNode));
	while ((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurAnniDate, 
		sizeof(CaleAnniNode))) != CALE_FAIL)
	{
		if(CALE_GetAnniAlmNode(&CurAnniDate, &AlmNode, nItemID))
		{
			tmpTime.wYear = AlmNode.wAlmYear;
			tmpTime.wMonth = AlmNode.bAlmMonth;
			tmpTime.wDay = AlmNode.bAlmDay;
			tmpTime.wHour = AlmNode.bAlmHour;
			tmpTime.wMinute = AlmNode.bAlmMinute;
			if(Early == CALE_CmpSysDate(&tmpTime, &AlmTime))
			{
				memcpy(&AlmTime, &tmpTime, sizeof(SYSTEMTIME));
				if(NULL == pFirstAlm)
				{
					pFirstAlm = (CaleAlarmNode*)malloc(sizeof(CaleAlarmNode));
					if(NULL == pFirstAlm)
						return FALSE;
				}
				memcpy(pFirstAlm, &AlmNode, sizeof(CaleAlarmNode));
			}
		}
	}
	CALE_CloseApi(iOpen);


	//////////////////////////////////////////////////////////////////////////
	///////////////// Set alarms to MC55 or RTC //////////////////////////////
	
	if(NULL != pFirstAlm)
	{
		CALE_ResetAlarm(pFirstAlm, nFlag);
		dwFirstID = (WORD)(pFirstAlm->CaleType);
		dwFirstID = (dwFirstID << 14) | (pFirstAlm->iSaveIndex & 0x3F);
		free(pFirstAlm);
		pFirstAlm = NULL;
	}

	//meeting alarm
	if((iOpen = CALE_OpenApi(CALE_MEETING)) == CALE_FAIL)
	{
		return FALSE;
	}
	memset(&CurMeetDate, 0x0, sizeof(CaleMeetingNode));
	while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurMeetDate, 
			sizeof(CaleMeetingNode))) != CALE_FAIL)
	{
		if(((CALE_MEETING << 14) | (nItemID & 0X3F)) != dwFirstID)
			CALE_NewMeet(&CurMeetDate, nItemID, nFlag);
	}//end while (nItemID = CALE_ReadApi != CALE_FAIL)
	CALE_CloseApi(iOpen);

	//anniversary alarm
	if((iOpen = CALE_OpenApi(CALE_ANNI)) == CALE_FAIL)
	{
		return FALSE;
	}
	memset(&CurAnniDate, 0x0, sizeof(CaleAnniNode));
	while ((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurAnniDate, 
		sizeof(CaleAnniNode))) != CALE_FAIL)
	{
		if(((CALE_ANNI << 14) | (nItemID & 0X3F)) != dwFirstID)
			CALE_NewAnni(&CurAnniDate, nItemID, nFlag);
	}
	CALE_CloseApi(iOpen);

	return TRUE;
}

BOOL CALE_CleanAlarm(void)
{
    DWORD nItemID;
    CaleMeetingNode CurMeetDate;
    CaleAnniNode CurAnniDate;
    int iOpen = 0;
    
    //meeting alarm
    if((iOpen = CALE_OpenApi(CALE_MEETING)) == CALE_FAIL)
    {
        return FALSE;
    }
    memset(&CurMeetDate, 0x0, sizeof(CaleMeetingNode));
    while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurMeetDate, 
            sizeof(CaleMeetingNode))) != CALE_FAIL)
    {
		CALE_KillAlarm(CALE_MEETING, nItemID);
    }//end while (nItemID = CALE_ReadApi != CALE_FAIL)
    CALE_CloseApi(iOpen);

    //anniversary alarm
    if((iOpen = CALE_OpenApi(CALE_ANNI)) == CALE_FAIL)
    {
        return FALSE;
    }
    memset(&CurAnniDate, 0x0, sizeof(CaleAnniNode));
    while ((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurAnniDate, 
        sizeof(CaleAnniNode))) != CALE_FAIL)
    {
		CALE_KillAlarm(CALE_ANNI, nItemID);
    }
    CALE_CloseApi(iOpen);

    return TRUE;
}


static BOOL CALE_GetMeetAlmNode(CaleMeetingNode *CurMeetDate, CaleAlarmNode *pNode, DWORD nItemID)
{
	int nCmpDateResult;
	int CmpRes;
	int outday;
	SYSTEMTIME Systime;
	SYSTEMTIME NodeAlmDate;

	if((CurMeetDate->LocalFlag == CALE_LOCAL) && CurMeetDate->MeetAlarmFlag)
	{
		CALE_MeetToAlmNode(CurMeetDate, pNode, nItemID);

		//compare whether the alarm time is earlier than local time
		GetLocalTime(&Systime);
		NodeAlmDate.wYear = (WORD)pNode->wAlmYear;
		NodeAlmDate.wMonth = (WORD)pNode->bAlmMonth;
		NodeAlmDate.wDay = (WORD)pNode->bAlmDay;
		NodeAlmDate.wHour = (WORD)pNode->bAlmHour;
		NodeAlmDate.wMinute = (WORD)pNode->bAlmMinute;

		nCmpDateResult = CALE_CmpSysDate(&Systime, &NodeAlmDate);
		if((nCmpDateResult == Late) || (nCmpDateResult == Equality))
		{
			if(CurMeetDate->MeetRepeatFlag == 0)
			{
				return FALSE;
			}
			else//(CurMeetDate->MeetRepeatFlag == 1)
			{
#ifndef _REPEAT_AFTER_END_
				SYSTEMTIME DeadDate;

				DeadDate.wYear	= (WORD)pNode->RepEndDate.bYear;
				DeadDate.wMonth = (WORD)pNode->RepEndDate.bMonth;
				DeadDate.wDay	= (WORD)pNode->RepEndDate.bDay;
				DeadDate.wHour	= (WORD)pNode->RepEndDate.bHour;
				DeadDate.wMinute = (WORD)pNode->RepEndDate.bMinute;				   
#endif
				switch(CurMeetDate->MeetRepeatStyle)
				{
				case CALE_REP_NONE:
					return FALSE;

				case CALE_REP_DAY:
					{
						while(1)
						{
							CmpRes = CALE_DayIncrease(&NodeAlmDate, 1);
							if(CmpRes == FALSE)
							{
								return FALSE;
							}
							if((CALE_CmpSysDate(&Systime, &NodeAlmDate)) == Early)
							{
								break;
							}
						}

#ifndef _REPEAT_AFTER_END_
						nCmpDateResult = CALE_CmpSysDate(&NodeAlmDate, &DeadDate);
						if(nCmpDateResult != Early)
							return FALSE;
						else
#endif
						{
							pNode->wAlmYear = (WORD)NodeAlmDate.wYear;
							pNode->bAlmMonth = (BYTE)NodeAlmDate.wMonth;
							pNode->bAlmDay = (BYTE)NodeAlmDate.wDay;

							return TRUE;

						}
					}
					break;

				case CALE_REP_WEEK:
					{
						while(1)
						{
							CmpRes = CALE_DayIncrease(&NodeAlmDate, 7);
							if(CmpRes == FALSE)
							{
								return FALSE;
							}
							if((CALE_CmpSysDate(&Systime, &NodeAlmDate)) == Early)
							{
								break;
							}
						}

#ifndef _REPEAT_AFTER_END_
						nCmpDateResult = CALE_CmpSysDate(&NodeAlmDate, &DeadDate);
						if(nCmpDateResult != Early)
							return FALSE;
						else
#endif
						{
							pNode->wAlmYear = (WORD)NodeAlmDate.wYear;
							pNode->bAlmMonth = (BYTE)NodeAlmDate.wMonth;
							pNode->bAlmDay = (BYTE)NodeAlmDate.wDay;
						
							return TRUE;
						}
					}
					break;

				case CALE_REP_TWOWEEK:
					{
						while (1)
						{
							CmpRes = CALE_DayIncrease(&NodeAlmDate, 14);
							if(CmpRes == FALSE)
							{
								return FALSE;
							}
							if((CALE_CmpSysDate(&Systime, &NodeAlmDate)) == Early)
							{
								break;
							}
						}

#ifndef _REPEAT_AFTER_END_
						nCmpDateResult = CALE_CmpSysDate(&NodeAlmDate, &DeadDate);
						if(nCmpDateResult != Early)
						{
							return FALSE;
						}
						else
#endif
						{
							pNode->wAlmYear = (WORD)NodeAlmDate.wYear;
							pNode->bAlmMonth = (BYTE)NodeAlmDate.wMonth;
							pNode->bAlmDay = (BYTE)NodeAlmDate.wDay;
						
							return TRUE;
						}
					}
					break;

				case CALE_REP_MONTH:
					{
						while(1)
						{
							CmpRes = CALE_MonthIncrease(&NodeAlmDate);
							if(CmpRes == FALSE)
							{
								break;
							}
							if((CALE_CmpSysDate(&Systime, &NodeAlmDate)) == Early)
							{
								break;
							}
						}
						if(CmpRes == FALSE)
						{
							return FALSE;
						}
						
						//exceed days of current month
						outday = CALE_AlmDaysOfMonth(&NodeAlmDate);
						if(NodeAlmDate.wDay > outday)
						{
							NodeAlmDate.wDay = outday;
							NodeAlmDate.wHour = 23;
							NodeAlmDate.wMinute = 59;
						}
						
#ifndef _REPEAT_AFTER_END_
						nCmpDateResult = CALE_CmpSysDate(&NodeAlmDate, &DeadDate);
						if(nCmpDateResult != Early)
						{
							return FALSE;
						}
						else
#endif
						{
							pNode->wAlmYear = (WORD)NodeAlmDate.wYear;
							pNode->bAlmMonth = (BYTE)NodeAlmDate.wMonth;
							pNode->bAlmDay = (BYTE)NodeAlmDate.wDay;
						
							return TRUE;
						}
					}
					break;
				
				case CALE_REP_YEAR:
					{
						while(1)
						{
							CmpRes = CALE_YearIncrease(&NodeAlmDate);
							if(CmpRes == FALSE)
							{
								break;
							}
							if((CALE_CmpSysDate(&Systime, &NodeAlmDate)) == Early)
							{
								break;
							}
						}
						if(CmpRes == FALSE)
						{
							return FALSE;
						}

						//exceed days of current month
						outday = CALE_AlmDaysOfMonth(&NodeAlmDate);
						if(NodeAlmDate.wDay > outday)
						{
							NodeAlmDate.wDay = outday;
							NodeAlmDate.wHour = 23;
							NodeAlmDate.wMinute = 59;
						}

#ifndef _REPEAT_AFTER_END_
						nCmpDateResult = CALE_CmpSysDate(&NodeAlmDate, &DeadDate);
						if(nCmpDateResult == Early)
						{
							return FALSE;
						}
						else
#endif
						{
							pNode->wAlmYear = (WORD)NodeAlmDate.wYear;
							pNode->bAlmMonth = (BYTE)NodeAlmDate.wMonth;
							pNode->bAlmDay = (BYTE)NodeAlmDate.wDay;
						
							return TRUE;
						}
					}
					break;

				default:
					break;
				}//end switch(CurMeetDate->MeetRepeatStyle)
			}//end else(CurMeetDate->MeetRepeatFlag == 1)
		}//end if((nCmpDateResult == Late) || (nCmpDateResult == Equality))
		else if(nCmpDateResult == Early)
		{
#ifndef _REPEAT_AFTER_END_
			SYSTEMTIME DeadDate;
			
			DeadDate.wYear	= (WORD)pNode->RepEndDate.bYear;
			DeadDate.wMonth = (WORD)pNode->RepEndDate.bMonth;
			DeadDate.wDay	= (WORD)pNode->RepEndDate.bDay;
			DeadDate.wHour	= (WORD)pNode->RepEndDate.bHour;
			DeadDate.wMinute = (WORD)pNode->RepEndDate.bMinute;

			nCmpDateResult = CALE_CmpSysDate(&NodeAlmDate, &DeadDate);
			
			return (nCmpDateResult == Early);
#else
			return TRUE;
#endif
		}
	}//end if((CurMeetDate->LocalFlag == CALE_LOCAL) && CurMeetDate->MeetAlarmFlag)
	else
	{
		return FALSE;
	}

	return TRUE;
}

static BOOL CALE_GetAnniAlmNode(CaleAnniNode *CurAnniDate, CaleAlarmNode *pNode, DWORD nItemID)
{
	int nCmpDateResult;
	int CmpRes;
	int outday;

	SYSTEMTIME Systime;
	SYSTEMTIME NodeAlmDate;
	
	if((CurAnniDate->LocalFlag == CALE_LOCAL) && CurAnniDate->AnniAlarmFlag)
	{
		CALE_AnniToAlmNode(CurAnniDate, pNode, nItemID);
		
		//compare whether the alarm time is earlier than local time
		GetLocalTime(&Systime);
		NodeAlmDate.wYear = (WORD)pNode->wAlmYear;
		NodeAlmDate.wMonth = (WORD)pNode->bAlmMonth;
		NodeAlmDate.wDay = (WORD)pNode->bAlmDay;
		NodeAlmDate.wHour = (WORD)pNode->bAlmHour;
		NodeAlmDate.wMinute = (WORD)pNode->bAlmMinute;
		
		nCmpDateResult = CALE_CmpSysDate(&Systime, &NodeAlmDate);
		if((nCmpDateResult == Late) || (nCmpDateResult == Equality))
		{
			while(1)
			{
				CmpRes = CALE_YearIncrease(&NodeAlmDate);
				if(CmpRes == FALSE)
				{
					return FALSE;
				}
				if((CALE_CmpSysDate(&Systime, &NodeAlmDate)) == Early)
				{
					break;
				}
			}

			//exceed days of current month
			outday = CALE_AlmDaysOfMonth(&NodeAlmDate);
			if(NodeAlmDate.wDay > outday)
			{
				NodeAlmDate.wDay = outday;
			}

			pNode->wAlmYear = (WORD)NodeAlmDate.wYear;
			pNode->bAlmMonth = (BYTE)NodeAlmDate.wMonth;
			pNode->bAlmDay = (BYTE)NodeAlmDate.wDay;
			
			return TRUE;
		}
		else if(nCmpDateResult == Early)
		{
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CALE_NewMeet(CaleMeetingNode *CurMeetDate, DWORD nItemID, int nFlag)
{
    CaleAlarmNode Node;

	if(!CALE_GetMeetAlmNode(CurMeetDate, &Node, nItemID))
	{
		CALE_KillAlarm(CALE_MEETING, nItemID);
		return FALSE;
	}
	else
	{
		if(!CALE_ResetAlarm(&Node, nFlag))
		{
			CALE_Print("\r\n[calendar]: anniversary alarm set failed(1).\r\n");
			
			PLXTipsWin(NULL, NULL, 0, ML("Set alarm failed!"), ML("Calendar"), 
				Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
			
			return FALSE;
		}
		return TRUE;
	}
}

BOOL CALE_NewAnni(CaleAnniNode *CurAnniDate, DWORD nItemID, int nFlag)
{
    CaleAlarmNode Node;
    
	if(!CALE_GetAnniAlmNode(CurAnniDate, &Node, nItemID))
	{
		CALE_KillAlarm(CALE_ANNI, nItemID);
		return FALSE;
	}
	else
	{
		if(!CALE_ResetAlarm(&Node, nFlag))
		{
			CALE_Print("\r\n[calendar]: anniversary alarm set failed(1).\r\n");
			
			PLXTipsWin(NULL, NULL, 0, ML("Set alarm failed!"), ML("Calendar"), 
				Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);

			return FALSE;
		}
		return TRUE;
	}	
}

void CALE_MeetToAlmNode(CaleMeetingNode *pCurCale, CaleAlarmNode *pNode, DWORD dwSaveIndex)
{
    pNode->CaleType         = CALE_MEETING;
    pNode->iSaveIndex       = dwSaveIndex;
    
    pNode->wCaldYear        = pCurCale->MeetStartYear;
    pNode->bCaldMonth       = pCurCale->MeetStartMonth;
    pNode->bCaldDay         = pCurCale->MeetStartDay;
    pNode->bCaldHour        = pCurCale->MeetStartHour;
    pNode->bCaldMinute      = pCurCale->MeetStartMinute;

    pNode->wAlmYear         = pCurCale->MeetStartYear;
    pNode->bAlmMonth        = pCurCale->MeetStartMonth;
    pNode->bAlmDay          = pCurCale->MeetStartDay;
    pNode->bAlmHour         = pCurCale->MeetStartHour;
    pNode->bAlmMinute       = pCurCale->MeetStartMinute;

    pNode->bCaldWeek        = pCurCale->MeetStartWeek;
    
    if(pCurCale->MeetAlarmFlag == CALE_BEFORE_5MIN)
    {
        CALE_CalcAlarmNode(pNode, 0, 0, 5);
    }
    else if(pCurCale->MeetAlarmFlag == CALE_BEFORE_10MIN)
    {
        CALE_CalcAlarmNode(pNode, 0, 0, 10);
    }
    else if(pCurCale->MeetAlarmFlag == CALE_BEFORE_15MIN)
    {
        CALE_CalcAlarmNode(pNode, 0, 0, 15);
    }
    else if(pCurCale->MeetAlarmFlag == CALE_BEFORE_30MIN)
    {
        CALE_CalcAlarmNode(pNode, 0, 0, 30);
    }
    else if(pCurCale->MeetAlarmFlag == CALE_BEFORE_45MIN)
    {
        CALE_CalcAlarmNode(pNode, 0, 0, 45);
    }
    else if(pCurCale->MeetAlarmFlag == CALE_BEFORE_1HOUR)
    {
        CALE_CalcAlarmNode(pNode, 0, 1, 0);
    }
    else if(pCurCale->MeetAlarmFlag == CALE_BEFORE_6HOUR)
    {
        CALE_CalcAlarmNode(pNode, 0, 6, 0);
    }
    else if(pCurCale->MeetAlarmFlag == CALE_BEFORE_12HOUR)
    {
        CALE_CalcAlarmNode(pNode, 0, 12, 0);
    }
    else if(pCurCale->MeetAlarmFlag == CALE_BEFORE_1DAY)
    {
        CALE_CalcAlarmNode(pNode, 1, 0, 0);
    }
    else if(pCurCale->MeetAlarmFlag == CALE_BEFORE_1WEEK)
    {
        CALE_CalcAlarmNode(pNode, 7, 0, 0);
    }

    if(pCurCale->MeetRepeatFlag == 1)
    {
        pNode->bRepStyle    = pCurCale->MeetRepeatStyle;
    }
    else
    {
        pNode->bRepStyle    = 0;
    }
    pNode->bRepMask         = 0;

    pNode->RepEndDate.bYear = pCurCale->MeetEndYear;
    pNode->RepEndDate.bMonth = pCurCale->MeetEndMonth;
    pNode->RepEndDate.bDay  = pCurCale->MeetEndDay;
    pNode->RepEndDate.bHour = pCurCale->MeetEndHour;
    pNode->RepEndDate.bMinute = pCurCale->MeetEndMinute;

    pNode->pNext = NULL;

    memset(pNode->CaldSubject, 0x0, sizeof(MAX_CALE_SUBJECT_LEN) + 1);
    strcpy(pNode->CaldSubject, pCurCale->MeetSubject);
    memset(pNode->CaldLocation, 0x0, sizeof(MAX_CALE_LOCATION_LEN) + 1);
    strcpy(pNode->CaldLocation, pCurCale->MeetLocation);
}

void CALE_CalcAlarmNode(CaleAlarmNode *pNode, int iDay, int iHour, int iMin)
{
    SYSTEMTIME OldTime;

	int nMin, nHour;

//    OldTime.wMinute = (WORD)((int)pNode->bAlmMinute - iMin);
//    OldTime.wHour = (WORD)((int)pNode->bAlmHour - iHour);
//    OldTime.wDay = (WORD)((int)pNode->bAlmDay);
//    OldTime.wMonth = (WORD)((int)pNode->bAlmMonth);
//    OldTime.wYear = (WORD)((int)pNode->wAlmYear);

    OldTime.wMinute = (WORD)((int)pNode->bAlmMinute);
    OldTime.wHour = (WORD)((int)pNode->bAlmHour);
    OldTime.wDay = (WORD)((int)pNode->bAlmDay);
    OldTime.wMonth = (WORD)((int)pNode->bAlmMonth);
    OldTime.wYear = (WORD)((int)pNode->wAlmYear);

	nMin = (short)OldTime.wMinute - iMin;
	nHour = (short)OldTime.wHour - iHour;

    while(nMin < 0)
    {
        nMin += 60;
        nHour --;
    }
    while(nHour < 0)
    {
        nHour += 24;
        iDay ++;
    }

	OldTime.wMinute = (WORD)nMin;
	OldTime.wHour = (WORD)nHour;
	
    if(CALE_DayReduce(&OldTime, iDay))
    {
        pNode->wAlmYear = OldTime.wYear;
        pNode->bAlmMonth = (BYTE)OldTime.wMonth;
        pNode->bAlmDay = (BYTE)OldTime.wDay;
        pNode->bAlmHour = (BYTE)OldTime.wHour;
        pNode->bAlmMinute = (BYTE)OldTime.wMinute;
    }

    return;
}

void CALE_AnniToAlmNode(CaleAnniNode *pCurCale, CaleAlarmNode *pNode, DWORD dwSaveIndex)
{
    pNode->CaleType         = CALE_ANNI;
    pNode->iSaveIndex       = dwSaveIndex;
    
    pNode->wCaldYear        = pCurCale->AnniDisYear;
    pNode->bCaldMonth       = pCurCale->AnniDisMonth;
    pNode->bCaldDay         = pCurCale->AnniDisDay;
    pNode->bCaldHour        = pCurCale->AnniHour;
    pNode->bCaldMinute      = pCurCale->AnniMinute;

    pNode->wAlmYear         = pCurCale->AnniYear;
    pNode->bAlmMonth        = pCurCale->AnniMonth;
    pNode->bAlmDay          = pCurCale->AnniDay;
    pNode->bAlmHour         = pCurCale->AnniHour;
    pNode->bAlmMinute       = pCurCale->AnniMinute;    
    
    pNode->bRepStyle        = CALE_REP_YEAR;
    pNode->bRepMask         = 1;
    
    pNode->RepEndDate.bYear = 0;
    pNode->RepEndDate.bMonth = 0;
    pNode->RepEndDate.bDay  = 0;
    pNode->RepEndDate.bHour = 0;
    pNode->RepEndDate.bMinute = 0;
    
    pNode->pNext = NULL;
    
    memset(pNode->CaldSubject, 0x0, sizeof(MAX_CALE_OCCASION_LEN) + 1);
    strcpy(pNode->CaldSubject, pCurCale->AnniOccasion);
}

int CALE_KillAlarm(int CaleType, int nAlmID)
{
    unsigned short nID;

	nID = (unsigned short)CaleType;
	nID = (nID << 14) | (nAlmID & 0x3F);
    
	RTC_KillAlarms("Calendar", (u_INT2)nID);

    return TRUE;
}

int CALE_ResetAlarm(CaleAlarmNode *pNode, int nFlag)
{
	RTCTIME AlmTime;
	unsigned short nID;

    memset(&AlmTime, 0x0, sizeof(RTCTIME));

	AlmTime.v_nYear		= pNode->wAlmYear;
	AlmTime.v_nMonth	= pNode->bAlmMonth;
	AlmTime.v_nDay		= pNode->bAlmDay;
	AlmTime.v_nHour		= pNode->bAlmHour;
	AlmTime.v_nMinute	= pNode->bAlmMinute;

//	ASSERT(pNode->CaleType < 4);
	nID = (unsigned short)(pNode->CaleType);
	nID = (nID << 14) | (pNode->iSaveIndex & 0x3F);

	CALE_Print("\r\n[calendar]: Setting alarm, ID = 0x%08x\r\n", nID);
	return RTC_SetAlarms("Calendar", (u_INT2)nID, (u_INT1)(nFlag & (~ALMF_POWEROFF)), "Calendar", &AlmTime);
}

int CALE_SnoozeAlarm(CaleAlarmNode *pNode, int nMinute)
{
	SYSTEMTIME		tmnow;
	RTCTIME			tmalm;
	unsigned short	nID;

	if(NULL == pNode || nMinute <= 0)
		return 0;

	GetLocalTime(&tmnow);

	tmnow.wMinute += nMinute;
	while(tmnow.wMinute >= 60)
	{
		tmnow.wMinute -= 60;
		tmnow.wHour ++;
		if(tmnow.wHour > 23)
		{
			tmnow.wHour -= 24;
			CALE_DayIncrease(&tmnow, 1);
		}
	}

	if(tmnow.wYear > MAX_YEAR || tmnow.wYear < MIN_YEAR)
		return 0;

	tmalm.v_nYear   = (u_INT2)tmnow.wYear;
	tmalm.v_nMonth  = (u_INT1)tmnow.wMonth;
	tmalm.v_nDay    = (u_INT1)tmnow.wDay;
	tmalm.v_nHour   = (u_INT1)tmnow.wHour;
	tmalm.v_nMinute = (u_INT1)tmnow.wMinute;
	tmalm.v_nSecond = (u_INT1)tmnow.wSecond;

	nID = (unsigned short)(pNode->CaleType);
	nID = (nID << 14) | (pNode->iSaveIndex & 0x3F);

	CALE_Print("\r\n[calendar]: Setting snooze alarm, ID = 0x%08x\r\n", nID);
	return RTC_SetAlarms("Calendar", (u_INT2)nID, (u_INT1)(ALMF_REPLACE|ALMF_RELATIVE & (~ALMF_POWEROFF)), "Calendar", &tmalm);
}

void CALE_AlarmOut(SYSTEMTIME *CurAlarmSys, long lSysAlmID)
{
    int CaleType = 0;
    int nAlmId = 0;
    int iOpen = 0;

    CaleMeetingNode CurMeetDate;
    CaleAnniNode CurAnniDate;
    CaleAlarmNode Node;

	CaleType = (lSysAlmID >> 14) & 0x0003;
	nAlmId = (lSysAlmID & 0x003F);

    memset(&Node, 0x0, sizeof(CaleAlarmNode));
    if(CaleType == CALE_MEETING)
    {
        if((iOpen = CALE_OpenApi(CALE_MEETING)) == CALE_FAIL)
        {
            return;
        }
        memset(&CurMeetDate, 0x0, sizeof(CaleMeetingNode));
        if(CALE_ReadApi(iOpen, nAlmId, (BYTE*)&CurMeetDate, sizeof(CaleMeetingNode)) 
            == CALE_FAIL)
        {
            CALE_CloseApi(iOpen);
            return;
        }
        CALE_CloseApi(iOpen);

        CALE_MeetToAlmNode(&CurMeetDate, &Node, nAlmId);
        
        CALE_AlmWnd(CurAlarmSys, &Node);
    }//end if(CaleType == CALE_MEETING)
    else if(CaleType == CALE_ANNI)
    {
        if((iOpen = CALE_OpenApi(CALE_ANNI)) == CALE_FAIL)
        {
            return;
        }
        memset(&CurAnniDate, 0x0, sizeof(CaleAnniNode));
        if(CALE_ReadApi(iOpen, nAlmId, (BYTE*)&CurAnniDate, sizeof(CaleAnniNode)) 
            == CALE_FAIL)
        {
            CALE_CloseApi(iOpen);
            return;
        }
        CALE_CloseApi(iOpen);
        
        CALE_AnniToAlmNode(&CurAnniDate, &Node, nAlmId);
        
        CALE_AlmWnd(CurAlarmSys, &Node);
    }//end else if(CaleType == CALE_ANNI)
    return;
}

/*
void CALE_SeparateName(char *AlarmName, int *FirstName, int *SecondName)
{
    char Temp[1];
    
    strncpy(Temp, AlarmName, 1);
    *FirstName = atoi(Temp);
    
    AlarmName ++;
    *SecondName = atoi(AlarmName);

    return;
}
*/

BOOL CALE_RegisterAlarmClass(void* hInst)
{
    WNDCLASS wc;
    HINSTANCE hInstance;
    
    hInstance = (HINSTANCE)hInst;
    
    wc.style        = 0;
    wc.lpfnWndProc  = CaleAlarmWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = sizeof(CaleAlarm);
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CALEALARMCLASS";
    
    if(!RegisterClass(&wc))
        return FALSE;
    
    return TRUE;
}

BOOL CALE_UnRegisterAlarmClass()
{
    UnregisterClass("CALEALARMCLASS", NULL);
    return TRUE;
}

static LRESULT CALLBACK CaleAlarmWndProc(HWND hWnd, UINT message,
                                         WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult;
    HDC hDC;
    PCREATESTRUCT pCreateStruct;
    PCaleAlarm pCaleAlarm;

    static CaleAlarmNode *CurAlarmNode;
    static int CurFocus;
    RECT rcClient, rcTemp;
    CaleAlarmNode *tmpNode;

    static HBITMAP hDis;
    int SnoozeFlag;
    unsigned int TimerID;

    HBRUSH hBrush;
    COLORREF hOldBkClr;
    
    lResult = (LRESULT)TRUE;
    pCaleAlarm = GetUserData(hWnd);
    
    switch(message)
    {
    case WM_CREATE:
        {
            memset(pCaleAlarm, 0x0, sizeof(CaleAlarm));
            pCreateStruct = (LPCREATESTRUCT)lParam;
            
            memcpy(pCaleAlarm, pCreateStruct->lpCreateParams, sizeof(CaleAlarm));
            pCaleAlarm->hMeet = LoadImage(NULL, CALE_ICON_MEET, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            pCaleAlarm->hAnni = LoadImage(NULL, CALE_ICON_ANNI, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

            CurAlarmNode = pCaleAlarm->AlarmHead.pNext;
            CurFocus = 0;
            SnoozeFlag = CALE_GetSnooze();
            SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)(IDP_IME_OFF));
            if(SnoozeFlag == CALE_SNOOZE_OFF)
            {
                SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_BUTTON_OK, 1), (LPARAM)"");
            }
            else
            {
                SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_BUTTON_OK, 1), (LPARAM)(IDP_IME_SNOOZE));
            }
            SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

            if(CurAlarmNode->CaleType == CALE_MEETING)
            {
                SetWindowText(hWnd, IDP_IME_MEETING);
            }
            else if(CurAlarmNode->CaleType == CALE_ANNI)
            {
                SetWindowText(hWnd, IDP_IME_ANNIVERARY);
            }

			CALE_SetSystemIcons(hWnd);

			CALE_Print("\r\n[calendar]: First alarm comes, subject is <%s>\r\n", CurAlarmNode->CaldSubject);
			CALE_Print("\r\n[calendar]: set 60-seconds timer, ID is %d\r\n", CALE_ALARM_TIMER(CurAlarmNode));

			if(!IsCallingWnd())
			{
				CALE_Print("\r\n[calendar]: Playing music, la-la-la ...\r\n");
				PrioMan_CallMusic(PRIOMAN_PRIORITY_CALENDAR, 0);
			}
            SetTimer(hWnd, CALE_ALARM_TIMER(CurAlarmNode), SNOOZE_TIMEOUT * 1000, NULL);
			CALE_AlarmSystemOn(TRUE);

            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        }
        break;

    case WM_SIZE:
        {
        }
        break;

	case WM_TIMER:
		{
			int i;
			
			TimerID = (unsigned int)(wParam);
			
			KillTimer(hWnd, TimerID);
			tmpNode = NULL;

			if((UINT)wParam >= 100000)	// 60-seconds timer
			{
				CALE_Print("\r\n[calendar]: 60-seconds timer comes, wParam = %d\r\n", wParam);
				i = CALE_GetAlarmNodeByID(&pCaleAlarm->AlarmHead, &tmpNode, 
					(BYTE)(TimerID / 100000), (int)(TimerID % 100000));
				if(CurAlarmNode == tmpNode)
				{
					CALE_Print("\r\n[calendar]: Stop playing music after 60s\r\n");
					CALE_Print("\r\n[calendar]: Annoying? Music stopped, biu ...\r\n");

					PrioMan_EndCallMusic(PRIOMAN_PRIORITY_CALENDAR, TRUE);
					CALE_AlarmSystemOn(FALSE);
				}
			}
#ifdef _SNOOZE_BY_TIMER_
			else						// snooze timer
			{
				CALE_Print("\r\n[calendar]: Snooze comes, wParam =  %d\r\n", wParam);
				i = CALE_GetAlarmNodeByID(&pCaleAlarm->AlarmHead, &tmpNode, 
					(BYTE)(TimerID / 10000), (int)(TimerID % 10000));

				if(NULL == tmpNode)
				{
					// this should NOT happen
				//	PrioMan_EndCallMusic(PRIOMAN_PRIORITY_ALARM, TRUE);
				//	PrioMan_CallMusic(PRIOMAN_PRIORITY_ALARM, 8);
					CALE_Print("\r\n[calendar]: snooze can't find node!!!\r\n");
					break;
				}
				
				CALE_Print("\r\n[calendar]: Snooze subject is <%s>\r\n", tmpNode->CaldSubject);

				tmpNode->SnoozeNum -= 1;
				tmpNode->AlarmFlag = TRUE;

				CurAlarmNode = tmpNode;
				CurFocus = i;
				
				ShowWindow(hWnd, SW_SHOW);
				if(CurAlarmNode->CaleType == CALE_MEETING)
				{
					SetWindowText(hWnd, IDP_IME_MEETING);
				}
				else if(CurAlarmNode->CaleType == CALE_ANNI)
				{
					SetWindowText(hWnd, IDP_IME_ANNIVERARY);
				}
				
				if(!IsCallingWnd())
				{
					CALE_Print("\r\n[calendar]: Playing music, la-la-la ...\r\n");
					//	PrioMan_EndCallMusic(PRIOMAN_PRIORITY_CALENDAR, TRUE);
					PrioMan_CallMusic(PRIOMAN_PRIORITY_CALENDAR, 0);
				}
				SetTimer(hWnd, CALE_ALARM_TIMER(CurAlarmNode), SNOOZE_TIMEOUT * 1000, NULL);
				CALE_Print("\r\n[calendar]: set 60-seconds timer, ID is %d\r\n", CALE_ALARM_TIMER(CurAlarmNode));
				
				CALE_AlarmSystemOn(TRUE);
				
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
#endif
		}
		break;
		
    case WM_PAINT:
        {
            BITMAP tmpbm;
            
            hDC = BeginPaint(hWnd, NULL);

            if((pCaleAlarm->AlarmHead.nNodeNum) && (CurAlarmNode != NULL))
            {
                SYSTEMTIME TmpSys;
                char TmpDate[15];
                char TmpTime[15];
                char cPaintText[31];
                char cTempText[31];

                GetClientRect(hWnd, &rcClient);

                rcTemp.left = rcClient.left;
                rcTemp.right = rcClient.right;
                rcTemp.top = rcClient.top;
                rcTemp.bottom = rcClient.top + 30;

                hBrush = CreateBrush(BS_SOLID, CALE_CAMBRIDGEBLUE, 0);
                FillRect(hDC, &rcTemp, (HBRUSH)hBrush);
                hOldBkClr = SetBkColor(hDC, CALE_CAMBRIDGEBLUE);

                memset(&tmpbm, 0x0, sizeof(BITMAP));
                GetObject(pCaleAlarm->hMeet, sizeof(BITMAP), &tmpbm);
                
                if(CurAlarmNode->CaleType == CALE_MEETING)
                {
                    BitBlt(hDC,
                        rcTemp.left + (rcTemp.right - rcTemp.left - tmpbm.bmWidth) / 2,
                        rcTemp.top + (rcTemp.bottom - rcTemp.top - tmpbm.bmHeight) / 2,
                        tmpbm.bmWidth, 
                        tmpbm.bmHeight, 
                        (HDC)pCaleAlarm->hMeet, 0, 0, ROP_SRC);
                }
                else if(CurAlarmNode->CaleType == CALE_ANNI)
                {
                    BitBlt(hDC,
                        rcTemp.left + (rcTemp.right - rcTemp.left - tmpbm.bmWidth) / 2,
                        rcTemp.top + (rcTemp.bottom - rcTemp.top - tmpbm.bmHeight) / 2,
                        tmpbm.bmWidth, 
                        tmpbm.bmHeight,  
                        (HDC)pCaleAlarm->hAnni, 0, 0, ROP_SRC);
                }
                
                SetBkColor(hDC, hOldBkClr);
                DeleteObject(hBrush);

                hBrush = CreateBrush(BS_SOLID, CALE_WHITE, 0);
                hOldBkClr = SetBkColor(hDC, CALE_WHITE);

                TmpSys.wYear = (WORD)CurAlarmNode->wCaldYear;
                TmpSys.wMonth = (WORD)CurAlarmNode->bCaldMonth;
                TmpSys.wDay = (WORD)CurAlarmNode->bCaldDay;
                TmpSys.wHour = (WORD)CurAlarmNode->bCaldHour;
                TmpSys.wMinute = (WORD)CurAlarmNode->bCaldMinute;
                memset(TmpDate, 0x0, 15);
                memset(TmpTime, 0x0, 15);
                GetTimeDisplay(TmpSys, TmpTime, TmpDate);
                
                rcTemp.top = rcTemp.bottom;
                rcTemp.bottom = rcTemp.top + 30;
                FillRect(hDC, &rcTemp, (HBRUSH)hBrush);
                DrawText(hDC, TmpDate, -1, &rcTemp, DT_CENTER | DT_BOTTOM);

                rcTemp.top = rcTemp.bottom;
                rcTemp.bottom = rcTemp.top + 30;
                FillRect(hDC, &rcTemp, (HBRUSH)hBrush);
                DrawText(hDC, TmpTime, -1, &rcTemp, DT_CENTER | DT_BOTTOM);
                
                memset(cTempText, 0x0, 31);
                rcTemp.top = rcTemp.bottom;
                rcTemp.bottom = rcTemp.top + 30;
                sprintf(cTempText, "%s", CurAlarmNode->CaldSubject);
                memset(cPaintText, 0x0, 31);
                CALE_FormatDisplay(hWnd, cPaintText, cTempText);
                FillRect(hDC, &rcTemp, (HBRUSH)hBrush);
                DrawText(hDC, cPaintText, -1, &rcTemp, DT_CENTER | DT_BOTTOM);

                memset(cTempText, 0x0, 31);
                rcTemp.top = rcTemp.bottom;
                rcTemp.bottom = rcTemp.top + 30;
                FillRect(hDC, &rcTemp, (HBRUSH)hBrush);
                if(CurAlarmNode->CaleType == CALE_MEETING)
                {
                    sprintf(cTempText, "%s", CurAlarmNode->CaldLocation);
                    memset(cPaintText, 0x0, 31);
                    CALE_FormatDisplay(hWnd, cPaintText, cTempText);
                    DrawText(hDC, cPaintText, -1, &rcTemp, DT_CENTER | DT_BOTTOM);
                }

                SetBkColor(hDC, hOldBkClr);
                DeleteObject(hBrush);
                
                if(CurAlarmNode->CaleType == CALE_MEETING)
                {
                    SetWindowText(hWnd, IDP_IME_MEETING);
                }
                else if(CurAlarmNode->CaleType == CALE_ANNI)
                {
                    SetWindowText(hWnd, IDP_IME_ANNIVERARY);
                }
                SnoozeFlag = CALE_GetSnooze();
                if((SnoozeFlag == CALE_SNOOZE_OFF) || (CurAlarmNode->SnoozeNum == 0))
                {
                    SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                }
                else
                {
                    SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)(IDP_IME_SNOOZE));
                }
            }
			else
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				CALE_Print("\r\n[calendar]: nothing to be painted. close now. (this should NOT happen)\r\n");
			}
            
            EndPaint(hWnd, NULL);
        }
        break;

    case WM_CLOSE:
        {
            DestroyWindow(hWnd);
        }
        break;
        
    case WM_DESTROY:
        {
            hCaleAlarm = NULL;
            CALE_ReleaseAlarmNode(&pCaleAlarm->AlarmHead);
            DeleteObject(pCaleAlarm->hMeet);
            DeleteObject(pCaleAlarm->hAnni);
            CALE_UnRegisterAlarmClass();
        }
        break;

	case CALE_ALARM_OFF:
		{
			CaleMeetingNode CurMeetDate;
			CaleAnniNode CurAnniDate;
			int iOpen;
			
#ifdef _CALENDAR_LOCAL_TEST_		
			ASSERT(NULL != CurAlarmNode);
#endif
			CALE_Print("\r\n[calendar]: Turn off alarm, subject is <%s>\r\n", CurAlarmNode->CaldSubject);

			if(CurAlarmNode->CaleType == CALE_MEETING)
			{
				iOpen = CALE_OpenApi(CALE_MEETING);
				if(iOpen != CALE_FAIL)
				{
					CALE_ReadApi(iOpen, CurAlarmNode->iSaveIndex, (BYTE*)&CurMeetDate, sizeof(CaleMeetingNode));
					CALE_CloseApi(iOpen);
					CALE_NewMeet(&CurMeetDate, CurAlarmNode->iSaveIndex, ALMF_REPLACE);
				}
			}
			else if(CurAlarmNode->CaleType == CALE_ANNI)
			{
				iOpen = CALE_OpenApi(CALE_ANNI);
				if(iOpen != CALE_FAIL)
				{
					CALE_ReadApi(iOpen, CurAlarmNode->iSaveIndex, (BYTE*)&CurAnniDate, sizeof(CaleAnniNode));
					CALE_CloseApi(iOpen);
					CALE_NewAnni(&CurAnniDate, CurAlarmNode->iSaveIndex, ALMF_REPLACE);
				}
			}
			
			KillTimer(hWnd, CALE_ALARM_TIMER(CurAlarmNode));
			CALE_DelAlarmNode(&pCaleAlarm->AlarmHead, CurFocus);
			PrioMan_EndCallMusic(PRIOMAN_PRIORITY_CALENDAR, TRUE);
			CALE_Print("\r\n[calendar]: Annoying? Music stopped, biu ...\r\n");
			
			if((pCaleAlarm->AlarmHead.pNext == NULL) && (pCaleAlarm->AlarmHead.nNodeNum == 0))
			{
				if(wParam)
					PostMessage(hWnd, WM_CLOSE, 0, 0);
				CALE_AlarmSystemOn(FALSE);
				CALE_LOCK();
				CALE_Print("\r\n[calendar]: No other alarms now, closing alarm window\r\n");
				break;
			}
			
			CurAlarmNode = pCaleAlarm->AlarmHead.pNext;
			CurFocus = 0;
			while(CurAlarmNode)
			{
				if(CurAlarmNode->AlarmFlag)
				{
					// not ring during second show
					CALE_Print("\r\n[calendar]: Find aother alarm, subject is <%s>\r\n", CurAlarmNode->CaldSubject);
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
					
					CALE_AlarmSystemOn(TRUE);
					return TRUE;
				}
				CurAlarmNode = CurAlarmNode->pNext;
				CurFocus ++;
			}
			
			CALE_Print("\r\n[calendar]: No other alarms now, but there're SNOOZES. Hiding alarm window\r\n");

			if(wParam)
				ShowWindow(hWnd, SW_HIDE);
			CALE_AlarmSystemOn(FALSE);
			CALE_LOCK();
		}
		break;

	case CALE_ALARM_SNOOZE:
		{
			int SnoozeTime;
			
#ifdef _CALENDAR_LOCAL_TEST
			ASSERT(NULL != CurAlarmNode);
#endif
			CALE_Print("\r\n[calendar]: Snooze this alarm, subject is <%s>\r\n", CurAlarmNode->CaldSubject);

			if(CurAlarmNode->SnoozeNum == 0)
			{
				CALE_Print("\r\n[calendar]: already 10 times of snooze, not snooze any more.\r\n");
				PostMessage(hWnd, CALE_ALARM_OFF, (WPARAM)TRUE, 0);
				break;
			}
			
			SnoozeFlag = CALE_GetSnooze();
			if(SnoozeFlag == CALE_SNOOZE_OFF)
			{
				CALE_Print("\r\n[calendar]: Negative. Calendar setting says no snooze.\r\n");
				PostMessage(hWnd, CALE_ALARM_OFF, (WPARAM)TRUE, 0);
				break;
			}
			else if(SnoozeFlag == CALE_SNOOZE_5MIN)
			{
				SnoozeTime = 5;
			}
			else if(SnoozeFlag == CALE_SNOOZE_10MIN)
			{
				SnoozeTime = 10;
			}
			else if(SnoozeFlag == CALE_SNOOZE_30MIN)
			{
				SnoozeTime = 30;
			}
			
			CurAlarmNode->AlarmFlag = FALSE;
			KillTimer(hWnd, CALE_ALARM_TIMER(CurAlarmNode));
			PrioMan_EndCallMusic(PRIOMAN_PRIORITY_CALENDAR, TRUE);
			CALE_Print("\r\n[calendar]: Annoying? Music stopped, biu ...\r\n");
			
#ifdef _SNOOZE_BY_TIMER_
			TimerID = CurAlarmNode->CaleType * 10000 + CurAlarmNode->iSaveIndex;
			SetTimer(hWnd, TimerID, SnoozeTime * 60 * 1000, NULL);

			CALE_Print("\r\n[calendar]: Snooze timer has been set, snooze time is %d min\r\n", SnoozeTime);
#else
			if(!CALE_SnoozeAlarm(CurAlarmNode, SnoozeTime))
			{
				PLXTipsWin(NULL, NULL, 0, ML("Set alarm failed!"), ML("Calendar"), 
					Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
			}
			CALE_Print("\r\n[calendar]: Snooze alarm has been set, snooze time is %d min\r\n", SnoozeTime);
#endif
			
			CurAlarmNode = pCaleAlarm->AlarmHead.pNext;
			CurFocus = 0;
			while(CurAlarmNode)
			{
				if(CurAlarmNode->AlarmFlag)
				{
					// not ring during second show
					CALE_Print("\r\n[calendar]: Showing old alarm, subject is <%s>\r\n", CurAlarmNode->CaldSubject);
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
					
					CALE_AlarmSystemOn(TRUE);
					return TRUE;
				}
				CurAlarmNode = CurAlarmNode->pNext;
				CurFocus ++;
			}
			
			CALE_Print("\r\n[calendar]: Hiding alarm after snooze\r\n");

			if(wParam)
			{
				if((pCaleAlarm->AlarmHead.pNext == NULL) && (pCaleAlarm->AlarmHead.nNodeNum == 0))
					PostMessage(hWnd, WM_CLOSE, 0, 0);
				else
					ShowWindow(hWnd, SW_HIDE);
			}
			CALE_AlarmSystemOn(FALSE);
			CALE_LOCK();
		}
		break;

    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDM_BUTTON_QUIT:
                {
					PostMessage(hWnd, CALE_ALARM_OFF, (WPARAM)TRUE, 0);
                }
            	break;

            case IDM_BUTTON_OK:
                {
					PostMessage(hWnd, CALE_ALARM_SNOOZE, (WPARAM)TRUE, 0);
                }
            	break;

            default:
                lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_KEYDOWN:
        {
            switch(wParam)
            {
            case VK_RETURN:
                {
                    if(CurAlarmNode->SnoozeNum == 0)
                    {
                        break;
                    }
                    else
                    {
                        PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_OK, 0), 0);
                    }
                }
                break;

            case VK_F10:
                {
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_QUIT, 0), 0);
                }
                break;

            default:
                lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case CALE_ALARM_SYS:
        {
			int i;
			if(NULL == lParam)
				break;

#ifndef _SNOOZE_BY_TIMER_

			tmpNode = NULL;
			i = CALE_GetAlarmNodeByID(&pCaleAlarm->AlarmHead, &tmpNode, 
				((CaleAlarmNode *)lParam)->CaleType, ((CaleAlarmNode *)lParam)->iSaveIndex);
				
			if(NULL != tmpNode)		// snooze
			{
				CALE_Print("\r\n[calendar]: Snooze comes, wParam =  %d\r\n", wParam);				
				CALE_Print("\r\n[calendar]: Snooze subject is <%s>\r\n", tmpNode->CaldSubject);
				
				tmpNode->SnoozeNum -= 1;
				tmpNode->AlarmFlag = TRUE;
				
				CurAlarmNode = tmpNode;
				CurFocus = i;
			}
			else					// new alarm
#endif
			{
				CALE_Print("\r\n[calendar]: New alarm comes\r\n");
				
				tmpNode = (CaleAlarmNode *)malloc(sizeof(CaleAlarmNode));
				if(tmpNode == NULL)
				{
					break;
				}
				memset(tmpNode, 0x0, sizeof(CaleAlarmNode));
				memcpy(tmpNode, (CaleAlarmNode *)lParam, sizeof(CaleAlarmNode));
            
				tmpNode->wAlmYear = (WORD)((LPSYSTEMTIME)wParam)->wYear;
				tmpNode->bAlmMonth = (BYTE)((LPSYSTEMTIME)wParam)->wMonth;
				tmpNode->bAlmDay = (BYTE)((LPSYSTEMTIME)wParam)->wDay;
				tmpNode->bAlmHour = (BYTE)((LPSYSTEMTIME)wParam)->wHour;
				tmpNode->bAlmMinute = (BYTE)((LPSYSTEMTIME)wParam)->wMinute;
				tmpNode->AlarmFlag = TRUE;
				tmpNode->SnoozeNum = 10;
				tmpNode->pNext = NULL;
            
				CALE_Print("\r\n[calendar]: New alarm's subject is <%s>\r\n", tmpNode->CaldSubject);

				CALE_AddAlarmNode(&(pCaleAlarm->AlarmHead), tmpNode);
/*
				if(IsWindowVisible(hWnd))
				{
					ShowWindow(hCaleAlarm, SW_SHOW);
					BringWindowToTop(hCaleAlarm);
					UpdateWindow(hCaleAlarm);
					
					break;
				}
//*/			
				CurAlarmNode = pCaleAlarm->AlarmHead.pNext;
				CurFocus = 0;
				while(CurAlarmNode)
				{
					if(CurAlarmNode->AlarmFlag)
					{
						break;
					}
					CurAlarmNode = CurAlarmNode->pNext;
					CurFocus ++;
				}
			}
#ifdef _CALENDAR_LOCAL_TEST_
			ASSERT(NULL != CurAlarmNode);
#endif

            SnoozeFlag = CALE_GetSnooze();
            SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)(IDP_IME_OFF));
            if(SnoozeFlag == CALE_SNOOZE_OFF)
            {
                SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_BUTTON_OK, 1), (LPARAM)"");
            }
            else
            {
                SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_BUTTON_OK, 1), (LPARAM)(IDP_IME_SNOOZE));
            }
            SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            
            if(CurAlarmNode->CaleType == CALE_MEETING)
            {
                SetWindowText(hWnd, IDP_IME_MEETING);
            }
            else if(CurAlarmNode->CaleType == CALE_ANNI)
            {
                SetWindowText(hWnd, IDP_IME_ANNIVERARY);
            }
		
			if(!IsCallingWnd())
			{
				CALE_Print("\r\n[calendar]: Playing music, la-la-la ...\r\n");
				//	PrioMan_EndCallMusic(PRIOMAN_PRIORITY_CALENDAR, TRUE);
				PrioMan_CallMusic(PRIOMAN_PRIORITY_CALENDAR, 0);
			}
            SetTimer(hWnd, CALE_ALARM_TIMER(CurAlarmNode), SNOOZE_TIMEOUT * 1000, NULL);
			CALE_Print("\r\n[calendar]: set 60-seconds timer, ID is %d\r\n", CALE_ALARM_TIMER(CurAlarmNode));

			CALE_AlarmSystemOn(TRUE);

            InvalidateRect(hWnd, NULL, TRUE);
            ShowWindow(hCaleAlarm, SW_SHOW);
            BringWindowToTop(hCaleAlarm);
            UpdateWindow(hCaleAlarm);
        }
        break;

    default:
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
    }
    return lResult;
}

void CALE_AlmWnd(SYSTEMTIME *CurAlarmSys, CaleAlarmNode *CurAlarmNode)//CaleAlarmHead *AlmWndHead,
{
    CaleAlarm CreateAlarm;
    CaleAlarmNode *tmpNode;
    
    if(IsWindow(hCaleAlarm))
    {
        SendMessage(hCaleAlarm, CALE_ALARM_SYS, (WPARAM)(CurAlarmSys), (LPARAM)(CurAlarmNode));
    }
    else
    {
        memset(&CreateAlarm, 0x0, sizeof(CaleAlarm));
        CALE_ReleaseAlarmNode(&CreateAlarm.AlarmHead);
        tmpNode = (CaleAlarmNode *)malloc(sizeof(CaleAlarmNode));
        if(tmpNode == NULL)
        {
            return;
        }
        memset(tmpNode, 0x0, sizeof(CaleAlarmNode));
        memcpy(tmpNode, (CaleAlarmNode *)CurAlarmNode, sizeof(CaleAlarmNode));
        
        tmpNode->wAlmYear = (WORD)CurAlarmSys->wYear;
        tmpNode->bAlmMonth = (BYTE)CurAlarmSys->wMonth;
        tmpNode->bAlmDay = (BYTE)CurAlarmSys->wDay;
        tmpNode->bAlmHour = (BYTE)CurAlarmSys->wHour;
        tmpNode->bAlmMinute = (BYTE)CurAlarmSys->wMinute;
        tmpNode->AlarmFlag = TRUE;
        tmpNode->SnoozeNum = 10;
        tmpNode->pNext = NULL;
        
        CALE_AddAlarmNode(&(CreateAlarm.AlarmHead), tmpNode);
        
        if (CALE_RegisterAlarmClass(NULL))
        {
            CALE_UnRegisterAlarmClass();
            if(!CALE_RegisterAlarmClass(NULL))
                return;
        }
        
        hCaleAlarm = CreateWindow(
            "CALEALARMCLASS",
            "",
            WS_VISIBLE | WS_CAPTION | PWS_STATICBAR,
            //SUBWND_X, SUBWND_Y, SUBWND_WIDTH, SUBWND_HEIGHT,
            WND_X, WND_Y, WND_WIDTH, WND_HEIGHT,
            NULL,
            NULL,
            NULL,
            (PVOID)&CreateAlarm
            );

        ShowWindow(hCaleAlarm, SW_SHOW);
        UpdateWindow(hCaleAlarm);
    }
    return;
}

BOOL CALE_AddAlarmNode(CaleAlarmHead *pHead, CaleAlarmNode *pNode)
{
//	CaleAlarmNode *pTemp;

	if(NULL == pHead || NULL == pNode)
		return FALSE;
/*	
	pNode->pNext = NULL;
	if(pHead->pNext == NULL)
	{
		pHead->pNext = pNode;
		pHead->nNodeNum ++;
		return TRUE;
	}
	pTemp = pHead->pNext;
	while(pTemp->pNext != NULL)
	{
		pTemp = pTemp->pNext;
	}
	pTemp->pNext = pNode;
	pHead->nNodeNum ++;
//*/

	pNode->pNext = pHead->pNext;
	pHead->pNext = pNode;
	pHead->nNodeNum ++;
	return TRUE;
}

BOOL CALE_DelAlarmNode(CaleAlarmHead *pHead, int iTmp)
{
    CaleAlarmNode *pTemp;
    CaleAlarmNode *pPre;
    int i;
    
    if(iTmp > (pHead->nNodeNum - 1))
        return FALSE;
    
    if(pHead->pNext == NULL)
        return FALSE;
    
    pTemp = pHead->pNext;
    if(iTmp == 0)
    {
        pHead->pNext = pTemp->pNext;
        free(pTemp);
        pTemp = NULL;
    }
    else
    {
        for(i = 0; i < iTmp; i ++)
        {
            pPre = pTemp;
            pTemp = pPre->pNext;
        }
        pPre->pNext = pTemp->pNext;
        free(pTemp);
        pTemp = NULL;
    }
    pHead->nNodeNum --;
    
    return TRUE;
}

static void CALE_ReleaseAlarmNode(CaleAlarmHead *pHead)
{
    CaleAlarmNode *pTemp;
    
    pTemp = pHead->pNext;
    while (pTemp)
    {
        pHead->pNext = pTemp->pNext;
        free(pTemp);
        pTemp = NULL;
        pTemp = pHead->pNext;
    }
    pHead->nNodeNum = 0;
    pHead->pNext = NULL;
    
    return;
}

static int CALE_GetAlarmNodeByID(CaleAlarmHead *pHead, CaleAlarmNode **pNode, BYTE bType, int iIndex)
{
    CaleAlarmNode *pTemp;
    int i;
    
    i = 0;
    pTemp = pHead->pNext;
    while (pTemp)
    {
        if((pTemp->CaleType == bType) && (pTemp->iSaveIndex == iIndex))
        {
            *pNode = pTemp;
            return i;
        }
        i ++;
        pTemp = pTemp->pNext;
    }
    
    return i;
}

BOOL CALE_IsAlarmWnd(void)
{
    if(IsWindow(hCaleAlarm))
    {
        if(IsWindowVisible(hCaleAlarm))
        {
            return TRUE;
        }
    }
    return FALSE;
}

void CALE_UNLOCK(void)
{
    if(PM_GetkeyLockStatus() || PM_GetPhoneLockStatus)
    {
        DlmNotify(PS_LOCK_ENABLE, FALSE);
        bChangedLock = TRUE;
    }
    else
    {
        bChangedLock = FALSE;
    }
    
    return;
}

void CALE_LOCK(void)
{
    if(bChangedLock)
    {
        bChangedLock = FALSE;
        DlmNotify(PS_LOCK_ENABLE, TRUE);
    }
    return;
}

static void CALE_AlarmSystemOn(BOOL bOn)
{
	if(bOn)		// on
	{
		CALE_UNLOCK();						// unlock key pad
		DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);	//turn on back light
		DlmNotify(MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 60);	//keep back light on for 60s
		f_sleep_register(CAL_ALARM);		// disable sleeping
		f_DisablePowerOff(CALENDARALARM);	// disable power off
	}
	else		// off
	{
	//	CALE_LOCK();						// key pad can be locked now
		DlmNotify (PMM_NEWS_ENABLE,PMF_AUTO_SHUT_MAIN_LIGHT);	//back light can be off after 20s
		f_sleep_unregister(CAL_ALARM);		// sleeping enabled
		f_EnablePowerOff(CALENDARALARM);	// power off enabled
	}
}

BOOL CALE_NeedClean(void)
{
    int Cal_OpenFlag;
    unsigned long lDelFlag;

	if(IsWindow(hCaleAlarm))
	{
		CALE_Print("\r\n[calendar]: alarm window is there, cleaning is forbidden!\r\n");
		return FALSE;
	}
	
    Cal_OpenFlag = open(CALE_DELETE_FLAG, O_RDWR | O_CREAT, S_IRWXU);
    if(Cal_OpenFlag < 0)
        return FALSE;

    lseek(Cal_OpenFlag, 0, SEEK_SET);
    read(Cal_OpenFlag, &lDelFlag, sizeof(unsigned long));
	close(Cal_OpenFlag);
#ifdef _CALENDAR_LOCAL_TEST_
	return (BOOL)(lDelFlag < 0x0fffffff);
#else
    return (BOOL)(lDelFlag == 0);
#endif
}
