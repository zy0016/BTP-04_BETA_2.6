/*****************************************************************************
*   "Gif-Lib" - Yet another gif library.				     *
*									     *
* Written by:  Gershon Elber			IBM PC Ver 0.1,	Jun. 1989    *
******************************************************************************
* The kernel of the GIF Encoding process can be found here.		     *
******************************************************************************
* History:								     *
* 14 Jun 89 - Version 1.0 by Gershon Elber.				     *
*****************************************************************************/

//#include <io.h>
//#include <fcntl.h>
//#include <stdio.h>
//#include <alloc.h>
#include <string.h>
//#include <stat.h>
#include "gif_lib.h"
//#include "gif_hash.h"

#define PROGRAM_NAME	"GIF_LIBRARY"
#define VERSION		"á Version 1.0, "

#define COMMENT_EXT_FUNC_CODE	'C'   /* Extension function code for comment */
#define GIF_STAMP	"GIF87a"	  /* First chars in file - GIF stamp */
#define ZL_MAX_CODE	4095		/* Biggest code possible in 12 bits. */

#define FILE_STATE_WRITE	0x01/* 1 write, 0 read - DGIF_LIB compatible */
#define FILE_STATE_SCREEN	0x02
#define FILE_STATE_IMAGE	0x04

#define FLUSH_OUTPUT		4096	 /* Impossible code, to signal flush */
#define FIRST_CODE		4097	 /* Impossible code, to signal first */

#define IS_WRITEABLE(Private)	(Private -> FileState & FILE_STATE_WRITE)

/* #define DEBUG_NO_PREFIX			/* Dump only compressed data */
/*
typedef struct GifFilePrivateType {

  GifHashTableType *HashTable;
} GifFilePrivateType;
*/
int _GifError;

/* Masks given codes to BitsPerPixel, to make sure all codes are in range: */
static PixelType CodeMask[] = {
    0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
};

static char *VersionStr =
	PROGRAM_NAME
	"	IBMPC "
	VERSION
	"	Gershon Elber,	"
	__DATE__ ",   " __TIME__ "\n"
	"(C) Copyright 1989 Gershon Elber, Non commercial use only.\n";

static int EGifPutWord(int Word, int hFile);
static int EGifSetupCompress(GifFileType *GifFile);
static int EGifCompressLine(GifFileType *GifFile, PixelType *Line, int LineLen);
static int EGifCompressOutput(GifFilePrivateType *Private, int Code);
static int EGifBufferedOutput(int hFile, ByteType *Buf, int c);


/******************************************************************************
*   Update a new gif file, given its file handle, which must be opened for    *
* write in binary mode.							      *
*   Returns GifFileType pointer dynamically allocated which serves as the gif *
* info record. _GifError is cleared if succesfull.			      *
******************************************************************************/
GifFileType *EGifOpenFileHandle(int FileHandle)
{
    GifFileType *GifFile;
    GifFilePrivateType *Private;     

    if ((GifFile = (GifFileType *) malloc(sizeof(GifFileType))) == NULL) {
	_GifError = E_GIF_ERR_NotEnoughMem;
	return NULL;
    }

    GifFile -> SWidth = GifFile -> SHeight =
    GifFile -> SColorResolution = GifFile -> SBitsPerPixel =
    GifFile -> SBackGroundColor =
    GifFile -> ILeft = GifFile -> ITop = GifFile -> IWidth = GifFile -> IHeight =
    GifFile -> IInterlace =
    GifFile -> IBitsPerPixel = 0;

    GifFile -> SColorMap = GifFile -> IColorMap = NULL;

#   ifndef DEBUG_NO_PREFIX
    if (GFWRITE(GIF_STAMP, 1, strlen(GIF_STAMP), FileHandle) != (long)strlen(GIF_STAMP)) {
	_GifError = E_GIF_ERR_WriteFailed;
	free((char *) GifFile);
	return NULL;
    }
#   endif DEBUG_NO_PREFIX

    if ((Private = (GifFilePrivateType *) malloc(sizeof(GifFilePrivateType)))
	== NULL) {
	_GifError = E_GIF_ERR_NotEnoughMem;
	return NULL;
    }

    GifFile -> Private = (void *) Private;
    Private -> FileHandle = FileHandle;
    Private -> hFile = NULL;
    Private -> FileState = FILE_STATE_WRITE;
    if ((Private -> HashTable = _InitHashTable()) == NULL) {
	_GifError = E_GIF_ERR_NotEnoughMem;
	return NULL;
    }

    _GifError = 0;

    return GifFile;
}

