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
 * $Source: /cvs/hopencvs/src/include/hal-armv/pgtable.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/05/30 06:06:09 $
 *
\**************************************************************************/

#ifndef __HAL_ARM_PGTABLE_H_
#define __HAL_ARM_PGTABLE_H_

#include    <hal/page.h>
#include    <hal/cache.h>

#define PGDIR_SHIFT	20
#define PGDIR_MASK	(~0L << PGDIR_SHIFT)
#define PGDIR_SIZE	(1L << PGDIR_SHIFT)

#define PTRS_PER_PTE	256
#define BYTES_PER_PTR	4

//typedef struct pgd {unsigned long value;} pgd_t;
//typedef struct pte {unsigned long value;} pte_t;
//typedef struct pgprot {unsigned long value;} pgprot_t;
typedef int pgd_t;
typedef int pte_t;
typedef int pgprot_t;

//#define pte_val(p)	(p).value
//#define pgd_val(p)	(p).value
//#define pgprot_val(p)	(p).value

#define pte_val(p)	(p)
#define pgd_val(p)	(p)
#define pgprot_val(p)	(p)


#define PTE_PRESENT	0x0002
#define PTE_DIRTY	0x0800

#define PTE_PROT_MASK	0x0030	    /* Only Ap 0 used */
#define  PTE_RW		0x0030
#define  PTE_RDONLY	0x0020	    /* Read only in used mode */
#define  PTE_KERNEL	0x0010	    /* Only kernel access */

#define PTE_TYPE_MASK	0x000C
#define  PTE_RAM	0x000C	    /* Cache, buffer */
#define  PTE_ROM	0x0008	    /* Cache, no buffer */
#define  PTE_IO		0x0000	    /* no cache, no buffer */

#define pte_none(p)	(pte_val(p) == 0)
#define pte_present(p)	(pte_val(p) & PTE_PRESENT)
#define pte_isram(p)	((pte_val(p) & PTE_TYPE_MASK) == PTE_RAM)
#define pte_read(p)	(pte_val(p) & PTE_RDONLY)
#define pte_write(p)	((pte_val(p) & PTE_PROT_MASK) == PTE_RW)
#define pte_dirty(p)	(pte_val(p) & PTE_DIRTY)
#define pte_same(p1,p2)	(((pte_val(p1) ^ pte_val(p2)) & 0xFFFFF03F) == 0) 

#define pgd_none(p)	(pgd_val(p) == 0)
#define pgd_present(p)	(pgd_val(p) & 0x0001)

#define pte_wrprotect(p)    (pte_t)(pte_val(p) & ~0x0010)
#define pte_mkwrite(p)	    (pte_t)(pte_val(p) | 0x0010)
#define pte_mkdirty(p)	    (pte_t)(pte_val(p) | PTE_DIRTY)
#define pte_mkclean(p)	    (pte_t)(pte_val(p) & ~PTE_DIRTY)

#define mk_pte_phys(pa,type,prot)	((pa) | pgprot_val(prot) | type | PTE_PRESENT)
#define mk_pte(pg,type,prot)		mk_pte_phys(page_address(pg), type, prot)

extern pgprot_t pte_protect(int vm_flags);

extern pgd_t * pgd_offset(struct mm_struct * mm, unsigned long);
extern pte_t * pgd_page(pgd_t);

#define pte_offset(p,a)	(pgd_page(*(p)) + (((a) & ~PGDIR_MASK)>>PAGE_SHIFT))
#define pte_page(p)	((struct page *)phys_to_page(p))

extern void set_pgd(struct mm_struct * mm, pgd_t *, pte_t *);
extern void pgd_clear(pgd_t *);

extern void set_pte (pte_t *, pte_t);
#define pte_clear(p)	set_pte(p, 0)

extern pte_t * pte_alloc_one(struct mm_struct *, unsigned long);
extern void pte_free_one(pte_t *);

#endif
