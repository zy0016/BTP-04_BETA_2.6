/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for bitmap object
 *            
\**************************************************************************/

#ifndef __WSOBMP_H
#define __WSOBMP_H

// PDC的提前引用说明
typedef struct tagDC*   PDC;

typedef struct
{
    XGDIOBJHDR  xgdihdr;    /* XGDI object header */
    BITMAP      bitmap;        /* bitmap struct            */
    PPHYSBMP    pPhysBmp;      /* Physical object pointer  */
    BYTE        bUserBuf;      /* Is user buffer           */
    BYTE        bReserved1;
    BYTE        bReserved2;
    BYTE        bReserved3;
    PGRAPHDEV   pGraphDev;          /* */
} BMPOBJ, *PBMPOBJ;

PBMPOBJ  BMP_Create(const BITMAP* pBitmap, BOOL bUserBuf, BOOL bShared);
PBMPOBJ  BMP_CreateCompatible(PDC pDC, const BITMAP* pBitmap, BOOL bUserBuf, BOOL bShared);
BOOL     BMP_Destroy(PBMPOBJ pBmpObj);
PPHYSBMP BMP_CreatePhysObj(PBMPOBJ pBmpObj);
void     BMP_DestroyPhysObj(PBMPOBJ pBmpObj, PPHYSBMP pPhysBmp);
PPHYSBMP BMP_GetPhysObj(PBMPOBJ pBmpObj);
void*    BMP_GetDevice(PBMPOBJ pBmpObj);
void     BMP_ReleaseDevice(PBMPOBJ pBmpObj);
void*    BMP_GetData(PBMPOBJ pBmpObj);
void     BMP_ReleaseData(PBMPOBJ pBmpObj);
BOOL     BMP_IsCompatible(PDC pDC, PBMPOBJ pBmpObj);
LONG     BMP_SetBits(PBMPOBJ pBmpObj, DWORD cBytes, const void* pBits);
int      BMP_GetDIBits(PDC pDC, PBMPOBJ pBmpObj, UINT uStartScan, 
                       UINT cScanLines, PVOID lpvBits, PBITMAPINFO lpbi, 
                       UINT uUsage);
int      BMP_SetDIBits(PDC pDC, PBMPOBJ pBmpObj, UINT uStartScan, 
                       UINT cScanLines, CONST VOID *lpvBits, 
                       CONST BITMAPINFO *lpbmi, UINT fuColorUse);
BOOL     BMP_GetDIBITMAP(PDIBITMAP pDIBitmap, const void *lpvBits, 
                      const BITMAPINFO *lpbmi, UINT fuColorUse);


// 位图对象的选中操作函数
//#define BMP_IsSelected(pBmpObj) (pBmpObj->bSelected)
//void    BMP_Select(PBMPOBJ pBmpObj);
//void    BMP_Unselect(PBMPOBJ pBmpObj);

#endif //__WSOBMP_H
