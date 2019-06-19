/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display driver
 *
 * Purpose  : Implement the bitblt functions.
 *            
\**************************************************************************/

// Internal function prototypes
static void BBF_NDST(PDEV pDestDev, int dest_x, int dest_y,
              int width, int height);

static void BBF_SRC(PDEV pDestDev, int dest_x, int dest_y, int width, 
                    int height, PDEV pSrcDev, int src_x, int src_y, 
                    int bmp_height);

static void BBF_NSRC(PDEV pDestDev, int dest_x, int dest_y, int width, 
                     int height, PDEV pSrcDev, int src_x, int src_y,
                     int bmp_height);

static void BBF_SRC_OR_DST(PDEV pDestDev, int dest_x, int dest_y, 
                    int width, int height, PDEV pSrcDev, 
                    int src_x, int src_y, int bmp_height);

static void BBF_NSRC_AND_DST(PDEV pDestDev, int dest_x, int dest_y, 
                      int width, int height, PDEV pSrcDev, 
                      int src_x, int src_y, int bmp_height);

static void BBF_SRC_AND_DST(PDEV pDestDev, int dest_x, int dest_y, 
                      int width, int height, PDEV pSrcDev, 
                      int src_x, int src_y, int bmp_height);

static void BBF_SRC_XOR_DST(PDEV pDestDev, int dest_x, int dest_y, 
                      int width, int height, PDEV pSrcDev, 
                      int src_x, int src_y, int bmp_height);


/**************************************************************************\
 *
 * Block transfer.
 *
\**************************************************************************/

/*
static int SpecialMono(PDEV pDestDev, int dst_x, int dst_y, 
            int width, int height, PDEV pSrcDev, int src_x, 
            int src_y, uint32 rop, int bk_mode);
*/

/**************************************************************************\
 *
 *  Function:   BitBlt
 *  Purpose:
 *      ������ʾ�豸֮�䡢�ڴ��豸֮�䡢������ʾ�豸���ڴ��豸֮����п鴫�ͣ�
 *  INPUT:
 *
 *      pDstDev     ָ��Ŀ���豸��ָ�룻
 *      pDestRect   ָ��Ŀ�ľ��ε�ָ�룻
 *      pSrcDev     ָ��Դ�豸��ָ�룻
 *      x , y       Դ�豸������ʼ��(���Ͻ�)x, y����;
 *      pGraphParam ָ���ͼ�����ṹ��ָ��.
 *
\**************************************************************************/

static int BitBlt(PDEV pDstDev, PRECT pDestRect, PDEV pSrcDev,
                  int x, int y, PGRAPHPARAM pGraphParam)
{
    RECT rect, src_rect;
    int32 rop = ROP_SRC, bk_color, fg_color;
    int bk_mode = BM_OPAQUE;
    int ret = 0;

    CopyRect(&rect, pDestRect);

    src_rect.x1 = x;
    src_rect.y1 = y;
    src_rect.x2 = x + (rect.x2 - rect.x1);
    src_rect.y2 = y + (rect.y2 - rect.y1);

    /*����Ŀ���豸���Ͷ�Ŀ�ľ��ν�����Ե���*/
    ClipByDev(&rect, pDstDev);
    if (!IsValidRect(&rect))
        RETURN(-1);

    _ExcludeCursor(pDstDev, &rect);

    /* �����ͼ����Ϊ�գ���Ĭ��ֵ*/
    if (pGraphParam != NULL)
    {
        rop = SRop3ToRop2(pGraphParam->rop);
        bk_mode = pGraphParam->bk_mode;
        bk_color = pGraphParam->bk_color;
        fg_color = 0;

        /* ���ü�����*/
        if ((pGraphParam->flags & GPF_CLIP_RECT) && 
            !ClipRect(&rect, &pGraphParam->clip_rect))
            RETURN(0);
    }

    if (pSrcDev == NULL)
    {
        /* ���Դ�豸Ϊ��, ����Ƿ�rop����Ч����Դ����*/
        if (rop != ROP_SET_DST_0 && rop != ROP_SET_DST_1 && 
            rop != ROP_DST && rop != ROP_NDST)
        RETURN(-1);
    }
    else
        ClipByDev(&src_rect, pSrcDev);

    //����Ŀ�ľ��Ρ�Դ���� 
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

    src_rect.x1 = rect.x1;
    src_rect.y1 = rect.y1;

    src_rect.x1 += x;
    src_rect.y1 += y;

    rect.x1 += pDestRect->x1;
    rect.y1 += pDestRect->y1;
    rect.x2 += pDestRect->x1;
    rect.y2 += pDestRect->y1;

    ret = MonoBitBlt(pDstDev, &rect, pSrcDev, src_rect.x1, 
        src_rect.y1, rop, bk_mode, -pSrcDev->height);

exit :

    //���»�ù��

    _UnexcludeCursor(pDstDev);

    return ret;
}

/**************************************************************************\
 *
 *  Function :TextBlt
 *  Purpose  : 
 *      ���ı�λͼ������ʾ�豸��
 *  INPUT:
 *      pDev        ָ����ʾ�豸��ָ�룻
 *      x , y       �ı��������ʼ�㣻
 *      pTextBmp    ָ���ı�λͼ��ָ�룻
 *      pGraphParam ָ���ͼ�����ṹ��ָ��.
 *
\**************************************************************************/

static int TextBlt(PDEV pDev, int x, int y, PTEXTBMP pTextBmp, 
                   PGRAPHPARAM pGraphParam)
{
    RECT rect;
    int offset_x = 0, offset_y = 0;
    int ret = 0;
    MEMDEV mem_dev;
    int32 rop;
    int height, width;

    height = pTextBmp->height;
    width = pTextBmp->width;

    //��ʼ��Ŀ�ľ���
    rect.x1 = x;
    rect.y1 = y;
    rect.x2 = x + width;
    rect.y2 = y + height;

    ClipByDev(&rect, pDev);
    _ExcludeCursor(pDev, &rect);

    // ���ü����� 
    if ((pGraphParam->flags & GPF_CLIP_RECT)
        && !ClipRect(&rect, &pGraphParam->clip_rect))
        RETURN(0);

    //����Դ����
    offset_x += rect.x1 - x;
    offset_y += rect.y1 - y;

    // ����Ŀ�ľ���
    if (rect.x2 - rect.x1 > width - offset_x)
        rect.x2 = rect.x1 + width - offset_x;
    if (rect.y2 - rect.y1 > height - offset_y)
        rect.y2 = rect.y1 + height - offset_y;

    if (IsValidRect(&rect))
    {   
        mem_dev.mode = DEV_MEMORY;
        mem_dev.bmpmode = pDev->bmpmode;
        mem_dev.width = width;
        mem_dev.height = -height;
        mem_dev.line_bytes = (mem_dev.width + 31)/ 32 * 4;
        mem_dev.pix_bits = 1;
        mem_dev.pdata = (uint8*)pTextBmp->data;

        if (pGraphParam->bk_mode == BM_TRANSPARENT)
        {   
            if (pGraphParam->text_color == 1)
                rop = ROP_SRC_OR_DST;
            else
                rop = ROP_NSRC_AND_DST;
        }
        else
        {
            if (pGraphParam->text_color == 1)
                rop = ROP_SRC;
            else
                rop = ROP_NSRC;
        }

        ret = MonoBitBlt(pDev, &rect, (PDEV)&mem_dev, offset_x,
            offset_y, rop, pGraphParam->bk_mode, mem_dev.height);
    }

exit :

    _UnexcludeCursor(pDev);

    return ret;
}

