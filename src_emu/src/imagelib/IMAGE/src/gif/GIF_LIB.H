/******************************************************************************
* In order to make life a little bit easier when using the GIF file format,   *
* this library was written, and which does all the dirty work...	      *
*									      *
*					Written by Gershon Elber,  Jun. 1989  *
*******************************************************************************
* History:								      *
* 14 Jun 89 - Version 1.0 by Gershon Elber.				      *
******************************************************************************/

#ifndef GIF_LIB_H
#define GIF_LIB_H

#include "../filesys.h"
#include "GIF_HASH.H"

#define	ERROR		0
#define OK		1

#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#define FILE_BUFFER_SIZE 16384	     /* Files uses bigger buffers than usual */

typedef	int		BooleanType;
typedef	unsigned char	PixelType;
typedef unsigned char *	RowType;
typedef unsigned char	ByteType;

#define MESSAGE(s)	fprintf(stderr, "\n%s: %s\n", __FILE__, s)
#define EXIT(s)		{ MESSAGE(s); exit(-3); }

typedef struct GifColorType {
    ByteType Red, Green, Blue;
} GifColorType;

/* Note entries prefixed with S are of Screen information, while entries     */
/* prefixed with I are of the current defined Image.			     */
typedef struct GifFileType {
    int SWidth, SHeight,				/* Screen dimensions */
	SColorResolution, SBitsPerPixel,/* How many colors can we generate? */
	SBackGroundColor,		/* I hope you understand this one... */
	ILeft, ITop, IWidth, IHeight,		 /* Current image dimensions */
	IInterlace,			      /* Sequential/Interlaced lines */
	IBitsPerPixel;			  /* How many colors this image has? */
    GifColorType *SColorMap, *IColorMap;	       /* NULL if not exists */
    void *Private;	  /* The regular user should not mesh with this one! */
} GifFileType;

#define COMMENT_EXT_FUNC_CODE	'C'   /* Extension function code for comment */
#define GIF_STAMP	"GIF87a"	  /* First chars in file - GIF stamp */
#define GIF_STAMP_LEN	sizeof(GIF_STAMP) - 1

#define LZ_MAX_CODE	4095		/* Biggest code possible in 12 bits. */
#define LZ_BITS		12

#define FILE_STATE_READ		0x01/* 1 write, 0 read - EGIF_LIB compatible */

#define FLUSH_OUTPUT		4096	 /* Impossible code, to signal flush */
#define FIRST_CODE		4097	 /* Impossible code, to signal first */
#define NO_SUCH_CODE		4098     /* Impossible code, to signal empty */

#define IS_READABLE(Private)	(!(Private -> FileState & FILE_STATE_READ))

typedef struct GifFilePrivateType {
    int FileState,			     /* Where all this data goes to! */
    FileHandle, 
    fSize, off, 
	BitsPerPixel,	     /* Bits per pixel (Codes uses at list this + 1) */
	ClearCode,					/* The CLEAR LZ code */
	EOFCode,					  /* The EOF LZ code */
	RunningCode,		     /* The next code algorithm can generate */
	RunningBits, /* The number of bits required to represent RunningCode */
	MaxCode1,/* 1 bigger than maximum possible code, in RunningBits bits */
	LastCode,			 /* The code before the current code */
	CrntCode,				   /* Current algorithm code */
	StackPtr,			  /* For character stack (see below) */
	CrntShiftState;			 /* Number of bits in CrntShiftDWord */
    
    unsigned long CrntShiftDWord,      /* For bytes decomposition into codes */
		  PixelCount;			/* Number of pixels in image */
    char *hFile;						   /* File as stream */
    ByteType Buf[256];		        /* Compressed input is buffered here */
    ByteType Stack[LZ_MAX_CODE];	  /* Decoded pixels are stacked here */
    ByteType Suffix[LZ_MAX_CODE+1];		/* So we can trace the codes */
    unsigned int Prefix[LZ_MAX_CODE+1];
    
    GifHashTableType *HashTable;
} GifFilePrivateType;

typedef enum {
    UNDEFINED_RECORD_TYPE,
    SCREEN_DESC_RECORD_TYPE,
    IMAGE_DESC_RECORD_TYPE,				   /* Begin with ',' */
    EXTENSION_RECORD_TYPE,				   /* Begin with '!' */
    TERMINATE_RECORD_TYPE				   /* Begin with ';' */
} GifRecordType;

/******************************************************************************
* O.k. here are the routines one can access in order to encode GIF file:      *
* (GIF_LIB file EGIF_LIB.C).						      *
******************************************************************************/

