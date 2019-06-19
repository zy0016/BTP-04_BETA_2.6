  /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Image Lib
 *
 * Purpose  : Implements Gif functions.
 *            
\**************************************************************************/

#include "window.h"
#include "filesys.h"
#include "malloc.h"
#include "string.h"
//#include "stdio.h"

#include "hpimage.h"
#include "DrawImg.h"

#ifndef NOJPEG
#include "jpeg/djpeg.h"
#include "jpeg/jpeglib.h"
#include <SETJMP.H>
#endif //NOJPEG

#ifndef NOJPEG
/* Jpeg decoder state */
enum djpeg_state {
			J_S_INIT,
			J_S_READHEAD,
			J_S_STARTDECOMPRESS,
			J_S_STARTOUTPUT,
			J_S_READSCANLINES,
			J_S_FINISHOUTPUT,
			J_S_FINISHDECOMPRESS,
			J_S_ENDDECODE,
			J_S_ERROR
};

//#define DISP_MAX_WIDTH      1024
//#define DISP_MAX_HEIGHT     768

typedef struct _tagStretchPattern{
    BYTE *  pbWidthPattern;
    BYTE *  pbHeightPattern;
}STRETCHPATTERN, *PSTRETCHPATTERN;

int			            j_decode_state;     /* Jpeg_decoder state */
int			            j_last_scan;		/* If last scan displied */
PJPEGOBJ	            j_pJpeg;            /* jpeg image object */
struct jpeg_dimensions  j_image;
BITMAPINFO              j_bmpinfo;
struct jpeg_datasource  j_datasource;

/* For data source read pointer */
unsigned char  * j_restart_ptr;
unsigned char  * j_read_ptr;
int	           j_data_to_skip;

#define SRCPIXEL_INCREASE_24(p1) {\
    p1 += 3;    \
}
#define SRCPIXEL_INCREASE_32(p1) {\
    p1 += 4;    \
}
#define EXCHANGE(a, b) {int t; t = (a); (a) = (b); (b) = t;}

static void GetStretchPattern(int nDstWidth, int nDstHeight, int nSrcWidht,
							  int nSrcHeight, PSTRETCHPATTERN p);
static void GetStretchPatternLine(int nStep, int nShort, BYTE* p);
extern int Jpeg_Reset(PJPEGOBJ pObject);
extern void ConvertImageData(DWORD dwData, BYTE* pBaseAddress, int hFile);


/*********************************************************************\
* Function	 :  JPEG_DS_GetData
* Purpose    :
*     initialize data source of jpeg decoder struct
* Params	 :
*     pGif   : jpeg object need to be initialized
**********************************************************************/
void JPEG_DS_GetData( PJPEGOBJ pJpeg)
{
	return;
}
/*********************************************************************\
* Function	 :  JPEG_DS_InitSource
* Purpose    :
*     initialize data source of jpeg decoder struct
* Params	 :
*     pGif   : jpeg object need to be initialized
**********************************************************************/
void JPEG_DS_InitSource( PJPEGOBJ pJpeg)
{
    DWORD       dwFileSize;
    void        *pData;

    pData = JPEG_GetUserData(pJpeg);

    dwFileSize = GET32(((BYTE *)pJpeg) + JPEG_GetObjectSize());

    j_datasource.next_input_byte = pData;

	j_datasource.bytes_in_buffer = dwFileSize;

    return;
}

/*********************************************************************\
* Function	   : JPEG_DS_SkipData
* Purpose      :
*     skip some datum from jpeg data source  
* Params	   :
*     pJpeg     : the jpeg object
*     num_bytes: number to be skipped
**********************************************************************/
void JPEG_DS_SkipData( PJPEGOBJ pJpeg, int num_bytes)
{
	if ( num_bytes <= j_datasource.bytes_in_buffer)
	{	/* Skip bytes less than bytes in buffer */
		j_datasource.next_input_byte += num_bytes;
		j_datasource.bytes_in_buffer -= num_bytes;
		j_restart_ptr = j_read_ptr = j_datasource.next_input_byte; 

        j_data_to_skip = 0;
		return;
	}

	/* Set bytes to skip on fill_input_buffer */
	j_data_to_skip = num_bytes - j_datasource.bytes_in_buffer;
	/* Set read pointer be the last consumed byte */
	j_read_ptr = j_datasource.next_input_byte + j_datasource.bytes_in_buffer;
	/* Set bytes in buffer = 0, next the library must call fill_input_buffer */
	j_datasource.bytes_in_buffer = 0;
}

/*********************************************************************\
* Function	   : JPEG_DS_TermSource
* Purpose      :
*     terminate data source of Jpeg struct  
* Params	   :
*     pGif     : the Jpeg object
**********************************************************************/
void JPEG_DS_TermSource( PJPEGOBJ pJpeg)
{
   return;
}

#define	BLOCK_LINES		20

/**************************************************************************/
/*              Get Jpeg  dimension                                       */
/**************************************************************************/

