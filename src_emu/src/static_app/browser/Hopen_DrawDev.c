/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : abstract
 *
 * Purpose  : 实现对Hopen窗口系统设备的封装
 *            
\**************************************************************************/

#include "window.h"
#include "hpimage.h"
#include "stdlib.h"
#include "string.h"
#include "winpda.h"
#include "plx_pdaex.h"

#include "PWBE.h"
#include "UIData.h"
#include "imesys.h"
#include "wUipub.h"
#include "WIE.h"

static void*    WB_pFont = 0;
extern HBROWSER pUIBrowser;
extern HWND hwndIEApp;
extern HWND hViewWnd;
extern HDC hViewDC;

static int scroll_y = 0;

extern HFONT hViewFont;

#define WIE_INPUT_SAVE      (WM_USER+101)

#define WIE_SELOPT_CLOSE 101
#define WIE_SELOPT_OK	 102

#define _GIFDISPLAY_

static LRESULT WML_EditProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT WMLSelOptProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
void RedrawScrollBar(HWND hWnd, int nTotalLines, int nLinePos, int nLinesPerPage);

typedef struct tagWB_PENSTYLEMAP
{
    enum WB_PENSTYLE style;
    int winstyle;
}WB_PENSTYLEMAP;

static const WB_PENSTYLEMAP PenStyleMap[] =
{
    {WBPS_SOLID,       PS_SOLID       },   /* 实线笔               */
    {WBPS_DASH,        PS_DASH        },   /* 虚线笔     -------   */
    {WBPS_DOT,         PS_DOT         },   /* 点线笔     .......   */
    {WBPS_DASHDOT,     PS_DASHDOT     },   /* 点划线笔   _._._._   */
    {WBPS_DASHDOTDOT,  PS_DASHDOTDOT  },   /* 双点划线笔 _.._.._   */
    {WBPS_NULL,        PS_NULL        },   /* 空笔                 */
};

short WB_GetSysFontsize(void *hWnd)
{
    return 16;
}

int   WB_InitFont(void *hWnd)
{
	return 1;
}

void  WB_ReleaseFont(void *hWnd)
{
	return;
}

void *WB_SetFont(void * pDev, WB_FONT* pWBNewFont, WB_FONT* pWBOldFont)
{
	return NULL;
}

int WB_RestoreFont(void * pDev, WB_FONT* pWBOldFont)
{
	return 1;
}

int WB_SetDefFontsize(void *hWnd, short defFontsize)
{
	return 1;
}

int WB_DrawLine(void* pDev, int x1, int y1, int x2, int y2, int style, 
                unsigned long color)
{
	//HDC			hdc = (HDC)pDev;
    //ASSERT(pDev != NULL);
	PUIDATA pUI = (PUIDATA)pDev;
	HDC hdc = ((PUIDATA)pDev)->hdc;
	
	if(hdc == NULL)
		return 0;
	
	x1 -= pUI->rc.left;
	y1 -= pUI->rc.top;
	
	x2 -= pUI->rc.left;
	y2 -= pUI->rc.top;

    if (color == COLOR_INVALID)
    {
		MoveToEx(hdc, x1, y1, NULL);
		LineTo(hdc, x2, y2);	
    }
    else
    {
        HPEN hpen, holdpen;

        hpen = CreatePen(PenStyleMap[style].winstyle, 1, color);
        holdpen = SelectObject(hdc, hpen);
		MoveToEx(hdc, x1, y1, NULL);
		LineTo(hdc, x2, y2);	
        SelectObject(hdc, holdpen);
        DeleteObject(hpen);
    }

    return 1;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int WB_Rectangle(void* pDev, WB_RECT *lprc, unsigned long color, int bFill)
{
    HBRUSH hbrush, holdbrush;
    LOGBRUSH logbrush;

    HPEN		hPen;
    HPEN		hOldPen;
	//HDC			hdc = (HDC)pDev;
	WB_RECT		rc;
	
	PUIDATA pUI = (PUIDATA)pDev;
	HDC hdc = ((PUIDATA)pDev)->hdc;
	
	if(hdc == NULL)
		return 0;
	
	rc.left = lprc->left - pUI->rc.left;
	rc.top = lprc->top - pUI->rc.top;
	rc.right = lprc->right - pUI->rc.left;
	rc.bottom = lprc->bottom - pUI->rc.top;
	
	if(color == COLOR_INVALID)
	{
		//Rectangle(hdc, lprc->left, lprc->top, lprc->right, lprc->bottom);
		Rectangle(hdc, rc.left, rc.top, rc.right + 1, rc.bottom + 1);
		return 1;
	}

	hPen = CreatePen(PS_SOLID, 1, color);
	hOldPen = SelectObject(hdc, hPen);  

    if (bFill)
    {
        logbrush.lbStyle = BS_SOLID;
        logbrush.lbColor = color;
        logbrush.lbHatch = 0;
    }
    else
    {
        logbrush.lbStyle = BS_HOLLOW;
        logbrush.lbColor = color;
        logbrush.lbHatch = 0;
    }

    hbrush = CreateBrushIndirect(&logbrush);
    holdbrush = SelectObject(hdc, hbrush);

    //Rectangle(hdc, lprc->left, lprc->top, lprc->right, lprc->bottom);
	Rectangle(hdc, rc.left, rc.top, rc.right + 1, rc.bottom + 1);

    SelectObject(hdc, holdbrush);
    DeleteObject(hbrush);

	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);

	return 1;
}

