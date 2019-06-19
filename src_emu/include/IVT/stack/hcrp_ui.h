/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    hcrp_ui.h
Abstract:
	This file includes the user interface of module hcrpapi
Author:
    Jin Lihong
Revision History: 2002.2
---------------------------------------------------------------------------*/

#ifndef HCRP_UI_H
#define HCRP_UI_H

#define HCRP_UUID		    0x0012
#define HCRP_PRT_CCH_PSM	  4097
#define HCRP_PRT_DCH_PSM	  4099
#define HCRP_NOTIF_PSM		  4095
#define HCRP_OFFLINE             0
#define HCRP_BONDING             1
#define HCRP_PRIVATE_ONLINE      2
#define HCRP_PUBLIC_ONLINE       3
#define FEATURE_UNSUPPORTED   0x02

#define SERVICE_HANDLE	0x01000000

struct HCRPP_InfoStru {
	DWORD   hport;
	UCHAR * id_1284; 
	WORD	cpsm;
	WORD	dpsm;
	WORD    mtu;
	UCHAR   bd[6];
	UCHAR   com_index; /*Windows system's virtual serial port num*/
};

struct HCRPP_Svr{
	WORD len;
	UCHAR * svc_name;
	UCHAR * id_1284;
	UCHAR * dev_name;
	UCHAR * frd_name;
};

/*-----------------------------both side -----------------------------*/
DWORD HCRPP_Init(void);
void  HCRPP_Done(void);
const CHAR* HCRPP_GetVersion(void);
WORD  HCRPP_GetStatus(UCHAR * status);
void  HCRPPAPP_RegCbk(UCHAR * hdl, UCHAR *pfunc);
void  HCRPP_SetHcrpRole(WORD cPsm,WORD dPsm,WORD nPsm,UCHAR role);

void  HCRPP_CallbackDispatch(WORD msgid, UCHAR* param);

#ifdef CONFIG_HCRP_CLIENT
UCHAR * HCRPP_ClntStart(UCHAR * bd, UCHAR * sdp_attrib, UCHAR * local_attrib);
DWORD HCRPP_ClntStop(UCHAR * handle);

UCHAR HCRPP_Open(UCHAR* bd, WORD cPsm, WORD dPsm, DWORD* hdl);
UCHAR HCRPP_Write(DWORD hdl, UCHAR* buf, WORD length);
UCHAR HCRPP_ClntWriteEx(UCHAR *hdl, UCHAR * buf, WORD length, WORD *written);
void  HCRPP_Close(DWORD hdl);
extern void Swrap_Send2DrvFromHCRP(DWORD h,UCHAR* Buff,WORD len );

UCHAR * HCRPP_GetSvr1284ID(UCHAR * handle, UCHAR * result);
UCHAR HCRPP_GetSvrLPTStatus(UCHAR * handle,UCHAR * lptstat);
UCHAR HCRPP_SoftResetSvr(UCHAR * handle);
UCHAR HCRPP_HardResetSvr(UCHAR * handle);

#ifdef CONFIG_HCRP_NOTIF
UCHAR HCRPP_RegisterNotif(UCHAR * handle, DWORD * timeout);
#endif

#endif 

#ifdef CONFIG_HCRP_SERVER
UCHAR * HCRPP_SvrStart(UCHAR * local_attrib);
DWORD HCRPP_SvrStop(UCHAR * handle);
UCHAR HCRPP_SetSvrMode(UCHAR mode);
UCHAR HCRPP_SvrWriteEx(UCHAR *hdl, UCHAR * buf, WORD length, WORD *written);
#ifdef CONFIG_INTER_TESTER
void HCRPP_SetFeature(UCHAR mode,UCHAR status);
#endif
#ifdef CONFIG_HCRP_NOTIF
UCHAR HCRPP_Notif(void);
#endif
#endif

#endif
