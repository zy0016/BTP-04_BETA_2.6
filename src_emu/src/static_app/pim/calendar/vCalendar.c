/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : vCalendar.c
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleHeader.h"

/*typedef struct tagVcardVcal
{ 
    char*   pszTitle;
    char*   pszContent;
    struct tagVcardVcal *pNext;
}VCARDVCAL,*PVCARDVCAL;*/

CaleMeetingNode CaleMeet;
CaleAnniNode CaleAnni;
CaleEventNode CaleEvent;

BOOL CALE_EncodeVcalendar(int itemID, BYTE *pRecord);
BOOL CALE_DecodeVcalendar(vCal_Obj *pObj, int *CaleType);
BOOL CALE_DecodeVcalToDisplay(vCal_Obj *pObj, VCARDVCAL **ppChain);
void CALE_VcaleChainEarse(VCARDVCAL *pChain);
BOOL CALE_SaveFromSms(vCal_Obj *pObj);

static int StringToInt(char *str);
static void CALE_CalcVcalAlarm(SYSTEMTIME *pDate, int iDay, int iHour, int iMin);
static int CALE_CalcBeforeAlarm(SYSTEMTIME *pDate, SYSTEMTIME *pBefore);

extern void CALE_DateTimeSysToChar(SYSTEMTIME *pSys, char *pChar);
extern BOOL CALE_DayReduce(SYSTEMTIME *psystime, unsigned int day);
extern int CALE_CmpSysDate(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate);
extern int CALE_IntervalSysDate(SYSTEMTIME *pStartDate, SYSTEMTIME *pEndDate);
extern int CALE_CmpOnlyDate(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate);
extern int CALE_CmpOnlyTime(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate);

extern char* GetVisitZoneStr(void);
extern char* GetHomeZoneStr(void);

