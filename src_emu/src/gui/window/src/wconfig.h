/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : PLX窗口系统配置文件, 用于定制窗口系统.
 *            
\**************************************************************************/

#ifndef __WCONFIG_H
#define __WCONFIG_H

#include "version.h"

#define PDA_RADIANT

#if defined(PDA_RADIANT)            // 润迪PDA平台

#ifdef LANGUAGE_CHN
#define USERAREA1 1
#else
#define CHARSET_GSM_BTP
#endif

#define SYSFONTHEIGHT   23          //系统字体的高度
#define SYSFONTWIDTH    23          //系统字体的宽度
#define CAPTIONHEIGHT   31 //20          //系统Caption的高度 //by axli

#define MENUVGAP        4           //vertical gap between two menu items
#define MENULMARGIN     15          //left margin in a popup menu

#define PDAAPI          1
/* The type of GUI version */
#define HANDLEISPTR     0           // 直接使用指针代替句柄

#define INTERFACE_MONO  1           // 使用PDA新的界面

#define MAX_USER_OBJS   512         // USER对象数
//change gdi obj max to 4096 to pass tck test
//#define MAX_GDI_OBJS    512         // GDI对象数
#define MAX_GDI_OBJS    4096         // GDI对象数

#define NODWMOVESIZE    1           // 禁止默认窗口移动和改变大小
#define NOCURSOR        1           // 不需要光标显示
#define NOMSEVMERGE     1           // 不进行鼠标(笔)事件的合并

#define NOKEYBOARD      0           // 没有键盘设备
#define NOKEYBOARDDEV   0           // 没有键盘设备
#define NOFOCUSRECT     1           // 不要焦点矩形

#define NOSENDCTLCOLOR  0           // 控件(static除外)不发送CTLCOLOR消息

#define RTC             1           // 支持实时时钟

#define PRINTERSUPPORT  0           // 支持打印机
#define NOMENUS         0           // 支持菜单
#define COLUMN_BITMAP   0           // 位图不以列方式存放
#define BIG5SUPPORT     0           // 不支持big5
#define OLDFILEAPI      0           // 不使用旧的文件系统接口
#define OLDFONTDRV      0           // 不使用旧的字体驱动程序接口

#define FOCUSBORDER     1           // 使用焦点边框表示子窗口的焦点

#if !defined(_DDI_VER)
#define _DDI_VER        200         // 使用设备驱动程序接口1.0
#endif

#elif defined(PDA_LEGEND860)        // 联想PDA860平台

#define INTERFACE_MONO  1           // 使用PDA新的界面

#define MAX_USER_OBJS   256         // USER对象数
#define MAX_GDI_OBJS    256         // GDI对象数

#define NODWMOVESIZE    1           // 禁止默认窗口移动和改变大小
#define NOCURSOR        1           // 不需要光标显示
#define NOMSEVMERGE     1           // 不进行鼠标(笔)事件的合并

#define NOKEYBOARD      0           // 没有键盘设备
#define NOKEYBOARDDEV   0           // 没有键盘设备
#define NOFOCUSRECT     1           // 不要焦点矩形

#define NOSENDCTLCOLOR  1           // 控件(static除外)不发送CTLCOLOR消息

#define RTC             0           // 支持实时时钟

#define PRINTERSUPPORT  1           // 支持打印机
#define NOMENUS         0           // 支持菜单
#define COLUMN_BITMAP   1           // 位图以列方式存放
#define BIG5SUPPORT     0           // 不支持big5
#define OLDFILEAPI      0           // 不使用旧的文件系统接口
#define OLDFONTDRV      0           // 不使用旧的字体驱动程序接口

#if !defined(_DDI_VER)
#define _DDI_VER        200         // 使用设备驱动程序接口1.0
#endif

#elif defined(PDA_LEGEND810)        // 联想PDA平台
#define INTERFACE_MONO  0           // 不使用PDA新的界面

#define MAX_USER_OBJS   256         // USER对象数
#define MAX_GDI_OBJS    256         // GDI对象数

#define NODWMOVESIZE    1           // 禁止默认窗口移动和改变大小
#define NOCURSOR        1           // 不需要光标显示
#define NOMSEVMERGE     1           // 不进行鼠标(笔)事件的合并

