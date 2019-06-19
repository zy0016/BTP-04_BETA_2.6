/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display driver
 *
 * Purpose  : Implement the blt n to 16 functions.
 *            
\**************************************************************************/


/* GETHEADBYTES�õ�������Ҫ�����ĵ��ֽ�����size��ʾ��ʹ�õĿ鿽����λ */
#define GETHEADBYTES(dst, size, dir)  \
    (((dir) == COPYLINE_R2L) ? \
    ((DWORD)(dst) & ((size) - 1)) : \
    (((size) - ((DWORD)(dst) & ((size) - 1))) & ((size) - 1)))

typedef struct CopyLine8to16Struct
{
    WORD    headlen;
    WORD    blocklen;
    WORD    taillen;
    void    (*copylinefunc)(BYTE*, BYTE*, struct CopyLine8to16Struct*,DWORD*);
}COPYLINE8TO16STRUCT, *PCOPYLINE8TO16STRUCT;

/*********************************************************************\
* Function	   BLOCK_8TO16_COPYLINE
* Purpose      ʵ��8λ��ɫ����16λɫ��ת���в���
* Params	   
            dst:        Ŀ�ĵ�ַ������ĳ������ʼ��ַ
            src:        Դ��ַ
            blocklen:   ����������16�ֽ�Ϊһ��, 8������
            headlen:    ��ʼ�����ظ���������Ϊ0��1
            pPalette:   �����ɫ�����
* Return	 	   
* Remarks	
            ��Ϊ16λɫĿ���豸�����ص�ַ��Ȼ2�ֽڶ��롣
**********************************************************************/
#define PUTPIXEL_8TO16(dst, src, p)   *(WORD*)(dst)++ = (WORD)p[*(src)++]
void BLOCK_COPYLINE_8TO16(BYTE* dst, BYTE* src, PCOPYLINE8TO16STRUCT p, 
                    DWORD* pPalette)
{
    int headlen = p->headlen;
    int blocklen = p->blocklen;
    
    while ((headlen)--)
        PUTPIXEL_8TO16(dst, src, pPalette);
    
    while ((blocklen)--)
    {
        PUTPIXEL_8TO16(dst, src, pPalette);
        PUTPIXEL_8TO16(dst, src, pPalette);
        PUTPIXEL_8TO16(dst, src, pPalette);
        PUTPIXEL_8TO16(dst, src, pPalette);
        PUTPIXEL_8TO16(dst, src, pPalette);
        PUTPIXEL_8TO16(dst, src, pPalette);
        PUTPIXEL_8TO16(dst, src, pPalette);
        PUTPIXEL_8TO16(dst, src, pPalette);
    }
}

void Get8to16CopyLineType(int pixlen, PCOPYLINE8TO16STRUCT p)
{
    p->blocklen = (WORD)(pixlen / (BLOCKSIZE / 2));
    pixlen %= (BLOCKSIZE / 2);
    p->headlen = pixlen;
    
    p->copylinefunc = BLOCK_COPYLINE_8TO16;
}

typedef struct CopyLine1to16Struct
{
    WORD    headlen;
    WORD    blocklen;
    WORD    taillen;
    WORD    headoffset; //�����β����һ���ֽ��ڣ������ֽ�β��Чλ��
    void    (*copylinefunc)(BYTE*, BYTE*, struct CopyLine1to16Struct*, DWORD*);
    DWORD   palette[4];
}COPYLINE1TO16STRUCT, *PCOPYLINE1TO16STRUCT;

