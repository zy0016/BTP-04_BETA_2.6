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
* Function	ClipLine   
* Purpose  : Clip line and get LINEDRAW struct, in LINEDRAW struct, the
*            start_x and end_x have not been exchanged. It's different 
*            with the old version.
*            06/20/2002
* Params
    pLineData   Pointer to the origin line data.
    pClipRect   Pointer to the clip rect.
    pLineDraw	Pointer to the cliped line data.   
* Return
    LINE_NONE	    0   No line to be drawn
    LINE_HORIZONTAL 1   Horizontal line
    LINE_VERTICAL   2   Vertical line
    LINE_CTRLX      3   control variable is X
    LINE_CTRLY      4   control variable is Y
* Remarks	   
**********************************************************************/
static int ClipLine(PLINEDATA pLineData, PRECT pClipRect, 
                    LINEDRAW* pLineDraw)
{
    int x1, x2, y1, y2;
    int tx1, tx2, ty1, ty2;     /* The clip result of slope line        */
    int dx, dy, twodx, twody;   /* Used to calculate the clip result    */
    int reverse = 0;            /* Flag the coordinates exchanged       */
    
    x1 = pLineData->x1;
    x2 = pLineData->x2;
    y1 = pLineData->y1;
    y2 = pLineData->y2;
    
    /* Horizontal line */
    if (y1 == y2)
    {
        /* Out of clip rectangle */
        if (y1 < pClipRect->y1 || y1 >= pClipRect->y2)
            return 0;   
        
        if (x1 < x2)
            x2--;
        else
        {
            x2++;
            EXCHANGE(x1, x2);
            reverse = 1;
        }
                            
        x1 = MAX(x1, pClipRect->x1);
        x2 = MIN(x2, pClipRect->x2 - 1);        
        if (x1 > x2)
            return 0;

        /* The LINEDRAW struct store the line that has been cliped with the 
        ** origin direction. And the cliped data item is used to store the 
        ** number of cliped points from the start point.
        ** So the DrawLine function can get all information of the origin 
        ** line and can draw the part of line as same as the origin part.
        */
        if (reverse)
        {
            pLineDraw->start_x  = x2;
            pLineDraw->start_y  = y1;
            pLineDraw->end_x    = x1;
            pLineDraw->end_y    = y2;
            pLineDraw->cliped   = pLineData->x1 - x2;
        }
        else
        {
            pLineDraw->start_x  = x1;
            pLineDraw->start_y  = y1;
            pLineDraw->end_x    = x2;
            pLineDraw->end_y    = y2;
            pLineDraw->cliped   = x1 - pLineData->x1;
        }

        return LINE_HORIZONTAL;
    }
    
    /* Vertical line */
    if (x1 == x2)
    {
        /* Out of clip rectangle */
        if (x1 < pClipRect->x1 || x1 >= pClipRect->x2)
            return 0;   
        
        if (y1 < y2)
            y2--;
        else
        {
            y2++;
            EXCHANGE(y1, y2);
            reverse = 1;
        }
        
        y1 = MAX(y1, pClipRect->y1);
        y2 = MIN(y2, pClipRect->y2 - 1);        
        if (y1 > y2)
            return 0;

        if (reverse)
        {
            pLineDraw->start_x  = x1;
            pLineDraw->start_y  = y2;
            pLineDraw->end_x    = x2;
            pLineDraw->end_y    = y1;
            pLineDraw->cliped   = pLineData->y1 - y2;
        }
        else
        {
            pLineDraw->start_x  = x1;
            pLineDraw->start_y  = y1;
            pLineDraw->end_x    = x2;
            pLineDraw->end_y    = y2;
            pLineDraw->cliped   = y1 - pLineData->y1;
        }

        return LINE_VERTICAL;
    }

    dx  = ABS(x1 - x2);
    dy  = ABS(y1 - y2);
    twodx = dx * 2;
    twody = dy * 2;
    if (dx >= dy)
    {
        /* the delta data item store the increase that is dependent on the 
        ** step data item. the step data item is the independent variable 
        ** and increase one by one.
        ** the line drawing is controled by the step.
        */
        pLineDraw->delta     = dy;
        pLineDraw->step      = dx;

        /* dir data item is used to determine the direction of the line and 
        ** the sign of linebytes or pixelbytes of the display device.
        */
        if (y1 > y2)
            pLineDraw->dir = REVR_DIR;
        else
            pLineDraw->dir = NORM_DIR;

        if (x1 > x2)
        {
            EXCHANGE(x1, x2);
            EXCHANGE(y1, y2);
            reverse = 1;
        }

        if (x1 >= pClipRect->x2 || x2 < pClipRect->x1)
            return 0;

        /* 
        ** Compute the x coordinate for given y:
        ** DIV_CEILING(a, b) (((a) * 2 + (b)) / ((b) * 2))
        **       dx
        **  x =---- * (y - y1) + x1
        **       dy
        **  x = x1 + (y - y1) * dx / dy
        **    = x1 + ((y - y1) * dx * 2 + dy) / (dy * 2)
        **    = x1 + DIV_CEILING((y - y1) * dx, dy)
        **
        **  y = y1 + DIV_CEILING((x - x1) * dy, dx)
		*/

        tx1 = x1;
        tx2 = x2;
        ty1 = y1;
        ty2 = y2;
        
        if (y1 < y2)    /* "\" type line*/
        {
            if (y1 >= pClipRect->y2 || y2 < pClipRect->y1)
                return 0;

            if (ty1 < pClipRect->y1)
            {
                ty1 = pClipRect->y1;
                if (reverse)
                {
                    /* Calculate the tx1 that decided by the ty1. The result
                    ** tx1 is most close to the exactness. But there is a 
                    ** problem that sometimes draw the line point ,usint DDA
                    ** method, with x = tx1 + 1, the result y is also equal 
                    ** to ty1.
                    ** So we should recalculate the tx1. The correct tx1  
                    ** must be a critical value that meet follow conditions.
                    **
                    ** When x = tx1, y = ty1
                    ** When x = tx1 - 1, y = ty1 - 1.
                    */
                    tx1 = x2 - DIV_CEILING((y2 - ty1) * dx, dy);
                    tx1 -= ((twodx - ((x2 - tx1) * twody + dx) % twodx) + 
                        twody - 1) / twody - 1;
                }
                else
                {
                    tx1 = x1 + DIV_CEILING((ty1 - y1) * dx, dy);
                    /*
                    ** When x = tx1, y = ty1
                    ** When x = tx1 - 1, y = ty1 - 1.
                    */
                    tx1 -= (((tx1 - x1) * twody + dx) % twodx) / twody;
                }

                if (tx1 > tx2 || tx1 >= pClipRect->x2)
                    return 0;
            }

            if (tx1 < pClipRect->x1)
            {
                tx1 = pClipRect->x1;
                ty1 = y1 + DIV_CEILING((tx1 - x1) * dy, dx);

                if (tx1 > tx2 || ty1 >= pClipRect->y2)
                    return 0;
            }

            if (ty2 >= pClipRect->y2)
            {
                ty2 = pClipRect->y2 - 1;
                if (reverse)
                {
                    /*
                    ** When x = tx2, y = ty2
                    ** When x = tx2 + 1, y = ty2 + 1.
                    */
                    tx2 = x2 - DIV_CEILING((y2 - ty2) * dx, dy);
                    tx2 += (((x2 - tx2) * twody + dx) % twodx) / twody;
                }
                else
                {
                    /*
                    ** When x = tx2, y = ty2
                    ** When x = tx2 + 1, y = ty2 + 1.
                    */
                    tx2 = x1 + DIV_CEILING((ty2 - y1) * dx, dy);
                    tx2 += ((twodx - ((tx2 - x1) * twody + dx) % twodx) + 
                        twody - 1) / twody - 1;
                }

                if (tx1 > tx2 || tx2 < pClipRect->x1)
                    return 0;
            }

            if (tx2 >= pClipRect->x2)
            {
                tx2 = pClipRect->x2 - 1;
                ty2 = y1 + DIV_CEILING((tx2 - x1) * dy, dx);

                if (tx1 > tx2 || ty2 < pClipRect->y1)
                    return 0;
            }
        }
        else    /* "/" type line */
        {
            if (y1 < pClipRect->y1 || y2 >= pClipRect->y2)
                return 0;

            if (ty1 >= pClipRect->y2)
            {
                ty1 = pClipRect->y2 - 1;
                if (reverse)
                {
                    /*
                    ** When x = tx1, y = ty1
                    ** When x = tx1 - 1, y = ty1 + 1.
                    */
                    tx1 = x2 - DIV_CEILING((ty1 - y2) * dx, dy);
                    tx1 -= ((twodx - ((x2 - tx1) * twody + dx) % twodx) + 
                        twody - 1) / twody - 1;
                }
                else
                {
                    /*
                    ** When x = tx1, y = ty1
                    ** When x = tx1 - 1, y = ty1 + 1.
                    */
                    tx1 = x1 + DIV_CEILING((y1 - ty1) * dx, dy);
                    tx1 -= (((tx1 - x1) * twody + dx) % twodx) / twody;
                }

                if (tx1 > tx2 || tx1 >= pClipRect->x2)
                    return 0;
            }

            if (tx1 < pClipRect->x1)
            {
                tx1 = pClipRect->x1;
                ty1 = y1 - DIV_CEILING((tx1 - x1) * dy, dx);

                if (tx1 > tx2 || ty1 < pClipRect->y1)
                    return 0;
            }

            if (ty2 < pClipRect->y1)
            {
                ty2 = pClipRect->y1;
                if (reverse)
                {
                    /*
                    ** When x = tx2, y = ty2
                    ** When x = tx2 + 1, y = ty2 - 1.
                    */
                    tx2 = x2 - DIV_CEILING((ty2 - y2) * dx, dy);
                    tx2 += (((x2 - tx2) * twody + dx) % twodx) / twody;
                }
                else
                {
                    /*
                    ** When x = tx2, y = ty2
                    ** When x = tx2 + 1, y = ty2 - 1.
                    */
                    tx2 = x1 + DIV_CEILING((y1 - ty2) * dx, dy);
                    tx2 += ((twodx - ((tx2 - x1) * twody + dx) % twodx) + 
                        twody - 1) / twody - 1;
                }

                if (tx1 > tx2 || tx2 < pClipRect->x1)
                    return 0;
            }

            if (tx2 >= pClipRect->x2)
            {
                tx2 = pClipRect->x2 - 1;
                ty2 = y1 - DIV_CEILING((tx2 - x1) * dy, dx);

                if (tx1 > tx2 || ty2 >= pClipRect->y2)
                    return 0;
            }
        }

        if (reverse)
        {
            pLineDraw->start_x   = tx2;
            pLineDraw->start_y   = ty2;
            pLineDraw->end_x     = tx1;
            pLineDraw->end_y     = ty1;
            /* calculate the cliped points number
            */
            pLineDraw->cliped    = x2 - tx2;

            /* The end point of the origin line is not cliped and should not
            ** be drawn.
            */
            /* WWh modified on 2004/07/27
            ** If the left of the line cliped is only the last point, we can't
            ** draw the line at all.
            */
            if (tx1 == pLineData->x2)
            {
                if (tx2 == tx1)
                    return 0;

                pLineDraw->end_x ++;
            }
        }
        else
        {
            pLineDraw->start_x   = tx1;
            pLineDraw->start_y   = ty1;
            pLineDraw->end_x     = tx2;
            pLineDraw->end_y     = ty2;
            /* calculate the cliped points number
            */
            pLineDraw->cliped   = tx1 - x1;

            /* The end point of the origin line is not cliped and should not
            ** be drawn.
            */
            /* WWh modified on 2004/07/27
            ** If the left of the line cliped is only the last point, we can't
            ** draw the line at all.
            */
            if (tx2 == pLineData->x2)
            {
                if (tx2 == tx1)
                    return 0;
                
                pLineDraw->end_x --;
            }
        }
        
        pLineDraw->type = LINE_CTRLX;
    }

    else /* dy > dx */
    {
        pLineDraw->delta     = dx;
        pLineDraw->step      = dy;

        if (x1 > x2)
            pLineDraw->dir = REVR_DIR;
        else
            pLineDraw->dir = NORM_DIR;

        if (y1 > y2)
        {
            EXCHANGE(x1, x2);
            EXCHANGE(y1, y2);
            reverse = 1;
        }

        if (y1 >= pClipRect->y2 || y2 < pClipRect->y1)
            return 0;

        tx1 = x1;
        tx2 = x2;
        ty1 = y1;
        ty2 = y2;
        
        if (x1 < x2)    /* "\" type line*/
        {
            if (x1 >= pClipRect->x2 || x2 < pClipRect->x1)
                return 0;

            if (tx1 < pClipRect->x1)
            {
                tx1 = pClipRect->x1;
                if (reverse)
                {
                    /*
                    ** When y = ty1, x = tx1
                    ** When y = ty1 - 1, y = tx1 - 1.
                    */
                    ty1 = y2 - DIV_CEILING((x2 - tx1) * dy, dx);
                    ty1 -= ((twody - ((y2 - ty1) * twodx + dy) % twody) + 
                        twodx - 1) / twodx - 1;
                }
                else
                {
                    /*
                    ** When y = ty1, x = tx1
                    ** When y = ty1 - 1, y = tx1 - 1.
                    */
                    ty1 = y1 + DIV_CEILING((tx1 - x1) * dy, dx);
                    ty1 -= (((ty1 - y1) * twodx + dy) % twody) / twodx;
                }

                if (ty1 > ty2 || ty1 >= pClipRect->y2)
                    return 0;
            }
            
            if (ty1 < pClipRect->y1)
            {
                ty1 = pClipRect->y1;
                tx1 = x1 + DIV_CEILING((ty1 - y1) * dx, dy);

                if (ty1 > ty2 || tx1 >= pClipRect->x2)
                    return 0;
            }

            if (tx2 >= pClipRect->x2)
            {
                tx2 = pClipRect->x2 - 1;
                if (reverse)
                {
                    /*
                    ** When y = ty2, x = tx2
                    ** When y = ty2 + 1, y = tx2 + 1.
                    */
                    ty2 = y2 - DIV_CEILING((x2 - tx2) * dy, dx);
                    ty2 += (((y2 - ty2) * twodx + dy) % twody) / twodx;
                }
                else
                {
                    /*
                    ** When y = ty2, x = tx2
                    ** When y = ty2 + 1, y = tx2 + 1.
                    */
                    ty2 = y1 + DIV_CEILING((tx2 - x1) * dy, dx);
                    ty2 += ((twody - ((ty2 - y1) * twodx + dy) % twody) + 
                        twodx - 1) / twodx - 1;
                }

                if (ty1 > ty2 || ty2 < pClipRect->y1)
                    return 0;
            }

            if (ty2 >= pClipRect->y2)
            {
                ty2 = pClipRect->y2 - 1;
                tx2 = x1 + DIV_CEILING((ty2 - y1) * dx, dy);

                if (ty1 > ty2 || tx2 < pClipRect->x1)
                    return 0;
            }
        }

        else    /* "/" type line */
        {
            if (x1 < pClipRect->x1 || x2 >= pClipRect->x2)
                return 0;

            if (tx1 >= pClipRect->x2)
            {
                tx1 = pClipRect->x2 - 1;
                if (reverse)
                {
                    /*
                    ** When y = ty1, x = tx1
                    ** When y = ty1 - 1, y = tx1 + 1.
                    */
                    ty1 = y2 - DIV_CEILING((tx1 - x2) * dy, dx);
                    ty1 -= ((twody - ((y2 - ty1) * twodx + dy) % twody) + 
                        twodx - 1) / twodx - 1;
                }
                else
                {
                    /*
                    ** When y = ty1, x = tx1
                    ** When y = ty1 - 1, y = tx1 + 1.
                    */
                    ty1 = y1 + DIV_CEILING((x1 - tx1) * dy, dx);
                    ty1 -= (((ty1 - y1) * twodx + dy) % twody) / twodx;
                }

                if (ty1 > ty2 || ty1 >= pClipRect->y2)
                    return 0;
            }
            
            if (ty1 < pClipRect->y1)
            {
                ty1 = pClipRect->y1;
                tx1 = x1 - DIV_CEILING((ty1 - y1) * dx, dy);

                if (ty1 > ty2 || tx1 < pClipRect->x1)
                    return 0;
            }

            if (tx2 < pClipRect->x1)
            {
                tx2 = pClipRect->x1;
                if (reverse)
                {
                    /*
                    ** When y = ty2, x = tx2
                    ** When y = ty2 + 1, y = tx2 - 1.
                    */
                    ty2 = y2 - DIV_CEILING((tx2 - x2) * dy, dx);
                    ty2 += (((y2 - ty2) * twodx + dy) % twody) / twodx;
                }
                else
                {
                    /*
                    ** When y = ty2, x = tx2
                    ** When y = ty2 + 1, y = tx2 - 1.
                    */
                    ty2 = y1 + DIV_CEILING((x1 - tx2) * dy, dx);
                    ty2 += ((twody - ((ty2 - y1) * twodx + dy) % twody) + 
                        twodx - 1) / twodx - 1;
                }

                if (ty1 > ty2 || ty2 < pClipRect->y1)
                    return 0;
            }

            if (ty2 >= pClipRect->y2)
            {
                ty2 = pClipRect->y2 - 1;
                tx2 = x1 - DIV_CEILING((ty2 - y1) * dx, dy);

                if (ty1 > ty2 || tx2 >= pClipRect->x2)
                    return 0;
            }
        }

        if (reverse)
        {
            pLineDraw->start_x   = tx2;
            pLineDraw->start_y   = ty2;
            pLineDraw->end_x     = tx1;
            pLineDraw->end_y     = ty1;
            /* calculate the cliped points number
            */
            pLineDraw->cliped    = y2 - ty2;

            /* The end point of the origin line is not cliped and should not
            ** be drawn.
            */
            /* WWh modified on 2004/07/27
            ** If the left of the line cliped is only the last point, we can't
            ** draw the line at all.
            */
            if (ty1 == pLineData->y2)
            {
                if (ty1 == ty2)
                    return 0;

                pLineDraw->end_y ++;
            }
        }
        else
        {
            pLineDraw->start_x   = tx1;
            pLineDraw->start_y   = ty1;
            pLineDraw->end_x     = tx2;
            pLineDraw->end_y     = ty2;
            pLineDraw->cliped   = ty1 - y1;

            /* WWh modified on 2004/07/27
            ** If the left of the line cliped is only the last point, we can't
            ** draw the line at all.
            */
            if (ty2 == pLineData->y2)
            {
                if (ty1 == ty2)
                    return 0;

                pLineDraw->end_y --;
            }
        }

        pLineDraw->type = LINE_CTRLY;
    }
    
    return pLineDraw->type;
}

