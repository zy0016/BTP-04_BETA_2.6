/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    sdk_hf_ag.h
Abstract:
	This module declares all the constants and functions used by the Hands-free
	AG application,	which is defined in module ag_hfap.c
---------------------------------------------------------------------------*/
#ifndef SDK_HF_AG_H
#define SDK_HF_AG_H


/* When macros in Ag_hfap.h is changed, we shall update these macors also. */
/* AG state*/
#define BTSDK_AGAP_ST_IDLE          0x01		/*before service level connection is established*/
#define BTSDK_AGAP_ST_STANDBY       0x02		/*service level connection is established*/
#define BTSDK_AGAP_ST_RINGING       0x03		/*ringing*/
#define BTSDK_AGAP_ST_OUTGOINGCALL  0x04		/*outgoing call*/
#define BTSDK_AGAP_ST_ONGOINGCALL   0x05		/*ongoing call*/
#define BTSDK_AGAP_ST_BVRA          0x06        /*voice recognition is ongoing*/
#define BTSDK_AGAP_ST_VOVG          0x07        

/* Available staus from function Btsdk_AGAP_GetStatus. */
#define BTSDK_AGAP_STATUS_GENERATE_INBAND_RINGTONG   0x01  /*whether AG is capable of generating in-band ring tone*/
#define BTSDK_AGAP_STATUS_AUDIO_CONN_ONGOING         0x02  /*whether audio connection with remote device is ongoing*/

/* Current state mask code for function Btsdk_AGAP_SetCurIndicatorVal. */
#define BTSDK_AGAP_INDICATOR_SVC_UNAVAILABLE     0x00    
#define BTSDK_AGAP_INDICATOR_SVC_AVAILABLE       0x01      
#define BTSDK_AGAP_INDICATOR_ACTIVE              0x02             
#define BTSDK_AGAP_INDICATOR_INCOMING            0x04           
#define BTSDK_AGAP_INDICATOR_DIALING             0x08            
#define BTSDK_AGAP_INDICATOR_ALERTING            0x10           

/* Possible received events from network. */
#define BTSDK_AGAP_NETWORK_RMT_IS_BUSY              0x01           
#define BTSDK_AGAP_NETWORK_ALERTING_RMT             0x02          
#define BTSDK_AGAP_NETWORK_INCOMING_CALL            0x03         
#define BTSDK_AGAP_NETWORK_RMT_ANSWER_CALL          0x04       
#define BTSDK_AGAP_NETWORK_LINK_NOT_ESTABLISHED     0x05  
#define BTSDK_AGAP_NETWORK_SVC_UNAVAILABLE          0x06       
#define BTSDK_AGAP_NETWORK_SVC_AVAILABLE            0x07         

/* The initiator of the answered incoming call, parameter of the 
   BTSDK_AGAP_EV_INCOMINGCALL_ANSWERED event. */
#define BTSDK_AGAP_HF_ANSWERED_INCOMINGCALL	0x01
#define BTSDK_AGAP_AG_ANSWERED_INCOMINGCALL	0x02

/* The initiator of the canceled incoming/outgoing/ongoing call, parameter of the 
   BTSDK_AGAP_EV_CALL_CANCELED event. */
#define BTSDK_AGAP_HF_CANCELED_CALL          0x01      
#define BTSDK_AGAP_AG_CANCELED_CALL          0x02             
#define BTSDK_AGAP_NETWORK_CANCELED_CALL     0x03        
#define BTSDK_AGAP_TBUSY_CANCELED_CALL       0x04          

/* When macros in hfp_ui.h is changed, we shall update these macors also. */
/* Local supported features, defined in spec. */
#define BTSDK_AGAP_FEA_3WAY_CALLING			0x00000001      
#define BTSDK_AGAP_FEA_NREC					0x00000002              
#define BTSDK_AGAP_FEA_VOICE_RECOG			0x00000004       
#define BTSDK_AGAP_FEA_INBAND_RING			0x00000008       
#define BTSDK_AGAP_FEA_VOICETAG_PHONE_NUM	0x00000010
#define BTSDK_AGAP_FEA_REJ_CALL				0x00000020          

/* When macros in gap_ui.h is changed, we shall update these macors also. */
/* SCO packet type for hci1.1, used by function Btsdk_AGAP_AGInit. */
#define BTSDK_AGAP_SCO_PKT_HV1				0x20
#define BTSDK_AGAP_SCO_PKT_HV2				0x40
#define BTSDK_AGAP_SCO_PKT_HV3				0x80


/*used in the BTSDK_APP_EV_HFAP_HF_AVAILABLE_IND event*/
struct Btsdk_AGAP_ConnInfo {
    BTUINT8 is_hsp;    /* Whether hsp is connected. */
	BTCONNHDL hdl;      /* Handle to the remote device. */
};


/* Hands-free Audio gateway callback function type */
typedef void  (Btsdk_AGAP_Event_Ind_Func)(BTUINT16 msgid, BTUINT8 *pArg, BTUINT32 dwArg);

/*user interface*/
FUNC_EXPORT void Btsdk_AGAP_APPRegCbk(Btsdk_AGAP_Event_Ind_Func *pfunc);
FUNC_EXPORT BTUINT32 Btsdk_AGAP_Init(BTUINT32 features, BTUINT8 sco_pkt_type, const BTUINT16 **call_state);
FUNC_EXPORT BTUINT32 Btsdk_AGAP_ConnWithHF(BTUINT8 *is_hsp, BTDEVHDL dev_hdl, BTCONNHDL *pconn_hdl);
FUNC_EXPORT BTUINT32  Btsdk_AGAP_DiscWithHF(BTCONNHDL conn_hdl);
FUNC_EXPORT void  Btsdk_AGAP_Done(void);
FUNC_EXPORT BTUINT32  Btsdk_AGAP_PairWithHF(BTDEVHDL dev_hdl, BTUINT8 *is_hsp);

FUNC_EXPORT void  Btsdk_AGAP_AnswerCall(void);
FUNC_EXPORT void  Btsdk_AGAP_OriginateCall(void);
FUNC_EXPORT void  Btsdk_AGAP_CancelCall(void);
FUNC_EXPORT void  Btsdk_AGAP_NetworkEvent(BTUINT8 ev, void *param);
FUNC_EXPORT void  Btsdk_AGAP_VoiceRecogDeactivatedInd(void);

FUNC_EXPORT void  Btsdk_AGAP_SetCurIndicatorVal(BTUINT8 mask);
FUNC_EXPORT BTUINT32 Btsdk_AGAP_SetSpkVol(BTUINT8 spk_vol);
FUNC_EXPORT void  Btsdk_AGAP_VoiceTagPhoneNumRsp(void *phone_num, BTUINT16 len);
FUNC_EXPORT BTUINT32 Btsdk_AGAP_AudioConnTrans(void);
FUNC_EXPORT BTUINT32 Btsdk_AGAP_ExtendCmd(void *cmd);
FUNC_EXPORT BTUINT32 Btsdk_AGAP_GetStatus(BTUINT16 mask);

FUNC_EXPORT BTSVCHDL Btsdk_RegisterHFPAGService(void);
FUNC_EXPORT BTUINT32 Btsdk_UnregisterHFPAGService(void);
FUNC_EXPORT BTSVCHDL Btsdk_RegisterHSPAGService(void);
FUNC_EXPORT BTUINT32 Btsdk_UnregisterHSPAGService(void);

#endif
