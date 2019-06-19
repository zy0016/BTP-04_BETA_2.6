/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : PIM
 *
 * Purpose  : sound application developer's API
 *            
\**************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"
#include "dirent.h"
#include "fcntl.h"
#include "pmi.h"
#include "plx_pdaex.h"
#include "SndUsrApi.h"

// get the space (in bytes) occupied by a folder
long StatFolder(PCSTR pFolderPath)
{
	struct dirent *dirinfo = NULL;
	struct stat	filestat;

	int  nLen;
	long nRtnSize;	
	char FullName[128];
	
	DIR  *dirtemp = NULL;

	if (NULL == pFolderPath)
	{
		return 0;
	}

    nRtnSize = 0;

	dirtemp = opendir(pFolderPath);

	if(dirtemp == NULL)
		return 0;
	
	memset(FullName, 0, 128);
	strcpy ( FullName, pFolderPath );
	if (!ISPATHSEPARATOR(FullName[strlen(FullName) - 1]))
		strcat(FullName, "/");

	nLen = strlen(FullName);

	while((dirinfo = readdir(dirtemp))!=NULL)
	{
		if(ISPRESERVEDPATH(dirinfo->d_name))
			continue;

		FullName[nLen] = '\0';
		strcat ( FullName, dirinfo->d_name);
		stat(FullName,&filestat);

		if ( (!S_ISLNK(filestat.st_mode)) && S_ISDIR(filestat.st_mode) )	// sub folder
		{			
			nRtnSize += StatFolder(FullName);
		}
		else if(S_ISREG(filestat.st_mode))	// common file
			nRtnSize += filestat.st_size;

		KickDog();	// clear the watch dog
	}
	closedir(dirtemp);
	return nRtnSize;
}

#define SNDCOPYVOLUME	256	// copies so many bytes once

// copy file api
BOOL SND_CopyFile(const char *szSrc, const char *szDst)
{
	FILE *fSrc = NULL;
	FILE *fDst = NULL;
	BYTE *pBuf = NULL;
	long lRead = -1;
	long lWrite = -1;
	BOOL bRet = FALSE;

	fSrc = fopen(szSrc, "rb");
	if(NULL == fSrc)
		return FALSE;

	pBuf = (BYTE*)malloc(SNDCOPYVOLUME);
	if(NULL == pBuf)
	{
		fclose(fSrc);
		return FALSE;
	}

	fDst = fopen(szDst, "wb");
	if(NULL == fDst)
	{
		fclose(fSrc);
		free(pBuf);
		return FALSE;
	}

	lRead = fread(pBuf, 1, SNDCOPYVOLUME, fSrc);
	while(lRead > 0)
	{
		if(lRead > SNDCOPYVOLUME)
			break;

		lWrite = fwrite(pBuf, 1, lRead, fDst);
		if(lRead != lWrite)
			break;

		if(SNDCOPYVOLUME > lRead)
		{
			bRet = TRUE;
			break;
		}
		lRead = fread(pBuf, 1, SNDCOPYVOLUME, fSrc);
	}

	free(pBuf);
	fclose(fSrc);
	fclose(fDst);

	if(!bRet)
		remove(szDst);

	return bRet;
}
#undef SNDCOPYVOLUME

// move file api
BOOL SND_MoveFile(const char *szSrc, const char *szDst)
{
	if(!SND_CopyFile(szSrc, szDst))
		return FALSE;

	chmod(szSrc, 0666);
	remove(szSrc);
	return TRUE;
}

// get the size of a file (in kB)
long SND_GetFileSize(const char *szPath)
{
	struct stat	filestat;
	if(stat(szPath, &filestat) != 0)
		return -1;
	return (long)(filestat.st_size/1024);
}

// insert menu by position (avoiding inserting items which exists) 
BOOL SND_InsertMenu(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem)
{
//	int nFlag = -1;
//	nFlag = (int)GetMenuState(hMenu, IDC_CREATEFOLDER, MF_BYCOMMAND);
//	
//	if(-1 == nFlag)
	DeleteMenu(hMenu, uIDNewItem, MF_BYCOMMAND);
	return InsertMenu(hMenu, uPosition, uFlags, uIDNewItem, lpNewItem);

	return TRUE;
}