BOOL CALE_EncodeVcalendar(int itemID, BYTE *pRecord)
{
    CaleMeetingNode *pCaleMeet;
    CaleAnniNode *pCaleAnni;
    CaleEventNode *pCaleEvent;

    vCal_Obj obj;
    vCal_entity	entity;
    Remindar	remindar;
    RecuRule	repeatRule;
    DT			startDT, endDT;
    DT          AlarmDT;
    Content		pCont;
    Content     pSum;
    Location    pLoca;
    Categories  pCate;
    TDuration	dureDT;
    char szDescBuff[100];
    char szLocaBuff[100];
    char szSumBuff[20];
	char c_timezone[16];

    unsigned char *pDataBuffer;
    int	 nDataLen;

    char szOutPath[100];
    int hFile;

	SYSTEMTIME tmpTime;
	
	memset(&tmpTime, 0, sizeof(SYSTEMTIME));
	memset(&startDT, 0, sizeof(DT));
	memset(&endDT, 0, sizeof(DT));
	
	if(GetUsingTimeZone() == TZ_VISIT)
		strcpy(c_timezone, GetVisitZoneStr());
	else
		strcpy(c_timezone, GetHomeZoneStr());
	
    if(itemID == CALE_MEETING)
    {
        pCaleMeet = (CaleMeetingNode *)pRecord;
        
		tmpTime.wYear	= pCaleMeet->MeetStartYear;
		tmpTime.wMonth	= pCaleMeet->MeetStartMonth;
		tmpTime.wDay	= pCaleMeet->MeetStartDay;
		tmpTime.wHour	= pCaleMeet->MeetStartHour;
		tmpTime.wMinute = pCaleMeet->MeetStartMinute;

//        sprintf(startDT.Date, "%04d%02d%02d", pCaleMeet->MeetStartYear, pCaleMeet->MeetStartMonth, pCaleMeet->MeetStartDay);
//        sprintf(startDT.Time, "%02d%02d00", pCaleMeet->MeetStartHour, pCaleMeet->MeetStartMinute);
//
//        sprintf(endDT.Date, "%04d%02d%02d", pCaleMeet->MeetEndYear, pCaleMeet->MeetEndMonth, pCaleMeet->MeetEndDay);
//        sprintf(endDT.Time, "%02d%02d00", pCaleMeet->MeetEndHour, pCaleMeet->MeetEndMinute);

		CALE_GetUTCTime(&tmpTime, c_timezone, FALSE);

        sprintf(startDT.Date, "%04d%02d%02d", tmpTime.wYear, tmpTime.wMonth, tmpTime.wDay);
        sprintf(startDT.Time, "%02d%02d00", tmpTime.wHour, tmpTime.wMinute);

		tmpTime.wYear	= pCaleMeet->MeetEndYear;
		tmpTime.wMonth	= pCaleMeet->MeetEndMonth;
		tmpTime.wDay	= pCaleMeet->MeetEndDay;
		tmpTime.wHour	= pCaleMeet->MeetEndHour;
		tmpTime.wMinute = pCaleMeet->MeetEndMinute;

		CALE_GetUTCTime(&tmpTime, c_timezone, FALSE);
		
        sprintf(endDT.Date, "%04d%02d%02d", tmpTime.wYear, tmpTime.wMonth, tmpTime.wDay);
        sprintf(endDT.Time, "%02d%02d00", tmpTime.wHour, tmpTime.wMinute);

        memset(&entity, 0, sizeof(entity));
        entity.EntityType = VCAL_VEVENT;
        entity.pDTStart = &startDT;
        entity.pDTEnd = &endDT;
        entity.pDTCreated = &startDT;
        entity.pDTLast_Modified = &startDT;

        memset(&pCont, 0, sizeof(Content));
        pCont.Charset = VCAL_CHARSET_UTF_8;
//        pCont.Valuelen = MultiByteToUTF8(CP_ACP, 0, pCaleMeet->MeetSubject, -1,
//            szDescBuff, 100 );
        pCont.Valuelen = MultiByteToUTF8(CP_ACP, 0, ML("meeting"), -1,
            szDescBuff, 100 );
        pCont.Valuelen --;
        pCont.pValue = szDescBuff;
        entity.pDescription = &pCont;

        memset(&pLoca, 0x0, sizeof(Location));
		pLoca.type = VCAL_TYPE_VCARD;
		pLoca.ValueType = VCAL_VALUE_INLINE;
        pLoca.Charset = VCAL_CHARSET_UTF_8;
        pLoca.Valuelen = MultiByteToUTF8(CP_ACP, 0, pCaleMeet->MeetLocation, -1,
            szLocaBuff, 100 );
        pLoca.Valuelen --;
        pLoca.pValue = szLocaBuff;
        entity.pLocation = &pLoca;

        memset(&pSum, 0x0, sizeof(Content));
        pSum.Charset = VCAL_CHARSET_UTF_8;
//        pSum.Valuelen = MultiByteToUTF8(CP_ACP, 0, "meeting", -1,
//            szSumBuff, 100 );
        pSum.Valuelen = MultiByteToUTF8(CP_ACP, 0, pCaleMeet->MeetSubject, -1,
            szSumBuff, 100 );
        pSum.Valuelen --;
        pSum.pValue = szSumBuff;
        entity.pSummary = &pSum;

        memset(&pCate, 0x0, sizeof(Categories));
        pCate.Category = VCAL_CAT_MEETING;
        pCate.pNext = NULL;
        entity.pCategories = &pCate;

        entity.Class = VCAL_CLASS_PRIVATE;

        memset(&dureDT, 0, sizeof(TDuration));
        dureDT.Weekly = FALSE;
        dureDT.Minute = 1;
        
        memset(&remindar, 0, sizeof(Remindar));
        if (pCaleMeet->MeetAlarmFlag)
        {
		//	int iIndex;
            SCENEMODE scenemode;
            SYSTEMTIME AlarmDate;

            remindar.pNext = NULL;
            remindar.Remindertype = VCAL_REMINDER_AUDIO;

            memset(&AlarmDate, 0x0, sizeof(SYSTEMTIME));
            AlarmDate.wYear = (WORD)pCaleMeet->MeetStartYear;
            AlarmDate.wMonth = (WORD)pCaleMeet->MeetStartMonth;
            AlarmDate.wDay = (WORD)pCaleMeet->MeetStartDay;
            AlarmDate.wHour = (WORD)pCaleMeet->MeetStartHour;
            AlarmDate.wMinute = (WORD)pCaleMeet->MeetStartMinute;
			
			CALE_GetUTCTime(&AlarmDate, c_timezone, FALSE);

            if(pCaleMeet->MeetAlarmFlag == CALE_BEFORE_5MIN)
            {
                CALE_CalcVcalAlarm(&AlarmDate, 0, 0, 5);
            }
            else if(pCaleMeet->MeetAlarmFlag == CALE_BEFORE_10MIN)
            {
                CALE_CalcVcalAlarm(&AlarmDate, 0, 0, 10);
            }
            else if(pCaleMeet->MeetAlarmFlag == CALE_BEFORE_15MIN)
            {
                CALE_CalcVcalAlarm(&AlarmDate, 0, 0, 15);
            }
            else if(pCaleMeet->MeetAlarmFlag == CALE_BEFORE_30MIN)
            {
                CALE_CalcVcalAlarm(&AlarmDate, 0, 0, 30);
            }
            else if(pCaleMeet->MeetAlarmFlag == CALE_BEFORE_45MIN)
            {
                CALE_CalcVcalAlarm(&AlarmDate, 0, 0, 45);
            }
            else if(pCaleMeet->MeetAlarmFlag == CALE_BEFORE_1HOUR)
            {
                CALE_CalcVcalAlarm(&AlarmDate, 0, 1, 0);
            }
            else if(pCaleMeet->MeetAlarmFlag == CALE_BEFORE_6HOUR)
            {
                CALE_CalcVcalAlarm(&AlarmDate, 0, 6, 0);
            }
            else if(pCaleMeet->MeetAlarmFlag == CALE_BEFORE_12HOUR)
            {
                CALE_CalcVcalAlarm(&AlarmDate, 0, 12, 0);
            }
            else if(pCaleMeet->MeetAlarmFlag == CALE_BEFORE_1DAY)
            {
                CALE_CalcVcalAlarm(&AlarmDate, 1, 0, 0);
            }
            else if(pCaleMeet->MeetAlarmFlag == CALE_BEFORE_1WEEK)
            {
                CALE_CalcVcalAlarm(&AlarmDate, 7, 0, 0);
            }
            sprintf(AlarmDT.Date, "%04d%02d%02d", AlarmDate.wYear, AlarmDate.wMonth, AlarmDate.wDay);
            sprintf(AlarmDT.Time, "%02d%02d00", AlarmDate.wHour, AlarmDate.wMinute);
            remindar.pDTRun = &AlarmDT;

            remindar.pSnooze = &dureDT;
            remindar.Repeat = 10;
            remindar.AudioType = VCAL_AUDIO_PCM;
            remindar.ValueType = VCAL_VALUE_URL;
            memset(&scenemode, 0x0, sizeof(SCENEMODE));

			Sett_GetActiveSM(&scenemode);
            remindar.pValue = scenemode.rCalendar.cMusicFileName;
            entity.pRemindar = &remindar;
        }
        
        if (pCaleMeet->MeetRepeatFlag == CALE_REP)
        {
            memset(&repeatRule, 0, sizeof(RecuRule));
            switch (pCaleMeet->MeetRepeatStyle)
            {
            case CALE_REP_NONE:
                break;
                
            case CALE_REP_DAY:
                repeatRule.FreqType = VCAL_RECU_DAILY;
                break;

            case CALE_REP_WEEK:
                repeatRule.FreqType = VCAL_RECU_WEEKLY;
                break;

            case CALE_REP_TWOWEEK:
                repeatRule.FreqType = VCAL_RECU_WEEKLY;
                break;

            case CALE_REP_MONTH:
                repeatRule.FreqType = VCAL_RECU_MONTHLY_DAY;
                break;

            case CALE_REP_YEAR:
                repeatRule.FreqType = VCAL_RECU_YEARLY_DAY;
                break;
            }
            
            if(pCaleMeet->MeetRepeatFlag != CALE_REP_NONE)
            {
                repeatRule.Interval = 1;
                repeatRule.pEnddate = &endDT;
                repeatRule.Duration = VCAL_RECU_LASTDAY;
                entity.pRecuRule = &repeatRule;
            }
        }
        
        memset(&obj, 0, sizeof(obj));
        obj.pEntity = &entity;
		strcpy(obj.TimeZone, c_timezone+3);
        strcpy(obj.Version, VCAL_VERSION);

    }
    else if(itemID == CALE_ANNI)
    {
        pCaleAnni = (CaleAnniNode *)pRecord;
        
		tmpTime.wYear	= pCaleAnni->AnniDisYear;
		tmpTime.wMonth	= pCaleAnni->AnniDisMonth;
		tmpTime.wDay	= pCaleAnni->AnniDisDay;
		tmpTime.wHour	= pCaleAnni->AnniHour;
		tmpTime.wMinute = pCaleAnni->AnniMinute;

		CALE_GetUTCTime(&tmpTime, c_timezone, FALSE);

        sprintf(startDT.Date, "%04d%02d%02d", tmpTime.wYear, tmpTime.wMonth, tmpTime.wDay);
        sprintf(startDT.Time, "%02d%02d00", tmpTime.wHour, tmpTime.wMinute);

        memset(&entity, 0, sizeof(entity));
        entity.EntityType = VCAL_VEVENT;
        entity.pDTStart = &startDT;
        entity.pDTCreated = &startDT;
        entity.pDTLast_Modified = &startDT;

        memset(&pCont, 0, sizeof(Content));
        pCont.Charset = VCAL_CHARSET_UTF_8;
//        pCont.Valuelen = MultiByteToUTF8(CP_ACP, 0, pCaleAnni->AnniOccasion, -1,
//            szDescBuff, 100 );
        pCont.Valuelen = MultiByteToUTF8(CP_ACP, 0, ML("anniversary"), -1,
            szDescBuff, 100 );
        pCont.Valuelen --;
        pCont.pValue = szDescBuff;
        entity.pDescription = &pCont;

        memset(&pSum, 0x0, sizeof(Content));
        pSum.Charset = VCAL_CHARSET_UTF_8;
//        pSum.Valuelen = MultiByteToUTF8(CP_ACP, 0, "anniversary", -1,
//            szSumBuff, 20 );
        pSum.Valuelen = MultiByteToUTF8(CP_ACP, 0, pCaleAnni->AnniOccasion, -1,
            szSumBuff, 20 );
        pSum.Valuelen --;
        pSum.pValue = szSumBuff;
        entity.pSummary = &pSum;

        memset(&pCate, 0x0, sizeof(Categories));
        pCate.Category = VCAL_CAT_OTHER;
        pCate.pCategory = "anniversary";
        pCate.pNext = NULL;
        entity.pCategories = &pCate;

        entity.Class = VCAL_CLASS_PRIVATE;

        memset(&dureDT, 0, sizeof(TDuration));
        dureDT.Weekly = FALSE;
        dureDT.Minute = 1;
        
        memset(&remindar, 0, sizeof(Remindar));
        if (pCaleAnni->AnniAlarmFlag == 1)
        {
		//	int iIndex;
            SCENEMODE scenemode;

            remindar.pNext = NULL;
            remindar.Remindertype = VCAL_REMINDER_AUDIO;

			tmpTime.wYear	= pCaleAnni->AnniYear;
			tmpTime.wMonth	= pCaleAnni->AnniMonth;
			tmpTime.wDay	= pCaleAnni->AnniDay;
			tmpTime.wHour	= pCaleAnni->AnniHour;
			tmpTime.wMinute = pCaleAnni->AnniMinute;
			
			CALE_GetUTCTime(&tmpTime, c_timezone, FALSE);
			
			memset(&AlarmDT, 0x0, sizeof(DT));
            remindar.pDTRun = &AlarmDT;
            sprintf(remindar.pDTRun->Date, "%04d%02d%02d", tmpTime.wYear, tmpTime.wMonth, tmpTime.wDay);
            sprintf(remindar.pDTRun->Time, "%02d%02d00", tmpTime.wHour, tmpTime.wMinute);
            
            remindar.pSnooze = &dureDT;
            remindar.Repeat = 10;
            remindar.AudioType = VCAL_AUDIO_PCM;
            remindar.ValueType = VCAL_VALUE_URL;
            memset(&scenemode, 0x0, sizeof(SCENEMODE));
		//	iIndex = GetCurSceneMode();
		//	GetSM(&scenemode, iIndex);
			Sett_GetActiveSM(&scenemode);
            remindar.pValue = scenemode.rCalendar.cMusicFileName;
			remindar.pNote = NULL;
            entity.pRemindar = &remindar;
        }
        
        memset(&repeatRule, 0, sizeof(RecuRule));
        repeatRule.FreqType = VCAL_RECU_YEARLY_DAY;
        repeatRule.Interval = 1;
        repeatRule.Duration = VCAL_RECU_FOREVER;
        entity.pRecuRule = &repeatRule;
        
        memset(&obj, 0, sizeof(obj));
        obj.pEntity = &entity;
		strcpy(obj.TimeZone, c_timezone+3);
        strcpy(obj.Version, VCAL_VERSION);
    }
    else if(itemID == CALE_EVENT)
    {
        pCaleEvent = (CaleEventNode *)pRecord;
        
		tmpTime.wYear	= pCaleEvent->EventStartYear;
		tmpTime.wMonth	= pCaleEvent->EventStartMonth;
		tmpTime.wDay	= pCaleEvent->EventStartDay;
		tmpTime.wHour	= pCaleEvent->EventStartHour;
		tmpTime.wMinute = pCaleEvent->EventStartMinute;
		
		CALE_GetUTCTime(&tmpTime, c_timezone, FALSE);
		
		sprintf(startDT.Date, "%04d%02d%02d", tmpTime.wYear, tmpTime.wMonth, tmpTime.wDay);
        sprintf(startDT.Time, "%02d%02d00", tmpTime.wHour, tmpTime.wMinute);
		
		tmpTime.wYear	= pCaleEvent->EventEndYear;
		tmpTime.wMonth	= pCaleEvent->EventEndMonth;
		tmpTime.wDay	= pCaleEvent->EventEndDay;
		tmpTime.wHour	= pCaleEvent->EventEndHour;
		tmpTime.wMinute = pCaleEvent->EventEndMinute;
		
		CALE_GetUTCTime(&tmpTime, c_timezone, FALSE);
		
		sprintf(endDT.Date, "%04d%02d%02d", tmpTime.wYear, tmpTime.wMonth, tmpTime.wDay);
        sprintf(endDT.Time, "%02d%02d00", tmpTime.wHour, tmpTime.wMinute);

        memset(&entity, 0, sizeof(entity));
        entity.EntityType = VCAL_VEVENT;
        entity.pDTStart = &startDT;
        entity.pDTEnd = &endDT;
        entity.pDTCreated = &startDT;
        entity.pDTLast_Modified = &startDT;

        memset(&pCont, 0, sizeof(Content));
        pCont.Charset = VCAL_CHARSET_UTF_8;
//        pCont.Valuelen = MultiByteToUTF8(CP_ACP, 0, pCaleEvent->EventSubject, -1,
//            szDescBuff, 100);
        pCont.Valuelen = MultiByteToUTF8(CP_ACP, 0, ML("memo"), -1,
            szDescBuff, 100);
        pCont.Valuelen --;
        pCont.pValue = szDescBuff;
        entity.pDescription = &pCont;

        memset(&pSum, 0x0, sizeof(Content));
        pSum.Charset = VCAL_CHARSET_UTF_8;
//        pSum.Valuelen = MultiByteToUTF8(CP_ACP, 0, "event", -1,
//            szSumBuff, 20);
        pSum.Valuelen = MultiByteToUTF8(CP_ACP, 0, pCaleEvent->EventSubject, -1,
            szSumBuff, 20);
        pSum.Valuelen --;
        pSum.pValue = szSumBuff;
        entity.pSummary = &pSum;

        memset(&pCate, 0x0, sizeof(Categories));
        pCate.Category = VCAL_CAT_OTHER;
        pCate.pCategory = "event";
        pCate.pNext = NULL;
        entity.pCategories = &pCate;

        entity.Class = VCAL_CLASS_PRIVATE;

        memset(&dureDT, 0, sizeof(TDuration));
        dureDT.Weekly = FALSE;
        dureDT.Minute = 1;
        
        memset(&obj, 0, sizeof(obj));
        obj.pEntity = &entity;
		strcpy(obj.TimeZone, c_timezone+3);
        strcpy(obj.Version, VCAL_VERSION);
    }

    if (vCal_Writer(&obj, &pDataBuffer, &nDataLen) == VCAL_FAILURE)
    {
        return FALSE;
    }
	if(NULL == pDataBuffer)
		return FALSE;
    
    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);

    remove(CALE_VCALE_SEND_FILE);

    hFile = open(CALE_VCALE_SEND_FILE, O_RDWR | O_CREAT, S_IRWXU);
    if(hFile < 0)
    {
        vCal_Clear(VCAL_OPT_WRITER, pDataBuffer);
        chdir(szOutPath);
        return FALSE;
    }
	
    write(hFile, pDataBuffer, nDataLen);
    close(hFile);
    chdir(szOutPath);

    vCal_Clear(VCAL_OPT_WRITER, pDataBuffer);
    
    return TRUE;
}

