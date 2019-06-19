/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : header file for font driver.
 *            
\**************************************************************************/

#ifndef __FONTDRV_H
#define __FONTDRV_H


//define whether use the font.ini file 
#define INIFONTFILE 0

/*(define the column style, decided by the display screen)*/
//#define _COLUMN_ 0
//Have been defined in the wconfig.h  COLUMN_BITMAP

/*(define the max length of file name)*/
#define MAX_FILENAME    32

/*(the max number of file name length)*/
#define MAX_FONTNAME    16

/*(the max number of font types)*/
#define MAX_FONT_NUM    16

/*(the max number of charset types)*/
#define MAX_CHARSET_NUM 6

//#define BIG5SUPPORT                 // 支持BIG5编码
//Have been defined in the wconfig.h

/*(define the default charset , decided by the system)*/
//想法：设定在系统的初始化文件中，这样可以避免修改源文件
#define _CS_GBK_        0
#define _CS_GB2312_     1
#define _CS_WESTEURP_   0

#define FONT_DIR        "*.fnt"
#if (INIFONTFILE)
/*(define the initial file infomation)*/
#define SYSFONTINFO         "Font="
#define SYSCHARSETINFO      "Charset="
#define FONTINI_NAME            "font.ini"
#endif

#define ISSBEXTCODE(c1, c2) ((unsigned char)(c1) == 0x1c && \
                            (unsigned char)(c2) >= 0x80 && \
                            (unsigned char)(c2) <= 0xfe)

#define InRange(c, lower, upper)  (((c) >= (lower)) && ((c) <= (upper)))

/* information of a created font */
typedef struct tagFontData
{
    uint8       Entry;          /* Index of the physical font array */
    uint8       CsEntry;        /* Index of the charset array   */
    uint8       Style;          /* font style, it's same as that in logfont */
    uint8       Attrib;         /* font Attrib */
    uint8       Height;         /* font height, same as that in logfont */
    uint8       Width;          /* width for each char, useless for variable width font */
    uint8       Charset;        /* charset */
    //uint16      DefChar;        /* default char for this font */
    
}FONTDATA, *PFONTDATA;


/* size information of a fix-size font */
typedef struct tagFixSize
{
    uint16  SizeNo;     /* size number of this font name */
    uint16  ThisSize;   /* size of this font */
}FIXSIZE;

/* size information about a scalable size font */
typedef struct tagVarSize   
{
    uint16  MinSize;    /* minimal size of this font */
    uint16  MaxSize;    /* maximal size of this font */
}VARSIZE;

/* size information about a scalable size font */
typedef struct tagSBCharInfo    
{
    uint8   Width;      /* Width of this char */
    uint8   Offset;     /* Offset in SB data */
    uint8   Reserved0;  /* Reserved */
    uint8   Reserved1;  /* Reserved */
}SBCHARINFO;

/* information of a physical font */
typedef struct tagPhyFont
{
    int8    Name[MAX_FONTNAME];     /* font name */
    int8    FileName[MAX_FILENAME]; /* file name of this font */

    uint8   Family;     /* font family, it decides the profile of a font*/
    //uint8 Unused;     /* only for alignment */
    uint8   Charset;    /* charset of this font, it decides which chars this font contains */
    uint32  Attrib;     /* attribute of this font, currently means equal width or not, fixed size of not */
    union
    {
        FIXSIZE fix;    /* size info when this font is fixed size */
        VARSIZE var;    /* size info when this font is variable size */
    }Size;

    uint8   Height;     /* Height of char, */
    uint8   DBWidth;    /* width of Double bytes char, when variable width, it is the minimal width */
    uint8   MaxWidth;   /* maximal width of this font, useless when equal width */
    uint8   SBWidth;    /* width of Single bytes char, when variable width, it is the minimal width */
    uint8   BaseLine;   /* base line of this font */
    uint8   LeftExt;    /* left extension of this font */
    uint8   RightExt;   /* right extension of this font */
    uint16  RefCount;   /* reference count of the font data stored in pui8Data */
    uint8   *pData;     /* file header*/
    uint32  SBOffset;   /* Single Byte  font data  start address*/
    uint32  DBOffset;   /* Double Byte  font data  start address*/
    uint32  SBCharInfoOffset;   /* Single Byte  char info table data  start address*/
    uint32  DataCount;  /* data sum in bytes */
}PHYFONT, *PPHYFONT;