// get folder name from full path
BOOL SND_GetFolderName(const char *szPath, char *pName)
{
	int i, j;

	if(NULL == szPath || NULL == pName)
		 return FALSE;

	i = strlen(szPath) - 1;
	while(ISPATHSEPARATOR(szPath[i]))
	{
		i --;
		if(i <= 0)
			return FALSE;
	}
//	if(0 >= i)
//		return FALSE;

	j = i;
	while(!ISPATHSEPARATOR(szPath[i]))
	{
		i--;
		if(i <= 0)
			return FALSE;
	}

	if(j-i > 63)
		j = i+63;
	
	strncpy(pName, (char*)(szPath+i+1), j-i);
	pName[j-i] = '\0';

	return TRUE;
}

// change to the sub dir
BOOL SND_EnterSubFolder(char *szPath, const char *szFolder)
{
	int nlen;

	if(NULL == szPath || NULL == szFolder)
		return FALSE;

	nlen = strlen(szPath) - 1;

	if(!ISPATHSEPARATOR(szPath[nlen]))
		strcat(szPath, "/");

	strcat(szPath, szFolder);

	nlen = strlen(szPath) - 1;

	if(!ISPATHSEPARATOR(szPath[nlen]))
		strcat(szPath, "/");

	return TRUE;
}

// check if a certain file or folder exists
BOOL SND_IsItemExist(const char *szPath, BOOL isFile)
{
	if(NULL == szPath)
		return FALSE;

	if(isFile)	// file
	{
		FILE *fp = NULL;
		fp = fopen(szPath, "rb");
		if(NULL == fp)
		{
			printf("\r\n[sound]: testing file name: <%s>, NOT found\r\n", szPath);
			return FALSE;
		}
		
		fclose(fp);
		printf("\r\n[sound]: testing file name: <%s>, found!\r\n", szPath);
		return TRUE;
	}
	else		// folder
	{
		DIR *dp = NULL;
		dp = opendir(szPath);
		if(NULL == dp)
		{
			printf("\r\n[sound]: testing folder name: <%s>, NOT found\r\n", szPath);
			return FALSE;
		}

		closedir(dp);
		printf("\r\n[sound]: testing folder name: <%s>, found!\r\n", szPath);
		return TRUE;
	}
}

// check if a folder is empty
BOOL SND_IsFolderEmpty(PCSTR pFolderPath)
{
	struct dirent *dirinfo = NULL;
	
	DIR  *dirtemp = NULL;

	if (NULL == pFolderPath)
		return TRUE;

	dirtemp = opendir(pFolderPath);

	if(dirtemp == NULL)
		return TRUE;
	

	while((dirinfo = readdir(dirtemp))!=NULL)
	{
		if(ISPRESERVEDPATH(dirinfo->d_name))
			continue;

		// there's something inside the folder
		closedir(dirtemp);
		return FALSE;
	}
	closedir(dirtemp);
	return TRUE;
}

/*********************************************************************\
* Function		SND_IsFolder
* Purpose		check if a string is a folder's path
* Params		szPath	path of the folder to be checked
* Return		TRUE when it is a folder, FALSE when it isn't.
* Remarks		
**********************************************************************/
BOOL SND_IsFolder(const char *szPath)
{
	struct stat xstat;
	if(NULL == szPath)
		return FALSE;

	if(stat(szPath, &xstat) != 0)
		return FALSE;

	return (S_ISDIR(xstat.st_mode) && !S_ISLNK(xstat.st_mode));
}

/*********************************************************************\
* Function		SND_SetListBoxFocus
* Purpose		set focus to a list box (and set selection if needed)
* Params		HWND hListWnd	handle of the listbox
* Return		handle of the previous window with focus
* Remarks		when there's nothing in the listbox, nothing will be done
**********************************************************************/
HWND SND_SetListBoxFocus (HWND hListWnd)
{
	if(NULL == hListWnd)
		return NULL;

	if(!IsWindow(hListWnd))
		return NULL;

	if(SendMessage(hListWnd, LB_GETCOUNT, 0, 0) <= 0)
		return NULL;

	if(SendMessage(hListWnd, LB_GETCURSEL, 0, 0) < 0)
		SendMessage(hListWnd, LB_SETCURSEL, 0, 0);
	
	return SetFocus(hListWnd);
}

