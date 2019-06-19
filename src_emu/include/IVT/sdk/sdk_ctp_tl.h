/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_ctp_tl.h
  Author:       Lin Hong
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/



#ifndef _SDK_CTP_TL_H
#define _SDK_CTP_TL_H

/* 
				event					parg		arg	

	(BTSDK_APP_EV_CTPICP_ATTACH,		NULL,		conn_hdl)	attached with the gateway
	(BTSDK_APP_EV_CTPICP_DETACH,		NULL,		conn_hdl)	detached with the gateway
	(BTSDK_APP_EV_CTPICP_CTPRING,		phone_no,	0)			CTP ring from the gateway
	(BTSDK_APP_EV_CTPICP_CTPREGRING,	phone_no,	0)			CTP ring from the gateway 
																called after Btsdk_RegisterNextCall()
	(BTSDK_APP_EV_CTPICP_ICPRING,		NULL,		dev_hdl)	ICP ring
	(BTSDK_APP_EV_CTPICP_HANGUP,		NULL,		cause)		call hangup
	(BTSDK_APP_EV_CTPICP_ACTIVE,		NULL,		0)			call is active

*/

	
typedef void  (Btsdk_Ctpicp_Event_Ind_Func)(BTUINT16 event, BTUINT8 *parg, BTUINT32 arg);

BTUINT32 Btsdk_CtpIcpInit(void);
BTUINT32 Btsdk_CtpIcpDone(void);
BTSVCHDL Btsdk_RegisterICPService(void);
BTUINT32 Btsdk_UnregisterICPService(void);
void Btsdk_RegisterCtpicpCallback(Btsdk_Ctpicp_Event_Ind_Func *func);

BTUINT32 Btsdk_JoinGateway(BTDEVHDL dev_hdl, BTCONNHDL *pconn_hdl);
BTUINT32 Btsdk_LeaveGateway(BTCONNHDL conn_hdl);
BTUINT32 Btsdk_RegisterNextCall(BTDEVHDL dev_hdl, BTUINT8 *phone_no);
BTUINT32 Btsdk_CallTranfer(BTDEVHDL dev_hdl);

BTUINT32 Btsdk_CtpDial(BTDEVHDL dev_hdl, BTUINT8 *phone_no);
BTUINT32 Btsdk_CtpHangup(BTDEVHDL dev_hdl);
BTUINT32 Btsdk_CtpAnswer(BTDEVHDL dev_hdl);
BTUINT32 Btsdk_CtpSendDTMF(BTDEVHDL dev_hdl, BTUINT8 number);

BTUINT32 Btsdk_IcpDial(BTDEVHDL dev_hdl, BTCONNHDL *pconn_hdl);
BTUINT32 Btsdk_IcpHangup(BTCONNHDL conn_hdl);
BTUINT32 Btsdk_IcpAnswer(BTCONNHDL conn_hdl);

#endif
