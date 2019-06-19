/***************************************************************************
Module Name:
    a2dpmacro.h
Abstract:
	This file provides a2dp macro definition.
Author:
    Lu Dongmei
Revision History:
	2002.11.29	Created

---------------------------------------------------------------------------*/

#ifndef A2DPMACRO_H
#define A2DPMACRO_H

#define  A2DP_OPTIMIZE			0x01

#define CONFIG_A2DP_WRITE_THROUGH	1

#define STREAMHANDLE			struct A2DP_StreamInfoStru *

#define AUDIOSRC				0x00 
#define AUDIOSNK				0x01
#define A2DP_SNK_SERVER		0x00
#define A2DP_SRC_SERVER		0x01
#define A2DP_CLIENT				0x02

#define A2DP_SRC_INT			0x01
#define A2DP_SRC_ACP			0x02
#define A2DP_SNK_INT			0x03
#define A2DP_SNK_ACP			0x04

/* MPEG-1,2 capabilitis */
#define A2DP_MPEG12_BITRATE0000		0x0001  /* free */

#define A2DP_MPEG12_BITRATE0001		0x0002  /* 32Kbps for MPEG-1 layer 3 */

#define A2DP_MPEG12_BITRATE0010		0x0004  /* 40Kbps for MPEG-1 layer 3 */

#define A2DP_MPEG12_BITRATE0011		0x0008  /* 48Kbps for MPEG-1 layer 3 */

#define A2DP_MPEG12_BITRATE0100		0x0010  /* 56Kbps for MPEG-1 layer 3 */

#define A2DP_MPEG12_BITRATE0101		0x0020  /* 64Kbps for MPEG-1 layer 3 */

#define A2DP_MPEG12_BITRATE0110		0x0040  /* 80Kbps for MPEG-1 layer 3 */

#define A2DP_MPEG12_BITRATE0111		0x0080  /* 96Kbps for MPEG-1 layer 3 */

#define A2DP_MPEG12_BITRATE1000		0x0100  /* 112Kbps for MPEG-1 layer 3 */

#define A2DP_MPEG12_BITRATE1001		0x0200  /* 128Kbps for MPEG-1 layer 3 */

#define A2DP_MPEG12_BITRATE1010		0x0400  /* 160Kbps for MPEG-1 layer 3 */

#define A2DP_MPEG12_BITRATE1011		0x0800  /* 192Kbps for MPEG-1 layer 3 */

#define A2DP_MPEG12_BITRATE1100		0x1000  /* 224Kbps for MPEG-1 layer 3 */

#define A2DP_MPEG12_BITRATE1101		0x2000  /* 256Kbps for MPEG-1 layer 3 */

#define A2DP_MPEG12_BITRATE1110		0x4000  /* 320Kbps for MPEG-1 layer 3 */
/*
bits    				V1,L1  V1,L2  V1,L3  V2,L1  V2, L2 & L3 
MPEG12_BITRATE0000  free    free     free    free     free 
MPEG12_BITRATE0001	32      32       32       32       8 
MPEG12_BITRATE0010  64      48       40       48       16 
MPEG12_BITRATE0011  96      56       48       56       24 
MPEG12_BITRATE0100  128    64       56       64       32 
MPEG12_BITRATE0101  160    80       64       80       40 
MPEG12_BITRATE0110  192    96       80       96       48 
MPEG12_BITRATE0111  224    112     96       112     56 
MPEG12_BITRATE1000  256    128     112     128     64 
MPEG12_BITRATE1001  288    160     128     144     80 
MPEG12_BITRATE1010  320    192     160     160     96 
MPEG12_BITRATE1011  352    224     192     176     112 
MPEG12_BITRATE1100  384    256     224     192     128 
MPEG12_BITRATE1101  416    320     256     224     144 
MPEG12_BITRATE1110  448    384     320     256     160 
*/

#define A2DP_MPEG12_VBRSUPPORT		0x01
#define A2DP_MPEG12_FS48000 			0x01
#define A2DP_MPEG12_FS44100 			0x02
#define A2DP_MPEG12_FS32000 			0x04
#define A2DP_MPEG12_FS24000 			0x08
#define A2DP_MPEG12_FS22050 			0x10
#define A2DP_MPEG12_FS16000 			0x20
#define A2DP_MPEG12_MPF2SUPPORT	0x01
#define A2DP_MPEG12_JOINTSTEREO		0x01
#define A2DP_MPEG12_STEREO      		0x02
#define A2DP_MPEG12_DUAL      			0x04
#define A2DP_MPEG12_MONO     			0x08
#define A2DP_MPEG12_CRCSUPPORT		0x01
#define A2DP_MPEG12_LAYER1			0x04
#define A2DP_MPEG12_LAYER2			0x02
#define A2DP_MPEG12_LAYER3 			0x01

/* SBC capabilitis */
#define A2DP_SBC_FS48000 			0x01
#define A2DP_SBC_FS44100 			0x02
#define A2DP_SBC_FS32000 			0x04
#define A2DP_SBC_FS16000 			0x08
#define A2DP_SBC_JOINTSTEREO			0x01
#define A2DP_SBC_STEREO      			0x02
#define A2DP_SBC_DUAL      				0x04
#define A2DP_SBC_MONO     				0x08
#define A2DP_SBC_BLOCK_16			0x01
#define A2DP_SBC_BLOCK_12			0x02
#define A2DP_SBC_BLOCK_8				0x04
#define A2DP_SBC_BLOCK_4				0x08
#define A2DP_SBC_SUBBAND_8			0x01
#define A2DP_SBC_SUBBAND_4			0x02
#define A2DP_SBC_LOUDNESS			0x01
#define A2DP_SBC_SNR					0x02
#define A2DP_SBC_MINBITPOOL			0x01
#define A2DP_SBC_MAXBITPOOL			0x02

