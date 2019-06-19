/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Head file for input device process.
 *            
\**************************************************************************/

#ifndef __DEVINPUT_H
#define __DEVINPUT_H

#include "wsownd.h"

// Inializes input device, called by main
BOOL    INPUTDEV_Init(void);

// Functions called by input request process function
PWINOBJ INPUTDEV_GetForegroundWindow(void);
BOOL INPUTDEV_SetForegroundWindow(PWINOBJ pWin);

PWINOBJ INPUTDEV_GetActiveWindow(void);
PWINOBJ INPUTDEV_SetActiveWindow(PWINOBJ pWin, int nActive, BOOL bUpdate);

PWINOBJ INPUTDEV_GetFocus(void);
PWINOBJ INPUTDEV_SetFocus(PWINOBJ pWin);

PWINOBJ INPUTDEV_GetCapture(void);
PWINOBJ INPUTDEV_SetCapture(PWINOBJ pWin, BOOL bClient);
BOOL    INPUTDEV_ReleaseCapture(void);

PWINOBJ INPUTDEV_GrabKeyboard(PWINOBJ pWin);
BOOL    INPUTDEV_UngrabKeyboard(void);
BOOL    INPUTDEV_TranslateMessage(const MSG* pMsg);

BOOL    INPUTDEV_SendChar(PWINOBJ pWin, WORD wCharCode);
BOOL    INPUTDEV_SendKeyDown(PWINOBJ pWin, WORD wVirtKey, WORD wKeyState);
BOOL    INPUTDEV_SendKeyUp(PWINOBJ pWin, WORD wVirtKey, WORD wKeyState);
BOOL    INPUTDEV_ResendMouseMessage(PWINOBJ pWin, UINT wMsgCmd, 
                                    WPARAM wParam, LPARAM lParam);

BOOL    INPUTDEV_GetInputMessage(PMSG pMsg);

// Functions to support system modal dialog
PWINOBJ INPUTDEV_SetSysModalWindow(PWINOBJ pWin);
PWINOBJ INPUTDEV_GetSysModalWindow(void);

// Functions to be used to calibrate pen device
void    INPUTDEV_SetPointDevScale(int nLogX1, int nLogY1, int nPhyX1, 
                                  int nPhyY1, int nLogX2, int nLogY2, 
                                  int nPhyX2, int nPhyY2);

// Function called by window destroy function
void    INPUTDEV_HideWindowNotify(PWINOBJ pWin);
void    INPUTDEV_DestroyWindowNotify(PWINOBJ pWin);

void    INPUTDEV_BeginPenCalibrate(PWINOBJ pWin);
void    INPUTDEV_EndPenCalibrate(void);

void    INPUTDEV_GetKeyboardState(PBYTE pKeyState);
int     INPUTDEV_ToAscii(UINT uVirtKey, UINT uScanCode, PBYTE pKeyState,
                         LPWORD pChar, UINT uFlags);

#endif
