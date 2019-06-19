/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/


#include <display.h>
#include "hpdisp.h"

static LDD_MODEINFO ModeInfo[MAX_SCREEN] =
{
    {
        LDD_MODE,
        GETPIXBYTES(LDD_MODE) * 8,
        FALSE,
        0, 
        0
    },
    {
        DDM_MONO,
        1,
        FALSE,
        0, 
        0
    },
    {
        DDM_OTHER,
        0,
        FALSE,
        0, 
        0
    },
};

extern PDISPLAYDRVINFO EnableRgbDisplayDev(struct DisplayDrvInfo * pDisplay);
extern PDISPLAYDRVINFO EnableMonoDisplayDev(struct DisplayDrvInfo * pDisplay);

static int OpenDevice(PDEV pDev, int dev)
{
    PDISPLAYDRVINFO pDisplayDriver;

    switch(dev)
    {
    case SCREEN_MAIN:
        pDisplayDriver = EnableRgbDisplayDev(NULL);
    	break;
    case SCREEN_SECOND:
        pDisplayDriver = EnableMonoDisplayDev(NULL);
    	break;
    default:
        return -1;
    }

    return pDisplayDriver->OpenDevice(pDev, dev);
}

static int CloseDevice(PDEV pDev)
{
    return pDev->pDDI->CloseDevice(pDev);
}

static int SuspendDevice(PDEV pDev)
{
    return pDev->pDDI->SuspendDevice(pDev);
}

static int ResumeDevice(PDEV pDev)
{
    return pDev->pDDI->ResumeDevice(pDev);
}

static int GetDevInfo(PDEV pDev, PDISPLAYDEVINFO pInfo)
{
    return pDev->pDDI->GetDevInfo(pDev, pInfo);
}

static int EnumMode(PDEV pDev, int modenum, PDISPMODE pDispMode)
{
    return pDev->pDDI->EnumMode(pDev, modenum, pDispMode);
}
static int ChangeMode(PDEV pDev, PDISPMODE pDispMode, uint32 flags)
{
    return pDev->pDDI->ChangeMode(pDev, pDispMode, flags);
}

static int CreateMemoryDevice(PDEV pDev, int width, int height,
                PMEMDEV pMemDev)
{
    return pDev->pDDI->CreateMemoryDevice(pDev, width, height, pMemDev);
}

static int DestroyMemoryDevice(PDEV pDev)
{
    return pDev->pDDI->DestroyMemoryDevice(pDev);
}

static int CreateBitmap(PDEV pDev, int width, int height, PPHYSBMP pPhysBmp)
{
    return pDev->pDDI->CreateBitmap(pDev, width, height, pPhysBmp);
}

static int DestroyBitmap(PDEV pDev, PPHYSBMP pPhysBmp)
{
    return pDev->pDDI->DestroyBitmap(pDev, pPhysBmp);
}

static int SetBitmapBuffer(PDEV pDev, PPHYSBMP pPhysBmp, void* pBuffer)
{
    return pDev->pDDI->SetBitmapBuffer(pDev, pPhysBmp, pBuffer);
}

static int RealizePen(PDEV pDev, PDRVLOGPEN pLogPen, PPHYSPEN pPhysPen)
{
    return pDev->pDDI->RealizePen(pDev, pLogPen, pPhysPen);
}
static int UnrealizePen(PDEV pDev, PPHYSPEN pPen)
{
    return pDev->pDDI->UnrealizePen(pDev, pPen);
}

static int RealizeBrush(PDEV pDev, PDRVLOGBRUSH pLogBrush, 
                        PPHYSBRUSH pPhysBrush)
{
    return pDev->pDDI->RealizeBrush(pDev, pLogBrush, pPhysBrush);
}

static int UnrealizeBrush(PDEV pDev, PPHYSBRUSH pBrush)
{
    return pDev->pDDI->UnrealizeBrush(pDev, pBrush);
}

static int RealizePalette(PDEV pDev, PDRVLOGPALETTE pLogPalette,
             PPHYSPALETTE pPhysPalette)
{
    return pDev->pDDI->RealizePalette(pDev, pLogPalette, pPhysPalette);
}

static int UnrealizePalette(PDEV pDev, PPHYSPALETTE pPalette)
{
    return pDev->pDDI->UnrealizePalette(pDev, pPalette);
}

static int RealizeColor(PDEV pDev, PPHYSPALETTE pPalette, int32 color)
{
    return pDev->pDDI->RealizeColor(pDev, pPalette, color);
}

static int SelectPalette(PDEV pDev, PPHYSPALETTE pPalette)
{
    return pDev->pDDI->SelectPalette(pDev, pPalette);
}

static int RealizeCursor(PDEV pDev, PDRVLOGCURSOR pLogCursor,
                           PPHYSCURSOR pPhysCursor)
{
    return pDev->pDDI->RealizeCursor(pDev, pLogCursor, pPhysCursor);
}

static int UnrealizeCursor(PDEV pDev, PPHYSCURSOR pCursor)
{
    return pDev->pDDI->UnrealizeCursor(pDev, pCursor);
}

