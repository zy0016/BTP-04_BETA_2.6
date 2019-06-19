/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	HOPEN LIB
 *
 * Purpose      String header
 *
 * Author  :	WeiZhong
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /hopen_system/include/string.h                        $
 * $Workfile:: string.h                                             $
 * $Revision:: 2  $     $Date:: 03-04-10 4:04p                      $
 *
\**************************************************************************/

#ifndef __STRING_H
#define __STRING_H

/**************************************************************************/
/*                  String Definitions and Functions                    */ 
/**************************************************************************/

#ifndef __SIZE_T
#define __SIZE_T
typedef unsigned int size_t;
#endif

#ifndef NULL
#define NULL 0
#endif

void*   memcpy(void*dst, const void*src, size_t count);
void*   memmove(void*dst, const void*src, size_t count);
int     memcmp(const void* buf1, const void* buf2, size_t count);
void*   memchr(const void* buf, int c, size_t count);
void*   memset(void* dst, int c, size_t count);

char*   strcpy(char* dst, const char* src);
char*   strncpy(char* dst, const char* src, size_t count);
char*   strcat(char* dst, const char* src);
char*   strncat(char* dst, const char* src, size_t count);
int     strcmp(const char* src, const char* dst);
int     stricmp(const char* src, const char* dst);
int     strncmp(const char* first, const char* last, size_t count);
char*   strchr(const char* str, int c);
size_t  strcspn(const char* str, const char* strCharSet);
char*   strpbrk(const char* str, const char* strCharSet);
char*   strrchr(const char* str, int c);
size_t  strspn(const char* str, const char* strCharSet);
char*   strstr(const char* str, const char* strCharSet);
char*   strtok(char* strToken, const char* strDelimit);
size_t  strlen(const char* str);
char*   strupr(char *string);

#endif
