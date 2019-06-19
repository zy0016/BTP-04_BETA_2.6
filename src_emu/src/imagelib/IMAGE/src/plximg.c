/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ImageLib
 *
 * Purpose  : dispose five format images: bitmap, WBMP, gif, jpeg, png
 *            
\**************************************************************************/

#include "window.h"
#include "filesys.h"
#include "malloc.h"
#include "string.h"
//#include "stdio.h"

#include "gif/dgif.h"

#include "hpimage.h"
#include "DrawImg.h"


#ifndef NOJPEG
#include "jpeg/djpeg.h"
#endif //NOJPEG

#define BMP_IMAGE    1
#define GIF_IMAGE    2
#define JPEG_IMAGE   3
#define PNG_IMAGE    4
#define WBMP_IMAGE   5
#define PNG_ALPHATRANSPATENT 6

#define MASK_INDEX          0x0000FFFF  /* index mask in handle          */
#define MASK_BMAPFILE       0x000F0000  /* flag to show whether mapfile  */
#define MASK_TYPE           0x00F00000  /* type mask in handle           */

#define BMAPFILE_SHIFT_BITS   16        /* shift bits of serial in handle */
#define TYPE_SHIFT_BITS       20        /* shift bits of type in handle   */

/* Get index, serial & type from handle */
#define INDEXOFHANDLE(h)  ((DWORD)(h) & MASK_INDEX)
#define BMAPOFHANDLE(h) (((DWORD)(h) & MASK_BMAPFILE) >> BMAPFILE_SHIFT_BITS)
#define TYPEOFHANDLE(h)   (((DWORD)(h) & MASK_TYPE) >> TYPE_SHIFT_BITS)

/* Make handle using specified index, serial & type */
#define MAKEHANDLE(index, bMapfile, type)     \
    (HANDLE)(index | (bMapfile << BMAPFILE_SHIFT_BITS) | \
    (type << TYPE_SHIFT_BITS))

#define MAX_IMGOBJ  256

typedef struct  
{
    int bUsed;
	PIMGOBJ pImgObj;
}IMAGEOBJ, *PIMAGEOBJ;

static IMAGEOBJ ImgObjTable[MAX_IMGOBJ];


typedef struct tagBITMAPINFO256
{
    BITMAPINFOHEADER bmiHeader; 
    RGBQUAD          bmiColors[256]; 
} BITMAPINFO256, *PBITMAPINFO256;

extern GIFWNDDATA  GifWndData[MAX_GIFNUM];
extern  void  SetBitmapInfo( PGIFWNDDATA pGifWndData);
extern  int   SeekToStart(PGIFWNDDATA pGifWndData);
extern  int   PaintGifImage(HDC hdc, PGIFWNDDATA pGifWndData, BOOL bPaint, BOOL bTimer);


/*********************************************************************/
/*              Create iamge from file or data                       */
/*********************************************************************/

static HIMAGE IMG_Add(PPHYIMG pPhyImg, int bMapFile, int type)
{
	int index;
    PIMGOBJ pImgObj;

    if (pPhyImg == NULL)
        return NULL;

	for (index=0; index<MAX_IMGOBJ; index++)
	{
		if (ImgObjTable[index].bUsed == 0)
		{
            pImgObj = (PIMGOBJ)malloc(sizeof(IMGOBJ));
			pImgObj->handle = MAKEHANDLE(index, bMapFile, type); 
            pImgObj->pPhyImg = pPhyImg;

			ImgObjTable[index].pImgObj = pImgObj;
			ImgObjTable[index].bUsed = 1;
			return pImgObj->handle;
		}
	}		
	return NULL;        
}

/*********************************************************************\
* Function       : CreateImageFromFile	
* Purpose        :
*     Create image from file
* Params         :
*     pImageFile : the file to create iamge
* Return         :
*      Return the image object
**********************************************************************/
HIMAGE IMAGEAPI CreateImageFromFile(PCSTR pImageFile)
{
    int               hFile;
	int               bMapFile;
    unsigned char     strType[8];        
    PPHYIMG           pPhyImg;
    int               nImgType=-1;
    struct data_buffer pData;

    if (!pImageFile)
    {
        SetLastError(1);
        return NULL;
    }

    hFile = IMGOS_CreateFile(pImageFile, PLXFS_ACCESS_READ, 0);
    if (hFile == -1)
    {
        SetLastError(1);
        return NULL;
    }    

    IMGOS_ReadFile(hFile, strType, 6);
    if ((strType[0] == 'B') && (strType[1] == 'M'))        /* BMP */
		nImgType = BMP_IMAGE;
	else if ((strType[0] == 'G') && (strType[1] == 'I') &&  
             (strType[2] == 'F'))                          /* GIF */
	    nImgType = GIF_IMAGE;
	else if ((strType[0] == 0xff) && (strType[1] == 0xd8)) /* JPEG */
		nImgType = JPEG_IMAGE;
	else if ((strType[0] == 0x89) && (strType[1] == 0x50) &&
		(strType[2] == 0x4e) && (strType[3] == 0x47))      /* PNG */
		nImgType = PNG_IMAGE;

    switch(nImgType)
	{
	case BMP_IMAGE:
		IMGOS_CloseFile(hFile);

        pPhyImg = (PPHYIMG)CreateBitmapFromImageFile(NULL, pImageFile, NULL, NULL);
        return IMG_Add(pPhyImg, 0, BMP_IMAGE);

	case GIF_IMAGE:
	case JPEG_IMAGE:
	case PNG_IMAGE:	
       
        pData.data_count = IMGOS_GetFileSize(hFile);
        pData.data = (void *) IMGOS_MapFile(hFile, 0, pData.data_count);
       
        bMapFile = 1;
        
         // 无法映射文件时则需要将文件内容读到内存中
        if (!pData.data)          
        {
            pData.data = malloc(pData.data_count);
            if (!pData.data)
            {
                IMGOS_CloseFile(hFile);
        
                SetLastError(1);
                return NULL;
            }
            IMGOS_SetFilePointer(hFile, 0, PLXFS_SEEK_SET);
            IMGOS_ReadFile(hFile, pData.data, pData.data_count);
            bMapFile = 0;
        }
        if (nImgType == GIF_IMAGE)
			pPhyImg = CreateGifFromData(pData, hFile);
		else if (nImgType == JPEG_IMAGE)
			pPhyImg = CreateJpegFromData(pData, hFile);
		else if (nImgType == PNG_IMAGE)
			pPhyImg = CreatePngFromData(pData, hFile);

		return IMG_Add(pPhyImg, bMapFile, nImgType);

	default:
		IMGOS_CloseFile(hFile);
		break;
	}     

    return NULL;
}

/*********************************************************************\
* Function	   : CreateImageFromData  
* Purpose      :
*     Create image from data 
* Params	   :
*   pImageData : the pointer to image data
*   dwDataSize : the size of data
* Return	   :
*      Return the image object
**********************************************************************/
HIMAGE  IMAGEAPI CreateImageFromData(const void* pImageData, 
                                        DWORD dwDataSize)
{
   BYTE        *pData;   
   PPHYIMG     pPhyImg;
   
   if (!pImageData)
       return NULL;
   if (dwDataSize == 0)
       return NULL;

   pData = (BYTE*)pImageData;

   if (pData[0] == 'B' && pData[1] == 'M')         /* BMP */
   {        
        pPhyImg =(PPHYIMG)CreateBitmapFromImageData(NULL,pImageData, dwDataSize, NULL, NULL); 
        return	IMG_Add(pPhyImg,  0, BMP_IMAGE);
   }
    else if (pData[0] == 'G' && pData[1] == 'I' && 
             pData[2] == 'F')                      /* GIF */
    {
        struct data_buffer pData;

        pData.data = (void *)pImageData;
        pData.data_count = dwDataSize;

        pPhyImg = CreateGifFromData(pData, 0); 
        return IMG_Add(pPhyImg, 0, GIF_IMAGE);
    }
    else if (pData[0] == 0xff && pData[1] == 0xd8) /* JPEG */
    {
#ifndef NOJPEG
        struct data_buffer pData;

        pData.data = (void *)pImageData;
        pData.data_count = dwDataSize;

        pPhyImg = CreateJpegFromData(pData, 0) ;
        return IMG_Add(pPhyImg, 0, JPEG_IMAGE);
#endif //NOJPEG
    }
	else if ((pData[0] == 0x89) && (pData[1] == 0x50) &&
		(pData[2] == 0x4e) && (pData[3] == 0x47))      /* PNG */
	{
        struct data_buffer pData;

        pData.data = (void *)pImageData;
        pData.data_count = dwDataSize;

        pPhyImg = CreatePngFromData(pData, 0) ;
        return IMG_Add(pPhyImg, 0, PNG_IMAGE);
    }	

     return NULL;
}


int		IMAGEAPI GetImageTypeFromHandle(HIMAGE hImage)
{
	int      nType, nIndex;	
	PPNGDIB	 pPngDib;

	if (!hImage)
        return -1;

    nIndex = INDEXOFHANDLE(hImage);
	if (ImgObjTable[nIndex].bUsed == 0 || 
		ImgObjTable[nIndex].pImgObj->handle != hImage)
		return 0;
    nType = TYPEOFHANDLE(hImage);
	if (nType == PNG_IMAGE)
	{
		pPngDib = (PPNGDIB)ImgObjTable[nIndex].pImgObj;
		if (pPngDib->nChannel == 4)
			nType = PNG_ALPHATRANSPATENT;
	}
	return nType;
}

