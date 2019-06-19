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
* Function	BitBlt   
* Purpose      
**      ������ʾ�豸֮�䡢�ڴ��豸֮�䡢
**      ������ʾ�豸���ڴ��豸֮����п鴫�ͣ�
* Params	   
**      pDstDev     ָ��Ŀ���豸��ָ�룻
**      pDestRect   ָ��Ŀ�ľ��ε�ָ�룻
**      pSrcDev     ָ��Դ�豸��ָ�룻
**      x , y       Դ�豸������ʼ��(���Ͻ�)x, y����;
**      pGraphParam ָ���ͼ�����ṹ��ָ��.
* Return	 	   
* Remarks	   
**********************************************************************/
static int BitBlt(PDEV pDstDev, PRECT pDestRect, PDEV pSrcDev,
                  int x, int y, PGRAPHPARAM pGraphParam)
{
    RECT        rect, src_rect;
    uint32      rop;
    int         width, height;

    ASSERT(pGraphParam != NULL && pDestRect != NULL && 
        pDstDev != NULL);

    CopyRect(&rect, pDestRect);
    ClipByDev(&rect, pDstDev);
    if (!IsValidRect(&rect))
        return -1;

    if ((pGraphParam->flags & GPF_CLIP_RECT) && 
        !ClipRect(&rect, &pGraphParam->clip_rect))
        return -1;

    rop = SRop3ToRop2(pGraphParam->rop);
    if (pSrcDev == NULL)
    {
        /* ���Դ�豸Ϊ��, ����Ƿ�rop����Ч����Դ����*/
        if (rop != ROP_SET_DST_0 && 
            rop != ROP_SET_DST_1 && 
            rop != ROP_DST && rop != ROP_NDST)
        return -1;

        pSrcDev = pDstDev;
    }
    else
    {
        src_rect.x1 = x;
        src_rect.y1 = y;
        src_rect.x2 = x + (pDestRect->x2 - pDestRect->x1);
        src_rect.y2 = y + (pDestRect->y2 - pDestRect->y1);

        ClipByDev(&src_rect, pSrcDev);

        // ����Ŀ�ľ��Ρ�Դ���� 
        rect.x1 -= pDestRect->x1;
        rect.y1 -= pDestRect->y1;
        rect.x2 -= pDestRect->x1;
        rect.y2 -= pDestRect->y1;
        
        src_rect.x1 -= x;
        src_rect.y1 -= y;
        src_rect.x2 -= x;
        src_rect.y2 -= y;
        
        //���Ŀ�ĺ�Դ���εĽ���
        if (!ClipRect(&rect, &src_rect))
            return -1;
        
        x += rect.x1;
        y += rect.y1;
        
        rect.x1 += pDestRect->x1;
        rect.y1 += pDestRect->y1;
        rect.x2 += pDestRect->x1;
        rect.y2 += pDestRect->y1;
    }

    width = rect.x2 - rect.x1;
    height = rect.y2 - rect.y1;

    _ExcludeCursor(pDstDev, &rect);
    
    MemBitBlt(pDstDev, &rect, pSrcDev, x, y, pGraphParam);

    _UnexcludeCursor(pDstDev);

    return 0;
}

