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



static PPHYSBRUSH pCurBrush;
static uint8 shift_brush_data[64];
static int brushshift; 
static int BRUSHID = -1; 
static int offset_y;
static void Draw_Hline(PDEV pDev, int x1, int x2, int y, uint32 rop, 
                       uint32 color);
static int RectFill(PDEV pDev, PGRAPHPARAM pGraphParam, PRECT pRect,
                    int flags);
/******************************************************************\
* Function:  ShiftBrush
* Purpose:   ʵ��ˢ�ӵ�ѭ����λ
* Input:
*     pSrcBrush : ָ��Դˢ�ӵ�ָ�룻    
*     shift:      ˢ�ӵ���λ����
\******************************************************************/
static void ShiftBrush(PPHYSBRUSH pSrcBrush, uint32 shift, int flag)
{
    uint32 src_brush, shift1, shift2;
    uint8* pBrushData;
    uint8* pShiftData;
    int   i;
    
    pBrushData = pSrcBrush->pPattern;   
    pShiftData = shift_brush_data;
    
    //����ʵ��ˢ�ӵ�ѭ����λ
    for (i = 0; i <= 15; i++)
    {
        src_brush = GET32(pBrushData);
        
        shift1 = src_brush << shift;
        shift2 = src_brush >> (32 - shift);
        
        PUT32(pShiftData, shift1 | shift2);
        
        pBrushData += 4;
        pShiftData += 4;
    }

    if (flag == 1)
    {
        pShiftData = shift_brush_data;
        for (i = 0; i <= 15; i++)
        {
            src_brush = GET32(pShiftData);
            src_brush = ~src_brush;
            PUT32(pShiftData, src_brush);
            
            pShiftData += 4;
        }
    }
}

// �ڲ�����ʵ��,�����㷨��MonoPatBlt����һ��,��ϸ��MonoPatBlt����

static uint32 PBF_SRC(PDEV pDev, int x1, int y1, int x2, int y2,
                     PPHYSBRUSH pBrush, int org_x, int org_y,
                     int bk_mode, int bk_color, int fg_color)
{
    uint32   first_mask, last_mask;
    uint32   data, pat_data, dst, mask;
    uint32   addr;
    int      y;
    uint8*   pDevData;
    uint8*   pPatData;
    int      i,dwords;
    
    //����first_mask,last_mask��ֵ
    first_mask = GetLeftMask32(x1 & 31);
    last_mask = GetRightMask32(x2 & 31);
    
    addr = (uint32)pDev->pdata + y1 * pDev->line_bytes +x1 / 8
        - (x1 % 32) / 8; 
    dwords = x2 / 32 - x1 / 32 + 1;
    offset_y = (y1 - org_y) % pBrush->height;

    //���ֻ��һ��˫�֣������ó�1
    if (dwords == 1)
    {
        first_mask &= last_mask;
            
        if (bk_mode == BM_OPAQUE)        
        {
            if (fg_color == bk_color)
            {    
                if (bk_color == 1)
                    pat_data = 0xffffffff;
                else
                    pat_data = 0;

                for (y = y1; y <= y2; y++)
                {
                    pDevData = (uint8*)addr;
                    
                    data = GET32(pDevData);
                    data = data & ~first_mask | pat_data & first_mask;
                    PUT32(pDevData,data); 
                    
                    addr += pDev->line_bytes;
                }
                return 0;
            }

            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                data = GET32(pDevData);
                pat_data = GET32(pPatData); 
               
                data = data & ~first_mask | pat_data & first_mask;
                
                PUT32(pDevData,data); 
                
                offset_y++;
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
            return 0;
        }
        else 
        {//bk_mode != BM_OPAQUE
            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                pat_data = GET32(pPatData); 
                mask = pat_data;
                if (fg_color == 0)
                    pat_data = 0;
                
                data = GET32(pDevData);
                dst = data & ~first_mask | pat_data & first_mask;
                data = dst & mask | data & ~mask;
                
                PUT32(pDevData,data); 
                
                offset_y++;
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
            return 0;
        }
    }
    else // dwords != 1
    {
        if (bk_mode == BM_OPAQUE)
        {
            if (fg_color == bk_color)
            {    
                if (bk_color == 1)
                    pat_data = 0xffffffff;
                else
                    pat_data = 0;

                for (y = y1; y <= y2; y++)
                {
                    pDevData = (uint8*)addr;
                    
                    data = GET32(pDevData);
                    data = data & ~first_mask | pat_data & first_mask;
                    PUT32(pDevData,data);
                    pDevData += 4;
                    
                    for (i = 0; i < dwords - 2; i++)
                    {
                        PUT32(pDevData, pat_data);
                        pDevData += 4;
                    }
                    
                    data = GET32(pDevData);
                    data = data & ~last_mask | pat_data & last_mask;
                    PUT32(pDevData, data);
                    
                    addr += pDev->line_bytes;
                }
                return 0;
            }

            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                pat_data = GET32(pPatData);
               
                data = GET32(pDevData);
                data = data & ~first_mask | pat_data & first_mask;
                PUT32(pDevData,data);
                pDevData += 4;
                
                for (i = 0; i < dwords - 2; i++)
                {
                    PUT32(pDevData, pat_data);
                    pDevData += 4;
                }
                
                data = GET32(pDevData);
                data = data & ~last_mask | pat_data & last_mask;
                PUT32(pDevData, data);
                
                offset_y++; 
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
            return 0;
        }
        else//bk_mode != BM_OPAQUE
        {
            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                mask = pat_data = GET32(pPatData);
                if (fg_color == 0)
                    pat_data = 0;
                
                data = GET32(pDevData);
                dst = data & ~first_mask | pat_data & first_mask;
                data = dst & mask | data & ~mask;
                PUT32(pDevData, data);
                pDevData += 4;
                
                for (i = 0; i < dwords - 2; i++)
                {
                    data = GET32(pDevData);
                    data = pat_data & mask | data & ~mask;
                    PUT32(pDevData,data);
                    pDevData += 4;
                }
                
                data = GET32(pDevData);
                dst = data & ~last_mask | pat_data & last_mask;
                data = dst & mask | data & ~mask;
                PUT32(pDevData,data);
                                
                offset_y++; 
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
            return 0;
        }        
    }

    return 0;
}

