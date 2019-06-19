/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : File system access layer for PLX GUI System.
 *            
\**************************************************************************/



 

#include "hpwin.h"

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)
#include "sys/types.h"
#include "sys/stat.h"
#include "ctype.h"
#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "dirent.h"
#include "unistd.h"
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

#include "hpfile.h"

#if defined(_MSC_VER)
#define DebugOutputString   
#else
//#define DebugOutputString(format,...)   
#define DebugOutputString   
#endif


static void catlwrstr(char *strDestination, const char *strSource)
{
    while (*strSource != '\0')
    {
        if (*strSource == '\\')
        {
            *strDestination++ = '/';
            strSource ++;
        }
        else
            *strDestination++ = tolower(*strSource++);
    }
    *strDestination = '\0';
}

int PLXOS_CreateFile(const char *pathname, int mode, int attrib)
{
    int fd;
    int flags = 0;
#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)
    int pathlen;
    int convert = 0;
    char filepath[380];
#endif  // _HOPEN_VER >= 300

    if (!pathname)
        return -1;

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    pathlen = strlen(pathname);
    if (pathlen >= 4)
    {
        if (toupper(pathname[0]) == 'R' && toupper(pathname[1]) == 'O' && 
            toupper(pathname[2]) == 'M' && pathname[3] == ':')
        {
            if (pathlen > sizeof(filepath) - 2)
                return -1;
            strcpy(filepath, "/rom/");
            if ((pathname[4] == '/') || (pathname[4] == '\\'))
                catlwrstr(filepath + 5, pathname + 5);
            else
                catlwrstr(filepath + 5, pathname + 4);
            convert = 1;
        }
        else if (pathlen >= 6)
        {
            if (toupper(pathname[0]) == 'F' && toupper(pathname[1]) == 'L' && 
            toupper(pathname[2]) == 'A' && toupper(pathname[3]) == 'S' && 
            toupper(pathname[4]) == 'H' )
            {
                int i = 5;
                while (pathname[i] != ':' && pathname[i] != '\0')
                    i++;

                if (pathname[i] == '\0')
                    return -1;

                if (pathlen > (int)(sizeof(filepath) - 11 + i))
                    return -1;

                i++;
                strcpy(filepath, "/mnt/flash/");
                if ((pathname[i] == '/') || (pathname[i] == '\\'))
                    catlwrstr(filepath + 11, pathname + i + 1);
                else
                    catlwrstr(filepath + 11, pathname + i);
                convert = 1;
            }
        }
    }

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

        if (convert)
            fd = creat(filepath, 0664);
        else
            fd = creat(pathname, 0664);
    }
    else
    {
        if (convert)
            fd = open(filepath, flags);
        else
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

    DebugOutputString("open %s return %d\n", pathname, fd); // fordebug
    return fd;
}


int PLXOS_CloseFile(int handle)
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

    DebugOutputString("close return %d\n", nRet);
    return nRet;
}


long PLXOS_ReadFile(int handle, void *pbuf, unsigned long count)
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

    DebugOutputString("read return %d\n", nRead);
    return nRead;
}


long PLXOS_WriteFile(int handle, void *pbuf, unsigned long count)
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

    DebugOutputString("write return %d\n", nWrite);
    return nWrite;
}


long PLXOS_SetFilePointer(int handle, unsigned long lDistance, int mode)
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

    DebugOutputString("seek file return %d\n", (int)nRet);
    return nRet;
}


long PLXOS_GetFileSize(int handle)
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

    DebugOutputString("filesize: %d\n", (int)nFileSize);
    return nFileSize;
}


void *PLXOS_MapFile(int handle, unsigned long offset, unsigned long length)
{
#if (_HOPEN_VER >= 300)
    long nCurPos;
#endif
    long nFileSize;
    void *pReturn;

    if (handle < 0)
        return 0;

    nFileSize = PLXOS_GetFileSize(handle);
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

    pReturn = MemAlloc(length);
    if (!pReturn)
    {
        TRACEERROR();
        return 0;
    }

    nCurPos = lseek(handle, 0, SEEK_SET);
    lseek(handle, offset, SEEK_SET);
    read(handle, pReturn, length);
    lseek(handle, nCurPos, SEEK_SET);

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


int PLXOS_UnmapFile(int handle, void *start, unsigned long length)
{
    int nRet = 0;

    if (handle < 0)
        return -1;

#if defined(_LINUX_OS_)

    nRet = munmap(start, length);

#elif (_HOPEN_VER >= 300)

    MemFree(start);

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

    DebugOutputString("unmap file return %d\n", nRet);
    return nRet;
}


BOOL PLXOS_IsFileType(const char *filename, unsigned long type)
{
#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)
    int nRet;
    struct stat sbuf;
#endif

    if (!filename)
        return FALSE;

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    nRet = stat(filename, &sbuf);
    if (nRet == -1)
        return FALSE;

    if ((sbuf.st_mode & type) == type)
        return TRUE;
    return FALSE;

#else   // _HOPEN_VER < 300

    return FALSE;

#endif  // _HOPEN_VER
}


int PLXOS_FindFirstFile(const char *filepath, void **ppDir, void **ppDirEntry)
{
#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    char    pathname[380];
    char    currpath[380];
    char    filename[380];
    char    *pos;
    int     len;
    int     type = 0;
    DIR     *pDir;
    struct  dirent *pDirEntry;

    len = strlen(filepath);
    if (len > (sizeof(pathname) - 1))
        return -1;

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
        DebugOutputString("open directory %s failed\n", pathname);
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
        len = strlen(pathname);
        if (pathname[len - 1] != '/')
        {
            if (len >= (sizeof(pathname) - 1))
                return -1;
            strcat(pathname, "/");
        }
        while ((pDirEntry = readdir(pDir)) != 0)
        {
            strcpy(currpath, pathname);
            if ((strlen(currpath) + strlen(pDirEntry->d_name)) > 
                (sizeof(currpath) - 1))
                continue;
            strcat(currpath, pDirEntry->d_name);
            if (PLXOS_IsFileType(currpath, PLXFS_IFDIR))
                break;

            pos = strrchr(pDirEntry->d_name, '.');
            if (pos && !strcmp(pos, filename))
                break;
        }
        
        if (!pDirEntry)
        {
            closedir(pDir);
            DebugOutputString("find first file *%s failed\n", filename);
            return -1;
        }
    }
    
    *ppDir = pDir;
    *ppDirEntry = pDirEntry;

    return 0;

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    char    pathname[380];
    char    filename[380];
    char    *pos;
    int     len;
    C_DIRENTRY  *pDirEntry;

    len = strlen(filepath);
    if (len > (sizeof(pathname) - 1))
        return -1;

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

    pFindData = (P_FIND_DATA)MemAlloc(sizeof(_FIND_DATA));
    if (!pFindData)
        return -1;

    hFindFile = FindFirstFile(filepath, pFindData);
    *ppDir = hFindFile;
    if (hFindFile == INVALID_HANDLE_VALUE)
    {
        MemFree(pFindData);
        *ppDirEntry = 0;
        return -1;
    }
    *ppDirEntry = pFindData;
    return 0;
#endif  // FILEAPI

#endif  // _HOPEN_VER
}


