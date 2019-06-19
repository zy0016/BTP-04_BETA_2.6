/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef __STUB_H__

#define __STUB_H__

typedef struct tagStubTable{
	char *strFuncName;
	void *FuncAddr;
}StubTable;

extern void stub_init(StubTable *pFuncList, int nFuncListNum);
void *stub_find_func_addr(char *strFuncName);

#endif /*__STUB_H__*/