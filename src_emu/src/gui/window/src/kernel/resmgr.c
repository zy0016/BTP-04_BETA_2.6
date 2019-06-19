/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements resource support functions.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "hpfile.h"
#include "string.h"
#include "wsoicon.h"

// Two macro used in LoadImage function
#define GET16(p)(WORD)((((WORD)*((BYTE*)(p) + 1)) << 8) + *((BYTE*)(p)))
#define GET32(p)(DWORD)((((DWORD)*((BYTE*)(p) + 3)) << 24) + \
                        (((DWORD)*((BYTE*)(p) + 2)) << 16)  + \
                        (((DWORD)*((BYTE*)(p) + 1)) << 8) +  *((BYTE*)(p)))

#define swapword(p) ((p << 8) | (p >> 8))
// 定义位图文件中一些字段的偏移量
#define OFFSET_TYPE             0       // 位图标识字段偏移量
#define OFFSET_OFFBITS          10      // 位图数据偏移字段偏移量
#define OFFSET_BITMAPINFOHEADER 14      // 位图头信息字段偏移量

typedef struct tagBITMAPINFO256
{
    BITMAPINFOHEADER bmiHeader; 
    RGBQUAD          bmiColors[256]; 
} BITMAPINFO256;

static HBITMAP CreateBitmapFromDIBData(HDC hdc, const void* pDIBData);

/*
**  Function : LoadImage
**  Purpose  :
**      Loads an icon, cursor, or bitmap. 
*/
HANDLE LoadImage(HINSTANCE hInst, LPCSTR lpszName, UINT uType, 
                 int cxDesired, int cyDesired, UINT fuLoad)
{
    return LoadImageEx(NULL, hInst, lpszName, uType, 
        cxDesired, cyDesired, fuLoad);
}
/*
**  Function : LoadImageEx
**  Purpose  :
**      Loads an icon, cursor, or bitmap. 
*/
HANDLE LoadImageEx(HDC hdc, HINSTANCE hInst, LPCSTR lpszName, UINT uType, 
                 int cxDesired, int cyDesired, UINT fuLoad)
{
    BYTE *pMapFile, *pData;
    char achFileName[128];
    int hFile;
    int nFileSize;
    int nIconCount;
    int nImageOffset;
    BITMAPINFO* pbmi;
    WORD  cClrBits;
    BITMAPINFOHEADER bmih;
    HBITMAP hbmpXor, hbmpAnd;
    int i;
    PICONOBJ pIconObj;
    DWORD   dwFileSize;
    BYTE*   pFileData;
    HBITMAP hBitmap;
    
    if (!lpszName)
    {
        SetLastError(1);
        return NULL;
    }
    
    if (uType == IMAGE_BITMAP && fuLoad == LR_LOADFROMFILE)
    {
        // Open the bitmap file.
        hFile = PLXOS_CreateFile(lpszName, PLXFS_ACCESS_READ, 0);
        if (hFile == -1)
        {
            SetLastError(1);
            return NULL;
        }
        
        // 尝试映射位图文件
        dwFileSize = PLXOS_GetFileSize(hFile);
        pMapFile = (void*)PLXOS_MapFile(hFile, 0, dwFileSize);
        
        // 无法映射文件时则需要将文件内容读到内存中
        if (pMapFile)
            pFileData = (BYTE*)pMapFile;
        else
        {
            pFileData = (BYTE*)LocalAlloc(LMEM_FIXED, dwFileSize);
            if (!pFileData)
            {
                PLXOS_CloseFile(hFile);
                
                SetLastError(1);
                return NULL;
            }
            
            PLXOS_ReadFile(hFile, pFileData, dwFileSize);
        }
        
        hBitmap = CreateBitmapFromDIBData(hdc, pFileData);
        
        if (!pMapFile)
            LocalFree((HLOCAL)pFileData);
        
        if (pMapFile)
            PLXOS_UnmapFile(hFile, pMapFile, dwFileSize);
        PLXOS_CloseFile(hFile);
        
        return (HANDLE)hBitmap;
    }
    else if (uType == IMAGE_ICON && fuLoad == LR_LOADFROMFILE)
    {
        strcpy(achFileName, lpszName);
        
        // open the bitmap file.
        hFile = PLXOS_CreateFile(achFileName, PLXFS_ACCESS_READ, 0);
        if (hFile == -1)
            return NULL;
        nFileSize = PLXOS_GetFileSize(hFile);
        pMapFile = (BYTE*)PLXOS_MapFile(hFile, 0, nFileSize);
        ASSERT(pMapFile != (BYTE*)(-1));
        
        pData = pMapFile;
        
        // 使用最后一个ICON
        nIconCount = GET16(pData + ICON_COUNT_OFFSET);
        pData += ICON_FILE_HEADER + sizeof(ICONDIRENTRY) * (nIconCount - 1);
        
        // 获得最后一个icon的位图数据的偏移量
        nImageOffset = GET32(pData + FIELDOFFSET(ICONDIRENTRY, 
            dwImagOffset));
        
        // 定位到位图数据所在的偏移
        pData = pMapFile + nImageOffset;
        
        // Gets BITMAPINFOHEADER from bitmap file data
        bmih.biSize = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSize)); 
        bmih.biWidth = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biWidth)); 
        bmih.biHeight = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biHeight)); 
        bmih.biPlanes = GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biPlanes)); 
        bmih.biBitCount = 
            GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biBitCount)); 
        bmih.biCompression = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biCompression)); 
        bmih.biSizeImage = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSizeImage));
        bmih.biXPelsPerMeter = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biXPelsPerMeter));
        bmih.biYPelsPerMeter = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biYPelsPerMeter));
        bmih.biClrUsed = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrUsed));
        bmih.biClrImportant = 
            GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrImportant));
        
        bmih.biHeight = bmih.biHeight / 2;
        
        // Skips bitmap info header
        pData += sizeof(BITMAPINFOHEADER);
        
        // Calculates the color bits, if the result is an invalid, return 
        // error.
        cClrBits = (WORD)(bmih.biPlanes * bmih.biBitCount); 
        if (cClrBits != 1 && cClrBits != 4 && cClrBits != 8 && 
            cClrBits != 16 && cClrBits != 24 && cClrBits != 32)
        {
            PLXOS_UnmapFile(hFile, pMapFile, nFileSize);
            PLXOS_CloseFile(hFile);
            return NULL;
        }
        
        // Allocates memory for bitmap info pointer
        if (cClrBits <= 8)
        {
            pbmi = (BITMAPINFO*)MemAlloc(sizeof(BITMAPINFOHEADER) + 
                (1 << cClrBits) * sizeof(RGBQUAD));
        }
        else
            pbmi = (BITMAPINFO*)MemAlloc(sizeof(BITMAPINFO) + sizeof(RGBQUAD));
        
        if (!pbmi)
        {
            PLXOS_UnmapFile(hFile, pMapFile, nFileSize);
            PLXOS_CloseFile(hFile);
            
            return NULL;
        }
        
        // Fills bmiHeader of pbmi using bmih
        pbmi->bmiHeader = bmih;
        
        // Gets the palettes if necessary
        if (cClrBits <= 8)
        {
            for (i = 0; i < (1 << cClrBits); i++)
            {
                pbmi->bmiColors[i].rgbBlue = *pData++;
                pbmi->bmiColors[i].rgbGreen = *pData++;
                pbmi->bmiColors[i].rgbRed = *pData++;
                pbmi->bmiColors[i].rgbReserved = *pData++;
            }
        }
        
        hbmpXor = CreateCompatibleBitmap(hdc, pbmi->bmiHeader.biWidth, 
            pbmi->bmiHeader.biHeight);
        hbmpAnd = CreateCompatibleBitmap(hdc, pbmi->bmiHeader.biWidth, 
            pbmi->bmiHeader.biHeight);
        
        if (!hbmpXor || !hbmpAnd)
        {
            MemFree(pbmi);
            
            PLXOS_UnmapFile(hFile, pMapFile, nFileSize);
            PLXOS_CloseFile(hFile);
            
            return NULL;
        }
        
        SetDIBits(hdc, hbmpXor, 0, pbmi->bmiHeader.biHeight, 
            pData, pbmi, DIB_RGB_COLORS);
        
        pData += pbmi->bmiHeader.biHeight * 
            ((pbmi->bmiHeader.biWidth * cClrBits + 31) / 32 * 4);
        
        pbmi->bmiHeader.biBitCount = 1;
        pbmi->bmiColors[0].rgbBlue = 0;
        pbmi->bmiColors[0].rgbGreen = 0;
        pbmi->bmiColors[0].rgbRed = 0;
        pbmi->bmiColors[0].rgbReserved = 0;
        pbmi->bmiColors[1].rgbBlue = 255;
        pbmi->bmiColors[1].rgbGreen = 255;
        pbmi->bmiColors[1].rgbRed = 255;
        pbmi->bmiColors[1].rgbReserved = 0;
        
        SetDIBits(hdc, hbmpAnd, 0, pbmi->bmiHeader.biHeight, 
            pData, pbmi, DIB_RGB_COLORS);
        
        MemFree(pbmi);
        
        PLXOS_UnmapFile(hFile, pMapFile, nFileSize);
        PLXOS_CloseFile(hFile);
        
        pIconObj = (PICONOBJ)MemAlloc(sizeof(ICONOBJ));
        if (!pIconObj)
            return NULL;
        
        pIconObj->bWidth = cxDesired;
        pIconObj->bHeight = cyDesired;
        pIconObj->hBitmapAND = hbmpAnd;
        pIconObj->hBitmapXOR = hbmpXor;
        
        return (HANDLE)pIconObj;
        
    }
    else
        return NULL;
}

