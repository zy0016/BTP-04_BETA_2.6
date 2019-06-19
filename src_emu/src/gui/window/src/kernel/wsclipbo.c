/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements Clipboard Manager Functions.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "wsownd.h"
#include "wsthread.h"

#ifndef NOCLIPBOARD

static HWND hwndOpen;       // Indentifies the window opens the clipboard
static HWND hwndOwner;      // Indentifies the window owns the clipboard
static UINT uDataFormat;    // Current clipboard data format
static HANDLE hDataMem;     // Handle of the clipboard data memory    

static COMMONMUTUALOBJ sem_clipboard;

#if (__MP_PLX_GUI)
#ifdef _EMULATE_

#define SEMNAME_CLIPBOARD   "$$PLXGUI_CLIPBOARD$$"
#define CLIPBOARD_CREATEOBJ(sem)        CREATEOBJ(sem, SEMNAME_CLIPBOARD)
#define CLIPBOARD_DESTROYOBJ(sem)       DESTROYOBJ(sem, SEMNAME_CLIPBOARD)
#define CLIPBOARD_WAITOBJ(sem)          WAITOBJ(sem, SEMNAME_CLIPBOARD)
#define CLIPBOARD_RELEASEOBJ(sem)       RELEASEOBJ(sem, SEMNAME_CLIPBOARD)
#else
#define CLIPBOARD_CREATEOBJ(sem)        CREATEOBJ(sem)
#define CLIPBOARD_DESTROYOBJ(sem)       DESTROYOBJ(sem)
#define CLIPBOARD_WAITOBJ(sem)          WAITOBJ(sem)
#define CLIPBOARD_RELEASEOBJ(sem)       RELEASEOBJ(sem)
#endif
#else
#define CLIPBOARD_CREATEOBJ(sem)        CREATEOBJ(sem)
#define CLIPBOARD_DESTROYOBJ(sem)       DESTROYOBJ(sem)
#define CLIPBOARD_WAITOBJ(sem)          WAITOBJ(sem)
#define CLIPBOARD_RELEASEOBJ(sem)       RELEASEOBJ(sem)
#endif

BOOL WS_InitClipboardSem(void)
{
    CLIPBOARD_CREATEOBJ(sem_clipboard);

    return TRUE;
}

BOOL WS_DestroyClipboardSem(void)
{
    CLIPBOARD_DESTROYOBJ(sem_clipboard);

    return TRUE;
}
/*
**  Function : OpenClipboard
**  Purpose  :
**      Opens the clipboard for examination and prevents other applications
**      from modifying the clipboard content. 
**  Params   :
**      hWnd : Identifies the window to be associated with the open 
**             clipboard. If this parameter is NULL, the open clipboard is 
**             associated with the current task. 
**  Return   :
**      If the function succeeds, return nonzero. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
**  Remark   :
**      OpenClipboard fails if another window has the clipboard open. 
**      An application should call the CloseClipboard function after every 
**      successful call to OpenClipboard. 
**      The window identified by the hWnd parameter does not become the 
**      clipboard owner unless the EmptyClipboard function is called. 
*/
BOOL WINAPI OpenClipboard(HWND hWnd)
{
    CLIPBOARD_WAITOBJ(sem_clipboard);

    // The clipboard has been opened by another window, return FALSE;
    if (hwndOpen)
    {
        SetLastError(1);
        CLIPBOARD_RELEASEOBJ(sem_clipboard);
        return FALSE;
    }

    // The specified window handle isn't a valid window handle, return 
    // FALSE
    if (!hWnd || !IsWindow(hWnd))
    {
        SetLastError(1);
        CLIPBOARD_RELEASEOBJ(sem_clipboard);
        return FALSE;
    }

    hwndOpen = hWnd;

    CLIPBOARD_RELEASEOBJ(sem_clipboard);
    return TRUE;
}