/*********************************************************************\
* Function		SND_IsInFolder
* Purpose		check if an item is in a folder
* Params		PCSTR szFolder	path of the folder
*				PCSTR szTarget	path of the item to be checked
* Return		0	if the two paths are the same
*				1	when the target is a sub folder of the parent
*					or a file in the parent folder
*				-1	in other cases.
* Remarks		this function doesn't check if the two strings
*				are valid paths.
**********************************************************************/
int SND_IsInFolder(PCSTR szFolder, PCSTR szTarget)
{
	int nLen1, nLen2;
	
	if(NULL == szFolder || NULL == szTarget)
		return -1;

	if(strcasecmp(szFolder, szTarget) == 0)	// sure
		return 0;
	
	nLen1 = strlen(szFolder);
	nLen2 = strlen(szTarget);

	if(ISPATHSEPARATOR(szFolder[nLen1-1]))
	{
		if(strncasecmp(szFolder, szTarget, nLen1-1) == 0)
		{
			if(nLen2+1 == nLen1)	// identical
				return 0;
			else if(nLen2 < nLen1)	// not in folder
				return -1;		
			else if(ISPATHSEPARATOR(szTarget[nLen1-1]))
			{
				if(nLen1 == nLen2)	// identical
					return 0;
				return 1;			// in folder
			}
		}
		else	// definitely not in folder
			return -1;
	}
	else
	{
		if(strncasecmp(szFolder, szTarget, nLen1) == 0)
		{
			if(nLen2 <= nLen1)
				return -1;
			else if(ISPATHSEPARATOR(szTarget[nLen1]))
			{
				if(nLen1+1 == nLen2)	// identical
					return 0;
				else					// in folder
					return 1;
			}
		}
		else	// definitely not in folder
			return -1;
	}
	return -1;
}

/*********************************************************************\
* Function		SND_AddListBoxItem
* Purpose		add a string into a list box
* Params		HWND	hListBox	Handle of the list box
*				PCSTR	pStr		string to be added
* Return		>=0		the actual index of the added string
*				<0		when error occurs
* Remarks		it performs the same as the "LB_ADDSTRING" message,
*				but it converts the string into multibytes.
**********************************************************************/
int SND_AddListBoxItem(HWND hListBox, PCSTR pStr)
{
	int nRet = -8;

#ifdef _SOUND_UTF8NAME_
	char *pTarget = NULL;
#endif

	if((NULL == pStr) || (NULL == hListBox))
		return -8;
	
#ifdef _SOUND_UTF8NAME_
	nRet = UTF8ToMultiByte(CP_ACP, 0, pStr, -1, NULL, 0, NULL, NULL);
	pTarget = (char *)malloc(nRet+1);
	if(NULL == pTarget)
		return -8;

	memset(pTarget, 0, nRet+1);
	UTF8ToMultiByte(CP_ACP, 0, pStr, -1, pTarget, nRet+1, NULL, NULL);

	nRet = SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pTarget);

	free(pTarget);
#else
	nRet = SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pStr);
#endif
	
	return nRet;
}

/*********************************************************************\
* Function		SND_SetWindowText
* Purpose		set window's text
* Params		HWND	hWnd	Handle of the window
*				PCSTR	pStr	string to be set
* Return		TRUE	when success
*				FALSE	when fail
* Remarks		it performs the same as the "SetWindowText" function,
*				but it converts the string into multibytes.
**********************************************************************/
BOOL SND_SetWindowText(HWND hWnd, PCSTR pStr)
{
	int nRet = -1;

#ifdef _SOUND_UTF8NAME_
	char *pTarget = NULL;
#endif

	if((NULL == pStr) || (NULL == hWnd))
		return FALSE;
	
#ifdef _SOUND_UTF8NAME_
	nRet = UTF8ToMultiByte(CP_ACP, 0, pStr, -1, NULL, 0, NULL, NULL);
	pTarget = (char *)malloc(nRet+1);
	if(NULL == pTarget)
		return FALSE;

	memset(pTarget, 0, nRet+1);
	UTF8ToMultiByte(CP_ACP, 0, pStr, -1, pTarget, nRet+1, NULL, NULL);

	nRet = SetWindowText(hWnd, pTarget);

	free(pTarget);
#else
	nRet = SetWindowText(hWnd, pStr);
#endif
	
	return (BOOL)nRet;
}

