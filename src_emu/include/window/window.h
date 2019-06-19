/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Define definition, typedef and function prototype for
 *            window libary functions.
 *            
\**************************************************************************/

#ifndef __WINDOW_H
#define __WINDOW_H

#define BASEWINDOW  0

#include "hpdef.h"          /* HopenOS basic type definition    */

/* Assume C declarations for C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define MSWIN_COMPATIBLE

/**************************************************************************/
/*          Common definitions and typedefs                               */
/**************************************************************************/

#define CALLBACK        
#define WINAPI          
#define APIENTRY        WINAPI

/* Types use for passing & returning polymorphic values */

typedef LONG    WPARAM;             /* wParam   */
typedef LONG    LPARAM;             /* lParam   */
typedef LONG    LRESULT;            /* l        */

#define MAKEWPARAM(low, high)   ((WPARAM)MAKELONG(low, high))
#define MAKELPARAM(low, high)   ((LPARAM)MAKELONG(low, high))
#define MAKELRESULT(low, high)  ((LRESULT)MAKELONG(low, high))

/**************************************************************************/
/*                  KERNEL typedefs, structures, and functions            */
/**************************************************************************/

DECLARE_HANDLE(HINSTANCE);
typedef HINSTANCE HMODULE; /* HMODULEs can be used in place of HINSTANCEs */

/**************************************************************************/
/*          Main application entry point function                         */
/**************************************************************************/


#ifdef _MSC_VER
#define WinMain HopenWinMain
#endif

int     WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                       LPSTR lpCmdLine, int nCmdShow); 

/**************************************************************************/
/*          Application Management                                        */
/**************************************************************************/

#define NOAPPMGR

#ifndef NOAPPMGR

/* XXXX_AppControl command constant */
#define APP_INIT            0
#define APP_INITINSTANCE    1
#define APP_GETICON         2
#define APP_ACTIVE          3
#define APP_INACTIVE        4
#define APP_GETSTATE        5

/* Application state constant for APP_GETSTATE command*/
#define AS_NOTRUNNING       0
#define AS_INACTIVE         1
#define AS_ACTIVE           2

#endif /* NOAPPMGR */

/**************************************************************************/
/*              Error Code Define                                         */
/**************************************************************************/

#define CLR_INVALID         0xFFFFFFFFL

/**************************************************************************/
/*              System Time Functions                                     */
/**************************************************************************/

#ifndef NOSYSTEMTIME

#ifndef _SYSTEMTIME_
#define _SYSTEMTIME_

typedef struct _SYSTEMTIME 
{  
    WORD wYear;     
    WORD wMonth; 
    WORD wDayOfWeek;     
    WORD wDay;     
    WORD wHour;     
    WORD wMinute; 
    WORD wSecond;     
    WORD wMilliseconds; 
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME; 

#endif

void    WINAPI GetSystemTime(PSYSTEMTIME pSystemTime);
BOOL    WINAPI SetSystemTime(const SYSTEMTIME *pSystemTime);

void    WINAPI GetLocalTime(PSYSTEMTIME pSystemTime);
BOOL    WINAPI SetLocalTime(const SYSTEMTIME* pSystemTime);

#ifndef NOTIMEZONE

typedef struct _TIME_ZONE_INFORMATION 
{
    LONG Bias;
    char StandardName[32];
    SYSTEMTIME StandardDate;
    LONG StandardBias;
    char DaylightName[32];
    SYSTEMTIME DaylightDate;
    LONG DaylightBias;
} TIME_ZONE_INFORMATION, *PTIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

BOOL    WINAPI SystemTimeToTzSpecificLocalTime(
                    PTIME_ZONE_INFORMATION pTimeZoneInformation,
                    PSYSTEMTIME pUniversalTime,
                    PSYSTEMTIME pLocalTime);

/* GetTimeZoneInfomation return value */
#define TIME_ZONE_ID_UNKNOWN  0
#define TIME_ZONE_ID_STANDARD 1
#define TIME_ZONE_ID_DAYLIGHT 2

DWORD   WINAPI GetTimeZoneInformation(
                   PTIME_ZONE_INFORMATION pTimeZoneInformation);
BOOL    WINAPI SetTimeZoneInformation(
                   const TIME_ZONE_INFORMATION* pTimeZoneInformation);

#endif /* NOTIMEZONE */

#endif /* NOSYSTEMTIME */

DWORD   WINAPI GetTickCount(void);

/**************************************************************************/
/*                  Error Functions                                       */
/**************************************************************************/

int     WINAPI GetLastError(void);
void    WINAPI SetLastError(int errorno);

/**************************************************************************/
/*              Resource  Functions                                       */
/**************************************************************************/

#ifndef NORESOURCE

HINSTANCE WINAPI RegisterResource(HINSTANCE hInst, const void* pRes,
                                  long lOffset, BOOL bFile);
BOOL      WINAPI UnregisterResource (HINSTANCE hInst);

DECLARE_HANDLE(HRSRC);

HRSRC   WINAPI FindResource(HINSTANCE hInst, PCSTR pName, PCSTR pType);
HANDLE  WINAPI LoadResource(HINSTANCE hInst, HRSRC hResInfo);
void*   WINAPI LockResource(HANDLE hResData);
BOOL    WINAPI FreeResource(HANDLE hResData);

#define MAKEINTRESOURCE(i)  ((PCSTR)(i))

/* Predefined Resource Types */
#define RT_CURSOR           MAKEINTRESOURCE(1)
#define RT_BITMAP           MAKEINTRESOURCE(2)
#define RT_ICON             MAKEINTRESOURCE(3)
#define RT_MENU             MAKEINTRESOURCE(4)
#define RT_DIALOG           MAKEINTRESOURCE(5)
#define RT_STRING           MAKEINTRESOURCE(6)
#define RT_ACCELERATOR      MAKEINTRESOURCE(7)
#define RT_RCDATA           MAKEINTRESOURCE(8)

#endif  /* NORESOURCE */

#define IMAGE_BITMAP        0
#define IMAGE_ICON          1
#define IMAGE_CURSOR        2

#define LR_LOADFROMFILE     0x0010

HANDLE  WINAPI LoadImage(HINSTANCE hInst, PCSTR pszName, UINT uType, 
                         int cxDesired, int cyDesired, UINT fuLoad);

/**************************************************************************/
/*              International & Char Translation Support                  */
/**************************************************************************/

#ifndef NODBCS
BOOL    WINAPI IsDBCSLeadByte(int byTestChar);
#endif  /* NODBCS */

int     WINAPI LoadString(HINSTANCE hInstance, UINT uID, PSTR pBuffer, 
                          int nBufferMax); 
PCSTR   WINAPI LoadStringPtr(HINSTANCE hInstance, UINT uID); 

/*
 *  Code Page Default Values.
 */
#define CP_ACP					0              /* default to ANSI code page */
#define CP_OEMCP				1              /* default to OEM  code page */
#define CP_MACCP				2              /* default to MAC  code page */

#define CP_UTF7					65000          /* UTF-7 translation */
#define CP_UTF8					65001          /* UTF-8 translation */

#define CP_JVM                  32767          /* special code page for JVM */

typedef unsigned short			WCHAR;
typedef WCHAR FAR *				LPWSTR;
typedef const WCHAR FAR *		LPCWSTR;

int WINAPI MultiByteToWideChar(
                    UINT     CodePage,
                    DWORD    dwFlags,
                    LPCSTR   lpMultiByteStr,
                    int      cchMultiByte,
                    LPWSTR   lpWideCharStr,
                    int      cchWideChar);

int WINAPI WideCharToMultiByte(
                    UINT     CodePage,
                    DWORD    dwFlags,
                    LPCWSTR  lpWideCharStr,
                    int      cchWideChar,
                    LPSTR    lpMultiByteStr,
                    int      cchMultiByte,
                    LPCSTR   lpDefaultChar,
                    LPBOOL   lpUsedDefaultChar);

int WINAPI MultiByteToUTF8(
                    UINT     CodePage,
                    DWORD    dwFlags,
                    LPCSTR   lpMultiByteStr,
                    int      cchMultiByte,
                    LPSTR    lpUtf8CharStr,
                    int      cUtf8Char);

int WINAPI UTF8ToMultiByte(
                    UINT     CodePage,
                    DWORD    dwFlags,
                    LPCSTR   lpUtf8CharStr,
                    int      cUtf8Char,
                    LPSTR    lpMultiByteStr,
                    int      cchMultiByte,
                    LPCSTR   lpDefaultChar,
                    LPBOOL   lpUsedDefaultChar);
/*
** GSM code convert function
*/
int     WINAPI GSMToMultiByte(LPCSTR lpGSMStr, int cchGSM, 
                        LPWSTR lpMultiByteStr, int cchMultiByte, 
                        LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);
int     WINAPI MultiByteToGSM(LPCSTR lpMultiByteStr, int cchMultiByte, 
                        LPWSTR lpGSMStr, int cchGSM, 
                        LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);
int     WINAPI IsGSMCode(BYTE c1, BYTE c2);
BOOL    WINAPI IsGSMString(LPCSTR lpGSMStr, int cchGSM);

/**************************************************************************/
/*              GDI typedefs, structures, and functions                   */
/**************************************************************************/

DECLARE_HANDLE(HDC);

#ifndef NOGDI

#define GDI_ERROR   (0xFFFFFFFFL)

typedef const void * HGDIOBJ;

#endif  /* NOGDI */

DECLARE_HANDLE(HBITMAP);
DECLARE_HANDLE(HPEN);
DECLARE_HANDLE(HBRUSH);
DECLARE_HANDLE(HRGN);
DECLARE_HANDLE(HPALETTE);
DECLARE_HANDLE(HFONT);

typedef struct tagRECT      /* rc   */
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT, *PRECT, *LPRECT;

typedef struct tagPOINT    /* pt    */
{
    LONG    x;
    LONG    y;
} POINT, *PPOINT, *LPPOINT;

typedef struct tagSIZE
{
    LONG    cx;
    LONG    cy;
} SIZE, *PSIZE, *LPSIZE;

#define LONGTOPOINT(pt, l)                 \
        { (pt).x = (LONG)(SHORT)LOWORD(l);   \
          (pt).y = (LONG)(SHORT)HIWORD(l); }

#define POINTTOLONG(pt) (MAKELONG((short)((pt).x), (short)((pt).y)))

#ifdef MSWIN_COMPATIBLE

typedef struct tagPOINTS
{
    SHORT   x;
    SHORT   y;
} POINTS, *PPOINTS, *LPPOINTS;

#define MAKEPOINTS(l)   (*((POINTS*)&(l)))

#define POINTSTOPOINT(pt, pts)                          \
        { (pt).x = (LONG)(SHORT)LOWORD(*(LONG*)&pts);   \
          (pt).y = (LONG)(SHORT)HIWORD(*(LONG*)&pts); }

#define POINTTOPOINTS(pt) (MAKELONG((short)((pt).x), (short)((pt).y)))

#endif /* MSWIN_COMPATIBLE */

#ifndef NOGDI

/**************************************************************************/
/*              Color support                                             */
/**************************************************************************/

typedef DWORD               COLORREF;
typedef COLORREF*           PCOLORREF;

#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)(g)<<8))|\
                            (((DWORD)(BYTE)(b))<<16)))

#define GetRValue(rgb)      ((BYTE)(rgb))
#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      ((BYTE)((rgb)>>16))

#ifndef NOCOLOR

/* Common color constant */
#define COLOR_BLACK         0x00000000
#define COLOR_BLUE          0x00AB0000
#define COLOR_GREEN         0x0000AB00
#define COLOR_CYAN          0x00ABAB00
#define COLOR_RED           0x000000AB
#define COLOR_MAGENTA       0x00AB00AB
#define COLOR_BROWN         0x000057AB
#define COLOR_LIGHTGRAY     0x00C0C0C0
#define COLOR_DARKGRAY      0x00808080
#define COLOR_LIGHTBLUE     0x00FF5757
#define COLOR_LIGHTGREEN    0x0057FF57
#define COLOR_LIGHTCYAN     0x00FFFF57
#define COLOR_LIGHTRED      0x005757FF
#define COLOR_LIGHTMAGENTA  0x00FF57FF
#define COLOR_YELLOW        0x0057FFFF
#define COLOR_WHITE         0x00FFFFFF

/* GetSysColor and SetSysColors */
COLORREF WINAPI GetSysColor(int nIndex);
BOOL     WINAPI SetSysColors(int nCount, const int* pIndexs, 
                             const COLORREF* pColors);

/* Constant for system color index */
#define COLOR_SCROLLBAR             0
#define COLOR_BACKGROUND            1
#define COLOR_ACTIVECAPTION         2
#define COLOR_INACTIVECAPTION       3
#define COLOR_MENU                  4
#define COLOR_WINDOW                5
#define COLOR_WINDOWFRAME           6
#define COLOR_MENUTEXT              7
#define COLOR_WINDOWTEXT            8
#define COLOR_CAPTIONTEXT           9
#define COLOR_ACTIVEBORDER         10
#define COLOR_INACTIVEBORDER       11
#define COLOR_HIGHLIGHT            12
#define COLOR_HIGHLIGHTTEXT        13
#define COLOR_BTNFACE              14
#define COLOR_BTNSHADOW            15
#define COLOR_GRAYTEXT             16
#define COLOR_BTNTEXT              17
#define COLOR_INACTIVECAPTIONTEXT  18
#define COLOR_BTNHIGHLIGHT         19
#define COLOR_MAX                  19 

#endif  /* NOCOLOR */

/**************************************************************************/
/*              DC Management                                             */
/**************************************************************************/
HDC     WINAPI CreateDC(LPCTSTR lpszDriver, LPCTSTR lpszDevice, 
                    LPCTSTR lpszOutput,  const void* lpInitData);
HDC     WINAPI CreateScreenDC(void);
HDC     WINAPI CreateCompatibleDC(HDC hdc);
HDC     WINAPI CreateMemoryDC(int nWidth, int nHeight);

BOOL    WINAPI DeleteDC(HDC hdc);

typedef struct     // di 
{
    int     cbSize; 
    LPCSTR  lpszDocName; 
    LPCSTR  lpszOutput;     // ignored
    LPCSTR  lpszDatatype;   // ignored
    DWORD   fwType;         // ignored
} DOCINFO, *PDOCINFO; 

HDC     WINAPI CreatePrinterDC(int width, int height);
int     WINAPI StartDoc(HDC hdc, const DOCINFO* pDocInfo);
int     WINAPI EndDoc(HDC hdc);
int     WINAPI StartPage(HDC hdc);
int     WINAPI EndPage(HDC hdc);
int     WINAPI StartBand(HDC hdc, PRECT pRect);
int     WINAPI EndBand(HDC hdc);

COLORREF WINAPI SetTextColor(HDC hdc, COLORREF color);
COLORREF WINAPI GetTextColor(HDC hdc);

COLORREF WINAPI SetBkColor(HDC hdc, COLORREF color);
COLORREF WINAPI GetBkColor(HDC hdc);

UINT    WINAPI SetTextAlign(HDC hdc, UINT uMode);
UINT    WINAPI GetTextAlign(HDC hdc);

/* Text Alignment Options */
#define TA_LEFT             0x0000      /* 0000 */
#define TA_RIGHT            0x0001      /* 0001 */
#define TA_CENTER           0x0003      /* 0011 */
#define TA_TOP              0x0000      /* 0000 */
#define TA_BOTTOM           0x0004      /* 0100 */
#define TA_VCENTER          0x000C      /* 1100 */

#define TA_HCENTER          TA_CENTER   /* 0011 */

int     WINAPI SetBkMode(HDC hdc, int nBkMode);
int     WINAPI GetBkMode(HDC hdc);

/* Define constant for background modes */
#define BM_TRANSPARENT      1               /* Background remains untouched. */
#define BM_OPAQUE           2               /* Background is filled with the
     current background color before the text, hatched brush, or pen is drawn.*/
#define BM_NEWTRANSPARENT   3               /* Background remains untouched used s
    pecified color*/
#define BM_ALPHATRANSPARENT 4    /* Background remains untouched in alpha channel. */
#define BM_NEWALPHATRANSPARENT 5 /* Background remains untouched used s
    pecified color in alpha channel*/

