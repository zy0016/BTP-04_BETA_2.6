/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Functions used for managing menu object.
 *            
\**************************************************************************/

#include "hpwin.h"

#if (!NOMENUS)

#include "string.h"
#include "menuobj.h"

#ifdef WINDEBUG
#define OBJ_MENU    11      /* Menu Object          */
extern void WINDEBUG_AddObj(int uType);
extern void WINDEBUG_RemoveObj(int uType);
#endif

static void MENU_FindHotTab(LPCSTR lpNewItem, WORD *wHotPos, 
                              WORD *wTabPos, WORD *wHotNum);

//static BOOL MENU_GetItemDimension(PMENUITEM pItemObj, PSIZE pSize);

/*
*   Creates a menu object.
*   If the function succeeds, return the pointer to the menu.
*   If the function fails, return NULL.
*/
PMENUOBJ MENU_Create(WORD wFlag)
{
    PMENUOBJ	pMenu;

    pMenu = (PMENUOBJ)MemAlloc(sizeof(MENUOBJ));

    if (!pMenu)
    {
        SetLastError(1);
        return NULL;
    }

    pMenu->wMagic = MENU_MAGIC;

    pMenu->wFlags = wFlag;
	pMenu->hTaskQ = 0;
	//pMenu->cxWidth = 0;
	//pMenu->cyHeight = 0;
	pMenu->cUsedItems = 0;
	pMenu->cItems = 0;
	//pMenu->hWin = NULL;
    pMenu->hParentMenuWin = NULL;
	pMenu->pItems = NULL;

#ifdef WINDEBUG
    WINDEBUG_AddObj(OBJ_MENU);
#endif

    return pMenu;
}


/*
**  Function : MENU_Destroy
**  Purpose  :
**      Destroys the specified menu.
**  Params   :
**      pCursor : Specifies the cursor to be destroyed.
*/
BOOL MENU_Destroy(PMENUOBJ pMenuObj)
{
    PMENUOBJ    pSubMenu;
    int         i;

    ASSERT(pMenuObj != NULL);

    if (!IsMenu((HMENU)pMenuObj))
        return FALSE;

    for (i = 0; i < pMenuObj->cUsedItems; i++)
    {
        if (pMenuObj->pItems[i].wFlags & MF_POPUP)
        {
            pSubMenu = (PMENUOBJ)(HANDLE)pMenuObj->pItems[i].hIDorPopup;
            MENU_Destroy(pSubMenu);
        }
    }


    // Free the menu object items and itself
    for (i = 0; i < pMenuObj->cUsedItems; i++)
    {
        if (!(pMenuObj->pItems[i].wFlags & MF_BITMAP) && 
            !(pMenuObj->pItems[i].wFlags & MF_OWNERDRAW) && 
            !(pMenuObj->pItems[i].wFlags & MF_SEPARATOR) && 
            (pMenuObj->pItems[i].wFlags & MF_NOTUSERSTRING))
            
            MemFree(pMenuObj->pItems[i].hStrOrBmp);
    }

    pMenuObj->wMagic = 0;
    MemFree(pMenuObj->pItems);
    MemFree(pMenuObj);

#ifdef WINDEBUG
    WINDEBUG_RemoveObj(OBJ_MENU);
#endif
    return TRUE;
}

