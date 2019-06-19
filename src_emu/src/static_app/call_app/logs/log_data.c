
#include "RcCommon.h"
#include "setting.h"
#include "phonebookext.h"

#include "MBPublic.h"
#include "MBRecordList.h"
#include "log.h"

#define MBRECORD_SAVEDATA_UNREPEAT

#define DEBUG_MBRECORDDATA

#define FILENAMEMAXLEN  40

#define AB_TEL_LEN  AB_MAXLEN_TEL




static const char * pUNANSWER_LOG = "/mnt/flash/calllogs/UnAnswerLog.dat";
static const char * pANSWERED_LOG = "/mnt/flash/calllogs/AnsweredLog.dat";
static const char * pDIALED_LOG   = "/mnt/flash/calllogs/DialedLong.dat";
static const char * pRECCOST_LOG  = "/mnt/flash/calllogs/RecCostLog.dat";

static CallRecord         * MBRecord;
static CallRecord         * pRecordUnAn;
static CallRecord         * pRecordAns; 
static CallRecord         * pRecordDial;

static MBRecordCostTime     sRecordCostTime;

static  BOOL    SaveMBRecord(const CallRecord * pCallRecNode,MBRECORDTYPE CallRecType);
static  int     InitGetMBRecord(CallRecord * pInitMBRecord,MBRECORDTYPE CallRecType);

static  BOOL    GetFileName(char * pFileName,MBRECORDTYPE iRectType)
{
    if (pFileName == NULL)
        return FALSE;

    switch(iRectType)
    {
    case UNANSWER_REC:
        strcpy(pFileName,pUNANSWER_LOG);
        break;
    case ANSWERED_REC:
        strcpy(pFileName,pANSWERED_LOG);
        break;
    case DIALED_REC:
        strcpy(pFileName,pDIALED_LOG);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

static  CallRecord * GetPointMBRecord(MBRECORDTYPE iRectType)
{
    switch(iRectType)
    {
    case UNANSWER_REC:
        return pRecordUnAn;
    case ANSWERED_REC:
        return pRecordAns;
    case DIALED_REC:
        return pRecordDial;
    default:
        return 0;
    }
}

BOOL    InitMBRecordData(void)
{
    MBRecord = (CallRecord*)malloc(sizeof(CallRecord) * MAX_PHONE_RECORD);
    if (NULL == MBRecord)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORDDATA)
        printf("mb MBRecord malloc fail\r\n");
#endif
        return FALSE;
    }
    
    pRecordUnAn = (CallRecord*)malloc(sizeof(CallRecord) * MAX_PHONE_RECORD);
    if (NULL == pRecordUnAn)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORDDATA)
        printf("mb pRecordUnAn malloc fail\r\n");
#endif
        return FALSE;
    }

    pRecordAns = (CallRecord*)malloc(sizeof(CallRecord) * MAX_PHONE_RECORD);
    if (NULL == pRecordAns)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORDDATA)
        printf("mb pRecordAns malloc fail\r\n");
#endif
        return FALSE;
    }

    pRecordDial = (CallRecord*)malloc(sizeof(CallRecord) * MAX_PHONE_RECORD);
    if (NULL == pRecordDial)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORDDATA)
        printf("mb pRecordDial malloc fail\r\n");
#endif
        return FALSE;
    }

    memset(MBRecord   ,0x00,sizeof(CallRecord) * MAX_PHONE_RECORD);
    memset(pRecordUnAn,0x00,sizeof(CallRecord) * MAX_PHONE_RECORD);
    memset(pRecordAns ,0x00,sizeof(CallRecord) * MAX_PHONE_RECORD);
    memset(pRecordDial,0x00,sizeof(CallRecord) * MAX_PHONE_RECORD);

    InitGetMBRecord(pRecordUnAn,UNANSWER_REC);
    InitGetMBRecord(pRecordAns ,ANSWERED_REC);
    InitGetMBRecord(pRecordDial,DIALED_REC  );

    return TRUE;
}

