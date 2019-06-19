/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    avdtp.h
    
Abstract:
	This is the global head file which includes other necessary head files, 
	and it includes the definition of global structures, variables and constants.
	
Author:   
    Lixin Qian

Create Date:
	10/22/2002
	
Revision History:
	
---------------------------------------------------------------------------*/
#ifndef AVDTP_H
#define AVDTP_H


/*#define CONFIG_AVDTP_REPORT_SERVICE 1*/

#ifdef WIN32
#include "winsock2.h"
#endif

#include "global.h"
#include "sm.h"
#include "l2capui.h"
#include "l2func.h"

#include "avdtpui.h"

/***************************************************************************
						Global Constants  
***************************************************************************/

/* signal msg header length */
#define AVDTP_SIGNAL_MSG_SINGLE_HEADER_LEN 2
#define AVDTP_SIGNAL_MSG_START_HEADER_LEN 3
#define AVDTP_SIGNAL_MSG_CONEND_HEADER_LEN 1

/********************************/
/*  reserved header len			*/
/********************************/
/* Reserved header length for L2CAP and HCI */
#define AVDTP_BUFFER_RESERVE	(HCI_RESERVE + L2CAP_CO_RESERVE)	/* 20 */
#define AVDTP_SIGNAL_BUF_RESERVE	(AVDTP_SIGNAL_MSG_SINGLE_HEADER_LEN+AVDTP_BUFFER_RESERVE)

/* MAX length of RTP payload */
#define AVDTP_RTP_MAX_PACKET_LEN	1600

/* channel type */
#define AVDTP_CHANNEL_TYPE_SIGNAL		0
#define AVDTP_CHANNEL_TYPE_MEDIA		1
#define AVDTP_CHANNEL_TYPE_REPORT		2
#define AVDTP_CHANNEL_TYPE_RECOVERY		3
#define AVDTP_CHANNEL_TYPE_COMMON		4
#define AVDTP_CHANNEL_TYPE_MULTIPLEX	 5
#define AVDTP_CHANNEL_TYPE_RESERVED	 6

/* signal id (6bits) */
#define AVDTP_RFA						0x00
#define AVDTP_DISCOVER 					0x01
#define AVDTP_GET_CAPABILITIES 			0x02
#define AVDTP_SET_CONFIGURATION 		0x03
#define AVDTP_GET_CONFIGURATION 		0x04
#define AVDTP_RECONFIGURE 				0x05
#define AVDTP_OPEN 						0x06
#define AVDTP_START 					0x07
#define AVDTP_CLOSE 					0x08
#define AVDTP_SUSPEND 					0x09
#define AVDTP_ABORT 					0x0A
#define AVDTP_SECURITY_CONTROL 			0x0B

/* these two signal id used locally */
#define AVDTP_CHANNEL_CONNECT			0xFE
#define AVDTP_CHANNEL_DISCONNECT		0xFF

/* signal direction */
#define AVDTP_SIGNAL_DIRECTION_REQ 1
#define AVDTP_SIGNAL_DIRECTION_IND 2
#define AVDTP_SIGNAL_DIRECTION_DONE 3



/* signal msg type */
#define AVDTP_SIGNAL_MSG_TYPE_COMMAND 		0
#define AVDTP_SIGNAL_MSG_TYPE_RESERVED 		1
#define AVDTP_SIGNAL_MSG_TYPE_RSP_ACCEPT 2
#define AVDTP_SIGNAL_MSG_TYPE_RSP_REJECT 3

/* signal packet type */
#define AVDTP_SIGNAL_PKT_TYPE_SINGLE 		0
#define AVDTP_SIGNAL_PKT_TYPE_START 		1
#define AVDTP_SIGNAL_PKT_TYPE_CONTINUE 	2
#define AVDTP_SIGNAL_PKT_TYPE_END 			3

/* service category value within PDU */
#define AVDTP_S_C_MEDIA_TRANSPORT 			1
#define AVDTP_S_C_REPORT					2
#define AVDTP_S_C_RECOVERY					3
#define AVDTP_S_C_CONTENT_PROTECTION		4
#define AVDTP_S_C_HEADER_COMPRESSION 		5
#define AVDTP_S_C_MULTIPLEXING				6
#define AVDTP_S_C_MEDIA_CODEC				7

#ifdef CONFIG_CONFORMANCE_TESTER
#define AVDTP_S_C_BAD_RECOVERY_TYPE			31
#endif

#define AVDTP_S_C_COUNT						7

#define AVDTP_S_C_NOT_DEFINED				0xff

/* Wait Handle Class ID  */
#define AVDTP_HANDLE_CLASS_ID_CONNECT					(PROT_AVDTP_BASE + 1)
#define AVDTP_HANDLE_CLASS_ID_DISCONNECT				(PROT_AVDTP_BASE + 2)
#define AVDTP_HANDLE_CLASS_ID_READ_WAIT_EVENT			(PROT_AVDTP_BASE + 3)


/* service category map between UI and PDU */
#define AVDTP_SC_MAP_UI_2_PDU(ui_sc)		ServiceCategoryMap_UI2PDU(ui_sc)
#define AVDTP_SC_MAP_PDU_2_UI(pdu_sc)		((pdu_sc) ? (1 << ((pdu_sc) - 1)) : 0)


#define AVDTP_TIMER_ID DWORD



/*Macro definition for Callback result*/
#define AVDTP_RESULT_INIT				0xFF
#define AVDTP_RESULT_CALLBACK			0x00
#define AVDTP_RESULT_NOACTION			0x01
#define AVDTP_RESULT_HOOKBACK			0x02

#define AVDTP_SET_L2CAP_CONFIG_PARAM(src_param, dst_param, copy) {\
		if ((src_param) && (src_param)->in_mtu)				\
			(dst_param).in_mtu = (src_param)->in_mtu;		\
		else											\
			(dst_param).in_mtu = DEFAULTMTUcno;			\
\
		if ((src_param) && (src_param)->flush_to)				\
			(dst_param).flush_to = (src_param)->flush_to;		\
		else											\
			(dst_param).flush_to = DEFAULTFLUSHTO;			\
\
		if (src_param)				\
			(dst_param).link_to = (src_param)->link_to;\
		else \
			(dst_param).link_to = 0;\
\
		if ((src_param) && (src_param)->flow) {				\
			if (copy) {\
				(dst_param).flow = (struct QosStru*)NEW(sizeof(struct QosStru));\
				memcpy((dst_param).flow, (src_param)->flow, sizeof(struct QosStru));\
			} else {\
				(dst_param).flow = (src_param)->flow;\
			}\
		} else											\
			(dst_param).flow = NULL;			\
	}


/***************************************************************************
						Callback Macros
***************************************************************************/
#define AVDTP_SendMsg(e, a) {\
	((AVDTP_SM_USERDATA*)(avdtp_fsminst->user_data))->cb(e, a);\
}

#define AVDTP_SendConnectionInd(ev, bd)	{\
	UCHAR *addr = (UCHAR*)NEW(BD_ADDR_LEN);\
	memcpy(addr, bd, BD_ADDR_LEN);\
	AVDTP_SendMsg(ev, addr);\
}

#define AVDTP_SendDisconnectInd(bd) \
	AVDTP_SendConnectionInd(AVDTP_Event_DisconnectReq_Ind, bd)

#define AVDTP_SendConnectInd(bd) \
	AVDTP_SendConnectionInd(AVDTP_Event_ConnectReq_Ind, bd)

#define AVDTP_SendConnectCfm(bd, res) {\
	struct AVDTP_ConnectCfmStru *cfm;\
	cfm = (struct AVDTP_ConnectCfmStru*)NEW(sizeof(struct AVDTP_ConnectCfmStru));\
	memcpy(cfm->bd_addr, bd, BD_ADDR_LEN);\
	cfm->connect_result = cfm->config_result = cfm->status = res;\
	AVDTP_SendMsg(AVDTP_Event_ConnectReq_Cfm, cfm);\
}

#define AVDTP_SendDiscoverCfm(bd, err, num, sep) {\
	struct AVDTP_DiscoverCfmStru *cfm;\
	cfm = (struct AVDTP_DiscoverCfmStru*)NEW(sizeof(struct AVDTP_DiscoverCfmStru));\
	memcpy(cfm->bd_addr, bd, BD_ADDR_LEN);\
	cfm->error_code = err;\
	cfm->sep_num = num;\
	cfm->p_acp_sep = sep;\
	AVDTP_SendMsg(AVDTP_Event_Discover_Cfm, cfm);\
}

