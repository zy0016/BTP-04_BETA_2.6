/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Default window proc for default dialog.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "string.h"

#include "defdlg.h"

typedef struct tagDLGDATA
{                   
    DLGPROC pDlgProc;           // Dialog proc function
    HWND    hwndFocusCtl;       // Current focus control
    HFONT   hUserFont;          // User font
    int     nResult;
    BYTE    bEnd;               // Is modal dialog end.
    BYTE    bModal;             // Is modal dialog    
} DLGDATA, *PDLGDATA;

// Internal function prototypes
static BOOL EraseBkgnd(HWND hWnd, HDC hdc);
static BOOL SaveDlgFocus(HWND hWnd, PDLGDATA pDlgData);
static BOOL RestoreDlgFocus(HWND hWnd, PDLGDATA pDlgData);
static void CheckDefPushButton(HWND hWnd, PDLGDATA pDlgData, 
                               HWND hwndOldFocus, HWND hwndNewFocus);
static HWND SetFocusToCtl(HWND hwndFocusCtl);
/*
**  Function : DEFDLG_RegisterClass
**  Purpose  :
**      Register default dialog class.
*/
BOOL DEFDLG_RegisterClass(void)
{
    WNDCLASS wc;

    wc.style            = CS_DEFWIN;
    wc.lpfnWndProc      = DEFDLG_Proc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = DLGWINDOWEXTRA;
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "DIALOG";

    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}