#ifdef MSWIN_COMPATIBLE
#define TRANSPARENT     BM_TRANSPARENT      /* Background remains untouched. */
#define OPAQUE          BM_OPAQUE           /* Background is filled with the current background color before the text, hatched brush, or pen is drawn.*/
#define NEWTRANSPARENT  BM_NEWTRANSPARENT   /* Background remains untouched used s
    pecified color*/
#define ALPHATRANSPARENT BM_ALPHATRANSPARENT    
#define NEWALPHATRANSPARENT BM_NEWALPHATRANSPARENT
#endif

int     WINAPI SetROP2(HDC hdc, int nRop);
int     WINAPI GetROP2(HDC hdc);

#ifndef NORASTEROPS

/* Binary raster ops */
#define ROP_SET_DST_0       1   /* dst = 0 */
#define ROP_NSRC_AND_NDST   2   /* !src && !dst */
#define ROP_NSRC_AND_DST    3   /* !src && dst*/
#define ROP_NSRC            4   /* !src */
#define ROP_SRC_AND_NDST    5   /* src && !dst */
#define ROP_NDST            6   /* !dst */
#define ROP_SRC_XOR_DST     7   /* src ^ dst */
#define ROP_NSRC_OR_NDST    8   /* !src || !dst */
#define ROP_SRC_AND_DST     9   /* src && dst */
#define ROP_NSRC_XOR_DST    10  /* !src ^ dst */
#define ROP_DST             11   /* dst */
#define ROP_NSRC_OR_DST     12  /* !src || dst */
#define ROP_SRC             13  /* src */
#define ROP_SRC_OR_NDST     14  /* src || !dst */
#define ROP_SRC_OR_DST      15  /* src || dst */
#define ROP_SET_DST_1       16  /* dst = 1 */

#ifdef MSWIN_COMPATIBLE                         /* Reverse Polish Notation*/
#define R2_BLACK            ROP_SET_DST_0       /*  0                   */
#define R2_NOTMERGEPEN      ROP_NSRC_AND_NDST   /*  DPon                */
#define R2_MASKNOTPEN       ROP_NSRC_AND_DST    /*  DPna                */
#define R2_NOTCOPYPEN       ROP_NSRC            /*  Pn                  */
#define R2_MASKPENNOT       ROP_SRC_AND_NDST    /*  PDna                */
#define R2_NOT              ROP_NDST            /*  Dn                  */
#define R2_XORPEN           ROP_SRC_XOR_DST     /*  DPx                 */
#define R2_NOTMASKPEN       ROP_NSRC_OR_NDST    /*  DPan                */
#define R2_MASKPEN          ROP_SRC_AND_DST     /*  DPa                 */
#define R2_NOTXORPEN        ROP_NSRC_XOR_DST    /*  DPxn                */
#define R2_NOP              ROP_DST             /*  D                   */
#define R2_MERGENOTPEN      ROP_NSRC_OR_DST     /*  DPno                */
#define R2_COPYPEN          ROP_SRC             /*  P                   */
#define R2_MERGEPENNOT      ROP_SRC_OR_NDST     /*  PDno                */
#define R2_MERGEPEN         ROP_SRC_OR_DST      /*  DPo                 */
#define R2_WHITE            ROP_SET_DST_1       /*  1                   */

#endif /* MSWIN_COMPATIBLE */

/* Ternary raster operations */
#define SRCCOPY             (DWORD)0x00CC0020 /* source                   */
#define SRCPAINT            (DWORD)0x00EE0086 /* source OR dest           */
#define SRCAND              (DWORD)0x008800C6 /* source AND dest          */
#define SRCINVERT           (DWORD)0x00660046 /* source XOR dest          */
#define SRCERASE            (DWORD)0x00440328 /* source AND (NOT dest )   */
#define NOTSRCCOPY          (DWORD)0x00330008 /* (NOT source)             */
#define NOTSRCERASE         (DWORD)0x001100A6 /* (NOT src) AND (NOT dest) */
#define MERGECOPY           (DWORD)0x00C000CA /* (source AND pattern)     */
#define MERGEPAINT          (DWORD)0x00BB0226 /* (NOT source) OR dest     */
#define PATCOPY             (DWORD)0x00F00021 /* pattern                  */
#define PATPAINT            (DWORD)0x00FB0A09 /* DPSnoo                   */
#define PATINVERT           (DWORD)0x005A0049 /* pattern XOR dest         */
#define DSTINVERT           (DWORD)0x00550009 /* (NOT dest)               */
#define BLACKNESS           (DWORD)0x00000042 /* BLACK                    */
#define WHITENESS           (DWORD)0x00FF0062 /* WHITE                    */

#endif /* NORASTEROPS */

/**************************************************************************/
/*              Device Capabilities                                       */
/**************************************************************************/

int     WINAPI GetDeviceCaps(HDC hdc, int nIndex);

/* Device Parameters for GetDeviceCaps() */
#define HORZSIZE      0
#define VERTSIZE      1
#define HORZRES       2
#define VERTRES       3
#define BITSPIXEL     4
#define PLANES        5

/**************************************************************************/
/*              Coordinate transformation support                         */
/**************************************************************************/

int     WINAPI SetMapMode(HDC hdc, int nMapMode);
int     WINAPI GetMapMode(HDC hdc);

/* Map modes */
#define MM_TEXT             1
#define MM_LOMETRIC         2
#define MM_HIMETRIC         3
#define MM_LOENGLISH        4
#define MM_HIENGLISH        5
#define MM_TWIPS            6
#define MM_ISOTROPIC        7
#define MM_ANISOTROPIC      8

/* Min and Max Mapping Mode values */
#define MM_MIN              MM_TEXT
#define MM_MAX              MM_ANISOTROPIC
#define MM_MAX_FIXEDSCALE   MM_TWIPS

BOOL    WINAPI SetWindowOrgEx(HDC hdc, int x, int y, PPOINT pPoint);
BOOL    WINAPI GetWindowOrgEx(HDC hdc, PPOINT pPoint);

BOOL    WINAPI SetWindowExtEx(HDC hdc, int nXExtent, int nYExtent, 
                              PSIZE pSize);
BOOL    WINAPI GetWindowExtEx(HDC hdc, PSIZE pSize);

BOOL    WINAPI OffsetWindowOrgEx(HDC hdc, int nXOffset, int nYOffset, 
                                 PPOINT pPoint);
BOOL    WINAPI ScaleWindowExtEx(HDC hdc, int Xnum, int Xdenom, int Ynum, 
                                int Ydenom, PSIZE pSize);

BOOL    WINAPI SetViewportExtEx(HDC hdc, int nXExtent, int nYExtent, 
                                PSIZE pSize);
BOOL    WINAPI GetViewportExtEx(HDC hdc, PSIZE pSize);

BOOL    WINAPI SetViewportOrgEx(HDC hdc, int x, int y, PPOINT pPoint);
BOOL    WINAPI GetViewportOrgEx(HDC hdc, PPOINT pPoint);

BOOL    WINAPI OffsetViewportOrgEx(HDC hdc, int nXOffset, int nYOffset, 
                                   PPOINT pPoint);
BOOL    WINAPI ScaleViewportExtEx(HDC hdc, int Xnum, int Xdenom, int Ynum, 
                                  int Ydenom, PSIZE pSize);

BOOL    WINAPI DPtoLP(HDC hdc, PPOINT pPoints, int nCount);
BOOL    WINAPI LPtoDP(HDC hdc, PPOINT pPoints, int nCount);

BOOL    WINAPI SetClipRect(HDC hdc, const RECT* pRect);

/**************************************************************************/
/*              GDI Object Support                                        */
/**************************************************************************/

/* Stock pens for use with GetStockObject()     */

#define NULL_PEN            0
#define BLACK_PEN           1
#define DKGRAY_PEN          2
#define GRAY_PEN            3
#define LTGRAY_PEN          4
#define WTGRAY_PEN          5
#define WHITE_PEN           6

/* Stock brushes for use with GetStockObject()  */

#define NULL_BRUSH          7
#define BLACK_BRUSH         8
#define DKGRAY_BRUSH        9
#define GRAY_BRUSH          10
#define LTGRAY_BRUSH        11
#define WTGRAY_BRUSH        12
#define WHITE_BRUSH         13

#define HOLLOW_BRUSH        NULL_BRUSH
#define DARKGRAY_BRUSH      DKGRAY_BRUSH
#define LIGHTGRAY_BRUSH     LTGRAY_BRUSH

/* Stock fonts for use with GetStockObject      */
#define SYSTEM_FONT         14

HGDIOBJ WINAPI GetStockObject(int nObjID);

BOOL    WINAPI DeleteObject(HGDIOBJ hGdiObj);
HGDIOBJ WINAPI SelectObject(HDC hdc, HGDIOBJ hGdiObj);
int     WINAPI GetObject(HGDIOBJ hGdiObj, int nBufferMax, void* pObject);
BOOL    WINAPI UnrealizeObject(HGDIOBJ hGdi);

/**************************************************************************/
/*              Pen Support                                               */
/**************************************************************************/

/* Define constant for pen style */
#define PS_SOLID        0       /*  The pen is solid. */
#define PS_DASH         1       /* The pen is dashed. This style is valid only 
        when the pen width is one or less in device units. -------   */
#define PS_DOT          2       /* The pen is dotted. This style is valid only 
        when the pen width is one or less in device units.  .......   */
#define PS_DASHDOT      3       /* The pen has alternating dashes and dots. This style is valid only 
        when the pen width is one or less in device units.   _._._._   */
#define PS_DASHDOTDOT   4       /* The pen has alternating dashes and double dots. This style is valid only 
        when the pen width is one or less in device units. _.._.._   */
#define PS_NULL         5       /* The pen is invisible.*/
#define PS_INSIDEFRAME  6       /*The pen is solid. When this pen is used in any GDI drawing function that 
                takes a bounding rectangle, the dimensions of the figure are 
                shrunk so that it fits entirely in the bounding rectangle, 
                taking into account the width of the pen. This applies only 
                to geometric pens. */
#define PS_LAST         6

HPEN    WINAPI CreatePen(int nStyle, int nWidth, COLORREF color);

#ifdef MSWIN_COMPATIBLE

/* Logical Pen */
typedef struct tagLOGPEN        /* lgpn */
{
    UINT    lopnStyle;
    POINT   lopnWidth;
    COLORREF lopnColor;
} LOGPEN, *PLOGPEN, *LPLOGPEN;

HPEN    WINAPI CreatePenIndirect(const LOGPEN* pLogPen);

#endif /* MSWIN_COMPATIBLE */

/**************************************************************************/
/*              Brush Support                                             */
/**************************************************************************/

/* Define constant for brush style */
#define BS_HOLLOW       0       /* Hollow brush.*/
#define BS_NULL         0       /* Hollow brush.*/
#define BS_SOLID        1       /* Solid brush.*/
#define BS_DIBPATTERN   2       /* A pattern brush defined by a 
        device-independent bitmap (DIB) specification. 
        If lbStyle is BS_DIBPATTERN, the lbHatch member contains 
        a handle to a packed DIB. For more information, see discussion 
        in lbHatch. */
#define BS_HATCHED      3       /*Hatched brush.*/
#define BS_PATTERN      4       /* Pattern brush defined by a memory bitmap.*/
#define BS_LAST         4

/* Define constant for hatch style */
#define HS_BDIAGONAL    0       /* A 45-degree upward, left-to-right hatch*/
#define HS_CROSS        1       /* Horizontal and vertical cross-hatch */
#define HS_DIAGCROSS    2       /* 45-degree crosshatch */
#define HS_FDIAGONAL    3       /* A 45-degree downward, left-to-right hatch*/
#define HS_HORIZONTAL   4       /* Horizontal hatch */
#define HS_VERTICAL     5       /*  Vertical hatch */
#define HS_SIEVE        6       /* sieve*/
#define HS_GRAY         6       /*gray sieve*/
#define HS_LIGHTGRAY    7       /*ligth gray sieve*/
#define HS_WALL         8       /*wall*/
#define HS_LAST         8       /* Last hatch style        */

HBRUSH  WINAPI CreateBrush(int nStyle, COLORREF color, LONG lHatch);

#ifdef MSWIN_COMPATIBLE

/* Logical Brush (or Pattern) */
typedef struct tagLOGBRUSH      /* lb   */
{
    UINT        lbStyle;
    COLORREF    lbColor;
    LONG        lbHatch;
} LOGBRUSH, *PLOGBRUSH, *LPLOGBRUSH;

typedef LOGBRUSH    PATTERN;
typedef PATTERN*    PPATTERN;
typedef PATTERN*    LPPATTERN;

HBRUSH  WINAPI CreateSolidBrush(COLORREF color);
HBRUSH  WINAPI CreateHatchBrush(int nHatchStyle, COLORREF color);
HBRUSH  WINAPI CreatePatternBrush(HBITMAP hBitmap);
HBRUSH  WINAPI CreateBrushIndirect(const LOGBRUSH* pLogBrush);

#endif /* MSWIN_COMPATIBLE */

BOOL    WINAPI SetBrushOrg(HDC hdc, int nOrgX, int nOrgY, PPOINT pptPrev);

#ifdef MSWIN_COMPATIBLE
#define SetBrushOrgEx   SetBrushOrg
#endif /* MSWIN_COMPATIBLE */

/**************************************************************************/
/*              Region Support                                            */
/**************************************************************************/

HRGN    WINAPI CreateRectRgn(int x1, int y1, int x2, int y2);
HRGN    WINAPI CreateRectRgnIndirect(const RECT* lprc);

/* Region type flags */
#define ERROR               0
#define NULLREGION          1
#define SIMPLEREGION        2
#define COMPLEXREGION       3
#define RGN_ERROR           ERROR

void    WINAPI SetRectRgn(HRGN hrgn, int x1, int y1, int x2, int y2);

int     WINAPI CombineRgn(HRGN hrgnDest, HRGN hrgnSrc1, HRGN hrgnSrc2, 
                          int nCombineMode);

/* CombineRgn() command values */
#define RGN_AND             1
#define RGN_OR              2
#define RGN_XOR             3
#define RGN_DIFF            4
#define RGN_COPY            5
#define RGN_MIN             RGN_AND
#define RGN_MAX             RGN_COPY

BOOL    WINAPI EqualRgn(HRGN hrgn1, HRGN hrgn2);
int     WINAPI OffsetRgn(HRGN hrgn, int nOffsetX, int nOffsetY);

int     WINAPI GetRgnBox(HRGN hrgn, RECT* pRect);

BOOL    WINAPI RectInRegion(HRGN hrgn, const RECT* pRect);
BOOL    WINAPI PtInRegion(HRGN hrgn, int x, int y);

/**************************************************************************/
/*              Color Palette Support                                     */
/**************************************************************************/

/**************************************************************************/
/*              Clipping support                                          */
/**************************************************************************/

int     WINAPI SelectClipRgn(HDC hdc, HRGN hrgn);
int     WINAPI ExtSelectClipRgn(HDC hdc, HRGN hrgn, int nMode);
int     WINAPI GetClipBox(HDC hdc, RECT* pRect);

int     WINAPI IntersectClipRect(HDC hdc, int x1, int y1, int x2, int y2);
int     WINAPI OffsetClipRgn(HDC hdc, int x, int y);
int     WINAPI ExcludeClipRect(HDC hdc, int x1, int y1, int x2, int y2);

BOOL    WINAPI PtVisible(HDC hdc, int x, int y);
BOOL    WINAPI RectVisible(HDC hdc, const RECT* pRect);

/**************************************************************************/
/*              General drawing support                                   */
/**************************************************************************/
#define AD_COUNTERCLOCKWISE 1
#define AD_CLOCKWISE        2

COLORREF WINAPI	SetPixel(HDC hdc, int x, int y, COLORREF crColor);
COLORREF WINAPI	GetPixel(HDC hdc, int x, int y);

BOOL    WINAPI DrawLine(HDC hdc, int x1, int y1, int x2, int y2);
BOOL    WINAPI DrawRect(HDC hdc, const RECT* pRect);
BOOL    WINAPI ClearRect(HDC hdc, const RECT* pRect, COLORREF color);
BOOL    WINAPI DrawCircle(HDC hdc, int x, int y, int r);

