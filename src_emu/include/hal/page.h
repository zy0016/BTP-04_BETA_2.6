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
 * $Source: /cvs/hopencvs/src/include/hal-armv/page.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/05/30 06:06:09 $
 *
\**************************************************************************/

#ifndef __HAL_ARM_PAGE_H_
#define __HAL_ARM_PAGE_H_



extern struct page * mem_map;
extern unsigned long map_nr;

#define phys_to_virt(paddr)	(void *)((unsigned long)(paddr) + (PAGE_OFFSET - PHYS_OFFSET))
#define virt_to_phys(vaddr)	((unsigned long)(vaddr) + (PHYS_OFFSET - PAGE_OFFSET))

#define virt_to_page(vaddr)	(mem_map + (((unsigned long)(vaddr) - PAGE_OFFSET) >> PAGE_SHIFT))
#define page_to_virt(page)	((void *)((((page) - mem_map) << PAGE_SHIFT) + PAGE_OFFSET))

#define phys_to_page(paddr)	(mem_map + (((unsigned long)(paddr) - PHYS_OFFSET) >> PAGE_SHIFT))
#define page_address(page)	((((page) - mem_map) << PAGE_SHIFT) + PHYS_OFFSET)

#endif
