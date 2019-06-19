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
	DWORD	v_nWriteBytes;		// 当前缓冲被写入的数据量
	DWORD	v_pBuf[QU_MAX_LEN];	// 数据缓冲
	BYTE	v_nBuf[QU_MAX_LEN];	// 状态

	BYTE	v_iRead;			// 读索引
	BYTE	v_iWrite;			// 写索引

} WQUEUE, *PWQUEUE;

typedef struct whandle_s
{
	HUT_ENTRY	v_hSemp;		// 唤醒信号
	HUT_ENTRY	v_hMutex;		// 数据互斥

	DWORD	v_hInstance;		// 实例句柄
	DWORD	v_iDevive;			// 输出设备标识符
	DWORD	v_nAttrib;			// 运行状态

	DWORD	v_CbStyle;			// 回调方式
	DWORD	v_CbParam;			// 回调参数

	WQUEUE	v_aQueue;			// 数据队列
	WAVEFORMATEX	v_wfx;		// 波形声音数据格式

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

	// 检查输入参数是否合法
	if (phwo == NULL || pwfx == NULL)
		return MMSYSERR_INVALPARAM;

	// 检查格式类型与输出设备标识符
	*phwo = NULL;
	if (pwfx->wFormatTag != WAVE_FORMAT_PCM || uDeviceID != WAVE_MAPPER)
		return WAVERR_BADFORMAT;

	// 检查回调函数地址
	if ((fdwOpen & CALLBACK_TYPEMASK) != CALLBACK_NULL)
	{
		if (dwCallback == 0)
			return MMSYSERR_NOTSUPPORTED;

		if ((fdwOpen & CALLBACK_TYPEMASK) != CALLBACK_FUNCTION &&
			(fdwOpen & CALLBACK_TYPEMASK) != CALLBACK_WINDOW)
			return MMSYSERR_NOTSUPPORTED;
	}

	// 创建线程参数
	if (g_hProc == NULL)
	{
		if (g_hSemp == NULL)
		{
			// 创建线程信号量
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
			// 创建线程互斥锁
			if (!f_utCreateMutex(&g_hMutex))
				return MMSYSERR_NOMEM;

			if (!g_hMutex->f_Operate(g_hMutex, OSM_INITIALIZE, (unsigned long)"threadmutex", 0))
			{
				g_hMutex->f_Operate(g_hMutex, OSM_RELEASE, 0, 0);
				g_hMutex = NULL;
				return MMSYSERR_NOMEM;
			}
		}

		// 创建后台线程
		if (!f_utCreateThread(&g_hProc))
			return MMSYSERR_NOMEM;

		if (!g_hProc->f_Operate(g_hProc, OSM_INITIALIZE, 0, (unsigned long)f_MainProc))
		{
			g_hProc->f_Operate(g_hProc, OSM_RELEASE, 0, 0);
			g_hProc = NULL;
			return MMSYSERR_NOMEM;
		}

		// 关闭设备用
		close_semp = CreateSemaphore(IPC_SEM, 0);
	}

	// 分配声音数据句柄
	pHandle = (PWHANDLE)mmi_Malloc(sizeof(WHANDLE));
	if (pHandle == NULL)
		return MMSYSERR_NOMEM;

	// 打开输出设备
	if (!f_InitDevice(pwfx, &dwDeviceID))
	{
		mmi_Free(pHandle);
		return MMSYSERR_ALLOCATED;
	}

	// 初始化声音数据句柄
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

	// 创建数据互斥锁
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

	// 创建数据信号量
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

	// 通知上层设备已打开
	f_NotifProc(pHandle, WOM_OPEN, 0, 0);

	// 新建波形输出节点
	pHwoNode = (PHWONODE)mmi_Malloc(sizeof(HWONODE));
	if (pHwoNode == NULL)
	{
		mmi_Free(pHandle);
		return MMSYSERR_NOMEM;
	}

	// 加入节点链表(只有一个节点啦！)
	g_hwoNode = pHwoNode;
	g_hwoNode->hwo = (HWAVEOUT)pHandle;
	g_hwoNode->pNext = NULL;

#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: 唤醒线程信号量\r\n");
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

	// 检查输入参数是否合法
	if (hwo == NULL)
		return MMSYSERR_INVALPARAM;

	// 打开mixer设备
	fd = open("/dev/mixer", O_WRONLY);
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: Open mixer device: %d\r\n", fd);
#endif
	if (fd < 0)
		return MMSYSERR_NOTSUPPORTED;

	// 设置音量大小
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

	// 关闭mixer设备
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

	// 检查输入参数是否合法
	if (hwo == NULL)
		return MMSYSERR_INVALPARAM;

	pHandle = (PWHANDLE)hwo;
	if (pHandle == NULL)
		return MMSYSERR_INVALPARAM;

	// 检查当前队列是否有数据
	if (pHandle->v_aQueue.v_iRead != pHandle->v_aQueue.v_iWrite)
		return WAVERR_STILLPLAYING;

	// 通知上层设备已关闭
	f_NotifProc(pHandle, WOM_CLOSE, 0, 0);
	g_nOpen--;

	// 结束本次播放
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: 数据互斥锁打开2\r\n");
#endif
	pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, MXM_LOCK, 0, 0);

	pHandle->v_nAttrib |= ATTR_RELEASE;

	// MAIN PROC TO WAIT Data Semaphore
	pHandle->v_hSemp->f_Operate(pHandle->v_hSemp, SEM_POST, 1, 0);

