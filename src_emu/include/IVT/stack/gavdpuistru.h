/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    gavdpuistru.h
    
Abstract:
	This is the ui structures head file which provides the structures 
	in which user interface is interested.
	
Author:
    Chen Zhongyi

Create Date:
	11/12/2002
	
Revision History:
	
---------------------------------------------------------------------------*/
#ifndef GAVDP_UI_STRUCT_H
#define GAVDP_UI_STRUCT_H

/********************************************************************************************/
/*																							*/
/*						Exposed constants 														*/
/*																							*/
/********************************************************************************************/

/*SEP state definition*/
#define GAVDP_SEP_STATE_IDLE				0 
#define GAVDP_SEP_STATE_OPEN				1
#define GAVDP_SEP_STATE_STREAMING		2

/* media type */
#define GAVDP_MEDIA_TYPE_AUDIO			AVDTP_MEDIA_TYPE_AUDIO
#define GAVDP_MEDIA_TYPE_VIDEO			AVDTP_MEDIA_TYPE_VIDEO
#define GAVDP_MEDIA_TYPE_MULTIMEDIA		AVDTP_MEDIA_TYPE_MULTIMEDIA

/* sep type */
#define GAVDP_SEP_TYPE_SRC					AVDTP_SEP_TYPE_SRC
#define GAVDP_SEP_TYPE_SNK				AVDTP_SEP_TYPE_SNK

/* sep status */
#define GAVDP_SEP_NOT_IN_USE				AVDTP_SEP_NOT_IN_USE
#define GAVDP_SEP_IN_USE					AVDTP_SEP_IN_USE


typedef void(GAVDP_CallBack) (UCHAR event, UCHAR* param);
typedef WORD(GAVDP_SigChnlCallBack) (UCHAR* bd_addr);

/*Event Code definition*/

/*for signal*/
#define GAVDP_EV_GETCAPS						0x00
#define GAVDP_EV_SETCONFIG						0x01
#define GAVDP_EV_GETCONFIG						0x02
#define GAVDP_EV_OPEN							0x03
#define GAVDP_EV_CLOSE							0x04
#define GAVDP_EV_START							0x05
#define GAVDP_EV_SUSPEND						0x06
#define GAVDP_EV_RECONFIG						0x07
#define GAVDP_EV_SECURITYCTL					0x08
#define GAVDP_EV_ABORT							0x09

/*for stream read*/
#define GAVDP_EV_READ							0x10

#define GAVDP_EV_TCMTU							0x11

/*L2CAP channel connect and disconnect ind*/

#define GAVDP_EV_SIGCHNLCONN					0x20
#define GAVDP_EV_SIGCHNLDISC					0x21




/********************************************************************************************/
/*																							*/
/*						SEP maintaining procedures											*/
/*																							*/
/********************************************************************************************/

#if 0
#define GAVDP_RecoveryParam AVDTP_RecoveryParam
#endif

struct GAVDP_RecoveryParam {
	UINT8	type;
	UINT8	mrws;
	UINT8	mnmp;
};


#if 0
#define GAVDP_MediaCodecParam AVDTP_MediaCodecParam
#endif

struct GAVDP_MediaCodecParam {
	UINT8	media_type;
	UINT8	media_codec_type;
	WORD	length;
	UCHAR	*media_codec_info;
};

#if 0
#define GAVDP_ContentProtectionParam AVDTP_ContentProtectionParam
#endif

struct GAVDP_ContentProtectionParam {
	WORD	type;
	WORD	length;
	UCHAR	*cp_value;
};

#if 0
#define GAVDP_HeaderCompressionParam AVDTP_HeaderCompressionParam
#endif

struct GAVDP_HeaderCompressionParam {
	UINT8	back_channel;
	UINT8	media;
	UINT8	recovery;
};

#if 0
#define GAVDP_MultiplexingParam AVDTP_MultiplexingParam
#endif

struct GAVDP_MultiplexingParam {
	UINT8	frag;
	UINT8	media_tsid;
	UINT8	media_tcid;
	UINT8	report_tsid;
	UINT8	report_tcid;
	UINT8	recovery_tsid;
	UINT8	recovery_tcid;
};

#if 0
#define GAVDP_ConfigParamStru AVDTP_ConfigParamStru
#endif

struct GAVDP_ConfigParamStru {
	WORD	service_category_mask;
		/* media transport and report has no param */
		
	UINT8								num_of_codec;
	UINT8								num_of_cp;		/* MAY be multiple in GET_CAPS_RSP */
	
	struct GAVDP_RecoveryParam 			*recovery;	
	struct GAVDP_MediaCodecParam 		*media_codec;
	struct GAVDP_ContentProtectionParam	*cp;	
	struct GAVDP_HeaderCompressionParam  *hc;
	struct GAVDP_MultiplexingParam		      *mp;
};

#if 0
#define GAVDP_SetConfigReqInStru AVDTP_SetConfigReqInStru
#endif