/* information of a charset */
typedef struct tagCharset
{
    uint8   Entry;      /* charset number, == PHYFONT.i16Charset */
    uint8   DBytes;     /* 1: double bytes code, 0: single byte code */
    uint8   MinLCode;   /* minimal valid value of low/single byte */
    uint8   MaxLCode;   /* maximal valid value of low/single byte */
    uint8   MinHCode;   /* minimal valid value of high byte */
    uint8   MaxHCode;   /* maximal valid value of high byte */
    uint8   Supported;  /* decide whether the charset is supported in the system */
    uint8   Reserved;  /* reserved */
    uint16  DefCharCode;
    uint16  wReserved;
}CHARSET, *PCHARSET;

/* information of a created font */
typedef struct tagPhyFontInfo
{
    uint8       DBEntry;
    uint8       SBEntry;
    uint8       DBWidth;
    uint8       SBWidth;
    uint8       Height;
    uint32      DBAttrib;
    uint32      SBAttrib;
    uint32      DBPageSize;
    uint32      SBInfoAddr;
    uint32      DBCharSize;
    uint32      SBCharSize;
    uint32      DBStartAddr;
    uint32      SBStartAddr;
    PCHARSET    pDBCharset;
    PCHARSET    pSBCharset;
    PFONTDATA   pFontData;

}PHYFONTINFO, *PPHYFONTINFO;

/* Global varibles */
static uint16   PhyFontNo;  /* number of physical font supported by this driver */
static uint16   CharsetNo;  /* number of charset used by all fonts in this driver */
static PPHYFONT ppfArray;       /* array of physical fonts */
static PCHARSET pcsArray;       /* array of charsets */
static PPHYFONTINFO pPhyFontInfo;

static FONTDATA *pCurFontData;  //for test

static char g_SystemFontName[MAX_FONTNAME];
static BYTE g_SystemCharset = FC_SYSTEMCHARSET;

//static PHYFONTINFO PhyFontInfo, PhyFontInfoOld;
//static FONTDATA   AssFontData;
/* forward function declaration of the driver */
static int InitPhysicalFont(void);
static int InitPhyFontInfo(int FontNum);
static int InitCharset(void);

//static BOOL ISDBCSCODE(PFONTDATA pFontData, BYTE c1, BYTE c2);

static int GetFontEntry(PDRVLOGFONT pLogFont);
static int IsValidCode(uint16 Charset, uint16 Code);
static int VerifyCharCode(PFONTDATA pFontData, uint16 Code);
static int IsDBytesFont(PFONTDATA pFontData);

static int GetFontExtend(PFONTDATA pFontData, PSIZE pSize);
static int GetCharSize(PFONTDATA pFontData, uint16 Code);
//static int GetCharBmp(PFONTDATA pFontData, uint16 Code, 
//                      int8 *pCharData, int16 Delta);

static int32 _CreateFont(PFONTDEV pFont, PDRVLOGFONT pLogFont);
static int32 DestroyFont(PFONTDEV pFont);
static int32 EnumFontName(PFONTDEV pFont, PFONTNAMEINFO pInfo, int16 Serial);
static int32 EnumFontSize(PFONTDEV pFont, PFONTNAMEINFO pInfo, int16 Serial);
static int32 GetCharWidth(PFONTDEV pFont, uint16 Code);
static int32 GetFontInfo(PFONTDEV pFont, PFONTINFO pFontInfo);
static int32 GetStringExt(PFONTDEV pFont, PSTRING pString, 
                          int StrLen, PTEXTBMP pTextBmp);
static int32 GetStringBmp(PFONTDEV pFont, PSTRING pString, 
                          int StrLen, int XOffset, PTEXTBMP pTextBmp);
static int32 _SetDefaultChar(PFONTDEV pFont, uint16 Code);

static int32 GetStringExtEx(PFONTDEV pFontDev, PSTRING pString, 
                            int StrLen, int MaxExtent, int *pnFit, 
                            int *apDx, PTEXTBMP pTextBmp);

static void _Combine(uint32 data,int8 *pSrcData);
static uint32 _readlong(unsigned char *pSrcData);
static int GetCharsetEntry(uint8 charsetno);
static int GetFontAssEntry(int Entry);

#if (BIG5SUPPORT)
void Big5ToGB2312(PSTR pszGB2312, PCSTR pszBig5, int nCount);
#endif

#endif // __FONTDRV_H
