/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display Driver  
 *
 * Purpose  : Internal head file of display driver.
 *            
\**************************************************************************/

#ifndef __HPDISP_H
#define __HPDISP_H

#define SLOPELINE_SUPPORTED
//#define CIRCLE_SUPPORTED
#define POLYGON_SUPPORTED
#define ELLIPSE_SUPPORTED
#define ELLIPSEARC_SUPPORTED
#define ROUNDRECT_SUPPORTED
#define ARC_SUPPORTED

/* polygon nees slopline */
#ifndef SLOPELINE_SUPPORTED
#undef POLYGON_SUPPORTED
#endif

/* circle arc need ellipse arc 
** roundrect need ellipse arc
*/
#ifndef ELLIPSEARC_SUPPORTED
#undef ARC_SUPPORTED
#endif

typedef struct PhysPen 
{
    int16   style;
    int16   end_style;  /* Pen end style    */
    int16   width;      /* pen width        */
    int16   frame_mode; /* Pen frame mode   */
    int16   pat_width;  /* pattern width    */
    int16   pat_height; /* pattern height   */
    int16   ge_handle;  /* Handle in hardware */
    int16   unused;
    int32   fgcolor;    /* Pen forground color */
    int32   bkcolor;    /* pen background color */
} PHYSPEN;

typedef struct PhysBrush {
    int16   style;
    int16   hatch_style;
    int16   width;
    int16   height;
    int16   ge_handle;  /* Handle in hardware */
    int16   unused;
    int32   fgcolor;
    int32   bkcolor;
    uint8 * pPattern;
} PHYSBRUSH;

typedef struct rgb_quad {
    unsigned char B;
    unsigned char G;
    unsigned char R;
    unsigned char unused;
} RGBQUAD;

typedef struct PhysPalette {
    struct rgb_quad color[1];
} PHYSPALETTE;

typedef struct PhysCursor {
    int16   handle;
    int16   type;   /* type of this cursor */
    int16   rop;    /* cursor ROP */
    int16   width;
    int16   height;
    int16   hot_x;
    int16   hot_y;
    int8 *  pMask;
    int8 *  pData;
    int32   fgcolor;
    int32   bkcolor;
} PHYSCURSOR;

typedef struct ScreenSave {
    short   x1;         /* X value of upper left */
    short   y1;         /* Y value of upper left */
    short   width;      /* width of save area */
    short   height;     /* height of save area */
    short   pitch;      /* bytes per scan line */
    short   unused;     /* unused */
    int data[1];    /* pointer to data */
} SCREENSAVE;

typedef struct PhysDev {
    int16   mode;       /* type of this device */
    int16   bmpmode;    /* Bitmap mode of this device */
    int16   width;      /* Width of this device */
    int16   height;     /* height of this device */
    int16   line_bytes; /* bytes per scan line */
    int16   pix_bits;   /* bytes per pixel */
    int8    bUpdate;    /* update screen flags */
    int8    device_no;  /* the device no */
    int8    bAccelerate;/* Hardware accelerate */
    int8    nReserved1; /* reserved */
    RECT    rcUpdate;   /* the update rect */
    PDISPLAYDRVINFO pDDI; /* display driver interface */
    uint8 * pdata;      /* pointer to data buffer */
} DISPLAYDEV, *PDISPLAYDEV;

#define DEV_PHYSICAL    0
#define DEV_MEMORY      1
#define DEV_BITMAP      2

typedef struct MemDev {
    int16   mode;       /* type of this device */
    int16   bmpmode;    /* Bitmap mode of this device */
    int16   width;      /* Width of this device */
    int16   height;     /* height of this device */
    int16   line_bytes; /* bytes per scan line */
    int16   pix_bits;   /* bytes per pixel */
    int8    bUpdate;    /* update screen flags */
    int8    device_no;  /* the device no */
    int8    bAccelerate;/* Hardware accelerate */
    int8    nReserved1; /* reserved */
    RECT    rcUpdate;   /* the update rect */
    PDISPLAYDRVINFO pDDI; /* display driver interface */
    uint8 * pdata;      /* pointer to data buffer */
} MEMDEV;

#define PHYSBMP     MEMDEV
#define DDBITMAP    MEMDEV
#define PDDBITMAP   PMEMDEV

