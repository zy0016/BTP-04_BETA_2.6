
/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Imagelib
 *
 * Purpose  : convert the bmp
 *            
\**************************************************************************/

#include "window.h"
#include "malloc.h"
#include "hpimage.h"

#define RGB32_R6G6B6

static int convert(const BITMAPINFO *lpbmi, void* lpvSrcBits, 
                   void* lpvDstBits, int nSrcX, int nWidth, int nAngle);

/*********************************************************************\
* Function	    ConvertBitmap
* Purpose       在指定位图中裁减一部分，将其旋转得到新的位图。
                旋转角度限于90的倍数，逆时针旋转。
* Params	    HDC hdc         设备句柄
                HBITMAP hBmp    源位图
                int nSrcX       裁减位图在源位图中的左上角X坐标    
                int nSrcY       裁减位图在源位图中的左上角Y坐标
                int nWidth      得到的位图宽度
                int nHeight     得到的位图宽度
                int nAngle      得到的位图相对源位图的旋转角度
                SIZE* pSize     得到的位图实际大小

* Return        TRUE/FALSE.
* Remarks	   
**********************************************************************/
HBITMAP ConvertBitmapEx(HDC hdc, HBITMAP hBmp, int nSrcX, int nSrcY, 
                      int nWidth, int nHeight, int nAngle, SIZE* pSize)
{
    static BITMAPINFO bmpInfo;
    HBITMAP hBmpDst, hOldBmp;
    HDC hmemdc;
    BYTE* lpvSrcBits, *lpvDstBits, *lpvDst;
    int border;
    int nSize, i, j;
    DWORD dwTmp;
    
    //申请内存设备
    hmemdc = CreateCompatibleDC(hdc);
    if (hmemdc == NULL)
        return NULL;

    hOldBmp = (HBITMAP)SelectObject(hmemdc, hBmp);

    /* 填充BITMAPINFOHEADER，得到源位图的信息，需要初始化下两个字段 */
    bmpInfo.bmiHeader.biBitCount = 0;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    if (GetDIBits(hdc, hBmp, nSrcY, nHeight, NULL, &bmpInfo, 
        DIB_RGB_COLORS) == 0)
        goto Error1;

    /* 超出了位图范围, 只需要判断X方向，Y方向已经在GetDIBits中进行了判断 */
    if (nSrcX >= bmpInfo.bmiHeader.biWidth || 
        bmpInfo.bmiHeader.biHeight == 0)
        goto Error1;
    else if (nSrcX + nWidth >= bmpInfo.bmiHeader.biWidth)
        nWidth = bmpInfo.bmiHeader.biWidth - nSrcX;

    /* biHeight < 0 则得到的数据是从上向下的顺序存放 */
    bmpInfo.bmiHeader.biHeight = (bmpInfo.bmiHeader.biHeight) < 0 ? 
        bmpInfo.bmiHeader.biHeight : -bmpInfo.bmiHeader.biHeight;
    nHeight = -bmpInfo.bmiHeader.biHeight;

    lpvSrcBits = malloc(bmpInfo.bmiHeader.biSizeImage);
    if (lpvSrcBits == NULL)
        goto Error1;

    if (GetDIBits(hdc, hBmp, nSrcY, nHeight, lpvSrcBits, &bmpInfo, 
        DIB_RGB_COLORS) == 0)
        goto Error2;

    nSize = convert(&bmpInfo, lpvSrcBits, NULL, nSrcX, nWidth, nAngle);
    if (nSize == 0)
        goto Error2;

    lpvDstBits = malloc(nSize);
    lpvDst = lpvDstBits;
    if (lpvDstBits == NULL)
        goto Error2;
    
    convert(&bmpInfo, lpvSrcBits, lpvDstBits, nSrcX, nWidth, nAngle);

    free(lpvSrcBits);

    if (nAngle/90%2)//如果是90度或270度旋转
    {
        border = nHeight;
        nHeight = nWidth;
        nWidth = border;
    }
    bmpInfo.bmiHeader.biWidth = nWidth;
    bmpInfo.bmiHeader.biHeight = -nHeight;

    hBmpDst = CreateCompatibleBitmap(hdc, nWidth, nHeight);
    if (hBmpDst == NULL)
        goto Error3;
    
    SelectObject(hmemdc, hBmpDst);

#ifdef RGB32_R6G6B6
    for (i = 0; i < nHeight; i++)
    {
        for(j = 0; j < nWidth; j++)
        {
           dwTmp = *(DWORD*)lpvDstBits;
           *lpvDstBits = (BYTE)(((dwTmp & 0x0003F) <<  2) | ((dwTmp & 0x00030) >>  4));
           lpvDstBits ++;
           *lpvDstBits = (BYTE)(((dwTmp & 0x00FC0) >>  4) | ((dwTmp & 0x00C00) >> 10));
           lpvDstBits ++;
           *lpvDstBits = (BYTE)(((dwTmp & 0x3F000) >> 10) | ((dwTmp & 0x30000) >> 16)); 
           lpvDstBits ++;
           *lpvDstBits = 0;
           lpvDstBits ++;
        }
    }
#endif
    
    if (SetDIBitsToDevice(hmemdc, 0, 0, nWidth, nHeight, 0, 0, 0, nHeight, 
        lpvDst, &bmpInfo, DIB_RGB_COLORS) == 0)
        goto Error4;
    
    SelectObject(hmemdc, hOldBmp);
    
    free(lpvDst);
    DeleteDC(hmemdc);
    
    if (pSize != NULL)
    {
        pSize->cx = nWidth;
        pSize->cy = nHeight;
    }
    return hBmpDst;
    
Error2: 
    free(lpvSrcBits);
Error1: 
    DeleteDC(hmemdc);
    
    return NULL;

Error4: 
    DeleteObject(hBmpDst);
Error3: 
    free(lpvDst);
    DeleteDC(hmemdc);
    
    return NULL;
}

