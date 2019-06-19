/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Image Lib
 *
 * Purpose  : defines for supporting ImageLib
 *            
\**************************************************************************/

#ifndef  _DRAWIMG_H
#define  _DRAWIMG_H

#include "hpimage.h"
#include "gif/dgif.h"

#ifndef NOJPEG
#include "jpeg/djpeg.h"
#endif //NOJPEG
  
#define GET16(p)(WORD)((((WORD)*((BYTE*)(p) + 1)) << 8) + *((BYTE*)(p)))
#define GET32(p)(DWORD)((((DWORD)*((BYTE*)(p) + 3)) << 24) + \
                        (((DWORD)*((BYTE*)(p) + 2)) << 16)  + \
                        (((DWORD)*((BYTE*)(p) + 1)) << 8) +  *((BYTE*)(p)))  
#define MAX_GIFNUM 32                    /*  最大可显示的动画GIF数 */
#define UPDATESCREEN    OS_UpdateScreen(0, 0, 640, 480) 

struct Bmp
{
    BITMAPINFO          m_bmpinfo;      /*     位图结构信息        */
    RGBQUAD	            m_colors[256];	/*     颜色表              */
};

struct data_buffer
{
	DWORD	            data_count;      /*      数据大小            */
    int                 nMapFile;
	char                * data;          /*      数据地址            */
};

typedef void* PPHYIMG;

typedef struct
{
    HANDLE    handle;
	PPHYIMG   pPhyImg;
} IMGOBJ, *PIMGOBJ;

/* 动画GIF的数据结构 */
typedef struct
{
    PGIFOBJ                 pGif;        /*     GIF 对象               */
    struct gif_frame 		frame;       /*     帧信息                 */
    BOOL                    bRepaint;    /*     重画标志               */
    int                     nframes;     /*     该图象总的帧数         */
    int                     x;           /* 图象的第一帧所在矩形的X坐标 */
    int                     y;           /* 图象的第一帧所在矩形的Y坐标 */
	int						width;
	int						height;
    int                     DstWidth;
    int                     DstHeight;
    int                     LastMethod;  /*     图象消失的方式          */
    int                     LastX;       /*     上一帧图象的X坐标       */
    int                     LastY;       /*     上一帧图象的Y坐标       */
    int                     LastWidth;   /*     上一帧图象的宽度        */
    int                     LastHeight;  /*     上一帧图象的高度        */
    int                     dm;          /*     图象显示方式            */
    int                     Times;       /*     指定的图象显示次数      */
    int                     Count;       /*     图象实际显示的次数      */
    void *                  pHead;       /*     图象的第一帧的地址      */
    void *                  pCur;        /*     图象当前显示的帧数      */
    unsigned long           m_timer;     /*     图象的定时器标识符      */
    struct gif_datasource   datasource;  /*     图象的数据源            */
    int                     nMapFile;    /* 图象数据是否是从文件映射而来*/
    HWND                    hwndParent;  /*     显示图象的窗口          */
	HDC						hCompatibleDC;
    HBITMAP                 hCompatibleBmp;
	PAINTBKFUNC				pfnPaintBkFunc;
}GIFWNDDATA, *PGIFWNDDATA;

/* DM_DATA方式显示动画GIF时各帧的数据结构 */
typedef struct tagDATA
{
    void *              pData;           /*     数据地址                */
    struct gif_frame    frame;           /*     帧信息                  */
    struct Bmp          bmp;             /*     帧对应的位图信息        */  
    RECT                rect;            /*  恢复原图象时需要刷新的区域 */
    BOOL                bRect;           /*     需要恢复原图象的标志    */
    struct tagDATA      *pNext;          /*     下一帧地址              */
}HEADOFDATA, *PHEADOFDATA;

/* DM_BITMAP方式显示动画GIF时各帧的数据结构 */
typedef struct tagHEADBITMAP
{
    HBITMAP                 hBitmap;     /*     帧对应的位图句柄        */
    struct gif_frame        frame;       /*     帧信息                  */
    void *                  pData;       /*     数据地址                */
    int                     clrIndex;       /*     透明显示的背景颜色      */
    RECT                    rect;        /*  恢复原图象时需要刷新的区域 */
    BOOL                    bRect;       /*     需要恢复原图象的标志    */
    struct tagHEADBITMAP    *pNext;      /*     下一帧地址              */
}HEADOFBITMAP, *PHEADOFBITMAP;

/* Gif decoder state */
enum dgif_state {
			S_INIT,                       /*     初始化GIF              */
			S_READHEAD,                   /*     读GIF头信息            */
			S_READFRAMEHEADER,            /*     读GIF帧头信息          */	   
			S_READSCANLINES,              /*     读帧信息               */
			S_CONSUMEINPUT,               /*     继续读入数据           */
			S_DATAEND,                    /*     结束显示               */
			S_ERROR                       /*     出错状态               */
};

enum color_tbl { GLOBAL_COLORTABLE = 1, FRAME_COLORTABLE };


/***********************************************************************/
/***************   GIF 函数    *****************************************/
/***********************************************************************/

