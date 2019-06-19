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



#include <hp_kern.h>
#include <hp_diag.h>
#include <display.h>
#include "dispcomn.h"
#include "dispconf.h"
#include "hpdisp.h"
#include "string.h"
#include "stdlib.h"

/*
**  define pattern data used for the style line and pattern brush :
**  It will be used in both the Real and the Emulate environment.
*/

#include "pattern.h"

/* dispfunc.h define the functions in the display driver */
#include "dispfunc.h"

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

static PDISPLAYDEV pDisplayDev;

//#include "region.c"

/*
**  implements the screen functions :
**
*/

#include "screen.c"

/*
**  implements the block transfer functions :
**
**      BitBlt
**      PatBlt
**      DIBitBlt
**      TextBlt
**      StretchBlt
*/

#if (LDD_MODE == DDM_RGB16_565 || LDD_MODE == DDM_RGB12_4440)
#include "bltnto16.c"
#elif (LDD_MODE == DDM_RGB24_BGR || LDD_MODE == DDM_RGB24_RGB || \
        LDD_MODE == DDM_RGB18_BGR || LDD_MODE == DDM_RGB18_RGB)
#include "bltnto24.c"
#endif

#include "bitblt.c"
#include "patblt.c"
#include "dibitblt.c"
#include "strblt.c"
#include "distrblt.c"

/*
**  implements line and rect functions :
**
**      DrawLine
**      DrawRect
*/

#include "line.c"

#if (TOUCH_DEVICE)
#include "dddline.c"
#endif

/*
**  implements circle functions :
**
**      DrawCircle
*/

#include "circle.c"

/*
**  implements ellipse functions :
**
**      DrawEllipse
*/

#include "ellipse.c"

/*
**  implements polygon functions :
**
**      DrawPolygon
*/

#include "polygon.c"

/*
**  implements the cursor functions :
**
*/

#include "cursor.c"

/*********************************************************************\
* Function	   EnableRgbDisplayDev
* Purpose      Display driver entry point.
* Params	   pDisplay
* Return       The display device driver interface pointer 
* Remarks	   
**********************************************************************/
PDISPLAYDRVINFO EnableRgbDisplayDev(struct DisplayDrvInfo * pDisplay)
{
    if (pDisplay != NULL)
    {   /*Copy the initialized data to caller*/
        *pDisplay = display_device;
        return pDisplay;
    }
    else
        return &display_device;
}

/*********************************************************************\
* Function  OpenDevice 
* Purpose   Initialize one Display device.
* Params	   
   pDev     Pointer to display device structure.

   dev      Device number. No use now because the system mode has been 
            defined and the real display mode should fit the system mode.
           
* Return    Success: sizeof(DISPLAYDEV)	   
            Failed:  -1
* Remarks	   
**********************************************************************/
static int OpenDevice(PDEV pDev, int dev)
{
    struct OS_DisplayMode DspMode;

    /* Get display mode from OS kernel*/
    if (!PDD_GetDisplayMode(dev, &DspMode))
        return -1;

    /* If the parameter pDev is NULL, just return the size of DISPLAYDEV */
    if (pDev)
    {
        /* Do some initialize */
        pDev->mode = DEV_PHYSICAL;
        pDev->width = DspMode.dsp_width;
        pDev->height = DspMode.dsp_height;
        pDev->line_bytes = DspMode.dsp_scanline_bytes;;
        pDev->pdata = (uint8*)DspMode.dsp_screen_buffer;
        SetRect(&pDev->rcUpdate, 10000, 10000, 0, 0);
        pDev->bUpdate = DspMode.dsp_updatescreen == 0 ? FALSE : TRUE;
        pDev->pDDI = &display_device;
        pDev->device_no = dev;

#if (GETPIXBYTES(LDD_MODE) == 3)
        pDev->pix_bits  = 24;
        pDev->bmpmode = BMP_RGB24;
#elif (GETPIXBYTES(LDD_MODE) == 2)
        pDev->pix_bits  = 16;
    #if (GETPIXCOLORBITS(LDD_MODE) == 16)
        pDev->bmpmode = BMP_RGB16;
    #elif (GETPIXCOLORBITS(LDD_MODE) == 12)
        pDev->bmpmode = BMP_RGB12;
    #endif
#elif (GETPIXBYTES(LDD_MODE) == 4)
        pDev->pix_bits  = 32;
        pDev->bmpmode = BMP_RGB32;
#endif

        pDisplayDev = pDev;
    }

    return sizeof(DISPLAYDEV);
}