BOOL CALE_DecodeVcalendar(vCal_Obj *pObj, int *CaleType)
{
    vCal_entity	*pEntity;
	Remindar	*pRemindar;
	RecuRule	*pRepeatRule;
	DT			*pStartDT, *pEndDT;

    char szSumBuff[21];
    char buffer[10];
	SYSTEMTIME tmpDate;

	if (pObj == NULL)
    {
        return FALSE;
    }

	pEntity = pObj->pEntity;

	CALE_Print("\r\n[calendar]: Decoding vCalendar, pEntity = 0x%08x\r\n", pEntity);

	if(NULL == pEntity)
		return FALSE;

	pStartDT = pEntity->pDTStart;
	pEndDT = pEntity->pDTEnd;

	CALE_Print("\r\n[calendar]: Decoding vCalendar, pStartDT = 0x%08x\r\n", pStartDT);

	if(NULL == pStartDT)
		return FALSE;

	if(pEntity->pCategories)
	{
		CALE_Print("\r\n[calendar]: Category = %d\r\n", pEntity->pCategories->Category);
		if(pEntity->pCategories->pCategory)
			CALE_Print("\r\n[calendar]: pCategory = <%s>\r\n", pEntity->pCategories->pCategory);
	}

    if(pEntity->pCategories == NULL)
    {
        *CaleType = CALE_MEETING;
		CALE_Print("\r\n[calendar]: vCalendar type is [Meeting] (pEntity->pCategories == NULL)\r\n");
    }
    else if(pEntity->pCategories->Category == VCAL_CAT_OTHER)
    {
        memset(szSumBuff, 0x0, 21);
        UTF8ToMultiByte(CP_ACP, 0, pEntity->pCategories->pCategory,
            strlen(pEntity->pCategories->pCategory), szSumBuff, 20, NULL, NULL);

        if(stricmp(szSumBuff, "anniversary") == 0)
        {
            *CaleType = CALE_ANNI;
			CALE_Print("\r\n[calendar]: vCalendar type is [Anniversary]\r\n");
        }
        else if(stricmp(szSumBuff, "event") == 0)
        {
            *CaleType = CALE_EVENT;
			CALE_Print("\r\n[calendar]: vCalendar type is [Event]\r\n");
        }
        else
        {
            if(pEntity->pRecuRule->FreqType == VCAL_RECU_YEARLY_DAY)
            {
                *CaleType = CALE_ANNI;
				CALE_Print("\r\n[calendar]: vCalendar type is [Anniversary]\r\n");
            }
            else
            {
                *CaleType = CALE_MEETING;
				CALE_Print("\r\n[calendar]: vCalendar type is [Meeting] (else, else)\r\n");
            }
        }
    }
    else if((pEntity->pCategories->Category == VCAL_CAT_APPOINT) ||
        (pEntity->pCategories->Category == VCAL_CAT_MEETING))
    {
        *CaleType = CALE_MEETING;
		CALE_Print("\r\n[calendar]: vCalendar type is [Meeting] (appiont or meeting)\r\n");
    }
    else
    {
        if(pEntity->pRecuRule != NULL)
        {
            if(pEntity->pRecuRule->FreqType == VCAL_RECU_YEARLY_DAY)
            {
                *CaleType = CALE_ANNI;
				CALE_Print("\r\n[calendar]: vCalendar type is [Anniversary]\r\n");
            }
            else
            {
                *CaleType = CALE_MEETING;
				CALE_Print("\r\n[calendar]: vCalendar type is [Meeting] (pRecuRule != NULL)\r\n");
            }
        }
        else
        {
            *CaleType = CALE_MEETING;
			CALE_Print("\r\n[calendar]: vCalendar type is [Meeting] (pRecuRule == NULL)\r\n");
        }
    }

	if(*CaleType == CALE_MEETING)
	{
		SYSTEMTIME StartDate;
		SYSTEMTIME BeforeDate;

		if(NULL == pEndDT)
			return FALSE;
		
		memset(&CaleMeet, 0x0, sizeof(CaleMeetingNode));
		memset(&StartDate, 0x0, sizeof(SYSTEMTIME));
		memset(&BeforeDate, 0x0, sizeof(SYSTEMTIME));

		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Date, 4);
	//	CaleMeet.MeetStartYear = StringToInt(buffer);
		StartDate.wYear = StringToInt(buffer);
		if((StartDate.wYear < MIN_YEAR) || (StartDate.wYear > MAX_YEAR))
		{
			return FALSE;
		}
	//	StartDate.wYear = (WORD)CaleMeet.MeetStartYear;

		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Date + 4, 2);
		StartDate.wMonth = StringToInt(buffer);
	//	StartDate.wMonth = (WORD)CaleMeet.MeetStartMonth;
		
		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Date + 6, 2);
		StartDate.wDay = StringToInt(buffer);
	//	StartDate.wDay = (WORD)CaleMeet.MeetStartDay;
		
		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Time, 2);
		StartDate.wHour = StringToInt(buffer);
	//	StartDate.wHour = (WORD)CaleMeet.MeetStartHour;
		
		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Time + 2, 2);
		StartDate.wMinute = StringToInt(buffer);
	//	StartDate.wMinute = (WORD)CaleMeet.MeetStartMinute;
		
		memcpy(&tmpDate, &StartDate, sizeof(SYSTEMTIME));
		CALE_GetUTCTime(&tmpDate, pObj->TimeZone, TRUE);
		
		CaleMeet.MeetStartYear		= tmpDate.wYear;
		CaleMeet.MeetStartMonth		= tmpDate.wMonth;
		CaleMeet.MeetStartDay		= tmpDate.wDay;
		CaleMeet.MeetStartHour		= tmpDate.wHour;
		CaleMeet.MeetStartMinute	= tmpDate.wMinute;


		memset(buffer, 0, 10);
		strncpy(buffer, pEndDT->Date, 4);
		tmpDate.wYear = StringToInt(buffer);
		if((tmpDate.wYear < MIN_YEAR) || (tmpDate.wYear > MAX_YEAR))
		{
			return FALSE;
		}

		memset(buffer, 0, 10);
		strncpy(buffer, pEndDT->Date + 4, 2);
		tmpDate.wMonth = StringToInt(buffer);
		
		memset(buffer, 0, 10);
		strncpy(buffer, pEndDT->Date + 6, 2);
		tmpDate.wDay = StringToInt(buffer);
		
		memset(buffer, 0, 10);
		strncpy(buffer, pEndDT->Time, 2);
		tmpDate.wHour = StringToInt(buffer);
		
		memset(buffer, 0, 10);
		strncpy(buffer, pEndDT->Time + 2, 2);
		tmpDate.wMinute = StringToInt(buffer);

		CALE_GetUTCTime(&tmpDate, pObj->TimeZone, TRUE);
		
		CaleMeet.MeetEndYear	= tmpDate.wYear;
		CaleMeet.MeetEndMonth	= tmpDate.wMonth;
		CaleMeet.MeetEndDay		= tmpDate.wDay;
		CaleMeet.MeetEndHour	= tmpDate.wHour;
		CaleMeet.MeetEndMinute	= tmpDate.wMinute;

