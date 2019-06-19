/* sbc.h */
#ifndef _SBC_H_
#define _SBC_H_

#include "global.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#include "a2dpui.h"

/* data type */
#ifndef WORD
#define WORD	unsigned short
#endif

#ifndef DWORD
#define DWORD	unsigned long
#endif

#ifndef UCHAR
#define UCHAR	unsigned char
#endif

#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

#define MAX_CH	2
#define MAX_SB	8
#define MAX_BLK 16

#define MAX_SF	4

/* sampling_frequency */
#define SBC_00	0
#define SBC_01	1
#define SBC_10	2
#define SBC_11	3

#define SBC_SF_16	0
#define SBC_SF_32	1
#define SBC_SF_44	2
#define SBC_SF_48	3

/* blocks */
#define SBC_BLOCKS_4	0
#define SBC_BLOCKS_8	1
#define SBC_BLOCKS_12	2
#define SBC_BLOCKS_16	3

#define SBC_BLOCK_NUM(x)	(((x)+1) <<2)
#define SBC_BLOCKS(y)		(((y)>>2)-1)

/* channel mode */
#define SBC_CHANNEL_MODE_MONO			0
#define SBC_CHANNEL_MODE_DUAL			1
#define SBC_CHANNEL_MODE_STEREO			2
#define SBC_CHANNEL_MODE_JOINT_STEREO			3

#define SBC_CHANNEL_NUM(x)		((x) == 0 ? 1 : 2)
/* allocation method */
#define SBC_AM_LOUDNESS					0
#define SBC_AM_SNR						1

/* subbands */
#define SBC_SUBBANDS_4		0
#define SBC_SUBBANDS_8		1

#define SBC_SUBBAND_NUM(x)		((x) == 0 ? 4 : 8)
#define SBC_SUBBANDS(y)		((y) == 4 ? 0 : 1)

#define SBC_MAXFRMLEN_MONODUAL			524
#define SBC_MAXFRMLEN_STEREO			525

#define SBC_MAXFRMLEN					532

#define SBC_STREAMSIZEF_DESTINATION		0
#define SBC_STREAMSIZEF_SOURCE			1

#define SBC_HEADER_LEN	4

#define SBC_SAMPLES_PER_FRAME(a)		(a->nrof_blocks)*(a->nrof_channels)*(a->nrof_subbands)

#define SBC_FRAME_STATISTICS 1
#define SBC_JOINT_CODING_OPTIMIZE 1

/* frame header */
typedef struct s_sbc_frame_hdr {
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
 } sbc_frame_hdr;


typedef double Joint_Sb_Sample[MAX_BLK][MAX_CH][MAX_SB];
typedef UCHAR Joint_Scale_Factor[MAX_CH][MAX_SB];

/* frame context */
typedef struct s_sbc_frame_context {

	/*analyzed frame information*/
	UCHAR			sampling_frequency;
	UCHAR			nrof_blocks;
	UCHAR			channel_mode;
	UCHAR			allocation_method;
	UCHAR			nrof_subbands;	
	UCHAR			bitpool;
	UCHAR			crc_check;
	UCHAR			crc_error_count;

	WORD 			pcm_samples_per_sbc_frame;		// = nrof_blocks * nrof_channels * nrof_subbands
	
	UCHAR   		joint[MAX_SB]; /*See P53*/
	UCHAR		 	scale_factor[MAX_CH][MAX_SB];	
	UCHAR			nrof_channels;
	UCHAR			is_sbc2pcm;
	WORD			framelength;	
	WORD			incomplete_frame_len;	/*if it is zero, indicates that no incomplete frame.*/
	UCHAR			incomplete_frame_buf[SBC_MAXFRMLEN];
	

	/*common help variables for encoder & decoder */
	char 			bitneed[MAX_CH][MAX_SB]; 	
	char 			bits[MAX_CH][MAX_SB];
	unsigned short 	levels[MAX_CH][MAX_SB];
	unsigned short 	audio_sample[MAX_BLK][MAX_CH][MAX_SB];
//	double 			sb_sample[MAX_BLK][MAX_CH][MAX_SB];		
	double			*sb_sample;
	char			bitslice;
	UCHAR			bitcount;

	
	short 			pcm_sample[MAX_CH][MAX_SB];  		/* for decoder & encoder */

	/*  only for joint-encoder in order to reduce STACK, but increased RAM !!! */
//	double 			joint_sb_sample[MAX_BLK][MAX_CH][MAX_SB];
#ifndef SBC_JOINT_CODING_OPTIMIZE
	Joint_Sb_Sample	*joint_sb_sample;
#endif
	UCHAR		 	joint_scale_factor[MAX_CH][MAX_SB];


	/*  only for sbc synthesis & analysis in order to reduce STACK, but increased RAM !!! */
//	double 			v_x[MAX_CH][160];
	double 			*v_x;
	short			bufoffset[MAX_CH];
	double 			*w_y;	/* only for encoder */
		
	UCHAR 			*srcbuf;         /* bit stream buffer */
  //DWORD        	bit_count;         /* bit counter of bit stream */
    short         	srcbuf_byte_idx;   /* pointer to top byte in buffer */
    char         	srcbuf_bit_idx;    /* pointer to top bit of top byte in buffer */

#ifdef SBC_FRAME_STATISTICS    
	DWORD			frame_count;
#endif
 } sbc_frame_context;


typedef struct s_sbc_info{
	UCHAR	sampling_frequency;
	UCHAR	blocks;
	UCHAR	channel_mode;
	UCHAR	allocation_method;
	UCHAR	subbands;
	UCHAR	nrof_channels;
	UCHAR	bitpool;
 } sbc_info;

typedef struct s_sbc_stream_header{ 
    UCHAR*     pbSrc; 
    DWORD      cbSrcLength; 
    DWORD      cbSrcLengthUsed; 
    UCHAR*     pbDst; 
    DWORD      cbDstLength; 
    DWORD      cbDstLengthUsed; 
} sbc_stream_header; 


typedef struct s_sbc_stream {
	UCHAR*	hdl;			/**/	
}sbc_stream;




/*********************************** sbc ui functions ****************************/

DWORD sbc_init(void);
void sbc_done(void);

int sbc_validate_params(
	sbc_info		*si);

UCHAR sbc_get_frameinfo(
	UCHAR			*sbcframebuf, 
	DWORD			sbcframebuflen,
	sbc_info		*info,
	WORD			*framesize);
int	sbc_streamopen(
	HANDLE			*phss,
	AudioWaveFmt 	*psrcfmt,
	AudioWaveFmt 	*pdstfmt);

UCHAR sbc_streamsize(
	HANDLE			hss,
	DWORD 			cbInput,        
	DWORD 			*pdwOutputBytes,
  	DWORD 			fdwSize);

UCHAR sbc_streamconvert(
	HANDLE				hss, 
	sbc_stream_header* 	pssh);

#ifdef SBC_FRAME_STATISTICS
DWORD sbc_get_convert_frame_count(HANDLE hss);
#endif

UCHAR sbc_streamclose(
	HANDLE				hss);


WORD sbc_calculate_framelen(
	UCHAR	channel_mode,
	UCHAR	subbands,
	UCHAR	blocks,
	UCHAR	bitpool);

#endif