static int MonoBitBlt(PDEV pDestDev, PRECT pDestRect, PDEV pSrcDev,
                      int src_x, int src_y, uint32 rop, int bk_mode, 
                      int bmp_height)
{
    uint32   first_mask, last_mask;
    uint32   dest_addr, src_addr; 
    uint32   src,src_data,pre_src_data;
    uint32   dest_data;
    uint32   pitch, dst_pitch;
    uint32   x1, x2, y1, y2, y;
    uint8*   pDestData;
    uint8*   pSrcData;
    int      shift, i,dwords, step;
    int      width, height;
    ROPFUNC  ropfunc;

    if (pSrcDev == NULL)
        return 1;

    // Converts parameters
    width = pDestRect->x2 - pDestRect->x1;
    height = pDestRect->y2 - pDestRect->y1;

    //    ��������������ڴ�����width<=16��λͼ, ��Ҫ�������������
    //ģʽ�У�����ʾһ���֣���Ҫ������λͼ����������֯���γ�width<=16
    //��λͼ������԰�32λ������Ŀ�ľ��εĺ�����X1,X2�ֱ�λ��������
    //��32λʱ��Ҫ��Դλͼ��ȡ����dword��������ٶȵĽ��͡�������ĺ�
    //�����⴦������Դλͼָ��ʹ��ǡ����һ��32λ������ɡ�
/*    if ((width <= 16) && ((pDestRect->x1 % 32) > 16))
    {
        return SpecialMono(pDestDev, pDestRect->x1, pDestRect->y1, 
            width, height, pSrcDev, src_x, src_y, rop, bk_mode);
    }
*/

    switch (rop)
    {
        case ROP_NDST:

            BBF_NDST(pDestDev, pDestRect->x1, pDestRect->y1, 
                width, height);
            return 1;

        case ROP_SRC:

            BBF_SRC(pDestDev, pDestRect->x1, pDestRect->y1, width, 
                height, pSrcDev, src_x, src_y, bmp_height);
            return 1;

        case ROP_NSRC :

            BBF_NSRC(pDestDev, pDestRect->x1, pDestRect->y1,
                width, height, pSrcDev, src_x, src_y, bmp_height);
            return 1;

        //�������������������ַ�����
        case ROP_SRC_OR_DST:

            // ��ʾģʽΪ͸��, ǰ��ɫ�а�ɫ��
            BBF_SRC_OR_DST(pDestDev, pDestRect->x1, pDestRect->y1,
                width, height, pSrcDev, src_x, src_y, bmp_height);
            return 1;

        case ROP_NSRC_AND_DST:

            //��ʾģʽΪ͸��, ǰ��ɫ�к�ɫ��
            BBF_NSRC_AND_DST(pDestDev, pDestRect->x1, pDestRect->y1,
                width, height, pSrcDev, src_x, src_y, bmp_height);
            return 1;

        case ROP_SRC_AND_DST:

            BBF_SRC_AND_DST(pDestDev, pDestRect->x1, pDestRect->y1,
                width, height, pSrcDev, src_x, src_y, bmp_height);
            return 1;

        case ROP_SRC_XOR_DST:   

            BBF_SRC_XOR_DST(pDestDev, pDestRect->x1, pDestRect->y1,
                width, height, pSrcDev, src_x, src_y, bmp_height);
            return 1;

        default:

            break;
    }

    // Compute clip area
    x1 = pDestRect->x1; 
    y1 = pDestRect->y1; 
    y2 = y1 + height - 1; 
    x2 = x1 + width - 1;

    ropfunc = GetRopFunc(rop);

    /*���Ŀ���豸��Դ�豸��ͬһ���豸��Դ������Ŀ�ľ��β����غ�ʱ����Դ
    *��Ŀ�ĵĿ�����Ҫ��������ۣ�
    *1.��src_y < dest_y ʱ�����ߣ�����src_y == dest_y && src_x <= dest_x��
    *ʱ����Դ���ε����½ǿ�ʼ�������������п�����
    *2.��������£��Դ�Դ���ε����Ͻǿ�ʼ�������������п�����
    */

    if ( (pDestDev == pSrcDev) && 
        ( (src_y < pDestRect->y1) || ( (src_y == pDestRect->y1) && 
        (src_x <= pDestRect->x1) ) ) )
    {
        first_mask= GetRightMask32(x2 & 31);
        last_mask= GetLeftMask32(x1 & 31);

        shift = (x2 & 31) - ((src_x + width - 1) & 31);

        dest_addr = (uint32)pDestDev->pdata + y2 * pDestDev->line_bytes + 
            x2 / 8;
        dest_addr = dest_addr - (x2 % 32) / 8;

        src_addr = (uint32)pSrcDev->pdata + (src_y + height - 1) * 
            pSrcDev->line_bytes + (src_x + width - 1) / 8;
        src_addr = src_addr - ((src_x + width - 1) % 32) / 8;

        step = -4;

        pitch = - pSrcDev->line_bytes;
        dst_pitch = - pDestDev->line_bytes; 
    }
    else
    {
        first_mask= GetLeftMask32(x1 & 31);
        last_mask= GetRightMask32(x2 & 31);

        shift = (x1 & 31) - (src_x & 31);

        dst_pitch = pDestDev->line_bytes; 
        dest_addr = (uint32)pDestDev->pdata + y1 * dst_pitch + x1 / 8;
        dest_addr = dest_addr - (x1 % 32) / 8;

        if ((pDestDev == pSrcDev) && (pSrcDev->mode == DEV_PHYSICAL))
        {
            pitch = pSrcDev->line_bytes;       
            src_addr = (uint32)pSrcDev->pdata + src_y * pitch + src_x / 8;
        }
        else
        {
            if (bmp_height > 0)
            {            
                pitch = -pSrcDev->line_bytes;       
                src_addr = (uint32)pSrcDev->pdata + 
                    (bmp_height - 1) * pSrcDev->line_bytes +
                    src_y * pitch + src_x / 8;       
            }
            else
            {
                pitch = pSrcDev->line_bytes;
                src_addr = (uint32)pSrcDev->pdata + src_y * pitch +
                    src_x / 8;
            }
        }
        src_addr = src_addr - (src_x % 32) / 8;

        step = 4;
    }

    //ÿɨ���е�˫����
    dwords = x2 / 32 - x1 / 32 + 1;

    // ��Դ��Ŀ�ĵĿ��������У�Դ������ʼ�����ֽ��е�λ�ã�������
    // Ŀ�ľ�����ʼ�����ֽ��е�λ�ò�ͬ����Ҫ������λ����λ��λ��
    // shift ������������ʼ�� x ���꣨�����˫����㣩�Ĳ
    //    shift = (x1 & 31) - (src_x & 31);

    //��������λ
    if (shift == 0)           
    {
        if (dwords == 1)
        {
            first_mask &= last_mask;
            last_mask= 0;

            for (y = y1; y <= y2; y++)
            {
                pSrcData = (uint8*)src_addr;
                pDestData = (uint8*)dest_addr;

                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data = dest_data & ~first_mask|
                    (*ropfunc)(src_data, dest_data) & first_mask;

                PUT32(pDestData,dest_data);

                src_addr += pitch;
                dest_addr += dst_pitch;                
            }
            return 0;
        }

        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            // �����һ���ֽ� 
            src_data  = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~first_mask|
                (*ropfunc)(src_data, dest_data) & first_mask;

            PUT32(pDestData,dest_data);

            pSrcData += step;
            pDestData += step;

            // �����м���ֽ�
            for(i = 0; i < dwords - 2; i++)
            {
                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data = (*ropfunc)(src_data, dest_data);

                PUT32(pDestData, dest_data);

                pSrcData += step;
                pDestData += step;
            }

            // �������һ���ֽ�
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~last_mask|
                (*ropfunc)(src_data, dest_data) & last_mask;

            PUT32(pDestData,dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }

        return 0;
    }

    if (shift < 0)
    {
        // ������λת����������λ
        shift = 32 + shift;
        src_addr += 4;
    }

    if (dwords == 1)
    {
        first_mask &= last_mask;

        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data = dest_data & ~first_mask|
                ropfunc(src, dest_data) & first_mask;

            PUT32(pDestData, dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }

        return 0;
    }

    // ��Դ��Ŀ�Ŀ���
    for (y = y1; y <= y2; y++)
    {
        pSrcData = (uint8*)src_addr;
        pDestData = (uint8*)dest_addr;

        // �����һ���ֽ� 
        // �����������Դ���ݸ�λ���ֵĶ�ʧ����ʧ��������
        // ����һ��˫�־�����λ�õ���
        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;
                
        dest_data = dest_data & ~first_mask|
            ropfunc(src, dest_data) & first_mask;

        PUT32(pDestData, dest_data); 

        pSrcData += step;
        pDestData += step;

        // �����м���ֽ�
        for (i = 0; i < dwords -2; i++ )
        {
            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data = ropfunc(src, dest_data);

            PUT32(pDestData, dest_data);

            pSrcData += step;
            pDestData += step;
        }

        // �������һ���ֽ�
        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data = dest_data & ~last_mask|
            ropfunc(src, dest_data) & last_mask;

        PUT32(pDestData, dest_data);

        src_addr += pitch;
        dest_addr += dst_pitch;
    }
    return 0;
}

