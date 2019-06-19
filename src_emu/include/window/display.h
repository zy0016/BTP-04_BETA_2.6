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

#ifndef __DISPLAY_H
#define __DISPLAY_H

#define _DISPLAY_VER 0x0200

#include "hpdef.h"

#ifndef _TYPEDEF_
#define _TYPEDEF_

#ifdef GUI_CALL
typedef void*   PDEV;  /* Device pointer type for window system */
#endif

#endif

typedef int32   RGB;

/* Define struct for display device infomation */
typedef struct DisplayDevInfo 
{
    int32   attrib;
    int32   width;
    int32   height;
    int8    planes;
    int8    bits_pixel;
    int8    mode;
    int8    unused;
    int32   phys_width;
    int32   phys_height;
    int16   pitch;
    int16   vesa_mode;
    int16   tv_mode;
    int16   frames;
} DISPLAYDEVINFO, *PDISPLAYDEVINFO;

/*
// Define constant for attrib field
#define DDA_PALETTE         0x00000001  / * 有调色板     * /
#define DDA_DIRECT_DRAW     0x00000002  / * 支持直接写屏 * /
#define DDA_ENERGY_SAVE     0x00000004  / * 具有节电功能 * /
#define DDA_ANIMATE_CURSOR  0x00000008  / * 支持动画光标 * /
#define DDA_HARDWARE_CURSOR 0x00000010  / * 支持硬件光标 * /
#define DDA_ROP3_SUPPORT    0x00000020  / * 支持ROP3操作 * /
#define DDA_REGION_CLIP     0x00000040  / * 支持复杂裁剪 * /
#define DDA_VESA            0x00040000  / * 该方式是一种 VESA方式 * /
#define DDA_TV              0x00080000  / * 该方式是一种 TV 方式 * /
#define DDA_LCD             0x00100000  / * 该方式是一种 LCD 方式  * /
#define DDA_VGAOUT          0x00200000  / * 该方式可以用 VGA 输出 * /
#define DDA_AVOUT           0x00400000  / * 该方式可以用 AV  输出 * /
#define DDA_SVIDEO          0x00800000  / * 该方式可以用 S 端子输出 * /
#define DDA_RGB             0x01000000  / * 该方式可以用RGB分量输出 * /

/ * Define constant for mode field * /
#define DDM_VGA             0           / * 标准VGA16色方式      * /
#define DDM_VGA256          1           / * Super VGA 256色方式  * /
#define DDM_VGA32K          2           / * Super VGA 32K彩色方式* /
#define DDM_VGA64K          3           / * Super VGA 64K彩色方式* /
#define DDM_RGB24           4           / * 24位R、G、B方式      * /
#define DDM_RGB32           5           / * 32位R、G、B方式      * /
#define DDM_YUV24           6           / * 24位Y、U、V方式      * /
#define DDM_YUV32           7           / * 32位Y、U、V方式      * /
#define DDM_OTHER           8           / * 其它方式。           * /*/


/* Define struct for bitmap */
typedef struct DIBitmap 
{
    int16   type;               /* Bitmap type                  */
    int16   width;              /* Bitmap width                 */
    int16   height;             /* Bitmap height                */
    int16   palette_entries;    /* number of palette entries    */
    int32   bmp_size;           /* Size of bmpBuffer            */
    void*   pBuffer;            /* Buffer point for bitmap data */
    void*   pPalette;           /* Palette pointer              */
} DIBITMAP, *PDIBITMAP;

/* Define constant for BITMAP type field */
#define BMP_MONO    0   /* 单色位图，调色板入口数为2，每像素1位     */
#define BMP_GRAY4   1   /* 四色位图，调色板入口数为4，每像素2位     */
#define BMP_4       1   /* 四色位图，调色板入口数为4，每像素2位     */
#define BMP_16      2   /* 16色位图，调色板入口数为16，每像素4位    */
#define BMP_256     3   /* 256色位图，调色板入口数为256，每像素8位  */
#define BMP_RGB16   4   /* 16位高彩色位图，无调色板，每像素16位。   */
#define BMP_RGB24   5   /* 24位真彩色位图，无调色板，每像素24位。   */
#define BMP_RGB32   6   /* 32位真彩色位图，无调色板，每像素32位。   */
#define BMP_RGB12   7   /* 16位高彩色位图，无调色板，每像素16位。   */