/*********************************************************************\
* Function	  DrawSolidHLine 
* Purpose     Draw Horizonal Line with PS_SOLID style 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DrawSolidHLine(PDEV pDev, int x1, int x2, int y, uint32 rop, 
                           uint32 color)
{
    uint32 left_mask, right_mask;
    uint32 dwords, data, pen;
    uint8* pDevData;
    ROPFUNC ropfunc;
    DWORD i;

    if (x1 > x2)
    {
        EXCHANGE(x1, x2);
    }

    /* Adjust color value */
    if (color == 0)
        pen = 0x00000000;
    else
        pen = 0xFFFFFFFF;

    // Computes left mask and right mask
    left_mask = GetLeftMask32(x1 & 31);
    right_mask = GetRightMask32(x2 & 31);
    dwords = x2 / 32 - x1 / 32 + 1;

    /* Get initial address */
    pDevData = GETXYADDR(pDev, x1, y) - (x1 % 32) / 8;

    switch (rop)
    {
    case ROP_SRC:
        if (dwords == 1)// Only one doubleword to process
        {
            left_mask &= right_mask;
            data = GET32(pDevData);        
            data = data & ~left_mask | pen & left_mask;
            PUT32(pDevData,data);
        }
        else
        {
            // Process first byte
            data = GET32(pDevData);
            data = data & ~left_mask | pen & left_mask;
            PUT32(pDevData,data);
            pDevData += 4;
            
            // Process inner bytes
            for (i = 0; i < dwords - 2; i++)
            {
                PUT32(pDevData,pen);
                pDevData += 4;
            }
            
            // Process last byte
            data = GET32(pDevData);
            data = data & ~right_mask | pen & right_mask;
            PUT32(pDevData,data);            
        }
        return;

    case ROP_SRC_XOR_DST:
        if (dwords == 1)// Only one doubleword to process
        {
            left_mask &= right_mask;
            data = GET32(pDevData);        
            data = data & ~left_mask | (data ^ pen) & left_mask;
            PUT32(pDevData,data);
        }
        else
        {
            // Process first byte
            data = GET32(pDevData);
            data = data & ~left_mask | (data ^ pen) & left_mask;
            PUT32(pDevData,data);
            pDevData += 4;
            
            // Process inner bytes
            for (i = 0; i < dwords - 2; i++)
            {
                data = GET32(pDevData);
                data ^= pen;
                PUT32(pDevData,data);
                pDevData += 4;
            }
            
            // Process last byte
            data = GET32(pDevData);
            data = data & ~right_mask | (data ^ pen) & right_mask;
            PUT32(pDevData,data);            
        }
        return;

    case ROP_NSRC:
        if (dwords == 1)// Only one doubleword to process
        {
            left_mask &= right_mask;
            data = GET32(pDevData);        
            data = data & ~left_mask | ~pen & left_mask;
            PUT32(pDevData,data);
        }
        else
        {
            // Process first byte
            data = GET32(pDevData);
            data = data & ~left_mask | ~pen & left_mask;
            PUT32(pDevData,data);
            pDevData += 4;
            
            // Process inner bytes
            for (i = 0; i < dwords - 2; i++)
            {
                PUT32(pDevData,~pen);
                pDevData += 4;
            }
            
            // Process last byte
            data = GET32(pDevData);
            data = data & ~right_mask | ~pen & right_mask;
            PUT32(pDevData,data);
        }
        return;

    default:
        break;
    }

    // Get rop function
    ropfunc = GetRopFunc(rop);
    if (!ropfunc)
        return;

    if (dwords == 1)// Only one doubleword to process
    {
        left_mask &= right_mask;
        data = GET32(pDevData);
        data = data & ~left_mask |
            (*ropfunc)(pen, data) & left_mask;
        PUT32(pDevData,data);
    }
    else
    {
        // Process first byte
        data = GET32(pDevData);
        data = data & ~left_mask |
            (*ropfunc)(pen, data) & left_mask;
        PUT32(pDevData,data);
        pDevData += 4;

        // Process inner bytes
        for (i = 0; i < dwords - 2; i++)
        {
            data = GET32(pDevData);
            data = (*ropfunc)(pen, data);
            PUT32(pDevData,data);
            pDevData += 4;
        }

        // Process last byte
        data = GET32(pDevData);
        data = data & ~right_mask | 
            (*ropfunc)(pen, data) & right_mask;
        PUT32(pDevData,data);
    }

    return;
}

