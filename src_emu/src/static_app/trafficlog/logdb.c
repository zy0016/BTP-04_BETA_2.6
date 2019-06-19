 /**************************************************************************\
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Model   : traffic log
*
* Purpose : Log DB
*  
* Author  : 
*
\**************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "logdb.h"

#define MAX_EXPIRE_LOGS	20	// there may be 20 expired logs in the db at most

static BOOL bDBOpened = FALSE;
static FILE *MoveToNewFile(FILE *oldfile, long offset);

/*********************************************************************\
* Function: LOG_DB_Open	   
* Purpose : open db object 
    
* Params  : db_filename     
	   
* Return  : Success return the pointer of db object
*           Fail return NULL
* Remarks	   
**********************************************************************/

void* LOG_DB_Open(void)
{
    LOG_DB *log_db;
    FILE * fp;
	LOG_FILE_HEAD buf;
    char dbfilepath[50];

	if(bDBOpened)
	{	// this definitely should NOT happen!
		printf("\r\n[LOG_DB_OPEN]: db already opened. second try refused!\r\n");
//		ASSERT(0);	//*********
		return NULL;
	}

	strcpy(dbfilepath, LOGFILEPATH);
	
//	remove(LOGFILEPATH);
//	return NULL;

	if ((fp = fopen(dbfilepath, "r+b")) == NULL)
	{
		fp = fopen(dbfilepath, "w+b"); 
		if (fp == NULL)
			return NULL;
		memset(&buf, 0, sizeof(LOG_FILE_HEAD));
		//the default duration is 30 days
		buf.savetime = 30;
		fseek(fp, 0, SEEK_SET);
		if (fwrite(&buf, sizeof(LOG_FILE_HEAD), 1, fp) != 1)
		{
			fclose(fp);
			return NULL;
		}
	}
	if (fp  == NULL)
		return NULL;
    if ((log_db = (LOG_DB*)malloc(sizeof(LOG_DB))) == NULL)
	{
		fclose(fp);
		return NULL;
	}
    memset(log_db, 0, sizeof(LOG_DB));
    log_db->fp = fp;

	GetLogTime(log_db);
	bDBOpened = TRUE;
    return log_db;
}

/*********************************************************************\
* Function:	LOG_DB_Close   
* Purpose : close db object  
* Params  : log_db  	   
* Return  : LOG_DB_FAILURE,	LOG_DB_SUCCESS 	   
\**********************************************************************/
int LOG_DB_Close(void * log_db)
{
    LOG_DB *plog_db;

    if (log_db == NULL)
        return LOG_DB_FAILURE;

    plog_db = (LOG_DB*)log_db;
	if(plog_db->fp)
	    fclose(plog_db->fp);
    free(plog_db);
	bDBOpened = FALSE;
    return LOG_DB_SUCCESS;
}

/*********************************************************************\
* Function: GetCurMaxID   
* Purpose : Get Max ID in db    
* Params  : log_db  	   
* Return  : Success return max id 
            no record return -1			
			failure return -2
\**********************************************************************/
int GetCurMaxID(LOG_DB *log_db)
{
   DB_REC_HEAD buf;

//   if (GetDBFlag(log_db) == -1) {
//	   return log_db->max_id = -1; 
//   }
//   else if (GetDBFlag(log_db) == -2)
//	   return -2;
   if (fseek(log_db->fp, sizeof(LOG_FILE_HEAD), SEEK_SET) == -1)
		return -2;
   log_db->max_id = -1;
   while (fread(&buf, sizeof(DB_REC_HEAD), 1, log_db->fp) == 1) // exit condition: end or err
   {
	   log_db->max_id ++; //= (buf.id>log_db->max_id ? buf.id :log_db->max_id);
	   if(fseek(log_db->fp, buf.len- sizeof(DB_REC_HEAD), SEEK_CUR) == -1)
		   return -2;
   }
   return log_db->max_id;
}

