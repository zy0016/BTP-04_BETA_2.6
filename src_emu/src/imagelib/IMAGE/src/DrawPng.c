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

#include "window.h"

#include "png/png.h"
#include "drawimg.h"
#include "filesys.h"

#define DEMOCLSNAME "pngdemoclass"
#define DEMOWNDNAME "png demo window"




static BOOL LoadPNG(PCSTR pngData, PNGDIB* pDib);
/*
  convert image object to HBITMAP;
  release HBITMAP use DeleteObject;
*/
static BYTE* PNGDIBToBitmap(PNGDIB* pDib, BITMAPINFO *bitmapInfo);

void FreePNGDIB(PNGDIB *pDib);

PPHYIMG  CreatePngFromData(struct data_buffer pData, int hFile)
{
	PPHYIMG pDib;
   
    pDib = (PPHYIMG)malloc(sizeof(PNGDIB) + sizeof(DWORD) + sizeof(int));
    if (!pDib)
       return FALSE;
    memset(pDib, 0, sizeof(PNGDIB) + sizeof(DWORD) + sizeof(int));
	ConvertImageData(pData.data_count, ((BYTE *)pDib) + sizeof(PNGDIB), hFile);

    if (!LoadPNG(pData.data, (PPNGDIB)pDib))
	{
		FreePNGDIB(pDib);
		return NULL;
	}	 
	return pDib;
}

BOOL DeletePng(PPHYIMG pImg, BOOL bMapFile)
{
     PPNGDIB pDib= (PPNGDIB)pImg;
	 BYTE    *pData = (BYTE*)pImg;
	 int     hFile;
     
	 hFile = *(int*)(pData + sizeof(PNGDIB) + sizeof(DWORD));
	 if (hFile)
	 {
		 if (bMapFile)
		 {        
			 DWORD dwFileSize;
			 dwFileSize = IMGOS_GetFileSize(hFile);
			 IMGOS_UnmapFile(hFile, pDib->io_str, dwFileSize); 
		 }
		 else
			free(pDib->io_str);
        IMGOS_CloseFile(hFile);
    }
    FreePNGDIB(pDib);
    return TRUE;
}

int PaintPng(HDC hdc, int nDestX, int nDestY, int nWidth, int nHeight,
              PPHYIMG pPhyImg, int nSrcX, int nSrcY, DWORD dwRop)
{	
    PPNGDIB pDib = (PPNGDIB)pPhyImg;
	BITMAPINFO bitmapInfo;
	BYTE*   pvBits = NULL;
    int nBkmode;
	
	pvBits = PNGDIBToBitmap(pDib, &bitmapInfo);
	if (!pvBits)
		return 0;

	if (pDib->nChannel == 4)
		nBkmode = SetBkMode(hdc, BM_ALPHATRANSPARENT);

    if (nWidth == pDib->nWidth && nHeight == pDib->nHeight)
		SetDIBitsToDevice(hdc, nDestX, nDestY, nWidth, nHeight,                
            nSrcX, nSrcY, 0, pDib->nHeight, pvBits, &bitmapInfo, DIB_RGB_COLORS);
	else
		StretchDIBits(hdc, nDestX, nDestY, nWidth, nHeight,
			nSrcX, nSrcY, pDib->nWidth - nSrcX, pDib->nHeight - nSrcY,
			pvBits, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY); 
	
	if (pDib->nChannel == 4)
		SetBkMode(hdc, nBkmode);

    //free(pvBits);
    return 1;
}

BOOL GetPngDimension(PPHYIMG pImg, PSIZE pSize)
{
	PPNGDIB pDib = (PPNGDIB)pImg;
	
	if (!pSize || !pDib)	
		return FALSE;
	
	pSize->cx = pDib->nWidth;
	pSize->cy = pDib->nHeight;

	return TRUE;
}

