/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : Define the assert macro.
 *            
\**************************************************************************/

#ifndef __HPDIAG_H_
#define __HPDIAG_H_

#ifdef _DEBUG
#define THIS_FILE          __FILE__

#ifdef WIN32
#define _CRT_WARN           0
#define _CRT_ERROR          1
#define _CRT_ASSERT         2
#define _CRT_ERRCNT         3

#define _CRTDBG_MODE_FILE      0x1
#define _CRTDBG_MODE_DEBUG     0x2
#define _CRTDBG_MODE_WNDW      0x4
#define _CRTDBG_REPORT_MODE    -1
int __cdecl _CrtDbgReport(
        int,
        const char *,
        int,
        const char *,
        const char *,
        ...);
int __cdecl _CrtSetReportMode(
        int nRptType,
        int fMode
        );

#define ASSERT(expr) \
  do {    \
    if (!(expr) && \
        (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, #expr))) \
        __asm { int 3 }; \
  } while (0)

#define TRACE(expr) \
    _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, expr)

#define TRACE0(sz)              TRACE(sz)
#define TRACE1(sz, p1)  \
    _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, sz, p1)
#define TRACE2(sz, p1, p2) \
    _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, sz, p1, p2)
#define TRACE3(sz, p1, p2, p3) \
    _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, sz, p1, p2, p3)

#else   /* WIN32    */
#include <assert.h>
#ifndef ASSERT
#define ASSERT assert
//void assert(const char* prompt, const char* pFile, int nLine);

/*
#define ASSERT(expr) \
  do {    \
    if (!(expr)) \
        assert(#expr, __FILE__, __LINE__); \
  } while (0)
*/
#endif

#define TRACE(expr) \
  do {    \
    printf("File: %s,Line :%d, ", __FILE__, __LINE__);\
    printf(expr); \
    fflush(NULL); \
  } while (0)

#define TRACE0(sz)              TRACE(sz)
#define TRACE1(sz, p1) \
  do {    \
    printf("File: %s,Line :%d, ", __FILE__, __LINE__);\
    printf(sz, p1); \
    fflush(NULL); \
  } while (0)
#define TRACE2(sz, p1, p2) \
  do {    \
    printf("File: %s,Line :%d, ", __FILE__, __LINE__);\
    printf(sz, p1, p2); \
    fflush(NULL); \
  } while (0)
#define TRACE3(sz, p1, p2, p3) \
  do {    \
    printf("File: %s,Line :%d, ", __FILE__, __LINE__);\
    printf(sz, p1, p2, p3); \
    fflush(NULL); \
  } while (0)

#endif  /* WIN32    */
#define VERIFY(f)               ASSERT(f)
#else   /* _DEBUG_		*/
#define ASSERT(f)          ((void)0)
#define VERIFY(f)          ((void)(f))
#define TRACE(sz)
#define TRACE0(sz)              
#define TRACE1(sz, p1)          
#define TRACE2(sz, p1, p2)      
#define TRACE3(sz, p1, p2, p3)  
#endif	/* !_DEBUG_		*/

#endif	/* __HPDIAG_H_	*/
