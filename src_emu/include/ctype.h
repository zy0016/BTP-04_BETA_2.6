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

#ifndef _CTYPE_H
#define _CTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
    For K&R compatibility don't always extern these with prototypes.
************************************************************************/
#if _INTBITS || __cplusplus
int isalnum( int );
int isalpha( int );
int isascii( int );
int iscntrl( int );
int isdigit( int );
int isgraph( int );
int islower( int );
int isprint( int );
int ispunct( int );
int isspace( int );
int isupper( int );
int isxdigit(int );
int tolower( int );
int toupper( int );
#endif


extern const unsigned char _uctype_[];

#define _U	0x01	/* upper case letter */
#define _L	0x02	/* lower case letter */
#define _N	0x04	/* digit [0 - 9] */
#define _S	0x08	/* white space */
#define _P	0x10	/* all chars that are not control or alphanumeric */
#define _C	0x20	/* control character */
#define _B	0x40	/* just the space (0x20) character */
#define _X	0x80	/* hexadecimal digit */

#if !_INTBITS || _CHARBITS == 8
/********************************************************************
	Only implement the macros for small chars because the table
	must be directly indexable by values EOF,0...UCHAR_MAX.
	Each character in the array _uctype is represented by a
	mask of 8 bits as defined here.
********************************************************************/

#define iscntrl(c)	((_uctype_+128)[(c)] & _C)
#define isupper(c)	((_uctype_+128)[(c)] & _U)
#define islower(c)	((_uctype_+128)[(c)] & _L)
#define isdigit(c)	((_uctype_+128)[(c)] & _N)
#define isxdigit(c)	((_uctype_+128)[(c)] & (_X | _N))
#define isspace(c)	((_uctype_+128)[(c)] & _S)
#define ispunct(c)	((_uctype_+128)[(c)] & _P)
#define isalpha(c)	((_uctype_+128)[(c)] & (_U | _L))
#define isalnum(c)	((_uctype_+128)[(c)] & (_U | _L | _N))
#define isgraph(c)	((_uctype_+128)[(c)] & (_U | _L | _N | _P))
#define isprint(c)	((_uctype_+128)[(c)] & (_U | _L | _N | _P | _B))

#define	isascii(c)	(((c) & ~0x7f) == 0)	/* If C is a 7 bit value.  */

/* Non-gcc versions will get the library versions, and will be
   slightly slower */
#ifdef __GNUC__
# define toupper(c) \
	({ int __x = (c); islower(__x) ? (__x - 'a' + 'A') : __x;})
# define tolower(c) \
	({ int __x = (c); isupper(__x) ? (__x - 'A' + 'a') : __x;})
#else
int toupper(int);
int tolower(int);
#endif
#endif

int isblank( int );

#ifdef __cplusplus
}
#endif

#endif /* _CTYPE_H */
