/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    avdtpuistru.h
    
Abstract:
	This is the ui structures head file which provides the structures 
	in which user interface is interested.
	
Author:
    Lixin Qian

Create Date:
	11/11/2002
	
Revision History: 
	
---------------------------------------------------------------------------*/
#ifndef AVDTP_UI_STRUCT_H
#define AVDTP_UI_STRUCT_H


/********************************/
/*  ERROR CODE					*/
/********************************/

/* Profile accept request from peer side */
#define AVDTP_ERROR_ACCEPT		0

/* ACP to INT, Signal Response Header Error Codes */
/* All messages  The request packet header format error that is not specified above ERROR_CODE */
#define AVDTP_ERROR_BAD_HEADER_FORMAT 0x01

/********** ACP to INT, Signal Response Payload Format Error Codes ****************/
/* All messages  The request packet length is not match the assumed length. */
#define AVDTP_ERROR_BAD_LENGTH 0x11

/* All messages  The requested command indicates an invalid ACP SEID (not addressable) */
#define AVDTP_ERROR_BAD_ACP_SEID 0x12

/* Set Configuration  The SEP is in use */
#define AVDTP_ERROR_SEP_IN_USE 0x13

/* Reconfigure  The SEP is not in use */
#define AVDTP_ERROR_SEP_NOT_IN_USE 0x14

/* Set Configuration Reconfigure    The value of Service Category in the request packet is not defined in AVDTP. */
#define AVDTP_ERROR_BAD_SERV_CATEGORY 0x17

/* All messages  The requested command has an incorrect payload format (Format errors not specified in this ERROR_CODE) */
#define AVDTP_ERROR_BAD_PAYLOAD_FORMAT 0x18

/*All messages  The requested command is not supported by the device */
#define AVDTP_ERROR_NOT_SUPPORTED_COMMAND 0x19 

/* Reconfigure  	The reconfigure command is an attempt to reconfigure a transport service capabilities
					of the SEP. Reconfigure is only permitted for application service capabilities */
#define AVDTP_ERROR_INVALID_CAPABILITIES 0x1A 

/**************ACP to INT, Signal Response Transport Service Capabilities Error Codes *************/
/* Set Configuration  The requested Recovery Type is not defined in AVDTP. */
#define AVDTP_ERROR_BAD_RECOVERY_TYPE 0x22

/* Set Configuration  The format of Media Transport Capability is not correct. */			
#define AVDTP_ERROR_BAD_MEDIA_TRANSPORT_FORMAT 0x23

/* Set Configuration  The format of Report Capability is not correct. */	
#define AVDTP_ERROR_BAD_REPORT_FORMAT	0x24

/* Set Configuration  The format of Recovery Service Capability is not correct. */
#define AVDTP_ERROR_BAD_RECOVERY_FORMAT 0x25

/* Set Configuration  The format of Header Compression Service */
#define AVDTP_ERROR_BAD_ROHC_FORMAT 0x26

/* Set Configuration  The format of Content Protection Service Capability is not correct. */
#define AVDTP_ERROR_BAD_CP_FORMAT 0x27

/* Set Configuration  The format of Multiplexing Service Capability is not correct. */
#define AVDTP_ERROR_BAD_MULTIPLEXING_FORMAT 0x28

/* Set Configuration  Configuration not supported. */
#define AVDTP_ERROR_UNSUPPORTED_CONFIGURAION 0x29 			

/* Set Configuration  The format of Media Codec Capability is not correct. */
#define AVDTP_ERROR_BAD_MEDIA_CODEC_FORMAT 0x02A

/************ ACP to INT, Procedure Error Codes *********************/
/* All messages  Indicates that the ACP state machine is in an invalid state in order to process the signal. */
#define AVDTP_ERROR_BAD_STATE 0x31

