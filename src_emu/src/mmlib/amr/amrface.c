/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : amrface.c
 *
 * Purpose  : Define AMR decoder functions.
 *
\**************************************************************************/

#include "amrdec.h"
#include "amrconv.h"
#include "dec_if.h"

#include "amrface.h"

/*********************************************************************\
* Function     amrDecoderCreate
* Purpose      Creates and initializes an AMR audio decoder.
* Params	   random
*                  Playing mode. 0 represents decoding in series,
*                  while 1 represents decoding in random
*			   pSource
*                  Pointer to structure of callback functions
*			   nSourceSize
*                  The total length of source data
*			   nPreferBufSize
*                  Size of internal buffer in decoder prefered by 
*                  application. Decoder will use default size if -1
*                  specified
*			   pParam
*                  Parameter specified by the application. Only when
*                  the callback function mm_read is called, passes it
*                  to application
* Return	   Returns pointer to the created AMR decoder if
*			   successful or NULL otherwise.
* Remarks	   
**********************************************************************/

PMMDECODER amrDecoderCreate(int random, PMMDataSource pSource,
                            int nSourceSize, int nPreferBufSize,
                            void *pParam)
{
    PMMDECODER pAmrDecoder = NULL;
    PAMRMEMBER pMember = NULL;
    unsigned char *pFileData = NULL, cFirstHdr = '\0';
    int *destate = NULL;
    int nAmrType = 0, nReadSize = 0, nMagicSize = 0, nFrameSize = 0;

    if (pSource == NULL)
        return NULL;

    if (pSource->mm_malloc == NULL || pSource->mm_free == NULL
        || pSource->mm_realloc == NULL || pSource->mm_memset == NULL
        || pSource->mm_memcpy == NULL || pSource->mm_read == NULL)
        return NULL;

    // Loads AMR file or data stream into a temporary memory.
    pFileData = pSource->mm_malloc(AMRWB_MAGIC_SIZE + 1);
    if (pFileData == NULL)
        return NULL;

    nReadSize = pSource->mm_read(pFileData, AMRWB_MAGIC_SIZE, 0,
        pParam);

    // Confirms whether it is an AMR file
    if (nReadSize < AMRWB_MAGIC_SIZE || !(nAmrType = IsAmrFile(pFileData)))
    {
        goto exit_create;
    }

    // Creates an AMR decoder.
    if (nAmrType == AMR_TYPE_NB)
    {
        destate = Decoder_Interface_init();
        nMagicSize = AMR_MAGIC_SIZE;
    }
    else
    {
        destate = D_IF_init();
        nMagicSize = AMRWB_MAGIC_SIZE;
    }
    pAmrDecoder = pSource->mm_malloc(sizeof(MMDECODER));
    if (pAmrDecoder == NULL)
    {
        goto exit_create;
    }
    else
    {
        pAmrDecoder->pSource   = NULL;
        pAmrDecoder->SourceBuf = NULL;
        pAmrDecoder->DecodeBuf = NULL;
        pAmrDecoder->nPrivate  = 0;
    }

    // Initializes AMR decoder parameters
    // type
    // pAmrDecoder->type = DECODER_TYPE_AMR;

    // attrib
    pAmrDecoder->attrib = 0;
    pAmrDecoder->attrib |= DECODER_ATTRIB_RANDOM;

    // pSource - for callback functions
    pAmrDecoder->pSource = pSource->mm_malloc(sizeof(MMDataSource));
    if (pAmrDecoder->pSource == NULL)
    {
        goto exit_create;
    }

    pSource->mm_memcpy(pAmrDecoder->pSource, pSource,
        sizeof(MMDataSource));

    // nTotalSrcSize
    pAmrDecoder->nTotalSrcSize = nSourceSize;

    // nSourceOffset
    pAmrDecoder->nSourceOffset = nMagicSize;

    // Initializes source data buffer
    if (nPreferBufSize == -1 || nPreferBufSize < AMR_SOURCE_BUFSIZE)
    {
        pAmrDecoder->nSBufSize = AMR_SOURCE_BUFSIZE;
    }
    else
    {
        pAmrDecoder->nSBufSize = nPreferBufSize;
    }
    pAmrDecoder->SourceBuf = pSource->mm_malloc(pAmrDecoder->nSBufSize);
    if (pAmrDecoder->SourceBuf == NULL)
    {
        goto exit_create;
    }
    pAmrDecoder->pSrcDataPtr = pAmrDecoder->SourceBuf;
    pAmrDecoder->nSrcDataSize = 0;

    // Initializes decode data buffer
    if (nAmrType == AMR_TYPE_NB)
    {
        pAmrDecoder->nDBufSize = AMR_DECODE_BUFSIZE;
        pAmrDecoder->DecodeBuf = pSource->mm_malloc(pAmrDecoder->nDBufSize + 32);
    }
    else
    {
        pAmrDecoder->nDBufSize = AMR_DECODE_BUFSIZE * sizeof(Word16);
        pAmrDecoder->DecodeBuf = pSource->mm_malloc(pAmrDecoder->nDBufSize);
    }
    if (pAmrDecoder->DecodeBuf == NULL)
    {
        goto exit_create;
    }
    pAmrDecoder->pDecDataPtr = pAmrDecoder->DecodeBuf;
    pAmrDecoder->nDecDataSize = 0;

    // Initializes structure of pMember for nPrivate
    pMember = pSource->mm_malloc(sizeof(AMRMEMBER));
    if (pMember == NULL)
    {
        goto exit_create;
    }

    // pDestate
    pMember->pDestate = destate;

    // pCodDataPtr
    pMember->pCodDataPtr = pAmrDecoder->DecodeBuf
        + pAmrDecoder->nDBufSize;

    // pFrameOffset
    // Decides if need to allocate frame offset buffer
    if (!(random & DECODER_ATTRIB_RANDOM))
    {
        pMember->pFrameOffset = NULL;
    }
    else
    {
        pMember->pFrameOffset = (int *)
            pSource->mm_malloc(OFFSET_BASESIZE * sizeof(int));
        if (pMember->pFrameOffset == NULL)
        {
            goto exit_create;
        }
    }

    pMember->nType = nAmrType;

    // nPrivate, only for AMR decoder
    pAmrDecoder->nPrivate = (int)pMember;

    // pParam
    pAmrDecoder->pParam = pParam;

    // nFrames
    if (!(random & DECODER_ATTRIB_RANDOM))
    {
        // Assumes each frame of AMR data is in the same code stream,
        // then estimates the total of frames in this case.
        cFirstHdr = *(pFileData + nMagicSize);
        nFrameSize = GetNextFrameSize(nAmrType, cFirstHdr) + 1;
        pAmrDecoder->nFrames = (nSourceSize - nMagicSize)
            / nFrameSize;
    }
    else
    {
        pFileData = pSource->mm_realloc(pFileData, FILE_BUFSIZE);
        nReadSize = pSource->mm_read(pFileData, FILE_BUFSIZE,
            nMagicSize, pParam);

        if (nReadSize <= 0)
        {
            goto exit_create;
        }

        // Accurately counts the total of frames by scanning AMR data,
        // and records the frame offset in the pFrameoffset buffer.
        pAmrDecoder->nFrames = GetAmrFrames(pAmrDecoder, pFileData);
    }
    // Has finished initializing AMR decoder.

    // Frees buffer of pFileData allocated when beginning.
    pSource->mm_free(pFileData);

    return pAmrDecoder;

exit_create:
    if (pFileData != NULL)
        pSource->mm_free(pFileData);

    if (destate != NULL)
    {
        if (nAmrType == AMR_TYPE_NB)
        {
            Decoder_Interface_exit(destate);
        }
        else
        {
            D_IF_exit(destate);
        }
    }

    if (pAmrDecoder != NULL)
    {
        if (pAmrDecoder->pSource != NULL)
            pSource->mm_free(pAmrDecoder->pSource);
        if (pAmrDecoder->SourceBuf != NULL)
            pSource->mm_free(pAmrDecoder->SourceBuf);
        if (pAmrDecoder->DecodeBuf != NULL)
            pSource->mm_free(pAmrDecoder->DecodeBuf);
        if (pMember != NULL)
        {
            if (pMember->pFrameOffset != NULL)
                pSource->mm_free(pMember->pFrameOffset);
            
            pSource->mm_free(pMember);
        }
        pSource->mm_free(pAmrDecoder);
    }

    return NULL;
}