// �����ڲ������ľ���ʵ������

static void BBF_NDST(PDEV pDestDev, int dest_x, int dest_y,
                     int width, int height)
{
    uint32   first_mask, last_mask;
    uint32   *dwAddr,dest_data;
    int   x1, x2, y1, y2, y;
    uint8*   pDestData;
    int     dwords,i;

    x1 = dest_x;
    y1 = dest_y;
    x2 = x1 + width - 1;
    y2 = y1 + height - 1;
    
    first_mask= GetLeftMask32(x1 & 31);
    last_mask= GetRightMask32(x2 & 31);
    
    dwAddr = (uint32 *)pDestDev->pdata + y1 * pDestDev->line_bytes + 
        x1 / 8 - (x1 % 32) / 8;

    dwords = x2 / 32 - x1 / 32 + 1;

    if (dwords == 1)
    {
        first_mask &= last_mask;

        for (y = y1; y <= y2; y++)
        {
            pDestData = (uint8*)dwAddr;

            dest_data = GET32(pDestData);  
            dest_data ^= first_mask;
            PUT32(pDestData, dest_data);

            dwAddr += pDestDev->line_bytes;
        }
    }
    else
    {
        for (y = y1; y <= y2; y++)
        {
            pDestData = (uint8*)dwAddr;

            dest_data = GET32(pDestData);  
            dest_data ^= first_mask;
            PUT32(pDestData, dest_data);

            pDestData += 4; 

            for(i = 0; i < dwords - 2; i++)
            {
                dest_data = GET32(pDestData);  
                dest_data ^= 0xffffffff;
                PUT32(pDestData, dest_data);

                pDestData += 4;       
            }

            dest_data = GET32(pDestData);  
            dest_data ^= last_mask;
            PUT32(pDestData, dest_data);

            dwAddr += pDestDev->line_bytes;
        }
    }
}

//���¸��������е��㷨��MonoBitBlt�����е��㷨��ͬ�������
//�μ�MonoBitBlt����

