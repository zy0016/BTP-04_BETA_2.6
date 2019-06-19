/***************************************************************************
Module Name:
    a2dputil.h
    
Abstract:
	This file are header file of a2dputil.c .
	
Author:
    Wu Zhaoming
    
Revision History:
	2002.12.3	Created
	
CopyRight:
	IVT Corporation
---------------------------------------------------------------------------*/
#ifndef _A2DP_UTIL_H
#define _A2DP_UTIL_H
#include "a2dputil_struct.h"
#ifdef CONFIG_OS_WIN32
#include "vaudioui.h"
#endif
#include "a2dputil_fun.h"
#include "a2dputil_ui.h"

#define A2DPAPP_HCI_BUFF_CONTROL 			1
#define A2DPAPP_SRC_MAX_BLOCK_BUF		2048//4096//ldm??? 8192 
/* interface of A2dpapp --------------------------------------------------------*/
HANDLE A2DPAPP_SRCENTRY_Create(void);
UCHAR A2DPAPP_SRCENTRY_Start(A2DPAPP_SRCENTRY *src_this);
UCHAR A2DPAPP_SRCENTRY_WriteData(A2DPAPP_SRCENTRY * src_this,UCHAR *pData,DWORD dataLen);
UCHAR A2DPAPP_SRCENTRY_WriteCertainData(A2DPAPP_SRCENTRY * src_this, int data, DWORD dataLen);
UCHAR A2DPAPP_SRCENTRY_Suspend(A2DPAPP_SRCENTRY *src_this);
UCHAR A2DPAPP_SRCENTRY_Config(A2DPAPP_SRCENTRY *src_this,HANDLE hA2DPStream,struct A2DP_GetConfigStru *pA2DPCfgCap);
UCHAR A2DPAPP_SRCENTRY_Destory(A2DPAPP_SRCENTRY *src_this);
BOOL A2DPAPP_SRCENTRY_ISValidHandle(A2DPAPP_SRCENTRY * src_this);
UCHAR A2DPAPP_SRCENTRY_SetSrcFmt(A2DPAPP_SRCENTRY * src_this,AudioWaveFmt *pSrcFmt,DWORD nMaxDataLen);
//void A2DPAPPSRC_SendDataThr(void *pParam);

#ifdef CONFIG_OS_WIN32
void MyProcessAudioMsg(AUDIOMSG_STRUCT *AudioMsg);
DWORD A2DPAPP_calPCMDataLen(AudioWaveFmt *pFmt,DWORD time);
#endif

#ifdef CONFIG_OS_WIN32
UCHAR A2DPVAUDIO_Init(void);
UCHAR A2DPVAUDIO_Done(void);
void A2DPAPP_VAudio_Start();
void A2DPAPP_VAudio_Stop();

#endif

UCHAR A2DPAPP_FillCodecCaps(UCHAR *local_attrib);

A2DPAPP_RESOURCE* A2DPAPP_Resource_Init(void);
UCHAR A2DPAPP_Resource_Done(A2DPAPP_RESOURCE* res);
void A2DPAPP_Resource_SaveSoundCardName(char *pDeviceName,DWORD nameLen);
UCHAR A2DPAPP_Resource_SaveSRCEntry(A2DPAPP_SRCENTRY*entry);
UCHAR A2DPAPP_Resource_ClearSRCEntry(void);
UCHAR A2DPAPP_Resource_SaveSNKEntry(A2DPAPP_SNKENTRY*entry);
UCHAR A2DPAPP_Resource_ClearSNKEntry(void);
 A2DPAPP_SRCENTRY *A2DPAPP_Resource_GetSRCEntry();
 A2DPAPP_SNKENTRY *A2DPAPP_Resource_GetSNKEntry();
SYSTEM_LOCK A2DPAPP_Resource_GetSRCLock();

UCHAR *A2DPAPP_Resource_FirstEntry(UCHAR issrc);
UCHAR *A2DPAPP_Resource_NextEntry(UCHAR *pos, UCHAR issrc);
UCHAR A2DPAPP_Resource_SetPauseEvent();	
void A2DPAPP_Resource_SetSvcHdl(DWORD svc_hdl, UCHAR issrc);
void A2DPAPP_Resource_ClearSvcHdl(UCHAR issrc);
DWORD A2DPAPP_Resource_GetSvcHdl(UCHAR issrc);