/*********************************************************************\
* Function	  DrawSolidVLine 
* Purpose     Draw Vertical Line with PS_SOLID style 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DrawSolidVLine(PDEV pDev, int y1, int y2, int x, uint32 rop, 
                           uint32 color)
{
    uint8   mask, pencolor, pixel;
    int     line_bytes;
    ROPFUNC ropfunc;
    uint8   *pDst;
    int16   y;

    if (y1 > y2)
    {
        EXCHANGE(y1, y2);
    }

    pencolor = (uint8)((color == 0) ? 0x00 : 0xFFFFFFFFL);

    mask = 0x80 >> (x % 8);
    pDst = GETXYADDR(pDev, x, y1);

    line_bytes = pDev->line_bytes;

    switch (rop)
    {
    case ROP_SRC:
        for (y = y1; y <= y2; y++)
        {
            PUTPIXEL(pDst, pencolor, mask);
            pDst += line_bytes;
        }
        return;

    case ROP_SRC_XOR_DST:
        for (y = y1; y <= y2; y++)
        {
            PUTPIXEL(pDst, ((*pDst) ^ pencolor), mask);
            pDst += line_bytes;    
        }
        return;

    case ROP_NSRC:
        for (y = y1; y <= y2; y++)
        {
            PUTPIXEL(pDst, ~pencolor, mask);
            pDst += line_bytes;   
        }
        return;

    default:
        break;
    }

    ropfunc = GetRopFunc(rop);
    if (!ropfunc)
        return;

    for (y = y1; y <= y2; y++)
    {
        pixel = (BYTE)(*ropfunc)(pencolor, GETPIXEL(pDst));
        PUTPIXEL(pDst, pixel, mask);
        pDst += line_bytes;   
    }

    return;
}

#ifdef SLOPELINE_SUPPORTED
/*********************************************************************\
* Function	   DrawSolidXLine
* Purpose      Draw X-Ctrl Line with PS_SOLID style
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DrawSolidXLine(PDEV pDev, LINEDRAW* pLineDraw,
                      GRAPHPARAM* pGraphParam)
{
    uint8   *pDst;
    uint32  pencolor, pixel;
    int     dir, line_bytes;
    ROPFUNC ropfunc;
    int     i;
    int     x1, y1, x2, y2;
    int     dx, dy, twoDy, twoDyDx;
    int     flagBresenham;
    uint32  rop;
    uint8   mask;

    rop = SRop3ToRop2(pGraphParam->rop);
    ASSERT(pGraphParam->pPen);

    x1  = pLineDraw->start_x;
    y1  = pLineDraw->start_y;
    x2  = pLineDraw->end_x;
    y2  = pLineDraw->end_y;

    pencolor = (pGraphParam->pen_color == 0) ? 0x00 : 0xFFFFFFFFL;

    mask = 0x80 >> (x1 % 8);
    pDst = GETXYADDR(pDev, x1, y1);

    line_bytes  = (pLineDraw->dir == REVR_DIR) ? 
        -pDev->line_bytes : pDev->line_bytes;

    dir   = (x1 > x2) ? -1 : 1;
    
    /* use the origin points to calculate the line!
    ** so the line cliped can be drawn as same as without cliped
    ** pLineDraw->cliped is the points number that have been cliped from the
    ** start point.
    ** The calculation of flagBresenham is equal to do the flagBresenham
    ** loops with number of pLineDraw->cliped times.
    */
    dx  = pLineDraw->step;
    dy  = pLineDraw->delta;

    twoDy   = dy * 2;
    flagBresenham   = twoDy - dx;
    twoDyDx = flagBresenham - dx;


    if (pLineDraw->cliped != 0)
    {
        flagBresenham = (pLineDraw->cliped) * twoDy + dx;
        flagBresenham %= 2 * dx;
        flagBresenham += twoDy;
        flagBresenham -= 2 * dx;
    }

/* The below is equal to the above calculator */
/*
    flagBresenham = twoDy - dx;
    for (i = 0; i < pLineDraw->cliped; i++)
    {
        if (flagBresenham < 0)
        {
            flagBresenham += twoDy;
        }
        else
        {
            flagBresenham += twoDyDx;
        }
    }
*/
    if (x1 > x2)
        EXCHANGE(x1, x2);

    switch (rop)
    {
    case ROP_SRC:

        if (dir == 1) /* x increase */
        {
            for (i = x1; i <= x2; i ++)
            {
                PUTPIXEL(pDst, pencolor, mask);
                
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    pDst += line_bytes;
                    flagBresenham += twoDyDx;
                }
                
                mask >>= 1;
                if (mask == 0)
                {
                    mask = 0x80;
                    pDst++;
                }
            }
        }
        else /* x decrease */
        {
            for (i = x1; i <= x2; i ++)
            {
                PUTPIXEL(pDst, pencolor, mask);
                
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    pDst += line_bytes;
                    flagBresenham += twoDyDx;
                }
                
                mask <<= 1;
                if (mask == 0)
                {
                    mask = 0x01;
                    pDst--;
                }
            }
        }

        break;

    default:

        ropfunc = GetRopFunc(rop);

        if (dir == 1)
        {
            for (i = x1; i <= x2; i ++)
            {
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, mask);
                
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    pDst += line_bytes;
                    flagBresenham += twoDyDx;
                }
                
                mask >>= 1;
                if (mask == 0)
                {
                    mask = 0x80;
                    pDst++;
                }
            }
        }
        else
        {
            for (i = x1; i <= x2; i ++)
            {
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, mask);
                
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    pDst += line_bytes;
                    flagBresenham += twoDyDx;
                }
                
                mask <<= 1;
                if (mask == 0)
                {
                    mask = 0x01;
                    pDst--;
                }
            }
        }

        break;
    }
}

