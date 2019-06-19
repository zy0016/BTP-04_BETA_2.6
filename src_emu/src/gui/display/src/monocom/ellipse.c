/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display Driver
 *
 * Purpose  : Implement the functions about drawing 
 *            ellipse, ellipse arc, roundrect, pie, chord
 *            
\**************************************************************************/

#ifdef ELLIPSE_SUPPORTED

/*********************************************************************\
* Function	   DrawEllipse
* Purpose      Draw ellipse with various of methods
* Params	   
    pDev            Pointer to display device structure.
    pGraphParam     Pointer to display device parameter.
    pEllipseData    Pointer to drawing shape data.
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawEllipse(PDEV pDev, PGRAPHPARAM pGraphParam, 
                      PELLIPSEDATA pEllipseData)
{
    ASSERT(pDev != NULL && pGraphParam != NULL);
    
    if (pEllipseData == NULL)
        return 0;
    if (pEllipseData->d1 <= 0 || pEllipseData->d2 <= 0)
        return -1;
    if (pEllipseData->d1 <= 2)
    {
        LINEDATA linedata;
        int i;
        for (i = 0; i < pEllipseData->d1; i ++)
        {
            linedata.x1 = linedata.x2 = 
                pEllipseData->x - pEllipseData->d1 / 2 + i;
            linedata.y1 = pEllipseData->y - pEllipseData->d2 / 2;
            linedata.y2 = linedata.y1 + pEllipseData->d2;
            DrawThinLine(pDev, pGraphParam, &linedata);
        }
        return 0;
    }
    else if (pEllipseData->d2 <= 2)
    {
        LINEDATA linedata;
        int i;
        for (i = 0; i < pEllipseData->d2; i ++)
        {
            linedata.y1 = linedata.y2 = 
                pEllipseData->y - pEllipseData->d2 / 2 + i;
            linedata.x1 = pEllipseData->x - pEllipseData->d1 / 2;
            linedata.x2 = linedata.x1 + pEllipseData->d1;
            DrawThinLine(pDev, pGraphParam, &linedata);
        }
        return 0;
    }
    
    if (pGraphParam->pPen == NULL)
        return DrawThinEllipse(pDev, pGraphParam, pEllipseData);
    
    if (pGraphParam->pPen->width > 1)
    {
        return DrawWideEllipse(pDev, pGraphParam, pEllipseData);
    }
    else if (pGraphParam->pPen->style != PS_SOLID && 
        pGraphParam->pPen->style != PS_NULL)
    {
        return DrawPatThinEllipse(pDev, pGraphParam, pEllipseData);
    }
    else
    {
        return DrawThinEllipse(pDev, pGraphParam, pEllipseData);
    }
}

/*********************************************************************\
* Function	   DrawThinEllipse
* Purpose      Draw ellipse with width = 1.
               Draw solid line Ellipse.
               Support the pattern brush
* Params	   
    pDev            Pointer to display device structure.
    pGraphParam     Pointer to display device parameter.
    pEllipseData    Pointer to drawing shape data.
* Return	 	   
* Remarks	below is the algorithm   

**************************************************************
** Using the MIddle Point method to draw Ellipse
** devide the total Ellipse line to four parts and draw the outline
** RU,RB,LU,LB:  means right up, right bottom, left up, left bottom 
** 
** devide the total circle area to two parts and filled the inner of ellipse 
** Below is the two parts
**                   ______
** up:_____________ /      \      
**                 /   2    \    
** _______________|__________|
**                |          |
** bottom:  _______\   1    /    
**                  \______/    
**                             
** yLast:        the y value used for draw the previous point. If the 
**               current y is changed, then draw the fill brush line 
**               and only be used in drawing left and right part of ellipse.
** flagXEven,flagYEven: 
**       the flagXEven(flagYEven) = 1 when the Dx(Dy) is a even number,
**       Dx(Dy) is the X(Y) axis length of the ellipse
** PatBltLineInCircle: draw the brush filled area
** 
**********************************************************************/
static int DrawThinEllipse(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEDATA pEllipseData)
{
    int     x, xNeg, y, yNeg;   /* The symmetrical points of ellipse    */
    int     cx, cy;             /* The certer point of ellipse          */
    int     rx, ry;             /* The radias of ellipse                */
    int     yLast;              /* The Y coordinates before last changed*/
    int32   flagMiddlePoint;    /* Used for the middle point algoeithm  */
    int32   Rx2, Ry2, twoRx2, twoRy2, twoRx2y, twoRy2x;
    int     flagXEven = 1;      /* Flag single or double length of x axies*/
    int     flagYEven = 1;      /* Flag single or double length of y axies*/
    int     yLoopEnd;           /* The end Y of the drawing loop        */
    RECT    rcClip, rcRealClip; /* The relative and absolute clip rect  */
    int     nScanLeft, nScanRight; /* Used to fill ellipse              */
    uint32  pencolor, pixel;
    uint8   lmask, rmask;
    uint8   *pDst;
    int32   rop;
    ROPFUNC ropfunc;

    ASSERT(pDev != NULL && pGraphParam != NULL);
    ASSERT(pEllipseData != NULL);
    if (pEllipseData->d1 <= 2 || pEllipseData->d2 <= 2)
        return 0;

    cx = pEllipseData->x;
    cy = pEllipseData->y;
    rx  = pEllipseData->d1 / 2;
    ry  = pEllipseData->d2 / 2;
    if (pEllipseData->d1 & 1)
        flagXEven = 0;
    if (pEllipseData->d2 & 1)
        flagYEven = 0;

    if (pGraphParam->pPen == NULL)
    {
        ropfunc = GetRopFunc(ROP_DST);
        pencolor = 0;
    }
    else
    {
        pencolor = (pGraphParam->pPen->fgcolor == 0) ? 0x00 : 0xFFFFFFFFL;
        if (pGraphParam->pPen->style == PS_NULL)
        {
            ropfunc = GetRopFunc(ROP_DST);
        }
        else
        {
            rop = SRop3ToRop2(pGraphParam->rop);
            ropfunc = GetRopFunc(rop);
        }
    }
    
    rcClip.x1 = cx - rx;
    rcClip.y1 = cy - ry;
    rcClip.x2 = rcClip.x1 + pEllipseData->d1;
    rcClip.y2 = rcClip.y1 + pEllipseData->d2;

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

    rx  -= flagXEven;
    ry  -= flagYEven;

    x = 0;
    y = ry;
    Rx2 = rx * rx;
    Ry2 = ry * ry;
    twoRx2 = Rx2 << 1;
    twoRy2 = Ry2 << 1;
    twoRx2y = twoRx2 * y;
    twoRy2x = 0;
    flagMiddlePoint = Ry2 - Rx2 * ry + (Rx2 >> 2);
    yLast = y;

    /* 绘制起始两个端点,dx为奇数长，起点只绘制一次 */
    if (flagXEven == 0)
    {
        lmask = 0x80 >> (cx % 8);
        if (IsPointInRect(&rcClip, 0, ry))
        {
            pDst = GETXYADDR(pDev, cx, cy + ry);
            pixel = ropfunc(pencolor, GETPIXEL(pDst));
            PUTPIXEL(pDst, pixel, lmask);
        }

        if (IsPointInRect(&rcClip, 0, -ry))
        {
            pDst = GETXYADDR(pDev, cx,  cy - ry);
            pixel = ropfunc(pencolor, GETPIXEL(pDst));
            PUTPIXEL(pDst, pixel, lmask);
        }

        x ++;
        twoRy2x += twoRy2;
        yLast = y;
        if (flagMiddlePoint < 0)
        {
            flagMiddlePoint += twoRy2x + Ry2;
        }
        else
        {
            y --;
            twoRx2y -= twoRx2;
            flagMiddlePoint += twoRy2x - twoRx2y + Ry2;
        }
    }

    xNeg = -x - flagXEven; //对于dx(dy)为奇数长，xNeg(yNeg) = -x(-y);
    yNeg = -y - flagYEven; //对于dx(dy)为偶数长，xNeg(yNeg) = -x(-y) - 1;

    //切线斜率> -1, X控制
    while (twoRy2x < twoRx2y) 
    {
        if (IsXOutOfRect(rcClip, xNeg, x))
        {
            x ++;
            xNeg --;
            twoRy2x += twoRy2;
            yLast = y;
            if (flagMiddlePoint < 0)
            {
                flagMiddlePoint += twoRy2x + Ry2;
            }
            else
            {
                y --;
                yNeg ++;
                twoRx2y -= twoRx2;
                flagMiddlePoint += twoRy2x - twoRx2y + Ry2;
            }

            continue;
        }

        rmask = 0x80 >> ((cx + x) % 8);
        lmask = 0x80 >> ((cx + xNeg) % 8);
        if (IsPtInYClip(rcClip, y))
        {
            if (IsPtInXClip(rcClip, x))  //RB1
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, xNeg)) //LB1
            {
                pDst = GETXYADDR(pDev,  cx + xNeg , cy + y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx + xNeg + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + y, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        if (IsPtInYClip(rcClip, yNeg))
        {
            if (IsPtInXClip(rcClip, x))  //RU1
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + yNeg);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, xNeg)) //LU1
            {
                pDst = GETXYADDR(pDev,  cx + xNeg , cy + yNeg);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx + xNeg + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + yNeg, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }
        
        x ++;
        xNeg --;
        twoRy2x += twoRy2;
        yLast = y;
        if (flagMiddlePoint < 0)
        {
            flagMiddlePoint += twoRy2x + Ry2;
        }
        else
        {
            y --;
            yNeg ++;
            twoRx2y -= twoRx2;
            flagMiddlePoint += twoRy2x - twoRx2y + Ry2;
        }
    }

    //绘制第二部分
    yLoopEnd = (flagYEven == 1) ? 0 : 1;
    flagMiddlePoint = (int32)(Ry2 * (x + 0.5) * (x + 0.5) + 
        Rx2 * (y - 1) * (y - 1) - Rx2 * Ry2);

    //切线斜率< -1, Y控制
    while (y >= yLoopEnd) 
    {
        if (IsXOutOfRect(rcClip, xNeg, x))
        {
            y --;
            yNeg ++;
            twoRx2y -= twoRx2;
            if (flagMiddlePoint > 0)
            {
                flagMiddlePoint += Rx2 - twoRx2y;
            }
            else
            {
                x ++;
                xNeg --;
                twoRy2x += twoRy2;
                flagMiddlePoint += twoRy2x - twoRx2y + Rx2;
            }

            continue;
        }

        rmask = 0x80 >> ((cx + x) % 8);
        lmask = 0x80 >> ((cx + xNeg) % 8);
        if (IsPtInYClip(rcClip, y))
        {
            if (IsPtInXClip(rcClip, x))  //RB1
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, xNeg)) //LB1
            {
                pDst = GETXYADDR(pDev,  cx + xNeg , cy + y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx + xNeg + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + y, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        if (IsPtInYClip(rcClip, yNeg))
        {
            if (IsPtInXClip(rcClip, x))  //RU1
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + yNeg);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, xNeg)) //LU1
            {
                pDst = GETXYADDR(pDev,  cx + xNeg , cy + yNeg);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx + xNeg + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + yNeg, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        y --;
        yNeg ++;
        twoRx2y -= twoRx2;
        if (flagMiddlePoint > 0)
        {
            flagMiddlePoint += Rx2 - twoRx2y;
        }
        else
        {
            x ++;
            xNeg --;
            twoRy2x += twoRy2;
            flagMiddlePoint += twoRy2x - twoRx2y + Rx2;
        }
    }//while (y >= yLoopEnd)

    //绘制左右两个端点
    if (flagYEven == 0)
    {
        if (x != rx)
        {
            LINEDATA linedata;
            linedata.y1 = linedata.y2 = cy;
            linedata.x1 = cx + x;
            linedata.x2 = cx + rx;
            DrawThinLine(pDev, pGraphParam, &linedata);
            linedata.x1 = cx - rx;
            linedata.x2 = cx + xNeg;
            DrawThinLine(pDev, pGraphParam, &linedata);
            nScanRight = cx + x - 1;
            nScanLeft = cx + xNeg + 1;
        }
        else if (IsPtInYClip(rcClip, 0))
        {
            if (IsPtInXClip(rcClip, rx))
            {
                rmask = 0x80 >> ((cx + rx) % 8);
                pDst = GETXYADDR(pDev, cx + rx,  cy);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + rx - 1;
            }
            
            if (IsPtInXClip(rcClip, -rx))
            {
                lmask = 0x80 >> ((cx - rx) % 8);
                pDst = GETXYADDR(pDev, cx - rx,  cy);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx - rx + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy, 
                pGraphParam);
        }
    }
    else if (x != rx)
    {
        LINEDATA linedata;
        linedata.y1 = linedata.y2 = cy;
        linedata.x1 = cx + x;
        linedata.x2 = cx + rx;
        DrawThinLine(pDev, pGraphParam, &linedata);
        linedata.x1 = cx - rx;
        linedata.x2 = cx + xNeg;
        DrawThinLine(pDev, pGraphParam, &linedata);
        linedata.y1 = linedata.y2 = cy - 1;
        DrawThinLine(pDev, pGraphParam, &linedata);
        linedata.x1 = cx + x;
        linedata.x2 = cx + rx;
        DrawThinLine(pDev, pGraphParam, &linedata);
    }

    _UnexcludeCursor(pDev);
    return 0;
}

/*********************************************************************\
* Function	  DrawWideEllipse 
* Purpose     Draw ellipse with width > 1.
              Draw solid line Ellipse.
              Support the pattern brush. 
* Params	   
    pDev        Pointer to display device structure.
    pGraphParam Pointer to display device parameter.
    pCircleData Pointer to drawing shape data.
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawWideEllipse(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEDATA pEllipseData)
{
    int16 penwidth, penstyle;
    int   d1, d2;

    ASSERT(pDev != NULL && pGraphParam != NULL);
    ASSERT(pEllipseData != NULL);

    penwidth = pGraphParam->pPen->width;
    penstyle = pGraphParam->pPen->style;
    d1  = pEllipseData->d1;
    d2  = pEllipseData->d2;

    if (pGraphParam->pPen->frame_mode == PFM_CENTER)
    {
        pEllipseData->d1 -= (penwidth / 2) * 2;
        pEllipseData->d2 -= (penwidth / 2) * 2;
    }
    else
    {
        pEllipseData->d1 -= (penwidth - 1) * 2;
        pEllipseData->d2 -= (penwidth - 1) * 2;
    }

    /* The total inner ellipse is null, fill it with pen color */
    /* wwh modified on 2002/09/25
    ** A bug occured when d1 or d2 is 1 or 2. The DrawThinEllipse can only 
    ** get the data that >=2.So here must draw a total ellipse with fill 
    ** color!
    */
