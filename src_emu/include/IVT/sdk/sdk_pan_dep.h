/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_conn.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/


#ifndef _SDK_PAN_DEP_H
#define _SDK_PAN_DEP_H


BTUINT32 SDPAN_SysDepInit(void);
BTUINT32 SDPAN_SysDepDone(void);
BTUINT32 SDPAN_UpdateMacAddress(void);

void SDPAN_PostConnected(PConnectionManagerStru new_conn);
void SDPAN_PostDisconnected(PConnectionManagerStru cur_conn);


#endif
