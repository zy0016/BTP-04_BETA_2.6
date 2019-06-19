/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : program for font driver.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "hpfile.h"
#include "string.h"

#include "font.h"
#include "fontdrv.h"

/*********************************************************************\
* Function	 _readlong  
* Purpose    read 4 bytes from pSrcData and combine them into unsigned 
             long data  
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static uint32 _readlong(unsigned char *pSrcData)
{
    DWORD dw1, dw2, dw3, dw4;
    dw1 = *pSrcData;
    dw2 = *(pSrcData + 1);
    dw3 = *(pSrcData + 2);
    dw4 = *(pSrcData + 3);
    dw1 = dw1 << 24;
    dw2 = dw2 << 16;
    dw3 = dw3 << 8;
    dw4 = dw1 + dw2 + dw3 + dw4;
    return dw4;
}

/*********************************************************************\
* Function	 _Combine  
* Purpose    combine a DWORD value into pSrcData
             for example: data = 0x00345678; then the pSrcDate will point
             to a memory with 00 34 56 78.
             In the font driver, it's used to do the mask operation.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void _Combine(uint32 data,int8 *pSrcData)
{
    BYTE b0, b1, b2, b3;
    b0 = (BYTE)(data & 0x000000ff);
    b1 = (BYTE)((data & 0x0000ff00) >> 8);
    b2 = (BYTE)((data & 0x00ff0000) >> 16);
    b3 = (BYTE)((data & 0xff000000) >> 24);

    pSrcData[0] |= b3;
    pSrcData[1] |= b2;
    pSrcData[2] |= b1;
    pSrcData[3] |= b0;

    return;
}

/* define the initial font file and the file will give the default font name
** and default charset. In future, the system font will be given in this 
** file too.
*/
#if (INIFONTFILE)

/*********************************************************************\
* Function	   FillSystemInfo
* Purpose      Get the System Default Infomation.
               As using the initial file has not been confirmed, this 
               function may be no use.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int FillSystemInfo(void)
{
    int     hFile;
    char    *pFile;
    char    *pInfoStart, *pInfoEnd;
    int     nInfoLength;
    DWORD   nFileSize;
    char    szTemp[20];
    char    dir[64];
    char    PathName[64];
    char    FilePath[128];
    char    *pPos;
    int     nLength;
    char    *pChar;

    /* get font directory of PLX GUI SYSTEM */
    if (PLXOS_GetFontDirectory(dir) == -1)
        return -1;

    pPos = strrchr(dir, '/');
    if (pPos)
    {
        nLength = pPos - dir + 1;
        strncpy(PathName, dir, nLength);
        PathName[nLength] = '\0';
    }
    else
    {
        strcpy(PathName, dir);
        strcat(PathName, "/");
    }

    strcpy(FilePath, PathName);
    strcat(FilePath, FONTINI_NAME);
    
    hFile = PLXOS_CreateFile(FilePath, PLXFS_ACCESS_READ, 0);
    ASSERT(hFile);
    nFileSize   = PLXOS_GetFileSize(hFile);
    pFile       = (char*)PLXOS_MapFile(hFile, 0, nFileSize);
    ASSERT(pFile);

    memset(g_SystemFontName, 0, MAX_FONTNAME);

    /* find the "Font= " key in the filebuffer. */
    pInfoStart  = strstr(pFile, SYSFONTINFO);
    if (pInfoStart != NULL)
    {
        /* move the pInfoStart to the font name start */
        pInfoStart  += strlen(SYSFONTINFO);

        while (*pInfoStart == ' ')
            pInfoStart ++;

        /* find the end of the line */
        pInfoEnd    = strchr(pInfoStart, '\r');
        pChar = strchr(pInfoStart, ' ');
        if (pInfoEnd > pChar)
            pInfoEnd = pChar;

        if (pInfoEnd != NULL)
        {
            /* get the font name length and get the font name */
            nInfoLength = pInfoEnd - pInfoStart;
            if(nInfoLength < MAX_FONTNAME)
            {
                memcpy(g_SystemFontName, pInfoStart, nInfoLength);
                g_SystemFontName[nInfoLength] = 0;
            }
        }
    }

    /* use the same method as above to get charset */
    pInfoStart  = strstr(pFile, SYSCHARSETINFO);
    if (pInfoStart != NULL)
    {
        pInfoStart  += strlen(SYSCHARSETINFO);
        while (*pInfoStart == ' ')
            pInfoStart ++;

        pInfoEnd    = strchr(pInfoStart, '\r');
        pChar = strchr(pInfoStart, ' ');
        if (pInfoEnd > pChar)
            pInfoEnd = pChar;
        
        if (pInfoEnd != NULL)
        {
            nInfoLength = pInfoEnd - pInfoStart;
            
            if(nInfoLength >= 0)
            {
                ASSERT(nInfoLength < 20);
                memcpy(szTemp, pInfoStart, nInfoLength);
                /* change the external charset to interior */
                if (strcmp(szTemp,"GB2312_CHARSET") == 0)
                    g_SystemCharset = FC_GB2312;
                else if (strcmp(szTemp,"CHINESEBIG5_CHARSET") == 0)
                    g_SystemCharset = FC_BIG5;
                else if (strcmp(szTemp,"ANSI_CHARSET") == 0)
                    g_SystemCharset = FC_ASCII;
            }
        }
    }

    PLXOS_UnmapFile(hFile, pFile, nFileSize);
    PLXOS_CloseFile(hFile);
    return 1;
}
#endif

/*********************************************************************\
* Function	   InitPhysicalFont
* Purpose      Load Font file head info into physical font array.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int InitPhysicalFont(void)
{
    int         FontNum = 0;
    int         hFontFile;
    int         i, index;
    int         iFileSize;
    char        fontname[MAX_FONTNAME];
    char        dir[64];
    char        PathName[64];
    char        FilePath[128];
    char        FileName[32];
    char        pFileName[16][32];
    BYTE        *pFileHeader;
    PPHYFONT    fontArray;
    void        *pDir;
    void        *pDirEntry;
    char        *pPos;
    int         nLength;

#if (INIFONTFILE)
    BOOL bHaveSystemFont = FALSE;

    // Set the system default font
    FillSystemInfo();
#endif

    /* get font directory of PLX GUI SYSTEM */
    if (PLXOS_GetFontDirectory(dir) == -1)
        return -1;

    /* 1.search all the font files in ROM, get the number of font file */

    if (PLXOS_FindFirstFile(dir, &pDir, &pDirEntry) == -1)
        return -1;

    pPos = strrchr(dir, '/');
    if (pPos)
    {
        nLength = pPos - dir + 1;
        strncpy(PathName, dir, nLength);
        PathName[nLength] = '\0';
    }
    else
    {
        strcpy(PathName, dir);
        strcat(PathName, "/");
    }    

    do
    {
        PLXOS_GetDirEntryName(FileName, pDirEntry);

        strcpy(FilePath, PathName);
        strcat(FilePath, FileName);
        if (PLXOS_IsFileType(FilePath, PLXFS_IFDIR))
            continue;
        hFontFile = PLXOS_CreateFile(FilePath, PLXFS_ACCESS_READ, 0);
        iFileSize = PLXOS_GetFileSize(hFontFile);
        pFileHeader = (BYTE*)PLXOS_MapFile(hFontFile, 0, iFileSize);

        // 判断文件头4个字节是否"font"，以此决定字体文件
        if (((*(pFileHeader) == 'f') && (*(pFileHeader + 1) == 'o') && \
            (*(pFileHeader + 2) == 'n') && (*(pFileHeader + 3) == 't')))
        {
            strcpy(pFileName[FontNum], FileName);
            FontNum ++;
        }
        PLXOS_UnmapFile(hFontFile, pFileHeader, iFileSize);
        PLXOS_CloseFile(hFontFile);

        if(FontNum > 0x10)
            break;
    } while(PLXOS_FindNextFile(dir, pDir, &pDirEntry) != -1);

    PLXOS_FindClose(pDir, pDirEntry);

    // 如果没有字库文件，返回-1
    if (FontNum == 0 )
        return -1;

    /* 2.allocate memory space to Font array according to the file number */ 
    fontArray = (PPHYFONT)MemAlloc(FontNum * sizeof(PHYFONT));
    if (fontArray == NULL)
    {
        return -1;      /* insufficient memory, return error number */
    }
    ppfArray = fontArray;

    /* 3.load every font file header info into FontArray */
    for (index = 0; index < FontNum; index ++)
    {
        strcpy(FileName, pFileName[index]);

        strcpy(FilePath, PathName);
        strcat(FilePath, FileName);
        hFontFile = PLXOS_CreateFile(FilePath, PLXFS_ACCESS_READ, 0);
        iFileSize = PLXOS_GetFileSize(hFontFile);
        pFileHeader = (BYTE*)PLXOS_MapFile(hFontFile, 0, iFileSize);

        for (i = 0; i < MAX_FONTNAME; i ++)
        {
            fontname[i] = *(pFileHeader + 4 + i);
        }

        strcpy(fontArray[index].Name, fontname);            // font name 
        strcpy(fontArray[index].FileName, pFileName[index]);// font filename
        fontArray[index].Family = *(pFileHeader + 20) ;     // family no use
        fontArray[index].Charset = *(pFileHeader + 20) ;    // charset
        fontArray[index].Attrib = _readlong(pFileHeader + 21);// attribute

#if (INIFONTFILE)
        if ((strcmp(fontname, g_SystemFontName) == 0) &&
            (fontArray[index].Charset == g_SystemCharset))
        {
            // Set the system default font
            fontArray[index].Attrib |= FA_SYSTEMFONT;
            bHaveSystemFont = TRUE;
        }
#endif

        // font Height
        fontArray[index].Height     = *(pFileHeader + 25);
        // Double Byte char width
        fontArray[index].DBWidth    = *(pFileHeader + 26);
        // single Byte char width
        fontArray[index].SBWidth    = *(pFileHeader + 27);
        // Max width of this font
        fontArray[index].MaxWidth   = fontArray[index].DBWidth;
        // base line num from bottom
        fontArray[index].BaseLine   = *(pFileHeader + 29);
        // left extension
        fontArray[index].LeftExt    = *(pFileHeader + 30);
        // right extension
        fontArray[index].RightExt   = *(pFileHeader + 31);
        // reference number
        fontArray[index].RefCount   = 0;
        // a point to file header
        fontArray[index].pData      = (BYTE*)pFileHeader;
        // single byte char data offset from file header
        fontArray[index].SBOffset   = _readlong(pFileHeader + 32);
        // double byte char data offset from file header 
        fontArray[index].DBOffset   = _readlong(pFileHeader + 36);
        // single byte char data offset from file header
        fontArray[index].SBCharInfoOffset = _readlong(pFileHeader + 40);

        if ((fontArray[index].Attrib & FA_FIXEDSIZE) == FA_FIXEDSIZE )
        {   /* fixed size font */
            fontArray[index].Size.fix.ThisSize = fontArray[index].Height;
            fontArray[index].Size.fix.SizeNo = 1;
        }
        else
        {   /* scalable size font */
            fontArray[index].Size.var.MinSize = *(pFileHeader + 44);
            fontArray[index].Size.var.MaxSize = *(pFileHeader + 45);
        }

        fontArray[index].DataCount = (int32)iFileSize;

        PLXOS_CloseFile(hFontFile);
    }

#if (INIFONTFILE)

    if (bHaveSystemFont == FALSE)
    {
        /* 同时匹配不成功，系统缺省字体只匹配字体名
        */
        for (index = 0; index < FontNum; index ++)
        {
            if ((strcmp(fontArray[index].Name, g_SystemFontName) == 0))
            {
                /* Set the system default font */
                fontArray[index].Attrib |= FA_SYSTEMFONT;
                /* 匹配到字体名后，要修改字符集*/
                g_SystemCharset = ppfArray[index].Charset;
                bHaveSystemFont = TRUE;
            }
        }
    }
    
    if (bHaveSystemFont == FALSE)    
    {
        /* 字体名匹配不成功，系统缺省字体匹配字符集
        */
       for (index = 0; index < FontNum; index ++)
       {
            if (fontArray[index].Charset == g_SystemCharset)
            {
                memcpy(g_SystemFontName, fontArray[index].Name, 
                    MAX_FONTNAME);
                fontArray[index].Attrib |= FA_SYSTEMFONT;
                bHaveSystemFont = TRUE;
                break;
            }
       }
       if (bHaveSystemFont == FALSE)
       {
           memcpy(g_SystemFontName, fontArray[0].Name, MAX_FONTNAME);
           g_SystemCharset = fontArray[0].Charset;
           fontArray[0].Attrib |= FA_SYSTEMFONT;
       }

       /* 得到匹配好的系统字体名后，对所有相同字体名字体设置缺省字体 */
       for (index = 0; index < FontNum; index ++)
       {
           if ((strcmp(fontArray[index].Name, g_SystemFontName) == 0))
           {
               /* Set the system default font */
               fontArray[index].Attrib |= FA_SYSTEMFONT;
           }
       }
    }
    
