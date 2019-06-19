/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    hep_ui.h
Abstract:
	This module declares all the constant and API functions exposed to headset
	application.
Author:
    Song Bin, Zhang Yanming
Revision History:
    2002.6.19
---------------------------------------------------------------------------*/

#ifndef HEP_UI_H
#define HEP_UI_H

struct HEP_InfoStru 
{
	UCHAR vport;	/*virtual port*/
	DWORD svc_hdl;	/*service handle*/
	UCHAR svr_chnl;	/*the server channel of headset service on local device*/
};

#ifdef CONFIG_HEP_HS
struct HEP_HS_AttribStru
{
 	UCHAR mic_vol;	/*microphone volume*/
	UCHAR spk_vol;	/*speaker volume*/
};
#endif

extern UCHAR AT_CMD_OK[7];
extern UCHAR AT_CMD_ERROR[10];
extern UCHAR AT_CMD_RING[9];
extern UCHAR AT_CMD_CKPD[13];

DWORD HEP_Init(void);
void HEP_Done(void);
const CHAR* HEP_GetVersion(void);
BOOL HEP_WriteData(UCHAR vport, UCHAR *buf, WORD len);

#ifdef CONFIG_HEP_AG
UCHAR *HEP_AG_ClntStart(UCHAR* bd, UCHAR * sdp_attrib, UCHAR * local_attrib); 
UCHAR HEP_AG_ClntStop(UCHAR * handle); 
UCHAR *HEP_AG_SvrStart(UCHAR * local_attrib); 
UCHAR HEP_AG_SvrStop(UCHAR * handle);
UCHAR HEP_AG_SendAlert(UCHAR * handle);
UCHAR HEP_AG_RelScoConn(void);
UCHAR HEP_AG_EstScoConn(void);
void HEP_AG_ConnTransfer(void);
void HeadsetAGAPP_RegCbk(UCHAR *hdl,UCHAR *pfunc);
/*--------------------------------------MMI COMMAND-------------------------------------*/
UCHAR HEP_AG_MMIAdjMicVol(UCHAR * handle, UCHAR volume);
UCHAR HEP_AG_MMIAdjSpkVol(UCHAR * handle, UCHAR volume);
/*-----------------------------------CALL BACK REGISTER---------------------------------*/
typedef void (*HEP_AG_Callback)(WORD event, UCHAR* param );
void HEP_AG_CallbackRegister(HEP_AG_Callback pProc);
/*------------------------------------PORTING FUNCTION----------------------------------*/
void HEP_AG_PickUp(void);
void HEP_AG_HangUp(void);
void HEP_AG_ActiveVoiceRecognition(void);
void HEP_AG_DisableLocalAudioDevice(void);
void HEP_AG_EnableLocalAudioDevice(void);
#endif

#ifdef CONFIG_HEP_HS
UCHAR *HEP_HS_ClntStart(UCHAR* bd, UCHAR * sdp_attrib, UCHAR * local_attrib); 
UCHAR HEP_HS_ClntStop(UCHAR * handle); 
UCHAR *HEP_HS_SvrStart(UCHAR * local_attrib); 
UCHAR HEP_HS_SvrStop(UCHAR * handle);
UCHAR HEP_HS_EstScoConn(UCHAR * handle);
UCHAR HEP_HS_RelScoConn(UCHAR * handle);
/*--------------------------------------MMI COMMAND-------------------------------------*/
UCHAR HEP_HS_MMIAdjMicVol(UCHAR * handle, UCHAR volume);
UCHAR HEP_HS_MMIAdjSpkVol(UCHAR * handle, UCHAR volume);
/*-----------------------------------CALL BACK REGISTER---------------------------------*/
typedef void (*HEP_HS_Callback)(WORD event, UCHAR* param );
void HEP_HS_CallbackRegister(HEP_HS_Callback pProc);
/*------------------------------------PORTING FUNCTION----------------------------------*/
void HEP_HS_SetSpeakerVol(int m_spkvol);
void HEP_HS_SetMicrophoneVol(int m_micvol);
void HEP_HS_AlertUserRingComing(void);
void HEP_HS_EnableLocalAudioDevice(void);
void HEP_HS_DisableLocalAudioDevice(void);
#endif

/*------CALL BACK EVENT------*/
#ifdef CONFIG_HEP_AG
#define HEP_EVENT_AG_CKPD			0X01
#define HEP_EVENT_AG_SCOEST_SUCC	0X02
#define HEP_EVENT_AG_SCOEST_FAIL	0X03
#define HEP_EVENT_AG_SCOREL_SUCC	0X04
#define HEP_EVENT_AG_SCOREL_FAIL	0X05
#define HEP_EVENT_AG_MIC_VOL		0X06
#define HEP_EVENT_AG_SPK_VOL		0X07
#define HEP_EVENT_AG_CMD_OK			0X08
#define HEP_EVENT_AG_CMD_ERROR		0X09
#define HEP_EVENT_AG_ACL_DISC		0X10
#define HEP_EVENT_AG_ACL_CONN       0X11
#define HEP_EVENT_AG_LINKLOST		0X12
#endif

#ifdef CONFIG_HEP_HS
#define HEP_EVENT_HS_RING		0X20
#define HEP_EVENT_HS_RESET		0X21
#define HEP_EVENT_HS_SCOEST		0X22
#define HEP_EVENT_HS_SCOREL		0X23
#define HEP_EVENT_HS_MIC_VOL	0X24
#define HEP_EVENT_HS_SPK_VOL	0X25
#define HEP_EVENT_HS_ACL_CONN   0X26
#define HEP_EVENT_HS_LINKLOST	0X27
#endif

#endif
