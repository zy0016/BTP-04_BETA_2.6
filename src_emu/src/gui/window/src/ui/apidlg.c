/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements dialog and control management functions.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "defdlg.h"
#include "string.h"


static HWND CreateDialogWindowsIndirect(HINSTANCE hInstance, 
                                        PCDLGTEMPLATE pTemplate, 
                                        HWND hwndOwner, DLGPROC pDlgProc, 
                                        LPARAM dwInitParam, BOOL bModal);

#ifndef NOMSG

/*
**  Function : IsDialogMessage
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
**  Remarks  :
**      Although the IsDialogMessage function is intended for modeless 
**      dialog boxes, you can use it with any window that contains controls,
**      enabling the windows to provide the same keyboard selection as is 
**      used in a dialog box. 
**      When IsDialogMessage processes a message, it checks for keyboard 
**      messages and converts them into selection commands for the 
**      corresponding dialog box. For example, the TAB key, when pressed, 
**      selects the next control or group of controls, and the DOWN ARROW 
**      key, when pressed, selects the next control in a group. 
**      Because the IsDialogMessage function performs all necessary 
**      translating and dispatching of messages, a message processed by 
**      IsDialogMessage must not be passed to the TranslateMessage or 
**      DispatchMessage function. 
**      IsDialogMessage sends WM_GETDLGCODE messages to the dialog box 
**      procedure to determine which keys should be processed. 
**      IsDialogMessage can send DM_GETDEFID and DM_SETDEFID messages to 
**      the window. These messages are defined as WM_USER and WM_USER + 1, 
**      so conflicts are possible with application-defined messages having
**       the same values. 
*/
BOOL WINAPI IsDialogMessage(HWND hDlg, PMSG pMsg)
{
    return DEFDLG_IsDialogMessage(hDlg, pMsg);
}

/*
**  Function : IsInputMessage
**  Purpose  :
**      Determines whether a message is a input message, including
**      pen message and keyboard message.
*/
BOOL WINAPI IsInputMessage(UINT wMsgCmd)
{
    if (wMsgCmd == WM_PENDOWN || wMsgCmd == WM_PENUP     || 
        wMsgCmd == WM_PENMOVE || wMsgCmd == WM_NCPENDOWN || 
        wMsgCmd == WM_NCPENUP || wMsgCmd == WM_NCPENMOVE || 
        wMsgCmd == WM_KEYDOWN || wMsgCmd == WM_KEYUP     || 
        wMsgCmd == WM_CHAR)
        return TRUE;

    return FALSE;
}
 
#endif

/*
**  Function : DefDlgProc
**  Purpose  :
**      Carries out default message processing for a window procedure 
**      belonging to an application-defined dialog box class. 
**  Params   :
**      hDlg    : Indentifies the dialog box.
**      wMsgCmd : Specifies the message comand.
**      wParam  : Specifies additional message-specific information. 
**      lParam  : Specifies additional message-specific information. 
**  Return   :
**      Return the result of the message processing and depends on the 
**      message sent. 
**  Remarks  :
**      The DefDlgProc function is the window procedure for the predefined
**      class of dialog box. This procedure provides internal processing 
**      for the dialog box by forwarding messages to the dialog box 
**      procedure and carrying out default processing for any messages that
**      the dialog box procedure returns as FALSE. Applications that create
**      custom window procedures for their custom dialog boxes often use 
**      DefDlgProc instead of the DefWindowProc function to carry out 
**      default message processing. 
**      Applications create custom dialog box classes by filling a WNDCLASS
**      structure with appropriate information and registering the class 
**      with the RegisterClass function. Some applications fill the 
**      structure by using the GetClassInfo function, specifying the name 
**      of the predefined dialog box. In such cases, the applications 
**      modify at least the lpszClassName member before registering. In all
**      cases, the cbWndExtra member of WNDCLASS for a custom dialog box 
**      class must be set to at least DLGWINDOWEXTRA. 
**      The DefDlgProc function must not be called by a dialog box 
**      procedure; doing so results in recursive execution. 
*/
LRESULT WINAPI DefDlgProc(HWND hDlg, UINT wMsgCmd, WPARAM wParam, 
                          LPARAM lParam)
{
    // 这里暂时不判断hDlg的有效性, hDlg的有效性将在DEFDLG_Proc函数中判断

    return DEFDLG_Proc(hDlg, wMsgCmd, wParam, lParam);
}

/*
**  Function : CreateDialog
**  Purpose  :
**      Creates a modeless dialog box from a dialog box template resource.
**  Params   :
**      hInstance   : Identifies an instance of the module who contains the
**                    dialog box template. 
**      pszTemplate : Identifies the dialog box template. This parameter is
**                    either the pointer to a null-terminated character 
**                    string that specifies the name of the dialog box 
**                    template or an integer value that specifies the 
**                    resource identifier of the dialog box template. If 
**                    the parameter specifies a resource identifier, its 
**                    high-order word must be zero and its low-order word 
**                    must contain the identifier. You can use the 
**                    MAKEINTRESOURCE macro to create this value. 
**      hwndOwner   : Identifies the window that owns the dialog box. 
**      pDlgProc    : Points to the dialog box procedure. 
**  Return   :
**      If the function succeeds, return the handle to the dialog box. 
**      If the function fails, return NULL. 
**  Remarks  :
**      The CreateDialog function uses the CreateWindowEx function to 
**      create the dialog box. Then sends a WM_INITDIALOG message (and a 
**      WM_SETFONT message if the template specifies the DS_SETFONT style)
**      to the dialog box procedure. The function displays the dialog box 
**      if the template specifies the WS_VISIBLE style. Finally, 
**      CreateDialog returns the window handle to the dialog box. 
**      After CreateDialog returns, the application displays the dialog box
**      (if it is not already displayed) by using the ShowWindow function. 
**      The application destroys the dialog box by using the DestroyWindow
**      function. 
*/
HWND WINAPI CreateDialog(HINSTANCE hInstance, PCSTR pszTemplate, 
                         HWND hwndOwner, DLGPROC pDlgProc)
{
    return CreateDialogParam(hInstance, pszTemplate, hwndOwner, pDlgProc,
        0);
}

/*
**  Function : CreateDialogIndirect
**  Purpose  :
**      Creates a modeless dialog box from a dialog box template in memory.
**  Params   :
**      hInstance : Identifies an instance of the module who contains the
**                  dialog box template. 
**      pTemplate : Pointer to a global memory object containing a template
**                  that DialogBoxIndirect uses to create the dialog box. A
**                  dialog box template consists of a header that describes
**                  the dialog box, followed by one or more additional 
**                  blocks of data that describe each of the controls in 
**                  the dialog box. The template can use either the 
**                  standard format or the extended format. 
**      hwndOwner : Identifies the window that owns the dialog box. 
**      pDlgProc  : Points to the dialog box procedure. 
**  Return   :
**      If the function succeeds, return the handle to the dialog box. 
**      If the function fails, return NULL. 
*/
HWND WINAPI CreateDialogIndirect(HINSTANCE hInstance, 
                                 PCDLGTEMPLATE pTemplate, 
                                 HWND hwndOwner, DLGPROC pDlgProc)
{
    return CreateDialogIndirectParam(hInstance, pTemplate, hwndOwner, 
        pDlgProc, 0);
}