/*********************************************************************\
* Function  CloseDevice
* Purpose   Disable one Display device.
* Params
    pDev    Pointer to display device structure.	   
* Return    0	 	   
* Remarks	   
**********************************************************************/
static int CloseDevice(PDEV pDev)
{
    return 0;
}

/*********************************************************************\
* Function  SuspendDevice
* Purpose   Set device to power save mode.
* Params
    pDev    Pointer to display device structure.	   
* Return    0	 	   
* Remarks	   
**********************************************************************/
static int SuspendDevice(PDEV pDev)
{
    return 0;
}

/*********************************************************************\
* Function  SuspendDevice
* Purpose   Set device to active mode.
* Params
    pDev    Pointer to display device structure.	   
* Return    0	 	   
* Remarks	   
**********************************************************************/
static int ResumeDevice(PDEV pDev)
{
    return 0;
}

/*********************************************************************\
* Function  GetDevInfo
* Purpose   return device information.
* Params
    pDev    Pointer to display device structure.	
            If value of pDev less than 256, pDev is a device number.
    pInfo   pointer to device info structure.
* Return    1	 	   
* Remarks	   
**********************************************************************/
static int GetDevInfo(PDEV pDev, PDISPLAYDEVINFO pInfo)
{
    if (pDev->mode == DEV_PHYSICAL)
    {
        pInfo->attrib           = 0;
        pInfo->width            = pDev->width;
        pInfo->height           = pDev->height;
        pInfo->planes           = 1;
        pInfo->bits_pixel       = (int8)(pDev->pix_bits);
        pInfo->mode             = (int8)DDM_OTHER;
        pInfo->phys_width       = 60;    /*mm*/
        pInfo->phys_height      = 80;    /*mm*/
    }
    else
    {
        pInfo->attrib           = 0;
        pInfo->width            = pDev->width;
        pInfo->height           = pDev->height;
        pInfo->planes           = 1;
        pInfo->bits_pixel       = (int8)(pDev->pix_bits);
        pInfo->mode             = (int8)DDM_OTHER;
        pInfo->phys_width       = 0;    
        pInfo->phys_height      = 0;    
    }

    return 1;
}

/*********************************************************************\
* Function  EnumMode
* Purpose   get the display mode supported by the GE
* Params
    pDev        Pointer to display device structure.
    modenum     The display mode index.
    pDispMode   pointer to DISPMODE structure.
* Return    0	 	   
* Remarks	   
**********************************************************************/
static int EnumMode(PDEV pDev, int modenum, PDISPMODE pDispMode)
{
    return -1;
}

/*********************************************************************\
* Function  ChangeMode
* Purpose   change the display mode
* Params
    pDev        Pointer to display device structure.
    pDispMode   pointer to the DISPMODE structure that will be set.
    flags       used to define effective element of the DISPMODE structure.
* Return    
    Success:    DISP_CHANGE_SUCCESSFUL  = 0
    Failed:     DISP_CHANGE_FAILED      = -1
    Not fited:  DISP_CHANGE_BADMODE     = -2
* Remarks	   
**********************************************************************/
static int ChangeMode(PDEV pDev, PDISPMODE pDispMode, uint32 flags)
{
    return DISP_CHANGE_FAILED;
}

