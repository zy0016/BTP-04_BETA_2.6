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


/*********************************************************************\
* Function	   DrawPatHLine
* Purpose      Draw style horizen line. 
               The style is given by the PENSTYLE struct.
* Params	   
    pDev        Pointer to display device structure.
    x1,x2,y     Drawing data.
    pGraphParam Pointer to display device parameter.
    penstyle    The pen style struct.
* Return	 	   
* Remarks	   
**********************************************************************/
static void DrawPatHLine(PDEV pDev, int x1, int x2, int y, 
                      GRAPHPARAM* pGraphParam, PENSTYLE penstyle)
{
    uint8*  pDst;
    int     pix_bytes;
    uint32  pixel;
    uint32  bkcolor, pencolor;
    uint32  penmask, penpattern, endmask;
    uint32  rop;
    ROPFUNC ropfunc;
    ROPFUNC ropDashfunc;
    int     i;

    ASSERT(pGraphParam != NULL);
    ASSERT(penstyle.count >= 1);
    if (pGraphParam->pBrush == NULL)
        return;

    rop = PRop3ToRop2(pGraphParam->rop);
    pDst = GETXYADDR(pDev, x1, y);
    pix_bytes = pDev->pix_bits / 8;

    if (x1 > x2)
    {
        EXCHANGE(x1, x2);
        pix_bytes = -pix_bytes;
    }
    else
    {
        pix_bytes = pix_bytes;
    }

    penmask = 0x80000000L;
    /* When the penstyle.count = 32, endmask will equal to penmask!!
    */
    endmask = penmask >> (penstyle.count - 1);
    endmask >>= 1;
    penpattern = penstyle.pattern;
    bkcolor = pGraphParam->bk_color;
//    pencolor = pGraphParam->pBrush->fgcolor;
    pencolor = pGraphParam->brush_color;

    ropfunc = GetRopFunc(rop);
    if (pGraphParam->bk_mode == BM_TRANSPARENT)
        ropDashfunc = GetRopFunc(ROP_DST);
    else
        ropDashfunc = ropfunc;

    for(i = x1; i <= x2; i++)
    {       
        if (penmask & penpattern)
        {
            pixel = ropfunc(pencolor, GETPIXEL(pDst));
        }
        else
        {
            pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
        }

        PUTPIXEL(pDst, pixel);
        pDst += pix_bytes;
        
        penmask >>= 1;
        if (penmask == endmask)
        {
            penmask = 0x80000000L;
        }
    }
}

