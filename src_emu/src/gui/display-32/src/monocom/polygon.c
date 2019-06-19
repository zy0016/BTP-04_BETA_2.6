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



#ifdef POLYGON_SUPPORTED
/*********************************************************************\
* Function	   FillPolygon
* Purpose      Fill a polygon use brush.
* Params	   
    pDev        Pointer to the device struct;
    count       The number of points that determine the polygon
    pointtable  The points data
    pGraphParam The draw parameter
* Return	 	   
* Remarks	   Use the scan line algorithm.
**********************************************************************/
static int FillPolygon(PDEV pDev, int count, POINT* pointtable, 
                     PGRAPHPARAM pGraphParam)
{
    PEDGE   Edge, pEdge, pEdgeNode, ActEdge, pActEdge, pActEdgeNode, pPreEdge;
    int     nEdge, nActEdge;
    PPOINT  pPoints;
	int     i, y, l, r;
    RECT cliprc;

    ASSERT(pGraphParam != NULL);
    if (pGraphParam->pBrush == NULL)
        return 0;
    if (pGraphParam->pBrush->style == BS_HOLLOW)
        return 0;

    pPoints = pointtable;
    if (pPoints == NULL || count < 3)
        return 0;
    
    GetDevRect(&cliprc, pDev);
    if (pGraphParam->flags & GPF_CLIP_RECT)
        ClipRect(&cliprc, &pGraphParam->clip_rect);
    if (!IsValidRect(&cliprc))
        return -1;
    
    /* malloc enough memory used to the edge table */
    Edge = malloc(sizeof(EDGE) * count);
    ActEdge = malloc(sizeof(EDGE) * count);
	if ((Edge == NULL) || (ActEdge == NULL))
    {
		/* error, couldn't allocate one or both of the needed tables */
		if (Edge != NULL)
			free(Edge);
		if (ActEdge != NULL)
			free(ActEdge);
		return -1;
	}

    nEdge = nActEdge = 0;
    pEdge = pActEdge = NULL;

    /* Set the pEdge list */
    for (i = 0; i < count; i++)
    {
        Edge[nEdge].x1 = pPoints[i].x;
        Edge[nEdge].y1 = pPoints[i].y;
        Edge[nEdge].x2 = pPoints[(i + 1) % count].x;
        Edge[nEdge].y2 = pPoints[(i + 1) % count].y;

        if (Edge[nEdge].y1 == Edge[nEdge].y2)
            continue;

        else if (Edge[nEdge].y1 > Edge[nEdge].y2)
        {
            EXCHANGE(Edge[nEdge].y1, Edge[nEdge].y2);
            EXCHANGE(Edge[nEdge].x1, Edge[nEdge].x2);
        }

        Edge[nEdge].cx = Edge[nEdge].x1;
        Edge[nEdge].dx = (Edge[nEdge].x2 - Edge[nEdge].x1) * 2;
        Edge[nEdge].dy = (Edge[nEdge].y2 - Edge[nEdge].y1) * 2;

        if (ABS(Edge[nEdge].dx) >= Edge[nEdge].dy)
        {
            Edge[nEdge].ctrl = LINE_CTRLX;
            /* 
            ** It's diffrent to the method to draw line. It use no offset 
            ** at the line start. So some filled point overlaped with the 
            ** outline point. 
            ** At the start of drawing LINE_CTRLX we use a dx/2 as a offset to 
            ** realize round.
            */
            Edge[nEdge].flag = 0;//- Edge[nEdge].dx / 2;
        }
        else
        {
            Edge[nEdge].ctrl = LINE_CTRLY;
            /* At the start of drawing LINE_CTRLY we use a dy/2 as a offset 
            ** to realize round.
            */
            Edge[nEdge].flag = ABS(Edge[nEdge].dx) - Edge[nEdge].dy;// / 2;
        }

        InsertEdge(&pEdge, &Edge[nEdge]);
        nEdge ++;
    }

    if (pEdge == NULL)
        return 0;
    /* start with the lowest y in the table */
	y = pEdge->y1;
    
    pEdgeNode = pEdge;
	do 
    {
		/* add edges to the active edge table from the global table */
		while (pEdgeNode)
        {
            if (pEdgeNode->y1 == y)
            {
                ActEdge[nActEdge] = *pEdgeNode;
                ActEdge[nActEdge].y1 = 0;
                if (InsertEdge(&pActEdge, &ActEdge[nActEdge]))
                {
                    nActEdge ++;
                }
            }
            else if (pEdgeNode->y1 > y)
                break;

            pEdgeNode = pEdgeNode->pNext;
		}

        /* sort by cx */
        SortActiveEdge(&pActEdge);

        pActEdgeNode = pActEdge;
		while (pActEdgeNode)
        {
            ASSERT(pActEdgeNode->pNext);

            /* When filling, the bellow algorithm is used to judge the 
            ** offset with the outline is -1,0 or 1
            */
            if (pActEdgeNode->ctrl == LINE_CTRLY)
            {
                if (pActEdgeNode->dx > 0)
                    l = pActEdgeNode->cx + 1;
                else
                    l = pActEdgeNode->cx;
            }
            else
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
            else
            {
                r = pActEdgeNode->cx - 1;
            }

            pActEdgeNode = pActEdgeNode->pNext;

            if (IsPtInYClip(cliprc, y))
            {
                if (l < cliprc.x1)
                    l = cliprc.x1;


                //if (r > cliprc.x2)
                //    r = cliprc.x2;
                if (r >= cliprc.x2)
                    r = cliprc.x2 - 1;

                if (r >= l)
                    PatBltHorizonalLine(pDev, l, r, y, pGraphParam);
            }
		}

        y++;
        
        /* delete the line that has been ended in the active edge table */
        while (pActEdge->y2 == y)
        {
            pActEdge = pActEdge->pNext;
            if (pActEdge == NULL)
                break;
        }

        pPreEdge = pActEdge;
        pActEdgeNode = pActEdge;
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

        /* update the line data */
        pActEdgeNode = pActEdge;
		while (pActEdgeNode)
		{
            if (pActEdgeNode->ctrl == LINE_CTRLX)
            {
                pActEdgeNode->flag += pActEdgeNode->dx;
				pActEdgeNode->cx += pActEdgeNode->flag / pActEdgeNode->dy;
                pActEdgeNode->flag %= pActEdgeNode->dy;
            }
            else
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

            pActEdgeNode = pActEdgeNode->pNext;
		}
    } while (pActEdge || pEdgeNode);

    free(Edge);
    free(ActEdge);
    return 1;
}