/*
**  Function : DEFDLG_Proc
**  Purpose  :
**      Carries out default message processing for a window procedure 
**      belonging to an application-defined dialog box class. 
*/
LRESULT CALLBACK DEFDLG_Proc(HWND hWnd, UINT message, WPARAM wParam, 
                             LPARAM lParam)
{
    LRESULT lResult;
    PDLGDATA pDlgData;
    HWND hwndCancel, hwndParent;
    HWND hwndOldFocus, hwndNewFocus;

    // Gets the internal data pointer of the specified dialog, if the 
    // result pointer is NULL, indicates that the handle of the dialog is
    // a invalid handle(maybe the window has been destroyed), just return.
    pDlgData = GetUserData(hWnd);
    if (!pDlgData)
        return 0;

    lResult = (LRESULT)0;

    // Call the dialog proc if it exists, if the dialog proc return TRUE, 
    // return the result
    if (pDlgData->pDlgProc)
    {
        lResult = pDlgData->pDlgProc(hWnd, message, wParam, lParam);
        if (message == WM_INITDIALOG || lResult)
            return lResult;
    }

    switch (message)
    {
    case WM_CREATE :

        memset(pDlgData, 0, sizeof(DLGDATA));

        pDlgData->pDlgProc = ((PDLGCREATEPARAM)
            (((PCREATESTRUCT)lParam)->lpCreateParams))->pDlgProc;
        pDlgData->bModal = (BYTE)((PDLGCREATEPARAM)
            (((PCREATESTRUCT)lParam)->lpCreateParams))->bModal;

        return TRUE;

    case WM_ERASEBKGND :

        return (LRESULT)EraseBkgnd(hWnd, (HDC)wParam);

    case WM_SHOWWINDOW :

        // If hiding the window, save the focus. If showing the window
        // by means of a SW_* command and the fEnd bit is set, do not
        // pass to DWP so it won't get shown.

        if (!wParam)
            SaveDlgFocus(hWnd, pDlgData);
        else if (lParam && pDlgData->bEnd)
            break;

        return DefWindowProc(hWnd, message, wParam, lParam);

    case WM_SYSCOMMAND :

        // If we're minimizing and a dialog control has the focus,
        // save the hWnd for that control

        if ((int)wParam == SC_MINIMIZE)
            SaveDlgFocus(hWnd, pDlgData);

        return DefWindowProc(hWnd, message, wParam, lParam);

    case WM_ACTIVATE :
    {
        int fActive, fMinimized;

        fActive = LOWORD(wParam);       // activation flag 
        fMinimized = HIWORD(wParam);    // minimized flag 

        if (fActive != WA_INACTIVE && !fMinimized)
        {
            if (!RestoreDlgFocus(hWnd, pDlgData))
                SetFocusToCtl(hWnd);
        }
        else
            SaveDlgFocus(hWnd, pDlgData);

        break;
    }
        
    case WM_SETFOCUS :

        if (!pDlgData->bEnd && !RestoreDlgFocus(hWnd, pDlgData))
            SetFocusToCtl(GetNextDlgTabItem(hWnd, NULL, FALSE));

        break;
        
    case WM_CLOSE :

        if (pDlgData->bModal)
        {
            // Make sure cancel button is not disabled before sending the
            // IDCANCEL. Note that we need to do this as a message instead
            // of directly calling the dlg proc so that any dialog box
            // filters get this.
            
            hwndCancel = GetDlgItem(hWnd, IDCANCEL);
            if (hwndCancel && !IsWindowEnabled(hwndCancel))
                MessageBeep(0);
            else
            {
                PostMessage(hWnd, WM_COMMAND, 
                    MAKEWPARAM(IDCANCEL, BN_CLICKED), (LPARAM)hwndCancel);
            }
        }
        else    // If is modaless dialog, destroys the dialog.
            DestroyWindow(hWnd);;

        break;

    case WM_NCDESTROY :

        // Make sure we are going to terminate the mode loop, in case
        // DestroyWindow was called instead of EndDialog.

        pDlgData->bEnd = TRUE;
        
        // Delete the user defined font if any
        if (pDlgData->hUserFont)
        {
            DeleteObject((HGDIOBJ)pDlgData->hUserFont);
            pDlgData->hUserFont = NULL;
        }
        
        // Always let DefWindowProc do its thing to ensure that everything
        // associated with the window is freed up.
        DefWindowProc(hWnd, message, wParam, lParam);

        break;

    case WM_COMMAND :

        if (pDlgData->bModal)
        {
            // If the WM_COMMAND message is caused by the OK or CANCEL 
            // button clicked, set the end flag of the dialog.
            if (HIWORD(wParam) == BN_CLICKED && 
                (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL))
            {
                pDlgData->bEnd = TRUE;
                pDlgData->nResult = LOWORD(wParam);
            }
        }
        
        break;
    
    case DM_SETDEFID :

        if (!pDlgData->bEnd)
        {
            // Make sure that the new default button has the highlight.
            // We need to ignore this if we are ending the dialog box
            // because hwnd->result is no longer a default window id but
            // rather the return value of the dialog box.
            //
            // Catch the case of setting the defid to null or setting
            // the defid to something else when it was initially null.

            CheckDefPushButton(hWnd, pDlgData, (pDlgData->nResult ? 
                GetDlgItem(hWnd, pDlgData->nResult) : NULL),
                (wParam ? GetDlgItem(hWnd, (int)wParam) : NULL));
            pDlgData->nResult = (int)wParam;
        }

        return (LRESULT)TRUE;
        
    case DM_GETDEFID :

        if (!pDlgData->bEnd && pDlgData->nResult)
            return (MAKELRESULT(pDlgData->nResult, DC_HASDEFID));
        else
            return (0L);
        
    case WM_NEXTDLGCTL :

        // wCtlFocus = wParam, If the fHandle parameter is TRUE, the 
        // wCtlFocus parameter identifies the control that receives the 
        // focus. If fHandle is FALSE, wCtlFocus is a flag that indicates 
        // whether the next or previous control with the WS_TABSTOP style
        // receives the focus. If wCtlFocus is zero, the next control 
        // receives the focus; otherwise, the previous control with the 
        // WS_TABSTOP style receives the focus. 

        // fHandle = (BOOL)LOWORD(lParam); Contains a flag that indicates 
        // how Windows uses the wCtlFocus parameter. If the fHandle 
        // parameter is TRUE, wCtlFocus is a handle associated with the 
        // control that receives the focus; otherwise, wCtlFocus is a flag
        // that indicates whether the next or previous control with the 
        // WS_TABSTOP style receives the focus. 

        // This message is so TAB-like operations can be properly handled
        // (simple SetFocusToCtl won't do the default button stuff)

        hwndOldFocus = GetFocus();
        if (LOWORD(lParam))         // fHandle is TRUE
        {
            if (!hwndOldFocus)      // No focus window
                hwndOldFocus = hWnd;
            
            // wParam contains the hwnd of the ctl to set focus to
            hwndNewFocus = (HWND)wParam;
        }
        else                        // fHandle is FALSE
        {
            if (!hwndOldFocus)      // No old focus window
            {
                // Set focus to the first tab item.
                hwndNewFocus = GetNextDlgTabItem(hWnd, NULL, FALSE);
                hwndOldFocus = hWnd;
            }
            else
            {
                // If window with focus not a dlg ctl, ignore message.
                if (!IsChild(hWnd, hwndOldFocus))
                    return (LRESULT)TRUE;

                hwndParent = GetParent(hwndOldFocus);
                if (hwndParent != hWnd)
                {
                    // wParam = TRUE for previous, FALSE for next
                    hwndNewFocus = GetNextDlgTabItem(hWnd, hwndParent, 
                        (BOOL)wParam);
                }
                else
                {
                    // wParam = TRUE for previous, FALSE for next
                    hwndNewFocus = GetNextDlgTabItem(hWnd, hwndOldFocus, 
                        (BOOL)wParam);
                }
            }
        }

        SetFocusToCtl(hwndNewFocus);
        CheckDefPushButton(hWnd, pDlgData, hwndOldFocus, hwndNewFocus);

        return (LRESULT)TRUE;
    
    case WM_GETFONT :

        return (LRESULT)pDlgData->hUserFont;

    // The following WM_CTLCOLOR... message 
    // Set up the supplied DC with the foreground and background
    // colors we want to use in the control, and return a brush
    // to use for filling.

    case WM_CTLCOLORDLG :
    case WM_CTLCOLORSTATIC :

#if (INTERFACE_MONO)
        SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
        SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
        return (LRESULT)(COLOR_WINDOW + 1);
#else // INTERFACE_MONO
        if (GetDeviceCaps(NULL, BITSPIXEL) == 1)
        {
            // For mono display
            // Background = COLOR_WINDOW
            // Foreground = COLOR_WINDOWTEXT
            // Brush = COLOR_WINDOW brush
            
            SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
            SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
            
            return (LRESULT)(COLOR_WINDOW + 1);
        }

        // For no-mono display 
        // Background = RGB(192, 192, 192)
        // Foreground = COLOR_WINDOWTEXT
        // Brush = LTGRAY_BRUSH

        SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor((HDC)wParam, RGB(192, 192, 192));
        return (LRESULT)GetStockObject(LTGRAY_BRUSH);
#endif // INTERFACE_MONO
        
    case WM_INITDIALOG :

        // Return TRUE to direct Windows to set the keyboard focus to the 
        // control given by hwndFocus = (HWND)wParam. 
        return TRUE;
    
    default :

        lResult = DefWindowProc(hWnd, message, wParam, lParam);
    }

    return lResult;
}

