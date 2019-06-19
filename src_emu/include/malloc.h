/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _MALLOC_H_
#define _MALLOC_H_

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef __SIZE_T
#define __SIZE_T
#if defined(__i386) || !defined (__GNUC__)
typedef unsigned int	size_t;
#else
typedef unsigned long	size_t;
#endif
#endif

#ifndef NULL
#define NULL 0
#endif

void *malloc (size_t _size);
void free (void* memblock);
void *realloc (void *_r, size_t _size);
void *calloc (size_t num, size_t elem_size);

#ifdef __cplusplus
}
#endif

#endif	//_MALLOC_H_