/*
**  Function : CloseClipboard
**  Purpose  :
**      Closes the clipboard. 
**  Params   :
**      None.
**  Return   :
**      If the function succeeds, return nonzero. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
**  Remark   :
**      When the window has finished examining or changing the clipboard, 
**      close the clipboard by calling CloseClipboard. This enables other 
**      windows to access the clipboard. 
**      Do not place an object on the clipboard after calling 
**      CloseClipboard. 
*/
BOOL WINAPI CloseClipboard(void)
{
    CLIPBOARD_WAITOBJ(sem_clipboard);

    // The clipboard isn't opened, return FALSE;
    if (!hwndOpen)
    {
        SetLastError(1);
        CLIPBOARD_RELEASEOBJ(sem_clipboard);
        return FALSE;
    }

    hwndOpen = NULL;

    CLIPBOARD_RELEASEOBJ(sem_clipboard);
    return TRUE;
}

/*
**  Function : EmptyClipboard
**  Purpose  :
**      Empties the clipboard and frees handles to data in the clipboard. 
**      The function then assigns ownership of the clipboard to the window
**      that currently has the clipboard open. 
**  Params   :
**      None.
**  Return   :
**      If the function succeeds, return TRUE. 
**      If the function fails, return FALSE. To get extended error 
**      information, call GetLastError. 
**  Remark   :
**      Before calling EmptyClipboard, an application must open the 
**      clipboard by using the OpenClipboard function. If the application
**      specifies a NULL window handle when opening the clipboard, 
**      EmptyClipboard succeeds but sets the clipboard owner to NULL.
*/
BOOL WINAPI EmptyClipboard(void)
{
    CLIPBOARD_WAITOBJ(sem_clipboard);

    // The clipboard isn't opened, return FALSE;
    if (!hwndOpen)
    {
        SetLastError(1);
        CLIPBOARD_RELEASEOBJ(sem_clipboard);
        return FALSE;
    }

    // Free the clipboard data memory if necessary
    if (hDataMem)
    {
        GlobalFree(hDataMem);

        uDataFormat = 0;
        hDataMem = NULL;
    }

    // Set the clipboard owner window to the window which call 
    // OpenClipborad
    hwndOwner = hwndOpen;

    CLIPBOARD_RELEASEOBJ(sem_clipboard);
    return TRUE;
}

/*
**  Function : SetClipboardData
**  Purpose  :
**      The SetClipboardData function places data on the clipboard in a 
**      specified clipboard format. The window must be the current 
**      clipboard owner, and the application must have called the 
**      OpenClipboard function.
**  Params   :
**      uFormat : Specifies a clipboard format. This parameter can be a 
**                registered format or any of the standard clipboard formats 
**                listed in the following Remarks section. For information 
**                about registered clipboard formats, see the 
**                RegisterClipboardFormat function. 
**      hMem    : Identifies the data in the specified format. 
**                Once SetClipboardData is called, the system owns the 
**                object identified by the hMem parameter. The application
**                can read the data, but must not free the handle or leave
**                it locked. If the hMem parameter identifies a memory 
**                object, the object must have been allocated using the 
**                GlobalAlloc function with the GMEM_MOVEABLE flags. 
**  Return   :
**      If the function succeeds, return the handle of the data. 
**      If the function fails, return NULL. To get extended error 
**      information, call GetLastError. 
*/
HANDLE WINAPI SetClipboardData(UINT uFormat, HANDLE hMem)
{
    CLIPBOARD_WAITOBJ(sem_clipboard);

    // Only support CF_TEXT format now.
    if (uFormat != CF_TEXT)
    {
        SetLastError(1);
        CLIPBOARD_RELEASEOBJ(sem_clipboard);
        return NULL;
    }

    // Must have called OpenClipboard 
    if (!hwndOpen)
    {
        SetLastError(1);
        CLIPBOARD_RELEASEOBJ(sem_clipboard);
        return NULL;
    }

    // Free the clipboard data memory if necessary
    if (hDataMem)
    {
        GlobalFree(hDataMem);

        uDataFormat = 0;
        hDataMem = NULL;
    }

    uDataFormat = uFormat;
    hDataMem = hMem;

    CLIPBOARD_RELEASEOBJ(sem_clipboard);
    return hMem;
}


