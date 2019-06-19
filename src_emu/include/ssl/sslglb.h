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

#ifndef _SSLGLB_H
#define _SSLGLB_H


/* ssl error code */
/* 由于我们SSL当发生网络错误的时候我们直接将错误码提交给用户，所以这里我们
   保证兼容全部POLLEX网络错误码 */
#ifndef	SSLEWOULDBLOCK
#define	SSLEWOULDBLOCK			-100		/*阻塞错误			*/
#define	SSLENOTCONN				-101		/*未连接错误		*/
#define SSLESOCKTNOSUPPORT		-102		/*本地址族中不支持该类型套接口*/
#define	SSLEAFNOSUPPORT			-103		/*家族不支持		*/
#define	SSLEISCONN				-104		/*已连接错误		*/
#define	SSLEOPNOTSUPP			-105		/*选项不支持		*/
#define	SSLEALARM				-106		/*警报				*/
#define	SSLEABORT				-107		/*放弃				*/
#undef	SSLEINTR
#define	SSLEINTR				-108
#define	SSLECONNREFUSED			-109		/*连接被拒绝		*/
#define SSLEMSGSIZE				-110		/*消息长度错误		*/
#define	SSLEADDRINUSE			-111		/*地址被占用		*/
#define	SSLEMIN					-112		/*最小值溢出		*/
#define	SSLEMAX					-113		/*最大值溢出		*/
#define	SSLESSLTABLEFULL		-114		/*SSL表满			*/
#define	SSLENOMEMORY			-115		/*内存不够			*/
#define	SSLENOTSSL				-116		/*SSL句柄无效		*/
#define	SSLEINVALIDPARA			-117		/*参数无效			*/
#define	SSLEADDRFORMAT			-118		/*地址格式不对		*/
#define	SSLEPROTNOSUPPORT		-119		/*协议不支持		*/
#define SSLELOWSYSRESOURCE		-120		/*系统资源不够		*/
#define SSLETIMEOUT				-121		/*超时错误			*/
#define SSLERESET				-122		/*连接被复位		*/
#define SSLEUNKNOWERROR			-123		/*未知错误			*/
#define SSLENETWORK				-124		/*低层网络错误		*/
#define SSLEBADMESSAGE			-125		/*错误消息			*/
#define SSLESHUTDOWN			-126		/*连接端口已经关闭  */
#define SSLEUNSUPVERSION		-127		/*版本不支持		*/
#define SSLEUNSUPCIPHER			-128		/*加密方法不支持	*/
#define SSLECIPHER				-129		/*加密出现错误		*/

#define SSL_RUNNINGERROR			-149		//运行错误
#define SSLEADUNEXPECTEDMESSAGE		-150		//unexpected_message
#define SSLEADBADRECORDMAC			-151		//bad_record_mac
#define SSLEADDECOMPRESSIONFAILURE	-152		//Decompression_failure
#define SSLEADHANDSHAKEFAILURE		-153		//handshake_failure
#define SSLEADNOCERTIFICATE			-154		//no_certificate
#define SSLEADBADCERTIFICATE		-155		//bad_certificate
#define SSLEADUNSUPPORTEDCERTIFICATE	-156	//unsupported_certificate
#define SSLEADCERTIFICATEREVOKED	-157		//certificate_revoked
#define SSLEADCERTIFICATEEXPIRED	-158		//certificate_expired
#define SSLEADCERTIFICATEUNKNOWN	-159		//certificate_unknown
#define SSLEADILLEGALPARAMETER		-160		//illegal_parameter
#define SSLEMAXERROR			-200			/*未知错误			*/
#endif

/* SSL EVENT CODE */
#define SSL_EVCONNECT			1			//连接成功事件，SSL握手成功或者失败以后上发。
#define SSL_EVREAD				2			//数据到来事件，SSL端口有数据到来时发送。
#define SSL_EVWRITE				3			//写缓冲区空闲事件，SSL端口写缓冲区空闲以后发送。
#define SSL_EVCLOSE				4			//关闭事件：SSL端口关闭以后发送。
#define SSL_EVALERT				5			//警告消息到来事件。
#define SSL_EVSESSION			6			//开始会话事件

/* ssl set option code */
#define SSL_OPCYP				1			//设置加密算法
#define SSL_OPCOM				3			//设置压缩算法

/* 选项 */
#define SSL_SESSION				5			/* 开始SSL会话	*/

/* SSL_SESSION对应的值 */
#define SSL_NO_SESSION			1			/* 连接时不进行握手协商 */
#define SSL_BEGIN_SESSION		2			/* 开始握手协商			*/

/* ssl set option support cypto method */
#define SSL_NULL_WITH_NULL_NULL					0x00l
#define SSL_RSA_WITH_NULL_MD5					0x01l
#define SSL_RSA_WITH_NULL_SHA					0x02l
#define SSL_RSA_EXPORT_WITH_RC4_40_MD5			0x03l
#define SSL_RSA_WITH_RC4_128_MD5				0x04l
#define SSL_RSA_WITH_RC4_128_SHA				0x05l
#define SSL_RSA_EXPORT_WITH_RC2_CBC_40_MD5		0x06l
#define SSL_RSA_WITH_IDEA_CBC_SHA				0x07l
#define SSL_RSA_EXPORT_WITH_DES40_CBC_SHA		0x08l
#define SSL_RSA_WITH_DES_CBC_SHA				0x09l
#define SSL_RSA_WITH_3DES_EDE_CBC_SHA			0x0Al

/* ssl port num */
#define SSL_PORT_HTTP	443		//Reserved for use by Hypertext Transfer Protocol with 
								//SSL (https).
#define SSL_PORT_SMTP	465		//Reserved (pending) for use by Simple Mail Transfer Protocol
								//with SSL (ssmtp).
#define SSL_PORT_NNTP	563		//Reserved (pending) for use by Network News Transfer
								//Protocol (snntp).	

typedef	int	SSLHD;

/* global API */
SSLHD SSL_Create( HWND hWnd, unsigned long Msg );
int SSL_connect( SSLHD ssl, const struct sockaddr* paddr, int addrlen );
int SSL_send( SSLHD ssl, char* pbuf, int buflen);
int SSL_recv( SSLHD ssl, char* pbuf, int buflen);
int SSL_shutdown( SSLHD ssl );
int SSL_close( SSLHD ssl );
int SSL_SetOpt( SSLHD ssl, int index, unsigned char number );

int SSL_SetOption(SSLHD ssl, int Option, void *pValue, int Length);

#define SSLGetLastError				OS_GetError

#endif
