/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ImageLib
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

#include "gif/dgif.h"
#include "gif/GIF_LIB.H"

static struct gif_logscreen     m_logical_screen;
static struct gif_frame 		m_frame;
static int	            		m_color_table;
int                             m_decode_state;
static PGIFOBJ                  m_pGif;

GIFWNDDATA   GifWndData[MAX_GIFNUM];


struct Bmp   bmp;

static int   DecodingToData(PGIFWNDDATA pGifWndData);
static int   DecodingToBitmap(PGIFWNDDATA pGifWndData);

/*********************************************************************\
* Function	 :  GIF_DS_InitSource
* Purpose    :
*     initialize data source of gif decoder struct
* Params	 :
*     pGif   : gif object need to be initialized
**********************************************************************/
void GIF_DS_InitSource( PGIFOBJ pGif)
{
    static DWORD               dwFileSize = 0;
    void *  pData;
    struct gif_decoder         *m_gif_decoder;
    
    m_gif_decoder = (struct gif_decoder*) pGif;

    pData = GIF_GetUserData(pGif);

    dwFileSize = GET32(((BYTE *)pGif) + GIF_GetObjectSize());  

    m_gif_decoder->DataSrc.next_input_byte = (unsigned char *) pData;
    m_gif_decoder->DataSrc.first_input_byte =(unsigned char *) pData;
    m_gif_decoder->DataSrc.bytes_in_buffer = (int)dwFileSize;
    m_gif_decoder->DataSrc.nbytes = (int)dwFileSize;

    return;
}
/*********************************************************************\
* Function	   : GIF_DS_GetData
* Purpose      :
*     get data of data source from gif decoder struct  
* Params	   :
*     pGif     : the gif object
* Remarks	   : not realize.
**********************************************************************/
int GIF_DS_GetData( PGIFOBJ pGif)
{
	return 0;
}

/*********************************************************************\
* Function	   : GIF_DS_SkipData
* Purpose      :
*     skip some datum from gif data source  
* Params	   :
*     pGif     : the gif object
*     num_bytes: number to be skipped
* Remarks	   : not realize.
**********************************************************************/
void GIF_DS_SkipData( PGIFOBJ pGif, int num_bytes)
{
	return;
}

/*********************************************************************\
* Function	   : GIF_DS_TermSource
* Purpose      :
*     terminate data source of gif struct  
* Params	   :
*     pGif     : the gif object
**********************************************************************/
void GIF_DS_TermSource( PGIFOBJ pGif)
{
   return;
}

#define	MAX_SCANLINES	768
unsigned char       * LineTable[MAX_SCANLINES];

static const int scan_offset[4] = { 0, 4, 2, 1 };
static const int scan_distence[4] = { 8, 8, 4, 2 };

/*********************************************************************\
* Function	       : GetGifDimensionFromData
* Purpose          :
*     Get gif image dimension from image data
* Params	       :
*     pData        : the address of data struct to get image dimension
*     pSize        : address of struct receiving dimensions
* Return	 	   :
*     if function success, return TRUE;
*     otherwise return FALSE  
**********************************************************************/
BOOL GetGifDimensionFromData(struct data_buffer pData, PSIZE pSize)
{
    int        ret;
    struct     gif_decoder *m_decoder; 
    PGIFOBJ    pGif;
   
    ret = GIF_GetObjectSize();
    pGif = (char *) malloc(sizeof(char) * ret + sizeof(DWORD) + sizeof(int));
    GIF_Construct(pGif);   

	ConvertImageData(pData.data_count, ((BYTE *)pGif) + ret, 0);
    GIF_SetUserData(pGif, pData.data);
    
    m_decoder = (struct gif_decoder*) pGif;

    m_decoder->DataSrc.init_source		= GIF_DS_InitSource;
	m_decoder->DataSrc.term_source		= GIF_DS_TermSource;
    m_decoder->DataSrc.next_input_byte = NULL;
	m_decoder->DataSrc.fill_input_buffer = GIF_DS_GetData;
    m_decoder->DataSrc.skip_input_data	 = GIF_DS_SkipData;
    m_decoder->DataSrc.resync_to_restart = NULL;

    GIF_SetDataSource(pGif, &m_decoder->DataSrc);
    ret = GIF_ReadHeader(pGif);
    if ( ret <= 0)
        return FALSE;
    GIF_GetLogicalScreen(pGif, &m_logical_screen);
    pSize->cx = m_logical_screen.width;
    pSize->cy = m_logical_screen.height;
    GIF_Destruct(pGif);
    free(pGif);

    return TRUE;
}

/*********************************************************************\
* Function	   : GetGifdimension
* Purpose      :
*     Get gif Image Dimension
* Params	   :
*     pImageObj: the image object that to be get dimension
*     pSize    : address of struct receiving dimensions
* Return	   :
*     if function success, return TRUE;
*     otherwise return FALSE;
**********************************************************************/
BOOL GetGifDimension(PPHYIMG pPhyImg, PSIZE pSize)
{
    PGIFOBJ             pGif;
    int                 ret;

    pGif = (PGIFOBJ) pPhyImg; 
    GIF_Reset(pGif);
    ret = GIF_ReadHeader(pGif);
    if ( ret <= 0)
        return NULL;
	GIF_GetLogicalScreen(pGif, &m_logical_screen);
    pSize->cx = m_logical_screen.width;
    pSize->cy = m_logical_screen.height;

    return TRUE;
}

/*********************************************************************\
* Function	   : Gif_SetBitmapInfo
* Purpose      :
*     set bitmap infomation from the first frame of gif object
* Params	   :
*     pGif     : handle of gif object
*     frame    : gif frame infomation 
* Return	   :
*     return bmp struct 	   
**********************************************************************/
struct Bmp Gif_SetBitmapInfo( PGIFOBJ pGif, struct gif_frame frame)
{
    int	                i;
    RGBQUAD             * pColor;
    unsigned char       * pColorTbl;
    struct gif_frame    Frame1;
    struct Bmp          bmp;

	bmp.m_bmpinfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);

    bmp.m_bmpinfo.bmiHeader.biWidth		= frame.width;
	bmp.m_bmpinfo.bmiHeader.biHeight	= -frame.height;
   	bmp.m_bmpinfo.bmiHeader.biPlanes	= 1;
	bmp.m_bmpinfo.bmiHeader.biBitCount	= 8;
	bmp.m_bmpinfo.bmiHeader.biCompression	= BI_RGB;
	bmp.m_bmpinfo.bmiHeader.biSizeImage		= 0;
	bmp.m_bmpinfo.bmiHeader.biXPelsPerMeter = 0;
	bmp.m_bmpinfo.bmiHeader.biYPelsPerMeter = 0;
	bmp.m_bmpinfo.bmiHeader.biClrUsed		= 0;
	bmp.m_bmpinfo.bmiHeader.biClrImportant	= 0;

    GIF_GetLogicalScreen(pGif, &m_logical_screen);
    if (frame.flags & GIF_COLORTBL)
    {	/* Copy frame color table */
        GIF_GetFrameDimensions(pGif, &Frame1);
        m_color_table = FRAME_COLORTABLE;
        pColorTbl = (unsigned char *)Frame1.p_color_table;
    }
    else
    {
        m_color_table = GLOBAL_COLORTABLE;
        pColorTbl = (unsigned char *)m_logical_screen.p_color_table;
    }

	/* Copy color table */
	pColor = bmp.m_bmpinfo.bmiColors;
	for ( i = 0; i < 256; i ++, pColor ++)
	{
		pColor->rgbRed	 = *pColorTbl++;
		pColor->rgbGreen = *pColorTbl++;
		pColor->rgbBlue	 = *pColorTbl++;
	}
    return bmp;
}

/*********************************************************************\
* Function	       : ConvertImageData
* Purpose          :
*     add data size and file handle to image object
* Params	       :
*     dwData       : the value of data size need to add to image object
*     pBaseAddress : the image object address
*     hFile        : the file handle need to add to image object 	   
**********************************************************************/
void ConvertImageData(DWORD dwData, BYTE* pBaseAddress, int hFile)
{
    BYTE * pData;

    pData = (BYTE *)pBaseAddress;
    *(pData) = (BYTE)dwData;
    *(pData + 1) = (BYTE)(dwData >> 8);
    *(pData + 2) = (BYTE)(dwData >> 16);
    *(pData + 3) = (BYTE)(dwData >> 24);
    *(int*)(pData + 4) = hFile;
}

/*********************************************************************\
* Function	   : CreateGifFromData
* Purpose      :
*     create gif object from file data
* Params	   :
*     pData    : the data struct to create gif 
*     hFile    : handle of file to create gif
* Return	   :
*     return gif object handle
**********************************************************************/
PPHYIMG  IMAGEAPI CreateGifFromData(struct data_buffer pData, int hFile)
{
    int ret;
    PGIFOBJ         pGif;
    struct gif_decoder     *m_gif_decoder;
   
    m_color_table = 0;
    ret = GIF_GetObjectSize();   
    pGif = (char *) malloc(sizeof(char) * ret + sizeof(DWORD) + sizeof(int));
	if (!pGif)
		return NULL;
  
    ConvertImageData(pData.data_count, ((BYTE *)pGif) + ret, hFile);
    GIF_Construct(pGif);

    GIF_SetUserData(pGif, pData.data);
    m_gif_decoder = (struct gif_decoder *)(pGif);

    
    m_gif_decoder->DataSrc.init_source		 = GIF_DS_InitSource;
    m_gif_decoder->DataSrc.term_source	     = GIF_DS_TermSource;
    m_gif_decoder->DataSrc.fill_input_buffer = GIF_DS_GetData;
    m_gif_decoder->DataSrc.skip_input_data	 = GIF_DS_SkipData;
    m_gif_decoder->DataSrc.resync_to_restart = NULL;
    
    m_gif_decoder->DataSrc.next_input_byte = NULL;

    GIF_SetDataSource(pGif, &m_gif_decoder->DataSrc);

    return  (PPHYIMG)pGif;
}

