/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    gavdpuifunc.h
    
Abstract:
	This is the ui functions head file which provides user interface prototypes.
	
Author:
    Chen Zhongyi

Create Date:
	11/12/2002
	
Revision History:
	
---------------------------------------------------------------------------*/
#ifndef GAVDP_UI_FUNC_H
#define GAVDP_UI_FUNC_H


/********************************************************************************************/
/*																							*/
/*						Common Interfaces													*/
/*																							*/
/********************************************************************************************/

BOOL GAVDP_Init(void);
BOOL GAVDP_Done(void);

void GAVDP_SEPCreate(
	/*[in]*/struct GAVDP_SEPCreateInStru*	in, 
	/*[out]*/UCHAR*						seid);
void GAVDP_SEPDestroy(
	/*[in]*/UCHAR 	seid, 
	/*[out]*/WORD*	rsp);
void GAVDP_SEPDiscover(
	/*[in]*/UCHAR*							bd_addr, 
	/*[out]*/struct GAVDP_SEPDiscoverOutStru*	out);
void GAVDP_SEPGetCaps(
	/*[in]*/struct GAVDP_GetCapsReqInStru*	in,
	/*[out]*/struct GAVDP_GetCapsOutStru*		out);


void GAVDP_Connect(
	/*[in]*/struct GAVDP_ConnectReqInStru* in, 
	/*[out]*/struct GAVDP_ConnectReqOutStru*	out);

void GAVDP_Disconnect(
	/*[in]*/UCHAR*	bd_addr);


/********************************************************************************************/
/*																							*/
/*						Signaling procedures												*/
/*																							*/
/********************************************************************************************/
/* main procedures */

void GAVDP_StreamSetConfig(
	/*[in]*/struct GAVDP_SetConfigReqInStru*	in, 
	/*[out]*/struct GAVDP_StreamConfigOutStru*	out);
void GAVDP_StreamGetConfig(
	/*[in]*/WORD							stream_handle, 
	/*[out]*/struct GAVDP_GetConfigOutStru*	out);
void GAVDP_StreamReconfig(
	/*[in]*/struct GAVDP_ReconfigReqInStru*		in, 
	/*[out]*/struct GAVDP_StreamReconfigOutStru*	out);
void GAVDP_StreamOpen(
	/*[in]*/WORD	stream_handle, 
	/*[out]*/WORD*	error_code);
void GAVDP_StreamStart(
	/*[in]*/WORD	stream_handle, 
	/*[out]*/WORD*	error_code);
void GAVDP_StreamStartEx(
	/*[in]*/ struct GAVDP_StartSuspendReqInStru*	in, 
	/*[out]*/struct GAVDP_StartSuspendOutStru*		out);
void GAVDP_StreamClose(
	/*[in]*/WORD	stream_handle,
	/*[out]*/WORD*	error_code);
void GAVDP_StreamSuspend(
	/*[in]*/WORD	stream_handle, 
	/*[out]*/WORD*	error_code);
void GAVDP_StreamSuspendEx(
	/*[in]*/struct GAVDP_StartSuspendReqInStru* 	in,
	/*[out]*/struct GAVDP_StartSuspendOutStru*		out);
void GAVDP_StreamAbort(
	/*[in]*/WORD	stream_handle,
	/*[out]*/WORD*	error_code);
void GAVDP_StreamSecurityControl(
	/*[in]*/struct GAVDP_SecurityControlReqInStru*	in, 
	/*[out]*/struct GAVDP_SecurityControlOutStru*	out);


/********************************************************************************************/
/*																							*/
/*						SEP maintaining procedures											*/
/*																							*/
/********************************************************************************************/

/********************************************************************************************/
/*																							*/
/*						Response															*/
/*																							*/
/********************************************************************************************/
/*GAVDP Response APIs corresponding to indications:*/

void GAVDP_SEPGetCapsRsp(
	/*[in]*/struct GAVDP_GetCapsRspInStru*		caps);
void GAVDP_StreamSetConfigRsp(
	/*[in]*/struct GAVDP_SetConfigRspInStru*	config);
void GAVDP_StreamGetConfigRsp(
	/*[in]*/struct GAVDP_GetConfigRspInStru*	config);
void GAVDP_StreamReconfigRsp(
	/*[in]*/struct GAVDP_ReconfigRspInStru*		reconfig);
void GAVDP_StreamOpenRsp(
	/*[in]*/struct GAVDP_CommonRspInStru*	open);
void GAVDP_StreamStartRsp(
	/*[in]*/struct GAVDP_StartSuspendRspInStru*		start);
void GAVDP_StreamCloseRsp(
	/*[in]*/struct GAVDP_CommonRspInStru*	close);
