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

#ifndef	_APPFILE_H_
#define	_APPFILE_H_

#include "app.h"
#include "progman.h"
/*
**	macro define here
*/

#define		APP_NODE_VALID		0x0001	//	valid node
#define     APP_NODE_HIDE       0x0002   //hide node such as call application
#define     APP_NODE_DLM        0x0004   //dynamic app
#define     APP_NODE_SHORTCUT   0x0008  //for edit sms, mms and mail
#define     APP_NODE_WAP        0x0010  //for wap shortcut

/*
**	type define here
*/
typedef	struct	appNode{	

	char    aDisplayName[PMNODE_NAMELEN + 1];
	char    aTitleName[PMNODE_NAMELEN + 1];
    char    cIconName[PMICON_NAMELEN];

	short	sFlag;						
	short	nType;				//	0:static 1:dynamic
	short	fatherId;
	short   iIndex;              //  postion in father dir
	short   appId;
	APPCONTROL appcontrol;

} APPNODE, *PAPPNODE;

typedef struct CurAppList{	

	struct	CurAppList * pNext;			 
	struct	appMain * pAdm;				
	void*	hInstance;					

	short	appId;						
	short	flags;						
	short   iIndex;                     //  postion in fahter dir
    short   fatherId;                
	short   happwnd ;                   // app main window
}CURAPPLIST, *PCURAPPLIST;


typedef struct FolderLink{
	
	struct	FolderLink * pNext;			 
	WORD	mId;
	short   nIndex;
	
}FOLDERLINK, *PFOLDERLINK;

/*
**	extern variable declare
*/
extern	const	APPLICATION* AppTable[];	//	static app entry table

/*
**	output function declare here
*/
PCURAPPLIST	AppFileSearchActive( PAPPADM pAdm, short fileID );
//	use to open already opened file
BOOL		AppFileOpenActive( PAPPADM pAdm, PCURAPPLIST pFile, char bShow ,BYTE CallType);
//	use to call program entery
DWORD		AppFileActive ( PCURAPPLIST pFile , int nCode, DWORD wParam, DWORD lParam );
//	open one file
BOOL		AppFileOpen	 ( struct appMain * pAdm, short fileID, char bShow, BYTE CallType);
DWORD		AppFileInitialize ( struct appMain * pAdm );
BOOL	    AppFileHide( struct appMain * pAdm, HINSTANCE hInst );

BOOL		AppFileClose ( PAPPADM pAdm, HINSTANCE hInst );//PAPPFILE pFile

BOOL		AppFileDelete( HWND hWnd, struct appMain * pAdm, short fileID );
BOOL		AppFileRemove( struct appMain * pAdm, short fileID );
BOOL		AppFileCheck( LPCTSTR lpszFileName, long lUserData );
BOOL		AppFileNameTransfer ( struct appMain * pAdm, char * pSrc, char* pDst );

BOOL		AppFileChangeFkey( struct appMain * pAdm, short srcID, short dstID );
short		AppFileCreateFkey( struct appMain * pAdm, short fatherID, short srcID, short flags );	
short		AppFileCreateFkeyIndirect( struct appMain * pAdm, short fatherID, short srcID, short flags, char* pName );

short		AppFileOrgGetID ( struct appMain * pAdm, PCSTR pFileName );	
short		AppFileGetID ( struct appMain * pAdm, PCSTR pFileName );	
BOOL		AppFileRelease( struct appMain * pAdm, short fileID );
BOOL        AppFileRename ( struct appMain * pAdm, PCSTR pNewName, short appId);

//PICONINFO	AppFileGetDefaultInfo ( void );

BOOL		ShowActiveFile ( struct appMain * pAdm, HINSTANCE hInst);//
BOOL	    CallActiveFile( struct appMain * pAppMain, PAPPENTRY hApp );
BOOL        CallAppFile( struct appMain * pAppMain, PAPPENTRY hApp );
void*	ActiveAppFile  ( struct appMain * pAppMain,BOOL bShow, LPCSTR pFileName );
BOOL AppFileMove( struct appMain * pAdm, short fileID ,short  srcIndex,short desIndex, short destdirid);


DWORD	AppFileActiveFromAdm ( struct appMain * pAdm , int nCode, DWORD wParam, DWORD lParam );
void  AppFilesChangeDisplayName ( struct appMain * pAdm ,int iType);
void  AppFilesChangeFocusPic ( struct appMain * pAdm ,int iType);
BOOL PM_FavoriteCall(PSTR pAppName, int nCode, WPARAM wparam, LPARAM lparam);

short AppGetIdFromIconName(struct appMain * pAdm, PCSTR pIconName);

void ExitWapShortcut(char*strIconName);
short  AddWapShortCut(PAPPGROUP pAppInfo);

short GetSMenuDirId(void);
#endif
/*
**	fileover	:
**	leave word	:	if you find some shortcoming, please modify it.
*/