static HBITMAP CreateBitmapFromDIBData(HDC hdc, const void* pDIBData)
{
    static BITMAPINFO256 BitmapInfo256;

    int         i;
    BITMAPINFO* pbmi;
    WORD        bfType;
    DWORD       bfOffBits;
    HBITMAP     hBitmap;
    int         nClrBits;
    BYTE*       pData;
    int         nHeight;

    pData = (BYTE*)pDIBData;

    // If the file is a invalid bitmap file, just return
    if ((bfType = GET16(pData + OFFSET_TYPE)) != 0x4d42)
        return NULL;

    pbmi = (PBITMAPINFO)&BitmapInfo256;

    // Gets the bfOffbits field of BITMAPFILEHEADER
    bfOffBits = GET32(pData + OFFSET_OFFBITS);

    // Skips the bitmap file header
    pData += OFFSET_BITMAPINFOHEADER;

    // Gets BITMAPINFOHEADER from bitmap file data
    pbmi->bmiHeader.biSize = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSize)); 
    pbmi->bmiHeader.biWidth = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biWidth)); 
    pbmi->bmiHeader.biHeight = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biHeight)); 
    pbmi->bmiHeader.biPlanes = 
        GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biPlanes)); 
    pbmi->bmiHeader.biBitCount = 
        GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biBitCount)); 
    pbmi->bmiHeader.biCompression = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biCompression)); 
    pbmi->bmiHeader.biSizeImage = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSizeImage));
    pbmi->bmiHeader.biXPelsPerMeter = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biXPelsPerMeter));
    pbmi->bmiHeader.biYPelsPerMeter = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biYPelsPerMeter));
    pbmi->bmiHeader.biClrUsed = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrUsed));
    pbmi->bmiHeader.biClrImportant = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrImportant));

    // Skips bitmap info header
    pData += sizeof(BITMAPINFOHEADER);

    // Calculates the color bits, if the result is an invalid, return 
    // error.
    nClrBits = pbmi->bmiHeader.biPlanes * pbmi->bmiHeader.biBitCount; 
    if (nClrBits != 1 && nClrBits != 2 && nClrBits != 4 && nClrBits != 8 &&
        nClrBits != 16 && nClrBits != 24 && nClrBits != 32)
        return NULL;

    // Gets the palettes if necessary
    if (nClrBits <= 8)
    {
        for (i = 0; i < (1 << nClrBits); i++)
        {
            pbmi->bmiColors[i].rgbBlue = *pData++;
            pbmi->bmiColors[i].rgbGreen = *pData++;
            pbmi->bmiColors[i].rgbRed = *pData++;
            pbmi->bmiColors[i].rgbReserved = *pData++;
        }
    }
    else if (pbmi->bmiHeader.biCompression == BI_BITFIELDS)
    {
        for (i = 0; i < 3; i ++)
        {
            pbmi->bmiColors[i].rgbBlue = *pData++;
            pbmi->bmiColors[i].rgbGreen = *pData++;
            pbmi->bmiColors[i].rgbRed = *pData++;
            pbmi->bmiColors[i].rgbReserved = *pData++;
        }
    }
    nHeight = (pbmi->bmiHeader.biHeight > 0) ? 
        pbmi->bmiHeader.biHeight : -(pbmi->bmiHeader.biHeight);

    hBitmap = CreateCompatibleBitmap(hdc, pbmi->bmiHeader.biWidth, nHeight);

    if (hBitmap)
    {
        int nScanLines;

#if (COLUMN_BITMAP)
        if (pbmi->bmiHeader.biCompression & BI_COLUMN)
            nScanLines = pbmi->bmiHeader.biWidth;
        else
            nScanLines = nHeight;
#else
        nScanLines = nHeight;
#endif

        SetDIBits(hdc, hBitmap, 0, nScanLines, 
            ((BYTE*)pDIBData + bfOffBits), pbmi, DIB_RGB_COLORS);
    }

    return hBitmap;
}

