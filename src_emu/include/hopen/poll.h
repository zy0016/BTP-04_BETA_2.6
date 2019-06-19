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
 * $Source: /cvs/hopencvs/src/include/hopen/poll.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef _HOPEN_POLL_H_
#define _HOPEN_POLL_H_

#define POLLRDNORM	0x0040
#define POLLWRNORM	0x0100

typedef struct poll_table_struct poll_table;

extern void __pollwait(void *, struct wait_queue **, poll_table *);
extern void free_wait(poll_table *);

#define	poll_wait(filp, addr, p)	if (p && addr)	__pollwait(filp, addr, p);

struct file;
struct pollfd;
extern int do_poll(int nfds, struct file ** p_filp, struct pollfd * p_fds,
			poll_table *wait, long timeout);

#endif	// _HOPEN_POLL_H_