static void BBF_SRC(PDEV pDestDev, int dest_x, int dest_y, 
                    int width, int height, PDEV pSrcDev, 
                    int src_x, int src_y, int bmp_height)
{
    uint32   first_mask, last_mask;
    uint32   dest_addr, src_addr;
    uint32   dest_data, src_data, pre_src_data;
    uint8*   pDestData;
    uint8*   pSrcData;
    uint32   src;
    int32    pitch, dst_pitch;
    int     shift, i, dwords, step;
    int     x1, x2, y1, y2, y;

    if ((pDestDev == pSrcDev) && 
          (src_y == dest_y) && (src_x == dest_x))
        return;

    x1 = dest_x;
    y1 = dest_y; 
    x2 = x1 + width - 1;
    y2 = y1 + height - 1;
    
    if ((pDestDev == pSrcDev) && 
        ((src_y < dest_y) ||  (src_y == dest_y) && (src_x <= dest_x)))
    {
        first_mask= GetRightMask32(x2 & 31);
        last_mask= GetLeftMask32(x1 & 31);
    
        shift = (x2 & 31) - ((src_x + width - 1) & 31);

        dest_addr = (uint32)pDestDev->pdata + y2 * pDestDev->line_bytes + 
            x2 / 8;
        dest_addr = dest_addr - (x2 % 32) / 8;

        src_addr = (uint32)pSrcDev->pdata + (src_y + height - 1) * 
            pSrcDev->line_bytes + (src_x + width - 1) / 8;
        src_addr = src_addr - ((src_x + width - 1) % 32) / 8;

        step = -4;
        pitch = -pSrcDev->line_bytes;
        dst_pitch = -pDestDev->line_bytes; 

    }
    else
    {
        first_mask= GetLeftMask32(x1 & 31);
        last_mask= GetRightMask32(x2 & 31);

        shift = (x1 & 31) - (src_x & 31);

        dst_pitch = pDestDev->line_bytes; 
        dest_addr = (uint32)pDestDev->pdata + y1 * dst_pitch + x1 / 8;
        dest_addr = dest_addr - (x1 % 32) / 8;

        if ((pDestDev == pSrcDev) && (pSrcDev->mode == DEV_PHYSICAL))
        {
            pitch = pSrcDev->line_bytes;
            src_addr = (uint32)pSrcDev->pdata + src_y * pitch + src_x / 8;
        }
        else
        {
            if (bmp_height > 0)
            {
                pitch = -pSrcDev->line_bytes;
                src_addr = (uint32)pSrcDev->pdata + 
                    (bmp_height - 1) * pSrcDev->line_bytes +
                    src_y * pitch + src_x / 8;
            }
            else
            {
                pitch = pSrcDev->line_bytes;
                src_addr = (uint32)pSrcDev->pdata + src_y * pitch +
                    src_x / 8;
            }
        }
        src_addr = src_addr - (src_x % 32) / 8;

        step = 4;
    }

    dwords = x2 / 32 - x1 / 32 + 1;

    if (shift == 0)
    {
        if (dwords == 1)
        {
            first_mask &= last_mask;
            for (y = y1; y <= y2; y++)
            {
                pSrcData = (uint8*)src_addr;
                pDestData = (uint8*)dest_addr;

                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data = dest_data & ~first_mask| 
                    src_data & first_mask;

                PUT32(pDestData, dest_data);

                src_addr += pitch;
                dest_addr += dst_pitch;
            }
            return ;
        }

        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~first_mask| 
                src_data & first_mask;

            PUT32(pDestData, dest_data);

            pSrcData += step;
            pDestData += step;

            for (i = 0; i < dwords - 2; i++)
            {
                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data = src_data;

                PUT32(pDestData,dest_data);

                pSrcData += step;
                pDestData += step;
            }

            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~last_mask| 
                src_data & last_mask;

            PUT32(pDestData, dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }
        return ;
    }

    if (shift < 0)
    {
        shift = 32 + shift;
        src_addr += 4;
    }

    if (dwords == 1)
    {
        first_mask&= last_mask;
        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data = dest_data & ~first_mask | src & first_mask;

            PUT32(pDestData,dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }
        return;
    }

    for (y = y1; y <= y2; y++)
    {
        pSrcData = (uint8*)src_addr;
        pDestData = (uint8*)dest_addr;
        
        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data = dest_data & ~first_mask | src & first_mask;

        PUT32(pDestData,dest_data);

        pSrcData += step;
        pDestData += step;

        for (i = 0; i < dwords - 2; i++)
        {
            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data = src;

            PUT32(pDestData,dest_data);

            pSrcData += step;
            pDestData += step;
        }

        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data = dest_data & ~last_mask|
            src & last_mask;

        PUT32(pDestData,dest_data);

        src_addr += pitch;
        dest_addr += dst_pitch;
    }
}

static void BBF_NSRC(PDEV pDestDev, int dest_x, int dest_y, 
                     int width, int height, PDEV pSrcDev, 
                     int src_x, int src_y, int bmp_height)
{
    uint32   first_mask, last_mask;
    uint32   dest_addr, src_addr;
    uint32   dest_data, src_data, pre_src_data;
    uint8*   pDestData;
    uint8*   pSrcData;
    uint32   src;
    int32    pitch, dst_pitch;
    int     shift, i, dwords, step;
    int     x1, x2, y1, y2, y;

    if ((pDestDev == pSrcDev) && 
          (src_y == dest_y) && (src_x == dest_x))
        return;

    x1 = dest_x;
    y1 = dest_y; 
    x2 = x1 + width - 1;
    y2 = y1 + height - 1;
    
    if ((pDestDev == pSrcDev) && 
        ((src_y < dest_y) ||  (src_y == dest_y) && (src_x <= dest_x)))
    {
        first_mask= GetRightMask32(x2 & 31);
        last_mask= GetLeftMask32(x1 & 31);

        shift = (x2 & 31) - ((src_x + width - 1) & 31);

        dest_addr = (uint32)pDestDev->pdata + y2 * pDestDev->line_bytes + 
            x2 / 8;
        dest_addr = dest_addr - (x2 % 32) / 8;

        src_addr = (uint32)pSrcDev->pdata + (src_y + height - 1) * 
            pSrcDev->line_bytes + (src_x + width - 1) / 8;
        src_addr = src_addr - ((src_x + width - 1) % 32) / 8;

        step = -4;
        pitch = - pSrcDev->line_bytes;
        dst_pitch = - pDestDev->line_bytes; 

    }
    else
    {
        first_mask= GetLeftMask32(x1 & 31);
        last_mask= GetRightMask32(x2 & 31);

        shift = (x1 & 31) - (src_x & 31);

        dst_pitch = pDestDev->line_bytes; 
        dest_addr = (uint32)pDestDev->pdata + y1 * dst_pitch + x1 / 8;
        dest_addr = dest_addr - (x1 % 32) / 8;

        if ((pDestDev == pSrcDev) && (pSrcDev->mode == DEV_PHYSICAL))
        {
            pitch = pSrcDev->line_bytes;       
            src_addr = (uint32)pSrcDev->pdata + src_y * pitch + src_x / 8;
        }
        else
        {
            if (bmp_height > 0)
            {
                pitch = -pSrcDev->line_bytes;
                src_addr = (uint32)pSrcDev->pdata + 
                    (bmp_height - 1) * pSrcDev->line_bytes +
                    src_y * pitch + src_x / 8;
            }
            else
            {
                pitch = pSrcDev->line_bytes;
                src_addr = (uint32)pSrcDev->pdata + src_y * pitch +
                    src_x / 8;
            }
        }
        src_addr = src_addr - (src_x % 32) / 8;

        step = 4;
    }

    dwords = x2 / 32 - x1 / 32 + 1;

    if (shift == 0)
    {
        if (dwords == 1)
        {
            first_mask&= last_mask;
            for (y = y1; y <= y2; y++)
            {

                pSrcData = (uint8*)src_addr;
                pDestData = (uint8*)dest_addr;

                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data = dest_data & ~first_mask| 
                    ~src_data & first_mask;

                PUT32(pDestData, dest_data);

                src_addr += pitch;
                dest_addr += dst_pitch;
            }
            return ;
        }

        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~first_mask| 
                ~src_data & first_mask;

            PUT32(pDestData, dest_data);

            pSrcData += step;
            pDestData += step;

            for (i = 0; i < dwords - 2; i++)
            {
                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data = ~src_data;

                PUT32(pDestData,dest_data);

                pSrcData += step;
                pDestData += step;
            }

            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~last_mask| 
                ~src_data & last_mask;

            PUT32(pDestData, dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }
        return ;
    }

    if (shift < 0)
    {
        shift = 32 + shift;
        src_addr += 4;
    }

    if (dwords == 1)
    {
        first_mask&= last_mask;
        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data = dest_data & ~first_mask | ~src & first_mask;

            PUT32(pDestData,dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }
        return;
    }

    for (y = y1; y <= y2; y++)
    {
        pSrcData = (uint8*)src_addr;
        pDestData = (uint8*)dest_addr;

        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data = dest_data & ~first_mask | ~src & first_mask;

        PUT32(pDestData,dest_data);

        pSrcData += step;
        pDestData += step;

        for (i = 0; i < dwords - 2; i++)
        {
            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data = ~src;

            PUT32(pDestData,dest_data);

            pSrcData += step;
            pDestData += step;
        }

        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data = dest_data & ~last_mask|
            ~src & last_mask;

        PUT32(pDestData,dest_data);

        src_addr += pitch;
        dest_addr += dst_pitch;
    }
}