#ifndef TEXTBMP_STRUCT
#define TEXTBMP_STRUCT

/* Define struct for text bitmap */
typedef struct TextBmp
{
    int16   width;          /* Mono bitmap width       */
    int16   height;         /* Mono bitmap height      */
    int32   data[1];        /* Mono bitmap data buffer */
} TEXTBMP, *PTEXTBMP;

/* Defines text bitmap type */
#define TB_NORMAL   0
#define TB_TVFONT   1

typedef struct
{
    int16 flag;             /* must be 0                */
    int16 type;             /* text bitmap type         */
    int16 width;            /* text bitmap width        */
    int16 height;           /* text bitmap height       */
    int32 data[1];          /* text bitmap data buffer  */
} TEXTBMPEX, *PTEXTBMPEX;

#endif  /* TEXTBMP_STRUCT */

#ifdef GUI_CALL

typedef void* PMEMDEV;
typedef void* PDDBITMAP;
typedef void* PPHYSBMP;
typedef void* PPHYSPEN;
typedef void* PPHYSBRUSH;
typedef void* PPHYSPALETTE;
typedef void* PPHYSCURSOR;
typedef void* PSCREENSAVE;

#else

typedef struct PhysDev*     PDEV;
typedef struct MemDev*      PMEMDEV;
typedef struct MemDev*      PDDBITMAP;
typedef struct MemDev*      PPHYSBMP;
typedef struct PhysPen*     PPHYSPEN;
typedef struct PhysBrush*   PPHYSBRUSH;
typedef struct PhysPalette* PPHYSPALETTE;
typedef struct PhysCursor*  PPHYSCURSOR;
typedef struct ScreenSave*  PSCREENSAVE;

/* Define struct for point */
typedef struct tagPOINT
{
    int32 x;
    int32 y;
} POINT, *PPOINT;

/* Define struct for rect */
typedef struct tagRECT
{
    int32 x1;
    int32 y1;
    int32 x2;
    int32 y2;
}RECT, *PRECT;

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

#endif  /* GUI_CALL */

/* Define struct for region */
typedef struct Region
{
    int32 count;                /* rectangle count */
    RECT rect[1];               /* rectangle array */
} REGION, *PREGION;

/* Define struct for log pen */
typedef struct DrvLogPen
{
    int8    style;              /* Pen style        */
    int8    width;              /* Pen width        */
    int8    end_style;          /* Pen end style    */
    int8    frame_mode;         /* Pen frame mode   */
    RGB     color;              /* Pen color        */
}DRVLOGPEN, *PDRVLOGPEN;

/* Define constant for pen style */
#define PS_SOLID        0       /* 实线笔           */
#define PS_DASH         1       /* 虚线笔           */
#define PS_DOT          2       /* 点线笔           */
#define PS_DASHDOT      3       /* 点划线笔         */
#define PS_DASHDOTDOT   4       /* 双点划线笔       */
#define PS_NULL         5       /* 空笔             */

/* Define constant for pen end style */
#define PES_DEFAULT     0       /* 系统默认形       */
#define PES_RECTANGLE   1       /* 方形             */
#define PES_TRIANGLE    2       /* 三角型           */
#define PES_CIRCLE      3       /* 圆型             */

/* Define constant for pen frame mode */
#define PFM_INTERNAL    0       /* 线被画在封闭图形的内部           */
#define PFM_CENTER      1       /* 以封闭图形的边界作为笔迹的中心   */

typedef struct DrvLogBrush
{
    int16   width;              /* Brush width      */
    int16   height;             /* Brush height     */
    int16   style;              /* Brush style      */       
    int16   hatch_style;        /* Hatch style      */
    RGB     color;              /* Brush color      */
    void*   pPattern;           /* Bitmap pattern   */
} DRVLOGBRUSH, *PDRVLOGBRUSH;


/* Define constant for brush style */
#define BS_HOLLOW       0       /* 空刷子               */
#define BS_NULL         0       /* 空刷子               */
#define BS_SOLID        1       /* 实心刷子             */
#define BS_BITMAP       2       /* 设备无关位图刷子     */
#define BS_HATCHED      3       /* 阴影刷子             */
#define BS_PATTERN      4       /* 内存单色点位图刷子   */

