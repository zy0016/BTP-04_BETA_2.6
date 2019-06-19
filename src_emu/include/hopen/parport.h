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
 * $Source: /cvs/hopencvs/src/include/hopen/parport.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 *
\**************************************************************************/

#ifndef _HOPEN_PARPORT_H_
#define _HOPEN_PARPORT_H_

/* Start off with user-visible constants */

/* Maximum of 8 ports per machine */
#define PARPORT_MAX  8 

/* Magic numbers */
#define PARPORT_IRQ_NONE	  -1
#define PARPORT_DMA_NONE	  -1
#define PARPORT_IRQ_AUTO	  -2
#define PARPORT_DMA_AUTO	  -2
#define PARPORT_DISABLE		  -2
#define PARPORT_IRQ_PROBEONLY -3

#define PARPORT_CONTROL_STROBE    0x1
#define PARPORT_CONTROL_AUTOFD    0x2
#define PARPORT_CONTROL_INIT      0x4
#define PARPORT_CONTROL_SELECT    0x8
#define PARPORT_CONTROL_INTEN     0x10
#define PARPORT_CONTROL_DIRECTION 0x20

#define PARPORT_STATUS_ERROR      0x8
#define PARPORT_STATUS_SELECT     0x10
#define PARPORT_STATUS_PAPEROUT   0x20
#define PARPORT_STATUS_ACK        0x40
#define PARPORT_STATUS_BUSY       0x80

/* The "modes" entry in parport is a bit field representing the following
 * modes.
 * Note that PARPORT_MODE_PCECPEPP is for the SMC EPP+ECP mode which is NOT
 * 100% compatible with EPP.
 */
#define PARPORT_MODE_PCSPP	        0x0001
#define PARPORT_MODE_PCPS2			0x0002
#define PARPORT_MODE_PCEPP			0x0004
#define PARPORT_MODE_PCECP			0x0008
#define PARPORT_MODE_PCECPEPP		0x0010
#define PARPORT_MODE_PCECR			0x0020  /* ECR Register Exists */
#define PARPORT_MODE_PCECPPS2		0x0040

/* Define this later. */
struct parport;

struct pc_parport_state {
	unsigned int ctr;
	unsigned int ecr;
};

struct parport_state {
	union {
		struct pc_parport_state pc;
		/* ARC has no state. */
		/* AX uses same state information as PC */
		void *misc; 
	} u;
};

struct parport_operations {
	void (*write_data)(struct parport *, unsigned char);
	unsigned char (*read_data)(struct parport *);
	void (*write_control)(struct parport *, unsigned char);
	unsigned char (*read_control)(struct parport *);
	unsigned char (*frob_control)(struct parport *, unsigned char mask, unsigned char val);
	void (*write_econtrol)(struct parport *, unsigned char);
	unsigned char (*read_econtrol)(struct parport *);
	unsigned char (*frob_econtrol)(struct parport *, unsigned char mask, unsigned char val);
	void (*write_status)(struct parport *, unsigned char);
	unsigned char (*read_status)(struct parport *);
	void (*write_fifo)(struct parport *, unsigned char);
	unsigned char (*read_fifo)(struct parport *);

	void (*change_mode)(struct parport *, int);

	void (*epp_write_data)(struct parport *, unsigned char);
	unsigned char (*epp_read_data)(struct parport *);
	void (*epp_write_addr)(struct parport *, unsigned char);
	unsigned char (*epp_read_addr)(struct parport *);
	int (*epp_check_timeout)(struct parport *);
	int (*epp_write_block)(struct parport *, void *, int);
	int (*epp_read_block)(struct parport *, void *, int);

	int (*ecp_write_block)(struct parport *, void *, int, void (*fn)(struct parport *, void *, int), void *);
	int (*ecp_read_block)(struct parport *, void *, int, void (*fn)(struct parport *, void *, int), void *);
};