static void BBF_SRC_OR_DST(PDEV pDestDev, int dest_x, int dest_y, 
                           int width, int height, PDEV pSrcDev, 
                           int src_x, int src_y, int bmp_height)
{
    uint32   first_mask, last_mask;
    uint32   dest_addr, src_addr;
    uint32   dest_data, src_data, pre_src_data;
    uint8*   pDestData;
    uint8*   pSrcData;
    uint32   src;
    int32    pitch, dst_pitch;
    int     shift, i, dwords, step;
    int     x1, x2, y1, y2, y;

    if ((pDestDev == pSrcDev) && 
          (src_y == dest_y) && (src_x == dest_x))
        return;

    x1 = dest_x;
    y1 = dest_y; 
    x2 = x1 + width - 1;
    y2 = y1 + height - 1;

    if ((pDestDev == pSrcDev) && 
        ((src_y < dest_y) ||  (src_y == dest_y) && (src_x <= dest_x)))
    {
        first_mask= GetRightMask32(x2 & 31);
        last_mask= GetLeftMask32(x1 & 31);

        shift = (x2 & 31) - ((src_x + width - 1) & 31);
        
        dest_addr = (uint32)pDestDev->pdata + y2 * pDestDev->line_bytes + 
            x2 / 8;
        dest_addr = dest_addr - (x2 % 32) / 8;

        src_addr = (uint32)pSrcDev->pdata + (src_y + height - 1) * 
            pSrcDev->line_bytes + (src_x + width - 1) / 8;
        src_addr = src_addr - ((src_x + width - 1) % 32) / 8;

        step = -4;
        pitch = - pSrcDev->line_bytes;
        dst_pitch = - pDestDev->line_bytes; 

    }
    else
    {
        first_mask= GetLeftMask32(x1 & 31);
        last_mask= GetRightMask32(x2 & 31);

        shift = (x1 & 31) - (src_x & 31);

        dst_pitch = pDestDev->line_bytes; 
        dest_addr = (uint32)pDestDev->pdata + y1 * dst_pitch + x1 / 8;
        dest_addr = dest_addr - (x1 % 32) / 8;

        if ((pDestDev == pSrcDev) && (pSrcDev->mode == DEV_PHYSICAL))
        {
            pitch = pSrcDev->line_bytes;       
            src_addr = (uint32)pSrcDev->pdata + src_y * pitch + src_x / 8;
        }
        else
        {
            if (bmp_height > 0)
            {            
                pitch = -pSrcDev->line_bytes;       
                src_addr = (uint32)pSrcDev->pdata + 
                    (bmp_height - 1) * pSrcDev->line_bytes +
                    src_y * pitch + src_x / 8;       
            }
            else
            {
                pitch = pSrcDev->line_bytes;
                src_addr = (uint32)pSrcDev->pdata + src_y * pitch +
                    src_x / 8;
            }
        }
        src_addr = src_addr - (src_x % 32) / 8;

        step = 4;
    }

    dwords = x2 / 32 - x1 / 32 + 1;    

    if (shift == 0)
    {
        if (dwords == 1)
        {
            first_mask&= last_mask;
            for (y = y1; y <= y2; y++)
            {

                pSrcData = (uint8*)src_addr;
                pDestData = (uint8*)dest_addr;

                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data = dest_data & ~first_mask| 
                    (dest_data | src_data) & first_mask;

                PUT32(pDestData, dest_data);

                src_addr += pitch;
                dest_addr += dst_pitch;
            }
            return ;
        }

        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~first_mask| 
                (src_data | dest_data) & first_mask;

            PUT32(pDestData, dest_data);

            pSrcData += step;
            pDestData += step;

            for (i = 0; i < dwords - 2; i++)
            {
                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data |= src_data;

                PUT32(pDestData,dest_data);

                pSrcData += step;
                pDestData += step;
            }

            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~last_mask| 
                (src_data | dest_data) & last_mask;

            PUT32(pDestData, dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }
        return ;
    }

    if (shift < 0)
    {
        shift = 32 + shift;
        src_addr += 4;
    }

    if (dwords == 1)
    {
        first_mask&= last_mask;
        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data = dest_data & ~first_mask | 
                (src | dest_data) & first_mask;

            PUT32(pDestData,dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }
        return;
    }

    for (y = y1; y <= y2; y++)
    {
        pSrcData = (uint8*)src_addr;
        pDestData = (uint8*)dest_addr;

        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data = dest_data & ~first_mask | 
            (src | dest_data) & first_mask;

        PUT32(pDestData,dest_data);

        pSrcData += step;
        pDestData += step;

        for (i = 0; i < dwords - 2; i++)
        {
            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data |= src;

            PUT32(pDestData,dest_data);

            pSrcData += step;
            pDestData += step;
        }

        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data = dest_data & ~last_mask|
            (src | dest_data) & last_mask;

        PUT32(pDestData,dest_data);

        src_addr += pitch;
        dest_addr += dst_pitch;
    }
}


