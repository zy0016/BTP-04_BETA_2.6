/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : waveout.c
 *
 * Purpose  : Implement interface for program designer
 *
\**************************************************************************/

#include <sys/ipc.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "utils.h"
#include "hopen/soundcard.h"
#include "compatable.h"
#include "plxmm.h"

#ifdef _DEBUG_MMLIB_
#undef _DEBUG_MMLIB_
#endif

#ifdef _DEBUGALL_MMLIB_
#undef _DEBUGALL_MMLIB_
#endif

#ifdef _NONBLOCK_MMLIB_
#undef _NONBLOCK_MMLIB_
#endif

#define	ATTR_OPEN		0x0001
#define	ATTR_START		0x0002
#define	ATTR_WAIT		0x0004
#define	ATTR_RELEASE	0x0008

#define	QU_MAX_LEN		32
#define QU_STATE_WRITE	0x0001

typedef struct wqueue_s
{
	DWORD	v_nWriteBytes;		// ��ǰ���屻д���������
	DWORD	v_pBuf[QU_MAX_LEN];	// ���ݻ���
	BYTE	v_nBuf[QU_MAX_LEN];	// ״̬

	BYTE	v_iRead;			// ������
	BYTE	v_iWrite;			// д����

} WQUEUE, *PWQUEUE;

typedef struct whandle_s
{
	HUT_ENTRY	v_hSemp;		// �����ź�
	HUT_ENTRY	v_hMutex;		// ���ݻ���

	DWORD	v_hInstance;		// ʵ�����
	DWORD	v_iDevive;			// ����豸��ʶ��
	DWORD	v_nAttrib;			// ����״̬

	DWORD	v_CbStyle;			// �ص���ʽ
	DWORD	v_CbParam;			// �ص�����

	WQUEUE	v_aQueue;			// ���ݶ���
	WAVEFORMATEX	v_wfx;		// �����������ݸ�ʽ

} WHANDLE, *PWHANDLE;

typedef struct hwonode_s
{
	struct hwonode_s *pNext;

	HWAVEOUT hwo;
} HWONODE, *PHWONODE;

static HUT_ENTRY g_hSemp = NULL;
static HUT_ENTRY g_hMutex = NULL;
static HUT_ENTRY g_hProc = NULL;

static PHWONODE g_hwoNode = NULL;
static DWORD g_nOpen = 0;
static HSEMP close_semp;

static void f_MainProc(LPVOID lpVoid);
static void f_NotifProc(PWHANDLE pHandle, UINT nCode, DWORD dw1, DWORD dw2);
static BOOL f_InitDevice(LPCWAVEFORMATEX pwfx, LPDWORD pDeviceID);
static void f_CloseDevice(PWHANDLE pHandle);

