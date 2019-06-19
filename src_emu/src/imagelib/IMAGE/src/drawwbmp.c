/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ImageLib
 *
 * Purpose  : Implements ImageLib WBMP functions.
 *            
\**************************************************************************/

#include "window.h"
#include "filesys.h"
#include "malloc.h"
#include "string.h"
//#include "stdio.h"

#include "hpimage.h"
#include "DrawImg.h"

#define CONT_FLAG 0x80

typedef struct tagMONOBITMAPINFO 
{  
    BITMAPINFOHEADER bmiHeader; 
    RGBQUAD          bmiColors[2]; 
} MONOBITMAPINFO; 


HBITMAP  CreateBitmapFromWbmp(struct data_buffer pData)
{
        BYTE *pBuf, *pNewBuf;
    int  nWidth, nHeight, nBytes, nDWords, i;
    MONOBITMAPINFO  bmi;
    
    pBuf = (BYTE *)(pData.data);
    ASSERT(*pBuf == 0x00);
    pBuf += 2;
    nWidth = (int)DecodeMultiByteInt(&pBuf);
    nHeight = (int)DecodeMultiByteInt(&pBuf);
    nBytes = (nWidth + 7) / 8;
    nDWords = ((nWidth + 31) / 32) * 4;
    if (nDWords > nBytes) 
    {
        pNewBuf = (BYTE *)malloc(nDWords * nHeight);
        memset(pNewBuf, 0, nDWords * nHeight);
        for (i = 0; i < nHeight; i++)
        {
            memcpy(pNewBuf, pBuf, nBytes);
            pNewBuf += nDWords;
            pBuf += nBytes;
        }
        pNewBuf -= nDWords * nHeight;
        
    }

    bmi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth			= nWidth;
	bmi.bmiHeader.biHeight			= -nHeight;
    bmi.bmiHeader.biPlanes			= 1;
    bmi.bmiHeader.biBitCount		= 1;
    bmi.bmiHeader.biCompression		= BI_RGB;
    bmi.bmiHeader.biSizeImage		= 0;
    bmi.bmiHeader.biXPelsPerMeter	= 0; 
    bmi.bmiHeader.biYPelsPerMeter	= 0; 
    bmi.bmiHeader.biClrUsed			= 0; 
    bmi.bmiHeader.biClrImportant	= 0; 

	bmi.bmiColors[0].rgbBlue     = 0;
	bmi.bmiColors[0].rgbGreen    = 0;
	bmi.bmiColors[0].rgbRed      = 0;
	bmi.bmiColors[0].rgbReserved = 0;
	bmi.bmiColors[1].rgbBlue     = 255;
	bmi.bmiColors[1].rgbGreen    = 255;
	bmi.bmiColors[1].rgbRed      = 255;
	bmi.bmiColors[1].rgbReserved = 0;
    if (nDWords > nBytes) 
        return CreateDIBitmap(NULL, &bmi.bmiHeader, CBM_INIT, 
            pNewBuf, (PBITMAPINFO)&bmi, DIB_RGB_COLORS);
    else
        return CreateDIBitmap(NULL, &bmi.bmiHeader, CBM_INIT, 
            pBuf, (PBITMAPINFO)&bmi, DIB_RGB_COLORS);
}



void DrawWBMP(HDC hdc, int x, int y, int width, int height,
                   struct data_buffer pData, DWORD dwRop)
{
    BYTE *pBuf, *pNewBuf;
    int  nWidth, nHeight, nBytes, nDWords, i;
    MONOBITMAPINFO  bmi;
    
    pBuf = (BYTE *)(pData.data);
    ASSERT(*pBuf == 0x00);
    pBuf += 2;
    nWidth = (int)DecodeMultiByteInt(&pBuf);
    nHeight = (int)DecodeMultiByteInt(&pBuf);
    nBytes = (nWidth + 7) / 8;
    nDWords = ((nWidth + 31) / 32) * 4;
    if (nDWords > nBytes) 
    {
        pNewBuf = (BYTE *)malloc(nDWords * nHeight);
        memset(pNewBuf, 0, nDWords * nHeight);
        for (i = 0; i < nHeight; i++)
        {
            memcpy(pNewBuf, pBuf, nBytes);
            pNewBuf += nDWords;
            pBuf += nBytes;
        }
        pNewBuf -= nDWords * nHeight;
        
    }

    bmi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth			= nWidth;
	bmi.bmiHeader.biHeight			= -nHeight;
    bmi.bmiHeader.biPlanes			= 1;
    bmi.bmiHeader.biBitCount		= 1;
    bmi.bmiHeader.biCompression		= BI_RGB;
    bmi.bmiHeader.biSizeImage		= 0;
    bmi.bmiHeader.biXPelsPerMeter	= 0; 
    bmi.bmiHeader.biYPelsPerMeter	= 0; 
    bmi.bmiHeader.biClrUsed			= 0; 
    bmi.bmiHeader.biClrImportant	= 0; 

	bmi.bmiColors[0].rgbBlue     = 0;
	bmi.bmiColors[0].rgbGreen    = 0;
	bmi.bmiColors[0].rgbRed      = 0;
	bmi.bmiColors[0].rgbReserved = 0;
	bmi.bmiColors[1].rgbBlue     = 255;
	bmi.bmiColors[1].rgbGreen    = 255;
	bmi.bmiColors[1].rgbRed      = 255;
	bmi.bmiColors[1].rgbReserved = 0;
    if (nDWords > nBytes) 
    {
        if (width == 0 && height == 0)
		{
			SetDIBitsToDevice(hdc, x, y, nWidth, nHeight, 0, 0, 0, nHeight,
            pNewBuf, (BITMAPINFO *)&bmi, DIB_RGB_COLORS);
		}
		else
		{            
			StretchDIBits(hdc, x, y, width, height, 0, 0, nWidth, nHeight, 
            pNewBuf, (BITMAPINFO *)&bmi, DIB_RGB_COLORS, SRCCOPY);
		}            
        free((void *)pNewBuf);
    }
    else
    {
        if (width == 0 && height == 0)
		{
            SetDIBitsToDevice(hdc, x, y, nWidth, nHeight, 0, 0, 0, nHeight,
            pBuf, (BITMAPINFO *)&bmi, DIB_RGB_COLORS);
		}
        else
		{
            StretchDIBits(hdc, x, y, width, height, 0, 0, nWidth, nHeight, 
            pBuf, (BITMAPINFO *)&bmi, DIB_RGB_COLORS, SRCCOPY);
		}
    }
}

unsigned int DecodeMultiByteInt(char **src)
{
	char *s = *src;
	char i, ByteNum; /*BYTE*/
	unsigned char ch;
	unsigned int  val;
	
	if ((*s & CONT_FLAG) == 0)
	{
		(*src)++;
		return((unsigned int)*s);
	}

	ByteNum = 0;
	while (*s & CONT_FLAG) 
		s++, ByteNum++;

	*src = s + 1; /* src point to the byte behind the integer */

	val = 0;
	for (i = 0; i < ByteNum; i++)
	{
		ch = ((((unsigned char)*s & 0x7F) >> i) | ((unsigned char)*(s-1) << (7-i))) & 0xFFF;
		val = val + (ch << (i * 8));
		s--;
	}
	ch = (*s & 0x7F) >> i;
	val = val + (int)(ch << (i * 8));

	return val;
}