#define AVDTP_ERROR_CHANNEL_ESTABLISH_FAIL 0xf0
#define AVDTP_ERROR_RTP_INSUFFICIENT_BUFFER	0xf1
#define AVDTP_ERROR_LEN_EXCEED_MTU				0xf2
#define AVDTP_ERROR_PACKET_LOST					0xf3
#define AVDTP_ERROR_RTP_BAD_VERSION				0xf4
#define AVDTP_ERROR_RTP_BAD_PACKET_LEN			0xf5
#define AVDTP_ERROR_CHANNEL_ESTABLISHING		0xf6

/* Local service call error codes */
#define AVDTP_ERROR_SUCCESS							0
#define AVDTP_ERROR_REJECT							0x0101
#define AVDTP_ERROR_NO_SIGNAL_CHANNEL				0x0102
#define AVDTP_ERROR_NO_SEP							0x0104
#define AVDTP_ERROR_CONFIG_PARAM_CHECK_FAIL			0x0105
#define AVDTP_ERROR_SEP_BUSY						0x0106
#define AVDTP_ERROR_WAIT_HANDLE						0x0107
#define AVDTP_ERROR_SEP_EXIT						0x0108

/* Read stream data request out parameter (spec p134) */
#define AVDTP_ERROR_SUCCESSFUL_TRANSFER	0x0000
#define AVDTP_ERROR_NO_DATA_AVAILABLE	0x0001
#define AVDTP_ERROR_INVALID_STREAM_HANDLE 0x0002
#define AVDTP_ERROR_DATA_RECEIVED_WARNING 0x0003

/* reliability */
#define AVDTP_ERROR_NO_ERROR_DETECTED			0x0000
#define AVDTP_ERROR_MEDIA_PACKET_LOST			0x0001
#define AVDTP_ERROR_MEDIA_PACKET_RECOVERED      0x0002
#define AVDTP_ERROR_MISMATCH_OF_FRAME_LEN	0x0003

/* AVDTP transaction timeout error code */
#define AVDTP_ERROR_TIMEOUT 0xFF			/* Transaction timeout */


/************************************************************************************************/
#ifndef BD_ADDR_LEN
#define BD_ADDR_LEN	 6
#endif

/* media type */
#define AVDTP_MEDIA_TYPE_AUDIO				0
#define AVDTP_MEDIA_TYPE_VIDEO				1
#define AVDTP_MEDIA_TYPE_MULTIMEDIA			2

/* sep type */
#define AVDTP_SEP_TYPE_SRC	0 
#define AVDTP_SEP_TYPE_SNK 1

/* sep status */
#define AVDTP_SEP_NOT_IN_USE	0
#define AVDTP_SEP_IN_USE		1

#ifdef CONFIG_CONFORMANCE_TESTER
/*********************************************************************************************/
/*																							 */
/*						Channel Type & Packet Type for Commformance Tester					 */
/*																							 */
/*********************************************************************************************/
#define AVDTP_TESTER_ERROR_SUCCESS					0

#define AVDTP_TESTER_INVALID_PARAMETER				7

#define AVDTP_TESTER_CHANNEL_TYPE_SIGNAL			1
#define AVDTP_TESTER_CHANNEL_TYPE_COMMON			2
#define AVDTP_TESTER_CHANNEL_TYPE_MULTIPLEX			3

#define AVDTP_TESTER_PACKET_TYPE_MEDIA				4
#define AVDTP_TESTER_PACKET_TYPE_REPORT				5
#define AVDTP_TESTER_PACKET_TYPE_RECOVERY			6


#endif




/*********************************************************************************************/
/*																							 */
/*						Signal Event  (cfm & ind)											 */
/*																							 */
/*********************************************************************************************/