static void BBF_NSRC_AND_DST(PDEV pDestDev, int dest_x, int dest_y, 
                           int width, int height, PDEV pSrcDev, 
                           int src_x, int src_y, int bmp_height)
{
    uint32   first_mask, last_mask;
    uint32   dest_addr, src_addr;
    uint32   dest_data, src_data, pre_src_data;
    uint8*   pDestData;
    uint8*   pSrcData;
    uint32   src;
    int32    pitch, dst_pitch;
    int     shift, i, dwords, step;
    int     x1, x2, y1, y2, y;

    if ((pDestDev == pSrcDev) && 
          (src_y == dest_y) && (src_x == dest_x))
        return;

    x1 = dest_x;
    y1 = dest_y; 
    x2 = x1 + width - 1;
    y2 = y1 + height - 1;

    if ((pDestDev == pSrcDev) && 
        ((src_y < dest_y) ||  (src_y == dest_y) && (src_x <= dest_x)))
    {
        first_mask= GetRightMask32(x2 & 31);
        last_mask= GetLeftMask32(x1 & 31);

        shift = (x2 & 31) - ((src_x + width - 1) & 31);

        dest_addr = (uint32)pDestDev->pdata + y2 * pDestDev->line_bytes + 
            x2 / 8;
        dest_addr = dest_addr - (x2 % 32) / 8;

        src_addr = (uint32)pSrcDev->pdata + (src_y + height - 1) * 
            pSrcDev->line_bytes + (src_x + width - 1) / 8;
        src_addr = src_addr - ((src_x + width - 1) % 32) / 8;

        step = -4;
        pitch = - pSrcDev->line_bytes;
        dst_pitch = - pDestDev->line_bytes; 

    }
    else
    {
        first_mask= GetLeftMask32(x1 & 31);
        last_mask= GetRightMask32(x2 & 31);

        shift = (x1 & 31) - (src_x & 31);

        dst_pitch = pDestDev->line_bytes; 
        dest_addr = (uint32)pDestDev->pdata + y1 * dst_pitch + x1 / 8;
        dest_addr = dest_addr - (x1 % 32) / 8;

        if ((pDestDev == pSrcDev) && (pSrcDev->mode == DEV_PHYSICAL))
        {
            pitch = pSrcDev->line_bytes;       
            src_addr = (uint32)pSrcDev->pdata + src_y * pitch + src_x / 8;
        }
        else
        {
            if (bmp_height > 0)
            {
                pitch = -pSrcDev->line_bytes;
                src_addr = (uint32)pSrcDev->pdata + 
                    (bmp_height - 1) * pSrcDev->line_bytes +
                    src_y * pitch + src_x / 8;       
            }
            else
            {
                pitch = pSrcDev->line_bytes;
                src_addr = (uint32)pSrcDev->pdata + src_y * pitch +
                    src_x / 8;
            }
        }
        src_addr = src_addr - (src_x % 32) / 8;

        step = 4;
    }
    
    dwords = x2 / 32 - x1 / 32 + 1;    

    if (shift == 0)
    {
        if (dwords == 1)
        {
            first_mask &= last_mask;
            for (y = y1; y <= y2; y++)
            {

                pSrcData = (uint8*)src_addr;
                pDestData = (uint8*)dest_addr;

                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data = dest_data & ~first_mask| 
                    (dest_data & (~src_data)) & first_mask;

                PUT32(pDestData, dest_data);

                src_addr += pitch;
                dest_addr += dst_pitch;
            }
            return ;
        }

        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~first_mask| 
                (dest_data & (~src_data)) & first_mask;

            PUT32(pDestData, dest_data);

            pSrcData += step;
            pDestData += step;

            for (i = 0; i < dwords - 2; i++)
            {
                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data &= ~src_data;

                PUT32(pDestData,dest_data);

                pSrcData += step;
                pDestData += step;
            }

            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~first_mask| 
                (dest_data & (~src_data)) & first_mask;

            PUT32(pDestData, dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }
        return ;
    }

    if (shift < 0)
    {
        shift = 32 + shift;
        src_addr += 4;
    }

    if (dwords == 1)
    {
        first_mask &= last_mask;
        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data = dest_data & ~first_mask | 
                (dest_data & (~src)) & first_mask;

            PUT32(pDestData,dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }
        return;
    }

    for (y = y1; y <= y2; y++)
    {
        pSrcData = (uint8*)src_addr;
        pDestData = (uint8*)dest_addr;

        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data = dest_data & ~first_mask | 
            (dest_data & (~src)) & first_mask;

        PUT32(pDestData,dest_data);

        pSrcData += step;
        pDestData += step;

        for (i = 0; i < dwords - 2; i++)
        {
            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data &= ~src;

            PUT32(pDestData,dest_data);

            pSrcData += step;
            pDestData += step;
        }

        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data &= dest_data & ~last_mask|
            (dest_data & (~src)) & last_mask;

        PUT32(pDestData,dest_data);

        src_addr += pitch;
        dest_addr += dst_pitch;
    }
}


static void BBF_SRC_AND_DST(PDEV pDestDev, int dest_x, int dest_y, 
                           int width, int height, PDEV pSrcDev, 
                           int src_x, int src_y, int bmp_height)
{
    uint32   first_mask, last_mask;
    uint32   dest_addr, src_addr;
    uint32   dest_data, src_data, pre_src_data;
    uint8*   pDestData;
    uint8*   pSrcData;
    uint32   src;
    int32    pitch, dst_pitch;
    int     shift, i, dwords, step;
    int     x1, x2, y1, y2, y;

    if ((pDestDev == pSrcDev) && 
          (src_y == dest_y) && (src_x == dest_x))
        return;

    x1 = dest_x;
    y1 = dest_y; 
    x2 = x1 + width - 1;
    y2 = y1 + height - 1;
    
    if ((pDestDev == pSrcDev) && 
        ((src_y < dest_y) ||  (src_y == dest_y) && (src_x <= dest_x)))
    {
        first_mask= GetRightMask32(x2 & 31);
        last_mask= GetLeftMask32(x1 & 31);

        shift = (x2 & 31) - ((src_x + width - 1) & 31);

        dest_addr = (uint32)pDestDev->pdata + y2 * pDestDev->line_bytes + 
            x2 / 8;
        dest_addr = dest_addr - (x2 % 32) / 8;

        src_addr = (uint32)pSrcDev->pdata + (src_y + height - 1) * 
            pSrcDev->line_bytes + (src_x + width - 1) / 8;
        src_addr = src_addr - ((src_x + width - 1) % 32) / 8;

        step = -4;
        pitch = - pSrcDev->line_bytes;
        dst_pitch = - pDestDev->line_bytes; 

    }
    else
    {
        first_mask= GetLeftMask32(x1 & 31);
        last_mask= GetRightMask32(x2 & 31);

        shift = (x1 & 31) - (src_x & 31);

        dst_pitch = pDestDev->line_bytes; 
        dest_addr = (uint32)pDestDev->pdata + y1 * dst_pitch + x1 / 8;
        dest_addr = dest_addr - (x1 % 32) / 8;

        if ((pDestDev == pSrcDev) && (pSrcDev->mode == DEV_PHYSICAL))
        {
            pitch = pSrcDev->line_bytes;       
            src_addr = (uint32)pSrcDev->pdata + src_y * pitch + src_x / 8;
        }
        else
        {
            if (bmp_height > 0)
            {
                pitch = -pSrcDev->line_bytes;
                src_addr = (uint32)pSrcDev->pdata + 
                    (bmp_height - 1) * pSrcDev->line_bytes +
                    src_y * pitch + src_x / 8;
            }
            else
            {
                pitch = pSrcDev->line_bytes;
                src_addr = (uint32)pSrcDev->pdata + src_y * pitch +
                    src_x / 8;
            }
        }
        src_addr = src_addr - (src_x % 32) / 8;

        step = 4;
    }

    dwords = x2 / 32 - x1 / 32 + 1;    

    if (shift == 0)
    {
        if (dwords == 1)
        {
            first_mask &= last_mask;
            for (y = y1; y <= y2; y++)
            {
                pSrcData = (uint8*)src_addr;
                pDestData = (uint8*)dest_addr;

                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data = dest_data & ~first_mask| 
                    (dest_data & src_data) & first_mask;

                PUT32(pDestData, dest_data);

                src_addr += pitch;
                dest_addr += dst_pitch;
            }
            return ;
        }

        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~first_mask| 
                (dest_data & src_data) & first_mask;

            PUT32(pDestData, dest_data);

            pSrcData += step;
            pDestData += step;

            for (i = 0; i < dwords - 2; i++)
            {
                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data &= src_data;

                PUT32(pDestData,dest_data);

                pSrcData += step;
                pDestData += step;
            }

            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~last_mask| 
                (dest_data & src_data) & last_mask;

            PUT32(pDestData, dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }
        return ;
    }

    if (shift < 0)
    {
        shift = 32 + shift;
        src_addr += 4;
    }

    if (dwords == 1)
    {
        first_mask &= last_mask;
        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data = dest_data & ~first_mask | 
                (dest_data & src) & first_mask;

            PUT32(pDestData,dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }
        return;
    }

    for (y = y1; y <= y2; y++)
    {
        pSrcData = (uint8*)src_addr;
        pDestData = (uint8*)dest_addr;

        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data = dest_data & ~first_mask | 
            (dest_data & src) & first_mask;

        PUT32(pDestData,dest_data);

        pSrcData += step;
        pDestData += step;

        for (i = 0; i < dwords - 2; i++)
        {
            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data &= src;

            PUT32(pDestData,dest_data);

            pSrcData += step;
            pDestData += step;
        }

        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data = dest_data & ~last_mask|
            (dest_data & src) & last_mask;

        PUT32(pDestData,dest_data);

        src_addr += pitch;
        dest_addr += dst_pitch;
    }
}

