/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	FILE SYSTEM
 *
 * Purpose :	Declare file API functions for old fs version
 *  
 * Author  :    Ling Di 
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /Hopen/include/fileapi.h                              $
 * $Workfile:: fileapi.h                                            $
 * $Revision:: 7  $     $Date:: 03-03-12 5:04p                      $
 * 
\**************************************************************************/
/***************************************************************************
 * 
 * *****************  Version 6  *****************
 * User: Ykfeng       Date: 02-10-30   Time: 9:45a
 * Updated in $/Hopen/include
 * delete the typedef of SYSTEMTIME
 * 
 * *****************  Version 5  *****************
 * User: Dling        Date: 00-07-13   Time: 10:11
 * Updated in $/Hopen/include
 * bak for stb product by ZhangBin
 * 
 * *****************  Version 4  *****************
 * User: Dling        Date: 00-01-07   Time: 15:17
 * Updated in $/Hopen/include
 * add branket to macros
 * 
 * *****************  Version 3  *****************
 * User: Dling        Date: 99-11-29   Time: 13:33
 * Updated in $/Hopen/include
 * 
 * *****************  Version 2  *****************
 * User: Dling        Date: 99-11-25   Time: 15:29
 * Updated in $/Hopen/include
 * updated by ling di
 * 
 * *****************  Version 1  *****************
 * User: Dling        Date: 99-11-08   Time: 14:20
 * Created in $/Hopen/include
 * 
 * *****************  Version 1  *****************
 * User: Dling        Date: 99-11-08   Time: 13:34
 * Created in $/HopenEmu/include
 * 
 * *****************  Version 2  *****************
 * User: Dling        Date: 99-08-02   Time: 12:44
 * Updated in $/include
 * new header
 * 
 * *****************  Version 8  *****************
 * User: Dengyi       Date: 99-06-08   Time: 8:59p
 * Updated in $/hopenpda/include
 * // 990608 changed, kent.
 * // To keep these functions private.
 * // moved from fileapi.h to Fileapi.c
 * 
 * extern char *CopyString( char *dest, char *src );
 * extern char *CopyStringN( char *dest, char *src, int n );
 * extern char *CatString( char *dest, char *src );
 * extern int StringLength( char * str );
 * extern int CompareString( char *str1, char *str2 );
 * extern char ToLower( char c );
 * extern char ToUpper( char c );
 * extern int CompareStringN( char *str1, char *str2, int n );
 * extern int CompareStringNoCase( char *str1, char *str2 );
 * extern int CompareStringNoCaseN( char *str1, char *str2, int n
 * );
 * extern char *FindString( char *string, char * substr );
 * extern char *FindStringNoCase( char *string, char * substr );
 * extern char *FindChar( char *string, char c );
 * extern int StringToInt( char *str );
 * extern void IntToString( int nValue, char *buf );
 * 
 * // 990608 changed, kent.
 * 
 * *****************  Version 7  *****************
 * User: Dengyi       Date: 99-05-11   Time: 10:06p
 * Updated in $/hopenpda/include
 * This version was done by Qiu Haiqing , 
 * 4 functions added and one bug fixed:
 * <1>GetPrivatProfileString
 * <2>GetPrivatProfileInt
 * <3>WritePrivatProfileString
 * <4>WritePrivatProfileInt
 * 
 * *****************  Version 6  *****************
 * User: Xswang       Date: 99-05-10   Time: 9:19a
 * Updated in $/hopenpda/include
 * Add some thing to support Async device.
 * 
 * *****************  Version 5  *****************
 * User: Xswang       Date: 99-05-06   Time: 12:10p
 * Updated in $/hopenpda/include
 * Change type of SetDefaultPath from char * to int
 * 
 * *****************  Version 4  *****************
 * User: Dengyi       Date: 99-04-23   Time: 2:09p
 * Updated in $/hopenpda/include
 * 
 * *****************  Version 1  *****************
 * User: Hqqiu        Date: 99-04-14   Time: 11:41a
 * Created in $/FileSys/casspda/include
 * File system initial version.
 * 14, April, 1999
 * 
 * *****************  Version 2  *****************
 * User: Xswang       Date: 99-03-05   Time: 6:05p
 * Updated in $/minitask/include
 * Add mapfile and Unmapfile
 * 
 * *****************  Version 1  *****************
 * User: Xswang       Date: 99-02-24   Time: 1:21p
 * Created in $/minitask/include
 * 
 * *****************  Version 1  *****************
 * User: Gaoyue       Date: 99-01-29   Time: 11:59a
 * Created in $/minitask/filesys
 * 
 
\**************************************************************************/
#include "fapi.h"