int PLXOS_FindNextFile(const char *filepath, void *pDir, void **ppDirEntry)
{
#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    char    pathname[380];
    char    currpath[380];
    char    filename[380];
    char    *pos;
    int     len;
    int     type = 0;
    struct dirent *pDirEntry;

    len = strlen(filepath);
    if (len > (sizeof(pathname) - 1))
        return -1;

    strcpy(pathname, filepath);
    if ((pos = strrchr(pathname, '.')) != 0 && *(pos - 1) == '*')
    {
        if (*(pos + 1) != '*')
            type = 1;
        strcpy(filename, pos);
        *(pos - 1) = 0;
    }

    if (type == 0)
    {
        pDirEntry = readdir((DIR *)pDir);
        if (!pDirEntry)
            return -1;
    }
    else
    {
        len = strlen(pathname);
        if (pathname[len - 1] != '/')
        {
            if (len >= (sizeof(pathname) - 1))
                return -1;
            strcat(pathname, "/");
        }
        while ((pDirEntry = readdir(pDir)) != 0)
        {
            strcpy(currpath, pathname);
            if ((strlen(currpath) + strlen(pDirEntry->d_name)) > 
                (sizeof(currpath) - 1))
                continue;
            strcat(currpath, pDirEntry->d_name);
            if (PLXOS_IsFileType(currpath, PLXFS_IFDIR))
                break;

            pos = strrchr(pDirEntry->d_name, '.');
            if (pos && !strcmp(pos, filename))
                break;
        }

        if (!pDirEntry)
        {
            DebugOutputString("find next file *%s failed\n", filename);
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


int PLXOS_FindClose(void *pDir, void *pDirEntry)
{
    int nRet;

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

    nRet = closedir((DIR *)pDir);

#else   // _HOPEN_VER < 300

#if (OLDFILEAPI)
    nRet = FindEnd((C_DIRENTRY *)pDirEntry);
#else   // NEWFILEAPI
    if (pDirEntry)
        MemFree(pDirEntry);
    nRet = FindClose((HANDLE)pDir);
    if (nRet)
        nRet = 0;
    else
        nRet = -1;
#endif  // FILEAPI

#endif  // _HOPEN_VER

    return nRet;
}


int PLXOS_GetDirEntryName(char *name, void *pDirEntry)
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


int PLXOS_GetDirEntryAttr(void *pDirEntry, unsigned long *attr)
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


int	PLXOS_GetCurrentDirectory(char *pbuf, int size)
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


int PLXOS_SetCurrentDirectory(const char *pathname)
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


#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

#define FONTDIRECTORY           "/usr/local/lib/gui/fonts/*.fnt"
#define INITFILEDIRECTORY       "/usr/local/lib/gui/sys/win.ini"

#else   // _HOPEN_VER < 300

#define FONTDIRECTORY           "ROM:/*.fnt"
#define INITFILEDIRECTORY       "ROM:/win.ini"

#endif  // _HOPEN_VER

int PLXOS_GetFontDirectory(char *pathname)
{
    if (!pathname)
        return -1;

    strcpy(pathname, FONTDIRECTORY);

    return 0;
}

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)

#define BMPDIRECTORY           "/usr/local/lib/gui/bitmaps/"
#define RESDIRECTORY           "/usr/local/lib/gui/resources/"

#else   // _HOPEN_VER < 300

#define BMPDIRECTORY           "ROM:/"
#define RESDIRECTORY           "ROM:/"

#endif  // _HOPEN_VER

int PLXOS_GetBitmapDirectory(char *pathname)
{
    if (!pathname)
        return -1;
    
    strcpy(pathname, BMPDIRECTORY);
    
    return 0;
}

int PLXOS_GetResourceDirectory(char *pathname)
{
    if (!pathname)
        return -1;
    
    strcpy(pathname, RESDIRECTORY);
    
    return 0;
}

int PLXOS_GetInitFileName(char *pathname)
{
    if (!pathname)
        return -1;

    strcpy(pathname, INITFILEDIRECTORY);

    return 0;
}
