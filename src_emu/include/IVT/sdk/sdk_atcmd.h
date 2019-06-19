/********************************************************************
  Copyright (C), 2005, IVT Corporation
  Module name:		sdk_atcmd.h
  Author:       guozhengfei
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/

#ifndef _sdk_ATCMD_H
#define _sdk_ATCMD_H

#include "sdk_atparse.h"
#define MAX_AT_BUF_SIZE 40

void Btsdk_ATResp_Register(AT_Resp_Func * func);
int  Btsdk_ATinit(void);
int  Btsdk_ATclose(void);
#endif
