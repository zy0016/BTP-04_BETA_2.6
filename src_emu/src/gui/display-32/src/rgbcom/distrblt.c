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


#define PUTPIXEL_ROPFUNC(p2, spixel, ropfunc)    {   \
    PUTPIXEL(p2, ropfunc(spixel, GETPIXEL(p2)));  \
}

#define SRCPIXEL_INCREASE_1(p1, src_mask) {\
    src_mask >>= 1;         \
    if (src_mask == 0)      \
    {                       \
        p1 ++;              \
        src_mask = 0x80;    \
    }                       \
}

#define SRCPIXEL_INCREASE_1_U(p1, src_mask, spixel, PhysColors) {\
    if ((*p1) & src_mask)       \
        spixel = PhysColors[1];  \
    else                        \
        spixel = PhysColors[0];  \
    SRCPIXEL_INCREASE_1(p1, src_mask);  \
}

#define SRCPIXEL_INCREASE_4(p1, src_mask) {\
    if (src_mask == 0x0F)                   \
    {                                       \
        p1 ++;                              \
    }                                       \
    src_mask = ~src_mask;                   \
}

#define SRCPIXEL_INCREASE_4_U(p1, src_mask, spixel, PhysColors) {\
    if (src_mask == 0x0F)                   \
    {                                       \
        spixel = PhysColors[(*p1) & 0x0F];  \
        p1 ++;                              \
    }                                       \
    else                                    \
    {                                       \
        spixel = PhysColors[(*p1) >> 4];    \
    }                                       \
    src_mask = ~src_mask;                   \
}

#define SRCPIXEL_INCREASE_8(p1) {\
    p1 ++;                              \
}

#define SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors) {\
    spixel = PhysColors[(*p1)]; \
    p1 ++;                      \
}

#define SRCPIXEL_INCREASE_16(p1) {\
    p1 += 2;                              \
}

/*
#define SRCPIXEL_INCREASE_16_U(p1, spixel) {\
    spixel = GET16_LITTLE(p1);   \
    GETRGBBYTE16_FROMMASK(spixel, dwRedMask, Rstart, Rlen, sred);    \
    GETRGBBYTE16_FROMMASK(spixel, dwGreenMask, Gstart, Glen, sgreen);    \
    GETRGBBYTE16_FROMMASK(spixel, dwBlueMask, Bstart, Blen, sblue);  \
    spixel = REALIZECOLOR(sred, sgreen, sblue);  \
    p1 += 2;                      \
}
*/

/* WWH added on 2004/02/06 
** Fixed a bug in the GETRGBBYTE16_FROMMASK
*/
#define SRCPIXEL_INCREASE_16_U(p1, spixel) {\
    spixel = GET16_LITTLE(p1);   \
    GETRGBBYTE16_FROMMASK(spixel, dwRedMask, RLShift, RRShift, RLen, sred); \
    GETRGBBYTE16_FROMMASK(spixel, dwGreenMask, GLShift, GRShift, GLen, sgreen); \
    GETRGBBYTE16_FROMMASK(spixel, dwBlueMask, BLShift, BRShift, BLen, sblue);   \
    spixel = REALIZECOLOR(sred, sgreen, sblue);  \
    p1 += 2;                      \
}

#define SRCPIXEL_INCREASE_24(p1) {\
    p1 += 3;    \
}

#define SRCPIXEL_INCREASE_24_U(p1, spixel) {\
    spixel = REALIZECOLOR(*(p1 + 2), *(p1 + 1), *p1);   \
    p1 += 3;    \
}

#define SRCPIXEL_INCREASE_32(p1) {\
    p1 += 4;    \
}

#define SRCPIXEL_INCREASE_32_U(p1, spixel) {\
    spixel = REALIZECOLOR(*(p1 + 2), *(p1 + 1), *p1);   \
    p1 += 4;    \
}

