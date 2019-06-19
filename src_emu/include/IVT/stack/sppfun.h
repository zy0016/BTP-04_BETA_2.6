/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    sppfun.h
Abstract:
	This file includes the definition of interface functions in the sppui.c
Author:
    chen zhongyi
Revision History:2000.9
---------------------------------------------------------------------------*/



#ifndef __SPPFUN_H
#define __SPPFUN_H

BOOL SPP_Init(void);
void SPP_Done(void);

BOOL SPP_AddEntry(struct SPP_EntryParStru *par,UCHAR *vport);
BOOL SPP_DelEntry(UCHAR vport);
BOOL SPP_Open(UCHAR vport);
BOOL SPP_Close(UCHAR vport);
BOOL SPP_ReadData(UCHAR vport,UCHAR *buf,DWORD nRequest,DWORD *pNRead);
BOOL SPP_WriteData(UCHAR vport,UCHAR *buf,DWORD nRequest,DWORD *pNWritten);
BOOL SPP_AddCredit(UCHAR vport, UCHAR credit);
BOOL SPP_SetCfg(UCHAR vport,struct struRpnPar *portcfg);
BOOL SPP_MdmCtrl(UCHAR vport,UCHAR signal);
BOOL SPP_SendError(UCHAR vport,UCHAR error);
BOOL SPP_TestLink(UCHAR vport);
BOOL SPP_GetCfg(UCHAR vport,struct struRpnPar *portcfg);
BOOL SPP_GetModemStatus(UCHAR vport,UCHAR *ctrl);
BOOL SPP_GetFlowStatus(UCHAR vport,UCHAR *congest);
BOOL SPP_GetRemoteAddress(UCHAR vport,UCHAR *bd);
BOOL SPP_GetMfs(UCHAR vport,WORD *mfs);
BOOL SPP_GetConnStatus(UCHAR vport);

const char * SPP_GetVersion(void);

#ifdef CONFIG_SPP_CLIENT
UCHAR * SPP_ClntStart(UCHAR *bd,UCHAR *spp_attrib,UCHAR *local_attrib);
UCHAR SPP_ClntStop(UCHAR *info);
#else
#define SPP_ClntStart(a,b,c)	0
#define SPP_ClntStop(a)			0
#endif

#ifdef CONFIG_SPP_SERVER
UCHAR* SPP_SvrStart(UCHAR *local_attrib);
UCHAR  SPP_SvrStop(UCHAR *info);
#else
#define SPP_SvrStart(a) 		0
#define SPP_SvrStop(a)			0
#endif

void SPPAPP_RegCbk(UCHAR *hdl,UCHAR*pfunc);

#endif