BOOL DeleteGif(PPHYIMG pImg, BOOL bMapFile)
{
     BYTE    *pData = (BYTE*)pImg;
	 int     hFile;
     
     hFile = *(int*)(pData +GIF_GetObjectSize() + sizeof(DWORD));
     GIF_Destruct((PGIFOBJ)pData);
	 if (hFile)
	 {
		 DWORD dwFileSize;
		 void  *pMapFile;

         pMapFile = GIF_GetUserData(pData);

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
* Function	   : CreateBitmapFromGif
* Purpose      :
*     Create bitmap from gif object
* Params	   :
*     pData    : data struct to create bitmap
* Return	   :
*     if function success, return bitmap handle
*     otherwise return NULL
**********************************************************************/
HBITMAP CreateBitmapFromGif(HDC hdc, struct data_buffer pData,
                            PCOLORREF pColor, BOOL * pbTran)
{
    int                 scanlines, y_start;
    int	                i, ret, count, line_w;
    unsigned char       * ptr, * pImgBuffer;
    PGIFOBJ             pGif;
    struct gif_frame    frame;
    struct Bmp          bmp;
    HBITMAP             hBitmap;
   
    if (pbTran)
        *pbTran = FALSE;

    pGif = (PGIFOBJ)CreateGifFromData(pData, 0);
	if (!pGif)
		return NULL;

    GIF_Reset(pGif);
    ret = GIF_ReadHeader(pGif);
    if ( ret <= 0)
    {
        GIF_Destruct(pGif);
        free(pGif);
        return NULL;
    }
	GIF_GetLogicalScreen(pGif, &m_logical_screen);
    ret = GIF_ReadFrameHeader(pGif, 0);
    if (ret <= 0)
    {
        GIF_Destruct(pGif);
        free(pGif);
        return NULL; 		
    }
    GIF_SetColorMode(pGif, GCS_RAW);

    GIF_GetFrameDimensions(pGif, &frame);
    if (frame.height > m_logical_screen.height || frame.width > m_logical_screen.width)
    {
        GIF_Destruct(pGif);
        free(pGif);
        return NULL; 		
    }

    bmp = Gif_SetBitmapInfo(pGif, frame);
   
    scanlines = frame.height;
	line_w = (m_logical_screen.width + 3) & ~3;

    if (frame.flags & GIF_TRANSPARENT) /* 图象透明 */
    {
        if(pbTran)
            *pbTran = TRUE;
        
        if (frame.transparent >= 0 && frame.transparent <= 255)
        {
            i = frame.transparent;
            if (pColor)
            {                
                *pColor = RGB(bmp.m_bmpinfo.bmiColors[i].rgbRed,
                    bmp.m_bmpinfo.bmiColors[i].rgbGreen,
                    bmp.m_bmpinfo.bmiColors[i].rgbBlue);
            }
        }
    }

	/* Make line table */
	pImgBuffer = ptr = (unsigned char *) malloc (line_w * m_logical_screen.height);
	if ( ptr == NULL)
    {
        GIF_Destruct(pGif);
        free(pGif);
        return 0;
    }
    memset(pImgBuffer, 0, line_w * m_logical_screen.height);
    ptr += frame.top * line_w + frame.left;
	if (frame.flags & GIF_INTERLACE)    /* 隔行扫描 */
    {
        int row, num, k;

	    for ( i = 0, k = 0, num = 0; i < min(scanlines, MAX_SCANLINES); num ++)
        {
            row = scan_offset[k] + scan_distence[k] * num;
            if ( row < min(scanlines, MAX_SCANLINES))
                LineTable[i ++] = ptr + (row + frame.top) * line_w + frame.left;
            else
            {
                num = -1;
                k ++;
            }
        }
    }
    else           /* 逐行扫描 */
    {
	    for ( i = 0; i < min(scanlines, MAX_SCANLINES); i ++, ptr += line_w)
		    LineTable[i] = ptr;
    }

    y_start = GIF_GetOutputScanline(pGif);
    	/* Read in scan line */
	count = (scanlines < MAX_SCANLINES) ? scanlines : MAX_SCANLINES;
	ret = GIF_ReadScanlines(pGif, LineTable, count);
    if ( ret <= 0)
    {
        free (pImgBuffer);
        GIF_Destruct(pGif);
        free(pGif);
        return NULL;
    }

    bmp.m_bmpinfo.bmiHeader.biWidth = m_logical_screen.width;
    bmp.m_bmpinfo.bmiHeader.biHeight = -m_logical_screen.height;
    hBitmap = CreateDIBitmap(NULL, &bmp.m_bmpinfo.bmiHeader, CBM_INIT, 
        pImgBuffer, &bmp.m_bmpinfo, DIB_RGB_COLORS);   

    free(pImgBuffer);
    GIF_Destruct(pGif);
    free(pGif);

    return hBitmap;
}

/*********************************************************************\
* Function	   : PaintStaticGif
* Purpose      :
*     Paint static gif
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
* 	   if function success, return 1;
*      otherwise return 0
**********************************************************************/
int PaintStaticGif(HDC hdc, int nDestX, int nDestY, int nWidth, int nHeight, 
                   PPHYIMG pPhyImg, int nSrcX, int nSrcY, DWORD dwRop)
{
    int                 scanlines, y_start, nOldBkMode;
    int	                i, ret, count, line_w;
    unsigned char       * ptr, * pImgBuffer;
    PGIFOBJ             pGif;
    struct gif_frame    frame;
    struct Bmp          bmp;
    COLORREF            oldColor;

    pGif = (PGIFOBJ) pPhyImg;

    GIF_Reset(pGif);
    ret = GIF_ReadHeader(pGif);
    if ( ret <= 0)
        return 0;
	GIF_GetLogicalScreen(pGif, &m_logical_screen);
    ret = GIF_ReadFrameHeader(pGif, 0);
    if (ret <= 0)
        return 0;
    GIF_SetColorMode(pGif, GCS_RAW);
    	    
    GIF_GetFrameDimensions(pGif, &frame);

    if (frame.width > m_logical_screen.width || frame.height > m_logical_screen.height)
        return 0;

    bmp = Gif_SetBitmapInfo(pGif, frame);
    
    scanlines = frame.height;
    if (frame.flags & GIF_TRANSPARENT) /* 图象透明 */
    {
        if (frame.transparent >= 0 && frame.transparent <= 255)
        {
            nOldBkMode = SetBkMode(hdc, BM_NEWTRANSPARENT);
            /*oldColor = SetBkColor(hdc, RGB(bmp.m_bmpinfo.bmiColors[i].rgbRed,
            bmp.m_bmpinfo.bmiColors[i].rgbGreen,
            bmp.m_bmpinfo.bmiColors[i].rgbBlue));*/
            oldColor = SetBkColor(hdc, (1 << 24) | frame.transparent);
        }
    }

	line_w = (frame.width + 3) & ~3;

	/* Make line table */
	pImgBuffer = ptr = (unsigned char *) malloc (line_w * scanlines);
	if ( ptr == NULL)
		return 0;

	if (frame.flags & GIF_INTERLACE)  /* 隔行扫描 */
    {
        int row, num, k;

	    for ( i = 0, k = 0, num = 0; i < min(scanlines, MAX_SCANLINES); num ++)
        {
            row = scan_offset[k] + scan_distence[k] * num;
            if ( row < min(scanlines, MAX_SCANLINES))
                LineTable[i ++] = ptr + row * line_w;
            else
            {
                num = -1;
                k ++;
            }
        }
    }
    else                   /* 逐行扫描 */
    {
	    for ( i = 0; i < min(scanlines, MAX_SCANLINES); i ++, ptr += line_w)
		    LineTable[i] = ptr;
    }

    y_start = GIF_GetOutputScanline(pGif);
	/* Read in scan line */
	while ( scanlines > 0)
	{
		count = (scanlines < MAX_SCANLINES) ? scanlines : MAX_SCANLINES;
		ret = GIF_ReadScanlines(pGif, LineTable, count);
        if ( ret <= 0)
            break;

        
        if (nWidth == 0 && nHeight == 0)
		{
			SetDIBitsToDevice(hdc, nDestX, nDestY + y_start,
                bmp.m_bmpinfo.bmiHeader.biWidth, ret - nSrcY,
                nSrcX, nSrcY, 0, ret - nSrcY, pImgBuffer, &bmp.m_bmpinfo, 
				DIB_RGB_COLORS);
		}
		else
            StretchDIBits(hdc, nDestX, nDestY + y_start, nWidth, nHeight,
                nSrcX, nSrcY, frame.width - nSrcX, frame.height - nSrcY,
				pImgBuffer, &bmp.m_bmpinfo, DIB_RGB_COLORS, SRCCOPY);    

        scanlines -= ret;
        y_start += ret;
	}

    if (frame.flags & GIF_TRANSPARENT) /* 图象透明 */
    {
        SetBkColor(hdc, oldColor);
        SetBkMode(hdc, nOldBkMode);
    }
	free (pImgBuffer);
   
	return 1;
}

/*********************************************************************\
* Function	   : DrawStaticGif
* Purpose      :
*     Draw static gif according to file handle
* Params	   :
*     hdc      : handle for device context
*     x        : x-coordination of rectangle of top-left corner
*     y        : y-coordination of rectangle of top-left corner
*     width    : width of rectangle to draw gif
*     height   : height of rectangle to draw gif
*     pData    : data struct to draw gif
*     dwRop    : raster operation code 
* Return	   :
*	  if function success, return TRUE
*     otherwise return S_ERROR  
**********************************************************************/
BOOL DrawStaticGif (HDC hdc, int x, int y, int width, int height,
                    struct data_buffer pData, DWORD dwRop)
{
    int                 ret;
    struct gif_decoder  *m_gif_decoder;
    PIMGOBJ             pImgObj;
    PGIFOBJ             pGif;

	 m_decode_state = S_INIT;
     m_color_table = 0;

next_state:

	switch ( m_decode_state)
	{
	case S_INIT:
		// Set data source 
        ret = GIF_GetObjectSize();
 	    pGif = (char *) malloc (sizeof(char) * ret + sizeof(DWORD) + sizeof(int));
               
        ConvertImageData(pData.data_count, ((BYTE*)pGif) + ret, 0);
	    GIF_Construct(pGif);
	    // Must do set user data before set data source 

	    GIF_SetUserData(pGif, pData.data);
        m_gif_decoder = (struct gif_decoder*) pGif;

        m_gif_decoder->DataSrc.init_source		 = GIF_DS_InitSource;
        m_gif_decoder->DataSrc.term_source	     = GIF_DS_TermSource;
	    m_gif_decoder->DataSrc.fill_input_buffer = GIF_DS_GetData;
	    m_gif_decoder->DataSrc.skip_input_data	 = GIF_DS_SkipData;
	    m_gif_decoder->DataSrc.resync_to_restart = NULL;
        
        m_gif_decoder->DataSrc.next_input_byte = NULL;

		GIF_SetDataSource(pGif, &m_gif_decoder->DataSrc);
		m_decode_state = S_READHEAD;
		goto next_state;

	case S_READHEAD:
		// Read in Gif header.
		ret = GIF_ReadHeader(pGif);
		if ( ret > 0)
		{	// Get logical screen dimensions.
			GIF_GetLogicalScreen(pGif, &m_logical_screen);
			m_decode_state = S_READFRAMEHEADER;
		}

		if ( ret <= 0)
			m_decode_state = S_ERROR;

        goto next_state;		

	case S_READFRAMEHEADER:
		// Read in frame header
		ret = GIF_ReadFrameHeader(pGif, 0);
		if ( ret > 0)
		{	// Get frame dimensions.
			GIF_GetFrameDimensions(pGif, &m_frame);
			m_decode_state  = S_READSCANLINES;
            // Set output mode 
            GIF_SetColorMode(pGif, GCS_RAW);
            // Set bitmap info of this frame 
			Gif_SetBitmapInfo(pGif, m_frame);
		}

		if ( ret <= 0)
			m_decode_state = S_ERROR;
	
        goto next_state;
	
        break;
        
    case S_READSCANLINES:
        // Write new image line 
        pImgObj = (PIMGOBJ)pGif;    
		ret = PaintStaticGif(hdc, x, y, width, height, pImgObj, 0, 0, dwRop);
		/* If return code big than 0, the image is all displied on it's display
		 * window, the next thing to do is consume input to check the end of
		 * input data.
		 */
		if ( ret > 0)        
			m_decode_state = S_CONSUMEINPUT;
		
		if ( ret <= 0)
			m_decode_state = S_ERROR;

        goto next_state;

        break;
		
	case S_CONSUMEINPUT:
		/* Consume input until end of input data or data error */
		ret = GIF_ConsumeInput (pGif);
        ret = GIF_EndFrame(pGif);
		if ( ret != 0)
		{
			m_decode_state = S_DATAEND;
            goto next_state;
		}
        break;
	
    case S_DATAEND:
    
        GIF_Destruct(pGif);
        free(pGif);
        break;

    case S_ERROR:
        GIF_Destruct(pGif);
        free(pGif);
        
        return FALSE;
    }

    return TRUE;
}

BOOL    GIF_IsGIFDynamic(void* pGIFData, DWORD dwDataSize)
{
    
    int                 ret;
    struct gif_decoder  *m_gif_decoder;
    PIMGOBJ             pImgObj;
    PGIFOBJ             pGif;
    int                 scanlines, count, retval, i, line_w;
    unsigned char       * ptr, * pImgBuffer;
    
    m_decode_state = S_INIT;
    m_color_table = 0;
    
    ret = GIF_GetObjectSize();
    pGif = (char *) malloc (sizeof(char) * ret + sizeof(DWORD) + sizeof(int));
    
    ConvertImageData(dwDataSize, ((BYTE*)pGif) + ret, 0);
    GIF_Construct(pGif);
    // Must do set user data before set data source 
    
    GIF_SetUserData(pGif, pGIFData);
    m_gif_decoder = (struct gif_decoder*) pGif;
    
    m_gif_decoder->DataSrc.init_source		 = GIF_DS_InitSource;
    m_gif_decoder->DataSrc.term_source	     = GIF_DS_TermSource;
    m_gif_decoder->DataSrc.fill_input_buffer = GIF_DS_GetData;
    m_gif_decoder->DataSrc.skip_input_data	 = GIF_DS_SkipData;
    m_gif_decoder->DataSrc.resync_to_restart = NULL;
    
    m_gif_decoder->DataSrc.next_input_byte = NULL;
    
    GIF_SetDataSource(pGif, &m_gif_decoder->DataSrc);
    ret = GIF_ReadHeader(pGif);
    if ( ret <= 0)
    {
        GIF_Destruct(pGif);
        free(pGif);
        return FALSE;
    }

    ret = GIF_ReadFrameHeader(pGif, 0);
    if ( ret <= 0)
    {
        GIF_Destruct(pGif);
        free(pGif);
        return FALSE;
    }
    GIF_GetFrameDimensions( pGif, &m_frame);

    GIF_SetColorMode( pGif, GCS_RAW);

    scanlines = m_frame.height;
    line_w = (m_frame.width + 3) & ~3;

    pImgBuffer = ptr = (unsigned char *) malloc (line_w * scanlines);
    if ( ptr == NULL)
        return 0;
    
    if ( m_frame.flags & GIF_INTERLACE)  /* 隔行扫描 */
    {
        int row, num, k;
        
        for ( i = 0, k = 0, num = 0; i < min(scanlines, MAX_SCANLINES); num ++)
        {
            row = scan_offset[k] + scan_distence[k] * num;
            if ( row < min(scanlines, MAX_SCANLINES))
                LineTable[i ++] = ptr + row * line_w;
            else
            {
                num = -1;
                k ++;
            }
        }
    }
    else                     /* 逐行扫描 */
    {
        for ( i = 0; i < min(scanlines, MAX_SCANLINES); i ++, ptr += line_w)
            LineTable[i] = ptr;
    }
    while ( scanlines > 0)
    {
        count = (scanlines < MAX_SCANLINES) ? scanlines : MAX_SCANLINES;
        retval = GIF_ReadScanlines( pGif, LineTable, count);
        if ( retval <= 0)
            break;
        scanlines -= retval;
    }

    //PaintGifImage(NULL, &GifWndData[index], FALSE, TRUE);
    ret = GIF_ReadFrameHeader(pGif, 0);
    if (ret > 0)
    {
        free(pImgBuffer);
        GIF_Destruct(pGif);
        free(pGif);
        return TRUE;
    }
    else
    {
        free(pImgBuffer);
        GIF_Destruct(pGif);
        free(pGif);
        return FALSE;
    }
        
}

/*********************************************************************\
* Function	   : DrawGif
* Purpose      :
*     Draw some frame gif
* Params	   :
*     hWnd     : window to draw gif image
*     x        : x_coordination of the rectangle left_top corner 
*     y        : y_coordination of the rectangle left_top corner
*     pData    : the address of data struct to draw animated gif
*     dm       : DM_BITMAP, DM_NONE and DM_DATA
*     Times    : times to show image
* Return	   :
*	  return animated gif object 
**********************************************************************/
HGIFANIMATE DrawGif (HWND hWnd, int x, int y, int width, int height, struct data_buffer pData,
                     int nType, int dm, int Times)
{   
    int                 ret;
    unsigned long       m_timer;
    int                 index, i;
    static BOOL         bFirstTime = TRUE;
    int                 timer;
    struct gif_decoder *m_decoder;
	
    index = 0;

    m_decode_state = S_INIT;
    m_color_table = 0;

next_state:

	switch ( m_decode_state)
	{
	case S_INIT:
		// Create one GIF debug object.
		ret = GIF_GetObjectSize();
 	    m_pGif = (char *) malloc (sizeof(char) * ret + sizeof(DWORD) + sizeof(int));
        ConvertImageData(pData.data_count, ((BYTE*)m_pGif) + ret, 0);
	    GIF_Construct( m_pGif);

	    /* Must do set user data before set data source */

	    GIF_SetUserData( m_pGif, pData.data);
        m_decoder = (struct gif_decoder*) m_pGif;
     
        m_decoder->DataSrc.init_source		= GIF_DS_InitSource;
	    m_decoder->DataSrc.fill_input_buffer	= GIF_DS_GetData;
	    m_decoder->DataSrc.skip_input_data	= GIF_DS_SkipData;
	    m_decoder->DataSrc.resync_to_restart	= NULL;
	    m_decoder->DataSrc.term_source		= GIF_DS_TermSource;
        m_decoder->DataSrc.next_input_byte = NULL;
		/* Set data source */
		GIF_SetDataSource( m_pGif, &m_decoder->DataSrc);
		m_decode_state = S_READHEAD;

		goto next_state;

	case S_READHEAD:
		// Read in GIF header.
		ret = GIF_ReadHeader( m_pGif);
		if ( ret > 0)
		{	// Get logical screen dimensions.
			GIF_GetLogicalScreen( m_pGif, &m_logical_screen);
			m_decode_state = S_READFRAMEHEADER;
		}

		if ( ret <= 0)
			m_decode_state = S_ERROR;

        goto next_state;

	case S_READFRAMEHEADER:
		// Read in frame header
        ret = GIF_ReadFrameHeader( m_pGif, 0);
        if ( ret <= 0)
        {
			m_decode_state = S_ERROR;
            goto next_state;
        }
		    // Get frame dimensions.
		GIF_GetFrameDimensions( m_pGif, &m_frame);
        GIF_SetColorMode( m_pGif, GCS_RAW);

		m_decode_state  = S_READSCANLINES;
        /* Set output mode */

        /* Set bitmap info of this frame */
        if (bFirstTime == TRUE)
        {
            for (i = 0; i < MAX_GIFNUM; i++)
                GifWndData[i].hwndParent = NULL;
            bFirstTime = FALSE;
        }
        for (index = 0; index < MAX_GIFNUM; index++)
        {
            if (GifWndData[index].hwndParent == NULL)
                break;
        }
        if (index == MAX_GIFNUM)
        {
            m_decode_state  = S_ERROR;        
            goto next_state;
        }
        if (m_frame.width > m_logical_screen.width || m_frame.height > m_logical_screen.height)
        {
            m_decode_state  = S_ERROR;        
            goto next_state;
        }
        GifWndData[index].frame = m_frame;
        GifWndData[index].nframes = 1;        
        GifWndData[index].bRepaint = TRUE;
        GifWndData[index].LastMethod = 0;
        GifWndData[index].pGif = m_pGif;
        GifWndData[index].datasource = m_decoder->DataSrc;
        GifWndData[index].dm = dm;
        timer = m_frame.delay_time;

        GIF_SetDataSource( m_pGif, &GifWndData[index].datasource);

        SetBitmapInfo(&GifWndData[index]);
		goto next_state;

        break;

    case S_READSCANLINES:
        /* Write new image line */ 
        GifWndData[index].hwndParent = hWnd;
        GifWndData[index].x = x;
        GifWndData[index].y = y;
		GifWndData[index].width = m_logical_screen.width;
		GifWndData[index].height = m_logical_screen.height;
        GifWndData[index].DstWidth = width;
		GifWndData[index].DstHeight = height;
        GifWndData[index].pHead = NULL;
        GifWndData[index].Times = Times;
        GifWndData[index].Count = 0;
		GifWndData[index].nMapFile = pData.nMapFile;
		GifWndData[index].pfnPaintBkFunc = NULL;

		GifWndData[index].hCompatibleDC = NULL;
        GifWndData[index].hCompatibleBmp = NULL;


        if (dm == DM_NONE)
		    //ret = PaintGifImage(&GifWndData[index], FALSE);
			ret = TRUE;
        else if (dm == DM_DATA)
        {
            ret = DecodingToData(&GifWndData[index]);
        }
        else if (dm == DM_BITMAP)
        {
            ret = DecodingToBitmap(&GifWndData[index]);
        }	
		/* If return code big than 0, the image is all displayed on it's display
		 * window, the next thing to do is consume input to check the end of
		 * input data.
		 */
		if ( ret > 0)
        	m_decode_state = S_CONSUMEINPUT;
		
		if ( ret <= 0)
			m_decode_state = S_ERROR;
        
        goto next_state;
		
        break;
		
	case S_CONSUMEINPUT:
		/* Consume input until end of input data or data error */
	
        ret = GIF_EndFrame(m_pGif);
		m_decode_state = S_DATAEND;
        if (nType == 1)
            timer = timer * 10;

        if (timer != 0)
        {
            m_timer = SetTimer(NULL, NULL, timer,TimerProc);
            GifWndData[index].m_timer = m_timer;
        }
		else
		{
			m_timer = SetTimer(NULL, NULL, 100,TimerProc);
			GifWndData[index].m_timer = m_timer;
		}

        return (HGIFANIMATE)(&GifWndData[index]);

        break;

    case S_DATAEND:
    case S_ERROR:

        GIF_Destruct( m_pGif);
        free(m_pGif);
        break;
    }

    return NULL;
}
/*********************************************************************\
* Function	     : SetBitmapInfo
* Purpose        :
*     set bitmap header infomation from gif data
* Params	     :
*     pGifWndData: gif data to set bitmap infomation
**********************************************************************/
void SetBitmapInfo( PGIFWNDDATA pGifWndData)
{
    int	                i;
    RGBQUAD             * pColor;
    unsigned char       * pColorTbl;

	bmp.m_bmpinfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);

	bmp.m_bmpinfo.bmiHeader.biWidth		= pGifWndData->frame.width;
	bmp.m_bmpinfo.bmiHeader.biHeight	= -pGifWndData->frame.height;

	bmp.m_bmpinfo.bmiHeader.biPlanes	= 1;
	bmp.m_bmpinfo.bmiHeader.biBitCount	= 8;
	bmp.m_bmpinfo.bmiHeader.biCompression	= BI_RGB;
	bmp.m_bmpinfo.bmiHeader.biSizeImage		= 0;
	bmp.m_bmpinfo.bmiHeader.biXPelsPerMeter = 0;
	bmp.m_bmpinfo.bmiHeader.biYPelsPerMeter = 0;
	bmp.m_bmpinfo.bmiHeader.biClrUsed		= 0;
	bmp.m_bmpinfo.bmiHeader.biClrImportant	= 0;

    GIF_GetLogicalScreen( pGifWndData->pGif, &m_logical_screen);

    if ( pGifWndData->frame.flags & GIF_COLORTBL) /* local color table */
    {	/* Copy frame color table */
        m_color_table = FRAME_COLORTABLE;
        pColorTbl = (unsigned char *)pGifWndData->frame.p_color_table;
    }
    else
    {	/* Use global color table */
        m_color_table = GLOBAL_COLORTABLE;
        pColorTbl = (unsigned char *)m_logical_screen.p_color_table;
    }

	/* Copy color table */
	pColor = bmp.m_bmpinfo.bmiColors;
	for ( i = 0; i < 256; i ++, pColor ++)
	{
		pColor->rgbRed	 = *pColorTbl++;
		pColor->rgbGreen = *pColorTbl++;
		pColor->rgbBlue	 = *pColorTbl++;        
	}
}

/*********************************************************************\
* Function	     : SeekToStart
* Purpose        :
*     Set gif data to start position
* Params	     :
*     pGifWndData: gif data
* Return	 	 : 
*     if function success, return 1;
*     otherwise return -1
**********************************************************************/
int SeekToStart(PGIFWNDDATA pGifWndData)
{
    int	ret;

	/* Reset GIF decoder to initial state */
	GIF_Reset( pGifWndData->pGif);
    pGifWndData->datasource.next_input_byte = pGifWndData->datasource.first_input_byte;
    pGifWndData->datasource.bytes_in_buffer = pGifWndData->datasource.nbytes;

    ret = GIF_ReadFrameHeader( pGifWndData->pGif, 0);
	if ( ret <= 0)
	{
		m_decode_state = S_ERROR;
		return -1;
	}

    return 1;
}

/*********************************************************************\
* Function	   : TimerProc
* Purpose      :
*     process WM_TIMER message
* Params	   :
*     hWnd     : handle of window for timer messages
*     uMsg     : WM_TIMER message
*     idEvent  : timer identifier
*     dwTime   : current system time  
**********************************************************************/
void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
    int ret, i;
    int index, nOldBkMode;
    HDC hdc;

    /* search timer identifer */
    for (index = 0; index < MAX_GIFNUM; index++)
    {
        if (GifWndData[index].m_timer == idEvent)
            break;
    }
    if (index == MAX_GIFNUM)
        return;

    if (GifWndData[index].dm == DM_NONE)
    {
        if ((GifWndData[index].hwndParent!= (HWND)-1)
            && (!IsWindow(GifWndData[index].hwndParent)))
        {
            KillTimer(hWnd, idEvent);
            GIF_Destruct( GifWndData[index].pGif);
            if (GifWndData[index].pfnPaintBkFunc && GifWndData[index].hwndParent != (HWND)-1)
            {
			    DeleteDC(GifWndData[index].hCompatibleDC);
                DeleteObject((HGDIOBJ)GifWndData[index].hCompatibleBmp);
            }
            free(GifWndData[index].pGif);
            GifWndData[index].hwndParent = NULL;
            for (i = 0; i < MAX_GIFNUM; i++)
            {
                if ((GifWndData[i].pGif == GifWndData[index].pGif)
                    && (i != (int)(index)) && (GifWndData[i].hwndParent != NULL))
                {
                    KillTimer(hWnd, GifWndData[i].m_timer);
                    GIF_Destruct(GifWndData[i].pGif);
                    if (GifWndData[i].pfnPaintBkFunc && GifWndData[i].hwndParent != (HWND)-1)
                    {
                        DeleteDC(GifWndData[i].hCompatibleDC);
                        DeleteObject((HGDIOBJ)GifWndData[i].hCompatibleBmp);
                    }
                    free(GifWndData[i].pGif);
                    GifWndData[i].hwndParent = NULL;
                }
            }
            return;
        }
        ret = GIF_ReadFrameHeader(GifWndData[index].pGif , 0);
        if (ret <= 0)                             /* 读完全部帧 */
        {
            if (GifWndData[index].nframes == 1)  /* 总共只有一帧 */
            {
                KillTimer(hWnd, idEvent);
                //GIF_Destruct( GifWndData[index].pGif);
                //free(GifWndData[index].pGif);
                return;
            }
            GifWndData[index].nframes = 0;
            GifWndData[index].Count ++;
            
            /* 达到指定显示次数*/
            if (GifWndData[index].Times == GifWndData[index].Count && 
				GifWndData[index].hwndParent) 
            {
                KillTimer(hWnd, idEvent);
                if (GifWndData[index].pfnPaintBkFunc)
                    GifWndData[index].pfnPaintBkFunc((HGIFANIMATE)&GifWndData[index], TRUE, 
                    0, 0, NULL);
                GIF_Destruct(GifWndData[index].pGif);
                if (GifWndData[index].pfnPaintBkFunc && GifWndData[index].hwndParent != (HWND)-1)
                {
                    DeleteDC(GifWndData[index].hCompatibleDC);
                    DeleteObject((HGDIOBJ)GifWndData[index].hCompatibleBmp);
                }
                free(GifWndData[index].pGif);
                GifWndData[index].hwndParent = NULL;
                return;                
            }
            /* 从头显示 */
            if (SeekToStart(&GifWndData[index]) <= 0)
            {
                KillTimer(hWnd, idEvent);
                GIF_Destruct(GifWndData[index].pGif);
                if (GifWndData[index].pfnPaintBkFunc && GifWndData[index].hwndParent != (HWND)-1)
                {
                    DeleteDC(GifWndData[index].hCompatibleDC);
                    DeleteObject((HGDIOBJ)GifWndData[index].hCompatibleBmp);
                }
                free(GifWndData[index].pGif);
                return;
            }
            if (GifWndData[index].frame.flags & GIF_TRANSPARENT)
            {
                RECT rect;
                if (!GifWndData[index].pfnPaintBkFunc)
                {
                    rect.left = GifWndData[index].x;
                    rect.top = GifWndData[index].y;
                    if (GifWndData[index].DstHeight != 0 && GifWndData[index].DstWidth != 0)
                    {
                        rect.bottom = rect.top + GifWndData[index].DstHeight;
                        rect.right = rect.left + GifWndData[index].DstWidth;
                    }
                    else
                    {
                        rect.bottom = rect.top + GifWndData[index].height;
                        rect.right = rect.left + GifWndData[index].width;
                    }
                    InvalidateRect(GifWndData[index].hwndParent, &rect, TRUE);
                    GifWndData[index].bRepaint = FALSE;
                    UpdateWindow(GifWndData[index].hwndParent);
                    GifWndData[index].bRepaint = TRUE;
                }
                else
                {
                    GifWndData[index].pfnPaintBkFunc((HGIFANIMATE)&GifWndData[index], FALSE, 
                        GifWndData[index].x, GifWndData[index].y, 
                        GifWndData[index].hCompatibleDC);
                }
            }

			/*if (GifWndData[index].frame.flags & GIF_TRANSPARENT)
			{
				RECT rect;
				
				rect.left = GifWndData[index].frame.left + GifWndData[index].x;
				rect.top = GifWndData[index].frame.top + GifWndData[index].y;
				rect.bottom = rect.top + GifWndData[index].frame.width;
				rect.right = rect.left + GifWndData[index].frame.height;
				InvalidateRect(GifWndData[index].hwndParent, &rect, TRUE);
				GifWndData[index].bRepaint = FALSE;
				UpdateWindow(GifWndData[index].hwndParent);
				GifWndData[index].bRepaint = TRUE;
			}*/
        }
        GifWndData[index].nframes ++;
        GIF_GetFrameDimensions(GifWndData[index].pGif, &(GifWndData[index].frame));
        GIF_SetColorMode(GifWndData[index].pGif, GCS_RAW);
        SetBitmapInfo(&GifWndData[index]);
        m_decode_state = S_READSCANLINES;

		
		

        PaintGifImage(NULL, &GifWndData[index], FALSE, TRUE);
    }
    else if (GifWndData[index].dm == DM_DATA)         /* DM_DATA方式 */
    {
        struct gif_frame *frame;
        int nTrans = -1;

        if (GifWndData[index].nframes == 1)  /* 总共只有一帧 */
        {
            KillTimer(hWnd, idEvent);
            return;
        }
        frame = &(((PHEADOFDATA)GifWndData[index].pCur)->frame);
        if (((PHEADOFDATA)GifWndData[index].pHead)->pNext == GifWndData[index].pHead)
        {
            if (GifWndData[index].pfnPaintBkFunc)
                GifWndData[index].pfnPaintBkFunc((HGIFANIMATE)&GifWndData[index], TRUE, 
                    0, 0, NULL);
             EndAnimatedGIF((HGIFANIMATE)&GifWndData[index]);
             return;
        }
        hdc = GetDC(GifWndData[index].hwndParent);

         /* 需要恢复为背景颜色或原来的图象 */
        if (((PHEADOFDATA)GifWndData[index].pCur)->bRect || GifWndData[index].pCur == GifWndData[index].pHead) 
        {
            RECT rect1, rect2;

            rect1 = ((PHEADOFDATA)GifWndData[index].pCur)->rect;
            rect2.left = GifWndData[index].x + rect1.left;
            rect2.right = GifWndData[index].x + rect1.right;
            rect2.top = GifWndData[index].y + rect1.top;
            rect2.bottom = GifWndData[index].y + rect1.bottom;
           
            InvalidateRect(GifWndData[index].hwndParent, &rect2, TRUE);
            GifWndData[index].bRepaint = FALSE;
            UpdateWindow(GifWndData[index].hwndParent);
            GifWndData[index].bRepaint = TRUE;
        }

        /* 图象透明 */
        if (frame->flags & GIF_TRANSPARENT)
        {
            if (GifWndData[index].nframes == 1)  /* 总共只有一帧 */
            {
                KillTimer(hWnd, idEvent);
                return;
            }
            if (frame->transparent >= 0 && frame->transparent <= 255)
                nTrans = frame->transparent;

            if ( (nTrans >= 0) && (nTrans <= 255))
            {
                struct Bmp bmp1;
                COLORREF oldColor;

                bmp1 = ((PHEADOFDATA)GifWndData[index].pCur)->bmp;

                nOldBkMode = SetBkMode(hdc, NEWTRANSPARENT);
                /*oldColor = SetBkColor(hdc, RGB(bmp1.m_bmpinfo.bmiColors[nTrans].rgbRed,
                    bmp1.m_bmpinfo.bmiColors[nTrans].rgbGreen,
                    bmp1.m_bmpinfo.bmiColors[nTrans].rgbBlue));
                 */
                oldColor = SetBkColor(hdc, (1 << 24) | nTrans);
                if (GifWndData[index].DstHeight == 0 && GifWndData[index].DstWidth == 0)
                {
                    SetDIBitsToDevice(hdc, GifWndData[index].x + frame->left, 
                        GifWndData[index].y + frame->top, 
                        frame->width, frame->height, 
                        0, 0, 0, frame->height,
                        ((PHEADOFDATA)GifWndData[index].pCur)->pData,
                        &((PHEADOFDATA)GifWndData[index].pCur)->bmp.m_bmpinfo,
                        0);
                }
                else
                {
                    StretchDIBits(hdc, GifWndData[index].x + frame->left * GifWndData[index].DstWidth / GifWndData[index].width,
                        GifWndData[index].y + frame->top *GifWndData[index].DstHeight / GifWndData[index].height, 
                        frame->width * GifWndData[index].DstWidth / GifWndData[index].width, frame->height * GifWndData[index].DstHeight / GifWndData[index].height,
                        0, 0, frame->width, frame->height,                        
                        ((PHEADOFDATA)GifWndData[index].pCur)->pData,
                        &((PHEADOFDATA)GifWndData[index].pCur)->bmp.m_bmpinfo,
                        DIB_RGB_COLORS, SRCCOPY); 
                }
                SetBkColor(hdc, oldColor);
                SetBkMode(hdc, nOldBkMode);
            }
        }
        else
        {
            if (GifWndData[index].DstHeight == 0 && GifWndData[index].DstWidth == 0)
            {
                SetDIBitsToDevice(hdc, GifWndData[index].x + frame->left, 
                    GifWndData[index].y + frame->top, 
                    frame->width, frame->height, 
                    0, 0, 0, frame->height,
                    ((PHEADOFDATA)GifWndData[index].pCur)->pData,
                    &((PHEADOFDATA)GifWndData[index].pCur)->bmp.m_bmpinfo, 0);
            }
            else
            {
            }
        }

         ReleaseDC(GifWndData[index].hwndParent, hdc); 

         GifWndData[index].pCur = ((PHEADOFDATA)GifWndData[index].pCur)->pNext;
        
         /* 全部帧读完一遍时增加遍数记录 */
         if(GifWndData[index].pCur == GifWndData[index].pHead )
            GifWndData[index].Count ++;

         /* 达到指定显示次数，结束显示 */
         if (GifWndData[index].Times == GifWndData[index].Count && 
             GifWndData[index].Times != 0)
         {
             if (GifWndData[index].pfnPaintBkFunc)
                GifWndData[index].pfnPaintBkFunc((HGIFANIMATE)&GifWndData[index], TRUE, 
                    0, 0, NULL);
             EndAnimatedGIF((HGIFANIMATE)&GifWndData[index]);
             return;
         }
    }
    else if (GifWndData[index].dm == DM_BITMAP)  /* DM_BITMAP方式 */
    {
        struct gif_frame *frame;
        PHEADOFBITMAP pTmp;

        pTmp = (PHEADOFBITMAP)GifWndData[index].pCur;
        frame = &pTmp->frame;
        if (((PHEADOFDATA)GifWndData[index].pHead)->pNext == GifWndData[index].pHead)
        {
            if (GifWndData[index].pfnPaintBkFunc)
                GifWndData[index].pfnPaintBkFunc((HGIFANIMATE)&GifWndData[index], TRUE, 
                    0, 0, NULL);
             EndAnimatedGIF((HGIFANIMATE)&GifWndData[index]);
             return;
        }
        hdc = GetDC(GifWndData[index].hwndParent);

         /* 需要恢复为背景颜色或原来的图象 */
        if (((PHEADOFBITMAP)GifWndData[index].pCur)->bRect || GifWndData[index].pCur == GifWndData[index].pHead) 
        {
            RECT rect1, rect2;

            rect1 = ((PHEADOFBITMAP)GifWndData[index].pCur)->rect;
            rect2.left = GifWndData[index].x + rect1.left;
            rect2.right = GifWndData[index].x + rect1.right;
            rect2.top = GifWndData[index].y + rect1.top;
            rect2.bottom = GifWndData[index].y + rect1.bottom;

            InvalidateRect(GifWndData[index].hwndParent, &rect2, TRUE);
            GifWndData[index].bRepaint = FALSE;
            UpdateWindow(GifWndData[index].hwndParent);
            GifWndData[index].bRepaint = TRUE;
        }

        if (frame->flags & GIF_TRANSPARENT)       /* 透明显示 */
        {      
            int nTrans = ((PHEADOFBITMAP)GifWndData[index].pCur)->clrIndex;
            COLORREF oldColor;

            nOldBkMode = SetBkMode(hdc, NEWTRANSPARENT);
            oldColor = SetBkColor(hdc, (1 << 24) | nTrans);
   
          //oldColor = SetBkColor(hdc, RGB(color.rgbRed, color.rgbGreen, color.rgbBlue));
          
            if (GifWndData[index].DstHeight == 0 && GifWndData[index].DstWidth == 0)
            {
                BitBlt(hdc, GifWndData[index].x + frame->left, 
                    GifWndData[index].y + frame->top, 
                    frame->width, frame->height, 
                    (HDC)((PHEADOFBITMAP)GifWndData[index].pCur)->hBitmap,
                    0, 0, SRCCOPY);
            }
            else
            {
                StretchBlt(hdc, GifWndData[index].x + frame->left * GifWndData[index].DstWidth / GifWndData[index].width, 
                    GifWndData[index].y + frame->top * GifWndData[index].DstHeight / GifWndData[index].height, 
                    frame->width * GifWndData[index].DstWidth / GifWndData[index].width, frame->height * GifWndData[index].DstHeight / GifWndData[index].height,
                    (HDC)((PHEADOFBITMAP)GifWndData[index].pCur)->hBitmap,
                    0, 0, frame->width, frame->height, SRCCOPY);
            }
            SetBkColor(hdc, oldColor);
            SetBkMode(hdc, nOldBkMode);       
        }
        else
        {
            if (GifWndData[index].DstHeight == 0 && GifWndData[index].DstWidth == 0)
            {
                BitBlt(hdc, GifWndData[index].x + frame->left, 
                    GifWndData[index].y + frame->top, 
                    frame->width, frame->height, 
                    (HDC)((PHEADOFBITMAP)GifWndData[index].pCur)->hBitmap,
                    0, 0, SRCCOPY);
            }
            else
            {
                StretchBlt(hdc, GifWndData[index].x + frame->left * GifWndData[index].DstWidth / GifWndData[index].width, 
                    GifWndData[index].y + frame->top * GifWndData[index].DstHeight / GifWndData[index].height, 
                    frame->width * GifWndData[index].DstWidth / GifWndData[index].width, frame->height * GifWndData[index].DstHeight / GifWndData[index].height,
                    (HDC)((PHEADOFBITMAP)GifWndData[index].pCur)->hBitmap,
                    0, 0, frame->width, frame->height, SRCCOPY);
            }
        }
        ReleaseDC(GifWndData[index].hwndParent, hdc);

        /* 读取下一帧 */
        GifWndData[index].pCur = ((PHEADOFBITMAP)GifWndData[index].pCur)->pNext;

        /* 全部帧读完一遍时增加遍数记录 */
         if(GifWndData[index].pCur == GifWndData[index].pHead )
            GifWndData[index].Count ++;

        /* 达到指定显示次数，结束显示 */
         if (GifWndData[index].Times == GifWndData[index].Count &&
             GifWndData[index].Times != 0)
         {
            if (GifWndData[index].pfnPaintBkFunc)
                GifWndData[index].pfnPaintBkFunc((HGIFANIMATE)&GifWndData[index], TRUE, 
                    0, 0, NULL);
             EndAnimatedGIF((HGIFANIMATE)&GifWndData[index]);
             return;
         }
    }
}

