/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header File for font device management.
 *            
\**************************************************************************/

#ifndef __DEVFONT_H
#define __DEVFONT_H

#define GUI_CALL
#include "font.h"

/* Golbal varible for font driver interface */
extern FONTDRVINFO      g_FontDrv;

// Function prototypes for font device
BOOL FONTDEV_Init(void);
void FONTDEV_Exit(void);

#endif /* __DEVFONT_H */
