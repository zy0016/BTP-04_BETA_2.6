/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef __FONT_H
#define __FONT_H

//#define INIFONTFILE

#ifndef TEXTBMP_STRUCT
#define TEXTBMP_STRUCT

/* Define struct for text bitmap */
typedef struct TextBmp
{
    int16   width;          /* Mono bitmap width       */
    int16   height;         /* Mono bitmap height      */
    int32   data[1];        /* Mono bitmap data buffer */
} TEXTBMP, *PTEXTBMP;

#endif  /* TEXTBMP_STRUCT */
/* Define CharsetsA
 */
#define FC_ASCII    0
#define FC_GB2312   1
#define FC_GBK      2
#define FC_BIG5     3
#define FC_GB18030  4
#define FC_1252     5
//#define FC_GSM      4

#define FC_SYSTEMCHARSET FC_GB2312

typedef uint8*          PSTRING;
typedef void*           PFONTDEV;

/* PitchAndFamily family values (high 4 bits) */
#define FF_DONTCARE     0x00
#define FF_ROMAN        0x10
#define FF_SWISS        0x20
#define FF_MODERN       0x30
#define FF_SCRIPT       0x40
#define FF_DECORATIVE   0x50


/* defined font attributes 
 */
#define FA_EQUALWIDTH  0x01
#define FA_FIXEDSIZE   0x02

#define FA_DBCS        0x04
#define FA_SBCS        0x08
#define FA_COLUMN      0x10
//#define FA_DEFAULT     0x80
#define FA_SYSTEMFONT  0x80

/* defined for style */
#define FS_NORMAL       0x00
#define FS_BOLD         0x01
#define FS_ITALIC       0x02
#define FS_UNDERLINE    0x04
#define FS_STRIKEOUT    0x08

/* EnumFonts Masks   May be used later and the value should be changed*/ 
#define RASTER_FONTTYPE     0x0001
#define DEVICE_FONTTYPE     0x0002
#define TRUETYPE_FONTTYPE   0x0004

/* Font information struct */
typedef struct tagFontInfo
{
    char    name[16];   // font name
    int32   attrib;     // font attribute
    int16   height;     // font height
    int16   width;      // font width
    int16   maxwidth;   // max width of this font
    int16   baseline;   // base line of this font
    int16   leftextend;     // left extension of this font
    int16   rightextend;    // right extension of this font
    int16   charset;
    int16   reserved;
}FONTINFO, *PFONTINFO;

/* Logical font struct */
typedef struct tagDrvLogFont
{
    char    name[16];       // font name
    int16   family;         // font family
    int16   style;          // font decoration, including bolding, italic, underline, border, greyback
    int16   weight;         // font weight
    int16   size;           // font size, usually equals to the height
    int16   proportion;
    uint8   charset;
    uint8   unused;
} DRVLOGFONT, *PDRVLOGFONT;

/* Font name information struct */
typedef struct tagFontNameInfo
{
    int8    name[16];       /* font name */
    int8    family;         /* font family */
    int8    charset;        /* char set of this font name */
	int8    unused;         /* useless */
    int32   attrib;         /* possible attrib of this font name */
    int16   minsize;        /* minimum size of this font name */
    int16   maxsize;        /* maximum size of this font name */
}FONTNAMEINFO, *PFONTNAMEINFO;

/* Font device information
 */
typedef struct tagFontDrvInfo
{
    int32 Devices;    /* device number, useless now */
    int32 FuncMask1;  /* optional functions mask, zero now */
    int32 FuncMask2;  /* extened optional functions mask, zero now */
    
    /* The following funtions are necessary for font driver */
    int32 (*CreateFont)(PFONTDEV pFontDev, PDRVLOGFONT pLogFont);
    int32 (*DestroyFont)(PFONTDEV pFontDev);
    int32 (*EnumFontName)(PFONTDEV pFontDev, PFONTNAMEINFO pInfo, 
                          int16 serial);
    int32 (*EnumFontSize)(PFONTDEV pFontDev, PFONTNAMEINFO pInfo, 
                          int16 serial);
    int32 (*GetCharWidth)(PFONTDEV pFontDev, uint16 Code);
    int32 (*GetFontInfo)(PFONTDEV pFontDev, PFONTINFO pFontInfo);
    int32 (*GetStringExt)(PFONTDEV pFontDev, PSTRING pString, 
                          int StrLen, PTEXTBMP pTextBmp);
    int32 (*GetStringBmp)(PFONTDEV pFontDev, PSTRING pString, 
                          int StrLen, int x, PTEXTBMP pTextBmp);
    int32 (*SetDefaultChar)(PFONTDEV pFontDev, uint16 Code);
    int32 (*GetStringExtEx)(PFONTDEV pFont, PSTRING pStr, 
                            int count, int nMaxExtent, int *pnFit, 
                            int *apDx, PTEXTBMP pTextBmp);
    /* The following functions are optional for font driver, now they are empty */
}FONTDRVINFO, *PFONTDRVINFO;

int32 EnableFont(PFONTDRVINFO pFontDrv);

#endif