/*********************************************************************\
* Function	     : GetJpegDimensionFromData
* Purpose        :
*     get jpeg image dimension from data
* Params	     :
*     pData      : data struct related to jpeg image 
*     pSize      : address of dimension struct
* Return	     :
*     if function success, return TRUE
*     otherwise return FALSE	   
**********************************************************************/
BOOL    IMAGEAPI GetJpegDimensionFromData(struct data_buffer pData, PSIZE pSize)
{
    struct jpeg_dimensions    imagedim;
    int     ret;

    j_data_to_skip	= 0;
	// Make data source.
	j_datasource.next_input_byte	= NULL;
	j_datasource.bytes_in_buffer	= 0;

	j_datasource.init_source		= JPEG_DS_InitSource;
	j_datasource.fill_input_buffer	= JPEG_DS_GetData;
	j_datasource.skip_input_data	= JPEG_DS_SkipData;
	j_datasource.resync_to_restart	= NULL;
	j_datasource.term_source		= JPEG_DS_TermSource;

    /* malloc and construct jpeg object */
    ret = JPEG_GetObjectSize();
    j_pJpeg = (char *) malloc (sizeof(char) * ret + sizeof(DWORD) + sizeof(int));
    ConvertImageData(pData.data_count, ((BYTE *)j_pJpeg) + ret, 0);
    JPEG_Construct( j_pJpeg);

    /* set user data and data source */
    JPEG_SetUserData( j_pJpeg, pData.data);
    JPEG_SetDataSource( j_pJpeg, &j_datasource);


    /* read head of jpeg */
    ret = JPEG_ReadHeader( j_pJpeg);
    if ( ret <= 0)
        return FALSE;

    /* get jpeg dimension */
    JPEG_GetImageDimensions( j_pJpeg, &imagedim);
    pSize->cx = imagedim.image_width;
    pSize->cy = imagedim.image_height;

    /* destruct and free jpeg object, close file */
    JPEG_Destruct( j_pJpeg);
    free(j_pJpeg);

    return TRUE;
}

/*********************************************************************\
* Function	   : GetJpegDimension
* Purpose      :
*     get jpeg image dimension from image handle
* Params	   :
*     hImage   : handle related to jpeg image 
*     pSize    : address of dimension struct
* Return	   :
*     if function success, return TRUE
*     otherwise return FALSE	   
**********************************************************************/
BOOL GetJpegDimension(PPHYIMG pPhyImg, PSIZE pSize)
{
    struct jpeg_dimensions    imagedim;
    PJPEGOBJ                  pJpegObj;
    int      ret;

    pJpegObj = (PJPEGOBJ)pPhyImg;

    Jpeg_Reset(pJpegObj);
    /* read head of jpeg */
    ret = JPEG_ReadHeader( pJpegObj);
    if ( ret <= 0 )
        return FALSE;

    /* get jpeg dimension */
    JPEG_GetImageDimensions(pJpegObj, &imagedim);

    pSize->cx = imagedim.image_width;
    pSize->cy = imagedim.image_height;

    return TRUE;
}


/**************************************************************************/
/*              Create Jpeg                                               */
/**************************************************************************/

/*********************************************************************\
* Function	   : CreateJpegFromData
* Purpose      :
*     Create jpeg image from file data
* Params	   :
*     pData    : data struct to create jpeg image
*     hFile    : file handle for creating jpeg image
* Return	   :
*     return jpeg image handle        
**********************************************************************/
PPHYIMG IMAGEAPI CreateJpegFromData(struct data_buffer pData, int file)
{
    int	               ret;
    PJPEGOBJ           pJpeg;	

	j_data_to_skip	= 0;
	// Make data source.
	j_datasource.next_input_byte	= NULL;
	j_datasource.bytes_in_buffer	= 0;

	j_datasource.init_source		= JPEG_DS_InitSource;
	j_datasource.fill_input_buffer	= JPEG_DS_GetData;
	j_datasource.skip_input_data	= JPEG_DS_SkipData;
	j_datasource.resync_to_restart	= NULL;
	j_datasource.term_source		= JPEG_DS_TermSource;

    /* malloc and construct jpeg object */
    ret = JPEG_GetObjectSize();
	pJpeg = (char *) malloc (sizeof(char) * ret + sizeof(DWORD) + sizeof(int));
	if (!pJpeg)
		return NULL;
    ConvertImageData(pData.data_count, ((BYTE *) pJpeg) + ret, file);

	JPEG_Construct((void*)pJpeg);

	/* Set user data and data source */
	JPEG_SetUserData(pJpeg, pData.data);
	JPEG_SetDataSource(pJpeg, &j_datasource);
	
    return pJpeg;
}

