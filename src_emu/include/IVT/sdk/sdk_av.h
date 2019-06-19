/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_av.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/

#ifndef _SDK_AV_H
#define _SDK_AV_H
/*************** Macro Definition ******************/

/*dev_type*/
#define BTSDK_A2DP_PLAYER	    			0x0001
#define BTSDK_A2DP_HEADPHONE		       0x0100

/*sep_type*/
#define BTSDK_AUDIOSRC				       0x00 
#define BTSDK_AUDIOSNK				       0x01

/*codec_type*/
#define BTSDK_A2DP_SBC					0x00
#define BTSDK_A2DP_MPEG12 				0x01
#define BTSDK_A2DP_MPEG24				0x02
#define BTSDK_A2DP_SBC_PAYLOAD_TYPE 		0x60
#define BTSDK_A2DP_MPEG12_PAYLOAD_TYPE 	0x61

/*codec_priority*/
#define BTSDK_A2DP_CODEC_PRIORITY_1       1
#define BTSDK_A2DP_CODEC_PRIORITY_2        2
#define BTSDK_A2DP_CODEC_PRIORITY_3        3 

/*codec_num*/
#define BTSDK_CODEC_NUM_1                       1 
#define BTSDK_CODEC_NUM_2                       2
#define BTSDK_CODEC_NUM_3                       3

/*content_protect*/
#define BTSDK_CONTENT_PROTECT_1                       1 
#define BTSDK_CONTENT_PROTECT_2                       2 
#define BTSDK_CONTENT_PROTECT_3                       3 

/*chnl_mode*/
#define BTSDK_A2DP_MPEG12_JOINTSTEREO		0x01
#define BTSDK_A2DP_MPEG12_STEREO      		0x02
#define BTSDK_A2DP_MPEG12_DUAL      			0x04
#define BTSDK_A2DP_MPEG12_MONO     			0x08

/*crc*/
#define BTSDK_A2DP_MPEG12_CRCSUPPORT		0x01

/*layer*/
#define BTSDK_A2DP_MPEG12_LAYER1			0x04
#define BTSDK_A2DP_MPEG12_LAYER2			0x02
#define BTSDK_A2DP_MPEG12_LAYER3 			0x01

/*sample_frequency*/
#define BTSDK_A2DP_MPEG12_FS48000 			0x01
#define BTSDK_A2DP_MPEG12_FS44100 			0x02
#define BTSDK_A2DP_MPEG12_FS32000 			0x04
#define BTSDK_A2DP_MPEG12_FS24000 			0x08
#define BTSDK_A2DP_MPEG12_FS22050 			0x10
#define BTSDK_A2DP_MPEG12_FS16000 			0x20

/*mpf*/
#define BTSDK_A2DP_MPEG12_MPF2SUPPORT	       0x01

/*bitrate*/
#define BTSDK_A2DP_MPEG12_BITRATE0000		0x0001  /* free */
#define BTSDK_A2DP_MPEG12_BITRATE0001		0x0002  /* 32Kbps for MPEG-1 layer 3 */
#define BTSDK_A2DP_MPEG12_BITRATE0010		0x0004  /* 40Kbps for MPEG-1 layer 3 */
#define BTSDK_A2DP_MPEG12_BITRATE0011		0x0008  /* 48Kbps for MPEG-1 layer 3 */
#define BTSDK_A2DP_MPEG12_BITRATE0100		0x0010  /* 56Kbps for MPEG-1 layer 3 */
#define BTSDK_A2DP_MPEG12_BITRATE0101		0x0020  /* 64Kbps for MPEG-1 layer 3 */
#define BTSDK_A2DP_MPEG12_BITRATE0110		0x0040  /* 80Kbps for MPEG-1 layer 3 */
#define BTSDK_A2DP_MPEG12_BITRATE0111		0x0080  /* 96Kbps for MPEG-1 layer 3 */
#define BTSDK_A2DP_MPEG12_BITRATE1000		0x0100  /* 112Kbps for MPEG-1 layer 3 */
#define BTSDK_A2DP_MPEG12_BITRATE1001		0x0200  /* 128Kbps for MPEG-1 layer 3 */
#define BTSDK_A2DP_MPEG12_BITRATE1010		0x0400  /* 160Kbps for MPEG-1 layer 3 */
#define BTSDK_A2DP_MPEG12_BITRATE1011		0x0800  /* 192Kbps for MPEG-1 layer 3 */
#define BTSDK_A2DP_MPEG12_BITRATE1100		0x1000  /* 224Kbps for MPEG-1 layer 3 */
#define BTSDK_A2DP_MPEG12_BITRATE1101		0x2000  /* 256Kbps for MPEG-1 layer 3 */
#define BTSDK_A2DP_MPEG12_BITRATE1110		0x4000  /* 320Kbps for MPEG-1 layer 3 */

/*vbr*/
#define BTSDK_A2DP_MPEG12_VBRSUPPORT		0x01

/* SBC capabilitis */
#define BTSDK_A2DP_SBC_FS48000 			0x01
#define BTSDK_A2DP_SBC_FS44100 			0x02
#define BTSDK_A2DP_SBC_FS32000 			0x04
#define BTSDK_A2DP_SBC_FS16000 			0x08

