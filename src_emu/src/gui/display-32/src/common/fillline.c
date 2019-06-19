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


static void DWORDBLOCK_16_FILLLINE(PFILLLINESTRUCT p)
{
    int     headpixellen = p->headlen;
    int     blocklen = p->blocklen;
    int     tailpixellen = p->taillen;
    int     taildwordlen = p->taildwordlen;
    BYTE    *dst = p->dst;
    DWORD   color = p->color;

    while (headpixellen --)
    {
        *((WORD*)(dst))++ = (WORD)color;
    }
    while (blocklen --)
    {
        *((DWORD*)(dst))++ = color;
        *((DWORD*)(dst))++ = color;
        *((DWORD*)(dst))++ = color;
        *((DWORD*)(dst))++ = color;
    }
    while (taildwordlen --)
    {
        *((DWORD*)(dst))++ = color;
    }
    while (tailpixellen --)
    {
        *((WORD*)(dst))++ = (WORD)color;
    }
}

static void DWORDBLOCK_24_FILLLINE(PFILLLINESTRUCT p)
{
    int     headbytelen = p->headlen;
    int     blocklen = p->blocklen;
    int     tailbytelen = p->taillen;
    int     taildwordlen = p->taildwordlen;
    BYTE    *dst = p->dst;
    DWORD   *color24 = p->color24;
    BYTE    *head = p->rgbcolor;
    BYTE    *tail = p->rgbcolor + (3 - p->taillen);
    
    while (headbytelen --)
    {
        *((BYTE*)(dst))++ = *head++;
    }
    while (blocklen --)
    {
        *((DWORD*)(dst))++ = color24[0];
        *((DWORD*)(dst))++ = color24[1];
        *((DWORD*)(dst))++ = color24[2];
    }
    while (taildwordlen --)
    {
        *((DWORD*)(dst))++ = color24[p->taildwordlen - 1 - taildwordlen];
    }
    while (tailbytelen --)
    {
        *((BYTE*)(dst))++ = *tail++;
    }
}

void GetFillLineType(BYTE* dst, DWORD color, int len, int pixbytes, 
                     PFILLLINESTRUCT p, DWORD rop, DWORD fillmode)
{
    p->dst = dst;
    len *= pixbytes;

    if (pixbytes == 2)
    {
        ASSERT(((DWORD)(dst) & (sizeof(WORD) - 1)) == 0);
        p->color = (color << 16) | (color & 0xFFFF);
        p->pixbytes = 2;

        p->headlen = (WORD)(GETHEADBYTES(dst, sizeof(DWORD), COPYLINE_L2R));
        len -= p->headlen;
        p->headlen /= 2; //bytes to pixels

        if (len == 0)
        {
            p->filllinefunc = NULL;
        }
        else
        {
            p->blocklen = (WORD)(len / BLOCKSIZE_16);
            len %= BLOCKSIZE_16;
            p->taildwordlen = (WORD)(len / sizeof(DWORD));
            len %= sizeof(DWORD);
            p->taillen = (WORD)(len / 2);
            p->filllinefunc = DWORDBLOCK_16_FILLLINE;
        }
    }
    else if (pixbytes == 3)
    {
        p->pixbytes = 3;
        p->headlen = (WORD)(GETHEADBYTES(dst, sizeof(DWORD), COPYLINE_L2R));
        len -= p->headlen;
        
        if (len < BLOCKSIZE_24)
        {
            p->filllinefunc = NULL;
        }
        else
        {
            DWORD colortmp[3];

            switch(fillmode)
            {
            case FILL_BIG_RGB:
                p->rgbcolor[0] = (BYTE)GETR(color);
                p->rgbcolor[1] = (BYTE)GETG(color);
                p->rgbcolor[2] = (BYTE)GETB(color);

                //framebuffer   RGBR GBRG BRGB color = 0x0BGR
                //value         RGBR GBRG BRGB
                color = ((color & 0xFF0000) >> 16) | 
                        ((color & 0x00FF00)      ) | 
                        ((color & 0x0000FF) << 16);
                //color = 0x0RGB
                colortmp[0] = (color <<  8) | ((color >> 16) & 0x0000FF);
                colortmp[1] = (color << 16) | ((color >>  8) & 0x00FFFF);
                colortmp[2] = (color << 24) | ((color      ) & 0xFFFFFF);
                break;
            case FILL_LITTLE_RGB:
                p->rgbcolor[0] = (BYTE)GETR(color);
                p->rgbcolor[1] = (BYTE)GETG(color);
                p->rgbcolor[2] = (BYTE)GETB(color);

                //framebuffer   RGBR GBRG BRGB color = 0x0BGR
                //value         RBGR GRBG BGRB
                colortmp[0] = (color << 24) | ((color      ) & 0xFFFFFF);
                colortmp[1] = (color << 16) | ((color >>  8) & 0x00FFFF);
                colortmp[2] = (color <<  8) | ((color >> 16) & 0x0000FF);
                break;
            case FILL_BIG_BGR:
                p->rgbcolor[0] = (BYTE)GETB(color);
                p->rgbcolor[1] = (BYTE)GETG(color);
                p->rgbcolor[2] = (BYTE)GETR(color);

                //framebuffer   BGRB GRBG RBGR color = 0x0BGR
                //value         BGRB GRBG RBGR
                colortmp[0] = (color <<  8) | ((color >> 16) & 0x0000FF);
                colortmp[1] = (color << 16) | ((color >>  8) & 0x00FFFF);
                colortmp[2] = (color << 24) | ((color      ) & 0xFFFFFF);
                break;
            case FILL_LITTLE_BGR:
                p->rgbcolor[0] = (BYTE)GETB(color);
                p->rgbcolor[1] = (BYTE)GETG(color);
                p->rgbcolor[2] = (BYTE)GETR(color);

                //framebuffer   BGRB GRBG RBGR color = 0x0BGR
                //value         BRGB GBRG RGBR
                color = ((color & 0xFF0000) >> 16) | 
                        ((color & 0x00FF00)      ) | 
                        ((color & 0x0000FF) << 16);
                //color = 0x0RGB
                colortmp[0] = (color << 24) | ((color      ) & 0xFFFFFF);
                colortmp[1] = (color << 16) | ((color >>  8) & 0x00FFFF);
                colortmp[2] = (color <<  8) | ((color >> 16) & 0x0000FF);
                break;
            default:
                ASSERT(0);
                return;
            }

            p->color24[0] = colortmp[p->headlen       % 3];
            p->color24[1] = colortmp[(p->headlen + 1) % 3];
            p->color24[2] = colortmp[(p->headlen + 2) % 3];
            
            p->blocklen = (WORD)(len / BLOCKSIZE_24);
            len %= BLOCKSIZE_24;
            p->taildwordlen = (WORD)(len / sizeof(DWORD));
            len %= sizeof(DWORD);
            p->taillen = (WORD)len;

            p->filllinefunc = DWORDBLOCK_24_FILLLINE;
        }
    }
    else if (pixbytes == 4)
    {
        p->filllinefunc = NULL;
    }
}
