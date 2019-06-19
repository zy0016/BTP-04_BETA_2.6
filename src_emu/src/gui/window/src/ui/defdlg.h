/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Head file for default dialog management.
 *            
\**************************************************************************/

#ifndef __DEFDLG_H
#define __DEFDLG_H

// WM_CREATE消息中传入的lpCreateParams指定的数据的结构
typedef struct tagDLGCREATEPARAM
{
    DLGPROC pDlgProc;
    BOOL    bModal;
} DLGCREATEPARAM, *PDLGCREATEPARAM;

LRESULT DEFDLG_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
BOOL    DEFDLG_IsDialogMessage(HWND hDlg, PMSG pMsg);
BOOL    DEFDLG_EndDialog(HWND hDlg, int nResult);
BOOL    DEFDLG_IsDialogEnd(HWND hDlg, int* pnResult);
BOOL    DEFDLG_SetFocusCtl(HWND hDlg, HWND hwndFocusCtl);

#endif /* __DEFDLG_H */