/*************************************************************************/
/*          Resource Functions                                           */
/*************************************************************************/

#ifndef NORESOURCE  // Resource management

typedef struct tagRESINFO
{
    HINSTANCE   hInst;
    BOOL        bFile;
    const void* pFileOrData;
    DWORD       dwOffset;
} RESINFO, *PRESINFO;

#define TYPE_HR     0x5248  // "HR"
#define TYPE_FR     0x5246  // "FR"

typedef struct tagFINDRESINFO
{
    WORD        wType;
    WORD        bFile;
    const void* pFileOrData;
    DWORD       dwOffset;
    DWORD       dwSize;
} FINDRESINFO, *PFINDRESINFO;

// 使用数组保存未指定hInstance的注册资源，hInstance的低字取数组的索引，高
// 字取AUTO_INSTANCE，以便与应用程序指定的hInstance区分

#define AUTO_INSTANCE_FLAG  0xFFFF0000

#define INSTANCE_TYPE_MASK  0xFFFF0000
#define INSTANCE_INDEX_MASK 0x0000FFFF

#define ISUSERINSTANCE(hInst) (((DWORD)(hInst) \
    & INSTANCE_TYPE_MASK) != AUTO_INSTANCE_FLAG)

#define INDEXOFINSTANCE(hInst)((DWORD)hInst & INSTANCE_INDEX_MASK)
#define MAKEINSTANCE(index)(HINSTANCE)(index | AUTO_INSTANCE_FLAG)

#define RES_TABLE_SIZE  16
static RESINFO ResTable[RES_TABLE_SIZE];
static int nFree = 0;

// 使用HASH表保存指定hInstance的注册资源，这样可以提高注册资源信息的提取
// 速度，从而提高资源函数的性能

typedef struct tagRESITEM
{
    struct tagRESITEM* pNext;
    RESINFO ResInfo;
} RESITEM, *PRESITEM;

#define RES_HASH_TABLE_SIZE  16
static PRESITEM ResHashTable[RES_HASH_TABLE_SIZE];

// Internal function for alloc and free RESITEM 
static PRESINFO GetResourceInfo(HINSTANCE hInst);
static void HASHTBL_InsertItem(PRESITEM pResItem);
static BOOL HASHTBL_DeleteItem(HINSTANCE hInst);
static PRESITEM HASHTBL_GetItem(HINSTANCE hInst);
static void HASHTBL_Destroy(void);
static PRESITEM AllocResItem(void);
static void FreeResItem(PRESITEM pItem);

