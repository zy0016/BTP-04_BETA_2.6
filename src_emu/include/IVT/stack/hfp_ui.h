/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    hfp_ui.h
Abstract:
	This module declares all the constants and API functions exposed to HF application.
Author:
    Zhang Jian
Revision History:
    2003.11
---------------------------------------------------------------------------*/
#ifndef HFP_UI_H
#define HFP_UI_H

/*temp:used by AG*/
struct HFPConnInfo {
    UCHAR is_hsp;
	UCHAR is_svr;
	UCHAR call_status;
};

struct HFP_InfoStru 
{
	UCHAR vport;	/*virtual port*/
	DWORD svc_hdl;	/*service handle*/
	UCHAR svr_chnl;	/*the server channel of headset service on local device*/
};

typedef void (HFP_Callback)(UCHAR event, UCHAR* param, WORD param_len);

/*call status indicator during connection establishment*/
#define HFP_CONN_ONGOING_CALL       0x01
#define HFP_CONN_INCOMING_CALL      0x02
#define HFP_CONN_OUTGOING_CALL      0x03

/*feature mask ID for HF, defined in spec*/
#define HFP_HF_FEA_NREC             0x00000001
#define HFP_HF_FEA_3WAYCALL         0x00000002
#define HFP_HF_FEA_CLIP             0x00000004
#define HFP_HF_FEA_BVRA             0x00000008
#define HFP_HF_FEA_RMTVOLCTRL       0x00000010

/*feature mask ID for AG, defined in spec*/
#define HFP_AG_FEA_3WAYCALL         0x00000001
#define HFP_AG_FEA_NREC             0x00000002
#define HFP_AG_FEA_BVRA             0x00000004
#define HFP_AG_FEA_INBANDRING       0x00000008
#define HFP_AG_FEA_BINP             0x00000010
#define HFP_AG_FEA_NETWORK          0x00000020

/*possible conn_status of HFP handle*/
#define SPP_ONGOING                 0x0001
#define SLC_ONGOING                 0x0002
#define AC_ONGOING                  0x0004
#define SVC_AVAILABLE               0x0008   /*service = 1*/
#define CLIP_DISENABLE              0x0010   /*init value in the hfp_hdl->conn_status is 0, namely CLIP is enabled*/
#define NREC_DISENABLE              0x0020
#define CCWA_DISENABLE              0x0040    
#define BSIR_DISENABLE              0x0080
#define BVRA_ONGOING                0x0100
#define CIEV_DISENABLE              0x0200   /*it is only used in AG*/

/*
return HFP_BT_ERROR when received ERROR result code
this error code is only available for HF/HS
*/
#define HFP_BT_ERROR                3        

/*command ID defined for HF/HS app*/
#define HFCMD_ANSWERCALL            0x01     /*Answer the Incmoing Call*/
#define HFCMD_CANCELCALL            0x02     /*Reject the Incoming Call or Terminate the Outgoing Call or Release the Ongoing Call*/
#define HFCMD_CLIP                  0x03     /*Calling Line Identification Presentation*/
#define HFCMD_BINP                  0x04     /*Bluetooth Input (attach a phone number to a voice tag)*/
#define HFCMD_NREC                  0x05     /*Noise Reduction & Echo Canceling*/
#define HFCMD_CCWA                  0x06     /*call waiting notification activation*/
#define HFCMD_BVRA                  0x07     /*Voice Recognition Activation*/
#define HFCMD_SETSPKVOL             0x08     /*Set speaker volume*/
#define HFCMD_SETMICVOL             0x09     /*Set microphone volume*/
#define HFCMD_DTMF                  0x0a     /*DTMF code*/
#define HFCMD_DIAL                  0x0b     /*Dial the specific phone number*/
#define HFCMD_MEMDIAL               0x0c     /*Dial the phone number indexed by the specific memory location*/
#define HFCMD_BLDN                  0x0d     /*Redial the Last Dialed Phone Number*/
#define HFCMD_CHLD                  0x0e
#define HFCMD_CKPD                  0x0f     /*It is used for Headset: AT+CKPD=200*/
#define HFCMD_CKPD_REJ_CALL         0x10     /*It is used for Headset: AT+CKPD="e", used for reject call, it is not defined by HSP*/
#define HFCMD_EXTEND_CMD            0x11     /*Extended at-command*/
#define HFCMD_ACTRANS               0x12    /*Audio Connection Transfer towards HF/AG*/
#define HFCMD_MANUFACTURER_ID       0x13     /*get the mobile's(AG) manufactor identification*/
#define HFCMD_MODEL_ID              0x14     /*get the mobile's(AG) model identification*/