static  int     InitGetMBRecord(CallRecord * pInitMBRecord,MBRECORDTYPE CallRecType)
{
	FILE * file;
	char    FileName[FILENAMEMAXLEN] = "";
    int     i;
    DWORD   dfsize = 0;
    if (!GetFileName(FileName,CallRecType))
        return -1;
    memset(MBRecord,0x00,sizeof(CallRecord) * MAX_PHONE_RECORD);
	file = fopen(FileName, "r");
	if (file == NULL) {
        file = fopen(FileName, "w+");
		if (file == NULL)
			return -1;
		else{
			dfsize = fwrite(MBRecord, sizeof(CallRecord), MAX_PHONE_RECORD, file);
			fclose(file);
			memcpy(pInitMBRecord,MBRecord,sizeof(CallRecord) * MAX_PHONE_RECORD);
			return 0;
		}
	}
	else{
		dfsize = fread(MBRecord, sizeof(CallRecord), MAX_PHONE_RECORD, file);
		fclose(file);
		memcpy(pInitMBRecord,MBRecord,sizeof(CallRecord) * MAX_PHONE_RECORD);
		for (i = 0;i < MAX_PHONE_RECORD;i++)
		{
			if (strlen(MBRecord[i].PhoneNo) == 0)
				break;
		}
		return i;
	}
}

#if 0
static  int     InitGetMBRecord(CallRecord * pInitMBRecord,MBRECORDTYPE CallRecType)
{
    HANDLE  hf;
    char    FileName[FILENAMEMAXLEN] = "";
    int     i;
    DWORD   dfsize = 0;

    if (!GetFileName(FileName,CallRecType))
        return -1;

    memset(MBRecord,0x00,sizeof(CallRecord) * MAX_PHONE_RECORD);
    if(INVALID_HANDLE_VALUE == (hf = CreateFile(FileName, ACCESS_READ, 0)))
    {
        CloseFile(hf);
        if (INVALID_HANDLE_VALUE == 
            (hf = CreateFile(FileName,ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE,0)))
        {
            goto Error;
        }
        if (-1 == SetFilePointer(hf, 0L, SEEK_SET))
            goto Error;

        if (-1 == WriteFile(hf,MBRecord,sizeof(CallRecord) * MAX_PHONE_RECORD))
            goto Error;
        
        CloseFile(hf);
        memcpy(pInitMBRecord,MBRecord,sizeof(CallRecord) * MAX_PHONE_RECORD);
        return 0;
    }
    dfsize = GetFileSize(hf);
    if (sizeof(CallRecord) * MAX_PHONE_RECORD != dfsize)
        goto Error;

    if (-1 == ReadFile(hf,MBRecord,sizeof(CallRecord) * MAX_PHONE_RECORD))
        goto Error;

    CloseFile(hf);

    memcpy(pInitMBRecord,MBRecord,sizeof(CallRecord) * MAX_PHONE_RECORD);

    for (i = 0;i < MAX_PHONE_RECORD;i++)
    {
        if (strlen(MBRecord[i].PhoneNo) == 0)
            break;
    }
    return i;
Error:
    CloseFile(hf);
    DeleteFile(FileName);
    return -1;
}
#endif

BOOL    SaveMBRecordByPhoneNum(MBRECORDTYPE CallRecType,const char * pPhoneNum,SYSTEMTIME * pCurTime)
{
    CallRecord  callrecord;
    int         iSour;
    SYSTEMTIME  sCurTime;

    if (pPhoneNum == NULL)
        return FALSE;

    iSour = strlen(pPhoneNum);
    if (0 == iSour)
        return FALSE;

    memset(&callrecord,0x00,sizeof(CallRecord));

    GetLocalTime(&sCurTime);
    memcpy(&callrecord.Time,&sCurTime,sizeof(SYSTEMTIME));
    
    if (pCurTime != NULL)
        memcpy(pCurTime,&sCurTime,sizeof(SYSTEMTIME));

    strncpy(callrecord.PhoneNo,pPhoneNum,sizeof(callrecord.PhoneNo) > iSour ? iSour + 1:sizeof(callrecord.PhoneNo) - 1);

    return (SaveMBRecord(&callrecord,CallRecType));
}

#define BASE_YEAR		1980

#define YEAR_MASK		0xFE000000		
#define MONTH_MASK		0x01E00000		
#define DAY_MASK		0x001F0000		
#define HOUR_MASK		0x0000F800		
#define MINUTE_MASK		0x000007E0		
#define SECOND_MASK		0x0000001F		


#define YEAR_OFFSET		25
#define MONTH_OFFSET	21
#define DAY_OFFSET		16
#define HOUR_OFFSET		11
#define MINUTE_OFFSET	5
#define SECOND_OFFSET	0

