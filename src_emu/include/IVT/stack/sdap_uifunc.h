#ifndef _BT_SDAP_UIFUNC_H
#define _BT_SDAP_UIFUNC_H


#define SDAPSVCATTRSTRULEN			9	/*sizeof(attr_id) + sizeof(align_byte) + sizeof(attr_val.size) + sizeof(attr_val.descriptor)*/
#define _SDAPSVCATTRSTRULEN(attr)		((WORD)(SDAPSVCATTRSTRULEN + ((attr)->attr_val.size ? (attr)->attr_val.size : 1) + (attr)->align_byte))

/*++++++++++++++++++++ SDAP Service Register/Unregister UI Definition ++++++++++++++++++++*/
#ifdef CONFIG_SDAP
DWORD	SDAP_Init(void);
void	SDAP_Done(void);
const char* SDAP_GetVersion(void);
UCHAR* SDAP_MallocMemory(int size);
void SDAP_FreeMemory(UCHAR *buffer);
#endif

#ifdef CONFIG_SDAP_SERVER
typedef UCHAR (BtMgrRegisterServiceCB)(WORD svc_type, struct SDAP_GeneralInfoStru *pSvcSrc);
typedef UCHAR (BtMgrUnregisterServiceCB)(DWORD svc_hdl);
void SDAP_RegiseterBtMgrRegSvcCB(BtMgrRegisterServiceCB *cb_func);
void SDAP_RegiseterBtMgrUnregSvcCB(BtMgrUnregisterServiceCB *cb_func);

WORD SDAP_RegisterAttribute(DWORD *svc_hdl,struct SDAP_SvcAttrStru *attr);
WORD SDAP_ServiceUnregister(DWORD svc_hdl);

struct SDAP_SvcAttrStru     *MakeUIntA(WORD id, UCHAR mode, const UCHAR *val);
struct SDAP_DataEleStru   	*MakeUIntDE(UCHAR mode, const UCHAR *val);
struct SDAP_SvcAttrStru  	*MakeIntA(WORD id, UCHAR mode, const UCHAR *val);
struct SDAP_DataEleStru	    *MakeIntDE(UCHAR mode, const UCHAR *val);
struct SDAP_SvcAttrStru  	*MakeUUIDA(WORD id, UCHAR mode, const struct SDAP_UUIDStru *val);
struct SDAP_DataEleStru      *MakeUUIDDE(UCHAR mode, const struct SDAP_UUIDStru *val);
struct SDAP_SvcAttrStru  	*MakeBoolA(WORD id, UCHAR mode, const UCHAR *val);
struct SDAP_DataEleStru      *MakeBoolDE(UCHAR mode, const UCHAR *val);
struct SDAP_SvcAttrStru  	*MakeStringA(WORD id, const UCHAR *src);
struct SDAP_DataEleStru      *MakeStringDE(const UCHAR *src);
struct SDAP_SvcAttrStru  	*MakeURLA(WORD id, const UCHAR *src);
struct SDAP_DataEleStru      *MakeURLDE(const UCHAR *src);
struct SDAP_SvcAttrStru    	*MakeDESeqA(WORD id, const char *szTypes,...);
struct SDAP_DataEleStru      *MakeDESeqDE(const char *szTypes,...);
struct SDAP_SvcAttrStru  	*MakeDEAltA(WORD id, const char *szTypes,...);
struct SDAP_DataEleStru      *MakeDEAltDE(const char *szTypes,...);


WORD SDAP_ModifyStringAttrib(DWORD svc_hdl, UCHAR *str_attrib, WORD attrib_id);
WORD SDAP_ModifySvcAvailAttrib(DWORD svc_hdl, UCHAR cur_clis, UCHAR max_clis);
WORD SDAP_RegisterGroup(DWORD *svc_hdl, DWORD mask, struct SDAP_UUIDStru *grp_id);

