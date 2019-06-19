/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements interface functions about menu called by 
 *            application.
 *            
\**************************************************************************/

#include "hpwin.h"

#if (!NOMENUS)     // Menu APIs(

#include "defwnd.h"
#include "menuobj.h"
#include "menuwnd.h"

// internal function
static PMENUOBJ MENU_LoadSubMenuIndirect(PMENUITEMTEMPLATE pMenuItemTemplate);

/*
**  Function : IsMenu
**  Purpose  :
**      Determines whether a handle is a menu handle. 
**  Params   :
**      hMenu : The handle to be tested. 
**  Return   :
**      If hMenu is a menu handle, return nonzero. 
**      If hMenu is not a menu handle, return zero. 
*/
BOOL WINAPI IsMenu(HMENU hMenu)
{
    PMENUOBJ    pMenuObj;

    pMenuObj = (PMENUOBJ)hMenu;
    if (pMenuObj == NULL)
        return FALSE;
    if (ISMENU(pMenuObj))
        return TRUE;
    else
        return FALSE;
}

/*
**  Funciton : CreateMenu
**  Purpose  :
**      The CreateMenu function creates a menu. The menu is initially
**      empty, but it can be filled with menu items by using the 
**      InsertMenuItem, AppendMenu, and InsertMenu functions. 
**  Params   :
**  Return   :
**      If the function succeeds, the return value is the handle to 
**      the newly created menu. 
**      If the function fails, the return value is NULL. 
*/
HMENU WINAPI CreateMenu(void)
{
    PMENUOBJ    pMenu;

    pMenu = MENU_Create(MT_NORMAL);
    if (!pMenu)
        return NULL;

    return (HMENU)pMenu;
}


/*
**  Funciton : CreatePopupMenu
**  Purpose  :
**      The CreatePopupMenu function creates a drop-down menu, submenu, 
**      or shortcut menu. The menu is initially empty. You can insert or
**      append menu items by using the InsertMenuItem function. You can 
**      also use the InsertMenu function to insert menu items and the 
**      AppendMenu function to append menu items. 
**  Params   :
**  Return   :
**      If the function succeeds, the return value is the handle to the 
**      newly created menu. 
**      If the function fails, the return value is NULL. 
*/
HMENU WINAPI CreatePopupMenu(void)
{
    PMENUOBJ    pMenu;

    pMenu = MENU_Create(MT_POPUP);
    if (!pMenu)
        return NULL;

    return (HMENU)pMenu;
}


/*
**  Funciton : DestroyMenu
**  Purpose  :
**      The DestroyMenu function destroys the specified menu and frees
**      any memory that the menu occupies. 
**  Params   :
**      hMenu: Identifies the menu to be destroyed. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended
**      error information, call GetLastError.
*/
BOOL WINAPI DestroyMenu(HMENU hMenu)
{
    PMENUOBJ    pMenu;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return FALSE;
    }
    pMenu = (PMENUOBJ)hMenu;

    return MENU_Destroy(pMenu);
}

/*
**  Funciton : LoadMenuIndirect
**  Purpose  :
**      The LoadMenuIndirect function loads the specified menu template
**      in memory. 
**  Params   :
**      pMenuTemplate : Points to a menu template or an extended menu
**                      template.
**  Return   :
**      If the function succeeds, the return value is the handle of the
**      menu. 
**      If the function fails, the return value is NULL. To get error 
**      information, call GetLastError
*/
HMENU WINAPI LoadMenuIndirect(const MENUTEMPLATE* pMenuTemplate)
{
    PMENUITEMTEMPLATE  pItemTemplate;
    PMENUOBJ pMenu, pSubMenu;
    int      nCnt;

    if (!pMenuTemplate)
    {
        SetLastError(1);
        return NULL;
    }

    pMenu = MENU_Create(MT_NORMAL);
    if (!IsMenu((HANDLE)pMenu))
    {
        SetLastError(1);
        return NULL;
    }

    pItemTemplate = pMenuTemplate->pMenuItems;

    nCnt = 0;
    while (!(pItemTemplate->wFlags & MF_END))
    {
        if (pItemTemplate->wFlags & MF_POPUP)   // popup submenu
        {
            // create submenu object
            pSubMenu = MENU_LoadSubMenuIndirect(pItemTemplate->pMenuItems);
            if (!pSubMenu)
            {
                SetLastError(1);
                return NULL;
            }

            MENU_InsertMenu(pMenu, nCnt,  
                pItemTemplate->wFlags | MF_BYPOSITION, 
                (UINT_PTR)(HANDLE)pSubMenu, 
                pItemTemplate->lpszItemName
                );
        }
        else
        {
            MENU_InsertMenu(pMenu, nCnt,  
                pItemTemplate->wFlags | MF_BYPOSITION, 
                (UINT_PTR)pItemTemplate->wID, 
                pItemTemplate->lpszItemName
                );
        }

        nCnt++;
        pItemTemplate++;
    }


    return (HMENU)pMenu;
}

