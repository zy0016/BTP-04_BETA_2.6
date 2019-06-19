/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : TFTP Server Module
 *
 * Purpose  : TFTP server api layer
 *            
\**************************************************************************/

#include "window.h"    
#include "device.h"  
#include "fapi.h"
//#include "stdlib.h"


HANDLE	ts_CreateFile(PCSTR pFileName, DWORD dwMode, DWORD dwAttrib);
BOOL	ts_CloseFile(HANDLE hFile);
DWORD	ts_ReadFile(HANDLE hFile, PVOID pBuf, DWORD nBytes);
DWORD	ts_WriteFile(HANDLE hFile, PVOID pBuf, DWORD nBytes);
BOOL	ts_MoveFile(PCSTR pOldFileName, PCSTR pNewFileName);
BOOL	ts_DeleteFile(PCSTR pFileName);
HANDLE	ts_FindFirstFile(PCSTR lpFileName, P_FIND_DATA pFindData);
BOOL	ts_FindNextFile(HANDLE hFindFile, P_FIND_DATA pFindData);
BOOL	ts_FindClose(HANDLE hFindFile);
DWORD	ts_GetFileSize(HANDLE hFile);
int		ts_SetFilePointer(HANDLE hFile, LONG lDistance, DWORD dwMode);

// Define FileSys Operate Function
HANDLE	ts_CreateFile(PCSTR pFileName, DWORD dwMode, DWORD dwAttrib)
{
	return CreateFile( pFileName,  dwMode,  dwAttrib);
}
BOOL	ts_CloseFile(HANDLE hFile)
{
	return CloseFile( hFile);
}
DWORD	ts_ReadFile(HANDLE hFile, PVOID pBuf, DWORD nBytes)
{
	return ReadFile( hFile,  pBuf,  nBytes);
}
DWORD	ts_WriteFile(HANDLE hFile, PVOID pBuf, DWORD nBytes)
{
	return WriteFile( hFile,  pBuf,  nBytes);
}
BOOL	ts_MoveFile(PCSTR pOldFileName, PCSTR pNewFileName)
{
	return MoveFile( pOldFileName,  pNewFileName);
}
BOOL	ts_DeleteFile(PCSTR pFileName)
{
	return DeleteFile( pFileName);
}
HANDLE	ts_FindFirstFile(PCSTR lpFileName, P_FIND_DATA pFindData)
{
	return FindFirstFile(lpFileName, pFindData);
}
BOOL	ts_FindNextFile(HANDLE hFindFile, P_FIND_DATA pFindData)
{
	return FindNextFile( hFindFile,  pFindData);
}
BOOL	ts_FindClose(HANDLE hFindFile)
{
	return FindClose( hFindFile);
}
DWORD	ts_GetFileSize(HANDLE hFile)
{
	return GetFileSize( hFile);
}
int		ts_SetFilePointer(HANDLE hFile, LONG lDistance, DWORD dwMode)
{
	return SetFilePointer(hFile, lDistance, dwMode);
}

// End of program