/*********************************************************************\
* Function  CreateMemoryDevice
* Purpose   Create memory device.
* Params
    pDev    Pointer to display device structure.
    width   Width of memory device.
    height  Height of memory device.
    pMemDev Pointer to memory device structure.
* Return    
    Success:    sizeof(MEMDEV) + height * line_bytes. 
    Failed:     -1
* Remarks	   
**********************************************************************/
static int CreateMemoryDevice(PDEV pDev, int width, int height,
                PMEMDEV pMemDev)
{
    int line_bytes;

    if (pDev == NULL || width <= 0 || width > MAX_MEMDEVWIDTH
            || height < 0 || height > MAX_MEMDEVHEIGHT)
        return -1;

    line_bytes = (width * pDev->pix_bits + 31) / 32 * 4;

    if (pMemDev != NULL)
    {   /* Do some initialize */
        pMemDev->mode       = DEV_MEMORY;
        pMemDev->bmpmode    = pDev->bmpmode;
        pMemDev->width      = width;
        pMemDev->height     = height;
        pMemDev->line_bytes = line_bytes;
        pMemDev->pix_bits  = pDev->pix_bits;
        pMemDev->pdata      = (uint8 *)(pMemDev + 1);

        pMemDev->bUpdate    = -1;
        SetRect(&pMemDev->rcUpdate, 0, 0, 0, 0);
        pMemDev->pDDI = pDev->pDDI;
    }

    return sizeof(MEMDEV) + height * line_bytes;
}

/*********************************************************************\
* Function  DestroyMemoryDevice
* Purpose   Destroy memory device.
* Params
    pDev    Pointer to memory device structure.
* Return    0
* Remarks	   
**********************************************************************/
static int DestroyMemoryDevice(PDEV pMemDev)
{
    return 0;
}

/*********************************************************************\
* Function  CreateBitmap
* Purpose   Create Bitmap.
* Params
    pDev    Pointer to display device structure.
    width   Width of memory device.
    height  Height of memory device.
    pPhysBmp Pointer to physical bitmap structure.
* Return    sizeof(PHYSBMP) 
* Remarks	   
**********************************************************************/
static int CreateBitmap(PDEV pDev, int width, int height, PPHYSBMP pPhysBmp)
{
    int line_bytes;

    /* If pDev == NULL, create mono bitmap */
    if (pDev == NULL)
    {
        line_bytes = (width + 31) / 32 * 4;
        if (pPhysBmp != NULL)
        {   /* Do some initialize */
            pPhysBmp->mode       = DEV_MEMORY;
            pPhysBmp->bmpmode    = BMP_MONO;
            pPhysBmp->width      = width;
            pPhysBmp->height     = height;
            pPhysBmp->line_bytes = line_bytes;
            pPhysBmp->pix_bits   = 1;
            pPhysBmp->pdata      = NULL;
            
            pPhysBmp->bUpdate    = -1;
            SetRect(&pPhysBmp->rcUpdate, 0, 0, 0, 0);
            pPhysBmp->pDDI       = &display_device;
        }
    }
    else
    {
        line_bytes = (width * pDev->pix_bits + 31) / 32 * 4;
        if (pPhysBmp != NULL)
        {   /* Do some initialize */
            pPhysBmp->mode       = DEV_MEMORY;
            pPhysBmp->bmpmode    = pDev->bmpmode;
            pPhysBmp->width      = width;
            pPhysBmp->height     = height;
            pPhysBmp->line_bytes = line_bytes;
            pPhysBmp->pix_bits   = pDev->pix_bits;
            pPhysBmp->pdata      = NULL;

            pPhysBmp->bUpdate    = -1;
            SetRect(&pPhysBmp->rcUpdate, 0, 0, 0, 0);
            pPhysBmp->pDDI       = pDev->pDDI;
        }
    }

    return sizeof(PHYSBMP);
}

/*********************************************************************\
* Function  DestroyBitmap
* Purpose   Destroy Bitmap.
* Params
    pDev    Pointer to display device structure.
    pPhysBmp Pointer to physical bitmap structure.
* Return    0
* Remarks	   
**********************************************************************/
static int DestroyBitmap(PDEV pDev, PPHYSBMP pPhysBmp)
{
    return 0;
}

