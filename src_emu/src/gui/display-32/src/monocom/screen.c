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
 *  Save area of screen to memory structure.
 *
 *  INPUT:
 *  pDev        Pointer to physical device structure.
 *  pScreenSave Pointer to screen save structure.
 *  pRect       Pointer to rectangle.
 *
\**************************************************************************/

static int SaveScreen(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect)
{
    int len, scanw, w, h;

    if (pDev->mode != DEV_PHYSICAL || pRect == NULL)
        return -1;

    ClipByDev(pRect, pDev);
    if (!IsValidRect(pRect))
        return -1;

    w = pRect->x2 - pRect->x1;
    h = pRect->y2 - pRect->y1;

    scanw = (w + 31) / 32 + 1;

    len = sizeof(SCREENSAVE) + scanw * h;

    return len;
}

/**************************************************************************\
 *
 *  Restore screen save area.
 *
 *  INPUT:
 *  pDev        Pointer to physical device structure.
 *  pScreenSave Pointer to screen save structure.
 *  pRect       Pointer to rectangle.
 *
\**************************************************************************/

static int RestoreScreen(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect)
{
    return 0;
}

/**************************************************************************\
* Function: ScrollScreen
* Input:
*     pDev      ָ��������ʾ�豸�ṹ��ָ��
*     pRect     ָ��������ĵľ��νṹ��ָ��
*     x��y      ����������Ͻ�λ��
* ��ϸ˵��:
*     �ò�������Ļ����pRectָ���ľ�������������ƶ�����x��yΪ���Ͻǵ�ͬ��
*     ��С�ľ��������ڡ�GUI�ڵ���ʱ��֤Դ�����Ŀ�����򶼲�������Ļ�Ľ��ޡ�
*     ��ʾ�豸��������Ӧ��ȷ����Դ������Ŀ�������и��ǵ����Σ���֤��������
*     ���κη������ȷ(��һ������BitBlt�������)��
\**************************************************************************/
static int ScrollScreen(PDEV pDev, PRECT pRect, PRECT pClipRect, int x, 
                        int y)
{
    RECT    dst_rc, src_rect, rect;
    GRAPHPARAM  gp;

    dst_rc.x1 = x;
    dst_rc.y1 = y;
    dst_rc.x2 = x + (pRect->x2 - pRect->x1);
    dst_rc.y2 = y + (pRect->y2 - pRect->y1);

    gp.flags        = GPF_ROP2 | GPF_CLIP_RECT;
    gp.bk_mode      = BM_OPAQUE;
    gp.bk_color     = 1;
    gp.text_color   = 0;
    gp.pPen         = NULL;
    gp.pBrush       = NULL;
    gp.brush_org_x  = 0;
    gp.brush_org_y  = 0;
    gp.rop          = ROP_SRC;
    gp.pRegion      = NULL;

    if (pClipRect != NULL)
        gp.clip_rect = *pClipRect;
    else
    {
        gp.clip_rect.x1 = 0;
        gp.clip_rect.x2 = pDev->width;
        gp.clip_rect.y1 = 0;
        gp.clip_rect.y2 = pDev->height;
    }

    CopyRect(&rect, &dst_rc);
    CopyRect(&src_rect, pRect);

    /* ���ü�����*/
    ClipByDev(&rect, pDev);
    if (!ClipRect(&rect, &(gp.clip_rect)))
        return 0;

    _ExcludeCursor(pDev, &rect);    
    ClipByDev(&src_rect, pDev);

    //����Ŀ�ľ��Ρ�Դ���� 
    rect.x1 -= dst_rc.x1;
    rect.y1 -= dst_rc.y1;
    rect.x2 -= dst_rc.x1;
    rect.y2 -= dst_rc.y1;
    
    src_rect.x1 -= pRect->x1;
    src_rect.y1 -= pRect->y1;
    src_rect.x2 -= pRect->x1;
    src_rect.y2 -= pRect->y1;
    
    //���Ŀ�ĺ�Դ���εĽ���
    if (!ClipRect(&rect, &src_rect))
        return -1;
    
    src_rect.x1 = rect.x1;
    src_rect.y1 = rect.y1;
    
    src_rect.x1 += pRect->x1;
    src_rect.y1 += pRect->y1;
    
    rect.x1 += dst_rc.x1;
    rect.y1 += dst_rc.y1;
    rect.x2 += dst_rc.x1;
    rect.y2 += dst_rc.y1;

    MonoBitBlt(pDev, &rect, pDev, src_rect.x1, src_rect.y1, 
        gp.rop, gp.bk_mode, -pDev->height);

    _UnexcludeCursor(pDev);
    return 0;
}
