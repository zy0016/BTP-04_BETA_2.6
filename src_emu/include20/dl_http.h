/**************************************************************************\
 *
 *                      Hopen TCP/IP System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 *
 * Filename		dl_http.h
 *
 * Purpose      Define some global const and structure about HTTP1.0.
 *				Those items only open to Http Programmer.				
 *  
 * Author       chriszhao
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /tcpipsec include/attsock.h                           $
 * $Revision:: 3  $     $Date:: 01-11-05 16:20                      $
 *
 * $History:: attsock.h                                             $
 * 
 * *****************  Version 3  *****************
 * User: Zhaozheng    Date: 01-11-05   Time: 16:20
 * Updated in $/tcpipsec include
 * 
\**************************************************************************/

#ifndef _DLHTTP_H
#define	_DLHTTP_H

typedef struct tagHTTPReq
{
	HWND Userwnd;
	unsigned long Msg;
	char* URLName; 
	char* Refername;
	char* HttpMethod; 
	char* pPostData; 
	unsigned long DataLen; 
	char* Contenttype;
	char* pUserHead;
	int headlen;	
	char* pUser;
	char* pPass;
}HTTPReq;

typedef struct tagProxy
{
	char* pName;
	char* pUser;
	char* pPass;
	unsigned short port;
}Proxy;

/* HTTP 请求码 */
#define HM_GET		"GET "
#define HM_POST		"POST "
#define HM_HEAD		"HEAD "

/* Ioctl 请求码 */
#define HTTP_IOOPTSETPROXY		1
#define HTTP_IOOPTCLEANPROXY	2
#define HTTP_IOOPTSETDIRET		3
#define HTTP_IOOPTCACHE			4
#define HTTP_IOOPTCOOKEI		5

void* HTTP_Open( HTTPReq* pReq );
int HTTP_Close( void* pHttp );
int HTTP_GetTotalLength( void* pHttp );
int HTTP_GetData( void* pHttp, int offset, char* pBuf, int* datalen );
int HTTP_GetTotalData( void* pHttp, char* pBuf, int* datalen);
int HTTP_GetResponseCode( void* pHttp );
int HTTP_GetHeaderField( void* pHttp, char* pheadfile, int* length );
int HTTP_GetLocation( void* pHttp, char* pLocation, int* length );
int HTTP_Ioctl( int Opcode, void* pValue, int len );

/* 通知事件 */
#define HTTP_EVHOST				1	//域名查询成功
#define HTTP_EVCONNECT			2	//连接成功
#define HTTP_EVDATAIN			3	//数据到来
#define HTTP_EVDATAEND			4	//数据接收完成
/* 函数返回 */
#define HTTP_SUCCESS	0
#define HTTP_UNINIT		-200	//未初始化，通常由于用户没用调用OPEN就进行其他操作了
#define HTTP_PARA		-201	//参数错误
#define HTTP_NAME		-202	//名字错误，传入的URLNAME必须是规范的名字(XXX://xxx.xxx.xxx.xxx/...)
#define HTTP_NOHEAD		-203	//无头数据
#define HTTP_BUFFER		-204	//长度不够
#define HTTP_SOURCE		-205	

#define HTTP_LOWER			-211
#define HTTP_CONTINUE		-212
#define HTTP_MOVED			-213
#define HTTP_WWWAUTHNEEDED	-214
#define HTTP_PROXYAUTHNEEDED	-215
#define HTTP_CLIENTERROR	-216
#define HTTP_SERVERERROR	-217

#define HTTP_FAILURE		-299	//未知错误

#define HTTP_Continue				100
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
#define HTTP_Movedtemporarily		302
#define HTTP_SeeOther				303
#define HTTP_Notmodified			304
#define HTTP_UseProxy				305
#define HTTP_BadRequest				400
#define HTTP_Unauthorized			401
#define HTTP_Paymentrequired		402
#define HTTP_Forbidden				403
#define HTTP_NotFound				404
#define HTTP_Methodnotallowed		405
#define HTTP_NotAcceptable			406
#define HTTP_ProxyAuthenticationrequired	407
#define HTTP_RequestTimeout			408
#define HTTP_Conflict				409
#define HTTP_Gone					410
#define HTTP_LengthRequired			411
#define HTTP_Preconditionfailed		412
#define HTTP_Requestedentitytoolarge	413
#define HTTP_RequestURItoolarge		414
#define HTTP_Unsupportedmediatype	415
#define HTTP_InternalServerError	500
#define HTTP_NotImplemented			501
#define HTTP_BadGateway				502
#define HTTP_ServiceUnavailable		503
#define HTTP_GatewayTimeout			504
#define HTTP_HTTPversionnotsupported	505

#endif
