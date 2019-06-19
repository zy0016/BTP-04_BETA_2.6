/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display Driver
 *
 * Purpose  : Define the function prototype in the display driver.
 *            
\**************************************************************************/

#ifndef __DISPFUNC_H
#define __DISPFUNC_H

/* The following functions are implemented in display.c  */
static int OpenDevice(PDEV pDev, int dev);
static int CloseDevice(PDEV pDev);
static int SuspendDevice(PDEV pDev);
static int ResumeDevice(PDEV pDev);
static int GetDevInfo(PDEV pDev, PDISPLAYDEVINFO pInfo);

static int EnumMode(PDEV pDev, int nModeNum, PDISPMODE pDispMode);
static int ChangeMode(PDEV pDev, PDISPMODE pDispMode, uint32 flags);

static int CreateMemoryDevice(PDEV pDev, int width, int height,
                                PMEMDEV pMemDev);
static int DestroyMemoryDevice(PDEV pMemDev);
static int CreateBitmap(PDEV pDev, int width, int height, 
                          PPHYSBMP pPhysBmp);
static int DestroyBitmap(PDEV pDev, PPHYSBMP pPhysBmp);
static int SetBitmapBuffer(PDEV pDev, PPHYSBMP pPhysBmp, 
                             void* pBuffer);

static int RealizePen(PDEV pDev, PDRVLOGPEN pLogPen, PPHYSPEN pPhysPen);
static int UnrealizePen(PDEV pDev, PPHYSPEN pPen);
static int RealizeBrush(PDEV pDev, PDRVLOGBRUSH pLogBrush,
                          PPHYSBRUSH pPhysBrush);
static int UnrealizeBrush(PDEV pDev, PPHYSBRUSH pBrush);
static int RealizePalette(PDEV pDev, PDRVLOGPALETTE pLogPalette,
                            PPHYSPALETTE pPhysPalette);
static int UnrealizePalette(PDEV pDev, PPHYSPALETTE pPalette);
static int RealizeColor(PDEV pDev, PPHYSPALETTE pPalette, int32 color);
static int SelectPalette(PDEV pDev, PPHYSPALETTE pPalette);

static int UpdateScreen(PDEV pDev);
static int DrawGraph(PDEV pDev, int func, PGRAPHPARAM pGraphPara, 
                       void* pDrawData);

/* The following functions are implemented in cursor.c  */
static int RealizeCursor(PDEV pDev, PDRVLOGCURSOR pLogCursor,
                           PPHYSCURSOR pPhysCursor);
static int UnrealizeCursor(PDEV pDev, PPHYSCURSOR pCursor);
static int ShowCursor(PDEV pDev, PPHYSCURSOR pCursor, int mode);
static int SetCursorPos(PDEV pDev, int x, int y);
static int CheckCursor(PDEV pDev);
static void _ExcludeCursor(PDEV pDev, PRECT pRC);
static void _UnexcludeCursor(PDEV pDev);

/* The following functions are implemented in screen.c  */
static int SaveScreen(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect);
static int RestoreScreen(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect);
static int ScrollScreen(PDEV pDev, PRECT pRect, PRECT pClipRect,
                          int x, int y);

/* The following functions are implemented in utils.c  */
static void InitGraphTools(PDEV pDev);
static int CreateGEPen(PPHYSPEN pPen);
static void DestroyGEPen(PPHYSPEN pPen);
static int CreateGEBrush(PPHYSBRUSH pBrush, int32 fgcolor, int32 bkcolor);
static void DestroyGEBrush(PPHYSBRUSH pBrush);
static void CreateGECursor(PPHYSCURSOR pCursor);
static void DestroyGECursor(PPHYSCURSOR pCursor);

static int RealizeColorToRGB(PDEV pDev, uint8* pscolor, uint32 *pdcolor, 
                             int32 pixellen);

/* The following functions are implemented in line.c  */
static int GetPixel(PDEV pDev, PGRAPHPARAM pGraphParam, 
                     PPOINTDATA pDrawData);
static int SetPixel(PDEV pDev, PGRAPHPARAM pGraphParam, 
                     PPOINTDATA pDrawData);
static int DrawPoint(PDEV pDev, PGRAPHPARAM pGraphParam, 
                     PPOINTDATA pDrawData);
static int DrawRect(PDEV pDev, PGRAPHPARAM pGraphParam, 
                    PRECTDATA pRectData);
static int DrawLine(PDEV pDev, PGRAPHPARAM pGraphParam, 
                    PLINEDATA pLineData);
static int ClipLine(PLINEDATA pLineData, PRECT pClipRect, 
                    LINEDRAW* pLineDraw);
static void DrawSolidHLine(PDEV pDev, int x1, int x2, int y, uint32 rop, 
                           uint32 color);
static void DrawSolidVLine(PDEV pDev, int y1, int y2, int x, uint32 rop, 
                           uint32 color);
