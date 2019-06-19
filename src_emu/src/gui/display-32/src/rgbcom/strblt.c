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
* Function	   GetStretchPatternLine
* Purpose      
    得到Stretch的pattern。按照步长计算，每次短边需要加一个象素时，
    pattern中计1，不变时计0 * Params	   
* Return	 	   
* Remarks
**********************************************************************/
static void GetStretchPatternLine(int nStep, int nShort, BYTE* p)
{
    int i;
    int x1 = 0, x2 = 0;

    for (i = 0; i < nStep; i ++)
    {
        x1 += nShort;
        if (x1 >= nStep)
        {
            *p++ = 1;
            x1 -= nStep;
        }
        else
            *p++ = 0;
    }

    *p = 1;
    ASSERT(*(--p) == 1);

    return;
}

/*********************************************************************\
* Function	   GetStretchPattern
* Purpose      
    得到X和Y向的stretch pattern
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void GetStretchPattern(PRECT pDestRect, PRECT pSrcRect, 
                              PSTRETCHPATTERN p)
{
    int nStep;
    int nShort;

    nStep = pDestRect->x2 - pDestRect->x1;
    nShort = pSrcRect->x2 - pSrcRect->x1;
    if (nStep < nShort)
        EXCHANGE(nShort, nStep);

/*
    p->wWidthStep = (WORD)nStep;
    p->wWidthShort = (WORD)nShort;
*/
    GetStretchPatternLine(nStep, nShort, p->bWidthPattern);

    nStep = pDestRect->y2 - pDestRect->y1;
    nShort = pSrcRect->y2 - pSrcRect->y1;
    if (nStep < nShort)
        EXCHANGE(nShort, nStep);
    
/*
    p->wHeightStep = nStep;
    p->wHeightShort = nShort;
*/
    GetStretchPatternLine(nStep, nShort, p->bHeightPattern);

    return;
}

/*********************************************************************\
* Function	   GetSrcOffset
* Purpose     
    得到源的起始偏移量，因为可能裁剪故而此处确定源位图中实际左上角坐标 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int GetSrcOffset(BYTE* pPattern, int dst_width, int src_width, 
                            int dst_offset)
{
    int offset = 0;

    if (dst_width >= src_width)
    {
        while (dst_offset--)
        {
            if (*(pPattern)++ == 1)
                offset++;
        }
    }
    else
    {
        while (dst_offset--)
        {
            while (*(pPattern)++ == 0)
            {
                offset++;
            }

            offset++;
        }
    }

    return offset;
}

/*********************************************************************\
* Function	   StretchBlt
* Purpose      设备相关位图StretchBlt。
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int StretchBlt(PDEV pDstDev, PRECT pDestRect, PDEV pSrcDev, 
                        PRECT pSrcRect, PGRAPHPARAM pGraphParam)
{
    uint8   *p1, *p2;
    uint8   *pSrc, *pDst;
    uint32  pixel;
    int     src_pix_bytes, dst_pix_bytes, src_line_bytes, dst_line_bytes;
    int     src_width, src_height, dest_width, dest_height; 
    int     src_width_origin, src_height_origin;
    int     dest_width_origin, dest_height_origin; 
    RECT    rc, src_rect; //, temp_rc;
    RECT    rcDevClip, rcSrcClip, rcSrcDev;
    uint32  rop;
    ROPFUNC ropfunc;
    /* wwh modified on 2004/04/28 for the given bitmap width or height is grater 
    ** than the difined macro DISP_MAX_WIDTH or DISP_MAX_HEIGHT.
    */
    STRETCHPATTERN StretchPattern;
    BYTE    bWidthPattern[DISP_MAX_WIDTH];
    BYTE    bHeightPattern[DISP_MAX_HEIGHT];
    BYTE *pXPattern, *pYPattern;
    BYTE *pX;
    int width_offset, height_offset;
    int width;
    PMEMDEV pMemDev;
    int16       bk_mode;

    if (pDstDev == NULL || pSrcDev == NULL)
        return -1;
    if (pGraphParam == NULL)
        return -1;

    if (!IsValidRect(pDestRect))
        return -1;
    if (!IsValidRect(pSrcRect))
        return -1;

    src_width = pSrcRect->x2 - pSrcRect->x1;
    src_height = pSrcRect->y2 - pSrcRect->y1;

    dest_width = pDestRect->x2 - pDestRect->x1; 
    dest_height = pDestRect->y2 - pDestRect->y1; 

    if (src_width == dest_width && src_height == dest_height)
    {
        return BitBlt(pDstDev, pDestRect, pSrcDev, pSrcRect->x1, 
            pSrcRect->y1, pGraphParam);
    }

    bk_mode = pGraphParam->bk_mode;
    if (bk_mode == BM_ALPHATRANSPARENT || bk_mode == BM_NEWALPHATRANSPARENT)
    {
        if (pGraphParam->pBlend == NULL || 
            pGraphParam->pBlend->BlendFlags != 0)
            return 0;

        /* 不能是AC_SRC_ALPHA或其他值，这里只能处理仅有SCA情况，
        ** 这样源不必是32位的含alpha通道的位图 
        */
        if (pGraphParam->pBlend->AlphaFormat != 0)
            return 0;

        /* 只支持源位图覆盖目标位图 */
        if (pGraphParam->pBlend->BlendOp != AC_SRC_OVER)
            return 0;

        /* 完全透明，直接返回 */
        if (pGraphParam->pBlend->SourceConstantAlpha == 0)
            return 0;

        if (pGraphParam->pBlend->SourceConstantAlpha == 0xFF)
        {
            if (bk_mode == BM_ALPHATRANSPARENT)
            {
                /* 完全不透明，直接贴图 */
                bk_mode = BM_OPAQUE;
            }
            else //BM_NEWALPHATRANSPARENT
            {
                bk_mode = BM_NEWTRANSPARENT;
            }
        }
        else
        {
            /* WWH modified on 2004/02/06, not support the new alpha */
            bk_mode = BM_ALPHATRANSPARENT;
        }
    }

    src_width_origin = src_width;
    src_height_origin = src_height;
    dest_width_origin = dest_width;
    dest_height_origin = dest_height;
    CopyRect(&rc, pDestRect);
    CopyRect(&src_rect, pSrcRect);