/*
**  Function : GetClipboardData
**  Purpose  :
**      Retrieves data from the clipboard in a specified format. The 
**      clipboard must have been opened previously. 
**  Params   :
**      uFormat : Specifies a clipboard format. For a description of the 
**                clipboard formats, see the SetClipboardData function. 
**  Return   :
**      If the function succeeds, return TRUE. 
**      If the function fails, return FALSE. To get extended error 
**      information, call GetLastError. 
**  Remark   :
**      The clipboard controls the handle that the GetClipboardData function
**      returns, not the application. The application should copy the data
**      immediately. The application cannot rely on being able to make 
**      long-term use of the handle. The application must not free the 
**      handle nor leave it locked. 
*/
HANDLE WINAPI GetClipboardData(UINT uFormat)
{
    HANDLE hRet;
    CLIPBOARD_WAITOBJ(sem_clipboard);

    // Only support CF_TEXT format now.
    if (uFormat != CF_TEXT)
    {
        SetLastError(1);
        CLIPBOARD_RELEASEOBJ(sem_clipboard);
        return NULL;
    }

    // If the current data format is not the specified data format
    if (uDataFormat != uFormat)
    {
        SetLastError(1);
        CLIPBOARD_RELEASEOBJ(sem_clipboard);
        return NULL;
    }

    // Must have called OpenClipboard 
    if (!hwndOpen)
    {
        SetLastError(1);
        CLIPBOARD_RELEASEOBJ(sem_clipboard);
        return NULL;
    }

    hRet = hDataMem;
    CLIPBOARD_RELEASEOBJ(sem_clipboard);
    return hRet;
}

/*
**  Function : IsClipboardFormatAvailable
**  Purpose  :
**      Determines whether the clipboard contains data in the specified 
**      format. 
**  Params   :
**      uFormat : Specifies a standard or registered clipboard format. 
**  Return   :
**      If the function succeeds, return TRUE. 
**      If the function fails, return FALSE. To get extended error 
**      information, call GetLastError. 
*/
BOOL WINAPI IsClipboardFormatAvailable(UINT uFormat)
{
    BOOL bRet;

    CLIPBOARD_WAITOBJ(sem_clipboard);

    // If the current data format is not the specified data format
    if (uDataFormat != uFormat)
    {
        SetLastError(1);
        CLIPBOARD_RELEASEOBJ(sem_clipboard);
        return FALSE;
    }

    // Must have called OpenClipboard 
    if (!hwndOpen)
    {
        SetLastError(1);
        CLIPBOARD_RELEASEOBJ(sem_clipboard);
        return FALSE;
    }

    bRet = (hDataMem != NULL);

    CLIPBOARD_RELEASEOBJ(sem_clipboard);
    return bRet;
}

/*
**  Function : RegisterClipboardFormat
**  Purpose  :
**      Registers a new clipboard format. This format can then be used as 
**      a valid clipboard format. 
**  Params   :
**      pszFormat : Points to a null-terminated string that names the new
**                  format. 
**  Return   :
**      If the function succeeds, return identifies the registered 
**      clipboard format. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
**  Remark   :
**      If a registered format with the specified name already exists, a 
**      new format is not registered and the return value identifies the
**      existing format. This enables more than one application to copy 
**      and paste data using the same registered clipboard format. Note 
**      that the format name comparison is case-insensitive. 
**      Registered clipboard formats are identified by values in the range 
**      0xC000 through 0xFFFF. 
*/
UINT WINAPI RegisterClipboardFormat(PCSTR pszFormat)
{
    return 0;
}

/*
**  Function : GetClipboardFormatName
**  Purpose  :
**      Retrieves from the clipboard the name of the specified registered
**      format. The function copies the name to the specified buffer. 
**  Params   :
**      uFormat       : Specifies the type of format to be retrieved. This
**                      parameter must not specify any of the predefined 
**                      clipboard formats. 
**      pszFormatName : Points to the buffer that is to receive the format
**                      name. 
**      nMaxCount     : Specifies the maximum length, in characters, of the
**                      string to be copied to the buffer. If the name 
**                      exceeds this limit, it is truncated. 
**  Return   :
**      If the function succeeds, return the length, in characters, of the
**      string copied to the buffer. 
**      If the function fails, return zero, indicating that the requested
**      format does not exist or is predefined. To get extended error 
**      information, call GetLastError. 
*/
int WINAPI GetClipboardFormatName(UINT uFormat, PSTR pszFormatName, 
                                  int nMaxCount)
{
    return 0;
}

#endif  /* NOCLIPBOARD */