/*event ID defined for HF/HS app*/
#define HFEV_RING                   0x01
#define HFEV_CHGRINGTONE            0x02
#define HFEV_SETSPKVOL              0x03
#define HFEV_SETMICVOL              0x04
#define HFEV_CLIP                   0x05
#define HFEV_BINP                   0x06
#define HFEV_CCWA                   0x07
#define HFEV_BVRA                   0x08
#define HFEV_CIEV_CALL              0x09
#define HFEV_CIEV_CALLSETUP         0x0a
#define HFEV_CIEV_SVC               0x0b
#define HFEV_EXTEND_CMD             0x0c
#define HFEV_MANUFACTURER_ID_IND    0x0d
#define HFEV_MODEL_ID_IND           0x0e
#ifdef CONFIG_INTER_TESTER
#define HFEV_CALLSETUP_NOT_SUPP_IND	0x0f
#define HFEV_NOT_SUPPORT_BRSF	    0x10
#endif

/*command ID defined for AG/VG app*/
#define AGCMD_OK                    0x01
#define AGCMD_ERROR                 0x02
#define AGCMD_AGFEATURES            0x03
#define AGCMD_CIND_T                0x04     /*supported indicator result code*/
#define AGCMD_CIND_R                0x05     /*current indicators value result code*/
#define AGCMD_RING                  0x06     
#define AGCMD_CHGRINGTONE           0x07     /*change inband-ring tone*/
#define AGCMD_SETSPKVOL             0x08
#define AGCMD_SETMICVOL             0x09
#define AGCMD_CLIP                  0x0a     /*Calling Line Identification result code*/
#define AGCMD_BINP                  0x0b     /*phone number input result code*/
#define AGCMD_BVRA                  0x0c
#define AGCMD_CIEV                  0x0d     /*CIEV:(call, callsetup, svc) result code*/
#define AGCMD_EXTEND_CMD            0x0e
#define AGCMD_ACTRANS2HF            0x0f
#define AGCMD_ACTRANS2AG            0x10

/*event ID defined for AG/VG app*/
#define AGEV_ANSWERCALL             0x01
#define AGEV_CANCELCALL             0x02
#define AGEV_CIND_T                 0x03
#define AGEV_CIND_R                 0x04
#define AGEV_CHLD_T                 0x05
#define AGEV_CLIP                   0x06
#define AGEV_BINP                   0x07
#define AGEV_NREC                   0x08
#define AGEV_CHLD                   0x09
#define AGEV_BVRA                   0x0a
#define AGEV_SETSPKVOL              0x0b
#define AGEV_SETMICVOL              0x0c
#define AGEV_DTMF                   0x0d
#define AGEV_DIAL                   0x0e
#define AGEV_MEMDIAL                0x0f
#define AGEV_BLDN                   0x10
#define AGEV_EXTEND_CMD             0x11
#define AGEV_CKPD                   0x12
#define AGEV_CKPD_REJECT_CALL       0x13
#ifdef CONFIG_INTER_TESTER
#define AGEV_CCWA					0x14
#define AGEV_BRSF_SUPPORT           0x15
#endif

/*event ID defined for both AG and HF app*/
#define HFPEV_SCO_DISC_IND          0x41
#define HFPEV_SCO_CONN_CFM          0x42
#define HFPEV_SPP_DISC_IND          0x43
#define HFPEV_SPP_CONN_IND          0x44
#define HFPEV_SPP_LINKLOST_IND      0x45
#define HFPEV_SLC_ESTABLISHED_IND   0x46
#define HFPEV_SLC_RELEASED_IND      0x47