BOOL    WINAPI Ellipse(HDC hdc, int x1, int y1, int x2, int y2);
BOOL    WINAPI AngleArc(HDC hdc, int x, int y, DWORD dwRadius, 
                     double eStartAngle, double eSweepAngle);
BOOL    WINAPI Arc(HDC hdc, int nLeftRect, int nTopRect, int nRightRect,
                int nBottomRect, int nXStartArc, int nYStartArc, 
                int nXEndArc, int nYEndArc);
BOOL    WINAPI ArcTo(HDC hdc, int nLeftRect, int nTopRect, int nRightRect,
                  int nBottomRect, int nXRadial1, int nYRadial1, 
                  int nXRadial2, int nYRadial2);
BOOL    WINAPI Chord(HDC hdc, int nLeftRect, int nTopRect, int nRightRect,
                  int nBottomRect, int nXRadial1, int nYRadial1, 
                  int nXRadial2, int nYRadial2);
BOOL    WINAPI Pie(HDC hdc, int nLeftRect, int nTopRect, int nRightRect,
                  int nBottomRect, int nXRadial1, int nYRadial1, 
                  int nXRadial2, int nYRadial2);
BOOL    WINAPI Polygon(HDC hdc, POINT* ppoints, int count);

int     WINAPI SetArcDirection(HDC hdc, int ArcDirection);
int     WINAPI GetArcDirection(HDC hdc);

#define FillSolidRect ClearRect

#ifdef MSWIN_COMPATIBLE

BOOL    WINAPI MoveToEx(HDC hdc, int x, int y, PPOINT pPoint);
BOOL    WINAPI GetCurrentPositionEx(HDC hdc, PPOINT pPoint);

#define MoveTo                  MoveToEx
#define GetCurrentPosition      GetCurrentPositionEx

BOOL    WINAPI LineTo(HDC hdc, int x, int y);

BOOL    WINAPI Rectangle(HDC hdc, int x1, int y1, int x2, int y2);
BOOL    WINAPI RoundRect(HDC hdc, int x1, int y1, int x2, int y2, 
                         int nWidth, int nHeight);

/* Rectangle output routines */
BOOL    WINAPI FillRect(HDC hdc, const RECT* pRect, HBRUSH hBrush);
BOOL    WINAPI FrameRect(HDC  hdc, const RECT* pRect, HBRUSH hBrush);
BOOL    WINAPI InvertRect(HDC hdc, const RECT* pRect);

#endif /* MSWIN_COMPATIBLE */

/**************************************************************************/
/*              Text Functions                                            */
/**************************************************************************/
DWORD   WINAPI SetDefaultChar(HDC hdc, DWORD dwChar);
BOOL    WINAPI TextOut(HDC hdc, int x, int y, PCSTR pStr, int nCount);

BOOL    WINAPI GetTextExtentPoint(HDC hdc, PCSTR pStr, int nCount, 
                                  PSIZE pSize);
BOOL    WINAPI GetTextExtentExPoint(HDC hdc, PCSTR pStr, int nCount, 
                                 int nMaxExtent, PINT lpnFit, 
                                 PINT alpDx, LPSIZE pSize);

#define GetTextExtent           GetTextExtentPoint
#define GetTextExtentPoint32    GetTextExtentPoint

/* DrawText() Format Flags */
#ifndef NODRAWTEXT
#define DT_TOP              0x0000
#define DT_LEFT             0x0000
#define DT_CENTER           0x0001
#define DT_RIGHT            0x0002
#define DT_VCENTER          0x0004
#define DT_BOTTOM           0x0008
#define DT_NOCLIP           0x0100
#define DT_CLEAR            0x1000

#define DT_HCENTER          DT_CENTER

int     WINAPI DrawText(HDC hdc, PCSTR pStr, int nCount, PRECT pRect, 
                        UINT uFormat);
#endif  /* NODRAWTEXT */

/**************************************************************************/
/*              Font Support                                              */
/**************************************************************************/

#ifndef NOGDIOBJ

/* Logical Font */

#define LF_FACESIZE         32

typedef struct tagLOGFONT
{
    LONG    lfHeight;
    LONG    lfWidth;
    LONG    lfEscapement;
    LONG    lfOrientation;
    LONG    lfWeight;
    BYTE    lfItalic;
    BYTE    lfUnderline;
    BYTE    lfStrikeOut;
    BYTE    lfCharSet;
    BYTE    lfOutPrecision;
    BYTE    lfClipPrecision;
    BYTE    lfQuality;
    BYTE    lfPitchAndFamily;
    char    lfFaceName[LF_FACESIZE];
} LOGFONT, *PLOGFONT, *LPLOGFONT;

/* Font Weights */
#define FW_DONTCARE         0
#define FW_THIN             100
#define FW_EXTRALIGHT       200
#define FW_LIGHT            300
#define FW_NORMAL           400
#define FW_MEDIUM           500
#define FW_SEMIBOLD         600
#define FW_BOLD             700
#define FW_EXTRABOLD        800
#define FW_HEAVY            900

#define FW_ULTRALIGHT       FW_EXTRALIGHT
#define FW_REGULAR          FW_NORMAL
#define FW_DEMIBOLD         FW_SEMIBOLD
#define FW_ULTRABOLD        FW_EXTRABOLD
#define FW_BLACK            FW_HEAVY

/* CharSet values */
#define ANSI_CHARSET                0
#define DEFAULT_CHARSET             1
#define SYMBOL_CHARSET              2
#define SHIFTJIS_CHARSET            128
#define HANGEUL_CHARSET             129
#define GB2312_CHARSET              134
#define CHINESEBIG5_CHARSET         136
#define OEM_CHARSET                 255
#define GSM_CHARSET					200
#define WESTEUROPE_CHARSET          254

/* OutPrecision values */
#define OUT_DEFAULT_PRECIS          0
#define OUT_STRING_PRECIS           1
#define OUT_CHARACTER_PRECIS        2
#define OUT_STROKE_PRECIS           3
#define OUT_TT_PRECIS               4
#define OUT_DEVICE_PRECIS           5
#define OUT_RASTER_PRECIS           6
#define OUT_TT_ONLY_PRECIS          7
#define OUT_OUTLINE_PRECIS          8
#define OUT_SCREEN_OUTLINE_PRECIS   9

/* ClipPrecision values */
#define CLIP_DEFAULT_PRECIS     0x00
#define CLIP_CHARACTER_PRECIS   0x01
#define CLIP_STROKE_PRECIS      0x02
#define CLIP_MASK               0x0F
#define CLIP_LH_ANGLES          0x10
#define CLIP_TT_ALWAYS          0x20
#define CLIP_EMBEDDED           0x80

/* Quality values */
#define DEFAULT_QUALITY         0
#define DRAFT_QUALITY           1
#define PROOF_QUALITY           2

/* PitchAndFamily pitch values (low 4 bits) */
#define DEFAULT_PITCH           0x00
#define FIXED_PITCH             0x01
#define VARIABLE_PITCH          0x02

/* PitchAndFamily family values (high 4 bits) */
#define FF_DONTCARE             0x00
#define FF_ROMAN                0x10
#define FF_SWISS                0x20
#define FF_MODERN               0x30
#define FF_SCRIPT               0x40
#define FF_DECORATIVE           0x50


HFONT   WINAPI CreateFont(int nHeight, int nWidth, int nEscapement, 
                          int nOrientation, int fnWeight, DWORD fdwItalic, 
                          DWORD fdwUnderline, DWORD fdwStrikeOut, 
                          DWORD fdwCharSet, DWORD fdwOutputPrecision, 
                          DWORD fdwClipPrecision, DWORD fdwQuality, 
                          DWORD fdwPitchAndFamily, PCSTR pszFace);
HFONT   WINAPI CreateFontIndirect(const LOGFONT* lplf);

#endif  /* NOGDIOBJ */

#ifndef NOTEXTMETRIC

#define CHARSET_ASCII    0
#define CHARSET_GB2312   1
#define CHARSET_GBK      2
#define CHARSET_BIG5     3
#define CHARSET_GB18030  4
#define CHARSET_1252     5

#define InRange(c, lower, upper)  (((c) >= (lower)) && ((c) <= (upper)))

#define ISGSMDBCode(p1, p2)     \
((BYTE)(p1) == 0xAA && (InRange((BYTE)(p2), 0xA1, 0xC8)))
#define IsGB2312Code(p1, p2)    \
((InRange((BYTE)(p1), 0xA1, 0xFE)) && (InRange((BYTE)(p2), 0xA1, 0xFE)))
#define IsGBKCode(p1, p2)       \
((InRange((BYTE)(p1), 0x81, 0xFE)) && (InRange((BYTE)(p2), 0x40, 0xFE)))
#define IsBIG5Code(p1, p2)      \
((InRange((BYTE)(p1), 0xA1, 0xF9)) && (InRange((BYTE)(p2), 0x40, 0xFE)))

#define IsGB18030LeadChar(p1)   (InRange((BYTE)(p1), 0x81, 0xFE))
#define IsGB18030Code2(p2)      (InRange((BYTE)(p2), 0x40, 0xFE))
#define IsGB18030Code4(p2)      (InRange((BYTE)(p2), 0x30, 0x39))

typedef struct tagTEXTMETRIC
{
    LONG        tmHeight;
    LONG        tmAscent;
    LONG        tmDescent;
    LONG        tmInternalLeading;
    LONG        tmExternalLeading;
    LONG        tmAveCharWidth;
    LONG        tmMaxCharWidth;
    LONG        tmWeight;
    LONG        tmOverhang;
    LONG        tmDigitizedAspectX;
    LONG        tmDigitizedAspectY;
    BYTE        tmFirstChar;
    BYTE        tmLastChar;
    BYTE        tmDefaultChar;
    BYTE        tmBreakChar;
    BYTE        tmItalic;
    BYTE        tmUnderlined;
    BYTE        tmStruckOut;
    BYTE        tmPitchAndFamily;
    BYTE        tmCharSet;
} TEXTMETRIC, *PTEXTMETRIC, LPPTEXTMETRIC;

/* tmPitchAndFamily values */
#define TMPF_FIXED_PITCH    0x01
#define TMPF_VECTOR         0x02
#define TMPF_DEVICE         0x08
#define TMPF_TRUETYPE       0x04

BOOL    WINAPI GetTextMetrics(HDC hdc, PTEXTMETRIC lptm);

#endif  /* NOTEXTMETRIC */

/**************************************************************************/
/*              Bitmap Functions                                          */
/**************************************************************************/

#ifndef NOBITMAP
typedef struct tagBITMAP
{
    LONG    bmType;
    LONG    bmWidth;
    LONG    bmHeight;
    LONG    bmWidthBytes;
    WORD    bmPlanes;
    WORD    bmBitsPixel;
    void*   bmBits;
} BITMAP, *PBITMAP, *LPBITMAP;

/* Bitmap Header structures */
typedef struct tagRGBTRIPLE
{
    BYTE    rgbtBlue;
    BYTE    rgbtGreen;
    BYTE    rgbtRed;
} RGBTRIPLE, *PRGBTRIPLE, *LPRGBTRIPLE;

typedef struct tagRGBQUAD
{
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
} RGBQUAD, *PRGBQUAD, *LPRGBQUAD;

/* structures for defining DIBs */
typedef struct tagBITMAPCOREHEADER
{
    DWORD   bcSize;
    short   bcWidth;
    short   bcHeight;
    WORD    bcPlanes;
    WORD    bcBitCount;
} BITMAPCOREHEADER, *PBITMAPCOREHEADER, *LPBITMAPCOREHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD   biSize;
    LONG    biWidth;
    LONG    biHeight;
    WORD    biPlanes;
    WORD    biBitCount;
    DWORD   biCompression;
    DWORD   biSizeImage;
    LONG    biXPelsPerMeter;
    LONG    biYPelsPerMeter;
    DWORD   biClrUsed;
    DWORD   biClrImportant;
} 
#ifdef __GNUC__
__attribute__((packed))
#else
#endif
BITMAPINFOHEADER, *PBITMAPINFOHEADER, *LPBITMAPINFOHEADER;

/* constants for the biCompression field */
#define BI_RGB          0L
#define BI_RLE8         1L
#define BI_RLE4         2L
#define BI_BITFIELDS    3L
#define BI_COLUMN       0x80000000L

typedef struct tagBITMAPINFO
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[1];
} BITMAPINFO, *PBITMAPINFO, *LPBITMAPINFO;

typedef struct tagBITMAPCOREINFO
{
    BITMAPCOREHEADER bmciHeader;
    RGBTRIPLE        bmciColors[1];
} BITMAPCOREINFO, *PBITMAPCOREINFO, *LPBITMAPCOREINFO;

/* For Microsoft Visual C++, turns 2 byte packing of structures on. */
#ifdef _MSC_VER /* For Microsoft Visual C++ */
#pragma pack(2)
#endif

/* The BITMAPFILEHEADER struct is just used to indicates the struct of */
/* bitmap file header, the struct can not be used to read bitmap file  */
/* header from bitmap file. Because the struct is depended on the      */
/* struct member alignment                                             */ 
typedef struct tagBITMAPFILEHEADER
{
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
}
#ifdef __GNUC__
__attribute__((packed))
#else
#endif
BITMAPFILEHEADER, *PBITMAPFILEHEADER, *LPBITMAPFILEHEADER;

/* For Microsoft Visual C++, turns packing of structures off.       */
#ifdef _MSC_VER /* For Microsoft Visual C++ */
#pragma pack()
#endif

HBITMAP WINAPI CreateBitmap(int nWidth, int nHeight, UINT cPlanes, 
                            UINT cBitsPerPel, const void* lpvBits);
HBITMAP WINAPI CreateBitmapEx(int nWidth, int nHeight, UINT cPlanes, 
                              UINT cBitsPerPel, const void* lpvBits, 
                              BOOL bUserBuf);
HBITMAP WINAPI CreateBitmapIndirect(const BITMAP* pBitmap);
HBITMAP WINAPI CreateCompatibleBitmap(HDC hdc, int nWidth, int nHeight);

HBITMAP WINAPI CreateDIBitmap(HDC hdc, BITMAPINFOHEADER* pbmih, 
                              DWORD dwInitFlag, const void* pInitData, 
                              BITMAPINFO* pbmi, UINT uUsage);

HBITMAP WINAPI LoadBitmap(HINSTANCE hInst, PCSTR pszBitmapName);
HANDLE WINAPI LoadImageEx(HDC hdc, HINSTANCE hInst, LPCSTR lpszName, UINT uType, 
                 int cxDesired, int cyDesired, UINT fuLoad);

/* DIB color table identifiers */
#define DIB_RGB_COLORS  0
#define DIB_PAL_COLORS  1

/* constants for CreateDIBitmap */
#define CBM_INIT        0x00000004L

BOOL    WINAPI PatBlt(HDC hdc, int x, int y, int width, int height, 
                      DWORD dwRop);
BOOL    WINAPI BitBlt(HDC hdcDest, int nDestX, int nDestY, int nWidth, 
                      int nHeight, HDC hdcSrc, int nSrcX, int nSrcY, 
                      DWORD dwRop);
BOOL    WINAPI StretchBlt(HDC hdcDest, int nDestX, int nDestY, int nDestW,
                          int nDestH, HDC hdcSrc, int nSrcX, int nSrcY, 
                          int nSrcW, int nSrcH, DWORD dwRop);

LONG    WINAPI SetBitmapBits(HBITMAP hBitmap, DWORD cBytes, 
                             const void* pBits);

int     WINAPI SetDIBits(HDC hdc, HBITMAP hbmp, UINT uStartScan, 
                         UINT cScanLines, CONST VOID *lpvBits, 
                         CONST BITMAPINFO *lpbmi, UINT fuColorUse);
int     WINAPI GetDIBits(HDC hdc, HBITMAP hbmp, UINT uStartScan, 
                         UINT cScanLines, PVOID lpvBits, 
                         PBITMAPINFO lpbmi, UINT fuColorUse);