BOOL DrawPNGFromData(HDC hdc, PCSTR pPngData, int x, int y, 
					 int width, int height, DWORD dwRop)
{
    PPNGDIB pDib;
    BYTE    *pvBits = NULL;
	BITMAPINFO bitmapInfo;
    int nBkmode;

    pDib = (PPNGDIB)malloc(sizeof(PNGDIB));
    if (!pDib)
       return FALSE;
    memset(pDib, 0, sizeof(PNGDIB));

    if (!LoadPNG(pPngData, pDib))
	{
		FreePNGDIB(pDib);
		return FALSE;
	}
       
    pvBits = PNGDIBToBitmap(pDib, &bitmapInfo);
    if (!pvBits)
	{
		FreePNGDIB(pDib);
        return FALSE;
	}
	if (pDib->nChannel == 4)
		nBkmode = SetBkMode(hdc, BM_ALPHATRANSPARENT);

    if (width == 0 && height == 0)
	{
		SetDIBitsToDevice(hdc, x, y, pDib->nWidth, pDib->nHeight,                
            0, 0, 0, pDib->nHeight, pvBits, &bitmapInfo, DIB_RGB_COLORS);
	}
	else
	{
		StretchDIBits(hdc, x, y, width, height,
            0, 0, pDib->nWidth, pDib->nHeight,
		    pvBits, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY); 
	}
	if (pDib->nChannel == 4)
		SetBkMode(hdc, nBkmode);

    //free(pvBits);
    FreePNGDIB(pDib);

    return TRUE;
}

BOOL DrawPNGFromFile(HDC hdc, PCSTR pszImgFile, int x, int y,
					 int width, int height, DWORD dwRop)
{
    char *pngData;
    int   hFile;
	DWORD dwFileSize;
    BOOL  bMapFile = TRUE, bRet;

	hFile = IMGOS_CreateFile(pszImgFile, 1, 0);
	if (hFile == -1)
		return FALSE;
	dwFileSize = IMGOS_GetFileSize(hFile);
	pngData = IMGOS_MapFile(hFile, 0, dwFileSize);
	if (!pngData)
	{
		bMapFile = FALSE;
		pngData = (char*)malloc(dwFileSize);
		if (!pngData)
		{
			IMGOS_CloseFile(hFile);
			return FALSE;
		}
        IMGOS_ReadFile(hFile, pngData, dwFileSize);
	}
    
    bRet = DrawPNGFromData(hdc, pngData, x, y, width, height, dwRop);
	if (bMapFile)
		IMGOS_UnmapFile(hFile, pngData, dwFileSize);
	else
		free(pngData);
	IMGOS_CloseFile(hFile);
	return bRet;
}

HBITMAP CreateBitmapFromPNG(PCSTR pImgData)
{
    static PNGDIB *pDib;
    HBITMAP hBitmap;
	HDC     hMemDC;
	BYTE*   pvBits;
	BITMAPINFO bitmapInfo;
    
	pDib = (PPNGDIB)malloc(sizeof(PNGDIB));
	if (!pDib)
        return NULL;
    memset(pDib, 0, sizeof(PNGDIB));
    
    if (!LoadPNG(pImgData, pDib))
	{
		FreePNGDIB(pDib);
		return NULL;
    }

    pvBits = PNGDIBToBitmap(pDib, &bitmapInfo);
    if (!pvBits)
	{
		FreePNGDIB(pDib);
		return NULL;
    }

    hMemDC = CreateCompatibleDC(NULL);
    hBitmap = CreateCompatibleBitmap(hMemDC, pDib->nWidth, pDib->nHeight);
    if (!hBitmap)
	{		
        //free(pvBits);
		FreePNGDIB(pDib);
		return NULL;
	}
    SelectObject(hMemDC, hBitmap);

    SetDIBitsToDevice(hMemDC, 0, 0, pDib->nWidth,
        pDib->nHeight, 0, 0, 0, pDib->nHeight, pvBits, &bitmapInfo, 0);

    DeleteDC(hMemDC);
	FreePNGDIB(pDib);
	//free(pvBits);

    return hBitmap;
}

BOOL GetPngDimensionFromData(PCSTR pImgData, PSIZE pSize)
{
	static PNGDIB *pDib;
    
    if (!pImgData || !pSize)
		return FALSE;

	pDib = (PPNGDIB)malloc(sizeof(PNGDIB));
	if (!pDib)
        return NULL;
    memset(pDib, 0, sizeof(PNGDIB));
    
    LoadPNG(pImgData, pDib);
	if (pDib->nWidth && pDib->nHeight)
	{
		pSize->cx = pDib->nWidth;
		pSize->cy = pDib->nHeight;
	}
	
	FreePNGDIB(pDib);
	return TRUE;
     
}
static void png_cexcept_error(png_structp png_ptr, png_const_charp msg)
{
   if(png_ptr)
     ;   
}

