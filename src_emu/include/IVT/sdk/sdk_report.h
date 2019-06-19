/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_report.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/



#ifndef _SDK_REPORT_H
#define _SDK_REPORT_H

void Btsdk_PrintLocalDeviceInfo(void);
void Btsdk_PrintLocalServiceInfo(void);
void Btsdk_PrintRemoteDeviceInfo(BTDEVHDL dev_hdl);
void Btsdk_PrintAllRemoteDeviceInfo(void);
void Btsdk_PrintConnectionInfo(BTCONNHDL dev_hdl);
void Btsdk_PrintCurTime(void);
void Btsdk_PrintAllReport(void);
void Btsdk_PrintShortcutInfo(void);

#endif
