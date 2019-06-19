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
                // �õ���pChild����֮��
                if (ISMAPPED(pChild) && PtInRectXY(&pChild->rcWindow, x, y))
                {
                    pWin = pChild;

                    // Child != NULL, ��Ȼ�����ٴδӸô��ڵ��Ӵ��ڿ�ʼ����
                    break;
                }

                pChild = pChild->pNext;
            }
        }
        else if (PtInRectXY(&pWin->rcWindow, x, y))
        {
            // x, yλ�ڴ��ڵķǿͻ����ڣ�ֻ����Ҵ��ڵķǿͻ����Ӵ���
            while (pChild && ISNCCHILD(pChild))
            {
                // �õ���pChild����֮��
                if (ISMAPPED(pChild) && PtInRectXY(&pChild->rcWindow, x, y))
                {
                    pWin = pChild;

                    // pChild != NULL, ��Ȼ�����ٴδӸô��ڵ��Ӵ��ڿ�ʼ����
                    break;
                }

                pChild = pChild->pNext;
            }

            // pChild = NULL��ʾ�����κ��Ӵ�����
            pChild = NULL;
        }
        else
        {
            //���곬������Ļ��������滷����ASSERT����ʵ��Ӧ�˳���������ѭ��
            //�����Ǳ�У׼
//            ASSERT(0);
            pChild = NULL;
        }

        // ���ϵ�ѭ����Ȼ����ʱ����ʾ���е��Ӵ��ڶ��������õ�

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


