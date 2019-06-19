/***************************************************************************
Module Name:
    	a2dpstru.h
Abstract:
	This file provides a2dp data structure definition.
Author:
    	Lu Dongmei
Revision History:
	2002.11.29	Created
	2003.02.28     Add SBC related structure
---------------------------------------------------------------------------*/

#ifndef A2DPSTRU_H
#define A2DPSTRU_H

/*++++++++++++++++++++++++++++++++++
Audio wave format 

pls refer to windows WAVEFORMATEX struct
--------------------------------------------*/
typedef struct s_AudioWaveFmt
{
	WORD  wFormatTag; 
	WORD  nChannels; 
	DWORD nSamplesPerSec; 
	DWORD nAvgBytesPerSec; 
	WORD  nBlockAlign; 
	WORD  wBitsPerSample; 
	WORD  cbSize;
}AudioWaveFmt;


struct A2DP_SEPInfoStru{	   	/* For a2dp_local_seplist */
	UCHAR   seid;				/* the identifier of the SEP */
	UCHAR	sep_type;		   	/* SRC or SNK */
	WORD    trans_mask;        		/* transport service mask */
	WORD 	content_protect;   	/* the content protection type RFD */
	struct A2DP_CodecBaseStru * codec; 	/* a list contains the codec info of the SEP */
	UCHAR 	status;	   	       	/* the status of the local SEP, can be idle, open or streaming */
};		

struct A2DP_CodecInfoStru{	/* the structure of codec_list element */
	UCHAR  * codec_caps;     		/* indicates the Codec information */
	UCHAR  codec_type;       		/* Codec type*/
};

struct A2DP_MPEG12UserInfoStru{	/* the structure of MPEG-1,2 codec_caps */
	UCHAR chnl_mode;			/* channel mode, can be MPEG12_JOINTSTEREO,MPEG12_STEREO,MPEG12_DUAL,MPEG12_MONO or the combination of them */
	UCHAR crc;					/* CRC protection flag, can be MPEG12_CRCSUPPORT or zero */
	UCHAR layer;				/* MPEG layer, can be MPEG12_LAYER1,MPEG12_LAYER2,MPEG12_LAYER3 or the combination of them */
	UCHAR sample_frequency;	/* sample frequency, can be MPEG12_FS48000,MPEG12_FS44100,MPEG12_FS32000,MPEG12_FS24000,MPEG12_FS22050,MPEG12_FS16000 or the combination of them */
	UCHAR mpf;					/* media payload format MPF-2 flag, can be MPEG12_MPF2SUPPORT or zero */	
	UCHAR rfa;					/* reserved for future additions */
	WORD bitrate;				/* bit rate index, can be MPEG12_BITRARE0000,..., MPEG12_BITRARE1110 or the combination of them */  
	UCHAR vbr;					/* variable bit rate flag, can be MPEG12_VBRSUPPORT or zero */ 
};	

struct A2DP_SBCUserInfoStru{	/* the structure of SBC codec_caps */
	UCHAR chnl_mode;
	UCHAR sample_frequency;
	UCHAR alloc_method;	
	UCHAR subband;	
	UCHAR block_length;
	UCHAR min_bitpool;
	UCHAR max_bitpool;
};	

struct A2DP_StreamInfoStru {   /* for a2dp_global_streamlist element */
	UCHAR   						seid;			   /* the seid of the local SEP */
	UCHAR   						peer_seid;
	UCHAR   						peer_bdaddr[6];
	WORD    						trans_mask;        /* transport service mask */
	WORD   	 					content_protect;   /* the content protection type RFD */
	struct A2DP_CodecBaseStru 	*codec;
	WORD    						stream_handle;	   /* the stream handle returned from GAVDP */
	WORD    						dmtu;
	SYSTEM_LOCK 				reflock;	/*for share access control*/
	HANDLE						refevent;
	UCHAR						refcount;
	UCHAR						istodestory;	/*TRUE, stream to be destoried*/
	UCHAR   						local_sep_type;
};