int     WINAPI SetDIBitsToDevice(HDC hdc, int nDestX, int nDestY, 
                                 DWORD dwWidth, DWORD dwHeight, int nSrcX, 
                                 int nSrcY, UINT uStartScan, 
                                 UINT cScanLines, CONST VOID *lpvBits, 
                                 CONST BITMAPINFO *lpbmi, UINT fuColorUse);

int     WINAPI StretchDIBits(HDC hdc, int XDest, int YDest, int nDestWidth,
                             int nDestHeight, int XSrc, int YSrc, 
                             int nSrcWidth, int nSrcHeight, 
                             CONST VOID* lpBits, 
                             CONST BITMAPINFO *lpBitsInfo, 
                             UINT iUsage, DWORD dwRop); 

/* The parameter for blending source and destination bitmap */
typedef struct _BLENDFUNCTION
{
    BYTE   BlendOp;
    BYTE   BlendFlags;
    BYTE   SourceConstantAlpha;
    BYTE   AlphaFormat;
}BLENDFUNCTION,*PBLENDFUNCTION;

/* currentlly defined blend function */
#define AC_SRC_OVER                 0x00

/* alpha format flags */
#define AC_SRC_NO_PREMULT_ALPHA     0x01
#define AC_SRC_NO_ALPHA             0x02
#define AC_SRC_ALPHA                0x04
#define AC_DST_NO_PREMULT_ALPHA     0x10
#define AC_DST_NO_ALPHA             0x20

BOOL    WINAPI AlphaBlend(HDC hdcDest, int nXOriginDest, int nYOriginDest, 
                int nWidthDest, int nHeightDest, HDC hdcSrc, int nXOriginSrc, 
                int nYOriginSrc, int nWidthSrc, int nHeightSrc, 
                BLENDFUNCTION blendFunction);
BOOL    WINAPI AlphaDIBBlend(HDC hdcDest, int nXOriginDest, int nYOriginDest, 
                int nWidthDest, int nHeightDest, const void* pInitData, 
                const BITMAPINFO* pbmi, UINT uUsage, int nXOriginSrc, 
                int nYOriginSrc, int nWidthSrc, int nHeightSrc, 
                BLENDFUNCTION blendFunction);
BOOL    WINAPI TransparentBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, 
                int nWidthDest, int nHeightDest, HDC hdcSrc, int nXOriginSrc, 
                int nYOriginSrc, int nWidthSrc, int nHeightSrc, 
                UINT crTransparent);
BOOL    WINAPI TransparentDIBits(HDC hdcDest, int nXOriginDest, int nYOriginDest, 
                int nWidthDest, int nHeightDest, const void* pInitData, 
                const BITMAPINFO* pbmi, UINT uUsage, int nXOriginSrc, 
                int nYOriginSrc, int nWidthSrc, int nHeightSrc, 
                UINT crTransparent);

/* Two functions drawing bitmap directly */
BOOL    WINAPI DrawBitmapFromData(HDC hdc, int x, int y, int nWidth, 
                                  int nHeight, UINT cPlanes, 
                                  UINT cBitsPerPel, const void* lpvBits, 
                                  DWORD dwRop);
BOOL    WINAPI DrawBitmapFromFile(HDC hdc, int x, int y, LPCSTR pszBmpFile,
                                  DWORD dwRop);

#endif  /* NOBITMAP */

#endif  /* NOGDI */

/**************************************************************************/
/*              USER typedefs, structures, and functions                  */
/**************************************************************************/

DECLARE_HANDLE(HWND);

#ifndef NOUSER

DECLARE_HANDLE(HMENU);

DECLARE_HANDLE(HICON);
typedef HICON HCURSOR;      /* HICONs & HCURSORs are polymorphic */

/**************************************************************************/
/*              System Metrics                                            */
/**************************************************************************/

#if (!NOSYSMETRICS)

int     WINAPI GetSystemMetrics(int nIndex);

/* GetSystemMetrics() codes */
#define SM_CXSCREEN          0
#define SM_CYSCREEN          1
#define SM_CXVSCROLL         2
#define SM_CYHSCROLL         3
#define SM_CYCAPTION         4
#define SM_CXBORDER          5
#define SM_CYBORDER          6
#define SM_CXDLGFRAME        7
#define SM_CYDLGFRAME        8
#define SM_CYVTHUMB          9
#define SM_CXHTHUMB          10
#define SM_CXICON            11
#define SM_CYICON            12
#define SM_CXCURSOR          13
#define SM_CYCURSOR          14
#define SM_CYMENU            15
#define SM_CXFULLSCREEN      16
#define SM_CYFULLSCREEN      17
#define SM_CYVSCROLL         18
#define SM_CXHSCROLL         19
#define SM_CXMIN             20
#define SM_CYMIN             21
#define SM_CXSIZE            22
#define SM_CYSIZE            23
#define SM_CXFRAME           24
#define SM_CYFRAME           25
#define SM_CXMINTRACK        26
#define SM_CYMINTRACK        27
#define SM_CYSTATICBAR       28

#endif  /* NOSYSMETRICS */

/**************************************************************************/
/*              Rectangle Support                                         */
/**************************************************************************/

BOOL    WINAPI SetRect(PRECT pRect, int x1, int y1, int x2, int y2);
BOOL    WINAPI SetRectEmpty(PRECT pRect);

BOOL    WINAPI CopyRect(PRECT prcDst, const RECT* prcSrc);

BOOL    WINAPI IsRectEmpty(const RECT* pRect);

BOOL    WINAPI EqualRect(const RECT* pRect1, const RECT* pRect2);

BOOL    WINAPI IntersectRect(PRECT prcDst, const RECT* prcSrc1, 
                             const RECT* prcSrc2);
BOOL    WINAPI UnionRect(PRECT prcDst, const RECT* prcSrc1, 
                         const RECT* prcSrc2);
BOOL    WINAPI SubtractRect(PRECT prcDst, const RECT* prcSrc1, 
                            const RECT* prcSrc2);

BOOL    WINAPI OffsetRect(PRECT pRect, int nOffsetX, int nOffsetY);
BOOL    WINAPI InflateRect(PRECT pRect, int nOffsetX, int nOffsetY);

BOOL    WINAPI PtInRect(const RECT* pRect, POINT pt);
BOOL    WINAPI PtInRectXY(const RECT* pRect, int x, int y);

BOOL    WINAPI NormalizeRect(PRECT pRect);

/**************************************************************************/
/*              Window message support                                    */
/**************************************************************************/

#define WM_NULL             0x0000

/* NOTE: All messages below 0x0400 are RESERVED */
#define WM_USER             0x0400

/* Queued message structure */
typedef struct tagMSG
{
    HWND        hwnd;       /* Handle for destination window    */
    UINT        message;    /* Message command                  */
    WPARAM      wParam;     /* First parameter of message       */
    LPARAM      lParam;     /* Second parameter of message      */
} MSG, *PMSG, *LPMSG;

/* Message function prototypes */
BOOL    WINAPI GetMessage(PMSG pMsg, HWND hWnd, UINT wMsgFilterMin, 
                          UINT wMsgFilterMax); 
BOOL    WINAPI PostMessage(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                           LPARAM lParam);
BOOL WINAPI PostThreadMessage(DWORD dwThreadId, UINT uMsgCmd, WPARAM wParam, 
                              LPARAM lParam);
LRESULT WINAPI SendMessage(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                           LPARAM lParam);
BOOL    WINAPI SendNotifyMessage(HWND hWnd, UINT uMsgCmd, WPARAM wParam, 
                           LPARAM lParam);
void WINAPI PostQuitMessage(int nExitCode);

BOOL    WINAPI TranslateMessage(const MSG* pMsg);
LONG    WINAPI DispatchMessage(const MSG* pMsg);

/*
 * PeekMessage() Options
 */
#define PM_NOREMOVE         0x0000
#define PM_REMOVE           0x0001
#define PM_NOYIELD          0x0002
BOOL    WINAPI PeekMessage(PMSG pMsg, HWND hWnd, UINT wMsgFilterMin, 
                       UINT wMsgFilterMax, UINT wRemoveMsg);

/* Special HWND value for use with PostMessage() and SendMessage() */
#define HWND_BROADCAST  ((HWND)0xFFFF)

void    WINAPI AppRun(void);

/**************************************************************************/
/*              Window class management                                   */
/**************************************************************************/
#define DEFWINDOWCLASSNAME "DefaultWindowClass"
#define PDADEFWINDOWCLASSNAME "PDADefWindowClass"

/* Prototype for window proc function */
typedef LRESULT (CALLBACK *WNDPROC)(HWND, UINT, WPARAM, LPARAM);

/* Define struct for window class */
typedef struct tagWNDCLASS
{
    UINT        style;
    WNDPROC     lpfnWndProc;
    int         cbClsExtra;
    int         cbWndExtra;
    HINSTANCE   hInstance;
    HICON       hIcon;
    HCURSOR     hCursor;
    HBRUSH      hbrBackground;
    LPCSTR      lpszMenuName;
    LPCSTR      lpszClassName;
} WNDCLASS, *PWNDCLASS, *LPWNDCLASS;

typedef struct tagWNDCLASSEX
{
    UINT        cbSize;
    UINT        style;
    WNDPROC     lpfnWndProc;
    int         cbClsExtra;
    int         cbWndExtra;
    HINSTANCE   hInstance;
    HICON       hIcon;
    HCURSOR     hCursor;
    HBRUSH      hbrBackground;
    LPCSTR      lpszMenuName;
    LPCSTR      lpszClassName;
    HICON       hIconSm;
} WNDCLASSEX, *PWNDCLASSEX, *LPWNDCLASSEXA;

BOOL    WINAPI RegisterClassBase(const WNDCLASS* pWndClass);
BOOL    WINAPI RegisterClassExBase(CONST WNDCLASSEX* lpwcx);

#if (!BASEWINDOW)
BOOL    WINAPI RegisterClass(const WNDCLASS* pWndClass);
BOOL    WINAPI RegisterClassEx(CONST WNDCLASSEX* lpwcx);
#else
#define RegisterClass RegisterClassBase
#define RegisterClassEx RegisterClassExBase
#endif

BOOL    WINAPI UnregisterClass(PCSTR pszClassName, HINSTANCE hInst);


BOOL    WINAPI GetClassInfo(HINSTANCE hInst, PCSTR pszClassName, 
                            PWNDCLASS pWndClass);
int     WINAPI GetClassName(HWND hWnd, PSTR pszClassName, int nMaxCount);

#ifndef NOWINSTYLES

/* Class styles */
#define CS_VREDRAW          0x0001
#define CS_HREDRAW          0x0002

#define CS_OWNDC            0x0010
#define CS_CLASSDC          0x0020
#define CS_PARENTDC         0x0040

#define CS_SAVEBITS         0x0800

#define CS_DBLCLKS          0x0008

#define CS_NOCLOSE          0x0200
#define CS_NOFOCUS          0x0400

#define CS_BYTEALIGNCLIENT  0x1000
#define CS_BYTEALIGNWINDOW  0x2000
#define CS_GLOBALCLASS      0x4000

#endif  /* NOWINSTYLES */

/* Class field offsets for GetClassLong() & SetClassLong */
#define GCL_MENUNAME        (-1)
#define GCL_HBRBACKGROUND   (-2)
#define GCL_HCURSOR         (-3)
#define GCL_HICON           (-4)
#define GCL_HMODULE         (-5)
#define GCL_CBWNDEXTRA      (-6)
#define GCL_CBCLSEXTRA      (-7)
#define GCL_WNDPROC         (-8)
#define GCL_STYLE           (-9)

WORD    WINAPI GetClassWord(HWND hWnd, int nIndex);
WORD    WINAPI SetClassWord(HWND hWnd, int nIndex, int wNewWord);
DWORD   WINAPI GetClassLong(HWND hWnd, int nIndex);
DWORD   WINAPI SetClassLong(HWND hWnd, int nIndex, DWORD dwNewLong);

/**************************************************************************/
/*              Window creation and destroy                               */
/**************************************************************************/

/* Window Styles */
#ifndef NOWINSTYLES

/* Basic window types */
#define WS_POPUP            0x00000000L
#define WS_CHILD            0x80000000L

/* Child window styles */
#define WS_NCCHILD          0x40000000L

/* Popup window styles */
#define WS_TOPMOST          0x40000000L

/* Clipping styles */
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L

/* Generic window states */
#define WS_VISIBLE          0x10000000L
#define WS_DISABLED         0x08000000L

/* Main window states */
#define WS_MINIMIZE         0x20000000L
#define WS_MAXIMIZE         0x01000000L

#define WS_BITMAP           0x01000000L
/* Main window styles */
#define WS_DLGFRAME         0x00C00000L     /* WS_BORDER | WS_DLGFRAME  */
#define WS_BORDER           0x00800000L
#define WS_THICKFRAME       0x00400000L
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
#define WS_SYSMENU          0x00080000L
#define WS_CAPTION          0x00040000L
#define WS_MINIMIZEBOX      0x00020000L
#define WS_MAXIMIZEBOX      0x00010000L

/* Control window styles */
#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L

/* Common Window Styles */
#define WS_OVERLAPPED      (WS_POPUP | WS_CAPTION | WS_BORDER)
#define WS_OVERLAPPEDWINDOW (WS_POPUP | WS_CAPTION | WS_SYSMENU | \
    WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_POPUPWINDOW      (WS_POPUP | WS_BORDER | WS_SYSMENU)
#define WS_CHILDWINDOW      (WS_CHILD)

#endif  /* NOWINSTYLES */

#define HWND_DESKTOP    ((HWND)0x00800000)

BOOL    WINAPI IsWindow(HWND hWnd);

HWND    WINAPI CreateWindow(PCSTR pszClassName, PCSTR pszWindowName, 
                            DWORD dwStyle, int x, int y, int nWidth, 
                            int nHeight, HWND hwndParent, HMENU hMenu, 
                            HINSTANCE hInstance, PVOID pParam);

HWND    WINAPI CreateWindowEx(DWORD dwExStyle, PCSTR pszClassName, 
                              PCSTR pszWindowName, DWORD dwStyle, 
                              int x, int y, int nWidth, int nHeight, 
                              HWND hwndParent, HMENU hMenu, 
                              HINSTANCE hInstance, PVOID pParam);

#define WM_CREATE           0x0001
#define WM_NCCREATE         0x0081

/* WM_CREATE/WM_NCCREATE lParam struct */
typedef struct tagCREATESTRUCT
{
    void*     lpCreateParams;
    HINSTANCE hInstance;
    HMENU     hMenu;
    HWND      hwndParent;
    int       cy;
    int       cx;
    int       y;
    int       x;
    LONG      style;
    LPCSTR    lpszName;
    LPCSTR    lpszClass;
    DWORD     dwExStyle;
} CREATESTRUCT, *PCREATESTRUCT, *LPCREATESTRUCT;

BOOL WINAPI DestroyWindow(HWND hWnd);

#define WM_DESTROY          0x0002
#define WM_NCDESTROY        0x0082

/**************************************************************************/
/*              End session & To DeskTop                                  */
/**************************************************************************/
#define WM_QUERYENDSESSION      0x039A
#define WM_ENDSESSION           0x039B
#define WM_PMKEYDOWN            0x039C
#define WM_TODESKTOP            0x039D

#define ENDSESSION_AUTO         0x10000000		//	auto
#define ENDSESSION_POWOFF       0x20000000		//	power off
#define ENDSESSION_LOGOFF       0x80000000		//	log off


/**************************************************************************/
/*              Basic window attributes                                   */
/**************************************************************************/

BOOL    WINAPI IsChild(HWND hwndParent, HWND hwnd);

HWND    WINAPI GetParent(HWND hWnd);

void*   WINAPI GetWindowExtraDataPtrBase(HWND hWnd);
#define GetUserDataBase GetWindowExtraDataPtrBase

#if (!BASEWINDOW)
void*   WINAPI GetWindowExtraDataPtr(HWND hWnd);
//#define GetUserData GetWindowExtraDataPtr
#else
#define GetWindowExtraDataPtr GetWindowExtraDataPtrBase
//#define GetUserData GetWindowExtraDataPtr
#endif

void*   WINAPI GetUserData(HWND hWnd);

HWND WINAPI GetDesktopWindow(void);

BOOL    WINAPI IsWindowVisible(HWND hWnd);

