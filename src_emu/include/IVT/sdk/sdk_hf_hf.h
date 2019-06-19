/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    sdk_hf_ag.h
Abstract:
	This module declares all the constants and functions used by the Hands-free
	AG application,	which is defined in module ag_hfap.c
---------------------------------------------------------------------------*/
#ifndef _BTSDK_HF_HF_H
#define _BTSDK_HF_HF_H


/* When macros in hf_hfap.h is changed, we shall update these macors also. */
/* AG state*/
#define BTSDK_HFAP_ST_IDLE                          0x01 /*conn not established, for HSP & HFP*/
#define BTSDK_HFAP_ST_STANDBY                       0x02 /*conn established, for HSP & HFP*/
#define BTSDK_HFAP_ST_RINGING                       0x03 /*ringing, for HSP & HFP*/
#define BTSDK_HFAP_ST_OUTGOINGCALL                  0x04 /*outgoing call, only for HFP*/
#define BTSDK_HFAP_ST_ONGOINGCALL                   0x05 /*ongoing call, only for HFP and HSP*/

/* Available staus from function Btsdk_HFAP_GetStatus. */
#define BTSDK_HFAP_STATUS_LOCAL_GENERATE_RINGTONG   0x01  /* whether HF device need to generate its own in-band ring tone */
#define BTSDK_HFAP_STATUS_AUDIO_CONN_ONGOING        0x02  /* whether audio connection with remote device is ongoing */

/* When macros in hfp_ui.h is changed, we shall update these macors also. */
/* Local supported features, defined in spec. */
#define BTSDK_HFAP_FEA_NREC                         0x00000001
#define BTSDK_HFAP_FEA_3WAY_CALLING                 0x00000002
#define BTSDK_HFAP_FEA_CALLING_LINE_NUM             0x00000004
#define BTSDK_HFAP_FEA_VOICE_RECOG                  0x00000008
#define BTSDK_HFAP_FEA_RMT_VOL_CTRL                 0x00000010

/* Three way calling mode */
#define BTSDK_HFAP_3WAY_MOD0                        '0' /*Set busy tone for a waiting call; Release the held call*/
#define BTSDK_HFAP_3WAY_MOD1                        '1' /*Release activate call & accept held/waiting call*/
#define BTSDK_HFAP_3WAY_MOD2                        '2' /*Swap between active call and held call; Place active call on held; Place held call on active*/
#define BTSDK_HFAP_3WAY_MOD3                        '3' /*Add a held call to the conversation*/
#define BTSDK_HFAP_3WAY_MOD4                        '4' /*Connects the two calls and disconnects the subscriber from both calls (Explicit Call Transfer)*/

/* When macros in gap_ui.h is changed, we shall update these macors also. */
/* SCO packet type for hci1.1, used by function Btsdk_AGAP_AGInit. */
#define BTSDK_HFAP_SCO_PKT_HV1				0x20
#define BTSDK_HFAP_SCO_PKT_HV2				0x40
#define BTSDK_HFAP_SCO_PKT_HV3				0x80


/* Used in the BTSDK_APP_EV_HFAP_AG_AVAILABLE_IND event */
struct Btsdk_HFAP_ConnInfo {
    BTUINT8 is_hsp_ag;    /*whether hsp AG is connected*/
	BTCONNHDL hdl;	   /*Connection handle to HFP/HEP AG.*/
};


/* Hands-free Audio gateway callback function type */
typedef void (Btsdk_HFAP_Event_Ind_Func)(BTUINT16 msgid, BTUINT8 *pArg, BTUINT32 dwArg);

/* User interface */
FUNC_EXPORT BTUINT32 Btsdk_HFAP_APPRegCbk(Btsdk_HFAP_Event_Ind_Func *pfunc);
FUNC_EXPORT BTUINT32 Btsdk_HFAP_Init(BTUINT32 features, BTUINT8 sco_pkt_type, const BTUINT16 **call_state);
FUNC_EXPORT BTUINT32 Btsdk_HFAP_Done(void);

FUNC_EXPORT BTUINT32 Btsdk_HFAP_AnswerCall(void);
FUNC_EXPORT BTUINT32 Btsdk_HFAP_CancelCall(void);
FUNC_EXPORT BTUINT32 Btsdk_HFAP_LastNumRedial(void);
FUNC_EXPORT BTUINT32 Btsdk_HFAP_MemNumDial(void *mem_location, BTUINT16 len);
FUNC_EXPORT BTUINT32 Btsdk_HFAP_Dial(void *phone_num, BTUINT16 len);

FUNC_EXPORT BTUINT32 Btsdk_HFAP_VoiceRecognitionReq(void);
FUNC_EXPORT BTUINT32 Btsdk_HFAP_3WayCallingHandler(BTUINT8 op_code);
FUNC_EXPORT BTUINT32 Btsdk_HFAP_AudioConnTrans(void);
FUNC_EXPORT BTUINT32 Btsdk_HFAP_TxDTMF(BTUINT8 chr);
FUNC_EXPORT BTUINT32 Btsdk_HFAP_SetSpkVol(BTUINT8 spk_vol);
FUNC_EXPORT BTUINT32 Btsdk_HFAP_VoiceTagPhoneNumReq(void);
FUNC_EXPORT BTUINT32 Btsdk_HFAP_TerminateRepConn(void);
FUNC_EXPORT BTUINT32 Btsdk_HFAP_ExtendCmd(void *param);
FUNC_EXPORT BTBOOL Btsdk_HFAP_GetStatus(BTUINT16 mask);
FUNC_EXPORT BTUINT16 Btsdk_HFAP_GetCurrHFState(void);

FUNC_EXPORT BTSVCHDL Btsdk_RegisterHFPHFService(void);
FUNC_EXPORT BTUINT32 Btsdk_UnregisterHFPHFService(void);
FUNC_EXPORT BTSVCHDL Btsdk_RegisterHSPHSService(void);
FUNC_EXPORT BTUINT32 Btsdk_UnregisterHSPHSService(void);

#endif
