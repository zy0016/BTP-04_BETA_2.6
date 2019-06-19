/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements interface functions called by application.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "hpfile.h"
#include "windowx.h"

#include "string.h"

#include "wsobj.h"
#include "wsownd.h"
#include "wsocur.h"
#include "wsoicon.h"
#include "clstbl.h"
#include "devinput.h"
#include "wscursor.h"
#include "wscaret.h"
#include "wstimer.h"

#include "wsthread.h"
#include "msgqueue.h"

#include "inputmgr.h"
#include "devqueue.h"

/**************************************************************************/
/*              Memory function                                           */
/**************************************************************************/

// Memory functions are implemented in memory.c

/**************************************************************************/
/*                  System Time Functions                                 */ 
/**************************************************************************/

// System Time Functions implemented in wstime.c

/**************************************************************************/
/*                  Error Functions                                       */ 
/**************************************************************************/

// Error functions are implemented in error.c

/**************************************************************************/
/*              Resource  Functions                                       */ 
/**************************************************************************/

// Resource functions are implemented in resmgr.c

/**************************************************************************/
/*              GDI functions                                             */
/**************************************************************************/

// GDI functions are implemented in apigdi.c

/**************************************************************************/
/*              USER functions                                            */ 
/**************************************************************************/

/**************************************************************************/
/*              System Metrics                                            */ 
/**************************************************************************/

// System metrics function implemendted in sysmetri.c

/**************************************************************************/
/*              Rectangle Support                                         */
/**************************************************************************/

// Rectangle management functions implements in rectmgr.c

/**************************************************************************/
/*              Window message support                                    */ 
/**************************************************************************/

// Message support functions implemented in message.c

/**************************************************************************/
/*              Window class management                                   */ 
/**************************************************************************/

/*
**  Function : RegisterClassBase
**  Purpose  : Registers a ekrnel window class for subsequent use in calls
**             to the CreateWindow function. 
**  Params   :
**      pWndClass : Points to a WNDCLASS structure. It must be filled
**                  with the appropriate class attributes before 
**                  passed to the function. 
**  Return   :
**      If the function succeeds, the return value is TRUE. Otherwise, 
**      the return value is FALSE. To get extended error information, 
**      call GetLastError. 
*/
BOOL WINAPI RegisterClassBase(const WNDCLASS* pWndClass)
{
    PCLSOBJ pClsObj;

    if (!pWndClass)
    {
        SetLastError(1);
        return FALSE;
    }

    if (!pWndClass->lpfnWndProc || !pWndClass->lpszClassName)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    pClsObj = CLSTBL_Insert(pWndClass);

    LEAVEMONITOR;

    if (!pClsObj)
    {
        SetLastError(1);
        return FALSE;
    }

    return TRUE;
}

#if (!BASEWINDOW)
BOOL WINAPI RegisterClass(const WNDCLASS* pWndClass)
{
    WNDCLASS wc;
    PCLSOBJ pClsObj;
    PCLSOBJ pClsDefWnd;

    if (!pWndClass)
    {
        SetLastError(1);
        return FALSE;
    }

    if (!pWndClass->lpfnWndProc || !pWndClass->lpszClassName)
    {
        SetLastError(1);
        return FALSE;
    }

    memcpy(&wc, pWndClass, sizeof(WNDCLASS));

    ENTERMONITOR;

#if (PDAAPI)
    /* PDA平台有自己的DefWindow。而控件使用缺省的DefWindow，具有CS_DEFWIN */
    if (wc.style & CS_DEFWIN)
        pClsDefWnd = CLSTBL_GetClassObj(DEFWINDOWCLASSNAME, NULL);
    else
        pClsDefWnd = CLSTBL_GetClassObj(PDADEFWINDOWCLASSNAME, NULL);
#else
    pClsDefWnd = CLSTBL_GetClassObj(DEFWINDOWCLASSNAME, NULL);
#endif

    if (pClsDefWnd != NULL)
        wc.cbWndExtra += pClsDefWnd->wc.cbWndExtra;

    pClsObj = CLSTBL_Insert(&wc);

    LEAVEMONITOR;

    if (!pClsObj)
    {
        SetLastError(1);
        return FALSE;
    }

    return TRUE;
}
#endif

BOOL WINAPI RegisterClassExBase(CONST WNDCLASSEX* lpwcx)
{
    WNDCLASS wc;
    PCLSOBJ pClsObj;

    if (!lpwcx)
    {
        SetLastError(1);
        return FALSE;
    }

    if (!lpwcx->lpfnWndProc || !lpwcx->lpszClassName)
    {
        SetLastError(1);
        return FALSE;
    }

    memcpy(&wc, (BYTE*)lpwcx + sizeof(UINT), sizeof(WNDCLASS));

    ENTERMONITOR;

    pClsObj = CLSTBL_Insert(&wc);

    LEAVEMONITOR;

    if (!pClsObj)
    {
        SetLastError(1);
        return FALSE;
    }

    return TRUE;
}

#if (!BASEWINDOW)
BOOL WINAPI RegisterClassEx(CONST WNDCLASSEX* lpwcx)
{
    WNDCLASS wc;
    PCLSOBJ pClsObj;
    PCLSOBJ pClsDefWnd;

    if (!lpwcx)
    {
        SetLastError(1);
        return FALSE;
    }

    if (!lpwcx->lpfnWndProc || !lpwcx->lpszClassName)
    {
        SetLastError(1);
        return FALSE;
    }

    memcpy(&wc, (BYTE*)lpwcx + sizeof(UINT), sizeof(WNDCLASS));

    ENTERMONITOR;

#if (PDAAPI)
    /* PDA平台有自己的DefWindow。而控件使用缺省的DefWindow，具有CS_DEFWIN */
    if (wc.style & CS_DEFWIN)
        pClsDefWnd = CLSTBL_GetClassObj(DEFWINDOWCLASSNAME, NULL);
    else
        pClsDefWnd = CLSTBL_GetClassObj(PDADEFWINDOWCLASSNAME, NULL);
#else
    pClsDefWnd = CLSTBL_GetClassObj(DEFWINDOWCLASSNAME, NULL);
#endif

    if (pClsDefWnd != NULL)
        wc.cbWndExtra += pClsDefWnd->wc.cbWndExtra;

    pClsObj = CLSTBL_Insert(&wc);

    LEAVEMONITOR;

    if (!pClsObj)
    {
        SetLastError(1);
        return FALSE;
    }

    return TRUE;
}
#endif

/*
**  Function : UnregisterClass
**  Purpose  :
**      Removes a window class, freeing the memory required for the 
**      class.
**  Params   :
**      pszClassName : Specifies the window class name. This class 
**                     name must have been registered by a previous
**                     call to the RegisterClass function.
**  Return   :
**      If the function succeeds, the return value is TRUE. If the 
**      class could not be found or if a window still exists that 
**      was created with the class, the return value is FALSE. To 
**      get extended error information, call GetLastError.
*/
BOOL WINAPI UnregisterClass(PCSTR pszClassName, HINSTANCE hInst)
{
    BOOL bRet;
    if (!pszClassName)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    bRet = CLSTBL_Delete(pszClassName, hInst);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : GetClassInfo
**  Purpose  :
**      Retrieves information about a window class.
**  Params   :
**      pClassName : Points to a null-terminated string containing
**                   the class name. The name must be that of a 
**                   preregistered class or a class registered by a
**                   previous call to the RegisterClass function. 
**      pWndClass  : Points to a WNDCLASS structure that receives 
**                   the information about the class.
**  Return   :
**      If the function succeeds, the return value is TRUE. If the 
**      class could not be found or if a window still exists that 
**      was created with the class, the return value is FALSE. To 
**      get extended error information, call GetLastError.
**  
*/
BOOL WINAPI GetClassInfo(HINSTANCE hInst, PCSTR pszClassName, 
                         PWNDCLASS pWndClass)
{
    PCLSOBJ pClsObj;

    if (!pszClassName || !pWndClass)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    pClsObj = CLSTBL_GetClassObj(pszClassName, hInst);

    LEAVEMONITOR;

    if (!pClsObj)
    {
        SetLastError(1);
        return FALSE;
    }

    return CLS_GetInfo(hInst, pClsObj, pWndClass);
}

/*
**  Function : GetClassName
**  Purpose  :
**      Retrieves the name of the class to which the specified 
**      window belongs.
**  Params   :
**      hWnd        : Identifies the window and, indirectly, the 
**                    class to which the window belongs. 
**      lpClassName : Points to the buffer that is to receive the 
**                    class name string. 
**      nMaxCount   : Specifies the length, in characters, of the 
**                    buffer pointed to by the lpClassName parameter. 
**                    The class name string is truncated if it is 
**                    longer than the buffer. 
**  Return   :
**      If the function succeeds, the return value is the number of 
**      characters copied to the specified buffer. If the function 
**      fails, the return value is zero. To get extended error 
**      information, call GetLastError.
*/
int WINAPI GetClassName(HWND hWnd, PSTR pszClassName, int nMaxCount)
{
    PWINOBJ pWin;
    int nRet;

    if (!pszClassName || nMaxCount < 1)
    {
        SetLastError(1);
        return 0;
    }

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return 0;
    }

    ENTERMONITOR;

    nRet = WND_GetClassName(pWin, pszClassName, nMaxCount);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : GetClassWord
**  Purpose  :
**      Retrieves the 16-bit (word) value at the specified offset into the
**      extra class memory for the window class to which the specified 
**      window belongs. 
**  Params   :
**      hWnd   : Identifies the window and, indirectly, the class 
**               to which the window belongs.
**      nIndex : Specifies the id of the item to be got.
**  Return   :
**      If the function succeeds, the return value is the item value. 
**      If the function fails, the return value is zero. To get extended 
**      error information, call GetLastError.
*/
WORD WINAPI GetClassWord(HWND hWnd, int nIndex)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);

    if (!pWin)
    {
        SetLastError(1);
        return 0;
    }

    return WND_GetClassWord(pWin, nIndex);
}

/*
**  Function : SetClassWord
**  Purpose  :
**      Replaces the 16-bit (word) value at the specified offset into the
**      extra class memory for the window class to which the specified 
**      window belongs. 
**  Params   :
**      hWnd   : Identifies the window and, indirectly, the class 
**               to which the window belongs.
**      nIndex : Specifies the id of the item to be got.
**  Return   :
**      If the function succeeds, return the previous value of the 
**      specified 16-bit integer. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
*/
WORD WINAPI SetClassWord(HWND hWnd, int nIndex, int wNewWord)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);

    if (!pWin)
    {
        SetLastError(1);
        return 0;
    }

    return WND_SetClassWord(pWin, nIndex, (WORD)wNewWord);
}

/*
**  Function : GetClassLong
**  Purpose  :
**      Retrieves the specified item value from the WNDCLASS 
**      structure associated with the specified window. 
**  Params   :
**      hWnd   : Identifies the window and, indirectly, the class 
**               to which the window belongs.
**      nIndex : Specifies the id of the item to be got.
**  Return   :
**      If the function succeeds, the return value is the item value. 
**      If the function fails, the return value is zero. To get extended 
**      error information, call GetLastError.
*/
DWORD WINAPI GetClassLong(HWND hWnd, int nIndex)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);

    if (!pWin)
    {
        SetLastError(1);
        return 0;
    }

    return WND_GetClassLong(pWin, nIndex);
}

/*
**  Function : SetClassLong
**  Purpose  :
**      Set the specified item value from the WNDCLASS structure 
**      associated with the specified window. 
**  Params   :
**      hWnd       : Identifies the window and, indirectly, the class 
**                   to which the window belongs.
**      nIndex    : Specifies the id of the item to be set.
**      dwNewValue : Specifies the new value to set.
**  Returns  :
**      If the function succeeds, the return value is the previous 
**      value.If the function fails, the return value is zero. To get 
**      extended error information, call GetLastError.
*/
DWORD WINAPI SetClassLong(HWND hWnd, int nIndex, DWORD dwNewValue)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);

    if (!pWin)
    {
        SetLastError(1);
        return 0;
    }

    return WND_SetClassLong(pWin, nIndex, dwNewValue);
}

/**************************************************************************/
/*              Window creation and destroy                               */
/**************************************************************************/

/*
**  Function : IsWindow 
**  Purpose  :
**      Determines whether the specified window handle identifies an 
**      existing window. 
**  Params   :
**      hWnd : Indentifies the window handle.
**  Return   :
**      If the window handle identifies an existing window, return nonzero. 
**      If the window handle does not identify an existing window, return 
**      zero. 
*/
BOOL WINAPI IsWindow(HWND hWnd)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);

    return (pWin != NULL);
}

/*
**  Function : CreateWindow
**  Purpose  :
**      Creates an overlapped, pop-up, or child window. It specifies 
**      the window class, window title, window style, and (optionally) 
**      the initial position and size of the window. The function also 
**      specifies the window's parent or owner, if any, and the window's 
**      menu. 
**  Params   :
**
**  Return   :
**      If the function succeeds, the return value is the handle to the
**      new window. If the function fails, the return value is NULL. To 
**      get extended error information, call GetLastError.
*/
HWND WINAPI CreateWindow(PCSTR pszClassName, PCSTR pszWindowName, 
                         DWORD dwStyle, int x, int y, int width, 
                         int height, HWND hwndParent, HMENU hMenu, 
                         HINSTANCE hInstance, PVOID pParam)
{
    return CreateWindowEx(0, pszClassName, pszWindowName, dwStyle, x, y,
        width, height, hwndParent, hMenu, hInstance, pParam);
}

/*
**  Function : CreateWindowEx
**  Purpose  :
**      Creates an overlapped, pop-up, or child window with an extended 
**      style; otherwise, this function is identical to the CreateWindow 
**      function. 
*/
void DEFWND_CalcMinSize(DWORD dwExStyle, DWORD dwStyle, HMENU hMenu, 
                        PSIZE pMinSize);
HWND WINAPI CreateWindowEx(DWORD dwExStyle, PCSTR pszClassName, 
                           PCSTR pszWindowName, DWORD dwStyle, int x, 
                           int y, int width, int height, HWND hwndParent, 
                           HMENU hMenu, HINSTANCE hInstance, PVOID pParam)
{
    PWINOBJ pWin;
    SIZE size;

    if (!pszClassName)
    {
        SetLastError(1);
        return NULL;
    }

    // 如果是创建button, combobox, menu, scrollbar, static这些窗口类的窗
    // 口, 清除WS_HSCROLL, WS_VSCROLL风格, 因为滚动条对这些类的窗口没有意
    // 义. 在这里清除不是一个好的方法, 应在这些窗口类的窗口函数中负责清除, 
    // 这里为了方便暂时这样做. MS Windows中只自动清除combobox窗口的相应风
    // 格, 具体原因不详.
    if (!stricmp(pszClassName, "button") || 
        !stricmp(pszClassName, "combobox") || 
        !stricmp(pszClassName, "menu") || 
        !stricmp(pszClassName, "scrollbar") || 
        !stricmp(pszClassName, "static"))
    {
        dwStyle &= ~(WS_HSCROLL | WS_VSCROLL);
    }

    // 根据窗口的风格和扩展风格计算窗口的最小宽度和最小高度, 若窗口指定的
    // 宽度或高度小于计算出的窗口的最小宽度和最小高度, 取窗口的最小宽度和
    // 最小高度为窗口的宽度和高度.
    if (stricmp(pszClassName, "#MENU"))
    {
        DEFWND_CalcMinSize(dwExStyle, dwStyle, hMenu, &size);
        if (width < size.cx)
            width = size.cx;
        if (height < size.cy)
            height = size.cy;
    }

    // 创建窗口, 成功返回窗口的句柄, 失败返回NULL.

    ENTERMONITOR;

    pWin = WND_Create(dwExStyle, pszClassName, pszWindowName, dwStyle, 
        x, y, width, height, hwndParent, hMenu, hInstance, pParam);

    LEAVEMONITOR;

    if (!pWin)
        return NULL;

    return (HWND)WOT_GetHandle((PWSOBJ)pWin);
}