#endif

    /* return number of physical fonts */
    return FontNum;
}

 /*********************************************************************\
 * Function	   InitCharset
 * Purpose     usually driver should support many charset, and info of
               the charsets are invariable, so driver can init the charset
               array immediately.
               current we just support 4 charsets: ASCII, GB2312, GBK, BIG5. 
 * Params	   
 * Return	 	   
 * Remarks	   
 **********************************************************************/
static int InitCharset(void)
{
    PCHARSET    csArray;

    csArray = (PCHARSET)MemAlloc(MAX_CHARSET_NUM * sizeof(CHARSET));
    if (csArray == NULL)
    {
        return -1;      /* insufficient memory, return error number */
    }

    pcsArray = csArray;

    ASSERT((MAX_CHARSET_NUM - 3) > 0);

    /* Init charset info of ASCII */
    csArray[0].Entry        = FC_ASCII;
    csArray[0].DBytes       = 0;        /* single byte char code */
    csArray[0].MinLCode     = 0x20;     /* minimal char code is 0 */
    csArray[0].MaxLCode     = 0x80;     /* maximal char code is 159 */
    csArray[0].MinHCode     = 1;        /* useless */
    csArray[0].MaxHCode     = 0xfe;     /* useless */
    csArray[0].DefCharCode  = 0x20;     /* default char */

    /* Init charset info of GB2312 */
    csArray[1].Entry        = FC_GB2312;
    csArray[1].DBytes       = 1;      /* double byte char code */
    csArray[1].MinLCode     = 0xa1;   /* minimal lower char code is 0xa1 */
    csArray[1].MaxLCode     = 0xfe;   /* maximal lower char code is 0xfe */
    csArray[1].MinHCode     = 0xa1;   /* minimal higher char code is 0xa1 */
    csArray[1].MaxHCode     = 0xfe;   /* maximal higher char code is 0xfe */
    csArray[1].DefCharCode  = 0xA1A1; /* default char */

    /* Init charset info of GBK */
    csArray[2].Entry        = FC_GBK;
    csArray[2].DBytes       = 1;      /* double byte char code */
    csArray[2].MinLCode     = 0x40;   /* minimal lower char code is 0xa1 */
    csArray[2].MaxLCode     = 0xfe;   /* maximal lower char code is 0xfe */
    csArray[2].MinHCode     = 0x81;   /* minimal higher char code is 0xa1 */
    csArray[2].MaxHCode     = 0xfe;   /* maximal higher char code is 0xfe */
    csArray[2].DefCharCode  = 0xA1A1; /* default char */

    /* Init charset info of BIG5 */
    csArray[3].Entry        = FC_BIG5;
    csArray[3].DBytes       = 1;      /* double byte char code */
    csArray[3].MinLCode     = 0x40;   /* minimal lower char code is 0xa1 */
    csArray[3].MaxLCode     = 0xfe;   /* maximal lower char code is 0xfe */
    csArray[3].MinHCode     = 0xa1;   /* minimal higher char code is 0xa1 */
    csArray[3].MaxHCode     = 0xf9;   /* maximal higher char code is 0xfe */
    csArray[3].DefCharCode  = 0xa140; /* default char */

    /* Init charset info of GB18030 */
    csArray[4].Entry        = FC_GB18030;
    csArray[4].DBytes       = 1;      /* double byte char code */
    csArray[4].MinLCode     = 0x30;   /* minimal lower char code is 0x20 */
    csArray[4].MaxLCode     = 0xfe;   /* maximal lower char code is 0xfe */
    csArray[4].MinHCode     = 0x81;   /* useless */
    csArray[4].MaxHCode     = 0xfe;   /* useless */
    csArray[4].DefCharCode  = 0xA3BF; /* default char */

    /* Init charset info of 1252 codepage */
    csArray[5].Entry        = FC_1252;
    csArray[5].DBytes       = 0;      /* double byte char code */
    csArray[5].MinLCode     = 0x00;   /* minimal lower char code is 0x20 */
    csArray[5].MaxLCode     = 0xff;   /* maximal lower char code is 0xfe */
    csArray[5].MinHCode     = 0x00;   /* useless */
    csArray[5].MaxHCode     = 0x00;   /* useless */
    csArray[5].DefCharCode  = 0x20; /* default char */

    /* return number of charsets */
    return MAX_CHARSET_NUM;
}

/*********************************************************************\
* Function	   InitPhyFontInfo
* Purpose      将所有物理字体的信息计算组合,有助于加快取字膜的速度,对于
               ASCII字符集字体，匹配合适的双字节字体。
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int InitPhyFontInfo(int FontNum)
{
    int16   Entry, AssEntry, CsIndex;

    pPhyFontInfo = (PPHYFONTINFO)MemAlloc(FontNum * sizeof(PHYFONTINFO));
    if (pPhyFontInfo == NULL)
        return -1;

    for (Entry = 0; Entry < FontNum; Entry ++)
    {
        CsIndex = GetCharsetEntry(ppfArray[Entry].Charset);

        /* 双字节字符集和非ANSI字符集不用匹配双字节字符集。
        ** 而对于其他单字节字符集FC_1252也不匹配，支持全西文。
        */
        if (pcsArray[CsIndex].DBytes == 1 || 
            pcsArray[CsIndex].Entry != FC_ASCII)
        {
            AssEntry = Entry;
            pPhyFontInfo[Entry].DBEntry     = Entry;
            pPhyFontInfo[Entry].SBEntry     = Entry;
            if ((ppfArray[Entry].Attrib & FA_EQUALWIDTH) == FA_EQUALWIDTH)
            {
                // 双字节字符集，不变宽ASCII字体
                pPhyFontInfo[Entry].SBInfoAddr  = 0;    
            }
            else
            {
                // 双字节字符集，变宽ASCII字体
                pPhyFontInfo[Entry].SBInfoAddr = 
                    (uint32)ppfArray[Entry].pData + 
                    ppfArray[Entry].SBCharInfoOffset;
            }

            pPhyFontInfo[Entry].pDBCharset = &(pcsArray[CsIndex]);
            /* 双字节字符集匹配单字节字符集，非ANSI单字节字符集不匹配 */
            if (pcsArray[CsIndex].DBytes == 1)
                pPhyFontInfo[Entry].pSBCharset = &(pcsArray[FC_ASCII]);
            else
                pPhyFontInfo[Entry].pSBCharset = &(pcsArray[CsIndex]);

            pPhyFontInfo[Entry].Height = ppfArray[Entry].Height;
        }
        else if (pcsArray[CsIndex].Entry == FC_ASCII) //只有ASCII才匹配
        {
            int AssCsIndex;

            /* 当字体为单字节字符集时，而要显示双字节字符，则寻找字号最接近
             * 的双字节字符集字体进行显示若没有找到双字节字符集字体，则仍返
             * 回原值
             */
            AssEntry = GetFontAssEntry(Entry);
            pPhyFontInfo[Entry].DBEntry     = AssEntry;
            pPhyFontInfo[Entry].SBEntry     = Entry;
            if ((ppfArray[Entry].Attrib & FA_EQUALWIDTH) == FA_EQUALWIDTH)
            {
                // 单字节字符集，不变宽ASCII字体
                pPhyFontInfo[Entry].SBInfoAddr  = 0;
            }
            else
            {
                // 单字节字符集，变宽ASCII字体
                pPhyFontInfo[Entry].SBInfoAddr  = 
                    (uint32)ppfArray[Entry].pData + 
                    ppfArray[Entry].SBCharInfoOffset;   // 支持变宽
            }
            // get charset info
            AssCsIndex = ppfArray[AssEntry].Charset;
            pPhyFontInfo[Entry].pDBCharset = &(pcsArray[AssCsIndex]);
            pPhyFontInfo[Entry].pSBCharset = &(pcsArray[CsIndex]);

            // 取两种物理字体中最大的高度
            if (ppfArray[Entry].Height > ppfArray[AssEntry].Height)
                pPhyFontInfo[Entry].Height = ppfArray[Entry].Height;
            else
                pPhyFontInfo[Entry].Height = ppfArray[AssEntry].Height;
        }

        pPhyFontInfo[Entry].DBWidth = (uint8)ppfArray[AssEntry].DBWidth;
        pPhyFontInfo[Entry].SBWidth = (uint8)ppfArray[Entry].SBWidth;
        
        pPhyFontInfo[Entry].DBPageSize = 
            (pPhyFontInfo[Entry].pDBCharset->MaxLCode -
            pPhyFontInfo[Entry].pDBCharset->MinLCode + 1);

        pPhyFontInfo[Entry].DBStartAddr = 
            (uint32)(ppfArray[AssEntry].pData + 
            ppfArray[AssEntry].DBOffset);
        pPhyFontInfo[Entry].SBStartAddr = 
            (uint32)(ppfArray[Entry].pData + 
            ppfArray[Entry].SBOffset);

        pPhyFontInfo[Entry].DBAttrib = ppfArray[AssEntry].Attrib;
        pPhyFontInfo[Entry].SBAttrib = ppfArray[Entry].Attrib;

        // : ...+3/4...由于两行存储奇数字节数，故宽度为4的倍数
        pPhyFontInfo[Entry].DBCharSize = 
            ((pPhyFontInfo[Entry].DBWidth + 3) / 4 * 
            ppfArray[AssEntry].Height) >> 1;

        /* 这里对SBCharSize 进行了不同处理，表明了行、列优先字体文件格式并
         * 不相同
         */
        if ((pPhyFontInfo[Entry].SBAttrib & FA_COLUMN) == FA_COLUMN)
            pPhyFontInfo[Entry].SBCharSize = 
                ((pPhyFontInfo[Entry].Height + 7) / 8 * 
                ppfArray[Entry].SBWidth);
        else
            pPhyFontInfo[Entry].SBCharSize = 
                ((pPhyFontInfo[Entry].SBWidth + 7) / 8 * 
                ppfArray[Entry].Height);
    }

    return FontNum;
}

/*********************************************************************\
* Function	   GetFontAssEntry
* Purpose      得到单字节所对应的双字节字库
* Params	   
    Entry      单字节字体字库入口
* Return	 	   
* Remarks	   
**********************************************************************/
static int GetFontAssEntry(int Entry)
{
    PPHYFONT    pPhyFont;
    PPHYFONT    pPhySBFont;
    uint16      Index;
    int         index = -1, bindex = -1, cindex = -1;
    int         distance, Min = 999, bMin = 999, cMin = 999;

    pPhySBFont = &(ppfArray[Entry]);

    // matching Font Height
    for (Index = 0; Index < PhyFontNo; Index ++)
    {
        pPhyFont = &(ppfArray[Index]);
        if ( pcsArray[pPhyFont->Charset].DBytes == 1)
        {
            //distance=pPhySBFont->Height - pPhyFont->Height;
            distance=pPhySBFont->DBWidth - pPhyFont->DBWidth;
            if (distance >= 0)
            {

                // 存在系统双字节字体，匹配比实际字体高度小最少的字号
                if (((pPhyFont->Attrib & FA_SYSTEMFONT) == FA_SYSTEMFONT) 
                    && bMin > distance)     
                {
                    bMin = distance;
                    bindex = Index;
                }

                if (cMin > distance)   /* 存在双字节字体，匹配比实际字
                                               体高度少最小的字号 */
                {
                    cMin=distance;
                    cindex=Index;
                }
            }
            else if (((pPhyFont->Attrib & FA_SYSTEMFONT) == FA_SYSTEMFONT) 
                && (Min > -distance))       /* 存在系统双字节字体，匹配比实
                                               际字体高度大最少字号 */
            {
                Min = -distance;
                index = Index;
            }
        }
    }

    if (bindex != -1)
        return bindex;
    if (cindex != -1)
        return cindex;
    if (index == -1)
        return Entry;
    else
        return index;
}

