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

#include "plxdebug.h"

#ifdef DEBUG_OBJECT

#include "window.h"
#include "winpda.h"
#include "string.h"
#include "stdio.h"
#include "assert.h"
#include "mullang.h"

#ifndef NO_DEBUG_MEMORY

#ifdef HOPEN20
#undef GlobalAlloc
#undef GlobalFree
#undef LocalAlloc
#undef LocalFree
#undef LocalRealloc

#else	// HOPEN20

#undef malloc
#undef free
#undef realloc
#undef calloc

#endif	// HOPEN20

#endif

#ifndef NO_DEBUG_FS

#ifdef HOPEN20

#undef CreateFile
#undef CloseFile
#undef FindFirstFile
#undef FindClose

#else

#undef open
#undef creat
#undef close

#undef opendir
#undef closedir

#endif
#endif

#ifndef NO_DEBUG_WINDOW

#undef CreatePen
#undef CreatePenIndirect

#undef CreateBrush
#undef CreateSolidBrush
#undef CreateHatchBrush
#undef CreatePatternBrush
#undef CreateBrushIndirect

#undef CreateFont
#undef CreateFontIndirect

#undef CreateBitmap
#undef CreateBitmapEx
#undef CreateBitmapIndirect
#undef CreateCompatibleBitmap
#undef CreateDIBitmap

#undef LoadBitmap
#undef LoadImage
#undef LoadImageEx

#undef DeleteObject

#undef CreateDC
#undef CreateScreenDC
#undef CreateCompatibleDC
#undef CreatePrinterDC
#undef CreateMemoryDC
#undef DeleteDC

#undef GetDC
#undef GetWindowDC
#undef ReleaseDC

#undef LoadIcon
#undef CreateIcon
#undef DestroyIcon

#undef LoadCursor
#undef CreateCursor
#undef DestroyCursor

#undef RegisterClass
#undef RegisterClassEx
#undef UnregisterClass

#undef SetTimer
#undef KillTimer

#endif // NO_DEBUG_WINDOW

#ifndef NO_DEBUG_IMAGE

#undef CreateBitmapFromImageFile
#undef CreateBitmapFromImageData
#undef ConvertBitmap

#undef CreateImageFromFile
#undef CreateImageFromData
#undef DeleteImage

#undef StartAnimatedGIFFromFile
#undef StartAnimatedGIFFromData
#undef EndAnimatedGIF

#endif // NO_DEBUG_IMAGE

// 定义对象类型
enum
{
    OT_NULL,            // unused
    OT_CRTMEMORY,       // malloc, realloc & free & calloc
    OT_GLOBALMEMORY,    // GlobalAlloc & GlobalFree
    OT_LOCALMEMORY,     // LocalAlloc, LocalRealloc & LocalFree

    OT_FILE,            // CreateFile & CloseFile , open & close & create
    OT_FINDFIRSTFILE,   // FindFirstFile & FindClose

	OT_DIR,				// opendir & closedir

    OT_GDI,             // Create(bitmap, pen, brush, font) & DeleteObject
    OT_DC,              // Create(memory, compatible, screen)DC & DeleteDC
    OT_WINDOWDC,        // GetDC, GetWindowDC & ReleaseDC
    OT_ICON,            // LoadIcon,  CreateIcon & DestroyIcon
    OT_CURSOR,          // LoadCursor, CreateCursor & DestroyCursor
    OT_WINDOWCLASS,     // RegisterClass, RegisterClassEx & UnregisterClass
	OT_TIMER,

    OT_IMAGE,           // CreateImageFromFile(Data) & DeleteImage
    OT_ANIMATEDGIF,     // StartAnimatedGIFFromFile(Data) & EndAnimatedGif
    OT_MAX
};

const char* ObjectNames[OT_MAX] = 
{
    "",                     // unused
    "malloc Memory",        // malloc, realloc & free
    "Global Memory",        // GlobalAlloc
    "Local Memory",         // LocalAlloc, LocalRealloc

    "File Handle",          // CreateFile
    "FindFirstFile Handle", // FindFirstFile
	"Dir Handle",			// opendir & closedir

    "GDI",                  // Create(bitmap, pen, brush, font)
    "DC",                   // Create(memory, compatible, screen)DC
    "Window DC",            // GetDC, GetWindowDC
    "Icon",                 // LoadIcon,  CreateIcon
    "Cursor",               // LoadCursor, CreateCursor
    "Window Class",         // RegisterClass, RegisterClassEx
	"Timer",

    "Image",                // CreateImageFromFile(Data)
    "Animated Gif",         // StartAnimatedGIFFromFile(Data)
};

static BOOL RegisterObject(const char* pszModule, const char* pszFile, 
                           int nLine, int nType, BOOL bString, 
                           DWORD dwObject);
static BOOL UnregisterObject(int nType, DWORD dwObject);
static BOOL RegisterObjectEx(const char* pszModule, const char* pszFile, 
                           int nLine, int nType, BOOL bString, 
                           DWORD dwObject, DWORD dwObject1, DWORD dwObject2);
static BOOL UnregisterObjectEx(int nType, DWORD dwObject, DWORD dwObject1, DWORD dwObject2);
static void UnregisterModuleObjects(int nModule);
static BOOL DumpModuleObjects(int nModule);

/*************************************************************************/
/*              对外提供的接口函数
/*************************************************************************/

typedef struct tagMODULEINFO
{
    const char* pszName;
    BOOL        bEnable;
} MODULEINFO, *PMODULEINFO;

#define MAX_MODULE_NUM  64
static MODULEINFO Modules[MAX_MODULE_NUM];

static int GetModuleIndex(const char* pszModule)
{
    int i;

    for (i = 0; i < MAX_MODULE_NUM; i++)
    {
        if (Modules[i].pszName)
        {
            if (strcmp(Modules[i].pszName, pszModule) == 0)
                return i;
        }
    }

    return -1;
}