/*Event Type: uint Size: 2 octets*/
/*0x0000 Forbidden*/
#define AVDTP_Event_ConnectReq_Ind 			0x0001
#define AVDTP_Event_ConnectReq_Cfm			0x0002
#define AVDTP_Event_DisconnectReq_Ind		0x0003
#define AVDTP_Event_DisconnectReq_Cfm		0x0004
#define AVDTP_Event_Discover_Ind			0x0005
#define AVDTP_Event_Discover_Cfm			0x0006
#define AVDTP_Event_Get_Capabilities_Ind	0x0007
#define AVDTP_Event_Get_Capabilities_Cfm	0x0008
#define AVDTP_Event_Set_Configuration_Ind	0x0009
#define AVDTP_Event_Set_Configuration_Cfm	0x000A
#define AVDTP_Event_Get_Configuration_Ind	0x000B
#define AVDTP_Event_Get_Configuration_Cfm	0x000C
#define AVDTP_Event_Open_Ind				0x000D
#define AVDTP_Event_Open_Cfm				0x000E
#define AVDTP_Event_Close_Ind				0x000F
#define AVDTP_Event_Close_Cfm				0x0010
#define AVDTP_Event_Start_Ind				0x0011
#define AVDTP_Event_Start_Cfm				0x0012
#define AVDTP_Event_Suspend_Ind				0x0013
#define AVDTP_Event_Suspend_Cfm				0x0014
#define AVDTP_Event_ReConfigure_Ind			0x0015
#define AVDTP_Event_ReConfigure_Cfm			0x0016
#define AVDTP_Event_Security_Control_Ind	0x0017
#define AVDTP_Event_Security_Control_Cfm	0x0018
#define AVDTP_Event_Abort_Ind				0x0019
#define AVDTP_Event_Abort_Cfm				0x001A

#define AVDTP_Event_Read_Cfm				0x001B
#define AVDTP_Event_Write_Cfm				0x001C
#define AVDTP_Event_GetTCMtu_Cfm			0x001D
#define AVDTP_Event_Data_Ind				0x001E
#define AVDTP_Event_Remote_TCMTU_Ind		0x001F

#define AVDTP_Event_Number					(AVDTP_Event_Remote_TCMTU_Ind-AVDTP_Event_ConnectReq_Ind+1)

/* Other RFD */


/*********************************************************************************************/
/*																							 */
/*						SEP register and unregister 										 */
/*																							 */
/*********************************************************************************************/

struct AVDTP_ChannelConfigParamStru {
	WORD			in_mtu;
	WORD 			flush_to;
  	WORD 			link_to;
  	struct QosStru 	*flow;
};

/* SEP register info */
struct AVDTP_SEPRegisterInStru {
	UINT8 seid; 
	UINT8 sep_type;
	UINT8 media_type;	

	struct AVDTP_ChannelConfigParamStru *media_param;
	struct AVDTP_ChannelConfigParamStru *report_param;
	struct AVDTP_ChannelConfigParamStru *recovery_param;  	
};

/*********************************************************************************************/
/*																							 */
/*						GetTransportChannelMtu Out Param Structure							 */
/*																							 */
/*********************************************************************************************/
struct AVDTP_TransportChannelMtuStru {
	WORD	stream_handle;
	WORD	result;				/* error code */
	
			/* remote mtu which can be accepted by remote device. */
	WORD	media_mtu;			/* media transport channel */
	WORD	report_mtu;			/* report transport channel */
	WORD	recovery_mtu;		/* recovery transport channel */

			/* 
			 * local mtu which can be accepted by local device. these local mtus are pre-set 
			 * when SEP is registered. 
			 */
	WORD	local_media_mtu;
	WORD	local_report_mtu;
	WORD	local_recovery_mtu;
};


/*********************************************************************************************/
/*																							 */
/*						Request 															 */
/*																							 */
/*********************************************************************************************/

/* Signal Channel Connect */
struct AVDTP_ConnectReqInStru {
	UCHAR	bd_addr[BD_ADDR_LEN];
	struct AVDTP_ChannelConfigParamStru *conf_param;	/* if NULL, DEFAULT value is used */	
};