/*********************************************************************\
* Function	   GetCharsetEntry
* Purpose      根据字符集,得到字符集信息数组的索引
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int GetCharsetEntry(uint8 charsetno)
{
    PCHARSET    pCharset;
    uint16      Index;

    for (Index = 0; Index < CharsetNo; Index ++)
    {
        pCharset = &(pcsArray[Index]);
        if ( pCharset->Entry == charsetno )
        {
            return Index;
        }
    }

    return 0;
}

/*********************************************************************\
* Function	   GetFontEntry
* Purpose      According to the data provided by _LOGFONT, get the
               respective physical font entry.
               根据逻辑字体的信息得到物理字体的索引
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int GetFontEntry(PDRVLOGFONT pLogFont)
{
    PPHYFONT    pPhyFont;
    uint16      Index;
    int         Min = 999, index = -1, distance;
    int         bMin = 999, bindex = -1;

    // 1.matching Font Name
    for (Index = 0; Index < PhyFontNo; Index ++)
    {
        pPhyFont = &(ppfArray[Index]);
        if ((strcmp(pPhyFont->Name, pLogFont->name) == 0))
        {
            break;
        }
    }
    // Cannot find name in the ROM, 
    //Matching Font Name With First Font in ROM
    if (Index >= PhyFontNo)
    {
        //pPhyFont = &(ppfArray[0]);
        strcpy(pLogFont->name, g_SystemFontName);
    }

    // 2.matching Font Height
    for (Index = 0; Index < PhyFontNo; Index ++)
    {
        pPhyFont = &(ppfArray[Index]);
        if ((strcmp(pPhyFont->Name, pLogFont->name) == 0))
        {
            //pPhyFont = &(ppfArray[Index]);
            //distance = pLogFont->size - pPhyFont->Height;
            distance = pLogFont->size - pPhyFont->DBWidth;
            distance = distance > 0 ? distance : -distance;
            if ((pPhyFont->Charset == pLogFont->charset) && 
                (bMin > distance))
            {
                bMin = distance;
                bindex = Index;
            }
            else if (Min > distance)
            {
                Min = distance;
                index = Index;
            }
        }
    }
    if(bindex != -1)
        index = bindex;

    pPhyFont = &(ppfArray[index]);
    pLogFont->size = (int16)pPhyFont->Height;
    pLogFont->charset = pPhyFont->Charset;

    /* At current situation(Only Fix width font in ROM), the below codes 
     * may be no use, bacause I have got the index already.
     */

    // 3.find the actually index entry .
    for (Index = 0; Index < PhyFontNo; Index ++)
    {
        pPhyFont = &(ppfArray[Index]);
        if ((strcmp(pPhyFont->Name, pLogFont->name) == 0)
             && (pPhyFont->Height == ((BYTE)pLogFont->size))
             && (pLogFont->charset == pPhyFont->Charset))
        {
            if ((pPhyFont->Attrib & FA_FIXEDSIZE) == FA_FIXEDSIZE)
            {   /* fixed size font */
                if (((BYTE)pLogFont->size) == (pPhyFont->Size).fix.ThisSize)
                    return Index;
            }
            else
            {   /* scalable size font */
                if ((pLogFont->size >= (pPhyFont->Size).var.MinSize)
                    && (pLogFont->size <= (pPhyFont->Size).var.MaxSize) )
                    return Index;
            }
        }
    }

    /* not find the specified font, return index */
    return index;
}

/*********************************************************************\
* Function	   EnableFont
* Purpose      字体驱动初始化
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int32 EnableFont(PFONTDRVINFO pFontDrv)
{
    int iRet;

    if (pFontDrv == NULL)
        return -1;      /* null FONTDEV struct */

//    JudgeCPUType();

    /* Init physical font link */
    iRet = InitPhysicalFont();
    if (iRet < 0)
    {
        return iRet;    /* fail to init physical font data */
    }
    else
    {
        PhyFontNo = iRet;
    }

    /* Init charset link */
    iRet = InitCharset();
    if (iRet < 0)
    {
        return iRet;    /* fail to init charset data */
    }
    else
    {
        CharsetNo = iRet;
    }

/* 如果使用定义的字体初始化文件，则在此处初始化物理字体索引信息
 * 否则，在第一次创建字体，也就是创建系统字体时进行初始化
 */
#if (INIFONTFILE)
    InitPhyFontInfo(PhyFontNo);
#endif

    pFontDrv->Devices = 0;      /* device number, useless now */
    pFontDrv->FuncMask1 = 0;    /* optional functions mask, zero now */
    pFontDrv->FuncMask2 = 0;    /* extended optional functions mask, 
                                   zero now */

    /* The following funtions are necessary for font driver */
    pFontDrv->CreateFont = _CreateFont;
    pFontDrv->DestroyFont = DestroyFont;
    pFontDrv->EnumFontName = EnumFontName;
    pFontDrv->EnumFontSize = EnumFontSize;
    pFontDrv->GetCharWidth = GetCharWidth;
    pFontDrv->GetFontInfo = GetFontInfo;
    pFontDrv->GetStringExt = GetStringExt;
    pFontDrv->GetStringBmp = GetStringBmp;
    pFontDrv->SetDefaultChar = _SetDefaultChar;
    pFontDrv->GetStringExtEx = GetStringExtEx;

    return 1;
}

/*********************************************************************\
* Function	   _CreateFont
* Purpose      1.填写pFontData结构.
               2.字体的Refcont+1;
* Params	   
    pFont       Pointer to the physical font.
    pLogFont    Pointer to the logical font.
* Return	 	   
* Remarks	   
**********************************************************************/
static int32 _CreateFont(PFONTDEV pFont, PDRVLOGFONT pLogFont)
{
    static BOOL bSystemFontCreate = TRUE;
    int16       Entry;
    PFONTDATA   pFontData;

#if (INIFONTFILE)
    /* the first creation is the system font, we should initial the system 
     * infomation
     */
    if(bSystemFontCreate)
    {
        strcpy(pLogFont->name, g_SystemFontName);
        pLogFont->charset = g_SystemCharset;
        bSystemFontCreate = FALSE;
    }
#else
    /* the first creation is the system font, we should initial the system 
     * infomation
     */
    if(bSystemFontCreate)
    {
        int index;
        BOOL bHaveSystemFont = FALSE;

        strcpy(g_SystemFontName, pLogFont->name);
        g_SystemCharset = pLogFont->charset;
        switch(g_SystemCharset)
        {
        case ANSI_CHARSET:
            g_SystemCharset = FC_ASCII;
            break;
        case CHINESEBIG5_CHARSET:
            g_SystemCharset = FC_BIG5;
            break;
        case GB2312_CHARSET:
            g_SystemCharset = FC_GB2312;
            break;
        default:
            g_SystemCharset = FC_SYSTEMCHARSET;
            break;
        }

        for (index = 0; index < PhyFontNo; index ++)
        {
            /* 系统缺省字体优先进行字体名和字符集同时匹配。
            */
            if ((strcmp(ppfArray[index].Name, g_SystemFontName) == 0) &&
                (ppfArray[index].Charset == g_SystemCharset))
            {
                /* Set the system default font */
                ppfArray[index].Attrib |= FA_SYSTEMFONT;
                bHaveSystemFont = TRUE;
            }
        }

        if (bHaveSystemFont == FALSE)
        {
            /* 同时匹配不成功，系统缺省字体只匹配字体名
            */
            for (index = 0; index < PhyFontNo; index ++)
            {
                if ((strcmp(ppfArray[index].Name, g_SystemFontName) == 0))
                {
                    /* Set the system default font */
                    ppfArray[index].Attrib |= FA_SYSTEMFONT;
                    /* 匹配到字体名后，要修改字符集*/
                    g_SystemCharset = ppfArray[index].Charset;
                    bHaveSystemFont = TRUE;
                }
            }
        }

        if (bHaveSystemFont == FALSE)
        {
            /* 字体名匹配不成功，系统缺省字体匹配字符集
            */
           for (index = 0; index < PhyFontNo; index ++)
           {
                if (ppfArray[index].Charset == g_SystemCharset)
                {
                    memcpy(g_SystemFontName, ppfArray[index].Name, 
                        MAX_FONTNAME);
                    ppfArray[index].Attrib |= FA_SYSTEMFONT;
                    bHaveSystemFont = TRUE;
                    break;
                }
           }
           if (bHaveSystemFont == FALSE)
           {
               memcpy(g_SystemFontName, ppfArray[0].Name, MAX_FONTNAME);
               g_SystemCharset = ppfArray[0].Charset;
               ppfArray[0].Attrib |= FA_SYSTEMFONT;
           }

            /* 得到匹配好的系统字体名后，对所有相同字体名字体设置缺省字体 */
            for (index = 0; index < PhyFontNo; index ++)
            {
                if ((strcmp(ppfArray[index].Name, g_SystemFontName) == 0))
                {
                    /* Set the system default font */
                    ppfArray[index].Attrib |= FA_SYSTEMFONT;
                }
            }
        }

        InitPhyFontInfo(PhyFontNo);

        bSystemFontCreate = FALSE;
    }
#endif

    if (pFont == NULL)
        return sizeof(FONTDATA);
    
    if (pLogFont ==NULL)
        return -1;  

    switch(pLogFont->charset)
    {
    case ANSI_CHARSET:
        pLogFont->charset = FC_ASCII;
        break;
    case CHINESEBIG5_CHARSET:
        pLogFont->charset = FC_BIG5;
        break;
    case WESTEUROPE_CHARSET:
        pLogFont->charset = FC_1252;
        break;
    case GB2312_CHARSET:
        /*支持GBK字符集*/
        if (g_SystemCharset == FC_GBK)
            pLogFont->charset = FC_GBK;
        else
            pLogFont->charset = FC_GB2312;
        break;
    default:
        pLogFont->charset = g_SystemCharset;
        break;
    }

#if (BIG5SUPPORT)
    if(pLogFont->charset == FC_BIG5 && g_SystemCharset != FC_BIG5)
    {
        pLogFont->charset = FC_GB2312;
        Entry    = GetFontEntry(pLogFont);
        pLogFont->charset = FC_BIG5;
    }
    else
#endif
    if (pLogFont->charset == FC_1252)
    {
        pLogFont->charset = FC_ASCII;
        Entry = GetFontEntry(pLogFont);
        pLogFont->charset = FC_1252;
    }
    else
    {
        Entry    = GetFontEntry(pLogFont);
    }
    if (Entry < 0) /* the required font doesn't exist */
        return -1;

    /* how many times that create font with this physical font */
    (ppfArray[Entry].RefCount) ++;
    if (pPhyFontInfo[Entry].DBEntry != pPhyFontInfo[Entry].SBEntry)
        (ppfArray[pPhyFontInfo[Entry].DBEntry].RefCount ++);

    pFontData = (PFONTDATA)pFont;
    pFontData->Entry = (uint16)Entry;
    pFontData->Style = (uint8)(pLogFont->style);    /* font style, it's same 
                                                       as that in logfont */
    pFontData->Height  = pLogFont->size;            /* font height, same as 
                                                       that in logfont */
    pFontData->CsEntry = GetCharsetEntry(ppfArray[Entry].Charset);
    pFontData->Attrib  = ppfArray[Entry].Attrib;
    pFontData->Width =  ppfArray[Entry].DBWidth;    /* not equal width, how 
                                                       to get the width  I 
                                                       dont know */
    pFontData->Charset = pLogFont->charset;

    pCurFontData = pFontData;

    return sizeof(FONTDATA);
}

/*********************************************************************\
* Function	   DestroyFont
* Purpose      Decrease the physical font reference count
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int32 DestroyFont(PFONTDEV pFont)
{
    int16       Entry;
    PFONTDATA   pFontData;

    if (pFont == NULL)
        return -1;

    pFontData = (PFONTDATA)pFont;
    Entry = pFontData->Entry;

    /* substract the reference count */
    (ppfArray[Entry].RefCount) --;
    /* no created font for this one */
    if (ppfArray[Entry].RefCount <= 0)
         ppfArray[Entry].RefCount = 0;

    if (pPhyFontInfo[Entry].DBEntry != pPhyFontInfo[Entry].SBEntry)
    {
        Entry = pPhyFontInfo[Entry].DBEntry;
        (ppfArray[Entry].RefCount) --;

        /* no created font for this one */
        if (ppfArray[Entry].RefCount <= 0)
             ppfArray[Entry].RefCount = 0;
    }

    return 1;   /* useless return value */
}

