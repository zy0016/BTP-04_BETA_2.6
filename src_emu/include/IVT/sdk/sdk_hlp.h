/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:	sdk_hlp.h 
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/


#ifndef _SDK_HLP_H
#define _SDK_HLP_H

#include "g_sdk.h"

#define BTSDK_MSG_DONE					0x00
#define BTSDK_MSG_PIN_CODE_REQ			0x01
#define BTSDK_MSG_LINK_KEY_REQ			0x02
#define BTSDK_MSG_LINK_KEY_NOTIF		0x03
#define BTSDK_MSG_DISCOVERY_RESULT		0x04
#define BTSDK_MSG_DISCOVERY_END			0x05
#define BTSDK_MSG_AUTHOR_REQ			0x06
#define BTSDK_MSG_CONN_COMPLETE_IND		0x07
#define BTSDK_MSG_SHORTCUT				0x08
#define BTSDK_MSG_AUTHENTICATION_FAIL	0x09
#define BTSDK_MSG_RMT_SVC_LIST_UPDATED	0x0A

#define BTSDK_MSG_PROFEV_BASE			0xA0
#define BTSDK_MSG_PROFEV_CONN			(BTSDK_MSG_PROFEV_BASE + BTAPP_EV_CONN)
#define BTSDK_MSG_PROFEV_DISC			(BTSDK_MSG_PROFEV_BASE + BTAPP_EV_DISC)
#define BTSDK_MSG_PROFEV_START			(BTSDK_MSG_PROFEV_BASE + BTAPP_EV_START)
#define BTSDK_MSG_PROFEV_STOP			(BTSDK_MSG_PROFEV_BASE + BTAPP_EV_STOP)
#define BTSDK_MSG_PROFEV_PREV_DISC		(BTSDK_MSG_PROFEV_BASE + BTAPP_EV_PREV_DISC)
#define BTSDK_MSG_PROFEV_SEND			(BTSDK_MSG_PROFEV_BASE + BTAPP_EV_SEND)
#define BTSDK_MSG_PROFEV_RECEIVE		(BTSDK_MSG_PROFEV_BASE + BTAPP_EV_RECEIVE)

struct BtSdkMsg {
	BTUINT16 id;
	BTUINT8  *pArg;
	BTUINT32 dwArg;
};

typedef struct _FTP_FINDHDL
{
	BTUINT32 file_hdl;   /*The XML file(to browse the remote folder) handle*/	
	BTUINT32 data_hdl;   /*The parse data handle*/	
} FTP_FINDHDL, *PFTP_FINDHDL;

void InitSdkMsgThread(void);
void SendSdkMsg(BTUINT16 msgid, BTUINT8 *pArg, BTUINT32 dwArg);
struct BtSdkMsg * GetSdkMsg(void);
void DoneSdkMsgThread(void);
void SdkMsgThread(void);

/* used by sdk_init.c, sdk_rmt.c and sdk_loc.c */
BTDEVHDL DevPnt2DevHdl(PRemoteDeviceStru rmt_ptr);
PRemoteDeviceStru hlp_FindRemoteDeviceFromList(BTUINT8 *bd_addr);
PRemoteDeviceStru hlp_AddRemoteDeviceToList(BTUINT8 *bd_addr, BTUINT8 *pdevice_class);
void EncodeDeviceClass(BTUINT32 *des, BTUINT8 *src);
void DecodeDeviceClass(BTUINT8 *des, BTUINT32 *src);
void Btsdk_PrintErrorMessage(BTUINT32 err_code, BTUINT8 is_prt);
void RemoveTrustedList(BTUINT8 *bd_addr);
void ReleaseConnectionList(void);
void ReleaseLocalServiceList(void);
void ReleaseRemoteServiceList(PRemoteDeviceStru rmt_dev);
void ReleaseRemoteDeviceList(void);
void ReleaseShortCutList(void);
void EncodeDeviceClass(BTUINT32 *des, BTUINT8 *src);
void DecodeDeviceClass(BTUINT8 *des, BTUINT32 *src);
BTINT32 hlp_SetLinkMode(BTUINT8* bd_addr, BTUINT8 link_mode, BTUINT8* param);

