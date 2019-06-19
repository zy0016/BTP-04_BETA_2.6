/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : imm.c
 *
 * Purpose  : implement input method manager 
 *            
\**************************************************************************/

#include	"window.h"
#include	"malloc.h"
#include	"string.h"

#include	"imm.h"

/**************************************************************************\
 *	Purpose:	define global variable   
 *	Remarks:	   
 *
 **************************************************************************/
static	HIMM		g_hImm;
static	HHOOK		g_hPen;
static	HHOOK		g_hKey;

static	LRESULT	ImmPenHookProc ( int nCode, WPARAM wParam, LPARAM lParam );
static	LRESULT	ImmKeyHookProc ( int nCode, WPARAM wParam, LPARAM lParam );

/**************************************************************************\
 * Function	ImmInitialize ( void );   
 * Purpose	initialize Input method manager       
 * Params	void			
 * Return	successed return TRUE 	   
 * Remarks	   
 **************************************************************************/
BOOL	ImmInitialize ( void )
{
	RECT rcIme;
	
	register char i = 0;
	
	if ( NULL != g_hImm )
		return	TRUE;

	g_hImm = (HIMM) malloc ( sizeof(UIMM) );
	if ( NULL == g_hImm )
		return	FALSE;

	memset ( g_hImm, 0, sizeof(UIMM) );
	ImcCreate ( &g_hImm->v_hImc );

	if ( !ImeGetEntry ( &g_hImm->v_hIme ) )
		return	ImmRelease ();

    ImeGetLangEntry(&g_hImm->v_hImc->v_ppImeLang);
	//	多线程时不需要设置钩子
	SetRect ( &rcIme, 0, 0, max(PAGE_XPOS + PAGE_WIDTH, 
		GetSystemMetrics ( SM_CXSCREEN ) ),
		max (PAGE_YPOS + PAGE_HEIGHT, GetSystemMetrics( SM_CYSCREEN) ) );

	SetMaxCursorRect ( &rcIme );
	ClipCursor ( NULL );
	g_hPen = SetWindowsHookEx( WH_MOUSE, ImmPenHookProc, 0, 0 );
	if ( NULL == g_hPen )
		return	ImmRelease ();
	
	g_hKey = SetWindowsHookEx( WH_KEYBOARD, ImmKeyHookProc, 0, 0 );
	if ( NULL == g_hKey )
		return	ImmRelease ();

	while ( g_hImm->v_hIme[i] )
	{
		g_hImm->v_hIme[i]->v_nID = i;
		g_hImm->v_hIme[i]->f_Inquire ( 0, IME_OPA_INIT, 0, 0 );		
		i++;
	}
	g_hImm->v_nIme = i;

    i = 0;
    while (g_hImm->v_hImc->v_ppImeLang[i] != NULL)
    {
        i++;
    }
    g_hImm->v_hImc->v_nTotalLang = i;

	return	TRUE;
}

/**************************************************************************\
 * Function	ImmRelease ( void );   
 * Purpose	Release Input method manager       
 * Params	void			
 * Return	   
 * Remarks	   
 **************************************************************************/
BOOL	ImmRelease	( void )
{	
	if ( NULL == g_hImm )
		return	FALSE;

	if ( NULL != g_hImm->v_hImc )
		ImcRelease ( g_hImm->v_hImc );

	free ( g_hImm );
	g_hImm = NULL;

	if ( NULL != g_hPen )
	{
		UnhookWindowsHookEx( g_hPen );
		g_hPen = NULL;
	}

	if ( NULL != g_hKey )
	{
		UnhookWindowsHookEx( g_hKey );
		g_hKey = NULL;
	}	
	return	TRUE;
}

/**************************************************************************\
 * Function	ImmGetDefaultIme	( LPWORD lpwID );
 * Purpose	Get default ime id   
 * Params	lpwID : a point to receive default ime id			
 * Return	successed return TRUE 	   
 * Remarks	   
 **************************************************************************/
BOOL	ImmGetDefaultIme ( LPWORD lpwID )
{
	if ( NULL == lpwID || NULL == g_hImm )
		return	FALSE;

	*lpwID = g_hImm->v_nDefault;

	return	TRUE;
}

/**************************************************************************\
 * Function	ImmGetDefaultImc ( LPHIMC lphImc );
 * Purpose	Get default input method control handle   
 * Params	lphImc : a point to receive default controller			
 * Return	successed return TRUE 	   
 * Remarks	   
 **************************************************************************/
BOOL	ImmGetDefaultImc ( LPHIMC lphImc )
{
	if ( NULL == lphImc || NULL == g_hImm )
		return	FALSE;

    *lphImc = g_hImm->v_hImc;
    if (*lphImc == NULL)
    {
        return FALSE;
    }
	return	TRUE;
}