#define Is1stByte(c)	(InRange((c), 0x81, 0xFE))
#define Is2ndByteIn2Bytes(c)	(InRange((c), 0x40, 0xFE) && (c) != 0x7F)
#define Is2ndByteIn4Bytes(c)	(InRange((c), 0x30, 0x39))
#define Is2ndByte(c)	(Is2ndByteIn2Bytes(c) || Is2ndByteIn4Bytes(c))
#define Is3rdByte(c)	(InRange((c), 0x81, 0xFE))
#define Is4thByte(c)	(InRange((c), 0x30, 0x39))

#define Start2Bytes (0)

/* 双字节一区
** A1A1 ~ A9FE 94 * 9 = 846
*/
#define Start2BytesArea1    Start2Bytes
#define PageSizeInArea1     (0xFE - 0xA1 + 1)       //94
#define InDBArea1(c1, c2)   \
    (InRange(c1, 0xA1, 0xA9) && InRange(c2, 0xA1, 0xFE))
#define GetOffset18030_2Bytes_Area1(c1, c2)     \
    (Start2BytesArea1 + ((c1) - 0xA1) * PageSizeInArea1 + (c2) - 0xA1)

/* 双字节用户区1 
** AAA1 ~ AFFE 94 * 6 = 564
*/
#define Start2BytesUserArea1 (GetOffset18030_2Bytes_Area1(0xA9, 0xFE) + 1)
#define PageSizeInUserArea1  (0xFE - 0xA1 + 1)       //94

#if (USERAREA1)
#define InDBUserArea1(c1, c2)   \
    (InRange(c1, 0xAA, 0xAF) && InRange(c2, 0xA1, 0xFE))
#define GetOffset18030_2Bytes_UserArea1(c1, c2)     \
    (Start2BytesUserArea1 + ((c1) - 0xAA) * PageSizeInUserArea1 + (c2) - 0xA1)
#else
#define InDBUserArea1(c1, c2)   \
    (FALSE)
#define GetOffset18030_2Bytes_UserArea1(c1, c2)     \
    (Start2BytesUserArea1 - 1)
#endif

/* 双字节二区
** B0A1 ~ F7FE 94 * 72 = 6768个
*/
#define Start2BytesArea2    (GetOffset18030_2Bytes_UserArea1(0xAF, 0xFE) + 1)
#define PageSizeInArea2     (0xFE - 0xA1 + 1)
#define InDBArea2(c1, c2)   \
    (InRange(c1, 0xB0, 0xF7) && InRange(c2, 0xA1, 0xFE))
#define GetOffset18030_2Bytes_Area2(c1, c2)     \
    (Start2BytesArea2 + ((c1) - 0xB0) * PageSizeInArea2 + (c2) - 0xA1)

/* 双字节用户区2
** F8A1 ~ FEFE 94 * 7 = 658
*/
#define Start2BytesUserArea2 (GetOffset18030_2Bytes_Area2(0xF7, 0xFE) + 1)
#define PageSizeInUserArea2  (0xFE - 0xA1 + 1)       //94

#if (USERAREA2)
#define InDBUserArea2(c1, c2)   \
    (InRange(c1, 0xF8, 0xFE) && InRange(c2, 0xA1, 0xFE))
#define GetOffset18030_2Bytes_UserArea2(c1, c2)     \
    (Start2BytesUserArea2 + ((c1) - 0xF7) * PageSizeInUserArea2 + (c2) - 0xA1)
#else
#define InDBUserArea2(c1, c2)   \
    (FALSE)
#define GetOffset18030_2Bytes_UserArea2(c1, c2)     \
    (Start2BytesUserArea2 - 1)
#endif

/* 双字节三区
** 8140 ~ A0FE 190 * 32 = 6080个
*/
#define Start2BytesArea3    (GetOffset18030_2Bytes_UserArea2(0xFE, 0xFE) + 1)
#define PageSizeInArea3     (0xFE - 0x80 + 1 + 0x7E - 0x40 + 1)
#define InDBArea3(c1, c2)   \
    (InRange(c1, 0x81, 0xA0) && InRange(c2, 0x40, 0xFE) && (c2) != 0x7F)
#define InDBArea3_1(c2)     ((c2) < 0x7F)
#define InDBArea3_2(c2)     ((c2) > 0x7F)
#define GetOffset18030_2Bytes_Area3_1(c1, c2)     \
    (Start2BytesArea3 + ((c1) - 0x81) * PageSizeInArea3 + (c2) - 0x40)
#define GetOffset18030_2Bytes_Area3_2(c1, c2)     \
    (Start2BytesArea3 + ((c1) - 0x81) * PageSizeInArea3 + (c2) - 0x40 - 1)

/* 双字节用户区3 
** A140 ~ A7A0 96 * 7 = 672
*/
#define Start2BytesUserArea3 (GetOffset18030_2Bytes_Area3_2(0xA0, 0xFE) + 1)
#define PageSizeInUserArea3  (0xA0 - 0x80 + 1 + 0x7E - 0x40 + 1)       //94

#if (USERAREA3)
#define InDBUserArea3(c1, c2)   \
    (InRange(c1, 0xA1, 0xA7) && InRange(c2, 0x40, 0xA0) && (c2) != 0x7F)
#define InDBUserArea3_1(c2)     ((c2) < 0x7F)
#define InDBUserArea3_2(c2)     ((c2) > 0x7F)
#define GetOffset18030_2Bytes_UserArea3_1(c1, c2)     \
    (Start2BytesUserArea3 + ((c1) - 0xA1) * PageSizeInUserArea3 + (c2) - 0x40)
#define GetOffset18030_2Bytes_UserArea3_2(c1, c2)     \
    (Start2BytesUserArea3 + ((c1) - 0xA1) * PageSizeInUserArea3 + (c2) - 0x40 - 1)
#else
#define InDBUserArea3(c1, c2)   \
    (FALSE)
#define InDBUserArea3_1(c2)     ((c2) < 0x7F)
#define InDBUserArea3_2(c2)     ((c2) > 0x7F)
#define GetOffset18030_2Bytes_UserArea3_1(c1, c2)     \
    (Start2BytesUserArea3 - 1)
#define GetOffset18030_2Bytes_UserArea3_2(c1, c2)     \
    (Start2BytesUserArea3 - 1)
#endif

/* 双字节四区
** AA40 ~ FEA0 96 * 85 = 8160个
*/
#define Start2BytesArea4    (GetOffset18030_2Bytes_UserArea3_2(0xA7, 0xA0) + 1)
#define PageSizeInArea4     (0xA0 - 0x80 + 1 + 0x7E - 0x40 + 1)
#define InDBArea4(c1, c2)   \
        (InRange(c1, 0xAA, 0xFE) && InRange(c2, 0x40, 0xA0) && (c2) != 0x7F)
#define InDBArea4_1(c2)     ((c2) < 0x7F)
#define InDBArea4_2(c2)     ((c2) > 0x7F)
#define GetOffset18030_2Bytes_Area4_1(c1, c2)     \
        (Start2BytesArea4 + ((c1) - 0xAA) * PageSizeInArea4 + (c2) - 0x40)
#define GetOffset18030_2Bytes_Area4_2(c1, c2)     \
        (Start2BytesArea4 + ((c1) - 0xAA) * PageSizeInArea4 + (c2) - 0x40 - 1)

/* 双字节五区
** A840 ~ A9A0 96 * 2 = 192个
*/
#define Start2BytesArea5    (GetOffset18030_2Bytes_Area4_2(0xFE, 0xA0) + 1)
#define PageSizeInArea5     (0xA0 - 0x80 + 1 + 0x7E - 0x40 + 1)
#define InDBArea5(c1, c2)   \
        (InRange(c1, 0xA8, 0xA9) && InRange(c2, 0x40, 0xA0) && (c2) != 0x7F)
#define InDBArea5_1(c2)     ((c2) < 0x7F)
#define InDBArea5_2(c2)     ((c2) > 0x7F)
#define GetOffset18030_2Bytes_Area5_1(c1, c2)     \
        (Start2BytesArea5 + ((c1) - 0xA8) * PageSizeInArea5 + (c2) - 0x40)
#define GetOffset18030_2Bytes_Area5_2(c1, c2)     \
        (Start2BytesArea5 + ((c1) - 0xA8) * PageSizeInArea5 + (c2) - 0x40 - 1)

/* 18030四字节区，由于不可能全部有效，故可能存在很多区，
** 客户提供的字库中共有 6531 个
*/
#define Start4Bytes (GetOffset18030_2Bytes_Area5_2(0xA9, 0xA0) + 1)
#define PageSizeIn4Bytes_1   (0xFE - 0x81 + 1)
#define PageSizeIn4Bytes_2   (0x39 - 0x30 + 1) 

#define Start4Bytes8139Area Start4Bytes
#define In4Bytes8139Area(c1, c2)     ((c1) == 0x81 && (c2) == 0x39)

/* 8139EE39 ~          */
#define Start4Bytes8139Area_1 Start4Bytes
#define In4Bytes8139Area_1(c3, c4)   ((c3) == 0xEE && (c4) == 0x39)
#define GetOffset18030_4Bytes_8139Area_1(c3, c4)  \
        (Start4Bytes8139Area_1 + 0)

/* 
8139EF30 ~ 8139FE39 16 * 10 = 160
    EF30 ~     EF39 10
    F030 ~     F039
    ...
    FE30 ~     FE39
*/
#define Start4Bytes8139Area_2 (GetOffset18030_4Bytes_8139Area_1(0xEE, 0x39) + 1)
#define In4Bytes8139Area_2(c3, c4)     \
        (InRange(c3, 0xEF, 0xFE) && InRange(c4, 0x30, 0x39))
#define GetOffset18030_4Bytes_8139Area_2(c3, c4)  \
        (Start4Bytes8139Area_2 + ((c3) - 0xEF) * PageSizeIn4Bytes_2 + (c4) - 0x30)
        
#define Start4Bytes82Area (GetOffset18030_4Bytes_8139Area_2(0xFE, 0x39) + 1)
#define In4Bytes82Area(c1)     ((c1) == 0x82)

/*
82308130 ~ 8234FE39 10 * 126 * 5 = 6300
  308130 ~   308139 10
    8230 ~     8239
    ...
    FE30 ~     FE39
  318130 ~   318139
    ...
    ...
  348130 ~   348139
    8230 ~     8239
    ...
  34FE30 ~   34FE39
*/
#define In4Bytes82Area_1(c2, c3, c4)     \
        (InRange(c2, 0x30, 0x34) && InRange(c3, 0x81, 0xFE) && InRange(c4, 0x30, 0x39))

/*
82358130 ~ 82358739 10 * 7 = 70
    8130 ~     8139 10
    8230 ~     8239
    ...
    8730 ~     8739
82358739是我直接补充的一个字符，便于最后成为一页。
*/
#define In4Bytes82Area_2(c2, c3, c4)     \
        ((c2) == 0x35 && InRange(c3, 0x81, 0x87) && InRange(c4, 0x30, 0x39))

#define GetOffset18030_4Bytes_82Area(c2, c3, c4)    \
        (Start4Bytes82Area + ((((c2) - 0x30) * PageSizeIn4Bytes_1 + (c3) - 0x81) * \
        PageSizeIn4Bytes_2 + (c4) - 0x30))
        
/* GSM编码到字库位置的映射 */
static const unsigned char gb2glyph[] = 
{
    0x01, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x09,
    0x0B, 0x0C, 0x0E, 0x0F, 0x10, 0x12, 0x13, 0x14, 
    0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 
    0x1D, 0x1E, 0x1F, 0x81, 0x82, 0x83, 0x84, 0x85, 
    0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D
};

#define IsGSMDBCode(c1, c2)     ((c1) == 0xAA && (InRange(c2, 0xA1, 0xC8)))
#define GetGSMglyphCode(c1, c2) (gb2glyph[(c2) - 0xA1])

