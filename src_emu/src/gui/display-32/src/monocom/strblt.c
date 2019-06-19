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


/**************************************************************************\
 *
 *  Function: StretchBlt
 *  Purpose:  
 *  INPUT:
 *      pDev        Pointer to display device.
 *      pDestRect   Destination rectangle.
 *      pBitmap     Pointer to device independent bitmap.
 *      pSrcRect    Source rectangle.
 *      pGraphParam pointer to graphics operation parameter group.
 *
\**************************************************************************/
static char LineBuffer[2048]; // 2 * 1024 bytes (2k)

static int StretchBlt(PDEV pDev, PRECT pDestRect, PDEV pSrcDev, 
                        PRECT pSrcRect, PGRAPHPARAM pGraphParam)
{
    unsigned char *pBitsSrc, *pBitsDst;
    unsigned char *pSrc, *pDst; 
    int src_width, src_height, dest_width, dest_height; 
    int buflines, ty, tx, count;
    int step_x, step_y;
    int src_y, dst_y, src_ry, dst_ry;
    int src_x, dst_x, src_rx, dst_rx; 
    RECT rc, src_rect, temp_rc;
    RECT rcDevClip, rcSrcClip, rcSrcDev, rcDestDev;
    uint32 rop;
    MEMDEV  Bitmap;
    
    if (pDev == NULL)  
        return -1;

    rop = SRop3ToRop2(pGraphParam->rop);

    if (pSrcDev == NULL)
    {
        if (rop != ROP_SET_DST_0 && rop != ROP_SET_DST_1 && 
            rop != ROP_DST && rop != ROP_NDST)
            return -1;
        else
            return BitBlt(pDev, pDestRect, pSrcDev, 0, 0, pGraphParam);
    }

    if (!IsValidRect(pDestRect))
        return -1;
    if (!IsValidRect(pSrcRect))
        return -1;

    src_width = pSrcRect->x2 - pSrcRect->x1;
    src_height = pSrcRect->y2 - pSrcRect->y1;

    dest_width = pDestRect->x2 - pDestRect->x1; 
    dest_height = pDestRect->y2 - pDestRect->y1; 

    CopyRect(&rc, pDestRect);
    CopyRect(&src_rect, pSrcRect);

    rcDevClip.x1 = rcDevClip.y1 = 0;
    rcDevClip.x2 = pDev->width;
    rcDevClip.y2 = pDev->height;

    //���ȣ����þ�����Ŀ���豸��С�󽻼�
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

    //Դ�豸���ھ���ӳ�䵽Ŀ�ľ�������ϵ��
    rcDestDev.x1 = (rcSrcDev.x1 - pSrcRect->x1) * dest_width / src_width
        + pDestRect->x1;
    rcDestDev.y1 = (rcSrcDev.y1 - pSrcRect->y1) * dest_height / src_height
        + pDestRect->y1;
    rcDestDev.x2 = ( (rcSrcDev.x2 - pSrcRect->x1) * dest_width + src_width
        - 1 ) / src_width + pDestRect->x1;
    rcDestDev.y2 = ( (rcSrcDev.y2 - pSrcRect->y1) * dest_height + src_height
        - 1 ) / src_height + pDestRect->y1;
    
    //���µļ��þ�����Ŀ�ľ��ν��м���
    if (!ClipRect(&rc, &rcDevClip))
        return -1;
    //�þ������õ�Ŀ�ľ�����ӳ�䵽Ŀ�ľ�������ϵ�µ�Դ�豸���ν��м���
    if (!ClipRect(&rc, &rcDestDev))
        return -1;

// ��Σ���Ŀ�ĵļ��þ���ӳ�䵽Դ��������ϵ��
    rcSrcClip.x1 = (rcDevClip.x1 - pDestRect->x1) * src_width
        / dest_width + pSrcRect->x1;
    rcSrcClip.x2 = ((rcDevClip.x2 - pDestRect->x1) * src_width + 
        dest_width - 1) / dest_width + pSrcRect->x1;
    rcSrcClip.y1 = (rcDevClip.y1 - pDestRect->y1) * src_height
        / dest_height + pSrcRect->y1;
    rcSrcClip.y2 = ((rcDevClip.y2 - pDestRect->y1) * src_height + 
        dest_height - 1) / dest_height + pSrcRect->y1;

    //Դ�����뾭��ӳ���Ŀ�ļ��þ����󽻼�
    if (!ClipRect(&src_rect, &rcSrcClip))
        return -1;
    
    if (!ClipRect(&src_rect, &rcSrcDev))
        return -1;
    
    _ExcludeCursor(pDev, &rc);

    //���¼�����
    src_width = src_rect.x2 - src_rect.x1;
    src_height = src_rect.y2 - src_rect.y1;
    dest_width = rc.x2 - rc.x1; 
    dest_height = rc.y2 - rc.y1; 

    Bitmap.mode       = DEV_MEMORY;
    Bitmap.bmpmode    = pDev->bmpmode;
    Bitmap.width      = dest_width;
    Bitmap.pix_bits   = 1;

    Bitmap.line_bytes = (Bitmap.width + 31) / 32 * 4;        
    if (Bitmap.line_bytes > sizeof(LineBuffer))
    {
        buflines = 1;
        Bitmap.height = 1;
        Bitmap.pdata = (uint8 *)malloc(Bitmap.line_bytes);
        if (!Bitmap.pdata)
            return -1;
    }
    else
    {
        buflines = sizeof(LineBuffer) / Bitmap.line_bytes;
        Bitmap.height = buflines;
        Bitmap.pdata  = (unsigned char *)LineBuffer;
    }

    //temp_rc�����ֶν�Դ���������ڵ���������Ŀ����,
    //count �ǽ���BitBlt�����ı�־,�Ƚ��г�ʼ��
    count = 0;
    temp_rc.x1 = rc.x1;
    temp_rc.x2 = temp_rc.x1 + dest_width;
    temp_rc.y1 = rc.y1;

    pSrc = (unsigned char*)pSrcDev->pdata + 
        pSrcDev->line_bytes * src_rect.y1 + src_rect.x1 / 8;
    
    if (Bitmap.pdata != (unsigned char*)LineBuffer)
        pDst = Bitmap.pdata;
    else
        pDst = (unsigned char*)LineBuffer;

    if (src_height >= dest_height)
        step_y = src_height;
    else
        step_y = dest_height;

    if (src_width >= dest_width)
        step_x = src_width;
    else
        step_x = dest_width;
    
    src_y   = src_rect.y1;
    dst_y   = 0;//rc.y1;
    src_ry  = dst_ry = 0; 
    
    for (ty = 0; ty < step_y; ty ++)
    {
        uint8 dst_mask, src_mask;

        pBitsSrc = pSrc;
        pBitsDst = pDst;

        dst_mask = 0x80;
        src_mask = 0x80 >> (src_rect.x1 & 7);

        if (src_y >= (src_rect.y1 + src_height) || 
            dst_y >= (rc.y1 + dest_height))
            break;
        else if (dst_y < 0 || src_y < 0)
            goto SBT_NEXT_Y;
            
        src_x   = src_rect.x1;
        dst_x   = 0;//rc.x1;
        src_rx  = dst_rx = 0;

        for (tx = 0; tx < step_x; tx ++)
        {
            if (src_x >= (src_rect.x1 + src_width) || 
                dst_x >= (rc.x1 + dest_width))
                break;
            else if (dst_x < 0 || src_x < 0)
                goto SBT_NEXT_X;
            
            if (*pBitsSrc & src_mask)
                *pBitsDst |= dst_mask;
            else
                *pBitsDst &= ~dst_mask;
    
            
SBT_NEXT_X: 
            src_rx += src_width;
            if (src_rx >= step_x)
            {
                src_rx -= step_x;
                src_x ++;
                src_mask >>= 1;
                if( src_mask == 0 )
                {
                    src_mask = 0x80;
                    pBitsSrc ++;
                }
            }// if( src_rx >= step_x )
            
            dst_rx += dest_width;
            if (dst_rx >= step_x)
            {
                dst_rx -= step_x;
                dst_x ++;
                dst_mask >>= 1;
                if( dst_mask == 0 )
                {
                    dst_mask = 0x80;
                    pBitsDst ++;
                }               
            }// if( dst_rx >= step_x )
        } // for x

SBT_NEXT_Y:
        src_ry += src_height;
        if (src_ry >= step_y)
        {
            src_ry -= step_y;
            src_y ++;
            pSrc += pSrcDev->line_bytes;
        }
        
        dst_ry += dest_height;
        if (dst_ry >= step_y)
        {
            dst_ry -= step_y;
            dst_y ++;
            pDst += Bitmap.line_bytes;
            count ++;
        }

        //�������ڴ����, ����BitBlt
        if (count == buflines)
        {
            temp_rc.y2 = temp_rc.y1 + count;
            
            //BitBlt( pDev, &temp_rc, (PDEV)&Bitmap, 0, 0, pGraphParam );
            MonoBitBlt(pDev, &temp_rc, (PDEV)&Bitmap, 0, 0,
                rop, pGraphParam->bk_mode, -Bitmap.height);

            temp_rc.y1 = temp_rc.y2;
            count   = 0;
            if (Bitmap.pdata != (unsigned char*)LineBuffer)
                pDst = Bitmap.pdata;
            else
                pDst = (unsigned char*)LineBuffer;
        }
    }// for y

    //����temp_rc����,��ȫ����
    if ( count != 0 )
    {
        temp_rc.y2 = temp_rc.y1 + count;
        //BitBlt( pDev, &temp_rc, (PDEV)&Bitmap, 0, 0, pGraphParam );
        MonoBitBlt(pDev, &temp_rc, (PDEV)&Bitmap, 0, 0, rop, 
            pGraphParam->bk_mode, -Bitmap.height);
    }

    if (Bitmap.pdata != (unsigned char*)LineBuffer)
        free(Bitmap.pdata);

    _UnexcludeCursor(pDev);

    return 1;
}