static uint32 PBF_SRC_XOR_DST(PDEV pDev, int x1, int y1, int x2, int y2,
                     PPHYSBRUSH pBrush, int org_x, int org_y,
                     int bk_mode, int bk_color, int fg_color)
{
    uint32   first_mask, last_mask;
    uint32   data, pat_data, dst, mask;
    uint32   addr;
    int      y;
    uint8*   pDevData;
    uint8*   pPatData;
    int      i,dwords;
    
    //����first_mask,last_mask��ֵ
    first_mask = GetLeftMask32(x1 & 31);
    last_mask = GetRightMask32(x2 & 31);
    
    addr = (uint32)pDev->pdata + y1 * pDev->line_bytes +x1 / 8
        - (x1 % 32) / 8; 
    dwords = x2 / 32 - x1 / 32 + 1;
    offset_y = (y1 - org_y) % pBrush->height;

    //���ֻ��һ��˫�֣������ó�1
    if (dwords == 1)
    {
        first_mask &= last_mask;
            
        if (bk_mode == BM_OPAQUE)        
        {
            if (fg_color == bk_color)
            {    
                if (bk_color == 1)
                    pat_data = 0xffffffff;
                else
                    pat_data = 0;

                for (y = y1; y <= y2; y++)
                {
                    pDevData = (uint8*)addr;
                    
                    data = GET32(pDevData);
                    data =
                        data & ~first_mask | (pat_data ^ data) & first_mask;
                    PUT32(pDevData,data); 
                    
                    addr += pDev->line_bytes;
                }
                return 0;
            }

            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                data = GET32(pDevData);
                pat_data = GET32(pPatData); 
               
                data = data & ~first_mask | (pat_data ^ data) & first_mask;
                
                PUT32(pDevData,data); 
                
                offset_y++;
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
            return 0;
        }
        else 
        {//bk_mode != BM_OPAQUE
            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                pat_data = GET32(pPatData); 
                
                mask = pat_data;
                if (fg_color == 0)
                    pat_data = 0;

                data = GET32(pDevData);
                dst = data & ~first_mask | (pat_data ^ data) & first_mask;
                data = dst & mask | data & ~mask;
                
                PUT32(pDevData,data); 
                
                offset_y++;
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
            return 0;
        }
    }
    else // dwords != 1
    {
        if (bk_mode == BM_OPAQUE)
        {
            if (fg_color == bk_color)
            {    
                if (bk_color == 1)
                    pat_data = 0xffffffff;
                else
                    pat_data = 0;

                for (y = y1; y <= y2; y++)
                {
                    pDevData = (uint8*)addr;
                    
                    data = GET32(pDevData);
                    data =
                        data & ~first_mask | (pat_data ^ data) & first_mask;
                    PUT32(pDevData,data);
                    pDevData += 4;
                    
                    for (i = 0; i < dwords - 2; i++)
                    {
                        /* WWH modified on 2003/01/21. Add below line. */
                        data = GET32(pDevData);
                        PUT32(pDevData, (pat_data ^ data));
                        pDevData += 4;
                    }
                    
                    data = GET32(pDevData);
                    data =
                        data & ~last_mask | (pat_data ^ data) & last_mask;
                    PUT32(pDevData, data);
                    
                    addr += pDev->line_bytes;
                }
                return 0;
            }

            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                pat_data = GET32(pPatData);
               
                data = GET32(pDevData);
                data = data & ~first_mask | (pat_data ^ data) & first_mask;
                PUT32(pDevData,data);
                pDevData += 4;
                
                for (i = 0; i < dwords - 2; i++)
                {
                    /* WWH modified on 2003/01/21. Add below line. */
                    data = GET32(pDevData);
                    PUT32(pDevData, (pat_data ^ data));
                    pDevData += 4;
                }
                
                data = GET32(pDevData);
                data = data & ~last_mask | (pat_data ^ data) & last_mask;
                PUT32(pDevData, data);
                
                offset_y++; 
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
            return 0;
        }
        else//bk_mode != BM_OPAQUE
        {
            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                mask = pat_data = GET32(pPatData);
                if (fg_color == 0)
                    pat_data = 0;
         
                data = GET32(pDevData);
                dst = data & ~first_mask | (pat_data ^ data) & first_mask;
                data = dst & mask | data & ~mask;
                PUT32(pDevData, data);
                pDevData += 4;
                
                for (i = 0; i < dwords - 2; i++)
                {
                    data = GET32(pDevData);
                    data = (pat_data ^ data) & mask | data & ~mask;
                    PUT32(pDevData,data);
                    pDevData += 4;
                }

                data = GET32(pDevData);
                dst = data & ~last_mask | (pat_data ^ data) & last_mask;
                data = dst & mask | data & ~mask;
                PUT32(pDevData,data);

                offset_y++; 
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
            return 0;
        }        
    }

    return 0;
}