#define AVDTP_SendDiscoverInd(label, bd) {\
	struct AVDTP_DiscoverIndStru *ind;\
	ind = (struct AVDTP_DiscoverIndStru*)NEW(sizeof(struct AVDTP_DiscoverIndStru));\
	ind->transaction = label;\
	memcpy(ind->bd_addr, bd, BD_ADDR_LEN);\
	AVDTP_SendMsg(AVDTP_Event_Discover_Ind, ind);\
}

#define AVDTP_SendGetCapsCfm(bd, seid, err, cp) {\
	struct AVDTP_GetCapsCfmStru *cfm;\
	cfm = (struct AVDTP_GetCapsCfmStru*)NEW(sizeof(struct AVDTP_GetCapsCfmStru));\
	memcpy(cfm->bd_addr, bd, BD_ADDR_LEN);\
	cfm->acp_seid = seid;\
	cfm->error_code = err;\
	cfm->config_param = cp;\
	AVDTP_SendMsg(AVDTP_Event_Get_Capabilities_Cfm, cfm);\
}

#define AVDTP_SendGetCapsInd(label, seid, bd) {\
	struct AVDTP_GetCapsIndStru *ind;\
	ind = (struct AVDTP_GetCapsIndStru*)NEW(sizeof(struct AVDTP_GetCapsIndStru));\
	ind->transaction = label;\
	ind->acp_seid = seid;\
	memcpy(ind->bd_addr, bd, BD_ADDR_LEN);\
	AVDTP_SendMsg(AVDTP_Event_Get_Capabilities_Ind, ind);\
}

#define AVDTP_SendSetConfigurationCfm(bd, _acp_seid, _int_seid, err, sh, fcf) {\
	struct AVDTP_SetConfigurationCfmStru *cfm;\
	cfm = (struct AVDTP_SetConfigurationCfmStru*)NEW(sizeof(struct AVDTP_SetConfigurationCfmStru));\
	memcpy(cfm->bd_addr, bd, BD_ADDR_LEN);\
	cfm->acp_seid = _acp_seid;\
	cfm->int_seid = _int_seid;\
	cfm->error_code = err;\
	cfm->stream_handle = sh;\
	cfm->first_category_to_fail = fcf;\
	AVDTP_SendMsg(AVDTP_Event_Set_Configuration_Cfm, cfm);\
}

#define AVDTP_SendSetConfigurationInd(label, _acp_seid, _int_seid, bd, sh, cp) {\
	struct AVDTP_SetConfigurationIndStru *ind;\
	ind = (struct AVDTP_SetConfigurationIndStru*)NEW(sizeof(struct AVDTP_SetConfigurationIndStru));\
	ind->transaction = label;\
	ind->acp_seid = _acp_seid;\
	ind->int_seid = _int_seid;\
	memcpy(ind->bd_addr, bd, BD_ADDR_LEN);\
	ind->stream_handle = sh;\
	ind->config_param = cp;\
	AVDTP_SendMsg(AVDTP_Event_Set_Configuration_Ind, ind);\
}

#define AVDTP_SendCommonCfm(ev, sh, err) {\
	struct AVDTP_CommonCfmStru *cfm;\
	cfm = (struct AVDTP_CommonCfmStru*)NEW(sizeof(struct AVDTP_CommonCfmStru));\
	cfm->stream_handle = sh;\
	cfm->error_code = err;\
	AVDTP_SendMsg(ev, cfm);\
}

#define AVDTP_SendOpenCfm(sh, err) \
	AVDTP_SendCommonCfm(AVDTP_Event_Open_Cfm, sh, err)
#define AVDTP_SendCloseCfm(sh, err) \
	AVDTP_SendCommonCfm(AVDTP_Event_Close_Cfm, sh, err)
#define AVDTP_SendAbortCfm(sh, err) \
	AVDTP_SendCommonCfm(AVDTP_Event_Abort_Cfm, sh, err)


#define AVDTP_SendCommonInd(ev, label, sh) {\
	struct AVDTP_CommonIndStru *ind;\
	ind = (struct AVDTP_CommonIndStru*)NEW(sizeof(struct AVDTP_CommonIndStru));\
	ind->transaction = label;\
	ind->stream_handle = sh;\
	AVDTP_SendMsg(ev, ind);\
}

#define AVDTP_SendOpenInd(label, sh) \
	AVDTP_SendCommonInd(AVDTP_Event_Open_Ind, label, sh)
#define AVDTP_SendCloseInd(label, sh) \
	AVDTP_SendCommonInd(AVDTP_Event_Close_Ind, label, sh)
#define AVDTP_SendAbortInd(label, sh) \
	AVDTP_SendCommonInd(AVDTP_Event_Abort_Ind, label, sh)


#define AVDTP_SendStartSuspendCfm(ev, err, sh) {\
	struct AVDTP_StartSuspendCfmStru *cfm;\
	cfm = (struct AVDTP_StartSuspendCfmStru*)NEW(sizeof(struct AVDTP_StartSuspendCfmStru));\
	cfm->first_failing_stream_handle = (sh);\
	cfm->error_code = (err);\
	AVDTP_SendMsg(ev, cfm);\
}

#define AVDTP_SendStartCfm(err, handle) \
	AVDTP_SendStartSuspendCfm(AVDTP_Event_Start_Cfm, err, handle)
#define AVDTP_SendSuspendCfm(err, handle) \
	AVDTP_SendStartSuspendCfm(AVDTP_Event_Suspend_Cfm, err, handle)

#define AVDTP_SendStartSuspendInd(ev, label, _num, handle) {\
	struct AVDTP_StartSuspendIndStru *ind;\
	ind = (struct AVDTP_StartSuspendIndStru*)NEW((_num)*sizeof(WORD) + sizeof(struct AVDTP_StartSuspendIndStru));\
	ind->transaction = (label);\
	ind->num = (_num);\
	memcpy(ind->p_stream_handle, (handle), (_num)*sizeof(WORD));\
	AVDTP_SendMsg(ev, ind);\
}

#define AVDTP_SendStartInd(label, num, handle) \
	AVDTP_SendStartSuspendInd(AVDTP_Event_Start_Ind, label, num, handle)
#define AVDTP_SendSuspendInd(label, num, handle) \
	AVDTP_SendStartSuspendInd(AVDTP_Event_Suspend_Ind, label, num, handle)


#define AVDTP_SendGetConfigurationCfm(handle, err, param) {\
	struct AVDTP_GetConfigurationCfmStru *cfm;\
	cfm = (struct AVDTP_GetConfigurationCfmStru*)NEW(sizeof(struct AVDTP_GetConfigurationCfmStru));\
	cfm->stream_handle = handle;\
	cfm->error_code = err;\
	cfm->config_param = param;\
	AVDTP_SendMsg(AVDTP_Event_Get_Configuration_Cfm, cfm);\
}

#define AVDTP_SendGetConfigurationInd(label, handle) \
	AVDTP_SendCommonInd(AVDTP_Event_Get_Configuration_Ind, label, handle)

#define AVDTP_SendReconfigureCfm(handle, err, catogory_fail) {\
	struct AVDTP_ReconfigCfmStru *cfm;\
	cfm = (struct AVDTP_ReconfigCfmStru*)NEW(sizeof(struct AVDTP_ReconfigCfmStru));\
	cfm->stream_handle = handle;\
	cfm->error_code = err;\
	cfm->first_category_to_fail = catogory_fail;\
	AVDTP_SendMsg(AVDTP_Event_ReConfigure_Cfm, cfm);\
}

#define AVDTP_SendReconfigureInd(label, handle, param) {\
	struct AVDTP_ReconfigureIndStru *ind;\
	ind = (struct AVDTP_ReconfigureIndStru*)NEW(sizeof(struct AVDTP_ReconfigureIndStru));\
	ind->transaction = label;\
	ind->stream_handle = handle;\
	ind->config_param = param;\
	AVDTP_SendMsg(AVDTP_Event_ReConfigure_Ind, ind);\
}

#define AVDTP_SendSecurityControlCfm(handle, err, len, data) {\
	struct AVDTP_SecurityControlCfmStru *cfm;\
	cfm = (struct AVDTP_SecurityControlCfmStru*)NEW((len) + sizeof(struct AVDTP_SecurityControlCfmStru));\
	cfm->stream_handle = handle;\
	cfm->error_code = err;\
	cfm->length = (len);\
	if (len && data) \
		memcpy(cfm->security_data, data, (len));\
	AVDTP_SendMsg(AVDTP_Event_Security_Control_Cfm, cfm);\
}