#if 1
    rcDevClip.x1 = rcDevClip.y1 = 0;
    rcDevClip.x2 = pDstDev->width;
    rcDevClip.y2 = pDstDev->height;

    //首先，剪裁矩形与目的设备大小求交集
    if (pGraphParam != NULL && (pGraphParam->flags & GPF_CLIP_RECT))
    {  
        ClipRect(&rcDevClip, &pGraphParam->clip_rect);
        if (!IsValidRect(&rcDevClip))
            return -1;
    }

    rcSrcDev.x1 = 0;
    rcSrcDev.y1 = 0;
    rcSrcDev.x2 = pSrcDev->width;
    rcSrcDev.y2 = pSrcDev->height;

    //源设备所在矩形映射到目的矩形坐标系下
/*
    rcDestDev.x1 = (rcSrcDev.x1 - pSrcRect->x1) * dest_width / src_width
        + pDestRect->x1;
    rcDestDev.y1 = (rcSrcDev.y1 - pSrcRect->y1) * dest_height / src_height
        + pDestRect->y1;
    //src_width对齐
    rcDestDev.x2 = ((rcSrcDev.x2 - pSrcRect->x1) * dest_width + src_width
        - 1) / src_width + pDestRect->x1;
    rcDestDev.y2 = ((rcSrcDev.y2 - pSrcRect->y1) * dest_height + src_height
        - 1) / src_height + pDestRect->y1;

*/
    //用经过剪裁的目的矩形与源设备矩形进行剪裁
    //用新的剪裁矩形与目的矩形进行剪裁
    
    if (!ClipRect(&rc, &rcDevClip))
        return -1;
//    if (!ClipRect(&rc, &rcDestDev))
//        return -1;

    // 其次，将目的的剪裁矩形映射到源矩形坐标系下
    rcSrcClip.x1 = (rcDevClip.x1 - pDestRect->x1) * src_width
        / dest_width + pSrcRect->x1;
    rcSrcClip.x2 = ((rcDevClip.x2 - pDestRect->x1) * src_width + 
        dest_width - 1) / dest_width + pSrcRect->x1;
    rcSrcClip.y1 = (rcDevClip.y1 - pDestRect->y1) * src_height
        / dest_height + pSrcRect->y1;
    rcSrcClip.y2 = ((rcDevClip.y2 - pDestRect->y1) * src_height + 
        dest_height - 1) / dest_height + pSrcRect->y1;

    //源矩形与经过映射的目的矩形求交集
    if (!ClipRect(&src_rect, &rcSrcClip))
        return -1;
    
    if (!ClipRect(&src_rect, &rcSrcDev))
        return -1;

