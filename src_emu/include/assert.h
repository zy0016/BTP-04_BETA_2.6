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


#ifndef _ASSERT_H_
#define	_ASSERT_H_

#ifdef _DEBUG

#ifndef __KERNEL__

#include "stdio.h"
#include "errno.h"

extern void error_inf(int reportType, const char* prompt, const char* pFile, int nLine);

#define _CRT_WARN       0
#define _CRT_ASSERT     2

#define ASSERT(expr) \
  do {    \
    if (!(expr)) \
        error_inf(_CRT_ASSERT, #expr, __FILE__, __LINE__); \
  } while (0)

#define assert(expr)	ASSERT(expr)

#define TRACE       printf
#define TRACELINE() error_inf(_CRT_WARN, "\r\n", __FILE__, __LINE__)

#define TRACE1(sz, s1)              TRACELINE()
#define TRACE2(sz, s1, s2)          TRACELINE()
#define TRACE3(sz, s1, s2, s3)      TRACELINE()
#define TRACE4(sz, s1, s2, s3, s4)  TRACELINE()

#define TRACEERROR()    TRACE1("Error: errno=%d occured!!\r\n", errno)

#else
#define ASSERT(expr)	(void)0
#define assert(expr)	(void)0
#endif //__KERNEL__

#else
extern void error_inf(int reportType, const char* prompt, const char* pFile, int nLine);

#define ASSERT(expr)	(void)0
#define assert(expr)	(void)0

#endif //_DEBUG

#endif	/* _ASSERT_H_ */