/*********************************************************************\
* Function WB_Polygon
* Purpose      
* Params pDev:handle to DC; lpPoints:polygon vertices; 
*		 nCount:count of polygon vertices
* Return	 	   
* Remarks	   
**********************************************************************/
int WB_Polygon(void *pDev, const WB_POINT *lpPoints, int nCount, 
			   unsigned long color, int penStyle, int bFill)
{
    HBRUSH hbrush, holdbrush;
    LOGBRUSH logbrush;

    HPEN		hPen;
    HPEN		hOldPen;
	POINT		*points = NULL;
	
	int			i = 0;
	PUIDATA pUI = (PUIDATA)pDev;
	HDC hdc = ((PUIDATA)pDev)->hdc;
	
	if(hdc == NULL)
		return 0;
	
	points = malloc(sizeof(POINT) * nCount);
	if(points == NULL)
		return 0;
	
	for(i = 0; i < nCount; i++)
	{
		points[i].x = lpPoints[i].x - pUI->rc.left;
		points[i].y = lpPoints[i].y - pUI->rc.top;
	}
	
	if(color == COLOR_INVALID)
	{
		Polygon(hdc, points, nCount);
		return 1;
	}


	hPen = CreatePen(PenStyleMap[penStyle].winstyle, 0 ,color);
	hOldPen = SelectObject(hdc, hPen);  

    if (bFill)
    {
        logbrush.lbStyle = BS_SOLID;
        logbrush.lbColor = color;
        logbrush.lbHatch = 0;
    }
    else
    {
        logbrush.lbStyle = BS_HOLLOW;
        logbrush.lbColor = color;
        logbrush.lbHatch = 0;
    }

    hbrush = CreateBrushIndirect(&logbrush);
    holdbrush = SelectObject(hdc, hbrush);

    //Polygon(hdc, (POINT *)lpPoints, nCount);
	Polygon(hdc, points, nCount);

    SelectObject(hdc, holdbrush);
    DeleteObject(hbrush);

	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);

	free(points);

	return 1;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int WB_Ellipse(void* pDev, WB_RECT *lprc, unsigned long color, int bFill)
{
    HBRUSH		hbrush, holdbrush;
    LOGBRUSH	logbrush;

    HPEN		hPen;
    HPEN		hOldPen;
	WB_RECT		rc;
	
	PUIDATA pUI = (PUIDATA)pDev;
	HDC hdc = ((PUIDATA)pDev)->hdc;
	
	if(hdc == NULL)
		return 0;
	
	rc.left = lprc->left - pUI->rc.left;
	rc.top = lprc->top - pUI->rc.top;
	rc.right = lprc->right - pUI->rc.left;
	rc.bottom = lprc->bottom - pUI->rc.top;
	
	if(color == COLOR_INVALID)
	{
		Ellipse(hdc, lprc->left, lprc->top, lprc->right, lprc->bottom);
		return 1;
	}

	hPen = CreatePen(PS_SOLID, 1, color);
	hOldPen = SelectObject(hdc, hPen);  

    if (bFill)
    {
        logbrush.lbStyle = BS_SOLID;
        logbrush.lbColor = color;
        logbrush.lbHatch = 0;
    }
    else
    {
        logbrush.lbStyle = BS_HOLLOW;
        logbrush.lbColor = color;
        logbrush.lbHatch = 0;
    }

    hbrush = CreateBrushIndirect(&logbrush);
    holdbrush = SelectObject(hdc, hbrush);

    Ellipse(hdc, lprc->left, lprc->top, lprc->right, lprc->bottom);

    SelectObject(hdc, holdbrush);
    DeleteObject(hbrush);

	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);

	return 1;
}

int WB_DrawText(void* pDev, WB_RECT *lprc, 
				char* pText, int len, unsigned long fgcolor, unsigned long bkcolor)
{
    COLORREF clrOldBk, clrOldFg;
	int oldmode;
	PUIDATA pUI = (PUIDATA)pDev;
	HDC hdc = ((PUIDATA)pDev)->hdc;
	int x = lprc->left, y = lprc->top;
	char *szMulti = NULL;
	int nLenMulti = 0;
	
	if(hdc == NULL)
		return 0;
	
	x -= pUI->rc.left;
	y -= pUI->rc.top;
	
	nLenMulti = UTF8ToMultiByte(CP_ACP, 0, pText, len, NULL, 0, NULL, NULL);
	szMulti = (char *)malloc(nLenMulti + 1);
	memset(szMulti, 0, nLenMulti + 1);
	UTF8ToMultiByte(CP_ACP, 0, pText, len, szMulti, nLenMulti, NULL, NULL);
	szMulti[nLenMulti] = '\0';

	if ( bkcolor != COLOR_INVALID )
	{
		clrOldBk = SetBkColor(hdc, bkcolor);
	}
	else
	{
		oldmode = SetBkMode(hdc, TRANSPARENT);
	}
	
	
	if ( fgcolor != COLOR_INVALID )
		clrOldFg = SetTextColor(hdc, fgcolor);

	SelectObject(hdc, hViewFont);

    TextOut(hdc, x, y, szMulti, strlen(szMulti));
	
	if ( bkcolor != COLOR_INVALID )
	{
		SetBkColor(hdc, clrOldBk);
	}
	else
	{
		SetBkMode(hdc, oldmode);			
	}
	
	if ( fgcolor != COLOR_INVALID )
		SetTextColor(hdc, clrOldFg);
	
	free(szMulti);
    return 1;
}


static unsigned int DecodeMultiByteInt(char **src);
static int My_DisplayBMP(HDC hdc, int x, int y, char *pixels, int width, int height);
static int My_DisplayWBMP(HDC hdc, int x, int y, char *pixels, int width, int height);


typedef struct tagBitMapPara
{
	void *hBitMap;
	unsigned long bgcolor;
	BOOL bTrans;
	struct tagBitMapPara *pNext;
}BITPARAM;


static BITPARAM *pBitParamHead = NULL;

static int WB_BitParmInsert(void* hBitmap, unsigned long bgcolor, BOOL bTrans)
{
	BITPARAM *pNew;
	if ( hBitmap == NULL )
		return 0;

	
    pNew = malloc(sizeof(BITPARAM));
	if ( pNew == NULL )
		return 0;
	pNew->hBitMap = hBitmap;
	pNew->bgcolor = bgcolor;
	pNew->bTrans  = bTrans;
	pNew->pNext   = NULL;

	if ( pBitParamHead == NULL )
	{
		pBitParamHead = pNew;
		return 1;	
	}

	pNew->pNext = pBitParamHead;
	pBitParamHead = pNew;

	return 1;
}

static int WB_BitParamDel(void* hBitmap)
{
	BITPARAM *p, *pre;
	p = pBitParamHead;

	if ( p == NULL )
		return 0;

	if ( p->hBitMap == hBitmap )
	{
		pBitParamHead = p->pNext;
		free(p);
		return 1;
	}

	pre = p;
	p = p->pNext;
	while ( p != NULL )
	{
		if ( p->hBitMap == hBitmap )
		{
			pre->pNext = p->pNext;
			free(p);
			return 1;
		}
		pre = p;
		p = p->pNext;
	}

	return 0;
}

static int WB_BitParamGet(void* hBitmap, unsigned long *bgcolor, BOOL *bTrans)
{
	BITPARAM *p;
	p = pBitParamHead;

	while ( p != NULL )
	{
		if ( p->hBitMap == hBitmap )
		{
			*bgcolor = p->bgcolor;
			*bTrans  = p->bTrans;
			return 1;
		}
		
		p = p->pNext;
	}

	return 0;
}

#ifdef _GIFDISPLAY_//如果是Autmen以及后续项目
static void GifPaintBkFunc(HGIFANIMATE hGIFAnimate, BOOL End, int x, int y, HDC hdcMem)
{
    int             width, height;
    RECT            rect;

    width = GetDeviceCaps(hdcMem, HORZRES);
    height = GetDeviceCaps(hdcMem, VERTRES);

    rect.bottom = height;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;

    ClearRect(hdcMem, &rect, RGB(255, 255, 255));
}
#endif

void* WB_StartGif(void* hParentControl, const char* pData, DWORD datalen, 
				  int x, int y, int width, int height)
{
	void* result = NULL;

#ifdef _GIFDISPLAY_//如果是Autmen以及后续项目
	HGIFANIMATE hGif;	
	hGif = StartAnimatedGIFFromDataEx(hParentControl, 
			pData, datalen, 
			x, y, width, height, DM_NONE);	
	if( hGif )
	    SetPaintBkFunc(hGif, GifPaintBkFunc);

	result = (void*)hGif;
#endif
		
	return result;
}