/*
**  Function : FindResource
**  Purpose  :
**      Determines the location of a resource with the specified type and 
**      name in the specified module. 
*/
HRSRC WINAPI FindResource(HINSTANCE hInst, PCSTR pName, PCSTR pType)
{
    static  FINDRESINFO FindResInfo;
    
    BYTE*   pResData;
    BYTE*   p;
    DWORD   strsize, rtType, rtResourceCount, rnOffset = 0, rnLength = 0;
    WORD    rscCount, resType;
    DWORD   rnFlags;
    DWORD   rnIDnum;
	WORD    i;
    BYTE*   res_cur;
    
    BYTE*   strtblpos;    // 全局字符串表的初始位置
    char*   strpos;
    char    resname[128];
    WORD    resNum, num = 0;
	WORD    strwSize, strwOffset, strNextOffset;
    DWORD   dwSize;
    PRESINFO pResInfo;

    int     hFile;

    pResInfo = GetResourceInfo(hInst);
    if (!pResInfo)
        return NULL;

    if (!pResInfo->bFile)
        pResData = (BYTE*)pResInfo->pFileOrData;
    else
    {
        hFile = PLXOS_CreateFile(pResInfo->pFileOrData, PLXFS_ACCESS_READ, 0);
        ASSERT(hFile != -1);

        dwSize = PLXOS_GetFileSize(hFile);
        ASSERT(dwSize > pResInfo->dwOffset);

        dwSize -= pResInfo->dwOffset;

        pResData = MemAlloc(dwSize);
        if (!pResData)
        {
            PLXOS_CloseFile(hFile);
            return NULL;
        }

        PLXOS_SetFilePointer(hFile, pResInfo->dwOffset, PLXFS_SEEK_SET);
        PLXOS_ReadFile(hFile, pResData, dwSize);
        PLXOS_CloseFile(hFile);
    }
    
    p = pResData + 2 + 2;       // Skip rscFileTypes & unused
    strsize = GET32(p);
    p = p + 4;
    rscCount = GET16(p);
    p = p + 2 + 2;              // rscCount & rscReserved;
    strtblpos = p;
    p = p + strsize;
    p = p + 2;// rscID
    resNum = GET16(p);          // rscTypes
    p = p + 2 + 4;              // rscTypes & rscTypePtr

    rtType = (DWORD)pType;
    resType = GET16(p);
    p = p + 2;
    while (resType != rtType)
    {
        rtResourceCount = GET16(p);
        p = p + 2 + 4 + rtResourceCount * 20;//20为资源信息结构的5个DWORD
        resType = GET16(p);
        p = p + 2;
        num++;
        if (num == resNum)
            return NULL;
    }

    rtResourceCount = GET16(p);
    p = p + 2 + 4;
    pName = MAKEINTRESOURCE(pName);

    for (i = 0; i < rtResourceCount; i++)
    {
        p = p + 12;//rnOffset & rnLength & rnFlags
        rnIDnum = GET32(p);
        p = p + 4;
		// for strtable
		if (rtType == 6)
		{
			if ((rnIDnum <= (unsigned long)pName) &&((unsigned long)pName < 
               (rnIDnum + 16)))
			{
				p = p - 4 * 4;//rnOffset & rnLength & rnFlags & rnID
				rnOffset = GET32(p);
				rnOffset = rnOffset + strsize + 6 * sizeof(WORD) + 
                    4 * sizeof(WORD); //资源总表，资源信息表，串表。

                p = p + 4;
                rnLength = GET32(p);
				p = pResData + rnOffset;
				strwSize = GET16(p);
				p = p + 2;
				p = p + 2 *((long)pName -(long)rnIDnum);
				strwOffset = GET16(p);
                p = p + 2;
                strNextOffset = GET16(p);
                p = p - 2;

                if (((unsigned long)pName <(rnIDnum + 15)) 
                    &&(strNextOffset != 0))
                {
                    rnLength = (DWORD)(GET16(p + 2) - strwOffset);
                }
                else
                    rnLength = (DWORD)(rnLength - 2 - 2 * 16 - strwOffset);
				p = p + 2 *(16 -(long)pName +(long)rnIDnum);
				p = p + strwOffset;
                rnOffset = p - pResData;
				//return(HRSRC)p;
                break;
			}
			res_cur = p;
			p = p + 4;//rnReserved
		}
		else
        {
			res_cur = p;
			p = p + 4;//rnReserved
            //Find the resource by string
			if (((rnIDnum - 0x80000000) >= 0) && 
               ((rnIDnum - 0x80000000) <= 0x80000000) &&((long) pName >= 0x8000))
			{
				rnIDnum = rnIDnum - 0x80000000;
				strpos = (char *)(strtblpos + rnIDnum);
				strcpy(resname, strpos);
				if (strcmp(resname, pName) == 0)
				{                
					p = p - 5 * 4;//rnOffset & rnLength & rnFlags & 
                                        //rnID & rnReserved
					rnOffset = GET32(p);
					rnOffset = rnOffset + strsize + 6 * sizeof(WORD) + 
                        4 * sizeof(WORD);

					p = p + 4;
					rnLength = GET32(p);
					p = p + 4;
					rnFlags = GET32(p);
					p = p + 4; 
					
					p = pResData + rnOffset;

                    break;
				}            
			}
			else //Find the resource by ID
			{
				if (rnIDnum == (unsigned long)pName)
				{
					p = p - 4 * 5;//rnOffset & rnLength & rnFlags & 
                                        //rnID & rnReserved
					rnOffset = GET32(p);
					rnOffset = rnOffset + strsize + 6 * sizeof(WORD)
						+ 4 * sizeof(WORD);//资源总表，资源信息表

					p = p + 4;
					rnLength = GET32(p);
					p = p + 4;
					rnFlags = GET32(p);

					p = pResData + rnOffset;
                    break;
				}                             
			}
		}
    }

    if (pResInfo->bFile)
        MemFree(pResData);

    if (i == rtResourceCount)
        return NULL;

    FindResInfo.wType = TYPE_FR;
    FindResInfo.bFile = (WORD)pResInfo->bFile;
    FindResInfo.pFileOrData = pResInfo->pFileOrData;
    FindResInfo.dwOffset = rnOffset;
    FindResInfo.dwSize = rnLength;

    return (HRSRC)&FindResInfo;
}

/*
**  Function : LoadResource
**  Purpose  :
**      Loads the specified resource into global memory. 
*/
HGLOBAL WINAPI LoadResource(HINSTANCE hInst, HRSRC hResInfo)
{
    int             hFile;
    BYTE*           pResData;
    PFINDRESINFO    pFindResInfo;

    pFindResInfo = (PFINDRESINFO)hResInfo;

    if (pFindResInfo->wType != TYPE_FR)
        return NULL;

    if (!pFindResInfo->bFile)
    {
        ASSERT(pFindResInfo->dwSize != 0);
        pResData = (BYTE*)pFindResInfo->pFileOrData;
        pResData += pFindResInfo->dwOffset;
    
    }
    else
    {
        hFile = PLXOS_CreateFile(pFindResInfo->pFileOrData, PLXFS_ACCESS_READ, 0);
        if (hFile == -1)
        {
            SetLastError(1);
            return NULL;
        }

        pResData = MemAlloc(pFindResInfo->dwSize + 4);
        *(DWORD*)pResData = 0xFFFFFFFF;

        PLXOS_SetFilePointer(hFile, pFindResInfo->dwOffset, PLXFS_SEEK_SET);

        PLXOS_ReadFile(hFile, pResData + 4, pFindResInfo->dwSize);
        PLXOS_CloseFile(hFile);
    }

    return (HGLOBAL)pResData;
}

/*
**  Function : LockResource
**  Purpose  :
**      Locks the specified resource in memory. 
*/
void* WINAPI LockResource(HGLOBAL hResData)
{
    BYTE* p;

    if (!hResData)
        return NULL;

    p = (BYTE*)hResData;
    if (GET32(p) == 0xFFFFFFFF)
        p = p + 4;

    return p;
}

/*
**  Function : FreeResource
**  Purpose  :
**      free resources loaded by using the LoadResource function.
*/
BOOL WINAPI FreeResource(HGLOBAL hResData)
{
    PBYTE p;
    
    p = (PBYTE)hResData;
    if (GET32(p) == 0xFFFFFFFF)
        MemFree((void *)hResData);
    
    return TRUE;
}

/*
**  Function : LoadBitmap
**  Purpose  :
**      Loads the specified bitmap resource from a resource file or data. 
*/
HBITMAP WINAPI LoadBitmap(HINSTANCE hInst, PCSTR pszBitmapName)
{
    HRSRC   hResInfo;
    HGLOBAL hResData;
    BYTE*   pResData;
    HBITMAP hBitmap;

    hResInfo = FindResource(hInst, pszBitmapName, RT_BITMAP);
    if (!hResInfo)
        return NULL;

    hResData = LoadResource(hInst, hResInfo);
    if (!hResData)
        return NULL;

    pResData = (BYTE*)LockResource(hResData);
    ASSERT(pResData != NULL);

    hBitmap = CreateBitmapFromDIBData(NULL, pResData);

    FreeResource(hResData);

    return hBitmap;
}