/* Define constant for hatch style */
#define HS_BDIAGONAL    0       /* 45度向上阴影线（从左到右）*/
#define HS_CROSS        1       /* 水平及垂直的十字阴影线    */
#define HS_DIAGCROSS    2       /* 45度十字阴影线；          */
#define HS_FDIAGONAL    3       /* 45度向下阴影线（从左到右）*/
#define HS_HORIZONTAL   4       /* 水平阴影线                */
#define HS_VERTICAL     5       /* 垂直阴影线                */
#define HS_SIEVE        6       /* 黑白点相间的筛网          */
#define HS_GRAY         6       /* 灰色效果的筛网            */
#define HS_LIGHTGRAY    7       /* 浅灰色效果的筛网          */
#define HS_WALL         8       /* 砖砌墙图                  */

/* Defines struct for log cursor */
typedef struct DrvLogCursor 
{
    int16       type;           /* cursor type              */
    int16       rop;            /* cursor rop               */
    int32       width;          /* cursor width             */
    int32       height;         /* cursor height            */
    int32       hot_x;          /* cursor hot point x       */
    int32       hot_y;          /* cursor hot point y       */
    int32       fgcolor;
    int32       bkcolor;
    const void* pANDPlane;      /* curosr mask pattern      */
    const void* pXORPlane;      /* curosr image data        */
} DRVLOGCURSOR, *PDRVLOGCURSOR;

/* Define constant for type filed */
#define CUR_BW          0       /* 单色光标                 */
#define CUR_COLOR       1       /* 彩色光标                 */
#define CUR_ANIMATE     2       /* 彩色动画光标             */

#define CUR_SHOW        1910        //光标已显示 0x0776
#define CUR_HIDE        1911        //光标正隐藏 0x0777

typedef struct DrvLogPalette
{
    int16   entries;
    int16   unused;
    RGB     color_map[1];
} DRVLOGPALETTE, *PDRVLOGPALETTE;

/* Define struct for graph drawing parameter */
typedef struct GraphParam
{
    int16       flags;          /* flags                        */
    int16       arcDirection;    /* arc direction, wwh 02/08/02  */
    int16       bk_mode;        /* background mode              */
    int32       bk_color;       /* Realized background color    */
    int32       text_color;     /* Realized text color         */    
    PPHYSPEN    pPen;           /* Pen to use                   */
    PPHYSBRUSH  pBrush;         /* Brush to use                 */
    int16       brush_org_x;    /* brush offset X               */
    int16       brush_org_y;    /* brush offset Y               */
    int32       rop;            /* ROP2 or ROP3                 */
    RECT        clip_rect;      /* Clip rectangle               */
    PREGION     pRegion;        /* Complicate clip region       */
    PBLENDFUNCTION pBlend;      /* Blending parameter */
    int32       pen_color;
    int32       brush_color;
}GRAPHPARAM, *PGRAPHPARAM;

/* Define constant for flags field in GraphPara struct */
#define GPF_PEN         0x0001  /* 使用笔pPen                   */
#define GPF_BRUSH       0x0002  /* 使用刷子pBrush               */
#define GPF_ROP2        0x0004  /* 使用ROP2                     */
#define GPF_ROP3        0x0008  /* 使用ROP3                     */
#define GPF_CLIP_RECT   0x0010  /* 使用简单裁剪区域clip_rect    */
#define GPF_CLIP_REGION 0x0020  /* 使用复杂裁剪区域pClipRegion  */

// Define constant for bk_mode field in GraphParam struct
#define BM_TRANSPARENT     1    /* 透明方式                     */
#define BM_OPAQUE          2    /* 不透明方式                   */
#define BM_NEWTRANSPARENT  3    /* 用于位图的透明贴图           */
#define BM_ALPHATRANSPARENT 4    /* 用于32位含alpha通道的透明贴图*/
#define BM_NEWALPHATRANSPARENT 5 /* 指定透明颜色,含alpha通道的透明贴图*/

