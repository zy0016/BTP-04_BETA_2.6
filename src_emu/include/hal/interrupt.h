/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
 * Author  :     WangXinshe
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/hal-armv/interrupt.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/05/30 06:06:09 $
 * 
\**************************************************************************/

#ifndef _ARM_INTERRUPT_H
#define _ARM_INTERRUPT_H

/* Get interrupt state word */
extern unsigned long _HAL_GetISR (void);

#endif  // _ARM_INTERRUPT_H
