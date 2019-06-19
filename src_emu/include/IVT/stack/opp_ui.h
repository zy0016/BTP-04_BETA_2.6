/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    opp_ui.h
Abstract:
	This module provides all the constants and user interface functions for 
	the upper layer application.
Author:
    Luo hongbo
Revision History:
	2002.5		created
version 2.0.2
	2002.7.17  wang junju: add a function OPP_Stop(OPPHDL hdl).
---------------------------------------------------------------------------*/

#ifndef OPP_UI_H
#define OPP_UI_H

#include "goep_ui.h"
#include "inbox_dep.h"

typedef UCHAR* OPPHDL;						/* OPP handle */

#define OPP_FAIL            0				/* invalid OPP response */
#define NUM_SUPPORT_FORMAT	4				/* the number of the supported file formats */
#ifdef CONFIG_MICROSOFT
#define OPP_MAX_PATH		256 			/* the maximum length of the path */
#define OPP_MAX_NAME		64				/* the maximum length of the file name */
#else
#define OPP_MAX_PATH		16
#define OPP_MAX_NAME		16
#endif
/* some types of the object */
#define OPP_VCARD_TYPE	0					/* vCard type */
#define OPP_VCAL_TYPE	1					/* vCalendar type */
#define OPP_VMSG_TYPE	2					/* vMessage type */
#define OPP_VNOTE_TYPE  3					/* vNote type */
#define OPP_ERROR_TYPE	4					/* error type */

/* reject, accept, and support attributes */
#define OPP_PUSH_REJECT			0x0001		/* reject the PUSH operation */
#define OPP_PUSH_ACCEPT			0x0003		/* accept the PUSH operation */
#define OPP_PULL_REJECT			0x0080		/* reject the PULL operation */
#define OPP_PULL_ACCEPT			0x000C		/* accept the PULL operation */
#define OPP_SUPPORT				0x0010		/* support the PUSH operation */
#define OPP_PULL_SUPPORT		0x0200		/* support the PULL operation */

/* some version attributes */
#define OPP_VER_VCARD_21		0x0020		/* vCard 2.1 */
#define OPP_VER_VCARD_30		0x0040		/* vCard 3.0 */
#define OPP_VER_VCAL_10			0x0080		/* vCalendar 1.0 */
#define OPP_VER_VCAL_20			0x0100		/* vCalendar 2.0 */

#define OPP_IS_PUSH(a)			(a&0x01)
#define OPP_IS_SUPPORT(a)		(a&0x10)
#define OPP_IS_PULL_SUPPORT(a)  (a&0x0200)
#define OPP_IS_ACCEPT(a)		(OPP_IS_PUSH(a)?a&2:a&4)

/* two kinds of OPP operation */
#define OPP_OP_PUSH	0						/* PUSH operation */
#define OPP_OP_PULL 1						/* PULL operation */


#define OPP_SUPPORT_FORMAT(a,f,v)	(a[f] |= (f<OPP_VMSG_TYPE)?(OPP_SUPPORT|v):OPP_SUPPORT)
#define OPP_SUPPORT_PULL(a)			(a[OPP_VCARD_TYPE] |=OPP_PULL_SUPPORT)
									
#define OPP_ACCEPT_FORMAT(a,op,f)	(a[f] |= (op==OPP_OP_PUSH)?OPP_PUSH_ACCEPT:OPP_PULL_ACCEPT)
#define OPP_REJECT_FORMAT(a,op,f)	(a[f] |= (op==OPP_OP_PUSH)?OPP_PUSH_REJECT:OPP_PULL_REJECT)

#define ATTRIB_MASK		 0x01
#define INBOX_PATH_MASK	 0x02
#define OUTBOX_PATH_MASK 0x04
#define OWN_CARD_MASK	 0x08
#define CARD_PATH_MASK	 0x10

#define INVALID_OPP_HDL	NULL				/* invalid opp handle */
#define IS_VALID_OPP_HDL(hdl) (hdl!=INVALID_OPP_HDL)

/* the OPP service attribute */
struct OPP_AttribStru{
	DWORD      svc_hdl;
	WORD       attrib[NUM_SUPPORT_FORMAT];	/* authentication and access,and not support attribute */
	UCHAR      inbox_path[OPP_MAX_PATH];	/* the inbox path */
	UCHAR	   outbox_path[OPP_MAX_PATH];	/* the outbox path */
	UCHAR      own_card[OPP_MAX_NAME];		/* busiess card */
	UCHAR      own_card_path[OPP_MAX_PATH];	/* path of the business card */
	UCHAR	   svr_chnl;					/* the output parameter */
};

struct OPP_InfoStru{
	UCHAR code;
	UCHAR name[OPP_MAX_NAME];
};

/* the common functions */
const char* OPP_GetVersion(void);											/* Get the version of OPP */
BOOL OPP_RegCbk(OPPHDL hdl,Status_Cbk status_cbk);							/* Register the event callback function */
void OPP_GetRemoteBd(UCHAR *hdl, UCHAR *bd);								/* Get the address of the remote device */
void OPP_SetAttrib(UCHAR *hdl,struct OPP_AttribStru* attrib,UCHAR mask);	/* Set the attributes of OPP service */
void OPP_GetAttrib(UCHAR *hdl,struct OPP_AttribStru* attrib,UCHAR mask);	/* Get the attributes of OPP service */
void OPPAPP_RegCbk(UCHAR *hdl,UCHAR* pfunc);								/* Register profile call back */

/* Client side functions */
#ifdef CONFIG_OPP_CLIENT
const char* OPP_GetRmtCardName(void);										/* Get the name of the card pulled from the remote device */
WORD   OPP_GetLastErr(OPPHDL hdl);											/* Get the error code of the latest operation */
OPPHDL OPP_ClntStart(UCHAR *bd, UCHAR *sdp_attrib, UCHAR *attrib);			/* Start the OPP client to connect */
DWORD OPP_ClntStop(OPPHDL hdl);												/* Stop  the OPP client to disconnect */
BOOL OPP_Stop(OPPHDL hdl);													/* Stop the transferring procedure */
BOOL OPP_PushObj(UCHAR *hdl,const char *name);								/* Push objects to the server */
BOOL OPP_PullObj(UCHAR *hdl,const char *path);								/* Pull business card from the server */
BOOL OPP_ExchangeObj(UCHAR *hdl);											/* Exchange business cards between server and client */
WORD OPP_Conn(UCHAR *hdl);													/* Set up the OPP connection */
WORD OPP_Disc(UCHAR *hdl);													/* Disconnect the OPP connection */
WORD OPP_PushObj_hlp(UCHAR *hdl,const char *name);							/* Push a object to the server */
WORD OPP_PullObj_hlp(UCHAR *hdl,const char *path);							/* Pull the business card from the server */

#ifdef CONFIG_OBEX_ADDITIONAL
BOOL   OPP_PutCancel(OPPHDL hdl);											/* Cancel the put operation */
#endif

#endif

/* Server side functions */
#ifdef CONFIG_OPP_SERVER
OPPHDL OPP_SvrStart(UCHAR *attrib);											/* Start the OPP service */
DWORD OPP_SvrStop(OPPHDL hdl);												/* Stop the OPP service */
#endif

#endif