/*********************************************************************\
* Function  SetBitmapBuffer
* Purpose   Set the physcal bitmap bits data pointer.
* Params
    pDev        Pointer to display device structure.
    pPhysBmp    Pointer to physical bitmap structure.
    pBuffer     Pointer to the bits data.
* Return    0
* Remarks	   
**********************************************************************/
static int SetBitmapBuffer(PDEV pDev, PPHYSBMP pPhysBmp, void* pBuffer)
{
    if (pPhysBmp)
        pPhysBmp->pdata = pBuffer;

    return 0;
}

/*********************************************************************\
* Function  RealizePen
* Purpose   logical pen to physical pen.
* Params
    pDev        Pointer to display device structure.
    pLogPen     Pointer to logical pen.
    pPhysPen    Pointer to physical pen.
* Return    sizeof(PHYSPEN)
* Remarks	   
**********************************************************************/
static int RealizePen(PDEV pDev, PDRVLOGPEN pLogPen, PPHYSPEN pPhysPen)
{
    /* This version only realize null pen or solid pen with
     * width = 1.
     */
    if (pLogPen == NULL)
        return -1;

    if (pPhysPen != NULL)
    {
        if (pLogPen->style == PS_NULL)
        {   // Null pen.
            pPhysPen->style = PS_NULL;
            pPhysPen->width = 0;
        }
        else
        if(pLogPen->style != PS_SOLID && pLogPen->width != 1)
        {   // If width not 1, must be solid pen.
            pPhysPen->style = PS_SOLID;
            pPhysPen->width = pLogPen->width;
        }
        else
        {
            pPhysPen->style = pLogPen->style;
            pPhysPen->width = pLogPen->width;
        }

        pPhysPen->fgcolor    = RealizeColor(pDev, NULL, pLogPen->color);
        pPhysPen->bkcolor    = 0;
        pPhysPen->end_style  = PES_DEFAULT;
        pPhysPen->frame_mode = pLogPen->frame_mode;
        pPhysPen->pat_width  = 0;
        pPhysPen->pat_height = 0;
        pPhysPen->ge_handle  = 0;
    }
    return sizeof(PHYSPEN);
}

/*********************************************************************\
* Function  UnrealizePen
* Purpose   Unrealize a physical pen.
* Params
    pDev        Pointer to display device structure.
    pPen    Pointer to physical pen.
* Return    0
* Remarks	   
**********************************************************************/
static int UnrealizePen(PDEV pDev, PPHYSPEN pPen)
{
//    DestroyGEPen(pPen);
    return 0;
}

