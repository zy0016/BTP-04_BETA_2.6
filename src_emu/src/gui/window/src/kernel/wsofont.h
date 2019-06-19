/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for font object
 *            
\**************************************************************************/

#ifndef __WSOFONT_H
#define __WSOFONT_H

#ifndef __DEVFONT_H
#include "devfont.h"
#endif

typedef struct
{
    XGDIOBJHDR  xgdihdr;        /* XGDI object header */
    LOGFONT     logfont;        /* log font struct          */
} FONTOBJ, *PFONTOBJ;

PFONTOBJ FONT_Create(CONST LOGFONT* pLogFont, BOOL bShared);
BOOL     FONT_Destroy(PFONTOBJ pFont);
PFONTDEV FONT_CreateFontDev(PFONTOBJ pFont);
void     FONT_DestroyFontDev(PFONTOBJ pFont, PFONTDEV pFontDev);
PFONTDEV FONT_GetFontDev(PFONTOBJ pFont);
void     FONT_ReleaseFontDev(PFONTOBJ pFont, PFONTDEV pFontDev);

#endif //__WSOFONT_H
