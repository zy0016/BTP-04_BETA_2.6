/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : application manager $ version 3.0
 *
 * Purpose  : Implements the control of application.
 *            
\**************************************************************************/

//#include    "hp_diag.h"
#include	"string.h"
#include	"window.h"
#include    "malloc.h"

#include	"plx_pdaex.h"
#include	"appmain.h"
#include	"appdirs.h"
#include	"appfile.h"
//#include    "blockapi.h"

/*
*	inline function declare here
*/
PAPPADM		pAppMain;		

static	BOOL	AppMainDestroy( struct appMain * pAdm, int mode );
extern  HWND    GetPhoneLockWnd();
extern  HWND	GetMissedEventWnd(void);
/*
*	global variable declare
*/
static	const	struct appMainVtbl funcVtbl = 
{	//	progman virfunction table
        AppDirsCreate,       
		AppDirsDelete,
        AppDirsMove,
        AppDirsRename,
        
        AppFileOpen,
        AppFileDelete,
        AppFileMove,
        //AppGetIdFromIconName,
        AppFileClose,
        AppFileActiveFromAdm,  
     
        
        AppMainDestroy,
      //  AppToolCreate,
        AppDeskCreate
};
/*
*	module:		AppAdmin
*	perpose:	Construct appAdmin object
*/
BOOL	AppMainConstruct( PAPPADM  pAdm )
{
    pAdm->pVtbl		= &funcVtbl;
    pAdm->pAppList	= NULL;
    pAdm->pDirRoot	= NULL;
    pAdm->pDirCurr	= NULL;
  //  pAdm->hToolWnd	= NULL;
    pAdm->hDeskWnd	= NULL;
	pAdm->hLaunchWnd = NULL;
	pAdm->hQuickMenuhWnd = NULL;
    
    pAdm->pDirsCache = ( DIRSNODE *)malloc( sizeof(DIRSNODE) * MAX_DIRS_NODE ); 
    pAdm->pFileCache = ( APPNODE *)malloc( sizeof(APPNODE) * MAX_FILE_NODE );
    
    if ( !pAdm->pDirsCache || !pAdm->pFileCache )
        return FALSE;
    
    memset ( pAdm->pFileCache, 0, sizeof(APPNODE) * MAX_FILE_NODE );	
    memset ( pAdm->pDirsCache, 0, sizeof(DIRSNODE) * MAX_DIRS_NODE );	
    
    pAppMain = pAdm;
    return TRUE;
};
/********************************************************************
* Function   GetAppMain
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
PAPPADM GetAppMain(void)
{
	return pAppMain;
}

/*
*	module	:	Appmain
*	perpose	:	Destroy Appmain object
*	remark	:	this function is no use;
*/
static	BOOL	AppMainDestroy( struct appMain * pAdm, int mode )
{
    PCURAPPLIST	pFile, pTemp;
    
#ifdef _EMULATE_
    //     CloseFBlockSys();
#endif
    
    if ( !pAdm )
        return FALSE;
       
    pFile = pAdm->pAppList;
    while ( pFile )
    {
        AppFileActive ( pFile, APP_DESTROY, 0, 0 );
        pTemp = pFile->pNext;
        free ( pFile );
        pFile = NULL;
        pFile = pTemp;
    }
 
    AppDirsTreeRelease ( pAdm->pDirRoot, 1 );
    
    if ( pAdm->pDirsCache )
        free ( pAdm->pDirsCache);
    if ( pAdm->pFileCache )
        free ( pAdm->pFileCache );
    return TRUE;
};
/*
**	module:		Appmain
**	perpose:	wait systerm update
*/
void	AppMainWaitUpDate ( void )
{
    MSG		msg;
    INT		iID;
    HWND	hDeskwnd;
    
    hDeskwnd = GetDesktopWindow ();	//	this window show not have timer
    if ( !hDeskwnd )
        return;
    
    iID = SetTimer( hDeskwnd, 0, 0, NULL );
    
    while ( GetMessage(&msg, NULL, 0, 0) )
    {
        if( msg.message == WM_TIMER && msg.hwnd == hDeskwnd )
            break;			
        
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }	
    KillTimer( hDeskwnd, iID );
}

/*
**	fileover	:
**	leave word	:	if you find some shortcoming, please modify it.
*/