#define GFWRITE( buf, size, count, hFile ) \
    (IMGOS_WriteFile(hFile, buf, count))

GifFileType *EGifOpenFileName(char *GifFileName, int GifTestExistance);
GifFileType *EGifOpenFileHandle(int GifFileHandle);
int EGifPutScreenDesc(GifFileType *GifFile,
	int GifWidth, int GifHeight, int GifColorRes, int GifBackGround,
	int GifBitsPerPixel, GifColorType *GifColorMap);
int EGifPutImageDesc(GifFileType *GifFile,
	int GifLeft, int GifTop, int Width, int GifHeight, int GifInterlace,
	int GifBitsPerPixel, GifColorType *GifColorMap);
int EGifPutLine(GifFileType *GifFile, PixelType *GifLine, int GifLineLen);
int EGifPutPixel(GifFileType *GifFile, PixelType GifPixel);
int EGifPutComment(GifFileType *GifFile, char *GifComment);
int EGifPutExtension(GifFileType *GifFile, int GifExtCode, int GifExtLen,
							void *GifExtension);
int EGifPutCode(GifFileType *GifFile, int GifCodeSize, ByteType *GifCodeBlock);
int EGifPutCodeNext(GifFileType *GifFile, ByteType *GifCodeBlock);
int EGifCloseFile(GifFileType *GifFile);

#define	E_GIF_ERR_OpenFailed	1		 /* And EGif possible errors */
#define	E_GIF_ERR_WriteFailed	2
#define E_GIF_ERR_HasScrnDscr	3
#define E_GIF_ERR_HasImagDscr	4
#define E_GIF_ERR_NoColorMap	5
#define E_GIF_ERR_DataTooBig	6
#define E_GIF_ERR_NotEnoughMem	7
#define E_GIF_ERR_DiskIsFull	8
#define E_GIF_ERR_CloseFailed	9
#define E_GIF_ERR_NotWriteable	10

/******************************************************************************
* O.k. here are the routines one can access in order to decode GIF file:      *
* (GIF_LIB file DGIF_LIB.C).						      *
******************************************************************************/

GifFileType *DGifOpenFileName(char *GifFileName);
GifFileType *DGifCreate(char *hFile, int fSize, int FileHandle);
int DGifGetScreenDesc(GifFileType *GifFile);
int DGifGetRecordType(GifFileType *GifFile, GifRecordType *GifType);
int DGifGetImageDesc(GifFileType *GifFile);
int DGifGetLine(GifFileType *GifFile, PixelType *GifLine, int GifLineLen);
int DGifGetPixel(GifFileType *GifFile, PixelType GifPixel);
int DGifGetComment(GifFileType *GifFile, char *GifComment);
int DGifGetExtension(GifFileType *GifFile, int *GifExtCode,
						ByteType **GifExtension);
int DGifGetExtensionNext(GifFileType *GifFile, ByteType **GifExtension);
int DGifGetCode(GifFileType *GifFile, int *GifCodeSize,
						ByteType **GifCodeBlock);
int DGifGetCodeNext(GifFileType *GifFile, ByteType **GifCodeBlock);
int DGifGetLZCodes(GifFileType *GifFile, int *GifCode);
int DGifCloseFile(GifFileType *GifFile);

#define	D_GIF_ERR_OpenFailed	101		 /* And DGif possible errors */
#define	D_GIF_ERR_ReadFailed	102
#define	D_GIF_ERR_NotGifFile	103
#define D_GIF_ERR_NoScrnDscr	104
#define D_GIF_ERR_NoImagDscr	105
#define D_GIF_ERR_NoColorMap	106
#define D_GIF_ERR_WrongRecord	107
#define D_GIF_ERR_DataTooBig	108
#define D_GIF_ERR_NotEnoughMem	109
#define D_GIF_ERR_CloseFailed	110
#define D_GIF_ERR_NotReadable	111
#define D_GIF_ERR_ImageDefect	112
#define D_GIF_ERR_EOFTooSoon	113

/******************************************************************************
* O.k. here are the routines from GIF_LIB file GIF_ERR.C.		      *
******************************************************************************/
void PrintGifError(void);
int GifLastError(void);

/******************************************************************************
* O.k. here are the routines from GIF_LIB file DEV2GIF.C.		      *
******************************************************************************/
int DumpScreen(char *FileName, int ReqGraphDriver, int ReqGraphMode);

#endif GIF_LIB_H