BOOL DeleteJpeg(PPHYIMG pImg, BOOL bMapFile)
{
	BYTE    *pData = (BYTE*)pImg;
	int     hFile;
     
    hFile = *(int*)(pData + JPEG_GetObjectSize() + sizeof(DWORD));
    JPEG_Destruct((PJPEGOBJ)pData);
	if (hFile)
	{
		DWORD dwFileSize;
		void  *pMapFile;

		pMapFile = JPEG_GetUserData(pData);
	    if (bMapFile)
		{        	 
		    dwFileSize = IMGOS_GetFileSize(hFile);			 
		    IMGOS_UnmapFile(hFile, pMapFile, dwFileSize); 
		}
		else
			free(pMapFile);
	 
        IMGOS_CloseFile(hFile);
	 }    
	free(pData);
    return TRUE;
}
/*********************************************************************\
* Function	   : CreateBitmapFromJpeg
* Purpose      :
*     Create bitmap from jpeg
* Params	   :
*     pData    : data struct for create bitmap 
* Return	   :
*     if function success, return bitmap handle	   
*     otherwise return NULL
**********************************************************************/
HBITMAP CreateBitmapFromJpeg(struct data_buffer pData)
{
    int	      i, count, retval, line_w, y_start, scanlines, nline, iTmp;
    char      * ptr, * pImgBuffer, *pTmp, *pTmp1, *pSrc;
    char      * LineTable[BLOCK_LINES];
    PJPEGOBJ  pJpeg;
    struct jpeg_dimensions jpeg_dem;
    HBITMAP   hBitmap;
    HDC       hdcMem;

    /* create jpeg object */
    pJpeg = (PJPEGOBJ)CreateJpegFromData(pData, 0);
	if (!pJpeg)
		return NULL;

    /* read head of jpeg */
    retval = JPEG_ReadHeader(pJpeg);
    if (retval <= 0 )
    {
        JPEG_Destruct(pJpeg);
        free(pJpeg);
        return NULL;
    }
    /* Get image dimensions */
	JPEG_GetImageDimensions(pJpeg, &jpeg_dem);

	/* If this is progressive image, set buffered image mode */
	if ( jpeg_dem.flags & JPEG_PROGRESSIVE)
		JPEG_SetBufferedMode(pJpeg, TRUE);

    /* start decompress */
    retval = JPEG_StartDecompress(pJpeg);
    if (retval < 0)
    {
        JPEG_Destruct(pJpeg);
        free(pJpeg);
        return NULL;
    }
    
    hdcMem = CreateCompatibleDC(NULL);
    if (!hdcMem)
    {
        JPEG_Destruct(pJpeg);
        free(pJpeg);
        return NULL;
    }
    hBitmap = CreateCompatibleBitmap(hdcMem,
                 jpeg_dem.image_width,
                 jpeg_dem.image_height);
    if (!hBitmap)
    {
        JPEG_Destruct(pJpeg);
        free(pJpeg);
        DeleteDC(hdcMem);
        return NULL;
    }
    SelectObject(hdcMem, hBitmap);

StartOutput:
    if ( jpeg_dem.flags & JPEG_PROGRESSIVE)
    {
        do {
	     	    retval = JPEG_ConsumeInput (pJpeg);
            }while ( retval != JPEG_REACHED_EOI && retval != JPEG_SUSPENDED);

		if ( retval >= 0)
		{
			retval = JPEG_GetInputScanNumber(pJpeg);
			JPEG_StartOutput(pJpeg, retval);
        }
    }
   /* get output scan line */
	y_start = JPEG_GetOutputScanline(pJpeg);

    /* calculate bits every line */
	if (jpeg_dem.num_components == 4)
	    line_w = jpeg_dem.image_width * jpeg_dem.num_components;
    else
        line_w = (jpeg_dem.image_width * 3 + 3) & ~3;
	scanlines = jpeg_dem.image_height;

    /* malloc and initialize buffer to save bitmap color */
	pImgBuffer = ptr = (char *) malloc (line_w * BLOCK_LINES);
	if ( ptr == NULL)
    {
        JPEG_Destruct(pJpeg);
        free(pJpeg);
        return NULL;
    }
    pTmp = malloc (line_w * BLOCK_LINES);
    if (pTmp == NULL)        
    {
        JPEG_Destruct(pJpeg);
        free(ptr);
        free(pJpeg);
        return NULL;
    }
    pTmp1 = pTmp;
	memset( ptr, 0, line_w * BLOCK_LINES);
    memset(pTmp, 0, line_w * BLOCK_LINES);
    pSrc = pImgBuffer;

	/* Make line table */
	for ( i = 0; i < BLOCK_LINES; i ++, ptr += line_w)
		LineTable[i] = ptr;

    while ( scanlines > 0)
    {	/* Try to paint */
        count = min(scanlines, BLOCK_LINES);
        
        retval = JPEG_ReadScanlines(pJpeg, LineTable, count);
        
        pSrc = pImgBuffer;
        if(jpeg_dem.color_mode == JCS_GRAYSCALE)
        {
            memcpy(pTmp, pImgBuffer, line_w * count);
            
            nline = 0;
            while (nline < count)
            {
                iTmp = line_w;
                while (iTmp > 0)
                {
                    *pImgBuffer = *pTmp;
                    *(pImgBuffer + 1)= *pTmp;
                    *(pImgBuffer + 2)= *pTmp;
                    pImgBuffer += 3;
                    pTmp ++;
                    iTmp -= 3;
                }
                pTmp =pTmp1 + line_w * (nline + 1);
                pImgBuffer = pSrc + line_w* (nline + 1);
                nline ++;
            }
            pTmp = pTmp1;
            pImgBuffer = pSrc;
        }
        if ( retval <= 0)        
           break;        
        
        /* fill j_bmpinfo.bmiHeader struct */
        if ( y_start == 0)
        {
            j_bmpinfo.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
            j_bmpinfo.bmiHeader.biWidth			= jpeg_dem.image_width;
            j_bmpinfo.bmiHeader.biHeight		= -retval;
            j_bmpinfo.bmiHeader.biPlanes		= 1;
            if (jpeg_dem.num_components == 4)
                j_bmpinfo.bmiHeader.biBitCount = 32;
            else
                j_bmpinfo.bmiHeader.biBitCount = 24;
            j_bmpinfo.bmiHeader.biCompression	= BI_RGB;
            j_bmpinfo.bmiHeader.biSizeImage		= 0;
            j_bmpinfo.bmiHeader.biXPelsPerMeter = 0;
            j_bmpinfo.bmiHeader.biYPelsPerMeter = 0;
            j_bmpinfo.bmiHeader.biClrUsed		= 0;
            j_bmpinfo.bmiHeader.biClrImportant	= 0;	
        }
                
        SetDIBitsToDevice(hdcMem, 0, y_start, jpeg_dem.image_width,
        		retval, 0, 0, 0, retval, pImgBuffer, &j_bmpinfo, 0);      
        
        y_start += retval;
        scanlines -= retval;
#ifndef _EMULATE_
				KickDog();
#endif //_EMULATE_
    }    
   	  
    if ( jpeg_dem.flags & JPEG_PROGRESSIVE)
    {
	    retval = JPEG_FinishOutput(pJpeg);
		// If last scan displied, no input data sction.
		if ( j_last_scan != 0)
		{
			retval = JPEG_FinishDecompress(pJpeg);  
			free (pImgBuffer); 
            free (pTmp);
			JPEG_Destruct(pJpeg);   
			free(pJpeg);
			DeleteDC(hdcMem);
			return hBitmap;
		}

		// try to display next scan.
		if ( retval > 0)
		{
			if ( JPEG_InputComplete(pJpeg))
				j_last_scan = TRUE;		
            free (pImgBuffer); 
            free(pTmp);
			goto StartOutput;
		}
	}
    
    free (pImgBuffer);
    free(pTmp);
    JPEG_Destruct(pJpeg);
    free(pJpeg);
    DeleteDC(hdcMem);

    return hBitmap;
    
}