/******************************************************************************
*   This routine should be called before any other EGif calls, immediately    *
* follows the GIF file openning.					      *
******************************************************************************/
int EGifPutScreenDesc(GifFileType *GifFile,
	int Width, int Height, int ColorRes, int BackGround,
	int BitsPerPixel, GifColorType *ColorMap)
{
    int i, Size;
    ByteType Buf[3];
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile -> Private;

    if (Private -> FileState & FILE_STATE_SCREEN) {
	/* If already has screen descriptor - something is wrong! */
	_GifError = E_GIF_ERR_HasScrnDscr;
	return ERROR;
    }
    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NotWriteable;
	return ERROR;
    }

    GifFile -> SWidth = Width;
    GifFile -> SHeight = Height;
    GifFile -> SColorResolution = ColorRes;
    GifFile -> SBitsPerPixel = BitsPerPixel;
    GifFile -> SBackGroundColor = BackGround;
    if (ColorMap) {
	Size = sizeof(GifColorType) * (1 << BitsPerPixel);
	GifFile -> SColorMap = (GifColorType *) malloc(Size);
	memcpy(GifFile -> SColorMap, ColorMap, Size);
    }

    /* Put the screen descriptor into the file: */
    EGifPutWord(Width, Private -> FileHandle);
    EGifPutWord(Height, Private -> FileHandle);
    Buf[0] = (ColorMap ? 0x80 : 0x00) |
	     ((ColorRes - 1) << 4) |
	     (BitsPerPixel - 1);
    Buf[1] = BackGround;
    Buf[2] = 0;
#   ifndef DEBUG_NO_PREFIX
    GFWRITE(Buf, 1, 3, Private -> FileHandle);
#   endif DEBUG_NO_PREFIX

    /* If we have Global color map - dump it also: */
#   ifndef DEBUG_NO_PREFIX
    if (ColorMap != NULL)
	for (i=0; i<(1 << BitsPerPixel); i++) {/* Put the ColorMap out also: */
	    Buf[0] = ColorMap[i].Red;
	    Buf[1] = ColorMap[i].Green;
	    Buf[2] = ColorMap[i].Blue;
	    if (GFWRITE(Buf, 1, 3, Private -> FileHandle) != 3) {
	        _GifError = E_GIF_ERR_WriteFailed;
		return ERROR;
	    }
	}
#   endif DEBUG_NO_PREFIX

    /* Mark this file as has screen descriptor, and no pixel written yet: */
    Private -> FileState |= FILE_STATE_SCREEN;

    return OK;
}

/******************************************************************************
*   This routine should be called before any attemp to dump an image - any    *
* call to any of the pixel dump routines.				      *
******************************************************************************/
int EGifPutImageDesc(GifFileType *GifFile,
	int Left, int Top, int Width, int Height, int Interlace,
	int BitsPerPixel, GifColorType *ColorMap)
{
    int i, Size;
    ByteType Buf[3];
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile -> Private;

    if (Private -> FileState & FILE_STATE_IMAGE &&
	Private -> PixelCount > 0) {
	/* If already has active image descriptor - something is wrong! */
	_GifError = E_GIF_ERR_HasImagDscr;
	return ERROR;
    }
    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NotWriteable;
	return ERROR;
    }
    GifFile -> ILeft = Left;
    GifFile -> ITop = Top;
    GifFile -> IWidth = Width;
    GifFile -> IHeight = Height;
    GifFile -> IBitsPerPixel = BitsPerPixel;
    GifFile -> IInterlace = Interlace;
    if (ColorMap) {
	Size = sizeof(GifColorType) * (1 << BitsPerPixel);
	if (GifFile -> IColorMap) free((char *) GifFile -> IColorMap);
	GifFile -> IColorMap = (GifColorType *) malloc(Size);
	memcpy(GifFile -> IColorMap, ColorMap, Size);
    }

    /* Put the image descriptor into the file: */
    Buf[0] = ',';				/* Image seperator character */