static uint32 PBF_NSRC(PDEV pDev, int x1, int y1, int x2, int y2,
                      PPHYSBRUSH pBrush, int org_x, int org_y,
                      int bk_mode, int bk_color, int fg_color)
{
    uint32   first_mask, last_mask;
    uint32   data, pat_data, dst, mask;
    uint32   addr;
    int      y;
    uint8*   pDevData;
    uint8*   pPatData;
    int      i,dwords;
    
    //����first_mask,last_mask��ֵ
    first_mask = GetLeftMask32(x1 & 31);
    last_mask = GetRightMask32(x2 & 31);
    
    addr = (uint32)pDev->pdata + y1 * pDev->line_bytes + x1 / 8
        - (x1 % 32) / 8; 
    dwords = x2 / 32 - x1 / 32 + 1;
    offset_y = (y1 - org_y) % pBrush->height;

    //���ֻ��һ��˫�֣������ó�1
    if (dwords == 1)
    {
        first_mask &= last_mask;
            
        if (bk_mode == BM_OPAQUE)        
        {
            if (fg_color == bk_color)
            {    
                if (bk_color == 1)
                    pat_data = 0xffffffff;
                else
                    pat_data = 0;

                for (y = y1; y <= y2; y++)
                {
                    pDevData = (uint8*)addr;
                    
                    data = GET32(pDevData);
                    data = data & ~first_mask | ~pat_data & first_mask;
                    PUT32(pDevData,data); 
                    
                    addr += pDev->line_bytes;
                }
                return 0;
            }

            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                data = GET32(pDevData);
                pat_data = GET32(pPatData);  
                data = data & ~first_mask | ~pat_data & first_mask;
                PUT32(pDevData,data); 
                
                offset_y++;
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
            return 0;
        }
        else 
        {//bk_mode != BM_OPAQUE
            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                mask = pat_data = GET32(pPatData); 
                if (fg_color == 0)
                    pat_data = 0;
                
                data = GET32(pDevData);
                dst = data & ~first_mask | ~pat_data & first_mask;
                data = dst & mask | data & ~mask;
                PUT32(pDevData,data); 
                
                offset_y++;
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
            return 0;
        }
    }
    else // dwords != 1
    {
        if (bk_mode == BM_OPAQUE)
        {
            if (fg_color == bk_color)
            {    
                if (bk_color == 1)
                    pat_data = 0xffffffff;
                else
                    pat_data = 0;

                for (y = y1; y <= y2; y++)
                {
                    pDevData = (uint8*)addr;
                    
                    data = GET32(pDevData);
                    data = data & ~first_mask | ~pat_data & first_mask;
                    PUT32(pDevData,data);
                    pDevData += 4;
                    
                    for (i = 0; i < dwords - 2; i++)
                    {
                        PUT32(pDevData, ~pat_data);
                        pDevData += 4;
                    }
                    
                    data = GET32(pDevData);
                    data = data & ~last_mask | ~pat_data & last_mask;
                    PUT32(pDevData, data);
                    
                    addr += pDev->line_bytes;
                }
                return 0;
            }

            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                pat_data = GET32(pPatData);
               
                data = GET32(pDevData);
                data = data & ~first_mask | ~pat_data & first_mask;
                PUT32(pDevData,data);
                pDevData += 4;
                
                for (i = 0; i < dwords - 2; i++)
                {
                    PUT32(pDevData, ~pat_data);
                    pDevData += 4;
                }
                
                data = GET32(pDevData);
                data = data & ~last_mask | ~pat_data & last_mask;
                PUT32(pDevData, data);
                
                offset_y++; 
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
            return 0;
        }
        else//bk_mode != BM_OPAQUE
        {
            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                mask = pat_data = GET32(pPatData);
                if (fg_color == 0)
                    pat_data = 0;
                
                data = GET32(pDevData);
                dst = data & ~first_mask | ~pat_data & first_mask;
                data = dst & mask | data & ~mask;
                PUT32(pDevData, data);
                pDevData += 4;
                
                for (i = 0; i < dwords - 2; i++)
                {
                    data = GET32(pDevData);
                    data = ~pat_data & mask | data & ~mask;
                    PUT32(pDevData,data);
                    pDevData += 4;
                }
                
                data = GET32(pDevData);
                dst = data & ~last_mask | ~pat_data & last_mask;
                data = dst & mask | data & ~mask;
                PUT32(pDevData,data);

                offset_y++; 
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
            return 0;
        }        
    }

    return 0;
}