/*********************************************************************\
* Function		SND_GetWindowText
* Purpose		get window's text
* Params		HWND	hWnd		Handle of the window
*				PSTR	pStr		buffer to get the text
*				int		nMaxCount	max length to get
* Return		If the function succeeds, the return value is the length, 
*				in characters, of the copied string, not including the 
*				terminating null character. 
*				If the window has no title bar or text, if the title bar is empty, 
*				or if the window or control handle is invalid, the return value is zero. 
* Remarks		it performs the same as the "GetWindowText" function,
*				but it converts the string into UTF-8.
**********************************************************************/
int SND_GetWindowText(HWND hWnd, PSTR pStr, int nMaxCount)
{
	int nRet = -1;

#ifdef _SOUND_UTF8NAME_
	char *pTarget = NULL;
#endif

	if((NULL == pStr) || (NULL == hWnd))
		return 0;

#ifdef _SOUND_UTF8NAME_
	nRet = GetWindowTextLength(hWnd);
	if(nRet < 0)
		return 0;

	pTarget = (char*)malloc(nRet+1);
	if(NULL == pTarget)
		return 0;
	
	nRet = GetWindowText(hWnd, pTarget, nRet+1);
	if(nRet <= 0)
	{
		free(pTarget);
		return 0;
	}

	nRet = MultiByteToUTF8(CP_ACP, 0, (LPCSTR)pTarget, -1, NULL, 0);
	if(nRet < 0 || nRet >  nMaxCount)
		return 0;

	MultiByteToUTF8(CP_ACP, 0, (LPCSTR)pTarget, -1, (LPSTR)pStr, nMaxCount);

	free(pTarget);
#else
	nRet = GetWindowText(hWnd, pStr, nMaxCount);
#endif
	
	return TRUE;
}

/*********************************************************************\
* Function		SND_MultibyteStrcpy
* Purpose		
* Params		
*				
*				
* Return		
*				
*				
*				
* Remarks		
*				
**********************************************************************/
char *SND_MultibyteStrcpy(PSTR pDst, PCSTR pSrc)
{
	char *pRet = NULL;
	
#ifdef _SOUND_UTF8NAME_
	int nRet = -1;
	char *pTarget = NULL;
#endif

	if((NULL == pSrc) || (NULL == pDst))
		return NULL;

#ifdef _SOUND_UTF8NAME_
	nRet = UTF8ToMultiByte(CP_ACP, 0, pSrc, -1, NULL, 0, NULL, NULL);
	pTarget = (char *)malloc(nRet+1);
	if(NULL == pTarget)
		return NULL;

	memset(pTarget, 0, nRet+1);
	UTF8ToMultiByte(CP_ACP, 0, pSrc, -1, pTarget, nRet+1, NULL, NULL);

	pRet = strcpy(pDst, pTarget);
	free(pTarget);
#else
	pRet = strcpy(pDst, pSrc);
#endif
	
	return pRet;
}

// check if a file name contains invalid characters
#define SNDINVALIDCHAR	"\\\"/?*<>|:"
BOOL SND_IsInvalidFileName(PCSTR szName)
{
	int i;
	static const char szBadChar[16] = SNDINVALIDCHAR;

	if(NULL == szName)
		return FALSE;
	
	if(szName[0] == 0x20)
		return TRUE;

	for(i=strlen(szName)-1; i>=0; i--)
	{
		if(szName[i] < 0x20)
			return TRUE;
		
		if(strchr(szBadChar, szName[i]))
			return TRUE;
	}

	return FALSE;
}
#undef SNDINVALIDCHAR

BOOL SND_ShowListBox (HWND hWnd)
{
	int nCount;

	if(NULL == hWnd)
		return FALSE;

	nCount = SendMessage(hWnd, LB_GETCOUNT, 0, 0);
	if(nCount > 0)
		ShowWindow(hWnd, SW_SHOW);
	else
		ShowWindow(hWnd, SW_HIDE);

	return TRUE;
}