static long Get2BytesCharOffset_18030(uint8 c1, uint8 c2)
{
    if (InDBArea1(c1, c2))
    {
        return GetOffset18030_2Bytes_Area1(c1, c2);
    }
#if (USERAREA1)
    if (InDBUserArea1(c1, c2))
    {
        return GetOffset18030_2Bytes_UserArea1(c1, c2);
    }
#endif
    else if (InDBArea2(c1, c2))
    {
        return GetOffset18030_2Bytes_Area2(c1, c2);
    }
#if (USERAREA2)
    else if (InDBUserArea2(c1, c2))
    {
        return GetOffset18030_2Bytes_UserArea2(c1, c2);
    }
#endif
    else if (InDBArea3(c1, c2))
    {
        if (InDBArea3_1(c2))
            return GetOffset18030_2Bytes_Area3_1(c1, c2);
        else
            return GetOffset18030_2Bytes_Area3_2(c1, c2);
    }
#if (USERAREA3)
    else if (InDBUserArea3(c1, c2))
    {
        if (InDBUserArea3_1(c2))
            return GetOffset18030_2Bytes_UserArea3_1(c1, c2);
        else
            return GetOffset18030_2Bytes_UserArea3_2(c1, c2);
    }
#endif
    else if (InDBArea4(c1, c2))
    {
        if (InDBArea4_1(c2))
            return GetOffset18030_2Bytes_Area4_1(c1, c2);
        else
            return GetOffset18030_2Bytes_Area4_2(c1, c2);
    }
    else if (InDBArea5(c1, c2))
    {
        if (InDBArea5_1(c2))
            return GetOffset18030_2Bytes_Area5_1(c1, c2);
        else
            return GetOffset18030_2Bytes_Area5_2(c1, c2);
    }

    return -1;
}

static long Get4BytesCharOffset_18030(uint8 c1, uint8 c2, uint8 c3, uint8 c4)
{
    if (In4Bytes8139Area(c1, c2))
    {
        if (In4Bytes8139Area_1(c3, c4))
            return GetOffset18030_4Bytes_8139Area_1(c3, c4);
        else if (In4Bytes8139Area_2(c3, c4))
            return GetOffset18030_4Bytes_8139Area_2(c3, c4);
    }
    else if (In4Bytes82Area(c1))
    {
        if (In4Bytes82Area_1(c2, c3, c4) || In4Bytes82Area_2(c2, c3, c4))
            return GetOffset18030_4Bytes_82Area(c2, c3, c4);
    }
    
    return -1;
}

static long GetDefaultCharOffset_18030(PCHARSET pCharSet)
{
    uint8 c1, c2;
    
    c1 = pCharSet->DefCharCode >> 8;
    c2 = pCharSet->DefCharCode & 0xFF;
    
    return Get2BytesCharOffset_18030(c1, c2);
}

static int GetDBCSBytes_18030(PFONTDATA pFontData, PSTRING pStr, uint32 *pOffset)
{
    uint8 c1, c2, c3, c4;
    PPHYFONTINFO pPhyFont;
    //CHARSET CharSet;
    //CharSet  = pcsArray[pFontData->CsEntry];

    PCHARSET pCharSet;

    c1 = *pStr;
    c2 = *(pStr + 1);
    if (IsGSMDBCode(c1, c2))
        return 0;

    pPhyFont = &pPhyFontInfo[pFontData->Entry];
    pCharSet  = pPhyFont->pDBCharset;

    ASSERT(pCharSet->DBytes);

    if (Is1stByte(c1))
    {
        if (Is2ndByteIn2Bytes(c2))
        {
            if (pOffset != NULL)
            {
                *pOffset = Get2BytesCharOffset_18030(c1, c2);
                if (*pOffset == -1)
                    *pOffset = GetDefaultCharOffset_18030(pCharSet);
            }

            return 2;
        }
        else if (Is2ndByteIn4Bytes(c2))
        {
            c3 = *(pStr + 2);
            c4 = *(pStr + 3);
            if (c3 == 0 || c4 == 0) //已经到了字符串尾
                return 0;

            if (pOffset != NULL)
            {
                *pOffset = Get4BytesCharOffset_18030(c1, c2, c3, c4);
                if (*pOffset == -1)
                    *pOffset = GetDefaultCharOffset_18030(pCharSet);
            }

            return 4;
        }
        else
        {
            if (c2 == 0) //已经到了字符串尾
                return 0; 

            if (pOffset != NULL)
            {
                *pOffset = GetDefaultCharOffset_18030(pCharSet);
            }
            
            return 2;
        }
    }
    
    return 0;
}

/*********************************************************************\
* Function	   GetDBCSBytes
* Purpose      
* Params
    pFontData   physical font
    pStr
    pOffset
* Return	 	   
* Remarks	   
**********************************************************************/
static int GetDBCSBytes(PFONTDATA pFontData, PSTRING pStr, uint32 *pOffset)
{
    uint8 c1;
    uint8 c2;
    PPHYFONTINFO pPhyFont;
    //CHARSET CharSet;
    //CharSet  = pcsArray[pFontData->CsEntry];

    PCHARSET pCharSet;

    if (pFontData->Charset == FC_GB18030)
    {
        return GetDBCSBytes_18030(pFontData, pStr, pOffset);
    }

    pPhyFont = &pPhyFontInfo[pFontData->Entry];
    c1 = *pStr;
    c2 = *(pStr + 1);

#if (BIG5SUPPORT)
    if(pFontData->Charset == FC_BIG5 && 
        (pPhyFont->pDBCharset->Entry == FC_GB2312 || 
        pPhyFont->pDBCharset->Entry == FC_GBK || 
        pPhyFont->pDBCharset->Entry == FC_GB18030))
    {
        uint16 Code;
        static PCHARSET pBIG5CharSet = NULL;

        if (pBIG5CharSet == NULL)
        {
            pBIG5CharSet = &pcsArray[GetCharsetEntry(FC_BIG5)];
            ASSERT(pGBKCharSet != 0);
        }

        pCharSet = pBIG5CharSet;
        if (!InRange(c1, pCharSet->MinHCode, pCharSet->MaxHCode) || 
            !InRange(c2, pCharSet->MinLCode, pCharSet->MaxLCode))
            return 0;

        if (pOffset != NULL)
        {
        /* 原来的设计不合理，并不是支持Big5，而是完成Big5到GB的转
        * 换。所以Charset不应该是CHINESEBIG5_CHARSET，对这种特殊
        * 情况如何处理有待讨论。目前暂时用CHINESEBIG5_CHARSET+1
        * 来表示这种情况。随之，STB的浏览器应该修改。
            */
            Big5ToGB2312((char *)(&Code), (PCSTR)pStr, 2);
            c2 = (uint8)(Code >> 8);
            c1 = (uint8)(Code);
            
            if (!InRange(c1, pPhyFont->pDBCharset->MinHCode, pPhyFont->pDBCharset->MaxHCode) || 
                !InRange(c2, pPhyFont->pDBCharset->MinLCode, pPhyFont->pDBCharset->MaxLCode))
            {
                c1 = pPhyFont->pDBCharset->DefCharCode >> 8;
                c2 = pPhyFont->pDBCharset->DefCharCode & 0xFF;
            }
            
            *pOffset = (c1 - pPhyFont->pDBCharset->MinHCode) 
                * pPhyFont->DBPageSize
                + (c2 - pPhyFont->pDBCharset->MinLCode);
        }

        return 2;
    }
#endif

    if (IsGSMDBCode(c1, c2))
        return 0;

    pCharSet  = pPhyFont->pDBCharset;
    if (pCharSet->DBytes)
    {
        if (pFontData->Charset == FC_GB2312)
        {
            static PCHARSET pGBKCharSet = NULL;
            if (pGBKCharSet == NULL)
            {
                pGBKCharSet = &pcsArray[GetCharsetEntry(FC_GBK)];
                ASSERT(pGBKCharSet != 0);
            }

            if (!InRange(c1, pGBKCharSet->MinHCode, pGBKCharSet->MaxHCode))
            {
                return 0;
            }
            else if (pOffset != NULL)
            {
                if (!InRange(c1, pCharSet->MinLCode, pCharSet->MaxLCode) ||
                    !InRange(c2, pCharSet->MinHCode, pCharSet->MaxHCode))
                {
                    /* The lead byte means the char is a DB char, 
                    ** but the second byte is out of the range. 
                    ** Then we use the default char.
                    */
                    c1 = pCharSet->DefCharCode >> 8;
                    c2 = pCharSet->DefCharCode & 0xFF;
                }

                *pOffset = ((c1 - pCharSet->MinHCode) 
                    * pPhyFont->DBPageSize + (c2 - pCharSet->MinLCode));
            }
        }
        else
        {
            if (!InRange(c1, pCharSet->MinHCode, pCharSet->MaxHCode))
            {
                return 0;
            }
            else if (pOffset != NULL)
            {
                if (!InRange(c2, pCharSet->MinLCode, pCharSet->MaxLCode))
                {
                    /* The lead byte means the char is a DB char, 
                    ** but the second byte is out of the range. 
                    ** Then we use the default char.
                    */
                    c1 = pCharSet->DefCharCode >> 8;
                    c2 = pCharSet->DefCharCode & 0xFF;
                }

                *pOffset = ((c1 - pCharSet->MinHCode) 
                    * pPhyFont->DBPageSize + (c2 - pCharSet->MinLCode));
            }
        }

        return 2;
    }
    else
    {
        return 0;//(c1 > pCharSet->MaxLCode );
    }
}

#if 0
static BOOL ISDBCSCODE(PFONTDATA pFontData, BYTE c1, BYTE c2)
{
    //CHARSET CharSet;
    //CharSet  = pcsArray[pFontData->CsEntry];

    PCHARSET pCharSet;
    pCharSet  = pPhyFontInfo[pFontData->Entry].pDBCharset;

#if (BIG5SUPPORT)
    if(pFontData->Charset == FC_BIG5 && g_SystemCharset != FC_BIG5)
    {
        /* 当进行BIG5间接支持时，字符集应该取BIG5字符集 */
        pCharSet = &pcsArray[GetCharsetEntry(FC_BIG5)];
    }
    else
#endif

    if (pFontData->Charset == FC_1252)
    {
        pCharSet = &pcsArray[GetCharsetEntry(FC_1252)];
    }

    if (pCharSet->DBytes)
    {
        return (c1>=pCharSet->MinHCode &&
                c1<=pCharSet->MaxHCode &&
                c2>=pCharSet->MinLCode &&
                c2<=pCharSet->MaxLCode );
    }
    else
    {
        return FALSE;//(c1 > pCharSet->MaxLCode );
    }
}
#endif

/*********************************************************************\
* Function	   GetStringBmp
* Purpose       得到字符串位图数据
* Params	   
     PFONTDEV pFont,    字体信息
     PSTRING pStr,      字符串
     int count,         字符串字节数
     int x,             字符串x方向偏移，为0
     PTEXTBMP pTextBmp  字符串位图buffer
* Return	 	   
* Remarks	   
**********************************************************************/
#if (COLUMN_BITMAP)

