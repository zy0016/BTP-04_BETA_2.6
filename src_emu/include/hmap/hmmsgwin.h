/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#ifndef _HMMSGWIN_H_

#define _HMMSGWIN_H_

// The Max Length of the Display Info
#define INFO_MAX_LEN 60

// Error Code
#define MsgWin_E_INFO_TOO_LONG -101 // The length of the info is too long
#define MsgWin_E_NO_INFO       -102 // the content of the info is NULL
#define MsgWin_E_MEM_LOW       -103 // failure of the memory allocation

HWND DisplayWarningMessage (HWND hWnd, LPCTSTR pText, int timeout, UINT wMsg,
                            LPCTSTR pLeftBtn, LPCTSTR pRightBtn );
HWND DisplayTimeInputWin (HWND hWnd, UINT wMsg);

#endif // _HMMSGWIN_H_