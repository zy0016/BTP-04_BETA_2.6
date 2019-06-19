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

#ifndef	_APPDIRS_H_
#define	_APPDIRS_H_

#define		DIRS_NAME_VIEW		12		

#define		DIRS_SHOW			1
#define		DIRS_ALL			2

#define     DIRS_NODE_VALID      1
#define     DIRS_NODE_OVER       2

#define   MIN_DIR_NODE         36 //36 bytes
#define	  STR_DIRS_MAIN		"Main level"

#define   DEFAULT_DIR_ICON     "/rom/progman/app/mdefaultfolder_43x28.bmp"
#define   DEFAULT_DIRFILE      "/mnt/flash/dir.ini"
#define   DEFAULT_APPFILE      "/mnt/flash/app.ini"
#define   DEFAULT_DLMFILE      "/mnt/flash/dlm.ini"
#define   PMPIC_PATH           "/rom/progman/app/"
#define   PMPICPATHLEN          17

typedef struct appLink{			

	struct	appLink * pNext;			
	short	appId;						

} DIRSAPP, *PDIRSAPP;

typedef	struct	dirsNode{	

	short	fatherId;					//	father dir id
	short   dirId;
	short   iIndex;
	short	flags;						
	char	aDisplayName[PMNODE_NAMELEN + 1];	
	char    aTitleName[PMNODE_NAMELEN + 1];
	char    aIconName[PMICON_NAMELEN];
	short   nDirChildNum;
	WORD    dwstyle;    //0£¬read_only£»1 £¬editable£¨user create£©

} DIRSNODE, *PDIRSNODE;
 
typedef struct DirsLink{		

	struct	DirsLink  * pNext;			
	struct  DirsLink  * pPrev;
	struct	DirsLink  * pFather;				
	struct	DirsLink  * pDirChild;	    
	struct	appMain   * pAdm;			//	poniter progman 
	struct	appLink   * pAppLink;		
			PDIRSNODE   pDirNode;       
	
	short	dirId;						

} DIRSLINK, * PDIRSLINK;

/*
**	struct type define here	
*/
typedef struct ItemInfotag{			

	char	aDisplayName[PMNODE_NAMELEN + 1];
	char	aTitleName[PMNODE_NAMELEN + 1];
	char    aIconName[PMICON_NAMELEN];
	
	short   pPrevId;      
	short 	pNextId;       
	short	pChildDirId;  
	short   pChildAppId;  
		
	short	fatherId;	   
	short	nodeId;       
    short   iIndex;       
	short   nChild;     //dirnode chidle number
	short	IsOpen;     
	short	flags;       //app or dir
	short   attrib;     //dir new_built or default ; app static or dynamic

} ITEMINFO, *PITEMINFO;
/*
*	extern function declare here
*/

extern	BOOL	AppFileRelease (  struct appMain * pAdm, short fileID );

/*
*	extern variable declare here
*/

extern const BYTE dfDirsIcon[PMICON_NAMELEN];
extern const BYTE dfDirsChildIcon[PMICON_NAMELEN];
/*
*	output function declare here
*/

int			AppDirsGetFileChilds ( struct DirsLink * pDir, int flags );
int			AppDirsGetDirsChilds (struct DirsLink * pDir, int flags );
short		AppDirsGetNodeID ( struct appMain * pAdm, PCSTR pPathName);
BOOL		AppDirsTreeRelease ( PDIRSLINK pDir, BOOL bSelf );
BOOL		AppDirsSaveFileEx ( struct appMain * pAdm, BOOL bRemove );
BOOL		AppDirsSaveDirsEx ( struct appMain * pAdm, BOOL bRemove );
BOOL		AppDirsSaveFile ( struct appMain * pAdm);
BOOL		AppDirsSaveDirs ( struct appMain * pAdm);
BOOL		AppDirsMoveFileByID ( struct appMain * pAdm, short fileID, PDIRSLINK pDirSrc, PDIRSLINK pDirDst );
BOOL		AppDirsMoveFile ( struct appMain * pAdm, PCSTR pName, PDIRSLINK pDirSrc, PDIRSLINK pDirDst );
BOOL		AppDirsMoveDirs ( struct appMain * pAdm, short dirID, PDIRSLINK pDirSrc, PDIRSLINK pDirDst );
BOOL		AppDirsReleaseFile( struct appMain * pAdm , short dirID, short fileID );

BOOL		AppDirsInitialize ( struct appMain * pAdm );
BOOL		AppDirsDelete ( struct appMain * pAdm,short dirId);
BOOL		AppDirsRemove ( struct appMain * pAdm, PCSTR pPathName);
BOOL		AppDirsRename ( struct appMain * pAdm, PCSTR pNewName, short dirId);
short		AppDirsCreate ( struct appMain * pAdm, short fatherId, short nIndex,short flags, PCSTR pPathName,
						   PCSTR pIconName, PCSTR pTitleName, BOOL bEdit);
PDIRSLINK	AppDirsFind ( PDIRSLINK pDir, short dirID );
BOOL		AppDirsAddFile( struct appMain * pAdm , short dirID, short fileID);
BOOL     AppDirsMove( struct appMain * pAdm, short dirId, short srcIndex, short desIndex,int nCurType);
//PICONINFO	AppDirGetDefaultInfo ( void );

BOOL        AppDirGrpFile(struct appMain * pAdm);
BOOL        AppDirGetRegIme(PAPPADM pAdm);
BOOL        AppImeRegNode(PAPPADM pAdm, PCSTR pName);
BOOL        AppImeReleaseNode(PAPPADM pAdm, PCSTR pName);
void		AppDirsChangeDisplayName ( struct appMain * pAdm ,int iType);
#endif
/*
*	fileover	:
*	leave word	:	if you find some shortcoming, please modify it.
*/