void SYSTIMEtoTIME(SYSTEMTIME* pSysTime, unsigned long * t)
{
	*t =  ((pSysTime->wYear-BASE_YEAR) << YEAR_OFFSET) & YEAR_MASK;
	*t |= (pSysTime->wMonth << MONTH_OFFSET) & MONTH_MASK;
	*t |= (pSysTime->wDay << DAY_OFFSET) & DAY_MASK;
	*t |= (pSysTime->wHour << HOUR_OFFSET) & HOUR_MASK;
	*t |= (pSysTime->wMinute << MINUTE_OFFSET) & MINUTE_MASK;
	*t |= (pSysTime->wSecond >> 1) & SECOND_MASK;
}

BOOL SaveMBRecordLog(MBRECORDTYPE CallRecType,const char * pPhoneNum,
                     SYSTEMTIME * pInOutTime,SYSTEMTIME * pUsedTime)
{
	LOGRECORD * plogrec;
	void      * ptmprec;
	unsigned long begintm, durationtm;
	int len, apnlen;
	apnlen = strlen(pPhoneNum);
//	len = sizeof(unsigned short)+ sizeof(RCDTYPE) +sizeof(DRTTYPE) +sizeof(unsigned long) +sizeof(VOICETYPE)+strlen(pPhoneNum) ;
	len = sizeof(LOGRECORD) + apnlen;
	ptmprec = malloc(len+1);
	memset(ptmprec, 0, len +1);
	plogrec = (LOGRECORD *)ptmprec;
	switch(CallRecType) 
	{
	case UNANSWER_REC:
		plogrec->direction = DRT_UNRECIEVE;
		
		break;
	case ANSWERED_REC:
		plogrec->direction = DRT_RECIEVE;
		break;
	case DIALED_REC:
		plogrec->direction = DRT_SEND;
		break;
	default:
		free(plogrec);
		return FALSE;
	}
	
	LOG_STtoFT(pInOutTime, &begintm);
	if (pUsedTime == NULL) 
		plogrec->u.voice.duration =0;
	else{
		LOG_STtoFT(pUsedTime, &durationtm);
		plogrec->u.voice.duration =durationtm;
	}
	plogrec->begintm = begintm;
	plogrec->type = TYPE_VOICE;
	plogrec->index = 0;
    strncpy(plogrec->APN, pPhoneNum, apnlen);
	plogrec->APN[apnlen] = 0;
	if (-1 == Log_Write( plogrec )) 
	{
		free(ptmprec);
		return FALSE;
	}
	else 
//		free(plogrec);
		free(ptmprec);
	return TRUE;
}

