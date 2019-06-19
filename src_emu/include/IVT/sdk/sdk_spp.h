/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_spp.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/

#ifndef _SDK_SPP_H
#define _SDK_SPP_H
/*************** Macro Definition ******************/


/*************** Function Prototype ******************/

BTSVCHDL Btsdk_RegisterSPPService(BTUINT16 index);
BTUINT32 Btsdk_UnregisterSPPService(BTSVCHDL svc_hdl);

#endif