int GetDBCounter(LOG_DB *log_db)
{
	int counter;
	counter = GetCurMaxID(log_db)+1;
	return counter;
}
/*********************************************************************\
* Function: UpdateLogDB   
* Purpose : check expire status and remove old records
* Params  : log_db  	   
* Return  : TRUE	success
            FALSE	failure
\**********************************************************************/
BOOL UpdateLogDB(LOG_DB *log_db)
{
	SYSTEMTIME cursystm;
	DB_REC_HEAD headbuf; 
	unsigned long curtime, savetime; 
	long offset = -1;
	int nRet, m = 0, n = 0;
	
	if (GetLogTime(log_db) == -1) 
		return -2;
	
	savetime = log_db->savetime;
	GetLocalTime(&cursystm);
	LOG_STtoFT(&cursystm, &curtime);
	
	/* skip file head*/
	if (fseek(log_db->fp, sizeof(LOG_FILE_HEAD), SEEK_SET) == -1) 
		return FALSE;
	
    /* check the records */
    while (fread(&headbuf, sizeof(DB_REC_HEAD), 1, log_db->fp) == 1)//exit condition: file end
    {
		// decide if the item expired
		nRet = dayncmp(curtime, headbuf.rectime, savetime);

		if(-1 == nRet)		// item with wrong time, just skip it
			m++;
		else if(1 == nRet)	// expired item
		{					// if any items before it recorded wrong time, 
			n += (1+m);		// they will be also taken as expired
			m = 0;
		}
		else				// valid item
		{
			if(n<MAX_EXPIRE_LOGS)	// the amount of old logs is acceptable
				return TRUE;
			else					// the db need to be rebuilt in this case
			{
				offset = ftell(log_db->fp) - sizeof(DB_REC_HEAD);
				if(offset < 0)
					return FALSE;

				log_db->fp = MoveToNewFile(log_db->fp, offset);
				return (log_db->fp != NULL);
			}
		}
		/* move to the next record */
        if (fseek(log_db->fp, headbuf.len - sizeof(DB_REC_HEAD), SEEK_CUR) == -1)
            return FALSE;
	}
	
	// all logs are expired
	if((n > 0) && (0 == m))
		log_db->fp = MoveToNewFile(log_db->fp, -1);
	return (log_db->fp != NULL);
}

/*********************************************************************\
* Function: MoveToNewFile   
* Purpose : move necessary info into a new file
			to throw the expired logs
* Params  : oldfile		pointer to the old db file
			offset		the position where useful data starts
* Return  : if fails, return the old file pointer
			else return the pointer to the new file
\**********************************************************************/
static FILE *MoveToNewFile(FILE *oldfile, long offset)
{
	FILE *newfile = NULL;
	long data_size = -1;
	LOG_FILE_HEAD log_head;
	unsigned char *pData = NULL;

	if(NULL == oldfile)
		return NULL;
	
	rewind(oldfile);

	// copy the header first
	if(1 != fread(&log_head, sizeof(LOG_FILE_HEAD), 1, oldfile))
		return oldfile;

	// get the useful part of logs
	if(offset > sizeof(LOG_FILE_HEAD))
	{
		fseek(oldfile, 0, SEEK_END);
		data_size = ftell(oldfile);
		
		if(data_size < offset)
			return oldfile;
		
		data_size -= offset;
		pData = (unsigned char *)malloc(data_size);

		if(NULL == pData)
			return oldfile;
		
		if(1 != fread(pData, data_size, 1, oldfile))
		{
			free(pData);
			return oldfile;
		}
	}

	// write to new file
	fclose(oldfile);
	newfile = fopen(LOGFILEPATH, "w+b");
	if (newfile == NULL)
		return NULL;

	fwrite(&log_head, sizeof(LOG_FILE_HEAD), 1, newfile);
	if(NULL != pData)
	{
		fwrite(pData, data_size, 1, newfile);
		free(pData);
	}
	return newfile;
}