#   ifndef DEBUG_NO_PREFIX
    GFWRITE(Buf, 1, 1, Private -> FileHandle);
#   endif DEBUG_NO_PREFIX
    EGifPutWord(Left, Private -> FileHandle);
    EGifPutWord(Top, Private -> FileHandle);
    EGifPutWord(Width, Private -> FileHandle);
    EGifPutWord(Height, Private -> FileHandle);
    Buf[0] = (ColorMap ? 0x80 : 0x00) |
	  (Interlace ? 0x40 : 0x00) |
	  (BitsPerPixel - 1);
#   ifndef DEBUG_NO_PREFIX
    GFWRITE(Buf, 1, 1, Private -> FileHandle);
#   endif DEBUG_NO_PREFIX

    /* If we have Global color map - dump it also: */
#   ifndef DEBUG_NO_PREFIX
    if (ColorMap != NULL)
	for (i=0; i<(1 << BitsPerPixel); i++) {/* Put the ColorMap out also: */
	    Buf[0] = ColorMap[i].Red;
	    Buf[1] = ColorMap[i].Green;
	    Buf[2] = ColorMap[i].Blue;
	    if (GFWRITE(Buf, 1, 3, Private -> FileHandle) != 3) {
	        _GifError = E_GIF_ERR_WriteFailed;
		return ERROR;
	    }
	}
#   endif DEBUG_NO_PREFIX
    if (GifFile -> SColorMap == NULL && GifFile -> IColorMap == NULL)
    {
	_GifError = E_GIF_ERR_NoColorMap;
	return ERROR;
    }

    /* Mark this file as has screen descriptor: */
    Private -> FileState |= FILE_STATE_IMAGE;
    Private -> PixelCount = (long) Width * (long) Height;

    EGifSetupCompress(GifFile);       /* Reset compress algorithm parameters */

    return OK;
}

/******************************************************************************
*  Put one full scanned line (Line) of length LineLen into GIF file.	      *
******************************************************************************/
int EGifPutLine(GifFileType *GifFile, PixelType *Line, int LineLen)
{
    int i;
    PixelType Mask;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile -> Private;

    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NotWriteable;
	return ERROR;
    }

    if (!LineLen) LineLen = GifFile -> IWidth;
    if ((Private -> PixelCount -= LineLen) < 0) {
	_GifError = E_GIF_ERR_DataTooBig;
	return ERROR;
    }

    /* Make sure the codes are not out of bit range, as we might generate    */
    /* wrong code (because of overflow when we combine them) in this case:   */
    Mask = CodeMask[Private -> BitsPerPixel];
    for (i=0; i<LineLen; i++) Line[i] &= Mask;

    return EGifCompressLine(GifFile, Line, LineLen);
}

/******************************************************************************
* Put one pixel (Pixel) into GIF file.					      *
******************************************************************************/
int EGifPutPixel(GifFileType *GifFile, PixelType Pixel)
{
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile -> Private;

    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NotWriteable;
	return ERROR;
    }

    if (--Private -> PixelCount < 0)
    {
	_GifError = E_GIF_ERR_DataTooBig;
	return ERROR;
    }

    /* Make sure the code is not out of bit range, as we might generate	     */
    /* wrong code (because of overflow when we combine them) in this case:   */
    Pixel &= CodeMask[Private -> BitsPerPixel];

    return EGifCompressLine(GifFile, &Pixel, 1);
}

/******************************************************************************
* Put a comment into GIF file using extension block.			      *
******************************************************************************/
int EGifPutComment(GifFileType *GifFile, char *Comment)
{
    return EGifPutExtension(GifFile, COMMENT_EXT_FUNC_CODE, strlen(Comment),
								Comment);
}

