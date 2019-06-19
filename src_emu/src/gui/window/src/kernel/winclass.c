/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements three functions to access the class info of the
 *            the specified window.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "string.h"

#include "wsobj.h"
#include "wsownd.h"

/*
**  Function : WND_GetClassName
**  Purpose  :
*/
int WND_GetClassName(PWINOBJ pWin, PSTR pszClassName, int nMaxCount)
{
    int nCount;

    ASSERT(pWin != NULL);
    ASSERT(pszClassName != NULL);
    ASSERT(nMaxCount >= 1);

    nCount = strlen(pWin->pWndClass->lpszClassName);

    if (nCount > nMaxCount - 1)
        nCount = nMaxCount - 1;

    strncpy(pszClassName, pWin->pWndClass->lpszClassName, nCount);
    pszClassName[nCount] = 0;

    return nCount;
}

/*
**  Function : WND_GetClassWord
**  Purpose  :
**      Retrieves the specified 16-bit (word) value from the window class.
*/
WORD WND_GetClassWord(PWINOBJ pWin, int nIndex)
{
    ASSERT(pWin != NULL);

    if (nIndex < 0)
    {
        SetLastError(1);
        return 0;
    }

    if ((nIndex % sizeof(WORD)) || 
        (nIndex > (int)(pWin->pWndClass->cbClsExtra - sizeof(WORD))))
    {
        SetLastError(1);
        return 0;
    }
    
    return *(WORD*)((BYTE*)(pWin->pWndClass + 1) + nIndex);
}

/*
**  Function : WND_SetClassWord
**  Purpose  :
**      Replaces the 16-bit (word) value at the specified offset into the
**      extra class memory for the window class to which the specified 
**      window belongs. 
*/
WORD WND_SetClassWord(PWINOBJ pWin, int nIndex, WORD wNewValue)
{
    WORD wOldValue;

    ASSERT(pWin!= NULL);

    if (nIndex < 0)
    {
        SetLastError(1);
        return 0;
    }
    
    if ((nIndex % sizeof(WORD)) || 
        (nIndex > (int)(pWin->pWndClass->cbClsExtra - sizeof(WORD))))
    {
        SetLastError(1);
        return 0;
    }
    
    wOldValue = *(WORD*)((BYTE*)(pWin->pWndClass + 1) + nIndex);
    *(WORD*)((BYTE*)(pWin->pWndClass + 1) + nIndex) = wNewValue;
    
    return wOldValue;
}

/*
**  Function : WND_GetClassLong
**  Purpose  :
**      Retrieves the specified 32-bit (long) value from the window class.
*/
DWORD WND_GetClassLong(PWINOBJ pWin, int nIndex)
{
    DWORD dwRet;

    ASSERT(pWin != NULL);

    if (nIndex >= 0)
    {
        if ((nIndex % sizeof(DWORD)) || 
            (nIndex > (int)(pWin->pWndClass->cbClsExtra - sizeof(DWORD))))
        {
            SetLastError(1);
            return 0;
        }

        return *(DWORD*)((BYTE*)(pWin->pWndClass + 1) + nIndex);
    }

    switch (nIndex)
    {
    case GCL_MENUNAME :
        
        dwRet = (LONG)pWin->pWndClass->lpszMenuName;
        break;
        
    case GCL_HBRBACKGROUND :
        
        dwRet = (LONG)pWin->pWndClass->hbrBackground;
        break;
        
    case GCL_HCURSOR :
        
        dwRet = (LONG)pWin->pWndClass->hCursor;
        break;
        
    case GCL_HICON :
        
        dwRet = (LONG)pWin->pWndClass->hIcon;
        break;
        
    case GCL_HMODULE :
        
        dwRet = (LONG)pWin->pWndClass->hInstance;
        break;
        
    case GCL_CBWNDEXTRA :
        
        dwRet = (LONG)pWin->pWndClass->cbWndExtra;
        break;
        
    case GCL_CBCLSEXTRA :
        
        dwRet = (LONG)pWin->pWndClass->cbClsExtra;
        break;
        
    case GCL_WNDPROC :
        
        dwRet = (LONG)pWin->pWndClass->lpfnWndProc;
        break;
        
    case GCL_STYLE :
        
        dwRet = (LONG)pWin->pWndClass->style;
        break;
        
    default :

        SetLastError(1);
        dwRet = 0;
    }

    return dwRet;
}

/*
**  Function : WND_SetClassLong
*/
DWORD WND_SetClassLong(PWINOBJ pWin, int nIndex, DWORD dwNewValue)
{
    DWORD dwOldValue;

    ASSERT(pWin!= NULL);

    if (nIndex >= 0)
    {
        if ((nIndex % sizeof(DWORD)) || 
            (nIndex > (int)(pWin->pWndClass->cbClsExtra - sizeof(DWORD))))
        {
            SetLastError(1);
            return 0;
        }

        dwOldValue = *(DWORD*)((BYTE*)(pWin->pWndClass + 1) + nIndex);
        *(DWORD*)((BYTE*)(pWin->pWndClass + 1) + nIndex) = dwNewValue;

        return dwOldValue;
    }

    switch (nIndex)
    {
    case GCL_MENUNAME :
        
        dwOldValue = (LONG)pWin->pWndClass->lpszMenuName;
        pWin->pWndClass->lpszMenuName = (LPCSTR)dwNewValue ;
        break;
        
    case GCL_HBRBACKGROUND :
        
        dwOldValue = (LONG)pWin->pWndClass->hbrBackground;
        pWin->pWndClass->hbrBackground = (HBRUSH)dwNewValue ;
        break;
        
    case GCL_HCURSOR :
        
        dwOldValue = (LONG)pWin->pWndClass->hCursor;
        pWin->pWndClass->hCursor = (HCURSOR)dwNewValue ;
        break;
        
    case GCL_HICON :
        
        dwOldValue = (LONG)pWin->pWndClass->hIcon;
        pWin->pWndClass->hIcon = (HICON)dwNewValue ;
        break;
        
    case GCL_HMODULE :
        
        dwOldValue = (LONG)pWin->pWndClass->hInstance;
        pWin->pWndClass->hInstance = (HINSTANCE)dwNewValue ;
        break;
        
    case GCL_CBWNDEXTRA :
        
        dwOldValue = (LONG)pWin->pWndClass->cbWndExtra;
        pWin->pWndClass->cbWndExtra = (int)dwNewValue ;
        break;
        
    case GCL_CBCLSEXTRA :
        
        dwOldValue = (LONG)pWin->pWndClass->cbClsExtra;
        pWin->pWndClass->cbClsExtra = (int)dwNewValue ;
        break;
        
    case GCL_WNDPROC :
        
        dwOldValue = (LONG)pWin->pWndClass->lpfnWndProc;
        pWin->pWndClass->lpfnWndProc = (WNDPROC)dwNewValue ;
        break;
        
    case GCL_STYLE :
        
        dwOldValue = (LONG)pWin->pWndClass->style;
        pWin->pWndClass->style = (DWORD)dwNewValue ;
        break;
        
    default :

        dwOldValue = 0;
    }

    return dwOldValue;
}
