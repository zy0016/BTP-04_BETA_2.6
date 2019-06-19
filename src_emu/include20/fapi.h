/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	FILE SYSTEM
 *
 * Purpose :	Declare file API functions
 *  
 * Author  :    Ling Di
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /Hopen/include/fapi.h                                 $
 * $Workfile:: fapi.h                                               $
 * $Revision:: 13 $     $Date:: 03-04-04 2:44p                      $
 * 
\**************************************************************************/
/***************************************************************************
 *
 * Program for File System.
 * 
 * 9     02-10-17 9:09a Ykfeng
 * 
 * 8     01-11-16 15:18 Dling
 * 1. Add definition of SYSTEMTIME
 * 2. Add FTtoST and STtoFT
 * 3. Add DetacheFileSystem
 * 
 * 7     01-04-05 9:36 Dling
 * 
 * 6     00-08-09 15:00 Dling
 * add for Java VM
 * 
 * 5     00-07-13 10:11 Dling
 * bak for stb product by ZhangBin
 * 
 * 4     00-01-18 15:44 Dling
 * remove defination of SYSTEMTIME
 * 
 * 3     00-01-07 15:17 Dling
 * change _MAX_FNAME
 * 
 * 2     99-11-29 13:33 Dling
 * 
 * 1     99-11-25 15:32 Dling
 * First Version by Ling Di
 * 
 ***************************************************************************
 */

#ifndef _FILE_API_H
#define _FILE_API_H

#include "stype.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/********************************************************************************
 *
 * macro and structures
 *
 ********************************************************************************/
#define _ASYNC_SELECT

/* length of string */
#define _MAX_PATH		256
#define _MAX_DRIVE		8
#define _MAX_DIR		_MAX_PATH
#define _MAX_FNAME		_MAX_PATH
#define _MAX_EXT		8

/* drive type */
#define DRIVE_UNKNOWN 		1	// The drive type cannot be determined.  
#define DRIVE_NO_ROOT_DIR 	2	// The root directory does not exist.  
#define DRIVE_REMOVABLE 	3	// The disk can be removed from the drive. 
#define DRIVE_FIXED 		4	// The disk cannot be removed from the drive. 
#define DRIVE_REMOTE  		5	// The drive is a remote (network) drive. 
#define DRIVE_CDROM  		6	// The drive is a CD-ROM drive. 
#define DRIVE_RAM  			7	// The drive is a RAM disk. 
#define DRIVE_ROM			8
#define DRIVE_FLASH			9
#define DRIVE_DEVFS			10

/* invalid file handle */
#undef  INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE	(HANDLE)-1

/* file create mode */
/* access mode bit */
#define ACCESS_READ					0x00000001
#define ACCESS_WRITE				0x00000002

/* share mode bits */
#define SHARE_READ					0x00000010  
#define SHARE_WRITE					0x00000020  
#define SHARE_DELETE				0x00000040  

/* create mode bits */
#define CREATE_NEW					0x00000100
#define CREATE_OVERWRITE			0x00000200
#define CREATE_TRUNCATE				0x00000400

/* file attribues */
#define FILE_ATTRIBUTE_READONLY     0x00000001  
#define FILE_ATTRIBUTE_HIDDEN       0x00000002  
#define FILE_ATTRIBUTE_SYSTEM       0x00000004  
#define FILE_ATTRIBUTE_DIRECTORY    0x00000010  
#define FILE_ATTRIBUTE_ARCHIVE      0x00000020  
#define FILE_ATTRIBUTE_NORMAL       0x00000080  
#define FILE_ATTRIBUTE_TEMPORARY    0x00000100  
#define FILE_ATTRIBUTE_COMPRESSED   0x00000800  

/* file type */
#define FILE_TYPE_UNKNOWN   0x0000
#define FILE_TYPE_DISK      0x0001
#define FILE_TYPE_CHAR      0x0002
#define FILE_TYPE_PIPE      0x0003
#define FILE_TYPE_REMOTE    0x8000

/* seek method */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* drive free space */
typedef struct tagFREESPACE{
	DWORD	dwSectorsPerCluster; 		// sectors per cluster 
	DWORD	dwBytesPerSector;			// bytes per sector  
	DWORD	dwBytesPerDir;				// sizeof directory structure
	DWORD	dwNumberOfFreeClusters; 	// number of free clusters  
	DWORD	dwNumberOfWasteClusters;	// number of waste clusters
	DWORD	dwTotalNumberOfClusters; 	// address of total number of clusters 
	DWORD	dwNumberOfWasteDirs;		// number of waste directorys
	DWORD	dwTotalNumberOfDirs;		// total directorys
}FREESPACE;

/* file time */
typedef struct _FILETIME {	// ft 
    DWORD		dwLowDateTime;	//
    DWORD		dwHighDateTime;	// 
}FILETIME, *PFILETIME, *LPFILETIME; 