/*********************************************************************\
* Function	 FillSolidRect  
* Purpose    Fill the specified rectangle using the specified color.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void FillSolidRect(PDEV pDev, PRECT pRect, uint32 color, int32 rop)
{
    if (rop > 16)
        rop = SRop3ToRop2(rop);

    MemFillSolidRect(pDev, pRect, color, rop);
}

/*********************************************************************\
* Function	   MemFillSolidRect
* Purpose      Fill a rect with color specified.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void MemFillSolidRect(PDEV pDev, PRECT pRect, uint32 color, 
                             int32 rop)
{
    ROPFUNC ropfunc;
    uint8   *pDst, *p;
    int     i, j;
    uint32  pixel;
    int     pix_bytes;
    FILLLINESTRUCT fillline;
    DWORD fillmode = 0;

    pDst = GETXYADDR(pDev, pRect->x1, pRect->y1);
    pix_bytes = pDev->pix_bits / 8;
    
    switch (rop)
    {
    case ROP_SRC :

#if (LDD_MODE == DDM_RGB24_RGB || LDD_MODE == DDM_RGB18_RGB)
#if (DISP_BIG_ENDIAN)
        fillmode = FILL_BIG_RGB;
#else
        fillmode = FILL_LITTLE_RGB;
#endif
#elif (LDD_MODE == DDM_RGB24_BGR || LDD_MODE == DDM_RGB18_BGR)
#if (DISP_BIG_ENDIAN)
        fillmode = FILL_BIG_BGR;
#else
        fillmode = FILL_LITTLE_BGR;
#endif
#endif
        GetFillLineType(pDst, color, pRect->x2 - pRect->x1, pix_bytes, 
            &fillline, ROP_SRC, fillmode);
        
        if (fillline.filllinefunc != NULL)
        {
            for (j = pRect->y1; j < pRect->y2; j++)
            {
                fillline.filllinefunc(&fillline);
                fillline.dst += pDev->line_bytes;
            }
        }
        else
        {
            for (j = pRect->y1; j < pRect->y2; j++)
            {
                p = pDst;
                
                for (i = pRect->x1; i < pRect->x2; i++)
                {
                    PUTPIXEL(p, color);
                    p += pix_bytes;
                }
                
                pDst += pDev->line_bytes;
            }
        }

        break;
        
    default :
        
        ropfunc = GetRopFunc(rop);
        
        for (j = pRect->y1; j < pRect->y2; j++)
        {
            p = pDst;
            
            for (i = pRect->x1; i < pRect->x2; i++)
            {
                pixel = ropfunc(color, GETPIXEL(p));
                PUTPIXEL(p, pixel);
                p += pix_bytes;
            }
            
            pDst += pDev->line_bytes;
        }
        
        break;
    }
}

/*********************************************************************\
* Function	   PatBlt
* Purpose      Block pattern fill.
* Params	   
    pDev        指向显示设备的指针；
    pRect       指向目的矩形的指针；
    pGraphParam 指向绘图参数结构的指针。
* Return	 	   
* Remarks	   
**********************************************************************/
static int PatBlt(PDEV pDev, PRECT pRect, PGRAPHPARAM pGraphParam)
{    
    RECT        rect;
    int         x1, x2, y1, y2;
    PPHYSBRUSH  pBrush;
    uint8       *pDst, *pSrc, *p1, *p2;
    int         i, j;
    uint32      pixel;
    uint32      rop;
    PENSTYLE    penstyle;
    int         x_offset, y_offset, s_scanw;
    int         pix_bytes;

    // 对要显示目的矩形进行裁剪
    CopyRect(&rect, pRect);
    ClipByDev(&rect, pDev);
    if (!IsValidRect(&rect))
        return -1;

    if ((pGraphParam->flags & GPF_CLIP_RECT) && 
        !ClipRect(&rect, &pGraphParam->clip_rect))
        return -1;
    
    if (!(pGraphParam->flags & GPF_BRUSH))
        pBrush = NULL;
    else 
        pBrush = pGraphParam->pBrush;
    
    // 空白刷子时，不做处理直接返回。
    if (pBrush != NULL && pBrush->style == BS_HOLLOW)
        return -1;

    _ExcludeCursor(pDev, &rect);

    x1 = rect.x1;
    y1 = rect.y1;
    x2 = rect.x2;
    y2 = rect.y2;

    rop = PRop3ToRop2(pGraphParam->rop);

    pDst = GETXYADDR(pDev, x1, y1);
    pix_bytes = pDev->pix_bits / 8;

    if (pGraphParam->bk_mode == BM_OPAQUE)
    {
        switch (rop)
        {
        case ROP_NDST : // for InvertRect
            
            for (j = y1; j < y2; j++)
            {
                p2 = pDst;

                for (i = x1; i < x2; i++)
                {
                    pixel = (~GETPIXEL(p2)) & 0x3FFFF ;
                    PUTPIXEL(p2, pixel);
                    p2 += pix_bytes;
                }

                pDst += pDev->line_bytes;
            }

            _UnexcludeCursor(pDev);

            return 0;

            break;

        case ROP_DST:

            _UnexcludeCursor(pDev);
            return 0;

            break;
        }
    }

	
	// 当上层要用某种颜色完全填充目的矩形时，可以把刷子置成空，就可免
	// 去创建一个新刷子的步骤。
    if (pBrush == NULL)
    {
        FillSolidRect(pDev, &rect, pGraphParam->bk_color, rop);

        _UnexcludeCursor(pDev);
        return 0;
    }

    switch (pBrush->style)
    {
    case BS_SOLID:  //用刷子颜色进行实填充

        FillSolidRect(pDev, &rect, pGraphParam->brush_color, rop);

        break;

    case BS_BITMAP:
        {
            MEMDEV memdev;
            int brWidth, brHeight;
            int nWidth, nHeight;
            RECT rcDst;
            int xFirstEnd;
            int pix_bytes;

            brWidth = pBrush->width;
            brHeight = pBrush->height;
            pix_bytes = pDev->pix_bits / 8;

            memdev.mode     = DEV_MEMORY;
            memdev.bmpmode  = pDev->bmpmode;
            memdev.width    = brWidth;
            memdev.height   = brHeight;
            memdev.line_bytes = (brWidth * pix_bytes * 8 + 31) / 32 * 4 ;
            memdev.pix_bits   = pDev->pix_bits;
            memdev.pdata      = pBrush->pPattern;

            x_offset = (x1 - pGraphParam->brush_org_x) % pBrush->width;
            if (x_offset < 0)
                x_offset += pBrush->width;

            y_offset = (y1 - pGraphParam->brush_org_y) % pBrush->height;
            if (y_offset < 0)
                y_offset += pBrush->height;


            nWidth = ((x2 - x1 - (brWidth - x_offset)) + brWidth - 1) / brWidth + 1;
            nHeight = ((y2 - y1 -(brHeight - y_offset)) + brHeight - 1) / brHeight + 1;

            if (x2 - x1 >= brWidth - x_offset)
                xFirstEnd = x1 + brWidth - x_offset;
            else
                xFirstEnd = x2;


            rcDst.y1 = y1;
            if (y2 - y1 >= brHeight - y_offset)
                rcDst.y2 = y1 + brHeight - y_offset;
            else
                rcDst.y2 = y2;

            rcDst.x1 = x1;
            rcDst.x2 = xFirstEnd;

            /* Bitblt the most left and top block.
            */
            BitBlt(pDev, &rcDst, (PDEV)&memdev, x_offset, y_offset, pGraphParam);
            
            /* The nWidth >=2 means the first line haven't finished and change 
            ** x value ,bitblt again without y change.
            */

            if (nWidth >= 2)
            {
                rcDst.x1 = rcDst.x2;
                rcDst.x2 += brWidth;
                for (i = 2; i < nWidth; i ++)
                {
                    BitBlt(pDev, &rcDst, (PDEV)&memdev, 0, y_offset, pGraphParam);
                    rcDst.x1 += brWidth;
                    rcDst.x2 += brWidth;
                }

                /* the milled part have finished ,then bitblt the last block
                ** of the first line.
                */
                rcDst.x2 = x2;
                BitBlt(pDev, &rcDst, (PDEV)&memdev, 0, y_offset, pGraphParam);
            }

            /* The first line have finished , then bitblt the following.
            ** Every line should do the same operation as above.
            */
            if (nHeight >= 2)
            {
                rcDst.y1 = rcDst.y2;
                rcDst.y2 += brHeight;

                for (j = 2; j < nHeight; j++)
                {
                    rcDst.x1 = x1;
                    rcDst.x2 = xFirstEnd;

                    BitBlt(pDev, &rcDst, (PDEV)&memdev, x_offset, 0, pGraphParam);

                    if (nWidth >= 2)
                    {
                        rcDst.x1 = rcDst.x2;
                        rcDst.x2 += brWidth;

                        for (i = 2; i < nWidth; i ++)
                        {
                            BitBlt(pDev, &rcDst, (PDEV)&memdev, 0, 0, pGraphParam);
                            rcDst.x1 += brWidth;
                            rcDst.x2 += brWidth;
                        }

                        rcDst.x2 = x2;
                        BitBlt(pDev, &rcDst, (PDEV)&memdev, 0, 0, pGraphParam);
                    }

                    rcDst.y1 += brHeight;
                    rcDst.y2 += brHeight;
                }

                /*Do the last line bitblt.
                */
                rcDst.y2 = y2;

                rcDst.x1 = x1;
                rcDst.x2 = xFirstEnd;

                BitBlt(pDev, &rcDst, (PDEV)&memdev, x_offset, 0, pGraphParam);

                if (nWidth >= 2)
                {
                    rcDst.x1 = rcDst.x2;
                    rcDst.x2 += brWidth;
                    
                    for (i = 2; i < nWidth; i ++)
                    {
                        BitBlt(pDev, &rcDst, (PDEV)&memdev, 0, 0, pGraphParam);
                        rcDst.x1 += brWidth;
                        rcDst.x2 += brWidth;
                    }
                    
                    rcDst.x2 = x2;
                    BitBlt(pDev, &rcDst, (PDEV)&memdev, 0, 0, pGraphParam);
                }
            }
        }

        break;

    case BS_PATTERN:
        /* The user defined pattern.
        ** The width of the pattern may be larger than 32,then it can't
        ** use the PENSTYLE struct to do the patblt horizenal line.
        ** So the procedure do it without calling other function. 
        */
        {
            int         pattern_line_bytes, x_bytes;
            BYTE        penmask, endmask;
            uint32      bkcolor, pencolor;
            ROPFUNC     ropfunc, ropDashfunc;
            int         pix_bytes;

            x_offset = (x1 - pGraphParam->brush_org_x) % pBrush->width;
            if (x_offset < 0)
                x_offset += pBrush->width;

            y_offset = (y1 - pGraphParam->brush_org_y) % pBrush->height;
            if (y_offset < 0)
                y_offset += pBrush->height;

            s_scanw = (pBrush->width + 31) / 32 * 4;
            pix_bytes = pDev->pix_bits / 8;

            pSrc = pBrush->pPattern + y_offset * s_scanw;

            /* pattern_line_bytes is the bytes used to store the pattern 
            ** width. It's not the bitmap line bytes.
            ** if width = 17, then pattern_line_bytes = 3, not 4;
            */
            pattern_line_bytes = (pBrush->width + 7) / 8;

            bkcolor = pGraphParam->bk_color;
            pencolor = pGraphParam->brush_color;

            ropfunc = GetRopFunc(pGraphParam->rop);
            if (pGraphParam->bk_mode == BM_TRANSPARENT)
                ropDashfunc = GetRopFunc(ROP_DST);
            else
                ropDashfunc = ropfunc;

            /* endmask is the mask of the last bytes of the pattern.
            ** Example: width = 18, then endmask = 00100000;
            */
            endmask = 0x80 >> (pBrush->width % 8);

            for (i = y1; i < y2; i++)
            {
                x_bytes = x_offset / 8;
                p1 = pSrc + x_bytes;
                p2 = pDst;
                penmask = 0x80 >> (x_offset % 8);

                for(j = x1; j < x2; j++)
                {       
                    if (penmask & *p1)
                    {
                        pixel = ropfunc(pencolor, GETPIXEL(pDst));
                    }
                    else
                    {
                        pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                    }

                    PUTPIXEL(p2, pixel);
                    p2 += pix_bytes;
        
                    penmask >>= 1;

                    /* One byte of pattern has been finished and go to the 
                    ** next bytes;
                    */
                    if (penmask == 0 && x_bytes < pattern_line_bytes)
                    {
                        penmask = 0x80;
                        x_bytes ++;
                        p1 ++;
                    }

                    /* reach the end of a pattern, then go back to the start
                    ** of the same line again!
                    */
                    if (x_bytes == pattern_line_bytes && penmask == endmask)
                    {
                        p1 = pSrc;
                        penmask = 0x80;
                        x_bytes = 0;
                    }
                }

                y_offset ++;
                if (y_offset >= pBrush->height)
                {
                    y_offset = 0;
                    pSrc = pBrush->pPattern;
                }
                else
                {
                    pSrc += s_scanw;
                }

                pDst += pDev->line_bytes;
            }
        }

        break;

    case BS_HATCHED:
        
        x_offset = (x1 - pGraphParam->brush_org_x) % pBrush->width;
        if (x_offset < 0)
            x_offset += pBrush->width;

        y_offset = (y1 - pGraphParam->brush_org_y) % pBrush->height;
        if (y_offset < 0)
            y_offset += pBrush->height;

        penstyle.count = pBrush->width;

        s_scanw = (pBrush->width + 7) / 8;
        ASSERT(s_scanw <= 4);

        pSrc = pBrush->pPattern + y_offset * s_scanw;
        for (i = y1; i < y2; i++)
        {
            uint32 penpattern = 0;
            int j;
            
            for (j = 0; j < s_scanw; j++)
            {
                penpattern |= (*(pSrc + j)) << ((3 - j) * 8);
            }
            if (x_offset > 0)
            {
                penstyle.pattern = 
                    GetOffsetPenStyle(penpattern, pBrush->width, x_offset);
            }
            else
            {
                penstyle.pattern = penpattern;
            }

            DrawPatHLine(pDev, x1, x2 -1, i, pGraphParam, penstyle);

            y_offset ++;
            if (y_offset >= pBrush->height)
            {
                y_offset = 0;
                pSrc = pBrush->pPattern;
            }
            else
            {
                pSrc += s_scanw;
            }
        }

        break;

    default:

        break;
    }

    _UnexcludeCursor(pDev);

    return 0;    
}