/******************************************************************************
*   Put an extension block (see GIF manual) into gif file.		      *
******************************************************************************/
int EGifPutExtension(GifFileType *GifFile, int ExtCode, int ExtLen,
							void *Extension)
{
    ByteType Buf[3];
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile -> Private;

    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NotWriteable;
	return ERROR;
    }

    Buf[0] = '!';
    Buf[1] = ExtCode;
    Buf[2] = ExtLen;
    GFWRITE(Buf, 1, 3, Private -> FileHandle);
    GFWRITE(Extension, 1, ExtLen, Private -> FileHandle);
    Buf[0] = 0;
    GFWRITE(Buf, 1, 1, Private -> FileHandle);

    return OK;
}

/******************************************************************************
*   Put the image code in compressed form. This routine can be called if the  *
* information needed to be piped out as is. Obviously this is much faster     *
* than decoding and encoding again. This routine should be followed by calls  *
* to EGifPutCodeNext, until NULL block is given.			      *
*   The block should NOT be freed by the user (not dynamically allocated).    *
******************************************************************************/
int EGifPutCode(GifFileType *GifFile, int CodeSize, ByteType *CodeBlock)
{
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile -> Private;

    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NotWriteable;
	return ERROR;
    }

    /* No need to dump code size as Compression set up does any for us: */
    /*
    Buf = CodeSize;
    if (GFWRITE(&Buf, 1, 1, Private -> File) != 1) {
	_GifError = E_GIF_ERR_WriteFailed;
	return ERROR;
    }
    */

    return EGifPutCodeNext(GifFile, CodeBlock);
}

/******************************************************************************
*   Continue to put the image code in compressed form. This routine should be *
* called with blocks of code as read via DGifGetCode/DGifGetCodeNext. If      *
* given buffer pointer is NULL, empty block is written to mark end of code.   *
******************************************************************************/
int EGifPutCodeNext(GifFileType *GifFile, ByteType *CodeBlock)
{
    ByteType Buf;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile -> Private;

    if (CodeBlock != NULL) {
	if (GFWRITE(CodeBlock, 1, CodeBlock[0] + 1, Private -> FileHandle)
							!= CodeBlock[0] + 1) {
	    _GifError = E_GIF_ERR_WriteFailed;
	    return ERROR;
	}
    }
    else {
	Buf = 0;
	if (GFWRITE(&Buf, 1, 1, Private -> FileHandle) != 1) {
	    _GifError = E_GIF_ERR_WriteFailed;
	    return ERROR;
	}
	Private -> PixelCount = 0;    /* And local info. indicate image read */
    }

    return OK;
}

/******************************************************************************
*   This routine should be called last, to close GIF file.		      *
******************************************************************************/
int EGifCloseFile(GifFileType *GifFile)
{
    ByteType Buf;
    GifFilePrivateType *Private;
    int hFile;

    if (GifFile == NULL) return ERROR;

    Private = (GifFilePrivateType *) GifFile -> Private;
    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NotWriteable;
	return ERROR;
    }

    hFile = Private -> FileHandle;

    Buf = ';';
    GFWRITE(&Buf, 1, 1, Private -> FileHandle);

    if (GifFile -> IColorMap) free((char *) GifFile -> IColorMap);
    if (GifFile -> SColorMap) free((char *) GifFile -> SColorMap);
    if (Private) {
	if (Private -> HashTable) free((char *) Private -> HashTable);
	free((char *) Private);
    }
    free(GifFile);
/*
    if (fclose(hFile) != 0) {
	_GifError = E_GIF_ERR_CloseFailed;
	return ERROR;
    }*/
    return OK;
}

/******************************************************************************
*   Put 2 bytes (word) into the given file:				      *
******************************************************************************/
static int EGifPutWord(int Word, int hFile)
{
    char c[2];

    c[0] = Word & 0xff;
    c[1] = (Word >> 8) & 0xff;
#   ifndef DEBUG_NO_PREFIX
    if (GFWRITE(c, 1, 2, hFile) == 2)
	 return OK;
    else return ERROR;
#   else
    return OK;
#   endif DEBUG_NO_PREFIX
}

