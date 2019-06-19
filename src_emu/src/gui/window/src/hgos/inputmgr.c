/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Manages the input device.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "../kernel/wsownd.h"
#include "wsthread.h"
#include "inputmgr.h"
#include "devdisp.h"
#include "../kernel/devqueue.h"
#include "../kernel/wscursor.h"

#if defined(_LINUX_OS_)
#include "inputlinux.c"
#elif (_HOPEN_VER <= 200)
#include "input20.c"
#else
#include "input30.c"
#endif

static LONGKEYMAP LongKey[MAX_LONGKEY];
static DWORD LKUseMask = 0;
static DWORD LKRegMask = 0;
static COMMONMUTUALOBJ sem_longkey;

#if (__MP_PLX_GUI)
#ifdef _EMULATE_
#define SEMNAME_LONGKEY       "$$PLXGUI_LONGKEY$$"
#define LONGKEY_CREATEOBJ(sem)        CREATEOBJ(sem, SEMNAME_LONGKEY)
#define LONGKEY_DESTROYOBJ(sem)       DESTROYOBJ(sem, SEMNAME_LONGKEY)
#define LONGKEY_WAITOBJ(sem)          WAITOBJ(sem, SEMNAME_LONGKEY)
#define LONGKEY_RELEASEOBJ(sem)       RELEASEOBJ(sem, SEMNAME_LONGKEY)
#else
#define LONGKEY_CREATEOBJ(sem)        CREATEOBJ(sem)
#define LONGKEY_DESTROYOBJ(sem)       DESTROYOBJ(sem)
#define LONGKEY_WAITOBJ(sem)          WAITOBJ(sem)
#define LONGKEY_RELEASEOBJ(sem)       RELEASEOBJ(sem)
#endif
#else
#define LONGKEY_CREATEOBJ(sem)        CREATEOBJ(sem)
#define LONGKEY_DESTROYOBJ(sem)       DESTROYOBJ(sem)
#define LONGKEY_WAITOBJ(sem)          WAITOBJ(sem)
#define LONGKEY_RELEASEOBJ(sem)       RELEASEOBJ(sem)
#endif

/*********************************************************************\
* Function	   InitLongKey
* Purpose      长按键支持初始化
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL InitLongKey(void)
{
    LONGKEY_CREATEOBJ(sem_longkey);
    return TRUE;
}

void ExitLongKey(void)
{
    LONGKEY_DESTROYOBJ(sem_longkey);
}
/*********************************************************************\
* Function	   RegisterLongKey
* Purpose      注册一个长按键
* Params	   
    scancode    注册键的扫描码，唯一
    vkShort     注册键的虚键码，唯一
    vkLong      注册键长按的虚键码，调用者指定
    escaped     长按键延迟时间
    bSequence   长按键是否按时间间隔连续发送
* Return	 	   
    成功返回TRUE，失败返回FALSE
* Remarks	   
    本接口不提供很完整的参数检查算法，需要调用者保证参数的有效性。
**********************************************************************/
BOOL WINAPI RegisterLongKey(WORD scancode, WORD vkShort, WORD vkLong, 
                            DWORD escaped, DWORD dwFlags)
{
    int i;

    if (escaped == 0)
        return FALSE;

    /* 不能单独丢掉DOWN的短按键事件，但可以单独丢掉短按键UP事件 */
    if (dwFlags & LONGKEY_EATSHORTDOWN)
        dwFlags |= LONGKEY_EATSHORTUP;

    if ((dwFlags & LONGKEY_EATSHORTUP) && (dwFlags & LONGKEY_EATLONGUP))
        return FALSE;

    LONGKEY_WAITOBJ(sem_longkey);

    for (i = 0; i < MAX_LONGKEY; i++)
    {
        if (LongKey[i].escaped == 0)
            break;
    }

    if (i == MAX_LONGKEY)
    {
        LONGKEY_RELEASEOBJ(sem_longkey);
        return FALSE;
    }

    LongKey[i].scancode = scancode;
    LongKey[i].vkShort = vkShort;
    LongKey[i].vkLong = vkLong;
    LongKey[i].escaped = escaped;
    LongKey[i].millisecond = 0;
    LongKey[i].flags = dwFlags & LONGKEY_USERMASK;
    LongKey[i].flags |= LONGKEY_UNUSED;

    LKRegMask |= (DWORD)1 << i;
    LONGKEY_RELEASEOBJ(sem_longkey);

    return TRUE;
}

/*********************************************************************\
* Function	   UnregisterLongKey
* Purpose      注销一个长按键的使用
* Params	   
    scancode    注册键的扫描码，唯一
    vkShort     注册键的虚键码，唯一
    vkLong      注册键长按的虚键码，调用者指定
    escaped     长按键延迟时间
* Return	 	   
* Remarks	   
    注销的参数应该与注册时的相同，否则会失败。
**********************************************************************/
BOOL WINAPI UnregisterLongKey(WORD scancode, WORD vkShort, WORD vkLong, 
                              DWORD escaped)
{
    int i;

    if (escaped == 0)
        return FALSE;

    LONGKEY_WAITOBJ(sem_longkey);

    for (i = 0; i < MAX_LONGKEY; i++)
    {
        if (LongKey[i].escaped == escaped 
            && LongKey[i].scancode == scancode 
            && LongKey[i].vkShort == vkShort
            && LongKey[i].vkLong == vkLong)
        {
            break;
        }
    }

    if (i == MAX_LONGKEY)
    {
        LONGKEY_RELEASEOBJ(sem_longkey);
        return FALSE;
    }

    LongKey[i].scancode = 0;
    LongKey[i].vkShort = 0;
    LongKey[i].vkLong = 0;
    LongKey[i].millisecond = 0;
    LongKey[i].escaped = 0;
    LongKey[i].flags = 0;

    LKRegMask &= ~((DWORD)1 << i);
    LONGKEY_RELEASEOBJ(sem_longkey);

    return TRUE;
}

