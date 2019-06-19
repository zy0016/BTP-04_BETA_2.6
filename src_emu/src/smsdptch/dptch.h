/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 短信分发模块的内部头文件，包括了模块的相关定义和函数声明。
 *
\**************************************************************************/

//#include <winpda.h>
/*
 外部头文件内容
*/
#ifndef DPTCH_H
#define DPTCH_H

#include <winpda.h>
#include <me_wnd.h> 
/************************************************************************
			以下是模块的外部定义
************************************************************************/
/* 普通短信上层回调函数的函数指针类型定义 */
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

/* 短信分发模块的短信类型 */
#define DPTCHTYPE_NORMAL	1
#define DPTCHTYPE_PORT		2
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

/************************************************************************
			以下是模块的内部定义
************************************************************************/
#define WM_CMT_COMING		(WM_USER+10)
#define WM_CMTI_COMING		(WM_USER+11)
#define WM_CDS_COMING		(WM_USER+12)
#define WM_CDSI_COMING		(WM_USER+13)
#define WM_CBM_COMING		(WM_USER+14)
#define WM_CBMI_COMING		(WM_USER+15)
#define WM_CMTICDSI_READ	(WM_USER+16)
#define WM_CBMI_READ		(WM_USER+17)
#define WM_PORTSMS_SEND		(WM_USER+18)
#define WM_NORMALSMS_SEND	(WM_USER+19)
#define WM_SMSDELETED		(WM_USER+20)
#define WM_ACKNOWLEDGE		(WM_USER+21)
#define WM_ASYN_PORT_SEND	(WM_USER+22)
#define WM_ASYN_NORMAL_SEND	(WM_USER+23)

#ifdef GSM_SMS
	#define NETWORK_PKT_LEN	140
#elif defined GSM_USSD
	#define NETWORK_PKT_LEN 160
#endif

/* UDH相关的宏定义 */
/*
FIELD									LENGTH
Length of User Data Header				1 octet
Information-Element-Identifier "A"		1 octet
Length of Information-Element "A"		1 octet
Information-Element "A" Data			1 to "n" octets
Information-Element-Identifier "B"		1 octet
Length of Information-Element "B"		1 octet
Information-Element "B" Data			1 to "n" octets
Information-Element-Identifier "n"		1 octet
Length of Information-Element "n"		1 octet
Information-Element "n" Data			1 to "n" octets
*/
#define UDHL_FIELD_LEN		1
#define IE_FIELD_LEN		1//Information-Element-Identifier "A" 1 octet
#define LIE_FIELD_LEN		1//Length of Information-Element "A" 1 octet
/* 以下宏定义用于发送带端口地址的短信,目前的IE仅包括16bitport和SAR */
#define PORT_MAX_HEADER_LEN		(2+4+2+3) /* 包括UDHL、16bitIE_PORT和IE_SAR*/
#define PORT_MIN_HEADER_LEN		(2+4)/* 包括UDHL、16bitIE_PORT*/

/* 以下宏定义用于发送不带端口地址的短信,目前的IE仅包括SAR */
#define NORMAL_MAX_HEADER_LEN		(1+2+3)
#define NORMAL_MIN_HEADER_LEN		0
#define NORMAL_MAX_BODY_LEN_SAR		(NETWORK_PKT_LEN - NORMAL_MAX_HEADER_LEN)
#define NORMAL_MAX_BODY_LEN_NOSAR	NETWORK_PKT_LEN

#define MAKE_PORT_ADDR(High,Low)	(((High) << 8) | (unsigned char)(Low)) 

#define IE_PORT_ID		0x05
#define IE_PORT_LEN		0x04
#define IE_SAR_ID		0x00
#define IE_SAR_ID8		0x08
#define IE_SAR_LEN		0x03
#define IE_SAR_LEN8		0x04

#define CTLBLK_CNT		10


typedef struct tagSegment
{
	unsigned char 	len;
	unsigned char 	pdata[NETWORK_PKT_LEN];

}SEGMENT;	


typedef struct tagDatagram
{	
	unsigned short		refnum;
	unsigned char		maxfragment;
	unsigned char		arrivedfragment;
	SEGMENT			*parrSegs;
	struct tagDatagram		*pNext;
	int					dcs;
	char				SendTime[ME_SMS_TIME_LEN];
	Address_t			srcaddr;//源端地址信息。
}DATAGRAM;

typedef struct tagDptchHandle //代表每一个本地上层端口
{	
		int 			isused;
		unsigned short 	addr;	//本地上层端口
		unsigned short	unused;
		HWND 			hwnd;
		UINT			msg;
//		unsigned char	*pSendBuf;		
		DATAGRAM		*pstDatagramLst;  //从不同的发送方接收到数据

}DPTCH_HANDLE;

typedef struct tagDptchCtl
{
	unsigned char	nCurRefNum;
	unsigned char	bInited;
	unsigned char	prehnd;
	unsigned char	unused1;
	HWND			hwnd;
	int				hMECMT;
	int				hMECMTI;
	int				hMECDS;
	int				hMECDSI;
	int				hMECBM;
	int				hMECBMI;
	DPTCH_HANDLE	handles[CTLBLK_CNT];

}DPTCH_CTL;


typedef struct  tagRegister
{
	char  				condition[ME_PHONENUM_LEN]; 
	PSMSTransact_f		pf;
	struct tagRegister 	*pNext;

}REGISTER;

#endif
