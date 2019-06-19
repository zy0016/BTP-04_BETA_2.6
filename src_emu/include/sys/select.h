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
 * Author  :     LiHejia
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/sys/select.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/12/30 01:14:50 $
 * 
\**************************************************************************/


#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

#ifdef __GNUC__
typedef unsigned long long fd_set;
#elif defined(_WIN32)
typedef __int64 fd_set;
#else
typedef unsigned long fd_set;
#endif

#define	FD_SETSIZE sizeof(fd_set) 

#define FD_SET(fd,fdsetp)	do { *(fdsetp) |= ((fd_set)1 << (fd)); } while (0)
#define FD_CLR(fd,fdsetp)	do { *(fdsetp) &= (fd_set)~((fd_set)1 << (fd)); } while (0)
#define FD_ZERO(fdsetp)		do { *(fdsetp) = 0; } while (0)
#define FD_ISSET(fd,fdsetp)	(*(fdsetp) & ((fd_set)1 << (fd)))

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

int select (int nfds, fd_set * readfds, fd_set * writefds,
			fd_set * exceptfds, const struct timeval * timeout);

#ifdef __cplusplus
}
#endif

#endif // _SYS_SELECT_H