//		if(NULL != pEntity->pDescription)
//		{
//			UTF8ToMultiByte(CP_ACP, 0, pEntity->pDescription->pValue,
//				pEntity->pDescription->Valuelen, CaleMeet.MeetSubject, MAX_CALE_SUBJECT_LEN, NULL, NULL);
//		}
		if(NULL != pEntity->pSummary)
		{
			UTF8ToMultiByte(CP_ACP, 0, pEntity->pSummary->pValue,
				pEntity->pSummary->Valuelen, CaleMeet.MeetSubject, MAX_CALE_SUBJECT_LEN, NULL, NULL);
		}
		if(NULL != pEntity->pLocation)
		{
			UTF8ToMultiByte(CP_ACP, 0, pEntity->pLocation->pValue,
				pEntity->pLocation->Valuelen, CaleMeet.MeetLocation, MAX_CALE_LOCATION_LEN, NULL, NULL);
		}
		
		if (pEntity->pRemindar != NULL)
		{
			int iBew;

			pRemindar = pEntity->pRemindar;
			CaleMeet.MeetAlarmTimes = pRemindar->Repeat;

			memset(buffer, 0, 10);
			strncpy(buffer, pRemindar->pDTRun->Date, 4);
			BeforeDate.wYear = (WORD)StringToInt(buffer);
			
			memset(buffer, 0, 10);
			strncpy(buffer, pRemindar->pDTRun->Date + 4, 2);
			BeforeDate.wMonth = (WORD)StringToInt(buffer);
			
			memset(buffer, 0, 10);
			strncpy(buffer, pRemindar->pDTRun->Date + 6, 2);
			BeforeDate.wDay = (WORD)StringToInt(buffer);
			
			memset(buffer, 0, 10);
			strncpy(buffer, pRemindar->pDTRun->Time, 2);
			BeforeDate.wHour = (WORD)StringToInt(buffer);
			
			memset(buffer, 0, 10);
			strncpy(buffer, pRemindar->pDTRun->Time + 2, 2);
			BeforeDate.wMinute = (WORD)StringToInt(buffer);

			iBew = CALE_CalcBeforeAlarm(&StartDate, &BeforeDate);
			if(iBew == -1)
			{
				CaleMeet.MeetAlarmFlag = CALE_ON_TIME;
			}
			else
			{
				CaleMeet.MeetAlarmFlag = iBew;
			}
		}
		else
		{
			CaleMeet.MeetAlarmFlag = CALE_NO_ALARM;
		}
		
		if (pEntity->pRecuRule != NULL)
		{
			pRepeatRule = pEntity->pRecuRule;
			CaleMeet.MeetRepeatFlag = 1;
			switch (pRepeatRule->FreqType)
			{
			case VCAL_RECU_DAILY:
				CaleMeet.MeetRepeatStyle = CALE_REP_DAY;
				break;

			case VCAL_RECU_WEEKLY:
				CaleMeet.MeetRepeatStyle = CALE_REP_WEEK;
				break;

			case VCAL_RECU_MONTHLY_DAY:
				CaleMeet.MeetRepeatStyle = CALE_REP_MONTH;
				break;

			case VCAL_RECU_YEARLY_DAY:
				CaleMeet.MeetRepeatStyle = CALE_REP_YEAR;
				break;

			default:
				CaleMeet.MeetRepeatStyle = CALE_REP_NONE;
				break;
			}
			
			switch (pRepeatRule->Duration)
			{
			case 0:
				{
					CaleMeet.MeetRepeatFlag = 0;
					CaleMeet.MeetRepeatStyle = CALE_REP_NONE;
				}
				break;
				
			default:
				CaleMeet.MeetRepeatFlag = 1;
				break;
			}
		}
		CaleMeet.LocalFlag = CALE_LOCAL;
	}
	else if(*CaleType == CALE_ANNI)
	{
		memset(&CaleAnni, 0x0, sizeof(CaleAnniNode));
		
		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Date, 4);
	//	CaleAnni.AnniDisYear = StringToInt(buffer);
		tmpDate.wYear = StringToInt(buffer);
		
		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Date + 4, 2);
		tmpDate.wMonth = StringToInt(buffer);
		
		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Date + 6, 2);
		tmpDate.wDay = StringToInt(buffer);
		
		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Time, 2);
		tmpDate.wHour = StringToInt(buffer);
		
		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Time + 2, 2);
		tmpDate.wMinute = StringToInt(buffer);

		CALE_GetUTCTime(&tmpDate, pObj->TimeZone, TRUE);

		CaleAnni.AnniDisYear	= tmpDate.wYear;
		CaleAnni.AnniDisMonth	= tmpDate.wMonth;
		CaleAnni.AnniDisDay		= tmpDate.wDay;
		CaleAnni.AnniHour		= tmpDate.wHour;
		CaleAnni.AnniMinute		= tmpDate.wMinute;

