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

#include    "progman.h"
#include	"appfile.h"
#include	"appdirs.h"

#include	"sys/types.h"
#include	"sys/stat.h"
#include	"fcntl.h"
#include	"unistd.h" 

#include	"string.h"
#include    "malloc.h"


static	PDIRSLINK	AppDirsCreateIndirect ( struct appMain * pAdm, PITEMINFO pInfo );
static	PDIRSLINK	AppDirsConstruct ( struct appMain * pAdm, PDIRSLINK pFather, PDIRSNODE pDirNode );
static	PDIRSLINK	AppInsertDirsConstruct( struct appMain * pAdm, PDIRSLINK pFather, PDIRSNODE pDirNode );

static	BOOL		AppDirsDeleteNode ( struct appMain * pAdm, short dirID);
static  PDIRSNODE	AppDirsCreateNode( struct appMain * pAdm, short fatherId,short nIndex,
									  short flags, PCSTR pPathName, PCSTR pTitleName, BOOL bEdit);
PDIRSNODE	AppGetDirNodeFromId(struct appMain *pAdm, short dirId, struct DirsLink *pDir);
extern BOOL GetSimStk(void);

extern BOOL PmHandleTable[MAX_DIRS_NODE];
static int  DirNodeID = MAX_DIRS_NODE;

