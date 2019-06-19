/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements cursor-controlling functions.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "wsobj.h"
#include "wsocur.h"
#include "wscursor.h"

static RECT rcCursor;           // Cursor clip rectangle
static RECT rcCursorMax;        // Max cursor clip rectangle
static int nCursorX, nCursorY;  // Current cursor position

#if (!NOCURSOR)

static PCURSOROBJ pCursor;      // Current cursor object
static int nCursorShow;         // Whether the cursor is showing

/*
**  Function : WS_SetCursor
**  Purpose  :
*/
PCURSOROBJ WS_SetCursor(PCURSOROBJ pNewCursor)
{
    PCURSOROBJ pOldCursor;
    PGRAPHDEV pGraphDev;

    // The new cursor is the same as the current cursor, just return
    if (pCursor == pNewCursor)
        return pCursor;

    // Save current cursor 
    pOldCursor = pCursor;

    pCursor = pNewCursor;

    pGraphDev = GRAPHDEV_GetCursorScreen(0);
    ASSERT(pGraphDev != NULL);

    if (pCursor)
    {
        pGraphDev->drv.ShowCursor(pGraphDev->pDev, CURSOR_GetPhysObj(pCursor), 
            nCursorShow);
    }
    else
        pGraphDev->drv.ShowCursor(pGraphDev->pDev, NULL, 0);

    return pOldCursor;
}

/*
**  Function : WS_GetCursor
*/
PCURSOROBJ WS_GetCursor(void)
{
    return pCursor;
}

/*
**  Function : WS_ShowCursor
**  Purpose  :
**      Show or hide the current cursor.
*/
int WS_ShowCursor(BOOL bShow)
{
    PGRAPHDEV pGraphDev;

    pGraphDev = GRAPHDEV_GetCursorScreen(0);
    ASSERT(pGraphDev != NULL);

    if (bShow)
    {
        nCursorShow++;
        if (pCursor && nCursorShow == 1)
            pGraphDev->drv.ShowCursor(pGraphDev->pDev, NULL, TRUE);
    }
    else
    {
        nCursorShow--;
        if (pCursor && nCursorShow == 0)
            pGraphDev->drv.ShowCursor(pGraphDev->pDev, NULL, FALSE);
    }

    return nCursorShow;
}

#endif // NOCURSOR

/*
**  Function : WS_SetCursorPos
*/
void WS_SetCursorPos(int x, int y)
{
    // Clip the coordinate by clip rectange

    if (x < rcCursor.left)
        x = rcCursor.left;
    else if (x > rcCursor.right - 1)
        x = rcCursor.right - 1;

    if (y < rcCursor.top)
        y = rcCursor.top;
    else if (y > rcCursor.bottom - 1)
        y = rcCursor.bottom - 1;

    if (nCursorX != x || nCursorY != y)
    {
        // Save current cursor postion
        nCursorX = x;
        nCursorY = y;
    }
}

void WS_GetCursorPos(PPOINT pPoint)
{
    ASSERT(pPoint != NULL);

    pPoint->x = nCursorX;
    pPoint->y = nCursorY;
}

/*
**  Function : WS_SetMaxCursorRect
**  Purpose  :
**      设置光标可以到达的最大矩形区域，调用ClipCursor函数时，如果指定的光
**      标裁剪矩形为NULL时，光标裁剪矩形将被设置为由本函数设置的最大矩形区
**      域。
**  Params   :
**      pRect : Points to the RECT structure that contains the screen 
**              coordinates of the upper-left and lower-right corners of 
**              the maximum confining rectangle. If this parameter is NULL,
**              the maximum confining rectangle is the screen rectangle. 
*/
void WS_SetMaxCursorRect(const RECT* pRect)
{
    if (pRect)
    {
        rcCursorMax = *pRect;
        NormalizeRect(&rcCursorMax);
    }
    else
    {
        rcCursorMax.left = 0;
        rcCursorMax.top = 0;
        rcCursorMax.right = DISPDEV_GetCaps(HORZRES);
        rcCursorMax.bottom = DISPDEV_GetCaps(VERTRES);

        CopyRect(&rcCursor, &rcCursorMax);
    }

    if (!IsRectEmpty(&rcCursor))
        IntersectRect(&rcCursor, &rcCursor, &rcCursorMax);
    else
        CopyRect(&rcCursor, &rcCursorMax);
}

/*
**  Function : WS_ClipCursor
*/
void WS_ClipCursor(const RECT* pRect)
{
    if (pRect)
        IntersectRect(&rcCursor, pRect, &rcCursorMax);
    else
        CopyRect(&rcCursor, &rcCursorMax);
}

/*
**  Function : WS_GetClipCursor
**  Purpose  :
**      
*/
void WS_GetClipCursor(PRECT pRect)
{
    ASSERT(pRect != NULL);

    *pRect = rcCursor;
}

/*
**  Function : WS_PtInCursorClipRect
*/
BOOL WS_PtInCursorClipRect(POINT pt)
{
    return PtInRect(&rcCursor, pt);
}

/*
**  Function : WS_ClipCursorPoint
*/
void WS_ClipCursorPoint(PPOINT pPoint)
{
    ASSERT(pPoint != NULL);

    if (pPoint->x < rcCursor.left)
        pPoint->x = rcCursor.left;
    else if (pPoint->x > rcCursor.right - 1)
        pPoint->x = rcCursor.right - 1;

    if (pPoint->y < rcCursor.top)
        pPoint->y = rcCursor.top;
    else if (pPoint->y > rcCursor.bottom - 1)
        pPoint->y = rcCursor.bottom - 1;
}
