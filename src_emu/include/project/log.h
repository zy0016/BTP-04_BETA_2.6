/**************************************************************************\
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
* Purpose : Log 
*  
* Author  : 
 *
\**************************************************************************/

#ifndef _LOG_H
#define _LOG_H
#include "window.h"

#define  LOG_DB_SUCCESS	 0
#define  LOG_DB_FAILURE	 -1 
#define  LOG_DB_EXPIRED  -3;

#define MAX_LEN 40
//record type
typedef enum {
	TYPE_VOICE = 0,
    TYPE_DATA,
	TYPE_GPRS,
	TYPE_SMS
} RCDTYPE;

//direction
typedef enum {
	DRT_SEND = 0,
    DRT_RECIEVE,
	DRT_UNRECIEVE
} DRTTYPE;

//sms status
typedef enum {
	SMS_SEND_UNCONFIRM = 0,
    SMS_SEND_CONFIRM,
	SMS_WAIT_SEND,
	SMS_SEND_FAIL,
	SMS_RECEIVE
} SMSSTATUS;

typedef struct  tagVoiceType {
	unsigned long endtm;
	unsigned long duration;
}VOICETYPE;

typedef struct  tagDataType {
	unsigned long endtm;
	unsigned long duration;
}DATATYPE;

typedef struct  tagGPRSType {
    unsigned long endtm;
   	unsigned long duration;
	unsigned long sendcounter;
	unsigned long recievecounter;
}GPRSTYPE;

typedef struct  tagSMSType {
	SMSSTATUS status;
	unsigned long SmsID;
	unsigned long smscounter;
}SMSTYPE;

typedef struct tagLogRecord {
	unsigned long index;                             //record index, must be filled
	RCDTYPE type;                          //record type, must be filled
	DRTTYPE direction;                     //record direction, must be filled
	unsigned long begintm;                 //record begin time, must be filled
    union{
        VOICETYPE voice;
        DATATYPE  data;
        GPRSTYPE gprs;
        SMSTYPE sms;
	}u;	
	char APN[1];                     //for data and GPRS, it's access point name
                                           //for voice and sms, it's name of the call number
} LOGRECORD, *PLOGRECORD;

typedef struct tagCallSetting {
	unsigned long savetime;
	int IsShowTime;
	int  CountType;
	int IsLimitCount;
	int IsShowCount;
} CALLSETTING, *PCALLSETTING;

//typedef struct tagListWndInfo{
//	HWND hListWnd;
//	UINT wLCMsg;
//}LISTWNDINFO;

typedef struct tagReadWndInfo{
	HWND hReadWnd;
	UINT wRCMsg;
}READWNDINFO;

int Log_Init (void);
//int Log_Count (void);
//int Log_List (LOGRECORD* buf, int count);
int Log_Read (void* buf, unsigned short idx, unsigned short buflen);
int Log_Write (LOGRECORD* plogrec);
int Log_Empty (void);
//int Log_SelectRCDTYPE (RCDTYPE type);
//int Log_SelectRCDTYPE (DRTTYPE type);
void Log_RegisterReadWnd(HWND hWnd, UINT wMsg);
void Log_UnRegisterReadWnd();
int Log_Expired(void);
void LOG_FTtoST(unsigned long * pTime, SYSTEMTIME* pSysTime);
void LOG_STtoFT(SYSTEMTIME* pSysTime, unsigned long* pTime);
void ResetLogSetting();

#endif
