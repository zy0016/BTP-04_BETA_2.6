/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/



static void DWORDBLOCK_L2R_SRCCOPYLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int taildwordlen = p->taildwordlen;

    while ((headbytelen)--)
        *((BYTE*)(dst))++ = *((BYTE*)(src))++;

    while ((blocklen)--)
    {
        *((DWORD*)(dst))++ = *((DWORD*)(src))++;
        *((DWORD*)(dst))++ = *((DWORD*)(src))++;
        *((DWORD*)(dst))++ = *((DWORD*)(src))++;
        *((DWORD*)(dst))++ = *((DWORD*)(src))++;
    }

    while ((taildwordlen)--)
        *((DWORD*)(dst))++ = *((DWORD*)(src))++;

    while ((tailbytelen)--)
        *((BYTE*)(dst))++ = *((BYTE*)(src))++;
}

static void DWORDBLOCK_R2L_SRCCOPYLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int taildwordlen = p->taildwordlen;

    while ((headbytelen)--)
        *(--(BYTE*)(dst)) = *(--((BYTE*)(src)));
    
    while ((blocklen)--)
    {
        *(--(DWORD*)(dst)) = *(--(DWORD*)(src));
        *(--(DWORD*)(dst)) = *(--(DWORD*)(src));
        *(--(DWORD*)(dst)) = *(--(DWORD*)(src));
        *(--(DWORD*)(dst)) = *(--(DWORD*)(src));
    }
    
    while ((taildwordlen)--)
        *(--(DWORD*)(dst)) = *(--(DWORD*)(src));
    
    while ((tailbytelen)--)
        *(--(BYTE*)(dst)) = *(--(BYTE*)(src));
}

static void DWORD_L2R_SRCCOPYLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int dwordlen = p->dwordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;

    while ((headbytelen)--)
        *((BYTE*)(dst))++ = *((BYTE*)(src))++;

    while ((dwordlen)--)
        *((DWORD*)dst)++ = *((DWORD*)src)++;

    while ((tailbytelen)--)
        *((BYTE*)(dst))++ = *((BYTE*)(src))++;
}

static void DWORD_R2L_SRCCOPYLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int dwordlen = p->dwordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;

    while ((headbytelen)--)
        *(--(BYTE*)(dst)) = *(--(BYTE*)(src));
    
    while ((dwordlen)--)
        *(--(DWORD*)dst) = *(--(DWORD*)src);
    
    while ((tailbytelen)--)
        *(--(BYTE*)(dst)) = *(--(BYTE*)(src));
}

static void WORDBLOCK_L2R_SRCCOPYLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int tailwordlen = p->tailwordlen;

    while ((headbytelen)--)
        *((BYTE*)(dst))++ = *((BYTE*)(src))++;
    
    while ((blocklen)--)
    {
        *((WORD*)(dst))++ = *((WORD*)(src))++;
        *((WORD*)(dst))++ = *((WORD*)(src))++;
        *((WORD*)(dst))++ = *((WORD*)(src))++;
        *((WORD*)(dst))++ = *((WORD*)(src))++;
        *((WORD*)(dst))++ = *((WORD*)(src))++;
        *((WORD*)(dst))++ = *((WORD*)(src))++;
        *((WORD*)(dst))++ = *((WORD*)(src))++;
        *((WORD*)(dst))++ = *((WORD*)(src))++;
    }
    
    while ((tailwordlen)--)
        *((WORD*)(dst))++ = *((WORD*)(src))++;
    
    while ((tailbytelen)--)
        *((BYTE*)(dst))++ = *((BYTE*)(src))++;
}