#define MAX_MEMDEVWIDTH     4096
#define MAX_MEMDEVHEIGHT    4096
#define MIN_MEMDEVWIDTH     1
#define MIN_MEMDEVHEIGHT    0

typedef struct {
    int type;       /* control variable                                 */
    int start_x;    /* start point of X                                 */
    int start_y;    /* start point of Y                                 */
    int end_x;      /* end value of X                                   */
    int end_y;      /* end value of Y                                   */
    int dir;        /* direction of Y or X                              */
    int delta;      /* delta value                                      */
    int step;       /* step value                                       */
    int remain;     /* remainder                                        */
    int cliped;     /* the points have been cliped of the origin line   */
} LINEDRAW;

#define LINE_NONE       0   /* No line to draw */
#define LINE_HORIZONTAL 1   /* Horizontal line */
#define LINE_VERTICAL   2   /* Vertical line */
#define LINE_CTRLX      3   /* control variable is X */
#define LINE_CTRLY      4   /* control variable is Y */

//the direction of line and rectangle
#define NORM_DIR         1
#define REVR_DIR         -1

//int     memcmp(const void *, const void *, int);
//void *  memcpy(void *, const void *, int);
//void *  memset(void *, int, int);

//define some math operation
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define ABS(a)      ((((a) < 0) ? -(a) : (a)))

#define EXCHANGE(a, b) {int t; t = (a); (a) = (b); (b) = t;}
#define EXCHANGEINT32(a, b) {int32 t; t = (a); (a) = (b); (b) = t;}
#define EXCHANGEINT16(a, b) {int16 t; t = (a); (a) = (b); (b) = t;}

//#define DIV_CEILING(a, b) ((a) + ((b) - 1)) / (b)
#define DIV_CEILING(a, b) (((a) * 2 + (b)) / ((b) * 2))

//define rect operation
#define IsValidRect(pRC) ((pRC)->x1 < (pRC)->x2 && (pRC)->y1 < (pRC)->y2)

#define SetRect(pRC, l, t, r, b)      { \
        (pRC)->x1 = (l); \
        (pRC)->y1 = (t); \
        (pRC)->x2 = (r); \
        (pRC)->y2 = (b); \
    }

#define NormalizeRect(pRC)  {   \
        int tmp;    \
        if ((pRC)->x1 > (pRC)->x2) \
        {   \
            tmp = (pRC)->x1;  \
            (pRC)->x1 = (pRC)->x2 + 1; \
            (pRC)->x2 = tmp + 1; \
        }   \
        if ((pRC)->y1 > (pRC)->y2) \
        {   \
            tmp = (pRC)->y1;   \
            (pRC)->y1 = (pRC)->y2 + 1; \
            (pRC)->y2 = tmp + 1;    \
        }   \
    }

#define CopyRect(pRC1, pRC2)  { \
        (pRC1)->x1 = (pRC2)->x1; \
        (pRC1)->y1 = (pRC2)->y1; \
        (pRC1)->x2 = (pRC2)->x2; \
        (pRC1)->y2 = (pRC2)->y2; \
    }

#define OffsetRect(pRC1, dx, dy) { \
        (pRC1)->x1 += (dx);   \
        (pRC1)->y1 += (dy);   \
        (pRC1)->x2 += (dx);   \
        (pRC1)->y2 += (dy);   \
    }

#define IntersectRect(pRC1, pRC2)   ( \
        (pRC1)->x1 < (pRC2)->x2 && \
        (pRC1)->y1 < (pRC2)->y2 && \
        (pRC1)->x2 > (pRC2)->x1 && \
        (pRC1)->y2 > (pRC2)->y1)

#define ClipByDev(pRC, pDev) { \
        if ((pRC)->x1 < 0) (pRC)->x1 = 0;   \
        if ((pRC)->y1 < 0) (pRC)->y1 = 0;   \
        if ((pRC)->x2 > (pDev)->width)      \
            (pRC)->x2 = (pDev)->width;  \
        if ((pRC)->y2 > (pDev)->height)     \
            (pRC)->y2 = (pDev)->height; \
    }

