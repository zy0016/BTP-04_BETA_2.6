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



/*************************************************************************/
/*  DDI_line 为手写识别和速记单独实现的函数
/*************************************************************************/

// #define HW_DOUBLE_LINE
#if 0
int DDD_Line(int x1, int y1, int x2, int y2, int width)
{
    PDEV pDev = pDisplayDev;

    int dx, dy;
    int x, y, t;
    uint8* pDst;
    uint8* p;
    int pix_bytes, line_bytes;
    RECT rc;

    SetRect(&rc, x1 - 1, y1 - 1, x2 + 1, y2 + 1);
    NormalizeRect(&rc);
    _ExcludeCursor(pDev, &rc);

    pix_bytes = pDev->pix_bits / 8;
    line_bytes = pDev->line_bytes;

    if (y1 == y2)
    {
        if (x1 == x2)
            return 0;

        if (x1 > x2)
        {
            t = x1;
            x1 = x2 + 1;
            x2 = t;
        }
        else
            x2--;

        pDst = GETXYADDR(pDev, x1, y1);
        
        p = pDst;
        for (x = x1; x <= x2; x++)
        {
            PUTPIXEL(p, HW_PIXEL);
            p += pix_bytes;
        }

#ifdef HW_DOUBLE_LINE
        if (y1 < pDev->height - 1)
        {
            p = pDst + pDev->line_bytes;
            for (x = x1; x <= x2; x++)
            {
                PUTPIXEL(p, HW_PIXEL);
                p += pix_bytes;
            }
        }
#endif  // HW_DOUBLE_LINE
    }
    else if (x1 == x2)
    {
        if (y1 > y2)
        {
            t = y1;
            y1 = y2 + 1;
            y2 = t;
        }
        else
            y2--;

        pDst = GETXYADDR(pDev, x1, y1);
        
        p = pDst;
        for (y = y1; y <= y2; y++)
        {
            PUTPIXEL(p, HW_PIXEL);
            p += line_bytes;
        }

#ifdef HW_DOUBLE_LINE
        if (x1 < pDev->width - 1)
        {
            p = pDst + pix_bytes;
            for (y = y1; y <= y2; y++)
            {
                PUTPIXEL(p, HW_PIXEL);
                p += line_bytes;
            }
        }
#endif // HW_DOUBLE_LINE
    }
    else
    {
        int p;
        int twoDx, twoDy, twoDxDy, twoDyDx;

        dx = ABS(x2 - x1);
        dy = ABS(y2 - y1);

        if (dx >= dy)  //CtrlX type
        {
            if (x2 < x1) //exchange coordinate
            {
                t = x1, x1 = x2, x2 = t;
                t = y1, y1 = y2, y2 = t;
            }

            twoDy = dy * 2;
            p = twoDy - dx;
            twoDyDx = p - dx;
            
            pDst = GETXYADDR(pDev, x1, y1);
      
#ifdef HW_DOUBLE_LINE            
            y = y1;
#endif
            
            if (y2 > y1)
            {
                for (x = x1; x <= x2; x++)
                {
                    PUTPIXEL(pDst, HW_PIXEL);

#ifdef HW_DOUBLE_LINE
                    if (y < pDev->height - 1)
                        PUTPIXEL(pDst + line_bytes, HW_PIXEL);
#endif
                    
                    if (p < 0)
                        p += twoDy;
                    else
                    {
                        p += twoDyDx;
                        pDst += line_bytes;

#ifdef HW_DOUBLE_LINE
                        y++;
#endif
                    }
                    
                    pDst += pix_bytes;
                }
            }
            else
            {
                for (x = x1; x <= x2; x++)
                {
                    PUTPIXEL(pDst, HW_PIXEL);

#ifdef HW_DOUBLE_LINE
                    if (y < pDev->height - 1)
                        PUTPIXEL(pDst + line_bytes, HW_PIXEL);
#endif
                    
                    if (p < 0)
                        p += twoDy;
                    else
                    {
                        p += twoDyDx;               
                        pDst -= line_bytes;

#ifdef HW_DOUBLE_LINE
                        y--;
#endif
                    }
                    
                    pDst += pix_bytes;
                }
            }
        }
        else  // CtrlY type
        {
            if (y2 < y1) //exchange coordinate
            {
                t = x1, x1 = x2, x2 = t;
                t = y1, y1 = y2, y2 = t;
            }

            twoDx = dx + dx;
            p = twoDx - dy;
            twoDxDy = p - dy;
            
            pDst = GETXYADDR(pDev, x1, y1);

#ifdef HW_DOUBLE_LINE
            x = x1;
#endif
            
            if (x2 > x1)
            {
                for (y = y1; y <= y2; y++)
                {
                    PUTPIXEL(pDst, HW_PIXEL);

#ifdef HW_DOUBLE_LINE
                    if (x < pDev->width - 1)
                        PUTPIXEL(pDst + pix_bytes, HW_PIXEL);
#endif

                    if (p < 0)
                        p += twoDx;
                    else
                    {
                        p += twoDxDy;
                        pDst += pix_bytes;

#ifdef HW_DOUBLE_LINE
                        x++;
#endif
                    }
                    
                    pDst += line_bytes;
                }
            }
            else
            {
                for (y = y1; y <= y2; y++)
                {
                    PUTPIXEL(pDst, HW_PIXEL);

#ifdef HW_DOUBLE_LINE
                    if (x < pDev->width - 1)
                        PUTPIXEL(pDst + pix_bytes, HW_PIXEL);
#endif
                    
                    if (p < 0)
                        p += twoDx;
                    else
                    {
                        p += twoDxDy;
                        pDst -= pix_bytes;

#ifdef HW_DOUBLE_LINE
                        x--;
#endif
                    }
                    
                    pDst += line_bytes;
                }
            }
        }
    }

    _UnexcludeCursor(pDev);
    return 1;
}