static void WORDBLOCK_R2L_SRCCOPYLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int tailwordlen = p->tailwordlen;

    while ((headbytelen)--)
        *(--(BYTE*)(dst)) = *(--(BYTE*)(src));
    
    while ((blocklen)--)
    {
        *(--(WORD*)(dst)) = *(--(WORD*)(src));
        *(--(WORD*)(dst)) = *(--(WORD*)(src));
        *(--(WORD*)(dst)) = *(--(WORD*)(src));
        *(--(WORD*)(dst)) = *(--(WORD*)(src));
        *(--(WORD*)(dst)) = *(--(WORD*)(src));
        *(--(WORD*)(dst)) = *(--(WORD*)(src));
        *(--(WORD*)(dst)) = *(--(WORD*)(src));
        *(--(WORD*)(dst)) = *(--(WORD*)(src));
    }
    
    while ((tailwordlen)--)
        *(--(WORD*)(dst)) = *(--(WORD*)(src));
    
    while ((tailbytelen)--)
        *(--(BYTE*)(dst)) = *(--(BYTE*)(src));
}

static void WORD_L2R_SRCCOPYLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int wordlen = p->wordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;

    while ((headbytelen)--)
        *((BYTE*)(dst))++ = *((BYTE*)(src))++;
    
    while ((wordlen)--)
        *((WORD*)(dst))++ = *((WORD*)(src))++;
    
    while ((tailbytelen)--)
        *((BYTE*)(dst))++ = *((BYTE*)(src))++;
}

static void WORD_R2L_SRCCOPYLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int wordlen = p->wordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    
    while ((headbytelen)--)
        *(--(BYTE*)(dst)) = *(--(BYTE*)(src));
    
    while ((wordlen)--)
        *(--(WORD*)(dst)) = *(--(WORD*)(src));
    
    while ((tailbytelen)--)
        *(--(BYTE*)(dst)) = *(--(BYTE*)(src));
}

static void BYTE_L2R_SRCCOPYLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int headbytelen = p->headbytelen;
    
    while ((headbytelen)--)
        *((BYTE*)(dst))++ = *((BYTE*)(src))++;
}

static void BYTE_R2L_SRCCOPYLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int headbytelen = p->headbytelen;

    while ((headbytelen)--)
        *(--(BYTE*)(dst)) = *(--(BYTE*)(src));
}

