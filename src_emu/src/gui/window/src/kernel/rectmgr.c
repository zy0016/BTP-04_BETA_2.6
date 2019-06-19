/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements the rectangle functions.
 *            
\**************************************************************************/

#include "hpwin.h"

/*
**  Function : SetRect
**  Purpose  :
**      Sets the coordinates of the specified rectangle. This is equivalent
**      to assigning the left, top, right, and bottom arguments to the 
**      appropriate members of the RECT structure.
**  Params   :
**      lpRect : Points to the RECT structure that contains the rectangle to 
**               be set-> 
**      x1     : The x-coordinate of the rectangle's upper-left corner.
**      y1     : The y-coordinate of the rectangle's upper-left corner. 
**      x1     : The x-coordinate of the rectangle's upper-left corner. 
**      y1     : The y-coordinate of the rectangle's low-right corner. 
*/
BOOL WINAPI SetRect(PRECT pRect, int x1, int y1, int x2, int y2)
{
    if (!pRect)
        return FALSE;

    pRect->left = x1;
    pRect->top = y1;
    pRect->right = x2;
    pRect->bottom = y2;

    return TRUE;
}

/*
**  Function : SetRectEmpty
**  Purpose  :
**      Creates an empty rectangle in which all coordinates are set to zero.
**  Purpose  :
**      pRect : Points to the RECT structure that contains the coordinates of 
**      the rectangle.
*/
BOOL WINAPI SetRectEmpty(PRECT pRect)
{
    if (!pRect)
        return FALSE;

    pRect->left = 0;
    pRect->top = 0;
    pRect->right = 0;
    pRect->bottom = 0;

    return TRUE;
}

/*
**  Function : CopyRect
**  Purpose  :
**      Copies the coordinates of one rectangle to another.
**  Params   :
**      prcDst : Points to the RECT structure that will receive the logical 
**               coordinates of the source rectangle.
**      prcSrc : Points to the RECT structure whose coordinates are to be 
**               copied. 
*/
BOOL WINAPI CopyRect(PRECT prcDst, const RECT* prcSrc)
{
    if (!prcDst)
        return FALSE;

    if (prcSrc)
    {
        prcDst->left = prcSrc->left;
        prcDst->top = prcSrc->top;
        prcDst->right = prcSrc->right;
        prcDst->bottom = prcSrc->bottom;
    }
    else
        SetRectEmpty(prcDst);

    return TRUE;
}

/*
**  Function : IsRectEmpty
**  Purpose  :
**      Determines whether the specified rectangle is empty-> A empty 
**      rectangle is one that has no area; that is, the coordinate of 
**      the right side is equal to the coordinate of the left side, or 
**      the coordinate of the bottom side is equal to the coordinate 
**      of the top side.
**  Purpose  :
**      pRect : Points to a RECT structure that contains the logical 
**              coordinates of the rectangle.
**  Return   :
**      If the rectangle is empty, returns TRUE; otherwise, return FALSE.
**      To get extended error information, call GetLastError.
*/
BOOL WINAPI IsRectEmpty(const RECT* pRect)
{
    if (!pRect)
        return FALSE;

    if (pRect->left == pRect->right || pRect->top == pRect->bottom)
        return TRUE;

    return FALSE;
}

/*
**  Function : EqualRect
**  Purpose  :
**      Determines whether the two specified rectangles are equal by 
**      comparing the coordinates of their upper-left and lower-right 
**      corners-> 
**  Params   :
**      pRect1 : Points to a RECT structure that contains the logical 
**               coordinates of the first rectangle. 
**      pRect2 : Points to a RECT structure that contains the logical 
**               coordinates of the second rectangle. 
**  Return   :
**      If the two rectangles are identical, return TRUE; otherwise, 
**      return FALSE. To get extended error information, call GetLastError.
*/
BOOL WINAPI EqualRect(const RECT* pRect1, const RECT* pRect2)
{
    if (!pRect1 || !pRect2)
        return FALSE;

    if (pRect1->left == pRect2->left && pRect1->top == pRect2->top &&
        pRect1->right == pRect2->right && pRect1->bottom == pRect2->bottom)
        return TRUE;

    return FALSE;
}

