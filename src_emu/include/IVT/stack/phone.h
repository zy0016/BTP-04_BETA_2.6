#ifndef PHONE_H
#define PHONE_H
#include "global.h"
#include "tcsui.h"
#include "phone_ui.h"
#include "sdap_ui.h"

#define TCS_TL					0x700
#define TCS_GW					0x701
#define INVALID_SCO_HANDLE		0

#define ROLE_GW_OUT			1
#define ROLE_GW_IN			2
#define ROLE_TL_OUT			3
#define ROLE_TL_IN			4
#define ROLE_GW_TLOUT		5
#define ROLE_GW_TLIN		6
#define ROLE_GW_TRANS		7

#define PHONE_NULL				0

#define AUDIO_CVSD				1
#define AUDIO_PCM_A			2
#define AUDIO_PCM_U			3
#define AUDIO_HV1				5
#define AUDIO_HV2				6
#define AUDIO_HV3				7
#define AUDIO_DV				8

struct PhoneUserData {
	UCHAR *	ccid;				/* call control ID of the active link */
	WORD		sco_handle;			/* call sco-handle of th activie all */
	UCHAR       bd[BDLENGTH];		/* Remote bd address */
};

#define			HW_ST_SCO	1
#define			HW_ST_HOOK	2

struct PhoneLocaInfo {
	UCHAR		role;				/* Role in the phone call */
	UCHAR		hw;					/* Hardware status */
	UCHAR		ccls;				/* call class of the active call */
	UCHAR		DTMF_key;                /* used by GW to send Stop DTMF Acknowledge */
	UCHAR		bd[BDLENGTH];		/* Bluetooth address used as transfer original */				
	struct TcsCallInfoStru info;	/* Call infomation */
};

#define MMI_SetupInd(handle, setup_ind)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_MMI_SetupInd, handle, 0, (DWORD)setup_ind); \
}

/* added by wangbin on Mar.12,2004, in order to get the outgoing call's ccid */
#define MMI_CcidInd(handle)\
{ \
        if (g_eventProc != NULL)\
                (*g_eventProc)(EV_MMI_CcidInd, handle, 0, 0); \
}

#define MMI_SetupCompleteInd(handle)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_MMI_SetupCompleteInd, handle, 0, 0);\
}

#define MMI_DisconnInd(handle, cause, transf)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_MMI_DisconnInd, handle, (WORD)cause, transf);\
}

#define MMI_LinklossInd(bd_addr)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_MMI_LinklossInd, 0, 0, (DWORD)bd_addr);\
}

#define MMI_LinkConnectInd(handle, bd_addr)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_MMI_LinkConnectInd, handle, 0, (DWORD)bd_addr);\
}
	
#define MMI_DigitInd(handle, digit)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_MMI_DigitInd, handle, (WORD)digit, 0);\
}

#define MMI_AccessRightsInd(bd_addr, company_spec)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_MMI_AccessRightsInd, bd_addr, 0, (DWORD)company_spec);\
}

#define MMI_AccessRightsCfm(flag, company_spec)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_MMI_AccessRightsCfm, 0, (WORD)flag, (DWORD)company_spec);\
}

#define MMI_FastAccessInd(company_spec)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_MMI_FastAccessInd, 0, 0, (DWORD)company_spec);\
}

#define MMI_FastAccessCfm(flag, cause, clock_off, company_spec)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_MMI_FastAccessCfm, clock_off, (WORD)(((WORD)cause) << 8 | flag), (DWORD)company_spec);\
}

#define MMI_InfoSuggestInd(num, bd_array, company_spec)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_MMI_InfoSuggestInd, bd_array, num, (DWORD)company_spec);\
}

#define MMI_ExitPageScanInd()\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_MMI_ExitPageScanInd, 0, 0, 0);\
}

#define MMI_InfoInd(handle, company_spec)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_MMI_InfoInd, handle, 0, (DWORD)company_spec);\
}


/*---------------------------------VOICE PATH CONTROLLER---------------------------------*/
/*
 Line: external line;
 SCO: SCO channel;
 Local: Local Microphone and Speakerphone
*/

#define HW_ConnSCO2Local(handle, sco_handle)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_HW_ConnSCO2Local, handle, (WORD)sco_handle, 0);\
}
		
#define HW_ConnLine2Local(handle)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_HW_ConnLine2Local, handle, 0, 0);\
}
		
#define HW_ConnLine2SCO(handle, sco_handle)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_HW_ConnLine2SCO, handle, (WORD)sco_handle, 0);\
}

#define HW_DisconnSCO(sco_handle)\
{ \
	if (g_eventProc!=NULL)\
		(*g_eventProc)(EV_HW_DisconnSCO, 0, (WORD)sco_handle, 0);\
}
		
#define HW_DisconnLine(handle)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_HW_DisconnLine, handle, 0, 0);\
}

/*---------------------------------SL COMMAND--------------------------------*/
#ifndef CONFIG_PHONE_TL
#define HW_Local_HookOn(handle)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_HW_Local_HookOn, handle, 0, 0);\
}

#define HW_Local_HookOff(handle)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_HW_Local_HookOff, handle, 0,0);\
}
	
#define HW_Local_Flash(handle)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_HW_Local_Flash, handle, 0, 0);\
}

#define HW_Local_Digit(handle, length, content)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_HW_Local_Digit, handle, (WORD)length, (DWORD)content);\
}
		
#define HW_Local_GetIncomingExCallInfo(handle, info)\
{ \
	if (g_eventProc != NULL)\
		(*g_eventProc)(EV_HW_Local_GetIncomingExCallInfo, handle, 0, (DWORD)info);\
}
#endif

struct SetupReqParamStru* Ph_CSetup(struct MMI_SetupReqStru* arg);
struct DisconnReqInStru* Ph_CDisc(UCHAR cause, UCHAR comp_len, UCHAR * comp);
struct MMI_SetupIndStru* Ph_CSetupInd(struct TcsCallInfoStru* arg);
struct PhoneLocaInfo *  Ph_SaveInfo(struct TcsCallInfoStru *s,UCHAR role, UCHAR ccls);
struct SetupReqParamStru * Ph_CSetupTrans(UCHAR * bd, struct PhoneLocaInfo *info);
struct PhoneUserData * Ph_CHdl(UCHAR *bd);

FUNC_EXPORT void Phone2TcsCallback(UCHAR event, TCS_HANDLE ccid, WORD wparam, DWORD lparam);

extern UCHAR		g_gw_addr[];
extern PHONECBK		g_eventProc;
extern const UCHAR BLANKADDR[];

#ifdef CONFIG_PHONE_TL
extern SYSTEM_LOCK g_tllock;
extern struct PhoneUserData *g_tlhdl;
#endif
#ifdef  CONFIG_PHONE_GW
extern SYSTEM_LOCK g_gwlock;
extern struct PhoneUserData *g_gwhdl;
extern struct PhoneLocaInfo *g_gwcinfo;
#endif

#define Ph_gwClearC(hdl,info,cause) \
{ \
	if (info->hw&HW_ST_SCO){ \
		HW_DisconnSCO((hdl->sco_handle));\
		info->hw&=~HW_ST_SCO;\
	} \
	if (cause==CAUSE_PHONE_CALL_TRANSFER) \
		info->role=ROLE_GW_TRANS; \
	else if (info->role==ROLE_GW_TRANS) { \
		; \
	} \
	else { \
		if (info->hw&HW_ST_HOOK) \
			HW_Local_HookOn(hdl->ccid);\
		FREE(info);\
		info=NULL;\
	}\
	FREE(hdl);\
	hdl=NULL; \
}

#endif