#define NOKEYBOARD      1           // 没有键盘设备
#define NOFOCUSRECT     0           // 要焦点矩形

#define NOSENDCTLCOLOR  1           // 控件(static除外)不发送CTLCOLOR消息

#define RTC             1           // 支持实时时钟

#define PRINTERSUPPORT  0           // 不支持打印机
#define NOMENUS         0           // 支持菜单
#define COLUMN_BITMAP   0           // 位图不以列方式存放
#define BIG5SUPPORT     0           // 不支持big5
#define OLDFILEAPI      0           // 不使用旧的文件系统接口
#define OLDFONTDRV      0           // 不使用旧的字体驱动程序接口

#elif defined(STB_WINBOND)          // Winbond产品平台

#define INTERFACE_MONO  0           // 不使用PDA新的界面

#define MAX_USER_OBJS   256         // USER对象数
#define MAX_GDI_OBJS    256         // GDI对象数

#define NODWMOVESIZE    1           // 禁止默认窗口移动和改变大小
#define NOCURSOR        0           // 需要光标显示
#define NOMSEVMERGE     0           // 进行鼠标(笔)事件的合并

#define NOKEYBOARDDEV   0           // 没有键盘设备
#define NOKEYBOARD      0           // 没有键盘设备
#define NOFOCUSRECT     0           // 要焦点矩形

#define NOSENDCTLCOLOR  0           // 控件(static除外)不发送CTLCOLOR消息

#define RTC             1           // 支持实时时钟

#define PRINTERSUPPORT  1           // 支持打印机
#define NOMENUS         0           // 支持菜单
#define COLUMN_BITMAP   0           // 位图不以列方式存放
#define BIG5SUPPORT     1           // 支持big5
#define OLDFILEAPI      1           // 使用旧的文件系统接口
#define OLDFONTDRV      1           // 使用旧的字体驱动程序接口

#if !defined(_DDI_VER)
#define _DDI_VER        200         // 使用设备驱动程序接口1.0
#endif

#elif defined(APP_WINBOND)          // Winbond应用程序平台
#define INTERFACE_MONO  0           // 不使用PDA新的界面

#define MAX_USER_OBJS   256         // USER对象数
#define MAX_GDI_OBJS    1024        // GDI对象数

#define NODWMOVESIZE    0           // 禁止默认窗口移动和改变大小
#define NOCURSOR        0           // 需要光标显示
#define NOMSEVMERGE     0           // 进行鼠标(笔)事件的合并

#define NOKEYBOARD      0           // 没有键盘设备
#define NOKEYBOARDDEV   0           // 没有键盘设备
#define NOFOCUSRECT     0           // 要焦点矩形

#define NOSENDCTLCOLOR  0           // 控件(static除外)不发送CTLCOLOR消息

#define RTC             0           // 不支持实时时钟

#define PRINTERSUPPORT  0           // 不支持打印机
#define NOMENUS         0           // 支持菜单
#define COLUMN_BITMAP   0           // 位图不以列方式存放
#define BIG5SUPPORT     0           // 不支持big5
#define OLDFILEAPI      0           // 不使用旧的文件系统接口
#define OLDFONTDRV      0           // 不使用旧的字体驱动程序接口

#if !defined(_DDI_VER)
#define _DDI_VER        100         // 使用设备驱动程序接口1.0
#endif

#elif defined(STB_MOTOROLA)         // Motorola PowerPC机顶盒平台

#define INTERFACE_MONO  0           // 不使用PDA新的界面

#define MAX_USER_OBJS   256         // USER对象数
#define MAX_GDI_OBJS    1024        // GDI对象数

#define NODWMOVESIZE    1           // 禁止默认窗口移动和改变大小
#define NOCURSOR        0           // 需要光标显示
#define NOMSEVMERGE     0           // 进行鼠标(笔)事件的合并

#define NOKEYBOARD      0           // 没有键盘设备
#define NOKEYBOARDDEV   0           // 没有键盘设备
#define NOFOCUSRECT     0           // 要焦点矩形

#define NOSENDCTLCOLOR  0           // 控件(static除外)不发送CTLCOLOR消息

#define RTC             0           // 不支持实时时钟