#define GetDevRect(pRC, pDev) { \
        (pRC)->x1 = (pRC)->y1 = 0; \
        (pRC)->x2 = (pDev)->width; \
        (pRC)->y2 = (pDev)->height; \
    }

#define IsPointInRect(pRC, point_x, point_y) ( \
    (point_x) >= (pRC)->x1 && \
    (point_x) <  (pRC)->x2 && \
    (point_y) >= (pRC)->y1 && \
    (point_y) <  (pRC)->y2 )

static int ClipRect(PRECT pRC1, PRECT pRC2)
{
    if ((pRC2)->x1 > (pRC1)->x1) 
        (pRC1)->x1 = (pRC2)->x1;

    if ((pRC2)->y1 > (pRC1)->y1) 
        (pRC1)->y1 = (pRC2)->y1;

    if ((pRC2)->x2 < (pRC1)->x2) 
        (pRC1)->x2 = (pRC2)->x2;

    if ((pRC2)->y2 < (pRC1)->y2) 
        (pRC1)->y2 = (pRC2)->y2;

    return IsValidRect(pRC1);
}

static int MergeRect(PRECT pRC1, PRECT pRC2)
{
    if ((pRC2)->x1 < (pRC1)->x1) 
        (pRC1)->x1 = (pRC2)->x1;

    if ((pRC2)->y1 < (pRC1)->y1) 
        (pRC1)->y1 = (pRC2)->y1;

    if ((pRC2)->x2 > (pRC1)->x2) 
        (pRC1)->x2 = (pRC2)->x2;

    if ((pRC2)->y2 > (pRC1)->y2) 
        (pRC1)->y2 = (pRC2)->y2;

    return IsValidRect(pRC1);
}

//判断x1和x2都在rcClip的同一边外测
#define IsXOutOfRect(rcClip, xstart, xend) ( \
    ((xstart) <  (rcClip).x1 && (xend) <  (rcClip).x1) || \
    ((xstart) >= (rcClip).x2 && (xend) >= (rcClip).x2))

//判断y坐标是否在剪裁区域内
#define IsPtInYClip(rcClip, point_y) \
    ((point_y) >= (rcClip).y1 && (point_y) < (rcClip).y2)

//判断x坐标是否在剪裁区域内
#define IsPtInXClip(rcClip, point_x) \
    ((point_x) >= (rcClip).x1 && (point_x) < (rcClip).x2)

#define MergeUpdate(pDev, ix1, iy1, ix2, iy2) {   \
        if ((ix1) < (pDev)->rcUpdate.x1)         \
            (pDev)->rcUpdate.x1 = (ix1);         \
        if ((iy1) < (pDev)->rcUpdate.y1)         \
            (pDev)->rcUpdate.y1 = (iy1);        \
        if ((ix2) > (pDev)->rcUpdate.x2)         \
            (pDev)->rcUpdate.x2 = (ix2);         \
        if ((iy2) > (pDev)->rcUpdate.y2)         \
            (pDev)->rcUpdate.y2 = (iy2);         \
}

#define MAKERGB(r, g, b) (r | ((uint32)(g) << 8) | ((uint32)(b) << 16))
/* DDM_RGB16_555 */
#define Color24To15(c24)    ((((c24) & 0xf80000L) >> 19) \
                         |   (((c24) & 0x00f800L) >>  6) \
                         |   (((c24) & 0x0000f8L) <<  7))
#define REALIZECOLOR15(r, g, b) \
    ((((b) & 0xf8) >> 3) | (((g) & 0xf8) << 2) | (((r) & 0xf8) << 7))

#define REALIZECOLOR15TO24(s)   \
    (((((s) >>  7) & 0x0000f8L) | (((s) >> 12)            )) \
   | ((((s) <<  6) & 0x00f800L) | (((s) <<  1) & 0x000700L)) \
   | ((((s) << 19) & 0xf80000L) | (((s) << 14) & 0x070000L)))

/* DDM_RGB16_565 */
#define Color24To16(c24)    ((((c24) & 0xf80000L) >> 19) \
                         |   (((c24) & 0x00fc00L) >>  5) \
                         |   (((c24) & 0x0000f8L) <<  8) )
