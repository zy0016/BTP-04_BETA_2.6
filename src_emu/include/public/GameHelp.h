/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : �ؼ�
 *
 * Purpose  : ��Ϸ�����Ŀؼ�
 *            
\**************************************************************************/

#ifndef	_GAMEHELP_
#define	_GAMEHELP_
#include "plxdebug.h"
#include "window.h"
/* GameHelp definition start */
BOOL GAMEHELP_RegisterClass(void*);
#define		PLX_GAMEMAINWND_POS	0,0,176,181
/*GameHelp Control*/
typedef struct tagSTRU_GAMEHELP
{
	HWND	hTextView;
    HWND    hParentWnd;
	PSTR	pHelpText;
	int		nCurLen;			//��ǰ�ĵ�һ���������ı���������0Ϊ����
}STRU_GAMEHELP, *PSTRU_GAMEHELP;


//GameHelp's macro
#define WC_GAMEHELP   "GXSGAMEHELPWNDCLASS"

BOOL	CallGameHelpWnd(HWND hParent, PCSTR HelpText);

#endif	//_GAMEHELP_