#define AVDTP_SendSecurityControlInd(label, handle, len, data) {\
	struct AVDTP_SecurityControlIndStru *ind;\
	ind = (struct AVDTP_SecurityControlIndStru*)NEW((len) + sizeof(struct AVDTP_SecurityControlIndStru));\
	ind->transaction = label;\
	ind->stream_handle = handle;\
	ind->length = (len);\
	if (len && data) \
		memcpy(ind->security_data, data, len);\
	AVDTP_SendMsg(AVDTP_Event_Security_Control_Ind, ind);\
}

#define AVDTP_SendReadCfm(handle, _result, len, t, pt, mk, rb) {\
	struct AVDTP_ReadCfmStru *cfm;\
	cfm = (struct AVDTP_ReadCfmStru*)NEW(sizeof(struct AVDTP_ReadCfmStru));\
	cfm->stream_handle = handle;\
	cfm->result = _result;\
	cfm->length = len;\
	cfm->time_info = t;\
	cfm->payload_type = pt;\
	cfm->marker = mk;\
	cfm->reliability = rb;\
	AVDTP_SendMsg(AVDTP_Event_Read_Cfm, cfm);\
}

#define AVDTP_SendWriteCfm(handle, _result) {\
	struct AVDTP_WriteCfmStru *cfm;\
	cfm = (struct AVDTP_WriteCfmStru*)NEW(sizeof(struct AVDTP_WriteCfmStru));\
	cfm->stream_handle = handle;\
	cfm->result = _result;\
	AVDTP_SendMsg(AVDTP_Event_Write_Cfm, cfm);\
}

#define AVDTP_SendDataInd(handle, buf, len, ti, pt, mk, r) {\
	struct AVDTP_DataIndStru *ind;\
	ind = (struct AVDTP_DataIndStru*)NEW(sizeof(struct AVDTP_DataIndStru));\
	ind->stream_handle = (handle);\
	ind->data = (buf);\
	ind->length = (len);\
	ind->time_info = (ti);\
	ind->payload_type = (pt);\
	ind->marker = (mk);\
	ind->reliability = (r);\
	AVDTP_SendMsg(AVDTP_Event_Data_Ind, ind);\
}

#define AVDTP_SendTCMtuInd(handle, r, cid_context) {\
	struct AVDTP_TransportChannelMtuStru *ind;\
	ind = (struct AVDTP_TransportChannelMtuStru*)NEW(sizeof(struct AVDTP_TransportChannelMtuStru));\
	memset(ind, 0, sizeof(struct AVDTP_TransportChannelMtuStru));\
	ind->stream_handle = handle;\
	ind->result = r;\
	if (cid_context) {\
		ind->media_mtu = (cid_context)->media_mtu;\
		ind->report_mtu = (cid_context)->report_mtu;\
		ind->recovery_mtu = (cid_context)->recovery_mtu;\
		ind->local_media_mtu = (cid_context)->media_param.in_mtu;\
		ind->local_report_mtu = (cid_context)->report_param.in_mtu;\
		ind->local_recovery_mtu = (cid_context)->recovery_param.in_mtu;\
	}\
	AVDTP_SendMsg(AVDTP_Event_Remote_TCMTU_Ind, ind);\
}

#define AVDTP_SendGetTCMtuCfm(handle, r, cid_context) {\
	struct AVDTP_TransportChannelMtuStru *cfm;\
	cfm = (struct AVDTP_TransportChannelMtuStru*)NEW(sizeof(struct AVDTP_TransportChannelMtuStru));\
	memset(cfm, 0, sizeof(struct AVDTP_TransportChannelMtuStru));\
	cfm->stream_handle = handle;\
	cfm->result = r;\
	if (cid_context) {\
		cfm->media_mtu = (cid_context)->media_mtu;\
		cfm->report_mtu = (cid_context)->report_mtu;\
		cfm->recovery_mtu = (cid_context)->recovery_mtu;\
		cfm->local_media_mtu = (cid_context)->media_param.in_mtu;\
		cfm->local_report_mtu = (cid_context)->report_param.in_mtu;\
		cfm->local_recovery_mtu = (cid_context)->recovery_param.in_mtu;\
	}\
	AVDTP_SendMsg(AVDTP_Event_GetTCMtu_Cfm, cfm);\
}

#define AVDTP_SendNULLGetTCMtuCfm(handle, r) {\
	struct AVDTP_TransportChannelMtuStru *cfm;\
	cfm = (struct AVDTP_TransportChannelMtuStru*)NEW(sizeof(struct AVDTP_TransportChannelMtuStru));\
	memset(cfm, 0, sizeof(struct AVDTP_TransportChannelMtuStru));\
	cfm->stream_handle = handle;\
	cfm->result = r;\
	AVDTP_SendMsg(AVDTP_Event_GetTCMtu_Cfm, cfm);\
}


/*****************************************/

#define AvdtpFsmEvent(fi,event,arg) FsmEventEx(fi,event,arg,BT_PRI_HIGH, 1)


/***************************************************************************
						Global structures
***************************************************************************/
struct AVDTP_UIParamStru{
	UCHAR* in;
	UCHAR* out;
};

struct AVDTP_SignalMsgHeader {
#ifndef CONFIG_BIG_ENDIAN	
	UCHAR msg_type:2;
	UCHAR pkt_type:2;
	UCHAR label:4;
#else	
	UCHAR label:4;
	UCHAR pkt_type:2;
	UCHAR msg_type:2;
#endif
};

typedef struct AVDTP_SignalMsgStru {
	UINT8	label;
	UINT8	msg_type;
	UINT8	signal_id;
	UINT8	nosp;
	WORD	len;
	UCHAR* data;	
}AVDTP_SignalMsg;

/********************************************************************************************/
/*																							*/
/*				transaction model (object)													*/
/*																							*/
/********************************************************************************************/
struct AVDTP_BD_CONTEXT_STRUCT;
typedef struct AVDTP_TRANSACTION_STRUCT {
	UINT8			label;		/* transaction label */
	UINT8			signal_id;	/* signal id 	*/
	UINT8			direction;	/* Req, Ind 	*/
	AVDTP_TIMER_ID	timer_id;		/* Only for Req    */
	struct FsmInst	*fi;		/* sm or sep fsm */
	struct AVDTP_BD_CONTEXT_STRUCT *bd_context;		/* pertaining to which bd_context */

		/* 
		 * Used for START or SUSPEND transaction to hold seid.
		 * The first byte is to keep the num of seids.
		 */
	UCHAR			*param;	
} AVDTP_TRANSACTION;

/********************************************************************************************/
/*																							*/
/*				Streaming devices context (object)											*/
/*																							*/
/********************************************************************************************/
typedef struct AVDTP_BD_CONTEXT_STRUCT {
	/* peer device address and its signal channel context */
	UCHAR 				bd_addr[BD_ADDR_LEN];
	WORD				signal_cid;
	WORD				signal_mtu;
	UCHAR				identifier;		/* for L2CAP */

	struct AVDTP_ChannelConfigParamStru *l2cap_param;
	
	/* signal message incomplete --- temporarily stored here */
	AVDTP_SignalMsg		*signal_msg;
	
	/* cid context list for non-multiplexed channel associated with this bd_addr */
	struct BtList		*common_cid_context_list; /* AVDTP_COMMON_CID_CONTEXT */

	/* tcid context list for multiplexed channel associated with this bd_addr */
	struct BtList		*tcid_context_list;	/* AVDTP_TCID_CONTEXT */

	/* transaction associated with this bd_context */
	struct BtList		*transaction_list;	/* AVDTP_TRANSACTION */

	/* sep fsm instance list */
	struct BtList		*sep_fsm_list;		/* sep fsm pointer */
	
	/* used for tsid & tcid allocation (5bits: 1-31) */
	UINT8				g_tsid;	
	UINT8				g_tcid;
	
	/* used for transaction label allocation */
	UINT8				g_label;	
}AVDTP_BD_CONTEXT;

/* Only for none-multiplexed channel */
typedef struct AVDTP_COMMON_CID_CONTEXT_STRUCT {
	WORD			cid;			/* L2CAP cid 					*/
	WORD			mtu;			/* channel mtu 					*/
	struct FsmInst*	sep_fsm_inst;	/* pertain to Specific stream pointer    */
	UINT8			type;			/* media, report, recovery             */
} AVDTP_COMMON_CID_CONTEXT;