/* Get capabilities request in structure */
struct AVDTP_GetCapsReqInStru {
	UCHAR	bd_addr[BD_ADDR_LEN];
	UINT8 	acp_seid;
};

/* service capabilities category and its parameter */
#define AVDTP_SERVICE_CATEGORY_MEDIA_TRANSPORT 				0x0001
#define AVDTP_SERVICE_CATEGORY_REPORT 				 		0x0002
#define AVDTP_SERVICE_CATEGORY_RECOVERY				 		0x0004
#define AVDTP_SERVICE_CATEGORY_CONTENT_PROTECTION 			0x0008
#define AVDTP_SERVICE_CATEGORY_HEADER_COMPRESSION			0x0010
#define AVDTP_SERVICE_CATEGORY_MULTIPLEXING					0x0020
#define AVDTP_SERVICE_CATEGORY_MEDIA_CODEC					0x0040
#define AVDTP_SERVICE_CATEGORY_NOT_DEFINED					0x0080

/* service category set MACROs */
#define AVDTP_SET_MEDIA_TRANSPORT(mask)			((mask) |= AVDTP_SERVICE_CATEGORY_MEDIA_TRANSPORT)
#define AVDTP_SET_REPORT(mask)					((mask) |= AVDTP_SERVICE_CATEGORY_REPORT)
#define AVDTP_SET_RECOVERY(mask)				((mask) |= AVDTP_SERVICE_CATEGORY_RECOVERY)
#define AVDTP_SET_CONTENT_PROTECTION(mask)  ((mask) |= AVDTP_SERVICE_CATEGORY_CONTENT_PROTECTION)
#define AVDTP_SET_HEADER_COMPRESSION(mask)		((mask) |= AVDTP_SERVICE_CATEGORY_HEADER_COMPRESSION)
#define AVDTP_SET_MULTIPLEXING(mask)			((mask) |= AVDTP_SERVICE_CATEGORY_MULTIPLEXING)
#define AVDTP_SET_MEDIA_CODEC(mask)				((mask) |= AVDTP_SERVICE_CATEGORY_MEDIA_CODEC)
#define AVDTP_SET_NOT_DEFINED(mask)				((mask) |= AVDTP_SERVICE_CATEGORY_NOT_DEFINED)

/* service category clear MACROs */
#define AVDTP_CLEAR_MEDIA_TRANSPORT(mask)			((mask) &= ~AVDTP_SERVICE_CATEGORY_MEDIA_TRANSPORT)
#define AVDTP_CLEAR_REPORT(mask)					((mask) &= ~AVDTP_SERVICE_CATEGORY_REPORT)
#define AVDTP_CLEAR_RECOVERY(mask)					((mask) &= ~AVDTP_SERVICE_CATEGORY_RECOVERY)
#define AVDTP_CLEAR_CONTENT_PROTECTION(mask)  	((mask) &= ~AVDTP_SERVICE_CATEGORY_CONTENT_PROTECTION)
#define AVDTP_CLEAR_HEADER_COMPRESSION(mask)		((mask) &= ~AVDTP_SERVICE_CATEGORY_HEADER_COMPRESSION)
#define AVDTP_CLEAR_MULTIPLEXING(mask)				((mask) &= ~AVDTP_SERVICE_CATEGORY_MULTIPLEXING)
#define AVDTP_CLEAR_MEDIA_CODEC(mask)				((mask) &= ~AVDTP_SERVICE_CATEGORY_MEDIA_CODEC)
#define AVDTP_CLEAR_NOT_DEFINED(mask)				((mask) &= ~AVDTP_SERVICE_CATEGORY_NOT_DEFINED)

