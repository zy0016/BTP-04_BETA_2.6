    /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : application manager $ version 3.0
 *
 * Purpose  : Implements the control of application.irunst
 *            
\**************************************************************************/

#include	"window.h"
#include	"string.h"

#include	"sys/types.h"
#include	"sys/stat.h"
#include	"fcntl.h"
#include	"unistd.h" 

#include    "malloc.h"

#include	"AppMain.h"
#include	"AppFile.h"
#include	"AppDirs.h"
#include    "app.h"
#include    "browser\func.h"
//#include    "PWBE.h"
/*
*	macro define herer
*/

#define		MAX_DEV_NUMS		1		//	file path number
#define		MAX_DEV_NAME		16		//	file path lenth

static	const char fileDevice[MAX_DEV_NUMS][MAX_DEV_NAME] = 
{
    "/mnt/flash/"		//	search for file path
};

static	BYTE		iDevice = 0;		//	driver index

static  HINSTANCE   hSTInst[MAX_FILE_NODE];

		PCURAPPLIST *AppFileGetListHead( struct appMain * pAdm);
		BOOL	AppRunStaticFile ( PAPPADM pAdm, PCSTR pPathName , short fileID, char bShow );
static	BOOL	AppRunDynamicFile( PAPPADM pAdm, PCSTR pPathName , short fileID, char bShow );
static  short   AppFileHinstance(PAPPADM  pAdm, HINSTANCE hInst);
static  short	AppFileCreateNode( struct appMain * pAdm, short flags, PCSTR pPathName );