//		if(NULL != pEntity->pDescription)
//		{
//			UTF8ToMultiByte(CP_ACP, 0, pEntity->pDescription->pValue,
//				pEntity->pDescription->Valuelen, CaleAnni.AnniOccasion, MAX_CALE_OCCASION_LEN, NULL, NULL);
//		}
		if(NULL != pEntity->pSummary)
		{
			UTF8ToMultiByte(CP_ACP, 0, pEntity->pSummary->pValue,
				pEntity->pSummary->Valuelen, CaleAnni.AnniOccasion, MAX_CALE_OCCASION_LEN, NULL, NULL);
		}
		
		if (pEntity->pRemindar != NULL)
		{
			pRemindar = pEntity->pRemindar;
			CaleAnni.AnniAlarmFlag = 1;
			CaleAnni.AnniAlarmTimes = pRemindar->Repeat;

			memset(buffer, 0, 10);
			strncpy(buffer, pRemindar->pDTRun->Date, 4);
			tmpDate.wYear = StringToInt(buffer);
			
			memset(buffer, 0, 10);
			strncpy(buffer, pRemindar->pDTRun->Date + 4, 2);
			tmpDate.wMonth = StringToInt(buffer);
			
			memset(buffer, 0, 10);
			strncpy(buffer, pRemindar->pDTRun->Date + 6, 2);
			tmpDate.wDay = StringToInt(buffer);
			
			memset(buffer, 0, 10);
			strncpy(buffer, pRemindar->pDTRun->Time, 2);
			tmpDate.wHour = StringToInt(buffer);
			
			memset(buffer, 0, 10);
			strncpy(buffer, pRemindar->pDTRun->Time + 2, 2);
			tmpDate.wMinute = StringToInt(buffer);

			CALE_GetUTCTime(&tmpDate, pObj->TimeZone, TRUE);

			CaleAnni.AnniYear	= tmpDate.wYear;
			CaleAnni.AnniMonth	= tmpDate.wMonth;
			CaleAnni.AnniDay	= tmpDate.wDay;
			CaleAnni.AnniHour	= tmpDate.wHour;
			CaleAnni.AnniMinute	= tmpDate.wMinute;
		}
		CaleAnni.LocalFlag = CALE_LOCAL;
		CALE_Print("\r\n[calendar]: Anniversary decoded!\r\n");
	}
	else if(*CaleType == CALE_EVENT)
	{
		if(NULL == pEndDT)
			return FALSE;
		
		memset(&CaleEvent, 0x0, sizeof(CaleEventNode));
		
		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Date, 4);
		tmpDate.wYear = StringToInt(buffer);
		if((tmpDate.wYear < MIN_YEAR) || (tmpDate.wYear > MAX_YEAR))
		{
			return FALSE;
		}
		
		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Date + 4, 2);
		tmpDate.wMonth = StringToInt(buffer);
		
		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Date + 6, 2);
		tmpDate.wDay = StringToInt(buffer);
		
		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Time, 2);
		tmpDate.wHour = StringToInt(buffer);
		
		memset(buffer, 0, 10);
		strncpy(buffer, pStartDT->Time + 2, 2);
		tmpDate.wMinute = StringToInt(buffer);

		CALE_GetUTCTime(&tmpDate, pObj->TimeZone, TRUE);

		CaleEvent.EventStartYear	= tmpDate.wYear;
		CaleEvent.EventStartMonth	= tmpDate.wMonth;
		CaleEvent.EventStartDay		= tmpDate.wDay;
		CaleEvent.EventStartHour	= tmpDate.wHour;
		CaleEvent.EventStartMinute	= tmpDate.wMinute;

		memset(buffer, 0, 10);
		strncpy(buffer, pEndDT->Date, 4);
		tmpDate.wYear = StringToInt(buffer);
		if((tmpDate.wYear < MIN_YEAR) || (tmpDate.wYear > MAX_YEAR))
		{
			return FALSE;
		}

		memset(buffer, 0, 10);
		strncpy(buffer, pEndDT->Date + 4, 2);
		tmpDate.wMonth = StringToInt(buffer);
		
		memset(buffer, 0, 10);
		strncpy(buffer, pEndDT->Date + 6, 2);
		tmpDate.wDay = StringToInt(buffer);
		
		memset(buffer, 0, 10);
		strncpy(buffer, pEndDT->Time, 2);
		tmpDate.wHour = StringToInt(buffer);
		
		memset(buffer, 0, 10);
		strncpy(buffer, pEndDT->Time + 2, 2);
		tmpDate.wMinute = StringToInt(buffer);
		
		CALE_GetUTCTime(&tmpDate, pObj->TimeZone, TRUE);

		CaleEvent.EventEndYear		= tmpDate.wYear;
		CaleEvent.EventEndMonth		= tmpDate.wMonth;
		CaleEvent.EventEndDay		= tmpDate.wDay;
		CaleEvent.EventEndHour		= tmpDate.wHour;
		CaleEvent.EventEndMinute	= tmpDate.wMinute;