int WB_PaintGif(void* pDev, void* hGifHandle)
{
	int result = 0;
	HDC			hdc = (HDC)pDev;

#ifdef _GIFDISPLAY_//如果是Autmen以及后续项目
	PaintAnimatedGIF(hdc, hGifHandle);
	result = 1;
#endif

	return 1;
}

int    WB_EndGif  (void* hGifHandle)
{
	int result = 0;

#ifdef _GIFDISPLAY_//如果是Autmen以及后续项目	
	EndAnimatedGIF(hGifHandle);
	result = 1;
#endif

	return 1;
}

int    WB_ScrollGif(void* hGifHandle, int x, int y)
{
	int result = 0;

#ifdef _GIFDISPLAY_//如果是Autmen以及后续项目
	ScrollAnimatedGIFEx(hGifHandle, x, y);
	result = 1;
#endif	

	return result;
}

int WB_DrawImageEx(void* pDev, int x, int y, int width, int height, 
				 int src_x, int src_y, int src_width, int src_height,
				 int imgType, char* pData, long len, void** hBitMap, void* Param)
{
	PUIDATA pUI = (PUIDATA)pDev;
	HDC hdc = ((PUIDATA)pDev)->hdc;
	
	if(hdc == NULL)
		return 0;
	
	x -= pUI->rc.left;
	y -= pUI->rc.top;

	switch (imgType)
	{
	case WBIMG_WBMP:// 针对wbmp没有拉伸功能？
		{
			int oldmode;
			oldmode = GetBkMode(hdc);
			SetBkMode(hdc, OPAQUE);

			My_DisplayWBMP(hdc, x, y, pData, width, height);
			
			SetBkMode(hdc, oldmode);
		}
		break;

	case WBIMG_BMP:
		{			
			int oldmode;
			oldmode = GetBkMode(hdc);
			SetBkMode(hdc, OPAQUE);

			My_DisplayBMP(hdc, x, y, pData, width, height);
			
			SetBkMode(hdc, oldmode);
		}
		break;
		
	case WBIMG_GIF:
		if(Param != NULL )			
		{
#ifdef _GIFDISPLAY_		
			if( *hBitMap == NULL || *hBitMap == (void *)-1)
			{
				*hBitMap = (void*)WB_StartGif((void *)((PUIDATA)pDev)->hwnd, pData, len, x, y, width, height);
			}
			if(*hBitMap != NULL )
			{
				WB_ScrollGif((void*)(*hBitMap), x, y);
				WB_PaintGif((void *)hdc, (void*)(*hBitMap));
				break;
			}			
#endif		
		}
		
	case WBIMG_JPG:
	case WBIMG_PNG:
		if ( src_x==0 && src_y==0 && src_width== -1 && src_height == -1 )
		{
			DrawImageFromDataEx(hdc, pData, x, y, width, height, len, SRCCOPY);
			break;
		}
		else
		{	
			unsigned long bgcolor;
			BOOL          bTrans;
			
			if(*hBitMap == NULL)
			{
				*hBitMap = (void*)CreateBitmapFromImageData(hdc, pData, 
					len, &bgcolor, &bTrans);
				WB_BitParmInsert(*hBitMap, bgcolor, bTrans);
			}
			if(*hBitMap != NULL )
			{
				unsigned long BkColor;
				int oldmode;
				
				WB_BitParamGet(*hBitMap, &bgcolor, &bTrans);
				if(bTrans)
				{
					oldmode = SetBkMode(hdc, TRANSPARENT);
					BkColor = SetBkColor(hdc, bgcolor);
				}

				BitBlt(hdc, x, y, width, height, (HDC)(*hBitMap), src_x, src_y, SRCCOPY);

				if(bTrans)
				{
					SetBkColor(hdc, BkColor);
					SetBkMode(hdc, oldmode);
				}
			}
		}
		break;

	default:
		break;
	}

	return 1;
}

int WB_EndImageEx(void* hBitMap, int imgType, void* param)
{
	if (hBitMap == NULL)
		return 0;

	switch ( imgType )
	{
	case WBIMG_GIF:
#ifdef _GIFDISPLAY_
		if(param != NULL)
		{
			WB_EndGif(hBitMap);
			break;
		}
#endif
	case WBIMG_JPG:
	case WBIMG_PNG:
	case WBIMG_BMP:
		DeleteObject(hBitMap);
		WB_BitParamDel(hBitMap);
		break;
	default:
		return 0;
	}

	return 1;
}

int WB_GetTextInfo(void* pDev, WB_FONT * pFont, char* pStr, int len, int* w, int* h)
{
    SIZE size;
	HDC hdc = ((PUIDATA)pDev)->hdc;
	char *szMulti;
	int len1, nLenMulti;
	
	if(hdc == NULL)
		return 0;

	{		
		if (len > 0)
			len1 = len;
		else
		    len1 = strlen(pStr);

		nLenMulti = UTF8ToMultiByte(CP_ACP, 0, pStr, len1, NULL, 0, NULL, NULL);
		szMulti = (char *)malloc(nLenMulti + 1);
		memset(szMulti, 0, nLenMulti + 1);
		UTF8ToMultiByte(CP_ACP, 0, pStr, len1, szMulti, nLenMulti, NULL, NULL);	
		szMulti[nLenMulti] = '\0';
	}


	GetTextExtentPoint32(hdc, szMulti, nLenMulti + 1, &size);
	
	*h = size.cy;
	*w = size.cx;

	free(szMulti);

    return 1;
}


static int My_GetBMPInfo(char *pBMPBuf, int *width, int *height);
static int My_GetWBMPInfo(char *pWBMPBuf, int *width, int *height);
static int My_GetGifInfo(char *pGifBuf, int nGiflen,int *width, int *height);
static unsigned int DecodeMultiByteInt(char **src);

int WB_GetImageInfo(int imagetype, char* pData, int len, int *w, int *h)
{
	if (imagetype == WBIMG_BMP)	//WBIMG_BMP)
		return My_GetBMPInfo(pData, w, h);
	else if(imagetype == WBIMG_WBMP)	//WBIMG_WBMP)
		return My_GetWBMPInfo(pData, w, h);
	//else  if(imagetype == WBIMG_GIF || imagetype == WBIMG_JPG || imagetype == WBIMG_PNG) 
	else if(imagetype == WBIMG_GIF || imagetype == WBIMG_JPG || imagetype == WBIMG_PNG) 
		return My_GetGifInfo(pData, len, w, h);
	return 0;
}

#define WML_GET16(p)(WORD)((*((BYTE*)(p) + 1) << 8) + *((BYTE*)(p)))
#define WML_GET32(p)(DWORD)((*((BYTE*)(p) + 3) << 24) + \
                        (*((BYTE*)(p) + 2) << 16)  + \
                        (*((BYTE*)(p) + 1) << 8) +  *((BYTE*)(p)))

#define OFFSET_OFFBITS          10      // 位图数据偏移字段偏移量
#define OFFSET_BITMAPINFOHEADER 14      // 位图头信息字段偏移量
#define FIELDOFFSET(type, field)    ((int)(&((type *)1)->field)-1)

