/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :		mmsuser.h
*
* Purpose :		MMS 常量、数据结构定义及函数声明
*  
* Author  :		hyzhang
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
\**************************************************************************/

#ifndef MMS_USER_H_
#define MMS_USER_H_

/**************************************************************
 *		 MMS 的函数返回值, 向应用程序发送的消息及错误码
 **************************************************************/

/* 以下两个即是函数返回值也做为消息的错误码 */
#define MMS_SUCCESS		0	/* 成功		*/
#define MMS_FAILURE		-1	/* 失败		*/
#define MMS_BLOCKED		-2	/* 阻塞		*/

/* 定义错误码 */
#define MMS_NOT_INIT	-6	/* 没有调用初始化函数	*/
#define MMS_NOT_PROXY	-7	/* 没有设置代理			*/
#define MMS_NOT_CONF	-8	/* 发送的消息没有得到对方的确认	*/
#define MMS_NOT_HANDLE	-9	/* 不是有效的 MMS 名柄	*/
#define MMS_INPROGRESS	-10	/* 操作正在进行中		*/
#define MMS_VALUE_ERR	-11	/* 参数错误				*/
#define MMS_BUFFER_ERR	-12	/* 缓冲区不够			*/
#define MMS_NOT_CONNECT	-13	/* 没有连接到代理		*/
#define MMS_SYS_ERROR	-14	/* 调用系统函数出错		*/
#define MMS_NOT_SUPPORT	-15	/* 不支持此选项			*/

/* HIWORD(lParam), 底层错误原因, 为避免与WAP冲突, 对应值已改名 */
#define MMS_Continue				0x10
#define MMS_ESwitching				0x11
#define MMS_EOK						0x20
#define MMS_ECreated				0x21
#define MMS_EAccepted				0x22
#define MMS_ENonAuthoritative		0x23
#define MMS_ENoContent				0x24
#define MMS_EReset					0x25
#define MMS_EPartialContent			0x26
#define MMS_EMultipleChoices		0x30
#define MMS_EMovedPermanently		0x31
#define MMS_EMovedtemporarily		0x32
#define MMS_ESeeOther				0x33
#define MMS_ENotmodified			0x34
#define MMS_EUseProxy				0x35
#define MMS_EBadRequest				0x40
#define MMS_EUnauthorized			0x41
#define MMS_EPaymentrequired		0x42
#define MMS_EForbidden				0x43
#define MMS_ENotFound				0x44
#define MMS_EMethodnotallowed		0x45
#define MMS_ENotAcceptable			0x46
#define MMS_EProxyAuthenticationrequired	0x47
#define MMS_ERequestTimeout			0x48
#define MMS_EConflict				0x49
#define MMS_EGone					0x4A
#define MMS_ELengthRequired			0x4B
#define MMS_EPreconditionfailed		0x4C
#define MMS_ERequestedentitytoolarge	0x4D
#define MMS_ERequestURItoolarge		0x4E
#define MMS_EUnsupportedmediatype	0x4F
#define MMS_EInternalServerError	0x60
#define MMS_ENotImplemented			0x61
#define MMS_EBadGateway				0x62
#define MMS_EServiceUnavailable		0x63
#define MMS_EGatewayTimeout			0x64
#define MMS_EHTTPversionnotsupported	0x65

#define MMS_EPROTOERR				0xe0	//远方协议错误	
#define MMS_EDISCONNECT				0xe1	//断联错误
#define MMS_ESUSPEND				0xe2	//挂起错误
#define MMS_ERESUME					0xe3	//恢复错误
#define MMS_ECONGESTION				0xe4	//远方资源不足
#define MMS_ECONNECTERR				0xe5	//会话创建错误
#define MMS_EMRUEXCEEDED			0xe6	//数据包大小错误
#define MMS_EMOREXCEEDED			0xe7	//协商方法或推数量错误
#define MMS_EPEERREQ				0xe8	//远方错误
#define MMS_ENETERR					0xe9	//网络错误
#define MMS_EUSERREQ				0xea	//用户错误
#define MMS_EREDIRECT				0xeb	//重定向
#define MMS_EREPLY					0xec

/* 消息定义: LOWORD(lParam) */
#define MMS_CONNECT		1	/* 与中继代理建立了连接 */
#define MMS_SENDED		2	/* MMS 消息发送是否成功	*/
#define MMS_ACCEPT		3	/* 有 MMS 消息到达		*/
#define MMS_READ		4	/* 有数据到达			*/

#define MMS_SUSPEND		8	/* 挂起消息				*/
#define MMS_RESUME		9	/* 恢复消息				*/
#define MMS_REDIRECT	10	/* 重定向消息			*/
#define MMS_DISCONNECT	11	/* 对方断开连接			*/

/**************************************************************
 *					 设置和获取 MMS 选项的常量定义
 **************************************************************/
/* 全局选项 */
#define MMS_FIONBIO		1	/* 设置MMS的连接方式 */

/* 选项Option */
#define MMS_HEAD		2	/* 设置MMS的消息头	*/
#define MMS_BODY		3	/* 设置MMS的消息体	*/

#define MMS_PROXYURI	4	/* 设置代理的uri	*/
#define MMS_REPORT		5	/* 是否接收投递报告	*/
#define MMS_TRANSID		6	/* 得到Transaction-ID*/
#define MMS_RESSTATUS	7	/* 得到反应状态		*/
#define MMS_MSGID		8	/* 得到Message-ID*/
#define MMS_ALWREPORT	9	/* 是否允许投递报告	*/

/**************************************************************
 *					 MMS 的打开方式
 **************************************************************/
#define MMS_SEND		1
#define MMS_RECV		2

 /**************************************************************
 *					 数据结构定义
 **************************************************************/
typedef struct tagMMSBody
{
	char* pHead;
	int headlen;
	char* pBody;
	int bodylen;
}MMSBody;

typedef struct tagMMSMultiBody
{
	int Bodynum;
	MMSBody body[1];
}MMSMultiBody;

/**************************************************************
 *				MMS 提供的公开函数声明
 **************************************************************/
int MMS_Startup(HWND hWnd, int Message);
int MMS_Connect(char *GateWay, unsigned short Port);
int MMS_Open(int Protocol, int Mode);
int MMS_Accept(int Handle, char *pBuf, int *BufLen, int *bDelivered);
int MMS_Send(int Handle, char *pHead, int HeadLen, char *pBody, int BodyLen);
int MMS_Recv(int Handle, char *pBuf, int BufLen, int bDelay);
int MMS_RecvDelay(int Handle, char *pBuf, int BufLen);
int MMS_GetData(int Handle, char *pHead, int *HeadLen, char *pBody, int *BodyLen);
int MMS_SetOption(int Handle, int Option, void *pValue, int Length);
int MMS_GetOption(int Handle, int Option, void *pValue, int Length);
int MMS_Disconnect(void);
int MMS_Close(int Handle);
int MMS_Destroy(void);
int MMS_GetLastError(void);

int MMS_PackHeadersInBody(char *pData, int DataLen, char *pBuf, int BufLen);
int MMS_PackMulitBody(MMSMultiBody *pBody, char *pData, int DataLen);
int MMS_LibParseMulitBody( char* pData, int datalen, MMSMultiBody* pBody );
int MMS_LibParseBodyHead(char *pData, int DataLen, char *pBuf, int BufLen);

#endif	/* MMS_USER_H_ */