/*
**  Function : CreateDialogParam
**  Purpose  :
**      Creates a modeless dialog box from a dialog box template resource. 
**      Before displaying the dialog box, the function passes an 
**      application-defined value to the dialog box procedure as the lParam
**      parameter of the the WM_INITDIALOG message. An application can use 
**      this value to initialize dialog box controls. 
**  Params   :
**      hInstance   : Identifies an instance of the module who contains the
**                    dialog box template. 
**      pszTemplate : Identifies the dialog box template. This parameter is
**                    either the pointer to a null-terminated character 
**                    string that specifies the name of the dialog box 
**                    template or an integer value that specifies the 
**                    resource identifier of the dialog box template. If 
**                    the parameter specifies a resource identifier, its 
**                    high-order word must be zero and its low-order word 
**                    must contain the identifier. You can use the 
**                    MAKEINTRESOURCE macro to create this value. 
**      hwndOwner   : Identifies the window that owns the dialog box. 
**      pDlgProc    : Points to the dialog box procedure. 
**      dwInitParam : Specifies the value to pass to the dialog box in the 
**                    lParam parameter of the WM_INITDIALOG message. 
**  Return   :
**      If the function succeeds, return the handle to the dialog box. 
**      If the function fails, return NULL. 
*/
HWND WINAPI CreateDialogParam(HINSTANCE hInstance, PCSTR pszTemplate, 
                              HWND hwndOwner, DLGPROC pDlgProc, 
                              LPARAM dwInitParam)
{
    HRSRC hResInfo;
    HGLOBAL hResData;
    BYTE * pHMem;
    HWND  hDlg;

    hResInfo = FindResource(hInstance, pszTemplate, RT_DIALOG);
    if (hResInfo == NULL)
        return NULL;
    hResData = LoadResource(hInstance, hResInfo);
    pHMem = (BYTE *)LockResource(hResData);
    if (pHMem == NULL)
        return NULL;

    hDlg = CreateDialogWindowsIndirect(NULL, (PCDLGTEMPLATE)pHMem, 
        hwndOwner, pDlgProc, dwInitParam, FALSE);
    FreeResource(hResData);
    return hDlg;
}

/*
**  Function : CreateDialogIndirectParam
**  Purpose  :
**      Creates a modeless dialog box from a dialog box template in memory.
**      Before displaying the dialog box, the function passes an 
**      application-defined value to the dialog box procedure as the lParam
**      parameter of the WM_INITDIALOG message. An application can use this
**      value to initialize dialog box controls. 
**  Params   :
**      hInstance : Identifies an instance of the module who contains the
**                  dialog box template. 
**      pTemplate : Pointer to a global memory object containing a template
**                  that DialogBoxIndirect uses to create the dialog box. A
**                  dialog box template consists of a header that describes
**                  the dialog box, followed by one or more additional 
**                  blocks of data that describe each of the controls in 
**                  the dialog box. The template can use either the 
**                  standard format or the extended format. 
**      hwndOwner : Identifies the window that owns the dialog box. 
**      pDlgProc  : Points to the dialog box procedure. 
**      dwInitParam : Specifies the value to pass to the dialog box in the 
**                    lParam parameter of the WM_INITDIALOG message. 
**  Return   :
**      If the function succeeds, return the handle to the dialog box. 
**      If the function fails, return NULL. 
*/
HWND WINAPI CreateDialogIndirectParam(HINSTANCE hInstance, 
                                      PCDLGTEMPLATE pTemplate, 
                                      HWND hwndOwner, DLGPROC pDlgProc, 
                                      LPARAM dwInitParam)
{
    return CreateDialogWindowsIndirect(hInstance, pTemplate, hwndOwner, 
        pDlgProc, dwInitParam, FALSE);
}

/*
**  Function : DialogBox
**  Purpose  :
**      Creates a modal dialog box from a dialog box template resource. 
**      DialogBox does not return control until the specified callback 
**      function terminates the modal dialog box by calling the EndDialog
**      function.
**  Params   :
**      hInstance   : Identifies an instance of the module who contains the
**                    dialog box template. 
**      pszTemplate : Identifies the dialog box template. This parameter is
**                    either the pointer to a null-terminated character 
**                    string that specifies the name of the dialog box 
**                    template or an integer value that specifies the 
**                    resource identifier of the dialog box template. If 
**                    the parameter specifies a resource identifier, its 
**                    high-order word must be zero and its low-order word 
**                    must contain the identifier. You can use the 
**                    MAKEINTRESOURCE macro to create this value. 
**      hwndOwner   : Identifies the window that owns the dialog box. 
**      pDlgProc    : Points to the dialog box procedure. 
**  Return   :
**      If the function succeeds, return the nResult parameter in the call
**      to the EndDialog function used to terminate the dialog box. 
**      If the function fails, return -1. 
**  Remark   :
**      The DialogBox function uses the CreateWindowEx function to create 
**      the dialog box. DialogBox then sends a WM_INITDIALOG message (and
**      WM_SETFONT message if the template specifies the DS_SETFONT style)
**      to the dialog box procedure. The function displays the dialog box
**      (regardless of whether the template specifies the WS_VISIBLE 
**      style), disables the owner window, and starts its own message loop
**      to retrieve and dispatch messages for the dialog box. 
**      When the dialog box procedure calls the EndDialog function, 
**      DialogBox destroys the dialog box, ends the message loop, enables 
**      the owner window (if previously enabled), and returns the nResult 
**      parameter specified by the dialog box procedure when it called 
**      EndDialog. 
*/
int WINAPI DialogBox(HINSTANCE hInstance, PCSTR pszTemplate, 
                     HWND hwndOwner, DLGPROC pDlgProc)
{
    return DialogBoxParam(hInstance, pszTemplate, hwndOwner, pDlgProc, 
        0);
}

