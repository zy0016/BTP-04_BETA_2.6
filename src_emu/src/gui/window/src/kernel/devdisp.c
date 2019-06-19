/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Display device management module.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "devdisp.h"

/* Golbal varible for dirplay driver interface */
DISPLAYDRVINFO   g_DisplayDrv;       // Display driver interface
PDEV             g_pDisplayDev;      // Display device

/* Internal variable for display device infomation */
DISPLAYDEVINFO   g_DisplayDevInfo;   // Display device info

BOOL WS_Init2(void);

/*
**  Function : DISPDEV_Init
**  Purpose  :
**      Get display driver interface and initialize display device. This
**      function must be called when window system initialized.
**  Params   :
**      None.
**  Return   :
**      If the function succeeds, return TRUE; If the function fails, 
**      return FALSE;
*/
BOOL DISPDEV_Init(PGRAPHDEV pGraphDev)
{
    int nRet;
    PDISPLAYDRVINFO pDisplayDrv;
    PDEV pDev;
    int index;

    // index of graphdev start from 1
    index = GET_GRAPHDEV_INDEX(pGraphDev->handle) - 1;
    
    // Get display driver interface
    pDisplayDrv = EnableDisplayDev(NULL);

    // Get size of display device struct

    nRet = pDisplayDrv->OpenDevice(NULL, index);
    if (nRet <= 0)
    {
        return FALSE;
    }

    // Allocate memory for display device struct pointer
    pGraphDev->pDev = MemAlloc(nRet);
    if (pGraphDev->pDev == NULL)
        return FALSE;
    pDev = (PDEV)pGraphDev->pDev;

    // Open the display device
    pDisplayDrv->OpenDevice(pDev, index);

    // Get display device information
    pGraphDev->pDevInfo = MemAlloc(sizeof(DISPLAYDEVINFO));
    if (pGraphDev->pDevInfo == NULL)
    {
        MemFree(pDev);
        return FALSE;
    }

    pDisplayDrv->GetDevInfo(pDev, pGraphDev->pDevInfo);

    /*Initial the graphic device driver interface */
    pGraphDev->drv.devices              = pDisplayDrv->devices;
    pGraphDev->drv.func_mask1           = pDisplayDrv->func_mask1;
    pGraphDev->drv.func_mask2           = pDisplayDrv->func_mask2;
    pGraphDev->drv.OpenDevice           = pDisplayDrv->OpenDevice;
    pGraphDev->drv.CloseDevice          = pDisplayDrv->CloseDevice;
    pGraphDev->drv.SuspendDevice        = pDisplayDrv->SuspendDevice;
    pGraphDev->drv.ResumeDevice         = pDisplayDrv->ResumeDevice;
    pGraphDev->drv.GetDevInfo           = pDisplayDrv->GetDevInfo;
    pGraphDev->drv.EnumMode             = pDisplayDrv->EnumMode;
    pGraphDev->drv.ChangeMode           = pDisplayDrv->ChangeMode;
    pGraphDev->drv.CreateMemoryDevice   = pDisplayDrv->CreateMemoryDevice;
    pGraphDev->drv.DestroyMemoryDevice  = pDisplayDrv->DestroyMemoryDevice;
    pGraphDev->drv.CreateBitmap         = pDisplayDrv->CreateBitmap;
    pGraphDev->drv.DestroyBitmap        = pDisplayDrv->DestroyBitmap;
    pGraphDev->drv.SetBitmapBuffer      = pDisplayDrv->SetBitmapBuffer;
    pGraphDev->drv.RealizePen           = pDisplayDrv->RealizePen;
    pGraphDev->drv.UnrealizePen         = pDisplayDrv->UnrealizePen;
    pGraphDev->drv.RealizeBrush         = pDisplayDrv->RealizeBrush;
    pGraphDev->drv.UnrealizeBrush       = pDisplayDrv->UnrealizeBrush;
    pGraphDev->drv.RealizePalette       = pDisplayDrv->RealizePalette;
    pGraphDev->drv.UnrealizePalette     = pDisplayDrv->UnrealizePalette;
    pGraphDev->drv.RealizeColor         = pDisplayDrv->RealizeColor;
    pGraphDev->drv.SelectPalette        = pDisplayDrv->SelectPalette;
    pGraphDev->drv.DrawGraph            = pDisplayDrv->DrawGraph;
    pGraphDev->drv.BitBlt               = pDisplayDrv->BitBlt;
    pGraphDev->drv.PatBlt               = pDisplayDrv->PatBlt;
    pGraphDev->drv.DIBitBlt             = pDisplayDrv->DIBitBlt;
    pGraphDev->drv.TextBlt              = pDisplayDrv->TextBlt;
    pGraphDev->drv.StretchBlt           = pDisplayDrv->StretchBlt;
    pGraphDev->drv.DIStretchBlt         = pDisplayDrv->DIStretchBlt;
    pGraphDev->drv.RealizeCursor        = pDisplayDrv->RealizeCursor;
    pGraphDev->drv.UnrealizeCursor      = pDisplayDrv->UnrealizeCursor;
    pGraphDev->drv.ShowCursor           = pDisplayDrv->ShowCursor;
    pGraphDev->drv.SetCursorPos         = pDisplayDrv->SetCursorPos;
    pGraphDev->drv.CheckCursor          = pDisplayDrv->CheckCursor;
    pGraphDev->drv.UpdateScreen         = pDisplayDrv->UpdateScreen;
    pGraphDev->drv.SaveScreen           = pDisplayDrv->SaveScreen;
    pGraphDev->drv.RestoreScreen        = pDisplayDrv->RestoreScreen;
    pGraphDev->drv.ScrollScreen         = pDisplayDrv->ScrollScreen;

    if (index == 0)
    {
        g_DisplayDrv = *(PDISPLAYDRVINFO)pDisplayDrv;
        g_pDisplayDev = (PDEV)pGraphDev->pDev;
        g_DisplayDevInfo = *((PDISPLAYDEVINFO)pGraphDev->pDevInfo);
    }
    
    return TRUE;
}