BOOL    WINAPI ShowWindow(HWND hWnd, int nCmdShow);

#ifndef NOSHOWWINDOW

#define SW_HIDE             0
#define SW_SHOWNORMAL       1
#define SW_NORMAL           1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_MAXIMIZE         3
#define SW_SHOWNOACTIVATE   4
#define SW_SHOW             5
#define SW_MINIMIZE         6
#define SW_SHOWMINNOACTIVE  7
#define SW_SHOWNA           8
#define SW_RESTORE          9

#endif  /* NOSHOWWINDOW */

#define WM_SHOWWINDOW       0x0018

#define WM_SETREDRAW        0x000B


/* Enabled state */
BOOL    WINAPI EnableWindow(HWND hWnd, BOOL bEnable);
BOOL    WINAPI IsWindowEnabled(HWND hWnd);

#define WM_ENABLE           0x000A

/* Window text */
BOOL    WINAPI SetWindowText(HWND hWnd, PCSTR pszText);
int     WINAPI GetWindowText(HWND hWnd, PSTR pszText, int nMaxCount);
int     WINAPI GetWindowTextLength(HWND hWnd);

#define WM_SETTEXT          0x000C
#define WM_GETTEXT          0x000D
#define WM_GETTEXTLENGTH    0x000E

#define WM_GETUSERDATA      0x011A

/* Window words */
WORD    WINAPI GetWindowWord(HWND hWnd, int nIndex);
WORD    WINAPI SetWindowWord(HWND hWnd, int nIndex, int wNewValue);

/* Window longs */
LONG    WINAPI GetWindowLong(HWND hWnd, int nIndex);
LONG    WINAPI SetWindowLong(HWND hWnd, int nIndex, LONG lNewValue);

#define GWL_WNDPROC         (-1)
#define GWL_HINSTANCE       (-2)
#define GWL_HWNDPARENT      (-3)
#define GWL_STYLE           (-4)
#define GWL_EXSTYLE         (-5)
#define GWL_USERDATA        (-6)
#define GWL_ID              (-7)
#define GWL_USERDATAADDR    (-8)

/**************************************************************************/
/*              Window size, position, Z-order, and visibility            */
/**************************************************************************/

#define CW_USEDEFAULT       ((int)0x80000000)

BOOL WINAPI GetClientRect(HWND hWnd, PRECT pRect);
BOOL WINAPI GetWindowRect(HWND hWnd, PRECT pRect);

/* Define constant for window part */
#define W_CLIENT        0
#define W_NCLIENT       1
#define W_TOPNC         2
#define W_BOTTOMNC      3
#define W_LEFTNC        4
#define W_RIGHTNC       5
#define W_WINDOW        6

/* Define constant for XY mode  */
#define XY_DEFAULT      0
#define XY_SCREEN       1
#define XY_WINDOW       2
#define XY_NCLIENT      3
#define XY_CLIENT       4

BOOL WINAPI GetWindowRectEx(HWND hWnd, PRECT pRect, int nPart, int nOrg);

BOOL WINAPI MoveWindow(HWND hWnd, int x, int y, int nWidth, int nHeight, 
                       BOOL bRepaint);
BOOL WINAPI MoveWindowToTop(HWND hWnd);

/* Rectangle output routines */
BOOL WINAPI StartDrag(HWND hWnd, HBRUSH hBrush, int width);
void WINAPI EndDrag(HWND hWnd);
void WINAPI DrawDragFrame(HDC hdc, const RECT* pRect);

#ifdef MSWIN_COMPATIBLE
#define BringWindowToTop    MoveWindowToTop
#endif /* MSWIN_COMPATIBLE */

#define WM_MOVE             0x0003
#define WM_SIZE             0x0005

/**************************************************************************/
/*              Window proc implementation                                */
/**************************************************************************/
LRESULT WINAPI DefWindowProcBase(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                 LPARAM lParam);

#if (!BASEWINDOW)
LRESULT WINAPI DefWindowProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                 LPARAM lParam);
#else
#define DefWindowProc DefWindowProcBase
#endif

LRESULT WINAPI CallWindowProc(WNDPROC lpPrevWndFunc, HWND hWnd, 
                              UINT uMsgCmd, WPARAM wParam, LPARAM lParam);

/**************************************************************************/
/*              Main window support                                       */
/**************************************************************************/
#define WM_QUIT             0x0012
#define WM_CLOSE            0x0010

/* Struct pointed to by WM_GETMINMAXINFO lParam */
typedef struct tagMINMAXINFO
{
    POINT ptReserved;
    POINT ptMaxSize;
    POINT ptMaxPosition;
    POINT ptMinTrackSize;
    POINT ptMaxTrackSize;
} MINMAXINFO, *PMINMAXINFO, *LPMINMAXINFO;

#define WM_GETMINMAXINFO    0x0024

BOOL    WINAPI ShowOwnedPopups(HWND hWnd, BOOL bShow);

/**************************************************************************/
/*              Window coordinate mapping and hit-testing                 */
/**************************************************************************/

BOOL    WINAPI ClientToScreen(HWND hWnd, PPOINT pPoint);
BOOL    WINAPI ScreenToClient(HWND hWnd, PPOINT pPoint);

HWND    WINAPI WindowFromPoint(POINT pt);
HWND    WINAPI ChildWindowFromPoint(HWND hWnd, POINT pt);

/**************************************************************************/
/*              Window query and enumeration                              */
/**************************************************************************/

HWND    WINAPI FindWindow(PCSTR pszClassName, PCSTR pszWindowName);

typedef BOOL (CALLBACK* WNDENUMPROC)(HWND, LPARAM);

BOOL    WINAPI EnumWindows(WNDENUMPROC pEnumFunc, LPARAM lParam);
BOOL    WINAPI EnumChildWindows(HWND hwndParent, WNDENUMPROC pEnumFunc, 
                                LPARAM lParam);

#define GetNextWindow(hWnd, wCmd) GetWindow(hWnd, wCmd)

/* GetWindow() Constants */
#define GW_HWNDFIRST        0
#define GW_HWNDLAST         1
#define GW_HWNDNEXT         2
#define GW_HWNDPREV         3
#define GW_OWNER            4
#define GW_CHILD            5
#define GW_LASTCHILD        6
#define GW_MAX              6

HWND    WINAPI GetWindow(HWND hWnd, UINT uCmd);

/**************************************************************************/
/*              Window drawing support                                    */
/**************************************************************************/

HDC  WINAPI GetDC(HWND hWnd);
int  WINAPI ReleaseDC(HWND hWnd, HDC hdc);

HDC  WINAPI GetWindowDC(HWND hWnd);

/**************************************************************************/
/*              Window repainting                                         */
/**************************************************************************/

#define WM_PAINT            0x000F
#define WM_ERASEBKGND       0x0014

typedef struct tagPAINTSTRUCT
{
    HDC     hdc;     /* Specifies DC for painting                        */
    BOOL    fErase;  /* Specifies whether the background must be erased. */
    RECT    rcPaint; /* Specifies a painting rectangle                   */
} PAINTSTRUCT, *PPAINTSTRUCT, *LPPAINTSTRUCT;

BOOL    WINAPI GetUpdateRect(HWND hWnd, PRECT pRect, BOOL bErase);
HDC     WINAPI BeginPaint(HWND hWnd, PPAINTSTRUCT pPaintStruct);
BOOL    WINAPI EndPaint(HWND hWnd, const PAINTSTRUCT* pPaintStruct);

BOOL    WINAPI UpdateWindow(HWND hWnd);

BOOL    WINAPI InvalidateRect(HWND hWnd, const RECT* pRect, BOOL bErase);
BOOL    WINAPI ValidateRect(HWND hWnd, const RECT* pRect);

/**************************************************************************/
/*              Window scrolling                                          */
/**************************************************************************/

BOOL    WINAPI ScrollWindow(HWND hWnd, int dx, int dy, 
                            const RECT* prcScroll, const RECT* prcClip);

/**************************************************************************/
/*              Non-client window area management                         */
/**************************************************************************/

#define WM_NCPAINT          0x0085

#define WM_NCCALCSIZE       0x0083

/* WM_NCCALCSIZE return flags */
#define WVR_ALIGNTOP        0x0010
#define WVR_ALIGNLEFT       0x0020
#define WVR_ALIGNBOTTOM     0x0040
#define WVR_ALIGNRIGHT      0x0080
#define WVR_HREDRAW         0x0100
#define WVR_VREDRAW         0x0200
#define WVR_REDRAW          (WVR_HREDRAW | WVR_VREDRAW)
#define WVR_VALIDRECTS      0x0400

/* WM_WINDOWPOSCHANGING/CHANGED struct pointed to by lParam */
typedef struct tagWINDOWPOS 
{
    HWND    hwnd;
    HWND    hwndInsertAfter;
    int     x;
    int     y;
    int     cx;
    int     cy;
    UINT    flags;
} WINDOWPOS, *LPWINDOWPOS, *PWINDOWPOS;

/* WM_NCCALCSIZE parameter structure */
typedef struct tagNCCALCSIZE_PARAMS 
{
    RECT       rgrc[3];
    PWINDOWPOS lppos;
} NCCALCSIZE_PARAMS, *LPNCCALCSIZE_PARAMS, *PNCCALCSIZE_PARAMS;

#define WM_NCHITTEST        0x0084

/* WM_NCHITTEST return codes */
#define HTERROR             (-2)
#define HTTRANSPARENT       (-1)
#define HTNOWHERE           0
#define HTCLIENT            1
#define HTCAPTION           2
#define HTSYSMENU           3
#define HTSIZE              4
#define HTMENU              5
#define HTHSCROLL           6
#define HTVSCROLL           7
#define HTMINBUTTON         8
#define HTMAXBUTTON         9
#define HTLEFT              10
#define HTRIGHT             11
#define HTTOP               12
#define HTTOPLEFT           13
#define HTTOPRIGHT          14
#define HTBOTTOM            15
#define HTBOTTOMLEFT        16
#define HTBOTTOMRIGHT       17
#define HTBORDER            18
#define HTCLOSE             19

#define HTGROWBOX           HTSIZE
#define HTREDUCE            HTMINBUTTON
#define HTZOOM              HTMAXBUTTON
#define HTCLOSEBOX          HTCLOSE

/**************************************************************************/
/*              Window activation                                         */
/**************************************************************************/
DWORD   WINAPI GetWindowThreadProcessId(HWND hWnd, LPDWORD lpdwProcessid);

BOOL    WINAPI SetForegroundWindow(HWND hWnd);
HWND    WINAPI GetForegroundWindow(void);

HWND    WINAPI SetActiveWindow(HWND hWnd);
HWND    WINAPI GetActiveWindow(void);

/* WM_ACTIVATE state values */
#define WA_INACTIVE         0
#define WA_ACTIVE           1
#define WA_CLICKACTIVE      2

#define WM_ACTIVATE         0x0006
#define WM_ACTIVATEAPP      0x001C
#define WM_NCACTIVATE       0x0086

/**************************************************************************/
/*              Keyboard input support                                    */
/**************************************************************************/

/* Virtual key codes */
#ifndef NOVIRTUALKEYCODES
#include "virtkey.h"
#endif  /* NOVIRTUALKEYCODES */

HWND    WINAPI GetFocus(void);
HWND    WINAPI SetFocus(HWND hWnd);

HWND    WINAPI GrabKeyboard(HWND hWnd);
BOOL    WINAPI UngrabKeyboard(void);

BOOL    WINAPI SendChar(HWND hWnd, int wCode);
BOOL    WINAPI SendKeyDown(HWND hWnd, int wVirtKey, int wKeyState);
BOOL    WINAPI SendKeyUp(HWND hWnd, int wVirtKey, int wKeyState);

BOOL    WINAPI GetKeyboardState(PBYTE lpKeyState);
int     WINAPI ToAscii(UINT uVirtKey, UINT uScanCode, PBYTE lpKeyState,
                       LPWORD lpChar, UINT uFlags);

#define WM_SETFOCUS         0x0007
#define WM_KILLFOCUS        0x0008

#define WM_KEYFIRST         0x0100
#define WM_KEYDOWN          0x0100
#define WM_KEYUP            0x0101
#define WM_CHAR             0x0102
#define WM_DEADCHAR         0x0103
#define WM_SYSKEYDOWN       0x0104
#define WM_SYSKEYUP         0x0105
#define WM_SYSCHAR          0x0106
#define WM_SYSDEADCHAR      0x0107
#define WM_KEYLAST          0x0108

#define KEYEVENTF_EXTENDEDKEY 0x00000001
#define KEYEVENTF_KEYUP       0x00000002
#define KEYEVENTF_FROMDEVICE  0x80000000

void    WINAPI keybd_event(int bVk, int bScan, DWORD dwFlags, 
                           DWORD dwExtraInfo);

#define LONGKEY_SEQUENCE     0x00000001L
#define LONGKEY_EATSHORTUP   0x00000002L
#define LONGKEY_EATSHORTDOWN 0x00000004L
#define LONGKEY_EATSHORT     (LONGKEY_EATSHORTDOWN | LONGKEY_EATSHORTUP)   
#define LONGKEY_EATLONGUP    0x00000008L

BOOL    WINAPI RegisterLongKey(WORD wScancode, WORD wVkShort, WORD wVkLong, 
                        DWORD dwEscaped, DWORD dwFlags);
BOOL    WINAPI UnregisterLongKey(WORD wScancode, WORD wVkShort, WORD wVkLong, 
                          DWORD dwEscaped);

/* Define control key mask  */
#define CK_SHIFT            (1L << 25)
#define CK_CONTROL          (1L << 26)
#define CK_ALT              (1L << 27)
#define CK_CAPS             (1L << 28)

/**************************************************************************/
/*              Point device input support                                */
/**************************************************************************/

// Pen Calibration Functions

BOOL    WINAPI BeginPenCalibrate(HWND hWnd);
void    WINAPI EndPenCalibrate(void);

#define WM_BEGINPENCALIBRATE    0x0320
#define WM_ENDPENCALIBRATE      0x0321

void    WINAPI SetPointDevScale(int nLogX1, int nLogY1, int nPhyX1, 
                                int nPhyY1, int nLogX2, int nLogY2, 
                                int nPhyX2, int nPhyY2);

HWND    WINAPI SetCapture(HWND hWnd);
BOOL    WINAPI ReleaseCapture(void);
HWND    WINAPI GetCapture(void);

#define GrabPen     SetCapture
#define UngrabPen   ReleaseCapture

/* Mouse input messages */
#define WM_MOUSEMOVE        0x0200
#define WM_LBUTTONDOWN      0x0201
#define WM_LBUTTONUP        0x0202
#define WM_LBUTTONDBLCLK    0x0203
#define WM_RBUTTONDOWN      0x0204
#define WM_RBUTTONUP        0x0205
#define WM_RBUTTONDBLCLK    0x0206
#define WM_MBUTTONDOWN      0x0207
#define WM_MBUTTONUP        0x0208
#define WM_MBUTTONDBLCLK    0x0209

/* Mouse input message range */
#define WM_MOUSEFIRST       0x0200
#define WM_MOUSELAST        0x0209

/* Pen input messages */
#define WM_PENMOVE          WM_MOUSEMOVE
#define WM_PENDOWN          WM_LBUTTONDOWN
#define WM_PENUP            WM_LBUTTONUP

/* Mouse message wParam key states */
#ifndef NOKEYSTATES
#define MK_LBUTTON          0x0001
#define MK_RBUTTON          0x0002
#define MK_SHIFT            0x0004
#define MK_CONTROL          0x0008
#define MK_MBUTTON          0x0010
#endif  /* NOKEYSTATES */

/* Non-client mouse messages */
#define WM_NCMOUSEMOVE      0x00A0
#define WM_NCLBUTTONDOWN    0x00A1
#define WM_NCLBUTTONUP      0x00A2
#define WM_NCLBUTTONDBLCLK  0x00A3
#define WM_NCRBUTTONDOWN    0x00A4
#define WM_NCRBUTTONUP      0x00A5
#define WM_NCRBUTTONDBLCLK  0x00A6
#define WM_NCMBUTTONDOWN    0x00A7
#define WM_NCMBUTTONUP      0x00A8
#define WM_NCMBUTTONDBLCLK  0x00A9