/* Define constant for rop field in GraphParam struct           */
#define ROP_SET_DST_0       1   /* 全0                                  */
#define ROP_NSRC_AND_NDST   2   /* 源操作对象的非与目的操作对象的非的与 */
#define ROP_NSRC_AND_DST    3   /* 源操作对象的非与目的操作对象的与     */
#define ROP_NSRC            4   /* 源操作对象的非                       */
#define ROP_SRC_AND_NDST    5   /* 源操作对象与目的操作对象的非的与     */
#define ROP_NDST            6   /* 目的操作对象的非                     */
#define ROP_SRC_XOR_DST     7   /* 源操作对象与目的操作对象的异或       */
#define ROP_NSRC_OR_NDST    8   /* 源操作对象的非与目的操作对象的非的或 */
#define ROP_SRC_AND_DST     9   /* 源操作对象与目的操作对象的与         */
#define ROP_NSRC_XOR_DST    10  /* 源操作对象的非与目的操作对象的异或   */
#define ROP_DST             11   /* 目的操作对象                         */
#define ROP_NSRC_OR_DST     12  /* 源操作对象的非与目的操作对象的或     */
#define ROP_SRC             13  /* 源操作对象                           */
#define ROP_SRC_OR_NDST     14  /* 源操作对象与目的操作对象的非的或     */
#define ROP_SRC_OR_DST      15  /* 源操作对象与目的操作对象的或         */
#define ROP_SET_DST_1       16  /* 全1                                  */

/* Define struct for rectangle drawing */
typedef struct RectData
{
    int32 x1;
    int32 y1;
    int32 x2;
    int32 y2;
} RECTDATA, *PRECTDATA;

/* Define struct for line drawing */
typedef struct LineData
{
    int32 x1;       /* start x  */
    int32 y1;       /* start y  */
    int32 x2;       /* end x    */
    int32 y2;       /* end y    */
} LINEDATA, *PLINEDATA;

/* Define struct for point drawing */
typedef struct PointData
{
    int32 x;
    int32 y;
    uint32 color;
} POINTDATA, *PPOINTDATA;

/* Define struct for circle drawing */
typedef struct CircleData
{
    int32 x;        /* x coordinates of center  */
    int32 y;        /* y coordinates of center  */
    int32 r;        /* length of radius         */
} CIRCLEDATA, *PCIRCLEDATA;

/* Define struct for polygon drawing */
typedef struct PolygonData
{
    int16 count;    /* number of the points     */
    int16 mode;     /* close mode               */
    POINT points[1];/* points array             */
} POLYLINEDATA, *PPOLYLINEDATA, POLYGONDATA, *PPOLYGONDATA;

/* Define struct for Ellipse drawing */
typedef struct EllipseData
{
    int32 x;        /* x coordinates of center          */
    int32 y;        /* y coordinates of center          */
    int32 d1;       /* length of x axes                 */
    int32 d2;       /* length of y axes                 */
    int32 rotate;   /* rotate degree. Unsupported now   */
} ELLIPSEDATA, *PELLIPSEDATA;

/* Define struct for arc drawing */
typedef struct ArcData
{
    int32 x;
    int32 y;
    int32 r;
    int32 mode;
    int32 start;
    int32 end;
} ARCDATA, *PARCDATA;

/* Define struct for ellipse arc drawing */
/*
typedef struct EllipseArcData
{
    int32 x;
    int32 y;
    int32 d1;
    int32 d2;
    int32 rotate;
    int32 mode;
    int32 start;
    int32 end;
} ELLIPSEARCDATA, *PELLIPSEARCDATA;
*/
typedef struct EllipseArcData
{
    int32 x;        /* x coordinates of center          */
    int32 y;        /* y coordinates of center          */
    int32 d1;       /* length of x axes                 */
    int32 d2;       /* length of y axes                 */
    int32 mode;     /* clockwise or counter clockwise   */
    int32 rotate;   /* rotate degree                    */
    int32 start_x;  
    int32 start_y;  /* link this point to the center and intersect 
                    ** the ellipse. The result intersect point is 
                    ** the start point of the ellipse arc.
                    ** This method is as same as MS Windows.
                    */
    int32 end_x;    
    int32 end_y;    /* link this point to the center and intersect 
                    ** the ellipse. The result intersect point is 
                    ** the end point of the ellipse arc
                    ** This method is as same as MS Windows.
                    */
} ELLIPSEARCDATA, *PELLIPSEARCDATA;