/*********************************************************************\
* Function	 DIStretchBlt  
* Purpose    设备无关位图的StretchBlt，调用各个不同的函数实现。  
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DIStretchBlt(PDEV pDev, PRECT pDstRect, PDIBITMAP pBitmap, 
                        PRECT pSrcRect, PGRAPHPARAM pGraphParam)
{
    RECT    rc, src_rect;
    int     src_width, src_height;
    int     dest_width, dest_height;
    /* wwh modified on 2004/04/28 for the given bitmap width or height is grater 
    ** than the difined macro DISP_MAX_WIDTH or DISP_MAX_HEIGHT.
    */
    DISTRBLTPARAM DIStrParam;
    BYTE    bWidthPattern[DISP_MAX_WIDTH];
    BYTE    bHeightPattern[DISP_MAX_HEIGHT];

    if (pDev == NULL || pBitmap == NULL || pGraphParam == NULL)
        return -1;
    if (pGraphParam == NULL)
        return -1;

    if (!IsValidRect(pSrcRect))
        return -1;
    if (!IsValidRect(pDstRect))
        return -1;

    ASSERT(pSrcRect->x2 <= ABS(pBitmap->width));
    ASSERT(pSrcRect->y2 <= ABS(pBitmap->height));

    src_width = pSrcRect->x2 - pSrcRect->x1;
    src_height = pSrcRect->y2 - pSrcRect->y1;

    dest_width = pDstRect->x2 - pDstRect->x1; 
    dest_height = pDstRect->y2 - pDstRect->y1; 

    if (src_width == dest_width && src_height == dest_height)
    {
        return DIBitBlt(pDev, pDstRect, pBitmap, pSrcRect->x1, 
            pSrcRect->y1, pGraphParam);
    }

    DIStrParam.src_width_origin = src_width;
    DIStrParam.src_height_origin = src_height;
    DIStrParam.dest_width_origin = dest_width;
    DIStrParam.dest_height_origin = dest_height;

    CopyRect(&rc, pDstRect);
    CopyRect(&src_rect, pSrcRect);
    
    ClipByDev(&rc, pDev);
    if (pGraphParam != NULL && (pGraphParam->flags & GPF_CLIP_RECT))
        ClipRect(&rc, &pGraphParam->clip_rect);
    
    if (!IsValidRect(&rc))
        return -1;

    //以(pDestRect->x1,pDestRect->y1)为原点,调整目的矩形 
    rc.x1 -= pDstRect->x1;
    rc.y1 -= pDstRect->y1;
    rc.x2 -= pDstRect->x1;
    rc.y2 -= pDstRect->y1;

    //以(x,y)为原点,调整源矩形
    src_rect.x1 -= pSrcRect->x1;
    src_rect.y1 -= pSrcRect->y1;
    src_rect.x2 -= pSrcRect->x1;
    src_rect.y2 -= pSrcRect->y1;
    
    //据未剪裁的矩形的宽,求缩放比
    if (dest_width > src_width)
    {
        //调整源矩形坐标,将源矩形放在目的矩形坐标系下
        src_rect.x1 = src_rect.x1 * dest_width / src_width;
        src_rect.y1 = src_rect.y1 * dest_height / src_height;
        src_rect.x2 = src_rect.x2 * dest_width / src_width;
        src_rect.y2 = src_rect.y2 * dest_height / src_height;

        //获得目的和源矩形的交集
        if (!ClipRect(&rc, &src_rect))
            return -1;

        CopyRect(&src_rect, &rc);
        
        //源矩形的坐标恢复到原坐标系下
        src_rect.x1 = src_rect.x1 * src_width / dest_width  + pSrcRect->x1;
        src_rect.y1 = src_rect.y1 * src_height / dest_height  + pSrcRect->y1;
        src_rect.x2 = src_rect.x2 * src_width / dest_width + pSrcRect->x1;
        src_rect.y2 = src_rect.y2 * src_height / dest_height + pSrcRect->y1;
        
        //恢复目的矩形坐标
        rc.x1 += pDstRect->x1;
        rc.y1 += pDstRect->y1;
        rc.x2 += pDstRect->x1;
        rc.y2 += pDstRect->y1;
    }
    else
    {
        RECT rcTemp;

        CopyRect(&rcTemp, &rc);
        //调整目的矩形坐标,将目的矩形放在源矩形坐标系下
        rc.x1 = rc.x1 * src_width / dest_width;
        rc.y1 = rc.y1 * src_height / dest_height;
        rc.x2 = rc.x2 * src_width / dest_width;
        rc.y2 = rc.y2 * src_height / dest_height;

        //获得目的和源矩形的交集
        if (!ClipRect(&rc, &src_rect))
            return -1;

        CopyRect(&src_rect, &rc);
        
        //恢复源矩形坐标
        src_rect.x1 += pSrcRect->x1;
        src_rect.y1 += pSrcRect->y1;
        src_rect.x2 += pSrcRect->x1;
        src_rect.y2 += pSrcRect->y1;

        CopyRect(&rc, &rcTemp);
        rc.x1 += pDstRect->x1;
        rc.y1 += pDstRect->y1;
        rc.x2 += pDstRect->x1;
        rc.y2 += pDstRect->y1;
        //目的矩形的坐标恢复到原坐标系下
/*
        rc.x1 = rc.x1 * dest_width / src_width  + pDstRect->x1;
        rc.y1 = rc.y1 * dest_height / src_height + pDstRect->y1;
        rc.x2 = rc.x2 * dest_width / src_width  + pDstRect->x1;
        rc.y2 = rc.y2 * dest_height / src_height + pDstRect->y1;
*/
    }

    //重新计算宽高
    src_width = src_rect.x2 - src_rect.x1;
    src_height = src_rect.y2 - src_rect.y1;
      
    dest_width = rc.x2 - rc.x1; 
    dest_height = rc.y2 - rc.y1; 

    CopyRect(&(DIStrParam.src_rc), &src_rect);
    CopyRect(&(DIStrParam.dest_rc), &rc);

    /* WWH modified on 2003/12/10 
    ** Some input parameter may be larger than device and should be cliped
    ** first. 
    ** This problem may be considerd later.
    */
    {
        RECT rcSrc, rcSrcDev;
        int maxlen;
        CopyRect(&rcSrc, pSrcRect);
        SetRect(&rcSrcDev, 0, 0, ABS(pBitmap->width), ABS(pBitmap->height));
        ClipRect(&rcSrc, &rcSrcDev);

    /* wwh modified on 2004/04/28 for the given bitmap width or height is grater 
    ** than the difined macro DISP_MAX_WIDTH or DISP_MAX_HEIGHT.
    */
        maxlen = max(DIStrParam.dest_width_origin, 
            DIStrParam.src_width_origin) + 1;
        if (maxlen > DISP_MAX_WIDTH)
        {
            DIStrParam.stretchpattern.bWidthPattern = malloc(maxlen);
            if (DIStrParam.stretchpattern.bWidthPattern == NULL)
                return -1;
        }
        else
            DIStrParam.stretchpattern.bWidthPattern = bWidthPattern;

        maxlen = max(DIStrParam.dest_height_origin, 
            DIStrParam.src_height_origin) + 1;
        if (maxlen > DISP_MAX_HEIGHT)
        {
            DIStrParam.stretchpattern.bHeightPattern = malloc(maxlen);
            if (DIStrParam.stretchpattern.bHeightPattern == NULL)
                return -1;
        }
        else
            DIStrParam.stretchpattern.bHeightPattern = bHeightPattern;

        GetStretchPattern(pDstRect, &rcSrc, &(DIStrParam.stretchpattern));
    }

    DIStrParam.src_x_offset = 
        GetSrcOffset(DIStrParam.stretchpattern.bWidthPattern, 
        DIStrParam.dest_width_origin, DIStrParam.src_width_origin, 
        rc.x1 - pDstRect->x1) + pSrcRect->x1;
    DIStrParam.src_y_offset = 
        GetSrcOffset(DIStrParam.stretchpattern.bHeightPattern, 
        DIStrParam.dest_height_origin, DIStrParam.src_height_origin, 
        rc.y1 - pDstRect->y1) + pSrcRect->y1;

    _ExcludeCursor(pDev, &rc);

    switch (pBitmap->type)
    {
    case BMP_MONO:
        Stretch1(pDev, pBitmap, pGraphParam, &DIStrParam);
        break;
    case BMP_16:
        Stretch4(pDev, pBitmap, pGraphParam, &DIStrParam);
        break;
    case BMP_256:
        Stretch8(pDev, pBitmap, pGraphParam, &DIStrParam);
        break;
    case BMP_RGB16:
        Stretch16(pDev, pBitmap, pGraphParam, &DIStrParam);
        break;
    case BMP_RGB24:
        Stretch24(pDev, pBitmap, pGraphParam, &DIStrParam);
        break;
    case BMP_RGB32:
        Stretch32(pDev, pBitmap, pGraphParam, &DIStrParam);
        break;
    default:
        break;
    }

    _UnexcludeCursor(pDev);

    if (DIStrParam.stretchpattern.bWidthPattern != bWidthPattern)
        free(DIStrParam.stretchpattern.bWidthPattern);
    if (DIStrParam.stretchpattern.bHeightPattern != bHeightPattern)
        free(DIStrParam.stretchpattern.bHeightPattern);

    return 1;
}