/*
**  Function : DEFDLG_IsDialogMessage
**  Purpose  :
**      Determines whether a message is intended for the specified dialog
**      box and, if it is, processes the message. 
**  Params   :
**      hDlg : Indentifies the dialog box.
**      pMsg : Points to a MSG struct that contains the message to be 
**             checked.
**  Returns  :
**      If the message has been processed, return nonzero. 
**      If the message has not been processed, return zero. 
*/
BOOL DEFDLG_IsDialogMessage(HWND hDlg, PMSG pMsg)
{
    int nDlgCode;
    HWND hwndNewFocus, hwndFocus;
    HWND hwndParent, hwndDef;
    int nDefID;
    PDLGDATA pDlgData;

    if (!IsWindow(hDlg) || !pMsg)
    {
        SetLastError(1);
        return FALSE;
    }

    // If the message target window isn't the dialog window or descendant
    // window of the dialog window, return FALSE.
    if (hDlg != pMsg->hwnd && !IsChild(hDlg, pMsg->hwnd))
        return FALSE;

    // If the message is WM_LBUTTONDOWN and the target window is a child
    // of the dialog, the child will get focus when it receives the mouse
    // message, call CheckDefPushButton to change the default button stuff
    if (pMsg->hwnd != hDlg && pMsg->message == WM_LBUTTONDOWN)
    {
        nDlgCode = SendMessage(pMsg->hwnd, WM_GETDLGCODE, 0, 0);
        if (nDlgCode != DLGC_STATIC && IsWindowEnabled(pMsg->hwnd))
        {
            hwndFocus = GetFocus();
            if (hwndFocus != pMsg->hwnd)
            {
                pDlgData = (PDLGDATA)GetUserData(hDlg);
                CheckDefPushButton(hDlg, pDlgData, hwndFocus, pMsg->hwnd);
            }
        }
    }

    // If the message isn't WM_KEYDOWN or WM_KEYUP message, return FALSE
    if (pMsg->message != WM_KEYDOWN && pMsg->message != WM_KEYUP && 
        pMsg->message != WM_CHAR)
        return FALSE;

    // Sends WM_GETDLGCODE message to the message target control
    if (hDlg != pMsg->hwnd)
        nDlgCode = SendMessage(pMsg->hwnd, WM_GETDLGCODE, 0, 0);
    else
        nDlgCode = 0;

    if (pMsg->message == WM_KEYDOWN)
    {
        // Processes enter key
        if (pMsg->wParam == VK_RETURN && !(pMsg->lParam & CK_CONTROL))
        {
            if (nDlgCode & DLGC_WANTRETURN)
                return FALSE;

            // Sends a WM_COMMAND message to the dialog box procedure. The
            // wParam parameter is set to IDOK or control identifier of 
            // the default push button

            hwndDef = GetFocus();
            if (hwndDef && IsChild(hDlg, hwndDef) && SendMessage(hwndDef, 
                WM_GETDLGCODE, 0, 0) == DLGC_DEFPUSHBUTTON)
                nDefID = (int)GetWindowLong(hwndDef, GWL_ID);
            else
            {
                nDefID = (int)SendMessage(hDlg, DM_GETDEFID, 0, 0);
                if (nDefID != 0)
                {
                    nDefID = LOWORD(nDefID);
                    hwndDef = GetDlgItem(hDlg, nDefID);
                }
            }

            // Make sure default push button is not disabled before sending
            // the nDefID. Note that we need to do this as a message 
            // instead of directly calling the dlg proc so that any dialog
            // box filters get this.
            if (hwndDef && !IsWindowEnabled(hwndDef))
                MessageBeep(0);
            else
            {
                PostMessage(hDlg, WM_COMMAND, 
                    MAKEWPARAM(nDefID, BN_CLICKED), (LPARAM)hwndDef);
            }

            return TRUE;
        }

        // Processes other keys

        if (nDlgCode & DLGC_WANTALLKEYS)
            return FALSE;

        switch (pMsg->wParam)
        {
        case VK_UP :
        case VK_LEFT :

            if (nDlgCode & DLGC_WANTARROWS)
                return FALSE;

            // Moves the input focus to the previous control in the group

            hwndFocus = GetFocus();
            if (!hwndFocus)      // No old focus window
            {
                // Set focus to the first group item.
                hwndNewFocus = GetNextDlgGroupItem(hDlg, NULL, TRUE);
                hwndFocus = hDlg;
            }
            else
            {
                // If window with focus not a dlg ctl, ignore message.
                if (!IsChild(hDlg, hwndFocus))
                    return (LRESULT)TRUE;

                hwndParent = GetParent(hwndFocus);
                if (hwndParent != hDlg)
                    hwndFocus = hwndParent;

                hwndNewFocus = GetNextDlgGroupItem(hDlg, hwndFocus, 
                    TRUE);
            }

            SetFocusToCtl(hwndNewFocus);

            pDlgData = (PDLGDATA)GetUserData(hDlg);
            CheckDefPushButton(hDlg, pDlgData, hwndFocus, hwndNewFocus);

            break;

        case VK_DOWN :
        case VK_RIGHT :
            
            if (nDlgCode & DLGC_WANTARROWS)
                return FALSE;

            // Moves the input focus to the next control in the group

            hwndFocus = GetFocus();
            if (!hwndFocus)      // No old focus window
            {
                // Set focus to the first tab item.
                hwndNewFocus = GetNextDlgGroupItem(hDlg, NULL, FALSE);
                hwndFocus = hDlg;
            }
            else
            {
                // If window with focus not a dlg ctl, ignore message.
                if (!IsChild(hDlg, hwndFocus))
                    return (LRESULT)TRUE;

                hwndParent = GetParent(hwndFocus);
                if (hwndParent != hDlg)
                    hwndFocus = hwndParent;

                // wParam = TRUE for previous, FALSE for next
                hwndNewFocus = GetNextDlgGroupItem(hDlg, hwndFocus, 
                    FALSE);
            }

            SetFocusToCtl(hwndNewFocus);

            pDlgData = (PDLGDATA)GetUserData(hDlg);
            CheckDefPushButton(hDlg, pDlgData, hwndFocus, hwndNewFocus);

            break;
            
        case VK_TAB :
            
            if (nDlgCode & DLGC_WANTTAB)
                return FALSE;

            if (pMsg->lParam & CK_SHIFT)
            {
                // SHIFT + TAB : moves the input focus to the previous
                // control that has the WS_TABSTOP style.
                SendMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
            }
            else
            {
                // TAB : moves the input focus to the next control that
                // has the WS_TABSTOP style.
                SendMessage(hDlg, WM_NEXTDLGCTL, 0, FALSE);
            }

            break;

        case VK_ESCAPE :

            // Sends a WM_COMMAND message to the dialog box procedure. The
            // wParam parameter is set to IDCANCEL.
            PostMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED),
                (LPARAM)GetDlgItem(hDlg, IDCANCEL));

            break;
            
        default :
            
            return FALSE;
        }
    }
    else if (pMsg->message == WM_KEYUP)
    {
        // Processes enter key
        if (pMsg->wParam == VK_RETURN && !(pMsg->lParam & CK_CONTROL))
        {
            if (nDlgCode & DLGC_WANTRETURN)
                return FALSE;

            return TRUE;
        }

        // Processes other keys

        if (nDlgCode & DLGC_WANTALLKEYS)
            return FALSE;

        switch (pMsg->wParam)
        {
        case VK_UP :
        case VK_DOWN :
        case VK_LEFT :
        case VK_RIGHT :
            
            if (nDlgCode & DLGC_WANTARROWS)
                return FALSE;

            break;
            
        case VK_TAB :
            
            if (nDlgCode & DLGC_WANTTAB)
                return FALSE;

            break;

        case VK_ESCAPE :

            break;
            
        default :
            
            return FALSE;
        }
    }
    else   // pMsg->message == WM_CHAR
    {
        // Processes enter key
        if (pMsg->wParam == '\r' || pMsg->wParam == '\n')
        {
            if (nDlgCode & DLGC_WANTRETURN)
                return FALSE;

            return TRUE;
        }

        if ((nDlgCode & DLGC_WANTALLKEYS) || (nDlgCode & DLGC_WANTCHARS))
            return FALSE;

        switch (pMsg->wParam)
        {
        case VK_UP :
        case VK_DOWN :
        case VK_LEFT :
        case VK_RIGHT :
        case VK_TAB :
        case VK_ESCAPE :

            break;
            
        default :
            
            return FALSE;
        }
    }

    return TRUE;
}