/*********************************************************************\
* Function	         : DeleteImage   
* Purpose            :
*     Delete the iamge object
* Params	         :
*    pImageObj       : the image object to be deleted
* Return	         :
*    if delete iamge object success, return TRUE;
*    otherwise return FALSE;
**********************************************************************/
BOOL    IMAGEAPI DeleteImage(HIMAGE hImage)
{   
    int      nType, nIndex;	
    PPHYIMG  pPhyImg;
    PIMGOBJ  pImgObj;

    if (!hImage)
        return FALSE;

    nIndex = INDEXOFHANDLE(hImage);
    nType = TYPEOFHANDLE(hImage);
	if (ImgObjTable[nIndex].bUsed == 0)
		return FALSE;
    
	pImgObj = ImgObjTable[nIndex].pImgObj;
    pPhyImg = pImgObj->pPhyImg;

    switch(nType)
    {
    case BMP_IMAGE:
        DeleteObject(pPhyImg);        
        break;

    case GIF_IMAGE:       
		DeleteGif(pPhyImg, BMAPOFHANDLE(hImage));
		break;

    case JPEG_IMAGE:
        DeleteJpeg(pPhyImg, BMAPOFHANDLE(hImage));
		break;

    case PNG_IMAGE:		
		DeletePng(pPhyImg, BMAPOFHANDLE(hImage));
        break;
	default:
        return FALSE;	
    }

	ImgObjTable[nIndex].bUsed = 0;    
	free(pImgObj);    
    return TRUE;
}

/*********************************************************************\
* Function	   : DrawImage
* Purpose      :
*     Draw iamge object on device context
* Params	   :
*     hdc      : the handle to destination device context
*     nDestX   : x-coordinate of destination rectangle's upper-left corner
*     nDestY   : y-coordinate of destination rectangle's upper-left corner
*     nDestW   : width of destination rectangle
*     nDestH   : height of destination rectangle
*     pImageObj: the iamge object to be drawn
*     nSrcX    : x-coordinate of source rectangle's upper-left corner
*     nSrcY    : y-coordinate of source rectangle's upper-left corner
*     dwRop    : raster operation code
* Return	   :
*     if draw iamge success, return TRUE;
*     otherwise return FALSE;
**********************************************************************/
BOOL IMAGEAPI DrawImage(HDC hdc, int nDestX, int nDestY, int nDestW, 
                           int nDestH, HIMAGE hImage, int nSrcX, 
                           int nSrcY, DWORD dwRop)
{
    int     uType, nIndex;
    int     ret;
	PIMGOBJ pImgObj;
    PPHYIMG pPhyImg;

    if (!hImage)
        return FALSE;
    if (!hdc)
        return FALSE;

    uType = TYPEOFHANDLE(hImage);
	nIndex = INDEXOFHANDLE(hImage);
	pImgObj = ImgObjTable[nIndex].pImgObj;
    pPhyImg = pImgObj->pPhyImg;

    if (uType == BMP_IMAGE)
    {
        SIZE size;
        GetImageDimension(hImage, &size);
        ret = StretchBlt(hdc, nDestX, nDestY, nDestW, nDestH, 
               (HDC)pPhyImg, nSrcX, nSrcY, 
               size.cx - nSrcX, size.cy - nSrcY, dwRop);
    }
    if (uType == GIF_IMAGE)
    {
        ret = PaintStaticGif(hdc, nDestX, nDestY, nDestW, nDestH, 
                       pPhyImg, nSrcX, nSrcY, dwRop);
    }
    if (uType == JPEG_IMAGE)
    {
        ret = PaintJpeg(hdc, nDestX, nDestY, nDestW, nDestH,
                   pPhyImg, nSrcX, nSrcY, dwRop);
    }
	if (uType == PNG_IMAGE)
    {
        ret = PaintPng(hdc, nDestX, nDestY, nDestW, nDestH,
                   pPhyImg, nSrcX, nSrcY, dwRop);
    }

    return (BOOL) ret;
}

HIMAGE IMAGEAPI ConvertImage(HIMAGE hImage, int nAngle)
{
    HDC hdc;
    HBITMAP hBitmap, hBitmap1;
    SIZE size;
    HIMAGE hImgNew;
    
    GetImageDimension(hImage, &size);
    hdc = CreateCompatibleDC(NULL);
    hBitmap = CreateCompatibleBitmap(hdc, size.cx, size.cy);
    SelectObject(hdc, (HGDIOBJ)hBitmap);
    DrawImage(hdc, 0, 0, size.cx, size.cy, hImage, 0, 0, SRCCOPY);
    hBitmap1 = ConvertBitmapEx(hdc, hBitmap, 0, 0, size.cx, size.cy, nAngle, &size);
    hImgNew = IMG_Add((PPHYIMG)hBitmap1,  0, BMP_IMAGE);
    DeleteObject((HGDIOBJ)hBitmap);
    DeleteDC(hdc);

    return (HIMAGE)hImgNew;
}

/*********************************************************************\
* Function	   : GetImageDimension
* Purpose      :
*     Get Image Dimension
* Params	   :
*     pImageObj: the image object that to be get dimension
*     pSize    : address of struct receiving dimensions
* Return	   :
*     if function success, return TRUE;
*     otherwise return FALSE;
**********************************************************************/
BOOL IMAGEAPI GetImageDimension(HIMAGE hImage, PSIZE pSize)
{
    int     uType, nIndex;
    PPHYIMG pPhyImg;

    if (!hImage)
        return FALSE;
    if (!pSize)
        return FALSE;

    uType = TYPEOFHANDLE(hImage);
	nIndex = INDEXOFHANDLE(hImage);
	pPhyImg = ImgObjTable[nIndex].pImgObj->pPhyImg;

    if (uType == BMP_IMAGE)
    {
        BITMAPINFO bmifo;
        bmifo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmifo.bmiHeader.biBitCount = 0;
        GetDIBits(NULL,(HBITMAP)pPhyImg, 0, 65535, NULL, &bmifo,
                  DIB_RGB_COLORS); 
        pSize->cx = bmifo.bmiHeader .biWidth ;
        pSize->cy = bmifo.bmiHeader.biHeight ;
        return TRUE;
    }
    else if (uType == GIF_IMAGE)
        return GetGifDimension(pPhyImg, pSize);
    else if (uType == JPEG_IMAGE)
        return  GetJpegDimension(pPhyImg, pSize);
    else if (uType == PNG_IMAGE)
        return  GetPngDimension(pPhyImg, pSize);
    else
        return FALSE;
}

/**************************************************************************/
/*              Create bitmap from file or data                           */
/**************************************************************************/
/*********************************************************************\
* Function	       : CreateBitmapFromImageFile
* Purpose          :
*     Create bitmap from image file
* Params	       :
*     pszImageFile : the file to create bitmap 
* Return	 	   :
*     if success, return the handle of bitmap
*     otherwise  return NULL
**********************************************************************/
HBITMAP IMAGEAPI CreateBitmapFromImageFile(HDC hdc, PCSTR pszImageFile, 
                                           PCOLORREF pColor, BOOL * pbTran)
{   
    HBITMAP          hBitmap;
    int              hFile;
    unsigned char    uType[4];
	int              nImgType = -1;	    
    BOOL             fMapFile = TRUE;
	struct data_buffer pData;

    if (!pszImageFile)
    {
        SetLastError(1);
        return NULL;
    }

    hFile = IMGOS_CreateFile(pszImageFile, PLXFS_ACCESS_READ, 0);
    if (hFile == -1)
    {
        SetLastError(1);
        return NULL;
    }

    IMGOS_ReadFile(hFile, uType, 4);
    if (uType[0] == 'B' && uType[1] == 'M')
		nImgType = BMP_IMAGE;
	else if (uType[0] == 'G' && uType[1] == 'I' && uType[2] == 'F')
		nImgType = GIF_IMAGE;
    else if (uType[0] == 0xff && uType[1] == 0xd8 )
		nImgType = JPEG_IMAGE;
    else if ((uType[0] == 0x89) && (uType[1] == 0x50) && (uType[2] == 0x4e) &&
        (uType[3] == 0x47))
		nImgType = PNG_IMAGE;
    else if ((uType[0] == 0x00) && (uType[1]) == 0x00)
        nImgType = WBMP_IMAGE;
	switch (nImgType)
    {
	case BMP_IMAGE:
	case GIF_IMAGE:
	case JPEG_IMAGE:
	case PNG_IMAGE:
    case WBMP_IMAGE:

        pData.data_count = IMGOS_GetFileSize(hFile);
        pData.data = IMGOS_MapFile(hFile, 0, pData.data_count);
        
        if (!pData.data)          
        {
            pData.data = malloc(pData.data_count);
            if (!pData.data)
            {
                IMGOS_CloseFile(hFile);
        
                SetLastError(1);
                return NULL;
            }
            IMGOS_SetFilePointer(hFile, 0, PLXFS_SEEK_SET);
            IMGOS_ReadFile(hFile, pData.data, pData.data_count);
            fMapFile = FALSE;
        }
        if (pbTran)
			*pbTran = FALSE;

        if (nImgType == BMP_IMAGE)
	         hBitmap = CreateBitmapFromImageData(NULL,pData.data, pData.data_count, NULL, NULL);
		else if (nImgType == GIF_IMAGE)
			hBitmap = CreateBitmapFromGif(hdc, pData, pColor, pbTran);
		else if (nImgType == JPEG_IMAGE)
			hBitmap = CreateBitmapFromJpeg(pData);
		else if (nImgType == PNG_IMAGE)
			hBitmap = CreateBitmapFromPNG(pData.data);
        else if (nImgType == WBMP_IMAGE)
            hBitmap = CreateBitmapFromWbmp(pData);

		if (fMapFile)
            IMGOS_UnmapFile(hFile, pData.data, pData.data_count);
        else 
            free(pData.data);
        IMGOS_CloseFile(hFile);
        return (HANDLE)hBitmap;
	default:
		IMGOS_CloseFile(hFile);
		break;
    }    
	return NULL;
}