//    if (pEllipseData->d1 <= 0 || pEllipseData->d2 <= 0)
    if (pEllipseData->d1 <= 2 || pEllipseData->d2 <= 2)
    {
        GRAPHPARAM GraphParam;

        GraphParam.bk_color = pGraphParam->pPen->fgcolor;
        GraphParam.bk_mode = pGraphParam->bk_mode;
        CopyRect(&(GraphParam.clip_rect), &(pGraphParam->clip_rect));
        GraphParam.flags = pGraphParam->flags;
        /* set the brush null, so fill the inner with pen */
        GraphParam.pBrush = NULL;
        GraphParam.pPen = pGraphParam->pPen;
        GraphParam.rop = pGraphParam->rop;

        pEllipseData->d1 += (penwidth - 1) * 2;
        pEllipseData->d2 += (penwidth - 1) * 2;
        return DrawThinEllipse(pDev, &GraphParam, pEllipseData);
    }
    else
    {
        DrawThinEllipse(pDev, pGraphParam, pEllipseData);
    }

    pEllipseData->d1 = d1;
    pEllipseData->d2 = d2;

    if (penstyle != PS_NULL)
    {
        DrawOuterEllipse(pDev, pGraphParam, pEllipseData);
    }

    return 0;    
}

/*********************************************************************\
* Function	  DrawOuterEllipse 
* Purpose     Draw the outer part of ellipse. 
              Called by the DrawWideEllipse. 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawOuterEllipse(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEDATA pEllipseData)
{
    int     xIn, xNegIn, yIn, yNegIn, rxIn, ryIn;
    int     xOut, xNegOut, yOut, yNegOut, rxOut, ryOut;
    int     cx, cy, yLast, flagInAdded;
    int32   flagMiddlePointIn, flagMiddlePointOut;
    int32   Rx2In, Ry2In, twoRx2In, twoRy2In, twoRx2yIn, twoRy2xIn;
    int32   Rx2Out, Ry2Out, twoRx2Out, twoRy2Out, twoRx2yOut, twoRy2xOut;
    int     flagInSecond = 0; /*内圆进入第二段绘制的标志 */
    int     flagXEven = 1, flagYEven = 1;
    int     yLoopEnd;
    int     nScanLeft, nScanRight;
    RECT    rcClip, rcRealClip;
    uint16  penwidth;
    uint32  pencolor, pixel;
    uint8   mask;
    uint8   *pDst;
    int     rop;
    ROPFUNC ropfunc;

    penwidth = pGraphParam->pPen->width;
    pencolor = (pGraphParam->pPen->fgcolor == 0) ? 0x00 : 0xFFFFFFFFL;

    ASSERT(pDev != NULL && pGraphParam != NULL);
    ASSERT(pEllipseData != NULL);

    rop = SRop3ToRop2(pGraphParam->rop);
    ropfunc = GetRopFunc(rop);
    cx = pEllipseData->x;
    cy = pEllipseData->y;

    if (pGraphParam->pPen->frame_mode == PFM_CENTER)
    {
        rxOut = pEllipseData->d1 / 2 + (penwidth - 1) / 2;
        ryOut = pEllipseData->d2 / 2 + (penwidth - 1) / 2;
        rcClip.x1 = cx - rxOut;
        rcClip.y1 = cy - ryOut;
        rcClip.x2 = rcClip.x1 + pEllipseData->d1 + (penwidth - 1) / 2 * 2;
        rcClip.y2 = rcClip.y1 + pEllipseData->d2 + (penwidth - 1) / 2 * 2;
    }
    else
    {
        rxOut = pEllipseData->d1 / 2;
        ryOut = pEllipseData->d2 / 2;
        rcClip.x1 = cx - rxOut;
        rcClip.y1 = cy - ryOut;
        rcClip.x2 = rcClip.x1 + pEllipseData->d1;
        rcClip.y2 = rcClip.y1 + pEllipseData->d2;
    }

    if (pEllipseData->d1 & 1)
        flagXEven = 0;
    if (pEllipseData->d2 & 1)
        flagYEven = 0;

    rxOut  -= flagXEven;
    ryOut  -= flagYEven;
    rxIn  = rxOut - penwidth + 1;
    ryIn  = ryOut - penwidth + 1;

    xOut = 0;
    yOut = ryOut;
    Rx2Out = rxOut * rxOut;
    Ry2Out = ryOut * ryOut;
    twoRx2Out = Rx2Out << 1;
    twoRy2Out = Ry2Out << 1;
    twoRx2yOut = twoRx2Out * yOut;
    twoRy2xOut = 0;
    flagMiddlePointOut = Ry2Out - Rx2Out * ryOut + (Rx2Out >> 2);

    xIn = 0;
    yIn = ryIn;
    Rx2In = rxIn * rxIn;
    Ry2In = ryIn * ryIn;
    twoRx2In = Rx2In << 1;
    twoRy2In = Ry2In << 1;
    twoRx2yIn = twoRx2In * yIn;
    twoRy2xIn = 0;
    flagMiddlePointIn = Ry2In - Rx2In * ryIn + (Rx2In >> 2);

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
    
    yLast = yOut;
    
    //绘制起始两个端点,dx为奇数长，起点只绘制一次
    if (flagXEven == 0)
    {
        mask = 0x80 >> (cx % 8);
        if (IsPointInRect(&rcClip, 0, ryOut))  //RB1
        {
            pDst = GETXYADDR(pDev, cx,  cy + ryOut);
            pixel = ropfunc(pencolor, GETPIXEL(pDst));
            PUTPIXEL(pDst, pixel, mask);
        }

        if (IsPointInRect(&rcClip, 0, -ryOut))  //RU1
        {
            pDst = GETXYADDR(pDev, cx,  cy - ryOut);
            pixel = ropfunc(pencolor, GETPIXEL(pDst));
            PUTPIXEL(pDst, pixel, mask);
        }

        xOut ++;
        twoRy2xOut += twoRy2Out;
        yLast = yOut;
        if (flagMiddlePointOut < 0)
        {
            flagMiddlePointOut += twoRy2xOut + Ry2Out;
        }
        else
        {
            yOut --;
            twoRx2yOut -= twoRx2Out;
            flagMiddlePointOut += twoRy2xOut - twoRx2yOut + Ry2Out;
        }
    }

    xNegOut = -xOut - flagXEven; //对于dx(dy)为奇数长，xNeg(yNeg) = -x(y);
    yNegOut = -yOut - flagYEven; //对于dx(dy)为偶数长，xNeg(yNeg) = -x(y)-1;
    xNegIn = -xIn - flagXEven; //对于dx(dy)为奇数长，xNeg(yNeg) = -x(y);
    yNegIn= -yIn - flagYEven; //对于dx(dy)为偶数长，xNeg(yNeg) = -x(y)-1;

    //切线斜率> -1, X控制
    while (twoRy2xOut < twoRx2yOut) 
    {
        yLast = yOut;
        while (yOut == yLast)
        {
            xOut ++;
            xNegOut --;
            twoRy2xOut += twoRy2Out;
            if (flagMiddlePointOut < 0)
            {
                flagMiddlePointOut += twoRy2xOut + Ry2Out;
            }
            else
            {
                yOut --;
                yNegOut ++;
                twoRx2yOut -= twoRx2Out;
                flagMiddlePointOut += twoRy2xOut - twoRx2yOut + Ry2Out;
            }
        }

        xOut --;
        xNegOut ++;
        yOut ++;
        yNegOut --;

        if (yOut > yIn) //内椭圆无关，填充外椭圆
        {
            if (IsXOutOfRect(rcClip, xNegOut, xOut))
            {
                xOut ++;
                xNegOut --;
                yOut --;
                yNegOut ++;
                continue;
            }

            if (IsPtInYClip(rcClip, yOut))
            {
                if (IsPtInXClip(rcClip, xOut))  //RBout
                {
                    nScanRight = cx + xOut;
                }

                if (IsPtInXClip(rcClip, xNegOut)) //LBout
                {
                    nScanLeft = cx + xNegOut;
                }

                DrawSolidHLine(pDev, nScanLeft, nScanRight, cy + yOut, 
                    rop, pencolor);
                nScanLeft = rcRealClip.x1;
                nScanRight = rcRealClip.x2 - 1;
            }
            
            if (IsPtInYClip(rcClip, yNegOut))
            {
                if (IsPtInXClip(rcClip, xOut))  //RUout
                {
                    nScanRight = cx + xOut;
                }

                if (IsPtInXClip(rcClip, xNegOut)) //LUout
                {
                    nScanLeft = cx + xNegOut;
                }

                DrawSolidHLine(pDev, nScanLeft, nScanRight, cy + yNegOut, 
                    rop, pencolor);
                nScanLeft = rcRealClip.x1;
                nScanRight = rcRealClip.x2 - 1;
            }
        } //内椭圆无关，填充外椭圆
        else
        {
            while (yIn == yOut)
            {
                xIn ++;
                xNegIn --;
                twoRy2xIn += twoRy2In;
                if (flagMiddlePointIn < 0)
                {
                    flagMiddlePointIn += twoRy2xIn + Ry2In;
                }
                else
                {
                    yIn --;
                    yNegIn ++;
                    twoRx2yIn -= twoRx2In;
                    flagMiddlePointIn += twoRy2xIn - twoRx2yIn + Ry2In;
                }
            }
            //yIn != yOut, now should draw line.

            xIn --;
            xNegIn ++;
            yIn ++;
            yNegIn --;

            if (IsPtInYClip(rcClip, yOut))
            {
                if (!IsXOutOfRect(rcClip, xIn + 1, xOut))
                {
                    if (IsPtInXClip(rcClip, xOut))  //RBout
                    {
                        nScanRight = cx + xOut;
                    }

                    if (IsPtInXClip(rcClip, xIn)) //RBin
                    {
                        nScanLeft = cx + xIn + 1;
                    }
                    
                    DrawSolidHLine(pDev, nScanLeft, nScanRight, cy + yOut, 
                        rop, pencolor);
                    nScanLeft = rcRealClip.x1;
                    nScanRight = rcRealClip.x2 - 1;
                    
                }
                if (!IsXOutOfRect(rcClip, xNegIn - 1, xNegOut))
                {
                    if (IsPtInXClip(rcClip, xNegOut))  //LBout
                    {
                        nScanLeft = cx + xNegOut;
                    }

                    if (IsPtInXClip(rcClip, xNegIn)) //LBin
                    {
                        nScanRight = cx + xNegIn - 1;
                    }
                    
                    DrawSolidHLine(pDev, nScanLeft, nScanRight, cy + yOut, 
                        rop, pencolor);
                    nScanLeft = rcRealClip.x1;
                    nScanRight = rcRealClip.x2 - 1;
                }
            }

            if (IsPtInYClip(rcClip, yNegOut))
            {
                if (!IsXOutOfRect(rcClip, xIn + 1, xOut))
                {
                    if (IsPtInXClip(rcClip, xOut))  //RUout
                    {
                        nScanRight = cx + xOut;
                    }

                    if (IsPtInXClip(rcClip, xIn)) //RUin
                    {
                        nScanLeft = cx + xIn + 1;
                    }

                    DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                        cy + yNegOut, rop, pencolor);
                    nScanLeft = rcRealClip.x1;
                    nScanRight = rcRealClip.x2 - 1;
                }

                if (!IsXOutOfRect(rcClip, xNegIn - 1, xNegOut))
                {
                    if (IsPtInXClip(rcClip, xNegOut))  //LUout
                    {
                        nScanLeft = cx + xNegOut;
                    }

                    if (IsPtInXClip(rcClip, xNegIn)) //LUin
                    {
                        nScanRight = cx + xNegIn - 1;
                    }

                    DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                        cy + yNegOut, rop, pencolor);
                    nScanLeft = rcRealClip.x1;
                    nScanRight = rcRealClip.x2 - 1;
                }
            }

            xIn ++;
            xNegIn --;
            yIn --;
            yNegIn ++;
        }

        xOut ++;
        xNegOut --;
        yOut --;
        yNegOut ++;
    }
    
    //绘制第二部分
    yLoopEnd = (flagYEven == 1) ? 0 : 1;
    flagMiddlePointOut = (int32)(Ry2Out * (xOut + 0.5) * (xOut + 0.5) + 
        Rx2Out * (yOut - 1) * (yOut - 1) - Rx2Out * Ry2Out);

    if (yOut == yLast)
    {
        yOut --;
        yNegOut ++;
        twoRx2yOut -= twoRx2Out;
        if (flagMiddlePointOut > 0)
        {
            flagMiddlePointOut += Rx2Out - twoRx2yOut;
        }
        else
        {
            xOut ++;
            xNegOut --;
            twoRy2xOut += twoRy2Out;
            flagMiddlePointOut += twoRy2xOut - twoRx2yOut + Rx2Out;
        }
    }

    while (yOut >= yLoopEnd) //切线斜率< -1, Y控制
    {
        if (yOut > yIn) //内椭圆无关，填充外椭圆
        {
            if (IsXOutOfRect(rcClip, xNegOut, xOut))
            {
                yOut --;
                yNegOut ++;
                twoRx2yOut -= twoRx2Out;
                if (flagMiddlePointOut > 0)
                {
                    flagMiddlePointOut += Rx2Out - twoRx2yOut;
                }
                else
                {
                    xOut ++;
                    xNegOut --;
                    twoRy2xOut += twoRy2Out;
                    flagMiddlePointOut += twoRy2xOut - twoRx2yOut + Rx2Out;
                }
                continue;
            }

            if (IsPtInYClip(rcClip, yOut))
            {
                if (IsPtInXClip(rcClip, xOut))  //RBout
                {
                    nScanRight = cx + xOut;
                }

                if (IsPtInXClip(rcClip, xNegOut)) //LBout
                {
                    nScanLeft = cx + xNegOut;
                }

                DrawSolidHLine(pDev, nScanLeft, nScanRight, cy + yOut, 
                    rop, pencolor);
                nScanLeft = rcRealClip.x1;
                nScanRight = rcRealClip.x2 - 1;
            }
            
            if (IsPtInYClip(rcClip, yNegOut))
            {
                if (IsPtInXClip(rcClip, xOut))  //RUout
                {
                    nScanRight = cx + xOut;
                }

                if (IsPtInXClip(rcClip, xNegOut)) //LUout
                {
                    nScanLeft = cx + xNegOut;
                }

                DrawSolidHLine(pDev, nScanLeft, nScanRight, cy + yNegOut, 
                    rop, pencolor);
                nScanLeft = rcRealClip.x1;
                nScanRight = rcRealClip.x2 - 1;
            }
        } //内椭圆无关，填充外椭圆

        else
        {
            flagInAdded = 0;
            if (flagInSecond == 0)
            {
                while (yOut == yIn && twoRy2xIn < twoRx2yIn)
                {
                    xIn ++;
                    flagInAdded = 1;
                    xNegIn --;
                    twoRy2xIn += twoRy2In;
                    if (flagMiddlePointIn < 0)
                    {
                        flagMiddlePointIn += twoRy2xIn + Ry2In;
                    }
                    else
                    {
                        yIn --;
                        yNegIn ++;
                        twoRx2yIn -= twoRx2In;
                        flagMiddlePointIn += twoRy2xIn - twoRx2yIn + Ry2In;
                    }                    
                }

                if (twoRy2xIn >= twoRx2yIn)
                {
                    flagMiddlePointIn = (int32)(Ry2In * (xIn + 0.5) * (xIn + 0.5) + 
                        Rx2In * (yIn - 1) * (yIn - 1) - Rx2In * Ry2In);
                    flagInSecond = 1;
                }
            }

            if (flagInSecond == 1 && yOut == yIn)
            {
                yIn --;
                yNegIn ++;
                twoRx2yIn -= twoRx2In;
                if (flagMiddlePointIn > 0)
                {
                    flagMiddlePointIn += Rx2In - twoRx2yIn;
                }
                else
                {
                    xIn ++;
                    flagInAdded = 1;
                    xNegIn --;
                    twoRy2xIn += twoRy2In;
                    flagMiddlePointIn += twoRy2xIn- twoRx2yIn + Rx2In;
                }
            }

            xIn -= (flagInAdded == 0 ? 0 : 1);
            xNegIn += (flagInAdded == 0 ? 0 : 1);

            if (IsPtInYClip(rcClip, yOut))
            {
                if (!IsXOutOfRect(rcClip, xIn + 1, xOut))
                {
                    if (IsPtInXClip(rcClip, xOut))  //RBout
                    {
                        nScanRight = cx + xOut;
                    }

                    if (IsPtInXClip(rcClip, xIn)) //RBin
                    {
                        nScanLeft = cx + xIn + 1;
                    }

                    DrawSolidHLine(pDev, nScanLeft, nScanRight, cy + yOut, 
                        rop, pencolor);
                    nScanLeft = rcRealClip.x1;
                    nScanRight = rcRealClip.x2 - 1;
                }
                
                if (!IsXOutOfRect(rcClip, xNegIn - 1, xNegOut))
                {
                    if (IsPtInXClip(rcClip, xNegOut))  //LBout
                    {
                        nScanLeft = cx + xNegOut;
                    }

                    if (IsPtInXClip(rcClip, xNegIn)) //LBin
                    {
                        nScanRight = cx + xNegIn - 1;
                    }

                    DrawSolidHLine(pDev, nScanLeft, nScanRight, cy + yOut, 
                        rop, pencolor);
                    nScanLeft = rcRealClip.x1;
                    nScanRight = rcRealClip.x2 - 1;
                }
            }
            
            if (IsPtInYClip(rcClip, yNegOut))
            {
                if (!IsXOutOfRect(rcClip, xIn + 1, xOut))
                {
                    if (IsPtInXClip(rcClip, xOut))  //RUout
                    {
                        nScanRight = cx + xOut;
                    }

                    if (IsPtInXClip(rcClip, xIn)) //RUin
                    {
                        nScanLeft = cx + xIn + 1;
                    }

                    DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                        cy + yNegOut, rop, pencolor);
                    nScanLeft = rcRealClip.x1;
                    nScanRight = rcRealClip.x2 - 1;
                }
                
                if (!IsXOutOfRect(rcClip, xNegIn - 1, xNegOut))
                {
                    if (IsPtInXClip(rcClip, xNegOut))  //LUout
                    {
                        nScanLeft = cx + xNegOut;
                    }

                    if (IsPtInXClip(rcClip, xNegIn)) //LUin
                    {
                        nScanRight = cx + xNegIn - 1;
                    }

                    DrawSolidHLine(pDev, nScanLeft, nScanRight, 
                        cy + yNegOut, rop, pencolor);
                    nScanLeft = rcRealClip.x1;
                    nScanRight = rcRealClip.x2 - 1;
                }
            }

            xIn += (flagInAdded == 0 ? 0 : 1);
            xNegIn -= (flagInAdded == 0 ? 0 : 1);
        }

        yOut --;
        yNegOut ++;
        twoRx2yOut -= twoRx2Out;
        if (flagMiddlePointOut > 0)
        {
            flagMiddlePointOut += Rx2Out - twoRx2yOut;
        }
        else
        {
            xOut ++;
            xNegOut --;
            twoRy2xOut += twoRy2Out;
            flagMiddlePointOut += twoRy2xOut - twoRx2yOut + Rx2Out;
        }
    }
    
    //绘制左右两个端点
    if (flagYEven == 0)
    {
        if (IsPtInYClip(rcClip, 0))
        {
            if (!IsXOutOfRect(rcClip, rxIn + 1, rxOut))
            {
                if (IsPtInXClip(rcClip, rxOut))  //Rout
                {
                    nScanRight = cx + rxOut;
                }
                
                if (IsPtInXClip(rcClip, rxIn))  //Rin
                {
                    nScanLeft = cx + rxIn + 1;
                }

                DrawSolidHLine(pDev, nScanLeft, nScanRight, cy, 
                    rop, pencolor);
            }

            if (!IsXOutOfRect(rcClip,  -rxIn - 1, -rxOut))
            {
                if (IsPtInXClip(rcClip, -rxOut))  //RB1
                {
                    nScanLeft = cx - rxOut;
                }
                
                if (IsPtInXClip(rcClip, -rxIn))  //RU1
                {
                    nScanRight = cx - rxIn - 1;
                }

                DrawSolidHLine(pDev, nScanLeft, nScanRight, cy, 
                    rop, pencolor);
            }
        }
    }

    _UnexcludeCursor(pDev);
    return 0;
}

