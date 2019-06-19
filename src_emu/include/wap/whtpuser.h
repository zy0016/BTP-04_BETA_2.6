/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : http.h
 *
 * Purpose  : HTTP ���������ݽṹ���弰��������
 *            
\**************************************************************************/

#ifndef HTTP_USER_H_
#define HTTP_USER_H_

//#include "Windef.h"

/**************************************************************
 *		 HTTP �ĺ�������ֵ, ��Ӧ�ó����͵���Ϣ��������
 **************************************************************/

/* �����������Ǻ�������ֵҲ��Ϊ��Ϣ�Ĵ����� */
#define HTTP_SUCCESS		0	/* �ɹ�		*/
#define HTTP_FAILURE		-1	/* ʧ��		*/
#define HTTP_BLOCKED		-2	/* ����		*/

/* ��������� */
#define HTTP_NOT_INIT		-6	/* û�е��ó�ʼ������	*/
#define HTTP_NOT_PROXY		-7	/* û�����ô���			*/
#define HTTP_NOT_HANDLE		-9	/* ������Ч�� HTTP ����	*/
#define HTTP_VALUE_ERR		-10	/* ��������				*/
#define HTTP_BUFFER_ERR		-11	/* ����������			*/
#define HTTP_INPROGRESS		-12	/* �������ڽ�����		*/
#define HTTP_NOT_CONNECT	-13	/* û�����ӵ�����		*/
#define HTTP_SYS_ERROR		-14	/* ����ϵͳ��������		*/
#define HTTP_NOT_SUPPORT	-15	/* ��֧�ִ�ѡ��			*/
#define HTTP_RECV_CONTINUE	-16	/* ��������				*/		
#define HTTP_RESP_ERROR		-17	/* ��������Ӧ����		*/
#define HTTP_HOST_ERROR		-18	/* �õ���������			*/
#define HTTP_PROT_ERROR		-19	/* Э�����				*/
#define HTTP_FD_CLOSE		-20

/* ��Ϣ����: LOWORD(lParam) */
#define HTTP_HOST			1
#define HTTP_CONNECT		2	/* ���м̴����������� */
#define HTTP_DATASTART		3	/* ���յ��˷�Ӧͷ		*/
#define HTTP_DATAIN			4	/* �����ݵ���			*/
#define HTTP_DATAEND		5

#define HTTP_SEND			7	/* HTTP ��Ϣ�����Ƿ�ɹ�*/
#define HTTP_ACCEPT			8	/* �� HTTP ���ӵ���		*/
#define HTTP_DISCONNECT		9	/* �Է��Ͽ�����			*/
#define HTTP_SESSION		10	/* ��ʼ�Ự				*/

/* ��HTTP1.0���� */
#define HTTP_EVHOST       HTTP_HOST
#define HTTP_EVCONNECT    HTTP_CONNECT
#define HTTP_EVDATAEND    HTTP_DATAEND  
#define HTTP_EVDATAIN     HTTP_DATAIN

/* HTTP�ķ��� */
#define METHOD_GET			0
#define METHOD_OPTION		1
#define METHOD_HEAD			2
#define METHOD_DELETE		3
#define METHOD_TRACE		4
#define METHOD_POST			5
#define METHOD_PUT			6
#define METHOD_CONNECT		7

/**************************************************************
 *					�����������Ӧ��״̬��
 **************************************************************/
#define HTTP_CONTINUE				100
#define HTTP_Switching				101

#define HTTP_OK						200
#define HTTP_Created				201
#define HTTP_Accepted				202
#define HTTP_NonAuthoritative		203
#define HTTP_NoContent				204
#define HTTP_Reset					205
#define HTTP_PartialContent			206

#define HTTP_MultipleChoices		300
#define HTTP_MovedPermanently		301
#define HTTP_MovedTemporarily		302
#define HTTP_SeeOther				303
#define HTTP_NotModified			304
#define HTTP_UseProxy				305
#define HTTP_TempRedirect			307

#define HTTP_BadRequest				400
#define HTTP_Unauthorized			401
#define HTTP_PaymentRequired		402
#define HTTP_Forbidden				403
#define HTTP_NotFound				404
#define HTTP_MethodNotAllowed		405
#define HTTP_NotAcceptable			406
#define HTTP_ProxyAuthRequired		407
#define HTTP_RequestTimeout			408
#define HTTP_Conflict				409
#define HTTP_Gone					410
#define HTTP_LengthRequired			411
#define HTTP_Preconditionfailed		412
#define HTTP_EntityTooLarge			413
#define HTTP_UrlTooLarge			414
#define HTTP_UnsupportedType		415
#define HTTP_RangeNoSatisfy			416
#define HTTP_ExpectFailed			417

#define HTTP_InternalServerError	500
#define HTTP_NotImplemented			501
#define HTTP_BadGateway				502
#define HTTP_ServiceUnavailable		503
#define HTTP_GatewayTimeout			504
#define HTTP_VersionNotSupported	505

/**************************************************************
 *				 ��������Ӧ��״̬�붨�����
 **************************************************************/

/**************************************************************
 *					 ���úͻ�ȡ HTTP ѡ��ĳ�������
 **************************************************************/