/* Non-client pen messages */
#define WM_NCPENMOVE        WM_NCMOUSEMOVE
#define WM_NCPENDOWN        WM_NCLBUTTONDOWN
#define WM_NCPENUP          WM_NCLBUTTONUP

/* Mouse click activation support */
#define WM_MOUSEACTIVATE    0x0021

/* WM_MOUSEACTIVATE return codes */
#define MA_ACTIVATE         1
#define MA_ACTIVATEANDEAT   2
#define MA_NOACTIVATE       3
#define MA_NOACTIVATEANDEAT 4

/* Mouse event flag */
#define MOUSEEVENTF_MOVE        0x00000001  /* mouse move           */
#define MOUSEEVENTF_LEFTDOWN    0x00000002  /* left button down     */
#define MOUSEEVENTF_LEFTUP      0x00000004  /* left button up       */
#define MOUSEEVENTF_RIGHTDOWN   0x00000008  /* right button down    */
#define MOUSEEVENTF_RIGHTUP     0x00000010  /* right button up      */
#define MOUSEEVENTF_MIDDLEDOWN  0x00000020  /* middle button down   */
#define MOUSEEVENTF_MIDDLEUP    0x00000040  /* middle button up     */
#define MOUSEEVENTF_WHEEL       0x00000800  /* wheel button rolled  */
#define MOUSEEVENTF_ABSOLUTE    0x00008000  /* absolute move        */
#define MOUSEEVENTF_FROMDEVICE  0x80000000  /* from device          */

#define MOUSEEVENTF_NOMERGEMOVE 0x10000000  /* from device          */

void    WINAPI mouse_event(DWORD dwFlags, LONG dx, LONG dy, DWORD dwData,
                           DWORD dwExtraInfo);

void    WINAPI FlushInputMessages(void);

/**************************************************************************/
/*              Mode control                                              */
/**************************************************************************/

#define WM_TIMECHANGE       0x001E
#define WM_CANCELMODE       0x001F

/**************************************************************************/
/*              System modal window support                               */
/**************************************************************************/

HWND    WINAPI GetSysModalWindow(void);
HWND    WINAPI SetSysModalWindow(HWND hWnd);

/**************************************************************************/
/*              Timer Support                                             */
/**************************************************************************/

typedef void (CALLBACK* TIMERPROC)(HWND, UINT, UINT, DWORD);

UINT    WINAPI SetTimer(HWND hWnd, UINT nIDEvent, DWORD uElapse, 
                        TIMERPROC pTimerFunc);

BOOL    WINAPI KillTimer(HWND hWnd, UINT uIDEvent);

#define WM_TIMER            0x0113

/**************************************************************************/
/*              Menu Functions                                            */
/**************************************************************************/

#if(!NOMENUS)

typedef struct tagMENUITEMTEMPLATE MENUITEMTEMPLATE;
typedef MENUITEMTEMPLATE *PMENUITEMTEMPLATE;

/* Menu item template */
struct tagMENUITEMTEMPLATE
{
    WORD    wFlags;               // menu item flag
    WORD    wID;                  // menu item id(omitted for pop-up items)
    PCSTR   lpszItemName;         // menu item name
    PMENUITEMTEMPLATE pMenuItems; // pointer to submenu's items
};

typedef MENUITEMTEMPLATE *LPMENUITEMTEMPLATE;
typedef CONST MENUITEMTEMPLATE *PCMENUITEMTEMPLATE;
typedef CONST MENUITEMTEMPLATE *LPCMENUITEMTEMPLATE;

/* Menu template */
typedef struct 
{
    DWORD               version;
    PMENUITEMTEMPLATE   pMenuItems;
} MENUTEMPLATE;

typedef MENUTEMPLATE* PMENUTEMPLATE;
typedef MENUTEMPLATE* LPMENUTEMPLATE;
typedef CONST MENUTEMPLATE* PCMENUTEMPLATE;
typedef CONST MENUTEMPLATE* LPCMENUTEMPLATE;

BOOL    WINAPI IsMenu(HMENU hMenu);

HMENU   WINAPI CreateMenu(void);
HMENU   WINAPI CreatePopupMenu(void);
HMENU   WINAPI LoadMenu(HINSTANCE hInstance, PCSTR pszMenuName);
HMENU   WINAPI LoadMenuIndirect(const MENUTEMPLATE* pMenuTemplate);

BOOL    WINAPI DestroyMenu(HMENU hMenu);

#define MF_INSERT           0x0000
#define MF_CHANGE           0x0080
#define MF_APPEND           0x0100
#define MF_DELETE           0x0200
#define MF_REMOVE           0x1000

/* Menu flags for Add/Check/EnableMenuItem() */
#define MF_BYCOMMAND        0x0000
#define MF_BYPOSITION       0x0400

#define MF_SEPARATOR        0x0800

#define MF_ENABLED          0x0000
#define MF_GRAYED           0x0001
#define MF_DISABLED         0x0002

#define MF_UNCHECKED        0x0000
#define MF_CHECKED          0x0008
#define MF_USECHECKBITMAPS  0x0200

#define MF_STRING           0x0000
#define MF_BITMAP           0x0004
#define MF_OWNERDRAW        0x0100

#define MF_POPUP            0x0010
#define MF_MENUBARBREAK     0x0020
#define MF_MENUBREAK        0x0040

#define MF_UNHILITE         0x0000
#define MF_HILITE           0x0080

#define MF_SYSMENU          0x2000
#define MF_HELP             0x4000
#define MF_MOUSESELECT      0x8000

#define MF_NOTUSERSTRING    0x1000

/* Only valid in menu resource templates */
#define MF_END              0x0080

#ifndef NOMENUAPI

HMENU   WINAPI GetMenu(HWND hWnd);
BOOL    WINAPI SetMenu(HWND hWnd, HMENU hMenu);

BOOL    WINAPI DrawMenuBar(HWND hWnd);

BOOL    WINAPI HiliteMenuItem(HWND hWnd, HMENU hMenu, UINT uItemHilite,
                              UINT uHilite);

BOOL    WINAPI InsertMenu(HMENU hMenu, UINT uPosition, UINT uFlags, 
                          UINT_PTR uIDNewItem, LPCSTR lpNewItem);
BOOL    WINAPI AppendMenu(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, 
                          LPCSTR lpNewItem);
BOOL    WINAPI ModifyMenu(HMENU hMenu, UINT uPosition, UINT uFlags, 
                          UINT_PTR uIDNewItem, LPCSTR lpNewItem);
BOOL    WINAPI RemoveMenu(HMENU hMenu, UINT uPosition, UINT uFlags);
BOOL    WINAPI DeleteMenu(HMENU hMenu, UINT uPosition, UINT uFlags);
BOOL    WINAPI ChangeMenu(HMENU hMenu, UINT cmd, LPCSTR lpszNewItem, 
                          UINT cmdInsert, UINT flags);

BOOL    WINAPI EnableMenuItem(HMENU hMenu, UINT uIDEnableItem, 
                              UINT uEnable);
BOOL    WINAPI CheckMenuItem(HMENU hMenu, UINT uIDCheckItem, UINT uCheck);

HMENU   WINAPI GetSubMenu(HMENU hMenu, int nPos);

int     WINAPI GetMenuItemCount(HMENU hMenu);
UINT    WINAPI GetMenuItemID(HMENU hMenu, int nPos);

int     WINAPI GetMenuString(HMENU hMenu, UINT uIDItem, LPSTR lpString, 
                             int nMaxCount, UINT uFlag);
UINT    WINAPI GetMenuState(HMENU hMenu, UINT uID, UINT uFlags);

BOOL    WINAPI SetMenuItemBitmaps(HMENU hMenu, UINT uPosition, UINT uFlags, 
                                  HBITMAP hbmpUnchecked, 
                                  HBITMAP hbmpChecked);
DWORD   WINAPI GetMenuCheckMarkDimensions(void);

#endif  /* NOMENUAPI */

BOOL    WINAPI TrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, 
                              int hReserved, HWND hWnd, 
                              const RECT* prcRect);

/* Flags for TrackPopupMenu */

#define TPM_LEFTBUTTON      0x0000
#define TPM_RIGHTBUTTON     0x0002

#define TPM_LEFTALIGN       0x0000
#define TPM_CENTERALIGN     0x0004
#define TPM_RIGHTALIGN      0x0008

#define TPM_TOPALIGN        0x0000L
#define TPM_VCENTERALIGN    0x0010L
#define TPM_BOTTOMALIGN     0x0020L

#endif  /* NOMENUS */

/* Menu messages */
#define WM_INITMENU         0x0116
#define WM_INITMENUPOPUP    0x0117

#if(!NOMENUS)

#define WM_MENUSELECT       0x011F
#define WM_MENUCHAR         0x0120

#endif  /* NOMENUS */

/* Menu and control command messages */
#define WM_COMMAND          0x0111

/**************************************************************************/
/*              Scroll bar support                                        */
/**************************************************************************/

#ifndef NOSCROLL

#define WM_HSCROLL          0x0114
#define WM_VSCROLL          0x0115

/* WM_H/VSCROLL commands */
#define SB_LINEUP           0
#define SB_LINELEFT         0
#define SB_LINEDOWN         1
#define SB_LINERIGHT        1
#define SB_PAGEUP           2
#define SB_PAGELEFT         2
#define SB_PAGEDOWN         3
#define SB_PAGERIGHT        3
#define SB_THUMBPOSITION    4
#define SB_THUMBTRACK       5
#define SB_TOP              6
#define SB_LEFT             6
#define SB_BOTTOM           7
#define SB_RIGHT            7
#define SB_ENDSCROLL        8

/* Scroll bar message */
#define SBM_SETPOS          0x00E0
#define SBM_GETPOS          0x00E1
#define SBM_SETRANGE        0x00E2
#define SBM_SETRANGEREDRAW  0x00E3
#define SBM_GETRANGE        0x00E4
#define SBM_ENABLE_ARROWS   0x00E5
#define SBM_SETSCROLLINFO   0x00E6
#define SBM_GETSCROLLINFO   0x00E7

#define SIF_RANGE           0x0001
#define SIF_PAGE            0x0002
#define SIF_POS             0x0004
#define SIF_DISABLENOSCROLL 0x0008
#define SIF_TRACKPOS        0x0010
#define SIF_ALL             (SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS)

typedef struct tagSCROLLINFO
{
    UINT    cbSize;
    UINT    fMask;
    int     nMin;
    int     nMax;
    UINT    nPage;
    int     nPos;
    int     nTrackPos;
} SCROLLINFO, *PSCROLLINFO, *LPSCROLLINFO;

typedef SCROLLINFO CONST *PCSCROLLINFO;
typedef SCROLLINFO CONST *LPCSCROLLINFO;

/* Scroll bar selection constants */
#define SB_HORZ             0
#define SB_VERT             1
#define SB_CTL              2
#define SB_BOTH             3

int     WINAPI SetScrollInfo(HWND hWnd, int nBar, PCSCROLLINFO pScrollInfo,
                             BOOL bRedraw);
BOOL    WINAPI GetScrollInfo(HWND hWnd, int nBar, PSCROLLINFO pScrollInfo);

BOOL    WINAPI EnableScrollBar(HWND hWnd, int nSBFlags, int nArrow);

/* EnableScrollBar() flags */
#define ESB_ENABLE_BOTH     0x0000
#define ESB_DISABLE_BOTH    0x0003

#define ESB_DISABLE_LEFT    0x0001
#define ESB_DISABLE_RIGHT   0x0002

#define ESB_DISABLE_UP      0x0001
#define ESB_DISABLE_DOWN    0x0002

#define ESB_DISABLE_LTUP    ESB_DISABLE_LEFT
#define ESB_DISABLE_RTDN    ESB_DISABLE_RIGHT

#endif  /* NOSCROLL */

/**************************************************************************/
/*              Clipboard manager                                         */
/**************************************************************************/

#ifndef NOCLIPBOARD

/* Predefined Clipboard Formats */
#define CF_TEXT              1

/* Clipboard Manager Functions */
BOOL    WINAPI OpenClipboard(HWND hWnd);
BOOL    WINAPI CloseClipboard(void);
BOOL    WINAPI EmptyClipboard(void);

HANDLE  WINAPI SetClipboardData(UINT uFormat, HANDLE hMem);
HANDLE  WINAPI GetClipboardData(UINT uFormat);

BOOL    WINAPI IsClipboardFormatAvailable(UINT uFormat);

UINT    WINAPI RegisterClipboardFormat(PCSTR pszFormat);
int     WINAPI GetClipboardFormatName(UINT uFormat, PSTR pszFormatName, 
                                      int nMaxCount);

/* Clipboard command messages */
#define WM_CUT              0x0300
#define WM_COPY             0x0301
#define WM_PASTE            0x0302
#define WM_CLEAR            0x0303
#define WM_UNDO             0x0304

#endif  /* NOCLIPBOARD */

/**************************************************************************/
/*              Cursor support                                            */
/**************************************************************************/

HCURSOR WINAPI LoadCursor(HINSTANCE hInstance, PCSTR pszCursorName);
HCURSOR WINAPI CreateCursor(HINSTANCE hInstance, int nHotX, int nHotY, 
                            int nWidth, int nHeight, 
                            const void* pANDPlane, const void* pXORPlane);
BOOL    WINAPI DestroyCursor(HCURSOR hCursor);

BOOL    WINAPI ShowCursor(BOOL bShow);

HCURSOR WINAPI SetCursor(HCURSOR hCursor);

/* Standard cursor resource IDs */
#define IDC_ARROW           MAKEINTRESOURCE(32752)
#define IDC_HAND            MAKEINTRESOURCE(32753)
#define IDC_IBEAM           MAKEINTRESOURCE(32754)
#define IDC_WAIT            MAKEINTRESOURCE(32755)
#define IDC_CROSS           MAKEINTRESOURCE(32756)
#define IDC_UPARROW         MAKEINTRESOURCE(32757)
#define IDC_SIZE            MAKEINTRESOURCE(32758)
#define IDC_ICON            MAKEINTRESOURCE(32759)
#define IDC_SIZENWSE        MAKEINTRESOURCE(32760)
#define IDC_SIZENESW        MAKEINTRESOURCE(32761)
#define IDC_SIZEWE          MAKEINTRESOURCE(32762)
#define IDC_SIZENS          MAKEINTRESOURCE(32763)
#define IDC_SIZEALL         MAKEINTRESOURCE(32764)
#define IDC_NO              MAKEINTRESOURCE(32765)
#define IDC_APPSTARTING     MAKEINTRESOURCE(32766)
#define IDC_HELP            MAKEINTRESOURCE(32767)

#define WM_SETCURSOR        0x0020

BOOL    WINAPI SetCursorPos(int x, int y);
BOOL    WINAPI GetCursorPos(POINT* pPoint);

BOOL    WINAPI ClipCursor(const RECT* pRect);
BOOL    WINAPI GetClipCursor(PRECT pRect);

BOOL    WINAPI SetMaxCursorRect(const RECT* pRect);

void    WINAPI BeginWaitCursor(void);
void    WINAPI EndWaitCursor(void);

/**************************************************************************/
/*              Icon support                                              */
/**************************************************************************/

#ifndef NOICONS

HICON   WINAPI LoadIcon(HINSTANCE hInstance, PCSTR pszIconName);
HICON   WINAPI CreateIcon(HINSTANCE hInstance, int nWidth, int nHeight, 
                          int cPlanes, int cBitsPixel, 
                          const void* pANDBits, 
                          const void* pXORBits);
BOOL    WINAPI DestroyIcon(HICON hIcon);

BOOL WINAPI GetIconDimension(HICON hIcon, PSIZE pSize);
BOOL WINAPI GetIconDimensionFromFile(PCSTR pszIconName, PSIZE pSize);

BOOL    WINAPI DrawIcon(HDC hdc, int x, int y, HICON hIcon);
void WINAPI DrawIconFromFile(HDC hdc, PCSTR pszIconName, int x, int y, int iw,int ih);
void WINAPI DrawIconFromFileEx(HDC hdc, PCSTR pszIconName, int x, int y, int iw,int ih, BOOL bStretch);

