/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    avdtpuifunc.h
    
Abstract:
	This is the ui functions head file which provides user interface prototypes.
	
Author:
    Lixin Qian

Create Date:
	4/30/2003(M-D-Y)
	
Revision History:
	
---------------------------------------------------------------------------*/
#ifndef AVDTP_UI_FUNC_H
#define AVDTP_UI_FUNC_H


/********************************************************************************************/
/*																							*/
/*						Common Interfaces													*/
/*																							*/
/********************************************************************************************/
void AVDTP_Init(void);
void AVDTP_Done(void);


void AVDT_Register_SEP(struct AVDTP_SEPRegisterInStru *in);
void AVDT_Unregister_SEP(UINT8 seid);

/* Only once for all streams */
void AVDT_EventRegistration(UCHAR *cb);

/* help function */
void AVDT_FreeConfigParam(struct AVDTP_ConfigParamStru *config_param);

#ifdef CONFIG_CONFORMANCE_TESTER
UINT8 AVDTP_GetChannelType(WORD cid, UINT8 *pkt_type);
UINT8 AVDTP_GetMultiplexPacketType(WORD cid, UINT8 tsid_num, UINT8 *p_tsid, UINT8 *p_pkt_type);
#endif

/********************************************************************************************/
/*																							*/
/*						Signal interface													*/
/*																							*/
/********************************************************************************************/
/* Query the supported transport capabilities */
void AVDT_GetLocalSupportedTransportCaps(struct AVDTP_ConfigParamStru *config_param);

/* Get transport channel mtu in OPEN or STREAMING state */
void AVDT_GetTransportChannelMtu(WORD stream_handle);

/* 
 * For INT (Initiator):	
 */

void AVDT_Connect_Req(struct AVDTP_ConnectReqInStru *in);
void AVDT_Disconnect_Req(UCHAR *bd_addr);
void AVDT_Discover_Req(UCHAR* bd_addr);
void AVDT_Get_Capabilities_Req(struct AVDTP_GetCapsReqInStru *in);
void AVDT_Set_Configuration_Req(struct AVDTP_SetConfigReqInStru *in);
void AVDT_Get_Configuration_Req(WORD stream_handle);
void AVDT_Reconfigure_Req(struct AVDTP_ReconfigReqInStru *in);
void AVDT_Open_Req(WORD stream_handle);
void AVDT_Start_Req(struct AVDTP_StartSuspendReqInStru *in);
void AVDT_Close_Req(WORD stream_handle);
void AVDT_Suspend_Req(struct AVDTP_StartSuspendReqInStru *in);
void AVDT_Abort_Req(WORD stream_handle);
void AVDT_Security_Control_Req(struct AVDTP_SecurityControlReqInStru *in);

#ifdef CONFIG_CONFORMANCE_TESTER
void AVDT_UndefinedSignal_Req(UCHAR* bd_addr, UINT8 undefined_signal_id);
void AVDT_Discover_Req_For_Tester(UCHAR* bd_addr);
void AVDT_Get_Capabilities_Req_For_Tester(struct AVDTP_GetCapsReqInStru *in);
void AVDT_Get_Configuration_Req_For_Tester(WORD stream_handle, UINT8 invalid_seid);
void AVDT_Start_Req_For_Tester(struct AVDTP_StartSuspendReqInStru *in);
void AVDT_Close_Req_For_Tester(WORD stream_handle, UINT8 invalid_seid);
void AVDT_Suspend_Req_For_Tester(struct AVDTP_StartSuspendReqInStru *in);
void AVDT_Reconfigure_Req_For_Tester(struct AVDTP_ReconfigReqInStru *in, UINT8 invalid_seid);
void AVDT_Open_Req_For_Tester(UCHAR *bd_addr, UINT8 seid);
void AVDT_Set_Configuration_Req_For_Tester(struct AVDTP_SetConfigReqInStru *in, UINT8 flag);
#endif

/********************************************************************************************/
/*
 * For ACP (Acceptor):
 */
void AVDT_Connect_Rsp(struct AVDTP_ConnectRspInStru *connect_rsp);
void AVDT_Discover_Rsp(struct AVDTP_DiscoverRspInStru *discover_rsp);
void AVDT_Get_Capabilities_Rsp(struct AVDTP_GetCapabilitiesRspInStru *get_capabilities_rsp);
void AVDT_Set_Configuration_Rsp(struct AVDTP_SetConfigurationRspInStru *set_configuration_rsp);
void AVDT_Get_Configuration_Rsp(struct AVDTP_GetConfigurationRspInStru *get_configuration_rsp);
void AVDT_Reconfigure_Rsp(struct AVDTP_ReconfigRspInStru *reconfig_rsp);
void AVDT_Open_Rsp(struct AVDTP_CommonRspInStru *open_rsp);
void AVDT_Start_Rsp(struct AVDTP_StartSuspendRspInStru *start_rsp);
void AVDT_Close_Rsp(struct AVDTP_CommonRspInStru *close_rsp);
void AVDT_Suspend_Rsp(struct AVDTP_StartSuspendRspInStru *suspend_rsp);
void AVDT_Abort_Rsp(struct AVDTP_CommonRspInStru *abort_rsp);
void AVDT_Security_Control_Rsp(struct AVDTP_SecurityControlRspInStru *security_control_rsp);



/********************************************************************************************/
/*																							*/
/*						Data Read & Write interface											*/
/*																							*/
/********************************************************************************************/
/* write */
void AVDT_Write_Req(struct AVDTP_WriteReqInStru *packet);

/* read */
void AVDT_ReadStreamData(struct AVDTP_ReadReqInStru *read_req_in);




#endif