#ifndef NOMENUAPI

/*
**  Funciton : GetMenu
**  Purpose  :
**      The GetMenu function retrieves the handle of the menu assigned
**      to the given window.  
**  Params   :
**      hWnd    : Identifies the window whose menu handle is retrieved. 
**  Return   :
**      If the function succeeds, the return value is the handle of the
**      menu. If the given window has no menu, the return value is NULL.
**      If the window is a child window, the return value is undefined. 
*/
HMENU WINAPI GetMenu(HWND hWnd)
{
    HWND hwndMenu;

    hwndMenu = DEFWND_GetMenuCtrl(hWnd);
    if (!hwndMenu)
    {
        SetLastError(1);
        return NULL;
    }

    return (HMENU)GetWindowLong(hWnd, GWL_ID);
}

/*
**  Funciton : SetMenu
**  Purpose  :
**      The SetMenu function assigns a new menu to the specified window.  
**  Params   :
**      hWnd    : Identifies the window to which the menu is to be 
**                assigned.
**      hMenu   : Identifies the new menu. If this parameter is NULL, the 
**                window¡¯s current menu is removed. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended 
**      error information, call GetLastError.
*/
BOOL WINAPI SetMenu(HWND hWnd, HMENU hMenu)
{
    HWND hwndMenu;

    hwndMenu = DEFWND_GetMenuCtrl(hWnd);
    if (!hwndMenu)
    {
        SetLastError(1);
        return FALSE;
    }

    SetWindowLong(hWnd, GWL_ID, (LONG)hMenu);
    SendMessage(hwndMenu, WM_SETMENU, 0, (LPARAM)hMenu);

    return TRUE;
}

/*
**  Funciton : DrawMenuBar
**  Purpose  :
**      The DrawMenuBar function redraws the menu bar of the specified
**      window.
**      If the menu bar changes after Windows has created the window, 
**      this function must be called to draw the changed menu bar.  
**  Params   :
**      hWnd    : Identifies the window whose menu bar needs redrawing.
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended
**      error information, call GetLastError
*/
BOOL WINAPI DrawMenuBar(HWND hWnd)
{
    hWnd = DEFWND_GetMenuCtrl(hWnd);
    if (!hWnd)
    {
        SetLastError(1);
        return FALSE;
    }

    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);

    return TRUE;
}

/*
**  Funciton : InsertMenu
**  Purpose  :
**      The InsertMenu function inserts a new menu item into a menu, moving
**      other items down the menu. 
**  Params   :
**      hMenu     : Identifies the menu to be changed. 
**      uPosition : Specifies the menu item before which the new menu item 
**                  is to be inserted, as determined by the uFlags parameter.
**      uFlags    : Specifies flags that control the interpretation of the
**                  uPosition parameter and the content, appearance, and 
**                  behavior of the new menu item. This parameter must be 
**                  a combination of one of the following required values
**                  and at least one of the values listed in the following 
**                  Remarks section.
**          MF_BYCOMMAND  Indicates that uItemHilite gives the identifier 
**                        of the menu item. 
**          MF_BYPOSITION Indicates that uItemHilite gives the zero-based
**                        relative position of the menu item. 
**
**      uIDNewItem: Specifies either the identifier of the new menu item
**                  or, if the uFlags parameter has the MF_POPUP flag set,
**                  the handle of the drop-down menu or submenu. 
**      lpNewItem : Specifies the content of the new menu item. The 
**                  interpretation of lpNewItem depends on whether the 
**                  uFlags parameter includes the MF_BITMAP, MF_OWNERDRAW,
**                  or MF_STRING flag, as follows:
**          MF_BITMAP  Contains a bitmap handle. 
**          MF_STRING  Contains a pointer to a null-terminated string
**                     (the default). 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended
**      error information, call GetLastError. 
*/
BOOL WINAPI InsertMenu(HMENU hMenu, UINT uPosition, 
					   UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem)
{
    PMENUOBJ    pMenu;
    BOOL        bInsert;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return FALSE;
    }
    pMenu = (PMENUOBJ)hMenu;

    if (!lpNewItem && !(uFlags & MF_SEPARATOR))
    {
        SetLastError(1);
        return FALSE;
    }

    bInsert = MENU_InsertMenu(pMenu, uPosition, uFlags, uIDNewItem, 
        lpNewItem);
    return bInsert;

}