#define A2DP_SBC_BITPOOL 					0x30//0x33


#define A2DP_CODEC	 				0x1000
#define A2DP_TRANSPORTSERVICE		0x2000
#define A2DP_CONTENTPROTECTION	0x4000

#define A2DP_SBC					0x00
#define A2DP_MPEG12 				0x01
#define A2DP_MPEG24 				0x02
#define A2DP_SBC_PAYLOAD_TYPE 		0x60
#define A2DP_MPEG12_PAYLOAD_TYPE 	0x61


#define A2DP_BASICSERVICE			0x0001
#define A2DP_REPORTSERVICE			0x0002
#define A2DP_RECOVERYSERIVCE		0x0004
#define A2DP_MULTISERIVCE			0x0008

#define A2DP_PLAYER		    			0x0001
#define A2DP_MICROPHONE			0x0002
#define A2DP_TUNER		        		0x0004
#define A2DP_MIXER			        	0x0008
#define A2DP_HEADPHONE		       0x0100
#define A2DP_SPEAKER		        	0x0200
#define A2DP_RECORDER		        	0x0400
#define A2DP_AMPLIFIER		        	0x0800

#define A2DP_EV_STREAMCONFIG    	0x00
#define A2DP_EV_STREAMOPEN	    	0x01
#define A2DP_EV_STREAMSTART		0x02
#define A2DP_EV_STREAMSUSPEND 	0x03
#define A2DP_EV_STREAMCLOSE	    	0x04
#define A2DP_EV_STREAMRECONFIG	0x05
#define A2DP_EV_STREAMABORT		0x06
#define A2DP_EV_SRCQOS				0x07
#define A2DP_EV_SNKQOS				0x08

#define A2DP_EV_SIGCHNLCONN		0x09
#define A2DP_EV_SIGCHNLDISC		0x0A

#define A2DP_EV_STREAMDATA		0x0B

#define SEP_STAT_IDLE				0x01
#define SEP_STAT_OPEN				0x02
#define SEP_STAT_STREAMING		0x03

#define A2DP_MAX_SEP_NUM				0x08
#define A2DP_MPEG_PAYLOAD_HEADER		0x04
#define A2DP_AUDIO						0x00
#define A2DP_AUDIOCARD_NAME_LEN      	0x80
#define A2DP_CODECCAPS_LEN			0x10
#define A2DP_FORMAT_TO_INFO			0x01
#define A2DP_INFO_TO_FORMAT			0x02

#define A2DP_SBC_PAYLOAD_HEADER		0x01
#define AUDIO							0x00
#define A2DP_PAYLOAD_HEADER			0x38
#define AVDTP_PAYLOAD_HEADER			0x0C

/* Error codes */
#define INVALID_CODEC_TYPE							0xC1
#define NOT_SUPPORTED_CODEC_TYPE					0xC2
#define INVALID_SAMPLING_FREQUENCY				0xC3
#define NOT_SUPPORTED_SAMPLING_FREQUENCY		0xC4
#define INVALID_CHANNEL_MODE						0xC5
#define NOT_SUPPORTED_CHANNEL_MODE				0xC6
#define INVALID_SUBBANDS							0xC7
#define NOT_SUPPORTED_SUBBANDS					0xC8
#define INVALID_ALLOCATION_METHOD				0xC9
#define NOT_SUPPORTED_ALLOCATION_METHOD			0xCA
#define INVALID_MINIMUM_BITPOOL_VALUE			0xCB
#define NOT_SUPPORTED_MINIMUM_BITPOOL_VALUE	0xCC
#define INVALID_MAXIMUM_BITPOOL_VALUE			0xCD
#define NOT_SUPPORTED_MAXIMUM_BITPOOL_VALUE	0xCE
#define INVALID_LAYER								0xCF
#define NOT_SUPPORTED_LAYER						0xD0
#define NOT_SUPPORTED_CRC							0xD1
#define NOT_SUPPORTED_MPF							0xD2
#define NOT_SUPPORTED_VBR							0xD3
#define INVALID_BIT_RATE							0xD4
#define NOT_SUPPORTED_BIT_RATE					0xD5
#define INVALID_OBJECT_TYPWBIT_RATE				0xD6
#define NOT_SUPPORTED_OBJECT_TYPE				0xD7
#define INVALID_CHANNELS							0xD8
#define NOT_SUPPORTED_CHANNELS					0xD9
#define INVALID_VERSION								0xDA
#define NOT_SUPPORTED_VERSION						0xDB
#define NOT_SUPPORTED_MAXIMUM_SUL				0xDC
#define INVALID_BLOCK_LENGTH						0xDD

#define INVALID_CP_TYPE 							0xE0
#define INVALID_CP_FORMAT							0xE1

/* A2DP Error code ID */
#define ERROR_A2DP_BASE								( PROT_A2DP_BASE + 20)
#define A2DP_ERROR_NOTENOUGH_DATA					( ERROR_A2DP_BASE + 1 )

#define A2DP_VALIDSEID(seid)		(seid>0)?BT_SUCCESS:BT_FAIL

/* AudioWaveFmt struct , wFormatTag */
#define A2DP_WAVEFMT_NULL				0
#define A2DP_WAVEFMT_PCM				1	 /* pcm format */
#define A2DP_WAVEFMT_MP3				85     /*mp3 format */
#define A2DP_WAVEFMT_ALAW				6	  /* a-law format*/
#define A2DP_WAVEFMT_ULAW				7	  /* u-law format */
#define A2DP_WAVEFMT_SBC				0x4001	/* SBC format */

#endif