static void BBF_SRC_XOR_DST(PDEV pDestDev, int dest_x, int dest_y, 
                           int width, int height, PDEV pSrcDev, 
                           int src_x, int src_y, int bmp_height)
{
    uint32   first_mask, last_mask;
    uint32   dest_addr, src_addr;
    uint32   dest_data, src_data, pre_src_data;
    uint8*   pDestData;
    uint8*   pSrcData;
    uint32   src;
    int32    pitch, dst_pitch;
    int     shift, i, dwords, step;
    int     x1, x2, y1, y2, y;

    if ((pDestDev == pSrcDev) && 
          (src_y == dest_y) && (src_x == dest_x))
        return;

    x1 = dest_x;
    y1 = dest_y; 
    x2 = x1 + width - 1;
    y2 = y1 + height - 1;

    if ((pDestDev == pSrcDev) && 
        ((src_y < dest_y) ||  (src_y == dest_y) && (src_x <= dest_x)))
    {
        first_mask= GetRightMask32(x2 & 31);
        last_mask= GetLeftMask32(x1 & 31);

        shift = (x2 & 31) - ((src_x + width - 1) & 31);

        dest_addr = (uint32)pDestDev->pdata + y2 * pDestDev->line_bytes + 
            x2 / 8;
        dest_addr = dest_addr - (x2 % 32) / 8;

        src_addr = (uint32)pSrcDev->pdata + (src_y + height - 1) * 
            pSrcDev->line_bytes + (src_x + width - 1) / 8;
        src_addr = src_addr - ((src_x + width - 1) % 32) / 8;

        step = -4;
        pitch = - pSrcDev->line_bytes;
        dst_pitch = - pDestDev->line_bytes; 
    }
    else
    {
        first_mask= GetLeftMask32(x1 & 31);
        last_mask= GetRightMask32(x2 & 31);

        shift = (x1 & 31) - (src_x & 31);

        dst_pitch = pDestDev->line_bytes; 
        dest_addr = (uint32)pDestDev->pdata + y1 * dst_pitch + x1 / 8;
        dest_addr = dest_addr - (x1 % 32) / 8;

        if ((pDestDev == pSrcDev) && (pSrcDev->mode == DEV_PHYSICAL))
        {
            pitch = pSrcDev->line_bytes;       
            src_addr = (uint32)pSrcDev->pdata + src_y * pitch + src_x / 8;
        }
        else
        {
            if (bmp_height > 0)
            {            
                pitch = -pSrcDev->line_bytes;       
                src_addr = (uint32)pSrcDev->pdata + 
                    (bmp_height - 1) * pSrcDev->line_bytes +
                    src_y * pitch + src_x / 8;       
            }
            else
            {
                pitch = pSrcDev->line_bytes;
                src_addr = (uint32)pSrcDev->pdata + src_y * pitch +
                    src_x / 8;
            }
        }
        src_addr = src_addr - (src_x % 32) / 8;

        step = 4;
    }

    dwords = x2 / 32 - x1 / 32 + 1;    

    if (shift == 0)
    {
        if (dwords == 1)
        {
            first_mask &= last_mask;
            for (y = y1; y <= y2; y++)
            {

                pSrcData = (uint8*)src_addr;
                pDestData = (uint8*)dest_addr;

                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data = dest_data & ~first_mask| 
                    (src_data ^ dest_data) & first_mask;

                PUT32(pDestData, dest_data);

                src_addr += pitch;
                dest_addr += dst_pitch;
            }
            return ;
        }

        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~first_mask| 
                (src_data ^ dest_data) & first_mask;

            PUT32(pDestData, dest_data);

            pSrcData += step;
            pDestData += step;

            for (i = 0; i < dwords - 2; i++)
            {
                src_data = GET32(pSrcData);
                dest_data = GET32(pDestData);

                dest_data ^= src_data;

                PUT32(pDestData,dest_data);

                pSrcData += step;
                pDestData += step;
            }

            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~last_mask| 
                (src_data ^ dest_data) & last_mask;

            PUT32(pDestData, dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }
        return ;
    }

    if (shift < 0)
    {
        shift = 32 + shift;
        src_addr += 4;
    }

    if (dwords == 1)
    {
        first_mask &= last_mask;
        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data = dest_data & ~first_mask | 
                (src ^ dest_data) & first_mask;

            PUT32(pDestData,dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }
        return;
    }

    for (y = y1; y <= y2; y++)
    {
        pSrcData = (uint8*)src_addr;
        pDestData = (uint8*)dest_addr;

        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data = dest_data & ~first_mask | 
            (src ^ dest_data) & first_mask;

        PUT32(pDestData,dest_data);

        pSrcData += step;
        pDestData += step;

        for (i = 0; i < dwords - 2; i++)
        {
            pre_src_data = GET32(pSrcData - 4);
            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            src_data = src_data >> shift;
            pre_src_data = pre_src_data << (32 - shift);
            src = pre_src_data | src_data;

            dest_data ^= src;

            PUT32(pDestData,dest_data);

            pSrcData += step;
            pDestData += step;
        }

        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data = dest_data & ~last_mask|
            (src ^ dest_data) & last_mask;

        PUT32(pDestData,dest_data);

        src_addr += pitch;
        dest_addr += dst_pitch;
    }
}