/*
**  Function : DestroyWindow
**  Purpose  :
**      Destroys the specified window. The function sends WM_DESTROY and 
**      WM_NCDESTROY messages to the window to deactivate it and remove 
**      the keyboard focus from it. The function also destroys the window's 
**      menu, destroys timers. 
**      If the specified window is a parent or owner window, DestroyWindow 
**      automatically destroys the associated child or owned windows when 
**      it destroys the parent or owner window. The function first destroys 
**      child or owned windows, and then it destroys the parent or owner 
**      window. 
**  Params   :
**      hWnd : Specifies the window to be destroyed.
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended 
**      error information, call GetLastError. 
*/
BOOL DestroyWindow(HWND hWnd)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    // Destroy the window
    if (!ISROOTWIN(pWin))
    {
        ENTERMONITOR;
        
        if (WND_IsInSameThread(pWin))
            WND_Destroy(pWin);

        LEAVEMONITOR;
    }

    return TRUE;
}

/**************************************************************************/
/*              Basic window attributes                                   */ 
/**************************************************************************/

/*
**  Function : IsChild
**  Purpose  :
**      Tests whether a window is a child window or descendant window of a
**      specified parent window. A child window is the direct descendant of
**      a specified parent window if that parent window is in the chain of
**      parent windows; the chain of parent windows leads from the original
**      pop-up window to the child window. 
**  Params   :
**      hwndParent : Identifies the parent window. 
**      hWnd       : Identifies the window to be tested. 
**  Return   :
**      If the window is a child or descendant window of the specified 
**      parent window, return TRUE. 
**      If the window is not a child or descendant window of the specified
**      parent window, return FALSE. 
*/
BOOL WINAPI IsChild(HWND hwndParent, HWND hWnd)
{
    PWINOBJ pParentWin, pWin;
    BOOL bRet;

    if (hwndParent == hWnd)
        return FALSE;

    pParentWin = (PWINOBJ)WOT_GetObj((HANDLE)hwndParent, OBJ_WINDOW);
    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);

    if (!pParentWin || !pWin)
        return FALSE;

    ENTERMONITOR;

    bRet = WND_IsDescendant(pWin, pParentWin);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : GetParent
**  Purpsoe  :
**      Retrieves the handle of the specified child window's parent 
**      window.
**  Params   :
**      hWnd : Identifies the window whose parent window handle is 
**             to be retrieved. 
**  Return   :
**      If the function succeeds, the return value is the handle of 
**      the parent window. If the window has no parent window, the 
**      return value is NULL. To get extended error information, 
**      call GetLastError. 
*/
HWND WINAPI GetParent(HWND hWnd)
{
    PWINOBJ pWin, pParent;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pParent = WND_GetParent(pWin);

    LEAVEMONITOR;

    if (!pParent)
        return NULL;

    return (HWND)WOT_GetHandle((PWSOBJ)pParent);
}

/*
**  Function : GetWndExtraDataPtrBase
**  Purpose  : Get the window extra data that specialed by the user. The
                window is a kernel window object.
**  Params   :
**  Return   : The pointer to the extra data.
*/
void* WINAPI GetWindowExtraDataPtrBase(HWND hWnd)
{
    PWINOBJ pWin;
    void* pRet;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pRet = WND_GetExtraDataPtrBase(pWin);

    LEAVEMONITOR;

    return pRet;
}

#if (!BASEWINDOW)
/*
**  Function : GetWndExtraDataPtr
**  Purpose  : Get the window extra data that specialed by the user. The 
                window is a general window object.
**  Params   :
**  Return   : The pointer to the extra data.
*/
void* WINAPI GetWindowExtraDataPtr(HWND hWnd)
{
    PWINOBJ pWin;
    void* pRet;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pRet = WND_GetExtraDataPtr(pWin);

    LEAVEMONITOR;

    return pRet;
}
#endif

/*
**  Function : IsWindowVisible
**  Purpose  :
**      Retrieves the visibility state of the specified window. 
**  Params   :
**      hWnd : Indentifies the window handle.
**  Return   :
**      If the specified window and its parent window have the WS_VISIBLE 
**      style, return nonzero. If the specified window and its parent 
**      window do not have the WS_VISIBLE style, return zero. Because the 
**      return value specifies whether the window has the WS_VISIBLE style, 
**      it may be nonzero even if the window is totally obscured by other 
**      windows. 
*/
BOOL WINAPI IsWindowVisible(HWND hWnd)
{
    PWINOBJ pWin;
    BOOL bRet;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    bRet = WND_IsVisible(pWin);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : ShowWindow
**  Purpose  :
**      sets the specified window's show state. 
**  Params   :
**      hWnd  : Identifies the window. 
**      nShow : Specifies how the window is to be shown.   
*/
BOOL WINAPI ShowWindow(HWND hWnd, int nCmdShow)
{
    PWINOBJ pWin;
    BOOL bRet;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    bRet = WND_Show(pWin, nCmdShow);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : ShowOwnedPopups
**  Purpose  :
**      Shows or hides all pop-up windows owned by the specified window. 
**  Params   :
**      hWnd  : Indentifies the window that owns the popup window to be
**              shown or hidden.
**      bShow : Specifies popup windows are to be shown hidden. If this 
**              parameter is TRUE, all hidden popup windows are shown. If 
**              this parameter is FALSE, all hidden popup windows are 
**              hidden.
**  Return   :
**      If the function succeeds, return TRUE;
**      If the function fails, return FALSE. To get extended error 
**      infomation, call GetLastError.
**  Remarks
**      ShowOwnedPopups shows only windows hidden by a previous call to 
**      ShowOwnedPopups. For example, if a pop-up window is hidden by 
**      using the ShowWindow function, subsequently calling ShowOwnedPopups
**      with the fShow parameter set to TRUE does not cause the window to 
**      be shown. 
*/
BOOL WINAPI ShowOwnedPopups(HWND hWnd, BOOL bShow)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);

    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    if (bShow)
        pWin = WND_MapOwnedPopups(pWin);
    else
        WND_UnmapOwnedPopups(pWin);

    INPUTDEV_SetActiveWindow(pWin, WA_ACTIVE, TRUE);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : EnableWindow
**  Purpose  :
**      Enables or disables mouse and keyboard input to the specified 
**      window or control. When input is disabled, the window does not 
**      receive input such as mouse clicks and key presses. When input 
**      is enabled, the window receives all input. 
**  Params   :
**      hWnd    : Identifies the window to be enabled or disabled. 
**      bEnable : Specifies whether to enable or disable the window. If 
**                this parameter is TRUE, the window is enabled. If the 
**                parameter is FALSE, the window is disabled. 
**  Return   :
**      If the window was previously disabled, return nonzero. 
**      If the window was not previously disabled, return zero. To get 
**      extended error information, call GetLastError. 
*/
BOOL WINAPI EnableWindow(HWND hWnd, BOOL bEnable)
{
    PWINOBJ pWin;
    BOOL bRet;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);

    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    bRet = WND_Enable(pWin, bEnable);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : IsWindowEnabled
**  Purpsoe  :
**      Determines whether the specified window is enabled for mouse and 
**      keyboard input.
**  Params   :
**      hWnd : Identifies the window to test. 
**  Return   :
**      If the window is enabled, the return value is nonzero. 
**      If the window is not enabled, the return value is zero. 
*/
BOOL WINAPI IsWindowEnabled(HWND hWnd)
{
    PWINOBJ pWin;
    BOOL bRet;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    bRet = WND_IsEnabled(pWin);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : SetWindowText
**  Purpose  :
**      Changes the text of the specified window’s title bar (if it has 
**      one). If the specified window is a control, the text of the 
**      control is changed. 
**  Params   :
**      hWnd    : Identifies the window whose text is to be changed. 
**      pszText : Points to a null-terminated string to be used as the new 
**                title or control text. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended 
**      error information, call GetLastError. 
*/
BOOL WINAPI SetWindowText(HWND hWnd, PCSTR pszText)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WND_SendMessage(pWin, WM_SETTEXT, 0, (LPARAM)pszText);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : GetWindowText
**  Purpose  :
**      Copies the text of the specified window’s title bar (if it has one) 
**      into a buffer. If the specified window is a control, the text of the 
**      control is copied. 
**  Params   :
**      hWnd      : Identifies the window or control containing the text. 
**      pszText   : Points to the buffer that will receive the text. 
**      nMaxCount : Specifies the maximum number of characters to copy to
**                  the buffer, including the NULL character. If the text 
**                  exceeds this limit, it is truncated. 
**  Return   :
**      If the function succeeds, return the length, in characters, of the 
**      copied string, not including the terminating null character. If the 
**      window has no title bar or text, if the title bar is empty, or if 
**      the window or control handle is invalid, return zero. To get 
**      extended error information, call GetLastError. 
*/
int WINAPI GetWindowText(HWND hWnd, PSTR pszText, int nMaxCount)
{
    PWINOBJ pWin;
    int nRet;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return 0;
    }

    // 这里不判断参数pszText和nMaxCount的有效性, 窗口在响应WM_GETTEXT消息时
    // 必须判断

    ENTERMONITOR;

    nRet = WND_SendMessage(pWin, WM_GETTEXT, (WPARAM)nMaxCount, 
        (LPARAM)pszText);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : GetWindowTextLength
**  Purpose  :
**      Retrieves the length, in characters, of the specified window’s 
**      title bar text (if the window has a title bar). If the specified 
**      window is a control, the function retrieves the length of the 
**      text within the control. 
**  Params   :
**      hWnd : Identifies the window or control. 
**  Return   :
**      If the function succeeds, return the length, in characters, of the
**      text. Under certain conditions, this value may actually be greater
**      than the length of the text. For more information, see the 
**      following Remarks section. 
**      If the window has no text, return zero. To get extended error 
**  information, call GetLastError. 
*/
int WINAPI GetWindowTextLength(HWND hWnd)
{
    PWINOBJ pWin;
    int nRet;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    nRet = WND_SendMessage(pWin, WM_GETTEXTLENGTH, 0, 0);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : GetWindowWord
**  Purpose  :
**      Retrieves a 16-bit (word) value at the specified offset into the 
**      extra window memory for the specified window. 
**  Params   :
**      hWnd   : Identifies the window and, indirectly, the class to which 
**               the window belongs. 
**      nIndex : Specifies the zero-based offset to the value to be 
**               retrieved. 
**  Return   :
**      If the function succeeds, return the requested 32-bit value. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
**  Remarks  :
**      Reserve extra window memory by specifying a nonzero value in the
**      cbWndExtra member of the WNDCLASS structure used with the 
**      RegisterClass function. 
*/
WORD WINAPI GetWindowWord(HWND hWnd, int nIndex)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return 0;
    }

    return WND_GetWord(pWin, nIndex);
}

/*
**  Function : SetWindowWord
**  Purpose  :
**      Replaces the 16-bit (word) value at the specified offset into the
**      extra window memory for the specified window. 
**  Params   :
**      hWnd      : Identifies the window and, indirectly, the class to 
**                  which the window belongs. 
**      nIndex    : Specifies the item index to the value to be set. Valid 
**                  values are in the range zero through the number of 
**                  bytes of window memory, minus two; for example, if you
**                  specified 10 or more bytes of extra window memory, a 
**                  value of 8 would be an index to the fifth 16-bit 
**                  integer. 
**      wNewValue : Specifies the replacement value. 
**  Return   :
**      If the function succeeds, return the previous value of the 
**      specified 32-bit integer. If the function fails, return zero. To 
**      get extended error information, call GetLastError. 
**  Remarks  :
**      Reserve extra window memory by specifying a nonzero value in the
**      cbWndExtra member of the WNDCLASS structure used with the 
**      RegisterClass function. 
*/
WORD WINAPI SetWindowWord(HWND hWnd, int nIndex, int wNewValue)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    return WND_SetWord(pWin, nIndex, (WORD)wNewValue);
}

/*
**  Function : GetWindowLong
**  Purpose  :
**      Retrieves information about the specified window. The function 
**      also retrieves the 32-bit (long) value at the specified offset 
**      into the extra window memory of a window. 
**  Params   :
**      hWnd   : Identifies the window and, indirectly, the class to which 
**               the window belongs. 
**      nIndex : Specifies the zero-based offset to the value to be 
**               retrieved. 
**  Return   :
**      If the function succeeds, return the requested 32-bit value. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
*/
LONG WINAPI GetWindowLong(HWND hWnd, int nIndex)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return 0;
    }

    return WND_GetLong(pWin, nIndex);
}

/*
**  Function : SetWindowLong
**  Purpose  :
**      Changes an attribute of the specified window. The function also 
**      sets a 32-bit (long) value at the specified offset into the extra 
**      window memory of a window. 
**  Params   :
**      hWnd      : Identifies the window and, indirectly, the class to 
**                  which the window belongs. 
**      nIndex    : Specifies the item index to the value to be set. 
**      lNewValue : Specifies the replacement value. 
**  Return   :
**      If the function succeeds, return the previous value of the 
**      specified 32-bit integer. If the function fails, return zero. To 
**      get extended error information, call GetLastError. 
*/
LONG WINAPI SetWindowLong(HWND hWnd, int nIndex, LONG lNewValue)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    return WND_SetLong(pWin, nIndex, lNewValue);
}

/**************************************************************************/
/*              Window size, position, Z-order, and visibility            */ 
/**************************************************************************/