//		if(NULL != pEntity->pDescription)
//		{
//			UTF8ToMultiByte(CP_ACP, 0, pEntity->pDescription->pValue,
//				pEntity->pDescription->Valuelen, CaleEvent.EventSubject, MAX_CALE_SUBJECT_LEN, NULL, NULL);
//		}
		if(NULL != pEntity->pSummary)
		{
			UTF8ToMultiByte(CP_ACP, 0, pEntity->pSummary->pValue,
				pEntity->pSummary->Valuelen, CaleEvent.EventSubject, MAX_CALE_SUBJECT_LEN, NULL, NULL);
		}
		CaleEvent.LocalFlag = CALE_LOCAL;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CALE_DecodeVcalToDisplay(vCal_Obj *pObj, VCARDVCAL **ppChain)
{
    VCARDVCAL *pNewChain;
    VCARDVCAL *pCur;
    int CaleType;

    SYSTEMTIME TmpSys;
    char TmpTime[15];
    char TmpDate[15];

    *ppChain = NULL;
    if(!CALE_DecodeVcalendar(pObj, &CaleType))
    {
        return FALSE;
    }
    
    //subject
    pNewChain = NULL;
    pNewChain = (VCARDVCAL*)malloc(sizeof(VCARDVCAL));
    if(pNewChain == NULL)
    {
        CALE_VcaleChainEarse(*ppChain);
        return FALSE;
    }
    memset(pNewChain,0,sizeof(VCARDVCAL));
    pNewChain->pszTitle = (char*)malloc(strlen(IDP_IME_SUBJECT) + 1);
    if(pNewChain->pszTitle == NULL)  
    {
        CALE_VcaleChainEarse(*ppChain);
        return FALSE;
    }
    memset(pNewChain->pszTitle, 0x0, strlen(IDP_IME_SUBJECT) + 1);
    strcpy(pNewChain->pszTitle, IDP_IME_SUBJECT);
    pNewChain->pszContent = (char*)malloc(MAX_CALE_SUBJECT_LEN + 1);
    if(pNewChain->pszContent == NULL)  
    {
        CALE_VcaleChainEarse(*ppChain);
        return FALSE;
    }
    memset(pNewChain->pszContent, 0x0, MAX_CALE_SUBJECT_LEN + 1);
    if(CaleType == CALE_MEETING)
    {
        strcpy(pNewChain->pszContent, CaleMeet.MeetSubject);
    }
    else if(CaleType == CALE_ANNI)
    {
        strcpy(pNewChain->pszContent, CaleAnni.AnniOccasion);
    }
    else if(CaleType == CALE_EVENT)
    {
        strcpy(pNewChain->pszContent, CaleEvent.EventSubject);
    }
    else
    {
        return FALSE;
    }
    *ppChain = pNewChain;
    pCur = pNewChain;
        
    //location
    if(CaleType == CALE_MEETING)
    {
        pNewChain = NULL;
        pNewChain = (VCARDVCAL*)malloc(sizeof(VCARDVCAL));
        if(pNewChain == NULL)
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain,0,sizeof(VCARDVCAL));
        pNewChain->pszTitle = (char*)malloc(strlen(IDP_IME_LOCATION) + 1);
        if(pNewChain->pszTitle == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain->pszTitle, 0x0, strlen(IDP_IME_LOCATION) + 1);
        strcpy(pNewChain->pszTitle, IDP_IME_LOCATION);
        pNewChain->pszContent = (char*)malloc(MAX_CALE_LOCATION_LEN + 1);
        if(pNewChain->pszContent == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain->pszContent, 0x0, MAX_CALE_LOCATION_LEN + 1);
        strcpy(pNewChain->pszContent, CaleMeet.MeetLocation);

        pCur->pNext = pNewChain;
        pCur = pNewChain;
    }

    //startdate and starttime
    if(CaleType == CALE_MEETING)
    {
        TmpSys.wYear = (WORD)CaleMeet.MeetStartYear;
        TmpSys.wMonth = (WORD)CaleMeet.MeetStartMonth;
        TmpSys.wDay = (WORD)CaleMeet.MeetStartDay;
        TmpSys.wHour = (WORD)CaleMeet.MeetStartHour;
        TmpSys.wMinute = (WORD)CaleMeet.MeetStartMinute;
    }
    else if(CaleType == CALE_ANNI)
    {
        TmpSys.wYear = (WORD)CaleAnni.AnniDisYear;
        TmpSys.wMonth = (WORD)CaleAnni.AnniDisMonth;
        TmpSys.wDay = (WORD)CaleAnni.AnniDisDay;
        TmpSys.wHour = (WORD)CaleAnni.AnniHour;
        TmpSys.wMinute = (WORD)CaleAnni.AnniMinute;
    }
    else if(CaleType == CALE_EVENT)
    {
        TmpSys.wYear = (WORD)CaleEvent.EventStartYear;
        TmpSys.wMonth = (WORD)CaleEvent.EventStartMonth;
        TmpSys.wDay = (WORD)CaleEvent.EventStartDay;
        TmpSys.wHour = (WORD)CaleEvent.EventStartHour;
        TmpSys.wMinute = (WORD)CaleEvent.EventStartMinute;
    }
    
    memset(TmpDate, 0x0, 15);
    memset(TmpTime, 0x0, 15);
    GetTimeDisplay(TmpSys, TmpTime, TmpDate);

    pNewChain = NULL;
    pNewChain = (VCARDVCAL*)malloc(sizeof(VCARDVCAL));
    if(pNewChain == NULL)
    {
        CALE_VcaleChainEarse(*ppChain);
        return FALSE;
    }
    memset(pNewChain,0,sizeof(VCARDVCAL));
    pNewChain->pszTitle = (char*)malloc(strlen(IDP_IME_STARTDATE) + 1);
    if(pNewChain->pszTitle == NULL)  
    {
        CALE_VcaleChainEarse(*ppChain);
        return FALSE;
    }
    memset(pNewChain->pszTitle, 0x0, strlen(IDP_IME_STARTDATE) + 1);
    strcpy(pNewChain->pszTitle, IDP_IME_STARTDATE);
    pNewChain->pszContent = (char*)malloc(15);
    if(pNewChain->pszContent == NULL)  
    {
        CALE_VcaleChainEarse(*ppChain);
        return FALSE;
    }
    memset(pNewChain->pszContent, 0x0, 15);
    strcpy(pNewChain->pszContent, TmpDate);
        
    pCur->pNext = pNewChain;
    pCur = pNewChain;

    if(CaleType == CALE_MEETING)
    {
        pNewChain = NULL;
        pNewChain = (VCARDVCAL*)malloc(sizeof(VCARDVCAL));
        if(pNewChain == NULL)
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain,0,sizeof(VCARDVCAL));
        pNewChain->pszTitle = (char*)malloc(strlen(IDP_IME_STARTTIME) + 1);
        if(pNewChain->pszTitle == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain->pszTitle, 0x0, strlen(IDP_IME_STARTTIME) + 1);
        strcpy(pNewChain->pszTitle, IDP_IME_STARTTIME);
        pNewChain->pszContent = (char*)malloc(15);
        if(pNewChain->pszContent == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain->pszContent, 0x0, 15);
        strcpy(pNewChain->pszContent, TmpTime);
        
        pCur->pNext = pNewChain;
        pCur = pNewChain;
    }
    
    //enddate and endtime
    if(CaleType == CALE_MEETING)
    {
        TmpSys.wYear = (WORD)CaleMeet.MeetEndYear;
        TmpSys.wMonth = (WORD)CaleMeet.MeetEndMonth;
        TmpSys.wDay = (WORD)CaleMeet.MeetEndDay;
        TmpSys.wHour = (WORD)CaleMeet.MeetEndHour;
        TmpSys.wMinute = (WORD)CaleMeet.MeetEndMinute;
    }
    else if(CaleType == CALE_ANNI)
    {
        TmpSys.wYear = (WORD)CaleAnni.AnniYear;
        TmpSys.wMonth = (WORD)CaleAnni.AnniMonth;
        TmpSys.wDay = (WORD)CaleAnni.AnniDay;
        TmpSys.wHour = (WORD)CaleAnni.AnniHour;
        TmpSys.wMinute = (WORD)CaleAnni.AnniMinute;
    }
    else if(CaleType == CALE_EVENT)
    {
        TmpSys.wYear = (WORD)CaleEvent.EventEndYear;
        TmpSys.wMonth = (WORD)CaleEvent.EventEndMonth;
        TmpSys.wDay = (WORD)CaleEvent.EventEndDay;
        TmpSys.wHour = (WORD)CaleEvent.EventEndHour;
        TmpSys.wMinute = (WORD)CaleEvent.EventEndMinute;
    }
    
    memset(TmpDate, 0x0, 15);
    memset(TmpTime, 0x0, 15);
    GetTimeDisplay(TmpSys, TmpTime, TmpDate);
    
    pNewChain = NULL;
    pNewChain = (VCARDVCAL*)malloc(sizeof(VCARDVCAL));
    if(pNewChain == NULL)
    {
        CALE_VcaleChainEarse(*ppChain);
        return FALSE;
    }
    memset(pNewChain,0,sizeof(VCARDVCAL));

    if((CaleType == CALE_MEETING) || (CaleType == CALE_EVENT))
    {
        pNewChain->pszTitle = (char*)malloc(strlen(IDP_IME_ENDDATE) + 1);
        if(pNewChain->pszTitle == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain->pszTitle, 0x0, strlen(IDP_IME_ENDDATE) + 1);
        strcpy(pNewChain->pszTitle, IDP_IME_ENDDATE);
        pNewChain->pszContent = (char*)malloc(15);
        if(pNewChain->pszContent == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain->pszContent, 0x0, 15);
        strcpy(pNewChain->pszContent, TmpDate);
    }
    else if(CaleType == CALE_ANNI)
    {
        char cTmpDis[17];

        pNewChain->pszTitle = (char*)malloc(strlen(IDP_IME_ALARM) + 1);
        if(pNewChain->pszTitle == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain->pszTitle, 0x0, strlen(IDP_IME_ALARM) + 1);
        strcpy(pNewChain->pszTitle, IDP_IME_ALARM);
        pNewChain->pszContent = (char*)malloc(15);
        if(pNewChain->pszContent == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(cTmpDis, 0x0, sizeof(char) * 25);
        memset(pNewChain->pszContent, 0x0, 15);
		if(CaleAnni.AnniAlarmFlag == 1)
		{
		//	CALE_DateTimeSysToChar(&TmpSys, cTmpDis);
			CALE_GetFormatedFullDate(&TmpSys, cTmpDis);
			strcpy(pNewChain->pszContent, cTmpDis);
		}
		else
			strcpy(pNewChain->pszContent, IDP_IME_OFF);
    }
    
    pCur->pNext = pNewChain;
    pCur = pNewChain;
    
    if(CaleType == CALE_MEETING)
    {
        pNewChain = NULL;
        pNewChain = (VCARDVCAL*)malloc(sizeof(VCARDVCAL));
        if(pNewChain == NULL)
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain,0,sizeof(VCARDVCAL));
        pNewChain->pszTitle = (char*)malloc(strlen(IDP_IME_ENDTIME) + 1);
        if(pNewChain->pszTitle == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain->pszTitle, 0x0, strlen(IDP_IME_ENDTIME) + 1);
        strcpy(pNewChain->pszTitle, IDP_IME_ENDTIME);
        pNewChain->pszContent = (char*)malloc(15);
        if(pNewChain->pszContent == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain->pszContent, 0x0, 15);
        strcpy(pNewChain->pszContent, TmpTime);
        
        pCur->pNext = pNewChain;
        pCur = pNewChain;
    }

    //repeat and alarm
    if(CaleType == CALE_MEETING)
    {
        pNewChain = NULL;
        pNewChain = (VCARDVCAL*)malloc(sizeof(VCARDVCAL));
        if(pNewChain == NULL)
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain,0,sizeof(VCARDVCAL));
        pNewChain->pszTitle = (char*)malloc(strlen(IDP_IME_REPEAT) + 1);
        if(pNewChain->pszTitle == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain->pszTitle, 0x0, strlen(IDP_IME_REPEAT) + 1);
        strcpy(pNewChain->pszTitle, IDP_IME_REPEAT);
        pNewChain->pszContent = (char*)malloc(15);
        if(pNewChain->pszContent == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain->pszContent, 0x0, 15);
        if(CaleMeet.MeetRepeatStyle == CALE_REP_NONE)
            strcpy(pNewChain->pszContent, IDP_IME_NONE);
        else if(CaleMeet.MeetRepeatStyle == CALE_REP_DAY)
            strcpy(pNewChain->pszContent, IDP_IME_EVERYDAY);
        else if(CaleMeet.MeetRepeatStyle == CALE_REP_WEEK)
            strcpy(pNewChain->pszContent, IDP_IME_EVERYWEEK);
        else if(CaleMeet.MeetRepeatStyle == CALE_REP_TWOWEEK)
            strcpy(pNewChain->pszContent, IDP_IME_EVERYTWOWEEK);
        else if(CaleMeet.MeetRepeatStyle == CALE_REP_MONTH)
            strcpy(pNewChain->pszContent, IDP_IME_EVERYMONTH);