/*
**  Function : DEFDLG_EndDialog
**  Purpose  :
**      Sets the end dialog flag and hides the dialog. The system checks 
**      the flag before attempting to retrieve the next message from the 
**      application queue. If the flag is set, the system ends the message
**      loop, destroys the dialog box, and uses the value in nResult as 
**      the return value from the function that created the dialog box. 
*/
BOOL DEFDLG_EndDialog(HWND hDlg, int nResult)
{
    PDLGDATA pDlgData;

    // Gets the internal data pointer of the specified dialog, if the 
    // result pointer is NULL, indicats that the window handle is a 
    // invalid handle(maybe the window has been destroyed), it is a 
    // unreached state, so use ASSERT statement to ensure it not happen.

    pDlgData = GetUserData(hDlg);
    ASSERT(pDlgData != NULL);

    pDlgData->bEnd = TRUE;
    pDlgData->nResult = nResult;

    return TRUE;
}

/*
**  Function : DEFDLG_IsDialogEnd
*/
BOOL DEFDLG_IsDialogEnd(HWND hDlg, int* pnResult)
{
    PDLGDATA pDlgData;

    ASSERT(pnResult != NULL);

    // Gets the internal data pointer of the specified dialog, if the 
    // result pointer is NULL, indicats that the window handle is a 
    // invalid handle(maybe the window has been destroyed), it is a 
    // unreached state, so use ASSERT statement to ensure it happen.

    pDlgData = GetUserData(hDlg);
    ASSERT(pDlgData != NULL);

    *pnResult = pDlgData->nResult;

    return pDlgData->bEnd;
}