extern PDIRSLINK GetDirFromId(struct appMain * pAdm, short dirId);
extern PAPPADM	 GetAppMain(void);
extern int  GetDirChildNum(struct appMain * pAdm, short dirId);
/********************************************************************
* Function     AppGetIdFromIconName
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
short AppGetIdFromIconName(struct appMain * pAdm, PCSTR pIconName)
{

	register	short	i;
    PAPPNODE	pNode = NULL;
    
	if(pIconName[0] == 0)
		return -1;
	
    for (i = 0; i < MAX_FILE_NODE; i++)						
    {
        pNode = pAdm->pFileCache+i;
        
		//        if ( pNode->aDisplayName[0] != pFileName[0] )
		//            continue;
		if(pNode->cIconName[0] == 0)
			continue;
		if(!(pNode->sFlag & APP_NODE_VALID))
			continue;
        
        if ( stricmp (pNode->cIconName, pIconName) == 0 )
            return i;
    }
    return -1;
}
/********************************************************************
* Function     AddDlmApp
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
extern HWND GetFMenuWnd(void);
short  AddDlmApp(PAPPGROUP pAppInfo)
{
	PAPPADM pappMain = NULL;
	short     hInst, fileID;
	PAPPNODE pNode;
    PCURAPPLIST	pFile = NULL, pTemp = NULL;

	pappMain = GetAppMain();

	if(pappMain == NULL)
		return -1;

    fileID = AppFileCreateNode(pappMain, 0, pAppInfo->achName);
	
	
	pNode = pappMain->pFileCache+fileID;
	PLXPrintf("\r\n app node id = %d, name=%s", fileID, pNode->aDisplayName);

	strcpy(pNode->aTitleName ,pAppInfo->achTitleName);
				
	pNode->sFlag |= APP_NODE_VALID;
				
	pNode->nType = DLM_APP;
				
	pNode->appId = (short)fileID;
	pNode->fatherId = 0;
	pNode->appcontrol = pAppInfo->AppControl;

	pNode->iIndex = (short)GetDirChildNum(pappMain, pNode->fatherId);
	PLXPrintf("\r\n app node index = %d", pNode->iIndex);
		
	strcpy(pNode->cIconName ,pAppInfo->strIconName);         
				
	hInst = fileID;
	
	AppDirsAddFile( pappMain, pNode->fatherId, fileID);

	PostMessage(pappMain->hGrpWnd, WM_DLMNODEADD, 0, 0);
	if(GetFMenuWnd())
		PostMessage(GetFMenuWnd(), WM_DLMNODEADD, 0, 0);

	AppDirsSaveFile(pappMain);
	AppDirsSaveDirs(pappMain);

	return fileID;

}
/********************************************************************
* Function     RunDlmApp
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
extern void * dlm_LoadModule (unsigned char *fn);//run dlm get appcontrol
BOOL RunDlmApp(char *strIconName)
{
	PCURAPPLIST	pFile = NULL, *pHead = NULL;
	PAPPNODE  pNode =NULL;
	PAPPADM pappMain = NULL;
	short   fileID= - 1;
	
	pappMain = GetAppMain();
	
	fileID = AppGetIdFromIconName(pappMain, strIconName);
	
	if(fileID == -1)
		return FALSE;

	pNode = pappMain->pFileCache+fileID;
	pFile	 = AppFileSearchActive( pappMain, fileID );// run or not

	if(pNode == NULL)
		return FALSE;

	if(pFile != NULL)
	{
		if(pNode->appcontrol != NULL)
			pNode->appcontrol(APP_ACTIVE, pFile, 0, 0);
		return TRUE;
	}

	pNode->appcontrol = (APPCONTROL)dlm_LoadModule (pNode->cIconName);//run dlm get appcontrol

	if(pNode->appcontrol != NULL)
	{
		pNode->appcontrol(APP_INIT, (void*)fileID, 0, 0);  
		
		pHead = AppFileGetListHead( pappMain);
		
		if ( !pHead )
			return FALSE;;
		
		pFile = (PCURAPPLIST)malloc(sizeof(CURAPPLIST));
		
		if ( !pFile )
			return FALSE;
		
		pFile->flags = APP_NODE_DLM |APP_NODE_VALID;
		pFile->fatherId = 0;
		pFile->iIndex = pNode->appId;
		
		pFile->hInstance	= (void*)fileID;
		pFile->appId		= fileID;
		pFile->pAdm			= pappMain;

		pFile->pNext	= *pHead;
		*pHead = pFile;	
		
		PLXPrintf("\r\nbegin  app_active !");
		pNode->appcontrol(APP_ACTIVE, pFile, 0, 0);
		return TRUE;
	}

	return FALSE;
}
/********************************************************************
* Function     DeleteDlmApp
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL DeleteDlmApp(char *strIconName)
{
	PAPPNODE	pNode = NULL, pTempNode = NULL;
	PAPPADM pappMain = NULL;
	PDIRSLINK  pDir = NULL, pTempDir =NULL;
	PDIRSAPP   pAppLink = NULL,p = NULL, pTemp = NULL;
	BOOL       bFind = FALSE;
	short      fileID = -1, fatherID = -1;
	PDIRSNODE  pDirNode = NULL;
    
	pappMain = GetAppMain();

	fileID = AppGetIdFromIconName(pappMain, strIconName);

	if(fileID == -1)
		return FALSE;

    if (  AppFileSearchActive ( pappMain, fileID ) )
        return FALSE;

    pNode = pappMain->pFileCache+fileID;
	fatherID = pNode->fatherId;
    pNode->sFlag &= ~ APP_NODE_VALID;
	PLXPrintf("\r\n app node id = %d, name=%s", fileID, pNode->aDisplayName);

	pDir = AppDirsFind(pappMain->pDirRoot, pNode->fatherId);
	pAppLink = pDir->pAppLink;
	pTempDir = pDir->pDirChild;

	while(pAppLink)
	{
		if(pAppLink->appId == pNode->appId)
		{
			pTemp = pAppLink;

			if(p)
				p->pNext = pAppLink->pNext;
			else
				pDir->pAppLink = pAppLink->pNext;

			free(pTemp);
			pTemp = NULL;
			bFind = TRUE;
			if(p)
				PLXPrintf("\r\n prenode id = %d", p->appId);
			break;
		}
		p = pAppLink;
		pAppLink = pAppLink->pNext;
	}

	pAppLink = pDir->pAppLink;
	while(pAppLink)
	{
		pTempNode = pappMain->pFileCache + pAppLink->appId;
		if(pTempNode->iIndex > pNode->iIndex)
		{
			pTempNode->iIndex--;
		}

		pAppLink = pAppLink->pNext;
	}


	while(pTempDir)
	{
		if(!pTempDir->pDirNode)
			pDirNode = pappMain->pDirsCache + pTempDir->dirId;
		else 
			pDirNode = pTempDir->pDirNode;
		
		if(pDirNode->iIndex > pNode->iIndex)
			pDirNode->iIndex = pDirNode->iIndex - 1;
		
		pTempDir = pTempDir->pNext;
		
	}

	if(!bFind)
		return FALSE;

	if(fatherID == 0)
	{
		PostMessage(pappMain->hGrpWnd, WM_DLMNODEDEL, 0, 0);
		
		if(GetFMenuWnd())
			PostMessage(GetFMenuWnd(), WM_DLMNODEDEL, 0, 0);
	}
	
	AppDirsSaveFile(pappMain);
	AppDirsSaveDirs(pappMain);

	return TRUE;

}
BOOL DeleteSTKAppItem(void)
{
	return DeleteDlmApp("/rom/progman/app/mstk_43x28.bmp");
}
/********************************************************************
* Function     AddWapShortCut
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
short  AddWapShortCut(PAPPGROUP pAppInfo)
{
	PAPPADM pappMain = NULL;
	short     hInst, fileID;
	PAPPNODE pNode;
    PCURAPPLIST	pFile = NULL, pTemp = NULL;

	pappMain = GetAppMain();

	if(pappMain == NULL)
		return -1;

	fileID = AppFileCreateNode(pappMain, 0, pAppInfo->achName);

	if(fileID == -1)
		return -1;
	
	pNode = pappMain->pFileCache+fileID;

	PLXPrintf("\r\n shortcut node id = %d, name=%s", fileID, pNode->aDisplayName);

	strcpy(pNode->aTitleName ,pAppInfo->achTitleName);
				
	pNode->sFlag |= APP_NODE_VALID;
	pNode->sFlag |= APP_NODE_WAP;
				
	pNode->nType = WAP_SHORTCUT;
				
	pNode->appId = (short)fileID;
	pNode->fatherId = 0;
	pNode->appcontrol = pAppInfo->AppControl;

	pNode->iIndex = (short)GetDirChildNum(pappMain, pNode->fatherId);
	PLXPrintf("\r\n app node index = %d", pNode->iIndex);
		
	strcpy(pNode->cIconName ,pAppInfo->strIconName);         
				
	hInst = fileID;
	
	AppDirsAddFile( pappMain, pNode->fatherId, fileID);

	PostMessage(pappMain->hGrpWnd, WM_DLMNODEADD, 0, 0);
	
	if(GetFMenuWnd())
		PostMessage(GetFMenuWnd(), WM_DLMNODEADD, 0, 0) ;

	AppDirsSaveFile(pappMain);

	return fileID;

}
/********************************************************************
* Function     DeleteWapShortcut
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL DeleteWapShortcut(HWND hWnd, short fileID)
{
	PAPPNODE	pNode = NULL, pTempNode = NULL;
	PAPPADM pappMain = NULL;
	PDIRSLINK  pDir = NULL, pTempDir =NULL;
	PDIRSAPP   pAppLink = NULL,p = NULL, pTemp = NULL;
	BOOL       bFind = FALSE;
	PDIRSNODE  pDirNode = NULL;
	short      fatherID = -1;
    
	pappMain = GetAppMain();

	if(fileID == -1)
		return FALSE;

    if (  AppFileSearchActive ( pappMain, fileID ) )
	{
		//add tipswin
        return FALSE;
	}

    pNode = pappMain->pFileCache+fileID;
	fatherID = pNode->fatherId;
    pNode->sFlag &= ~ APP_NODE_VALID;
	PLXPrintf("\r\n app node id = %d, name=%s", fileID, pNode->aDisplayName);

	pDir = AppDirsFind(pappMain->pDirRoot, pNode->fatherId);
	pAppLink = pDir->pAppLink;
	pTempDir = pDir->pDirChild;

	while(pAppLink)
	{
		if(pAppLink->appId == pNode->appId)
		{
			pTemp = pAppLink;

			if(p)
				p->pNext = pAppLink->pNext;
			else
				pDir->pAppLink = pAppLink->pNext;

			remove(pNode->cIconName);

			free(pTemp);
			pTemp = NULL;
			bFind = TRUE;
			if(p)
				PLXPrintf("\r\n prenode id = %d", p->appId);
			break;
		}
		p = pAppLink;
		pAppLink = pAppLink->pNext;
	}

	pAppLink = pDir->pAppLink;
	while(pAppLink)
	{
		pTempNode = pappMain->pFileCache + pAppLink->appId;
		if(pTempNode->iIndex > pNode->iIndex)
		{
			pTempNode->iIndex--;
		}

		pAppLink = pAppLink->pNext;
	}


	while(pTempDir)
	{
		if(!pTempDir->pDirNode)
			pDirNode = pappMain->pDirsCache + pTempDir->dirId;
		else 
			pDirNode = pTempDir->pDirNode;
		
		if(pDirNode->iIndex > pNode->iIndex)
			pDirNode->iIndex = pDirNode->iIndex - 1;
		
		pTempDir = pTempDir->pNext;
		
	}

	if(!bFind)
		return FALSE;

	if(fatherID == 0)
	{
		PostMessage(hWnd, WM_DLMNODEDEL, 0, 0);
		
		PostMessage(pappMain->hGrpWnd, WM_DLMNODEDEL, 0, 0);
	}

	AppDirsSaveFile(pappMain);

	return TRUE;

}
/********************************************************************
* Function     OpenWapShortCut
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL OpenWapShortCut(short fileID)
{
	PCURAPPLIST	pFile = NULL, *pHead = NULL;
	PAPPNODE  pNode =NULL;
	PAPPADM pappMain = NULL;

	pappMain = GetAppMain();
	
	if(fileID == -1)
		return FALSE;

	pNode = pappMain->pFileCache+fileID;
	pFile	 = AppFileSearchActive( pappMain, fileID );// run or not

	if(pNode == NULL)
		return FALSE;

	if(pFile != NULL)
	{
		App_WapRequestUrlFromFile(pNode->cIconName);//call goto url
		return TRUE;
	}
	if(pNode->cIconName[0] !=0)	
	{
		App_WapRequestUrlFromFile(pNode->cIconName);//call goto url

		pHead = AppFileGetListHead( pappMain);
		
		if ( !pHead )
			return FALSE;;
		
		pFile = (PCURAPPLIST)malloc(sizeof(CURAPPLIST));
		
		if ( !pFile )
			return FALSE;
		
		pFile->flags = APP_NODE_VALID|APP_NODE_WAP;
		pFile->fatherId = 0;
		pFile->iIndex = pNode->appId;
		
		pFile->hInstance	= (void*)fileID;
		pFile->appId		= fileID;
		pFile->pAdm			= pappMain;

		pFile->pNext	= *pHead;
		*pHead = pFile;	
	}

	return TRUE;
}
/********************************************************************
* Function     ExitWapShortcut
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
void ExitWapShortcut(char*strIconName)
{
	PAPPADM pappMain = NULL;
	short      fileID = -1;
    
	pappMain = GetAppMain();
	
	fileID = AppGetIdFromIconName(pappMain, strIconName);

	if(fileID < 0)
		return;

	DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)fileID);
}
/********************************************************************
* Function     AppFileCreateNode
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static  short	AppFileCreateNode( struct appMain * pAdm, short flags, PCSTR pPathName )
{
    PAPPNODE	pFileNode = NULL;
    short		fileID;
    
    pFileNode = pAdm->pFileCache;
    
    for ( fileID = 0; fileID < MAX_FILE_NODE; fileID ++, pFileNode ++ )
    {
        if (!(pFileNode->sFlag & APP_NODE_VALID))		//	when the  node idlesse the flags == 0
            break;
    }
    
    if ( fileID >= MAX_FILE_NODE )
        return -1;

	memset(pFileNode, 0, sizeof(APPNODE));
    
    flags &= ~APP_NODE_VALID;
    pFileNode->sFlag = flags;
 
    strcpy( pFileNode->aDisplayName, pPathName);
	
    return fileID;
};

/********************************************************************
* Function     AppGetDynamicFile
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL	AppGetDynamicFile( PAPPADM  pAdm )
{
/*
#ifndef	_EMULATE_

  HANDLE		hf;
  _FIND_DATA	fd;
  char        pPath[20];			
  register    i = 0;	
  
    GetCurrentDirectory( pPath, 20 );		
    
     
      for ( i = 0; i < MAX_DEV_NUMS; i++ )	
      {
      SetCurrentDirectory( fileDevice[i] );
      
        iDevice = i+APP_DEV_NUMS;
        hf = FindFirstFile ( "*.ime", &fd );
        
          while ( hf != INVALID_HANDLE_VALUE )
          {	//	while
          
            
            if ( !AppFileCheck( fd.cFileName, pAdm ) )
            break;
            if ( !FindNextFile( hf, &fd ) ) 
            break;	
            
              }	//	while
              FindClose(hf);
              }
              SetCurrentDirectory(pPath);
              
              for ( i = 0; i < MAX_DEV_NUMS; i++ )	
              {
              SetCurrentDirectory( fileDevice[i] );
              
                iDevice = i+APP_DEV_NUMS;
                hf = FindFirstFile ( "*.dlm", &fd );
                
                  while ( hf != INVALID_HANDLE_VALUE )
                  {	//	while
                  
                    
                    if ( !AppFileCheck( fd.cFileName, pAdm ) )
                    break;
                    if ( !FindNextFile( hf, &fd ) ) 
                    break;	
                    
                      }	//	while
                      FindClose(hf);
                      }
                      SetCurrentDirectory(pPath);
                      #else
                      
                        iDevice = 1;
                        EnumDlmFiles( AppFileCheck, (long)pAdm );
                        return TRUE;
#endif*/
    return TRUE;
}
/********************************************************************
* Function   AppGetStaticFile 
* Purpose    find static appfile,return file number
* Params     
* Return     
* Remarks      
**********************************************************************/
static	BOOL	AppGetStaticFile( PAPPADM  pAdm )
{
    int				nFile = 0, fileID;
    register	int	i = 0, j = 0;	
    PAPPNODE		pNode = NULL;
    HINSTANCE       hInst;
	int             hFile = -1, hDirFile = -1;
    PITEMINFO        pAppInfo = NULL;
    int             nread, sItemInfo;
	int             nDirNum = 0;
	BOOL            bStk = FALSE, bSimInfo = FALSE, bMailNum = FALSE;

	sItemInfo = sizeof(ITEMINFO);

	hFile = open(DEFAULT_APPFILE,  O_RDONLY);
	if(hFile != -1)
	{
		pAppInfo = malloc(sItemInfo);

		memset(pAppInfo, 0, sItemInfo);

		lseek(hFile, -sItemInfo , SEEK_END);
		nread = read(hFile, pAppInfo, sItemInfo);
        if(nread != sItemInfo || stricmp(pAppInfo->aIconName, "endoffile") != NULL)
		{
			close(hFile);
			remove(DEFAULT_APPFILE);
			hFile = -1;
			hFile = open(DEFAULT_DIRFILE, O_RDONLY);
			if(hFile != -1)
			{
				close(hFile);
				remove(DEFAULT_DIRFILE);
			}
			free(pAppInfo);
			goto FILE_NOSAVE;
		}
		else
		{
			
			hDirFile = open(DEFAULT_DIRFILE, O_RDONLY);

			if(hDirFile != -1)
			{
				memset(pAppInfo, 0, sItemInfo);
				nread = 0;
				lseek(hFile, -sItemInfo , SEEK_END);
				nread = read(hFile, pAppInfo, sItemInfo);
				if(nread != sItemInfo || stricmp(pAppInfo->aIconName, "endoffile") != NULL)
				{
					close(hDirFile);
					remove(DEFAULT_DIRFILE);
					close(hFile);
					remove(DEFAULT_APPFILE);
					free(pAppInfo);
					goto FILE_NOSAVE;
				}
				else
				{
					close(hDirFile);
				}
				
			}
			else
			{
				close(hFile);
				remove(DEFAULT_APPFILE);
				free(pAppInfo);
				goto FILE_NOSAVE;
			}

		}

		memset(pAppInfo, 0, sItemInfo);
		lseek(hFile, 0, SEEK_SET);
		nread = read(hFile, pAppInfo, sItemInfo);
        
		while(nread == sItemInfo && stricmp(pAppInfo->aIconName, "endoffile") != NULL)//read from flash
		{
			 fileID = AppFileCreateNode( pAdm, 0, pAppInfo->aDisplayName );

			 if(stricmp(pAppInfo->aDisplayName, "Toolkit") == NULL)
				 bStk = TRUE;
			 if(stricmp(pAppInfo->aDisplayName, "SIM info") == NULL)
				 bSimInfo = TRUE;
			 if(stricmp(pAppInfo->aDisplayName, "SIM mailbox") == NULL)
				 bMailNum = TRUE;
			 
			 if ( fileID < 0 )
			 {
				 free(pAppInfo);
				 pAppInfo = NULL;
				 close(hFile);
				 return nFile;
			 }

			 pNode = pAdm->pFileCache+fileID;
			 
			 pNode->sFlag = APP_NODE_VALID;
			 if(pAppInfo->attrib & STATIC_APP)
			 {
				 pNode->nType = STATIC_APP;
			 }
			 if(pAppInfo->attrib & HIDE_APP)
			 {
				 pNode->sFlag |= APP_NODE_HIDE;
				 pNode->nType = HIDE_APP;
			 }
			 
			 if(pAppInfo->attrib & DLM_APP)
			 {
				 pNode->sFlag |= APP_NODE_DLM;
				 pNode->nType = DLM_APP;
			 }

			 if(pAppInfo->attrib & WAP_SHORTCUT)
			 {
				 pNode->sFlag |= APP_NODE_WAP;
				 pNode->nType = WAP_SHORTCUT;
			 }

			 pNode->appId = (short)fileID;
			 pNode->iIndex = pAppInfo->iIndex;
			 pNode->fatherId = pAppInfo->fatherId;
			 strcpy(pNode->aTitleName, pAppInfo->aTitleName);
			 strcpy(pNode->cIconName ,pAppInfo->aIconName);

			 if ( nFile >= MAX_FILE_NODE )
			 {
				 free(pAppInfo);
				 pAppInfo = NULL;
				 close(hFile);
				 return nFile;
			 }
			 nFile++;	 
			 
		
			 i = 0;
			 while(GrpTable[i] != NULL)
			 {
				 if(GrpTable[i]->nType == APP_TYPE)
				 {
					 if(strcmp(GrpTable[i]->achName, pNode->aDisplayName) == 0)
					 {
						pNode->appcontrol = GrpTable[i]->AppControl; 
						break;
					 }
					 
				 }
				 else
				 {
					 j = 0;
					 while(GrpTable[i]->AppTable[j])
					 {
						 if(!GrpTable[i]->AppTable[j]->AppControl)
						 {
							 j++;
							 continue;
						 }
						 if(strcmp(GrpTable[i]->AppTable[j]->achName,pNode->aDisplayName) == 0)
						 {
							 pNode->appcontrol = GrpTable[i]->AppTable[j]->AppControl;
							 break;
						 }
						 j++;
						
					 }
				 }
				 i++;
				 
			 }
			 if(pNode->appcontrol != NULL)
			 {
				 hInst	= (void*)fileID;
				 pNode->appcontrol(APP_INIT, (void*)hInst, 0, 0);  
			 }
			 memset(pAppInfo, 0, sItemInfo);
			 nread = read(hFile, pAppInfo, sItemInfo);
			
		}
		close(hFile);
		free(pAppInfo);
		pAppInfo = NULL;
		i = 0;
		while (	GrpTable[i] != NULL	)// hide app like call app(not save in flash) and shortcut(like edit sms...)
		{		
		
			if(GrpTable[i]->nType == APP_TYPE && ((GrpTable[i]->attrib & HIDE_APP) ||
				stricmp(GrpTable[i]->achName, "Toolkit") == NULL||
				stricmp(GrpTable[i]->achName, "SIM info") == NULL ||
				stricmp(GrpTable[i]->achName, "SIM mailbox") == NULL ||(GrpTable[i]->attrib & SHORTCUT) ))
			{			
				
				if(bStk && stricmp(GrpTable[i]->achName, "Toolkit") == NULL)
				{
					i++;
					continue;
				}

				if(bSimInfo && stricmp(GrpTable[i]->achName, "SIM info") == NULL)
				{
					i++;
					continue;
				}

				if(bMailNum && stricmp(GrpTable[i]->achName, "SIM mailbox") == NULL)
				{
					i++;
					continue;
				}
				
				fileID = AppFileCreateNode( pAdm, 0, GrpTable[i]->achName );
				
				if ( fileID < 0 )
					return nFile;
				
				pNode = pAdm->pFileCache+fileID;
								
				pNode->sFlag |= APP_NODE_VALID;
				
				pNode->nType = STATIC_APP;
				
				if(stricmp(GrpTable[i]->achName, "Toolkit") != NULL &&
					stricmp(GrpTable[i]->achName, "SIM info") != NULL &&
					stricmp(GrpTable[i]->achName, "SIM mailbox") != NULL)
				{
					if(GrpTable[i]->attrib & HIDE_APP)
					{
						pNode->sFlag |= APP_NODE_HIDE;
						pNode->nType = HIDE_APP;
					}
					else
					{
						pNode->sFlag |= APP_NODE_SHORTCUT;
						pNode->nType = SHORTCUT;
					}
				}
		
				pNode->appId = (short)fileID;
				pNode->iIndex = (short)0;
				pNode->fatherId = 0;
				
			
				strcpy(pNode->cIconName ,GrpTable[i]->strIconName);
				strcpy(pNode->aTitleName, GrpTable[i]->achTitleName);
				
				if ( nFile >= MAX_FILE_NODE )
					return nFile;
				nFile++;
				
				{                   
					hInst	= (void*)fileID;
					if(GrpTable[i]->AppControl != NULL)
						GrpTable[i]->AppControl(APP_INIT, (void*)hInst, 0, 0);            
				}
				
			}
			else if(GrpTable[i]->nType == DIR_TYPE)
			{
				j =0 ;

				while (GrpTable[i]->AppTable[j])
				{
					if(stricmp(GrpTable[i]->AppTable[j]->achName, "Toolkit") == NULL||
						stricmp(GrpTable[i]->AppTable[j]->achName, "SIM info") == NULL ||
						stricmp(GrpTable[i]->AppTable[j]->achName, "SIM mailbox") == NULL)	
					{
						if(bStk && stricmp(GrpTable[i]->AppTable[j]->achName, "Toolkit") == NULL)
						{
							j++;
							continue;
						}
						
						if(bSimInfo && stricmp(GrpTable[i]->AppTable[j]->achName, "SIM info") == NULL)
						{
							j++;
							continue;
						}
						
						if(bMailNum && stricmp(GrpTable[i]->AppTable[j]->achName, "SIM mailbox") == NULL)
						{
							j++;
							continue;
						}
						
						fileID = AppFileCreateNode( pAdm, 0, GrpTable[i]->AppTable[j]->achName );
						
						if ( fileID < 0 )
							return nFile;
						
						pNode = pAdm->pFileCache+fileID;
											
						pNode->sFlag |= APP_NODE_VALID;
						
						pNode->nType = STATIC_APP;
						
						pNode->appId = (short)fileID;
						pNode->iIndex = (short)0;
						pNode->fatherId = 0;
						
						
						strcpy(pNode->cIconName ,GrpTable[i]->AppTable[j]->strIconName);
						strcpy(pNode->aTitleName, GrpTable[i]->AppTable[j]->achTitleName);
						
						if ( nFile >= MAX_FILE_NODE )
							return nFile;
						nFile++;
						
						{                   
							hInst	= (void*)fileID;
							if(GrpTable[i]->AppTable[j]->AppControl != NULL)
								GrpTable[i]->AppTable[j]->AppControl(APP_INIT, (void*)hInst, 0, 0);            
						}
				
					}
					j++;
				}

			}
			i++;
		}

		return nFile;
	}

FILE_NOSAVE:	

    while (	GrpTable[i] != NULL	) //first read from app.c (not save in flash)
    {		
        j = 0;	
        if(GrpTable[i]->nType == APP_TYPE)
        {
//			if(stricmp(GrpTable[i].achName, "Toolkit") == NULL && GetSIMState() == 0)
//				continue;
                     
            fileID = AppFileCreateNode( pAdm, 0, GrpTable[i]->achName );
         
            if ( fileID < 0 )
                return nFile;
            
            pNode = pAdm->pFileCache+fileID;
			
			pNode->sFlag |= APP_NODE_VALID;
			pNode->nType = STATIC_APP;

			if(GrpTable[i]->attrib & HIDE_APP)
			{
				pNode->sFlag |= APP_NODE_HIDE;
				pNode->nType = HIDE_APP;
			}
			if(GrpTable[i]->attrib & SHORTCUT)
			{
				pNode->sFlag |= APP_NODE_SHORTCUT;
				pNode->nType = SHORTCUT;
			}          
                     
            pNode->appId = (short)fileID;
		    pNode->iIndex = (short)i;
			pNode->fatherId = 0;
            
            strcpy(pNode->cIconName ,GrpTable[i]->strIconName); 
			strcpy(pNode->aTitleName, GrpTable[i]->achTitleName);
                           
            if ( nFile >= MAX_FILE_NODE )
                return nFile;
            nFile++;
                    
			{                   
                hInst	= (void*)fileID;
				if(GrpTable[i]->AppControl != NULL)
					GrpTable[i]->AppControl(APP_INIT, (void*)hInst, 0, 0);            
            }

        }
		else 
		{
			nDirNum++;
			while(GrpTable[i]->AppTable[j] != NULL)
			{			
				
				fileID = AppFileCreateNode( pAdm, 0, GrpTable[i]->AppTable[j]->achName );
				if ( fileID < 0 )
					return nFile;
				
				pNode = pAdm->pFileCache+fileID;
					
				pNode->sFlag |= APP_NODE_VALID;
				pNode->nType = STATIC_APP;

				if(GrpTable[i]->AppTable[j]->attrib & HIDE_APP)
				{
					pNode->sFlag |= APP_NODE_HIDE;
					pNode->nType = HIDE_APP;
				}
				if(GrpTable[i]->AppTable[j]->attrib & SHORTCUT)
				{
					pNode->sFlag |= APP_NODE_SHORTCUT;
					pNode->nType = SHORTCUT;
				}
	
				pNode->fatherId = (short)nDirNum;	
				pNode->appId = fileID;
				pNode->iIndex = j;
				
				strcpy(pNode->cIconName ,GrpTable[i]->AppTable[j]->strIconName);
				strcpy(pNode->aTitleName, GrpTable[i]->AppTable[j]->achTitleName);
				
				if ( nFile >= MAX_FILE_NODE )
					return nFile;	

				nFile++;
												
				{            
			        hInst	= (void*)fileID;
					if(GrpTable[i]->AppTable[j]->AppControl != NULL)
						GrpTable[i]->AppTable[j]->AppControl(APP_INIT, (void*)hInst, 0, 0);            
					j++;
				}
			}
		
				
		}			//	while
		i++;
	}
	return nFile;
}
/*
*	initialize file node 
*/
/********************************************************************
* Function     AppFileInitialize
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
DWORD	AppFileInitialize ( struct appMain * pAdm )
{
     
    AppGetStaticFile( pAdm );
     return AppGetDynamicFile( pAdm );
};
/********************************************************************
* Function   AppFileOpen  
* Purpose    according file ID open assigned file
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppFileOpen	 ( struct appMain * pAdm, short fileID, char bShow, BYTE CallType)
{
#ifndef	_EMULATE_	
    
    int			iDev;
    char		fileName[PMNODE_NAMELEN +1];
    
#endif
    int         iNum = 0;
    PCURAPPLIST	pFile = NULL, pTemp = NULL;
    PAPPNODE	pNode = NULL;
	int         i =0 ;
    
    pFile	 = AppFileSearchActive( pAdm, fileID );// run or not
	
	pNode = pAdm->pFileCache+fileID;

	if(pFile)
		return AppFileOpenActive( pAdm, pFile, bShow, CallType );   
     
    if ( !( pNode->sFlag & APP_NODE_VALID ) )
        return FALSE;

	if(pNode->nType == DLM_APP)
		return RunDlmApp(pNode->cIconName);

	if(pNode->nType == WAP_SHORTCUT)
		return OpenWapShortCut(pNode->appId);

    if ( pNode->nType == STATIC_APP )				
        return AppRunStaticFile( pAdm, pNode->aDisplayName, fileID, bShow );
         
  
    return FALSE;
};
/********************************************************************
* Function   AppFileNameTransfer  
* Purpose    tansfer file node name to original name
* Params     
* Return     
* Remarks      
**********************************************************************/
//BOOL	AppFileNameTransfer ( PAPPADM pAdm , char * pSrc, char* pDst )
//{
//  
//    int iDev;
//    short       fileID;
//    PAPPNODE   pNode = NULL;
//
//
//    fileID = AppFileGetID(pAdm, pSrc);
//
//    if(fileID < 0)
//        return FALSE;
//    
//    pNode = pAdm->pFileCache + fileID;
//    if(pNode == NULL)
//        return FALSE;
//
//    iDev = 0;
//    if(pNode->nType != DLM_APP)
//       sprintf( pDst, "%s%s.dlm", fileDevice[iDev], pSrc );
//    
//   
//    return TRUE;
//};
/********************************************************************
* Function   AppFileHide  
* Purpose    hide current appfile and return ture or false
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppFileHide( struct appMain * pAdm, HINSTANCE hInst )//struct appFile* pFile
{
    PCURAPPLIST	pFind = NULL;
    PCURAPPLIST*	pHead;	
    short       nFileID;
    
    
    if ( !hInst ) 
        return FALSE;
    
    //find appid based on hinstance
    nFileID = AppFileHinstance(pAdm, hInst);
    
    if ( nFileID < 0 || nFileID >= MAX_FILE_NODE )
        return FALSE;
    
    pFind = AppFileSearchActive( pAdm, nFileID );
    pHead = AppFileGetListHead ( pAdm);	
    
    if ( !pFind || pFind != *pHead )	//	not current app
        return FALSE;
    
    if ( !pFind->pNext )	//	havn't other open app	just hide it	
    {
       return AppFileActive( *pHead, APP_INACTIVE, 0, 0 );
    }
    
  return AppFileOpenActive( pAdm, pFind->pNext, TRUE ,0);//	show one and hide one

};
/********************************************************************
* Function     AppFileCreateNode
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL  AppCurFileHide(struct appMain * pAdm)
{
	PCURAPPLIST*	pHead = NULL, pFile = NULL;
	PAPPNODE pNode =NULL;
	int i =0, j = 0;
	
	pHead = AppFileGetListHead ( pAdm);	
	
	pFile = *pHead;
	
	while(pFile)
	{
		pNode = pAdm->pFileCache+ pFile->appId;
		if(pNode->sFlag & APP_NODE_WAP)
		{
			i = 0;
			while(GrpTable[i] != NULL)
			{
				if(GrpTable[i]->nType == APP_TYPE)
				{
					if(strcmp(GrpTable[i]->achName, "WAP") == 0)
					{
						pNode->appcontrol = GrpTable[i]->AppControl; 
						break;
					}
					
				}
				else
				{
					j = 0;
					while(GrpTable[i]->AppTable[j])
					{
						if(!GrpTable[i]->AppTable[j]->AppControl)
						{
							j++;
							continue;
						}
						if(strcmp(GrpTable[i]->AppTable[j]->achName,"WAP") == 0)
						{
							pNode->appcontrol = GrpTable[i]->AppTable[j]->AppControl;
							break;
						}
						j++;
						
					}
				}
				i++;
				
			}
			if(pNode->appcontrol != NULL)
			{
				pNode->appcontrol(APP_INACTIVE, NULL, 0, 0);  
			}
			pFile = pFile->pNext;
			continue;
		}
		
		i = 0;
		while(GrpTable[i] != NULL)
		{
			if(GrpTable[i]->nType == APP_TYPE)
			{
				if(strcmp(GrpTable[i]->achName, pNode->aDisplayName) == 0)
				{
					pNode->appcontrol = GrpTable[i]->AppControl; 
					break;
				}
				
			}
			else
			{
				j = 0;
				while(GrpTable[i]->AppTable[j])
				{
					if(!GrpTable[i]->AppTable[j]->AppControl)
					{
						j++;
						continue;
					}
					if(strcmp(GrpTable[i]->AppTable[j]->achName,pNode->aDisplayName) == 0)
					{
						pNode->appcontrol = GrpTable[i]->AppTable[j]->AppControl;
						break;
					}
					j++;
					
				}
			}
			i++;
			
		}
		if(pNode->appcontrol != NULL)
		{
			pNode->appcontrol(APP_INACTIVE, NULL, 0, 0);  
		}

		pNode = NULL;

		pFile = pFile->pNext;
	}
	
	return TRUE;
}

/********************************************************************
* Function   AppFileClose  
* Purpose    according file handle close assigned file
* Params     
* Return     
* Remarks      
**********************************************************************/
/*
*	module:		AppFile
*	perpose:	according  to file handle close assigned file  
*/
BOOL		AppFileClose ( PAPPADM pAdm,  HINSTANCE hInst)  
{
    PCURAPPLIST	pTemp = NULL, pFind = NULL, pCurr = NULL;
    PCURAPPLIST*	pHead;
    int         nFileID = -1;
    BOOL        bFind = FALSE ;
	PAPPNODE	pAppNode= NULL;
    
//    if(hInst == NULL) 
//        return FALSE;  
      
    if(pAdm->pAppList != NULL)
    {
        pCurr = pAdm->pAppList;
        while(pCurr != NULL)
        {
            if((HINSTANCE)pCurr->hInstance == hInst)
            {
                nFileID = pCurr->appId ;
                bFind = TRUE;
                break;
            }
            pCurr = pCurr->pNext ;
        }     
        
    }
    
    pFind = AppFileSearchActive( pAdm, (short)nFileID);
    if ( !pFind )
        return FALSE;
    
    pHead = AppFileGetListHead( pAdm);
    pTemp = *pHead;
    if ( !pTemp )
        return FALSE;
    
    if ( pTemp == pFind )	//	the file is on the top
    {
        *pHead = pFind->pNext;	
     }
    else
    {
        while ( pTemp->pNext && pTemp->pNext != pFind )
            pTemp = pTemp->pNext;
        if ( pTemp->pNext == pFind )
            pTemp->pNext = pFind->pNext;
    }
    
    free(pFind);
    pFind = NULL;
   
	pAppNode = pAdm->pFileCache + nFileID;
	if(pAppNode != NULL && pAppNode->nType & DLM_APP)
	{
		PLXPrintf("\r\n post message  PM_DLMEXIT !");
		PostMessage(pAdm->hDeskWnd, PM_DLMEXIT, nFileID, 0);
	}

    return TRUE;
};
/********************************************************************
* Function   AppFileDelete  
* Purpose    according file name delete assigned file
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppFileDelete( HWND hWnd, struct appMain * pAdm, short fileID )
{
    PAPPNODE	pNode = NULL;
    
    if (  AppFileSearchActive ( pAdm, fileID ) )
        return FALSE;
    
    pNode = pAdm->pFileCache+fileID;
    pNode->sFlag &= ~ APP_NODE_VALID;

	if(pNode->nType & DLM_APP)
		DeleteDlmApp(pNode->cIconName);
	else
	if(pNode->nType & WAP_SHORTCUT)
		DeleteWapShortcut(hWnd,pNode->appId);
    return TRUE;
};
/********************************************************************
* Function   AppFileRemove  
* Purpose    according file name remove assigned file
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppFileRemove( struct appMain * pAdm, short fileID )
{
    PAPPNODE	pNode = NULL;
    
    if (  AppFileSearchActive ( pAdm, fileID ) )
        return FALSE;
    
    pNode = pAdm->pFileCache+fileID;
    
    if ( !AppDirsReleaseFile( pAdm, pNode->fatherId, fileID ) )
        return AppFileRelease ( pAdm, fileID );
    
    return TRUE;
};
/********************************************************************
* Function   AppFileRelease  
* Purpose    according file name release file info
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppFileRelease( struct appMain * pAdm, short fileID )
{
    PAPPNODE	pFile = NULL;
    
    pFile = pAdm->pFileCache+fileID;
    
    
    pFile->sFlag = 0;
       
    return TRUE;	
};
/********************************************************************
* Function   AppFileGetID  
* Purpose    get file id according to file's tansfer name
* Params     
* Return     
* Remarks      
**********************************************************************/
short	AppFileGetID ( struct appMain * pAdm, PCSTR pFileName )
{
    register	short	i;
    PAPPNODE	pNode = NULL;
    
	if(pFileName[0] == 0)
		return -1;

    for (i = 0; i < MAX_FILE_NODE; i++)						
    {
        pNode = pAdm->pFileCache+i;
        
//        if ( pNode->aDisplayName[0] != pFileName[0] )
//            continue;
		if(pNode->aDisplayName[0] == 0)
			continue;
		if(pNode->sFlag & APP_NODE_DLM || pNode->sFlag & APP_NODE_WAP)
			continue;
        
        if ( stricmp (pNode->aDisplayName, pFileName) == 0 )
            return i;
    }
    return -1;
}
/********************************************************************
* Function   AppFileOrgGetID  
* Purpose    get file id according to file's orginal name
* Params     
* Return     
* Remarks      
**********************************************************************/
//short	AppFileOrgGetID ( struct appMain * pAdm, PCSTR pFileName )
//{
//    char	fileName[PMNODE_NAMELEN + 1], *UpperName;
//    int		nLen, nDev = 0;
//    register int i = 0;
//    
//    memset(fileName,0x00,PMNODE_NAMELEN + 1);
//
//	if(pFileName == NULL)
//        return -1;
//    
//    if ( pFileName[0] != 0)
//    {
//        strncpy( fileName, pFileName, strlen(pFileName));
//        return AppFileGetID ( pAdm, fileName );
//    }
//    
//    nLen = strlen ( pFileName );	
//    
//       
//    UpperName = strupr(fileName);
//    
//    for ( i = 0; i < MAX_DEV_NUMS; i++ ) 
//    {
//        if ( strcmp(UpperName, fileDevice[i] ) == 0 )
//        {
//         
//            strncpy( fileName, pFileName, PMNODE_NAMELEN);
//            
//            fileName[ nLen-nDev ] = '\0';
//            
//            return AppFileGetID( pAdm, fileName );
//        }
//    }
//    return -1;
//};	
/********************************************************************
* Function   AppFileSearchActive  
* Purpose    search open appFile
* Params     
* Return     
* Remarks      
**********************************************************************/
PCURAPPLIST	AppFileSearchActive( PAPPADM  pAdm, short fileID )
{
    PCURAPPLIST*	pHead;
    PCURAPPLIST	pFile = NULL;
    
    pHead = AppFileGetListHead( pAdm);
    if ( !pHead )
        return NULL;
    
    pFile = *pHead;
    if ( !pFile )
        return NULL;
    
    while ( pFile->pNext && pFile->appId != fileID )	
        pFile = pFile->pNext;
    
    return ( pFile->appId == fileID ) ? pFile : NULL;
}
/********************************************************************
* Function   AppFileOpenActive  
* Purpose    change open appFile
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL AppActiveHideApp(PCURAPPLIST pFile, BYTE CallType);
BOOL	AppFileOpenActive( PAPPADM pAdm, PCURAPPLIST pFile, char bShow, BYTE CallType )
{
    PCURAPPLIST	pTemp = NULL;
    PCURAPPLIST*	pHead;
    PAPPNODE pNode = NULL;
    
    if ( !pFile )
        return FALSE;
    
    pHead = AppFileGetListHead( pAdm);

    if ( !pHead )
        return FALSE;
    
    pTemp = *pHead;
    if ( !pTemp )
        return FALSE;
    
    if ( !bShow )
        return SetWindowLong( (HWND)pAdm->hDeskWnd, GWL_USERDATA, (LONG)pFile);  

    if ( pTemp == pFile )	//	current app just show it
    {
        pNode = pAdm->pFileCache + pFile->appId;

        if(pNode->sFlag & APP_NODE_DLM)
            return RunDlmApp(pNode->cIconName);
        
        if(pNode->sFlag & APP_NODE_WAP)
            return OpenWapShortCut(pNode->appId);
      
		if(pNode->sFlag & APP_NODE_HIDE)
			return (AppActiveHideApp(pFile, CallType));
		else
			AppFileActive ( pFile, APP_ACTIVE, 0, 0 );
		
		return SetWindowLong( (HWND)pAdm->hDeskWnd, GWL_USERDATA, (LONG)pFile);			
	}
		
    while ( pTemp->pNext && pTemp->pNext != pFile )
        pTemp = pTemp->pNext;
    if ( pTemp->pNext != pFile )
        return FALSE;
    
    pTemp->pNext = pFile->pNext;
	pFile->pNext = *pHead;
    (*pHead) = pFile;
	
	
	if(pFile->flags & APP_NODE_HIDE)
		AppActiveHideApp(pFile, CallType);
	else
		AppFileActive ( pFile, APP_ACTIVE, 0, 0 );

    
    SetWindowLong( (HWND)pAdm->hDeskWnd, GWL_USERDATA, (LONG)pFile );	//	news window will getwindowlong
    return TRUE;
}
/********************************************************************
* Function   AppActiveHideApp  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL AppActiveHideApp(PCURAPPLIST pFile, BYTE CallType)
{
	int i = 0;
	PAPPNODE pNode = NULL;

   if(pFile == NULL)
	   return FALSE;

   pNode = pAppMain->pFileCache + pFile->appId;

   if(pNode == NULL)
	   return FALSE;

	while (	GrpTable[i] != NULL	)
	{		
		if(GrpTable[i]->nType == APP_TYPE && (GrpTable[i]->attrib & HIDE_APP))
		{			
			if(strcmp(pNode->aDisplayName, GrpTable[i]->achName) !=0)
			{
				i++;
				continue;
			}
			else
			{                   
				if(GrpTable[i]->AppControl != NULL)
					GrpTable[i]->AppControl(APP_ACTIVE, pFile, CallType, 0);            
				
				return TRUE;	
			}
			
		}
		i++;
	}
	return FALSE;
}
/********************************************************************
* Function   AppActiveCallApp  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL AppActiveCallApp(BYTE CallType)
{
	short fileID = 0;
	
	fileID = AppGetIdFromIconName(pAppMain, "/rom/progman/app/mcall_43x28.bmp");
    if ( fileID == -1 )
	   return FALSE;
	   
    return AppFileOpen( pAppMain, fileID, TRUE, CallType);
}
/********************************************************************
* Function   AppFileGetListHead  
* Purpose    get open appFile list head 
* Params     
* Return     
* Remarks      
**********************************************************************/
PCURAPPLIST *	AppFileGetListHead( struct appMain * pAdm )
{
    PCURAPPLIST*	pFile;
  	
	pFile = &(pAdm->pAppList);
    
    return pFile;
}
/********************************************************************
* Function   AppFileActive  
* Purpose    active current file
* Params     
* Return     
* Remarks      
**********************************************************************/
DWORD AppFileActive ( PCURAPPLIST pFile , int nCode, DWORD wParam, DWORD lParam )
{ 
	PAPPNODE pNode = NULL;
	int i =0, j = 0;

	pNode = pAppMain->pFileCache+ pFile->appId;

	if(pNode == NULL)
		return 0;

	if(pNode->nType == DLM_APP )
	{
	//	pNode->appcontrol =(APPCONTROL)dlm_LoadModule (pNode->cIconName);
		if(pNode->appcontrol != NULL)
		{
			return (pNode->appcontrol(nCode, pFile, wParam, lParam));  
		} 
	}
	if(pNode->nType == WAP_SHORTCUT)
	{
		if(nCode == APP_DESTROY)
			return 1;
		else
			return OpenWapShortCut(pNode->appId);
	}
	
	i = 0;
	while(GrpTable[i] != NULL)
	{
		if(GrpTable[i]->nType == APP_TYPE)
		{
			if(strcmp(GrpTable[i]->achName, pNode->aDisplayName) == 0)
			{
				pNode->appcontrol = GrpTable[i]->AppControl; 
				break;
			}
			
		}
		else
		{
			j = 0;
			while(GrpTable[i]->AppTable[j])
			{
				if(!GrpTable[i]->AppTable[j]->AppControl)
				{
					j++;
					continue;
				}
				if(strcmp(GrpTable[i]->AppTable[j]->achName,pNode->aDisplayName) == 0)
				{
					pNode->appcontrol = GrpTable[i]->AppTable[j]->AppControl;
					break;
				}
				j++;
				
			}
		}
		i++;
		
	}
	if(pNode->appcontrol != NULL)
	{
		return (pNode->appcontrol(nCode, pFile, wParam, lParam));  
	}

	return 0;
   
}
/********************************************************************
* Function	   CallAppFile
* Purpose         
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
BOOL   CallAppFile( struct appMain * pAppMain, PAPPENTRY hApp )
{
    register	int i = 0, j;
    
    while ( GrpTable[i] != NULL )  
    {		//	while
        j = 0;
        
        if(GrpTable[i]->AppTable != NULL)//application group
        {  
			
			while(GrpTable[i]->AppTable[j] != NULL)
			{
				// find an application by name
				if (strcmp(GrpTable[i]->AppTable[j]->achName, hApp->pAppName) )
				{
					j++;
					continue;
				}
				
				// application has no entry
				if ( !GrpTable[i]->AppTable[j]->AppControl )
					return FALSE;		
				
				// application was found
				GrpTable[i]->AppTable[j]->AppControl (hApp->nCode, NULL, hApp->wParam, hApp->lParam);  
				return TRUE;
			}
		
			// no application was found in this group, we will shift to next group
        }
		else //application node
		{
			if ( (0 == strcmp(GrpTable[i]->achName, hApp->pAppName)) &&
				 GrpTable[i]->AppControl )
			{
				GrpTable[i]->AppControl (hApp->nCode, NULL, hApp->wParam, hApp->lParam);  
				return TRUE;
			}
		}
        i++;
    }	//	while
	return FALSE;   
}
/********************************************************************
* Function	   AppRunStaticFile
* Purpose      Open static appFile    
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
BOOL	AppRunStaticFile( PAPPADM  pAdm, PCSTR pPathName , short fileID, char bShow )
{
    register	int i = 0, j = 0 ,m = 0;
    PCURAPPLIST*	pHead;
    PCURAPPLIST	pFile = NULL, pTemp = NULL;
    PAPPNODE   pNode = NULL;
    
    pHead = AppFileGetListHead( pAdm);
    
    if ( !pHead )
        return FALSE;
    
    while ( GrpTable[i] != NULL )  
    {		//	while
        j = 0;
        
        while(GrpTable[i]->nType == DIR_TYPE && GrpTable[i]->AppTable[j])
        {            
			
			if (strcmp(GrpTable[i]->AppTable[j]->achName, pPathName))
			{  
				j++;
				continue;
			}
			else
			{
				pFile = (PCURAPPLIST)malloc(sizeof(CURAPPLIST));
				if ( !pFile )
					return FALSE;

				memset(pFile, 0, sizeof(CURAPPLIST));
				
				pFile->flags |= APP_NODE_VALID;
				pFile->fatherId = i;//??
				pFile->iIndex = j;
				if(GrpTable[i]->AppTable[j]->attrib & HIDE_APP)
				{
					pFile->flags |= APP_NODE_HIDE;
					pFile->iIndex = 0;
				}
               				
				pFile->hInstance	= (void*)fileID;
				pFile->appId		= fileID;
				pFile->pAdm			= pAdm;
				
						
				if ( bShow )
				{		
					if (GrpTable[i]->AppTable[j]->AppControl == NULL)
					{
						free(pFile);
						return FALSE;
					}
					pFile->pNext	= *pHead;
					*pHead = pFile;		
					pFile->happwnd =NULL;
					AppFileActive ( pFile, APP_ACTIVE, 0, 0 );
				}
				else
				{
					pFile->pNext =	NULL;
					pTemp = *pHead;
					
					if ( !pTemp )
						*pHead = pFile;
					else
					{
						while ( pTemp->pNext )
							pTemp = pTemp->pNext;
						pTemp->pNext = pFile;
					}
				}
				SetWindowLong( (HWND)pAdm->hDeskWnd, GWL_USERDATA, (LONG)pFile );                 
				return TRUE;
			}         
            
        }//while
	    if(GrpTable[i]->AppControl != NULL)
		{
			if (strcmp(GrpTable[i]->achName, pPathName) )
			{  
				i++;
				continue;
			}
			else
			{
				pFile = (PCURAPPLIST)malloc(sizeof(CURAPPLIST));
				if ( !pFile )
					return FALSE;
				
				pFile->flags = APP_NODE_VALID;
				pFile->iIndex = i;
				if(GrpTable[i]->attrib & HIDE_APP)
				{
					pFile->flags |= APP_NODE_HIDE;
					pFile->iIndex = 0;
				}
				pFile->fatherId = 0;
			
				
				pFile->hInstance	= (void*)fileID;
				pFile->appId		= fileID;
				pFile->pAdm			= pAdm;
	
				if ( bShow )
				{		
					if (!GrpTable[i]->AppControl)
					{
						free(pFile);
						return FALSE;
					}
					pFile->pNext	= *pHead;
					*pHead = pFile;	
					
					AppFileActive ( pFile, APP_ACTIVE, 0, 0 );
				}
				else
				{
					pFile->pNext =	NULL;
					pTemp = *pHead;
					
					if ( !pTemp )
						*pHead = pFile;
					else
					{
						while ( pTemp->pNext )
							pTemp = pTemp->pNext;
						pTemp->pNext = pFile;
					}
				}
			
				SetWindowLong( (HWND)pAdm->hDeskWnd, GWL_USERDATA, (LONG)pFile );                 
				return TRUE;
			}         
			
		}
        i++;
    }	//	while
    return FALSE;
}
/********************************************************************
* Function	   AppRunDynamicFile
* Purpose      Open dynamic appFile   
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
static	BOOL	AppRunDynamicFile( PAPPADM  pAdm, PCSTR pPathName , short fileID, char bShow )
{	
    return TRUE;
};
/********************************************************************
* Function	AppFileActiveFromAdm   
* Purpose      
* Params	struct appMain * pAdm :   
* Return	 	   
* Remarks	  
**********************************************************************/
DWORD	AppFileActiveFromAdm ( struct appMain * pAdm , int nCode, DWORD wParam, DWORD lParam )
{
    PCURAPPLIST pFile;
    
    if(pAdm->pAppList == NULL)
        return 0;
    
    pFile = pAdm->pAppList;
  
    if (pFile->fatherId != 0)
    {
        APPLICATION **ChildTable;    
        ChildTable = (APPLICATION **)(GrpTable[pFile->fatherId]->AppTable);
        
        return ChildTable[pFile->iIndex]->AppControl( nCode, pFile, wParam, lParam );
    }
    else
        return GrpTable[pFile->iIndex]->AppControl ( nCode, pFile, wParam, lParam );    
};
/********************************************************************
* Function	   ShowActiveFile
* Purpose      according file ID open assigned active file  
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
BOOL	ShowActiveFile	 ( struct appMain * pAdm,  HINSTANCE hInst)
{
    PCURAPPLIST	pFind = NULL; 
    short       nFileID;
    
    if ( hInst == NULL ) 
        return FALSE;
    
    nFileID = AppFileHinstance(pAdm, hInst);
    
       
    if ( nFileID < 0 || nFileID >= MAX_FILE_NODE )
        return FALSE;
    
    pFind = AppFileSearchActive( pAdm, nFileID );
    
    if ( !pFind )
        return FALSE;
    
    return AppFileOpenActive( pAdm, pFind, TRUE, 0);
}
/********************************************************************
* Function	   ActiveAppFile
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
//void*	ActiveAppFile( struct appMain * pAppMain,BOOL bShow, LPCSTR pFileName )
//{
//    short		fileID;
//    PCURAPPLIST	pFile = NULL;
//    PCURAPPLIST    pFind = NULL;
//    if ( !pFileName )
//        return NULL;
//    
//    fileID = pAppMain->pVtbl->GetFileID( pAppMain, pFileName);	
//    if ( fileID == -1 )
//        return NULL;
// 
//    if ( pAppMain->pVtbl->OpenFile ( pAppMain, fileID, (char)bShow, 0 ) )
//        pFile = (PCURAPPLIST)GetWindowLong( (HWND)pAppMain->hDeskWnd, GWL_USERDATA );
//    else
//        return NULL;
//    
//    return pFile->hInstance;
//}
/********************************************************************
* Function	   CallActiveFile
* Purpose      call one appfile and return
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
BOOL	CallActiveFile( struct appMain * pAppMain,PAPPENTRY hApp )
{
    PCURAPPLIST	pFile = NULL, pFind = NULL;
    HWND        hTemp;
    HINSTANCE   hInst;
    short       nFileID;
    
    if ( !hApp )  
        return FALSE;
    
    hInst = hApp->hApp;
	
//    if(hInst == NULL)
//        return FALSE;
    
    nFileID = AppFileHinstance(pAppMain, hInst);
    if(nFileID < 0 || nFileID > MAX_FILE_NODE)
        return FALSE;
    
    pFind = AppFileSearchActive( pAppMain, nFileID );
    
    if ( !pFind )
        return FALSE;
    
    if(hApp->nCode == APP_EXIT)
    {
        hTemp = (HWND)AppFileActive(pFile, APP_GETOPTION, AS_APPWND, 0);
        if(hTemp == NULL )
            return FALSE;
        DestroyWindow(hTemp);
        return TRUE;
    }
    
   return AppFileActive( pFind, hApp->nCode, hApp->wParam, hApp->lParam);
   
};
/********************************************************************
* Function	   AppFileHinstance
* Purpose      
* Params	   pAppmain
* Return	   
* Remarks	   
**********************************************************************/
static short AppFileHinstance(PAPPADM  pAdm, HINSTANCE hInst)
{
    short nFileID;
    BOOL  bFind = FALSE;
    PCURAPPLIST pCurr = NULL;
    
    if(hInst == NULL)
        return -1;

    if(pAdm->pAppList != NULL)
    {
        pCurr = pAdm->pAppList;
        while(pCurr != NULL)
        {
            if((HINSTANCE)pCurr->hInstance == hInst)
            {
                nFileID = pCurr->appId ;
                bFind = TRUE;
                return nFileID;
            }
            pCurr = pCurr->pNext ;
        }     
       
    }
    return -1;
}
/********************************************************************
* Function   GrpRefreshFile
* Purpose    deal file transfer;such as del a file(*.dlm) or upload a file
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL  GrpRefeshFile(int nType ,PCSTR pName)
{
    HWND  hDeskWnd;
    
    if( nType < 0 )
        return FALSE;
    
    hDeskWnd = GetDesktopWindow();
    SendMessage( hDeskWnd, WM_TRANSFER, 
        (WPARAM)nType, (LPARAM)pName);  
    return TRUE;
}
/********************************************************************
* Function   AppFileMove
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL AppFileMove( struct appMain * pAdm, short fileID ,short  srcIndex,short destIndex, short destDirId)
{
    PAPPNODE  pAppNode = NULL, pTempAppNode = NULL;
	PDIRSNODE pDirNode = NULL;
	struct DirsLink  *pNewFatherDir = NULL, *pOldFatherDir = NULL, *pTempDir = NULL;
	struct appLink *pTemp = NULL, *pAppLink = NULL,*p = NULL;
	int     nChild = 0;
	BOOL    bForward = TRUE , bSet = FALSE;
	
	pAppNode = pAdm->pFileCache + fileID;

	PLXPrintf("\r\n move the app name = %s from %d to %d \r\n",pAppNode->aDisplayName,
		srcIndex, destIndex);

	if(srcIndex < 0 || destIndex < 0)
		return FALSE;
	
	if(destIndex == srcIndex && destDirId == pAppNode->fatherId)
		return FALSE;

	if(destIndex > srcIndex)
		bForward = FALSE; 

	if(fileID < 0)
		return FALSE;

	if(pAppNode->fatherId == destDirId)//in the same level dir move
	{
		pTempDir = AppDirsFind(pAdm->pDirRoot, pAppNode->fatherId);
		pTemp = pTempDir->pAppLink;
		while(pTemp)
		{
			
			if(pTemp->appId == fileID)
			{
				pAppLink =  pTemp;
				if(p)
					p->pNext = pTemp->pNext;
				else
					pTempDir->pAppLink = pTemp->pNext;
				
				pAppLink->pNext = NULL;

				break;
			}
			p = pTemp;
			pTemp = pTemp->pNext;
		}

		p = NULL;
		pTemp = pTempDir->pAppLink;
		while(pTemp)
		{
			
			pTempAppNode = pAdm->pFileCache + pTemp->appId;

			if(!bForward && pTempAppNode->iIndex >srcIndex && pTempAppNode->iIndex <= destIndex)
			{
				if(!bSet && pTempAppNode->iIndex == destIndex)
				{
					pAppLink->pNext = pTemp->pNext;
					pTemp->pNext = pAppLink;
                    bSet = TRUE;
				}
				pTempAppNode->iIndex = pTempAppNode->iIndex -1;
			}
			if(bForward && pTempAppNode->iIndex >= destIndex && pTempAppNode->iIndex < srcIndex)
			{
				if(!bSet && pTempAppNode->iIndex == destIndex)
				{
					if(p)
						p->pNext = pAppLink;
					else
						pTempDir->pAppLink = pAppLink;
					if(pAppLink)
						pAppLink->pNext = pTemp;
					bSet = TRUE;
				}
				pTempAppNode->iIndex = pTempAppNode->iIndex + 1;
			
			}
			p = pTemp;
			pTemp = pTemp->pNext;
		}

		pTempDir = pTempDir->pDirChild;
		while(pTempDir)
		{
			if(!pTempDir->pDirNode)
				pDirNode = pAdm->pDirsCache + pTempDir->dirId;
			else 
				pDirNode = pTempDir->pDirNode;

			if(!bForward && pDirNode->iIndex >srcIndex && pDirNode->iIndex <= destIndex)
				pDirNode->iIndex = pDirNode->iIndex - 1;
			if(bForward && pDirNode->iIndex >=destIndex && pDirNode->iIndex < srcIndex)
				pDirNode->iIndex = pDirNode->iIndex + 1;	
			pTempDir = pTempDir->pNext;
		}

		pAppNode->iIndex = destIndex;
		
		if(!bSet)
		{	
			p = NULL;
			pTempDir = AppDirsFind(pAdm->pDirRoot, pAppNode->fatherId);
			pTemp = pTempDir->pAppLink;

			while(pTemp)
			{
				pTempAppNode = pAdm->pFileCache + pTemp->appId;
				
				if(pTempAppNode->iIndex >destIndex)
				{
					if(p)
						p->pNext = pAppLink;
					else
						pTempDir->pAppLink = pAppLink;
					if(pAppLink)
						pAppLink->pNext = pTemp;

					bSet = TRUE;
					break;
				}
				p = pTemp;
				pTemp = pTemp->pNext;
			}
			if(!bSet)
			{
				if(p)
					p->pNext = pAppLink;
				else
					pTempDir->pAppLink = pAppLink;
			}
		}
		AppDirsSaveFileEx(pAdm, FALSE);
		AppDirsSaveDirsEx(pAdm, FALSE);

		return TRUE;

	}

	//move between deferent dir

	pOldFatherDir = GetDirFromId(pAdm, pAppNode->fatherId);

	pTemp = pOldFatherDir->pAppLink;//remove from old dir and change  app and dir index
	
	while(pTemp)
	{
		if(pTemp->appId == fileID)
		{
			if(p)
				p->pNext = pTemp->pNext;		
			else
				pOldFatherDir->pAppLink = pTemp->pNext;
			pAppLink = pTemp;
			pAppLink->pNext = NULL;
			break;
		}
		p = pTemp;
		pTemp = pTemp->pNext;
	}

	pTemp = pOldFatherDir->pAppLink;//remove from old dir and change  app and dir index

	while(pTemp)
	{
		pTempAppNode = pAdm->pFileCache + pTemp->appId;
		
		if(pTempAppNode->iIndex > pAppNode->iIndex)
			pTempAppNode->iIndex = pTempAppNode->iIndex - 1; 
	
		pTemp = pTemp->pNext;
	}

	pTempDir = pOldFatherDir->pDirChild;

	while(pTempDir)
	{
	
		if(!pTempDir->pDirNode)
			pDirNode = pAdm->pDirsCache + pTempDir->dirId;
		else 
			pDirNode = pTempDir->pDirNode;

		if(pDirNode->iIndex > pAppNode->iIndex)
			pDirNode->iIndex = pDirNode->iIndex - 1;
	
		pTempDir = pTempDir->pNext;
	}

	pAppNode->fatherId = destDirId;

	pNewFatherDir = GetDirFromId(pAdm, destDirId);

	p =  NULL;
	pTemp = pNewFatherDir->pAppLink;
	while(pTemp)
	{
		p = pTemp;
		pTemp = pTemp->pNext;
		nChild ++;
	}
	if(destDirId == 0) // move from subdir to rootdir
	{
		pTempDir = pNewFatherDir->pDirChild;
		while(pTempDir)
		{
			pTempDir = pTempDir->pNext;
			nChild++;
		}
	}

	pTemp = p;
	if(!pTemp)//if new folder
		pNewFatherDir->pAppLink = pAppLink;
	else
		pTemp->pNext = pAppLink;
	pAppNode->iIndex = nChild;
	
	AppDirsSaveFileEx(pAdm, FALSE);
    AppDirsSaveDirsEx(pAdm, FALSE);
	

	return TRUE;
}
/********************************************************************
* Function   AppFileMove
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL AppFileMoveEx( struct appMain * pAdm, short fileID ,short  srcIndex,short destIndex)
{
    PAPPNODE  pAppNode = NULL, pTempAppNode = NULL;
	PDIRSNODE pDirNode = NULL;
	struct DirsLink  *pNewFatherDir = NULL, *pOldFatherDir = NULL, *pTempDir = NULL;
	struct appLink *pTemp = NULL, *pAppLink = NULL,*p = NULL;
	int     nChild = 0;
	BOOL    bForward = TRUE , bSet = FALSE;
	
	pAppNode = pAdm->pFileCache + fileID;

	PLXPrintf("\r\n move the app name = %s from %d to %d \r\n",pAppNode->aDisplayName,
		srcIndex, destIndex);

	if(srcIndex < 0 || destIndex < 0)
		return FALSE;
	
	if(destIndex == srcIndex )
		return FALSE;

	if(destIndex > srcIndex)
		bForward = FALSE; 

	if(fileID < 0)
		return FALSE;

	pTempDir = AppDirsFind(pAdm->pDirRoot, pAppNode->fatherId);

	pTemp = pTempDir->pAppLink;
	while(pTemp)
	{
		pTempAppNode = pAdm->pFileCache + pTemp->appId;
		
		if(!bForward && pTempAppNode->iIndex >srcIndex && pTempAppNode->iIndex <= destIndex)
		{
			pTempAppNode->iIndex = pTempAppNode->iIndex -1;
		}
		if(bForward && pTempAppNode->iIndex >= destIndex && pTempAppNode->iIndex < srcIndex)
		{
			pTempAppNode->iIndex = pTempAppNode->iIndex + 1;
		}
		p = pTemp;
		pTemp = pTemp->pNext;
	}
	
	pTempDir = pTempDir->pDirChild;
	while(pTempDir)
	{
		if(!pTempDir->pDirNode)
			pDirNode = pAdm->pDirsCache + pTempDir->dirId;
		else 
			pDirNode = pTempDir->pDirNode;
		
		if(!bForward && pDirNode->iIndex >srcIndex && pDirNode->iIndex <= destIndex)
			pDirNode->iIndex = pDirNode->iIndex - 1;
		if(bForward && pDirNode->iIndex >=destIndex && pDirNode->iIndex < srcIndex)
			pDirNode->iIndex = pDirNode->iIndex + 1;	
		pTempDir = pTempDir->pNext;
	}
	
	pAppNode->iIndex = destIndex;
	

	return TRUE;

}
/********************************************************************
* Function   AppFileRename  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL AppFileRename ( struct appMain * pAdm, PCSTR pNewName, short appId)
{
	PAPPNODE pAppNode = NULL;

	if(appId < 0)
		return FALSE;

	if(pNewName == NULL)
		return FALSE;

	pAppNode = pAdm->pFileCache + appId;

	if(pAppNode && (/*pAppNode->nType & DLM_APP || */pAppNode->nType & WAP_SHORTCUT))
	{
		if(stricmp(pAppNode->aDisplayName, pNewName) == NULL)
			return FALSE;

		strcpy(pAppNode->aDisplayName, pNewName);
		strcpy(pAppNode->aTitleName, pNewName);

		AppDirsSaveFileEx(pAdm,FALSE);
		
		return TRUE;
	}

	return FALSE;
}
/********************************************************************
* Function   PM_RegisteApp  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
//BOOL PM_RegisteApp(char * strAppName)
//{
//	PCURAPPLIST	pFile = NULL, *pHead = NULL;
//	PAPPNODE  pNode =NULL;
//	PAPPADM pappMain = NULL;
//	short fileID = 0;
//
//	pappMain = GetAppMain();
//
//	pHead = AppFileGetListHead( pappMain);
//	
//	if ( !pHead )
//		return FALSE;;
//	
//	pFile = (PCURAPPLIST)malloc(sizeof(CURAPPLIST));
//	
//	if ( !pFile )
//		return FALSE;
//
//	fileID = AppFileGetID(pappMain, strAppName);
//
//	if(fileID < 0)
//		return FALSE;
//
//	pNode = pappMain->pFileCache + fileID;
//
//	if(!pNode)
//		return FALSE;
//	
//	pFile->flags = APP_NODE_VALID;
//	pFile->fatherId = pNode->fatherId;
//
//	pFile->iIndex = pNode->iIndex;
//	
//	pFile->hInstance	= (void*)fileID;
//	pFile->appId		= fileID;
//	pFile->pAdm			= pappMain;
//	
//	pFile->pNext	= *pHead;
//	*pHead = pFile;
//	
//	return TRUE;
//
//}
/********************************************************************
* Function   NotifyAppFile  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL NotifyAppFile(int nCode)
{
	PCURAPPLIST*	pHead = NULL, pFile = NULL;
	PAPPNODE pNode =NULL;
	int i =0, j = 0;
	PAPPADM pappMain = NULL;


	pappMain = GetAppMain();

	pHead = AppFileGetListHead ( pappMain);	
	
	pFile = *pHead;
	
	while(pFile)
	{
		pNode = pappMain->pFileCache+ pFile->appId;
		
		i = 0;
		while(GrpTable[i] != NULL)
		{
			if(GrpTable[i]->nType == APP_TYPE)
			{
				if(strcmp(GrpTable[i]->achName, pNode->aDisplayName) == 0)
				{
					pNode->appcontrol = GrpTable[i]->AppControl; 
					break;
				}
				
			}
			else
			{
				j = 0;
				while(GrpTable[i]->AppTable[j])
				{
					if(!GrpTable[i]->AppTable[j]->AppControl)
					{
						j++;
						continue;
					}
					if(strcmp(GrpTable[i]->AppTable[j]->achName,pNode->aDisplayName) == 0)
					{
						pNode->appcontrol = GrpTable[i]->AppTable[j]->AppControl;
						break;
					}
					j++;
					
				}
			}
			i++;
			
		}
		if(pNode->appcontrol != NULL)
		{
			pNode->appcontrol(nCode, NULL, 0, 0);  
		}
		
		pNode = NULL;
		
		pFile = pFile->pNext;
	}

	return TRUE;
}
/********************************************************************
* Function   PM_FavoriteCall  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL PM_FavoriteCall(PSTR pAppName, int nCode, WPARAM wparam, LPARAM lparam)
{
	PCURAPPLIST pFile = NULL, *pHead = NULL, pFind = NULL;
	PAPPNODE    pNode =NULL;
	short       appId = -1;
	int i = 0, j = 0;
	BOOL    bForWap = FALSE;
	
	pHead = AppFileGetListHead(pAppMain);
	
	if ( !pHead )
		return FALSE;

	appId = AppFileGetID(pAppMain, pAppName);

	if(appId < 0)
		return FALSE;

	if(stricmp(pAppName, "WAP") == NULL && wparam == SHORTCUTFORWAP)
		bForWap = TRUE;

	if(!bForWap)
	{
		pFind = AppFileSearchActive( pAppMain, appId );
		
		if(pFind != NULL)
		{
			AppFileActive( pFind, nCode, wparam, lparam);
			return TRUE;
		}
	}

	pNode = pAppMain->pFileCache + appId;

	if(!pNode)
		return FALSE;
	
	while(GrpTable[i] != NULL)
	{
		if(GrpTable[i]->nType == APP_TYPE)
		{
			if(stricmp(GrpTable[i]->achName, pNode->aDisplayName) == 0)
			{
				pNode->appcontrol = GrpTable[i]->AppControl; 
				break;
			}
			
		}
		else
		{
			j = 0;
			while(GrpTable[i]->AppTable[j])
			{
				if(!GrpTable[i]->AppTable[j]->AppControl)
				{
					j++;
					continue;
				}
				if(stricmp(GrpTable[i]->AppTable[j]->achName,pNode->aDisplayName) == 0)
				{
					pNode->appcontrol = GrpTable[i]->AppTable[j]->AppControl;
					break;
				}
				j++;
				
			}
		}
		i++;
		
	}

	if(pNode->appcontrol != NULL)
	{
		if(bForWap)
		{
			pNode->appcontrol(nCode, pFile, wparam, lparam);
			return TRUE;
		}
		pFile = (PCURAPPLIST)malloc(sizeof(CURAPPLIST));

		if ( !pFile )
			return FALSE;

		pFile->flags = pNode->sFlag;
		pFile->fatherId = pNode->fatherId;;
		pFile->iIndex = pNode->iIndex;
		
		pFile->hInstance	= (void*)appId;
		pFile->appId		= appId;
		pFile->pAdm		= pAppMain;
		
		pFile->pNext	= *pHead;
		*pHead = pFile;	

		printf("\r\n PM---------------->enter appname %s, wparam = %x, lparam = %x", 
			pAppName, wparam, lparam);
		pNode->appcontrol(nCode, pFile, wparam, lparam);
		return TRUE;
	}
	return FALSE;
}
/********************************************************************
* Function   CallAppEntryEx  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL CallAppEntryEx(PSTR pAppName, WPARAM wparam, LPARAM lparam)
{
	return PM_FavoriteCall(pAppName, APP_ACTIVE, wparam, lparam);
}
 