static void DrawHLine(PDEV pDev, int x1, int x2, int y, 
                      GRAPHPARAM* pGraphParam, PENSTYLE penstyle);
static void DrawVLine(PDEV pDev, int y1, int y2, int x, 
                      GRAPHPARAM* pGraphParam, PENSTYLE penstyle);

#ifdef SLOPELINE_SUPPORTED
static void DrawSolidXLine(PDEV pDev, LINEDRAW* pLineDraw,
                      GRAPHPARAM* pGraphParam);
static void DrawSolidYLine(PDEV pDev, LINEDRAW* pLineDraw,
                      GRAPHPARAM* pGraphParam);
static void DrawXLine(PDEV pDev, LINEDRAW* pLineDraw,
                      GRAPHPARAM* pGraphParam, PENSTYLE penstyle);
static void DrawYLine(PDEV pDev, LINEDRAW* pLineDraw,
                      GRAPHPARAM* pGraphParam, PENSTYLE penstyle);
#endif //SLOPELINE_SUPPORTED

static int DrawThinLine(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PLINEDATA pLineData);
static int DrawWideLine(PDEV pDev, PGRAPHPARAM pGraphParam, 
                         PLINEDATA pLineData);
/* The following functions are implemented in bitblt.c  */
static int BitBlt(PDEV pDstDev, PRECT pDestRect, PDEV pSrcDev,
                  int x, int y, PGRAPHPARAM pGraphParam);
static int MemBitBlt(PDEV pDstDev, PRECT pDestRect, PDEV pSrcDev,
                  int x, int y, PGRAPHPARAM pGraphParam);

/* The following functions are implemented in patblt.c  */
static void DrawPatHLine(PDEV pDev, int x1, int x2, int y, 
                      GRAPHPARAM* pGraphParam, PENSTYLE penstyle);
static void FillSolidRect(PDEV pDev, PRECT pRect, uint32 color, int32 rop);
static void MemFillSolidRect(PDEV pDev, PRECT pRect, uint32 color, int32 rop);
static int PatBlt(PDEV pDev, PRECT pRect, PGRAPHPARAM pGraphParam);
static int PatBltHorizonalLine(PDEV pDev, int x1, int x2, int y,
                      PGRAPHPARAM pGraphParam);


/* The following functions are implemented in dibitblt.c  */
static void DIBitBlt1(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                      int x, int y, PGRAPHPARAM pGraphParam);
static void MemDIBitBlt1(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
                      int x, int y, PGRAPHPARAM pGraphParam);
static void DIBitBlt4(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                      int x, int y, PGRAPHPARAM pGraphParam);
static void DIBitBlt8(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                      int x, int y, PGRAPHPARAM pGraphParam);
static void DIBitBlt16(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                      int x, int y, PGRAPHPARAM pGraphParam);
static void DIBitBlt24(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
                       int x, int y, PGRAPHPARAM pGraphParam);
static void DIBitBlt32(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
                       int x, int y, PGRAPHPARAM pGraphParam);
static void RealizePaletteColor(void* pPalette, uint32* pPhysColor, 
                                int count);
static int TextBlt(PDEV pDev, int x, int y, PTEXTBMP pTextBmp, 
                   PGRAPHPARAM pGraphParam);
static int DIBitBlt(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                    int x, int y, PGRAPHPARAM pGraphParam);

/* The following functions are implemented in strblt.c  */
/*
static int  StretchMono(PDEV pDev, int dest_startx, int dest_starty, 
                 int dest_width, int dest_height, PDIBITMAP pBitmap, 
                 int src_startx, int src_starty, int src_width, 
                 int src_height, PGRAPHPARAM pGraphParam);
*/
static int  Stretch1(PDEV pDev, PDIBITMAP pBitmap,
                 PGRAPHPARAM pGraphParam, PDISTRBLTPARAM pDIStretchParam);
static int  Stretch4(PDEV pDev, PDIBITMAP pBitmap,
                 PGRAPHPARAM pGraphParam, PDISTRBLTPARAM pDIStretchParam);
static int  Stretch8(PDEV pDev, PDIBITMAP pBitmap,
                 PGRAPHPARAM pGraphParam, PDISTRBLTPARAM pDIStretchParam);
static int  Stretch16(PDEV pDev, PDIBITMAP pBitmap,
                 PGRAPHPARAM pGraphParam, PDISTRBLTPARAM pDIStretchParam);
static int  Stretch24(PDEV pDev, PDIBITMAP pBitmap,
                 PGRAPHPARAM pGraphParam, PDISTRBLTPARAM pDIStretchParam);
static int  Stretch32(PDEV pDev, PDIBITMAP pBitmap,
                 PGRAPHPARAM pGraphParam, PDISTRBLTPARAM pDIStretchParam);

