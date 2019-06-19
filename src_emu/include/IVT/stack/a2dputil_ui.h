/***************************************************************************
Module Name:
    a2dputil.h
    
Abstract:
	This file are header file of a2dputil.c .
	
Author:
    Wu Zhaoming
    
Revision History:
	2003.5.3	Created
	
CopyRight:
	IVT Corporation
---------------------------------------------------------------------------*/
#ifndef _A2DPUTIL_UI_H
#define _A2DPUTIL_UI_H

#include "btwin32/a2dputil_dep.h"
#include "a2dputil_struct.h"
#include "a2dputil.h"
#include "a2dputil_fun.h"
#include "a2dp.h"


UCHAR A2DPAPP_Init(void);
UCHAR A2DPAPP_Done(void);
UCHAR A2DPAPPSRC_StartSend(AudioWaveFmt *pAudioWaveFmt,DWORD nMaxDataLen);

UCHAR A2DPAPPSRC_EndSend(void);
UCHAR A2DPAPPSRC_WriteData(DWORD dataLen,UCHAR *pData);
UCHAR * A2DPAPPSRC_SvrStart(UCHAR *local_attrib);
UCHAR * A2DPAPPSRC_ClntStart(UCHAR * bd, UCHAR * sdp_attrib, UCHAR * local_attrib);
DWORD A2DPAPPSRC_ClntStop(UCHAR *handle);
UCHAR *A2DPAPPSRC_SvrStart(UCHAR *local_attrib);
UCHAR A2DPAPPSRC_SvrStop(UCHAR *Handle);

UCHAR *A2DPAPPSNK_SvrStart(UCHAR *local_attrib);
UCHAR *A2DPAPPSNK_ClntStart(UCHAR * bd, UCHAR * sdp_attrib, UCHAR * local_attrib);
UCHAR A2DPAPPSNK_ClntStop(UCHAR *handle);
UCHAR A2DPAPPSNK_SvrStop(UCHAR *Handle);
UCHAR A2DPAPP_GetErrorCode();
void A2DPAPP_SetErrorCode(UCHAR errorCode);
#ifdef CONFIG_OS_WIN32
void A2DPAPP_SwitchUSBAudio(char *pDeviceName);
#endif
// inner fun
UCHAR A2DPAPP_RegisterSEP(void);
void A2DPAPP_UnRegisterSEP(void);

void A2DPAPP_RegSpecificCbk(UCHAR event, UCHAR *pcbk);
void A2DPAPP_SelectAudioCodec(void);

#ifdef CONFIG_INTER_TESTER
void A2DPAPP_Stream_Suspend(UCHAR *hdl);
void A2DPAPP_Stream_Start(UCHAR *hdl);
void A2DPAPP_RegTesterCbk(UCHAR *pfun);
#endif

#endif
