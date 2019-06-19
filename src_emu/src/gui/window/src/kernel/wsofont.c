/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Manages the font object
 *            
\**************************************************************************/

#include "hpwin.h"

#include "string.h"
#include "wsobj.h"
#include "wsofont.h"

#if (__MP_PLX_GUI)
#define FONT_MemAlloc(size, bShared)     \
    ((bShared) ? MemAlloc(size) : WIN_LocalAlloc(LMEM_FIXED, size))
#define FONT_MemFree(p, bShared)        \
    ((bShared) ? MemFree(p) : WIN_LocalFree(p))
#else //__MP_PLX_GUI
#define FONT_MemAlloc(size, bShared)     MemAlloc(size)
#define FONT_MemFree(p, bShared)         MemFree(p)
#endif

/*
**  Function : FONT_Create
**  Purpose  :
**      Creates a logical font that has specific characteristics. 
*/
PFONTOBJ FONT_Create(CONST LOGFONT* pLogFont, BOOL bShared)
{
    PFONTOBJ pFontObj;

    ASSERT(pLogFont != NULL);

    pFontObj = (PFONTOBJ)FONT_MemAlloc(sizeof(FONTOBJ), bShared);
    
    if (!pFontObj)
    {
        SetLastError(1);
        return NULL;
    }

    pFontObj->logfont = *pLogFont;

    if (!WOT_RegisterObj((PWSOBJ)pFontObj, OBJ_FONT, bShared))
    {
        FONT_MemFree(pFontObj, bShared);

        SetLastError(1);
        return NULL;
    }

    return pFontObj;
}

/*
**  Function : FONT_Destroy
*/
BOOL FONT_Destroy(PFONTOBJ pFont)
{
    ASSERT(pFont != NULL);

    // Can't destory a stock object

    if(WOT_IsStockObj((PXGDIOBJ)pFont))
        return FALSE;

    ASSERT(ISDELETEDXGDIOBJ(pFont));

    WOT_UnregisterObj((PWSOBJ)pFont);

    FONT_MemFree(pFont, WOT_IsSharedObj(pFont));

    return TRUE;
}

/*
**  Function : FONT_CreateFontDev
**  Purpose  :
**      Creates a font device from a specifed font object
*/
PFONTDEV FONT_CreateFontDev(PFONTOBJ pFont)
{
    int nRet;
    PFONTDEV pFontDev;
    DRVLOGFONT DrvLogFont;

    ASSERT(pFont!= NULL);

    // Fills DRVLOGFONT struct using LOGFONT
    
    strncpy(DrvLogFont.name, pFont->logfont.lfFaceName, 15);
    DrvLogFont.name[15] = 0;
    
    DrvLogFont.family     = pFont->logfont.lfPitchAndFamily;
    
    DrvLogFont.style      = 0;
    if (pFont->logfont.lfItalic)
        DrvLogFont.style |= FS_ITALIC;
    if (pFont->logfont.lfUnderline)
        DrvLogFont.style |= FS_UNDERLINE;
    if (pFont->logfont.lfStrikeOut)
        DrvLogFont.style |= FS_STRIKEOUT;
    if (pFont->logfont.lfWeight == FW_BOLD)
        DrvLogFont.style |= FS_BOLD;
    
    DrvLogFont.weight     = (int16)pFont->logfont.lfWeight;
    DrvLogFont.size       = (int16)pFont->logfont.lfHeight;
    DrvLogFont.proportion = 100;
    DrvLogFont.charset    = pFont->logfont.lfCharSet;
    
    if (DrvLogFont.size < 0)
        DrvLogFont.size = -DrvLogFont.size;
    
    nRet = g_FontDrv.CreateFont(NULL, &DrvLogFont);
    if (nRet <= 0)
        return NULL;
    
    pFontDev = (PFONTDEV)MemAlloc(nRet);
    if (!pFontDev)
        return NULL;
    
    nRet = g_FontDrv.CreateFont(pFontDev, &DrvLogFont);
    if (nRet <= 0)
    {
        MemFree(pFontDev);
        return NULL;
    }

    return pFontDev;
}

/*
**  Function : FONT_DestroyFontDev
**  Purpose  :
**      Destroys a specified font device of a specified font object.
*/
void FONT_DestroyFontDev(PFONTOBJ pFont, PFONTDEV pFontDev)
{
    ASSERT(pFontDev != NULL);

    g_FontDrv.DestroyFont(pFontDev);
    MemFree(pFontDev);
}

