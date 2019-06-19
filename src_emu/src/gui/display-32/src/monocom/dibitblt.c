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



static uint8 GetMonoColor1(RGBQUAD* pPalette, int i);
static void BitBlt16(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
              int x, int y, int s_scanw, int32 rop, int bkmode);
static void BitBlt256(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
               int x, int y, int s_scanw, int32 rop, int bkmode);
static void BitBltRGB(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
               int x, int y, int src_scanw, int32 rop, int bkmode);
static void BitBlt16RGB(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
                 int x, int y, int src_scanw, int32 rop, int bkmode);

static uint8 GetMonoColor1(RGBQUAD* pPalette, int i)
{   
    uint8 r, g, b;
    uint32 color;

    b = pPalette[i].B; 
    g = pPalette[i].G;
    r = pPalette[i].R;  

    color = r * 30 + g * 59 + b * 11; 

    if (color <= 12800)
        return 0x0;    //black
    else 
        return 0x01;    //white
}

static uint8 GetMonoColor2(uint8* rgb, int bmpmode)
{
    uint8 r = 0, g = 0, b = 0;
    int color;

    if (bmpmode == BMP_RGB24)
    {
        b = *rgb; 
        g = *(rgb + 1);
        r = *(rgb + 2);
    }
    else if (bmpmode == BMP_RGB16)
    {
        color = *(uint16*)rgb;
#ifdef _EMULATE_
    r = (uint8)((color & 0x7c00) >> 7);
    g = (uint8)((color & 0x03e0) >> 2);
    b = (uint8)((color & 0x001f) << 3);
#else
    r = (uint8)((color & 0xf800) >> 8);
    g = (uint8)((color & 0x07e0) >> 3);
    b = (uint8)((color & 0x001f) << 3);
#endif
    }

    color = r * 30 + g * 59 + b * 11; 

    if (color <= 12800)
        return 0x0;    //black
    else 
        return 0x01;    //white
}


static int DIBitBlt(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                    int x, int y, PGRAPHPARAM pGraphParam)
{
    int        src_scanw, ret = 0;
    RECT       rc, src_rect;
    MEMDEV     mem_dev;
    RGBQUAD    *quad;
    uint32     fg_color, bk_color;
    uint32     old_fgcolor, old_bkcolor;
    uint32     rop;

    if (pBitmap == NULL || pDev == NULL)
        return -1;

    rop = SRop3ToRop2(pGraphParam->rop);

    CopyRect(&rc, pDestRect);

    src_rect.x1 = x;
    src_rect.y1 = y;
    src_rect.x2 = x + pBitmap->width;

    if (pBitmap->height > 0)
        src_rect.y2 = y + pBitmap->height;
    else
        src_rect.y2 = y - pBitmap->height;

    //据目的设备、剪裁矩形对目的矩形裁剪
    ClipByDev(&rc, pDev);
    if (!IsValidRect(&rc))
        return -1;

    if ((pGraphParam != NULL) && (pGraphParam->flags & GPF_CLIP_RECT))
        if (!ClipRect(&rc, &pGraphParam->clip_rect))
            return -1;

    _ExcludeCursor(pDev, &rc); 
    
        //调整目的矩形、源矩形 
    rc.x1 -= pDestRect->x1;
    rc.y1 -= pDestRect->y1;
    rc.x2 -= pDestRect->x1;
    rc.y2 -= pDestRect->y1;

    src_rect.x1 -= x;
    src_rect.y1 -= y;
    src_rect.x2 -= x;
    src_rect.y2 -= y;

    //获得目的和源矩形的交集
    if (!ClipRect(&rc, &src_rect))
        return -1;
    
    src_rect.x1 = rc.x1;
    src_rect.y1 = rc.y1;

    src_rect.x1 += x;
    src_rect.y1 += y;

    rc.x1 += pDestRect->x1;
    rc.y1 += pDestRect->y1;
    rc.y2 += pDestRect->y1;
    rc.x2 += pDestRect->x1;
    
    switch( pBitmap->type )
    {
    case BMP_MONO:

        mem_dev.mode = DEV_MEMORY;
        mem_dev.bmpmode = pDev->bmpmode;
        mem_dev.width = pBitmap->width;
        mem_dev.height = pBitmap->height;
        mem_dev.line_bytes = (mem_dev.width + 31)/ 32 * 4,
        mem_dev.pix_bits = 1;
        mem_dev.pdata = (uint8*)pBitmap->pBuffer;

        quad = (RGBQUAD *)(pBitmap->pPalette);
        bk_color = quad->R + (quad->G << 8) + (quad->B << 16);
        bk_color = RealizeColor(pDev, NULL, bk_color);

        quad++;
        fg_color = quad->R + (quad->G << 8) + (quad->B << 16);
        fg_color = RealizeColor(pDev, NULL, fg_color);

        //Save the color of GraphParam
        old_fgcolor = pGraphParam->text_color;
        old_bkcolor = pGraphParam->bk_color;

        pGraphParam->text_color = fg_color;
        pGraphParam->bk_color = bk_color;

        MonoBitBlt(pDev, &rc, (PDEV)&mem_dev, src_rect.x1, src_rect.y1, 
            rop, pGraphParam->bk_mode, mem_dev.height);
        
        //restore the graphParam's color
        pGraphParam->text_color = old_fgcolor;
        pGraphParam->bk_color = old_bkcolor;        
        break;

    case BMP_GRAY4:
        //待做
        break;

    case BMP_16:

        src_scanw = (pBitmap->width * 4 + 31) / 32 * 4;
        BitBlt16(pDev, &rc, pBitmap, src_rect.x1, src_rect.y1,
            src_scanw, rop, pGraphParam->bk_mode);
        break;
        
    case BMP_256:

        src_scanw = (pBitmap->width * 8 + 31) / 32 * 4;     
        BitBlt256(pDev, &rc, pBitmap, src_rect.x1, src_rect.y1, 
            src_scanw, rop, pGraphParam->bk_mode);
        break;

    case BMP_RGB24:

        src_scanw = (pBitmap->width * 24 + 31) / 32 * 4;
        BitBltRGB(pDev, &rc, pBitmap, src_rect.x1, src_rect.y1,
            src_scanw, rop, pGraphParam->bk_mode);
        break;
    }

    _UnexcludeCursor(pDev);

    return ret;
}