typedef struct tagBITMAPINFO256
{
    BITMAPINFOHEADER bmiHeader; 
    RGBQUAD          bmiColors[256]; 
} BITMAPINFO256;

#define  CONT_FLAG  0x80

typedef struct tagMONOBITMAPINFO 
{  
    BITMAPINFOHEADER bmiHeader; 
    RGBQUAD          bmiColors[2]; 
} MONOBITMAPINFO; 


static unsigned int DecodeMultiByteInt(char **src)
{
	char *s = *src;
	char i, ByteNum; /*BYTE*/
	unsigned char ch;
	unsigned int  val;
	
	if ((*s & CONT_FLAG) == 0)
	{
		(*src)++;
		return((unsigned int)*s);
	}

	ByteNum = 0;
	while (*s & CONT_FLAG) 
		s++, ByteNum++;

	*src = s + 1; /* src point to the byte behind the integer */

	val = 0;
	for (i = 0; i < ByteNum; i++)
	{
		ch = (char)((*s & 0x7F) >> i) | (char)(*(s - 1) << ( 7 - i));
		val = val + (ch << (i * 8));
		s--;
	}
	ch = (*s & 0x7F) >> i;
	val = val + (char)(ch << (i * 8));

	return val;
}

static int My_GetGifInfo(char *pGifBuf, int nGiflen,int *width, int *height)
{
	char *s = pGifBuf;
	SIZE nGifsize;
	
	GetImageDimensionFromData(pGifBuf,nGiflen, &nGifsize);

	*width = nGifsize.cx;
	*height = nGifsize.cy;

	return 0;
}

static int My_GetWBMPInfo(char *pWBMPBuf, int *width, int *height)
{
	char *s = pWBMPBuf;

	/* check the TypeField: only level 0 is supported. */
	if (*s != 0)
	{
		*width = -1;
		*height = -1;
		return 0;
	}
	s = s + 1;

	/* skip over FixHeaderField and ExtHeaderField(s) */
	while (*s & CONT_FLAG)
		s++;
	s = s + 1;

	/* get the values of width and height */
	*width = DecodeMultiByteInt(&s);
	*height = DecodeMultiByteInt(&s);

	return (s - pWBMPBuf);
}

static int My_GetBMPInfo(char *pBMPBuf, int *width, int *height)
{
	char *s = pBMPBuf;


    // If the file is a invalid bitmap file, just return
    if (WML_GET16(s) != 0x4d42)
    {
		*width = -1;
		*height = -1;
		return 0;
	}

    // Skips the bitmap file header
    s += OFFSET_BITMAPINFOHEADER;

    // Gets BITMAPINFOHEADER from bitmap file data
    *width =  WML_GET32(s + FIELDOFFSET(BITMAPINFOHEADER, biWidth)); 
    *height = WML_GET32(s + FIELDOFFSET(BITMAPINFOHEADER, biHeight)); 

	return 0;
}


static int My_DisplayBMP(HDC hdc, int x, int y, char *pixels, int width, int height)
{
    static BITMAPINFO256 BitmapInfo256;

    int         i;
    BITMAPINFO* pbmi;
    WORD        bfType;
    DWORD       bfOffBits;
    int         nClrBits;
    BYTE*       pData;
    
    pData = (BYTE*)pixels;

    // If the file is a invalid bitmap file, just return
    if ((bfType = WML_GET16(pData + 0)) != 0x4d42)
        return 0;

    pbmi = (PBITMAPINFO)&BitmapInfo256;

    // Gets the bfOffbits field of BITMAPFILEHEADER
    bfOffBits = WML_GET32(pData + OFFSET_OFFBITS);

    // Skips the bitmap file header
    pData += OFFSET_BITMAPINFOHEADER;

    // Gets BITMAPINFOHEADER from bitmap file data
    pbmi->bmiHeader.biSize = 
        WML_GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSize)); 
    pbmi->bmiHeader.biWidth = 
        WML_GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biWidth)); 
    pbmi->bmiHeader.biHeight = 
        WML_GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biHeight)); 
    pbmi->bmiHeader.biPlanes = 
        (WORD)WML_GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biPlanes)); 
    pbmi->bmiHeader.biBitCount = 
        (WORD)WML_GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biBitCount)); 
    pbmi->bmiHeader.biCompression = 
        (WORD)WML_GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biCompression)); 
    pbmi->bmiHeader.biSizeImage = 
        WML_GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSizeImage));
    pbmi->bmiHeader.biXPelsPerMeter = 
        WML_GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biXPelsPerMeter));
    pbmi->bmiHeader.biYPelsPerMeter = 
        WML_GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biYPelsPerMeter));
    pbmi->bmiHeader.biClrUsed = 
        WML_GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrUsed));
    pbmi->bmiHeader.biClrImportant = 
        WML_GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrImportant));

    // Skips bitmap info header
    pData += sizeof(BITMAPINFOHEADER);

    // Calculates the color bits, if the result is an invalid, return 
    // error.
    nClrBits = pbmi->bmiHeader.biPlanes * pbmi->bmiHeader.biBitCount; 
    if (nClrBits != 1 && nClrBits != 2 && nClrBits != 4 && nClrBits != 8
						 && nClrBits != 24)
        return 0;

    // Gets the palettes if necessary
    if (nClrBits <= 8)
    {
        for (i = 0; i < (1 << nClrBits); i++)
        {
            pbmi->bmiColors[i].rgbBlue = *pData++;
            pbmi->bmiColors[i].rgbGreen = *pData++;
            pbmi->bmiColors[i].rgbRed = *pData++;
            pbmi->bmiColors[i].rgbReserved = *pData++;
        }
    }
    else if (pbmi->bmiHeader.biCompression == BI_BITFIELDS)
    {
        pbmi->bmiColors[0] = *((RGBQUAD*)pData);
        pData += 4;
        pbmi->bmiColors[1] = *((RGBQUAD*)pData);
        pData += 4;
        pbmi->bmiColors[2] = *((RGBQUAD*)pData);
    }

    if (width == pbmi->bmiHeader.biWidth && height == pbmi->bmiHeader.biHeight)
        SetDIBitsToDevice(hdc, x, y, pbmi->bmiHeader.biWidth, 
            pbmi->bmiHeader.biHeight, 0, 0, 0, pbmi->bmiHeader.biHeight,
            ((BYTE*)pixels + bfOffBits), (PBITMAPINFO)pbmi, DIB_RGB_COLORS);
    else
        StretchDIBits(hdc, x, y, width, height, 0, 0, 
            pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight,
            ((BYTE*)pixels + bfOffBits), (PBITMAPINFO)pbmi, DIB_RGB_COLORS, SRCCOPY);

	return 1;
}