#define BITSPERBYTES    (8)
/*********************************************************************\
* Function	   BLOCK_1TO16_COPYLINE
* Purpose      ʵ��1λ��16λɫ��ת���в���
* Params	   
            dst:        Ŀ�ĵ�ַ������ĳ������ʼ��ַ
            src:        Դ��ַ
            blocklen:   ����������16�ֽ�Ϊһ��, 8�����أ�Դһ���ֽ�
            headlen:    ��ʼ�����ظ���������Ϊ0-7, Դ�ĵ�һ���ֽ�����Чλ��
            taillen:    ��ֹ�����ظ���������Ϊ0-7, Դ�����һ���ֽ���Чλ��
            head:       ��00011111��ʽ��ŵ����ֽ�
            tail:       ��00001111��ʽ��ŵ�β�ֽ�
            pPalette:   �����ɫ�����
* Return	 	   
* Remarks	
            ��Ϊ16λɫĿ���豸�����ص�ַ��Ȼ2�ֽڶ��롣
            1λɫ�豸�����ص�ַ������
/**********************************************************************/
#define PUTPIXEL_1TO16(dst, index, p) *((WORD*)(dst))++ = (WORD)(p)[index]
#define PUTDWORD_1TO16(dst, index, p) *((DWORD*)(dst))++ = (DWORD)(p)[index]
void DWORDBLOCK_COPYLINE_1TO16(BYTE* dst, BYTE* src, PCOPYLINE1TO16STRUCT p, 
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
            PUTPIXEL_1TO16(dst, srcheadbyte >> 7 , pPalette);
            srcheadbyte <<= 1;
        }
        ++(src);
    }
    
    while ((blocklen)--)
    {
        if (*src == 0)
        {
            PUTDWORD_1TO16(dst, 0, p->palette);
            PUTDWORD_1TO16(dst, 0, p->palette);
            PUTDWORD_1TO16(dst, 0, p->palette);
            PUTDWORD_1TO16(dst, 0, p->palette);
        }
        else if (*src == 0xFF)
        {
            PUTDWORD_1TO16(dst, 0x03, p->palette);
            PUTDWORD_1TO16(dst, 0x03, p->palette);
            PUTDWORD_1TO16(dst, 0x03, p->palette);
            PUTDWORD_1TO16(dst, 0x03, p->palette);
        }
        else
        {
            PUTDWORD_1TO16(dst, (*src & 0xC0) >> 6, p->palette);
            PUTDWORD_1TO16(dst, (*src & 0x30) >> 4, p->palette);
            PUTDWORD_1TO16(dst, (*src & 0x0C) >> 2, p->palette);
            PUTDWORD_1TO16(dst, (*src & 0x03) >> 0, p->palette);
        }

        src++;
    }

    srctailbyte = *src;
    while ((taillen)--)
    {
        PUTPIXEL_1TO16(dst, srctailbyte >> 7 , pPalette);
        srctailbyte <<= 1;
    }
}

void WORDBLOCK_COPYLINE_1TO16(BYTE* dst, BYTE* src, PCOPYLINE1TO16STRUCT p, 
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
            PUTPIXEL_1TO16(dst, srcheadbyte >> 7 , pPalette);
            srcheadbyte <<= 1;
        }
        ++(src);
    }
    
    while ((blocklen)--)
    {
        if (*src == 0)
        {
            PUTPIXEL_1TO16(dst, 0, pPalette);
            PUTPIXEL_1TO16(dst, 0, pPalette);
            PUTPIXEL_1TO16(dst, 0, pPalette);
            PUTPIXEL_1TO16(dst, 0, pPalette);
            PUTPIXEL_1TO16(dst, 0, pPalette);
            PUTPIXEL_1TO16(dst, 0, pPalette);
            PUTPIXEL_1TO16(dst, 0, pPalette);
            PUTPIXEL_1TO16(dst, 0, pPalette);
        }
        else if (*src == 0xFF)
        {
            PUTPIXEL_1TO16(dst, 1, pPalette);
            PUTPIXEL_1TO16(dst, 1, pPalette);
            PUTPIXEL_1TO16(dst, 1, pPalette);
            PUTPIXEL_1TO16(dst, 1, pPalette);
            PUTPIXEL_1TO16(dst, 1, pPalette);
            PUTPIXEL_1TO16(dst, 1, pPalette);
            PUTPIXEL_1TO16(dst, 1, pPalette);
            PUTPIXEL_1TO16(dst, 1, pPalette);
        }
        else
        {
            PUTPIXEL_1TO16(dst, (*src & 0x80) >> 7, pPalette);
            PUTPIXEL_1TO16(dst, (*src & 0x40) >> 6, pPalette);
            PUTPIXEL_1TO16(dst, (*src & 0x20) >> 5, pPalette);
            PUTPIXEL_1TO16(dst, (*src & 0x10) >> 4, pPalette);
            PUTPIXEL_1TO16(dst, (*src & 0x08) >> 3, pPalette);
            PUTPIXEL_1TO16(dst, (*src & 0x04) >> 2, pPalette);
            PUTPIXEL_1TO16(dst, (*src & 0x02) >> 1, pPalette);
            PUTPIXEL_1TO16(dst, (*src & 0x01) >> 0, pPalette);
        }
        
        src++;
    }
    
    srctailbyte = *src;
    while ((taillen)--)
    {
        PUTPIXEL_1TO16(dst, srctailbyte >> 7 , pPalette);
        srctailbyte <<= 1;
    }
}

#define PUTPIXEL_T_1TO16(dst, index, p) \
{if (index != 0) *((WORD*)(dst)) = (WORD)(p)[index]; ++(WORD*)(dst);}

