/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements GDI API functions.
 *            
\**************************************************************************/

#include "hpwin.h"

#ifdef RGNSUPPORT

#include "wsorgn.h"
#include "wsobj.h"

static void AddRectIntoRgn(RGNOBJ *pRgn, RECT rc);
static void RgnAndRect(RGNOBJ *pRgn, RGNOBJ *pRgnResult, RECT rcAnd);
static void RgnOrRect(RGNOBJ *pRgn, RECT rcOr);

HRGN WINAPI CreateRectRgn(int nLeftRect, int nTopRect, int nRightRect, 
                          int nBottomRect)
{
    PRGNOBJ pRgn = NULL;

    ENTERMONITOR;

    pRgn = RGN_Create(nLeftRect, nTopRect, nRightRect, nBottomRect );
    
    LEAVEMONITOR;

    if (!pRgn)
    {
        SetLastError(1);
        return NULL;
    }

    return (HRGN)WOT_GetHandle(pRgn);
}

HRGN WINAPI CreateRectRgnIndirect( CONST RECT *lprc )
{
    PRGNOBJ pRgn = NULL;
    int nLeftRect, nTopRect, nRightRect, nBottomRect;
    
    nLeftRect = lprc->left;
    nTopRect = lprc->top;
    nRightRect = lprc->right;
    nBottomRect = lprc->bottom;

    ENTERMONITOR;

    pRgn = RGN_Create(nLeftRect, nTopRect, nRightRect, nBottomRect );

    LEAVEMONITOR;
 
    if (!pRgn)
    {
        SetLastError(1);
        return NULL;
    }

    return (HRGN)WOT_GetHandle(pRgn);
}
 
int WINAPI CombineRgn( HRGN hrgnDest, HRGN hrgnSrc1, HRGN hrgnSrc2, int fnCombineMode )
{
    PRGNOBJ pRgnDest, pRgnSrc1, pRgnSrc2;
    int nRet;
    
    if((hrgnDest == NULL) || (hrgnSrc1 == NULL) || (hrgnSrc2 == NULL))
    {
        return ERROR ;
    }

    ENTERMONITOR;

    pRgnDest = (PRGNOBJ)WOT_LockObj((HANDLE)hrgnDest, OBJ_RGN);
    if (pRgnDest == NULL)
    {
        LEAVEMONITOR;
        return ERROR;
    }

    pRgnSrc1 = (PRGNOBJ)WOT_LockObj((HANDLE)hrgnSrc1, OBJ_RGN);
    if (pRgnSrc1 == NULL)
    {
        WOT_UnlockObj(pRgnDest, OBJ_RGN);

        LEAVEMONITOR;
        return ERROR;
    }

    pRgnSrc2 = (PRGNOBJ)WOT_LockObj((HANDLE)hrgnSrc2, OBJ_RGN);
    if (pRgnSrc2 == NULL)
    {
        WOT_UnlockObj(pRgnDest, OBJ_RGN);
        WOT_UnlockObj(pRgnSrc1, OBJ_RGN);

        LEAVEMONITOR;
        return ERROR;
    }

    nRet = RGN_Combine(pRgnDest, pRgnSrc1, pRgnSrc2, fnCombineMode);

    WOT_UnlockObj(pRgnDest, OBJ_RGN);
    WOT_UnlockObj(pRgnSrc1, OBJ_RGN);
    WOT_UnlockObj(pRgnSrc2, OBJ_RGN);

    LEAVEMONITOR;

    return nRet;
}

int WINAPI OffsetRgn( HRGN hrgn, int nXOffset, int nYOffset )
{
    PRGNOBJ pRgn = NULL;
    int nRet;

    if(hrgn == NULL)
        return ERROR;

    ENTERMONITOR;

    pRgn = (PRGNOBJ)WOT_LockObj((HANDLE)hrgn, OBJ_RGN);
    if (pRgn == NULL)
    {
        LEAVEMONITOR;
        return ERROR;
    }

    nRet =  RGN_Offset(pRgn, nXOffset, nYOffset );

    WOT_UnlockObj(pRgn, OBJ_RGN);

    LEAVEMONITOR;

    return nRet;

}

#else   // RGNSUPPORT

static void UnusedFunc(void)
{
}

#endif // RGNSUPPORT
