/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : StkEx.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _STK_EX_H_
#define _STK_EX_H_

BOOL IfStkMainMenu(void);
//返回TRUE表示在STK主菜单界面，允许回到待机界面；
//返回FALSE表示不在STK主菜单界面，禁止回到待机界面；

BOOL IfStkInitSucc(void);
//返回STK是否初始化完毕，这里的初始化指的是取主菜单信息并给予回应。
//该函数在有SIM卡且支持STK功能的前提下可以调用。
//程序管理器可以在STK初始化完毕前就显示STK图标，也可以在成功初始化之后再SHOW。

void STK_GetMainText(char* pMainText);
//返回STK主界面大标题，供程序管理器使用。

void STK_GetIdleModeText(char* pIdleModeText);
//在收到URC: 40时解析得到的字串。

BOOL CreateStkHideWindow(void);
//创建STK隐藏窗口。

#endif // _STK_EX_H_
