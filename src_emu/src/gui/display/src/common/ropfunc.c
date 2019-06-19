/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display Driver
 *
 * Purpose  : Implement the raster operation.
 *            
\**************************************************************************/

ROPFUNC GetRopFunc(uint32 rop);

// Internal function prototypes
static uint32 SetDst0(uint32 src, uint32 dst);
static uint32 SrcAndDst(uint32 src, uint32 dst);
static uint32 SrcAndNDst(uint32 src, uint32 dst);
static uint32 Src(uint32 src, uint32 dst);
static uint32 NSrcAndDst(uint32 src, uint32 dst);
static uint32 Dst(uint32 src, uint32 dst);
static uint32 SrcXorDst(uint32 src, uint32 dst);
static uint32 SrcOrDst(uint32 src, uint32 dst);
static uint32 NSrcAndNDst(uint32 src, uint32 dst);
static uint32 NSrcXorDst(uint32 src, uint32 dst);
static uint32 NDst(uint32 src, uint32 dst);
static uint32 SrcOrNDst(uint32 src, uint32 dst);
static uint32 NSrc(uint32 src, uint32 dst);
static uint32 NSrcOrDst(uint32 src, uint32 dst);
static uint32 NSrcOrNDst(uint32 src, uint32 dst);
static uint32 SetDst1(uint32 src, uint32 dst);

// Rop Function List
static const ROPFUNC RopFuncList[] = 
{
    SetDst0,            /* 全0                                  */
    NSrcAndNDst,        /* 源操作对象的非与目的操作对象的非的与 */
    NSrcAndDst,         /* 源操作对象的非与目的操作对象的与     */
    NSrc,               /* 源操作对象的非                       */
    SrcAndNDst,         /* 源操作对象与目的操作对象的非的与     */
    NDst,               /* 目的操作对象的非                     */
    SrcXorDst,          /* 源操作对象与目的操作对象的异或       */
    NSrcOrNDst,         /* 源操作对象的非与目的操作对象的非的或 */
    SrcAndDst,          /* 源操作对象与目的操作对象的与         */
    NSrcXorDst,         /* 源操作对象的非与目的操作对象的异或   */
    Dst,                /* 目的操作对象                         */
    NSrcOrDst,          /* 源操作对象的非与目的操作对象的或     */
    Src,                /* 源操作对象                           */
    SrcOrNDst,          /* 源操作对象与目的操作对象的非的或     */
    SrcOrDst,           /* 源操作对象与目的操作对象的或         */
    SetDst1            /* 全1                                  */

};

#define ROP_HAS_SRC(rop) (0 != (0x330000L & ((int)rop ^ ((int)rop >> 2))))
#define ROP_HAS_PAT(rop) (0 != (0x0F0000L & ((int)rop ^ ((int)rop >> 4))))
#define ROP_HAS_DST(rop) (0 != (0x550000L & ((int)rop ^ ((int)rop >> 1))))

/*********************************************************************\
* Function	  SROP3ToROP2 
* Purpose     Converts no pattern rop3 to rop2 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
uint32 SRop3ToRop2(uint32 rop)
{
    ASSERT(rop >= 1);

    if (rop > 16)
    {
        if (!ROP_HAS_PAT(rop))
            return ((rop >> 16) & 0x0f) + 1;
        
        return ROP_SRC;
    }

    return rop;
}

/*********************************************************************\
* Function	  PROP3ToROP2 
* Purpose     Converts no src rop3 to rop2 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
uint32 PRop3ToRop2(uint32 rop)
{
    ASSERT(rop >= 1);

    if (rop > 16)
    {
        if (!ROP_HAS_SRC(rop))
        {
            rop >>= 16;
            return (((rop >> 4) & 0x0C) | (rop & 0x03)) + 1;
        }
        
        return ROP_SRC;
    }

    return rop;
}

/*********************************************************************\
* Function	GetRopFunc   
* Purpose   Get the rop function for the specified raster operation code.   
* Params	  
    byRop   : Specifies the raster operation code. 
* Return	The rop function pointer.	   
* Remarks	   
**********************************************************************/
ROPFUNC GetRopFunc(uint32 rop)
{
    ASSERT(rop >= 1 && rop <= 16);
    return RopFuncList[rop - 1];
}

