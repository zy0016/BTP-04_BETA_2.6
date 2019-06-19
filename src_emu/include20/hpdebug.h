/**************************************************************************\
 *
 *                      Hopen Object Debug Library
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 *
 *
 * Filename     hpdebug.h
 *
 * Purpose      
 *  
 * Author       WeiZhong
 *
\**************************************************************************/

#ifndef __HPDEBUG_H
#define __HPDEBUG_H

// _DEBUG版本自动定义DEBUG_OBJECT
#ifdef _DEBUG
#ifndef DEBUG_OBJECT
#define DEBUG_OBJECT
#endif
#endif

#if (NO_DEBUG_MEMORY && NO_DEBUG_FS && NO_DEBUG_WINDOW && NO_DEBUG_IMAGE)
#ifdef DEBUG_OBJECT
#undef DEBUG_OBJECT
#endif
#endif

#ifdef DEBUG_OBJECT

#ifndef __MODULE__
#define __MODULE__  "HOPENDEBUG"
#endif

#include "hpdef.h"

#ifndef NO_DEBUG_MEMORY
#include "hp_kern.h"
#endif

#ifndef NO_DEBUG_FS
#include "fapi.h"
#endif

#ifndef NO_DEBUG_WINDOW
#include "window.h"
#endif

#ifndef NO_DEBUG_IMAGE
#include "hpimage.h"
#endif

