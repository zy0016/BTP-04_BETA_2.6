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
 * $Source: /cvs/hopencvs/src/include/hal-armv/portio.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2003/06/27 08:32:36 $
 * 
\**************************************************************************/


#ifndef _HAL_ARM_PORTIO_H
#define _HAL_ARM_PORTIO_H

#define inb(port)		*((volatile unsigned char  *)(port))
#define inw(port)		*((volatile unsigned short *)(port))
#define ind(port)		*((volatile unsigned long  *)(port))
#define inl(port)		*((volatile unsigned long  *)(port))

#define outb(value,port)	*((volatile unsigned char  *)(port)) = (value)
#define outw(value,port)	*((volatile unsigned short *)(port)) = (value)
#define outd(value,port)	*((volatile unsigned long  *)(port)) = (value)
#define outl(value,port)	*((volatile unsigned long  *)(port)) = (value)

#endif	// _HAL_ARM_PORTIO_H
