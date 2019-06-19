/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	HOPEN LIB
 *
 * Filename:    malloc.h
 *
 * Purpose      Standard library header.
 *
 * Author  :	WeiZhong
 *
  *
 * $Archive:: /hopen_system/include/malloc.h                        $
 * $Workfile:: malloc.h                                             $
 * $Revision:: 2  $     $Date:: 03-04-10 4:04p                      $
 *
\**************************************************************************/

#ifndef __MALLOC_H
#define __MALLOC_H

#if __cplusplus
extern "C" {
#endif

#ifndef __SIZE_T
#define __SIZE_T
    typedef unsigned int	size_t;
#endif

#ifndef NULL
#define NULL 0
#endif

// Redefine the malloc and free function name to avoid conflict with C run
// time library
#ifdef _MSC_VER

#ifndef malloc

#define malloc  os_malloc
#define free    os_free
//#define realloc os_realloc

void*   malloc(size_t size);
void    free(void* memblock);
void*   realloc(void *memblock, size_t size);

#endif  // malloc

#else   // _MSC_VER

#ifndef malloc
void*   malloc(size_t size);
void    free(void* memblock);
void*   realloc(void *memblock, size_t size);
#endif

#endif

#if __cplusplus
}
#endif

#endif
