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

#ifndef _LIMITS_H_
#define _LIMITS_H_

/* Number of bits in a `char'.	*/
#define CHAR_BIT	8

/* Minimum and maximum values a `signed char' can hold.  */
#define SCHAR_MIN	(-128)
#define SCHAR_MAX	127

/* Maximum value an `unsigned char' can hold.  (Minimum is 0.)  */
#define UCHAR_MAX	255

/* Minimum and maximum values a `char' can hold.  */
#ifdef __CHAR_UNSIGNED__
# define CHAR_MIN	0
# define CHAR_MAX	UCHAR_MAX
#else
# define CHAR_MIN	SCHAR_MIN
# define CHAR_MAX	SCHAR_MAX
#endif

/* Minimum and maximum values a `signed short int' can hold.  */
#define SHRT_MIN	(-32768)
#define SHRT_MAX	32767

/* Maximum value an `unsigned short int' can hold.  (Minimum is 0.)  */
#define USHRT_MAX	65535

/* Minimum and maximum values a `signed int' can hold.  */
#define INT_MIN		(-INT_MAX - 1)
#define INT_MAX		2147483647

/* Maximum value an `unsigned int' can hold.  (Minimum is 0.)  */
#define UINT_MAX	4294967295U

/* Minimum and maximum values a `signed long int' can hold.  */
#define LONG_MAX	2147483647L
#define LONG_MIN	(-LONG_MAX - 1L)

/* Maximum value an `unsigned long int' can hold.  (Minimum is 0.)  */
#define ULONG_MAX	4294967295UL

#ifndef __KERNEL__
#define NAME_MAX		32
#define	PATH_MAX		255
#endif

#define	OPEN_MAX		20
#define	PIPE_BUF		4096
#define SIZE_MAX		0x01000000
#define	TZNAME_MAX		8

#define _POSIX_NAME_MAX		NAME_MAX
#define	_POSIX_OPEN_MAX		OPEN_MAX
#define	_POSIX_PATH_MAX		PATH_MAX
#define	_POSIX_PIPE_BUF		PIPE_BUF
#define	_POSIX_SIZE_BUF		SIZE_BUF
#define	_POSIX_TZNAME_MAX	TZNAME_MAX

#endif	// _LIMITS_H
