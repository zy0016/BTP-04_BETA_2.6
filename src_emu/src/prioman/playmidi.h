/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : playmidi.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _PLAYMIDI_H_
#define _PLAYMIDI_H_

#ifdef _EMULATE_

BOOL AllocBuffers(HWND hWnd);
void FreeBuffers(void);
void AttemptFileOpen(HWND hWnd, LPSTR pstrName);
BOOL PrerollAndWait(HWND hWnd);

void Start(void);
void Stop(void);

#endif // _EMULATE_

#endif // _PLAYMIDI_H_
