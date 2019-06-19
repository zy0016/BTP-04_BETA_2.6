/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Pen object management module. 
 *            
\**************************************************************************/

#include "hpwin.h"

#include "string.h"

#include "wsobj.h"
#include "devdisp.h"
#include "wsodc.h"
#include "graphdev.h"
#include "wsobmp.h"

#ifdef WINDEBUG
static int nBmpMemUse;      // Current user memory used for bitmap object
static int nBmpMemMax;      // Maximum user memory used for bitmap object
#endif

#if (__MP_PLX_GUI)
#define BMP_MemAlloc(size, bShared)    \
    ((bShared) ? MemAlloc(size) : WIN_LocalAlloc(LMEM_FIXED, size))
#define BMP_MemFree(p, bShared)        \
    ((bShared) ? MemFree(p) : WIN_LocalFree(p))
#define BMP_LocalAlloc(flag, size, bShared)    \
((bShared) ? MemAlloc(size) : WIN_LocalAlloc(flag, size))
#define BMP_LocalFree(p, bShared)        \
((bShared) ? MemFree(p) : WIN_LocalFree(p))
#define BMP_LocalLock(p, bShared)         \
    ((bShared) ? (p) : WIN_LocalLock(p))
#define BMP_LocalUnlock(p, bShared)         \
    ((bShared) ? (p) : WIN_LocalUnlock(p))
#else //__MP_PLX_GUI
#define BMP_MemAlloc(size, bShared)         MemAlloc(size)
#define BMP_MemFree(p, bShared)             MemFree(p)
#define BMP_LocalAlloc(flag, size, bShared) WIN_LocalAlloc(flag, size)
#define BMP_LocalFree(p, bShared)           WIN_LocalFree(p)
#define BMP_LocalLock(p, bShared)           WIN_LocalLock(p)
#define BMP_LocalUnlock(p, bShared)         WIN_LocalUnlock(p)
#endif

/*
**  Function : BMP_Create
**  Purpose  :
**      Creates a bitmap using a specified bitmap struct.
**  Params   :
**      pBitmap : Specifies a bitmap struct using to create a bitmap 
**                object.
**  Return   :
**      If the function succeeds, return the bitmap object pointer.
**      If the function fails, return NULL.
*/
PBMPOBJ BMP_Create(const BITMAP* pBitmap, BOOL bUserBuf, BOOL bShared)
{
    return BMP_CreateCompatible(NULL, pBitmap, bUserBuf, bShared);
}

PBMPOBJ BMP_CreateCompatible(PDC pDC, const BITMAP* pBitmap, BOOL bUserBuf, BOOL bShared)
{
    PBMPOBJ pBmpObj;
    void* pBmpBuf;
    UINT uSize = 0;
    int nBitsPixel, nPlanes;
    PGRAPHDEV   pGraphDev;

    ASSERT(pBitmap != NULL);

    // 参数有效性判断

    // bmPlanes and bmBitsPixel must > 0
    if (pBitmap->bmWidth <= 0 || pBitmap->bmHeight <= 0)
        return NULL;

    // Must be mono bitmap or compatiable bitmap
    if (pDC != NULL)
    {
        pGraphDev = pDC->pGraphDev;
        nBitsPixel = DISPDEV_GetDeviceCaps(pGraphDev->pDev, BITSPIXEL);
        nPlanes = DISPDEV_GetDeviceCaps(pGraphDev->pDev, PLANES);
    }
    else
    {
        pGraphDev = GRAPHDEV_GetDefault(DEV_DISPLAY);
        nBitsPixel = DISPDEV_GetCaps(BITSPIXEL);
        nPlanes = DISPDEV_GetCaps(PLANES);
    }

    if (!(pBitmap->bmPlanes == 1 && pBitmap->bmBitsPixel == 1) && 
        !(pBitmap->bmPlanes == nPlanes && 
          pBitmap->bmBitsPixel == nBitsPixel))
          return NULL;

    // If use user buffer, bmBits mustn't be NULL
    if (bUserBuf && !pBitmap->bmBits)
        return NULL;

    // Memory allocate for the bitmap object
    pBmpObj = (PBMPOBJ)BMP_MemAlloc(sizeof(BMPOBJ), bShared);
    if (!pBmpObj)
        return NULL;

    pBmpObj->bitmap = *pBitmap;

    // Recalculates the bmType field and bmWidthBytes field
    pBmpObj->bitmap.bmType = 0;

#if (COLUMN_BITMAP)
    pBmpObj->bitmap.bmWidthBytes = 
        ((pBitmap->bmHeight * pBitmap->bmBitsPixel + 31) & ~31) >> 3;
#else
    pBmpObj->bitmap.bmWidthBytes = 
        ((pBitmap->bmWidth * pBitmap->bmBitsPixel + 31) & ~31) >> 3;
#endif

    if (!bUserBuf)
    {
        // Calculates the bitmap buffer size
#if (COLUMN_BITMAP)
        uSize = pBmpObj->bitmap.bmWidthBytes * pBitmap->bmWidth;
#else
        uSize = pBmpObj->bitmap.bmWidthBytes * pBitmap->bmHeight;
#endif

        // Allocs buffer for bitmap object, use user memory
        pBmpObj->bitmap.bmBits = (void*)BMP_LocalAlloc(LMEM_MOVEABLE, uSize, bShared);
        if (!pBmpObj->bitmap.bmBits)
        {
            BMP_MemFree(pBmpObj, bShared);
            return NULL;
        }
    }

    // Adds the bitmap object to object table
    if (!WOT_RegisterObj((PWSOBJ)pBmpObj, OBJ_BITMAP, bShared))
    {
        if (!bUserBuf)
            BMP_LocalFree((HLOCAL)pBmpObj->bitmap.bmBits, bShared);

        BMP_MemFree(pBmpObj, bShared);
        return NULL;
    }

    pBmpObj->pPhysBmp = NULL;
    pBmpObj->bUserBuf = bUserBuf;
    pBmpObj->pGraphDev = pGraphDev;

    // If the bitmap is a compatiable bitmap, creates the physical bitmap
    if (pBitmap->bmPlanes == nPlanes && pBitmap->bmBitsPixel == nBitsPixel)
    {
        pBmpObj->pPhysBmp = BMP_CreatePhysObj(pBmpObj);
        if (!pBmpObj->pPhysBmp)
        {
            // Deletes from handle table
            WOT_UnregisterObj((PWSOBJ)pBmpObj);

            // Frees the buffer if not user buffer
            if (!bUserBuf)
                BMP_LocalFree((HLOCAL)pBmpObj->bitmap.bmBits, bShared);

            BMP_MemFree(pBmpObj, bShared);
            return NULL;
        }
    }
    else
    {
        // Processes the mono bitmap
        pBmpObj->pPhysBmp = NULL;
    }

    if (!bUserBuf)
    {
        // 如果指定了初始化数据，使用初始化数据初始化位图数据区；如果没有指
        // 定初始化数据，并不清除位图数据区的内容为0，因为清除操作是耗时操
        // 作，会导致位图的生成很慢。
        if (pBitmap->bmBits)
        {
            pBmpBuf = (void*)BMP_LocalLock((HLOCAL)(pBmpObj->bitmap.bmBits), bShared);
            memcpy(pBmpBuf, pBitmap->bmBits, uSize);
            BMP_LocalUnlock((HLOCAL)(pBmpObj->bitmap.bmBits), bShared);
        }
        
#ifdef WINDEBUG
        
        // 统计位图对象使用用户内存的数量
        nBmpMemUse += uSize;
        
        if (nBmpMemUse > nBmpMemMax)
            nBmpMemMax = nBmpMemUse;
        
#endif
    }

    return pBmpObj;
}