/*********************************************************************\
* Function	   MemBitBlt
* Purpose      Do bitblt of memory device. When the GE isn't support 
               bitblt, it's also called.
* Params	   
* Return	 	   
* Remarks	   The clip operation has been completed in the Bitblt!
**********************************************************************/
static int MemBitBlt(PDEV pDstDev, PRECT pDestRect, PDEV pSrcDev,
                  int x, int y, PGRAPHPARAM pGraphParam)
{
    uint32      rop;
    ROPFUNC     ropfunc;
    int         width, height, i;
    uint8       *pSrc, *pDst;
    uint8       *p1, *p2;
    uint32      pixel;
    int         dst_line_bytes, src_line_bytes, pix_bytes;
    int16       bk_mode;

    ASSERT(pGraphParam != NULL && pDestRect != NULL && 
        pSrcDev != NULL && pDstDev != NULL);

    rop = SRop3ToRop2(pGraphParam->rop);
    pDst = GETXYADDR(pDstDev, pDestRect->x1, pDestRect->y1);
    pSrc = GETXYADDR(pSrcDev, x, y);

    width = pDestRect->x2 - pDestRect->x1;
    height = pDestRect->y2 - pDestRect->y1;

    pix_bytes = pDstDev->pix_bits / 8;

    bk_mode = pGraphParam->bk_mode;

    if (bk_mode == BM_ALPHATRANSPARENT || bk_mode == BM_NEWALPHATRANSPARENT)
    {
        if (pGraphParam->pBlend == NULL || 
            pGraphParam->pBlend->BlendFlags != 0)
            return 0;

        /* ������AC_SRC_ALPHA������ֵ������ֻ�ܴ������SCA�����
        ** ����Դ������32λ�ĺ�alphaͨ����λͼ 
        */
        if (pGraphParam->pBlend->AlphaFormat != 0)
            return 0;

        /* ֻ֧��Դλͼ����Ŀ��λͼ */
        if (pGraphParam->pBlend->BlendOp != AC_SRC_OVER)
            return 0;

        /* ��ȫ͸����ֱ�ӷ��� */
        if (pGraphParam->pBlend->SourceConstantAlpha == 0)
            return 0;

        if (pGraphParam->pBlend->SourceConstantAlpha == 0xFF)
        {
            if (bk_mode == BM_ALPHATRANSPARENT)
            {
                /* ��ȫ��͸����ֱ����ͼ */
                bk_mode = BM_OPAQUE;
            }
            else //BM_NEWALPHATRANSPARENT
            {
                bk_mode = BM_NEWTRANSPARENT;
            }
        }
    }

#if (USELINECOPY)
    if (bk_mode != BM_NEWALPHATRANSPARENT && 
        bk_mode != BM_ALPHATRANSPARENT && 
        bk_mode != BM_NEWTRANSPARENT)
    {
        COPYLINESTRUCT p;

        if (pDst >= pSrc && pDst < pSrc + pSrcDev->line_bytes * height)
        {
            src_line_bytes = -pSrcDev->line_bytes;
            dst_line_bytes = -pDstDev->line_bytes;
            
            pDst += pDstDev->line_bytes * (height - 1);
            pSrc += pSrcDev->line_bytes * (height - 1);
            
            GetCopyLineType(pDst, pSrc, width * pix_bytes, &p, COPYLINE_R2L, rop);
        }
        else
        {
            src_line_bytes = pSrcDev->line_bytes;
            dst_line_bytes = pDstDev->line_bytes;
            
            GetCopyLineType(pDst, pSrc, width * pix_bytes, &p, COPYLINE_L2R, rop);
        }

        while (height--)
        {
            p.copylinefunc(&p);
            p.dst += dst_line_bytes;
            p.src += src_line_bytes;
        }

        return 1;
    }
#endif
    
    /*Դ��Ŀ�����غϣ�Ӧ�÷��򿽱�
    */
    if (pDst >= pSrc && pDst < pSrc + pSrcDev->line_bytes * height)
    {
        pDst += pDstDev->line_bytes * (height - 1) + 
            (width - 1) * pix_bytes;
        pSrc += pSrcDev->line_bytes * (height - 1) + 
            (width - 1) * pix_bytes;

        pix_bytes = -pix_bytes;

        src_line_bytes = -pSrcDev->line_bytes;
        dst_line_bytes = -pDstDev->line_bytes;
    }
    else
    {
        src_line_bytes = pSrcDev->line_bytes;
        dst_line_bytes = pDstDev->line_bytes;
    }

    if (bk_mode == BM_ALPHATRANSPARENT || bk_mode == BM_NEWALPHATRANSPARENT)
    {
        /* Support the new alpha transparent */
        uint32   uSCA;
        uint32   dred, dgreen, dblue;
        uint32   sred, sgreen, sblue;

        /* BlendOp == AC_SRC_OVER && BlendFlags == 0 && AlphaFormat == 0 */
        uSCA = pGraphParam->pBlend->SourceConstantAlpha;
        if (bk_mode == BM_ALPHATRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
            
                for (i = 0; i < width; i++)
                {
                    GETRGBBYTE(p1, sred, sgreen, sblue);
                    GETRGBBYTE(p2, dred, dgreen, dblue);

                    /* dred = (sred * uSCA + dred * (255 - uSCA)) / 255*/
                    INT8_255DIVIDED(sred * uSCA + (255 - uSCA) * dred, dred);
                    INT8_255DIVIDED(sgreen * uSCA + (255 - uSCA) * dgreen, dgreen);
                    INT8_255DIVIDED(sblue * uSCA + (255 - uSCA) * dblue, dblue);

                    pixel = REALIZECOLOR(dred, dgreen, dblue);
                    PUTPIXEL(p2, pixel);
                
                    p1 += pix_bytes;
                    p2 += pix_bytes;
                }
            
                pSrc += src_line_bytes;
                pDst += dst_line_bytes;
            }
        }
        else //bk_mode == BM_NEWALPHATRANSPARENT
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
            
                for (i = 0; i < width; i++)
                {
                    pixel = GETPIXEL(p1);
                    if (pixel != (uint32)pGraphParam->bk_color)
                    {
                        GETRGBBYTE(p1, sred, sgreen, sblue);
                        GETRGBBYTE(p2, dred, dgreen, dblue);

                        /* dred = (sred * uSCA + dred * (255 - uSCA)) / 255*/
                        INT8_255DIVIDED(sred * uSCA + (255 - uSCA) * dred, dred);
                        INT8_255DIVIDED(sgreen * uSCA + (255 - uSCA) * dgreen, dgreen);
                        INT8_255DIVIDED(sblue * uSCA + (255 - uSCA) * dblue, dblue);

                        pixel = REALIZECOLOR(dred, dgreen, dblue);
                        PUTPIXEL(p2, pixel);
                    }

                    p1 += pix_bytes;
                    p2 += pix_bytes;
                }
            
                pSrc += src_line_bytes;
                pDst += dst_line_bytes;
            }
        }

        return 1;
    }

