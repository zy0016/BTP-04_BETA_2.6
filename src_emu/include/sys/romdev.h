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
 * $Source: /cvs/hopencvs/src/include/sys/romdev.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef _SYS_ROMDEV_H
#define _SYS_ROMDEV_H

#include <hopen/ioctl.h>

#define	ROMDEV	    'R'

#define	ROM_GETBASEADDR	    _IO(ROMDEV, 1)
#define ROM_GETSIZE	    _IO(ROMDEV, 2)

#define ROM_SETBASEADDR	    _IO(ROMDEV, 3)
#define ROM_SETSIZE	    _IO(ROMDEV, 4)

#endif //_SYS_ROMDEV_H