/*********************************************************************\
* Function	   Stretch1
* Purpose      Stretch a monocolor DIBitmap and bitblt it on destination.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int  Stretch1(PDEV pDev, PDIBITMAP pBitmap, 
                        PGRAPHPARAM pGraphParam, PDISTRBLTPARAM pDIStrParam)
{
    uint32      PhysColors[2];
    uint8       *pSrc, *pDst, *p1, *p2;
    uint8       src_mask;
    uint32      spixel;
    int         dst_pix_bytes;
    int         src_line_bytes, dst_line_bytes;
    int32       rop;
    ROPFUNC     ropfunc;
    int         dest_width, dest_height;
    int         src_width, src_height;
    int         src_width_origin, src_height_origin;
    int         dest_width_origin, dest_height_origin; 
    BYTE        *pXPattern, *pYPattern;
    BYTE        *pX;
    int         width_offset, height_offset;
    int         width;

    src_line_bytes = (pBitmap->width + 31) / 32 * 4;
    dst_pix_bytes = pDev->pix_bits / 8;
    dst_line_bytes = pDev->line_bytes;

    if (! src_line_bytes || ! dst_line_bytes)
        return 0;
   
    if (pBitmap->pPalette)
        RealizePaletteColor(pBitmap->pPalette, PhysColors, 2);
    else
    {
        PhysColors[0] = pGraphParam->bk_color;
        PhysColors[1] = pGraphParam->text_color;
    }

    src_width = pDIStrParam->src_rc.x2 - pDIStrParam->src_rc.x1;
    src_height = pDIStrParam->src_rc.y2 - pDIStrParam->src_rc.y1;
    src_width_origin = pDIStrParam->src_width_origin;
    src_height_origin = pDIStrParam->src_height_origin;
    
    dest_width = pDIStrParam->dest_rc.x2 - pDIStrParam->dest_rc.x1;
    dest_height = pDIStrParam->dest_rc.y2 - pDIStrParam->dest_rc.y1;
    dest_width_origin = pDIStrParam->dest_width_origin;
    dest_height_origin = pDIStrParam->dest_height_origin;

    width_offset = pDIStrParam->src_x_offset;
    height_offset = pDIStrParam->src_y_offset;
    pXPattern = pDIStrParam->stretchpattern.bWidthPattern + width_offset;
    pYPattern = pDIStrParam->stretchpattern.bHeightPattern + height_offset;

    pSrc = (uint8*)pBitmap->pBuffer;

    if (pBitmap->height > 0)
    {
        pSrc += (pBitmap->height - height_offset - 1) * src_line_bytes + 
            width_offset / 8;
        src_line_bytes = -src_line_bytes;
    }
    else
        pSrc += height_offset * src_line_bytes + width_offset / 8;

    pDst = GETXYADDR(pDev, pDIStrParam->dest_rc.x1, pDIStrParam->dest_rc.y1);

    rop = SRop3ToRop2(pGraphParam->rop);
    ropfunc = GetRopFunc(rop);

    if (pGraphParam->bk_mode == BM_TRANSPARENT)
    {
        if (dest_height_origin >= src_height_origin)
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0x80 >> (width_offset & 7);
                    while(width--)
                    {
                        if ((*p1) & src_mask)
                        {
                            PUTPIXEL_ROPFUNC(p2, PhysColors[1], ropfunc);
                        }
                        
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_1(p1, src_mask);
                        }
                    }

                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0x80 >> (width_offset & 7);
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_1(p1, src_mask);
                        }

                        if ((*p1) & src_mask)
                        {
                            PUTPIXEL_ROPFUNC(p2, PhysColors[1], ropfunc);
                        }

                        p2 += dst_pix_bytes;
                        SRCPIXEL_INCREASE_1(p1, src_mask);
                    }
                    
                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
        }
        else
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0x80 >> (width_offset & 7);
                    while(width--)
                    {
                        if ((*p1) & src_mask)
                        {
                            PUTPIXEL_ROPFUNC(p2, PhysColors[1], ropfunc);
                        }
                    
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_1(p1, src_mask);
                        }
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0x80 >> (width_offset & 7);
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_1(p1, src_mask);
                        }
                        if ((*p1) & src_mask)
                        {
                            PUTPIXEL_ROPFUNC(p2, PhysColors[1], ropfunc);
                        }
                        
                        p2 += dst_pix_bytes;
                        SRCPIXEL_INCREASE_1(p1, src_mask);
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }

        }
    }
    else //default is opqque
    {
        if (dest_height_origin >= src_height_origin)
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0x80 >> (width_offset & 7);
                    SRCPIXEL_INCREASE_1_U(p1, src_mask, spixel, PhysColors);
                    while(width--)
                    {
                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_1_U(p1, src_mask, spixel, PhysColors);
                        }
                    }

                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0x80 >> (width_offset & 7);
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_1(p1, src_mask);
                        }

                        SRCPIXEL_INCREASE_1_U(p1, src_mask, spixel, PhysColors);

                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
        }
        else
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0x80 >> (width_offset & 7);
                    SRCPIXEL_INCREASE_1_U(p1, src_mask, spixel, PhysColors);
                    while(width--)
                    {
                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                    
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_1_U(p1, src_mask, spixel, PhysColors);
                        }
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0x80 >> (width_offset & 7);
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_1(p1, src_mask);
                        }

                        SRCPIXEL_INCREASE_1_U(p1, src_mask, spixel, PhysColors);

                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }

        }
    }

    return 1;
}

/*********************************************************************\
* Function	   Stretch4
* Purpose      Stretch DIBitmap with a 16 color palette and bitblt it on 
               destination.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int  Stretch4(PDEV pDev, PDIBITMAP pBitmap, 
                        PGRAPHPARAM pGraphParam, PDISTRBLTPARAM pDIStrParam)
{
    uint32      PhysColors[16];
    uint8       *pSrc, *pDst, *p1, *p2;
    uint8       src_mask;
    uint32      spixel;
    int         dst_pix_bytes;
    int         src_line_bytes, dst_line_bytes;
    int32       rop;
    ROPFUNC     ropfunc;
    int         dest_width, dest_height;
    int         src_width, src_height;
    int         src_width_origin, src_height_origin;
    int         dest_width_origin, dest_height_origin; 
    BYTE        *pXPattern, *pYPattern;
    BYTE        *pX;
    int         width_offset, height_offset;
    int         width;

    src_line_bytes = (pBitmap->width * 4 + 31) / 32 * 4;
    dst_pix_bytes = pDev->pix_bits / 8;
    dst_line_bytes = pDev->line_bytes;
    
    if (! src_line_bytes || ! dst_line_bytes)
        return 0;

    if (pBitmap->pPalette)
        RealizePaletteColor(pBitmap->pPalette, PhysColors, 16);
    else
    {
        return -1; //should give default palette in future!!
    }

    src_width = pDIStrParam->src_rc.x2 - pDIStrParam->src_rc.x1;
    src_height = pDIStrParam->src_rc.y2 - pDIStrParam->src_rc.y1;
    src_width_origin = pDIStrParam->src_width_origin;
    src_height_origin = pDIStrParam->src_height_origin;
    
    dest_width = pDIStrParam->dest_rc.x2 - pDIStrParam->dest_rc.x1;
    dest_height = pDIStrParam->dest_rc.y2 - pDIStrParam->dest_rc.y1;
    dest_width_origin = pDIStrParam->dest_width_origin;
    dest_height_origin = pDIStrParam->dest_height_origin;

    width_offset = pDIStrParam->src_x_offset;
    height_offset = pDIStrParam->src_y_offset;
    pXPattern = pDIStrParam->stretchpattern.bWidthPattern + width_offset;
    pYPattern = pDIStrParam->stretchpattern.bHeightPattern + height_offset;

    pSrc = (uint8*)pBitmap->pBuffer;

    if (pBitmap->height > 0)
    {
        pSrc += (pBitmap->height - height_offset - 1) * src_line_bytes + 
            width_offset * 4 / 8;
        src_line_bytes = -src_line_bytes;
    }
    else
        pSrc += height_offset * src_line_bytes + width_offset * 4 / 8;

    pDst = GETXYADDR(pDev, pDIStrParam->dest_rc.x1, pDIStrParam->dest_rc.y1);

    rop = SRop3ToRop2(pGraphParam->rop);
    ropfunc = GetRopFunc(rop);

    if (pGraphParam->bk_mode == BM_NEWTRANSPARENT)
    {
        if (dest_height_origin >= src_height_origin)
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0xF0 >> (width_offset & 1) * 4;

                    SRCPIXEL_INCREASE_4_U(p1, src_mask, spixel, PhysColors);
                    while(width--)
                    {
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }

                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_4_U(p1, src_mask, spixel, PhysColors);
                        }
                    }

                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0xF0 >> (width_offset & 1) * 4;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_4(p1, src_mask);
                        }

                        SRCPIXEL_INCREASE_4_U(p1, src_mask, spixel, PhysColors);
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
        }
        else
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0xF0 >> (width_offset & 1) * 4;
                    SRCPIXEL_INCREASE_4_U(p1, src_mask, spixel, PhysColors);
                    while(width--)
                    {
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }
                    
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_4_U(p1, src_mask, spixel, PhysColors);
                        }
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0xF0 >> (width_offset & 1) * 4;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_4(p1, src_mask);
                        }

                        SRCPIXEL_INCREASE_4_U(p1, src_mask, spixel, PhysColors);
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }
                        
                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }

        }
    }
    else //default is opqque
    {
        if (dest_height_origin >= src_height_origin)
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0xF0 >> (width_offset & 1) * 4;
                    SRCPIXEL_INCREASE_4_U(p1, src_mask, spixel, PhysColors);
                    while(width--)
                    {
                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_4_U(p1, src_mask, spixel, PhysColors);
                        }
                    }

                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0xF0 >> (width_offset & 1) * 4;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_4(p1, src_mask);
                        }

                        SRCPIXEL_INCREASE_4_U(p1, src_mask, spixel, PhysColors);

                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
        }
        else
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0xF0 >> (width_offset & 1) * 4;
                    SRCPIXEL_INCREASE_4_U(p1, src_mask, spixel, PhysColors);
                    while(width--)
                    {
                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                    
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_4_U(p1, src_mask, spixel, PhysColors);
                        }
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    src_mask = 0xF0 >> (width_offset & 1) * 4;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_4(p1, src_mask);
                        }

                        SRCPIXEL_INCREASE_4_U(p1, src_mask, spixel, PhysColors);

                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }

        }
    }

    return 1;
}

/*********************************************************************\
* Function	   Stretch8
* Purpose      Stretch DIBitmap with a 256 color palette and bitblt it on 
               destination.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int  Stretch8(PDEV pDev, PDIBITMAP pBitmap, 
                        PGRAPHPARAM pGraphParam, PDISTRBLTPARAM pDIStrParam)
{
    uint32      PhysColors[256];
    uint8       *pSrc, *pDst, *p1, *p2;
    uint32      spixel;
    int         dst_pix_bytes;
    int         src_line_bytes, dst_line_bytes;
    int32       rop;
    ROPFUNC     ropfunc;
    int         dest_width, dest_height;
    int         src_width, src_height;
    int         src_width_origin, src_height_origin;
    int         dest_width_origin, dest_height_origin; 
    BYTE        *pXPattern, *pYPattern;
    BYTE        *pX;
    int         width_offset, height_offset;
    int         width;

    src_line_bytes = (pBitmap->width + 3) / 4 * 4;
    dst_pix_bytes = pDev->pix_bits / 8;
    dst_line_bytes = pDev->line_bytes;
    
    if (! src_line_bytes || ! dst_line_bytes)
        return 0;

    if (pBitmap->pPalette)
        RealizePaletteColor(pBitmap->pPalette, PhysColors, 256);
    else
    {
        return -1; //should give default palette in future!!
    }

    src_width = pDIStrParam->src_rc.x2 - pDIStrParam->src_rc.x1;
    src_height = pDIStrParam->src_rc.y2 - pDIStrParam->src_rc.y1;
    src_width_origin = pDIStrParam->src_width_origin;
    src_height_origin = pDIStrParam->src_height_origin;
    
    dest_width = pDIStrParam->dest_rc.x2 - pDIStrParam->dest_rc.x1;
    dest_height = pDIStrParam->dest_rc.y2 - pDIStrParam->dest_rc.y1;
    dest_width_origin = pDIStrParam->dest_width_origin;
    dest_height_origin = pDIStrParam->dest_height_origin;

    width_offset = pDIStrParam->src_x_offset;
    height_offset = pDIStrParam->src_y_offset;
    pXPattern = pDIStrParam->stretchpattern.bWidthPattern + width_offset;
    pYPattern = pDIStrParam->stretchpattern.bHeightPattern + height_offset;

    pSrc = (uint8*)pBitmap->pBuffer;

    if (pBitmap->height > 0)
    {
        pSrc += (pBitmap->height - height_offset - 1) * src_line_bytes + 
            width_offset;
        src_line_bytes = -src_line_bytes;
    }
    else
        pSrc += height_offset * src_line_bytes + width_offset;

    pDst = GETXYADDR(pDev, pDIStrParam->dest_rc.x1, pDIStrParam->dest_rc.y1);

    rop = SRop3ToRop2(pGraphParam->rop);
    ropfunc = GetRopFunc(rop);

    if (pGraphParam->bk_mode == BM_NEWTRANSPARENT)
    {
        /* Added on 20031031 for the index transpare display 
        ** Only in 256 colors bitmap 
        */
        if (((uint32)pGraphParam->bk_color & 0xFF000000) != 0)
        {
            int index;

            index = pGraphParam->bk_color & 0x000000FF;
            if (dest_height_origin >= src_height_origin)
            {
                if (dest_width_origin >= src_width_origin)
                {
                    while(dest_height--)
                    {
                        pX = pXPattern;
                        p1 = pSrc;
                        p2 = pDst;
                        width = dest_width;

                        spixel = PhysColors[(*p1)];
                        while(width--)
                        {
                            if (*p1 != index)
                            {
                                PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                            }

                            p2 += dst_pix_bytes;
                            if (*pX++ == 1)
                            {
                                SRCPIXEL_INCREASE_8(p1);
                                spixel = PhysColors[(*p1)];
                            }
                        }

                        pDst += dst_line_bytes;
                        if (*pYPattern++ == 1)
                            pSrc += src_line_bytes;
                    }
                }
                else
                {
                    while(dest_height--)
                    {
                        pX = pXPattern;
                        p1 = pSrc;
                        p2 = pDst;
                        width = dest_width;

                        while(width--)
                        {
                            /* 1、先取到需要处理的有效源象素 */
                            while (*pX++ == 0)
                            {
                                SRCPIXEL_INCREASE_8(p1);
                            }
                            
                            /* 2、有效源象素如果不需要透明处理，填充颜色
                            ** 如果透明，则只增加源地址
                            */
                            if (*p1 != index)
                            {
                                SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);
                                PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                            }
                            else
                            {
                                SRCPIXEL_INCREASE_8(p1);
                            }
                            
                            p2 += dst_pix_bytes;
                        }
                        
                        pDst += dst_line_bytes;
                        if (*pYPattern++ == 1)
                            pSrc += src_line_bytes;
                    }
                }
            }
            else
            {
                if (dest_width_origin >= src_width_origin)
                {
                    while(dest_height--)
                    {
                        while (*pYPattern++ == 0)
                        {
                            pSrc += src_line_bytes;
                        }
                        
                        pX = pXPattern;
                        p1 = pSrc;
                        p2 = pDst;
                        width = dest_width;

                        spixel = PhysColors[(*p1)];
                        while(width--)
                        {
                            if (*p1 != index)
                            {
                                PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                            }

                            p2 += dst_pix_bytes;
                            if (*pX++ == 1)
                            {
                                SRCPIXEL_INCREASE_8(p1);
                                spixel = PhysColors[(*p1)];
                            }
						}
                        
                        pDst += dst_line_bytes;
                        pSrc += src_line_bytes;
                    }
                }
                else
                {
                    while(dest_height--)
                    {
                        while (*pYPattern++ == 0)
                        {
                            pSrc += src_line_bytes;
                        }
                        
                        pX = pXPattern;
                        p1 = pSrc;
                        p2 = pDst;
                        width = dest_width;
                        while(width--)
                        {
                            while (*pX++ == 0)
                            {
                                SRCPIXEL_INCREASE_8(p1);
                            }
                            
                            if (*p1 != index)
                            {
                                SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);
                                PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                            }
                            else
                            {
                                SRCPIXEL_INCREASE_8(p1);
                            }
                            
                            p2 += dst_pix_bytes;
                        }
                        
                        pDst += dst_line_bytes;
                        pSrc += src_line_bytes;
                    }
                }
                
            }
        }
        else
        {
            if (dest_height_origin >= src_height_origin)
            {
                if (dest_width_origin >= src_width_origin)
                {
                    while(dest_height--)
                    {
                        pX = pXPattern;
                        p1 = pSrc;
                        p2 = pDst;
                        width = dest_width;
                        SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);
                        while(width--)
                        {
                            if (spixel != (uint32)pGraphParam->bk_color)
                            {
                                PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                            }
                            
                            p2 += dst_pix_bytes;
                            if (*pX++ == 1)
                            {
                                SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);
                            }
                        }
                        
                        pDst += dst_line_bytes;
                        if (*pYPattern++ == 1)
                            pSrc += src_line_bytes;
                    }
                }
                else
                {
                    while(dest_height--)
                    {
                        pX = pXPattern;
                        p1 = pSrc;
                        p2 = pDst;
                        width = dest_width;
                        while(width--)
                        {
                            while (*pX++ == 0)
                            {
                                SRCPIXEL_INCREASE_8(p1);
                            }
                            
                            SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);
                            if (spixel != (uint32)pGraphParam->bk_color)
                            {
                                PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                            }
                            
                            p2 += dst_pix_bytes;
                        }
                        
                        pDst += dst_line_bytes;
                        if (*pYPattern++ == 1)
                            pSrc += src_line_bytes;
                    }
                }
            }
            else
            {
                if (dest_width_origin >= src_width_origin)
                {
                    while(dest_height--)
                    {
                        while (*pYPattern++ == 0)
                        {
                            pSrc += src_line_bytes;
                        }
                        
                        pX = pXPattern;
                        p1 = pSrc;
                        p2 = pDst;
                        width = dest_width;
                        SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);
                        while(width--)
                        {
                            if (spixel != (uint32)pGraphParam->bk_color)
                            {
                                PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                            }
                            
                            p2 += dst_pix_bytes;
                            if (*pX++ == 1)
                            {
                                SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);
                            }
                        }
                        
                        pDst += dst_line_bytes;
                        pSrc += src_line_bytes;
                    }
                }
                else
                {
                    while(dest_height--)
                    {
                        while (*pYPattern++ == 0)
                        {
                            pSrc += src_line_bytes;
                        }
                        
                        pX = pXPattern;
                        p1 = pSrc;
                        p2 = pDst;
                        width = dest_width;
                        while(width--)
                        {
                            while (*pX++ == 0)
                            {
                                SRCPIXEL_INCREASE_8(p1);
                            }
                            
                            SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);
                            if (spixel != (uint32)pGraphParam->bk_color)
                            {
                                PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                            }
                            
                            p2 += dst_pix_bytes;
                        }
                        
                        pDst += dst_line_bytes;
                        pSrc += src_line_bytes;
                    }
                }
                
            }
        }
    }
    else //default is opqque
    {
        if (dest_height_origin >= src_height_origin)
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);
                    while(width--)
                    {
                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);
                        }
                    }

                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_8(p1);
                        }

                        SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);

                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
        }
        else
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);
                    while(width--)
                    {
                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                    
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);
                        }
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_8(p1);
                        }

                        SRCPIXEL_INCREASE_8_U(p1, spixel, PhysColors);

                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }

        }
    }

    return 1;
}