/*********************************************************************\
* Function	   : PaintJpeg
* Purpose      :
*     paint jpeg image
* Params	   :
*     hdc      : handle for device context
*     nDestX   : x-coordination of destination rectangle of top-left corner
*     nDestY   : y-coordination of destination rectangle of top-left corner
*     nWidth   : width of rectangle to paint gif 
*     nHeight  : height of rectangle to paint gif
*     hImage   : handle of gif image to paint
*     nSrcX    : x-coordination of source rectangle of top-left corner
*     nSrcY    : y-coordination of source rectangle of top-left corner
*     dwRop    : raster operation code
* Return	   :
*     if function success, return 1;
*     otherwise return 0
**********************************************************************/
int PaintJpeg(HDC hdc, int nDestX, int nDestY, int nWidth, int nHeight,
              PPHYIMG pPhyImg, int nSrcX, int nSrcY, DWORD dwRop)
{
    int	i, count, retval, line_w, y_start, scanlines, nline, iTmp;
    char                   * ptr, * pImgBuffer, *pTmp, *pTmp1, *pSrc;
    char                   *LineTable[BLOCK_LINES];
    PJPEGOBJ               pJpeg; 
    struct jpeg_dimensions jpeg_dem;
    HDC      hdcMem;
    HBITMAP  hBitmap;
	
	pJpeg = (PJPEGOBJ)pPhyImg;

    /* reset data source to sure correct data source */
    j_datasource.resync_to_restart = NULL;
    Jpeg_Reset(pJpeg);
    JPEG_SetDataSource(pJpeg, &j_datasource);

    /* read head of jpeg */
    retval = JPEG_ReadHeader(pJpeg);
    if (retval < 0)
        return 0;

    // Get image dimensions.
	JPEG_GetImageDimensions(pJpeg, &jpeg_dem);
	// If this is progressive image, set buffered image mode.
	if ( jpeg_dem.flags & JPEG_PROGRESSIVE)
		JPEG_SetBufferedMode(pJpeg, TRUE);

    retval = JPEG_StartDecompress(pJpeg);
    if (retval < 0)
       return 0;

StartOutput:
    if ( jpeg_dem.flags & JPEG_PROGRESSIVE)
    {
        do {
	     	    retval = JPEG_ConsumeInput (pJpeg);
            }while ( retval != JPEG_REACHED_EOI && retval != JPEG_SUSPENDED && retval >= 0);

		if ( retval >= 0)
		{
			retval = JPEG_GetInputScanNumber(pJpeg);
			JPEG_StartOutput(pJpeg, retval);
        }
		else
			return 0;
    }

	y_start = JPEG_GetOutputScanline(pJpeg);

	if (jpeg_dem.num_components == 4)
	    line_w = jpeg_dem.image_width * jpeg_dem.num_components;
    else
        line_w = (jpeg_dem.image_width * 3 + 3) & ~3;
	scanlines = jpeg_dem.image_height;
     
    pImgBuffer = ptr = (char *) malloc (line_w * BLOCK_LINES);
	if ( ptr == NULL)
		return 0;

    pTmp = malloc (line_w * BLOCK_LINES);
    if (pTmp == NULL)
    {
        free(pImgBuffer);
        return 0;
    }
    pTmp1 = pTmp;
	memset(pTmp, 0, line_w * BLOCK_LINES);
    
	memset( ptr, 0, line_w * BLOCK_LINES);
	/* Make line table */
	for ( i = 0; i < BLOCK_LINES; i ++, ptr += line_w)
		LineTable[i] = ptr;

    hdcMem = CreateCompatibleDC(hdc);
    if (!hdcMem)
    {
        free (pImgBuffer); 
        free(pTmp);
        return 0;
    }
	hBitmap = CreateCompatibleBitmap(hdcMem, jpeg_dem.image_width,
              jpeg_dem.image_height);
    if (!hBitmap)
    {
        free (pImgBuffer); 
        free(pTmp);
        DeleteDC(hdcMem);
        return 0;
    }
	SelectObject(hdcMem, hBitmap);
	
	while ( scanlines > 0)
	{	/* Try to paint */
		count = min(scanlines, BLOCK_LINES);

		retval = JPEG_ReadScanlines(pJpeg, LineTable, count);
        pSrc = pImgBuffer;
        if(jpeg_dem.color_mode == JCS_GRAYSCALE)
        {
            memcpy(pTmp, pImgBuffer, line_w * count);
            
            nline = 0;
            while (nline < count)
            {
                iTmp = line_w;
                while (iTmp > 0)
                {
                    *pImgBuffer = *pTmp;
                    *(pImgBuffer + 1)= *pTmp;
                    *(pImgBuffer + 2)= *pTmp;
                    pImgBuffer += 3;
                    pTmp ++;
                    iTmp -= 3;
                }
                pTmp =pTmp1 + line_w * (nline + 1);
                pImgBuffer = pSrc + line_w* (nline + 1);
                nline ++;
            }
            pTmp = pTmp1;
            pImgBuffer = pSrc;
        }
		if ( retval <= 0)
			break;

		if ( y_start == 0)
		{
			j_bmpinfo.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
			j_bmpinfo.bmiHeader.biWidth			= jpeg_dem.image_width;
			j_bmpinfo.bmiHeader.biHeight		= -retval;
			j_bmpinfo.bmiHeader.biPlanes		= 1;
			if (jpeg_dem.num_components == 4)
                j_bmpinfo.bmiHeader.biBitCount = 32;
            else
                j_bmpinfo.bmiHeader.biBitCount = 24;
			j_bmpinfo.bmiHeader.biCompression	= BI_RGB;
			j_bmpinfo.bmiHeader.biSizeImage		= 0;
			j_bmpinfo.bmiHeader.biXPelsPerMeter = 0;
			j_bmpinfo.bmiHeader.biYPelsPerMeter = 0;
			j_bmpinfo.bmiHeader.biClrUsed		= 0;
			j_bmpinfo.bmiHeader.biClrImportant	= 0;

		}
		SetDIBitsToDevice(hdcMem, 0, y_start, jpeg_dem.image_width,
			retval, 0, 0, 0, retval, pImgBuffer, &j_bmpinfo, 0); 
	    
		y_start += retval;
		scanlines -= retval;
#ifndef _EMULATE_
				KickDog();
#endif //_EMULATE_
	}

    StretchBlt(hdc, nDestX, nDestY, nWidth, nHeight, (HDC)hBitmap, 
		nSrcX, nSrcY, jpeg_dem.image_width - nSrcX,
		jpeg_dem.image_height - nSrcY, ROP_SRC);	

	if ( jpeg_dem.flags & JPEG_PROGRESSIVE)
    {
	    retval = JPEG_FinishOutput(pJpeg);
		// If last scan displied, no input data sction.
		if ( j_last_scan != 0)
		{
			retval = JPEG_FinishDecompress(pJpeg);  
			free (pImgBuffer); 
            free(pTmp);
			DeleteDC(hdcMem);
			DeleteObject(hBitmap);    
            return 1;
		}

		// try to display next scan.
		if ( retval > 0)
		{
			if ( JPEG_InputComplete(pJpeg))
				j_last_scan = TRUE;		
            free (pImgBuffer); 
            free(pTmp);
			goto StartOutput;
		}
	}
	free (pImgBuffer); 
    free(pTmp);
	DeleteDC(hdcMem);
	DeleteObject(hBitmap);
	
	return 1;
}

