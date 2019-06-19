 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : application manager $ version 3.0
 *
 * Purpose  : 
 *
\**************************************************************************/

#include	"window.h"
#include    "path.h"
#include	"osver.h"

#if (_HOPEN_VER >= 300)
#include "sys/types.h"
#include "sys/stat.h"
#include "ctype.h"
#include "fcntl.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dirent.h"
#include "unistd.h"

#else
#include    "string.h"
#include    "stdlib.h"
#include    "fapi.h"
#endif

#include "sys/mount.h"

#if (_HOPEN_VER >= 300)
BOOL  CreateAppPathInFlash()
{  
   
    register   int  i = 0, j = 0;
    char        strPathName[64] = "", *strChildName;    
    DIR         *pDir;
    struct  dirent *pDirEntry;
    BOOL    bExist = FALSE;;
    
	PLXPrintf("CreateAppPathInFlash 30\r\n");

    while (	AppPath[i] != NULL	&& AppPath[i]->achName != NULL)
    {
        if (chdir("/mnt/flash/") != 0)
        {  
            PLXPrintf("chdir false!\r\n");
            return FALSE;
        }
        strcpy(strPathName, "/mnt/flash");
        
        pDir = opendir(strPathName);
        if (!pDir)
        {
            return FALSE;
        }
        
        bExist = FALSE;
        
        while ((pDirEntry = readdir(pDir)) != 0)
        {        
            if (strcmp(AppPath[i]->achName, pDirEntry->d_name) == 0)
            {
                bExist = TRUE;
                break;
            }
        }
        
        closedir(pDir);
        
        strcat(strPathName, "/");
        strcat(strPathName, AppPath[i]->achName);
        
        if (!bExist)
        {
            if (mkdir(strPathName,  0x666) != 0)
                return FALSE;
        }
        
        j = 0;
        
        if ((char**)AppPath[i]->strChildName != NULL
			&& *(char**)AppPath[i]->strChildName != NULL)
        {
            if (chdir(strPathName) != 0)
                return FALSE;
            
            pDir = opendir(strPathName);
            if (!pDir)
            {
                return FALSE;
            }
            
          //  strChildName = *(char**)AppPath[i]->strChildName[0];
			strChildName = *(char**)AppPath[i]->strChildName;//xlzhu
            
            while(strChildName != NULL)
            {
                bExist = FALSE;
                
                while ((pDirEntry = readdir(pDir)) != 0)
                {        
                    if (strcmp(strChildName, pDirEntry->d_name) == 0)
                    {
                        bExist = TRUE;
                        break;
                    }
                }
                
                if (!bExist)    
                {
                    if (mkdir(strChildName, 0x666) != 0)
                    {
                        closedir(pDir);
                        return FALSE;
                    }
                }
                
                j++;
                strChildName = *(char**)(AppPath[i]->strChildName + j);
                rewinddir(pDir);
            }
            closedir(pDir);
        }        
        i++;
    }    
    
    return TRUE;
}
/********************************************************************
* Function   MMC_CheckCardStatus 
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
#define		MMC_PATH_LEN	10
#define PMIS_MMC_PATH   "/mnt/fat16/"
#define PMIA_INI_MMC    0x0001
#define PMIA_HAS_MMC    0x0002
static  DWORD   g_nAttrib;
BOOL    MMC_CheckCardStatus(void)
{
   
    DIR *       hDir;
	struct      dirent* pDir;
	register    int i, j;
	char        strcurpath[128];
	char		aPath[][MMC_PATH_LEN] = 
	{
		"\0txt",
        "\0audio",
        "\0pictures",
	};
	
	if ( g_nAttrib & PMIA_HAS_MMC )
	{
		printf("\r\n have sd card!");
		return  TRUE;
	}
	
	if ( g_nAttrib & PMIA_INI_MMC )
	{
		printf("\r\n already init sd card!");
		return  FALSE;
	}
	
	g_nAttrib |= PMIA_INI_MMC;
	
	if ( 0 != mount("/dev/mmc1","/mnt/fat16","fatfs", 0, NULL) )
	{
		printf("\r\n mount sd card fail!");
		return	FALSE;
	}
	
	memset(strcurpath, 0, 128);
	getcwd(strcurpath, 128);
	if ( 0 != chdir(PMIS_MMC_PATH) )// zero behalf success
	{
		printf("\r\n chdir failure!");
		return FALSE;
	}
	
	hDir = opendir(PMIS_MMC_PATH);
	if ( NULL == hDir )
	{
		if(strcurpath[0] != 0)
			chdir(strcurpath);

		printf("\r\n open dir failure!");
		return  FALSE;    
	}
	
	g_nAttrib |= PMIA_HAS_MMC;
	
	pDir = readdir(hDir);
	j = sizeof(aPath)/MMC_PATH_LEN;
    
	while ( NULL != pDir )
	{
		for ( i = 0; i <j ; i++)
		{
			if ( '\0' != aPath[i][0] )
			{
				i++;
				continue;
			}
			if ( 0 == strcmp(pDir->d_name, &aPath[i][1] ) )
			{
				aPath[i][0] = '1';
				i++;
				continue;
			}
			i++;
		}
		pDir = readdir(hDir);
	}
	for ( i = 0; i < j; i++)
	{
		if ( '\0' != aPath[i][0] )
		{
			i ++;
			continue;
		}
		mkdir(&aPath[i][1], 666);
		i++;
	}
	rewinddir(hDir);

	if(strcurpath[0] != 0)
		chdir(strcurpath);
	closedir(hDir);

	return TRUE;

}
#else
BOOL  CreateAppPathInFlash()
{  	
    register    int i = 0, j = 0;
    char        oldDir[64] = "", strPathName[64] = "", *strChildName;
    P_FIND_DATA pFindData;
    HANDLE      hFindFile;    
    
    GetCurrentDirectory(oldDir, 64);
    
    pFindData = (P_FIND_DATA)malloc(sizeof(_FIND_DATA));
    if (!pFindData)
        return FALSE;
    
    while (	AppPath[i] != NULL	)
    {
        if (!SetCurrentDirectory("FLASH2:"))
        {            
            free(pFindData);
            SetCurrentDirectory(oldDir);
            return FALSE;
        }
        
        strcpy(strPathName, "FLASH2:");
        
        if (AppPath[i]->achName)
        {
            hFindFile = FindFirstFile(AppPath[i]->achName, pFindData);
            
         
            if (hFindFile == INVALID_HANDLE_VALUE) 
            {  
                if (!CreateDirectory(AppPath[i]->achName))
                {
                    free(pFindData);
                    SetCurrentDirectory(oldDir);
                    return FALSE;
                }                
            }
            
            FindClose(hFindFile);
            
            strcat(strPathName, "/");
            strcat(strPathName, AppPath[i]->achName);            
            if (!SetCurrentDirectory(strPathName))
            {
                free(pFindData);
                SetCurrentDirectory(oldDir);
                return FALSE;
            }
            
            j = 0;
            
            if (AppPath[i]->strChildName != NULL)
            {            
                strChildName = (char*)AppPath[i]->strChildName[j];
                
                while(strChildName != NULL)
                {
                    hFindFile = FindFirstFile(strChildName, pFindData);            
                    if (hFindFile == INVALID_HANDLE_VALUE)
                    {                
                        if (!CreateDirectory(strChildName))
                        {
                            free(pFindData);
                            SetCurrentDirectory(oldDir);
                            return FALSE;
                        }
                    }
                    
                    FindClose(hFindFile);
                    
                    j++;
                    strChildName = (char*)AppPath[i]->strChildName[j];
                }
            }
        }
        
        i++;
    }
    
    free(pFindData);
    SetCurrentDirectory(oldDir);
    return TRUE;
}
#endif