typedef enum enumCacheMode
{
    CACHE_OK	        = 0x0,	        /* ʹ���κη���Ļ���	*/
	CACHE_FLUSH_MEM	    = 0x1,			/* ������򻺴�����		*/
	CACHE_VALIDATE	    = 0x2,	        /* ��֤����				*/
	CACHE_END_VALIDATE  = 0x4,          /* �˵��˵���֤			*/
	CACHE_RANGE_VALIDATE= 0x8,
	CACHE_FLUSH			= 0x10,         /* ǿ������				*/
} CACHEMODE;

#define HTTP_CACHE_SIZE		1	/* ����Ĵ�С	*/
#define HTTP_CACHE_MODE		2	/* ����ģʽ		*/

#define	HTTP_COOKIE_MODE	3	/* �Ƿ�����Cookie*/
#define HTTP_COOKIE_SIZE	4	/* Cookie �Ĵ�С*/
#define HTTP_COOKIE_COUNT	5	/* Cookie ������*/

#define HTTP_SET_HOST		6	/* ���ò�ʹ�ô�������� */
#define HTTP_CLEAN_HOST		7	/* �����ʹ�ô�������� */

/* Ϊ����HTTP1.0���ӵĽṹ������, ��������dl_http.c */
typedef struct tagHTTPReq
{
	HWND			Userwnd;
	unsigned long	Msg;
	char*			URLName; 
	char*			Refername;
	int				Method; 
	char*			pPostData; 
	unsigned long	DataLen; 
	char*			Contenttype;
	char*			pUserHead;
	int				headlen;	
	char*			pUser;
	char*			pPass;
	unsigned short	Port;			/* ������ʱʹ�� */
}HTTPReq;

typedef struct tagProxy
{
	char* pName;
	char* pUser;
	char* pPass;
	unsigned short port;
}Proxy;

/* HTTP_Open ���� */
#define HTTP_CLIENT			1	/* ��Ϊ�ͻ���	*/
#define HTTP_SERVER			2	/* ��Ϊ��������	*/

/* GetStreamData ����Part ��ȡֵ */
#define HTTP_PART_HEAD		0
#define HTTP_PART_BODY		1

/* Ioctl ������ */
#define HTTP_IOOPTSETPROXY		1
#define HTTP_IOOPTCLEANPROXY	2
#define HTTP_IOOPTSETDIRET		3
#define HTTP_IOOPTCACHE			4
#define HTTP_IOOPTCOOKEI		5

int HTTP_GetResponseCode( void* pIndex );
int HTTP_Ioctl( int Opcode, void* pValue, int len );
int HTTP_GetHeaderField( void* pHttpIndex, char *pHead, int *HeadLen);
int HTTP_GetTotalLength(void *pHttpIndex);

/* ���Ϻ�������HTTP1.0 */

/**************************************************************
 *				HTTP �ṩ�Ĺ�����������
 **************************************************************/
int HTTP_Startup(void);
int HTTP_SetProxy(char *pProxy, int nPort, char *pUser, char *pPassWord);
int HTTP_SetDirectHost(char *pHost, int bSet);
int HTTP_GetData(int Handle, char *pHead, int *HeadLen, 
				 char *pBody, int *BodyLen);
int HTTP_GetStreamData(int Handle, int Part, char *pBuf, int Offset, int BufLen);

int HTTP_SetOption(int Handle, int Option, void *pValue, int Length);
int HTTP_GetOption(int Handle, int Option, void *pValue, int Length);
int HTTP_GetLocation(int Handle, char *pLocation, int *Length);
int HTTP_Close(int Handle);
int HTTP_Cleanup(void);
int HTTP_GetCersHD(int Handle);

int HTTP_Open(HTTPReq* pReq, int Mode);
int HTTP_Accept(int Handle, char *pAddr, int *AddrLen, int *Port);

/***********************************************************************
Cache & cookie API        
/************************************************************************/
#define CACOO_TYPE_CACHE	1
#define CACOO_TYPE_COOKIE	2

#define CACOO_SUCCESS				0	/*	ȫ��ת���ɹ� */	
#define CACOO_SUCCESS_CONTINUE		-1	/* ת���ɹ����������*/
#define CACOO_NOANY_CACHE			-2  /* û�д洢��cache*/
#define CACOO_NOANY_COOKIE			-3
#define CACOO_ERR_PARAM				-4	/* ����������� */
#define CACOO_ERR_BUFSIZE			-5	/* ���������������� */
#define CACOO_ERR_MEM				-6 /* ��������ʧ�� */
#define CACOO_ERR_VERSION			-7
#define CACOO_ERR_DATA				-8
#define CACOO_CACHE_FORBIDDEN		-9
#define CACOO_COOKIE_FORBIDDEN		-10

//int CACOO1_MemtoStream(int type, unsigned char* pUrl, int UrlLen,
//					   unsigned char *pStream, int StreamLen, int* pStreamLen);
//int CACOO1_StreamtoMem(int type, unsigned char *pUrl, unsigned char *pStream, int len);

int CACOO_MemtoStream(int type, unsigned char *pStream, int StreamLen, int *pStreamLen);
int CACOO_StreamtoMem(int type, unsigned char *pStream, int len);
int CACOO_Delete(char *pUrl);
void CACOO_Undo(int type);
/************************************************************************/
/*					End add                                             */
/************************************************************************/

#endif