/********************************************************************\
* Function: MonoPatBlt
* Purpose:
*     Block pattern fill.
* Input:
*     pDev     ָ��Ŀ���豸��ָ�룻
*     x1,y1    Ŀ�ľ��ε����Ͻ�����
*     x2,y2    Ŀ�ľ��ε����½�����
*     pBrush   ָ��ˢ�ӵ�ָ��
*     org_x    ˢ��ԭ��ĺ�����
*     org_y    ˢ��ԭ���������
*     rop      ��դ������
*     bk_mode  ��ͼ�����ı���ģʽ
*     bk_color ��ͼ�����ı�����ɫ
*     fg_color ��ͼ������ǰ����ɫ
*
\********************************************************************/
static int MonoPatBlt(PDEV pDev, int x1, int y1, int x2, int y2,
                        PPHYSBRUSH pBrush, int org_x, int org_y, 
                        uint32 rop,int bk_mode, int bk_color,
                        int fg_color)
{
    uint32   first_mask, last_mask;
    uint32   data,pat_data,dst,mask;
    uint32   addr;
    uint8*   pDevData;
    uint8*   pPatData;
    ROPFUNC  ropfunc;
    int      y,i,dwords, flag = 0;

    //PatBlt�ڵ��ñ�����ʱ��Ӧ��֤pBrush��Ϊ��      
    //if (pBrush != NULL) 
    //  ��ʹ������ģʽ��ˢ�ӻ�ˢ��ԭ�������ı�ʱ��Ҫ��λ
    if (fg_color == 0)
        flag = 1;

    if ((!(pCurBrush == pBrush/* && BRUSHID == pBrush->id */ && 
        brushshift == org_x)) || (flag == 1))
    {
        pCurBrush = pBrush;
        /* The follow is modified by WWH on 2002/08/13 
        ** Then the patblt is consistent. 
        */
        brushshift = (x1 - (x1 % 32) - org_x) % pBrush->width;
//        BRUSHID = pBrush->id;
        // ����ˢ�ӵ���λ
        ShiftBrush(pBrush, brushshift, flag);
    }

    // Ϊ��ʡʱ��. ���������������������
    switch (rop)
    {
    case ROP_SRC :
        PBF_SRC(pDev, x1, y1, x2, y2, pBrush, org_x, org_y, 
            bk_mode, bk_color, fg_color);
        return 0;
    case ROP_NSRC :
        PBF_NSRC(pDev, x1, y1, x2, y2, pBrush, org_x, org_y,
            bk_mode, bk_color, fg_color);
        return 0;
    case ROP_SRC_XOR_DST:
        PBF_SRC_XOR_DST(pDev, x1, y1, x2, y2, pBrush, org_x, org_y,
            bk_mode, bk_color, fg_color);
        return 0;
    }
    
    // ���ù�դ����
    ropfunc = GetRopFunc(rop);
    
    first_mask = GetLeftMask32(x1 & 31);
    last_mask = GetRightMask32(x2 & 31);
    
    // ���Ŀ�ľ������Ͻ����ڴ��ж�Ӧ�ĵ�ַ
    addr = (uint32)pDev->pdata + y1 * pDev->line_bytes + x1 / 8
        - (x1 % 32) / 8;
    // Ŀ�ľ���һɨ�����ж���˫��
    dwords = x2 / 32 - x1 / 32 + 1;
    // Ŀ�ľ������Ͻǵ������ˢ��ԭ���y�����λ�� 
    offset_y = (y1 - org_y) % pBrush->height;

    if (dwords == 1)
    {
        first_mask &= last_mask;

        if (bk_mode == BM_OPAQUE)
        {
            if (fg_color == bk_color)
            {    
                if (bk_color == 1)
                    pat_data = 0xffffffff;
                else
                    pat_data = 0;
                for (y = y1; y <= y2; y++)
                {
                    pDevData = (uint8*)addr;
                    
                    data = GET32(pDevData);
                    /* WWH modified on 2003/01/21.use the rop function */
//                    data = data & ~first_mask | pat_data & first_mask;
                    data = data & ~first_mask |
                        (*ropfunc)(pat_data, data) & first_mask;
                    PUT32(pDevData,data); 
                    
                    addr += pDev->line_bytes;
                }
                return 0;
            }
            
            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                //ȡˢ�����ݵ���ʼ��ַ
                pPatData = shift_brush_data + offset_y * 4;
                
                data = GET32(pDevData);
                pat_data = GET32(pPatData);
                
                data = data & ~first_mask |
                    (*ropfunc)(pat_data, data) & first_mask;
                    
                PUT32(pDevData,data);
                
                offset_y++;
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
        }
        else//bk_mode == BM_TRANSPARENT 
        {
            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                //ȡˢ�����ݵ���ʼ��ַ
                pPatData = shift_brush_data + offset_y * 4;
                
                data = GET32(pDevData);
                pat_data = GET32(pPatData);
                mask = pat_data;

                if (fg_color == 0)
                    pat_data = 0;
                
                dst = data & ~first_mask |
                    (*ropfunc)(pat_data, data) & first_mask;
                data = dst & mask | data & ~mask;
                
                PUT32(pDevData,data);
                
                offset_y++;
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
        }        
    }
    //ÿɨ�����ж��˫��,ÿһ˫�ֵĴ�����ǰ����ͬ
    else//dwords != 1 
    {
        if (bk_mode == BM_OPAQUE)
        {
            if (fg_color == bk_color)
            {    
                if (bk_color == 1)
                    pat_data = 0xffffffff;
                else
                    pat_data = 0;

                for (y = y1; y <= y2; y++)
                {
                    pDevData = (uint8*)addr;
                    
                    data = GET32(pDevData);
                    data = data & ~first_mask |
                        (*ropfunc)(pat_data, data) & first_mask;
                    PUT32(pDevData,data);                    
                    pDevData += 4;
                    
                    for (i = 0; i < dwords - 2; i++)
                    {
                        PUT32(pDevData, pat_data);                    
                        pDevData += 4;
                    }
                    
                    data = GET32(pDevData);
                    data = data & ~last_mask | 
                        (*ropfunc)(pat_data, data) & last_mask;
                    PUT32(pDevData, data);
                    
                    addr += pDev->line_bytes;
                }
                return 0;
            }

            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                pat_data = GET32(pPatData);
 
                // �����һ��˫�� 
                data = GET32(pDevData);
                data = data & ~first_mask |
                    (*ropfunc)(pat_data, data) & first_mask;
                PUT32(pDevData,data);
                
                pDevData += 4;
                
                // �����м��˫��
                for (i = 0; i < dwords - 2; i++)
                {
                    data = GET32(pDevData);
                    data = (*ropfunc)(pat_data, data);
                    PUT32(pDevData,data);
                    
                    pDevData += 4;
                }
                
                // �������һ��˫�� 
                data = GET32(pDevData);
                data = data & ~last_mask |
                    (*ropfunc)(pat_data, data) & last_mask;
                PUT32(pDevData,data);
                
                offset_y++;
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
        }
        else //bk_mode != BM_OPAQUE
        {
            for (y = y1; y <= y2; y++)
            {
                pDevData = (uint8*)addr;
                pPatData = shift_brush_data + offset_y * 4;
                
                pat_data = GET32(pPatData);
                mask = pat_data;

                if (fg_color == 0)
                    pat_data = 0;

                // �����һ��˫�� 
                data = GET32(pDevData);
                dst = data & ~first_mask |
                    (*ropfunc)(pat_data, data) & first_mask;
                data = dst & mask | data & ~mask;
                PUT32(pDevData,data);
                pDevData += 4;
                
                // �����м��˫�� 
                for (i = 0; i < dwords - 2; i++)
                {
                    data = GET32(pDevData);
                    dst = (*ropfunc)(pat_data, data);
                    data = dst & mask | data & ~mask; 
                    PUT32(pDevData,data);
                    
                    pDevData += 4;
                }
                
                // �������һ��˫�� 
                data = GET32(pDevData);
                dst = data & ~last_mask |
                    (*ropfunc)(pat_data, data) & last_mask;
                data = dst & mask | data & ~mask;
                PUT32(pDevData,data);
                
                offset_y++;
                if (offset_y == pBrush->height)
                    offset_y = 0;
                
                addr += pDev->line_bytes;
            }
        }
        
    }

    return 1;
}

