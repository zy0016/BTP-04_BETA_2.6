/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Region object management module. 
 *            
\**************************************************************************/

#include "hpwin.h"

#ifdef RGNSUPPORT

#include "wsobj.h"
#include "wsorgn.h"

#include "string.h"

#define RDH_RECTANGLES  1

#define INITRECTCNTOFRGN 10
#define RECTINCRCNTOFRGN 10

#define TOTAL_OVERLAY   0
#define LEFT_OVERLAY    1
#define RIGHT_OVERLAY   2
#define CENTER_OVERLAY  3

#define TOP_OVERLAY     1
#define BOTTOM_OVERLAY  2

static void AddRectIntoRgn( RGNOBJ * pRgn, RECT rc );
static void RgnAndRect( RGNOBJ *pRgn, RGNOBJ *pRgnResult, RECT rcAnd );
static void RgnOrRect( RGNOBJ *pRgn, RECT rcOr );
static void * ReMemAlloc( void *memblock, size_t size, size_t oldsize );

PRGNOBJ     RGN_Create( int nLeftRect, int nTopRect, int nRightRect,
                       int nBottomRect )
{
    PRGNOBJ pRgn = NULL;
    HANDLE  handle = NULL;
    LONG *pTemp;

    pRgn = (PRGNOBJ)MemAlloc(sizeof(RGNDATA) + INITRECTCNTOFRGN * sizeof(RECT) + 
            sizeof(handle));
    // Ensure MemAlloc function succeeds
    if (!pRgn)
    {
        SetLastError(1);
        return NULL;
    }

     // Ensure the region object is added in object table successfully
    if (!WOT_RegisterObj((PWSOBJ)pRgn, OBJ_RGN, FALSE))
    {
        MemFree(pRgn);

        SetLastError(1);
        return NULL;
    }
    pRgn->rgndata.dwSize = sizeof(RGNDATA);
    pRgn->rgndata.iType = RDH_RECTANGLES;
    pRgn->rgndata.nCount = 1;
    pRgn->rgndata.nTotalCount = INITRECTCNTOFRGN;
    pRgn->rgndata.nRgnSize = pRgn->rgndata.nCount * sizeof(RECT);
    
    pTemp = (LONG *) pRgn->Buffer;
    *pTemp++ = nLeftRect;
	*pTemp++ = nTopRect;
	*pTemp++ = nRightRect;
	*pTemp = nBottomRect;

    return pRgn;
}

BOOL   RGN_Destroy(PRGNOBJ pRgn)
{
    if(pRgn == NULL)
        return FALSE;

    ASSERT(pRgn->bDeleted && pRgn->refcount == 0);
    
    WOT_UnregisterObj(pRgn);//Remove the region object from window object table

    MemFree(pRgn);
    
    return TRUE;

}