/*********************************************************************\
* Function	   : PaintJpegImage
* Purpose      :
*     paint jpeg image
* Params	   :
*     hdc      : handle for device context
*     x        : x-coordination of destination rectangle of top-left corner
*     y        : y-coordination of destination rectangle of top-left corner
*     dwRop    : raster operation code
* Return	   :
*     if function success, return 1;
*     otherwise return 0	 	      
**********************************************************************/
int PaintJpegImage( HDC hdc, int x, int y, int width, int height,
                   DWORD dwRop)
{
    int	i, count, retval, Src_line_w, y_start, scanlines, nSrcHeight, Dst_line_w, nwidth, iTmp, nline;
    char * ptr, * pImgBuffer, *pTmp, *pTmp1;
    char * LineTable[BLOCK_LINES];
    HBITMAP hBitmap;
    HDC     hdcMem;
	BOOL    bStretch;
	STRETCHPATTERN	StretchPattern;
	BYTE	*pXPattern, *pYPattern, *pX, *pSrc, *p1, *p2, *pDstBuffer, *pDst;
	BITMAPINFO          bmpinfo;     

	if (width == 0 && height == 0)
		bStretch = FALSE;
	else 
		bStretch = TRUE;

	y_start = JPEG_GetOutputScanline( j_pJpeg);
    if (j_image.num_components == 4)
	    Src_line_w = j_image.image_width * j_image.num_components;
    else
        Src_line_w = (j_image.image_width  * 3 + 3) & ~3;
	scanlines = j_image.image_height;

	pImgBuffer = ptr = (char *) malloc (Src_line_w* BLOCK_LINES);
    if ( ptr == NULL)
		return 0;
    pTmp = malloc (Src_line_w * BLOCK_LINES);
    if (pTmp == NULL)
    {
        free(ptr);
        return 0;
    }
    pTmp1 = pTmp;
	pSrc = pImgBuffer;
	

	memset( ptr, 0, Src_line_w * BLOCK_LINES);

	/* Make line table */
	for ( i = 0; i < BLOCK_LINES; i ++, ptr += Src_line_w)
		LineTable[i] = ptr;
 
    if (bStretch && (width > j_image.image_width || height > j_image.image_height))
    {
        hdcMem = CreateCompatibleDC(hdc);
        if (!hdcMem)
        {
            free (pImgBuffer);
            free (pTmp);
            return 0;
        }
        hBitmap = CreateCompatibleBitmap(hdcMem, j_image.image_width, 
            j_image.image_height);
        if (!hBitmap)
        {
            DeleteDC(hdcMem);
            free (pImgBuffer);
            free (pTmp);
            return 0;
        }
        SelectObject(hdcMem, hBitmap);
    }
	else if (bStretch)
	{
        StretchPattern.pbWidthPattern = (BYTE*)malloc(j_image.image_width);
        StretchPattern.pbHeightPattern = (BYTE*)malloc(j_image.image_height);
		GetStretchPattern(width, height, j_image.image_width, j_image.image_height,
			&StretchPattern);
		pXPattern = StretchPattern.pbWidthPattern;
		pYPattern = StretchPattern.pbHeightPattern;
		
		if (j_image.num_components == 4)
            pDst = pDstBuffer = (BYTE*)malloc(((width*j_image.num_components + 3)&~3) * height);
        else
            pDst = pDstBuffer = (BYTE*)malloc(((width*3 + 3)&~3) * height);
		if (!pDstBuffer)
		{
            free (pImgBuffer);
            free (pTmp);
            free (StretchPattern.pbWidthPattern);
            free (StretchPattern.pbHeightPattern);
            return 0;
        }
		
		if (j_image.num_components == 4)
            Dst_line_w = (width * j_image.num_components + 3) & ~3;
        else
            Dst_line_w = (width * 3 + 3) & ~3;
	}


    while ( scanlines > 0)
	{	/* Try to paint */
		count = min(scanlines, BLOCK_LINES);

        retval = JPEG_ReadScanlines( j_pJpeg, LineTable, count);

        pSrc = pImgBuffer;
        if(j_image.color_mode == JCS_GRAYSCALE)
        {
            
            
            memcpy(pTmp, pImgBuffer, Src_line_w * count);
            
            nline = 0;
            while (nline < count)
            {
                iTmp = Src_line_w;
                while (iTmp > 0)
                {
                    *pImgBuffer = *pTmp;
                    *(pImgBuffer + 1)= *pTmp;
                    *(pImgBuffer + 2)= *pTmp;
                    pImgBuffer += 3;
                    pTmp ++;
                    iTmp -= 3;
                }
                pTmp =pTmp1 + Src_line_w * (nline + 1);
                pImgBuffer = pSrc + Src_line_w* (nline + 1);
                nline ++;
            }
            pTmp = pTmp1;
            pImgBuffer = pSrc;
        }
        
		if ( retval <= 0)
			break;

		if ( y_start == 0)
		{
			j_bmpinfo.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
			j_bmpinfo.bmiHeader.biWidth			= j_image.image_width;
			j_bmpinfo.bmiHeader.biHeight		= -retval;
			j_bmpinfo.bmiHeader.biPlanes		= 1;
            if (j_image.num_components == 4)
                j_bmpinfo.bmiHeader.biBitCount		= 32;
            else
			    j_bmpinfo.bmiHeader.biBitCount		= 24;
			j_bmpinfo.bmiHeader.biCompression	= BI_RGB;
			j_bmpinfo.bmiHeader.biSizeImage		= 0;
			j_bmpinfo.bmiHeader.biXPelsPerMeter = 0;
			j_bmpinfo.bmiHeader.biYPelsPerMeter = 0;
			j_bmpinfo.bmiHeader.biClrUsed		= 0;
			j_bmpinfo.bmiHeader.biClrImportant	= 0;

		}
        if (bStretch && (width > j_image.image_width || height > j_image.image_height))
            SetDIBitsToDevice(hdcMem, 0, y_start, j_image.image_width,
                retval, 0, 0, 0, retval, pImgBuffer, &j_bmpinfo, 0);      
		else if (bStretch)
		{
			nSrcHeight = 0;
			while(nSrcHeight < retval)
			{
				while (*pYPattern++ == 0)
				{
					pSrc += Src_line_w;
					nSrcHeight ++;
					if (nSrcHeight == retval)
						break;
				}
				if (nSrcHeight == retval)
					break;
				
				pX = pXPattern;
				p1 = pSrc;
				p2 = pDstBuffer;
				nwidth = j_image.image_width;
				while(nwidth--)
				{
					while (*pX++ == 0)
					{
                        if (j_image.num_components == 4)
                        {
                            SRCPIXEL_INCREASE_32(p1);
                        }
                        else
                        {
						    SRCPIXEL_INCREASE_24(p1);
                        }
						nwidth --;
					}
					if (nwidth < 0)
						break;
                    if (j_image.num_components == 4)
                    {
                        *(p2 + 3) = *(p1 + 3);
                        *(p2 + 2) = *(p1 + 2);
                        *(p2 + 1) = *(p1 + 1);
                        *p2 = *p1;
                        SRCPIXEL_INCREASE_32(p1);
                        SRCPIXEL_INCREASE_32(p2);
                    }
                    else
                    {
                        *(p2 + 2) = *(p1 + 2);
                        *(p2 + 1) = *(p1 + 1);
                        *p2 = *p1;
                        SRCPIXEL_INCREASE_24(p1);
                        SRCPIXEL_INCREASE_24(p2);
                    }
					
					
				}
				
				pDstBuffer += Dst_line_w;
				pSrc += Src_line_w;
				nSrcHeight ++;
			}
		
            
		}
        else
        {
            if (width == 0 && height == 0)
                SetDIBitsToDevice(hdc, x, y + y_start, j_image.image_width,
                    retval, 0, 0, 0, retval, pImgBuffer, &j_bmpinfo, 0);  
            else
                SetDIBitsToDevice(hdc, x, y + y_start, width,
                    retval, 0, 0, 0, retval, pImgBuffer, &j_bmpinfo, 0);  
        }
                
		y_start += retval;
		scanlines -= retval;
#ifndef _EMULATE_
				KickDog();
#endif //_EMULATE_
	}


    if (bStretch && (width > j_image.image_width || height > j_image.image_height))
    {
        StretchBlt(hdc, x, y, width, height, (HDC)hBitmap,
            0, 0, j_image.image_width, j_image.image_height, ROP_SRC);
        DeleteObject((HGDIOBJ)hBitmap);
        DeleteDC(hdcMem);
    }
	else if(bStretch)
	{
		bmpinfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
		bmpinfo.bmiHeader.biWidth		= width;
		bmpinfo.bmiHeader.biHeight	= -height;
		bmpinfo.bmiHeader.biPlanes	= 1;
        if (j_image.num_components == 4)
            bmpinfo.bmiHeader.biBitCount = 32;
        else
            bmpinfo.bmiHeader.biBitCount = 24;
		bmpinfo.bmiHeader.biCompression	= BI_RGB;
		bmpinfo.bmiHeader.biSizeImage		= 0;
		bmpinfo.bmiHeader.biXPelsPerMeter = 0;
		bmpinfo.bmiHeader.biYPelsPerMeter = 0;
		bmpinfo.bmiHeader.biClrUsed		= 0;
		bmpinfo.bmiHeader.biClrImportant	= 0;
		
        hBitmap = CreateDIBitmap(NULL, &bmpinfo.bmiHeader, CBM_INIT, 
			pDst, &bmpinfo, DIB_RGB_COLORS);
        if (!hBitmap)
        {
            free (pDst);
			free (pImgBuffer);
            free (pTmp);
            free (StretchPattern.pbWidthPattern);
            free (StretchPattern.pbHeightPattern);
            return 0;
        }
		BitBlt(hdc, x, y, width, height, (HDC)hBitmap, 0, 0, SRCCOPY);
		DeleteObject((HGDIOBJ)hBitmap);
        free(StretchPattern.pbWidthPattern);
        free(StretchPattern.pbHeightPattern);
		free (pDst);
			
	}
	free (pImgBuffer);
    free (pTmp);
	return 1;
}