#else
#define JustPoint(x, min, max) (((x) < (min)) ? (min) : (((x) > (max)) ? (max) : (x)))

static RECT DDD_ClipRect = {0, 0, 0, 0};
BOOL DDD_SetClipRect(PRECT prc)
{
    if (prc == NULL || !IsValidRect(prc))
        return FALSE;

    CopyRect(&DDD_ClipRect, prc);
    NormalizeRect(&DDD_ClipRect);
    return TRUE;
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

static uint32 DDD_Color=0;
uint32 DDD_SetColor(uint32 color)
{
    uint32 oldcolor;

    oldcolor = DDD_Color;
    DDD_Color = RealizeColor(pDisplayDev, NULL, color);
    return oldcolor;
}

#define HW_PIXEL    DDD_Color

int DDD_Line_1(int x1, int y1, int x2, int y2)
{
    PDEV pDev = pDisplayDev;

    int dx, dy;
    int x, y;
    uint8* pDst;
    uint8 *p1;
    int pix_bytes, line_bytes;
    RECT rc;

    /* Just the point in the rect(0, 0, width-1 , height-1) */
    x1 = JustPoint(x1, DDD_ClipRect.x1, DDD_ClipRect.x2 - 1);
    y1 = JustPoint(y1, DDD_ClipRect.y1, DDD_ClipRect.y2 - 1);
    x2 = JustPoint(x2, DDD_ClipRect.x1, DDD_ClipRect.x2 - 1);
    y2 = JustPoint(y2, DDD_ClipRect.y1, DDD_ClipRect.y2 - 1);

    DDD_SetRect(&rc, x1, y1, x2, y2);

    _ExcludeCursor(pDev, &rc);

    pix_bytes = pDev->pix_bits / 8;
    line_bytes = pDev->line_bytes;

    if (y1 == y2)
    {
        if (x1 == x2)
            return 0;

        if (x1 > x2)
        {
            /* In DDD_Line, the performation is the key problem. So the 
            ** redrawing of some point decrease the complication of the 
            ** line algorith.
            */
//            t = x1; x1 = x2 + 1; x2 = t;
            EXCHANGE(x1, x2);
        }
        else
            x2--;

        pDst = GETXYADDR(pDev, x1, y1);
        
        p1 = pDst;
        for (x = x1; x <= x2; x++)
        {
            PUTPIXEL(p1, HW_PIXEL);
            p1 += pix_bytes;
        }
    }
    else if (x1 == x2)
    {
        if (y1 > y2)
        {
            EXCHANGE(y1, y2);
        }
        else
            y2--;

        pDst = GETXYADDR(pDev, x1, y1);
        
        p1 = pDst;
        for (y = y1; y <= y2; y++)
        {
            PUTPIXEL(p1, HW_PIXEL);
            p1 += line_bytes;
        }
    }
    else
    {
        int p;
        int twoDx, twoDy, twoDxDy, twoDyDx;

        dx = ABS(x2 - x1);
        dy = ABS(y2 - y1);

        if (dx >= dy)  //CtrlX type
        {
            if (x1 > x2) //exchange coordinate
            {
                EXCHANGE(x1, x2);
                EXCHANGE(y1, y2);
            }

            twoDy = dy * 2;
            p = twoDy - dx;
            twoDyDx = p - dx;
            
            pDst = GETXYADDR(pDev, x1, y1);
            p1 = pDst;

            if (y1 > y2)
                line_bytes = -line_bytes;

            for (x = x1; x <= x2; x++)
            {
                PUTPIXEL(p1, HW_PIXEL);

                if (p < 0)
                    p += twoDy;
                else
                {
                    p += twoDyDx;
                    p1 += line_bytes;
                }
                
                p1 += pix_bytes;
            }
        }
        else  // CtrlY type
        {
            if (y1 > y2) //exchange coordinate
            {
                EXCHANGE(x1, x2);
                EXCHANGE(y1, y2);
            }

            twoDx = dx + dx;
            p = twoDx - dy;
            twoDxDy = p - dy;
            
            pDst = GETXYADDR(pDev, x1, y1);
            p1 = pDst;

            if (x1 > x2)
                pix_bytes = -pix_bytes;

            for (y = y1; y <= y2; y++)
            {
                PUTPIXEL(p1, HW_PIXEL);

                if (p < 0)
                    p += twoDx;
                else
                {
                    p += twoDxDy;
                    p1 += pix_bytes;
                }
                
                p1 += line_bytes;
            }
        }
    }

    _UnexcludeCursor(pDev);
    return 1;
}

int DDD_Line_2(int x1, int y1, int x2, int y2)
{
    PDEV pDev = pDisplayDev;

    int dx, dy;
    int x, y;
    uint8* pDst;
    uint8 *p1, *p2;
    int pix_bytes, line_bytes;
    RECT rc;

    /* The second point is always at the left or top of the first point */
    /* Just the point in the rect(1, 1, width-1 , height-1) */
    x1 = JustPoint(x1, DDD_ClipRect.x1 + 1, DDD_ClipRect.x2 - 1);
    y1 = JustPoint(y1, DDD_ClipRect.y1 + 1, DDD_ClipRect.y2 - 1);
    x2 = JustPoint(x2, DDD_ClipRect.x1 + 1, DDD_ClipRect.x2 - 1);
    y2 = JustPoint(y2, DDD_ClipRect.y1 + 1, DDD_ClipRect.y2 - 1);

    DDD_SetRect(&rc, x1 - 1, y1 - 1, x2, y2);
    _ExcludeCursor(pDev, &rc);

    pix_bytes = pDev->pix_bits / 8;
    line_bytes = pDev->line_bytes;

    if (y1 == y2)
    {
        if (x1 == x2)
            return 0;

        if (x1 > x2)
        {
            EXCHANGE(x1, x2);
        }
        else
            x2--;

        pDst = GETXYADDR(pDev, x1, y1);
        
        p1 = pDst;
        p2 = p1 - line_bytes;
        for (x = x1; x <= x2; x++)
        {
            PUTPIXEL(p1, HW_PIXEL);
            p1 += pix_bytes;
            PUTPIXEL(p2, HW_PIXEL);
            p2 += pix_bytes;
        }
    }
    else if (x1 == x2)
    {
        if (y1 > y2)
        {
            EXCHANGE(y1, y2);
        }
        else
            y2--;

        pDst = GETXYADDR(pDev, x1, y1);
        
        p1 = pDst;
        p2 = p1 - pix_bytes;
        for (y = y1; y <= y2; y++)
        {
            PUTPIXEL(p1, HW_PIXEL);
            p1 += line_bytes;
            PUTPIXEL(p2, HW_PIXEL);
            p2 += line_bytes;
        }
    }
    else
    {
        int p;
        int twoDx, twoDy, twoDxDy, twoDyDx;

        dx = ABS(x2 - x1);
        dy = ABS(y2 - y1);

        if (dx >= dy)  //CtrlX type
        {
            if (x1 > x2) //exchange coordinate
            {
                EXCHANGE(x1, x2);
                EXCHANGE(y1, y2);
            }

            twoDy = dy * 2;
            p = twoDy - dx;
            twoDyDx = p - dx;
            
            pDst = GETXYADDR(pDev, x1, y1);
            p1 = pDst;
            p2 = p1 - line_bytes;
            
            if (y1 > y2)
                line_bytes = -line_bytes;

            for (x = x1; x <= x2; x++)
            {
                PUTPIXEL(p1, HW_PIXEL);
                PUTPIXEL(p2, HW_PIXEL);

                if (p < 0)
                    p += twoDy;
                else
                {
                    p += twoDyDx;
                    p1 += line_bytes;
                    p2 += line_bytes;
                }
                
                p1 += pix_bytes;
                p2 += pix_bytes;
            }
        }
        else  // CtrlY type
        {
            if (y1 > y2) //exchange coordinate
            {
                EXCHANGE(x1, x2);
                EXCHANGE(y1, y2);
            }

            twoDx = dx + dx;
            p = twoDx - dy;
            twoDxDy = p - dy;
            
            pDst = GETXYADDR(pDev, x1, y1);
            p1 = pDst;
            p2 = p1 - pix_bytes;

            if (x1 > x2)
                pix_bytes = -pix_bytes;

            for (y = y1; y <= y2; y++)
            {
                PUTPIXEL(p1, HW_PIXEL);
                PUTPIXEL(p2, HW_PIXEL);

                if (p < 0)
                    p += twoDx;
                else
                {
                    p += twoDxDy;
                    p1 += pix_bytes;
                    p2 += pix_bytes;
                }
                
                p1 += line_bytes;
                p2 += line_bytes;
            }
        }
    }

    _UnexcludeCursor(pDev);
    return 1;
}

int DDD_Line_3(int x1, int y1, int x2, int y2)
{
    PDEV pDev = pDisplayDev;

    int dx, dy;
    int x, y;
    uint8* pDst;
    uint8 *p1, *p2, *p3;
    int pix_bytes, line_bytes;
    RECT rc;

    /* The second point is always at the left or top of the first point 
    ** The third point is always at the right or bottom of the first point
    */
    /* Just the point in the rect(1, 1, width-2 , height-2) */
    x1 = JustPoint(x1, DDD_ClipRect.x1 + 1, DDD_ClipRect.x2 - 2);
    y1 = JustPoint(y1, DDD_ClipRect.y1 + 1, DDD_ClipRect.y2 - 2);
    x2 = JustPoint(x2, DDD_ClipRect.x1 + 1, DDD_ClipRect.x2 - 2);
    y2 = JustPoint(y2, DDD_ClipRect.y1 + 1, DDD_ClipRect.y2 - 2);

    DDD_SetRect(&rc, x1 - 1, y1 - 1, x2 + 1, y2 + 1);
    _ExcludeCursor(pDev, &rc);

    pix_bytes = pDev->pix_bits / 8;
    line_bytes = pDev->line_bytes;

    if (y1 == y2)
    {
        if (x1 == x2)
            return 0;

        if (x1 > x2)
        {
            EXCHANGE(x1, x2);
        }
        else
            x2--;

        pDst = GETXYADDR(pDev, x1, y1);
        
        p1 = pDst;
        p2 = p1 - line_bytes;
        p3 = p1 + line_bytes;

        /* The loop is from x1-1 to x2+1. In the range of 0-max.
        ** The first point and the last point should be drawn twice
        */
        x1--;
        x2++;
        for (x = x1; x <= x2; x++)
        {
            PUTPIXEL(p1, HW_PIXEL);
            p1 += pix_bytes;
            PUTPIXEL(p2, HW_PIXEL);
            p2 += pix_bytes;
            PUTPIXEL(p3, HW_PIXEL);
            p3 += pix_bytes;
        }
    }
    else if (x1 == x2)
    {
        if (y1 > y2)
        {
            EXCHANGE(y1, y2);
        }
        else
            y2--;

        pDst = GETXYADDR(pDev, x1, y1);
        
        p1 = pDst;
        p2 = p1 - pix_bytes;
        p3 = p1 + pix_bytes;
        y1--;
        y2++;
        for (y = y1; y <= y2; y++)
        {
            PUTPIXEL(p1, HW_PIXEL);
            p1 += line_bytes;
            PUTPIXEL(p2, HW_PIXEL);
            p2 += line_bytes;
            PUTPIXEL(p3, HW_PIXEL);
            p3 += line_bytes;
        }
    }
    else
    {
        int p;
        int twoDx, twoDy, twoDxDy, twoDyDx;

        dx = ABS(x2 - x1);
        dy = ABS(y2 - y1);

        if (dx >= dy)  //CtrlX type
        {
            if (x1 > x2) //exchange coordinate
            {
                EXCHANGE(x1, x2);
                EXCHANGE(y1, y2);
            }

            twoDy = dy * 2;
            p = twoDy - dx;
            twoDyDx = p - dx;
            
            pDst = GETXYADDR(pDev, x1, y1);
            p1 = pDst;
            p2 = p1 - line_bytes;
            p3 = p1 + line_bytes;
            
            if (y1 > y2)
                line_bytes = -line_bytes;

            /* Draw the first point */
            PUTPIXEL(p1 - pix_bytes, HW_PIXEL);
            PUTPIXEL(p2 - pix_bytes, HW_PIXEL);
            PUTPIXEL(p3 - pix_bytes, HW_PIXEL);
            for (x = x1; x <= x2; x++)
            {
                PUTPIXEL(p1, HW_PIXEL);
                PUTPIXEL(p2, HW_PIXEL);
                PUTPIXEL(p3, HW_PIXEL);

                if (p < 0)
                    p += twoDy;
                else
                {
                    p += twoDyDx;
                    p1 += line_bytes;
                    p2 += line_bytes;
                    p3 += line_bytes;
                }
                
                p1 += pix_bytes;
                p2 += pix_bytes;
                p3 += pix_bytes;
            }

            /* Draw the last point */
            PUTPIXEL(p1, HW_PIXEL);
            PUTPIXEL(p2, HW_PIXEL);
            PUTPIXEL(p3, HW_PIXEL);

        }
        else  // CtrlY type
        {
            if (y1 > y2) //exchange coordinate
            {
                EXCHANGE(x1, x2);
                EXCHANGE(y1, y2);
            }

            twoDx = dx + dx;
            p = twoDx - dy;
            twoDxDy = p - dy;
            
            pDst = GETXYADDR(pDev, x1, y1);
            p1 = pDst;
            p2 = p1 - pix_bytes;
            p3 = p1 + pix_bytes;

            if (x1 > x2)
                pix_bytes = -pix_bytes;

            /* Draw the first point */
            PUTPIXEL(p1 - line_bytes, HW_PIXEL);
            PUTPIXEL(p2 - line_bytes, HW_PIXEL);
            PUTPIXEL(p3 - line_bytes, HW_PIXEL);

            for (y = y1; y <= y2; y++)
            {
                PUTPIXEL(p1, HW_PIXEL);
                PUTPIXEL(p2, HW_PIXEL);
                PUTPIXEL(p3, HW_PIXEL);

                if (p < 0)
                    p += twoDx;
                else
                {
                    p += twoDxDy;
                    p1 += pix_bytes;
                    p2 += pix_bytes;
                    p3 += pix_bytes;
                }
                
                p1 += line_bytes;
                p2 += line_bytes;
                p3 += line_bytes;
            }

            /* Draw the last point */
            PUTPIXEL(p1, HW_PIXEL);
            PUTPIXEL(p2, HW_PIXEL);
            PUTPIXEL(p3, HW_PIXEL);

        }
    }

    _UnexcludeCursor(pDev);
    return 1;
}

int DDD_Line(int x1, int y1, int x2, int y2, int width)
{
    int nRet = 0;
    static BOOL bInit = FALSE;

    if (!bInit)
    {
        if (!IsValidRect(&DDD_ClipRect))
            SetRect(&DDD_ClipRect, 0, 0, pDisplayDev->width, pDisplayDev->height);

        bInit = TRUE;
    }

    if (width < 0)
        width = 0;

    switch(width)
    {
    case 1:
        nRet = DDD_Line_1(x1, y1, x2, y2);
        break;
    case 0:
    case 2:
        nRet = DDD_Line_2(x1, y1, x2, y2);
        break;
    case 3:
    default:
        nRet = DDD_Line_3(x1, y1, x2, y2);
        break;
    }

    return nRet;
}

#endif