/*********************************************************************
* Function	waveOutOpen
* Purpose   
* Parameter	
* Return	MMRESULT
* Remarks	
**********************************************************************/
WINMMAPI MMRESULT WINAPI waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
	UT_SEM_OPEN	oSemp;
	PWHANDLE pHandle;
	PHWONODE pHwoNode;
	DWORD dwDeviceID;

	// �����������Ƿ�Ϸ�
	if (phwo == NULL || pwfx == NULL)
		return MMSYSERR_INVALPARAM;

	// ����ʽ����������豸��ʶ��
	*phwo = NULL;
	if (pwfx->wFormatTag != WAVE_FORMAT_PCM || uDeviceID != WAVE_MAPPER)
		return WAVERR_BADFORMAT;

	// ���ص�������ַ
	if ((fdwOpen & CALLBACK_TYPEMASK) != CALLBACK_NULL)
	{
		if (dwCallback == 0)
			return MMSYSERR_NOTSUPPORTED;

		if ((fdwOpen & CALLBACK_TYPEMASK) != CALLBACK_FUNCTION &&
			(fdwOpen & CALLBACK_TYPEMASK) != CALLBACK_WINDOW)
			return MMSYSERR_NOTSUPPORTED;
	}

	// �����̲߳���
	if (g_hProc == NULL)
	{
		if (g_hSemp == NULL)
		{
			// �����߳��ź���
			if (!f_utCreateSemaphore(&g_hSemp))
				return MMSYSERR_NOMEM;

			oSemp.v_nMax	= 1;
			oSemp.v_nShare	= 0;
			oSemp.v_nKey	= IPC_SEM;
			oSemp.v_nValue	= 0;
			oSemp.v_pName	= "threadsemp";
			if (!g_hSemp->f_Operate(g_hSemp, OSM_INITIALIZE, (unsigned long)&oSemp, 0))
			{
				g_hSemp->f_Operate(g_hSemp, OSM_RELEASE, 0, 0);
				g_hSemp = NULL;
				return MMSYSERR_NOMEM;
			}
		}

		if (g_hMutex == NULL)
		{
			// �����̻߳�����
			if (!f_utCreateMutex(&g_hMutex))
				return MMSYSERR_NOMEM;

			if (!g_hMutex->f_Operate(g_hMutex, OSM_INITIALIZE, (unsigned long)"threadmutex", 0))
			{
				g_hMutex->f_Operate(g_hMutex, OSM_RELEASE, 0, 0);
				g_hMutex = NULL;
				return MMSYSERR_NOMEM;
			}
		}

		// ������̨�߳�
		if (!f_utCreateThread(&g_hProc))
			return MMSYSERR_NOMEM;

		if (!g_hProc->f_Operate(g_hProc, OSM_INITIALIZE, 0, (unsigned long)f_MainProc))
		{
			g_hProc->f_Operate(g_hProc, OSM_RELEASE, 0, 0);
			g_hProc = NULL;
			return MMSYSERR_NOMEM;
		}

		// �ر��豸��
		close_semp = CreateSemaphore(IPC_SEM, 0);
	}

	// �����������ݾ��
	pHandle = (PWHANDLE)mmi_Malloc(sizeof(WHANDLE));
	if (pHandle == NULL)
		return MMSYSERR_NOMEM;

	// ������豸
	if (!f_InitDevice(pwfx, &dwDeviceID))
	{
		mmi_Free(pHandle);
		return MMSYSERR_ALLOCATED;
	}

	// ��ʼ���������ݾ��
	memset(pHandle, 0, sizeof(WHANDLE));
	pHandle->v_hInstance = dwInstance;
	pHandle->v_iDevive = dwDeviceID;
	pHandle->v_nAttrib = ATTR_OPEN | ATTR_START;
	pHandle->v_CbStyle = fdwOpen;
	pHandle->v_CbParam = dwCallback;

	pHandle->v_wfx.wFormatTag = pwfx->wFormatTag;
	pHandle->v_wfx.nChannels = pwfx->nChannels;
	pHandle->v_wfx.nSamplesPerSec = pwfx->nSamplesPerSec;
	pHandle->v_wfx.nAvgBytesPerSec = pwfx->nAvgBytesPerSec;
	pHandle->v_wfx.nBlockAlign = pwfx->nBlockAlign;
	pHandle->v_wfx.wBitsPerSample = pwfx->wBitsPerSample;
	pHandle->v_wfx.cbSize = pwfx->cbSize;

	// �������ݻ�����
	if (!f_utCreateMutex(&pHandle->v_hMutex))
	{
		mmi_Free(pHandle);
		return MMSYSERR_NOMEM;
	}

	if (!pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, OSM_INITIALIZE, (unsigned long)"datamutex", 0))
	{
		pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, OSM_RELEASE, 0, 0);
		pHandle->v_hMutex = NULL;
		return MMSYSERR_NOMEM;
	}

	// ���������ź���
	if (!f_utCreateSemaphore(&pHandle->v_hSemp))
	{
		mmi_Free(pHandle);
		return MMSYSERR_NOMEM;
	}

	oSemp.v_nMax	= 1;
	oSemp.v_nShare	= 0;
	oSemp.v_nKey	= IPC_SEM;
	oSemp.v_nValue	= 0;
	oSemp.v_pName	= "datasemp";
	if (!pHandle->v_hSemp->f_Operate(pHandle->v_hSemp, OSM_INITIALIZE, (unsigned long)&oSemp, 0))
	{
		pHandle->v_hSemp->f_Operate(pHandle->v_hSemp, OSM_RELEASE, 0, 0);
		pHandle->v_hSemp = NULL;
		return MMSYSERR_NOMEM;
	}

	// ֪ͨ�ϲ��豸�Ѵ�
	f_NotifProc(pHandle, WOM_OPEN, 0, 0);

	// �½���������ڵ�
	pHwoNode = (PHWONODE)mmi_Malloc(sizeof(HWONODE));
	if (pHwoNode == NULL)
	{
		mmi_Free(pHandle);
		return MMSYSERR_NOMEM;
	}

	// ����ڵ�����(ֻ��һ���ڵ�����)
	g_hwoNode = pHwoNode;
	g_hwoNode->hwo = (HWAVEOUT)pHandle;
	g_hwoNode->pNext = NULL;