/*********************************************************************\
* Function	   GetStretchPatternLine
* Purpose      
    得到Stretch的pattern。按照步长计算，每次短边需要加一个象素时，
    pattern中计1，不变时计0 * Params	   
* Return	 	   
* Remarks
**********************************************************************/
static void GetStretchPatternLine(int nStep, int nShort, BYTE* p)
{
    int i;
    int x1 = 0, x2 = 0;

    for (i = 0; i < nStep; i ++)
    {
        x1 += nShort;
        if (x1 >= nStep)
        {
            *p++ = 1;
            x1 -= nStep;
        }
        else
            *p++ = 0;
    }
    if (nShort)
        ASSERT(*(--p) == 1);

    return;
}

/*********************************************************************\
* Function	   GetStretchPattern
* Purpose      
    得到X和Y向的stretch pattern
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void GetStretchPattern(int nDstWidth, int nDstHeight, int nSrcWidth,
							  int nSrcHeight, PSTRETCHPATTERN p)
{
    int nStep;
    int nShort;

    nStep = nDstWidth;
    nShort = nSrcWidth;
    if (nStep < nShort)
        EXCHANGE(nShort, nStep);

    GetStretchPatternLine(nStep, nShort, p->pbWidthPattern);

    nStep = nDstHeight;
    nShort = nSrcHeight;
    if (nStep < nShort)
        EXCHANGE(nShort, nStep);
    GetStretchPatternLine(nStep, nShort, p->pbHeightPattern);

    return;
}

/*********************************************************************\
* Function	   : DrawJpeg
* Purpose      :
*     Draw jpeg image
* Params	   :
*     hdc      : handle for device context
*     x        : x-coordination of destination rectangle of top-left corner
*     y        : y-coordination of destination rectangle of top-left corner
*     pData    : data struct to draw jpeg image
*     dwRop    : raster operation code	      
**********************************************************************/
BOOL DrawJpeg( HDC hdc, int x, int y, int width, int height, 
              struct data_buffer pData, DWORD dwRop)
{
    int	                ret;