/*********************************************************************\
* Function	    : CreateBitmapFromImageData
* Purpose       :
*     Create bitmap from image data
* Params	    :
*     pImageData: data address to create bitmap
*     dwDataSize: the size of data used for creating image
* Return	 	:
*     return the handle of bitmap
**********************************************************************/
HBITMAP IMAGEAPI CreateBitmapFromImageData(HDC hdc, const void* pImageData, 
                                           DWORD dwDataSize,
                                           PCOLORREF pColor, 
                                           BOOL * pbTran)
{
    static BITMAPINFO256 BitmapInfo;
    int         i;
    BITMAPINFO256* pbmi;
    DWORD       bfOffBits;
    HBITMAP     hBitmap;
    int         nClrBits;
    BYTE*       pData;

    if (!pImageData)
        return NULL;
    if (dwDataSize == 0)
        return NULL;
    if(pbTran)
        *pbTran = FALSE;

    pData = (BYTE*)pImageData;
 
    if (pData[0] == 'B' && pData[1] == 'M')
    {
        pbmi = (PBITMAPINFO256)&BitmapInfo;
        bfOffBits = GET32(pData + 10);
        pData += 14;

        pbmi->bmiHeader.biSize = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSize)); 
        pbmi->bmiHeader.biWidth = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biWidth)); 
        pbmi->bmiHeader.biHeight = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biHeight)); 
        pbmi->bmiHeader.biPlanes = 
            GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biPlanes)); 
        pbmi->bmiHeader.biBitCount = 
            GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biBitCount)); 
        pbmi->bmiHeader.biCompression = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biCompression)); 
        pbmi->bmiHeader.biSizeImage = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSizeImage));
        pbmi->bmiHeader.biXPelsPerMeter = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biXPelsPerMeter));
        pbmi->bmiHeader.biYPelsPerMeter = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biYPelsPerMeter));
        pbmi->bmiHeader.biClrUsed = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrUsed));
        pbmi->bmiHeader.biClrImportant = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrImportant));

        if (pbmi->bmiHeader.biCompression == BI_RLE8)
            return NULL;
        // Skips bitmap info header
        pData += sizeof(BITMAPINFOHEADER);

        nClrBits = pbmi->bmiHeader.biPlanes * pbmi->bmiHeader.biBitCount; 
        if (nClrBits != 1 && nClrBits != 2 && nClrBits != 4 && nClrBits != 8 &&
            nClrBits != 16 && nClrBits != 24 && nClrBits != 32)
            return NULL;

        // Gets the palettes if necessary
        if (nClrBits <= 8)
        {
            for (i = 0; i < (1 << nClrBits); i++)
            {
                pbmi->bmiColors[i].rgbBlue = *pData++;
                pbmi->bmiColors[i].rgbGreen = *pData++;
                pbmi->bmiColors[i].rgbRed = *pData++;
                pbmi->bmiColors[i].rgbReserved = *pData++;
            }
        }
		else if (pbmi->bmiHeader.biCompression == BI_BITFIELDS)
        {
            pbmi->bmiColors[0] = *((RGBQUAD*)pData);
            pData += 4;
            pbmi->bmiColors[1] = *((RGBQUAD*)pData);
            pData += 4;
            pbmi->bmiColors[2] = *((RGBQUAD*)pData);
        }

        if (dwDataSize < pbmi->bmiHeader.biSizeImage)
            return NULL;

        hBitmap = CreateCompatibleBitmap(NULL, pbmi->bmiHeader.biWidth, 
            pbmi->bmiHeader.biHeight);

        if (hBitmap)
        {
            int nHeight;

            nHeight = (pbmi->bmiHeader.biHeight > 0) ? 
                pbmi->bmiHeader.biHeight : -(pbmi->bmiHeader.biHeight);

            SetDIBits(NULL, hBitmap, 0, nHeight, 
                ((BYTE*)pImageData + bfOffBits), (PBITMAPINFO)pbmi, DIB_RGB_COLORS);

            return hBitmap;
        }
        return NULL;
    }
    else if(pData[0] == 'G' && pData[1] == 'I' && pData[2] == 'F')
    {
        struct data_buffer pDataStruct;

        pDataStruct.data_count = dwDataSize;
        pDataStruct.data = (void *)pImageData;

        hBitmap = CreateBitmapFromGif(hdc, pDataStruct, pColor, pbTran);
        return hBitmap;
    }
    else if(pData[0] == 0xff && pData[1] == 0xd8)
    {
        struct data_buffer pDataStruct;

        pDataStruct.data_count = dwDataSize;
        pDataStruct.data = (void *)pImageData;

        hBitmap = CreateBitmapFromJpeg(pDataStruct);

        return hBitmap;
    }
    else if (pData[0] == 0x00 && pData[1] == 0x00)
    {
        struct data_buffer pDataStruct;

        pDataStruct.data_count = dwDataSize;
        pDataStruct.data = (void *)pImageData;

        hBitmap = CreateBitmapFromWbmp(pDataStruct);

        return hBitmap;
    }
#ifndef NOPNG
	else if ((pData[0] == 0x89) && (pData[1] == 0x50) && (pData[2] == 0x4e) &&
        (pData[3] == 0x47))
    {
        hBitmap = CreateBitmapFromPNG(pData);
        return hBitmap;
    }
#endif
    else
        return NULL;
}
/*********************************************************************\
* Function	       : DrawImageFromFile
* Purpose          :
*     Draw image from file
* Params	       :
*     hdc          : handle to device context
*     pszImgFile   : file to draw image
*     x            : x-coordinate of destination rectangle's
*                    upper-left corner
*     y            : y-coordinate of destination rectangle's
*                    upper-left corner
*     dwRop        : raster operation code
* Return	 	   :
*     if function success, return TRUE;
*     otherwise return FALSE  
**********************************************************************/
BOOL IMAGEAPI DrawImageFromFile(HDC hdc, PCSTR pszImgFile, int x, int y,
                                   DWORD dwRop)
{
    return DrawImageFromFileEx(hdc, pszImgFile, x, y, 0, 0, dwRop);
}
BOOL IMAGEAPI DrawImageFromFileEx(HDC hdc, PCSTR pszImgFile, int x, int y,
                                   int width , int height, DWORD dwRop)

{
    int               hFile;
    unsigned char     strType[4];
    BOOL              bRet = TRUE;
  
    if (!hdc)
        return NULL;
    if (!pszImgFile)
    {
        SetLastError(1);
        return NULL;
    }

    hFile = IMGOS_CreateFile(pszImgFile, PLXFS_ACCESS_READ, 0);
    if (hFile == -1)
    {
        SetLastError(1);
        return NULL;
    }

    IMGOS_ReadFile(hFile, strType, 4);

    if ((strType[0] == 'B') && (strType[1] == 'M'))
    {
        IMGOS_CloseFile(hFile);
        if (width == 0 || height == 0)
            bRet = DrawBitmapFromFile(hdc, x, y, pszImgFile, dwRop);           
    }
    else if ((strType[0] == 0x00) && (strType[1] == 0x00))
    {
        struct data_buffer pData;
        BOOL   fMapFile = TRUE;

        pData.data_count = IMGOS_GetFileSize(hFile);
        pData.data = IMGOS_MapFile(hFile, 0, pData.data_count);

        if (!pData.data)          
        {
            pData.data = malloc(pData.data_count);
            if (!pData.data)
            {
                IMGOS_CloseFile(hFile);
        
                SetLastError(1);
                return FALSE;
            }
            IMGOS_SetFilePointer(hFile, 0, 0);
            IMGOS_ReadFile(hFile, pData.data, pData.data_count);
            fMapFile = FALSE;
        }
   
		DrawWBMP(hdc, x, y, width, height, pData, dwRop);

        if (fMapFile)
            IMGOS_UnmapFile(hFile, pData.data, pData.data_count);
        else 
            free(pData.data);
       
        IMGOS_CloseFile(hFile);
    }
    else if ((strType[0] == 'G') && (strType[1] == 'I') && (strType[2] == 'F'))
    {
        struct data_buffer pData;
        BOOL   fMapFile = TRUE;
        
        pData.data_count = IMGOS_GetFileSize(hFile);
        pData.data = IMGOS_MapFile(hFile, 0, pData.data_count);

        if (!pData.data)          
        {
            pData.data = malloc(pData.data_count);
            if (!pData.data)
            {
                IMGOS_CloseFile(hFile);
        
                SetLastError(1);
                return FALSE;
            }
            IMGOS_SetFilePointer(hFile, 0, 0);
            IMGOS_ReadFile(hFile, pData.data, pData.data_count);
            fMapFile = FALSE;
        }

        bRet = DrawStaticGif (hdc, x, y, width, height, pData, dwRop);

        if (fMapFile)
            IMGOS_UnmapFile(hFile, pData.data, pData.data_count);
        else 
            free(pData.data);
       
        IMGOS_CloseFile(hFile);
    }
    else if ((strType[0] == 0xff) && (strType[1] == 0xd8))
    {
#ifndef NOJPEG
        struct data_buffer pData;
        BOOL   fMapFile = TRUE;

        pData.data_count = IMGOS_GetFileSize(hFile);
        pData.data = IMGOS_MapFile(hFile, 0, pData.data_count);

        if (!pData.data)          
        {
            pData.data = malloc(pData.data_count);
            if (!pData.data)
            {
                IMGOS_CloseFile(hFile);
        
                SetLastError(1);
                return FALSE;
            }
            IMGOS_SetFilePointer(hFile, 0, 0);
            IMGOS_ReadFile(hFile, pData.data, pData.data_count);
            fMapFile = FALSE;
        }

        bRet = DrawJpeg(hdc, x, y, width, height, pData, dwRop);

        if (fMapFile)
            IMGOS_UnmapFile(hFile, pData.data, pData.data_count);
        else 
            free(pData.data);
       
        IMGOS_CloseFile(hFile);
        

#endif //NOJPEG
    }
#ifndef NOPNG
    else if ((strType[0] == 0x89) && (strType[1] == 0x50) && (strType[2] == 0x4e) &&
        (strType[3] == 0x47))
    {
        IMGOS_CloseFile(hFile);
        bRet = DrawPNGFromFile(hdc, pszImgFile, x, y, width, height, dwRop);
    }
#endif
	else
	{
		IMGOS_CloseFile(hFile);
		return FALSE;
	}


    return bRet;
}

