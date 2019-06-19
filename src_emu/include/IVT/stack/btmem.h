/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    btmem.h
Abstract:
	This file includes the definition of static memory allocation functions.
Author:
    Gang He
Revision History:2002.3
---------------------------------------------------------------------------*/
#ifndef BTMEM_H
#define BTMEM_H

#ifdef CONFIG_TYPE_BIT16_ONLY
#define HUGE huge
#else
#define HUGE 
#endif

#define BLK_IDXNUM	(CONFIG_BLOCK_NUMBER>>3)
#define MAX_BITS	(BLK_IDXNUM<<3)
#define POOL_SIZE	((DWORD)CONFIG_BLOCK_NUMBER*CONFIG_BLOCK_SIZE)

void * mem_malloc (int size);
void mem_free(void * buf);

void * mem_realloc (void * buf, int size);
void * mem_calloc (int num, int size);

void InitStaticMem(void);
void DoneStaticMem(void);

extern int      mem_debug_breakat;
#ifdef CONFIG_DEBUG
WORD GetFreePages(void);
#endif

#ifdef CONFIG_MEM_USER_FUNC
typedef void* (FuncUserAlloc) (int);
typedef void (FuncUserDeAlloc) (void*);
typedef void (FuncUserMemAlert) (void);
void RegisterUserFunc(UCHAR* ualloc,UCHAR* udealloc,UCHAR* ualert);
#endif

#endif
