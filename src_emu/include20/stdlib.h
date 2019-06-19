/**************************************************************************\
 *
 *                      PDAOS Window System
 *
 * Copyright (c) 1998 by CASS Corporation & Software Engineering Center
 *                       All Rights Reserved
 *
 *
 * Filename     stdlib.h
 *
 * Purpose      Standard library header.
 *  
 *                size_t    unsigned integral type of the result of sizeof
 *
 *                NULL      a null pointer constant
 *
 *                RAND_MAX  maximum value returned by the rand function
 *
 * Author       WeiZhong
 *
\**************************************************************************/

#ifndef __STDLIB_H
#define __STDLIB_H

#if __cplusplus
extern "C" {
#endif

#ifndef __SIZE_T
#define __SIZE_T
    typedef unsigned int	size_t;
#endif

typedef struct { int  rem; int  quot; }  div_t;
typedef struct { long rem; long quot; } ldiv_t;

#ifndef NULL
#define NULL 0
#endif

#ifdef _EMULATE_
#define malloc  os_malloc
#define free    os_free
#endif

#define RAND_MAX	32767

int     atoi(const char* string);
long    atol(const char* string);
long    strtol(const char* nptr, char** endptr, int base);
unsigned long strtoul(const char* nptr, char** endptr, int base);

int     rand(void);
void    srand(unsigned int seed);

void    free(void* memblock);
void*   malloc(size_t size);

void    *bsearch(const void *, const void *, size_t, size_t,
                 int (*)(const void*, const void*));
void    qsort(void *, size_t, size_t, int (*)(const void*, const void*));

int     abs(int n);
div_t   div(int number, int denom);
long    labs(long l);
ldiv_t  ldiv(long number, long denom);

#if __cplusplus
}
#endif

#endif