/*********************************************************************\
* Function	     : PaintGifImage
* Purpose        :
*     paint  gif image according to gif data
* Params	     :
*     pGifWndData: gif data
*     bPaint     : paint flag
* Return	 	 :
*     if function success, return 1;
*     otherwise return 0 
**********************************************************************/
int PaintGifImage(HDC hMemDC, PGIFWNDDATA pGifWndData, BOOL bPaint, BOOL bTimer)
{
    int                 scanlines, y_start, nOldBkMode;
    int	                i, retval, count, line_w;
    unsigned char       * ptr, * pImgBuffer;
    HDC					hdc;
    RECT				rc;
    int                 nTrans = -1;
    int                 x, y;
    
    if (pGifWndData->hwndParent == (HWND)-1)
    {
        x = pGifWndData->x;
        y = pGifWndData->y;
        hdc = pGifWndData->hCompatibleDC;
    }
    else if (hMemDC)
    {
        x = pGifWndData->x;
        y = pGifWndData->y;
        hdc = hMemDC;
    }
    else if (!pGifWndData->pfnPaintBkFunc)
    {
        x = pGifWndData->x;
        y = pGifWndData->y;
        hdc = GetDC(pGifWndData->hwndParent);
    }
    else
    {
        x = 0;
        y = 0;
        hdc = pGifWndData->hCompatibleDC;
    }
    
    pGifWndData->bRepaint = TRUE;
    scanlines = pGifWndData->frame.height;
    
    /* 图象透明 */
    if (pGifWndData->frame.flags & GIF_TRANSPARENT)
    {
        if (pGifWndData->frame.transparent >= 0 && pGifWndData->frame.transparent <= 255)
            nTrans = pGifWndData->frame.transparent;
    }
    
    line_w = (pGifWndData->frame.width + 3) & ~3;
    
    /* Make line table */
    pImgBuffer = ptr = (unsigned char *) malloc (line_w * scanlines);
    if ( ptr == NULL)
        return 0;
    
    if ( pGifWndData->frame.flags & GIF_INTERLACE)  /* 隔行扫描 */
    {
        int row, num, k;
        
        for ( i = 0, k = 0, num = 0; i < min(scanlines, MAX_SCANLINES); num ++)
        {
            row = scan_offset[k] + scan_distence[k] * num;
            if ( row < min(scanlines, MAX_SCANLINES))
                LineTable[i ++] = ptr + row * line_w;
            else
            {
                num = -1;
                k ++;
            }
        }
    }
    else                     /* 逐行扫描 */
    {
        for ( i = 0; i < min(scanlines, MAX_SCANLINES); i ++, ptr += line_w)
            LineTable[i] = ptr;
    }
    
    y_start = GIF_GetOutputScanline( pGifWndData->pGif);
    /* Read in scan line */
    while ( scanlines > 0)
    {
        count = (scanlines < MAX_SCANLINES) ? scanlines : MAX_SCANLINES;
        retval = GIF_ReadScanlines( pGifWndData->pGif, LineTable, count);
        if ( retval <= 0)
            break;
        
        /* 图象恢复为背景颜色或原来的内容 */
        if (((pGifWndData->LastMethod == 3) || (pGifWndData->LastMethod == 2))
            && !bPaint)
        {
            RECT rect;
            if (!pGifWndData->pfnPaintBkFunc)
            {
                if (pGifWndData->DstHeight == 0 && pGifWndData->DstWidth == 0)
                    SetRect(&rect, x + pGifWndData->LastX, 
                        y + y_start + pGifWndData->LastY, 
                        pGifWndData->LastWidth + x + pGifWndData->LastX,
                        pGifWndData->LastHeight + y + 
                        y_start + pGifWndData->LastY);
                else
                    SetRect(&rect, x + pGifWndData->LastX * 
                        pGifWndData->DstWidth / pGifWndData->width,
                        y + (y_start + pGifWndData->LastY) * 
                        pGifWndData->DstHeight / pGifWndData->height, 
                        x + (pGifWndData->LastWidth + pGifWndData->LastX) 
                        * pGifWndData->DstWidth / pGifWndData->width,
                        y + (pGifWndData->LastHeight + y_start + 
                        pGifWndData->LastY)  * pGifWndData->DstHeight / 
                        pGifWndData->height);
                pGifWndData->bRepaint = FALSE;
                if (bTimer)
                {
                    InvalidateRect(pGifWndData->hwndParent, &rect,TRUE);
                    UpdateWindow(pGifWndData->hwndParent);
                }
                SetBitmapInfo(pGifWndData);
                pGifWndData->bRepaint = TRUE;
            }
            else
            {
                pGifWndData->pfnPaintBkFunc((HGIFANIMATE)pGifWndData, FALSE, 
                    x + pGifWndData->LastX, 
                    y + y_start + pGifWndData->LastY, 
                    pGifWndData->hCompatibleDC);
            }
            
        }
        
        pGifWndData->LastMethod = pGifWndData->frame.disposal_method;
        pGifWndData->LastX = pGifWndData->frame.left;
        pGifWndData->LastY = pGifWndData->frame.top;
        pGifWndData->LastWidth = pGifWndData->frame.width;
        pGifWndData->LastHeight = pGifWndData->frame.height;
        
        /* 透明颜色 */
        if ((pGifWndData->frame.flags & GIF_TRANSPARENT) && (nTrans >= 0) && (nTrans <= 255))
        {   
            COLORREF oldColor;
            
            nOldBkMode = SetBkMode(hdc, NEWTRANSPARENT);
            /*oldColor = SetBkColor(hdc, RGB(bmp.m_bmpinfo.bmiColors[nTrans].rgbRed,
            bmp.m_bmpinfo.bmiColors[nTrans].rgbGreen,
            bmp.m_bmpinfo.bmiColors[nTrans].rgbBlue));*/
            oldColor = SetBkColor(hdc, (1 << 24) | nTrans);
            if (!pGifWndData->pfnPaintBkFunc)
            {
                if (pGifWndData->DstHeight == 0 && pGifWndData->DstWidth == 0)
                {
                    SetDIBitsToDevice(hdc, x + pGifWndData->frame.left,
                        y + pGifWndData->frame.top + y_start, 
                        bmp.m_bmpinfo.bmiHeader.biWidth, retval,
                        0, 0, 0, retval, pImgBuffer, &bmp.m_bmpinfo, 0);
                }
                else
                {
                    StretchDIBits(hdc, 
                        x + pGifWndData->frame.left * pGifWndData->DstWidth / pGifWndData->width,
                        y + (pGifWndData->frame.top + y_start) * pGifWndData->DstHeight / pGifWndData->height,
                        bmp.m_bmpinfo.bmiHeader.biWidth * pGifWndData->DstWidth / pGifWndData->width, retval * pGifWndData->DstHeight / pGifWndData->height,
                        0, 0, bmp.m_bmpinfo.bmiHeader.biWidth, retval,pImgBuffer, 
                        &bmp.m_bmpinfo, DIB_RGB_COLORS, SRCCOPY);
                }
            }
            else
            {
                if (pGifWndData->DstHeight == 0 && pGifWndData->DstWidth == 0)
                {
                    SetDIBitsToDevice(hdc, x + pGifWndData->frame.left,
                        y + pGifWndData->frame.top + y_start, 
                        bmp.m_bmpinfo.bmiHeader.biWidth, retval,
                        0, 0, 0, retval, pImgBuffer, &bmp.m_bmpinfo, 0);
                }
                else
                {
                    StretchDIBits(hdc, x + pGifWndData->frame.left
                        * pGifWndData->DstWidth / pGifWndData->width,
                        y + (pGifWndData->frame.top + y_start) * pGifWndData->DstHeight / pGifWndData->height,
                        bmp.m_bmpinfo.bmiHeader.biWidth * pGifWndData->DstWidth / pGifWndData->width, retval * pGifWndData->DstHeight / pGifWndData->height,
                        0, 0, bmp.m_bmpinfo.bmiHeader.biWidth, retval,pImgBuffer, 
                        &bmp.m_bmpinfo, DIB_RGB_COLORS, SRCCOPY);
                }
            }
            SetBkColor(hdc, oldColor);
            SetBkMode(hdc, nOldBkMode);            
        }
        else
        { 
            if (!pGifWndData->pfnPaintBkFunc)
            {
                if (pGifWndData->DstHeight == 0 && pGifWndData->DstWidth == 0)
                {					
                    SetDIBitsToDevice(hdc, x + pGifWndData->frame.left,
                        y + pGifWndData->frame.top + y_start, 
                        bmp.m_bmpinfo.bmiHeader.biWidth, retval,
                        0, 0, 0, retval, pImgBuffer, &bmp.m_bmpinfo, 0);
                }
                else
                {
                    StretchDIBits(hdc, 
                        x + pGifWndData->frame.left * pGifWndData->DstWidth / pGifWndData->width,
                        y + (pGifWndData->frame.top + y_start) * pGifWndData->DstHeight / pGifWndData->height,
                        bmp.m_bmpinfo.bmiHeader.biWidth * pGifWndData->DstWidth / pGifWndData->width, retval * pGifWndData->DstHeight / pGifWndData->height,
                        0, 0, bmp.m_bmpinfo.bmiHeader.biWidth, retval,pImgBuffer, 
                        &bmp.m_bmpinfo, DIB_RGB_COLORS, SRCCOPY);
                    
                }
            }
            else
            {
                if (pGifWndData->DstHeight == 0 && pGifWndData->DstWidth == 0)
                {					
                    SetDIBitsToDevice(hdc, x + pGifWndData->frame.left,
                        y + pGifWndData->frame.top + y_start, 
                        bmp.m_bmpinfo.bmiHeader.biWidth, retval,
                        0, 0, 0, retval, pImgBuffer, &bmp.m_bmpinfo, 0);
                }
                else
                {
                    StretchDIBits(hdc, x + pGifWndData->frame.left * pGifWndData->DstWidth / pGifWndData->width,
                        y + (pGifWndData->frame.top + y_start) * pGifWndData->DstHeight / pGifWndData->height,
                        bmp.m_bmpinfo.bmiHeader.biWidth * pGifWndData->DstWidth / pGifWndData->width, retval * pGifWndData->DstHeight / pGifWndData->height,
                        0, 0, bmp.m_bmpinfo.bmiHeader.biWidth, retval,pImgBuffer, 
                        &bmp.m_bmpinfo, DIB_RGB_COLORS, SRCCOPY);
                }
            }
        }
        //UPDATESCREEN;
        scanlines -= retval;
        y_start += retval;
    }
    
    free (pImgBuffer);
    if (pGifWndData->hwndParent == (HWND)-1)
         UpdateScreen();
    else if (hMemDC)
        UpdateScreen();
    else if (!pGifWndData->pfnPaintBkFunc)
    {
        UpdateScreen();
        ReleaseDC(pGifWndData->hwndParent, hdc);
    }
    else
    {
        if (bTimer)
        {
            if (pGifWndData->DstHeight == 0 && pGifWndData->DstWidth == 0)
                SetRect(&rc, pGifWndData->x, pGifWndData->y, 
                pGifWndData->x + pGifWndData->width, pGifWndData->y + pGifWndData->height);
            else
                SetRect(&rc, pGifWndData->x, pGifWndData->y, 
                pGifWndData->x + pGifWndData->DstWidth, pGifWndData->y + pGifWndData->DstHeight);
            InvalidateRect(pGifWndData->hwndParent, &rc, TRUE);
            UpdateWindow(pGifWndData->hwndParent);
            UpdateScreen();
        }
        
    }
    
    
    return 1;
}

