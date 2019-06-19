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

#ifndef		_BTDEVICENODE_H_
#define		_BTDEVICENODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sdk_ui.h"
#include "sdk_hlp.h"

typedef struct tagDeviceNode
{
   BTUINT8     DevName[BTSDK_DEVNAME_LEN];
   BTUINT8     DevBdAddr[BTSDK_BDADDR_LEN];
   BTUINT32    DevHandle;
   BTUINT32    DevClass;
   struct      tagDeviceNode   *Next;
}DeviceNode;

typedef DeviceNode *DeviceNodeLink;

void Free_DeviceNodeList(DeviceNodeLink Head);
DeviceNodeLink Append_DeviceNodeList(DeviceNodeLink Head,DeviceNodeLink DevInfo);

BOOL IsExsitDeviceInList(DeviceNodeLink Head,BTUINT8 *BdAddr);

#endif
