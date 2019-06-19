/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : playamr.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "plx_pdaex.h"
#include "string.h"
#include "stdio.h"
#include "malloc.h"

#include "prioman.h"
#include "plxmm.h"

#define	WAVE_BUF_NUM    2
#define	WAVE_BUF_SIZE   2048
#define	PERFERSIZE      2048

/////////////////////////////////////////////////////////////////
static FILE *pAMRFile;
static HDECODER hDecoder;
static HWAVEOUT hWaveOut;
static WAVEHDR WaveHDR[WAVE_BUF_NUM];
static BYTE WaveBuf[WAVE_BUF_NUM][WAVE_BUF_SIZE];
static BOOL bPlayData, bMMSDataEnd;
static unsigned char* pMMSDataBuf;
static int nMMSDataLen;
static BOOL bPlayOver;
static int nAudPrio;

/////////////////////////////////////////////////////////////////
HDECODER amrDecoderCreate(int random, PMMDataSource pSource, int nSourceSize, int nPreferBufSize, void *pParam);
int amrDecoderDestroy(HDECODER hDecoder);
int amrDecoderGetTotalTime(HDECODER hDecoder);
int amrDecoderSeek(HDECODER hDecoder, int nSeconds);
int amrDecoderGetData(HDECODER hDecoder, unsigned char *pBuf, int nReqSize);
int amrDecoderGetAudioInfo(HDECODER hDecoder, PAUDIOINFO pInfo);