int RGN_Offset( PRGNOBJ pRgn, int nXOffset, int nYOffset )
{
    LONG *pTemp = NULL;
    int i;

    if(!pRgn)
        return NULLREGION;
    pTemp = (LONG *) pRgn->Buffer;
    
    for (i = pRgn->rgndata.nCount; i >= 1; i--)
    {
        *pTemp++ += nXOffset;
        *pTemp++ += nYOffset;
        *pTemp++ += nXOffset;
        *pTemp++ += nYOffset;
    }
    if(pRgn->rgndata.nCount == 0)
        return NULLREGION;
    else if(pRgn->rgndata.nCount == 1)
        return SIMPLEREGION;
    else if (pRgn->rgndata.nCount > 1)
        return COMPLEXREGION;   

    return RGN_ERROR;
}
int RGN_Combine( PRGNOBJ pRgnDest, PRGNOBJ pRgnSrc1, PRGNOBJ pRgnSrc2, int fnCombineMode )
{
    
	RGNOBJ *pRgn = NULL;
    HANDLE handle;
	RECT rcSrc;
	LONG *pTempSrc = NULL;
	DWORD dwDataSize;
	int i;

    
    /* initialize data of the result region. */
    pRgn = (PRGNOBJ)MemAlloc(sizeof(RGNDATA) + INITRECTCNTOFRGN * sizeof(RECT) + 
            sizeof(HANDLE));
    // Ensure MemAlloc function succeeds
    if (!pRgn)
    {
        SetLastError(1);
        return NULL;
    }

     // Ensure the region object is added in object table successfully
    if (!WOT_RegisterObj((PWSOBJ)pRgn, OBJ_RGN, FALSE))
    {
        MemFree(pRgn);

        SetLastError(1);
        return NULL;
    }
    pRgn->rgndata.dwSize = sizeof(RGNDATA);
    pRgn->rgndata.iType = RDH_RECTANGLES;
    pRgn->rgndata.nCount = 0;
    pRgn->rgndata.nTotalCount = INITRECTCNTOFRGN;
    pRgn->rgndata.nRgnSize = 0;

    switch (fnCombineMode) {
	case RGN_AND:
		pTempSrc = (LONG *)pRgnSrc2->Buffer;
		for (i = pRgnSrc2->rgndata.nCount; i >= 1; i--)
		{
			rcSrc.left = *pTempSrc++;
			rcSrc.top = *pTempSrc++;
			rcSrc.right = *pTempSrc++;
			rcSrc.bottom = *pTempSrc++;

			RgnAndRect(pRgnSrc1, pRgn, rcSrc);
		}
		break;
    case RGN_COPY:
        pTempSrc = (LONG *)pRgnSrc1->Buffer;
        for(i = pRgnSrc1->rgndata.nCount; i>=1; i--)
        {
            rcSrc.left = *pTempSrc++;
			rcSrc.top = *pTempSrc++;
			rcSrc.right = *pTempSrc++;
			rcSrc.bottom = *pTempSrc++;
            AddRectIntoRgn(pRgn, rcSrc); 
        }
        break;

	case RGN_OR:
		/* copy data of region hrgnSrc1 into the result region. */
		dwDataSize = sizeof(RGNDATA) + pRgnSrc1->rgndata.nCount * sizeof(RECT)
                + sizeof(HANDLE);
		if (pRgn->rgndata.nTotalCount < pRgnSrc1->rgndata.nCount)
		{
			pRgn = (RGNOBJ *)ReMemAlloc( pRgn, sizeof(RGNDATA) + 
				pRgnSrc1->rgndata.nTotalCount * sizeof(RECT) + sizeof(HANDLE),
                sizeof(RGNDATA) + pRgn->rgndata.nCount * sizeof(RECT) +
                sizeof(HANDLE));
		}
		memcpy(pRgn, pRgnSrc1, dwDataSize);

		pTempSrc = (LONG *)pRgnSrc2->Buffer;
		for (i = pRgnSrc2->rgndata.nCount; i >= 1; i--)
		{
			rcSrc.left = *pTempSrc++;
			rcSrc.top = *pTempSrc++;
			rcSrc.right = *pTempSrc++;
			rcSrc.bottom = *pTempSrc++;

			RgnOrRect(pRgn, rcSrc);
		}
		break;

	default:
		break;
	}

	/* copy data of the result region into the dest region. */
	dwDataSize = sizeof(RGNDATA) + pRgn->rgndata.nCount * sizeof(RECT) + 
            sizeof(HANDLE);
	if (pRgnDest->rgndata.nTotalCount < pRgn->rgndata.nCount)
	{
		pRgnDest = (RGNOBJ *)ReMemAlloc( pRgnDest, 
            sizeof(RGNDATA) + pRgn->rgndata.nTotalCount * sizeof(RECT) 
            + sizeof(HANDLE), 
            sizeof(RGNDATA) + pRgnDest->rgndata.nCount * sizeof(RECT) 
            + sizeof(HANDLE));
	}
	memcpy(pRgnDest, pRgn, dwDataSize);
	MemFree(pRgn);

	if (pRgnDest->rgndata.nCount == 0)
		return NULLREGION;
	else if (pRgnDest->rgndata.nCount == 1)
		return SIMPLEREGION;
	else if (pRgnDest->rgndata.nCount > 1)
		return COMPLEXREGION;

	return RGN_ERROR;
}

static void * ReMemAlloc( void *memblock, size_t size, size_t oldsize )
{
    void * newblock;

    if(size <= oldsize)
        return memblock;
    else
    {
        newblock = MemAlloc(size);
        memcpy(newblock, memblock, oldsize);
        MemFree(memblock);
        return newblock;
    }
}