/*********************************************************************\
* Function	     : DecodingToData
* Purpose        :
*     Decode animated gif object to data 
* Params	     :
*    pGifWndData : animated gif object to be decoded to data

  * Return	 	 :
*    If function success, return 1;
*    otherwise return -1
**********************************************************************/
static int DecodingToData(PGIFWNDDATA pGifWndData)
{
    int                 ret;
    int                 nFrame = 1;
    int                 scanlines, y_start;
    int	                i, retval, count, line_w;
    unsigned char       * ptr, * pImgBuffer;
    int                 nTrans = -1;
    PHEADOFDATA  pCur, pLast;

    pGifWndData->frame = m_frame;

    if (pGifWndData->nframes == 1)  /* 总共只有一帧 */
    {
        return 1;
    }
    while (TRUE)
    {
        if (pGifWndData->frame.flags & GIF_TRANSPARENT)
        {
            if (pGifWndData->frame.transparent >= 0 && pGifWndData->frame.transparent <= 255)
                nTrans = pGifWndData->frame.transparent;
        }

        scanlines = pGifWndData->frame.height;
        line_w = (pGifWndData->frame.width + 3) & ~3;

        /* Make line table */
        pImgBuffer = ptr = (unsigned char *) malloc (line_w * scanlines);
        if ( pImgBuffer == NULL)
            return -1;

        if ( pGifWndData->frame.flags & GIF_INTERLACE) /* 隔行扫描 */
        {
            int row, num, k;
            
            for ( i = 0, k = 0, num = 0; i < min(scanlines, MAX_SCANLINES); num ++)
            {
                row = scan_offset[k] + scan_distence[k] * num;
                if ( row < min(scanlines, MAX_SCANLINES))
                    LineTable[i ++] = ptr + row * line_w;
                else
                {
                    num = -1;
                    k ++;
                }
            }
        }
        else        /* 逐行扫描 */
        {
            for ( i = 0; i < min(scanlines, MAX_SCANLINES); i ++, ptr += line_w)
                LineTable[i] = ptr;
        }

        while (scanlines > 0)
        {
            count = (scanlines < MAX_SCANLINES) ? scanlines : MAX_SCANLINES;
            retval = GIF_ReadScanlines( pGifWndData->pGif, LineTable, count);
            if ( retval <= 0)
                break;

            scanlines -= retval;
            y_start += retval;
        }

        pCur = (PHEADOFDATA)malloc(sizeof(HEADOFDATA));
        if (pCur == NULL)
        {
            free(pImgBuffer);
            pCur = pGifWndData->pHead;
            while (pCur != NULL)
            {
                free(pCur);
                pCur = pCur->pNext;
            }
        }

        pCur->pNext = NULL;
        pCur->pData = pImgBuffer;
        pCur->frame = pGifWndData->frame;
        pCur->bmp   = bmp;

        /* 图象恢复为背景色或恢复原来图象，需要重画该区域 */
        if ((pGifWndData->LastMethod == 3) || (pGifWndData->LastMethod == 2))
        {
            pCur->bRect = TRUE;
            if (pGifWndData->DstHeight == 0 && pGifWndData->DstWidth == 0)
            {
                SetRect(&pCur->rect, pGifWndData->LastX, pGifWndData->LastY, 
                    pGifWndData->LastWidth + pGifWndData->LastX,
                    pGifWndData->LastHeight + pGifWndData->LastY);
            }
            else
            {
                SetRect(&pCur->rect, 
                    pGifWndData->LastX * pGifWndData->DstWidth / pGifWndData->width, 
                    pGifWndData->LastY * pGifWndData->DstHeight / pGifWndData->height, 
                    (pGifWndData->LastWidth + pGifWndData->LastX) * pGifWndData->DstWidth / pGifWndData->width,
                    (pGifWndData->LastHeight + pGifWndData->LastY) * pGifWndData->DstHeight / pGifWndData->height);
            }

        }
        else if (pGifWndData->pHead == NULL)
        {
            pCur->bRect = TRUE;
            if (pGifWndData->DstHeight == 0 && pGifWndData->DstWidth == 0)
            {
                SetRect(&pCur->rect, 0, 0, pGifWndData->width, pGifWndData->height);
            }
            else
            {
                SetRect(&pCur->rect, 0, 0, pGifWndData->DstWidth,
                    pGifWndData->DstHeight);
            }
        }
        else
            pCur->bRect = FALSE;

         pGifWndData->LastMethod = pGifWndData->frame.disposal_method;
         pGifWndData->LastX = pGifWndData->frame.left;
         pGifWndData->LastY = pGifWndData->frame.top;
         pGifWndData->LastWidth = pGifWndData->frame.width;
         pGifWndData->LastHeight = pGifWndData->frame.height;

        if (pGifWndData->pHead == NULL)
            pGifWndData->pHead = pGifWndData->pCur = (void *)pCur;
        else
            pLast->pNext = pCur;
        pLast = pCur;
        pCur->pNext = NULL; 

        pGifWndData->nframes ++;

        ret = GIF_ReadFrameHeader(pGifWndData->pGif , 0);
        if (ret <= 0)
            break;
 
        GIF_GetFrameDimensions(pGifWndData->pGif, &pGifWndData->frame);
        
        GIF_SetColorMode(pGifWndData->pGif, GCS_RAW);

        SetBitmapInfo(pGifWndData);
    }

    pCur->pNext = pGifWndData->pHead;
    return 1;
}