/*
**  Function : GetClientRect
**
**  Purpose  :
**      Retrieves the coordinates of a window's client area. The 
**      client coordinates specify the upper-left and lower-right
**      corners of the client area. Because client coordinates are 
**      relative to the upper-left corner of a window's client area, 
**      the coordinates of the upper-left corner are (0,0). 
**
**  Params   :
**      hWnd    : Indentifies the window whose client coordinates 
**                are to be retrieved.
**      pRect   : Points to a RECT structure that receives the client
**                coordinates. The left and top members are zero. The
**                right and bottom members contain the width and 
**                height of the window. 
**  Return   :
**      If the function succeeds, the return value is TRUE.
**      If the function fails, the return value is FALSE. To get 
**      extended error information, call GetLastError. 
*/
BOOL WINAPI GetClientRect(HWND hWnd, PRECT pRect)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WND_GetRect(pWin, pRect, W_CLIENT, XY_CLIENT);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : GetWindowRect
**
**  Purpose  :
**      Retrieves the dimensions of the bounding rectangle of the 
**      specified window. The dimensions are given in screen 
**      coordinates that are relative to the upper-left corner of 
**      the screen. 
**
**  Params   :
**      hwnd    : Indentifies the window.
**      pRect   : Points to a RECT struct that retrieves the screen
**                coordinates of the upper-left and lower-right 
**                corners of the window.
**  Return   :
**      If the function succeeds, the return value is TRUE.
**      If the function fails, the return value is FALSE. To get the
**      extended error infomation, call GetLastError.
*/
BOOL WINAPI GetWindowRect(HWND hWnd, PRECT pRect)
{
    PWINOBJ pWin;

    if (!pRect)
    {
        SetLastError(1);
        return FALSE;
    }

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WND_GetRect(pWin, pRect, W_WINDOW, XY_SCREEN);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : GetWindowRectEx
**  Purpose  :
**      Get position and size of a window.
*/
BOOL WINAPI GetWindowRectEx(HWND hWnd, PRECT pRect, int nPart, int nXYMode)
{
    PWINOBJ pWin;

    if (!pRect)
    {
        SetLastError(1);
        return FALSE;
    }

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WND_GetRect(pWin, pRect, nPart, nXYMode);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : MoveWindow
**  Purpose  :
**      Changes the position and dimensions of the specified window. For
**      a top-level window, the position and dimensions are relative to 
**      the upper-left corner of the screen. For a child window, they are 
**      relative to the upper-left corner of the parent window's client 
**      area. 
**  Params  :
**      hWnd    : Identifies the window. 
**      x       : Specifies the new position of the left side of the window. 
**      y       : Specifies the new position of the top of the window. 
**      nWidth  : Specifies the new width of the window. 
**      nHeight : Specifies the new height of the window. 
**      bRepaint: Specifies whether the window is to be repainted. If this 
**                parameter is TRUE, the window receives a WM_PAINT message. 
**                If the parameter is FALSE, no repainting of any kind occurs. 
**                This applies to the client area, the nonclient area 
**                (including the title bar and scroll bars), and any part of 
**                the parent window uncovered as a result of moving a child 
**                window. If this parameter is FALSE, the application must 
**                explicitly invalidate or redraw any parts of the window and 
**                parent window that need redrawing. 
**  Return  :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. 
*/
BOOL WINAPI MoveWindow(HWND hWnd, int x, int y, int width, int height, 
                       BOOL bRepaint)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WND_Move(pWin, x, y, width, height, bRepaint);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : BringWindowToTop
**  Purpose  :
**      Move a window to top.
**  Params:
**      hWnd : Identifies the window to be move to top.
**  Return  :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. 
*/
BOOL BringWindowToTop(HWND hWnd)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    if (ISCHILD(pWin))
        WND_MoveToTop(pWin);
    else    // Sets the window to be the active window        
        INPUTDEV_SetActiveWindow(pWin, WA_ACTIVE, TRUE);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : CallWindowProc
**  Purpose  :
**      Passes message information to the specified window procedure. 
**  Return   :
**      Return the result of the message processing and depends on the 
**      message sent. 
*/
LRESULT WINAPI CallWindowProc(WNDPROC pPrevWndFunc, HWND hWnd, 
                              UINT uMsgCmd, WPARAM wParam, LPARAM lParam)
{
    if (!pPrevWndFunc)
    {
        SetLastError(1);
        return 0;
    }

    return pPrevWndFunc(hWnd, uMsgCmd, wParam, lParam);
}


/**************************************************************************/
/*              Window coordinate mapping and hit-testing                 */
/**************************************************************************/

/*
**  Function : ClientToScreen
**  Purpose  :
**      Converts the client coordinates of a specified point to screen 
**      coordinates. 
**  Params   :
**      hWnd   : Identifies the window whose client area is used for the 
**               conversion. 
**      pPoint : Points to a POINT structure that contains the client 
**               coordinates to be converted. The new screen coordinates 
**               are copied into this structure if the function succeeds. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. 
*/
BOOL WINAPI ClientToScreen(HWND hWnd, PPOINT pPoint)
{
    PWINOBJ pWin;
    RECT rect;

    if (!pPoint)
    {
        SetLastError(1);
        return FALSE;
    }

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WND_GetRect(pWin, &rect, W_CLIENT, XY_SCREEN);

    LEAVEMONITOR;

    pPoint->x += rect.left;
    pPoint->y += rect.top;

    return TRUE;
}

/*
**  Function : ScreenToClient
**  Purpose  :
**      Converts the screen coordinates of a specified point to client 
**      coordinates. 
**  Params   :
**      hWnd   : Identifies the window whose client area is used for the 
**               conversion. 
**      pPoint : Points to a POINT structure that contains the screen 
**               coordinates to be converted. The new client coordinates 
**               are copied into this structure if the function succeeds. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. 
*/
BOOL WINAPI ScreenToClient(HWND hWnd, PPOINT pPoint)
{
    PWINOBJ pWin;
    RECT rect;

    if (!pPoint)
    {
        SetLastError(1);
        return FALSE;
    }

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WND_GetRect(pWin, &rect, W_CLIENT, XY_SCREEN);

    LEAVEMONITOR;

    pPoint->x -= rect.left;
    pPoint->y -= rect.top;

    return TRUE;
}

/*
**  Function : WindowFromPoint
**  Purpsoe  :
**      Retrieves the handle of the window that contains the specified 
**      point. 
**  Params   :
**      pt   : Specifies a POINT structure that defines the point to be 
**             checked. 
**  Return   :
**      If the function succeeds, return the handle of the window that 
**      contains the point. If no window exists at the given point, return
**      NULL. 
*/
HWND WINAPI WindowFromPoint(POINT pt)
{
    PWINOBJ pWin;

    ENTERMONITOR;

    pWin = WND_FromPoint(pt.x, pt.y);

    LEAVEMONITOR;

    if (!pWin)
        return NULL;

    return (HWND)WOT_GetHandle((PWSOBJ)pWin);
}

/*
**  Function : ChildWindowFromPoint
**  Purpose  :
**      Determines which, if any, of the child windows belonging to a 
**      parent window contains the specified point. 
**  Params   :
**      hwndParent : Identifies the parent window. 
**      pt         : Specifies a POINT structure that defines the client 
**                   coordinates of the point to be checked. 
**  Return   :
**      If the function succeeds, return the handle of the child window 
**      that contains the point, even if the child window is hidden or 
**      disabled. If the point lies outside the parent window, return 
**      NULL. If the point is within the parent window but not within 
**      any child window, the return value is the handle to the parent 
**      window. 
**  Remarks
**      Windows maintains an internal list, containing the handles of 
**      the child windows associated with a parent window. The order of
**      the handles in the list depends on the Z order of the child 
**      windows. If more than one child window contains the specified 
**      point, Windows returns the handle of the first window in the 
**      list that contains the point. 
*/
HWND WINAPI ChildWindowFromPoint(HWND hwndParent, POINT pt)
{
    return NULL;
}

/**************************************************************************/
/*              Window query and enumeration                              */
/**************************************************************************/

//static HWND hwndDesktop = HWND_DESKTOP;
static PWINOBJ pDesktop = NULL;

/*
**  Function : RegisterDesktopWindow
**  Purpose  :
**      Register the desktop window.  The desktop window covers the entire
**      screen. The desktop window is the area on top of which all icons 
**      and other windows are painted. 
**  Params   :
**      hWnd : Indentifies the desktop window.
**  Return   :
**      If the function succeeds, return TRUE;
**      If the function fails, return FALSE;    
*/
BOOL WINAPI RegisterDesktopWindow(HWND hWnd)
{
    PWINOBJ pWin;
    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (pWin == NULL)
    {
        SetLastError(1);
        return FALSE;
    }

    pDesktop = pWin;
    return TRUE;
}

/*
**  Function : GetDesktopWindow
**  Purpose  :
**      Returns the handle of the Windows desktop window. The desktop 
**      window covers the entire screen. The desktop window is the area
**      on top of which all icons and other windows are painted. 
**  Params   :
**      None.
**  Return   :
**      Return the handle of the desktop window. 
*/
HWND WINAPI GetDesktopWindow(void)
{
    if (pDesktop == NULL)
        pDesktop = g_pRootWin;
    
    return (HWND)WOT_GetHandle((PWSOBJ)pDesktop);
//    return hwndDesktop;
}

/*
**  Function : FindWindow
**  Purpose  :
**      Retrieves the handle to the top-level window whose class name and 
**      window name match the specified strings. This function does not 
**      search child windows. 
**  Params   :
**      pszClassName  : Points to a null-terminated string that specifies
**                      the class name 
**      pszWindowName : Points to a null-terminated string that specifies
**                      the window name (the window’s title). If this 
**                      parameter is NULL, all window names match. 
**  Return   :
**      If the function succeeds, return the handle to the window that has
**      the specified class name and window name. If the funciton fails, 
**      return NULL;
*/
HWND WINAPI FindWindow(PCSTR pszClassName, PCSTR pszWindowName)
{
    PWINOBJ pWin;

    if (!pszClassName)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pWin = WND_FindWindow(pszClassName, pszWindowName);

    LEAVEMONITOR;

    if (!pWin)
        return NULL;

    return (HWND)WOT_GetHandle((PWSOBJ)pWin);
}

/*
**  Function : EnumWindows
**  Purpose  :
**      Enumerates all top-level windows on the screen by passing the 
**      handle of each window, in turn, to an application-defined callback
**      function. EnumWindows continues until the last top-level window is
**      enumerated or the callback function returns FALSE. 
**  Params   :
**      pEnumFunc : Points to an application-defined callback function. 
**      lParam    : Specifies a 32-bit, application-defined value to be 
**                  passed to the callback function. 
**  Return   :
**      If the function succeeds, return TRUE.
**      If the function fails, return FALSE.
**  Remarks  :
**      The EnumWindows function does not enumerate child windows. 
**      This function is more reliable than calling the GetWindow function
**      in a loop. An application that calls GetWindow to perform this 
**      task risks being caught in an infinite loop or referencing a handle
**      to a window that has been destroyed. 
*/
BOOL WINAPI EnumWindows(WNDENUMPROC pEnumFunc, LPARAM lParam)
{
    HWND *pHwnd;
    HWND HwndList[128];
    int nHwnd;
    BOOL bRet;

    if (!pEnumFunc)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    nHwnd = WND_MakeHwndList(NULL, 0);
    if (nHwnd <= 0)
    {
        ASSERT(0);
        LEAVEMONITOR;
        return FALSE;
    }

    if (nHwnd <= 128)
    {
        pHwnd = HwndList;
    }
    else
    {
        pHwnd = MemAlloc(nHwnd * sizeof(HWND));
        if (pHwnd == NULL)
        {
            LEAVEMONITOR;
            return FALSE;
        }
    }

    nHwnd = WND_MakeHwndList(pHwnd, nHwnd);
    ASSERT(nHwnd > 0);
    while (nHwnd --)
    {
        if (IsWindow(*pHwnd))
        {
            bRet = pEnumFunc(*pHwnd, lParam);
            if (!bRet)
                break;
        }
    }

    if (pHwnd != HwndList)
        MemFree(pHwnd);

//    WND_EnumWindows(pEnumFunc, lParam);
    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : EnumChildWindows
**  Purpose  :
**      Enumerates the child windows that belong to the specified parent 
**      window by passing the handle of each child window, in turn, to an 
**      application-defined callback function. EnumChildWindows continues
**      until the last child window is enumerated or the callback function
**      returns FALSE. 
**  Params   :
**      hwndParent  : Identifies the parent window whose child windows are
**                    to be enumerated. 
**      pEnumFunc   : Points to an application-defined callback function. 
**      lParam      : Specifies a 32-bit, application-defined value to be
**                    passed to the callback function. 
**  Return   :
**      If the function succeeds, return TRUE.
**      If the function fails, return FALSE.
**  Remarks  :
**      The EnumChildWindows function does not enumerate top-level windows
**      owned by the specified window, nor does it enumerate any other 
**      owned windows. 
**      If a child window has created child windows of its own, this 
**      function enumerates those windows as well. 
**      A child window that is moved or repositioned in the Z order during
**      the enumeration process will be properly enumerated. The function
**      does not enumerate a child window that is destroyed before being 
**      enumerated or that is created during the enumeration process. 
**      This function is more reliable than calling the GetWindow function
**      in a loop. An application that calls GetWindow to perform this 
**      task risks being caught in an infinite loop or referencing a handle
**      to a window that has been destroyed. 
*/
BOOL WINAPI EnumChildWindows(HWND hwndParent, WNDENUMPROC pEnumFunc, 
                             LPARAM lParam)
{
    PWINOBJ pWin;

    if (!pEnumFunc)
    {
        SetLastError(1);
        return FALSE;
    }

    // If the parent window is NULL, call EnumWindows to enumrate all 
    // top-level windows
    if (!hwndParent)
        return EnumWindows(pEnumFunc, lParam);

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hwndParent, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WND_EnumChildWindows(pWin, pEnumFunc, lParam);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : GetWindow
**  Purpose  :
**      Retrieves the handle of a window that has the specified 
**      relationship (Z order or owner) to the specified window. 
**  Params   :
**      hWnd : Identifies a window. The window handle retrieved is relative
**             to this window, based on the value of the uCmd parameter. 
**      uCmd : Specifies the relationship between the specified window and
**             the window whose handle is to be retrieved. This parameter 
**             can be one of the following values: 
**                  GW_CHILD, GW_HWNDFIRST, GW_HWNDLAST, GW_HWNDNEXT, 
**                  GW_HWNDPREV, GW_OWNED
**  Return   :
**      If the function succeeds, return a window handle. If no window 
**      exists with the specified relationship to the specified window, 
**      return NULL. To get extended error information, call GetLastError. 
*/
HWND WINAPI GetWindow(HWND hWnd, UINT uCmd)
{
    PWINOBJ pWin;

    if (uCmd > GW_MAX)
    {
        SetLastError(1);
        return FALSE;
    }

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pWin = WND_GetWindow(pWin, uCmd);

    LEAVEMONITOR;

    if (!pWin)
        return NULL;

    return (HWND)WOT_GetHandle((PWSOBJ)pWin);
}

/**************************************************************************/
/*              Window drawing support                                    */
/**************************************************************************/

/*
**  Function : GetDC
**  Purpose  :
**      Retrieves a handle of a display device context (DC) for the client 
**      area of the specified window. The display device context can be used 
**      in subsequent GDI functions to draw in the client area of the window. 
**      This function retrieves a common, class, or private device context 
**      depending on the class style specified for the specified window. 
**      For common device contexts, GetDC assigns default attributes to the 
**      device context each time it is retrieved. For class and private device 
**      contexts, GetDC leaves the previously assigned attributes unchanged. 
**  Params   :
**      hWnd : Identifies the window whose device context is to be retrieved. 
**  Return   :
**      If the function succeeds, return the device context for the given
**      window's client area. 
**      If the function fails, the return value is NULL. 
**  Remark   :
**      After painting with a common device context, the ReleaseDC function 
**      must be called to release the device context. Class and private device 
**      contexts do not have to be released. The number of device contexts is 
**      limited only by available memory.
*/
HDC WINAPI GetDC(HWND hWnd)
{
    PWINOBJ pWin;
    PDC pDC;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);

    if (!pWin)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pDC = WND_GetClientDC(pWin);

    LEAVEMONITOR;

    if (!pDC)
        return NULL;

    return (HDC)WOT_GetHandle((PGDIOBJ)pDC);
}

/*
**  Function : ReleaseDC
**  Purpose  :
**      Releases a device context (DC), freeing it for use by other 
**      applications. The effect of the ReleaseDC function depends on 
**      the type of device context. It frees only common and window 
**      device contexts. It has no effect on class or private device 
**      contexts.
**  Params   :
**      hWnd : Identifies the window whose device context is to be 
**             released. 
**      hdc  : Identifies the device context to be released. 
**  Return   : 
**      The return value specifies whether the device context is released. 
**      If the device context is released, the return value is 1. 
**      If the device context is not released, the return value is zero.
**  Remark   :
**      The application must call the ReleaseDC function for each call to 
**      the GetWindowDC function and for each call to the GetDC function 
**      that retrieves a common device context. 
*/
int WINAPI ReleaseDC(HWND hWnd, HDC hdc)
{
    PWINOBJ pWin;
    PDC pDC;
    int nRet;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);

    if (!pWin)
    {
        SetLastError(1);
        return 0;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return 0;
    }

    ENTERMONITOR;

    nRet = WND_ReleaseDC(pWin, pDC);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : GetWindowDC
**  Purpose  :
**      Retrieves the device context (DC) for the entire window, including 
**      title bar, menus, and scroll bars. A window device context permits 
**      painting anywhere in a window, because the origin of the device 
**      context is the upper-left corner of the window instead of the client 
**      area. 
**  Params   :
**      hWnd : Identifies the window with a device context that is to be 
**             retrieved. 
**  Return   :
**      If the function succeeds, return the handle of a device context for 
**      the specified window. 
**      If the function fails, return NULL, indicating an error or an 
**      invalid hWnd parameter. 
**  Remarks  :
**      GetWindowDC is intended for special painting effects within a 
**      window's nonclient area. After painting is complete, the ReleaseDC 
**      function must be called to release the device context. Not releasing 
**      the window device context has serious effects on painting requested 
**      by applications. 
*/
HDC WINAPI GetWindowDC(HWND hWnd)
{
    PWINOBJ pWin;
    PDC pDC;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);

    if (!pWin)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pDC = WND_GetWindowDC(pWin);

    LEAVEMONITOR;

    if (!pDC)
    {
        SetLastError(1);
        return NULL;
    }

    return (HDC)WOT_GetHandle((PGDIOBJ)pDC);
}

/**************************************************************************/
/*              Window repainting                                         */
/**************************************************************************/

/*
**  Function : BeginPaint
**  Purpose  :
**      Prepares the specified window for painting and fills a PAINTSTRUCT 
**      structure with information about the painting. 
**  Params   :
**      hWnd         : Identifies the window to be repainted. 
**      pPaintStruct : Pointer to the PAINTSTRUCT structure that will 
**                     receive painting information.
**  Return   :
**      If the function succeeds, return the handle to a display device 
**      context (DC) for the specified window. If the function fails, the 
**      return value is NULL, indicating that no display DC is available. 
*/
HDC WINAPI BeginPaint(HWND hWnd, PPAINTSTRUCT pPaintStruct)
{
    PWINOBJ pWin;
    PDC pDC;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pDC = WND_BeginPaint(pWin, pPaintStruct);

    LEAVEMONITOR;

    ASSERT(pDC != NULL);

    return (HDC)WOT_GetHandle((PGDIOBJ)pDC);
}

/*
**  Function : EndPaint
**  Purpose  :
**      Marks the end of painting in the specified window. This function
**      is required for each call to the BeginPaint function, but only 
**      after painting is complete. 
**  Params   :
**      hWnd         : Identifies the window that has been repainted. 
**      pPaintStruct : Points to a PAINTSTRUCT structure that contains the
**                     painting information retrieved by BeginPaint. 
**  Return   :
**      If the function succeeds, return TRUE. 
**      If the funciton fails, return FALSE.
**  Remark   :
**      If the caret was hidden by BeginPaint, EndPaint restores the caret
**      to the screen. 
*/
BOOL WINAPI EndPaint(HWND hWnd, const PAINTSTRUCT* pPaintStruct)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);

    if (!pWin)
        return FALSE;

    ENTERMONITOR;

    WND_EndPaint(pWin, pPaintStruct);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Funciton : UpdateWindow
**  Purpose  :
**      Updates the client area of the specified window by sending a 
**      WM_PAINT message to the window if the window’s update region 
**      is not empty. 
**  Params   :
**      hWnd : Identifies the window to be updated. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. 
*/
BOOL WINAPI UpdateWindow(HWND hWnd)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WND_UpdateTree(pWin);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : InvalidateRect
**  Purpose  :
**      Adds a rectangle to the specified window's update rectangle.
**      The update region represents the portion of the window’s client 
**      area that must be redrawn. 
**  Params   :
**      hWnd   : Identifies the window whose update region has changed. 
**      pRect  : Points to a RECT structure that contains the client 
**               coordinates of the rectangle to be added to the update 
**               region. If this parameter is NULL, the entire client area
**               is added to the update region. 
**      bErase : Specifies whether the background within the update region
**               is to be erased when the update region is processed. If 
**               this parameter is TRUE, the background is erased when the
**               BeginPaint function is called. If this parameter is FALSE,
**               the background remains unchanged. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. 
*/
BOOL WINAPI InvalidateRect(HWND hWnd, const RECT* pRect, BOOL bErase)
{
    PWINOBJ pWin;
    RECT rcClient, rect;
    BOOL bInvalidateAll = FALSE;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        pWin = g_pRootWin;
        bInvalidateAll = TRUE;
    }

    // The rectangle to be invalidated is empty, just return
    if (IsRectEmpty(pRect))
        return TRUE;

    ENTERMONITOR;

    if (bInvalidateAll)
        WND_GetRect(g_pRootWin, &rcClient, W_WINDOW, XY_SCREEN);
    else
        WND_GetRect(pWin, &rcClient, W_CLIENT, XY_SCREEN);

    if (pRect)
    {
        // Converts client coordinate to screen coordinate
        CopyRect(&rect, pRect);
        NormalizeRect(&rect);
        OffsetRect(&rect, rcClient.left, rcClient.top);

        // Only invalidate the client area
        IntersectRect(&rect, &rect, &rcClient);

        WND_InvalidateTree(pWin, &rect, bErase, bInvalidateAll);
    }
    else
        WND_InvalidateTree(pWin, &rcClient, bErase, bInvalidateAll);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : ValidateRect
**  Purpose  :
**      Validates the client area within a rectangle by removing the 
**      rectangle from the update region of the specified window. 
**  Params   :
**      hWnd  : Identifies the window whose update region is to be 
**              modified. 
**      pRect : Points to a RECT structure that contains the client 
**              coordinates of the rectangle to be removed from the 
**              update region. If this parameter is NULL, the entire
**              client area is removed. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. 
*/
BOOL WINAPI ValidateRect(HWND hWnd, const RECT* pRect)
{
    PWINOBJ pWin;
    RECT rcClient, rect;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    // The rectangle to be validated is empty, just return
    if (IsRectEmpty(pRect))
        return TRUE;

    ENTERMONITOR;

    WND_GetRect(pWin, &rcClient, W_CLIENT, XY_SCREEN);

    if (pRect)
    {
        CopyRect(&rect, pRect);
        OffsetRect(&rect, rcClient.left, rcClient.top);

        // Only validate the client area
        IntersectRect(&rect, &rect, &rcClient);
        
        WND_ValidateRect(pWin, &rect);
    }
    else
        WND_ValidateRect(pWin, &rcClient);

    LEAVEMONITOR;

    return TRUE;
}

/**************************************************************************/
/*              Window scrolling                                          */ 
/**************************************************************************/

/*
**  Function : ScrollWindow
**  Purpose  :
**      Scrolls the content of the specified window’s client area. 
**  Params   :
**      hWnd      : Indentifies the window where the client area is 
**                  scrolled.
**      dx        : Specifies the amount, in device units, of horizontal 
**                  scrolling. If the window being scrolled has the 
**                  CS_OWNDC or CS_CLASSDC style, then this parameter uses
**                  logical units rather than device units. This parameter
**                  must be a negative value to scroll the content of the 
**                  window to the left. 
**      dy        : Specifies the amount, in device units, of vertical 
**                  scrolling. If the window being scrolled has the 
**                  CS_OWNDC or CS_CLASSDC style, then this parameter uses
**                  logical units rather than device units. This parameter
**                  must be a negative value to scroll the content of the 
**                  window up. 
**      prcScroll : Points to the RECT structure specifying the portion of
**                  the client area to be scrolled. If this parameter is 
**                  NULL, the entire client area is scrolled. 
**      prcClip   : Points to the RECT structure containing the coordinates
**                  of the clipping rectangle. Only device bits within the
**                  clipping rectangle are affected. Bits scrolled from the
**                  outside of the rectangle to the inside are painted; 
**                  bits scrolled from the inside of the rectangle to the
**                  outside are not painted. 
**  Return   : 
**      If the function succeeds, return nonzero.
**      If the function fails, return zero. To get extended infomation, 
**      call GetLastError.
**  Remarks  :
**      If the caret is in the window being scrolled, ScrollWindow 
**      automatically hides the caret to prevent it from being erased and
**      then restores the caret after the scrolling is finished. The caret 
**      position is adjusted accordingly. 
**      The area uncovered by ScrollWindow is not repainted, but it is 
**      combined into the window’s update region. The application 
**      eventually receives a WM_PAINT message notifying it that the region
**      must be repainted. To repaint the uncovered area at the same time 
**      the scrolling is in action, call the UpdateWindow function 
**      immediately after calling ScrollWindow. 
**      If the prcScroll parameter is NULL, the positions of any child 
**      windows in the window are offset by the amount specified by the 
**      dx and dy parameters; invalid (unpainted) areas in the window are 
**      also offset. ScrollWindow is faster when prcScroll is NULL. 
**      If prcScroll is not NULL, the positions of child windows are not 
**      changed and invalid areas in the window are not offset. To prevent
**      updating problems when prcScroll is not NULL, call UpdateWindow to
**      repaint the window before calling ScrollWindow. 
*/
BOOL WINAPI ScrollWindow(HWND hWnd, int dx, int dy, 
                         const RECT* prcScroll, const RECT* prcClip)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    if (dx == 0 && dy == 0)
        return TRUE;

    ENTERMONITOR;

    WND_ScrollWindow(pWin, dx, dy, prcScroll, prcClip);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : ScrollWindowEx
*/
int WINAPI ScrollWindowEx(HWND hwnd, int dx, int dy, const RECT* prcScroll, 
                          const RECT* prcClip, HRGN hrgnUpdate, 
                          RECT* prcUpdate, UINT flags)
{
    return 0;
}

/**************************************************************************/
/*              Window activation                                         */ 
/**************************************************************************/
DWORD GetWindowThreadProcessId(HWND hWnd, LPDWORD lpdwProcessid)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    if (lpdwProcessid != NULL)
        *lpdwProcessid = WS_GetProcessIdFromThreadId(pWin->dwThreadId);

    return pWin->dwThreadId;
}
/*
**  Fucntion : GetForegroundWindow
**  Purpose  :
**      The GetForegroundWindow function returns a handle to the foreground 
**      window (the window with which the user is currently working). The 
**      system assigns a slightly higher priority to the thread that creates
**      the foreground window than it does to other threads. 
**  Params   :
**      None.
**  Return   :
**      The return value is a handle to the foreground window. The 
**      foreground window can be NULL in certain circumstances, such as 
**      when a window is losing activation. 
*/
HWND WINAPI GetForegroundWindow(void)
{
    PWINOBJ pWin;

    ENTERMONITOR;

    pWin = INPUTDEV_GetForegroundWindow();

    LEAVEMONITOR;

    if (!pWin)
        return NULL;

    return (HWND)WOT_GetHandle((PWSOBJ)pWin);
}

