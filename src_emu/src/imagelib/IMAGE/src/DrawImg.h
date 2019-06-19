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
#define MAX_GIFNUM 32                    /*  ������ʾ�Ķ���GIF�� */
#define UPDATESCREEN    OS_UpdateScreen(0, 0, 640, 480) 

struct Bmp
{
    BITMAPINFO          m_bmpinfo;      /*     λͼ�ṹ��Ϣ        */
    RGBQUAD	            m_colors[256];	/*     ��ɫ��              */
};

struct data_buffer
{
	DWORD	            data_count;      /*      ���ݴ�С            */
    int                 nMapFile;
	char                * data;          /*      ���ݵ�ַ            */
};

typedef void* PPHYIMG;

typedef struct
{
    HANDLE    handle;
	PPHYIMG   pPhyImg;
} IMGOBJ, *PIMGOBJ;

/* ����GIF�����ݽṹ */
typedef struct
{
    PGIFOBJ                 pGif;        /*     GIF ����               */
    struct gif_frame 		frame;       /*     ֡��Ϣ                 */
    BOOL                    bRepaint;    /*     �ػ���־               */
    int                     nframes;     /*     ��ͼ���ܵ�֡��         */
    int                     x;           /* ͼ��ĵ�һ֡���ھ��ε�X���� */
    int                     y;           /* ͼ��ĵ�һ֡���ھ��ε�Y���� */
	int						width;
	int						height;
    int                     DstWidth;
    int                     DstHeight;
    int                     LastMethod;  /*     ͼ����ʧ�ķ�ʽ          */
    int                     LastX;       /*     ��һ֡ͼ���X����       */
    int                     LastY;       /*     ��һ֡ͼ���Y����       */
    int                     LastWidth;   /*     ��һ֡ͼ��Ŀ��        */
    int                     LastHeight;  /*     ��һ֡ͼ��ĸ߶�        */
    int                     dm;          /*     ͼ����ʾ��ʽ            */
    int                     Times;       /*     ָ����ͼ����ʾ����      */
    int                     Count;       /*     ͼ��ʵ����ʾ�Ĵ���      */
    void *                  pHead;       /*     ͼ��ĵ�һ֡�ĵ�ַ      */
    void *                  pCur;        /*     ͼ��ǰ��ʾ��֡��      */
    unsigned long           m_timer;     /*     ͼ��Ķ�ʱ����ʶ��      */
    struct gif_datasource   datasource;  /*     ͼ�������Դ            */
    int                     nMapFile;    /* ͼ�������Ƿ��Ǵ��ļ�ӳ�����*/
    HWND                    hwndParent;  /*     ��ʾͼ��Ĵ���          */
	HDC						hCompatibleDC;
    HBITMAP                 hCompatibleBmp;
	PAINTBKFUNC				pfnPaintBkFunc;
}GIFWNDDATA, *PGIFWNDDATA;

/* DM_DATA��ʽ��ʾ����GIFʱ��֡�����ݽṹ */
typedef struct tagDATA
{
    void *              pData;           /*     ���ݵ�ַ                */
    struct gif_frame    frame;           /*     ֡��Ϣ                  */
    struct Bmp          bmp;             /*     ֡��Ӧ��λͼ��Ϣ        */  
    RECT                rect;            /*  �ָ�ԭͼ��ʱ��Ҫˢ�µ����� */
    BOOL                bRect;           /*     ��Ҫ�ָ�ԭͼ��ı�־    */
    struct tagDATA      *pNext;          /*     ��һ֡��ַ              */
}HEADOFDATA, *PHEADOFDATA;

/* DM_BITMAP��ʽ��ʾ����GIFʱ��֡�����ݽṹ */
typedef struct tagHEADBITMAP
{
    HBITMAP                 hBitmap;     /*     ֡��Ӧ��λͼ���        */
    struct gif_frame        frame;       /*     ֡��Ϣ                  */
    void *                  pData;       /*     ���ݵ�ַ                */
    int                     clrIndex;       /*     ͸����ʾ�ı�����ɫ      */
    RECT                    rect;        /*  �ָ�ԭͼ��ʱ��Ҫˢ�µ����� */
    BOOL                    bRect;       /*     ��Ҫ�ָ�ԭͼ��ı�־    */
    struct tagHEADBITMAP    *pNext;      /*     ��һ֡��ַ              */
}HEADOFBITMAP, *PHEADOFBITMAP;

/* Gif decoder state */
enum dgif_state {
			S_INIT,                       /*     ��ʼ��GIF              */
			S_READHEAD,                   /*     ��GIFͷ��Ϣ            */
			S_READFRAMEHEADER,            /*     ��GIF֡ͷ��Ϣ          */	   
			S_READSCANLINES,              /*     ��֡��Ϣ               */
			S_CONSUMEINPUT,               /*     ������������           */
			S_DATAEND,                    /*     ������ʾ               */
			S_ERROR                       /*     ����״̬               */
};

enum color_tbl { GLOBAL_COLORTABLE = 1, FRAME_COLORTABLE };


/***********************************************************************/
/***************   GIF ����    *****************************************/
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
/*****************    JPEG ����  ***************************************/
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
/*****************    PNG ����   ***************************************/
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
/*****************    WBMP ����   ***************************************/
/***********************************************************************/
unsigned int DecodeMultiByteInt(char **src);
void DrawWBMP(HDC hdc, int x, int y, int width, int height,
                   struct data_buffer pData, DWORD dwRop);
HBITMAP  CreateBitmapFromWbmp(struct data_buffer pData);

/***********************************************************************/
/*****************    ���� ����   ***************************************/
/***********************************************************************/
void    ConvertImageData(DWORD dwData, BYTE* pBaseAddress, int hFile);
HBITMAP  ConvertBitmapEx(HDC hdc, HBITMAP hBmp, int nSrcX, int nSrcY, 
                      int nWidth, int nHeight, int nAngle, SIZE* pSize);
BOOL    BMPToGifFile(int hGif, char *ImageBuffer);
BOOL    BMPToPngFile(int hPng, char *ImageBuffer);
BOOL    RGBToJpegFile(char* fileName, BYTE *dataBuf, UINT widthPix, UINT height);

#endif