/* Assume C declarations for C++ */
#ifdef __cplusplus
extern "C" {
#endif

BOOL DBG_StartObjectDebug(const char* pszModule);
BOOL DBG_EnableObjectDebug( const char* pszModule, BOOL bEnable);
BOOL DBG_EndObjectDebug(const char* pszModule);
BOOL DBG_DumpObjectLeaks(const char* pszModule);

#define StartObjectDebug()    DBG_StartObjectDebug(__MODULE__)
#define EnableObjectDebug(bEnable)  DBG_EnableObjectDebug(__MODULE__, bEnable)
#define EndObjectDebug()    DBG_EndObjectDebug(__MODULE__)
#define DumpObjectLeaks()   DBG_DumpObjectLeaks(__MODULE__)

#ifndef NO_DEBUG_MEMORY

HGLOBAL DBG_GlobalAlloc(unsigned uFlags, unsigned nBytes, 
                        const char* pszModule, const char* pszFile, 
                        int nLine);
HGLOBAL DBG_GlobalFree(HGLOBAL hMem, const char* pszModule, 
                       const char* pszFile, int nLine);

HLOCAL  DBG_LocalAlloc(unsigned uFlags, unsigned uBytes, 
                       const char* pszModule, const char* pszFile, 
                       int nLine);
HLOCAL  DBG_LocalRealloc(HLOCAL hMem, unsigned long newsize, 
                         unsigned int flags, const char* pszModule, 
                         const char* pszFile, int nLine);
HLOCAL  DBG_LocalFree(HLOCAL hMem, const char* pszModule, 
                      const char* pszFile, int nLine);

void*   DBG_malloc(unsigned int size, const char* pszModule, 
                   const char* pszFile, int nLine);
void*   DBG_realloc(void* pMem, unsigned int size, const char* pszModule, 
                   const char* pszFile, int nLine);
void    DBG_free(void* pMem, const char* pszModule, const char* pszFile, 
                 int nLine);

#define GlobalAlloc(uFlags, nBytes) \
    DBG_GlobalAlloc(uFlags, nBytes, __MODULE__, __FILE__, __LINE__)

#define GlobalFree(hMem)    \
    DBG_GlobalFree(hMem, __MODULE__, __FILE__, __LINE__);

#define LocalAlloc(uFlags, nBytes) \
    DBG_LocalAlloc(uFlags, nBytes, __MODULE__, __FILE__, __LINE__)

#define LocalRealloc(pMem, newsize, flags)  \
    DBG_LocalRealloc(pMem, newsize, flags, __MODULE__, __FILE__, __LINE__)

#define LocalFree(hMem)     \
    DBG_LocalFree(hMem, __MODULE__, __FILE__, __LINE__)

#ifdef malloc
#undef malloc
#endif

#ifdef realloc
#undef realloc
#endif

#ifdef free
#undef free
#endif

#define malloc(size)        DBG_malloc(size, __MODULE__, __FILE__, __LINE__)
#define realloc(mem, size)  \
    DBG_realloc(mem, size, __MODULE__, __FILE__, __LINE__)
#define free(mem)           DBG_free(mem, __MODULE__, __FILE__, __LINE__)

#endif  // NO_DEBUG_MEORY

#ifndef NO_DEBUG_FS

/* open/close file */
HANDLE  DBG_CreateFile(PCSTR pFileName, DWORD dwMode, DWORD dwAttrib, 
                       const char* pszModule, const char* pszFile, 
                       int nLine);
BOOL    DBG_CloseFile(HANDLE hFile, const char* pszModule, 
                      const char* pszFile, int nLine);

/* find file */
HANDLE  DBG_FindFirstFile(PCSTR lpFileName, P_FIND_DATA pFindData, 
                          const char* pszModule, const char* pszFile, 
                          int nLine);
BOOL    DBG_FindClose(HANDLE hFindFile, const char* pszModule, 
                      const char* pszFile, int nLine);

#define CreateFile(pFileName, dwMode, dwAttrib)  \
    DBG_CreateFile(pFileName, dwMode, dwAttrib, __MODULE__, __FILE__, __LINE__)

#define CloseFile(hFile)    \
    DBG_CloseFile(hFile, __MODULE__, __FILE__, __LINE__)

#define FindFirstFile(lpFileName, pFindData) \
    DBG_FindFirstFile(lpFileName, pFindData, __MODULE__, __FILE__, __LINE__)

#define FindClose(hFindFile)    \
    DBG_FindClose(hFindFile, __MODULE__, __FILE__, __LINE__)

#endif  // NO_DEBUG_FS

#ifndef NO_DEBUG_WINDOW

HPEN    DBG_CreatePen(int nStyle, int nWidth, COLORREF color, 
                      const char* pszModule, const char* pszFile, 
                      int nLine);
HPEN    DBG_CreatePenIndirect(const LOGPEN* pLogPen, const char* pszModule, 
                              const char* pszFile, int nLine);

#define CreatePen(nStyle, nWidth, color) \
    DBG_CreatePen(nStyle, nWidth, color, __MODULE__, __FILE__, __LINE__)
#define CreatePenIndirect(pLogPen) \
    DBG_CreatePenIndirect(pLogPen, __MODULE__, __FILE__, __LINE__)


HBRUSH  DBG_CreateBrush(int nStyle, COLORREF color, LONG lHatch, 
                        const char* pszModule, const char* pszFile, 
                        int nLine);
HBRUSH  DBG_CreateSolidBrush(COLORREF color, const char* pszModule, 
                             const char* pszFile, int nLine);
HBRUSH  DBG_CreateHatchBrush(int nHatchStyle, COLORREF color, 
                             const char* pszModule, const char* pszFile, 
                             int nLine);
HBRUSH  DBG_CreatePatternBrush(HBITMAP hBitmap, const char* pszModule, 
                               const char* pszFile, int nLine);
HBRUSH  DBG_CreateBrushIndirect(const LOGBRUSH* pLogBrush, 
                                const char* pszModule, const char* pszFile, 
                                int nLine);

#define CreateBrush(nStyle, color, lHatch)  \
    DBG_CreateBrush(nStyle, color, lHatch, __MODULE__, __FILE__, __LINE__)
#define CreateSolidBrush(color) \
    DBG_CreateSolidBrush(color, __MODULE__, __FILE__, __LINE__)
#define CreateHatchBrush(nHatchStyle, color) \
    DBG_CreateHatchBrush(nHatchStyle, color, __MODULE__, __FILE__, __LINE__)
#define CreatePatternBrush(hBitmap) \
    DBG_CreatePatternBrush(hBitmap, __MODULE__, __FILE__, __LINE__)
#define CreateBrushIndirect(pLogBrush) \
    DBG_CreateBrushIndirect(pLogBrush, __MODULE__, __FILE__, __LINE__)

HFONT   DBG_CreateFont(int nHeight, int nWidth, int nEscapement, 
                       int nOrientation, int fnWeight, DWORD fdwItalic, 
                       DWORD fdwUnderline, DWORD fdwStrikeOut, 
                       DWORD fdwCharSet, DWORD fdwOutputPrecision, 
                       DWORD fdwClipPrecision, DWORD fdwQuality, 
                       DWORD fdwPitchAndFamily, PCSTR pszFace, 
                       const char* pszModule, const char* pszFile, 
                       int nLine);
HFONT   DBG_CreateFontIndirect(const LOGFONT* lplf, const char* pszModule, 
                               const char* pszFile, int nLine);

#define CreateFont(nHeight, nWidth, nEscapement, nOrientation, fnWeight, \
                   fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet, \
                   fdwOutputPrecision, fdwClipPrecision, fdwQuality, \
                   fdwPitchAndFamily, pszFace) \
    DBG_CreateFont(nHeight, nWidth, nEscapement, nOrientation, fnWeight, \
                   fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet, \
                   fdwOutputPrecision, fdwClipPrecision, fdwQuality, \
                   fdwPitchAndFamily, pszFace, \
                   __MODULE__, __FILE__, __LINE__)