#define REALIZECOLOR16(r, g, b) \
    ((((b) & 0xf8) >> 3) | (((g) & 0xfc) << 3) | (((r) & 0xf8) << 8))

#define REALIZECOLOR16TO24(s) \
    (((((s) >>  8) & 0x0000f8L) | (((s) >> 13)            )) \
   | ((((s) <<  5) & 0x00fc00L) | (((s) >>  1) & 0x000300L)) \
   | ((((s) << 19) & 0xf80000L) | (((s) << 14) & 0x070000L)))

/* DDM_RGB12_4440 */
#define REALIZECOLOR12(r, g, b) \
    ((((r) / 17) << 12) | (((g) / 17) << 8) | (((b) / 17) << 4))
#define Color24To12(c24)   ( \
    (((((c24) & 0x0000ffL)      ) / 17) << 12) | \
    (((((c24) & 0x00ff00L) >>  8) / 17) <<  8) | \
    (((((c24) & 0xff0000L) >> 16) / 17) <<  4))
#define REALIZECOLOR12TO24(s) \
    (((((s) >>  8) & 0x0000f0L) | (((s) >> 12)            )) \
   | ((((s) <<  4) & 0x00f000L) | (((s) >>  0) & 0x000f00L)) \
   | ((((s) << 16) & 0xf00000L) | (((s) << 12) & 0x0f0000L)))

/* DDM_RGB24_BGR, DDM_RGB18_BGR 0xBGR*/
#define REALIZECOLOR24(r, g, b) (((b) << 16) | ((g) << 8) | (r))
#define Color24To24(c24)   (c24)
#define REALIZECOLOR24TO24(s) (s)

/* DDM_RGB32_???? 0x0RGB */
#define REALIZECOLOR32_0x0RGB(r, g, b) (((r) << 16) | ((g) << 8) | (b))
#define REALIZECOLOR32_0xBGR0(r, g, b) (((b) << 24) | ((g) << 16) | ((r) << 8))
#define Color24To32_0x0RGB(c24)   (    \
    ((c24) & 0x0000ff << 16) |  \
    ((c24) & 0x00ff00      ) |  \
    ((c24) & 0xff0000 >> 16))
#define Color24To32_0xBGR0(c24)  ((c24) << 8)
#define REALIZECOLOR32TO24_0x0RGB(s) \
    ((((s) & 0xff0000) >> 16) | (((s) & 0x00ff00) >> 8) | (((s) & 0x0000ff)))
#define REALIZECOLOR32TO24_0xBGR0(s) ((s) >> 8)
#define REALIZECOLOR32TO24 REALIZECOLOR32TO24_0x0RGB

//define the disp_addr operation
#define GETXYADDR(pDev, x, y) \
    ((uint8*)((pDev)->pdata) + (y) * (pDev)->line_bytes + (x) * (pDev)->pix_bits / 8)

#define GET16_W(p)      (*(WORD*)(p))
#define PUT16_W(p, w)   *((WORD*)(p)) = (WORD)(w)
#define GET16_BIG(p)    (WORD)(((*((BYTE*)(p)    )) << 8) +  \
                               ((*((BYTE*)(p) + 1))     )))
#define PUT16_BIG(p, w) {   \
    *((BYTE*)(p)    ) = (BYTE)((w) >> 8);   \
    *((BYTE*)(p) + 1) = (BYTE)((w)     );   \
}
#define GET16_LITTLE(p) ((WORD)(((*((BYTE*)(p) + 1)) << 8) +  \
                                ((*((BYTE*)(p)    ))     )))
#define PUT16_LITTLE(p, w) {   \
    *((BYTE*)(p)    ) = (BYTE)((w)     );   \
    *((BYTE*)(p) + 1) = (BYTE)((w) >> 8);   \
}