static int32 GetStringBmp(PFONTDEV pFont, PSTRING pStr, int count, 
                          int x, PTEXTBMP pTextBmp)
{
    int8        iCount, *pCharData, mod, *pData, bFixed;
    uint8       High, Low, *pSrcData;
    uint8       Height, SBWidth, SBHeight, DBWidth, DBHeight;
    uint32      BytesOffset, LineOffset, StrWidth,Entry;
    WORD        Code;
    PFONTDATA   pFontData;
    PPHYFONTINFO pPhyFont;
    int         nCharBytes;

    mod         = 0;
    pData       = (int8 *)pTextBmp->data;
    pData       += (x >> 3);
    pCharData   =  pData;


    StrWidth    = x;
    pFontData   = (PFONTDATA)pFont;
    Entry       = pFontData->Entry;
    pPhyFont    = &(pPhyFontInfo[Entry]);
    Height      = pFontData->Height;
    LineOffset  = (uint32)((pTextBmp->height + 31)/ 32 * 4 );
    // 下面四个变量为字体中实际存储用的宽、高
    SBWidth     = pPhyFont->SBWidth;
    DBWidth     = pPhyFont->DBWidth;
    SBHeight    = ppfArray[pPhyFont->SBEntry].Height;
    DBHeight    = ppfArray[pPhyFont->DBEntry].Height;
    bFixed      = (pPhyFont->SBAttrib & FA_EQUALWIDTH) == FA_EQUALWIDTH;

    while (count > 0)
    {
        nCharBytes == GetDBCSBytes(pFontData, pStr, &BytesOffset)
        if (nCharBytes >= 2)
        {
            BytesOffset *= pPhyFont->DBCharSize; /* move to this char */

            /* get the data */
            pSrcData    =  (uint8 *)(pPhyFont->DBStartAddr + BytesOffset);

            if (DBHeight == 16)
            {
                for (iCount = 0; iCount < DBWidth; iCount ++)
                    {   
                        pCharData[0] = pSrcData[0];
                        pCharData[1] = pSrcData[1];
                        pSrcData += 2;
                        pCharData += LineOffset;
                    }

                    pStr += nCharBytes;
                    count -= nCharBytes;
            }
            else if (DBHeight == 12)
            {
                for (iCount = 0; iCount < DBWidth; iCount += 2)
                {
                    /* 两行数据由3个字节纪录 */
                    /* 奇数行. */
                    pCharData[0] = pSrcData[0];
                    pCharData[1] = pSrcData[2] & 0xF0;
                    pCharData += LineOffset;
                    /* 偶数行. */
                    pCharData[0] = pSrcData[1];
                    pCharData[1] = (pSrcData[2] & 0x0F) << 4;
                    pCharData += LineOffset;
                    pSrcData += 3;
                }

                pStr += nCharBytes;
                count -= nCharBytes;
            }
            else if (DBHeight == 20)
            {
                for (iCount = 0; iCount < DBWidth; iCount += 2)
                {   
                    pCharData[0] = pSrcData[0];
                    pCharData[1] = pSrcData[1];
                    pCharData[2] = pSrcData[4] & 0xF0;
                    pCharData += LineOffset;
                    /* 偶数行. */
                    pCharData[0] = pSrcData[2];
                    pCharData[1] = pSrcData[3];
                    pCharData[2] = (pSrcData[4] & 0x0F) << 4;
                    pCharData += LineOffset;
                    pSrcData += 5;
                }

                pStr += nCharBytes;
                count -= nCharBytes;
            }
            else if (DBHeight == 24)
            {
                for (iCount = 0; iCount < DBWidth; iCount ++)
                {
                    pCharData[0] = pSrcData[0];
                    pCharData[1] = pSrcData[1];
                    pCharData[2] = pSrcData[2];
                    pCharData += LineOffset;
                    pSrcData += 3;
                }

                pStr += nCharBytes;
                count -= nCharBytes;
            }
            else if (DBHeight == 28)
            {
                for (iCount = 0; iCount < DBWidth; iCount += 2)
                {
                    pCharData[0] = pSrcData[0];
                    pCharData[1] = pSrcData[1];
                    pCharData[2] = pSrcData[2];
                    pCharData[3] = pSrcData[6] & 0xF0;
                    pCharData += LineOffset;
                    pCharData[0] = pSrcData[3];
                    pCharData[1] = pSrcData[4];
                    pCharData[2] = pSrcData[5];
                    pCharData[3] = (pSrcData[6] & 0x0F) << 4;
                    pCharData += LineOffset;
                    pSrcData += 7;
                }

                pStr += nCharBytes;
                count -= nCharBytes;
            }
            else if (DBHeight == 32)
            {
                for (iCount = 0; iCount < DBWidth; iCount ++)
                {   
                    pCharData[0] = pSrcData[0];
                    pCharData[1] = pSrcData[1];
                    pCharData[2] = pSrcData[2];
                    pCharData[3] = pSrcData[3];
                    pCharData += LineOffset;
                    pSrcData += 4;
                }

                pStr += nCharBytes;
                count -= nCharBytes;
            }           
        }
        else    // 单字节
        {
            if (IsGSMDBCode(*pStr, *(pStr+1))
                && (pFontData->Charset != FC_1252))
            {
                /* GSM的双字节字符，使用单字节字模显示 */
                Code = GetGSMglyphCode(*pStr, *(pStr+1));
                pStr += 2;
                count -= 2;
            }
            else
            {
                Code = (uint8)(*pStr);
                pStr += 1;
                count -= 1;
            }

            if ((Code < pPhyFont->pSBCharset->MinLCode) 
                || (Code > pPhyFont->pSBCharset->MaxLCode) )
            {
                Code = pPhyFont->pSBCharset->DefCharCode;
            }
            
            pSrcData = (uint8 *)(pPhyFont->SBStartAddr 
                + (Code * pPhyFont->SBCharSize));
            
            if (!bFixed)
            {
                int         Bytes, i;
                SBCHARINFO  *pCharInfo;
                int         UnFixedWidth;

                pCharInfo = (SBCHARINFO *)(pPhyFont->SBInfoAddr + Code * 4);
                UnFixedWidth      = pCharInfo->Width;
                Bytes = (SBHeight + 7) / 8;
                for (iCount = 0; iCount < UnFixedWidth; iCount ++)
                {
                    for(i = 0; i < Bytes; i++)
                    {
                        pCharData[i] = pSrcData[i];
                    }

                    pCharData += LineOffset;
                    pSrcData += Bytes;
                }
            }
            else
            {
                if (SBHeight <= 16 && SBHeight > 8)
                {
                    for (iCount = 0; iCount < SBWidth; iCount ++)
                    {
                        pCharData[0] = pSrcData[0];
                        pCharData[1] = pSrcData[1];
                        pCharData += LineOffset;
                        pSrcData += 2;
                    }
                }
                else if (SBHeight <= 24)
                {
                    for (iCount = 0; iCount < SBWidth; iCount ++)
                    {
                        pCharData[0] = pSrcData[0];
                        pCharData[1] = pSrcData[1];
                        pCharData[2] = pSrcData[2];
                        pCharData += LineOffset;
                        pSrcData += 3;
                    }
                    //StrWidth += SBWidth;
                }           
                else if (SBHeight <= 32)
                {
                    for (iCount = 0; iCount < SBWidth; iCount ++)
                    {
                        pCharData[0] = pSrcData[0];
                        pCharData[1] = pSrcData[1];
                        pCharData[2] = pSrcData[2];
                        pCharData[3] = pSrcData[3];
                        pCharData += LineOffset;
                        pSrcData += 4;
                    }
                    //StrWidth += SBWidth;
                }           
                else if (SBHeight <=8)
                {
                    for (iCount = 0; iCount < SBWidth; iCount ++)
                    {
                        pCharData[0] = pSrcData[0];
                        pCharData += LineOffset;
                        pSrcData ++;
                    }
                }
            }
        }
    }

    return pTextBmp->width;
}

#else

static int32 GetStringBmp(PFONTDEV pFont, PSTRING pStr, int count, 
                          int x, PTEXTBMP pTextBmp)
{
    int8        iCount, *pCharData, mod, *pData, bFixed;
    uint8       *pSrcData, SBWidth, DBWidth, DBHeight, SBHeight;
    uint32      i, BytesOffset, LineOffset, StrWidth, Entry;
    WORD        Code;
    PFONTDATA   pFontData;
    PPHYFONTINFO pPhyFont;
    int         nCharBytes;
    
    mod         = 0;
    pData       = (int8 *)pTextBmp->data;
    pData       += (x >> 3);

    StrWidth    = x;
    // 得到物理字体信息
    pFontData   = (PFONTDATA)pFont;
    Entry       = pFontData->Entry;
    pPhyFont    = &(pPhyFontInfo[Entry]);
    // 计算行偏移
    LineOffset  = (uint32)((pTextBmp->width + 31)/ 32 * 4 );
    // 下面四个变量为字体中实际存储用的宽、高
    SBWidth     = pPhyFont->SBWidth;
    DBWidth     = pPhyFont->DBWidth;
    SBHeight    = ppfArray[pPhyFont->SBEntry].Height;
    DBHeight    = ppfArray[pPhyFont->DBEntry].Height;
    bFixed      = (pPhyFont->SBAttrib & FA_EQUALWIDTH) == FA_EQUALWIDTH;

    while (count > 0)
    {
        nCharBytes = GetDBCSBytes(pFontData, pStr, &BytesOffset);
        if (nCharBytes >= 2)
        {
            BytesOffset *= pPhyFont->DBCharSize; /* move to this char */
                        
            /* get the data */
            pSrcData    =  (uint8 *)(pPhyFont->DBStartAddr + BytesOffset);
            pCharData   =  pData;

            if (DBWidth ==16 || DBWidth == 14)
            {
                DWORD A;
                for (iCount = 0; iCount < DBHeight; iCount ++)
                {
                    A=( ((DWORD)pSrcData[1] << 16) | 
                        ((DWORD)pSrcData[0] << 24)
                      ) >> mod;
                    _Combine(A, pCharData);
                    pSrcData += 2;
                    pCharData += LineOffset;
                }

                pData  += (mod + DBWidth) / 8;
                mod     = (mod + DBWidth) % 8; 

                pStr += nCharBytes;
                count -= nCharBytes;
            }
            else if (DBWidth ==12)
            {
                DWORD A;
                for (iCount = 0; iCount < DBHeight; iCount +=2)
                {
                    /* 两行数据由3个字节纪录 */
                    /* 奇数行. */
                    A = ((((DWORD)pSrcData[2] & 0xF0) << 16) | 
                        ((DWORD)pSrcData[0] << 24)) >> mod;
                    _Combine(A, pCharData);
                    pCharData += LineOffset;
                    /* 偶数行. */
                    A = ((((DWORD)pSrcData[2] & 0x0F) <<20) | 
                        ((DWORD)pSrcData[1] << 24)) >> mod;
                    _Combine(A, pCharData);
                    pCharData += LineOffset;
                    pSrcData += 3;
                }

                pData  += (mod + DBWidth) / 8;
                mod     = (mod + DBWidth) % 8;
                //StrWidth += pPhyFont->DBWidth;

                pStr += nCharBytes;
                count -= nCharBytes;
            }
            else if (DBWidth == 20)
            {
                DWORD A;
                for (iCount = 0; iCount < DBHeight; iCount +=2)
                {   
                    
                    // 两行数据由5个字节纪录
                    A = ((((DWORD)pSrcData[4] & 0xF0) << 8) | 
                        ((DWORD)pSrcData[1] << 16) | 
                        ((DWORD)pSrcData[0] << 24)) >> mod;
                    _Combine(A, pCharData);
                    pCharData += LineOffset;
                    /* 偶数行. */
                    A = ((((DWORD)pSrcData[4] & 0x0F) << 12) | 
                        ((DWORD)pSrcData[3] << 16) | 
                        ((DWORD)pSrcData[2] << 24)) >> mod;
                    _Combine(A, pCharData);
                    pCharData += LineOffset;
                    pSrcData += 5;
                }

                pData  += (mod + DBWidth) / 8;
                mod     = (mod + DBWidth) % 8;
                //StrWidth += pPhyFont->DBWidth;

                pStr += nCharBytes;
                count -= nCharBytes;
            }
            else if (DBWidth == 24)
            {
                DWORD A;
                for (iCount = 0; iCount < DBHeight; iCount ++)
                {
                    A = ( ((DWORD)pSrcData[2] << 8) | 
                        ((DWORD)pSrcData[1] << 16) | 
                        ((DWORD)pSrcData[0] << 24) ) >> mod;
                    _Combine(A, pCharData);
                    pSrcData += 3;
                    pCharData += LineOffset;
                }

                pData  += (mod + DBWidth) / 8;
                mod     = (mod + DBWidth) % 8;

                pStr += nCharBytes;
                count -= nCharBytes;
            }
            else if (DBWidth == 28)
            {
                DWORD A;
                for (iCount = 0; iCount < DBHeight; iCount += 2)
                {   
                    if (mod<=4)
                    {
                        // 两行数据由7个字节纪录
                        A = ((((DWORD)pSrcData[6] & 0xF0)) 
                            | ((DWORD)pSrcData[2] << 8 )
                            | ((DWORD)pSrcData[1] << 16)
                            | ((DWORD)pSrcData[0] << 24)) >> mod;
                        
                        _Combine(A, pCharData);
                        pCharData += LineOffset;
                        /* 偶数行. */
                        A = ((((DWORD)pSrcData[6] & 0x0F) << 4) 
                            | ((DWORD)pSrcData[5] << 8 )
                            | ((DWORD)pSrcData[4] << 16)
                            | ((DWORD)pSrcData[3] << 24)) >> mod;
                        _Combine(A, pCharData);
                        pCharData += LineOffset;
                    }
                    else
                    {/* data will overflow after shift mod */
                        // 两行数据由7个字节纪录
                        A = ((((DWORD)pSrcData[6] & 0xF0)) 
                            | ((DWORD)pSrcData[2] << 8 )
                            | ((DWORD)pSrcData[1] << 16)
                            | ((DWORD)pSrcData[0] << 24)) >> mod;
                        
                        _Combine(A, pCharData);
                        *(pCharData + 4) |= (((pSrcData[6] & 0xF0) >> 
                            (mod - 4) & 0x0f) << 4);

                        pCharData += LineOffset;
                        /* 偶数行. */
                        A = ((((DWORD)pSrcData[6] & 0x0F) << 4) 
                            | ((DWORD)pSrcData[5] << 8 )
                            | ((DWORD)pSrcData[4] << 16)
                            | ((DWORD)pSrcData[3] << 24)) >> mod;
                        _Combine(A,pCharData);
                        *(pCharData + 4) |= ((((pSrcData[6] & 0x0F) << 4) >> 
                            (mod - 4) & 0x0f) << 4);
                        pCharData += LineOffset;
                    }

                    pSrcData += 7;
                }

                pData  += (mod + DBWidth) / 8;
                mod     = (mod + DBWidth) % 8;

                pStr += nCharBytes;
                count -= nCharBytes;
            }
            else if (DBWidth == 32)
            {
                DWORD A;
                for (iCount = 0; iCount < DBHeight; iCount ++)
                {

                    A=( ((DWORD)pSrcData[1] << 16) | 
                        ((DWORD)pSrcData[0] << 24)
                      ) >> mod;
                    _Combine(A, pCharData);
                    A=( ((DWORD)pSrcData[3] << 16) | 
                        ((DWORD)pSrcData[2] << 24)
                      ) >> mod;
                    _Combine(A, pCharData + 2);
                    
                    pSrcData += 4;
                    pCharData += LineOffset;
                }

                pData  += (mod + DBWidth) / 8;
                mod     = (mod + DBWidth) % 8;

                pStr += nCharBytes;
                count -= nCharBytes;
            }
        }
        else    // 单字节
        {
            if (IsGSMDBCode(*pStr, *(pStr+1))
                && (pFontData->Charset != FC_1252))
            {
                /* GSM的双字节字符，使用单字节字模显示 */
                Code = GetGSMglyphCode(*pStr, *(pStr+1));
                pStr += 2;
                count -= 2;
            }
            else
            {
                Code = (uint8)(*pStr);
                pStr += 1;
                count -= 1;
//                if (pFontData->Charset == FC_1252)
//                {
//                }
//                else
                if ((Code < pPhyFont->pSBCharset->MinLCode) 
                    || (Code > pPhyFont->pSBCharset->MaxLCode) )
                {
                    Code = pPhyFont->pSBCharset->DefCharCode;
                }
            }

            pSrcData = (uint8 *)(pPhyFont->SBStartAddr + 
                                 (Code * pPhyFont->SBCharSize) );
            pCharData   =  pData;

            if (!bFixed)
            {
                SBCHARINFO  *pCharInfo;
                int         Bytes;
                uint8       UnFixedWidth;
                pCharInfo = (SBCHARINFO *)(pPhyFont->SBInfoAddr + Code * 4);

                UnFixedWidth = pCharInfo->Width;
                /* 字符宽度为实际宽度和实际存储宽度的较小值，对实际宽度大于
                ** 存储宽度的，将被截断。对变宽字体会出现
                */
                if (UnFixedWidth > SBWidth)
                    UnFixedWidth = SBWidth;

                // SBWidth为实际存储宽度
                Bytes = (SBWidth + 7) / 8; 
                
                for (iCount = 0; iCount < pPhyFont->Height; iCount ++)
                {
                    uint32 A=0;
                    //对宽度小于24的字符，不会由于mod原因丢掉，DWORD处理
                    if (UnFixedWidth <= 24)
                    {
                        for (i = 0; i < (uint32)Bytes; i++)
                        {
                            A |=  ((uint32)(pSrcData[i]) << (((3-i)<<3)));
                        }
                        //A = ( ((uint32)pSrcData[0]<<24) | 
                        //    ((uint32)pSrcData[1]<<16) ) >> mod;
                        A = A >> mod;
                        _Combine(A,pCharData);
                        pCharData += LineOffset;
                        pSrcData += Bytes;
                    }
                    //对宽度大于24的字符，可能由于mod原因丢掉,单字节处理
                    else
                    {
                        int8* pDstData;
                        pDstData = pCharData;
                        for (i = 0; i < (uint32)Bytes; i++)
                        {
                            A =  ((uint32)(pSrcData[i]) << 24) >> mod;
                            _Combine(A,pDstData);
                            pDstData ++;
                        }
                        pCharData += LineOffset;
                        pSrcData += Bytes;

                        //A = ( ((uint32)pSrcData[0]<<24) | 
                        //    ((uint32)pSrcData[1]<<16) ) >> mod;

                    }
                }

                pData  += (mod + UnFixedWidth) / 8;
                mod     = (mod + UnFixedWidth) % 8;
            }
            else
            {
                if (SBWidth == 8 || SBWidth == 6 || SBWidth == 7)
                {
                    for (iCount = 0; iCount < SBHeight; iCount ++)
                    {
                        uint32 A;
                        A = ((uint32)pSrcData[0]<<24) >> mod;
                        _Combine(A, pCharData);
                        pCharData += LineOffset;
                        pSrcData += 1;
                    }

                    pData  += (mod+SBWidth) / 8;
                    mod     = (mod+SBWidth) % 8;
                }
                else if (SBWidth == 10 || SBWidth == 12 || 
                    SBWidth == 14 || SBWidth == 16)  // 单字节字符占两个字节
                {
                    for (iCount = 0; iCount < SBHeight; iCount ++)
                    {
                        uint32 A;
                        A = ( ((uint32)pSrcData[0]<<24) | 
                            ((uint32)pSrcData[1]<<16) ) >> mod;
                        _Combine(A,pCharData);
                        pCharData += LineOffset;
                        pSrcData += 2;
                    }

                    pData  += (mod+SBWidth) / 8;
                    mod     = (mod+SBWidth) % 8;
                }
            }
        }
    }

    return pTextBmp->width;
}