/*
**  Funciton : AppendMenu
**  Purpose  :
**      The AppendMenu function appends a new item to the end of the 
**      specified menu bar, drop-down menu, submenu, or shortcut menu.
**      You can use this function to specify the content, appearance, 
**      and behavior of the menu item. 
**  Params   :
**      hMenu     : Identifies the menu to be changed. 
**      uFlags    : Specifies flags that control the interpretation of 
**                  the uPosition parameter and the content, appearance, 
**                  and behavior of the new menu item. This parameter must
**                  be a combination of one of the following required values
**                  and at least one of the values listed in the following 
**                  Remarks section.
**          MF_BYCOMMAND  Indicates that uItemHilite gives the identifier 
**                        of the menu item. 
**          MF_BYPOSITION Indicates that uItemHilite gives the zero-based
**                        relative position of the menu item. 
**      uIDNewItem: Specifies either the identifier of the new menu item or,
**                  if the uFlags parameter has the MF_POPUP flag set, the
**                  handle of the drop-down menu or submenu. 
**      lpNewItem : Specifies the content of the new menu item. The 
**                  interpretation of lpNewItem depends on whether the 
**                  uFlags parameter includes the MF_BITMAP, MF_OWNERDRAW,
**                  or MF_STRING flag, as follows:
**          MF_BITMAP  Contains a bitmap handle. 
**          MF_STRING  Contains a pointer to a null-terminated string
**                     (the default). 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended
**      error information, call GetLastError. 
*/
BOOL WINAPI AppendMenu(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, 
                       LPCSTR lpNewItem)
{
    PMENUOBJ    pMenu;
    int         nCount;
    BOOL        bInsert;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return FALSE;
    }
    pMenu = (PMENUOBJ)hMenu;

    if (!lpNewItem  && !(uFlags & MF_SEPARATOR))
    {
        SetLastError(1);
        return FALSE;
    }

    nCount = MENU_GetMenuItemCount(pMenu);
    if (nCount == -1)
    {
        SetLastError(1);
        return FALSE;
    }

    bInsert =  MENU_InsertMenu(pMenu, nCount, uFlags | MF_BYPOSITION, 
        uIDNewItem, lpNewItem);
    return bInsert;

}

/*
**  Funciton : ModifyMenu
**  Purpose  :
**      The ModifyMenu function changes an existing menu item. This
**      function is used to specify the content, appearance, and 
**      behavior of the menu item.  
**  Params   :
**      hMenu     : Identifies the menu to be changed. 
**      uPosition : Specifies the menu item to be changed, as determined by
**                  the uFlags parameter.
**      uFlags    : Specifies flags that control the interpretation of the 
**                  uPosition parameter and the content, appearance, and 
**                  behavior of the menu item. This parameter must be a 
**                  combination of one of the following
**                  required values and at least one of the values listed in
**                  the following Remarks section. 
**          MF_BYCOMMAND  Indicates that uItemHilite gives the identifier of
**                        the menu item. 
**          MF_BYPOSITION Indicates that uItemHilite gives the zero-based 
**                        relative position of the menu item. 
**
**      uIDNewItem: Specifies either the identifier of the new menu item or, 
**                  if the uFlags parameter has the MF_POPUP flag set, the 
**                  handle of the drop-down menu or submenu. 
**      lpNewItem : Specifies the content of the new menu item. The 
**                  interpretation of lpNewItem depends on whether the 
**                  uFlags parameter includes the MF_BITMAP, MF_OWNERDRAW, 
**                  or MF_STRING flag, as follows:
**          MF_BITMAP  Contains a bitmap handle. 
**          MF_STRING  Contains a pointer to a null-terminated string 
**                    (the default). 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended 
**      error information, call GetLastError. 
*/
BOOL WINAPI ModifyMenu(HMENU hMenu, UINT uPosition, 
					   UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem)
{
    PMENUOBJ    pMenu;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return FALSE;
    }
    pMenu = (PMENUOBJ)hMenu;

    if (!lpNewItem  && !(uFlags & MF_SEPARATOR))
    {
        SetLastError(1);
        return FALSE;
    }

    return MENU_ModifyMenu(pMenu, uPosition, 
					       uFlags, uIDNewItem, lpNewItem);
}

