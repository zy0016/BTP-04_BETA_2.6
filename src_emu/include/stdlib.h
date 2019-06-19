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

#ifndef __STDLIB_H_
#define __STDLIB_H_

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

#define RAND_MAX 0x7fffffff

#ifdef __cplusplus
extern "C" {
#endif

void *malloc (size_t _size);
void free (void* memblock);
void *realloc (void *_r, size_t _size);
void *calloc (size_t num, size_t elem_size);

int  abs (int i);
int  atoi (const char *nptr);
long atol (const char *nptr);
long strtol (const char *nptr, char **endptr, int base);
unsigned long strtoul (const char *nptr, char **endptr, int base);
char *realpath (const char *name, char *resolved);

char *getenv (const char *name);
int  setenv (const char *name, const char *value, int replace);
void unsetenv (const char *name);

void abort (void);
void exit (int status);

void srand (unsigned int seed);
int  rand (void);

/* Convert a string to a floating-point number.  */
double strtod (const char * nptr, char ** endptr);

/* Convert a string to a floating-point number.  */
double atof(const char *nptr);

/* Convert a floating-point number to a string.  */
char * ecvt(double number, int ndigits, int *decpt, int *sign);

/* Convert a floating-point number to a string.  */
char  *fcvt(double number, int ndigits, int *decpt, int *sign);

/* Convert a floating-point number to a string. */
char *gcvt(double number, int ndigit, char *buf);

/*Performs a quick sort*/
void qsort( void *base, size_t num, size_t width, 
            int (* compare )(const void *elem1, const void *elem2 ) );

#ifdef __cplusplus
}
#endif

#endif /* __STDLIB_H_ */
