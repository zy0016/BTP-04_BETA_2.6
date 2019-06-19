/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header File for Printer device management.
 *            
\**************************************************************************/

#ifndef __DEVPRN_H
#define __DEVPRN_H

/* Head file for Printer driver */
#define GUI_CALL
#include "graphdev.h"
//#include "printer.h"

/* Golbal varible for printer driver interface */
extern PRINTERDRVINFO   g_PrinterDrv;       // Printer driver interface

/* Function prototypes for Printer device   */

/*
BOOL PRNDEV_Init(void);
void PRNDEV_Exit(void);
*/
BOOL PRNDEV_Init(PGRAPHDEV pGraphDev);
void PRNDEV_Exit(PGRAPHDEV pGraphDev);
int  PRNDEV_GetCaps(void* pDev, int nIndex);

void* PRNDEV_CreatePrinterDev(int width, int height);
void PRNDEV_DestroyPrinterDev(void* pDev);
int  PRNDEV_StartDoc(void* pDev, const DOCINFO* pDocInfo);
int  PRNDEV_EndDoc(void* pDev);
int  PRNDEV_StartPage(void* pDev);
int  PRNDEV_EndPage(void* pDev);
int  PRNDEV_StartBand(void* pDev, PRECT pRect);
int  PRNDEV_EndBand(void* pDev);

#endif  /* __DEVPRN_H */
