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
 * $Source: /cvs/hopencvs/src/include/hal-armv/pgalloc.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/05/30 06:06:09 $
 *
\**************************************************************************/

#ifndef __HAL_ARM_PGALLOC_H
#define __HAL_ARM_PGALLOC_H

#include <hal/page.h>
/*
 * ARM processors do not cache TLB tables in RAM.
 */
#define flush_tlb_pgtables(mm,start,end)	do { } while (0)

pte_t *pte_alloc(struct mm_struct *mm, pmd_t *pmd, unsigned long address);

/*
 * Allocate one PTE table.
 *
 * Note that we keep the processor copy of the PTE entries separate
 * from the Linux copy.  The processor copies are offset by -PTRS_PER_PTE
 * words from the Linux copy.
 */
extern pte_t *pte_alloc_one(struct mm_struct *mm, unsigned long address);
/*
 * Free one PTE table.
 */
extern void pte_free(pte_t *pte);

/*
 * Populate the pmdp entry with a pointer to the pte.  This pmd is part
 * of the mm address space.
 */
#define pmd_populate(mm,pmdp,pte)	set_pmd(pmdp, __mk_pmd(pte, PMD_TYPE_TABLE))
#define pmd_free(pmd)			do { } while (0)

extern pgd_t * pgd_alloc(struct mm_struct *mm);
extern void pgd_free(struct mm_struct *mm);

#endif