/*********************************************************************\
* Function	   DrawPatThinEllipse
* Purpose      Draw thin ellipse with width = 1 and using style pen. 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawPatThinEllipse(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEDATA pEllipseData)
{
    int     x, xNeg, y, yNeg, cx, cy, rx, ry, yLast;
    int32   nLoopNum, penoffset;
    int32   flagMiddlePoint;
    int32   Rx2, Ry2, twoRx2, twoRy2, twoRx2y, twoRy2x;
    int     rop;
    int     flagXEven = 1, flagYEven = 1;
    int     yLoopEnd;
    RECT    rcClip, rcRealClip;
    uint32  pencolor, bkcolor;
    PENSTYLE    penstyle;
    uint32  penpattern, penmask = 0x80000000L;
    int     pencount;
    uint8   *pDst;
    uint32  pixel;
    uint8   lmask, rmask;
    int     nScanLeft, nScanRight;
    ROPFUNC ropfunc, ropDashfunc;

    ASSERT(pDev != NULL && pGraphParam != NULL);
    ASSERT(pGraphParam->pPen != NULL);
    ASSERT(pEllipseData != NULL);
    ASSERT(pEllipseData->d1 > 2 && pEllipseData->d2 > 2);

    pencolor = (pGraphParam->pPen->fgcolor == 0) ? 0x00 : 0xFFFFFFFFL;
    bkcolor  = (pGraphParam->bk_color == 0) ? 0x00 : 0xFFFFFFFFL;

    penstyle = GetPenStyle(pGraphParam->pPen->style);
    penpattern = penstyle.pattern;
    pencount = penstyle.count;

    if (pGraphParam->pPen->style == PS_NULL)
    {
        ropDashfunc = GetRopFunc(ROP_DST);
        ropfunc = GetRopFunc(ROP_DST);
    }
    else
    {
        rop = SRop3ToRop2(pGraphParam->rop);
        ropfunc = GetRopFunc(rop);

        if (pGraphParam->bk_mode == BM_TRANSPARENT)
            ropDashfunc = GetRopFunc(ROP_DST);
        else
            ropDashfunc = GetRopFunc(ROP_SRC);
    }

    cx = pEllipseData->x;
    cy = pEllipseData->y;
    rx  = pEllipseData->d1 / 2;
    ry  = pEllipseData->d2 / 2;

    if (pEllipseData->d1 & 1)
    {
        flagXEven = 0;
    }
    if (pEllipseData->d2 & 1)
    {
        flagYEven = 0;
    }

    rcClip.x1 = cx - rx;
    rcClip.y1 = cy - ry;
    rcClip.x2 = rcClip.x1 + pEllipseData->d1;
    rcClip.y2 = rcClip.y1 + pEllipseData->d2;

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

    rx  -= flagXEven;
    ry  -= flagYEven;

    x = 0;
    y = ry;
    Rx2 = rx * rx;
    Ry2 = ry * ry;
    twoRx2 = Rx2 << 1;
    twoRy2 = Ry2 << 1;
    twoRx2y = twoRx2 * y;
    twoRy2x = 0;
    flagMiddlePoint = Ry2 - Rx2 * ry + (Rx2 >> 2);

    nLoopNum = 0;
    while (twoRy2x < twoRx2y) //切线斜率> -1, X控制
    {
        nLoopNum ++;
        x ++;
        twoRy2x += twoRy2;
        if (flagMiddlePoint < 0)
        {
            flagMiddlePoint += twoRy2x + Ry2;
        }
        else
        {
            y --;
            twoRx2y -= twoRx2;
            flagMiddlePoint += twoRy2x - twoRx2y + Ry2;
        }
    }
    while (y > 0) //切线斜率< -1, Y控制
    {
        nLoopNum ++;
        y --;
        twoRx2y -= twoRx2;
        if (flagMiddlePoint > 0)
        {
            flagMiddlePoint += Rx2 - twoRx2y;
        }
        else
        {
            x ++;
            twoRy2x += twoRy2;
            flagMiddlePoint += twoRy2x - twoRx2y + Rx2;
        }
    } //得到了1/4椭圆长度

    x = 0;
    y = ry;
    Rx2 = rx * rx;
    Ry2 = ry * ry;
    twoRx2 = Rx2 << 1;
    twoRy2 = Ry2 << 1;
    twoRx2y = twoRx2 * y;
    twoRy2x = 0;
    flagMiddlePoint = Ry2 - Rx2 * ry + (Rx2 >> 2);
    yLast = y;

    //绘制起始两个端点,dx为奇数长，起点只绘制一次
    if (flagXEven == 0)
    {
        lmask = 0x80 >> (cx % 8);
        if (IsPointInRect(&rcClip, 0, ry))  //RB1
        {
            pDst = GETXYADDR(pDev, cx,  cy + ry);

            if (penmask & penpattern)
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
            else
                pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));

            PUTPIXEL(pDst, pixel, lmask);
        }

        if (IsPointInRect(&rcClip, 0, -ry))  //RU1
        {
            pDst = GETXYADDR(pDev, cx,  cy - ry);
            penoffset = (nLoopNum * 2 + flagYEven) % pencount;

            if ((penmask >> penoffset) & penpattern)
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
            else
                pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));

            PUTPIXEL(pDst, pixel, lmask);
        }
        x ++;
        twoRy2x += twoRy2;
        if (flagMiddlePoint < 0)
        {
            flagMiddlePoint += twoRy2x + Ry2;
        }
        else
        {
            yLast = y;
            y --;
            twoRx2y -= twoRx2;
            flagMiddlePoint += twoRy2x - twoRx2y + Ry2;
        }
    }

    xNeg = -x - flagXEven;
    yNeg = -y - flagYEven;
    
    while (twoRy2x < twoRx2y) //切线斜率> -1, X控制
    {
        if (IsXOutOfRect(rcClip, xNeg, x))
        {
            x ++;
            xNeg --;
            twoRy2x += twoRy2;
            yLast = y;
            if (flagMiddlePoint < 0)
            {
                flagMiddlePoint += twoRy2x + Ry2;
            }
            else
            {
                y --;
                yNeg ++;
                twoRx2y -= twoRx2;
                flagMiddlePoint += twoRy2x - twoRx2y + Ry2;
            }

            continue;
        }

        lmask = 0x80 >> ((cx + xNeg) % 8);
        rmask = 0x80 >> ((cx + x) % 8);
        if (IsPtInYClip(rcClip, y))
        {
            if (IsPtInXClip(rcClip, x))  //RB1
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + y);
                penoffset = x % pencount;

                if ((penmask >> penoffset) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, xNeg)) //LB1
            {
                pDst = GETXYADDR(pDev,  cx + xNeg , cy + y);
                penoffset = (nLoopNum * 4 + (flagXEven + flagYEven) * 2 - 
                    flagXEven - x) % pencount;

                if ((penmask >> penoffset) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx + xNeg + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + y, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        if (IsPtInYClip(rcClip, yNeg))
        {
            if (IsPtInXClip(rcClip, x))  //RU1
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + yNeg);
                penoffset = (nLoopNum * 2 + flagYEven - x) % pencount;

                if ((penmask >> penoffset) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, xNeg)) //LU1
            {
                pDst = GETXYADDR(pDev,  cx + xNeg , cy + yNeg);
                penoffset = (nLoopNum * 2 + flagYEven + flagXEven + x) % 
                    pencount;

                if ((penmask >> penoffset) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx + xNeg + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + yNeg, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }
        
        x ++;
        xNeg --;
        twoRy2x += twoRy2;
        yLast = y;
        if (flagMiddlePoint < 0)
        {
            flagMiddlePoint += twoRy2x + Ry2;
        }
        else
        {
            y --;
            yNeg ++;
            twoRx2y -= twoRx2;
            flagMiddlePoint += twoRy2x - twoRx2y + Ry2;
        }
    }

    //绘制第二部分
    yLoopEnd = (flagYEven == 1) ? 0 : 1;
    flagMiddlePoint = (int32)(Ry2 * (x + 0.5) * (x + 0.5) + 
        Rx2 * (y - 1) * (y - 1) - Rx2 * Ry2);

    while (y >= yLoopEnd) //切线斜率< -1, Y控制
    {
        if (IsXOutOfRect(rcClip, xNeg, x))
        {
            y --;
            yNeg ++;
            twoRx2y -= twoRx2;
            if (flagMiddlePoint > 0)
            {
                flagMiddlePoint += Rx2 - twoRx2y;
            }
            else
            {
                x ++;
                xNeg --;
                twoRy2x += twoRy2;
                flagMiddlePoint += twoRy2x - twoRx2y + Rx2;
            }

            continue;
        }

        lmask = 0x80 >> ((cx + xNeg) % 8);
        rmask = 0x80 >> ((cx + x) % 8);
        if (IsPtInYClip(rcClip, y))
        {
            if (IsPtInXClip(rcClip, x))  //RB1
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + y);
                penoffset = (nLoopNum - y) % pencount;

                if ((penmask >> penoffset) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, xNeg)) //LB1
            {
                pDst = GETXYADDR(pDev,  cx + xNeg , cy + y);
                penoffset = (nLoopNum * 3 + flagXEven + flagYEven * 2 + y) % 
                    pencount;

                if ((penmask >> penoffset) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx + xNeg + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + y, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        if (IsPtInYClip(rcClip, yNeg))
        {
            if (IsPtInXClip(rcClip, x))  //RU1
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + yNeg);
                penoffset = (nLoopNum + flagYEven + y) % pencount;

                if ((penmask >> penoffset) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, xNeg)) //LU1
            {
                pDst = GETXYADDR(pDev,  cx + xNeg , cy + yNeg);
                penoffset = (nLoopNum * 3 + flagYEven + flagXEven - y) % 
                    pencount;

                if ((penmask >> penoffset) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx + xNeg + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + yNeg, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        y --;
        yNeg ++;
        twoRx2y -= twoRx2;
        if (flagMiddlePoint > 0)
        {
            flagMiddlePoint += Rx2 - twoRx2y;
        }
        else
        {
            x ++;
            xNeg --;
            twoRy2x += twoRy2;
            flagMiddlePoint += twoRy2x - twoRx2y + Rx2;
        }
    }
    //绘制左右两个端点
    if (flagYEven == 0)
    {
        if (x != rx)
        {
            LINEDATA linedata;
            linedata.y1 = linedata.y2 = cy;
            linedata.x1 = cx + x;
            linedata.x2 = cx + rx;
            DrawThinLine(pDev, pGraphParam, &linedata);
            linedata.x1 = cx - rx;
            linedata.x2 = cx + xNeg;
            DrawThinLine(pDev, pGraphParam, &linedata);
            nScanRight = cx + x - 1;
            nScanLeft = cx + xNeg + 1;
        }
        else if (IsPtInYClip(rcClip, 0))
        {
            if (IsPointInRect(&rcClip, rx, 0))  //RB1
            {
                rmask = 0x80 >> ((cx + rx) % 8);
                pDst = GETXYADDR(pDev, cx + rx,  cy);
                penoffset = nLoopNum % pencount;

                if ((penmask >> penoffset) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + rx - 1;
            }
            
            if (IsPointInRect(&rcClip, -rx, 0))  //RU1
            {
                lmask = 0x80 >> ((cx - rx) % 8);
                pDst = GETXYADDR(pDev, cx - rx,  cy);
                penoffset = (nLoopNum * 3 + flagXEven) % pencount;

                if ((penmask >> penoffset) & penpattern)
                    pixel = ropfunc(pencolor, GETPIXEL(pDst));
                else
                    pixel = ropDashfunc(bkcolor, GETPIXEL(pDst));
                
                PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx - rx + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy, 
                pGraphParam);
        }
    }
    else if (x != rx)
    {
        LINEDATA linedata;
        linedata.y1 = linedata.y2 = cy;
        linedata.x1 = cx + x;
        linedata.x2 = cx + rx;
        DrawThinLine(pDev, pGraphParam, &linedata);
        linedata.x1 = cx - rx;
        linedata.x2 = cx + xNeg;
        DrawThinLine(pDev, pGraphParam, &linedata);
        linedata.y1 = linedata.y2 = cy - 1;
        DrawThinLine(pDev, pGraphParam, &linedata);
        linedata.x1 = cx + x;
        linedata.x2 = cx + rx;
        DrawThinLine(pDev, pGraphParam, &linedata);
    }
    
    _UnexcludeCursor(pDev);
    return 0;
}

#endif //ELLIPSE_SUPPORTED

#ifdef ELLIPSEARC_SUPPORTED

/*********************************************************************\
* Function	   GetIntersectPoint
* Purpose      get the intersect point of two axeses that decided by 
               the two points defined in ptsIn and the ellipse deined in
               the pEllipseData. Link the point and the origin point, 
               then get a axes!
* Params	   
    pEllipseData    Pointer to the ellipse data struct;
    ptsIn           Pointer to the two points that define the axeses;
    ptsOut          Pointer to the result of the intersect point;
* Return	 	   
* Remarks	   
**********************************************************************/
static void GetIntersectPoint(PELLIPSEDATA pEllipseData, 
                             PPOINT ptsIn, PPOINT ptsOut)
{
    int     x1, y1, x2, y2;
    int     x, y, rx, ry, cx, cy;
    int32   flagMiddlePoint;
    int32   Rx2, Ry2, twoRx2, twoRy2, twoRx2y, twoRy2x;
    int32   flag1, flag2;
    int     reverse = 0;

    ASSERT(ptsIn != NULL && ptsOut != NULL);
    ASSERT(pEllipseData != NULL);

    cx = pEllipseData->x;
    cy = pEllipseData->y;
    rx = pEllipseData->d1 / 2;
    ry = pEllipseData->d2 / 2;

    /* ptsIn is a array with two points, get the relative coordinate */
    x1 = ABS(ptsIn[0].x - cx);
    y1 = ABS(ptsIn[0].y - cy);
    x2 = ABS(ptsIn[1].x - cx);
    y2 = ABS(ptsIn[1].y - cy);

    /* exchange the two points, and make the y2 to be the first intersect 
    ** point.
    */
    if (y1 * x2 > y2 * x1) // 1点的轴斜率大于2点，
    {
        EXCHANGE(y1, y2);
        EXCHANGE(x1, x2);
        reverse = 1;
    }

    x = 0;
    y = ry;
    Rx2 = rx * rx;
    Ry2 = ry * ry;
    twoRx2 = Rx2 << 1;
    twoRy2 = Ry2 << 1;
    twoRx2y = twoRx2 * y;
    twoRy2x = 0;
    flagMiddlePoint = Ry2 - Rx2 * ry + (Rx2 >> 2);

    /* flag1 = x1 * y - x * y1 
    ** if (x1 * y == x * y1), then the x, y is the intersect point
    ** The same as flag2.
    */
    flag1 = x1 * y;
    flag2 = x2 * y;

    /* from y to y2 or end in the first part */
    while (twoRy2x < twoRx2y && flag2 > 0) //切线斜率> -1, X控制
    {
        x ++;
        flag2 -= y2;
        flag1 -= y1;
        twoRy2x += twoRy2;
        if (flagMiddlePoint < 0)
        {
            flagMiddlePoint += twoRy2x + Ry2;
        }
        else
        {
            y --;
            flag1 -= x1;
            flag2 -= x2;
            twoRx2y -= twoRx2;
            flagMiddlePoint += twoRy2x - twoRx2y + Ry2;
        }

    }

    if (flag2 <= 0)
    {
        ptsOut[1].x = x;
        ptsOut[1].y = y;
    }
    
    while (twoRy2x < twoRx2y && flag1 > 0) //切线斜率> -1, X控制
    {
        x ++;
        flag1 -= y1;
        twoRy2x += twoRy2;
        if (flagMiddlePoint < 0)
        {
            flagMiddlePoint += twoRy2x + Ry2;
        }
        else
        {
            y --;
            flag1 -= x1;
            twoRx2y -= twoRx2;
            flagMiddlePoint += twoRy2x - twoRx2y + Ry2;
        }

    }

    if (flag1 <= 0)
    {
        ptsOut[0].x = x;
        ptsOut[0].y = y;
    }

    flagMiddlePoint = (int32)(Ry2 * (x + 0.5) * (x + 0.5) + 
        Rx2 * (y - 1) * (y - 1) - Rx2 * Ry2);

    if (flag2 > 0)
    {
        while (y >= 0 && flag2 > 0) //切线斜率< -1, Y控制
        {
            y --;
            flag1 -= x1;
            flag2 -= x2;
            twoRx2y -= twoRx2;
            if (flagMiddlePoint > 0)
            {
                flagMiddlePoint += Rx2 - twoRx2y;
            }
            else
            {
                x ++;
                flag1 -= y1;
                flag2 -= y2;
                twoRy2x += twoRy2;
                flagMiddlePoint += twoRy2x - twoRx2y + Rx2;
            }
        }

        ptsOut[1].x = x;
        ptsOut[1].y = y;
    }

    while (y >= 0 && flag1 > 0) //切线斜率< -1, Y控制
    {
        y --;
        flag1 -= x1;
        twoRx2y -= twoRx2;
        if (flagMiddlePoint > 0)
        {
            flagMiddlePoint += Rx2 - twoRx2y;
        }
        else
        {
            x ++;
            flag1 -= y1;
            twoRy2x += twoRy2;
            flagMiddlePoint += twoRy2x - twoRx2y + Rx2;
        }
    }

    ptsOut[0].x = x;
    ptsOut[0].y = y;

    if (reverse)
    {
        EXCHANGE(ptsOut[0].x, ptsOut[1].x);
        EXCHANGE(ptsOut[0].y, ptsOut[1].y);
    }

    /* get the origin coordinate */
    if (ptsIn[0].x >= cx)
        ptsOut[0].x += cx;
    else
        ptsOut[0].x = cx - ptsOut[0].x;

    if (ptsIn[0].y >= cy)
        ptsOut[0].y += cy;
    else
        ptsOut[0].y = cy - ptsOut[0].y;

    if (ptsIn[1].x >= cx)
        ptsOut[1].x += cx;
    else
        ptsOut[1].x = cx - ptsOut[1].x;

    if (ptsIn[1].y >= cy)
        ptsOut[1].y += cy;
    else
        ptsOut[1].y = cy - ptsOut[1].y;

    return;
}

