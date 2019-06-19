/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for hook support module.
 *            
\**************************************************************************/

#ifndef __WSHOOK_H
#define __WSHOOK_H

LRESULT WH_CallHookProc(int nHookId, int nCode, WPARAM wParam, 
                        LPARAM lParam);

#endif // __WSHOOK_H