/* used by sdk_svc.c and sdk_init.c */
BTUINT16 Svc_DecideProtocolID(BTUINT16 svc_class);
BTLPVOID Svc_CreateLocExtAttr(BTUINT16 svc_class, BTLPVOID ext_attr_src);
BTUINT32 Svc_GetLocalServiceListByType(BTUINT32 service_class, BTSVCHDL *psvc_hdl, BTUINT32 max_svc_num);
BTSVCHDL Svc_AddAndSaveServer(BtSdkLocalServerAttrStru *attribute);
BTUINT32 Svc_StartServer(PLocalServiceStru svc_record);
BTINT32 Svc_StopServer(PLocalServiceStru svc_record);
void Svc_RemoveAllRemoteSvcListItems(struct BtList *svc_list);
PRemoteServiceStru Svc_FindRemoteServiceRecord(BTSVCHDL svc_hdl, PRemoteDeviceStru *rmt_dev);
BTINT32 Svc_BrowseRemoteService(PRemoteDeviceStru rmt_dev, BTUINT32 ptn_num, 
								PBtSdkSDPSearchPatternStru sch_ptn, BTBOOL del_old);
BTUINT32 Svc_RetrieveStrAttrValue(PRemoteServiceStru rmt_svc, BTUINT16 attr_id, BTINT8 *buf, BTUINT32 len);
BTSVCHDL Svc_FindRmtSvcRecordByNameEx(struct BtList *svc_list, BTUINT16 svc_class, BTUINT8 *src_name);

/* Connection related help functions */
typedef HDLTYPE (*Profile_ClntStart)(BTUINT8 *bd, BTUINT8 *sdap_attr, BTUINT8 *loc_attr);

BTINT32 hlp_FindConnBySvcHandle(BTSVCHDL svc_hdl, BTCONNHDL *conn_hdls, BTUINT32 conn_num);
BTINT32 hlp_FindConnByDevHandle(BTDEVHDL dev_hdl, BTCONNHDL *conn_hdls, BTUINT32 conn_num);
PConnectionManagerStru hlp_GetConnectionByServiceClass(BTDEVHDL dev_hdl, BTUINT16 svc_class);

BTINT32 Conn_Connect2Server(Profile_ClntStart start_func, PConnectionManagerStru conn_obj, 
						    BTUINT8 *loc_attr);
BTINT32 Conn_StartClient(PRemoteDeviceStru rmt_dev, PRemoteServiceStru rmt_svc, BTUINT32 lParam, 
						 BTCONNHDL *conn_hdl);
PRemoteServiceStru Conn_SelectServiceRecord2Connect(PRemoteDeviceStru rmt_dev, BTUINT16 service_class, 
													BTCONNHDL *conn_hdl);
void Conn_ServiceConnectedEvent(BTCONNHDL conn_hdl, struct AppEvConn *param, BTUINT16 svr_flag);
void Conn_ServiceDisconnectedEvent(BTCONNHDL conn_hdl, BTUINT8 *param, BTUINT16 svr_flag);
void Conn_DataTransceivedEvent(BTBOOL is_sent, BTCONNHDL conn_hdl, BTUINT32 bytes_transceived);
BTINT32 Conn_StopClient(PConnectionManagerStru cur_conn);
BTINT32 Conn_ActivateACLLink(BTCONNHDL conn_hdl);

/* Security related help functions. */
void Sec_SetTrustedDevice(PLocalServiceStru svc, BTUINT8* bd, BTBOOL bIsTrusted);

/* Shortcut related help functions. */
BTUINT32 Shc_RemoteServiceListUpdated(BTDEVHDL dev_hdl);

/* SPP help functions. */
#ifdef CONFIG_SPP_SERVER
void Spp_CopyLocExtAttr(PBtSdkLocalSPPServerAttrStru ext_attr_src, 
						PBtSdkLocalSPPServerAttrStru ext_attr_dest, BOOL use_default);
BTUINT32 Spp_StartService(PLocalServiceStru spp_svc, BTUINT32 *sdp_record_hdl);
#else
#define Spp_CopyLocExtAttr(ext_attr_src, ext_attr_dest, use_default)
#define Spp_StartService(spp_svc, sdp_record_hdl) BTSDK_ER_FUNCTION_NOTSUPPORT
#endif

