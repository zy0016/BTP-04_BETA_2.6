/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : smsdptch.h
 *
 * Purpose  : 短信分发模块的外部接口.
 *
\**************************************************************************/

#ifndef _SMS_DISPATCH_H_
#define _SMS_DISPATCH_H_

#include <winpda.h>
#include <me/me_wnd.h> 

typedef int (*PSMSTransact_f)(int type, void *pInfo);

typedef struct Address
{
	unsigned short	port;
	unsigned char	SCA[ME_PHONENUM_LEN];
	unsigned char	loweraddr[ME_PHONENUM_LEN];

}Address_t;

// Cell Broadcasting Message Info
typedef struct tagBCInfo {
    unsigned short  SNum;       // Serial Number
    unsigned short  MId;        // Message Identifier
    int             Code;       // Alphabet
    int             Lang;       // Language
    int             Class;      // Msg Class
    int             PageNum;    // Total number of page
    int             PageRef;    // page reference
    int             DataLen;
    char            Data[94];   // Message Content
} BC_INFO, *PBC_INFO;

typedef struct tagPortSMS_Recv
{
	unsigned char *pBuf;
	int nBufLen;
	int nDataLen;
	int nCodeMode;
	char Time[ME_SMS_TIME_LEN];
	
} PORT_RECV;

typedef struct tagPortSMS_Send
{
	unsigned char *pData;
	int DataLen;
	int CodeMode;
} PORT_SEND;

typedef struct tagNormalSMS_Send
{
	int		dcs;
#define DCS_GSM         0
#define DCS_8BIT        1
#define DCS_UCS2        2
	char	SCA[ME_PHONENUM_LEN];
	int		Type;
	int		DataLen;
	BOOL	Report;
	BOOL	ReplyPath;
	int		PID;
#define		PID_IMPLICIT	0
#define		PID_TELEX		1
#define		PID_TELEFAX		2
#define		PID_VOICE		3
#define		PID_ERMES		4
#define		PID_NPAGING		5
#define		PID_X400		6
#define		PID_EMAIL		7
	
	int		VPvalue;
#define SMS_1HOUR	0	//一小时
#define SMS_6HOUR	1	//六小时
#define SMS_24HOUR	2	//二十四小时
#define SMS_72HOUR	3	//七十二小时
#define SMS_1WEEK	4	//一星期
#define SMS_MAXTIME	5	//最长时间
	
	char	PhoneNum[ME_PHONENUM_LEN];
	char	*pData;
	
}NORMAL_SEND, *PNORMAL_SEND;


typedef struct tagNormal_Recv
{
	int		segments;
	int		sequence;
	int		dcs;
	int		Index;
	int		Stat;
	int		Type;
	int		Status;
	int		ConLen;
	int		Class;
#define SMS_CLASS_NONE	0
#define SMS_CLASS_0		1
#define SMS_CLASS_1		2
#define SMS_CLASS_2		3
#define SMS_CLASS_3		4
	char	SCA[ME_PHONENUM_LEN];
	char	SenderNum[ME_PHONENUM_LEN];
	char	SendTime[ME_SMS_TIME_LEN];
	char	Context[ME_SMS_CONTEXT_LEN];
	
}NORMAL_RECV, *PNORMAL_RECV;

/*
 Error code 	
*/
#define ERR_SUCCESS		0
#define ERR_FAILURE		-1
#define ERR_SYS			-2
#define ERR_PARAM		-3
#define ERR_CTLBLK		-4

/* 普通短信的细分类型 */
#define NORMAL_SMS			1 
#define BC_SMS				2
#define STATUS_REPORT_SMS	3

/*
 External interface functions.
*/
int SMSDISPATCH_Init		(void);
int SMSDISPATCH_Shutdown	(void);
/*
 For NORMAL SMS.
*/
int SMSDISPATCH_Register	(char *pCondition, PSMSTransact_f pCbFun);
int SMSDISPATCH_SendSMS		(HWND hWnd, int Msg, NORMAL_SEND stSMSSend);
int SMSDISPATCH_UnReg		(char *pCondition);
/*
 For PORT SMS.
*/
int SMSDISPATCH_Open		(void);
int SMSDISPATCH_Close		(int handle);
int SMSDISPATCH_Ioctl		(int handle, unsigned short port, HWND hwnd, UINT msg);
int SMSDISPATCH_GetFragNum	(int type, int dcs, int DataLen, int *pnSegs);
int SMSDISPATCH_Send		(int handle, HWND hWnd, int msg, Address_t destaddr, PORT_SEND SendInfo);
int SMSDISPATCH_Recv		(int handle, Address_t *psrcaddr, PORT_RECV *pRecvInfo);


#endif