/*
*  insert a new menu item to the specified menu.
*/
BOOL MENU_InsertMenu(PMENUOBJ pMenuObj, UINT uPosition, 
					 UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem)
{
	int		    i;
	PMENUITEM   pItems;
    PMENUOBJ    pMenu = NULL; //, pSubMenu;
    HANDLE      handle;
    UINT        uPos;
    WORD        wHotPos, wTabPos, wHotNum;
    int         slen, nIndex = 0;
    HWND        hwndMenu = NULL;
    char        ClassName[10];
	//WORD	    wMaxWidth, wMaxHeight;
    //SIZE      itemSize;

	ASSERT(pMenuObj != NULL);

    hwndMenu = GetCapture();
    GetClassName(hwndMenu, ClassName, 10);
    if (!strcmp(ClassName, "#MENU"))
    {
        pMenu = (PMENUOBJ)GetWindowLong(hwndMenu, GWL_ID);
        while (pMenu != pMenuObj)
        {
            hwndMenu = pMenu->hParentMenuWin;
            if (hwndMenu == NULL)
                break;

            pMenu = (PMENUOBJ)GetWindowLong(hwndMenu, GWL_ID);
        }
        if (hwndMenu != NULL)
        {
            SendMessage(hwndMenu, WM_REFRESHMENU, (WPARAM)FALSE, (LPARAM)&nIndex);
        }
    }

    if (uFlags & MF_POPUP)
    {
        if (!IsMenu((HMENU)uIDNewItem))
            return FALSE;
        else if (ISMENUUSED((PMENUOBJ)uIDNewItem))
            return FALSE;
        else
            SETMENUUSED((PMENUOBJ)uIDNewItem);
    }
    
    // find the position of the specified menu string
	if (uFlags & MF_BYPOSITION)	   // uPosition is zero-based position 
	{
        if (uPosition + 1 > pMenuObj->cUsedItems)
            uPos = pMenuObj->cUsedItems;
		else
	        uPos = uPosition;
	}
    else    // uPosition is the identifier
    {
        for (i=0; i<pMenuObj->cUsedItems; i++)
		{
			if (pMenuObj->pItems[i].hIDorPopup == uPosition)
				break;

            if (pMenuObj->pItems[i].wFlags & MF_POPUP)  // is submenu
            {
                handle = (HANDLE)pMenuObj->pItems[i].hIDorPopup;
                if (!IsMenu((HMENU)handle))
                    return FALSE;
                pMenu = (PMENUOBJ)handle;

                if (MENU_InsertMenu(pMenu, uPosition, uFlags, uIDNewItem, lpNewItem))
                    return TRUE;
            }
		}

        if (i == pMenuObj->cUsedItems)
            //uPos = pMenuObj->cUsedItems;
            return FALSE;
		else
			uPos = i; 
    }

    // malloc the new space if the space is lack.
    if (pMenuObj->cUsedItems + 1 > pMenuObj->cItems)
    {
	    pItems = (MENUITEM *)MemAlloc((pMenuObj->cItems + 5) * sizeof(MENUITEM));

		if (!pItems)
		{
			SetLastError(1);
            return FALSE;
		}

        if (pMenuObj->pItems)
        {
            memcpy(pItems, pMenuObj->pItems, (pMenuObj->cItems) * sizeof(MENUITEM));
            MemFree(pMenuObj->pItems);
        }

        pMenuObj->pItems = pItems;
		pMenuObj->cItems += 5;
    }

	memmove(&pMenuObj->pItems[uPos + 1], &pMenuObj->pItems[uPos], 
            (pMenuObj->cUsedItems - uPos) * sizeof(MENUITEM));

	pMenuObj->pItems[uPos].wFlags = uFlags;
    pMenuObj->pItems[uPos].xULStart = 0;
    pMenuObj->pItems[uPos].xTab = 0;
    pMenuObj->pItems[uPos].hCheckedBmp = NULL;
    pMenuObj->pItems[uPos].hUncheckedBmp = NULL;
	pMenuObj->pItems[uPos].hIDorPopup = uIDNewItem;

    if (!(uFlags & MF_BITMAP) && !(uFlags & MF_OWNERDRAW) 
        && !(uFlags & MF_SEPARATOR) && (uFlags & MF_NOTUSERSTRING))
    {
        slen = strlen(lpNewItem);
        pMenuObj->pItems[uPos].hStrOrBmp = MemAlloc((slen + 1) * sizeof(char));
        strcpy(pMenuObj->pItems[uPos].hStrOrBmp, lpNewItem);
    }
    else
        pMenuObj->pItems[uPos].hStrOrBmp = (HANDLE)lpNewItem;
   
    if (!lpNewItem || (uFlags & MF_BITMAP))
		pMenuObj->pItems[uPos].cbItemLen = 0;
	else
    {
        // get the position of the hot key and the tab key
        MENU_FindHotTab(lpNewItem, &wHotPos, &wTabPos, &wHotNum);

        pMenuObj->pItems[uPos].xULStart = wHotPos;
        pMenuObj->pItems[uPos].xULNum = wHotNum;
        pMenuObj->pItems[uPos].xTab = wTabPos;
		pMenuObj->pItems[uPos].cbItemLen = strlen(lpNewItem);
    }

    pMenuObj->cUsedItems++;

    if (hwndMenu != NULL)
    {
        if (uPosition <= (UINT)nIndex)
            nIndex++;

        SendMessage(hwndMenu, WM_REFRESHMENU, (WPARAM)TRUE, (LPARAM)nIndex);
    }

    /*

    if (uFlags & MF_POPUP)  // is submenu
    {
        pSubMenu = (PMENUOBJ)WOT_GetObj((HANDLE)uIDNewItem, OBJ_MENU);
        if (!pSubMenu)
        {
            SetLastError(1);
            return FALSE;
        }

        pSubMenu->wFlags = MT_POPUP;

    }

	// caculate the width and height of the menu

	wMaxWidth = 0;
	wMaxHeight = 0;
	for (i=0; i<pMenuObj->cUsedItems; i++)
	{
		if (!MENU_GetItemDimension(&(pMenuObj->pItems[i]), &itemSize))
        {
            SetLastError(1);
            return FALSE;
        }

		wMaxWidth = (wMaxWidth > itemSize.cx) ? wMaxWidth : (WORD)(itemSize.cx);
		wMaxHeight += (WORD)(itemSize.cy);
	}
	
	pMenuObj->cxWidth = wMaxWidth;
	pMenuObj->cyHeight = wMaxHeight;
    */


	return TRUE;
}

