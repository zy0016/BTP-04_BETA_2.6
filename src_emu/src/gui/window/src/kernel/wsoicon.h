/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for icon object
 *            
\**************************************************************************/

#ifndef __WSOICON_H
#define __WSOICON_H

#include "hpwin.h"

#define MAX_IDCOUNT 8
#define ICON_FILE_HEADER    6
#define ICON_COUNT_OFFSET   4

typedef struct
{
    BYTE    bWidth;
    BYTE    bHeight;
    BYTE    bColorCount;
    BYTE    bReserved;
    WORD    wPlanes;
    WORD    wBitCount;
    DWORD   dwBytesInRes;
    DWORD   dwImagOffset;
}ICONDIRENTRY, *PICONDIRENTRY;

typedef struct
{
    BITMAPINFOHEADER    icHeader;
    RGBQUAD             icColors;   //[wBitCount]
    BYTE                icXOR;      //[bWidth, bHeight, wPlanes, wBitCount]
    BYTE                icAND;      //[bWidth, bHeight, wPlanes, wBitCount]
}ICONIMAGE, *PICONIMAGE;

typedef struct
{
    BYTE            bWidth;
    BYTE            bHeight;
    HBITMAP         hBitmapXOR;
    HBITMAP         hBitmapAND;
} ICONOBJ, *PICONOBJ;


PICONOBJ  ICON_Create(int nRop, int nWidth, int nHeight, 
                        int ColorCount, int wPlanes, const void* pANDPlane, 
                          const void* pXORPlane);
void        ICON_Destroy(PICONOBJ pIcon);

#endif //__WSOICON_H
