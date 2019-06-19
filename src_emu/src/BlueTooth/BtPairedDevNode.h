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

#ifndef		_BTPAIREDDEVNODE_H_
#define		_BTPAIREDDEVNODE_H_

#include "BtLowCall.h"

typedef struct tagPAIREDDEVNODE
{
	BTUINT8     PairedDevBdAddr[BTSDK_BDADDR_LEN];
	BTUINT8     PairedDevName[BTSDK_DEVNAME_LEN];
    BTUINT32    PairedDevHandle;
	BOOL        PairedDevAuthor;
    BTUINT8     PairedDevAliasName[BTSDK_DEVNAME_LEN];
	BTCONNHDL   PairedDevConnhandle;
	struct      tagPAIREDDEVNODE *Next;
}PAIREDDEVNODE,*PPAIREDDEVNODE;

typedef struct tagPAIREDDEVFIlEINFO
{
	BTUINT8     PairedDevBdAddr[BTSDK_BDADDR_LEN];
    BTUINT8     PairedDevName[BTSDK_DEVNAME_LEN];
	BTUINT8     PairedDevAliasName[BTSDK_DEVNAME_LEN];
	BOOL        PairedDevAuthor;
}PAIREDDEVFIlEINFO,*PPAIREDDEVFIlEINFO;

void FreePairedDevNode(PPAIREDDEVNODE Head);
PPAIREDDEVNODE AppendPairedDevNode(PPAIREDDEVNODE Head,PPAIREDDEVNODE New);
PPAIREDDEVNODE DeletePairedDevNode(PPAIREDDEVNODE Head,BTDEVHDL Dev_Hdl);
PPAIREDDEVNODE GetDeviceNodePointer(PPAIREDDEVNODE Head,BTDEVHDL dev_hdl);

PPAIREDDEVNODE GetPairedDevInfoFromFile();
void SetPairedDevInfoToFile(PPAIREDDEVNODE pPairedDevHead);

#endif
