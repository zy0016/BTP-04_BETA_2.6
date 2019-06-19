/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display driver
 *
 * Purpose  : Implement the cursor operation.
 *            
\**************************************************************************/

int InitCursor(PDEV pDev)
{
    return 1;
}

static int RealizeCursor(PDEV pDev, PDRVLOGCURSOR pLogCursor,
                         PPHYSCURSOR pPhysCursor)
{
    return 1;
}

static int UnrealizeCursor(PDEV pDev, PPHYSCURSOR pCursor)
{
    return 0;
}

static int ShowCursor(PDEV pDev, PPHYSCURSOR pCursor, int mode)
{
    return 0;
}

static int SetCursorPos(PDEV pDev, int x, int y)
{
    return 0;
}

static int CheckCursor(PDEV pDev)
{
    return 0;
}

/*
**  Function : _ExcludeCursor
**
**  Purpose  :
**      Hide the current cursor if the cursor area is intersected with
**      the specified rectangle(draw operation area).
**
**  Params   :
**      nX      : X coordinate of the specified rectangle.
**      nY      : Y coordinate of the specified rectangle.
**      nWidth  : Width of the specified rectangle.
**      nHeight : Height of the specified rectangle.
**
**  Remarks  :
**
**      Each drawing operation must call this function first.
*/
static void _ExcludeCursor(PDEV pDev, PRECT pRC)
{
    RECT rc;
    /* memory device, return directly. */
    if (pDev->bUpdate < 0)
        return;

    /* In real environment, bUpdate == 0 means LCDC, == 1 means no LCDC */
#ifndef _EMULATE_
    if (pDev->bUpdate == 1)
#endif
    {
        rc = *pRC;
        NormalizeRect(&rc);
        MergeUpdate(pDev, rc.x1, rc.y1, rc.x2, rc.y2);
    }

    return;
}

static void _UnexcludeCursor(PDEV pDev)
{
#ifdef _EMULATE_
    if (pDev->bUpdate > 0)
        UpdateScreen(pDev);
#endif

    return;
}