#endif

/*********************************************************************\
* Function	   GetStringExt
* Purpose      Get width and height of string.
               It's used in the GetTextExtent()
* Params	   
    pFont       physical font info
    pStr        Pointer the string
    count       the count of string in bytes
    pTextBmp    Point to a TTEXTBMP struct to store the size of the bmp
* Return	 	   
* Remarks	   
**********************************************************************/
static int32 GetStringExt(PFONTDEV pFont, PSTRING pStr, int count, 
                          PTEXTBMP pTextBmp)
{
    int32       bmpsize, SBInfoAddr,Entry;
    uint32      StrWidth;
    PFONTDATA   pFontData;
    SBCHARINFO *pCharInfo;
    uint8       DBWidth, SBWidth, Height, bFixed;
    PPHYFONTINFO pPhyFont;
    int         nCharBytes;
    WORD        Code;

    pFontData   = (PFONTDATA)pFont;
    Entry       = pFontData->Entry;
    // 得到物理字体信息
    pPhyFont    = &(pPhyFontInfo[Entry]);

    // 单字节英文是否等宽
    bFixed    = (pPhyFont->SBAttrib & FA_EQUALWIDTH) == FA_EQUALWIDTH;
    StrWidth  = 0; 
    
    DBWidth     = pPhyFont->DBWidth;
    SBWidth     = pPhyFont->SBWidth;
    Height      = pPhyFont->Height;
    SBInfoAddr  = pPhyFont->SBInfoAddr;

    /* 对字符串循环处理，如果是中文，加上双字节的宽度，
     * 否则还要区分是否是等宽。
     */
    while (count > 0)
    {
        nCharBytes = GetDBCSBytes(pFontData, pStr, NULL);
        if (nCharBytes >= 2)
        {
            pStr     += nCharBytes;
            count    -= nCharBytes;
            StrWidth += DBWidth;
        }
        else    // 单字节
        {
            if (IsGSMDBCode(*pStr, *(pStr+1))
                && (pFontData->Charset != FC_1252))
            {
                /* GSM的双字节字符，使用单字节字模显示 */
                Code = GetGSMglyphCode(*pStr, *(pStr+1));
                pStr += 2;
                count -= 2;
            }
            else
            {
                Code = *pStr;
                pStr    += 1;
                count   -= 1;
                if ((Code < pPhyFont->pSBCharset->MinLCode) 
                    || (Code > pPhyFont->pSBCharset->MaxLCode) )
                {
                    Code = pPhyFont->pSBCharset->DefCharCode;
                }
            }
            if (!bFixed)
            {
                pCharInfo = (SBCHARINFO *)(SBInfoAddr + Code * 4);
                SBWidth   = pCharInfo->Width;
            }
            StrWidth += SBWidth;
        }
    }
    pTextBmp->height = Height;
    pTextBmp->width  = StrWidth;

#if (COLUMN_BITMAP)
    bmpsize = sizeof(TEXTBMP) + (Height + 31) / 32 * 4 * StrWidth;
#else
    bmpsize = sizeof(TEXTBMP) + (StrWidth + 31) / 32 * 4 * Height;
#endif

    return bmpsize;
}

static int32 GetStringExtEx(PFONTDEV pFont, PSTRING pStr, 
                            int count, int nMaxExtent, int *pnFit, 
                            int *apDx, PTEXTBMP pTextBmp)
{
    int32       bmpsize, SBInfoAddr,Entry;
    uint32      StrWidth;
    PFONTDATA   pFontData;
    SBCHARINFO *pCharInfo;
    uint8       DBWidth, SBWidth, Height, bFixed;
    PPHYFONTINFO pPhyFont;
    int nCharBytes;
    uint8       nFit;
    uint8       width;
    uint16      Code;

    ASSERT(count > 0);
    ASSERT(pnFit != NULL);

    pFontData   = (PFONTDATA)pFont;
    Entry       = pFontData->Entry;
    // 得到物理字体信息
    pPhyFont    = &(pPhyFontInfo[Entry]);

    // 单字节英文是否等宽
    bFixed    = (pPhyFont->SBAttrib & FA_EQUALWIDTH) == FA_EQUALWIDTH;
    StrWidth  = 0; 
    nFit = 0;
    
    DBWidth     = pPhyFont->DBWidth;
    SBWidth     = pPhyFont->SBWidth;
    Height      = pPhyFont->Height;
    SBInfoAddr  = pPhyFont->SBInfoAddr;

    /* 对字符串循环处理，如果是中文，加上双字节的宽度，
     * 否则还要区分是否是等宽。
     */
    while (count > 0)
    {
        nCharBytes = GetDBCSBytes(pFontData, pStr, NULL);
        if (nCharBytes >= 2)
        {
            if (count < nCharBytes)
            {
                /* 按照指定的串个数，最后一个为半字符，则不计算
                **  处理为一个单字节字符
                */
                nCharBytes = count;
                pStr    += count;
                count    = 0;
                width = pPhyFont->SBWidth * nCharBytes;
                goto process;
            }
            count -= nCharBytes;
            pStr  += nCharBytes;
            width = DBWidth;
        }
        else    // 单字节
        {
            if (IsGSMDBCode(*pStr, *(pStr+1))
                && (pFontData->Charset != FC_1252))
            {
                /* GSM的双字节字符，使用单字节字模显示 */
                Code = GetGSMglyphCode(*pStr, *(pStr+1));
                nCharBytes = 2;
                count   -= 2;
                pStr    += 2;
            }
            else
            {
                Code = *pStr;
                nCharBytes = 1;
                count   -= 1;
                pStr    += 1;
            }
            if (!bFixed)
            {
                pCharInfo = (SBCHARINFO *)(SBInfoAddr + Code * 4);
                SBWidth   = pCharInfo->Width;
            }
            width = SBWidth;
        }

process:
        StrWidth += width;
        /* 在制定的宽度以内，填充数组*/
        if (StrWidth <= (uint32)nMaxExtent)
        {
            if (apDx != NULL)
            {
                while (nCharBytes --)
                    apDx[nFit++] = StrWidth;
            }
            else
            {
                nFit += nCharBytes;
            }
        }
        else if(pTextBmp == NULL)
        {
            /* 不需要得到全部串的显示大小，直接返回能够显示的串大小 */
            StrWidth -= width;
            break;
        }
        else
        {
            /* 需要计算整个串的显示大小 */
            continue;
        }
    }

    *pnFit = nFit;

    if (pTextBmp != NULL)
    {
        pTextBmp->height = Height;
        pTextBmp->width  = StrWidth;
    }

#if (COLUMN_BITMAP)
    bmpsize = sizeof(TEXTBMP) + (Height + 31) / 32 * 4 * StrWidth;
#else
    bmpsize = sizeof(TEXTBMP) + (StrWidth + 31) / 32 * 4 * Height;
#endif

    return bmpsize;
}