/*********************************************************************\
* Function	   StartLongKey
* Purpose      按键按下，启动长按键timer计时
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD StartLongKey(WORD scancode, WORD vkShort)
{
    int i;
    DWORD dwRet = SHORTKEY_DOWN;
    DWORD dwTimeout = MAX_ULONG;

    if (LKRegMask == 0)
        return dwRet;

    LONGKEY_WAITOBJ(sem_longkey);

    for (i = 0; i < MAX_LONGKEY; i++)
    {
        /* 多个down消息会重复这个过程，应该只在第一次产生这个过程 */
        if (!(LongKey[i].flags & LONGKEY_USED) 
            && (scancode == LongKey[i].scancode
            || vkShort == LongKey[i].vkShort))
        {
            /* The first wait timeout should add the elapsed time of 
            ** the last timer task
            */
            LongKey[i].millisecond = LongKey[i].escaped
                + PLXOS_GetElapsedTime(FALSE);
            LongKey[i].flags &= ~LONGKEY_STATEMASK;
            LongKey[i].flags |= LONGKEY_USED;
            LKUseMask |= (DWORD)1 << i;

            if (LongKey[i].flags & LONGKEY_EATSHORTDOWN)
                dwRet = SHORTKEY_NONE;

            if (LongKey[i].millisecond < dwTimeout)
                dwTimeout = LongKey[i].millisecond;
        }
    }

    PLXOS_SetTimerEvent(dwTimeout);
    LONGKEY_RELEASEOBJ(sem_longkey);

    return dwRet;
}

/*********************************************************************\
* Function	   EndLongKey
* Purpose      按键释放，取消长按键timer计时
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD EndLongKey(WORD scancode, WORD vkShort)
{
    int i;
    DWORD dwRet = SHORTKEY_UP;
    DWORD dwFlags;
    BYTE vkLong;

    if (LKUseMask == 0)
        return dwRet;

    LONGKEY_WAITOBJ(sem_longkey);

    for (i = 0; i < MAX_LONGKEY; i++)
    {
        if (scancode == LongKey[i].scancode
            || vkShort == LongKey[i].vkShort)
        {
            LongKey[i].millisecond = 0;
            dwFlags = LongKey[i].flags;

            LongKey[i].flags &= ~LONGKEY_STATEMASK;
            LongKey[i].flags |= LONGKEY_UNUSED;
            LKUseMask &= ~((DWORD)1 << i);

            if (dwFlags & LONGKEY_DISPACHED)
            {
                vkLong = (BYTE)LongKey[i].vkLong;

                if (dwFlags & LONGKEY_EATSHORT)
                {
                    /* must a long up */
                    ASSERT((dwFlags & LONGKEY_EATLONGUP) == 0);
                    dwRet = SHORTKEY_NONE;
                }

                if ((dwFlags & LONGKEY_EATLONGUP) == 0)
                {
                    LONGKEY_RELEASEOBJ(sem_longkey);
                    PostKeyEvent((BYTE)(scancode | 0x80), vkLong, 0);
                    LONGKEY_WAITOBJ(sem_longkey);
                }
            }
            else if ((dwFlags & LONGKEY_USED) 
                && (dwFlags & LONGKEY_EATSHORTDOWN))
            {
                /* patch a down event here */
                dwRet |= SHORTKEY_DOWN;
            }
        }
    }

    LONGKEY_RELEASEOBJ(sem_longkey);

    return dwRet;
}

/*********************************************************************\
* Function	   LongKeyTask
* Purpose      长按键Timer过程
* Params	   
* Return	 	   
* Remarks	   
    长按键timer由窗口timer过程定时调用。此时将会出现Timer线程和Event线程
    同时操作LongKey数据结构。此情况出现在按键抬起处理时
**********************************************************************/
DWORD LongKeyTask(UINT uTimerTick)
{
    int i;
    DWORD dwTimeout = MAX_ULONG;

    if (LKUseMask == 0)
        return dwTimeout;

    LONGKEY_WAITOBJ(sem_longkey);

    for (i = 0; i < MAX_LONGKEY; i++)
    {
        /* 第一次或者需要连续发送长按键 */
        if ((LongKey[i].flags & LONGKEY_USED) 
            && ((!(LongKey[i].flags & LONGKEY_DISPACHED))
            || (LongKey[i].flags & LONGKEY_SEQUENCE)))
        {
            /* 需要发长按键消息*/
            if (LongKey[i].millisecond <= uTimerTick)
            {
                LongKey[i].millisecond = LongKey[i].escaped;
                LongKey[i].flags |= LONGKEY_DISPACHED;
                LONGKEY_RELEASEOBJ(sem_longkey);
                PostKeyEvent((BYTE)LongKey[i].scancode, (BYTE)LongKey[i].vkLong, 1);
                LONGKEY_WAITOBJ(sem_longkey);
            }
            else
            {
                LongKey[i].millisecond -= uTimerTick;
            }

            if (LongKey[i].millisecond < dwTimeout)
                dwTimeout = LongKey[i].millisecond;
        }
    }

    LONGKEY_RELEASEOBJ(sem_longkey);

    return dwTimeout;
}