#ifdef CONFIG_SPP_CLIENT
BTINT32 Spp_StartClient(PConnectionManagerStru conn_obj);
#else
#define Spp_StartClient(conn_obj) BTSDK_ER_FUNCTION_NOTSUPPORT
#endif

/* FTP help functions. */
#ifdef CONFIG_FTP_SERVER
void Ftp_CopyLocExtAttr(PBtSdkLocalFTPServerAttrStru ext_attr_src, 
						PBtSdkLocalFTPServerAttrStru ext_attr_dest, BOOL use_default);
BTUINT32 Ftp_StartService(PLocalServiceStru ftp_svc, BTUINT32 *sdp_record_hdl);
#else
#define Ftp_CopyLocExtAttr(ext_attr_src, ext_attr_dest, use_default)
#define Ftp_StartService(ftp_svc, sdp_record_hdl) BTSDK_ER_FUNCTION_NOTSUPPORT
#endif

#ifdef CONFIG_FTP_CLIENT
BTINT32 Ftp_StartClient(PConnectionManagerStru conn_obj);
BTINT32 hlp_FTPGetLastErr(BTCONNHDL conn_hdl);
#else
#define Ftp_StartClient(conn_obj) BTSDK_ER_FUNCTION_NOTSUPPORT
#define hlp_FTPGetLastErr(conn_hdl) BTSDK_ER_FUNCTION_NOTSUPPORT
#endif

#ifdef CONFIG_FTP
FTP_FINDHDL *FTP_BrowseObjectDeal(FTPHDL ftp_hdl,FTP_FIND_DATA *data);
void Ftp_ProfileEventHandler(BTUINT16 event, BTUINT8 *hdl, BTUINT8 *param);
#endif

/* OPP help functions. */
#ifdef CONFIG_OPP
void Opp_ProfileEventHandler(BTUINT16 event, BTUINT8 *hdl, BTUINT8 *param);
#endif

#ifdef CONFIG_OPP_SERVER
void Opp_CopyLocExtAttr(PBtSdkLocalOPPServerAttrStru ext_attr_src, 
						PBtSdkLocalOPPServerAttrStru ext_attr_dest, BOOL use_default);
BTUINT32 Opp_StartService(PLocalServiceStru opp_svc, BTUINT32 *sdp_record_hdl);
#else
#define Opp_CopyLocExtAttr(ext_attr_src, ext_attr_dest, use_default)
#define Opp_StartService(opp_svc, sdp_record_hdl)  BTSDK_ER_FUNCTION_NOTSUPPORT
#endif

#ifdef CONFIG_OPP_CLIENT
BTINT32 Opp_StartClient(PConnectionManagerStru conn_obj);
BTINT32 Opp_StopClient(PConnectionManagerStru conn_obj);
BTINT32 hlp_OPPGetLastErr(BTCONNHDL conn_hdl);
#else
#define Opp_StartClient(conn_obj)  BTSDK_ER_FUNCTION_NOTSUPPORT
#define Opp_StopClient(conn_obj)  BTSDK_ER_FUNCTION_NOTSUPPORT
#define hlp_OPPGetLastErr(conn_hdl) BTSDK_ER_FUNCTION_NOTSUPPORT
#endif

/* CTP help functions. */
void Ctpicp_SdkCallback(BTUINT16 msgid, BTUINT8 *pArg,  BTUINT32 dwArg);
BTUINT32 Ctp_JoinGateway(PConnectionManagerStru conn_obj);
BTUINT32 Ctp_LeaveGateway(PConnectionManagerStru conn_obj);

/* Intercom help functions */
#ifdef CONFIG_PHONE_TL
BTUINT32 Icp_StartService(PLocalServiceStru icp_svc, BTUINT32 *sdp_record_hdl);
BTUINT32 Icp_IcpDial(PConnectionManagerStru conn_obj);
BTUINT32 Icp_IcpHangup(PConnectionManagerStru conn_obj);
#else
#define Icp_StartService(icp_svc, sdp_record_hdl) BTSDK_ER_FUNCTION_NOTSUPPORT
#define Icp_IcpDial(conn_obj) BTSDK_ER_FUNCTION_NOTSUPPORT
#define Icp_IcpHangup(conn_obj) BTSDK_ER_FUNCTION_NOTSUPPORT
#endif