/*
**  Function : DBG_StartObjectDebug
**  Purpose  :
**      
*/
BOOL DBG_StartObjectDebug(const char* pszModule)
{
    int i;
    int nUnused = -1;

    if (*pszModule == '\0')
        return FALSE;

    for (i = 0; i < MAX_MODULE_NUM; i++)
    {
        if (Modules[i].pszName)
        {
            if (strcmp(Modules[i].pszName, pszModule) == 0)
            {
                UnregisterModuleObjects(i);
                return TRUE;
            }
        }
        else if (nUnused == -1)
            nUnused = i;
    }

    if (nUnused == -1)
        return FALSE;
    
    Modules[nUnused].pszName = pszModule;
    Modules[nUnused].bEnable = TRUE;

    return TRUE;
}

/*
**  Function : DBG_EnableObjectDebug
**  Purpose  :
**      
*/
BOOL DBG_EnableObjectDebug(const char* pszModule, BOOL bEnable)
{
    int i;

    if (*pszModule == '\0')
        return FALSE;

    for (i = 0; i < MAX_MODULE_NUM; i++)
    {
        if (Modules[i].pszName)
        {
            if (strcmp(Modules[i].pszName, pszModule) == 0)
            {
                Modules[i].bEnable = bEnable;
                return TRUE;
            }
        }
    }
    
    return FALSE;
}

/*
**  Function : DBG_EndObjectDebug
**  Purpose  :
**      
*/
BOOL DBG_EndObjectDebug(const char* pszModule)
{
    int nModule;

    if (*pszModule == '\0')
        return FALSE;

    nModule = GetModuleIndex(pszModule);

    if (nModule == -1)
        return FALSE;

    DumpModuleObjects(nModule);

    UnregisterModuleObjects(nModule);

    Modules[nModule].pszName = NULL;

    return TRUE;
}

/*
**  Function : DBG_DumpObjectLeaks
*/
BOOL DBG_DumpObjectLeaks(const char* pszModule)
{
    int nModule;

    if (*pszModule == '\0')
        return FALSE;

    nModule = GetModuleIndex(pszModule);

    if (nModule == -1)
        return FALSE;

    DumpModuleObjects(nModule);

    return TRUE;
}

/*************************************************************************/
/*              封装的内存管理相关函数
/*************************************************************************/

#ifndef NO_DEBUG_MEMORY

#ifdef HOPEN20

HGLOBAL DBG_GlobalAlloc(unsigned uFlags, unsigned nBytes, 
                        const char* pszModule, const char* pszFile, 
                        int nLine)
{
    HGLOBAL hMem;

    hMem = (void*)GlobalAlloc(uFlags, nBytes);

    if (!hMem)
        return NULL;

    RegisterObject(pszModule, pszFile, nLine, OT_GLOBALMEMORY, FALSE, 
        (DWORD)hMem);

    return hMem;
}

HGLOBAL DBG_GlobalFree(HGLOBAL hMem, const char* pszModule, 
                       const char* pszFile, int nLine)
{
    UnregisterObject(OT_GLOBALMEMORY, (DWORD)hMem);
    return (void*)GlobalFree(hMem);
}

HLOCAL DBG_LocalAlloc(unsigned uFlags, unsigned uBytes, 
                      const char* pszModule, const char* pszFile, 
                      int nLine)
{

    HLOCAL hMem;

    hMem = (void*)LocalAlloc(uFlags, uBytes);

    if (!hMem)
        return NULL;

    RegisterObject(pszModule, pszFile, nLine, OT_LOCALMEMORY, FALSE, 
        (DWORD)hMem);

    return hMem;
}

HLOCAL DBG_LocalRealloc(HLOCAL hMem, unsigned long newsize, 
                        unsigned int flags, const char* pszModule, 
                        const char* pszFile, int nLine)
{
    HLOCAL hNewMem;

    hNewMem = (void*)LocalRealloc(hMem, newsize, flags);

    if (hNewMem && hNewMem != hMem)
    {
        UnregisterObject(OT_LOCALMEMORY, (DWORD)hMem);
        RegisterObject(pszModule, pszFile, nLine, OT_LOCALMEMORY, FALSE, 
            (DWORD)hNewMem);
    }

    return hNewMem;
}

HLOCAL DBG_LocalFree(HLOCAL hMem, const char* pszModule, 
                     const char* pszFile, int nLine)
{
    UnregisterObject(OT_LOCALMEMORY, (DWORD)hMem);
    return (void*)free(hMem);
}

#else

void* DBG_malloc(unsigned int size, const char* pszModule, 
                 const char* pszFile, int nLine)
{
    void* pMem;

    pMem = (void*)malloc(size);

    if (!pMem)
        return NULL;

    RegisterObject(pszModule, pszFile, nLine, OT_CRTMEMORY, FALSE, 
        (DWORD)pMem);

    return pMem;
}

void* DBG_calloc(size_t nmemb, size_t size, const char* pszModule, 
                 const char* pszFile, int nLine)
{
    void* pMem;

    pMem = (void*)calloc(nmemb, size);

    if (!pMem)
        return NULL;

    RegisterObject(pszModule, pszFile, nLine, OT_CRTMEMORY, FALSE, 
        (DWORD)pMem);

    return pMem;
}

void* DBG_realloc(void* pMem, unsigned int newsize, const char* pszModule, 
                  const char* pszFile, int nLine)
{
    void* pNewMem;

    pNewMem = realloc(pMem, newsize);

    if (pNewMem && pNewMem != (void*)pMem)
    {
        UnregisterObject(OT_CRTMEMORY, (DWORD)pMem);
        RegisterObject(pszModule, pszFile, nLine, OT_CRTMEMORY, FALSE, 
            (DWORD)pNewMem);
    }

    return pNewMem;
}

void DBG_free(void* pMem, const char* pszModule, const char* pszFile, 
              int nLine)
{
    UnregisterObject(OT_CRTMEMORY, (DWORD)pMem);
    free(pMem);
}

#endif

#endif  // NO_DEBUG_MEMORY

/*************************************************************************/
/*              封装的文件系统相关函数
/*************************************************************************/

#ifndef NO_DEBUG_FS

#ifndef HOPEN20

extern int sys_open (const char * filename, int flags, int mode);

#include <stdarg.h>

static	int fileopened = 0;

/*
 *	return the number of opened files registered in this module.
 */
