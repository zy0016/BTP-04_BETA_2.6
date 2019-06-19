/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display Driver 
 *
 * Purpose  : Implement the functions about drawing lines and rects.
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
#if (!SIMPLE_LINECLIP)
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

        pLineData->x1   = x1;
        pLineData->y1   = y1;
        pLineData->x2   = x2;
        pLineData->y2   = y2;

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

        pLineData->x1   = x1;
        pLineData->y1   = y1;
        pLineData->x2   = x2;
        pLineData->y2   = y2;

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
#else //#if (!SIMPLE_LINECLIP)
static int ClipLine(PLINEDATA pLineData, PRECT pClipRect, 
                    LINEDRAW* pLineDraw)
{
    int x1, x2, y1, y2;
    int tx1, tx2, ty1, ty2;     /* The clip result of slope line        */
    int dx, dy;   /* Used to calculate the clip result    */
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
//    twodx = dx * 2;
//    twody = dy * 2;

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
                    /* 2004/07/28 omit for the efficiency */
//                    tx1 -= ((twodx - ((x2 - tx1) * twody + dx) % twodx) + 
//                        twody - 1) / twody - 1;
                }
                else
                {
                    tx1 = x1 + DIV_CEILING((ty1 - y1) * dx, dy);
                    /*
                    ** When x = tx1, y = ty1
                    ** When x = tx1 - 1, y = ty1 - 1.
                    */
                    /* 2004/07/28 omit for the efficiency */
//                    tx1 -= (((tx1 - x1) * twody + dx) % twodx) / twody;
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
                    /* 2004/07/28 omit for the efficiency */
//                    tx2 += (((x2 - tx2) * twody + dx) % twodx) / twody;
                }
                else
                {
                    /*
                    ** When x = tx2, y = ty2
                    ** When x = tx2 + 1, y = ty2 + 1.
                    */
                    tx2 = x1 + DIV_CEILING((ty2 - y1) * dx, dy);
                    /* 2004/07/28 omit for the efficiency */
//                    tx2 += ((twodx - ((tx2 - x1) * twody + dx) % twodx) + 
//                        twody - 1) / twody - 1;
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
                    /* 2004/07/28 omit for the efficiency */
//                    tx1 -= ((twodx - ((x2 - tx1) * twody + dx) % twodx) + 
//                        twody - 1) / twody - 1;
                }
                else
                {
                    /*
                    ** When x = tx1, y = ty1
                    ** When x = tx1 - 1, y = ty1 + 1.
                    */
                    tx1 = x1 + DIV_CEILING((y1 - ty1) * dx, dy);
                    /* 2004/07/28 omit for the efficiency */
//                    tx1 -= (((tx1 - x1) * twody + dx) % twodx) / twody;
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
                    /* 2004/07/28 omit for the efficiency */
//                    tx2 += (((x2 - tx2) * twody + dx) % twodx) / twody;
                }
                else
                {
                    /*
                    ** When x = tx2, y = ty2
                    ** When x = tx2 + 1, y = ty2 - 1.
                    */
                    tx2 = x1 + DIV_CEILING((y1 - ty2) * dx, dy);
                    /* 2004/07/28 omit for the efficiency */
//                    tx2 += ((twodx - ((tx2 - x1) * twody + dx) % twodx) + 
//                        twody - 1) / twody - 1;
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
//                    ty1 -= ((twody - ((y2 - ty1) * twodx + dy) % twody) + 
//                        twodx - 1) / twodx - 1;
                }
                else
                {
                    /*
                    ** When y = ty1, x = tx1
                    ** When y = ty1 - 1, y = tx1 - 1.
                    */
                    ty1 = y1 + DIV_CEILING((tx1 - x1) * dy, dx);
//                    ty1 -= (((ty1 - y1) * twodx + dy) % twody) / twodx;
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
//                    ty2 += (((y2 - ty2) * twodx + dy) % twody) / twodx;
                }
                else
                {
                    /*
                    ** When y = ty2, x = tx2
                    ** When y = ty2 + 1, y = tx2 + 1.
                    */
                    ty2 = y1 + DIV_CEILING((tx2 - x1) * dy, dx);
//                    ty2 += ((twody - ((ty2 - y1) * twodx + dy) % twody) + 
//                        twodx - 1) / twodx - 1;
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
//                    ty1 -= ((twody - ((y2 - ty1) * twodx + dy) % twody) + 
//                        twodx - 1) / twodx - 1;
                }
                else
                {
                    /*
                    ** When y = ty1, x = tx1
                    ** When y = ty1 - 1, y = tx1 + 1.
                    */
                    ty1 = y1 + DIV_CEILING((x1 - tx1) * dy, dx);
//                    ty1 -= (((ty1 - y1) * twodx + dy) % twody) / twodx;
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
//                    ty2 += (((y2 - ty2) * twodx + dy) % twody) / twodx;
                }
                else
                {
                    /*
                    ** When y = ty2, x = tx2
                    ** When y = ty2 + 1, y = tx2 - 1.
                    */
                    ty2 = y1 + DIV_CEILING((x1 - tx2) * dy, dx);
//                    ty2 += ((twody - ((ty2 - y1) * twodx + dy) % twody) + 
//                        twodx - 1) / twodx - 1;
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
#endif //#if (!SIMPLE_LINECLIP)

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
    uint8   *pDst;
    uint32  pixel;
    int     pix_bytes;
    int     i;
    ROPFUNC ropfunc;

    if (x1 > x2)
        EXCHANGE(x1, x2);

    pDst = GETXYADDR(pDev, x1, y);
    pix_bytes = pDev->pix_bits / 8;
    switch (rop)
    {
    case ROP_SRC:

        for(i = x1; i <= x2; i++)
        {
            PUTPIXEL(pDst, color);
            pDst += pix_bytes;
        }
        
        break;

    case ROP_SRC_XOR_DST :

        for(i = x1; i <= x2; i++)
        {
            pixel = GETPIXEL(pDst) ^ color;
            PUTPIXEL(pDst, pixel);
            pDst += pix_bytes;
        }

        break;
        
    default:

        ropfunc = GetRopFunc(rop);
        for(i = x1; i <= x2; i++)
        {
            pixel = ropfunc(color, GETPIXEL(pDst));
            PUTPIXEL(pDst, pixel);
            pDst += pix_bytes;
        }

        break;
    }
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
    uint8   *pDst;
    uint32  pixel;
    int     i;
    int     line_bytes;
    ROPFUNC ropfunc;

    if (y1 > y2)
    {
        EXCHANGE(y1, y2);
    }
    
    line_bytes = pDev->line_bytes;
    pDst = GETXYADDR(pDev, x, y1);

    switch(rop)
    {
    case ROP_SRC :

        for (i = y1; i <= y2; i++)
        {
            PUTPIXEL(pDst, color);
            pDst += line_bytes;
        }

        break;

    case ROP_SRC_XOR_DST :

        for (i = y1; i <= y2; i++)
        {
            pixel = GETPIXEL(pDst) ^ color;
            PUTPIXEL(pDst, pixel);
            pDst += line_bytes;
        }

        break;

    default :

        ropfunc = GetRopFunc(rop);

        for (i = y1; i <= y2; i++)
        {
            pixel = ropfunc(color, GETPIXEL(pDst));
            PUTPIXEL(pDst, pixel);
            pDst += line_bytes;
        }

        break;
    }
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
    int     pix_bytes, line_bytes;
    uint32  rop;
    ROPFUNC ropfunc;
    int     i;
    int     x1, y1, x2, y2;
    int     dx, dy, twoDy, twoDyDx;
    int     flagBresenham;

    ASSERT(pGraphParam != NULL);
    if (pGraphParam->pPen == NULL)
        return;

    rop = SRop3ToRop2(pGraphParam->rop);

    x1  = pLineDraw->start_x;
    y1  = pLineDraw->start_y;
    x2  = pLineDraw->end_x;
    y2  = pLineDraw->end_y;

//    pencolor    = pGraphParam->pPen->fgcolor;
    pencolor = pGraphParam->pen_color;
    pDst    = GETXYADDR(pDev, x1, y1);

//    line_bytes  = (y1 > y2) ? -pDev->line_bytes : pDev->line_bytes;
    line_bytes  = (pLineDraw->dir == REVR_DIR) ? 
        -pDev->line_bytes : pDev->line_bytes;

    pix_bytes   = (x1 > x2) ? -pDev->pix_bits / 8 : pDev->pix_bits / 8;
    
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

        for (i = x1; i <= x2; i ++)
        {
            PUTPIXEL(pDst, pencolor);

            if (flagBresenham < 0)
            {
                flagBresenham += twoDy;
            }
            else
            {
                pDst += line_bytes;
                flagBresenham += twoDyDx;
            }

            pDst += pix_bytes;
        }

        break;

    default:

        ropfunc = GetRopFunc(rop);

        for (i = x1; i <= x2; i ++)
        {
            pixel = ropfunc(pencolor, GETPIXEL(pDst));
            PUTPIXEL(pDst, pixel);
            
            if (flagBresenham < 0)
            {
                flagBresenham += twoDy;
            }
            else
            {
                pDst += line_bytes;
                flagBresenham += twoDyDx;
            }

            pDst += pix_bytes;
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
    int     pix_bytes, line_bytes;
    uint32  rop;
    ROPFUNC ropfunc;
    int     i;
    int     x1, y1, x2, y2;
    int     dx, dy, twoDy, twoDyDx;
    int     flagBresenham;

    ASSERT(pGraphParam != NULL);
    if (pGraphParam->pPen == NULL)
        return;

    rop = SRop3ToRop2(pGraphParam->rop);

    x1  = pLineDraw->start_x;
    y1  = pLineDraw->start_y;
    x2  = pLineDraw->end_x;
    y2  = pLineDraw->end_y;

//    pencolor    = pGraphParam->pPen->fgcolor;
    pencolor = pGraphParam->pen_color;
    pDst    = GETXYADDR(pDev, x1, y1);

    line_bytes  = (y1 > y2) ? -pDev->line_bytes : pDev->line_bytes;
    
//    pix_bytes   = (x1 > x2) ? -pDev->pix_bytes : pDev->pix_bytes;
    pix_bytes  = (pLineDraw->dir == REVR_DIR) ?
        -pDev->pix_bits / 8 : pDev->pix_bits / 8;

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

        for (i = y1; i <= y2; i ++)
        {
            PUTPIXEL(pDst, pencolor);

            if (flagBresenham < 0)
            {
                flagBresenham += twoDy;
            }
            else
            {
                pDst += pix_bytes;
                flagBresenham += twoDyDx;
            }

            pDst += line_bytes;
        }

        break;

    default:

        ropfunc = GetRopFunc(rop);

        for (i = y1; i <= y2; i ++)
        {
            pixel = ropfunc(pencolor, GETPIXEL(pDst));
            PUTPIXEL(pDst, pixel);
            
            if (flagBresenham < 0)
            {
                flagBresenham += twoDy;
            }
            else
            {
                pDst += pix_bytes;
                flagBresenham += twoDyDx;
            }

            pDst += line_bytes;
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
    int     pix_bytes;
    uint32  pixel;
    uint32  bkcolor, pencolor;
    uint32  penmask, penpattern;
    uint32  rop;
    ROPFUNC ropfunc;
    ROPFUNC ropDashfunc;
    int     i;

    ASSERT(pGraphParam != NULL);
    if (pGraphParam->pPen == NULL)
        return;

    rop = SRop3ToRop2(pGraphParam->rop);

    pDst = GETXYADDR(pDev, x1, y);

    if (x1 > x2)
    {
        EXCHANGE(x1, x2);
        pix_bytes = -pDev->pix_bits / 8;
    }
    else
    {
        pix_bytes = pDev->pix_bits / 8;
    }

    penmask = 0x80000000L;
    penpattern = penstyle.pattern;
    bkcolor = pGraphParam->bk_color;
//    pencolor = pGraphParam->pPen->fgcolor;
    pencolor = pGraphParam->pen_color;

    ropfunc = GetRopFunc(rop);
    if (pGraphParam->bk_mode == BM_TRANSPARENT)
        ropDashfunc = GetRopFunc(ROP_DST);
    else
        ropDashfunc = ropfunc;

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

        PUTPIXEL(pDst, pixel);
        pDst += pix_bytes;
        
        penmask >>= 1;
        if ((penmask >> (32 - penstyle.count)) == 0)
        {
            penmask = 0x80000000L;
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
    uint32  pixel;
    uint32  bkcolor, pencolor;
    uint32  penmask, penpattern;
    uint32 rop;
    ROPFUNC ropfunc;
    ROPFUNC ropDashfunc;
    int     i;

    ASSERT(pGraphParam != NULL);
    if (pGraphParam->pPen == NULL)
        return;

    rop = SRop3ToRop2(pGraphParam->rop);

    pDst = GETXYADDR(pDev, x, y1);
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
    bkcolor = pGraphParam->bk_color;
//    pencolor = pGraphParam->pPen->fgcolor;
    pencolor = pGraphParam->pen_color;

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

        PUTPIXEL(pDst, pixel);
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
    int     pix_bytes, line_bytes;
    uint32  pixel;
    uint32  bkcolor, pencolor;
    uint32  penmask, penpattern;
    uint32  rop;
    ROPFUNC ropfunc;
    ROPFUNC ropDashfunc;
    int     i;
    int     x1, y1, x2, y2;
    int     dx, dy, twoDy, twoDyDx;
    int     flagBresenham;

    ASSERT(pGraphParam != NULL);
    if (pGraphParam->pPen == NULL)
        return;

    rop = SRop3ToRop2(pGraphParam->rop);

    x1  = pLineDraw->start_x;
    y1  = pLineDraw->start_y;
    x2  = pLineDraw->end_x;
    y2  = pLineDraw->end_y;

    penmask = 0x80000000L;
    penpattern = penstyle.pattern;
//    pencolor    = pGraphParam->pPen->fgcolor;
    pencolor = pGraphParam->pen_color;
    bkcolor     = pGraphParam->bk_color;

    pDst    = GETXYADDR(pDev, x1, y1);

//    line_bytes  = (y1 > y2) ? -pDev->line_bytes : pDev->line_bytes;
    line_bytes  = (pLineDraw->dir == REVR_DIR) ? 
        -pDev->line_bytes : pDev->line_bytes;

    pix_bytes   = (x1 > x2) ? -pDev->pix_bits / 8 : pDev->pix_bits / 8;

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
            for (i = x1; i <= x2; i ++)
            {
                if (penmask & penpattern)
                {
                    PUTPIXEL(pDst, pencolor);
                }
                else
                {
                    PUTPIXEL(pDst, bkcolor);
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
                pDst += pix_bytes;

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
                    PUTPIXEL(pDst, pencolor);
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
                pDst += pix_bytes;

                penmask >>= 1;
                if ((penmask >> (32 - penstyle.count)) == 0)
                {
                    penmask = 0x80000000L;
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
            PUTPIXEL(pDst, pixel);

            if (flagBresenham < 0)
            {
                flagBresenham += twoDy;
            }
            else
            {
                pDst += line_bytes;
                flagBresenham += twoDyDx;
            }
            pDst += pix_bytes;

            penmask >>= 1;
            if ((penmask >> (32 - penstyle.count)) == 0)
            {
                penmask = 0x80000000L;
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
    int     pix_bytes, line_bytes;
    uint32  pixel;
    uint32  bkcolor, pencolor;
    uint32  penmask, penpattern;
    uint32  rop;
    ROPFUNC ropfunc;
    ROPFUNC ropDashfunc;
    int     i;
    int     x1, y1, x2, y2;
    int     dx, dy, twoDy, twoDyDx;
    int     flagBresenham;

    ASSERT(pGraphParam != NULL);
    if (pGraphParam->pPen == NULL)
        return;

    rop = SRop3ToRop2(pGraphParam->rop);

    x1  = pLineDraw->start_x;
    y1  = pLineDraw->start_y;
    x2  = pLineDraw->end_x;
    y2  = pLineDraw->end_y;

    penmask = 0x80000000L;
    penpattern = penstyle.pattern;
//    pencolor    = pGraphParam->pPen->fgcolor;
    pencolor = pGraphParam->pen_color;
    bkcolor     = pGraphParam->bk_color;

    pDst    = GETXYADDR(pDev, x1, y1);

    line_bytes  = (y1 > y2) ? -pDev->line_bytes : pDev->line_bytes;

//    pix_bytes   = (x1 > x2) ? -pDev->pix_bytes : pDev->pix_bytes;
    pix_bytes  = (pLineDraw->dir == REVR_DIR) ?
        -pDev->pix_bits / 8 : pDev->pix_bits / 8;
    
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
            for (i = y1; i <= y2; i ++)
            {
                if (penmask & penpattern)
                {
                    PUTPIXEL(pDst, pencolor);
                }
                else
                {
                    PUTPIXEL(pDst, bkcolor);
                }

                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    pDst += pix_bytes;
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
                    PUTPIXEL(pDst, pencolor);
                }

                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    pDst += pix_bytes;
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

    default:

        ropfunc = GetRopFunc(rop);
        if (pGraphParam->bk_mode == BM_TRANSPARENT)
            ropDashfunc = GetRopFunc(ROP_DST);
        else
            ropDashfunc = ropfunc;

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
            PUTPIXEL(pDst, pixel);

            if (flagBresenham < 0)
            {
                flagBresenham += twoDy;
            }
            else
            {
                pDst += pix_bytes;
                flagBresenham += twoDyDx;
            }
            pDst += line_bytes;

            penmask >>= 1;
            if ((penmask >> (32 - penstyle.count)) == 0)
            {
                penmask = 0x80000000L;
            }
        } 

        break;
    }
}
#endif

/*********************************************************************\
* Function	   DrvDrawThinLine
* Purpose      Draw thin Line use GE function
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrvDrawThinLine(PDEV pDev, PGRAPHPARAM pGraphParam, 
                        PLINEDATA pLineData)
{
    return 0;
}

/*********************************************************************\
* Function	   DrvDrawWideLine
* Purpose      Draw wide Line use GE function
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrvDrawWideLine(PDEV pDev, PGRAPHPARAM pGraphParam, 
                        PLINEDATA pLineData)
{
    return 0;
}

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

    ASSERT(pGraphParam != NULL);
    if (pGraphParam->pPen == NULL)
        return 0;
    if (pGraphParam->pPen->style == PS_NULL)
        return 0;

    if (pGraphParam->flags & GPF_CLIP_RECT)
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
    int     dx, dy, twoDy, twoDyDx;
    int     flagBresenham, LineType, line_bytes, pix_bytes;
    RECT    rcLine, rc;
    LINEDATA line;
    int     x1, x2, y1, y2, i;
    LINEDRAW LineDraw;

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
    {   /* Other direction, use emulate to draw.ожн╢й╣ож */
        //return 0;
        line.x1 = pLineData->x1;
        line.y1 = pLineData->y1;
        line.x2 = pLineData->x2;
        line.y2 = pLineData->y2;
        SetRect(&rcLine, MIN(line.x1, line.x2), 
            MIN(line.y1, line.y2), 
            MAX(line.x1, line.x2) + 1, 
            MAX(line.y1, line.y2) + 1);
        CopyRect(&rc, &pGraphParam->clip_rect);
        ClipRect(&rcLine, &rc);
        if (!IsValidRect(&rcLine))
            return -1;
        LineType = ClipLine(&line, &rc, &LineDraw);
        if (LineType == 0)
            return 0;

        x1 = line.x1;
        x2 = line.x2;
        y1 = line.y1;
        y2 = line.y2;
        
        LineType = ClipLine(&line, &rc, &LineDraw);
        line_bytes = (LineDraw.dir == REVR_DIR) ? 
            -pDev->line_bytes : pDev->line_bytes;
        pix_bytes = (x1 > x2) ?
            -pDev->pix_bits / 8 : pDev->pix_bits / 8;
        dx = LineDraw.step;
        dy = LineDraw.delta;
        twoDy = dy * 2;
        flagBresenham = twoDy - dx;
        twoDyDx = flagBresenham - dx;
        
        if (LineDraw.cliped != 0)
        {
            flagBresenham = (LineDraw.cliped) * twoDy + dx;
            flagBresenham %= 2 * dx;
            flagBresenham += twoDy;
            flagBresenham -= 2 * dx;
        }
        if (abs(x2 - x1) >= abs(y2 - y1) )
        {
            if (x1 > x2)
            {
                EXCHANGE(x1, x2);
                EXCHANGE(y1, y2);
            }
            
            for( i = x1; i <= x2; i++)
            {
                LineRC.y1 = y1;
                LineRC.y2 = y1;
                LineRC.x1 = i;
                LineRC.x2 = i;
                
                LineRC.x1 -= width / 2;
                LineRC.x2 += width - width / 2;
                LineRC.y1 -= width / 2;
                LineRC.y2 += width - width / 2;
                
                ClipByDev(&LineRC, pDev);
                if (!IsValidRect(&LineRC))
                    return -1;
                
                if ((pGraphParam->flags & GPF_CLIP_RECT) && 
                    !ClipRect(&LineRC, &pGraphParam->clip_rect))
                    return -1;
                
                _ExcludeCursor(pDev, &LineRC);
                FillSolidRect(pDev, &LineRC, 
                    pGraphParam->pen_color, rop);
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    flagBresenham += twoDyDx;
                    if (y1 < y2)
                        y1 ++;
                    else
                        y1 --;
                }
            }
        }
        else
        {
            if (y1 > y2)
            {
                EXCHANGE(x1, x2);
                EXCHANGE(y1, y2);
            }
            
            for( i = y1; i <= y2; i++)
            {
                LineRC.y1 = i;
                LineRC.y2 = i;
                LineRC.x1 = x1;
                LineRC.x2 = x1;
                
                LineRC.x1 -= width / 2;
                LineRC.x2 += width - width / 2;
                LineRC.y1 -= width / 2;
                LineRC.y2 += width - width / 2;
                
                ClipByDev(&LineRC, pDev);
                if (!IsValidRect(&LineRC))
                    return -1;
                
                if ((pGraphParam->flags & GPF_CLIP_RECT) && 
                    !ClipRect(&LineRC, &pGraphParam->clip_rect))
                    return -1;
                
                _ExcludeCursor(pDev, &LineRC);
               FillSolidRect(pDev, &LineRC, 
                    pGraphParam->pen_color, rop);
                if (flagBresenham < 0)
                {
                    flagBresenham += twoDy;
                }
                else
                {
                    flagBresenham += twoDyDx;
                    if (x1 < x2)
                        x1 ++;
                    else
                        x1 --;
                }
            }
        }
    }

    _UnexcludeCursor(pDev);

    return 0;
}

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
    uint32 color;

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
    color = GETPIXEL(pDst);

    RealizeColorToRGB(pDev, (uint8*)&color, &(pDrawData->color), 1);

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
    pencolor = RealizeColor(pDev, NULL, pDrawData->color);
    pDst = GETXYADDR(pDev, pDrawData->x, pDrawData->y);

    switch(rop)
    {
    case ROP_SRC:

        PUTPIXEL(pDst, pencolor);
        
        break;

    case ROP_SRC_XOR_DST :

        pixel = GETPIXEL(pDst) ^ pencolor;
        PUTPIXEL(pDst, pixel);

        break;
        
    default:

        ropfunc = GetRopFunc(rop);
        pixel = ropfunc(pencolor, GETPIXEL(pDst));
        PUTPIXEL(pDst, pixel);
        
        break;
    }

    RealizeColorToRGB(pDev, (uint8*)&pencolor, &(pDrawData->color), 1);

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

    if (pDev->mode != DEV_PHYSICAL)
    {
        return (pGraphParam->pPen->width == 1 ? 
            DrawThinLine(pDev, pGraphParam, pLineData) :
            DrawWideLine(pDev, pGraphParam, pLineData));            
    }

    if (pGraphParam->pPen->width == 1)
    {
        return DrawThinLine(pDev, pGraphParam, pLineData);
    }
    else
    {
        return DrawWideLine(pDev, pGraphParam, pLineData);
    }
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
