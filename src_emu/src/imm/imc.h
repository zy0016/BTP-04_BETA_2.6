/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : imc.h
 *
 * Purpose  : define input method control struct and output function
 *            
\**************************************************************************/

#ifndef	HAVE_IMC_H
#define	HAVE_IMC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "zi8api.h"

/**************************************************************************\
*	Purpose:	define data struct  
*	Remarks:	   
*
**************************************************************************/
typedef	struct imelink_s
{
	struct	ime_s *		v_hIme;
	struct	imelink_s*	v_hNext;

	PVOID	v_hThis;	//	输入法内部句柄

}	IMELINK, *HIMELINK;

typedef struct imc_s
{
	struct	imelink_s*	v_hShow;
	struct	imelink_s*	v_hHide;

	WORD	v_nHotKey;	//	上次热健
    WORD    v_nKeyType;
    WORD    v_nVirtKey;
	WORD	v_nDown;	//	输入法键按下状态
    WORD    v_nRepeats;
	WORD	v_nLockID;
	WORD	v_nAttrib;
    BOOL    v_bCatchUp;

    UINT    v_nTimerID;
    UINT    v_nTimerNotify;
	DWORD	v_nTicker;

    UINT                v_iLang;
    UINT                v_nTotalLang;
    PPIMELANG           v_ppImeLang;
    PZI8VOID            v_pZiGlobal;

    DWORD	(*f_Notify)(struct imc_s * pThis, INT32 nCode, 
		WPARAM nOne, LPARAM nTwo);//保留参数
	
}	UIMC, * HIMC, ** LPHIMC;

/**************************************************************************\
*	Purpose:	define macro const   
*	Remarks:	
*
**************************************************************************/
#define	IMC_MSG_KEY		0x0001
#define	IMC_MSG_PEN		0x0002

#define	IMC_LOCK_IME		0x0001
#define	IMC_UNLOCK_IME		0x0002
#define IMC_LOCK_SWITCH		0x0003
#define	IMC_UNLOCK_SWITCH	0x0004

#define	IMC_ATR_LOCK		0x0001
#define	IMC_ATR_NOSWITCH	0x0002
#define IMC_ATR_IME_KEYDOWN 0x0004
#define IMC_ATR_WORD        0x0008
#define IMC_ATR_NOWORD      0x0010

/**************************************************************************\
*	Purpose:	declare output function  
*	Remarks:	   
*
**************************************************************************/
BOOL	ImcCreate	( LPHIMC lphImc );
BOOL	ImcRelease	( HIMC hImc );

BOOL	ImcLockIme	( HIMC hImc, UINT nMode );
BOOL	ImcGetImeNumber	( HIMC hImc, BOOL bShow, LPWORD pNumber );
BOOL	ImcShowImeByID	( HIMC hImc, WORD imeID, BOOL bShow );
BOOL    ImcToggleTextInput(HIMC hImc, BOOL bWordMode);
BOOL    ImcGetImeLangState(HIMC hImc, PSTR *ppszIcon);
BOOL    ImcEnableWordInput(HIMC hImc, BOOL bEnable);
BOOL    ImcSetImeLanguage(HIMC hImc);
PZI8VOID ImcGetZiGlobal(void);
BOOL	ImcMessageProc	( HIMC hImc, INT32 nType, WPARAM nOne, LPARAM nTwo );

#ifdef __cplusplus
}
#endif

#endif
