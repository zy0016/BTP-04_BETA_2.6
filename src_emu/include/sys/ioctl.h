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
 * $Source: /cvs/hopencvs/src/include/sys/ioctl.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef _SYS_IOCTL_H_
#define	_SYS_IOCTL_H_

#define _IOC_NRBITS		8
#define _IOC_TYPEBITS	8
#define _IOC_SIZEBITS	13
#define _IOC_DIRBITS	3

#define _IOC_NRMASK	((1L << _IOC_NRBITS)-1)
#define _IOC_TYPEMASK	((1L << _IOC_TYPEBITS)-1)
#define _IOC_SIZEMASK	((1L << _IOC_SIZEBITS)-1)
#define _IOC_DIRMASK	((1L << _IOC_DIRBITS)-1)

#define _IOC_NRSHIFT	0
#define _IOC_TYPESHIFT	(_IOC_NRSHIFT+_IOC_NRBITS)
#define _IOC_SIZESHIFT	(_IOC_TYPESHIFT+_IOC_TYPEBITS)
#define _IOC_DIRSHIFT	(_IOC_SIZESHIFT+_IOC_SIZEBITS)

/*
 * Direction bits _IOC_NONE could be 0, but OSF/1 gives it a bit.
 * And this turns out useful to catch old ioctl numbers in header
 * files for us.
 */

#define _IOC_NONE	1UL
#define _IOC_READ	2UL
#define _IOC_WRITE	4UL

#define _IOC(dir,type,nr,size) \
	(((dir) << _IOC_DIRSHIFT) | ((type) << _IOC_TYPESHIFT) | \
	 ((nr) << _IOC_NRSHIFT)  | ((size) << _IOC_SIZESHIFT))

/* used to create numbers */
#define _IO(type,nr)		_IOC(_IOC_NONE,(type),(nr),0L)
#define _IOR(type,nr,size)	_IOC(_IOC_READ,(type),(nr),(long)sizeof(size))
#define _IOW(type,nr,size)	_IOC(_IOC_WRITE,(type),(nr),(long)sizeof(size))
#define _IOWR(type,nr,size)	_IOC(_IOC_READ|_IOC_WRITE,(type),(nr),(long)sizeof(size))

/* used to decode them.. */
#define _IOC_DIR(nr)		(((nr) >> _IOC_DIRSHIFT) & _IOC_DIRMASK)
#define _IOC_TYPE(nr)		(((nr) >> _IOC_TYPESHIFT) & _IOC_TYPEMASK)
#define _IOC_NR(nr)		(((nr) >> _IOC_NRSHIFT) & _IOC_NRMASK)
#define _IOC_SIZE(nr)		(((nr) >> _IOC_SIZESHIFT) & _IOC_SIZEMASK)

/* various drivers, such as the pcmcia stuff, need these... */
#define IOC_IN			(_IOC_WRITE << _IOC_DIRSHIFT)
#define IOC_OUT			(_IOC_READ << _IOC_DIRSHIFT)
#define IOC_INOUT		((_IOC_WRITE|_IOC_READ) << _IOC_DIRSHIFT)
#define IOCSIZE_MASK		(_IOC_SIZEMASK << _IOC_SIZESHIFT)
#define IOCSIZE_SHIFT		(_IOC_SIZESHIFT)

#ifdef __cplusplus
extern "C" {
#endif

int  ioctl(int fd, unsigned long cmd, ...);

#ifdef __cplusplus
}
#endif

#endif /* _IOCTL_H_ */
