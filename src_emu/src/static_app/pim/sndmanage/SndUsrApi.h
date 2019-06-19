/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : PIM
 *
 * Purpose  : sound application api head
 *            
\**************************************************************************/

#ifndef _SOUND_USER_API_
#define _SOUND_USER_API_

/*
#ifndef _SOUND_UTF8NAME_
#define _SOUND_UTF8NAME_	// File names are utf-8 codes
#endif
//*/
#undef  _SOUND_UTF8NAME_	// File names are local codes

#define	MAXSNDFOLDERLEN		480
#define	MAXSNDPATHLENGTH	512
#define ISPATHSEPARATOR(c)	(('/'==(c))||('\\'==(c)))
#define ISPRESERVEDPATH(s)	((strcmp((s),".")==0)||(strcmp((s),"..")==0))

long StatFolder(PCSTR pFolderPath);
BOOL SND_CopyFile(const char *szSrc, const char *szDst);
BOOL SND_MoveFile(const char *szSrc, const char *szDst);
BOOL SND_GetFolderName(const char *szPath, char *pName);
BOOL SND_EnterSubFolder(char *szPath, const char *szFolder);
BOOL SND_InsertMenu(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem);
BOOL SND_IsItemExist(const char *szPath, BOOL isFile);
BOOL SND_IsFolderEmpty(PCSTR pFolderPath);
long SND_GetFileSize(const char *szPath);
BOOL SND_IsFolder(const char *szPath);
HWND SND_SetListBoxFocus (HWND hListWnd);
int	 SND_IsInFolder(PCSTR szFolder, PCSTR szTarget);
BOOL SND_IsInvalidFileName(PCSTR szName);

// coverting between UTF-8 and MultiByte
int  SND_AddListBoxItem(HWND hListBox, PCSTR pStr);
BOOL SND_SetWindowText(HWND hWnd, PCSTR pStr);
int  SND_GetWindowText(HWND hWnd, PSTR pStr, int nMaxCount);
BOOL SND_ShowListBox (HWND hWnd);
char *SND_MultibyteStrcpy(PSTR pDst, PCSTR pSrc);

#endif	// _SOUND_USER_API_