/* 0xRGB */
#define GETRBYTE16_565(c)   ((((c) & 0xF800) >> 8) | (((c)         ) >> 13))
#define GETGBYTE16_565(c)   ((((c) & 0x07E0) >> 3) | (((c) & 0x07E0) >>  9))
#define GETBBYTE16_565(c)   ((((c) & 0x001F) << 3) | (((c) & 0x001F) >>  2))
/* 0xRGB*/
#define GETRBYTE16_555(c)   ((((c) & 0x7C00) >> 7) | (((c)         ) >> 12))
#define GETGBYTE16_555(c)   ((((c) & 0x03E0) >> 2) | (((c) & 0x03E0) >>  7))
#define GETBBYTE16_555(c)   ((((c) & 0x001F) << 3) | (((c) & 0x001F) >>  2))
/* 0xRGB0 */
//#define GETRBYTE16_4440(c)         ((((c) & 0xF000) >> 12) * 17)
//#define GETGBYTE16_4440(c)         ((((c) & 0x0F00) >> 8) * 17)
//#define GETBBYTE16_4440(c)         ((((c) & 0x00F0) >> 4) * 17)
#define GETRBYTE16_4440(c)  ((((c) & 0xF000) >> 8) | (((c)         ) >> 12))
#define GETGBYTE16_4440(c)  ((((c) & 0x0F00) >> 4) | (((c) & 0x0F00) >>  8))
#define GETBBYTE16_4440(c)  ((((c) & 0x00F0) >> 0) | (((c) & 0x00F0) >>  4))
/* 0x0BGR 上层使用的RGB格式 */
#define GETRBYTE24_BGR(c)         (((c) & 0x000000FF)      )
#define GETGBYTE24_BGR(c)         (((c) & 0x0000FF00) >>  8)
#define GETBBYTE24_BGR(c)         (((c) & 0x00FF0000) >> 16)
/* 0x0RGB */
#define GETRBYTE24_RGB(c)         (((c) & 0x00FF0000) >> 16)
#define GETGBYTE24_RGB(c)         (((c) & 0x0000FF00) >>  8)
#define GETBBYTE24_RGB(c)         (((c) & 0x000000FF)      )
/* 0x0BGR 上层使用的RGB格式 */
#define GETRBYTE24_BGR(c)         (((c) & 0x000000FF)      )
#define GETGBYTE24_BGR(c)         (((c) & 0x0000FF00) >>  8)
#define GETBBYTE24_BGR(c)         (((c) & 0x00FF0000) >> 16)
/* 0x0RGB */
#define GETRBYTE24_RGB(c)         (((c) & 0x00FF0000) >> 16)
#define GETGBYTE24_RGB(c)         (((c) & 0x0000FF00) >>  8)
#define GETBBYTE24_RGB(c)         (((c) & 0x000000FF)      )
/* 0xBGRA */
#define GETABYTE32_BGRA(c)         (((c) & 0x000000FF)      )
#define GETRBYTE32_BGRA(c)         (((c) & 0x0000FF00) >>  8)
#define GETGBYTE32_BGRA(c)         (((c) & 0x00FF0000) >> 16)
#define GETBBYTE32_BGRA(c)         (((c) & 0xFF000000) >> 24)
/* 0xARGB */
#define GETABYTE32_ARGB(c)         (((c) & 0xFF000000) >> 24)
#define GETRBYTE32_ARGB(c)         (((c) & 0x00FF0000) >> 16)
#define GETGBYTE32_ARGB(c)         (((c) & 0x0000FF00) >>  8)
#define GETBBYTE32_ARGB(c)         (((c) & 0x000000FF)      )

#define GET32_DW(p)     (*(DWORD*)(p))
#define PUT32_DW(p, dw) *((DWORD*)(p)) = (DWORD)(dw)
#define GET32_BIG(p)    ((DWORD)(((*((BYTE*)(p)    )) << 24) +  \
                                 ((*((BYTE*)(p) + 1)) << 16) +  \
                                 ((*((BYTE*)(p) + 2)) <<  8) +  \
                                 ((*((BYTE*)(p) + 3))      )))
#define PUT32_BIG(p, dw)    {   \
    *((BYTE*)(p)    ) = (BYTE)((dw) >> 24);   \
    *((BYTE*)(p) + 1) = (BYTE)((dw) >> 16);   \
    *((BYTE*)(p) + 2) = (BYTE)((dw) >>  8);   \
    *((BYTE*)(p) + 3) = (BYTE)((dw)      );   \
}
#define GET32_LITTLE(p) ((DWORD)(((*((BYTE*)(p)    ))      ) +  \
                                 ((*((BYTE*)(p) + 1)) <<  8) +  \
                                 ((*((BYTE*)(p) + 2)) << 16) +  \
                                 ((*((BYTE*)(p) + 3)) << 24)))