	j_data_to_skip	= 0;

	// Make data source.
	j_datasource.next_input_byte	= NULL;
	j_datasource.bytes_in_buffer	= 0;

	j_datasource.init_source		= JPEG_DS_InitSource;
	j_datasource.fill_input_buffer	= JPEG_DS_GetData;
	j_datasource.skip_input_data	= JPEG_DS_SkipData;
	j_datasource.resync_to_restart	= NULL;
	j_datasource.term_source		= JPEG_DS_TermSource;

	j_decode_state	= J_S_INIT;
	j_last_scan		= 0;
	j_image.image_width = j_image.image_height = 0;


next_state:

	switch ( j_decode_state)
	{
	case J_S_INIT:
		// Create one JPEG debug object.
		ret = JPEG_GetObjectSize();
		j_pJpeg = (char *) malloc (sizeof(char) * ret + sizeof(DWORD) + sizeof(int));
        ConvertImageData(pData.data_count, ((BYTE *) j_pJpeg) + ret, 0);
		JPEG_Construct( j_pJpeg);
		/* Set data source */
		JPEG_SetUserData( j_pJpeg, pData.data);
		JPEG_SetDataSource( j_pJpeg, &j_datasource);
		j_decode_state = J_S_READHEAD;
		goto next_state;

	case J_S_READHEAD:
		// Read in jpeg header.
		ret = JPEG_ReadHeader( j_pJpeg);
		if ( ret > 0)
		{	// Get image dimensions.
			JPEG_GetImageDimensions( j_pJpeg, &j_image);

			// If this is progressive image, set buffered image mode.
			if ( j_image.flags & JPEG_PROGRESSIVE)
				JPEG_SetBufferedMode( j_pJpeg, TRUE);

			j_decode_state = J_S_STARTDECOMPRESS;
			goto next_state;
		}

		if ( ret <= 0 )
			j_decode_state = J_S_ERROR;
		break;

	case J_S_STARTDECOMPRESS:
		// Do start decompress
		ret = JPEG_StartDecompress( j_pJpeg);
		if ( ret > 0)
		{	// If this is progressive image, call start output.
			if ( j_image.flags & JPEG_PROGRESSIVE)
				j_decode_state  = J_S_STARTOUTPUT;
			else
				j_decode_state  = J_S_READSCANLINES;
			goto next_state;
        }

		if ( ret < 0)
			j_decode_state = J_S_ERROR;
		break;

	case J_S_STARTOUTPUT:
		// Try to consume input data.
        do {
	     	ret = JPEG_ConsumeInput ( j_pJpeg);
        }while ( ret != JPEG_REACHED_EOI && ret != JPEG_SUSPENDED);
		
		if ( ret >= 0)
		{
			ret = JPEG_GetInputScanNumber( j_pJpeg);
			JPEG_StartOutput( j_pJpeg, ret);
			j_decode_state  = J_S_READSCANLINES;
			goto next_state;
		}

		j_decode_state = J_S_ERROR;
		break;

	case J_S_READSCANLINES:
		/* Write new image line */
		ret = PaintJpegImage( hdc, x, y, width, height, dwRop);
        
		if ( ret > 0)
		{	/* If progressive image, finish output */
			if ( JPEG_GetOutputScanline( j_pJpeg) < j_image.image_height)
				return TRUE;

			if (!(j_image.flags & JPEG_PROGRESSIVE))
				break;
			j_decode_state = J_S_FINISHOUTPUT;
			goto next_state;
		}
		else
        {
            JPEG_Destruct( j_pJpeg);
            free(j_pJpeg);
            
            return FALSE;
        }
		break;

	case J_S_FINISHOUTPUT:
		// Do finish output.
		ret = JPEG_FinishOutput( j_pJpeg);
		// If last scan displied, no input data sction.
		if ( j_last_scan != 0)
		{
			j_decode_state = J_S_FINISHDECOMPRESS;
			goto next_state;
		}

		// try to display next scan.
		if ( ret > 0)
		{
			if ( JPEG_InputComplete( j_pJpeg))
				j_last_scan = TRUE;
			j_decode_state = J_S_STARTOUTPUT;
			goto next_state;
		}
		else
        {
            JPEG_Destruct( j_pJpeg);
            free(j_pJpeg);
            
            return FALSE;
        }
		if ( ret < 0)
			j_decode_state = J_S_ERROR;
		break;

	case J_S_FINISHDECOMPRESS:
		/* Finish decompress */
		ret = JPEG_FinishDecompress( j_pJpeg);
		if ( ret > 0)
			j_decode_state = J_S_ENDDECODE;
		break;
	}

