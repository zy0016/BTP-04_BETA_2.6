
/* 
 * struct flchip definition
 * 
 * Contains information about the location and state of a given flash device 
 *
 * (C) 2000 Red Hat. GPLd.
 *
 * $Id: flashchip.h,v 1.3 2003/09/15 01:39:59 LiChun_k Exp $
 *
 */

#ifndef __MTD_FLASHCHIP_H__
#define __MTD_FLASHCHIP_H__

/* For spinlocks. sched.h includes spinlock.h from whichever directory it
 * happens to be in - so we don't have to care whether we're on 2.2, which
 * has asm/spinlock.h, or 2.4, which has linux/spinlock.h 
 */

typedef enum { 
	FL_READY,
	FL_STATUS,
	FL_CFI_QUERY,
	FL_JEDEC_QUERY,
	FL_ERASING,
	FL_ERASE_SUSPENDING,
	FL_ERASE_SUSPENDED,
	FL_WRITING,
	FL_WRITING_TO_BUFFER,
	FL_WRITE_SUSPENDING,
	FL_WRITE_SUSPENDED,
	FL_PM_SUSPENDED,
	FL_SYNCING,
	FL_UNLOADING,
	FL_LOCKING,
	FL_UNLOCKING,
	FL_POINT,
	FL_UNKNOWN
} flstate_t;



/* NOTE: confusingly, this can be used to refer to more than one chip at a time, 
   if they're interleaved. */

struct flchip {
	unsigned long start; /* Offset within the map */
	//	unsigned long len;
	/* We omit len for now, because when we group them together
	   we insist that they're all of the same size, and the chip size
	   is held in the next level up. If we get more versatile later,
	   it'll make it a damn sight harder to find which chip we want from
	   a given offset, and we'll want to add the per-chip length field
	   back in.
	*/
	int ref_point_counter;
	flstate_t state;	// 芯片当前的状态
	flstate_t oldstate;	// 芯片原来的状态
	
	int write_suspended:1;
	int erase_suspended:1;
	
	struct mutex c_mutex;
	spinlock_t _spinlock; /* We do it like this because sometimes they'll be shared. */
	wait_queue_head_t wq; /* Wait on here when we're waiting for the chip
			     to be ready */
	int word_write_time;
	int buffer_write_time;
	int erase_time;
	void *priv;
	//unsigned long last_cmd_adr;// 上一次命令操作的地址，针对multi-patition的falsh
};



#endif /* __MTD_FLASHCHIP_H__ */
