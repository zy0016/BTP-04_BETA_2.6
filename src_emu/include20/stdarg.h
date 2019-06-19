/**************************************************************************\
 *
 *                      PDAOS Window System
 *
 * Copyright (c) 1998 by CASS Corporation & Software Engineering Center
 *                       All Rights Reserved
 *
 *
 * Filename     ctype.h
 *
 * Purpose      Standard variable argument header.
 *
 *              va_start    initializes an arg pointer of type va_list.
 *              va_arg      gets the next argument given its promoted type.
 *                      NOTE : the type cannot be "char" or "short" and
 *                      instead must be the promoted type ("int") for
 *                      such arguments.
 *              va_end      must be done at the end.
 *                
 *              A function defined to take a variable number of arguments
 *              should be patterned after:
 *
 *              #include <stdarg.h>
 *                
 *              int f(int a, char *b, ...) 
 *              {  
 *                  int x; 
 *                  long y;
 *
 *                  va_list argp;
 *                  va_start(argp, b);
 *                      x = va_arg(argp, int);
 *                      y = va_arg(argp, long);
 *                  va_end( argp ); 
 *              }
 *  
 * Author       WeiZhong
 *
\**************************************************************************/

#ifndef __STDARG_H
#define __STDARG_H

#ifndef __VA_LIST
#define __VA_LIST
typedef char *va_list;
#endif

#define va_start(ap, parmN) ((ap) = ((char*)&parmN) + sizeof(parmN))
#define va_arg(ap, type)    ((type *)((ap) += sizeof(type)))[-1]
#define va_end(ap)

#endif