/*
**  Function : DISPDEV_Exit
**  Purpose  :
**      Close the display device and free the memory space of display 
**      device. This function must be called when the window system 
**      shut down.
*/
void DISPDEV_Exit(PGRAPHDEV pGraphDev)
{
    // Close display device
    pGraphDev->drv.CloseDevice(pGraphDev->pDev);

    // Free memory space for display device 
    MemFree(pGraphDev->pDev);
    pGraphDev->pDev = NULL;
    g_pDisplayDev = NULL;
}

/*
**  Function : DISPDEV_GetCaps
**  Purpose  :
**      Retrieves device-specific information about display device. 
**  Params   :
**      Specifies the item to return, this parameter can be one of the 
**      following values:
**          HORZSIZE  : Width, in millimeters, of the physical screen. 
**          VERTSIZE  : Height, in millimeters, of the physical screen. 
**          HORZRES   : Width, in pixels, of the screen. 
**          VERTRES   : Height, in pixels, of the screen. 
**          BITSPIXEL : Number of adjacent color bits for each pixel.
**          PLANES    : Number of color planes. 
**  Return   :
**      Return the value of the desired item.
*/
int DISPDEV_GetCaps(int nIndex)
{
    int nRet;

    // Ensure display device is initialized
    ASSERT(g_pDisplayDev != NULL);

    nRet = 0;

    switch (nIndex)
    {
    case HORZSIZE :

        nRet = g_DisplayDevInfo.phys_width;
        break;

    case VERTSIZE :

        nRet = g_DisplayDevInfo.phys_height;
        break;

    case HORZRES :

        nRet = g_DisplayDevInfo.width;
        break;

    case VERTRES :

        nRet = g_DisplayDevInfo.height;
        break;

    case BITSPIXEL :

        nRet = g_DisplayDevInfo.bits_pixel;
        break;

    case PLANES :

        nRet = g_DisplayDevInfo.planes;
        break;
    }

    return nRet;
}

/*
**  Function : DISPDEV_GetDeviceCaps
**  Purpose  :
**      Retrieves device-specific information about display device and
**      memory display device. 
**  Params   :
**      Specifies the item to return, this parameter can be one of the 
**      following values:
**          HORZSIZE  : Width, in millimeters, of the display device. 
**          VERTSIZE  : Height, in millimeters, of the display device. 
**          HORZRES   : Width, in pixels, of the display device. 
**          VERTRES   : Height, in pixels, of the display device. 
**          BITSPIXEL : Number of adjacent color bits for each pixel.
**          PLANES    : Number of color planes. 
**  Return   :
**      Return the value of the desired item.
*/
int DISPDEV_GetDeviceCaps(PDEV pDev, int nIndex)
{
    int nRet;
    DISPLAYDEVINFO DevInfo;

    // Ensure display device is initialized
    ASSERT(g_pDisplayDev != NULL);

    if (!pDev || pDev == g_pDisplayDev)
        return DISPDEV_GetCaps(nIndex);

    g_DisplayDrv.GetDevInfo(pDev, &DevInfo);

    nRet = 0;

    switch (nIndex)
    {
    case HORZSIZE :

        nRet = DevInfo.phys_width;
        break;

    case VERTSIZE :

        nRet = DevInfo.phys_height;
        break;

    case HORZRES :

        nRet = DevInfo.width;
        break;

    case VERTRES :

        nRet = DevInfo.height;
        break;

    case BITSPIXEL :

        nRet = DevInfo.bits_pixel;
        break;

    case PLANES :

        nRet = DevInfo.planes;
        break;
    }

    return nRet;
}