/*********************************************************************\
* Function	   Stretch16
* Purpose      Stretch DIBitmap with 16bit color and bitblt it 
               on destination.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int  Stretch16(PDEV pDev, PDIBITMAP pBitmap, 
                        PGRAPHPARAM pGraphParam, PDISTRBLTPARAM pDIStrParam)
{
    uint8       *pSrc, *pDst, *p1, *p2;
    uint32      spixel;
    int         dst_pix_bytes;
    int         src_line_bytes, dst_line_bytes;
    int32       rop;
    ROPFUNC     ropfunc;
    DWORD       dwRedMask, dwGreenMask, dwBlueMask;
    int         RLShift, RRShift, GLShift, GRShift, BLShift, BRShift;
    int         RLen, GLen, BLen;
    uint32      sred, sgreen, sblue;
    int         dest_width, dest_height;
    int         src_width, src_height;
    int         src_width_origin, src_height_origin;
    int         dest_width_origin, dest_height_origin; 
    BYTE        *pXPattern, *pYPattern;
    BYTE        *pX;
    int         width_offset, height_offset;
    int         width;

    src_line_bytes = (pBitmap->width * 2 + 3) / 4 * 4;
    dst_pix_bytes = pDev->pix_bits / 8;
    dst_line_bytes = pDev->line_bytes;
    
    if (! src_line_bytes || ! dst_line_bytes)
        return 0;

    if (pBitmap->pPalette == NULL)
    {
        dwRedMask   = 0x00007C00L;
        dwGreenMask = 0x000003E0L;
        dwBlueMask  = 0x0000001FL;
    }
    else
    {
        /* wwh removed this line for the address align problem. Can not be 
        ** converted to DWORD* . And the mask stored use little endian.
        ** On 2004/05/31.
        */
        //pRGBMask    = (DWORD*)pBitmap->pPalette;
        dwRedMask   = GET32_LITTLE((BYTE*)(pBitmap->pPalette));
        dwGreenMask = GET32_LITTLE((BYTE*)(pBitmap->pPalette) + 4);
        dwBlueMask  = GET32_LITTLE((BYTE*)(pBitmap->pPalette) + 8);
        ASSERT((dwRedMask & dwGreenMask & dwBlueMask) == 0);
    }

    /* WWH added on 2004/02/06 
    ** Fixed a bug in the GETRGBBYTE16_FROMMASK
    */
    GetRGBAMask(&RLShift, &RRShift, &RLen, dwRedMask);
    GetRGBAMask(&GLShift, &GRShift, &GLen, dwGreenMask);
    GetRGBAMask(&BLShift, &BRShift, &BLen, dwBlueMask);

    src_width = pDIStrParam->src_rc.x2 - pDIStrParam->src_rc.x1;
    src_height = pDIStrParam->src_rc.y2 - pDIStrParam->src_rc.y1;
    src_width_origin = pDIStrParam->src_width_origin;
    src_height_origin = pDIStrParam->src_height_origin;
    
    dest_width = pDIStrParam->dest_rc.x2 - pDIStrParam->dest_rc.x1;
    dest_height = pDIStrParam->dest_rc.y2 - pDIStrParam->dest_rc.y1;
    dest_width_origin = pDIStrParam->dest_width_origin;
    dest_height_origin = pDIStrParam->dest_height_origin;

    width_offset = pDIStrParam->src_x_offset;
    height_offset = pDIStrParam->src_y_offset;
    pXPattern = pDIStrParam->stretchpattern.bWidthPattern + width_offset;
    pYPattern = pDIStrParam->stretchpattern.bHeightPattern + height_offset;

    pSrc = (uint8*)pBitmap->pBuffer;

    if (pBitmap->height > 0)
    {
        pSrc += (pBitmap->height - height_offset - 1) * src_line_bytes + 
            width_offset * 2;
        src_line_bytes = -src_line_bytes;
    }
    else
        pSrc += height_offset * src_line_bytes + width_offset * 2;

    pDst = GETXYADDR(pDev, pDIStrParam->dest_rc.x1, pDIStrParam->dest_rc.y1);

    rop = SRop3ToRop2(pGraphParam->rop);
    ropfunc = GetRopFunc(rop);

    if (pGraphParam->bk_mode == BM_NEWTRANSPARENT)
    {
        if (dest_height_origin >= src_height_origin)
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;

                    SRCPIXEL_INCREASE_16_U(p1, spixel);
                    while(width--)
                    {
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }

                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_16_U(p1, spixel);
                        }
                    }

                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_16(p1);
                        }

                        SRCPIXEL_INCREASE_16_U(p1, spixel);
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
        }
        else
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    SRCPIXEL_INCREASE_16_U(p1, spixel);
                    while(width--)
                    {
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }
                    
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_16_U(p1, spixel);
                        }
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_16(p1);
                        }

                        SRCPIXEL_INCREASE_16_U(p1, spixel);
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }
                        
                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }

        }
    }
    else //default is opqque
    {
        if (dest_height_origin >= src_height_origin)
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    SRCPIXEL_INCREASE_16_U(p1, spixel);
                    while(width--)
                    {
                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_16_U(p1, spixel);
                        }
                    }

                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_16(p1);
                        }

                        SRCPIXEL_INCREASE_16_U(p1, spixel);

                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
        }
        else
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    SRCPIXEL_INCREASE_16_U(p1, spixel);
                    while(width--)
                    {
                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                    
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_16_U(p1, spixel);
                        }
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_16(p1);
                        }

                        SRCPIXEL_INCREASE_16_U(p1, spixel);

                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }

        }
    }

    return 1;
}

