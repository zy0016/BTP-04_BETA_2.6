/***************************************************************************
Module Name:
    a2dputil_fun.h
    
Abstract:
	This file are header file of a2dputil.c .
	
Author:
    Wu Zhaoming
    
Revision History:
	2003.1.7	Created
	
CopyRight:
	IVT Corporation
---------------------------------------------------------------------------*/
#ifndef _A2DP_UTIL_FUN_H
#define _A2DP_UTIL_FUN_H
#include "a2dputil_struct.h"
#include "btwin32/a2dputil_dep.h"

/* interface on diff system */


typedef struct sA2dPAPP_if
{
	/* init and done of system depended */
	UCHAR (*A2DPAPP_SysDepInit)();
	UCHAR (*A2DPAPP_SysDepDone)();
	
	/* wave out play function */
	HANDLE (*A2DPAPPW_WaveOutOpen)(char *pDeviceName,AudioWaveFmt *pWaveFmt,WAVEOPEN_Config *pconfig); /* open a device to play wave */
	UCHAR (*A2DPAPPW_WaveOutClose)(HANDLE hWavePlay); /* close the device */
	UCHAR (*A2DPAPPW_GetBuf)(/*in*/HANDLE handle,AudioDataBlock *pinfo);
	UCHAR  (*A2DPAPPW_WaveOutWrite)(HANDLE hWavePlay, void *pbuffer,DWORD datalen); /*write audio data */
	UCHAR  (*A2DPAPPW_GetDeviceInfo)(DWORD *pDeviceNum,DWORD *pMaxNameLen);
	UCHAR  (*A2DPAPPW_GetCertainDeviceName)(DWORD deviceNUm,char *pName,DWORD nameLen);

	/* codec functions */
	HANDLE (*A2DPAPPF_GetCovertHandle)(AudioWaveFmt *pSrcFmt,AudioWaveFmt *pDesFmt);
	UCHAR (*A2DPAPPF_Covert)(HANDLE handle,AUDIODATACONVERT *pAudioDataconvert);
	UCHAR (*A2DPAPPF_FreeConvertHandle)(HANDLE handle);
	UCHAR (*A2DPAPPF_GetStreamBufLen)(HANDLE handle,DWORD SrcDataLen,DWORD *pDesDataLen,BOOL bDirection);
	UCHAR (*A2DPAPPF_ConvertReset)(HANDLE handle,DWORD fdwReset);
	UCHAR (*A2DPAPPF_GetPCMFmt)(AudioWaveFmt *pSrcFmt,AudioWaveFmt *pDesFmt,DWORD len);
	UCHAR (*A2DPAPPF_getcvtInfo)(DWORD *pMaxFmtLen);

	/* query codec info funcitons , for SEP register  */
	UCHAR (*A2DPAPPS_GetCodecCaps)(UCHAR *pFormatStr,DWORD StructLen,DWORD formatTag); 
	BOOL (*A2DPAPP_IFSpportFmt)(AudioWaveFmt *pSrcFmt,AudioWaveFmt *pDesFmt);

	/* if have sound device or codec */
	BOOL (*A2DPAPP_IsValidSoundCard)(char *pDeviceName);
	BOOL (*A2DPAPP_IFExistMp3Codec)();
	void (*A2DPAPPS_SelectAudioCodec)(void);
}A2DPAPP_interface;


UCHAR A2DPAPP_SysDepInit(void);
UCHAR A2DPAPP_SysDepDone(void);
HANDLE A2DPAPPW_WaveOutOpen(char *pDeviceName,AudioWaveFmt *pWaveFmt,WAVEOPEN_Config *pconfig);
UCHAR A2DPAPPW_WaveOutClose(HANDLE hWavePlay);
UCHAR A2DPAPPW_GetBuf(/*in*/HANDLE handle,AudioDataBlock *pinfo);
BOOL  A2DPAPPW_WaveOutWrite(HANDLE hWavePlay, void *pbuffer,DWORD datalen);
UCHAR  A2DPAPPW_GetDeviceInfo(DWORD *pDeviceNum,DWORD *pMaxNameLen);
UCHAR  A2DPAPPW_GetCertainDeviceName(DWORD deviceNUm,char *pName,DWORD nameLen);
HANDLE A2DPAPPF_GetCovertHandle(AudioWaveFmt *pSrcFmt,AudioWaveFmt *pDesFmt);
BOOL A2DPAPPF_IfNeedFmtCvt(AudioWaveFmt *pSrcFmt,AudioWaveFmt *pDesFmt);

UCHAR A2DPAPPF_Covert(HANDLE handle,AUDIODATACONVERT *pAudioDataconvert);
UCHAR A2DPAPPF_FreeConvertHandle(HANDLE handle);
UCHAR A2DPAPPF_GetPCMFmt(AudioWaveFmt *pSrcFmt,AudioWaveFmt *pDesFmt,DWORD len);
UCHAR A2DPAPPF_GetcvtInfo(DWORD *pMaxFmtLen);
UCHAR A2DPAPPF_GetStreamBufLen(HANDLE handle,DWORD SrcDataLen,DWORD *pDesDataLen,BOOL bDirection);
UCHAR A2DPAPPF_ConvertReset(HANDLE handle,DWORD fdwReset);
UCHAR A2DPAPPS_GetCodecCaps(UCHAR *pFormatStr,DWORD StructLen,DWORD formatTag);
BOOL A3DP2_IFNeedCvt(AudioWaveFmt *pSrcFmt,AudioWaveFmt *pDesFmt);
BOOL A3DP2_IsWaveFmtSame(AudioWaveFmt *pSrcFmt, AudioWaveFmt *pDesFmt);
UCHAR A3DP2_FmtUNPCMtoPCM(AudioWaveFmt *pSrcFmt,AudioWaveFmt *pDesFmt);
UCHAR A2DPAPP_MP3FmtFromA2DP(UCHAR *pA2DPStru,DWORD StruLen,AudioWaveFmt *pWaveFmt);
UCHAR A3DP2_FmtPCMtoUNPCM(AudioWaveFmt *pSrcFmt,AudioWaveFmt *pDesFmt);

BOOL A2DPAPP_IsValidSoundCard(char *pDeviceName);
BOOL A2DPAPP_IFExistMp3Codec(void);
void A2DPAPP_SelAudioCodec(void);

UCHAR A2DPAPP_FillSBCCaps(UCHAR *pFormatStr,DWORD StructLen);
UCHAR A2DPAPP_SBCFmtFromA2DP(UCHAR *pA2DPStru,DWORD StruLen,AudioWaveFmt *pWaveFmt);


UCHAR A2DPAPP_GetFmtFromA2DP(HANDLE hA2DPStream,AudioWaveFmt *pFmt);

HANDLE A2DPAPP_StreamOpen(AudioWaveFmt *pSrcFmt,AudioWaveFmt *pDesFmt,/*out*/WORD *flagTag);
UCHAR A2DPAPP_StreamCvt(HANDLE handle,AUDIODATACONVERT *pAudioDataconvert,WORD flagTag);
UCHAR A2DPAPP_StreamClose(HANDLE handle,WORD flagTag);
UCHAR A2DPAPP_Streambuflen(HANDLE handle,DWORD srclen,DWORD *deslen,BOOL bdire,WORD flagTag);
UCHAR A2DPAPP_AddExtroInfo(AudioWaveFmt *pFmtcvt);
#endif