#define CreateFontIndirect(lplf) \
    DBG_CreateFontIndirect(lplf, __MODULE__, __FILE__, __LINE__)


HBITMAP DBG_CreateBitmap(int nWidth, int nHeight, UINT cPlanes, 
                         UINT cBitsPerPel, const void* lpvBits, 
                         const char* pszModule, const char* pszFile, 
                         int nLine);
HBITMAP DBG_CreateBitmapEx(int nWidth, int nHeight, UINT cPlanes, 
                           UINT cBitsPerPel, const void* lpvBits, 
                           BOOL bUserBuf, const char* pszModule, 
                           const char* pszFile, int nLine);
HBITMAP DBG_CreateBitmapIndirect(const BITMAP* pBitmap, 
                                 const char* pszModule, const char* pszFile,
                                 int nLine);
HBITMAP DBG_CreateCompatibleBitmap(HDC hdc, int nWidth, int nHeight, 
                                   const char* pszModule, 
                                   const char* pszFile, int nLine);

HBITMAP DBG_CreateDIBitmap(HDC hdc, BITMAPINFOHEADER* pbmih, 
                           DWORD dwInitFlag, const void* pInitData, 
                           BITMAPINFO* pbmi, UINT uUsage, 
                           const char* pszModule, const char* pszFile, 
                           int nLine);

HBITMAP DBG_LoadBitmap(HINSTANCE hInst, PCSTR pszBitmapName, 
                       const char* pszModule, const char* pszFile, 
                       int nLine);
HANDLE  DBG_LoadImage(HINSTANCE hInst, PCSTR pszName, UINT uType, 
                      int cxDesired, int cyDesired, UINT fuLoad, 
                      const char* pszModule, const char* pszFile, 
                      int nLine);
HANDLE  DBG_LoadImageEx(HDC hdc, HINSTANCE hInst, LPCSTR lpszName, 
                        UINT uType, int cxDesired, int cyDesired, 
                        UINT fuLoad, const char* pszModule, 
                        const char* pszFile, int nLine);

#define CreateBitmap(nWidth, nHeight, cPlanes, cBitsPerPel, lpvBits) \
    DBG_CreateBitmap(nWidth, nHeight, cPlanes, cBitsPerPel, lpvBits, \
                     __MODULE__, __FILE__, __LINE__)
#define CreateBitmapEx(nWidth, nHeight, cPlanes, cBits, lpvBits, bUserBuf) \
    DBG_CreateBitmapEx(nWidth, nHeight, cPlanes, cBits, lpvBits, bUserBuf, \
                       __MODULE__, __FILE__, __LINE__)