/* set configuration */
struct GAVDP_SetConfigReqInStru {
	UCHAR 	bd_addr[BD_ADDR_LEN];		/* remote device address */
	UINT8	acp_seid;		/* remote sep id */
	UINT8	int_seid;		/* local sep id */
	struct GAVDP_ConfigParamStru *config_param;
};


#if 0
#define GAVDP_ReconfigReqInStru AVDTP_ReconfigReqInStru
#endif

/* reconfigure application service capabilities (media codec and content protection) */
struct GAVDP_ReconfigReqInStru {
	WORD stream_handle;
	struct GAVDP_ConfigParamStru *config_param;	/* only app caps */
};

#if 0
#define GAVDP_SecurityControlReqInStru AVDTP_SecurityControlReqInStru
#endif

/* security control request in structure */
struct GAVDP_SecurityControlReqInStru {
	WORD 	stream_handle;
	WORD	length;
	UCHAR	security_data[1];
};

#if 0
#define GAVDP_StartSuspendReqInStru AVDTP_StartSuspendReqInStru
#endif

/* start and suspend request in structure */
struct GAVDP_StartSuspendReqInStru {
	UINT8	num;	/* # of stream handle */
	WORD	p_stream_handle[1]; /* pointer to the array of stream handle */
};


#if 0
#define GAVDP_GetCapsReqInStru AVDTP_GetCapsReqInStru
#endif

/* Get capabilities request in structure */
struct GAVDP_GetCapsReqInStru {
	UCHAR	bd_addr[BD_ADDR_LEN];
	UINT8 	acp_seid;
};

 #if 0
#define GAVDP_ConnectRspInStru AVDTP_ConnectRspInStru
#endif


/* connect response in structure  */
struct GAVDP_ConnectRspInStru {
	UCHAR* bd_addr;	
	WORD 	connect_result;
	WORD 	status;
	UCHAR* conf_param;
};

 #if 0
#define GAVDP_SEPPrimaryDataStru AVDTP_SEPPrimaryDataStru
#endif

/* Discover cfm structure */
typedef struct GAVDP_SEPPrimaryDataStru {
	UINT8	seid;
	UINT8	in_use;
	UINT8	media_type;
	UINT8	sep_type;	/* GAVDP_SEP_TYPE_SRC or GAVDP_SEP_TYPE_SNK */
}GAVDP_SEPPrimaryDataStru;




/*********************************************************************************************/
/*																							 */
/*						Indication															 */
/*																							 */
/*********************************************************************************************/
/* common indication structure */
/*The following structures are same as the corresponding structures in AVDTP, 
change transaction to resevered, so upper application won't use it.
*/
struct GAVDP_CommonIndStru {
	UCHAR	reserved;
	WORD	stream_handle;
};

/* discover indication structure */
struct GAVDP_DiscoverIndStru {
	UCHAR	reserved;
	UCHAR	bd_addr[BD_ADDR_LEN];
};

/* get capabilities indication structure */
struct GAVDP_GetCapsIndStru {
	UCHAR	reserved;
	UCHAR	acp_seid;
	UCHAR	bd_addr[BD_ADDR_LEN];
};

/* set configuration indication structure */
struct GAVDP_SetConfigurationIndStru {
	UCHAR							reserved;
	UCHAR							acp_seid;
	UCHAR							bd_addr[BD_ADDR_LEN];
	UCHAR 							int_seid;
	WORD							stream_handle;
	struct GAVDP_ConfigParamStru*		config_param;
};

/* start or suspend indication structure */
struct GAVDP_StartSuspendIndStru {
	UCHAR		reserved;
	UCHAR		num;	/* # of stream handle */
	WORD		p_stream_handle[1];
};


/* security control indication structure */
struct GAVDP_SecurityControlIndStru {
	UCHAR	reserved;
	WORD	stream_handle;
	WORD	length;
	UCHAR	security_data[1];
};

/* reconfigure indication structure */
struct GAVDP_ReconfigureIndStru {
	UCHAR							reserved;
	WORD							stream_handle;
	struct GAVDP_ConfigParamStru*		config_param;	/* only app caps */
};

/*stream data indication*/
struct GAVDP_DataIndStru {
	WORD		stream_handle;
	struct BuffStru *data;
	WORD 		length;
	DWORD		time_info;
	UINT8		payload_type;
	UINT8		marker;
	UINT8		reliability;
};

/********************************************************************************************/
/*																							*/
/*						sep response														*/
/*																							*/
/********************************************************************************************/
/* avdtp common response in structure */
struct GAVDP_CommonRspInStru {
	WORD 	stream_handle;
	UCHAR 	reserved;	
	UCHAR	error_code;
};

/* discover response in structure */
struct GAVDP_DiscoverRspInStru {
	UCHAR 							reserved;
	UCHAR							error_code;
	UCHAR*							bd_addr;
	UCHAR							sep_num;
	struct GAVDP_SEPPrimaryDataStru*	p_acp_sep;	/* sep primary data (InUse, MediaType, SEPType) */	
};