int	OpenedFileNumber(void)
{
	return fileopened;
}

int DBG_open(const char* filename, int flags, ...)
{
	int fd;
	int mode;

	va_list varg;
	va_start( varg, flags );
	mode = va_arg (varg, int);
	va_end( varg );

	fd = sys_open (filename, flags, mode);
	if ( -1 != fd)
	{
		if ( RegisterObject(__MODULE__, filename, 0, OT_FILE, FALSE, (DWORD)fd) )
		{
			//printf("\r\nfile %s was opened, the file id is %d\r\n",filename,fd);
			fileopened ++;
		}
	}
	else
	{
		//printf("\r\n!!!!!!!!!!!!!! open file failed. !!!!!!!!!!!!!!!!!\r\n");
		//printf("The file is %s \r\n",filename);
	}

	return fd;
}

int DBG_creat(const char * pathname, mode_t mode,
			   const char* pszModule, const char* pszFile, int nLine)
{
	int fd;

	fd = creat(pathname,mode);
	if ( -1!=fd)
	{
		if ( RegisterObject(pszModule, pszFile, nLine, OT_FILE, FALSE, (DWORD)fd) )
		{
			//printf("\r\n!!!!! application %s open file %s, the id is %d !!!!!\r\n",pszFile,pathname,fd);
			fileopened ++;
		}
	}
	else
	{
		//printf("\r\n!!!!!!!!!!!!!! create file failed. !!!!!!!!!!!!!!!!!\r\n");
		//printf("Application %s create file %s failed. \r\n",pszFile,pathname);
	}

	return fd;
}

int DBG_close(int fd, const char* pszModule, const char* pszFile, int nLine)
{
	int ret;

	if ( -1==fd )
	{
		printf("\r\n!!!!!!!!!!!!! Close file id is -1 !!!!!!!!!!!!\r\n");
		printf("In application:\r\n");
		printf("%s\r\n",pszFile);
		printf("Line:%d\r\n",nLine);
	}

	ret = close(fd);
	if ( 0==ret )
	{
		if ( UnregisterObject(OT_FILE, (DWORD)fd) )
			if ( -1 != fd )
				fileopened --;
	}

	return ret;
}

static	int diropened = 0;

int	OpenedDirNumber(void)
{
	return diropened;
}

DIR * DBG_opendir(const char* dirname, const char* pszModule, const char* pszFile, int nLine)
{
	DIR * dir;

	dir = opendir(dirname);
	if ( NULL!=dir )
	{
		if ( RegisterObject(__MODULE__, dirname, 0, OT_DIR, FALSE, (DWORD)dir) )
			diropened ++;
	}
	return dir;
}

int	DBG_closedir(DIR * dir, const char* pszModule, const char* pszFile, int nLine)
{
	int ret;

	ret = closedir(dir);
	if ( 0 == ret )
	{
		if ( UnregisterObject(OT_DIR, (DWORD)dir) )
			diropened --;
	}
	return ret;
}

#else

HANDLE DBG_CreateFile(PCSTR pFileName, DWORD dwMode, DWORD dwAttrib, 
                      const char* pszModule, const char* pszFile, 
                      int nLine)
{
    HANDLE hFile;

    hFile = CreateFile(pFileName, dwMode, dwAttrib);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_FILE, FALSE, 
            (DWORD)hFile);
    }

    return hFile;
}

BOOL DBG_CloseFile(HANDLE hFile, const char* pszModule, 
                   const char* pszFile, int nLine)
{
    BOOL bRet;

    bRet = CloseFile(hFile);
    if (bRet)
    {
        UnregisterObject(OT_FILE, (DWORD)hFile);
    }

    return bRet;
}

HANDLE DBG_FindFirstFile(PCSTR lpFileName, P_FIND_DATA pFindData, 
                         const char* pszModule, const char* pszFile, 
                         int nLine)
{
    HANDLE hFindFile;

    hFindFile = FindFirstFile(lpFileName, pFindData);
    if (hFindFile != INVALID_HANDLE_VALUE)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_FINDFIRSTFILE, FALSE, 
            (DWORD)hFindFile);
    }

    return hFindFile;
}

BOOL DBG_FindClose(HANDLE hFindFile, const char* pszModule, 
                   const char* pszFile, int nLine)
{
    BOOL bRet;

    bRet = FindClose(hFindFile);
    if (bRet)
    {
        UnregisterObject(OT_FINDFIRSTFILE, (DWORD)hFindFile);
    }

    return bRet;
}

#endif

#endif  // NO_DEBUG_FS

/*************************************************************************/
/*              封装的窗口系统相关函数
/*************************************************************************/

#ifndef NO_DEBUG_WINDOW

/*************************************************************************/
/*              封装的GDI对象相关函数                                    */
/*************************************************************************/

HPEN DBG_CreatePen(int nStyle, int nWidth, COLORREF color, 
                   const char* pszModule, const char* pszFile, 
                   int nLine)
{
    HPEN hPen;

    hPen = CreatePen(nStyle, nWidth, color);
    if (hPen)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hPen);
    }

    return hPen;
}

HPEN DBG_CreatePenIndirect(const LOGPEN* pLogPen, const char* pszModule, 
                           const char* pszFile, int nLine)
{
    HPEN hPen;

    hPen = CreatePenIndirect(pLogPen);
    if (hPen)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hPen);
    }

    return hPen;
}

HBRUSH DBG_CreateBrush(int nStyle, COLORREF color, LONG lHatch, 
                       const char* pszModule, const char* pszFile, 
                       int nLine)
{
    HBRUSH hBrush;

    hBrush = CreateBrush(nStyle, color, lHatch);
    if (hBrush)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBrush);
    }

    return hBrush;
}

HBRUSH DBG_CreateSolidBrush(COLORREF color, const char* pszModule, 
                            const char* pszFile, int nLine)
{
    HBRUSH hBrush;

    hBrush = CreateSolidBrush(color);
    if (hBrush)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBrush);
    }

    return hBrush;
}

HBRUSH DBG_CreateHatchBrush(int nHatchStyle, COLORREF color, 
                            const char* pszModule, const char* pszFile, 
                            int nLine)
{
    HBRUSH hBrush;

    hBrush = CreateHatchBrush(nHatchStyle, color);
    if (hBrush)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBrush);
    }

    return hBrush;
}