static void DWORDBLOCK_L2R_ROPFUNCLINE(PCOPYLINESTRUCT p)
{
    BYTE* dst = p->dst;
    BYTE* src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int taildwordlen = p->taildwordlen;
    ROPFUNC ropfunc = p->ropfunc;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = (BYTE)ropfunc(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
    
    while ((blocklen)--)
    {
        *((DWORD*)(dst)) = ropfunc(*((DWORD*)(src))++, *((DWORD*)(dst)));
        ++(DWORD*)(dst);
        *((DWORD*)(dst)) = ropfunc(*((DWORD*)(src))++, *((DWORD*)(dst)));
        ++(DWORD*)(dst);
        *((DWORD*)(dst)) = ropfunc(*((DWORD*)(src))++, *((DWORD*)(dst)));
        ++(DWORD*)(dst);
        *((DWORD*)(dst)) = ropfunc(*((DWORD*)(src))++, *((DWORD*)(dst)));
        ++(DWORD*)(dst);
    }
    
    while ((taildwordlen)--)
    {
        *((DWORD*)(dst)) = ropfunc(*((DWORD*)(src))++, *((DWORD*)(dst)));
        ++(DWORD*)(dst);
    }

    while ((tailbytelen)--)
    {
        *((BYTE*)(dst)) = (BYTE)ropfunc(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
}

static void DWORDBLOCK_R2L_ROPFUNCLINE(PCOPYLINESTRUCT p)
{
    BYTE* dst = p->dst;
    BYTE* src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int taildwordlen = p->taildwordlen;
    ROPFUNC ropfunc = p->ropfunc;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = (BYTE)ropfunc(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
    
    while ((blocklen)--)
    {
        --(DWORD*)(dst);
        *((DWORD*)(dst)) = ropfunc(*(--(DWORD*)(src)), *((DWORD*)(dst)));
        --(DWORD*)(dst);
        *((DWORD*)(dst)) = ropfunc(*(--(DWORD*)(src)), *((DWORD*)(dst)));
        --(DWORD*)(dst);
        *((DWORD*)(dst)) = ropfunc(*(--(DWORD*)(src)), *((DWORD*)(dst)));
        --(DWORD*)(dst);
        *((DWORD*)(dst)) = ropfunc(*(--(DWORD*)(src)), *((DWORD*)(dst)));
    }
    
    while ((taildwordlen)--)
    {
        --(DWORD*)(dst);
        *((DWORD*)(dst)) = ropfunc(*(--(DWORD*)(src)), *((DWORD*)(dst)));
    }
    
    while ((tailbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = (BYTE)ropfunc(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
}

static void DWORD_L2R_ROPFUNCLINE(PCOPYLINESTRUCT p)
{
    BYTE* dst = p->dst;
    BYTE* src = p->src;
    int dwordlen = p->dwordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    ROPFUNC ropfunc = p->ropfunc;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = (BYTE)ropfunc(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }

    while ((dwordlen)--)
    {
        *((DWORD*)(dst)) = ropfunc(*((DWORD*)(src))++, *((DWORD*)(dst)));
        ++(DWORD*)(dst);
    }

    while ((tailbytelen)--)
    {
        *((BYTE*)(dst)) = (BYTE)ropfunc(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
}

static void DWORD_R2L_ROPFUNCLINE(PCOPYLINESTRUCT p)
{
    BYTE* dst = p->dst;
    BYTE* src = p->src;
    int dwordlen = p->dwordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    ROPFUNC ropfunc = p->ropfunc;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = (BYTE)ropfunc(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
    
    while ((dwordlen)--)
    {
        --(DWORD*)(dst);
        *((DWORD*)(dst)) = ropfunc(*(--(DWORD*)(src)), *((DWORD*)(dst)));
    }
    
    while ((tailbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = (BYTE)ropfunc(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
}

static void WORDBLOCK_L2R_ROPFUNCLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int tailwordlen = p->tailwordlen;
    ROPFUNC ropfunc = p->ropfunc;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = (BYTE)ropfunc(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
    
    while ((blocklen)--)
    {
        *((WORD*)(dst)) = (WORD)ropfunc(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
    }
    
    while ((tailwordlen)--)
    {
        *((WORD*)(dst)) = (WORD)ropfunc(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
    }
    
    while ((tailbytelen)--)
    {
        *((BYTE*)(dst)) = (BYTE)ropfunc(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
}

static void WORDBLOCK_R2L_ROPFUNCLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int tailwordlen = p->tailwordlen;
    ROPFUNC ropfunc = p->ropfunc;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = (BYTE)ropfunc(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
    
    while ((blocklen)--)
    {
        --(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*(--(WORD*)(src)), *((WORD*)(dst)));
    }
    
    while ((tailwordlen)--)
    {
        --(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*(--(WORD*)(src)), *((WORD*)(dst)));
    }
    
    while ((tailbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = (BYTE)ropfunc(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
}

static void WORD_L2R_ROPFUNCLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int wordlen = p->wordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    ROPFUNC ropfunc = p->ropfunc;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = (BYTE)ropfunc(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
    
    while ((wordlen)--)
    {
        *((WORD*)(dst)) = (WORD)ropfunc(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
    }
    
    while ((tailbytelen)--)
    {
        *((BYTE*)(dst)) = (BYTE)ropfunc(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
}

static void WORD_R2L_ROPFUNCLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int wordlen = p->wordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    ROPFUNC ropfunc = p->ropfunc;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = (BYTE)ropfunc(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
    
    while ((wordlen)--)
    {
        --(WORD*)(dst);
        *((WORD*)(dst)) = (WORD)ropfunc(*(--(WORD*)(src)), *((WORD*)(dst)));
    }
    
    while ((tailbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = (BYTE)ropfunc(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
}

static void BYTE_L2R_ROPFUNCLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int headbytelen = p->headbytelen;
    ROPFUNC ropfunc = p->ropfunc;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = (BYTE)ropfunc(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
}

static void BYTE_R2L_ROPFUNCLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int headbytelen = p->headbytelen;
    ROPFUNC ropfunc = p->ropfunc;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = (BYTE)ropfunc(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
}
#define XORSD(src, dst)   ((src) ^ (dst))
static void DWORDBLOCK_L2R_XORLINE(PCOPYLINESTRUCT p)
{
    BYTE* dst = p->dst;
    BYTE* src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int taildwordlen = p->taildwordlen;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = XORSD(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
    
    while ((blocklen)--)
    {
        *((DWORD*)(dst)) = XORSD(*((DWORD*)(src))++, *((DWORD*)(dst)));
        ++(DWORD*)(dst);
        *((DWORD*)(dst)) = XORSD(*((DWORD*)(src))++, *((DWORD*)(dst)));
        ++(DWORD*)(dst);
        *((DWORD*)(dst)) = XORSD(*((DWORD*)(src))++, *((DWORD*)(dst)));
        ++(DWORD*)(dst);
        *((DWORD*)(dst)) = XORSD(*((DWORD*)(src))++, *((DWORD*)(dst)));
        ++(DWORD*)(dst);
    }
    
    while ((taildwordlen)--)
    {
        *((DWORD*)(dst)) = XORSD(*((DWORD*)(src))++, *((DWORD*)(dst)));
        ++(DWORD*)(dst);
    }

    while ((tailbytelen)--)
    {
        *((BYTE*)(dst)) = XORSD(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
}

static void DWORDBLOCK_R2L_XORLINE(PCOPYLINESTRUCT p)
{
    BYTE* dst = p->dst;
    BYTE* src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int taildwordlen = p->taildwordlen;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = XORSD(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
    
    while ((blocklen)--)
    {
        --(DWORD*)(dst);
        *((DWORD*)(dst)) = XORSD(*(--(DWORD*)(src)), *((DWORD*)(dst)));
        --(DWORD*)(dst);
        *((DWORD*)(dst)) = XORSD(*(--(DWORD*)(src)), *((DWORD*)(dst)));
        --(DWORD*)(dst);
        *((DWORD*)(dst)) = XORSD(*(--(DWORD*)(src)), *((DWORD*)(dst)));
        --(DWORD*)(dst);
        *((DWORD*)(dst)) = XORSD(*(--(DWORD*)(src)), *((DWORD*)(dst)));
    }
    
    while ((taildwordlen)--)
    {
        --(DWORD*)(dst);
        *((DWORD*)(dst)) = XORSD(*(--(DWORD*)(src)), *((DWORD*)(dst)));
    }
    
    while ((tailbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = XORSD(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
}

static void DWORD_L2R_XORLINE(PCOPYLINESTRUCT p)
{
    BYTE* dst = p->dst;
    BYTE* src = p->src;
    int dwordlen = p->dwordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = XORSD(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }

    while ((dwordlen)--)
    {
        *((DWORD*)(dst)) = XORSD(*((DWORD*)(src))++, *((DWORD*)(dst)));
        ++(DWORD*)(dst);
    }

    while ((tailbytelen)--)
    {
        *((BYTE*)(dst)) = XORSD(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
}

static void DWORD_R2L_XORLINE(PCOPYLINESTRUCT p)
{
    BYTE* dst = p->dst;
    BYTE* src = p->src;
    int dwordlen = p->dwordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = XORSD(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
    
    while ((dwordlen)--)
    {
        --(DWORD*)(dst);
        *((DWORD*)(dst)) = XORSD(*(--(DWORD*)(src)), *((DWORD*)(dst)));
    }
    
    while ((tailbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = XORSD(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
}

static void WORDBLOCK_L2R_XORLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int tailwordlen = p->tailwordlen;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = XORSD(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
    
    while ((blocklen)--)
    {
        *((WORD*)(dst)) = XORSD(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
    }
    
    while ((tailwordlen)--)
    {
        *((WORD*)(dst)) = XORSD(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
    }
    
    while ((tailbytelen)--)
    {
        *((BYTE*)(dst)) = XORSD(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
}

static void WORDBLOCK_R2L_XORLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int tailwordlen = p->tailwordlen;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = XORSD(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
    
    while ((blocklen)--)
    {
        --(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*(--(WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*(--(WORD*)(src)), *((WORD*)(dst)));
    }
    
    while ((tailwordlen)--)
    {
        --(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*(--(WORD*)(src)), *((WORD*)(dst)));
    }
    
    while ((tailbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = XORSD(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
}

static void WORD_L2R_XORLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int wordlen = p->wordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = XORSD(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
    
    while ((wordlen)--)
    {
        *((WORD*)(dst)) = XORSD(*((WORD*)(src))++, *((WORD*)(dst)));
        ++(WORD*)(dst);
    }
    
    while ((tailbytelen)--)
    {
        *((BYTE*)(dst)) = XORSD(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
}

static void WORD_R2L_XORLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int wordlen = p->wordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = XORSD(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
    
    while ((wordlen)--)
    {
        --(WORD*)(dst);
        *((WORD*)(dst)) = XORSD(*(--(WORD*)(src)), *((WORD*)(dst)));
    }
    
    while ((tailbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = XORSD(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
}

static void BYTE_L2R_XORLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int headbytelen = p->headbytelen;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = XORSD(*((BYTE*)(src))++, *((BYTE*)(dst)));
        ++(BYTE*)(dst);
    }
}

static void BYTE_R2L_XORLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int headbytelen = p->headbytelen;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        *((BYTE*)(dst)) = XORSD(*(--(BYTE*)(src)), *((BYTE*)(dst)));
    }
}

#define ANDSD(src, dst)   ( ((src) == 0) ? ((src)&(dst)):(((dst>>18)<<18) | ((src) & (dst))))
static void DWORDBLOCK_L2R_ANDLINE(PCOPYLINESTRUCT p)
{
    BYTE* dst = p->dst;
    BYTE* src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int taildwordlen = p->taildwordlen;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
        ++(BYTE*)(dst);
        ++(BYTE*)(src);
    }
    
    while ((blocklen)--)
    {
        *((DWORD*)(dst)) = ANDSD(*((DWORD*)(src)), *((DWORD*)(dst)));
        ++(DWORD*)(dst);
        ++(DWORD*)(src);
        *((DWORD*)(dst)) = ANDSD(*((DWORD*)(src)), *((DWORD*)(dst)));
        ++(DWORD*)(dst);
        ++(DWORD*)(src);
        *((DWORD*)(dst)) = ANDSD(*((DWORD*)(src)), *((DWORD*)(dst)));
        ++(DWORD*)(dst);
        ++(DWORD*)(src);
        *((DWORD*)(dst)) = ANDSD(*((DWORD*)(src)), *((DWORD*)(dst)));
        ++(DWORD*)(dst);
        ++(DWORD*)(src);
    }
    
    while ((taildwordlen)--)
    {
        *((DWORD*)(dst)) = ANDSD(*((DWORD*)(src)), *((DWORD*)(dst)));
        ++(DWORD*)(dst);
        ++(DWORD*)(src);
    }

    while ((tailbytelen)--)
    {
        *((BYTE*)(dst)) = ANDSD(*((DWORD*)(src)), *((BYTE*)(dst)));
        ++(BYTE*)(dst);
        ++(BYTE*)(src);
    }
}

static void DWORDBLOCK_R2L_ANDLINE(PCOPYLINESTRUCT p)
{
    BYTE* dst = p->dst;
    BYTE* src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int taildwordlen = p->taildwordlen;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        --(BYTE*)(src);
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
    }
    
    while ((blocklen)--)
    {
        --(DWORD*)(dst);
        --(DWORD*)(src);
        *((DWORD*)(dst)) = ANDSD(*((DWORD*)(src)), *((DWORD*)(dst)));
        --(DWORD*)(dst);
        --(DWORD*)(src);
        *((DWORD*)(dst)) = ANDSD(*((DWORD*)(src)), *((DWORD*)(dst)));
        --(DWORD*)(dst);
        --(DWORD*)(src);
        *((DWORD*)(dst)) = ANDSD(*((DWORD*)(src)), *((DWORD*)(dst)));
        --(DWORD*)(dst);
        --(DWORD*)(src);
        *((DWORD*)(dst)) = ANDSD(*((DWORD*)(src)), *((DWORD*)(dst)));
    }
    
    while ((taildwordlen)--)
    {
        --(DWORD*)(dst);
        --(DWORD*)(src);
        *((DWORD*)(dst)) = ANDSD(*((DWORD*)(src)), *((DWORD*)(dst)));
    }
    
    while ((tailbytelen)--)
    {
        --(BYTE*)(dst);
        --(BYTE*)(src);
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
    }
}

static void DWORD_L2R_ANDLINE(PCOPYLINESTRUCT p)
{
    BYTE* dst = p->dst;
    BYTE* src = p->src;
    int dwordlen = p->dwordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
        ++(BYTE*)(dst);
        ++(BYTE*)(src);
    }

    while ((dwordlen)--)
    {
        *((DWORD*)(dst)) = ANDSD(*((DWORD*)(src)), *((DWORD*)(dst)));
        ++(DWORD*)(dst);
        ++(DWORD*)(src);
    }

    while ((tailbytelen)--)
    {
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
        ++(BYTE*)(dst);
        ++(BYTE*)(src);
    }
}

static void DWORD_R2L_ANDLINE(PCOPYLINESTRUCT p)
{
    BYTE* dst = p->dst;
    BYTE* src = p->src;
    int dwordlen = p->dwordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        --(BYTE*)(src);
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
    }
    
    while ((dwordlen)--)
    {
        --(DWORD*)(dst);
        --(DWORD*)(src);
        *((DWORD*)(dst)) = ANDSD(*((DWORD*)(src)), *((DWORD*)(dst)));
    }
    
    while ((tailbytelen)--)
    {
        --(BYTE*)(dst);
        --(BYTE*)(src);
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
    }
}

static void WORDBLOCK_L2R_ANDLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int tailwordlen = p->tailwordlen;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
        ++(BYTE*)(dst);
        ++(BYTE*)(src);
    }
    
    while ((blocklen)--)
    {
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        ++(WORD*)(dst);
        ++(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        ++(WORD*)(dst);
        ++(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        ++(WORD*)(dst);
        ++(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        ++(WORD*)(dst);
        ++(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        ++(WORD*)(dst);
        ++(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        ++(WORD*)(dst);
        ++(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        ++(WORD*)(dst);
        ++(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        ++(WORD*)(dst);
        ++(WORD*)(src);
    }
    
    while ((tailwordlen)--)
    {
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        ++(WORD*)(dst);
        ++(WORD*)(src);
    }
    
    while ((tailbytelen)--)
    {
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
        ++(BYTE*)(dst);
        ++(BYTE*)(src);
    }
}

static void WORDBLOCK_R2L_ANDLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int blocklen = p->blocklen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    int tailwordlen = p->tailwordlen;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        --(BYTE*)(src);
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
    }
    
    while ((blocklen)--)
    {
        --(WORD*)(dst);
        --(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        --(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        --(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        --(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        --(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        --(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        --(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        --(WORD*)(dst);
        --(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
    }
    
    while ((tailwordlen)--)
    {
        --(WORD*)(dst);
        --(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
    }
    
    while ((tailbytelen)--)
    {
        --(BYTE*)(dst);
        --(BYTE*)(src);
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
    }
}

static void WORD_L2R_ANDLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int wordlen = p->wordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
        ++(BYTE*)(dst);
        ++(BYTE*)(src);
    }
    
    while ((wordlen)--)
    {
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
        ++(WORD*)(dst);
        ++(WORD*)(src);
    }
    
    while ((tailbytelen)--)
    {
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
        ++(BYTE*)(dst);
        ++(BYTE*)(src);
    }
}

static void WORD_R2L_ANDLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int wordlen = p->wordlen;
    int headbytelen = p->headbytelen;
    int tailbytelen = p->tailbytelen;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        --(BYTE*)(src);
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
    }
    
    while ((wordlen)--)
    {
        --(WORD*)(dst);
        --(WORD*)(src);
        *((WORD*)(dst)) = ANDSD(*((WORD*)(src)), *((WORD*)(dst)));
    }
    
    while ((tailbytelen)--)
    {
        --(BYTE*)(dst);
        --(BYTE*)(src);
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
    }
}

static void BYTE_L2R_ANDLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int headbytelen = p->headbytelen;

    while ((headbytelen)--)
    {
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
        ++(BYTE*)(dst);
        ++(BYTE*)(src);
    }
}

static void BYTE_R2L_ANDLINE(PCOPYLINESTRUCT p)
{
    BYTE *dst = p->dst;
    BYTE *src = p->src;
    int headbytelen = p->headbytelen;
    
    while ((headbytelen)--)
    {
        --(BYTE*)(dst);
        --(BYTE*)(src);
        *((BYTE*)(dst)) = ANDSD(*((BYTE*)(src)), *((BYTE*)(dst)));
    }
}

static COPYLINEFUNC CopyLineFuncTable[] =
{
    //ROPFUNC
    DWORDBLOCK_L2R_ROPFUNCLINE,
    DWORD_L2R_ROPFUNCLINE,
    WORDBLOCK_L2R_ROPFUNCLINE,
    WORD_L2R_ROPFUNCLINE,
    BYTE_L2R_ROPFUNCLINE,
    DWORDBLOCK_R2L_ROPFUNCLINE,
    DWORD_R2L_ROPFUNCLINE,
    WORDBLOCK_R2L_ROPFUNCLINE,
    WORD_R2L_ROPFUNCLINE,
    BYTE_R2L_ROPFUNCLINE,

    //SRCCOPY
    DWORDBLOCK_L2R_SRCCOPYLINE,
    DWORD_L2R_SRCCOPYLINE,
    WORDBLOCK_L2R_SRCCOPYLINE,
    WORD_L2R_SRCCOPYLINE,
    BYTE_L2R_SRCCOPYLINE,
    DWORDBLOCK_R2L_SRCCOPYLINE,
    DWORD_R2L_SRCCOPYLINE,
    WORDBLOCK_R2L_SRCCOPYLINE,
    WORD_R2L_SRCCOPYLINE,
    BYTE_R2L_SRCCOPYLINE,

    //SRC_XOR_DST
    DWORDBLOCK_L2R_XORLINE,
    DWORD_L2R_XORLINE,
    WORDBLOCK_L2R_XORLINE,
    WORD_L2R_XORLINE,
    BYTE_L2R_XORLINE,
    DWORDBLOCK_R2L_XORLINE,
    DWORD_R2L_XORLINE,
    WORDBLOCK_R2L_XORLINE,
    WORD_R2L_XORLINE,
    BYTE_R2L_XORLINE,

    //SRC_AND_XOR
    DWORDBLOCK_L2R_ANDLINE,
    DWORD_L2R_ANDLINE,
    WORDBLOCK_L2R_ANDLINE,
    WORD_L2R_ANDLINE,
    BYTE_L2R_ANDLINE,
    DWORDBLOCK_R2L_ANDLINE,
    DWORD_R2L_ANDLINE,
    WORDBLOCK_R2L_ANDLINE,
    WORD_R2L_ANDLINE,
    BYTE_R2L_ANDLINE,
};

void GetCopyLineType(BYTE* dst, BYTE* src, int len, PCOPYLINESTRUCT p, 
                     int dir, DWORD rop)
{
    int index;

    p->type = 0;
    /* 由右向左拷贝时，首地址使用尾字节的下一个字节地址 */
    if (dir == COPYLINE_R2L)
    {
        dst += len;
        src += len;
        p->type |= R2L_COPY;
    }

    p->dst = dst;
    p->src = src;

#if (DWORD_ACCESS_1)
    if (ALIGNED_DWORD(dst, src))
    {
        p->headbytelen = GETHEADBYTES(dst, (sizeof(DWORD)), dir);
        len -= p->headbytelen;
    }
    else if (ALIGNED_WORD(dst, src))
    {
        p->headbytelen = GETHEADBYTES(dst, (sizeof(WORD)), dir);
        len -= p->headbytelen;
    }

    if (len == 0)
    {
        p->type |= BYTE_COPY;
    }
    else if(len < BLOCKSIZE)
    {
        p->dwordlen = len / sizeof(DWORD);
        p->tailbytelen = len % sizeof(DWORD);
        p->type |= DWORD_COPY;
    }
    else
    {
        p->blocklen = len / BLOCKSIZE;
        len %= BLOCKSIZE;
        p->taildwordlen = len / sizeof(DWORD);
        len %= sizeof(DWORD);
        p->tailbytelen = len;
        p->type |= DWORDBLOCK_COPY;
    }
    
#elif(DWORD_ACCESS_2)

    if (ALIGNED_WORD(dst, src))
    {
        if (ALIGNED_DWORD(dst, src))
        {
            p->headbytelen = GETHEADBYTES(dst, sizeof(DWORD), dir);
            len -= p->headbytelen;
        }
        else
        {
            p->headbytelen = GETHEADBYTES(dst, sizeof(WORD), dir);
            len -= p->headbytelen;
        }

        if (len == 0)
        {
            p->type |= BYTE_COPY;
        }
        else if(len < BLOCKSIZE)
        {
            p->dwordlen = len / sizeof(DWORD);
            p->tailbytelen = len % sizeof(DWORD);
            p->type |= DWORD_COPY;
        }
        else
        {
            p->blocklen = len / BLOCKSIZE;
            len %= BLOCKSIZE;
            p->taildwordlen = len / sizeof(DWORD);
            len %= sizeof(DWORD);
            p->tailbytelen = len;
            p->type |= DWORDBLOCK_COPY;
        }
    }
    else
    {
        p->headbytelen = len;
        p->type |= BYTE_COPY;
    }

#else

    if (ALIGNED_DWORD(dst, src))
    {
        p->headbytelen = (WORD)(GETHEADBYTES(dst, sizeof(DWORD), dir));
        
        len -= p->headbytelen;
        if (len == 0)
        {
            p->type |= BYTE_COPY;
        }
        else if(len < BLOCKSIZE)
        {
            p->dwordlen = len / sizeof(DWORD);
            p->tailbytelen = len % sizeof(DWORD);
            p->type |= DWORD_COPY;
        }
        else
        {
            p->blocklen = (WORD)(len / BLOCKSIZE);
            len %= BLOCKSIZE;
            p->taildwordlen = len / sizeof(DWORD);
            len %= sizeof(DWORD);
            p->tailbytelen = len;
            p->type |= DWORDBLOCK_COPY;
        }
    }
    else if (ALIGNED_WORD(dst, src))
    {
        
        p->headbytelen = (WORD)(GETHEADBYTES(dst, sizeof(WORD), dir));
        
        len -= p->headbytelen;
        if (len == 0)
        {
            p->type |= BYTE_COPY;
        }
        else if (len < BLOCKSIZE)
        {
            p->wordlen = len / sizeof(WORD);
            p->tailbytelen = len % sizeof(WORD);
            p->type |= WORD_COPY;
        }
        else
        {
            p->blocklen = (WORD)(len / BLOCKSIZE);
            len %= BLOCKSIZE;
            p->tailwordlen = len / sizeof(WORD);
            len %= sizeof(WORD);
            p->tailbytelen = len;
            p->type |= WORDBLOCK_COPY;
        }
    }
    else
    {
        p->headbytelen = len;
        p->type |= BYTE_COPY;
    }
#endif

    rop = SRop3ToRop2(rop);
    p->ropfunc = GetRopFunc(rop);

    switch(rop)
    {
    case ROP_SRC:
        index = MAX_FUNCTYPE * SRC_COPY;
        break;
    case ROP_SRC_XOR_DST:
        index = MAX_FUNCTYPE * XORSD_COPY;
        break;
    case ROP_SRC_AND_DST:
        index = MAX_FUNCTYPE * ANDSD_COPY;
        break;
    default:
        index = MAX_FUNCTYPE * ROP_COPY;
        break;
    }

    if (p->type & R2L_COPY)
        index += MAX_FUNCTYPE / 2;

    index += GETFUNCINDEX(p->type);
    p->copylinefunc = CopyLineFuncTable[index];

}
