/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display driver
 *
 * Purpose  : Implement the blt n to 24 functions.
 *            
\**************************************************************************/

typedef struct CopyLine1to24Struct
{
    WORD    headlen;
    WORD    blocklen;
    WORD    taillen;
    WORD    headoffset; //如果首尾都在一个字节内，则存放字节尾无效位数
    void    (*copylinefunc)(BYTE*, BYTE*, struct CopyLine1to24Struct*, DWORD*);
    DWORD   palette[16][3];
}COPYLINE1TO24STRUCT, *PCOPYLINE1TO24STRUCT;

#define BITSPERBYTES    (8)
/*********************************************************************\
* Function	   BLOCK_1TO24_COPYLINE
* Purpose      实现1位向24位色的转换行操作
* Params	   
            dst:        目的地址，必是某象素起始地址
            src:        源地址
            blocklen:   传输块个数，16字节为一块, 8个象素，源一个字节
            headlen:    起始单象素个数，可能为0-7, 源的第一个字节中有效位数
            taillen:    终止单象素个数，可能为0-7, 源的最后一个字节有效位数
            head:       以00011111格式存放的首字节
            tail:       以00001111格式存放的尾字节
            pPalette:   物理调色板入口
* Return	 	   
* Remarks	
            认为24位色目的设备，象素地址必然2字节对齐。
            1位色设备，象素地址不定。
/**********************************************************************/
#define PUTPIXEL_1TO24(dst, index, p) { \
    PUTPIXEL(dst, p[index]);    \
    (BYTE*)(dst)+=3;    \
}

#define PUTBLOCK_1TO24(dst, index, p) { \
    *((DWORD*)(dst))++ = (p)[index][0];     \
    *((DWORD*)(dst))++ = (p)[index][1];     \
    *((DWORD*)(dst))++ = (p)[index][2];     \
}

void DWORDBLOCK_COPYLINE_1TO24(BYTE* dst, BYTE* src, PCOPYLINE1TO24STRUCT p, 
                          DWORD* pPalette)
{
    int blocklen = p->blocklen;
    int headlen = p->headlen;
    int taillen = p->taillen;
    BYTE srcheadbyte;
    BYTE srctailbyte;

    srcheadbyte = *src << p->headoffset;
    if (headlen != 0)
    {
        while ((headlen)--)
        {
            PUTPIXEL_1TO24(dst, srcheadbyte >> 7 , pPalette);
            srcheadbyte <<= 1;
        }
        ++(src);
    }
    
    while ((blocklen)--)
    {
        if (*src == 0)
        {
            PUTBLOCK_1TO24(dst, 0, p->palette);
            PUTBLOCK_1TO24(dst, 0, p->palette);
        }
        else if (*src == 0xFF)
        {
            PUTBLOCK_1TO24(dst, 0x0F, p->palette);
            PUTBLOCK_1TO24(dst, 0x0F, p->palette);
        }
        else
        {
            PUTBLOCK_1TO24(dst, (*src & 0xF0) >> 4, p->palette);
            PUTBLOCK_1TO24(dst, (*src & 0x0F) >> 0, p->palette);
        }

        src++;
    }

    srctailbyte = *src;
    while ((taillen)--)
    {
        PUTPIXEL_1TO24(dst, srctailbyte >> 7 , pPalette);
        srctailbyte <<= 1;
    }
}