/*
**  Funciton : RemoveMenu
**  Purpose  :
**      The RemoveMenu function deletes a menu item from the specified 
**      menu. If the menu item opens a drop-down menu or submenu, 
**      RemoveMenu does not destroy the menu or its handle, allowing 
**      the menu to be reused. Before this function is called, the 
**      GetSubMenu function should retrieve the handle of the drop-down
**      menu or submenu.  
**  Params   :
**      hMenu     : Identifies the menu to be changed. 
**      uPosition : Specifies the menu item to be deleted, as determined
**                  by the uFlags parameter. 
**      uFlags    : Specifies flags that control the interpretation of the
**                  uPosition parameter and the content, appearance, and 
**                  behavior of the new menu item. This parameter must be
**                  a combination of one of the following required values
**                  and at least one of the values listed in the following
**                  Remarks section.
**          MF_BYCOMMAND  Indicates that uItemHilite gives the identifier 
**                        of the menu item. 
**          MF_BYPOSITION Indicates that uItemHilite gives the zero-based
**                        relative position of the menu item. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended 
**      error information, call GetLastError. 
*/
BOOL WINAPI RemoveMenu(HMENU hMenu, UINT uPosition, UINT uFlags)
{
    PMENUOBJ    pMenu;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return FALSE;
    }
    pMenu = (PMENUOBJ)hMenu;
    

    return MENU_RemoveMenu(pMenu, uPosition, uFlags);
}

/*
**  Funciton : DeleteMenu
**  Purpose  :
**      The DeleteMenu function deletes an item from the specified menu.
**      If the menu item opens a menu or submenu, this function destroys
**      the handle to the menu or submenu and frees the memory used by 
**      the menu or submenu.   
**  Params   :
**      hMenu     : Identifies the menu to be changed. 
**      uPosition : Specifies the menu item to be deleted, as determined
**                  by the uFlags parameter. 
**      uFlags    : Specifies flags that control the interpretation of the
**                  uPosition parameter and the content, appearance, and
**                  behavior of the new menu item. This parameter must be 
**                  a combination of one of the following required values
**                  and at least one of the values listed in the following 
**                  Remarks section.
**          MF_BYCOMMAND  Indicates that uItemHilite gives the identifier
**                        of the menu item. 
**          MF_BYPOSITION Indicates that uItemHilite gives the zero-based 
**                        relative position of the menu item. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended 
**      error information, call GetLastError. 
*/
BOOL WINAPI DeleteMenu(HMENU hMenu, UINT uPosition, UINT uFlags)
{
    PMENUOBJ    pMenu;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return FALSE;
    }
    pMenu = (PMENUOBJ)hMenu;

    return MENU_DeleteMenu(pMenu, uPosition, uFlags);
}