#define BTSDK_A2DP_SBC_JOINTSTEREO			0x01
#define BTSDK_A2DP_SBC_STEREO      			0x02
#define BTSDK_A2DP_SBC_DUAL      				0x04
#define BTSDK_A2DP_SBC_MONO     				0x08

#define BTSDK_A2DP_SBC_BLOCK_16			0x01
#define BTSDK_A2DP_SBC_BLOCK_12			0x02
#define BTSDK_A2DP_SBC_BLOCK_8				0x04
#define BTSDK_A2DP_SBC_BLOCK_4				0x08

#define BTSDK_A2DP_SBC_SUBBAND_8			0x01
#define BTSDK_A2DP_SBC_SUBBAND_4			0x02

#define BTSDK_A2DP_SBC_LOUDNESS			0x01
#define BTSDK_A2DP_SBC_SNR					0x02

#define BTSDK_A2DP_SBC_MINBITPOOL			0x01
#define BTSDK_A2DP_SBC_MAXBITPOOL			0x02

#define BTSDK_A2DP_SBC_BITPOOL 					0x30//0x3



#define BTSDK_A2DP_AUDIOCARD_NAME_LEN      	0x80
#define BTSDK_A2DP_CODECCAPS_LEN			       0x10


struct BTSDK_A2DP_CodecCapsStru{
	BTUINT8   codec_type;        						/* Codec type */
	BTUINT8   codec_priority;						/* the priority of the Codec */
	BTUINT8   codec_caps[BTSDK_A2DP_CODECCAPS_LEN];      /* indicates the Codec information */	
};

typedef struct _BtSdkLocalA2DPServerAttrStru 
{	
	BTUINT16	dev_type;							/* the local device type, can be PLAYER or HEADPHONE*/
	BTUINT16     trans_mask;         					/* transport service mask */
	BTUINT16	content_protect;    					/* the content protection type RFD */
	BTUINT8	    sep_type;							/* SRC or SNK */
	BTUINT8      codec_num;   				
	BTUINT8      audio_card[BTSDK_A2DP_AUDIOCARD_NAME_LEN];	/* the audio card name used */	
	struct BTSDK_A2DP_CodecCapsStru codec[1];
} BtSdkLocalA2DPServerAttrStru, *PBtSdkLocalA2DPServerAttrStru;

/*************** Function Prototype ******************/

struct BTSDK_A2DP_MPEG12UserInfoStru{	/* the structure of MPEG-1,2 codec_caps */
	BTUINT8 chnl_mode;			/* channel mode, can be MPEG12_JOINTSTEREO,MPEG12_STEREO,MPEG12_DUAL,MPEG12_MONO or the combination of them */
	BTUINT8 crc;					/* CRC protection flag, can be MPEG12_CRCSUPPORT or zero */
	BTUINT8 layer;				/* MPEG layer, can be MPEG12_LAYER1,MPEG12_LAYER2,MPEG12_LAYER3 or the combination of them */
	BTUINT8 sample_frequency;	/* sample frequency, can be MPEG12_FS48000,MPEG12_FS44100,MPEG12_FS32000,MPEG12_FS24000,MPEG12_FS22050,MPEG12_FS16000 or the combination of them */
	BTUINT8 mpf;					/* media payload format MPF-2 flag, can be MPEG12_MPF2SUPPORT or zero */	
	BTUINT8 rfa;					/* reserved for future additions */
	BTUINT16 bitrate;				/* bit rate index, can be MPEG12_BITRARE0000,..., MPEG12_BITRARE1110 or the combination of them */  
	BTUINT8 vbr;					/* variable bit rate flag, can be MPEG12_VBRSUPPORT or zero */ 
};	

struct BTSDK_A2DP_SBCUserInfoStru{	/* the structure of SBC codec_caps */
	BTUINT8 chnl_mode;
	BTUINT8 sample_frequency;
	BTUINT8 alloc_method;	
	BTUINT8 subband;	
	BTUINT8 block_length;
	BTUINT8 min_bitpool;
	BTUINT8 max_bitpool;
};	

struct BTSDK_A2DP_LocalAttribStru {					/* structure for the local_attrib of BTSDK_A2DP_SRC(SNK)ClntStart BTSDK_A2DP_SRC(SNK)SvrStart */
	BTUINT16	dev_type;							/* the local device type, can be PLAYER or HEADPHONE*/
	BTUINT16     trans_mask;         					/* transport service mask */
	BTUINT16 	content_protect;    					/* the content protection type RFD */
	BTUINT8	sep_type;							/* SRC or SNK */
	BTUINT8    codec_num;   				
	BTUINT8	audio_card[BTSDK_A2DP_AUDIOCARD_NAME_LEN];	/* the audio card name used */
	struct BTSDK_A2DP_CodecCapsStru codec[1];	
};

typedef BTUINT8 (BTSDK_A2DP_CallBack) (BTUINT8 event, BTUINT8* param);
typedef BTUINT8 (A2DPSDK_CODECCBK) (BTUINT8 *codecName, BTUINT32 version);

BTSVCHDL Btsdk_RegisterA2DPSRCService(void);
void A2DPSDK_RegCodecCbk(BTUINT8 event, BTUINT8 *pcbk);
void A2DPSDK_SelectAudioCodec(void);

#endif