/*
**  Function : FillSolidRect
**  Purpose  :
**      Fill the specified rectangle using the specified color
*/

static void FillSolidRect(PDEV pDev, PRECT pRect, uint32 color, int32 rop)
{
    ROPFUNC ropfunc;
    uint8   *pDst, *p;
    int     i, dwords;
    int     line_bytes;
    int     x1, x2, y1, y2, y;
    uint32  first_mask, last_mask;
    uint32  data, pat_data;
//   uint32  pixel, fillcolor;
//   uint8   mask;
//    int     j;

#if 1
    ASSERT(rop <= 16);
    x1 = pRect->x1;
    x2 = pRect->x2 - 1;
    y1 = pRect->y1;
    y2 = pRect->y2 - 1;
    
    ASSERT(x2 >= x1 && y2 >= y1);
    //����first_mask,last_mask��ֵ
    first_mask = GetLeftMask32(x1 & 31);
    last_mask = GetRightMask32(x2 & 31);

    pDst = GETXYADDR(pDev, x1, y1) - (x1 % 32) / 8; 
    dwords = x2 / 32 - x1 / 32 + 1;
    line_bytes = pDev->line_bytes;
    pat_data = (color == 0) ? 0 : 0xFFFFFFFFL;

    switch(rop)
    {
    case ROP_SRC:
        //���ֻ��һ��˫�֣������ó�1
        if (dwords == 1)
        {
            first_mask &= last_mask;

            for (y = y1; y <= y2; y++)
            {
                data = GET32(pDst);
                data = data & ~first_mask | pat_data & first_mask;
                PUT32(pDst, data); 
            
                pDst += line_bytes;
            }
        }
        else // dwords != 1
        {
            for (y = y1; y <= y2; y++)
            {
                p = pDst;
                data = GET32(p);
                data = data & ~first_mask | pat_data & first_mask;
                PUT32(p,data);
                p += 4;

                for (i = 0; i < dwords - 2; i++)
                {
                    PUT32(p, pat_data);
                    p += 4;
                }

                data = GET32(p);
                data = data & ~last_mask | pat_data & last_mask;
                PUT32(p, data);

                pDst += line_bytes;
            }
        }

        break;

    default:
        ropfunc = GetRopFunc(rop);

        //���ֻ��һ��˫�֣������ó�1
        if (dwords == 1)
        {
            first_mask &= last_mask;

            for (y = y1; y <= y2; y++)
            {
                data = GET32(pDst);
                data = data & ~first_mask | 
                    ropfunc(pat_data, data) & first_mask;
                PUT32(pDst, data); 
            
                pDst += line_bytes;
            }
        }
        else // dwords != 1
        {
            for (y = y1; y <= y2; y++)
            {
                p = pDst;
                data = GET32(p);
                data = data & ~first_mask | 
                    ropfunc(pat_data, data) & first_mask;
                PUT32(p,data);
                p += 4;

                for (i = 0; i < dwords - 2; i++)
                {
                    data = ropfunc(pat_data, GET32(p));
                    PUT32(p, data);
                    p += 4;
                }

                data = GET32(p);
                data = data & ~last_mask | 
                    ropfunc(pat_data, data) & last_mask;
                PUT32(p, data);

                pDst += line_bytes;
            }
        }
        break;
    }

    return;

#else
    fillcolor = (color == 0) ? 0x00 : 0xFF;
    pDst = GETXYADDR(pDev, pRect->x1, pRect->y1);
    mask = 0x80 >> (pRect->x1 % 8);
    line_bytes = pDev->line_bytes;
    
    switch (rop)
    {
    case ROP_SRC :
        
        for (j = pRect->x1; j < pRect->x2; j++)
        {
            p = pDst;
            
            for (i = pRect->y1; i < pRect->y2; i++)
            {
                PUTPIXEL(p, fillcolor, mask);
                p += line_bytes;
            }
            mask >>= 1;
            if (mask == 0)
            {
                mask = 0x80;
                pDst++;
            }
        }

        break;
        
    default :
        
        ropfunc = GetRopFunc(rop);
        
        for (j = pRect->x1; j < pRect->x2; j++)
        {
            p = pDst;
            
            for (i = pRect->y1; i < pRect->y2; i++)
            {
                pixel = ropfunc(fillcolor, GETPIXEL(p));
                PUTPIXEL(p, pixel, mask);
                p += line_bytes;
            }
            
            mask >>= 1;
            if (mask == 0)
            {
                mask = 0x80;
                pDst++;
            }
        }
        
        break;
    }
#endif
}

