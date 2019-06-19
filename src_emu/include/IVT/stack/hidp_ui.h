/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* Copyright (c) 1999-2002 IVT Corporation
*
* All rights reserved.
* 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    hidp_ui.h

Abstract:
	This file defines all the UI data structures and UI funtions of the HID profile.

Author:
    Yongping Zeng

Revision History:
2002.3
	create the initiator version
2003.3
	adapted for SOC stack by luo xiqiong
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


#ifndef HIDP_UI_H
#define HIDP_UI_H

#ifndef BDADDRLEN
#define BDADDRLEN 6
#endif

#define HID_MAX_CONN_NUM 0x16

#ifdef CONFIG_HID_CLIENT

struct HID_ClntLocAttr{ //use in HIDP_ClntStart:)

	WORD cbsize;
	UCHAR subclass;
	UCHAR bdaddr[BDADDRLEN];
};

UCHAR HIDP_Init(void);
UCHAR HIDP_Done(void);
UCHAR HIDP_AppRegCbk(UCHAR* hdl, UCHAR* event_cbk);
UCHAR* HIDP_ClntInit(UCHAR *cbk);
UCHAR* HIDP_ClntStart(UCHAR *bd, UCHAR *sdap_attrib, UCHAR *local_attrib);
UCHAR HIDP_ClntStop(UCHAR* hdl);
UCHAR HIDP_DiscReq(DWORD* hdl, UCHAR* bdaddr);
void HIDP_ConnReq(struct HID_ConnReqInStru* in, struct HID_ConnReqOutStru* out);
UCHAR HIDP_CtrlReq( struct HID_CtrlReqInStru* in);
UCHAR HIDP_GetReportReq(struct HID_GetReportReqInStru* in);
UCHAR HIDP_SetReportReq(struct HID_SetReportReqInStru* in);
UCHAR HIDP_GetProtReq(DWORD* hdl);
UCHAR HIDP_SetProtReq(struct HID_SetProtReqInStru* in);
UCHAR HIDP_GetIdleReq(DWORD* hdl);
UCHAR HIDP_SetIdleReq(struct HID_SetIdleReqInStru* in);
UCHAR HIDP_DataReq(struct HID_DataWriteInStru* in);
int HIDP_ClntUnPlugDev( UCHAR* bdaddr);
int HIDP_ClntAddPluggedDev(UCHAR *bd_addr, struct SDAP_HIDInfoStru* sdp_attrib, UCHAR * sdap_pnp_info);
UCHAR *HIDP_FindHidHdlByHidpHdl(UCHAR *hdl);
UCHAR *HIDP_FindHidpHdlByHidHdl(UCHAR *hdl);

void HidLinkLostProcess();
void HidP_DisableAutoConnect();

#endif

/*====================================================
**====================================================
**/

#ifdef CONFIG_HID_SERVER

#define HID_SVR_IDLE		0x0
#define HID_SVR_HALFCONN	0x1
#define HID_SVR_HALFDISC	0x2
#define HID_SVR_CONNECTED	0x3

struct HID_ReportStru{ /*use for HIDP_WriteReport*/
	WORD len;
	UCHAR report[1];
};

struct HID_SvrStru
{
	DWORD srv_hdl;					// HID service handle
	StatusInd *status_cbk;			// the status callback function for app
	UCHAR bd_addr[BDADDRLEN];		// the bd address of the remote device
	UCHAR subclass;					// added by luoxi, the local device's subclass.
	WORD ctrl_cid;					// the control channel id of l2cap
	WORD int_cid;					// the interrupt channel id of l2cap
	WORD ctrl_mtu;					// the out MTU for control channel
	WORD int_mtu;					// the out MTU for control channel
	UCHAR status;					// the status of HID server: 
									// HID_SVR_IDLE, HID_SVR_HALFCONN, HID_SVR_HALFDISC, HID_SVR_CONNECTED	//*** added by xiaoheng 11-06
	UCHAR virtual_cable; 			/* 1: support Virtual Connection; 0: unsupport Virtual Connection*/	
#ifdef CONFIG_HID_SAR
	struct NewBtList *datalist;		// the list for receiving data packet
	struct NewBtList *ctrllist;		// the list for receiving control packet
#endif
};

struct HID_SvrLocAttr {
	WORD cbSize;
	UCHAR subclass;
	UCHAR virtual_cable; /* 1: support Virtual Connection; 0: unsupport Virtual Connection*/
	UCHAR host_bd[BDADDRLEN];	
};


UCHAR HIDP_SvrInit(void);
UCHAR HIDP_SvrDone(void);
UCHAR HIDP_SvrAppRegCbk(UCHAR* hdl, UCHAR* event_cbk);
UCHAR* HIDP_SvrStart(UCHAR * local_attrib);
UCHAR HIDP_SvrStop(UCHAR* handle);
UCHAR HIDP_WriteReport(UCHAR* hdl, UCHAR* report, WORD length);
UCHAR HIDP_CableUnplug(UCHAR* hdl);
UCHAR HIDP_DiscClnt(UCHAR* handle);
UCHAR HIDP_ConnClnt(UCHAR* handle, UCHAR *bdaddr);

#endif	// end HID_SERVER

#ifndef CONFIG_HID_CLIENT
#define HIDP_Init()				0
#define HIDP_Done()				0
#define HIDP_AppRegCbk(a, b)	0
#define HIDP_ClntStart(a, b, c)	0
#define HIDP_ClntStop(a) 		0
#endif
#ifndef CONFIG_HID_SERVER
#define HIDP_SvrInit()			0
#define HIDP_SvrDone()			0
#define HIDP_SvrAppRegCbk(a,b)	0
#define HIDP_SvrStart(a)		0
#define HIDP_SvrStop(a) 		0
#endif

#endif