struct AVDTP_ALStartSegmentStru;
/* Only for multiplexed channel */
typedef struct AVDTP_TCID_CONTEXT_STRUCT {
	UINT8			tcid;			/* transport channel id        */
	WORD			cid;			/* L2CAP cid 			*/
	WORD			mtu;			/* channel mtu			*/		

		/* multiplexed transport session node list */
	struct BtList	*transport_session_list; /* AVDTP_MultiplexNode */

		/* multiplexed mode's packets pending to be sent */
	AVDTP_TIMER_ID	timer_id;				/* timer used to safeguard not waiting too long  */
	WORD			total;					/* accumulated total bytes of packets to be sent */
	struct BtList	*multiplex_packet_list;	/* AVDTP_MultiplexPacket 			    */

		/* AL-Start segment received is stored here temporarily ---only one for each TCID Context    */
	struct AVDTP_ALStartSegmentStru *al_start;
	
} AVDTP_TCID_CONTEXT;

/* multiplexed transport session node */
typedef struct AVTDP_MultiplexNode_STRUCT {
	UINT8			tsid;			/* transport session id 	*/	
	UINT8			type;			/* media, report, recovery 	*/
	struct FsmInst*	sep_fsm_inst; 	/* sep fsm instance			*/
} AVDTP_MultiplexNode;

/* multiplexed packet pending to be sent */
typedef struct AVDTP_MultiplexPacketStru {
	UINT8			tsid;			/* transport session id 	*/	
	UINT8			type;			/* media, report, recovery 	*/
	UINT8			frag;			/* support fragmentation?	*/
	struct FsmInst*	sep_fsm_inst; 	/* sep fsm instance			*/
	WORD			len_total;		/* packet total length		*/
	WORD			len_left;		/* length to be sent if FRAG  */
	UCHAR			*data;			/* packet data				*/
}AVDTP_MultiplexPacket;

/* AL-PDU start segment */
typedef struct AVDTP_ALStartSegmentStru {
	UINT8			tsid;			/* transport session id 		*/
	WORD			total_length;	/* total length of this multiplex packet */
	WORD			arrived_length;	/* length of data arrived	    */
	UCHAR			data[1];		/* multiplex packet data		*/
}AVDTP_ALStartSegment;


/********************************************************************************************/
/*																							*/
/*				AL (Adaptation Layer)														*/
/*																							*/
/********************************************************************************************/

#define AVDTP_AL_HEADER_LEN 3


/********************************************************************************************/
/*																							*/
/*				RTP																			*/
/*																							*/
/********************************************************************************************/
/* # of Payload types available. */
#define _RTP_MAX_PAYLOAD_TYPES 128

typedef struct {
  DWORD secs;
  DWORD frac;
} NTP64;


#ifdef WIN32	
	typedef struct timeval	AVDTP_TIMEVAL;
#else
	struct timeval {
        long    tv_sec;       
        long    tv_usec;
	};
	typedef struct timeval	AVDTP_TIMEVAL;
#endif

/* Number of different SDES fields we support. */
#define _RTP_NUM_SDES_TYPES 12

/* Protocol version */
#define _RTP_VERSION 2

typedef struct {
#ifdef CONFIG_BIG_ENDIAN
	UCHAR   v:2;		/* packet type                */
	UCHAR   p:1;		/* padding flag               */
	UCHAR   x:1;		/* header extension flag      */
	UCHAR   cc:4;		/* CSRC count                 */
	UCHAR   m:1;		/* marker bit                 */
	UCHAR   pt:7;		/* payload type               */
#else
	UCHAR   cc:4;		/* CSRC count                 */
	UCHAR   x:1;		/* header extension flag      */
	UCHAR   p:1;		/* padding flag               */
	UCHAR   v:2;		/* packet type   (MSB_F: bit7-6)*/
	UCHAR   pt:7;		/* payload type               */
	UCHAR   m:1;		/* marker bit                 */
#endif
  WORD 		seq;             	/* sequence number */
  DWORD 	ts;              	/* timestamp */
  DWORD 	ssrc;            	/* synchronization source */
  DWORD 	csrc[1];         	/* CSRC list */
} RTP_HEADER;

/* Protocol version */
#define RTP_VERSION(hdr) ((hdr).v)
#define SET_RTP_VERSION(hdr, val) ((hdr).v = (val))

/* Padding flag */
#define RTP_P(hdr) ((hdr).p)
#define SET_RTP_P(hdr, val) ((hdr).p = (val))

/* Extension flag */
#define RTP_X(hdr) ((hdr).x)
#define SET_RTP_X(hdr, val) ((hdr).x = (val))

/* CSRC Count */
#define RTP_CC(hdr) ((hdr).cc)
#define SET_RTP_CC(hdr, val) ((hdr).cc = (UCHAR)(val))

/* Marker bit */
#define RTP_M(hdr) ((hdr).m)
#define SET_RTP_M(hdr, val) ((hdr).m = (val))

/* Payload Type */
#define RTP_PT(hdr) ((hdr).pt)
#define SET_RTP_PT(hdr, val) ((hdr).pt = (val))

typedef struct {           		/* RTP Header Extension */
  WORD 		etype;           	/* extension type */
  WORD 		elen;            	/* extension length */
  UCHAR 	epayload[1];        /* extension payload */
} RTP_HDR_EXT;



typedef struct {
  RTP_HEADER 	*rtp_header;
  RTP_HDR_EXT 	*rtp_extension;
  UCHAR			*payload;         	/* payload portion of RTP packet */
  WORD			payload_len;        /* The length of the payload */
} RTP_PACKET;


#ifdef CONFIG_AVDTP_REPORT_SERVICE
/************************/
/*						*/
/*		RTCP			*/
/*						*/
/************************/
/* 
 * NOTE:
 *	AVDTP_ReceiverReport & AVDTP_SenderReport is defined in avdtpuistru.h
 */
/* These  define the RTCP packet types */
#define  RTCP_PACKET_SR   	200
#define  RTCP_PACKET_RR   	201
#define  RTCP_PACKET_SDES 202
#define  RTCP_PACKET_BYE  	203
#define  RTCP_PACKET_APP  	204

typedef struct {               /* REPORT BLOCK */
  DWORD ssrc;                /* ssrc identifier */
#ifndef CONFIG_BIG_ENDIAN  
  DWORD frac_lost:8;    /* Fraction lost */
  DWORD cum_lost:24;    /* Cumulative pkts lost */
#else  
  DWORD cum_lost:24;    /* Cumulative pkts lost */
  DWORD frac_lost:8;    /* Fraction lost */
#endif
  DWORD highest_seqno;       /* highest seqno received */
  DWORD jitter;              /* interarrival jitter */
  DWORD lsr;                 /* last SR timestamp */
  DWORD dlsr;                /* delay since last SR */
} rtcp_report_block;

#define RTCP_REPORT_BLOCK_SIZE (6 * 4)

typedef struct {                 /* Common header of RTCP packets */
#ifndef CONFIG_BIG_ENDIAN
	UCHAR   v:2;		/* packet type                */
	UCHAR   p:1;		/* padding flag               */
	UCHAR   rc:5;		/* Report block count         */
#else
	UCHAR   rc:5;		/* Report block count         */
	UCHAR   p:1;		/* padding flag               */
	UCHAR   v:2;		/* packet type                */
#endif
  UINT8 pt;             /* payload type 				*/
  WORD len;             /* length 						*/
} rtcp_common;

#define RTCP_COMMON_SIZE 4

#define RTCP_VERSION(com) ((com).v)
#define SET_RTCP_VERSION(com, val) ((com).v = (val))

#define RTCP_P(com) ((com).p)
#define SET_RTCP_P(com, val) ((com).p = (val))

#define RTCP_RC(com) ((com).rc)
#define SET_RTCP_RC(com, val) ((com).rc = (UCHAR)(val))

typedef struct {               /* SR-specific fields -- fixed part */
  DWORD ssrc;                /* SSRC of sender */
  DWORD ntp_secs;            /* NTP timestamp -- integer part */
  DWORD ntp_frac;            /* NTP timestamp -- fractional part */
  DWORD rtp_stamp;           /* RTP timestamp */
  DWORD pkt_count;           /* Sender's packet count */
  DWORD oct_count;           /* Sender's octet count */
} rtcp_sr;

#define RTCP_SR_SIZE (6 * 4)

typedef struct {
  DWORD ssrc;                /* SSRC of sender */
} rtcp_rr;

#define RTCP_RR_SIZE 4

typedef struct {          /* SDES item */
  DWORD ssrc;           /* SSRC of the member described by the SDES item */
  int chunkno;            /* The index of the SDES chunk */
  UINT8 type;            /* SDES item type */
  UINT8 len;             /* SDES item length */
  UCHAR *description;      /* SDES item description (255 char limit) */
  int offset;             /* Describes the position of the type field
                             octet within int32: for internal use of
                             iterator */
} rtcp_sdes_item;