BOOL    IMAGEAPI StretchJpegFromFile(HDC hdc, PCSTR pszImgFile, int x, 
                                     int y, int width, int height, DWORD dwRop)
{   
    return DrawImageFromFileEx(hdc, pszImgFile, x, y, width, height, dwRop);
}

BOOL    IMAGEAPI StretchGif(HDC hdc, PCSTR pszImgFile, int x, int y,
                            int width, int height, DWORD dwRop)
{
    return DrawImageFromFileEx(hdc, pszImgFile, x, y, width, height, dwRop);
}

BOOL    IMAGEAPI StretchWBMP(HDC hdc, PCSTR pszImgFile, int x, int y,
                            int width, int height, DWORD dwRop)
{
    return DrawImageFromFileEx(hdc, pszImgFile, x, y, width, height, dwRop);
}

/*********************************************************************\
* Function	       : DrawImageFromData 
* Purpose          :
*     Draw image from file
* Params	       :
*     hdc          : handle to device context
*     pImageData   : data to draw image
*     dwDataSize   : size of data to draw image
*     x            : x-coordinate of destination rectangle's
*                    upper-left corner
*     y            : y-coordinate of destination rectangle's
*                    upper-left corner
*     dwRop        : raster operation code
* Return	 	   :
*     if function success, return TRUE;
*     otherwise return FALSE  
**********************************************************************/
BOOL IMAGEAPI DrawImageFromData(HDC hdc, const void* pImageData, 
                                    int x, int y, DWORD dwDataSize,
                                   DWORD dwRop)

{
    return DrawImageFromDataEx(hdc, pImageData, x, y, 0, 0, dwDataSize, dwRop);
}

BOOL IMAGEAPI DrawImageFromDataEx(HDC hdc, const void* pImageData, 
                                    int x, int y, int width, int height,
                                    DWORD dwDataSize, DWORD dwRop)

{
    BYTE*       pData;
    int         bRet = TRUE;

    if (!hdc)
        return FALSE;
    if (!pImageData)
        return FALSE;
    if (dwDataSize == 0)
        return FALSE;

    pData = (BYTE *)pImageData;

    if (pData[0] == 'B' && pData[1] == 'M')
    {
        /*HBITMAP hBitmap;
        SIZE    size;

        hBitmap = CreateBitmapFromImageData(NULL, pImageData, dwDataSize, NULL, NULL);
        GetImageDimensionFromData(pImageData, dwDataSize, &size);
        if (width == 0 || height == 0)
            BitBlt(hdc, x, y, size.cx, size.cy, (HDC)hBitmap, 0, 0, dwRop);
            else
            StretchBlt(hdc, x, y, width, height, (HDC)hBitmap, 0, 0, 
            size.cx, size.cy, dwRop);
        DeleteObject(hBitmap);*/
        static BITMAPINFO256 BitmapInfo;
        int         i;
        BITMAPINFO256* pbmi;
        DWORD       bfOffBits;
        int         nClrBits;
        
        pbmi = (PBITMAPINFO256)&BitmapInfo;
        bfOffBits = GET32(pData + 10);
        pData += 14;
        
        pbmi->bmiHeader.biSize = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSize)); 
        pbmi->bmiHeader.biWidth = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biWidth)); 
        pbmi->bmiHeader.biHeight = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biHeight)); 
        pbmi->bmiHeader.biPlanes = 
            GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biPlanes)); 
        pbmi->bmiHeader.biBitCount = 
            GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biBitCount)); 
        pbmi->bmiHeader.biCompression = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biCompression)); 
        pbmi->bmiHeader.biSizeImage = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSizeImage));
        pbmi->bmiHeader.biXPelsPerMeter = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biXPelsPerMeter));
        pbmi->bmiHeader.biYPelsPerMeter = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biYPelsPerMeter));
        pbmi->bmiHeader.biClrUsed = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrUsed));
        pbmi->bmiHeader.biClrImportant = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrImportant));

        if (pbmi->bmiHeader.biSizeImage > dwDataSize)
            return FALSE;
        if (pbmi->bmiHeader.biCompression == BI_RLE8)
            return FALSE;
        // Skips bitmap info header
        pData += sizeof(BITMAPINFOHEADER);
        
        nClrBits = pbmi->bmiHeader.biPlanes * pbmi->bmiHeader.biBitCount; 
        if (nClrBits != 1 && nClrBits != 2 && nClrBits != 4 && nClrBits != 8 &&
            nClrBits != 16 && nClrBits != 24 && nClrBits != 32)
            return NULL;
        
        // Gets the palettes if necessary
        if (nClrBits <= 8)
        {
            for (i = 0; i < (1 << nClrBits); i++)
            {
                pbmi->bmiColors[i].rgbBlue = *pData++;
                pbmi->bmiColors[i].rgbGreen = *pData++;
                pbmi->bmiColors[i].rgbRed = *pData++;
                pbmi->bmiColors[i].rgbReserved = *pData++;
            }
        }
        else if (pbmi->bmiHeader.biCompression == BI_BITFIELDS)
        {
            pbmi->bmiColors[0] = *((RGBQUAD*)pData);
            pData += 4;
            pbmi->bmiColors[1] = *((RGBQUAD*)pData);
            pData += 4;
            pbmi->bmiColors[2] = *((RGBQUAD*)pData);
        }
        if (width == 0 || height == 0)
            SetDIBitsToDevice(hdc, x, y, pbmi->bmiHeader.biWidth, 
                pbmi->bmiHeader.biHeight, 0, 0, 0, pbmi->bmiHeader.biHeight,
                ((BYTE*)pImageData + bfOffBits), (PBITMAPINFO)pbmi, DIB_RGB_COLORS);
        else
            StretchDIBits(hdc, x, y, width, height, 0, 0, 
                pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight,
                ((BYTE*)pImageData + bfOffBits), (PBITMAPINFO)pbmi, DIB_RGB_COLORS, SRCCOPY);
        
    }
    else if ((pData[0] == 0x00) && (pData[1] == 0x00))
    {
        struct data_buffer pData;

        pData.data = (void *)pImageData;
        pData.data_count = dwDataSize;
    
        DrawWBMP(hdc, x, y, width, height, pData, dwRop);
    }
    else if ((pData[0] == 'G') && (pData[1] == 'I') && (pData[2] == 'F'))
    {
        struct data_buffer pData;

        pData.data = (void *)pImageData;
        pData.data_count = dwDataSize;

        bRet = DrawStaticGif (hdc, x, y, width, height, pData, dwRop);
    }
    else if ((pData[0] == 0xff) && (pData[1] == 0xd8))
    {
#ifndef NOJPEG
        struct data_buffer pData;

        pData.data = (void *)pImageData;
        pData.data_count = dwDataSize;

		bRet = DrawJpeg(hdc, x, y, width, height, pData, dwRop);
#endif //NOJPEG
    }
#ifndef NOPNG
	else if ((pData[0] == 0x89) && (pData[1] == 0x50) && (pData[2] == 0x4e) &&
        (pData[3] == 0x47))
    {
        bRet = DrawPNGFromData(hdc, pData, x, y, width, height, dwRop);
    }