/******************************************************************************
*   Setup the LZ compression for this image:				      *
******************************************************************************/
static int EGifSetupCompress(GifFileType *GifFile)
{
    int BitsPerPixel;
    ByteType Buf;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile -> Private;

    /* Test and see what color map to use, and from it # bits per pixel: */
    if (GifFile -> IColorMap)
	 BitsPerPixel = GifFile -> IBitsPerPixel;
    else
    if (GifFile -> SColorMap)
	 BitsPerPixel = GifFile -> SBitsPerPixel;
    else {
	_GifError = E_GIF_ERR_NoColorMap;
	return ERROR;
    }

    Buf = BitsPerPixel = (BitsPerPixel < 2 ? 2 : BitsPerPixel);
    GFWRITE(&Buf, 1, 1, Private -> FileHandle);      /* Write the Code size to file */

    Private -> Buf[0] = 0;			   /* Nothing was output yet */
    Private -> BitsPerPixel = BitsPerPixel;
    Private -> ClearCode = (1 << BitsPerPixel);
    Private -> EOFCode = Private -> ClearCode + 1;
    Private -> RunningCode = Private -> EOFCode + 1;
    Private -> RunningBits = BitsPerPixel + 1;	  /* Number of bits per code */
    Private -> MaxCode1 = 1 << Private -> RunningBits;	    /* Max. code + 1 */
    Private -> CrntCode = FIRST_CODE;	   /* Signal that this is first one! */
    Private -> CrntShiftState = 0;	 /* No information in CrntShiftDWord */
    Private -> CrntShiftDWord = 0;

    /* Clear hash table and send Clear to make sure the decoder do the same  */
    _ClearHashTable(Private -> HashTable);
    if (EGifCompressOutput(Private, Private -> ClearCode) == ERROR) {
	_GifError = E_GIF_ERR_DiskIsFull;
	return ERROR;
    }
    return OK;
}

/******************************************************************************
*   The LZ compression routine:						      *
*   This version compress the given buffer Line of length LineLen.	      *
*   This routine can be called few times (one per scan line, for example), in *
* order the complete the whole image.					      *
******************************************************************************/
static int EGifCompressLine(GifFileType *GifFile, PixelType *Line, int LineLen)
{
    int i = 0, CrntCode, NewCode;
    unsigned long NewKey;
    PixelType Pixel;
    GifHashTableType *HashTable;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile -> Private;

    HashTable = Private -> HashTable;

    if (Private -> CrntCode == FIRST_CODE)		  /* Its first time! */
	 CrntCode = Line[i++];
    else CrntCode = Private -> CrntCode;     /* Get last code in compression */

    while (i < LineLen) {			    /* Decode LineLen items. */
	Pixel = Line[i++];		       /* Get next pixel from stream */
	/* Form a new unique key to search hash table for the code combines  */
	/* CrntCode as Prefix string with Pixel as postfix char.	     */
	NewKey = (((unsigned long) CrntCode) << 8) + Pixel;
	if ((NewCode = _ExistsHashTable(HashTable, NewKey)) >= 0) {
	    /* This Key is already there, or the string is old one, so	     */
	    /* simple take new code as our CrntCode:			     */
	    CrntCode = NewCode;
	}
	else {
	    /* Put it in hash table, output the prefix code, and make our    */
	    /* CrntCode equal to Pixel.					     */
	    if (EGifCompressOutput(Private, CrntCode)
		== ERROR) {
		_GifError = E_GIF_ERR_DiskIsFull;
		return ERROR;
	    }
	    CrntCode = Pixel;

	    /* If however the HashTable if full, we send a clear first and   */
	    /* Clear the hash table.					     */
	    if (Private -> RunningCode >= ZL_MAX_CODE) {
		/* Time to do some clearance: */
		if (EGifCompressOutput(Private, Private -> ClearCode)
		    == ERROR) {
		    _GifError = E_GIF_ERR_DiskIsFull;
		    return ERROR;
		}
		Private -> RunningCode = Private -> EOFCode + 1;
		Private -> RunningBits = Private -> BitsPerPixel + 1;
		Private -> MaxCode1 = 1 << Private -> RunningBits;
		_ClearHashTable(HashTable);
	    }
	    else {
		/* Put this unique key with its relative Code in hash table: */
		_InsertHashTable(HashTable, NewKey, Private -> RunningCode++);
	    }
	}
    }

    /* Preserve the current state of the compression algorithm: */
    Private -> CrntCode = CrntCode;

    if (Private -> PixelCount == 0)
    {
	/* We are done - output last Code and flush output buffers: */
	if (EGifCompressOutput(Private, CrntCode)
	    == ERROR) {
	    _GifError = E_GIF_ERR_DiskIsFull;
	    return ERROR;
	}
	if (EGifCompressOutput(Private, Private -> EOFCode)
	    == ERROR) {
	    _GifError = E_GIF_ERR_DiskIsFull;
	    return ERROR;
	}
	if (EGifCompressOutput(Private, FLUSH_OUTPUT) == ERROR) {
	    _GifError = E_GIF_ERR_DiskIsFull;
	    return ERROR;
	}
    }

    return OK;
}

