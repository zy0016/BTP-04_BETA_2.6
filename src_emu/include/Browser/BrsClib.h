/**************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   :  wap browser
 *
 * Purpose  : 为浏览器抽象C库函数
 *
\**************************************************************************/

#ifndef _BRSCLIB_H
#define _BRSCLIB_H


#ifdef __cplusplus
extern "C"{
#endif
/*****************************************************************
 Hopen Platform
*****************************************************************/
#ifdef _WBFOR_HOPEN_
#     include "math.h"
#ifndef PLX_TI
#     include "ctype.h"
#     include "stdlib.h"
#     include <stdarg.h>
#else
#	  include "plxosal.h"
#endif


#  ifdef _SAMSUNG_
#    include "stdio.h"
#    include "string.h"
#    ifndef _EMULATE_
#      define  stricmp       strcasecmp
#      define  strnicmp      strncasecmp
#    endif
#  else
#ifndef PLX_TI
#    include "stdio.h"
#	 include "string.h"
#endif
#  endif

#define WB_atof         atof
#define WB_strtod       strtod

#ifdef _BENEFON_
#   define   wb_strnicmp    strncasecmp
#   define   wb_stricmp     strcasecmp

#	undef	stricmp
#	define  stricmp      strcasecmp
#	undef	strnicmp
#	define  strnicmp     strncasecmp
#endif

#ifdef PLX_TI
#	define   strcasecmp    stricmp
#	define  strncasecmp    strnicmp 

int   strnicmp(const char * s1, const char * s2, size_t n);
int   stricmp(const char* dst, const char* src);
    
#endif


long  wb_strtol(const char *nptr, char **endptr, int base);
int   wb_atoi(const char *str);
long  wb_atol(const char *str);
char* wb_itoa(long value, char* str, int radix);

#ifndef _BENEFON_

int   wb_strnicmp(const char * s1, const char * s2, int n);
int   wb_stricmp(const char* dst, const char* src);
#endif

char* wb_strupr(char * string);
void* wb_memset(void *dst, int val, int count);
int   wb_strlen(const char * str);

#ifndef _WALDI_
#ifndef _BENEFON_
#ifndef PLX_TI
extern int    strnicmp(const char *, const char *, int);
#endif
#endif
extern int    sprintf( char *buffer, const char *format, ... );
extern double strtod( const char *nptr, char **endptr );
extern double atof( const char *string );
#endif



#endif

/*****************************************************************
 Windows Platform
*****************************************************************/
#ifdef WB_FOR_MSWIN
#ifndef PLX_TI
#  include <string.h>
#  include <math.h>
#  include "stdlib.h"
#  include <stdarg.h>
#else
#  include "plxosal.h"
#endif

#   define   wb_strtol      strtol
#   define   wb_atoi        atoi
#   define   wb_atol        atol
#   define   wb_itoa        itoa
#   define   wb_strnicmp    strnicmp
#   define   wb_strupr      strupr
#   define   wb_stricmp     stricmp
#   define   wb_memset      memset
#   define   wb_strlen      strlen
#   define   WB_atof        atof
#   define   WB_strtod      strtod

#endif

/*****************************************************************
 Skyworks Platform
*****************************************************************/
#ifdef _SKYWORKS_
#include <stddef.h>
#include <stdarg.h>
#include "plxclib.h"

#   define   wb_strtol      plx_strtol  
#   define   wb_atoi        plx_atoi    
#   define   wb_atol        plx_atol        
#   define   wb_itoa        plx_itoa 
#   define   wb_strnicmp    plx_strncasecmp
#   define   wb_strupr      plx_strupr
#   define   wb_stricmp     plx_strcasecmp 
#   define   wb_memset      plx_memset  
#   define   wb_strlen      plx_strlen  

#   define   WB_atof        plx_atof
#   define   WB_strtod      plx_strtod

extern void qsort( void *base, int num, int width, int (*compare )(const void *elem1, const void *elem2 ) );
extern unsigned long strtoul( const char *nptr, char **endptr, int base );

#endif

#ifdef _RONI_
#include "plxdef.h"
#include "plxclib.h"

#   define   wb_strtol      plx_strtol  
#   define   wb_atoi        plx_atoi    
#   define   wb_atol        plx_atol        
#   define   wb_itoa        plx_itoa 
#   define   wb_strnicmp    plx_strncasecmp
#   define   wb_strupr      plx_strupr
#   define   wb_stricmp     plx_strcasecmp 
#   define   wb_memset      plx_memset  
#   define   wb_strlen      plx_strlen  

#   define   WB_atof        plx_atof
#   define   WB_strtod      plx_strtod

extern void qsort( void *base, int num, int width, int (*compare )(const void *elem1, const void *elem2 ) );
extern unsigned long strtoul( const char *nptr, char **endptr, int base );

#endif

/*****************************************************************
 Apoxi Platform
*****************************************************************/
#ifdef _APOXI
//#   include "plxdef.h"
#   include "wbdef.h"

typedef char *  va_list;
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
#define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v) )
#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)      ( ap = (va_list)0 )

//#include <stdarg.h>
//#include <math.h>
/*   WB_XXX 系列函数的声明在wbdef.h中  */
#   define   atoi        WB_atoi
#   define   strtol      WB_strtol
#   define   strtoul     WB_strtoul
#   define   malloc      WB_malloc
#   define   realloc     WB_realloc
#   define   free        WB_free
#   define   memcpy      WB_memcpy
#   define   memmove     WB_memmove
#   define   memcmp      WB_memcmp
#   define   memchr      WB_memchr
#   define   memset      WB_memset

#   define   strcpy      WB_strcpy
#   define   strncpy     WB_strncpy
#   define   strcat      WB_strcat
#   define   strncat     WB_strncat
#   define   strcmp      WB_strcmp
#   define   stricmp     WB_stricmp
#   define   strncmp     WB_strncmp
#   define   strnicmp    WB_strnicmp

#   define   strchr      WB_strchr
#   define   strcspn     WB_strcspn
#   define   strpbrk     WB_strpbrk
#   define   strrchr     WB_strrchr
#   define   strspn      WB_strspn
#   define   strstr      WB_strstr
#   define   strtok      WB_strtok
#   define   strlen      WB_strlen
#   define   strupr      WB_strupr
#   define   strdup      WB_strdup
#   define   rand        WB_rand 
#   define   srand       WB_srand

#   define   pow         WB_pow
#   define   sqrt        WB_sqrt

#   define   wb_strtol      WB_strtol  
#   define   wb_atoi        WB_atoi    
#   define   wb_atol        WB_atol        
#   define   wb_itoa        WB_itoa 
#   define   wb_strnicmp    WB_strnicmp
#   define   wb_strupr      WB_strupr
#   define   wb_stricmp     WB_stricmp 
#   define   wb_memset      WB_memset  
#   define   wb_strlen      WB_strlen  

#   define   isxdigit       WB_isxdigit
#   define   isalpha        WB_isalpha
#   define   isalnum        WB_isalnum
#   define   isdigit        WB_isdigit

/*   The declaration are in   "  include "wbdef.h" "
double WB_atof( const char *string );
char*  WB_itoa(int value, char* str, int radix);
double WB_strtod(const char* p1, char** p2);
int    WB_atoi (const char* string);
long   WB_atol(const char *str);
long   WB_strtol (const char * s1, char **ps, int n);
void*  WB_memcpy(void*dst, const void*src, unsigned long count);
void*  WB_memmove(void*dst, const void*src, long count);
int    WB_memcmp(const void* buf1, const void* buf2, long count);
void*  WB_memchr(const void* buf, int c, long count);
void*  WB_memset(void* dst, int c, long count);
char*   WB_strcpy(char* dst, const char* src);
char*   WB_strncpy(char* dst, const char* src, long count);
char*   WB_strcat(char* dst, const char* src);
char*   WB_strncat(char* dst, const char* src, long count);
int     WB_strcmp(const char* src, const char* dst);
int     WB_stricmp(const char* s1, const char* s2);
int     WB_strncmp(const char* s1, const char* s2, long n);
int     WB_strnicmp(const char * s1, const char * s2, long n);
char*   WB_strchr(const char *string, int c);
char*   WB_strrchr(const char* str, int c);
long    WB_strspn(const char* str, const char* strCharSet);
char*   WB_strstr(const char *string, const char *strCharSet);
char*   WB_strtok(char* strToken, const char* strDelimit);
long    WB_strlen(const char* str);
char*   WB_strupr(char *string);
char*   WB_strdup(const char *str);
int     WB_rand(void);
void    WB_srand(unsigned int n);
double  WB_pow( double x, double y );
double  WB_sqrt( double x );
*/

#endif

/*****************************************************************
 QT Platform
*****************************************************************/
#ifdef WB_FOR_QT

#include "string.h"
#include "math.h"
#include "stdlib.h"
#include <stdarg.h>

#   define   wb_strtol      strtol
#   define   wb_atoi        atoi
#   define   wb_atol        atol
#   define   wb_strnicmp    strncasecmp
#   define   wb_stricmp     strcasecmp
#   define   wb_memset      memset
#   define   wb_strlen      strlen

#	define   WB_atof		atof
#	define	 WB_strtod		strtod

#	define  stricmp      strcasecmp
#	define  strnicmp     strncasecmp

#   define   itoa			wb_itoa
#   define   strupr			wb_strupr

char* wb_itoa(long value, char* str, int radix);
char* wb_strupr(char * string);


#endif

#ifdef PLX_TI
#   define   strupr			wb_strupr
#endif

int sprintf( char *buffer, const char *format, ... );

#ifdef __cplusplus
}
#endif

#endif 