HBRUSH DBG_CreatePatternBrush(HBITMAP hBitmap, const char* pszModule, 
                              const char* pszFile, int nLine)
{
    HBRUSH hBrush;

    hBrush = CreatePatternBrush(hBitmap);
    if (hBrush)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBrush);
    }

    return hBrush;
}

HBRUSH DBG_CreateBrushIndirect(const LOGBRUSH* pLogBrush, 
                               const char* pszModule, const char* pszFile, 
                               int nLine)
{
    HBRUSH hBrush;

    hBrush = CreateBrushIndirect(pLogBrush);
    if (hBrush)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBrush);
    }

    return hBrush;
}

HFONT DBG_CreateFont(int nHeight, int nWidth, int nEscapement, 
                     int nOrientation, int fnWeight, DWORD fdwItalic, 
                     DWORD fdwUnderline, DWORD fdwStrikeOut, 
                     DWORD fdwCharSet, DWORD fdwOutputPrecision, 
                     DWORD fdwClipPrecision, DWORD fdwQuality, 
                     DWORD fdwPitchAndFamily, PCSTR pszFace, 
                     const char* pszModule, const char* pszFile, 
                     int nLine)
{
    HFONT hFont;
    
    hFont = CreateFont(nHeight, nWidth, nEscapement, nOrientation, 
        fnWeight, fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet, 
        fdwOutputPrecision, fdwClipPrecision, fdwQuality, 
        fdwPitchAndFamily, pszFace);
    if (hFont)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hFont);
    }
    
    return hFont;
}

HFONT DBG_CreateFontIndirect(const LOGFONT* lplf, const char* pszModule, 
                             const char* pszFile, int nLine)
{
    HFONT hFont;
    
    hFont = CreateFontIndirect(lplf);
    if (hFont)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hFont);
    }
    
    return hFont;
}

HBITMAP DBG_CreateBitmap(int nWidth, int nHeight, UINT cPlanes, 
                         UINT cBitsPerPel, const void* lpvBits, 
                         const char* pszModule, const char* pszFile, 
                         int nLine)
{
    HBITMAP hBitmap;
    
    hBitmap = CreateBitmap(nWidth, nHeight, cPlanes, cBitsPerPel, lpvBits);
    if (hBitmap)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBitmap);
    }
    
    return hBitmap;
}

HBITMAP DBG_CreateBitmapEx(int nWidth, int nHeight, UINT cPlanes, 
                           UINT cBitsPerPel, const void* lpvBits, 
                           BOOL bUserBuf, const char* pszModule, 
                           const char* pszFile, int nLine)
{
    HBITMAP hBitmap;
    
    hBitmap = CreateBitmapEx(nWidth, nHeight, cPlanes, cBitsPerPel, 
        lpvBits, bUserBuf);
    if (hBitmap)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBitmap);
    }
    
    return hBitmap;
}

HBITMAP DBG_CreateBitmapIndirect(const BITMAP* pBitmap, 
                                 const char* pszModule, const char* pszFile,
                                 int nLine)
{
    HBITMAP hBitmap;
    
    hBitmap = CreateBitmapIndirect(pBitmap);
    if (hBitmap)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBitmap);
    }
    
    return hBitmap;
}

HBITMAP DBG_CreateCompatibleBitmap(HDC hdc, int nWidth, int nHeight, 
                                   const char* pszModule, 
                                   const char* pszFile, int nLine)
{
    HBITMAP hBitmap;
    
    hBitmap = CreateCompatibleBitmap(hdc, nWidth, nHeight);
    if (hBitmap)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBitmap);
    }
    
    return hBitmap;
}

HBITMAP DBG_CreateDIBitmap(HDC hdc, BITMAPINFOHEADER* pbmih, 
                           DWORD dwInitFlag, const void* pInitData, 
                           BITMAPINFO* pbmi, UINT fuUsage, 
                           const char* pszModule, const char* pszFile, 
                           int nLine)
{
    HBITMAP hBitmap;
    
    hBitmap = CreateDIBitmap(hdc, pbmih, dwInitFlag, pInitData, pbmi, 
        fuUsage);
    if (hBitmap)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBitmap);
    }
    
    return hBitmap;
}

HBITMAP DBG_LoadBitmap(HINSTANCE hInst, PCSTR pszBitmapName, 
                       const char* pszModule, const char* pszFile, 
                       int nLine)
{
    HBITMAP hBitmap;
    
    hBitmap = LoadBitmap(hInst, pszBitmapName);
    if (hBitmap)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBitmap);
    }
    
    return hBitmap;
}

HANDLE DBG_LoadImage(HINSTANCE hInst, PCSTR pszName, UINT uType, 
                     int cxDesired, int cyDesired, UINT fuLoad, 
                     const char* pszModule, const char* pszFile, 
                     int nLine)
{
    HANDLE hImage;
    int nType;
    
    hImage = LoadImage(hInst, pszName, uType, cxDesired, cyDesired, fuLoad);
    if (hImage)
    {
        switch (uType)
        {
        case IMAGE_BITMAP : 

            nType = OT_GDI;
            break;

        case IMAGE_ICON : 

            nType = OT_ICON;
            break;

        case IMAGE_CURSOR : 

            nType = OT_CURSOR;
            break;

        default :

            return hImage;

        }
        
        RegisterObject(pszModule, pszFile, nLine, nType, FALSE, 
            (DWORD)hImage);
    }
    
    return hImage;
}

HANDLE DBG_LoadImageEx(HDC hdc, HINSTANCE hInst, LPCSTR pszName, 
                       UINT uType, int cxDesired, int cyDesired, 
                       UINT fuLoad, const char* pszModule, 
                       const char* pszFile, int nLine)
{
    HANDLE hImage;
    int nType;
    
    hImage = LoadImageEx(hdc, hInst, pszName, uType, cxDesired, cyDesired, 
        fuLoad);
    if (hImage)
    {
        switch (uType)
        {
        case IMAGE_BITMAP : 

            nType = OT_GDI;
            break;

        case IMAGE_ICON : 

            nType = OT_ICON;
            break;

        case IMAGE_CURSOR : 

            nType = OT_CURSOR;
            break;

        default :

            return hImage;

        }
        
        RegisterObject(pszModule, pszFile, nLine, nType, FALSE, 
            (DWORD)hImage);
    }
    
    return hImage;
}

