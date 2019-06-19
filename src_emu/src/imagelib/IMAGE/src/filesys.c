/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ImageLib
 *
 * Purpose  : File system access layer for POLLEX GUI System.
 *            
\**************************************************************************/


#include "window.h"
#include "osver.h"
#include "map.h"

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
//#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "dirent.h"
#include "unistd.h"
#include "sys\mman.h"
#if defined(_LINUX_OS_)
#include "sys/mman.h"
#endif  // _LINUX_OS_
#else   // _HOPEN_VER < 300
#include "string.h"
#if (OLDFILEAPI)
#include "fileapi.h"
#else   // NEWFILEAPI
#include "fapi.h"
#endif  // FILEAPI
#endif  // _HOPEN_VER

#include "filesys.h"
/*
#if defined(_MSC_VER)
#define DebugOutputString
#else
//#define DebugOutputString(format,...)
#endif
*/

int IMGOS_CreateFile(const char *pathname, int mode, int attrib)
{
    int fd;
    int flags = 0;

    if (!pathname)
        return -1;

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    if (mode & PLXFS_ACCESS_WRITE)
    {
        if (mode & PLXFS_ACCESS_READ)
            flags |= O_RDWR;
        else
            flags |= O_WRONLY;
    }

    if (mode & PLXFS_CREATE_NEW)
    {
        //if (mode & PLXFS_CREATE_TRUNCATE)
            //flags |= O_TRUNC;
        
        fd = creat(pathname, 0);
    }
    else
    {
        fd = open(pathname, flags);
    }

    if (fd == -1)
        return -1;

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    if (mode & PLXFS_ACCESS_WRITE)
    {
        if (mode & PLXFS_ACCESS_READ)
            flags |= O_RDWR;
        else
            flags |= O_WRITE;
    }

    if (mode & PLXFS_CREATE_NEW)
    {
        flags |= O_NEWFILE;
        
        fd = CreateFile((char *)pathname, flags, 0, 0);
    }
    else
    {
        fd = OpenFile((char *)pathname, flags, 0, 0);
    }    
#else   // NEWFILEAPI
    if (mode & PLXFS_ACCESS_READ)
        flags |= ACCESS_READ;
    if (mode & PLXFS_ACCESS_WRITE)
        flags |= ACCESS_WRITE;

    if (mode & PLXFS_CREATE_NEW)
    {
        flags |= CREATE_NEW;
        if (mode & PLXFS_CREATE_TRUNCATE)
            flags |= CREATE_OVERWRITE;        
    }

    fd = (int)CreateFile(pathname, flags, FILE_ATTRIBUTE_NORMAL);
#endif  // FILEAPI

    if (fd == (int)INVALID_HANDLE_VALUE)
        return -1;

#endif  // _HOPEN_VER

   // DebugOutputString("open %s return %d\n", pathname, fd); // fordebug
    return fd;
}


int IMGOS_CloseFile(int handle)
{
    int nRet;

    if (handle < 0)
        return -1;

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    nRet = close(handle);

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    nRet = CloseFile(handle);
#else   // NEWFILEAPI
    nRet = CloseFile((HANDLE)handle);
    if (nRet)
        nRet = 0;
    else
        nRet = -1;
#endif  // FILEAPI

#endif  // _HOPEN_VER

    //DebugOutputString("close return %d\n", nRet);
    return nRet;
}


long IMGOS_ReadFile(int handle, void *pbuf, unsigned long count)
{
    int nRead;

    if (handle < 0)
        return -1;

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    nRead = read(handle, pbuf, count);

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    nRead = ReadFile(handle, pbuf, count);
#else   // NEWFILEAPI
    nRead = ReadFile((HANDLE)handle, pbuf, count);
#endif  // FILEAPI

#endif  // _HOPEN_VER

    //DebugOutputString("read return %d\n", nRead);
    return nRead;
}


long IMGOS_WriteFile(int handle, void *pbuf, unsigned long count)
{
    int nWrite;

    if (handle < 0)
        return -1;

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    nWrite = write(handle, pbuf, count);

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    nWrite = WriteFile(handle, pbuf, count);
#else   // NEWFILEAPI
    nWrite = WriteFile((HANDLE)handle, pbuf, count);
#endif  // FILEAPI

#endif  // _HOPEN_VER

    //DebugOutputString("write return %d\n", nWrite);
    return nWrite;
}


long IMGOS_SetFilePointer(int handle, unsigned long lDistance, int mode)
{
    long nRet;

    if (handle < 0)
        return -1;

    if (mode == PLXFS_SEEK_SET)
        mode = SEEK_SET;
    else if (mode == PLXFS_SEEK_CUR)
        mode = SEEK_CUR;
    else if (mode == PLXFS_SEEK_END)
        mode = SEEK_END;
    else
        return -1;

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    nRet = lseek(handle, lDistance, mode);

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    nRet = SeekFile(handle, lDistance, mode);
#else   // NEWFILEAPI
    nRet = SetFilePointer((HANDLE)handle, lDistance, mode);
#endif  // FILEAPI

#endif  // _HOPEN_VER

    //DebugOutputString("seek file return %d\n", (int)nRet);
    return nRet;
}