#ifdef CONFIG_TCS_GW
WORD SDAP_RegisterCTPService(UCHAR *svc_name, struct SDAP_CTPInfoStru *reg_info);
#else
#define SDAP_RegisterCTPService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_CTP_ICP
WORD SDAP_RegisterICPService(UCHAR *svc_name, struct SDAP_ICPInfoStru *reg_info);	
#else 
#define SDAP_RegisterICPService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_SPP_SERVER
WORD SDAP_RegisterSPPService(UCHAR *svc_name, struct SDAP_SPPInfoStru *reg_info);
#else
#define SDAP_RegisterSPPService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_SAP_SERVER
WORD SDAP_RegisterSAPService(UCHAR *svc_name, struct SDAP_SAPInfoStru *reg_info);
#else
#define SDAP_RegisterSAPService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#endif
#if (defined(CONFIG_HEP_HS) || defined(CONFIG_HFP_HF))
WORD SDAP_RegisterHEPHSService(UCHAR *svc_name, struct SDAP_HEPHSInfoStru *reg_info);
#else
#define SDAP_RegisterHEPHSService(name, reg)		(ERROR_SDAP_API_UNAVAILABLE)
#endif
#define SDAP_RegisterHEPService(name, reg)			SDAP_RegisterHEPHSService(name, reg)
#if (defined(CONFIG_HEP_AG) || defined(CONFIG_HFP_AG))
WORD SDAP_RegisterHEPAGService(UCHAR *svc_name, struct SDAP_HEPAGInfoStru *reg_info);
#else
#define SDAP_RegisterHEPAGService(name, reg)		(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_HFP_HF
WORD SDAP_RegisterHFPHFService(UCHAR *svc_name, struct SDAP_HFPHFInfoStru *reg_info);
#else
#define SDAP_RegisterHFPHFService(name, reg)		(ERROR_SDAP_API_UNAVAILABLE)
#endif
#define SDAP_RegisterHFPService(name, reg)			SDAP_RegisterHFPHFService(name, reg)
#ifdef CONFIG_HFP_AG
WORD SDAP_RegisterHFPAGService(UCHAR *svc_name, struct SDAP_HFPAGInfoStru *reg_info);
#else
#define SDAP_RegisterHFPAGService(name, reg)	 	(ERROR_SDAP_API_UNAVAILABLE)	
#endif
#ifdef CONFIG_DUN_SERVER
WORD SDAP_RegisterDUNService(UCHAR *svc_name, struct SDAP_DUNInfoStru *reg_info);
#else
#define SDAP_RegisterDUNService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_FAX_SERVER
WORD SDAP_RegisterFaxService(UCHAR *svc_name, struct SDAP_FaxInfoStru *reg_info);
#else
#define SDAP_RegisterFaxService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_LAP_SERVER
WORD SDAP_RegisterLAPService(UCHAR *svc_name, struct SDAP_LAPInfoStru *reg_info);
#else
#define SDAP_RegisterLAPService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_OPP_SERVER
WORD SDAP_RegisterOPPService(UCHAR *svc_name, struct SDAP_OPPInfoStru *reg_info);
#else
#define SDAP_RegisterOPPService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_FTP_SERVER
WORD SDAP_RegisterFTPService(UCHAR *svc_name, struct SDAP_FTPInfoStru *reg_info);
#else
#define SDAP_RegisterFTPService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_IRMC_SERVER
WORD SDAP_RegisterSyncService(UCHAR *svc_name, struct SDAP_SyncInfoStru *reg_info);
#else
#define SDAP_RegisterSyncService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_IRMC_CLIENT
WORD SDAP_RegisterSyncCmdService(UCHAR *svc_name, struct SDAP_SyncCmdInfoStru *reg_info);
#else
#define SDAP_RegisterSyncCmdService(name, reg)		(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_BIP_SERVER
WORD SDAP_RegisterBIPImgService(UCHAR *svc_name, struct SDAP_BIPImgInfoStru *reg_info);
#else
#define SDAP_RegisterBIPImgService(name, reg)		(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_BIP_CLIENT
WORD SDAP_RegisterBIPObjService(UCHAR *svc_name, struct SDAP_BIPObjInfoStru *reg_info);
#else
#define SDAP_RegisterBIPObjService(name, reg)		(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_PAN_SERVER
WORD SDAP_RegisterPANService(UCHAR *svc_name, struct SDAP_PANInfoStru *reg_info);
#define SDAP_RegisterGNService(name, reg)		SDAP_RegisterPANService(name, reg); 
#define SDAP_RegisterNAPService(name, reg)		SDAP_RegisterPANService(name, reg);
#define SDAP_RegisterPANUService(name, reg)		SDAP_RegisterPANService(name, reg); 
#else
#define SDAP_RegisterPANService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#define SDAP_RegisterNAPService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)	
#define SDAP_RegisterGNService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)	
#define SDAP_RegisterPANUService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)	
#endif
#ifdef CONFIG_AVRCP
WORD SDAP_RegisterAVRCPService(UCHAR *svc_name, struct SDAP_AVRCPInfoStru *reg_info);
#define SDAP_RegisterAVCTService(name, reg)		SDAP_RegisterAVRCPService(name, reg); 
#define SDAP_RegisterAVTGService(name, reg)		SDAP_RegisterAVRCPService(name, reg); 
#else
#define SDAP_RegisterAVRCPService(name, reg)		(ERROR_SDAP_API_UNAVAILABLE)
#define SDAP_RegisterAVCTService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)	
#define SDAP_RegisterAVTGService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)	
#endif
#ifdef CONFIG_A2DP
WORD SDAP_RegisterA2DPService(UCHAR *svc_name, struct SDAP_A2DPInfoStru *reg_info);
#define SDAP_RegisterA2DPSrcService(name, reg)		SDAP_RegisterA2DPService(name, reg); 
#define SDAP_RegisterA2DPSnkService(name, reg)		SDAP_RegisterA2DPService(name, reg); 
#else
#define SDAP_RegisterA2DPService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#define SDAP_RegisterA2DPSrcService(name, reg)		(ERROR_SDAP_API_UNAVAILABLE)	
#define SDAP_RegisterA2DPSnkService(name, reg)		(ERROR_SDAP_API_UNAVAILABLE)	
#endif
#ifdef CONFIG_HCRP
WORD SDAP_RegisterHCRPService(UCHAR *svc_name, struct SDAP_HCRPInfoStru *reg_info);
#define SDAP_RegisterHCRPCtrlService(name, reg)		SDAP_RegisterHCRPService(name, reg); 
#define SDAP_RegisterHCRPNotifyService(name, reg)	SDAP_RegisterHCRPService(name, reg); 
#else
#define SDAP_RegisterHCRPService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#define SDAP_RegisterHCRPCtrlService(name, reg)		(ERROR_SDAP_API_UNAVAILABLE)
#define SDAP_RegisterHCRPNotifyService(name, reg)	(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_HID_SERVER
WORD SDAP_ModifyHIDDescListAttr(DWORD svc_hdl, UCHAR *desc_list, DWORD list_size);
WORD SDAP_RegisterHIDService(UCHAR *svc_name, struct SDAP_HIDInfoStru *reg_info);
#else
#define SDAP_RegisterHIDService(name, reg)			(ERROR_SDAP_API_UNAVAILABLE)
#define SDAP_ModifyHIDDescListAttr					(ERROR_SDAP_API_UNAVAILABLE)
#endif
#ifdef CONFIG_DI_SERVER
WORD SDAP_RegisterDIService(UCHAR *svc_name, struct SDAP_DIInfoStru *reg_info);
#else
#define SDAP_RegisterDIService(name, reg)				(ERROR_SDAP_API_UNAVAILABLE)
#endif