/*
**  Function : IntersectRect
**  Purpose  :
**      Calculates the intersection of two source rectangles and places 
**      the coordinates of the intersection rectangle into the destination 
**      rectangle-> If the source rectangles do not intersect, an empty 
**      rectangle (in which all coordinates are set to zero) is placed into 
**      the destination rectangle. 
**  Params   :
**      prcDst  : Points to the RECT structure that is to receive the 
**                intersection of the rectangles pointed to by the lprcSrc1 
**                and lprcSrc2 parameters-> 
**      prcSrc1 : Points to the RECT structure that contains the first 
**                source rectangle. 
**      prcSrc2 : Points to the RECT structure that contains the second
**                source rectangle. 
**  Return   :
**      If the rectangles intersect, the return value is TRUE; otherwise, 
**      it is FALSE. To get extended error information, call GetLastError. 
*/
BOOL WINAPI IntersectRect(PRECT prcDst, const RECT* prcSrc1, 
                          const RECT* prcSrc2)
{
    RECT rect;
    RECT rcSrc1, rcSrc2;

    if (!prcSrc1 || !prcSrc2)
        return FALSE;

    rcSrc1 = *prcSrc1;
    NormalizeRect(&rcSrc1);
    
    rcSrc2 = *prcSrc2;
    NormalizeRect(&rcSrc2);

    if (rcSrc1.left >= rcSrc2.right || rcSrc1.top >= rcSrc2.bottom ||
        rcSrc1.right <= rcSrc2.left || rcSrc1.bottom <= rcSrc2.top)
    {
        if (prcDst)
            SetRectEmpty(prcDst);

        return FALSE;
    }
    
    if (rcSrc1.left < rcSrc2.left)
        rect.left = rcSrc2.left;
    else
        rect.left = rcSrc1.left;

    if (rcSrc1.top < rcSrc2.top)
        rect.top = rcSrc2.top;
    else
        rect.top = rcSrc1.top;

    if (rcSrc1.right < rcSrc2.right)
        rect.right = rcSrc1.right;
    else
        rect.right = rcSrc2.right;

    if (rcSrc1.bottom < rcSrc2.bottom)
        rect.bottom = rcSrc1.bottom;
    else
        rect.bottom = rcSrc2.bottom;

    if (prcDst)
        CopyRect(prcDst, &rect);

    return TRUE;
}

/*
**  Function : UnionRect
**  Purpose  :
**      Creates the union of two rectangles. The union is the smallest 
**      rectangle that contains both source rectangles. 
**  Params   :
**      prcDst  : Points to the RECT structure that will receive a rectangle 
**                containing the rectangles pointed to by the prcSrc1 and 
**                prcSrc2 parameters. 
**      prcSrc1 : Points to the RECT structure that contains the first 
**                source rectangle. 
**      prcSrc2 : Points to the RECT structure that contains the second 
**                source rectangle. 
**  Return   :
**      If the specified structure contains a nonempty rectangle, return
**      TRUE; otherwise, return FALSE. To get extended error information, 
**      call GetLastError. 
**  Remarks  :
**      Windows ignores the dimensions of an empty rectangle. that is, a 
**      rectangle in which all coordinates are set to zero, so that it 
**      has no height or no width. 
*/
BOOL WINAPI UnionRect(PRECT prcDst, const RECT* prcSrc1, const RECT* prcSrc2)
{
    RECT rect;
    RECT rcSrc1, rcSrc2;

    if (!prcDst)
        return FALSE;

    if (!prcSrc1 && !prcSrc2)
    {
        SetRectEmpty(prcDst);
        return TRUE;
    }

    if (!prcSrc1 || IsRectEmpty(prcSrc1))
    {
        CopyRect(prcDst, prcSrc2);
        return TRUE;
    }

    if (!prcSrc2 || IsRectEmpty(prcSrc2))
    {
        CopyRect(prcDst, prcSrc1);
        return TRUE;
    }

    rcSrc1 = *prcSrc1;
    NormalizeRect(&rcSrc1);
    
    rcSrc2 = *prcSrc2;
    NormalizeRect(&rcSrc2);

    if (rcSrc1.left < rcSrc2.left)
        rect.left = rcSrc1.left;
    else
        rect.left = rcSrc2.left;

    if (rcSrc1.top < rcSrc2.top)
        rect.top = rcSrc1.top;
    else
        rect.top = rcSrc2.top;

    if (rcSrc1.right > rcSrc2.right)
        rect.right = rcSrc1.right;
    else
        rect.right = rcSrc2.right;

    if (rcSrc1.bottom > rcSrc2.bottom)
        rect.bottom = rcSrc1.bottom;
    else
        rect.bottom = rcSrc2.bottom;

    CopyRect(prcDst, &rect);

    return TRUE;
}