BOOL DBG_DeleteObject(HGDIOBJ hGdiObj, const char* pszModule, 
                      const char* pszFile, int nLine)
{
    BOOL bRet;

    bRet = DeleteObject(hGdiObj);
    if (bRet)
    {
        UnregisterObject(OT_GDI, (DWORD)hGdiObj);
    }

    return bRet;
}

/*************************************************************************/
/*              封装的DC对象相关函数                                     */
/*************************************************************************/

HDC DBG_CreateDC(LPCTSTR lpszDriver,LPCTSTR lpszDevice,LPCTSTR lpszOutput,
                 const void* lpInitData, const char* pszModule, 
                 const char* pszFile, int nLine)
{
	HDC hdc;

	hdc = CreateDC(lpszDriver, lpszDevice, lpszOutput, lpInitData);
	if (hdc)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_DC, FALSE, 
            (DWORD)hdc);
    }
    
    return hdc;
}

HDC DBG_CreateScreenDC(const char* pszModule, const char* pszFile, 
                       int nLine)
{
    HDC hdc;
    
    hdc = CreateScreenDC();
    if (hdc)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_DC, FALSE, 
            (DWORD)hdc);
    }
    
    return hdc;
}

HDC DBG_CreateCompatibleDC(HDC hdc, const char* pszModule, 
                           const char* pszFile, int nLine)
{
    HDC hdcNew;
    
    hdcNew = CreateCompatibleDC(hdc);
    if (hdcNew)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_DC, FALSE, 
            (DWORD)hdcNew);
    }
    
    return hdcNew;
}

HDC DBG_CreateMemoryDC(int nWidth, int nHeight, const char* pszModule, 
                       const char* pszFile, int nLine)
{
    HDC hdc;
    
    hdc = CreateMemoryDC(nWidth, nHeight);
    if (hdc)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_DC, FALSE, 
            (DWORD)hdc);
    }
    
    return hdc;
}

HDC DBG_CreatePrinterDC(int width, int height, const char* pszModule, 
                        const char* pszFile, int nLine)
{
    return NULL;
/*
    HDC hdc;
    
    hdc = CreatePrinterDC(width, height);
    if (hdc)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_DC, FALSE, 
            (DWORD)hdc);
    }
    
    return hdc;
*/
}

BOOL DBG_DeleteDC(HDC hdc, const char* pszModule, const char* pszFile, 
                  int nLine)
{
    BOOL bRet;

    bRet = DeleteDC(hdc);
    if (bRet)
    {
        UnregisterObject(OT_DC, (DWORD)hdc);
    }

    return bRet;
}

/*************************************************************************/
/*              封装的Window DC对象相关函数
/*************************************************************************/

HDC DBG_GetDC(HWND hWnd, const char* pszModule, const char* pszFile, 
              int nLine)
{
    HDC hdc;
    
    hdc = GetDC(hWnd);
    if (hdc)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_WINDOWDC, FALSE, 
            (DWORD)hdc);
    }
    
    return hdc;
}

HDC DBG_GetWindowDC(HWND hWnd, const char* pszModule, const char* pszFile, 
                    int nLine)
{
    HDC hdc;
    
    hdc = GetWindowDC(hWnd);
    if (hdc)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_WINDOWDC, FALSE, 
            (DWORD)hdc);
    }
    
    return hdc;
}

int DBG_ReleaseDC(HWND hWnd, HDC hdc, const char* pszModule, 
                  const char* pszFile, int nLine)
{
    BOOL bRet;

    bRet = ReleaseDC(hWnd, hdc);
    if (bRet)
    {
        UnregisterObject(OT_WINDOWDC, (DWORD)hdc);
    }

    return bRet;
}

/*************************************************************************/
/*              封装的Icon对象相关函数
/*************************************************************************/

HICON DBG_LoadIcon(HINSTANCE hInstance, PCSTR pszIconName, 
                   const char* pszModule, const char* pszFile, int nLine)
{
    HICON hIcon;
    
    hIcon = LoadIcon(hInstance, pszIconName);
    if (hIcon)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_ICON, FALSE, 
            (DWORD)hIcon);
    }
    
    return hIcon;
}

HICON DBG_CreateIcon(HINSTANCE hInstance, int nWidth, int nHeight, 
                     int cPlanes, int cBitsPixel,const void* pANDBits, 
                     const void* pXORBits, const char* pszModule, 
                     const char* pszFile, int nLine)
{
    HICON hIcon;
    
    hIcon = CreateIcon(hInstance, nWidth, nHeight, cPlanes, cBitsPixel, 
        pANDBits, pXORBits);
    if (hIcon)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_ICON, FALSE, 
            (DWORD)hIcon);
    }
    
    return hIcon;
}

BOOL DBG_DestroyIcon(HICON hIcon, const char* pszModule, 
                     const char* pszFile, int nLine)
{
    BOOL bRet;

    bRet = DestroyIcon(hIcon);
    if (bRet)
    {
        UnregisterObject(OT_ICON, (DWORD)hIcon);
    }

    return bRet;
}

/*************************************************************************/
/*              封装的Cursor对象相关函数
/*************************************************************************/

HCURSOR DBG_LoadCursor(HINSTANCE hInstance, PCSTR pszCursorName, 
                       const char* pszModule, const char* pszFile, 
                       int nLine)
{
    HCURSOR hCursor;
    
    hCursor = LoadCursor(hInstance, pszCursorName);
    if (hCursor)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_CURSOR, FALSE, 
            (DWORD)hCursor);
    }
    
    return hCursor;
}