/*********************************************************************\
* Function	   amrDecoderDestroy
* Purpose      Exits the AMR decoder and frees memory blocks allocated
*              when initializing the decoder.
* Params	   pAmrDecoder
*					Pointer to the AMR decoder
* Return	   Returns 0 if successful or -1 otherwise.
* Remarks	   
**********************************************************************/

int amrDecoderDestroy(PMMDECODER pAmrDecoder)
{
    PMMDataSource pSource = NULL;
    PAMRMEMBER pMember = NULL;

    if (pAmrDecoder == NULL)
        return -1;

    pSource = pAmrDecoder->pSource;

    pMember = (PAMRMEMBER)pAmrDecoder->nPrivate;

    if (pMember->nType == AMR_TYPE_NB)
    {
        Decoder_Interface_exit(pMember->pDestate);
    }
    else
    {
        D_IF_exit(pMember->pDestate);
    }

    if (pMember->pFrameOffset != NULL)
        pSource->mm_free(pMember->pFrameOffset);

    if (pAmrDecoder->SourceBuf != NULL)
        pSource->mm_free(pAmrDecoder->SourceBuf);

    if (pAmrDecoder->DecodeBuf != NULL)
        pSource->mm_free(pAmrDecoder->DecodeBuf);

    if (pAmrDecoder->nPrivate != 0)
        pSource->mm_free((void *)pAmrDecoder->nPrivate);

    pSource->mm_free(pAmrDecoder);
    // Always frees pSource pointer at last.
    pSource->mm_free(pSource);

    return 0;
}

