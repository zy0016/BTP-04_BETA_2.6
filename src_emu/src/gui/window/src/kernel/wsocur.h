/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for cursor object
 *            
\**************************************************************************/

#ifndef __WSOCUR_H
#define __WSOCUR_H

#ifndef __DEVDISP_H
#include "devdisp.h"
#endif

typedef OBJHDR CURSOROBJHDR;

typedef struct
{
    CURSOROBJHDR    objhead;         // Handle for the object
    DRVLOGCURSOR    logcursor;      // Log cursor
    PPHYSCURSOR     pPhysCursor;    // Physical object
} CURSOROBJ, *PCURSOROBJ;

PCURSOROBJ  CURSOR_Create(int nType, int nRop, int nHotX, int nHotY, 
                          int nWidth, int nHeight, const void* pANDPlane, 
                          const void* pXORPlane);
void        CURSOR_Destroy(PCURSOROBJ pCursor);
PPHYSCURSOR CURSOR_GetPhysObj(PCURSOROBJ pCursor);

#endif //__WSOCUR_H
