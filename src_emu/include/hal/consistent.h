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
 * $Source: /cvs/hopencvs/src/include/hal-armv/consistent.h,v $
 * $Name:  $
 *
 * $Revision: 1.1 $     $Date: 2004/02/23 02:15:14 $
 * 
\**************************************************************************/

#ifndef _ARM_CONSISTENT_H
#define _ARM_CONSISTENT_H

#define DMA_SYNC_NONE			0
#define DMA_SYNC_FROMDEVICE		1
#define DMA_SYNC_TODEVICE		2
#define DMA_SYNC_BIDIRECTION	3

extern void *consistent_alloc_page(int gfp, dma_addr_t *dma_handle);
extern void consistent_free_page(void *vaddr, dma_addr_t handle);
extern void consistent_sync(void *vaddr, size_t size, int direction);

#endif /* _ARM_CONSISTENT_H */