/*
**  Function : BMP_Destroy
**  Purpose  :
**      Destroys a specified bitmap.
**  Params   :
**      pBmpObj : Specifies the bitmap to be destroyed.
*/
BOOL BMP_Destroy(PBMPOBJ pBmpObj)
{
    ASSERT(pBmpObj != NULL);

    // Can't destroy a stock object
    if (WOT_IsStockObj((PXGDIOBJ)pBmpObj))
        return FALSE;

    ASSERT(ISDELETEDXGDIOBJ(pBmpObj));

/*
    if (pBmpObj->bSelected)
    {
        // 位图处于选中状态, 不能立即删除位图, 设置位图删除标志, 位图的
        // 删除操作延迟到从DC中选出时进行
        pBmpObj->bDeleted = TRUE;
        return TRUE;
    }
*/

    WOT_UnregisterObj((PWSOBJ)pBmpObj);

    // Destroys the physical bitmap for bitmap object
    if (pBmpObj->pPhysBmp)
        BMP_DestroyPhysObj(pBmpObj, pBmpObj->pPhysBmp);
    
    // If not user buffer, free the buffer
    if (!pBmpObj->bUserBuf)
    {
        BMP_LocalFree((HLOCAL)pBmpObj->bitmap.bmBits, WOT_IsSharedObj(pBmpObj));
        
#ifdef WINDEBUG
#if (COLUMN_BITMAP)
        nBmpMemUse -= pBmpObj->bitmap.bmWidthBytes * 
            pBmpObj->bitmap.bmWidth * pBmpObj->bitmap.bmPlanes;
#else // COLUMN_BITMAP
        nBmpMemUse -= pBmpObj->bitmap.bmWidthBytes * 
            pBmpObj->bitmap.bmHeight * pBmpObj->bitmap.bmPlanes;
#endif // COLUMN_BITMAP
#endif
    }

    BMP_MemFree(pBmpObj, WOT_IsSharedObj(pBmpObj));

    return TRUE;
}