HCURSOR DBG_CreateCursor(HINSTANCE hInstance, int nHotX, int nHotY, 
                         int nWidth, int nHeight, const void* pANDPlane, 
                         const void* pXORPlane, const char* pszModule, 
                         const char* pszFile, int nLine)
{
    HCURSOR hCursor;
    
    hCursor = CreateCursor(hInstance, nHotX, nHotY, nWidth, nHeight, 
        pANDPlane, pXORPlane);
    if (hCursor)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_CURSOR, FALSE, 
            (DWORD)hCursor);
    }
    
    return hCursor;
}

BOOL DBG_DestroyCursor(HCURSOR hCursor, const char* pszModule, 
                       const char* pszFile, int nLine)
{
    BOOL bRet;

    bRet = DestroyCursor(hCursor);
    if (bRet)
    {
        UnregisterObject(OT_CURSOR, (DWORD)hCursor);
    }

    return bRet;
}

BOOL DBG_RegisterClass(const WNDCLASS* pWndClass, const char* pszModule, 
                       const char* pszFile, int nLine)
{
    BOOL bRet;
    
    bRet = RegisterClass(pWndClass);
    if (bRet)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_WINDOWCLASS, TRUE, 
            (DWORD)(pWndClass->lpszClassName));
    }

    return bRet;
}

BOOL DBG_RegisterClassEx(CONST WNDCLASSEX* lpwcx, const char* pszModule, 
                         const char* pszFile, int nLine)
{
    BOOL bRet;
    
    bRet = RegisterClassEx(lpwcx);
    if (bRet)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_WINDOWCLASS, TRUE, 
            (DWORD)(lpwcx->lpszClassName));
    }

    return bRet;
}

BOOL DBG_UnregisterClass(PCSTR pszClassName, HINSTANCE hInst, 
                         const char* pszModule, const char* pszFile, 
                         int nLine)
{
    BOOL bRet;
    
    bRet = UnregisterClass(pszClassName, hInst);
    if (bRet)
    {
		UnregisterObject(OT_WINDOWCLASS, (DWORD)pszClassName);
    }

    return bRet;
}

UINT	DBG_SetTimer(HWND hWnd, UINT uIDEvent, UINT uElapse, TIMERPROC pTimerFunc,
					 const char* pszModule, const char* pszFile, int nLine)
{
	UINT bRet;
	bRet = SetTimer(hWnd,uIDEvent,uElapse,pTimerFunc);
	if (bRet)
	{
		RegisterObjectEx(pszModule, pszFile, nLine, OT_TIMER, FALSE, (UINT)hWnd, bRet, (DWORD)NULL);
	}
	return bRet;
}

BOOL	DBG_KillTimer(HWND hWnd, UINT uIDEvent,
					 const char* pszModule, const char* pszFile, int nLine)
{
	BOOL bRet;
	bRet = KillTimer(hWnd, uIDEvent);
	if (bRet)
	{
		UnregisterObjectEx(OT_TIMER, (UINT)hWnd, uIDEvent, (DWORD)NULL);
	}
	return bRet;
}

#endif  // NO_DEBUG_WINDOW

/*************************************************************************/
/*              封装的图像库对象相关函数
/*************************************************************************/

#ifndef NO_DEBUG_IMAGE

/*************************************************************************/
/*              封装的Bitmap对象相关函数
/*************************************************************************/

HBITMAP DBG_CreateBitmapFromImageFile(HDC hdc, PCSTR pszImageFile,
                                      PCOLORREF pColor, BOOL *pbTran, 
                                      const char* pszModule, 
                                      const char* pszFile, int nLine)
{
    HBITMAP hBitmap;
    
    hBitmap = CreateBitmapFromImageFile(hdc, pszImageFile, pColor, pbTran);
    if (hBitmap)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBitmap);
    }
    
    return hBitmap;
}

HBITMAP DBG_CreateBitmapFromImageData(HDC hdc, const void* pImageData, 
                                      DWORD dwDataSize, PCOLORREF pColor, 
                                      BOOL *pbTran, const char* pszModule, 
                                      const char* pszFile, int nLine)
{
    HBITMAP hBitmap;
    
    hBitmap = CreateBitmapFromImageData(hdc, pImageData, dwDataSize, 
        pColor, pbTran);
    if (hBitmap)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBitmap);
    }
    
    return hBitmap;
}

HBITMAP DBG_ConvertBitmap(HDC hdc, HBITMAP hBmp, int nSrcX, int nSrcY, 
                          int nWidth, int nHeight, int nAngle, SIZE* pSize, 
                          const char* pszModule, const char* pszFile, 
                          int nLine)
{
    HBITMAP hBitmap;
    
    hBitmap = ConvertBitmap(hdc, hBmp, nSrcX, nSrcY, nWidth, nHeight, 
        nAngle, pSize);
    if (hBitmap)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_GDI, FALSE, 
            (DWORD)hBitmap);
    }
    
    return hBitmap;
}

/*************************************************************************/
/*              封装的Image对象相关函数
/*************************************************************************/

HIMAGE DBG_CreateImageFromFile(PCSTR pImageFile, const char* pszModule, 
                               const char* pszFile, int nLine)
{
    HIMAGE hImage;
    
    hImage = CreateImageFromFile(pImageFile);
    if (hImage)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_IMAGE, FALSE, 
            (DWORD)hImage);
    }
    
    return hImage;
}

HIMAGE DBG_CreateImageFromData(const void* pImageData, DWORD dwDataSize, 
                               const char* pszModule, const char* pszFile, 
                               int nLine)
{
    HIMAGE hImage;
    
    hImage = CreateImageFromData(pImageData, dwDataSize);
    if (hImage)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_IMAGE, FALSE, 
            (DWORD)hImage);
    }
    
    return hImage;
}

BOOL DBG_DeleteImage(HIMAGE hImage, const char* pszModule, 
                     const char* pszFile, int nLine)
{
    BOOL bRet;

    bRet = DeleteImage(hImage);
    if (bRet)
    {
        UnregisterObject(OT_IMAGE, (DWORD)hImage);
    }

    return bRet;
}

/*************************************************************************/
/*              封装的Animated Gif对象相关函数
/*************************************************************************/