/*********************************************************************\
*  Function :   PatBlt
*  Purpose: 
*      Block pattern fill.
*  INPUT:
*     pDev        ָ����ʾ�豸��ָ�룻
*     pDestRect   ָ��Ŀ�ľ��ε�ָ�룻
*     pGraphParam ָ���ͼ�����ṹ��ָ�롣
*
\*********************************************************************/

static int PatBlt(PDEV pDev, PRECT pRect, PGRAPHPARAM pGraphParam)
{
    uint32   rop = ROP_SRC;
    PPHYSBRUSH  pBrush;
    int bk_mode;
    RECT rc;

    //��Ҫ��ʾĿ�ľ��ν��вü�
    CopyRect(&rc, pRect);
    ClipByDev(&rc, pDev);
    if (!IsValidRect(&rc))
        return -1;

    _ExcludeCursor(pDev, &rc);
    
    bk_mode = pGraphParam->bk_mode;
    
    if (!(pGraphParam->flags & GPF_BRUSH))
        pBrush = NULL;
    else
        pBrush = pGraphParam->pBrush;

    if (!(pGraphParam->flags & GPF_ROP2))
        rop = ROP_SRC;
    else
        rop = PRop3ToRop2(pGraphParam->rop);
    
    if (!IsValidRect(&rc))
        return 0;

    if ((pGraphParam->flags & GPF_CLIP_RECT) && 
        !ClipRect(&rc, &(pGraphParam->clip_rect)))
        return 0;
    
    //�հ�ˢ��ʱ����������ֱ�ӷ��ء�
    if (pBrush != NULL && pBrush->style == BS_HOLLOW)
        return 0;
    
    //���ϲ�Ҫ��ĳ����ɫ��ȫ���Ŀ�ľ���ʱ�����԰�ˢ���óɿգ��Ϳ���
    //ȥ����һ����ˢ�ӵĲ��衣
    if (pBrush == NULL)
//        RectFill(pDev, pGraphParam, &rc, 0);
        FillSolidRect(pDev, &rc, pGraphParam->bk_color, rop);
    else if (pBrush->style == BS_SOLID)//��ˢ����ɫ����ʵ���
    {        
        FillSolidRect(pDev, &rc, pGraphParam->brush_color, rop);
/*
        bk_color = pGraphParam->bk_color;
        pGraphParam->bk_color = pBrush->color;
        RectFill(pDev, pGraphParam, &rc, 0);
        pGraphParam->bk_color = bk_color;
*/
    }
    else if(pBrush->style == BS_HATCHED)
    {
        //����Ӱˢ�ӽ������
        MonoPatBlt(pDev, rc.x1, rc.y1, rc.x2 - 1, 
            rc.y2 - 1, pBrush, pGraphParam->brush_org_x,
            pGraphParam->brush_org_y, rop, bk_mode, 
            pGraphParam->bk_color, pGraphParam->brush_color);
    }   

    _UnexcludeCursor(pDev);

    return 0;
}

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
    int     dir;
    uint32  pixel;
    uint32  bkcolor, pencolor;
    uint32  penmask, penpattern, endmask;
    uint8   mask;
    ROPFUNC ropfunc;
    ROPFUNC ropDashfunc;
    int     i;
    uint32  rop;

    rop = PRop3ToRop2(pGraphParam->rop);
    ASSERT(penstyle.count >= 1);

    pDst = GETXYADDR(pDev, x1, y);
    mask = 0x80 >> (x1 % 8);

    if (x1 > x2)
    {
        EXCHANGE(x1, x2);
        dir = -1;
    }
    else
    {
        dir = 1;
    }

    penmask = 0x80000000L;
    /* When the penstyle.count = 32, endmask will equal to penmask!!
    */
    endmask = penmask >> (penstyle.count - 1);
    endmask >>= 1;
    penpattern = penstyle.pattern;
    bkcolor = (pGraphParam->bk_color == 0) ? 0x00 : 0xFFFFFFFFL;
    pencolor = (pGraphParam->brush_color == 0) ? 0x00 : 0xFFFFFFFFL;

    ropfunc = GetRopFunc(rop);
    if (pGraphParam->bk_mode == BM_TRANSPARENT)
        ropDashfunc = GetRopFunc(ROP_DST);
    else
        ropDashfunc = ropfunc;

    if (dir == 1)
    {
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
            
            PUTPIXEL(pDst, pixel, mask);
            
            mask >>= 1;
            if (mask == 0)
            {
                mask = 0x80;
                pDst++;
            }
            
            penmask >>= 1;
            if (penmask == endmask)
            {
                penmask = 0x80000000L;
            }
        }
    }
    else
    {
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
            
            PUTPIXEL(pDst, pixel, mask);
            
            mask <<= 1;
            if (mask == 0)
            {
                mask = 0x01;
                pDst--;
            }
            
            penmask >>= 1;
            if (penmask == endmask)
            {
                penmask = 0x80000000L;
            }
        }
    }
}

static void DrawSolidHLine(PDEV pDev, int x1, int x2, int y, uint32 rop, 
                           uint32 color);

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
            DrawSolidHLine(pDev, x1, x2, y,
                rop, pGraphParam->bk_color);
        }
        else 
        {
            DrawSolidHLine(pDev, x1, x2, y,
                rop, pGraphParam->bk_color);
        }
        return 0;
    }

    if (pBrush->style == BS_NULL)
    {
        return 0;
    }    

    else if (pBrush->style == BS_SOLID || pBrush->pPattern == NULL)
    {
            DrawSolidHLine(pDev, x1, x2, y,
                rop, pGraphParam->brush_color);
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

        /* The hatch pattern is 32 bit width */
        s_scanw = (pBrush->width + 31) / 32 * 4;
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
        return 0;
/*
        RECT rc;
        SetRect(&rc, x1, y, x2 + 1, y + 1);
        PatBlt(pDev, &rc, pGraphParam);
*/
    }
    else if (pBrush->style == BS_BITMAP)
    {
        return 0;
    /*
        RECT rc;
        SetRect(&rc, x1, y, x2 + 1, y + 1);
        PatBlt(pDev, &rc, pGraphParam);
*/
    }

    return 0;
}