#define PUT32_LITTLE(p, dw)    {   \
    *((BYTE*)(p)    ) = (BYTE)((dw)      );   \
    *((BYTE*)(p) + 1) = (BYTE)((dw) >>  8);   \
    *((BYTE*)(p) + 2) = (BYTE)((dw) >> 16);   \
    *((BYTE*)(p) + 3) = (BYTE)((dw) >> 24);   \

#define GET24_RGB(p)    ( \
    ((*((BYTE*)(p)    ))      ) | \
    ((*((BYTE*)(p) + 1)) <<  8) | \
    ((*((BYTE*)(p) + 2)) << 16))
#define PUT24_RGB(p, dw)    {   \
    *((BYTE*)(p)    ) = (BYTE)((dw)      ); \
    *((BYTE*)(p) + 1) = (BYTE)((dw) >>  8); \
    *((BYTE*)(p) + 2) = (BYTE)((dw) >> 16); \
}
#define GET24_BGR(p)    ( \
    ((*((BYTE*)(p) + 2))      ) | \
    ((*((BYTE*)(p) + 1)) <<  8) | \
    ((*((BYTE*)(p)    )) << 16))
#define PUT24_BGR(p, dw)    {   \
    *((BYTE*)(p) + 2) = (BYTE)((dw)      ); \
    *((BYTE*)(p) + 1) = (BYTE)((dw) >>  8); \
    *((BYTE*)(p)    ) = (BYTE)((dw) >> 16); \
}

#define PUTDI24_RGB(p, r, g, b)  \
{   \
    *((BYTE*)(p)    ) = (BYTE)(r);    \
    *((BYTE*)(p) + 1) = (BYTE)(g);    \
    *((BYTE*)(p) + 2) = (BYTE)(b);    \
}

#define PUTDI24_BGR(p, r, g, b)  \
{   \
    *((BYTE*)(p) + 2) = (BYTE)(r);    \
    *((BYTE*)(p) + 1) = (BYTE)(g);    \
    *((BYTE*)(p)    ) = (BYTE)(b);    \
}

#define PUTDI32_0RGB(p, r, g, b)  \
{   \
    *((BYTE*)(p)    ) = (BYTE)(0);    \
    *((BYTE*)(p) + 1) = (BYTE)(r);    \
    *((BYTE*)(p) + 2) = (BYTE)(g);    \
    *((BYTE*)(p) + 3) = (BYTE)(b);    \
}

#define PUTDI32_BGR0(p, r, g, b)  \
{   \
    *((BYTE*)(p) + 3) = (BYTE)(0);    \
    *((BYTE*)(p) + 2) = (BYTE)(r);    \
    *((BYTE*)(p) + 1) = (BYTE)(g);    \
    *((BYTE*)(p)    ) = (BYTE)(b);    \
}

#define GetLeftMask(shift)  (BYTE)(0xFF >> (shift))
#define GetRightMask(shift) (BYTE)(0xFF << (7 - (shift)))

#define GetLeftMask32(shift)  (DWORD)(0xFFFFFFFF >> (shift))
#define GetRightMask32(shift) (DWORD)(0xFFFFFFFF << (31 - (shift)))

#define GetLeftMask16(shift)  (WORD)(0xFFFF >> (shift))
#define GetRightMask16(shift) (WORD)(0xFFFF << (15 - (shift)))

#define REALIZECOLOR1(r, g, b) ((r) + (g) + (b) < 383 ? 0 : 1)
#define Color24To1(c24) (REALIZECOLOR1((c24) & 0xFF, (c24) & 0x00FF, (c24) & 0xFF0000L))
#define REALIZECOLOR1TO24(s) (((s) == 0) ?  0 : 0xFFFFFFL)

#define GET8(p) (*(p))
#define PUT1(p, color, mask) \
    (*(p)) = (((*(p)) & ~(mask)) | ((uint8)(color) & (mask)))