/////////////////////////////////////////////////////////////////
#ifdef _EMULATE_
void FAR __stdcall amrDecoderCallback(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
#else
void CALLBACK amrDecoderCallback(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
#endif // _EMULATE_

void *AMR_Malloc(unsigned int size);
void  AMR_Free(void *memblock);
void *AMR_Realloc(void *memblock, unsigned int size);
void *AMR_Memset(void *dest, int c, unsigned int count);
void *AMR_Memcpy(void *dest, const void *src, unsigned int count);
void *AMR_Memmove(void *dest, const void *src, unsigned int count);

int   AMR_Read(unsigned char *pBuf, int nSize, int nOffset, void *pParam);
int   AMR_ReadData(unsigned char *pBuf, int nSize, int nOffset, void *pParam);

/*********************************************************************
* Function	AMRDecodeBegin
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int AMRDecodeBegin(HWND hWnd, LPSTR pstrName, unsigned char* pDataBuf, int nDataLen, int nPriority)
{
	MMDataSource ds;
	AUDIOINFO ai;
	WAVEFORMATEX wfx;
	int nResult, i, size;

	if (pstrName != NULL)
	{
		pAMRFile = fopen(pstrName, "rb");
		if (pAMRFile == NULL)
			return PRIOMAN_ERROR_OPENFAIL;

		// Music file
		bPlayData = FALSE;

		fseek(pAMRFile, 0, SEEK_END);
		size = ftell(pAMRFile);

		ds.mm_malloc = AMR_Malloc;
		ds.mm_free = AMR_Free;
		ds.mm_read = AMR_Read;
		ds.mm_realloc = AMR_Realloc;
		ds.mm_memcpy = AMR_Memcpy;
		ds.mm_memmove = AMR_Memmove;
		ds.mm_memset = AMR_Memset;

		hDecoder = amrDecoderCreate(DECODER_ATTRIB_RANDOM, &ds, size, PERFERSIZE, pAMRFile);
		if (hDecoder == NULL)
			return PRIOMAN_ERROR_DECORDER;
	}
	else
	{
		// MMS Data
		bPlayData = TRUE;
		bMMSDataEnd = FALSE;
		nMMSDataLen = nDataLen;

		ds.mm_malloc = AMR_Malloc;
		ds.mm_free = AMR_Free;
		ds.mm_read = AMR_ReadData;
		ds.mm_realloc = AMR_Realloc;
		ds.mm_memcpy = AMR_Memcpy;
		ds.mm_memmove = AMR_Memmove;
		ds.mm_memset = AMR_Memset;

		pMMSDataBuf = (unsigned char*)malloc(nMMSDataLen+1);
		if (pMMSDataBuf == NULL)
			return PRIOMAN_ERROR_ALLOCMEM;
		memcpy(pMMSDataBuf, pDataBuf, nMMSDataLen);

		hDecoder = amrDecoderCreate(DECODER_ATTRIB_RANDOM, &ds, nDataLen, PERFERSIZE, pMMSDataBuf);
		if (hDecoder == NULL)
			return PRIOMAN_ERROR_DECORDER;
	}

	nResult = amrDecoderGetTotalTime(hDecoder);
	if (nResult <= 0)
		return PRIOMAN_ERROR_GETPLAYTIME;

	nResult = amrDecoderGetAudioInfo(hDecoder, &ai);
	if (nResult < 0)
		return PRIOMAN_ERROR_GETAUDIOINFO;

	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = ai.nChannel;
	wfx.wBitsPerSample = ai.nSampleBits;
	wfx.nSamplesPerSec = ai.nSampleRate;
	wfx.nBlockAlign = wfx.nChannels*wfx.wBitsPerSample/8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec*wfx.nBlockAlign;

	bPlayOver = FALSE;
	nAudPrio = nPriority;
	nResult = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, (DWORD)amrDecoderCallback, 0, CALLBACK_FUNCTION);
	if (nResult != MMSYSERR_NOERROR)
		return PRIOMAN_ERROR_WAVEOPEN;

	for (i=0; i<WAVE_BUF_NUM; i++)
	{
		memset(&WaveHDR[i], 0, sizeof(WAVEHDR));
		WaveHDR[i].lpData = (char *)WaveBuf[i];
		WaveHDR[i].dwBufferLength = WAVE_BUF_SIZE;
		WaveHDR[i].dwUser = (DWORD)hWnd;
		WaveHDR[i].dwFlags = WHDR_DONE;

		nResult = waveOutPrepareHeader(hWaveOut, &WaveHDR[i], sizeof(WAVEHDR));
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_PREPARE;
	}

	for (i=0; i<WAVE_BUF_NUM; i++)
	{
		nResult = amrDecoderGetData(hDecoder, WaveBuf[i], WAVE_BUF_SIZE);
		if (nResult <= 0)
			return PRIOMAN_ERROR_GETDECDATA;
	}

	for (i=0; i<WAVE_BUF_NUM; i++)
	{
		nResult = waveOutWrite(hWaveOut, &WaveHDR[i], sizeof(WAVEHDR));
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_WRITEOUT;
	}

	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	AMRDecodeEnd
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int AMRDecodeEnd(void)
{
	int nResult, i;

	if (bPlayData)
	{
		if (bMMSDataEnd)
			return PRIOMAN_ERROR_SUCCESS;

		free(pMMSDataBuf);
		pMMSDataBuf = NULL;
		bMMSDataEnd = TRUE;
	}
	else
	{
		if (pAMRFile == NULL)
			return PRIOMAN_ERROR_SUCCESS;

		fclose(pAMRFile);
		pAMRFile = NULL;
	}

	if (hWaveOut != NULL)
	{
		nResult = waveOutReset(hWaveOut);
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_RESET;

		for (i=0; i<WAVE_BUF_NUM; i++)
		{
			nResult = waveOutUnprepareHeader(hWaveOut, &WaveHDR[i], sizeof(WAVEHDR));
			if (nResult != MMSYSERR_NOERROR)
				return PRIOMAN_ERROR_PREPARE;
		}

		nResult = waveOutClose(hWaveOut);
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_WAVECLOSE;

		// !!!!!!
		hWaveOut = NULL;
	}

	if (hDecoder != NULL)
	{
		amrDecoderDestroy(hDecoder);
		hDecoder= NULL;
	}

	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	AMRReadData
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void AMRReadData(WPARAM wParam, LPARAM lParam)
{
	LPWAVEHDR pHDR;
	int nReadLen;

	pHDR = (LPWAVEHDR)lParam;
	if (pHDR == NULL)
		return;

	if (bPlayData)
	{
		if (bMMSDataEnd)
			return;
	}
	else
	{
		if (pAMRFile == NULL)
			return;
	}

	if (hDecoder == NULL)
		return;

	nReadLen = amrDecoderGetData(hDecoder, (unsigned char *)pHDR->lpData, WAVE_BUF_SIZE);
	if (nReadLen <= 0)
	{
		bPlayOver = TRUE;
		SendMessage((HWND)pHDR->dwUser, PRIOMAN_MESSAGE_PLAYOVER, NULL, NULL);
		return;
	}

	PostMessage((HWND)pHDR->dwUser, PRIOMAN_MESSAGE_WRITEDATA, nAudPrio, (LPARAM)pHDR);
}

/*********************************************************************
* Function	AMRWriteData
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void AMRWriteData(WPARAM wParam, LPARAM lParam)
{
	LPWAVEHDR pHDR;
	int nWriteLen;

	pHDR = (LPWAVEHDR)lParam;
	if (pHDR == NULL)
		return;

	if (hWaveOut == NULL)
		return;

	nWriteLen = waveOutWrite(hWaveOut, pHDR, sizeof(WAVEHDR));
	if (nWriteLen != MMSYSERR_NOERROR)
		return;
}

/*********************************************************************
* Function	amrDecoderCallback
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
#ifdef _EMULATE_
void FAR __stdcall amrDecoderCallback(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
#else
void CALLBACK amrDecoderCallback(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
#endif // _EMULATE_
{
	LPWAVEHDR pHDR;

	if (uMsg == WOM_OPEN)
	{
		bPlayOver = FALSE;
		return;
	}

	if (uMsg == WOM_CLOSE)
		return;

	if (uMsg == WOM_DONE)
	{
		pHDR = (LPWAVEHDR)dwParam1;
		if (pHDR == NULL)
			return;

		if (!bPlayOver)
			PostMessage((HWND)pHDR->dwUser, PRIOMAN_MESSAGE_READDATA, nAudPrio, (LPARAM)pHDR);
	}
}

/*********************************************************************
* Function	AMR_Malloc
* Purpose   
* Parameter	
* Return	void *
* Remarks	
**********************************************************************/
void *AMR_Malloc(unsigned int size)
{
	void *memblock;

	memblock = malloc(size);
	if (memblock == NULL)
		return NULL;

	return memblock;
}

/*********************************************************************
* Function	AMR_Free
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void AMR_Free(void *memblock)
{
	free(memblock);
}

/*********************************************************************
* Function	AMR_Realloc
* Purpose   
* Parameter	
* Return	void *
* Remarks	
**********************************************************************/
void *AMR_Realloc(void *memblock, unsigned int size)
{
	return realloc(memblock, size);
}

/*********************************************************************
* Function	AMR_Memset
* Purpose   
* Parameter	
* Return	void *
* Remarks	
**********************************************************************/
void *AMR_Memset(void *dest, int c, unsigned int count)
{
	return memset(dest, c, count);
}

/*********************************************************************
* Function	AMR_Memcpy
* Purpose   
* Parameter	
* Return	void *
* Remarks	
**********************************************************************/
void *AMR_Memcpy(void *dest, const void *src, unsigned int count)
{
	return memcpy(dest, src, count);
}

/*********************************************************************
* Function	AMR_Memmove
* Purpose   
* Parameter	
* Return	void *
* Remarks	
**********************************************************************/
void *AMR_Memmove(void *dest, const void *src, unsigned int count)
{
	return memmove(dest, src, count);
}

/*********************************************************************
* Function	AMR_Read
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int AMR_Read(unsigned char *pBuf, int nSize, int nOffset, void *pParam)
{
	FILE* pFile;

	pFile = (FILE *)pParam;
	if (pFile == NULL)
		return -1;

	fseek(pFile, nOffset, SEEK_SET);

	return fread(pBuf, sizeof(unsigned char), nSize, pFile);
}

/*********************************************************************
* Function	AMR_ReadData
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int AMR_ReadData(unsigned char *pBuf, int nSize, int nOffset, void *pParam)
{
	unsigned char* pDataBuf;
	int nReadLen;

	pDataBuf = (unsigned char*)pParam;
	if (pDataBuf == NULL)
		return -1;

	if (nMMSDataLen-nOffset < nSize)
		nReadLen = nMMSDataLen-nOffset;
	else
		nReadLen = nSize;
	memcpy(pBuf, pDataBuf+nOffset, nReadLen);

	return nReadLen;
}