/* service category support(?) MACROs */
#define AVDTP_SUPPORT_MEDIA_TRANSPORT(mask)				((mask) & AVDTP_SERVICE_CATEGORY_MEDIA_TRANSPORT)
#define AVDTP_SUPPORT_REPORT(mask)						((mask) & AVDTP_SERVICE_CATEGORY_REPORT)
#define AVDTP_SUPPORT_RECOVERY(mask)					((mask) & AVDTP_SERVICE_CATEGORY_RECOVERY)
#define AVDTP_SUPPORT_CONTENT_PROTECTION(mask)  		((mask) & AVDTP_SERVICE_CATEGORY_CONTENT_PROTECTION)
#define AVDTP_SUPPORT_HEADER_COMPRESSION(mask)			((mask) & AVDTP_SERVICE_CATEGORY_HEADER_COMPRESSION)
#define AVDTP_SUPPORT_MULTIPLEXING(mask)				((mask) & AVDTP_SERVICE_CATEGORY_MULTIPLEXING)
#define AVDTP_SUPPORT_MEDIA_CODEC(mask)					((mask) & AVDTP_SERVICE_CATEGORY_MEDIA_CODEC)
#define AVDTP_SUPPORT_NOT_DEFINED(mask)					((mask) & AVDTP_SERVICE_CATEGORY_NOT_DEFINED)

struct AVDTP_RecoveryParam {
	UINT8	type;
	UINT8	mrws;
	UINT8	mnmp;
};
struct AVDTP_MediaCodecParam {
	UINT8	media_type;
	UINT8	media_codec_type;
	WORD	length;
	UCHAR	*media_codec_info;
};

struct AVDTP_ContentProtectionParam {	
	WORD	type;
	WORD	length;
	UCHAR	*cp_value;
};

struct AVDTP_HeaderCompressionParam {
	UINT8	back_channel;
	UINT8	media;
	UINT8	recovery;
};
struct AVDTP_MultiplexingParam {
	UINT8	frag;
	UINT8	media_tsid;
	UINT8	media_tcid;
	UINT8	report_tsid;
	UINT8	report_tcid;
	UINT8	recovery_tsid;
	UINT8	recovery_tcid;
};

struct AVDTP_ConfigParamStru {
	WORD	service_category_mask;
		/* media transport and report has no param */
		
	UINT8								num_of_codec;
	UINT8								num_of_cp;		/* MAY be multiple in GET_CAPS_RSP */
	
	struct AVDTP_RecoveryParam 			*recovery;	
	struct AVDTP_MediaCodecParam 		*media_codec;
	struct AVDTP_ContentProtectionParam	*cp;	
	struct AVDTP_HeaderCompressionParam *hc;
	struct AVDTP_MultiplexingParam		*mp;
};

/* set configuration */
struct AVDTP_SetConfigReqInStru {
	UCHAR	bd_addr[BD_ADDR_LEN];		/* remote device address */
	UINT8	acp_seid;					/* remote sep id */
	UINT8	int_seid;					/* local sep id */
	struct AVDTP_ConfigParamStru *config_param;
};

/* start and suspend request in structure */
struct AVDTP_StartSuspendReqInStru {
	UINT8	num;				/* # of stream handle */
	WORD	p_stream_handle[1]; /* pointer to the array of stream handle */
};


/* reconfigure application service capabilities (media codec and content protection) */
struct AVDTP_ReconfigReqInStru {
	WORD stream_handle;
	struct AVDTP_ConfigParamStru *config_param;	/* only app caps */
};

/* write stream data */
struct AVDTP_WriteReqInStru {
	WORD 	stream_handle;
	WORD	length;
	UCHAR	*out_buffer; 
	DWORD	time_info;
	UINT8	payload_type;
	UINT8	marker;
};

/* read request in structure */
struct AVDTP_ReadReqInStru {
	WORD 	stream_handle;
	WORD	exp_length;
	UCHAR	*in_buffer; 
};


/* security control request in structure */
struct AVDTP_SecurityControlReqInStru {
	WORD 	stream_handle;
	WORD	length;
	UCHAR	security_data[1];
};


/*********************************************************************************************/
/*																							 */
/*						Confirm 															 */
/*																							 */
/*********************************************************************************************/
/* common cfm structure (open/close/abort/write) */
struct AVDTP_CommonCfmStru {
	WORD		stream_handle;
	WORD		error_code;		/* or result, rsp */
};

