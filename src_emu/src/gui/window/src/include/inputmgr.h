/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Declare the function of input management.
 *            
\**************************************************************************/

#ifndef __INPUTMGR_H
#define __INPUTMGR_H
void        KeyboardEvent(int byVirtKey, int bScan, DWORD dwFlags, 
                          DWORD dwExtraInfo);
void        MouseEvent(DWORD dwFlags, LONG dx, LONG dy, DWORD dwData, 
                       DWORD dwExtraInfo);
PTHREADINFO INPUTMGR_GetFgThread(void);
BOOL        INPUTMGR_SetFgThread(DWORD dwThreadId);
BOOL        INPUTMGR_SetFgWindow(PWINOBJ pWin);
PWINOBJ     INPUTMGR_GetFgWindow(void);

BOOL    InitLongKey(void);
void    ExitLongKey(void);
DWORD   StartLongKey(WORD scancode, WORD vkShort);
DWORD   EndLongKey(WORD scancode, WORD vkShort);
DWORD    LongKeyTask(UINT uTimerTick);

typedef struct _tagLongKeyMap{
    WORD scancode;      //按键扫描码
    WORD vkShort;       //按键虚键码
    WORD vkLong;        //长按键虚键码
    WORD escaped;       //长按键延迟时间
    DWORD millisecond;   //长按键已经走过时间
    DWORD flags;        //长按键标志
}LONGKEYMAP, *PLONGKEYMAP;

#define MAX_LONGKEY 8

#define LONGKEY_USERMASK     0x0000FFFFL
#define LONGKEY_STATEMASK    0xFFFF0000L
#define LONGKEY_UNUSED       0x00000000L
#define LONGKEY_USED         0x00010000L
#define LONGKEY_DISPACHED    0x00020000L

#define SHORTKEY_NONE   0x00000000L
#define SHORTKEY_DOWN   0x00000001L
#define SHORTKEY_UP     0x00000002L


#endif

