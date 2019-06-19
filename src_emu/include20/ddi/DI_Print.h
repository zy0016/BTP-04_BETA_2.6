/*

	print.h
	logic layer


	Kent.
	20000719
*/

#ifndef _PRINT_H_
#define _PRINT_H_

#include "hp_kern.h"
#include "hpdef.h"

/*
typedef unsigned long	DWORD ;
typedef signed long		LONG ;
typedef unsigned char	BYTE ;
typedef unsigned short	WORD ;
*/

#define FF    				0x0c
#define ESC			 		27

/*#define	IO_BEGIN_PRINT		1+100
#define	IO_PRINTDATA_FMT	2+100
#define	IO_PRINT_DATA		3+100
#define	IO_CANCEL_PRINT		4+100
#define	IO_END_PRINT		5+100*/
#define	IO_START_PRINT		1+100
#define	IO_PRINTDATA_FMT	2+100
#define IO_BEGIN_PAGE       7+100
#define	IO_END_PAGE			8+100
#define IO_START_BAND       6+100
#define	IO_END_BAND			3+100
#define	IO_CANCEL_PRINT		4+100
#define	IO_END_PRINT		5+100

typedef struct tagIMAGEINFOHEADER{ // bmih 
   DWORD  biSize ; 
   LONG   biWidth ; 
   LONG   biHeight ; 
   WORD   biPlanes ; 
   WORD   biBitCount ;
   DWORD  biCompression ; 
   DWORD  biSizeImage ; 
   LONG   biXPelsPerMeter ; 
   LONG   biYPelsPerMeter ; 
   DWORD  biClrUsed ; 
   DWORD  biClrImportant ; 
} IMAGEINFOHEADER ; 
 

typedef struct tagRGB { // rgbq 
    BYTE    rgbBlue ; 
    BYTE    rgbGreen ; 
    BYTE    rgbRed ; 
    BYTE    rgbReserved ; 
} RGB_QUAD ; 

typedef struct tagIMAGEINFO {
    IMAGEINFOHEADER		bmiHeader ;
    RGB_QUAD            bmiColors[256] ;
} IMAGEINFO ;


#endif
