/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Define prototypes for graph function.
 *            
\**************************************************************************/

#ifndef __WSGRAPH_H
#define __WSGRAPH_H

BOOL WS_StartDrag(PBRUSHOBJ pBrush, int width);
void WS_EndDrag(void);
void WS_DrawDragFrame(PDC pDC, const RECT* pRect, const RECT *pRectClip);

void WS_LineTo(PDC pDC, int x, int y);
void WS_DrawRect(PDC pDC, const RECT* pRect);
void WS_ClearRect(PDC pDC, const RECT* pRect, COLORREF color);
void WS_FrameRect(PDC pDC, const RECT* pRect, PBRUSHOBJ pBrush);

void WS_FrameRectEx(PDC pDC, const RECT* pRect, PBRUSHOBJ pBrush, 
                    int width, const RECT *pRectClip);

void WS_RoundRect(PDC pDC, int x1, int y1, int x2, int y2, int nWidth, 
                  int nHeight);

void WS_DrawCircle(PDC pDC, int x, int y, int r);

void WS_PatBlt(PDC pDC, int x, int y, int width, int height, DWORD dwRop);
void WS_BitBlt(PDC pDestDC, int nDestX, int nDestY, int nWidth, 
               int nHeight, PDC pSrcDC, int nSrcX, int nSrcY, DWORD dwRop);
void WS_StretchBlt(PDC pDestDC, int nDestX, int nDestY, int nDestW, 
                   int nDestH, PDC pSrcDC, int nSrcX, int nSrcY, 
                   int nSrcW, int nSrcH, DWORD dwRop);
int  WS_SetDIBitsToDevice(PDC pDestDC, int nDestX, int nDestY, 
                          DWORD dwWidth, DWORD dwHeight, int nSrcX, 
                          int nSrcY, UINT uStartScan, 
                          UINT cScanLines, CONST VOID *lpvBits, 
                          CONST BITMAPINFO *lpbmi, UINT fuColorUse);
int WS_StretchDIBits(PDC pDestDC, int nDestX, int nDestY, 
                     DWORD nDestWidth, DWORD nDestHeight, int nSrcX, 
                     int nSrcY, UINT nSrcWidth,UINT nSrcHeight, 
                     CONST VOID *lpvBits, CONST BITMAPINFO *lpbmi,
                     UINT fuColorUse,DWORD dwRop);
BOOL WS_DrawBitmap(PDC pDC, int x, int y, int width, int height, 
                   int srcx, int srcy, const void* pvBits, DWORD dwRop);
BOOL WS_DrawDIBitmap(PDC pDC, int x, int y, int srcx, int srcy, 
                     const DIBITMAP* pDIBitmap, DWORD dwRop);

DWORD WS_SetDefaultChar(PDC pDC, DWORD dwChar);
BOOL WS_TextOut(PDC pDC, int x, int y, PCSTR pStr, int count);
int  WS_DrawText(PDC pDC, PCSTR pStr, int count, PRECT prcClear, UINT format);
BOOL WS_GetTextExtent(PDC pDC, PCSTR pStr, int count, PSIZE pSize);
BOOL WS_GetTextExtentExPoint(PDC pDC, PCSTR pStr, int count, int nMaxExtent,
                             PINT lpnFit, PINT alpDx, PSIZE pSize);

COLORREF WS_SetPixel(PDC pDC, int x, int y, COLORREF crColor);
COLORREF WS_GetPixel(PDC pDC, int x, int y);

void WS_DrawEllipse(PDC pDC, int x1, int y1, int x2, int y2);
void WS_DrawPolygon(PDC pDC, POINT* ppoints, int count);
void WS_AngleArc(PDC pDC, int x, int y, DWORD dwRadius, 
                 FLOAT eStartAngle, FLOAT eSweepAngle);
void WS_Arc(PDC pDC, int nLeftRect, int nTopRect, int nRightRect, 
            int nBottomRect, int nXStartArc, int nYStartArc, int nXEndArc, 
            int nYEndArc, int mode);
void WS_ArcTo(PDC pDC, 
            int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, 
            int nXStartArc, int nYStartArc, int nXEndArc, int nYEndArc);

void WS_AlphaBlend(PDC pDestDC, int nDestX, int nDestY, int nDestW, 
                   int nDestH, PDC pSrcDC, int nSrcX, int nSrcY, 
                   int nSrcW, int nSrcH, PBLENDFUNCTION pBlend);
void WS_AlphaDIBBlend(PDC pDestDC, int nDestX, int nDestY, int nDestW, 
                      int nDestH, const void *lpvBits, 
                      const BITMAPINFO *lpbmi, UINT fuColorUse, int nSrcX, 
                      int nSrcY, int nSrcW, int nSrcH, PBLENDFUNCTION pBlend);
void WS_TransparentBlt(PDC pDestDC, int nDestX, int nDestY, int nDestW, 
                      int nDestH, PDC pSrcDC, int nSrcX, int nSrcY, 
                   int nSrcW, int nSrcH, UINT crTransparent);
void WS_TransparentDIBits(PDC pDestDC, int nDestX, int nDestY, int nDestW, 
                      int nDestH, const void *lpvBits, 
                      const BITMAPINFO *lpbmi, UINT fuColorUse, int nSrcX, 
                      int nSrcY, int nSrcW, int nSrcH, UINT crTransparent);
#endif //__WSGRAPH_H
