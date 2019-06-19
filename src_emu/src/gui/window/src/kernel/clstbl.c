/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements window class table management functions.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "string.h"
#include "wsodc.h"
#include "clstbl.h"

static PCLSOBJ GlobalClassTableHead = NULL;
static PCLSOBJ LocaleClassTableHead = NULL;
/*******************************************************************\
**          Window class operation function                        
\*******************************************************************/

/*
**  Function : CLS_GetInfo
**  Purpose  :
*/
BOOL CLS_GetInfo(HINSTANCE hInst, PCLSOBJ pClsObj, PWNDCLASS pWndClass)
{
    ASSERT(pClsObj != NULL);
    ASSERT(pWndClass != NULL);

    if (pClsObj->wc.hInstance != hInst)
        return FALSE;

    memcpy(pWndClass, &pClsObj->wc, sizeof(WNDCLASS));

    return TRUE;
}

/********************************************************************/
/*              Window class table operation functions              */
/********************************************************************/

/*
**  Function : CLSTBL_Insert
**  Purpose  :
*/
PCLSOBJ CLSTBL_Insert(const WNDCLASS* pWndClass)
{
    PCLSOBJ pClsObj;

    ASSERT(pWndClass != NULL);

    if (pWndClass->style & CS_GLOBALCLASS || pWndClass->hInstance == NULL)
    {
        // Find whether the specified class name is used.
        pClsObj = GlobalClassTableHead;
        while (pClsObj)
        {
            if (!stricmp(pClsObj->wc.lpszClassName, pWndClass->lpszClassName))
                break;

            pClsObj = pClsObj->pNext;
        }
    }
    else
    {
        // Find whether the specified class name is used.
        pClsObj = LocaleClassTableHead;
        while (pClsObj)
        {
            /* 局部注册类需要同时匹配类名和hInstance */
            if (!stricmp(pClsObj->wc.lpszClassName, pWndClass->lpszClassName)
                && pClsObj->wc.hInstance == pWndClass->hInstance)
                break;

            pClsObj = pClsObj->pNext;
        }
    }

    // If the class name is used, return NULL
    if (pClsObj != NULL)
    {
        SetLastError(1);
        return NULL;
    }

    // Creates a new class item

    pClsObj = (PCLSOBJ)MemAlloc(sizeof(CLSOBJ) + 
        pWndClass->cbClsExtra);

    if (!pClsObj)
        return NULL;

    memcpy(&pClsObj->wc, pWndClass, sizeof(WNDCLASS));
#if (__MP_PLX_GUI)
    pClsObj->wc.lpszClassName = (LPCSTR)MemAlloc(strlen(pWndClass->lpszClassName) + 1);
    if (!pClsObj->wc.lpszClassName)
    {
        MemFree(pClsObj);
        return NULL;
    }
    
    if (pWndClass->lpszMenuName)
    {
        pClsObj->wc.lpszMenuName = (LPCSTR)MemAlloc(strlen(pWndClass->lpszMenuName) + 1);
        if (!pClsObj->wc.lpszMenuName)
        {
            MemFree(pClsObj->wc.lpszClassName);
            MemFree(pClsObj);
            return NULL;
        }
        strcpy(pClsObj->wc.lpszMenuName, pWndClass->lpszMenuName);
    }
    
    strcpy(pClsObj->wc.lpszClassName, pWndClass->lpszClassName);
#endif
    pClsObj->nRef = 0;
    pClsObj->bDeleted = FALSE;

    if (pWndClass->style & CS_CLASSDC)
    {
        pClsObj->pDC = DC_CreateWindowDC(NULL, TRUE);
        if (!pClsObj->pDC)
        {
            CLSTBL_Delete(pWndClass->lpszClassName, pWndClass->hInstance);    
            return NULL;
        }
    }
    else
    {
        pClsObj->pDC = NULL;
    }
    
    // Adds the new class item into class table
    if (pWndClass->style & CS_GLOBALCLASS || pWndClass->hInstance == NULL)
    {
        pClsObj->wc.style |= CS_GLOBALCLASS;
        pClsObj->pNext = GlobalClassTableHead;
        GlobalClassTableHead = pClsObj;
    }
    else
    {
        pClsObj->pNext = LocaleClassTableHead;
        LocaleClassTableHead = pClsObj;
    }

    return pClsObj;
}