/*********************************************************************\
* Function	  DrawThinPolygon 
* Purpose     Draw thin polygon. Call the line function. 
* Params	   
    pDev            Pointer to display device structure.
    pGraphParam     Pointer to display device parameter.
    pPolyLineData   Pointer to drawing shape data.
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawThinPolygon(PDEV pDev, PGRAPHPARAM pGraphParam, 
                             PPOLYLINEDATA pPolyLineData)
{
    int         nCount, nLineCount;
    int         i;
    int         nDrawPixels;
    LINEDATA    LineData;
    LINEDRAW    LineDraw;
    PENSTYLE    penstyle, penstyle1;
    int         pencount;      
    int         LineType;
    RECT        rc;

    GetDevRect(&rc, pDev);
    if (pGraphParam->flags & GPF_CLIP_RECT)
        ClipRect(&rc, &pGraphParam->clip_rect);
    if (!IsValidRect(&rc))
        return 0;
    
    _ExcludeCursor(pDev, &rc);

    nCount = pPolyLineData->count;
    if (pPolyLineData->mode == 1) // close the polygon
    {
        FillPolygon(pDev, nCount, pPolyLineData->points, pGraphParam);
        nLineCount = nCount;
    }
    else
    {
        nLineCount = nCount - 1;
    }

    if (pGraphParam->pPen->style == PS_SOLID)
    {
        for (i = 0; i < nLineCount; i++)
        {
            LineData.x1 = pPolyLineData->points[i].x;
            LineData.y1 = pPolyLineData->points[i].y;
            LineData.x2 = pPolyLineData->points[(i + 1) % nCount].x;
            LineData.y2 = pPolyLineData->points[(i + 1) % nCount].y;
            DrawThinLine(pDev, pGraphParam, &LineData);
        }
    }
    else
    {
        nDrawPixels = 0;
        penstyle1 = GetPenStyle(pGraphParam->pPen->style);
        pencount = penstyle1.count;

        for (i = 0; i < nLineCount; i++)
        {
            nDrawPixels %= pencount;// &= 0x0f;
            penstyle1.pattern = 
                GetOffsetPenStyle(penstyle1.pattern, pencount, nDrawPixels);
            penstyle = penstyle1;
            
            LineData.x1 = pPolyLineData->points[i].x;
            LineData.y1 = pPolyLineData->points[i].y;
            LineData.x2 = pPolyLineData->points[(i + 1) % nCount].x;
            LineData.y2 = pPolyLineData->points[(i + 1) % nCount].y;
            
            LineType = ClipLine(&LineData, &rc, &LineDraw);

            switch (LineType) 
            {
            case LINE_HORIZONTAL:

                nDrawPixels = LineData.x2 - LineData.x1 + 1;

                if (LineDraw.cliped != 0)
                {
                    penstyle.pattern = GetOffsetPenStyle(penstyle.pattern, 
                        penstyle.count, LineDraw.cliped % penstyle.count);
                }

                DrawHLine(pDev, LineDraw.start_x, LineDraw.end_x, 
                    LineDraw.start_y, pGraphParam, penstyle);

                break;

            case LINE_VERTICAL:

                nDrawPixels = LineData.y2 - LineData.y1 + 1;

                if (LineDraw.cliped != 0)
                {
                    penstyle.pattern = GetOffsetPenStyle(penstyle.pattern, 
                        penstyle.count, LineDraw.cliped % penstyle.count);
                }

                DrawVLine(pDev, LineDraw.start_y, LineDraw.end_y,
                    LineDraw.start_x, pGraphParam, penstyle);

                break;
            case LINE_CTRLX:
                nDrawPixels = ABS(LineData.x2 - LineData.x1) + 1;

                if (LineDraw.cliped != 0)
                {
                    penstyle.pattern = GetOffsetPenStyle(penstyle.pattern, 
                        penstyle.count, LineDraw.cliped % penstyle.count);
                }

                DrawXLine(pDev, &LineDraw, pGraphParam, penstyle);
                break;
            case LINE_CTRLY:
                nDrawPixels = ABS(LineData.y2 - LineData.y1) + 1;

                if (LineDraw.cliped != 0)
                {
                    penstyle.pattern = GetOffsetPenStyle(penstyle.pattern, 
                        penstyle.count, LineDraw.cliped % penstyle.count);
                }

                DrawYLine(pDev, &LineDraw, pGraphParam, penstyle);
                break;
            default:
                break;
            }    
        }
    }

    _UnexcludeCursor(pDev);
    return 0;
}

/*********************************************************************\
* Function	  DrawWidePolygon 
* Purpose     Draw wide polygon. Haven't been realized now. 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawWidePolygon(PDEV pDev, PGRAPHPARAM pGraphParam, 
                             PPOLYLINEDATA pPolyLineData)
{
    return DrawThinPolygon(pDev, pGraphParam, pPolyLineData);
//    return 0;
}

/*********************************************************************\
* Function	 DrawPolygon  
* Purpose    DrawPolygon with various of methods.
             Only support the polygon with width=1 now.  
* Params	   
    pDev            Pointer to display device structure.
    pGraphParam     Pointer to display device parameter.
    pPolyLineData   Pointer to drawing shape data.
* Return	 	   
* Remarks	   
**********************************************************************/
static int DrawPolygon(PDEV pDev, PGRAPHPARAM pGraphParam, 
                             PPOLYLINEDATA pPolyLineData)
{
    if (pGraphParam->pPen != NULL && pGraphParam->pPen->width > 1)
    {
        return DrawWidePolygon(pDev, pGraphParam, pPolyLineData);
    }
    else
    {
        return DrawThinPolygon(pDev, pGraphParam, pPolyLineData);
    }
}

#endif //POLYGON_SUPPORTED