/*********************************************************************\
* Function	   Stretch24
* Purpose      Stretch DIBitmap with 24bit color(TRUE Color) and bitblt it 
               on destination.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int  Stretch24(PDEV pDev, PDIBITMAP pBitmap, 
                        PGRAPHPARAM pGraphParam, PDISTRBLTPARAM pDIStrParam)
{
    uint8       *pSrc, *pDst, *p1, *p2;
    uint32      spixel;
    int         dst_pix_bytes;
    int         src_line_bytes, dst_line_bytes;
    int32       rop;
    ROPFUNC     ropfunc;
    int         dest_width, dest_height;
    int         src_width, src_height;
    int         src_width_origin, src_height_origin;
    int         dest_width_origin, dest_height_origin; 
    BYTE        *pXPattern, *pYPattern;
    BYTE        *pX;
    int         width_offset, height_offset;
    int         width;

    src_line_bytes = (pBitmap->width * 3 + 3) / 4 * 4;
    dst_pix_bytes = pDev->pix_bits / 8;
    dst_line_bytes = pDev->line_bytes;
    
    if (! src_line_bytes || ! dst_line_bytes)
        return 0;
   
    src_width = pDIStrParam->src_rc.x2 - pDIStrParam->src_rc.x1;
    src_height = pDIStrParam->src_rc.y2 - pDIStrParam->src_rc.y1;
    src_width_origin = pDIStrParam->src_width_origin;
    src_height_origin = pDIStrParam->src_height_origin;
    
    dest_width = pDIStrParam->dest_rc.x2 - pDIStrParam->dest_rc.x1;
    dest_height = pDIStrParam->dest_rc.y2 - pDIStrParam->dest_rc.y1;
    dest_width_origin = pDIStrParam->dest_width_origin;
    dest_height_origin = pDIStrParam->dest_height_origin;

    width_offset = pDIStrParam->src_x_offset;
    height_offset = pDIStrParam->src_y_offset;
    pXPattern = pDIStrParam->stretchpattern.bWidthPattern + width_offset;
    pYPattern = pDIStrParam->stretchpattern.bHeightPattern + height_offset;

    pSrc = (uint8*)pBitmap->pBuffer;

    if (pBitmap->height > 0)
    {
        pSrc += (pBitmap->height - height_offset - 1) * src_line_bytes + 
            width_offset * 3;
        src_line_bytes = -src_line_bytes;
    }
    else
        pSrc += height_offset * src_line_bytes + width_offset * 3;

    pDst = GETXYADDR(pDev, pDIStrParam->dest_rc.x1, pDIStrParam->dest_rc.y1);

    rop = SRop3ToRop2(pGraphParam->rop);
    ropfunc = GetRopFunc(rop);

    if (pGraphParam->bk_mode == BM_NEWTRANSPARENT)
    {
        if (dest_height_origin >= src_height_origin)
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;

                    SRCPIXEL_INCREASE_24_U(p1, spixel);
                    while(width--)
                    {
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }

                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_24_U(p1, spixel);
                        }
                    }

                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_24(p1);
                        }

                        SRCPIXEL_INCREASE_24_U(p1, spixel);
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
        }
        else
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    SRCPIXEL_INCREASE_24_U(p1, spixel);
                    while(width--)
                    {
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }
                    
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_24_U(p1, spixel);
                        }
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_24(p1);
                        }

                        SRCPIXEL_INCREASE_24_U(p1, spixel);
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }
                        
                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }

        }
    }
    else //default is opqque
    {
        if (dest_height_origin >= src_height_origin)
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    SRCPIXEL_INCREASE_24_U(p1, spixel);
                    while(width--)
                    {
                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_24_U(p1, spixel);
                        }
                    }

                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_24(p1);
                        }

                        SRCPIXEL_INCREASE_24_U(p1, spixel);

                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
        }
        else
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    SRCPIXEL_INCREASE_24_U(p1, spixel);
                    while(width--)
                    {
                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                    
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_24_U(p1, spixel);
                        }
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_24(p1);
                        }

                        SRCPIXEL_INCREASE_24_U(p1, spixel);

                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }

        }
    }

    return 1;
}

/* SPA */
#define SRCPIXEL_INCREASE_32_ALPHA_P(p1) {\
    sblue = *p1++;      \
    sgreen = *p1++;     \
    sred = *p1++;       \
    salpha = *p1++;     \
}

