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
 * $Source: /cvs/hopencvs/src/include/hopen/mm.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2003/06/12 02:30:55 $
 *
\**************************************************************************/
#ifndef _HOPEN_MM_H
#define _HOPEN_MM_H
#include  <hopen/list.h>
#include  <hal/pgtable.h>
#include  <hopen/mutex.h>

/*
 * This struct defines a memory VMM memory area. There is one of these
 * per VM-area/task.  A VM area is any part of the process virtual memory
 * space that has a special rule for the page-fault handlers (ie a shared
 * library, the executable area etc).
 */
struct vm_area_struct {
        struct mm_struct * vm_mm;       /* VM area parameters */
        unsigned long vm_start;
        unsigned long vm_end;

        pgprot_t vm_page_prot;
        unsigned long vm_flags;
	struct vm_operations_struct * vm_ops;
	struct vm_area_struct * vm_next;	/* Link to next node */
};

struct vm_operations_struct {
        void (*open)(struct vm_area_struct * area);
        void (*close)(struct vm_area_struct * area);
        struct page * (*nopage)(struct vm_area_struct * area,
	    unsigned long address, int write_access);
};

/*
 * vm_flags..
 */
#define VM_READ         0x0001      /* currently active flags */
#define VM_WRITE        0x0002
#define VM_EXEC         0x0004      /* currently active flags */
#define VM_SHARED	0x0008


#define VM_STACK        0x00000100      /* general info on the segment */
#define VM_GROWSDOWN	0x00000010


#define VM_SHM          0x00000400      /* shared memory area, don't swap out */
#define VM_IO           0x00004000      /* Memory mapped I/O or similar */

/*************************************************************************
 * Memory map structure.
 *************************************************************************
 */

struct mm_struct {
	pgd_t *  pgd;			/* Page dir of this structure */
	unsigned long procid;		/* MMU process id, different
					 * from process id */
	unsigned long domain;		/* Domain of this structure */
	unsigned long mapstart, mapend; /* The virtual address start and end */ 
	unsigned long start_brk, brk;
	mutex    page_table_lock;	/* page table lock */	
	mutex    vma_lock;		/* VM area link lock */	
	struct vm_area_struct * vma;	/* Virtual memory structure */
	struct vm_area_struct * vma_cache; /* Cached vma */
};

struct page {
	struct list_head list;
	atomic_t count;
	unsigned short flags;	/* Flags of this page block */
	unsigned short unused;
};

#define PG_dirty		0x0002
#define PG_uptodate		0x0004
#define PG_reserved		0x8000

#define get_page(p)             atomic_inc(&(p)->count)
#define put_page(p)             __free_page(p)
#define page_count(p)           ((p)->count)
#define set_page_count(p,v)     atomic_set(&(p)->count, v)

#define PageReserved(p)		((p)->flags & PG_reserved)

typedef struct zone_struct {
    int gfp_mask;
    unsigned long start_addr;	/* Strat address of this zone */
    unsigned long end_addr;	/* End address of this zone + 1 */
    int total_pages;		/* Total pages of this zone */
    int free_pages;		/* Free pages in this zone */
    struct list_head free_list; /* free areas of different sizes */
} zone_t;

extern struct mm_struct init_mm;

struct page * mm_alloc_one_page (int gfp_mode);
void mm_free_one_page (struct page *page);
int mm_get_free_page_count (int gfp_mode);

struct page * get_vaddr_page(struct mm_struct *mm, unsigned long vaddr);

int map_page_range(struct mm_struct * mm, struct vm_area_struct * vma,
		       unsigned long addr, unsigned long end);
void unmap_page_range(struct mm_struct *mm, unsigned long address, unsigned long end);
void clear_page_tables(struct mm_struct *mm, int first, int nr);
int copy_page_range(struct mm_struct *dst, struct mm_struct *src, struct vm_area_struct *vma);

/* Map physical page mode */
enum map_mode { MAP_RAM, MAP_ROM, MAP_IO };
int map_phys_pages(struct mm_struct * mm, unsigned long vm_addr,
	    unsigned long phys_addr, int pages, int map_mode, pgprot_t pgprot);
void unmap_phys_pages(struct mm_struct *mm, unsigned long vm_addr, int pages);

zone_t * mm_add_zone (int gfp_mask, unsigned long start, unsigned long size);
void mm_add_free_area (zone_t * zone, unsigned long start, unsigned long end);

int expand_stack(struct mm_struct * mm, struct vm_area_struct * vma, unsigned long address);

struct vm_area_struct * find_vma(struct mm_struct * mm, unsigned long addr);
void insert_vma(struct mm_struct *mm, struct vm_area_struct *vma);
int do_brk (struct mm_struct * mm, unsigned long new_brk);

struct mm_struct * dup_mm (struct mm_struct * src_mm);
void free_mm (struct mm_struct * mm);

int handle_mm_fault(struct mm_struct *mm, struct vm_area_struct * vma,
		    unsigned long address, int write_access);
#endif
