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
/* ��������SSL��������������ʱ������ֱ�ӽ��������ύ���û���������������
   ��֤����ȫ��POLLEX��������� */
#ifndef	SSLEWOULDBLOCK
#define	SSLEWOULDBLOCK			-100		/*��������			*/
#define	SSLENOTCONN				-101		/*δ���Ӵ���		*/
#define SSLESOCKTNOSUPPORT		-102		/*����ַ���в�֧�ָ������׽ӿ�*/
#define	SSLEAFNOSUPPORT			-103		/*���岻֧��		*/
#define	SSLEISCONN				-104		/*�����Ӵ���		*/
#define	SSLEOPNOTSUPP			-105		/*ѡ�֧��		*/
#define	SSLEALARM				-106		/*����				*/
#define	SSLEABORT				-107		/*����				*/
#undef	SSLEINTR
#define	SSLEINTR				-108
#define	SSLECONNREFUSED			-109		/*���ӱ��ܾ�		*/
#define SSLEMSGSIZE				-110		/*��Ϣ���ȴ���		*/
#define	SSLEADDRINUSE			-111		/*��ַ��ռ��		*/
#define	SSLEMIN					-112		/*��Сֵ���		*/
#define	SSLEMAX					-113		/*���ֵ���		*/
#define	SSLESSLTABLEFULL		-114		/*SSL����			*/
#define	SSLENOMEMORY			-115		/*�ڴ治��			*/
#define	SSLENOTSSL				-116		/*SSL�����Ч		*/
#define	SSLEINVALIDPARA			-117		/*������Ч			*/
#define	SSLEADDRFORMAT			-118		/*��ַ��ʽ����		*/
#define	SSLEPROTNOSUPPORT		-119		/*Э�鲻֧��		*/
#define SSLELOWSYSRESOURCE		-120		/*ϵͳ��Դ����		*/
#define SSLETIMEOUT				-121		/*��ʱ����			*/
#define SSLERESET				-122		/*���ӱ���λ		*/
#define SSLEUNKNOWERROR			-123		/*δ֪����			*/
#define SSLENETWORK				-124		/*�Ͳ��������		*/
#define SSLEBADMESSAGE			-125		/*������Ϣ			*/
#define SSLESHUTDOWN			-126		/*���Ӷ˿��Ѿ��ر�  */
#define SSLEUNSUPVERSION		-127		/*�汾��֧��		*/
#define SSLEUNSUPCIPHER			-128		/*���ܷ�����֧��	*/
#define SSLECIPHER				-129		/*���ܳ��ִ���		*/

#define SSL_RUNNINGERROR			-149		//���д���
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
#define SSLEMAXERROR			-200			/*δ֪����			*/
#endif

/* SSL EVENT CODE */
#define SSL_EVCONNECT			1			//���ӳɹ��¼���SSL���ֳɹ�����ʧ���Ժ��Ϸ���
#define SSL_EVREAD				2			//���ݵ����¼���SSL�˿������ݵ���ʱ���͡�
#define SSL_EVWRITE				3			//д�����������¼���SSL�˿�д�����������Ժ��͡�
#define SSL_EVCLOSE				4			//�ر��¼���SSL�˿ڹر��Ժ��͡�
#define SSL_EVALERT				5			//������Ϣ�����¼���
#define SSL_EVSESSION			6			//��ʼ�Ự�¼�

/* ssl set option code */
#define SSL_OPCYP				1			//���ü����㷨
#define SSL_OPCOM				3			//����ѹ���㷨

/* ѡ�� */
#define SSL_SESSION				5			/* ��ʼSSL�Ự	*/

/* SSL_SESSION��Ӧ��ֵ */
#define SSL_NO_SESSION			1			/* ����ʱ����������Э�� */
#define SSL_BEGIN_SESSION		2			/* ��ʼ����Э��			*/

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