/*
**  Function : DialogBoxIndirect
**  Purpose  :
**      Creates a modal dialog box from a dialog box template in memory. 
**      DialogBoxIndirect does not return control until the specified 
**      callback function terminates the modal dialog box by calling the
**      EndDialog function. 
**  Params   :
**      hInstance : Identifies an instance of the module who contains the
**                  dialog box template. 
**      pTemplate : Pointer to a global memory object containing a template
**                  that DialogBoxIndirect uses to create the dialog box. A
**                  dialog box template consists of a header that describes
**                  the dialog box, followed by one or more additional 
**                  blocks of data that describe each of the controls in 
**                  the dialog box. The template can use either the 
**                  standard format or the extended format. 
**      hwndOwner : Identifies the window that owns the dialog box. 
**      pDlgProc  : Points to the dialog box procedure. 
**  Return   :
**      If the function succeeds, return the nResult parameter in the call
**      to the EndDialog function used to terminate the dialog box. 
**      If the function fails, return -1. 
*/
int WINAPI DialogBoxIndirect(HINSTANCE hInstance, PCDLGTEMPLATE pTemplate,
                             HWND hwndOwner, DLGPROC pDlgProc)
{
    return DialogBoxIndirectParam(hInstance, pTemplate, hwndOwner, 
        pDlgProc, 0);
}




