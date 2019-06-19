/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header File for display device management.
 *            
\**************************************************************************/

#ifndef __DEVDISP_H
#define __DEVDISP_H

/* Head file for display driver */
#define GUI_CALL
#include "display.h"
#include "graphdev.h"

/* Golbal varible for dirplay driver interface */
extern DISPLAYDRVINFO   g_DisplayDrv;       // Display driver interface
extern DISPLAYDEVINFO   g_DisplayDevInfo;   // Display device info
extern PDEV             g_pDisplayDev;      // Display device

/* Function prototypes for display device   */
/*
BOOL DISPDEV_Init(void);
void DISPDEV_Exit(void);
*/
BOOL DISPDEV_Init(PGRAPHDEV pGraphDev);
void DISPDEV_Exit(PGRAPHDEV pGraphDev);
int  DISPDEV_GetCaps(int nIndex);
int  DISPDEV_GetDeviceCaps(PDEV pDev, int nIndex);
LONG DISPDEV_ChangeDisplaySettings(PDEVMODE pDevMode, DWORD dwFlags);
BOOL DISPDEV_EnumDisplaySettings(DWORD dwModeNum, PDEVMODE pDevMode);

#endif  /* __DEVDISP_H */
