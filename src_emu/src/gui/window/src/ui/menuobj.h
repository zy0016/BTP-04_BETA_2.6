/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for menu object management.
 *            
\**************************************************************************/

#ifndef __WSOMENU_H
#define __WSOMENU_H

/* Define menu type constant */
#define MT_NORMAL       0
#define MT_POPUP        1

#define WM_REFRESHMENU  (WM_USER + 0x05A0)

#define MENU_MAGIC 0x554D
#define ISMENUUSED(pMenuObj)    ((pMenuObj)->wMagic & 0x8000)
#define SETMENUUSED(pMenuObj)   ((pMenuObj)->wMagic |= 0x8000)
#define CLRMENUUSED(pMenuObj)   ((pMenuObj)->wMagic &= 0x7FFF)
#define ISMENU(pMenuObj)        (((pMenuObj)->wMagic & 0x7FFF) == MENU_MAGIC)

// Menu item substructure
typedef struct 
{
    WORD      wFlags;           // Flags
    DWORD     hIDorPopup;       // Item ID or popup menu handle
    //RECT    rectCapture;      // Item area in screen coords. 
    WORD      xTab;             // tab position in string
    HBITMAP   hCheckedBmp;      // Bitmap to use for checked
    HBITMAP   hUncheckedBmp;    // Bitmap to use for unchecked
    HANDLE    hStrOrBmp;        // Handle to string or bitmap
    WORD      xULStart;         // Start of underline
    WORD      xULNum;           // Number of '&'
    //WORD    cxULLen;          // Length of underline (== width of char)
    WORD      cbItemLen;        // Length of item if string or NULL
} MENUITEM, *PMENUITEM;

typedef struct tagMenuObj
{
    WORD                wMagic;         // Signature/magic (0x554D == 'MU')
    WORD                wFlags;         // flags
    HANDLE              hTaskQ;         // Owning task queue handle ???
    WORD                cUsedItems;     // actual count of items
    WORD                cItems;         // Count of items
//    struct tagMenuObj * pParentMenuObj; 
    HWND                hParentMenuWin; // the parent menu of the current menu
    PMENUITEM           pItems;         // hItems
} MENUOBJ, *PMENUOBJ;

PMENUOBJ    MENU_Create(WORD wFlag);
BOOL        MENU_Destroy(PMENUOBJ pMenuObj);

BOOL        MENU_InsertMenu(PMENUOBJ pMenuObj, UINT uPosition, UINT uFlags, 
                            UINT_PTR uIDNewItem, LPCSTR lpNewItem);
BOOL        MENU_ModifyMenu(PMENUOBJ pMenuObj, UINT uPosition, UINT uFlags, 
                            UINT_PTR uIDNewItem, LPCSTR lpNewItem);
BOOL        MENU_RemoveMenu(PMENUOBJ pMenuObj, UINT uPosition, UINT uFlags);
BOOL        MENU_DeleteMenu(PMENUOBJ pMenuObj, UINT uPosition, UINT uFlags);

PMENUOBJ    MENU_GetSubMenu(PMENUOBJ pMenuObj, int nPos);

int         MENU_GetMenuItemCount(PMENUOBJ pMenuObj);
UINT        MENU_GetMenuItemID(PMENUOBJ pMenuObj, int nPos);

BOOL        MENU_SetMenuItemBitmaps(PMENUOBJ pMenuObj, UINT uPosition, 
                                    UINT uFlags, HBITMAP hBitmapUnchecked, 
                                    HBITMAP hBitmapChecked);

BOOL        MENU_EnableMenuItem(PMENUOBJ pMenuObj, UINT uIDEnableItem, 
                                UINT uEnable);
BOOL        MENU_CheckMenuItem(PMENUOBJ pMenuObj, UINT uIDCheckItem, 
                               UINT uCheck);

UINT        MENU_GetMenuState(PMENUOBJ pMenuObj, UINT uId, UINT uFlags);

int         MENU_GetMenuString(PMENUOBJ pMenuObj, UINT uIDItem, 
                               LPSTR lpString, int nMaxCount, UINT uFlag); 

BOOL        MENU_TrackPopupMenu(PMENUOBJ pMenuObj, UINT uFlags, int x, 
                                int y, int nReserved, HWND hWnd, 
                                const RECT *prcRect);
#endif //__WSOMENU_H