/*********************************************************************\
* Function  RealizeBrush
* Purpose   Realize logical brush to physical brush.
* Params
    pDev        Pointer to display device structure.
    pLogBrush   Pointer to logical brush.
    pPhysBrush  Pointer to physical brush.
* Return    the size used to the physcal brush.
* Remarks	   
**********************************************************************/
static int RealizeBrush(PDEV pDev, PDRVLOGBRUSH pLogBrush, 
                        PPHYSBRUSH pPhysBrush)
{
    int len;
    DWORD dwSize;

    len = sizeof(PHYSBRUSH);

    if (pPhysBrush != NULL)
    {
        /* realize the brush */
        pPhysBrush->width   = BRUSH_WIDTH;
        pPhysBrush->height  = BRUSH_HEIGHT;
        pPhysBrush->style   = pLogBrush->style;
        pPhysBrush->hatch_style = pLogBrush->hatch_style;
        pPhysBrush->fgcolor = RealizeColor(pDev, NULL, pLogBrush->color);
        pPhysBrush->bkcolor = 0;
        pPhysBrush->ge_handle = 0;
        if(pLogBrush->style == BS_SOLID)
            pPhysBrush->pPattern = NULL;
        else
            pPhysBrush->pPattern = (uint8 *)(pPhysBrush + 1);
    }

    if (pLogBrush->style == BS_BITMAP)
    {
        if (pLogBrush->pPattern == NULL)
            return -1;

        dwSize = (pLogBrush->width * pDev->pix_bits + 31) / 32 * 4 * 
            pLogBrush->height;
        len += dwSize;

        /* only return size of physical brush */
        if (pPhysBrush == NULL)
            return len;

        pPhysBrush->width   = pLogBrush->width;
        pPhysBrush->height  = pLogBrush->height;

        memcpy(pPhysBrush->pPattern, (uint8*)pLogBrush->pPattern, dwSize);
//        pPhysBrush->pPattern = (uint8*)pLogBrush->pPattern;
    }

    else if (pLogBrush->style == BS_PATTERN )
    {
        if (pLogBrush->pPattern == NULL || pLogBrush->width < 8 || 
            pLogBrush->height < 8)
            return -1;

        dwSize = (pLogBrush->width + 31) / 32 * 4 * pLogBrush->height;
        len += dwSize;

        if (pPhysBrush == NULL)
            return len;

        /* only return size of physical brush */
        pPhysBrush->width   = pLogBrush->width;
        pPhysBrush->height  = pLogBrush->height;

        memcpy(pPhysBrush->pPattern, (uint8*)pLogBrush->pPattern, dwSize);
//        pPhysBrush->pPattern = (uint8*)pLogBrush->pPattern;
    }

    else if (pLogBrush->style == BS_HATCHED)
    {
        if (pLogBrush->hatch_style < 0 ||
                pLogBrush->hatch_style >= NUM_HATCHPATTERN)
            return -1;

        /* only return size of physical brush */
        if (pPhysBrush != NULL)
        pPhysBrush->pPattern = 
        (unsigned char *)pattern_data[pLogBrush->hatch_style];
    }
    return len;
}

/*********************************************************************\
* Function  UnrealizeBrush
* Purpose   Unrealize a physical brush.
* Params
    pDev    Pointer to display device structure.
    pBrush  Pointer to physical brush.
* Return    the size used to the physcal brush.
* Remarks	   
**********************************************************************/
static int UnrealizeBrush(PDEV pDev, PPHYSBRUSH pBrush)
{
//    DestroyGEBrush(pBrush);
    return 0;
}

/*********************************************************************\
* Function  RealizeColor
* Purpose   Realize logical color to physical color.
* Params
    pDev    Pointer to display device structure.
    pPalette Pointer to physical palette.
    color    logical color.
* Return    physical color
* Remarks	the palette is not supported now.
**********************************************************************/
static int RealizeColor(PDEV pDev, PPHYSPALETTE pPalette, int32 color)
{
    uint32 r, g, b;

    r = color & 0x000000FFL;
    g = (color & 0x0000FF00L) >> 8;
    b = (color & 0x00FF0000L) >> 16;

    return REALIZECOLOR(r, g, b);
}

/*********************************************************************\
* Function  RealizePalette
* Purpose   Realize logical palette to physical palette.
* Params
    pDev    Pointer to display device structure.
    pLogPalette  Pointer to logical palette.
    pPhysPalette Pointer to physical palette.
* Return    -1
* Remarks	
**********************************************************************/
static int RealizePalette(PDEV pDev, PDRVLOGPALETTE pLogPalette,
             PPHYSPALETTE pPhysPalette)
{
    return -1;
}

/*********************************************************************\
* Function  UnrealizePalette
* Purpose   Unrealize physical palette.
* Params
    pDev    Pointer to display device structure.
    pPalette     Pointer to physical palette.
* Return    0
* Remarks	
**********************************************************************/
static int UnrealizePalette(PDEV pDev, PPHYSPALETTE pPalette)
{
    return 0;
}

/*********************************************************************\
* Function  SelectPalette
* Purpose   Select one physical palette to device.
* Params
    pDev    Pointer to display device structure.
    pPalette     Pointer to physical palette.
* Return    -1
* Remarks	This device is not palette device.
**********************************************************************/
static int SelectPalette(PDEV pDev, PPHYSPALETTE pPalette)
{
    return -1;
}

#include "rgbcom.c"