void GAVDP_StreamSuspendRsp(
	/*[in]*/struct GAVDP_StartSuspendRspInStru*		suspend);
void GAVDP_StreamAbortRsp(
	/*[in]*/struct GAVDP_CommonRspInStru*	abort);
void GAVDP_StreamSecurityControlRsp(
	/*[in]*/struct GAVDP_SecurityControlRspInStru*	sc);


/********************************************************************************************/
/*																							*/
/*						AV Streaming procedures												*/
/*																							*/
/********************************************************************************************/

	
void GAVDP_StreamWrite(
	/*[in]*/struct GAVDP_WriteReqInStru*	packet, 
	/*[out]*/WORD*						result);

/*
 * Application should utilize a seperate thread to read packet.
 */

void GAVDP_StreamRead(
	/*[in]*/struct GAVDP_ReadReqInStru*	in, 
	/*[out]*/struct GAVDP_ReadReqOutStru*	out);


/* additional basic procedures */
/* Query the supported transport capabilities */
void GAVDP_GetTransportCaps(
	struct GAVDP_ConfigParamStru *config_param);

void GAVDP_RegisterCallBack(
	/*[in]*/UCHAR 			media_type, 
	/*[in]*/ GAVDP_CallBack*	cb);	

void GAVDP_GetTransportChannelMtu(
	WORD								stream_handle,
	struct GAVDP_TransportChannelMtuStru*	mturesult);

void GAVDP_FreeConfigParam(
	struct GAVDP_ConfigParamStru* config_param);

void GAVDP_RegisterSigChnlCallBack(
	/*[in]*/ GAVDP_SigChnlCallBack*	cb);
/********************************************************************************************/
/*																							*/
/*						AV Report Interfaces												*/
/*																							*/
/********************************************************************************************/
/* 
 * Common for SRC & SNK:
 */
void GAVDP_GetPayloadTypeInfo(
	WORD 	payload_type, 
	DWORD*	sample_rate);
void GAVDP_SetPayloadTypeInfo(
	/*[in]*/struct GAVDP_PayloadTypeInfoStru*	in, 
	/*[out]*/WORD*							rsp);

/* set/get bandwidth allocated for the stream */
void GAVDP_SetStreamBandwidth(
	/*[in]*/struct GAVDP_SetStreamBandwithInStru*	in,
	/*[out]*/WORD*								rsp);
void GAVDP_GetStreamBandwidth(
	/*[in]*/UCHAR 							seid, 
	/*[out]*/struct GAVDP_StreamBandwidthStru*	bw);

/* Upper profile can adjust the periodicity of the reports */
void GAVDP_SetReportPeriodicity(
	/*[in]*/struct GAVDP_SetReportPeriodicityInStru*	in, 
	/*[out]*/WORD*								rsp);
void GAVDP_GetReportPeriodicity(
	/*[in]*/UCHAR 	seid, 
	/*[out]*/WORD*	milliseconds);
	
/* Upper profile can activate/deactivate the report service */
void GAVDP_ActivateReportService(
	/*[in]*/UCHAR 	seid, 
	/*[out]*/WORD*	rsp);
void GAVDP_DeactivateReportService(
	/*[in]*/UCHAR 	seid, 
	/*[out]*/WORD*	rsp);

/* get/set local rtcp sdes */

void GAVDP_SetSdes(
	/*[in]*/struct GAVDP_SetSdesInStru*	in, 
	/*[out]*/WORD*						rsp);
void GAVDP_GetSdes(
	/*[in]*/struct GAVDP_GetSdesInStru*	in, 
	/*[out]*/struct GAVDP_GetSdesOutStru*	out);
/*
 * For SRC (source):
 */ 
/*
 * Qos information can be used to adjust codec parameters or to change the 
 * parity code of recovery, and so on. 
 */
void GAVDP_QueryQosReport(
	/*[in]*/WORD 						stream_handle, 
	/*[out]*/struct GAVDP_ReportQosStru*	qos);


/********************************************************************************************/
/*																							*/
/*						AV Recovery Setting													*/
/*																							*/
/********************************************************************************************/

/* Parity codes of RFC2733 supported: Scheme 0, Scheme1, Scheme 2, Scheme 3 */

/* GAVDP_ParityCode supported list  are returned */
void GAVDP_QueryRecoveryParityCode(
	struct GAVDP_ParityCodeQueryOutStru *out);

/* Default parity code is Scheme 0. */
void GAVDP_GetRecoveryParityCode(
	UCHAR 			seid, 
	UCHAR*			parity_code_id);
void GAVDP_SetRecoveryParityCode(
	/*[in]*/struct GAVDP_SetRecoveryParityCodeInStru*	in, 
	/*[out]*/WORD*									rsp);
#endif