#else 

    //根据目的设备进行剪裁
    if (pDev->mode == DEV_PHYSICAL)
    {
        ClipByScreen(&rc);
    }
    else
    {
        if (pDev->mode == DEV_MEMORY || pSrcDev->mode == DEV_BITMAP)
        {
            ClipByDev(&rc, (PMEMDEV)pDev);
        }
        else
            return -1;
    }

    if (pGraphParam != NULL && (pGraphParam->flags & GPF_CLIP_RECT))
        ClipRect(&rc, &pGraphParam->clip_rect);
    
    if (!IsValidRect(&rc))
        return -1;

    //根据设备类型对源矩形剪裁
    if (pSrcDev->mode == DEV_PHYSICAL)
    {   
        ClipByScreen(&src_rect);
    }
    else
    {
        if (pSrcDev->mode == DEV_MEMORY || pSrcDev->mode == DEV_BITMAP)
        {
            ClipByDev(&src_rect, (PMEMDEV)pSrcDev);
        }
        else
            return -1;
    }

    if (!IsValidRect(&src_rect))
        return -1;

    //以(pDestRect->x1,pDestRect->y1)为原点,调整目的矩形 
    rc.x1 -= pDestRect->x1;
    rc.y1 -= pDestRect->y1;
    rc.x2 -= pDestRect->x1;
    rc.y2 -= pDestRect->y1;

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
        rc.x1 += pDestRect->x1;
        rc.y1 += pDestRect->y1;
        rc.x2 += pDestRect->x1;
        rc.y2 += pDestRect->y1;
    }
    else
    {   
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

        //目的矩形的坐标恢复到原坐标系下
        rc.x1 = rc.x1 * dest_width / src_width  + pDestRect->x1;
        rc.y1 = rc.y1 * dest_height / src_height + pDestRect->y1;
        rc.x2 = rc.x2 * dest_width / src_width  + pDestRect->x1;
        rc.y2 = rc.y2 * dest_height / src_height + pDestRect->y1;
    }
