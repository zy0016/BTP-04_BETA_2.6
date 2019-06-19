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

#ifndef __DIALMGR_H
#define __DIALMGR_H

#include "dialer.h"

int DIALMGR_Initialize(void);
int DIALMGR_SelConnect(HWND hWnd, UINT msg, int nSelectType, void *pDialInfo, 
                       BOOL bWait);
int DIALMGR_HangUp(HWND hWnd);

#define DIALMGR_GetLineState    DIALER_GetLineState
#define DIALMGR_GetError        Dialer_GetError

#endif  // __DIALMGR_H