static int My_DisplayWBMP(HDC hdc, int x, int y, 
				char *pixels, int dstwidth, int dstheight)
{
	MONOBITMAPINFO  bmi;
	char *PixelData, *s;
	int  LnBytes, LnDWBytes;
	char *src = NULL;
	char *head = pixels;
	int width, height;

	/* check the TypeField: only level 0 is supported. */
	if (*head != 0)
	{
		return 0;
	}
	head = head + 1;

	/* skip over FixHeaderField and ExtHeaderField(head) */
	while (*head & CONT_FLAG)
		head++;
	head = head + 1;

	/* get the values of width and height */
	width = DecodeMultiByteInt(&head);
	height = DecodeMultiByteInt(&head);


	/* process the pixel data */
	LnBytes = (width + 7) / 8;
	LnDWBytes = ((width + 31) / 32) * 4;

	PixelData = head;
	s = head;

	if (LnDWBytes > LnBytes) /* the buf is not DWORD aligned */
	{
		char *d;
		int i;
		src = malloc(LnDWBytes * height + 1);
		if (src == NULL)
			return 0;
		memset(src, 0, LnDWBytes * height);
		d = src;
		for (i = 0; i < height; i++)
		{
			memcpy(d, s, LnBytes);
			d = d + LnDWBytes;
			s = s + LnBytes;
		}
		PixelData = src;
	}

	bmi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth			= width;
	bmi.bmiHeader.biHeight			= -height;
    bmi.bmiHeader.biPlanes			= 1;
    bmi.bmiHeader.biBitCount		= 1;
    bmi.bmiHeader.biCompression		= BI_RGB;
    bmi.bmiHeader.biSizeImage		= 0;
    bmi.bmiHeader.biXPelsPerMeter	= 0; 
    bmi.bmiHeader.biYPelsPerMeter	= 0; 
    bmi.bmiHeader.biClrUsed			= 0; 
    bmi.bmiHeader.biClrImportant	= 0; 

	bmi.bmiColors[0].rgbBlue     = 0;
	bmi.bmiColors[0].rgbGreen    = 0;
	bmi.bmiColors[0].rgbRed      = 0;
	bmi.bmiColors[0].rgbReserved = 0;
	bmi.bmiColors[1].rgbBlue     = 255;
	bmi.bmiColors[1].rgbGreen    = 255;
	bmi.bmiColors[1].rgbRed      = 255;
	bmi.bmiColors[1].rgbReserved = 0;

	if (width == dstwidth && height == dstheight)
	{
		SetDIBitsToDevice(hdc, x, y, dstwidth, dstheight, 0, 0, 0, height, 
				PixelData, (BITMAPINFO *)&bmi, DIB_RGB_COLORS);
	}
	else
	{
		StretchDIBits(hdc, x, y, dstwidth, dstheight, 0, 0, width, height,
                PixelData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, SRCCOPY);
	}

    if (src != NULL)
        free(src);

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//Gif相关函数


/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks zjy add
**********************************************************************/
/*int WB_SetClipRect(void* pDev, WB_RECT* lprc)
{
	HDC hdc = (HDC)pDev;
	SetClipRect(hdc, (RECT*)lprc);
	return 1;
}*/


void WB_DeleteObject(void *hObj)
{
	DeleteObject(hObj);
}

int WB_SetTextStyle( void* pUIData, WB_TEXTSTYLEOPTION option, void* pnewStyle, void* poldStyle)
{
	HDC			hdc = ((PUIDATA)pUIData)->hdc;
	
    if(hdc == NULL)
		return 0;

	switch (option)
	{
	case WBTS_FONT:
		{
			if(poldStyle != NULL)
				WB_SetFont(pUIData, (WB_FONT *)pnewStyle, (WB_FONT *)poldStyle);
			else
				WB_RestoreFont(pUIData, (WB_FONT *)pnewStyle);
		}
		break;
	case WBTS_COLOR:
		{
			unsigned long color = SetTextColor(hdc, *(unsigned long*)pnewStyle);
			if(poldStyle != NULL)
				*((unsigned long *)poldStyle) = color;
		}
		break;
	default:
		break;
	}
	return 1;
}


int WB_StartRing(int soundType, int playmode, unsigned char *data, int datalen)
{	
#if (defined(HUSTON_AUTMAN) || defined(APOLLO))//如果是Autmen以及后续项目
	BYTE realType;
	BYTE realmode;

#ifdef APOLLO
	MC_INFO audioitem;
	memset(&audioitem, 0, sizeof(MC_INFO));
#endif
	
#ifdef HUSTON_AUTMAN
	switch(soundType)
	{
	case WB_AUDIOMMF:
		realType = SOUND_MMF;
		break;
	case WB_AUDIOMIDI:
		realType = SOUND_MIDI;
		break;
	case WB_AUDIOAMR:
		realType = SOUND_AMR;
		break;
	case WB_AUDIOWAV:
		realType = SOUND_WAVE;
		break;
	default:
		realType = SOUND_UNKNOWN;
		break;
	}
#else 
	switch(soundType)
	{
	case WB_AUDIOMMF:
		realType = MEDIA_INVALIDATE;		
		break;
	case WB_AUDIOMIDI:
		realType = MEDIA_MIDI;
		strcpy( audioitem.media_name,  "*.midi");
		break;
	case WB_AUDIOAMR:
		realType = MEDIA_AMR;
		strcpy( audioitem.media_name,  "*.amr");
		break;
	case WB_AUDIOWAV:
		realType = MEDIA_WAV;
		strcpy( audioitem.media_name,  "*.wav");
		break;
	default:
		realType = MEDIA_INVALIDATE;
		break;
	}
#endif	

	switch (playmode)
	{
	case -1:
		realmode = 0;
	default:
		realmode = playmode;
	}
#ifdef HUSTON_AUTMAN
	SEF_PlayRingFromData(realType, realmode, data, datalen, playSoundCallback);       
#endif

#ifdef APOLLO
	audioitem.media_type = realType;
	audioitem.play_type = MC_BUFFER_TYPE;
	audioitem.buf_len = datalen;
//	audioitem.play_times = realmode;
//	audioitem.play_priority = 25;
	audioitem.media_buf = data;
	if(audioitem.media_type == MEDIA_AMR)
	{
		//amr数据需要去掉amr的头信息传进去，因此+6
		audioitem.media_buf = data + 6;
	}
	
	audioitem.p_mc_ui_callback = (void *)playSoundCallback;
	//mc_play(&audioitem);
	HIM_StartNotify_buffer(9, audioitem.media_type, 
		audioitem.media_buf, audioitem.buf_len, audioitem.p_mc_ui_callback);

#endif
	
#endif
	return 1;
}

int WB_StopRing (void)
{
#ifdef HUSTON_AUTMAN//如果是Autmen以及后续项目
	AS_StopPlay();
#endif
#ifdef APOLLO
    HIM_StopNotify(9);	//audio_control_stop();
#endif

	return 1;
}

int  WB_BeginDisplay(void* pUIData, WB_RECT *lprc)
{
	PUIDATA pUI = (PUIDATA)pUIData;
	HDC hMemDC = NULL;
	RECT rc;
	
	if(lprc != NULL)
	{
		pUI->rc.left = lprc->left;
		pUI->rc.top = lprc->top;
		pUI->rc.right = lprc->right;
		pUI->rc.bottom = lprc->bottom;
	}
	else
	{
		GetClientRect(hViewWnd, &rc);
		
		pUI->rc.left = rc.left;
		pUI->rc.top = rc.top;
		pUI->rc.right = rc.right;
		pUI->rc.bottom = rc.bottom;
	}
	
	hMemDC = CreateMemoryDC(pUI->rc.right - pUI->rc.left + 1, 
		pUI->rc.bottom - pUI->rc.top + 1);
	
	if(hMemDC != NULL)
	{
		pUI->hdc = hMemDC;
		
		rc.left = rc.top = 0;
		rc.right = pUI->rc.right - pUI->rc.left + 1;
		rc.bottom = pUI->rc.bottom - pUI->rc.top + 1;
		FillRect (hMemDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
	}
	
	return 1;
}

int  WB_EndDisplay(void* pUIData)
{
	PUIDATA pUI = (PUIDATA)pUIData;
	
	if(hViewDC != pUI->hdc && pUI->hdc != NULL && IsWindowVisible(GetWAPFrameWindow()))
	{
		BitBlt(hViewDC, pUI->rc.left, pUI->rc.top, 
			pUI->rc.right - pUI->rc.left + 1, pUI->rc.bottom - pUI->rc.top + 1, 
			pUI->hdc, 0, 0, SRCCOPY);
		
		DeleteDC(pUI->hdc);
		pUI->hdc = hViewDC;
	}
	
	return 1;
}

static HWND hEdit;
static RECT rClient;
int nInputMaxLen;
char *pTmpbuf;
static ECHO_FINISHEDITTEXT callback;
HBROWSER hWin;
HWND hInput;
char *szBuf;

int WB_InputMethod(WB_EDITTYPE iEditType , char *pBuffer, int iMaxCharLen, int iBufferLen, char *InputFormat, ECHO_FINISHEDITTEXT pFinishEditProc)
{
	WNDCLASS wc;
	wc.style         = 0;
    wc.lpfnWndProc   = WML_EditProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 4;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLEditClass";

	if (!RegisterClass(&wc))
        return 0;

	GetClientRect(hwndIEApp, &rClient);

	nInputMaxLen = iBufferLen;
	callback = pFinishEditProc;
	hWin = pUIBrowser;
	pTmpbuf = pBuffer;
	
	hEdit = CreateWindow(
        "WMLEditClass", 
        WML_INPUTEDIT,
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        GetWAPFrameWindow(), 
        NULL,
        NULL, 
        NULL
        );
		
	if (hEdit == NULL)
		return 0;
	SendMessage(hInput, EM_SETSEL, -1, -1);
	ShowWindow(hEdit, SW_SHOW);
	UpdateWindow(hEdit); 

	return 1;
	
}

static LRESULT WML_EditProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
		IMEEDIT ie;
		LRESULT lResult = 0;
		HDC hdc;

		char *szMulti = NULL;
		int nLenMulti = 0;

		switch (wMsgCmd)
		{
		case WM_CREATE:
			{
				memset(&ie, 0, sizeof(IMEEDIT));
				ie.hwndNotify   = hWnd;
				ie.dwAscTextMax = 0;
				ie.dwAttrib     = 0;//IME_ATTRIB_GENERAL;
				ie.dwUniTextMax = 0;
				ie.pszCharSet   = NULL;
				ie.pszImeName   = NULL;//"字母";
				ie.pszTitle     = NULL;
				ie.uMsgSetText  = NULL;
				ie.wPageMax     = 0;

				hInput = CreateWindow("IMEEDIT", "", 
					//WS_VISIBLE|WS_CHILD|WS_TABSTOP|ES_UNDERLINE|ES_MULTILINE|WS_VSCROLL, 
					WS_CHILD|WS_TABSTOP|WS_VISIBLE|ES_AUTOHSCROLL|ES_TITLE,
					rClient.left + CX_FITIN, 
					rClient.top + CY_FITIN, 
					rClient.right - rClient.left - 2 * CX_FITIN, 
					(rClient.bottom - rClient.top - CY_FITIN)/3, 
					hWnd, 
					NULL, NULL, (PVOID)&ie);
				
				if(hInput == NULL)
					return FALSE;
				
				SendMessage(hInput, EM_SETTITLE, 0, (LPARAM)WML_INPUTEDIT);

				if(pTmpbuf != NULL && strlen(pTmpbuf) > 0)
				{
					nLenMulti = UTF8ToMultiByte(CP_ACP, 0, pTmpbuf, -1, NULL, 0, NULL, NULL);
					szMulti = (char *)malloc(nLenMulti + 1);
					memset(szMulti, 0, nLenMulti + 1);
					UTF8ToMultiByte(CP_ACP, 0, pTmpbuf, -1, szMulti, nLenMulti, NULL, NULL);
					szMulti[nLenMulti] = '\0';
				
					SetWindowText(hInput, szMulti);
				}
				szBuf = (char *)malloc(nInputMaxLen + 1);
				SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_INPUTEDIT);	

				PDASetMenu(GetWAPFrameWindow(), NULL);
				SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_INPUTEDIT);	
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)STR_WAPWINDOW_SAVE);
				free(szMulti);
				SetFocus(hInput);
			}
			break;

		case PWM_SHOWWINDOW:
			PDASetMenu(GetWAPFrameWindow(), NULL);
			SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_INPUTEDIT);	
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)STR_WAPWINDOW_SAVE);
			break;
			
		case WM_SETRBTNTEXT:
			if (strcmp((char *)lParam, (LPCSTR)WML_MENU_BACK) == 0)
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
			else
			{
				if((hWnd == GetFocus()) || (hWnd == GetParent(GetFocus())))
					SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, lParam);
			}
			break;
			

		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_F10:
				{
					int len;
					char *tmp;
					int tmplen;
					
					GetWindowText(hInput, szBuf, nInputMaxLen);
					len = strlen(szBuf);

					tmplen = MultiByteToUTF8(CP_ACP, 0, (LPCSTR)szBuf, -1, NULL, 0);
					tmp = (char *)malloc(tmplen + 1);
					memset(tmp, 0, tmplen + 1);
					MultiByteToUTF8(CP_ACP, 0, (LPCSTR)szBuf, -1, tmp, tmplen);
					tmp[tmplen] = '\0';

					strcpy(pTmpbuf, tmp);
					callback(hWin, pTmpbuf, FALSE);
					free(tmp);
					PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				}
				break;
				
			case VK_RETURN:
				PostMessage(hWnd, WIE_INPUT_SAVE, NULL, NULL);
				break;
				
			default:
				return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);            
			}
			break;
			
		case WIE_INPUT_SAVE:
			{
				char *tmp;
				int tmplen;
				GetWindowText(hInput, szBuf, nInputMaxLen);

				tmplen = MultiByteToUTF8(CP_ACP, 0, (LPCSTR)szBuf, -1, NULL, 0);
				tmp = (char *)malloc(tmplen + 1);
				memset(tmp, 0, tmplen + 1);
				MultiByteToUTF8(CP_ACP, 0, (LPCSTR)szBuf, -1, tmp, tmplen);
				tmp[tmplen] = '\0';

				strcpy(pTmpbuf, tmp);
				callback(hWin, pTmpbuf, TRUE);
				free(tmp);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			}
			break;

		case WM_PAINT:
			{				
				int OldStyle;
				
				hdc = BeginPaint(hWnd, NULL);//为窗口的重画准备DC，并填充给出的绘图结构。
				OldStyle = SetBkMode(hdc, TRANSPARENT);
				SelectObject(hdc, hViewFont);
				if((szBuf != NULL) && (strlen(szBuf) != 0))
					TextOut(hdc, 0, 4, szBuf, -1);
				else
					TextOut(hdc, 0, 4, "", -1);
				SetBkMode(hdc, OldStyle);
				EndPaint(hWnd, NULL);;
			}        
			break;
			
		case WM_CLOSE:
			SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			DestroyWindow(hWnd);
			free(szBuf);
			UnregisterClass("WMLEditClass", NULL);
			break;

		default:        
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}

		return lResult; 
}

