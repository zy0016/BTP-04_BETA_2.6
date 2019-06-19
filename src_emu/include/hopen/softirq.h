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
 * $Source: /cvs/hopencvs/src/include/hopen/softirq.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef _HOPEN_SOFTIRQ_H
#define _HOPEN_SOFTIRQ_H

extern unsigned long bh_active, bh_mask;
extern void (*bh_base[])(void);

#ifdef WIN32
extern void mark_bh(int nr);
#else
#define	mark_bh(nr)	atomic_set_mask ((1<<(nr)), &bh_active)
#endif

#define get_active_bhs()	(bh_mask & bh_active)
#define clear_active_bhs(x)	atomic_clear_mask((x), &bh_active)

#define init_bh(nr, routine)	\
	do { \
		bh_base[nr] = (routine); \
		bh_mask |= 1 << (nr);	 \
	} while (0)

#define remove_bh(nr)		\
	do { \
		bh_mask &= ~(1 << (nr)); \
		mb();			 \
		bh_base[nr] = (void *)0; \
	} while (0)

extern void start_bh_atomic (void);
extern void end_bh_atomic (void);

#endif	/* _EMU_SOFTIRQ_H */
