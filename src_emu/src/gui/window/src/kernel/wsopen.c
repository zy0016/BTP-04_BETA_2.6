/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Pen object management module. 
 *            
\**************************************************************************/

#include "hpwin.h"
#include "wsobj.h"
#include "wsopen.h"

#if (__MP_PLX_GUI)
#define PEN_MemAlloc(size, bShared)     \
    ((bShared) ? MemAlloc(size) : WIN_LocalAlloc(LMEM_FIXED, size))
#define PEN_MemFree(p, bShared)        \
    ((bShared) ? MemFree(p) : WIN_LocalFree(p))
#else //__MP_PLX_GUI
#define PEN_MemAlloc(size, bShared)      MemAlloc(size)
#define PEN_MemFree(p, bShared)          MemFree(p)
#endif

/*
**  Function : PEN_Create
**  Purpose  :
**      Creates a pen that has the specified width, style and color.
**  Params   :
**      nWidth : Specifies the width of the pen to be created.
**      nStyle : Specifies the style of the pen to be created.
**      color  :  Specifies the color of the pen to be created.
**  Return   :
**      If the function succeeds, the return value is a pointer that 
**      points a the pen. If the function fails, the return value is 
**      NULL. 
*/
PPENOBJ PEN_Create(int nStyle, int nWidth, int nEndStyle, int nFrameMode, 
                   COLORREF color, BOOL bShared)
{
    PPENOBJ pPen;

    // If the pen style is an invalid style, change the pen style to 
    // PS_SOLID style
    if (nStyle < 0 || nStyle > PS_LAST)
        nStyle = PS_SOLID;

    if (nStyle == PS_INSIDEFRAME)
    {
        nStyle = PS_SOLID;
        nFrameMode = PFM_INTERNAL;
    }

    // If the pen width is an invalid width, change the pen width to 1
    if (nWidth <= 0)
        nWidth = 1;

    // Only PS_SOLID pen support wide pen
    if (nStyle != PS_NULL && nWidth > 1)
        nStyle = PS_SOLID;

    // If the pen end style is an invalid style, change the end style to
    // PES_DEFAULT
    if (nEndStyle != PES_RECTANGLE && nEndStyle != PES_TRIANGLE &&
        nEndStyle != PES_CIRCLE)
        nEndStyle = PES_DEFAULT;

    // If the frame mode is an invalid mode, change the frame mode to 
    // PFM_INTERNAL
    if (nFrameMode != PFM_INTERNAL)
        nFrameMode = PFM_CENTER;

    pPen = (PPENOBJ)PEN_MemAlloc(sizeof(PENOBJ), bShared);

    // Ensure MemAlloc function succeeds
    if (!pPen)
    {
        SetLastError(1);
        return NULL;
    }

     // Ensure the pen object is added in object table successfully
    if (!WOT_RegisterObj((PWSOBJ)pPen, OBJ_PEN, bShared))
    {
        pPen = (PPENOBJ)PEN_MemAlloc(sizeof(PENOBJ), bShared);
        
        SetLastError(1);
        return NULL;
    }

    pPen->logpen.style = (BYTE)nStyle;
    pPen->logpen.width = (BYTE)nWidth;
    pPen->logpen.end_style = (BYTE)nEndStyle;
    pPen->logpen.frame_mode = (BYTE)nFrameMode;
    pPen->logpen.color = color;

    return pPen;
}

/*
**  Function : PEN_Destroy
**  Purpose  :
**      Destroys a specified pen.
**  Params   :
**      pPen : Specifies the pen to be destroyed.
*/
BOOL PEN_Destroy(PPENOBJ pPen)
{
    ASSERT(pPen != NULL);

    // Can't destroy a stock object
    if(WOT_IsStockObj((PXGDIOBJ)pPen))
        return FALSE;

    ASSERT(ISDELETEDXGDIOBJ(pPen));

    // Remove the pen object from window object table
    WOT_UnregisterObj((PWSOBJ)pPen);

    PEN_MemFree(pPen, WOT_IsSharedObj(pPen));

    return TRUE;
}

/*
**  Function : PEN_GetLogObj
**  Purpose  :
**      Gets information about a specified pen object.
*/
void PEN_GetLogObj(PPENOBJ pPen, PLOGPEN pLogPen)
{
    ASSERT(pPen != NULL);
    ASSERT(pLogPen != NULL);

    pLogPen->lopnStyle      = pPen->logpen.style;
    pLogPen->lopnWidth.x    = pPen->logpen.width;
    pLogPen->lopnWidth.y    = 0;
    pLogPen->lopnColor      = pPen->logpen.color;
}