/*
**  Function : DEFDLG_SetFocusCtl
*/
BOOL DEFDLG_SetFocusCtl(HWND hDlg, HWND hwndFocusCtl)
{
    PDLGDATA pDlgData;

    // Gets the internal data pointer of the specified dialog, if the 
    // result pointer is NULL, indicats that the window handle is a 
    // invalid handle(maybe the window has been destroyed), it is a 
    // unreached state, so use ASSERT statement to ensure it happen.

    pDlgData = GetUserData(hDlg);
    ASSERT(pDlgData != NULL);

    pDlgData->hwndFocusCtl = hwndFocusCtl;

    return TRUE;
}

// Internal functions

/*
**  Function : EraseBkgnd
**  Purpose  :
**      Response the WM_ERASEBKGND message to paint the dialog background.
*/
static BOOL EraseBkgnd(HWND hWnd, HDC hdc)
{
    RECT rcClient;
    HBRUSH hbrBackground;

    // Sends the WM_CTLCOLORDLG message to a dialog box. By responding to 
    // this message, the dialog box can set its text and background colors 
    // by using the given display device context handle. 
    hbrBackground = (HBRUSH)SendMessage(hWnd, WM_CTLCOLORDLG, (WPARAM)hdc, 
        (LPARAM)hWnd);

    // The dialog box proc doesn't specified the background brush, use
    // the default window color to clear the background.
    if (!hbrBackground)
         hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    GetClientRect(hWnd, &rcClient);
    DPtoLP(hdc, (PPOINT)&rcClient, 2);
    FillRect(hdc, &rcClient, hbrBackground);

    return TRUE;
}