static BOOL LoadPNG(PCSTR pngData,  PNGDIB *pDib)
{
    int i;
   
    int nWidth;
    int nHeight;
    int nChannel;
	int nBitDepth;
	int nColorType;
    double dGamma;    
    png_byte *pData = NULL;
    png_byte **ppbRowPointers = NULL;
    png_uint_32 ulRowBytes;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;	
    png_bytep png_datap = (png_bytep)pngData;

    if (!pngData || !pDib)
        return FALSE;
  
    // first check the eight byte PNG signature
    if (!png_check_sig(png_datap, 8))  // 8 is signature
    {        
        return FALSE;
    }
    png_datap += 8;

    // create the two png(-info) structures

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
        (png_error_ptr)png_cexcept_error, (png_error_ptr)NULL);
    if (!png_ptr)
    {        
        return FALSE;
    }
#ifdef PNG_SETJMP_SUPPORTED
#ifdef USE_FAR_KEYWORD
    if (setjmp(jmpbuf))
#else
        if (setjmp(png_ptr->jmpbuf))
#endif
        {
            png_destroy_read_struct(&png_ptr, &info_ptr,
							NULL);

#ifdef PNG_USER_MEM_SUPPORTED
            png_destroy_struct_2((png_voidp)png_ptr, 
                (png_free_ptr)png_free_ptr_NULL,
                (png_voidp)png_voidp_NULL);
#else
            png_destroy_struct((png_voidp)png_ptr);
#endif
            return (NULL);
        }
#ifdef USE_FAR_KEYWORD
   png_memcpy(png_ptr->jmpbuf,jmpbuf,sizeof(jmp_buf));
#endif
#endif

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);     
        return FALSE;
    }

    // initialize the png structure
    
//#if !defined(PNG_NO_STDIO)	
    png_init_io(png_ptr, (png_FILE_p)png_datap);
//#else
//        png_set_read_fn(png_ptr, (png_voidp)pfFile, png_read_data);
//#endif
    
    png_set_sig_bytes(png_ptr, 8);
    
    // read all PNG info up to image data
    
    png_read_info(png_ptr, info_ptr);
    
    // get width, height, bit-depth and color-type
    
    png_get_IHDR(png_ptr, info_ptr,
				 (png_uint_32*)&nWidth,
				 (png_uint_32*)&nHeight,
				 &nBitDepth, &nColorType,
				 NULL, NULL, NULL);

    pDib->nWidth = nWidth;
    pDib->nHeight = nHeight;
    
    nChannel = png_get_channels(png_ptr, info_ptr);
    // expand images of all color-type and bit-depth to 3x8 bit RGB images
    // let the library process things like alpha, transparency, background
    
    if (nBitDepth == 16)
        png_set_strip_16(png_ptr);
    if (nColorType == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);
    if (nBitDepth < 8)
        png_set_expand(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);
    if (nColorType == PNG_COLOR_TYPE_GRAY ||
        nColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);


    // if required set gamma conversion
    if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
        png_set_gamma(png_ptr, (double) 2.2, dGamma);
    
    // after the transformations have been registered update info_ptr data
    
    png_read_update_info(png_ptr, info_ptr);
    
    // get again width, height and the new bit-depth and color-type
    
    png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)&nWidth, 
        (png_uint_32*)&nHeight, &nBitDepth,
        &nColorType, NULL, NULL, NULL);
    
    
    // row_bytes is the width x number of channels
    
    ulRowBytes = png_get_rowbytes(png_ptr, info_ptr);
    nChannel = png_get_channels(png_ptr, info_ptr);

    if (nChannel == 3)
        ulRowBytes = (ulRowBytes + 3) & ~3;
    // now we can allocate memory to store the image
    
    if ((pData = (png_byte *)malloc(ulRowBytes* (nHeight)
                        * sizeof(png_byte))) == NULL)
    {
        png_error(png_ptr, "Visual PNG: out of memory");
    }
    
    // and allocate memory for an array of row-pointers
    
    if ((ppbRowPointers = (png_bytepp) malloc((nHeight)
                        * sizeof(png_bytep))) == NULL)
    {
        png_error(png_ptr, "Visual PNG: out of memory");
    }
    
    // set the individual row-pointers to point at the correct offsets
    
    for (i = 0; i < (nHeight); i++)
        ppbRowPointers[i] = pData + i * ulRowBytes;
    
    // now we can go ahead and just read the whole image
    
    //png_ptr->rowbytes = ulRowBytes;
    //png_ptr->channels = nChannel;
    //png_ptr->width = nWidth;
    png_read_image(png_ptr, info_ptr, ppbRowPointers);
    
    // read the additional chunks in the PNG file (not really needed)
    
    png_read_end(png_ptr, NULL);
    
    // and we're done
    
    free(ppbRowPointers);
    ppbRowPointers = NULL;
    
    // yepp, done

    png_destroy_read_struct(&png_ptr, &info_ptr,
							NULL);
   
    pDib->nWidth = nWidth;
    pDib->nHeight = nHeight;
    pDib->nChannel = nChannel;
    pDib->nBitDepth = nBitDepth;
    pDib->nColorType = nColorType;
    pDib->data = pData;
    pDib->io_str = (char*)pngData;
	return TRUE;
}