void  GIF_DS_InitSource( PGIFOBJ pGif);
int   GIF_DS_GetData( PGIFOBJ pGif);
void  GIF_DS_SkipData( PGIFOBJ pGif, int num_bytes);
void  GIF_DS_TermSource( PGIFOBJ pGif);

void  CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);

PPHYIMG  CreateGifFromData(struct data_buffer pData, int hFile);
BOOL     DeleteGif(PPHYIMG pImg, BOOL bMapFile);
HBITMAP  CreateBitmapFromGif(HDC hdc, struct data_buffer pData, 
                             PCOLORREF pColor, BOOL *pbTran);
int      PaintStaticGif(HDC hdc, int nDestX, int nDestY, int nWidth,
                           int nHeight, PPHYIMG pPhyImg, int nSrcX, int nSrcY,
                           DWORD dwRop);
HGIFANIMATE DrawGif(HWND hWnd, int x, int y, int width, int height, struct data_buffer pData, 
                    int nType, int dm, int Times);
BOOL        DrawStaticGif (HDC hdc, int x, int y, int width, int height,
                   struct data_buffer pData, DWORD dwRop);
void    SetBitmapInfo( PGIFWNDDATA pGifWndData);
int     SeekToStart(PGIFWNDDATA pGifWndData);
int     PaintGifImage(HDC hdc, PGIFWNDDATA pGifWndData, BOOL bPaint, BOOL bTimer);

BOOL    GetGifDimensionFromData(struct data_buffer pData, PSIZE pSize);
BOOL    GetGifDimension(PPHYIMG pPhyImg, PSIZE pSize);

BOOL    GIF_IsGIFDynamic(void* pGIFData, DWORD dwDataSize);

/***********************************************************************/
/*****************    JPEG 函数  ***************************************/
/***********************************************************************/

#ifndef NOJPEG
void     JPEG_DS_InitSource( PJPEGOBJ pJpeg);
void     JPEG_DS_SkipData( PJPEGOBJ pJpeg, int num_bytes);
void     JPEG_DS_TermSource( PJPEGOBJ pJpeg);

PPHYIMG  CreateJpegFromData(struct data_buffer pData, int hFile);
HBITMAP  CreateBitmapFromJpeg(struct data_buffer pData);
BOOL     DeleteJpeg(PPHYIMG pImg, BOOL bMapFile);
int      PaintJpegImage( HDC hdc, int x, int y, int width, int height,
                   DWORD dwRop);
int      PaintJpeg(HDC hdc, int nDestX, int nDestY, int nWidth, int nHeight,
              PPHYIMG pPhyImg, int nSrcX, int nSrcY, DWORD dwRop);
BOOL     DrawJpeg( HDC hdc, int x, int y, int width, int height,
              struct data_buffer pData, DWORD dwRop);
BOOL     GetJpegDimensionFromData(struct data_buffer pData, PSIZE pSize);
BOOL     GetJpegDimension(PPHYIMG pPhyImg, PSIZE pSize);
#endif //NOJPEG


/***********************************************************************/
/*****************    PNG 函数   ***************************************/
/***********************************************************************/
typedef struct tagPNGDIB {
	BYTE* data;
	char* io_str;
	int nWidth;
	int nHeight;
	int nChannel;
	int nBitDepth;
	int nColorType;
} PNGDIB, *PPNGDIB;

PPHYIMG    CreatePngFromData(struct data_buffer pData, int hFile);
HBITMAP    CreateBitmapFromPNG(PCSTR pszImgFile);
BOOL       DeletePng(PPHYIMG pImg, BOOL bMapFile);
BOOL       GetPngDimension(PPHYIMG pImg, PSIZE pSize);
BOOL       GetPngDimensionFromData(PCSTR pImgData, PSIZE pSize);
int        PaintPng(HDC hdc, int nDestX, int nDestY, int nWidth, int nHeight,
                    PPHYIMG pPhyImg, int nSrcX, int nSrcY, DWORD dwRop);
BOOL       DrawPNGFromFile(HDC hdc, PCSTR pszImgFile, int x, int y,
		            	   int width, int height, DWORD dwRop);
BOOL       DrawPNGFromData(HDC hdc, PCSTR pPngData, int x, int y, 
		   			       int width, int height, DWORD dwRop);

/***********************************************************************/
/*****************    WBMP 函数   ***************************************/
/***********************************************************************/
unsigned int DecodeMultiByteInt(char **src);
void DrawWBMP(HDC hdc, int x, int y, int width, int height,
                   struct data_buffer pData, DWORD dwRop);
HBITMAP  CreateBitmapFromWbmp(struct data_buffer pData);

/***********************************************************************/
/*****************    其他 函数   ***************************************/
/***********************************************************************/
void    ConvertImageData(DWORD dwData, BYTE* pBaseAddress, int hFile);
HBITMAP  ConvertBitmapEx(HDC hdc, HBITMAP hBmp, int nSrcX, int nSrcY, 
                      int nWidth, int nHeight, int nAngle, SIZE* pSize);
BOOL    BMPToGifFile(int hGif, char *ImageBuffer);
BOOL    BMPToPngFile(int hPng, char *ImageBuffer);
BOOL    RGBToJpegFile(char* fileName, BYTE *dataBuf, UINT widthPix, UINT height);

#endif