/*********************************************************************\
* Function	    ConvertBitmap
* Purpose       在指定位图中裁减一部分，将其旋转得到新的位图。
                旋转角度限于90的倍数，逆时针旋转。
* Params	    HDC hdc         设备句柄
                HBITMAP hBmp    源位图
                int nSrcX       裁减位图在源位图中的左上角X坐标    
                int nSrcY       裁减位图在源位图中的左上角Y坐标
                int nWidth      得到的位图宽度
                int nHeight     得到的位图宽度
                int nAngle      得到的位图相对源位图的旋转角度
                SIZE* pSize     得到的位图实际大小

* Return        TRUE/FALSE.
* Remarks	   
**********************************************************************/
HBITMAP IMAGEAPI ConvertBitmap(HDC hdc, HBITMAP hBmp, int nSrcX, int nSrcY, 
                      int nWidth, int nHeight, int nAngle, SIZE* pSize)
{
    static BITMAPINFO bmpInfo;
    HBITMAP hBmpDst, hOldBmp;
    HDC hmemdc;
    BYTE* lpvSrcBits, *lpvDstBits, *lpvDst;
    int border;
    int nSize;
    DWORD dwTmp;
    int i, j;
    
    //申请内存设备
    hmemdc = CreateCompatibleDC(hdc);
    if (hmemdc == NULL)
        return NULL;

    hOldBmp = (HBITMAP)SelectObject(hmemdc, hBmp);

    /* 填充BITMAPINFOHEADER，得到源位图的信息，需要初始化下两个字段 */
    bmpInfo.bmiHeader.biBitCount = 0;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    if (GetDIBits(hdc, hBmp, nSrcY, nHeight, NULL, &bmpInfo, 
        DIB_RGB_COLORS) == 0)
        goto Error1;

    /* 超出了位图范围, 只需要判断X方向，Y方向已经在GetDIBits中进行了判断 */
    if (nSrcX >= bmpInfo.bmiHeader.biWidth || 
        bmpInfo.bmiHeader.biHeight == 0)
        goto Error1;
    else if (nSrcX + nWidth >= bmpInfo.bmiHeader.biWidth)
        nWidth = bmpInfo.bmiHeader.biWidth - nSrcX;

    /* biHeight < 0 则得到的数据是从上向下的顺序存放 */
    bmpInfo.bmiHeader.biHeight = (bmpInfo.bmiHeader.biHeight) < 0 ? 
        bmpInfo.bmiHeader.biHeight : -bmpInfo.bmiHeader.biHeight;
    nHeight = -bmpInfo.bmiHeader.biHeight;

    lpvSrcBits = malloc(bmpInfo.bmiHeader.biSizeImage);
    if (lpvSrcBits == NULL)
        goto Error1;

    if (GetDIBits(hdc, hBmp, nSrcY, nHeight, lpvSrcBits, &bmpInfo, 
        DIB_RGB_COLORS) == 0)
        goto Error2;

    nSize = convert(&bmpInfo, lpvSrcBits, NULL, nSrcX, nWidth, nAngle);
    if (nSize == 0)
        goto Error2;

    lpvDstBits = malloc(nSize);
    lpvDst = lpvDstBits;
    if (lpvDstBits == NULL)
        goto Error2;
    
    convert(&bmpInfo, lpvSrcBits, lpvDstBits, nSrcX, nWidth, nAngle);

    free(lpvSrcBits);

    if (nAngle/90%2)//如果是90度或270度旋转
    {
        border = nHeight;
        nHeight = nWidth;
        nWidth = border;
    }
    bmpInfo.bmiHeader.biWidth = nWidth;
    bmpInfo.bmiHeader.biHeight = -nHeight;

    hBmpDst = CreateCompatibleBitmap(hdc, nWidth, nHeight);
    if (hBmpDst == NULL)
        goto Error3;
    
    SelectObject(hmemdc, hBmpDst);

#ifdef RGB32_R6G6B6
    for (i = 0; i < nHeight; i++)
    {
        for(j = 0; j < nWidth; j++)
        {
           dwTmp = *(DWORD*)lpvDstBits;
           *lpvDstBits = (BYTE)(((dwTmp & 0x0003F) <<  2) | ((dwTmp & 0x00030) >>  4));
           lpvDstBits ++;
           *lpvDstBits = (BYTE)(((dwTmp & 0x00FC0) >>  4) | ((dwTmp & 0x00C00) >> 10));
           lpvDstBits ++;
           *lpvDstBits = (BYTE)(((dwTmp & 0x3F000) >> 10) | ((dwTmp & 0x30000) >> 16)); 
           lpvDstBits ++;
           *lpvDstBits = 0;
           lpvDstBits ++;
        }
    }
#endif

    if (SetDIBitsToDevice(hmemdc, 0, 0, nWidth, nHeight, 0, 0, 0, nHeight, 
        lpvDst, &bmpInfo, DIB_RGB_COLORS) == 0)
        goto Error4;
    
    SelectObject(hmemdc, hOldBmp);
    
    free(lpvDst);
    DeleteDC(hmemdc);
    
    if (pSize != NULL)
    {
        pSize->cx = nWidth;
        pSize->cy = nHeight;
    }
    return hBmpDst;
    
Error2: 
    free(lpvSrcBits);
Error1: 
    DeleteDC(hmemdc);
    
    return NULL;

Error4: 
    DeleteObject(hBmpDst);
Error3: 
    free(lpvDst);
    DeleteDC(hmemdc);
    
    return NULL;
}