static void BitBltRGB(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap, 
                      int x, int y, int src_scanw, int32 rop, int bkmode)
{   
    int     width, height;
    int     m, i, count;
    uint8   *pSrc, *pDst;
    uint8   *p1, *p2;
    uint8   leftmask, rightmask;
    ROPFUNC ropfunc;
    static uint8   Buf[640];

    /* Set ROP function pointer */
    ropfunc = GetRopFunc(rop);

    width = pDestRect->x2 - pDestRect->x1;
    height = pDestRect->y2 - pDestRect->y1;
    
    pSrc = (uint8*)pBitmap->pBuffer;
    pSrc += y * src_scanw + x * 3;
    m = -pBitmap->height - y;
    if (pBitmap->height > 0)
    {
        pSrc += (pBitmap->height - y - 1) * src_scanw + x * 3;
        m = pBitmap->height - y;
        src_scanw = -src_scanw;
    }

    pDst = pDev->pdata + pDestRect->y1 * pDev->line_bytes + 
        pDestRect->x1 / 8;

    for (count = 0; count < m; count++)
    {
        int   size, pixbit, shift;
        uint8 pixcolor, src;
        uint8 *pBuf, *pBlockBuf; 

        //分配一行大小的buffer
        pBuf = Buf;
        //size = ((width + 31) >> 5) << 2; 
        size = (width + 7) >> 3; 
        memset(pBuf, 0, size);      
        pixbit = 0;

        //第一步: 将位图转换成单色的位图        
        p1 = pSrc;
        for (i = 0; i < width; i++)
        {        
            pixcolor = GetMonoColor2(p1, BMP_RGB24);
            *pBuf = *pBuf | (pixcolor << (7 - pixbit));
            p1 += 3;

            pixbit ++;       //next pixel                    
            if (pixbit == 8)
            { 
                pBuf++;      //move pBuf8 to the next byte
                pixbit = 0;  //reset pixbit
            }
        }

        //第二步：将转换后的位图写入目的设备;               
        pBlockBuf = Buf;
        p2 = pDst;

        //shift = pDestRect->x1 & 7;
        shift = x & 7;
        leftmask = 0xff >> shift;
        rightmask = 0xff << (7 - ((x + width - 1) & 7));

        //first byte
        src = (0xff << (8 - shift) ) | ((*pBlockBuf) >> shift);
        if (bkmode == BM_OPAQUE)
            *p2 = (*p2) & ~leftmask | 
            (uint8)ropfunc(src, (*p2)) & leftmask;

        pBlockBuf ++;
        p2 ++;

        if (size != 1)
        {
            if (bkmode == BM_OPAQUE)
            {
                //middle bytes
                for (i = 0; i < size - 2; i++ )
                {
                    src = ( (*(pBlockBuf - 1)) << (8 - shift) ) |
                        ( (*pBlockBuf) >> shift );
                    *p2 = (uint8)ropfunc(src, (*p2));

                    pBlockBuf ++;
                    p2 ++;
                }

                //last byte
                src = ( (*(pBlockBuf - 1)) << (8 - shift) ) |
                    ( (*pBlockBuf) >> shift );
                *p2 = (*p2) & ~rightmask | 
                    (uint8)ropfunc(src, (*p2)) & rightmask;
            }
        }

        //第三步:下一行
        pSrc += src_scanw;
        pDst += pDev->line_bytes;
    }
}

