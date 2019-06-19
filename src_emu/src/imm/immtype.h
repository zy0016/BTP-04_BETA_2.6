/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : immtype.h
 *
 * Purpose  : define input method edit struct and output function
 *            
\**************************************************************************/

#ifndef	HAVE_IMMTYPE_H
#define	HAVE_IMMTYPE_H

#include "version.h"

#ifndef INT_DEFINE
#define INT_DEFINE

typedef	signed	char	INT8;
typedef	signed	short	INT16;
typedef	signed	long	INT32;
typedef	unsigned char	UINT8;
typedef	unsigned short	UINT16;
typedef	unsigned long	UINT32;

#endif

#undef _HUGE
#if (_C166)
#define _HUGE   _huge
#else
#define _HUGE
#endif

#ifndef TCHAR
#define TCHAR char
#endif

#ifndef LANGUAGE_CHN
#define IME_NO_CHINESE
#endif

#endif