/* Connect cfm structure */
struct AVDTP_ConnectCfmStru {
	UCHAR 	bd_addr[BD_ADDR_LEN];
	
	WORD	connect_result;
	WORD	config_result;
	WORD	status;
};

/* Discover cfm structure */
typedef struct AVDTP_SEPPrimaryDataStru {
	UINT8	seid;
	UINT8	in_use;
	UINT8	media_type;
	UINT8	sep_type;	/* AVDTP_SEP_TYPE_SRC or AVDTP_SEP_TYPE_SNK */
}AVDTP_SEPPrimaryData;

struct AVDTP_DiscoverCfmStru {
	UCHAR 	bd_addr[BD_ADDR_LEN];
	
	WORD	error_code;
	UINT8	sep_num;
	AVDTP_SEPPrimaryData *p_acp_sep;	/* AVDTP_SEPPrimaryDataStru */
};

/* Get capabilities cfm structure */
struct AVDTP_GetCapsCfmStru {
	UCHAR 	bd_addr[BD_ADDR_LEN];
	UINT8	acp_seid;
	
	WORD	error_code;
	struct AVDTP_ConfigParamStru *config_param;
};

/* set configuration cfm structure */
struct AVDTP_SetConfigurationCfmStru {
	UCHAR	bd_addr[BD_ADDR_LEN];		/* remote device address */
	UINT8	acp_seid;					/* remote sep id */
	UINT8	int_seid;					/* local sep id */
		
	WORD	error_code;					/* or rsp */
	WORD 	stream_handle;				/* return on success */
	WORD	first_category_to_fail;		/* sevice category mask */
};

/* start or suspend cfm structure */
struct AVDTP_StartSuspendCfmStru {		
	WORD	error_code;
	WORD	first_failing_stream_handle;

#if 0
	UINT8	num;				/* # of stream handle */
	WORD	p_stream_handle[1]; /* pointer to the array of stream handle */
#endif
};

/* disconnect cfm structure */
/*struct AVDTP_DisconnectCfmStru {
	UCHAR 	bd_addr[BD_ADDR_LEN];
	WORD	result;
};*/

/* get configuration cfm structure */
struct AVDTP_GetConfigurationCfmStru {	
	WORD		stream_handle;
	
	WORD		error_code;
	struct AVDTP_ConfigParamStru *config_param;
};

/* security control cfm structure */
struct AVDTP_SecurityControlCfmStru {
	WORD		stream_handle;
	
	WORD		error_code;
	WORD		length;
	UCHAR 		security_data[1];
};

/* reconfigure cfm structure */
struct AVDTP_ReconfigCfmStru {
	WORD	stream_handle;
	
	WORD	error_code;
	WORD	first_category_to_fail;	/* sevice category mask */
};

/* read cfm structure */
struct AVDTP_ReadCfmStru {
	WORD	stream_handle;
	
	WORD	result;
	WORD	length;
	DWORD	time_info;
	UINT8	payload_type;
	UINT8	marker;
	WORD	reliability;	/* packet lost is indicated here */
};

/* write cfm structure */
struct AVDTP_WriteCfmStru {
	WORD	stream_handle;	
	WORD	result;
};

/*********************************************************************************************/
/*																							 */
/*						Indication															 */
/*																							 */
/*********************************************************************************************/
/* common indication structure */
struct AVDTP_CommonIndStru {
	UINT8	transaction;
	WORD	stream_handle;
};

/* discover indication structure */
struct AVDTP_DiscoverIndStru {
	UINT8	transaction;
	UCHAR	bd_addr[BD_ADDR_LEN];
};

/* get capabilities indication structure */
struct AVDTP_GetCapsIndStru {
	UINT8	transaction;
	UINT8	acp_seid;
	UCHAR	bd_addr[BD_ADDR_LEN];
};