#define PUTPIXEL_ALPHA_P(p2)  {   \
    GETRGBBYTE(p2, dred, dgreen, dblue);                                \
    INT8_255DIVIDED(sred * salpha + (255 - salpha) * dred, red);        \
    INT8_255DIVIDED(sgreen * salpha + (255 - salpha) * dgreen, green);  \
    INT8_255DIVIDED(sblue * salpha + (255 - salpha) * dblue, blue);     \
    pixel = REALIZECOLOR(red, green, blue);                             \
    PUTPIXEL(p2, pixel);                                                \
}

/* SPA & SCA */
#define SRCPIXEL_INCREASE_32_ALPHA_CP(p1) {\
    sblue = (*p1++ * uSCA) >> 8;      \
    sgreen = (*p1++ * uSCA) >> 8;     \
    sred = (*p1++ * uSCA) >> 8;       \
    salpha = (*p1++ * uSCA) >> 8;     \
}

#define PUTPIXEL_ALPHA_CP(p2)  {   \
    GETRGBBYTE(p2, dred, dgreen, dblue);                                \
    INT8_255DIVIDED(sred * salpha + (255 - salpha * uSCA) * dred, red);        \
    INT8_255DIVIDED(sgreen * salpha + (255 - salpha * uSCA) * dgreen, green);  \
    INT8_255DIVIDED(sblue * salpha + (255 - salpha * uSCA) * dblue, blue);     \
    pixel = REALIZECOLOR(red, green, blue);                             \
    PUTPIXEL(p2, pixel);                                                \
}