#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: �����߳��ź���\r\n");
#endif
	g_hSemp->f_Operate(g_hSemp, SEM_POST, 1, 0);

	*phwo = pHwoNode->hwo;
	return MMSYSERR_NOERROR;
}

/*********************************************************************
* Function	waveOutSetVolume
* Purpose   
* Parameter	
* Return	MMRESULT
* Remarks	
**********************************************************************/
WINMMAPI MMRESULT WINAPI waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
{
	int fd;
	int val, ret;

	// �����������Ƿ�Ϸ�
	if (hwo == NULL)
		return MMSYSERR_INVALPARAM;

	// ��mixer�豸
	fd = open("/dev/mixer", O_WRONLY);
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: Open mixer device: %d\r\n", fd);
#endif
	if (fd < 0)
		return MMSYSERR_NOTSUPPORTED;

	// ����������С
	val = dwVolume;
	ret = ioctl(fd, SOUND_MIXER_WRITE_VOLUME, (char *)&val);
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: SOUND_MIXER_WRITE_VOLUME = %d\r\n", ret);
#endif
	if (ret < 0)
	{
		close(fd);
		return MMSYSERR_NOTSUPPORTED;
	}

	// �ر�mixer�豸
	close(fd);
	return MMSYSERR_NOERROR;
}

/*********************************************************************
* Function	waveOutClose
* Purpose   
* Parameter	
* Return	MMRESULT
* Remarks	
**********************************************************************/
WINMMAPI MMRESULT WINAPI waveOutClose(HWAVEOUT hwo)
{
	PWHANDLE pHandle;

	// �����������Ƿ�Ϸ�
	if (hwo == NULL)
		return MMSYSERR_INVALPARAM;

	pHandle = (PWHANDLE)hwo;
	if (pHandle == NULL)
		return MMSYSERR_INVALPARAM;

	// ��鵱ǰ�����Ƿ�������
	if (pHandle->v_aQueue.v_iRead != pHandle->v_aQueue.v_iWrite)
		return WAVERR_STILLPLAYING;

	// ֪ͨ�ϲ��豸�ѹر�
	f_NotifProc(pHandle, WOM_CLOSE, 0, 0);
	g_nOpen--;

	// �������β���
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: ���ݻ�������2\r\n");
#endif
	pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, MXM_LOCK, 0, 0);

	pHandle->v_nAttrib |= ATTR_RELEASE;

	// MAIN PROC TO WAIT Data Semaphore
	pHandle->v_hSemp->f_Operate(pHandle->v_hSemp, SEM_POST, 1, 0);

#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: ���ݻ������ر�2\r\n");
#endif
	pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, MXM_UNLOCK, 0, 0);

#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: �ȴ��ر��豸\r\n");
#endif
	WaitSemaphore(close_semp, -1);

	return MMSYSERR_NOERROR;
}

/*********************************************************************
* Function	waveOutPrepareHeader
* Purpose   
* Parameter	
* Return	MMRESULT
* Remarks	
**********************************************************************/
WINMMAPI MMRESULT WINAPI waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	PWHANDLE pHandle;

	// �����������Ƿ�Ϸ�
	if (hwo == NULL || pwh == NULL || cbwh == 0)
		return MMSYSERR_INVALPARAM;

	// ����豸����Ƿ�Ϸ�
	pHandle = (PWHANDLE)hwo;
	if (!(pHandle->v_nAttrib & ATTR_OPEN))
		return MMSYSERR_INVALHANDLE;

	// ��װ׼������
	pwh->dwFlags |= WHDR_PREPARED;

	return MMSYSERR_NOERROR;
}

/*********************************************************************
* Function	waveOutUnprepareHeader
* Purpose   
* Parameter	
* Return	MMRESULT
* Remarks	
**********************************************************************/
WINMMAPI MMRESULT WINAPI waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	// �����������Ƿ�Ϸ�
	if (hwo == NULL || pwh == NULL)
		return MMSYSERR_INVALPARAM;

	// ��鵱ǰ�Ƿ��������ڶ�����
	if (pwh->dwFlags & WHDR_INQUEUE)
		return WAVERR_STILLPLAYING;

	// ж��׼������
	pwh->dwFlags &= ~WHDR_PREPARED;
	pwh->dwFlags &= ~WHDR_DONE;

	return MMSYSERR_NOERROR;
}