/*
**  Function : BMP_CreatePhysObj
**  Purpose  :
**     Creates the physical object of the specfied bitmap object. 
*/
PPHYSBMP BMP_CreatePhysObj(PBMPOBJ pBmpObj)
{
    int nRet;
    PGRAPHDEV pGraphDev;

    ASSERT (pBmpObj != NULL);
    ASSERT(pBmpObj->pGraphDev != NULL);

    pGraphDev = pBmpObj->pGraphDev;
    
    // Creates the physical bitmap of the bitmap object
    
    nRet = pGraphDev->drv.CreateBitmap(pGraphDev->pDev, 
        pBmpObj->bitmap.bmWidth, pBmpObj->bitmap.bmHeight, NULL);
    if (nRet <= 0)
        return NULL;
    
    pBmpObj->pPhysBmp = MemAlloc(nRet);
    if (!pBmpObj->pPhysBmp)
        return NULL;
    
    nRet = pGraphDev->drv.CreateBitmap(pGraphDev->pDev, 
        pBmpObj->bitmap.bmWidth, pBmpObj->bitmap.bmHeight, 
        pBmpObj->pPhysBmp);
    ASSERT(nRet > 0);

    if (pBmpObj->bUserBuf)
    {
        pGraphDev->drv.SetBitmapBuffer(pGraphDev->pDev, 
            pBmpObj->pPhysBmp, pBmpObj->bitmap.bmBits);
    }
    
    return pBmpObj->pPhysBmp;
}


/*
**  Function : BMP_DestroyPhysObj
**  Purpose  :
**      Destroys a specifed physical bitmap;
*/
void BMP_DestroyPhysObj(PBMPOBJ pBmpObj, PPHYSBMP pPhysBmp)
{
    ASSERT(pBmpObj != NULL);
    ASSERT(pBmpObj->pPhysBmp == pPhysBmp);
    ASSERT(pBmpObj->pGraphDev != NULL);

    pBmpObj->pGraphDev->drv.DestroyBitmap(pBmpObj->pGraphDev->pDev, 
        pBmpObj->pPhysBmp);
    MemFree(pBmpObj->pPhysBmp);
    pBmpObj->pPhysBmp = NULL;
}

/*
**  Function : BMP_GetPhysObj
**  Purpose  :
**     Gets the physical object of the specfied bitmap object. 
*/
PPHYSBMP BMP_GetPhysObj(PBMPOBJ pBmpObj)
{
    ASSERT(pBmpObj != NULL);
    return pBmpObj->pPhysBmp;
}

/*
**  Function : BMP_GetDevice
**  Purpose  :
**      
*/
void* BMP_GetDevice(PBMPOBJ pBmpObj)
{
    void* pBmpBuf;

    ASSERT(pBmpObj != NULL);
    ASSERT(pBmpObj->pGraphDev != NULL);
    
    // 没有相应物理对象直接返回NULL(单色位图)
    if (!pBmpObj->pPhysBmp)
        return NULL;

    // Lock the bitmap buffer and set physical bitmap buffer
    if (!pBmpObj->bUserBuf)
    {
        pBmpBuf = BMP_LocalLock(pBmpObj->bitmap.bmBits, WOT_IsSharedObj(pBmpObj));
        pBmpObj->pGraphDev->drv.SetBitmapBuffer(pBmpObj->pGraphDev->pDev, 
            pBmpObj->pPhysBmp, pBmpBuf);
    }

    return pBmpObj->pPhysBmp;
}

/*
**  Function : BMP_ReleaseDevice
**  Purpose  :
*/
void BMP_ReleaseDevice(PBMPOBJ pBmpObj)
{
    ASSERT(pBmpObj != NULL);
    ASSERT(pBmpObj->pGraphDev != NULL);

    // 没有相应物理对象直接返回(单色位图)
    if (!pBmpObj->pPhysBmp)
        return;

    // Unlock the data memory of bitmap object, and set the physical 
    // bitmap buffer to NULL
    if (!pBmpObj->bUserBuf)
    {
        BMP_LocalUnlock(pBmpObj->bitmap.bmBits, WOT_IsSharedObj(pBmpObj));
        pBmpObj->pGraphDev->drv.SetBitmapBuffer(pBmpObj->pGraphDev->pDev, 
            pBmpObj->pPhysBmp, NULL);
    }
}

/*
**  Function : BMP_GetData
**  Purpose  :
**      
*/
void* BMP_GetData(PBMPOBJ pBmpObj)
{
    ASSERT(pBmpObj != NULL);

    if (pBmpObj->bUserBuf)
        return pBmpObj->bitmap.bmBits;

    return BMP_LocalLock(pBmpObj->bitmap.bmBits, WOT_IsSharedObj(pBmpObj));
}

/*
**  Function : BMP_ReleaseData
*/
void BMP_ReleaseData(PBMPOBJ pBmpObj)
{
    ASSERT(pBmpObj != NULL);

    if (!pBmpObj->bUserBuf)
        BMP_LocalUnlock(pBmpObj->bitmap.bmBits, WOT_IsSharedObj(pBmpObj));
}

/*
**  Function : BMP_IsCompatible
**  Purpose  :
**      Retrieves whether a specified bitmap object is compatible with
**      display device.
*/
BOOL BMP_IsCompatible(PDC pDC, PBMPOBJ pBmpObj)
{
    int nBitsPixel, nPlanes;

    ASSERT(pBmpObj != NULL);

    if (pDC != NULL)
    {
        nBitsPixel = DISPDEV_GetDeviceCaps(pDC->pGraphDev->pDev, BITSPIXEL);
        nPlanes = DISPDEV_GetDeviceCaps(pDC->pGraphDev->pDev, PLANES);
    }
    else
    {
        nBitsPixel = DISPDEV_GetCaps(BITSPIXEL);
        nPlanes = DISPDEV_GetCaps(PLANES);
    }

    return (pBmpObj->pPhysBmp && 
        pBmpObj->bitmap.bmPlanes == nPlanes && 
        pBmpObj->bitmap.bmBitsPixel == nBitsPixel);
}