/* Standard icon resource IDs */
#define IDI_APPLICATION     MAKEINTRESOURCE(32512)
#define IDI_HAND            MAKEINTRESOURCE(32513)
#define IDI_QUESTION        MAKEINTRESOURCE(32514)
#define IDI_EXCLAMATION     MAKEINTRESOURCE(32515)
#define IDI_ASTERISK        MAKEINTRESOURCE(32516)

#endif  /* NOICONS */

/**************************************************************************/
/*              Message Box support                                       */
/**************************************************************************/

#ifndef NOMB

int     WINAPI MessageBox(HWND hWnd, PCSTR pszText, PCSTR pszCaption, 
                          UINT uType);
BOOL    WINAPI MessageBeep(UINT uType);

/* MessageBox() Flags   */

#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
#define MB_TYPEMASK                 0x0000000FL

#define MB_ICONHAND                 0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONASTERISK             0x00000040L
#define MB_ICONMASK                 0x000000F0L

#define MB_ICONWARNING              MB_ICONEXCLAMATION
#define MB_ICONERROR                MB_ICONHAND

#define MB_ICONINFORMATION          MB_ICONASTERISK
#define MB_ICONSTOP                 MB_ICONHAND

#define MB_DEFBUTTON1               0x00000000L
#define MB_DEFBUTTON2               0x00000100L
#define MB_DEFBUTTON3               0x00000200L
#define MB_DEFBUTTON4               0x00000300L
#define MB_DEFMASK                  0x00000F00L

#define MB_APPLMODAL                0x00000000L
#define MB_SYSTEMMODAL              0x00001000L
#define MB_TASKMODAL                0x00002000L
#define MB_HELP                     0x00004000L // Help Button

#define MB_NOFOCUS                  0x00008000L
#define MB_SETFOREGROUND            0x00010000L
#define MB_DEFAULT_DESKTOP_ONLY     0x00020000L

#define MB_TOPMOST                  0x00040000L
#define MB_RIGHT                    0x00080000L

#define MB_ENGLISH                  0x00100000L

#endif  /* NOMB */

/**************************************************************************/
/*              Caret support                                             */
/**************************************************************************/

BOOL    WINAPI CreateCaret(HWND hWnd, HBITMAP hBitmap, int nWidth, 
                           int nHeight);
BOOL    WINAPI DestroyCaret(void);

BOOL    WINAPI SetCaretPos(int x, int y);
BOOL    WINAPI GetCaretPos(POINT* pPoint);

BOOL    WINAPI HideCaret(HWND hWnd);
BOOL    WINAPI ShowCaret(HWND hWnd);
COLORREF WINAPI SetCaretColor(COLORREF crColor);

UINT    WINAPI GetCaretBlinkTime(void);
BOOL    WINAPI SetCaretBlinkTime(UINT uMSeconds);

/**************************************************************************/
/*              WM_SYSCOMMAND support                                     */
/**************************************************************************/

#define WM_SYSCOMMAND   0x0112

#ifndef NOSYSCOMMANDS

/* System Menu Command Values */
#define SC_SIZE         0xF000
#define SC_MOVE         0xF010
#define SC_MINIMIZE     0xF020
#define SC_MAXIMIZE     0xF030
#define SC_NEXTWINDOW   0xF040
#define SC_PREVWINDOW   0xF050
#define SC_CLOSE        0xF060
#define SC_VSCROLL      0xF070
#define SC_HSCROLL      0xF080
#define SC_MOUSEMENU    0xF090
#define SC_KEYMENU      0xF100
#define SC_ARRANGE      0xF110
#define SC_RESTORE      0xF120
#define SC_TASKLIST     0xF130
#define SC_SCREENSAVE   0xF140
#define SC_HOTKEY       0xF150

/* Obsolete names */
#define SC_ICON         SC_MINIMIZE
#define SC_ZOOM         SC_MAXIMIZE

#endif  /* NOSYSCOMMANDS */

/**************************************************************************/
/*              Dialog and Control Management                             */
/**************************************************************************/

#ifndef NOCTLMGR

/* cbWndExtra bytes needed by dialog manager for dialog classes */
#define DLGWINDOWEXTRA  32

/* Dialog styles */
#define DS_ABSALIGN         0x01L
#define DS_SYSMODAL         0x02L
#define DS_LOCALEDIT        0x20L
#define DS_SETFONT          0x40L
#define DS_MODALFRAME       0x80L
#define DS_NOIDLEMSG        0x100L

/* Dialog messages */
#define DM_GETDEFID         (WM_USER+0)
#define DM_SETDEFID         (WM_USER+1)

/* Returned in HIWORD() of DM_GETDEFID result if msg is supported */
#define DC_HASDEFID         0x534B

#endif  /* NOCTLMGR */

/* Dialog notification messages */
#define WM_INITDIALOG       0x0110
#define WM_NEXTDLGCTL       0x0028

#ifndef NOCTLMGR

typedef BOOL (CALLBACK* DLGPROC)(HWND, UINT, WPARAM, LPARAM);

#ifndef NOMSG
BOOL    WINAPI IsDialogMessage(HWND hDlg, MSG* pMsg);
BOOL    WINAPI IsInputMessage(UINT wMsgCmd);
#endif

LRESULT WINAPI DefDlgProc(HWND hDlg, UINT wMsgCmd, WPARAM wParam, 
                          LPARAM lParam);

/* Dialog item template */
typedef struct 
{
    DWORD   style;
    DWORD   dwExStyle;
    short   x;
    short   y;
    short   cx;
    short   cy;
    DWORD   id;
    PCSTR   lpszClassName;      // Dialog item class name
    PCSTR   lpszWindowText;     // Dialog item window text
} DLGITEMTEMPLATE;
typedef DLGITEMTEMPLATE *PDLGITEMTEMPLATE;
typedef DLGITEMTEMPLATE *LPDLGITEMTEMPLATE;
typedef CONST DLGITEMTEMPLATE *PCDLGITEMTEMPLATE;
typedef CONST DLGITEMTEMPLATE *LPDCLGITEMTEMPLATE;

/* Dialog template: */
typedef struct 
{
    DWORD   type;               // Dialog template type, must be zero
    DWORD   style;              // Dialog style
    DWORD   dwExStyle;          // Dialog extend style
    DWORD   cdit;               // Count of dialog control items
    short   x;                  // x-coordinates of dialog left-top corner
    short   y;                  // y-coordinates of dialog left-top corner
    short   cx;                 // width of dialog left-top corner
    short   cy;                 // height of dialog left-top corner
    PCSTR   lpszMenuName;       // Dialog menu name
    PCSTR   lpszClassName;      // Dialog class name
    PCSTR   lpszWindowText;     // Dialog title
    DLGITEMTEMPLATE dlgItems[1];// Dialog items : dlgItems[cdit]
} DLGTEMPLATE;
typedef DLGTEMPLATE* PDLGTEMPLATE;
typedef DLGTEMPLATE* LPDLGTEMPLATE;
typedef CONST DLGTEMPLATE* PCDLGTEMPLATE;
typedef CONST DLGTEMPLATE* LPCDLGTEMPLATE;

HWND    WINAPI CreateDialog(HINSTANCE hInstance, PCSTR pszTemplate, 
                            HWND hwndOwner, DLGPROC pDlgProc);
HWND    WINAPI CreateDialogIndirect(HINSTANCE hInstance, 
                                    PCDLGTEMPLATE pTemplate, 
                                    HWND hwndOwner, DLGPROC pDlgProc);
HWND    WINAPI CreateDialogParam(HINSTANCE hInstance, PCSTR pszTemplate, 
                                 HWND hwndOwner, DLGPROC pDlgProc, 
                                 LPARAM dwInitParam);
HWND    WINAPI CreateDialogIndirectParam(HINSTANCE hInstance, 
                                         PCDLGTEMPLATE pTemplate, 
                                         HWND hwndOwner, DLGPROC pDlgProc, 
                                         LPARAM dwInitParam);

int     WINAPI DialogBox(HINSTANCE hInstance, PCSTR pszTemplate, 
                         HWND hwndOwner, DLGPROC pDlgProc);
int     WINAPI DialogBoxIndirect(HINSTANCE hInstance, 
                                 PCDLGTEMPLATE pTemplate, 
                                 HWND hwndOwner, DLGPROC pDlgProc);
int     WINAPI DialogBoxParam(HINSTANCE hInstance, PCSTR pszTemplate, 
                              HWND hwndOwner, DLGPROC pDlgProc, 
                              LPARAM dwInitParam);
int     WINAPI DialogBoxIndirectParam(HINSTANCE hInstance, 
                                      PCDLGTEMPLATE pTemplate, 
                                      HWND hwndOwner, DLGPROC pDlgProc, 
                                      LPARAM dwInitParam);
#define WM_ENDDIALOG    0x0310

BOOL    WINAPI EndDialog(HWND hDlg, int nResult);

int     WINAPI GetDlgCtrlID(HWND hwndCtrl);
HWND    WINAPI GetDlgItem(HWND hDlg, int nItemID);
LRESULT WINAPI SendDlgItemMessage(HWND hDlg, int nItemID, UINT wMsgCmd, 
                                  WPARAM wParam, LPARAM lParam);

BOOL    WINAPI SetDlgItemText(HWND hDlg, int nItemID, PCSTR pszText);
int     WINAPI GetDlgItemText(HWND hDlg, int nItemID, LPSTR pszText, 
                              int nMaxCount);


BOOL    WINAPI CheckDlgButton(HWND hDlg, int nItemID, UINT uCheck);
BOOL    WINAPI CheckRadioButton(HWND hDlg, int nFirstID, int nLastID, 
                                int nCheckID);
UINT    WINAPI IsDlgButtonChecked(HWND hDlg, int nButtonID);

/* button check state code */
#define BST_UNCHECKED      0x0000
#define BST_CHECKED        0x0001
#define BST_INDETERMINATE  0x0002
#define BST_PUSHED         0x0004
#define BST_FOCUS          0x0008

HWND    WINAPI GetNextDlgGroupItem(HWND hDlg, HWND hCtl, BOOL bPrev);
HWND    WINAPI GetNextDlgTabItem(HWND hDlg, HWND hCtl, BOOL bPrev);

#define WM_GETDLGCODE           0x0087

#define DLGC_WANTARROWS         0x0001  /* Control wants arrow keys     */
#define DLGC_WANTTAB            0x0002  /* Control wants tab keys       */
#define DLGC_WANTALLKEYS        0x0004  /* Control wants all keys       */
#define DLGC_WANTMESSAGE        0x0004  /* Pass message to control      */
#define DLGC_HASSETSEL          0x0008  /* Understands EM_SETSEL message*/
#define DLGC_DEFPUSHBUTTON      0x0010  /* Default pushbutton           */
#define DLGC_UNDEFPUSHBUTTON    0x0020  /* Non-default pushbutton       */
#define DLGC_RADIOBUTTON        0x0040  /* Radio button                 */
#define DLGC_WANTCHARS          0x0080  /* Want WM_CHAR messages        */
#define DLGC_STATIC             0x0100  /* Static item: don't include   */
#define DLGC_BUTTON             0x2000  /* Button item: can be checked  */
#define DLGC_WANTRETURN         0x4000  /* Controls wants return keys   */

#define WM_CTLCOLORMSGBOX       0x0132
#define WM_CTLCOLOREDIT         0x0133
#define WM_CTLCOLORLISTBOX      0x0134
#define WM_CTLCOLORBTN          0x0135
#define WM_CTLCOLORDLG          0x0136
#define WM_CTLCOLORSCROLLBAR    0x0137
#define WM_CTLCOLORSTATIC       0x0138

#define WM_SETFONT              0x0030
#define WM_GETFONT              0x0031

#endif  /* NOCTLMGR */

/* Standard dialog button IDs */
#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7

/**************************************************************************/
/*              Owner draw control support                                */
/**************************************************************************/

/* Owner draw control types */
#define ODT_MENU        1
#define ODT_LISTBOX     2
#define ODT_COMBOBOX    3
#define ODT_BUTTON      4

/* Owner draw actions */
#define ODA_DRAWENTIRE  0x0001
#define ODA_SELECT      0x0002
#define ODA_FOCUS       0x0004

/* Owner draw state */
#define ODS_SELECTED    0x0001
#define ODS_GRAYED      0x0002
#define ODS_DISABLED    0x0004
#define ODS_CHECKED     0x0008
#define ODS_FOCUS       0x0010

#define WM_DRAWITEM         0x002B

typedef struct tagDRAWITEMSTRUCT
{
    UINT        CtlType;
    UINT        CtlID;
    UINT        itemID;
    UINT        itemAction;
    UINT        itemState;
    HWND        hwndItem;
    HDC         hDC;
    RECT        rcItem;
    DWORD       itemData;
} DRAWITEMSTRUCT;
typedef DRAWITEMSTRUCT* PDRAWITEMSTRUCT;

#define WM_MEASUREITEM      0x002C

typedef struct tagMEASUREITEMSTRUCT
{
    UINT        CtlType;
    UINT        CtlID;
    UINT        itemID;
    UINT        itemWidth;
    UINT        itemHeight;
    DWORD       itemData;
} MEASUREITEMSTRUCT;
typedef MEASUREITEMSTRUCT* PMEASUREITEMSTRUCT;

#define WM_DELETEITEM       0x002D

typedef struct tagDELETEITEMSTRUCT
{
    UINT       CtlType;
    UINT       CtlID;
    UINT       itemID;
    HWND       hwndItem;
    DWORD      itemData;
} DELETEITEMSTRUCT;
typedef DELETEITEMSTRUCT* PDELETEITEMSTRUCT;

#define WM_COMPAREITEM      0x0039

typedef struct tagCOMPAREITEMSTRUCT
{
    UINT        CtlType;
    UINT        CtlID;
    HWND        hwndItem;
    UINT        itemID1;
    DWORD       itemData1;
    UINT        itemID2;
    DWORD       itemData2;
} COMPAREITEMSTRUCT;
typedef COMPAREITEMSTRUCT* PCOMPAREITEMSTRUCT;

/**************************************************************************/
/*              Static control                                            */
/**************************************************************************/

#ifndef NOCTLMGR

/* Static Control Styles */
#define SS_LEFT             0x00000000L
#define SS_CENTER           0x00000001L
#define SS_RIGHT            0x00000002L
#define SS_ICON             0x00000003L
#define SS_BLACKRECT        0x00000004L
#define SS_GRAYRECT         0x00000005L
#define SS_WHITERECT        0x00000006L
#define SS_BLACKFRAME       0x00000007L
#define SS_GRAYFRAME        0x00000008L
#define SS_WHITEFRAME       0x00000009L
#define SS_SIMPLE           0x0000000BL
#define SS_LEFTNOWORDWRAP   0x0000000CL
#define SS_BITMAP           0x0000000DL
#define SS_NOPREFIX         0x00000080L

#ifndef NOWINMESSAGES

#define IMAGE_BITMAP        0
#define IMAGE_ICON          1
#define IMAGE_DISABLE       0x00010000
#define IMAGE_PUSHDOWN      0x00020000
#define IMAGE_FOCUS         0x00040000

/* Static Control Mesages */
#define STM_SETIMAGE        0x0170
#define STM_GETIMAGE        0x0171

#endif  /* NOWINMESSAGES */

#endif  /* NOCTLMGR */

/**************************************************************************/
/*              Button control                                            */
/**************************************************************************/

#ifndef NOCTLMGR

/* Button Control Styles */
#define BS_PUSHBUTTON       0x00000000L
#define BS_DEFPUSHBUTTON    0x00000001L
#define BS_CHECKBOX         0x00000002L
#define BS_AUTOCHECKBOX     0x00000003L
#define BS_RADIOBUTTON      0x00000004L
#define BS_3STATE           0x00000005L
#define BS_AUTO3STATE       0x00000006L
#define BS_GROUPBOX         0x00000007L
#define BS_AUTORADIOBUTTON  0x00000008L
#define BS_OWNERDRAW        0x00000009L
#define BS_LEFTTEXT         0x00000020L
#define BS_LEFT             0x00000040L
#define BS_CENTER           0x00000080L

