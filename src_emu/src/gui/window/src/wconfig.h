/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : PLX����ϵͳ�����ļ�, ���ڶ��ƴ���ϵͳ.
 *            
\**************************************************************************/

#ifndef __WCONFIG_H
#define __WCONFIG_H

#include "version.h"

#define PDA_RADIANT

#if defined(PDA_RADIANT)            // ���PDAƽ̨

#ifdef LANGUAGE_CHN
#define USERAREA1 1
#else
#define CHARSET_GSM_BTP
#endif

#define SYSFONTHEIGHT   23          //ϵͳ����ĸ߶�
#define SYSFONTWIDTH    23          //ϵͳ����Ŀ��
#define CAPTIONHEIGHT   31 //20          //ϵͳCaption�ĸ߶� //by axli

#define MENUVGAP        4           //vertical gap between two menu items
#define MENULMARGIN     15          //left margin in a popup menu

#define PDAAPI          1
/* The type of GUI version */
#define HANDLEISPTR     0           // ֱ��ʹ��ָ�������

#define INTERFACE_MONO  1           // ʹ��PDA�µĽ���

#define MAX_USER_OBJS   512         // USER������
//change gdi obj max to 4096 to pass tck test
//#define MAX_GDI_OBJS    512         // GDI������
#define MAX_GDI_OBJS    4096         // GDI������

#define NODWMOVESIZE    1           // ��ֹĬ�ϴ����ƶ��͸ı��С
#define NOCURSOR        1           // ����Ҫ�����ʾ
#define NOMSEVMERGE     1           // ���������(��)�¼��ĺϲ�

#define NOKEYBOARD      0           // û�м����豸
#define NOKEYBOARDDEV   0           // û�м����豸
#define NOFOCUSRECT     1           // ��Ҫ�������

#define NOSENDCTLCOLOR  0           // �ؼ�(static����)������CTLCOLOR��Ϣ

#define RTC             1           // ֧��ʵʱʱ��

#define PRINTERSUPPORT  0           // ֧�ִ�ӡ��
#define NOMENUS         0           // ֧�ֲ˵�
#define COLUMN_BITMAP   0           // λͼ�����з�ʽ���
#define BIG5SUPPORT     0           // ��֧��big5
#define OLDFILEAPI      0           // ��ʹ�þɵ��ļ�ϵͳ�ӿ�
#define OLDFONTDRV      0           // ��ʹ�þɵ�������������ӿ�

#define FOCUSBORDER     1           // ʹ�ý���߿��ʾ�Ӵ��ڵĽ���

#if !defined(_DDI_VER)
#define _DDI_VER        200         // ʹ���豸��������ӿ�1.0
#endif

#elif defined(PDA_LEGEND860)        // ����PDA860ƽ̨

#define INTERFACE_MONO  1           // ʹ��PDA�µĽ���

#define MAX_USER_OBJS   256         // USER������
#define MAX_GDI_OBJS    256         // GDI������

#define NODWMOVESIZE    1           // ��ֹĬ�ϴ����ƶ��͸ı��С
#define NOCURSOR        1           // ����Ҫ�����ʾ
#define NOMSEVMERGE     1           // ���������(��)�¼��ĺϲ�

#define NOKEYBOARD      0           // û�м����豸
#define NOKEYBOARDDEV   0           // û�м����豸
#define NOFOCUSRECT     1           // ��Ҫ�������

#define NOSENDCTLCOLOR  1           // �ؼ�(static����)������CTLCOLOR��Ϣ

#define RTC             0           // ֧��ʵʱʱ��

#define PRINTERSUPPORT  1           // ֧�ִ�ӡ��
#define NOMENUS         0           // ֧�ֲ˵�
#define COLUMN_BITMAP   1           // λͼ���з�ʽ���
#define BIG5SUPPORT     0           // ��֧��big5
#define OLDFILEAPI      0           // ��ʹ�þɵ��ļ�ϵͳ�ӿ�
#define OLDFONTDRV      0           // ��ʹ�þɵ�������������ӿ�

#if !defined(_DDI_VER)
#define _DDI_VER        200         // ʹ���豸��������ӿ�1.0
#endif

#elif defined(PDA_LEGEND810)        // ����PDAƽ̨
#define INTERFACE_MONO  0           // ��ʹ��PDA�µĽ���

#define MAX_USER_OBJS   256         // USER������
#define MAX_GDI_OBJS    256         // GDI������

#define NODWMOVESIZE    1           // ��ֹĬ�ϴ����ƶ��͸ı��С
#define NOCURSOR        1           // ����Ҫ�����ʾ
#define NOMSEVMERGE     1           // ���������(��)�¼��ĺϲ�

#define NOKEYBOARD      1           // û�м����豸
#define NOFOCUSRECT     0           // Ҫ�������

