/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : playmp3.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _PLAYMP3_H_
#define _PLAYMP3_H_

int  MP3DecodeBegin(HWND hWnd, LPSTR pstrName, int nPriority);
int  MP3DecodeEnd(void);

void MP3ReadData(WPARAM wParam, LPARAM lParam);
void MP3WriteData(WPARAM wParam, LPARAM lParam);
int  MP3Pause(void);
int  MP3Restart(void);
int  MP3GetTotalTime(void);
int  MP3CurrentTime(int* pSeconds);
int  MP3Seek(int nSeconds);
int  MP3SetVolume(int nVolume);

#endif // _PLAYMP3_H_