/********************************************************************
* Function   AppDirsCreateDefault  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static	void	AppDirsCreateDefault ( struct appMain * pAdm )
{
    register  int i = 0;
  
    
    //based on group info defined in the file App.c
    while (	GrpTable[i] != NULL	)
    {
		if(GrpTable[i]->nType == DIR_TYPE)
			AppDirsCreate( pAdm, pAdm->pDirRoot->dirId, (short)i,
            DIRS_NODE_VALID, GrpTable[i]->achName, GrpTable[i]->strIconName, 
			GrpTable[i]->achTitleName,0 );
        
        i++;
    }
    
};
/********************************************************************
* Function   AppDirsSetUp  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static	BOOL	AppDirsSetUp ( struct appMain * pAdm )
{
    register    int nRead;
    int			nDirInfo, nDirNum= 0, i = 0;
    int		    hFile;
    ITEMINFO    dirInfo;

    
    //	create root dir
    if ( AppDirsCreate( pAdm, -1, 0, DIRS_NODE_VALID, STR_DIRS_MAIN,NULL,NULL,0)<0 )		
        return FALSE;
       
    //	create user dir
    nDirInfo = sizeof(ITEMINFO);		
    hFile = open( DEFAULT_DIRFILE, O_RDONLY);	
    if ( hFile != -1 )
    {
		memset(&dirInfo, 0, nDirInfo);
		lseek(hFile, -nDirInfo, SEEK_END);
        nRead = read(hFile, &dirInfo, nDirInfo);

		if(nRead != nDirInfo || stricmp(dirInfo.aIconName, "endoffile") != NULL)
		{
			close(hFile);
			remove(DEFAULT_DIRFILE);
			hFile = -1;
			hFile = open( DEFAULT_APPFILE, O_RDONLY);
			if(hFile != -1)
			{
				close(hFile);
				remove(DEFAULT_APPFILE);
			}
			//	create default dir
			AppDirsCreateDefault( pAdm );//1 level menu
			//save and modify info
			AppDirsSaveDirs(pAdm);

			return AppDirGrpFile(pAdm);	//2 level menu
		}

		memset(&dirInfo, 0, nDirInfo);
		lseek(hFile,0, SEEK_SET);
        nRead = read(hFile, &dirInfo, nDirInfo);

		if(nRead == nDirInfo && dirInfo.nodeId == 0)
		{
			nDirNum = dirInfo.nChild;
		    pAdm->pDirsCache->nDirChildNum = dirInfo.nChild;
		}
		memset(&dirInfo, 0, nDirInfo);
        nRead = read(hFile, &dirInfo, nDirInfo);

	    while(nRead == nDirInfo && i < nDirNum 
			&& stricmp(dirInfo.aIconName, "endoffile") != NULL)
        {		
			AppDirsCreateIndirect( pAdm, &dirInfo );
			memset(&dirInfo, 0, nDirInfo);
			nRead = read(hFile, &dirInfo, nDirInfo);
			i++;
        }
        close(hFile);
    }
	else
	{
	    //	create default dir
		AppDirsCreateDefault( pAdm );//1 level menu
		//save and modify info
		AppDirsSaveDirs(pAdm);
	}
    
    return AppDirGrpFile(pAdm);	//2 level menu
    
};
/********************************************************************
* Function   AppDirsInitialize  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL AppDirsInitialize( struct appMain * pAdm )
{
   
    return AppDirsSetUp ( pAdm );
};
/********************************************************************
* Function   AppDirsCreateIndirect  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static	PDIRSLINK	AppDirsCreateIndirect ( struct appMain * pAdm, PITEMINFO pInfo )
{
    LPCSTR		pPathName;
    PDIRSLINK	pFather = NULL, pDirsLink = NULL;
    PDIRSNODE	pDirNode = NULL;
    

    if (pInfo->fatherId < 0 )
        return NULL;					
    
	if(pInfo->nodeId < MAX_DIRS_NODE)
		pDirNode  = AppGetDirNodeFromId(pAdm, pInfo->nodeId, NULL);
	else
		pDirNode = malloc(sizeof(DIRSNODE));

	if(!pDirNode)
		return NULL;
	
	if(pInfo->nodeId >= MAX_DIRS_NODE)
	{
		pDirNode->flags = DIRS_NODE_OVER;
		DirNodeID ++;	
	}
	 
    pPathName = pInfo->aDisplayName;
    while ( (*pPathName == '/' || *pPathName == '\\' || *pPathName == ' ') && *(pPathName+1) != 0 )
        pPathName++;
    
    if (*pPathName == ' ')
	{
		if(pInfo->nodeId >= MAX_DIRS_NODE)
		{
			free(pDirNode);
			pDirNode = NULL;
		}
        return NULL;
	}
    
//    if ( AppDirsGetNodeID ( pAdm, pPathName ) != -1 )
//        return NULL;					//	the same appname exist
    
    if ( pInfo->fatherId >= 0 )
        pFather = AppDirsFind( pAdm->pDirRoot, pInfo->fatherId );
    
    if ( !pFather )
	{
		if(pInfo->nodeId >= MAX_DIRS_NODE)
		{
			free(pDirNode);
			pDirNode = NULL;
		}
        return NULL;					//	father node is not exist
	}
    
    pDirNode->flags	= DIRS_NODE_VALID;
	pDirNode->dwstyle = pInfo->attrib;
    pDirNode->fatherId	= pInfo->fatherId;
	pDirNode->dirId = pInfo->nodeId;
	pDirNode->iIndex = pInfo->iIndex;

	if(pPathName)
	{
		strcpy( pDirNode->aDisplayName, pPathName);
	}
	if(pInfo->aIconName[0] != 0)
		strcpy(pDirNode->aIconName, pInfo->aIconName);

	if(pInfo->aTitleName[0] != 0)
		strcpy(pDirNode->aTitleName, pInfo->aTitleName);
    
	pDirsLink = AppDirsConstruct ( pAdm, pFather, pDirNode );
	if(pDirsLink!= NULL)
		return pDirsLink;
	else
	{
		if(pInfo->nodeId >= MAX_DIRS_NODE)
		{
			free(pDirNode);
			pDirNode = NULL;
		}
		return NULL;
	}
}
/********************************************************************
* Function   AppDirsCreate  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
short	AppDirsCreate ( struct appMain * pAdm, short fatherId, short nIndex,
						   short flags, PCSTR pPathName, PCSTR pIconName, 
						   PCSTR pTitleName,BOOL bEdit)
{
	PDIRSNODE   pDirNode = NULL;
    PDIRSLINK	pFather = NULL;	
	PDIRSLINK   pDirsLink =  NULL;
    
    while ( (*pPathName == '/' || *pPathName == '\\' || *pPathName == ' ') && *(pPathName+1) != 0 )
        pPathName++;
    
	if(*pPathName == 0)
		return NULL;
	
    if (*pPathName == ' ')
        return NULL;
    
    if (fatherId == -1 && pAdm->pDirRoot)	//	root is created or not
        return NULL;						//	root is created
    
//    if ( AppDirsGetNodeID ( pAdm, pPathName ) != -1 )
//        return NULL;						//	the same name exists
    
    if ( fatherId >= 0 )
        pFather = AppDirsFind(pAdm->pDirRoot, fatherId);
    
    if ( fatherId != -1 && !pFather )
        pFather = pAdm->pDirCurr;
    
    pDirNode = AppDirsCreateNode(pAdm, fatherId, nIndex, flags, pPathName,pTitleName, bEdit);
    if ( pDirNode == NULL )
        return NULL;

	if(pIconName)
		strcpy(pDirNode->aIconName, pIconName);
	if(pTitleName)
		strcpy(pDirNode->aTitleName, pTitleName);
    
	if(!bEdit)
	{
		pDirsLink = AppDirsConstruct ( pAdm, pFather, pDirNode );
		if(pDirsLink == NULL && pDirNode->dirId > MAX_DIRS_NODE)
		{
			free(pDirNode);
			pDirNode = NULL;
		}
	}
	else
	{
		pDirsLink = AppInsertDirsConstruct( pAdm, pFather, pDirNode );
		if(pDirsLink == NULL && pDirNode->dirId > MAX_DIRS_NODE)
		{
			free(pDirNode);
			pDirNode = NULL;
		}
	}

	return pDirNode->dirId;
}
/********************************************************************
* Function   AppDirsRename  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppDirsRename ( struct appMain * pAdm, PCSTR pNewName, short dirId)
{	
    struct		DirsLink * pDir;
    PDIRSNODE	pDirsNode = NULL;
	PCSTR       pOldName = NULL;
	int         strNewNamelen = 0;
    
	if ( dirId < 0 )
        return FALSE;

	pDirsNode = AppGetDirNodeFromId(pAdm, dirId, NULL);

	if(pDirsNode)
		pOldName = pDirsNode->aDisplayName;
	else
		return FALSE;
	
    while ( (*pNewName == '/' || *pNewName == '\\' || *pNewName == ' ') && *(pNewName+1) != 0 )
        pNewName++;
    
	if(*pNewName == 0)
		return NULL;
	
    if ( *pNewName == ' ' )
        return FALSE;
    
    if ( strcmp(pNewName, pOldName) == 0 )
        return FALSE;
    
    pDir = AppDirsFind( pAdm->pDirRoot, dirId );
    if ( !pDir || ( pDir->pFather == NULL && pDir->dirId == 0 ) )
        return FALSE;		//	root dir

	strNewNamelen = strlen(pNewName);

	memset(pDirsNode->aDisplayName, 0, PMNODE_NAMELEN + 1);
	if(strNewNamelen > PMNODE_NAMELEN)
		strNewNamelen = PMNODE_NAMELEN;
	strncpy( pDirsNode->aDisplayName, pNewName, strNewNamelen);
	pDirsNode->aDisplayName[strNewNamelen] = 0;
	strncpy( pDirsNode->aTitleName, pNewName, strNewNamelen);
	pDirsNode->aTitleName[strNewNamelen] = 0;
    
    //save info
//	AppDirsSaveFile(pAdm);
    AppDirsSaveDirs(pAdm);
    return TRUE;
};
/********************************************************************
* Function   AppDirsDelete  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppDirsDelete ( struct appMain * pAdm, short dirID)
{	
    struct		DirsLink * pDir, * pTemp;
	struct      appLink  * pTempApp;
    PDIRSNODE	pDirsNode = NULL,pFatherNode = NULL;
	PAPPNODE    pAppNode = NULL;
	short       iOldIndex;
    
    if (dirID < 0)
        return FALSE;

    pDir = AppDirsFind ( pAdm->pDirRoot, dirID );
    if ( !pDir || (pDir->pFather == NULL && pDir->dirId == 0) )
        return FALSE;
    
    pTemp = pDir->pFather;
    if ( !pTemp )		//	no father node is root node	
        return FALSE;

	if(pDir->pAppLink != NULL)
		return FALSE;   //only delete null dir

	pTemp =  pDir->pFather->pDirChild;
	pFatherNode = pAdm->pDirsCache;

	if(!pFatherNode)
		return FALSE;
	
	while(pTemp) //delete link to root dir
	{
		if(pDir->dirId == pTemp->dirId)
		{
			if(pTemp->pPrev)
				pTemp->pPrev->pNext = pDir->pNext;
			else
			{
				pDir->pFather->pDirChild = pTemp->pNext;
				pDir->pFather->pDirChild->pPrev = NULL;
			}
			if(pTemp->pNext)
				pTemp->pNext->pPrev = pDir->pPrev;
			pFatherNode->nDirChildNum --;
			break;
		}
		pTemp = pTemp->pNext;
	}

	if(pDir != pTemp)
		return FALSE;

	if(pDir->pDirNode != NULL) // over dir
	{
		pDirsNode = pDir->pDirNode;
		iOldIndex = pDirsNode->iIndex;
		free(pDirsNode);
		pDirsNode = NULL;
	}
	else
	{
		pDirsNode = pAdm->pDirsCache+dirID;
		if(!pDirsNode)
			return FALSE;
		iOldIndex = pDirsNode->iIndex;
		pDirsNode->flags = 0;	
	}

	pTemp =  pDir->pFather->pDirChild;

	while(pTemp)
	{
		pDirsNode = AppGetDirNodeFromId(pAdm, pTemp->dirId, pTemp);
	
		if(pDirsNode->iIndex > iOldIndex )
			pDirsNode->iIndex = pDirsNode->iIndex - 1;

		pTemp = pTemp->pNext;
	}

	pTempApp = pDir->pFather->pAppLink;

	while (pTempApp)
	{
		pAppNode = pAdm->pFileCache + pTempApp->appId;
		if(pAppNode->iIndex > iOldIndex)
			pAppNode->iIndex = pAppNode->iIndex - 1;

		pTempApp = pTempApp->pNext;		

	}
    //save info
	AppDirsSaveFile(pAdm);
    AppDirsSaveDirs(pAdm);
    
    return TRUE;
};
/********************************************************************
* Function   AppDirsConstruct  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static	PDIRSLINK AppDirsConstruct ( struct appMain * pAdm, PDIRSLINK pFather, PDIRSNODE pDirsNode )
{
    PDIRSLINK	pDir = NULL, pTemp = NULL;
	PDIRSNODE   pFatherNode = NULL;
	short       nDirNum = 0;

//	if(pFather == NULL)//xlzhu not add this line ,which will make rootdir create fail
//		return NULL;
    
    pDirsNode->flags |= DIRS_NODE_VALID;	//dir valid
    
    pDir = ( PDIRSLINK ) malloc ( sizeof(DIRSLINK) );
    if ( pDir == NULL )						
    {
        pDirsNode->flags = 0;				//	dir invalid
        return NULL;
    }
    
    pDir->dirId		= pDirsNode->dirId;	
    pDir->pAdm		= pAdm;	
    pDir->pFather   = pFather;
    pDir->pDirChild = NULL;
    pDir->pAppLink = NULL;					//	havn't sub app
    pDir->pNext		= NULL;
	pDir->pPrev		= NULL;

	if(pDirsNode->dirId >= MAX_DIRS_NODE)
		pDir->pDirNode = pDirsNode;
	else
		pDir->pDirNode = NULL;
    
    if ( pDirsNode->dirId == 0 )			//	root dir
    {
        pAdm->pDirRoot = pDir;
        return pDir;
    }
    
    if(pDir->pFather == pAdm->pDirRoot )
        pAdm->pDirCurr = pDir;
    
    pTemp = pFather->pDirChild;
    if ( !pTemp )
    {
		pFatherNode = pAdm->pDirsCache + pFather->dirId;

        pFather->pDirChild = pDir;		//	add new built dir to dirlist
		pFatherNode->nDirChildNum = 1;
	    return pDir;
    }

    while ( pTemp->pNext )
	{
        pTemp = pTemp->pNext;
		nDirNum++;
	}

    pTemp->pNext = pDir;
    pDir->pPrev = pTemp;
	nDirNum ++;
    
	pFatherNode = pAdm->pDirsCache + pFather->dirId;
	pFatherNode->nDirChildNum = nDirNum +1;

    return pDir;
};
/********************************************************************
* Function   AppInsertDirsConstruct  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static	PDIRSLINK AppInsertDirsConstruct( struct appMain * pAdm, PDIRSLINK pFather, PDIRSNODE pDirsNode )
{
	PDIRSLINK	pDir = NULL, pTemp = NULL;
	PDIRSAPP    pAppLink = NULL; 
	PDIRSNODE  pFatherNode= NULL;
	short      iIndex = 0, nDirNum = 0;;
    
    pDirsNode->flags |= DIRS_NODE_VALID;	//dir valid
    
    pDir = ( PDIRSLINK ) malloc ( sizeof(DIRSLINK) );
    if ( pDir == NULL )						
    {
        pDirsNode->flags = 0;				//	dir invalid
        return NULL;
    }
    
    pDir->dirId		= pDirsNode->dirId;	
    pDir->pAdm		= pAdm;	
    pDir->pFather   = pFather;
    pDir->pDirChild = NULL;
    pDir->pAppLink = NULL;					//	havn't sub app
    pDir->pNext		= NULL;
	pDir->pPrev = NULL;

	if(pDirsNode->dirId >= MAX_DIRS_NODE)
		pDir->pDirNode = pDirsNode;
	else
		pDir->pDirNode = NULL;
    
//    if ( pDirsNode->dirId == 0 )			//	root dir
//    {
//        pAdm->pDirRoot = pDir;
//        return pDir;
//    }
    
    if(pDir->pFather == pAdm->pDirRoot )
        pAdm->pDirCurr = pDir;
    
	pAppLink = pFather->pAppLink;
	while (pAppLink)
	{
		iIndex ++;
		pAppLink = pAppLink->pNext;
	}

    pTemp = pFather->pDirChild;
    if ( !pTemp )
    {
		pFatherNode = pAdm->pDirsCache + pFather->dirId;
        pFather->pDirChild = pDir;		//	add new built dir to dirlist
		pDirsNode->iIndex = iIndex;
		pFatherNode->nDirChildNum = 1;
	
		AppDirsSaveDirs(pAdm);
	    return pDir;
    }

	while ( pTemp->pNext )
	{
		iIndex ++;
		pTemp = pTemp->pNext;
		nDirNum++;
	}
	
	pDirsNode->iIndex = iIndex + 1;
	pTemp->pNext = pDir;
	pDir->pPrev = pTemp;
	
	nDirNum++;
	pFatherNode = pAdm->pDirsCache + pFather->dirId;
	pFatherNode->nDirChildNum = nDirNum + 1;
    //save info
	AppDirsSaveDirs(pAdm);

    return pDir;
}
/********************************************************************
* Function   AppDirsCreateNode  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static  PDIRSNODE AppDirsCreateNode( struct appMain * pAdm, short fatherId,short nIndex,
							  short flags, PCSTR pPathName,PCSTR pTitleName, BOOL bEdit)
{
    PDIRSNODE	pDirsNode = NULL;
    register	int dirID;
    
    pDirsNode = pAdm->pDirsCache;
    
    for ( dirID = 0; dirID < MAX_DIRS_NODE; dirID ++, pDirsNode ++ )
    {
        if ( pDirsNode->flags == 0 )	//	when the  node idlesse the flags == 0
            break;
    }
	
	if(dirID >= MAX_DIRS_NODE)
	{
		dirID = DirNodeID;
		//need to know the memory is enough ?
		pDirsNode = malloc(sizeof(DIRSNODE));
		if(pDirsNode == NULL)
			return NULL;
        pDirsNode->flags = DIRS_NODE_OVER;
		DirNodeID ++;
	}
  	
    if ( dirID >= MAX_DIRS_NODE && (pDirsNode->flags != DIRS_NODE_OVER) )
	{
		if(dirID >= MAX_DIRS_NODE)
		{
			free(pDirsNode);
			pDirsNode = NULL;
		}
        return NULL;
	}
    
    if ( fatherId < 0 && dirID > 0)		//	root dir id must be 0
	{
		if(dirID >= MAX_DIRS_NODE)
		{
			free(pDirsNode);
			pDirsNode = NULL;
		}
        return NULL;	
	}
    
    flags &= ~DIRS_NODE_VALID;
    pDirsNode->flags	= (short)flags;
    pDirsNode->fatherId	= fatherId;
	pDirsNode->dwstyle = bEdit;
	pDirsNode->iIndex = nIndex;
	
	pDirsNode->dirId = dirID;
    strcpy(pDirsNode->aDisplayName, pPathName);
	
    if(pTitleName)
		strcpy(pDirsNode->aTitleName, pTitleName);
    
    return pDirsNode;
};
/********************************************************************
* Function   AppDirsDeleteNode  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static	BOOL	AppDirsDeleteNode ( struct appMain * pAdm, short dirID )
{
    PDIRSNODE	pDirsNode = NULL;
	PDIRSLINK   pDir = NULL;
    
    pDirsNode = AppGetDirNodeFromId(pAdm, dirID, NULL);

	if(pDirsNode->flags == DIRS_NODE_OVER)
	{
		pDir = AppDirsFind( pAdm->pDirRoot, dirID );
		free(pDirsNode);
		pDir->pDirNode = NULL;
		DirNodeID--;
		return TRUE;
	}
    pDirsNode->flags = 0;

	if(pDirsNode->aDisplayName)
        memset(pDirsNode->aDisplayName, 0x00, sizeof(pDirsNode->aDisplayName));
	if(pDirsNode->aIconName)
		memset(pDirsNode->aIconName, 0x00, sizeof(pDirsNode->aIconName));
	if(pDirsNode->aTitleName)
		memset(pDirsNode->aTitleName, 0x00, sizeof(pDirsNode->aTitleName));
    
    return TRUE;
}
/********************************************************************
* Function   AppDirsFind  
* Purpose    search dirs object,find dirID of given dir
* Params     
* Return     
* Remarks      
**********************************************************************/
PDIRSLINK	AppDirsFind ( PDIRSLINK pDir, short dirID )
{
    PDIRSLINK pDirFind = NULL;
    
    pDirFind = pDir;
    
    if ( !pDirFind || pDirFind->dirId == dirID )
        return pDirFind;
    
    if (pDir->pNext)
        pDirFind = AppDirsFind( pDir->pNext, dirID );
    else
        pDirFind = NULL;
    
    if (!pDirFind && pDir->pDirChild)
        pDirFind = AppDirsFind( pDir->pDirChild, dirID );	
    
    return pDirFind;	
};
/********************************************************************
* Function	  AppDirsAddFile
* Purpose   add app to the given group
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL	AppDirsAddFile( struct appMain * pAdm , short dirID, short fileID)
{
    PDIRSAPP	pFileLink = NULL, pTempLink = NULL, pNext = NULL;
    PDIRSLINK	pDir = NULL;
    PAPPNODE	pFileNode = NULL, pTempNode = NULL;
    
    pDir = AppDirsFind( pAdm->pDirRoot, dirID );
    if ( !pDir )
        return FALSE;
	
	pFileNode = pAdm->pFileCache+fileID;

	if((GetSIMState()==0 ||GetSimStk() == 0)&& stricmp(pFileNode->aDisplayName, "Toolkit") == NULL)
		return TRUE;

	if((GetSIMState()==0 ||GetSIMInfoAct() == 0) && stricmp(pFileNode->aDisplayName, "SIM info") == NULL)
		return TRUE;

	if((GetSIMState()==0 ||GetMailNumAct() == 0) && stricmp(pFileNode->aDisplayName, "SIM mailbox") == NULL)
		return TRUE;
	
	pFileLink  = (DIRSAPP*) malloc (sizeof(DIRSAPP));
    if ( pFileLink == NULL)
        return FALSE;
    
    pFileLink->appId	= fileID;				//	add new dir node to father dir node
       
    pTempLink = pDir->pAppLink;
    if ( !pTempLink )
    {
        pFileLink->pNext	= pDir->pAppLink;
        pDir->pAppLink		= pFileLink;
    }
    else 
    {
		pTempNode = pAdm->pFileCache+pTempLink->appId;
		
		pNext = pTempLink->pNext;
		while ( pNext )
		{
			pTempNode = pAdm->pFileCache+pNext->appId;		
			pTempLink = pNext;
			pNext = pNext->pNext;
		}
		PLXPrintf("\r\n prenode name = %s, id=%d",pTempNode->aDisplayName,
			pTempNode->appId);
		pFileLink->pNext = pNext;
		pTempLink->pNext = pFileLink;
  
    }
    return TRUE;
}
/********************************************************************
* Function   AppDirsSaveNode  
* Purpose    save child appDirs object
* Params     
* Return     
* Remarks      
**********************************************************************/
static	void	AppDirsSaveNode( struct appMain * pAdm, PDIRSLINK pDir , int hFile )
{
    ITEMINFO	pInfo;
    PDIRSNODE	pNode = NULL;
    
    if ( !pDir )
        return ;

	memset(&pInfo, 0, sizeof(ITEMINFO));
    
	if(pDir->dirId < MAX_DIRS_NODE)
		pNode = pAdm->pDirsCache+pDir->dirId;
	else
		pNode = pDir->pDirNode;
    if (pNode->flags)
    {
		strcpy(pInfo.aDisplayName , pNode->aDisplayName);
		strcpy(pInfo.aTitleName, pNode->aTitleName);
		strcpy(pInfo.aIconName, pNode->aIconName);
        
        pInfo.nodeId	= pDir->dirId;
        pInfo.fatherId	= pNode->fatherId;
        pInfo.flags		= DIR_TYPE;
		pInfo.iIndex = pNode->iIndex;
		pInfo.attrib = pNode->dwstyle;
		pInfo.nChild = pNode->nDirChildNum;
       
        write(hFile, &pInfo, sizeof(ITEMINFO));
    }	
    
    if ( pDir->pNext )
        AppDirsSaveNode( pAdm, pDir->pNext, hFile );
    
    if ( pDir->pDirChild )
        AppDirsSaveNode( pAdm, pDir->pDirChild, hFile );
}
/********************************************************************
* Function   AppDirsSaveDirs  
* Purpose    save appDirs object,save config file
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppDirsSaveDirs( struct appMain * pAdm)
{
    int			hFile;
    PDIRSLINK	pDir = NULL;
	ITEMINFO	*pInfo = NULL;
    
	hFile = open(DEFAULT_DIRFILE, O_RDWR, S_IRWXU);
	
	if(hFile != -1)
	{
		close(hFile);
		remove(DEFAULT_DIRFILE);
	}

	hFile = open(DEFAULT_DIRFILE, O_RDWR|O_CREAT, S_IRWXU);	
	if (hFile == -1)
		return FALSE;
    
    pDir = pAdm->pDirRoot;
    
    if ( !pDir )
    {
        close(hFile);
        return FALSE;
    }
    AppDirsSaveNode( pAdm, pDir, hFile );

	pInfo = malloc(sizeof(ITEMINFO));
	if(pInfo == NULL)
	{
		close(hFile);
		return FALSE;
	}
	memset(pInfo, 0, sizeof(ITEMINFO));
	strcpy(pInfo->aIconName, "endoffile");
	write(hFile, pInfo, sizeof(ITEMINFO));//user for close file
	free(pInfo);

    close(hFile);
    
    return TRUE;
};
/********************************************************************
* Function   AppDirsSaveDirsEx  
* Purpose    save appDirs object,save config file
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppDirsSaveDirsEx( struct appMain * pAdm, BOOL bRemove )
{
    int			hFile;
    PDIRSLINK	pDir = NULL;
	ITEMINFO	*pInfo = NULL;
    
	if(bRemove)
	{
		
		hFile = open(DEFAULT_DIRFILE, O_RDWR, S_IRWXU);
		
		if(hFile != -1)
		{
			close(hFile);
			remove(DEFAULT_DIRFILE);
		}
	}
	
	hFile = open(DEFAULT_DIRFILE, O_RDWR|O_CREAT, S_IRWXU);	
	if (hFile == -1)
		return FALSE;
    
    pDir = pAdm->pDirRoot;
    
    if ( !pDir )
    {
        close(hFile);
        return FALSE;
    }
    AppDirsSaveNode( pAdm, pDir, hFile );
	
	pInfo = malloc(sizeof(ITEMINFO));
	if(pInfo == NULL)
	{
		close(hFile);
		return FALSE;
	}
	memset(pInfo, 0, sizeof(ITEMINFO));
	strcpy(pInfo->aIconName, "endoffile");
	write(hFile, pInfo, sizeof(ITEMINFO));//user for close file
	free(pInfo);

    close(hFile);
    
    return TRUE;
};
/********************************************************************
* Function   AppFileSaveNode  
* Purpose    save child appGroup object,save appnode group info
* Params     
* Return     
* Remarks      
**********************************************************************/
static	void	AppFileSaveNode( struct appMain * pAdm, PDIRSLINK pDir , int hFile )
{
    ITEMINFO	pInfo;
    PAPPNODE	pFileNode = NULL;
    PDIRSAPP	pFileLink = NULL;
    
    if ( !pDir )
        return ;

	memset(&pInfo, 0, sizeof(ITEMINFO));
    
    pFileLink = pDir->pAppLink;
    while ( pFileLink )
    {
        pFileNode = pAdm->pFileCache+pFileLink->appId;
        if (! pFileNode->sFlag /*|| (pFileNode->sFlag & APP_NODE_HIDE)*/)
        {
            pFileLink = pFileLink->pNext;
            continue;
        }
        
		pInfo.nodeId = pFileLink->appId;
        
        pInfo.fatherId	= pFileNode->fatherId;
		pInfo.flags		= APP_TYPE;
		pInfo.nChild = 0;
		pInfo.attrib = pFileNode->nType;
		if(pFileNode->sFlag & APP_NODE_HIDE)
			pInfo.attrib |= HIDE_APP;
		pInfo.pChildAppId = 0;
		pInfo.pChildDirId = 0;
		pInfo.iIndex = pFileNode->iIndex;
       
        strcpy( pInfo.aDisplayName, pFileNode->aDisplayName);
		strcpy(pInfo.aTitleName, pFileNode->aTitleName);

		strcpy(pInfo.aIconName, pFileNode->cIconName);
		
        write(hFile, &pInfo, sizeof(ITEMINFO));
        
        pFileLink = pFileLink->pNext;
    }
    
    if ( pDir->pNext )
        AppFileSaveNode( pAdm, pDir->pNext, hFile );
    
    if ( pDir->pDirChild )
        AppFileSaveNode( pAdm, pDir->pDirChild, hFile );
}
/********************************************************************
* Function   AppDirsSaveFile  
* Purpose    save appGroup object,save group info
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppDirsSaveFile ( struct appMain * pAdm)
{
    int			hFile;
    PDIRSLINK	pDir = NULL;
	ITEMINFO    *pInfo = NULL;
    
	hFile = open(DEFAULT_APPFILE, O_RDWR, S_IRWXU);
	
	if(hFile != -1)
	{
		close(hFile);
		remove(DEFAULT_APPFILE);
	}	

	hFile = open(DEFAULT_APPFILE, O_RDWR|O_CREAT, S_IRWXU);
	
	if (hFile == -1)
		return FALSE;
	
    pDir = pAdm->pDirRoot;
    if ( !pDir )
    {
        close(hFile);
        return FALSE;
    }
    
    AppFileSaveNode( pAdm, pDir, hFile );

	pInfo = malloc(sizeof(ITEMINFO));
	if(pInfo == NULL)
	{
		close(hFile);
		return FALSE;
	}
	memset(pInfo, 0, sizeof(ITEMINFO));
	strcpy(pInfo->aIconName, "endoffile");
	write(hFile, pInfo, sizeof(ITEMINFO));//user for close file
	free(pInfo);

    close(hFile);
    
    return TRUE;
};
/********************************************************************
* Function   AppDirsSaveFileEx  
* Purpose    save appGroup object,save group info
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppDirsSaveFileEx ( struct appMain * pAdm, BOOL bRemove )
{
    int			hFile;
    PDIRSLINK	pDir = NULL;
	ITEMINFO    *pInfo = NULL;
    

    if(bRemove)
	{
		hFile = open(DEFAULT_APPFILE, O_RDWR, S_IRWXU);

		if(hFile != -1)
		{
			close(hFile);
			remove(DEFAULT_APPFILE);
		}	
	}

	hFile = open(DEFAULT_APPFILE, O_RDWR|O_CREAT, S_IRWXU);
	
	if (hFile == -1)
		return FALSE;
	
    pDir = pAdm->pDirRoot;
    if ( !pDir )
    {
        close(hFile);
        return FALSE;
    }
    
    AppFileSaveNode( pAdm, pDir, hFile );

	pInfo = malloc(sizeof(ITEMINFO));
	if(pInfo == NULL)
	{
		close(hFile);
		return FALSE;
	}
	memset(pInfo, 0, sizeof(ITEMINFO));
	strcpy(pInfo->aIconName, "endoffile");
	write(hFile, pInfo, sizeof(ITEMINFO));//user for close file
	free(pInfo);

    close(hFile);
    
    return TRUE;
};
/********************************************************************
* Function   AppDirsMoveFile  
* Purpose    change dirs father
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppDirsMoveDirs ( struct appMain * pAdm, short dirID, PDIRSLINK pDirSrc, PDIRSLINK pDirDst )
{
    PDIRSLINK	pDir = NULL, pTemp = NULL;
    PDIRSNODE    pDirNode = NULL;
    
    if ( pDirSrc == pDirDst )
        return FALSE;
    if ( !pDirSrc || !pDirDst )
        return FALSE;
    
    pTemp = pDirSrc->pDirChild;	
    if ( !pTemp )
        return FALSE;
    
    while ( pTemp->dirId != dirID && pTemp->pNext )
    {
        pDir  = pTemp;
        pTemp = pTemp->pNext;
    }
    
    if ( pTemp->dirId != dirID )
        return FALSE;
    
    if ( pTemp == pDirSrc->pDirChild )		//	move out from src dirs
        pDirSrc->pDirChild = pTemp->pNext;
    else
        pDir->pNext = pTemp->pNext;
    
    //father node need changed
    pTemp->pFather = pDirDst;
    pDirNode = AppGetDirNodeFromId(pAdm, dirID, NULL);

    if(pDirNode != NULL)
        pDirNode->fatherId = pDirDst->dirId;
    
    pTemp->pNext = NULL;
    pDir = pDirDst->pDirChild;				//	move to dest last one
    if ( !pDir )
    {
        pDirDst->pDirChild = pTemp;
        return TRUE;
    }
    
    while ( pDir->pNext )
        pDir = pDir->pNext;
    pDir->pNext = pTemp;
    
    return TRUE;
};
/********************************************************************
* Function   AppDirsMoveFile  
* Purpose    change app dirs
* Params     
* Return     
* Remarks      
**********************************************************************/
/*
BOOL	AppDirsMoveFile ( struct appMain * pAdm, PCSTR pName, PDIRSLINK pDirSrc, PDIRSLINK pDirDst )
{
    short		fileID;
    PDIRSAPP	pFile, pTemp = NULL;
    PAPPNODE	pNode = NULL;
    
    if ( pDirSrc == pDirDst )
        return FALSE;
    
    if ( !pDirSrc || !pDirDst )
        return FALSE;
    
    fileID = pAdm->pVtbl->GetFileID( pAdm, pName );
    if ( fileID < 0 )
        return FALSE;
    
    pTemp = pDirSrc->pAppLink;
    if ( !pTemp )
        return FALSE;
    
    pFile = pTemp;
    
    while ( pTemp->appId != fileID && pTemp->pNext )
    {
        pFile = pTemp;
        pTemp = pTemp->pNext;
    }
    
    if ( pTemp->appId != fileID )	
        return FALSE;
    
    if ( pTemp == pDirSrc->pAppLink )
        pDirSrc->pAppLink = pTemp->pNext;
    else
        pFile->pNext = pTemp->pNext;
    
    pNode = pAdm->pFileCache+fileID;
    pNode->fatherId = pDirDst->dirId;
    
    pTemp->pNext = NULL;
    pFile = pDirDst->pAppLink;		//	move to the last one	
    if ( !pFile )
    {
        pDirDst->pAppLink = pTemp;
        return TRUE;
    }
    
    while ( pFile->pNext )
        pFile = pFile->pNext;
    pFile->pNext = pTemp;
    
    return TRUE;
};*/

