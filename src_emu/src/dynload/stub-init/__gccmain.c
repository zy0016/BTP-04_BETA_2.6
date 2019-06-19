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

extern int AppControl(int nCode, void* pInstance, int wParam, int lParam);

void __gccmain()
{
	return;
}

void* _start (unsigned int pFuncList, unsigned int nFuncListNum)
{
//	unsigned char *bss;
//	for(bss=__bss_start; bss<=_end ;bss++)
//		*bss=0;
	stub_init((void *)pFuncList, nFuncListNum) ;
	return AppControl;
}