/*
**  Function : BMP_SetBits
**  Purpose  :
**      sets the bits of color data for a bitmap to the specified values.
*/
LONG BMP_SetBits(PBMPOBJ pBmpObj, DWORD cBytes, const void* pBits)
{
    DWORD dwSize;
    void* pBmpBuf;

    ASSERT(pBmpObj != NULL);
    ASSERT(pBits != NULL);

    if (pBmpObj->bUserBuf)
    {
        pBmpObj->bitmap.bmBits = (void*)pBits;

#if (COLUMN_BITMAP)
        return pBmpObj->bitmap.bmWidthBytes * pBmpObj->bitmap.bmWidth * 
            pBmpObj->bitmap.bmPlanes;
#else // COLUMN_BITMAP
        return pBmpObj->bitmap.bmWidthBytes * pBmpObj->bitmap.bmHeight * 
            pBmpObj->bitmap.bmPlanes;
#endif // COLUMN_BITMAP
    }

#if (COLUMN_BITMAP)
    dwSize = pBmpObj->bitmap.bmWidthBytes * pBmpObj->bitmap.bmWidth * 
        pBmpObj->bitmap.bmPlanes;
#else // COLUMN_BITMAP
    dwSize = pBmpObj->bitmap.bmWidthBytes * pBmpObj->bitmap.bmHeight * 
        pBmpObj->bitmap.bmPlanes;
#endif // COLUMN_BITMAP
    
    if (cBytes > dwSize)
        cBytes = dwSize;
    
    pBmpBuf = BMP_LocalLock(pBmpObj->bitmap.bmBits, WOT_IsSharedObj(pBmpObj));
    memcpy(pBmpBuf, pBits, cBytes);
    BMP_LocalUnlock(pBmpObj->bitmap.bmBits, WOT_IsSharedObj(pBmpObj));

    return cBytes;
}

/*
**  Function : BMP_GetDIBits
**  Purpose  :
**      Retrieves the bits of the specified bitmap and copies them into a 
**      buffer using the specified format. 
*/
typedef unsigned char* PBMPDATA;
typedef unsigned char* PBMPBYTE;
typedef unsigned short* PBMPWORD;

typedef struct _tagTransferBits
{
    PBMPDATA    pd;
    PBMPDATA    ps;
    LONG        dpixelbits;
    LONG        spixelbits;
    LONG        dlinebytes;
    LONG        slinebytes;
    LONG        copypixels;
    LONG        copylines;
}TRANSFERBITS, *PTRANSFERBITS;

static void TransferBits(PTRANSFERBITS p);