/*********************************************************************
* Function	waveOutWrite
* Purpose   
* Parameter	
* Return	MMRESULT
* Remarks	
**********************************************************************/
WINMMAPI MMRESULT WINAPI waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	PWHANDLE pHandle;
	PWQUEUE pQue;

	// �����������Ƿ�Ϸ�
	if (hwo == NULL || pwh == NULL)
		return MMSYSERR_INVALPARAM;

	// δ׼��������
	if (!(pwh->dwFlags & WHDR_PREPARED))
		return WAVERR_UNPREPARED;

	// ����豸����Ƿ�Ϸ�
	pHandle = (PWHANDLE)hwo;
	if (!(pHandle->v_nAttrib & ATTR_OPEN))
		return MMSYSERR_INVALHANDLE;

	pQue = &pHandle->v_aQueue;
	if (pQue->v_nBuf[pQue->v_iWrite] != 0)
		return MMSYSERR_NOMEM;

	pwh->reserved = (DWORD)hwo;
	pwh->dwFlags |= WHDR_INQUEUE;
	pQue->v_pBuf[pQue->v_iWrite] = (DWORD)pwh;
	pQue->v_nBuf[pQue->v_iWrite] = QU_STATE_WRITE;

	pQue->v_iWrite++;
	if (pQue->v_iWrite == QU_MAX_LEN)
		pQue->v_iWrite = 0;

	if (pHandle->v_nAttrib & ATTR_START)
	{
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: ���������ź���\r\n");
#endif
		pHandle->v_hSemp->f_Operate(pHandle->v_hSemp, SEM_POST, 1, 0);
	}

	return MMSYSERR_NOERROR;
}

/*********************************************************************
* Function	waveOutPause
* Purpose   
* Parameter	
* Return	MMRESULT
* Remarks	
**********************************************************************/
WINMMAPI MMRESULT WINAPI waveOutPause(HWAVEOUT hwo)
{
	PWHANDLE pHandle;

	// �����������Ƿ�Ϸ�
	if (hwo == NULL)
		return MMSYSERR_INVALPARAM;

	// ����豸����Ƿ�Ϸ�
	pHandle = (PWHANDLE)hwo;
	if (!(pHandle->v_nAttrib & ATTR_OPEN))
		return MMSYSERR_INVALHANDLE;

	// ����״̬
	pHandle->v_nAttrib &= ~ATTR_START;
	pHandle->v_nAttrib |= ATTR_WAIT;

	// MAIN PROC TO WAIT Data Semaphore
	pHandle->v_hSemp->f_Operate(pHandle->v_hSemp, SEM_POST, 1, 0);

	return MMSYSERR_NOERROR;
}

/*********************************************************************
* Function	waveOutRestart
* Purpose   
* Parameter	
* Return	MMRESULT
* Remarks	
**********************************************************************/
WINMMAPI MMRESULT WINAPI waveOutRestart(HWAVEOUT hwo)
{
	PWHANDLE pHandle;

	// �����������Ƿ�Ϸ�
	if (hwo == NULL)
		return MMSYSERR_INVALPARAM;

	// ����豸����Ƿ�Ϸ�
	pHandle = (PWHANDLE)hwo;
	if (!(pHandle->v_nAttrib & ATTR_OPEN))
		return MMSYSERR_INVALHANDLE;

	// ����״̬
	pHandle->v_nAttrib |= ATTR_START;
	pHandle->v_nAttrib &= ~ATTR_WAIT;

	// MAIN PROC TO WAIT Thread Semaphore
	g_hSemp->f_Operate(g_hSemp, SEM_POST, 1, 0);

	return MMSYSERR_NOERROR;
}