/*
**  Function : LoadIcon
**  Purpose  :
**      Loads the specified icon resource from the executable (.EXE) file 
**      associated with an application instance. 
**  Params   :
**      hInstance   : Identifies an instance of the module whose executable
**                    file contains the icon to be loaded. This parameter 
**                    must be NULL when a standard icon is being loaded. 
**      pszIconName : Points to a null-terminated string that contains the 
**                    name of the icon resource to be loaded.Alternatively,
**                    this parameter can contain the resource identifier in
**                    the low-order word and zero in the high-order word. 
**                    Use the MAKEINTRESOURCE macro to create this value. 
**  Return   :
**      If the function succeeds, returns the handle of the newly loaded 
**      icon. If the function fails, return NULL. To get the extendted 
**      error information, call GetLastError.
*/
HICON WINAPI LoadIcon(HINSTANCE hInstance, PCSTR pszIconName)
{
    HRSRC           hResInfo;
    HGLOBAL         hResData;
    BYTE            *pResData, *pData;
    PICONOBJ        pIconObj;
    WORD            idReserved, idType, idCount;
    ICONDIRENTRY    idEntries[MAX_IDCOUNT];
    int             i;
    BITMAPINFO*     pbmi;
    WORD            cClrBits;
    HBITMAP         hbmpXor, hbmpAnd;
    BITMAPINFOHEADER bmih;

    hResInfo = FindResource(hInstance, pszIconName, RT_ICON);
    if (!hResInfo)
        return NULL;

    hResData = LoadResource(hInstance, hResInfo);
    if (!hResData)
        return NULL;

    pResData = (BYTE*)LockResource(hResData);
    ASSERT(pResData != NULL);
    pData = pResData;

    pIconObj = (PICONOBJ)MemAlloc(sizeof(ICONOBJ));
    
    idReserved = GET16(pData);
    pData += 2;
    idType = GET16(pData);
    pData += 2;
    if (idType != (WORD)1)
    {
        FreeResource(pResData);
        return NULL;
    }
    idCount = GET16(pData);
    pData += 2;
    for (i = 0; i < idCount; i++)
    {
        idEntries[i].bWidth = *(BYTE*)pData;
        pData += 1;
        idEntries[i].bHeight = *(BYTE*)pData;
        pData += 1;
        idEntries[i].bColorCount = *(BYTE*)pData;
        pData += 1;
        idEntries[i].bReserved = *(BYTE*)pData;
        pData += 1;
        idEntries[i].wPlanes = GET16(pData);
        pData += 2;
        idEntries[i].wBitCount = GET16(pData);
        pData += 2;
        idEntries[i].dwBytesInRes = GET32(pData);
        pData += 4;
        idEntries[i].dwImagOffset = GET32(pData);
        pData += 4;
        if (i == idCount - 1)
        {
            pIconObj->bWidth = idEntries[i].bWidth;
            pIconObj->bHeight = idEntries[i].bHeight;
            pData = pResData + idEntries[i].dwImagOffset;
        }
    }

    // Gets BITMAPINFOHEADER from bitmap file data
    bmih.biSize = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSize)); 
    bmih.biWidth = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biWidth)); 
    bmih.biHeight = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biHeight)); 
    bmih.biPlanes = GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biPlanes)); 
    bmih.biBitCount = 
        GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biBitCount)); 
    bmih.biCompression = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biCompression)); 
    bmih.biSizeImage = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSizeImage));
    bmih.biXPelsPerMeter = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biXPelsPerMeter));
    bmih.biYPelsPerMeter = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biYPelsPerMeter));
    bmih.biClrUsed = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrUsed));
    bmih.biClrImportant = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biClrImportant));
    
    bmih.biHeight = bmih.biHeight / 2;
    
    // Skips bitmap info header
    pData += sizeof(BITMAPINFOHEADER);
    
    // Calculates the color bits, if the result is an invalid, return 
    // error.
    cClrBits = (WORD)(bmih.biPlanes * bmih.biBitCount); 
    if (cClrBits != 1 && cClrBits != 4 && cClrBits != 8 && 
        cClrBits != 16 && cClrBits != 24 && cClrBits != 32)
    {
        FreeResource(hResData);
        return NULL;
    }

    
    // Allocates memory for bitmap info pointer
    if (cClrBits <= 8)
    {
        pbmi = (BITMAPINFO*)MemAlloc(sizeof(BITMAPINFOHEADER) + 
            (1 << cClrBits) * sizeof(RGBQUAD));
    }
    else
        pbmi = (BITMAPINFO*)MemAlloc(sizeof(BITMAPINFOHEADER));
    
    if (!pbmi)
    {
        FreeResource(hResData);
        return NULL;
    }
    
    // Fills bmiHeader of pbmi using bmih
    pbmi->bmiHeader = bmih;
    
    // Gets the palettes if necessary
    if (cClrBits <= 8)
    {
        for (i = 0; i < (1 << cClrBits); i++)
        {
            pbmi->bmiColors[i].rgbBlue = *pData++;
            pbmi->bmiColors[i].rgbGreen = *pData++;
            pbmi->bmiColors[i].rgbRed = *pData++;
            pbmi->bmiColors[i].rgbReserved = *pData++;
        }
    }
    
    hbmpXor = CreateCompatibleBitmap(NULL, pbmi->bmiHeader.biWidth, 
        pbmi->bmiHeader.biHeight);
    hbmpAnd = CreateCompatibleBitmap(NULL, pbmi->bmiHeader.biWidth, 
        pbmi->bmiHeader.biHeight);
    
    if (!hbmpXor || !hbmpAnd)
    {
        MemFree(pbmi);
        
        FreeResource(hResData);
        return NULL;
    }
    
    SetDIBits(NULL, hbmpXor, 0, pbmi->bmiHeader.biHeight, 
        pData, pbmi, DIB_RGB_COLORS);
    
    pData += pbmi->bmiHeader.biHeight * 
        ((pbmi->bmiHeader.biWidth * cClrBits + 31) / 32 * 4);
    
    pbmi->bmiHeader.biBitCount = 1;
    pbmi->bmiColors[0].rgbBlue = 0;
    pbmi->bmiColors[0].rgbGreen = 0;
    pbmi->bmiColors[0].rgbRed = 0;
    pbmi->bmiColors[0].rgbReserved = 0;
    pbmi->bmiColors[1].rgbBlue = 255;
    pbmi->bmiColors[1].rgbGreen = 255;
    pbmi->bmiColors[1].rgbRed = 255;
    pbmi->bmiColors[1].rgbReserved = 0;
    
    SetDIBits(NULL, hbmpAnd, 0, pbmi->bmiHeader.biHeight, 
        pData, pbmi, DIB_RGB_COLORS);   
    
    MemFree(pbmi);
    pIconObj->hBitmapAND = hbmpAnd;
    pIconObj->hBitmapXOR = hbmpXor;

    FreeResource(hResData);
    
    return (HICON)pIconObj;
}

