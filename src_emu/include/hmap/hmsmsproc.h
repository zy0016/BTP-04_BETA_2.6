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
#ifndef _SMSPROC_H_

#define _SMSPROC_H_

/* Coding Style Define */
#define DCS_Alpha	1 // GSM
#define DCS_Data	2 // DCS_Data	
#define DCS_UCS2	3 // Unicode

#define		PID_IMPLICIT	0
#define		PID_TELEX		1
#define		PID_TELEFAX		2
#define		PID_VOICE		3
#define		PID_ERMES		4
#define		PID_NPAGING		5
#define		PID_X400		6
#define		PID_EMAIL		7

#define SMS_1HOUR	0	
#define SMS_6HOUR	1	
#define SMS_24HOUR	2	
#define SMS_72HOUR	3	
#define SMS_1WEEK	4	
#define SMS_MAXTIME	5	


#if defined INFINION_MODULE
#define SMS_TELNUMLEN			44
#else
#define SMS_TELNUMLEN			24
#endif


#define SPLIT_MODE_ONE	1
#define SPLIT_MODE_ALL	2

#define SMS_SUCCESS		0
#define SMS_FAILURE		-1
#define SMS_ERR_PARAM	-2
#define SMS_ERR_BUF		-3

enum sms_type { SMS_SUBMIT, SUBMIT_REPORT, STATUS_REPORT, SMS_DELIVER };

typedef struct tagSMS_Data
{
    struct tagSMS_Data* pNext;  
    unsigned long foff;         
    unsigned long flags;        
    unsigned long MsgID;        
    unsigned long time;         
    unsigned short totalen;           
    unsigned char MsgType;      
    unsigned char Coding;       
    unsigned short Segments;    
    unsigned short index;       
    unsigned short RefNumber;   
    unsigned short Datalen;     	
    unsigned char ctnum[SMS_TELNUMLEN];    
    unsigned char* number;   
	unsigned char sendernumber[SMS_TELNUMLEN];
    unsigned char MEIDIndex[8];
	unsigned long numlength;
	unsigned long reportstat;   	                            
    unsigned char* pData;       
    union 
    {
        struct 
        {
            unsigned char TP_FLAGS;
            unsigned char TP_PID;
            unsigned char TP_DCS;
            unsigned char TP_UDL;
            unsigned char TP_SCTS[7];
            unsigned char unused[5];
        } Delever;
        struct 
        {
            unsigned char TP_FLAGS;
            unsigned char TP_MR;
            unsigned char TP_PID;
            unsigned char TP_DCS;
            unsigned char TP_UDL;
            unsigned char TP_VP[7];
            unsigned long sendhd;   
        } Submit;
        struct 
        {
            unsigned char TP_FLAGS;
			unsigned char TP_MR;
			unsigned char TP_SCTS[7];
			unsigned char TP_DT[7];
			unsigned char TP_ST;
			unsigned char TP_PI;
			unsigned char TP_PID;
			unsigned char TP_DCS;
			unsigned char unused[4];
        } StatusReport;
    }u;
}SMS_Data;

typedef struct tagSMSDataBuf
{
	int buflen;
	int datalen;
	int UDHlen;	
	unsigned char * pBuf;	
}SMSDataBuf;


int SMS_Split (const void * pSrcData, int datalen, int dcs,
				unsigned char * pUDH, int UDHlen, int UDHMode,
				int bufnum, SMSDataBuf * pBufList);
int SMS_Encode (SMS_Data * pSmsData, int validity_period,
				unsigned char * pBuf, int buflen);
SMS_Data* SMS_Decode (unsigned char * pSmsData, int datalen,
					  unsigned char * pBuf, int buflen);
int SMS_Combine (SMS_Data ** pSmsHeadList, unsigned char * pBuf, int buflen);

#endif