/*********************************************************************\
* Function	   GetEdgeTable
* Purpose      Get the edge table of a ellipse. The edge table is used
               to realize the scanline algorithm.
* Params	   
    pEllipseArcData     Pointer to the ellipse data
    ptIntersect         Pointer to the output intersect points
    edgeTop             the edge table of the half part of ellipse that
                        y >= 0;
    edgeBottom          the edge table of the half part of ellipse that
                        y < 0;
    mode                AD_COUNTERCLOCKWISE or AD_CLOCKWISE
* Return	 	   
* Remarks	   
**********************************************************************/
static void GetEdgeTable(PELLIPSEARCDATA pEllipseArcData, 
                         PPOINT ptIntersect, PEDGE edgeTop, 
                         PEDGE edgeBottom, int mode)
{
    int i;
    PEDGE pEdgeTop, pEdgeBottom;
    int x1, y1, x2, y2, cx, cy, rx, ry;

    ASSERT(pEllipseArcData != NULL);
    ASSERT(edgeTop != NULL && edgeBottom != NULL && ptIntersect != NULL);

    cx = pEllipseArcData->x;
    cy = pEllipseArcData->y;
    rx = pEllipseArcData->d1 / 2;
    ry = pEllipseArcData->d2 / 2;

    x1 = (ptIntersect[0].x - cx);
    y1 = (ptIntersect[0].y - cy);
    x2 = (ptIntersect[1].x - cx);
    y2 = (ptIntersect[1].y - cy);

    for (i = 0; i < 3; i ++)
    {
        edgeTop[i].x1 = edgeTop[i].x2 = 0;
        edgeTop[i].y1 = edgeTop[i].y2 = -1;
        edgeBottom[i].x1 = edgeBottom[i].x2 = 0;
        edgeBottom[i].y1 = edgeBottom[i].y2 = -1;
    }

    pEdgeTop = edgeTop;
    pEdgeBottom = edgeBottom;

    if (y1 < 0 && y2 < 0)
    {
        y1 = -y1; y2 = -y2;
        if (mode == AD_CLOCKWISE)
            mode = AD_COUNTERCLOCKWISE;
        else
            mode = AD_CLOCKWISE;

        pEdgeTop = edgeBottom;
        pEdgeBottom = edgeTop;

    }

    if ((y1 >= 0 && y2 >= 0))/* 都在同一上半区 */
    {
        /* 当椭圆很狭窄时，也会出现x1 ＝ x2的情况，2002/08/16*/
/*
        if (x1 == x2)
            return;
*/

        if((x1 > x2) ||
            ((x1 == x2) &&
            ((x1 < 0 && y2 < y1) || (x1 >= 0 && y2 > y1))))
        {
            EXCHANGE(x1, x2);
            EXCHANGE(y1, y2);
            if (mode == AD_CLOCKWISE)
                mode = AD_COUNTERCLOCKWISE;
            else
                mode = AD_CLOCKWISE;
        }

        /* x2 > x1 */
        if (mode == AD_COUNTERCLOCKWISE)
        {
            /* 两条轴都在第1象限 */
            if (x1 >= 0)
            {
                pEdgeTop[0].y1 = y1;
                pEdgeTop[0].x1 = x1;
                pEdgeTop[0].y2 = y2;
                pEdgeTop[0].x2 = x2;
            }
            /* 两条轴都在第2象限 */
            else if(x2 <= 0)
            {
                {
                    pEdgeTop[1].y1 = y2;
                    pEdgeTop[1].x1 = x2;
                    pEdgeTop[1].y2 = y1;
                    pEdgeTop[1].x2 = x1;
                }
            }
            /* 起始轴在第2象限，终止轴在第1象限，逆时针 */
            else// if (x2 > 0)
            {
                pEdgeTop[0].y1 = ry;
                pEdgeTop[0].x1 = 0;
                pEdgeTop[0].y2 = y2;
                pEdgeTop[0].x2 = x2;
                pEdgeTop[1].y1 = ry;
                pEdgeTop[1].x1 = 0;
                pEdgeTop[1].y2 = y1;
                pEdgeTop[1].x2 = x1;
            }
        }
        /* 在同一上下半区，但将包含整个另一个半区 */
        else //1,2,3,4,1 顺时针
        {
            /* 两条轴都在第1象限 */
            if (x1 > 0) 
            {
                pEdgeTop[0].y1 = ry;
                pEdgeTop[0].x1 = 0;
                pEdgeTop[0].y2 = y1;
                pEdgeTop[0].x2 = x1;
                pEdgeTop[1].y1 = ry;
                pEdgeTop[1].x1 = 0;
                pEdgeTop[1].y2 = 0;
                pEdgeTop[1].x2 = -rx;
                pEdgeTop[2].y1 = y2;
                pEdgeTop[2].x1 = x2;
                pEdgeTop[2].y2 = 0;
                pEdgeTop[2].x2 = rx;
            }
            /* 两条轴都在第2象限 */
            else if (x2 < 0) 
            {
                pEdgeTop[0].y1 = ry;
                pEdgeTop[0].x1 = 0;
                pEdgeTop[0].y2 = 0;
                pEdgeTop[0].x2 = rx;
                pEdgeTop[1].y1 = ry;
                pEdgeTop[1].x1 = 0;
                pEdgeTop[1].y2 = y2;
                pEdgeTop[1].x2 = x2;
                pEdgeTop[2].y1 = y1;
                pEdgeTop[2].x1 = x1;
                pEdgeTop[2].y2 = 0;
                pEdgeTop[2].x2 = -rx;
            }
            /* 起始轴在第2象限，终止轴在第1象限，顺时针 */
            /* 此时计算top边时，应该以y坐标值作为判断标准，因为可能出现垂直
            ** 线条的绘制 2002/08/16
            */
            else// if (x2 >= 0 && x1 <= 0)
            {
//                if (x2 < rx)
                if (y2 > 0)
                {
                    pEdgeTop[0].y1 = y2;
                    pEdgeTop[0].x1 = x2;
                    pEdgeTop[0].y2 = 0;
                    pEdgeTop[0].x2 = rx;
                }
//                if (x1 > -rx)
                if (y1 > 0)
                {
                    pEdgeTop[1].y1 = y1;
                    pEdgeTop[1].x1 = x1;
                    pEdgeTop[1].y2 = 0;
                    pEdgeTop[1].x2 = -rx;
                }
            }
            pEdgeBottom[0].y1 = ry;
            pEdgeBottom[0].x1 = 0;
            pEdgeBottom[0].y2 = 0;
            pEdgeBottom[0].x2 = rx;
            pEdgeBottom[1].y1 = ry;
            pEdgeBottom[1].x1 = 0;
            pEdgeBottom[1].y2 = 0;
            pEdgeBottom[1].x2 = -rx;
        }
    }

    if (y1 < 0 && y2 >= 0)
    {
        EXCHANGE(y1, y2);
        EXCHANGE(x1, x2);
        if (mode == AD_CLOCKWISE)
            mode = AD_COUNTERCLOCKWISE;
        else
            mode = AD_CLOCKWISE;
    }
    if (y1 >= 0 && y2 < 0)
    {
        if (mode == AD_CLOCKWISE)
        {
            if (x1 > 0) //1,2象限
            {
                edgeTop[0].y1 = ry;
                edgeTop[0].x1 = 0;
                edgeTop[0].y2 = y1;
                edgeTop[0].x2 = x1;
                edgeTop[1].y1 = ry;
                edgeTop[1].x1 = 0;
                edgeTop[1].y2 = 0;
                edgeTop[1].x2 = -rx;
            }
            else if (x1 >= -rx)//2象限
            {
                edgeTop[1].y1 = y1;
                edgeTop[1].x1 = x1;
                edgeTop[1].y2 = 0;
                edgeTop[1].x2 = -rx;
            }

            if (x2 > 0)//3，4象限
            {
                edgeBottom[0].y1 = ry;
                edgeBottom[0].x1 = 0;
                edgeBottom[0].y2 = -y2;
                edgeBottom[0].x2 = x2;
                edgeBottom[1].y1 = ry;
                edgeBottom[1].x1 = 0;
                edgeBottom[1].y2 = 0;
                edgeBottom[1].x2 = -rx;
            }
            else if (x2 >= -rx) //3象限
            {
                edgeBottom[1].y1 = -y2;
                edgeBottom[1].x1 = x2;
                edgeBottom[1].y2 = 0;
                edgeBottom[1].x2 = -rx;
            }
        }

        else// if (mode == AD_COUNTERCLOCKWISE)
        {
            if (x1 < 0)//1，2象限
            {
                edgeTop[0].y1 = ry;
                edgeTop[0].x1 = 0;
                edgeTop[0].y2 = 0;
                edgeTop[0].x2 = rx;
                edgeTop[1].y1 = ry;
                edgeTop[1].x1 = 0;
                edgeTop[1].y2 = y1;
                edgeTop[1].x2 = x1;
            }
            else if (x1 <= rx) //1象限
            {
                edgeTop[0].y1 = y1;
                edgeTop[0].x1 = x1;
                edgeTop[0].y2 = 0;
                edgeTop[0].x2 = rx;
            }

            if (x2 < 0) //3,4象限
            {
                edgeBottom[0].y1 = ry;
                edgeBottom[0].x1 = 0;
                edgeBottom[0].y2 = 0;
                edgeBottom[0].x2 = rx;
                edgeBottom[1].y1 = ry;
                edgeBottom[1].x1 = 0;
                edgeBottom[1].y2 = -y2;
                edgeBottom[1].x2 = x2;
            }
            else if (x2 <= rx)//4象限
            {
                edgeBottom[0].y1 = -y2;
                edgeBottom[0].x1 = x2;
                edgeBottom[0].y2 = 0;
                edgeBottom[0].x2 = rx;
            }
        }
    }
}

