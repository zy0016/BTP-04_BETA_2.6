/**************************************************************************\
 *
 *                      PDAOS Window System
 *
 * Copyright (c) 1998 by CASS Corporation & Software Engineering Center
 *                       All Rights Reserved
 *
 *
 * Filename     stdio.h
 *
 * Purpose      Standard Input/Output header.
 *  
 * Author       WeiZhong
 *
\**************************************************************************/

#ifndef __STDIO_H
#define __STDIO_H

#if __cplusplus
extern "C" {
#endif

#ifndef __VA_LIST
#define __VA_LIST
typedef char *va_list;
#endif

#ifndef NULL
#define NULL 0
#endif

int sprintf(char* buffer, const char* format, ...);
int vsprintf(char* buffer, const char* format, va_list argptr);

#if __cplusplus
}
#endif

#endif
