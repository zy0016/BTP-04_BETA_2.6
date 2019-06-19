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
 * $Source: /cvs/hopencvs/src/include/hopen/parport_pc.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 *
\**************************************************************************/

#ifndef _HOPEN_PARPORT_PC_H
#define _HOPEN_PARPORT_PC_H

#include <hal/portio.h>

/* --- register definitions ------------------------------- */

#define ECONTROL 0x402
#define CONFIGB  0x401
#define CONFIGA  0x400
#define EPPDATA  0x4
#define EPPADDR  0x3
#define CONTROL  0x2
#define STATUS   0x1
#define DATA     0

/* Private data for PC low-level driver. */
struct parport_pc_private {
	/* Contents of CTR. */
	unsigned char ctr;
};

extern int parport_pc_epp_clear_timeout(struct parport *pb);

extern volatile unsigned char parport_pc_ctr;

#ifdef __GNUC__

extern __inline__ void parport_pc_write_epp(struct parport *p, unsigned char d)
{
	outb(d, p->base+EPPDATA);
}

extern __inline__ unsigned char parport_pc_read_epp(struct parport *p)
{
	return inb(p->base+EPPDATA);
}

extern __inline__ void parport_pc_write_epp_addr(struct parport *p, unsigned char d)
{
	outb(d, p->base+EPPADDR);
}

extern __inline__ unsigned char parport_pc_read_epp_addr(struct parport *p)
{
	return inb(p->base+EPPADDR);
}

extern __inline__ int parport_pc_check_epp_timeout(struct parport *p)
{
	if (!(inb(p->base+STATUS) & 1))
		return 0;
	parport_pc_epp_clear_timeout(p);
	return 1;
}

extern __inline__ unsigned char parport_pc_read_configb(struct parport *p)
{
	return inb(p->base+CONFIGB);
}

extern __inline__ void parport_pc_write_data(struct parport *p, unsigned char d)
{
	outb(d, p->base+DATA);
}

extern __inline__ unsigned char parport_pc_read_data(struct parport *p)
{
	return inb(p->base+DATA);
}

extern __inline__ void parport_pc_write_control(struct parport *p, unsigned char d)
{
	struct parport_pc_private *priv = p->private_data;
	priv->ctr = d;/* update soft copy */
	outb(d, p->base+CONTROL);
}

extern __inline__ unsigned char parport_pc_read_control(struct parport *p)
{
	struct parport_pc_private *priv = p->private_data;
	return priv->ctr;
}

extern __inline__ unsigned char parport_pc_frob_control(struct parport *p, unsigned char mask,  unsigned char val)
{
	struct parport_pc_private *priv = p->private_data;
	unsigned char ctr = priv->ctr;
	ctr = (ctr & ~mask) ^ val;
	outb (ctr, p->base+CONTROL);
	return priv->ctr = ctr; /* update soft copy */
}

extern __inline__ void parport_pc_write_status(struct parport *p, unsigned char d)
{
	outb(d, p->base+STATUS);
}

extern __inline__ unsigned char parport_pc_read_status(struct parport *p)
{
	return inb(p->base+STATUS);
}

extern __inline__ void parport_pc_write_econtrol(struct parport *p, unsigned char d)
{
	outb(d, p->base+ECONTROL);
}

extern __inline__ unsigned char parport_pc_read_econtrol(struct parport *p)
{
	return inb(p->base+ECONTROL);
}

extern __inline__ unsigned char parport_pc_frob_econtrol(struct parport *p, unsigned char mask,  unsigned char val)
{
	unsigned char old = inb(p->base+ECONTROL);
	outb(((old & ~mask) ^ val), p->base+ECONTROL);
	return old;
}

#else

extern void parport_pc_write_epp(struct parport *p, unsigned char d);
extern unsigned char parport_pc_read_epp(struct parport *p);
extern void parport_pc_write_epp_addr(struct parport *p, unsigned char d);
extern unsigned char parport_pc_read_epp_addr(struct parport *p);
extern int parport_pc_check_epp_timeout(struct parport *p);
extern unsigned char parport_pc_read_configb(struct parport *p);
extern void parport_pc_write_data(struct parport *p, unsigned char d);
extern unsigned char parport_pc_read_data(struct parport *p);
extern void parport_pc_write_control(struct parport *p, unsigned char d);
extern unsigned char parport_pc_read_control(struct parport *p);
extern unsigned char parport_pc_frob_control(struct parport *p, unsigned char mask,  unsigned char val);
extern void parport_pc_write_status(struct parport *p, unsigned char d);
extern unsigned char parport_pc_read_status(struct parport *p);
extern void parport_pc_write_econtrol(struct parport *p, unsigned char d);
extern unsigned char parport_pc_read_econtrol(struct parport *p);
extern unsigned char parport_pc_frob_econtrol(struct parport *p, unsigned char mask,  unsigned char val);

#endif

extern void parport_pc_change_mode(struct parport *p, int m);

extern void parport_pc_write_fifo(struct parport *p, unsigned char v);

extern unsigned char parport_pc_read_fifo(struct parport *p);

extern int parport_pc_epp_read_block(struct parport *p, void *buf, int length);

extern int parport_pc_epp_write_block(struct parport *p, void *buf, int length);

extern int parport_pc_ecp_read_block(struct parport *p, void *buf, int length, void (*fn)(struct parport *, void *, int), void *handle);

extern int parport_pc_ecp_write_block(struct parport *p, void *buf, int length, void (*fn)(struct parport *, void *, int), void *handle);

#endif	/*  _HOPEN_PARPORT_PC_H */