/*
  convert image object to HBITMAP;
  release HBITMAP use DeleteObject;
*/
static BYTE* PNGDIBToBitmap(PNGDIB* pDib, BITMAPINFO *bitmapInfo)
{
//    int i, j;
//	BYTE *pSrc;
//    BYTE *pDest;
    int  nWidthBytes;
	BYTE *pvBits = NULL;
    
	if (!pDib || !bitmapInfo)
        return NULL;
	
	if (!pDib->data)
	{		
		return NULL;
	}

    memset(bitmapInfo, 0, sizeof(BITMAPINFO));
    bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo->bmiHeader.biWidth = pDib->nWidth;
    bitmapInfo->bmiHeader.biHeight = -pDib->nHeight;
    bitmapInfo->bmiHeader.biPlanes = 1;
	if (pDib->nChannel == 4)
		bitmapInfo->bmiHeader.biBitCount = 32;
	else
		bitmapInfo->bmiHeader.biBitCount = 24;
    bitmapInfo->bmiHeader.biCompression = 0;
    
    if (pDib->nChannel == 4)
		nWidthBytes = pDib->nWidth * 4;
	else
		nWidthBytes = (pDib->nWidth * 3 + 3) & ~3;
    
    return pDib->data;
    /*pvBits = (BYTE *)malloc(nWidthBytes * pDib->nHeight);
    if (!pvBits)
		return NULL;
    
    pSrc = pDib->data;
	if (pDib->nChannel == 4)
	{
		for (i = 0; i < pDib->nHeight; i++)
		{
			pDest = pvBits + i * nWidthBytes;
			for (j = 0; j < pDib->nWidth; j++)
			{
				*pDest++ = *(pSrc + 2);
				*pDest++ = *(pSrc + 1);
				*pDest++ = *pSrc;
				*pDest++ = *(pSrc + 3);				
				pSrc += pDib->nChannel;
			}
		}
	}
	else
	{
		for (i = 0; i < pDib->nHeight; i++)
		{
			pDest = pvBits + i * nWidthBytes;
			for (j = 0; j < pDib->nWidth; j++)
			{
				*pDest++ = *(pSrc + 2);
				*pDest++ = *(pSrc + 1);
				*pDest++ = *pSrc;
				pSrc += pDib->nChannel;
			}
		}
	}
	
    return pvBits;*/
}

static void FreePNGDIB(PNGDIB *pDib)
{
    if (!pDib)
        return;

    if (pDib->data)
        free(pDib->data);
    free(pDib);
}


BOOL    BMPToPngFile(int hPng, char *ImageBuffer)
{	
   int width, height, bit_depth, nPlane, offbit, color_type;
   int bits_pixel, bits_line;
   int i;
   png_structp png_ptr;
   png_infop info_ptr;
   png_colorp palette;
   png_text   text_ptr[3];
   png_bytep  pData;
   png_byte   **row_pointers = NULL;
   int        number_passes;
   
   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also check that
    * the library version is compatible with the one used at compile time,
    * in case we are using dynamically linked libraries.  REQUIRED.
    */

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
        NULL, (png_error_ptr)png_cexcept_error, NULL);

   if (png_ptr == NULL)
   {      
      return (ERROR);
   }
   #ifdef PNG_SETJMP_SUPPORTED
#ifdef USE_FAR_KEYWORD
    if (setjmp(jmpbuf))
#else
        if (setjmp(png_ptr->jmpbuf))
