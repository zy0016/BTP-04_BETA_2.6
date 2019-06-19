/******************************************************
Module Name:
    a2dp.h
Abstract:
	This file provides a2dp.c  fun.
Author:
    wu zhaoming
Revision History:
	2003.07.24	Created


******************************************************/
#ifndef A2DP_H
#define A2DP_H

STREAMHANDLE A2DPAPP_CreateStream(UCHAR * bd, UCHAR * sdp_attrib, UCHAR * local_attrib,A2DP_CONFIGFILESTRU *pconfig,struct A2DP_GetConfigStru *pA2DPCfgCap);
DWORD  A2DP_RegisterService(UCHAR svc_type, UCHAR * local_attrib);
DWORD A2DP_UnregisterService(DWORD svr_hdl);
#ifdef CONFIG_OS_WIN32
UCHAR A2DP_ReadFile(A2DPAPP_RESOURCE *pres);
A2DP_CONFIGFILESTRU * A2DP_ReadConfigFile();
#endif

#ifdef CONFIG_INTER_TESTER
#define A2DPAPP_STREAM_OPEN_CLNT			0x10
#define A2DPAPP_STREAM_START_CLNT		0x11
#define A2DPAPP_STREAM_RELEASE_CLNT		0x12
#define A2DPAPP_STREAM_ABORT_CLNT		0x13
#define A2DPAPP_STREAM_SUSPEND_CLNT		0x14

extern A2DPAPP_CallBack *p_a2dpapp_cbk;
#endif

extern DWORD mpeg1_bitrate[16];

#endif