#endif
    else 
        return FALSE;
    return bRet;
}
/*********************************************************************\
* Function	       : GetImageDimensionFromFile  
* Purpose          :
*     Get image dimension from file
* Params	       :
*     pszImageFile : the file to get image dimension
*     pSize        : address of struct receiving dimensions
* Return	 	   :
*     if function success, return TRUE;
*     otherwise return FALSE  
**********************************************************************/
BOOL IMAGEAPI GetImageDimensionFromFile(PCSTR pszImgFile, PSIZE pSize)
{
    int              hFile; 
    unsigned char    strType[4];
	int              nImgType = -1;
    BYTE             size[100];
	struct data_buffer pData;
    BOOL             fMapFile = TRUE;
	void             *pTmp;
    BYTE            *pDataTemp;

    if (!pSize)
        return FALSE;
    hFile = IMGOS_CreateFile(pszImgFile, PLXFS_ACCESS_READ, 0);
    if (hFile == -1)
        return FALSE;

    IMGOS_ReadFile(hFile, strType, 4);

    if ((strType[0] == 'B') && (strType[1] == 'M'))
		nImgType = BMP_IMAGE;
	else if ((strType[0] == 'G') && (strType[1] == 'I') && (strType[2] == 'F'))
		nImgType = GIF_IMAGE;
	else if ((strType[0] == 0xff) && (strType[1] == 0xd8))
		nImgType = JPEG_IMAGE;
	else if ((strType[0] == 0x00) && (strType[1] == 0x00))
		nImgType = WBMP_IMAGE;
	else if ((strType[0] == 0x89) && (strType[1] == 0x50) &&
		(strType[2] == 0x4e) && (strType[3] == 0x47))
	    nImgType = PNG_IMAGE;

    switch (nImgType)
	{
	case BMP_IMAGE:
        IMGOS_SetFilePointer(hFile, 3 * sizeof(DWORD) + 3 * sizeof(WORD), 
                            PLXFS_SEEK_SET);
        IMGOS_ReadFile(hFile, (void *)size, 2 * sizeof(DWORD));
        pSize->cx = GET32(size);
        pSize->cy = GET32(size + sizeof(DWORD));
        IMGOS_CloseFile(hFile);
		return TRUE;

	case GIF_IMAGE:
    case JPEG_IMAGE:
	case WBMP_IMAGE:
	case PNG_IMAGE:
		pData.data_count = IMGOS_GetFileSize(hFile);
        pData.data = IMGOS_MapFile(hFile, 0, pData.data_count);

        if (!pData.data)          
        {
            pData.data = malloc(pData.data_count);
			pTmp = pData.data;
            if (!pData.data)
            {
                IMGOS_CloseFile(hFile);
        
                SetLastError(1);
                return FALSE;
            }
            IMGOS_SetFilePointer(hFile, 0, PLXFS_SEEK_SET);
            IMGOS_ReadFile(hFile, pData.data, pData.data_count);
            fMapFile = FALSE;
        }
       if (nImgType == GIF_IMAGE)
			GetGifDimensionFromData(pData, pSize);
       else if(nImgType == JPEG_IMAGE)
			GetJpegDimensionFromData(pData, pSize);
	   else if(nImgType == WBMP_IMAGE)
	   {
           pDataTemp = pData.data;
			pDataTemp += 2;
			pSize->cx = (int)DecodeMultiByteInt(&pDataTemp);
			pSize->cy = (int)DecodeMultiByteInt(&pDataTemp);
	   }
	   else if(nImgType == PNG_IMAGE)
		   GetPngDimensionFromData(pData.data, pSize);

        if (fMapFile)
            IMGOS_UnmapFile(hFile, pData.data, pData.data_count);
        else 
            free(pTmp);
       
        IMGOS_CloseFile(hFile);
	    return TRUE;	
	default:
		IMGOS_CloseFile(hFile);
		break;
    }
    return FALSE;
}

/*********************************************************************\
* Function	       : GetImageDimensionFromData
* Purpose          :
*     Get image dimension from image data
* Params	       :
*     pImageData   : the address of data to get image dimension
*     dwDataSize   : data size
*     pSize        : address of struct receiving dimensions
* Return	 	   :
*     if function success, return TRUE;
*     otherwise return FALSE  
**********************************************************************/
BOOL IMAGEAPI GetImageDimensionFromData(const void* pImageData, 
                                        DWORD dwDataSize, PSIZE pSize)
{
    BYTE * pData;

    if (!pSize)
        return FALSE;
    if (!pImageData)
        return FALSE;
    if (dwDataSize == 0)
        return FALSE;

    pData = (BYTE *) pImageData;
    if (pData[0] == 'B' && pData[1] == 'M')
    {
        int offset;
        offset = 3 * sizeof(DWORD) + 3 * sizeof(WORD); 
        pSize->cx = GET32(pData + offset);
        pSize->cy = GET32(pData + offset + sizeof(DWORD));
        return TRUE;
    }
    else if (pData[0] == 'G' && pData[1] == 'I' && pData[2] == 'F')
    {
        struct data_buffer pDataTmp;

        pDataTmp.data = (void *)pImageData;
        pDataTmp.data_count = dwDataSize;

        return GetGifDimensionFromData(pDataTmp, pSize);
    }
    else if (pData[0] == 0xff && pData[1] == 0xd8)
    {
        struct data_buffer pDataTmp;

        pDataTmp.data = (void *)pImageData;
        pDataTmp.data_count = dwDataSize;

        return GetJpegDimensionFromData(pDataTmp, pSize);
    }
    else if (pData[0] == 0x00 && pData[1] == 0x00)
    {
        pData += 2;
        pSize->cx = (int)DecodeMultiByteInt(&pData);
        pSize->cy = (int)DecodeMultiByteInt(&pData);
        return TRUE;
    }
	else if ((pData[0] == 0x89) && (pData[1] == 0x50) && (pData[2] == 0x4e) &&
        (pData[3] == 0x47))
	{
		return GetPngDimensionFromData(pData, pSize);
	}
    return FALSE;
}

BOOL IMAGEAPI IsGIFDynamic(void * pszGIFFile, BOOL bFile, int nDataSize)
{
    int hFile, nFileSize;
    BYTE* pData;
    BOOL bResult;

    if (bFile)
    {
        hFile = IMGOS_CreateFile(pszGIFFile, PLXFS_ACCESS_READ, 0);
        if (hFile == -1)
        {
            return FALSE;
        }
        nFileSize = IMGOS_GetFileSize(hFile);
        pData = (BYTE*)malloc(nFileSize);
        if (!pData)
        {
            IMGOS_CloseFile(hFile);
            return FALSE;
        }
        IMGOS_SetFilePointer(hFile, 0, 0);
        IMGOS_ReadFile(hFile, pData, nFileSize);
    }
    else
    {
        pData = pszGIFFile;
        nFileSize = nDataSize;
    }
    
    if (pData[0] == 'G' && pData[1] == 'I' && pData[2] == 'F') 
    {
        bResult = GIF_IsGIFDynamic(pData, nFileSize);
        if (bFile)
        {
            IMGOS_CloseFile(hFile);
            free(pData);
        }
        return bResult;
    }
    else
    {
        if (bFile)
        {
            IMGOS_CloseFile(hFile);
            free(pData);
        }
    }
    return FALSE;
}

/*********************************************************************\
* Function	      : StartAnimatedGIFFromFile
* Purpose         :
*     start drawing gif image from file
* Params	      :
*     hWnd        : the window to show image
*     pszGIFFile  : the file to draw image
*     x           : x-coordinate of destination rectangle's
*                   upper-left corner
*     y           : y-coordinate of destination rectangle's
*                   upper-left corner
*     dm          : DM_NONE, DM_BITMAP and DM_DATA
* Return	 	  :
*      return the object of animated gif
**********************************************************************/
HGIFANIMATE IMAGEAPI StartAnimatedGIFFromFile(HWND hWnd, PCSTR pszGIFFile, 
                                              int x, int y, int dm)
{
    return StartAnimatedGIFFromFileEx(hWnd, pszGIFFile, x, y, 0, 0, dm);
}
HGIFANIMATE IMAGEAPI StartAnimatedGIFFromFileEx(HWND hWnd, PCSTR pszGIFFile, 
                                              int x, int y, int width, int height, int dm)
{
    int       hFile;
    HGIFANIMATE hGif;
    char        strType[8];

    hFile = IMGOS_CreateFile(pszGIFFile, PLXFS_ACCESS_READ, 0);
    if (hFile == -1)
    {
        return NULL;
    }
    IMGOS_ReadFile(hFile, strType, 6);
    if ((strType[0] == 'G') && (strType[1] == 'I') && (strType[2] == 'F'))
    {    
        struct data_buffer pData;

        pData.data_count = IMGOS_GetFileSize(hFile);
        //pData.data = IMGOS_MapFile(hFile, 0, pData.data_count);
        //pData.nMapFile = TRUE;

        //if (!pData.data)          
        {
            pData.data = malloc(pData.data_count);
            if (!pData.data)
            {
                IMGOS_CloseFile(hFile);

                SetLastError(1);
                return FALSE;
            }
            IMGOS_SetFilePointer(hFile, 0, 0);
            IMGOS_ReadFile(hFile, pData.data, pData.data_count);
            pData.nMapFile = 0;
        }
        if (strType[3] == '8' && strType[4] == '9')
            hGif = DrawGif (hWnd, x, y, width, height, pData, 1, dm, 0);
        else if (strType[3] == '8' && strType[4] == '7')
            hGif = DrawGif (hWnd, x, y, width, height, pData, 0, dm, 0);
        
        IMGOS_CloseFile(hFile);    
    }
    else 
	{
		IMGOS_CloseFile(hFile);
        hGif = NULL;
	}

    return hGif;
}

