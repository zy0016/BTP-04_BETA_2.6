/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    buff.h
Abstract:
	This file includes the definition of buffer management functions.
Author:
    Gang He
Revision History:
	2000.2
---------------------------------------------------------------------------*/
#ifndef BUFF_H
#define BUFF_H

struct BuffStru {
	WORD len; /* length of whole buffer, including data before off */
	WORD off;
	UCHAR data[1];
};

#define BUFFER_NOFREE	0x8000
#define BUFFER_RESERVE	0x4000
#define USER_BUFFER		0X8000

#ifdef CONFIG_MEMORY_LEAK_DETECT
struct BuffStru * DBG_BuffNew(WORD size, WORD off, char* filename, int line);
#define BuffNew(A,B) DBG_BuffNew(A,B, __FILE__, __LINE__)
#else
struct BuffStru * BuffNew(WORD size, WORD off);
#endif
void BuffFree(struct BuffStru * buf);
#define UserBuffFree BuffFree

UCHAR BuffRes(struct BuffStru * buf, INT16 delta);
UCHAR CopyFromBuf(UCHAR * data, struct BuffStru * buf, INT16 len);
#define CopyToBuf(data,buf,len) CopyFromBuf(data,buf,-len)

struct BuffStru * UserBuffNew(WORD size, WORD off);

#define HDLPTR(buf)	((DWORD*)(&(((struct BuffStru*)buf)->data[0])))
#define BUFLEN(buf)  ((((struct BuffStru*)buf)->len)&0x3fff)
#define BUFDATA(buf)	((UCHAR*)(((struct BuffStru*)buf)->data + ((struct BuffStru*)buf)->off))
#define DATASIZE(buf) (BUFLEN((struct BuffStru*)buf)-((struct BuffStru*)buf)->off)


#define BufferNewSpecific(size,off) BuffNew(size|BUFFER_NOFREE,off)
#define BufferSetSpecific(buf)	buf->len|=BUFFER_NOFREE
#define BufferFreeSpecific(buf) {\
	buf->len&=~BUFFER_NOFREE; \
	BuffFree(buf);\
}

#endif