static int nItem;
static ECHO_FINISHSELECT select;
static int tp;
static WB_OPTIONDATA opts[50];
#define SELOPT scroll_y
#define EACH 30
#define WIDTH 176
#define NUMPERPAGE 5
HWND hSelOpt;
int *pSelect = NULL;
int nSelNum = 0;
int index = 0;
HWND *options;
BOOL bSigOrMul = FALSE;
int WIE_WapSelectOption(int type, WB_OPTIONDATA seloptions[], int iItemNum, ECHO_FINISHSELECT FinishSelect)
{
	WNDCLASS wc;            
	
	wc.style         = 0;
	wc.lpfnWndProc   = WMLSelOptProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "WMLSelOptClass";
	
	if (!RegisterClass(&wc))
		return 0;

	tp = type;
	nItem = iItemNum;
	select = FinishSelect;
	memcpy(opts, seloptions, (sizeof(WB_OPTIONDATA) * iItemNum));

	GetClientRect(hwndIEApp, &rClient);

	hSelOpt = CreateWindow(
		"WMLSelOptClass", 
		"", 
		WS_VISIBLE| WS_VSCROLL|WS_CHILD,
		rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
		GetWAPFrameWindow(), 
		NULL,
		NULL, 
		NULL
		);


	if ( hSelOpt == NULL ) 
    {
        UnregisterClass("WMLSelOptClass",NULL);
        return 0;
    }
    
    ShowWindow(hSelOpt, SW_SHOW);            
    UpdateWindow(hSelOpt);
    
    return 1;
	
	
}

