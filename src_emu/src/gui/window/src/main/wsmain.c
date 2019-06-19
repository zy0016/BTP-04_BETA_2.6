/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implementation application entry point function.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "wsinit.h"

#ifndef MODULECOMPILE

#ifdef _EMULATE_
#if (__MP_PLX_GUI)
#define MAXWIDTH	640
#define MAXHEIGHT	480
static unsigned char framebuffer[MAXWIDTH * MAXHEIGHT * 3] ;

unsigned char *GetFrameBuffer(void)
{
    return framebuffer;
}
#endif
#endif

#if (defined(_LINUX_OS_) && !defined(_HOPEN_VER))

extern int  LinuxInit(void);
extern void LinuxFini(void);

int main(int argc, char** argv)
{
#if !defined(_EMULATE_)
    if (!LinuxInit())
    {
        ASSERT(0);
        return 0;
    }
#endif  // _EMULATE_

    if (!WS_Init())
    {
        ASSERT(0);
        return 0;
    }

    if (!WSUI_Init())
    {
        ASSERT(0);
        return 0;
    }

    WinMain(0, 0, 0, 0);

    WSUI_Exit();
    WS_Exit();

#if !defined(_EMULATE_)
    LinuxFini();
#endif  // _EMULATE_

    return 0;
}

#elif (_HOPEN_VER >= 300)

typedef int (*entry_func) (int cmd, ...);

#if defined(_EMULATE_)
/*
int main()
{
    // TODO:init window system
    printf("Initialize Window system ... ");

    if (!WS_Init())
    {
        printf(" fail \n");
        ASSERT(0);
        return FALSE;
    }
    
    if (!WSUI_Init())
    {
        printf(" fail \n");
        ASSERT(0);
        return FALSE;
    }
    
    printf(" OK \n");
    
    return TRUE;
}
*/
#endif  // _EMULATE_

/*
**  Function : WS_Main
**  Purpose  :
**      本函数为窗口系统的初始化函数。
*/
BOOL InitPLXGUI(int (*syscall_entry) (int cmd, ...))
{
#if defined(_EMULATE_)
#endif  // _EMULATE_

    if (!WS_Init())
    {
        ASSERT(0);
        return FALSE;
    }

    printf("FILE: %s, LINE: %d\r\n", __FILE__, __LINE__);
    if (!WSUI_Init())
    {
        ASSERT(0);
        return FALSE;
    }

    printf("FILE: %s, LINE: %d\r\n", __FILE__, __LINE__);
    return TRUE;
}

#else   // _HOPEN_VER < 300

/*
**  Function : AppMain
**  Purpose  :
**      本函数为窗口应用程序模块该模块的入口函数。在PLX系统所有模块一起编
**      译连接时，窗口应用程序模块实现为窗口任务，在main函数中创建。本函数
**      为窗口任务的实体。
*/
void AppMain(void)
{
    if (!WS_Init())
    {
        ASSERT(0);
        return;
    }

    if (!WSUI_Init())
    {
        ASSERT(0);
        return;
    }

    WinMain(0, 0, 0, 0);

    WSUI_Exit();
    WS_Exit();
}

#endif  // _HOPEN_VER

#else // MODULECOMPILE

#include "wsyscall.h"       // Window system syscall index definition
#include "wsapitbl.h"       // Window system api function table

// Kernel API prototypes
int OS_SetSyscallHandle(int handle, int (*entry)(void));

// Window system call entry function prototypes
static int SysCallEntry(void);

#if defined (__GNUC__) && defined (m68000)

typedef int(*ENTRY)(void);
static ENTRY GetApiFuncTbl(void)
{
    __asm ("move.l #ApiFuncTbl, %d0");
    __asm ("move.l %d0, %a0");
}

/*
**  Function : WIN_Init
**  Purpose  :
**      The entry of window system module.
*/
int WIN_Init(void)
{
    // 设置窗口系统系统调用函数入口
    OS_SetSyscallHandle(0x4C, GetApiFuncTbl());

    // 初始化窗口系统
    WS_Init();
}

#else // __GNUC__ && m68000

/*
**  Function : dll_main
**  Purpose  :
**      The entry of window system module.
*/
int dll_main(void)
{
    // 设置窗口系统系统调用函数入口
    OS_SetSyscallHandle(0x4C, SysCallEntry);

    // 初始化窗口系统
    WS_Init();
}

// For x86 platform and VC compiler
#ifdef X86VC

#pragma warning(disable : 4035)

_declspec (naked) 
static int SysCallEntry(void)
{
    _asm 
    {
        /* Remove return auto pushed EIP, CS, EFLAGS,
        ** Jump to user expend function 
        */
        add     esp, 12
        and     eax, 01ffh
        jmp     DWORD PTR ApiFuncTbl[eax * 4]
    }
}

#endif  // X86VC

// For x86 platform and GCC compiler

#ifdef X86GCC

static int SysCallEntry(void)
{
}

#endif  // X86GCC

#ifdef HPPAGCC

static int SysCallEntry(void)
{
    // Save data base register to caller's frame,
    // Save return address to caller's frame.
    // load data base of this module.
    __asm ("stw		%dp,-24(%sp)\n"
        "	ldil	L%ApiFuncTbl,%r19\n"
        "	ldil	L%$global$,%dp\n"
        "	ldo		R%ApiFuncTbl(%r19),%r19\n"
        "	ldo		R%$global$(%dp),%dp\n"
        "	ldwx,s	%r22(0,%r19),%r22\n"
        "	stw		%r2,-8(%sp)\n" );

    /* Call target function */
    __asm ("ble		0(%sr4,%r22)\n"
        "	copy	%r31,%r2\n"  );

    /* Restore calles's dp and return */
    __asm ("ldw		-8(%sp),%r2\n"
        "	bv		0(%r2)\n"
        "	ldw		-24(%sp),%dp\n" );
}

#endif // HPPAGCC

#endif // __GNUC__ && m68000

#endif // MODULECOMPILE