/*
**  Function : LoadMenu
**  Purpose  :
**      Loads the specified menu resource from the executable(.EXE) file 
**      associated with an application instance. 
*/
#if(!NOMENUS)
#define MAXMENUITME 16

HMENU WINAPI LoadMenu(HINSTANCE hInstance, PCSTR pszMenuName)
{
    HRSRC hResInfo;
    HGLOBAL  hResData;
    BYTE * pHMem, * pMem;
    char * strpos, * strp;
    //char pType[20];
    MENUTEMPLATE  MenuTemplate;
    WORD wResSize, MenuName;
    WORD wItems, wIt;
    MENUITEMTEMPLATE MenuItem[MAXMENUITME][MAXMENUITME];
    PMENUITEMTEMPLATE pMenuItem;
    WORD version;
    int flag = 1;//是否是第一项
    int n = 0, m = 1, i;
    int lastm = 0;
    int lastn[MAXMENUITME] ;
    HMENU hMenu;
    PRESINFO   pResInfo;

    //strcpy(pType, "MENU");
    for (i = 0; i < MAXMENUITME; i++)
        lastn[i] = 0;

    hResInfo = FindResource(hInstance, pszMenuName, RT_MENU);
    if (hResInfo == NULL)
        return NULL;
    hResData = LoadResource(hInstance, hResInfo);
    pHMem = (BYTE *)LockResource(hResData);
    if (pHMem == NULL)
    {
        FreeResource(hResData);
        return NULL;
    }

    pMem = pHMem;

    version = GET16(pMem);
    if (version != 0x0101)
    {
        FreeResource(hResData);
        return NULL;
    }

    MenuTemplate.version = 0x0101;
    pMem = pMem + 2;
    wResSize = GET16(pMem);
    pMem = pMem + 2;
	pMem = pMem + 2;//跳过reserved
    wItems = GET16(pMem);    
    wIt = wItems;
    pMem = pMem + 2;
    strpos = (char *)(pMem + 4 * 2 * wItems);

    while (wItems != 0)
    {   
        MenuItem[m][n].wFlags = GET16(pMem);

        pResInfo = GetResourceInfo(hInstance);
        if (pResInfo->bFile)
            MenuItem[m][n].wFlags |= MF_NOTUSERSTRING;
        
        pMem = pMem + 2;
        MenuItem[m][n].wID = GET16(pMem);
        pMem = pMem + 2;
        MenuName = GET16(pMem);		
        pMem = pMem + 2;

        if ((!(MenuItem[m][n].wFlags & MF_MENUBREAK))
            &&(!(MenuItem[m][n].wFlags & MF_MENUBARBREAK)))
        {
            strp = (char *)(strpos + MenuName);
            MenuItem[m][n].lpszItemName = strp;
        } 
        else
        {
            MenuItem[m][n].lpszItemName = NULL;
        }
     	pMem = pMem + 2;//跳过reserved

        if (flag == 1)//第一项一定是popup的
        {
            MenuTemplate.pMenuItems= &MenuItem[m][n]; 
            pMenuItem = MenuTemplate.pMenuItems ;            
            MenuItem[m][n].pMenuItems = &MenuItem[m + 1][0];
            if (MenuItem[m][n].wFlags & MF_END)
            {
                MenuItem[m][n].wFlags = MenuItem[m][n].wFlags - MF_END;
                n++ ;
                if (n > MAXMENUITME - 1)
                {
                    FreeResource(hResData);
                    return NULL;
                }
                MenuItem[m][n].wFlags = MF_END;
                MenuItem[m][n].wID = 0;
                MenuItem[m][n].lpszItemName = NULL;
                MenuItem[m][n].pMenuItems = NULL;
            }
            n++ ;
            if (n > MAXMENUITME - 1)
            {
                FreeResource(hResData);
                return NULL;
            }
            lastn[m] = n;
            m++ ;
            if (m > MAXMENUITME - 1)
            {
                FreeResource(hResData);
                return NULL;
            }
            n = 0;
            flag = 0;
        }
        else
        {            
            if (MenuItem[m][n].wFlags & MF_SEPARATOR)
            {
                MenuItem[m][n].lpszItemName = NULL;
                MenuItem[m][n].pMenuItems = NULL;
                n++ ;
                if (n > MAXMENUITME - 1)
                {
                    FreeResource(hResData);
                    return NULL;
                }
                lastn[m] = n;
                wItems --;
                continue;
            }
            if (!(MenuItem[m][n].wFlags & MF_POPUP) &&
               (MenuItem[m][n].wFlags & MF_END))
            {
                MenuItem[m][n].wFlags = MenuItem[m][n].wFlags - MF_END;
                n++ ;
                if (n > MAXMENUITME - 1)                  
                {
                    FreeResource(hResData);
                    return NULL;
                }
                MenuItem[m][n].wFlags = MF_END;
                MenuItem[m][n].wID = 0;
                MenuItem[m][n].lpszItemName = NULL;
                MenuItem[m][n].pMenuItems = NULL;
                n++;
                if (n > MAXMENUITME - 1)                  
                {
                    FreeResource(hResData);
                    return NULL;
                }
                lastn[m] = n;
                m --;
                n = lastn[m]; 
                while ((MenuItem[m][n - 1].wFlags == MF_END))
                {
                    m --;
                    n = lastn[m];
                }
            }
            else if ((MenuItem[m][n].wFlags & MF_POPUP) && 
                !(MenuItem[m][n].wFlags & MF_END))
            {   
                n++;
                if (n > MAXMENUITME - 1)                  
                {
                    FreeResource(hResData);
                    return NULL;
                }
                lastn[m] = n;
                lastm = m;

                m++;
                if (m > MAXMENUITME - 1)
                {
                    FreeResource(hResData);
                    return NULL;
                }

                while (lastn[m] != 0)
                {
                    m++;
                    if (m > MAXMENUITME - 1)
                    {
                        FreeResource(hResData);
                        return NULL;
                    }
                }
                MenuItem[lastm][lastn[lastm] - 1].pMenuItems 
                    = &MenuItem[m][0];
                n = 0;
            }
            else if ((MenuItem[m][n].wFlags & MF_POPUP) &&
               (MenuItem[m][n].wFlags & MF_END))
            {
                MenuItem[m][n].wFlags = MenuItem[m][n].wFlags - MF_END;
                n++ ;
                if (n > MAXMENUITME - 1)                  
                {
                    FreeResource(hResData);
                    return NULL;
                }
                MenuItem[m][n].wFlags = MF_END;
                MenuItem[m][n].wID = 0;
                MenuItem[m][n].lpszItemName = NULL;
                MenuItem[m][n].pMenuItems = NULL;
                
                n++;
                if (n > MAXMENUITME - 1)                  
                {
                    FreeResource(hResData);
                    return NULL;
                }
                lastn[m] = n;
                lastm = m;
                m++;
                if (m > MAXMENUITME - 1)
                {
                    FreeResource(hResData);
                    return NULL;
                }

                while (lastn[m] != 0)
                {
                    m++;
                    if (m > MAXMENUITME - 1)
                    {
                        FreeResource(hResData);
                        return NULL;
                    }
                }
                MenuItem[lastm][lastn[lastm] - 2].pMenuItems = &MenuItem[m][0];
                n = 0;
            }
            else if (!(MenuItem[m][n].wFlags & MF_POPUP) 
                && !(MenuItem[m][n].wFlags & MF_END))
            {
                n++ ;
                if (n > MAXMENUITME - 1)                  
                {
                    FreeResource(hResData);
                    return NULL;
                }
                lastn[m] = n;
            }
        }
        
        
        wItems --;
    }

    hMenu = LoadMenuIndirect(&MenuTemplate);

    FreeResource(hResData);
    return hMenu;
}
#endif //NOMENUS

