/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for pen object
 *            
\**************************************************************************/

#ifndef __WSOPEN_H
#define __WSOPEN_H

#ifndef __DEVDISP_H
#include "devdisp.h"
#endif

typedef struct tagPENOBJ
{               
    XGDIOBJHDR  xgdihdr;    /* XGDI object header */
    DRVLOGPEN   logpen;     /* drv log pen struct   */
} PENOBJ, *PPENOBJ;

PPENOBJ     PEN_Create(int nStyle, int nWidth, int nEndStyle, 
                       int nFrameMode, COLORREF color, BOOL bShared);
BOOL        PEN_Destroy(PPENOBJ pPen);
void        PEN_GetLogObj(PPENOBJ pPen, PLOGPEN pLogPen);
/*
PPHYSPEN    PEN_CreatePhysObj(PPENOBJ pPen);
void        PEN_DestroyPhysObj(PPENOBJ pPen, PPHYSPEN pPhysPen);
PPHYSPEN    PEN_GetPhysObj(PPENOBJ pPen);
void        PEN_ReleasePhysObj(PPENOBJ pPen, PPHYSPEN pPhysPen);
*/
#endif //__WSOPEN_H