/*********************************************************************\
* Function: GetLogTime   
* Purpose : Get Log time   
* Params  : log_db  	   
* Return  : success fail  
\**********************************************************************/
int GetLogTime(LOG_DB *log_db)
{
	LOG_FILE_HEAD buf;

	if((NULL == log_db) || (NULL == log_db->fp))
		return LOG_DB_FAILURE;

	if (fseek(log_db->fp, 0, SEEK_SET) == -1)
		return LOG_DB_FAILURE;
	
	if (fread(&buf, sizeof(LOG_FILE_HEAD), 1, log_db->fp) == 1)
	{
		log_db->fstrcdtime = buf.fstrcdtime;
		log_db->savetime = buf.savetime;
		return LOG_DB_SUCCESS;
	}
	return LOG_DB_FAILURE;
}
/*********************************************************************\
* Function: WriteLogTime   
* Purpose : Write Log time   
* Params  : log_db  	   
* Return  : success fail  
\**********************************************************************/
int WriteLogTime(LOG_DB *log_db, unsigned long fstrectime, unsigned long savetime)
{
	LOG_FILE_HEAD buf;

	if(NULL == log_db || NULL == log_db->fp)
		return LOG_DB_FAILURE;

	buf.fstrcdtime = fstrectime;
	buf.savetime = savetime;
	
	if (fseek(log_db->fp, 0, SEEK_SET) == -1)
		return LOG_DB_FAILURE;
	if (fwrite(&buf, sizeof(LOG_FILE_HEAD), 1, log_db->fp) == 1)
		return LOG_DB_SUCCESS;

	return LOG_DB_FAILURE;
}

/*********************************************************************\
* Function:	LOG_DB_Add   
* Purpose : Add reord 
    
* Params  :	log_db	          
            log_record	  

* Return  :	LOG_DB_SUCCESS£¬Success 
            LOG_DB_FAILURE£¬Failure
* Remarks :	   
\**********************************************************************/
int LOG_DB_Add(LOG_DB *log_db, const LOGRECORD* log_db_record)
{
    LOG_DB *plog_db;
    DB_REC_HEAD headbuf;
    unsigned short apnlen = 0, ulen = 0; 

    memset(&headbuf, 0, sizeof(DB_REC_HEAD));
  
    if ((log_db == NULL) || (log_db_record == NULL))
        return LOG_DB_FAILURE;
    plog_db = (LOG_DB*)log_db;

//    if ((maxid = GetCurMaxID(plog_db)) == -2)
//        return LOG_DB_FAILURE;
//    headbuf.id = maxid + 1;
	headbuf.direction = log_db_record->direction;
	headbuf.rectime = log_db_record->begintm;
	headbuf.type = log_db_record->type;

    /* seek file end */
    if (fseek(plog_db->fp, 0, SEEK_END) == -1)
        return LOG_DB_FAILURE;

	/* count record len */
    headbuf.len += sizeof(DB_REC_HEAD); //record head len 

	headbuf.len += sizeof(unsigned short); //u len
    switch(headbuf.type) {
    case TYPE_VOICE:
		ulen = sizeof(VOICETYPE);
		headbuf.len += ulen;
    	break;
    case TYPE_DATA:
		ulen = sizeof(DATATYPE);
		headbuf.len += ulen;
    	break;
	case TYPE_GPRS:
		ulen = sizeof(GPRSTYPE);
		headbuf.len += ulen;
		break;
	case TYPE_SMS:
		ulen = sizeof(SMSTYPE);
		headbuf.len += ulen;
		break;
    default:
		return LOG_DB_FAILURE;
    }

    headbuf.len += sizeof(unsigned short);  // APN_len 
    if (log_db_record->APN)
    {
        apnlen = strlen(log_db_record->APN);
        headbuf.len += apnlen;
    }
	else
		return LOG_DB_FAILURE;

	/* start write record including head*/
	fwrite(&headbuf, sizeof(DB_REC_HEAD), 1, log_db->fp);  //write head
	
    /* write record */
	fwrite(&ulen, sizeof(unsigned short), 1, log_db->fp);
    switch(headbuf.type) {
    case TYPE_VOICE:
		fwrite(&(log_db_record->u.voice), 1, ulen, log_db->fp);
    	break;
    case TYPE_DATA:
		fwrite(&(log_db_record->u.data), 1, ulen, log_db->fp);
    	break;
	case TYPE_GPRS:
		fwrite(&(log_db_record->u.gprs), 1, ulen, log_db->fp);
		break;
	case TYPE_SMS:
		fwrite(&(log_db_record->u.sms), 1, ulen, log_db->fp);
		break;
    default:
		return LOG_DB_FAILURE;
    }
	fwrite(&apnlen, sizeof(unsigned short), 1, log_db->fp);
	fwrite(log_db_record->APN, 1, sizeof(char)*apnlen, log_db->fp);
    fflush(log_db->fp);
    return LOG_DB_SUCCESS;
}