#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: 数据互斥锁关闭2\r\n");
#endif
	pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, MXM_UNLOCK, 0, 0);

#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: 等待关闭设备\r\n");
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

	// 检查输入参数是否合法
	if (hwo == NULL || pwh == NULL || cbwh == 0)
		return MMSYSERR_INVALPARAM;

	// 检查设备句柄是否合法
	pHandle = (PWHANDLE)hwo;
	if (!(pHandle->v_nAttrib & ATTR_OPEN))
		return MMSYSERR_INVALHANDLE;

	// 安装准备数据
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
	// 检查输入参数是否合法
	if (hwo == NULL || pwh == NULL)
		return MMSYSERR_INVALPARAM;

	// 检查当前是否有数据在队列中
	if (pwh->dwFlags & WHDR_INQUEUE)
		return WAVERR_STILLPLAYING;

	// 卸载准备数据
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

	// 检查输入参数是否合法
	if (hwo == NULL || pwh == NULL)
		return MMSYSERR_INVALPARAM;

	// 未准备好数据
	if (!(pwh->dwFlags & WHDR_PREPARED))
		return WAVERR_UNPREPARED;

	// 检查设备句柄是否合法
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
	printf("\r\nWAVEOUT: 唤醒数据信号量\r\n");
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

	// 检查输入参数是否合法
	if (hwo == NULL)
		return MMSYSERR_INVALPARAM;

	// 检查设备句柄是否合法
	pHandle = (PWHANDLE)hwo;
	if (!(pHandle->v_nAttrib & ATTR_OPEN))
		return MMSYSERR_INVALHANDLE;

	// 运行状态
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

	// 检查输入参数是否合法
	if (hwo == NULL)
		return MMSYSERR_INVALPARAM;

	// 检查设备句柄是否合法
	pHandle = (PWHANDLE)hwo;
	if (!(pHandle->v_nAttrib & ATTR_OPEN))
		return MMSYSERR_INVALHANDLE;

	// 运行状态
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

	// 检查输入参数是否合法
	if (hwo == NULL)
		return MMSYSERR_INVALPARAM;

	// 检查设备句柄是否合法
	pHandle = (PWHANDLE)hwo;
	if (!(pHandle->v_nAttrib & ATTR_OPEN))
		return MMSYSERR_INVALHANDLE;

	// 播放暂停/数据锁打开
	waveOutPause(hwo);
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: 数据互斥锁打开1\r\n");
#endif
	pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, MXM_LOCK, 0, 0);

	// 清理数据队列
	pQue = &pHandle->v_aQueue;
	pQue->v_nWriteBytes = 0;

	while (pQue->v_nBuf[pQue->v_iRead] != 0)
	{
		pHdr = (PWAVEHDR)(pQue->v_pBuf[pQue->v_iRead]);
		pQue->v_nBuf[pQue->v_iRead] = 0;

		pQue->v_iRead++;
		if (pQue->v_iRead >= QU_MAX_LEN)
			pQue->v_iRead = 0;

		// 通知上层播放已结束
		pHdr->dwFlags |= WHDR_DONE;
		f_NotifProc(pHandle, WOM_DONE, (DWORD)pHdr, 0);
	}

	// 播放继续/数据锁关闭
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: 数据互斥锁关闭1\r\n");
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
	printf("\r\nWAVEOUT: 等待线程信号量\r\n");