#define RTP_REPORT_BLOCK_MAX_NUM 1

/* An RTCP packet.  This will consist of pointers into
 * the actual packet. */
typedef struct {                     /* RTCP Packet */
  rtcp_common *common;               /* Common headers */

  union {
    struct {                         	/* SR specific fields */
      rtcp_sr *sr_fixed;
      rtcp_report_block *rblock[RTP_REPORT_BLOCK_MAX_NUM]; 	/* report blocks */
      UCHAR* extension;
    } sr;

    struct {                         	/* RR specific fields */
      rtcp_rr *rr_fixed;
      rtcp_report_block *rblock[RTP_REPORT_BLOCK_MAX_NUM]; 	/* report blocks */
      UCHAR* extension;
    } rr;

    struct {
      UCHAR *remainder;
    } sdes;        

    struct {
      UCHAR *data;
    } unknown;

  } variant;
} rtcp_packet;


/* The minimum time in which an RTCP packet is allowed to be sent 
   (note: this is the value before being multiplied by the random
   factor) */
#define _RTCP_MIN_TIME 60


/* Default bandwidth in Kb / sec */
#define _RTP_DEFAULT_BANDWIDTH 720



/* RTCP bandwidth fraction (default is 5%)*/
#define _RTP_DEFAULT_RTCP_FRAC 0.05



/* Fraction of RTCP bandwidth to be shared among active senders */
#define _RTP_DEFAULT_SENDER_BW_FRAC 0.25


/* Which sub-parts of an RTCP packet we want to send. */
typedef enum {
  RTCP_SUBPARTS_ALL,
  RTCP_SUBPARTS_RRSR,
  RTCP_SUBPARTS_SDES,
  RTCP_SUBPARTS_SR,
  RTCP_SUBPARTS_RR
} rtcp_subparts_t;

#endif

/********************************************************************************************/
/*																							*/
/*				stream context for report service											*/
/*																							*/
/********************************************************************************************/
typedef struct AVDTP_REPORT_CONTEXT_STRUCT {
	UINT8				context_id;
	UINT8				context_type;	/* sender or receiver (SRC or SNK) */
	/* Contributors */
  	WORD				CSRClen;		/* The number of sources that contribute locally in the next packet */	  
	DWORD 				*CSRCList;		/* The SSRCs for these locally contributing sources in the next packet */

	/* The header extension */
 	RTP_HDR_EXT			*hdr_extension;

	/* The comparison in rates between NTP timestamp and RTP timestamp.
     A value of n means that each increment of an RTP timestamp
     corresponds to n microseconds. */
#ifdef CONFIG_AVDTP_REPORT_SERVICE     
	INT32 profileRTPTimeRates[_RTP_MAX_PAYLOAD_TYPES];
#endif

	/**********************
	 *	For sender/SRC       *
	 **********************/
	DWORD				sender_ssrc;
#ifdef CONFIG_AVDTP_REPORT_SERVICE
	AVDTP_rtcp_sdes_item *sender_sdes_info[_RTP_NUM_SDES_TYPES];

		/* updated every RTP packet is sent */
	DWORD 				sending_pkt_count; 
  	DWORD 				sending_octet_count;
#endif

	DWORD 				init_rtp_timestamp; 
	DWORD 				time_elapsed; 
	DWORD 				init_seq_no;
	DWORD 				seq_no;

#ifdef CONFIG_AVDTP_REPORT_SERVICE
		/* updated from each RR packet */
	double 				packet_loss_rate;
	double				round_trip_time;
	double				interarrival_jitter;
#endif

	/**********************
	 *	For receiver/SNK   *
	 **********************/
	DWORD				receiver_ssrc;

#ifdef CONFIG_AVDTP_REPORT_SERVICE
		/* obtained from SR packet */
	DWORD 				rtp_timestamp; 
	NTP64 				ntp_timestamp;
	DWORD 				pkt_count; 
	DWORD 				oct_count;
	AVDTP_TIMEVAL 		last_sr_receipt_time;
		/* updated from each RTP packet */
	double 				jitter;
	DWORD 				prev_sndtime;  
	AVDTP_TIMEVAL 		prev_rcvtime; 
	DWORD 				num_pkts_recd_since_last;
	DWORD 				num_pkts_recd;
	DWORD 				first_seqno; 
	DWORD 				highest_seqno;
	DWORD 				expected_prior;
		/* obtained from SDES */
	AVDTP_rtcp_sdes_item *receiver_sdes_info[_RTP_NUM_SDES_TYPES];

	/**********************
	   * RTCP interval info *
	   * used to calculate  *
	   * the RTCP send rate *
	   **********************/
	AVDTP_TIMER_ID		rtcp_timer;
	float 				session_bandwidth;
	float 				rtcp_fraction;
	float 				sender_bw_fraction;
	AVDTP_TIMEVAL 		last_rtcp_send;
	BOOL 				initial;
	WORD				last_pkt_size;
	double 				avg_rtcp_size;
	double 				random_factor;

	WORD				rtcp_count;
#endif	
} AVDTP_REPORT_CONTEXT;

/********************************************************************************************/
/*																							*/
/*				Stream Manager User Data for SM FSM											*/
/*																							*/
/********************************************************************************************/
typedef struct AVDTP_SM_USERDATA_STRUCT {
	struct BtList 	*sep_list;		/* struct AVDTP_SEPInfoStru  */
	struct BtList 	*bd_list;		/* AVDTP_BD_CONTEXT 	*/

	/* each supported transport capabilities within present version */
	WORD				service_category_mask; /* report, recovery, ROHC, multiplexing */
	struct AVDTP_RecoveryParam 			*recovery;
	struct AVDTP_HeaderCompressionParam *hc;
	struct AVDTP_MultiplexingParam		*mp;

	
	AVDT_CBFunc 		cb;
} AVDTP_SM_USERDATA;


/********************************************************************************************/
/*																							*/
/*				Stream SEP User Data for SC FSM												*/
/*																							*/
/********************************************************************************************/
typedef struct AVDTP_SEPInfoStru {
	UINT8			seid;				/* sep identifier (allocated by GAVDP) 			*/
	WORD			stream_handle;		/* sep stream handle (allocated by AVDTP at initial time)    */
	struct FsmInst*  fsm_inst;			/* sep fsm instance 							*/
}AVDTP_SEPInfo;


typedef struct AVDTP_CidContextStru {
	WORD			media_cid;			/* media channel cid (l2cap) 	*/
	WORD			report_cid;			/* report channel cid (l2cap)	*/
	WORD			recovery_cid;		/* recovery channel cid (l2cap)	*/

			/* remote mtu which can be accepted by remote device. */
	WORD			media_mtu;			
	WORD			report_mtu;
	WORD			recovery_mtu;

			/* 
			 * local mtu which can be accepted by local device. these local mtus are pre-set 
			 * when SEP is registered. 
			 */
	/*WORD			local_media_mtu;
	WORD			local_report_mtu;
	WORD			local_recovery_mtu;*/
	struct AVDTP_ChannelConfigParamStru media_param;
	struct AVDTP_ChannelConfigParamStru report_param;
	struct AVDTP_ChannelConfigParamStru recovery_param;

}AVDTP_CidContext;


/* 
 * TSID is assigned by the AVDTP entity of the INT 
 * and shall be adopted by the AVDTP entity of the ACP.   
 * ----- Connection-Local (with bd_addr) !!! 
 */
	 
typedef struct AVDTP_SEP_USERDATA_STRUCT {
	/************  basic info 	******************/
	UINT8				seid;			/* identifier */	
	UINT8				sep_type;		/* src, or snk */	
	UINT8				media_type;		/* Audio, or video */	
	UINT8				remote_seid;	/* peer seid */
	WORD				stream_handle;	/* stream handle associated with this sep */

	AVDTP_BD_CONTEXT 	*bd_context;	/* bd context including some info */

				/* only a transaction at a time, no concurrency for specific stream */
	AVDTP_TRANSACTION	*transaction;	/* sep transaction within bd_context transaction list */
	
	/*********** Configured Transport service capabilities  **************/
	WORD				service_category_mask; /* report, recovery, ROHC, multiplexing */
	struct AVDTP_RecoveryParam 			*recovery;
	struct AVDTP_HeaderCompressionParam *hc;
	struct AVDTP_MultiplexingParam		*mp;	
	
				/* channel cid allocated by L2CAP */
	AVDTP_CidContext 		*cid_context;

	UINT8			need_to_establish_new_channel;

				/* 
				 * reserved buf <size = sizeof(struct BuffStru) + MTU> 
				 * for media transfer only in COMMON <not multiplexing> mode 
				 */
	/*struct BuffStru					*media_buf;*/		/* only for SRC */
	
	/*********** packet buffer for receiving  **************/
	struct BtList	*packet_list;		/* AVDTP_MediaPacket */	

	/*********** report context ***********/	
	struct AVDTP_REPORT_CONTEXT_STRUCT *report_context;

}AVTDP_SEP_USERDATA;