/*
**  Funciton : ChangeMenu
**  Purpose  : Insert, modify, append, remove and delete a menu item.
**  Params   :
**      hMenu     : Identifies the menu to be changed. 
**      cmd       : Specifies the menu item to be changed, as determined
**                  by the uFlags parameter. 
**      lpNewItem : Specifies the content of the new menu item. The 
**                  interpretation of lpNewItem depends on whether the 
**                  uFlags parameter includes: 
**          MF_BITMAP  Contains a bitmap handle. 
**          MF_STRING  Contains a pointer to a null-terminated string 
**                     (the default).
**
**      cmdInsert : Include the following flags:
**          MF_INSERT:  Insert a new menu item.
**          MF_CHANGE:  Modify the existed menu item.
**          MF_APPEND:  Append a new menu item to the end of the specified
**                      menu.
**          MF_DELETE:  Delete a menu item and release the memory.
**          MF_REMOVE:  remove a menu item.
**
**      flags     : Specifies flags that control the interpretation of the
**                  uPosition parameter and the content, appearance, and
**                  behavior of the new menu item. This parameter must be 
**                  a combination of one of the following required values
**                  and at least one of the values listed in the following 
**                  Remarks section.
**          MF_BYCOMMAND  Indicates that uItemHilite gives the identifier
**                        of the menu item. 
**          MF_BYPOSITION Indicates that uItemHilite gives the zero-based 
**                        relative position of the menu item. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended 
**      error information, call GetLastError. 
*/
BOOL WINAPI ChangeMenu(HMENU hMenu, UINT cmd, LPCSTR lpszNewItem, 
                       UINT cmdInsert, UINT flags)
{
    PMENUOBJ    pMenu;
    int         nCount;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return FALSE;
    }
    pMenu = (PMENUOBJ)hMenu;

    // find the position of the specified menu string
	if (cmdInsert == MF_INSERT)
        return MENU_InsertMenu(pMenu, cmd, flags, cmdInsert, lpszNewItem);

    if (cmdInsert & MF_CHANGE)
        return MENU_ModifyMenu(pMenu, cmd, flags, cmdInsert, lpszNewItem);

    if (cmdInsert & MF_APPEND)
    {
        nCount = MENU_GetMenuItemCount(pMenu);
        if (nCount == -1)
        {
            SetLastError(1);
            return FALSE;
        }

        return MENU_InsertMenu(pMenu, nCount, flags | MF_BYPOSITION, 
            cmdInsert, lpszNewItem);
    }

    if (cmdInsert & MF_DELETE)
        return MENU_DeleteMenu(pMenu, cmd, flags);

    if (cmdInsert & MF_REMOVE)
        return MENU_RemoveMenu(pMenu, cmd, flags);

    return FALSE;
}

/*
**  Funciton : EnableMenuItem
**  Purpose  :
**      The EnableMenuItem function enables, disables, or grays the s
**      pecified menu item.    
**  Params   :
**      hMenu         : Identifies the menu. 
**      uIDEnableItem : Specifies the menu item to be enabled, disabled,
**                      or grayed, as determined by the uEnable parameter. 
**                      This parameter specifies an item in a menu bar, 
**                      menu, or submenu. 
**      uEnable       : Specifies flags that control the interpretation of
**                      the uIDEnableItem parameter and indicate whether 
**                      the menu item is  enabled, disabled, or grayed. 
**                      This parameter must be a combination of either 
**                      MF_BYCOMMAND or MF_BYPOSITION and MF_ENABLED, 
**                      MF_DISABLED, or MF_GRAYED. 
**          MF_BYCOMMAND  Indicates that uItemHilite gives the identifier 
**                        of the menu item. 
**          MF_BYPOSITION Indicates that uItemHilite gives the zero-based 
**                        relative position of the menu item. 
**          MF_DISABLED   Indicates that the menu item is disabled, but not
**                        grayed, so it cannot be selected. 
**          MF_ENABLED    Indicates that the menu item is enabled and 
**                        restored from a grayed state so that it can be
**                        selected. 
**          MF_GRAYED     Indicates that the menu item is disabled and 
**                        grayed so that it cannot be selected. 
**  Return   :
**      The return value specifies the previous state of the menu item 
**      (it is either MF_DISABLED, MF_ENABLED, or MF_GRAYED). If the menu
**      item does not exist, the return value is 0xFFFFFFFF. 
*/
BOOL WINAPI EnableMenuItem(HMENU hMenu, UINT uIDEnableItem, UINT uEnable)
{
    PMENUOBJ    pMenu;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return FALSE;
    }
    pMenu = (PMENUOBJ)hMenu;

    return MENU_EnableMenuItem(pMenu, uIDEnableItem, uEnable);
}

