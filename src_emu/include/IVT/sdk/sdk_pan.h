/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_pan.h
  Author:       	Zhang Jing
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/

#ifndef _SDK_PAN_H
#define _SDK_PAN_H

/*************** Macro Definition ******************/


/*************** Function Prototype ******************/
BTUINT32 Btsdk_PanInit();
BTUINT32 Btsdk_PanDone();
BTUINT32 Btsdk_UpdateBtNicMACAddress(void);
BTSVCHDL Btsdk_RegisterGNService(void);
BTUINT32 Btsdk_UnregisterGNService(void);

#endif
