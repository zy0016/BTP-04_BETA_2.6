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
* Function     TextBlt
* Purpose      文本位图输出函数。
* Params       
* Return           
* Remarks      
**********************************************************************/
static int TextBlt(PDEV pDev, int x, int y, PTEXTBMP pTextBmp, 
                   PGRAPHPARAM pGraphParam)
{
    RECT rect;
    DIBITMAP bitmap;

    ASSERT(pTextBmp != NULL);

    rect.x1 = x;
    rect.y1 = y;
    rect.x2 = x + pTextBmp->width;
    rect.y2 = y + pTextBmp->height;

    bitmap.type = BMP_MONO;
    bitmap.width = pTextBmp->width;
    bitmap.height = -pTextBmp->height;
    bitmap.palette_entries = 2;
    bitmap.bmp_size = 0;
    bitmap.pBuffer = pTextBmp->data;
    bitmap.pPalette = NULL;

    return DIBitBlt(pDev, &rect, &bitmap, 0, 0, pGraphParam);
}

/*********************************************************************\
* Function   DIBitBlt  
* Purpose    Block transfer between display debice and device independent
             bitmap  
* Params       
    pDev        Pointer to display device.
    pDestRect   Destination rectangle.
    pBitmap     Pointer to device independent bitmap.
    x , y       start point of source device.
    pGraphParam pointer to graphics operation parameter group.
* Return           
* Remarks      
**********************************************************************/
static int DIBitBlt(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                    int x, int y, PGRAPHPARAM pGraphParam)
{
    RECT dst_rect, src_rect, SrcClipRc;

    ASSERT(pDev != NULL && pDestRect != NULL &&
        pBitmap != NULL && pGraphParam != NULL);

    CopyRect(&dst_rect, pDestRect);

    //据目的设备、剪裁矩形对目的矩形裁剪
    ClipByDev(&dst_rect, pDev);
    if (!IsValidRect(&dst_rect))
        return -1;

    if ((pGraphParam->flags & GPF_CLIP_RECT) &&
        !ClipRect(&dst_rect, &pGraphParam->clip_rect))
        return -1;

    _ExcludeCursor(pDev, &dst_rect);

    src_rect.x1 = x;
    src_rect.y1 = y;
    src_rect.x2 = x + (pDestRect->x2 - pDestRect->x1);
    src_rect.y2 = y + (pDestRect->y2 - pDestRect->y1);

    SrcClipRc.x1 = SrcClipRc.y1 = 0;
    SrcClipRc.x2 = pBitmap->width;
    if (pBitmap->height > 0)
        SrcClipRc.y2 = pBitmap->height;
    else
        SrcClipRc.y2 = -pBitmap->height;

    if (!ClipRect(&src_rect, &SrcClipRc))
        return -1;

    //调整目的矩形、源矩形 
    dst_rect.x1 -= pDestRect->x1;
    dst_rect.y1 -= pDestRect->y1;
    dst_rect.x2 -= pDestRect->x1;
    dst_rect.y2 -= pDestRect->y1;

    src_rect.x1 -= x;
    src_rect.y1 -= y;
    src_rect.x2 -= x;
    src_rect.y2 -= y;

    //获得目的和源矩形的交集
    if (!ClipRect(&dst_rect, &src_rect))
        return -1;

    src_rect.x1 = dst_rect.x1;
    src_rect.y1 = dst_rect.y1;

    src_rect.x1 += x;
    src_rect.y1 += y;

    dst_rect.x1 += pDestRect->x1;
    dst_rect.y1 += pDestRect->y1;
    dst_rect.y2 += pDestRect->y1;
    dst_rect.x2 += pDestRect->x1;

    switch(pBitmap->type)
    {
    case BMP_MONO :

        DIBitBlt1(pDev, &dst_rect, pBitmap, src_rect.x1, src_rect.y1, 
            pGraphParam);
        break;

    case BMP_16:

        DIBitBlt4(pDev, &dst_rect, pBitmap, src_rect.x1, src_rect.y1, 
            pGraphParam);

        break;
        
    case BMP_256 :

        DIBitBlt8(pDev, &dst_rect, pBitmap, src_rect.x1, src_rect.y1, 
            pGraphParam);

        break;

    case BMP_RGB16:

        DIBitBlt16(pDev, &dst_rect, pBitmap, src_rect.x1, src_rect.y1, 
            pGraphParam);

        break;

    case BMP_RGB24:

        DIBitBlt24(pDev, &dst_rect, pBitmap, src_rect.x1, src_rect.y1, 
            pGraphParam);

        break;

    case BMP_RGB32:

        DIBitBlt32(pDev, &dst_rect, pBitmap, src_rect.x1, src_rect.y1, 
            pGraphParam);

        break;
    }

    _UnexcludeCursor(pDev);

    return 0;
}

/*********************************************************************\
* Function    DIBitBlt1 
* Purpose     输出指定的单色位图到指定的设备。 
* Params       
* Return           
* Remarks      
**********************************************************************/
static void DIBitBlt1(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
                      int x, int y, PGRAPHPARAM pGraphParam)
{
    MemDIBitBlt1(pDev, pDestRect, pBitmap, x, y, pGraphParam);
}