#define CreateBitmapIndirect(pBitmap) \
    DBG_CreateBitmapIndirect(pBitmap, __MODULE__, __FILE__, __LINE__)
#define CreateCompatibleBitmap(hdc, nWidth, nHeight) \
    DBG_CreateCompatibleBitmap(hdc, nWidth, nHeight, __MODULE__, __FILE__, \
                               __LINE__)
#define CreateDIBitmap(hdc, pbmih, dwInitFlag, pInitData, pbmi, uUsage) \
    DBG_CreateDIBitmap(hdc, pbmih, dwInitFlag, pInitData, pbmi, uUsage, \
                       __MODULE__, __FILE__, __LINE__)

#define LoadBitmap(hInst, pszBitmapName) \
    DBG_LoadBitmap(hInst, pszBitmapName, __MODULE__, __FILE__, __LINE__)
#define LoadImage(hInst, pszName, uType, cx, cy, fuLoad) \
    DBG_LoadImage(hInst, pszName, uType, cx, cy, fuLoad, \
                  __MODULE__, __FILE__, __LINE__)
#define LoadImageEx(hdc, hInst, lpszName, uType, cx, cy, fuLoad) \
    DBG_LoadImageEx(hdc, hInst, lpszName, uType, cx, cy, fuLoad, \
                    __MODULE__, __FILE__, __LINE__)

BOOL    DBG_DeleteObject(HGDIOBJ hGdiObj, const char* pszModule, 
                         const char* pszFile, int nLine);

#define DeleteObject(hGdiObj) \
    DBG_DeleteObject(hGdiObj, __MODULE__, __FILE__, __LINE__)

HDC     DBG_CreateDC(LPCTSTR lpszDriver, LPCTSTR lpszDevice, 
                     LPCTSTR lpszOutput, const void* lpInitData, 
                     const char* pszModule, const char* pszFile, int nLine);
HDC     DBG_CreateScreenDC(const char* pszModule, const char* pszFile, 
                           int nLine);
HDC     DBG_CreateCompatibleDC(HDC hdc, const char* pszModule, 
                               const char* pszFile, int nLine);
HDC     DBG_CreateMemoryDC(int nWidth, int nHeight, const char* pszModule, 
                           const char* pszFile, int nLine);
HDC     DBG_CreatePrinterDC(int width, int height, const char* pszModule, 
                            const char* pszFile, int nLine);
BOOL    DBG_DeleteDC(HDC hdc, const char* pszModule, const char* pszFile, 
                     int nLine);

#define CreateDC(lpszDriver, lpszDevice, lpszOutput, lpInitData) \
    DBG_CreateDC(lpszDriver, lpszDevice, lpszOutput, lpInitData, \
                 __MODULE__, __FILE__, __LINE__)
#define CreateScreenDC() \
    DBG_CreateScreenDC(__MODULE__, __FILE__, __LINE__)
#define CreateCompatibleDC(hdc) \
    DBG_CreateCompatibleDC(hdc, __MODULE__, __FILE__, __LINE__)
#define CreatePrinterDC(width, height) \
    DBG_CreatePrinterDC(width, height, __MODULE__, __FILE__, __LINE__)
#define CreateMemoryDC(nWidth, nHeight) \
    DBG_CreateMemoryDC(nWidth, nHeight, __MODULE__, __FILE__, __LINE__)
#define DeleteDC(hdc) \
    DBG_DeleteDC(hdc, __MODULE__, __FILE__, __LINE__)

HDC     DBG_GetDC(HWND hWnd, const char* pszModule, const char* pszFile, 
                  int nLine);
HDC     DBG_GetWindowDC(HWND hWnd, const char* pszModule, 
                        const char* pszFile, int nLine);
int     DBG_ReleaseDC(HWND hWnd, HDC hdc, const char* pszModule, 
                      const char* pszFile, int nLine);

#define GetDC(hWnd) \
    DBG_GetDC(hWnd, __MODULE__, __FILE__, __LINE__)