/*********************************************************************\
* Function	     : DecodingToBitmap
* Purpose        :
*     Decode animated gif object to bitmap 
* Params	     :
*    pGifWndData : animated gif object to be decoded to bitmap
* Return	 	 :
*    If function success, return 1;
*    otherwise return -1
**********************************************************************/
static int DecodingToBitmap(PGIFWNDDATA pGifWndData)
{
    int                     ret;
    int                     nFrame = 1;
    int                     scanlines, y_start;
    int	                    i, retval, count, line_w;
    unsigned char           * ptr, * pImgBuffer;
    unsigned char           * LineTable[MAX_SCANLINES];
    int                     nTrans = -1;
    PHEADOFBITMAP           pCur, pLast;
    HBITMAP                 hBitmap;
    HDC                     hdc;

    pGifWndData->frame = m_frame;

    while (TRUE)
    {
        scanlines = pGifWndData->frame.height;
        line_w = (pGifWndData->frame.width + 3) & ~3;

        nTrans = -1;
        if (pGifWndData->frame.flags & GIF_TRANSPARENT)
        {
            if (pGifWndData->frame.transparent >= 0 && pGifWndData->frame.transparent <= 255)
                nTrans = pGifWndData->frame.transparent;
        }
        /* Make line table */
        pImgBuffer = ptr = (unsigned char *) malloc (line_w * scanlines);
        if ( pImgBuffer == NULL)
            return -1;

        if ( pGifWndData->frame.flags & GIF_INTERLACE)   /* 隔行扫描 */
        {
            int row, num, k;
            
            for ( i = 0, k = 0, num = 0; i < min(scanlines, MAX_SCANLINES); num ++)
            {
                row = scan_offset[k] + scan_distence[k] * num;
                if ( row < min(scanlines, MAX_SCANLINES))
                    LineTable[i ++] = ptr + row * line_w;
                else
                {
                    num = -1;
                    k ++;
                }
            }
        }
        else              /* 逐行扫描 */
        {
            for ( i = 0; i < min(scanlines, MAX_SCANLINES); i ++, ptr += line_w)
                LineTable[i] = ptr;
        }

        while (scanlines > 0)
        {
            count = (scanlines < MAX_SCANLINES) ? scanlines : MAX_SCANLINES;
            retval = GIF_ReadScanlines( pGifWndData->pGif, LineTable, count);
            if ( retval <= 0)
                break;
            scanlines -= retval;
            y_start += retval;
        }
        pCur = (PHEADOFBITMAP)malloc(sizeof(HEADOFBITMAP));
        if (pCur == NULL)
        {
            free(pImgBuffer);
            pCur = pGifWndData->pHead;
            while (pCur != NULL)
            {
                free(pCur);
                pCur = pCur->pNext;
            }
        }
        pCur->pNext = NULL;
        pCur->pData = pImgBuffer;
        pCur->frame = pGifWndData->frame;
        
        /* 设置透明时的背景颜色 */        
        if (pGifWndData->frame.flags & GIF_TRANSPARENT &&
            (nTrans >= 0) && (nTrans <= 255))   
        {
            pCur->clrIndex = nTrans;
        }

        hdc = GetDC(pGifWndData->hwndParent);
        hBitmap = CreateDIBitmap(hdc, &bmp.m_bmpinfo.bmiHeader, CBM_INIT, 
                pImgBuffer, &bmp.m_bmpinfo, DIB_RGB_COLORS);
        ReleaseDC(pGifWndData->hwndParent, hdc);

        pCur->hBitmap = hBitmap;

         /* 图象恢复为背景色或恢复原来图象，需要重画该区域 */
        if ((pGifWndData->LastMethod == 3) || (pGifWndData->LastMethod == 2))
        {
            pCur->bRect = TRUE;
            if (pGifWndData->DstHeight == 0 && pGifWndData->DstWidth == 0)
            {
                SetRect(&pCur->rect, pGifWndData->LastX, pGifWndData->LastY, 
                    pGifWndData->LastWidth + pGifWndData->LastX,
                    pGifWndData->LastHeight + pGifWndData->LastY);
            }
            else
            {
                SetRect(&pCur->rect, 
                    pGifWndData->LastX * pGifWndData->DstWidth / pGifWndData->width, 
                    pGifWndData->LastY * pGifWndData->DstHeight / pGifWndData->height, 
                    (pGifWndData->LastWidth + pGifWndData->LastX) * pGifWndData->DstWidth / pGifWndData->width,
                    (pGifWndData->LastHeight + pGifWndData->LastY) * pGifWndData->DstHeight / pGifWndData->height);
            }

        }
        else if (pGifWndData->pHead == NULL)
        {
            pCur->bRect = TRUE;
            if (pGifWndData->DstHeight == 0 && pGifWndData->DstWidth == 0)
            {
                SetRect(&pCur->rect, 0, 0, pGifWndData->width, pGifWndData->height);
            }
            else
            {
                SetRect(&pCur->rect, 0, 0, pGifWndData->DstWidth,
                    pGifWndData->DstHeight);
            }
        }
        
         pGifWndData->LastMethod = pGifWndData->frame.disposal_method;
         pGifWndData->LastX = pGifWndData->frame.left;
         pGifWndData->LastY = pGifWndData->frame.top;
         pGifWndData->LastWidth = pGifWndData->frame.width;
         pGifWndData->LastHeight = pGifWndData->frame.height;

        if (pGifWndData->pHead == NULL)
            pGifWndData->pHead = pGifWndData->pCur = (void *)pCur;
        else
            pLast->pNext = pCur;

        pLast = pCur;

        ret = GIF_ReadFrameHeader(pGifWndData->pGif , 0);
        if (ret <= 0)
            break;
        pGifWndData->nframes ++;
        GIF_GetFrameDimensions(pGifWndData->pGif, &(pGifWndData->frame));
        GIF_SetColorMode(pGifWndData->pGif, GCS_RAW);
        SetBitmapInfo(pGifWndData);
    }
    pCur->pNext = pGifWndData->pHead;
    return 1;
}

