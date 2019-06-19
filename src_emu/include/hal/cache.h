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
 * $Source: /cvs/hopencvs/src/include/hal-armv/cache.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2004/02/20 01:44:17 $
 *
\**************************************************************************/

#ifndef _HAL_ARM_CACHE_H_
#define _HAL_ARM_CACHE_H_

extern void cpu_cache_clean_invalidate_all(void);
extern void cpu_cache_clean_invalidate_range(unsigned long start, unsigned long end, int flags);
extern void cpu_flush_ram_page(unsigned long addr);
extern void cpu_dcache_clean_range(unsigned long start, unsigned long end);
extern void cpu_dcache_invalidate_range(unsigned long start, unsigned long end);
extern void cpu_icache_invalidate_range(unsigned long start, unsigned long end);

extern void cpu_tlb_invalidate_all(void);
extern void cpu_tlb_invalidate_range(unsigned long start, unsigned long end);
extern void cpu_tlb_invalidate_page(unsigned long addr, int flags);

/*
 * Generic I + D cache
 */
#define flush_cache_all()				    \
    do {						    \
	cpu_cache_clean_invalidate_all();		    \
    } while (0)

#define flush_cache_range(_mm,_start,_end)		    \
    do {						    \
	cpu_cache_clean_invalidate_range((_start)+(_mm)->procid,    \
		(_end)+(_mm)->procid, 1);		    \
    } while (0)

#define flush_cache_page(_mm,_vma,_vmaddr)		    \
    do {						    \
	cpu_cache_clean_invalidate_range((_vmaddr)+(_mm)->procid,    \
		(_vmaddr)+(_mm)->procid + PAGE_SIZE,	    \
		((_vma)->vm_flags & VM_EXEC));		    \
    } while (0)

#define invalidate_cache_range(_mm,_start,_end)		    \
    do {						    \
	unsigned long mva_start = (_start)+(_mm)->procid;   \
	unsigned long mva_end = (_end)+(_mm)->procid;	    \
	cpu_dcache_invalidate_range(mva_start, mva_end);    \
	cpu_icache_invalidate_range(mva_start, mva_end);    \
    } while (0)

#define flush_page_to_ram(page)				    \
    do {						    \
	cpu_flush_ram_page((unsigned long)page_to_virt(page)); \
    } while (0)

/* This is always called for current->mm */
#define flush_cache_mm(_mm)						\
	do {								\
		cpu_cache_clean_invalidate_all();		\
	} while (0)

/*
 * Invalidating the icache at the kernels virtual page isn't really
 * going to do us much good, since we wouldn't have executed any
 * instructions there.
 */
#define flush_icache_page(vma,pg)	do { } while (0)

/*
 * I cache coherency stuff.
 *
 * This *is not* just icache.  It is to make data written to memory
 * consistent such that instructions fetched from the region are what
 * we expect.
 *
 * This generally means that we have to clean out the Dcache and write
 * buffers, and maybe flush the Icache in the specified range.
 */
#define flush_icache_range(_s,_e)					\
	do {								\
		cpu_icache_invalidate_range((_s), (_e));		\
	} while (0)

/*
 * TLB flushing.
 *
 *  - flush_tlb_all()			flushes all processes TLBs
 *  - flush_tlb_page(vma, vmaddr)	flushes TLB for specified page
 *  - flush_tlb_range(mm, start, end)	flushes TLB for specified range of pages
 *
 * We drain the write buffer in here to ensure that the page tables in ram
 * are really up to date.  It is more efficient to do this here...
 */

#define flush_tlb_all()				    \
    do {					    \
	cpu_tlb_invalidate_all();		    \
    } while (0);

#define flush_tlb_range(_mm,_start,_end)		\
    do {						\
	cpu_tlb_invalidate_range((_start)+(_mm)->procid,\
		(_end)+(_mm)->procid);	\
    } while (0)

#define flush_tlb_page(_mm,_vma,_vaddr)	    \
    do {					    \
	cpu_tlb_invalidate_page((_vaddr)+(_mm)->procid, \
	    ((_vma)->vm_flags & VM_EXEC));	    \
    } while (0);


#define flush_tlb_mm(_mm)						\
	do {								\
			cpu_tlb_invalidate_all();			\
	} while (0)

/*
 * 32-bit ARM Processors don't have any MMU cache
 */
#define update_mmu_cache(vma,address,pte)   do { } while (0)

#endif	//_HAL_ARM_CACHE_H_