/*
**  Function : DialogBoxParam
**  Purpose  :
**      Creates a modal dialog box from a dialog box template resource. 
**      Before displaying the dialog box, the function passes an 
**      application-defined value to the dialog box procedure as the lParam
**      parameter of the WM_INITDIALOG message. An application can use this
**      value to initialize dialog box controls. 
**  Params   :
**      hInstance   : Identifies an instance of the module who contains the
**                    dialog box template. 
**      pszTemplate : Identifies the dialog box template. This parameter is
**                    either the pointer to a null-terminated character 
**                    string that specifies the name of the dialog box 
**                    template or an integer value that specifies the 
**                    resource identifier of the dialog box template. If 
**                    the parameter specifies a resource identifier, its 
**                    high-order word must be zero and its low-order word 
**                    must contain the identifier. You can use the 
**                    MAKEINTRESOURCE macro to create this value. 
**      hwndOwner   : Identifies the window that owns the dialog box. 
**      pDlgProc    : Points to the dialog box procedure. 
**      dwInitParam : Specifies the value to pass to the dialog box in the 
**                    lParam parameter of the WM_INITDIALOG message. 
**  Return   :
**      If the function succeeds, return the nResult parameter in the call
**      to the EndDialog function used to terminate the dialog box. 
**      If the function fails, return -1. 
*/
int WINAPI DialogBoxParam(HINSTANCE hInstance, PCSTR pszTemplate, 
                          HWND hwndOwner, DLGPROC pDlgProc, 
                          LPARAM dwInitParam)
{
#ifndef NORESOURCE  // Resource management
    HRSRC hResInfo;
    HGLOBAL hResData;
    BYTE * pHMem;
    HWND hDlg;
    int nResult;
    MSG msg;

    hResInfo = FindResource(hInstance, pszTemplate, RT_DIALOG);
    if (hResInfo == NULL)
        return 0;
    hResData = LoadResource(hInstance, hResInfo);
    pHMem = (BYTE *)LockResource(hResData);
    if (pHMem == NULL)
        return 0;

    hDlg = CreateDialogWindowsIndirect(NULL, (PCDLGTEMPLATE)pHMem, 
        hwndOwner, pDlgProc, dwInitParam, TRUE);
    FreeResource(hResData);
    if (!hDlg)
        return -1;

    nResult = -1;

    // Enter dialog message loop
    while (IsWindow(hDlg))
    {
        if (DEFDLG_IsDialogEnd(hDlg, &nResult))
        {
            DestroyWindow(hDlg);
            break;
        }

        GetMessage(&msg, 0, 0, 0);

        if (!IsDialogMessage(hDlg, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    return nResult;
#else //NORESOURCE
	return NULL;
#endif //NORESOURCE
}

/*
**  Function : DialogBoxIndirectParam
**  Purpose  :
**      Creates a modal dialog box from a dialog box template in memory. 
**      Before displaying the dialog box, the function passes an 
**      application-defined value to the dialog box procedure as the lParam 
**      parameter of the WM_INITDIALOG message. An application can use this
**      value to initialize dialog box controls. 
**  Params   :
**      hInstance : Identifies an instance of the module who contains the
**                  dialog box template. 
**      pTemplate : Pointer to a global memory object containing a template
**                  that DialogBoxIndirect uses to create the dialog box. A
**                  dialog box template consists of a header that describes
**                  the dialog box, followed by one or more additional 
**                  blocks of data that describe each of the controls in 
**                  the dialog box. The template can use either the 
**                  standard format or the extended format. 
**      hwndOwner : Identifies the window that owns the dialog box. 
**      pDlgProc  : Points to the dialog box procedure. 
**      dwInitParam : Specifies the value to pass to the dialog box in the 
**                    lParam parameter of the WM_INITDIALOG message. 
**  Return   :
**      If the function succeeds, return the nResult parameter in the call
**      to the EndDialog function used to terminate the dialog box. 
**      If the function fails, return -1. 
*/
int WINAPI DialogBoxIndirectParam(HINSTANCE hInstance, 
                                  PCDLGTEMPLATE pTemplate, 
                                  HWND hwndOwner, DLGPROC pDlgProc, 
                                  LPARAM dwInitParam)
{
    HWND hDlg;
    int nResult;
    MSG msg;

    hDlg = CreateDialogWindowsIndirect(hInstance, pTemplate, hwndOwner, 
        pDlgProc, dwInitParam, TRUE);

    if (!hDlg)
        return -1;

    nResult = -1;

    // Enter dialog message loop
    while (IsWindow(hDlg))
    {
        if (DEFDLG_IsDialogEnd(hDlg, &nResult))
        {
            DestroyWindow(hDlg);
            break;
        }

        GetMessage(&msg, 0, 0, 0);

        if (!IsDialogMessage(hDlg, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    return nResult;
}

/*
**  Function : EndDialog
**  Purpose  :
**      Destroys a modal dialog box, causing the system to end any 
**      processing for the dialog box. 
**  Params   :
**      hDlg    : Identifies the dialog box to be destroyed. 
**      nResult : Specifies the value to be returned to the application 
**                from the function that created the dialog box.
**  Return   :
**      If the function succeeds, return TRUE. 
**      If the function fails, return FALSE. 
**  Remarks  :
**      Dialog boxes created by the DialogBox, DialogBoxParam, 
**      DialogBoxIndirect, and DialogBoxIndirectParam functions must be 
**      destroyed using the EndDialog function. An application calls 
**      EndDialog from within the dialog box procedure; the function must 
**      not be used for any other purpose. 
**      A dialog box procedure can call EndDialog at any time, even during 
**      the processing of the WM_INITDIALOG message. If your application 
**      calls the function while WM_INITDIALOG is being processed, the 
**      dialog box is destroyed before it is shown and before the input 
**      focus is set. 
**      EndDialog does not destroy the dialog box immediately. Instead, it 
**      sets a flag and allows the dialog box procedure to return control 
**      to the system. The system checks the flag before attempting to 
**      retrieve the next message from the application queue. If the flag 
**      is set, the system ends the message loop, destroys the dialog box, 
**      and uses the value in nResult as the return value from the function
**      that created the dialog box. 
*/
BOOL WINAPI EndDialog(HWND hDlg, int nResult)
{
    HWND hOwner;
    if (!IsWindow(hDlg))
    {
        SetLastError(1);
        return FALSE;
    }

    /* */
    hOwner = GetWindow(hDlg, GW_OWNER);
    EnableWindow(hOwner, TRUE);
    /*
    if (hDlg != GetSysModalWindow())
    {
    SetLastError(1);
    return FALSE;
    }
    */
    
    DEFDLG_EndDialog(hDlg, nResult);
    
    return TRUE;
}

/*
**  Function : GetDlgCtrlID
**  Purpose  :
**      Returns the identifier of the specified control. 
**  Params   :
**      hwndCtrl : Identifies the control. 
**  Returns  :
**      If the function succeeds, return the identifier of the control. 
**      If the function fails, the return value is zero. An invalid value 
**      for the hwndCtl parameter, will cause the function to fail. 
**  Remarks  :
**      GetDlgCtrlID accepts child window handles as well as handles of 
**      controls in dialog boxes. An application sets the identifier for a 
**      child window when it creates the window by assigning the identifier
**      value to the hmenu parameter when calling the CreateWindow or 
**      CreateWindowEx function. 
**      Although GetDlgCtrlID may return a value if hwndCtl identifies a
**      top-level window, top-level windows cannot have identifiers and 
**      such a return value is never valid. 
*/
int WINAPI GetDlgCtrlID(HWND hwndCtrl)
{
    return (int)GetWindowLong(hwndCtrl, GWL_ID);
}

/*
**  Function : GetDlgItem
**  Purpose  :
**      Retrieves the handle of a control in the specified dialog box. 
**  Params   :
**      hDlg    : Indentifies the dialog box that contains the control.
**      nItemID : Specifies the indentifier of the control to be retrieved.
**  Return   :
**      If the function succeeds, return the window handle of the given
**      control. 
**      If the function fails, return NULL, indicating an invalid dialog 
**      box handle or a nonexistent control. 
**  Remarks  :
**      You can use the GetDlgItem function with any parent-child window
**      pair, not just with dialog boxes. As long as the hDlg parameter 
**      specifies a parent window and the child window has a unique 
**      identifier (as specified by the hMenu parameter in the CreateWindow
**      or CreateWindowEx function that created the child window), 
**      GetDlgItem returns a valid handle to the child window. 
*/
HWND WINAPI GetDlgItem(HWND hDlg, int nItemID)
{
    HWND hwndCtl;
    int nCtlID;

    if (!IsWindow(hDlg))
    {
        SetLastError(1);
        return NULL;
    }

    // Get the first child of the specified dialog
    hwndCtl = GetWindow(hDlg, GW_CHILD);

    // Finds the control window whoese ID is equal to the specified ID
    while (hwndCtl != NULL)
    {
        if ((nCtlID = GetWindowLong(hwndCtl, GWL_ID)) == nItemID)
            break;

        hwndCtl = GetWindow(hwndCtl, GW_HWNDNEXT);
    }

    return hwndCtl;
}

/*
**  Function : SendDlgItemMessage
**  Purpose  :
**      Sends a message to the specified control in a dialog box. 
**  Params   :
**      hDlg    : Identifies the dialog box that contains the control. 
**      nItemID : Specifies the identifier of the control that receives the
**                message. 
**      wMsgCmd : Specifies the message to be sent. 
**      wParam  : Specifies additional message-specific information. 
**      lParam  : Specifies additional message-specific information. 
**  Return   :
**      return the result of the message processing and depends on the 
**      message sent. 
**  Remarks  :
**      The SendDlgItemMessage function does not return until the message
**      has been processed. 
**      Using SendDlgItemMessage is identical to retrieving a handle of 
**      the specified control and calling the SendMessage function. 
*/
LRESULT WINAPI SendDlgItemMessage(HWND hDlg, int nItemID, UINT wMsgCmd, 
                                  WPARAM wParam, LPARAM lParam)
{
    HWND hwndCtl;

    hwndCtl = GetDlgItem(hDlg, nItemID);

    if (!hwndCtl)
    {
        SetLastError(1);
        return 0;
    }

    return SendMessage(hwndCtl, wMsgCmd, wParam, lParam);
}

/*
**  Function : SetDlgItemText
**  Purpose  :
**      Identifies the dialog box that contains the control. 
**  Params   :
**      hDlg    : Identifies the dialog box that contains the control. 
**      nItemId : Identifies the control with a title or text that is to
**                be set. 
**      pszText : Points to the null-terminated string that contains the
**                text to be copied to the control. 
**  Return   :
**      If the function succeeds, return nonzero. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
**  Remarks  :
**      The SetDlgItemText function sends a WM_SETTEXT message to the 
**      specified control. 
*/
BOOL WINAPI SetDlgItemText(HWND hDlg, int nItemID, PCSTR pszText)
{
    HWND hwndCtl;

    hwndCtl = GetDlgItem(hDlg, nItemID);

    if (!hwndCtl)
    {
        SetLastError(1);
        return FALSE;
    }

    return SetWindowText(hwndCtl, pszText);
}

/*
**  Function : GetDlgItemText
**  Purpose  :
**      Retrieves the title or text associated with a control in a dialog
**      box. 
**  Params   :
**      hDlg      : Identifies the dialog box that contains the control. 
**      nItemId   : Identifies the control with a title or text that is to
**                  be set. 
**      pszText   : Points to the buffer to receive the title or text. 
**      nMaxCount : Specifies the maximum length, in characters, of the 
**                  string to be copied to the buffer pointed to by 
**                  lpString. If the length of the string exceeds the 
**                  limit, the string is truncated. 
**  Return   :
**      If the function succeeds, return the number of characters copied
**      to the buffer, not including the terminating null character. 
**      If the function fails, return zero. 
*/
int WINAPI GetDlgItemText(HWND hDlg, int nItemID, LPSTR pszText, 
                          int nMaxCount)
{
    HWND hwndCtl;

    hwndCtl = GetDlgItem(hDlg, nItemID);

    if (!hwndCtl)
    {
        SetLastError(1);
        return 0;
    }

    return GetWindowText(hwndCtl, pszText, nMaxCount);
}

/*
**  Function : CheckDlgButton
**  Purpose  :
**      Changes the check state of a button control. 
**  Params   :
**      hDlg    : Indentifies the dialog box that contains the button.
**      nItemID : Indentifies the button to modify.
**      uCheck  : Specifies the check state of the button, This parameter
**                can be one of the following value:
**                      BST_CHECKED, BST_INDETERMINATE, BST_UNCHECKED
**  Returns  :
**      If the function succeeds, return nonzero. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
*   Remarks  :
**      The CheckDlgButton function sends a BM_SETCHECK message to the 
**      specified button control in the specified dialog box. 
*/
BOOL WINAPI CheckDlgButton(HWND hDlg, int nItemID, UINT uCheck)
{
    if (uCheck != BST_CHECKED && uCheck != BST_INDETERMINATE &&
        uCheck != BST_UNCHECKED)
    {
        SetLastError(1);
        return FALSE;
    }

    return (BOOL)SendDlgItemMessage(hDlg, nItemID, BM_SETCHECK, 
        (WPARAM)uCheck, 0);
}

/*
**  Function : CheckRadioButton
**  Purpose  :
**      Adds a check mark to (checks) a specified radio button in a group
**      and removes a check mark from (clears) all other radio buttons in
**      the group. 
**  Params   :
**      hDlg     : Indentifies the dialog box that contains the button.
**      nItemID  : Indentifies the button to modify.
**      nFirstID : Specifies the identifier of the last radio button in 
**                 the group. 
**      nLastID  : Specifies the identifier of the last radio button in 
**                 the group. 
**      nCheckID : Specifies the identifier of the radio button to select. 
**  Returns  :
**      If the function succeeds, return nonzero. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
**  Remarks  :
**      The CheckRadioButton function sends a BM_SETCHECK message to each
**      of the radio buttons in the indicated group. 
*/
BOOL WINAPI CheckRadioButton(HWND hDlg, int nFirstID, int nLastID, 
                             int nCheckID)
{
    HWND hwndCtl;
    int nCtlID;
    DWORD dwStyle;

    // Gets the specified first radio button
    hwndCtl = GetDlgItem(hDlg, nFirstID);
    if (!hwndCtl)
    {
        SetLastError(1);
        return FALSE;
    }

    nCtlID = nFirstID;
    dwStyle = GetWindowLong(hwndCtl, GWL_STYLE);

    while (hwndCtl)
    {
        // Only check or uncheck the radio button control
        if (dwStyle & BS_RADIOBUTTON || dwStyle & BS_AUTORADIOBUTTON)
        {
            if (nCtlID != nCheckID)
                SendMessage(hwndCtl, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
            else
                SendMessage(hwndCtl, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
        }

        // The current control is the last control, break
        if (nCtlID == nLastID)
            break;

        // Gets the next control
        hwndCtl = GetWindow(hwndCtl, GW_HWNDNEXT);

        if (hwndCtl)
        {
            dwStyle = GetWindowLong(hwndCtl, GWL_STYLE);

            // If the group is end, break
            if (dwStyle & WS_GROUP)
                break;

            nCtlID = GetWindowLong(hwndCtl, GWL_ID);
        }
    }

    return TRUE;
}

/*
**  Function : IsDlgButtonChecked
**  Purpose  :
**      Determines whether a button control has a check mark next to it or
**      whether a three-state button control is grayed, checked, or neither. 
**  Params   :
**      hDlg    : Indentifies the dialog box that contains the button.
**      nItemID : Specifies the integer identifier of the button control. 
**  Returns  :
**      Return from a button created with the BS_AUTOCHECKBOX, 
**      BS_AUTORADIOBUTTON, BS_AUTO3STATE, BS_CHECKBOX, BS_RADIOBUTTON, or 
**      BS_3STATE style can be one of the following: 
**          BST_CHECKED, BST_INDETERMINATE, BST_UNCHECKED
**  Remarks  :
**      If the button has any other style, the return value is zero. 
*/
UINT WINAPI IsDlgButtonChecked(HWND hDlg, int nItemID)
{
    return (UINT)SendDlgItemMessage(hDlg, nItemID, BM_GETCHECK, 0, 0);
}

/*
**  Function : GetNextDlgGroupItem
**  Purpose  :
**      Retrieves the handle of the first control in a group of controls 
**      that precedes (or follows) the specified control in a dialog box. 
**  Params   :
**      hDlg  : Identifies the dialog box being searched. 
**      hCtl  : Identifies the control to be used as the starting point 
**              for the search. If this parameter is NULL, the function
**              uses the last (or first) control in the dialog box as the
**              starting point for the search. 
**      bPrev : Specifies how the function is to search the group of 
**              controls in the dialog box. If this parameter is TRUE, 
**              the function searches for the previous control in the 
**              group. If it is FALSE, the function searches for the next 
**              control in the group. 
**  Return   :
**      Retrieves the handle of the first control in a group of controls 
**      that precedes (or follows) the specified control in a dialog box. 
**  Remarks  :
**      The GetNextDlgGroupItem function searches controls in the order 
**      (or reverse order) they were created in the dialog box template. 
**      The first control in the group must have the WS_GROUP style; all 
**      other controls in the group must have been consecutively created 
**      and must not have the WS_GROUP style. 
**      When searching for the previous control, the function returns the
**      first control it locates that is visible and not disabled. If the
**      control given by hCtl has the WS_GROUP style, the function 
**      temporarily reverses the search to locate the first control having
**      the WS_GROUP style, then resumes the search in the original 
**      direction, returning the first control it locates that is visible 
**      and not disabled, or returning hCtl if no such control is found. 
**      When searching for the next control, the function returns the first
**      control it locates that is visible, not disabled, and does not have
**      the WS_GROUP style. If it encounters a control having the WS_GROUP
**      style, the function reverses the search, locates the first control
**      having the WS_GROUP style, and returns this control if it is 
**      visible and not disabled. Otherwise, the function resumes the 
**      search in the original direction and returns the first control it 
**      locates that is visible and not disabled, or returns hCtl if no 
**      such control is found. 
**
**      注意: 不能使用IsWindowVisible函数判断控件是否可见, 因为在对话框显示
**      之前, IsWindowVisible(hwndNextCtl)总是返回FALSE, 这样, 就无法在响应
**      WM_INITDIALOG消息正确使用本函数
*/
HWND WINAPI GetNextDlgGroupItem(HWND hDlg, HWND hCtl, BOOL bPrev)
{
    HWND hwndNextCtl, hwndLastCtl;
    DWORD dwStyle;

    if (!hDlg)
    {
        SetLastError(1);
        return NULL;
    }

    if (!hCtl)
    {
        if (bPrev)
        {
            hwndNextCtl = GetWindow(hDlg, GW_LASTCHILD);
            while (hwndNextCtl)
            {
                dwStyle = GetWindowLong(hwndNextCtl, GWL_STYLE);

                if ((dwStyle & WS_VISIBLE) && !(dwStyle & WS_DISABLED))
                    break;

                hwndNextCtl = GetWindow(hwndNextCtl, GW_HWNDPREV);

                dwStyle = GetWindowLong(hwndNextCtl, GWL_STYLE);
                if (dwStyle & WS_GROUP)
                {
                    hwndNextCtl = NULL;
                    break;
                }
            }
        }
        else
        {
            hwndNextCtl = GetWindow(hDlg, GW_CHILD);
            while (hwndNextCtl)
            {
                dwStyle = GetWindowLong(hwndNextCtl, GWL_STYLE);

                if ((dwStyle & WS_VISIBLE) && !(dwStyle & WS_DISABLED))
                    break;

                if (dwStyle & WS_GROUP)
                {
                    hwndNextCtl = NULL;
                    break;
                }

                hwndNextCtl = GetWindow(hwndNextCtl, GW_HWNDNEXT);
            }
        }

        return hwndNextCtl;
    }

    if (GetParent(hCtl) != hDlg)
    {
        SetLastError(1);
        return NULL;
    }
    
    if (bPrev)
    {
        dwStyle = GetWindowLong(hCtl, GWL_STYLE);

        // If the specified control isn't the first control in the group, 
        // searches the previous control in the controls that precedes the
        // specified control
        if (!(dwStyle & WS_GROUP))
        {
            hwndNextCtl = GetWindow(hCtl, GW_HWNDPREV);
            while (hwndNextCtl)
            {
                dwStyle = GetWindowLong(hwndNextCtl, GWL_STYLE);

                if ((dwStyle & WS_VISIBLE) && !(dwStyle & WS_DISABLED))
                    return hwndNextCtl;
                
                if (dwStyle & WS_GROUP)
                    break;

                hwndNextCtl = GetWindow(hwndNextCtl, GW_HWNDPREV);
            }
        }

        // Can't find the proper control prededes the specified control, 
        // finds the last proper control follows the specified control 
        // in the group
        
        // Finds the last proper control in the group
        hwndLastCtl = hCtl;
        hwndNextCtl = GetWindow(hCtl, GW_HWNDNEXT);
        while (hwndNextCtl)
        {
            dwStyle = GetWindowLong(hwndNextCtl, GWL_STYLE);
            if (dwStyle & WS_GROUP)
                break;
            
            if ((dwStyle & WS_VISIBLE) && !(dwStyle & WS_DISABLED))
                hwndLastCtl = hwndNextCtl;

            hwndNextCtl = GetWindow(hwndNextCtl, GW_HWNDNEXT);
        }
        
        return hwndLastCtl;
    }

    // Now gets the next proper control in the same group
    hwndNextCtl = GetWindow(hCtl, GW_HWNDNEXT);
    while (hwndNextCtl)
    {
        dwStyle = GetWindowLong(hwndNextCtl, GWL_STYLE);
        if (dwStyle & WS_GROUP)
            break;

        if ((dwStyle & WS_VISIBLE) && !(dwStyle & WS_DISABLED))
            return hwndNextCtl;

        hwndNextCtl = GetWindow(hwndNextCtl, GW_HWNDNEXT);
    }

    // Gets the first proper control in the group
    hwndLastCtl = hCtl;
    hwndNextCtl = GetWindow(hCtl, GW_HWNDPREV);
    while (hwndNextCtl)
    {
        dwStyle = GetWindowLong(hwndNextCtl, GWL_STYLE);

        if ((dwStyle & WS_VISIBLE) && !(dwStyle & WS_DISABLED))
            hwndLastCtl = hwndNextCtl;

        if (dwStyle & WS_GROUP)
            break;

        hwndNextCtl = GetWindow(hwndNextCtl, GW_HWNDPREV);
    }

    return hwndLastCtl;
}

/*
**  Function : GetNextDlgTabItem
**  Purpose  :
**      Retrieves the handle of the first control that has the WS_TABSTOP
**      style that precedes (or follows) the specified control. 
**  Params   :
**      hdlg    : Identifies the dialog box to be searched. 
**      hCtl    : Identifies the control to be used as the starting point 
**                for the search. If this parameter is NULL, the function 
**                uses the last (or first) control in the dialog box as 
**                the starting point for the search. 
**      bPrev   : Specifies how the function is to search the dialog box. 
**                If this parameter is TRUE, the function searches for the
**                previous control in the dialog box. If this parameter is
**                FALSE, the function searches for the next control in the
**                dialog box. 
**  Return   :
**      If the function succeeds, the return value is the window handle of
**      the previous (or next) control that has the WS_TABSTOP style set. 
**  Remarks  :
**      The GetNextDlgTabItem function searches controls in the order (or 
**      reverse order) they were created in the dialog box template. The 
**      function returns the first control it locates that is visible, not
**      disabled, and has the WS_TABSTOP style. If no such control exists,
**      the function returns hCtl. 
*/
HWND WINAPI GetNextDlgTabItem(HWND hDlg, HWND hCtl, BOOL bPrev)
{
    HWND hwndNextCtl, hwndLastCtl;
    DWORD dwStyle;

    if (!hDlg)
    {
        SetLastError(1);
        return NULL;
    }

    if (!hCtl)
    {
        if (!bPrev)
        {
            hwndNextCtl = GetWindow(hDlg, GW_LASTCHILD);
            while (hwndNextCtl)
            {
                dwStyle = GetWindowLong(hwndNextCtl, GWL_STYLE);

                if ((dwStyle & WS_VISIBLE) && !(dwStyle & WS_DISABLED) &&
                    (dwStyle & WS_TABSTOP))
                    break;

                hwndNextCtl = GetWindow(hwndNextCtl, GW_HWNDPREV);
            }
        }
        else
        {
            hwndNextCtl = GetWindow(hDlg, GW_CHILD);
            while (hwndNextCtl)
            {
                dwStyle = GetWindowLong(hwndNextCtl, GWL_STYLE);

                if ((dwStyle & WS_VISIBLE) && !(dwStyle & WS_DISABLED) &&
                    (dwStyle & WS_TABSTOP))
                    break;

                hwndNextCtl = GetWindow(hwndNextCtl, GW_HWNDNEXT);
            }
        }

        return hwndNextCtl;
    }

    if (GetParent(hCtl) != hDlg)
    {
        SetLastError(1);
        return NULL;
    }
    
    if (!bPrev)
    {
        hwndNextCtl = GetWindow(hCtl, GW_HWNDPREV);
        while (hwndNextCtl)
        {
            dwStyle = GetWindowLong(hwndNextCtl, GWL_STYLE);
            
            if ((dwStyle & WS_VISIBLE) && !(dwStyle & WS_DISABLED) &&
                (dwStyle & WS_TABSTOP))
                return hwndNextCtl;

            hwndNextCtl = GetWindow(hwndNextCtl, GW_HWNDPREV);
        }

        // Gets the last tab stop control in the dialog
        hwndLastCtl = hCtl;
        hwndNextCtl = GetWindow(hCtl, GW_HWNDNEXT);
        while (hwndNextCtl)
        {
            dwStyle = GetWindowLong(hwndNextCtl, GWL_STYLE);
            
            if ((dwStyle & WS_VISIBLE) && !(dwStyle & WS_DISABLED) &&
                (dwStyle & WS_TABSTOP))
                hwndLastCtl = hwndNextCtl;
            
            hwndNextCtl = GetWindow(hwndNextCtl, GW_HWNDNEXT);
        }
        
        return hwndLastCtl;
    }

    // Now get the next tab stop control in the dialog

    hwndNextCtl = GetWindow(hCtl, GW_HWNDNEXT);
    while (hwndNextCtl)
    {
        dwStyle = GetWindowLong(hwndNextCtl, GWL_STYLE);
        if ((dwStyle & WS_VISIBLE) && !(dwStyle & WS_DISABLED) &&
            (dwStyle & WS_TABSTOP))
            return hwndNextCtl;

        hwndNextCtl = GetWindow(hwndNextCtl, GW_HWNDNEXT);
    }

    // Gets the first tab stop control in the dialog
    hwndLastCtl = hCtl;
    hwndNextCtl = GetWindow(hCtl, GW_HWNDPREV);
    while (hwndNextCtl)
    {
        dwStyle = GetWindowLong(hwndNextCtl, GWL_STYLE);
        
        if ((dwStyle & WS_VISIBLE) && !(dwStyle & WS_DISABLED) &&
            (dwStyle & WS_TABSTOP))
            hwndLastCtl = hwndNextCtl;

        hwndNextCtl = GetWindow(hwndNextCtl, GW_HWNDPREV);
    }

    return hwndLastCtl;
}

// Two macro used in LoadImage function

#define GET32(p, v) \
{   \
    (v) = (DWORD)((((DWORD)*((BYTE*)(p) + 3)) << 24) + (((DWORD)*((BYTE*)(p) + 2)) << 16)  + \
        (((DWORD)*((BYTE*)(p) + 1) << 8)) +  *((BYTE*)(p))); \
    (p) = ((BYTE*)(p)) + 4;  \
}

#define GET16(p, v)   \
{   \
    (v) = (WORD)((((WORD)*((BYTE*)(p) + 1)) << 8) + *((BYTE*)(p))); \
    (p) = ((BYTE*)(p)) + 2;  \
}

typedef struct 
{
    WORD    dlgVer;
    WORD    signature;
    DWORD   helpID;
    DWORD   type;               // Dialog template type, must be zero
    DWORD   style;              // Dialog style
    DWORD   dwExStyle;          // Dialog extend style
    DWORD   wItems;               // Count of dialog control items
    WORD    x;                  // x-coordinates of dialog left-top corner
    WORD    y;                  // y-coordinates of dialog left-top corner
    WORD    cx;                 // width of dialog left-top corner
    WORD    cy;                 // height of dialog left-top corner
    WORD    MenuName;       // Dialog menu name
    WORD    ClassName;      // Dialog class name
    WORD    WindowText;     // Dialog title
    WORD    pointsize;
    WORD    weight;
    WORD    bItalic;
    WORD    font;
    //CONTROLDATA   ControlList[];
    //BYTE          TextString[];
} DLGTEMPLATEHEADER, *PDLGTEMPLATEHEADER;

typedef struct
{
    DWORD   helpID;
    DWORD   style;
    DWORD   dwStyle;
    WORD    x;
    WORD    y;
    WORD    cx;
    WORD    cy;
    WORD    wID;
    WORD    wClass;
    WORD    wText;
    WORD    extraCount;
}DLGITEMHEADER, *PDLGITEMHEADER;


//定义此结构是为了约束Dialog中dlgItems的项数。   

const static char ControlClassNameTable[16][16] = 
{
    "BUTTON", "EDIT", "STATIC", "LISTBOX", "SCROLLBAR", "COMBOBOX"
};

#define CT_BUTTON       32769
#define CT_EDITBOX      32770
#define CT_STATIC       32771
#define CT_LISTBOX      32772
#define CT_SCROLLBAR    32773
#define CT_COMBOBOX     32774
#define CT_FIRST        CT_BUTTON


// Internal functions

/*
**  Function : CreateDialogWindowsIndirect
**  Purpose  :
**      生成由内存模板指定的对话框和对话框控件. 若生成成功, 返回对话框的句
**      柄, 否则返回NULL.
*/
static HWND CreateDialogWindowsIndirect(HINSTANCE hInstance, 
                                        PCDLGTEMPLATE pTemplate, 
                                        HWND hwndOwner, DLGPROC pDlgProc, 
                                        LPARAM dwInitParam, BOOL bModal)
{
    HWND hDlg, hwndCtl, hwndFocusCtl;
    PCSTR lpszClassName;
    PCDLGITEMTEMPLATE pDlgItem;
    DLGCREATEPARAM DlgCreateParam;
    WORD i;
    DWORD dwStyle;

    DLGTEMPLATEHEADER   DlgTemHeader;
    DLGITEMHEADER  DlgItemHeader;
    BYTE *pMem;
    BYTE *strpos;
    PCSTR pMenuName, pWindowText, pClassName;
    int   count;
    int   exCount;
    
    // 参数有效性判断, 当对话框模板或对话框函数指定时, 返回NULL
    if (!pTemplate || !pDlgProc)
    {
        SetLastError(1);
        return NULL;
    }

    hwndFocusCtl = NULL;
    
    if (pTemplate->type == 0)
    {
        // Memory dialog template

        if (pTemplate->lpszClassName)
            lpszClassName = pTemplate->lpszClassName;
        else
            lpszClassName = "DIALOG";

        // Preparas the dialog create params 
        DlgCreateParam.pDlgProc = pDlgProc;
        DlgCreateParam.bModal = bModal;

        // 创建对话框窗口, 去掉WS_VISIBLE风格可以使创建效率较高
        hDlg = CreateWindowEx(
            pTemplate->dwExStyle, 
            lpszClassName, 
            pTemplate->lpszWindowText, 
            (pTemplate->style & (unsigned long)~WS_CHILD) & (unsigned long)~WS_VISIBLE,
            pTemplate->x, 
            pTemplate->y, 
            pTemplate->cx, 
            pTemplate->cy, 
            hwndOwner, 
            NULL, 
            NULL, 
            &DlgCreateParam
            );

        if (!hDlg)
        {
            SetLastError(1);
            return NULL;
        }

        // 创建对话框模板中指定的控件窗口, 记录可见的, 未被禁止具有
        // WS_TABSTOP属性的第一个控件句柄
        for (i = 0; i < pTemplate->cdit; i++)
        {
            pDlgItem = &(pTemplate->dlgItems[i]);
            hwndCtl = CreateWindowEx(
                pDlgItem->dwExStyle, 
                pDlgItem->lpszClassName, 
                pDlgItem->lpszWindowText, 
                pDlgItem->style | WS_CHILD,
                pDlgItem->x, 
                pDlgItem->y, 
                pDlgItem->cx, 
                pDlgItem->cy,
                hDlg, 
                (HMENU)pDlgItem->id, 
                NULL, 
                NULL 
                );
            
            if (hwndCtl && !hwndFocusCtl)
            {
                if ((pDlgItem->style & WS_VISIBLE) && 
                    (pDlgItem->style & WS_TABSTOP) &&
                    !(pDlgItem->style & WS_DISABLED))
                    hwndFocusCtl = hwndCtl;
            }
        }
    }
    else
    {
        // For resource dialog template

        pMem = (BYTE*)pTemplate;

        GET16(pMem, DlgTemHeader.dlgVer);
        GET16(pMem, DlgTemHeader.signature);
        GET32(pMem, DlgTemHeader.helpID);
        GET32(pMem, DlgTemHeader.style);
        GET32(pMem, DlgTemHeader.dwExStyle);
        GET16(pMem, DlgTemHeader.wItems);
        GET16(pMem, DlgTemHeader.x);
        GET16(pMem, DlgTemHeader.y);
        GET16(pMem, DlgTemHeader.cx);
        GET16(pMem, DlgTemHeader.cy);
        GET16(pMem, DlgTemHeader.MenuName);
        GET16(pMem, DlgTemHeader.ClassName);
        GET16(pMem, DlgTemHeader.WindowText);
        GET16(pMem, DlgTemHeader.pointsize);
        GET16(pMem, DlgTemHeader.weight);
        GET16(pMem, DlgTemHeader.bItalic);
        GET16(pMem, DlgTemHeader.font);
        
        strpos = (BYTE*)pMem;
        count = DlgTemHeader.wItems;
        while (count != 0)
        {
            strpos += sizeof(DLGITEMHEADER) - sizeof(WORD);//WORD extraCout;
            GET16(strpos, exCount);
            if (exCount % 4 != 0)
            {
                strpos += 4 - exCount % 4;
            }
            strpos += exCount;
            count --;
        }
        
        pMenuName = (char *)strpos + DlgTemHeader.MenuName;
        pClassName = (char *)strpos + DlgTemHeader.ClassName;
        pWindowText = (char *)strpos + DlgTemHeader.WindowText;
        
        if (*pClassName == '\0')
            pClassName = "DIALOG";

        DlgCreateParam.pDlgProc = pDlgProc;
        DlgCreateParam.bModal = bModal;
        
        hDlg = CreateWindowEx(
            DlgTemHeader.dwExStyle,
            pClassName,
            pWindowText,
            (DlgTemHeader.style & (unsigned long)~WS_CHILD) & (unsigned long)~WS_VISIBLE,
            DlgTemHeader.x,
            DlgTemHeader.y,
            DlgTemHeader.cx,
            DlgTemHeader.cy,
            hwndOwner,
            NULL,
            NULL,
            &DlgCreateParam
            );

        if (!hDlg)
        {
            SetLastError(1);
            return NULL;
        }
        
        count = DlgTemHeader.wItems;
        while (count != 0)
        {
            GET32(pMem, DlgItemHeader.helpID);
            GET32(pMem, DlgItemHeader.style);
            GET32(pMem, DlgItemHeader.dwStyle);
            GET16(pMem, DlgItemHeader.x);
            GET16(pMem, DlgItemHeader.y);
            GET16(pMem, DlgItemHeader.cx);
            GET16(pMem, DlgItemHeader.cy);
            GET16(pMem, DlgItemHeader.wID);
            GET16(pMem, DlgItemHeader.wClass);
            GET16(pMem, DlgItemHeader.wText);
            GET16(pMem, DlgItemHeader.extraCount);
            
            if ( DlgItemHeader.wClass < CT_FIRST)
                pClassName = (PSTR)strpos + DlgItemHeader.wClass;
            else
                pClassName = (char *)ControlClassNameTable[(WORD)(DlgItemHeader.wClass
                - CT_FIRST)];
            
            pWindowText = (PSTR)strpos + DlgItemHeader.wText;
            
            hwndCtl = CreateWindowEx(
                DlgItemHeader.dwStyle,
                pClassName,
                pWindowText,
                DlgItemHeader.style | WS_CHILD,
                DlgItemHeader.x,
                DlgItemHeader.y,
                DlgItemHeader.cx,
                DlgItemHeader.cy,
                hDlg,
                (HMENU)((DWORD)DlgItemHeader.wID),
                NULL,
                NULL
                );

            if (hwndCtl && !hwndFocusCtl)
            {
                if ((DlgItemHeader.style & WS_VISIBLE) && 
                    (DlgItemHeader.style & WS_TABSTOP) &&
                    !(DlgItemHeader.style & WS_DISABLED))
                    hwndFocusCtl = hwndCtl;
            }
            
            exCount = DlgItemHeader.extraCount;
            while (exCount > 0)
            {
                SendDlgItemMessage(hDlg, DlgItemHeader.wID,
                    CB_ADDSTRING, (WPARAM)-1, (LPARAM)pMem);
                exCount -= strlen((char *)pMem) + 1;
                pMem += strlen((char *)pMem) + 1;
            }
            if ((DlgItemHeader.extraCount % 4) != 0)
                pMem += 4 - DlgItemHeader.extraCount % 4;
            
            count --;
        }
    }
    
    // 根据WM_INITDIALOG的返回值设置对话框的focus控件
    
    if (SendMessage(hDlg, WM_INITDIALOG, (WPARAM)hwndFocusCtl, 
        (LPARAM)dwInitParam))
    {
        if (hwndFocusCtl)
        {
            dwStyle = GetWindowLong(hwndFocusCtl, GWL_STYLE);
            if (!(dwStyle & WS_VISIBLE) || (dwStyle & WS_DISABLED))
            {
                hwndFocusCtl = GetNextDlgTabItem(hDlg, hwndFocusCtl, 
                    FALSE);
            }
        }
    }
    else
    {
        hwndFocusCtl = GetFocus();
        if (!IsChild(hDlg, hwndFocusCtl))
            hwndFocusCtl = NULL;
        else
        {
            HWND hwndParent;
            
            hwndParent = GetParent(hwndFocusCtl);
            if (hwndParent != hDlg)
                hwndFocusCtl = hwndParent;
        }
    }
    
    DEFDLG_SetFocusCtl(hDlg, hwndFocusCtl);
    
    /* */
    if (bModal)
        EnableWindow(hwndOwner, FALSE);
    //        SetSysModalWindow(hDlg);
    
    if (bModal || (pTemplate->style & WS_VISIBLE))
    {
        ShowWindow(hDlg, SW_SHOW);
        
        UpdateWindow(hDlg);
    }

    return hDlg;
}