BOOL    SaveMBRecordUsedTime(MBRECORDTYPE CallRecType,const char * pPhoneNum,
                             const SYSTEMTIME * pInOutTime,const SYSTEMTIME * pUsedTime)
{
    FILE * file;
    char         FileName[FILENAMEMAXLEN] = "";
    int          i;
    CallRecord * pCurRecord;

    if ((pPhoneNum == NULL) || (pInOutTime == NULL) || (pUsedTime == NULL))
        return FALSE;

    if (!GetFileName(FileName,CallRecType))
        return FALSE;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == 0)
        return FALSE;
	file = fopen(FileName, "r+");
	if (file == NULL)
		file = fopen(FileName, "w+");
	if (file == NULL)
		return FALSE;
	else{
		memset(MBRecord,0x00      ,sizeof(CallRecord) * MAX_PHONE_RECORD);
		memcpy(MBRecord,pCurRecord,sizeof(CallRecord) * MAX_PHONE_RECORD);
		if (fseek(file, 0, SEEK_SET) != 0)
		{
			fclose(file);
			return FALSE;
		}
		for (i = 0;i < MAX_PHONE_RECORD;i++)
		{
/*
			if ((strcmp(pPhoneNum,MBRecord[i].PhoneNo) == 0) &&
				(pInOutTime->wYear == MBRecord[i].Time.wYear) &&
				(pInOutTime->wMonth == MBRecord[i].Time.wMonth) &&
				(pInOutTime->wDay == MBRecord[i].Time.wDay) &&
				(pInOutTime->wHour == MBRecord[i].Time.wHour) &&
				(pInOutTime->wMinute == MBRecord[i].Time.wMinute) &&
				(pInOutTime->wSecond == MBRecord[i].Time.wSecond))*/
        if ((pInOutTime->wYear == MBRecord[i].Time.wYear) &&
            (pInOutTime->wMonth == MBRecord[i].Time.wMonth) &&
            (pInOutTime->wDay == MBRecord[i].Time.wDay) &&
            (pInOutTime->wHour == MBRecord[i].Time.wHour) &&
            (pInOutTime->wMinute == MBRecord[i].Time.wMinute) &&
            (pInOutTime->wSecond == MBRecord[i].Time.wSecond))
			{
				memcpy(&MBRecord[i].TimeUsed,pUsedTime,sizeof(SYSTEMTIME));
				if (-1 == fseek(file, sizeof(CallRecord) * i, SEEK_CUR))
				{
					fclose(file);
					return FALSE;
				}
				fwrite(&MBRecord[i], sizeof(CallRecord), 1, file);
				fclose(file);
				memcpy(&pCurRecord[i],&MBRecord[i],sizeof(CallRecord));
				return TRUE;
			}
		}
		fclose(file);
		return FALSE;
	}
}
#if 0
BOOL    SaveMBRecordUsedTime(MBRECORDTYPE CallRecType,const char * pPhoneNum,
                             const SYSTEMTIME * pInOutTime,const SYSTEMTIME * pUsedTime)
{
    HANDLE       hf;
    char         FileName[FILENAMEMAXLEN] = "";
    int          i;
    CallRecord * pCurRecord;

    if ((pPhoneNum == NULL) || (pInOutTime == NULL) || (pUsedTime == NULL))
        return FALSE;

    if (!GetFileName(FileName,CallRecType))
        return FALSE;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == 0)
        return FALSE;

    if(INVALID_HANDLE_VALUE == (hf = CreateFile(FileName, ACCESS_WRITE|ACCESS_READ, 0)))
    {
        if (INVALID_HANDLE_VALUE == 
                    (hf = CreateFile(FileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
         {
            CloseFile(hf);
            return FALSE;
         }
    }
    memset(MBRecord,0x00      ,sizeof(CallRecord) * MAX_PHONE_RECORD);
    memcpy(MBRecord,pCurRecord,sizeof(CallRecord) * MAX_PHONE_RECORD);
    

    if (-1 == SetFilePointer(hf, 0, SEEK_SET))
        goto Error;

    for (i = 0;i < MAX_PHONE_RECORD;i++)
    {
/*
        if ((strcmp(pPhoneNum,MBRecord[i].PhoneNo) == 0) &&
            (pInOutTime->wYear == MBRecord[i].Time.wYear) &&
            (pInOutTime->wMonth == MBRecord[i].Time.wMonth) &&
            (pInOutTime->wDay == MBRecord[i].Time.wDay) &&
            (pInOutTime->wHour == MBRecord[i].Time.wHour) &&
            (pInOutTime->wMinute == MBRecord[i].Time.wMinute) &&
            (pInOutTime->wSecond == MBRecord[i].Time.wSecond))*/
        if ((pInOutTime->wYear == MBRecord[i].Time.wYear) &&
            (pInOutTime->wMonth == MBRecord[i].Time.wMonth) &&
            (pInOutTime->wDay == MBRecord[i].Time.wDay) &&
            (pInOutTime->wHour == MBRecord[i].Time.wHour) &&
            (pInOutTime->wMinute == MBRecord[i].Time.wMinute) &&
            (pInOutTime->wSecond == MBRecord[i].Time.wSecond))
        {
            memcpy(&MBRecord[i].TimeUsed,pUsedTime,sizeof(SYSTEMTIME));
            if (-1 == SetFilePointer(hf, sizeof(CallRecord) * i, SEEK_CUR))
                goto Error;

            if (-1 == WriteFile(hf,&MBRecord[i],sizeof(CallRecord)))
            {
                goto Error;
            }
            else
            {
                CloseFile(hf);
                memcpy(&pCurRecord[i],&MBRecord[i],sizeof(CallRecord));
                return TRUE;
            }
        }
    }
    CloseFile(hf);
    return FALSE;
Error:
    CloseFile(hf);
    return FALSE;
}
#endif

static  BOOL    SaveMBRecord(const CallRecord * pCallRecNode, MBRECORDTYPE CallRecType)
{
	FILE * file;
    char         FileName[FILENAMEMAXLEN] = "";
    CallRecord * pCurRecord;

    if (pCallRecNode == NULL)
        return FALSE;

    if (!GetFileName(FileName,CallRecType))
        return FALSE;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == 0)
        return FALSE;
	file = fopen(FileName, "w+");
	if (file == NULL) 
		return FALSE;
	else{
		memset(MBRecord,0x00      ,sizeof(CallRecord) * MAX_PHONE_RECORD);
#if defined(MBRECORD_SAVEDATA_UNREPEAT)
		{
			int i,j;
			for (i = 0,j = 0;i < MAX_PHONE_RECORD;i++)
			{
				if (strcmp(pCurRecord[i].PhoneNo,pCallRecNode->PhoneNo) == 0)
					continue;
				else
				{
					if (strlen(pCurRecord[i].PhoneNo) != 0)
						memcpy(&MBRecord[j++],&pCurRecord[i],sizeof(CallRecord));
				}
			}
		}
#else
		memcpy(MBRecord,pCurRecord,sizeof(CallRecord) * MAX_PHONE_RECORD);	
#endif

		fwrite(pCallRecNode, sizeof(CallRecord), 1, file);
		fwrite(MBRecord, sizeof(CallRecord), MAX_PHONE_RECORD-1, file);
		fclose(file);
		memcpy(pCurRecord,pCallRecNode,sizeof(CallRecord));
		//pCurRecord++;
		memcpy(pCurRecord + 1,MBRecord,(MAX_PHONE_RECORD - 1) * sizeof(CallRecord));
		return TRUE;
	}
}
#if 0
static  BOOL    SaveMBRecord(const CallRecord * pCallRecNode, MBRECORDTYPE CallRecType)
{
    HANDLE       hf;
    char         FileName[FILENAMEMAXLEN] = "";
    CallRecord * pCurRecord;

    if (pCallRecNode == NULL)
        return FALSE;

    if (!GetFileName(FileName,CallRecType))
        return FALSE;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == 0)
        return FALSE;

    if(INVALID_HANDLE_VALUE == (hf = CreateFile(FileName, ACCESS_WRITE|ACCESS_READ, 0)))
    {
        if (INVALID_HANDLE_VALUE == 
                    (hf = CreateFile(FileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
         {
            CloseFile(hf);
            return FALSE;
         }
    }
    memset(MBRecord,0x00      ,sizeof(CallRecord) * MAX_PHONE_RECORD);
    memcpy(MBRecord,pCurRecord,sizeof(CallRecord) * MAX_PHONE_RECORD);
    
    if (-1 == SetFilePointer(hf, 0, SEEK_SET))
        goto Error;

    if (-1 == WriteFile(hf,(PVOID)pCallRecNode, sizeof(CallRecord)))
        goto Error;

    if (-1 == WriteFile(hf,MBRecord,(MAX_PHONE_RECORD - 1) * sizeof(CallRecord)))
        goto Error;

    memcpy(pCurRecord,pCallRecNode,sizeof(CallRecord));
    pCurRecord++;
    memcpy(pCurRecord,MBRecord,(MAX_PHONE_RECORD - 1) * sizeof(CallRecord));

    CloseFile(hf);
    return TRUE;
Error:
    CloseFile(hf);
    return FALSE;
}
#endif

BOOL    DeleteMBRecord(MBRECORDTYPE CallRecType,int index)
{
    FILE * file;
    char         FileName[FILENAMEMAXLEN] = "";
    int          i;
    CallRecord * pCurRecord;

    if (!GetFileName(FileName,CallRecType))
        return FALSE;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == 0)
        return FALSE;
	file = fopen(FileName, "w+");
	if (file == NULL) 
		return FALSE;
	else{
		memset(MBRecord,0x00      ,sizeof(CallRecord) * MAX_PHONE_RECORD);
		memcpy(MBRecord,pCurRecord,sizeof(CallRecord) * MAX_PHONE_RECORD);
		
		if (-1 == fseek(file, 0, SEEK_SET))
		{
			fclose(file);
			return FALSE;
		}

		if (-1 == index)
		{
			memset(MBRecord  ,0x00,sizeof(CallRecord) * MAX_PHONE_RECORD);
			memset(pCurRecord,0x00,sizeof(CallRecord) * MAX_PHONE_RECORD);
            fwrite(MBRecord, sizeof(CallRecord), MAX_PHONE_RECORD, file);
		}
		else
		{
			for (i = 0;i < MAX_PHONE_RECORD;i++)
			{
				if (i == index)
				{
					memset(&MBRecord[i],0x00,sizeof(CallRecord));
					continue;
				}
				fwrite(&MBRecord[i], sizeof(CallRecord), 1, file);				
				memcpy(pCurRecord,&MBRecord[i],sizeof(CallRecord));
				pCurRecord++;
			}
			fwrite(&MBRecord[index], sizeof(CallRecord), 1, file);			
			memcpy(pCurRecord,&MBRecord[index],sizeof(CallRecord));
		}
		fclose(file);
		return TRUE;
	}
}

extern IsSelected(int i);

BOOL    DeleteSelectedMBRecord(MBRECORDTYPE CallRecType,int counter)
{
    FILE * file;
    char         FileName[FILENAMEMAXLEN] = "";
    int          i,j;
    CallRecord * pCurRecord;

    if (!GetFileName(FileName,CallRecType))
        return FALSE;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == 0)
        return FALSE;
	file = fopen(FileName, "w+");
	if (file == NULL) 
		return FALSE;
	else{
		memset(MBRecord,0x00      ,sizeof(CallRecord) * MAX_PHONE_RECORD);
		//memcpy(MBRecord,pCurRecord,sizeof(CallRecord) * MAX_PHONE_RECORD);
		
		if (-1 == fseek(file, 0, SEEK_SET))
		{
			fclose(file);
			return FALSE;
		}		
//		for (i = 0;i < MAX_PHONE_RECORD;i++)
//		{
//			for (j = 0; j <counter; j++)
//			{
//				if (IsSelected(j)&& j == i)
//				{
//					memset(&MBRecord[i],0x00,sizeof(CallRecord));
//					break;
//				}
//			}
//			if ( j == i)
//				continue;
//			fwrite(&MBRecord[i], sizeof(CallRecord), 1, file);				
//			memcpy(pCurRecord,&MBRecord[i],sizeof(CallRecord));
//			pCurRecord++;
//		}
        for (i = 0,j = 0;i < MAX_PHONE_RECORD;i++)
		{
            if (!IsSelected(i))
            {
                memcpy(&MBRecord[j++],pCurRecord + i,sizeof(CallRecord));
            }
		}
        memcpy(pCurRecord,MBRecord,sizeof(CallRecord) * MAX_PHONE_RECORD);
        fwrite(pCurRecord,sizeof(CallRecord),MAX_PHONE_RECORD,file);
//		for (j = 0; j <counter; j++)
//		{
//			if (IsSelected(j))
//			{
//                CallRecord cTemp;
//                memset(&cTemp,0x00,sizeof(CallRecord));
//				//fwrite(&MBRecord[i], sizeof(CallRecord), 1, file);			
//                fwrite(&cTemp, sizeof(CallRecord), 1, file);			
//				memcpy(pCurRecord,&MBRecord[i],sizeof(CallRecord));
//			}
//		}
		fclose(file);
		return TRUE;
	}
}

#if 0
BOOL    DeleteMBRecord(MBRECORDTYPE CallRecType,int index)
{
    HANDLE       hf;
    char         FileName[FILENAMEMAXLEN] = "";
    int          i;
    CallRecord * pCurRecord;

    if (!GetFileName(FileName,CallRecType))
        return FALSE;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == 0)
        return FALSE;

    if(INVALID_HANDLE_VALUE == (hf = CreateFile(FileName, ACCESS_WRITE|ACCESS_READ, 0)))
    {
        if (INVALID_HANDLE_VALUE == 
                    (hf = CreateFile(FileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
         {
            CloseFile(hf);
            return FALSE;
         }
    }
    memset(MBRecord,0x00      ,sizeof(CallRecord) * MAX_PHONE_RECORD);
    memcpy(MBRecord,pCurRecord,sizeof(CallRecord) * MAX_PHONE_RECORD);
    

    if (-1 == SetFilePointer(hf, 0, SEEK_SET))
        goto Error;

    if (-1 == index)
    {
        memset(MBRecord  ,0x00,sizeof(CallRecord) * MAX_PHONE_RECORD);
        memset(pCurRecord,0x00,sizeof(CallRecord) * MAX_PHONE_RECORD);

        if (-1 == WriteFile(hf,MBRecord,sizeof(CallRecord) * MAX_PHONE_RECORD))
            goto Error;
    }
    else
    {
        for (i = 0;i < MAX_PHONE_RECORD;i++)
        {
            if (i == index)
            {
                memset(&MBRecord[i],0x00,sizeof(CallRecord));
                continue;
            }
            if (-1 == WriteFile(hf,&MBRecord[i],sizeof(CallRecord)))
                goto Error;

            memcpy(pCurRecord,&MBRecord[i],sizeof(CallRecord));
            pCurRecord++;
        }
        if (-1 == WriteFile(hf,&MBRecord[index],sizeof(CallRecord)))
            goto Error;

        memcpy(pCurRecord,&MBRecord[index],sizeof(CallRecord));
    }
    CloseFile(hf);
    return TRUE;
Error:
    CloseFile(hf);
    return FALSE;
}
#endif

int     GetMBRecord(CallRecord * pCallRecNode,size_t count,MBRECORDTYPE CallRecType,int iDataIndex)
{
    CallRecord * pCurRecord;
    int i;

    if (NULL == pCallRecNode)
        return -1;

    
    if ((count != sizeof(CallRecord)) && (count != (sizeof(CallRecord) * MAX_PHONE_RECORD)))
        return -1;

    if ((iDataIndex > MAX_PHONE_RECORD - 1) || (iDataIndex < -1))
        return -1;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == 0)
        return -1;

    if (-1 == iDataIndex)
        memcpy(pCallRecNode,pCurRecord,count);
    else
        memcpy(pCallRecNode,&pCurRecord[iDataIndex],count);

    for (i = 0;i < MAX_PHONE_RECORD;i++)
    {
        if (strlen(pCurRecord[i].PhoneNo) == 0)
            break;
    }
    return i;
}

int     GetRecorderNumber(MBRECORDTYPE CallRecType)
{
    int     i,iRNum = 0;
    char    FileName[FILENAMEMAXLEN] = "";

    if (!GetFileName(FileName,CallRecType))
        return FALSE;

    memset(MBRecord,0x00,sizeof(CallRecord) * MAX_PHONE_RECORD);
    GetMBRecord(MBRecord,sizeof(CallRecord) * MAX_PHONE_RECORD,CallRecType,-1);
    for (i = 0;i < MAX_PHONE_RECORD;i++)
    {
        if (strlen(MBRecord[i].PhoneNo) != 0)
            iRNum++;
        else
            break;
    }
    return (iRNum);
}

BOOL    ReadRecordFromData(MBRECORDTYPE CallRecType)
{
    return (GetMBRecord(MBRecord,sizeof(CallRecord) * MAX_PHONE_RECORD,CallRecType,-1) != -1);
}

BOOL    GetRecordFromData(CallRecord * pCallRecNode,int iDataIndex)
{
    if ((iDataIndex > MAX_PHONE_RECORD - 1) || (iDataIndex < 0))
        return FALSE;
    memcpy(pCallRecNode,&MBRecord[iDataIndex],sizeof(CallRecord));
    return TRUE;
}


BOOL    SaveRecord_Cost(const MBRecordCostTime * pCost)
{
	FILE * file;
    if (NULL == pCost)
        return FALSE;
	file = fopen(pRECCOST_LOG, "w+");
    if (file == NULL) 
		return FALSE;
	else{
		fseek(file, 0 , SEEK_SET);
		fwrite(pCost, sizeof(MBRecordCostTime), 1, file);
		fclose(file);
		memcpy(&sRecordCostTime,pCost,sizeof(MBRecordCostTime));
		UpdateCallCounters();
		return TRUE;
	}
}
#if 0
BOOL    SaveRecord_Cost(const MBRecordCostTime * pCost)
{
    HANDLE hFile;

    if (NULL == pCost)
        return FALSE;

    hFile = CreateFile(pRECCOST_LOG, ACCESS_WRITE, 0);
    
    if (hFile == INVALID_HANDLE_VALUE)
    {
        hFile = CreateFile(pRECCOST_LOG, CREATE_NEW|ACCESS_WRITE, 0);
        if (hFile == INVALID_HANDLE_VALUE)
            return FALSE;
    }
    
    if (-1 == WriteFile(hFile, ( PVOID)pCost, sizeof(MBRecordCostTime)))
    {
        CloseFile(hFile);
        return FALSE;
    }
    CloseFile(hFile);

    memcpy(&sRecordCostTime,pCost,sizeof(MBRecordCostTime));
    return TRUE;
}
#endif

BOOL    ReadRecord_Cost(MBRecordCostTime * pCost)
{
	FILE * file;
    DWORD   dfsize = 0;
    static BOOL bInit = FALSE;

    if (NULL == pCost)
        return FALSE;

    if (bInit)
    {
        memcpy(pCost,&sRecordCostTime,sizeof(MBRecordCostTime));
        return TRUE;
    }
	file = fopen(pRECCOST_LOG, "r");
	if (file == NULL) {
		file = fopen(pRECCOST_LOG, "w+");
		if (file == NULL)
			return FALSE;
		else{
			memset(pCost, 0, sizeof(MBRecordCostTime));
			fwrite(pCost, sizeof(MBRecordCostTime), 1, file);
			fclose(file);
			memcpy(&sRecordCostTime,pCost,sizeof(MBRecordCostTime));
			return TRUE;
		}
	}
	else{
		dfsize = fread(pCost, sizeof(MBRecordCostTime), 1, file);
		fclose(file);
		memcpy(&sRecordCostTime,pCost,sizeof(MBRecordCostTime));
		bInit = TRUE;
		return TRUE;
	}

}
#if 0
BOOL    ReadRecord_Cost(MBRecordCostTime * pCost)
{
    HANDLE  hFile;
    DWORD   dfsize = 0;
    static BOOL bInit = FALSE;

    if (NULL == pCost)
        return FALSE;

    if (bInit)
    {
        memcpy(pCost,&sRecordCostTime,sizeof(MBRecordCostTime));
        return TRUE;
    }
    hFile = CreateFile(pRECCOST_LOG, ACCESS_READ, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        hFile = CreateFile(pRECCOST_LOG, CREATE_NEW|ACCESS_WRITE, 0);
        if (hFile == INVALID_HANDLE_VALUE)
            return FALSE;

        memset(pCost, 0, sizeof(MBRecordCostTime));
        
        if (-1 == WriteFile(hFile, pCost, sizeof(MBRecordCostTime)))
            goto Error;

        CloseFile(hFile);

        memcpy(&sRecordCostTime,pCost,sizeof(MBRecordCostTime));

        return TRUE;
    }
    dfsize = GetFileSize(hFile);
    if (sizeof(MBRecordCostTime) != dfsize)
        goto Error;

    if (-1 == ReadFile(hFile, pCost, sizeof(MBRecordCostTime)))
        goto Error;

    CloseFile(hFile);

    memcpy(&sRecordCostTime,pCost,sizeof(MBRecordCostTime));
    bInit = TRUE;

    return TRUE;

Error:
    CloseFile(hFile);
    DeleteFile(pRECCOST_LOG);
    return FALSE;
}
#endif

/*
BOOL    APP_ViewUnanswered(void)
{
    return (CallMBLogListExtWindow(NULL,UNANSWER_REC,MBL_WS_DEFAULT, TRUE));
}
BOOL    APP_ViewDialed(void)
{
    return (CallMBLogListExtWindow(NULL,DIALED_REC,MBL_WS_DEFAULT, TRUE));
}*/

int     APP_GetFrqntNo(char* pFrqntNo)
{
    int  i,iSour;
    char * pNo = NULL;

    memset(MBRecord,0x00,sizeof(CallRecord) * MAX_PHONE_RECORD);
    GetMBRecord(MBRecord,sizeof(CallRecord) * MAX_PHONE_RECORD,DIALED_REC,-1);

    pNo = pFrqntNo;
    for (i = 0;i < MAX_PHONE_RECORD;i++)
    {
        iSour = strlen(MBRecord[i].PhoneNo);
        if (0 == iSour)
            break;
        strncat(pNo,MBRecord[i].PhoneNo,AB_TEL_LEN > iSour ? iSour + 1 : AB_TEL_LEN - 1);
        pNo += AB_TEL_LEN;
    }
    return (i);
}

BOOL FindNumberInUnAnswerList(const char * number)
{
    char    FileName[FILENAMEMAXLEN] = "";	
	int i;
    if (!GetFileName(FileName, UNANSWER_REC))
        return FALSE;	
    memset(MBRecord,0x00,sizeof(CallRecord) * MAX_PHONE_RECORD);
    GetMBRecord(MBRecord,sizeof(CallRecord) * MAX_PHONE_RECORD,UNANSWER_REC,-1);
    for (i = 0;i < MAX_PHONE_RECORD;i++)
    {
        if (strcmp(MBRecord[i].PhoneNo, number) == 0)
            return TRUE;
        else
		    continue;
    }
	return FALSE;
}