#if 0
static int SpecialMono(PDEV pDestDev, int dst_x, int dst_y, 
            int width, int height, PDEV pSrcDev, int src_x, 
            int src_y, uint32 rop, int bk_mode)
{
    uint32   first_mask, last_mask;
    uint32   dest_addr, src_addr; 
    uint32   src,src_data,pre_src_data;
    uint32   dest_data;
    uint32   pitch, dst_pitch;
    uint32   x1, x2, y1, y2, y;
    uint8*   pDestData;
    uint8*   pSrcData;
    int      shift;
    ROPFUNC  ropfunc;

    /*    switch (rop)
    {     
        case ROP_NDST:
            SPECIAL_DST(pDestDev, dst_x, dst_y, width, height);
            return 1;

        case ROP_SRC:
            SPECIAL_SRC(pDestDev, dst_x, dst_y, width, height, 
                pSrcDev, src_x, src_y);
            return 1;

        case ROP_NSRC :
            SPECIAL_NSRC(pDestDev, dst_x, dst_y, width, height, 
                pSrcDev, src_x, src_y);
            return 1;
        
        //�������������������ַ�����
        case ROP_SRC_OR_DST:                
            // ��ʾģʽΪ͸��, ǰ��ɫ�а�ɫ��
            SPECIAL_SRC_OR_DST(pDestDev, dst_x, dst_y, width, height,
                pSrcDev, src_x, src_y);
            return 1;
        
        case ROP_NSRC_AND_DST:              
            //��ʾģʽΪ͸��, ǰ��ɫ�к�ɫ��
            SPECIAL_NSRC_AND_DST(pDestDev, dst_x, dst_y, width, height,
                pSrcDev, src_x, src_y);
            return 1;

        case ROP_SRC_AND_DST:               
            SPECIAL_SRC_AND_DST(pDestDev, dst_x, dst_y, width, height, 
                pSrcDev, src_x, src_y);        
            return 1;

        case ROP_SRC_XOR_DST:            
            SPECIAL_SRC_XOR_DST(pDestDev, dst_x, dst_y, width, height, 
                pSrcDev, src_x, src_y);
            return 1;
            
        default:
            break;
    }
    */    
    
    // Compute clip area
    x1 = dst_x; 
    y1 = dst_y; 
    y2 = y1 + height - 1; 
    x2 = x1 + width - 1;

    ropfunc = GetRopFunc(rop);

    /*���Ŀ���豸��Դ�豸��ͬһ���豸��Դ������Ŀ�ľ��β����غ�ʱ����Դ
    *��Ŀ�ĵĿ�����Ҫ��������ۣ�
    *1.��src_y < dest_y ʱ�����ߣ�����src_y == dest_y && src_x <= dest_x��
    *ʱ����Դ���ε����½ǿ�ʼ�������������п�����
    *2.��������£��Դ�Դ���ε����Ͻǿ�ʼ�������������п�����
    */

    pDestDev->line_bytes = (pDestDev->width + 15) / 16 * 2;

    if ( (pDestDev == pSrcDev) && 
        ( (src_y < dst_y) || ((src_y == dst_y) && (src_x <= dst_x)) ) )
    {    
        dest_addr = (uint32)pDestDev->pdata + y2 * pDestDev->line_bytes + 
            x1 / 8;
        dest_addr = dest_addr - (x1 % 16) / 8;

        src_addr = (uint32)pSrcDev->pdata + (src_y + height - 1) * 
            pSrcDev->line_bytes + src_x / 8;
        src_addr = src_addr - (src_x % 32) / 8;

        pitch = - pSrcDev->line_bytes;
        dst_pitch = - pDestDev->line_bytes; 
    }
    else
    {
        dst_pitch = pDestDev->line_bytes; 
        dest_addr = (uint32)pDestDev->pdata + y1 * dst_pitch + x1 / 8;
        dest_addr = dest_addr - (x1 % 16) / 8;

        if ((pSrcDev->mode == DEV_MEMORY) && (pSrcDev->height > 0))
        {
            pitch = pSrcDev->line_bytes;       
            src_addr = (uint32)pSrcDev->pdata + src_y * pitch + src_x / 8;
        }
        else
        {
            pitch = -pSrcDev->line_bytes;       
            src_addr =
                (uint32)pSrcDev->pdata + (pSrcDev->height - 1 + src_y)
                * pitch + src_x / 8;
        }
        src_addr = src_addr - (src_x % 32) / 8;
    }

    // ��Դ��Ŀ�ĵĿ��������У�Դ������ʼ�����ֽ��е�λ�ã�������
    // Ŀ�ľ�����ʼ�����ֽ��е�λ�ò�ͬ����Ҫ������λ����λ��λ��
    // shift ������������ʼ�� x ���꣨�����˫����㣩�Ĳ
    shift = ((x1 & 15) & 31) - (src_x & 31);

    first_mask= GetLeftMask32((x1 & 15) & 31);
    last_mask= GetRightMask32((x2 - (x1 / 16) * 16) & 31);

    first_mask &= last_mask;
    last_mask= 0;

    //��������λ
    if (shift == 0)
    {
        for (y = y1; y <= y2; y++)
        {
            pSrcData = (uint8*)src_addr;
            pDestData = (uint8*)dest_addr;

            src_data = GET32(pSrcData);
            dest_data = GET32(pDestData);

            dest_data = dest_data & ~first_mask|
                (*ropfunc)(src_data, dest_data) & first_mask;

            PUT32(pDestData,dest_data);

            src_addr += pitch;
            dest_addr += dst_pitch;
        }
        return 0;
    }

    if (shift < 0)
    {
        // ������λת����������λ
        shift = 32 + shift;
        src_addr += 4;
    }

    for (y = y1; y <= y2; y++)
    {
        pSrcData = (uint8*)src_addr;
        pDestData = (uint8*)dest_addr;

        pre_src_data = GET32(pSrcData - 4);
        src_data = GET32(pSrcData);
        dest_data = GET32(pDestData);

        src_data = src_data >> shift;
        pre_src_data = pre_src_data << (32 - shift);
        src = pre_src_data | src_data;

        dest_data = dest_data & ~first_mask|
            ropfunc(src, dest_data) & first_mask;

        PUT32(pDestData, dest_data);

        src_addr += pitch;
        dest_addr += dst_pitch;
    }

    return 0;
}
#endif