/*
**  Function : SetForegroundWindow
**  Purpose  :
**      The SetForegroundWindow function puts the thread that created the 
**      specified window into the foreground and activates the window. 
**      Keyboard input is directed to the window, and various visual cues 
**      are changed for the user. The system assigns a slightly higher 
**      priority to the thread that created the foreground window than it 
**      does to other threads. 
**  Params   :
**      hWnd : Identifies the top-level window to be activated. 
**  Return   :
**      If the function succeeds, return the handle of the window that was
**      previously active. 
*/
BOOL WINAPI SetForegroundWindow(HWND hWnd)
{
    PWINOBJ pWin, pTopLevelWin;
    BOOL bRet = FALSE;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    pTopLevelWin = pWin;
    while (!ISROOTWIN(pTopLevelWin->pParent))
        pTopLevelWin = pTopLevelWin->pParent;

    bRet = INPUTDEV_SetForegroundWindow(pTopLevelWin);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Fucntion : GetActiveWindow
**  Purpose  :
**      Retrieves the window handle to the active window associated with 
**      the thread that calls the function. 
**  Params   :
**      None.
**  Return   :
**      If the function succeeds, return the handle to the active window 
**      associated with the thread that calls the function. If the calling 
**      thread does not have an active window, the return value is NULL. 
*/
HWND WINAPI GetActiveWindow(void)
{
    PWINOBJ pWin;

    ENTERMONITOR;

    pWin = INPUTDEV_GetActiveWindow();

    LEAVEMONITOR;

    if (!pWin)
        return NULL;

    return (HWND)WOT_GetHandle((PWSOBJ)pWin);
}

/*
**  Function : SetActiveWindow
**  Purpose  :
**      Activates the specified window. 
**  Params   :
**      hWnd : Identifies the top-level window to be activated. 
**  Return   :
**      If the function succeeds, return the handle of the window that was
**      previously active. 
*/
HWND WINAPI SetActiveWindow(HWND hWnd)
{
    PWINOBJ pWin, pOldWin, pTopLevelWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return NULL;
    }

    if (pWin->dwThreadId != WS_GetCurrentThreadId())
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pTopLevelWin = pWin;
    while (!ISROOTWIN(pTopLevelWin->pParent))
        pTopLevelWin = pTopLevelWin->pParent;

    ASSERT(pTopLevelWin != NULL);
    pOldWin = INPUTDEV_SetActiveWindow(pTopLevelWin, WA_ACTIVE, FALSE);

    LEAVEMONITOR;

    if (!pOldWin)
        return NULL;

    return (HWND)WOT_GetHandle((PWSOBJ)pOldWin);
}

