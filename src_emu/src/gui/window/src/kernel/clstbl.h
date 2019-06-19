/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Define prototypes for window class and window class
 *            table management function prototype
 *            
\**************************************************************************/

#ifndef __CLSTBL_H
#define __CLSTBL_H
// Define window class struct for internal use
typedef struct tagCLSOBJ
{
    struct tagCLSOBJ    *pNext;
    short               nRef;
    BOOL                bDeleted;
    WNDCLASS            wc;
    PDC                 pDC;
} CLSOBJ, *PCLSOBJ;

// Window class operation function
BOOL    CLS_GetInfo(HINSTANCE hInst, PCLSOBJ pClsObj, PWNDCLASS pWndClass);

// Window Class table operation function
PCLSOBJ CLSTBL_Insert(const WNDCLASS* pWndClass);
BOOL    CLSTBL_Delete(PCSTR pszClassName, HINSTANCE hInstance);
PCLSOBJ CLSTBL_GetClassObj(PCSTR pszClassName, HINSTANCE hInstance);

#endif  // __CLSTBL_H