/*
**  Function : LoadStringPtr
**  Purpose  :
**      Loads a string resource associated with a specified module, return
**      the string pointer. 
**  Params   :
**      hInstance   : Identifies an instance of the module. 
**      nID         : Specifies the ID of the string to be loaded. 
**      pBuffer     : Points to the buffer to receive the string. 
*/
PCSTR WINAPI LoadStringPtr(HINSTANCE hInstance, UINT uID)
{
	HRSRC   hResInfo;
    HGLOBAL hResData;

    hResInfo = FindResource(hInstance,(PCSTR)(DWORD)uID, RT_STRING);
    if (!hResInfo)
        return NULL;

    if (((PFINDRESINFO)hResInfo)->bFile)
    {
        SetLastError(1);
        return NULL;
    }

    hResData = LoadResource(hInstance, hResInfo);
    if (!hResData)
        return NULL;

	return (PCSTR)LockResource(hResData);
}

/*
**  Function : LoadString
**  Purpose  :
**      Loads a string resource associated with a specified module, copies 
**      the string into a buffer, and appends a terminating null character. 
**  Params   :
**      hInstance   : Identifies an instance of the module. 
**      nID         : Specifies the ID of the string to be loaded. 
**      pBuffer     : Points to the buffer to receive the string. 
**      nBufferMax  : Specifies the size of the buffer in bytes. The string 
**                    is truncated and null terminated if it is longer than 
**                    the number of characters specified. 
**  Return   :
**      If the function succeeds, return the number of bytes copied into the
**      buffer, not including the null-terminating character, or zero if the
**      string resource does not exist. To get extended error information, 
**      call GetLastError. 
*/
int WINAPI LoadString(HINSTANCE hInstance, UINT uID, PSTR pBuffer, 
                      int nBufferMax)
{
	HRSRC   hResInfo;
    HGLOBAL hResData;
    PSTR    pResData;
	int     i;

    hResInfo = FindResource(hInstance, (PCSTR)(DWORD)uID, RT_STRING);
    if (!hResInfo)
        return 0;

    hResData = LoadResource(hInstance, hResInfo);
    if (!hResData)
        return 0;

    pResData = (PSTR)LockResource(hResData);
    ASSERT(pResData != NULL);

	for (i = 0; i < nBufferMax - 1 && *pResData != '\0'; i++)
		*pBuffer++ = *pResData++;

	// 不管pBuffer是否够大，都以0结尾
    *pBuffer = '\0';

    FreeResource(hResData);

	return i;
}

