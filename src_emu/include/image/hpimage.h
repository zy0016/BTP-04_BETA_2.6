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

#ifndef __HPIMAGE_H
#define __HPIMAGE_H

#ifndef __WINDOW_H
#include "window.h"
#endif

//#include "hp_diag.h"

/* Assume C declarations for C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define IMAGEAPI

/**************************************************************************/
/*              Typedefs and functions for image object                   */
/**************************************************************************/

DECLARE_HANDLE(HIMAGE);

HIMAGE  IMAGEAPI CreateImageFromFile(PCSTR pImageFile);
HIMAGE  IMAGEAPI CreateImageFromData(const void* pImageData, 
									 DWORD dwDataSize);

BOOL    IMAGEAPI DeleteImage(HIMAGE hImage);

BOOL    IMAGEAPI DrawImage(HDC hdc, int nDestX, int nDestY, int nDestW, 
                           int nDestH, HIMAGE hImage, int nSrcX, int nSrcY,
						   DWORD dwRop);

BOOL    IMAGEAPI GetImageDimension(HIMAGE hImage, PSIZE pSize);


/**************************************************************************/
/*                    change image type                                   */
/**************************************************************************/

HBITMAP IMAGEAPI CreateBitmapFromImageFile(HDC hdc, PCSTR pszImageFile,
                                           PCOLORREF pColor, BOOL *pbTran);
HBITMAP IMAGEAPI CreateBitmapFromImageData(HDC hdc, const void* pImageData, 
                                           DWORD dwDataSize,
                                           PCOLORREF pColor, BOOL *pbTran);
BOOL    IMAGEAPI BmpFileToJpegFile(char* BmpFile, char* JpegFile);
BOOL    IMAGEAPI BmpDataToJpegFile(const char* BmpData, DWORD dwDataSize, 
	    						   char* JpegFile);
BOOL    IMAGEAPI BmpFileToGifFile(char* BmpFile, char* GifFile);
BOOL    IMAGEAPI BmpDataToGifFile(const char* BmpData, DWORD dwDataSize,
									  char* GifFile);
BOOL    IMAGEAPI BmpFileToPngFile(char* BmpFile, char* PngFile);
BOOL    IMAGEAPI BmpDataToPngFile(const char* BmpData, DWORD dwDataSize,
									  char* PngFile);
/**************************************************************************/
/*              Draw image file or data directly                          */
/**************************************************************************/

BOOL    IMAGEAPI DrawImageFromFile(HDC hdc, PCSTR pszImgFile, int x, int y,
								   DWORD dwRop);
BOOL    IMAGEAPI DrawImageFromData(HDC hdc, const void* pImageData, 
                                   int x, int y, DWORD dwDataSize,
                                   DWORD dwRop);
BOOL    IMAGEAPI DrawImageFromFileEx(HDC hdc, PCSTR pszImgFile, int x, int y,
                                     int width, int height, DWORD dwRop);
BOOL    IMAGEAPI DrawImageFromDataEx(HDC hdc, const void* pImageData, 
                                     int x, int y, int width, int height,
                                     DWORD dwDataSize, DWORD dwRop);

BOOL    IMAGEAPI StretchJpegFromFile(HDC hdc, PCSTR pszImgFile, int x, 
                                     int y, int width, int height, DWORD dwRop);
BOOL    IMAGEAPI StretchGif(HDC hdc, PCSTR pszImgFile, int x, int y,
                            int width, int height, DWORD dwRop);

BOOL    IMAGEAPI StretchWBMP(HDC hdc, PCSTR pszImgFile, int x, int y,
                            int width, int height, DWORD dwRop);

BOOL    IMAGEAPI GetImageDimensionFromFile(PCSTR pszImageFile, PSIZE pSize);
BOOL    IMAGEAPI GetImageDimensionFromData(const void* pImageData,
                                           DWORD dwDataSize, PSIZE pSize);

/**************************************************************************/
/*              Definitions and functions for Animated GIF                */
/**************************************************************************/

DECLARE_HANDLE(HGIFANIMATE);
#define  DM_NONE    0           
#define  DM_DATA    1
#define  DM_BITMAP  2

BOOL IMAGEAPI IsGIFDynamic(void * pszGIFFile, BOOL bFile, int nFileSize);

HGIFANIMATE IMAGEAPI StartAnimatedGIFFromFile(HWND hWnd, PCSTR pszGIFFile, 
                                              int x, int y, int dm);
HGIFANIMATE IMAGEAPI StartAnimatedGIFFromData(HWND hWnd, const void* pGIFData,
                                              DWORD dwDataSize,
                                              int x, int y, int dm);

HGIFANIMATE IMAGEAPI StartAnimatedGIFFromFileEx(HWND hWnd, PCSTR pszGIFFile, 
                                              int x, int y, int width, int height,int dm);
HGIFANIMATE IMAGEAPI StartAnimatedGIFFromDataEx(HWND hWnd, const void* pGIFData,
                                              DWORD dwDataSize, int x, int y, 
                                              int width, int height, int dm);

typedef void (*PAINTBKFUNC)(HGIFANIMATE hGIFAnimate, BOOL bEnd, int x, int y, HDC hdcMem);
BOOL		IMAGEAPI SetPaintBkFunc(HGIFANIMATE hGIFAnimate, 
									PAINTBKFUNC pfnPaintBkFunc);


HGIFANIMATE IMAGEAPI DrawGIFFromFileTime(HWND hWnd, 
                                         PCSTR pszGIFFile,
                                         int x, int y, int dm, int Time);

HGIFANIMATE IMAGEAPI DrawGIFFromDataTime(HWND hWnd, 
                                         void *pGifData,
                                         DWORD dwDataSize,
                                         int x, int y, int dm, int Times);
HGIFANIMATE IMAGEAPI DrawGIFFromFileTimeEx(HWND hWnd, 
                                         PCSTR pszGIFFile, int x, int y, 
                                         int width, int height, int dm, int Time);

HGIFANIMATE IMAGEAPI DrawGIFFromDataTimeEx(HWND hWnd, void *pGifData,DWORD dwDataSize,
                                         int x, int y, int width, int height, 
                                         int dm, int Times);


BOOL        IMAGEAPI EndAnimatedGIF(HGIFANIMATE hGIFAnimate);

BOOL        IMAGEAPI PaintAnimatedGIF(HDC hdc, HGIFANIMATE hGif);
BOOL        IMAGEAPI ScrollAnimatedGIF(HGIFANIMATE hGIFAnimate, int dx, int dy);
BOOL        IMAGEAPI ScrollAnimatedGIFEx(HGIFANIMATE hGif, int x, int y);

/**************************************************************************/
/*              Definitions and functions for Convert Bitmap              */
/**************************************************************************/

HBITMAP IMAGEAPI ConvertBitmap(HDC hdc, HBITMAP hBmp, int nSrcX, int nSrcY, 
                               int nWidth, int nHeight, int nAngle, SIZE* pSize);
BOOL    IMAGEAPI SetAngleBitbltBmp(HDC hdcDest, int nDestX, int nDestY, int nDestW,
								   int nDestH, HBITMAP hBmp, int nSrcX, int nSrcY, 
                                   DWORD dwRop, int nAngle);
BOOL    IMAGEAPI SetAngleStretchBltBmp(HDC hdcDest, int nDestX, int nDestY, int nDestW,
                           int nDestH, HBITMAP hBmp, int nSrcX, int nSrcY, 
                           int nSrcW, int nSrcH, DWORD dwRop, int nAngle);
#ifdef __cplusplus
}   /* End of extern "C" */
#endif

#endif 

