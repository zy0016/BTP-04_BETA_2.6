/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for file system access layer.
 *            
\**************************************************************************/

#ifndef __HPFILE_H
#define __HPFILE_H

#define MAX_PATH		256

/* access mode bits */
#define PLXFS_ACCESS_READ                    0x0001
#define PLXFS_ACCESS_WRITE                   0x0002

/* share mode bits */
#define PLXFS_SHARE_READ                     0x0010
#define PLXFS_SHARE_WRITE                    0x0020
#define PLXFS_SHARE_DELETE                   0x0040

/* create mode bits */
#define PLXFS_CREATE_NEW                     0x0100
#define PLXFS_CREATE_OVERWRITE               0x0200
#define PLXFS_CREATE_TRUNCATE                0x0400

/* file attribues */
#define PLXFS_FILE_ATTRIBUTE_READONLY        0x0001
#define PLXFS_FILE_ATTRIBUTE_HIDDEN          0x0002
#define PLXFS_FILE_ATTRIBUTE_SYSTEM          0x0004
#define PLXFS_FILE_ATTRIBUTE_DIRECTORY       0x0010
#define PLXFS_FILE_ATTRIBUTE_ARCHIVE         0x0020
#define PLXFS_FILE_ATTRIBUTE_NORMAL          0x0080
#define PLXFS_FILE_ATTRIBUTE_TEMPORARY       0x0100
#define PLXFS_FILE_ATTRIBUTE_COMPRESSED      0x0800

/* seek method */
#define PLXFS_SEEK_SET 0
#define PLXFS_SEEK_CUR 1
#define PLXFS_SEEK_END 2

/* encoding of the file mode */
#define PLXFS_IFMT   0170000     /* these bits determine file type */

/* file types */
#define PLXFS_IFDIR  0040000     /* directory */
#define PLXFS_IFCHR  0020000     /* character device */
#define PLXFS_IFBLK  0060000     /* block device */
#define PLXFS_IFREG  0100000     /* regular file */
#define PLXFS_IFIFO  0010000     /* fifo */
#define PLXFS_IFLNK  0120000     /* symbolic link */
#define PLXFS_IFSOCK 0140000     /* socket */


int  PLXOS_CreateFile(const char *pathname, int mode, int attrib);
int  PLXOS_CloseFile(int handle);

long PLXOS_ReadFile(int handle, void *pbuf, unsigned long count);
long PLXOS_WriteFile(int handle, void *pbuf, unsigned long count);

long PLXOS_SetFilePointer(int handle, unsigned long lDistance, int mode);

long PLXOS_GetFileSize(int handle);

void *PLXOS_MapFile(int handle, unsigned long offset, unsigned long length);
int  PLXOS_UnmapFile(int handle, void *start, unsigned long length);

int  PLXOS_FindFirstFile(const char *filepath, void **ppDir, void **ppDirEntry);
int  PLXOS_FindNextFile(const char *filepath, void *pDir, void **ppDirEntry);
int  PLXOS_FindClose(void *pDir, void *pDirEntry);
int  PLXOS_GetDirEntryName(char *name, void *pDirEntry);
int  PLXOS_GetDirEntryAttr(void *pDirEntry, unsigned long *attr);

int  PLXOS_GetCurrentDirectory(char *pbuf, int size);
int  PLXOS_SetCurrentDirectory(const char *pathname);
BOOL PLXOS_IsFileType(const char *filename, unsigned long type);

int  PLXOS_GetFontDirectory(char *pathname);
int  PLXOS_GetInitFileName(char *pathname);

int PLXOS_GetBitmapDirectory(char *pathname);
int PLXOS_GetResourceDirectory(char *pathname);

#endif  // __HPFILE_H
