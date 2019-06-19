/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    file_dep.h
Abstract:
	This module defines the prototype of all the system depedent functions.
Author:
    Luo hongbo
Revision History:
	2002.5		created
---------------------------------------------------------------------------*/
#ifndef FILE_DEP_H
#define FILE_DEP_H

#define FTP_ATTRIB_DIR			0x01	/* directory   */
#define FTP_ATTRIB_READ			0x02	/* read only   */
#define FTP_ATTRIB_SYS			0x04	/* system file */
#define FTP_ATTRIB_HIDDEN		0x08	/* hidden file */
#define FTP_ATTRIB_NORMAL		0x10	/* normal file */
#define FTP_ATTRIB_PROCESSED	0x80

#define FIND_NAME_MASK			0x01
#define FIND_EX_MASK			0x02

#define FTP_ERR_NOTEMPTY		0x01	/* delete a non-empty folder */
#define FTP_ERR_INVALID_PATH	0x02	/* delete a folder whose path is invalid */

#define FILE_HDL				DWORD	/* file acess handle */
#define INVALID_FILE_HDL		0		/* invalid file access handle */
#define FILE_FIND_HDL			long	/* file find handle */
#define INVALID_FILE_FIND_HDL	-1		/* invalid file find handle */

/* this value must be at least 25 */
#ifdef CONFIG_MICROSOFT
#define FTP_MAX_NAME 256				/* the maximum length of the FTP name */
#else
#define FTP_MAX_NAME 32					/* for the embedded system */
#endif

#ifndef MAX_TIME
#define MAX_TIME 20
#endif

/* file information structure*/
typedef struct FTP_FindDataStru{
	DWORD   size;						/* the size of the file */
	UCHAR	name[FTP_MAX_NAME];			/* the name of the file */
	UCHAR	create_time[MAX_TIME];		/* the created time of the file */
	UCHAR	acc_time[MAX_TIME];			/* the access time of the file */
	UCHAR	write_time[MAX_TIME];		/* the written time of the file */
	UCHAR	attrib;						/* the attribute of the file */
}FTP_FIND_DATA;

FILE_HDL FILE_OpenFile(const char* path);	/* open a specified file */
FILE_HDL FILE_CreateFile(const char* path);	/* create a specified file */
void  FILE_CloseFile(FILE_HDL hdl);			/* close a file */
int   FILE_WriteFile(FILE_HDL hdl,UCHAR* buf,DWORD bytes_to_write);	/* return the actual written size ;*/
DWORD FILE_ReadFile(FILE_HDL hdl,UCHAR* buf,DWORD bytes_to_read);	/* return the actual read size ; */
DWORD FILE_GetFileSize(FILE_HDL hdl);		/* get the size of the file */
UCHAR FILE_GetFileAttrib(const char* path);	/* get the attributes of the file */
void  FILE_SetFileAttrib(const char* path,UCHAR attrib);	/* set the attributes of the file */
int   FILE_Rewind(FILE_HDL hdl, long offset);	/* set the file pointer offset from the beginning of the file */

/* the following five functions implement file store with memory */
#ifdef CONFIG_FILE_MEM
FILE_HDL FILE_CreateFileEx(const char* path);
int   FILE_WriteFileEx(FILE_HDL hdl,UCHAR* buf,DWORD bytes_to_write);
DWORD FILE_ReadFileEx(FILE_HDL hdl,UCHAR* buf,DWORD bytes_to_read);
DWORD FILE_GetFileSizeEx(FILE_HDL hdl);
void  FILE_CloseFileEx(FILE_HDL hdl);
int   FILE_RewindEx(FILE_HDL hdl, long offset);
#endif

int FILE_CreateDir(const char* path);	/* create new directory;		0 success; else failure ; */
int FILE_ChangeDir(const char* path);	/* change current directory;	0 success; else failure ; */
int FILE_DelDir(const char* path);		/* delete a empty directory ;	0 success; else faiulre ; */
int FILE_DelFile(const char* path);		/* delete a file ;				0 success; else faiulre ; */

/* enumerate the files or folders in a specifed path */
FILE_FIND_HDL	FILE_FindFirst(const char* path,UCHAR* data,UCHAR mask);
int				FILE_FindNext(FILE_FIND_HDL hdl,UCHAR* data,UCHAR mask);	/* 0: success; else failure ; */
void			FILE_FindClose(FILE_FIND_HDL hdl);

/* some auxiliary functions */
void ADD_PATH(char* dir,char* s);
void ADD_SPLIT(char* dir);
void SET_ROOT(char* dir);
void BackFolder(char* path);
void BackFolder2(char* path);
DWORD GetLogicalDrive(DWORD len,UCHAR* drive);

#endif
