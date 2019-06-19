/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Caret support function prototypes.
 *            
\**************************************************************************/

#ifndef __WSCARET_H
#define __WSCARET_H

#include "wsownd.h"

void WS_CreateCaret(PWINOBJ pWin, PBMPOBJ pBmp, int nWidth, int nHeight);
void WS_DestroyCaret(void);

void WS_SetCaretPos(int x, int y);
void WS_GetCaretPos(PPOINT pPoint);

BOOL WS_HideCaret(PWINOBJ pWin);
BOOL WS_ShowCaret(PWINOBJ pWin);

UINT WS_GetCaretBlinkTime(void);
void WS_SetCaretBlinkTime(UINT uMSeconds);

PWINOBJ WS_GetCaretWindow(void);
void WS_ScrollCaret(PWINOBJ pWin, int dx, int dy);

void WS_DestroyWindowCaret(PWINOBJ pWin);

#endif //__WSCARET_H
