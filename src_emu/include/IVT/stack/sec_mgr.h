#ifndef _SEC_MGR_H
#define _SEC_MGR_H

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    Sec.h
Abstract:
	The module defines the parameter structures and prototypes of the security manager functions 
	
Author: 
Revision History:
---------------------------------------------------------------------------*/
#define BD_ADDR_LEN          6
#define APP_MAX_NAME_LEN    24
#define MAX_PIN_CODE_LEN    16
#define MAX_LINK_KEY_LEN    16
#define AUTHORIZED           1
#define FUTURE_TRUSTED      2

#define SEC_NOPENDING					0/*00000000*/
#define SEC_AUTHENTICATION_PENDING		1/*00000001*/
#define SEC_AUTHORISATION_PENDING		2/*00000010*/

/* the structure for requesting security access */
struct SecurityAccessStru{
	UCHAR bd_addr[BD_ADDR_LEN];			/* baseband address */
	UCHAR protocol_id;					/* protocol identification */
	WORD channel;						/* psm value if used at l2cap level or channelid if used at rfcomm level*/
	UCHAR is_incoming;					/* if incoming connection request */
};

/* the structure for registering service security level */
struct AppSecRegStru{
    UCHAR name[APP_MAX_NAME_LEN];     /* human readable name of application */                  
	UCHAR protocol_id;					/* protocol identification of which protocol has to make the decision for access */
	WORD channel;						/* psm value used at l2cap level or channel used at multiprotocol(such as  rfcomm) level*/
	UCHAR sl;							/* security level */
    UCHAR index;                       /* index of service */
};

/* the structure for registering multiprotocol security level */
struct MultiProtSecRegStru{
    UCHAR protocol_id;                  /* protocol identification */
    WORD psm;                         /* psm value used at l2cap level */
    UCHAR sl;                           /* security level */
};

/* security information related to the remote device*/
struct RmtDevSecuInfoStru{
	UCHAR pin_len;                         /* length of pin code */
    UCHAR pin_cd[MAX_PIN_CODE_LEN];          /* PIN code */
    UCHAR ln_key[MAX_LINK_KEY_LEN];          /* link key */
    UCHAR lk_manage;                     /*link manage mode*/
    UCHAR is_trusted;                     /*if device is trusted */
};

UCHAR SecAccessReq(struct SecurityAccessStru *sa);
UCHAR SecAccessReqMultiProt(struct SecurityAccessStru *sa, UCHAR *pa);
UCHAR SecCreateTrustedRelationship(UCHAR *bd_addr);
UCHAR SecCancelTrustedRelationship(UCHAR *bd_addr);
UCHAR SecRegisterApp(struct AppSecRegStru *as);
UCHAR SecRegisterMultiProt(struct MultiProtSecRegStru *ms);
UCHAR SecUnregisterApp(struct AppSecRegStru *as);
UCHAR SecUnregisterMultiProt(struct MultiProtSecRegStru *ms);
UCHAR SecSetAuthorizeInfo(UCHAR index, UCHAR *bd_addr);
UCHAR SecClearAuthorizeInfo(UCHAR index, UCHAR *bd_addr);
UCHAR SecPairRemDev(UCHAR *bd_addr);
UCHAR SecGetRmtDevSecuInfo(UCHAR *bd_addr, struct RmtDevSecuInfoStru *secu_info);

#endif
