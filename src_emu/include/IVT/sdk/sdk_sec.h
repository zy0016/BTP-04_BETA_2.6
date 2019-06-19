/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_sec.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/



#ifndef _SDK_SEC_H
#define _SDK_SEC_H


/* Security Mode */
#define BTSDK_SECURITY_LOW						0x01
#define BTSDK_SECURITY_MEDIUM					0x02
#define BTSDK_SECURITY_HIGH						0x03


/* Security Level */
#define BTSDK_SSL_NO_SECURITY					0x00
#define BTSDK_SSL_AUTHENTICATION				0x01
#define BTSDK_SSL_AUTHORIZATION					0x02
#define BTSDK_SSL_ENCRYPTION					0x04
#define BTSDK_DEFAULT_SECURITY					(BTSDK_SSL_AUTHORIZATION | BTSDK_SSL_AUTHENTICATION)

/* Authorization Method */
#define BTSDK_AUTHORIZATION_ACCEPT				0x01
#define BTSDK_AUTHORIZATION_REJECT				0x02
#define BTSDK_AUTHORIZATION_PROMPT				0x03

/* Authorization Result */
#define BTSDK_AUTHORIZATION_GRANT				0x01
#define BTSDK_AUTHORIZATION_DENY				0x02

/* Trust Level */
#define BTSDK_TRUSTED 							0x01
#define BTSDK_UNTRUSTED							0x02

#define BTMGR_AUTHENCOMP(b,s)	BTMGR_SecuComplete(b,s,(UCHAR)1)
#define BTMGR_AUTHORCOMP(b,s)	BTMGR_SecuComplete(b,s,(UCHAR)2)
#define BTMGR_ENCRYCOMP(b,s)	BTMGR_SecuComplete(b,s,3)

/*************** Function Prototype ******************/
/* Security Manager */
BTUINT32 Btsdk_SetSecurityMode(BTUINT16 security_mode);
BTUINT32 Btsdk_GetSecurityMode(BTUINT16 *security_mode);
BTUINT32 Btsdk_SetServiceSecurityLevel(BTSVCHDL svc_hdl, BTUINT8 level);
BTUINT32 Btsdk_GetServiceSecurityLevel(BTSVCHDL svc_hdl, BTUINT8 *level);
BTUINT32 Btsdk_SetAuthorizationMethod(BTSVCHDL svc_hdl, BTUINT32 method);
BTUINT32 Btsdk_SetTrustedDevice(BTSVCHDL svc_hdl, BTDEVHDL dev_hdl, BTBOOL bIsTrusted);
BTUINT32 Btsdk_GetTrustedDeviceList(BTSVCHDL svc_hdl, BTDEVHDL *dev_hdl, BTUINT32 *dev_count);
BTUINT32 Btsdk_AuthorRsp(BTSVCHDL svc_hdl, BTDEVHDL dev_hdl, BTUINT16 author_result);


#endif