#ifndef NOMENUAPI

/*
*  changes an existing menu item.
*/
BOOL MENU_ModifyMenu(PMENUOBJ pMenuObj, UINT uPosition, 
					 UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem)
{
	int		    i;
	UINT	    uPos;
    PMENUOBJ    pMenu; //, pSubMenu;
    HANDLE      handle;
    WORD        wHotPos, wTabPos, wHotNum;
    int         slen;
    HWND hMenu;
    char ClassName[10];
    //WORD      wMaxWidth, wMaxHeight;
    //SIZE      itemSize;

	ASSERT(pMenuObj != NULL);

    // find the position of the specified menu string
	if (uFlags & MF_BYPOSITION)	   // uPosition is zero-based position 
	{
		if (uPosition + 1 > pMenuObj->cUsedItems)
            return FALSE;

        uPos = uPosition;
	}
    else    // uPosition is the identifier
    {
        for (i=0; i<pMenuObj->cUsedItems; i++)
		{
			if (pMenuObj->pItems[i].hIDorPopup == uPosition)
				break;

            if (pMenuObj->pItems[i].wFlags & MF_POPUP)  // submenu
            {
                handle = (HANDLE)pMenuObj->pItems[i].hIDorPopup;
                if (!IsMenu((HMENU)handle))
                    return FALSE;
                pMenu = (PMENUOBJ)handle;

                if (MENU_ModifyMenu(pMenu, uPosition, uFlags, uIDNewItem, lpNewItem))
                    return TRUE;
            }
		}

        if (i == pMenuObj->cUsedItems)
			return FALSE;

		uPos = i;
    }
    if ((pMenuObj->pItems[uPos].wFlags & MF_POPUP) && (uFlags & MF_POPUP)
        && pMenuObj->pItems[uPos].hIDorPopup != uIDNewItem)
    {
        handle = (HANDLE)pMenuObj->pItems[uPos].hIDorPopup;
		MENU_Destroy((PMENUOBJ)handle);
    }

    pMenuObj->pItems[uPos].wFlags = uFlags;
    pMenuObj->pItems[uPos].xULStart = 0;
    pMenuObj->pItems[uPos].xTab = 0;
    pMenuObj->pItems[uPos].hCheckedBmp = NULL;
    pMenuObj->pItems[uPos].hUncheckedBmp = NULL;
	
    pMenuObj->pItems[uPos].hIDorPopup = uIDNewItem;

    if (!(uFlags & MF_BITMAP) && !(uFlags & MF_OWNERDRAW) 
        && !(uFlags & MF_SEPARATOR) && (uFlags & MF_NOTUSERSTRING))
    {
        slen = strlen(lpNewItem);
        pMenuObj->pItems[uPos].hStrOrBmp = MemAlloc((slen + 1) * sizeof(char));
        strcpy(pMenuObj->pItems[uPos].hStrOrBmp, lpNewItem);
    }
    else
        pMenuObj->pItems[uPos].hStrOrBmp = (HANDLE)lpNewItem;
   
	if (!lpNewItem || (uFlags & MF_BITMAP))
		pMenuObj->pItems[uPos].cbItemLen = 0;
	else
    {
        // get the position of the hot key and the tab key
        MENU_FindHotTab(lpNewItem, &wHotPos, &wTabPos, &wHotNum);

        pMenuObj->pItems[uPos].xULStart = wHotPos;
        pMenuObj->pItems[uPos].xULNum = wHotNum;
        pMenuObj->pItems[uPos].xTab = wTabPos;
		pMenuObj->pItems[uPos].cbItemLen = strlen(lpNewItem);
    }

    hMenu = GetCapture();
    if (hMenu)
    {
        GetClassName(hMenu, ClassName, 10);
        while(!strcmp(ClassName, "#MENU"))
        {
            InvalidateRect(hMenu, NULL, TRUE);
            hMenu = GetWindow(hMenu, GW_HWNDNEXT);
            if (hMenu == NULL)
                break;
            GetClassName(hMenu, ClassName, 10);
        }
    }
    // caculate the width and height of the new menu
    /*
    wMaxWidth = 0;
	wMaxHeight = 0;
	for (i=0; i<pMenuObj->cUsedItems; i++)
	{
		if (!MENU_GetItemDimension(&(pMenuObj->pItems[i]), &itemSize))
        {
            SetLastError(1);
            return FALSE;
        }

		wMaxWidth = (wMaxWidth > itemSize.cx) ? wMaxWidth : (WORD)(itemSize.cx);
		wMaxHeight += (WORD)(itemSize.cy);
	}
	
	pMenuObj->cxWidth = wMaxWidth;
	pMenuObj->cyHeight = wMaxHeight;
    */

	return TRUE;
}

