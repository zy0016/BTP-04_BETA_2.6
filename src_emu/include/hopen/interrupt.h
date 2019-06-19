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
 * $Source: /cvs/hopencvs/src/include/hopen/interrupt.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:58:59 $
 * 
\**************************************************************************/

#ifndef _HOPEN_INTERRUPT_H
#define _HOPEN_INTERRUPT_H

/* Who gets which entry in bh_base.  Things which will occur most often
   should come first - in which case NET should be up the top with SERIAL/TQUEUE! */
   
enum {
	TIMER_BH = 0, IMMEDIATE_BH, TQUEUE_BH, NET_BH,
	DISK_BH, SERIAL_BH, CONSOLE_BH, KEYBOARD_BH,
	END_BH	/* This is the last one */
};

struct pt_regs;

struct irqaction {
	void (*handler)(int, void *, struct pt_regs *);
	unsigned long	flags;
	unsigned long	mask;
	const char *	name;
	void *		dev_id;
	struct irqaction * next;
};

extern int	bh_mask_count[END_BH];
extern void	(*bh_base[END_BH])(void);
extern unsigned	long bh_active, bh_mask;

//extern volatile long	bh_running;

extern void do_bottom_half(void);

extern int  setup_irq(unsigned int irq, struct irqaction * action);
extern void free_irq(unsigned int irq, void *dev_id);

extern void enable_irq(unsigned int irq);
extern void disable_irq(unsigned int irq);

#define	SA_SHIRQ	0x0010

#include <hal/interrupt.h>

#endif