static void WML_OnVScroll(HWND hWnd, int nScrollCode, short nPos)
{
	int i;
	switch(nScrollCode)
	{
    case SB_LINEUP :
		if(scroll_y < -1 * EACH)
			scroll_y += EACH;
		else
			scroll_y = 0;
		break;
	case SB_LINEDOWN:
		if(scroll_y > -(nItem-NUMPERPAGE-1)*EACH)
			scroll_y -= EACH;
		else
			scroll_y = -(nItem-NUMPERPAGE)*EACH;
		break;

	case SB_ENDSCROLL:// Ends scroll. 
		break;

	case SB_PAGEDOWN:// Scrolls one page down. 
		if(scroll_y > -(nItem-NUMPERPAGE * 2)*EACH)
			scroll_y -= NUMPERPAGE*EACH;
		else
			scroll_y = -(nItem-NUMPERPAGE)*EACH;
		break;
		
	case SB_PAGEUP:// Scrolls one page up. 
		if(scroll_y< -NUMPERPAGE * EACH)
			scroll_y += NUMPERPAGE*EACH;
		else
			scroll_y = 0;
		break;

	case SB_THUMBPOSITION:// The user has dragged the scroll box (thumb) and released the mouse button. The nPos parameter indicates the position of the scroll box at the end of the drag operation. 
		scroll_y = -nPos * EACH;
		break;

	case SB_THUMBTRACK:// The user is dragging the scroll box. This message is sent repeatedly until the user releases the mouse button. The nPos parameter indicates the position that the scroll box has been dragged to. 
		break;

	case SB_TOP:// Scrolls to the upper left. 
		scroll_y = 0;
		break;

	case SB_BOTTOM:// Scrolls to the lower right. 
		scroll_y = -(nItem - NUMPERPAGE) * EACH;
		break;

	default:
		break;
	}
	
	nPos = -scroll_y/EACH;
	RedrawScrollBar(hWnd, nItem, nPos, NUMPERPAGE);
	for(i=0; i<nItem; i++)
	MoveWindow(options[i], 0, (SELOPT + EACH * i), WIDTH, EACH, FALSE);
	InvalidateRect(hWnd, NULL, TRUE);
}

static LRESULT WMLSelOptProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
		LRESULT lResult = 0;
		HDC hdc;
		HWND hcurfocus;
		int j;
		char *szMulti = NULL;
		int nLenMulti = 0;

		switch (wMsgCmd)
		{
		case WM_CREATE:
			{
				int i = 0;
				j = 0;
				options = (HWND *)malloc(sizeof(HWND) * nItem);
				pSelect = (int *)malloc(sizeof(int) * nItem);
				for(i = 0; i < nItem; i++)
				{
					if(tp == WBSEL_SINGLE)
					{
						options[i] =  CreateWindow(
							"BUTTON",
							"",
							WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_CHILD | CS_NOSYSCTRL,
							0, (SELOPT + EACH * i), WIDTH, EACH,
							hWnd,
							NULL,
							NULL,
							NULL
							);
						if(opts[i].selected == TRUE)
						{
							SendMessage(options[i], BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
							*(pSelect + (j++)) = i;
							nSelNum++;
						}
						bSigOrMul = FALSE;	
					}
					else if(tp == WBSEL_MULTIPLE)
					{
						options[i] =  CreateWindow(
							"BUTTON",
							"",
							WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX | WS_CHILD | CS_NOSYSCTRL,
							0, SELOPT + EACH * i, WIDTH, EACH,
							hWnd,
							NULL,
							NULL,
							NULL
							);
						if(opts[i].selected == TRUE)
						{
							SendMessage(options[i], BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
							*(pSelect + (j++)) = i;
							nSelNum++;
						}
						bSigOrMul = TRUE;
					}
					if(opts[i].caption != NULL)
					{
						nLenMulti = UTF8ToMultiByte(CP_ACP, 0, opts[i].caption, -1, NULL, 0, NULL, NULL);
						szMulti = (char *)malloc(nLenMulti + 1);
						memset(szMulti, 0, nLenMulti + 1);
						nLenMulti = UTF8ToMultiByte(CP_ACP, 0, opts[i].caption, -1, szMulti, nLenMulti, NULL, NULL);
						szMulti[nLenMulti] = '\0';
						SetWindowText(options[i], szMulti);
					}
				}	
				SetFocus(options[0]);
				index = 0;
				RedrawScrollBar(hWnd, nItem, 0, NUMPERPAGE);
			}

			PDASetMenu(GetWAPFrameWindow(), NULL);
			SetWindowText(GetWAPFrameWindow(), (LPCTSTR)"Select Option");	
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)STR_WAPWINDOW_OK);
			
			free(szMulti);
			break;

		case PWM_SHOWWINDOW:
			PDASetMenu(GetWAPFrameWindow(), NULL);
			SetWindowText(GetWAPFrameWindow(), (LPCTSTR)"Select Option");	
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)STR_WAPWINDOW_OK);
			break;

		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_UP:
				if(index == 0)
					index = nItem - 1;
				else
					index -= 1;
				SetFocus(options[index]);
				
				if((index * EACH + scroll_y) < 0)
					WML_OnVScroll(hWnd, SB_THUMBPOSITION, (short)index);