/*********************************************************************\
* Function	   amrDecoderGetTotalTime
* Purpose      Gets the total time of playing in current decoder.
* Params	   pAmrDecoder
*                  Pointer to the AMR decoder
* Return       Returns the number of seconds for playing.
* Remarks	   
**********************************************************************/

int amrDecoderGetTotalTime(PMMDECODER pAmrDecoder)
{
    if (pAmrDecoder == NULL)
        return -1;
    
    return ((160 * pAmrDecoder->nFrames + (8000 - 1)) / 8000);
}

/*********************************************************************\
* Function	   decoderSeek
* Purpose      Modifies the offset of source file or data specified
*			   location.
* Params	   pAmrDecoder
*                  Pointer to the AMR decoder
*			   nSeconds
*                  New position to decode. The playing time,
*                  in second, from the origin of source data to new
*                  position
* Return	   Returns the new location to be decoded if successful
*			   or -1 otherwise.
* Remarks	   
**********************************************************************/

int amrDecoderSeek(PMMDECODER pAmrDecoder, int nSeconds)
{
    PAMRMEMBER pMember = NULL;
    int nTotalSeconds, nFrameNo;

    if (pAmrDecoder == NULL)
        return -1;

    if (!(pAmrDecoder->attrib & DECODER_ATTRIB_RANDOM))
        return -1;

    pMember = (PAMRMEMBER)pAmrDecoder->nPrivate;

    if (pMember->pFrameOffset == NULL)
        return -1;

    if (nSeconds < 0)
        nSeconds = 0;

    nTotalSeconds = amrDecoderGetTotalTime(pAmrDecoder);

    if (nSeconds >= nTotalSeconds)
    {
        pAmrDecoder->nSourceOffset = pAmrDecoder->nTotalSrcSize;
        nSeconds = nTotalSeconds;
    }
    else
    {
        nFrameNo = pAmrDecoder->nFrames / nTotalSeconds * nSeconds;
        pAmrDecoder->nSourceOffset
            = pMember->pFrameOffset[nFrameNo];
    }

    pAmrDecoder->nSrcDataSize = 0;
    pAmrDecoder->pSrcDataPtr = pAmrDecoder->SourceBuf;
    pAmrDecoder->nDecDataSize = 0;
    pAmrDecoder->pDecDataPtr = pAmrDecoder->DecodeBuf;

    return nSeconds;
}