#endif

/*++++++++++++++++++++++++ SDAP Service Search UI Definition +++++++++++++++++++++++++*/
#ifdef CONFIG_SDAP_CLIENT
WORD SDAP_OpenSearch(UCHAR* bd, WORD *trans_hdl);
WORD SDAP_CloseSearch(WORD trans_hdl);
/*WORD SDAP_Search(struct SDP_SearchReqStru *sreq, struct SDP_SearchCfmStru *scfm);
WORD SDAP_SearchEx(struct SDP_SearchReqStru *sreq, struct SDP_SearchCfmStru *scfm);*/
WORD SDAP_ServiceBrowse(struct SDAP_BrowseInfoStru *info, WORD *size, DWORD *hdl_buf);
WORD SDAP_GetServiceAttribute(WORD trans_hdl, DWORD svc_hdl, WORD id, struct SDAP_DataEleStru *value);
WORD SDAP_ServiceSearch(struct SDAP_SearchInfoStru *info, DWORD svc_hdl, DWORD *size, UCHAR *attr_buf);
WORD SDAP_GetStringAttribute(WORD trans_hdl, DWORD svc_hdl, WORD id, DWORD *size, UCHAR *str);
WORD SDAP_GetServiceInfo(WORD trans_hdl, DWORD *uuid, struct SDAP_GeneralInfoStru *sdx_info);
struct SDAP_GeneralInfoStru *SDAP_GetServiceInfoEx(WORD trans_hdl, DWORD svc_hdl, DWORD *svc_type);
WORD SDAP_GetServiceInfo2(WORD trans_hdl, WORD svc_type, struct SDAP_UUIDStru *uuid, struct SDAP_GeneralInfoStru *sdx_info);