#define PRINTERSUPPORT  0           // 不支持打印机
#define NOMENUS         1           // 不支持菜单
#define COLUMN_BITMAP   0           // 位图不以列方式存放
#define BIG5SUPPORT     1           // 支持big5
#define OLDFILEAPI      0           // 不使用旧的文件系统接口
#define OLDFONTDRV      0           // 不使用旧的字体驱动程序接口

#if !defined(_DDI_VER)
#define _DDI_VER        100         // 使用设备驱动程序接口1.0
#endif

#elif defined(APP_MOTOROLA)         // 应用程序平台

#define INTERFACE_MONO  0           // 不使用PDA新的界面

#define MAX_USER_OBJS   256         // USER对象数
#define MAX_GDI_OBJS    1024        // GDI对象数

#define NODWMOVESIZE    0           // 禁止默认窗口移动和改变大小
#define NOCURSOR        0           // 需要光标显示
#define NOMSEVMERGE     0           // 进行鼠标(笔)事件的合并

#define NOKEYBOARD      0           // 没有键盘设备
#define NOFOCUSRECT     0           // 要焦点矩形

#define NOSENDCTLCOLOR  0           // 控件(static除外)不发送CTLCOLOR消息

#define RTC             0           // 不支持实时时钟

#define PRINTERSUPPORT  0           // 不支持打印机
#define NOMENUS         0           // 支持菜单
#define COLUMN_BITMAP   0           // 位图不以列方式存放
#define BIG5SUPPORT     0           // 不支持big5
#define OLDFILEAPI      0           // 不使用旧的文件系统接口
#define OLDFONTDRV      0           // 不使用旧的字体驱动程序接口

#if !defined(_DDI_VER)
#define _DDI_VER        100         // 使用设备驱动程序接口1.0
#endif

#else                               // 默认配置

#if (__MP_PLX_GUI)
#pragma comment(linker, "/section:.data,rws /section:.bss,rws")
#pragma comment(linker, "/base:""0x40000000")
#endif

#define HANDLEISPTR     0           // 直接使用指针代替句柄
#define INTERFACE_MONO  0           // 不使用PDA新的界面

#define MAX_USER_OBJS   256         // USER对象数
#define MAX_GDI_OBJS    1024        // GDI对象数

#define NODWMOVESIZE    0           // 禁止默认窗口移动和改变大小
#define NOCURSOR        0           // 需要光标显示
#define NOMSEVMERGE     0           // 进行鼠标(笔)事件的合并

#define NOKEYBOARD      0           // 没有键盘设备
#define NOKEYBOARDDEV   0           // 没有键盘设备
#define NOFOCUSRECT     0           // 要焦点矩形

#define NOSENDCTLCOLOR  0           // 控件(static除外)不发送CTLCOLOR消息

#define RTC             0           // 不支持实时时钟

#define PRINTERSUPPORT  0           // 不支持打印机
#define NOMENUS         0           // 支持菜单
#define COLUMN_BITMAP   0           // 位图不以列方式存放
#define BIG5SUPPORT     0           // 不支持big5
#define OLDFILEAPI      0           // 不使用旧的文件系统接口
#define OLDFONTDRV      0           // 不使用旧的字体驱动程序接口

#if !defined(_DDI_VER)
#define _DDI_VER        200         // 使用设备驱动程序接口1.0
#endif

#endif

#if (__ST_PLX_GUI)
#define USE_WINDOW_MEMORY 0
#else
#define USE_WINDOW_MEMORY 1
#endif


// 仿真环境定制
#if defined(_EMULATE_)

// 仿真环境固定使用键盘设备
#if NOKEYBOARD
#undef NOKEYBOARD
#define NOKEYBOARD  0
#endif

// 仿真环境需重新定义设备驱动程序接口版本
#if defined(_DDI_VER)
#undef _DDI_VER
#endif  // _DDI_VER

// 仿真环境需重新定义是否支持光标
// 仿真环境暂时不支持光标显示
#if NOCURSOR
#undef NOCURSOR
#define  NOCURSOR   0
#endif

// 仿真环境使用设备驱动程序2.0, 
#define _DDI_VER 200

// 如果使用旧设备驱动程序接口仿真环境不支持光标
#if !defined(_DDI_VER)
#define NOCURSOR    1
#endif

// 仿真环境使用窗口系统调试功能
#if !defined(WINDEBUG)
#define WINDEBUG
#endif

#endif // _EMULATE_
#endif // __WCONFIG_H
