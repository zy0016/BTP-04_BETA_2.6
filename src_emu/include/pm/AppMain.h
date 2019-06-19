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

#ifndef	_APPMAIN_H_
#define	_APPMAIN_H_

#include "plx_pdaex.h"
/*
*	macro define here
*/

#define		MAX_DIRS_NODE		10//100		//	max dir number, over it and malloc memory
#define		MAX_FILE_NODE		50		//	max app node number


#define     DISPLAY_NAMELEN  8//9
#define     TITLENODE_NAMELEN  15
//#define		DIRS_HIDE			0	
//#define		DIRS_FILEoON			1	
//#define		DIRS_FILEUNDER		2
//#define		DIRS_FILELEFT		3

//#define		TOOL_HIDE			0	
//#define		TOOL_BOTTOM			1		

//#define		SAVE_NAME_VIEW		12		

/*
*	inline struct define here
*/
//notify group window node add
#define  WM_PMNODEADD   WM_USER + 500
#define  WM_PMNODEDEL   WM_USER + 501

struct	dirsNode;
struct	fileNode;
struct	appDirs;
struct appMain;

struct	appMainVtbl {	//progman virtual functions table

	short  ( *CreateDir ) ( struct appMain * pAdm, short fatherId, short nIndex, short flags, PCSTR pPathName, PCSTR pIconName,PCSTR pTitleName,int bEdit);
	BOOL (*DeleteDir) ( struct appMain * pAdm, short dirId );
	BOOL (*MoveDir) ( struct appMain * pAdm, short dirId, short srcIndex, short desIndex, int nCurType );
	BOOL (*RenameDir) ( struct appMain * pAdm, PCSTR pNewName, short dirId );

	BOOL (*OpenFile)  ( struct appMain * pAdm, short fileID, char bShow,BYTE CallType );
	BOOL (*DeleteFile)( HWND hWnd, struct appMain * pAdm, short fileID );
	BOOL (*MoveFile)( struct appMain * pAdm, short fileID ,short  srcIndex,short desIndex, short destdirId);
//	short(*GetFileID )( struct appMain * pAdm, PCSTR pFileName );	
    BOOL (*CloseFile) ( struct appMain * pAdm,  HINSTANCE hInst);
    DWORD(*ActiveFile) ( struct appMain * pAdm , int nCode, DWORD wParam, DWORD lParam );

	BOOL (*Destroy)			( struct appMain * pAdm, int mode ); 
//	BOOL (*CreateToolWnd)	( struct appMain * pAdm );
	BOOL (*CreateDeskWnd)	( struct appMain * pAdm );	
};

typedef struct appMain{		
							
	const	struct	appMainVtbl * pVtbl;	
	struct			appNode * pFileCache;	//	appnode table 
	struct			dirsNode * pDirsCache;	//	dirnode table 
	struct			CurAppList * pAppList;		//	current app

    struct			DirsLink* pDirRoot;		//	root dir poniter
	struct			DirsLink* pDirCurr;		//	current dir pointer

	HWND	        hDeskWnd;				
	//HWND			hToolWnd;				//	GPS area window handle
	HWND			hGrpWnd;               //first menu window handle
	HWND            hIdleWnd;
	HWND            hLaunchWnd;            //application launch
	HWND			hQuickMenuhWnd;        //Quick menu
	HWND            hAppWnd;               //second menu window handle
	int 	        iStyle;	              //unused

} APPADM, * PAPPADM;

/*
**	output function declare
*/
void	AppMainWaitUpDate ( void );
BOOL	AppMainConstruct( PAPPADM  pAdm );

/*
**	extern function declare
*/
extern	BOOL	AppDeskCreate( struct appMain * pAdm );
extern	BOOL	AppGPSCreate( void );

#endif
/*
**	fileover	:
**	leave word	:	if you find some shortcoming, please modify it.
*/
