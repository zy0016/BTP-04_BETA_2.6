/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display driver
 *
 * Purpose  : Implement the scan line algorithm.
 *            
\**************************************************************************/

#include "dispcomn.h"
/*********************************************************************\
* Function	   InsertEdge
* Purpose      Insert a edge to the edge list. 
               Sort by y from small to big.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL InsertEdge(PEDGE *ppEdgeList, PEDGE pEdge)
{
    PEDGE pNode, pPreNode;
    ASSERT(ppEdgeList != NULL && pEdge != NULL);

    if (*ppEdgeList == NULL)
    {
        pEdge->pNext = NULL;
        *ppEdgeList = pEdge;
        return TRUE;
    }

    pNode = *ppEdgeList;
    pPreNode = NULL;

    /* sort by y*/
    while (pNode->y1 < pEdge->y1)
    {
        pPreNode = pNode;
        pNode = pNode->pNext;
        if (!pNode)
            break;
    }

    if (pNode)
    {
        /* sort by x when pNode->y1 == pEdge->y1 */
        while (pNode->y1 == pEdge->y1 && pNode->cx <= pEdge->cx)
        {
            pPreNode = pNode;
            pNode = pNode->pNext;
            if (!pNode)
                break;
        }
    }

    /* add to the head of the list */
    if (pPreNode == NULL)
    {
        pEdge->pNext = pNode;
        *ppEdgeList = pEdge;
        return TRUE;
    }
    /* add to the list */
    else
    {
        pPreNode->pNext = pEdge;
        pEdge->pNext = pNode;
    }

    return TRUE;
}

/*********************************************************************\
* Function	   InsertEdgeReverse
* Purpose      Insert a edge to the edge list. 
               Sort by y from big to small.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL InsertEdgeReverse(PEDGE *ppEdgeList, PEDGE pEdge)
{
    PEDGE pNode, pPreNode;
    ASSERT(ppEdgeList != NULL && pEdge != NULL);

    if (*ppEdgeList == NULL)
    {
        pEdge->pNext = NULL;
        *ppEdgeList = pEdge;
        return TRUE;
    }

    pNode = *ppEdgeList;
    pPreNode = NULL;

    /* sort by y*/
    while (pNode->y1 > pEdge->y1)
    {
        pPreNode = pNode;
        pNode = pNode->pNext;
        if (!pNode)
            break;
    }

    if (pNode)
    {
        /* sort by x when pNode->y1 == pEdge->y1 */
        while (pNode->y1 == pEdge->y1 && pNode->cx <= pEdge->cx)
        {
            pPreNode = pNode;
            pNode = pNode->pNext;
            if (!pNode)
                break;
        }
    }

    /* add to the head of the list */
    if (pPreNode == NULL)
    {
        pEdge->pNext = pNode;
        *ppEdgeList = pEdge;
        return TRUE;
    }
    /* add to the list */
    else
    {
        pPreNode->pNext = pEdge;
        pEdge->pNext = pNode;
    }

    return TRUE;
}

/*********************************************************************\
* Function	   SortActiveEdge
* Purpose      Insert a edge to the active edge list. 
               Sort by cx from small to big.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SortActiveEdge(PEDGE* ppEdgeList)
{
    PEDGE pEnd, pNode, pPreNode, pTmp;
    BOOL bChanged = TRUE;

    pEnd = NULL;

    /* null list to be sorted */
    if (*ppEdgeList == NULL)
        return;

    while (pEnd != (*ppEdgeList)->pNext && bChanged)
    {
        bChanged = FALSE;
        pNode = *ppEdgeList;
        /* get the head node */
        if (pNode->cx > pNode->pNext->cx)
        {
            *ppEdgeList = pNode->pNext;
            pNode->pNext = (*ppEdgeList)->pNext;
            (*ppEdgeList)->pNext = pNode;
            bChanged = TRUE;
        }

        pPreNode = *ppEdgeList;
        while (pNode->pNext != pEnd)
        {
            if (pNode->cx > pNode->pNext->cx)
            {
                pTmp = pNode->pNext;
                pPreNode->pNext = pTmp;
                pNode->pNext = pTmp->pNext;
                pTmp->pNext = pNode;
                pPreNode = pTmp;
                bChanged = TRUE;
            }
            else
            {
                pPreNode = pNode;
                pNode = pNode->pNext;
            }
        }

        pEnd = pNode;
    }

    return;
}