/*********************************************************************\
* Function	   amrDecoderGetData
* Purpose      Reads the data in the source buffer and then decodes
*			   them into specified buffer. When the data in the
*			   source buffer are inadequate, the decoder will load
*			   source data from the current offset. In addition, the
*			   decoder will return the decoded data directly if there 
*			   still remain data in the decode buffer.
* Params	   pAmrDecoder
*                  Pointer to the AMR decoder
*			   pBuf
*                  Pointer to the buffer which to receive the decoded
*                  data
*			   nReqSize
*                  Size of the required data to decode
* Return       Returns the size of decoded data that actually copied
*			   if successful or -1 otherwise
* Remarks	   
**********************************************************************/

int amrDecoderGetData(PMMDECODER pAmrDecoder, unsigned char *pBuf,
                      int nReqSize)
{
    int nCopiedTotal, nExportSize;
    unsigned char *pBufPtr = NULL;

    if (pAmrDecoder == NULL)
        return -1;

    for (nCopiedTotal = 0, pBufPtr = pBuf;
         nCopiedTotal < nReqSize;
         nCopiedTotal += nExportSize, pBufPtr += nExportSize)
    {
        if ((pAmrDecoder->nSourceOffset
            >= pAmrDecoder->nTotalSrcSize)
            && (pAmrDecoder->nSrcDataSize == 0)
            && (pAmrDecoder->nDecDataSize == 0))
            break;

        pAmrDecoder->nSourceOffset += ImportSourceData(pAmrDecoder);
        DecodeCertainFrame(pAmrDecoder);
        nExportSize = ExportDecodedData(pAmrDecoder, pBufPtr, nReqSize,
            nCopiedTotal);
    }

    return nCopiedTotal;
}

/*********************************************************************\
* Function	   amrDecoderGetAudioInfo
* Purpose      Gets audio information about the decoded data.
* Params	   pAmrDecoder
*				   Pointer to the AMR decoder
*			   pInfo
*                  Pointer to an AUDIOINFO structure that receive the
*                  audio stream's information
* Return       Returns 0 if successful or -1 otherwise.
* Remarks	   
**********************************************************************/

int amrDecoderGetAudioInfo(PMMDECODER pAmrDecoder, PAUDIOINFO pInfo)
{
    PAMRMEMBER pMember = NULL;

    if (pAmrDecoder == NULL)
        return -1;

    pMember = (PAMRMEMBER)pAmrDecoder->nPrivate;

#ifdef _EMULATE_
    pInfo->nChannel    = (pMember->nType == AMR_TYPE_NB) ? 1 : 2;
    pInfo->nSampleBits = 16;
    pInfo->nSampleRate = 8000;
#else
    pInfo->nChannel    = 1;
    pInfo->nSampleBits = 16;
    pInfo->nSampleRate = (pMember->nType == AMR_TYPE_NB) ? 8000 : 11025;
#endif

    return 0;
}

/*********************************************************************\
* Function	   IsAmrFile
* Purpose      Judge whether the source data are AMR data.
* Params	   pFileData
*                  Pointer to an AMR data stream
* Return       Return 1 if they are AMR data or 0 otherwise
* Remarks	   
**********************************************************************/

static int IsAmrFile(unsigned char *pFileData)
{
    if (pFileData[0] == '#' && pFileData[1] == '!'
        && pFileData[2] == 'A' && pFileData[3] == 'M'
        && pFileData[4] == 'R' && pFileData[5] == '\n')
        return AMR_TYPE_NB;

    if (pFileData[0] == '#' && pFileData[1] == '!'
        && pFileData[2] == 'A' && pFileData[3] == 'M'
        && pFileData[4] == 'R' && pFileData[5] == '-'
        && pFileData[6] == 'W' && pFileData[7] == 'B'
        && pFileData[8] == '\n')
        return AMR_TYPE_WB;

    return 0;
}

