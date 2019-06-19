/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Bluetooth module
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef	    _BTSENDDATA_H_
#define		_BTSENDDATA_H_

#include "window.h"
#include "plx_pdaex.h"
#include "winpda.h"
#include "mullang.h"
#include "pubapp.h"

#include "sdk_def.h"

typedef enum
{
    BTNOTE=1,
	BTPICTURE,
    BTCARDCALENDER,
    BTMULTIPICTURE
}BTFILETYPE;

typedef struct tagBTCONNFILENAME
{
	BTCONNHDL conn_hdl;
    char  szFileName[256]; 
	char  szViewFileName[64];
	int   iFiletype;
}BTCONNFILENAME, *PBTCONNFILENAME;

typedef struct tagBTCONNLIST
{
	BTCONNHDL conn_hdl;
    char* pListHead;
	int   iFiletype;
}BTCONNLIST, *PBTCONNLIST;

BOOL BtSendData(HWND hAppFrameWnd,char* szAppPathName,char* szViewFileName,int iFileType);

#endif