//        else if(CaleMeet.MeetRepeatStyle == CALE_REP_YEAR)
//            strcpy(pNewChain->pszContent, IDP_IME_EVERYYEAR);
        
        pCur->pNext = pNewChain;
        pCur = pNewChain;

        pNewChain = NULL;
        pNewChain = (VCARDVCAL*)malloc(sizeof(VCARDVCAL));
        if(pNewChain == NULL)
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain,0,sizeof(VCARDVCAL));
        pNewChain->pszTitle = (char*)malloc(strlen(IDP_IME_ALARM) + 1);
        if(pNewChain->pszTitle == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain->pszTitle, 0x0, strlen(IDP_IME_ALARM) + 1);
        strcpy(pNewChain->pszTitle, IDP_IME_ALARM);
        pNewChain->pszContent = (char*)malloc(15);
        if(pNewChain->pszContent == NULL)  
        {
            CALE_VcaleChainEarse(*ppChain);
            return FALSE;
        }
        memset(pNewChain->pszContent, 0x0, 15);
        if(CaleMeet.MeetAlarmFlag == CALE_NO_ALARM)
            strcpy(pNewChain->pszContent, IDP_ALARM_NO);
        else if(CaleMeet.MeetAlarmFlag == CALE_ON_TIME)
            strcpy(pNewChain->pszContent, IDP_ALARM_ON);
        else if(CaleMeet.MeetAlarmFlag == CALE_BEFORE_5MIN)
            strcpy(pNewChain->pszContent, IDP_ALARM_5MIN);
        else if(CaleMeet.MeetAlarmFlag == CALE_BEFORE_10MIN)
            strcpy(pNewChain->pszContent, IDP_ALARM_10MIN);
        else if(CaleMeet.MeetAlarmFlag == CALE_BEFORE_15MIN)
            strcpy(pNewChain->pszContent, IDP_ALARM_15MIN);
        else if(CaleMeet.MeetAlarmFlag == CALE_BEFORE_30MIN)
            strcpy(pNewChain->pszContent, IDP_ALARM_30MIN);
        else if(CaleMeet.MeetAlarmFlag == CALE_BEFORE_45MIN)
            strcpy(pNewChain->pszContent, IDP_ALARM_45MIN);
        else if(CaleMeet.MeetAlarmFlag == CALE_BEFORE_1HOUR)
            strcpy(pNewChain->pszContent, IDP_ALARM_1HOUR);
        else if(CaleMeet.MeetAlarmFlag == CALE_BEFORE_6HOUR)
            strcpy(pNewChain->pszContent, IDP_ALARM_6HOUR);
        else if(CaleMeet.MeetAlarmFlag == CALE_BEFORE_12HOUR)
            strcpy(pNewChain->pszContent, IDP_ALARM_12HOUR);
        else if(CaleMeet.MeetAlarmFlag == CALE_BEFORE_1DAY)
            strcpy(pNewChain->pszContent, IDP_ALARM_1DAY);
        else if(CaleMeet.MeetAlarmFlag == CALE_BEFORE_1WEEK)
            strcpy(pNewChain->pszContent, IDP_ALARM_1WEEK);
        
        pCur->pNext = pNewChain;
        pCur = pNewChain;
    }
 
    return TRUE;
}

