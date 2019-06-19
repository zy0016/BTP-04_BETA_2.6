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



#ifdef CIRCLE_SUPPORTED

/*********************************************************************\
* Function	  DrawCircle 
* Purpose     Draw Circle with various of methods 
* Params	   
    pDev        Pointer to display device structure.
    pGraphParam Pointer to display device parameter.
    pCircleData Pointer to drawing shape data.
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawCircle(PDEV pDev, PGRAPHPARAM pGraphParam, 
                      PCIRCLEDATA pCircleData)
{
    ASSERT(pDev != NULL && pGraphParam != NULL);

    if (pGraphParam->pPen == NULL)
        return DrawThinCircle(pDev, pGraphParam, pCircleData);

    if (pGraphParam->pPen->width > 1)
    {
        return DrawWideCircle(pDev, pGraphParam, pCircleData);
    }
    else if (pGraphParam->pPen->style != PS_SOLID && 
        pGraphParam->pPen->style != PS_NULL)
    {
        return DrawPatThinCircle(pDev, pGraphParam, pCircleData);
    }
    else
    {
        return DrawThinCircle(pDev, pGraphParam, pCircleData);
    }
}

/*********************************************************************\
* Function	  DrawThinCircle 
* Purpose     Draw thin ellipse with width = 1.Support the pattern brush
* Params	   
    pDev        Pointer to display device structure.
    pGraphParam Pointer to display device parameter.
    pCircleData Pointer to drawing shape data.
* Return	 	   
* Remarks	  below is the algorithm. 

**************************************************************
 Draw solid line circle
 Support the pattern brush
**************************************************************
** Using the Bresenham method to draw circle
** devide the total circle line to eight parts and draw the outline
** BR,RB,UL,LU:  means right of bottom, bottom of right, left of up, up of 
**               left 
** devide the total circle area to four parts and filled the inner of circle 
** Below is the four parts
**                   ______
** up:_____________ /__4___\      
**                 /        \    
** third: ________|____3_____|
** second:        |          |
**      ___________\___2____/    
** bottom:          \__1___/    
**                             
** yLast:        the y value used for draw the previous point. If the 
**               current y is changed, then draw the fill brush line 
**               and only be used in drawing left and right part of circle.
** 
** rcClip:       the clip rect of a circle with (0, 0) as the center
** rcRealClip:   the clip rect of the circle with (cx, cy) as the center
** PatBltLineInCircle: draw the brush filled area
**
**********************************************************************/
static int DrawThinCircle(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PCIRCLEDATA pCircleData)
{
    int     x, y, cx, cy, r, yLast;
    int     nScanLeft, nScanRight;
    int32   flagBresenham;
    uint32   rop;
    RECT    rcClip, rcRealClip;
    uint32  pencolor, pixel;
    uint8   *pDst;
    ROPFUNC ropfunc;

    ASSERT(pDev != NULL && pGraphParam != NULL);

    if (pCircleData == NULL || pGraphParam->pPen == NULL)
        return 0;

    if (pCircleData->r == 1)
    {
        LINEDATA LineData;
        LineData.x1 = pCircleData->x;
        LineData.x2 = LineData.x1 + 1;
        LineData.y1 = LineData.y2 = pCircleData->y;

        return DrawThinLine(pDev, pGraphParam, &LineData);
    }
    
    cx = pCircleData->x;
    cy = pCircleData->y;
    r  = pCircleData->r - 1;
    if (r <= 0)
        return 0;

    rcClip.x1 = cx - r;
    rcClip.x2 = cx + r + 1;
    rcClip.y1 = cy - r;
    rcClip.y2 = cy + r + 1;

//    pencolor = pGraphParam->pPen->fgcolor;
    pencolor = pGraphParam->pen_color;
    if (pGraphParam->pPen->style == PS_NULL)
    {
        ropfunc = GetRopFunc(ROP_DST);
    }
    else
    {
        rop = SRop3ToRop2(pGraphParam->rop);
        ropfunc = GetRopFunc(rop);
    }

    if (pGraphParam->flags & GPF_CLIP_RECT)
        ClipRect(&rcClip, &pGraphParam->clip_rect);
    ClipByDev(&rcClip, pDev);
    if (!IsValidRect(&rcClip))
        return 0;

    _ExcludeCursor(pDev, &rcClip);

    CopyRect(&rcRealClip, &rcClip);
    OffsetRect(&rcClip, -cx, -cy);
    
    nScanLeft   = rcRealClip.x1;
    nScanRight  = rcRealClip.x2 - 1;

    x = 0;
    y = r;
    flagBresenham = 3 - (y * 2);
    yLast = y;

    /* draw first point
    ** the first points are the points at the bottom, up, left and right 
    ** point then the four points will be draw only once, it's required
    */
    {
        if (IsPointInRect(&rcClip, 0, y))  //BR Bottem Right
        {
            pDst = GETXYADDR(pDev, cx + x,  cy + y);
            pixel = ropfunc(pencolor, GETPIXEL(pDst));
            PUTPIXEL(pDst, pixel);
        }
    
        if (IsPointInRect(&rcClip, 0, -y)) //UL
        {
            pDst = GETXYADDR(pDev, cx - x, cy - y);
            pixel = ropfunc(pencolor, GETPIXEL(pDst));
            PUTPIXEL(pDst, pixel);
        }

        if (IsPtInYClip(rcClip, 0))
        {
            if (IsPtInXClip(rcClip, y)) //RU
            {
                pDst = GETXYADDR(pDev, cx + y, cy - x);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanRight = cx + y - 1;
            }
    
            if (IsPtInXClip(rcClip, -y)) //LB
            {
                pDst = GETXYADDR(pDev, cx - y, cy + x);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanLeft = cx - y + 1;
            }

            PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        yLast = y;
        if (flagBresenham < 0)
            flagBresenham = flagBresenham + (x * 4) + 6;
        else
        {
            flagBresenham = flagBresenham + ((x - y) * 4) + 10;
            y--;
        }
        x++;
    } // draw first point

    while (x < y)
    {
        //draw bottom
        if (IsPtInYClip(rcClip, y) && !IsXOutOfRect(rcClip, x, -x))
        {
            if (IsPtInXClip(rcClip, x))  //BR
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, -x)) //BL
            {
                pDst = GETXYADDR(pDev,  cx - x , cy + y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanLeft = cx - x + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + y, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        //draw second
        if (IsPtInYClip(rcClip, x) && !IsXOutOfRect(rcClip, y, -y))
        {
            if (IsPtInXClip(rcClip, y)) //RB
            {
                pDst = GETXYADDR(pDev, cx + y, cy + x);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanRight = cx + y - 1;
            }

            if (IsPtInXClip(rcClip, -y)) //LB
            {
                pDst = GETXYADDR(pDev, cx - y, cy + x);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanLeft = cx - y + 1;
            }

            PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + x, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        //draw third
        if (IsPtInYClip(rcClip, -x) && !IsXOutOfRect(rcClip, y, -y))
        {
            if (IsPtInXClip(rcClip, y)) //RU
            {
                pDst = GETXYADDR(pDev, cx + y, cy - x);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanRight = cx + y - 1;
            }

            if (IsPtInXClip(rcClip, -y)) //LU
            {
                pDst = GETXYADDR(pDev, cx - y, cy - x);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanLeft = cx - y + 1;
            }

            PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy - x, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        //draw up
        if (IsPtInYClip(rcClip, -y) && !IsXOutOfRect(rcClip, x, -x))
        {
            if (IsPtInXClip(rcClip, x)) //UR
            {
                pDst = GETXYADDR(pDev, cx + x , cy - y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, -x)) //UL
            {
                pDst = GETXYADDR(pDev, cx - x, cy - y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanLeft = cx - x + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy - y, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        yLast = y;

        if (flagBresenham < 0)
            flagBresenham = flagBresenham + (x * 4) + 6;
        else
        {
            flagBresenham = flagBresenham + ((x - y) *4) + 10;
            y--;
        }
        x++;
    } // while (x < y)

    //x = y, draw the intersect point of two direction drawing only once
    if (x == y)
    {
        if (IsPtInYClip(rcClip, y) && !IsXOutOfRect(rcClip, x, -x))
        {
            if (IsPtInXClip(rcClip, x))
            {
                pDst = GETXYADDR(pDev, cx + x, cy + y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, -x))
            {
                pDst = GETXYADDR(pDev, cx - x, cy + y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanLeft = cx - x + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + y, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        if (IsPtInYClip(rcClip, -y) && !IsXOutOfRect(rcClip, x, -x))
        {
            if (IsPtInXClip(rcClip, x))
            {
                pDst = GETXYADDR(pDev, cx + x, cy - y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, -x))
            {
                pDst = GETXYADDR(pDev, cx - x, cy - y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel);
                nScanLeft = cx - x + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy - y, 
                pGraphParam);
        }
    } // draw x = y point

    _UnexcludeCursor(pDev);

    return 0;
}

/*********************************************************************\
* Function	  DrawPatThinCircle 
* Purpose     Draw thin circle with width = 1 and using style pen. 
              Support the pattern brush 
* Params	   
    pDev        Pointer to display device structure.
    pGraphParam Pointer to display device parameter.
    pCircleData Pointer to drawing shape data.
* Return	 	   
* Remarks	  below is the algorithm. 
**************************************************************
** Using the Bresenham method to draw circle
** devide the total circle line to eight parts and draw the outline
** BR,RB,UL,LU:  means right of bottom, bottom of right, left of up, up of 
**               left 
** devide the total circle area to four parts and filled the inner of circle 
** Below is the four parts
**                   ______
** up:_____________ /__1___\      
**                 /        \    
** second:________|____2_____|
** third:         |          |
**      ___________\___3____/    
** bottom:          \__4___/    
**                             
** yLast:        the y value used for draw the previous point. If the 
**               current y is changed, then draw the fill brush line 
**               and only be used in drawing left and right part of circle.
** 
** PatBltLineInCircle: draw the brush filled area
** 
** nLoopNum:    the number of points when draw one fourth of the total 
**              circle
**********************************************************************/
static int DrawPatThinCircle(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PCIRCLEDATA pCircleData)
{
    int     x, y, cx, cy, r, nLoopNum, yLast;
    int     nScanLeft, nScanRight;
    int32   flagBresenham;
    uint32   rop;
    RECT    rcClip, rcRealClip;
    uint32  pencolor, bkcolor;
    PENSTYLE    penstyle;
    uint32  penpattern, penmask = 0x80000000L;
    int     pencount;
    uint8   *pDst;
    uint32  pixel;
    ROPFUNC ropfunc, ropDashfunc;

    ASSERT(pDev != NULL && pGraphParam != NULL);
    ASSERT(pGraphParam->pPen != NULL);

    if (pCircleData == NULL)
        return 0;

    cx = pCircleData->x;
    cy = pCircleData->y;
    r  = pCircleData->r - 1;

    rcClip.x1 = cx - r;
    rcClip.x2 = cx + r + 1;
    rcClip.y1 = cy - r;
    rcClip.y2 = cy + r + 1;

//    pencolor = pGraphParam->pPen->fgcolor;
    pencolor = pGraphParam->pen_color;
    penstyle = GetPenStyle(pGraphParam->pPen->style);
    penpattern = penstyle.pattern;
    pencount = penstyle.count;
    bkcolor  = pGraphParam->bk_color;

    rop = SRop3ToRop2(pGraphParam->rop);    
    ropfunc = GetRopFunc(rop);

    if (pGraphParam->bk_mode == BM_TRANSPARENT)
        ropDashfunc = GetRopFunc(ROP_DST);
    else
        ropDashfunc = GetRopFunc(ROP_SRC);

    if (pGraphParam->flags & GPF_CLIP_RECT)
        ClipRect(&rcClip, &pGraphParam->clip_rect);
    ClipByDev(&rcClip, pDev);
    if (!IsValidRect(&rcClip))
        return 0;
    
    _ExcludeCursor(pDev, &rcClip);

    CopyRect(&rcRealClip, &rcClip);
    OffsetRect(&rcClip, -cx, -cy);

    nScanLeft = rcRealClip.x1;
    nScanRight = rcRealClip.x2 - 1;

    x = 0;
    y = r;
    flagBresenham = 3 - (y * 2);

    nLoopNum = 0;   //record one fourth of the total circle
    while (x < y)
    {
        if (flagBresenham < 0)
            flagBresenham = flagBresenham + (x * 4) + 6;
        else
        {
            flagBresenham = flagBresenham + ((x - y) * 4) + 10;
            y--;
        }
        x++;
        nLoopNum ++;
    }
    nLoopNum *= 2;
    if (x == y)
    {
        nLoopNum++;
    }
    nLoopNum--;

    x = 0;
    y = r;             
    flagBresenham = 3 - (y * 2);
    yLast = y;
    /* draw first point
    ** the first points are the points at the bottom, up, left and right 
    ** point then the four points will be draw only once, it's required
    */
    {
        if (IsPointInRect(&rcClip, 0, y))  //BR Bottem Right
        {
            pDst = GETXYADDR(pDev, cx + x,  cy + y);

            if ((penmask >> (x % pencount)) & penpattern)
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
            else
                pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));

            PUTPIXEL(pDst, pixel);
        }
    
        if (IsPointInRect(&rcClip, 0, -y)) //UL
        {
            pDst = GETXYADDR(pDev, cx - x, cy - y);

            if ((penmask >> (((nLoopNum << 1) + x) % pencount)) & 
                penpattern)
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
            else
                pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));

            PUTPIXEL(pDst, pixel);
        }

        if (IsPtInYClip(rcClip, 0))
        {
            if (IsPtInXClip(rcClip, y)) //RU
            {
                pDst = GETXYADDR(pDev, cx + y, cy - x);
                
                if ((penmask >> ((nLoopNum - x) % pencount)) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel);
                nScanRight = cx + y - 1;
            }
            
            if (IsPtInXClip(rcClip, -y)) //LB
            {
                pDst = GETXYADDR(pDev, cx - y, cy + x);

                if ((penmask >> (((nLoopNum << 1) + nLoopNum + x) % 
                    pencount)) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel);
                nScanLeft = cx - y + 1;
            }

            PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        yLast = y;

        if (flagBresenham < 0)
            flagBresenham = flagBresenham + (x * 4) + 6;
        else
        {
            flagBresenham = flagBresenham + ((x - y) * 4) + 10;
            y--;
        }
        x++;
    }//draw first point
    
    while (x < y)
    {
        //draw bottom of circle
        if (IsPtInYClip(rcClip, y) && !IsXOutOfRect(rcClip, x, -x))
        {
            if (IsPtInXClip(rcClip, x))  //BR Bottem Right
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + y);

                if ((penmask >> (x % pencount)) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel); 
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, -x)) //BL
            {
                pDst = GETXYADDR(pDev,  cx - x , cy + y);

                if ((penmask >> (((nLoopNum * 4) - x) % pencount)) & 
                    penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel);
                nScanLeft = cx - x + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + y, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        // draw second part of circle
        if (IsPtInYClip(rcClip, x) && !IsXOutOfRect(rcClip, y, -y))
        {
            if (IsPtInXClip(rcClip, y)) //RB
            {
                pDst = GETXYADDR(pDev, cx + y, cy + x);

                if ((penmask >> ((nLoopNum - x) % pencount)) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel);
                nScanRight = cx + y - 1;
            }

            if (IsPtInXClip(rcClip, -y)) //LB
            {
                pDst = GETXYADDR(pDev, cx - y, cy + x);

                if ((penmask >> ((nLoopNum * 3 + x) % pencount))
                    & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel); 
                nScanLeft = cx - y + 1;
            }
            
            PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + x, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;            
        }

        //draw third part of circle
        if (IsPtInYClip(rcClip, -x) && !IsXOutOfRect(rcClip, y, -y))
        {
            if (IsPtInXClip(rcClip, y)) //RU
            {
                pDst = GETXYADDR(pDev, cx + y, cy - x);

                if ((penmask >> ((nLoopNum + x) % pencount)) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel); 
                nScanRight = cx + y - 1;
            }

            if (IsPtInXClip(rcClip, -y)) //LU
            {
                pDst = GETXYADDR(pDev, cx - y, cy - x);

                if ((penmask >> ((nLoopNum * 3 - x) % pencount))
                    & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel); 
                nScanLeft = cx - y + 1;
            }

            PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy - x, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        // draw up part of circle
        if (IsPtInYClip(rcClip, -y) && !IsXOutOfRect(rcClip, x, -x))
        {
            if (IsPtInXClip(rcClip, x)) //UR
            {
                pDst = GETXYADDR(pDev, cx + x , cy - y);

                if ((penmask >> ((nLoopNum * 2 - x) % pencount)) & 
                    penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, -x)) //UL
            {
                pDst = GETXYADDR(pDev, cx - x, cy - y);

                if ((penmask >> ((nLoopNum * 2 + x) % pencount)) & 
                    penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel);
                nScanLeft = cx - x + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy - y, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        yLast = y;
        if (flagBresenham < 0)
        {
            flagBresenham = flagBresenham + (x * 4) + 6;
        }
        else
        {
            flagBresenham = flagBresenham + ((x - y) * 4) + 10;
            y--;
        }
        x++;
    }// while (x < y)

    //x = y, draw the intersect point of two direction drawing only once
    if (x == y)
    {
        if (IsPtInYClip(rcClip, y) && !IsXOutOfRect(rcClip, y, -y))
        {
            if (IsPtInXClip(rcClip, x))
            {
                pDst = GETXYADDR(pDev, cx + x, cy + y);

                if ((penmask >> (x % pencount)) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));

                PUTPIXEL(pDst, pixel);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, -x))
            {
                pDst = GETXYADDR(pDev, cx - x, cy + y);

                if ((penmask >> ((nLoopNum * 4 - x) % pencount)) & 
                    penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));

                PUTPIXEL(pDst, pixel);
                nScanLeft = cx - x + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + y, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        if (IsPtInYClip(rcClip, -y) && !IsXOutOfRect(rcClip, y, -y))
        {
            if (IsPtInXClip(rcClip, x))
            {
                pDst = GETXYADDR(pDev, cx + x, cy - y);

                if ((penmask >> ((nLoopNum * 2 - x) % pencount)) & 
                    penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, -x))
            {
                pDst = GETXYADDR(pDev, cx - x, cy - y);

                if ((penmask >> ((nLoopNum * 2 + x) % pencount)) & 
                    penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel);
                nScanLeft = cx - x + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy - y, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

    } // draw x = y point

    _UnexcludeCursor(pDev);
    return 0;
}

/*********************************************************************\
* Function	  DrawWideCircle 
* Purpose     Draw circle with width > 1.
* Params	   
    pDev        Pointer to display device structure.
    pGraphParam Pointer to display device parameter.
    pCircleData Pointer to drawing shape data.
* Return	 	   
* Remarks	  
**********************************************************************/
static int DrawWideCircle(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PCIRCLEDATA pCircleData)
{
    int16 penwidth, penstyle;
    int   r;

    ASSERT(pDev != NULL && pGraphParam != NULL);
    ASSERT(pGraphParam->pPen != NULL);

    if (pCircleData == NULL)
        return 0;

    penwidth = pGraphParam->pPen->width;
    penstyle = pGraphParam->pPen->style;
    r = pCircleData->r;

    if (pGraphParam->pPen->frame_mode == PFM_CENTER)
    {
        pCircleData->r -= penwidth / 2;
    }
    else
    {
        /* wwh modified on 2002/09/24
        ** A bug when draw circle with PS_INSIDEFRAME style and there is
        ** a clip between the inner circle and the outer circle.
        */
//        pCircleData->r -= penwidth;   //error
        pCircleData->r -= penwidth - 1;
    }
    if (pCircleData->r <= 0)
    {
        GRAPHPARAM GraphParam;
        /* wwh modified on 2002/09/24
        ** A bug when draw circle with the pwnwidth > r.We should use the 
        ** pen fgcolor.
        */
//        GraphParam.bk_color = pGraphParam->bk_color;
//        GraphParam.bk_color = pGraphParam->pPen->fgcolor;
        GraphParam.bk_color = pGraphParam->pen_color;
        GraphParam.bk_mode = pGraphParam->bk_mode;
        CopyRect(&(GraphParam.clip_rect), &(pGraphParam->clip_rect));
        GraphParam.flags = pGraphParam->flags;
        GraphParam.pBrush = NULL;
        GraphParam.pPen = pGraphParam->pPen;
        GraphParam.rop = pGraphParam->rop;
        pCircleData->r += penwidth - 1;
        return DrawThinCircle(pDev, &GraphParam, pCircleData);
    }
    else
    {
        DrawThinCircle(pDev, pGraphParam, pCircleData);
    }
    
    pCircleData->r = r;
    if (penstyle != PS_NULL)
    {
        DrawOuterCircle(pDev, pGraphParam, pCircleData);
    }

    return 0;    
}

/*********************************************************************\
* Function	  DrawOuterCircle 
* Purpose     Draw the outer part of circle. Called by the DrawWideCircle.
* Params	   
    pDev        Pointer to display device structure.
    pGraphParam Pointer to display device parameter.
    pCircleData Pointer to drawing shape data.
* Return	 	   
* Remarks	  
**********************************************************************/
static int DrawOuterCircle(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PCIRCLEDATA pCircleData)
{
    int     yIn, xIn, rIn, yOut, xOut, rOut;
    int     yInNext, yOutNext;
    int     cx, cy;       /* circle center */ 
    int32   flagBresenhamIn, flagBresenhamOut;
    int     nScanLeft, nScanRight;
    RECT    rcClip, rcRealClip;
    int32   rop;
    uint16  penwidth;
    uint32  pencolor;
    ROPFUNC ropfunc;

    ASSERT(pDev != NULL && pGraphParam != NULL && pCircleData != NULL);
    ASSERT(pGraphParam->pPen != NULL);

    penwidth = pGraphParam->pPen->width;
//    pencolor = pGraphParam->pPen->fgcolor;
    pencolor = pGraphParam->pen_color;

    rop = SRop3ToRop2(pGraphParam->rop);
    ropfunc = GetRopFunc(rop);

    if (pGraphParam->pPen->frame_mode == PFM_CENTER)
    {
        rOut = pCircleData->r + (penwidth - 1) / 2 - 1;
    }
    else
    {
        rOut = pCircleData->r - 1;
    }

    rIn  = rOut - penwidth + 1;

    cx = pCircleData->x;
    cy = pCircleData->y;
    xOut = 0;
    yOut = rOut;
    flagBresenhamOut = 3 - (yOut * 2);
    xIn  = 0;
    yIn  = rIn;
    flagBresenhamIn = 3 - (yIn * 2);

    rcClip.x1 = cx - rOut;
    rcClip.x2 = cx + rOut + 1;
    rcClip.y1 = cy - rOut;
    rcClip.y2 = cy + rOut + 1;

    if (pGraphParam->flags & GPF_CLIP_RECT)
        ClipRect(&rcClip, &pGraphParam->clip_rect);
    ClipByDev(&rcClip, pDev);
    if (!IsValidRect(&rcClip))
        return 0;

    _ExcludeCursor(pDev, &rcClip);

    CopyRect(&rcRealClip, &rcClip);
    OffsetRect(&rcClip, -cx, -cy);

    nScanLeft = rcRealClip.x1;
    nScanRight = rcRealClip.x2 - 1;

    yInNext = yIn; //标识内圆下一点的y值
    yOutNext = yOut;//标识外圆下一点的y值

    //Draw first point
    {
        if (IsPtInYClip(rcClip, 0))
        {
            if (!IsXOutOfRect(rcClip, rIn, rOut))
            {
                if (IsPtInXClip(rcClip, rOut))  
                {
                    nScanRight = cx + rOut;
                }

                if (IsPtInXClip(rcClip, rIn))
                {
                    nScanLeft = cx + rIn + 1;
                }
                
                DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                    cy + xOut, rop, pencolor);
                nScanLeft = rcRealClip.x1;
                nScanRight = rcRealClip.x2 - 1;
            }
        
            if (!IsXOutOfRect(rcClip, -rIn, -rOut))
            {
                if (IsPtInXClip(rcClip, -rOut))  
                {
                    nScanLeft = cx - rOut;
                }

                if (IsPtInXClip(rcClip, -rIn))  //BR
                {
                    nScanRight = cx - rIn - 1;
                }
                
                DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                    cy + xOut, rop, pencolor);
                nScanLeft = rcRealClip.x1;
                nScanRight = rcRealClip.x2 - 1;
            }
        }
    }

    while (xOut <= yOut)
    {
        if (flagBresenhamOut >= 0)
            yOutNext = yOut - 1;

        if (yOut > rIn && yOut != yOutNext && 
            !IsXOutOfRect(rcClip, xOut, -xOut))//外圆上下，内圆无关
        {
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
            //下部
            if (IsPtInYClip(rcClip, yOut))
            {
                if (IsPtInXClip(rcClip, xOut))  //BR
                {
                    nScanRight = cx + xOut;
                }

                if (IsPtInXClip(rcClip, -xOut)) //BL
                {
                    nScanLeft = cx - xOut;
                }

                DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                    cy + yOut, rop, pencolor);            
                nScanLeft = rcRealClip.x1;
                nScanRight = rcRealClip.x2 - 1;
            }

            //上部
            if (IsPtInYClip(rcClip, -yOut))
            {
                if (IsPtInXClip(rcClip, xOut)) //UR
                {
                    nScanRight = cx + xOut;
                }

                if (IsPtInXClip(rcClip, -xOut)) //UL
                {
                    nScanLeft = cx - xOut;
                }

                DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                    cy - yOut, rop, pencolor);
                nScanLeft = rcRealClip.x1;
                nScanRight = rcRealClip.x2 - 1;
            }
        }

        if (xOut > rIn && xOut != yOut && 
            !IsXOutOfRect(rcClip, yOut, -yOut))//外圆左右，内圆无关
        {
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
            //下部
            if (IsPtInYClip(rcClip, xOut))
            {
                if (IsPtInXClip(rcClip, yOut)) //RB
                {
                    nScanRight = cx + yOut;
                }

                if (IsPtInXClip(rcClip, -yOut)) //LB
                {
                    nScanLeft = cx - yOut;
                }

                DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                    cy + xOut, rop, pencolor);
                nScanLeft = rcRealClip.x1;
                nScanRight = rcRealClip.x2 - 1;
            }

            //上部
            if (IsPtInYClip(rcClip, -xOut))
            {
                if (IsPtInXClip(rcClip, yOut)) //RU
                {
                    nScanRight = cx + yOut;
                }

                if (IsPtInXClip(rcClip, -yOut)) //LU
                {
                    nScanLeft = cx - yOut;
                }
                
                DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                    cy - xOut, rop, pencolor);
                nScanLeft = rcRealClip.x1;
                nScanRight = rcRealClip.x2 - 1;
            }
        }

        if (yOut <= rIn || xOut <= rIn)//内圆相关
        {
            while (xIn <= yIn)
            {
                if (flagBresenhamIn >= 0)
                    yInNext = yIn - 1;
                if (yIn == yOut && yOut != xOut && 
                    yOut != yOutNext && yIn != yInNext) //外圆，内圆上下部
                {
                    if (IsPtInYClip(rcClip, yOut)) //下部
                    {
                        if (!IsXOutOfRect(rcClip, xIn, xOut))
                        {
                            //下右
                            if (IsPtInXClip(rcClip, xOut))//BR
                            {
                                nScanRight = cx + xOut;
                            }

                            if (IsPtInXClip(rcClip, xIn)) //BR
                            {
                                nScanLeft = cx + xIn + 1;
                            }

                            DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                                cy + yOut, rop, pencolor);
                            nScanLeft = rcRealClip.x1;
                            nScanRight = rcRealClip.x2 - 1;
                        }
                        
                        if (!IsXOutOfRect(rcClip, -xIn, -xOut))
                        {
                            //下左
                            if (IsPtInXClip(rcClip, -xOut))  //BL
                            {
                                nScanLeft = cx - xOut;
                            }

                            if (IsPtInXClip(rcClip, -xIn)) //BL
                            {
                                nScanRight = cx - xIn -1 ;
                            }

                            DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                                cy + yOut, rop, pencolor);
                            nScanLeft = rcRealClip.x1;
                            nScanRight = rcRealClip.x2 - 1;
                        }
                    }//下部
 
                    if (IsPtInYClip(rcClip, -yOut)) //上部
                    {
                        if (!IsXOutOfRect(rcClip, xIn, xOut))
                        {
                            //上右
                            if (IsPtInXClip(rcClip, xOut))  //BR
                            {
                                nScanRight = cx + xOut;
                            }

                            if (IsPtInXClip(rcClip, xIn)) //BR
                            {
                                nScanLeft = cx + xIn + 1;
                            }

                            DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                                cy - yOut, rop, pencolor);
                            nScanLeft = rcRealClip.x1;
                            nScanRight = rcRealClip.x2 - 1;
                        }
                        if (!IsXOutOfRect(rcClip, -xIn, -xOut))
                        {
                            //上左
                            if (IsPtInXClip(rcClip, -xOut))  //UL
                            {
                                nScanLeft = cx - xOut;
                            }

                            if (IsPtInXClip(rcClip, -xIn)) //UL
                            {
                                nScanRight = cx - xIn - 1;
                            }

                            DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                                cy - yOut, rop, pencolor);
                            nScanLeft = rcRealClip.x1;
                            nScanRight = rcRealClip.x2 - 1;
                        }
                    }//上部
                }//外圆，内圆上下部
                
                if (xOut == yIn && yIn != yInNext)//外圆左右两部分，内圆上下
                {
                    if (IsPtInYClip(rcClip, yIn)) //下部
                    {
                        if (!IsXOutOfRect(rcClip, xIn, yOut))
                        {
                            //右下部
                            if (IsPtInXClip(rcClip, yOut)) //RB
                            {
                                nScanRight = cx + yOut;
                            }

                            if (IsPtInXClip(rcClip, xIn))  //BR
                            {
                                nScanLeft = cx + xIn + 1;
                            }

                            DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                                cy + xOut, rop, pencolor);
                            nScanLeft = rcRealClip.x1;
                            nScanRight = rcRealClip.x2 - 1;
                        }
                        
                        if (!IsXOutOfRect(rcClip, -xIn, -yOut))
                        {
                            //左下部
                            if (IsPtInXClip(rcClip, -yOut)) //LB
                            {
                                nScanLeft = cx - yOut;
                            }

                            if (IsPtInXClip(rcClip, -xIn)) //BL
                            {
                                nScanRight = cx - xIn - 1;
                            }

                            DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                                cy + xOut, rop, pencolor);
                            nScanLeft = rcRealClip.x1;
                            nScanRight = rcRealClip.x2 - 1;
                        }
                    } //下部

                    if (IsPtInYClip(rcClip, -yIn)) //上部
                    {
                        if (!IsXOutOfRect(rcClip, xIn, yOut))
                        {
                            //右上部
                            if (IsPtInXClip(rcClip, yOut)) //RU
                            {
                                nScanRight = cx + yOut;
                            }

                            if (IsPtInXClip(rcClip, xIn))  //UR
                            {
                                nScanLeft = cx + xIn + 1;
                            }

                            DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                                cy - xOut, rop, pencolor);
                            nScanLeft = rcRealClip.x1;
                            nScanRight = rcRealClip.x2 - 1;
                            
                        }
                        
                        if (!IsXOutOfRect(rcClip, -xIn, -yOut))
                        {
                            //左上部
                            if (IsPtInXClip(rcClip, -yOut)) //LU
                            {
                                nScanLeft = cx - yOut;
                            }

                            if (IsPtInXClip(rcClip, -xIn)) //UL
                            {
                                nScanRight = cx - xIn - 1;
                            }

                            DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                                cy - xOut, rop, pencolor);
                            nScanLeft = rcRealClip.x1;
                            nScanRight = rcRealClip.x2 - 1;
                        }
                    }//上部
                }//外圆左右两部分，内圆上下


                if (xOut == xIn && xIn != yIn) //外圆，内圆左右两部分
                {
                    if (IsPtInYClip(rcClip, xOut)) //下部
                    {
                        if (!IsXOutOfRect(rcClip, yIn, yOut))
                        {
                            //右下
                            if (IsPtInXClip(rcClip, yOut)) //RB
                            {
                                nScanRight = cx + yOut;
                            }

                            if (IsPtInXClip(rcClip, yIn)) //RB
                            {
                                nScanLeft = cx + yIn + 1;
                            }

                            DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                                cy + xOut, rop, pencolor);
                            nScanLeft = rcRealClip.x1;
                            nScanRight = rcRealClip.x2 - 1;
                        }
                        
                        if (!IsXOutOfRect(rcClip, -yIn, -yOut))
                        {
                            //左下
                            if (IsPtInXClip(rcClip, -yOut)) //LB
                            {
                                nScanLeft = cx - yOut;
                            }

                            if (IsPtInXClip(rcClip, -yIn)) //LB
                            {
                                nScanRight = cx - yIn - 1;
                            }

                            DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                                cy + xOut, rop, pencolor);
                            nScanLeft = rcRealClip.x1;
                            nScanRight = rcRealClip.x2 - 1;
                        }
                    }//下部

                    if (IsPtInYClip(rcClip, -xOut)) //上部
                    {
                        if (!IsXOutOfRect(rcClip, yIn, yOut))
                        {
                            //右上
                            if (IsPtInXClip(rcClip, yOut)) //RU
                            {
                                nScanRight = cx + yOut;
                            }

                            if (IsPtInXClip(rcClip, yIn)) //RU
                            {
                                nScanLeft = cx + yIn + 1;
                            }

                            DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                                cy - xOut, rop, pencolor);
                            nScanLeft = rcRealClip.x1;
                            nScanRight = rcRealClip.x2 - 1;
                        }
                        
                        if (!IsXOutOfRect(rcClip, -yIn, -yOut))
                        {
                            //左上
                            if (IsPtInXClip(rcClip, -yOut)) //LU
                            {
                                nScanLeft = cx - yOut;
                            }
                            
                            if (IsPtInXClip(rcClip, -yIn)) //LU
                            {
                                nScanRight = cx - yIn - 1;
                            }

                            DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                                cy - xOut, rop, pencolor);
                            nScanLeft = rcRealClip.x1;
                            nScanRight = rcRealClip.x2 - 1;
                        }
                        
                    }//上部
                }//外圆左右两部分，内圆左右两部分

                if (flagBresenhamIn < 0)
                    flagBresenhamIn = flagBresenhamIn + (xIn << 2) + 6;
                else
                {
                    flagBresenhamIn = flagBresenhamIn + 
                        ((xIn - yIn) << 2) + 10;
                    yIn--;
                }
                xIn++;                
            }//绘制内圆过程循环            

            xIn  = 0;
            yIn  = rIn;
            flagBresenhamIn = 3 - (yIn << 1);
            yInNext = yIn;
        }//有相交情况，内圆相关

        if (flagBresenhamOut < 0)
        {
            flagBresenhamOut = flagBresenhamOut + (xOut << 2) + 6;
        }
        else
        {
            flagBresenhamOut = flagBresenhamOut + ((xOut - yOut) << 2) + 10;
            yOut--;
        }
        xOut++;
    } // while (xOut < yOut) 外圆循环

    _UnexcludeCursor(pDev);
    return 0;
}

#endif //CIRCLE_SUPPORTED
