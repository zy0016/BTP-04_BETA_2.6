/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : 重新定义 Microsoft Visual C++的warning编译选项, 使VC的编译
 *            效果与其他平台编译器一致. 使用本头文件时请将VC编译设置中的
 *            警告级别设置为4级, 即打开所有警告.
 *            
\**************************************************************************/

#ifndef __WARNING_H
#define __WARNING_H

#ifdef _MSC_VER /* For Microsoft Visual C++ */

#pragma warning(disable : 4001) /* nonstandard extension 'single line     */
                                /* comment' was used                      */
#pragma warning(disable : 4100) /* unreferenced formal parameter          */

/* Disable the unimportant warnings if WARNING_ALL is undefined */
#ifndef WARNING_ALL

#pragma warning(disable : 4127) /* conditional expression is constant     */
#pragma warning(disable : 4244) /* conversion from int to unsigned short  */
#pragma warning(disable : 4310) /* cast truncates constant value          */
#pragma warning(disable : 4505) /* unreferenced local function has been   */
                                /* removed                                */
#pragma warning(disable : 4701) /* local variable may be used without     */
                                /* having been initialized                */

#endif /* WARNING_ALL */

/* Report the specified warnings as errors */
#pragma warning(error : 4057)   /* 'identifier1' indirection to slightly  */
                                /* different base types from 'identifier2'*/

#endif /* _MSC_VER */

#pragma warning(error : 4013)
#pragma warning(error : 4028)
#pragma warning(error : 4029)
#pragma warning(error : 4033)
#pragma warning(error : 4142)

#endif  /* __WARNING_H */