#endif
        {
            //png_free(png_ptr, png_ptr->zbuf);
            //png_ptr->zbuf=NULL;
			png_destroy_read_struct(&png_ptr, &info_ptr,
							NULL);
#ifdef PNG_USER_MEM_SUPPORTED
            png_destroy_struct_2((png_voidp)png_ptr, 
                (png_free_ptr)png_free_ptr_NULL,
                (png_voidp)png_voidp_NULL);
#else
            png_destroy_struct((png_voidp)png_ptr);
#endif
            return (NULL);
        }
#ifdef USE_FAR_KEYWORD
   png_memcpy(png_ptr->jmpbuf,jmpbuf,sizeof(jmp_buf));
#endif
#endif

   /* Allocate/initialize the image information data.  REQUIRED */
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {      
      png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
      return (ERROR);
   }

   /* One of the following I/O initialization functions is REQUIRED */
   /* set up the output control if you are using standard C streams */
   png_init_io(png_ptr, (png_FILE_p)hPng);

   /* Set the image information here.  Width and height are up to 2^31,
    * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
    * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
    * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
    * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
    * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
    * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
    */        
   
    offbit = GET32(ImageBuffer + 10);
    width = GET32(ImageBuffer + 18);
    height = GET32(ImageBuffer + 22); 
	nPlane = GET16(ImageBuffer + 26);
    bit_depth = GET16(ImageBuffer + 28);
	bits_pixel = bit_depth * nPlane;
	
    if (bit_depth == 1 || bit_depth == 2 || bit_depth == 4 || bit_depth == 8)
		color_type = PNG_COLOR_TYPE_PALETTE;
	else if(bit_depth == 16 || bit_depth == 24)
		color_type = PNG_COLOR_TYPE_RGB;

    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

   /* set the palette if there is one.  REQUIRED for indexed-color images */
   palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH
             * sizeof (png_color));
   if (!palette)
   {
	   return FALSE;
   }

   if (bit_depth <= 8)
    {    
	    char *ptr;
        ptr = ImageBuffer + 54; 
        for(i=0;i<(1<<bit_depth);i++, ptr += 4) 
        {
            palette[i].red = *(ptr+2);
            palette[i].green = *(ptr+1);
            palette[i].blue = *ptr;            
        }    
        /* ... set palette colors ... */
        png_set_PLTE(png_ptr, info_ptr, palette, (1<<bit_depth));
        /* You must not free palette here, because png_set_PLTE only makes a link to
        the palette that you malloced.  Wait until you are about to destroy
        the png structure. */
    }    
   

   /* optional significant bit chunk */
   /* if we are dealing with a grayscale image then */
//sig_bit.gray = true_bit_depth;
   /* otherwise, if we are dealing with a color image then */
//   sig_bit.red = true_red_bit_depth;
//   sig_bit.green = true_green_bit_depth;
//   sig_bit.blue = true_blue_bit_depth;
   /* if the image has an alpha channel then */
//   sig_bit.alpha = true_alpha_bit_depth;
//   png_set_sBIT(png_ptr, info_ptr, sig_bit);


   /* Optional gamma chunk is strongly suggested if you have any guess
    * as to the correct gamma of the image.
    */
    
   png_set_gAMA(png_ptr, info_ptr, (double)0);

   /* Optionally write comments into the image */
   text_ptr[0].key = "Title";
   text_ptr[0].text = " ";
   text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
   text_ptr[1].key = "Author";
   text_ptr[1].text = " b";
   text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
   text_ptr[2].key = "Description";
   text_ptr[2].text = "<long text>";
   text_ptr[2].compression = PNG_TEXT_COMPRESSION_zTXt;
#ifdef PNG_iTXt_SUPPORTED
   text_ptr[0].lang = NULL;
   text_ptr[1].lang = NULL;
   text_ptr[2].lang = NULL;