/********************************************************************
* Function   AppDirsMoveFileByID  
* Purpose    change appfile from onr app dir to the other
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppDirsMoveFileByID ( struct appMain * pAdm, short fileID, PDIRSLINK pDirSrc, PDIRSLINK pDirDst )
{
    PDIRSAPP	pFile, pTemp = NULL;
    PAPPNODE	pNode = NULL;
    
    if ( pDirSrc == pDirDst )
        return FALSE;
    
    if ( !pDirSrc || !pDirDst )
        return FALSE;
    
    if ( fileID < 0 )
        return FALSE;
    
    pTemp = pDirSrc->pAppLink;
    if ( !pTemp )
        return FALSE;
    
    pFile = pTemp;
    
    while ( pTemp->appId != fileID && pTemp->pNext )
    {
        pFile = pTemp;
        pTemp = pTemp->pNext;
    }
    
    if ( pTemp->appId != fileID )	
        return FALSE;
    
    if ( pTemp == pDirSrc->pAppLink )
        pDirSrc->pAppLink = pTemp->pNext;
    else
        pFile->pNext = pTemp->pNext;
    
    pNode = pAdm->pFileCache+fileID;
    pNode->fatherId = pDirDst->dirId;
    
    pTemp->pNext = NULL;
    pFile = pDirDst->pAppLink;		//	move to the last one	
    if ( !pFile )
    {
        pDirDst->pAppLink = pTemp;
        return TRUE;
    }
    
    while ( pFile->pNext )
        pFile = pFile->pNext;
    pFile->pNext = pTemp;

    return TRUE;
};

/********************************************************************
* Function	  AppDirGrpFile
* Purpose     group app and save group info
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AppDirGrpFile(struct appMain* pAdm)
{
    int			hFile = -1, hDirFile = -1;
    register    int i, nRead;
    int			sDirInfo;
    ITEMINFO    appInfo;
    PAPPNODE	pFileNode = NULL;
	static short    stkIndex, fatherid = -1,fatherid_SimInfo = -1,fatherid_MailNum = -1;
	static short Index_SimInfo, Index_MailNum;
       
    hFile = open(DEFAULT_APPFILE, O_RDONLY);
    if ( hFile == -1 )
        goto GROUP_LEFT;
    
    sDirInfo = sizeof(ITEMINFO);
	memset(&appInfo, 0, sizeof(ITEMINFO));
	lseek(hFile, -sDirInfo, SEEK_END);
	nRead = read( hFile, &appInfo, sDirInfo );
	if(nRead != sDirInfo || stricmp(appInfo.aIconName, "endoffile") != NULL)
	{
		close(hFile);
		remove(DEFAULT_APPFILE);
		hFile = -1;
		hFile = open(DEFAULT_DIRFILE, O_RDONLY);
		{
			if(hFile != -1)
			{
				close(hFile);
				remove(DEFAULT_DIRFILE);
			}
		}
		goto GROUP_LEFT;
	}
	else
	{
		
		hDirFile = open(DEFAULT_DIRFILE, O_RDONLY);
		
		if(hDirFile != -1)
		{
			memset(&appInfo, 0, sDirInfo);
			nRead = 0;
			lseek(hFile, -sDirInfo , SEEK_END);
			nRead = read(hFile, &appInfo, sDirInfo);
			if(nRead != sDirInfo || stricmp(appInfo.aIconName, "endoffile") != NULL)
			{
				close(hDirFile);
				remove(DEFAULT_DIRFILE);
				close(hFile);
				remove(DEFAULT_APPFILE);
				goto GROUP_LEFT;
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
			goto GROUP_LEFT;
		}
		
	}
	
	memset(&appInfo, 0, sizeof(ITEMINFO));
    lseek(hFile, 0, SEEK_SET);
    nRead = read( hFile, &appInfo, sDirInfo );
    while(nRead == sDirInfo && stricmp(appInfo.aIconName, "endoffile") != NULL)
    {
        short fileID;    
        
        fileID = AppGetIdFromIconName(pAdm, appInfo.aIconName);
        if (fileID >= 0)
        {
            pFileNode = pAdm->pFileCache+fileID;
			if(stricmp(pFileNode->aDisplayName, "Toolkit") == NULL 
				&&(GetSIMState()==0 ||GetSimStk() == 0))
			{
				short fileID = 0;
				PAPPNODE pNode = NULL;
				
				fileID = AppGetIdFromIconName(pAdm, "/rom/progman/app/mstk_43x28.bmp");
				if(fileID >=0)
				{
					int num = 0;
					pNode = pAdm->pFileCache+fileID;
					fatherid = pNode->fatherId;
					stkIndex = pNode->iIndex;
				}
			}
			else if(stricmp(pFileNode->aDisplayName, "SIM info") == NULL 
				&&(GetSIMState()==0 ||GetSIMInfoAct() == 0))
			{
				short fileID = 0;
				PAPPNODE pNode = NULL;
				
				fileID = AppGetIdFromIconName(pAdm, "/rom/progman/app/msiminfo_43x28.bmp");
				if(fileID >=0)
				{
					int num = 0;
					pNode = pAdm->pFileCache+fileID;
					fatherid_SimInfo = pNode->fatherId;
					Index_SimInfo = pNode->iIndex;
				}

			}
			else if(stricmp(pFileNode->aDisplayName, "SIM mailbox") == NULL 
				&&(GetSIMState()==0 ||GetMailNumAct() == 0))
			{
				short fileID = 0;
				PAPPNODE pNode = NULL;
				
				fileID = AppGetIdFromIconName(pAdm, "/rom/progman/app/msimmailbox_43x28.bmp");
				if(fileID >=0)
				{
					int num = 0;
					pNode = pAdm->pFileCache+fileID;
					fatherid_MailNum = pNode->fatherId;
					Index_MailNum = pNode->iIndex;
				}
			}
			else
			{
				PAPPNODE pNode = NULL;			

				AppDirsAddFile( pAdm, appInfo.fatherId, fileID);
				pNode = pAdm->pFileCache +fileID;

				if(pNode->fatherId == fatherid)
				{
					if(pNode->iIndex > stkIndex)
						pNode->iIndex--;
				}
				if(pNode->fatherId == fatherid_SimInfo)
				{
					if(pNode->iIndex > Index_SimInfo)
						pNode->iIndex--;
				}
				if(pNode->fatherId == fatherid_MailNum)
				{
					if(pNode->iIndex > Index_MailNum)
						pNode->iIndex--;
				}
			}
        }
		memset(&appInfo, 0, sDirInfo);
        nRead = read( hFile, &appInfo, sDirInfo );
    }
    close(hFile);
	
	
	if(GetSIMState()==1 && GetSimStk()==1)
	{
		short fileID = 0;
		PAPPNODE pNode = NULL;
		PDIRSLINK pDir = NULL;
		PDIRSAPP pFileLink = NULL;
		
		fileID = AppGetIdFromIconName(pAdm, "/rom/progman/app/mstk_43x28.bmp");
		if(fileID >=0)
		{
			pNode = pAdm->pFileCache+fileID;

			pDir = AppDirsFind( pAdm->pDirRoot, pNode->fatherId );
			if ( !pDir )
				return FALSE;
				
			pFileLink = pDir->pAppLink;
			while(pFileLink)
			{
				if(pFileLink->appId == fileID)
				{
					break;
				}
				pFileLink = pFileLink->pNext;
				
			}

			if(!pFileLink)
			{
				pNode->iIndex = GetDirChildNum(pAdm, pNode->fatherId);
				
				AppDirsAddFile( pAdm, pNode->fatherId, fileID);
			}
			
		}
	}
	else
	{
		if(fatherid != -1)
		{
			PDIRSLINK pDir = NULL, pDirChild = NULL;
			PDIRSNODE pDirNode = NULL;
			
			pDir = AppDirsFind(pAdm->pDirRoot, fatherid);
			if(!pDir)
				return FALSE;
			
			pDirChild = pDir->pDirChild;
			while(pDirChild)
			{
				pDirNode = AppGetDirNodeFromId(pAdm, pDirChild->dirId, pDir);
				if(!pDirNode)
					return FALSE;
				
				if(pDirNode->iIndex > stkIndex)
					pDirNode->iIndex--;
				
				pDirChild = pDirChild->pNext;
			}
		}
	}

	if(GetSIMState()==1 && GetSIMInfoAct()==TRUE)
	{
		short fileID = 0;
		PAPPNODE pNode = NULL;
		PDIRSLINK pDir = NULL;
		PDIRSAPP pFileLink = NULL;
		
		fileID = AppGetIdFromIconName(pAdm, "/rom/progman/app/msiminfo_43x28.bmp");
		if(fileID >=0)
		{
			pNode = pAdm->pFileCache+fileID;
			
			pDir = AppDirsFind( pAdm->pDirRoot, pNode->fatherId );
			if ( !pDir )
				return FALSE;
			
			pFileLink = pDir->pAppLink;
			while(pFileLink)
			{
				if(pFileLink->appId == fileID)
				{
					break;
				}
				pFileLink = pFileLink->pNext;
				
			}
			
			if(!pFileLink)
			{
				pNode->iIndex = GetDirChildNum(pAdm, pNode->fatherId);
				
				AppDirsAddFile( pAdm, pNode->fatherId, fileID);
			}
			
		}

	}
	else
	{
		if(fatherid_SimInfo != -1)
		{
			PDIRSLINK pDir = NULL, pDirChild = NULL;
			PDIRSNODE pDirNode = NULL;
			
			pDir = AppDirsFind(pAdm->pDirRoot, fatherid_SimInfo);
			if(!pDir)
				return FALSE;
			
			pDirChild = pDir->pDirChild;
			while(pDirChild)
			{
				pDirNode = AppGetDirNodeFromId(pAdm, pDirChild->dirId, pDir);
				if(!pDirNode)
					return FALSE;
				
				if(pDirNode->iIndex > Index_SimInfo)
					pDirNode->iIndex--;
				
				pDirChild = pDirChild->pNext;
			}
		}
	}
	
	if(GetSIMState()==1 && GetMailNumAct()==TRUE)
	{
		short fileID = 0;
		PAPPNODE pNode = NULL;
		PDIRSLINK pDir = NULL;
		PDIRSAPP pFileLink = NULL;
		
		fileID = AppGetIdFromIconName(pAdm, "/rom/progman/app/msimmailbox_43x28.bmp");
		if(fileID >=0)
		{
			pNode = pAdm->pFileCache+fileID;
			
			pDir = AppDirsFind( pAdm->pDirRoot, pNode->fatherId );
			if ( !pDir )
				return FALSE;
			
			pFileLink = pDir->pAppLink;
			while(pFileLink)
			{
				if(pFileLink->appId == fileID)
				{
					break;
				}
				pFileLink = pFileLink->pNext;
				
			}
			
			if(!pFileLink)
			{
				pNode->iIndex = GetDirChildNum(pAdm, pNode->fatherId);
				
				AppDirsAddFile( pAdm, pNode->fatherId, fileID);
			}
			
		}

	}
	else
	{
	
		if(fatherid_MailNum != -1)
		{
			PDIRSLINK pDir = NULL, pDirChild = NULL;
			PDIRSNODE pDirNode = NULL;
			
			pDir = AppDirsFind(pAdm->pDirRoot, fatherid_MailNum);
			if(!pDir)
				return FALSE;
			
			pDirChild = pDir->pDirChild;
			while(pDirChild)
			{
				pDirNode = AppGetDirNodeFromId(pAdm, pDirChild->dirId, pDir);
				if(!pDirNode)
					return FALSE;
				
				if(pDirNode->iIndex > Index_MailNum)
					pDirNode->iIndex--;
				
				pDirChild = pDirChild->pNext;
			}
		}
	
	}
	AppDirsSaveFile(pAdm);
	AppDirsSaveDirs(pAdm);

	return TRUE;
    
    //	group the ungrouped app
GROUP_LEFT:	
    for ( i = 0; i < MAX_FILE_NODE; i++ )							
    {
        pFileNode = pAdm->pFileCache+i;
        if ( !pFileNode->sFlag ||(pFileNode->sFlag & APP_NODE_HIDE) 
			|| (pFileNode->sFlag & APP_NODE_SHORTCUT))//edit sms mms email
            continue;
                     
        if(pFileNode->fatherId >= 0)
            AppDirsAddFile(pAdm, pFileNode->fatherId, (short)i);
        
    }
	if(GetSIMState()==0 || GetSimStk() == 0)
	{
		short fileID = 0;
		PAPPNODE pNode = NULL;
		int num = 0;

		fileID = AppGetIdFromIconName(pAdm, "/rom/progman/app/mstk_43x28.bmp");
		
		pNode = pAdm->pFileCache+fileID;
		num = GetDirChildNum(pAdm, pNode->fatherId);
		AppFileMoveEx(pAdm,fileID,pNode->iIndex,(short)num);	
	}
	if(GetSIMState()==0 || GetSIMInfoAct() == FALSE)
	{
		short fileID = 0;
		PAPPNODE pNode = NULL, pTempNode = NULL;
		PDIRSLINK pDir = NULL;
		PDIRSAPP pAppLink = NULL;

		fileID = AppGetIdFromIconName(pAdm, "/rom/progman/app/msiminfo_43x28.bmp");
		
		pNode = pAdm->pFileCache+fileID;
	
		pDir = AppDirsFind(pAdm->pDirRoot, pNode->fatherId);
		if(!pDir)
			return FALSE;
		pAppLink = pDir->pAppLink;
		
		while(pAppLink)
		{
			pTempNode = pAdm->pFileCache + pAppLink->appId;
			if(pTempNode->iIndex > pNode->iIndex)
				pTempNode->iIndex--;
			pAppLink = pAppLink->pNext;
		}
	 	
	}
	if(GetSIMState()==0 || GetMailNumAct() == FALSE)
	{
		short fileID = 0;
		PAPPNODE pNode = NULL, pTempNode = NULL;
		PDIRSLINK pDir = NULL;
		PDIRSAPP pAppLink = NULL;
	//	int num = 0;
	
		fileID = AppGetIdFromIconName(pAdm, "/rom/progman/app/msimmailbox_43x28.bmp");
		
		pNode = pAdm->pFileCache+fileID;
	//	num = GetDirChildNum(pAdm, pNode->fatherId);
	//	AppFileMoveEx(pAdm,fileID,pNode->iIndex,(short)num);
		pDir = AppDirsFind(pAdm->pDirRoot, pNode->fatherId);
		if(!pDir)
			return FALSE;
		pAppLink = pDir->pAppLink;
		
		while(pAppLink)
		{
			pTempNode = pAdm->pFileCache + pAppLink->appId;
			if(pTempNode->iIndex > pNode->iIndex)
				pTempNode->iIndex--;
			pAppLink = pAppLink->pNext;
		}
	}
	
	AppDirsSaveFile(pAdm);
	AppDirsSaveDirs(pAdm);
    return TRUE;
};
/********************************************************************
* Function   AppDirsGetNum(short dirID)  
* Purpose    get dirs num ;get chile numbers of the given dir
* Params     
* Return     
* Remarks      
**********************************************************************/
int AppDirsGetNum(short dirID)
{
    int nNum;
    PDIRSLINK  pDir;
    
    if(dirID < 0)
        return -1;
    
    if(!dirID)
    {
        //root dir
        nNum = AppDirsGetDirsChilds(pAppMain->pDirRoot, DIRS_SHOW );
        return nNum;
    }
    else
    {
        pDir = AppDirsFind ( pAppMain->pDirRoot, dirID );
        if(!pDir)
            return -1;
        nNum = AppDirsGetDirsChilds(pDir, DIRS_SHOW);
        return nNum;
    }
    
}
/********************************************************************
* Function   AppDirsShowName  
* Purpose    show dirs name display father dir name 
* Params     
* Return     
* Remarks      
**********************************************************************/
void AppDirsShowName(int fatherID, int iCurID, char *cBuf)
{
    int  nNum = 0, i = 0;
    PDIRSLINK 	pDirChild,pDirs;
    PDIRSNODE	pNode;
    
    if(fatherID < 0 )
        return;
    
    if(fatherID == 0)//0--root dir
    {
        nNum = AppDirsGetDirsChilds(pAppMain->pDirRoot, DIRS_SHOW );
        
        pDirChild = pAppMain->pDirRoot->pDirChild;
        while ( pDirChild )
        {
            pNode = AppGetDirNodeFromId(pAppMain, pDirChild->dirId, NULL);
            i++;

            if(iCurID == i - 1)  
            {
                strcpy(cBuf, pNode->aDisplayName); 
                return;
            }       
            pDirChild = pDirChild->pNext;
        }
    }
    else
    {
        pDirs = AppDirsFind( pAppMain->pDirRoot, (short)fatherID );
        nNum = AppDirsGetDirsChilds(pDirs, DIRS_SHOW );
        pDirChild = pDirs->pDirChild ;
        while ( pDirChild )
        {
            pNode = AppGetDirNodeFromId(pAppMain, pDirChild->dirId, pDirChild);
            i++;

            if(iCurID == i - 1)  
            {
                strcpy(cBuf, pNode->aDisplayName); 
                return;
            }       
            pDirChild = pDirChild->pNext;
        }
    }
}
/********************************************************************
* Function   AppGetDirNodeFromId  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
PDIRSNODE AppGetDirNodeFromId(struct appMain *pAdm, short dirId, struct DirsLink *pDirLink)
{
	PDIRSNODE pDirsNode = NULL;
	struct DirsLink *pDir;
	
	if(dirId < 0)
		return NULL;
	
	if(dirId < MAX_DIRS_NODE)
		pDirsNode = pAdm->pDirsCache + dirId;
	else
	{
		if(pDirLink)
			return(pDirLink->pDirNode);

		pDir = pAdm->pDirRoot->pDirChild;

		while(pDir)
		{
			if(pDir->dirId == dirId)
				return(pDir->pDirNode);

			pDir = pDir->pNext;
		}

	}
	return pDirsNode;
	
}
/********************************************************************
* Function   AppDirMoveDir  
* Purpose    Move app from one folder to another folder
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL AppDirMoveDir(struct appMain *pAdm,struct DirsLink *pMoveDir,BOOL bForWard, short dirId,
						  short srcIndex,short desIndex, int nCurType)
{
	PDIRSNODE  pDirsNode = NULL, pTempNode =NULL;
	struct DirsLink *pDirLink = NULL, *pDir = NULL,*pDirEnd = NULL, *p = NULL;
	BOOL bInsert = FALSE, bRemove = FALSE, bHaveDir =FALSE, bHead = FALSE, bTail = FALSE;

	if(!pMoveDir)
		return FALSE;

	pDir = pAdm->pDirRoot->pDirChild;

	if(!pDir)
		return FALSE;

	while(pDir)
	{
		if(pDir->dirId == dirId)  //  remove dirlink
		{
			if(pDir == pAdm->pDirRoot->pDirChild)
			{
				bHead = TRUE;
				pAdm->pDirRoot->pDirChild = pMoveDir->pNext;
				if(pMoveDir->pNext)
					pMoveDir->pNext->pPrev = NULL;
			}
			else
			{
				if(pDir->pPrev)
					pDir->pPrev->pNext = pMoveDir->pNext;	
				if(pDir->pNext)
				{
					pDir->pNext->pPrev = pMoveDir->pPrev;//?????
				}
				else
				{
				//	pDir = pMoveDir->pPrev;///???
					bTail = TRUE;
				}
			}
			pMoveDir->pPrev = NULL;
			pMoveDir->pNext = NULL;
			break;
		}
		pDir = pDir->pNext;
	}

	pDir = pAdm->pDirRoot->pDirChild;

	while(pDir)
	{
		
		pDirsNode = AppGetDirNodeFromId(pAdm, pDir->dirId, pDir);
		
		if(!pDirsNode)
			return FALSE;
	
		if(!bForWard && pDirsNode->iIndex>srcIndex && pDirsNode->iIndex<=desIndex) //insert dirlink
		{
			if(!bInsert && nCurType ==DIR_TYPE &&pDirsNode->iIndex == desIndex)//backward insert
			{
				pMoveDir->pNext = pDir->pNext;
				if(pDir->pNext)
					pDir->pNext->pPrev = pMoveDir;
				pDir->pNext = pMoveDir;
				pMoveDir->pPrev = pDir;
				bInsert = TRUE;
			}
			if(!bHaveDir && nCurType == APP_TYPE)
			{
				bHaveDir= TRUE;//dir node numbers between src node and dest node
			}
			pDirsNode->iIndex = pDirsNode->iIndex -1;
		}

		if(!bForWard && bHaveDir&& !bInsert && nCurType == APP_TYPE && pDirsNode->iIndex > desIndex)
		{
			if(!pDir->pPrev)
				pAdm->pDirRoot->pDirChild = pMoveDir;
			else
			{
				pDir->pPrev->pNext = pMoveDir;
				pMoveDir->pPrev = pDir->pPrev;
			}
			pMoveDir->pNext = pDir;
			pDir->pPrev = pMoveDir;
			
			bInsert = TRUE;
		}
	
		if(bForWard && pDirsNode->iIndex<srcIndex && pDirsNode->iIndex>=desIndex)
		{
			if(!bInsert && nCurType == DIR_TYPE && pDirsNode->iIndex == desIndex)// forward insert 
			{
				if(pDir->pPrev)
				{
					pDir->pPrev->pNext = pMoveDir;
					pMoveDir->pPrev = pDir->pPrev;
				}
				else
					pAdm->pDirRoot->pDirChild = pMoveDir;
				
				pDir->pPrev = pMoveDir;
				pMoveDir->pNext = pDir; 
				bInsert = TRUE;
			}
			
			if(!bInsert && nCurType == APP_TYPE && pDirsNode->iIndex > desIndex)
			{
				if(pDir->pPrev)
				{
					pDir->pPrev->pNext = pMoveDir;
					pMoveDir->pPrev = pDir->pPrev;
				}
				else
					pAdm->pDirRoot->pDirChild = pMoveDir;
			
				pMoveDir->pNext = pDir;
				pDir->pPrev = pMoveDir;
				bInsert = TRUE;
			}
			pDirsNode->iIndex = pDirsNode->iIndex +1;
			
		}	
		pDir = pDir->pNext;
	}

	if(!bInsert)
	{
		if(!bHaveDir)
		{
			if(bHead)//head node foward or backword£¬no dir node between src and dest
			{
				pDir = pAdm->pDirRoot->pDirChild;
				
				pAdm->pDirRoot->pDirChild = pMoveDir;
				pDir->pPrev=pMoveDir;
				pMoveDir->pNext = pDir;
			}
			else if(bTail)//tail node foward or backword£¬no dir node between src and dest
			{
				pDir = pAdm->pDirRoot->pDirChild;
				while(pDir->pNext)
				{
					pDir = pDir->pNext;
				}
				pDir->pNext=pMoveDir;
				pMoveDir->pPrev = pDir;
			}
			else  //no move£¬no dir node between src and dest
			{
				p = NULL;
				pDir = pDir = pAdm->pDirRoot->pDirChild;
				while(!bInsert && pDir)
				{
					pTempNode = AppGetDirNodeFromId(pAdm, pDir->dirId, pDir);
					if( pTempNode->iIndex > desIndex)
					{
						if(p)
							p->pNext = pMoveDir;
						else
							pDir = pAdm->pDirRoot->pDirChild = pMoveDir;
						pMoveDir->pPrev = p;
						pMoveDir->pNext = pDir;
						pDir->pPrev = pMoveDir;
						bInsert = TRUE;
					}
					
					p = pDir;
					pDir = pDir->pNext;
				}
			}
		
		}
		else //such condition only in backword move
		{
			pDir = pAdm->pDirRoot->pDirChild;
			while(pDir->pNext)
			{
				pDir = pDir->pNext;
			}
			pDir->pNext=pMoveDir;
			pMoveDir->pPrev = pDir;
		}
	}

	return TRUE;

}
/********************************************************************
* Function   AppDirMoveFile  
* Purpose    Move app in the same folder
* Params     bForWard  : 
* Return     
* Remarks      
**********************************************************************/
static BOOL AppDirMoveFile(struct appMain *pAdm, BOOL bForWard, short srcIndex,short desIndex)
{
	PAPPNODE  pAppNode = NULL;
	struct appLink *pAppLink = NULL;

	pAppLink = pAdm->pDirRoot->pAppLink;

	while(pAppLink)
	{
		pAppNode = pAdm->pFileCache + pAppLink->appId;

		if(!bForWard && pAppNode->iIndex > srcIndex && pAppNode->iIndex <= desIndex)
		{
			pAppNode->iIndex = pAppNode->iIndex - 1;
		}
		if(bForWard && pAppNode->iIndex >= desIndex && pAppNode->iIndex < srcIndex)
		{
			pAppNode->iIndex = pAppNode->iIndex + 1;
		}
		pAppLink = pAppLink->pNext;
	}

	return TRUE;
}
/********************************************************************
* Function   AppDirsMove  
* Purpose    move folder in mainmenu
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL AppDirsMove( struct appMain * pAdm, short dirId, short srcIndex, short desIndex , int nCurType)
{
	struct DirsLink   *pDir = NULL, *pTemp = NULL;
	       PDIRSNODE   pDirsNode = NULL;
	struct appLink    *pTempApp = NULL;
		   BOOL        bForWard = FALSE;
	
	if(srcIndex <0 || desIndex < 0)
		return FALSE;

	if(srcIndex == desIndex)
		return FALSE;

	if(srcIndex > desIndex)
		bForWard = TRUE;

	pDir = AppDirsFind(pAdm->pDirRoot, dirId);

	if(!pDir)
		return FALSE;

    pDirsNode = AppGetDirNodeFromId(pAdm, dirId, pDir);

	if(!pDirsNode)
		return FALSE;

	AppDirMoveDir(pAdm, pDir, bForWard, dirId, srcIndex, desIndex, nCurType);

    AppDirMoveFile(pAdm,bForWard, srcIndex,desIndex);
	
	pDirsNode->iIndex = desIndex;
	AppDirsSaveFile(pAdm);
    AppDirsSaveDirs(pAdm);

	return TRUE;
}
/********************************************************************
* Function   GetDirFromId  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
PDIRSLINK GetDirFromId(struct appMain * pAdm, short dirId)
{
	PDIRSLINK pDir = NULL;

	if(dirId < 0)
		return NULL;

	if(dirId == 0)
		return pAdm->pDirRoot;

	pDir = pAdm->pDirRoot->pDirChild;
	while(pDir)
	{
		if(pDir->dirId == dirId)
			return pDir;
		pDir = pDir->pNext;
	}
	return NULL;

}
/********************************************************************
* Function   GetDirChildNum  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
int  GetDirChildNum(struct appMain * pAdm, short dirId)
{
	PDIRSLINK pDir = NULL, pDirTemp =NULL;
	struct appLink* pApplist =NULL;
	int num = 0;
	
	if(dirId < 0)
		return 0;
	
	if(dirId == 0)
		pDir = pAdm->pDirRoot;
	else
	{
		pDir = pAdm->pDirRoot->pDirChild;
		while(pDir)
		{
			if(pDir->dirId == dirId)
				break;
			pDir = pDir->pNext;
		}
	}
	
	if(pDir == NULL)
		return 0;

	pDirTemp = pDir->pDirChild;
	pApplist = pDir->pAppLink;
	while(pDirTemp)
	{
		pDirTemp= pDirTemp->pNext;
		num++;
	}
	while(pApplist)
	{
		pApplist = pApplist->pNext;
		num++;
	}

	return num;
}
/********************************************************************
* Function   AppDirsTreeRelease  
* Purpose    Release AppDirs object, sucess return 1	
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppDirsTreeRelease ( PDIRSLINK pDir, BOOL bSelf )
{
    PDIRSAPP	pFile = NULL, pNext = NULL;
    PDIRSLINK	pTemp = NULL;
    
    if ( !pDir )
        return FALSE;
    
    if ( pDir->pDirChild )			//	delete children dir node
        AppDirsTreeRelease ( pDir->pDirChild, 0 );
    
    if ( pDir->pNext && !bSelf )	//	delete brother dir node
        AppDirsTreeRelease ( pDir->pNext, 0 );	
    
    pFile = pDir->pAppLink;
    while ( pFile )
    {
        pNext = pFile->pNext;
        if ( !AppFileRelease ( pDir->pAdm, pFile->appId) )
            return FALSE;
        free ( pFile );
        pFile = NULL;
        pFile = pNext;
    }
    
    AppDirsDeleteNode( pDir->pAdm, pDir->dirId );	//	release dirs node
    
    if ( NULL != pDir->pFather )
    {
        pTemp = pDir->pFather->pDirChild;			
        if ( pTemp == pDir )
        {
            pDir->pFather->pDirChild = pDir->pNext;	
        }
        else
        {
            while ( pTemp->pNext && pTemp->pNext != pDir )
                pTemp = pTemp->pNext;
            
            if ( pTemp->pNext == pDir )
            {
                pTemp->pNext = pDir->pNext;	
                if ( NULL != pDir->pNext )
                    pDir->pNext->pPrev = pTemp;
            }
        }
    }
    
    //save info
//	AppDirsSaveFile(pDir->pAdm);
   // AppDirsSaveDirs(pDir->pAdm);
    
    free ( pDir );
    pDir = NULL;
    return TRUE;
}
/********************************************************************
* Function   AppDirsGetDirsChilds  
* Purpose    Get the dir child number of AppDirs 	
* Params     
* Return     
* Remarks      
**********************************************************************/
int		AppDirsGetDirsChilds( struct DirsLink * pDir, int flags )
{	
    int			i = 0;
    PDIRSLINK 	pDirChild = NULL;
    PDIRSNODE	pNode = NULL;
    
    if ( !pDir || !pDir->pDirChild )
        return 0;
    
    pDirChild = pDir->pDirChild;
    
    while ( pDirChild )
    {
        switch ( flags )
        {
        case DIRS_ALL:
            
            i++;
            break;
        case DIRS_SHOW:
            pNode = AppGetDirNodeFromId(pDir->pAdm, pDirChild->dirId, pDirChild);
            if ( !pNode->flags)
                break;
            i++;
            break;
        }
        pDirChild = pDirChild->pNext;
    }
    return i;
};
/********************************************************************
* Function   AppDirsReleaseFile  
* Purpose    release one appfile from  AppDirs	
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL	AppDirsReleaseFile( struct appMain * pAdm , short dirID, short fileID )
{
    PDIRSAPP	pFileLink = NULL, pTemp = NULL;
    PDIRSLINK	pDir = NULL;
    
    pDir = AppDirsFind( pAdm->pDirRoot, dirID );
    if ( !pDir )
        return FALSE;
    
    pFileLink  = pDir->pAppLink;
    if ( !pFileLink )
        return FALSE;
    
    while ( pFileLink->pNext && pFileLink->appId != fileID )
    {
        pTemp = pFileLink;
        pFileLink= pFileLink->pNext;
    }
    
    if ( pFileLink->appId != fileID )	
        return FALSE;
    
    if ( pFileLink == pDir->pAppLink )
        pDir->pAppLink = pFileLink->pNext;
    else
        pTemp->pNext = pFileLink->pNext;
    
    return AppFileRelease ( pAdm, fileID );//	only return true
}
/********************************************************************
* Function   AppDirsGetFileChilds  
* Purpose    Get the file child number of appDirs 
* Params     
* Return     
* Remarks      
**********************************************************************/
int		AppDirsGetFileChilds( struct DirsLink * pDir, int flags )
{	
    int			i = 0;
    PDIRSAPP	pFileLink = NULL;
    PAPPNODE	pNode = NULL;
    
    if ( !pDir || !pDir->pAppLink )
        return 0;
    
    pFileLink = pDir->pAppLink;
    
    while ( pFileLink )
    {
        switch (flags)
        {
        case DIRS_ALL:         
            i++;
            break;
        case DIRS_SHOW:
            
            pNode = pDir->pAdm->pFileCache+pFileLink->appId;
            if (!pNode->sFlag || (pNode->sFlag & APP_NODE_HIDE))
                break;
            i++;
            break;
        }
        pFileLink = pFileLink->pNext;
    }
    return i;
};
