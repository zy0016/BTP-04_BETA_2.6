/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Font device management module.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "devfont.h"

/* Golbal varible for font driver interface */
FONTDRVINFO      g_FontDrv;

/*
**  Function : FONTDEV_Init
**  Purpose  :
**      Get font driver interface and initialize font device. This
**      function must be called when window system initialized.
**  Params   :
**      None.
**  Return   :
**      If the function succeeds, return TRUE; If the function fails, 
**      return FALSE;
*/
BOOL FONTDEV_Init(void)
{
    if (EnableFont(&g_FontDrv) < 0)
        return FALSE;

    return TRUE;
}

/*
**  Function : FONTDEV_Exit
**  Purpose  :
**      Close the display device and free the memory space of display 
**      device. This function must be called when the window system 
**      shut down.
*/
void FONTDEV_Exit(void)
{
}