/*
**  Funciton : CheckMenuItem
**  Purpose  :
**      The CheckMenuItem function sets the state of the specified menu
**      item's check mark attribute to either checked or unchecked.    
**  Params   :
**      hMenu         : Identifies the menu. 
**      uIDCheckItem  : Specifies the menu item whose check-mark attribute
**                      is to be set, as determined by the uCheck parameter.  
**      uCheck        : Specifies flags that control the interpretation of 
**                      the uIDCheckItem parameter and the state of the menu
**                      item¡¯s check-mark attribute. This parameter can be 
**                      a combination of either MF_BYCOMMAND, or MF_BYPOSITION
**                      and MF_CHECKED or MF_UNCHECKED.  
**          MF_BYCOMMAND  Indicates that uItemHilite gives the identifier of
**                        the menu item. 
**          MF_BYPOSITION Indicates that uItemHilite gives the zero-based 
**                        relative position of the menu item. 
**          MF_CHECKED    Sets the check-mark attribute to the checked state. 
**          MF_UNCHECKED  Sets the check-mark attribute to the unchecked state. 
**  Return   :
**      The return value specifies the previous state of the menu item 
**      (either MF_CHECKED or MF_UNCHECKED). If the menu item does not exist, 
**      the return value is 0xFFFFFFFF. 
*/
BOOL WINAPI CheckMenuItem(HMENU hMenu, UINT uIDCheckItem, UINT uCheck)
{
    PMENUOBJ    pMenu;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return FALSE;
    }
    pMenu = (PMENUOBJ)hMenu;

    return MENU_CheckMenuItem(pMenu, uIDCheckItem, uCheck);
}

/*
**  Funciton : GetSubMenu
**  Purpose  :
**      The GetSubMenu function retrieves the handle of the drop-down
**      menu or submenu activated by the specified menu item.    
**  Params   :
**      hMenu  : Identifies the menu. 
**      nPos   : Specifies the zero-based relative position in the 
**               given menu of an item that activates a drop-down 
**               menu or submenu. 
**  Return   :
**      If the function succeeds, the return value is the handle 
**      of the drop-down menu or submenu activated by the menu item.
**      If the menu item does not activate a drop-down menu or submenu,
**      the return value is NULL
*/
HMENU WINAPI GetSubMenu(HMENU hMenu, int nPos)
{
    PMENUOBJ    pMenu, pSubMenu;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return FALSE;
    }
    pMenu = (PMENUOBJ)hMenu;

    pSubMenu = MENU_GetSubMenu(pMenu, nPos);
    if (!pSubMenu)
        return NULL;

    return (HMENU)pSubMenu;
}

/*
**  Funciton : GetMenuItemCount
**  Purpose  :
**      The GetMenuItemCount function determines the number of items in 
**      the specified menu.   
**  Params   :
**      hMenu  : Identifies the menu. 
**  Return   :
**      If the function succeeds, the return value specifies the number
**      of items in the menu. 
**      If the function fails, the return value is -1. To get extended
**      error information, call GetLastError
*/
int WINAPI GetMenuItemCount(HMENU hMenu)
{
    PMENUOBJ    pMenu;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return 0;
    }
    pMenu = (PMENUOBJ)hMenu;

    return MENU_GetMenuItemCount(pMenu);
}

/*
**  Funciton : GetMenuItemID
**  Purpose  :
**      The GetMenuItemID function retrieves the menu item identifier of a
**      menu item located at the specified position in a menu.   
**  Params   :
**      hMenu  : Identifies the menu. 
**      nPos   : Specifies the zero-based relative position of the menu 
**               item whose
**               identifier is to be retrieved. 
**  Return   :
**      If the function succeeds, the return value specifies the identifier
**      of the given menu item. If the menu item identifier is NULL or if
**      the specified item opens a submenu, the return value is 0xFFFFFFFF. 
*/
UINT WINAPI GetMenuItemID(HMENU hMenu, int nPos)
{
    PMENUOBJ    pMenu;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return (UINT)-1;
    }
    pMenu = (PMENUOBJ)hMenu;

    return MENU_GetMenuItemID(pMenu, nPos);
}

/*
**  Funciton : GetMenuString
**  Purpose  :
**      The GetMenuString function copies the text string of the specified
**      menu item into the specified buffer.   
**  Params   :
**      hMenu    : Identifies the menu. 
**      uIDItem  : Specifies the menu item to be changed, as determined by 
**                 the uFlag parameter. 
**      lpString : Points to the buffer that is to receive the 
**                 null-terminated string. 
**                 If lpString is NULL, the function returns the length of
**                 the menu string. 
**      nMaxCount: Specifies the maximum length, in characters, of the 
**                 string to be copied. 
**                 If the string is longer than the maximum specified in 
**                 the nMaxCount parameter, the extra characters are 
**                 truncated. 
**                 If nMaxCount is 0, the function returns the length of 
**                 the menu string. 
**      uFlag    : Specifies how the uIDItem parameter is interpreted. This 
**                 parameter must be one of the following values: 
**          MF_BYCOMMAND  Indicates that uIDItem gives the identifier of 
**                        the menu item. 
**                        If neither the MF_BYCOMMAND nor MF_BYPOSITION 
**                        flag is specified,
**                        the MF_BYCOMMAND flag is the default flag. 
**          MF_BYPOSITION Indicates that uIDItem gives the zero-based 
**                        relative position of the menu item. 
**  Return   :
**      If the function succeeds, the return value specifies the number of 
**      characters copied to the buffer, not including the terminating null
**      character. 
**      If the function fails, the return value is zero. 
*/
int WINAPI GetMenuString(HMENU hMenu, UINT uIDItem, LPSTR lpString, 
                         int nMaxCount, UINT uFlag)
{
    PMENUOBJ    pMenu;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return 0;
    }
    pMenu = (PMENUOBJ)hMenu;

    return MENU_GetMenuString(pMenu, uIDItem, lpString, nMaxCount, uFlag);
}