static void BitBlt256(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                      int x, int y, int s_scanw, int32 rop, int bkmode)
{
    int     w, h, i, shift;
    int     m, count;
    uint8   src;
    uint8   *p1, *p2;
    uint8   *pBlockBuf, *pBuf;
    uint8   *pSrc, *pDst;
    uint8   leftmask, rightmask;
    ROPFUNC ropfunc;
    RGBQUAD* pPalette;
    static uint8   Buf[640];

    if ((pDev == NULL) || (pBitmap == NULL))
        return;

    /* Set ROP function pointer */
    ropfunc = GetRopFunc(rop);
    pPalette = pBitmap->pPalette;

    w = pDestRect->x2 - pDestRect->x1;
    h = pDestRect->y2 - pDestRect->y1;

    pSrc = (uint8 *)pBitmap->pBuffer + y * s_scanw + x; 
    m = -pBitmap->height - y;
    if ( pBitmap->height > 0)
    {
        pSrc = (uint8 *)pBitmap->pBuffer 
            + (pBitmap->height - y - 1) * s_scanw + x;
        m = pBitmap->height - y;
        s_scanw = -s_scanw;
    }

    pDst = pDev->pdata + pDestRect->y1 * pDev->line_bytes + 
        pDestRect->x1 / 8;

    for(count = 0; count < m; count++)
    {
        int size, pixbit;
        uint8 pixcolor;

//分配一行大小的buffer
        pBuf = Buf;
        size = ((w + 31) >> 5) << 2; 
        memset(pBuf, 0, size);      
        pixbit = 0;

        p1 = pSrc;
//第一步: 将位图转换成单色的位图
        for (i =0; i < w; i++ )
        {
            pixcolor = *p1;
            pixcolor = GetMonoColor1(pPalette, pixcolor);
            *pBuf = *pBuf | (pixcolor << (7 - pixbit));
            pixbit ++;       //next pixel

            p1++;

            if (pixbit == 8)
            {
                pBuf++;      //move pBuf8 to the next byte
                pixbit = 0;  //reset pixbit
            }
        }

//第二步：将转换后的位图写入目的设备;   
        pBlockBuf = Buf;
        p2 = pDst;

        shift = x & 7;
        leftmask = 0xff >> shift;
        rightmask = 0xff << (7 - ((x + w - 1) & 7));

        //first byte
        src = (0xff << (8 - shift) ) | ((*pBlockBuf) >> shift);
        if (bkmode == BM_OPAQUE)
            *p2 = (*p2) & ~leftmask | 
                (uint8)ropfunc(src, (*p2)) & leftmask;

        pBlockBuf ++;
        p2 ++;

        if (size != 1)
        {
            if (bkmode == BM_OPAQUE)
            {
                //middle bytes
                for (i = 0; i < size - 2; i++ )
                {
                    src = ( (*(pBlockBuf - 1)) << (8 - shift) ) |
                        ( (*pBlockBuf) >> shift );
                    *p2 = (uint8)ropfunc(src, (*p2));

                    pBlockBuf ++;
                    p2 ++;
                }

                //last byte
                src = ( (*(pBlockBuf - 1)) << (8 - shift) ) |
                    ( (*pBlockBuf) >> shift );
                *p2 = (*p2) & ~rightmask | 
                    (uint8)ropfunc(src, (*p2)) & rightmask;
            }
        }

//第三步:下一行
        pSrc += s_scanw;
        pDst += pDev->line_bytes;
    }
}