/*********************************************************************\
* Function	   : BMPToGifFile
* Purpose      :
*     save bitmap data to gif file
* Params	   :
*     hGif     : the handle of gif file to be saved
*     ImageBuffer : the pointer of bitmap data
* Return	   :
*     if success to save data, return TRUE;
*     otherwise return FALSE.	   
* Remarks	   :
*     only support the bitmap in 256 colors
**********************************************************************/
BOOL  BMPToGifFile(int hGif, char *ImageBuffer)
{
    int	i, nOffBit, Size;
    DWORD ImgWidth,ImgHeight;
    WORD  nBitCount;
    GifColorType  *pPalette = NULL;
    char *ptr, *LineTable[800];
    GifFileType *GifFileOut = NULL;
  
    if ((GifFileOut = EGifOpenFileHandle(hGif)) == NULL)
        return FALSE;
      
    nOffBit = GET32(ImageBuffer + 10);
    ImgWidth = GET32(ImageBuffer + 18);
    ImgHeight= GET32(ImageBuffer + 22); 
    nBitCount = GET16(ImageBuffer + 28);
  
    if (nBitCount == 8)
    {      
        pPalette = (GifColorType *) 
                malloc(sizeof(GifColorType) * (1<<nBitCount));
        if(!pPalette)
        {
            EGifCloseFile(GifFileOut);
            return FALSE;
        }

        ptr = ImageBuffer + 54;
        for(i=0;i<(1<<nBitCount);i++, ptr += 4)  // read bitmap pixels array to buffer 
        {
            pPalette[i].Red = *(ptr+2);
            pPalette[i].Green = *(ptr+1);
            pPalette[i].Blue = *ptr;            
        }       
    }    
    else               
    {
        EGifCloseFile(GifFileOut);
        return FALSE;    
    }
    /* And dump out its new scaled screen information: */
    if (EGifPutScreenDesc(GifFileOut,
	       ImgWidth, ImgHeight,
	        nBitCount,  //SColorResolution, 
            0, // SBackGroundColor,
	        nBitCount, // SBitsPerPixel,
            pPalette) == ERROR)
    {
        EGifCloseFile(GifFileOut);
        return FALSE;
    }

    /* Scan the content of the GIF file and load the image(s) in: */
    if(EGifPutImageDesc(GifFileOut,
           0, 0, //left, top
	       ImgWidth, ImgHeight,
           0, //IInterlace        
           nBitCount, // SBitsPerPixel,
           0) //IColorMap  
           == ERROR)
    {       
        EGifCloseFile(GifFileOut);
        return FALSE;
    }

    ptr = (char*)(ImageBuffer + nOffBit);

    Size = (sizeof(PixelType) * GifFileOut->IWidth + 3) & ~3;      
       
    for ( i = 0; i < GifFileOut->IHeight; i ++, ptr += Size)
        LineTable[i] = ptr;
    
    for (i= GifFileOut->IHeight - 1; i>=0; i--) 
    {       			
        if (EGifPutLine(GifFileOut, LineTable[i], GifFileOut->IWidth) == ERROR)
        {   
            EGifCloseFile(GifFileOut);
            return  FALSE;
        }
    }
    EGifCloseFile(GifFileOut);
    if (pPalette)
    {
        free(pPalette);
        pPalette = NULL;
    }
    return TRUE;
}