/*routines defined in hfp_ui.c, they are called by both AG and HF applications.*/
UCHAR HFP_Init(void);
void HFP_Done(void);
const CHAR *HFP_GetVersion(void);
FUNC_EXPORT void  HFP_APPRegCbk(HFP_Callback *pfunc);
FUNC_EXPORT UCHAR HFP_SetupAudioConn(WORD pkt_type);
FUNC_EXPORT UCHAR HFP_RlsAudioConn(void);
FUNC_EXPORT void  HFP_ASYNC_SetupAudioConn(WORD pkt_type);
FUNC_EXPORT void  HFP_ASYNC_RlsAudioConn(void);
FUNC_EXPORT UCHAR HFP_GetConnStatus(WORD mask);
FUNC_EXPORT UCHAR HFP_GetRmtFeatures(WORD mask);
FUNC_EXPORT UCHAR HFP_GetLocalFeatures(WORD mask);
FUNC_EXPORT UCHAR *HFP_GetPeerBDAddr(void);

/*routines defined in hfp_ag.c, they are only called by AG application.*/
FUNC_EXPORT	UCHAR HFP_AG_SvrStart(UCHAR *features, struct HFP_InfoStru *ag_info);
FUNC_EXPORT	void  HFP_AG_SvrStop(void);
FUNC_EXPORT	UCHAR HFP_AG_ClntStart(UCHAR *bd,UCHAR *sdp_attrib);
FUNC_EXPORT	void  HFP_AG_ClntStop(void);
FUNC_EXPORT void  HFP_AG_ReversedSDP(void);
FUNC_EXPORT UCHAR HFP_AG_CtrlEntry(UCHAR cmd, void *ag_param, WORD param_len);

/*routines defined in hfp_hf.c, they are only called by HF application.*/
FUNC_EXPORT	UCHAR HFP_HF_SvrStart(UCHAR *features, struct HFP_InfoStru *hf_info);
FUNC_EXPORT	void  HFP_HF_SvrStop(void);
FUNC_EXPORT	UCHAR HFP_HF_ClntStart(UCHAR *bd,UCHAR *sdp_attrib);
FUNC_EXPORT	void  HFP_HF_ClntStop(void);
FUNC_EXPORT UCHAR HFP_HF_SLC_Init(void);
FUNC_EXPORT UCHAR HFP_HF_CtrlEntry(UCHAR cmd, void *hf_param, WORD param_len);

/*routines defined in hfp_hs.c.*/
FUNC_EXPORT	UCHAR HSP_HS_SvrStart(struct HFP_InfoStru *hs_info);
FUNC_EXPORT	void  HSP_HS_SvrStop(void);
FUNC_EXPORT	UCHAR HSP_HS_ClntStart(UCHAR *bd, UCHAR *sdp_attrib);
FUNC_EXPORT	void  HSP_HS_ClntStop(void);

/*routines defined in hfp_vg.c.*/
FUNC_EXPORT	UCHAR HSP_VG_SvrStart(struct HFP_InfoStru *vg_info);
FUNC_EXPORT	void  HSP_VG_SvrStop(void);
FUNC_EXPORT	UCHAR HSP_VG_ClntStart(UCHAR *bd, UCHAR *sdp_attrib);
FUNC_EXPORT	void  HSP_VG_ClntStop(void);

/*routines and macros defined in hfp_spp.c.*/
typedef void (HFP_SPP_Callback)(UCHAR event, UCHAR* param, WORD param_len);

#define HFPSPPEV_ATCMD                0x01
#define HFPSPPEV_SPP_DISC_IND         0x02
#define HFPSPPEV_SPP_LINKLOST_IND     0x03

FUNC_EXPORT UCHAR HFP_SPP_Init(void);
FUNC_EXPORT void  HFP_SPP_Done(void);
FUNC_EXPORT void  HFP_SPP_APPRegCbk(HFP_SPP_Callback *pfunc);
FUNC_EXPORT	UCHAR HFP_SPP_ClntStart(UCHAR *bd);
FUNC_EXPORT	void  HFP_SPP_ClntStop(void);
FUNC_EXPORT UCHAR HFP_SPP_TxATCmd(void *at_cmd, DWORD len);

#endif