/*********************************************************************\
* Function	   GetAmrFrames
* Purpose      Counts the total of frames and records each frame
*			   offset of source data if the application need to
*			   seek the source data later.
* Params	   pAmrDecoder
*                  Pointer to the AMR decoder
*			   pData
*                  Pointer to an AMR data stream
* Return       Number of frames.
* Remarks	   
**********************************************************************/

static int GetAmrFrames(PMMDECODER pAmrDecoder, unsigned char *pData)
{
    PMMDataSource pSource = NULL;
    PAMRMEMBER pMember = NULL;
    unsigned char *pDataPtr = NULL;
    int *pFrameOffset = NULL, nFrameNo, nSourceOffset, nOffsetSize,
        nLeftSize, fsize;

    if (pAmrDecoder == NULL)
        return -1;

    pSource = pAmrDecoder->pSource;
    pMember = (PAMRMEMBER)pAmrDecoder->nPrivate;
    pFrameOffset = pMember->pFrameOffset;
    nSourceOffset = pAmrDecoder->nSourceOffset;
    pDataPtr = pData;
    nLeftSize = FILE_BUFSIZE;
    nOffsetSize = OFFSET_BASESIZE;
    nFrameNo = 0;

    while (1)
    {
        if (nSourceOffset >= pAmrDecoder->nTotalSrcSize)
        {
            *pFrameOffset = pAmrDecoder->nTotalSrcSize;
            
            break;
        }

        if (nFrameNo >= nOffsetSize - 1)
        {
            pMember->pFrameOffset = pSource->mm_realloc(
                pMember->pFrameOffset,
                (nOffsetSize + OFFSET_BASESIZE)	* sizeof(int));

            nOffsetSize += OFFSET_BASESIZE;
            pFrameOffset = pMember->pFrameOffset + nFrameNo;
        }

        *pFrameOffset ++ = nSourceOffset;

        fsize = GetNextFrameSize(pMember->nType, *pDataPtr);
        pDataPtr ++;
        nSourceOffset ++;

        nLeftSize = pData + FILE_BUFSIZE - pDataPtr;
        if (nLeftSize <= fsize)
        {
            pSource->mm_read(pData, FILE_BUFSIZE,
                nSourceOffset + nLeftSize, pAmrDecoder->pParam);
            
            pDataPtr = pData + (fsize - nLeftSize);
        }
        else
        {
            pDataPtr += fsize;
        }

        nSourceOffset += fsize;

        nFrameNo ++;
    }

    return nFrameNo;
}

/*********************************************************************\
* Function	   ConvertAmrIntoCod
* Purpose      Converts a certain frame of AMR data into 'COD'
*			   intermediate code data.
* Params	   pAmrDecoder
*				  Pointer to the AMR decoder.
* Return       Returns size of 'COD' data buffer that has converted.
* Remarks	   
**********************************************************************/

static int ConvertAmrIntoCod(PMMDECODER pAmrDecoder)
{
    PAMRMEMBER pMember = NULL;
    unsigned char ft, ftype, n, frame[32], *pnw = NULL;
    unsigned int fsize, outfsize, framelen;
    nibble_reader nr;
    nibble_writer nw;
    static int nHasMadefliptab = 0;
    
    if (pAmrDecoder == NULL)
        return -1;
    
    pMember = (PAMRMEMBER)pAmrDecoder->nPrivate;
    
    if (!nHasMadefliptab)
    {
        makefliptab();
        nHasMadefliptab = 1;
    }
    
    ft = *pAmrDecoder->pSrcDataPtr ++;
    pAmrDecoder->nSrcDataSize --;
    
    ftype = (ft >> 3) & 0xF;
    fsize = (Framebits[ftype] + 7) / 8;
    
    for (framelen = 0; framelen < fsize; framelen ++)
    {
        frame[framelen] = *pAmrDecoder->pSrcDataPtr ++;
    }
    
    pAmrDecoder->nSrcDataSize -= fsize;
    
    init_nibble_reader(&nr, flip_bitorder(frame, fsize), fsize);
    init_nibble_writer(&nw);
    write_nibble_writer(&nw, ftype);
    while (1)
    {
        n = read_nibble_reader(&nr);
        if (n == NONE)
            break;
        write_nibble_writer(&nw, n);
    }
    
    outfsize = (Framebits[ftype] + 4 + 7) / 8;
    
    pnw = tostring_nibble_writer(&nw);
    
    pAmrDecoder->pSource->mm_memcpy(pMember->pCodDataPtr, pnw,
        outfsize);
    
    return outfsize;
}