/*
*  deletes a menu item from the specified menu.
*  If the menu item opens a drop-down menu or submenu, RemoveMenu does
*  not destroy the menu or its handle
*/
BOOL MENU_RemoveMenu(PMENUOBJ pMenuObj, UINT uPosition, UINT uFlags)
{
	int		    i, nIndex = 0;
	UINT	    uPos;
    //SIZE      itemSize;
    PMENUOBJ    pMenu = NULL;
    HANDLE      handle;
    HWND        hwndMenu = NULL;
    char        ClassName[10];

	ASSERT(pMenuObj != NULL);

    hwndMenu = GetCapture();
    GetClassName(hwndMenu, ClassName, 10);

    if (!strcmp(ClassName, "#MENU"))
    {
        pMenu = (PMENUOBJ)GetWindowLong(hwndMenu, GWL_ID);
        while (pMenu != pMenuObj)
        {
            hwndMenu = pMenu->hParentMenuWin;
            if (hwndMenu == NULL)
                break;

            pMenu = (PMENUOBJ)GetWindowLong(hwndMenu, GWL_ID);
        }
        if (hwndMenu != NULL)
        {
            SendMessage(hwndMenu, WM_REFRESHMENU, (WPARAM)FALSE, (LPARAM)&nIndex);
        }
    }

    // find the position of the specified menu string
	if (uFlags & MF_BYPOSITION)	    // uPosition is zero-based position 
	{
		if (uPosition + 1 > pMenuObj->cUsedItems)
            return FALSE;

        uPos = uPosition;
	}
    else    // uPosition is the identifier
    {
        for (i=0; i<pMenuObj->cUsedItems; i++)
		{
			if (pMenuObj->pItems[i].hIDorPopup == uPosition)
				break;

            if (pMenuObj->pItems[i].wFlags & MF_POPUP)  // is submenu
            {
                handle = (HANDLE)pMenuObj->pItems[i].hIDorPopup;
                if (!IsMenu((HMENU)handle))
                    return FALSE;
                pMenu = (PMENUOBJ)handle;

                if (MENU_RemoveMenu(pMenu, uPosition, uFlags))
                    return TRUE;
            }
		}

        if (i == pMenuObj->cUsedItems)
			return FALSE;

		uPos = i;
    }
    if (pMenuObj->pItems[uPos].wFlags & MF_POPUP)
        CLRMENUUSED((PMENUOBJ)(HANDLE)pMenuObj->pItems[uPos].hIDorPopup);

    memmove(&pMenuObj->pItems[uPos], &pMenuObj->pItems[uPos + 1], 
            (pMenuObj->cUsedItems - uPos - 1) * sizeof(MENUITEM));

	pMenuObj->cUsedItems--;

    if (hwndMenu != NULL)
    {
        if ((uPosition <= (UINT)nIndex) && (nIndex > 0))
            nIndex--;

        SendMessage(hwndMenu, WM_REFRESHMENU, (WPARAM)TRUE, (LPARAM)nIndex);
    }

    // caculate the width and height of the new menu
    /*
    wMaxWidth = 0;
	wMaxHeight = 0;
	for (i=0; i<pMenuObj->cUsedItems; i++)
	{
		if (!MENU_GetItemDimension(&(pMenuObj->pItems[i]), &itemSize))
        {
            SetLastError(1);
            return FALSE;
        }

		wMaxWidth = (wMaxWidth > itemSize.cx) ? wMaxWidth : (WORD)(itemSize.cx);
		wMaxHeight += (WORD)(itemSize.cy);
	}
	
	pMenuObj->cxWidth = wMaxWidth;
	pMenuObj->cyHeight = wMaxHeight;
    */

	return TRUE;
}