    JPEG_Destruct( j_pJpeg);
    free(j_pJpeg);

    return TRUE;
}



struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void) my_error_exit (j_common_ptr cinfo);

//
//	to handle fatal errors.
//	the original JPEG code will just exit(0). can't really
//	do that in Windows....
//

METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	char buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	/* Return control to the setjmp point */
}

BOOL RGBToJpegFile(char* fileName, 
							BYTE *dataBuf,
							UINT widthPix,
							UINT height)
{
    BYTE *tmp;  
	struct jpeg_compress_struct cinfo;
	/* More stuff */
	int  outfile;			/* target file */
	int row_stride;			/* physical row widthPix in image buffer */
    struct my_error_mgr jerr;
     
	if (dataBuf==NULL)
		return FALSE;
	if (widthPix==0)
		return FALSE;
	if (height==0)
		return FALSE;
  
	/* Step 1: allocate and initialize JPEG compression object */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
    
	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);

	/* Step 2: specify data destination (eg, a file) */
	/* Note: steps 2 and 3 can be done in either order. */
    outfile = IMGOS_CreateFile(fileName, PLXFS_CREATE_NEW | PLXFS_CREATE_TRUNCATE, 0);
    IMGOS_CloseFile(outfile);
    outfile = IMGOS_CreateFile(fileName, PLXFS_ACCESS_WRITE, 0);
	if (outfile == -1) {		
		return FALSE;
	}

	jpeg_stdio_dest(&cinfo, outfile);

	/* Step 3: set parameters for compression */
												    
	/* First we supply a description of the input image.
	* Four fields of the cinfo struct must be filled in:
	*/
	cinfo.image_width = widthPix; 	/* image widthPix and height, in pixels */
	cinfo.image_height = height;

	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
    //cinfo.input_components = 3;		/* # of color components per pixel */
    //cinfo.in_color_space = JCS_YCbCr; 	/* colorspace of input image */


 
/* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */

  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality(&cinfo, 100 /*quality*/, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */
  row_stride = widthPix * 3;	/* JSAMPLEs per row in image_buffer */
//    row_stride = widthPix * 2;	/* JSAMPLEs per row in image_buffer */
  
  tmp = dataBuf;
  while (cinfo.next_scanline < cinfo.image_height) {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
	LPBYTE outRow;
	//if (color)    
        outRow = tmp + (cinfo.image_height - cinfo.next_scanline - 1) * row_stride;
	
    //else        
	//	outRow = tmp + (cinfo.next_scanline * widthPix);
	

    (void) jpeg_write_scanlines(&cinfo, &outRow, 1);
  }

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);

  /* After finish_compress, we can close the output file. */
  IMGOS_CloseFile(outfile);

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);
  
  /* And we're done! */

  return TRUE;
}


#endif //NOJPEG
