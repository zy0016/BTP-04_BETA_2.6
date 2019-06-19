/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	 IrDA
 *
 * Filename		irdaglb.h
 *
 * Purpose      Define some global const and structure about IRDA.
 *				Those items only open to Irda Programmer.				
 *  
 * Author       chriszhao
 *
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Revision::    $     $Date::                                     $
 *
 * $History::                                                       $
 * 
\**************************************************************************/

#ifndef _IRDAGLB_H
#define _IRDAGLB_H

#if __cplusplus
extern "C" {
#endif

typedef struct tagIrDAData
{
	unsigned char * pData;			//����ָ��
	int len;						//���ݳ���
}IrDAData;


/* charset Define */
#define IRLM_CHARSET_ASCII		0
#define IRLM_CHARSET_ISO8859_1	1
#define IRLM_CHARSET_ISO8859_2	2
#define IRLM_CHARSET_ISO8859_3	3
#define IRLM_CHARSET_ISO8859_4	4
#define IRLM_CHARSET_ISO8859_5	5
#define IRLM_CHARSET_ISO8859_6	6
#define IRLM_CHARSET_ISO8859_7	7
#define IRLM_CHARSET_ISO8859_8	8
#define IRLM_CHARSET_ISO8859_9	9
#define IRLM_CHARSET_UNICODE	0xff

/* IAS������ */

/* ���Խṹ */
typedef struct tagLMIAS_Attribute
{
	struct tagLMIAS_Attribute* pNext;
	IrDAData AttrName;					//��������	
	unsigned char type;			//attribute type
	unsigned char charset;		//char set
	unsigned short length;		//value lenght
	unsigned char Value[4];		//value
}LMIAS_Attribute;

/* IAS���ݶ���ṹ */
typedef struct tagLMIAS_Object
{
	struct tagLMIAS_Object* pNext;
	IrDAData ClassName;					//����
	unsigned short flags;				//���
	unsigned short ObjId;				//�����ʶ
	struct tagLMIAS_Attribute* pAttr;	//�����б�
}LMIAS_Object;

typedef struct tagLMIAS_GetInfo
{
	unsigned short ObjNum;		//number of object
	unsigned short LagObjId;	//largest object identifier
}LMIAS_GetInfo;

typedef struct tagLMIAS_GetObjectSub
{
	struct tagLMIAS_GetObjectSub* pNext;
	unsigned short ObjId;		//object identifier
	unsigned char Attrnum;		//number of attributes
	unsigned char NameLen;		//length of octet sequence, may be zero
	unsigned char Name[4];		//class name
}LMIAS_GetObjectSub;

typedef struct tagLMIAS_GetObject
{
	unsigned short NextId;		//next identifier after list end
	unsigned short Listnum;		//list length
	struct tagLMIAS_GetObjectSub* pList;	//data
}LMIAS_GetObject;

typedef struct tagLMIAS_GetValue
{
	unsigned short Unused;		//
	unsigned short Listnum;		//list length
	struct tagLMIAS_Attribute* pList;	//data����ֻ��VALUE����������Ч
}LMIAS_GetValue;

typedef struct tagLMIAS_GetValueByClassSub
{
	struct tagLMIAS_GetValueByClassSub* pNext;
	unsigned short ObjId;		//object identifier	
	unsigned short Unused;
	struct tagLMIAS_Attribute* Value;	//data
}LMIAS_GetValueByClassSub;

typedef struct tagLMIAS_GetValueByClass
{
	unsigned short Unused;
	unsigned short Listnum;		//list length
	struct tagLMIAS_GetValueByClassSub* pList;
}LMIAS_GetValueByClass;

typedef struct tagLMIAS_GetObjectInfo
{
	unsigned short lower_slot;	//lowest slot
	unsigned short High_slot;	//highest slot
	unsigned short slotnum;		//number of slots in use
}LMIAS_GetObjectInfo;

typedef struct tagLMIAS_GetAttrSub
{
	struct tagLMIAS_GetAttrSub* pNext;
	unsigned char Namelen;		//length of attribute name
	unsigned char valuetype;	//value type
	unsigned char Name[4];		//attribute name
}LMIAS_GetAttrSub;

typedef struct tagLMIAS_GetAttr
{
	unsigned short Nextslot;	//next slot in use after last list item
	unsigned short Listnum;		//list length
	struct tagLMIAS_GetAttrSub* pList;	//data
}LMIAS_GetAttr;

//int LMIAS_SetTimeOut( unsigned long Time );
//int LMIAS_SetAsync( int ifAsync );
int LMIAS_GetAnswer( int Hd, int* answer );
int LMIAS_CancleOption( int Hd );

int LMIAS_RegisterInformation( LMIAS_Object* pObject );

int LMIAS_GetInfoBaseDetails_Req( unsigned long address, IrDAData* pResult, int timeout );
int LMIAS_GetObjects_Req( unsigned long address, int firstID, int maxList, 
						IrDAData* ClassName, IrDAData* pResult, int timeout );
int LMIAS_GetValue_Req( unsigned long address, int ID, int attrnum, 
					  IrDAData* pAttrName, IrDAData* pResult, int timeout );
int LMIAS_GetValueByClass_Req( unsigned long address, IrDAData* ClassName, 
							 IrDAData* AttrName, IrDAData* pResult, int timeout );
int LMIAS_GetObjectInfo_Req( unsigned long address, int ID, IrDAData* pResult, int timeout );
int LMIAS_GetAttributeNames_Req( unsigned long address, int ID, int FirstSlot, 
							   int NameNum, IrDAData* pResult, int timeout );

#define IRLLSAPIASNUM				0x00
#define IRLLSAPCONNECTKLESSNUM		0x70
#define IRLLSAPINVALID				0x7f

/* LM-IAS����ֵ */
#define IRA_SUCCESS			0
#define IRA_UNSUPPORT		0xff	//Unsupport optioned operation
#define IRA_EWOULDBLOCK		-1		//Would block, option doing...
#define IRA_EBUFFERLOW		-2		//User buffer so short, needed length write in para
#define IRA_ENOSUCHCLASS	-3		//No such class, no other results
#define IRA_ENOSUCHOBJECT	-4		//No such object, no other results
#define IRA_ENOSUCHATTR		-5		//No such attribute, no other results
#define IRA_ENOMOREATTR		-6		//One or more attribute name do not exist, results as show
									//Attribute name in error will have a value type of "missing"
#define IRA_EATTRNAMELONG	-7		//Attribute name list too long, list length result field give maximum accepted length
#define IRA_ETIMEOUT		-8		//Time out
#define IRA_ESOURCE			-9		//resource low
#define IRA_EPARAMETER		-10
#define IRA_EUSERCANCLE		-11
#define IRA_NOTSUPPORT		-12
#define IRA_FAILURE			-100	//Unknow error

/* �������� */
#define IRA_AT_MISSING			0
#define IRA_AT_INTEGER			1
#define IRA_AT_OCTETAEQ			2
#define IRA_AT_USERSTR			3

/* �����豸������DEVICE�� */
#define LMIAS_DV_CLASSNAME			"Device"			//Device Object Class Name
#define LMIAS_DV_DVNAMEATTRNAME		"DeviceName"		//Device Name Attribute Name
#define LMIAS_DV_ISATTRNAME			"IrLMPSupport"		//IrLMP Support Attribute Name
#define LMIAS_DVISA_GETINFO			0x01				//IrLMP IAS Support GetInfoBaseDetails
#define LMIAS_DVISA_GETOBJECT		0x02				//IrLMP IAS Support GetObjects
#define LMIAS_DVISA_GETVALUE		0x04				//IrLMP IAS Support GetValue
#define LMIAS_DVISA_GETOBJECTINFO	0x10				//IrLMP IAS Support GetObjectInfo
#define LMIAS_DVISA_GETATTR			0x20				//IrLMP IAS Support GetAttributeNames

#define LMIAS_DVISA_EXCLUSIVE		0x01				//IrLMP MUX Support Exclusive Mode
#define LMIAS_DVISA_ROLEEXCHANGE	0x02				//IrLMP MUX Support Role Exchange
#define LMIAS_DVISA_CONNECTIONLESS	0x04				//IrLMP MUX Support ConnectionlessData
	
#define LMIAS_DVISA_EXTENSION		0x80
//IrDA:IrLMP:InstanceName Attribute Name
#define LMIAS_DV_IIIATTRNAME		"IrDA:IrLMP:InstanceName"	
//IrDA:IrLMP:LsapSel Attribute Name
#define LMIAS_DV_IILATTRNAME		"IrDA:IrLMP:LsapSel"

/* �����豸��Ϣ��Ԥ���� */
#define LM_DVSVRHINTLEN		4		//In specification discribe only 2 byte, we define 4 byte
#define LM_DVNICKLEN		23		//nick name max len
#define LM_DVPNPCOMP		0x01	//PnP compatible
#define LM_DVPDAPALMTOP		0x02	//PDA/Palmtop
#define LM_DVCOMPUTER		0x04	//Computer
#define LM_DVPRINTER		0x08	//Printer
#define LM_DVMODEM			0x10	//Modem
#define LM_DVFAX			0x20	//Fax
#define LM_DVLANACCESS		0x40	//Lan access
#define LM_DVTELEPHONE		0x01	//Telephone
#define LM_DVFILESERVER		0x02	//File Srever
#define LM_DVEXTENSIONMASK	0x80	//Extension mask

#define LM_DVMETHODSNIFF	1		//Sniffing
#define LM_DVMETHODACTIVE	2		//Active Discovery
#define LM_DVMETHODPASSIVE	3		//Passive Discovery
#define LM_DVMETHODCACHE	4		//Cache Discovery

typedef struct tagLM_DvInfo
{
	unsigned long DvAddress;				//�豸��ַ
	unsigned char DvHint[LM_DVSVRHINTLEN];	//�豸����
	unsigned char Charset;					//�ַ���
	unsigned char DvNickName[LM_DVNICKLEN];	//�豸����
	unsigned long method;					//��ø���Ϣ�ķ���
}LM_DvInfo;

/* ����LMMUX�㹫��API���� */
// �������������
#define IRD_SUCCESS				     0	//�ɹ�	
#define IRD_EINVALIDPARA			-1	//��������	
#define IRD_EEINVOKEING				-2	//�Ѿ����û��������	
#define IRD_EUSERCANCEL				-3	//�û�ȡ������
#define IRD_EWOULDBLOCK				-4	//����
#define IRD_ETIMEOUT				-5  //��ʱ����
#define IRD_ELOWERINVOKE			-6  //�Ͳ����ʧ��
#define IRD_NODEVICE				-7	//���豸
#define IRD_ENKNOWERROR				-100//δ֪����

#define LMDDEV_MAXDVINFO	8
typedef struct tagIrDDev_Info
{
	int DDevNum;
	LM_DvInfo DvTable[1];
}IrDDev_InfoList;

// ��������
int LMDDev_DiscoverDevicesReq( unsigned long Time, int nrSlots, unsigned char *buf, int buflen );
int LMDDev_CancelOption( void );
int LMDDev_CheckResult( int* Result );


/* Irdaȫ�ֺ������� */
int IrDA_SetIrdaName( char* Name, unsigned char CharSet );
int IrDA_SetIrdaServerHint( unsigned char* hint );

typedef enum
{
	UART1 = 1,
	UART2
}UART;

// ��ʼ������
int IrDA_Init();
//������·������
int IrDA_Link_Init( UART uartname );
//������·��ر�
int IrDA_Link_Shutdown( void );
//��õײ���·�����ݵ�ʱ����
int IrDA_GetIdleTimeSpace( void );

/* OBEX����㶨�� */
/* ��ɫ���� */
#define IROBEX_ROLE_SERVER		1
#define IROBEX_ROLE_CLIENT		2

#define IRO_SUCCESS		0
#define IRO_EBLOCK		-1		//��������
#define IRO_EPARA		-10		//��������
#define IRO_ESOURCE		-11		//��Դ����
#define IRO_EUSER		-12		//�û�ȡ��
#define IRO_ETIMEOUT	-14		//��ʱ
#define IRO_EBUFFER		-15		//�û��ṩ�Ļ���������
#define IRO_EREFUSE		-16		//�޷��������ܾ�
#define IRO_ELOWER		-17		//�²����
#define IRO_ESERVER		-18		//Զ�˷���������
#define IRO_FAILURE		-100	//δ֪

//���������¼�
#define IROBEXMT_PUT			0x02
#define IROBEXMT_GET			0x03
#define IROBEXMT_SETPATH		0x85
#define IROBEXMT_ABORT			0xff

//������Ӧ����
#define IROBEX_PT_SUCCESS		0xa0 //200 OK, Success
#define IROBEX_PT_CREATE		0xa1 //201 Created
#define IROBEX_PT_ACCEPTED		0xa2 //202 Accepted
#define IROBEX_PT_NONAUTH		0xa3 //203 Non-Authoritative Information
#define IROBEX_PT_NOCONTENT		0xa4 //204 No Content
#define IROBEX_PT_RESETCONTENT	0xa5 //205 Reset Content
#define IROBEX_PT_PARTCONTENT	0xa6 //206 Partial Content

#define IROBEX_PT_MULTICHOICE	0xb0 //300 Multiple Choices
#define IROBEX_PT_MOVEDPERM		0xb1 //301 Moved Permanently
#define IROBEX_PT_MOVEDTEMP		0xb2 //302 Moved temporarily
#define IROBEX_PT_SEEOTHER		0xb3 //303 See Other
#define IROBEX_PT_NOTMODIFY		0xb4 //304 Not modified
#define IROBEX_PT_USEPROXY		0xb5 //305 Use Proxy

#define IROBEX_PT_BADREQUEST	0xc0 //400 Bad Request - server couldn��t understand request
#define IROBEX_PT_UNAUTHORIZED	0xc1 //401 Unauthorized
#define IROBEX_PT_PAYMENTREQ	0xc2 //402 Payment required
#define IROBEX_PT_FORBIDDEN		0xc3 //403 Forbidden - operation is understood but refused
#define IROBEX_PT_NOTFOUND		0xc4 //404 Not Found
#define IROBEX_PT_METHODNOTALLOW	0xc5 //405 Method not allowed
#define IROBEX_PT_NOTACCEPT		0xc6 //406 Not Acceptable
#define IROBEX_PT_PROXYREQ		0xc7 //407 Proxy Authentication required
#define IROBEX_PT_TIMEOUT		0xc8 //408 Request Time Out
#define IROBEX_PT_CONFLICT		0xc9 //409 Conflict
#define IROBEX_PT_GONE			0xcA //410 Gone
#define IROBEX_PT_LENGTHREQ		0xcB //411 Length Required
#define IROBEX_PT_PRECONDITION	0xcC //412 Precondition failed
#define IROBEX_PT_ENTITYTOOLARGE	0xcD //413 Requested entity too large
#define IROBEX_PT_URLTOOLARGE	0xcE //414 Request URL too large
#define IROBEX_PT_UNSUPPORTEDMEDIA	0xcF //415 Unsupported media type

#define IROBEX_PT_INTERNALERROR	0xd0 //500 Internal Server Error
#define IROBEX_PT_NOTIMPLE		0xd1 //501 Not Implemented
#define IROBEX_PT_BADGATEWAY	0xd2 //502 Bad Gateway
#define IROBEX_PT_SERVICEUNAVAI	0xd3 //503 Service Unavailable
#define IROBEX_PT_GATEWAYTIMEOUT	0xd4 //504 Gateway Timeout
#define IROBEX_PT_HTTPVERNOTSUPPORT	0xd5 //505 HTTP version not supported

#define IROBEX_PT_DATABASEFULL	0xe0 //- - - Database Full
#define IROBEX_PT_DATABASELOCK	0xe1 //- - - Database Locked

//Header ID
#define IROBEX_HI_COUNT			0xc0
#define IROBEX_HI_NAME			0x01
#define IROBEX_HI_TYPE			0x42
#define IROBEX_HI_LEHGTH		0xc3
#define IROBEX_HI_TIMEZ			0x44
#define IROBEX_HI_TIMET			0xc4
#define IROBEX_HI_DISCRIPTION	0x05
#define IROBEX_HI_TARGET		0x46
#define IROBEX_HI_HTTP			0x47
#define IROBEX_HI_BODY			0x48
#define IROBEX_HI_ENDOFBODY		0x49
#define IROBEX_HI_WHO			0x4a
#define IROBEX_HI_CONNECTIONID	0xcb
#define IROBEX_HI_APPPARA		0x4c
#define IROBEX_HI_AUTHCHALL		0x4d
#define IROBEX_HI_AUTHRESP		0x4e
#define IROBEX_HI_OBJECTCLASS	0x4f


//���ݻ�ȡ������
#define IROBEX_AR_DATA	1	//��ȡ���ݣ�ֻ������״̬����Ч�����ڶ�ȡ��������
#define IROBEX_AR_HEAD	2	//��ȡͷ����ֻ�ڷ���״̬����Ч�����ڶ�ȡ����ͷ��
#define IROBEX_AR_BODY	3	//��ȡ�����壬ֻ�ڷ���״̬����Ч�����ڶ�ȡ���������塣

//���ؽ���ṹ
/*
���ṹ��������OBEX�����Ľ�����û��������κ�ʱ��ͨ���������û�øý����
���ṹ�ĳ�Ա�����治ͬ�����ͬ��
RsCode�������ʶ�룬��ʶ������
		IROBEXRS_INVALID			��Ч״̬����ʱ�������������塣
		IROBEXRS_IDLE				����״̬����ʱ�������������塣
		IROBEXRS_BLOCKING			���������У���ʱ�������������塣
		IROBEXRS_CONNECTED			���ӳɹ�״̬��
										param1	���ӱ�ʶ
										param2	Զ�����ݱ���󳤶�
										param3	Զ�˸������ݳ���
		IROBEXRS_ANSWERED			����������
										param1	���
										param2	���ͷ�����ݳ���
										param3	������������ݳ���
*/
#define IROBEXRS_INVALID		1
#define IROBEXRS_IDLE			2
#define IROBEXRS_BLOCKING		3
#define IROBEXRS_CONNECTED		4
#define IROBEXRS_ANSWERED		5

typedef struct tagIRO_Result
{
	int RsCode;			//�����ʶ��
	int Param1;			//����һ
	int Param2;			//������
	unsigned long Param3;	//������
}IRO_Result;

int IROBEX_Create( int Async, int Timeout );
int IROBEX_Close( int Obex );
int IROBEX_DisConnect( int Obex, IrDAData* pData, IrDAData* pRet );
int IROBEX_Connect( int Obex, unsigned long address, int lSel, int rSel, int* flags, 
				    IrDAData* pData, IrDAData* pRet );
int IROBEX_Method( int Obex, int Opcode, int flags, int Constants, IrDAData* pHead,
				  IrDAData* pData, IrDAData* pRHead, IrDAData* pRBody );
int IROBEX_GetResult( int Obex, IRO_Result* pResult );
int IROBEX_ReadData( int Obex, int Code, IrDAData* pData );

int IROBEX_DefaultPut( unsigned long addr, int Opcode, char* name, char* type, char* pbuf, int len );
int IROBEX_DefaultGet( unsigned long addr, char* type, char* name, char* pbuf, int* len );

#define IROBEX_DS_SNUM				5

#define IROBEX_DSO_CAPABILITY		1
#define IROBEX_DSO_IRMC				2
#define IROBEX_DSO_PUSH				3
#define IROBEX_DSO_PULL				4
#define IROBEX_DSO_SETPATH			5

int IROBEX_DS_Startup( char* classname, int ifAsync );
int IROBEX_DS_Cleanup( int sobex );
int IROBEX_DS_Setup( int sobex, int Opcode, char* pname, int nlen, char* ptype, int tlen, IrDAData* pdata );
int IROBEX_DS_Query( int sobex, int index, int* method, IrDAData* name, IrDAData* type, int* len );
int IROBEX_DS_QueryData( int sobex, int index, IrDAData* pdata );
/*
int IROBEX_APP_Register( unsigned char * pUUID, int len );
int IROBEX_APP_GetObj( int AppHD, int * ConnectID, int * OpCode,
					  IrDAData * pHead, IrDAData * pBody, int Timeout );
int IROBEX_APP_AnswerObj( int ConnectID, int ResponseCode, IrDAData * pHead, IrDAData * pBody );
*/
/**************************************************************
 *		 IrCOMM ��ʼ 
 **************************************************************/

/* ���庯������ֵ */
#define IR_SUCCESS			1
#define IR_FAILURE			-1

/* IrComm �˿������˿��� */
#define IRCOMM_PORT_NUM		4
#define IRCOMM_PORT_NAME1	"IrCOMM1"
#define IRCOMM_PORT_NAME2	"IrCOMM2"
#define IRCOMM_PORT_NAME3	"IrCOMM3"
#define IRCOMM_PORT_NAME4	"IrCOMM4"

/* SetIrCommOption��GetIrCommOption�е�ѡ�� */
typedef enum
{
	OPT_CALLBACK,		/* ���ûص����� */
	OPT_COMCONFIG,		/* ���ö˿ڵĲ��� */
	OPT_LOCAL,			/* �õ�������ѡ�� */
	OPT_REMOTE,			/* �õ�Զ��ѡ�� */
	OPT_SEND,			/* ��ѯ�Ƿ����ڿ��Է������� */
	OPT_MAXDATASZIE,	/* ÿ�������Է��͵��ֽ��� */
	OPT_DISCONRESON,	/* ����ԭ�� */
}ENTITY_OPTION;

/* ����֪ͨ�û����¼� */
typedef enum
{
	LINE_DATA_IN,		/* �����ݵ��� */
	LINE_DATA_OUT,		/* �ɼ����������� */
	LINE_CONNECT,		/* �����ӵ��� */
	LINE_DISCONNECT,	/* ���� */
}EVNET;

/**************************************
 *		IrCOMM_PARAMS ������ֵ(PV)
 **************************************/
/* �������ͷ��� */
#define IRCOMM_3_WIRE_RAW       0x01
#define IRCOMM_3_WIRE           0x02
#define IRCOMM_9_WIRE           0x04
#define IRCOMM_CENTRONICS       0x08

/* �˿����� */
#define IRCOMM_SERIAL           0x01
#define IRCOMM_PARALLEL         0x02

/* ���ݸ�ʽϸ�� */
#define IRCOMM_WSIZE_5          0x00
#define IRCOMM_WSIZE_6          0x01
#define IRCOMM_WSIZE_7          0x02
#define IRCOMM_WSIZE_8          0x03

#define IRCOMM_1_STOP_BIT       0x00
#define IRCOMM_2_STOP_BIT       0x04 /* 1.5 if char len 5 */

#define IRCOMM_PARITY_DISABLE   0x00
#define IRCOMM_PARITY_ENABLE    0x08

#define IRCOMM_PARITY_ODD       0x00
#define IRCOMM_PARITY_EVEN      0x10
#define IRCOMM_PARITY_MARK      0x20
#define IRCOMM_PARITY_SPACE     0x30

/* ���� */
#define IRCOMM_XON_XOFF_IN      0x01
#define IRCOMM_XON_XOFF_OUT     0x02
#define IRCOMM_RTS_CTS_IN       0x04
#define IRCOMM_RTS_CTS_OUT      0x08
#define IRCOMM_DSR_DTR_IN       0x10
#define IRCOMM_DSR_DTR_OUT      0x20
#define IRCOMM_ENQ_ACK_IN       0x40
#define IRCOMM_ENQ_ACK_OUT      0x80

/* ��·״̬ */
#define IRCOMM_OVERRUN_ERROR    0x02
#define IRCOMM_PARITY_ERROR     0x04
#define IRCOMM_FRAMING_ERROR    0x08

/* �����ַ�ȱʡֵ */
#define IRCOMM_XON_DEFAULT		0x11
#define IRCOMM_XOFF_DEFAULT		0x13
#define IRCOMM_ENQ_DEFAULT		0x05
#define IRCOMM_ACK_DEFAULT		0x06

/* DTE (Data Terminal Equipment) ��·���� */
#define IRCOMM_DELTA_DTR        0x01
#define IRCOMM_DELTA_RTS        0x02
#define IRCOMM_DTR              0x04
#define IRCOMM_RTS              0x08

/* DCE (Data Communications Equipment) ��·���� */
#define IRCOMM_DELTA_CTS        0x01  /* CTS �����仯 */
#define IRCOMM_DELTA_DSR        0x02  /* DSR �����仯 */
#define IRCOMM_DELTA_RI         0x04  /* RI �����仯 */
#define IRCOMM_DELTA_CD         0x08  /* CD �����仯 */
#define IRCOMM_CTS              0x10  /* ���� CTS */
#define IRCOMM_DSR              0x20  /* ���� DSR */
#define IRCOMM_RI               0x40  /* ���� RI */
#define IRCOMM_CD               0x80  /* ���� CD */
#define IRCOMM_DCE_DELTA_ANY    0x0f

/* IrCOMM���� */
typedef struct tagIrCOMM_PARAMS 
{
	/* ͨ�ò��� */
	unsigned char ServiceType;	/* �������ͣ�3-Wire raw��3-Wire��9-Wire��Centronics */
	unsigned char PortType;		/* �˿����ͣ����ڻ򲢿� */
	unsigned char DataFormat;	/* ���ݸ�ʽ */
	unsigned char FlowControl;	/* ���� */
	char PortName[32];			/* �̶��˿���-��󳤶�32 */
	unsigned long DataRate;		/* �������� bps */
	char XonXoff[2];			/* XON/XOFF�����ַ� */
	char EnqAck[2];				/* ENQ/ACK�����ַ� */
	unsigned char LineStatus;	/* ��·״̬ */
	unsigned char Break;
	unsigned char NullModem;	/* ģ�� Modem */
	/* 9-wire�������Ϳ��Ʋ��� */
	unsigned char DTE;			/* DET ��·���úͱ仯 */
	unsigned char DCE;			/* DCE��·���úͱ仯 */
	unsigned char Poll;			/* ��ѯ��·���� */
	unsigned short UnUsed;
}IrCOMM_PARAMS;

/**************************************************************
 *		IrCOMM �˿�ģ�����ṩ���ϲ�Ӧ�ó����API����
 **************************************************************/
int OpenIrComm(char *pName);
int CloseIrComm(int Handle);
int SetIrCommOption(int Handle, int Option, void *pValue, int Length);
int GetIrCommOption(int Handle, int Option, void *pValue, int Length);
int IrCommControl(int Handle, int Option, void *pValue, int Length);
int ReadIrComm(int Handle,char *pBuffer, int Length);
int WriteIrComm(int Handle,char *pBuffer, int Length);


/**************************************************************************\
		IrMC����
\**************************************************************************/

//����ֵ����
#define IRI_SUCCESS		0
#define IRI_EBLOCK		-1		//��������
#define IRI_EPARA		-10		//��������
#define IRI_ESOURCE		-11		//��Դ����
#define IRI_EUSER		-12		//�û�ȡ��
#define IRI_ETIMEOUT	-14		//��ʱ
#define IRI_EBUFFER		-15		//�û��ṩ�Ļ���������
#define IRI_EREFUSE		-16		//�޷��������ܾ�
#define IRI_ELOWER		-17		//�²����
#define IRI_ESERVER		-18		//Զ�˷���������
#define IRI_EDEV		-19		//δ�����豸�����豸��֧��IRMC����
#define IRI_EREGIST		-20		//δע��
#define IRI_EDISCONN	-21		//�Ͳ����
#define IRI_FAILURE		-100	//δ֪

//IRMC�豸֧�ֵ�ͬ����ɫ����
#define IRMC_SYNCROLE_CLIENT			1
#define IRMC_SYNCROLE_SERVER			2

//IRMC�����壬���붨�������
#define IRMC_OBJ_PB					1
#define IRMC_OBJ_MSGOUT				2
#define IRMC_OBJ_MSGIN				3
#define IRMC_OBJ_MSGSEND			4
#define IRMC_OBJ_NOTES				5
#define IRMC_OBJ_CAL				6

//GET,PUT�������Ͷ��� 
#define IRMC_GET_CHANGELOG			1
#define IRMC_GET_DEVINF				2
#define IRMC_GET_INFOLOG			3		
#define IRMC_GET_OBJSTORE			4	
#define IRMC_GET_ARECORD			5
#define IRMC_GET_CC					6

#define IRMC_PUT_ADDARECORD			7
#define IRMC_PUT_MODOBJSTORE		8
#define IRMC_PUT_MODARECORD			9
#define IRMC_PUT_DELARECORD			10
#define IRMC_PUT_INBOX				11
#define IRMC_PUT_SYNC				12

/* irmc page 21, must be no more than 50 Unicode characters(100octets) in length */

#define IRMC_MAXLUIDLEN		50

//��¼��������
#define IRMC_M_ACTION		1
#define IRMC_H_ACTION		2
#define IRMC_D_ACTION		3

//�����¼
typedef struct tagIRMC_RD
{
	int action;
	int ifHDel;			//ֻ����ɾ��ʱ�����ǲ���Ч	
	int ResCode;		//GET��PUT��������Ӧ����
	IrDAData data;		//��¼����	
	unsigned char Luid[IRMC_MAXLUIDLEN];	
	unsigned short UnUsed;	
}IRMC_RD;

//�����¼�б�
typedef struct tagIRMC_RdList  
{
	int flag;
	int totalNum;
	IRMC_RD rd[1];
}IRMC_RdList;

/* ��¼������Ͷ��� */
#define IRMC_RDTYPE_ALL			0			//����,���м�¼
#define IRMC_RDTYPE_CHANGED		1			//����,�Ķ���¼

/* ������ */
typedef struct tagIRMC_Result
{	
	int totalRd;
	int maxRd;
	unsigned long srvCC;		
	IrDAData SN;
	IrDAData DID;
	IrDAData infoLog;
	IrDAData devInfo;		
	int type;
	IrDAData ARdList;		//���м�¼�б�
	IRMC_RdList* pRdList;	//�ı��¼�б�
}IRMC_Result;//���ڴ������

typedef struct tagIRMC_CLogItem
{
	struct tagIRMC_CLogItem* pNext;
	int action;
	int cc;
	unsigned char Luid[IRMC_MAXLUIDLEN];
	unsigned short UnUsed;	
}IRMC_CLogItem;

typedef struct tagIRMC_ChangeLog
{
	IrDAData SN;
	IrDAData DID;
	int TotalRds;
	int MaxRds;
	int ifValid; //If the sync anchor submitted by the synv engine is
				 //smaller than the sync anchor of the oldest record in
				 //the stored change log.
	IRMC_CLogItem* pItems;
}IRMC_ChangeLog;

//��Ϣ����param1
#define IRMC_MSG_FAILURE			0	//����ʧ��
#define IRMC_MSG_SUCCESS			1	//�����ɹ�
#define IRMC_MSG_SYNCSRV			2	//������ͬ��

//���MsgΪIRMC_MSG_SUCCESS��param1���ã�param2Ϊ������ݡ�����������ݡ��ơ���param2���ã���
//���MsgΪIRMC_MSG_FAILURE��param1Ϊʧ��ԭ��param2���á�
//���MsgΪIRMC_MSG_SYNCSRV��param1���ã�param2ΪIRMC_Result*���͡�
//���MsgΪIRMC_MSG_SUCCESS��param1Ϊ�������˵ĵ�ǰ�ı��������CC��ֵ��param2ΪIRMC_RdList*���͡�
//�ص�������
typedef struct tagIRMCAPP_LowerDispatch
{	
	//�ͻ��˺���		
	int (* IrMC_Message)( int irmc, int Msg, int param1, void* param2 ); 		
	//����������
	int (* IrMC_GetInd)( int irmc, int GetType, void* pParam1 );	
	int (* IrMC_PutInd)( int irmc, int PutType, IrDAData* Param1, IrDAData* Param2, int param3 );	
}IRMCAPP_LowerDispatch; 

//�������ע��
#define IRMC_MAXNAMELEN				64
#define IRMC_MAXEXTENTLEN			24
typedef struct tagIRMC_Regist
{
	int object;			//�����ʶ	
	struct tagIRMCAPP_LowerDispatch* disapp; //�ص�����
	char objstr[IRMC_MAXNAMELEN];	 //������
	char extent[IRMC_MAXEXTENTLEN]; //������չ		
}IRMC_Regist;

//ͬ�������ͻ�����Ҫ֪���ķ�������Ϣ
typedef struct tagIRMC_SyncInfo 
{
	int object;			//�����ʶ
	unsigned long cc;	//ͬ��������
	IrDAData SN;	//�豸������
	IrDAData DID;	//���ݿ�ID
}IRMC_SyncInfo;
/* irmc�������*/

int IRMC_Sync( IRMC_SyncInfo* pInfo, unsigned Timeout, int* irmc );
int IRMC_SyncSrv( int irmc, IRMC_RdList *pRdList, int* ResultMsg );
int IRMC_Push( int object, int Timeout, int* irmc, IrDAData* pData, IrDAData* pName );
int IRMC_CancelOperate( int irmc );
int IRMC_GetRes( int irmc, int optCode, IrDAData* pData );
int IRMC_PutRes( int irmc, int optCode, unsigned long cc, IrDAData* pLuid );
int IRMC_ReadResultLen( int irmc, int* len );
int IRMC_ReadResult( int irmc, IrDAData* pdata, int *cc );
/**************************************************************************\
		IrMC�������
\**************************************************************************/

#if __cplusplus
}
#endif


#endif
