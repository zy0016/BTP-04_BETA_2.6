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
 * $Source: /cvs/hopencvs/src/include/hopen/timer.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2004/12/13 06:20:18 $
 * 
\**************************************************************************/

#ifndef _HOPEN_TIMER_H
#define _HOPEN_TIMER_H

/*
 * timer struct
 */

struct timer_list {
	struct timer_list *	next;
	struct timer_list *	prev;
	unsigned		expires;	/* overtime */
	void *			data;		/* A overtime function deals with multi-timer */
	void (*function)(void * data);
};

extern void start_timer(struct timer_list * timer, unsigned expires);
extern int  stop_timer(struct timer_list * timer);
extern void mod_timer(struct timer_list * timer, unsigned expires);

#define	init_timer(tm)		(tm)->prev = (tm)->next = NULL

#define	timer_pending(tm)	((tm)->prev != 0)
#define timer_expires(tm)	((tm)->expires)

#define time_after(a,b)		((signed)((b)-(a)) < 0)
#define time_before(a,b)	time_after(b,a)

#define time_after_eq(a,b)	((signed)((a)-(b)) >= 0)
#define time_before_eq(a,b)	time_after_eq(b,a)

/* Change into clock cycle , there should be a clock cycle at least. 
*  Unless for preventing from there aren't operation, when number value is great 
*  it use multiplication after division, or else  division after multiplication.
*/

#ifndef CPU_16BIT
#define	mini_to_ticks(m)	(((m)<40000000) ? \
				((m)*TICKS_PERSECOND+512)/1024 : \
				(((m)+512)/1024*TICKS_PERSECOND))
#else
/* On 16 bit system, the input timeout unit is clock tick. */
#define	mini_to_ticks(m)	(m)
#endif

#endif