/* Define constant for mode field in ArcData and EllipsArcData struct */
#define AM_ARC      0       /* 不闭合，成为弧   */
#define AM_SECTOR   1       /* 闭合成扇形       */
#define AM_LUNE     2       /* 闭合成弓形       */

#define AD_COUNTERCLOCKWISE 1
#define AD_CLOCKWISE        2

typedef struct tagROUNDRECTDATA
{
    int32 x1;       /* The left of boundary rect    */
    int32 y1;       /* The top of boundary rect     */
    int32 x2;       /* The right of boundary rect   */
    int32 y2;       /* The bottom of boundary rect  */
    int32 width;    /* The width of the ellipse arc */
    int32 height;   /* The height of ellipse arc    */
} ROUNDRECTDATA, *PROUNDRECT;

typedef struct PolyPoint
{
    uint32 number;
    PPOINT pPoint;
}POLYPOINTDATA, *PPOLYPOINTDATA; 

typedef struct PolyPolyLine
{
    POINT  *pPoints; // address of an array of points 
    uint32 *pPolyPoints; // address of an array of values 
    uint32 count; 
}POLYPOLYLINEDATA, *PPOLYPOLYLINEDATA; 

/* field selection bits */
#define DM_BITSPERPEL       0x00040000L
#define DM_PELSWIDTH        0x00080000L
#define DM_PELSHEIGHT       0x00100000L
#define DM_DISPLAYFLAGS     0x00200000L
#define DM_DISPLAYFREQUENCY 0x00400000L

typedef struct _dispmode 
{
    uint32  dmBitsPerPel;
    uint32  dmPelsWidth;
    uint32  dmPelsHeight;
    uint32  dmDisplayFlags;
    uint32  dmDisplayFrequency;
} DISPMODE, *PDISPMODE;

/* Flags for ChangeDisplaySettings */

/* Return values for ChangeDisplaySettings */
#define DISP_CHANGE_SUCCESSFUL       0
#define DISP_CHANGE_RESTART          1
#define DISP_CHANGE_FAILED          -1
#define DISP_CHANGE_BADMODE         -2
#define DISP_CHANGE_NOTUPDATED      -3
#define DISP_CHANGE_BADFLAGS        -4
#define DISP_CHANGE_BADPARAM        -5

