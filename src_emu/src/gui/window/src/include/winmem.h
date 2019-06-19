/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Head file of winmem.c
 *            
\**************************************************************************/

#ifndef __WINMEM_H
#define __WINMEM_H

void*   WinAlloc(int size);
void    WinFree(void* pMem);

BOOL    WINMEM_Init(int type);
void    WINMEM_Exit();


#endif //__WINMEM_H