#define NOSENDCTLCOLOR  1           // �ؼ�(static����)������CTLCOLOR��Ϣ

#define RTC             1           // ֧��ʵʱʱ��

#define PRINTERSUPPORT  0           // ��֧�ִ�ӡ��
#define NOMENUS         0           // ֧�ֲ˵�
#define COLUMN_BITMAP   0           // λͼ�����з�ʽ���
#define BIG5SUPPORT     0           // ��֧��big5
#define OLDFILEAPI      0           // ��ʹ�þɵ��ļ�ϵͳ�ӿ�
#define OLDFONTDRV      0           // ��ʹ�þɵ�������������ӿ�

#elif defined(STB_WINBOND)          // Winbond��Ʒƽ̨

#define INTERFACE_MONO  0           // ��ʹ��PDA�µĽ���

#define MAX_USER_OBJS   256         // USER������
#define MAX_GDI_OBJS    256         // GDI������

#define NODWMOVESIZE    1           // ��ֹĬ�ϴ����ƶ��͸ı��С
#define NOCURSOR        0           // ��Ҫ�����ʾ
#define NOMSEVMERGE     0           // �������(��)�¼��ĺϲ�

#define NOKEYBOARDDEV   0           // û�м����豸
#define NOKEYBOARD      0           // û�м����豸
#define NOFOCUSRECT     0           // Ҫ�������

#define NOSENDCTLCOLOR  0           // �ؼ�(static����)������CTLCOLOR��Ϣ

#define RTC             1           // ֧��ʵʱʱ��

#define PRINTERSUPPORT  1           // ֧�ִ�ӡ��
#define NOMENUS         0           // ֧�ֲ˵�
#define COLUMN_BITMAP   0           // λͼ�����з�ʽ���
#define BIG5SUPPORT     1           // ֧��big5
#define OLDFILEAPI      1           // ʹ�þɵ��ļ�ϵͳ�ӿ�
#define OLDFONTDRV      1           // ʹ�þɵ�������������ӿ�

#if !defined(_DDI_VER)
#define _DDI_VER        200         // ʹ���豸��������ӿ�1.0
#endif

#elif defined(APP_WINBOND)          // WinbondӦ�ó���ƽ̨
#define INTERFACE_MONO  0           // ��ʹ��PDA�µĽ���

#define MAX_USER_OBJS   256         // USER������
#define MAX_GDI_OBJS    1024        // GDI������

#define NODWMOVESIZE    0           // ��ֹĬ�ϴ����ƶ��͸ı��С
#define NOCURSOR        0           // ��Ҫ�����ʾ
#define NOMSEVMERGE     0           // �������(��)�¼��ĺϲ�

#define NOKEYBOARD      0           // û�м����豸
#define NOKEYBOARDDEV   0           // û�м����豸
#define NOFOCUSRECT     0           // Ҫ�������

#define NOSENDCTLCOLOR  0           // �ؼ�(static����)������CTLCOLOR��Ϣ

#define RTC             0           // ��֧��ʵʱʱ��

#define PRINTERSUPPORT  0           // ��֧�ִ�ӡ��
#define NOMENUS         0           // ֧�ֲ˵�
#define COLUMN_BITMAP   0           // λͼ�����з�ʽ���
#define BIG5SUPPORT     0           // ��֧��big5
#define OLDFILEAPI      0           // ��ʹ�þɵ��ļ�ϵͳ�ӿ�
#define OLDFONTDRV      0           // ��ʹ�þɵ�������������ӿ�

#if !defined(_DDI_VER)
#define _DDI_VER        100         // ʹ���豸��������ӿ�1.0
#endif

#elif defined(STB_MOTOROLA)         // Motorola PowerPC������ƽ̨

#define INTERFACE_MONO  0           // ��ʹ��PDA�µĽ���

#define MAX_USER_OBJS   256         // USER������
#define MAX_GDI_OBJS    1024        // GDI������

#define NODWMOVESIZE    1           // ��ֹĬ�ϴ����ƶ��͸ı��С
#define NOCURSOR        0           // ��Ҫ�����ʾ
#define NOMSEVMERGE     0           // �������(��)�¼��ĺϲ�

#define NOKEYBOARD      0           // û�м����豸
#define NOKEYBOARDDEV   0           // û�м����豸
#define NOFOCUSRECT     0           // Ҫ�������

#define NOSENDCTLCOLOR  0           // �ؼ�(static����)������CTLCOLOR��Ϣ

#define RTC             0           // ��֧��ʵʱʱ��

#define PRINTERSUPPORT  0           // ��֧�ִ�ӡ��
#define NOMENUS         1           // ��֧�ֲ˵�
#define COLUMN_BITMAP   0           // λͼ�����з�ʽ���
#define BIG5SUPPORT     1           // ֧��big5
#define OLDFILEAPI      0           // ��ʹ�þɵ��ļ�ϵͳ�ӿ�
#define OLDFONTDRV      0           // ��ʹ�þɵ�������������ӿ�

#if !defined(_DDI_VER)
#define _DDI_VER        100         // ʹ���豸��������ӿ�1.0
#endif

#elif defined(APP_MOTOROLA)         // Ӧ�ó���ƽ̨

#define INTERFACE_MONO  0           // ��ʹ��PDA�µĽ���

#define MAX_USER_OBJS   256         // USER������
#define MAX_GDI_OBJS    1024        // GDI������

#define NODWMOVESIZE    0           // ��ֹĬ�ϴ����ƶ��͸ı��С
#define NOCURSOR        0           // ��Ҫ�����ʾ
#define NOMSEVMERGE     0           // �������(��)�¼��ĺϲ�

#define NOKEYBOARD      0           // û�м����豸
#define NOFOCUSRECT     0           // Ҫ�������

#define NOSENDCTLCOLOR  0           // �ؼ�(static����)������CTLCOLOR��Ϣ

#define RTC             0           // ��֧��ʵʱʱ��

#define PRINTERSUPPORT  0           // ��֧�ִ�ӡ��
#define NOMENUS         0           // ֧�ֲ˵�
#define COLUMN_BITMAP   0           // λͼ�����з�ʽ���
#define BIG5SUPPORT     0           // ��֧��big5
#define OLDFILEAPI      0           // ��ʹ�þɵ��ļ�ϵͳ�ӿ�
#define OLDFONTDRV      0           // ��ʹ�þɵ�������������ӿ�

#if !defined(_DDI_VER)
#define _DDI_VER        100         // ʹ���豸��������ӿ�1.0
#endif

#else                               // Ĭ������

#if (__MP_PLX_GUI)
#pragma comment(linker, "/section:.data,rws /section:.bss,rws")
#pragma comment(linker, "/base:""0x40000000")
#endif

#define HANDLEISPTR     0           // ֱ��ʹ��ָ�������
#define INTERFACE_MONO  0           // ��ʹ��PDA�µĽ���

#define MAX_USER_OBJS   256         // USER������
#define MAX_GDI_OBJS    1024        // GDI������

#define NODWMOVESIZE    0           // ��ֹĬ�ϴ����ƶ��͸ı��С
#define NOCURSOR        0           // ��Ҫ�����ʾ
#define NOMSEVMERGE     0           // �������(��)�¼��ĺϲ�

#define NOKEYBOARD      0           // û�м����豸
#define NOKEYBOARDDEV   0           // û�м����豸
#define NOFOCUSRECT     0           // Ҫ�������

#define NOSENDCTLCOLOR  0           // �ؼ�(static����)������CTLCOLOR��Ϣ

#define RTC             0           // ��֧��ʵʱʱ��

#define PRINTERSUPPORT  0           // ��֧�ִ�ӡ��
#define NOMENUS         0           // ֧�ֲ˵�
#define COLUMN_BITMAP   0           // λͼ�����з�ʽ���
#define BIG5SUPPORT     0           // ��֧��big5
#define OLDFILEAPI      0           // ��ʹ�þɵ��ļ�ϵͳ�ӿ�
#define OLDFONTDRV      0           // ��ʹ�þɵ�������������ӿ�

#if !defined(_DDI_VER)
#define _DDI_VER        200         // ʹ���豸��������ӿ�1.0
#endif

#endif

#if (__ST_PLX_GUI)
#define USE_WINDOW_MEMORY 0
#else
#define USE_WINDOW_MEMORY 1
#endif


// ���滷������
#if defined(_EMULATE_)

// ���滷���̶�ʹ�ü����豸
#if NOKEYBOARD
#undef NOKEYBOARD
#define NOKEYBOARD  0
#endif

// ���滷�������¶����豸��������ӿڰ汾
#if defined(_DDI_VER)
#undef _DDI_VER
#endif  // _DDI_VER

// ���滷�������¶����Ƿ�֧�ֹ��
// ���滷����ʱ��֧�ֹ����ʾ
#if NOCURSOR
#undef NOCURSOR
#define  NOCURSOR   0
#endif

// ���滷��ʹ���豸��������2.0, 
#define _DDI_VER 200

// ���ʹ�þ��豸��������ӿڷ��滷����֧�ֹ��
#if !defined(_DDI_VER)
#define NOCURSOR    1
#endif

// ���滷��ʹ�ô���ϵͳ���Թ���
#if !defined(WINDEBUG)
#define WINDEBUG
#endif

#endif // _EMULATE_
#endif // __WCONFIG_H