/*********************************************************************\
* Function	   PatBltHorizonalLine
* Purpose      Draw style horizen line. The style is given by the brush.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int PatBltHorizonalLine(PDEV pDev, int x1, int x2, int y,
                      PGRAPHPARAM pGraphParam)
{
    PPHYSBRUSH  pBrush;
    uint32      rop;

    if (x1 > x2)
        return 0;

    pBrush = pGraphParam->pBrush;
    rop = PRop3ToRop2(pGraphParam->rop);

    if (pBrush == NULL)
    {
        //fill with bkcolor
        if (pDev->mode != DEV_PHYSICAL)
        {
            DrawSolidHLine(pDev, x1, x2, y, rop, pGraphParam->bk_color);
        }
        else 
        {
            DrawSolidHLine(pDev, x1, x2, y, rop, pGraphParam->bk_color);
        }
        return 0;
    }

    if (pBrush->style == BS_NULL)
    {
        return 0;
    }    

    else if (pBrush->style == BS_SOLID || pBrush->pPattern == NULL)
    {
        DrawSolidHLine(pDev, x1, x2, y, rop, pGraphParam->brush_color);
    }

    else if (pBrush->style == BS_HATCHED)
    {
        int     j;
        uint32  penpattern = 0;
        uint8   *p;
        int     x_offset, y_offset, s_scanw;
        PENSTYLE    penstyle;

        x_offset = (x1 - pGraphParam->brush_org_x) % pBrush->width;
        if (x_offset < 0)
            x_offset += pBrush->width;

        y_offset = (y - pGraphParam->brush_org_y) % pBrush->height;
        if (y_offset < 0)
            y_offset += pBrush->height;

        penstyle.count = pBrush->width;

        s_scanw = (pBrush->width + 7) / 8;
        ASSERT(s_scanw <= 4);

        p = pGraphParam->pBrush->pPattern + y_offset * s_scanw;
        for (j = 0; j < s_scanw; j++)
        {
            penpattern |= (*(p + j)) << ((3 - j) * 8);
        }

        if (x_offset > 0)
        {
            penstyle.pattern = 
                GetOffsetPenStyle(penpattern, pBrush->width, x_offset);
        }
        else
        {
            penstyle.pattern = penpattern;
        }

        DrawPatHLine(pDev, x1, x2, y, pGraphParam, penstyle);
    }
    else if (pBrush->style == BS_PATTERN)
    {
        RECT rc;
        SetRect(&rc, x1, y, x2 + 1, y + 1);
        PatBlt(pDev, &rc, pGraphParam);
    }
    else if (pBrush->style == BS_BITMAP)
    {
        RECT rc;
        SetRect(&rc, x1, y, x2 + 1, y + 1);
        PatBlt(pDev, &rc, pGraphParam);
    }

    return 0;
}