/**************************************************************************/
/*              Keyboard input support                                    */ 
/**************************************************************************/

/*
**  Function : GetFocus
**  Purpose  :
**      Retrieves the handle of the keyboard focus window.
**  Params   :
**      None.
**  Return   :
**      If the function succeeds, return the handle of the window with the
**      keyboard focus. If the calling thread’s message queue does not 
**      have an associated window with the keyboard focus, return NULL. 
*/
HWND WINAPI GetFocus(void)
{
    PWINOBJ pWin;

    ENTERMONITOR;

    pWin = INPUTDEV_GetFocus();

    LEAVEMONITOR;

    if (!pWin)
        return NULL;

    return (HWND)WOT_GetHandle((PWSOBJ)pWin);
}

/*
**  Function : SetFocus
**  Purpose  :
**      Sets the keyboard focus to the specified window. All subsequent 
**      keyboard input is directed to this window. The window, if any, 
**      that previously had the keyboard focus loses it.
**  Params:
**      hWnd : Identifies the window that will receive the keyboard input. 
**  Return   :
**      If the function succeeds, return the handle of the window that 
**      previously had the keyboard focus. If the hWnd parameter is invalid,
**      return NULL. 
**  Remark   :
**      The SetFocus function sends a WM_KILLFOCUS message to the window 
**      that loses the keyboard focus and a WM_SETFOCUS message to the 
**      window that receives the keyboard focus. It also activates either
**      the window that receives the focus or the parent of the window that
**      receives the focus. 
*/
HWND WINAPI SetFocus(HWND hWnd)
{
    PWINOBJ pWin, pOldWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    
    if (!pWin)
    {
        SetLastError(1);
        return NULL;
    }

    if (pWin->dwThreadId != WS_GetCurrentThreadId())
        return NULL;
    
    ENTERMONITOR;

    pOldWin = INPUTDEV_SetFocus(pWin);

    LEAVEMONITOR;

    if (!pOldWin)
        return NULL;

    return (HWND)WOT_GetHandle((PWSOBJ)pOldWin);
}

/*
**  Function : GrabKeyboard
**  Purpose  :
**      Grabs the keyboard input to a specified window.
**  Params   :
**      hWnd : Identifies the window that will grab the keyboard input. 
*/
HWND WINAPI GrabKeyboard(HWND hWnd)
{
    PWINOBJ pWin, pOldWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    
    if (!pWin)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pOldWin = INPUTDEV_GrabKeyboard(pWin);

    LEAVEMONITOR;

    if (!pOldWin)
        return NULL;

    return (HWND)WOT_GetHandle((PWSOBJ)pOldWin);
}

