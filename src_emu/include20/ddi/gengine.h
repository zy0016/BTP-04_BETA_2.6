/**************************************************************************\
**                                                                        **
**                     Hopen OS Debug and emulate                         **
**                                                                        **
**  Copyright (C) 1998 by CASS Corptation & Software Engineering Center   **
**                       All Rights Reserved                              **
**                                                                        **
** The contents of this file are subject to the CASS License;             **
**                                                                        **
** Software distributed under this packet is distributed on an "AS IS"    **
** basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.        **
**                                                                        **
** Duplicate this file without CASS license is illegal.                   **
**                                                                        **
\**************************************************************************/

/***************************************************************************
 *
 * CPU program for Stb_Hardware.
 *
 * $Revision: 1 $     $Date: 99-11-08 14:21 $    
 *
 * $Log: /Hopen/include/ddi/gengine.h $
 * 
 * 1     99-11-08 14:21 Dling
 * 
 * 1     99-11-08 13:38 Dling
 * 
 * 2     99-08-15 16:26 Dling
 * 
 * 
 ***************************************************************************
 */

#ifndef _gengine_h_
#define _gengine_h_
//#include <_ansi.h>

typedef struct GE_Attrib
{
  short   size;
  short   version;
  char    dev_name[16];
  short   modes;
  short   bit_width;
  long    vram_size;
  long    func_mask;
} GE_ATTRIB;

/*--- graphics driver capability ---*/

#define GE_BITBLT               0x00000001
#define GE_ROP2                 0x00000002
#define GE_ROP3                 0x00000004
#define GE_FILLRECT             0x00000008
#define GE_LINE                 0x00000010
#define GE_HLINE                0x00000020
#define GE_VLINE                0x00000040
#define GE_CLIP                 0x00000080
#define GE_CURSOR               0x00000100
#define GE_LINESTYLE            0x00000200
#define GE_TRANSPARENT          0x00000400
#define GE_PATTERN              0x00000800
#define GE_MONOBLT              0x00001000

typedef struct GE_TVInfo
{
  short   tv_output_enable;
  short   tv_mode;
  short   tv_connector;
} GE_TVINFO;

typedef struct GE_CRTMode
{
  short   size;
  short   version;
  short	  attrib;
  short   width;
  short   height;
  short   planes;
  short   pix_bits;
  short   pitch;
  short   palettes;
  short   vesa_mode;
  short   tv_mode;
  short   frames;
} GE_CRTMODE;

/*-- CRT mode attribute ---*/

#define M_COLOR         0x0001
#define M_PALETTE       0x0002
#define M_VESA          0x0004
#define M_TV            0x0008
#define M_LCD           0x0010
#define M_VGAOUT        0x0020
#define M_SVIDEO        0x0040
#define M_AVOUT         0x0080
#define M_RGBOUT        0x0100

/*--- TV mode attribute ---*/

#define	PAL_50_443	0x0001
#define	PAL_60_334	0x0002
#define	NTSC_50_443	0x0004
#define	NTSC_60_334	0x0008

typedef struct GE_DrawPara
{
  long    field_mask;
  long    rop_code;
  short   pen;
  short   pattern;
  short   clip_flag;
  short   bk_mode;
  long    bk_color;
  long    fg_color;
  short   pattern_org_x;
  short   pattern_org_y;
  short   clip_rect_x1;
  short   clip_rect_y1;
  short   clip_rect_x2;
  short   clip_rect_y2;
} GE_DRAWPARA;

/*--- field masks ---*/

#define F_PEN           0x00000001
#define F_PATTERN       0x00000002
#define F_ROP           0x00000004
#define F_BKMODE        0x00000008
#define F_BKCOLOR       0x00000010
#define F_FGCOLOR       0x00000020
#define F_PATTERNXY     0x00000040
#define F_CLIPFLAG      0x00000100
#define F_CLIPRECT      0x00000200

/*--- BK mode ---*/

#define BK_TRANSPARENT     1
#define BK_OPAQUE          2
#define BK_NEWTRANSPARENT  3

/*--- ternary raster operations ---*/

#ifndef SRCCOPY