/*
*  deletes an item from the specified menu. If the menu
*  item opens a menu or submenu, this function destroys the handle to the menu
*  or submenu and frees the memory used by the menu or submenu.
*/
BOOL MENU_DeleteMenu(PMENUOBJ pMenuObj, UINT uPosition, UINT uFlags)
{
	int		    i, nIndex = 0;
	UINT	    uPos;
    PMENUOBJ    pMenu = NULL;
    HANDLE      handle;
    HWND        hwndMenu = NULL;
    char        ClassName[10];
    //SIZE        itemSize;

	ASSERT(pMenuObj != NULL);

    hwndMenu = GetCapture();
    GetClassName(hwndMenu, ClassName, 10);

    if (!strcmp(ClassName, "#MENU"))
    {
        pMenu = (PMENUOBJ)GetWindowLong(hwndMenu, GWL_ID);
        while (pMenu != pMenuObj)
        {
            hwndMenu = pMenu->hParentMenuWin;
            if (hwndMenu == NULL)
                break;

            pMenu = (PMENUOBJ)GetWindowLong(hwndMenu, GWL_ID);
        }
        if (hwndMenu != NULL)
        {
            SendMessage(hwndMenu, WM_REFRESHMENU, (WPARAM)FALSE, (LPARAM)&nIndex);
        }
    }

    // find the position of the specified menu string
	if (uFlags & MF_BYPOSITION)	    // uPosition is zero-based position 
	{
		if (uPosition + 1 > pMenuObj->cUsedItems)
            return FALSE;

        uPos = uPosition;
	}
    else    // uPosition is the identifier
    {
        for (i=0; i<pMenuObj->cUsedItems; i++)
		{
			if (pMenuObj->pItems[i].hIDorPopup == uPosition)
				break;

            if (pMenuObj->pItems[i].wFlags & MF_POPUP)  // is submenu
            {
                handle = (HANDLE)pMenuObj->pItems[i].hIDorPopup;
                if (!IsMenu((HMENU)handle))
                    return FALSE;
                pMenu = (PMENUOBJ)handle;

                if (MENU_DeleteMenu(pMenu, uPosition, uFlags))
                    return TRUE;
            }
		}

        if (i == pMenuObj->cUsedItems)
			return FALSE;

		uPos = i;
    }

	if (pMenuObj->pItems[uPos].wFlags & MF_POPUP)	// is submenu
    {
        handle = (HANDLE)pMenuObj->pItems[uPos].hIDorPopup;
		MENU_Destroy((PMENUOBJ)handle);
    }

 	memmove(&pMenuObj->pItems[uPos], &pMenuObj->pItems[uPos + 1], 
            (pMenuObj->cUsedItems - uPos - 1) * sizeof(MENUITEM));

	pMenuObj->cUsedItems--;

    if (hwndMenu != NULL)
    {
        if ((uPosition <= (UINT)nIndex) && (nIndex > 0))
            nIndex--;

        SendMessage(hwndMenu, WM_REFRESHMENU, (WPARAM)TRUE, (LPARAM)nIndex);
    }

    // caculate the width and height of the new menu
    /*
    wMaxWidth = 0;
	wMaxHeight = 0;
	for (i=0; i<pMenuObj->cUsedItems; i++)
	{
		if (!MENU_GetItemDimension(&(pMenuObj->pItems[i]), &itemSize))
        {
            SetLastError(1);
            return FALSE;
        }

		wMaxWidth = (wMaxWidth > itemSize.cx) ? wMaxWidth : (WORD)(itemSize.cx);
		wMaxHeight += (WORD)(itemSize.cy);
	}
	
	pMenuObj->cxWidth = wMaxWidth;
	pMenuObj->cyHeight = wMaxHeight;
    */

	return TRUE;
}

/*
*  retrieves the object of the drop-down menu or submenu
*  activated by the specified menu item. 
*/
PMENUOBJ MENU_GetSubMenu(PMENUOBJ pMenuObj, int nPos)
{
    HANDLE handle;

	ASSERT(pMenuObj != NULL);

	if (nPos + 1 > pMenuObj->cUsedItems)
        return NULL;

	if (!(pMenuObj->pItems[nPos].wFlags & MF_POPUP))
		return NULL;

	handle = (HANDLE)pMenuObj->pItems[nPos].hIDorPopup;

    return (PMENUOBJ)handle;
}