static void AddRectIntoRgn(RGNOBJ * pRgn, RECT rc) 
{
	LONG *pTemp = NULL;

	if (pRgn->rgndata.nTotalCount <= pRgn->rgndata.nCount) {
		pRgn = (RGNOBJ *)ReMemAlloc( pRgn,
            sizeof(RGNDATA) + (pRgn->rgndata.nTotalCount + RECTINCRCNTOFRGN) * 
            sizeof(RECT) + sizeof(HANDLE), 
            sizeof(RGNDATA) + pRgn->rgndata.nCount * sizeof(RECT)
            + sizeof(HANDLE));
		pRgn->rgndata.nTotalCount += 10;
	}

	pTemp = (LONG *)( pRgn->Buffer + pRgn->rgndata.nCount * sizeof(RECT) );
	*pTemp++ = rc.left;
	*pTemp++ = rc.top;
	*pTemp++ = rc.right;
	*pTemp = rc.bottom;
	pRgn->rgndata.nCount++;
	pRgn->rgndata.nRgnSize = pRgn->rgndata.nCount * sizeof(RECT);
}

static void RgnAndRect(RGNOBJ *pRgn, RGNOBJ *pRgnResult, RECT rcAnd) 
{
	int i;
	RECT rcRgn, rcResult;
	LONG *pTemp = NULL;

	for (i = pRgn->rgndata.nCount; i >= 1; i--)
	{
		pTemp = (LONG *)( pRgn->Buffer + (pRgn->rgndata.nCount - i) * sizeof(RECT) );
		rcRgn.left = *pTemp;
		rcRgn.top = *(pTemp+1);
		rcRgn.right = *(pTemp+2);
		rcRgn.bottom = *(pTemp+3);

		IntersectRect(&rcResult, &rcRgn, &rcAnd);
		if ( !IsRectEmpty(&rcResult) ) {
			AddRectIntoRgn(pRgnResult, rcResult);
		}
	}
}

