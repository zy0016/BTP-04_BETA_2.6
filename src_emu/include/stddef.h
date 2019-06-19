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

#ifndef _STDDEF_H_
#define _STDDEF_H_

#ifndef NULL
#define NULL	(void *)0
#endif

#ifndef offsetof
#define offsetof(T,F)	((unsigned int)((char *)&(*(T *)0L).F - (char *)0L))
#endif

typedef int ptrdiff_t;

#ifndef __SIZE_T
#define __SIZE_T
#if defined(__i386) || !defined (__GNUC__)
typedef unsigned int	size_t;
#else
typedef unsigned long	size_t;
#endif
#endif

#endif //_STDDEF_H
