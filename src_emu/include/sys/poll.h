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
 * $Source: /cvs/hopencvs/src/include/sys/poll.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef	_SYS_POLL_H
#define	_SYS_POLL_H

/* Data structure describing a polling request.  */
struct pollfd {
	unsigned char fd;	/* File descriptor to poll.  */
	unsigned char unused;
	unsigned char events;	/* Types of events poller cares about.  */
	unsigned char revents;	/* Types of events that actually occurred.  */
};

/* Event types that can be polled for.  These bits may be set in `events'
   to indicate the interesting event types; they will appear in `revents'
   to indicate the status of the file descriptor.  */
#define POLLIN		0x0001		/* There is data to read.  */
#define POLLPRI		0x0002		/* There is urgent data to read.  */
#define POLLOUT		0x0004		/* Writing now will not block.  */

/* Event types always implicitly polled for.  These bits need not be set in
   `events', but they will appear in `revents' to indicate the status of
   the file descriptor.  */
#define POLLERR		0x0008		/* Error condition.  */
#define POLLHUP		0x0010		/* Hung up.  */
#define POLLNVAL	0x0020		/* Invalid polling request.  */

/* Canonical number of polling requests to read in at a time in poll.  */
#define NPOLLFILE	30

#ifdef __cplusplus
extern "C" {
#endif

/* Poll the file descriptors described by the NFDS structures starting at
   FDS.  If TIMEOUT is nonzero and not -1, allow TIMEOUT milliseconds for
   an event to occur; if TIMEOUT is -1, block until an event occurs.
   Returns the number of file descriptors with events, zero if timed out,
   or -1 for errors.  */
int poll (struct pollfd * fds, int nfds, int timeout);

#ifdef __cplusplus
}
#endif

#endif	/* sys/poll.h */