void WORDBLOCK_COPYLINE_T_1TO16(BYTE* dst, BYTE* src, PCOPYLINE1TO16STRUCT p, 
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
            PUTPIXEL_T_1TO16(dst, srcheadbyte >> 7 , pPalette);
            srcheadbyte <<= 1;
        }
        ++(src);
    }
    
    while ((blocklen)--)
    {
        if (*src == 0)
        {
            dst += 16;
        }
        else if (*src == 0xFF)
        {
            PUTPIXEL_T_1TO16(dst, 1, pPalette);
            PUTPIXEL_T_1TO16(dst, 1, pPalette);
            PUTPIXEL_T_1TO16(dst, 1, pPalette);
            PUTPIXEL_T_1TO16(dst, 1, pPalette);
            PUTPIXEL_T_1TO16(dst, 1, pPalette);
            PUTPIXEL_T_1TO16(dst, 1, pPalette);
            PUTPIXEL_T_1TO16(dst, 1, pPalette);
            PUTPIXEL_T_1TO16(dst, 1, pPalette);
        }
        else
        {
            PUTPIXEL_T_1TO16(dst, (*src & 0x80) >> 7, pPalette);
            PUTPIXEL_T_1TO16(dst, (*src & 0x40) >> 6, pPalette);
            PUTPIXEL_T_1TO16(dst, (*src & 0x20) >> 5, pPalette);
            PUTPIXEL_T_1TO16(dst, (*src & 0x10) >> 4, pPalette);
            PUTPIXEL_T_1TO16(dst, (*src & 0x08) >> 3, pPalette);
            PUTPIXEL_T_1TO16(dst, (*src & 0x04) >> 2, pPalette);
            PUTPIXEL_T_1TO16(dst, (*src & 0x02) >> 1, pPalette);
            PUTPIXEL_T_1TO16(dst, (*src & 0x01) >> 0, pPalette);
        }
        
        src++;
    }
    
    srctailbyte = *src;
    while ((taillen)--)
    {
        PUTPIXEL_T_1TO16(dst, srctailbyte >> 7 , pPalette);
        srctailbyte <<= 1;
    }
}

void Get1to16CopyLineType(BYTE* dst, BYTE* src, int x, int pixlen, 
                COPYLINE1TO16STRUCT *p, DWORD* pPalette, BOOL bTransparent)
{
    p->headoffset = x & (BITSPERBYTES - 1);
    p->headlen = (BITSPERBYTES - p->headoffset) % BITSPERBYTES;

    if (p->headlen >= pixlen)
    {
        p->headlen = pixlen;
        p->taillen = 0;
        p->blocklen = 0;
    }
    else
    {
        p->taillen = (x + pixlen) & (BITSPERBYTES - 1);
        pixlen -= p->headlen + p->taillen;
        p->blocklen = (WORD)(pixlen / (BLOCKSIZE / 2));
    }

    if (bTransparent)
    {
        p->copylinefunc = WORDBLOCK_COPYLINE_T_1TO16;
        return;
    }

#if (DWORD_ACCESS_1)
    p->copylinefunc = DWORDBLOCK_COPYLINE_1TO16;
#elif(DWORD_ACCESS_2)
    p->copylinefunc = DWORDBLOCK_COPYLINE_1TO16;
#else
    (WORD*)(dst) += p->headlen;
    if (((DWORD)(dst) & (sizeof(DWORD) - 1)) != 0)
        p->copylinefunc = WORDBLOCK_COPYLINE_1TO16;
    else
        p->copylinefunc = DWORDBLOCK_COPYLINE_1TO16;
#endif

    if (p->copylinefunc == DWORDBLOCK_COPYLINE_1TO16)
    {
#if (BIG_ENDIAN)
        p->palette[0] = ((pPalette[0] << 16) | (pPalette[0] & 0x0000FFFF));
        p->palette[1] = ((pPalette[0] << 16) | (pPalette[1] & 0x0000FFFF));
        p->palette[2] = ((pPalette[1] << 16) | (pPalette[0] & 0x0000FFFF));
        p->palette[3] = ((pPalette[1] << 16) | (pPalette[1] & 0x0000FFFF));
#else
        p->palette[0] = ((pPalette[0] << 16) | (pPalette[0] & 0x0000FFFF));
        p->palette[1] = ((pPalette[1] << 16) | (pPalette[0] & 0x0000FFFF));
        p->palette[2] = ((pPalette[0] << 16) | (pPalette[1] & 0x0000FFFF));
        p->palette[3] = ((pPalette[1] << 16) | (pPalette[1] & 0x0000FFFF));
#endif
    }
}