/*********************************************************************\
* Function	  DrawSolidYLine 
* Purpose     Draw Y-Ctrl Line with PS_SOLID style 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DrawSolidYLine(PDEV pDev, LINEDRAW* pLineDraw,
                      GRAPHPARAM* pGraphParam)
{
    uint8   *pDst;
    uint32  pencolor, pixel;
    uint8   mask;
    int     dir, line_bytes;
    ROPFUNC ropfunc;
    int     i;
    int     x1, y1, x2, y2;
    int     dx, dy, twoDy, twoDyDx;
    int     flagBresenham;
    uint32  rop;

    rop = SRop3ToRop2(pGraphParam->rop);
    ASSERT(pGraphParam->pPen);

    x1  = pLineDraw->start_x;
    y1  = pLineDraw->start_y;
    x2  = pLineDraw->end_x;
    y2  = pLineDraw->end_y;

    pencolor = (pGraphParam->pen_color == 0) ? 0x00 : 0xFFFFFFFFL;

    pDst = GETXYADDR(pDev, x1, y1);
    mask = 0x80 >> (x1 % 8);

    line_bytes  = (y1 > y2) ? -pDev->line_bytes : pDev->line_bytes;
    
    dir  = (pLineDraw->dir == REVR_DIR) ? -1 : 1;

    /* The follow dx, dy are not the real x and y coordinates. They are just
    ** used to do the Bresenham algorithms. You can think that the x and y 
    ** have been exchanged.
    */
    dx  = pLineDraw->step;
    dy  = pLineDraw->delta;

    twoDy   = dy * 2;
    flagBresenham   = twoDy - dx;
    twoDyDx = flagBresenham - dx;

    if (pLineDraw->cliped != 0)
    {
        flagBresenham = pLineDraw->cliped * twoDy + dx;
        flagBresenham %= 2 * dx;
        flagBresenham += twoDy;
        flagBresenham -= 2 * dx;
    }

    if (y1 > y2)
        EXCHANGE(y1, y2);

    switch (rop)
    {
    case ROP_SRC:

        if (dir == 1) /* x increase */
        {
            for (i = y1; i <= y2; i ++)
            {
                PUTPIXEL(pDst, pencolor, mask);
                
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    mask >>= 1;
                    if (mask == 0)
                    {
                        mask = 0x80;
                        pDst++;
                    }
                    
                    flagBresenham += twoDyDx;
                }
                
                pDst += line_bytes;
            }
        }
        else /* x decrease */
        {
            for (i = y1; i <= y2; i ++)
            {
                PUTPIXEL(pDst, pencolor, mask);
                
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    mask <<= 1;
                    if (mask == 0)
                    {
                        mask = 0x01;
                        pDst--;
                    }
                    
                    flagBresenham += twoDyDx;
                }
                
                pDst += line_bytes;
            }
        }

        break;

    default:

        ropfunc = GetRopFunc(rop);

        if (dir == 1) /* x increase */
        {
            for (i = y1; i <= y2; i ++)
            {
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, mask);
                
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    mask >>= 1;
                    if (mask == 0)
                    {
                        mask = 0x80;
                        pDst++;
                    }
                    
                    flagBresenham += twoDyDx;
                }
                
                pDst += line_bytes;
            }
        }
        else
        {
            for (i = y1; i <= y2; i ++)
            {
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, mask);
                
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    mask <<= 1;
                    if (mask == 0)
                    {
                        mask = 0x01;
                        pDst--;
                    }
                    
                    flagBresenham += twoDyDx;
                }
                
                pDst += line_bytes;
            }
        }

        break;
    }
}

#endif

/*********************************************************************\
* Function	  DrawHLine 
* Purpose     Draw Horizonal Line with not PS_SOLID style 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DrawHLine(PDEV pDev, int x1, int x2, int y, 
                      GRAPHPARAM* pGraphParam, PENSTYLE penstyle)
{
    uint8   *pDst;
    int     dir;
    uint32  pixel, bkcolor, pencolor;
    uint8   mask;
    uint32  penmask, penpattern;
    ROPFUNC ropfunc;
    ROPFUNC ropDashfunc;
    int     i;
    uint32  rop;

    rop = SRop3ToRop2(pGraphParam->rop);

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
    penpattern = penstyle.pattern;
    bkcolor = (pGraphParam->bk_color == 0) ? 0x00 : 0xFFFFFFFFL;
    pencolor = (pGraphParam->pen_color == 0) ? 0x00 : 0xFFFFFFFFL;

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
            if ((penmask >> (32 - penstyle.count)) == 0)
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
            if ((penmask >> (32 - penstyle.count)) == 0)
            {
                penmask = 0x80000000L;
            }
        }
    }
}
/*********************************************************************\
* Function	  DrawVLine 
* Purpose     Draw Vertical Line with not PS_SOLID style 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DrawVLine(PDEV pDev, int y1, int y2, int x, 
                      GRAPHPARAM* pGraphParam, PENSTYLE penstyle)
{
    uint8   *pDst;
    int     line_bytes;
    uint32  pixel, bkcolor, pencolor;
    uint8   mask;
    uint32  penmask, penpattern;
    ROPFUNC ropfunc;
    ROPFUNC ropDashfunc;
    int     i;
    uint32  rop;

    rop = SRop3ToRop2(pGraphParam->rop);

    pDst = GETXYADDR(pDev, x, y1);
    mask = 0x80 >> (x % 8);

    if (y1 > y2)
    {
        EXCHANGE(y1, y2);
        line_bytes = -pDev->line_bytes;
    }
    else
    {
        line_bytes = pDev->line_bytes;
    }

    penmask = 0x80000000L;
    penpattern = penstyle.pattern;
    bkcolor = (pGraphParam->bk_color == 0) ? 0x00 : 0xFFFFFFFFL;
    pencolor = (pGraphParam->pen_color == 0) ? 0x00 : 0xFFFFFFFFL;
    
    ropfunc = GetRopFunc(rop);
    if (pGraphParam->bk_mode == BM_TRANSPARENT)
        ropDashfunc = GetRopFunc(ROP_DST);
    else
        ropDashfunc = ropfunc;

    for(i = y1; i <= y2; i++)
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
        pDst += line_bytes;

        penmask >>= 1;
        if ((penmask >> (32 - penstyle.count)) == 0)
        {
            penmask = 0x80000000L;
        }
    }

    return;
}


#ifdef SLOPELINE_SUPPORTED

/*********************************************************************\
* Function	  DrawXLine 
* Purpose     Draw X_Ctrl Line with not PS_SOLID style 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DrawXLine(PDEV pDev, LINEDRAW* pLineDraw,
                      GRAPHPARAM* pGraphParam, PENSTYLE penstyle)
{

    uint8   *pDst;
    int     dir, line_bytes;
    uint32  pixel, bkcolor, pencolor;
    uint8   mask;
    uint32  penmask, penpattern;
    ROPFUNC ropfunc;
    ROPFUNC ropDashfunc;
    int     i;
    int     x1, y1, x2, y2;
    int     dx, dy, twoDy, twoDyDx;
    int     flagBresenham;
    uint32  rop;

    rop = SRop3ToRop2(pGraphParam->rop);

    x1  = pLineDraw->start_x;
    y1  = pLineDraw->start_y;
    x2  = pLineDraw->end_x;
    y2  = pLineDraw->end_y;

    penmask = 0x80000000L;
    penpattern = penstyle.pattern;
    bkcolor = (pGraphParam->bk_color == 0) ? 0x00 : 0xFFFFFFFFL;
    pencolor = (pGraphParam->pen_color == 0) ? 0x00 : 0xFFFFFFFFL;

    pDst = GETXYADDR(pDev, x1, y1);
    mask = 0x80 >> (x1 % 8);

    line_bytes  = (pLineDraw->dir == REVR_DIR) ? 
        -pDev->line_bytes : pDev->line_bytes;

    dir   = (x1 > x2) ? -1 : 1;

    dx  = pLineDraw->step;
    dy  = pLineDraw->delta;

    twoDy   = dy * 2;
    flagBresenham   = twoDy - dx;
    twoDyDx = flagBresenham - dx;

    if (pLineDraw->cliped != 0)
    {
        flagBresenham = pLineDraw->cliped * twoDy + dx;
        flagBresenham %= 2 * dx;
        flagBresenham += twoDy;
        flagBresenham -= 2 * dx;
    }

    if (x1 > x2)
        EXCHANGE(x1, x2);

    switch(rop)
    {
    case ROP_SRC:

        if (pGraphParam->bk_mode != BM_TRANSPARENT)
        {
            if (dir == 1) /* x increase */
            {
                for (i = x1; i <= x2; i ++)
                {
                    if (penmask & penpattern)
                    {
                        PUTPIXEL(pDst, pencolor, mask);
                    }
                    else
                    {
                        PUTPIXEL(pDst, bkcolor, mask);
                    }
                    
                    if (flagBresenham < 0)
                    {
                        flagBresenham += twoDy;
                    }
                    else
                    {
                        pDst += line_bytes;
                        flagBresenham += twoDyDx;
                    }
                    
                    mask >>= 1;
                    if (mask == 0)
                    {
                        mask = 0x80;
                        pDst++;
                    }
                    
                    penmask >>= 1;
                    if ((penmask >> (32 - penstyle.count)) == 0)
                    {
                        penmask = 0x80000000L;
                    }
                }
            }
            else /* x decrease */
            {
                for (i = x1; i <= x2; i ++)
                {
                    if (penmask & penpattern)
                    {
                        PUTPIXEL(pDst, pencolor, mask);
                    }
                    else
                    {
                        PUTPIXEL(pDst, bkcolor, mask);
                    }
                    
                    if (flagBresenham < 0)
                    {
                        flagBresenham += twoDy;
                    }
                    else
                    {
                        pDst += line_bytes;
                        flagBresenham += twoDyDx;
                    }
                    
                    mask <<= 1;
                    if (mask == 0)
                    {
                        mask = 0x01;
                        pDst--;
                    }
                    
                    penmask >>= 1;
                    if ((penmask >> (32 - penstyle.count)) == 0)
                    {
                        penmask = 0x80000000L;
                    }
                }
            }
        }//pGraphParam->bk_mode != BM_TRANSPARENT
        else //pGraphParam->bk_mode == BM_TRANSPARENT
        {
            if (dir == 1)
            {
                for (i = x1; i <= x2; i ++)
                {
                    if (penmask & penpattern)
                    {
                        PUTPIXEL(pDst, pencolor, mask);
                    }
                    
                    if (flagBresenham < 0)
                    {
                        flagBresenham += twoDy;
                    }
                    else
                    {
                        pDst += line_bytes;
                        flagBresenham += twoDyDx;
                    }
                    
                    mask >>= 1;
                    if (mask == 0)
                    {
                        mask = 0x80;
                        pDst++;
                    }
                    
                    penmask >>= 1;
                    if ((penmask >> (32 - penstyle.count)) == 0)
                    {
                        penmask = 0x80000000L;
                    }
                }
            }
            else
            {
                for (i = x1; i <= x2; i ++)
                {
                    if (penmask & penpattern)
                    {
                        PUTPIXEL(pDst, pencolor, mask);
                    }
                    
                    if (flagBresenham < 0)
                    {
                        flagBresenham += twoDy;
                    }
                    else
                    {
                        pDst += line_bytes;
                        flagBresenham += twoDyDx;
                    }
                    
                    mask <<= 1;
                    if (mask == 0)
                    {
                        mask = 0x01;
                        pDst--;
                    }
                    
                    penmask >>= 1;
                    if ((penmask >> (32 - penstyle.count)) == 0)
                    {
                        penmask = 0x80000000L;
                    }
                }
            }
        }

        break;

    default:

        ropfunc = GetRopFunc(rop);
        if (pGraphParam->bk_mode == BM_TRANSPARENT)
            ropDashfunc = GetRopFunc(ROP_DST);
        else
            ropDashfunc = ropfunc;

        if (dir == 1)
        {
            for (i = x1; i <= x2; i ++)
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
                
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    pDst += line_bytes;
                    flagBresenham += twoDyDx;
                }
                
                mask >>= 1;
                if (mask == 0)
                {
                    mask = 0x80;
                    pDst++;
                }
                
                penmask >>= 1;
                if ((penmask >> (32 - penstyle.count)) == 0)
                {
                    penmask = 0x80000000L;
                }
            }
        }
        else /* x decrease */
        {
            for (i = x1; i <= x2; i ++)
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
                
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    pDst += line_bytes;
                    flagBresenham += twoDyDx;
                }
                
                mask <<= 1;
                if (mask == 0)
                {
                    mask = 0x01;
                    pDst++;
                }
                
                penmask >>= 1;
                if ((penmask >> (32 - penstyle.count)) == 0)
                {
                    penmask = 0x80000000L;
                }
            }
        }

        break;
    }
}