typedef struct AVDTP_MediaPacketStru {
	struct BuffStru *data;
	WORD	length;
	DWORD	time_info;
	UINT8	payload_type;
	UINT8	marker;
	UINT8	reliability;	/* successfully-transferred, successfully-transferred with previously lost, recovered,   */
}AVDTP_MediaPacket;


/***************************************************************************
						Global variables 
***************************************************************************/
extern struct FsmInst * avdtp_fsminst;

#ifdef CONFIG_DEBUG
#ifndef assert
void assert(BOOL exp);
#endif
#else
#define assert(x)	do {} while (0)
#endif

int AVDTP_SM_Init(AVDTP_SM_USERDATA * user_data);
void AVDTP_SM_Done(AVDTP_SM_USERDATA *user_data, struct FsmInst *fsm_inst);


void AVDTP_L2CAPConnectInd(struct L2CAConnIndStru *ind);
void AVDTP_L2CAPConfigInd(struct L2CAConfigIndStru *ind);
void AVDTP_L2CAPDisconnectInd(WORD *ind);
void AVDTP_L2CAPDataInd(struct DataIndStru	*ind);

/***************************************************************************
						RTP functions 
***************************************************************************/
WORD RTP_GetRTPHeaderLength(AVDTP_REPORT_CONTEXT *report_context);
UINT8 RTP_BuildRTPHeader(AVTDP_SEP_USERDATA *user_data, DWORD ts, UINT8 marker, UINT8 pti, UINT8 padding, WORD payload_len, UCHAR *buffer, WORD *buf_len);			   
UINT8 RTP_BuildRTPPacket(AVTDP_SEP_USERDATA *user_data, struct AVDTP_WriteReqInStru *in, UCHAR *media_buf, WORD buf_len);
UINT8 RTP_GetRTPPacket(UCHAR *rtppacket, WORD pktlen, RTP_PACKET *the_packet);


#ifdef CONFIG_AVDTP_REPORT_SERVICE
void RTP_UpdateInfoByRTPPacket(AVDTP_REPORT_CONTEXT *the_context, RTP_PACKET *rtp_pkt);
UINT8 RTCPPacketReceived(AVDTP_REPORT_CONTEXT *the_context, UCHAR *rtp_pkt_stream, WORD len);
UINT8 RTP_BuildRTCPPacket(AVTDP_SEP_USERDATA *user_data, rtcp_subparts_t subparts,
			    WORD padding_block, UCHAR *buffer, WORD *length, AVDTP_TIMEVAL *rtcp_delay);
DWORD Timeval2MS(AVDTP_TIMEVAL *v);
#endif

/***************************************************************************
						help functions
***************************************************************************/
void SEP_InitUserData(AVTDP_SEP_USERDATA *user_data, struct AVDTP_SEPRegisterInStru *sep, WORD stream_handle);
struct FsmInst *SEP_Create(struct AVDTP_SEPRegisterInStru *sep, WORD stream_handle);
void SEP_Destruct(AVTDP_SEP_USERDATA *user_data, struct FsmInst *fsm_inst);
void SEP_Reset(AVTDP_SEP_USERDATA *user_data, struct FsmInst *fsm_inst);
void SEP_SetRemoteSeid(AVTDP_SEP_USERDATA *user_data, UINT8 acp_seid);
UINT8 SEP_GetRemoteSeid(AVTDP_SEP_USERDATA *user_data);
void SEP_SetBDContext(AVTDP_SEP_USERDATA *user_data, struct FsmInst *fi, AVDTP_BD_CONTEXT *bd_context);
AVDTP_BD_CONTEXT *SEP_GetBDContext(AVTDP_SEP_USERDATA *user_data);
void SEP_SetTransaction(AVTDP_SEP_USERDATA* user_data, AVDTP_TRANSACTION *t);
AVDTP_TRANSACTION *SEP_GetTransaction(AVTDP_SEP_USERDATA* user_data);
WORD SEP_GetStreamHandle(AVTDP_SEP_USERDATA* user_data);
UINT8 SEP_NeedWaitForChannelDisconnect(AVTDP_SEP_USERDATA* user_data);
UINT8 SEP_StoreConfigParam(AVTDP_SEP_USERDATA *user_data, UINT8 local, 
									struct AVDTP_ConfigParamStru *config_param, 
									struct FsmInst *fi, UINT8 *error_code);
void SEP_ResetConfigParam(AVTDP_SEP_USERDATA *user_data);
UINT8 SEP_SetMultiplexNode(AVTDP_SEP_USERDATA *user_data, struct FsmInst *fi);
UINT8 SEP_AllocateMultiplexContext(AVTDP_SEP_USERDATA *user_data, struct FsmInst *fi);
UINT8 SEP_DisconnectChannel(AVTDP_SEP_USERDATA *user_data);
void SEP_ResetCidContext(AVTDP_SEP_USERDATA *user_data);
AVDTP_TRANSACTION *SEP_ConstructTransaction(AVTDP_SEP_USERDATA *user_data, UINT8 signal_id, UINT8 direction, struct FsmInst *fi);
void SEP_DestructTransaction(AVTDP_SEP_USERDATA *user_data, UINT8 timeout);
UINT8 SEP_NeedWaitForChannelConnect(AVTDP_SEP_USERDATA *user_data);
UCHAR SEP_GetLocalConfigParam(struct FsmInst *fi, struct AVDTP_ChannelConfigParamStru **config_param);

AVDTP_SEPInfo* SEPInfo_New(void);
void SEPInfo_Init(AVDTP_SEPInfo *sep_info, UINT8 seid, WORD stream_handle, struct FsmInst *sep_fsm);
void SEPInfo_Add(struct BtList *sep_list, AVDTP_SEPInfo *sep_info);
void SEPInfo_Delete(struct BtList *sep_list, AVDTP_SEPInfo *sep_info);
AVDTP_SEPInfo *SEPInfo_FindBySeid(struct BtList *sep_list, UINT8 seid);
AVDTP_SEPInfo *SEPInfo_FindByStreamHandle(struct BtList *sep_list, WORD stream_handle);
UCHAR SEPInfo_CheckMultiStreamHandle(struct BtList *sep_list, WORD *stream_handle, WORD num);
AVDTP_BD_CONTEXT *SEPInfo_CheckSameBDContext(struct BtList *sep_list, UINT8 num, WORD *p_stream_handle);

AVDTP_BD_CONTEXT* BDContext_New(void);
UCHAR BDContext_IsValid(AVDTP_BD_CONTEXT *bd_context);
void BDContext_Init(AVDTP_BD_CONTEXT *bd_context, UCHAR *bd_addr, WORD cid, WORD mtu, UCHAR identifier,
						struct AVDTP_ChannelConfigParamStru *param);
void BDContext_Done(AVDTP_BD_CONTEXT *bd_context);
void BDContext_Destruct(AVDTP_BD_CONTEXT *bd_context, UINT8 direction);
void BDContext_Add(struct BtList *bd_list, AVDTP_BD_CONTEXT *bd_context);
void BDContext_Delete(struct BtList *bd_list, AVDTP_BD_CONTEXT *bd_context);
AVDTP_BD_CONTEXT *BDContext_FindByBDAddr(struct BtList *bd_list, UCHAR *bd_addr);
UCHAR *BDContext_FindByCid(struct BtList *bd_list, WORD cid, UINT8 *channel_type);
AVDTP_MultiplexNode *BDContext_FindMultiplexNode(AVDTP_BD_CONTEXT *bd_context, UINT8 tsid);
WORD BDContext_GetCid(AVDTP_BD_CONTEXT *bd_context);
WORD BDContext_GetMtu(AVDTP_BD_CONTEXT *bd_context);
void BDContext_SetMtu(AVDTP_BD_CONTEXT *bd_context, WORD mtu);
UCHAR *BDContext_GetBDAddress(AVDTP_BD_CONTEXT *bd_context);
struct FsmInst *BDContext_FindSEPByNeedChannel(AVDTP_BD_CONTEXT *bd_context);
void BDContext_AddSEP(AVDTP_BD_CONTEXT *bd_context, struct FsmInst *fi);
void BDContext_RemoveSEP(AVDTP_BD_CONTEXT *bd_context, struct FsmInst *fi);