/*
**  Function : UngrabKeyboard
**  Purpose  :
**      Ungrabs the keyboard input to a specified window.
*/
BOOL WINAPI UngrabKeyboard(void)
{
    BOOL bRet;

    ENTERMONITOR;

    bRet = INPUTDEV_UngrabKeyboard();

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : SendChar
*/
BOOL WINAPI SendChar(HWND hWnd, int wCharCode)
{
    PWINOBJ pWin;
    BOOL bRet;

    if (hWnd != NULL)
    {
        pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
        
        if (!pWin)
        {
            SetLastError(1);
            return FALSE;
        }
    }
    else
        pWin = NULL;

    ENTERMONITOR;

    bRet = INPUTDEV_SendChar(pWin, (WORD)wCharCode);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : SendKeyDown
*/
BOOL WINAPI SendKeyDown(HWND hWnd, int wVirtKey, int wKeyState)
{
    PWINOBJ pWin;
    BOOL bRet;

    if (hWnd != NULL)
    {
        pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
        
        if (!pWin)
        {
            SetLastError(1);
            return FALSE;
        }
    }
    else
        pWin = NULL;

    ENTERMONITOR;

    bRet = INPUTDEV_SendKeyDown(pWin, (WORD)wVirtKey, (WORD)wKeyState);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : SendKeyUp
*/
BOOL WINAPI SendKeyUp(HWND hWnd, int wVirtKey, int wKeyState)
{
    PWINOBJ pWin;
    BOOL bRet;

    if (hWnd != NULL)
    {
        pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
        
        if (!pWin)
        {
            SetLastError(1);
            return FALSE;
        }
    }
    else
        pWin = NULL;

    ENTERMONITOR;

    bRet = INPUTDEV_SendKeyUp(pWin, (WORD)wVirtKey, (WORD)wKeyState);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : keybd_event
**  Purpose  :
**      Synthesizes a keystroke. The system can use such a synthesized 
**      keystroke to generate a WM_KEYUP or WM_KEYDOWN message. 
**  Params   :
**      bVk         : Specifies a virtual-key code. The code must be a 
**                    value in the range 1 to 254. 
**      bScan       : Specifies a hardware scan code for the key. 
**      dwFlags     : A set of flag bits that specify various aspects of 
**                    function operation. An application can use any 
**                    combination of the following predefined constant 
**                    values to set the flags: 
**                      KEYEVENTF_EXTENDEDKEY 
**                      KEYEVENTF_KEYUP 
**      dwExtraInfo : Specifies an additional 32-bit value associated with
**                    the key stroke. 
*/
void WINAPI keybd_event(int bVk, int bScan, DWORD dwFlags, 
                        DWORD dwExtraInfo)
{
//    DEVQ_PutKeyboardEvent((BYTE)bVk, (BYTE)bScan, dwFlags, dwExtraInfo);
    KeyboardEvent(bVk, bScan, dwFlags, dwExtraInfo);

}

/*
**  Function : GetKeyboardState
**  Purpose  :
**      Copies the status of the 256 virtual keys to the specified buffer.
*/
BOOL WINAPI GetKeyboardState(PBYTE pKeyState)
{
    if (!pKeyState)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    INPUTDEV_GetKeyboardState(pKeyState);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : ToAscii
**  Purpose  :
**      Translates the specified virtual-key code and keyboard state to 
**      the corresponding Windows character or characters. The function 
**      translates the code using the input language and physical 
**      keyboard layout identified by the given keyboard layout handle. 
*/
int WINAPI ToAscii(UINT uVirtKey, UINT uScanCode, PBYTE pKeyState,
                   PWORD pChar, UINT uFlags)
{
    int nRet;

    if (!pKeyState || !pChar)
    {
        SetLastError(1);
        return 0;
    }

    ENTERMONITOR;

    nRet = INPUTDEV_ToAscii(uVirtKey, uScanCode, pKeyState, pChar, uFlags);

    LEAVEMONITOR;
    return nRet;
}

/**************************************************************************/
/*              Point device input support                                */ 
/**************************************************************************/

BOOL BeginPenCalibrate(HWND hWnd)
{
    PWINOBJ pWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    INPUTDEV_BeginPenCalibrate(pWin);

    LEAVEMONITOR;

    return TRUE;
}

void EndPenCalibrate(void)
{
    ENTERMONITOR;

	INPUTDEV_EndPenCalibrate();

    LEAVEMONITOR;
}


/*
**  Function : SetPointDevScale
**  Purpose  :
**      
*/
void WINAPI SetPointDevScale(int nLogX1, int nLogY1, int nPhyX1, 
                             int nPhyY1, int nLogX2, int nLogY2, 
                             int nPhyX2, int nPhyY2)
{
    ENTERMONITOR;

    INPUTDEV_SetPointDevScale(nLogX1, nLogY1, nPhyX1, nPhyY1, 
        nLogX2, nLogY2, nPhyX2, nPhyY2);

    LEAVEMONITOR;
}

/*
**  Function : GetCapture
**  Purpose  :
**      Retrieves the handle of the window (if any) that has captured the 
**      mouse. Only one window at a time can capture the mouse; this 
**      window receives mouse input whether or not the cursor is within its
**      borders. 
**  Params   :
**      None.
**  Return   :
**      If the function succeeds, return the handle of the capture window 
**      associated with the current thread. If no window in the thread has 
**      captured the mouse, the return value is NULL. 
**  Remark   :
**      A NULL return value does not mean no other thread or process in the
**      system has captured the mouse; it just means the current thread has
**      not captured the mouse. 
*/
HWND WINAPI GetCapture(void)
{
    PWINOBJ pWin;

    ENTERMONITOR;

    pWin = INPUTDEV_GetCapture();

    LEAVEMONITOR;
    
    return pWin ? (HWND)WOT_GetHandle((PWSOBJ)pWin) : NULL;
}

/*
**  Function : SetCapture
**  Purpose  :    
**      Sets the mouse capture to the specified window belonging to the 
**      current thread. Once a window has captured the mouse, all mouse 
**      input is directed to that window, regardless of whether the cursor
**      is within the borders of that window. Only one window at a time 
**      can capture the mouse. 
**      If the mouse cursor is over a window created by another thread, 
**      the system will direct mouse input to the specified window only if 
**      a mouse button is down. 
**  Params   :
**      hWnd : Identifies the window in the current thread that is to 
**             capture the mouse. 
**  Return   :
**      If the function succeeds, return the handle of the window that 
**      had previously captured the mouse. If there is no such window, 
**      the return value is NULL. 
**  Remark   :
**      Only the foreground window can capture the mouse. When a background
**      window attempts to do so, the window receives messages only for 
**      mouse events that occur when the cursor hot spot is within the 
**      visible portion of the window. Also, even if the foreground window
**      has captured the mouse, the user can still click another window, 
**      bringing it to the foreground. 
**      When the window no longer requires all mouse input, the thread that
**      created the window should call the ReleaseCapture function to 
**      release the mouse. 
**      This function cannot be used to capture mouse input meant for 
**      another process. 
*/
HWND WINAPI SetCapture(HWND hWnd)
{
    PWINOBJ pWin, pOldWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pOldWin = INPUTDEV_SetCapture(pWin, TRUE);

    LEAVEMONITOR;
    
    return pOldWin ? (HWND)WOT_GetHandle((PWSOBJ)pOldWin) : NULL;
}

/*
**  Function : SetNCCapture
*/
HWND WINAPI SetNCCapture(HWND hWnd)
{
    PWINOBJ pWin, pOldWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pOldWin = INPUTDEV_SetCapture(pWin, FALSE);

    LEAVEMONITOR;

    return pOldWin ? (HWND)WOT_GetHandle((PWSOBJ)pOldWin) : NULL;
}


/*
**  Function : ReleaseCapture
**  Purpose  :
**      Releases the mouse capture from a window in the current thread and
**      restores normal mouse input processing. A window that has captured
**      the mouse receives all mouse input, regardless of the position of 
**      the cursor, except when a mouse button is clicked while the cursor
**      hot spot is in the window of another thread. 
**  Params   :
**      None.
**  Return   :
**      If the function succeeds, return nonzero. 
**      If the function fails, return zero. 
**  Remark   :
**      An application calls this function after calling the SetCapture 
**      function. 
*/
BOOL WINAPI ReleaseCapture(void)
{
    BOOL bRet;

    ENTERMONITOR;

    bRet = INPUTDEV_ReleaseCapture();

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : mouse_event
**  Purpose  :
**      Synthesizes mouse motion and button clicks. 
**  Params   :
**      dwFlags     : A set of flag bits that specify various aspects of
**                    mouse motion and button clicking. 
**      dx          : Specifies the mouse’s absolute position along the 
**                    x-axis or its amount of motion since the last mouse 
**                    event was generated, depending on the setting of 
**                    MOUSEEVENTF_ABSOLUTE. Absolute data is given as the 
**                    mouse’s actual x-coordinate; relative data is given
**                    as the number of mickeys moved. A mickey is the 
**                    amount that a mouse has to move for it to report 
**                    that it has moved. 
**      dy          : Specifies the mouse’s absolute position along the 
**                    y-axis or its amount of motion since the last mouse
**                    event was generated, depending on the setting of 
**                    MOUSEEVENTF_ABSOLUTE. Absolute data is given as the
**                    mouse’s actual y-coordinate; relative data is given
**                    as the number of mickeys moved. 
**      dwData      : If dwFlags is MOUSEEVENTF_WHEEL, then dwData 
**                    specifies the amount of wheel movement. A positive
**                    value indicates that the wheel was rotated forward, 
**                    away from the user; a negative value indicates that
**                    the wheel was rotated backward, toward the user. One
**                    wheel click is defined as WHEEL_DELTA, which is 120. 
**                    If dwFlags is not MOUSEEVENTF_WHEEL, then dwData 
**                    should be zero. 
**      dwExtraInfo : Specifies an additional 32-bit value associated with
**                    the mouse event. An application calls 
**                    GetMessageExtraInfo to obtain this extra information. 
*/
void WINAPI mouse_event(DWORD dwFlags, LONG dx, LONG dy, DWORD dwData,
                        DWORD dwExtraInfo)
{
//    ENTERMONITOR;
//    DEVQ_PutMouseEvent(dwFlags, dx, dy, dwData, dwExtraInfo);
    MouseEvent(dwFlags, dx, dy, dwData, dwExtraInfo);

//    LEAVEMONITOR;
}

/**************************************************************************/
/*              Resend mouse message                                      */ 
/**************************************************************************/

BOOL WINAPI ResendMouseMessage(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                               LPARAM lParam)
{
    PWINOBJ pWin;
    BOOL bRet;
    
    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    bRet = INPUTDEV_ResendMouseMessage(pWin, wMsgCmd, wParam, lParam);

    LEAVEMONITOR;

    return bRet;
}

/**************************************************************************/
/*              Remove input events                                       */ 
/**************************************************************************/

/*
**  Function : FlushInputMessages
**  Purpose  :
**      Flushs all input messages in input message queue.
*/
void WINAPI FlushInputMessages(void)
{
    ENTERMONITOR;

//    DEVQ_RemoveInputEvents();
    DEVQ_RemoveInputEvents(WS_GetCurrentThreadInfo());

    LEAVEMONITOR;
}

/**************************************************************************/
/*              System modal window support                               */
/**************************************************************************/

/*
**  Function : GetSysModalWindow
**  Purpose  :
**      Retrieves the handle of the system-modal window, if one is present. 
**  Return   :
**      Returns the handle of the system-modal window, if one is present. 
**      Otherwise, it is NULL. 
*/
HWND WINAPI GetSysModalWindow(void)
{
    PWINOBJ pWin;

    ENTERMONITOR;

    pWin = INPUTDEV_GetSysModalWindow();

    LEAVEMONITOR;
    
    if (!pWin)
        return NULL;

    return (HWND)WOT_GetHandle((PWSOBJ)pWin);
}

/*
**  Function : SetSysModalWindow
**  Purpose  :
**      Makes the given window the system-modal window. 
**  Params   :
**      hWnd : Indentifies the window to be made to be system modal.
**  Return   :
**      Returns the handle of the window that was previously the 
**      system-modal window, if the function is successful. 
**  Remarks  :
**      If another window is made the active window (for example, the 
**      system-modal window creates a dialog box that becomes the active
**      window), the active window becomes the system-modal window. When
**      the original window becomes active again, it is once again the 
**      system-modal window. To end the system-modal state, destroy the 
**      system-modal window. 
*/
HWND WINAPI SetSysModalWindow(HWND hWnd)
{
    PWINOBJ pWin, pPrevWin;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pPrevWin = INPUTDEV_SetSysModalWindow(pWin);

    LEAVEMONITOR;

    if (!pPrevWin)
        return NULL;

    return (HWND)WOT_GetHandle((PWSOBJ)pPrevWin);
}

/**************************************************************************/
/*              Timer Support                                             */
/**************************************************************************/

// Timer support functions implmented in wstimer.c

/**************************************************************************/
/*              Menu Functions                                            */
/**************************************************************************/

// Menu function implemented in apimenu.c

/**************************************************************************/
/*              Clipboard manager                                         */ 
/**************************************************************************/

// Clipboard functions implemented in wsclipboard.c

/**************************************************************************/
/*              Cursor support                                            */ 
/**************************************************************************/

static BOOL bWaitCursor;    // 是否进入了等待光标状态

#if (!NOCURSOR)
static HCURSOR hcurSave;    // 进入了等待光标状态时用于保存原来的光标
#endif

/*
**  Function : LoadCursor
**  Purpose  :
**      Loads the specified cursor resource from the executable (.EXE) 
**      file associated with an application instance. 
*/
HCURSOR WINAPI LoadCursor(HINSTANCE hInstance, PCSTR pszCursorName)
{
#if (!NOCURSOR)

    PCURSOROBJ pCursor;

    if (!hInstance && ((DWORD)pszCursorName) <= 65535)
    {
        pCursor = (PCURSOROBJ)WOT_GetStockCursor((DWORD)pszCursorName);
        if (!pCursor)
        {
            SetLastError(1);
            return NULL;
        }
        
        return (HCURSOR)WOT_GetHandle((PWSOBJ)pCursor);
    }

#endif // NOCURSOR

    return NULL;
}

/*
**  Function : CreateCursor
**  Purpose  :
**      Creates a cursor having the specified size, bit patterns, and hot
**      spot. 
**  Params   :
**      hInst     : Identifies the current instance of the application 
**                  creating the cursor. 
**      nHotX     : Specifies the x-coordinate of the cursor’s hot spot. 
**      nHotY     : Specifies the y-coordinate of the cursor’s hot spot. 
**      nWidth    : Specifies the width, in pixels, of the cursor. 
**      nHeight   : Specifies the height, in pixels, of the cursor. 
**      pANDPlane : Points to an array of bytes that contains the bit 
**                  values for the AND bitmask of the cursor, as in a 
**                  device-dependent monochrome bitmap. 
**      pXORPlane : Points to an array of bytes that contains the bit 
**                  values for the XOR bitmask of the cursor, as in a 
**                  device-dependent monochrome bitmap. 
**  Return   :
**      If the function succeeds, the return value identifies the cursor. 
**      If the function fails, the return value is NULL. To get extended 
**      error information, call GetLastError. 
*/
HCURSOR WINAPI CreateCursor(HINSTANCE hInst, int nHotX, int nHotY, 
                            int nWidth, int nHeight, 
                            const void* pANDPlane, const void* pXORPlane)
{
#if (!NOCURSOR)

    PCURSOROBJ pCursor;

    ENTERMONITOR;

    pCursor = CURSOR_Create(CUR_BW, ROP_SRC, nHotX, nHotY, nWidth, nHeight,
        pANDPlane, pXORPlane);

    LEAVEMONITOR;

    if (!pCursor)
        return (HCURSOR)NULL;

    return (HCURSOR)WOT_GetHandle((PWSOBJ)pCursor);
    
#else  // NOCURSOR

    return NULL;

#endif // NOCURSOR
}

/*
**  Funciton : DestroyCursor
**  Purpose  :
**      Destroys a cursor created by the CreateCursor function and frees 
**      any memory the cursor occupied. Do not use this function to 
**      destroy a cursor that was not created with the CreateCursor 
**      function. 
**  Params   :
**      hCursor : Identifies the cursor to be destroyed. The cursor must 
**                not be in use. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended
**      error information, call GetLastError. 
*/
BOOL WINAPI DestroyCursor(HCURSOR hCursor)
{
#if (!NOCURSOR)

    PCURSOROBJ pCursor, pCursorCur;

    // If the cursor is in use, can't destroy it, return FALSE;
    pCursor = (PCURSOROBJ)WOT_GetObj((HANDLE)hCursor, OBJ_CURSOR);
    if (!pCursor)
    {
        SetLastError(1);
        return FALSE;
    }

    pCursorCur = WS_GetCursor();
    if (pCursor == pCursorCur)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    CURSOR_Destroy(pCursor);

    LEAVEMONITOR;

#endif // NOCURSOR

    return TRUE;
}

/*
**  Function : ShowCursor
**  Purpose  :
**      Displays or hides the cursor. 
**  Params   :
**      bShow : Specifies whether the internal display counter is to be 
**              incremented or decremented. If bShow is TRUE, the display
**              count is incremented by one. If bShow is FALSE, the 
**              display count is decremented by one. 
**  Return   :
**      The return value specifies the new display counter. 
**  Remark   :
**      This function sets an internal display counter that determines 
**      whether the cursor should be displayed. The cursor is displayed 
**      only if the display count is greater than or equal to 0. 
*/
int WINAPI ShowCursor(BOOL bShow)
{
#if (!NOCURSOR)
    int nRet;

    ENTERMONITOR;

    nRet = WS_ShowCursor(bShow);

    LEAVEMONITOR;

    return nRet;
#else  // NOCURSOR
    return 0;
#endif // NOCURSOR
}

/*
**  Function : SetCursorPos
**  Purpose  :
**      Moves the cursor to the specified screen coordinates. If the new
**      coordinates are not within the screen rectangle set by the most 
**      recent ClipCursor function, Windows automatically adjusts the 
**      coordinates so that the cursor stays within the rectangle. 
**  Params   :
**      x    : Specifies the new x-coordinate, in screen coordinates. 
**      y    : Specifies the new y-coordinate, in screen coordinates. 
**  Return   :
**      If the function succeeds, return nonzero. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
*/
BOOL WINAPI SetCursorPos(int x, int y)
{
    mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, x, y, 0, 0);
    return TRUE;
}

/*
**  Function : GetCursorPos
**  Purpose  :
**      Retrieves the cursor’s position, in screen coordinates. 
**  Params   :
**      pPoint : Points to a POINT structure that receives the screen 
**               coordinates of the cursor. 
**  Return   :
**      If the function succeeds, return nonzero. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
*/
BOOL WINAPI GetCursorPos(PPOINT pPoint)
{
    if (!pPoint)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_GetCursorPos(pPoint);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : SetCursor
**  Purpose  :
**      Establishes the cursor shape. 
**  Params   :
**      hCursor : Identifies the cursor. The cursor must have been created
**                by the CreateCursor or loaded by the LoadCursor function.
**                If this parameter is NULL, the cursor is removed from the
**                screen. 
**  Return   :
**      Return the handle of the previous cursor, if there was one. 
**  Remarks
**      The cursor is set only if the new cursor is different from the 
**      previous cursor; otherwise, the function returns immediately. 
*/
HCURSOR WINAPI SetCursor(HCURSOR hCursor)
{
#if (!NOCURSOR)

    PCURSOROBJ pCursor, pOldCursor;

    if (hCursor)
    {
        pCursor = (PCURSOROBJ)WOT_GetObj((HANDLE)hCursor, OBJ_CURSOR);
        if (!pCursor)
        {
            SetLastError(1);
            return NULL;
        }
    }
    else
        pCursor = NULL;

    // 在处于等待光标状态时不真正切换光标
    if (bWaitCursor)
    {
        HCURSOR hcurOld;

        hcurOld = hcurSave;
        hcurSave = hCursor;
        return hcurOld;
    }

    ENTERMONITOR;

    pOldCursor = WS_SetCursor(pCursor);

    LEAVEMONITOR;

    if (!pOldCursor)
        return NULL;

    return (HCURSOR)WOT_GetHandle((PWSOBJ)pOldCursor);
    
#else  // NOCURSOR

    return NULL;

#endif // NOCURSOR    
}

/*
**  Function : SetMaxCursorRect
**  Purpose  :
**      设置光标可以到达的最大矩形区域，调用ClipCursor函数时，如果指定的光
**      标裁剪矩形为NULL时，光标裁剪矩形将被设置为由本函数设置的最大矩形区
**      域。
**  Params   :
**      pRect : Points to the RECT structure that contains the screen 
**              coordinates of the upper-left and lower-right corners of
**              the maximum cursor confining rectangle. If this parameter 
**              is NULL, the cursor is free to move anywhere on the screen. 
**  Return   :
**      If the function succeeds, return nonzero. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
*/
BOOL WINAPI SetMaxCursorRect(const RECT* pRect)
{
    ENTERMONITOR;

    WS_SetMaxCursorRect(pRect);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : ClipCursor
**  Purpose  :
**      Confines the cursor to a rectangular area on the screen. If a 
**      subsequent cursor position (set by the SetCursorPos function or
**      the mouse) lies outside the rectangle, Windows automatically 
**      adjusts the position to keep the cursor inside the rectangular area.
**  Params   :
**      pRect : Points to the RECT structure that contains the screen 
**              coordinates of the upper-left and lower-right corners of
**              the confining rectangle. If this parameter is NULL, the 
**              cursor is free to move anywhere on the screen. 
**  Return   :
**      If the function succeeds, return nonzero. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
**  Remarks
**      The cursor is a shared resource. If an application confines the 
**      cursor, it must release the cursor by using ClipCursor before 
**      relinquishing control to another application. 
*/
BOOL WINAPI ClipCursor(const RECT* pRect)
{
    ENTERMONITOR;

    WS_ClipCursor(pRect);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : GetClipCursor
**  Purpose  :
**      Retrieves the screen coordinates of the rectangular area to which 
**      the cursor is confined
**  Params   :
**      pRect : Points to a RECT struct that receives the screen coordinate
**              of the cofining rectangle. The structure receives the 
**              dimensions of the screen if the cursor is not confined to a
**              rectangle. 
**  Return   :
**      If the function succeeds, return nonzero. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
*/
BOOL WINAPI GetClipCursor(PRECT pRect)
{
    if (!pRect)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_GetClipCursor(pRect);

    LEAVEMONITOR;

    return TRUE;
}

/**************************************************************************/
/*              Wait Cursor Support                                       */ 
/**************************************************************************/

/*
**  Function : BeginWaitCursor
**  Purpose  :
**      Call this function to display the cursor as an hourglass when you 
**      expect a command to take a noticeable time interval to execute. 
*/
void WINAPI BeginWaitCursor(void)
{
#if (!NOCURSOR)

    HCURSOR hcurWait;

    hcurWait = LoadCursor(NULL, IDC_WAIT);
    hcurSave = SetCursor(hcurWait);

#endif // NOCURSOR

    bWaitCursor = TRUE;
}

/*
**  Function : EndWaitCursor
**  Purpose  :
**      Call this function after you have called the BeginWaitCursor 
**      function to return from the hourglass cursor to the previous 
**      cursor. 
*/
void WINAPI EndWaitCursor(void)
{
    // Restore the original cursor
    if (bWaitCursor)
    {
#if (!NOCURSOR)
        SetCursor(hcurSave);
        hcurSave = NULL;
#endif // NOCURSOR

        // Remove all input events 
        ENTERMONITOR;

//        DEVQ_RemoveInputEvents();
        DEVQ_RemoveInputEvents(WS_GetCurrentThreadInfo());

        LEAVEMONITOR;

        bWaitCursor = FALSE;
    }
}

/**************************************************************************/
/*              Icon support                                              */ 
/**************************************************************************/

/*
**  Function : CreateIcon
**  Purpose  :
**      Creates an icon that has the specified size, colors, and bit 
**      patterns. 
**  Params   :
**      hInstance : Identifies the instance of the module creating the icon.
**      nWidth    : Specifies the width, in pixels, of the icon. 
**      nHeight   : Specifies the height, in pixels, of the icon. 
**      cPlanes   : Specifies the number of planes in the XOR bitmask of 
**                  the icon. 
**      cBitsPixel: Specifies the number of bits per pixel in the XOR 
**                  bitmask of the icon. 
**      pANDBits  : Points to an array of bytes that contains the bit 
**                  values for the AND bitmask of the icon. This bitmask
**                  describes a monochrome bitmap. 
**      pXORBits  : Points to an array of bytes that contains the bit 
**                  values for the XOR bitmask of the icon. This bitmask
**                  describes a monochrome or device-dependent color 
**                  bitmap. 
**  Return   :
**      If the function succeeds, return the handle to an icon.
**      If the function fails, return NULL. To get the extendted error
**      information, call GetLastError.
*/
HICON WINAPI CreateIcon(HINSTANCE hInstance, int nWidth, int nHeight, 
                        int cPlanes, int cBitsPixel, 
                        const void* pANDBits, const void* pXORBits)
{
    PICONOBJ pIconObj;
    BITMAPINFO*     pbmi;
    WORD            cClrBits;
    HBITMAP         hbmpXor, hbmpAnd;
    BITMAPINFOHEADER bmih;

    pIconObj = (PICONOBJ)MemAlloc(sizeof(ICONOBJ));
    if (!pIconObj)
        return NULL;

    pIconObj->bWidth = nWidth;
    pIconObj->bHeight = nHeight;
    cClrBits = (WORD)(cPlanes * cBitsPixel); 
    if (cClrBits != GetDeviceCaps(NULL, BITSPIXEL))
        return NULL;

    hbmpXor = CreateBitmap(nWidth, nHeight, cPlanes, 
        cBitsPixel, pXORBits);
    hbmpAnd = CreateCompatibleBitmap(NULL, nWidth, nHeight);
    if (!hbmpXor || !hbmpAnd)
        return NULL;

    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = nWidth;
    bmih.biHeight = nHeight;
    bmih.biPlanes = 1;
    bmih.biBitCount = 1;
    bmih.biCompression = 0;
    bmih.biSizeImage = 0;
    bmih.biXPelsPerMeter = 0;
    bmih.biYPelsPerMeter = 0;
    bmih.biClrImportant = 0;
    bmih.biClrUsed = 0;
    
    pbmi = (BITMAPINFO*)MemAlloc(sizeof(BITMAPINFOHEADER) + 
        2 * sizeof(RGBQUAD));
    pbmi->bmiHeader = bmih;
    pbmi->bmiHeader.biBitCount = 1;
    pbmi->bmiColors[0].rgbBlue = 0;
    pbmi->bmiColors[0].rgbGreen = 0;
    pbmi->bmiColors[0].rgbRed = 0;
    pbmi->bmiColors[0].rgbReserved = 0;
    pbmi->bmiColors[1].rgbBlue = 255;
    pbmi->bmiColors[1].rgbGreen = 255;
    pbmi->bmiColors[1].rgbRed = 255;
    pbmi->bmiColors[1].rgbReserved = 0;

    SetDIBits(NULL, hbmpAnd, 0, pbmi->bmiHeader.biHeight, 
        pANDBits, pbmi, DIB_RGB_COLORS);   
    
    MemFree(pbmi);
    pIconObj->hBitmapAND = hbmpAnd;
    pIconObj->hBitmapXOR = hbmpXor;

    return (HICON)pIconObj;
}


#define GET16(p)(WORD)((((WORD)*((BYTE*)(p) + 1)) << 8) + *((BYTE*)(p)))
#define GET32(p)(DWORD)((((DWORD)*((BYTE*)(p) + 3)) << 24) + \
                        (((DWORD)*((BYTE*)(p) + 2)) << 16)  + \
                        (((DWORD)*((BYTE*)(p) + 1)) << 8) +  *((BYTE*)(p)))

HICON WINAPI CreateIconFromFile(PCSTR pszIconName, int nWidth,int nHeight)
{
    BYTE *pMapFile, *pData;
    char achFileName[128];
    int hFile;
    long nFileSize;
    int nIconCount;
    int nImageOffset;
    BITMAPINFO* pbmi;
    WORD  cClrBits;
    BITMAPINFOHEADER bmih;
    HBITMAP hbmpXor, hbmpAnd;
    int i;
    PICONOBJ pIconObj;

    strcpy(achFileName, pszIconName);

    // open the bitmap file.
    hFile = PLXOS_CreateFile(achFileName, PLXFS_ACCESS_READ, 0);
    if (hFile == -1)
        return NULL;
    nFileSize = PLXOS_GetFileSize(hFile);
    pMapFile = (BYTE*)PLXOS_MapFile(hFile, 0, nFileSize);
    ASSERT(pMapFile != (BYTE*)(-1));

    pData = pMapFile;

    // 使用最后一个ICON
    nIconCount = GET16(pData + ICON_COUNT_OFFSET);
    pData += ICON_FILE_HEADER + sizeof(ICONDIRENTRY) * (nIconCount - 1);

    // 获得最后一个icon的位图数据的偏移量
    nImageOffset = GET32(pData + FIELDOFFSET(ICONDIRENTRY, 
        dwImagOffset));

    // 定位到位图数据所在的偏移
    pData = pMapFile + nImageOffset;
    
    // Gets BITMAPINFOHEADER from bitmap file data
    bmih.biSize = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSize)); 
    bmih.biWidth = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biWidth)); 
    bmih.biHeight = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biHeight)); 
    bmih.biPlanes = GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biPlanes)); 
    bmih.biBitCount = 
        GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biBitCount)); 
    bmih.biCompression = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biCompression)); 
    bmih.biSizeImage = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSizeImage));
    bmih.biXPelsPerMeter = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biXPelsPerMeter));
    bmih.biYPelsPerMeter = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biYPelsPerMeter));
    bmih.biClrUsed = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrUsed));
    bmih.biClrImportant = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrImportant));
    
    bmih.biHeight = bmih.biHeight / 2;
    
    // Skips bitmap info header
    pData += sizeof(BITMAPINFOHEADER);
    
    // Calculates the color bits, if the result is an invalid, return 
    // error.
    cClrBits = (WORD)(bmih.biPlanes * bmih.biBitCount); 
    if (cClrBits != 1 && cClrBits != 4 && cClrBits != 8 && 
        cClrBits != 16 && cClrBits != 24 && cClrBits != 32)
    {
        PLXOS_UnmapFile(hFile, pMapFile, nFileSize);
        PLXOS_CloseFile(hFile);
        return NULL;
    }
    
    // Allocates memory for bitmap info pointer
    if (cClrBits <= 8)
    {
        pbmi = (BITMAPINFO*)MemAlloc(sizeof(BITMAPINFOHEADER) + 
            (1 << cClrBits) * sizeof(RGBQUAD));
    }
    else
        pbmi = (BITMAPINFO*)MemAlloc(sizeof(BITMAPINFOHEADER) + 
        2 * sizeof(RGBQUAD));
    
    if (!pbmi)
    {
        PLXOS_UnmapFile(hFile, pMapFile, nFileSize);
        PLXOS_CloseFile(hFile);
        return NULL;
    }
    
    // Fills bmiHeader of pbmi using bmih
    pbmi->bmiHeader = bmih;
    
    // Gets the palettes if necessary
    if (cClrBits <= 8)
    {
        for (i = 0; i < (1 << cClrBits); i++)
        {
            pbmi->bmiColors[i].rgbBlue = *pData++;
            pbmi->bmiColors[i].rgbGreen = *pData++;
            pbmi->bmiColors[i].rgbRed = *pData++;
            pbmi->bmiColors[i].rgbReserved = *pData++;
        }
    }
    
    hbmpXor = CreateCompatibleBitmap(NULL, pbmi->bmiHeader.biWidth, 
        pbmi->bmiHeader.biHeight);
    hbmpAnd = CreateCompatibleBitmap(NULL, pbmi->bmiHeader.biWidth, 
        pbmi->bmiHeader.biHeight);
    
    if (!hbmpXor || !hbmpAnd)
    {
        MemFree(pbmi);
        
        PLXOS_UnmapFile(hFile, pMapFile, nFileSize);
        PLXOS_CloseFile(hFile);
        return NULL;
    }
    
    SetDIBits(NULL, hbmpXor, 0, pbmi->bmiHeader.biHeight, 
        pData, pbmi, DIB_RGB_COLORS);
    
    pData += pbmi->bmiHeader.biHeight * 
        ((pbmi->bmiHeader.biWidth * cClrBits + 31) / 32 * 4);
    
    pbmi->bmiHeader.biBitCount = 1;
    pbmi->bmiColors[0].rgbBlue = 0;
    pbmi->bmiColors[0].rgbGreen = 0;
    pbmi->bmiColors[0].rgbRed = 0;
    pbmi->bmiColors[0].rgbReserved = 0;
    pbmi->bmiColors[1].rgbBlue = 255;
    pbmi->bmiColors[1].rgbGreen = 255;
    pbmi->bmiColors[1].rgbRed = 255;
    pbmi->bmiColors[1].rgbReserved = 0;
    
    SetDIBits(NULL, hbmpAnd, 0, pbmi->bmiHeader.biHeight, 
        pData, pbmi, DIB_RGB_COLORS);
    
    MemFree(pbmi);
    
    PLXOS_UnmapFile(hFile, pMapFile, nFileSize);
    PLXOS_CloseFile(hFile);
    pIconObj = (PICONOBJ)MemAlloc(sizeof(ICONOBJ));
    if (!pIconObj)
        return NULL;

    pIconObj->bWidth = nWidth;
    pIconObj->bHeight = nHeight;
    pIconObj->hBitmapAND = hbmpAnd;
    pIconObj->hBitmapXOR = hbmpXor;

    return (HICON)pIconObj;

}