#define SDAP_GetCTPSvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_CORDLESS_TELE, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetICPSvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_INTERCOM, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetSPPSvcInfo(hdl, info)  		SDAP_GetServiceInfo2(hdl, CLS_SERIAL_PORT, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetSAPSvcInfo(hdl, info)  		SDAP_GetServiceInfo2(hdl, CLS_SIM_ACCESS, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetHEPSvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_HEADSET, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetHEPHSSvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_HEADSET, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetHEPAGSvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_HEADSET_AG, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetHFPSvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_HANDSFREE, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetHFPHFSvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_HANDSFREE, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetHFPAGSvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_HANDSFREE_AG, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetDUNSvcInfo(hdl, info)  		SDAP_GetServiceInfo2(hdl, CLS_DIALUP_NET, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetFaxSvcInfo(hdl, info)  		SDAP_GetServiceInfo2(hdl, CLS_FAX, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetLAPSvcInfo(hdl, info)  		SDAP_GetServiceInfo2(hdl, CLS_LAN_ACCESS, NULL, (struct SDAP_GeneralInfoStru*)info) 
#define SDAP_GetFTPSvcInfo(hdl, info)  		SDAP_GetServiceInfo2(hdl, CLS_OBEX_FILE_TRANS, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetOPPSvcInfo(hdl, info)  		SDAP_GetServiceInfo2(hdl, CLS_OBEX_OBJ_PUSH, NULL, (struct SDAP_GeneralInfoStru*)info); 
#define SDAP_GetSyncSvcInfo(hdl, info)  	SDAP_GetServiceInfo2(hdl, CLS_IRMC_SYNC, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetSyncCmdSvcInfo(hdl, info)  	SDAP_GetServiceInfo2(hdl, CLS_IRMC_SYNC_CMD, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetBIPImgSvcInfo(hdl, info)  	SDAP_GetServiceInfo2(hdl, CLS_IMAG_RESPONDER, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetBIPRefObjSvcInfo(hdl, info)	SDAP_GetServiceInfo2(hdl, CLS_IMAG_REF_OBJ, &((info)->svc_id), (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetBIPArchSvcInfo(hdl, info)  	SDAP_GetServiceInfo2(hdl, CLS_IMAG_AUTO_ARCH, &((info)->svc_id), (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetNAPSvcInfo(hdl, info)  		SDAP_GetServiceInfo2(hdl, CLS_PAN_NAP, NULL, (struct SDAP_GeneralInfoStru*)info) 
#define SDAP_GetGNSvcInfo(hdl, info)   		SDAP_GetServiceInfo2(hdl, CLS_PAN_GN, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetPANUSvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_PAN_PANU, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetAVRCPSvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, (info)->svc_type, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetAVRCTInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_AVRCP_CT, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetAVRTGInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_AVRCP_TG, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetA2DPSvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_ADV_AUDIO_DISTRIB, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetA2DPSrcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_AUDIO_SOURCE, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetA2DPSnkInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_AUDIO_SINK, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetHCRPSvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, (info)->conn_type, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetHCRPCtrlSvcInfo(hdl, info) 	SDAP_GetServiceInfo2(hdl, PROTOCOL_HCR_CONTROL, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetHCRPNotifySvcInfo(hdl, info)   SDAP_GetServiceInfo2(hdl, PROTOCOL_HCR_NOTIFY, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetHIDSvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_HID, NULL, (struct SDAP_GeneralInfoStru*)info)
#define SDAP_GetDISvcInfo(hdl, info)		SDAP_GetServiceInfo2(hdl, CLS_PNP_INFO, NULL, (struct SDAP_GeneralInfoStru*)info)
#endif

#endif