#endif

    /* WWH modified on 2003/12/10 
    ** Some input parameter may be larger than device and should be cliped
    ** first. 
    ** This problem may be considerd later.
    */
    {
        RECT rcSrc;
        int maxlen;
        CopyRect(&rcSrc, pSrcRect);
        ClipByDev(&rcSrc, pSrcDev);

    /* wwh modified on 2004/04/28 for the given bitmap width or height is grater 
    ** than the difined macro DISP_MAX_WIDTH or DISP_MAX_HEIGHT.
    */
        maxlen = max(dest_width_origin, src_width_origin) + 1;
        if (maxlen > DISP_MAX_WIDTH)
        {
            StretchPattern.bWidthPattern = malloc(maxlen);
            if (StretchPattern.bWidthPattern == NULL)
                return -1;
        }
        else
            StretchPattern.bWidthPattern = bWidthPattern;

        maxlen = max(dest_height_origin, src_height_origin) + 1;
        if (maxlen > DISP_MAX_HEIGHT)
        {
            StretchPattern.bHeightPattern = malloc(maxlen);
            if (StretchPattern.bHeightPattern == NULL)
                return -1;
        }
        else
            StretchPattern.bHeightPattern = bHeightPattern;

        GetStretchPattern(pDestRect, &rcSrc, &StretchPattern);
    }

    pXPattern = StretchPattern.bWidthPattern;
    pYPattern = StretchPattern.bHeightPattern;
    width_offset = GetSrcOffset(pXPattern, dest_width_origin, 
        src_width_origin, rc.x1 - pDestRect->x1);
    height_offset = GetSrcOffset(pYPattern, dest_height_origin, 
        src_height_origin, rc.y1 - pDestRect->y1);

    pXPattern += width_offset;
    pYPattern += height_offset;
    _ExcludeCursor(pDstDev, &rc);
    
    //重新计算宽高
    src_width = src_rect.x2 - src_rect.x1;
    src_height = src_rect.y2 - src_rect.y1;
    dest_width = rc.x2 - rc.x1; 
    dest_height = rc.y2 - rc.y1; 

    src_pix_bytes = pSrcDev->pix_bits / 8;
    dst_pix_bytes = pDstDev->pix_bits / 8;
    dst_line_bytes = pDstDev->line_bytes;

    if (pSrcDev == pDstDev && IntersectRect(&src_rect, &rc))
    {
        int nRet, i;
        uint8 *pMemData;

        src_width = src_width_origin - width_offset;
        src_height = src_height_origin - height_offset;

        nRet = CreateMemoryDevice(pSrcDev, src_width, src_height, NULL);
        if (nRet < 0)
            goto err_RET;
        pMemDev = malloc(nRet);
        CreateMemoryDevice(pSrcDev, src_width, src_height, pMemDev);

        // aligned by 4 bytes
        src_line_bytes = pMemDev->line_bytes;
        pMemData = pMemDev->pdata;

        width_offset += pSrcRect->x1;
        height_offset += pSrcRect->y1;
        pSrc = GETXYADDR(pSrcDev, width_offset, height_offset);
        for (i = 0; i < src_height; i ++)
        {
//            pSrc = GETXYADDR(pSrcDev, src_rect.x1, src_rect.y1 + i);
            memcpy(pMemData, pSrc, src_line_bytes);
            pMemData += src_line_bytes;
            pSrc += src_line_bytes;
        }

        pSrc = pMemDev->pdata;
    }

    else
    {
        src_line_bytes = pSrcDev->line_bytes;
//        pSrc = GETXYADDR(pSrcDev, src_rect.x1, src_rect.y1);
        width_offset += pSrcRect->x1;
        height_offset += pSrcRect->y1;
        pSrc = GETXYADDR(pSrcDev, width_offset, height_offset);
    }

    pDst = GETXYADDR(pDstDev, rc.x1, rc.y1);

    rop = SRop3ToRop2(pGraphParam->rop);
    ropfunc = GetRopFunc(rop);

    if (bk_mode == BM_ALPHATRANSPARENT)
    {
        uint32   uSCA;
        uint32   dred, dgreen, dblue;
        uint32   sred, sgreen, sblue;
        uint32   red, green, blue;

        uSCA = pGraphParam->pBlend->SourceConstantAlpha;

        if (dest_height_origin >= src_height_origin)
        {
            if (dest_width_origin >= src_width_origin)
            {
                while(dest_height--)
                {
                    pX = pXPattern;
                    p1 = pSrc;
                    p2 = pDst;
                    GETRGBBYTE(p1, sred, sgreen, sblue);
                    width = dest_width;
                    while(width--)
                    {
                        GETRGBBYTE(p2, dred, dgreen, dblue);
                        /* dred = (sred * uSCA + dred * (255 - uSCA)) / 255*/
                        INT8_255DIVIDED(sred * uSCA + (255 - uSCA) * dred, red);
                        INT8_255DIVIDED(sgreen * uSCA + (255 - uSCA) * dgreen, green);
                        INT8_255DIVIDED(sblue * uSCA + (255 - uSCA) * dblue, blue);

                        pixel = REALIZECOLOR(red, green, blue);
                        PUTPIXEL(p2, pixel);
                    
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            p1 += src_pix_bytes;
                            GETRGBBYTE(p1, sred, sgreen, sblue);
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
                            p1 += src_pix_bytes;
                        }

                        GETRGBBYTE(p1, sred, sgreen, sblue);
                        GETRGBBYTE(p2, dred, dgreen, dblue);
                        /* dred = (sred * uSCA + dred * (255 - uSCA)) / 255*/
                        INT8_255DIVIDED(sred * uSCA + (255 - uSCA) * dred, red);
                        INT8_255DIVIDED(sgreen * uSCA + (255 - uSCA) * dgreen, green);
                        INT8_255DIVIDED(sblue * uSCA + (255 - uSCA) * dblue, blue);

                        pixel = REALIZECOLOR(red, green, blue);
                        PUTPIXEL(p2, pixel);

                        p2 += dst_pix_bytes;
                        p1 += src_pix_bytes;
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
                    GETRGBBYTE(p1, sred, sgreen, sblue);
                    width = dest_width;
                    while(width--)
                    {
                        GETRGBBYTE(p2, dred, dgreen, dblue);
                        /* dred = (sred * uSCA + dred * (255 - uSCA)) / 255*/
                        INT8_255DIVIDED(sred * uSCA + (255 - uSCA) * dred, red);
                        INT8_255DIVIDED(sgreen * uSCA + (255 - uSCA) * dgreen, green);
                        INT8_255DIVIDED(sblue * uSCA + (255 - uSCA) * dblue, blue);

                        pixel = REALIZECOLOR(red, green, blue);
                        PUTPIXEL(p2, pixel);
                
                        p2 += dst_pix_bytes;
                        if (*pX++ == 1)
                        {
                            p1 += src_pix_bytes;
                            GETRGBBYTE(p1, sred, sgreen, sblue);
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
                            p1 += src_pix_bytes;
                        }

                        GETRGBBYTE(p1, sred, sgreen, sblue);
                        GETRGBBYTE(p2, dred, dgreen, dblue);
                        /* dred = (sred * uSCA + dred * (255 - uSCA)) / 255*/
                        INT8_255DIVIDED(sred * uSCA + (255 - uSCA) * dred, red);
                        INT8_255DIVIDED(sgreen * uSCA + (255 - uSCA) * dgreen, green);
                        INT8_255DIVIDED(sblue * uSCA + (255 - uSCA) * dblue, blue);

                        pixel = REALIZECOLOR(red, green, blue);
                        PUTPIXEL(p2, pixel);

                        p2 += dst_pix_bytes;
                        p1 += src_pix_bytes;
                    }
                
                    pDst += dst_line_bytes;
                    pSrc += src_line_bytes;
                }
            }
        }
         
        goto RET;
    }

    switch(rop) 
    {
    case ROP_SRC:

        if (bk_mode != BM_NEWTRANSPARENT)
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
                        while(width--)
                        {
                            pixel = GETPIXEL(p1);
                            PUTPIXEL(p2, pixel);
                            
                            p2 += dst_pix_bytes;
                            if (*pX++ == 1)
                                p1 += src_pix_bytes;
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
                                p1 += src_pix_bytes;
                            }

                            pixel = GETPIXEL(p1);
                            PUTPIXEL(p2, pixel);

                            p2 += dst_pix_bytes;
                            p1 += src_pix_bytes;
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
                        while(width--)
                        {
                            pixel = GETPIXEL(p1);
                            PUTPIXEL(p2, pixel);
                        
                            p2 += dst_pix_bytes;
                            if (*pX++ == 1)
                                p1 += src_pix_bytes;
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
                                p1 += src_pix_bytes;
                            }

                            pixel = GETPIXEL(p1);
                            PUTPIXEL(p2, pixel);
                            
                            p2 += dst_pix_bytes;
                            p1 += src_pix_bytes;
                        }
                        
                        pDst += dst_line_bytes;
                        pSrc += src_line_bytes;
                    }
                }

            }
        }
        else //if (bk_mode != BM_NEWTRANSPARENT)
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
                        while(width--)
                        {
                            pixel = GETPIXEL(p1);
                            if (pixel != (uint32)pGraphParam->bk_color)
                                PUTPIXEL(p2, pixel);
                            
                            p2 += dst_pix_bytes;
                            if (*pX++ == 1)
                                p1 += src_pix_bytes;
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
                                p1 += src_pix_bytes;
                            }

                            pixel = GETPIXEL(p1);
                            if (pixel != (uint32)pGraphParam->bk_color)
                                PUTPIXEL(p2, pixel);

                            p2 += dst_pix_bytes;
                            p1 += src_pix_bytes;
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
                        while(width--)
                        {
                            pixel = GETPIXEL(p1);
                            if (pixel != (uint32)pGraphParam->bk_color)
                                PUTPIXEL(p2, pixel);
                        
                            p2 += dst_pix_bytes;
                            if (*pX++ == 1)
                                p1 += src_pix_bytes;
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
                                p1 += src_pix_bytes;
                            }

                            pixel = GETPIXEL(p1);
                            if (pixel != (uint32)pGraphParam->bk_color)
                                PUTPIXEL(p2, pixel);
                            
                            p2 += dst_pix_bytes;
                            p1 += src_pix_bytes;
                        }
                        
                        pDst += dst_line_bytes;
                        pSrc += src_line_bytes;
                    }
                }

            }
        }

        break;

    default:

        if (bk_mode != BM_NEWTRANSPARENT)
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
                        while(width--)
                        {
                            pixel = ropfunc(GETPIXEL(p1), GETPIXEL(p2));
                            PUTPIXEL(p2, pixel);
                            
                            p2 += dst_pix_bytes;
                            if (*pX++ == 1)
                                p1 += src_pix_bytes;
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
                                p1 += src_pix_bytes;
                            }

                            pixel = ropfunc(GETPIXEL(p1), GETPIXEL(p2));
                            PUTPIXEL(p2, pixel);

                            p2 += dst_pix_bytes;
                            p1 += src_pix_bytes;
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
                        while(width--)
                        {
                            pixel = ropfunc(GETPIXEL(p1), GETPIXEL(p2));
                            PUTPIXEL(p2, pixel);
                        
                            p2 += dst_pix_bytes;
                            if (*pX++ == 1)
                                p1 += src_pix_bytes;
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
                                p1 += src_pix_bytes;
                            }

                            pixel = ropfunc(GETPIXEL(p1), GETPIXEL(p2));
                            PUTPIXEL(p2, pixel);
                            
                            p2 += dst_pix_bytes;
                            p1 += src_pix_bytes;
                        }
                        
                        pDst += dst_line_bytes;
                        pSrc += src_line_bytes;
                    }
                }

            }
        }
        else //if (bk_mode != BM_NEWTRANSPARENT)
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
                        while(width--)
                        {
                            pixel = GETPIXEL(p1);
                            if (pixel != (uint32)pGraphParam->bk_color)
                            {
                                pixel = ropfunc(pixel, GETPIXEL(p2));
                                PUTPIXEL(p2, pixel);
                            }
                            
                            p2 += dst_pix_bytes;
                            if (*pX++ == 1)
                                p1 += src_pix_bytes;
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
                                p1 += src_pix_bytes;
                            }

                            pixel = GETPIXEL(p1);
                            if (pixel != (uint32)pGraphParam->bk_color)
                            {
                                pixel = ropfunc(pixel, GETPIXEL(p2));
                                PUTPIXEL(p2, pixel);
                            }

                            p2 += dst_pix_bytes;
                            p1 += src_pix_bytes;
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
                        while(width--)
                        {
                            pixel = GETPIXEL(p1);
                            if (pixel != (uint32)pGraphParam->bk_color)
                            {
                                pixel = ropfunc(pixel, GETPIXEL(p2));
                                PUTPIXEL(p2, pixel);
                            }
                        
                            p2 += dst_pix_bytes;
                            if (*pX++ == 1)
                                p1 += src_pix_bytes;
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
                                p1 += src_pix_bytes;
                            }

                            pixel = GETPIXEL(p1);
                            if (pixel != (uint32)pGraphParam->bk_color)
                            {
                                pixel = ropfunc(pixel, GETPIXEL(p2));
                                PUTPIXEL(p2, pixel);
                            }
                            
                            p2 += dst_pix_bytes;
                            p1 += src_pix_bytes;
                        }
                        
                        pDst += dst_line_bytes;
                        pSrc += src_line_bytes;
                    }
                }

            }
        }

        break;
    }

RET :
    
    if (pSrcDev == pDstDev && IntersectRect(&src_rect, &rc))
    {
        free(pMemDev);
    }

    if (StretchPattern.bWidthPattern != bWidthPattern)
        free(StretchPattern.bWidthPattern);
    if (StretchPattern.bHeightPattern != bHeightPattern)
        free(StretchPattern.bHeightPattern);

    _UnexcludeCursor(pDstDev);

    return 1;

err_RET:
    if (StretchPattern.bWidthPattern != bWidthPattern)
        free(StretchPattern.bWidthPattern);
    if (StretchPattern.bHeightPattern != bHeightPattern)
        free(StretchPattern.bHeightPattern);

    return 1;
}