/*
**  Function : DestroyIcon
**  Purpose  :
**      Destroys an icon and frees the memory the icon occupied.
**  Params   :
**      hIcon : Identifes the icon to be destoryed. The icon must not be 
**              used.
**  Return   :
**      If the function succeeds, return TRUE;
**      If the function fails, return FALSE; To get the extended error
**      infomation, call GetLastError.
**  Remarks  :
**      It is only necessary to call DestroyIcon for icons created with 
**      the CreateIconIndirect function. 
*/
BOOL WINAPI DestroyIcon(HICON hIcon)
{
    PICONOBJ pIconObj;

    pIconObj = (PICONOBJ)hIcon;
    if (pIconObj == NULL)
        return FALSE;
    DeleteObject((HGDIOBJ)pIconObj->hBitmapAND);
    DeleteObject((HGDIOBJ)pIconObj->hBitmapXOR);
    MemFree(pIconObj);
    return TRUE;
}

/*
**  Function : DrawIcon
**  Purpose  : 
**      Draws an icon in the client area of the window of the specified 
**      device context. 
**  Params   :
**      hdc   : Identifes the device context for a window.
**      x     : Specifies the logical x-coordinate of the upper-left corner
**              of the icon. 
**      y     : Specifies the logical y-coordinate of the upper-left corner
**              of the icon.
**      hIcon : Identifies the icon to be drawn.
**  Return   :
**      If the function succeeds, return TRUE;
**      If the function fails, return FALSE; To get the extended error
**      infomation, call GetLastError.
*/
BOOL WINAPI DrawIcon(HDC hdc, int x, int y, HICON hIcon)
{
    PICONOBJ pIconObj;

    pIconObj = (PICONOBJ)hIcon;
    if (pIconObj == NULL)
        return FALSE;
    BitBlt(hdc, x, y, pIconObj->bWidth, pIconObj->bHeight,
        (HDC)pIconObj->hBitmapAND, 0, 0, ROP_SRC_AND_DST);
    BitBlt(hdc, x, y, pIconObj->bWidth, pIconObj->bHeight,
        (HDC)pIconObj->hBitmapXOR, 0, 0, ROP_SRC_XOR_DST);
    return TRUE;
}

BOOL WINAPI GetIconDimension(HICON hIcon, PSIZE pSize)
{
    PICONOBJ pIconObj;

    pIconObj = (PICONOBJ)hIcon;
    if (pIconObj == NULL)
        return FALSE;
    pSize->cx = pIconObj->bWidth;
    pSize->cy = pIconObj->bHeight;
    return TRUE;
}

BOOL WINAPI GetIconDimensionFromFile(PCSTR pszIconName, PSIZE pSize)
{
    BYTE *pMapFile, *pData;
    int hFile;
    int nFileSize;
    int nIconCount;
    int nImageOffset;

    hFile = PLXOS_CreateFile(pszIconName, PLXFS_ACCESS_READ, 0);
    if (hFile == -1)
        return FALSE;
    nFileSize = PLXOS_GetFileSize(hFile);
    pMapFile = (BYTE*)PLXOS_MapFile(hFile, 0, nFileSize);
    ASSERT(pMapFile != (BYTE*)(-1));

    pData = pMapFile;

    // 使用最后一个ICON
    nIconCount = GET16(pData + ICON_COUNT_OFFSET);
    pData += ICON_FILE_HEADER + sizeof(ICONDIRENTRY) * (nIconCount - 1);

    // 获得最后一个icon的位图数据的偏移量
    nImageOffset = GET32(pData + FIELDOFFSET(ICONDIRENTRY, 
        dwImagOffset));

    // 定位到位图数据所在的偏移
    pData = pMapFile + nImageOffset;
    
    // Gets BITMAPINFOHEADER from bitmap file data
    pSize->cx = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biWidth)); 
    pSize->cy = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biHeight)); 
    PLXOS_UnmapFile(hFile, pMapFile, nFileSize);
    PLXOS_CloseFile(hFile);
    return TRUE;
}

#define GET16(p)(WORD)((((WORD)*((BYTE*)(p) + 1)) << 8) + *((BYTE*)(p)))
#define GET32(p)(DWORD)((((DWORD)*((BYTE*)(p) + 3)) << 24) + \
                        (((DWORD)*((BYTE*)(p) + 2)) << 16)  + \
                        (((DWORD)*((BYTE*)(p) + 1)) << 8) +  *((BYTE*)(p)))


void WINAPI DrawIconFromFileEx(HDC hdc, PCSTR pszIconName, int x, int y, int iw, int ih, BOOL bStretch)
{
    BYTE *pMapFile, *pData;
    char achFileName[128];
    int hFile;
    int nFileSize;
    int nIconCount;
    int nImageOffset;
    BITMAPINFO* pbmi;
    WORD  cClrBits;
    BITMAPINFOHEADER bmih;
    HBITMAP hbmpXor, hbmpAnd;
    int i;
    int bmpwidth, bmpheight;

    strcpy(achFileName, pszIconName);

    // open the bitmap file.
    hFile = PLXOS_CreateFile(achFileName, PLXFS_ACCESS_READ, 0);
    if (hFile == -1)
        return;
    nFileSize = PLXOS_GetFileSize(hFile);
    pMapFile = (BYTE*)PLXOS_MapFile(hFile, 0, nFileSize);
    ASSERT(pMapFile != (BYTE*)(-1));

    pData = pMapFile;

    // 使用最后一个ICON
    nIconCount = GET16(pData + ICON_COUNT_OFFSET);
    pData += ICON_FILE_HEADER + sizeof(ICONDIRENTRY) * (nIconCount - 1);

    // 获得最后一个icon的位图数据的偏移量
    nImageOffset = GET32(pData + FIELDOFFSET(ICONDIRENTRY, 
        dwImagOffset));

    // 定位到位图数据所在的偏移
    pData = pMapFile + nImageOffset;
    
    // Gets BITMAPINFOHEADER from bitmap file data
    bmih.biSize = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSize)); 
    bmih.biWidth = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biWidth)); 
    bmih.biHeight = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biHeight)); 
    bmih.biPlanes = GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biPlanes)); 
    bmih.biBitCount = 
        GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biBitCount)); 
    bmih.biCompression = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biCompression)); 
    bmih.biSizeImage = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSizeImage));
    bmih.biXPelsPerMeter = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biXPelsPerMeter));
    bmih.biYPelsPerMeter = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biYPelsPerMeter));
    bmih.biClrUsed = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrUsed));
    bmih.biClrImportant = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrImportant));
    
    bmih.biHeight = bmih.biHeight / 2;
    
    // Skips bitmap info header
    pData += sizeof(BITMAPINFOHEADER);
    
    // Calculates the color bits, if the result is an invalid, return 
    // error.
    cClrBits = (WORD)(bmih.biPlanes * bmih.biBitCount); 
    if (cClrBits != 1 && cClrBits != 4 && cClrBits != 8 && 
        cClrBits != 16 && cClrBits != 24 && cClrBits != 32)
    {
        PLXOS_UnmapFile(hFile, pMapFile, nFileSize);
        PLXOS_CloseFile(hFile);
        return;
    }
    
    // Allocates memory for bitmap info pointer
    if (cClrBits <= 8)
    {
        pbmi = (BITMAPINFO*)MemAlloc(sizeof(BITMAPINFOHEADER) + 
            (1 << cClrBits) * sizeof(RGBQUAD));
    }
    else
        pbmi = (BITMAPINFO*)MemAlloc(sizeof(BITMAPINFOHEADER) + 
        2 * sizeof(RGBQUAD));
    
    if (!pbmi)
    {
        PLXOS_UnmapFile(hFile, pMapFile, nFileSize);
        PLXOS_CloseFile(hFile);
        
        return;
    }
    
    // Fills bmiHeader of pbmi using bmih
    pbmi->bmiHeader = bmih;
    
    // Gets the palettes if necessary
    if (cClrBits <= 8)
    {
        for (i = 0; i < (1 << cClrBits); i++)
        {
            pbmi->bmiColors[i].rgbBlue = *pData++;
            pbmi->bmiColors[i].rgbGreen = *pData++;
            pbmi->bmiColors[i].rgbRed = *pData++;
            pbmi->bmiColors[i].rgbReserved = *pData++;
        }
    }
    
    bmpwidth = pbmi->bmiHeader.biWidth;
    bmpheight = (pbmi->bmiHeader.biHeight) > 0 ? (pbmi->bmiHeader.biHeight) : 
        (-pbmi->bmiHeader.biHeight);
    hbmpXor = CreateCompatibleBitmap(hdc, bmpwidth, bmpheight);
    hbmpAnd = CreateCompatibleBitmap(hdc, bmpwidth, bmpheight);
    
    if (!hbmpXor || !hbmpAnd)
    {
        MemFree(pbmi);
        
        PLXOS_UnmapFile(hFile, pMapFile, nFileSize);
        PLXOS_CloseFile(hFile);
        
        return;
    }
    
    SetDIBits(hdc, hbmpXor, 0, bmpheight, pData, pbmi, DIB_RGB_COLORS);
    
    pData += bmpheight * ((bmpwidth * cClrBits + 31) / 32 * 4);
    
    pbmi->bmiHeader.biBitCount = 1;
    pbmi->bmiColors[0].rgbBlue = 0;
    pbmi->bmiColors[0].rgbGreen = 0;
    pbmi->bmiColors[0].rgbRed = 0;
    pbmi->bmiColors[0].rgbReserved = 0;
    pbmi->bmiColors[1].rgbBlue = 255;
    pbmi->bmiColors[1].rgbGreen = 255;
    pbmi->bmiColors[1].rgbRed = 255;
    pbmi->bmiColors[1].rgbReserved = 0;
    
    SetDIBits(hdc, hbmpAnd, 0, bmpheight, pData, pbmi, DIB_RGB_COLORS);
    
    MemFree(pbmi);
    
    PLXOS_UnmapFile(hFile, pMapFile, nFileSize);
    PLXOS_CloseFile(hFile);

    if (!bStretch || (bmpwidth == iw && bmpheight == ih))
    {
        BitBlt(hdc, x, y, iw, ih, (HDC)hbmpAnd, 0, 0, ROP_SRC_AND_DST);
        BitBlt(hdc, x, y, iw, ih, (HDC)hbmpXor, 0, 0, ROP_SRC_XOR_DST);
    }
    else
    {
        StretchBlt(hdc, x, y, iw, ih, (HDC)hbmpAnd, 0, 0, 
            bmpwidth, bmpheight, ROP_SRC_AND_DST);
        StretchBlt(hdc, x, y, iw, ih, (HDC)hbmpXor, 0, 0, 
            bmpwidth, bmpheight, ROP_SRC_XOR_DST);
    }

    DeleteObject(hbmpXor);
    DeleteObject(hbmpAnd);
}

