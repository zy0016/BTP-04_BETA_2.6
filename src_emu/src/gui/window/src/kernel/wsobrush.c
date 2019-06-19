/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Brush object management module.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "wsobj.h"
#include "wsobrush.h"
#include "wsobmp.h"
#include "string.h"

#if (__MP_PLX_GUI)
#define BRUSH_MemAlloc(size, bShared)    \
    ((bShared) ? MemAlloc(size) : WIN_LocalAlloc(LMEM_FIXED, size))
#define BRUSH_MemFree(p, bShared)        \
    ((bShared) ? MemFree(p) : WIN_LocalFree(p))
#define BMP_LocalLock(p, bShared)         \
    ((bShared) ? (p) : WIN_LocalLock(p))
#define BMP_LocalUnlock(p, bShared)         \
    ((bShared) ? (p) : WIN_LocalUnlock(p))
#else //__MP_PLX_GUI
#define BRUSH_MemAlloc(size, bShared)   MemAlloc(size)
#define BRUSH_MemFree(p, bShared)       MemFree(p)
#define BMP_LocalLock(p, bShared)       WIN_LocalLock(p)
#define BMP_LocalUnlock(p, bShared)     WIN_LocalUnlock(p)
#endif

/*
**  Function : BRUSH_Create
**  Purpose  :
**      Creates a brush that has the specified width, style and color
**      and pattern data.
**  Params   :
**      color  : Specifies the rgb value of the Brush to be created.
**  Return   :
**      If the function succeeds, the return value is a pointer that 
**      points a the brush. If the function fails, the return value 
**      is NULL. 
*/
PBRUSHOBJ BRUSH_Create(int nStyle, COLORREF color, LONG lHatch, BOOL bShared)
{
    PBRUSHOBJ pBrush;

    // If the brush style is an invalid style, change the brush style to 
    // BS_SOLID
    if (nStyle != BS_NULL && nStyle != BS_BITMAP && 
        nStyle != BS_HATCHED && nStyle != BS_PATTERN)
        nStyle = BS_SOLID;

    // If the brush is hatch brush and the hatch style is invalid style, 
    // return error;
    if (nStyle == BS_HATCHED && (lHatch < 0 || lHatch > HS_LAST))
    {
        SetLastError(1);
        return NULL;
    }
    if ((nStyle == BS_PATTERN || nStyle == BS_BITMAP) && lHatch == 0)
    {
        SetLastError(1);
        return NULL;
    }
    
    pBrush = (PBRUSHOBJ)BRUSH_MemAlloc(sizeof(BRUSHOBJ), bShared);

    if (pBrush == NULL)
    {
        SetLastError(1);
        return NULL;
    }

    if (!WOT_RegisterObj((PWSOBJ)pBrush, OBJ_BRUSH, bShared))
    {
        BRUSH_MemFree(pBrush, bShared);

        SetLastError(1);
        return NULL;
    }

    pBrush->logbrush.width  = 16;
    pBrush->logbrush.height = 16;
    pBrush->logbrush.style  = (WORD)nStyle;
    pBrush->logbrush.color  = color;

    if (nStyle == BS_HATCHED)
    {
        pBrush->logbrush.hatch_style = (WORD)lHatch;
        pBrush->logbrush.pPattern = NULL;
    }
    else if (nStyle == BS_PATTERN)
    {
        PBMPOBJ pBmpObj;
        DWORD dwSize;
        BYTE    *pBrushBuffer, *pBmpBuf;

        pBmpObj = (PBMPOBJ)WOT_LockObj((HANDLE)lHatch, OBJ_BITMAP);
        if (!pBmpObj)
        {
            WOT_UnregisterObj((PWSOBJ)pBrush);
            BRUSH_MemFree(pBrush, bShared);

            SetLastError(1);
            return NULL;
        }

        dwSize = pBmpObj->bitmap.bmWidthBytes * pBmpObj->bitmap.bmHeight * 
            pBmpObj->bitmap.bmPlanes;
        pBrush->logbrush.width  = pBmpObj->bitmap.bmWidth;
        pBrush->logbrush.height = pBmpObj->bitmap.bmHeight;
        pBrush->logbrush.hatch_style = 0;
        pBrush->logbrush.pPattern = BRUSH_MemAlloc(dwSize, bShared);

        pBrushBuffer = pBrush->logbrush.pPattern;

        pBmpBuf = (BYTE*)BMP_LocalLock((HLOCAL)(pBmpObj->bitmap.bmBits), 
            WOT_IsSharedObj(pBmpObj) || WOT_IsStockObj(pBmpObj));

        memcpy(pBrushBuffer, pBmpBuf, dwSize);

        BMP_LocalUnlock((HLOCAL)(pBmpObj->bitmap.bmBits), 
            WOT_IsSharedObj(pBmpObj) || WOT_IsStockObj(pBmpObj));

        WOT_UnlockObj((PXGDIOBJ)pBmpObj);
    }
    else if (nStyle == BS_BITMAP)
    {
        PBMPOBJ pBmpObj;
        DWORD dwSize;
        BYTE    *pBrushBuffer, *pBmpBuf;

        pBmpObj = (PBMPOBJ)WOT_LockObj((HANDLE)lHatch, OBJ_BITMAP);
        if (!pBmpObj)
        {
            WOT_UnregisterObj((PWSOBJ)pBrush);
            BRUSH_MemFree(pBrush, bShared);

            SetLastError(1);
            return NULL;
        }
        
        dwSize = pBmpObj->bitmap.bmWidthBytes * pBmpObj->bitmap.bmHeight * 
            pBmpObj->bitmap.bmPlanes;
        pBrush->logbrush.width  = pBmpObj->bitmap.bmWidth;
        pBrush->logbrush.height = pBmpObj->bitmap.bmHeight;
        pBrush->logbrush.hatch_style = 0;

        pBrush->logbrush.pPattern = BRUSH_MemAlloc(dwSize, bShared);
        pBrushBuffer = pBrush->logbrush.pPattern;

        pBmpBuf = (BYTE*)BMP_LocalLock((HLOCAL)(pBmpObj->bitmap.bmBits), 
            WOT_IsSharedObj(pBmpObj) || WOT_IsStockObj(pBmpObj));
        
        memcpy(pBrushBuffer, pBmpBuf, dwSize);
        
        BMP_LocalUnlock((HLOCAL)(pBmpObj->bitmap.bmBits), 
            WOT_IsSharedObj(pBmpObj) || WOT_IsStockObj(pBmpObj));

        WOT_UnlockObj((PXGDIOBJ)pBmpObj);
    }
    else
    {
        pBrush->logbrush.hatch_style = 0;
    }

    return pBrush;
}