/*********************************************************************\
* Function	   DrawYLine
* Purpose      Draw Y_Ctrl Line with not PS_SOLID styl
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DrawYLine(PDEV pDev, LINEDRAW* pLineDraw,
                      GRAPHPARAM* pGraphParam, PENSTYLE penstyle)
{
    uint8   *pDst;
    int     dir, line_bytes;
    uint32  pixel, bkcolor, pencolor;
    uint8   mask;
    uint32  penmask, penpattern;
    ROPFUNC ropfunc;
    ROPFUNC ropDashfunc;
    int     i;
    int     x1, y1, x2, y2;
    int     dx, dy, twoDy, twoDyDx;
    int     flagBresenham;
    uint32  rop;

    rop = SRop3ToRop2(pGraphParam->rop);
    ASSERT(pGraphParam->pPen->style != PS_SOLID);

    x1  = pLineDraw->start_x;
    y1  = pLineDraw->start_y;
    x2  = pLineDraw->end_x;
    y2  = pLineDraw->end_y;

    penmask = 0x80000000L;
    penpattern = penstyle.pattern;
    bkcolor = (pGraphParam->bk_color == 0) ? 0x00 : 0xFFFFFFFFL;
    pencolor = (pGraphParam->pen_color == 0) ? 0x00 : 0xFFFFFFFFL;

    pDst    = GETXYADDR(pDev, x1, y1);
    mask    = 0x80 >> (x1 % 8);

    line_bytes  = (y1 > y2) ? -pDev->line_bytes : pDev->line_bytes;

    dir  = (pLineDraw->dir == REVR_DIR) ? -1 : 1;
    
    /* The follow dx, dy are not the real x and y coordinates. They are just
    ** used to do the Bresenham algorithms. You can think that the x and y 
    ** have been exchanged.
    */
    dx  = pLineDraw->step;
    dy  = pLineDraw->delta;

    twoDy   = dy * 2;
    flagBresenham   = twoDy - dx;
    twoDyDx = flagBresenham - dx;

    if (pLineDraw->cliped != 0)
    {
        flagBresenham = pLineDraw->cliped * twoDy + dx;
        flagBresenham %= 2 * dx;
        flagBresenham += twoDy;
        flagBresenham -= 2 * dx;
    }

    if (y1 > y2)
        EXCHANGE(y1, y2);

    switch(rop)
    {
    case ROP_SRC:

        if (pGraphParam->bk_mode != BM_TRANSPARENT)
        {
            if (dir == 1)
            {
                for (i = y1; i <= y2; i ++)
                {
                    if (penmask & penpattern)
                    {
                        PUTPIXEL(pDst, pencolor, mask);
                    }
                    else
                    {
                        PUTPIXEL(pDst, bkcolor, mask);
                    }
                    
                    if (flagBresenham < 0)
                    {
                        flagBresenham += twoDy;
                    }
                    else
                    {
                        mask >>= 1;
                        if (mask == 0)
                        {
                            mask = 0x80;
                            pDst++;
                        }
                        flagBresenham += twoDyDx;
                    }
                    pDst += line_bytes;
                    
                    penmask >>= 1;
                    if ((penmask >> (32 - penstyle.count)) == 0)
                    {
                        penmask = 0x80000000L;
                    }
                }
            }
            else
            {
                for (i = y1; i <= y2; i ++)
                {
                    if (penmask & penpattern)
                    {
                        PUTPIXEL(pDst, pencolor, mask);
                    }
                    else
                    {
                        PUTPIXEL(pDst, bkcolor, mask);
                    }
                    
                    if (flagBresenham < 0)
                    {
                        flagBresenham += twoDy;
                    }
                    else
                    {
                        mask <<= 1;
                        if (mask == 0)
                        {
                            mask = 0x01;
                            pDst--;
                        }
                        flagBresenham += twoDyDx;
                    }
                    pDst += line_bytes;
                    
                    penmask >>= 1;
                    if ((penmask >> (32 - penstyle.count)) == 0)
                    {
                        penmask = 0x80000000L;
                    }
                }
            }
        }
        else
        {
            if (dir == 1)
            {
                for (i = y1; i <= y2; i ++)
                {
                    if (penmask & penpattern)
                    {
                        PUTPIXEL(pDst, pencolor, mask);
                    }
                    
                    if (flagBresenham < 0)
                    {
                        flagBresenham += twoDy;
                    }
                    else
                    {
                        mask >>= 1;
                        if (mask == 0)
                        {
                            mask = 0x80;
                            pDst++;
                        }
                        flagBresenham += twoDyDx;
                    }
                    pDst += line_bytes;
                    
                    penmask >>= 1;
                    if ((penmask >> (32 - penstyle.count)) == 0)
                    {
                        penmask = 0x80000000L;
                    }
                }
            }
            else
            {
                for (i = y1; i <= y2; i ++)
                {
                    if (penmask & penpattern)
                    {
                        PUTPIXEL(pDst, pencolor, mask);
                    }
                    
                    if (flagBresenham < 0)
                    {
                        flagBresenham += twoDy;
                    }
                    else
                    {
                        mask <<= 1;
                        if (mask == 0)
                        {
                            mask = 0x01;
                            pDst--;
                        }
                        flagBresenham += twoDyDx;
                    }
                    pDst += line_bytes;
                    
                    penmask >>= 1;
                    if ((penmask >> (32 - penstyle.count)) == 0)
                    {
                        penmask = 0x80000000L;
                    }
                }
            }
        }

        break;

    default:

        ropfunc = GetRopFunc(rop);
        if (pGraphParam->bk_mode == BM_TRANSPARENT)
            ropDashfunc = GetRopFunc(ROP_DST);
        else
            ropDashfunc = ropfunc;

        if (dir == 1)
        {
            for (i = y1; i <= y2; i ++)
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
                
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    mask >>= 1;
                    if (mask == 0)
                    {
                        mask = 0x80;
                        pDst++;
                    }
                    flagBresenham += twoDyDx;
                }
                pDst += line_bytes;
                
                penmask >>= 1;
                if ((penmask >> (32 - penstyle.count)) == 0)
                {
                    penmask = 0x80000000L;
                }
            } 
        }
        else
        {
            for (i = y1; i <= y2; i ++)
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
                
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    mask <<= 1;
                    if (mask == 0)
                    {
                        mask = 0x01;
                        pDst--;
                    }
                    flagBresenham += twoDyDx;
                }
                pDst += line_bytes;
                
                penmask >>= 1;
                if ((penmask >> (32 - penstyle.count)) == 0)
                {
                    penmask = 0x80000000L;
                }
            } 
        }

        break;
    }
}
#endif