void WINAPI DrawIconFromFile(HDC hdc, PCSTR pszIconName, int x, int y, int iw, int ih)
{
    DrawIconFromFileEx(hdc, pszIconName, x, y, iw, ih, FALSE);
}
/**************************************************************************/
/*              Message Box support                                       */ 
/**************************************************************************/

// MessageBox and MessageBeep function implemented in msgbox.c

/**************************************************************************/
/*              Caret support                                             */ 
/**************************************************************************/

/*
**  Funciton : CreateCaret
**  Purpsoe  :
**      Creates a new shape for the system caret and assigns ownership of 
**      the caret to the specified window. The caret shape can be a line, 
**      a block, or a bitmap. 
**  Params   :
**      hWnd    : Identifies the window that owns the caret. 
**      hBitmap : Identifies the bitmap that defines the caret shape. If 
**                this parameter is NULL, the caret is solid. If this 
**                parameter is a bitmap handle, the caret is the specified 
**                bitmap. 
**      nWidth  : Specifies the width of the caret. If this parameter is 
**                zero, the width is set to the system-defined window 
**                border width. If hBitmap is a bitmap handle, CreateCaret
**                ignores this parameter. 
**      nHeight : Specifies the height of the caret. If this parameter is 
**                zero, the height is set to the system-defined window 
**                border height. If hBitmap is a bitmap handle, CreateCaret
**                ignores this parameter. 
**  Return   : 
**      If the function succeeds, return nonzero. If the function fails, 
**      return zero. To get extended error information, call GetLastError. 
**
**  Remarks  :
**
**      The nWidth and nHeight parameters specify the caret’s width and 
**      height; 
**
**      CreateCaret automatically destroys the previous caret shape,
**      if any, regardless of the window that owns the caret. The caret is 
**      hidden until the application calls the ShowCaret function to make 
**      the caret visible. 
**
**      The caret is a shared resource; there is only one caret in the 
**      system. A window should create a caret only when it has the 
**      keyboard focus or is active. The window should destroy the caret
**      before losing the keyboard focus or becoming inactive. 
**
**      You can retrieve the width or height of the system’s window border 
**      by using the GetSystemMetrics function, specifying the SM_CXBORDER
**      and SM_CYBORDER values. Using the window border width or height 
**      guarantees that the caret will be visible on a high-resolution 
**      screen. 
*/
BOOL WINAPI CreateCaret(HWND hWnd, HBITMAP hBitmap, int nWidth, 
                        int nHeight)
{
    PWINOBJ pWin;
    PBMPOBJ pBmp;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
    if (!pWin)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    if ((hBitmap != NULL) && ((int)hBitmap != 1))
    {
        pBmp = (PBMPOBJ)WOT_LockObj((HANDLE)hBitmap, OBJ_BITMAP);
        if (!pBmp)
        {
            LEAVEMONITOR;
            SetLastError(1);
            return FALSE;
        }
    }
    else
        pBmp = (PBMPOBJ)hBitmap;

    WS_CreateCaret(pWin, pBmp, nWidth, nHeight);

    if ((hBitmap != NULL) && ((int)hBitmap != 1))
        WOT_UnlockObj((PXGDIOBJ)pBmp);

    LEAVEMONITOR;
    
    return TRUE;
}

/*
**  Function : DestroyCaret
**  Purpose  :
**      Destroys the caret’s current shape, frees the caret from the 
**      window, and removes the caret from the screen. 
**  Params   :
**      None.
**  Return   :
**      If the function succeeds, return nonzero. If the function fails, 
**      return zero. To get extended error information, call GetLastError. 
**
**  Remarks  :
**
**      If the caret shape is based on a bitmap, DestroyCaret does not free
**      the bitmap. 
**
**      The caret is a shared resource; there is only one caret in the 
**      system. A window should create a caret only when it has the 
**      keyboard focus or is active. The window should destroy the caret 
**      before losing the keyboard focus or becoming inactive.
*/
BOOL WINAPI DestroyCaret(void)
{
    ENTERMONITOR;

    WS_DestroyCaret();

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : SetCaretPos
**  Purpose  :
**      Moves the caret to the specified coordinates.
**  Params   :
**      x : Specifies the new x-coordinate of the caret. 
**      y : Specifies the new y-coordinate of the caret. 
**  Return   :
**      If the function succeeds, return nonzero. If the function fails, 
**      return zero. To get extended error information, call GetLastError. 
**  Remarks  :
**      SetCaretPos moves the caret whether or not the caret is hidden. 
**      The caret is a shared resource; there is only one caret in the 
**      system. A window can set the caret position only if it owns the 
**      caret. 
*/
BOOL WINAPI SetCaretPos(int x, int y)
{
    ENTERMONITOR;

    WS_SetCaretPos(x, y);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : GetCaretPos
**  Purpose  :
**      Copies the caret’s position, in client coordinates, to the 
**      specified POINT structure. 
**  Params   :
**      Points to the POINT structure that is to receive the client 
**      coordinates of the caret. 
**  Return   :
**      If the function succeeds, return nonzero. If the function fails, 
**      return zero. To get extended error information, call GetLastError. 
**  Remarks  :
**      The caret position is always given in the client coordinates of 
**      the window that contains the caret. 
*/
BOOL WINAPI GetCaretPos(PPOINT pPoint)
{
    if (!pPoint)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_GetCaretPos(pPoint);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : HideCaret
**  Purpose  :
**      Removes the caret from the screen. Hiding a caret does not destroy
**      its current shape or invalidate the insertion point. 
**  Params   :
**      hWnd : Identifies the window that owns the caret. 
**  Return   :
**      If the function succeeds, return nonzero. If the function fails, 
**      return zero. To get extended error information, call GetLastError. 
**  Remarks  :
**      HideCaret hides the caret only if the specified window owns the 
**      caret. If the specified window does not own the caret, HideCaret
**      does nothing and returns FALSE. 
**      Hiding is cumulative. If your application calls HideCaret five 
**      times in a row, it must also call ShowCaret five times before the
**      caret is displayed. 
*/
BOOL WINAPI HideCaret(HWND hWnd)
{
    PWINOBJ pWin;
    BOOL bRet;

    if (hWnd)
    {
        pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
        if (!pWin)
        {
            SetLastError(1);
            return FALSE;
        }
    }
    else
        pWin = NULL;

    ENTERMONITOR;

    bRet = WS_HideCaret(pWin);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : ShowCaret
**  Purpose  :
**      Makes the caret visible on the screen at the caret’s current 
**      position. When the caret becomes visible, it begins flashing 
**      automatically. 
**  Params   :
**      hWnd : Identifies the window that owns the caret. 
**  Return   :
**      If the function succeeds, return nonzero. If the function fails, 
**      return zero. To get extended error information, call GetLastError. 
*/
BOOL WINAPI ShowCaret(HWND hWnd)
{
    PWINOBJ pWin;
    BOOL bRet;

    if (hWnd)
    {
        pWin = (PWINOBJ)WOT_GetObj((HANDLE)hWnd, OBJ_WINDOW);
        if (!pWin)
        {
            SetLastError(1);
            return FALSE;
        }
    }
    else
        pWin = NULL;

    ENTERMONITOR;

    bRet = WS_ShowCaret(pWin);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : GetCaretBlinkTime
**  Purpose  :
**      Returns the elapsed time, in milliseconds, required to invert the 
**      caret’s pixels. 
**  Params   :
**      None.
**  Return   :
**      If the function succeeds, return the blink time, in milliseconds. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
*/
UINT WINAPI GetCaretBlinkTime(void)
{
    UINT uRet;

    ENTERMONITOR;

    uRet = WS_GetCaretBlinkTime();

    LEAVEMONITOR;

    return uRet;
}

/*
**  Function : SetCaretBlinkTime
**  Purpose  :
**      Sets the caret blink time to the specified number of milliseconds.
**      The blink time is the elapsed time, in milliseconds, required to 
**      invert the caret’s pixels. 
**  Params   :
**      uMSeconds : Specifies the new blink time, in milliseconds. 
**  Return   :
**      If the function succeeds, return nonzero. If the function fails, 
**      return zero. To get extended error information, call GetLastError. 
*/
BOOL WINAPI SetCaretBlinkTime(UINT uMSeconds)
{
    ENTERMONITOR;

    WS_SetCaretBlinkTime(uMSeconds);

    LEAVEMONITOR;

    return TRUE;
}

/**************************************************************************/
/*              Dialog and Control Management                             */ 
/**************************************************************************/

// Dialog and Control Management functions implemented in apidlg.c

/**************************************************************************/
/*              Change display mode                                       */ 
/**************************************************************************/

/*
**  Function : ChangeDisplaySettings
**  Purpose  :
**      changes the display settings to the specified graphics mode. 
*/
LONG WINAPI ChangeDisplaySettings(PDEVMODE pDevMode, DWORD dwFlags)
{
    LONG lRet;

    if (!pDevMode)
        return DISP_CHANGE_BADPARAM;

    ENTERMONITOR;

    lRet = DISPDEV_ChangeDisplaySettings(pDevMode, dwFlags);

    if (lRet >= 0)
    {
        WPARAM wParam;
        LPARAM lParam;

        wParam = DISPDEV_GetCaps(BITSPIXEL);
        lParam = MAKELPARAM(DISPDEV_GetCaps(HORZRES), 
            DISPDEV_GetCaps(VERTRES));
/* 这里应该改为通知消息 */
        SendMessage(HWND_BROADCAST, WM_DISPLAYCHANGE, wParam, lParam);
    }

    LEAVEMONITOR;

    return lRet;
}

/*
**  Function : EnumDisplaySettings
**  Purpose  :
**      Obtains information about one of a display device’s graphics 
**      modes. You can obtain information for all of a display device’s 
**      graphics modes by making a series of calls to this function. 
**  Params   :
**       pszDeviceName : Pointer to a null-terminated string that specifies
**                       the display device whose graphics mode the 
**                       function will obtain information about. 
**                       This parameter can be NULL. A NULL value specifies
**                       the current display device on the computer that 
**                       the calling thread is running on. 
**       dwModeNum     : Index value that specifies the graphics mode for 
**                       which information is to be obtained. 
**                       Graphics mode indexes start at zero. To obtain 
**                       information for all of a display device’s 
**                       graphics modes, make a series of calls to 
**                       EnumDisplaySettings, as follows: Set iModeNum to 
**                       zero for the first call, and increment iModeNum by
**                       one for each subsequent call. Continue calling the
**                       function until the return value is FALSE. 
**                       When you call EnumDisplaySettings with iModeNum 
**                       set to zero, the operating system initializes and
**                       caches information about the display device. When
**                       you call EnumDisplaySettings with iModeNum set to
**                       a non-zero value, the function returns the 
**                       information that was cached the last time the 
**                       function was called with iModeNum set to zero.
**       pDevMode      : Pointer to a DEVMODE structure into which the 
**                       function stores information about the specified 
**                       graphics mode. 
**  Return :
**      If the function succeeds, return TRUE; Otherwise return FALSE.
*/
BOOL WINAPI EnumDisplaySettings(PCSTR pszDeviceName, DWORD dwModeNum,
                                PDEVMODE pDevMode)
{
    BOOL bRet;

    if (!pDevMode)
        return DISP_CHANGE_BADPARAM;

    ENTERMONITOR;

    bRet = DISPDEV_EnumDisplaySettings(dwModeNum, pDevMode);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : UpdateScreen
**  Purpose  :
*/
void WINAPI UpdateScreen(void)
{
    ENTERMONITOR;

    if (g_DisplayDrv.func_mask1 & FM_CHECKCURSOR)
        g_DisplayDrv.CheckCursor(g_pDisplayDev);

    if (g_DisplayDrv.func_mask1 & FM_UPDATESCREEN)
        g_DisplayDrv.UpdateScreen(g_pDisplayDev);

    LEAVEMONITOR;

}

/**************************************************************************/
/*              International & Char Translation Support                  */
/**************************************************************************/

#ifndef NODBCS

/*
**  Function : IsDBCSLeadByte
**  Purpose  :
**      Determines whether a character is a lead byte - that is, the 
**      first byte of a character in a double-byte character set (DBCS). 
**  Params   :
**      byTestChar : Specifies the character to be tested. 
**  Return   :
**      If the character is a lead byte, the return value is nonzero. 
**      If the character is not a lead byte, the return value is zero.
*/
BOOL WINAPI IsDBCSLeadByte(int byTestChar)
{
    byTestChar &= 0xFF;
    return byTestChar >= 0xA1 && byTestChar <= 0xFC;
}

#endif // NODBCS

void*   WINAPI GetUserData(HWND hWnd)
{
    return GetWindowExtraDataPtr(hWnd);
}
