/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    hf_hfap.h
Abstract:
	This module declares all the constants and functions used by the HF application,
	which is defined in module hf_hfap.c
Author:
    Zhang Jian
Revision History:
	2003.11
---------------------------------------------------------------------------*/
#ifndef HF_HFAP_H
#define HF_HFAP_H

#include "hfp_ui.h"

typedef void (HFAP_Callback)(UCHAR event, UCHAR* param, WORD param_len);

struct HFAPConnInfo {
    UCHAR *rmt_bd;
	UCHAR is_hsp;
};

/*HF state*/
#define HFAP_ST_IDLE                          0x01 /*conn not established, for HSP & HFP*/
#define HFAP_ST_STANDBY                       0x02 /*conn established, for HSP & HFP*/
#define HFAP_ST_RINGING                       0x03 /*ringing, for HSP & HFP*/
#define HFAP_ST_OUTGOINGCALL                  0x04 /*outgoing call, only for HFP*/
#define HFAP_ST_ONGOINGCALL                   0x05 /*ongoing call, only for HFP and HSP*/

/*local supported features, defined in spec*/
#define HFAP_FEA_NREC                         HFP_HF_FEA_NREC
#define HFAP_FEA_3WAY_CALLING                 HFP_HF_FEA_3WAYCALL
#define HFAP_FEA_CALLING_LINE_NUM             HFP_HF_FEA_CLIP
#define HFAP_FEA_VOICE_RECOG                  HFP_HF_FEA_BVRA
#define HFAP_FEA_RMT_VOL_CTRL                 HFP_HF_FEA_RMTVOLCTRL

/*SCO packet type, used by function HFAP_HFInit*/
#define HFAP_SCO_PKT_HV1                      SCO_PKT_HV1
#define HFAP_SCO_PKT_HV2                      SCO_PKT_HV2
#define HFAP_SCO_PKT_HV3                      SCO_PKT_HV3

#define HFAP_3WAY_MOD0                        '0' /*Set busy tone for a waiting call; Release the held call*/
#define HFAP_3WAY_MOD1                        '1' /*Release activate call & accept held/waiting call*/
#define HFAP_3WAY_MOD2                        '2' /*Swap between active call and held call; Place active call on held; Place held call on active*/
#define HFAP_3WAY_MOD3                        '3' /*Add a held call to the conversation*/
#define HFAP_3WAY_MOD4                        '4' /*Connects the two calls and disconnects the subscriber from both calls (Explicit Call Transfer)*/

/*staus available from function HFAP_GetStatus*/
#define HFAP_STATUS_LOCAL_GENERATE_RINGTONG   0x01 /*whether HF device need to generate its own in-band ring tone*/
#define HFAP_STATUS_AUDIO_CONN_ONGOING        0x02 /*whether audio connection with remote device is ongoing*/

/*event code*/
#define HFAP_EV_RINGING_IND                   0x01 /*a call is incoming*/
#define HFAP_EV_HANGUP_IND                    0x02 /*the incoming call or outgoing call or ongoing call is canceled*/
#define HFAP_EV_OUTGOINGCALL_IND              0x03 /*an outgoing call is being established*/
#define HFAP_EV_ONGOINGCALL_IND               0x04 /*a call (incoming call or outgoing call) is established (ongoing)*/
#define HFAP_EV_AG_AVAILABLE_IND              0x05 /*AG is available (connection is established)*/
#define HFAP_EV_AG_UNAVAILABLE_IND            0x06 /*AG is unavailable (connection is released or link lost)*/

