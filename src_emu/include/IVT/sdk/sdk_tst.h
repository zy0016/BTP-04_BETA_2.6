#ifndef _SDK_TST_H
#define _SDK_TST_H

#include <stdio.h>
#include <stdlib.h>
/*
#include <windows.h>
#include <conio.h>*/


#include "sdk_ui.h"
#include "sdk_hlp.h"

#define FIXPINCODEVAL "1"

#define MAX_DEV_NUM			6

extern BTDEVHDL g_rmt_dev_hdls[MAX_DEV_NUM];
extern BTINT32 g_rmt_dev_num;

/*
void RegAppIndCallback(void);
void UnRegAppIndCallback(void);

void DisplayRemoteDevices(BTUINT32 dev_class);
BTDEVHDL SelectRemoteDevice(BTUINT32 dev_class);
void StartSearchDevice(BTUINT32 device_class);
void PrintBdAddr(BTUINT8 *bd_addr);
int MultibyteToMultibyte(BTUINT32 dwSrcCodePage, char *lpSrcStr, int cbSrcStr,
						 BTUINT32 dwDestCodePage, char *lpDestStr, int cbDestStr);

void CtpIcpInit(void);
void CtpIcpDone(void);
void HfpInit(void);
void HfpDone(void);
void TestLocDevMgr(void);
void TestRmtDevMgr(void);
void TestSecMgr(void);
void TestSvcSch(void);
void TestSvcRegMgr(void);
void TestShcMgr(void);

void TestCtpIcp(void);
void TestHfp(void);
void TestFTPFunc(void);
void TestOPPFunc(void);

void OppServerReceiveFileInd(BTUINT8 *pFilePathName);*/

#endif