/*********************************************************************\
* Function : FindAccordID	   
* Purpose  : From current location find requested id, and then move point
             to the next record;
* Params   : log_db      
             id          
             offset       
* Return   : Success 0 £¬Failure-1	 	   
\**********************************************************************/
static int FindAccordID(LOG_DB *log_db, unsigned short id, unsigned long *offset)
{
    DB_REC_HEAD headbuf; 
    long cur_offset;
	int index = 0;

    /* find from head */
    if (fseek(log_db->fp, 0, SEEK_SET) == -1)
        return -1;
	/* skip file head*/
	if (fseek(log_db->fp, sizeof(LOG_FILE_HEAD), SEEK_SET) == -1) 
		return -1;
    
    /* find the record */
    while (fread(&headbuf, sizeof(DB_REC_HEAD), 1, log_db->fp) == 1)//exit condition: file end
    {
        if (/*headbuf.id*/index == id)
        {
            if (fseek(log_db->fp, 0, SEEK_CUR) == -1)
                return -1;
			cur_offset = ftell(log_db->fp);
            *offset = cur_offset - sizeof(DB_REC_HEAD);
            return 0;
        }
        /* move to the next record */
        if (fseek(log_db->fp, headbuf.len - sizeof(DB_REC_HEAD), SEEK_CUR) == -1)
            return -1;
		index ++;
    }    
    return -1;
}

/*********************************************************************\
* Function:	LOG_DB_Get   
* Purpose : use record ID to get record 
* Params  :	log_db	    [IN]        log db object point
            id	        [IN]        record ID
            record_buf  [IN]/[OUT]	record buf
            record_size	[IN]        buf len
  
* Return  :	if return != NULL£¬DRM_DB_SUCCESS£¬
                               DRM_DB_FAILURE£»
            else success return = buflen
			     failure return = -1
* Remarks :	   
\**********************************************************************/

