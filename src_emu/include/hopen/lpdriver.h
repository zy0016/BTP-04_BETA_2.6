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
 * $Source: /cvs/hopencvs/src/include/hopen/lpdriver.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 *
\**************************************************************************/

#ifndef _HOPEN_LPDRIVER_H
#define _HOPEN_LPDRIVER_H

#include <hopen/lp.h>

/* timeout for printk'ing a timeout, in jiffies (100ths of a second).
   This is also used for re-checking error conditions if LP_ABORT is
   not set.  This is the default behavior. */

#define LP_TIMEOUT_INTERRUPT	60		/* 60 seconds */
#define LP_TIMEOUT_POLLED	10		/* 10 seconds */

/* Magic numbers for defining port-device mappings */
#define LP_PARPORT_UNSPEC	-4
#define LP_PARPORT_AUTO		-3
#define LP_PARPORT_OFF		-2
#define LP_PARPORT_NONE		-1

#define LP_F(minor)		lp_table[(minor)].flags		/* flags for busy, etc. */
#define LP_CHAR(minor)	lp_table[(minor)].chars		/* busy timeout */
#define LP_TIME(minor)	lp_table[(minor)].time		/* wait time */
#define LP_WAIT(minor)	lp_table[(minor)].wait		/* strobe wait */
#define LP_IRQ(minor)	lp_table[(minor)].port->irq /* interrupt # */
							/* PARPORT_IRQ_NONE means polled */
#define LP_STAT(minor)	lp_table[(minor)].stats		/* statistics area */
#define LP_BUFFER_SIZE	256

#define LP_BASE(x)	lp_table[(x)].dev->port->base

struct lp_stats {
	unsigned long chars;
	unsigned long sleeps;
	unsigned int maxrun;
	unsigned int maxwait;
	unsigned int meanwait;
	unsigned int mdev;
};

struct lp_struct {
	struct parport *port;
	unsigned long	flags;
	unsigned int	chars;
	unsigned int	time;
	unsigned int	wait;
	char *		lp_buffer;
	unsigned int	lastcall;
	unsigned int	runchars;
	struct lp_stats stats;
	struct wait_queue *wait_q;
	unsigned int	last_error;
	volatile unsigned int irq_detected:1;
	volatile unsigned int irq_missed:1;
};

/*
 * The following constants describe the various signals of the printer port
 * hardware.  Note that the hardware inverts some signals and that some
 * signals are active low.  An example is LP_STROBE, which must be programmed
 * with 1 for being active and 0 for being inactive, because the strobe signal
 * gets inverted, but it is also active low.
 */

/*
 * bit defines for 8255 status port
 * base + 1
 * accessed with LP_S(minor), which gets the byte...
 */
#define LP_PBUSY	0x80  /* inverted input, active high */
#define LP_PACK		0x40  /* unchanged input, active low */
#define LP_POUTPA	0x20  /* unchanged input, active high */
#define LP_PSELECD	0x10  /* unchanged input, active high */
#define LP_PERRORP	0x08  /* unchanged input, active low */

/*
 * defines for 8255 control port
 * base + 2
 * accessed with LP_C(minor)
 */
#define LP_PINTEN	0x10  /* high to read data in or-ed with data out */
#define LP_PSELECP	0x08  /* inverted output, active low */
#define LP_PINITP	0x04  /* unchanged output, active low */
#define LP_PAUTOLF	0x02  /* inverted output, active low */
#define LP_PSTROBE	0x01  /* short high output on raising edge */

/*
 * the value written to ports to test existence. PC-style ports will
 * return the value written. AT-style ports will return 0. so why not
 * make them the same ?
 */
#define LP_DUMMY	0x00

/*
 * This is the port delay time, in microseconds.
 * It is used only in the lp_init() and lp_reset() routine.
 */
#define LP_DELAY 	50
#define LP_POLLED(minor)	(lp_table[(minor)].port->irq == PARPORT_IRQ_NONE)

#endif		/* HOPEN_LPDRIVER_H */