long IMGOS_GetFileSize(int handle)
{
#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)
    long nCurPos;
#endif
    long nFileSize;

    if (handle < 0)
        return -1;

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    nCurPos = lseek(handle, 0, SEEK_SET);
    nFileSize = lseek(handle, 0, SEEK_END);
    lseek(handle, nCurPos, SEEK_SET);

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    nFileSize = GetFileSize(handle);
#else   // NEWFILEAPI
    nFileSize = GetFileSize((HANDLE)handle);
#endif  // FILEAPI

#endif  // _HOPEN_VER

    //DebugOutputString("filesize: %d\n", (int)nFileSize);
    return nFileSize;
}


void *IMGOS_MapFile(int handle, unsigned long offset, unsigned long length)
{
#if (_HOPEN_VER >= 300)
//    long nCurPos;
#endif
    long nFileSize;
    void *pReturn;

    if (handle < 0)
        return 0;

    nFileSize = IMGOS_GetFileSize(handle);
    if (nFileSize == 0 || (unsigned long)nFileSize <= offset)
        return 0;
    if (length > nFileSize - offset)
        length = nFileSize - offset;

#if defined(_LINUX_OS_)

    pReturn = mmap(0, length, PROT_READ, MAP_SHARED, handle, offset);
    if (pReturn == MAP_FAILED)
        return 0;

    return pReturn;

#elif (_HOPEN_VER >= 300)

    pReturn = mmap(0, length, NULL, NULL, handle, offset);
    if (pReturn == MAP_FAILED)
        return 0;

    return pReturn;

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    pReturn = (void *)MapFile(handle, offset, (int *)&length);
    return pReturn;
#else   // NEWFILEAPI
    pReturn = MapFile((HANDLE)handle, offset, length);
    return pReturn;
#endif  // FILEAPI

#endif  // _HOPEN_VER
}


int IMGOS_UnmapFile(int handle, void *start, unsigned long length)
{
    int nRet = 0;

    if (handle < 0)
        return -1;

#if defined(_LINUX_OS_)

    nRet = munmap(start, length);

#elif (_HOPEN_VER >= 300)

    nRet = munmap(start, length);

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    nRet = UnmapFile(handle);
#else   // NEWFILEAPI
    nRet = UnmapFile((HANDLE)handle);
    if (nRet)
        nRet = 0;
    else
        nRet = -1;
#endif  // FILEAPI

#endif  // _HOPEN_VER

    //DebugOutputString("unmap file return %d\n", nRet);
    return nRet;
}


int IMGOS_FindFirstFile(const char *filepath, void **ppDir, void **ppDirEntry)
{
#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    char    pathname[256];
    char    filename[16];
    char    *pos;
    int     type = 0;
    DIR     *pDir;
    struct  dirent *pDirEntry;

    strcpy(pathname, filepath);
    if ((pos = strrchr(pathname, '.')) != 0 && *(pos - 1) == '*')
    {
        if (*(pos + 1) != '*')
            type = 1;
        strcpy(filename, pos);
        *(pos - 1) = 0;
    }
    pDir = opendir(pathname);
    if (!pDir)
    {
        //DebugOutputString("open directory %s failed\n", pathname);
        return -1;
    }

    if (type == 0)
    {
        pDirEntry = readdir(pDir);
        if (!pDirEntry)
            return -1;
    }
    else
    {
        while ((pDirEntry = readdir(pDir)) != 0)
        {
            pos = strrchr(pDirEntry->d_name, '.');
            if (pos && !strcmp(pos, filename))
                break;
        }
        
        if (!pDirEntry)
        {
            closedir(pDir);
            //DebugOutputString("find first file *%s failed\n", filename);
            return -1;
        }
    }
    
    *ppDir = pDir;
    *ppDirEntry = pDirEntry;

    return 0;

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    char    pathname[256];
    char    filename[16];
    char    *pos;
    C_DIRENTRY  *pDirEntry;

    strcpy(pathname, filepath);
    if ((pos = strrchr(pathname, '.')) && *(pos - 1) == '*')
    {
        strcpy(filename, pos - 1);
        *(pos - 1) = 0;
    }
    else
        strcpy(filename, "*.*");
    pDirEntry = FindFirst((char *)pathname, -1, filename);
    *ppDirEntry = pDirEntry;

    if (pDirEntry && pDirEntry != (C_DIRENTRY  *)-1)
        return 0;
    else
        return -1;
#else   // NEWFILEAPI
    P_FIND_DATA pFindData;
    HANDLE hFindFile;

    pFindData = (P_FIND_DATA)malloc(sizeof(_FIND_DATA));
    if (!pFindData)
        return -1;

    hFindFile = FindFirstFile(filepath, pFindData);
    *ppDir = hFindFile;
    if (hFindFile == INVALID_HANDLE_VALUE)
    {
        free(pFindData);
        *ppDirEntry = 0;
        return -1;
    }
    *ppDirEntry = pFindData;
    return 0;
#endif  // FILEAPI

#endif  // _HOPEN_VER
}


