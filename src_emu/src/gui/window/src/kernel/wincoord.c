/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements window coordinate mapping functions.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "wsobj.h"
#include "wsownd.h"

/*
**  Function : WND_FromPoint
**  Purpose  :
**      Gets the window which a specified point in.
*/
PWINOBJ WND_FromPoint(int x, int y)
{
    PWINOBJ pWin, pChild;

    pWin = g_pRootWin;

    do 
    {
        if ((pChild = pWin->pChild) == NULL)
            break;

        if (PtInRectXY(&pWin->rcClient, x, y))
        {
            while (pChild)
            {
                // 该点在pChild窗口之内
                if (ISMAPPED(pChild) && PtInRectXY(&pChild->rcWindow, x, y))
                {
                    pWin = pChild;

                    // Child != NULL, 必然导致再次从该窗口的子窗口开始查找
                    break;
                }

                pChild = pChild->pNext;
            }
        }
        else if (PtInRectXY(&pWin->rcWindow, x, y))
        {
            // x, y位于窗口的非客户区内，只需查找窗口的非客户区子窗口
            while (pChild && ISNCCHILD(pChild))
            {
                // 该点在pChild窗口之内
                if (ISMAPPED(pChild) && PtInRectXY(&pChild->rcWindow, x, y))
                {
                    pWin = pChild;

                    // pChild != NULL, 必然导致再次从该窗口的子窗口开始查找
                    break;
                }

                pChild = pChild->pNext;
            }

            // pChild = NULL表示不在任何子窗口中
            pChild = NULL;
        }
        else
        {
            //坐标超出了屏幕，错误仿真环境中ASSERT，真实中应退出，否则死循环
            //可能是笔校准
//            ASSERT(0);
            pChild = NULL;
        }

        // 以上的循环自然结束时，表示所有的子窗口都不包含该点

    } while (pChild != NULL);

    return pWin;
}

/*
**  Function : WND_PointInClient
**  Purpose  :
**      Retrieves whether a specified point is in window client area.
*/
BOOL WND_PointInClient(PWINOBJ pWin, int x, int y)
{
    RECT rcClient;

    ASSERT(pWin != NULL);

    WND_GetRect(pWin, &rcClient, W_CLIENT, XY_SCREEN);

    return PtInRectXY(&rcClient, x, y);
}

/*
**  Function : WND_WindowToScreen
**  Purpose  :
**      Converts window coordinate to screen coordinate.
*/
void WND_WindowToScreen(PWINOBJ pWin, int* pX, int* pY)
{
    RECT rect;

    ASSERT(pWin != NULL);
    ASSERT(pX != NULL && pY != NULL);

    WND_GetRect(pWin, &rect, W_WINDOW, XY_SCREEN);

    *pX += rect.left;
    *pY += rect.top;
}

void WND_NClientToScreen(PWINOBJ pWin, int* pX, int* pY)
{
    RECT rect;

    ASSERT(pWin != NULL);
    ASSERT(pX != NULL && pY != NULL);

    WND_GetRect(pWin, &rect, W_TOPNC, XY_SCREEN);

    *pX += rect.left;
    *pY += rect.top;
}

void WND_ClientToScreen(PWINOBJ pWin, int* pX, int* pY)
{
    RECT rect;

    ASSERT(pWin != NULL);
    ASSERT(pX != NULL && pY != NULL);

    WND_GetRect(pWin, &rect, W_CLIENT, XY_SCREEN);

    *pX += rect.left;
    *pY += rect.top;
}

void WND_ScreenToWindow(PWINOBJ pWin, int* pX, int* pY)
{
    RECT rect;

    ASSERT(pWin != NULL);
    ASSERT(pX != NULL && pY != NULL);

    WND_GetRect(pWin, &rect, W_WINDOW, XY_SCREEN);

    *pX -= rect.left;
    *pY -= rect.top;
}

void WND_ScreenToNClient(PWINOBJ pWin, int* pX, int* pY)
{
    RECT rect;

    ASSERT(pWin != NULL);
    ASSERT(pX != NULL && pY != NULL);

    WND_GetRect(pWin, &rect, W_TOPNC, XY_SCREEN);

    *pX -= rect.left;
    *pY -= rect.top;
}

void WND_ScreenToClient(PWINOBJ pWin, int* pX, int* pY)
{
    RECT rect;

    ASSERT(pWin != NULL);
    ASSERT(pX != NULL && pY != NULL);
    
    WND_GetRect(pWin, &rect, W_CLIENT, XY_SCREEN);

    *pX -= rect.left;
    *pY -= rect.top;
}