/*********************************************************************\
* Function	  DrawThinLine 
* Purpose     Draw thin Line with width = 1. 
* Params	   
    pDev        Pointer to display device structure.
    pGraphParam Pointer to display device parameter.
    pLineData   Pointer to drawing shape data.
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawThinLine(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PLINEDATA pLineData)
{
    RECT     rc, rcLine;
    LINEDATA line;
    LINEDRAW LineDraw;
    int      LineType;
    PPHYSPEN pPen;
    PENSTYLE penstyle;
    uint32   rop;

    if (pGraphParam != NULL && (pGraphParam->flags & GPF_CLIP_RECT))
    {
        CopyRect(&rc, &pGraphParam->clip_rect);
        ClipByDev(&rc, pDev);
        if (!IsValidRect(&rc))
            return -1;
    }
    else
    {
        rc.x1 = 0;
        rc.y1 = 0;
        rc.x2 = pDev->width;
        rc.y2 = pDev->height;       
    }

    line.x1 = pLineData->x1;
    line.y1 = pLineData->y1;
    line.x2 = pLineData->x2;
    line.y2 = pLineData->y2;

    SetRect(&rcLine, MIN(line.x1, line.x2), 
       MIN(line.y1, line.y2), 
       MAX(line.x1, line.x2) + 1, 
       MAX(line.y1, line.y2) + 1);
    ClipRect(&rcLine, &rc);
    if (!IsValidRect(&rcLine))
        return -1;

    LineType = ClipLine(&line, &rc, &LineDraw);
    if (LineType == 0)
        return 0;
        
    _ExcludeCursor(pDev, &rcLine);

    rop = SRop3ToRop2(pGraphParam->rop);
    pPen = pGraphParam->pPen;
    switch (LineType) 
    {
    case LINE_HORIZONTAL:

        if (pPen->style == PS_SOLID)
        {
            DrawSolidHLine(pDev, LineDraw.start_x, LineDraw.end_x, 
                LineDraw.start_y, rop, pGraphParam->pen_color);
        }
        else
        {
            penstyle = GetPenStyle(pPen->style);
            if (LineDraw.cliped != 0)
            {
                /* When the line was cliped, to keep the part line to be as
                ** same as origin line, we should change the pattern start 
                ** point.
                */
                penstyle.pattern = GetOffsetPenStyle(penstyle.pattern, 
                    penstyle.count, LineDraw.cliped % penstyle.count);
            }

            DrawHLine(pDev, LineDraw.start_x, LineDraw.end_x, 
                LineDraw.start_y, pGraphParam, penstyle);
        }
            
        break;

    case LINE_VERTICAL:

        if (pPen->style == PS_SOLID)
        {
            DrawSolidVLine(pDev, LineDraw.start_y, LineDraw.end_y, 
                LineDraw.start_x, rop, pGraphParam->pen_color);
        }
        else
        {
            penstyle = GetPenStyle(pPen->style);
            if (LineDraw.cliped != 0)
            {
                /* When the line was cliped, to keep the part line to be as
                ** same as origin line, we should change the pattern start 
                ** point.
                */
                penstyle.pattern = GetOffsetPenStyle(penstyle.pattern, 
                    penstyle.count, LineDraw.cliped % penstyle.count);
            }

            DrawVLine(pDev, LineDraw.start_y, LineDraw.end_y, 
                LineDraw.start_x, pGraphParam, penstyle);
        }

        break;

    case LINE_CTRLX:

#ifdef SLOPELINE_SUPPORTED

        if (pPen->style == PS_SOLID)
        {
            DrawSolidXLine(pDev, &LineDraw, pGraphParam);
        }
        else
        {
            penstyle = GetPenStyle(pPen->style);
            if (LineDraw.cliped != 0)
            {
                /* When the line was cliped, to keep the part line to be as
                ** same as origin line, we should change the pattern start 
                ** point.
                */
                penstyle.pattern = GetOffsetPenStyle(penstyle.pattern, 
                    penstyle.count, LineDraw.cliped % penstyle.count);
            }

            DrawXLine(pDev, &LineDraw, pGraphParam, penstyle);
        }

#endif

        break;

    case LINE_CTRLY:

#ifdef SLOPELINE_SUPPORTED

        if (pPen->style == PS_SOLID)
        {
            DrawSolidYLine(pDev, &LineDraw, pGraphParam);
        }
        else
        {
            penstyle = GetPenStyle(pPen->style);
            if (LineDraw.cliped != 0)
            {
                /* When the line was cliped, to keep the part line to be as
                ** same as origin line, we should change the pattern start 
                ** point.
                */
                penstyle.pattern = GetOffsetPenStyle(penstyle.pattern, 
                    penstyle.count, LineDraw.cliped % penstyle.count);
            }

            DrawYLine(pDev, &LineDraw, pGraphParam, penstyle);
        }

#endif
        break;

    default:

        break;
    }

    /* In the emulate environment, we should change the update rect to 
    ** implement the screen refresh.
    */
    _UnexcludeCursor(pDev);

    return 0;
}

/*********************************************************************\
* Function	   DrawWideLine
* Purpose      Draw wide Line with width > 1.
* Params	   
    pDev        Pointer to display device structure.
    pGraphParam Pointer to display device parameter.
    pLineData   Pointer to drawing shape data.
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawWideLine(PDEV pDev, PGRAPHPARAM pGraphParam, 
                         PLINEDATA pLineData)
{
    int         width;
    PPHYSPEN    pPen;
    RECT        LineRC;
    uint32      rop;

    ASSERT(pGraphParam != NULL);
    if (pGraphParam->pPen == NULL)
        return 0;

    rop = SRop3ToRop2(pGraphParam->rop);
    pPen  = pGraphParam->pPen;
    width = pGraphParam->pPen->width;

    LineRC.x1 = pLineData->x1;
    LineRC.x2 = pLineData->x2;
    LineRC.y1 = pLineData->y1;
    LineRC.y2 = pLineData->y2;

    if (pLineData->y1 == pLineData->y2)
    {   /* Horyzontal line */
        if (pLineData->x1 > pLineData->x2)
        {
            LineRC.x1 = pLineData->x2 + 1;
            LineRC.x2 = pLineData->x1 + 1;
        }
        /* wwh modified on 2002/09/25 
        ** A bug occured when draw wide line.
        ** The frame_mode is invalide in drawing wide line.So needn't judge 
        ** the frame_mode 
        */
//        if (pGraphParam->pPen->frame_mode == PFM_CENTER)
//        {
            LineRC.y1 -= width / 2;
            LineRC.y2 += width - width / 2;
            LineRC.x1 -= width / 2;
            LineRC.x2 += width - width / 2;
//        }

        ClipByDev(&LineRC, pDev);
        if (!IsValidRect(&LineRC))
            return -1;
        
        if ((pGraphParam->flags & GPF_CLIP_RECT) && 
            !ClipRect(&LineRC, &pGraphParam->clip_rect))
            return -1;

        _ExcludeCursor(pDev, &LineRC);
        /* Use FillSolidRect to realize the wide horizonal or vertical line.
        ** we just give the color and rop.
        */
        FillSolidRect(pDev, &LineRC, pGraphParam->pen_color, rop);
    }
    else if (pLineData->x1 == pLineData->x2)
    {   /* Vertical  line */
        if (pLineData->y1 > pLineData->y2)
        {
            LineRC.y1 = pLineData->y2 + 1;
            LineRC.y2 = pLineData->y1 + 1;
        }
        /* wwh modified on 2002/09/25 
        ** A bug occured when draw wide line.
        ** The frame_mode is invalide in drawing wide line.So needn't judge 
        ** the frame_mode 
        */
//        if (pGraphParam->pPen->frame_mode == PFM_CENTER)
//        {
            LineRC.x1 -= width / 2;
            LineRC.x2 += width - width / 2;
            LineRC.y1 -= width / 2;
            LineRC.y2 += width - width / 2;
//        }

        ClipByDev(&LineRC, pDev);
        if (!IsValidRect(&LineRC))
            return -1;

        if ((pGraphParam->flags & GPF_CLIP_RECT) && 
            !ClipRect(&LineRC, &pGraphParam->clip_rect))
            return -1;

        _ExcludeCursor(pDev, &LineRC);
        /* Use FillSolidRect to realize the wide horizonal or vertical line.
        ** we just give the color and rop.
        */
        FillSolidRect(pDev, &LineRC, 
            pGraphParam->pen_color, rop);
    }
    else
    {   /* Other direction, use emulate to draw.现未实现 */
        return 0;
    }

    _UnexcludeCursor(pDev);

    return 0;
}

/*************************************************************************/
/*  DDI_line 为手写识别和速记单独实现的函数
/*************************************************************************/
#if (TOUCH_DEVICE)
static RECT DDD_ClipRect = {0, 0, 0, 0};
/* WWH modified on 2004/03/15, use the GET32 macro */
int MONO_HW_HLine(PDEV pDev, int x1, int x2, int y)
{
    uint32 LeftMask, RightMask;
    uint16 dwords;
    uint32 *pDst, *pVal;
    uint32 data;
    int h, i;

    // Computes left mask and right mask
    LeftMask = GetLeftMask32(x1 & 31);
    RightMask = GetRightMask32(x2 & 31);

    pDst = (uint32*)(pDev->pdata + y * pDev->line_bytes + x1 / 8
        - (x1 % 32) / 8);

    dwords = x2 / 32 - x1 / 32 + 1;
    if (dwords == 1)
        LeftMask &= RightMask;

    //set black color
    for(h = 0; h < 2; h++)
    {
        pVal = pDst;
        if (dwords == 1)// Only one doubleword to process
        {
            data = GET32(pVal);        
            PUT32(pVal,data & ~LeftMask);
        }
        else
        {
            // Process first byte
            data = GET32(pVal);        
            PUT32(pVal,data & ~LeftMask);
            pVal ++;
            
            // Process inner bytes
            for (i = 0; i < dwords - 2; i++)
            {
                PUT32(pVal, 0);
                pVal ++;
            }
            
            // Process last byte
            data = GET32(pVal);        
            PUT32(pVal,data & ~RightMask);
        }

        //another line
        pDst += pDev->line_bytes / 4;
    }

    return 1;
}