static void RgnOrRect(RGNOBJ *pRgn, RECT rcOr)
{
	int i;
	RECT rcRgn, rcSplit1, rcSplit2;
    BYTE xOverlay, yOverlay;
	LONG *pTemp = NULL;

	for (i = pRgn->rgndata.nCount; i >= 1; i--)
	{
		pTemp = (LONG *)( pRgn->Buffer + (pRgn->rgndata.nCount - i) * sizeof(RECT) );
		rcRgn.left = *pTemp;
		rcRgn.top = *(pTemp+1);
		rcRgn.right = *(pTemp+2);
		rcRgn.bottom = *(pTemp+3);

        /* Check if no overlay */
        if (rcOr.right <= rcRgn.left || rcRgn.right <= rcOr.left ||
            rcOr.bottom <= rcRgn.top || rcRgn.bottom <= rcOr.top)
            continue;

        // Check overlap mode.
        if (rcOr.left > rcRgn.left)
        {
            if (rcRgn.right > rcOr.right)
                xOverlay = CENTER_OVERLAY;
            else
                xOverlay = RIGHT_OVERLAY;
        }
        else    // (rcOr.left <= rcRgn.left)
        {
            if (rcRgn.right <= rcOr.right)
                xOverlay = TOTAL_OVERLAY;
            else
                xOverlay = LEFT_OVERLAY;
        }

        if (rcOr.top > rcRgn.top)
        {
            if (rcRgn.bottom > rcOr.bottom)
                yOverlay = CENTER_OVERLAY;
            else
                yOverlay = BOTTOM_OVERLAY;
        }
        else    // (rcOr.top <= rcRgn.top)
        {
            if (rcRgn.bottom <= rcOr.bottom)
                yOverlay = TOTAL_OVERLAY;
            else
                yOverlay = TOP_OVERLAY;
        }

        switch ((xOverlay << 2) | yOverlay)
        {
        case (TOTAL_OVERLAY << 2) | TOTAL_OVERLAY :

            // Total overlayed, just remove the region clip rectangle.
            //
            // +---------------+    
            // | +---------+   |
            // | |         |   |
            // | |         |   |
            // | |         |   |
            // | |         |   |
            // | |         |   |
            // | +---------+   |
            // +---------------+
            //

			pRgn->rgndata.nCount--;
			memcpy( pTemp, pTemp+4, (i-1)*sizeof(RECT) );
			RgnOrRect(pRgn, rcOr);

			return;

        case (TOTAL_OVERLAY << 2) | TOP_OVERLAY:

            // Change top coordinate of the region clip rectangle.
            //
            //  +----------------+  
            //  | +----------+   |
            //  | |          |   |
            //  | |          |   |
            //  +-+----------+---+
            //    |          |
            //    |    1     |
            //    +----------+
            //

            *(pTemp+1) = rcOr.bottom;
			RgnOrRect(pRgn, rcOr);

			return;

        case (TOTAL_OVERLAY << 2) | BOTTOM_OVERLAY:

            // Change bottom coordinate of the region clip rectangle.
            //
            //    +-----------+ 
            //    |           |
            //    |     1     |
            //    |           |
            // +--+-----------+---+
            // |  |           |   |
            // |  +-----------+   |
            // +------------------+
            //

            *(pTemp+3) = rcOr.top;
			RgnOrRect(pRgn, rcOr);

			return;

        case (LEFT_OVERLAY << 2) | TOTAL_OVERLAY:

            // Change left coordinate of the region clip rectangle.
            //
            // +--------+
            // |  +-----+----+
            // |  |     |    |
            // |  |     | 1  |
            // |  |     |    |
            // |  +-----+----+
            // |        |    
            // +--------+

            *pTemp = rcOr.right;
			RgnOrRect(pRgn, rcOr);

			return;

        case (RIGHT_OVERLAY << 2) | TOTAL_OVERLAY:

            // Change right coordinate of the region clip rectangle.
            //         +------+
            //         |      |
            //  +------+---+  |
            //  |      |   |  |
            //  | 1    |   |  |
            //  |      |   |  |
            //  +------+---+  |
            //         |      |
            //         +------+

            *(pTemp+2) = rcOr.left;
			RgnOrRect(pRgn, rcOr);

			return;

        case (TOTAL_OVERLAY << 2) | CENTER_OVERLAY:

            // Split the "or" rectangle into 2 small rectangles.
			//
            //
            //    +----------+
            //    |          |
            // +--+----------+--+
            // | 1|          |2 |
            // |  |          |  |
            // +--+----------+--+
            //    |          |
            //    |          |
            //    +----------+
            //

            rcSplit1.left = rcOr.left;
			rcSplit1.top = rcOr.top;
			rcSplit1.right = rcRgn.left;
			rcSplit1.bottom = rcOr.bottom;
			RgnOrRect(pRgn, rcSplit1);

            rcSplit2.left = rcRgn.right;
			rcSplit2.top = rcOr.top;
			rcSplit2.right = rcOr.right;
			rcSplit2.bottom = rcOr.bottom;
			RgnOrRect(pRgn, rcSplit2);

			return;

        case (CENTER_OVERLAY << 2) | TOTAL_OVERLAY:

            // Split the "or" rectangle into 2 small rectangles.
            //
            //        +------+
            //        |   1  |
            // +------+------+-----+
            // |      |      |     |
            // |      |      |     |
            // |      |      |     |
            // |      |      |     |
            // +------+------+-----+
            //        |   2  |
            //        +------+
            //

            rcSplit1.left = rcOr.left;
			rcSplit1.top = rcOr.top;
			rcSplit1.right = rcOr.right;
			rcSplit1.bottom = rcRgn.top;
			RgnOrRect(pRgn, rcSplit1);

            rcSplit2.left = rcOr.right;
			rcSplit2.top = rcRgn.bottom;
			rcSplit2.right = rcOr.right;
			rcSplit2.bottom = rcOr.bottom;
			RgnOrRect(pRgn, rcSplit2);

			return;

        case (LEFT_OVERLAY << 2) | TOP_OVERLAY:

            // Split the "or" rectangle into 2 small rectangles.
            //
            // +-------+
            // |   1   |
            // |**+----+-------+
            // |  |    |       |
            // | 2|    |       |
            // +--+----+       |
            //    |            |
            //    |            |
            //    +------------+
            //

            rcSplit1.left = rcOr.left;
			rcSplit1.top = rcOr.top;
			rcSplit1.right = rcOr.right;
			rcSplit1.bottom = rcRgn.top;
			RgnOrRect(pRgn, rcSplit1);

            rcSplit2.left = rcOr.left;
			rcSplit2.top = rcRgn.top;
			rcSplit2.right = rcRgn.left;
			rcSplit2.bottom = rcOr.bottom;
			RgnOrRect(pRgn, rcSplit2);

			return;

        case (RIGHT_OVERLAY << 2) | TOP_OVERLAY :

            // Split the "or" rectangle into 2 small rectangles.
            //
			//       +--------+
            //       |    1   | 
            // +-----+-----***|
            // |     |    |   |
            // |     |    | 2 |
            // |     +----+---+
            // |          |
            // |          |
            // +----------+
            //

            rcSplit1.left = rcOr.left;
			rcSplit1.top = rcOr.top;
			rcSplit1.right = rcOr.right;
			rcSplit1.bottom = rcRgn.top;
			RgnOrRect(pRgn, rcSplit1);

            rcSplit2.left = rcRgn.right;
			rcSplit2.top = rcRgn.top;
			rcSplit2.right = rcOr.right;
			rcSplit2.bottom = rcOr.bottom;
			RgnOrRect(pRgn, rcSplit2);

			return;

        case (LEFT_OVERLAY << 2) | BOTTOM_OVERLAY :

            // Split the "or" rectangle into 2 small rectangles.
            //       +------------+
            //       |            |
            //       |            |
            //       |            |
            //   +---+------+     |
            //   | 2 |      |     |
            //   |   |      |     |
            //   |***+------+-----+
            //   |     1    |
            //   +----------+
            //

            rcSplit1.left = rcOr.left;
			rcSplit1.top = rcRgn.bottom;
			rcSplit1.right = rcOr.right;
			rcSplit1.bottom = rcOr.bottom;
			RgnOrRect(pRgn, rcSplit1);

            rcSplit2.left = rcOr.left;
			rcSplit2.top = rcOr.top;
			rcSplit2.right = rcRgn.left;
			rcSplit2.bottom = rcRgn.bottom;
			RgnOrRect(pRgn, rcSplit2);

			return;

        case (RIGHT_OVERLAY << 2) | BOTTOM_OVERLAY :

            // Split the "or" rectangle into 2 small rectangles.
            // +------------+
            // |            |
            // |            |
            // |       +----+----+
            // |       |    |    |
            // |       |    |  2 |
            // |       |    |    |
            // +-------+----+****|
            //         |    1    |
            //         +---------+
            //

            rcSplit1.left = rcOr.left;
			rcSplit1.top = rcRgn.bottom;
			rcSplit1.right = rcOr.right;
			rcSplit1.bottom = rcOr.bottom;
			RgnOrRect(pRgn, rcSplit1);

            rcSplit2.left = rcRgn.right;
			rcSplit2.top = rcOr.top;
			rcSplit2.right = rcOr.right;
			rcSplit2.bottom = rcRgn.bottom;
			RgnOrRect(pRgn, rcSplit2);

			return;

        case (LEFT_OVERLAY << 2) | CENTER_OVERLAY:

            // Change right coordinate of the "or" rectangle.
            //
            //      +------------+
            //      |     1      |
            //      |            |
            //  +---+---+        |
            //  |   |   |        |
            //  |   |   |        |
            //  +---+---+        |
            //      |            |
            //      |            |
            //      +------------+

            rcOr.right = rcRgn.left;
			RgnOrRect(pRgn, rcOr);

			return;

        case (RIGHT_OVERLAY << 2) | CENTER_OVERLAY :

            // Change left coordinate of the "or" rectangle.
            //
            //  +------------+
            //  |     1      |
            //  |            |
            //  |        +---+---+
            //  |        |   |   |
            //  |        |   |   |
            //  |        +---+---+
            //  |            |
            //  |            |
            //  +------------+

            rcOr.left = rcRgn.right;
			RgnOrRect(pRgn, rcOr);

			return;

        case (CENTER_OVERLAY << 2) | TOP_OVERLAY :

            // Change bottom coordinate of the "or" rectangle.
            //
            //         +-----+
            //         |     |
            // +-------+-----+------+
            // |       |     |      |
            // |       |     |      |
            // |       +-----+      |
            // |                    |
            // |          1         |
            // +--------------------+

            rcOr.bottom = rcRgn.top;
			RgnOrRect(pRgn, rcOr);

			return;

        case (CENTER_OVERLAY << 2) | BOTTOM_OVERLAY :

            // Change top coordinate of the "or" rectangle.
            //
            // +--------------------+
            // |          1         |
            // |                    |
            // |       +-----+      |
            // |       |     |      |
            // |       |     |      |
            // +-------+-----+------+
            //         |     |
            //         +-----+

            rcOr.top = rcRgn.bottom;
			RgnOrRect(pRgn, rcOr);

			return;

        case (CENTER_OVERLAY << 2) | CENTER_OVERLAY :

            // just exit.
            // +---------------+    
            // |       1       |
            // |   +-------+   |
            // |   |       |   |
            // |   |       |   |
            // |   |       |   |
            // |   +-------+   |
            // |               |
            // +---------------+
            //

			return;
        }   /* End of switch */
	}       /* End of for  */

	/* no overlay, just add the "or" rectangle into the clip region */
	AddRectIntoRgn(pRgn, rcOr);
}

#else   // RGNSUPPORT

static void UnusedFunc(void)
{
}

#endif // RGNSUPPORT