struct A2DP_DataStru { 	       /* the structure of A2DP data */
	DWORD						timestamp;   		/* the timestamp of the data packet */
	WORD    						length; 				/* the length of the data packet */
	UCHAR   						*buf;				/* the pointer to the buffer of the data packet */
};

struct A2DP_DataIndStru { 	       /* the structure of A2DP data indication parameter*/
	STREAMHANDLE handle;		/* the stream handle */
	DWORD	timestamp;  			/* the timestamp of the data packet */
	struct BuffStru *data;
};

struct A2DP_DataComposeInStru{
	WORD len;
	WORD mtu;
	WORD offset;
	UCHAR *buf;
};

struct A2DP_DataDecomposeInStru{
	WORD  len;
	UCHAR *buf;
};

struct A2DP_SEPRemInfoStru {
	UCHAR   seid;
	UCHAR	sep_type;		   /* SRC or SNK */
	UCHAR	in_use;
	UCHAR    codec_type;
	WORD     trans_mask;        			/* transport service mask */
	WORD 	content_protect;   			/* the content protection type RFD */
	UCHAR   *codec_caps;     				/* the codec info of the SEP */
};

struct A2DP_SEPRemCapsInfoStru{	   	/* For a2dp_local_seplist */
	WORD     trans_mask;        			/* transport service mask */
	WORD 	content_protect;   			/* the content protection type RFD */
	struct   A2DP_CodecInfoStru * codec;      /* a list contains the codec info of the SEP */
};

struct A2DP_SetConfigStru {    
	UCHAR   *bd;
	UCHAR    local_seid;
	UCHAR    rem_seid;
	WORD     trans_mask;        			/* transport service mask */
	WORD     content_protect;   			/* the content protection type */
	UCHAR   * codec_caps;	   			/* the Codec information that to be configured*/
	UCHAR    codec_type;     				/* the Codec configured to use */
};

struct A2DP_GetConfigStru {		   	/* for A2DP_CONF_IND parameter */
	WORD     trans_mask;         			/* transport service mask */
	WORD     content_protect;   			/* the content protection type */
	UCHAR   * codec_caps;	   			/* the Codec information that has been configured*/
	UCHAR    codec_type;     				/* the Codec configured to use */
};

struct A2DP_ReconfigStru { 	               
 	WORD     content_protect;    			/* the content protection type */
	UCHAR   *codec_caps;   	    			/* the Codec information that has been configured*/
	UCHAR    codec_type;    	    			/* the Codec configured to use */
};

struct A2DP_SRCQosIndStru {   			/* for A2DP_SRCQOS_IND parameter */
	STREAMHANDLE handle;
	UCHAR   packet_loss_rate;  
	UCHAR   round_trip_time;    
	UCHAR   interarrival_jitter;   
};

struct A2DP_SNKQosIndStru {   			/* for A2DP_SNKQOS_IND parameter */
	STREAMHANDLE handle;
	ULONGLONG   ntp_timestamp;
	DWORD  rtp_timestamp;
	WORD    packet_count;
	WORD    octet_count;   
};

struct A2DP_StreamConfigIndStru {    	/* for A2DP_EV_STREAMCONFIG parameter */
	STREAMHANDLE handle;
	UCHAR   local_seid;			   		/* the seid of the local SEP */
	UCHAR   sep_type;			   		/* the seid of the local SEP */
	WORD    trans_mask;         			/* transport service mask */
	WORD    content_protect;  			/* the content protection type */
	UCHAR   * rem_bd;					/* the baseband address of the remote device */
	UCHAR   * cfg_codeccaps;	   			/* the Codec information that has been configured */
	UCHAR   cfg_codectype;     			/* the Codec configured to use */
};