// Intenal functions

static uint32 SetDst0(uint32 src, uint32 dst)
{
    return 0;
}

static uint32 SrcAndDst(uint32 src, uint32 dst)
{
    return src & dst;
}

static uint32 SrcAndNDst(uint32 src, uint32 dst)
{
    return src & ~dst;
}

static uint32 Src(uint32 src, uint32 dst)
{
    return src;
}

static uint32 NSrcAndDst(uint32 src, uint32 dst)
{
    return ~src & dst;
}

static uint32 Dst(uint32 src, uint32 dst)
{
    return dst;
}

static uint32 SrcXorDst(uint32 src, uint32 dst)
{
    return src ^ dst;
}

static uint32 SrcOrDst(uint32 src, uint32 dst)
{
    return src| dst;
}

static uint32 NSrcAndNDst(uint32 src, uint32 dst)
{
    return ~src & ~dst;
}

static uint32 NSrcXorDst(uint32 src, uint32 dst)
{
    return ~src ^ dst;
}

static uint32 NDst(uint32 src, uint32 dst)
{
    return ~dst;
}

static uint32 SrcOrNDst(uint32 src, uint32 dst)
{
    return src | ~dst;
}

static uint32 NSrc(uint32 src, uint32 dst)
{
    return ~src;
}

static uint32 NSrcOrDst(uint32 src, uint32 dst)
{
    return ~src | dst;
}

static uint32 NSrcOrNDst(uint32 src, uint32 dst)
{
    return ~src | ~dst;
}

static uint32 SetDst1(uint32 src, uint32 dst)
{
    return 0xFFFFFFFFL;
}

//the below two array is used to convert rop2 to rop3
/* ROP code table used for pen */
static const uint32 Rop2_Table_SD[16] = 
{
    0x00000042L,     // ROP_SET_DST_0
    0x001100A6L,     // ROP_NSRC_AND_NDST
    0x00220326L,     // ROP_NSRC_AND_DST
    0x00330008L,     // ROP_NSRC
    0x00440328L,     // ROP_SRC_AND_NDST
    0x00550009L,     // ROP_NDST
    0x00660046L,     // ROP_SRC_XOR_DST
    0x007700E6L,     // ROP_NSRC_OR_NDST
    0x008800C6L,     // ROP_SRC_AND_DST
    0x00990066L,     // ROP_NSRC_XOR_DST
    0x00AA0029L,     // ROP_DST
    0x00BB0226L,     // ROP_NSRC_OR_DST
    0x00CC0020L,     // ROP_SRC
    0x00DD0228L,     // ROP_SRC_OR_NDST
    0x00EE0086L,     // ROP_SRC_OR_DST
    0x00FF0062L,     // ROP_SET_DST_1
};

/* ROP code table used for pattern blt */
static const uint32 Rop2_Table_PD[16] = 
{
    0x00000042L,     //DO_ROP_SET_DST_0
    0x000500A9L,     //DO_ROP_NSRC_AND_NDST,
    0x000a0329L,     //DO_ROP_NSRC_AND_DST,
    0x000f0001L,     //DO_ROP_NSRC,
    0x00500325L,     //DO_ROP_SRC_AND_NDST,
    0x00550009L,     //DO_ROP_NDST,
    0x005a0049L,     //DO_ROP_SRC_XOR_DST,
    0x005f00e9L,     //DO_ROP_NSRC_OR_NDST,
    0x00a000c9L,     //DO_ROP_SRC_AND_DST
    0x00a50065L,     //DO_ROP_NSRC_XOR_DST,
    0x00aa0029L,     //DO_ROP_DST,
    0x00af0229L,     //DO_ROP_NSRC_OR_DST,
    0x00f00021L,     //DO_ROP_SRC,
    0x00f50225L,     //DO_ROP_SRC_OR_NDST,
    0x00fa0089L,     //DO_ROP_SRC_OR_DST,
    0x00FF0062L      //DO_ROP_SET_DST_1 };
};
