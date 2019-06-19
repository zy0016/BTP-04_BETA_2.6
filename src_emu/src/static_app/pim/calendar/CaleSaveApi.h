/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleSaveApi.h
*
* Purpose  : 
*
\**************************************************************************/

#ifndef _CALE_SAVE_API_H_
#define _CALE_SAVE_API_H_

#include "CaleHeader.h"

int CALE_OpenApi(int CaleEntry);
BOOL CALE_CloseApi(int Cal_OpenFile);
int CALE_ReadApi(int Cal_Open, int itemID, BYTE *pRecord, int size);
int CALE_AddApi(int CaleEntry, int Cal_Open, BYTE *pRecord, int size);
BOOL CALE_ModifyApi(int Cal_Open, int itemID, BYTE *pRecord, int size);
BOOL CALE_DelApi(int CaleEntry, int Cal_Open, int itemID, BYTE *pRecord, int size);
BOOL CALE_CleanApi(void);

#endif
