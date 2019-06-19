/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    ag_hfap.h
Abstract:
	This module declares all the constants and functions used by the AG application,
	which is defined in module ag_hfap.c
Author:
    Zhang Jian
Revision History:
	2003.11
---------------------------------------------------------------------------*/
#ifndef HF_CTRL_H_
#define HF_CTRL_H_

#include "hfp_ui.h"

typedef void (AGAP_Callback)(UCHAR event, UCHAR *param, WORD param_len);

/*used in the AGAP_EV_HF_AVAILABLE_IND event*/
struct AGAP_ConnInfo {
    UCHAR is_hsp;    /*whether hsp is connected*/
	UCHAR *bd_addr;  /*peer bd address*/
};

/*AG state, parameter for event AGAP_EV_STATE_CHANGED_IND*/
#define AGAP_ST_IDLE                           0x01 /*before service level connection is established*/
#define AGAP_ST_STANDBY                        0x02 /*service level connection is established*/
#define AGAP_ST_RINGING                        0x03 /*ringing*/
#define AGAP_ST_OUTGOINGCALL                   0x04 /*outgoing call*/
#define AGAP_ST_ONGOINGCALL                    0x05 /*ongoing call*/
#define AGAP_ST_BVRA                           0x06 /*voice recognition is ongoing*/
#define AGAP_ST_VOVG                           0x07

/*local supported features, defined in spec, used by function AGAP_AGInit*/
#define AGAP_FEA_3WAY_CALLING                  HFP_AG_FEA_3WAYCALL
#define AGAP_FEA_NREC                          HFP_AG_FEA_NREC
#define AGAP_FEA_VOICE_RECOG                   HFP_AG_FEA_BVRA
#define AGAP_FEA_INBAND_RING                   HFP_AG_FEA_INBANDRING
#define AGAP_FEA_VOICETAG_PHONE_NUM            HFP_AG_FEA_BINP
#define AGAP_FEA_REJ_CALL                      HFP_AG_FEA_NETWORK

/*SCO packet type, used by function AGAP_AGInit*/
#define AGAP_SCO_PKT_HV1                       SCO_PKT_HV1
#define AGAP_SCO_PKT_HV2                       SCO_PKT_HV2
#define AGAP_SCO_PKT_HV3                       SCO_PKT_HV3

/*available staus from function AGAP_GetStatus*/
#define AGAP_STATUS_GENERATE_INBAND_RINGTONG   0x01 /*whether AG is capable of generating in-band ring tone*/
#define AGAP_STATUS_AUDIO_CONN_ONGOING         0x02 /*whether audio connection with remote device is ongoing*/

/*current state mask code for function AGAP_SetCurIndicatorVal*/
#define AGAP_INDICATOR_SVC_UNAVAILABLE         0x00
#define AGAP_INDICATOR_SVC_AVAILABLE           0x01
#define AGAP_INDICATOR_ACTIVE                  0x02
#define AGAP_INDICATOR_INCOMING                0x04
#define AGAP_INDICATOR_DIALING                 0x08
#define AGAP_INDICATOR_ALERTING                0x10

/*the initiator of the answered incoming call, parameter of the AGAP_EV_INCOMINGCALL_ANSWERED event*/
#define AGAP_HF_ANSWERED_INCOMINGCALL          0x01
#define AGAP_AG_ANSWERED_INCOMINGCALL          0x02

/*the initiator of the canceled incoming/outgoing/ongoing call, parameter of the AGAP_EV_CALL_CANCELED event*/
#define AGAP_HF_CANCELED_CALL                  0x01
#define AGAP_AG_CANCELED_CALL                  0x02
#define AGAP_NETWORK_CANCELED_CALL             0x03
#define AGAP_TBUSY_CANCELED_CALL               0x04

/*possible received events from cellular network*/
#define AGAP_NETWORK_RMT_IS_BUSY               0x01
#define AGAP_NETWORK_ALERTING_RMT              0x02
#define AGAP_NETWORK_INCOMING_CALL             0x03
#define AGAP_NETWORK_RMT_ANSWER_CALL           0x04
#define AGAP_NETWORK_LINK_NOT_ESTABLISHED      0x05
#define AGAP_NETWORK_SVC_UNAVAILABLE           0x06
#define AGAP_NETWORK_SVC_AVAILABLE             0x07