/*
**  Function : SaveDlgFocus
**  Purpose  :
**      Saves the current focus control to dialog data.
*/
static BOOL SaveDlgFocus(HWND hWnd, PDLGDATA pDlgData)
{
    HWND hwndFocus;
    DWORD dwStyle;

    hwndFocus = GetFocus();
    if (hwndFocus && IsChild(hWnd, hwndFocus) && !pDlgData->hwndFocusCtl)
    {
        pDlgData->hwndFocusCtl = hwndFocus;

        // Remove the default button state if the focus control is push 
        // button
        dwStyle = GetWindowLong(hwndFocus, GWL_STYLE);
        SendMessage(hwndFocus, BM_SETSTYLE, LOWORD(dwStyle), 
            MAKELPARAM(TRUE, 0));

        return TRUE;
    }

    return FALSE;
}

/*
**  Function : RestoreDlgFocus
**  Purpose  :
**      Restores the saved focus control in dialog data.
*/
static BOOL RestoreDlgFocus(HWND hWnd, PDLGDATA pDlgData)
{
    HWND hwndFocus, hwndDef;
    BOOL bRestored = FALSE;
    DWORD dwStyle;
    int nDlgCode;

    // 对话框的DEFID没有设置, 说明此函数是在对话框生成时被调用, 按照控
    // 件生成的顺序查找具有BS_DEFPUSHBUTTON风格的BUTTON, 若找不到, 则
    // 使用IDOK替代
    if (pDlgData->nResult == 0)
    {
        hwndDef = GetWindow(hWnd, GW_LASTCHILD);
        while (hwndDef)
        {
            nDlgCode = SendMessage(hwndDef, WM_GETDLGCODE, 0, 0);
            if (nDlgCode == DLGC_DEFPUSHBUTTON)
                break;
            
            hwndDef = GetWindow(hwndDef, GW_HWNDPREV);
        }
        
        if (hwndDef)
            pDlgData->nResult = (int)GetWindowLong(hwndDef, GWL_ID);
        else
        {
            pDlgData->nResult = IDOK;
            
            // 如果ID为IDOK的控件时PushButton, 强制置为DefPushButton
            hwndDef = GetDlgItem(hWnd, IDOK);
            if (hwndDef)
            {
                nDlgCode = SendMessage(hwndDef, WM_GETDLGCODE, 0, 0);
                if (nDlgCode == DLGC_UNDEFPUSHBUTTON)
                {
                    SendMessage(hwndDef, BM_SETSTYLE, BS_DEFPUSHBUTTON,
                        TRUE);
                }
            }
        }
    }

    hwndFocus = GetFocus();
    dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);

    if (pDlgData->hwndFocusCtl && !(dwStyle & WS_MINIMIZE))
    {
        if (IsWindow(pDlgData->hwndFocusCtl))
        {
            CheckDefPushButton(hWnd, pDlgData, hwndFocus, 
                pDlgData->hwndFocusCtl);
            SetFocusToCtl(pDlgData->hwndFocusCtl);
            bRestored = TRUE;
        }

        pDlgData->hwndFocusCtl = NULL;
    }

    return bRestored;
}

