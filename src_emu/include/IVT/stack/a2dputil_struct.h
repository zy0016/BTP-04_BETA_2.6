/***************************************************************************
Module Name:
    a2dputil_struct.h
    
Abstract:
	This file are header file of a2dputil.c .
	
Author:
    Wu Zhaoming
    
Revision History:
	2003.1.7	Created
	
CopyRight:
	IVT Corporation
---------------------------------------------------------------------------*/
#ifndef _A2DP_UTIL_STRUCT_H
#define _A2DP_UTIL_STRUCT_H

#include "autoconf.h"
#include "a2dpui.h"

#ifndef BD_ADDR_LEN
#define BD_ADDR_LEN		6
#endif

#define A2DPAPP_BLUESOLEIL

/* error code */
#define A2DPAPP_ERROR						0
#define A2DPAPP_RETURN_SUCC				1

#define A2DPAPP_ERROR_WAVEWRITEOPEN		2
#define A2DPAPP_ERROR_ACM					3
#define A2DPAPP_ERROR_VAUDIO				4
#define A2DPAPP_ERROR_INVALIDPARA		5
#define A2DPAPP_ERROR_WAVEOUTOPEN		6
#define A2DPAPP_ERROR_NOSOUNDCARD		8 	/* no related sound card  */
#define A2DPAPP_ERROR_NOPROMP3CODEC		9	/* professional mp3 codec is not installed */
#define A2DPAPP_ERROR_NOWAVEHDR			10   /* no wave hdr to use*/
#define AV_ERROR_GAVDPCONNECT			11   // gavdp connect error
#define A2DP_ERROR_NOREMSEP				12   //can't get the remote SEP
#define A2DP_ERROR_NOMATCHSEP			13   
#define A2DP_ERROR_STREAMCONFIGURE		14 
#define A2DP_ERROR_STREAMOPEN                     15  
#define A2DP_ERROR_STREAMSTART                   16  
#define A2DPAPP_ERROR_HAVESTREAM			17  // there have a stream already


/*temp macro*/
#define A2DPAPP_SRCSVCHDL			0x01
#define A2DPAPP_SNKSVCHDL			0x02
//#define A2DPAPP_FIRSTPLAYBUFFERNUM		 5   /* at first play ,the number of buffer */
//#define A2DPAPP_SNKBUFFERNUMBER			60
#define A2DPAPP_SNKBUFLENGTH				6000 /*   > 4086       */

#define A2DPAPPSRC_PCM2Mp3BUF_PCMLEN		1920    // > 1920
#define A2DPAPPSRC_PCM2SBCBUF_PCMLEN		1920      //
#define A2DPAPPSRC_MP3BUF_EXTRALEN			2048	
#define A2DPAPPSRC_SBCBUF_EXTRALEN			2048	
//#define A2DPAPP_WAVEPLAYWAITTIME			5   /* when play header is null, wait time (ms)*/

/*for specific callback function*/
#define A2DPAPP_CBK_MP3CODEC				1

typedef UCHAR (A2DPAPP_CODECCBK) (char *codecName, DWORD version);

typedef struct _SPECIFICCBK
{
	A2DPAPP_CODECCBK *pcodecCbk;
}SpecifcCBK;

/*48k*/
#define A2DPAPP_VAUDIO_DATALEN				1920  /*vaudio report data length*/


#define A2DPAPP_SILENCEDATA_NUM			24	/* when vaudio closed, then send serveral silence data*/

#define  A2DPAPP_INFINITE 					0xFFFFFFFF

#define A2DPAPP_EXIT_RECEIVE_THREAD_WAIT_FOR_MS 400
#define A2DPAPP_READ_THREAD_WAIT_TIME_ONCE	200


#define A2DPAPP_SNKSVRENTRYNUM			2
#define A2DPAPP_SRCSVRENTRYNUM			1

#define A2DPAPP_MAXFOLDERPATH			256

#define A2DPAPP_SEPNUMBER					4

//hci link lost time out
#define A2DPAPP_HCI_LINKLSOT_TIMEOUT		5000
#ifndef MAX
#define MAX(a,b) (((DWORD)a>(DWORD)b)?a:b)
#endif

/* audio data block struct , a buffer */ 
typedef struct s_AudioDataBlock
{
	union {
		void 			*pData;   /*buffer address*/
		struct BuffStru	*pBuffStruData; 
	};
	DWORD dataLen;  /* buffer length*/
}AudioDataBlock,*LPAudioDataBlock;

typedef struct s_SNKAUDIODATA
{
	void *pData;
	DWORD dataLen;
	BOOL	bUsed;  /* indicate if this buffer is complete by sound card*/
}A2DP_SNKAUDIODATA,*LPA2DP_SNKAUDIODATA;

 /* for audio data convert struct */
 typedef struct s_AudioDataConvertStr
 {
 	void 	*pSrcData;
 	DWORD 	SrcDataLen;
 	void 	*pDesData;
 	DWORD 	DesDataLen;
 	DWORD   DesDataLenUsed;
 }AUDIODATACONVERT;
 
typedef struct openconfig
{
	DWORD buflen;
	DWORD whdnum;
}WAVEOPEN_Config;

/* mpeg 1 layer 3,(mp3) frame header struct */
typedef struct
{
	UINT		sync			:12;// A
	UINT		eVersion		:1; // B
	UINT		LayerNo			:2; // C
	UINT		protect			:1; // D
	UINT		bitRateIdx		:4; // E
	UINT		eFreq			:2; // F
	UINT		padding 		:1; // G
	UINT		extension		:1; // H
	UINT		eMode			:2; // I
	UINT		modeExt		:2; // J
	UINT		copyright		:1; // K
	UINT		original			:1;	// L
	UINT		emphasis		:2; // M   original/copy	
}MP3FrameHdr;

