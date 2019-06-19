/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : playamr.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _PLAYAMR_H_
#define _PLAYAMR_H_

int  AMRDecodeBegin(HWND hWnd, LPSTR pstrName, unsigned char* pDataBuf, int nDataLen, int nPriority);
int  AMRDecodeEnd(void);

void AMRReadData(WPARAM wParam, LPARAM lParam);
void AMRWriteData(WPARAM wParam, LPARAM lParam);

#endif // _PLAYAMR_H_
