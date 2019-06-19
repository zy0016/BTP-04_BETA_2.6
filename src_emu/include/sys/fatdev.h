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
 * Author  :     Feng Yikun
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/sys/fatdev.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef _SYS_FATDEV_H
#define _SYS_FATDEV_H

#include <hopen/ioctl.h>

#define	FATDEV	    'F'

#define	FAT_GETBASEADDR	    _IO(FATDEV, 1)
#define FAT_GETSIZE	    _IO(FATDEV, 2)

#define FAT_SETBASEADDR	    _IO(FATDEV, 3)
#define FAT_SETSIZE	    _IO(FATDEV, 4)

#define FAT_SETBLKSIZE	_IO(FATDEV, 5)
#define FAT_GETBLKSIZE	_IO(FATDEV, 6)

#endif //_SYS_FATDEV_H