#define HFAP_EV_CLI_IND                       0x41 /*caller ID incication*/
#define HFAP_EV_SPKVOL_CHANGED_IND            0x42 /*speaker volume changed indication*/
#define HFAP_EV_VOICETAG_PHONE_NUM_RSP        0x44 /*the AG inputted phone number for voice-tag*/
#define HFAP_EV_VOICE_RECOGN_ACTIVATED_IND    0x45 /*voice recognition activated indication*/
#define HFAP_EV_VOICE_RECOGN_DEACTIVATED_IND  0x46 /*voice recognition deactivated indication*/
#define HFAP_EV_TERMINATE_LOCAL_RINGTONE_IND  0x47 /*indicate app to terminate local generated ring tone*/
#define HFAP_EV_CALL_WAITING_NOTIF            0x48 /*call waiting notification*/
#define HFAP_EV_EXTEND_CMD_IND                0x49 /*indicate app extend command received*/
#define HFAP_EV_NETWORK_UNAVAILABLE_IND       0x4a /*indicate app cellular network is unavailable*/
#define HFAP_EV_NETWORK_AVAILABLE_IND         0x4b /*indicate app cellular network is available*/
#define HFAP_EV_EXTEND_FUNC_ENABLED_IND       0x4c /*indicate app to setup another SPP connection for extended functions if needed*/
#define HFAP_EV_EXTEND_FUNC_DISENABLED_IND    0x4d /*indicate app to release the SPP connection for extended functions*/
#define HFAP_EV_REPEAT_CONN_START_IND         0x4e /*indicate app the starting of repeated connecting with remote device (due to link lost!)*/
#define HFAP_EV_REPEAT_CONN_TERMINATE_IND     0x4f /*indicate app the termination of repeated connecting with remote device*/
#define HFAP_EV_SCO_CONN_IND                  0x50 /*indicate app the SCO connection has been established*/
#define HFAP_EV_SCO_DISC_IND                  0x51 /*indicate app the SCO connection has been released*/
#define HFAP_EV_HF_SVR_START_IND			  0x52 /*indicate app the HFP server is started*/
#define HFAP_EV_HF_SVR_STOP_IND				  0x53 /*indicate app the HFP server is stopped*/
#define HFAP_EV_HS_SVR_START_IND			  0x54 /*indicate app the Headset server is started*/
#define HFAP_EV_HS_SVR_STOP_IND				  0x55 /*indicate app the Headset server is stopped*/

/*user interface*/
FUNC_EXPORT void  HFAP_APPRegCbk(HFAP_Callback *pfunc);
FUNC_EXPORT UCHAR HFAP_HFInit(DWORD features, UCHAR sco_pkt_type, const WORD **call_state);
FUNC_EXPORT UCHAR HFAP_ConnWithAG(void *bd_addr);
FUNC_EXPORT void  HFAP_DiscWithAG(void);
FUNC_EXPORT void  HFAP_HFDone(void);

FUNC_EXPORT UCHAR HFAP_CancelCall(void);
FUNC_EXPORT UCHAR HFAP_AnswerCall(void);
FUNC_EXPORT UCHAR HFAP_LastNumRedial(void);
FUNC_EXPORT UCHAR HFAP_MemNumDial(void *mem_location, WORD len);
FUNC_EXPORT UCHAR HFAP_Dial(void *phone_num, WORD len);
FUNC_EXPORT UCHAR HFAP_VoiceRecognitionReq(void);
FUNC_EXPORT UCHAR HFAP_3WayCallingHandler(UCHAR op_code);

FUNC_EXPORT UCHAR HFAP_AudioConnTrans(void);
FUNC_EXPORT UCHAR HFAP_TxDTMF(UCHAR chr);
FUNC_EXPORT UCHAR HFAP_SetSpkVol(UCHAR spk_vol);
FUNC_EXPORT UCHAR HFAP_VoiceTagPhoneNumReq(void);
FUNC_EXPORT void  HFAP_TerminateRepConn(void);
FUNC_EXPORT UCHAR HFAP_ExtendCmd(void *param);
FUNC_EXPORT UCHAR HFAP_GetStatus(WORD mask);

/* Functions to be ported in application */
UCHAR GetSpkVol(UCHAR *spk_vol);
UCHAR HFMGR_GetHighestPrioDev(void *bd_addr, void *index);

#endif