/*
**  Funciton : GetMenuState
**  Purpose  :
**      The GetMenuState function retrieves the menu flags associated with
**      the specified menu item. If the menu item opens a submenu, this 
**      function also returns the number of items in the submenu.
**  Params   :
**      hMenu    : Identifies the menu that contains the menu item whose 
**                 flags are to be retrieved. 
**      uId      : Specifies the menu item for which the menu flags are to 
**                 be retrieved, as determined by the uFlags parameter
**      uFlags   : Specifies how the uIDItem parameter is interpreted. This 
**                 parameter must be one of the following values: 
**          MF_BYCOMMAND  Indicates that uIDItem gives the identifier of 
**                        the menu item.
**                        If neither the MF_BYCOMMAND nor MF_BYPOSITION 
**                        flag is specified, the MF_BYCOMMAND flag is the 
**                        default flag. 
**          MF_BYPOSITION Indicates that uIDItem gives the zero-based 
**                        relative position of the menu item. 
**  Return   :
**      If the specified item does not exist, the return value is 
**      0xFFFFFFFF. 
**      If the menu item opens a submenu, the low-order byte of the return
**      value contains the menu flags associated with the item, and the 
**      high-order byte contains the number of items in the submenu opened
**      by the item. 
*/
UINT WINAPI GetMenuState(HMENU hMenu, UINT uId, UINT uFlags)
{
    PMENUOBJ    pMenu;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return (UINT)-1;
    }
    pMenu = (PMENUOBJ)hMenu;


    return MENU_GetMenuState(pMenu, uId, uFlags);
}

/*
**  Funciton : SetMenuItemBitmaps
**  Purpose  :
**      The SetMenuItemBitmaps function associates the specified bitmap 
**      with a menu item. Whether the menu item is checked or unchecked,
**      Windows displays the appropriate bitmap next to the menu item. 
**  Params   :
**      hMenu    : Identifies the menu containing the item to receive new 
**                 check-mark bitmaps. 
**      uPosition: Specifies the menu item to be changed, as determined by 
**                 the uFlags parameter. 
**      uFlags   : Specifies how the uPosition parameter is interpreted. 
**                 The uFlags parameter must be one of the following 
**                 values. 
**          MF_BYCOMMAND  Indicates that uIDItem gives the identifier of 
**                        the menu item. If neither the MF_BYCOMMAND nor
**                        MF_BYPOSITION flag is specified, the MF_BYCOMMAND
**                        flag is the default flag. 
**          MF_BYPOSITION Indicates that uIDItem gives the zero-based 
**                        relative position of the menu item. 
**
**      hBitmapUnchecked: Identifies the bitmap displayed when the menu
**                        item is not checked.
**      hBitmapChecked  : Identifies the bitmap displayed when the menu
**                        item is checked.
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended
**      error information, call GetLastError. 
*/
BOOL WINAPI SetMenuItemBitmaps(HMENU hMenu, UINT uPosition, UINT uFlags, 
							   HBITMAP hBitmapUnchecked, 
                               HBITMAP hBitmapChecked)
{
    PMENUOBJ    pMenu;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return FALSE;
    }
    pMenu = (PMENUOBJ)hMenu;


    return MENU_SetMenuItemBitmaps(pMenu, uPosition, uFlags, 
        hBitmapUnchecked, hBitmapChecked);
}

