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
 * $Source: /cvs/hopencvs/src/include/hal-armv/hal.h,v $
 * $Name:  $
 *
 * $Revision: 1.8 $     $Date: 2004/02/20 01:44:50 $
 * 
\**************************************************************************/

#ifndef _ARM_HAL_H_
#define	_ARM_HAL_H_

#ifdef __GNUC__
#define	__inline__	inline
#else
#define	__inline__	__inline
#endif

#define __init
#define	__devinitdata
#define __devinit
#define	__devexit



/******************************************************************************
 *
 *
 ******************************************************************************
 */

#undef	STACK_GROWNUP
#define	SAVED_STACK_FRAME_SIZE	72	/* Save stack space for make user frame */
#define KERNEL_STACKSIZE	4096	/* Kernel stack size for every task */

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT	12
#define PAGE_SIZE	(1UL << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

typedef struct hal_context {
	unsigned long sp_save;	/* Switch stack save */
	unsigned long pc_save;	/* Switch PC save */
	unsigned long domain;	/* Domain mask, used for MMU switch */
	unsigned long procid;   /* Process ID, used for MMU switch */
} hal_context;

struct taskinit;

struct pt_regs {
	unsigned long 	regs[13];	/* user R0 - R12 */
	unsigned long	sp;		/* user stack pointer */
	unsigned long	lr;		/* user link register */
	unsigned long   pc;		/* program counter */
	unsigned long	psr;		/* program status register */
	unsigned long	old_r0;		/* r0 value when system call */
};

#define	UDELAY_FACTOR	1024

/******************************************************************************
 *
 * 与设备相关的宏定义
 *
 ******************************************************************************
 */
#ifdef __GNUC__

#define mb()			__asm volatile ("nop" : : : "memory")

#define barrier(p)		__asm volatile ("nop" : : : "memory")

/* interrupt control.. */

static inline int _HAL_DisableInterrupt(void)
{
	register unsigned retval, temp;

	__asm volatile ("mrs	%0, cpsr\n"
		"	orr	%1, %0, #128\n"
		"	msr	cpsr_c, %1\n"
		: "=r" (retval), "=r" (temp) );
	return retval;
}

static inline void _HAL_EnableInterrupt(void)
{
	register unsigned temp;

	__asm volatile ("mrs	%0, cpsr\n"
		"	bic	%0, %0, #128\n"
		"	msr	cpsr_c, %0\n"
		: "=r" (temp) );
}

static inline void _HAL_RestoreIntState(int flags)
{
	__asm volatile ("msr	cpsr_c, %0\n" : : "r" (flags) );
}

#else

extern void mb(void);
extern void barrier(void);
extern int _HAL_DisableInterrupt(void);
extern void _HAL_EnableInterrupt(void);
extern void _HAL_RestoreIntState(int flags);

#endif	// __GNUC__

#define _HAL_EnterCritical(x)	x = _HAL_DisableInterrupt()
#define _HAL_LeaveCritical(x)	_HAL_RestoreIntState(x)

/******************************************************************************
 *
 * Lock macros
 *
 ******************************************************************************
 */

/* Data type and macro for linux drivers */
typedef int spinlock_t;
typedef int rwlock_t;

#define RW_LOCK_LOCKED		0x7f
#define RW_LOCK_UNLOCKED	0

#define SPIN_LOCK_LOCKED	0x7f
#define SPIN_LOCK_UNLOCKED	0

#define spin_lock_init(pl)	*(pl) = 0

#define spin_lock(pl)	
#define spin_unlock(pl)

#define spin_lock_irqsave(pl,sav)	_HAL_EnterCritical(sav)
#define spin_unlock_irqrestore(pl,sav)	_HAL_LeaveCritical(sav)

#define read_lock(pl)
#define read_unlock(pl)

#define write_lock(pl)
#define write_unlock(pl)

extern void local_bh_disable(void);
extern void local_bh_enable(void);

#define spin_lock_bh(l)		do { local_bh_disable(); spin_lock(l); } while (0)
#define spin_unlock_bh(l)	do { spin_unlock(l); local_bh_enable(); } while (0)

#define read_lock_bh(l)		do { local_bh_disable(); read_lock(l); } while (0)
#define read_unlock_bh(l)	do { read_unlock(l); local_bh_enable(); } while (0)

#define write_lock_bh(l)	do { local_bh_disable(); write_lock(l); } while (0)
#define write_unlock_bh(l)	do { write_unlock(l); local_bh_enable(); } while (0)

#define synchronize_irq()

/******************************************************************************
 *
 * 由设备抽象层实现的函数
 *
 ******************************************************************************
 */

extern void _HAL_ContextSwitch (hal_context * pContext1, hal_context * pContext2);
extern int  _HAL_MakeTaskContext (hal_context * pcontext, struct taskinit * pdata);
/* Make the task 0 context */
extern int  _HAL_MakeTask0Context (hal_context * pcontext);

/* get pointer of task frame, return value is size of task frame */
extern int  _HAL_GetTaskFrame (hal_context * pcontext, struct pt_regs ** ppframe);

/* 从核心态进入用户态 */
extern void _HAL_ToUser (void * entry, void * param, void * ustktop);

/* 捕捉信号 */
extern void _HAL_DoSignal (int sigval, struct pt_regs * pregs);

/* get user stack pointer of this thread */
extern void * _HAL_GetUserStackPointer (void);

#ifdef __RTC_TIME__
extern unsigned long _HAL_GetRTC();
extern void	_HAL_SetRTC(unsigned long secs);
#endif

#define SR_MODE		0x001F		/* Mode mask */

#define SR_USR		0x0010
#define SR_FIQ		0x0011
#define SR_IRQ		0x0012
#define SR_SVC		0x0013
#define SR_ABT		0x0017
#define SR_UND		0x001B
#define SR_SYS		0x001F	


#define PSW_SVC		0x00000093	/* SVC mode with disable interrupt */
#define PSW_USR		0x00000010

#define _HAL_IsUserMode(pregs)		((pregs->psr&SR_MODE) == SR_USR)
#define _HAL_FlashCache(addr,len)

#define	SIGNAL_IDLE
/* Enter idle or leave idle */
extern void _HAL_Idle (int flag);

#define bus_to_virt(paddr)	(void *)((unsigned long)(paddr) + (PAGE_OFFSET - PHYS_OFFSET))
#define virt_to_bus(vaddr)	((unsigned long)(vaddr) + (PHYS_OFFSET - PAGE_OFFSET))

#define get_unaligned(ptr) \
  ({ __typeof__(*(ptr)) __tmp; memcpy(&__tmp, (ptr), sizeof(*(ptr))); __tmp; })

#define put_unaligned(val, ptr)				\
  ({ __typeof__(*(ptr)) __tmp = (val);			\
     memcpy((ptr), &__tmp, sizeof(*(ptr)));		\
     (void)0; })

#endif
