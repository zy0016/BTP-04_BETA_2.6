/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_dun.h
  Author:       Liao Cheng
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/



#ifndef _SDK_DUN_H
#define _SDK_DUN_H

struct BTSDK_DUN_ConnInfo {
    BTUINT8 is_dun;    /*whether dun is connected*/
	BTCONNHDL hdl;  /*peer bd address*/
};

typedef void (BTSDK_DUN_SVR_Callback)(BTUINT8 event, BTUINT8 *param, BTUINT16 param_len);

BTSVCHDL Btsdk_RegisterDUNService(BTUINT16 index);
BTUINT32 Btsdk_UnregisterDUNService(void);
void Btsdk_Dun_APPRegCbk(BTSDK_DUN_SVR_Callback *pfunc);

#endif