int MONO_HW_VLine(PDEV pDev, int x, int y1, int y2)
{
    uint8 mask, *pDst;
    int h;

    h = y2 - y1 + 1;   //height of the line
    //get starting address
    pDst = (uint8 *)(pDev->pdata + pDev->line_bytes * y1 + 
        x / 8);

    //pen mask of the first pixel, penwidth = 2
    mask = 0xc0 >> (x & 7);

    if (mask == 0x01) //the last pixel in a byte
    {
        while (h--)
        {
            *pDst = *pDst & ~mask;          //set black color
            *(pDst + 1) &= 0x7F;
            pDst += pDev->line_bytes;          //next line
        }
    }
    else
    {
        while (h--)
        {
            *pDst = *pDst & ~mask;    //set black color
            pDst += pDev->line_bytes;    //next line
        }
    }

    return 1;
}

int MONO_HW_XLine(PDEV pDev, int x1, int y1, int x2, int y2, int dir)
{
    int16 dx, dy, p, x, y;
    int16 twoDy, twoDyDx;
    uint8 *pDst, *pDst2;
    uint8 mask;

    dx = ABS(x2 - x1);
    dy = ABS(y2 - y1);
    
    twoDy = dy * 2;
    p = twoDy - dx;
    twoDyDx = p - dx;

    /*Draw first point*/
    pDst = (uint8 *)(pDev->pdata + pDev->line_bytes * y1 + x1 / 8);
    mask = 0x80 >> (x1 & 7);
    *pDst = *pDst & ~mask;   //set black color

    pDst2 = pDst + pDev->line_bytes; //next line
    *pDst2 = *pDst2 & ~mask; //set black color

//    x = x1 + 1;
    x = x1;
    y = y1;

    if (dir == NORM_DIR)
    {
        for(; x <= x2; x++)
        {
            if (p < 0)
                p += twoDy;
            else
            {
                p += twoDyDx;               
                pDst += pDev->line_bytes;
                y++;
            }

            /*Draw next point*/
            mask >>= 1;
            if (mask == 0)
            {
                mask = 0x80;
                pDst++;
            }
            *pDst = *pDst & ~mask;        //set black color
            pDst2 = pDst + pDev->line_bytes; //next line
            *pDst2 = *pDst2 & ~mask;      //set black color
        }
    }
    else //REV_DIR
    {
        for(; x <= x2; x++)
        {
            if (p < 0)
                p += twoDy;
            else
            {
                p += twoDyDx;               
                pDst -= pDev->line_bytes;
                y--;
            }
            
            /*Draw next point*/
            mask >>= 1;
            if (mask == 0)
            {
                mask = 0x80;
                pDst++;
            }
            *pDst = *pDst & ~mask;        //set black color
            pDst2 = pDst + pDev->line_bytes; //next line
            *pDst2 = *pDst2 & ~mask;      //set black color
        }
    }

    return 1;
}

int MONO_HW_YLine(PDEV pDev,  int x1, int y1, int x2, int y2, int dir)
{
    int16  dx, dy, p, x, y;
    int16  twoDx, twoDxDy;
    uint8  *pDst;
    uint8  mask, mask2 = 0x7F;

    dx = ABS(x2 - x1);
    dy = ABS(y2 - y1);
    twoDx = dx + dx;
    p = twoDx -dy;
    twoDxDy = p - dy;

    /*Draw first point*/
    pDst = (uint8 *)(pDev->pdata + y1 * pDev->line_bytes + x1 / 8);
    mask = 0xc0 >> (x1 & 7);
    
    *pDst = *pDst & ~mask; //set black color
    if (mask == 0x01)
        *(pDst + 1) &= mask2;

    x = x1;
//    y = y1 + 1;
    y = y1;

    if (dir == NORM_DIR)
    {
        for(; y <= y2; y++)
        {
            if (p < 0)
                p += twoDx;
            else
            {
                p += twoDxDy;
                x++;

                mask >>= 1;
                if (mask == 0)
                {
                    mask = 0xc0;
                    pDst++;
                }
            }

            /*Draw next point*/
            pDst += pDev->line_bytes;
            *pDst = *pDst & ~mask; //set black color
            if (mask == 0x01) 
                *(pDst + 1) &= mask2;
        }
    }
    else //REV_DIR
    {
        mask2 = 0xFE;
        for(; y <= y2; y++)
        {
            if (p < 0)
                p += twoDx;
            else
            {
                p += twoDxDy;
                x--;
/*
                if(mask == 0x01)
                    mask = 0x0c;
                else
                {
                    mask <<= 1;
                    if (mask == 0x80)
                    {
                        mask = 0x01;
                        pDst--;
                    }
                }
*/
                mask <<= 1;
                if(mask == 0x00)
                {
                    mask = 0x03;
                    pDst --;
                }
            }
            /*Draw next point*/
            pDst += pDev->line_bytes;
            *pDst = *pDst & ~mask; //set black color
            if (mask == 0x80)
                *(pDst - 1) &= mask2;
        }
    }

    return 1;
}

static void DDD_SetRect(PRECT prc, int x1, int y1, int x2, int y2)
{
    if (x1 <= x2)
    {
        prc->x1 = x1;
        prc->x2 = x2 + 1;
    }
    else
    {
        prc->x1 = x2;
        prc->x2 = x1 + 1;
    }

    if (y1 <= y2)
    {
        prc->y1 = y1;
        prc->y2 = y2 + 1;
    }
    else
    {
        prc->y1 = y2;
        prc->y2 = y1 + 1;
    }
}

#define JustPoint(x, min, max) (((x) < (min)) ? (min) : (((x) > (max)) ? (max) : (x)))

int MONO_HW_Line(int x1, int y1, int x2, int y2)
{
    PDEV pDev = pDisplayDev;
    int ret, dx, dy;
    int t;
    RECT rc;

//    SetRect(&rc, x1 - 8, y1 - 8, x2 + 8, y2 + 8);
    x1 = JustPoint(x1, DDD_ClipRect.x1, DDD_ClipRect.x2 - 2);
    y1 = JustPoint(y1, DDD_ClipRect.y1 + 1, DDD_ClipRect.y2 - 1);
    x2 = JustPoint(x2, DDD_ClipRect.x1, DDD_ClipRect.x2 - 2);
    y2 = JustPoint(y2, DDD_ClipRect.y1 + 1, DDD_ClipRect.y2 - 1);
    
    DDD_SetRect(&rc, x1, y1, x2, y2);
//    SetRect(&rc, x1, y1, x2, y2);
//    NormalizeRect(&rc);
    _ExcludeCursor(pDev, &rc);

    if (y1 == y2)
    {
        if (x1 == x2)
            return 0;

        if (x1 > x2)
        {
            t = x1;
            x1 = x2++;
            x2 = t;
        }
        else
            x2--;

        ret = MONO_HW_HLine(pDev, x1, x2, y1);
    }
    else if (x1 == x2)
    {
        if (y1 > y2)
        {
            t = y1;
            y1 = y2++;
            y2 = t;
        }
        else
            y2--;

        ret = MONO_HW_VLine(pDev, x1, y1, y2);
    }
    else
    {
        dx = ABS(x2 - x1);
        dy = ABS(y2 - y1);

        if (dx >= dy)  //CtrlX type
        {
            if (x2 < x1) //exchange coordinate
            {
                t = x1, x1 = x2, x2 = t;
                t = y1, y1 = y2, y2 = t;
            }

            if (y2 > y1)
                ret = MONO_HW_XLine(pDev, x1, y1, x2, y2, NORM_DIR);
            else
                ret = MONO_HW_XLine(pDev, x1, y1, x2, y2, REVR_DIR);
        }
        else  //CtrlY type
        {
            if (y2 < y1) //exchange coordinate
            {
                t = x1, x1 = x2, x2 = t;
                t = y1, y1 = y2, y2 = t;
            }

            if (x2 > x1)
                ret = MONO_HW_YLine(pDev, x1, y1, x2, y2, NORM_DIR);
            else
                ret = MONO_HW_YLine(pDev, x1, y1, x2, y2, REVR_DIR);
        }
    }

    _UnexcludeCursor(pDev);
    return ret;
}

BOOL DDD_SetClipRect(PRECT prc)
{
    if (prc == NULL || !IsValidRect(prc))
        return FALSE;

    CopyRect(&DDD_ClipRect, prc);
    NormalizeRect(&DDD_ClipRect);
    return TRUE;
}

uint32 DDD_SetColor(uint32 color)
{
    return 0;
}

int DDD_Line(int x1, int y1, int x2, int y2, int width)
{
    if (width != 2)
        width = 2;

    return MONO_HW_Line(x1, y1, x2, y2);
}
#endif