/*********************************************************************\
* Function	   GetNextFrameSize
* Purpose      Gets the size of next frame following by the frame
*              header.
* Params	   cFrameHdr
*                  A byte, the frame header of the next frame.
* Return       Returns the number of size if successful or -1
*			   otherwise.
* Remarks	   
**********************************************************************/

static int GetNextFrameSize(int nType, unsigned char cFrameHdr)
{
    int nFrameSize = 0, nFrameType = 0;

    if (nType == AMR_TYPE_NB)
    {
        if (!(cFrameHdr & 4))
            return 0;

        nFrameType = (cFrameHdr >> 3) & 0xF;
        nFrameSize = (Framebits[nFrameType] + 7) / 8;
    }
    else
    {
        //    if (!(cFrameHdr & 4))
        //        return 0;

#ifdef IF2
        nFrameType = (Word16)(cFrameHdr >> 4);
#else
        nFrameType = (Word16)((cFrameHdr >> 3) & 0x0F);
#endif

        nFrameSize = block_size[nFrameType] - 1;
    }

    return nFrameSize;
}

/*********************************************************************\
* Function	   ImportSourceData
* Purpose      Loads source data into source buffer.
* Params	   pAmrDecoder
*					Pointer to the AMR decoder.
* Return       Returns the size of data that actually loaded. If the
*			   source data have been loaded completely or there are
*			   adequate data in the source buffer, it returns 0.
*			   Returns -1 if not successful.
* Remarks	   
**********************************************************************/

static int ImportSourceData(PMMDECODER pAmrDecoder)
{
    PMMDataSource pSource = NULL;
    PAMRMEMBER pMember = NULL;
    int nReadSize = 0, nSrcBufLeft;

    if (pAmrDecoder == NULL)
        return -1;

    if (pAmrDecoder->nSourceOffset == pAmrDecoder->nTotalSrcSize)
        return 0;

    pMember = (PAMRMEMBER)pAmrDecoder->nPrivate;

    if ((pAmrDecoder->nSrcDataSize > 0) && (pAmrDecoder->nSrcDataSize
        >= GetNextFrameSize(pMember->nType, *pAmrDecoder->pSrcDataPtr) + 1))
        return 0;

    pSource = pAmrDecoder->pSource;

    if (pAmrDecoder->nSrcDataSize == 0)
    {
        // If there is no data that can be read,
        // 如果SourceBuf缓冲区内无可读数据，
        // reads source data, and fills up SourceBuf buffer entirely
        // with them(if source data are adequate).
        // 读入源数据，并填充满SourceBuf缓冲区(如果数据量充足的话)
        nReadSize = pSource->mm_read(pAmrDecoder->SourceBuf,
            pAmrDecoder->nSBufSize, pAmrDecoder->nSourceOffset,
            pAmrDecoder->pParam);
    }
    else
    {
        // If there are data that can be read in the SourceBuf buffer,
        // but the remained data are less than a frame,
        // 如果SourceBuf缓冲区内有可读数据，但剩余数据不足一帧
        // moves the remained data to the top of buffer.
        // 移动剩余数据至缓冲区首部
        pSource->mm_memcpy(pAmrDecoder->SourceBuf, pAmrDecoder->pSrcDataPtr,
            pAmrDecoder->nSrcDataSize);
        pAmrDecoder->pSrcDataPtr = pAmrDecoder->SourceBuf
            + pAmrDecoder->nSrcDataSize;

        nSrcBufLeft = pAmrDecoder->nSBufSize - pAmrDecoder->nSrcDataSize;
        // Reads the source data, and fills up the left SourceBuf
        // buffer with them(if source data is adequate).
        // 读入源数据，并填充满剩余SourceBuf缓冲区(如果数据量充足的话)
        nReadSize = pSource->mm_read(pAmrDecoder->pSrcDataPtr,
            nSrcBufLeft, pAmrDecoder->nSourceOffset,
            pAmrDecoder->pParam);
    }

    pAmrDecoder->pSrcDataPtr = pAmrDecoder->SourceBuf;
    pAmrDecoder->nSrcDataSize += nReadSize;

    return nReadSize;
}

