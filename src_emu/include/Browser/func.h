/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef WML_FUNC_H
#define WML_FUNC_H

#include "PWBE.h"

HBROWSER App_WapRequestUrl(char *szurl);
int WBM_ADD_FROMURL(char *szinput, HWND hWnd);
int WP_GetUnReadCount(void);
int WP_GetTotalCount(void);
HBROWSER App_WapRequestUrlFromFile(char *szFilename);
DWORD WAP_RoomChecker(void);
BOOL WML_SetToDefault(void);

#endif		//WML_FUNC_H