/*********************************************************************
*   Function:	SetAngleBitbltBmp
*   Purpose :	封装为上曾易调用的函数接口，贴图
*   Argument:	HDC hdcDest		目的设备
				int nDestX		目的坐标X
				int nDestY		目的坐标Y
				int nDestW		图片显示宽度
				int nDestH		图片显示高度
				HBITMAP hBmp	图片句柄
                int nSrcX       源坐标X
                int nSrcY       源坐标Y
				DWORD dwRop		操作方式
				int nAngle		旋转角度
*	Return  :	TRUE/FALSE.
*	Explain :	if convert failed return FALSE.             
*	NOTE    :  . 
**********************************************************************/
BOOL IMAGEAPI SetAngleBitbltBmp(HDC hdcDest, int nDestX, int nDestY, int nDestW, 
                       int nDestH, HBITMAP hBmp, int nSrcX, int nSrcY, 
                       DWORD dwRop, int nAngle)
{
    HBITMAP hBmpDst;
    SIZE size;
    BOOL bRet;

    hBmpDst = ConvertBitmap(hdcDest, hBmp, nSrcX, nSrcX, nDestW, nDestH, 
        nAngle, &size);
    if (hBmpDst == NULL)
        return FALSE;

    bRet = BitBlt(hdcDest, nDestX, nDestY, size.cx, size.cy, 
        (HDC)hBmpDst, 0, 0, dwRop);

    DeleteObject(hBmpDst);

    return bRet;
}

/*********************************************************************
*   Function:	SetAngleStretchBltBmp
*   Purpose :	封装为上曾易调用的函数接口，拉伸贴图
*   Argument:	HDC hdcDest		目的设备
				int nDestX		目的坐标X
				int nDestY		目的坐标Y
				int nDestW		图片显示宽度
				int nDestH		图片显示高度
				HBITMAP hBmp	图片句柄
				int nSrcX		源数据起始坐标X
				int nSrcY		源数据起始坐标Y
				int nSrcW		源数据宽度
				int nSrcH		源数据高度
				DWORD dwRop		操作方式
				int nAngle		旋转角度
*	Return  :	TRUE/FALSE.
*	Explain :	if convert failed return FALSE.             
*	NOTE    :  . 
**********************************************************************/
BOOL IMAGEAPI SetAngleStretchBltBmp(HDC hdcDest, int nDestX, int nDestY, int nDestW,
                           int nDestH, HBITMAP hBmp, int nSrcX, int nSrcY, 
                           int nSrcW, int nSrcH, DWORD dwRop, int nAngle)
{
    HBITMAP hBmpDst;
    SIZE size;
    BOOL bRet;
    
    hBmpDst = ConvertBitmap(hdcDest, hBmp, nSrcX, nSrcY, nSrcW, nSrcH, nAngle, &size);
    if (hBmpDst == NULL)
        return FALSE;
    
    bRet = StretchBlt(hdcDest, nDestX, nDestY, nDestW, nDestH, (HDC)hBmpDst, 
        0, 0, size.cx, size.cy, dwRop);
    
    DeleteObject(hBmpDst);

    return bRet;
}