/*********************************************************************
* Function	waveOutReset
* Purpose   
* Parameter	
* Return	MMRESULT
* Remarks	
**********************************************************************/
WINMMAPI MMRESULT WINAPI waveOutReset(HWAVEOUT hwo)
{
	PWHANDLE pHandle;
	LPWAVEHDR pHdr;
	PWQUEUE pQue;

	// �����������Ƿ�Ϸ�
	if (hwo == NULL)
		return MMSYSERR_INVALPARAM;

	// ����豸����Ƿ�Ϸ�
	pHandle = (PWHANDLE)hwo;
	if (!(pHandle->v_nAttrib & ATTR_OPEN))
		return MMSYSERR_INVALHANDLE;

	// ������ͣ/��������
	waveOutPause(hwo);
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: ���ݻ�������1\r\n");
#endif
	pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, MXM_LOCK, 0, 0);

	// �������ݶ���
	pQue = &pHandle->v_aQueue;
	pQue->v_nWriteBytes = 0;

	while (pQue->v_nBuf[pQue->v_iRead] != 0)
	{
		pHdr = (PWAVEHDR)(pQue->v_pBuf[pQue->v_iRead]);
		pQue->v_nBuf[pQue->v_iRead] = 0;

		pQue->v_iRead++;
		if (pQue->v_iRead >= QU_MAX_LEN)
			pQue->v_iRead = 0;

		// ֪ͨ�ϲ㲥���ѽ���
		pHdr->dwFlags |= WHDR_DONE;
		f_NotifProc(pHandle, WOM_DONE, (DWORD)pHdr, 0);
	}

	// ���ż���/�������ر�
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: ���ݻ������ر�1\r\n");
#endif
	pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, MXM_UNLOCK, 0, 0);
	waveOutRestart(hwo);

	return MMSYSERR_NOERROR;
}

/*********************************************************************
* Function	waveOutSetChannel
* Purpose   
* Parameter	
* Return	MMRESULT
* Remarks	
**********************************************************************/
WINMMAPI MMRESULT WINAPI waveOutSetChannel(HWAVEOUT hwo, BOOL bSet, DWORD nflags)
{
	return MMSYSERR_NOERROR;
}