/*
**  Function : RegisterResource
**  Purpose  :
**      Registers the resource file or data for subsequent use.
*/
HINSTANCE WINAPI RegisterResource(HINSTANCE hInst, const void* pRes,
                                  long lOffset, BOOL bFile)
{
    int         hFile;

    int         i;
    int         nFileLen;
    PRESITEM    pResItem;
    WORD        wFileType;

    if (!pRes || lOffset < 0)
    {
        SetLastError(1);
        return NULL;
    }

    // 以资源文件的形式注册资源时，要判断传入参数的有效性，包括文件可以打
    // 开，lOffset是否合理。另外，如果文件可以映射，则通过映射变换为资源
    // 数据的形式注册。
    if (bFile)
    {
        void*   pMapFile;

        hFile = PLXOS_CreateFile(pRes, PLXFS_ACCESS_READ, 0);
        if (hFile == -1)
        {
            SetLastError(1);
            return NULL;
        }

        nFileLen = PLXOS_GetFileSize(hFile);
        if (lOffset >= nFileLen)
        {
            SetLastError(1);
            PLXOS_CloseFile(hFile);
            return NULL;
        }

        // 尝试映射位图文件
        pMapFile = (void*)PLXOS_MapFile(hFile, 0, nFileLen);

        // 可以映射的资源文件
        if (pMapFile != NULL)
        {
            bFile = FALSE;
            pRes = pMapFile;

            wFileType = GET16((BYTE*)pRes + lOffset);
        }
        else
        {
            PLXOS_SetFilePointer(hFile, lOffset, PLXFS_SEEK_SET);
            PLXOS_ReadFile(hFile, &wFileType, sizeof(WORD));
            wFileType = swapword(wFileType);
        }

        PLXOS_CloseFile(hFile);
    }
    else
        wFileType = GET16((BYTE*)pRes + lOffset);

    if (wFileType != TYPE_HR)
    {
        SetLastError(1);
        return NULL;
    }

    if (!bFile)
    {
        pRes = (BYTE*)pRes + lOffset;
        lOffset = 0;
    }

    // hInst为NULL，需要分配hInst并返回，资源注册信息直接保存在注册资源表中
    if (!hInst)
    {
        for (i = nFree; i <= RES_TABLE_SIZE; i++)
        {
            if (ResTable[i].hInst == NULL)
                break;
        }

        if (i > RES_TABLE_SIZE)
            return NULL;

        ResTable[i].hInst = MAKEINSTANCE(i);
        ResTable[i].bFile = bFile;
        ResTable[i].pFileOrData = pRes;
        ResTable[i].dwOffset = lOffset;
        
        nFree = i + 1;

        return ResTable[i].hInst;
    }
    
    // hInst不为NULL，保存在资源注册信息的HASH表中
    
    pResItem = AllocResItem();

    pResItem->ResInfo.hInst = hInst;
    pResItem->ResInfo.bFile = bFile;
    pResItem->ResInfo.pFileOrData = pRes;
    pResItem->ResInfo.dwOffset = lOffset;

    HASHTBL_InsertItem(pResItem);

    return hInst;
}

/*
**  Function : UnregisterResource
**  Purpose  :
**      
*/
BOOL WINAPI UnregisterResource(HINSTANCE hInst)
{
    int index;

    if (ISUSERINSTANCE(hInst))
        return HASHTBL_DeleteItem(hInst);

    index = INDEXOFINSTANCE(hInst);
    if (index >= RES_TABLE_SIZE)
        return FALSE;
    
    ResTable[index].hInst = NULL;
    if (index < nFree)
        nFree = index;
    
    return TRUE;
}

/*
**  Function : GetResourceInfo
*/
static PRESINFO GetResourceInfo(HINSTANCE hInst)
{
    DWORD       index;
    PRESITEM    pResItem;

    if (ISUSERINSTANCE(hInst))
    {
        pResItem = HASHTBL_GetItem(hInst);
        return &pResItem->ResInfo;
    }

    index = INDEXOFINSTANCE(hInst);
    if (index >= RES_TABLE_SIZE)
        return NULL;

    return &ResTable[index];
}

/**************************************************************************/
/*          Functions for Hash                                            */
/**************************************************************************/

static int GetHashKey(HINSTANCE hInst)
{
    return ((DWORD)hInst) % RES_HASH_TABLE_SIZE;
}

static void HASHTBL_InsertItem(PRESITEM pResItem)
{
    int key;

    key = GetHashKey(pResItem->ResInfo.hInst);

    pResItem->pNext = ResHashTable[key];
    ResHashTable[key] = pResItem;
}

static BOOL HASHTBL_DeleteItem(HINSTANCE hInst)
{
    int key;
    PRESITEM pItem, pPrev;

    key = GetHashKey(hInst);

    pItem = ResHashTable[key];
    pPrev = NULL;

    while (pItem)
    {
        if (pItem->ResInfo.hInst == hInst)
            break;

        pPrev = pItem;
        pItem = pItem->pNext;
    }

    if (!pItem)
        return FALSE;

    if (!pPrev)
        ResHashTable[key] = NULL;
    else
        pPrev->pNext = pItem->pNext;

    FreeResItem(pItem);

    return TRUE;
}

static PRESITEM HASHTBL_GetItem(HINSTANCE hInst)
{
    int key;
    PRESITEM pResItem;
    
    key = GetHashKey(hInst);
    pResItem = ResHashTable[key];
    
    while (pResItem != NULL)
    {
        if (pResItem->ResInfo.hInst == hInst)
            break;
        
        pResItem = pResItem->pNext;
    }

    return pResItem;
}

static void HASHTBL_Destroy(void)
{
    int i;
    PRESITEM pResItem;
    
    for (i = 0; i < RES_HASH_TABLE_SIZE; i++)
    {
        while (ResHashTable[i])
        {
            pResItem = ResHashTable[i];
            ResHashTable[i] = pResItem->pNext;

            FreeResItem(pResItem);
        }
    }
}

/**************************************************************************/
/*          Functions for RESITEM Alloc and Free                          */
/**************************************************************************/

static RESITEM ResItemHeap[RES_HASH_TABLE_SIZE];
static PRESITEM pFreeItemList;
static BOOL bResItemHeapInit = FALSE;

#define ISHEAPITEM(pItem)((pItem >= ResItemHeap) && \
   (pItem <= ResItemHeap + RES_HASH_TABLE_SIZE - 1))

/*
**  Function : AllocResItem
**  Purpose  :
*/
static PRESITEM AllocResItem(void)
{
    PRESITEM pItem;

    if (!bResItemHeapInit)
    {
        int i;

        pItem = &ResItemHeap[RES_HASH_TABLE_SIZE - 1];
        for (i = RES_HASH_TABLE_SIZE - 1; i >= 0; i--, pItem--)
        {
            pItem->pNext = pFreeItemList;
            pFreeItemList = pItem;
        }

        bResItemHeapInit = TRUE;
    }

    if (pFreeItemList)
    {
        pItem = pFreeItemList;
        pFreeItemList = pFreeItemList->pNext;
    }
    else
        pItem = MemAlloc(sizeof(RESITEM));

    return pItem;
}

/*
**  Function : FreeResItem
**  Purpose  :
*/
static void FreeResItem(PRESITEM pItem)
{
    if (ISHEAPITEM(pItem))
    {
        pItem->pNext = pFreeItemList;
        pFreeItemList = pItem;
    }
    else
        MemFree(pItem);
}

#endif  // NORESOURCE
