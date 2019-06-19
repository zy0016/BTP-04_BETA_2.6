/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    sppstru.h
Abstract:
	This file includes the definition of structs and constants in the SPP
Author:
    CHEN ZHONGYI
Revision History:
	2001.08.14 
		revise definition of MODEM_CLR_FC from ~MODEM_FC_ON to (~MODEM_FC_ON & 0xFF)
	2003.01.19
		the structure of the SPP_CommEntryStru be modified
---------------------------------------------------------------------------*/



#ifndef __SPPSTRU_H
#define __SPPSTRU_H

#include "rfcommui.h"

/* Windows defined	*/
#define MODEM_CTS_ON				(UCHAR)0x10
#define MODEM_DSR_ON				(UCHAR)0x20
#define MODEM_RING_ON				(UCHAR)0x40
#define MODEM_DCD_ON				(UCHAR)0x80

/* Stack defined	*/
#define MODEM_CLR_BIT				(UCHAR)0x01
#define MODEM_FC_ON					(UCHAR)0x02
#define MODEM_RTC_ON				(UCHAR)0x04
#define MODEM_RTR_ON				(UCHAR)0x08
#define MODEM_IC_ON					(UCHAR)0x40
#define MODEM_DV_ON					(UCHAR)0x80

#define MODEM_CLR_DTRDSR			(UCHAR)(MODEM_RTC_ON | MODEM_CLR_BIT)
#define MODEM_CLR_CTSRTS			(UCHAR)(MODEM_RTR_ON | MODEM_CLR_BIT)
#define MODEM_CLR_FC				(UCHAR)(MODEM_FC_ON  | MODEM_CLR_BIT)
#define MODEM_CLR_RI				(UCHAR)(MODEM_IC_ON  | MODEM_CLR_BIT)
#define MODEM_CLR_DCD				(UCHAR)(MODEM_DV_ON  | MODEM_CLR_BIT)
#define MODEM_SET_DTRDSR			MODEM_RTC_ON
#define MODEM_SET_CTSRTS			MODEM_RTR_ON
#define MODEM_SET_FC				MODEM_FC_ON
#define MODEM_SET_RI				MODEM_IC_ON 
#define MODEM_SET_DCD				MODEM_DV_ON 

/* modem status changed event */
#define MODEM_EV_CTS        		0x01
#define MODEM_EV_DSR				0x02
#define MODEM_EV_RING				0x04
#define MODEM_EV_RLSD         		0x08
#define MODEM_EV_RXCHAR       		0x0100
#define MODEM_EV_TXEMPTY       		0x0200

#define	SPP_EV_RXCHAR				0x0001	 /* Any Character received		*/
#define	SPP_EV_CTS					0x0002	 /* CTS changed state			*/
#define	SPP_EV_DSR					0x0004	 /* DSR changed state			*/
#define	SPP_EV_RLSD					0x0008	 /* RLSD changed state			*/
#define	SPP_EV_ERR					0x0010	 /* Line status error occurred	*/
#define	SPP_EV_RING					0x0020	 /* Ring signal detected		*/
#define	SPP_EV_FC 					0x0040	 /* Flow control				*/
#define SPP_EV_CONNIND				0x0080   /* Connection					*/
#define SPP_EV_DISCIND				0x0100   /* Disconnection				*/
#define SPP_EV_LINKLOSTIND			0x0200   /* Link lost					*/
#define SPP_EV_TXCHAR				0x0400	 /* Any Character sent			*/

#define SPP_ERR_OVERRUN				0x02
#define SPP_ERR_PARITY				0x04
#define SPP_ERR_FRAME				0x08

#define ENTRY_DEL					0x01	/* CommEntry should be delteted	*/
#define CREDIT_OFF					0x02	/* Credti Flow Control not used	*/
#define INVALID_VPORT 				0		/* Invalid virtual port number	*/

#define SPP_EX_CREDIT				0x7F

typedef void (*SPP_Callback)(WORD event,UCHAR *param);

struct SPP_RcvDataStru {
	WORD	length;
	struct BuffStru		*buf;
};

struct SPP_StatusStru {
	UCHAR	bd[BDLENGTH];
	UCHAR   creditin;
	UCHAR	creditout;
	UCHAR	creditsum;
	UCHAR   congest;
	UCHAR   mdmin;
	UCHAR   mdmout;
	UCHAR   line;
	struct struRpnPar		rpn;
	WORD					mask;
	SPP_Callback			cbk;
};

struct SPP_CommEntryStru {
	SYSTEM_LOCK lock;
	HANDLE	refevent;
	CHAR	refcount;
	UCHAR	flag;
	WORD	rfhandle;
	WORD	uuid;
	WORD    mfs;
	UCHAR   svrchnl;
	UCHAR	issvr;
	UCHAR   vport;
	struct BtList			*rcvdatalist;
	struct SPP_StatusStru	status;
};

struct SPP_CfgStru {
	DWORD	svrchnl_status;
	DWORD	vport_status;
	struct BtList			*commlist;
};

struct SPP_InfoStru {
	UCHAR	vport;
	UCHAR	svr_side;
	UCHAR	svr_chnl;
	UCHAR	com_index;
	DWORD	svc_hdl;
};

struct SPP_CbkStru {
	UCHAR		*hdl;
	UCHAR		*cbk;
};

struct SPP_LocalAttrStru {
#ifdef CONFIG_SDP_FIXED_DATABASE
	DWORD	svc_hdl;
	UCHAR	svr_chnl;
#endif
	UCHAR	com_index;
	WORD					ev_mask;
	SPP_Callback			ev_cbk;
};

struct SPP_EntryParStru {
	WORD	uuid;
	WORD	mfs;
	UCHAR	*svrchnl;
	UCHAR	*bd;
	BOOL	issvr;
	WORD					ev_mask;
	SPP_Callback			ev_cbk;
};

struct SPP_BytesSentStru {
	UCHAR vport;
	UCHAR reserved[3];
	DWORD bytes_sent;
};

#endif
