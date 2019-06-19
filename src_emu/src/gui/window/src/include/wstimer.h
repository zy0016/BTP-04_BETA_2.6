/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Function prototypes for timer fucntion
 *            
\**************************************************************************/

#ifndef __WSTIMER_H
#define __WSTIMER_H

UINT WINAPI SetSystemTimer(HWND hWnd, UINT uIDEvent, DWORD uElapse, 
                           TIMERPROC pTimerFunc);
BOOL WINAPI KillSystemTimer(HWND hWnd, UINT uIDEvent);

DWORD WS_TimerTask(void);
void WS_KillWindowTimer(PWINOBJ pWin);

UINT WS_CreateTimer(PWINOBJ pWin, UINT uIDEvent, DWORD uElapse, 
                        TIMERPROC pTimerFunc, BOOL bSystem);
BOOL WS_DestroyTimer(PWINOBJ pWin, UINT uIDEvent, BOOL bSystem);

#endif //__WSTIMER_H