/*********************************************************************\
* Function	   DrawThinEllipseArc
* Purpose      Draw a thin ellipse arc
* Params	   
    pDev                Pointer to the device
    pGraphParam         Pointer to the draw parameter
    pEllipseArcData     Pointer to the ellipse arc data
* Return	 	   
    success     1
    failed      -1
* Remarks	   
**********************************************************************/
static int DrawThinEllipseArc(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEARCDATA pEllipseArcData)
{
    int     x, y, cx, cy, rx, ry;
    POINT   ptIn[2], ptIntersect[2];
    EDGE    edgeTop[3], edgeBottom[3];
    PEDGE   pEdge5;         /* 第五条边，只在两轴均在同一象限时有效 */
    int     factX, factY;   /* 用于同一象限中的大包含情况 */
    int32   flagMiddlePoint;
    int32   Rx2, Ry2, twoRx2, twoRy2, twoRx2y, twoRy2x;
    uint8   *pDst;
    uint32  pencolor, pixel;
    uint8   lmask, rmask;
    uint32  rop;
    ROPFUNC ropfunc;
    RECT cliprc;
    ELLIPSEDATA ellipsedata;

    ASSERT(pGraphParam != NULL);
    if (pGraphParam->pPen == NULL || pGraphParam->pPen->style == PS_NULL)
        return 0;

    GetDevRect(&cliprc, pDev);
    if (pGraphParam->flags & GPF_CLIP_RECT)
        ClipRect(&cliprc, &pGraphParam->clip_rect);
    if (!IsValidRect(&cliprc))
        return -1;

    _ExcludeCursor(pDev, &cliprc);

    rop = SRop3ToRop2(pGraphParam->rop);
    ropfunc = GetRopFunc(rop);
    pencolor = (pGraphParam->pPen->fgcolor == 0) ? 0 : 0xFFFFFFFFL;

    ptIn[0].x = pEllipseArcData->start_x;
    ptIn[0].y = pEllipseArcData->start_y;
    ptIn[1].x = pEllipseArcData->end_x;
    ptIn[1].y = pEllipseArcData->end_y;

    ellipsedata.x = pEllipseArcData->x;
    ellipsedata.y = pEllipseArcData->y;
    ellipsedata.d1 = pEllipseArcData->d1;
    ellipsedata.d2 = pEllipseArcData->d2;
    GetIntersectPoint(&ellipsedata, ptIn, ptIntersect);
    
    /* in a same axes, do nothing and return */
    if (ptIntersect[0].x == ptIntersect[1].x && 
        ptIntersect[0].y == ptIntersect[1].y)
        return 1;

    GetEdgeTable(pEllipseArcData, ptIntersect, edgeTop, edgeBottom, 
        pGraphParam->arcDirection);

    pEllipseArcData->start_x = ptIntersect[0].x;
    pEllipseArcData->start_y = ptIntersect[0].y;
    pEllipseArcData->end_x = ptIntersect[1].x;
    pEllipseArcData->end_y = ptIntersect[1].y;


    if (edgeTop[2].y1 >= 0)
    {
        ASSERT(edgeBottom[2].y1 < 0);
        pEdge5 = &edgeTop[2];
    }
    else
    {
        ASSERT(edgeTop[2].y1 < 0);
        pEdge5 = &edgeBottom[2];
    }
    
    cx = pEllipseArcData->x;
    cy = pEllipseArcData->y;
    rx = pEllipseArcData->d1 / 2;
    ry = pEllipseArcData->d2 / 2;

    cliprc.x1 -= cx;
    cliprc.y1 -= cy;
    cliprc.x2 -= cx;
    cliprc.y2 -= cy;

    x = 0;
    y = ry;
    Rx2 = rx * rx;
    Ry2 = ry * ry;
    twoRx2 = Rx2 << 1;
    twoRy2 = Ry2 << 1;
    twoRx2y = twoRx2 * y;
    twoRy2x = 0;
    flagMiddlePoint = Ry2 - Rx2 * ry + (Rx2 >> 2);

    if (ptIntersect[0].y > cy)
        factY = 1;
    else
        factY = -1;
    if (ptIntersect[0].x > cx)
        factX = 1;
    else
        factX = -1;

    while (twoRy2x < twoRx2y) //切线斜率> -1, X控制
    {
        rmask = 0x80 >> ((x + cx) % 8);
        lmask = 0x80 >> ((-x + cx) % 8);
        if (IsPtInYClip(cliprc, y))
        {
            if (x >= edgeTop[0].x1 && x < edgeTop[0].x2 && 
                IsPtInXClip(cliprc, x))
            {
                pDst = GETXYADDR(pDev, x + cx, y + cy);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, rmask);
            }
            if (x >= -edgeTop[1].x1 && x < -edgeTop[1].x2 && 
                IsPtInXClip(cliprc, -x))
            {
                pDst = GETXYADDR(pDev, -x + cx, y + cy);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, lmask);
            }
        }
        if (IsPtInYClip(cliprc, -y))
        {
            if (x >= edgeBottom[0].x1 && x < edgeBottom[0].x2 && 
                IsPtInXClip(cliprc, x))
            {
                pDst = GETXYADDR(pDev, x + cx, -y + cy);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, rmask);
            }
            if (x >= -edgeBottom[1].x1 && x < -edgeBottom[1].x2 && 
                IsPtInXClip(cliprc, -x))
            {
                pDst = GETXYADDR(pDev, -x + cx, -y + cy);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, lmask);
            }
        }
        if (x >= factX * pEdge5->x1 && x < factX * pEdge5->x2 && 
                IsPointInRect(&cliprc, x * factX, y * factY))
        {
            lmask = 0x80 >> ((x * factX + cx) % 8);
            pDst = GETXYADDR(pDev, x * factX + cx, y * factY + cy);
            pixel = ropfunc(pencolor, GETPIXEL(pDst));
            PUTPIXEL(pDst, pixel, lmask);
        }

        x ++;
        twoRy2x += twoRy2;
        if (flagMiddlePoint < 0)
        {
            flagMiddlePoint += twoRy2x + Ry2;
        }
        else
        {
            y --;
            twoRx2y -= twoRx2;
            flagMiddlePoint += twoRy2x - twoRx2y + Ry2;
        }

    }

    flagMiddlePoint = (int32)(Ry2 * (x + 0.5) * (x + 0.5) + 
        Rx2 * (y - 1) * (y - 1) - Rx2 * Ry2);

    while (y >= 0) //切线斜率< -1, Y控制
    {
        rmask = 0x80 >> ((x + cx) % 8);
        lmask = 0x80 >> ((-x + cx) % 8);
        if (IsPtInYClip(cliprc, y))
        {
            if (y <= edgeTop[0].y1 && y >= edgeTop[0].y2 && 
                IsPtInXClip(cliprc, x))
            {
                pDst = GETXYADDR(pDev, x + cx, y + cy);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, rmask);
            }
            if (y <= edgeTop[1].y1 && y >= edgeTop[1].y2 && 
                IsPtInXClip(cliprc, -x))
            {
                pDst = GETXYADDR(pDev, -x + cx, y + cy);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, lmask);
            }
        }
        if (IsPtInYClip(cliprc, -y))
        {
            if (y <= edgeBottom[0].y1 && y >= edgeBottom[0].y2 && 
                IsPtInXClip(cliprc, x))
            {
                pDst = GETXYADDR(pDev, x + cx, -y + cy);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, rmask);
            }
            if (y <= edgeBottom[1].y1 && y >= edgeBottom[1].y2 && 
                IsPtInXClip(cliprc, -x))
            {
                pDst = GETXYADDR(pDev, -x + cx, -y + cy);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, lmask);
            }
        }
        if (y <= pEdge5->y1 && y > pEdge5->y2 && 
                IsPointInRect(&cliprc, x * factX, y * factY))
        {
            lmask = 0x80 >> ((x * factX + cx) % 8);
            pDst = GETXYADDR(pDev, x * factX + cx, y * factY + cy);
            pixel = ropfunc(pencolor, GETPIXEL(pDst));
            PUTPIXEL(pDst, pixel, lmask);
        }

        y --;
        twoRx2y -= twoRx2;
        if (flagMiddlePoint > 0)
        {
            flagMiddlePoint += Rx2 - twoRx2y;
        }
        else
        {
            x ++;
            twoRy2x += twoRy2;
            flagMiddlePoint += twoRy2x - twoRx2y + Rx2;
        }
    }

    _UnexcludeCursor(pDev);
    return 1;
}

