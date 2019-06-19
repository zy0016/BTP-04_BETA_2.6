/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : playwave.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _PLAYWAVE_H_
#define _PLAYWAVE_H_

int  WaveDecodeBegin(HWND hWnd, LPSTR pstrName, unsigned char* pDataBuf, int nDataLen, int nPriority);
int  WaveDecodeEnd(void);

void WaveReadData(WPARAM wParam, LPARAM lParam);
void WaveWriteData(WPARAM wParam, LPARAM lParam);

#endif // _PLAYWAVE_H_