int BMP_GetDIBits(PDC pDC, PBMPOBJ pBmpObj, UINT uStartScan, 
                  UINT cScanLines, PVOID pvBits, PBITMAPINFO pbmi, 
                  UINT uUsage)
{
    int     nScanLines;
    int     i;
    BYTE    bColor;
    PBMPDATA   pDIBData;
    PBMPDATA   pBmpData;
    int     nLineBytes;
    int     nLinePixels, nCopyPixels;
    TRANSFERBITS Transfer;

    ASSERT(pBmpObj != NULL);
    ASSERT(pbmi != NULL);

    if (pbmi->bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
    {
        SetLastError(1);
        return 0;
    }

#if (COLUMN_BITMAP)

    if (uStartScan > (UINT)(pBmpObj->bitmap.bmWidth - 1))
        nScanLines = 0;
    else
    {
        if (uStartScan + cScanLines > (UINT)(pBmpObj->bitmap.bmWidth))
            nScanLines = pBmpObj->bitmap.bmWidth - uStartScan;
        else
            nScanLines = cScanLines;
    }

#else // COLUMN_BITMAP

    if (uStartScan > (UINT)(pBmpObj->bitmap.bmHeight - 1))
        nScanLines = 0;
    else
    {
        if (uStartScan + cScanLines > (UINT)(pBmpObj->bitmap.bmHeight))
            nScanLines = pBmpObj->bitmap.bmHeight - uStartScan;
        else
            nScanLines = cScanLines;
    }

#endif // COLUMN_BITMAP

    // If pvBits is NULL, passes the dimensions and format of the bitmap
    // to the BITMAPINFO structure pointed to by the pbmi parameter. 
    if (!pvBits)
    {
        pbmi->bmiHeader.biPlanes = 1;
        pbmi->bmiHeader.biXPelsPerMeter = 0; 
        pbmi->bmiHeader.biYPelsPerMeter = 0; 
        pbmi->bmiHeader.biClrUsed = 0; 
        pbmi->bmiHeader.biClrImportant = 0; 

        if (pbmi->bmiHeader.biCompression != BI_BITFIELDS)
            pbmi->bmiHeader.biCompression = BI_RGB;

        // 如果pbmi中指定的biBitCount不为0, 表示要填充pbmi中的颜色表( 如果
        // 需要的话).如果pbmi中指定的biBitCount为0, 则不需要填充pbmi中的颜
        // 色表.
        if (pbmi->bmiHeader.biBitCount != 0)
        {
//            pbmi->bmiHeader.biBitCount = pBmpObj->bitmap.bmBitsPixel;

            switch (pbmi->bmiHeader.biBitCount)
            {
            case 1 :    // mono color

                pbmi->bmiColors[0].rgbBlue = 0;
                pbmi->bmiColors[0].rgbGreen = 0;
                pbmi->bmiColors[0].rgbRed = 0;
                pbmi->bmiColors[0].rgbReserved = 0;
                pbmi->bmiColors[1].rgbBlue = 255;
                pbmi->bmiColors[1].rgbGreen = 255;
                pbmi->bmiColors[1].rgbRed = 255;
                pbmi->bmiColors[1].rgbReserved = 0;

                break;

            case 2 : 

                bColor = 0;
                for (i=0; i<4; i++)
                {
                    if (i == 0)
                        bColor = 0;
                    else if (i == 1)
                        bColor = 128;
                    else if (i == 2)
                        bColor = 192;
                    else
                        bColor = 255;

                    pbmi->bmiColors[i].rgbBlue = bColor;
                    pbmi->bmiColors[i].rgbGreen = bColor;
                    pbmi->bmiColors[i].rgbRed = bColor;
                    pbmi->bmiColors[i].rgbReserved = 0;
                }

                break;

            case 4 :

                break;

            case 8 :

                break;

            case 16 :

                if (pbmi->bmiHeader.biCompression == BI_BITFIELDS)
                {
                    *(DWORD*)(&pbmi->bmiColors[0]) = 0x0000F800L;
                    *(DWORD*)(&pbmi->bmiColors[1]) = 0x000007E0L;
                    *(DWORD*)(&pbmi->bmiColors[2]) = 0x0000001FL;
                }

                break;

            case 24 :

                break;

            default : 

                ASSERT(0);
                break;
            }
        }
        else
        {
            pbmi->bmiHeader.biBitCount = pBmpObj->bitmap.bmBitsPixel;
        }

#if (COLUMN_BITMAP)

        pbmi->bmiHeader.biWidth = nScanLines;
        pbmi->bmiHeader.biHeight = pBmpObj->bitmap.bmHeight;
        pbmi->bmiHeader.biSizeImage = (pBmpObj->bitmap.bmHeight * 
            pbmi->bmiHeader.biBitCount + 31) / 32 * 4 * nScanLines;
        pbmi->bmiHeader.biCompression |= BI_COLUMN;

#else // COLUMN_BITMAP

        pbmi->bmiHeader.biWidth = pBmpObj->bitmap.bmWidth;
        pbmi->bmiHeader.biHeight = nScanLines;
        pbmi->bmiHeader.biSizeImage = (pBmpObj->bitmap.bmWidth * 
            pbmi->bmiHeader.biBitCount + 31) / 32 * 4 * nScanLines;
#endif // COLUMN_BITMAP

        return nScanLines;
    }

    if (nScanLines == 0)
        return 0;

    if (pbmi->bmiHeader.biWidth <= 0 || pbmi->bmiHeader.biHeight == 0)
    {
        SetLastError(1);
        return 0;
    }

    // Calculates the line bytes of the DIB
#if (COLUMN_BITMAP)
    if (pbmi->bmiHeader.biHeight > 0)
    {
        nLinePixels = pbmi->bmiHeader.biHeight;
        nLineBytes = (pbmi->bmiHeader.biBitCount * 
            pbmi->bmiHeader.biHeight + 31) / 32 * 4;
    }
    else
    {
        nLinePixels = -pbmi->bmiHeader.biHeight;
        nLineBytes = (pbmi->bmiHeader.biBitCount * 
            (-pbmi->bmiHeader.biHeight) + 31) / 32 * 4;
    }
#else // COLUMN_BITMAP
    nLinePixels = pbmi->bmiHeader.biWidth;
    nLineBytes = (pbmi->bmiHeader.biBitCount * pbmi->bmiHeader.biWidth
        + 31) / 32 * 4;
#endif  // COLUMN_BITMAP

    if (nLinePixels <= pBmpObj->bitmap.bmWidth)
        nCopyPixels = nLinePixels;
    else
        nCopyPixels = pBmpObj->bitmap.bmWidth;

//    if (nLineBytes <= pBmpObj->bitmap.bmWidthBytes)
//        nCopyBytes = nLineBytes;
//    else
//        nCopyBytes = pBmpObj->bitmap.bmWidthBytes;

    // copy the bitmap data.

    if (pBmpObj->bUserBuf)
        pBmpData = pBmpObj->bitmap.bmBits;
    else
        pBmpData = (PBMPDATA)BMP_LocalLock(pBmpObj->bitmap.bmBits, WOT_IsSharedObj(pBmpObj));

    pBmpData += pBmpObj->bitmap.bmWidthBytes * uStartScan;
    
    if (pbmi->bmiHeader.biHeight > 0)   // bottom-up DIB 
    {
#if (COLUMN_BITMAP)

        if (nScanLines > pbmi->bmiHeader.biWidth)
            nScanLines = pbmi->bmiHeader.biWidth;

        pDIBData = (PBMPDATA)pvBits + nLineBytes * (nScanLines - 1);
            //(pbmi->bmiHeader.biWidth - 1);

#else // COLUMN_BITMAP

        if (nScanLines > pbmi->bmiHeader.biHeight)
            nScanLines = pbmi->bmiHeader.biHeight;

        pDIBData = (PBMPDATA)pvBits + nLineBytes * (nScanLines - 1);
            //(pbmi->bmiHeader.biHeight - 1);

#endif // COLUMN_BITMAP

        nLineBytes = -nLineBytes;
    }
    else                                // top-down DIB 
    {
#if (COLUMN_BITMAP)

        if (nScanLines > pbmi->bmiHeader.biWidth)
            nScanLines = pbmi->bmiHeader.biWidth;

#else // COLUMN_BITMAP

        if (nScanLines > -pbmi->bmiHeader.biHeight)
            nScanLines = -pbmi->bmiHeader.biHeight;

#endif // COLUMN_BITMAP

        pDIBData = (PBMPDATA)pvBits;
    }

    Transfer.ps = pBmpData;
    Transfer.pd = pDIBData;
    Transfer.slinebytes = pBmpObj->bitmap.bmWidthBytes;
    Transfer.dlinebytes = nLineBytes;
    Transfer.spixelbits = pBmpObj->bitmap.bmBitsPixel;
    Transfer.dpixelbits = pbmi->bmiHeader.biBitCount;
    Transfer.copypixels = nCopyPixels;
    Transfer.copylines  = nScanLines;

    TransferBits(&Transfer);
/*    for (i = 0; i < nScanLines; i++)
    {
        memcpy(pDIBData, pBmpData, nCopyBytes);

        pDIBData += nLineBytes;
        pBmpData += pBmpObj->bitmap.bmWidthBytes;
    }
*/
    if (!pBmpObj->bUserBuf)
        BMP_LocalUnlock(pBmpObj->bitmap.bmBits, WOT_IsSharedObj(pBmpObj));

    return nScanLines;
}

/*
**  Function : BMP_SetDIBits
**  Purpsoe  :
**      Sets the pixels in a bitmap using the color data found in the 
**      specified device-independent bitmap (DIB). 
*/
int BMP_SetDIBits(PDC pDC, PBMPOBJ pBmpObj, UINT uStartScan, 
                  UINT cScanLines, CONST VOID *pvBits, 
                  CONST BITMAPINFO *lpbmi, UINT fuColorUse)
{
    DIBITMAP    diBitmap;
    int         nHeight;
    GRAPHPARAM  graph_param;
    RECT        rcDest;

    ASSERT(pBmpObj != NULL);
    ASSERT(pvBits != NULL);
    ASSERT(lpbmi != NULL);

    if (lpbmi->bmiHeader.biHeight == 0 || lpbmi->bmiHeader.biWidth <= 0)
    {
        SetLastError(1);
        return 0;
    }

    // Support compatible bitmap only
    if (!BMP_IsCompatible(pDC, pBmpObj))
    {
        ASSERT(0);
        return 0;
    }

    if (!BMP_GetDIBITMAP(&diBitmap, pvBits, lpbmi, fuColorUse))
        return 0;

    nHeight = (lpbmi->bmiHeader.biHeight > 0) ? 
        (lpbmi->bmiHeader.biHeight) : -(lpbmi->bmiHeader.biHeight);

#if (COLUMN_BITMAP)

    if (lpbmi->bmiHeader.biCompression & BI_COLUMN)
    {
        if (uStartScan > (UINT)(lpbmi->bmiHeader.biWidth - 1))
        {
            SetLastError(1);
            return 0;
        }
        
        if (lpbmi->bmiHeader.biHeight > 0)
            diBitmap.width = (int16)(uStartScan + cScanLines);
        else
            diBitmap.width = -(int16)(uStartScan + cScanLines);

        // Calculates the dest rectangle
        rcDest.left = diBitmap.width - uStartScan - cScanLines;
        rcDest.top = 0;
        rcDest.right = diBitmap.width - uStartScan;
        rcDest.bottom = rcDest.top + nHeight;
    }
    else
    {
        if (uStartScan > (UINT)(nHeight - 1))
        {
            SetLastError(1);
            return 0;
        }
        
        if (lpbmi->bmiHeader.biHeight > 0)
            diBitmap.height = (int16)(uStartScan + cScanLines);
        else
            diBitmap.height = -(int16)(uStartScan + cScanLines);
        
        // Calculates the dest rectangle
        rcDest.left = 0;
        rcDest.top = nHeight - uStartScan - cScanLines;
        rcDest.right = rcDest.left + diBitmap.width;
        rcDest.bottom = nHeight - uStartScan;
    }
#else  // COLUMN_BITMAP

    if (uStartScan > (UINT)(nHeight - 1))
    {
        SetLastError(1);
        return 0;
    }

    if (lpbmi->bmiHeader.biHeight > 0)
        diBitmap.height = (int16)(uStartScan + cScanLines);
    else
        diBitmap.height = -(int16)(uStartScan + cScanLines);

    // Calculates the dest rectangle
    rcDest.left = 0;
    rcDest.top = nHeight - uStartScan - cScanLines;
    rcDest.right = rcDest.left + diBitmap.width;
    rcDest.bottom = nHeight - uStartScan;

#endif // COLUMN_BITMAP

    // Fills the graph parameter struct
    graph_param.flags = 0;
    graph_param.bk_mode = BM_OPAQUE;
    graph_param.bk_color = 0;
    graph_param.text_color = 0;
    graph_param.pPen = NULL;
    graph_param.pBrush = NULL;
    graph_param.brush_org_x = 0;
    graph_param.brush_org_y = 0;
    graph_param.rop = ROP_SRC;
    graph_param.clip_rect.left = 0;
    graph_param.clip_rect.top = 0;
    graph_param.clip_rect.right = 0;
    graph_param.clip_rect.bottom = 0;
    graph_param.pRegion = NULL;

    pBmpObj->pGraphDev->drv.DIBitBlt(BMP_GetDevice(pBmpObj), &rcDest, 
        &diBitmap, 0, uStartScan, &graph_param);
    BMP_ReleaseDevice(pBmpObj);
  
    return (int)cScanLines;
}

/*********************************************************************\
* Function	   BMP_GetDIBITMAP
* Purpose      通过DIB参数，填充内部DIBITMAP结构。
* Params	   
* Return	 	   
    成功返回TRUE；失败返回FALSE
* Remarks	   
**********************************************************************/
BOOL BMP_GetDIBITMAP(PDIBITMAP pDIBitmap, const void *lpvBits, 
                      const BITMAPINFO *lpbmi, UINT fuColorUse)
{
    int nHeight;

    ASSERT(pDIBitmap != NULL);
    ASSERT(lpvBits != NULL);
    ASSERT(lpbmi != NULL);
    ASSERT(lpbmi->bmiHeader.biWidth > 0);
    ASSERT(lpbmi->bmiHeader.biHeight != 0);

    switch (lpbmi->bmiHeader.biBitCount)
    {
    case 1 :

        pDIBitmap->type = BMP_MONO;
        break;

    case 2 :

        pDIBitmap->type = BMP_GRAY4;
        break;

    case 4 :

        pDIBitmap->type = BMP_16;
        break;

    case 8 :

        pDIBitmap->type = BMP_256;
        break;

    case 16 :
        pDIBitmap->type = BMP_RGB16;
        break;
        
    case 24 :
        pDIBitmap->type = BMP_RGB24;
        break;

    case 32 :

        pDIBitmap->type = BMP_RGB32;
        break;

    default :

        SetLastError(1);
        return FALSE;
    }

    nHeight = (lpbmi->bmiHeader.biHeight > 0) ? 
        (lpbmi->bmiHeader.biHeight) :-(lpbmi->bmiHeader.biHeight);

	pDIBitmap->width = (int16)(lpbmi->bmiHeader.biWidth);
	pDIBitmap->height = (int16)(lpbmi->bmiHeader.biHeight);
    
	if (lpbmi->bmiHeader.biBitCount <= 8)
    {
        pDIBitmap->palette_entries = 1 << (lpbmi->bmiHeader.biBitCount);
        pDIBitmap->pPalette = (void *)(lpbmi->bmiColors);
    }
    else if (lpbmi->bmiHeader.biCompression == BI_BITFIELDS)
    {
        pDIBitmap->palette_entries = 1;
        pDIBitmap->pPalette = (void *)(lpbmi->bmiColors);
    }
    else
    {
        pDIBitmap->palette_entries = 0;
        pDIBitmap->pPalette = NULL;
    }

#if (COLUMN_BITMAP)
    if (lpbmi->bmiHeader.biCompression & BI_COLUMN)
    {
        pDIBitmap->bmp_size = lpbmi->bmiHeader.biWidth * 
            ((nHeight * lpbmi->bmiHeader.biBitCount + 31) 
            / 32 * 4);
    }
    else
    {
        pDIBitmap->bmp_size = nHeight * ((lpbmi->bmiHeader.biWidth * 
            lpbmi->bmiHeader.biBitCount + 31) / 32 * 4);

        // 指定位图是正常位图，不是列位图
        pDIBitmap->type |= 0x8000;
    }
#else  // COLUMN_BITMAP
    pDIBitmap->bmp_size = nHeight * ((lpbmi->bmiHeader.biWidth * 
        lpbmi->bmiHeader.biBitCount + 31) / 32 * 4);
#endif // COLUMN_BITMAP

    pDIBitmap->pBuffer = (BYTE *)lpvBits;

    return TRUE;
}

#define RGB565      1
#define RGB4440     0
static void Transfer16To24(PTRANSFERBITS p)
{
    PBMPDATA pdst, psrc;
    int i, j, nScanLines, nCopyPixels;
    PBMPWORD p1;
    PBMPBYTE p2;

    pdst = p->pd;
    psrc = p->ps;
    nScanLines = p->copylines;
    nCopyPixels = p->copypixels;

    for (i = 0; i < nScanLines; i++)
    {
        p1 = (PBMPWORD)psrc;
        p2 = pdst;
        for (j = 0; j < nCopyPixels; j++)
        {
#if (RGB4440)
            *p2++ = ((*p1 & 0x00F0) <<  4) * 17;
            *p2++ = ((*p1 & 0x0F00) >>  8) * 17;
            *p2++ = ((*p1 & 0xF000) >> 12) * 17;
#else
            *p2++ = ((*p1 & 0x001F) << 3) | ((*p1 & 0x001F) >> 3);
            *p2++ = ((*p1 & 0x07E0) >> 3) | ((*p1 & 0x07E0) >> 9);
            *p2++ = ((*p1 & 0xF800) >> 8) | ((*p1 & 0xF800) >> 13);
#endif 
            p1++;
        }
    
        pdst += p->dlinebytes;
        psrc += p->slinebytes;
    }
}

static void Transfer32To24(PTRANSFERBITS p)
{
    PBMPDATA pdst, psrc;
    int i, j, nScanLines, nCopyPixels;
    PBMPBYTE p1, p2;

    pdst = p->pd;
    psrc = p->ps;
    nScanLines = p->copylines;
    nCopyPixels = p->copypixels;

    for (i = 0; i < nScanLines; i++)
    {
        p1 = psrc;
        p2 = pdst;
        for (j = 0; j < nCopyPixels; j++)
        {
            *p2++ = *p1++;
            *p2++ = *p1++;
            *p2++ = *p1++;
            p1++;
        }
    
        pdst += p->dlinebytes;
        psrc += p->slinebytes;
    }
}

static void Transfer24To16(PTRANSFERBITS p)
{
    PBMPDATA pdst, psrc;
    int i, j, nScanLines, nCopyPixels;
    PBMPBYTE p1;
    PBMPWORD p2;

    pdst = p->pd;
    psrc = p->ps;
    nScanLines = p->copylines;
    nCopyPixels = p->copypixels;

    for (i = 0; i < nScanLines; i++)
    {
        p1 = psrc;
        p2 = (PBMPWORD)pdst;
        for (j = 0; j < nCopyPixels; j++)
        {
            *p2++ = (((*(p1  )) & 0xf8) >> 3) 
                  | (((*(p1+1)) & 0xfc) << 3) 
                  | (((*(p1+2)) & 0xf8) << 8);
            p1 += 3;
        }
    
        pdst += p->dlinebytes;
        psrc += p->slinebytes;
    }
}

static void Transfer32To16(PTRANSFERBITS p)
{
    PBMPDATA pdst, psrc;
    int i, j, nScanLines, nCopyPixels;
    PBMPBYTE p1;
    PBMPWORD p2;

    pdst = p->pd;
    psrc = p->ps;
    nScanLines = p->copylines;
    nCopyPixels = p->copypixels;

    for (i = 0; i < nScanLines; i++)
    {
        p1 = psrc;
        p2 = (PBMPWORD)pdst;
        for (j = 0; j < nCopyPixels; j++)
        {
            *p2++ = (((*(p1  )) & 0xf8) >> 3) 
                  | (((*(p1+1)) & 0xfc) << 3) 
                  | (((*(p1+2)) & 0xf8) << 8);
            p1 += 4;
        }
    
        pdst += p->dlinebytes;
        psrc += p->slinebytes;
    }
}

static void Transfer24To8(PTRANSFERBITS p)
{
    return;
}

static void Transfer16To8(PTRANSFERBITS p)
{
    return;
}

static void Transfer32To8(PTRANSFERBITS p)
{
    return;
}

static void TransferSame(PTRANSFERBITS p)
{
    PBMPDATA pdst, psrc;
    int i, nScanLines;
    int nCopyBytes;

    pdst = p->pd;
    psrc = p->ps;
    nScanLines = p->copylines;
    nCopyBytes = (p->copypixels * p->dpixelbits + 31) / 32 * 4;

    for (i = 0; i < nScanLines; i++)
    {
        memcpy(pdst, psrc, nCopyBytes);
        
        pdst += p->dlinebytes;
        psrc += p->slinebytes;
    }
}

static void TransferBits(PTRANSFERBITS p)
{
    if (p->spixelbits == p->dpixelbits)
    {
        TransferSame(p);
        return;
    }
    
    if (p->dpixelbits == 24)
    {
        if (p->spixelbits == 16)
        {
            Transfer16To24(p);
        }
        else if (p->spixelbits == 32)
        {
            Transfer32To24(p);
        }

        return;
    }
    
    if (p->dpixelbits == 16)
    {
        if (p->spixelbits == 24)
        {
            Transfer24To16(p);
        }
        else if (p->spixelbits == 32)
        {
            Transfer32To16(p);
        }

        return;
    }

    if (p->dpixelbits == 8)
    {
        if (p->spixelbits == 16)
        {
            Transfer16To8(p);
        }
        if (p->spixelbits == 24)
        {
            Transfer24To8(p);
        }
        else if (p->spixelbits == 32)
        {
            Transfer32To8(p);
        }
    }
}
