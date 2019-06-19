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
 * $Source: /cvs/hopencvs/src/include/hopen/blkdev.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:58:59 $
 * 
\**************************************************************************/

#ifndef _HOPEN_BLKDEV_H
#define _HOPEN_BLKDEV_H

#include <hopen/tqueue.h>

struct request {
	struct request * next;
	volatile int    rq_status;
	kdev_t          rq_dev;
	int             cmd;
	int             errors;
	unsigned long   sector;
	unsigned long   nr_sectors;
	unsigned long   current_nr_sectors;
	char *          buffer;
	struct buffer_head * bh;
	struct buffer_head * bhtail;
};

#define RQ_INACTIVE		(-1)
#define RQ_ACTIVE		1
#define RQ_SCSI_BUSY		0xffff
#define RQ_SCSI_DONE		0xfffe
#define RQ_SCSI_DISCONNECTING	0xffe0

typedef void (request_fn_proc) (void);
typedef struct request ** (queue_proc) (kdev_t dev);

struct blk_dev_struct {
	request_fn_proc	*request_fn;
	queue_proc	*queue;         /* queue_proc has to be atomic */
	void		*data;
	struct request	*current_request;
	struct request   plug;
	struct tq_struct plug_tq;
};

extern struct blk_dev_struct * blk_dev[MAX_BLKDEV];

#define CURRENT			(blk_dev[MAJOR_NR]->current_request)
#define CURRENT_DEV		DEVICE_NR(CURRENT->rq_dev)

/*
 * end_request() and friends. Must be called with the request queue spinlock
 * acquired. All functions called within end_request() _must_be_ atomic.
 *
 * Several drivers define their own end_request and call end_that_request_first()
 * and end_that_request_last() for parts of the original function. This prevents
 * code duplication in drivers.
 */

int end_that_request_first(struct request *req, int uptodate);
void end_that_request_last(struct request *req);

#define INIT_REQUEST	if (!CURRENT) { CLEAR_INTR; return; }

#define end_request(uptodate) \
    do { \
        struct request * req = CURRENT; \
        if ( end_that_request_first(req, uptodate) == 0 ) { \
            CURRENT = req->next; \
            end_that_request_last(req); \
        } \
    } while ( 0 )

#endif //_HOPEN_BLKDEV_H

