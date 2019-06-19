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

#ifndef	_STRING_H
#define	_STRING_H	1

#ifndef NULL
#define NULL		((void *) 0)
#endif

#ifndef __SIZE_T
#define __SIZE_T
#if defined(__i386) || !defined (__GNUC__)
typedef unsigned int	size_t;
#else
typedef unsigned long	size_t;
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Copy N bytes of SRC to DEST.  */
void *memcpy (void * dest, const void *src, size_t n);

/* Copy N bytes of SRC to DEST, guaranteeing
   correct behavior for overlapping strings.  */
void *memmove (void * dest, const void * src, size_t n);

/* Set N bytes of S to C.  */
void * memset (void * str, int c, size_t);

/* Compare N bytes of S1 and S2.  */
int memcmp (const void * s1, const void * s2, size_t n);

/* Find char C in the first COUNT bytes of BUF.  */
void* memchr(const void* buf, int c, size_t count);

/* Copy SRC to DEST.  */
char *strcpy (char * dest, const char * src);

/* Copy no more than N characters of SRC to DEST.  */
char *strncpy (char * dest, const char * src, size_t n);

/* Append SRC onto DEST.  */
char *strcat (char * dest, const char * src);

/* Find char in string.  */
char *strchr (const char * str, int chr);

/* Find char from the end of string.  */
char * strrchr(const char *s, int i);

/* Append no more than N characters from SRC onto DEST.  */
char *strncat (char * dest, const char * src, size_t n);

/* Compare S1 and S2.  */
int strcmp (const char * s1, const char * s2);

/* Compare N characters of S1 and S2.  */
int strncmp (const char * s1, const char * s2, size_t n);

/* Compare string ignoring case */
int strcasecmp (const char * s1, const char * s2);

/* Compare N characters of S1 and S2 ignoring case */
int strncasecmp (const char * s1, const char * s2, size_t);

/* Return the length of S.  */
unsigned int strlen (const char *__s);

/* Duplicate S, returning an identical malloc'd string.  */
char *strdup (const char *s);

/* locate a substring.  */
char *strstr(const char *haystack, const char *needle);
       
/* extract tokens from strings.  */
char *strtok(char *s, const char *delim);
char *strtok_r(char *s, const char *delim, char **lasts);

/* convert to upper */
char *strupr(char *a);

/* search a string for a set of characters.  */
size_t strspn(const char *s, const char *accept);
size_t strcspn(const char *s, const char *reject);

/* find chars in string.  */
char *strpbrk(const char *s, const char *accept);

/* We define this function always since `bzero' is sometimes needed when
   the namespace rules does not allow this.  */
void __bzero (void * s, size_t n);

/* Copy N bytes of SRC to DEST (like memmove, but args reversed).  */
void bcopy (const void * src, void * dest, size_t n);

/* Set N bytes of S to 0.  */
void bzero (void * s, unsigned int n);

/* Compare N bytes of S1 and S2 (same as memcmp).  */
int bcmp (const void * s1, const void * s2, size_t n);

/* Compare S1 and S2 case-insensitive.  */
int stricmp (const char * s1, const char * s2);

/* Return string describing error code ERRNUM.  */
char *strerror(int errnum);

#ifdef __cplusplus
}
#endif

/* For compatible of microsoft VC */
#define stricmp(s1,s2)		strcasecmp(s1, s2)
#define strnicmp(s1,s2,n)	strncasecmp(s1, s2, n)

#endif /* string.h  */