#if (BIG_ENDIAN)
#define GET12   GET16_BIG
#define PUT12   PUT16_BIG
#define GET16   GET16_BIG
#define PUT16   PUT16_BIG
#define GET32   GET32_BIG
#define PUT32   PUT32_BIG
#else
#define GET12   GET16_LITTLE
#define PUT12   PUT16_LITTLE
#define GET16   GET16_LITTLE
#define PUT16   PUT16_LITTLE
#define GET32   GET32_LITTLE
#define PUT32   PUT32_LITTLE
#endif

#define GET16_EXCHANGE(p)    (((p) & 0x02) ? \
    GET16((WORD*)(p) - 1) : GET16((WORD*)(p) + 1)
#define PUT16_EXCHANGE(p, w) (((p) & 0x02) ? \
    PUT16(((WORD*)(p) - 1), w) : PUT16(((WORD*)(p) + 1), w)

#if (LDD_MODE == DDM_RGB16_565)
#define GETRBYTE        GETRBYTE16_565
#define GETGBYTE        GETGBYTE16_565
#define GETBBYTE        GETBBYTE16_565
#define	GETPIXEL        GET16_W
#define	PUTPIXEL        PUT16_W
#define REALIZECOLOR    REALIZECOLOR16

#elif (LDD_MODE == DDM_RGB16_555)
#define GETRBYTE        GETRBYTE16_555
#define GETGBYTE        GETGBYTE16_555
#define GETBBYTE        GETBBYTE16_555
#define	GETPIXEL        GET16_W
#define	PUTPIXEL        PUT16_W
#define REALIZECOLOR    REALIZECOLOR15

#elif (LDD_MODE == DDM_RGB12_4440)
#define GETRBYTE        GETRBYTE16_4440
#define GETGBYTE        GETGBYTE16_4440
#define GETBBYTE        GETBBYTE16_4440
#define	GETPIXEL        GET16_W
#define	PUTPIXEL        PUT16_W
#define REALIZECOLOR    REALIZECOLOR12

#elif (LDD_MODE == DDM_RGB24_RGB || LDD_MODE == DDM_RGB18_RGB)
#define GETRBYTE        GETRBYTE24_BGR
#define GETGBYTE        GETGBYTE24_BGR
#define GETBBYTE        GETBBYTE24_BGR
#define	GETPIXEL        GET24_RGB
#define	PUTPIXEL        PUT24_RGB
#define REALIZECOLOR    REALIZECOLOR24

#elif (LDD_MODE == DDM_RGB24_BGR || LDD_MODE == DDM_RGB18_BGR)
#define GETRBYTE        GETRBYTE24_BGR
#define GETGBYTE        GETGBYTE24_BGR
#define GETBBYTE        GETBBYTE24_BGR
#define	GETPIXEL        GET24_BGR
#define	PUTPIXEL        PUT24_BGR
#define REALIZECOLOR    REALIZECOLOR24

#elif ((LDD_MODE == DDM_RGB32_0RGB || LDD_MODE == DDM_RGB18_0RGB) && \
      (BIG_ENDIAN)) ||    \
      ((LDD_MODE == DDM_RGB32_BGR0 || LDD_MODE == DDM_RGB18_BGR0) && \
      (!BIG_ENDIAN))
#define GETRBYTE        GETRBYTE32_ARGB
#define GETGBYTE        GETGBYTE32_ARGB
#define GETBBYTE        GETBBYTE32_ARGB
#define	GETPIXEL        GET32_DW
#define	PUTPIXEL        PUT32_DW
#define REALIZECOLOR    REALIZECOLOR32_0x0RGB
#define REALIZECOLOR32TO24 REALIZECOLOR32TO24_0x0RGB

#elif (LDD_MODE == DDM_RGB32_0RGB || LDD_MODE == DDM_RGB18_0RGB || \
       LDD_MODE == DDM_RGB32_BGR0 || LDD_MODE == DDM_RGB18_BGR0)
#define GETRBYTE        GETRBYTE32_BGRA
#define GETGBYTE        GETGBYTE32_BGRA
#define GETBBYTE        GETBBYTE32_BGRA
#define	GETPIXEL        GET32_DW
#define	PUTPIXEL        PUT32_DW
#define REALIZECOLOR    REALIZECOLOR32_0xBGR0
#define REALIZECOLOR32TO24 REALIZECOLOR32TO24_0xBGR0