/*
**  Function : SubtractRect
**  Purpose  :
**      Obtains the coordinates of a rectangle determined by subtracting 
**      one rectangle from another. 
**  Params   :
**      prcDst  : Points to a RECT structure that receives the coordinates 
**                of the rectangle determined by subtracting the rectangle 
**                pointed to by prcSrc2 from the rectangle pointed to by 
**                prcSrc1. 
**      prcSrc1 : Points to a RECT structure from which the function 
**                subtracts the rectangle pointed to by prcSrc2. 
**      prcSrc2 : Points to a RECT structure that the function subtracts 
**                from the rectangle pointed to by lprcSrc1. 
**  Return   :
**      If the function succeeds, return TRUE.
**      If the function fails, return FALSE. To get extended error 
**      information, call GetLastError. 
**  Remark   :
**      The function only subtracts the rectangle specified by prcSrc2 
**      from the rectangle specified by prcSrc1 when the rectangles 
**      intersect completely in either the x- or y-direction. For example, 
**      if *prcSrc1 has the coordinates (10,10,100,100) and *prcSrc2 has 
**      the coordinates (50,50,150,150), the function sets the coordinates 
**      of the rectangle pointed to by prcDst to (10,10,100,100). If 
**      *prcSrc1 has the coordinates (10,10,100,100) and *prcSrc2 has the 
**      coordinates (50,10,150,150), however, the function sets the 
**      coordinates of the rectangle pointed to by prcDst to (10,10,50,100). 
*/
BOOL WINAPI SubtractRect(PRECT prcDst, const RECT* prcSrc1, 
                         const RECT* prcSrc2)
{
    RECT rect;
    RECT rcSrc1, rcSrc2;
    
    if (!prcDst)
        return FALSE;

    if (!prcSrc1)
    {
        SetRectEmpty(prcDst);
        return FALSE;
    }

    if (!prcSrc2 || !IntersectRect(&rect, prcSrc1, prcSrc2))
    {
        CopyRect(prcDst, prcSrc1);
        return TRUE;
    }

    rcSrc1 = *prcSrc1;
    NormalizeRect(&rcSrc1);
    
    rcSrc2 = *prcSrc2;
    NormalizeRect(&rcSrc2);

    // If the first rectangle is all in second rectangle, set the 
    // destination rectangle to empty rectangle
    if (rcSrc1.left >= rcSrc2.left && rcSrc1.right <= rcSrc2.right &&
        rcSrc1.top >= rcSrc2.top && rcSrc1.bottom <= rcSrc2.bottom)
    {
        SetRectEmpty(prcDst);
        return TRUE;
    }

    // x-direction intersect completely
    if (rcSrc1.left >= rcSrc2.left && rcSrc1.right <= rcSrc2.right)
    {
        // Top intersect
        if (rcSrc2.top <= rcSrc1.top)
        {
            rect.left = rcSrc1.left;
            rect.top = rcSrc2.bottom;
            rect.right = rcSrc1.right;
            rect.bottom = rcSrc1.bottom;

            CopyRect(prcDst, &rect);

            return TRUE;
        }

        // Bottom intersect
        if (rcSrc2.bottom >= rcSrc1.bottom)
        {
            rect.left = rcSrc1.left;
            rect.top = rcSrc1.top;
            rect.right = rcSrc1.right;
            rect.bottom = rcSrc2.top;

            CopyRect(prcDst, &rect);

            return TRUE;
        }

        // Internal intersect
        CopyRect(prcDst, prcSrc1);

        return TRUE;
    }

    // y-direction intersect
    if (rcSrc1.top >= rcSrc2.top && rcSrc1.bottom <= rcSrc2.bottom)
    {
        // Left intersection
        if (rcSrc2.left <= rcSrc1.left)
        {
            rect.left = rcSrc2.right;
            rect.top = rcSrc1.top;
            rect.right = rcSrc1.right;
            rect.bottom = rcSrc1.bottom;

            CopyRect(prcDst, &rect);

            return TRUE;
        }

        // Right intersection
        if (rcSrc2.right >= rcSrc1.right)
        {
            rect.left = rcSrc1.left;
            rect.top = rcSrc1.top;
            rect.right = rcSrc2.left;
            rect.bottom = rcSrc1.bottom;

            CopyRect(prcDst, &rect);

            return TRUE;
        }

        // Internal contains
        CopyRect(prcDst, prcSrc1);

        return TRUE;
    }

    CopyRect(prcDst, prcSrc1);

    return TRUE;
}

/*
**  Function : OffsetRect
**  Purpose  :
**      Moves the specified rectangle by the specified offsets. 
**  Params   :
**      pRect     : Contains the logical coordinates of the rectangle 
**                  to be moved.
**      iwOffsetX : Specifies the amount to move the rectangle left or 
**                  right. This parameter must be a negative value to 
**                  move the rectangle to the left. 
**      iwOffsetY : Specifies the amount to move the rectangle up or 
**                  down. This parameter must be a negative value to 
**                  move the rectangle up. 
*/
BOOL WINAPI OffsetRect(PRECT pRect, int nOffsetX, int nOffsetY)
{
    if (!pRect)
        return FALSE;

    pRect->left += nOffsetX;
    pRect->top += nOffsetY;
    pRect->right += nOffsetX;
    pRect->bottom += nOffsetY;

    return TRUE;
}

