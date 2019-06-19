/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef	_MMS_COMMON_H
#define _MMS_COMMON_H

#define MIN_FREE_SPACE		200

/* file time */
typedef struct _FILETIME {	// ft 
    DWORD		dwLowDateTime;	//
    DWORD		dwHighDateTime;	// 
}FILETIME, *PFILETIME, *LPFILETIME; 

BOOL	MMS_IsFlashEnough(void);
BOOL    MMS_GetFileTime(char *pFileName, FILETIME *pFileTime);

void	MMS_CreateFileName(PSTR pszFileName, const int nType, const int nFolder);
BOOL	MMS_DeleteFile(PCSTR filename);
BOOL	RenameMms(char *srcname, const int nType, const int nFolder);
BOOL	MMS_IsMyfile(int hFile);
int		MMS_GetFileType(const char *pFileName);
int		MMS_GetFileFolder(const char *pFileName);
int		MMS_GetFileMmft(const char *pFileName);
int		MMS_WriteHead(int hMmsFile, BYTE bDelivery);
int		MMS_ParseFileName(PCSTR pAll, PSTR pFileName, PSTR pPathName);

void	MMS_EditTitle(char* pszTitle, int nLen);
char*	FindContentByKey(const char *pBuf, const char *pField, int *pLen);
BOOL	MMS_IsChinese(const BYTE c);
void	MMS_SetPageNum(HWND hWnd, const char *str, int curSlide, int tNum);
BOOL	IsLegalChar(BYTE c);
BOOL	IsLegalFileName(PCSTR  pName);

void MMS_FTtoST(FILETIME* pFileTime, SYSTEMTIME* pSysTime);
void MMS_STtoFT(SYSTEMTIME* pSysTime, FILETIME* pFileTime);


int		MMS_SysTimeToStr(SYSTEMTIME sysTime, char *strTime);
void	MMS_StrToSysTime(char *strTime, SYSTEMTIME *sysTime);
void	MMS_GreenwichToLocal(PSYSTEMTIME pTime, int timeDef);
char*	MMS_strtok (char * string, const char * control );
char*	MMS_chrtok (char * string, const char control1,  const char control2);
int		PhonenumToNameTitle(char *strPhonenum, char *strName, int nLen);


void	MMS_ProcessListBoxScroll(HWND hParent, HWND hList, int num);
void	MMS_WaitWindowState(HWND hwnd, BOOL bShow);
void    MMS_InsertWindowState(HWND hwnd, BOOL bShow);
void	NullMsg(char *a, char *b, int c);
int		MMS_GetUsedSpace(void);
int     MMS_StringCut(char * pStr, int nCutLen);
int     GetFileNameSuffix(const PSTR pFileName, PSTR pSuffix);
BOOL    MMS_ModifyType(char *srcname, const int nFileType, int nFolder);
BOOL    MMS_ModifyFolder(char *srcname, const int nFileType);
BOOL MMS_ModifyResend(char *srcname, BOOL bResend);

BOOL    MMS_CloseFile(int  hFile);
int     MMS_CreateFile(PCSTR pFileName, DWORD dwMode);
char*   My_StrDup( const char* SrcStr );
int     MMS_GetFileSuffix(PCSTR pFileName, PSTR pSuffix);
int		MMS_GetNameWithoutSuffix(PCSTR pFull, PSTR pName);
extern int APP_GetMMSSize(void);
int     MMS_FormatStringNoRtn(char* pString);
int     MMS_GetFileSize(const char*pFilename);
BOOL    MMS_CopyFile(PCSTR pOldFileName, PCSTR pNewFileName);
BOOL    MMS_MoveFile(PCSTR pOldFileName, PCSTR pNewFileName);
BOOL MMS_IsEmail(const char* pString);

static int IsFlashAvailable(int nSize);
#endif
