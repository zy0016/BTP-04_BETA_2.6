/***************************************************************************
Module Name:
    a2dp_mpeg.h
Abstract:
	This file provides a2dp MPEG related structure and interface.
Author:
    Lu Dongmei
Revision History:
	2002.12.20	Created

---------------------------------------------------------------------------*/
/* MPEG AUDIO HEADER STRUCTURES
Sign Length
(bits) Position
(bits) Description 
A 11 (31-21) Frame sync (all bits must be set) 
B 2 (20,19) MPEG Audio version ID
00 - MPEG Version 2.5 (later extension of MPEG 2)
01 - reserved
10 - MPEG Version 2 (ISO/IEC 13818-3)
11 - MPEG Version 1 (ISO/IEC 11172-3) 
Note: MPEG Version 2.5 was added lately to the MPEG 2 standard. It is an extension used for very low bitrate files, allowing the use of lower sampling frequencies. If your decoder does not support this extension, it is recommended for you to use 12 bits for synchronization instead of 11 bits. 
 
C 2 (18,17) Layer description
00 - reserved
01 - Layer III
10 - Layer II
11 - Layer I 
D 1 (16) Protection bit
0 - Protected by CRC (16bit CRC follows header)
1 - Not protected 
E 4 (15,12) Bitrate index
bits V1,L1 V1,L2 V1,L3 V2,L1 V2, L2 & L3 
0000 free free free free free 
0001 32 32 32 32 8 
0010 64 48 40 48 16 
0011 96 56 48 56 24 
0100 128 64 56 64 32 
0101 160 80 64 80 40 
0110 192 96 80 96 48 
0111 224 112 96 112 56 
1000 256 128 112 128 64 
1001 288 160 128 144 80 
1010 320 192 160 160 96 
1011 352 224 192 176 112 
1100 384 256 224 192 128 
1101 416 320 256 224 144 
1110 448 384 320 256 160 
1111 bad bad bad bad bad 

NOTES: All values are in kbps
V1 - MPEG Version 1
V2 - MPEG Version 2 and Version 2.5
L1 - Layer I
L2 - Layer II
L3 - Layer III

"free" means free format. The free bitrate must remain constant, an must be lower than the maximum allowed bitrate. Decoders are not required to support decoding of free bitrate streams.
"bad" means that the value is unallowed. 

MPEG files may feature variable bitrate (VBR). Each frame may then be created with a different bitrate. It may be used in all layers. Layer III decoders must support this method. Layer I & II decoders may support it. 

For Layer II there are some combinations of bitrate and mode which are not allowed. Here is a list of allowed combinations. bitrate single channel stereo intensity stereo dual channel 
free yes yes yes yes 
32 yes no no no 
48 yes no no no 
56 yes no no no 
64 yes yes yes yes 
80 yes no no no 
96 yes yes yes yes 
112 yes yes yes yes 
128 yes yes yes yes 
160 yes yes yes yes 
192 yes yes yes yes 
224 no yes yes yes 
256 no yes yes yes 
320 no yes yes yes 
384 no yes yes yes 

 
F 2 (11,10) Sampling rate frequency index bits MPEG1 MPEG2 MPEG2.5 
00 44100 Hz 22050 Hz 11025 Hz 
01 48000 Hz 24000 Hz 12000 Hz 
10 32000 Hz 16000 Hz 8000 Hz 
11 reserv. reserv. reserv. 
 
G 1 (9) Padding bit
0 - frame is not padded
1 - frame is padded with one extra slot

Padding is used to exactly fit the bitrate.As an example: 128kbps 44.1kHz layer II uses a lot of 418 bytes and some of 417 bytes long frames to get the exact 128k bitrate. For Layer I slot is 32 bits long, for Layer II and Layer III slot is 8 bits long. 
H 1 (8) Private bit. This one is only informative. 
I 2 (7,6) Channel Mode
00 - Stereo
01 - Joint stereo (Stereo)
10 - Dual channel (2 mono channels)
11 - Single channel (Mono)

Note: Dual channel files are made of two independant mono channel. Each one uses exactly half the bitrate of the file. Most decoders output them as stereo, but it might not always be the case.
    One example of use would be some speech in two different languages carried in the same bitstream, and then an appropriate decoder would decode only the choosen language.  
J 2 (5,4) Mode extension (Only used in Joint stereo) 
Mode extension is used to join informations that are of no use for stereo effect, thus reducing needed bits. These bits are dynamically determined by an encoder in Joint stereo mode, and Joint Stereo can be changed from one frame to another, or even switched on or off. 

Complete frequency range of MPEG file is divided in subbands There are 32 subbands. For Layer I & II these two bits determine frequency range (bands) where intensity stereo is applied. For Layer III these two bits determine which type of joint stereo is used (intensity stereo or m/s stereo). Frequency range is determined within decompression algorithm.

Layer I and II Layer III 
value Layer I & II 
00 bands 4 to 31 
01 bands 8 to 31 
10 bands 12 to 31 
11 bands 16 to 31 
 Intensity stereo MS stereo 
off off 
on off 
off on 
on on 
 

 
K 1 (3) Copyright
0 - Audio is not copyrighted
1 - Audio is copyrighted

The copyright has the same meaning as the copyright bit on CDs and DAT tapes, i.e. telling that it is illegal to copy the contents if the bit is set.  
L 1 (2) Original
0 - Copy of original media
1 - Original media

The original bit indicates, if it is set, that the frame is located on its original media. 
M 2 (1,0) Emphasis
00 - none
01 - 50/15 ms
10 - reserved
11 - CCIT J.17

The emphasis indication is here to tell the decoder that the file must be de-emphasized, ie the decoder must 're-equalize' the sound after a Dolby-like noise supression. It is rarely used.  

*/