/*
**  Function : CheckDefPushButton
**  Purpose  :
**      当键盘输入焦点由一个对话框控件转移到另一个控件时, 进行对话框默认
**      PUSH BUTTON的转换.
*/
static void CheckDefPushButton(HWND hWnd, PDLGDATA pDlgData, 
                               HWND hwndOldFocus, HWND hwndNewFocus)
{
    HWND hwndDef;
    int nDlgCode;

    if (!hwndNewFocus)
        return;

    // 旧的焦点窗口不是对话框的子窗口, 与没有旧的焦点窗口同样对待
    if (hwndOldFocus && !IsChild(hWnd, hwndOldFocus))
        hwndOldFocus = NULL;

    // 通过发送WM_GETDLGCODE消息判断获得焦点的窗口是不是push button
    nDlgCode = SendMessage(hwndNewFocus, WM_GETDLGCODE, 0, 0);

    // 获得焦点的窗口是push button, 设置为default push button, 并清除原来
    // default push button的default风格
    if (nDlgCode == DLGC_UNDEFPUSHBUTTON || nDlgCode == DLGC_DEFPUSHBUTTON)
    {
        // 通过发送WM_GETDLGCODE消息判断失去焦点的窗口是不是default push 
        // button. 若不是, 取对话框的默认ID对应的控件为default push button
        if (hwndOldFocus && SendMessage(hwndOldFocus, WM_GETDLGCODE, 
            0, 0) == DLGC_DEFPUSHBUTTON)
            hwndDef = hwndOldFocus;
        else
        {
            hwndDef = GetDlgItem(hWnd, pDlgData->nResult);
            if (!hwndDef || SendMessage(hwndDef, WM_GETDLGCODE, 
                0, 0) != DLGC_DEFPUSHBUTTON)
                hwndDef = NULL;
        }

        // 清除原来的default push button的BS_DEFPUSHBUTTON风格
        if (hwndDef && hwndDef != hwndNewFocus)
        {
            SendMessage(hwndDef, BM_SETSTYLE, BS_PUSHBUTTON, 
                MAKELPARAM(TRUE, 0));
        }

        // 设置获得焦点的push button为default push button.
        if (nDlgCode == DLGC_UNDEFPUSHBUTTON)
        {
            SendMessage(hwndNewFocus, BM_SETSTYLE, BS_DEFPUSHBUTTON, 
                MAKELPARAM(TRUE, 0));
        }

        return;
    }
    
    // 此时获得焦点的控件不是push button

    if (hwndOldFocus)
    {
        // 判断失去焦点的控件是不是default push button, 若是, 清除该控件的
        // default风格, 并将对话框的默认ID对应的push button的风格设置为
        // default
        nDlgCode = SendMessage(hwndOldFocus, WM_GETDLGCODE, 0, 0);
        if (nDlgCode == DLGC_DEFPUSHBUTTON)
        {
            hwndDef = GetDlgItem(hWnd, pDlgData->nResult);
            if (hwndOldFocus != hwndDef)
            {
                // 清除失去焦点控件的default风格
                SendMessage(hwndOldFocus, BM_SETSTYLE, BS_PUSHBUTTON, 
                    MAKELPARAM(TRUE, 0));

                // 将对话框的默认ID对应的push button的风格设置为default
                if (hwndDef && SendMessage(hwndDef, WM_GETDLGCODE, 
                    0, 0) == DLGC_UNDEFPUSHBUTTON)
                {
                    SendMessage(hwndDef, BM_SETSTYLE, BS_DEFPUSHBUTTON, 
                        MAKELPARAM(TRUE, 0));
                }
            }
        }
    }

    // 此时获得焦点的控件不是push button, 失去焦点的控件也不是push button, 
    // 不需要做任何事.
}

/*
**  Function : SetFocusToCtl
**  Purpose  :
**      Sets focus to the specified control, if the specified control is 
**      a edit has ES_NOHIDESEL style, select all the texts of the edit.
*/
static HWND SetFocusToCtl(HWND hwndFocusCtl)
{
    char achClassName[16];

    GetClassName(hwndFocusCtl, achClassName, 15);
    achClassName[15] = 0;
    if (!stricmp(achClassName, "EDIT"))
        SendMessage(hwndFocusCtl, EM_SETSEL, 0, -1);
    else if (!stricmp(achClassName, "COMBOBOX"))
        SendMessage(hwndFocusCtl, CB_SETEDITSEL, 0, MAKELPARAM(0, -1));

    return SetFocus(hwndFocusCtl);
}
