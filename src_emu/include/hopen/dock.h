#ifndef _HOPEN_DOCK_H
#define _HOPEN_DOCK_H

#include <hopen/ioctl.h>

#define  DOCK_IOC_MAGIC        	  'd'

#define  DOCK_GET_STATE       	  _IOR(DOCK_IOC_MAGIC, 0, sizeof(int))

#endif /* _HOPEN_DOCK_H */