/*********************************************************************\
* Function     MemDIBitBlt1
* Purpose      Do mono bitblt of memory device or no GE supporting 
               physical device.
* Params       
* Return           
* Remarks      
**********************************************************************/
static void MemDIBitBlt1(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
                      int x, int y, PGRAPHPARAM pGraphParam)
{
    uint32  PhysColors[2];
    int     s_scanw, d_scanw;
    int     width, height, i;
    uint8   *pSrc, *pDst;
    uint8   *p1, *p2;  
    uint8   mask;
    uint32  pixel;
    uint32  rop;
    int     bk_mode;
    ROPFUNC ropfunc;
    int     pix_bytes;

    s_scanw = (pBitmap->width + 31) / 32 * 4;

    // Realize physical palette
    if (pBitmap->pPalette)
        RealizePaletteColor(pBitmap->pPalette, PhysColors, 2);
    else
    {
        PhysColors[0] = pGraphParam->bk_color;
        PhysColors[1] = pGraphParam->text_color;
    }

    width = pDestRect->x2 - pDestRect->x1;
    height = pDestRect->y2 - pDestRect->y1;
    
    pSrc = (uint8*)pBitmap->pBuffer + y * s_scanw + x / 8; 
    if (pBitmap->height > 0)
    {
        pSrc = (uint8 *)pBitmap->pBuffer 
            + (pBitmap->height - y - 1) * s_scanw + x / 8;
        s_scanw = -s_scanw;
    }

    rop = SRop3ToRop2(pGraphParam->rop);
    bk_mode = pGraphParam->bk_mode;

    d_scanw = pDev->line_bytes;
    pDst = GETXYADDR(pDev, pDestRect->x1, pDestRect->y1);
    pix_bytes = pDev->pix_bits / 8;
    
    switch (rop)
    {
    case ROP_SRC : 
    case ROP_NSRC :

        if (rop == ROP_NSRC)
        {
            PhysColors[0] = ~PhysColors[0];
            PhysColors[1] = ~PhysColors[1];
        }

#if ((USELINECOPY) && (LDD_MODE == DDM_RGB16_565 || LDD_MODE == DDM_RGB12_4440))
        {
            COPYLINE1TO16STRUCT p;

            Get1to16CopyLineType(pDst, pSrc, x, width, &p, PhysColors, 
                bk_mode == BM_TRANSPARENT);
            while (height--)
            {
                p.copylinefunc(pDst, pSrc, &p, PhysColors);
                pDst += d_scanw;
                pSrc += s_scanw;
            }
        }
#elif ((USELINECOPY) && (LDD_MODE == DDM_RGB24_RGB || LDD_MODE == DDM_RGB24_BGR))
        {
            COPYLINE1TO24STRUCT p;
            
            Get1to24CopyLineType(pDst, pSrc, x, width, &p, PhysColors, 
                bk_mode == BM_TRANSPARENT);
            while (height--)
            {
                p.copylinefunc(pDst, pSrc, &p, PhysColors);
                pDst += d_scanw;
                pSrc += s_scanw;
            }
        }
#else
        if (bk_mode == BM_TRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                mask = 0x80 >> (x & 7);
                
                for (i = 0; i < width; i++, mask >>= 1)
                {
                    if (mask == 0)
                    {
                        p1++;
                        mask = 0x80;
                    }
                    
                    if (*p1 & mask)
                        PUTPIXEL(p2, PhysColors[1]);

                    p2 += pix_bytes;
                }
                
                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else    // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                mask = 0x80 >> (x & 7);
                
                for (i = 0; i < width; i++, mask >>= 1)
                {
                    if (mask == 0)
                    {
                        p1++;
                        mask = 0x80;
                    }

                    if (*p1 & mask)
                    {
                        PUTPIXEL(p2, PhysColors[1]);
                    }
                    else
                    {
                        PUTPIXEL(p2, PhysColors[0]);
                    }

                    p2 += pix_bytes;
                }
                
                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
#endif

        break;

    default :

        ropfunc = GetRopFunc(rop);

        if (bk_mode == BM_TRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                mask = 0x80 >> (x & 7);
                
                for (i = 0; i < width; i++, mask >>= 1)
                {
                    if (mask == 0)
                    {
                        p1++;
                        mask = 0x80;
                    }
                    
                    if (*p1 & mask)
                    {
                        pixel = ropfunc(PhysColors[1], GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                    }

                    p2 += pix_bytes;
                }
                
                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else    // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                mask = 0x80 >> (x & 7);
                
                for (i = 0; i < width; i++, mask >>= 1)
                {
                    if (mask == 0)
                    {
                        p1++;
                        mask = 0x80;
                    }

                    if (*p1 & mask)
                        pixel = PhysColors[1];
                    else
                        pixel = PhysColors[0];

                    pixel = ropfunc(pixel, GETPIXEL(p2));
                    PUTPIXEL(p2, pixel);

                    p2 += pix_bytes;
                }
                
                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }

        break;
    }
}

/*********************************************************************\
* Function    DIBitBlt4 
* Purpose     输出指定的16色位图到指定的设备。 
* Params       
* Return           
* Remarks      
**********************************************************************/
static void DIBitBlt4(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                      int x, int y, PGRAPHPARAM pGraphParam)
{
    static uint32 PhysColors[16];
    int     s_scanw, s_bytes, d_scanw;
    int     width, height, i;
    uint8   *p1, *p2;
    uint8   *pSrc, *pDst;
    uint32  pixel;
    uint32  rop;
    int     bk_mode;
    ROPFUNC ropfunc;
    int     pix_bytes;

    s_scanw = (pBitmap->width * 4 + 31) / 32 * 4;

    // Realize physical palette
    RealizePaletteColor(pBitmap->pPalette, PhysColors, 16);

    width = pDestRect->x2 - pDestRect->x1;
    height = pDestRect->y2 - pDestRect->y1;
    
    if (pBitmap->height > 0)
    {
        pSrc = (uint8 *)pBitmap->pBuffer 
            + (pBitmap->height - y - 1) * s_scanw + x * 4 / 8;
        s_scanw = -s_scanw;
    }
    else
        pSrc = (uint8*)pBitmap->pBuffer + y * s_scanw + x * 4 / 8; 

    rop = SRop3ToRop2(pGraphParam->rop);
    bk_mode = pGraphParam->bk_mode;

    d_scanw = pDev->line_bytes;
    pDst = GETXYADDR(pDev, pDestRect->x1, pDestRect->y1);
    pix_bytes = pDev->pix_bits / 8;

    s_bytes = (width + 1) / 2;
    if ((x & 1) && !(width & 1))
        s_bytes += 1;

    switch (rop)
    {
    case ROP_SRC :

        if (bk_mode == BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                // process first byte
                if (x & 1)
                {
                    pixel = PhysColors[*p1 & 0x0F];
                    if (pixel != (uint32)pGraphParam->bk_color)
                        PUTPIXEL(p2, pixel);
                    p2 += pix_bytes;
                }
                else
                {
                    // process high 4 bits in source
                    pixel = PhysColors[*p1 >> 4];
                    if (pixel != (uint32)pGraphParam->bk_color)
                        PUTPIXEL(p2, pixel);
                    p2 += pix_bytes;
                    
                    // process low 4 bits in source
                    pixel = PhysColors[*p1 & 0x0F];
                    if (pixel != (uint32)pGraphParam->bk_color)
                        PUTPIXEL(p2, pixel);
                    p2 += pix_bytes;
                }
                
                p1++;
                
                if (s_bytes != 1)
                {
                    // process middle bytes
                    for (i = 0; i < s_bytes - 2; i++)
                    {
                        // process high 4 bits in source
                        pixel = PhysColors[*p1 >> 4];
                        if (pixel != (uint32)pGraphParam->bk_color)
                            PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;
                        
                        // process low 4 bits in source
                        pixel = PhysColors[*p1 & 0x0F];
                        if (pixel != (uint32)pGraphParam->bk_color)
                            PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;
                        
                        p1++;
                    }
                    
                    // process last byte
                    if (!((x + width - 1) & 1))
                    {
                        // process high 4 bits in source
                        pixel = PhysColors[*p1 >> 4];
                        if (pixel != (uint32)pGraphParam->bk_color)
                            PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;
                    }
                    else
                    {
                        // process high 4 bits in source
                        pixel = PhysColors[*p1 >> 4];
                        if (pixel != (uint32)pGraphParam->bk_color)
                            PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;
                        
                        // process low 4 bits in source
                        pixel = PhysColors[*p1 & 0x0F];
                        if (pixel != (uint32)pGraphParam->bk_color)
                            PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;
                    }
                }
                
                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                // process first byte
                if (x & 1)
                {
                    pixel = PhysColors[*p1 & 0x0F];
                    PUTPIXEL(p2, pixel);
                    p2 += pix_bytes;
                }
                else
                {
                    // process high 4 bits in source
                    pixel = PhysColors[*p1 >> 4];
                    PUTPIXEL(p2, pixel);
                    p2 += pix_bytes;
                    
                    // process low 4 bits in source
                    pixel = PhysColors[*p1 & 0x0F];
                    PUTPIXEL(p2, pixel);
                    p2 += pix_bytes;
                }

                p1++;
                
                if (s_bytes != 1)
                {
                    // process middle bytes
                    for (i = 0; i < s_bytes - 2; i++)
                    {
                        // process high 4 bits in source
                        pixel = PhysColors[*p1 >> 4];
                        PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;
                        
                        // process low 4 bits in source
                        pixel = PhysColors[*p1 & 0x0F];
                        PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;

                        p1++;
                    }
                    
                    // process last byte
                    if (!((x + width - 1) & 1))
                    {
                        // process high 4 bits in source
                        pixel = PhysColors[*p1 >> 4];
                        PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;
                    }
                    else
                    {
                        // process high 4 bits in source
                        pixel = PhysColors[*p1 >> 4];
                        PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;
                        
                        // process low 4 bits in source
                        pixel = PhysColors[*p1 & 0x0F];
                        PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;
                    }
                }
                
                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }


        break;

    default :

        ropfunc = GetRopFunc(rop);

        if (bk_mode == BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                // process first byte
                if (x & 1)
                {
                    pixel = PhysColors[*p1 & 0x0F];
                    if (pixel != (uint32)pGraphParam->bk_color)
                    {
                        pixel = ropfunc(pixel, GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                    }
                    p2 += pix_bytes;
                }
                else
                {
                    // process high 4 bits in source
                    pixel = PhysColors[*p1 >> 4];
                    if (pixel != (uint32)pGraphParam->bk_color)
                    {
                        pixel = ropfunc(pixel, GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                    }
                    p2 += pix_bytes;
                    
                    // process low 4 bits in source
                    pixel = PhysColors[*p1 & 0x0F];
                    if (pixel != (uint32)pGraphParam->bk_color)
                    {
                        pixel = ropfunc(pixel, GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                    }
                    p2 += pix_bytes;
                }

                p1++;
                
                if (s_bytes != 1)
                {
                    // process middle bytes
                    for (i = 0; i < s_bytes - 2; i++)
                    {
                        // process high 4 bits in source
                        pixel = PhysColors[*p1 >> 4];
                        if (pixel != (uint32)pGraphParam->bk_color)
                        {
                            pixel = ropfunc(pixel, GETPIXEL(p2));
                            PUTPIXEL(p2, pixel);
                        }
                        p2 += pix_bytes;
                        
                        // process low 4 bits in source
                        pixel = PhysColors[*p1 & 0x0F];
                        if (pixel != (uint32)pGraphParam->bk_color)
                        {
                            pixel = ropfunc(pixel, GETPIXEL(p2));
                            PUTPIXEL(p2, pixel);
                        }
                        p2 += pix_bytes;

                        p1++;
                    }
                    
                    // process last byte
                    if (!((x + width - 1) & 1))
                    {
                        pixel = PhysColors[*p1 >> 4];
                        if (pixel != (uint32)pGraphParam->bk_color)
                        {
                            pixel = ropfunc(pixel, GETPIXEL(p2));
                            PUTPIXEL(p2, pixel);
                        }
                        p2 += pix_bytes;
                    }
                    else
                    {
                        // process high 4 bits in source
                        pixel = PhysColors[*p1 >> 4];
                        if (pixel != (uint32)pGraphParam->bk_color)
                        {
                            pixel = ropfunc(pixel, GETPIXEL(p2));
                            PUTPIXEL(p2, pixel);
                        }
                        p2 += pix_bytes;
                        
                        // process low 4 bits in source
                        pixel = PhysColors[*p1 & 0x0F];
                        if (pixel != (uint32)pGraphParam->bk_color)
                        {
                            pixel = ropfunc(pixel, GETPIXEL(p2));
                            PUTPIXEL(p2, pixel);
                        }
                        p2 += pix_bytes;
                    }
                }
                
                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else  // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                // process first byte
                if (x & 1)
                {
                    pixel = ropfunc(PhysColors[*p1 & 0x0F], GETPIXEL(p2));
                    PUTPIXEL(p2, pixel);
                    p2 += pix_bytes;
                }
                else
                {
                    // process high 4 bits in source
                    pixel = ropfunc(PhysColors[*p1 >> 4], GETPIXEL(p2));
                    PUTPIXEL(p2, pixel);
                    p2 += pix_bytes;
                    
                    // process low 4 bits in source
                    pixel = ropfunc(PhysColors[*p1 & 0x0F], GETPIXEL(p2));
                    PUTPIXEL(p2, pixel);
                    p2 += pix_bytes;
                }

                p1++;
                
                if (s_bytes != 1)
                {
                    // process middle bytes
                    for (i = 0; i < s_bytes - 2; i++)
                    {
                        // process high 4 bits in source
                        pixel = ropfunc(PhysColors[*p1 >> 4], GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;
                        
                        // process low 4 bits in source
                        pixel = ropfunc(PhysColors[*p1 & 0x0F], 
                            GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;

                        p1++;
                    }
                    
                    // process last byte
                    if (!((x + width - 1) & 1))
                    {
                        pixel = ropfunc(PhysColors[*p1 >> 4], GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;
                    }
                    else
                    {
                        // process high 4 bits in source
                        pixel = ropfunc(PhysColors[*p1 >> 4], GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;
                        
                        // process low 4 bits in source
                        pixel = ropfunc(PhysColors[*p1 & 0x0F], 
                            GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                        p2 += pix_bytes;
                    }
                }
                
                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }

        break;
    }
}

/*********************************************************************\
* Function     DIBitBlt8
* Purpose      输出指定的256色位图到指定的设备。 
* Params       
* Return           
* Remarks      
**********************************************************************/
static void DIBitBlt8(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
                      int x, int y, PGRAPHPARAM pGraphParam)
{
    static uint32 PhysColors[256];
    int     s_scanw, d_scanw;
    int     width, height, i;
    uint8   *pSrc, *pDst;
    uint8   *p1, *p2;
    uint32  pixel;
    uint32  rop;
    int     bk_mode;
    ROPFUNC ropfunc;
    int     pix_bytes;

    s_scanw = (pBitmap->width * 8 + 31) / 32 * 4;

    width = pDestRect->x2 - pDestRect->x1;
    height = pDestRect->y2 - pDestRect->y1;
    
    pSrc = (uint8*)pBitmap->pBuffer;

    if (pBitmap->height > 0)
    {
        pSrc += (pBitmap->height - y - 1) * s_scanw + x;
        s_scanw = -s_scanw;
    }
    else
        pSrc += y * s_scanw + x;
    
    rop = SRop3ToRop2(pGraphParam->rop);
    bk_mode = pGraphParam->bk_mode;

    d_scanw = pDev->line_bytes;
    pDst = GETXYADDR(pDev, pDestRect->x1, pDestRect->y1);
    pix_bytes = pDev->pix_bits / 8;

    /* AlphaDIBBlend函数的调用功能实现，只使用SCA
    ** BlendFunction非空，而且SCA不是255。
    ** 当SCA=255时，不存在alpha操作，直接贴图。
    */
    if (bk_mode == BM_ALPHATRANSPARENT
        && pGraphParam->pBlend != NULL
        && pGraphParam->pBlend->BlendFlags == 0 
        && pGraphParam->pBlend->BlendOp == AC_SRC_OVER
        && pGraphParam->pBlend->SourceConstantAlpha != 0xFF)
    {
        uint32  dred, dgreen, dblue;
        uint32  sred, sgreen, sblue;
        RGBQUAD *pPalette = pBitmap->pPalette;
        RGBQUAD *p3;

        uint32   uSCA = pGraphParam->pBlend->SourceConstantAlpha;

        /* 完全透明 */
        if (uSCA == 0)
            return ;
        
        /* 只使用SCA */
        while (height--)
        {
            p1 = pSrc;
            p2 = pDst;

            for (i = 0; i < width; i++)
            {
                p3 = &pPalette[*p1];
                sred = p3->R;
                sgreen = p3->G;
                sblue = p3->B;

                GETRGBBYTE(p2, dred, dgreen, dblue);

                //dred = (sred * salpha + (255 - salpha) * dred) / 255;
                //dred = dred + (sred - dred) * salpha / 255;

                INT8_255DIVIDED(sred * uSCA + (255 - uSCA) * dred, dred);
                INT8_255DIVIDED(sgreen * uSCA + (255 - uSCA) * dgreen, dgreen);
                INT8_255DIVIDED(sblue * uSCA + (255 - uSCA) * dblue, dblue);

                pixel = REALIZECOLOR(dred, dgreen, dblue);
                PUTPIXEL(p2, pixel);

                p2 += pix_bytes;
            }

            pSrc += s_scanw;
            pDst += d_scanw;
        }

        return;
    }

    // Realize physical palette
    RealizePaletteColor(pBitmap->pPalette, PhysColors, 256);

    switch (rop)
    {
    case ROP_SRC :

        if (bk_mode == BM_NEWTRANSPARENT)
        {
            /* Added on 20030325 for the index transpare display 
            ** Only in 256 colors bitmap 
            */
            if (((uint32)pGraphParam->bk_color & 0xFF000000) != 0)
            {
                int index;

                index = pGraphParam->bk_color & 0x000000FF;
                while (height--)
                {
                    p1 = pSrc;
                    p2 = pDst;
                    
                    for (i = 0; i < width; i++)
                    {
                        if (*p1 != index)
                            PUTPIXEL(p2, PhysColors[*p1]);
                        
                        p1++;
                        p2 += pix_bytes;
                    }
                    
                    pSrc += s_scanw;
                    pDst += d_scanw;
                }
            }
            else
            {
                while (height--)
                {
                    p1 = pSrc;
                    p2 = pDst;
                    
                    for (i = 0; i < width; i++)
                    {
                        pixel = PhysColors[*p1];
                        if (pixel != (uint32)pGraphParam->bk_color)
                            PUTPIXEL(p2, pixel);
                        
                        p1++;
                        p2 += pix_bytes;
                    }
                    
                    pSrc += s_scanw;
                    pDst += d_scanw;
                }
            }
        }
        else  // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = PhysColors[*p1];
                    PUTPIXEL(p2, pixel);

                    p1++;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }

        break;

    default :

        ropfunc = GetRopFunc(rop);

        if (bk_mode == BM_NEWTRANSPARENT)
        {
            /* Added on 20030325 for the index transpare display 
            ** Only in 256 colors bitmap 
            */
            if (((uint32)pGraphParam->bk_color & 0xFF000000) != 0)
            {
                int index;

                index = pGraphParam->bk_color & 0x000000FF;
                while (height--)
                {
                    p1 = pSrc;
                    p2 = pDst;
                    
                    for (i = 0; i < width; i++)
                    {
                        if (*p1 != index)
                        {
                            pixel = PhysColors[*p1];
                            pixel = ropfunc(pixel, GETPIXEL(p2));
                            PUTPIXEL(p2, pixel);
                        }
                        
                        p1++;
                        p2 += pix_bytes;
                    }
                    
                    pSrc += s_scanw;
                    pDst += d_scanw;
                }
            }
            else
            {
                while (height--)
                {
                    p1 = pSrc;
                    p2 = pDst;
                    
                    for (i = 0; i < width; i++)
                    {
                        pixel = PhysColors[*p1];
                        if (pixel != (uint32)pGraphParam->bk_color)
                        {
                            pixel = ropfunc(pixel, GETPIXEL(p2));
                            PUTPIXEL(p2, pixel);
                        }
                        
                        p1++;
                        p2 += pix_bytes;
                    }
                    
                    pSrc += s_scanw;
                    pDst += d_scanw;
                }
            }
        }
        else  // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = ropfunc(PhysColors[*p1], GETPIXEL(p2));
                    PUTPIXEL(p2, pixel);

                    p1++;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }

        break;
    }
}

#if (LDD_MODE == DDM_RGB12_4440)
#define Realize12BitColor(p) (((DWORD)(*(p)) << 8) | (DWORD)(*(p + 1)))
/*********************************************************************\
* Function     DIBitBlt12
* Purpose      输出指定的16位彩色位图到指定的设备。
* Params       
* Return           
* Remarks      
**********************************************************************/
static void DIBitBlt12(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
                       int x, int y, PGRAPHPARAM pGraphParam)
{
    int     s_scanw, d_scanw;
    int     width, height, i;
    uint8   *pSrc, *pDst;
    uint8   *p1, *p2;
    uint32  pixel;
    uint32  rop;
    int     bk_mode;
    ROPFUNC ropfunc;
    int     pix_bytes;
    
    s_scanw = (pBitmap->width * 16 + 31) / 32 * 4;

    width = pDestRect->x2 - pDestRect->x1;
    height = pDestRect->y2 - pDestRect->y1;
    
    pSrc = (uint8*)pBitmap->pBuffer;

    if (pBitmap->height > 0)
    {
        pSrc += (pBitmap->height - y - 1) * s_scanw + x * 2;
        s_scanw = -s_scanw;
    }
    else
        pSrc += y * s_scanw + x * 2;
    
    rop = SRop3ToRop2(pGraphParam->rop);
    bk_mode = pGraphParam->bk_mode;

    d_scanw = pDev->line_bytes;
    pDst = GETXYADDR(pDev, pDestRect->x1, pDestRect->y1);
    pix_bytes = pDev->pix_bits / 8;

#if (USELINECOPY)
    if (pGraphParam->bk_mode != BM_NEWTRANSPARENT)
    {
        COPYLINESTRUCT p;

        GetCopyLineType(pDst, pSrc, width * pix_bytes, &p, COPYLINE_L2R, rop);

        while (height--)
        {
            p.copylinefunc(&p);
            p.dst += d_scanw;
            p.src += s_scanw;
        }
        
        return;
    }
#endif
    
    switch (rop)
    {
    case ROP_SRC :

#if (!USELINECOPY)
        if (pGraphParam->bk_mode != BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                for (i = 0; i < width; i++)
                {
                    pixel = Realize12BitColor(p1);
                    PUTPIXEL(p2, pixel);
                    
                    p1 += 2;
                    p2 += pix_bytes;
                }
                
                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else
#endif
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                for (i = 0; i < width; i++)
                {
                    pixel = Realize12BitColor(p1);
                    
                    if (pixel != (uint32)pGraphParam->bk_color)
                        PUTPIXEL(p2, pixel);
                    
                    p1 += 2;
                    p2 += pix_bytes;
                }
                
                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }

        break;

    default :

        ropfunc = GetRopFunc(rop);

#if (!USELINECOPY)
        if (pGraphParam->bk_mode != BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                for (i = 0; i < width; i++)
                {
                    pixel = Realize12BitColor(p1);

                    pixel = ropfunc(pixel, GETPIXEL(p2));
                    PUTPIXEL(p2, pixel);
                    
                    p1 += 2;
                    p2 += pix_bytes;
                }
                
                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else
#endif
        while (height--)
        {
            p1 = pSrc;
            p2 = pDst;

            for (i = 0; i < width; i++)
            {
                pixel = Realize12BitColor(p1);

                if (pixel != (uint32)pGraphParam->bk_color)
                {
                    pixel = ropfunc(pixel, GETPIXEL(p2));
                    PUTPIXEL(p2, pixel);
                }

                p1 += 2;
                p2 += pix_bytes;
            }

            pSrc += s_scanw;
            pDst += d_scanw;
        }
        
        break;
    }
    
    return;
}
#endif //LDD_MODE == DDM_RGB12_4440

/* WWH modifed on 2004/02/06
** Fixed a bug in the GETRGBBYTE16_FROMMASK
*/
static void GetRGBAMask(int *lShift, int *rShift, int *len, DWORD dwMask)
{
    int shift;

    if (dwMask == 0)
    {
        *lShift = 0;
        *rShift = 0;
        *len = 0;
        return;
    }

    for (shift = 0; !(dwMask & 1); shift++)
        dwMask >>= 1;

    *rShift = shift;

    for (shift = 0; dwMask & 1; shift++)
        dwMask >>= 1;

    *len = shift;
    *lShift = 8 - shift;

    return;
}

#define GETRGBBYTE_FROMMASK(pixel, dwMask, LShift, RShift, len, color)  \
{   \
    color = (((pixel) & (dwMask)) >> (RShift)) << (LShift);  \
    color = color | (color >> (len));   \
}

#define GETRGBBYTE16_FROMMASK GETRGBBYTE_FROMMASK

#define RGB15TORGB16(pixel) ((((pixel) << 1) & 0xFFC0) | ((pixel) & 0x003F))
static void DIBitBlt15To16(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
                       int x, int y, PGRAPHPARAM pGraphParam)
{
    int     s_scanw, d_scanw;
    int     width, height, i;
    uint8   *pSrc, *pDst;
    uint8   *p1, *p2;
    uint32  pixel;
    uint32  rop;
    int     bk_mode;
    ROPFUNC ropfunc;
    uint32  dred, dgreen, dblue;
    uint32  sred, sgreen, sblue;
    int     pix_bytes;
    
    s_scanw = (pBitmap->width * 16 + 31) / 32 * 4;

    width = pDestRect->x2 - pDestRect->x1;
    height = pDestRect->y2 - pDestRect->y1;
    
    pSrc = (uint8*)pBitmap->pBuffer;

    if (pBitmap->height > 0)
    {
        pSrc += (pBitmap->height - y - 1) * s_scanw + x * 2;
        s_scanw = -s_scanw;
    }
    else
        pSrc += y * s_scanw + x * 2;
    
    rop = SRop3ToRop2(pGraphParam->rop);
    bk_mode = pGraphParam->bk_mode;

    d_scanw = pDev->line_bytes;
    pDst = GETXYADDR(pDev, pDestRect->x1, pDestRect->y1);
    pix_bytes = pDev->pix_bits / 8;

    /* AlphaDIBBlend函数的调用功能实现，只使用SCA
    ** BlendFunction非空，而且SCA不是255。
    ** 当SCA=255时，不存在alpha操作，直接贴图。
    */
    if (bk_mode == BM_ALPHATRANSPARENT
        && pGraphParam->pBlend != NULL
        && pGraphParam->pBlend->BlendFlags == 0 
        && pGraphParam->pBlend->BlendOp == AC_SRC_OVER
        && pGraphParam->pBlend->SourceConstantAlpha != 0xFF)
    {
        uint32   uSCA = pGraphParam->pBlend->SourceConstantAlpha;

        /* 完全透明 */
        if (uSCA == 0)
            return ;
        
        /* 只使用SCA */
        while (height--)
        {
            p1 = pSrc;
            p2 = pDst;

            for (i = 0; i < width; i++)
            {
                pixel = GET16_LITTLE(p1);//(*p1) | (*(p1 + 1) << 8);
                sred = GETRBYTE16_555(pixel);
                sgreen = GETGBYTE16_555(pixel);
                sblue = GETBBYTE16_555(pixel);
                
                GETRGBBYTE(p2, dred, dgreen, dblue);
                
                INT8_255DIVIDED(sred * uSCA + (255 - uSCA) * dred, dred);
                INT8_255DIVIDED(sgreen * uSCA + (255 - uSCA) * dgreen, dgreen);
                INT8_255DIVIDED(sblue * uSCA + (255 - uSCA) * dblue, dblue);
                
                pixel = REALIZECOLOR(dred, dgreen, dblue);
                PUTPIXEL(p2, pixel);

                p2 += pix_bytes;
            }

            pSrc += s_scanw;
            pDst += d_scanw;
        }

        return;
    }

    switch (rop)
    {
    case ROP_SRC :

        if (bk_mode == BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = GET16_LITTLE(p1);//(*p1) | (*(p1 + 1) << 8);
                    pixel = RGB15TORGB16(pixel);
                    
                    if (pixel != (uint32)pGraphParam->bk_color)
                        PUTPIXEL(p2, pixel);

                    p1 += 2;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else  // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = GET16_LITTLE(p1);//(*p1) | (*(p1 + 1) << 8);
                    pixel = RGB15TORGB16(pixel);

                    PUTPIXEL(p2, pixel);

                    p1 += 2;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }

        break;

    default :

        ropfunc = GetRopFunc(rop);

        if (bk_mode == BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = GET16_LITTLE(p1);//(*p1) | (*(p1 + 1) << 8);
                    pixel = RGB15TORGB16(pixel);

                    if (pixel != (uint32)pGraphParam->bk_color)
                    {
                        pixel = ropfunc(pixel, GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                    }

                    p1 += 2;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else  // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = GET16_LITTLE(p1);//(*p1) | (*(p1 + 1) << 8);
                    pixel = RGB15TORGB16(pixel);

                    pixel = ropfunc(pixel, GETPIXEL(p2));
                    PUTPIXEL(p2, pixel);

                    p1 += 2;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }

        break;
    }
}

/*********************************************************************\
* Function     DIBitBlt16
* Purpose      输出指定的16位彩色位图到指定的设备。
* Params       
* Return           
* Remarks      
    存在多种象素RGB排列格式，这里支持Microsoft的16位DIB格式，象素存放按
    Little Endian格式，例如5－5－5格式：
    11111111 01111111
    GGGBBBBB 0RRRRRGG
    在处理时按字节读入，用WORD型存放：
    0111111111111111
    0RRRRRGGGGGBBBBB

    调用者要给出Mask以确定RGB分别位数:
    dwRedMask   = 0x00007C00
    dwGreenMask = 0x000003E0
    dwBlueMask  = 0x0000001F

    另外支持12位4－4－4格式：
    11111111 11110000
    RRRRGGGG BBBB0000
    处理时如果当前显示模式就是16位，则直接写入显存；否则用Mask转换
       
**********************************************************************/
static void DIBitBlt16(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
                       int x, int y, PGRAPHPARAM pGraphParam)
{
    int     s_scanw, d_scanw;
    int     width, height, i;
    uint8   *pSrc, *pDst;
    uint8   *p1, *p2;
    uint32  pixel;
    uint32  rop;
    int     bk_mode;
    ROPFUNC ropfunc;
    DWORD   dwRedMask, dwGreenMask, dwBlueMask;
    int     RLShift, RRShift, GLShift, GRShift, BLShift, BRShift;
    int     RLen, GLen, BLen;
    uint32  dred, dgreen, dblue;
    uint32  sred, sgreen, sblue;
    int     pix_bytes;
    
    /* The default option is 5-5-5 */
    if (pBitmap->pPalette == NULL)
    {
#if (LDD_MODE == DDM_RGB16_565)
        DIBitBlt15To16(pDev, pDestRect, pBitmap, x, y, pGraphParam);
        return;
#else
        dwRedMask   = 0x00007C00L;
        dwGreenMask = 0x000003E0L;
        dwBlueMask  = 0x0000001FL;
#endif //LDD_MODE == DDM_RGB16_565
    }
    else
    {
        /* wwh removed this line for the address align problem. Can not be 
        ** converted to DWORD* . And the mask stored use little endian
        ** On 2004/05/31.
        */
        //pRGBMask    = (DWORD*)pBitmap->pPalette;
        dwRedMask   = GET32_LITTLE((BYTE*)(pBitmap->pPalette));
        dwGreenMask = GET32_LITTLE((BYTE*)(pBitmap->pPalette) + 4);
        dwBlueMask  = GET32_LITTLE((BYTE*)(pBitmap->pPalette) + 8);
        ASSERT((dwRedMask & dwGreenMask & dwBlueMask) == 0);

#if (LDD_MODE == DDM_RGB12_4440)
        if (dwRedMask == 0x0000F000L && 
            dwGreenMask == 0x00000F00L && 
            dwBlueMask == 0x000000F0L && 
            pGraphParam->bk_mode != BM_ALPHATRANSPARENT)
        {
            DIBitBlt12(pDev, pDestRect, pBitmap, x, y, pGraphParam);
            return ;
        }
#endif //LDD_MODE == DDM_RGB12_4440
    }

    s_scanw = (pBitmap->width * 16 + 31) / 32 * 4;

    width = pDestRect->x2 - pDestRect->x1;
    height = pDestRect->y2 - pDestRect->y1;
    
    pSrc = (uint8*)pBitmap->pBuffer;

    if (pBitmap->height > 0)
    {
        pSrc += (pBitmap->height - y - 1) * s_scanw + x * 2;
        s_scanw = -s_scanw;
    }
    else
        pSrc += y * s_scanw + x * 2;
    
    rop = SRop3ToRop2(pGraphParam->rop);
    bk_mode = pGraphParam->bk_mode;

    d_scanw = pDev->line_bytes;
    pDst = GETXYADDR(pDev, pDestRect->x1, pDestRect->y1);
    pix_bytes = pDev->pix_bits / 8;

#if ((USELINECOPY) && (LDD_MODE == DDM_RGB16_565))
    if (bk_mode == BM_OPAQUE && 
        dwRedMask   == 0x0000F800L && 
        dwGreenMask == 0x000007E0L && 
        dwBlueMask  == 0x0000001FL)
    {
        COPYLINESTRUCT p;

        GetCopyLineType(pDst, pSrc, width * pix_bytes, &p, COPYLINE_L2R, rop);

        while (height--)
        {
            p.copylinefunc(&p);
            p.dst += d_scanw;
            p.src += s_scanw;
        }

        return;
    }
#endif

    /* WWH added on 2004/02/06 
    ** Fixed a bug in the GETRGBBYTE16_FROMMASK
    */
    GetRGBAMask(&RLShift, &RRShift, &RLen, dwRedMask);
    GetRGBAMask(&GLShift, &GRShift, &GLen, dwGreenMask);
    GetRGBAMask(&BLShift, &BRShift, &BLen, dwBlueMask);

    /* AlphaDIBBlend函数的调用功能实现，只使用SCA
    ** BlendFunction非空，而且SCA不是255。
    ** 当SCA=255时，不存在alpha操作，直接贴图。
    */
    if (bk_mode == BM_ALPHATRANSPARENT
        && pGraphParam->pBlend != NULL
        && pGraphParam->pBlend->BlendFlags == 0 
        && pGraphParam->pBlend->BlendOp == AC_SRC_OVER
        && pGraphParam->pBlend->SourceConstantAlpha != 0xFF)
    {
        uint32   uSCA = pGraphParam->pBlend->SourceConstantAlpha;

        /* 完全透明 */
        if (uSCA == 0)
            return ;
        
        /* 只使用SCA */
        while (height--)
        {
            p1 = pSrc;
            p2 = pDst;

            for (i = 0; i < width; i++)
            {
                pixel = GET16_LITTLE(p1);//(*p1) | (*(p1 + 1) << 8);
                GETRGBBYTE16_FROMMASK(pixel, dwRedMask, RLShift, RRShift, RLen, sred);
                GETRGBBYTE16_FROMMASK(pixel, dwGreenMask, GLShift, GRShift, GLen, sgreen);
                GETRGBBYTE16_FROMMASK(pixel, dwBlueMask, BLShift, BRShift, BLen, sblue);

//                sred = GetRGBFrom16(pixel, dwRedMask, RedRShift, RedLShift);
//                sgreen = GetRGBFrom16(pixel, dwGreenMask, GreenRShift, GreenLShift);
//                sblue = GetRGBFrom16(pixel, dwBlueMask, BlueRShift, BlueLShift);

                GETRGBBYTE(p2, dred, dgreen, dblue);

                //dred = (sred * salpha + (255 - salpha) * dred) / 255;
                //dred = dred + (sred - dred) * salpha / 255;

                INT8_255DIVIDED(sred * uSCA + (255 - uSCA) * dred, dred);
                INT8_255DIVIDED(sgreen * uSCA + (255 - uSCA) * dgreen, dgreen);
                INT8_255DIVIDED(sblue * uSCA + (255 - uSCA) * dblue, dblue);

                pixel = REALIZECOLOR(dred, dgreen, dblue);
                PUTPIXEL(p2, pixel);

                p2 += pix_bytes;
            }

            pSrc += s_scanw;
            pDst += d_scanw;
        }

        return;
    }

    switch (rop)
    {
    case ROP_SRC :

        if (bk_mode == BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = GET16_LITTLE(p1);//(*p1) | (*(p1 + 1) << 8);
                    GETRGBBYTE16_FROMMASK(pixel, dwRedMask, RLShift, RRShift, RLen, sred);
                    GETRGBBYTE16_FROMMASK(pixel, dwGreenMask, GLShift, GRShift, GLen, sgreen);
                    GETRGBBYTE16_FROMMASK(pixel, dwBlueMask, BLShift, BRShift, BLen, sblue);

                    pixel = REALIZECOLOR(sred, sgreen, sblue);

                    if (pixel != (uint32)pGraphParam->bk_color)
                        PUTPIXEL(p2, pixel);

                    p1 += 2;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else  // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = GET16_LITTLE(p1);//(*p1) | (*(p1 + 1) << 8);
                    GETRGBBYTE16_FROMMASK(pixel, dwRedMask, RLShift, RRShift, RLen, sred);
                    GETRGBBYTE16_FROMMASK(pixel, dwGreenMask, GLShift, GRShift, GLen, sgreen);
                    GETRGBBYTE16_FROMMASK(pixel, dwBlueMask, BLShift, BRShift, BLen, sblue);

                    pixel = REALIZECOLOR(sred, sgreen, sblue);

                    PUTPIXEL(p2, pixel);

                    p1 += 2;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }

        break;

    default :

        ropfunc = GetRopFunc(rop);

        if (bk_mode == BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = GET16_LITTLE(p1);//(*p1) | (*(p1 + 1) << 8);
                    GETRGBBYTE16_FROMMASK(pixel, dwRedMask, RLShift, RRShift, RLen, sred);
                    GETRGBBYTE16_FROMMASK(pixel, dwGreenMask, GLShift, GRShift, GLen, sgreen);
                    GETRGBBYTE16_FROMMASK(pixel, dwBlueMask, BLShift, BRShift, BLen, sblue);

                    pixel = REALIZECOLOR(sred, sgreen, sblue);

                    if (pixel != (uint32)pGraphParam->bk_color)
                    {
                        pixel = ropfunc(pixel, GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                    }

                    p1 += 2;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else  // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = GET16_LITTLE(p1);//(*p1) | (*(p1 + 1) << 8);
                    GETRGBBYTE16_FROMMASK(pixel, dwRedMask, RLShift, RRShift, RLen, sred);
                    GETRGBBYTE16_FROMMASK(pixel, dwGreenMask, GLShift, GRShift, GLen, sgreen);
                    GETRGBBYTE16_FROMMASK(pixel, dwBlueMask, BLShift, BRShift, BLen, sblue);

                    pixel = REALIZECOLOR(sred, sgreen, sblue);

                    pixel = ropfunc(pixel, GETPIXEL(p2));
                    PUTPIXEL(p2, pixel);

                    p1 += 2;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }

        break;
    }
}

/*********************************************************************\
* Function     DIBitBlt24
* Purpose      输出指定的24位真彩色位图到指定的设备。
* Params       
* Return           
* Remarks      
**********************************************************************/
static void DIBitBlt24(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
                       int x, int y, PGRAPHPARAM pGraphParam)
{   
    int     s_scanw, d_scanw;
    int     width, height, i;
    uint8   *pSrc, *pDst;
    uint8   *p1, *p2;
    uint32  pixel;
    uint32  rop;
    int     bk_mode;
    ROPFUNC ropfunc;
    int     pix_bytes;

    s_scanw = (pBitmap->width * 24 + 31) / 32 * 4;

    width = pDestRect->x2 - pDestRect->x1;
    height = pDestRect->y2 - pDestRect->y1;
    
    pSrc = (uint8*)pBitmap->pBuffer;

    if (pBitmap->height > 0)
    {
        pSrc += (pBitmap->height - y - 1) * s_scanw + x * 3;
        s_scanw = -s_scanw;
    }
    else
        pSrc += y * s_scanw + x * 3;
    
    rop = SRop3ToRop2(pGraphParam->rop);
    bk_mode = pGraphParam->bk_mode;

    d_scanw = pDev->line_bytes;
    pDst = GETXYADDR(pDev, pDestRect->x1, pDestRect->y1);
    pix_bytes = pDev->pix_bits / 8;

    /* AlphaDIBBlend函数的调用功能实现，只使用SCA
    ** BlendFunction非空，而且SCA不是255。
    ** 当SCA=255时，不存在alpha操作，直接贴图。
    */
    if (bk_mode == BM_ALPHATRANSPARENT
        && pGraphParam->pBlend != NULL
        && pGraphParam->pBlend->BlendFlags == 0 
        && pGraphParam->pBlend->BlendOp == AC_SRC_OVER
        && pGraphParam->pBlend->SourceConstantAlpha != 0xFF)
    {
        uint32  dred, dgreen, dblue;
        uint32  sred, sgreen, sblue;

        uint32   uSCA = pGraphParam->pBlend->SourceConstantAlpha;

        /* 完全透明 */
        if (uSCA == 0)
            return ;
        
        /* 只使用SCA */
        while (height--)
        {
            p1 = pSrc;
            p2 = pDst;

            for (i = 0; i < width; i++)
            {
                sblue = *p1++;
                sgreen = *p1++;
                sred = *p1++;

                GETRGBBYTE(p2, dred, dgreen, dblue);

                //dred = (sred * salpha + (255 - salpha) * dred) / 255;
                //dred = dred + (sred - dred) * salpha / 255;

                INT8_255DIVIDED(sred * uSCA + (255 - uSCA) * dred, dred);
                INT8_255DIVIDED(sgreen * uSCA + (255 - uSCA) * dgreen, dgreen);
                INT8_255DIVIDED(sblue * uSCA + (255 - uSCA) * dblue, dblue);

                DI24_PUTPIXEL_EX(p2, dred, dgreen, dblue);

                p2 += pix_bytes;
            }

            pSrc += s_scanw;
            pDst += d_scanw;
        }

        return;
    }

#if ((USELINECOPY) && (LDD_MODE == DDM_RGB24_BGR || LDD_MODE == DDM_RGB18_BGR))
    if (bk_mode == BM_OPAQUE)
    {
        COPYLINESTRUCT p;

        GetCopyLineType(pDst, pSrc, width * pix_bytes, &p, COPYLINE_L2R, rop);

        while (height--)
        {
            p.copylinefunc(&p);
            p.dst += d_scanw;
            p.src += s_scanw;
        }
        
        return;
    }
#endif

    switch (rop)
    {
    case ROP_SRC :

        if (bk_mode == BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = REALIZECOLOR(*(p1 + 2), *(p1 + 1), *p1);
                    if (pixel != (uint32)pGraphParam->bk_color)
                        PUTPIXEL(p2, pixel);

                    p1 += 3;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else  // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    DI24_PUTPIXEL_EX(p2, *(p1 + 2), *(p1 + 1), *p1);

                    p1 += 3;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }

        break;

    default :

        ropfunc = GetRopFunc(rop);

        if (bk_mode == BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = REALIZECOLOR(*(p1 + 2), *(p1 + 1), *p1);
                    if (pixel != (uint32)pGraphParam->bk_color)
                    {
                        pixel = ropfunc(pixel, GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                    }

                    p1 += 3;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else  // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = REALIZECOLOR(*(p1 + 2), *(p1 + 1), *p1);
                    pixel = ropfunc(pixel, GETPIXEL(p2));
                    PUTPIXEL(p2, pixel);

                    p1 += 3;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }

        break;
    }
}

/*********************************************************************\
* Function     DIBitBlt32
* Purpose      输出指定的32位真彩色位图到指定的设备。
* Params       
* Return           
* Remarks      
**********************************************************************/
static void DIBitBlt32(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
                       int x, int y, PGRAPHPARAM pGraphParam)
{   
    int     s_scanw, d_scanw;
    int     width, height, i;
    uint8   *pSrc, *pDst;
    uint8   *p1, *p2;
    uint32  pixel;
    uint32  rop;
    int     bk_mode;
    ROPFUNC ropfunc;
    int     pix_bytes;

    s_scanw = (pBitmap->width * 32 + 31) / 32 * 4;

    width = pDestRect->x2 - pDestRect->x1;
    height = pDestRect->y2 - pDestRect->y1;
    
    pSrc = (uint8*)pBitmap->pBuffer;

    if (pBitmap->height > 0)
    {
        pSrc += (pBitmap->height - y - 1) * s_scanw + x * 4;
        s_scanw = -s_scanw;
    }
    else
        pSrc += y * s_scanw + x * 4;
    
    rop = SRop3ToRop2(pGraphParam->rop);
    bk_mode = pGraphParam->bk_mode;

    d_scanw = pDev->line_bytes;
    pDst = GETXYADDR(pDev, pDestRect->x1, pDestRect->y1);
    pix_bytes = pDev->pix_bits / 8;

    if (bk_mode == BM_ALPHATRANSPARENT)
    {
        uint32  salpha;
        uint32  dred, dgreen, dblue;
        uint32  sred, sgreen, sblue;

        /* BlendFunction非空，而且SCA不是255，则考虑SCA。
        ** 当SCA=255时，则单独使用SPA贴图。
        */
        if (pGraphParam->pBlend != NULL
            && pGraphParam->pBlend->BlendFlags == 0 
            && pGraphParam->pBlend->BlendOp == AC_SRC_OVER
            && pGraphParam->pBlend->SourceConstantAlpha != 0xFF)
        {
            uint32   uSCA = pGraphParam->pBlend->SourceConstantAlpha;

            /* 完全透明 */
            if (uSCA == 0)
                return ;
            
            if (pGraphParam->pBlend->AlphaFormat == AC_SRC_ALPHA)
            {
                /* 同时使用SPA和SCA */
                uSCA <<= 8;
                INT8_255DIVIDED(uSCA, uSCA);
                while (height--)
                {
                    p1 = pSrc;
                    p2 = pDst;

                    for (i = 0; i < width; i++)
                    {
                        sblue = (*p1++ * uSCA) >> 8; //s * (SCA * 256 / 255) / 256
                        sgreen = (*p1++ * uSCA) >> 8;
                        sred = (*p1++ * uSCA) >> 8;
                        salpha = (*p1++ * uSCA) >> 8;

                        if (salpha == 255)
                        {
                            DI24_PUTPIXEL_EX(p2, sred, sgreen, sblue);
                        }
                        else if (salpha != 0)
                        {
                            GETRGBBYTE(p2, dred, dgreen, dblue);

                            //dred = (sred * salpha + (255 - salpha) * dred) / 255;
                            //dred = dred + (sred - dred) * salpha / 255;

                            INT8_255DIVIDED(sred * salpha + (255 - salpha * uSCA) * dred, dred);
                            INT8_255DIVIDED(sgreen * salpha + (255 - salpha * uSCA) * dgreen, dgreen);
                            INT8_255DIVIDED(sblue * salpha + (255 - salpha * uSCA) * dblue, dblue);

                            DI24_PUTPIXEL_EX(p2, dred, dgreen, dblue);
                        }

                        p2 += pix_bytes;
                    }

                    pSrc += s_scanw;
                    pDst += d_scanw;
                }

                return;
            }
            else
            {
                /* 只使用SCA */
                while (height--)
                {
                    p1 = pSrc;
                    p2 = pDst;

                    for (i = 0; i < width; i++)
                    {
                        sblue = *p1++;
                        sgreen = *p1++;
                        sred = *p1++;
                        p1++;

                        GETRGBBYTE(p2, dred, dgreen, dblue);

                        //dred = (sred * salpha + (255 - salpha) * dred) / 255;
                        //dred = dred + (sred - dred) * salpha / 255;

                        INT8_255DIVIDED(sred * uSCA + (255 - uSCA) * dred, dred);
                        INT8_255DIVIDED(sgreen * uSCA + (255 - uSCA) * dgreen, dgreen);
                        INT8_255DIVIDED(sblue * uSCA + (255 - uSCA) * dblue, dblue);

                        DI24_PUTPIXEL_EX(p2, dred, dgreen, dblue);

                        p2 += pix_bytes;
                    }

                    pSrc += s_scanw;
                    pDst += d_scanw;
                }

                return;
            }

        }

        /* 只使用SPA进行贴图 */
        while (height--)
        {
            p1 = pSrc;
            p2 = pDst;

            for (i = 0; i < width; i++)
            {
                sblue = *p1++;
                sgreen = *p1++;
                sred = *p1++;
                salpha = *p1++;

                if (salpha == 255)
                {
                    DI24_PUTPIXEL_EX(p2, sred, sgreen, sblue);
                }
                else if (salpha != 0)
                {
                    GETRGBBYTE(p2, dred, dgreen, dblue);

                    //dred = (sred * salpha + (255 - salpha) * dred) / 255;
                    //dred = dred + (sred - dred) * salpha / 255;

                    INT8_255DIVIDED(sred * salpha + (255 - salpha) * dred, dred);
                    INT8_255DIVIDED(sgreen * salpha + (255 - salpha) * dgreen, dgreen);
                    INT8_255DIVIDED(sblue * salpha + (255 - salpha) * dblue, dblue);
    /*
                    INT8_255DIVIDED(sred * salpha + (255 - salpha) * dred, dred);
                    INT8_255DIVIDED(sgreen * salpha + (255 - salpha) * dgreen, dgreen);
                    INT8_255DIVIDED(sblue * salpha + (255 - salpha) * dblue, dblue);
    */

    /*
                    dred += (sred - dred) * salpha / 255
                    dgreen += (sgreen - dgreen) * salpha / 255
                    dblue += (sblue - dblue) * salpha / 255
    */

                    DI24_PUTPIXEL_EX(p2, dred, dgreen, dblue);
                }

                p2 += pix_bytes;
            }

            pSrc += s_scanw;
            pDst += d_scanw;
        }

        return;
    }

#if ((USELINECOPY) && (LDD_MODE == DDM_RGB32_BGR0 || LDD_MODE == DDM_RGB18_BGR0 ||    \
    LDD_MODE == DDM_RGB32_BGR0 || LDD_MODE == DDM_RGB18_BGR0))
    if (bk_mode == BM_OPAQUE)
    {
        COPYLINESTRUCT p;

        GetCopyLineType(pDst, pSrc, width * pix_bytes, &p, COPYLINE_L2R, rop);

        while (height--)
        {
            p.copylinefunc(&p);
            p.dst += d_scanw;
            p.src += s_scanw;
        }
        
        return;
    }
#endif

    switch (rop)
    {
    case ROP_SRC :
        
        if (bk_mode == BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = REALIZECOLOR(*(p1 + 2), *(p1 + 1), *p1);
                    if (pixel != (uint32)pGraphParam->bk_color)
                        PUTPIXEL(p2, pixel);

                    p1 += 4;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else  // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    DI24_PUTPIXEL_EX(p2, *(p1 + 2), *(p1 + 1), *p1);

                    p1 += 4;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }

        break;

    default :

        ropfunc = GetRopFunc(rop);

        if (bk_mode == BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = REALIZECOLOR(*(p1 + 2), *(p1 + 1), *p1);
                    if (pixel != (uint32)pGraphParam->bk_color)
                    {
                        pixel = ropfunc(pixel, GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                    }

                    p1 += 4;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }
        else  // default bk_mode is BM_OPAQUE
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;

                for (i = 0; i < width; i++)
                {
                    pixel = REALIZECOLOR(*(p1 + 2), *(p1 + 1), *p1);
                    pixel = ropfunc(pixel, GETPIXEL(p2));
                    PUTPIXEL(p2, pixel);

                    p1 += 4;
                    p2 += pix_bytes;
                }

                pSrc += s_scanw;
                pDst += d_scanw;
            }
        }

        break;
    }
}

/*********************************************************************\
* Function     RealizePaletteColor
* Purpose      将调色板中的24位颜色转换为物理颜色。
* Params       
* Return           
* Remarks      
**********************************************************************/
static void RealizePaletteColor(void* pPalette, uint32* pPhysColor, 
                                int count)
{
    RGBQUAD* pLogColor;

    if (pPalette == NULL || pPhysColor == NULL)
        return;

    pLogColor = (RGBQUAD*)pPalette;
    while (count--)
    {
        *pPhysColor = REALIZECOLOR(pLogColor->R, pLogColor->G, 
            pLogColor->B);

        pLogColor++;
        pPhysColor++;
    }
}