#define LEFTELLIPSEARC  11
#define RIGHTELLIPSEARC 12
/*********************************************************************\
* Function	   DrawPieOrChord
* Purpose      Draw the ellipse arc and fill it with Pie or Chord mode.
* Params	   
* Return	 	   
* Remarks	   Use the scan line algorithm
**********************************************************************/
static int DrawPieOrChord(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEARCDATA pEllipseArcData)
{
    int     x, y, cx, cy, rx, ry;
    POINT   ptIn[2], ptIntersect[2], ptChord;
    EDGE    EdgeArcTop[3], EdgeArcBot[3], EdgeLineTop[2], EdgeLineBot[2];
    PEDGE   pEdgeTop, pEdgeBot, pEdgeTopNode, pEdgeBotNode;
    PEDGE   pActEdgeTop, pActEdgeBot, pActEdgeNode, pPreEdge;
    int32   flagMiddlePoint;
    int32   Rx2, Ry2, twoRx2, twoRy2, twoRx2y, twoRy2x;
    BOOL    bSecondPart = FALSE;
	int     i, l, r;
    RECT    cliprc;
    LINEDATA linedata;
    ELLIPSEDATA ellipsedata;

    ASSERT(pDev != NULL && pGraphParam != NULL);
    ASSERT(pEllipseArcData != NULL);
    
    GetDevRect(&cliprc, pDev);
    if (pGraphParam->flags & GPF_CLIP_RECT)
        ClipRect(&cliprc, &pGraphParam->clip_rect);
    if (!IsValidRect(&cliprc))
        return -1;

    _ExcludeCursor(pDev, &cliprc);

    ptIn[0].x = pEllipseArcData->start_x;
    ptIn[0].y = pEllipseArcData->start_y;
    ptIn[1].x = pEllipseArcData->end_x;
    ptIn[1].y = pEllipseArcData->end_y;

    ellipsedata.x = pEllipseArcData->x;
    ellipsedata.y = pEllipseArcData->y;
    ellipsedata.d1 = pEllipseArcData->d1;
    ellipsedata.d2 = pEllipseArcData->d2;
    GetIntersectPoint(&ellipsedata, ptIn, ptIntersect);

    /* in a same axes, do nothing and return */
    if (ptIntersect[0].x == ptIntersect[1].x && 
        ptIntersect[0].y == ptIntersect[1].y)
        return -1;

    GetEdgeTable(pEllipseArcData, ptIntersect, EdgeArcTop, EdgeArcBot, 
        pGraphParam->arcDirection);

    cx = pEllipseArcData->x;
    cy = pEllipseArcData->y;
    rx = pEllipseArcData->d1 / 2;
    ry = pEllipseArcData->d2 / 2;

    pEdgeTop = pEdgeBot = NULL;
    pActEdgeTop = pActEdgeBot = NULL;
    
    /* Add the arc edge to the edge table.
    ** Define two edge table that represent the two part of ellipse, one is
    ** y >= cy and the other is y < cy.
    */
    /* add the top arc edge */
    for (i = 0; i < 3; i++)
    {
        /* 增加了对圆弧y1 ＝ y2时的判断，此时忽略这条弧 2002/08/16 */
        if (EdgeArcTop[i].y1 >= 0 && EdgeArcTop[i].y1 != EdgeArcTop[i].y2)
        {
            if (EdgeArcTop[i].x2 < 0)
                EdgeArcTop[i].ctrl = LEFTELLIPSEARC;
            else
                EdgeArcTop[i].ctrl = RIGHTELLIPSEARC;
            EdgeArcTop[i].cx = EdgeArcTop[i].x1 + cx;
            
            InsertEdgeReverse(&pEdgeTop, &EdgeArcTop[i]);
        }

        if (EdgeArcBot[i].y1 >= 0 && EdgeArcBot[i].y1 != EdgeArcBot[i].y2)
        {
            if (EdgeArcBot[i].x2 < 0)
                EdgeArcBot[i].ctrl = LEFTELLIPSEARC;
            else
                EdgeArcBot[i].ctrl = RIGHTELLIPSEARC;
            EdgeArcBot[i].cx = EdgeArcBot[i].x1 + cx;

            InsertEdgeReverse(&pEdgeBot, &EdgeArcBot[i]);
        }
    }
    /* Add the line edge to the edge table.
    ** There are two case that represent the Pie and the Chord.
    */
    if (pEllipseArcData->mode == AM_LUNE && 
        ptIntersect[0].y != ptIntersect[1].y)
    {
        if (ptIntersect[0].y < ptIntersect[1].y)
        {
            EXCHANGE(ptIntersect[0].x, ptIntersect[1].x);
            EXCHANGE(ptIntersect[0].y, ptIntersect[1].y);
        }
        /* the line is totally in top part of ellipse */
        if (ptIntersect[1].y >= cy)
        {
            EdgeLineTop[0].x1 = ptIntersect[0].x - cx;
            EdgeLineTop[0].y1 = ptIntersect[0].y - cy;
            EdgeLineTop[0].x2 = ptIntersect[1].x - cx;
            EdgeLineTop[0].y2 = ptIntersect[1].y - cy;

            EdgeLineTop[0].cx = ptIntersect[0].x;
            EdgeLineTop[0].dx = (EdgeLineTop[0].x2 - EdgeLineTop[0].x1) * 2;
            EdgeLineTop[0].dy = (EdgeLineTop[0].y1 - EdgeLineTop[0].y2) * 2;
            if (ABS(EdgeLineTop[0].dx) >= EdgeLineTop[0].dy)
            {
                EdgeLineTop[0].ctrl = LINE_CTRLX;
                EdgeLineTop[0].flag = 0;
            }
            else
            {
                EdgeLineTop[0].ctrl = LINE_CTRLY;
                EdgeLineTop[0].flag = 
                    ABS(EdgeLineTop[0].dx) - EdgeLineTop[0].dy;
            }

            InsertEdgeReverse(&pEdgeTop, &EdgeLineTop[0]);
        }
        /* the line is devide into two part respectively in two part of 
        ** ellipse
        */
        else if (ptIntersect[0].y >= cy)//(ptIntersect[1].y < cy)
        {
            EdgeLineTop[0].x1 = ptIntersect[0].x - cx;
            EdgeLineTop[0].y1 = ptIntersect[0].y - cy;
            EdgeLineTop[0].cx = ptIntersect[0].x;

            EdgeLineTop[0].dx = (ptIntersect[1].x - ptIntersect[0].x);
            EdgeLineTop[0].dy = (ptIntersect[0].y - ptIntersect[1].y);
            ptChord.y = 0;
            ptChord.x = ptIntersect[0].x - 
                DIV_CEILING(EdgeLineTop[0].y1 * EdgeLineTop[0].dx, 
                EdgeLineTop[0].dy);

            EdgeLineTop[0].dx *= 2;
            EdgeLineTop[0].dy *= 2;

            EdgeLineTop[0].x2 = ptChord.x;
            EdgeLineTop[0].y2 = ptChord.y;

            if (ABS(EdgeLineTop[0].dx) >= EdgeLineTop[0].dy)
            {
                EdgeLineTop[0].ctrl = LINE_CTRLX;
                EdgeLineTop[0].flag = 0;
            }
            else
            {
                EdgeLineTop[0].ctrl = LINE_CTRLY;
                EdgeLineTop[0].flag = 
                    ABS(EdgeLineTop[0].dx) - EdgeLineTop[0].dy;
            }

            InsertEdgeReverse(&pEdgeTop, &EdgeLineTop[0]);

            EdgeLineBot[0].cx = ptIntersect[1].x;
            EdgeLineBot[0].dx = EdgeLineTop[0].dx;
            EdgeLineBot[0].dy = EdgeLineTop[0].dy;

            EdgeLineBot[0].x1 = ptIntersect[1].x;
            EdgeLineBot[0].y1 = cy - ptIntersect[1].y;
            EdgeLineBot[0].x2 = ptChord.x;
            EdgeLineBot[0].y2 = ptChord.y;

            EdgeLineBot[0].ctrl = EdgeLineTop[0].ctrl;
            EdgeLineBot[0].flag = EdgeLineTop[0].flag;

            InsertEdgeReverse(&pEdgeBot, &EdgeLineBot[0]);
        }
        /* the line is total in the bottom part of ellipse*/
        else //(ptIntersect[0].y < cy) && (ptIntersect[1].y < cy)
        {
            EdgeLineBot[0].x1 = ptIntersect[1].x - cx;
            EdgeLineBot[0].y1 = cy - ptIntersect[1].y;
            EdgeLineBot[0].x2 = ptIntersect[0].x - cx;
            EdgeLineBot[0].y2 = cy - ptIntersect[0].y;

            EdgeLineBot[0].cx = ptIntersect[1].x;
            EdgeLineBot[0].dx = (EdgeLineBot[0].x2 - EdgeLineBot[0].x1) * 2;
            EdgeLineBot[0].dy = (EdgeLineBot[0].y1 - EdgeLineBot[0].y2) * 2;
            if (ABS(EdgeLineBot[0].dx) >= EdgeLineBot[0].dy)
            {
                EdgeLineBot[0].ctrl = LINE_CTRLX;
                EdgeLineBot[0].flag = 0;
            }
            else
            {
                EdgeLineBot[0].ctrl = LINE_CTRLY;
                EdgeLineBot[0].flag = 
                    ABS(EdgeLineBot[0].dx) - EdgeLineBot[0].dy;
            }

            InsertEdgeReverse(&pEdgeBot, &EdgeLineBot[0]);
        }
    }
    /* In the Pie case, the two line is desided by the origin point and 
    ** the intersect point
    */
    else if (pEllipseArcData->mode == AM_SECTOR)
    {
        /* ptIntersect[i].y = cy is canceled*/
        for (i = 0; i < 2; i++)
        {
            if (ptIntersect[i].y > cy)
            {
                EdgeLineTop[i].x1 = ptIntersect[i].x - cx;
                EdgeLineTop[i].y1 = ptIntersect[i].y - cy;
                EdgeLineTop[i].x2 = 0;
                EdgeLineTop[i].y2 = 0;

                EdgeLineTop[i].cx = ptIntersect[i].x;
                EdgeLineTop[i].dx = -EdgeLineTop[i].x1 * 2;
                EdgeLineTop[i].dy = EdgeLineTop[i].y1 * 2;
                if (ABS(EdgeLineTop[i].dx) >= EdgeLineTop[i].dy)
                {
                    EdgeLineTop[i].ctrl = LINE_CTRLX;
                    EdgeLineTop[i].flag = 0;
                }
                else
                {
                    EdgeLineTop[i].ctrl = LINE_CTRLY;
                    EdgeLineTop[i].flag = 
                        ABS(EdgeLineTop[i].dx) - EdgeLineTop[i].dy;
                }

                InsertEdgeReverse(&pEdgeTop, &EdgeLineTop[i]);
            }

            else if (ptIntersect[i].y < cy)
            {
                EdgeLineBot[i].x1 = ptIntersect[i].x - cx;
                EdgeLineBot[i].y1 = cy - ptIntersect[i].y;
                EdgeLineBot[i].x2 = 0;
                EdgeLineBot[i].y2 = 0;

                EdgeLineBot[i].cx = ptIntersect[i].x;
                EdgeLineBot[i].dx = -EdgeLineBot[i].x1 * 2;
                EdgeLineBot[i].dy = EdgeLineBot[i].y1 * 2;
                if (ABS(EdgeLineBot[i].dx) >= EdgeLineBot[i].dy)
                {
                    EdgeLineBot[i].ctrl = LINE_CTRLX;
                    EdgeLineBot[i].flag = 0;
                }
                else
                {
                    EdgeLineBot[i].ctrl = LINE_CTRLY;
                    EdgeLineBot[i].flag = 
                        ABS(EdgeLineBot[i].dx) - EdgeLineBot[i].dy;
                }

                InsertEdgeReverse(&pEdgeBot, &EdgeLineBot[i]);
            }
        }
    }

    x = 0;
    y = ry;
    Rx2 = rx * rx;
    Ry2 = ry * ry;
    twoRx2 = Rx2 << 1;
    twoRy2 = Ry2 << 1;
    twoRx2y = twoRx2 * y;
    twoRy2x = 0;
    flagMiddlePoint = Ry2 - Rx2 * ry + (Rx2 >> 2);
    pEdgeTopNode = pEdgeTop;
    pEdgeBotNode = pEdgeBot;

    do 
    {
		/* add edges to the active edge table from the edge table */
		while (pEdgeTopNode)
        {
            if (pEdgeTopNode->y1 == y)
            {
                pEdgeTop = pEdgeTop->pNext;
                InsertEdgeReverse(&pActEdgeTop, pEdgeTopNode);
            }
            else if (pEdgeTopNode->y1 < y)
                break;

            pEdgeTopNode = pEdgeTop;
		}

        SortActiveEdge(&pActEdgeTop);

		while (pEdgeBotNode)
        {
            if (pEdgeBotNode->y1 == y)
            {
                pEdgeBot = pEdgeBot->pNext;
                InsertEdgeReverse(&pActEdgeBot, pEdgeBotNode);
            }
            else if (pEdgeBotNode->y1 < y)
                break;

            pEdgeBotNode = pEdgeBot;
		}

        SortActiveEdge(&pActEdgeBot);

        /* draw the top part */
        pActEdgeNode = pActEdgeTop;
		while (pActEdgeNode)
        {
            ASSERT(pActEdgeNode->pNext);

            if (IsPtInYClip(cliprc, y + cy))
            {
                if (pActEdgeNode->ctrl == LINE_CTRLY)
                {
                    if (pActEdgeNode->dx > 0)
                        l = pActEdgeNode->cx + 1;
                    else
                        l = pActEdgeNode->cx;
                }
                else if (pActEdgeNode->ctrl == LINE_CTRLX)
                {
                    l = pActEdgeNode->cx + 1;
                }
                else if (pActEdgeNode->ctrl == LEFTELLIPSEARC || 
                    pActEdgeNode->ctrl == RIGHTELLIPSEARC)
                {
                    l = pActEdgeNode->cx + 1;
                }

                pActEdgeNode = pActEdgeNode->pNext;

                if (pActEdgeNode->ctrl == LINE_CTRLY)
                {
                    if (pActEdgeNode->dx > 0)
                        r = pActEdgeNode->cx;
                    else
                        r = pActEdgeNode->cx - 1;
                }
                else if (pActEdgeNode->ctrl == LINE_CTRLX)
                {
                    r = pActEdgeNode->cx - 1;
                }
                else if (pActEdgeNode->ctrl == LEFTELLIPSEARC || 
                    pActEdgeNode->ctrl == RIGHTELLIPSEARC)
                {
                    r = pActEdgeNode->cx - 1;
                }

                pActEdgeNode = pActEdgeNode->pNext;

                if (l < cliprc.x1)
                    l = cliprc.x1;

                if (r > cliprc.x2)
                    r = cliprc.x2 - 1;

                if (r >= l)
                    PatBltHorizonalLine(pDev, l, r, y + cy, pGraphParam);
            }
            else
            {
                break;
            }
		}

        /* draw the bottom part */
        pActEdgeNode = pActEdgeBot;
		while (pActEdgeNode)
        {
            ASSERT(pActEdgeNode->pNext);

            if (IsPtInYClip(cliprc, -y + cy))
            {
                if (pActEdgeNode->ctrl == LINE_CTRLY)
                {
                    if (pActEdgeNode->dx > 0)
                        l = pActEdgeNode->cx + 1;
                    else
                        l = pActEdgeNode->cx;
                }
                else if (pActEdgeNode->ctrl == LINE_CTRLX)
                {
                    l = pActEdgeNode->cx + 1;
                }
                else if (pActEdgeNode->ctrl == LEFTELLIPSEARC || 
                    pActEdgeNode->ctrl == RIGHTELLIPSEARC)
                {
                    l = pActEdgeNode->cx + 1;
                }

                pActEdgeNode = pActEdgeNode->pNext;

                if (pActEdgeNode->ctrl == LINE_CTRLY)
                {
                    if (pActEdgeNode->dx > 0)
                        r = pActEdgeNode->cx;
                    else
                        r = pActEdgeNode->cx - 1;
                }
                else if (pActEdgeNode->ctrl == LINE_CTRLX)
                {
                    r = pActEdgeNode->cx - 1;
                }
                else if (pActEdgeNode->ctrl == LEFTELLIPSEARC || 
                    pActEdgeNode->ctrl == RIGHTELLIPSEARC)
                {
                    r = pActEdgeNode->cx - 1;
                }

                pActEdgeNode = pActEdgeNode->pNext;

                if (l < cliprc.x1)
                    l = cliprc.x1;

                if (r > cliprc.x2)
                    r = cliprc.x2 - 1;

                if (r >= l)
                    PatBltHorizonalLine(pDev, l, r, -y + cy, pGraphParam);
            }
            else
            {
                break;
            }
		}
        
        /* update the ellipse data*/
        while (twoRy2x < twoRx2y) //切线斜率> -1, X控制
        {
            x ++;
            twoRy2x += twoRy2;
            if (flagMiddlePoint < 0)
            {
                flagMiddlePoint += twoRy2x + Ry2;
            }
            else
            {
                y --;
                twoRx2y -= twoRx2;
                flagMiddlePoint += twoRy2x - twoRx2y + Ry2;
                break;
            }
        }
        if (bSecondPart && y >= 0)
        {
            /* In the second part of drawing a ellipse */
            y --;
            twoRx2y -= twoRx2;
            if (flagMiddlePoint > 0)
            {
                flagMiddlePoint += Rx2 - twoRx2y;
            }
            else
            {
                x ++;
                twoRy2x += twoRy2;
                flagMiddlePoint += twoRy2x - twoRx2y + Rx2;
            }
        }

        if (!bSecondPart && twoRy2x >= twoRx2y)
        {
            flagMiddlePoint = (int32)(Ry2 * (x + 0.5) * (x + 0.5) + 
                Rx2 * (y - 1) * (y - 1) - Rx2 * Ry2);
            bSecondPart = TRUE;
        }
        
        /*
        ** When y = 0, the active edge table isn't deleted and retained
        ** to draw the last line.
        */
        if (y != 0)
        {
            /* delete the node of the top act edge.*/
            while (pActEdgeTop)
            {
                if (pActEdgeTop->y2 == y)
                    pActEdgeTop = pActEdgeTop->pNext;
                else
                    break;
            }
            
            pPreEdge = pActEdgeTop;
            pActEdgeNode = pActEdgeTop;
            while (pActEdgeNode)
            {
                if (pActEdgeNode->y2 == y)
                {
                    pPreEdge->pNext = pActEdgeNode->pNext;
                    pActEdgeNode = pActEdgeNode->pNext;
                    continue;
                }
                
                pPreEdge = pActEdgeNode;
                pActEdgeNode = pActEdgeNode->pNext;
            }

            /* delete the node of the bottom act edge */
            while (pActEdgeBot)
            {
                if (pActEdgeBot->y2 == y)
                    pActEdgeBot = pActEdgeBot->pNext;
                else
                    break;
            }

            pPreEdge = pActEdgeBot;
            pActEdgeNode = pActEdgeBot;
	        while (pActEdgeNode)
            {
		        if (pActEdgeNode->y2 == y)
                {
                    pPreEdge->pNext = pActEdgeNode->pNext;
                    pActEdgeNode = pActEdgeNode->pNext;
                    continue;
                }

                pPreEdge = pActEdgeNode;
                pActEdgeNode = pActEdgeNode->pNext;
            }
        }

        /* calculate the new value of top act edge */
        pActEdgeNode = pActEdgeTop;
		while (pActEdgeNode)
		{
            if (pActEdgeNode->ctrl == LINE_CTRLX)
            {
                pActEdgeNode->flag += pActEdgeNode->dx;
				pActEdgeNode->cx += pActEdgeNode->flag / pActEdgeNode->dy;
                pActEdgeNode->flag %= pActEdgeNode->dy;
            }
            else if (pActEdgeNode->ctrl == LINE_CTRLY)
            {
                int dx, increase;
                if (pActEdgeNode->dx < 0)
                {
                    dx = - pActEdgeNode->dx;
                    increase = -1;
                }
                else
                {
                    dx = pActEdgeNode->dx;
                    increase = 1;
                }
                
                if (pActEdgeNode->flag >= 0)
                {
                    pActEdgeNode->cx += increase;
                    pActEdgeNode->flag -= pActEdgeNode->dy;
                }
                pActEdgeNode->flag += dx;
            }
            else if (pActEdgeNode->ctrl == LEFTELLIPSEARC)
            {
                pActEdgeNode->cx = -x + cx;
            }
            else if (pActEdgeNode->ctrl == RIGHTELLIPSEARC)
            {
                pActEdgeNode->cx = x + cx;
            }

            pActEdgeNode = pActEdgeNode->pNext;
		}

        /* calculate the new value of bottom act edge */
        pActEdgeNode = pActEdgeBot;
		while (pActEdgeNode)
		{
            if (pActEdgeNode->ctrl == LINE_CTRLX)
            {
                pActEdgeNode->flag += pActEdgeNode->dx;
				pActEdgeNode->cx += pActEdgeNode->flag / pActEdgeNode->dy;
                pActEdgeNode->flag %= pActEdgeNode->dy;
            }
            else if (pActEdgeNode->ctrl == LINE_CTRLY)
            {
                int dx, increase;
                if (pActEdgeNode->dx < 0)
                {
                    dx = - pActEdgeNode->dx;
                    increase = -1;
                }
                else
                {
                    dx = pActEdgeNode->dx;
                    increase = 1;
                }
                
                if (pActEdgeNode->flag >= 0)
                {
                    pActEdgeNode->cx += increase;
                    pActEdgeNode->flag -= pActEdgeNode->dy;
                }
                pActEdgeNode->flag += dx;
            }
            else if (pActEdgeNode->ctrl == LEFTELLIPSEARC)
            {
                pActEdgeNode->cx = -x + cx;
            }
            else if (pActEdgeNode->ctrl == RIGHTELLIPSEARC)
            {
                pActEdgeNode->cx = x + cx;
            }

            pActEdgeNode = pActEdgeNode->pNext;
		}

    } while (y > 0);

    /* draw the last line the at y == 0*/
    if (pActEdgeTop)
    {
        /* draw the top part */
        pActEdgeNode = pActEdgeTop;
        while (pActEdgeNode)
        {
            ASSERT(pActEdgeNode->pNext);
            
            if (IsPtInYClip(cliprc, cy))
            {
                if (pActEdgeNode->ctrl == LINE_CTRLY)
                {
                    if (pActEdgeNode->dx > 0)
                        l = pActEdgeNode->cx + 1;
                    else
                        l = pActEdgeNode->cx;
                }
                else if (pActEdgeNode->ctrl == LINE_CTRLX)
                {
                    l = pActEdgeNode->cx + 1;
                }
                else if (pActEdgeNode->ctrl == LEFTELLIPSEARC || 
                    pActEdgeNode->ctrl == RIGHTELLIPSEARC)
                {
                    l = pActEdgeNode->cx + 1;
                }

                pActEdgeNode = pActEdgeNode->pNext;
            
                if (pActEdgeNode->ctrl == LINE_CTRLY)
                {
                    if (pActEdgeNode->dx > 0)
                        r = pActEdgeNode->cx;
                    else
                        r = pActEdgeNode->cx - 1;
                }
                else if (pActEdgeNode->ctrl == LINE_CTRLX)
                {
                    r = pActEdgeNode->cx - 1;
                }
                else if (pActEdgeNode->ctrl == LEFTELLIPSEARC || 
                    pActEdgeNode->ctrl == RIGHTELLIPSEARC)
                {
                    r = pActEdgeNode->cx - 1;
                }
            
                pActEdgeNode = pActEdgeNode->pNext;
            
                if (l < cliprc.x1)
                    l = cliprc.x1;
                
                if (r > cliprc.x2)
                    r = cliprc.x2 - 1;
                
                if (r >= l)
                    PatBltHorizonalLine(pDev, l, r, cy, pGraphParam);
            }
            else
            {
                break;
            }
        }
    }

    else if (pActEdgeBot)
    {
        /* draw the bottom part */
        pActEdgeNode = pActEdgeBot;
        while (pActEdgeNode)
        {
            ASSERT(pActEdgeNode->pNext);
            
            if (IsPtInYClip(cliprc, cy))
            {
                if (pActEdgeNode->ctrl == LINE_CTRLY)
                {
                    if (pActEdgeNode->dx > 0)
                        l = pActEdgeNode->cx + 1;
                    else
                        l = pActEdgeNode->cx;
                }
                else if (pActEdgeNode->ctrl == LINE_CTRLX)
                {
                    l = pActEdgeNode->cx + 1;
                }
                else if (pActEdgeNode->ctrl == LEFTELLIPSEARC || 
                    pActEdgeNode->ctrl == RIGHTELLIPSEARC)
                {
                    l = pActEdgeNode->cx + 1;
                }

                pActEdgeNode = pActEdgeNode->pNext;
            
                if (pActEdgeNode->ctrl == LINE_CTRLY)
                {
                    if (pActEdgeNode->dx > 0)
                        r = pActEdgeNode->cx;
                    else
                        r = pActEdgeNode->cx - 1;
                }
                else if (pActEdgeNode->ctrl == LINE_CTRLX)
                {
                    r = pActEdgeNode->cx - 1;
                }
                else if (pActEdgeNode->ctrl == LEFTELLIPSEARC || 
                    pActEdgeNode->ctrl == RIGHTELLIPSEARC)
                {
                    r = pActEdgeNode->cx - 1;
                }
            
                pActEdgeNode = pActEdgeNode->pNext;
            
                if (l < cliprc.x1)
                    l = cliprc.x1;
                
                if (r > cliprc.x2)
                    r = cliprc.x2 - 1;
                
                if (r >= l)
                    PatBltHorizonalLine(pDev, l, r, cy, pGraphParam);
            }
            else
            {
                break;
            }
        }
    }

    if (pGraphParam->pPen == NULL)
        return 1;
    if (pGraphParam->pPen->style == PS_NULL)
        return 1;
    
    /* Draw the out line of pie of chord */
    if (pEllipseArcData->mode == AM_LUNE)
    {
        linedata.x1 = ptIntersect[0].x;
        linedata.y1 = ptIntersect[0].y;
        linedata.x2 = ptIntersect[1].x;
        linedata.y2 = ptIntersect[1].y;
        DrawThinLine(pDev, pGraphParam, &linedata);

        DrawThinEllipseArc(pDev, pGraphParam, pEllipseArcData);
    }
    else
    {
        linedata.x1 = ptIntersect[0].x;
        linedata.y1 = ptIntersect[0].y;
        linedata.x2 = cx;
        linedata.y2 = cy;
        DrawThinLine(pDev, pGraphParam, &linedata);
        linedata.x1 = ptIntersect[1].x;
        linedata.y1 = ptIntersect[1].y;
        DrawThinLine(pDev, pGraphParam, &linedata);
        DrawThinEllipseArc(pDev, pGraphParam, pEllipseArcData);
    }

    _UnexcludeCursor(pDev);
    return 1;
}