/******************************************************************************
*   The LZ compression output routine:					      *
*   This routine is responsable for the compression of the bit stream into    *
* 8 bits (bytes) packets.						      *
*   Returns OK if written succesfully.					      *
******************************************************************************/
static int EGifCompressOutput(GifFilePrivateType *Private, int Code)
{
    int retval = OK;

    if (Code == FLUSH_OUTPUT) {
	while (Private -> CrntShiftState > 0) {
	    /* Get Rid of what is left in DWord, and flush it */
	    if (EGifBufferedOutput(Private -> FileHandle, Private -> Buf,
		Private -> CrntShiftDWord & 0xff) == ERROR) retval = ERROR;
	    Private -> CrntShiftDWord >>= 8;
	    Private -> CrntShiftState -= 8;
	}
	Private -> CrntShiftState = 0;			    /* For next time */
	if (EGifBufferedOutput(Private -> FileHandle, Private -> Buf,
	    FLUSH_OUTPUT) == ERROR) retval = ERROR;
    }
    else {
	Private -> CrntShiftDWord |= ((long) Code) << Private -> CrntShiftState;
	Private -> CrntShiftState += Private -> RunningBits;
	while (Private -> CrntShiftState >= 8) {
	    /* Dump out full bytes: */
	    if (EGifBufferedOutput(Private -> FileHandle, Private -> Buf,
		Private -> CrntShiftDWord & 0xff) == ERROR) retval = ERROR;
	    Private -> CrntShiftDWord >>= 8;
	    Private -> CrntShiftState -= 8;
	}
    }

    /* If code cannt fit into RunningBits bits, must raise its size. Note */
    /* however that codes above 4095 are used for special signaling.      */
    if (Private -> RunningCode >= Private -> MaxCode1 && Code <= 4095) {
	Private -> MaxCode1 = 1 << ++Private -> RunningBits;
    }

    return retval;
}

/******************************************************************************
*   This routines buffers the given characters until 255 characters are ready *
* to be output. If Code is equal to -1 the buffer is flushed (EOF).	      *
*   The buffer is Dumped with first byte as its size, as GIF format requires. *
*   Returns OK if written succesfully.					      *
******************************************************************************/
static int EGifBufferedOutput(int hFile, ByteType *Buf, int c)
{
    if (c == FLUSH_OUTPUT) {
	/* Flush everything out */
	if (Buf[0] != 0 && GFWRITE(Buf, 1, Buf[0]+1, hFile) != Buf[0] + 1)
	{
	    _GifError = E_GIF_ERR_WriteFailed;
	    return ERROR;
	}
	/* Mark end of compressed data, by an empty block (see GIF doc): */
	Buf[0] = 0;
	if (GFWRITE(Buf, 1, 1, hFile) != 1)
	{
	    _GifError = E_GIF_ERR_WriteFailed;
	    return ERROR;
	}
    }
    else {
	if (Buf[0] == 255) {
	    /* Dump out this buffer - it is full: */
	    if (GFWRITE(Buf, 1, Buf[0] + 1, hFile) != Buf[0] + 1)
	    {
		_GifError = E_GIF_ERR_WriteFailed;
		return ERROR;
	    }
	    Buf[0] = 0;
	}
	Buf[++Buf[0]] = c;
    }

    return OK;
}