void BYTEBLOCK_COPYLINE_1TO24(BYTE* dst, BYTE* src, PCOPYLINE1TO24STRUCT p, 
                               DWORD* pPalette)
{
    int blocklen = p->blocklen;
    int headlen = p->headlen;
    int taillen = p->taillen;
    BYTE srcheadbyte;
    BYTE srctailbyte;
    
    srcheadbyte = *src << p->headoffset;
    if (headlen != 0)
    {
        while ((headlen)--)
        {
            PUTPIXEL_1TO24(dst, srcheadbyte >> 7 , pPalette);
            srcheadbyte <<= 1;
        }
        ++(src);
    }
    
    while ((blocklen)--)
    {
        if (*src == 0)
        {
            PUTPIXEL_1TO24(dst, 0, pPalette);
            PUTPIXEL_1TO24(dst, 0, pPalette);
            PUTPIXEL_1TO24(dst, 0, pPalette);
            PUTPIXEL_1TO24(dst, 0, pPalette);
            PUTPIXEL_1TO24(dst, 0, pPalette);
            PUTPIXEL_1TO24(dst, 0, pPalette);
            PUTPIXEL_1TO24(dst, 0, pPalette);
            PUTPIXEL_1TO24(dst, 0, pPalette);
        }
        else if (*src == 0xFF)
        {
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
        }
        else
        {
            PUTPIXEL_1TO24(dst, (*src & 0x80) >> 7, pPalette);
            PUTPIXEL_1TO24(dst, (*src & 0x40) >> 6, pPalette);
            PUTPIXEL_1TO24(dst, (*src & 0x20) >> 5, pPalette);
            PUTPIXEL_1TO24(dst, (*src & 0x10) >> 4, pPalette);
            PUTPIXEL_1TO24(dst, (*src & 0x08) >> 3, pPalette);
            PUTPIXEL_1TO24(dst, (*src & 0x04) >> 2, pPalette);
            PUTPIXEL_1TO24(dst, (*src & 0x02) >> 1, pPalette);
            PUTPIXEL_1TO24(dst, (*src & 0x01) >> 0, pPalette);
        }
        
        src++;
    }
    
    srctailbyte = *src;
    while ((taillen)--)
    {
        PUTPIXEL_1TO24(dst, srctailbyte >> 7 , pPalette);
        srctailbyte <<= 1;
    }
}

#define PUTPIXEL_T_1TO24(dst, index, p) {   \
if (index != 0)     \
   PUTPIXEL(dst, p[index]);    \
(BYTE*)(dst)+=3;    \
}

void BYTEBLOCK_COPYLINE_T_1TO24(BYTE* dst, BYTE* src, PCOPYLINE1TO24STRUCT p, 
                                DWORD* pPalette)
{
    int blocklen = p->blocklen;
    int taillen = p->taillen;
    int headlen = p->headlen;
    BYTE srcheadbyte;
    BYTE srctailbyte;

    srcheadbyte = *src << p->headoffset;
    if (headlen != 0)
    {
        while ((headlen)--)
        {
            PUTPIXEL_T_1TO24(dst, srcheadbyte >> 7 , pPalette);
            srcheadbyte <<= 1;
        }
        ++(src);
    } 
    
    while ((blocklen)--)
    {
        if (*src == 0)
        {
            dst += 24;
        }
        else if (*src == 0xFF)
        {
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
            PUTPIXEL_1TO24(dst, 1, pPalette);
        }
        else
        {
            PUTPIXEL_T_1TO24(dst, (*src & 0x80) >> 7, pPalette);
            PUTPIXEL_T_1TO24(dst, (*src & 0x40) >> 6, pPalette);
            PUTPIXEL_T_1TO24(dst, (*src & 0x20) >> 5, pPalette);
            PUTPIXEL_T_1TO24(dst, (*src & 0x10) >> 4, pPalette);
            PUTPIXEL_T_1TO24(dst, (*src & 0x08) >> 3, pPalette);
            PUTPIXEL_T_1TO24(dst, (*src & 0x04) >> 2, pPalette);
            PUTPIXEL_T_1TO24(dst, (*src & 0x02) >> 1, pPalette);
            PUTPIXEL_T_1TO24(dst, (*src & 0x01) >> 0, pPalette);
        }
        
        src++;
    }
    
    srctailbyte = *src;
    while ((taillen)--)
    {
        PUTPIXEL_T_1TO24(dst, srctailbyte >> 7 , pPalette);
        srctailbyte <<= 1;
    }
}