HANDLE A2DPAPP_SNKENTRY_Create(void);
UCHAR A2DPAPP_SNKENTRY_Config(A2DPAPP_SNKENTRY *snk_this,HANDLE hA2DPStream,struct A2DP_GetConfigStru *pA2DPCfgCap);
UCHAR A2DPAPP_SNKENTRY_Start(A2DPAPP_SNKENTRY *snk_this);
UCHAR A2DPAPP_SNKENTRY_SetSrcFmt(A2DPAPP_SNKENTRY *snk_this,AudioWaveFmt *pSrcFmt);
UCHAR A2DPAPP_SNKENTRY_Suspend(A2DPAPP_SNKENTRY *snk_this);
UCHAR A2DPAPP_SNKENTRY_Destroy(A2DPAPP_SNKENTRY *snk_this);
UCHAR A2DPAPP_SNKENTRY_NotifyDestroy(A2DPAPP_SNKENTRY *snk_this);
void A2DPAPP_SNKENTRY_FrameLen(A2DPAPP_SNKENTRY *snk_this);
UCHAR A2DPAPP_SNKENTRY_WriteData(A2DPAPP_SNKENTRY *snk_this,UCHAR *pData,DWORD dataLen);
UCHAR A2DPAPP_SNKENTRY_WriteFrame(A2DPAPP_SNKENTRY *snk_this,UCHAR *pData,DWORD dataLen);
void A2DPAPPSNK_ReceiveDataThr(void);

UCHAR A2DPAPP_A2dpCallback (UCHAR event, UCHAR* param);
void A2DPAPP_RegCbk(UCHAR * hdl, UCHAR * pfunc);
UCHAR A2DPAPP_QueryEntryByUpperLayer(UCHAR * hdl, UCHAR *sepType);
void A2DPAPP_RegCbkAuto(UCHAR * pfunc);
HANDLE A2DPAPP_QueryEntry(HANDLE hA2DPStream,UCHAR *sepType);
BOOL A2DPAPP_BDADD_IsExist(UCHAR *bd_add);
void A2DPAPP_BDADD_Add(UCHAR *bd_add);
BOOL A2DPAPP_BDADD_Remove(UCHAR *bd_add);
void A2DPAPP_StreamConn_Ind(UCHAR* cb, UCHAR* hdl, UCHAR* bd,BOOL bSvr);
void A2DPAPP_StreamDisc_Ind(UCHAR* cb, UCHAR* hdl, BOOL isserver, UCHAR*bd);

UCHAR A2DPAPP_A2DPCbk_Config(UCHAR* param);
UCHAR A2DPAPP_A2DPCbk_Open(UCHAR* param);
UCHAR A2DPAPP_A2DPCbk_Start(UCHAR* param);
UCHAR A2DPAPP_A2DPCbk_Abort(UCHAR* param);
UCHAR A2DPAPP_A2DPCbk_Reconfig(UCHAR* param);
UCHAR A2DPAPP_A2DPCbk_Suspend(UCHAR *param);

extern A2DPAPP_RESOURCE *g_a2dpapp_resource;
extern char g_btavini_filename[A2DPAPP_MAXFOLDERPATH];

/*------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
MP3 frequency and bitrate table:

frequency table:
	int mp3_bitrate[16] = {0, 32000, 40000, 48000, 56000, 64000, 80000, 96000, 112000, 
			       		   128000, 160000, 192000, 224000, 256000, 320000, 0};
Bitrate table:
	int mp3_frequency[4] = {44100, 48000, 32000, 1};
////////////////////////////////////////////////////////////////////////////
SBC 
/////////////////////////////////////////////////////////////////////////////
WMA
---------------------------------------------------------------------------------------*/

/*
struct AppEvConn {
	WORD clsid;    // local role: CLS_AUDIO_SINK or CLS_AUDIO_SOURCE
	UCHAR bd[6];	// Remote bd address//
};
*/
#endif