HGIFANIMATE DBG_StartAnimatedGIFFromFile(HWND hWnd, PCSTR pszGIFFile, 
                                         int x, int y, int dm, 
                                         const char* pszModule, 
                                         const char* pszFile, int nLine)
{
    HGIFANIMATE hGif;
    
    hGif = StartAnimatedGIFFromFile(hWnd, pszGIFFile, x, y, dm);
    if (hGif)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_ANIMATEDGIF, FALSE, 
            (DWORD)hGif);
    }
    
    return hGif;
}

HGIFANIMATE DBG_StartAnimatedGIFFromData(HWND hWnd, const void* pGIFData,
                                         DWORD dwDataSize, int x, int y, 
                                         int dm, const char* pszModule, 
                                         const char* pszFile, int nLine)
{
    HGIFANIMATE hGif;
    
    hGif = StartAnimatedGIFFromData(hWnd, pGIFData, dwDataSize, x, y, dm);
    if (hGif)
    {
        RegisterObject(pszModule, pszFile, nLine, OT_ANIMATEDGIF, FALSE, 
            (DWORD)hGif);
    }
    
    return hGif;
}

BOOL DBG_EndAnimatedGIF(HGIFANIMATE hGIFAnimate, const char* pszModule, 
                        const char* pszFile, int nLine)
{
    BOOL bRet;

    bRet = EndAnimatedGIF(hGIFAnimate);
    if (bRet)
    {
        UnregisterObject(OT_ANIMATEDGIF, (DWORD)hGIFAnimate);
    }

    return bRet;

}

#endif  \\ NO_DEBUG_IMAGE

/*************************************************************************/
/*              Internal functions
/*************************************************************************/
typedef struct tagOBJECTINFO
{
    char		pszFile[128];
    int         nLine;
    int         nModule;
    int         nType;
    BOOL        bString;
    DWORD       dwObject;
	DWORD		dwObject1;
	DWORD		dwObject2;
} OBJECTINFO, *POBJECTINFO;

#define MAX_OBJECT_NUM  1024

// we use static array in emulator and dynamic alloc memory on the board
#ifdef _MSC_VER
static OBJECTINFO ObjectInfos[MAX_OBJECT_NUM];
#else
static POBJECTINFO ObjectInfos;
#endif

static LRESULT DumpWndProc(HWND hWnd, UINT message, WPARAM wParam, 
                           LPARAM lParam);

static BOOL RegisterObject(const char* pszModule, const char* pszFile, 
                           int nLine, int nType, BOOL bString, 
                           DWORD dwObject)
{
    int i;
    int nModule;

    if (*pszModule == '\0')
        return FALSE;

    nModule = GetModuleIndex(pszModule);
    if (nModule == -1)
        return FALSE;

    if (!Modules[nModule].bEnable)
        return FALSE;

#ifndef _MSC_VER
    if (ObjectInfos == NULL)
    {
        ObjectInfos = malloc(sizeof(OBJECTINFO) * MAX_OBJECT_NUM);
		memset(ObjectInfos,0,sizeof(OBJECTINFO) * MAX_OBJECT_NUM );
    }
#endif

    for (i = 0; i < MAX_OBJECT_NUM; i++)
    {
        if (ObjectInfos[i].nType == OT_NULL)
            break;
    }

    if (i == MAX_OBJECT_NUM)
    {
        ASSERT(0);
        return FALSE;
    }

	if (strlen(pszFile) < 128)
		strcpy(ObjectInfos[i].pszFile, pszFile);
	else
	{
		strncpy(ObjectInfos[i].pszFile, pszFile, 127);
		ObjectInfos[i].pszFile[127] = 0;
	}

    ObjectInfos[i].nLine    = nLine;
    ObjectInfos[i].nModule  = nModule;
    ObjectInfos[i].nType    = nType;
    ObjectInfos[i].bString  = bString;
    ObjectInfos[i].dwObject = dwObject;
	ObjectInfos[i].dwObject1= (unsigned long)NULL;
	ObjectInfos[i].dwObject2= (unsigned long)NULL;

    return TRUE;
}

static BOOL UnregisterObject(int nType, DWORD dwObject)
{
    int i;

    if (ObjectInfos == NULL)
        return FALSE;

    for (i = 0; i < MAX_OBJECT_NUM; i++)
    {
        if (ObjectInfos[i].nType == nType)
        {
            if ((ObjectInfos[i].bString && 
                !strcmp((char*)dwObject, (char*)ObjectInfos[i].dwObject)) || 
                (!ObjectInfos[i].bString && 
                ObjectInfos[i].dwObject == dwObject))
                break;
        }
    }

    if (i == MAX_OBJECT_NUM)
        return FALSE;

    ObjectInfos[i].nType = OT_NULL;

    return TRUE;
}

static BOOL RegisterObjectEx(const char* pszModule, const char* pszFile, 
                           int nLine, int nType, BOOL bString, 
                           DWORD dwObject, DWORD dwObject1, DWORD dwObject2)
{
    int i;
    int nModule;

    if (*pszModule == '\0')
        return FALSE;

    nModule = GetModuleIndex(pszModule);
    if (nModule == -1)
        return FALSE;

    if (!Modules[nModule].bEnable)
        return FALSE;

#ifndef _MSC_VER
    if (ObjectInfos == NULL)
    {
        ObjectInfos = malloc(sizeof(OBJECTINFO) * MAX_OBJECT_NUM);
		memset(ObjectInfos,0,sizeof(OBJECTINFO) * MAX_OBJECT_NUM );
    }
#endif

    for (i = 0; i < MAX_OBJECT_NUM; i++)
    {
        if (ObjectInfos[i].nType == OT_NULL)
            break;
    }

    if (i == MAX_OBJECT_NUM)
    {
        ASSERT(0);
        return FALSE;
    }

//    ObjectInfos[i].pszFile  = pszFile;

	if (strlen(pszFile) < 128)
		strcpy(ObjectInfos[i].pszFile, pszFile);
	else
	{
		strncpy(ObjectInfos[i].pszFile, pszFile, 127);
		ObjectInfos[i].pszFile[127] = 0;
	}

    ObjectInfos[i].nLine    = nLine;
    ObjectInfos[i].nModule  = nModule;
    ObjectInfos[i].nType    = nType;
    ObjectInfos[i].bString  = bString;
    ObjectInfos[i].dwObject = dwObject;
	ObjectInfos[i].dwObject1= dwObject1;
	ObjectInfos[i].dwObject2= dwObject2;

    return TRUE;
}