#endif
   //png_set_text(png_ptr, info_ptr, text_ptr, 2);

   /* other optional chunks like cHRM, bKGD, tRNS, tIME, oFFs, pHYs, */
   /* note that if sRGB is present the gAMA and cHRM chunks must be ignored
    * on read and must be written in accordance with the sRGB profile */

   /* Write the file header information.  REQUIRED */
   png_write_info(png_ptr, info_ptr);
   /* If you want, you can write the info in two steps, in case you need to
    * write your private chunk ahead of PLTE:
    *
    *   png_write_info_before_PLTE(write_ptr, write_info_ptr);
    *   write_my_chunk();
    *   png_write_info(png_ptr, info_ptr);
    *
    * However, given the level of known- and unknown-chunk support in 1.1.0
    * and up, this should no longer be necessary.
    */

   /* Once we write out the header, the compression type on the text
    * chunks gets changed to PNG_TEXT_COMPRESSION_NONE_WR or
    * PNG_TEXT_COMPRESSION_zTXt_WR, so it doesn't get written out again
    * at the end.
    */

   /* set up the transformations you want.  Note that these are
    * all optional.  Only call them if you want them.
    */

   /* invert monochrome pixels */
   png_set_invert_mono(png_ptr);   

   /* Shift the pixels up to a legal bit depth and fill in
    * as appropriate to correctly scale the image.
    */
  // png_set_shift(png_ptr, &sig_bit);

   /* pack pixels into bytes */
   png_set_packing(png_ptr); 

   /* swap location of alpha bytes from ARGB to RGBA */
   png_set_swap_alpha(png_ptr);

   /* Get rid of filler (OR ALPHA) bytes, pack XRGB/RGBX/ARGB/RGBA into
    * RGB (4 channels -> 3 channels). The second parameter is not used.
    */
   png_set_filler(png_ptr, 0, PNG_FILLER_BEFORE);

   /* flip BGR pixels to RGB */
   png_set_bgr(png_ptr);

   /* swap bytes of 16-bit files to most significant byte first */
   png_set_swap(png_ptr);

   /* swap bits of 1, 2, 4 bit packed pixel formats */
   png_set_packswap(png_ptr);

   /* turn on interlace handling if you are not using png_write_image() */
  // if (interlacing)
   //   number_passes = png_set_interlace_handling(png_ptr);
   //else
      number_passes = 1;

   /* The easiest way to write the image (you may have a different memory
    * layout, however, so choose what fits your needs best).  You need to
    * use the first method if you aren't handling interlacing yourself.
    */
 
	pData = (png_bytep)(ImageBuffer + offbit);
    
	bits_line = (width * bits_pixel / 8+ 3) & ~3;      
   
    row_pointers = (png_bytepp)malloc(sizeof(png_bytep) * height);
    if (!row_pointers)
		return FALSE;

    for ( i = height - 1; i >= 0; i-- , pData += bits_line)
        row_pointers[i] = pData;

   /* One of the following output methods is REQUIRED */
//#ifdef entire /* write out the entire image data in one call */

	png_write_image(png_ptr, row_pointers);

   /* the other way to write the image - deal with interlacing */
/*
#else no_entire /* write out the image data by one or more scanlines */
   /* The number of passes is either 1 for non-interlaced images,

  * or 7 for interlaced images.
    */
  // for (pass = 0; pass < number_passes; pass++)
   //{
      /* Write a few rows at a time. */
     // png_write_rows(png_ptr, &row_pointers[first_row], number_of_rows);

      /* If you are only writing one row at a time, this works */
     // for (y = 0; y < height; y++)
      //{
       //  png_write_rows(png_ptr, &row_pointers[y], 1);
     // }
  // }
//#endif no_entire /* use only one output method */

   /* You can write optional chunks like tEXt, zTXt, and tIME at the end
    * as well.  Shouldn't be necessary in 1.1.0 and up as all the public
    * chunks are supported and you can use png_set_unknown_chunks() to
    * register unknown chunks into the info structure to be written out.
    */

   /* It is REQUIRED to call this to finish writing the rest of the file */
   png_write_end(png_ptr, info_ptr);
//#endif hilevel

   /* If you png_malloced a palette, free it here (don't free info_ptr->palette,
      as recommended in versions 1.0.5m and earlier of this example; if
      libpng mallocs info_ptr->palette, libpng will free it).  If you
      allocated it with malloc() instead of png_malloc(), use free() instead
      of png_free(). */
   png_free(png_ptr, palette);
   free(row_pointers);   
   palette=NULL;
                  
   /* Similarly, if you png_malloced any data that you passed in with
      png_set_something(), such as a hist or trans array, free it here,
      when you can be sure that libpng is through with it. */
   //png_free(png_ptr, trans);
   //trans=NULL;

   /* clean up after the write, and free any memory allocated */
   png_destroy_write_struct(&png_ptr, &info_ptr); 

   return TRUE;
}
