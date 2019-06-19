/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : imm.h
 *
 * Purpose  : define input method manager struct and output function
 *            
\**************************************************************************/

#ifndef	HAVE_IMM_H
#define	HAVE_IMM_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************\
*	Purpose:	define macro const   
*	Remarks:	   
*
**************************************************************************/
#ifndef	WIN32
#define	IME_IMAGE_PATH	"/usr/images/\0"
#else
#define	IME_IMAGE_PATH	"\0"
#endif

#define PAGE_XPOS		0	
#define PAGE_YPOS		304	
#define PAGE_WIDTH		128
#define PAGE_HEIGHT		64

/**************************************************************************\
*	Purpose:	define data struct  
*	Remarks:	   
*
**************************************************************************/

#include	"immtype.h"
#include	"ime.h"
#include	"imc.h"

typedef struct imm_s
{
	struct	ime_s **	v_hIme;	//	输入法入口
	struct	imc_s *		v_hImc;	//	缺省控制器
	
	WORD	v_nIme;		// 当前系统存在的输入法数目
	WORD	v_nDefault;	// 缺省输入法id
    
    DWORD	(*f_Notify)(struct imm_s * pThis, INT32 nCode, WPARAM nOne, LPARAM nTwo);
	
}	UIMM, * HIMM, ** LPHIMM;

/**************************************************************************\
*	Purpose:	declare output function  
*	Remarks:	   
*
**************************************************************************/
BOOL	ImmInitialize	( void );
BOOL	ImmRelease		( void );

BOOL	ImmGetDefaultImc( LPHIMC lphImc );
BOOL	ImmGetDefaultIme( LPWORD lpwID );

BOOL	ImmGetImeNumber	( LPWORD pNumber );
BOOL	ImmGetImeIDByName	( LPWORD pID, LPSTR pName );
BOOL	ImmGetImeHandleByID	( WORD nID, LPHIME lphIme );
BOOL	ImmGetImeHandleByHK	( WORD nHotKey, LPHIME lphIme );

#ifdef __cplusplus
}
#endif

#endif