AVDTP_MultiplexPacket *MultiplexPacket_New(void);
void MultiplexPacket_Free(AVDTP_MultiplexPacket *multiplex_pkt);
void MultiplexPacket_Init(AVDTP_MultiplexPacket *multiplex_pkt, 
							UINT8 tsid,
							UINT8 type,
							UINT8 frag,
							struct FsmInst *fi,
							WORD len,
							UCHAR *data);
UINT8 MultiplexPacket_Add(AVDTP_TCID_CONTEXT *tcid_context, AVDTP_MultiplexPacket *multiplex_pkt);

UINT8 MultiplexContext_Allocate(struct AVDTP_MultiplexingParam *multiplex_param, AVDTP_BD_CONTEXT *bd_context, WORD mask);
UINT8 MultiplexContext_CheckMultiplexingParam(WORD mask, struct AVDTP_MultiplexingParam *mp, AVDTP_BD_CONTEXT *bd_context);
AVDTP_COMMON_CID_CONTEXT *CommonCidContext_FindByCid(struct BtList *c_list, WORD cid);

UINT8 TSID_Allocate(AVDTP_BD_CONTEXT *bd_context);
UINT8 TCID_Allocate(AVDTP_BD_CONTEXT *bd_context);
UINT8 TCIDContext_GetTCID(AVDTP_TCID_CONTEXT *tc);
AVDTP_TCID_CONTEXT *TCIDContext_FindAailable(AVDTP_BD_CONTEXT *bd_context);
AVDTP_TCID_CONTEXT *TCIDContext_Allocate(AVDTP_BD_CONTEXT *bd_context, UINT8 tcid, WORD cid, WORD mtu);
void TCIDContext_SetCidAndMtu(AVDTP_TCID_CONTEXT *tcid_context, WORD cid, WORD mtu);
void TCIDContext_AddMultiplexNode(AVDTP_TCID_CONTEXT *tc, UINT8 tsid, UINT8 type, struct FsmInst *fi);
void TCIDContext_RemoveMultiplexNode(AVDTP_BD_CONTEXT *bd_context, UINT8 tcid, UINT8 tsid);
void TCIDContext_Done(AVDTP_BD_CONTEXT *bd_context, AVDTP_TCID_CONTEXT *tcid_context);
AVDTP_MultiplexNode *TCIDContext_FindMultiplexNode(AVDTP_TCID_CONTEXT *tcid_context, UINT8 tsid);
AVDTP_TCID_CONTEXT *TCIDContext_FindByTcid(struct BtList *tcid_context_list, UINT8 tcid);
AVDTP_TCID_CONTEXT *TCIDContext_FindByCid(struct BtList *tcid_context_list, WORD cid);

AVDTP_COMMON_CID_CONTEXT *CommonCidContext_Add(AVDTP_BD_CONTEXT *bd_context, WORD cid, WORD out_mtu, 
									struct FsmInst *fi, UINT8 channel_type);
void CommonCidContext_Delete(AVDTP_BD_CONTEXT *bd_context, WORD cid);

void SetDefaultPayloadRates(AVDTP_REPORT_CONTEXT *the_context);
AVDTP_REPORT_CONTEXT *ReportContext_New(void);
void ReportContext_Init(AVDTP_REPORT_CONTEXT *report_context, UINT8 sep_type);

#ifdef CONFIG_AVDTP_REPORT_SERVICE
void ReportContext_InitRTCPReport(AVDTP_REPORT_CONTEXT *report_context, struct FsmInst *fi);
#endif

void ReportContext_Reset(AVDTP_REPORT_CONTEXT *report_context, struct FsmInst *fi);
void ReportContext_Destruct(AVDTP_REPORT_CONTEXT *report_context, struct FsmInst *fi);

UINT8 Transaction_AllocateLabel(AVDTP_BD_CONTEXT *bd_context);
AVDTP_TRANSACTION *Transaction_New(void);
void Transaction_Init(AVDTP_TRANSACTION *t, UINT8 label, UINT8 signal_id, UINT8 direction, 
						struct FsmInst *fi, AVDTP_BD_CONTEXT *bd_context);
UINT8 Transaction_Compare(AVDTP_TRANSACTION *t, UINT8 label, UINT8 signal_id, UINT8 direction);
AVDTP_TRANSACTION *Transaction_Construct(AVDTP_BD_CONTEXT *bd_context, UINT8 label,
												UINT8 signal_id, UINT8 direction, struct FsmInst *fi);
void Transaction_Destruct(AVDTP_TRANSACTION *t, UINT8 timeout);
void Transaction_Add(struct BtList *transaction_list, AVDTP_TRANSACTION *t);
void Transaction_Delete(struct BtList *transaction_list, AVDTP_TRANSACTION *t);
UINT8 Transaction_AllocateLabel(AVDTP_BD_CONTEXT *bd_context);
UINT8 Transaction_GetLabel(AVDTP_TRANSACTION *t);
UCHAR Transaction_Find(struct BtList *transaction_list, AVDTP_TRANSACTION *t);
AVDTP_TRANSACTION *Transaction_FindByLabelAndDirection(struct BtList *transaction_list, UINT8 label, UINT8 direction);
AVDTP_TRANSACTION *Transaction_FindByLabel(struct BtList *transaction_list, UINT8 label);
AVDTP_TRANSACTION *Transaction_FindBySigalIdDirectionAndFsmInst(struct BtList *transaction_list, UINT8 signal_id, UINT8 direction, struct FsmInst *fsm_inst);
void Transaction_SetParam(AVDTP_TRANSACTION *t, UCHAR *p_seid);
UCHAR *Transaction_GetParam(AVDTP_TRANSACTION *t);

AVDTP_SignalMsg* SignalMsg_New(void);
void SignalMsg_Init(AVDTP_SignalMsg *signal_msg, UINT8 label, UINT8 msg_type, UINT8 signal_id, UINT8 nosp, WORD len, UCHAR* data);
void SignalMsg_Set(AVDTP_SignalMsg *signal_msg, UINT8 label, UINT8 msg_type, UINT8 signal_id, UINT8 nosp, WORD len, UCHAR* data);
void SignalMsg_Reset(AVDTP_SignalMsg *signal_msg);
void SignalMsg_Store(AVDTP_SignalMsg *signal_msg, WORD len, UCHAR* data);
void SignalMsg_EncodeCommonHeader(UCHAR *buf, UINT8 label, UINT8 pkt_type, UINT8 msg_type);
UINT8 SignalMsg_DecodeCommonHeader(UCHAR *buf, WORD length, UINT8 *label, UINT8 *pkt_type, UINT8 *msg_type, UINT8 *signal_id);
void SignalMsg_EncodeSinglePacket(UCHAR *buf, UINT8 label, UINT8 signal_id, UINT8 msg_type);
void SignalMsg_EncodeStartPacket(UCHAR *buf, UINT8 label, UINT8 signal_id, UINT8 msg_type, UINT8 nosp, WORD len, UCHAR *data);
void SignalMsg_EncodeConPacket(UCHAR *buf, UINT8 label, UINT8 msg_type, WORD len, UCHAR *data);
void SignalMsg_EncodeEndPacket(UCHAR *buf, UINT8 label, UINT8 msg_type, WORD len, UCHAR *data);
UINT8 SignalMsg_NumOfSignalPackets(WORD len, WORD mtu);
void SignalMsg_BuildSend(WORD cid, WORD mtu, UINT8 label, UINT8 signal_id, 
								UINT8 msg_type, struct BuffStru *param);
void SignalMsg_Received(AVDTP_BD_CONTEXT *bd_context, struct BuffStru*arg);
void SignalMsg_Process(AVDTP_BD_CONTEXT *bd_context, AVDTP_SignalMsg *signal_msg);

/*****************************************************************************************/
/* signal request */
void SignalMsg_SendUndefinedSignalReq(WORD cid, WORD mtu, 
							UINT8 signal_id, UINT8 label);
void SignalMsg_SendCommonReq(WORD cid, WORD mtu, 
							UINT8 signal_id, UINT8 label, UINT8 acp_seid);
void SignalMsg_SendDiscoverReq(WORD cid, WORD mtu, 
							UINT8 label, UINT8 msg_type);
