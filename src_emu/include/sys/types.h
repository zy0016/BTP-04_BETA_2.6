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
 * $Source: /cvs/hopencvs/src/include/sys/types.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/07/15 03:04:51 $
 * 
\**************************************************************************/

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#ifndef NULL
#define NULL		((void *) 0)
#endif

#ifndef __SIZE_T
#define __SIZE_T
#if defined(__i386) || !defined (__GNUC__)
typedef unsigned int	size_t;
#else
typedef unsigned long	size_t;
#endif
#endif

typedef unsigned int	dev_t;
typedef unsigned long	kdev_t;
typedef unsigned long	ino_t;
typedef unsigned short	mode_t;
typedef unsigned short	nlink_t;
typedef unsigned short	pid_t;
typedef unsigned short	uid_t;
typedef unsigned short	gid_t;
typedef int		ssize_t;
typedef long		off_t;
typedef long		loff_t;
typedef	unsigned short	key_t;

typedef unsigned short	umode_t;
typedef unsigned long	dma_addr_t;

#ifndef __TIME_T
#define __TIME_T
typedef unsigned long	time_t;
#endif

#endif /* _SYS_TYPES_H */