#elif (LDD_MODE == DDM_MONO)
#define	GETPIXEL        GET8
#define	PUTPIXEL        PUT1
#define REALIZECOLOR    REALIZECOLOR1

#undef GET32
#undef PUT32
#define GET32   GET32_BIG
#define PUT32   PUT32_BIG    
#endif

#if (LDD_MODE == DDM_RGB24_RGB || LDD_MODE == DDM_RGB18_RGB)
#define GETRGBBYTE(p, r, g, b)  \
{                               \
    (r) = (*((BYTE*)p    ));    \
    (g) = (*((BYTE*)p + 1));    \
    (b) = (*((BYTE*)p + 2));    \
}
#define DI24_PUTPIXEL_EX    PUTDI24_RGB

#elif (LDD_MODE == DDM_RGB24_BGR || LDD_MODE == DDM_RGB18_BGR)
#define GETRGBBYTE(p, r, g, b)  \
{                               \
    (r) = (*((BYTE*)p + 2));    \
    (g) = (*((BYTE*)p + 1));    \
    (b) = (*((BYTE*)p    ));    \
}
#define DI24_PUTPIXEL_EX    PUTDI24_BGR

#elif (LDD_MODE == DDM_RGB32_BGR0 || LDD_MODE == DDM_RGB18_BGR0)
#define GETRGBBYTE(p, r, g, b)  \
{                               \
    (r) = (*((BYTE*)p + 2));    \
    (g) = (*((BYTE*)p + 1));    \
    (b) = (*((BYTE*)p    ));    \
}
#define DI24_PUTPIXEL_EX    PUTDI32_BGR0

#elif (LDD_MODE == DDM_RGB32_0RGB || LDD_MODE == DDM_RGB18_0RGB)
#define GETRGBBYTE(p, r, g, b)  \
{                               \
    (r) = (*((BYTE*)(p) + 1));    \
    (g) = (*((BYTE*)(p) + 2));    \
    (b) = (*((BYTE*)(p) + 3));    \
}

#define DI24_PUTPIXEL_EX    PUTDI32_0RGB

#else
#define GETRGBBYTE(p, r, g, b)  \
{                               \
    uint32 c = GETPIXEL(p);     \
    (r) = GETRBYTE(c);          \
    (g) = GETGBYTE(c);          \
    (b) = GETBBYTE(c);          \
}

#define DI24_PUTPIXEL_EX(p, r, g, b)  \
{   \
    uint32 c;   \
    c = REALIZECOLOR((r), (g), (b));  \
    PUTPIXEL(p, c);    \
}

#endif

/* t的取值范围是15*255 */
#define INT8_15DIVIDED(a, t)    \
(((t) = (a) + 8), t = ((((((t) >> 4) + (t)) >> 4) + (t)) >> 4))

/* t的取值范围是31*255 */
#define INT8_31DIVIDED(a, t)    \
(((t) = (a) + 16), t = ((((((t) >> 5) + (t)) >> 5))))

/* t的取值范围是63*255 */
#define INT8_63DIVIDED(a, t)    \
(((t) = (a) + 32), t = (((t) >> 6) + (t)) >> 6)

/* t的取值范围是255*255 */
#define INT8_255DIVIDED(a, t)   \
(((t) = (a) + 128), t = ((((t) >> 8) + (t)) >> 8))

#define INT_MULT15(t)   (((t) << 4) - (t))  //t * 15
#define INT_MULT31(t)   (((t) << 5) - (t))  //t * 31
#define INT_MULT63(t)   (((t) << 6) - (t))  //t * 63
#define INT_MULT255(t)  (((t) << 8) - (t))  //t * 255

#define GetPenStyle(index) (PenStyles[index])

/* Get penstyle of a pattern moveright x offset
 * dst is uint32 type
 * src = 00111111.0.0;
 * count = 8;
 * offset = 4;
 * dst = 11110011.0.0;
 * return dst
 */
#define GetOffsetPenStyle(src, count, offset) ( \
                (((src) << (offset)) | \
                ((src) >> ((count) - (offset)))) & \
                (0xFFFFFFFFL << (32 - (count))) \
)

#define RETURN(code)  {ret = code; goto exit;}
#endif //__HPDISP_H
