/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display Driver 
 *
 * Purpose  : Implement the functions about rgb device.
 *            
\**************************************************************************/

/*********************************************************************\
* Function  UpdateScreen
* Purpose   Transfer contents in VRAM to screen.
* Params
    pDev    Pointer to display device structure.
* Return    0
* Remarks	
**********************************************************************/
static int UpdateScreen(PDEV pDev)
{
    if (pDev->rcUpdate.x1 >= pDev->rcUpdate.x2 &&
        pDev->rcUpdate.y1 >= pDev->rcUpdate.y2)
        return 0;

    PDD_UpdateScreen(pDev->device_no, pDev->rcUpdate.x1, pDev->rcUpdate.y1, 
        pDev->rcUpdate.x2, pDev->rcUpdate.y2);

    pDev->rcUpdate.x1 = pDev->rcUpdate.y1 = 10000;
    pDev->rcUpdate.x2 = pDev->rcUpdate.y2 = 0;

    return 0;
}

static int RealizeColorToRGB(PDEV pDev, uint8* pscolor, uint32 *pdcolor, 
                             int32 pixellen)
{
    if (pDev == NULL || pscolor == NULL || pdcolor == NULL)
        return 0;
    if (pixellen <= 0)
        return 0;
    if ((uint32)pscolor == (uint32)pdcolor)
        return 0;

    switch (pDev->bmpmode)
    {
    case BMP_RGB32:
        while (pixellen --)
        {
            *pdcolor++ = REALIZECOLOR32TO24((DWORD)GET32_DW(pscolor));
            pscolor += 4;
        }

        break;
    case BMP_RGB24:
        while (pixellen--)
        {
            *pdcolor++ = REALIZECOLOR24TO24((DWORD)GET24_BGR(pscolor));
            pscolor += 3;
        }

        break;
    case BMP_RGB16:
        while (pixellen--)
        {
            *pdcolor++ = REALIZECOLOR16TO24((DWORD)GET16_W(pscolor));
            pscolor += 2;
        }

        break;
    case BMP_RGB12:
        while (pixellen--)
        {
            *pdcolor++ = REALIZECOLOR12TO24((DWORD)GET16_W(pscolor));
            pscolor += 2;
        }

        break;
    default:
        ASSERT(0);
        break;
    }

    return 0;
}

/*********************************************************************\
* Function  DrawGraph
* Purpose   Draw various of shapes with various functions.
* Params
    pDev        Pointer to display device structure.
    func        the flag to define the drawing shape.
    pGraphParam Pointer to display device parameter.
    pDrawData   Pointer to drawing shape data.
* Return    
    Success:    0
    Failed:     -1
* Remarks	
**********************************************************************/
static int DrawGraph(PDEV pDev, int func, PGRAPHPARAM pGraphParam, 
                     void* pDrawData)
{
    switch (func)
    {
    case DG_GETPIXEL :
        
        //defined in line.c
        return GetPixel(pDev, pGraphParam, (PPOINTDATA)pDrawData);

    case DG_SETPIXEL:
        //defined in line.c
        return SetPixel(pDev, pGraphParam, (PPOINTDATA)pDrawData);
        
    case DG_POINT :

        //defined in line.c
        return DrawPoint(pDev, pGraphParam, (PPOINTDATA)pDrawData);
        
    case DG_LINE :

        //defined in line.c
        return DrawLine(pDev, pGraphParam, (PLINEDATA)pDrawData);
        
    case DG_RECT :

        //defined in rect.c
        return DrawRect(pDev, pGraphParam, (PRECTDATA)pDrawData);

    case DG_CIRCLE :

#ifdef CIRCLE_SUPPORTED
        //defined in circle.c
        return DrawCircle(pDev, pGraphParam, (PCIRCLEDATA)pDrawData);
#elif defined(ELLIPSE_SUPPORTED)
        {
            ELLIPSEDATA edata;

            edata.x = ((PCIRCLEDATA)pDrawData)->x;
            edata.y = ((PCIRCLEDATA)pDrawData)->y;
            edata.d1 = ((PCIRCLEDATA)pDrawData)->r * 2;
            edata.d2 = ((PCIRCLEDATA)pDrawData)->r * 2;
            edata.rotate = 0;

            return DrawEllipse(pDev, pGraphParam, &edata);
        }
#else
        return -1;
#endif

    case DG_POLYGON:

#ifdef POLYGON_SUPPORTED
        //defined in polygon.c
        return DrawPolygon(pDev, pGraphParam, (PPOLYLINEDATA)pDrawData);
#else
        return -1;
#endif

    case DG_ELLIPSE : 

#ifdef ELLIPSE_SUPPORTED
        //defined in ellipse.c
        return DrawEllipse(pDev, pGraphParam, (PELLIPSEDATA)pDrawData);
#else
        return -1;
#endif

    case DG_ARC :

#ifdef ARC_SUPPORTED
        
        return 1;
        //defined in ellipse.c
#else
        return -1;
#endif

    case DG_ELLIPSEARC :

#ifdef ELLIPSEARC_SUPPORTED
        //defined in ellipse.c
        return DrawEllipseArc(pDev, pGraphParam, 
            (PELLIPSEARCDATA)pDrawData);
#else
        return -1;
#endif

    case DG_ROUNDRECT :

#ifdef ROUNDRECT_SUPPORTED
        //defined in rect.c
//        return DrawRoundRect(pDev, pGraphParam, (PRECTDATA)pDrawData);
        return DrawRoundRect(pDev, pGraphParam, (PROUNDRECT)pDrawData);
#else
        return -1;
#endif

    case DG_POLYPOINT :

#ifdef POLYPOINT_SUPPORTED
        //defined in line.c
        return DrawPolyPoint(pDev, pGraphParam, (PPOLYPOINTDATA)pDrawData);
#else
        return -1;
#endif

    case DG_POLYPOLYLINE :

#ifdef POLYPOLYLINE_SUPPORTED
        //defined in polygon.c
        return DrawPolypolyLine(pDev, pGraphParam, 
            (PPOLYPOLYLINEDATA)pDrawData);
#else
        return -1;
#endif

    default:

        break;    
    }

    return -1;
}