/*********************************************************************\
* Function	      : StartAnimatedGIFFromData
* Purpose         :
*     start drawing gif image from data
* Params	      :
*     hWnd        : the window to show image
*     pGifData    : the data to draw image
*     dwDataSize  : the size of data to draw gif
*     x           : x-coordinate of destination rectangle's
*                   upper-left corner
*     y           : y-coordinate of destination rectangle's
*                   upper-left corner
*     dm          : DM_NONE, DM_BITMAP and DM_DATA
* Return	 	  :
*      return the object of animated gif
**********************************************************************/
HGIFANIMATE IMAGEAPI StartAnimatedGIFFromData(HWND hWnd, const void* pGIFData,
                                              DWORD dwDataSize,
                                              int x, int y, int dm)
{
    return StartAnimatedGIFFromDataEx(hWnd, pGIFData, dwDataSize, x, y, 0, 0, dm);
}

HGIFANIMATE IMAGEAPI StartAnimatedGIFFromDataEx(HWND hWnd, const void* pGIFData,
                                              DWORD dwDataSize,int x, int y, 
                                              int width, int height, int dm)
{
    HGIFANIMATE hGif;
    BYTE      *pGif;

    pGif = (BYTE *)pGIFData;
    if (!pGif)
        return NULL;
    if (dwDataSize == 0)
        return NULL;

    if (pGif[0] == 'G' && pGif[1] == 'I' && pGif[2] == 'F')        
    {       
        struct data_buffer pData;

        pData.data = (void *)pGIFData;
        pData.data_count = dwDataSize;
        pData.nMapFile = -1;

        if (pGif[3] == '8' && pGif[4] == '9')
            hGif = DrawGif (hWnd, x, y, width, height, pData, 1, dm, 0);
        else if (pGif[3] == '8' && pGif[4] == '7')
            hGif = DrawGif (hWnd, x, y, width, height, pData, 0, dm, 0);            
    }
    else 
        hGif = NULL;

    return hGif;
}

BOOL		IMAGEAPI SetPaintBkFunc(HGIFANIMATE hGIFAnimate, 
									PAINTBKFUNC pfnPaintBkFunc)
{
	PGIFWNDDATA pGifData;

	if (!hGIFAnimate)
		return FALSE;
	if (!pfnPaintBkFunc)
		return FALSE;
	
	pGifData = (PGIFWNDDATA)hGIFAnimate;
	pGifData->pfnPaintBkFunc = pfnPaintBkFunc;
	//pfnPaintBkFunc(hGIFAnimate, pGifData->x, pGifData->y, pGifData->hMemDC);
	//PaintGifImage(NULL, pGifData, FALSE);
	return TRUE;
}


/*********************************************************************\
* Function	      : DrawGIFFromFileTime
* Purpose         :
*     drawing gif image from data periodly
* Params	      :
*     hWnd        : the window to show image
*     pcszImgFile : the file to draw image
*     x           : x-coordinate of destination rectangle's
*                   upper-left corner
*     y           : y-coordinate of destination rectangle's
*                   upper-left corner
*     dm          : DM_NONE, DM_BITMAP and DM_DATA
*     Times       : the times to display image
* Return	 	  :
*      return the object of animated gif
**********************************************************************/
HGIFANIMATE IMAGEAPI DrawGIFFromFileTime(HWND hWnd, 
                                         PCSTR pcszImgFile, 
                                         int x, int y, int dm, int Times)
{
    return DrawGIFFromFileTimeEx(hWnd, pcszImgFile, x, y, 0, 0, dm, Times);
}
HGIFANIMATE IMAGEAPI DrawGIFFromFileTimeEx(HWND hWnd, 
                                         PCSTR pcszImgFile, int x, int y, 
                                         int width, int height, int dm, int Times)
{
    int      hFile;
    HGIFANIMATE hGif;
    char        strType[8];

    if (Times <= 0)
        return NULL;
    hFile = IMGOS_CreateFile(pcszImgFile, PLXFS_ACCESS_READ, 0);
    if (hFile == -1)
    {
        return NULL;
    }
    IMGOS_ReadFile(hFile, strType, 6);
    if ((strType[0] == 'G') && (strType[1] == 'I') && (strType[2] == 'F'))
    {        
        struct data_buffer pData;
        
        pData.data_count = IMGOS_GetFileSize(hFile);
        pData.data = IMGOS_MapFile(hFile, 0, pData.data_count);
        pData.nMapFile = 1;

        if (!pData.data)          
        {
            pData.data = malloc(pData.data_count);
            if (!pData.data)
            {
                IMGOS_CloseFile(hFile);
    
                SetLastError(1);
                return FALSE;
            }
            IMGOS_SetFilePointer(hFile, 0, PLXFS_SEEK_SET);
            IMGOS_ReadFile(hFile, pData.data, pData.data_count);
            pData.nMapFile = 0;
        }

        if (strType[3] == '8' && strType[4] == '9')
            hGif = DrawGif (hWnd, x, y, width, height, pData, 1, dm, Times);
        else if (strType[3] == '8' && strType[4] == '7')
            hGif = DrawGif (hWnd, x, y, width, height, pData, 0, dm, Times);
     
        IMGOS_CloseFile(hFile);       
    }
    else 
	{
		IMGOS_CloseFile(hFile);
        hGif = NULL;
	}

    return hGif;
}

/*********************************************************************\
* Function	      : DrawGIFFromDataTime
* Purpose         :
*     drawing gif image from data periodly
* Params	      :
*     hWnd        : the window to show image
*     pGifData    : the data to draw image
*     dwDataSize  : data size 
*     x           : x-coordinate of destination rectangle's
*                   upper-left corner
*     y           : y-coordinate of destination rectangle's
*                   upper-left corner
*     dm          : DM_NONE, DM_BITMAP and DM_DATA
*     Times       : the times to display image
* Return	 	  :
*      return the object of animated gif
**********************************************************************/
HGIFANIMATE IMAGEAPI DrawGIFFromDataTime(HWND hWnd, 
                                         void *pGifData,
                                         DWORD dwDataSize,
                                         int x, int y, int dm, int Times)
{
    return DrawGIFFromDataTimeEx(hWnd, pGifData, dwDataSize, x, y, 0, 0, dm, Times);
}

HGIFANIMATE IMAGEAPI DrawGIFFromDataTimeEx(HWND hWnd, void *pGifData, DWORD dwDataSize,
                                         int x, int y, int width, int height,
                                         int dm, int Times)
{
    HGIFANIMATE hGif;
    BYTE *pImgData;

    if (Times <= 0)
        return NULL;
    pImgData = (BYTE *) pGifData;
    if (!pImgData)
        return NULL;
    if (dwDataSize == 0)
        return NULL;

    if ((pImgData[0] == 'G') && (pImgData[1] == 'I') && (pImgData[2] == 'F'))
    {        
        struct data_buffer pData;

        pData.data_count = dwDataSize;
        pData.data = pGifData;
        pData.nMapFile = -1;

        if (pImgData[3] == '8' && pImgData[4] == '9')
            hGif = DrawGif (hWnd, x, y, width, height, pData, 1, dm, Times);
        else if (pImgData[3] == '8' && pImgData[4] == '7')
            hGif = DrawGif (hWnd, x, y, width, height, pData, 0, dm, Times);                   
    }
    else 
        hGif = NULL;

    return hGif;
}
/*********************************************************************\
* Function	   : EndAnimatedGIF   
* Purpose      :
*     End show gif dynamicly
* Params	   :
*     hGif     : the gif object drawing
* Return	   :
*     if function success, return TRUE;
*     otherwise return FALSE
**********************************************************************/
BOOL IMAGEAPI EndAnimatedGIF(HGIFANIMATE hGif)
{
    int i;
    PGIFWNDDATA pGifData;

    pGifData = (PGIFWNDDATA)hGif;
    if (pGifData == NULL)
        return NULL;
    for (i = 0; i < MAX_GIFNUM; i++)
    {
        if (GifWndData[i].pGif == pGifData->pGif)
            break;
    }
    if (i != MAX_GIFNUM && GifWndData[i].hwndParent)
    {
        KillTimer(NULL, GifWndData[i].m_timer);
        

        if (GifWndData[i].pGif)
        {
            char *p;
            p = (char *)GIF_GetUserData(GifWndData[i].pGif);
            /*if (GifWndData[i].nMapFile == 1)    
                UnmapFile(p);
            else if (GifWndData[i].nMapFile == 0)    */
            if (GifWndData[i].nMapFile == 0)    
                free(p);

            GIF_Destruct(GifWndData[i].pGif);
            if (GifWndData[i].pfnPaintBkFunc && GifWndData[i].hwndParent != (HWND)-1)
            {
			    DeleteObject((HGDIOBJ)GifWndData[i].hCompatibleBmp);
                DeleteDC(GifWndData[i].hCompatibleDC);
            }
            free(GifWndData[i].pGif);
        }
        GifWndData[i].hwndParent = NULL;
        if (GifWndData[i].dm == DM_BITMAP)
        {
            PHEADOFBITMAP phBmp, pLhBmp;

            phBmp = (PHEADOFBITMAP)GifWndData[i].pHead;

            while (phBmp->pNext != GifWndData[i].pHead)
            {
                DeleteObject(phBmp->hBitmap);
                free(phBmp->pData);  
                pLhBmp = phBmp;
                phBmp = phBmp->pNext;
                free(pLhBmp);
            }
            DeleteObject(phBmp->hBitmap);
            free(phBmp->pData); 
            free(phBmp);
        }

        if (GifWndData[i].dm == DM_DATA)
        {
            PHEADOFDATA phData, pLhData;
            
            phData = (PHEADOFDATA)GifWndData[i].pHead;
            if (phData)
            {
                while (phData->pNext != GifWndData[i].pHead)
                {
                    free(phData->pData);
                    pLhData = phData;
                    phData = phData->pNext;
                    free(pLhData);
                }
                free(phData->pData);
                free(phData);
            }
        }

        return TRUE;
    }
    else
        return FALSE;

}