/*********************************************************************\
* Function	   DecodeCertainFrame
* Purpose      Decodes a certain frame into decode buffer.
* Params	   pAmrDecoder
*                  Pointer to the AMR decoder
* Return       Returns the size of decoded data or zero if there are
*			   some data left in the decode buffer. Returns -1 If not
*			   successful.
* Remarks
**********************************************************************/

static int DecodeCertainFrame(PMMDECODER pAmrDecoder)
{
    PAMRMEMBER pMember = NULL;
    int nCodSize = 0, nFrameSize = 0;

    if (pAmrDecoder == NULL)
        return -1;

    if (pAmrDecoder->nDecDataSize > 0)
        return 0;

    pMember = (PAMRMEMBER)pAmrDecoder->nPrivate;

    if (pMember->nType == AMR_TYPE_NB)
    {
        pAmrDecoder->pSource->mm_memset(pMember->pCodDataPtr, 0, 32);
        nCodSize = ConvertAmrIntoCod(pAmrDecoder);

        Decoder_Interface_Decode(pMember->pDestate,
            pMember->pCodDataPtr, (short *)pAmrDecoder->DecodeBuf, 0);
    }
    else
    {
        D_IF_decode(pMember->pDestate,
            pAmrDecoder->pSrcDataPtr, (short *)pAmrDecoder->DecodeBuf,
            _good_frame);

        nFrameSize = GetNextFrameSize(pMember->nType, *pAmrDecoder->pSrcDataPtr);
        pAmrDecoder->pSrcDataPtr += nFrameSize + 1;
        pAmrDecoder->nSrcDataSize -= nFrameSize + 1;
    }

    pAmrDecoder->pDecDataPtr = pAmrDecoder->DecodeBuf;
    pAmrDecoder->nDecDataSize += pAmrDecoder->nDBufSize;

    return pAmrDecoder->nDecDataSize;
}

/*********************************************************************\
* Function	   ExportDecodedData
* Purpose      Copies the decoded data in the decode buffer to the
*			   specified buffer.
* Params	   pAmrDecoder
*                  Pointer to the AMR decoder
*			   pBuf
*                  Pointer to an AMR data stream
*			   nReqSize
*                  Size of the required data to decode
*			   nCopiedTotal
*                  Size of the decoded data that have been copied
* Return	   Returns the size of data that actually copied or -1
*			   otherwise.
* Remarks	   
**********************************************************************/

static int ExportDecodedData(PMMDECODER pAmrDecoder, unsigned char *pBuf,
                      int nReqSize, int nCopiedTotal)
{
    int nReqLeft = 0, nExportSize = 0;

    if (pAmrDecoder == NULL)
        return -1;

    nReqLeft = nReqSize - nCopiedTotal;
    nExportSize = (nReqLeft > pAmrDecoder->nDecDataSize) ?
        pAmrDecoder->nDecDataSize : nReqLeft;

    pAmrDecoder->pSource->mm_memcpy(pBuf, pAmrDecoder->pDecDataPtr,
        nExportSize);
    pAmrDecoder->pDecDataPtr += nExportSize;
    pAmrDecoder->nDecDataSize -= nExportSize;

    return nExportSize;
}
