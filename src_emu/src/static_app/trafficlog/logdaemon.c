/**************************************************************************\
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Model   : traffic log
*
* Purpose : Log OP interface
*  
* Author  : 
*
\**************************************************************************/
#include "window.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "log.h"
#include "logdb.h"

//#define WM_ADDRECORD  WM_USER+100
//#define WM_LISTRECORD WM_USER+101
//#define WM_GETRECORD  WM_USER+102
//#define WM_DELETEALL  WM_USER+103
//#define WM_EXPIRED    WM_USER+104

HWND ghDaemon;
int  gInitDone;
int gCounter;
int gflag;
READWNDINFO greadwndinfo;
const char* DAEMONWNDCLASS = "LOGDAEMONWNDCLASS";

static LRESULT CALLBACK LogDaemonWndProc (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
static BOOL SeekSMSAndUpdate(LOG_DB *plogdb, unsigned long smsID, SMSSTATUS newstatus);
//static void savetohost();
extern void * CallSetting_Open(void);
extern int CallSetting_Close(void * callsetting);
extern unsigned long GetSaveTime();
extern BOOL SaveFile(char* buf, int buflen, char* szFileName);
extern int DaynCmp(SYSTEMTIME systime1, SYSTEMTIME systime2, unsigned int numday);
extern	void	RealTimeUpdateCommunication();
/*********************************************************************\
* Function: Log_Init	   
* Purpose : Init and judge whether the log is expired
* Params  :      	   
* Return  : -1 Failure 1 Success
* Remarks	   
\**********************************************************************/
int Log_Init(void)
{
	LOG_DB * plogdb; 
	if ( gInitDone )
        return gInitDone = 1;
	plogdb = LOG_DB_Open();
//	GetDBFlag(plogdb);
	if(FALSE == UpdateLogDB(plogdb))
	{
		gCounter = -1;
		LOG_DB_Close(plogdb);
		return gInitDone = -1;
	}
	else
	{
		gCounter = GetCurMaxID(plogdb)+1;
		LOG_DB_Close(plogdb);
	}
	memset(&greadwndinfo, 0, sizeof(READWNDINFO));
//	savetohost();
	return gInitDone = 1;
}

/*********************************************************************\
* Function: Log_RegisterReadWnd	   
* Purpose : Register notified window and message
* Params  :      	   
* Return  : 
* Remarks	   
\**********************************************************************/
void Log_RegisterReadWnd(HWND hWnd, UINT wMsg)
{
	greadwndinfo.hReadWnd = hWnd;
	greadwndinfo.wRCMsg = wMsg;
}

/*********************************************************************\
* Function: Log_UnRegisterReadWnd	   
* Purpose : UnRegister
* Params  :      	   
* Return  : 
* Remarks	   
\**********************************************************************/
void Log_UnRegisterReadWnd()
{
	memset(&greadwndinfo, 0, sizeof(READWNDINFO));
}
/*********************************************************************\
* Function: Log_Empty	   
* Purpose : delete log file and recreate
* Params  :      	   
* Return  : 
* Remarks	   
\**********************************************************************/
int Log_Empty (void)
{
	LOG_DB * plogdb; 
	plogdb = LOG_DB_Open();
	if (LOG_DB_Clear(plogdb) == -1)
	{
		LOG_DB_Close(plogdb);
		return -1;
	}
	else
	{
		LOG_DB_Close(plogdb);
		return 0;
	}
}
/*********************************************************************\
* Function: Log_Read	   
* Purpose : read record
* Params  : buf: read record buf
*           idx: record index
*           buflen: buf length     	   
* Return  : if buf==NULL  success : record len 
                          failure : -1
            if buf!=NULL  success : LOG_DB_SUCCESS
			              failure : LOG_DB_FAILURE
* Remarks	   
\**********************************************************************/
int Log_Read (void* buf, unsigned short idx, unsigned short buflen)
{
	LOG_DB * plogdb; 
	LOGRECORD *plogrec;
	unsigned short recindex;
	unsigned short reclen;
	plogrec = (LOGRECORD *)buf;
	recindex = idx;
	reclen = buflen;
	if (plogrec == NULL)
	{
		plogdb = LOG_DB_Open();
		reclen = LOG_DB_Get(plogdb, recindex, plogrec, 0);
		LOG_DB_Close(plogdb);
		return reclen;
	}
	else
	{
		plogdb = LOG_DB_Open();
		if (LOG_DB_Get(plogdb, recindex, plogrec, reclen) == LOG_DB_SUCCESS)
		{
			LOG_DB_Close(plogdb);
			return LOG_DB_SUCCESS;
		}
		else
		{
			LOG_DB_Close(plogdb);
			return LOG_DB_FAILURE;
		}
	}			
}
/*********************************************************************\
* Function: Log_Write	   
* Purpose : write record
* Params  : plogrec record    	   
* Return  : success : 0 
            failure : -1
* Remarks	   
\**********************************************************************/
int Log_Write (LOGRECORD* plogrec)
{
	LOG_DB * plogdb; 
//	unsigned long savetime, rectime;
	
	if(NULL == plogrec)
		return -1;
	
	plogdb = LOG_DB_Open();
	if(NULL == plogdb)
		return -1;

	// check and update the records
	UpdateLogDB(plogdb);

	gCounter = GetDBCounter(plogdb);
	if (gCounter < 0)
	{
		LOG_DB_Close(plogdb);
		return -1;
	}
//	if (gCounter == 0)
//	{
//		rectime = plogrec->begintm;
//	//	savetime = GetSaveTime();
//		if (WriteLogTime(plogdb, rectime, savetime) == -1)
//		{
//			LOG_DB_Close(plogdb);
//			return -1;
//		}
//	}
/*
	if (gCounter >0)
	{
		if (GetLogTime(plogdb) == -1)
		{
			LOG_DB_Close(plogdb);
			return -1;
		}
		rectime = plogdb->fstrcdtime;
		savetime = plogdb->savetime;
        curtime = plogrec->begintm;
		if (dayncmp(curtime, rectime, savetime) == 1)
		{
			LOG_DB_Clear(plogdb);
			gCounter = GetDBCounter(plogdb);
			if (WriteLogTime(plogdb, rectime, savetime) == -1)
			{
				LOG_DB_Close(plogdb);
				return -1;
			}           
		}
	}
*/
	if(plogdb->savetime <= 0)
	{
		LOG_DB_Close(plogdb);
		return 0;
	}
	
	if (LOG_DB_Add(plogdb, plogrec) == -1)
	{
		LOG_DB_Close(plogdb);
		return -1;
	}
	
	LOG_DB_Close(plogdb);
	gCounter ++;
	SendMessage(greadwndinfo.hReadWnd, greadwndinfo.wRCMsg, (WPARAM)gCounter-1, (LPARAM)plogrec);
//	savetohost();
	RealTimeUpdateCommunication();
    return 0;
}
/*********************************************************************\
* Function: Log_Expired	   
* Purpose : write record
* Params  : plogrec record    	   
* Return  : if return == -1 expired
            else return == record counter in the log
* Remarks	   
\**********************************************************************/
int Log_Expired(void)
{
	LOG_DB * plogdb = LOG_DB_Open();
	if(NULL == plogdb)
		return -1;

	gCounter = GetDBCounter(plogdb);
	LOG_DB_Close(plogdb);

	return gCounter;
}

unsigned long GetSaveTime()
{
	return 1;
}

#define BASE_YEAR          1980
#define YEAR_MASK          0xFE000000        /*    (1111 1110 0000 0000 0000 0000 00000000b)*/
#define MONTH_MASK         0x01E00000        /*    (0000 0001 1110 0000 0000 0000 00000000b)*/
#define DAY_MASK           0x001F0000        /*    (0000 0000 0001 1111 0000 0000 00000000b)*/
#define HOUR_MASK          0x0000F800        /*    (0000 0000 0000 0000 1111 1000 00000000b)*/
#define MINUTE_MASK        0x000007E0        /*    (0000 0000 0000 0000 0000 0111 11100000b)*/
#define SECOND_MASK        0x0000001F        /*    (0000 0000 0000 0000 0000 0000 00011111b)*/
#define YEAR_OFFSET       25
#define MONTH_OFFSET      21
#define DAY_OFFSET        16
#define HOUR_OFFSET       11
#define MINUTE_OFFSET     5
#define SECOND_OFFSET     0
/*********************************************************************\
* Function: LOG_FTtoST	   
* Purpose : time type trasformation: unsigned long -> SYSTEMTIME
* Params  :     	   
* Return  : 
* Remarks	   
\**********************************************************************/
void LOG_FTtoST(unsigned long * pTime, SYSTEMTIME* pSysTime)
{	
	pSysTime->wYear             = (WORD)((( (*pTime) & YEAR_MASK ) >> YEAR_OFFSET ) + BASE_YEAR);
	pSysTime->wMonth    = (WORD)(( (*pTime) & MONTH_MASK ) >> MONTH_OFFSET );
	pSysTime->wDay              = (WORD)(( (*pTime) & DAY_MASK ) >> DAY_OFFSET );
	pSysTime->wHour             = (WORD)(( (*pTime) & HOUR_MASK ) >> HOUR_OFFSET );
	pSysTime->wMinute   = (WORD)(( (*pTime) & MINUTE_MASK ) >> MINUTE_OFFSET );
	pSysTime->wSecond  = (WORD)(( (*pTime) & SECOND_MASK ) << 1);
	pSysTime->wDayOfWeek= 0;
	pSysTime->wMilliseconds = 0;
}
/*********************************************************************\
* Function: LOG_FTtoST	   
* Purpose : time type trasformation:SYSTEMTIME->unsigned long 
* Params  :     	   
* Return  : 
* Remarks	   
\**********************************************************************/
void LOG_STtoFT(SYSTEMTIME* pSysTime, unsigned long* pTime)
{
	*pTime =  ((pSysTime->wYear-BASE_YEAR) << YEAR_OFFSET) & YEAR_MASK;
	*pTime |= (pSysTime->wMonth << MONTH_OFFSET) & MONTH_MASK;	
	*pTime |= (pSysTime->wDay << DAY_OFFSET) & DAY_MASK;	
	*pTime |= (pSysTime->wHour << HOUR_OFFSET) & HOUR_MASK;	
	*pTime |= (pSysTime->wMinute << MINUTE_OFFSET) & MINUTE_MASK;	
	*pTime |= (pSysTime->wSecond >> 1) & SECOND_MASK;			
}
/*********************************************************************\
* Function: dayncmp	   
* Purpose : time compare
* Params  : curtime: current time
*           fsttime: the first record time in the log
*           saveday: save days    	   
* Return  : -1: no record so fsttime == 0
*            0: unexpired
*            1: expired
* Remarks	   
\**********************************************************************/
int dayncmp(unsigned long curtime, unsigned long fsttime, unsigned long saveday)
{
	SYSTEMTIME cursystime, fstsystime;
	if (fsttime == 0) 
		return -1;
	LOG_FTtoST(&curtime,&cursystime);
	LOG_FTtoST(&fsttime,&fstsystime);
	if(DaynCmp(cursystime, fstsystime, 0) == 0)
		return -1;	// time is incorrect
    else if (DaynCmp(cursystime, fstsystime, saveday) == 0) //early
		return 0; //unexpired
	else
		return 1; //expired
}

/*
static void savetohost()
{
	FILE * fp;
	char buf[500];
	const char * filename ="/mnt/host/log.dat";
	fp = fopen(LOGFILEPATH, "rb");
	if (fp == NULL)
		return;
	fread(buf, 1, 500, fp);
	fclose(fp);
	SaveFile(buf, 500, filename);
	return;
}*/
/*********************************************************************\
* Function:	ResetLog   
* Purpose : reset
* Params  :	
* Return  :	
* Remarks :	   
\**********************************************************************/
void ResetLogSetting()
{
	LOG_DB * plogdb; 
	plogdb = LOG_DB_Open();
	GetLogTime(plogdb);
	WriteLogTime(plogdb, plogdb->fstrcdtime, 30);
	LOG_DB_Close(plogdb);	
}


/*********************************************************************\
* Function:	LOG_UpdateSMSStatus(unsigned long SmsID, SMSSTATUS newstatus)
* Purpose : update status of a recorded sms log
* Params  :	SmsID	    [IN]        ID of the sms to be updated
			newstatus	[IN]		the new status of the sms
* Return  :	TRUE		success£¬
            FALSE		failure
* Remarks :	   
\**********************************************************************/
//*
BOOL LOG_UpdateSMSStatus(unsigned long SmsID, SMSSTATUS newstatus)
{
	BOOL bRet;
	LOG_DB * plogdb;
	
	plogdb = LOG_DB_Open();
	if(NULL == plogdb)
		return FALSE;

	bRet = SeekSMSAndUpdate(plogdb, SmsID, newstatus);
	
	LOG_DB_Close(plogdb);
	
	if(bRet)
		RealTimeUpdateCommunication();
	
	return bRet;
}
//*/

static BOOL SeekSMSAndUpdate(LOG_DB *plogdb, unsigned long smsID, SMSSTATUS newstatus)
{
    DB_REC_HEAD headbuf; 
	SMSTYPE smslog;
	unsigned short ulen;
	
	if(NULL == plogdb)
		return FALSE;

	if(plogdb->savetime <= 0)
		return FALSE;

	// skip file head
	if (fseek(plogdb->fp, sizeof(LOG_FILE_HEAD), SEEK_SET) == -1) 
		return FALSE;
    
    // find the record
    while (fread(&headbuf, sizeof(DB_REC_HEAD), 1, plogdb->fp) == 1)//exit condition: file end
    {
        if ((TYPE_SMS == headbuf.type) && (DRT_RECIEVE != headbuf.direction))
        {
			fread(&ulen, sizeof(unsigned short), 1, plogdb->fp);
            if (sizeof(SMSTYPE) != ulen)
                return FALSE;

			fread(&smslog, ulen, 1, plogdb->fp);
			if(smslog.SmsID == smsID)			// find the sms
			{
				smslog.status = newstatus;		// change the status
				if(0 != fseek(plogdb->fp, 0-ulen, SEEK_CUR))
					return FALSE;
				if(1 != fwrite(&smslog, ulen, 1, plogdb->fp))
					return FALSE;
				return TRUE;
			}
			
			if(0 != fseek(plogdb->fp, (headbuf.len-sizeof(DB_REC_HEAD)-sizeof(unsigned short)-ulen), SEEK_CUR))
				return FALSE;
        }
        // move to the next record
        else if (fseek(plogdb->fp, headbuf.len - sizeof(DB_REC_HEAD), SEEK_CUR) == -1)
            return FALSE;
    }    
    return FALSE;
}
//#endif