int IMGOS_FindNextFile(const char *filepath, void *pDir, void **ppDirEntry)
{
#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    char    filename[16];
    char    *pos;
    int     type = 0;
    struct dirent *pDirEntry;

    if ((pos = strrchr(filepath, '.')) != 0 && *(pos - 1) == '*')
    {
        if (*(pos + 1) != '*')
            type = 1;
        strcpy(filename, pos);
    }

    if (type == 0)
    {
        pDirEntry = readdir((DIR *)pDir);
        if (!pDirEntry)
            return -1;
    }
    else
    {
        while ((pDirEntry = readdir(pDir)) != 0)
        {
            pos = strrchr(pDirEntry->d_name, '.');
            if (pos && !strcmp(pos, filename))
                break;
        }

        if (!pDirEntry)
        {
            //DebugOutputString("find next file *%s failed\n", filename);
            return -1;
        }
    }
    *ppDirEntry = pDirEntry;

    return 0;

#else   // _HOPEN_VER < 300

    int nRet;
#if (OLDFILEAPI)
    nRet = FindNext((C_DIRENTRY *)*ppDirEntry);
#else   // NEWFILEAPI
    nRet = FindNextFile((HANDLE)pDir, (P_FIND_DATA)*ppDirEntry);
    if (nRet)
        nRet = 0;
    else
        nRet = -1;
#endif  // FILEAPI
    return nRet;

#endif  // _HOPEN_VER
}


int IMGOS_FindClose(void *pDir, void *pDirEntry)
{
    int nRet;

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    nRet = closedir((DIR *)pDir);

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    nRet = FindEnd((C_DIRENTRY *)pDirEntry);
#else   // NEWFILEAPI
    if (pDirEntry)
        free(pDirEntry);
    nRet = FindClose((HANDLE)pDir);
    if (nRet)
        nRet = 0;
    else
        nRet = -1;
#endif  // FILEAPI

#endif  // _HOPEN_VER

    return nRet;
}


int IMGOS_GetDirEntryName(char *name, void *pDirEntry)
{
    if (!pDirEntry)
        return -1;

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    strcpy(name, ((struct dirent *)pDirEntry)->d_name);

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    strcpy(name, ((C_DIRENTRY *)pDirEntry)->name);
#else   // NEWFILEAPI
    strcpy(name, ((P_FIND_DATA)pDirEntry)->cFileName);
#endif  // FILEAPI

#endif  // _HOPEN_VER

    return 0;
}


int IMGOS_GetDirEntryAttr(void *pDirEntry, unsigned long *attr)
{
    if (!pDirEntry)
        return -1;

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    return -1;

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    *attr = ((C_DIRENTRY *)pDirEntry)->attrib;
#else   // NEWFILEAPI
    *attr = ((P_FIND_DATA)pDirEntry)->dwAttributes;
#endif  // FILEAPI

    return 0;

#endif  // _HOPEN_VER
}


int	IMGOS_GetCurrentDirectory(char *pbuf, int size)
{
#if ((_HOPEN_VER < 300) && (!OLDFILEAPI)) || defined(_LINUX_OS_)
    int     nLen;
#if defined (_LINUX_OS_)
    char    *pathname;
#endif  // _LINUX_OS_
#endif

    if (!pbuf)
        return -1;

#if defined(_LINUX_OS_)

    pathname = getenv("PWD");
    if (!pathname)
        return -1;
    nLen = strlen(pathname);
    if (nLen < size - 1)
    {
        memcpy(pbuf, pathname, nLen + 1);
        return nLen;
    }

    *pbuf = '\0';
    return nLen + 1;

#elif (_HOPEN_VER >= 300)

    return -1;

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    return -1;
#else   // NEWFILEAPI
    nLen = GetCurrentDirectory(pbuf, size);

    return nLen;
#endif  // FILEAPI

#endif  // _HOPEN_VER
}


int IMGOS_SetCurrentDirectory(const char *pathname)
{
#if ((_HOPEN_VER < 300) && (!OLDFILEAPI)) || defined(_LINUX_OS_)
    int nRet;
#endif

    if (!pathname)
        return -1;

#if defined(_LINUX_OS_)

    nRet = setenv("PWD", pathname, 1);
    return nRet;

#elif (_HOPEN_VER >= 300)

    return -1;

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    return -1;
#else   // NEWFILEAPI
    nRet = SetCurrentDirectory(pathname);
    if (nRet)
        return 0;
    else
        return -1;
#endif  // FILEAPI

#endif  // _HOPEN_VER
}


BOOL IMGOS_IsFileType(const char *filename, unsigned long type)
{
#if defined(_LINUX_OS_)
    int nRet;
    struct stat sbuf;
#endif

    if (!filename)
        return FALSE;

#if defined(_LINUX_OS_)

    nRet = lstat(filename, &sbuf);
    if (nRet == -1)
        return FALSE;

    if ((sbuf.st_mode & S_IFMT) == type)
        return TRUE;
    return FALSE;

#else   // _HOPEN_VER

    return FALSE;

#endif  // _HOPEN_VER
}