typedef UCHAR (A2DP_CODECSetCaps)(struct A2DP_CodecBaseStru * codec, UCHAR * codec_caps);
typedef UCHAR (A2DP_CODECGetCaps)(struct A2DP_CodecBaseStru * codec, UCHAR * codec_caps);
typedef UCHAR (A2DP_CODECSelectCaps)(struct A2DP_CodecBaseStru * codec, struct A2DP_SEPRemCapsInfoStru *rem_sepcaps, struct A2DP_SetConfigStru *sep_config);
typedef UCHAR * (A2DP_CODECConvert)(UCHAR convert_flag, UCHAR * codec_caps);
typedef UCHAR (A2DP_CODECCheckCaps)(UCHAR * codec_caps, UCHAR * config_codec_caps);
typedef UCHAR (A2DP_CODECFrameLength)(struct A2DP_CodecBaseStru * codec, UCHAR * buf, WORD len);
typedef void (A2DP_CODECDATACOMPOSE)(struct A2DP_CodecBaseStru * codec, struct A2DP_DataComposeInStru * data_comp_in, struct A2DP_DataStru * data_comp_out);
typedef void (A2DP_CODECDATADECOMPOSE)(struct A2DP_CodecBaseStru * codec, struct A2DP_DataDecomposeInStru * data_decom_in, struct A2DP_DataStru * data_decom_out);

struct A2DP_CodecBaseStru{						/* the base structure for the Codec */
	/*Parameters*/
	UCHAR    codec_type;        						/* Codec type*/
	UCHAR    payload_type;							/* payload type */	
	UCHAR    marker;    
	WORD     codec_caps_len;
	UCHAR    *codec_caps;
	union{
		struct A2DP_SBCConfigedCaps		*configed_sbc_caps;
		struct A2DP_MPEG12ConfigedCaps	*configed_mpeg_caps;
	};
	WORD    rough_frame_length;
	WORD	frame_length;					
	DWORD   time_info;								/* time stamp */
	DWORD	ts_factor;
	WORD    dataleft;
	WORD	fragment_size;					/*now only used for SBC packet*/
	UCHAR   *databuf_raw;
	UCHAR   *databuf_decom;
	UCHAR   *data_reserved; 
	UCHAR   data_reserved_len;
	UCHAR   payload_header_len;
};

struct A2DP_CbkStru {
	UCHAR*	hdl;
	UCHAR*	cbk;
};

struct A2DP_InfoStru {			
	STREAMHANDLE stream_handle;				/* the stream handle*/
	DWORD	svc_hdl;								/* the handle of A2DP service record on local SDP database*/
	WORD	dev_type;							/* the local device type, can be PLAYER or HEADPHONE*/
	WORD     trans_mask;         					/* transport service mask */
	WORD 	content_protect;    					/* the content protection type RFD */
	UCHAR    flag;                   						/* A2DP_CLIENT or A2DP_SNK_SERVER or A2DP_SRC_SERVER */
};

struct A2DP_CodecCapsStru{
	UCHAR   codec_type;        						/* Codec type */
	UCHAR   codec_priority;						/* the priority of the Codec */
	UCHAR   codec_caps[A2DP_CODECCAPS_LEN];      /* indicates the Codec information */	
};

struct A2DP_LocalAttribStru {					/* structure for the local_attrib of A2DP_SRC(SNK)ClntStart A2DP_SRC(SNK)SvrStart */
	WORD	dev_type;							/* the local device type, can be PLAYER or HEADPHONE*/
	WORD     trans_mask;         					/* transport service mask */
	WORD 	content_protect;    					/* the content protection type RFD */
	UCHAR	sep_type;							/* SRC or SNK */
	UCHAR    codec_num;   				
	UCHAR	audio_card[A2DP_AUDIOCARD_NAME_LEN];	/* the audio card name used */
	struct A2DP_CodecCapsStru codec[1];	
};

struct A2DP_LocalSEPAttribStru {	
	UCHAR    seid;								/* the id of the SEP */
	UCHAR	sep_type;							/* SRC or SNK */
	WORD     trans_mask;         					/* transport service mask */
	WORD 	content_protect;    					/* the content protection type RFD */
	WORD    media_chnl_mtu;
	UCHAR   codec_caps[A2DP_CODECCAPS_LEN];      /* indicates the Codec information */	
	UCHAR   codec_type;        						/* Codec type */
};

typedef UCHAR (A2DP_CallBack) (UCHAR event, UCHAR* param);

#endif