/* Define struct for display driver infomation */
typedef struct DisplayDrvInfo
{
    int32 devices;
    int32 func_mask1;
    int32 func_mask2;
    int   (*OpenDevice)(PDEV pDev, int dev);
    int   (*CloseDevice)(PDEV pDev);
    int   (*SuspendDevice)(PDEV pDev);
    int   (*ResumeDevice)(PDEV pDev);
    int   (*GetDevInfo)(PDEV pDev, PDISPLAYDEVINFO pInfo);
    int   (*EnumMode)(PDEV pDev, int nModeNum, PDISPMODE pDispMode);
    int   (*ChangeMode)(PDEV pDev, PDISPMODE pDispMode, uint32 flags);
    int   (*CreateMemoryDevice)(PDEV pDev, int width, int height,
                                PMEMDEV pMemDev);
    int   (*DestroyMemoryDevice)(PDEV pMemDev);
    int   (*CreateBitmap)(PDEV pDev, int width, int height, 
                          PDDBITMAP pBitmap);
    int   (*DestroyBitmap)(PDEV pDev, PDDBITMAP pBitmap);
    int   (*SetBitmapBuffer)(PDEV pDev, PDDBITMAP pBitmap, void* pBuffer);
    int   (*RealizePen)(PDEV pDev, PDRVLOGPEN pLogPen, PPHYSPEN pPhysPen);
    int   (*UnrealizePen)(PDEV pDev, PPHYSPEN pPen);
    int   (*RealizeBrush)(PDEV pDev, PDRVLOGBRUSH pLogBrush, 
                                    PPHYSBRUSH pPhysBrush);
    int   (*UnrealizeBrush)(PDEV pDev, PPHYSBRUSH pBrush);
    int   (*RealizePalette)(PDEV pDev, PDRVLOGPALETTE pLogPalette,
                            PPHYSPALETTE pPhysPalette);
    int   (*UnrealizePalette)(PDEV pDev, PPHYSPALETTE pPalette);
    int   (*RealizeColor)(PDEV pDev, PPHYSPALETTE pPalette, int32 color);
    int   (*SelectPalette)(PDEV pDev, PPHYSPALETTE pPalette);
    int   (*RealizeCursor)(PDEV pDev, PDRVLOGCURSOR pLogCursor,
                           PPHYSCURSOR pPhysCursor);
    int   (*UnrealizeCursor)(PDEV pDev, PPHYSCURSOR pCursor);
    int   (*ShowCursor)(PDEV pDev, PPHYSCURSOR pCursor, int mode);
    int   (*SetCursorPos)(PDEV pDev, int x, int y);
    int   (*CheckCursor)(PDEV pDev);
    int   (*UpdateScreen)(PDEV pDev);
    int   (*DrawGraph)(PDEV pDev, int func, PGRAPHPARAM pGraphPara, 
                       void* pDrawData);
    int   (*BitBlt)(PDEV pDev, PRECT pDstRect, PDEV pSrcDev,
                    int x, int y, PGRAPHPARAM pGraphParam);
    int   (*PatBlt)(PDEV pDev, PRECT pRect, PGRAPHPARAM pGraphParam);
    int   (*DIBitBlt)(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                      int x, int y, PGRAPHPARAM pGraphParam);
    int   (*TextBlt)(PDEV pDev, int x, int y, PTEXTBMP pTextBmp, 
                     PGRAPHPARAM pGraphParam);
    int   (*StretchBlt)(PDEV pDev, PRECT pDstRect, PDEV pSrcDev, 
                        PRECT pSrcRect, PGRAPHPARAM pGraphParam);
    int   (*DIStretchBlt)(PDEV pDev, PRECT pDstRect, PDIBITMAP pBitMap, 
                        PRECT pSrcRect, PGRAPHPARAM pGraphParam);
    int   (*SaveScreen)(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect);
    int   (*RestoreScreen)(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect);
    int   (*ScrollScreen)(PDEV pDev, PRECT pRect, PRECT pClipRect, 
                          int x, int y);
} DISPLAYDRVINFO, *PDISPLAYDRVINFO;

/* Define constant for function mask in DisplayDrvInfo struct */
#define FM_SUSPENDDEVICE    0x00000001  /* 有SuspendDevice函数  */
#define FM_RESUMEDEVICE     0x00000002  /* 有ResumeDevice函数   */
#define FM_CHECKCURSOR      0x00000004  /* 有CheckCursor函数    */
#define FM_UPDATESCREEN     0x00000008  /* 有UpdateScreen 函数  */
#define FM_STRETCHBLT       0x00000010  /* 有StretchBlt函数     */
#define FM_SCROLLSCREEN     0x00000020  /* 有ScrollScreen函数   */

/* Define constant for cursor type */
#define CUS_BW          0       /* 单色光标                 */
#define CUS_COLOR       1       /* 彩色光标(设备相关)       */
#define CUS_ANIMATE     2       /* 彩色动画光标             */

/* Define constant for func parameter of DrawGraph function */
#define DG_POINT            0           /* 画点                 */
#define DG_LINE             1           /* 画线                 */
#define DG_RECT             2           /* 画矩形               */
#define DG_CIRCLE           3           /* 画圆                 */
#define DG_POLYGON          4           /* 画多边形             */
#define DG_ELLIPSE          5           /* 画椭圆               */
#define DG_ARC              6           /* 画弧                 */
#define DG_ELLIPSEARC       7           /* 画椭圆弧             */

#define DG_ROUNDRECT        8           /* 画圆角矩形           */
#define DG_POLYPOINT        9           /* 画多个点             */
#define DG_POLYPOLYLINE     10          /* 画多个点             */

#define DG_SETPIXEL         100         /* 设置象素值 */
#define DG_GETPIXEL         101         /* 得到象素值 */
/* The entry function for display driver */
PDISPLAYDRVINFO EnableDisplayDev(PDISPLAYDRVINFO pDisplayDrvInfo);

/* the display mode define */
#include "disppdd.h"
#endif // __DISPLAY_H