/* A parallel port */
struct parport {
	unsigned long base;		/* base address */
	unsigned int  size;		/* IO extent */
	int			  irq;		/* interrupt (or -1 for none) */
	int			  dma;
	unsigned int  modes;
	unsigned int  flags;
	int			  number;	/* port index - the `n' in `parportn' */

	struct parport_operations *ops;
	void *private_data;     /* for lowlevel driver */
};

/* parport_register_port registers a new parallel port at the given address (if
 * one does not already exist) and returns a pointer to it.  This entails
 * claiming the I/O region, IRQ and DMA.
 * NULL is returned if initialisation fails. 
 */
struct parport *parport_register_port(unsigned long base, int irq, int dma,
				      struct parport_operations *ops);
void parport_unregister_port(struct parport * port);
void parport_interrupt(struct parport * port);

/* If PC hardware is the only type supported, we can optimise a bit.  */
#ifdef	CONFIG_PARPORT_PC

#include <hopen/parport_pc.h>

#define parport_write_data(p,x)            parport_pc_write_data(p,x)
#define parport_read_data(p)               parport_pc_read_data(p)
#define parport_write_control(p,x)         parport_pc_write_control(p,x)
#define parport_read_control(p)            parport_pc_read_control(p)
#define parport_frob_control(p,m,v)        parport_pc_frob_control(p,m,v)
#define parport_write_econtrol(p,x)        parport_pc_write_econtrol(p,x)
#define parport_read_econtrol(p)           parport_pc_read_econtrol(p)
#define parport_frob_econtrol(p,m,v)       parport_pc_frob_econtrol(p,m,v)
#define parport_write_status(p,v)          parport_pc_write_status(p,v)
#define parport_read_status(p)             parport_pc_read_status(p)
#define parport_write_fifo(p,v)            parport_pc_write_fifo(p,v)
#define parport_read_fifo(p)               parport_pc_read_fifo(p)
#define parport_change_mode(p,m)           parport_pc_change_mode(p,m)
#define parport_epp_write_data(p,x)        parport_pc_write_epp(p,x)
#define parport_epp_read_data(p)           parport_pc_read_epp(p)
#define parport_epp_write_addr(p,x)        parport_pc_write_epp_addr(p,x)
#define parport_epp_read_addr(p)           parport_pc_read_epp_addr(p)
#define parport_epp_check_timeout(p)       parport_pc_check_epp_timeout(p)

#else

/* Generic operations vector through the dispatch table. */
#define parport_write_data(p,x)            (p)->ops->write_data(p,x)
#define parport_read_data(p)               (p)->ops->read_data(p)
#define parport_write_control(p,x)         (p)->ops->write_control(p,x)
#define parport_read_control(p)            (p)->ops->read_control(p)
#define parport_frob_control(p,m,v)        (p)->ops->frob_control(p,m,v)
#define parport_write_econtrol(p,x)        (p)->ops->write_econtrol(p,x)
#define parport_read_econtrol(p)           (p)->ops->read_econtrol(p)
#define parport_frob_econtrol(p,m,v)       (p)->ops->frob_econtrol(p,m,v)
#define parport_write_status(p,v)          (p)->ops->write_status(p,v)
#define parport_read_status(p)             (p)->ops->read_status(p)
#define parport_write_fifo(p,v)            (p)->ops->write_fifo(p,v)
#define parport_read_fifo(p)               (p)->ops->read_fifo(p)
#define parport_change_mode(p,m)           (p)->ops->change_mode(p,m)
#define parport_epp_write_data(p,x)        (p)->ops->epp_write_data(p,x)
#define parport_epp_read_data(p)           (p)->ops->epp_read_data(p)
#define parport_epp_write_addr(p,x)        (p)->ops->epp_write_addr(p,x)
#define parport_epp_read_addr(p)           (p)->ops->epp_read_addr(p)
#define parport_epp_check_timeout(p)       (p)->ops->epp_check_timeout(p)

#endif

#endif /* _HOPEN_PARPORT_H_ */