void CALE_VcaleChainEarse(VCARDVCAL *pChain)
{
    VCARDVCAL* p;
    VCARDVCAL* ptemp;

    p = pChain;
    while(p)
    {
        ptemp = p->pNext;
        if(p->pszTitle)
            GARY_FREE(p->pszTitle);
        if(p->pszContent)
            GARY_FREE(p->pszContent);
        GARY_FREE(p);
        p = ptemp;
    }
    return;
}

BOOL CALE_SaveFromSms(vCal_Obj *pObj)
{
    int CaleType;

	CALE_Print("\r\n[calendar]: savefromsms, pObj = 0x%08x\r\n", pObj);
	
	if(NULL == pObj)
		return FALSE;

    if(!CALE_DecodeVcalendar(pObj, &CaleType))
    {
        return FALSE;
    }

    if(CaleType == CALE_MEETING)
    {
		CALE_Print("\r\n[calendar]: save meeting, CaleMeet = 0x%08x\r\n", &CaleMeet);
        if(CALE_ExternAddApi(CaleType, (BYTE *)&CaleMeet) == -1)
        {
            return FALSE;
        }
    }
    else if(CaleType == CALE_ANNI)
    {
		CALE_Print("\r\n[calendar]: save anniversary, CaleAnni = 0x%08x\r\n", &CaleAnni);
        if(CALE_ExternAddApi(CaleType, (BYTE *)&CaleAnni) == -1)
        {
            return FALSE;
        }
    }
    else if(CaleType == CALE_EVENT)
    {
		CALE_Print("\r\n[calendar]: save event, CaleEvent = 0x%08x\r\n", &CaleEvent);
        if(CALE_ExternAddApi(CaleType, (BYTE *)&CaleEvent) == -1)
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
    
    return TRUE;
}

static int StringToInt(char *str)
{
    int len;
    char *p;
    
    len = strlen(str);
    p = str;
    while (*p == '0' && *p != 0)
    {
        p++;
    }
    
    return atoi(p);
}

static void CALE_CalcVcalAlarm(SYSTEMTIME *pDate, int iDay, int iHour, int iMin)
{
    SYSTEMTIME OldTime;
    
	int minute, hour;

    minute = (int)pDate->wMinute - iMin;
    hour = (int)pDate->wHour - iHour;
    OldTime.wDay = (WORD)((int)pDate->wDay);
    OldTime.wMonth = (WORD)((int)pDate->wMonth);
    OldTime.wYear = (WORD)((int)pDate->wYear);
    
    while(minute < 0)
    {
        minute += 60;
        hour --;
    }
    while(hour < 0)
    {
        hour += 24;
        iDay ++;
    }
	OldTime.wHour = hour;
	OldTime.wMinute = minute;
    if(CALE_DayReduce(&OldTime, iDay))
    {
        pDate->wYear = OldTime.wYear;
        pDate->wMonth = OldTime.wMonth;
        pDate->wDay = OldTime.wDay;
        pDate->wHour = OldTime.wHour;
        pDate->wMinute = OldTime.wMinute;
    }
    
    return;
}

static int CALE_CalcBeforeAlarm(SYSTEMTIME *pDate, SYSTEMTIME *pBefore)
{
    int iCom;
    int iBew;

    int iComDate;
    int iComTime;
    
    iCom = CALE_CmpSysDate(pDate, pBefore);
    if(iCom == Early)
    {
        return -1;
    }
    else if(iCom == Equality)
    {
        return CALE_ON_TIME;
    }
    else
    {
        iComDate = CALE_CmpOnlyDate(pBefore, pDate);
        if(iComDate == Early)
        {
            iBew = CALE_IntervalSysDate(pBefore, pDate);
            if(iBew == 7)
            {
                return CALE_BEFORE_1WEEK;
            }
            else if(iBew != 1)
            {
                return -1;
            }
            else
            {
                iComTime = CALE_CmpOnlyTime(pBefore, pDate);
                if(iComTime == Early)
                {
                    return -1;
                }
                else if(iComTime == Equality)
                {
                    return CALE_BEFORE_1DAY;
                }
                else
                {
                    int iHour;
                    int iMinute;

                    iHour = (int)pDate->wHour + 24 - (int)pBefore->wHour;
                    if(iHour < 0)
                    {
                        return -1;
                    }
                    else if(iHour == 12)
                    {
                        return CALE_BEFORE_12HOUR;
                    }
                    else if(iHour == 6)
                    {
                        return CALE_BEFORE_6HOUR;
                    }
                    else if(iHour == 1)
                    {
                        if(pDate->wMinute == pBefore->wMinute)
                        {
                            return CALE_BEFORE_1HOUR;
                        }
                        else if(pDate->wMinute > pBefore->wMinute)
                        {
                            return -1;
                        }
                        else
                        {
                            iMinute = (int)pDate->wMinute + 60 - (int)pBefore->wMinute;
                            if(iMinute == 60)
                            {
                                return CALE_BEFORE_1HOUR;
                            }
                            else if(iMinute == 45)
                            {
                                return CALE_BEFORE_45MIN;
                            }
                            else if(iMinute == 30)
                            {
                                return CALE_BEFORE_30MIN;
                            }
                            else if(iMinute == 15)
                            {
                                return CALE_BEFORE_15MIN;
                            }
                            else if(iMinute == 10)
                            {
                                return CALE_BEFORE_10MIN;
                            }
                            else if(iMinute == 5)
                            {
                                return CALE_BEFORE_5MIN;
                            }
                            else if(iMinute == 0)
                            {
                                return CALE_ON_TIME;
                            }
                            else
                            {
                                return -1;
                            }
                        }
                    }
                    else if(iHour == 0)
                    {
                        return CALE_ON_TIME;
                    }
                    else
                    {
                        return -1;
                    }
                }
            }
        }
        else if(iComDate == Equality)
        {
            iComTime = CALE_CmpOnlyTime(pBefore, pDate);
            if(iComTime == Late)
            {
                return -1;
            }
            else if(iComTime == Equality)
            {
                return CALE_ON_TIME;
            }
            else
            {
                int iHour;
                int iMinute;
                
                iHour = (int)pDate->wHour - (int)pBefore->wHour;
                if(iHour < 0)
                {
                    return -1;
                }
                else if(iHour == 12)
                {
                    return CALE_BEFORE_12HOUR;
                }
                else if(iHour == 6)
                {
                    return CALE_BEFORE_6HOUR;
                }
                else if(iHour == 1)
                {
                    if(pDate->wMinute == pBefore->wMinute)
                    {
                        return CALE_BEFORE_1HOUR;
                    }
                    else if(pDate->wMinute > pBefore->wMinute)
                    {
                        return -1;
                    }
                    else
                    {
                        iMinute = (int)pDate->wMinute + 60 - (int)pBefore->wMinute;
                        if(iMinute == 60)
                        {
                            return CALE_BEFORE_1HOUR;
                        }
                        else if(iMinute == 45)
                        {
                            return CALE_BEFORE_45MIN;
                        }
                        else if(iMinute == 30)
                        {
                            return CALE_BEFORE_30MIN;
                        }
                        else if(iMinute == 15)
                        {
                            return CALE_BEFORE_15MIN;
                        }
                        else if(iMinute == 10)
                        {
                            return CALE_BEFORE_10MIN;
                        }
                        else if(iMinute == 5)
                        {
                            return CALE_BEFORE_5MIN;
                        }
                        else if(iMinute == 0)
                        {
                            return CALE_ON_TIME;
                        }
                        else
                        {
                            return -1;
                        }
                    }
                }// else if(iHour == 1)
                else if(iHour == 0)
                {
                    return CALE_ON_TIME;
                }
                else
                {
                    return -1;
                }
            }
        }
        else
        {
            return -1;
        }
    }
}