/*
static int  Stretch4(PDEV pDev, int dest_startx, int dest_starty, 
                 int dest_width, int dest_height, PDIBITMAP pBitmap, 
                 int src_startx, int src_starty, int src_width, 
                 int src_height, PGRAPHPARAM pGraphParam);
static int  Stretch8(PDEV pDev, int dest_startx, int dest_starty, 
                 int dest_width, int dest_height, PDIBITMAP pBitmap, 
                 int src_startx, int src_starty, int src_width, 
                 int src_height, PGRAPHPARAM pGraphParam);
static int Stretch16(PDEV pDev, int dest_startx, int dest_starty, 
                 int dest_width, int dest_height, PDIBITMAP pBitmap, 
                 int src_startx, int src_starty, int src_width, 
                 int src_height, PGRAPHPARAM pGraphParam);
static int  Stretch24(PDEV pDev, int dest_startx, int dest_starty, 
                 int dest_width, int dest_height, PDIBITMAP pBitmap, 
                 int src_startx, int src_starty, int src_width, 
                 int src_height, PGRAPHPARAM pGraphParam);
static int  Stretch32(PDEV pDev, int dest_startx, int dest_starty, 
                 int dest_width, int dest_height, PDIBITMAP pBitmap, 
                 int src_startx, int src_starty, int src_width, 
                 int src_height, PGRAPHPARAM pGraphParam);
*/
static int StretchBlt(PDEV pDstDev, PRECT pDestRect, PDEV pSrcDev, 
                        PRECT pSrcRect, PGRAPHPARAM pGraphParam);
static int DIStretchBlt(PDEV pDev, PRECT pDstRect, PDIBITMAP pBitmap, 
                        PRECT pSrcRect, PGRAPHPARAM pGraphParam);

static void GetStretchPatternLine(int nStep, int nShort, BYTE* p);
static void GetStretchPattern(PRECT pDestRect, PRECT pSrcRect, 
                              PSTRETCHPATTERN p);
static int GetSrcOffset(BYTE* pPattern, int dst_width, int src_width, 
                            int dst_offset);

#ifdef CIRCLE_SUPPORTED
/* The following functions are implemented in circle.c  */
static int DrawCircle(PDEV pDev, PGRAPHPARAM pGraphParam, 
                      PCIRCLEDATA pCircleData);
static int DrawThinCircle(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PCIRCLEDATA pCircleData);
static int DrawPatThinCircle(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PCIRCLEDATA pCircleData);
static int DrawWideCircle(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PCIRCLEDATA pCircleData);
static int DrawOuterCircle(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PCIRCLEDATA pCircleData);
#endif //CIRCLE_SUPPORTED


#ifdef POLYGON_SUPPORTED
/* The following functions are implemented in polygon.c  */
static int DrawPolygon(PDEV pDev, PGRAPHPARAM pGraphParam, 
                             PPOLYLINEDATA pPolyLineData);
static int DrawThinPolygon(PDEV pDev, PGRAPHPARAM pGraphParam, 
                              PPOLYLINEDATA pPolyLineData);
static int DrawWidePolygon(PDEV pDev, PGRAPHPARAM pGraphParam, 
                             PPOLYLINEDATA pPolyLineData);
static int FillPolygon(PDEV pDev, int count, POINT* pointtable, 
                     PGRAPHPARAM pGraphParam);
#endif //POLYGON_SUPPORTED

#ifdef ELLIPSE_SUPPORTED
/* The following functions are implemented in ellipse.c  */
static int DrawEllipse(PDEV pDev, PGRAPHPARAM pGraphParam, 
                      PELLIPSEDATA pEllipseData);
static int DrawThinEllipse(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEDATA pEllipseData);
static int DrawPatThinEllipse(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEDATA pEllipseData);
static int DrawWideEllipse(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEDATA pEllipseData);
static int DrawOuterEllipse(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEDATA pEllipseData);
#endif //ELLIPSE_SUPPORTED

#ifdef ELLIPSEARC_SUPPORTED
static int DrawEllipseArc(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEARCDATA pEllipseArcData);
static int DrawThinEllipseArc(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEARCDATA pEllipseArcData);
static int DrawPatThinEllipseArc(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEARCDATA pEllipseArcData);
static int DrawWideEllipseArc(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEARCDATA pEllipseArcData);
static int DrawPieOrChord(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEARCDATA pEllipseArcData);
#endif //ELLIPSEARC_SUPPORTED

#ifdef ROUNDRECT_SUPPORTED
static int DrawRoundRect(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PROUNDRECT pRoundRectData);
static int DrawThinRoundRect(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PROUNDRECT pRoundRectData);
static int DrawPatRoundRect(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PROUNDRECT pRoundRectData);
static int DrawWideRoundRect(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PROUNDRECT pRoundRectData);
#endif //ROUNDRECT_SUPPORTED

#endif //__DISPFUNC_H
