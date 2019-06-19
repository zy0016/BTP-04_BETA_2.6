/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display Driver
 *
 * Purpose  : Define the function prototype in the display driver.
 *            
\**************************************************************************/

#ifndef __DISPCOMN_H
#define __DISPCOMN_H

/* The following functions are implemented in ropfunc.c  */
typedef uint32 (*ROPFUNC)(uint32, uint32);
ROPFUNC GetRopFunc(uint32 rop);
uint32 SRop3ToRop2(uint32 rop);
uint32 PRop3ToRop2(uint32 rop);

/* The following functions are implemented in scanline.c */
typedef struct edgelist{
	int     x1, y1, x2, y2;
	int     cx, flag, dx, dy;
    BYTE    ctrl;
    struct edgelist* pNext;
} EDGE, *PEDGE;

BOOL InsertEdge(PEDGE *ppEdgeList, PEDGE pEdge);
BOOL InsertEdgeReverse(PEDGE *ppEdgeList, PEDGE pEdge);
void SortActiveEdge(PEDGE* ppEdgeList);


/* The following functions are implemented in lineblt.c */
#define DWORDBLOCK_COPY     0x00000000
#define DWORD_COPY          0x00000001
#define WORDBLOCK_COPY      0x00000002
#define WORD_COPY           0x00000003
#define BYTE_COPY           0x00000004

#define R2L_COPY            0x00000100

#define ROP_COPY            0
#define SRC_COPY            1
#define XORSD_COPY    2
#define ANDSD_COPY    3

#define MAX_FUNCTYPE        10
#define GETFUNCINDEX(type)  ((type) & 0x000000FF)

typedef struct CopyLineStruct
{
    DWORD   type;
    WORD    headbytelen;
    WORD    tailbytelen;
    WORD    tailwordlen;
    WORD    taildwordlen;
    WORD    wordlen;
    WORD    dwordlen;
    WORD    blocklen;
    WORD    reserved;
    BYTE    *dst;
    BYTE    *src;
    ROPFUNC ropfunc;
    void    (*copylinefunc)(struct CopyLineStruct* p);
}COPYLINESTRUCT, *PCOPYLINESTRUCT;

typedef void (*COPYLINEFUNC)(PCOPYLINESTRUCT);

#define DWORD_ACCESS_1  0  //DWORD can be accessed from any address
#define DWORD_ACCESS_2  0  //DWORD can be accessed from WORD aligned address
#define DWORD_ACCESS_4  1  //DWORD can be accessed from DWORD aligned address

#define ALIGNED_DWORD(X, Y) \
(((DWORD)(X) & (sizeof(DWORD) - 1)) == ((DWORD)(Y) & (sizeof (DWORD) - 1)))

#define ALIGNED_WORD(X, Y) \
(((DWORD)(X) & (sizeof(WORD) - 1)) == ((DWORD)(Y) & (sizeof(WORD) - 1)))

/* How many bytes are copied each iteration of the 4X unrolled loop.  */
#define BLOCKSIZE    (sizeof(DWORD) << 2)

#define COPYLINE_L2R    1
#define COPYLINE_R2L    2

/* GETHEADBYTES得到首先需要拷贝的单字节数，size表示将使用的块拷贝单位 */
#define GETHEADBYTES(dst, size, dir)  \
    (((dir) == COPYLINE_R2L) ? \
    ((DWORD)(dst) & ((size) - 1)) : \
    (((size) - ((DWORD)(dst) & ((size) - 1))) & ((size) - 1)))

void GetCopyLineType(BYTE* dst, BYTE* src, int len, PCOPYLINESTRUCT p, 
                     int dir, DWORD rop);

/* The following functions are implemented in fillline.c */

typedef struct FillLineStruct
{
    WORD    headlen;
    WORD    taillen;
    WORD    taildwordlen;
    WORD    blocklen;
    WORD    pixbytes;
    BYTE    rgbcolor[3];
    DWORD   color;
    DWORD   color24[3];
    BYTE    *dst;
    ROPFUNC ropfunc;
    void    (*filllinefunc)(struct FillLineStruct* p);
}FILLLINESTRUCT, *PFILLLINESTRUCT;

typedef void (*FILLLINEFUNC)(PFILLLINESTRUCT);

#define BLOCKSIZE_16    (sizeof(DWORD) << 2)
#define BLOCKSIZE_24    (sizeof(DWORD) * 3)
#define CYCLEMOVEBITS32_LEFT(a, n)  (((a) << (n)) | ((a) >> (32 - n)))
#define CYCLEMOVEBITS32_RIGHT(a, n) (((a) >> (n)) | ((a) << (32 - n))) 

#define GETR(c) ((c) & 0x000000FFL)
#define GETG(c) (((c) & 0x0000FF00L) >> 8)
#define GETB(c) (((c) & 0x00FF0000L) >> 16)

void GetFillLineType(BYTE* dst, DWORD color, int len, int pixbytes, 
                     PFILLLINESTRUCT p, DWORD rop, DWORD fillmode);
/* fillmode */
#define FILL_LITTLE_BGR 0
#define FILL_LITTLE_RGB 1
#define FILL_BIG_BGR    2
#define FILL_BIG_RGB    3

#endif // __DISPCOMN_H