/*
*  get the number of menu item of the specific menu
*/
int	MENU_GetMenuItemCount(PMENUOBJ pMenuObj)
{
	ASSERT(pMenuObj != NULL);

	return pMenuObj->cUsedItems;
}

/*
*  get the identifer of the specific menu item
*/
UINT MENU_GetMenuItemID(PMENUOBJ pMenuObj, int nPos)
{
	ASSERT(pMenuObj != NULL);

	if (nPos + 1 > pMenuObj->cUsedItems)
        return 0XFFFFFFFF;

	if (pMenuObj->pItems[nPos].hIDorPopup == 0 ||
		pMenuObj->pItems[nPos].wFlags & MF_POPUP)
		return 0XFFFFFFFF;

	return (UINT)(pMenuObj->pItems[nPos].hIDorPopup);
}

/*
*  associates the specified bitmap with a menu
*  item. Whether the menu item is checked or unchecked, Windows displays the 
*  appropriate bitmap next to the menu item.
*/
BOOL MENU_SetMenuItemBitmaps(PMENUOBJ pMenuObj, UINT uPosition, UINT uFlags, 
							 HBITMAP hBitmapUnchecked, HBITMAP hBitmapChecked)
{
	int		    i;
	UINT	    uPos;
    HANDLE      handle;
    PMENUOBJ    pMenu;

	ASSERT(pMenuObj != NULL);

    // find the position of the specified menu string
	if (uFlags & MF_BYPOSITION)	   // uPosition is zero-based position 
	{
		if (uPosition + 1 > pMenuObj->cUsedItems)
            return FALSE;

        uPos = uPosition;
	}
    else    // uPosition is the identifier
    {
        for (i=0; i<pMenuObj->cUsedItems; i++)
		{
			if (pMenuObj->pItems[i].hIDorPopup == uPosition)
				break;

            if (pMenuObj->pItems[i].wFlags & MF_POPUP)  // is submenu
            {
                handle = (HANDLE)pMenuObj->pItems[i].hIDorPopup;
                if (!IsMenu((HMENU)handle))
                    return FALSE;
                pMenu = (PMENUOBJ)handle;

                if (MENU_SetMenuItemBitmaps(pMenu, uPosition, uFlags, 
                            hBitmapUnchecked, hBitmapChecked))
                    return TRUE;
            }
		}

        if (i == pMenuObj->cUsedItems)
			return FALSE;

		uPos = i;
    }

	if (hBitmapChecked && hBitmapUnchecked)  // is valid value
	{
		pMenuObj->pItems[uPos].hCheckedBmp = hBitmapChecked;
		pMenuObj->pItems[uPos].hUncheckedBmp = hBitmapUnchecked;
	}
	else
	{
		pMenuObj->pItems[uPos].hCheckedBmp = NULL;
		pMenuObj->pItems[uPos].hUncheckedBmp = NULL;
	}

	return TRUE;
}

/*
*  enables, disables, or grays the specified menu item.
*/
BOOL MENU_EnableMenuItem(PMENUOBJ pMenuObj, UINT uIDEnableItem, UINT uEnable)
{
    //int         nOldEnable;
    static DWORD nOldEnable;
    UINT        uPos;
    PMENUOBJ    pMenu;
    HANDLE      handle;
    int         i;

    ASSERT(pMenuObj != NULL);

    // find the position of the specified menu string
   	if (uEnable & MF_BYPOSITION)	 // uPosition is zero-based position 
	{
		if (uIDEnableItem + 1 > pMenuObj->cUsedItems)
            return 0xFFFFFFFF;

        uPos = uIDEnableItem;
	}
    else    // uPosition is the identifier
    {
        for (i=0; i<pMenuObj->cUsedItems; i++)
		{
			if (pMenuObj->pItems[i].hIDorPopup == uIDEnableItem)
				break;

            if (pMenuObj->pItems[i].wFlags & MF_POPUP)  // is submenu
            {
                handle = (HANDLE)pMenuObj->pItems[i].hIDorPopup;
                if (!IsMenu((HMENU)handle))
                    return 0xFFFFFFFF;
                pMenu = (PMENUOBJ)handle;

                nOldEnable = MENU_EnableMenuItem(pMenu, uIDEnableItem, uEnable);
                if (nOldEnable != 0xFFFFFFFF)
                    return nOldEnable;
            }
		}

        if (i == pMenuObj->cUsedItems)
			return 0xFFFFFFFF;

		uPos = i;
    }

    // get the old state.
    if (pMenuObj->pItems[uPos].wFlags & MF_DISABLED)
    {
        nOldEnable = MF_DISABLED;
        pMenuObj->pItems[uPos].wFlags &= 0xFFFD;
    }
    else if (pMenuObj->pItems[uPos].wFlags & MF_GRAYED)
    {
        nOldEnable = MF_GRAYED;
        pMenuObj->pItems[uPos].wFlags &= 0xFFFE;
    }
    else
        nOldEnable = MF_ENABLED;

    // set the new state
    if (uEnable & MF_DISABLED)
        pMenuObj->pItems[uPos].wFlags |= MF_DISABLED;
    else if (uEnable & MF_GRAYED)
        pMenuObj->pItems[uPos].wFlags |= MF_GRAYED;

    return nOldEnable;
}
 