// Internal function to support big5 to GB2312 convertion

#if (BIG5SUPPORT)

#include "big5togb.h"

/*********************************************************************\
* Function	   Big5ToGB2312
* Purpose      Converts a specified big5 string to GB2312 string.
* Params	   
    pszGB2312   Pointer to the GB code that has been transfered
    pszBig5     Pointer to the BIG5 code that should be transfered
    nCount      the length of the string that should be transfered
* Return	 	   
* Remarks	   
**********************************************************************/
void Big5ToGB2312(PSTR pszGB2312, PCSTR pszBig5, int nCount)
{
    ASSERT(pszGB2312 != NULL);
    ASSERT(pszBig5 != NULL);
    ASSERT(nCount > 0);

    while (nCount > 0)
    {
        // 编码大于0xA0, 是汉字的第一个字符, 进行big5到gb2312编码的转换
        if ((BYTE)(*pszBig5) > 0xa0)
        {
            BYTE low, high;
            int index;

            // 只剩一个字符, 错误字符编码, 填充空格退出循环 
            if (nCount == 1)
            {
                *pszGB2312 ++ = ' ';
                break;
            }

            // 获得汉字的big5编码
            high = (BYTE)(*pszBig5);
            low =  (BYTE)(*(pszBig5 + 1));

            if (high > 0xF9 || low < 0x40 || (low >= 0x7f && low <= 0xA0) 
                || low > 0xFE)
            {
                // 汉字的big5编码错误, 无法转换填充空格
                *pszGB2312 = ' ';
                *(pszGB2312 + 1) = ' ';
            }
            else
            {
                // 进行big5到gb2312编码的转换

                if (low > 0xa0)
                    low = low - 0x22;
                
                high = high - 0xa1;
                low  = low  - 0x40;
                
                index = (high * 0x9d + low) * 2;
            
                *pszGB2312 = Big5ToGB2312Table[index];
                *(pszGB2312 + 1) = Big5ToGB2312Table[index + 1];
            }

            pszGB2312 += 2;
            pszBig5 += 2;

            nCount -= 2;
        }
        else
        {
            // 英文字符不需要转换
            *pszGB2312 ++ = *pszBig5 ++;
            nCount --;
        }
    }
}
#endif

/*********************************************************************\
* Function	   IsValidCode
* Purpose      the following functions not used in this file, maybe used 
               by other. I am not sure these are correct.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int IsValidCode(uint16 Charset, uint16 Code)
{
    PCHARSET    pCharset;
    uint8       Code1, Code2;

    pCharset = (PCHARSET)&(pcsArray[Charset]);
    Code1    = (uint8)(Code & 0x00ff);
    Code2    = (uint8)(Code >> 8);

    if (pCharset->DBytes == 0)  /* Single byte charset*/
    {
        if ((Code1 < pCharset->MinLCode) 
            || (Code1 > pCharset->MaxLCode) )
            return 0;           /* invalid code, return FALSE */
    }
    else                        /* Double bytes charset */
    {
        if ((Code1 < pCharset->MinLCode) 
            || (Code1 > pCharset->MaxLCode)
            || (Code2 < pCharset->MinHCode) 
            || (Code2 > pCharset->MaxHCode) )
            return 0;           /* invalid code, return FALSE */
    }

    return 1;                   /* valide code, return TRUE */
}

/*********************************************************************\
* Function	   VerifyCharCode
* Purpose      this function maybe no use
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int VerifyCharCode(PFONTDATA pFontData, uint16 Code)
{
    uint32      Entry;
    PCHARSET    pCharset;
    uint8       Code1, Code2;
    PPHYFONTINFO pPhyFont;

    /* null pointer, return error number */
    if (pFontData == NULL)
        return -1;
    pFontData   = (PFONTDATA)pFontData;
    Entry       = pFontData->Entry;
    // 得到物理字体信息
    pPhyFont    = &(pPhyFontInfo[Entry]);

    Code1    = (uint8)(Code & 0x00ff);
    Code2    = (uint8)(Code >> 8);

    if (Code2 == 0)
        pCharset = pPhyFont->pSBCharset;
    else
        pCharset = pPhyFont->pDBCharset;

    if (pCharset->DBytes == 0)  /* Single byte charset*/
    {
        if ((Code1 < pCharset->MinLCode) 
            || (Code1 > pCharset->MaxLCode) )
            return 0;           /* invalid code, return FALSE */
    }
    else                        /* Double bytes charset */
    {
        if ((Code1 < pCharset->MinLCode) 
            || (Code1 > pCharset->MaxLCode)
            || (Code2 < pCharset->MinHCode) 
            || (Code2 > pCharset->MaxHCode) )
            return 0;           /* invalid code, return FALSE */
    }

    return 1;                   /* valide code, return TRUE */
}

/*********************************************************************\
* Function	   IsDBytesFont
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int IsDBytesFont(PFONTDATA pFontData)
{
    /* null pointer, return error number */
    if (pFontData == NULL)
        return -1;

    return pcsArray[pFontData->CsEntry].DBytes;
}

/*********************************************************************\
* Function	   GetFontExtend
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int GetFontExtend(PFONTDATA pFontData, PSIZE pSize)
{
    if ((pFontData == NULL) || (pSize == NULL) )
        return -1;  /* return error number */

    pSize->cx = (ppfArray[pFontData->Entry]).LeftExt;
    pSize->cy = (ppfArray[pFontData->Entry]).RightExt;

    return 1;
}

/*********************************************************************\
* Function	   GetCharSize
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int GetCharSize(PFONTDATA pFontData, uint16 Code)
{
    /* for each char of a font, the size/height is equal */
    return pFontData->Height;
}

/*********************************************************************\
* Function	   EnumFontName
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int32 EnumFontName(PFONTDEV pFont, PFONTNAMEINFO pInfo, int16 Serial)
{
    uint16      Index;
    PPHYFONT    pPhyFont;

    if (Serial == 0)
        return PhyFontNo;

    /* serial overrun the actual font number, return error no */
    if (Serial > PhyFontNo)
        return 0;

    /* value the font name info */
    Index = Serial -1;
    pPhyFont = (PPHYFONT)&(ppfArray[Index]);

    strcpy(pInfo->name, pPhyFont->Name);
    pInfo->family = pPhyFont->Family;
    //pInfo->charset = pPhyFont->Charset;
    pInfo->attrib = pPhyFont->Attrib;

    if ((pPhyFont->Attrib & FA_FIXEDSIZE) == FA_FIXEDSIZE )
    {
        pInfo->minsize = (pPhyFont->Size).fix.ThisSize;
        pInfo->maxsize = (pPhyFont->Size).fix.ThisSize;
    }
    else    /* scalable size */
    {
        pInfo->minsize = (pPhyFont->Size).var.MinSize;
        pInfo->maxsize = (pPhyFont->Size).var.MaxSize;
    }

    return 1;
}

/*********************************************************************\
* Function	   EnumFontSize
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int32 EnumFontSize(PFONTDEV pFont, PFONTNAMEINFO pInfo, int16 Serial)
{
    uint16  Index;
    uint16  Count;

    if (pInfo == NULL)
        return -1;  /* null pointer, return error number */

    for (Index = 0; Index < PhyFontNo; Index ++)
    {
        if ((strcmp(pInfo->name, ppfArray[Index].Name) == 0)
            && (pInfo->family == ppfArray[Index].Family) )
            break;  /* find the first matched font */
    }

    if (Index == PhyFontNo)
        return -1;

    if ((ppfArray[Index].Attrib & FA_FIXEDSIZE) == FA_FIXEDSIZE )
    {   /* this font is fixed size */
        if (Serial == 0)
        {
            return ppfArray[Index].Size.fix.SizeNo;
        }
        else
        {
            Count = 1;
            while (Count != Serial)
            {
                Index ++;
                if ((strcmp(pInfo->name, ppfArray[Index].Name) != 0)
                    || (pInfo->family != ppfArray[Index].Family) )
                    break;
            }

            if (Count != Serial)
            {
                /* serial number overrun the actual font number of this 
                   font name */
                return -1;  /* return error number */
            }

            return ppfArray[Index].Size.fix.ThisSize;
        }
    }
    else    /* scalable size font */
    {
        if (Serial == 0)
        {
            return (ppfArray[Index].Size.var.MaxSize - 
                ppfArray[Index].Size.var.MinSize + 1);
        }
        else
        {
            if (Serial <= (ppfArray[Index].Size.var.MaxSize - 
                ppfArray[Index].Size.var.MinSize + 1))
            {
                return (ppfArray[Index].Size.var.MinSize + Serial);
            }
            else
            {
                /* serial number overrun the actual font number of this 
                   font name */
                return -1;  /* return error number */
            }
        }
    }
}

/*********************************************************************\
* Function	   GetCharWidth
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int32 GetCharWidth(PFONTDEV pFont, uint16 Code)
{
    SBCHARINFO  *pCharInfo;
    PFONTDATA   pFontData;
    PPHYFONT    pPhyFont;
    int         width;
    uint8       pStr[3];

    /* null pointer, return error number */
    if (pFont == NULL)
        return -1;

    pStr[0] = Code >> 8;
    pStr[1] = Code;
    pStr[2] = '\0';
    pFontData = (PFONTDATA)pFont;

    if (pcsArray[pFontData->CsEntry].DBytes== 1)
    {
        pPhyFont    = (PPHYFONT)&(ppfArray[pFontData->Entry]);
        if (GetDBCSBytes(pFontData, pStr, NULL) >= 2)
        {
            width       = pPhyFont->DBWidth;
        }
        else // 单字节
        {
            if ((pPhyFont->Attrib & FA_EQUALWIDTH) == FA_EQUALWIDTH)
            {
                width = pPhyFont->SBWidth;
            }
            else
            {
                pCharInfo = (SBCHARINFO*)(pPhyFont->pData + 
                    pPhyFont->SBCharInfoOffset + Code * 4);
                width = pCharInfo->Width;
            }
        }
    }
    else
    {
        pPhyFont    = (PPHYFONT)&(ppfArray[pFontData->Entry]);
        if ((pPhyFont->Attrib & FA_EQUALWIDTH) == FA_EQUALWIDTH )
        {
            width   = pPhyFont->SBWidth;
        }
        else
        {
            // get char info address  
            pCharInfo   = (SBCHARINFO *)(pPhyFont->pData + 
                pPhyFont->SBCharInfoOffset + (uint8)Code * 4);
            width       = pCharInfo->Width;
        }
    }

    return width;

}

/*********************************************************************\
* Function	   GetFontInfo
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int32 GetFontInfo(PFONTDEV pFont, PFONTINFO pFontInfo)
{
    PFONTDATA   pFontData;
    PPHYFONT    pPhyFont;

    if (pFont == NULL)
        return -1;

    pFontData = (PFONTDATA)pFont;
    pPhyFont = (PPHYFONT)&(ppfArray[pFontData->Entry]);

    // info from physical font data
    strcpy(pFontInfo->name, pPhyFont->Name);
    pFontInfo->attrib       = pPhyFont->Attrib;
    pFontInfo->baseline     = pPhyFont->BaseLine;
    pFontInfo->leftextend   = pPhyFont->LeftExt;
    pFontInfo->rightextend  = pPhyFont->RightExt;

    // info from created font data
    pFontInfo->height   = pFontData->Height;
    pFontInfo->width    = pFontData->Width;
    pFontInfo->maxwidth = pPhyFont->MaxWidth;
    pFontInfo->charset  = pPhyFont->Charset; 

    return 1;
}

/*********************************************************************\
* Function	   SetDefaultChar
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int32 _SetDefaultChar(PFONTDEV pFont, uint16 Code)
{
    PFONTDATA   pFontData;
    int         iOriginChar;
    int         iRet;

    if (pFont == NULL)
        return -1;      /* return error number */

    iRet = VerifyCharCode(pFont, Code);

    if (iRet <= 0)      /* incorrect default char code */
    {
        return iRet;    /* return error number or FALSE */
    }

    pFontData = (PFONTDATA)pFont;
    iOriginChar = pcsArray[pFontData->CsEntry].DefCharCode;
    pcsArray[pFontData->CsEntry].DefCharCode = Code;

    return iOriginChar; /* return original default char */
}