#define SRCCOPY       0x00CC0020 /* dest = source                   */
#define SRCPAINT      0x00EE0086 /* dest = source OR dest           */
#define SRCAND        0x008800C6 /* dest = source AND dest          */
#define SRCINVERT     0x00660046 /* dest = source XOR dest          */
#define SRCERASE      0x00440328 /* dest = source AND (NOT dest )   */
#define NOTSRCCOPY    0x00330008 /* dest = (NOT source)             */
#define NOTSRCERASE   0x001100A6 /* dest = (NOT src) AND (NOT dest) */
#define MERGECOPY     0x00C000CA /* dest = (source AND pattern)     */
#define MERGEPAINT    0x00BB0226 /* dest = (NOT source) OR dest     */
#define PATCOPY       0x00F00021 /* dest = pattern                  */
#define PATPAINT      0x00FB0A09 /* dest = DPSnoo                   */
#define PATINVERT     0x005A0049 /* dest = pattern XOR dest         */
#define DSTINVERT     0x00550009 /* dest = (NOT dest)               */
#define BLACKNESS     0x00000042 /* dest = BLACK                    */
#define WHITENESS     0x00FF0062 /* dest = WHITE                    */

#endif

typedef struct GE_RGBQuad
{
  unsigned char  rgbReserved;
  unsigned char  rgbRed;
  unsigned char  rgbGreen;
  unsigned char  rgbBlue;
} GE_RGBQUAD;


///////////////////////////////////////////////////////////////////////////////
//
// Declaration of function prototypes
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _EXFUN
#define _EXFUN(name, proto)	name proto
#endif


#ifdef __cplusplus
extern "C" {
#endif

GE_TVINFO      *_EXFUN(GE_GetTVInfo, (void));
GE_ATTRIB      *_EXFUN(GE_GetAttrib, (void));
GE_CRTMODE     *_EXFUN(GE_EnumMode, (int index));
int		_EXFUN(GE_Init,	(int mode, long	outdev));
int		_EXFUN(GE_SetMode, (int	mode, long outdev));
int     _EXFUN(GE_GetMode, (void));
void*	_EXFUN(GE_GetVRAMAddr, (void));

int		_EXFUN(GE_CreateCursor, (int handle, int hot_spot_x, int hot_spot_y, int width, int height, long fg_color, long bk_color, void *pCursorData));
int		_EXFUN(GE_SelectCursor, (int handle));
int		_EXFUN(GE_SetCursorPos, (int x, int y));
int		_EXFUN(GE_ShowCursor, (int mode));

int		_EXFUN(GE_CreatePen, (int handle, long pattern));
int		_EXFUN(GE_CreatePattern, (int handle, void *pData));
int		_EXFUN(GE_CreateMonoPattern, (int handle, void *pData, long fgcolor, long bkcolor));
int		_EXFUN(GE_SetDrawPara, (GE_DRAWPARA *pDrawPara));

int		_EXFUN(GE_ClearScreen, (long color));
int		_EXFUN(GE_DrawPixel, (int x, int y, long color));
int		_EXFUN(GE_DrawLine, (int x1, int y1, int x2, int y2));
int		_EXFUN(GE_FillRect, (int x, int y, int width, int height, long color));
int		_EXFUN(GE_BitBlt, (int destx, int desty, int width, int height, int pitch,
							int bmpheight, void *pBmpData, int srcx, int srcy));
int		_EXFUN(GE_MonoBitBlt, (int destx, int desty, int width, int height, int pitch,
							int bmpheight, void *pBmpData, int srcx, int srcy));
int		_EXFUN(GE_ScreenBitBlt, (int x, int y, int width, int height, int srcx, int srcy));

int		_EXFUN(GE_Scroll, (int x, int y, int width, int height, int dx, int dy));
int		_EXFUN(GE_GetImage, (int x, int y, int width, int height, int pitch, void *pMemBmp));
int		_EXFUN(GE_PutImage, (int x, int y, int width, int height, int pitch, void *pMemBmp));
int		_EXFUN(GE_GetPalette, (int out_p, GE_RGBQUAD *out_pal, int out_num));
int		_EXFUN(GE_SetPalette, (int in_p, GE_RGBQUAD *in_pal, int in_num));
int		_EXFUN(GE_UpdateScreen,	(void));

unsigned short	_EXFUN(_swap16,	(unsigned short	data16));
unsigned long	_EXFUN(_swap32,	(unsigned long data32));

#ifdef __cplusplus
};
#endif

#endif /* _gengine_h */