/* set configuration indication structure */
struct AVDTP_SetConfigurationIndStru {
	UINT8	transaction;
	UINT8	acp_seid;
	UCHAR	bd_addr[BD_ADDR_LEN];
	UINT8 	int_seid;
	WORD	stream_handle;
	struct AVDTP_ConfigParamStru *config_param;
};

/* start or suspend indication structure */
struct AVDTP_StartSuspendIndStru {
	UINT8		transaction;
	UINT8		num;	/* # of stream handle */
	WORD		p_stream_handle[1];
};


/* security control indication structure */
struct AVDTP_SecurityControlIndStru {
	UINT8	transaction;
	WORD	stream_handle;
	WORD	length;
	UCHAR	security_data[1];
};

/* reconfigure indication structure */
struct AVDTP_ReconfigureIndStru {
	UINT8	transaction;
	WORD	stream_handle;
	struct AVDTP_ConfigParamStru *config_param;	/* only app caps */
};

struct AVDTP_DataIndStru {
	WORD	stream_handle;
	struct BuffStru *data;
	WORD 		length;
	DWORD		time_info;
	UINT8		payload_type;
	UINT8		marker;
	UINT8		reliability;
};
/*********************************************************************************************/
/*																							 */
/*						Response															 */
/*																							 */
/*********************************************************************************************/

/* avdtp common response in structure */
struct AVDTP_CommonRspInStru {
	WORD 	stream_handle;
	UINT8 	transaction;	
	UINT8	error_code;
};

/* connect response in structure  */
struct AVDTP_ConnectRspInStru {
	UCHAR	bd_addr[BD_ADDR_LEN];
	WORD 	connect_result;
	WORD 	status;
	struct AVDTP_ChannelConfigParamStru *conf_param;
};

/* discover response in structure */
struct AVDTP_DiscoverRspInStru {
	UINT8 			transaction;
	UINT8			error_code;
	UCHAR			bd_addr[BD_ADDR_LEN];
	UINT8			sep_num;
	AVDTP_SEPPrimaryData *p_acp_sep;	/* sep primary data (InUse, MediaType, SEPType) */	
};

/* get capabilities response in structure */
struct AVDTP_GetCapabilitiesRspInStru {
	UINT8 	transaction;
	UINT8	error_code;
	UCHAR	bd_addr[BD_ADDR_LEN];
	struct AVDTP_ConfigParamStru *config_param;	
};

/* set configuration response in structure */
struct AVDTP_SetConfigurationRspInStru {
	UINT8 	transaction;
	UINT8	error_code;
	WORD 	stream_handle;
	WORD	first_category_to_fail;	/* sevice category mask */	
};


/* start or suspend response in structure */
struct AVDTP_StartSuspendRspInStru {
	UINT8 	transaction;
	UINT8	error_code;
	WORD 	first_stream_handle;	/* if error_code != 0,  then it represents first failing stream handle */
};


/* get configuration response in structure */
struct AVDTP_GetConfigurationRspInStru {
	WORD 	stream_handle;
	UINT8 	transaction;
	UINT8	error_code;
	struct AVDTP_ConfigParamStru *config_param;	/* Configured capabilities */	
};

/* security control response in structure */
struct AVDTP_SecurityControlRspInStru {
	WORD 	stream_handle;
	UINT8 	transaction;
	UINT8	error_code;	
	WORD	length;
	UCHAR	security_data[1];
};


/* reconfig response in structure */
struct AVDTP_ReconfigRspInStru {
	WORD 	stream_handle;
	UINT8 	transaction;	
	UINT8	error_code;
	WORD	first_category_to_fail;	/* sevice category mask */
};
	

/*********************************************************************************************/
/*																							 */
/*						Cfm & Ind callback function definition								 */
/*																							 */
/*********************************************************************************************/

typedef void (*AVDT_CBFunc) (WORD, void*);

#endif
