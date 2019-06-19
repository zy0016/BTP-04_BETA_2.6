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
 * $Source: /cvs/hopencvs/src/include/hal-armv/irq.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2004/02/13 07:07:43 $
 *
\**************************************************************************/

#ifndef _ARM_IRQ_H_
#define _ARM_IRQ_H_

/*
 * IRQ line status.
 */
#define IRQ_INPROGRESS		1	/* IRQ handler active - do not enter! */
#define IRQ_DISABLED		2	/* IRQ disabled - do not enter! */
#define IRQ_PENDING		4	/* IRQ pending - replay on enable */
#define IRQ_REPLAY		8	/* IRQ has been replayed but not acked yet */
#define IRQ_AUTODETECT		16	/* IRQ is being autodetected */
#define IRQ_WAITING		32	/* IRQ not yet seen - for autodetection */

/*
 * This is the "IRQ descriptor", which contains various information
 * about the irq, including what kind of hardware handling it has,
 * whether it is disabled etc etc.
 *
 * Pad this out to 32 bytes for cache and indexing reasons.
 */

typedef struct irq_desc {
	unsigned int status;			/* IRQ status - IRQ_INPROGRESS, IRQ_DISABLED */
	struct irqaction *action;		/* IRQ action list */
	unsigned int depth;			/* Disable depth for nested irq disables */
} irq_desc_t;

/*
 * IDT vectors usable for external interrupt sources start
 * at 0x20: Vectors 0x20-0x2f are used for ISA interrupts.
 */

#ifndef CONFIG_NUM_IRQS
#define CONFIG_NUM_IRQS 64
#endif

#define	NUM_IRQS	CONFIG_NUM_IRQS

extern irq_desc_t	irq_desc[NUM_IRQS];

extern void disable_irq(unsigned int);
extern void disable_irq_nosync(unsigned int);
extern void enable_irq(unsigned int);

#endif
