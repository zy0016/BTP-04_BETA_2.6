/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Cursor management function prototypes.
 *            
\**************************************************************************/

#ifndef __WSCURSOR_H
#define __WSCURSOR_H

#include "wsocur.h"

PCURSOROBJ WS_SetCursor(PCURSOROBJ pCursor);
PCURSOROBJ WS_GetCursor(void);
int  WS_ShowCursor(BOOL bShow);
void WS_SetCursorPos(int x, int y);
void WS_GetCursorPos(PPOINT pPoint);
void WS_SetMaxCursorRect(const RECT* pRect);
void WS_ClipCursor(const RECT* pRect);
void WS_GetClipCursor(PRECT pRect);
BOOL WS_PtInCursorClipRect(POINT pt);
void WS_ClipCursorPoint(PPOINT pPoint);

#endif //__WSCURSOR_H
