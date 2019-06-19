/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_lap.h
  Author:       Liao Cheng
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/



#ifndef _SDK_LAP_H
#define _SDK_LAP_H

/*used in the BTSDK_APP_EV_LAP_AVAILABLE_IND event*/
struct LAP_ConnInfo {
    BTUINT8 is_lap;    /*whether lap is connected*/
	BTUINT8 *bd_addr;  /*peer bd address*/
};

struct BTSDK_LAP_ConnInfo {
    BTUINT8 is_lap;    /*whether lap is connected*/
	BTCONNHDL hdl;  /*peer bd address*/
};

struct LAP_CltLocalAttrStru {
	BTUINT8 com_index;
};

typedef void (BTSDK_LAP_SVR_Callback)(BTUINT8 event, BTUINT8 *param, BTUINT16 param_len);

BTSVCHDL Btsdk_RegisterLAPService(BTUINT16 index);
BTUINT32 Btsdk_UnregisterLAPService(void);
void Btsdk_Lap_APPRegCbk(BTSDK_LAP_SVR_Callback *pfunc);

#endif