/* Button Control Messages  */
#define BM_GETCHECK         0x00F0
#define BM_SETCHECK         0x00F1
#define BM_GETSTATE         0x00F2
#define BM_SETSTATE         0x00F3
#define BM_GETIMAGE         0x00F4
#define BM_SETIMAGE         0x00F5
#define BM_SETSTYLE         0x00F6

/* Constant for BM_GETIMAGE and BM_SETIMAGE*/
#define IMAGE_NORMAL        0x00000000
#define IMAGE_DISABLE       0x00010000
#define IMAGE_PUSHDOWN      0x00020000
#define IMAGE_FOCUS         0x00040000
#define IMAGE_MASKRADIO     0x00080000
#define IMAGE_CHECKEDRADIO  0x00100000
#define IMAGE_UNCHECKEDRADIO 0x00200000

/* User Button Notification Codes */
#define BN_CLICKED          0
#define BN_SETFOCUS         1
#define BN_KILLFOCUS        2

#endif  /* NOCTLMGR */

/**************************************************************************/
/*              Edit control                                              */
/**************************************************************************/

#ifndef NOCTLMGR

/* Edit control styles */
#ifndef NOWINSTYLES
#define ES_LEFT             0x00000000L
#define ES_CENTER           0x00000001L
#define ES_RIGHT            0x00000002L
#define ES_MULTILINE        0x00000004L
#define ES_UPPERCASE        0x00000008L
#define ES_LOWERCASE        0x00000010L
#define ES_PASSWORD         0x00000020L
#define ES_AUTOVSCROLL      0x00000040L
#define ES_AUTOHSCROLL      0x00000080L
#define ES_NOHIDESEL        0x00000100L
#define ES_USERSTRING       0x00000200L
#define ES_NOSOFTKEYBOARD   0x00000400L
#define ES_READONLY         0x00000800L
#define ES_WANTRETURN       0x00001000L
#define ES_NUMBER           0x00002000L
#define ES_UNDERLINE        0x00004000L
#define ES_TITLE            ES_NOSOFTKEYBOARD
#define ES_GSM				ES_NOSOFTKEYBOARD
#endif  /* NOWINSTYLES */

/* Edit control messages */
#ifndef NOWINMESSAGES
#define EM_GETSEL               0x00B0
#define EM_SETSEL               0x00B1
#define EM_LINESCROLL           0x00B2
#define EM_GETMODIFY            0x00B3
#define EM_SETMODIFY            0x00B4
#define EM_GETLINECOUNT         0x00B5
#define EM_LINELENGTH           0x00B6
#define EM_REPLACESEL           0x00B7
#define EM_GETLINE              0x00B8
#define EM_LIMITTEXT            0x00B9
#define EM_CANUNDO              0x00BA
#define EM_UNDO                 0x00BB
#define EM_SETPASSWORDCHAR      0x00BC
#define EM_EMPTYUNDOBUFFER      0x00BD
#define EM_SETREADONLY          0x00BE
#define EM_GETLIMITTEXT         0x00BF
#define EM_GETFIRSTVISIBLELINE  0x00C0
#define EM_GETHANDLE            0x00C1
#define EM_GETSELTEXT           0x00C2
#define EM_GETCARETSTATE        0x00C3
#define EM_SETTITLE             0x00C4
#define EM_SETLIMITTEXT     EM_LIMITTEXT
#endif  /* NOWINMESSAGES */

/* Edit control notification codes */
#define EN_SETFOCUS         0x0100
#define EN_KILLFOCUS        0x0200
#define EN_CHANGE           0x0300
#define EN_UPDATE           0x0400
#define EN_ERRSPACE         0x0500
#define EN_MAXTEXT          0x0501

#endif  /* NOCTLMGR */

/**************************************************************************/
/*              Scroll bar control                                        */
/**************************************************************************/

/* Also see scrolling support */

#ifndef NOCTLMGR

#ifndef NOWINSTYLES

/* Scroll bar styles */
#define SBS_HORZ                    0x0000L
#define SBS_VERT                    0x0001L

#endif  /* NOWINSTYLES */

#endif  /* NOCTLMGR */

/**************************************************************************/
/*              Listbox control                                           */
/**************************************************************************/

#ifndef NOCTLMGR

/* Listbox styles */
#ifndef NOWINSTYLES
#define LBS_NOTIFY              0x0001L
#define LBS_SORT                0x0002L
#define LBS_NOREDRAW            0x0004L
#define LBS_MULTIPLESEL         0x0008L
#define LBS_OWNERDRAWFIXED      0x0010L
#define LBS_OWNERDRAWVARIABLE   0x0020L
#define LBS_HASSTRINGS          0x0040L
#define LBS_NOSEL               0x0080L
#define LBS_BITMAP              0x0100L
#define LBS_MULTICOLUMN         0x0200L
#define LBS_USERSTRINGS         0x0400L
#define LBS_EXTENDEDSEL         0x0800L
#define LBS_DISABLENOSCROLL     0x1000L
#define LBS_ICON                0x2000L
#define LBS_MULTILINE           0x4000L

#define LBS_STANDARD    (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)

#endif  /* NOWINSTYLES */

/* Listbox messages */
#ifndef NOWINMESSAGES
#define LB_ADDSTRING            0x0180
#define LB_INSERTSTRING         0x0181
#define LB_DELETESTRING         0x0182
#define LB_RESETCONTENT         0x0183
#define LB_SETSEL               0x0184
#define LB_SETCURSEL            0x0185
#define LB_GETSEL               0x0186
#define LB_GETCURSEL            0x0187
#define LB_GETTEXT              0x0188
#define LB_GETTEXTLEN           0x0189
#define LB_GETCOUNT             0x018A
#define LB_SELECTSTRING         0x018B
#define LB_GETTOPINDEX          0x018C
#define LB_FINDSTRING           0x018D
#define LB_GETSELCOUNT          0x018E
#define LB_GETSELITEMS          0x018F
#define LB_SETTOPINDEX          0x0190
#define LB_GETITEMDATA          0x0191
#define LB_SETITEMDATA          0x0192
#define LB_SELITEMRANGE         0x0193
#define LB_FINDSTRINGEXACT      0x0194
#define LB_GETIMAGE             0x0195
#define LB_SETIMAGE             0x0196
#define LB_SETCOLUMNWIDTH       0x0197
#define LB_BEGININIT            0x0198
#define LB_ENDINIT              0x0199
#define LB_SETTEXT              0x019A
#define LB_GETITEMRECT          0x019B
#define LB_DIR                  0x019C
#define LB_GETITEMHEIGHT        0x019D

#define LB_INSERT               0x019E
#define LB_MAXSELCOUNT			0x019F

#define LB_SETAUXTEXT           0x01A0

#endif  /* NOWINMESSAGES */

#define DDL_READWRITE       0x0000
#define DDL_READONLY        0x0001
#define DDL_HIDDEN          0x0002
#define DDL_SYSTEM          0x0004
#define DDL_DIRECTORY       0x0010
#define DDL_ARCHIVE         0x0020

#define DDL_POSTMSGS        0x2000
#define DDL_DRIVES          0x4000
#define DDL_EXCLUSIVE       0x8000

/* Listbox notification codes */
#define LBN_SELCHANGE           1
#define LBN_DBLCLK              2
#define LBN_SETFOCUS            3
#define LBN_KILLFOCUS           4
#define LBN_MAXSEL				5
#define LBN_CLICKED             6
#define LBN_SETFONT             7

/* Listbox message return values */
#define LB_OKAY                 0
#define LB_ERR                  (-1)
#define LB_ERRSPACE             (-2)

#define LB_CTLCODE              0L

#endif  /* NOCTLMGR */

/**************************************************************************/
/*              Combo box control                                         */
/**************************************************************************/

#ifndef NOCTLMGR

/* Combo box styles */
#ifndef NOWINSTYLES
#define CBS_SIMPLE              0x0001L
#define CBS_DROPDOWN            0x0002L
#define CBS_DROPDOWNLIST        0x0004L
#define CBS_AUTOHSCROLL         0x0010L
#define CBS_SORT                0x0020L
#define CBS_HASSTRINGS          0x0200L
#define CBS_ADDNEWSTRINGS       0x0400L
#define CBS_USERSTRINGS         0x0800L
#endif  /* NOWINSTYLES */

/* Combo box messages */
#ifndef NOWINMESSAGES
#define CB_GETEDITSEL           0x0140
#define CB_LIMITTEXT            0x0141
#define CB_SETEDITSEL           0x0142
#define CB_ADDSTRING            0x0143
#define CB_DELETESTRING         0x0144
#define CB_GETCOUNT             0x0145
#define CB_GETCURSEL            0x0146
#define CB_GETLBTEXT            0x0147
#define CB_GETLBTEXTLEN         0x0148
#define CB_INSERTSTRING         0x0149
#define CB_RESETCONTENT         0x014A
#define CB_FINDSTRING           0x014B
#define CB_SELECTSTRING         0x014C
#define CB_SETCURSEL            0x014D
#define CB_SHOWDROPDOWN         0x014E
#define CB_GETITEMDATA          0x014F
#define CB_SETITEMDATA          0x0150
#define CB_FINDSTRINGEXACT      0x0151
#define CB_BEGININIT            0x0152
#define CB_ENDINIT              0x0153
#define CB_SETTEXT              0x0154
#define CB_GETDROPDOWNSTATE     0x0155//change 7/25
#define CB_GETITEMHEIGHT        0x0156
#endif  /* NOWINMESSAGES */

/* Combo box notification codes */
#define CBN_ERRSPACE        (-1)
#define CBN_SELCHANGE       1
#define CBN_DBLCLK          2
#define CBN_SETFOCUS        3
#define CBN_KILLFOCUS       4
#define CBN_EDITCHANGE      5
#define CBN_EDITUPDATE      6
#define CBN_DROPDOWN        7
#define CBN_CLOSEUP         8
#define CBN_SELENDOK        9
#define CBN_SELENDCANCEL    10

/* Combo box message return values */
#define CB_OKAY             0
#define CB_ERR              (-1)
#define CB_ERRSPACE         (-2)

#endif  /* NOCTLMGR */

/**************************************************************************/
/*              NumSpinBox control                                        */
/**************************************************************************/

#ifndef NOCTLMGR

#ifndef NOWINSTYLES
/* SpinNumBox styles */
#define NSBS_LEFT                    0x0001L
#define NSBS_RIGHT                   0x0002L
#define NSBS_CENTER                  0x0004L
#endif  /* NOWINSTYLES */

#ifndef NOWINMESSAGES
#define NSBM_SETINITDATA             0x0156L                                
#define NSBM_GETDATA                 0x0157L                                
#endif /*NOWINMESSAGES*/

#define NSBN_RANGEMAX   1
#define NSBN_RANGEMIN   2
#define NSBN_SETFOCUS   3
#define NSBN_KILLFOCUS  4
#define NSBN_CHANGE     5

#endif  /* NOCTLMGR */


#endif  /* NOUSER */

/**************************************************************************/
/*              StrSpinBox control                                        */
/**************************************************************************/

#ifndef NOCTLMGR

#ifndef NOWINSTYLES
/* SpinStrBox styles */
#define SSBS_LEFT                   0x0001L
#define SSBS_RIGHT                  0x0002L
#define SSBS_CENTER                 0x0004L
#define SSBS_ARROWRIGHT             0x0008L
#define SSBS_SWAPFONT               0x0010L
#endif  /* NOWINSTYLES */

#ifndef NOWINMESSAGES
#define SSBM_ADDSTRING              0x0158L                                
#define SSBM_DELETESTRING           0x0159L                                
#define SSBM_SETCURSEL              0x015AL
#define SSBM_GETCURSEL              0x015BL
#define SSBM_SETTEXT                0x015CL
#define SSBM_GETTEXT                0x015DL
#define SSBM_GETTEXTLEN             0x015EL
#define SSBM_GETCOUNT               0x015FL
#define SSBM_SETTITLE               0x0160L
#define SSBM_GETTITLE               0x0161L
#endif /*NOWINMESSAGES*/

#define SSBN_SETFOCUS   1
#define SSBN_KILLFOCUS  2
#define SSBN_CHANGE     3
#define SSBN_CLICKED    4
#define SSBN_ERASECTLBK 5

#endif  /* NOCTLMGR */

/**************************************************************************/
/*              Windows hook support                                      */
/**************************************************************************/

#ifndef NOWH

DECLARE_HANDLE(HHOOK);

typedef LRESULT (CALLBACK* HOOKPROC)(int code, WPARAM wParam, 
                                     LPARAM lParam);

/* SetWindowsHookEx() codes */
#define WH_MIN              0
#define WH_KEYBOARD         0
#define WH_MOUSE            1
#define WH_MAX              1

/* Hook Codes */
#define HC_ACTION           0
#define HC_NOREMOVE         1

/* Structure used by WH_MOUSE */
typedef struct tagMOUSEHOOKSTRUCT
{
    POINT   pt;
    HWND    hwnd;
    UINT    wHitTestCode;
    DWORD   dwExtraInfo;
} MOUSEHOOKSTRUCT, *PMOUSEHOOKSTRUCT, *LPMOUSEHOOKSTRUCT;

HHOOK   WINAPI SetWindowsHookEx(int idHook, HOOKPROC pfnHookProc, 
                                HINSTANCE hInstance, DWORD dwThreadId);
BOOL    WINAPI UnhookWindowsHookEx(HHOOK hHook);
LRESULT WINAPI CallNextHookEx(HHOOK hHook, int code, WPARAM wParam, 
                              LPARAM lParam);

#endif /* !NOWH */

/**************************************************************************/
/*              Change display settings                                   */
/**************************************************************************/

#define WM_DISPLAYCHANGE    0x007E

/* size of a device name string */
#define CCHDEVICENAME 32

/* size of a form name string */
#define CCHFORMNAME 32

/* field selection bits */
#define DM_BITSPERPEL       0x00040000L
#define DM_PELSWIDTH        0x00080000L
#define DM_PELSHEIGHT       0x00100000L
#define DM_DISPLAYFLAGS     0x00200000L
#define DM_DISPLAYFREQUENCY 0x00400000L

typedef struct _devicemode 
{
    BYTE dmDeviceName[CCHDEVICENAME];
    WORD dmSpecVersion;
    WORD dmDriverVersion;
    WORD dmSize;
    WORD dmDriverExtra;
    DWORD dmFields;
    short dmOrientation;
    short dmPaperSize;
    short dmPaperLength;
    short dmPaperWidth;
    short dmScale;
    short dmCopies;
    short dmDefaultSource;
    short dmPrintQuality;
    short dmColor;
    short dmDuplex;
    short dmYResolution;
    short dmTTOption;
    short dmCollate;
    BYTE   dmFormName[CCHFORMNAME];
    WORD   dmLogPixels;
    DWORD  dmBitsPerPel;
    DWORD  dmPelsWidth;
    DWORD  dmPelsHeight;
    DWORD  dmDisplayFlags;
    DWORD  dmDisplayFrequency;
    DWORD  dmICMMethod;
    DWORD  dmICMIntent;
    DWORD  dmMediaType;
    DWORD  dmDitherType;
    DWORD  dmICCManufacturer;
    DWORD  dmICCModel;
    DWORD  dmPanningWidth;
    DWORD  dmPanningHeight;
} DEVMODE, *PDEVMODE, *LPDEVMODE;

/* Flags for ChangeDisplaySettings */

/* Return values for ChangeDisplaySettings */
#define DISP_CHANGE_SUCCESSFUL       0
#define DISP_CHANGE_RESTART          1
#define DISP_CHANGE_FAILED          -1
#define DISP_CHANGE_BADMODE         -2
#define DISP_CHANGE_NOTUPDATED      -3
#define DISP_CHANGE_BADFLAGS        -4
#define DISP_CHANGE_BADPARAM        -5

LONG    WINAPI ChangeDisplaySettings(PDEVMODE pDevMode, DWORD dwFlags);

BOOL    WINAPI EnumDisplaySettings(PCSTR pszDeviceName, DWORD iModeNum,
                                   PDEVMODE pDevMode);

void WINAPI UpdateScreen(void);

#ifdef __cplusplus
}                       /* End of extern "C" */
#endif

#endif  /* __window_h */