#if (USELINECOPY)
    if (bk_mode != BM_NEWTRANSPARENT)
        return 0;
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
                    pixel = GETPIXEL(p1);
                    PUTPIXEL(p2, pixel);
                    
                    p1 += pix_bytes;
                    p2 += pix_bytes;
                }
                
                pSrc += src_line_bytes;
                pDst += dst_line_bytes;
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
                    pixel = GETPIXEL(p1);
                    if (pixel != (uint32)pGraphParam->bk_color)
                        PUTPIXEL(p2, pixel);
                    
                    p1 += pix_bytes;
                    p2 += pix_bytes;
                }
                
                pSrc += src_line_bytes;
                pDst += dst_line_bytes;
            }
        }
        
        break;

    case ROP_NSRC :

#if (!USELINECOPY)
        if (pGraphParam->bk_mode != BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                for (i = 0; i < width; i++)
                {
                    pixel = GETPIXEL(p1);
                    PUTPIXEL(p2, ~pixel);
                    
                    p1 += pix_bytes;
                    p2 += pix_bytes;
                }
                
                pSrc += src_line_bytes;
                pDst += dst_line_bytes;
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
                    pixel = GETPIXEL(p1);
                    if (pixel != (uint32)pGraphParam->bk_color)
                        PUTPIXEL(p2, ~pixel);
                    
                    p1 += pix_bytes;
                    p2 += pix_bytes;
                }
                
                pSrc += src_line_bytes;
                pDst += dst_line_bytes;
            }
        }
        
        break;

    case ROP_SRC_AND_DST :
#if (!USELINECOPY)
        if (pGraphParam->bk_mode != BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                for (i = 0; i < width; i++)
                {
                    pixel = GETPIXEL(p1) & GETPIXEL(p2);
                    PUTPIXEL(p2, pixel);
                    
                    p1 += pix_bytes;
                    p2 += pix_bytes;
                }
                
                pSrc += src_line_bytes;
                pDst += dst_line_bytes;
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
                    pixel = GETPIXEL(p1);
                    if (pixel != (uint32)pGraphParam->bk_color)
                    {
                        pixel = pixel & GETPIXEL(p2);
                        PUTPIXEL(p2, pixel);
                    }
                    
                    p1 += pix_bytes;
                    p2 += pix_bytes;
                }
                
                pSrc += src_line_bytes;
                pDst += dst_line_bytes;
            }
        }
        
        break;
        
    case ROP_SRC_XOR_DST :
#if (!USELINECOPY)
        if (pGraphParam->bk_mode != BM_NEWTRANSPARENT)
        {
            while (height--)
            {
                p1 = pSrc;
                p2 = pDst;
                
                for (i = 0; i < width; i++)
                {
                    pixel = GETPIXEL(p1) ^ GETPIXEL(p2);
                    PUTPIXEL(p2, pixel);
                    
                    p1 += pix_bytes;
                    p2 += pix_bytes;
                }
                
                pSrc += src_line_bytes;
                pDst += dst_line_bytes;
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
                    pixel = GETPIXEL(p1);
                    if (pixel != (uint32)pGraphParam->bk_color)
                    {
                        pixel = pixel ^ GETPIXEL(p2);
                        PUTPIXEL(p2, pixel);
                    }
                    
                    p1 += pix_bytes;
                    p2 += pix_bytes;
                }
                
                pSrc += src_line_bytes;
                pDst += dst_line_bytes;
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
                    pixel = ropfunc(GETPIXEL(p1), GETPIXEL(p2));
                    PUTPIXEL(p2, pixel);
                    
                    p1 += pix_bytes;
                    p2 += pix_bytes;
                }
                
                pSrc += src_line_bytes;
                pDst += dst_line_bytes;
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
                    pixel = GETPIXEL(p1);
                    if (pixel != (uint32)pGraphParam->bk_color)
                    {
                        pixel = ropfunc(pixel, GETPIXEL(p2));
                        PUTPIXEL(p2, pixel);
                    }
                    
                    p1 += pix_bytes;
                    p2 += pix_bytes;
                }
                
                pSrc += src_line_bytes;
                pDst += dst_line_bytes;
            }
        }
        
        break;
    }

    return 0;
}