/*
**  Function : HiliteMenuItem
**  Purpose  :
**      Highlights or removes the highlighting from an item in a menu bar. 
*/
BOOL WINAPI HiliteMenuItem(HWND hwnd, HMENU hmenu, UINT uItemHilite,
                           UINT uHilite)
{
    return FALSE;
}

/*
**  Function : GetMenuCheckMarkDimensions
**  Purpose  :
**      
*/
DWORD WINAPI GetMenuCheckMarkDimensions(void)
{
    return 0;
}

#endif // NOMENUAPI

/*
**  Funciton : TrackPopupMenu
**  Purpose  :
**      The TrackPopupMenu function displays a shortcut menu at the 
**      specified location and tracks the selection of items on the menu.
**      The shortcut menu can appear anywhere on the screen.  
**  Params   :
**      hMenu    : Identifies the shortcut menu to be displayed. The handle
**                 can be obtained by calling CreatePopupMenu to create a 
**                 new shortcut menu, or by calling GetSubMenu to retrieve
**                 the handle of a submenu associated with an existing menu
**                 item.  
**      uFlags   : A set of bit flags that specify function options.
**          TPM_CENTERALIGN If this flag is set, the function centers the 
**                          shortcut menu horizontally relative to the 
**                          coordinate specified by the x parameter
**          TPM_LEFTALIGN   If this flag is set, the function positions the
**                          shortcut menu so that its left side is aligned 
**                          with the coordinate specified by the x 
**                          parameter
**          TPM_RIGHTALIGN  Positions the shortcut menu so that its right
**                          side is aligned with the coordinate specified
**                          by the x parameter. 
**      x        : Specifies the horizontal location of the shortcut menu,
**                 in screen coordinates. 
**      y        : Specifies the vertical location of the shortcut menu, 
**                 in screen coordinates. 
**      nReserved: Reserved; must be zero. 
**      hWnd     : Identifies the window that owns the shortcut menu. This
**                 window receives all messages from the menu. The window
**                 does not receive a WM_COMMAND message from the menu 
**                 until the function returns. 
**      prcRect  : Points to a RECT structure that specifies the portion 
**                 of the screen in which the user can select without 
**                 dismissing the shortcut menu. If this parameter is NULL,
**                 the shortcut menu is dismissed if the user clicks 
**                 outside the shortcut menu. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended
**      error information, call GetLastError. 
*/
BOOL WINAPI TrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, 
                           int nReserved, HWND hWnd, CONST RECT *prcRect)
{
    PMENUOBJ    pMenu;

    if (!IsMenu(hMenu))
    {
        SetLastError(1);
        return FALSE;
    }
    pMenu = (PMENUOBJ)hMenu;

    return MENU_TrackPopupMenu(pMenu, uFlags, x, y, nReserved, hWnd, 
        prcRect);
}


/*
*  load submenu from MENUITEMTEMPLATE
*/

static PMENUOBJ MENU_LoadSubMenuIndirect(PMENUITEMTEMPLATE pMenuItemTemplate)
{
    PMENUOBJ pMenu, pSubMenu;
    int      nCnt;

    if (!pMenuItemTemplate)
    {
        SetLastError(1);
        return NULL;
    }

    pMenu = MENU_Create(MT_NORMAL);
    if (!pMenu)
    {
        SetLastError(1);
        return NULL;
    }

    nCnt = 0;
    while (!(pMenuItemTemplate->wFlags & MF_END))
    {
        if (pMenuItemTemplate->wFlags & MF_POPUP)   // popup submenu
        {
            pSubMenu = MENU_LoadSubMenuIndirect(
                pMenuItemTemplate->pMenuItems);

            if (!pSubMenu)
            {
                SetLastError(1);
                return NULL;
            }

            MENU_InsertMenu(pMenu, nCnt,  
                pMenuItemTemplate->wFlags | MF_BYPOSITION, 
                (UINT_PTR)(HANDLE)pSubMenu, 
                pMenuItemTemplate->lpszItemName
                );
        }
        else
        {
            MENU_InsertMenu(pMenu, nCnt,  
                pMenuItemTemplate->wFlags | MF_BYPOSITION, 
                (UINT_PTR)pMenuItemTemplate->wID, 
                pMenuItemTemplate->lpszItemName
                );
        }

        nCnt++;
        pMenuItemTemplate++;
    }

    return pMenu;
}

#else   // NOMENUS

static void UnusedFunc(void)
{
}

#endif  // NOMENUS