/* SCA */
#define SRCPIXEL_INCREASE_32_ALPHA_C(p1) {\
    sblue = *p1++;      \
    sgreen = *p1++;     \
    sred = *p1++;       \
    p1++;     \
}

#define PUTPIXEL_ALPHA_C(p2)  {   \
    GETRGBBYTE(p2, dred, dgreen, dblue);                                \
    INT8_255DIVIDED(sred * uSCA + (255 - uSCA) * dred, red);        \
    INT8_255DIVIDED(sgreen * uSCA + (255 - uSCA) * dgreen, green);  \
    INT8_255DIVIDED(sblue * uSCA + (255 - uSCA) * dblue, blue);     \
    pixel = REALIZECOLOR(red, green, blue);                             \
    PUTPIXEL(p2, pixel);                                                \
}


/*********************************************************************\
* Function	   Stretch32
* Purpose      Stretch DIBitmap with 24bit color(TRUE Color) and bitblt it 
               on destination.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int  Stretch32(PDEV pDev, PDIBITMAP pBitmap, 
                        PGRAPHPARAM pGraphParam, PDISTRBLTPARAM pDIStrParam)
{
    uint8       *pSrc, *pDst, *p1, *p2;
    uint32      pixel, spixel;
    int         dst_pix_bytes;
    int         src_line_bytes, dst_line_bytes;
    int32       rop;
    ROPFUNC     ropfunc;
    int         dest_width, dest_height;
    int         src_width, src_height;
    int         src_width_origin, src_height_origin;
    int         dest_width_origin, dest_height_origin; 
    BYTE        *pXPattern, *pYPattern;
    BYTE        *pX;
    int         width_offset, height_offset;
    int         width;

    src_line_bytes = pBitmap->width * 4;
    dst_pix_bytes = pDev->pix_bits / 8;
    dst_line_bytes = pDev->line_bytes;
    
    if (! src_line_bytes || ! dst_line_bytes)
        return 0;
   
    src_width = pDIStrParam->src_rc.x2 - pDIStrParam->src_rc.x1;
    src_height = pDIStrParam->src_rc.y2 - pDIStrParam->src_rc.y1;
    src_width_origin = pDIStrParam->src_width_origin;
    src_height_origin = pDIStrParam->src_height_origin;
    
    dest_width = pDIStrParam->dest_rc.x2 - pDIStrParam->dest_rc.x1;
    dest_height = pDIStrParam->dest_rc.y2 - pDIStrParam->dest_rc.y1;
    dest_width_origin = pDIStrParam->dest_width_origin;
    dest_height_origin = pDIStrParam->dest_height_origin;

    width_offset = pDIStrParam->src_x_offset;
    height_offset = pDIStrParam->src_y_offset;
    pXPattern = pDIStrParam->stretchpattern.bWidthPattern + width_offset;
    pYPattern = pDIStrParam->stretchpattern.bHeightPattern + height_offset;

    pSrc = (uint8*)pBitmap->pBuffer;

    if (pBitmap->height > 0)
    {
        pSrc += (pBitmap->height - height_offset - 1) * src_line_bytes + 
            width_offset * 4;
        src_line_bytes = -src_line_bytes;
    }
    else
        pSrc += height_offset * src_line_bytes + width_offset * 4;

    pDst = GETXYADDR(pDev, pDIStrParam->dest_rc.x1, pDIStrParam->dest_rc.y1);

    if (pGraphParam->bk_mode == BM_ALPHATRANSPARENT)
    {
        uint32  salpha;
        uint32  dred, dgreen, dblue;
        uint32  sred, sgreen, sblue;
        uint32   red, green, blue;

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
                return 1;
            
            if (pGraphParam->pBlend->AlphaFormat == AC_SRC_ALPHA)
            {
                /* 同时使用SPA和SCA */
                uSCA <<= 8;
                INT8_255DIVIDED(uSCA, uSCA);
                if (dest_height_origin >= src_height_origin)
                {
                    if (dest_width_origin >= src_width_origin)
                    {
                        while(dest_height--)
                        {
                            pX = pXPattern;
                            p1 = pSrc;
                            p2 = pDst;
                            width = dest_width;

                            SRCPIXEL_INCREASE_32_ALPHA_CP(p1);
                            while(width--)
                            {
                                PUTPIXEL_ALPHA_CP(p2);
                                p2 += dst_pix_bytes;
                                if (*pX++ == 1)
                                {
                                    SRCPIXEL_INCREASE_32_ALPHA_CP(p1);
                                }
                            }

                            pDst += dst_line_bytes;
                            if (*pYPattern++ == 1)
                                pSrc += src_line_bytes;
                        }
                    }
                    else
                    {
                        while(dest_height--)
                        {
                            pX = pXPattern;
                            p1 = pSrc;
                            p2 = pDst;
                            width = dest_width;
                            while(width--)
                            {
                                while (*pX++ == 0)
                                {
                                    SRCPIXEL_INCREASE_32(p1);
                                }

                                SRCPIXEL_INCREASE_32_ALPHA_CP(p1);
                                PUTPIXEL_ALPHA_CP(p2);

                                p2 += dst_pix_bytes;
                            }
                    
                            pDst += dst_line_bytes;
                            if (*pYPattern++ == 1)
                                pSrc += src_line_bytes;
                        }
                    }
                }
                else
                {
                    if (dest_width_origin >= src_width_origin)
                    {
                        while(dest_height--)
                        {
                            while (*pYPattern++ == 0)
                            {
                                pSrc += src_line_bytes;
                            }

                            pX = pXPattern;
                            p1 = pSrc;
                            p2 = pDst;
                            width = dest_width;
                            SRCPIXEL_INCREASE_32_ALPHA_CP(p1);
                            while(width--)
                            {
                                PUTPIXEL_ALPHA_CP(p2);
                    
                                p2 += dst_pix_bytes;
                                if (*pX++ == 1)
                                {
                                    SRCPIXEL_INCREASE_32_ALPHA_CP(p1);
                                }
                            }
                    
                            pDst += dst_line_bytes;
                            pSrc += src_line_bytes;
                        }
                    }
                    else
                    {
                        while(dest_height--)
                        {
                            while (*pYPattern++ == 0)
                            {
                                pSrc += src_line_bytes;
                            }

                            pX = pXPattern;
                            p1 = pSrc;
                            p2 = pDst;
                            width = dest_width;
                            while(width--)
                            {
                                while (*pX++ == 0)
                                {
                                    SRCPIXEL_INCREASE_32(p1);
                                }

                                SRCPIXEL_INCREASE_32_ALPHA_CP(p1);
                                PUTPIXEL_ALPHA_CP(p2);
                        
                                p2 += dst_pix_bytes;
                            }
                    
                            pDst += dst_line_bytes;
                            pSrc += src_line_bytes;
                        }
                    }

                }
            }
            else
            {
                /* 只使用SCA */
                if (dest_height_origin >= src_height_origin)
                {
                    if (dest_width_origin >= src_width_origin)
                    {
                        while(dest_height--)
                        {
                            pX = pXPattern;
                            p1 = pSrc;
                            p2 = pDst;
                            width = dest_width;

                            SRCPIXEL_INCREASE_32_ALPHA_C(p1);
                            while(width--)
                            {
                                PUTPIXEL_ALPHA_C(p2);
                                p2 += dst_pix_bytes;
                                if (*pX++ == 1)
                                {
                                    SRCPIXEL_INCREASE_32_ALPHA_C(p1);
                                }
                            }

                            pDst += dst_line_bytes;
                            if (*pYPattern++ == 1)
                                pSrc += src_line_bytes;
                        }
                    }
                    else
                    {
                        while(dest_height--)
                        {
                            pX = pXPattern;
                            p1 = pSrc;
                            p2 = pDst;
                            width = dest_width;
                            while(width--)
                            {
                                while (*pX++ == 0)
                                {
                                    SRCPIXEL_INCREASE_32(p1);
                                }

                                SRCPIXEL_INCREASE_32_ALPHA_C(p1);
                                PUTPIXEL_ALPHA_C(p2);

                                p2 += dst_pix_bytes;
                            }
                    
                            pDst += dst_line_bytes;
                            if (*pYPattern++ == 1)
                                pSrc += src_line_bytes;
                        }
                    }
                }
                else
                {
                    if (dest_width_origin >= src_width_origin)
                    {
                        while(dest_height--)
                        {
                            while (*pYPattern++ == 0)
                            {
                                pSrc += src_line_bytes;
                            }

                            pX = pXPattern;
                            p1 = pSrc;
                            p2 = pDst;
                            width = dest_width;
                            SRCPIXEL_INCREASE_32_ALPHA_C(p1);
                            while(width--)
                            {
                                PUTPIXEL_ALPHA_C(p2);
                    
                                p2 += dst_pix_bytes;
                                if (*pX++ == 1)
                                {
                                    SRCPIXEL_INCREASE_32_ALPHA_C(p1);
                                }
                            }
                    
                            pDst += dst_line_bytes;
                            pSrc += src_line_bytes;
                        }
                    }
                    else
                    {
                        while(dest_height--)
                        {
                            while (*pYPattern++ == 0)
                            {
                                pSrc += src_line_bytes;
                            }

                            pX = pXPattern;
                            p1 = pSrc;
                            p2 = pDst;
                            width = dest_width;
                            while(width--)
                            {
                                while (*pX++ == 0)
                                {
                                    SRCPIXEL_INCREASE_32(p1);
                                }

                                SRCPIXEL_INCREASE_32_ALPHA_C(p1);
                                PUTPIXEL_ALPHA_C(p2);
                        
                                p2 += dst_pix_bytes;
                            }
                    
                            pDst += dst_line_bytes;
                            pSrc += src_line_bytes;
                        }
                    }
                }
            }
        }
        
        /* 只使用SPA进行贴图 */
        if (dest_height_origin >= src_height_origin)
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;

                    SRCPIXEL_INCREASE_32_ALPHA_P(p1);
                    while(width--)
                    {
                        PUTPIXEL_ALPHA_P(p2);
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_32_ALPHA_P(p1);
                        }
                    }

                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_32(p1);
                        }

                        SRCPIXEL_INCREASE_32_ALPHA_P(p1);
                        PUTPIXEL_ALPHA_P(p2);

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
        }
        else
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    SRCPIXEL_INCREASE_32_ALPHA_P(p1);
                    while(width--)
                    {
                        PUTPIXEL_ALPHA_P(p2);
                    
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_32_ALPHA_P(p1);
                        }
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_32(p1);
                        }

                        SRCPIXEL_INCREASE_32_ALPHA_P(p1);
                        PUTPIXEL_ALPHA_P(p2);
                        
                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }

        }

        return 1;
    }

    rop = SRop3ToRop2(pGraphParam->rop);
    ropfunc = GetRopFunc(rop);

    if (pGraphParam->bk_mode == BM_NEWTRANSPARENT)
    {
        if (dest_height_origin >= src_height_origin)
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;

                    SRCPIXEL_INCREASE_32_U(p1, spixel);
                    while(width--)
                    {
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }

                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_32_U(p1, spixel);
                        }
                    }

                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_32(p1);
                        }

                        SRCPIXEL_INCREASE_32_U(p1, spixel);
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
        }
        else
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    SRCPIXEL_INCREASE_32_U(p1, spixel);
                    while(width--)
                    {
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }
                    
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_32_U(p1, spixel);
                        }
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_32(p1);
                        }

                        SRCPIXEL_INCREASE_32_U(p1, spixel);
                        if (spixel != (uint32)pGraphParam->bk_color)
                        {
                            PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                        }
                        
                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }

        }
    }
    else //default is opqque
    {
        if (dest_height_origin >= src_height_origin)
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    SRCPIXEL_INCREASE_32_U(p1, spixel);
                    while(width--)
                    {
                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_32_U(p1, spixel);
                        }
                    }

                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_32(p1);
                        }

                        SRCPIXEL_INCREASE_32_U(p1, spixel);

                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    if (*pYPattern++ == 1)
                        pSrc += src_line_bytes;
                }
            }
        }
        else
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    SRCPIXEL_INCREASE_32_U(p1, spixel);
                    while(width--)
                    {
                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);
                    
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            SRCPIXEL_INCREASE_32_U(p1, spixel);
                        }
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }
            else
            {
                while(dest_height--)
                {
                    while (*pYPattern++ == 0)
                    {
                        pSrc += src_line_bytes;
                    }

                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    width = dest_width;
                    while(width--)
                    {
                        while (*pX++ == 0)
                        {
                            SRCPIXEL_INCREASE_32(p1);
                        }

                        SRCPIXEL_INCREASE_32_U(p1, spixel);

                        PUTPIXEL_ROPFUNC(p2, spixel, ropfunc);

                        p2 += dst_pix_bytes;
                    }
                    
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }

        }
    }

    return 1;
}
