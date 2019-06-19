/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : amrface.h
 *
 * Purpose  : Declare AMR decoder structures and functions.
 *
\**************************************************************************/

#ifndef __AMRFACE_H
#define __AMRFACE_H

#include "plxmm.h"

#define AMR_SOURCE_BUFSIZE  256
#define AMR_DECODE_BUFSIZE  320
#define AMR_MAGIC_SIZE      6
#define AMRWB_MAGIC_SIZE    9
#define OFFSET_BASESIZE		256
#define FILE_BUFSIZE        0x1000

#define AMR_TYPE_NB         1
#define AMR_TYPE_WB         2

extern int Framebits[];
extern const UWord8 block_size[];

typedef struct tagMMDECODER
{
    char            magic[4];       // For checking pointer.
                                    // Fixed value: "MMD"(4D 4D 44 00).

    int             type;           // Type of decoder.
                                    // 0 - mp3 decoder
                                    // 1 - amr decoder
    int             attrib;         // Attribution of decoder.
                                    // 0 - decode in series
                                    // 1 - decode in random

    MMDataSource    *pSource;       // Pointer to data source structure.
    int             nTotalSrcSize;  // Total size of data source.
    int             nSourceOffset;  // Offset of data source currently
                                    // to read.
    int             nFrames;        // Total of frames of data source.

    unsigned char   *SourceBuf;     // Buffer to contain undecoded raw
                                    // data from source.
    unsigned char   *pSrcDataPtr;   // Pointer to source data inside
                                    // the source buffer.
    int             nSBufSize;      // Size of undecoded data buffer.
    int             nSrcDataSize;   // Size of source data currently in
                                    // undecoded data buffer.

    unsigned char   *DecodeBuf;     // Buffer to contain decoded data.
    unsigned char   *pDecDataPtr;   // Pointer to decoded data inside
                                    // the decode buffer.
    int             nDBufSize;      // Size of decoded data buffer.
    int             nDecDataSize;   // Size of decoded data that still
                                    // left in decoded data buffer.

    int             nPrivate;       // Private member for individual
                                    // decoder.

    void            *pParam;        // User specified data.
} MMDECODER, *PMMDECODER;

typedef struct tagAMRMEMBER
{
	int				*pDestate;		/* Pointer to the AMR decoder
									 * device.
									 */
	unsigned char	*pCodDataPtr;	/* Pointer to cod data within the
									 * decode buffer.
									 */
    int             *pFrameOffset;  /* Offset of each frame in data 
                                     * source, presently only used 
                                     * for AMR decoder.
                                     */
    int             nType;          /* Identify the type of AMR audio
                                     */
} AMRMEMBER, *PAMRMEMBER;

static int IsAmrFile(unsigned char *pFileData);
static int GetAmrFrames(PMMDECODER pMMDecoder, unsigned char *pFileData);
static int ConvertAmrIntoCod(PMMDECODER pMMDecoder);
static int GetNextFrameSize(int nType, unsigned char cFrameHdr);
static int ImportSourceData(PMMDECODER pMMDecoder);
static int DecodeCertainFrame(PMMDECODER pMMDecoder);
static int ExportDecodedData(PMMDECODER pMMDecoder, unsigned char *pBuf,
					  int nReqSize, int nCopiedTotal);

#endif // __AMRFACE_H