/* HSP/HFP help functions */
void Btsdk_HFAPCallback(BTUINT16 msgid, BTUINT8 *pArg,  BTUINT32 dwArg);
BTUINT32 HF_ConnWithAG(PConnectionManagerStru conn_obj);
BTUINT32 HF_DiscWithAG(PConnectionManagerStru conn_obj);
BTUINT32 Hfp_Hf_StartService(PLocalServiceStru hf_svc, BTUINT32 *sdp_record_hdl);
BTUINT32 HS_ConnWithAG(PConnectionManagerStru conn_obj);
BTUINT32 HS_DiscWithAG(PConnectionManagerStru conn_obj);
BTUINT32 Hsp_Hs_StartService(PLocalServiceStru hf_svc, BTUINT32 *sdp_record_hdl);

/* HSP/HFP AG help functions */
void Btsdk_AGAPCallback(BTUINT16 msgid, BTUINT8 *pArg,  BTUINT32 dwArg);
BTUINT32 AG_ConnWithHF(PConnectionManagerStru conn_obj);
BTUINT32 AG_DiscWithHF(PConnectionManagerStru conn_obj);
BTUINT32 Hfp_Ag_StartService(PLocalServiceStru ag_svc, BTUINT32 *sdp_record_hdl);
BTUINT32 Hsp_Ag_StartService(PLocalServiceStru ag_svc, BTUINT32 *sdp_record_hdl);

#ifdef CONFIG_DUN
BTUINT32 Btsdk_DunInit(void);
BTUINT32 Btsdk_DunDone(void);
#else
#define Btsdk_DunInit()
#define Btsdk_DunDone()
#endif

#ifdef CONFIG_DUN_SERVER
BTINT32 Dun_StartService(PLocalServiceStru dun_svc, BTUINT32 *dun_record_hdl);
BTINT32 Dun_StopService(BTUINT32 *dun_record_hdl);
void Btsdk_DunCallback(BTUINT8 msgid, BTUINT8 *param, BTUINT16 param_len);
#else
#define Dun_StartService(dun_svc, dun_record_hdl) BTSDK_ER_FUNCTION_NOTSUPPORT
#define Dun_StopService(dun_record_hdl) BTSDK_ER_FUNCTION_NOTSUPPORT 
#define  Btsdk_DunCallback(msgid, param, param_len) 
#endif

#ifdef CONFIG_DUN_CLIENT
BTINT32 Dun_StartClient(PConnectionManagerStru conn_obj);
BTINT32 Dun_StopClient(PConnectionManagerStru conn_obj);
#else
#define Dun_StartClient(conn_obj) BTSDK_ER_FUNCTION_NOTSUPPORT
#define Dun_StopClient(conn_obj) BTSDK_ER_FUNCTION_NOTSUPPORT
#endif

/* FAX function */
#ifdef CONFIG_FAX_CLIENT
BTINT32 Fax_StartClient(PConnectionManagerStru conn_obj);
BTINT32 Fax_StopClient(PConnectionManagerStru conn_obj); 
#else
#define Fax_StartClient(conn_obj) BTSDK_ER_FUNCTION_NOTSUPPORT
#define Fax_StopClient (conn_obj) BTSDK_ER_FUNCTION_NOTSUPPORT
#endif

#ifdef CONFIG_FAX_SERVER
BTINT32 Fax_StartService(PLocalServiceStru fax_svc, BTUINT32 *fax_record_hdl);
#else
#define Fax_StartService(fax_svc, fax_record_hdl) BTSDK_ER_FUNCTION_NOTSUPPORT
#endif

/* HCRPP function */
BTINT32 HCRPP_StartService(PLocalServiceStru hcrpp_svc, BTUINT32 *sdp_record_handle);
BTINT32 HCRPP_StopService(BTUINT32* real_hdl);

BTINT32 HCRPP_StartClient(PConnectionManagerStru conn_obj);
BTINT32 HCRPP_StopClient(PConnectionManagerStru conn_obj);

#ifdef CONFIG_LAP_CLIENT
BTINT32 Lap_StartClient(PConnectionManagerStru conn_obj);
BTINT32 Lap_StopClient(PConnectionManagerStru conn_obj);
#else
#define Lap_StartClient(conn_obj) BTSDK_ER_FUNCTION_NOTSUPPORT
#define Lap_StopClient(conn_obj) BTSDK_ER_FUNCTION_NOTSUPPORT
#endif

