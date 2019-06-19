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
 * $Source: /cvs/hopencvs/src/include/hopen/buffer.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/11/12 03:18:45 $
 * 
\**************************************************************************/

#ifndef _HOPEN_BUFFER_H
#define _HOPEN_BUFFER_H

#ifdef __KERNEL__

/* bh state bits */
#define BH_Uptodate	0	/* 1 if the buffer contains valid data */
#define BH_Dirty	1	/* 1 if the buffer is dirty */
#define BH_Lock		2	/* 1 if the buffer is locked */
#define BH_Req		3	/* 0 if the buffer has been invalidated */
#define BH_Protected	6	/* 1 if the buffer is protected */

/*
 * Try to keep the most commonly used fields in single cache lines (16
 * bytes) to improve performance.  This ordering should be
 * particularly beneficial on 32-bit processors.
 * 
 * We use the first 16 bytes for the data which is used in searches
 * over the block hash lists (ie. getblk(), find_buffer() and
 * friends).
 * 
 * The second 16 bytes we use for lru buffer scans, as used by
 * sync_buffers() and refill_freelist().  -- sct
 */
struct buffer_head {
    /* First cache line: */
    struct buffer_head *    b_next;	/* Hash queue list */
    unsigned long	    b_blocknr;	/* block number */
    kdev_t		    b_dev;	/* device (B_FREE = free) */
    unsigned long	    b_state;	/* buffer state bitmap (see above) */
    struct buffer_head *    b_next_free;
    unsigned char      	    b_count;	/* users using this block */

    /* Non-performance-critical data follows. */
    unsigned char      	    b_list;	/* List that this buffer appears */
    unsigned long           b_rsector;  /* Real sector number in device */
    char *		    b_data;	/* pointer to data block (1024 bytes) */
    unsigned long	    b_flushtime;/* Time when this (dirty) buffer
				   	 * should be written */
    struct wait_queue  *    b_wait;
    struct buffer_head *    b_prev_free;/* doubly linked list of buffers */
    struct buffer_head *    b_this_page;/* circular list of buffers in one page */
    struct buffer_head **   b_pprev;	/* doubly linked list of hash-queue */
    struct buffer_head *    b_reqnext;	/* request queue */

    /* I/O completion */
    void (*b_end_io)(struct buffer_head *bh, int uptodate);
    void *b_dev_id;
};

typedef void (bh_end_io_t)(struct buffer_head *bh, int uptodate);

#define buffer_uptodate(bh)	(bh->b_state & (1 << BH_Uptodate))
#define buffer_dirty(bh)	(bh->b_state & (1 << BH_Dirty))
#define buffer_locked(bh)	(bh->b_state & (1 << BH_Lock))
#define buffer_req(h)		(bh->b_state & (1 << BH_Req))
#define buffer_protected(bh)	(bh->b_state & (1 << BH_Protected))

#define BUF_CLEAN	0
#define BUF_LOCKED	1	/* Buffers scheduled for write */
#define BUF_DIRTY	2	/* Dirty buffers, not yet scheduled for write */
#define NR_LIST		3

struct buffer_head * getblk(kdev_t dev, int block, int size);
struct buffer_head * get_hash_table(kdev_t dev, unsigned long block, int size);
extern struct buffer_head * bread(kdev_t dev, int block, int size);
extern void mark_buffer_uptodate(struct buffer_head * bh, int on);
extern void mark_buffer_dirty(struct buffer_head * bh, int flag);
extern void mark_buffer_clean(struct buffer_head * bh);
extern void invalidate_buffers(kdev_t dev);
extern int fsync_dev(kdev_t dev);

extern void __brelse(struct buffer_head *);
extern void __bforget(struct buffer_head *buf);

#define brelse(buf)     do { if (buf) __brelse(buf); } while (0)
#define bforget(buf)    do { if (buf) __bforget(buf); } while (0)

#include <hal/bitops.h>

/*
 * Buffer cache locking - note that interrupts may only unlock, not
 * lock buffers.
 */
extern void __wait_on_buffer(struct buffer_head *);

#define wait_on_buffer(bh)  \
    do {    \
	if (test_bit(BH_Lock, &bh->b_state)) \
		__wait_on_buffer(bh); \
    } while (0)

#define lock_buffer(bh)     \
    do { \
	while (test_and_set_bit(BH_Lock, &bh->b_state)) \
		__wait_on_buffer(bh); \
    } while (0)

#define unlock_buffer(bh)   \
    do { \
	clear_bit(BH_Lock, &bh->b_state); \
	wake_up(&bh->b_wait); \
    } while (0)

#endif /* __KERNEL__ */

#endif
