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
 * $Source: /cvs/hopencvs/src/include/hopen/ipc.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2004/12/13 06:19:14 $
 * 
\**************************************************************************/

#ifndef _HOPEN_IPC_H_
#define	_HOPEN_IPC_H_

#define	IPC_EVENT	0x01
#define	IPC_SEM		0x02
#define IPC_QUEUE	0x04
#define	MODE_MASK	0x07

/* manual reset event */
#define MANUALRESET	0x0001

#include <hopen/ioctl.h>

/* Destroy object used */
#define IPCIODESTROY	_IO (0x0049L, 1)

/* wait event with timeout */
#define IPCIOEWAIT	_IO (0x0049L, 4)

/* Set event */
#define IPCIOESET	_IO (0x0049L, 5)

/* Check semaphore */
#define IPCIOSEMCHK	_IO (0x0049L, 10)

/* wait semaphore with timeout */
#define IPCIOSWAIT	_IO (0x0049L, 11)

/* Set semaphore */
#define IPCIOSSET	_IO (0x0049L, 12)

/* Set queue size */
#define IPCIOQSIZE	_IO (0x0049L, 20)

/* Set queue address, only used when no system memory */
#define IPCIOQADDR	_IO (0x0049L, 21)

/* send queue nonblock */
#define IPCIOQNBSEND	_IO (0x0049L, 22)

/* receive queue nonblock */
#define IPCIOQNBRECV	_IO (0x0049L, 23)

/* block mode send queue */
#define IPCIOQSEND	_IO (0x0049L, 24)

/* block mode send queue */
#define IPCIOQRECV	_IO (0x0049L, 25)

/* send queue with timeout */
#define IPCIOQTMOSEND	_IOR (0x0049L, 26, 8)

/* receive queue with timeour */
#define IPCIOQTMORECV	_IOR (0x0049L, 27, 8)

#ifdef __KERNEL__

#include <hal/atomic.h>
#include <hopen/mutex.h>

#define	MAX_QSIZE	4000

struct ipc_node {
	unsigned short	key;	/* Create key of this node */
	unsigned short	flags;	/* Flags of this node */
	short		count;	/* Object open count */
	struct wait_queue * waitq;
	union {
		long		value;		/* Value of event and semaphore */
		struct queue_data * pdata;	/* Point to queue data */
	} u;
};

struct queue_data {
	mutex	mutex;		/* Mutex for this queue */
	int	szitem;		/* queue unit size */
	int	qsize;		/* queue size */
	int	front;		/* front pointer */
	int	rear;		/* rear pointer */
};

#define	QU_EMPTYMAIL	0x4000	/* Empty mail */
#define	DELETED		0x8000	/* This IPC node is deleted */
#define	EV_AUTORESET	0x0010	/* Auto reset event */

struct poll_table_struct;

int ipc_initbase (int max_ipcnodes);

int ipc_createnode (struct ipc_node ** ppnode, int key, int excl);
int ipc_destroynode (struct ipc_node * pnode);
int ipc_freenode (struct ipc_node * pnode);

int ipc_waitevent ( struct ipc_node * pEvent, int timeout );
int ipc_setevent ( struct ipc_node * pEvent, int event );
int ipc_pollevent( struct ipc_node * pEvent, int mask, struct poll_table_struct * wait);
int ipc_waitsem ( struct ipc_node * pSem, int count, long timeout );
int ipc_releasesem ( struct ipc_node * pSem, int count );
int ipc_pollsem( struct ipc_node * pSem, int mask, struct poll_table_struct * wait);
int ipc_recvqueue ( struct ipc_node * pQueue, char * buf, long timeout );
int ipc_sendqueue( struct ipc_node * pQueue, const char * buf, long timeout );
int ipc_pollqueue( struct ipc_node * pQueue, int mask, struct poll_table_struct * wait);

int ipc_initqueuedata (struct queue_data * qdata, int szitem, int qsize);

/* Only For kernel program use kernel ipc object. */
int ipc_initevent (struct ipc_node * pevent, int flags);
int ipc_initsem (struct ipc_node * psem, int value);
int ipc_initqueue (struct ipc_node * pqueue, struct queue_data * qdata);

extern struct ipc_node * ipc_node_table[];

#endif	// __KERNEL__

#endif
