/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	HOPEN OS Kernel System
 *
 * Purpose :    Define the assert macro.
 *  
 * Author  :	Hejia Li
 *
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Revision: 1.4 $     $Date: 2003/07/10 02:25:47 $
 *
 * $History::                                                       $
 * 
\**************************************************************************/

#ifndef __HPDIAG_H_
#define __HPDIAG_H_

#ifdef _DEBUG

#if defined(_EMULATE_) || defined(WIN32) /* For Emulator */

extern int OS_GetError (void);

#define _CRT_WARN       0
#define _CRT_ASSERT     2
int __cdecl _CrtDbgReport(
        int,
        const char *,
        int,
        const char *,
        const char *,
        ...);

#define ASSERT(expr) \
  do {    \
    if (!(expr) && \
        (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, #expr))) \
        __asm { int 3 }; \
  } while (0)

#define TRACE
#define TRACELINE() _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, "\r\n")

#define TRACE1(sz, s1) TRACELINE()
#define TRACE2(sz, s1, s2) TRACELINE()
#define TRACE3(sz, s1, s2, s3) TRACELINE()
#define TRACE4(sz, s1, s2, s3, s4) TRACELINE()
/*
#define TRACE1(sz, s1) _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, sz, s1)
#define TRACE2(sz, s1, s2) _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, sz, s1, s2)
#define TRACE3(sz, s1, s2, s3) _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, sz, s1, s2, s3)
#define TRACE4(sz, s1, s2, s3, s4) _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, sz, s1, s2, s3, s4)
*/
#define TRACEERROR()  TRACE1("Error: errno=%d occured!!\r\n", OS_GetError())

#else   /* _EMULATE_    */

#if	defined( _M68328_ )	/* legend pda */

int _FS_DbgReport(const char *, int, const char *);

#define ASSERT(expr) \
    if( ! (expr) ) { \
    _FS_DbgReport(__FILE__, __LINE__, #expr); \
    }

#define TRACE                       
#define TRACE1(sz, s1)              
#define TRACE2(sz, s1, s2)          
#define TRACE3(sz, s1, s2, s3)      
#define TRACE4(sz, s1, s2, s3, s4)   
#define TRACELINE()   
#define TRACEERROR()

#else	/* #if defined( _M68328_ ) */

extern int OS_GetError (void);

#define ASSERT(f)          ((void)0)

#define TRACE                       printf
#define TRACE1(sz, s1)              {printf(sz, s1);}
#define TRACE2(sz, s1, s2)          {printf(sz, s1, s2);}
#define TRACE3(sz, s1, s2, s3)      {printf(sz, s1, s2, s3);}
#define TRACE4(sz, s1, s2, s3, s4)  {printf(sz, s1, s2, s3, s4);}
#define TRACELINE()  \
	printf("TRACE: File: %s, Line: %d\r\n", __FILE__, __LINE__)
#define TRACEERROR() \
	printf("Error: %d\toccured at File: %s, Line: %d\r\n", \
	OS_GetError(), __FILE__, __LINE__)

#endif  /* #if defined( _M68328_ ) */

#endif  /* _EMULATE_    */

#else   /* _DEBUG		*/

#define ASSERT(f)          ((void)0)
#define TRACE                       
#define TRACE1(sz, s1)               
#define TRACE2(sz, s1, s2)           
#define TRACE3(sz, s1, s2, s3)       
#define TRACE4(sz, s1, s2, s3, s4)   
#define TRACELINE()   
#define TRACEERROR() 

#endif	/* !_DEBUG		*/

#define assert ASSERT

#endif	/* __HPDIAG_H_	*/