void Get1to24CopyLineType(BYTE* dst, BYTE* src, int x, int pixlen, 
                COPYLINE1TO24STRUCT *p, DWORD* pPalette, BOOL bTransparent)
{
    p->headoffset = x & (BITSPERBYTES - 1);
    if (p->headoffset != 0)
        p->headlen = (BITSPERBYTES - p->headoffset);
    else
        p->headlen = 0;

    if (p->headlen >= pixlen)
    {
        p->headlen = pixlen;
        p->taillen = 0;
    }
    else
        p->taillen = (x + pixlen) & (BITSPERBYTES - 1);

    pixlen -= p->headlen + p->taillen;

    p->blocklen = (WORD)(pixlen / (BLOCKSIZE / 2));

    if (bTransparent)
    {
        p->copylinefunc = BYTEBLOCK_COPYLINE_T_1TO24;
        return;
    }

#if (DWORD_ACCESS_1)
    p->copylinefunc = DWORDBLOCK_COPYLINE_1TO24;
#else
    (BYTE*)(dst) += p->headlen * 3;

    if (((DWORD)(dst) & (sizeof(DWORD) - 1)) == 0)
        p->copylinefunc = DWORDBLOCK_COPYLINE_1TO24;
    else
        p->copylinefunc = BYTEBLOCK_COPYLINE_1TO24;
#endif

    if (p->copylinefunc == DWORDBLOCK_COPYLINE_1TO24)
    {
        DWORD palette[2];
        DWORD tmp[2][6];
        int i;

#if (BIG_ENDIAN)
#if (LDD_MODE == DDM_RGB24_RGB)
        /* 0x0BGR -> 0x0RGB */
        palette[0] = 
            ((pPalette[0] & 0xFF0000) >> 16) | (pPalette[0] & 0xFF00) | 
            ((pPalette[0] & 0xFF) << 16);
        palette[1] = 
            ((pPalette[1] & 0xFF0000) >> 16) | (pPalette[1] & 0xFF00) | 
            ((pPalette[1] & 0xFF) << 16);

        tmp[0][0] = palette[0] <<  8;    //RGB0 //first pixel
        tmp[0][1] = palette[0] >> 16;    //000R //second
        tmp[0][2] = palette[0] << 16;    //GB00 //second
        tmp[0][3] = palette[0] >>  8;    //00RG //third
        tmp[0][4] = palette[0] << 24;    //B000 //third
        tmp[0][5] = palette[0] >>  0;    //0RGB //third
        
        tmp[1][0] = palette[1] <<  8;    //first pixel
        tmp[1][1] = palette[1] >> 16;    //second
        tmp[1][2] = palette[1] << 16;    //second
        tmp[1][3] = palette[1] >>  8;    //third
        tmp[1][4] = palette[1] << 24;    //third
        tmp[1][5] = palette[1] >>  0;    //third
#else
        palette[0] = pPalette[0];
        palette[1] = pPalette[1];
        
        tmp[0][0] = palette[0] <<  8;    //BGR0 //first
        tmp[0][1] = palette[0] >> 16;    //000B //second
        tmp[0][2] = palette[0] << 16;    //GR00 //second
        tmp[0][3] = palette[0] >>  8;    //00BG //third
        tmp[0][4] = palette[0] << 24;    //R000 //third
        tmp[0][5] = palette[0] >>  0;    //0BGR //third
        
        tmp[1][0] = palette[1] <<  8;    //first pixel
        tmp[1][1] = palette[1] >> 16;    //second
        tmp[1][2] = palette[1] << 16;    //second
        tmp[1][3] = palette[1] >>  8;    //third
        tmp[1][4] = palette[1] << 24;    //third
        tmp[1][5] = palette[1] >>  0;    //third
        
#endif

        for (i = 0; i < 0x0F; i++)
        {
            p->palette[i][0] = tmp[(i & 0x08) >> 3][0] | tmp[(i & 0x04) >> 2][1];
            p->palette[i][1] = tmp[(i & 0x04) >> 2][2] | tmp[(i & 0x02) >> 1][3];
            p->palette[i][2] = tmp[(i & 0x02) >> 1][4] | tmp[(i & 0x01) >> 0][5];
        }
        
#else //(BIG_ENDIAN)
#if (LDD_MODE == DDM_RGB24_RGB)
        palette[0] = pPalette[0];
        palette[1] = pPalette[1];
        
        tmp[0][0] = palette[0] << 24;    //R000 //second
        tmp[0][1] = palette[0] >>  0;    //0BGR //first
        tmp[0][2] = palette[0] << 16;    //GR00 //third
        tmp[0][3] = palette[0] >>  8;    //00BG //second
        tmp[0][4] = palette[0] <<  8;    //BGR0 //forth
        tmp[0][5] = palette[0] >> 16;    //000B //third
        
        tmp[1][0] = palette[1] << 24;    //second
        tmp[1][1] = palette[1] >>  0;    //first
        tmp[1][2] = palette[1] << 16;    //third
        tmp[1][3] = palette[1] >>  8;    //second
        tmp[1][4] = palette[1] <<  8;    //forth
        tmp[1][5] = palette[1] >> 16;    //third
#else
        /* 0x0BGR -> 0x0RGB */
        palette[0] = 
            ((pPalette[0] & 0xFF0000) >> 16) | (pPalette[0] & 0xFF00) | 
            ((pPalette[0] & 0xFF) << 16);
        palette[1] = 
            ((pPalette[1] & 0xFF0000) >> 16) | (pPalette[1] & 0xFF00) | 
            ((pPalette[1] & 0xFF) << 16);
        
        tmp[0][0] = palette[0] << 24;    //B000 //second
        tmp[0][1] = palette[0] >>  0;    //0RGB //first
        tmp[0][2] = palette[0] << 16;    //GB00 //third
        tmp[0][3] = palette[0] >>  8;    //00RG //second
        tmp[0][4] = palette[0] <<  8;    //RGB0 //forth
        tmp[0][5] = palette[0] >> 16;    //000R //third
        
        tmp[1][0] = palette[1] << 24;    //second
        tmp[1][1] = palette[1] >>  0;    //first
        tmp[1][2] = palette[1] << 16;    //third
        tmp[1][3] = palette[1] >>  8;    //second
        tmp[1][4] = palette[1] <<  8;    //forth
        tmp[1][5] = palette[1] >> 16;    //third
#endif
        for (i = 0; i <= 0x0F; i++)
        {
            p->palette[i][0] = tmp[(i & 0x08) >> 3][1] | tmp[(i & 0x04) >> 2][0];
            p->palette[i][1] = tmp[(i & 0x04) >> 2][3] | tmp[(i & 0x02) >> 1][2];
            p->palette[i][2] = tmp[(i & 0x02) >> 1][5] | tmp[(i & 0x01) >> 0][4];
        }
        
#endif
    }
}