/*
*  sets the state of the specified menu item's check
*  mark attribute to either checked or unchecked. 
*/
BOOL MENU_CheckMenuItem(PMENUOBJ pMenuObj, UINT uIDCheckItem, UINT uCheck)
{
    //int         nOldCheck;
    static DWORD nOldCheck;
    UINT        uPos;
    PMENUOBJ    pMenu;
    HANDLE      handle;
    int         i;

    ASSERT(pMenuObj != NULL);

    // find the position of the specified menu string
   	if (uCheck & MF_BYPOSITION)	    // uPosition is zero-based position 
	{
		if (uIDCheckItem + 1 > pMenuObj->cUsedItems)
            return 0xFFFFFFFF;

        uPos = uIDCheckItem;
	}
    else    // uPosition is the identifier
    {
        for (i=0; i<pMenuObj->cUsedItems; i++)
		{
			if (pMenuObj->pItems[i].hIDorPopup == uIDCheckItem)
				break;

            if (pMenuObj->pItems[i].wFlags & MF_POPUP)  // is submenu
            {
                handle = (HANDLE)pMenuObj->pItems[i].hIDorPopup;
                if (!IsMenu((HMENU)handle))
                    return 0xFFFFFFFF;
                pMenu = (PMENUOBJ)handle;

                nOldCheck = MENU_CheckMenuItem(pMenu, uIDCheckItem, uCheck);
                if (nOldCheck != 0xFFFFFFFF)
                    return nOldCheck;
            }
		}

        if (i == pMenuObj->cUsedItems)
			return 0xFFFFFFFF;

		uPos = i;
    }

    // save the old state
    if (pMenuObj->pItems[uPos].wFlags & MF_CHECKED)
    {
        nOldCheck = MF_CHECKED;
        pMenuObj->pItems[uPos].wFlags &= 0xFFF7;
    }
    else
        nOldCheck = MF_UNCHECKED;

    // set the new state
    if (uCheck & MF_CHECKED)
        pMenuObj->pItems[uPos].wFlags |= MF_CHECKED;

    return nOldCheck;
}

/*
*  get the menu item's state include checked, grayed and disabled
*/
UINT MENU_GetMenuState(PMENUOBJ pMenuObj, UINT uId, UINT uFlags)
{
    UINT        uPos;
    UINT        uRet;
    PMENUOBJ    pMenu;
    HANDLE      handle;
    int         i;

    ASSERT(pMenuObj != NULL);

    // find the position of the specified menu string
   	if (uFlags & MF_BYPOSITION)	    // uPosition is zero-based position 
	{
		if (uId + 1 > pMenuObj->cUsedItems)
            return 0xFFFFFFFF;

        uPos = uId;
	}
    else    // uPosition is the identifier
    {
        for (i=0; i<pMenuObj->cUsedItems; i++)
		{
			if (pMenuObj->pItems[i].hIDorPopup == uId)
				break;

            if (pMenuObj->pItems[i].wFlags & MF_POPUP)  // is submenu
            {
                handle = (HANDLE)pMenuObj->pItems[i].hIDorPopup;
                if (!IsMenu((HMENU)handle))
                    return 0xFFFFFFFF;
                pMenu = (PMENUOBJ)handle;

                uRet = MENU_GetMenuState(pMenu, uId, uFlags);
                if (uRet != 0xFFFFFFFF)
                    return uRet;
            }
		}

        if (i == pMenuObj->cUsedItems)
			return 0xFFFFFFFF;

		uPos = i;
    }

    if (pMenuObj->pItems[uPos].wFlags & MF_POPUP)
    {
        handle = (HANDLE)pMenuObj->pItems[uPos].hIDorPopup;
        if (!IsMenu((HMENU)handle))
            return 0xFFFFFFFF;
        pMenu = (PMENUOBJ)handle;

        uRet = MAKELONG(pMenuObj->pItems[uPos].wFlags, pMenu->cUsedItems);
    }
    else
        uRet = pMenuObj->pItems[uPos].wFlags;

    return uRet;
}

