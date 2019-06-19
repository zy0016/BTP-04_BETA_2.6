/**************************************************************************\
 *
 *                      Pollex Software System
 *
 * Copyright (c) Pollex Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
\**************************************************************************/

#ifndef _INFO_H
#define _INFO_H

#include <sys/ioctl.h> 


//lcd ioctl cmd
#define  INFO_IOC_MAGIC        		'I'
#define  INFO_IOC_GETRAMADDR   		_IO(INFO_IOC_MAGIC, 0)
#define  INFO_IOC_GETROMADDR   		_IO(INFO_IOC_MAGIC, 1)
#ifdef	FLASHMAP_BASE
#define  INFO_IOC_GETFLHADDR   		_IO(INFO_IOC_MAGIC, 2)
#endif
#define  INFO_IOC_GETRAMLEN   		_IO(INFO_IOC_MAGIC, 3)
#define  INFO_IOC_GETROMLEN   		_IO(INFO_IOC_MAGIC, 4)
#ifdef	FLASHMAP_SIZE
#define  INFO_IOC_GETFLHLEN   		_IO(INFO_IOC_MAGIC, 5)
#endif
#define  INFO_IOC_GETMEMLEN   		_IO(INFO_IOC_MAGIC, 6)
#define  INFO_IOC_GETMEMMFREE   	_IO(INFO_IOC_MAGIC, 7)
#define  INFO_IOC_FLUSHCACHE        _IO(INFO_IOC_MAGIC, 8)
#define INFO_IOC_GETHIGHMEM		_IO(INFO_IOC_MAGIC, 9)
#define INFO_IOC_RELEASEHIGHMEM		_IO(INFO_IOC_MAGIC, 10)
#endif