#ifdef CONFIG_PAN
void Pan_CopyLocExtAttr(PBtSdkLocalGNServerAttrStru ext_attr_src,
						PBtSdkLocalGNServerAttrStru ext_attr_dest, BOOL use_default);
BTUINT32 Pan_StartService(PLocalServiceStru pan_svc, BTUINT32 *sdp_record_hdl);
void Pan_StartPausedService(void);
BTINT32 Pan_StartClient(PConnectionManagerStru conn_obj);
BTINT32 Pan_StopClient(PConnectionManagerStru conn_obj);
#else
#define Pan_CopyLocExtAttr(ext_attr_src, ext_attr_dest, use_default) 
#define Pan_StartService(pan_svc, sdp_record_hdl) BTSDK_ER_FUNCTION_NOTSUPPORT
#define Pan_StartPausedService	  BTSDK_ER_FUNCTTION_NOTSUPPORT
#define Pan_StartClient(conn_obj) BTSDK_ER_FUNCTION_NOTSUPPORT
#define Pan_StopClient(conn_obj) BTSDK_ER_FUNCTION_NOTSUPPORT
#endif

/* AV help functions. */
BTUINT32 A2DPSDK_StartSRCService(PLocalServiceStru a2dp_svc, BTUINT32 *sdp_record_hdl);
BTUINT32 A2DPSDK_StopSRCService(BTUINT32* real_hdl);
BTINT32 A2DPSDK_StartClient(PConnectionManagerStru conn_obj);
BTINT32 A2DPSDK_StopClient(PConnectionManagerStru conn_obj);
void A2DPSDK_CopyLocExtAttr(PBtSdkLocalA2DPServerAttrStru ext_attr_src, PBtSdkLocalA2DPServerAttrStru ext_attr_dest, BOOL use_default);

/* Porting help functions. */
BTINT32 Port_RenamePrivateProfileSection(BTUINT8 *old_sec, BTUINT8 *new_sec, const char *name);
BTINT32 Port_GetPrivateProfileString(const char *sec, const char *key,
									 const char *defval, char *val, BTUINT32 size, const char *name);
BTINT32 Port_GetPrivateProfileStringEx(const char *sec, const char *key,
									   const char *defval, char *val, BTUINT32 size, const char *name);
BTINT32 Port_WritePrivateProfileString(const char *sec, const char *key,
									   const char *val, const char *name);
BTINT32 Port_WritePrivateProfileStringEx(const char *sec, const char *key,
										 const char *val, const char *name);
BTINT32 Port_DeletePrivateProfileStringSec(const char *sec, const char *name);
BTINT32 Port_DeletePrivateProfileStringSecEx(const char *sec, const char *name);
BTUINT32 Port_GetPrivateProfileInt(const char *sec, const char *key, BTUINT32 defval,
									const char *name);

/* event callback */
void hlp_InqResultCBK(BTUINT8 *bd_addr, BTUINT8 *dev_class);
void hlp_PinCodeReqCBK(BTUINT8 *bd_addr, BTUINT32 delay);
void hlp_LinkKeyNotifyCBK(BTUINT8 *bd_addr, BTUINT8 *link_key);
void hlp_LinkKeyReqCBK(BTUINT8* bd_addr);
BTUINT8 hlp_ConnReqCBK(BTUINT8* bd_addr, BTUINT8 *dev_class, BTUINT8 link_type);
BTUINT8 hlp_AclConnCompCBK(BTUINT8 *bd_addr);
void hlp_ProfileEventCBK(BTUINT16 event, BTUINT8 *hdl, BTUINT8 * param);
BTUINT32 Btsdk_AuthorReqInd(PAuthorReqIndStru);
void hlp_AuthorReqInd(BTUINT8 *bd_addr, BTUINT8 proto, BTUINT16 channel);
void hlp_GAP_Err_Ind(BTUINT8 code);
void hlp_AuthenFailInd(BTUINT8* bd_addr);
void hlp_HandleConnectionEvent(BTUINT16 event, PConnectionManagerStru cur_conn, BTBOOL bCreateInd);

#endif