/*********************************************************************\
* Function	   GetPixel
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int GetPixel(PDEV pDev, PGRAPHPARAM pGraphParam, 
                     PPOINTDATA pDrawData)
{
    uint8* pDst;
    RECT   clip_rect;
    uint8 mask;
    int offset;
    int color;

    if (pDrawData == NULL)
        return 0;
    
    if (pGraphParam == NULL)
    {
        pDrawData->color = 0xFFFFFFFFL;
        return 0;
    }

    /* Get clip rectangle */
    GetDevRect(&clip_rect, pDev); 
    if (pGraphParam->flags & GPF_CLIP_RECT)
        ClipRect(&clip_rect, &pGraphParam->clip_rect);

    if (!IsPointInRect(&clip_rect, pDrawData->x, pDrawData->y))
    {
        pDrawData->color = 0xFFFFFFFFL;
        return 0;
    }

    pDst = GETXYADDR(pDev, pDrawData->x, pDrawData->y);
    offset = pDrawData->x % 8;
    mask = 0x80 >> offset;
    color = (GETPIXEL(pDst) & mask) >> (7 - offset);

    pDrawData->color = (color == 0) ? 0 : 0xFFFFFFL;

    return 0;
}

/*********************************************************************\
* Function	   SetPixel
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int SetPixel(PDEV pDev, PGRAPHPARAM pGraphParam, 
                     PPOINTDATA pDrawData)
{
    uint32 pencolor, pixel;
    uint8   *pDst;
    uint32 rop;
    ROPFUNC ropfunc;
    RECT   clip_rect;
    uint8 mask;

    if (pGraphParam == NULL || pDrawData == NULL)
        return 0;
    /* Get clip rectangle */
    GetDevRect(&clip_rect, pDev); 
    if (pGraphParam->flags & GPF_CLIP_RECT)
        ClipRect(&clip_rect, &pGraphParam->clip_rect);

    if (!IsPointInRect(&clip_rect, pDrawData->x, pDrawData->y))
        return 0;

    _ExcludeCursor(pDev, &clip_rect);

    rop = SRop3ToRop2(pGraphParam->rop);
    pencolor = (RealizeColor(pDev, NULL, pDrawData->color) == 0) 
        ? 0 : 0xFFFFFFFFL;
    pDst = GETXYADDR(pDev, pDrawData->x, pDrawData->y);
    mask = 0x80 >> (pDrawData->x % 8);

    switch(rop)
    {
    case ROP_SRC:

        PUTPIXEL(pDst, pencolor, mask);
        
        break;

    case ROP_SRC_XOR_DST :

        pixel = GETPIXEL(pDst) ^ pencolor;
        PUTPIXEL(pDst, pixel, mask);

        break;
        
    default:

        ropfunc = GetRopFunc(rop);
        pixel = ropfunc(pencolor, GETPIXEL(pDst));
        PUTPIXEL(pDst, pixel, mask);
        
        break;
    }

    pDrawData->color = (pencolor == 0) ? 0 : 0xFFFFFFL;

    _UnexcludeCursor(pDev);
    return 0;
}

/*********************************************************************\
* Function	   DrawPoint
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawPoint(PDEV pDev, PGRAPHPARAM pGraphParam, 
                     PPOINTDATA pDrawData)
{
    return SetPixel(pDev, pGraphParam, pDrawData);
}

/*********************************************************************\
* Function	   DrawLine
* Purpose      
* Params	   
    pDev        Pointer to display device structure.
    pGraphParam Pointer to display device parameter.
    pLineData   Pointer to drawing shape data.
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawLine(PDEV pDev, PGRAPHPARAM pGraphParam, 
                    PLINEDATA pLineData)
{
    ASSERT(pGraphParam != NULL);
    if (pGraphParam->pPen == NULL)
        return 0;

        return (pGraphParam->pPen->width == 1 ? 
            DrawThinLine(pDev, pGraphParam, pLineData) :
            DrawWideLine(pDev, pGraphParam, pLineData));            
}

/*********************************************************************\
* Function	   DrawRect
* Purpose      
* Params	   
    pDev        Pointer to display device structure.
    pGraphParam Pointer to display device parameter.
    pLineData   Pointer to drawing shape data.
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawRect(PDEV pDev, PGRAPHPARAM pGraphParam, 
                    PRECTDATA pRectData)
{
    PPHYSPEN    pPen;
    PPHYSBRUSH  pBrush;
    LINEDATA    linedata;
    RECT        outer_rect, inner_rect, line_rect, clip_rect;
    GRAPHPARAM  GraphParam;
    int         penwidth = 0;

    ASSERT(pGraphParam != NULL);

    pPen = pGraphParam->pPen;
    pBrush = pGraphParam->pBrush;

    if ((pPen != NULL) && (pGraphParam->flags & GPF_PEN))
        penwidth = pPen->width;
    
    outer_rect.x1 = pRectData->x1;
    outer_rect.x2 = pRectData->x2;
    outer_rect.y1 = pRectData->y1;
    outer_rect.y2 = pRectData->y2;

    /* Make rectangle be even rectangle, x1 <= x2, y1 <= y2 */
    if (pRectData->x1 > pRectData->x2)
    {
        outer_rect.x2 = pRectData->x1 + 1;
        outer_rect.x1 = pRectData->x2 + 1;
    }

    if (pRectData->y1 > pRectData->y2)
    {
        outer_rect.y2 = pRectData->y1 + 1;
        outer_rect.y1 = pRectData->y2 + 1;
    }
   
    /* Check pen frame style */
    if ((penwidth > 1) && (pPen->frame_mode == PFM_CENTER))
    {
        outer_rect.x1 -= penwidth / 2;
        outer_rect.x2 += (penwidth - 1) / 2;
        outer_rect.y1 -= penwidth / 2;
        outer_rect.y2 += (penwidth - 1) / 2;   
    }

    /* Get clip rectangle */
    GetDevRect(&clip_rect, pDev); 
    if (pGraphParam->flags & GPF_CLIP_RECT)
        ClipRect(&clip_rect, &pGraphParam->clip_rect);

    /* If draw area not in clip area, return */
    if (!IntersectRect(&outer_rect, &clip_rect))
        return 0;

    if (pPen == NULL)
    {
        inner_rect.x1 = outer_rect.x1;
        inner_rect.x2 = outer_rect.x2;
        inner_rect.y1 = outer_rect.y1;
        inner_rect.y2 = outer_rect.y2;
    }
    else if (pPen->style == PS_NULL)
    {
        inner_rect.x1 = outer_rect.x1;
        inner_rect.x2 = outer_rect.x2 - 1;
        inner_rect.y1 = outer_rect.y1;
        inner_rect.y2 = outer_rect.y2 - 1;
    }
    else
    {
        inner_rect.x1 = outer_rect.x1 + penwidth;
        inner_rect.x2 = outer_rect.x2 - penwidth;
        inner_rect.y1 = outer_rect.y1 + penwidth;
        inner_rect.y2 = outer_rect.y2 - penwidth;
    }

    /* Call PatBlt function to fill the rectangle inner area */
    if (IsValidRect(&inner_rect))
        PatBlt(pDev, &inner_rect, pGraphParam);

    if (penwidth == 0)
        return 0;

    /* Now draw outline of the rectangle using the currrent pen */
    if (penwidth == 1)
    {   
        linedata.x1 = outer_rect.x1;
        linedata.x2 = outer_rect.x2 - 1;
        linedata.y1 = outer_rect.y1;
        linedata.y2 = outer_rect.y1;
        DrawThinLine(pDev, pGraphParam, &linedata);

        linedata.x1 = outer_rect.x2 - 1;
        linedata.y2 = outer_rect.y2 - 1;
        DrawThinLine(pDev, pGraphParam, &linedata);
        
        linedata.x2 = outer_rect.x1;
        linedata.y1 = outer_rect.y2 - 1;
        DrawThinLine(pDev, pGraphParam, &linedata);
        
        linedata.y2 = outer_rect.y1;
        linedata.x1 = outer_rect.x1;
        DrawThinLine(pDev, pGraphParam, &linedata);
        
        return 0;
    }

    /* Now the pen width > 1, use PatBlt to draw the outline */
    GraphParam.flags = GPF_ROP2 | GPF_CLIP_RECT;
    GraphParam.rop = pGraphParam->rop;
    GraphParam.bk_color = pGraphParam->pen_color;
    CopyRect(&GraphParam.clip_rect, &pGraphParam->clip_rect);

    if (!IsValidRect(&inner_rect))
    {   
        /* No inner rectangle, Use pen color to fill the area */
        PatBlt(pDev, &outer_rect, &GraphParam);
        return 0;
    }

    /* Use GE_RectFill to draw thick frame */
    line_rect.x1 = outer_rect.x1;
    line_rect.x2 = outer_rect.x2 - penwidth;
    line_rect.y1 = outer_rect.y1;
    line_rect.y2 = outer_rect.y1 + penwidth;
    PatBlt(pDev, &line_rect, &GraphParam);
    
    line_rect.x1 = outer_rect.x2 - penwidth;
    line_rect.x2 = outer_rect.x2;
    line_rect.y1 = outer_rect.y1;
    line_rect.y2 = outer_rect.y2 - penwidth;
    PatBlt(pDev, &line_rect, &GraphParam);
    
    line_rect.x1 = outer_rect.x1 + penwidth;
    line_rect.x2 = outer_rect.x2;
    line_rect.y1 = outer_rect.y2 - penwidth;
    line_rect.y2 = outer_rect.y2;
    PatBlt(pDev, &line_rect, &GraphParam);
    
    line_rect.x1 = outer_rect.x1;
    line_rect.x2 = outer_rect.x1 + penwidth;
    line_rect.y1 = outer_rect.y1 + penwidth;
    line_rect.y2 = outer_rect.y2;
    PatBlt(pDev, &line_rect, &GraphParam);
    
    return 0;
}
