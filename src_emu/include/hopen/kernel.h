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
 * $Source: /cvs/hopencvs/src/include/hopen/kernel.h,v $
 * $Name:  $
 *
 * $Revision: 1.7 $     $Date: 2004/12/13 06:19:24 $
 * 
\**************************************************************************/

#ifndef _KERNL_KERNEL_H_
#define	_KERNL_KERNEL_H_

#include <sys/types.h>
#include <hopen/k_data.h>
#include <hopen/task.h>
#include <hopen/proc.h>

struct kernel_data;

typedef struct taskinit {
	pid_t	taskid;		/* task id */
	pid_t	procid;		/* process id */
	void  (*entry)(void);	/* entry of new task */
	void *	stacktop;
} taskinit;

struct pt_regs;

extern int _KERN_InitKernel(void * kmem_start, unsigned long kmem_size);
extern int _KERN_StartKernel(void);

/* Test whether has a signal that needs dealing with  */
#define _KERN_CheckSignal()		(current->signal & current->sigMask)

/* Deal with the signal */
extern int _KERN_DoSignal (int mode, struct pt_regs * pregs);

/* Send signal */
extern int _KERN_kill (int pid, int sig);
extern int _KERN_SendSignal (struct task * pTask, int signal);


extern void _KERN_PostInterrupt (struct pt_regs * pregs);

extern int _KERN_InInterrupt;

/* timer interrupt */
void _KERN_TimerTick (void);

/* Send a signal to task ,maybe wake up it */
int  _KERN_SendSignal (struct task * pTask, int signal);

struct task * _KERN_CheckSched(void);
void _KERN_TaskSched (void);
void _KERN_Yieldup (void);
void _KERN_EndTask (int error, int mode);
void _KERN_FreeTCB (struct task * pTask);

#ifndef __NO_MALLOC__

/* construct process page table */
void * _KERN_MakePageTable (pid_t procid);
/* destruct process page table */
void _KERN_FreePageTable (void * pTable);

/*
 * GFP bitmasks..
 */
#define __GFP_WAIT	0x01
#define __GFP_LOW	0x02
#define __GFP_MED	0x04
#define __GFP_HIGH	0x08
#define __GFP_IO	0x10
#define __GFP_SWAP	0x20

#define __GFP_DMA	0x80

#define GFP_BUFFER	(__GFP_LOW | __GFP_WAIT)
#define GFP_ATOMIC	(__GFP_HIGH)
#define GFP_USER	(__GFP_LOW | __GFP_WAIT | __GFP_IO)
#define GFP_KERNEL	(__GFP_MED | __GFP_WAIT | __GFP_IO)
#define GFP_NFS		(__GFP_HIGH | __GFP_WAIT | __GFP_IO)
#define GFP_KSWAPD	(__GFP_IO | __GFP_SWAP)
#define GFP_DMA         (__GFP_MED | __GFP_WAIT | __GFP_IO | __GFP_DMA)

extern void * _KERN_AllocPage (int count, int mode);
extern void _KERN_FreePage ( void * page, int count);
extern void * _KERN_AllocMemory ( size_t size, int mode );
extern void _KERN_FreeMemory ( void * pMem, size_t size );
extern int  _KERN_GetBlockSize ( void * pMem );

extern int  _KERN_RegisterPageBuffer (int (*free_func)(int want));

#endif //#ifndef __NO_MALLOC__

/* setup a signal captrue function in process for debug */
struct sigaction;
extern void _KERN_SigAction (int sig, struct sigaction * act);
extern int  _KERN_SendSignal (struct task * pTask, int signal);

extern struct kernel_data	K_Data;

#ifdef WIN32
extern unsigned long current_time(void);
#define	CURRENT_TIME	current_time()
#else
#define	CURRENT_TIME	( K_Data.basetime + K_Data.systimer/TICKS_PERSECOND)
#endif

#define	isSignal()		(current->signal && (current->signal & current->sigMask))
#define	doSignal()		_KERN_DoSignal ()
#define signal_pending(t)	((t)->signal && ((t)->signal & (t)->sigMask))
#define send_sig(s,t,priv)	_KERN_SendSignal(t, s)

#define	kmalloc(size, mode)	_KERN_AllocMemory(size, mode)
#define	kfree(ptr, size)	_KERN_FreeMemory(ptr,size)
#define kblksize(ptr)		_KERN_GetBlockSize(ptr)
#define kallocpage(count, mode)	_KERN_AllocPage(count, mode)
#define kfreepage(page, count)	_KERN_FreePage(page, count)


#if defined( __WITH_CONSOLE__ ) ||  !defined(__GNUC__)
extern void k_printf( const char *fmt, ... );
#define printk		k_printf
#else
#define k_printf(fmt, args...)
#define printk(fmt, args...)
#endif

#define	KERN_ERR	"KERNEL ERROR::"

#define k_memset memset
#define k_memcpy memcpy
#define k_memcmp memcmp
#define k_strcmp strcmp
#define k_strncmp strncmp
#define k_strchr strchr
#define k_strlen strlen
#define k_strcpy strcpy
#define k_strncpy strncpy
#define k_memmove memmove
#define k_strcat strcat

#define sprintf k_sprintf
extern int  k_sprintf( char * pStr, const char *fmt, ... );

/*****************************************************
 * short delay
 *****************************************************
 */

extern void __delay(unsigned long loops);
extern void __udelay(unsigned usecs);
extern void __const_udelay(unsigned usecs);

#ifdef __GNUC__
#define udelay(n) (__builtin_constant_p(n) ? __const_udelay((n) * UDELAY_FACTOR) : __udelay(n))
#else
#define udelay(n) __udelay(n)
#endif

void mdelay (int ms);

/*****************************************************
 * for debug
 *****************************************************
 */
extern void _KERN_Yieldup (void);
extern int  _KERN_DisableSched(void);
extern void _KERN_EnableSched(int old);
extern void _KERN_RaiseSignal(int signal, int istrap);
extern int  _KERN_sigaction (int signal, const struct sigaction * act);

/* Macros for linux drivers */

#define	HZ		TICKS_PERSECOND
#define jiffies		K_Data.systimer//CURRENT_TIME
#define	realtime	(K_Data.systimer/TICKS_PERSECOND+K_Data.basetime)

#define KERN_DEBUG	"KERN_DEBUG::"
#define KERN_INFO	"KERN_INFO::"
#define KERN_WARNING	"KERN_WARNING::"
#define KERN_NOTICE	"KERN_NOTICE::"

#endif