/*********************************************************************\
* Function	   : PaintAnimatedGIF
* Purpose      :
*      paint one frame of animated gif
* Params	   :
*      hdc     : handle of device context
*      hGif    : handle of animated gif
* Return	   :
*      if function success, return TRUE;
*      otherwise return FALSE. 
**********************************************************************/

BOOL IMAGEAPI PaintAnimatedGIF(HDC hdc, HGIFANIMATE hGif)
{
    int         ret;
    PGIFWNDDATA pGifData;
    int         nframe;

    if (!hGif)
        return FALSE;
    if (!hdc)
        return FALSE;

    pGifData = (PGIFWNDDATA)hGif;
    if (pGifData->hwndParent == (HWND)-1)
    {
        pGifData->hCompatibleDC = hdc;
        if (pGifData->pfnPaintBkFunc)
            pGifData->pfnPaintBkFunc(hGif, FALSE, pGifData->x, pGifData->y, hdc);
        PaintGifImage(hdc, pGifData, FALSE, FALSE);
        return TRUE;
    }
	if (pGifData->pfnPaintBkFunc)
	{
        if (!pGifData->hCompatibleDC)
        {
            pGifData->hCompatibleDC = CreateCompatibleDC(hdc);
            if (!pGifData->hCompatibleDC)
                return FALSE;
            if (pGifData->DstHeight == 0 && pGifData->DstWidth == 0)
                pGifData->hCompatibleBmp = CreateCompatibleBitmap(hdc, pGifData->width,
                    pGifData->height);
            else
                pGifData->hCompatibleBmp = CreateCompatibleBitmap(hdc, pGifData->DstWidth,
                    pGifData->DstHeight);
            if (!pGifData->hCompatibleBmp)
            {
                DeleteDC(pGifData->hCompatibleDC);
                return FALSE;
            }
            SelectObject(pGifData->hCompatibleDC, pGifData->hCompatibleBmp);
            pGifData->pfnPaintBkFunc(hGif, FALSE, pGifData->x, pGifData->y, pGifData->hCompatibleDC);
	        PaintGifImage(NULL, pGifData, FALSE, FALSE);
        }
        if (pGifData->DstHeight == 0 && pGifData->DstWidth == 0)
		    BitBlt(hdc, pGifData->x, pGifData->y, pGifData->width, pGifData->height,
			    pGifData->hCompatibleDC, 0, 0, SRCCOPY);
        else
            BitBlt(hdc, pGifData->x, pGifData->y, pGifData->DstWidth, pGifData->DstHeight,
			    pGifData->hCompatibleDC, 0, 0, SRCCOPY);
            /*StretchBlt(hdc, pGifData->x, pGifData->y, pGifData->DstWidth, pGifData->DstHeight,
                pGifData->hCompatibleDC, 0, 0, pGifData->width, pGifData->height, SRCCOPY);*/
		return TRUE;
	}

    if (pGifData != NULL && !pGifData->bRepaint)
        return FALSE;
    
    if ((pGifData != NULL) && (pGifData->hwndParent != NULL))
    {
        nframe = pGifData->nframes;
        if (pGifData->LastMethod == 1)
        {
            nframe = pGifData->nframes;
            pGifData->nframes = 0;
            GIF_Reset( pGifData->pGif);
            pGifData->datasource.next_input_byte = pGifData->datasource.first_input_byte;
            pGifData->datasource.bytes_in_buffer = pGifData->datasource.nbytes;  
			ret = GIF_ReadFrameHeader( pGifData->pGif, 0);
            pGifData->nframes ++;
            GIF_GetFrameDimensions(pGifData->pGif, &(pGifData->frame));
            GIF_SetColorMode(pGifData->pGif, GCS_RAW);
            SetBitmapInfo(pGifData);
            nframe --;
			PaintGifImage(hdc, pGifData, FALSE, FALSE);
        }
        else
        {
            pGifData->nframes = 0;
            while (1)
            {
                
                GIF_Reset( pGifData->pGif);
                pGifData->datasource.next_input_byte = pGifData->datasource.first_input_byte;
                pGifData->datasource.bytes_in_buffer = pGifData->datasource.nbytes;
				ret = GIF_ReadFrameHeader(pGifData->pGif , 0);    
                if (ret < 0)
					return FALSE;
                GIF_GetFrameDimensions(pGifData->pGif, &(pGifData->frame));
                pGifData->nframes ++;
#ifndef _EMULATE_
				KickDog();
#endif //_EMULATE_
                if (nframe == pGifData->nframes)
                    break;
            }
            GIF_SetColorMode(pGifData->pGif, GCS_RAW);
            SetBitmapInfo(pGifData);
            //m_decode_state = S_READSCANLINES;
            PaintGifImage(hdc, pGifData, TRUE, FALSE);   
        }
        return TRUE;
    }
    
    else
		return FALSE;
}
/*********************************************************************\
* Function	   : ScrollAnimatedGIF
* Purpose      :
*     Scroll animated GIF
* Params	   :
*     hGif     : the gif object to be scrolled
*     dx       : the horizontal distance to scroll image
*     dy       : the vertical distance to scroll image
* Return	   :
*     if function success, return TRUE;
*     otherwise return FALSE	  
**********************************************************************/
BOOL IMAGEAPI ScrollAnimatedGIF(HGIFANIMATE hGif, int dx, int dy)
{
    PGIFWNDDATA pGifData;
	//RECT		rc;

    if (!hGif)
        return FALSE;
    

    pGifData = (PGIFWNDDATA)hGif;   
    if (pGifData->hwndParent == NULL)
        return FALSE;
    pGifData->y += dy;
    pGifData->x += dx;

    return TRUE;
	/*if (!pGifData->pfnPaintBkFunc)
	{
		InvalidateRect(pGifData->hwndParent , NULL, TRUE);
		UpdateWindow(pGifData->hwndParent );
		return TRUE;
	}
//	SetRect(&rc, pGifData->x, pGifData->y, 
//		pGifData->x + pGifData->width, pGifData->y + pGifData->height);
	InvalidateRect(pGifData->hwndParent , NULL, TRUE);
    UpdateWindow(pGifData->hwndParent );*/
}

BOOL IMAGEAPI ScrollAnimatedGIFEx(HGIFANIMATE hGif, int x, int y)
{
    PGIFWNDDATA pGifData;
	//RECT		rc;

    if (!hGif)
        return FALSE;
    

    pGifData = (PGIFWNDDATA)hGif;   
    if (pGifData->hwndParent == NULL)
        return FALSE;
    pGifData->y = y;
    pGifData->x = x;

    return TRUE;
}

/*********************************************************************\
* Function	   : BmpFileToJpegFile
* Purpose      :
*     convert bitmap(24bit) file  to jpeg file
* Params	   :
*     BmpFile  : bitmap file to be converted
*     JpegFile : jpeg file converted from bitmap file
* Return	 :
*     if success to convert bitmap to jpeg, return TRUE;
*     otherwise return FALSE.	   
* Remarks	   :
*     only support 24bit bitmap.
**********************************************************************/
BOOL IMAGEAPI BmpFileToJpegFile(char* BmpFile, char* JpegFile)
{
    int   hBmp, nBitCount;
    BYTE  Head[54], *Buff;
    DWORD ImgWidth,ImgHeight,i,j,p;
    BYTE  bt0,bt1,bt2, stuff[4];
    BOOL   ret;

    hBmp = IMGOS_CreateFile(BmpFile, 1, 0);
    if (hBmp == -1)
        return FALSE;
    
    IMGOS_ReadFile(hBmp, Head, 54);
    if ((Head[0] != 'B') || (Head[1] != 'M'))
    {
        IMGOS_CloseFile(hBmp);
        return FALSE;
    }
    
    ImgWidth = GET32(Head+18);
    ImgHeight= GET32(Head+22); 
    nBitCount = GET32(Head+28);
    if (nBitCount != 24)
    {
        IMGOS_CloseFile(hBmp);
        return FALSE;
    }
    p=(ImgWidth*3)%4;
        
    Buff = (BYTE *) malloc(ImgWidth*ImgHeight*3);
    if(!Buff) 
    {
        IMGOS_CloseFile(hBmp);
        return FALSE;
    }
 
    for(i=0;i<ImgHeight;i++)  // read bitmap pixels array to buffer 
    {
        for(j=0;j<ImgWidth;j++)
        {   
            IMGOS_ReadFile(hBmp, (BYTE *)&bt0, 1);
            IMGOS_ReadFile(hBmp, (BYTE *)&bt1, 1);
            IMGOS_ReadFile(hBmp, (BYTE *)&bt2, 1);
            
            *(Buff+i*ImgWidth*3+j*3+0)= bt0;
            *(Buff+i*ImgWidth*3+j*3+1)= bt1;
            *(Buff+i*ImgWidth*3+j*3+2)= bt2;
        }
        if(p!=0)
            IMGOS_ReadFile(hBmp, stuff, 4 - p );       
    }
    
    ret = RGBToJpegFile(JpegFile,Buff,ImgWidth,ImgHeight);
    
    IMGOS_CloseFile(hBmp);
    
    free(Buff);
    return ret;
}