/* get capabilities response in structure */
struct GAVDP_GetCapsRspInStru {
	UCHAR 							seid;		/*the seid of this caps*/
	UCHAR							error_code;
	UCHAR 							bd_addr[BD_ADDR_LEN];
	struct GAVDP_ConfigParamStru*		config_param;	
};

/* set configuration response in structure */
struct GAVDP_SetConfigRspInStru {
	UCHAR 	reserved;
	UCHAR	error_code;
	WORD 	stream_handle;
	WORD	first_category_to_fail;	/* sevice category mask */	
};


/* start or suspend response in structure */
struct GAVDP_StartSuspendRspInStru {
	UCHAR 	reserved;
	UCHAR	error_code;
	WORD 	stream_handle;	
};


/* get configuration response in structure */
struct GAVDP_GetConfigRspInStru {
	WORD 							stream_handle;
	UCHAR 							reserved;
	UCHAR							error_code;
	struct GAVDP_ConfigParamStru*		config_param;	/* Configured capabilities */	
};

/* security control response in structure */
struct GAVDP_SecurityControlRspInStru {
	WORD 	stream_handle;
	UCHAR 	reserved;
	UCHAR	error_code;	
	WORD	length;
	UCHAR	security_data[1]; 	
};


/* reconfig response in structure */
struct GAVDP_ReconfigRspInStru {
	WORD 	stream_handle;
	UCHAR 	reserved;	
	UCHAR	error_code;
	WORD	first_category_to_fail;	/* sevice category mask */
};
	

/* create sep */
struct GAVDP_ChannelConfigParamStru {
	WORD			in_mtu;
	WORD 			flush_to;
  	WORD 			link_to;
  	struct QosStru 	*flow;
};

struct GAVDP_SEPCreateInStru {
	UCHAR	reserved;
	UCHAR	sep_type;
	UCHAR	media_type;
	/*
	WORD	local_media_mtu;
	WORD	local_report_mtu;
	WORD	local_recovery_mtu;
	*/
	struct GAVDP_ChannelConfigParamStru *media_param;
	struct GAVDP_ChannelConfigParamStru *report_param;
	struct GAVDP_ChannelConfigParamStru *recovery_param;  	
};

/* Connect */
struct GAVDP_ConnectReqInStru {
	UCHAR	bd_addr[BD_ADDR_LEN];
	struct GAVDP_ChannelConfigParamStru *conf_param;	/* if NULL, DEFAULT value is used */	
};

struct GAVDP_ConnectReqOutStru {
	WORD	connect_result;
	WORD	config_result;
	WORD	status;	
};

/* sep discover */
struct GAVDP_SEPDiscoverOutStru {
	WORD								error_code;
	UCHAR								sep_num;
	struct GAVDP_SEPPrimaryDataStru* 		p_acp_sep;
};

/* get capabilities */
struct GAVDP_GetCapsOutStru {	
	WORD							error_code;	
	struct GAVDP_ConfigParamStru* 	config_param;
};

/* get configuration */
struct GAVDP_GetConfigOutStru {	
	WORD							error_code;	
	struct GAVDP_ConfigParamStru*		config_param;
};


/* stream configure */
struct GAVDP_StreamConfigOutStru {
	WORD		stream_handle;	
	WORD		first_category_to_fail;	/* sevice category mask */
	WORD		error_code;	
};

/* change application parameters */
struct GAVDP_StreamReconfigOutStru {
	WORD	error_code;
	WORD	first_category_to_fail;
};

/* security control */
struct GAVDP_SecurityControlOutStru {
	WORD 	error_code;
	WORD	length;
	UCHAR*	security_data; 	
};

/* start or suspend out structure */
struct GAVDP_StartSuspendOutStru {
	WORD		error_code;
	WORD		first_failing_stream_handle;
};


/********************************************************************************************/
/*																							*/
/*						AV Streaming procedures												*/
/*																							*/
/********************************************************************************************/

#if 0
#define GAVDP_WriteReqInStru AVDTP_WriteReqInStru
#endif
/* write stream data */
struct GAVDP_WriteReqInStru {
	WORD 	stream_handle;
	WORD	length;
	UCHAR	*out_buffer; 
	DWORD	time_info;
	UINT8	payload_type;
	UINT8	marker;
};

#if 0
#define GAVDP_ReadReqInStru AVDTP_ReadReqInStru
#endif
/* read request in structure */
struct GAVDP_ReadReqInStru {
	WORD 	stream_handle;
	WORD	exp_length;
	UCHAR	*in_buffer; 
};

 #if 0
#define GAVDP_ReadReqInStru AVDTP_ReadReqInStru
#endif
/* read request out structure */
struct GAVDP_ReadReqOutStru {
	WORD	result;
	WORD	length;
	DWORD	time_info;
	UINT8	payload_type;
	UINT8	marker;
	WORD	reliability;	/* packet lost is indicated here */
};


/*********************************************************************************************/
/*																							 */
/*						GetTransportChannelMtu Out Param Structure							 */
/*																							 */
/*********************************************************************************************/
struct GAVDP_TransportChannelMtuStru {
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
#endif