static int ShowCursor(PDEV pDev, PPHYSCURSOR pCursor, int mode)
{
    return pDev->pDDI->ShowCursor(pDev, pCursor, mode);
}

static int SetCursorPos(PDEV pDev, int x, int y)
{
    return pDev->pDDI->SetCursorPos(pDev, x, y);
}

static int CheckCursor(PDEV pDev)
{
    return pDev->pDDI->CheckCursor(pDev);
}

static int UpdateScreen(PDEV pDev)
{
    return pDev->pDDI->UpdateScreen(pDev);
}

static int DrawGraph(PDEV pDev, int func, PGRAPHPARAM pGraphParam, 
                     void* pDrawData)
{
    return pDev->pDDI->DrawGraph(pDev, func, pGraphParam, pDrawData);
}

static int BitBlt(PDEV pDev, PRECT pDestRect, PDEV pSrcDev,
                  int x, int y, PGRAPHPARAM pGraphParam)
{
    return pDev->pDDI->BitBlt(pDev, pDestRect, pSrcDev, x, y, pGraphParam);
}

static int PatBlt(PDEV pDev, PRECT pRect, PGRAPHPARAM pGraphParam)
{
    return pDev->pDDI->PatBlt(pDev, pRect, pGraphParam);
}

static int DIBitBlt(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                    int x, int y, PGRAPHPARAM pGraphParam)
{
    return pDev->pDDI->DIBitBlt(pDev, pDestRect, pBitmap, x, y, pGraphParam);
}

static int TextBlt(PDEV pDev, int x, int y, PTEXTBMP pTextBmp, 
                   PGRAPHPARAM pGraphParam)
{
    return pDev->pDDI->TextBlt(pDev, x, y, pTextBmp, pGraphParam);
}

static int StretchBlt(PDEV pDev, PRECT pDestRect, PDEV pSrcDev, 
                        PRECT pSrcRect, PGRAPHPARAM pGraphParam)
{
    return pDev->pDDI->StretchBlt(pDev, pDestRect, pSrcDev, pSrcRect, pGraphParam);
}

static int DIStretchBlt(PDEV pDev, PRECT pDstRect, PDIBITMAP pBitmap, 
                        PRECT pSrcRect, PGRAPHPARAM pGraphParam)
{
    return pDev->pDDI->DIStretchBlt(pDev, pDstRect, pBitmap, pSrcRect, pGraphParam);
}

static int SaveScreen(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect)
{
    return pDev->pDDI->SaveScreen(pDev, pScreenSave, pRect);
}

static int RestoreScreen(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect)
{
    return pDev->pDDI->RestoreScreen(pDev, pScreenSave, pRect);
}

static int ScrollScreen(PDEV pDev, PRECT pRect, PRECT pClipRect,
                          int x, int y)
{
    return pDev->pDDI->ScrollScreen(pDev, pRect, pClipRect, x, y);
}

/* Define display device driver interface   */
static struct DisplayDrvInfo display_device = 
{
    8,                          /* 8 display mode, as 8 devices */
    FM_SUSPENDDEVICE            /* Function mask1       */
    | FM_RESUMEDEVICE
    | FM_CHECKCURSOR
    | FM_UPDATESCREEN
    | FM_SCROLLSCREEN,
    0,                          /* Function mask2       */
    OpenDevice,
    CloseDevice,
    SuspendDevice,
    ResumeDevice,
    GetDevInfo,
    EnumMode,
    ChangeMode,
    CreateMemoryDevice,
    DestroyMemoryDevice,
    CreateBitmap,
    DestroyBitmap,
    SetBitmapBuffer, 
    RealizePen,
    UnrealizePen,
    RealizeBrush,
    UnrealizeBrush,
    RealizePalette,
    UnrealizePalette,
    RealizeColor,
    SelectPalette,
    RealizeCursor,
    UnrealizeCursor,
    ShowCursor,
    SetCursorPos,
    CheckCursor,
    UpdateScreen,
    DrawGraph, 
    BitBlt,
    PatBlt,
    DIBitBlt,
    TextBlt, 
    StretchBlt,
    DIStretchBlt,
    SaveScreen,
    RestoreScreen,
    ScrollScreen,
};

/*********************************************************************\
* Function	   EnableDisplayDev
* Purpose      Display driver entry point.
* Params	   pDisplay
* Return       The display device driver interface pointer 
* Remarks	   
**********************************************************************/
PDISPLAYDRVINFO EnableDisplayDev(struct DisplayDrvInfo * pDisplay)
{
    if (pDisplay != NULL)
    {   /*Copy the initialized data to caller*/
        *pDisplay = display_device;
        return pDisplay;
    }
    else
        return &display_device;
}

#ifdef _MSC_VER
_declspec(dllexport) 
#endif
BOOL LDD_GetDisplayDriverInfo(int nDev, PLDD_MODEINFO pDisplayModeInfo)
{
    int i = nDev;

    if (pDisplayModeInfo == NULL)
        return FALSE;

    if (i > MAX_SCREEN)
        i = MAX_SCREEN;

    while (i--)
    {
        pDisplayModeInfo[i] = ModeInfo[i];
    }

    return TRUE;
}
