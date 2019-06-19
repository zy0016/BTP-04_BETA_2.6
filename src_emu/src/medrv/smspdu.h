/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Mobile Engine
 *
 * Purpose  : 
 *
\**************************************************************************/
#ifndef _SMS_PDU_H

#define _SMS_PDU_H

#define ME_SMS_PDU_LEN			660

int	ME_TranStr_Code( PSMS_SEND pSMS_Send, char* pDesStr, int len );
int	ME_RecStr_Code( PSMS_INFO pDesSMS_Read, char* pPDUStr, int stat );
int	ME_RecStr_Flag( PSMS_INFO pSMSRead, char* SMS_Prompt );
int ME_RecLstStr_Code( PSMS_INFO pSMSInfo, char* pSmsContext, int Count );
int HexStringToInt(const char *pString);
int AnalyseCPHSRecordSMS(SMS_CPHS_INFO * psms_chps_info,char * str);
#endif