static BOOL UnregisterObjectEx(int nType, DWORD dwObject, DWORD dwObject1, DWORD dwObject2)
{
    int i;

    if (ObjectInfos == NULL)
        return FALSE;

    for (i = 0; i < MAX_OBJECT_NUM; i++)
    {
        if (ObjectInfos[i].nType == nType)
        {
            if ((ObjectInfos[i].bString && 
                !strcmp((char*)dwObject, (char*)ObjectInfos[i].dwObject)) || 
                (!ObjectInfos[i].bString && 
				ObjectInfos[i].dwObject == dwObject &&
				ObjectInfos[i].dwObject1 == dwObject1 &&
				ObjectInfos[i].dwObject2 == dwObject2 ))
                break;
        }
    }

    if (i == MAX_OBJECT_NUM)
        return FALSE;

    ObjectInfos[i].nType = OT_NULL;

    return TRUE;
}

static void UnregisterModuleObjects(int nModule)
{
    int i;

    if (ObjectInfos == NULL)
        return;

    for (i = 0; i < MAX_OBJECT_NUM; i++)
    {
        if (ObjectInfos[i].nType != OT_NULL && 
            ObjectInfos[i].nModule == nModule)
        {
            ObjectInfos[i].nType = OT_NULL;
        }
    }
}

/*
 *	format the string to local character set
 *  this is mainly because benefon has its specified character set.
 */
static format(char *p)
{
	while( *p )
	{
		if ( '\\'==*p )
			*p = 0x2f;
		if ( '_'==*p )
			*p = 0x11;
		p++;
	}
}
static BOOL DumpModuleObjects(int nModule)
{
#define DUMPWNDCLASS  "DumpWndClass"

#define MODULE_DUMP_LEN     128
#define OBJECT_DUMP_LEN     320
#define MAX_TEXT_LEN        65536

    char achModule[33];
    void* pDumpBuf;
    char* pBuf;
    WNDCLASS wc;
    HWND hwndDump;
    int i, nFirst;
    int nNum, nMaxNum;

    if (ObjectInfos == NULL)
		return FALSE;
	
    for (i = 0; i < MAX_OBJECT_NUM; i++)
    {
        if (ObjectInfos[i].nType != OT_NULL &&
            ObjectInfos[i].nModule == nModule)
            break;
    }

    if (i == MAX_OBJECT_NUM)
        return FALSE;

    nFirst = i;

    nNum = 0;
    nMaxNum = (MAX_TEXT_LEN - MODULE_DUMP_LEN) / OBJECT_DUMP_LEN;

    for (; i < MAX_OBJECT_NUM; i++)
    {
        if (ObjectInfos[i].nType != OT_NULL &&
            ObjectInfos[i].nModule == nModule)
        {
            nNum++;
            if (nNum == nMaxNum)
                break;
        }
    }

    pDumpBuf = malloc(MODULE_DUMP_LEN + OBJECT_DUMP_LEN * nNum);
    if (!pDumpBuf)
        return FALSE;

    wc.style         = 0;
    wc.lpfnWndProc   = DumpWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = DUMPWNDCLASS;
    
    RegisterClass(&wc);

	DBG_EnableObjectDebug(__MODULE__, FALSE);
    hwndDump = CreateWindow(
        DUMPWNDCLASS, 
        "Object Leaks", 
        WS_VISIBLE | WS_CAPTION | WS_CLIPCHILDREN | PWS_STATICBAR,
        0, 
        0, 
        GetDeviceCaps(NULL, HORZRES), 
        GetDeviceCaps(NULL, VERTRES),
        NULL, 
        NULL,
        NULL, 
        NULL
        );
	DBG_EnableObjectDebug(__MODULE__, TRUE);

    if (!hwndDump)
    {
        free(pDumpBuf);
        return FALSE;
    }

    strncpy(achModule, Modules[nModule].pszName, 32);
    achModule[32] = 0;

    pBuf = pDumpBuf;
    pBuf += sprintf(pBuf, 
        "Module <%s> Dumping objects:\r\n\r\n", 
        achModule);

    for (i = nFirst; i < MAX_OBJECT_NUM; i++)
    {
        if (ObjectInfos[i].nType != OT_NULL &&
            ObjectInfos[i].nModule == nModule)
        {
			char * p;
			p = pBuf;
            pBuf += sprintf(pBuf, 
                "<%s>\r\nFile: %s\r\nLine: %d\r\n\r\n", 
                ObjectNames[ObjectInfos[i].nType], 
                ObjectInfos[i].pszFile, 
                ObjectInfos[i].nLine
                );

			format(p);

            nNum--;
            if (nNum == 0)
                break;
        }
    }

    SetWindowText(GetWindow(hwndDump, GW_CHILD), pDumpBuf);

    free(pDumpBuf);

    return TRUE;
}

static LRESULT DumpWndProc(HWND hWnd, UINT message, WPARAM wParam, 
                           LPARAM lParam)
{
    LRESULT lResult;
    RECT rcClient;
    
    switch (message)	
    {
    case WM_CREATE:
        
        GetClientRect(hWnd, &rcClient);
        
        CreateWindow(
            "EDIT", 
            "", 
            WS_CHILD | WS_VISIBLE | ES_UNDERLINE | ES_MULTILINE | 
            ES_READONLY | WS_VSCROLL, 
            0, 0, rcClient.right, rcClient.bottom, 
            hWnd, 
            (HMENU)100,
            NULL, 
            NULL
            );
        
        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDOK, 0), 
            (LPARAM)ML("Exit"));
        break;

    case WM_SETFOCUS :

        SetFocus(GetWindow(hWnd, GW_CHILD));
        break;

    case WM_COMMAND:

        if (LOWORD(wParam) == IDOK)
            DestroyWindow(hWnd);

        break;

    default :
        
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    
    return lResult;
}

#else   // DEBUG_OBJECT

static void UnusedFunc(void)
{
}

#endif