static void BitBlt16(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                     int x, int y, int s_scanw, int32 rop, int bkmode)
{
    int     w, h, i, shift;
    int     len, m, count;
    uint8   src;
    uint8   *p1, *p2;
    static uint8   Buf[640];
    uint8   *pBlockBuf, *pBuf;
    uint8   *pSrc, *pDst;
    uint8   leftmask, rightmask;
    ROPFUNC  ropfunc;
    RGBQUAD* pPalette;

    if (pDev == NULL || pBitmap == NULL)
        return ;

    /* Set ROP function pointer */
    ropfunc = GetRopFunc(rop);
    pPalette = pBitmap->pPalette;

    w = pDestRect->x2 - pDestRect->x1;
    h = pDestRect->y2 - pDestRect->y1;

    pSrc = (uint8 *)pBitmap->pBuffer + y * s_scanw + x * 4 / 8; 
    m = -pBitmap->height - y;
    if ( pBitmap->height > 0)
    {
        pSrc = (uint8 *)pBitmap->pBuffer 
            + (pBitmap->height - y - 1) * s_scanw + x * 4 / 8;
        m = pBitmap->height - y;
        s_scanw = -s_scanw;
    }

    pDst = pDev->pdata + pDestRect->y1 * pDev->line_bytes + 
        pDestRect->x1 / 8;
    len = (w * 4 + 31) / 32 * 4;

    for(count = 0; count < m; count++)
    {
        int size, pixbit;
        uint8 pixcolor;

//分配一行大小的buffer
        pBuf = Buf;
        size = (w + 31) / 32 * 4; 
        memset(pBuf, 0, size);      
        pixbit = 0;

//第一步: 将位图转换成单色的位图
        p1 = pSrc;

        //first byte
        if((x * 4 % 8) != 0)
        {
            pixcolor = (uint8)((*p1) & 0x0f);
            pixcolor = GetMonoColor1(pPalette, pixcolor);
            *pBuf = *pBuf | pixcolor << (7 - pixbit);
            pixbit ++;
        }
        else
        {
            //process high 4 bits in source
            pixcolor = ((*p1) & 0xf0) >> 4;
            pixcolor = GetMonoColor1(pPalette, pixcolor);
            *pBuf = *pBuf | pixcolor << (7 - pixbit);
            pixbit ++;  //next pixel

            //process low 4 bits in source
            pixcolor = (*p1) & 0x0f;
            pixcolor = GetMonoColor1(pPalette, pixcolor);
            *pBuf = *pBuf | pixcolor << (7 - pixbit);
            pixbit ++;  //next pixel
        }
        p1++;

        if (len != 1)
        {
            //middle bytes
            for ( i = 0; i < len - 2; i ++ )
            {
                //process high 4 bits in source
                pixcolor = ((*p1) & 0xf0) >> 4;
                pixcolor = GetMonoColor1(pPalette, pixcolor);
                *pBuf = *pBuf | pixcolor << (7 - pixbit);
                pixbit ++;       //next pixel
                
                //process low 4 bits in source
                pixcolor = (*p1) & 0x0f;
                pixcolor = GetMonoColor1(pPalette, pixcolor);
                *pBuf = *pBuf | pixcolor << (7 - pixbit);
                pixbit ++;       //next pixel                    

                p1++;

                if (pixbit == 8)
                {
                    pBuf++;      //move pBuf8 to the next byte
                    pixbit = 0;  //reset pixbit
                }
            }

            //last byte
            if((x + w) * 4 % 8 != 0)
            {
                pixcolor = ((*p1) & 0xf0) >> 4;
                pixcolor = GetMonoColor1(pPalette, pixcolor);
                *pBuf = *pBuf | pixcolor << (7 - pixbit);
                pixbit ++;       //next pixel                    
            }
            else
            {
                //process high 4 bits in source
                pixcolor = ((*p1) & 0xf0) >> 4;
                pixcolor = GetMonoColor1(pPalette, pixcolor);
                *pBuf = *pBuf | pixcolor << (7 - pixbit);
                pixbit ++;       //next pixel

                //process low 4 bits in source
                pixcolor = (*p1) & 0x0f;
                pixcolor = GetMonoColor1(pPalette, pixcolor);
                *pBuf = *pBuf | pixcolor << (7 - pixbit);
                pixbit ++;       //next pixel                    
            }
        }

//第二步：将转换后的位图写入目的设备;   
        //Set pDstDev with pBuf
        pBlockBuf = Buf;
        p2 = pDst;

        shift = x & 7;
        leftmask = 0xff >> shift;
        rightmask = 0xff << (7 - ((x + w - 1) & 7));

        //first byte
        src = (0x00 << (8 - shift) ) | ((*pBlockBuf) >> shift);
        if (bkmode == BM_OPAQUE)
            *p2 = (*p2) & ~leftmask | 
                (uint8)ropfunc(src, (*p2)) & leftmask;
        pBlockBuf ++;
        p2 ++;

        if (size!= 1)
        {
            if (bkmode == BM_OPAQUE)
            {
                //middle bytes
                for (i = 0; i < size - 2; i++ )
                {
                    src = ( (*(pBlockBuf - 1)) << (8 - shift) ) |
                        ( (*pBlockBuf) >> shift );
                    *p2 = (uint8)ropfunc(src, (*p2));

                    pBlockBuf ++;
                    p2 ++;
                }

                //last byte
                src = ( (*(pBlockBuf - 1)) << (8 - shift) ) |
                    ( (*pBlockBuf) >> shift );
                *p2 = (*p2) & ~rightmask | 
                    (uint8)ropfunc(src, (*p2)) & rightmask;
            }
        }

//第三步:下一行
        pSrc += s_scanw;
        pDst += pDev->line_bytes;
    }
    return;
}