#if (0)
typedef struct CopyLine24to24Struct
{
    WORD    headlen;
    WORD    blocklen;
    WORD    taillen;
    void    (*copylinefunc)(BYTE*, BYTE*, struct CopyLine8to24Struct*);
}COPYLINE24TO24STRUCT, *PCOPYLINE24TO24STRUCT;
/*********************************************************************\
* Function	   BLOCK_24TO24_COPYLINE
* Purpose      实现24位向24位色的转换行操作
* Params	   
* Return	 	   
* Remarks	
            认为24位色目的设备，象素地址必然2字节对齐。
            24位色设备，象素地址不定。
**********************************************************************/
//#define COLOR24TO24(r, g, b) \
//    ((((b) & 0xf8) >> 3) | (((g) & 0xfc) << 3) | (((r) & 0xf8) << 8))

//#define COLOR24TO24(r, g, b) \
//    ((Color24To24BTable[(b) >> 3]) | (Color24To24GTable[(g) >> 2]) | (Color24To24RTable[(r) >> 3]))
#define COLOR24TO24(r, g, b) \
    ((Color24To24BTable[(b)]) | (Color24To24GTable[(g)]) | (Color24To24RTable[(r)]))

#define COLOR24TO24(c) \
((((c) & 0xf800) >> 8) | (((c) & 0x07E0) << 5) | (((c) & 0x1F) << 19))

#if (LDD_MODE == DDM_RGB24_RGB)
#define PUTPIXEL_24TO24(dst, src)   \
{   \
    *((BYTE*)(dst))++ = *((BYTE*)(src) + 2), 
    *((BYTE*)(dst))++ = *((BYTE*)(src) + 1), 
    *((BYTE*)(dst))++ = *((BYTE*)(src));  \
    (src) += 3; \
}
#elif (LDD_MODE == DDM_RGB24_BGR)
#define PUTPIXEL_24TO24(dst, src)   \
{   \
    *((BYTE*)(dst))++ = *((BYTE*)(src))++,
    *((BYTE*)(dst))++ = *((BYTE*)(src))++,
    *((BYTE*)(dst))++ = *((BYTE*)(src))++,
}
#endif

void BLOCK_COPYLINE_24TO24(BYTE* dst, BYTE* src, PCOPYLINE24TO24STRUCT p)
{
    int headlen = p->headlen;
    int blocklen = p->blocklen;

    while ((headlen)--)
        PUTPIXEL_24TO24(dst, src);

    while ((blocklen)--)
    {
        PUTPIXEL_24TO24(dst, src);
        PUTPIXEL_24TO24(dst, src);
        PUTPIXEL_24TO24(dst, src);
        PUTPIXEL_24TO24(dst, src);
        PUTPIXEL_24TO24(dst, src);
        PUTPIXEL_24TO24(dst, src);
        PUTPIXEL_24TO24(dst, src);
        PUTPIXEL_24TO24(dst, src);
    }
}

void Get24to24CopyLineType(int pixlen, PCOPYLINE24TO24STRUCT p)
{
    p->blocklen = pixlen / (BLOCKSIZE / 2);
    pixlen %= (BLOCKSIZE / 2);
    p->headlen = pixlen;
    
    p->copylinefunc = BLOCK_COPYLINE_24TO24;
}
#endif