/*********************************************************************
* Function	f_MainProc
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
static void f_MainProc(LPVOID lpVoid)
{
	PWHANDLE pHandle;
	PHWONODE pHwoNode;
	LPWAVEHDR pHdr;
	PWQUEUE pQue;
	int WriteBytes;
	int ReadIndex;

	while (1)
	{
		if ((pHwoNode = g_hwoNode) == NULL ||
			(pHandle = (PWHANDLE)pHwoNode->hwo) == NULL)
		{
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: �ȴ��߳��ź���\r\n");
#endif
			g_hSemp->f_Operate(g_hSemp, SEM_WAITE, 1, 0);
			continue;
		}

		if (pHandle->v_nAttrib & ATTR_WAIT)
		{
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: �߳���ͣ״̬\r\n");
#endif
			g_hSemp->f_Operate(g_hSemp, SEM_WAITE, 1, 0);
			continue;
		}

		// �������β���
		if (pHandle->v_nAttrib & ATTR_RELEASE)
		{
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: �������β���\r\n");
#endif
			// �ͷŽڵ���Դ
			mmi_Free(pHwoNode);
			g_hwoNode = NULL;
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: �ͷ�������Դ\r\n");
#endif
			pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, OSM_RELEASE, 0, 0);
			pHandle->v_hSemp->f_Operate(pHandle->v_hSemp, OSM_RELEASE, 0, 0);
			// �رմ��豸
			f_CloseDevice(pHandle);
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: �ر��豸����\r\n");
#endif
			ReleaseSemaphore(close_semp, 1);
			continue;
		}

		// ���������Ƿ��ж�������
		pQue = &pHandle->v_aQueue;
		if (pQue->v_nBuf[pQue->v_iRead] == 0)
		{
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: �ȴ������ź���\r\n");
#endif
			pHandle->v_hSemp->f_Operate(pHandle->v_hSemp, SEM_WAITE, 1, 0);
			continue;
		}

		// ��������д
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: ���ݻ�������3\r\n");
#endif
		pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, MXM_LOCK, 0, 0);
		pHdr = (LPWAVEHDR)pQue->v_pBuf[pQue->v_iRead];

		ReadIndex = pQue->v_iRead;
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: ��������д��ʼ\r\n");
#endif
		WriteBytes = write(pHandle->v_iDevive, pHdr->lpData + pQue->v_nWriteBytes,
			pHdr->dwBufferLength - pQue->v_nWriteBytes);
		if (WriteBytes >= 0)
		{
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: ��������д = %d\r\n", WriteBytes);
#endif
			pQue->v_nWriteBytes += WriteBytes;
		}
		else
		{
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: ��������дʧ��\r\n");
#endif
		}

		if (pQue->v_nWriteBytes >= pHdr->dwBufferLength)
		{
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: ����д��\r\n");
#endif
			pQue->v_nWriteBytes = 0;
			pQue->v_nBuf[pQue->v_iRead] = 0;
			pHdr->dwFlags |= WHDR_DONE;

			pQue->v_iRead++;
			if (pQue->v_iRead >= QU_MAX_LEN)
				pQue->v_iRead = 0;
		}

		// ����ص�����
		while (ReadIndex != pQue->v_iRead)
		{
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: ֪ͨ�ص�����\r\n");
#endif
			f_NotifProc(pHandle, WOM_DONE, pQue->v_pBuf[ReadIndex], 0);

			ReadIndex++;
			if (ReadIndex >= QU_MAX_LEN)
				ReadIndex = 0;
		}

#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: ���ݻ������ر�3\r\n");
#endif
		pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, MXM_UNLOCK, 0, 0);
	}
}

/*********************************************************************
* Function	f_NotifProc
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
static void f_NotifProc(PWHANDLE pHandle, UINT nCode, DWORD dw1, DWORD dw2)
{
	LPWAVECALLBACK lpCallBack;
	PWAVEHDR pHdr;
	DWORD dwCallBack;

	// ϵͳԤ��
	pHdr = (PWAVEHDR)dw1;
	if (pHdr != NULL)
	{
		pHdr->lpNext = NULL;
		pHdr->reserved = 0;
		pHdr->dwFlags &= ~WHDR_INQUEUE;
	}

	// ���ص���ʽ
	dwCallBack = pHandle->v_CbStyle & CALLBACK_TYPEMASK;
	if (dwCallBack == CALLBACK_NULL)
		return;

	// ���ص�����
	if (pHandle->v_CbParam == 0)
		return;

	// ֪ͨ�ϲ�ص�
	switch (dwCallBack) {
	case CALLBACK_WINDOW:
		PostMessage((HWND)pHandle->v_CbParam, nCode, (DWORD)pHandle, dw1);
		break;
	case CALLBACK_FUNCTION:
		lpCallBack = (LPWAVECALLBACK)pHandle->v_CbParam;
		lpCallBack((HANDLE)pHandle, nCode, pHandle->v_hInstance, dw1, dw2);
		break;
	default:
		break;
	}
}

/*********************************************************************
* Function	f_InitDevice
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
static BOOL f_InitDevice(LPCWAVEFORMATEX pwfx, LPDWORD pDeviceID)
{
	int fd;
	int val, ret;

	// ��sound�豸
	fd = open("/dev/sound", O_WRONLY);
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: Open sound device, fd = %d\r\n", fd);
#endif
	*pDeviceID = -1;
	if (fd < 0)
		return FALSE;

	// ���ò�����
	val = pwfx->nSamplesPerSec;
	ret = ioctl(fd, SNDCTL_DSP_SPEED, (char *)&val);
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: SNDCTL_DSP_SPEED = %d\r\n", ret);
#endif
	if (ret < 0)
	{
		close(fd);
		return FALSE;
	}

	// �����ŵ�
	val = pwfx->nChannels;
	ret = ioctl(fd, SNDCTL_DSP_CHANNELS, (char *)&val);
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: SNDCTL_DSP_CHANNELS = %d\r\n", ret);
#endif
	if (ret < 0)
	{
		close(fd);
		return FALSE;
	}

	// ��������λ
	val = pwfx->wBitsPerSample;
	ret = ioctl(fd, SNDCTL_DSP_SETFMT, (char *)&val);
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: SNDCTL_DSP_SETFMT = %d\r\n", ret);
#endif
	if (ret < 0)
	{
		close(fd);
		return FALSE;
	}

	// �ж϶����Ƿ��Ѿ����ϣ�����TRUE˵���Ѿ����ϣ�����δ����
	if (GetHeadSetStatus())
	{
		printf("\r\nAudio route is set to external speaker.\r\n");
		ioctl(fd, SNDCTL_SET_EXTERNAL_CALL);
	}
	else
	{
		printf("\r\nAudio route is set to internal speaker.\r\n");
		ioctl(fd, SNDCTL_SET_INTERNAL_CALL);
	}

	*pDeviceID = fd;
	g_nOpen++;

	return TRUE;
}

/*********************************************************************
* Function	f_CloseDevice
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
static void f_CloseDevice(PWHANDLE pHandle)
{
	if (pHandle == NULL)
		return;

	// �رմ��豸
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: Close sound device, v_iDevive = %d\r\n", pHandle->v_iDevive);
#endif
	close(pHandle->v_iDevive);
	mmi_Free(pHandle);
}