typedef void (A2DPAPP_CallBack) (WORD event, UCHAR * hdl, UCHAR * param);

/*src entry struct*/
typedef struct A2DPAPPSRCENTRY
{
	/* hCvtStream and hA2DPStream is controled in gerneral block  	*/
	HANDLE		hCvtStream;		/* convert stream handle		*/
	
	HANDLE		hA2DPStream;	/* handle of a2dp stream    		*/

	/* followed var is controled in vaudio block                            		 */
	AudioDataBlock pSRCDataBlock;/* src format  , data block 	*/
	AudioDataBlock pDESDataBlock; /* des format , data block         */
	
	/* waveformat */
	AudioWaveFmt srcAudioFmt;
	AudioWaveFmt desAudioFmt;

	/*write data information , BtList buffer len*/
	DWORD maxInputDataLen;
	/*write to a2dp buffer length, include pSRCDataBlock and pDesDataBlock */
	DWORD desBufLen;
	DWORD srcBufLen;
	
	WORD   remaindatasize;	/*just for SBC now*/
	
	UCHAR peer_bd[BD_ADDR_LEN];

	UCHAR bDestroydire;/*1: uplayer 2: indication*/

}A2DPAPP_SRCENTRY;


typedef struct A2DPAPPSNKRESOURCE
{
	/* snk resource , now , the item only one*/	
	/* followed var is controled in received data thread                  */
	BOOL		bExitReceiveThr;/* control exit received data thread*/
	BOOL		bPause;
	HANDLE		hExitRevThrEvent; /* control exit received data    */
	HANDLE 		hStartReceiveEvent;
	struct BtList 	*pSNKList;
	CHAR 		*pDeviceName; 
	DWORD		MaxNameLen;
	
}A2DPAPP_SNKRESOURCE;

typedef struct A2DPAPPBD_ADDStruct
{
	UCHAR bd_add[BD_ADDR_LEN];
	UCHAR refcount;
	UCHAR reseved1;
}A2DPAPP_BDADD;

typedef struct A2DP_ConfigFileStru{	
	WORD 	mp3_channels; 	// for mp3 and sbc
	WORD 	mp3_fs;			//for mp3 and sbc	
	WORD	mp3_bitrate;		//for mp3 only

	WORD	sbc_channels;
	WORD 	sbc_fs;
	WORD 	max_bitpool;
	WORD 	min_bitpool;
	WORD	block;
	WORD	subband;
	WORD 	alloc_meth;

	WORD     mtu;
}A2DP_CONFIGFILESTRU;


/* a entry of a2dp in snk */
typedef struct A2DPAPPSNKENTRY
{
	/* followed var is controled in gerenral control block				*/
	HANDLE		hCvtStream;		/* convert stream handle 		*/
	
	HANDLE		hA2DPStream;	/* handle of a2dp stream    		*/

	/* followed var is controled in play data thread                 		*/
	BOOL		bSuspend;  	
	HANDLE 		hWavePlay;

	/* waveformat */
	AudioWaveFmt srcAudioFmt;
	AudioWaveFmt desAudioFmt;	

	DWORD		nFrameLen;

	UCHAR *pData;
	UCHAR bDestroydire; /*1: uplayer 2: indication*/

	UCHAR peer_bd[BD_ADDR_LEN];

	SYSTEM_LOCK snk_lock;

}A2DPAPP_SNKENTRY;



/* format cvt struct */
typedef struct sA2DP_FmtCvt
{
	WORD    decomflag;	/*which codec decompress use*/
	HANDLE 	hdecom;	/* handle of decompree */
	WORD 	cvtflag;		/* which codec cvt use */
	HANDLE 	hcvt;		/**/
	WORD	compreflag;	/**/
	HANDLE 	hcompre;	/**/
}A2DPAPP_FmtCvt;

struct A2DPEvConn {
	WORD clsid;     // local role: 
	UCHAR bd[6]; 	// Remote bd address//
};

struct A2DPAPP_DataRcvd {
	DWORD	timestamp;  			/* the timestamp of the data packet */
	struct BuffStru *buf;
};
typedef struct A2DPAPP_RESOURCE_STRUCT
{
	UCHAR 					*pGUICallBack;	/*BlueSoleil or upper app's call back func*/
	struct BtList			*pRemoteAddrList;
	A2DPAPP_SRCENTRY		*pSRCEntry;

	/*for snk*/
	BOOL					bExitReceiveThr;/* control exit received data thread*/
	BOOL					bPause;
	HANDLE					hExitRevThrEvent; /* control exit received data    */
	HANDLE 					hStartReceiveEvent;
	A2DPAPP_SNKENTRY		*pSNKEntry;
	CHAR 					*pDeviceName; 
	DWORD					MaxNameLen;
	SYSTEM_LOCK			srcentry_lock;
	struct BtList				*pSNKRcvDataList;

	char						*pfilename;
	A2DP_CONFIGFILESTRU		configStru;

	DWORD					srcSvcHdl;   			//sdp service handle
	DWORD					snkSvcHdl;			//sdp service handle

	UCHAR					seid[A2DPAPP_SEPNUMBER];
	/*for src*/
	BOOL 					bSrcEntryDestroy;
}A2DPAPP_RESOURCE;

#endif