/* find file data */
typedef struct _FIND_DATA {	// fd
	DWORD		dwAttributes;
    FILETIME	ftCreationTime;
	FILETIME	ftLastAccessTime;
    FILETIME	ftLastWriteTime;
    DWORD		nFileSizeLow;
	DWORD		nFileSizeHigh;
	DWORD		dwReserved0;
	DWORD		dwReserved1;
    char		cFileName[_MAX_PATH];
}_FIND_DATA, *P_FIND_DATA;

/* system time */
#ifndef _SYSTEMTIME_
#define _SYSTEMTIME_
typedef struct _SYSTEMTIME 
{  
    WORD wYear;     
    WORD wMonth; 
    WORD wDayOfWeek;     
    WORD wDay;     
    WORD wHour;     
    WORD wMinute; 
    WORD wSecond;     
    WORD wMilliseconds; 
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME; 
#endif // _SYSTEMTIME_

/********************************************************************************
 *
 * functions
 *
 ********************************************************************************/

/* get/set current directory */
DWORD	GetCurrentDirectory(PSTR pBuf, DWORD nSize);
BOOL	SetCurrentDirectory(PCSTR pPathName);

/* drive properties */
UINT	GetDriveType(PCSTR pDriveName);
BOOL	GetFreeSpace(PCSTR pDriveName, FREESPACE * pFreeSpace);

/* create/remove directory */
BOOL	CreateDirectory(PCSTR pPathName);
BOOL	RemoveDirectory(PCSTR pPathName);

/* delete file */
BOOL	DeleteFile(PCSTR pFileName);
BOOL	MoveFile(PCSTR pOldFileName, PCSTR pNewFileName);
BOOL	CopyFile(PCSTR pOldFileName, PCSTR pNewFileName);

/* get/set file attributes */
int		GetFileAttributes(PCSTR pFileName);
BOOL	SetFileAttributes(PCSTR pFileName, DWORD dwAttrib);

/* open/close file */
HANDLE	CreateFile(PCSTR pFileName, DWORD dwMode, DWORD dwAttrib);
BOOL	CloseFile(HANDLE hFile);

/* get file type */ 
DWORD	GetFileType(HANDLE hFile);
/* get file mode */
int		GetFileMode(HANDLE hFile);

/* read/write file */
int		ReadFile(HANDLE hFile, PVOID pBuf, DWORD nBytes);
int		WriteFile(HANDLE hFile, PVOID pBuf, DWORD nBytes);

/* seek file */
int		SetFilePointer(HANDLE hFile, LONG lDistance, DWORD dwMode); 

/* get/set file size */
int		GetFileSize(HANDLE hFile);
BOOL	SetFileSize(HANDLE hFile, DWORD nSize);

/* get/set file time */
BOOL	GetFileTime( HANDLE hFile, // identifies the file 
			LPFILETIME lpCreationTime,		// address of creation time 
			LPFILETIME lpLastAccessTime,	// address of last access time 
			LPFILETIME lpLastWriteTime		// address of last write time 
		); 
BOOL	SetFileTime( HANDLE hFile, // identifies the file 
			LPFILETIME lpCreationTime,		// address of creation time 
			LPFILETIME lpLastAccessTime,	// address of last access time 
			LPFILETIME lpLastWriteTime		// address of last write time 
		); 

/* lock/unlock file */
BOOL	LockFile(HANDLE hFile, DWORD dwOffset, DWORD dwLength);
BOOL	UnlockFile(HANDLE hFile, DWORD dwOffset, DWORD dwLength);

/* map/unmap file */
PVOID	MapFile(HANDLE hFile, DWORD dwOffset, DWORD dwSize);
BOOL	UnmapFile(HANDLE hFile);

// async select
#ifdef _ASYNC_SELECT
BOOL	AsyncSelect(HANDLE hFile, int hWnd, UINT uMsg, DWORD dwEvent);
#endif // _ASYNC_SELECT
BOOL	SetAsyncEvent(HANDLE hFile, HANDLE hEvent, DWORD dwEvent);
BOOL	DeviceIoControl(HANDLE hFile, UINT iCtlCode, PVOID pBuf, DWORD nSize);

/* find file */
HANDLE	FindFirstFile(PCSTR lpFileName, P_FIND_DATA pFindData);
BOOL	FindNextFile(HANDLE hFindFile, P_FIND_DATA pFindData);
BOOL	FindClose(HANDLE hFindFile);

/* conversion between FILETIME and SYSTEMTIME */
void	FTtoST(FILETIME* pFileTime, SYSTEMTIME* pSystemTime);
void	STtoFT(SYSTEMTIME* pSystemTime, FILETIME* pFileTime);

// detach a filesystem:
BOOL	DetachFileSystem(PCSTR pDriveName);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //_FILE_API_H
