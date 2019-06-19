/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : 定义窗口系统提供的, 未包含在window.h中的宏、消息和函数, 这
 *            些消息和函数仅提供给内部使用, 对普通应用程序不公开.
 *            
\**************************************************************************/

#ifndef __WINDOWX_H
#define __WINDOWX_H

/* include window.h if necessary */
#ifndef __WINDOW_H
#include "window.h"
#endif

/* Internal system message */
#define WM_SYSTIMER     0x03F1
#define WM_SYSWAKEUP    0x03F0

/* Resend the latest mouse message */
BOOL WINAPI ResendMouseMessage(HWND hWnd, UINT message, WPARAM wParam, 
                               LPARAM lParam);

/* Captures the mouse for non-client area */
HWND WINAPI SetNCCapture(HWND hWnd);

/* Callback function when caret position changing */
typedef void (CALLBACK *CARETPROC)(const RECT*);
CARETPROC WINAPI SetCaretProc(CARETPROC pNewProc);

#if (__MP_PLX_GUI)
HBITMAP WINAPI CreateSharedBitmap(int nWidth, int nHeight, UINT cPlanes, 
                                  UINT cBitsPixel, const void* lpvBits);
HBRUSH WINAPI CreateSharedBrush(int nStyle, COLORREF color, LONG lHatch);
#else
#define CreateSharedBitmap CreateBitmap
#define CreateSharedBrush  CreateBrush
#endif

#endif //__WINDOWX_H
