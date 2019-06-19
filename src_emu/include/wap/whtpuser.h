/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : http.h
 *
 * Purpose  : HTTP 常量、数据结构定义及函数声明
 *            
\**************************************************************************/

#ifndef HTTP_USER_H_
#define HTTP_USER_H_

//#include "Windef.h"

/**************************************************************
 *		 HTTP 的函数返回值, 向应用程序发送的消息及错误码
 **************************************************************/

/* 以下两个即是函数返回值也做为消息的错误码 */
#define HTTP_SUCCESS		0	/* 成功		*/
#define HTTP_FAILURE		-1	/* 失败		*/
#define HTTP_BLOCKED		-2	/* 阻塞		*/

/* 定义错误码 */
#define HTTP_NOT_INIT		-6	/* 没有调用初始化函数	*/
#define HTTP_NOT_PROXY		-7	/* 没有设置代理			*/
#define HTTP_NOT_HANDLE		-9	/* 不是有效的 HTTP 名柄	*/
#define HTTP_VALUE_ERR		-10	/* 参数错误				*/
#define HTTP_BUFFER_ERR		-11	/* 缓冲区不够			*/
#define HTTP_INPROGRESS		-12	/* 操作正在进行中		*/
#define HTTP_NOT_CONNECT	-13	/* 没有连接到代理		*/
#define HTTP_SYS_ERROR		-14	/* 调用系统函数出错		*/
#define HTTP_NOT_SUPPORT	-15	/* 不支持此选项			*/
#define HTTP_RECV_CONTINUE	-16	/* 继续接收				*/		
#define HTTP_RESP_ERROR		-17	/* 服务器反应错误		*/
#define HTTP_HOST_ERROR		-18	/* 得到主机错误			*/
#define HTTP_PROT_ERROR		-19	/* 协议错误				*/
#define HTTP_FD_CLOSE		-20

/* 消息定义: LOWORD(lParam) */
#define HTTP_HOST			1
#define HTTP_CONNECT		2	/* 与中继代理建立了连接 */
#define HTTP_DATASTART		3	/* 接收到了反应头		*/
#define HTTP_DATAIN			4	/* 有数据到达			*/
#define HTTP_DATAEND		5

#define HTTP_SEND			7	/* HTTP 消息发送是否成功*/
#define HTTP_ACCEPT			8	/* 有 HTTP 连接到达		*/
#define HTTP_DISCONNECT		9	/* 对方断开连接			*/
#define HTTP_SESSION		10	/* 开始会话				*/

/* 与HTTP1.0兼容 */
#define HTTP_EVHOST       HTTP_HOST
#define HTTP_EVCONNECT    HTTP_CONNECT
#define HTTP_EVDATAEND    HTTP_DATAEND  
#define HTTP_EVDATAIN     HTTP_DATAIN

/* HTTP的方法 */
#define METHOD_GET			0
#define METHOD_OPTION		1
#define METHOD_HEAD			2
#define METHOD_DELETE		3
#define METHOD_TRACE		4
#define METHOD_POST			5
#define METHOD_PUT			6
#define METHOD_CONNECT		7

/**************************************************************
 *					定义服务器反应的状态码
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
 *				 服务器反应的状态码定义结束
 **************************************************************/

/**************************************************************
 *					 设置和获取 HTTP 选项的常量定义
 **************************************************************/
typedef enum enumCacheMode
{
    CACHE_OK	        = 0x0,	        /* 使用任何方便的缓存	*/
	CACHE_FLUSH_MEM	    = 0x1,			/* 从网络或缓存重载		*/
	CACHE_VALIDATE	    = 0x2,	        /* 验证缓存				*/
	CACHE_END_VALIDATE  = 0x4,          /* 端到端的验证			*/
	CACHE_RANGE_VALIDATE= 0x8,
	CACHE_FLUSH			= 0x10,         /* 强制重载				*/
} CACHEMODE;

#define HTTP_CACHE_SIZE		1	/* 缓存的大小	*/
#define HTTP_CACHE_MODE		2	/* 缓存模式		*/

#define	HTTP_COOKIE_MODE	3	/* 是否允许Cookie*/
#define HTTP_COOKIE_SIZE	4	/* Cookie 的大小*/
#define HTTP_COOKIE_COUNT	5	/* Cookie 的数量*/

#define HTTP_SET_HOST		6	/* 设置不使用代理的主机 */
#define HTTP_CLEAN_HOST		7	/* 清除不使用代理的主机 */

/* 为兼容HTTP1.0增加的结构及函数, 见赵征的dl_http.c */
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
	unsigned short	Port;			/* 服务器时使用 */
}HTTPReq;

typedef struct tagProxy
{
	char* pName;
	char* pUser;
	char* pPass;
	unsigned short port;
}Proxy;

/* HTTP_Open 函数 */
#define HTTP_CLIENT			1	/* 作为客户端	*/
#define HTTP_SERVER			2	/* 做为服务器端	*/

/* GetStreamData 函数Part 的取值 */
#define HTTP_PART_HEAD		0
#define HTTP_PART_BODY		1

/* Ioctl 请求码 */
#define HTTP_IOOPTSETPROXY		1
#define HTTP_IOOPTCLEANPROXY	2
#define HTTP_IOOPTSETDIRET		3
#define HTTP_IOOPTCACHE			4
#define HTTP_IOOPTCOOKEI		5

int HTTP_GetResponseCode( void* pIndex );
int HTTP_Ioctl( int Opcode, void* pValue, int len );
int HTTP_GetHeaderField( void* pHttpIndex, char *pHead, int *HeadLen);
int HTTP_GetTotalLength(void *pHttpIndex);

/* 以上函数兼容HTTP1.0 */

/**************************************************************
 *				HTTP 提供的公开函数声明
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

#define CACOO_SUCCESS				0	/*	全部转换成功 */	
#define CACOO_SUCCESS_CONTINUE		-1	/* 转换成功，需继续。*/
#define CACOO_NOANY_CACHE			-2  /* 没有存储的cache*/
#define CACOO_NOANY_COOKIE			-3
#define CACOO_ERR_PARAM				-4	/* 输入参数有误 */
#define CACOO_ERR_BUFSIZE			-5	/* 数据流缓冲区不足 */
#define CACOO_ERR_MEM				-6 /* 申请内容失败 */
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