/*********************************************************************\
* Function	   DrawPatThinEllipseArc
* Purpose      Draw thin ellipse arc use pattern pen
* Params	   Same as DrawThinEllipseArc
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawPatThinEllipseArc(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEARCDATA pEllipseArcData)
{
    return DrawThinEllipseArc(pDev, pGraphParam, pEllipseArcData);
//    return -1;
}

/*********************************************************************\
* Function	   DrawWideEllipseArc
* Purpose      Draw ellipse arc use wide pen that width > 1
* Params	   Same as DrawThinEllipseArc
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawWideEllipseArc(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEARCDATA pEllipseArcData)
{
    return DrawThinEllipseArc(pDev, pGraphParam, pEllipseArcData);
//    return -1;
}

/*********************************************************************\
* Function	   DrawEllipseArc
* Purpose      Draw ellipse arc, Pie, Chord with various of methods
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawEllipseArc(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PELLIPSEARCDATA pEllipseArcData)
{
    switch(pEllipseArcData->mode)
    {
    case AM_SECTOR:

        return DrawPieOrChord(pDev, pGraphParam, pEllipseArcData);

    case AM_LUNE:

        return DrawPieOrChord(pDev, pGraphParam, pEllipseArcData);

    case AM_ARC:

        if (pGraphParam->pPen == NULL)
            return -1;
        if (pGraphParam->pPen->style == PS_NULL)
            return -1;
        if (pGraphParam->pPen->width > 1)
        {
            return DrawWideEllipseArc(pDev, pGraphParam, pEllipseArcData);
        }
        else if (pGraphParam->pPen->style != PS_SOLID)
        {
            return DrawPatThinEllipseArc(pDev, pGraphParam, pEllipseArcData);
        }
        else
        {
            return DrawThinEllipseArc(pDev, pGraphParam, pEllipseArcData);
        }
        
    default:

        return -1;
    }
}

#endif //ELLIPSEARC_SUPPORTED


#ifdef ROUNDRECT_SUPPORTED
/*********************************************************************\
* Function	   DrawThinRoundRect
* Purpose      Draw thin round rect use the method of drawing ellipse
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawThinRoundRect(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PROUNDRECT pRoundRectData)
{
    int     x, xNeg, y, yNeg, cx, cy, rx, ry, yLast;
    int32   flagMiddlePoint;
    int32   Rx2, Ry2, twoRx2, twoRy2, twoRx2y, twoRy2x;
    int32   rop;
    RECT    rcClip, rcRealClip;
    RECT    rc;
    int     width, height;
    uint32  pencolor;
    uint8   *pDst;
    uint32  pixel;
    uint8   lmask, rmask;
    int     nScanLeft, nScanRight;
    ROPFUNC ropfunc;
    LINEDATA linedata;

    ASSERT(pRoundRectData != NULL);
    ASSERT(pGraphParam != NULL);
    
    width = pRoundRectData->width;
    height = pRoundRectData->height;
    if ((pRoundRectData->x2 - pRoundRectData->x1) <= (width * 2))
    {
        if ((pRoundRectData->y2 - pRoundRectData->y1) <= (height * 2))
        {
            ELLIPSEDATA ellipsedata;
            ellipsedata.d1 = pRoundRectData->x2 - pRoundRectData->x1;
            ellipsedata.d2 = pRoundRectData->y2 - pRoundRectData->y1;
            ellipsedata.x = (pRoundRectData->x1 + pRoundRectData->x2) / 2;
            ellipsedata.y = (pRoundRectData->y1 + pRoundRectData->y2) / 2;
            return DrawEllipse(pDev, pGraphParam, &ellipsedata);
        }
        cx = (pRoundRectData->x1 + pRoundRectData->x2) / 2;
        cy = pRoundRectData->y2 - height;
        width = (pRoundRectData->x2 - pRoundRectData->x1) / 2;        
    }
    else if ((pRoundRectData->y2 - pRoundRectData->y1) <= (height * 2))
    {
        cx = pRoundRectData->x2 - width;
        cy = (pRoundRectData->y1 + pRoundRectData->y2) / 2;
        height = (pRoundRectData->y2 - pRoundRectData->y1) / 2;
    }
    else
    {
        cx = pRoundRectData->x2 - width;
        cy = pRoundRectData->y2 - height;
    }
    rx  = width - 1;
    ry  = height - 1;

    if (pGraphParam->pPen == NULL)
    {
        ropfunc = GetRopFunc(ROP_DST);
        pencolor = 0;
    }
    else
    {
        pencolor = (pGraphParam->pPen->fgcolor == 0) ? 0 : 0xFFFFFFFFL;
        if (pGraphParam->pPen->style == PS_NULL)
        {
            ropfunc = GetRopFunc(ROP_DST);
        }
        else
        {
            rop = SRop3ToRop2(pGraphParam->rop);
            ropfunc = GetRopFunc(rop);
        }
    }

    rc.x1 = pRoundRectData->x1 + 1;
    rc.x2 = pRoundRectData->x2 - 1;
    /* when height == 0, the pat rc should be different */
    if (height == 0)
    {
        rc.y1 = pRoundRectData->y1 + 1;
        rc.y2 = cy - 1;
    }
    else
    {
        rc.y1 = pRoundRectData->y1 + height;
        rc.y2 = cy;
    }
    rcClip.x1 = pRoundRectData->x1;
    rcClip.y1 = pRoundRectData->y1;
    rcClip.x2 = pRoundRectData->x2;
    rcClip.y2 = pRoundRectData->y2;

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
    y = ry;
    Rx2 = rx * rx;
    Ry2 = ry * ry;
    twoRx2 = Rx2 << 1;
    twoRy2 = Ry2 << 1;
    twoRx2y = twoRx2 * y;
    twoRy2x = 0;
    flagMiddlePoint = Ry2 - Rx2 * ry + (Rx2 >> 2);
    yLast = y;

    /* The negative direction coordinate should offset */
    xNeg = -(cx - pRoundRectData->x1 - width) - 1; 
    yNeg = -y -(cy - pRoundRectData->y1 - height) - 1;

    //切线斜率> -1, X控制
    while (twoRy2x < twoRx2y) 
    {
        if (IsXOutOfRect(rcClip, xNeg, x))
        {
            x ++;
            xNeg --;
            twoRy2x += twoRy2;
            yLast = y;
            if (flagMiddlePoint < 0)
            {
                flagMiddlePoint += twoRy2x + Ry2;
            }
            else
            {
                y --;
                yNeg ++;
                twoRx2y -= twoRx2;
                flagMiddlePoint += twoRy2x - twoRx2y + Ry2;
            }

            continue;
        }

        rmask = 0x80 >> ((x + cx) % 8);
        lmask = 0x80 >> ((xNeg + cx) % 8);
        if (IsPtInYClip(rcClip, y))
        {
            if (IsPtInXClip(rcClip, x))  //RB1
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, xNeg)) //LB1
            {
                pDst = GETXYADDR(pDev,  cx + xNeg , cy + y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                    PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx + xNeg + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + y, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        if (IsPtInYClip(rcClip, yNeg))
        {
            if (IsPtInXClip(rcClip, x))  //RU1
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + yNeg);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, xNeg)) //LU1
            {
                pDst = GETXYADDR(pDev,  cx + xNeg , cy + yNeg);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx + xNeg + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + yNeg, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }
        
        x ++;
        xNeg --;
        twoRy2x += twoRy2;
        yLast = y;
        if (flagMiddlePoint < 0)
        {
            flagMiddlePoint += twoRy2x + Ry2;
        }
        else
        {
            y --;
            yNeg ++;
            twoRx2y -= twoRx2;
            flagMiddlePoint += twoRy2x - twoRx2y + Ry2;
        }
    }

    //绘制第二部分
    flagMiddlePoint = (int32)(Ry2 * (x + 0.5) * (x + 0.5) + 
        Rx2 * (y - 1) * (y - 1) - Rx2 * Ry2);

    //切线斜率< -1, Y控制
    while (y >= 0) 
    {
        if (IsXOutOfRect(rcClip, xNeg, x))
        {
            y --;
            yNeg ++;
            twoRx2y -= twoRx2;
            if (flagMiddlePoint > 0)
            {
                flagMiddlePoint += Rx2 - twoRx2y;
            }
            else
            {
                x ++;
                xNeg --;
                twoRy2x += twoRy2;
                flagMiddlePoint += twoRy2x - twoRx2y + Rx2;
            }

            continue;
        }

        rmask = 0x80 >> ((x + cx) % 8);
        lmask = 0x80 >> ((xNeg + cx) % 8);
        if (IsPtInYClip(rcClip, y))
        {
            if (IsPtInXClip(rcClip, x))  //RB1
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, xNeg)) //LB1
            {
                pDst = GETXYADDR(pDev,  cx + xNeg , cy + y);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx + xNeg + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + y, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        if (IsPtInYClip(rcClip, yNeg))
        {
            if (IsPtInXClip(rcClip, x))  //RU1
            {
                pDst = GETXYADDR(pDev, cx + x,  cy + yNeg);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, rmask);
                nScanRight = cx + x - 1;
            }

            if (IsPtInXClip(rcClip, xNeg)) //LU1
            {
                pDst = GETXYADDR(pDev,  cx + xNeg , cy + yNeg);
                pixel = ropfunc(pencolor, GETPIXEL(pDst));
                PUTPIXEL(pDst, pixel, lmask);
                nScanLeft = cx + xNeg + 1;
            }

            if (y != yLast)
                PatBltHorizonalLine(pDev, nScanLeft, nScanRight, cy + yNeg, 
                pGraphParam);
            nScanLeft = rcRealClip.x1;
            nScanRight = rcRealClip.x2 - 1;
        }

        y --;
        yNeg ++;
        twoRx2y -= twoRx2;
        if (flagMiddlePoint > 0)
        {
            flagMiddlePoint += Rx2 - twoRx2y;
        }
        else
        {
            x ++;
            xNeg --;
            twoRy2x += twoRy2;
            flagMiddlePoint += twoRy2x - twoRx2y + Rx2;
        }
    }//while (y >= 0)

    /* Draw the four out lines */
    if (pGraphParam->pPen != NULL && pGraphParam->pPen->style != PS_NULL)
    {
        linedata.x1 = pRoundRectData->x1 + width;
        linedata.x2 = pRoundRectData->x2 - width;
        linedata.y1 = linedata.y2 = pRoundRectData->y1;
        DrawThinLine(pDev, pGraphParam, &linedata);
        
        linedata.y1 = linedata.y2 = pRoundRectData->y2 - 1;
        DrawThinLine(pDev, pGraphParam, &linedata);
        
        linedata.y1 = pRoundRectData->y1 + height;
        linedata.y2 = pRoundRectData->y2 - height;
        linedata.x1 = linedata.x2 = pRoundRectData->x1;
        DrawThinLine(pDev, pGraphParam, &linedata);
        
        linedata.x1 = linedata.x2 = pRoundRectData->x2 - 1;
        DrawThinLine(pDev, pGraphParam, &linedata);
    }
    

    /* fill the inner rect */
    PatBlt(pDev, &rc, pGraphParam);
    
    _UnexcludeCursor(pDev);
    return 0;
}

