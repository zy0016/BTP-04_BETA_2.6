/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Head file for default window management.
 *            
\**************************************************************************/

#ifndef __DEFWND_H
#define __DEFWND_H

BOOL DEFWND_RegisterClass(void);

int  DEFWND_SetScrollInfo(HWND hWnd, int nBar, PCSCROLLINFO pScrollInfo, 
                          BOOL bRedraw);
BOOL DEFWND_GetScrollInfo(HWND hWnd, int nBar, PSCROLLINFO pScrollInfo);
BOOL DEFWND_EnableScrollBar(HWND hWnd, int nBar, int nArrow);
int  DEFWND_SetScrollPos(HWND hWnd, int nBar, int nPos, BOOL bRedraw); 
int  DEFWND_GetScrollPos(HWND hWnd, int nBar); 
BOOL DEFWND_SetScrollRange(HWND hWnd, int nBar, int nMinPos, int nMaxPos, 
                           BOOL bRedraw); 
BOOL DEFWND_GetScrollRange(HWND hWnd, int nBar, LPINT lpMinPos, 
                           LPINT lpMaxPos); 
DWORD DEFWND_GetDataSize(HWND hWnd);

#if (!NOMENUS)
HWND DEFWND_GetMenuCtrl(HWND hWnd);
#endif // NOMENUS

LRESULT DEFWND_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
LRESULT DEFWND_ProcBase(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

void DEFWND_CalcMinSize(DWORD dwExStyle, DWORD dwStyle, HMENU hMenu, 
                        PSIZE pMinSize);

#endif /* __DEFWND_H */
