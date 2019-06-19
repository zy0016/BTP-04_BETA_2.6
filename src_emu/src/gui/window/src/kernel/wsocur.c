/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Cursor object management module. 
 *            
\**************************************************************************/

#include "hpwin.h"

#if (!NOCURSOR)

#include "wsobj.h"
#include "wsocur.h"

/*
**  Function : CURSOR_Create
**  Purpose  :
**      Creates a cursor using specified cursor bitmap data.
**  Return   :
**      If the function succeeds, return the pointer to the cursor.
**      If the function fails, return NULL.
*/
PCURSOROBJ CURSOR_Create(int nType, int nRop, int nHotX, int nHotY, 
                         int nWidth, int nHeight, const void* pANDPlane, 
                         const void* pXORPlane)
{
    PCURSOROBJ pCursor;
    int nRet;
    PGRAPHDEV pGraphDev;

    // pAndPlane and pXORPlane must not be NULL
    if (!pANDPlane || !pXORPlane)
        return NULL;

    // Now only support 32x32 cursor
    if (nWidth != 32 || nHeight != 32)
        return NULL;

    // Now only support mono cursor
    if (nType != CUR_BW)
        return NULL;

    // Adjust the coordinate of the hot point if necessary
    nHotX %= nWidth;
    nHotY %= nHeight;

    pCursor = (PCURSOROBJ)MemAlloc(sizeof(CURSOROBJ));

    if (!pCursor)
    {
        SetLastError(1);
        return NULL;
    }

    pCursor->logcursor.type = nType;
    pCursor->logcursor.rop = nRop;
    pCursor->logcursor.hot_x = nHotX;
    pCursor->logcursor.hot_y = nHotY;
    pCursor->logcursor.width = nWidth;
    pCursor->logcursor.height = nHeight;
    pCursor->logcursor.fgcolor = 0xFFFFFF;
    pCursor->logcursor.bkcolor = 0;
    pCursor->logcursor.pANDPlane = pANDPlane;
    pCursor->logcursor.pXORPlane = pXORPlane;

    pGraphDev = GRAPHDEV_GetCursorScreen(0);
    nRet = pGraphDev->drv.RealizeCursor(pGraphDev->pDev, &pCursor->logcursor, 
        NULL);
    if (nRet <= 0)
    {
        MemFree(pCursor);

        SetLastError(1);
        return NULL;
    }
    
    pCursor->pPhysCursor = (PPHYSCURSOR)MemAlloc(nRet);
    if (!pCursor->pPhysCursor)
    {
        MemFree(pCursor);

        SetLastError(1);
        return NULL;
    }
    
    nRet = pGraphDev->drv.RealizeCursor(pGraphDev->pDev, &pCursor->logcursor, 
        pCursor->pPhysCursor);
    if (nRet <= 0)
    {
        MemFree(pCursor->pPhysCursor);
        MemFree(pCursor);

        SetLastError(1);
        return NULL;
    }

    if (!WOT_RegisterObj((PWSOBJ)pCursor, OBJ_CURSOR, FALSE))
    {
        pGraphDev->drv.UnrealizeCursor(pGraphDev->pDev, pCursor->pPhysCursor);
        MemFree(pCursor->pPhysCursor);
        MemFree(pCursor);

        return NULL;
    }

    return pCursor;
}

/*
**  Function : CURSOR_Destroy
**  Purpose  :
**      Destroys the specified cursor.
**  Params   :
**      pCursor : Specifies the cursor to be destroyed.
*/
void CURSOR_Destroy(PCURSOROBJ pCursor)
{
    PGRAPHDEV pGraphDev;

    ASSERT(pCursor != NULL);

    if (WOT_IsStockObj((PXGDIOBJ)pCursor))
        return;

    // Remove it from the object table
    WOT_UnregisterObj((PWSOBJ)pCursor);

    pGraphDev = GRAPHDEV_GetCursorScreen(0);
    ASSERT(pGraphDev != NULL);

    pGraphDev->drv.UnrealizeCursor(pGraphDev->pDev, pCursor->pPhysCursor);
    MemFree(pCursor->pPhysCursor);
    MemFree(pCursor);
}

/*
**  Function : CURSOR_GetPhysObj
**  Purpose  :
**      Retrieves the physical object of the specified cursor object.
*/
PPHYSCURSOR CURSOR_GetPhysObj(PCURSOROBJ pCursor)
{
    ASSERT(pCursor != NULL);

    return pCursor->pPhysCursor;
}

#else

static void CURSOR_Null(void)
{
}

#endif // NOCURSOR