//				else if((index * EACH + scroll_y) == (nItem - 1) * EACH)
//					WML_OnVScroll(hWnd, SB_THUMBPOSITION, (short)(nItem - 6));
				else if((index * EACH + scroll_y) >= 5 * EACH)
					WML_OnVScroll(hWnd, SB_THUMBPOSITION, (short)(index - 4));
				break;
				
			case VK_DOWN:
				if(index == (nItem - 1))
					index = 0;
				else
					index += 1;
				SetFocus(options[index]);
				
				if((index * EACH + scroll_y) < 0)
					WML_OnVScroll(hWnd, SB_THUMBPOSITION, (short)index);
//				else if((index * EACH + scroll_y) == (nItem - 1) * EACH)
//					WML_OnVScroll(hWnd, SB_THUMBPOSITION, (short)(nItem - 6));
				else if((index * EACH + scroll_y) >= 5 * EACH)
					WML_OnVScroll(hWnd, SB_THUMBPOSITION, (short)(index - 4));
				break;

			case VK_LEFT:
				if(opts[index].selected == FALSE)
				{
					opts[index].selected = TRUE;
					SendMessage(options[index], BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
					*(pSelect + (nSelNum++)) = index;
					if(opts[index].onpick == TRUE)
					{
						select(pUIBrowser, pSelect, nSelNum, index, WBSEL_ONPICK);
						PostMessage(hWnd, WM_CLOSE, 0, 0);
					}
				}
				break;

			case VK_RIGHT:
				{
					int k, a;
					if(opts[index].selected == TRUE)
					{
						opts[index].selected = FALSE;
						SendMessage(options[index], BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
						for(k = 0; k < nSelNum; k++)
						{
							if(*(pSelect + k) == index)
							{
								a = k;
								while(k < nSelNum)
									*(pSelect + k) = *(pSelect + (k++));
								nSelNum--;
								if((opts[a].onpick == TRUE) && (tp == WBSEL_MULTIPLE))
								{
									select(pUIBrowser, pSelect, nSelNum, index, WBSEL_ONPICK);
									PostMessage(hWnd, WM_CLOSE, 0, 0);
								}
							}
						}
					}
				}
				break;

			case VK_F5:
				if(bSigOrMul)
				{
					if(opts[index].selected == FALSE)
					{
						opts[index].selected = TRUE;
						SendMessage(options[index], BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
						*(pSelect + (nSelNum++)) = index;
						if(opts[index].onpick == TRUE)
						{
							select(pUIBrowser, pSelect, nSelNum, index, WBSEL_ONPICK);
							PostMessage(hWnd, WM_CLOSE, 0, 0);
						}
					}
					else if(opts[index].selected == TRUE)
					{
						int k, a;
						
						opts[index].selected = FALSE;
						SendMessage(options[index], BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
						for(k = 0; k < nSelNum; k++)
						{
							if(*(pSelect + k) == index)
							{
								a = k;
								while(k < nSelNum)
									*(pSelect + k) = *(pSelect + (k++));
								nSelNum--;
								if((opts[a].onpick == TRUE) && (tp == WBSEL_MULTIPLE))
								{
									select(pUIBrowser, pSelect, nSelNum, index, WBSEL_ONPICK);
									PostMessage(hWnd, WM_CLOSE, 0, 0);
								}
							}
						}
					}
				}
				else
				{
					SendMessage(options[*pSelect], BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
					opts[index].selected = TRUE;
					SendMessage(options[index], BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
					*pSelect = index;
					if(opts[index].onpick == TRUE)
					{
						select(pUIBrowser, pSelect, nSelNum, index, WBSEL_ONPICK);
						PostMessage(hWnd, WM_CLOSE, 0, 0);
					}
				}

				break;
				
			case VK_F10:
				select(pUIBrowser, pSelect, nSelNum, index, WBSEL_CANCEL);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;
				
			case VK_RETURN:
				select(pUIBrowser, pSelect, nSelNum, index, WBSEL_OK);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;
				
			default:
				return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);            
			}
			break;
		case WM_COMMAND:
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				hcurfocus = (HWND)lParam;
				for(j = 0; j < nItem; j++)
				{
					if(hcurfocus == options[j])
					{
						index = j;
						break;
					}
				}
				if(j != nItem)
				{
					int a, k;
					if(opts[index].selected == FALSE)
					{
						opts[index].selected = TRUE;
						if(tp == WBSEL_SINGLE)
							*pSelect = index;
						else
							*(pSelect + (nSelNum++)) = index;
						if(opts[index].onpick == TRUE)
						{
							select(pUIBrowser, pSelect, nSelNum, index, WBSEL_ONPICK);
							PostMessage(hWnd, WM_CLOSE, 0, 0);
							break;
						}
					}
					else
					{
						if(tp == WBSEL_MULTIPLE)
						{	
							opts[index].selected = FALSE;
							for(k = 0; k < nSelNum; k++)
							{
								if(*(pSelect + k) == index)
								{
									a = k;
									while(k < nSelNum)
										*(pSelect + k) = *(pSelect + (k++));
									nSelNum--;
									if((opts[a].onpick == TRUE) && (tp == WBSEL_MULTIPLE))
									{
										select(pUIBrowser, pSelect, nSelNum, index, WBSEL_ONPICK);
										PostMessage(hWnd, WM_CLOSE, 0, 0);
										break;
									}
								}
							}
						}
					}
					SetFocus(options[index]);
				}
				break;
			}
			
			break;

		case WM_VSCROLL:
			WML_OnVScroll(hWnd, (int) LOWORD(wParam), (short int) HIWORD(wParam));
			break;

		case WM_ACTIVATE:
			if (WA_ACTIVE == LOWORD(wParam))
				SetFocus(options[index]);//激活后设置焦点
			break;

		case WM_PAINT:
			{				
				hdc = BeginPaint(hWnd, NULL);
				EndPaint(hWnd, NULL);
			}        
			break;
			
		case WM_CLOSE:
			SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			UnregisterClass("WMLSelOptClass", NULL);
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			free((void *)options);
			free(pSelect);
			pSelect = NULL;
			nSelNum = 0;
			break;

		default:        
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}

		return lResult; 
}

void RedrawScrollBar(HWND hWnd, int nTotalLines, int nLinePos, int nLinesPerPage)
{
	SCROLLINFO sInfo;
	
	sInfo.cbSize = sizeof(SCROLLINFO);
	sInfo.fMask = SIF_ALL;
	sInfo.nMax = nTotalLines - 1;
	sInfo.nMin = 0;
	sInfo.nPage = nLinesPerPage;
	sInfo.nPos = nLinePos;
	SetScrollInfo(hWnd, SB_VERT, &sInfo, TRUE);
}