#ifndef _DIRENTRY_DEFINED
#define _DIRENTRY_DEFINED
typedef struct c_direntry {
	unsigned long	index;
	unsigned long	type;
	unsigned long	attrib;
	unsigned long	fid;
	unsigned long	size;
	unsigned long	date;
	char			name[64];
} C_DIRENTRY;
#endif //_DIRENTRY_DEFINED


#define O_RDONLY	ACCESS_READ		/* 只读方式 */
#define O_WRITE		ACCESS_WRITE	/* 可写入方式 */
#define O_RDWR		(ACCESS_READ | ACCESS_WRITE) 	/* 可读写方式 */
#define O_CREAT		(CREATE_NEW | CREATE_OVERWRITE)	/* 可创建方式 */
#define O_NEWFILE   (CREATE_NEW | CREATE_OVERWRITE)	/* 创建新文件方式 */
#define O_WRLOCK	SHARE_READ		/* 封锁写入共享操作方式 */
#define O_LOCK		0				/* 排它性方式 */
#define O_NOEXPAND  ACCESS_READ		/* 不可扩展长度方式 */

//typedef struct _SYSTEMTIME SYSTEMTIME;

#define _OLD_RETVAL(function)	(function ? 0 : -1)

int SetDefaultPath(char *newpath);

//int DeleteFile(char * name);
#define DeleteFile(name)	_OLD_RETVAL(DeleteFile(name))

int _CreateFile(char * name, int mode, int attrib, int size);
#define CreateFile	_CreateFile

int OpenFile(char * name, int mode, int attrib, int size);

//int CloseFile(int handle);
#define CloseFile(handle)	_OLD_RETVAL(CloseFile((HANDLE)handle))

unsigned int GetFileAttr(int handle);

//int ReadFile(int handle, void * pbuf, int count);
//int WriteFile(int handle, void * pbuf, int count);
#define ReadFile(handle, pbuf, count)	(int)ReadFile((HANDLE)handle, pbuf, (DWORD)(count))
#define WriteFile(handle, pbuf, count)	(int)WriteFile((HANDLE)handle, pbuf, (DWORD)(count))

int _MapFile(int handle, int offset, int * pMapLen);
#define MapFile	_MapFile

//int UnmapFile( int handle );
#define UnmapFile(handle)	_OLD_RETVAL(UnmapFile((HANDLE)handle))

int SeekFile(int handle, int offset, int orign);

int ExpandFile(int handle, int newsize);
int TruncateFile(int handle, int newsize);

//int GetFileSize(int handle);
#define GetFileSize(handle)	GetFileSize((HANDLE)handle)

int _GetFileTime(PCSTR pFileName, SYSTEMTIME * pTime);
#define GetFileTime	_GetFileTime

int FILE_AsyncSelect(int handle, int hwindow, int message, int event);
int FILE_ioctl(int handle, int func, void * parg, int argsz);

//int FindFirst(char *pDirName, char *pName, C_DIRENTRY* result);
C_DIRENTRY* FindFirst(char *pDirName, int type, char *pName);
int FindNext(C_DIRENTRY *result);
int FindEnd(C_DIRENTRY *result);

int FormatRam(short dirs,int sizetotal);

long FS_GetPrivateProfileInt( 
				char *lpAppName,  // address of section name 
				char *lpKeyName,  // address of key name 
				unsigned long nDefault,   		// return value if key name is not found 
				char *lpFileName  // address of initialization filename 
);

long FS_GetPrivateProfileString( 
				char *lpAppName,  // points to section name 
				char *lpKeyName,  // points to key name 
				char *lpDefault,  // points to default string 
				char *lpReturnedString,  // points to destination buffer 
				unsigned long nSize,				// size of destination buffer 
				char *lpFileName  // points to initialization filename 
);

long FS_WritePrivateProfileString( 
				char *lpAppName,  // points to section name 
				char *lpKeyName,  // points to key name 
				char *lpString,		// pointer to string to add 
				char *lpFileName  // points to initialization filename 
);

long FS_WritePrivateProfileInt( 
				char *lpAppName,  // points to section name 
				char *lpKeyName,  // points to key name 
				long	 nValue,			// pointer to string to add 
				char *lpFileName  // points to initialization filename 
);
