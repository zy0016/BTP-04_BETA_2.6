/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for device input queue management module.
 *            
\**************************************************************************/

#ifndef __DEVQUEUE_H
#define __DEVQUEUE_H

BOOL DEVQ_GetInputEvent(PTHREADINFO pThreadInfo, PDEVEVENT pDevEvent, BOOL bRemove);
BOOL DEVQ_PutKeyboardEvent(PTHREADINFO pThreadInfo, BYTE byVirtKey, BYTE byScan, DWORD dwFlag, 
                           DWORD dwExtraInfo);
BOOL DEVQ_PutMouseEvent(PTHREADINFO pThreadInfo, DWORD dwFlags, LONG dx, LONG dy, DWORD dwData,
                        DWORD dwExtraInfo, HWND hWnd);
void DEVQ_RemoveInputEvents(PTHREADINFO pThreadInfo);

#endif  // __DEVQUEUE_H