#endif
			g_hSemp->f_Operate(g_hSemp, SEM_WAITE, 1, 0);
			continue;
		}

		if (pHandle->v_nAttrib & ATTR_WAIT)
		{
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: 线程暂停状态\r\n");
#endif
			g_hSemp->f_Operate(g_hSemp, SEM_WAITE, 1, 0);
			continue;
		}

		// 结束本次播放
		if (pHandle->v_nAttrib & ATTR_RELEASE)
		{
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: 结束本次播放\r\n");
#endif
			// 释放节点资源
			mmi_Free(pHwoNode);
			g_hwoNode = NULL;
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: 释放数据资源\r\n");
#endif
			pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, OSM_RELEASE, 0, 0);
			pHandle->v_hSemp->f_Operate(pHandle->v_hSemp, OSM_RELEASE, 0, 0);
			// 关闭打开设备
			f_CloseDevice(pHandle);
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: 关闭设备结束\r\n");
#endif
			ReleaseSemaphore(close_semp, 1);
			continue;
		}

		// 检查队列中是否有读入数据
		pQue = &pHandle->v_aQueue;
		if (pQue->v_nBuf[pQue->v_iRead] == 0)
		{
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: 等待数据信号量\r\n");
#endif
			pHandle->v_hSemp->f_Operate(pHandle->v_hSemp, SEM_WAITE, 1, 0);
			continue;
		}

		// 波形数据写
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: 数据互斥锁打开3\r\n");
#endif
		pHandle->v_hMutex->f_Operate(pHandle->v_hMutex, MXM_LOCK, 0, 0);
		pHdr = (LPWAVEHDR)pQue->v_pBuf[pQue->v_iRead];

		ReadIndex = pQue->v_iRead;
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: 波形数据写开始\r\n");
#endif
		WriteBytes = write(pHandle->v_iDevive, pHdr->lpData + pQue->v_nWriteBytes,
			pHdr->dwBufferLength - pQue->v_nWriteBytes);
		if (WriteBytes >= 0)
		{
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: 波形数据写 = %d\r\n", WriteBytes);
#endif
			pQue->v_nWriteBytes += WriteBytes;
		}
		else
		{
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: 波形数据写失败\r\n");
#endif
		}

		if (pQue->v_nWriteBytes >= pHdr->dwBufferLength)
		{
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: 队列写满\r\n");
#endif
			pQue->v_nWriteBytes = 0;
			pQue->v_nBuf[pQue->v_iRead] = 0;
			pHdr->dwFlags |= WHDR_DONE;

			pQue->v_iRead++;
			if (pQue->v_iRead >= QU_MAX_LEN)
				pQue->v_iRead = 0;
		}

		// 处理回调过程
		while (ReadIndex != pQue->v_iRead)
		{
#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: 通知回调函数\r\n");
#endif
			f_NotifProc(pHandle, WOM_DONE, pQue->v_pBuf[ReadIndex], 0);

			ReadIndex++;
			if (ReadIndex >= QU_MAX_LEN)
				ReadIndex = 0;
		}

#ifdef _DEBUGALL_MMLIB_
	printf("\r\nWAVEOUT: 数据互斥锁关闭3\r\n");
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

	// 系统预留
	pHdr = (PWAVEHDR)dw1;
	if (pHdr != NULL)
	{
		pHdr->lpNext = NULL;
		pHdr->reserved = 0;
		pHdr->dwFlags &= ~WHDR_INQUEUE;
	}

	// 检查回调方式
	dwCallBack = pHandle->v_CbStyle & CALLBACK_TYPEMASK;
	if (dwCallBack == CALLBACK_NULL)
		return;

	// 检查回调参数
	if (pHandle->v_CbParam == 0)
		return;

	// 通知上层回调
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

	// 打开sound设备
	fd = open("/dev/sound", O_WRONLY);
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: Open sound device, fd = %d\r\n", fd);
#endif
	*pDeviceID = -1;
	if (fd < 0)
		return FALSE;

	// 设置采样率
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

	// 设置信道
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

	// 设置数据位
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

	// 判断耳机是否已经插上，返回TRUE说明已经插上，否则未插上
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

	// 关闭打开设备
#ifdef _DEBUG_MMLIB_
	printf("\r\nWAVEOUT: Close sound device, v_iDevive = %d\r\n", pHandle->v_iDevive);
#endif
	close(pHandle->v_iDevive);
	mmi_Free(pHandle);
}