/*
**  Function : InflateRect
**  Purpose  :
**      Increases or decreases the width and height of the specified 
**      rectangle. The InflateRect function adds dx units to the left 
**      and right ends of the rectangle and dy units to the top and 
**      bottom. The dx and dy parameters are signed values; positive 
**      values increase the width and height, and negative values 
**      decrease them. 
**  Params   :
**      pRect     : Points to the RECT structure that increases or 
**                  decreases in size. 
**      iwOffsetX : Specifies the amount to increase or decrease the 
**                  rectangle width. This parameter must be negative 
**                  to decrease the width. 
**      iwOffsetY : Specifies the amount to increase or decrease the 
**                  rectangle height. This parameter must be negative 
**                  to decrease the height. 
*/
BOOL WINAPI InflateRect(PRECT pRect, int nOffsetX, int nOffsetY)
{
    BOOL bResult = TRUE;

    if (!pRect)
        return FALSE;

    if (pRect->left < pRect->right)
    {
        pRect->left  -= nOffsetX;
        pRect->right += nOffsetX;

        if (pRect->left > pRect->right)
            bResult = FALSE;
    }
    else
    {
        pRect->left  += nOffsetX;
        pRect->right -= nOffsetX;

        if (pRect->left < pRect->right)
            bResult = FALSE;
    }

    if (pRect->top < pRect->bottom)
    {
        pRect->top    -= nOffsetY;
        pRect->bottom += nOffsetY;

        if (pRect->top > pRect->bottom)
            bResult = FALSE;
    }
    else
    {
        pRect->top    += nOffsetY;
        pRect->bottom -= nOffsetY;

        if (pRect->top < pRect->bottom)
            bResult = FALSE;
    }

    return bResult;
}

/*
**  Function : PtInRect
**  Purpose  :
**      Determines whether the specified point lies within the specified 
**      rectangle. A point is within a rectangle if it lies on the left 
**      or top side or is within all four sides. A point on the right or
**      bottom side is considered outside the rectangle. 
**  Params   :
**      pRect : Points to a RECT structure that contains the specified 
**              rectangle.
**      pt    : Specifies the point.
**  Return   :
**      If the specified point lies within the rectangle, return TRUE.
**      If the function fails, return FALSE. To get extended error 
**      information, call GetLastError. 
*/
BOOL WINAPI PtInRect(const RECT* pRect, POINT pt)
{
    RECT rect;

    if (!pRect)
        return FALSE;

    rect = *pRect;
    NormalizeRect(&rect);

    if (pt.x < rect.left || pt.x >= rect.right || 
        pt.y < rect.top || pt.y >= rect.bottom)
        return FALSE;

    return TRUE;
}


/*
**  Function : PtInRectXY
**  Purpose  :
**      Determines whether the specified point lies within the specified 
**      rectangle. A point is within a rectangle if it lies on the left 
**      or top side or is within all four sides. A point on the right or
**      bottom side is considered outside the rectangle. 
**  Params   :
**      pRect : Points to a RECT structure that contains the specified 
**              rectangle.
**      x     : Specifies the x-coordinate of the point.
**      y     : Specifies the y-coordinate of the point.
**  Return   :
**      If the specified point lies within the rectangle, return TRUE.
**      If the function fails, return FALSE. To get extended error 
**      information, call GetLastError. 
*/
BOOL WINAPI PtInRectXY(const RECT* pRect, int x, int y)
{
    RECT rect;

    if (!pRect)
        return FALSE;

    rect = *pRect;
    NormalizeRect(&rect);

    if (x < rect.left || x >= rect.right || y < rect.top || 
        y >= rect.bottom)
        return FALSE;

    return TRUE;
}

/*
**  Function : NormalizeRect
**  Purpose  :
**      Normalizes RECT so that both the height and width are positive. '
**  Params   :
**      pRect : Specifies the rect to be normalized.
**  Return   :
**      If the specified point lies within the rectangle, return TRUE.
**      If the function fails, return FALSE.
*/
BOOL WINAPI NormalizeRect(PRECT pRect)
{
    int tmp;

    if (!pRect)
    {
        SetLastError(1);
        return FALSE;
    }

    if (pRect->left > pRect->right)
    {
        tmp = pRect->left;
        pRect->left = pRect->right + 1;
        pRect->right = tmp + 1;
    }

    if (pRect->top > pRect->bottom)
    {
        tmp = pRect->top;
        pRect->top = pRect->bottom + 1;
        pRect->bottom = tmp + 1;
    }

    return TRUE;
}
