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
* Function	   SaveScreen
* Purpose      Save area of screen to memory structure.
* Params	   
    pDev        Pointer to physical device structure.
    pScreenSave Pointer to screen save structure.
    pRect       Pointer to rectangle.
* Return	 	   
* Remarks	   
**********************************************************************/
static int SaveScreen(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect)
{
	return -1;
}

/*********************************************************************\
* Function	   RestoreScreen
* Purpose      Restore screen save area.
* Params	   
    pDev        Pointer to physical device structure.
    pScreenSave Pointer to screen save structure.
    pRect       Pointer to rectangle.
* Return	 	   
* Remarks	   
**********************************************************************/
static int RestoreScreen(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect)
{
	return -1;
}

/*********************************************************************\
* Function	   ScrollScreen
* Purpose     该操作将屏幕上由pRect指定的矩形区域的内容移动到以x、y为左
              上角的同样大小的矩形区域内。 
* Params	   
    pDev      指向物理显示设备结构的指针
    pRect     指向待滚动的的矩形结构的指针
    x、y      滚动后的左上角位置
* Return	 	   
* Remarks	   
    GUI在调用时保证源区域和目标区域都不超过屏幕的界限。显示设备驱动程序
    应正确处理源区域与目标区域有覆盖的情形，保证滚动操作在任何方向的正确
    (这一功能由BitBlt函数完成)。
**********************************************************************/
static int ScrollScreen(PDEV pDev, PRECT pRect, PRECT pClipRect,
                          int x, int y)
{
    RECT src_rc, dest_rc;
    int  dx, dy, srcx, srcy;
    int  width, height;
    
    if (pDev->mode != DEV_PHYSICAL || pRect == NULL)
        return 0;
        
    srcx = pRect->x1;
    srcy = pRect->y1;
    dx = x - srcx;
    dy = y - srcy;

    /* Clip source rectangle to fit in device */
    CopyRect(&src_rc, pRect);
    if (pClipRect != NULL)
        ClipRect(&src_rc, pClipRect);
    ClipByDev(&src_rc, pDev);
    
    /* If left or top cliped, adjust destination x and y */
    width  = src_rc.x2 - src_rc.x1;
    height = src_rc.y2 - src_rc.y1;
    x += src_rc.x1 - srcx;
    y += src_rc.y1 - srcy;

    dest_rc.x1 = x;
    dest_rc.y1 = y;
    dest_rc.x2 = x + width;
    dest_rc.y2 = y + height;
    /* Clip destination rectangle to fit in device */
    if (pClipRect != NULL)
        ClipRect(&dest_rc, pClipRect);
    ClipByDev(&dest_rc, pDev);
    
    /* If destination is cliped out, noting to do */
    if (!IsValidRect(&dest_rc))
        return 0;

    /* Check if left side cliped out */
    if (dx < 0)
        src_rc.x1 = dest_rc.x1 - dx;
    else
        src_rc.x2 = dest_rc.x2 - dx;
        
    if (dy < 0)
        src_rc.y1 = dest_rc.y1 - dy;
    else
        src_rc.y2 = dest_rc.y2 - dy;

    /* If destination is cliped out, noting to do */
    if (!IsValidRect(&src_rc))
        return 0;

    {
        GRAPHPARAM graphparam;

        graphparam.flags = 0;
        graphparam.bk_mode = BM_OPAQUE;
        graphparam.bk_color = 0;   
        graphparam.text_color = 0; 
        graphparam.pPen = 0;
        graphparam.pBrush = 0;
        graphparam.brush_org_x = 0;
        graphparam.brush_org_y = 0;
        graphparam.rop = ROP_SRC;
        graphparam.clip_rect.x1 = 0;
        graphparam.clip_rect.y1 = 0;
        graphparam.clip_rect.x2 = 0;
        graphparam.clip_rect.y2 = 0;
        graphparam.pRegion = NULL;

        BitBlt(pDev, &dest_rc, pDev, src_rc.x1, src_rc.y1, &graphparam);
    }
    
    return 0;
}