/*
**  Function : BRUSH_Destroy
**  Purpose  :
**      Destroys a specified brush.
**  Params   :
**      pBrush : Specifies the brush to be destroyed.
*/
BOOL BRUSH_Destroy(PBRUSHOBJ pBrush)
{
    ASSERT(pBrush != NULL);

    // Can't destory stock object
    if (WOT_IsStockObj((PXGDIOBJ)pBrush))
        return FALSE;

    ASSERT(ISDELETEDXGDIOBJ(pBrush));
    
    // Remove the brush object from window object table
    WOT_UnregisterObj((PWSOBJ)pBrush);
    if (pBrush->logbrush.style == BS_PATTERN || pBrush->logbrush.style == BS_BITMAP)
        BRUSH_MemFree(pBrush->logbrush.pPattern, WOT_IsSharedObj(pBrush));

    BRUSH_MemFree(pBrush, WOT_IsSharedObj(pBrush));

    return TRUE;
}

/*
**  Function : BRUSH_GetLogObj
**  Purpose  :
**      Gets infomation about a specified brush object
*/
void BRUSH_GetLogObj(PBRUSHOBJ pBrushObj, PLOGBRUSH pLogBrush)
{
    ASSERT(pBrushObj != NULL);
    ASSERT(pLogBrush != NULL);

    pLogBrush->lbStyle = pBrushObj->logbrush.style;
    pLogBrush->lbColor = pBrushObj->logbrush.color;
    pLogBrush->lbHatch = pBrushObj->logbrush.hatch_style;
}

/*
**  Function : BRUSH_GetSize
**  Purpose  :
**      Retrieves the size(width and height) of the specified brush object.
*/
void BRUSH_GetSize(PBRUSHOBJ pBrush, PSIZE pSize)
{
    ASSERT(pBrush != NULL);
    ASSERT(pSize != NULL);

    pSize->cx = pBrush->logbrush.width;
    pSize->cy = pBrush->logbrush.height;
}