#define GetWindowDC(hWnd) \
    DBG_GetWindowDC(hWnd, __MODULE__, __FILE__, __LINE__)
#define ReleaseDC(hWnd, hdc) \
    DBG_ReleaseDC(hWnd, hdc, __MODULE__, __FILE__, __LINE__)

HICON   DBG_LoadIcon(HINSTANCE hInstance, PCSTR pszIconName, 
                     const char* pszModule, const char* pszFile, int nLine);
HICON   DBG_CreateIcon(HINSTANCE hInstance, int nWidth, int nHeight, 
                       int cPlanes, int cBitsPixel,const void* pANDBits, 
                       const void* pXORBits, const char* pszModule, 
                       const char* pszFile, int nLine);
BOOL    DBG_DestroyIcon(HICON hIcon, const char* pszModule, 
                        const char* pszFile, int nLine);

#define LoadIcon(hInst, pszIconName) \
    DBG_LoadIcon(hInst, pszIconName, __MODULE__, __FILE__, __LINE__)
#define CreateIcon(hInst, nWidth, nHeight, cPlanes, cBitsPixel, pAND, pXOR) \
    DBG_CreateIcon(hInst, nWidth, nHeight, cPlanes, cBitsPixel, pAND, pXOR, \
                   __MODULE__, __FILE__, __LINE__)
#define DestroyIcon(hIcon) \
    DBG_DestroyIcon(hIcon, __MODULE__, __FILE__, __LINE__)

HCURSOR DBG_LoadCursor(HINSTANCE hInstance, PCSTR pszCursorName, 
                       const char* pszModule, const char* pszFile, int nLine);
HCURSOR DBG_CreateCursor(HINSTANCE hInstance, int nHotX, int nHotY, 
                         int nWidth, int nHeight, const void* pANDPlane, 
                         const void* pXORPlane, const char* pszModule, 
                         const char* pszFile, int nLine);
BOOL    DBG_DestroyCursor(HCURSOR hCursor, const char* pszModule, 
                          const char* pszFile, int nLine);

#define LoadCursor(hInstance, pszCursorName) \
    DBG_LoadCursor(hInstance, pszCursorName, __MODULE__, __FILE__, __LINE__)
#define CreateCursor(hInstance, nHotX, nHotY, nWidth, nHeight, pAND, pXOR) \
    DBG_CreateCursor(hInstance, nHotX, nHotY, nWidth, nHeight, pAND, pXOR, \
                     __MODULE__, __FILE__, __LINE__)
#define DestroyCursor(hCursor) \
    DBG_DestroyCursor(hCursor, __MODULE__, __FILE__, __LINE__)

BOOL DBG_RegisterClass(const WNDCLASS* pWndClass, const char* pszModule, 
                       const char* pszFile, int nLine);
BOOL DBG_RegisterClassEx(CONST WNDCLASSEX* lpwcx, const char* pszModule, 
                         const char* pszFile, int nLine);
BOOL DBG_UnregisterClass(PCSTR pszClassName, HINSTANCE hInst, 
                         const char* pszModule, const char* pszFile, 
                         int nLine);

#define RegisterClass(pWndClass) \
    DBG_RegisterClass(pWndClass, __MODULE__, __FILE__, __LINE__)
#define RegisterClassEx(lpwcx) \
        DBG_RegisterClassEx(lpwcx, __MODULE__, __FILE__, __LINE__)
#define UnregisterClass(pszClassName, hInst) \
    DBG_UnregisterClass(pszClassName, hInst, __MODULE__, __FILE__, __LINE__)

#endif // NO_DEBUG_WINDOW

#ifndef NO_DEBUG_IMAGE

HBITMAP DBG_CreateBitmapFromImageFile(HDC hdc, PCSTR pszImageFile,
                                      PCOLORREF pColor, BOOL *pbTran, 
                                      const char* pszModule, 
                                      const char* pszFile, int nLine);