#ifndef A2DP_MPEG_H
#define A2DP_MPEG_H

struct A2DP_MPEG12InfoElements {	/* the structure of MPEG-1,2 codec_caps */
#ifndef CONFIG_BIG_ENDIAN
	UCHAR chnl_mode:4;
	UCHAR crc:1;
	UCHAR layer:3;
	UCHAR sample_frequency:6;
	UCHAR mpf:1;	
	UCHAR rfa:1;	
	UCHAR bitrate0:7;
	UCHAR vbr:1;
#else
	UCHAR layer:3;
	UCHAR crc:1;
	UCHAR chnl_mode:4;
	UCHAR rfa:1;	
	UCHAR mpf:1;
	UCHAR sample_frequency:6;
	UCHAR vbr:1;
	UCHAR bitrate0:7;
#endif
	UCHAR bitrate1;
};	

struct A2DP_MPEG12ConfigedCaps {	/* the structure of MPEG-1,2 codec_caps */
	UCHAR chnl_mode:2;
	UCHAR sample_frequency:2;
	UCHAR bitrate_idx:4;
	UCHAR layer:2;
	UCHAR crc:1;
	UCHAR mpf:1;	
	UCHAR vbr:1;
	UCHAR rfa:3;	
	UCHAR padding;
};	

struct A2DP_MPEG12_FRAME_HDR {
	UCHAR		sync1;
#ifdef CONFIG_BIG_ENDIAN
	UCHAR		sync2				:3;	
	UCHAR		version				:2; 
	UCHAR		layer				:2; 
	UCHAR		crc					:1;
	UCHAR		bitrate_idx			:4;
	UCHAR		sample_frequency	:2; 
	UCHAR		padding 			:1; 
	UCHAR		extension			:1; 
	UCHAR		chnl_mode			:2; 
	UCHAR		mode_ext			:2; 
	UCHAR		copyright			:1; 
	UCHAR		original			:1;	
	UCHAR 		emphasis			:2; 	
#else
	UCHAR		crc					:1;
	UCHAR		layer				:2; 
	UCHAR		version				:2; 
	UCHAR		sync2				:3;	
	UCHAR		extension			:1; 
	UCHAR		padding 			:1; 
	UCHAR		sample_frequency	:2; 
	UCHAR		bitrate_idx			:4;
	UCHAR 		emphasis			:2; 	
	UCHAR		original			:1;	
	UCHAR		copyright			:1; 
	UCHAR		mode_ext			:2; 
	UCHAR		chnl_mode			:2; 
#endif
 };


void A2DP_MPEG12Construct(
		struct A2DP_CodecBaseStru * codec);

UCHAR A2DP_MPEG12SetCodecCaps(
		struct A2DP_CodecBaseStru 	*codec, 
		UCHAR 						*codec_caps);

UCHAR A2DP_MPEG12GetCodecCaps(
		struct A2DP_CodecBaseStru 	*codec, 
		UCHAR 						*codec_caps);

UCHAR A2DP_MPEG12SelectCodecCaps(
		struct A2DP_CodecBaseStru 		*codec, 
		struct A2DP_SEPRemCapsInfoStru 	*rem_sepcaps, 
		struct A2DP_SetConfigStru 			*sep_config);

UCHAR * A2DP_MPEG12Convert(
		UCHAR 		convert_flag, 
		UCHAR 		*codec_caps);

UCHAR A2DP_MPEG12CheckCaps(
		UCHAR 		*codec_caps, 
		UCHAR 		*config_codec_caps);

UCHAR A2DP_MPEG12FrameLength(
		struct A2DP_CodecBaseStru 	*codec, 
		UCHAR 						*buf, 
		WORD 						len);

WORD A2DP_MPEG12ComposeData(
		struct A2DP_CodecBaseStru * codec, 
		struct A2DP_DataComposeInStru * in, 
		struct A2DP_DataStru * out);

void A2DP_MPEG12DecomposeData(
		struct A2DP_CodecBaseStru 		*codec, 
		struct A2DP_DataDecomposeInStru 	*data_decom_in, 
		struct A2DP_DataIndStru 			*data_decom_out);

DWORD A2DP_MPEG12GetBitRate(
		UCHAR 		bitrate_index, 
		UCHAR 		fs_index);

void A2DP_MPEG12SaveConfigedCaps(
		struct A2DP_CodecBaseStru 	*codec,
		UCHAR						*media_codec_info);
	
#endif