int LOG_DB_Get(void* log_db, unsigned long id, void* record_buf, int record_size)
{
    LOG_DB *plog_db; 
    LOGRECORD *log_db_record;
	int apnlen = 0, structlen = 0;
	char * apn;
	void * ustruct;
	VOICETYPE * pvoice;
    DATATYPE  * pdata;
	GPRSTYPE  * pgprs;
	SMSTYPE   * psms;
    int  recsize = 0;
    unsigned long offset = 0;
    DB_REC_HEAD headbuf;

	SYSTEMTIME cursystm;
	unsigned long curtime;
    
    if (log_db == NULL)
        return (!record_buf ? -1 : LOG_DB_FAILURE );
    plog_db = (LOG_DB *)log_db;

	if(FindAccordID(plog_db, (unsigned short)id, &offset) == -1)
		return (!record_buf ? -1 : LOG_DB_FAILURE );

    
    if (fseek(plog_db->fp, offset, SEEK_SET) == -1)
        return (!record_buf ? -1 : LOG_DB_FAILURE );

    /* read record head */
    fread(&headbuf, sizeof(DB_REC_HEAD), 1, plog_db->fp);
	fread(&structlen, sizeof(unsigned short), 1, plog_db->fp);
	if (structlen == 0)
		return LOG_DB_FAILURE;
	ustruct = malloc(structlen);
	if (fread(ustruct, 1, structlen, plog_db->fp) != (size_t)structlen) 
		return LOG_DB_FAILURE;
    fread(&apnlen, sizeof(unsigned short), 1, plog_db->fp);
	if (apnlen == 0)
		return LOG_DB_FAILURE;
	recsize = sizeof(LOGRECORD) + apnlen;
	/* log_db_record len = headbuf.len- sizeof(unsigned short) - apnlen - structlen*/
//	recsize = headbuf.len-sizeof(DB_REC_HEAD)-2* sizeof(unsigned short)+sizeof(LOGRECORD)-structlen;
	if (record_buf == NULL)
		return recsize;
	else if (record_size < recsize)
		return LOG_DB_FAILURE;

	// decide if the item is expired
	GetLocalTime(&cursystm);
	LOG_STtoFT(&cursystm, &curtime);
	if (dayncmp(curtime, headbuf.rectime, plog_db->savetime) == 1)
	{
		return LOG_DB_EXPIRED;
	}

	// read the item
	memset(record_buf, 0, record_size);
	log_db_record = (LOGRECORD *)record_buf;
    log_db_record->index = headbuf.id;
    log_db_record->type = headbuf.type;
	log_db_record->direction = headbuf.direction;
	log_db_record->begintm = headbuf.rectime;

//	fread(&structlen, sizeof(unsigned short), 1, plog_db->fp);
//	if (structlen == 0)
//		return LOG_DB_FAILURE;
//	ustruct = malloc(structlen);
//	if (fread(ustruct, 1, structlen, plog_db->fp) != (size_t)structlen) 
//		return LOG_DB_FAILURE;
	switch(log_db_record->type)
	{
	case TYPE_VOICE:
        pvoice = (VOICETYPE *) ustruct;
		log_db_record->u.voice.duration = pvoice->duration;
		break;
	case TYPE_DATA:
		pdata = (DATATYPE *) ustruct;
		log_db_record->u.data.duration = pdata->duration;
		break;
	case TYPE_GPRS:
		pgprs = (GPRSTYPE *) ustruct;
		log_db_record->u.gprs.duration = pgprs->duration;
		log_db_record->u.gprs.recievecounter = pgprs->recievecounter;
		log_db_record->u.gprs.sendcounter = pgprs->sendcounter;
		break;
	case TYPE_SMS:
		psms = (SMSTYPE *) ustruct;
		log_db_record->u.sms.smscounter = psms->smscounter;
		log_db_record->u.sms.status = psms->status;
		log_db_record->u.sms.SmsID = psms->SmsID;
		break;
	default:
		return LOG_DB_FAILURE;
	}
    free(ustruct);

//    fread(&apnlen, sizeof(unsigned short), 1, plog_db->fp);
//	if (apnlen == 0)
//		return LOG_DB_FAILURE;
	apn = malloc(apnlen);
	if (fread(apn, 1, apnlen, plog_db->fp) != (size_t)apnlen)
		return LOG_DB_FAILURE;
	strncpy(log_db_record->APN, apn, apnlen);
	free(apn);

	return LOG_DB_SUCCESS;
}
/*********************************************************************\
* Function:	LOG_DB_Clear   
* Purpose : delete log file and recreate
* Params  :	log_db	    [IN]        log db object point  
* Return  :	-1 success£¬
             0 failure
* Remarks :	   
\**********************************************************************/
int LOG_DB_Clear(void * log_db)
{
	LOG_DB *plog_db; 
	FILE * fp;
	LOG_FILE_HEAD buf;
//	char dbfilepath[50];    
//	strcpy(dbfilepath, LOGFILEPATH);

	if (log_db == NULL)
		return -1;
	plog_db = (LOG_DB *)log_db;

//	memset(&buf, 0, sizeof(LOG_FILE_HEAD));
//	fread(&buf, sizeof(LOG_FILE_HEAD), 1, plog_db->fp);
	buf.fstrcdtime = 0;
	buf.savetime = plog_db->savetime;
	
	if(plog_db->fp)
		fclose(plog_db->fp);
	
	fp = fopen(LOGFILEPATH, "w+b");
	if (fp == NULL)
		return -1;
	
	fseek(fp, 0, SEEK_SET);
	if (fwrite(&buf, sizeof(LOG_FILE_HEAD), 1, fp) != 1)
		return -1;
	plog_db->fp = fp;
	return 0;
}