/*********************************************************************
*   Function:   convert
*   Purpose :   The convert routine implement the rotate algorithm.
*   Argument:   BITMAPINFO *lpbmi   图片结构信息
                void* lpvSrcBits    源数据
                void* lpvDstBits    目的数据
                int nSrcX           裁减位图的左边界X坐标
                int nWidth          裁减位图的宽度
                int angle           旋转角度
*   Return  :   TRUE/FALSE.
*   Explain :   return the size of  buffer.             
*   NOTE    :  . 
**********************************************************************/
static int convert(const BITMAPINFO *lpbmi, void* lpvSrcBits, 
                   void* lpvDstBits, int nSrcX, int nWidth, int nAngle)
{
    int i, j, k;
    int width, height;
    int xOffset, yOffset;
    int nSize;
    int pixbits, pixbytes, srclinebytes, srcstartbytes, dstlinebytes;
    unsigned char *pSrc, *pDst, *p1, *p2;

    ASSERT(lpbmi != NULL && lpvSrcBits != NULL);

    if (nAngle % 90 != 0)
    {
        SetLastError(1);
        return 0;
    }

    pixbits = lpbmi->bmiHeader.biBitCount;
    if ((pixbits % 8) != 0 || pixbits == 0)
    {
        SetLastError(1);
        return 0;
    }

    if (lpbmi->bmiHeader.biHeight > 0)
    {
        SetLastError(1);
        return 0;
    }

    pixbytes = pixbits / 8;
    srclinebytes = (lpbmi->bmiHeader.biWidth * pixbytes + 3) / 4 * 4;
    srcstartbytes = nSrcX * pixbytes;
    pSrc = (unsigned char*)lpvSrcBits + srcstartbytes;
    pDst = (unsigned char*)lpvDstBits;

    width = nWidth;
    if (nSrcX + width >= lpbmi->bmiHeader.biWidth)
        width = lpbmi->bmiHeader.biWidth - nSrcX;
    height = -lpbmi->bmiHeader.biHeight;
    
    switch((nAngle % 360) / 90)
    {
    case 1:
        dstlinebytes = (height * pixbytes + 3) / 4 * 4;
        nSize = dstlinebytes * width;
        if (pDst != NULL)
        {
            xOffset = -dstlinebytes;
            yOffset = pixbytes;
            pDst += (width - 1) * (-xOffset);
        }
        break;
    case 2:
        dstlinebytes = (width * pixbytes + 3) / 4 * 4;
        nSize = dstlinebytes * height;
        if (pDst != NULL)
        {
            xOffset = -pixbytes;
            yOffset = -dstlinebytes;
            pDst += (height - 1) * (-yOffset) + (width - 1) * pixbytes;
        }
        break;
    case 3:
        dstlinebytes = (height * pixbytes + 3) / 4 * 4;
        nSize = dstlinebytes * width;
        if (pDst != NULL)
        {
            xOffset = (height * pixbytes + 3) / 4 * 4;
            yOffset = -pixbytes;
            pDst += (height - 1) * pixbytes;
        }
        break;
    case 0:
    default:
        dstlinebytes = (width * pixbytes + 3) / 4 * 4;
        nSize = dstlinebytes  * height;
        if (pDst != NULL)
        {
            xOffset = pixbytes;
            yOffset = dstlinebytes;
        }
        break;
    }

    if (pDst == NULL)
        return nSize;

    for (i = 0; i < height; i ++)
    {
        p1 = pSrc;
        p2 = pDst;
        for (j = 0; j < width; j ++)
        {
            for (k = 0; k < pixbytes; k ++)
            {
                *(p2 + k) = *(p1 + k);
            }

            p1 += pixbytes;
            p2 += xOffset;
        }
        pSrc += srclinebytes;
        pDst += yOffset;
    }

    return nSize;
}