/*********************************************************************\
* Function	   : BmpDataToJpegFile
* Purpose      :
*     convert bitmap(24bit) file  to jpeg file
* Params	   :
*     BmpData  : bitmap Data to be converted
*     JpegFile : jpeg file converted from bitmap file
* Return	 :
*     if success to convert bitmap to jpeg, return TRUE;
*     otherwise return FALSE.	   
* Remarks	   :
*     only support 24bit bitmap.
**********************************************************************/
BOOL IMAGEAPI BmpDataToJpegFile(const char* BmpData, DWORD dwDataSize, 
								char* JpegFile)
{  
    BYTE  *Buff, *pData;
    DWORD ImgWidth,ImgHeight, nBitCount,i,j,p;  
    BOOL   ret;

    pData = (BYTE*)BmpData;
	if (!pData)
		return FALSE;
	if (dwDataSize < 54) // sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
		return FALSE;
    if ((pData[0] != 'B') || (pData[1] != 'M'))
        return FALSE;
	
    ImgWidth = GET32(pData+18);
    ImgHeight = GET32(pData+22);
    nBitCount = GET32(pData+28);

    if (nBitCount != 24)
    {        
        return FALSE;
    }
	if (dwDataSize < 54 + ((ImgWidth + 3) & ~3) * ImgHeight)
	{
		return FALSE;
	}

    p=(ImgWidth*3)%4;
        
    Buff = (BYTE *) malloc(ImgWidth*ImgHeight*3);
    if(!Buff) 
    {    
        return FALSE;
    }
    	
	pData = (BYTE*)(BmpData + 54);
	for(i=0;i<ImgHeight;i++)  // read bitmap pixels array to buffer 
	{
		for(j=0;j<ImgWidth;j++)
		{   				
			*(Buff+i*ImgWidth*3+j*3+0)= *pData++;
			*(Buff+i*ImgWidth*3+j*3+1)= *pData++;
			*(Buff+i*ImgWidth*3+j*3+2)= *pData++;
		}
		if(p!=0)
			pData += 4-p;
	}
	    
    ret = RGBToJpegFile(JpegFile,Buff,ImgWidth,ImgHeight);        
    
    free(Buff);
    return ret;
}

/*********************************************************************\
* Function	   : BmpFileToGifFile
* Purpose      :
*     convert bitmap(256) file  to gif file
* Params	   :
*     BmpFile  : bitmap file to be converted
*     GifFile  : gif file converted from bitmap file
* Return	 :
*     if success to convert bitmap to gif, return TRUE;
*     otherwise return FALSE.	   
* Remarks	   :
*     only support bitmap with 256 colors.
**********************************************************************/
BOOL IMAGEAPI BmpFileToGifFile(char* BmpFile, char* GifFile)
{
    int hBmp, hGif;
    BOOL ret, bMapFile = TRUE;
    DWORD dwFileSize;
    char *ImageBuffer;
    char Head[3];
    
    hBmp = IMGOS_CreateFile(BmpFile, 1, 0);
    if (hBmp == -1)
        return FALSE;

    IMGOS_ReadFile(hBmp, Head, 2);
    if ((Head[0] != 'B') || (Head[1] != 'M'))
    {
        IMGOS_CloseFile(hBmp);
        return FALSE;
    }
    dwFileSize = IMGOS_GetFileSize(hBmp);
    ImageBuffer = IMGOS_MapFile(hBmp, 0, dwFileSize);
    if(!ImageBuffer)
    {
        bMapFile = FALSE;
        ImageBuffer = (char*)malloc(dwFileSize);
        if(!ImageBuffer)
        {
            IMGOS_CloseFile(hBmp);
            return FALSE;
        }
    }
    IMGOS_ReadFile(hBmp, ImageBuffer, dwFileSize);
    hGif = IMGOS_CreateFile(GifFile, 
        PLXFS_CREATE_NEW | PLXFS_CREATE_TRUNCATE | PLXFS_ACCESS_WRITE,
        0);
    if (hGif == -1)    	
    {
        IMGOS_CloseFile(hBmp);
        return FALSE;
    }
        
    
    ret = BMPToGifFile(hGif, ImageBuffer);
    
    if (bMapFile)
        IMGOS_UnmapFile(hBmp, ImageBuffer, dwFileSize);
    else 
        free(ImageBuffer);

    IMGOS_CloseFile(hBmp);
    IMGOS_CloseFile(hGif);
    
    return ret;
}

/*********************************************************************\
* Function	   : BmpDataToGifFile
* Purpose      :
*     convert bitmap(256) file  to gif file
* Params	   :
*     BmpData  : bitmap data to be converted
*     GifFile  : gif file converted from bitmap file
* Return	 :
*     if success to convert bitmap to gif, return TRUE;
*     otherwise return FALSE.	   
* Remarks	   :
*     only support bitmap with 256 colors.
**********************************************************************/
BOOL IMAGEAPI BmpDataToGifFile(const char* BmpData, DWORD dwDataSize,
							   char* GifFile)
{
    int  hGif;
    BOOL ret;    
    char *pData = (char*)BmpData;
    
    if (!pData || dwDataSize < 54)
		return FALSE;
    if ((pData[0] != 'B') || (pData[1] != 'M'))
        return FALSE;

    hGif = IMGOS_CreateFile(GifFile, 
        PLXFS_CREATE_NEW | PLXFS_CREATE_TRUNCATE | PLXFS_ACCESS_WRITE,
        0);
    if (hGif == -1)    		
        return FALSE;
        
    ret = BMPToGifFile(hGif, pData);
    
    IMGOS_CloseFile(hGif);
    
    return ret;
}
/*********************************************************************\
* Function	   : BmpFileToPngFile
* Purpose      :
*     convert bitmap file  to Png file
* Params	   :
*     BmpFile  : bitmap file to be converted
*     PngFile  : Png file converted from bitmap file
* Return	 :
*     if success to convert bitmap to Png, return TRUE;
*     otherwise return FALSE.	   
* Remarks	   :
*     only support 1bit, 2bit, 4bit, 8bit, 16bit bitmap.
**********************************************************************/
BOOL IMAGEAPI BmpFileToPngFile(char* BmpFile, char* PngFile)
{
    int  hBmp, hPng;
    BOOL ret, bMapFile = TRUE;
    DWORD dwFileSize;
    char *ImageBuffer;
    char Head[3];
    
    hBmp = IMGOS_CreateFile(BmpFile, 1, 0);
    if (hBmp == -1)
        return FALSE;

    IMGOS_ReadFile(hBmp, Head, 2);
    if ((Head[0] != 'B') || (Head[1] != 'M'))
    {
        IMGOS_CloseFile(hBmp);
        return FALSE;
    }

    hPng = IMGOS_CreateFile(PngFile,
		PLXFS_CREATE_NEW | PLXFS_CREATE_TRUNCATE | PLXFS_ACCESS_WRITE, 0);    
	if (hPng == -1)
    {		
        IMGOS_CloseFile(hBmp);
		return FALSE;
	}

    dwFileSize = IMGOS_GetFileSize(hBmp);
    ImageBuffer = IMGOS_MapFile(hBmp, 0, dwFileSize);
    if(!ImageBuffer)
    {
        bMapFile = FALSE;
        ImageBuffer = (char*)malloc(dwFileSize);
        if(!ImageBuffer)
            return FALSE;
        IMGOS_ReadFile(hBmp, ImageBuffer, dwFileSize);    
    }
    ret = BMPToPngFile(hPng, ImageBuffer);
    
    if (bMapFile)
        IMGOS_UnmapFile(hBmp, ImageBuffer, dwFileSize);
    else 
        free(ImageBuffer);

    IMGOS_CloseFile(hBmp);
    IMGOS_CloseFile(hPng);
    
    return ret;
}

/*********************************************************************\
* Function	   : BmpDataToPngFile
* Purpose      :
*     convert bitmap file to Png file
* Params	   :
*     BmpData  : bitmap Data to be converted
*     PngFile  : Png file converted from bitmap file
* Return	 :
*     if success to convert bitmap to png, return TRUE;
*     otherwise return FALSE.	   
* Remarks	   :
*     only support 1bit, 2bit, 4bit, 8bit, 16bit bitmap.
**********************************************************************/
BOOL IMAGEAPI BmpDataToPngFile(const char* BmpData, DWORD dwDataSize, 
								char* PngFile)
{  
    int  hPng;
    BOOL ret;    
    char *pData = (char*)BmpData;
    
    if (!pData || dwDataSize < 54)
		return FALSE;
    if ((pData[0] != 'B') || (pData[1] != 'M'))
        return FALSE;

    hPng = IMGOS_CreateFile(PngFile, 
        PLXFS_CREATE_NEW | PLXFS_CREATE_TRUNCATE | PLXFS_ACCESS_WRITE,
        0);
    if (hPng == -1)    		
        return FALSE;
        
    ret = BMPToPngFile(hPng, pData);
    
    IMGOS_CloseFile(hPng);
    
    return ret;
}