/*********************************************************************\
* Function	   DrawWideRoundRect
* Purpose      Draw wide round rect use the method of drawing ellipse
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawWideRoundRect(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PROUNDRECT pRoundRectData)
{
    return DrawThinRoundRect(pDev, pGraphParam, pRoundRectData);
//    return -1;
}

/*********************************************************************\
* Function	   DrawPatRoundRect
* Purpose      Draw pattern round rect use the method of drawing ellipse
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawPatRoundRect(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PROUNDRECT pRoundRectData)
{
    return DrawThinRoundRect(pDev, pGraphParam, pRoundRectData);
//    return -1;
}

/*********************************************************************\
* Function	   GetRoundPoints
* Purpose      get the four corner point of a button 
               with round rect style
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void GetRoundPoints(PPOINT points, int x, int y, 
                           int xflag, int yflag)
{
    points[0].x = x + 3 * (xflag);
    points[1].x = x + 2 * (xflag);
    points[2].x = x + 1 * (xflag);
    points[3].x = x + 1 * (xflag);
    
    points[0].y = y + 1 * (yflag);
    points[1].y = y + 1 * (yflag);
    points[2].y = y + 2 * (yflag);
    points[3].y = y + 3 * (yflag);

    return ; 
}

/*********************************************************************\
* Function	   DrawPolyPoint
* Purpose      draw a seriels of points
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawPolyPoint(PDEV pDev, PGRAPHPARAM pGraphParam, 
                         PPOLYPOINTDATA pDrawData)
{
    uint32 number, pencolor, i;
    uint8  mask;
    uint8* pDst;
    PPOINT  ppt;
    PPHYSPEN    pPen;
    RECT   clip_rc;

    ASSERT(pGraphParam != NULL);
    if (pGraphParam->pPen == NULL)
        return 0;
    if (!(pGraphParam->flags & GPF_PEN))
        return 0;

    pPen = pGraphParam->pPen;
    //For null pen ,return 
    if (pPen->style == PS_NULL)
        return 0;

    pencolor = (pPen->fgcolor == 0) ? 0 : 0xFFFFFFFFL;

    //Get clip rectangle
    GetDevRect(&clip_rc, pDev);   
    if (pGraphParam->flags & GPF_CLIP_RECT)
        ClipRect(&clip_rc, &pGraphParam->clip_rect);
    _ExcludeCursor(pDev, &clip_rc);    

    number = pDrawData->number;

    ppt = pDrawData->pPoint;
    for (i = 0; i < number; i++)
    {
        if (!IsPointInRect(&clip_rc, ppt->x, ppt->y))
            continue;

        pDst = GETXYADDR(pDev, ppt->x, ppt->y);
        mask = 0x80 >> (ppt->x % 8);
        PUTPIXEL(pDst, pencolor, mask);
        ppt++;
    }

    _UnexcludeCursor(pDev);
    
    return 0;
}

/*********************************************************************\
* Function	   DrawRoundButton
* Purpose      When the width and height of roundrect is zero, the 
               caller want to draw a button, then call this function to
               realize. It is faster than the roundrect drawing for no 
               ellipse drawing.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawRoundButton(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PROUNDRECT pRoundRectData)
{
    PPHYSPEN    pPen;
    PPHYSBRUSH  pBrush;
    LINEDATA    linedata;
    RECT        outer_rect, clip_rect, inner_rect;
    RECTDATA    rc;
    int         penwidth = 0;
    int         width, height;
    int         x1, x2, y1, y2;
    POINT       points[4];
    POLYPOINTDATA polypoint;

    ASSERT(pGraphParam != NULL);
    if (pGraphParam->pPen == NULL)
        return 0;
    pPen = pGraphParam->pPen;
    pBrush = pGraphParam->pBrush;

    width = pRoundRectData->x2 - pRoundRectData->x1;
    height = pRoundRectData->y2 - pRoundRectData->y1;
    rc.x1 = pRoundRectData->x1;
    rc.x2 = pRoundRectData->x2;
    rc.y1 = pRoundRectData->y1;
    rc.y2 = pRoundRectData->y2;

    if ((width < 8) || (height < 8))
    {
        int nRet, style = pGraphParam->pBrush->style;
        pGraphParam->pBrush->style = BS_HOLLOW;
        nRet = DrawRect(pDev, pGraphParam, &rc);
        pGraphParam->pBrush->style = style;

        return nRet;
    }

    if ((pPen != NULL) && (pGraphParam->flags & GPF_PEN) )
        penwidth = pPen->width;
    if (penwidth > 1)
        return DrawRect(pDev, pGraphParam, (PRECTDATA)pRoundRectData);

    CopyRect(&outer_rect, &rc);

    // Make rectangle be even rectangle, x1 <= x2, y1 <= y2 
    if (rc.x1 > rc.x2)
    {
        outer_rect.x2 = rc.x1 + 1;
        outer_rect.x1 = rc.x2 + 1;
    }

    if (rc.y1 > rc.y2)
    {
        outer_rect.y2 = rc.y1 + 1;
        outer_rect.y1 = rc.y2 + 1;
    }

    // Get clip rectangle
    GetDevRect(&clip_rect, pDev); 
    if (pGraphParam->flags & GPF_CLIP_RECT)
        ClipRect( &clip_rect, &pGraphParam->clip_rect );

    //计算画圆角所需的各点坐标, 并绘图。
    polypoint.number = 4;
    //左上角
    GetRoundPoints(points, outer_rect.x1, outer_rect.y1, 1, 1);
    polypoint.pPoint = points;
    DrawPolyPoint(pDev, pGraphParam, &polypoint);

    //左下角
    GetRoundPoints(points, outer_rect.x1, (outer_rect.y2 - 1), 1, -1);
    polypoint.pPoint = points;
    DrawPolyPoint(pDev, pGraphParam, &polypoint);

    //右上角
    GetRoundPoints(points, (outer_rect.x2 - 1), outer_rect.y1, -1, 1);
    polypoint.pPoint = points;
    DrawPolyPoint(pDev, pGraphParam, &polypoint);

    //右下角
    GetRoundPoints(points, (outer_rect.x2 - 1), 
        (outer_rect.y2 - 1), -1, -1);
    polypoint.pPoint = points;
    DrawPolyPoint(pDev, pGraphParam, &polypoint);
    
    if ( !IntersectRect(&outer_rect, &clip_rect) )
        return 0;

    //上边
    linedata.x1 = outer_rect.x1 + 4;
    linedata.x2 = outer_rect.x2 - 1 - 3;
    linedata.y1 = outer_rect.y1;
    linedata.y2 = outer_rect.y1;
    DrawThinLine(pDev, pGraphParam, &linedata);

    //下边
    linedata.y2 = outer_rect.y2 - 1;
    linedata.y1 = outer_rect.y2 - 1;
    DrawThinLine(pDev, pGraphParam, &linedata);

    //右边
    linedata.x1 = outer_rect.x2 - 1;
    linedata.x2 = outer_rect.x2 - 1;
    linedata.y1 = outer_rect.y1 + 4;
    linedata.y2 = outer_rect.y2 - 1 - 3;
    DrawThinLine(pDev, pGraphParam, &linedata);


    //左边
    linedata.y1 = outer_rect.y2 - 1 - 4;
    linedata.y2 = outer_rect.y1 + 3;
    linedata.x1 = outer_rect.x1;
    linedata.x2 = outer_rect.x1;
    DrawThinLine(pDev, pGraphParam, &linedata);


    if ((pGraphParam->flags & GPF_BRUSH))
    {
        if (pPen == NULL)
        {
            inner_rect.x1 = outer_rect.x1;
            inner_rect.x2 = outer_rect.x2;
            inner_rect.y1 = outer_rect.y1;
            inner_rect.y2 = outer_rect.y2;
        }
        else
        {
            inner_rect.x1 = outer_rect.x1 + 1;
            inner_rect.x2 = outer_rect.x2 - 1;
            inner_rect.y1 = outer_rect.y1 + 4;
            inner_rect.y2 = outer_rect.y2 - 4;
        }
        if (!IsValidRect(&inner_rect))
            return 0;

        x1 = outer_rect.x1 + 4;
        x2 = outer_rect.x2 - 5;
        y1 = outer_rect.y1 + 1;
        y2 = outer_rect.y2 - 2;
        if (IsPtInYClip(clip_rect, y1))
            PatBltHorizonalLine(pDev, MAX(x1, clip_rect.x1), 
            MIN(x2, clip_rect.x2 - 1), y1, pGraphParam);
        if (IsPtInYClip(clip_rect, y2))
            PatBltHorizonalLine(pDev, MAX(x1, clip_rect.x1),
            MIN(x2, clip_rect.x2 - 1), y2, pGraphParam);
        x1 -= 2;
        x2 += 2;
        y1++;
        y2--;
        if (IsPtInYClip(clip_rect, y1))
            PatBltHorizonalLine(pDev, MAX(x1, clip_rect.x1), 
            MIN(x2, clip_rect.x2 - 1), y1, pGraphParam);
        if (IsPtInYClip(clip_rect, y2))
            PatBltHorizonalLine(pDev, MAX(x1, clip_rect.x1),
            MIN(x2, clip_rect.x2 - 1), y2, pGraphParam);
        
        y1++;
        y2--;
        if (IsPtInYClip(clip_rect, y1))
            PatBltHorizonalLine(pDev, MAX(x1, clip_rect.x1), 
            MIN(x2, clip_rect.x2 - 1), y1, pGraphParam);
        if (IsPtInYClip(clip_rect, y2))
            PatBltHorizonalLine(pDev, MAX(x1, clip_rect.x1),
            MIN(x2, clip_rect.x2 - 1), y2, pGraphParam);
        

        PatBlt(pDev, &inner_rect, pGraphParam);
    }

    return 0;
}

/*********************************************************************\
* Function	   DrawRoundRect
* Purpose      Draw round rect use the method of drawing ellipse
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawRoundRect(PDEV pDev, PGRAPHPARAM pGraphParam,
                        PROUNDRECT pRoundRectData)
{
    if (pRoundRectData == NULL || pDev == NULL || pGraphParam == NULL)
        return 0;
    if ((pRoundRectData->x2 - pRoundRectData->x1 <= 2) ||
        (pRoundRectData->y2 - pRoundRectData->y1 <= 2))
    {
        RECTDATA rcdata;
        rcdata.x1 = pRoundRectData->x1;
        rcdata.y1 = pRoundRectData->y1;
        rcdata.x2 = pRoundRectData->x2;
        rcdata.y2 = pRoundRectData->y2;
        return DrawRect(pDev, pGraphParam, &rcdata);
    }

    if (pRoundRectData->height == 0 && pRoundRectData->width == 0)
    {
        return DrawRoundButton(pDev, pGraphParam, pRoundRectData);
    }
    /* WWH modified on 2002/11/03 
    ** The width or height <= 1. We should draw a rect 
    ** instead of a round rect
    */
    if (pRoundRectData->height <= 1 || pRoundRectData->width <= 1)
    {
        return DrawRect(pDev, pGraphParam, (PRECTDATA)pRoundRectData);
    }
    if (pGraphParam->pPen == NULL)
    {
        return DrawThinRoundRect(pDev, pGraphParam, pRoundRectData);
    }
    if (pGraphParam->pPen->width > 1)
    {
        return DrawWideRoundRect(pDev, pGraphParam, pRoundRectData);
    }
    else if (pGraphParam->pPen->style != PS_SOLID && 
        pGraphParam->pPen->style != PS_NULL)
    {
        return DrawPatRoundRect(pDev, pGraphParam, pRoundRectData);
    }
    else
    {
        return DrawThinRoundRect(pDev, pGraphParam, pRoundRectData);
    }

    return -1;
}

#endif //ROUNDRECT_SUPPORTED