/*
*  MENU_GetMenuString
*/
int MENU_GetMenuString(PMENUOBJ pMenuObj, UINT uIDItem, LPSTR lpString, int nMaxCount, UINT uFlag)
{
    UINT        uPos;
    PMENUOBJ    pMenu;
    HANDLE      handle;
    int         i, nRet;

    ASSERT(pMenuObj != NULL);

    // find the position of the specified menu string
   	if (uFlag & MF_BYPOSITION)	    // uPosition is zero-based position 
	{
		if (uIDItem + 1 > pMenuObj->cUsedItems)
            return 0;

        uPos = uIDItem;
	}
    else    // uPosition is the identifier
    {
        for (i=0; i<pMenuObj->cUsedItems; i++)
		{
			if (pMenuObj->pItems[i].hIDorPopup == uIDItem)
				break;

            if (pMenuObj->pItems[i].wFlags & MF_POPUP)  // is submenu
            {
                handle = (HANDLE)pMenuObj->pItems[i].hIDorPopup;
                if (!IsMenu((HMENU)handle))
                    return 0;
                pMenu = (PMENUOBJ)handle;

                nRet = MENU_GetMenuString(pMenu, uIDItem, lpString, nMaxCount, uFlag);
                if (nRet)
                    return nRet;
            }
		}

        if (i == pMenuObj->cUsedItems)
			return 0;

		uPos = i;
    }

    // get the length of the menu string.
    nRet = pMenuObj->pItems[uPos].cbItemLen;

    if (lpString)
    {
        if (nMaxCount && nRet)
        {
            nRet = (nMaxCount > nRet) ? nRet : nMaxCount;
            strncpy(lpString, (LPSTR)(pMenuObj->pItems[uPos].hStrOrBmp), nRet);
        }
    }

    return nRet;
}

#endif // NOMENUAPI

static void MENU_FindHotTab(LPCSTR lpNewItem, WORD *wHotPos, 
                              WORD *wTabPos, WORD *wHotNum)
{
    int nLen, i;

    *wHotPos = 0;
    *wTabPos = 0;
    *wHotNum = 0;

    nLen = strlen(lpNewItem);
    for (i = 0; i < nLen; i++)
    {
        if (lpNewItem[i] == '&' && lpNewItem[i + 1] != '&' )
        {
            *wHotPos = (i + 1 == nLen) ? 0 : i + 1;
            *wHotNum += 1;
        }
        else if(lpNewItem[i] == '&' && lpNewItem[i + 1] == '&')
        {
            *wHotNum += 1;
            i++;
        }
    }

     for (i = 0; i < nLen; i++)
    {
        if (lpNewItem[i] == '\t')
        {
            *wTabPos = (i + 1 == nLen) ? 0 : i + 1;
            break;
        }
    }
}


#ifndef NOMENUAPI

/*
static BOOL MENU_GetItemDimension(PMENUITEM pItemObj, PSIZE pSize)
{
    SIZE	    TextSize;
    PBMPOBJ     pBmp;
    BOOL        feed;

    if (!pSize)
    {
	    SetLastError(1);
    	return FALSE;
	}

    pSize->cx = 0;
    pSize->cy = 0;

    if (pItemObj->wFlags & MF_BITMAP)		// is a bitmap
	{
        pBmp = (PBMPOBJ)WOT_GetObj(pItemObj->hStrOrBmp, OBJ_BITMAP);
		if (!pBmp)
		{
			SetLastError(1);
			return FALSE;
		}

		pSize->cx = pBmp->bitmap.bmWidth;
        pSize->cy = pBmp->bitmap.bmHeight;
	}
	else
	{
		feed = GetTextExtentPoint32(NULL, (PCSTR)(pItemObj->hStrOrBmp), -1, &TextSize);
		if (!feed)
		{
			SetLastError(1);
			return FALSE;
		}

        pSize->cx = TextSize.cx;
        pSize->cy = TextSize.cy;
	}

    return TRUE;
}
*/

#endif  // NOMENUAPI

#else   // NOMENUS

static void UnusedFunc(void)
{
}

#endif  // NOMENUS
