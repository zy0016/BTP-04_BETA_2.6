/**************************************************************************\
**                                                                        **
**                         HOPEN OS Program                               **
**                                                                        **
**  Copyright (C) 1998 by CASS Corptation & Software Engineering Center   **
**                       All Rights Reserved                              **
**                                                                        **
** The contents of this file are subject to the HOPEN License;            **
**                                                                        **
** Software distributed under this packet is distributed on an "AS IS"    **
** basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.        **
**                                                                        **
** Duplicate this file without HOPEN license is illegal.                  **
**                                                                        **
\**************************************************************************/

/***************************************************************************
 *
 * Sysytm header file
 *
 * $Name:  $
 *
 * $Revision: 1.2 $  $Date: 2003/05/30 06:06:11 $  $State: Exp $
 *
 * $Author: LiXia $
 *
 ***************************************************************************
 */

#ifndef _HAL_ARM_UHAL_H_                /* Only include stuff once */
#define _HAL_ARM_UHAL_H_

#include <hal/platform.h>

/***********************************************************************
 *
 * For interrupt controler.
 *
 ***********************************************************************
 */

extern void uHALir_MaskIrq(unsigned int irq);
extern void uHALir_UnmaskIrq(unsigned int irq);
extern unsigned long uHALir_Read_Irq(void);

/***********************************************************************
 *
 * For UART.
 *
 ***********************************************************************
 */

extern void uHALir_InitSerial(unsigned int port, unsigned int baudRate);

/***********************************************************************
 *
 * For timer.
 *
 ***********************************************************************
 */

/* Enum to describe timer: free, one-shot, on-going interval or locked-out */
enum uHALe_TimerState
{
    T_FREE, T_ONESHOT, T_INTERVAL, T_LOCKED
};

void uHALir_PlatformClearTimerInterrupt(unsigned int timer);
void uHALir_PlatformDisableTimer(unsigned int timer);
void uHALir_PlatformEnableTimer(unsigned int timer, unsigned int period, int mode);

#endif /* _HAL_ARM_UHAL_H_ */