/*event code*/
#define AGAP_EV_STATE_CHANGED_IND              0x01 /*state changed indication*/
#define AGAP_EV_RMT_RELEASE_CALL_IND           0x02 /*the ongoing call is released by remote party*/
#define AGAP_EV_RMT_IS_BUSY_IND                0x03 /*remote party is busy during the outgong call procedure*/
#define AGAP_EV_HF_AVAILABLE_IND               0x04 /*HF device is available (connection is established)*/
#define AGAP_EV_HF_UNAVAILABLE_IND             0x05 /*HF device is unavailable (connection is released or link lost)*/
#define AGAP_EV_VOICE_RECOG_ACTIVATE_REQ       0x41 /*HF request to start voice recognition procedure*/
#define AGAP_EV_VOICE_RECOG_DEACTIVATE_REQ     0x42 /*requests AG to deactivate the voice recognition procedure*/
#define AGAP_EV_INCOMINGCALL_ANSWERED_IND      0x43 /*the incoming call is answered (by HF/HS or AG/VG)*/
#define AGAP_EV_CALL_CANCELED_IND              0x44 /*the incoming/outgoing/ongoing call is canceled (by HF/HS or AG/VG or network)*/
#define AGAP_EV_AUDIO_CONN_ESTABLISHED_IND     0x45 /*audio connection with HF is established*/
#define AGAP_EV_AUDIO_CONN_RELEASED_IND        0x46 /*audio connection with HF is released*/
#define AGAP_EV_CUR_INDICATOR_VAL_REQ          0x47 /*set the current indicator during the service level connection initialization procedure*/
#define AGAP_EV_DTMF_IND                       0x48 /*instructs AG to transmit the specific DTMF code*/
#define AGAP_EV_SPKVOL_CHANGED_IND             0x49 /*speaker volume of HF device has changed*/
#define AGAP_EV_VOICETAG_PHONE_NUM_REQ         0x4a /*requests AG to input a phone number for the voice-tag at the HF side*/
#define AGAP_EV_HF_DIAL_IND                    0x4b /*instructs AG to dial the specific phone number*/
#define AGAP_EV_HF_MEM_DIAL_IND                0x4c /*instructs AG to dial the phone number indexed by the specific memory location of SIM card*/
#define AGAP_EV_HF_LASTNUM_REDIAL_IND          0x4d /*instructs AG to redial the last dialed phone number*/
#define AGAP_EV_NREC_DISENABLE_IND             0x4e /*requests AG to disenable NREC function*/
#define AGAP_EV_GENERATE_INBAND_RINGTONE_IND   0x4f /*generate the in-band ring tone*/
#define AGAP_EV_TERMINATE_INBAND_RINGTONE_IND  0x50 /*terminate the in-band ring tone*/
#define AGAP_EV_EXTEND_CMD_IND                 0x51 /*extend command is received*/
#define AGAP_EV_NETWORK_AVAILABLE_IND          0x52 /*cellular network is available*/
#define AGAP_EV_NETWORK_UNAVAILABLE_IND        0x53 /*cellular network is unavailable*/
#define AGAP_EV_PAIRED_DEV_BDADDR_REQ          0x54 /*get paired device's baseband address*/
#define AGAP_EV_HFAG_SVR_START_IND			   0x55 /*indicate app the HFP AG server is started*/
#define AGAP_EV_HFAG_SVR_STOP_IND			   0x56 /*indicate app the HFP AG server is stopped*/
#define AGAP_EV_HSAG_SVR_START_IND			   0x57 /*indicate app the Headset AG server is started*/
#define AGAP_EV_HSAG_SVR_STOP_IND			   0x58 /*indicate app the Headset AG server is stopped*/

/*user interface*/
FUNC_EXPORT void  AGAP_APPRegCbk(AGAP_Callback *pfunc);
FUNC_EXPORT UCHAR AGAP_AGInit(DWORD features, UCHAR sco_pkt_type, const WORD **call_state);
FUNC_EXPORT UCHAR AGAP_ConnWithHF(UCHAR *is_hsp, UCHAR *bd_addr);
FUNC_EXPORT void  AGAP_DiscWithHF(void);
FUNC_EXPORT void  AGAP_AGDone(void);
FUNC_EXPORT WORD  AGAP_PairWithHF(UCHAR *bd_addr, UCHAR *is_hsp);

FUNC_EXPORT void  AGAP_AGAnswerCall(void);
FUNC_EXPORT void  AGAP_AGOriginateCall(void);
FUNC_EXPORT void  AGAP_AGCancelCall(void);
FUNC_EXPORT void  AGAP_NetworkEvent(UCHAR ev, void *param);
FUNC_EXPORT void  AGAP_VoiceRecogDeactivatedInd(void);

FUNC_EXPORT void  AGAP_SetCurIndicatorVal(UCHAR mask);
FUNC_EXPORT UCHAR AGAP_SetSpkVol(UCHAR spk_vol);
FUNC_EXPORT void  AGAP_VoiceTagPhoneNumRsp(void *phone_num, WORD len);
FUNC_EXPORT UCHAR AGAP_AudioConnTrans(void);
FUNC_EXPORT UCHAR AGAP_ExtendCmd(void *cmd);
FUNC_EXPORT UCHAR AGAP_GetStatus(WORD mask);

#endif
