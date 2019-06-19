#ifndef _HP_DIAG_H
#define _HP_DIAG_H

/**************************************************************************/
/*              Diagnostic support                                        */
/**************************************************************************/

#ifdef _MSC_VER /* For Microsoft Visual C++ */

#ifdef _DEBUG

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
#else   /* _DEBUG       */

#define ASSERT(f)          ((void)0)

#endif  /* !_DEBUG      */

#else   /* _MSC_VER     */

#define ASSERT(f)          ((void)0)

#endif  /* _MSC_VER     */

#endif	/* _HP_DIAG_H	*/