/**************************************************************************\
 * Function	ImmPenHookProc( int nCode, WPARAM wParam, LPARAM lParam )
 * Purpose	Set Pen Hook;      
 * Params	nCode	:
			wParam	:
			lParam	:
 * Return	 	   
 * Remarks	   
 **************************************************************************/
static LRESULT ImmPenHookProc ( int nCode, WPARAM wParam, LPARAM lParam )
{
	if ( nCode < 0 )  
        return	CallNextHookEx( g_hPen, nCode, wParam, lParam );
	
	if ( ImcMessageProc( NULL, IMC_MSG_PEN, wParam, lParam ) )
		return	TRUE;
	
	return	CallNextHookEx( g_hPen, nCode, wParam, lParam );
}

/**************************************************************************\
 * Function	ImmKeyHookProc( int nCode, WPARAM wParam, LPARAM lParam )
 * Purpose	Set key broad Hook;      
 * Params	nCode	:
			wParam	:
			lParam	:
 * Return	 	   
 * Remarks	   
 **************************************************************************/
static	LRESULT	ImmKeyHookProc ( int nCode, WPARAM wParam, LPARAM lParam )
{
	if ( nCode < 0 )  
        return	CallNextHookEx( g_hKey, nCode, wParam, lParam );

	if ( ImcMessageProc( NULL, IMC_MSG_KEY, wParam, lParam ) )
		return	TRUE;

	return	CallNextHookEx( g_hKey, nCode, wParam, lParam );
}

/**************************************************************************\
 * Function	ImmGetImeNumber	( LPWORD pNumber );
 * Purpose	Get system ime number  
 * Params	pNumber	: a point to receive value
 * Return	successed return TRUE 	   
 * Remarks	   
 **************************************************************************/
BOOL	ImmGetImeNumber	( LPWORD pNumber )
{
	if ( NULL == pNumber || NULL == g_hImm )
		return	FALSE;

    *pNumber = g_hImm->v_nIme;
	return	TRUE;
}

/**************************************************************************\
 * Function	ImmGetImeHandleByID	( WORD nID, LPHIME lphIme );
 * Purpose	Get ime handle by ime id  
 * Params	lphIme	: a point to receive default ime handle
			nID		: ime id
 * Return	successed return TRUE 	   
 * Remarks	   
 **************************************************************************/
BOOL	ImmGetImeHandleByID	( WORD nID, LPHIME lphIme )
{
	if ( nID >= g_hImm->v_nIme || NULL == lphIme )
		return	FALSE;

    *lphIme = g_hImm->v_hIme[nID];
    if (*lphIme == NULL)
    {
        return FALSE;
    }
	return	TRUE;
}

/**************************************************************************\
 * Function	ImmGetImeHandleByHK	( WORD nHotKey, LPHIME lphIme );
 * Purpose	Get ime handle by ime hot key  
 * Params	lphIme	: a point to receive default ime handle
			nHotKey	: ime hot key
 * Return	successed return TRUE 	   
 * Remarks	   
 **************************************************************************/
BOOL	ImmGetImeHandleByHK	( WORD nHotKey, LPHIME lphIme )
{
	register BYTE imeID = 0;
	
	if ( NULL == lphIme || 0 == nHotKey )
		return	FALSE;

	if ( NULL == g_hImm )
		return	FALSE;

	while ( g_hImm->v_hIme[imeID] )
	{
		if ( (g_hImm->v_hIme[imeID]->v_wHotKey & nHotKey) == 
			g_hImm->v_hIme[imeID]->v_wHotKey && 
			(g_hImm->v_hIme[imeID]->v_wHotKey & 0xff) == (nHotKey & 0xff) )
//   		if ( (g_hImm->v_hIme[imeID]->v_wHotKey == nHotKey) )
			break;
		
		imeID++;
	}	

    *lphIme = g_hImm->v_hIme[imeID];
    if (*lphIme == NULL)
    {
        return FALSE;
    }

	return	TRUE;
}

/**************************************************************************\
 * Function	ImmGetImeIDByName	( LPWORD pID, LPSTR pName )
 * Purpose	Get ime ID by ime name  
 * Params	pID		: a pointer to receive ID
			pName	: ime name
 * Return	successed return TRUE 	   
 * Remarks	   
 **************************************************************************/
BOOL	ImmGetImeIDByName	( LPWORD pID, LPSTR pName )
{
	register INT32 i;

	if ( NULL == pID || NULL == pName )
		return	FALSE;

	if ( NULL == g_hImm )
		return	FALSE;

	*pID = 0;

	for ( i = 0; i < g_hImm->v_nIme; i++ )
	if ( 0 == strcmp ( pName, g_hImm->v_hIme[i]->v_aImeName ) )
	{
		*pID = g_hImm->v_hIme[i]->v_nID;

		return	TRUE;
	}
	return	FALSE;
}
