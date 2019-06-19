/***************************************************************************
Module Name:
    a2dp_sbc.h
Abstract:
	This file provides a2dp SBC related structure and interface.
Author:
    Lu Dongmei
Revision History:
	2003.2.28	Created
---------------------------------------------------------------------------*/

#ifndef A2DP_SBC_H
#define A2DP_SBC_H

#include "sbc.h"

struct A2DP_SBCInfoElements{	
/* the structure of SBC codec_caps used for GetCaps or SetConfigs */
#ifndef CONFIG_BIG_ENDIAN
	UCHAR chnl_mode:4;
	UCHAR sample_frequency:4;
	UCHAR alloc_method:2;	
	UCHAR subband:2;	
	UCHAR block_length:4;
#else
	UCHAR sample_frequency:4;
	UCHAR chnl_mode:4;
	UCHAR block_length:4;	
	UCHAR subband:2;	
	UCHAR alloc_method:2;	
#endif	
	UCHAR min_bitpool;
	UCHAR max_bitpool;
};	

struct A2DP_SBCConfigedCaps{	
/* the structure of SBC configed codec_caps used for compose & decompose sbc streams */
#ifdef	CONFIG_BIG_ENDIAN
	UCHAR	sample_frequency:2;
	UCHAR	blocks:2;
	UCHAR	channel_mode:2;
	UCHAR	allocation_method:1;
	UCHAR	subbands:1;
#else
	UCHAR	subbands:1;	
	UCHAR	allocation_method:1;
	UCHAR	channel_mode:2;
	UCHAR	blocks:2;
	UCHAR	sample_frequency:2;
#endif /*CONFIG_BIG_ENDIAN*/
	UCHAR min_bitpool;
	UCHAR max_bitpool;
};	

struct A2DP_SBC_FRAME_HDR {
	UCHAR	syncword;					/* 0x9C */	
#ifdef	CONFIG_BIG_ENDIAN
	UCHAR	sampling_frequency:2;
	UCHAR	blocks:2;
	UCHAR	channel_mode:2;
	UCHAR	allocation_method:1;
	UCHAR	subbands:1;
#else
	UCHAR	subbands:1;	
	UCHAR	allocation_method:1;
	UCHAR	channel_mode:2;
	UCHAR	blocks:2;
	UCHAR	sampling_frequency:2;
#endif /*CONFIG_BIG_ENDIAN*/
	UCHAR	bitpool;
	UCHAR	crc_check;
	UCHAR	joint;	
 };

struct A2DP_SBC_PACKET_HDR {
#ifdef	CONFIG_BIG_ENDIAN
	UCHAR	fragment:1;
	UCHAR	start:1;
	UCHAR	last:1;
	UCHAR	rfa:1;
	UCHAR	number:4;
#else
	UCHAR	number:4;	
	UCHAR	rfa:1;
	UCHAR	last:1;
	UCHAR	start:1;
	UCHAR	fragment:1;
#endif /*CONFIG_BIG_ENDIAN*/
 };

void A2DP_SBCConstruct(
		struct A2DP_CodecBaseStru * codec);

UCHAR A2DP_SBCSetCodecCaps(
		struct A2DP_CodecBaseStru 	*codec, 
		UCHAR 						*codec_caps);

UCHAR A2DP_SBCGetCodecCaps(
		struct A2DP_CodecBaseStru 	*codec, 
		UCHAR 						*codec_caps);

UCHAR A2DP_SBCSelectCodecCaps(
		struct A2DP_CodecBaseStru 		*codec, 
		struct A2DP_SEPRemCapsInfoStru 	*rem_sepcaps, 
		struct A2DP_SetConfigStru 			*sep_config);

UCHAR * A2DP_SBCConvert(
		UCHAR 		convert_flag, 
		UCHAR 		*codec_caps);

UCHAR A2DP_SBCCheckCaps(
		UCHAR 		*codec_caps, 
		UCHAR 		*config_codec_caps);

UCHAR A2DP_SBCFrameLength(
		struct A2DP_CodecBaseStru 	*codec, 
		UCHAR 						*buf, 
		WORD 						len);

WORD A2DP_SBCComposeData(
	struct A2DP_CodecBaseStru 		*codec, 
	struct A2DP_DataComposeInStru 	*in, 
	struct A2DP_DataStru 				*out);

void A2DP_SBCDecomposeData(
	struct A2DP_CodecBaseStru 		 *codec, 
	struct A2DP_DataDecomposeInStru	 *data_decom_in, 
	struct A2DP_DataIndStru 		 	 *data_decom_out);

void A2DP_SBCBuildPacketHeader(
	struct A2DP_CodecBaseStru 		*codec,
	struct A2DP_DataComposeInStru 	*in,
	struct A2DP_DataStru 				*out);

void A2DP_SBCSaveConfigedCaps(
	struct A2DP_CodecBaseStru 		*codec,
	UCHAR							*media_codec_info);

#endif