HBITMAP DBG_CreateBitmapFromImageData(HDC hdc, const void* pImageData, 
                                      DWORD dwDataSize, PCOLORREF pColor, 
                                      BOOL *pbTran, const char* pszModule, 
                                      const char* pszFile, int nLine);

HBITMAP DBG_ConvertBitmap(HDC hdc, HBITMAP hBmp, int nSrcX, int nSrcY, 
                          int nWidth, int nHeight, int nAngle, SIZE* pSize, 
                          const char* pszModule, const char* pszFile, 
                          int nLine);

#define CreateBitmapFromImageFile(hdc, pszImageFile,pColor, pbTran) \
    DBG_CreateBitmapFromImageFile(hdc, pszImageFile,pColor, pbTran, \
                                  __MODULE__, __FILE__, __LINE__)
#define CreateBitmapFromImageData(hdc, pImageData, dwDataSize, pColor, pbTran) \
    DBG_CreateBitmapFromImageData(hdc, pImageData, dwDataSize, pColor, pbTran, \
                                  __MODULE__, __FILE__, __LINE__)
#define ConvertBitmap(hdc, hBmp, nSrcX, nSrcY, nWidth, nHeight, nAngle, pSize) \
    DBG_ConvertBitmap(hdc, hBmp, nSrcX, nSrcY, nWidth, nHeight, nAngle, pSize, \
                      __MODULE__, __FILE__, __LINE__)

HIMAGE  DBG_CreateImageFromFile(PCSTR pImageFile, const char* pszModule, 
                                const char* pszFile, int nLine);
HIMAGE  DBG_CreateImageFromData(const void* pImageData, DWORD dwDataSize, 
                                const char* pszModule, 
                                const char* pszFile, int nLine);

BOOL    DBG_DeleteImage(HIMAGE hImage, const char* pszModule, 
                        const char* pszFile, int nLine);

#define CreateImageFromFile(pImageFile) \
    DBG_CreateImageFromFile(pImageFile, __MODULE__, __FILE__, __LINE__)
#define CreateImageFromData(pImageData, dwDataSize) \
    DBG_CreateImageFromData(pImageData, dwDataSize, __MODULE__, __FILE__, \
                            __LINE__)
#define DeleteImage(hImage) \
    DBG_DeleteImage(hImage, __MODULE__, __FILE__, __LINE__)

HGIFANIMATE DBG_StartAnimatedGIFFromFile(HWND hWnd, PCSTR pszGIFFile, 
                                         int x, int y, int dm, 
                                         const char* pszModule, 
                                         const char* pszFile, int nLine);
HGIFANIMATE DBG_StartAnimatedGIFFromData(HWND hWnd, const void* pGIFData,
                                         DWORD dwDataSize, int x, int y, 
                                         int dm, const char* pszModule, 
                                         const char* pszFile, int nLine);

BOOL        DBG_EndAnimatedGIF(HGIFANIMATE hGIFAnimate, 
                               const char* pszModule, const char* pszFile, 
                               int nLine);

#define StartAnimatedGIFFromFile(hWnd, pszGIFFile, x, y, dm) \
    DBG_StartAnimatedGIFFromFile(hWnd, pszGIFFile, x, y, dm, \
                                 __MODULE__, __FILE__, __LINE__)
#define StartAnimatedGIFFromData(hWnd, pGIFData, dwDataSize, x, y, dm) \
    DBG_StartAnimatedGIFFromData(hWnd, pGIFData, dwDataSize, x, y, dm, \
                                 __MODULE__, __FILE__, __LINE__)
#define EndAnimatedGIF(hGIFAnimate) \
    DBG_EndAnimatedGIF(hGIFAnimate, __MODULE__, __FILE__, __LINE__)

#endif // NO_DEBUG_IMAGE

#ifdef __cplusplus
}   /* End of extern "C" */
#endif

#else   // DEBUG_OBJECT

#define StartObjectDebug    ((void)0)
#define EnableObjectDebug   ((void)0)
#define EndObjectDebug      ((void)0)

#endif  // DEBUG_OBJECT

#endif  // __HPDEBUG_H