void SignalMsg_SendGetCapsReq(WORD cid, WORD mtu, 
							UINT8 label, UINT8 acp_seid, UINT8 flag);
void SignalMsg_SendSetConfigurationReq(WORD cid, WORD mtu, 
							UINT8 label, UINT8 flag, struct AVDTP_SetConfigReqInStru *in);
void SignalMsg_SendReConfigurationReq(WORD cid, WORD mtu, 
							UINT8 label, UINT8 acp_seid, struct AVDTP_ReconfigReqInStru *in);
void SignalMsg_SendSecurityControlReq(
	WORD 	cid, 
	WORD 	mtu, 
	UINT8 	label,
	UINT8 	acp_seid,
	WORD	length,
	UCHAR*	security_data);
void SignalMsg_SendRFA(WORD cid, WORD mtu, UINT8 label);

/*****************************************************************************************/
/* signal response */
void SignalMsg_SendCommonRsp(WORD cid, WORD mtu, 
							UINT8 signal_id, UINT8 label,UINT8 err);
void SignalMsg_SendDiscoverRsp(WORD cid, WORD mtu, 
							UINT8 label,UINT8 err, UINT8 sep_num, AVDTP_SEPPrimaryData *sep);
void SignalMsg_SendGetConfigurationRsp(WORD cid, WORD mtu, 
							UINT8 label, UINT8 err, struct AVDTP_ConfigParamStru *in);
void SignalMsg_SendGetCapsRsp(WORD cid, WORD mtu, 
							UINT8 label, UINT8 err, struct AVDTP_ConfigParamStru *in);
void SignalMsg_SendSetReConfigRsp(WORD cid, WORD mtu, 
							UINT8 label, UINT8 signal_id, UINT8 err, UINT8 s_c_fail);
void SignalMsg_SendStartSuspendRsp(WORD cid, WORD mtu, 
							UINT8 label, UINT8 signal_id, UINT8 err, UINT8 acp_seid);
void SignalMsg_SendSecurityControlRsp(
	WORD 	cid, 
	WORD 	mtu, 
	UINT8 	label,
	UINT8 	err,
	WORD	length,
	UCHAR*	security_data);
/*****************************************************************************************/

UCHAR Signal_Id2SepEvent(UINT8 signal_id, UINT8 direction, UINT8 request);
UCHAR Signal_SmEvent2SepEvent(UINT8 sm_event);
UINT8 Signal_SmEvent2CallbackEvent(UINT8 sm_event);

void CommonMsg_Received(AVDTP_COMMON_CID_CONTEXT *common_cid_context, struct BuffStru *arg);
void ALMsg_Received(AVDTP_TCID_CONTEXT *tcid_context, struct BuffStru *arg);




UINT8 Param_InterpretDiscoverRspAccept(UCHAR *data, WORD len, UINT8 *sep_num, 
												AVDTP_SEPPrimaryData **p_acp_sep);
WORD Param_LengthOfServiceCaps(struct AVDTP_ConfigParamStru *config_param);
void Param_PDUSetContentProtection(UCHAR *p, struct AVDTP_ContentProtectionParam *content_protection);
void Param_PDUSetMultiplexing(UCHAR *p, WORD mask, struct AVDTP_MultiplexingParam *mp);

/* param build request */
UINT8 Param_CheckServiceCaps(struct AVDTP_ConfigParamStru *config_param, UINT8 *s_c_fail, UINT8 *error_code);
UINT8 Param_CheckReconfigCaps(struct AVDTP_ConfigParamStru *config_param, UINT8 *s_c_fail, UINT8 *error_code);

struct BuffStru* Param_BuildDiscoverRspAccept(UINT8 sep_num, AVDTP_SEPPrimaryData *p_acp_sep);

struct BuffStru* Param_BuildServiceCaps(WORD offset, struct AVDTP_ConfigParamStru *config_param);
#ifdef CONFIG_CONFORMANCE_TESTER
struct BuffStru* Param_BuildSetConfigurationReq(struct AVDTP_SetConfigReqInStru *in, UINT8 flag);
#else
struct BuffStru* Param_BuildSetConfigurationReq(struct AVDTP_SetConfigReqInStru *in);
#endif

struct BuffStru* Param_BuildReConfigurationReq(struct AVDTP_ReconfigReqInStru *in, UINT8 acp_seid);
struct BuffStru* Param_BuildCommonReq(UINT8 acp_seid);

/* param build response */
struct BuffStru* Param_BuildSetReConfigurationRsp(UINT8 error_code, UINT8 s_c_fail);
struct BuffStru* Param_BuildCapsRsp(struct AVDTP_ConfigParamStru *config_param, UINT8 err);
struct BuffStru* Param_BuildStartSuspendRspReject(UINT8 acp_seid, UINT8 error_code);
struct BuffStru* Param_BuildCommonRspReject(UINT8 error_code);

UINT8 Param_InterpretServiceCaps(UCHAR *data, WORD len, struct AVDTP_ConfigParamStru *config_param,
										UINT8 *s_c_fail, UINT8 is_reconfig);
UINT8 Param_InterpretSetConfigurationReq(UCHAR *data, WORD len, 
												UINT8 *int_seid, UINT8 *acp_seid, 
												struct AVDTP_ConfigParamStru *config_param, 
												UINT8 *s_c_fail);
UINT8 Param_InterpretReConfigurationReq(UCHAR *data, WORD len, 
												UINT8 *acp_seid, 
												struct AVDTP_ConfigParamStru *config_param, 
												UINT8 *s_c_fail);

UINT8 Param_InterpretSetReConfigurationRspReject(UCHAR *data, WORD len, 
													UINT8 *s_c_fail, UINT8 *error_code);
UINT8 Param_InterpretStartSuspendReq(UCHAR *data, WORD len, UINT8 *num, UINT8 **p_acp_seid);
UINT8 Param_InterpretStartSuspendRspReject(UCHAR *data, WORD len, UINT8 *acp_seid, UINT8 *error_code);

UINT8 Param_InterpretCommonReq(UCHAR *data, WORD len, UINT8 *acp_seid);

UINT8 Param_InterpretCommonRspReject(UCHAR *data, UINT8 len, UINT8 *error_code);


struct BuffStru* Param_BuildSecurityControlReq( 
	UINT8 				acp_seid,
	WORD				length,
	UCHAR*				security_data);
struct BuffStru* Param_BuildSecurityControlRsp(
	WORD				length,
	UCHAR*				security_data);


void L2CAP_Channel_Establish(UCHAR *bd_addr);
void AVDTP_L2CAP_Config(WORD cid, struct AVDTP_ChannelConfigParamStru *l2_param, UCHAR *bd, UCHAR side);
void AVDTP_L2CAP_ConnectCfmConfig(struct L2CAConnReqOutStru *ind, struct AVDTP_ChannelConfigParamStru *param);
void AVDTP_L2CAPCallback(WORD msgid, void *arg);
void AVDTP_L2CAP_ParamFree(struct AVDTP_ChannelConfigParamStru *param);
void L2CAP_ConstructConnectRspParam(struct L2CAConnRspInStru *in_param, 
										AVDTP_BD_CONTEXT *bd_context,
										struct AVDTP_ConnectRspInStru *connect_rsp);
void AVDTP_L2CAP_WriteData(WORD cid, struct BuffStru *buf);

AVDTP_MediaPacket *MediaPacket_Construct(struct BuffStru *buf, RTP_PACKET *rtp_pkt, UINT8 reliability);
void MediaPacket_Add(AVTDP_SEP_USERDATA* user_data, AVDTP_MediaPacket *media_pkt);
void MediaPacket_AddHead(AVTDP_SEP_USERDATA* user_data, AVDTP_MediaPacket *media_pkt);
UINT8 MediaPacket_Remove(AVTDP_SEP_USERDATA* user_data, AVDTP_MediaPacket **media_pkt);
void MediaPacket_Destruct(AVDTP_MediaPacket *media_pkt);
UINT8 ServiceCategoryMap_UI2PDU(WORD ui_sc);
void ChannelConfigParam_Free(struct AVDTP_ChannelConfigParamStru *config_param);
void ConfigParam_Free(struct AVDTP_ConfigParamStru *config_param);


WORD AVDTP_htons(WORD src);
DWORD AVDTP_htonl(DWORD src);
WORD AVDTP_ntohs(WORD src);
DWORD AVDTP_ntohl(DWORD src);


void AVDTP_SendPacketEvent2Fsm(struct FsmInst *fi, UCHAR event, UCHAR *data, WORD len);
UCHAR AVDTP_CheckEvent(struct FsmInst *fi, UCHAR event);

#endif