/*
**  Function : DISPDEV_ChangeDisplaySettings
**  Purpose  :
**      Changes the display settings to the specified graphics mode. 
*/
LONG DISPDEV_ChangeDisplaySettings(PDEVMODE pDevMode, DWORD dwFlags)
{
#if (_DISPLAY_VER >= 0x0200)
    LONG lRet;
#endif // (_DISPLAY_VER >= 0x0200)

    DISPLAYDEVINFO DevInfo;

    // Ensure display device is initialized
    ASSERT(g_pDisplayDev != NULL);

    ASSERT(pDevMode != NULL);

    if ((dwFlags == 0) || (dwFlags == DM_BITSPERPEL))
    {
        g_DisplayDrv.GetDevInfo(g_pDisplayDev, &DevInfo);
        
        switch (pDevMode->dmBitsPerPel)
        {
        case 1 :
            
            if (DevInfo.bits_pixel != 1)
            {
                g_DisplayDrv.CloseDevice(g_pDisplayDev);
                g_DisplayDrv.OpenDevice(g_pDisplayDev, 0);
                
                // Display mode changed, must get display mode infomation again
                g_DisplayDrv.GetDevInfo(g_pDisplayDev, &g_DisplayDevInfo);                
                
                return DISP_CHANGE_SUCCESSFUL;
            }
            break;
            
        case 2 :
            
            if (DevInfo.bits_pixel != 2)
            {
                g_DisplayDrv.CloseDevice(g_pDisplayDev);
                g_DisplayDrv.OpenDevice(g_pDisplayDev, 1);
                
                // Display mode changed, must get display mode infomation again
                g_DisplayDrv.GetDevInfo(g_pDisplayDev, &g_DisplayDevInfo);
                
                return DISP_CHANGE_SUCCESSFUL;
            }
            
            break;
        }
        
        return DISP_CHANGE_BADMODE;
    }

#if (_DISPLAY_VER >= 0x0200)

    lRet = g_DisplayDrv.ChangeMode(g_pDisplayDev, 
        (PDISPMODE)&(pDevMode->dmBitsPerPel), dwFlags);
    if (lRet >= 0)
    {
        int nOldScrW, nOldScrH;

        nOldScrW = g_DisplayDevInfo.width;
        nOldScrH = g_DisplayDevInfo.height;

        // Display mode changed, must get display mode infomation again
        g_DisplayDrv.GetDevInfo(g_pDisplayDev, &g_DisplayDevInfo);

        if ((nOldScrW != g_DisplayDevInfo.width) || 
            (nOldScrH != g_DisplayDevInfo.height))
            WS_Init2();
    }

    return lRet;

#else // (_DISPLAY_VER >= 0x0200)

    return DISP_CHANGE_FAILED;

#endif // (_DISPLAY_VER >= 0x0200)
}

/*
**  Function : DISPDEV_EnumDisplaySettings
**  Purpose  :
**      Obtains information about one of a display device¡¯s graphics 
**      modes. You can obtain information for all of a display device¡¯s 
**      graphics modes by making a series of calls to this function. 
*/
BOOL DISPDEV_EnumDisplaySettings(DWORD dwModeNum, PDEVMODE pDevMode)
{
#if (_DISPLAY_VER >= 0x0200)

    DISPMODE dispmode;

    ASSERT(g_pDisplayDev != NULL);
    ASSERT(pDevMode != NULL);

    if (g_DisplayDrv.EnumMode(g_pDisplayDev, dwModeNum, &dispmode) == -1)
        return FALSE;

    pDevMode->dmBitsPerPel       = dispmode.dmBitsPerPel;
    pDevMode->dmPelsWidth        = dispmode.dmPelsWidth;
    pDevMode->dmPelsHeight       = dispmode.dmPelsHeight;
    pDevMode->dmDisplayFlags     = dispmode.dmDisplayFlags;
    pDevMode->dmDisplayFrequency = dispmode.dmDisplayFrequency;

    return TRUE;

#else // (_DISPLAY_VER > 0x0200)

    return FALSE;

#endif // (_DISPLAY_VER > 0x0200)
}