/*
**  Function : CLSTBL_Delete
**  Purpose  :
*/
BOOL CLSTBL_Delete(PCSTR pszClassName, HINSTANCE hInst)
{
    PCLSOBJ pClsObj = NULL, pPrevClsObj = NULL;
    PXGDIOBJ pObj;
    
    // Find the class item to be deleted
    if (hInst != NULL)
    {
        pPrevClsObj = NULL;
        pClsObj = LocaleClassTableHead;

        while (pClsObj != NULL)
        {
            if (!stricmp(pClsObj->wc.lpszClassName, pszClassName) && 
                pClsObj->wc.hInstance == hInst)
                break;

            pPrevClsObj = pClsObj;
            pClsObj = pClsObj->pNext;
        }
    }
    if (pClsObj == NULL)
    {
        pPrevClsObj = NULL;
        pClsObj = GlobalClassTableHead;

        while (pClsObj != NULL)
        {
            if (!stricmp(pClsObj->wc.lpszClassName, pszClassName))
                break;

            pPrevClsObj = pClsObj;
            pClsObj = pClsObj->pNext;
        }
    }

    // If can't find, return FALSE;
    if (!pClsObj)
    {
        SetLastError(1);
        return FALSE;
    }

    ASSERT(pClsObj->nRef >= 0);

    // 看创建窗口的应用计数是否为0，不为0表示还有该类的窗口存在
    if (pClsObj->nRef > 0)
    {
        pClsObj->bDeleted = TRUE;
        return FALSE;
    }
    // If finds, delete the class item and free the class item memory 
    // space

    if (pPrevClsObj)
        pPrevClsObj->pNext = pClsObj->pNext;
    else
    {
        if (hInst == NULL)
            GlobalClassTableHead = pClsObj->pNext;
        else
            LocaleClassTableHead = pClsObj->pNext;
    }
    
    if (pClsObj->wc.style & CS_CLASSDC)
    {
        DC_Destroy(pClsObj->pDC);
    }

    /* 这里删除仍可能有问题，句柄可能会重新使用，在用了serial16次以后*/

    /* 管程内部DeleteObject(pClsObj->wc.hbrBackground); */
    pObj = WOT_LockObj((HANDLE)pClsObj->wc.hbrBackground, OBJ_ANY);
    if (pObj != NULL)
    {
        ASSERT(WOT_GetObjType((PWSOBJ)pObj) >= OBJ_XGDI_MIN && 
            WOT_GetObjType((PWSOBJ)pObj) <= OBJ_XGDI_MAX);
        
        WOT_DelectObj(pObj);
        WOT_UnlockObj(pObj);
    }
#if (__MP_PLX_GUI)
    MemFree(pClsObj->wc.lpszClassName);
    MemFree(pClsObj->wc.lpszMenuName);
#endif
    MemFree(pClsObj);

    return TRUE;
}

/*
**  Function : CLSTBL_GetClassObj
**  Purpose  :
**      Gets the handle of the class which is specified by class name.
**  Params   :
**      pszClassName : Specified the class name.
**  Return   :
**      If the function succeed, the return value is the handle of 
**      the specified class. If the function fails, the return value
**      is NULL.
*/
PCLSOBJ CLSTBL_GetClassObj(PCSTR pszClassName, HINSTANCE hInst)
{
    PCLSOBJ pClsObj = NULL;

    ASSERT(pszClassName != NULL);

    if (hInst != NULL)
    {
        pClsObj = LocaleClassTableHead;
        while (pClsObj != NULL)
        {
            if (!stricmp(pClsObj->wc.lpszClassName, pszClassName) && 
                pClsObj->wc.hInstance == hInst)
                break;

            pClsObj = pClsObj->pNext;
        }
    }

    if (pClsObj == NULL)
    {
        pClsObj = GlobalClassTableHead;
        while (pClsObj != NULL)
        {
            if(!stricmp(pClsObj->wc.lpszClassName, pszClassName))
            //if (!strcasecmp(pClsObj->wc.lpszClassName, pszClassName))
	       break;

            pClsObj = pClsObj->pNext;
        }
    }

    return pClsObj;
}
