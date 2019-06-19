/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for brush object.
 *            
\**************************************************************************/

#ifndef __WSOBRUSH_H
#define __WSOBRUSH_H

#ifndef __DEVDISP_H
#include "devdisp.h"
#endif

typedef struct
{
    XGDIOBJHDR  xgdihdr;    /* XGDI object header */
    DRVLOGBRUSH logbrush;       /* logbrush struct          */
} BRUSHOBJ, *PBRUSHOBJ;

PBRUSHOBJ   BRUSH_Create(int nStyle, COLORREF color, LONG lHatch, BOOL bShared);
BOOL        BRUSH_Destroy(PBRUSHOBJ pBrushObj);
void        BRUSH_GetLogObj(PBRUSHOBJ pBrushObj, PLOGBRUSH pLogBrush);
/*
PPHYSBRUSH  BRUSH_CreatePhysObj(PBRUSHOBJ pBrushObj);
void        BRUSH_DestroyPhysObj(PBRUSHOBJ pBrush, PPHYSBRUSH pPhysBrush);
PPHYSBRUSH  BRUSH_GetPhysObj(PBRUSHOBJ pBrushObj);
void        BRUSH_ReleasePhysObj(PBRUSHOBJ pBrush, PPHYSBRUSH pPhysBrush);
*/
void        BRUSH_GetSize(PBRUSHOBJ pBrushObj, PSIZE pSize);

#endif //__WSOBRUSH_H
