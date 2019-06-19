/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : smsdptch.c
 *
 * Purpose  : 短信分发模块外部接口函数和内部函数的实现.
 *
\**************************************************************************/

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <window.h>
#include <hp_modcfg.h>
#include <project/log.h>

#include "dptch.h"

#ifdef SMSDISPATCH_DEBUG

	extern void* wapalloc( int size );
	extern int wapfree( void* pBuf ); 	
	extern BOOL WAP_IsMemCleanup();
	#define SMSDptchMalloc		wapalloc
	#define SMSDptchFree		wapfree

#else

	#define SMSDptchMalloc		malloc
	#define SMSDptchFree		free

#endif

#ifdef SMSDISPATCH_ASSERT

	#include "assert.h"
	#define SMSDptchAssert	assert

#else

	#define SMSDptchAssert(x)

#endif

/*以下队列数据结构解决读短信index的匹配问题。*/
#define QUEUE_LEN	10

/*
*	需要队列保存的信息。
*/
typedef struct  tagSMSITEM
{
	int index;
	int devtype;
}SMSITEM;

static struct QUEUE
{
	BOOL bFull;
	int iHead;
	int iTail;
	SMSITEM astSMSITEM[QUEUE_LEN];/*长度可以考虑为SIM卡存储短信的个数*/

}g_stQueue;

#ifdef TI_MODULE
/*该结构用于解决TI模块中拨号和ME删除的问题。*/

#define DEL_QUEUE_LEN 5

struct INFO
{
	HWND hwnd;
	int	msg;
	int lWparam;
	int hWparam;
	
};

static struct  SMSDELQUEUE{
	BOOL bFull;
	int iHead;
	int iTail;
	struct INFO astInfo[DEL_QUEUE_LEN];
}g_stQueueForSMSDEL;

#endif

static DPTCH_CTL	g_stCtl;

static PSMSTransact_f	pfDefault; //for normal sms.
static REGISTER		*g_pRegLstHeader;

struct tagAsynPortSend {
	int bUsed;
	HWND hWnd;
	int msg;
	PORT_SEND PortSend;
	SMS_SEND SMSSend; 
	int offset;	
	int SumSegs;
	int iLastLen;
	int iMidLen;
	/* 返回值*/
	int SentSegs;
	int SentLen;

}gAsynPortSend;

struct tagAsynNormalSend{
	int bUsed;
	HWND hWnd;
	int msg;
	NORMAL_SEND NormalSend;
	SMS_SEND SMSSend;
	int offset;
	int SumSegs;
	int iLastLen;
	int iMidLen;
	int SentSegs;
	int SentLen;

}gAsynNormalSend;



static void DispatchNormalSMS(int type, void *pInfo);
#ifdef TI_MODULE
static void DispatchPortSMS(SMS_INFO *pstSMSInfo, BOOL bQueue);
#else
static void DispatchPortSMS(SMS_INFO *pstSMSInfo);
#endif

static int GetDptchType(SMS_INFO stSMSInfo);
static LRESULT MECbWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam); 
static BOOL IsMatching(char *srcstr, char *condition);

extern BOOL SMS_FillTrafficLog(char* pszNum,DRTTYPE direction,unsigned long SmsID,int nFrag,SMSSTATUS status);

/*********************************************************************************
 函数功能：完成短信分发模块的初始化工作，包括创建与ME模块通讯的窗口和数据的初始化等。
 输入参数：	无
 输出参数：	无
 返回值：ERR_SUCCESS
		 ERR_SYS

**************************************************************************************/

int SMSDISPATCH_Init(void)
{
	WNDCLASS wc;

	if(g_stCtl.bInited == TRUE)
		return ERR_SUCCESS;

	wc.style         = 0;
	wc.lpfnWndProc   = MECbWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "SMSDispatchClass";

	if ( !RegisterClass(&wc) )
		return ERR_SYS;
	
	memset((void *)&g_stCtl, 0, sizeof(DPTCH_CTL));
	
	g_stCtl.hwnd = CreateWindow("SMSDispatchClass", NULL, WS_POPUP, 
			0, 0, 0, 0, 
			NULL, NULL, NULL, NULL);
	if(g_stCtl.hwnd == NULL)
		return ERR_SYS;
	/*
	ME_URC_CMT 			短消息到来（标识短消息内容）
	ME_URC_CMTI 		短消息到来（标识存储区和索引）
	*/	
	g_stCtl.hMECMTI = ME_RegisterIndication(ME_URC_CMTI, IND_PRIORITY, g_stCtl.hwnd, WM_CMTI_COMING);
	
	if(g_stCtl.hMECMTI == -1)
		return ERR_SYS; 

	g_stCtl.hMECMT = ME_RegisterIndication(ME_URC_CMT, IND_PRIORITY, g_stCtl.hwnd, WM_CMT_COMING);

	if(g_stCtl.hMECMT == -1)
		return ERR_SYS; 
	/*
	ME_URC_CDS 			短消息状态报告到来（标识状态报告内容）
	ME_URC_CDSI 		短消息状态报告到来（标识存储区和索引）
	*/
	g_stCtl.hMECDS = ME_RegisterIndication(ME_URC_CDS, IND_PRIORITY, g_stCtl.hwnd, WM_CDS_COMING);

	if(g_stCtl.hMECDS == -1)
		return ERR_SYS; 

	g_stCtl.hMECDSI = ME_RegisterIndication(ME_URC_CDSI, IND_PRIORITY, g_stCtl.hwnd, WM_CDSI_COMING);

	if(g_stCtl.hMECDSI == -1)
		return ERR_SYS; 
	/*
		ME_URC_CBM 			小区广播消息到来（标识广播消息内容）
		ME_URC_CBMI 		小区广播消息到来（标识存储区和索引）
	*/	
	g_stCtl.hMECBM = ME_RegisterIndication(ME_URC_CBM, IND_PRIORITY, g_stCtl.hwnd, WM_CBM_COMING);

	if(g_stCtl.hMECBM == -1)
		return ERR_SYS; 	

	g_stCtl.hMECBMI = ME_RegisterIndication(ME_URC_CBMI, IND_PRIORITY, g_stCtl.hwnd, WM_CBMI_COMING);

	if(g_stCtl.hMECBMI == -1)
		return ERR_SYS; 	

	pfDefault = NULL;
	g_pRegLstHeader = NULL;

	memset((void *)g_stQueue.astSMSITEM, 0, QUEUE_LEN*sizeof(struct tagSMSITEM));
	g_stQueue.iHead = 0;
	g_stQueue.iTail = 0;
	g_stQueue.bFull = FALSE;

#ifdef TI_MODULE
	memset((void *)g_stQueueForSMSDEL.astInfo, 0, DEL_QUEUE_LEN*sizeof(struct INFO));
	g_stQueueForSMSDEL.bFull = FALSE;
	g_stQueueForSMSDEL.iHead = 0;
	g_stQueueForSMSDEL.iTail = 0;
#endif

	memset((void *)&gAsynPortSend, 0, sizeof(struct tagAsynPortSend));
	memset((void *)&gAsynNormalSend, 0, sizeof(struct tagAsynNormalSend));
	g_stCtl.bInited = TRUE;
	
	return ERR_SUCCESS;
}
/*********************************************************************************
 函数功能：短信分发模块的关闭接口。包括注销和系统资源的释放。
 输入参数：无
 输出参数：无
 返回值：ERR_SUCCESS
		 ERR_SYS

**************************************************************************************/

int SMSDISPATCH_Shutdown(void)
{
	BOOL bRet;
	int i;

	if(g_stCtl.bInited == FALSE)
		return ERR_SUCCESS;

	bRet = ME_UnRegisterIndication(g_stCtl.hMECDS);
	if(bRet == FALSE)
		return ERR_SYS;

	bRet = ME_UnRegisterIndication(g_stCtl.hMECDSI);
	if(bRet == FALSE)
		return ERR_SYS;	

	bRet = ME_UnRegisterIndication(g_stCtl.hMECMT);
	if(bRet == FALSE)
		return ERR_SYS;
	
	bRet = ME_UnRegisterIndication(g_stCtl.hMECMTI);
	if(bRet == FALSE)
		return ERR_SYS;

	bRet = ME_UnRegisterIndication(g_stCtl.hMECBM);
	if(bRet == FALSE)
		return ERR_SYS;
	
	bRet = ME_UnRegisterIndication(g_stCtl.hMECBMI);
	if(bRet == FALSE)
		return ERR_SYS;

	memset((void *)&g_stCtl, 0, sizeof(DPTCH_CTL));	

	for(i=1; i<=CTLBLK_CNT; i++)
	{
		while(g_stCtl.handles[i].pstDatagramLst != NULL)
		{
			DATAGRAM *pCurDatagram;

			pCurDatagram = g_stCtl.handles[i].pstDatagramLst;

			g_stCtl.handles[i].pstDatagramLst = g_stCtl.handles[i].pstDatagramLst->pNext;

			SMSDptchFree((void *)pCurDatagram);
		}
	}

	while(g_pRegLstHeader != NULL)
	{
		REGISTER *pTmp;

		pTmp = g_pRegLstHeader;

		g_pRegLstHeader = g_pRegLstHeader->pNext;
	
		SMSDptchFree((void *)pTmp);

	}

	g_stCtl.bInited = FALSE;

#ifdef SMSDISPATCH_DEBUG

	WAP_IsMemCleanup();

#endif
	
	return ERR_SUCCESS;
}


/*********************************************************************************
 函数功能：通过GSM SMS等方式发送数据和接收数据时，需要首先申请一个短信分发模块句柄，
			此函数就是用于申请短信分发模块的句柄。
 输入参数：	无
			
 输出参数：	无
 返回值：	成功返回短信分发模块句柄，如果没有可用的句柄返回0。

**************************************************************************************/

int SMSDISPATCH_Open(void)
{
	int iCurHnd;
	int iCnt = 0;

	iCurHnd = g_stCtl.prehnd;

	iCurHnd++;
	if( iCurHnd > CTLBLK_CNT )
		iCurHnd = 1;

	
	while( (iCnt < CTLBLK_CNT) && (g_stCtl.handles[iCurHnd].isused == 1) )
	{
		iCurHnd++;
		if(iCurHnd > CTLBLK_CNT)
			iCurHnd = 1;

		iCnt++;
	}

	if(iCnt < CTLBLK_CNT)
	{
		g_stCtl.prehnd = iCurHnd;
		g_stCtl.handles[iCurHnd].isused = 1;

		return iCurHnd;
	}

	return 0;
}

/*********************************************************************************
 函数功能：当关闭通过GSM SMS等方式发送数据和接收数据的应用时，需要调用此函数来关闭
			open时申请的短信分发模块句柄以释放资源。
 输入参数：
		handle：	要关闭的短信分发模块句柄

 输出参数：无
 返回值：ERR_SUCCESS
		 ERR_PARAM

**************************************************************************************/


int SMSDISPATCH_Close(int handle)
{
	if( (handle<1) || (handle>CTLBLK_CNT) || 
		(g_stCtl.handles[handle].isused != 1))
		return ERR_PARAM;

	/*这里需要考虑释放datagram list的内存吗？*/
	while(g_stCtl.handles[handle].pstDatagramLst != NULL)
	{
		DATAGRAM *pCurDatagram;

		pCurDatagram = g_stCtl.handles[handle].pstDatagramLst;

		g_stCtl.handles[handle].pstDatagramLst = g_stCtl.handles[handle].pstDatagramLst->pNext;

		SMSDptchFree((void *)pCurDatagram);
	}

	memset(&g_stCtl.handles[handle], 0, sizeof(DPTCH_HANDLE));

	return ERR_SUCCESS;
}
/*********************************************************************************
 函数功能：通过GSM SMS等方式发送数据和接收数据时，需要首先申请一个短信分发模块句柄，
			然后通过此函数设置该句柄,将该句柄与上层的一个端口进行绑定。
			一个短信分发模块句柄只能与一个上层端口绑定。也就是说，对应一个短信分发
			模块句柄只能调用一次SMSDISPATCH_Ioctl来进行设置和绑定工作。
 输入参数：
		handle：	短信分发模块句柄
		port：		该句柄绑定的端口	
		hwnd		该句柄绑定的端口接收到数据时，发送消息到这个窗口句柄	
		len：		该句柄绑定的端口接收到数据时，发送这个消息到窗口句柄
 输出参数：无
 返回值：ERR_SUCCESS
		 ERR_PARAM
		 ERR_CTLBLK

**************************************************************************************/

int SMSDISPATCH_Ioctl(int handle, unsigned short port, HWND hwnd, UINT msg)
{
	
	if((handle<1) || (handle>CTLBLK_CNT))
		return ERR_PARAM;

	if((g_stCtl.handles[handle].isused != 1) ||
		(g_stCtl.handles[handle].addr != 0)||
		(g_stCtl.handles[handle].hwnd != NULL)||
		(g_stCtl.handles[handle].msg != 0))
		return ERR_CTLBLK;

	g_stCtl.handles[handle].addr = port;
	g_stCtl.handles[handle].hwnd = hwnd;
	g_stCtl.handles[handle].msg = msg;

	return ERR_SUCCESS;
}
/************************************************************************/
/* 
函数功能：根据待发送数据长度得到需要分片数目。
输入参数：
	DataLen：待发送数据长度；
输出参数：
	pnSegs：返回需要的分片数
返回值：
	ERR_PARAM:输入参数有误；
	ERR_SUCCESS:函数执行成功。
                                                                     */
/************************************************************************/

int SMSDISPATCH_GetFragNum	(int type, int dcs, int DataLen, int *pnSegs)
{
	
	int nTmplen;

	if (type != DPTCHTYPE_NORMAL && type != DPTCHTYPE_PORT) 
		return ERR_PARAM;
	if(dcs !=  DCS_GSM&& dcs != DCS_GSM && dcs != DCS_UCS2)
		return ERR_PARAM;
	if (pnSegs == NULL || DataLen <= 0)
		return ERR_PARAM;

	if ( type == DPTCHTYPE_PORT) {

		if(dcs == DCS_GSM)
			nTmplen = 160 - (2+4) - ((2+4)/8 + 1 );
		else
			nTmplen = 140 - 2 - 4;

	}
	else if(type == DPTCHTYPE_NORMAL)
	{	
		if (dcs == DCS_GSM) 
			nTmplen = 160;
		else
			nTmplen = 140;
	}


	if(DataLen > nTmplen)
	{
		/* 需要分片 */
		nTmplen -= 6; /*6为UDHI+SARID+LEN等*/
		if(DataLen % nTmplen == 0)
			*pnSegs = DataLen/nTmplen;
		else
			*pnSegs = DataLen/nTmplen + 1;
	}
	else
		/* 不需要分片 */
		*pnSegs = 1;

	return ERR_SUCCESS;
}
/*********************************************************************************
 函数功能：通过GSM SMS等方式发送带有端口信息的数据。如果数据大于一个底层网络数据包的长度时，
			需要分片发送。
 输入参数：
		handle：	短信分发模块句柄
		destaddr：	数据的发送地址
		SendInfo：	将要发送的数据的信息，包括数据、数据长度和指定的编码方式。
		hWnd:		上层接收消息的窗口句柄；
		msg：		上层接收的消息。
	
 返回值：
	ERR_SUCCESS:函数执行成功；
	ERR_FAILURE:函数执行失败
	ERR_PARAM:输入参数有误

  函数说明：此函数为异步函数，上层应用通过hWnd窗口接收msg来得到结果。当接收到消息后，
			才能释放SendInfo的数据缓冲区。

**************************************************************************************/

int SMSDISPATCH_Send(int handle, HWND hWnd, int msg, 
					 Address_t destaddr, PORT_SEND SendInfo)
{
	int offset=0;
	int nTmplen;

	if ( gAsynPortSend.bUsed == TRUE )
		return ERR_FAILURE;

	/* 检查输入参数 */
	if( handle < 1 || handle > CTLBLK_CNT || g_stCtl.handles[handle].isused != 1 )
		return ERR_PARAM;
	if( strlen(destaddr.loweraddr) == 0 || strlen(destaddr.SCA) == 0 )
		return ERR_PARAM;	
	if( SendInfo.pData == NULL || SendInfo.DataLen <= 0 )
		return ERR_PARAM;
	if(SendInfo.CodeMode != DCS_GSM && SendInfo.CodeMode != DCS_8BIT && SendInfo.CodeMode != DCS_UCS2 )
		return ERR_PARAM;	
	
	gAsynPortSend.hWnd = hWnd;
	gAsynPortSend.msg = msg;
	memcpy((void *)&gAsynPortSend.PortSend, &SendInfo, sizeof(PORT_SEND));

	gAsynPortSend.SMSSend.dcs = SendInfo.CodeMode;
	strcpy(gAsynPortSend.SMSSend.SCA, destaddr.SCA);
	strcpy(gAsynPortSend.SMSSend.PhoneNum, destaddr.loweraddr);
	if (SendInfo.CodeMode == DCS_GSM) 
		nTmplen = 160 - ((1+2+4)*8/7);
	else
		nTmplen = 140 - (1+2+4);
	
	if( SendInfo.DataLen > nTmplen )
		/*需要分片发送,目前IE仅包括SAR和16bit PORT */
	{

		
		if ( SendInfo.CodeMode == DCS_GSM) {
			nTmplen = 160 - ((1+2+4+5)*8/7 + 1);
		}
		else
			nTmplen = 140 - (1+2+4+5);

		gAsynPortSend.iMidLen = gAsynPortSend.iLastLen = nTmplen;
		
		if(SendInfo.DataLen % nTmplen == 0)
		{
			gAsynPortSend.SumSegs = SendInfo.DataLen/nTmplen;
		}
		else
		{
			gAsynPortSend.SumSegs = SendInfo.DataLen/nTmplen+1;
			gAsynPortSend.iLastLen = SendInfo.DataLen - (gAsynPortSend.SumSegs-1)*nTmplen;
		}
		/*这里的分量在每一次的分片都一样*/
		gAsynPortSend.SMSSend.udhl = PORT_MAX_HEADER_LEN;

		gAsynPortSend.SMSSend.UDH[offset++] = IE_PORT_ID;
		gAsynPortSend.SMSSend.UDH[offset++] = IE_PORT_LEN;
		gAsynPortSend.SMSSend.UDH[offset++] = (destaddr.port >> 8) & 0xff;
		gAsynPortSend.SMSSend.UDH[offset++] = destaddr.port & 0xff;
		gAsynPortSend.SMSSend.UDH[offset++] = (g_stCtl.handles[handle].addr >> 8) & 0xff;
		gAsynPortSend.SMSSend.UDH[offset++] =  g_stCtl.handles[handle].addr & 0xff;
		gAsynPortSend.SMSSend.UDH[offset++] = IE_SAR_ID;
		gAsynPortSend.SMSSend.UDH[offset++] = IE_SAR_LEN;
		if(g_stCtl.nCurRefNum == 0xff)
			g_stCtl.nCurRefNum = 0;
		gAsynPortSend.SMSSend.UDH[offset++] = ++g_stCtl.nCurRefNum;
		gAsynPortSend.SMSSend.UDH[offset++] = gAsynPortSend.SumSegs;		
		
	}
	else 
	{
		/*待发送的数据仅不需要分片。*/
		gAsynPortSend.SMSSend.udhl = PORT_MIN_HEADER_LEN;
		//stSMSSend.UDH[offset++] = PORT_MIN_HEADER_LEN - UDHL_FIELD_LEN;
		gAsynPortSend.SMSSend.UDH[offset++] = IE_PORT_ID;
		gAsynPortSend.SMSSend.UDH[offset++] = IE_PORT_LEN;
		gAsynPortSend.SMSSend.UDH[offset++] = (destaddr.port >> 8) & 0xff;
		gAsynPortSend.SMSSend.UDH[offset++] = destaddr.port & 0xff;
		gAsynPortSend.SMSSend.UDH[offset++] = (g_stCtl.handles[handle].addr >> 8) & 0xff;
		gAsynPortSend.SMSSend.UDH[offset] =  g_stCtl.handles[handle].addr & 0xff;	
		gAsynPortSend.SumSegs = 1;
	}

	PostMessage(g_stCtl.hwnd, WM_ASYN_PORT_SEND, 0, 0);
	
	return ERR_SUCCESS;
}


/*
	当收到完整的datagram时，PostMessage()通知上层,参数如果消息参数为Address_t＋refnum，
	则可以保证读取的正是发送消息通知读取的那个datagram。
	这样，需要增加一个输入参数refnum。
*/
/*********************************************************************************
 函数功能：通过GSM SMS等方式接收数据。上层接到端口数据通知消息后，主动调用这个函数来
			读取从端口接收到的数据。此时，接收到的数据是已经完成重组后（如果需要）的完整
			数据包。
 输入参数：
		handle：短信分发模块句柄
		pRecvInfo－>nBufLen:接收数据的缓存区的大小

 输出参数：
		pRecvInfo－>pnDataLen：实际接收到的端口数据长度。如果为0，则下面两个值无意义。
		pRecvInfo－>pBuf：上层分配的用于接收端口数据的缓冲区，当函数返回时，该缓冲区存储
							接收到的端口数据；		
		pRecvInfo－>pnCodeMode：接收到的端口数据的编码方式
		psraddr：	接收到数据的来源。


 返回值：
	ERR_SUCCESS:表明函数执行完成。
**************************************************************************************/
int SMSDISPATCH_Recv(int handle, Address_t *psrcaddr, PORT_RECV *pRecvInfo)
{

	DATAGRAM *pstDatagram, *pre = NULL;
	unsigned char *pCur;
	int i;
	int unfill;
	int retlen = 0;

	if((g_stCtl.handles[handle].isused != 1) ||
		(pRecvInfo->pBuf == NULL) ||
		(psrcaddr == NULL))

		return ERR_PARAM;

	pCur = pRecvInfo->pBuf;
	unfill = pRecvInfo->nBufLen;
	
	pstDatagram = g_stCtl.handles[handle].pstDatagramLst;

	while(pstDatagram != NULL)
	{
		if(pstDatagram->maxfragment == pstDatagram->arrivedfragment)
		{
			/*这是一个完整的数据包。填充上层的接收数据缓冲区和数据长度值。*/
			for(i=0; i<pstDatagram->maxfragment; i++)
			{
				
				SMSDptchAssert((pstDatagram->parrSegs+i)->len != 0);

				if(unfill >= (pstDatagram->parrSegs+i)->len)
				{
					memcpy(pCur, (pstDatagram->parrSegs+i)->pdata, (pstDatagram->parrSegs+i)->len);
					pCur += (pstDatagram->parrSegs+i)->len;
					retlen += (pstDatagram->parrSegs+i)->len;
					unfill -= (pstDatagram->parrSegs+i)->len;

				}
				else
				{
					memcpy(pCur, (pstDatagram->parrSegs+i)->pdata, unfill);
					retlen += unfill;

					break;
				}
			
			}
			/*填充源端地址信息*/
			strcpy(psrcaddr->loweraddr, pstDatagram->srcaddr.loweraddr);
			psrcaddr->port = pstDatagram->srcaddr.port;
            strcpy(psrcaddr->SCA,pstDatagram->srcaddr.SCA);
			pRecvInfo->nDataLen = retlen;
			pRecvInfo->nCodeMode = pstDatagram->dcs;
			strcpy(pRecvInfo->Time, pstDatagram->SendTime);

			/*将数据包从datagram list中删除*/
			if(pre == NULL)
			{
				g_stCtl.handles[handle].pstDatagramLst = pstDatagram->pNext;			
			}
			else
			{
				pre->pNext = pstDatagram->pNext;
			}
			SMSDptchFree((void *)pstDatagram);

			return ERR_SUCCESS;
		}
		pre = pstDatagram;
		pstDatagram = pstDatagram->pNext;
	}

	pRecvInfo->nDataLen = 0;
	
	return ERR_SUCCESS;
}

					
/*********************************************************************************
 函数功能： 通过GSM SMS等方式接收文本短信。
			上层各种文本短信应用需要调用该接口注册短信分发模块的回调函数。

 输入参数：
		pCondition：电话号码的匹配条件
		pCbFun	：	回调函数的函数指针

 输出参数：无

 返回值：ERR_SUCCESS
 说明：  当pCondition ＝＝ NULL时，为普通文本短信。

**************************************************************************************/
int SMSDISPATCH_Register(char *pCondition, PSMSTransact_f pCbFun)
{
	REGISTER *pTmp;
	
	if((pCbFun == NULL) || (sizeof(pCondition) > ME_PHONENUM_LEN))
		return ERR_PARAM;

	if(pCondition == NULL)
	{
		if(pfDefault == NULL)
		{
			pfDefault = pCbFun;
			return ERR_SUCCESS;
		}
		else
		{
			return ERR_FAILURE;
		}
	}
	
	pTmp = g_pRegLstHeader;
	
	while(pTmp != NULL)
	{
		if(strcmp(pCondition, pTmp->condition) == 0)
			return ERR_FAILURE;

		pTmp = pTmp->pNext;

	}

	pTmp = SMSDptchMalloc(sizeof(REGISTER));
	memset(pTmp, 0, sizeof(REGISTER));
	
	strcpy(pTmp->condition, pCondition);
	pTmp->pf = pCbFun;
	pTmp->pNext = g_pRegLstHeader;
	
	g_pRegLstHeader = pTmp;
	
	return ERR_SUCCESS;
}
/*********************************************************************************
 函数功能： 通过GSM SMS等方式接收文本短信。
			上层各种文本短信应用需要调用该接口来注销注册过的信息。。

 输入参数：
		pCondition：电话号码的匹配条件

 输出参数：无

 返回值：ERR_SUCCESS
		 ERR_FAILURE

**************************************************************************************/
int SMSDISPATCH_UnReg(char *pCondition)
{
	REGISTER *pCur, *pPre;
	
	if(sizeof(pCondition) > ME_PHONENUM_LEN)
		return ERR_PARAM;
	
	pCur = g_pRegLstHeader;

	if(strcmp(g_pRegLstHeader->condition, pCondition) == 0)
	{
		g_pRegLstHeader = g_pRegLstHeader->pNext;
		SMSDptchFree(pCur);
		return ERR_SUCCESS;
	}

	pPre = g_pRegLstHeader;
	pCur = g_pRegLstHeader->pNext;

	while(pCur != NULL)
	{
		if(strcmp(pCondition, pCur->condition) == 0)
		{
			pPre->pNext = pCur->pNext;
			SMSDptchFree(pCur);
			return ERR_SUCCESS;
		}
		pPre = pCur;
		pCur = pCur->pNext;
	}

	return ERR_FAILURE;
}
/*********************************************************************************
 函数功能： 上层各种非端口的短信应用需要调用该接口来实现通过GSM SMS等方式的短信的发送。
			如果待发送的数据超过一个网络承载包的长度，则为长短信，需要分片发送。

 输入参数：
		hWnd: 功能执行完毕后通知的窗口句柄。
		Msg: 功能执行完毕后通知的窗口消息。
		pstSMSSend：待发送的文本短信的结构体，此结构定义为兼容前一版本ME中的定义，这样短信应用
					就不需要修改了。

 输出参数：无

 返回值：ERR_S

**************************************************************************************/
int SMSDISPATCH_SendSMS(HWND hWnd, int Msg, NORMAL_SEND stSMSSend)
{
	int offset = 0;
	int nTmplen;

	if ( gAsynNormalSend.bUsed == TRUE )
		return ERR_FAILURE;

	gAsynNormalSend.hWnd = hWnd;
	gAsynNormalSend.msg = Msg;

	gAsynNormalSend.SMSSend.dcs = stSMSSend.dcs;
	strcpy(gAsynNormalSend.SMSSend.SCA, stSMSSend.SCA);
	strcpy(gAsynNormalSend.SMSSend.PhoneNum, stSMSSend.PhoneNum);
	gAsynNormalSend.SMSSend.PID = stSMSSend.PID;
	gAsynNormalSend.SMSSend.ReplyPath = stSMSSend.ReplyPath;
	gAsynNormalSend.SMSSend.Report = stSMSSend.Report;
	gAsynNormalSend.SMSSend.VPvalue = stSMSSend.VPvalue;
	gAsynNormalSend.SMSSend.Type = stSMSSend.Type;

	memcpy((void *)&gAsynNormalSend.NormalSend, &stSMSSend, sizeof(NORMAL_SEND));

	if (stSMSSend.dcs == DCS_GSM) 
		nTmplen = 160;
	else
		nTmplen = 140;
	
	if(stSMSSend.DataLen > nTmplen)
	{
        /* 这里说明需要分片发送 */
        if (stSMSSend.dcs == DCS_GSM) 
            nTmplen = (140-6)*8/7;
        else
            nTmplen -= 6;/*6为UDHI+SARID+LEN等*/
		gAsynNormalSend.iMidLen = gAsynNormalSend.iLastLen = nTmplen;

		if(stSMSSend.DataLen % nTmplen == 0)
		{
			gAsynNormalSend.SumSegs = stSMSSend.DataLen/nTmplen;
		}
		else
		{
			gAsynNormalSend.SumSegs = stSMSSend.DataLen/nTmplen+1;
			gAsynNormalSend.iLastLen = stSMSSend.DataLen - (gAsynNormalSend.SumSegs-1)*nTmplen;
		}
		gAsynNormalSend.SMSSend.udhl = 5;
		
		gAsynNormalSend.SMSSend.UDH[offset++] = IE_SAR_ID;
		gAsynNormalSend.SMSSend.UDH[offset++] = IE_SAR_LEN;
		if(g_stCtl.nCurRefNum == 0xff)
			g_stCtl.nCurRefNum = 0;
		gAsynNormalSend.SMSSend.UDH[offset++] = ++g_stCtl.nCurRefNum;
		gAsynNormalSend.SMSSend.UDH[offset++] = gAsynNormalSend.SumSegs;
	}
	else{
		gAsynNormalSend.SumSegs = 1;
		gAsynNormalSend.SMSSend.udhl = 0;
	}

	PostMessage(g_stCtl.hwnd, WM_ASYN_NORMAL_SEND, 0, 0);
	return ERR_SUCCESS;
}

/***************************************
			内部函数实现部分
****************************************/

/*********************************************************************************
 函数功能：窗口函数。用来接收ME模块发送来的消息。
 输入参数：
		hWnd：		窗口句柄
		wMsgCmd：	消息类型	
		wParam：	消息参数
		lParam：	消息参数

**************************************************************************************/

static LRESULT MECbWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	static int devtype;
	static int iIndex;

	lResult = TRUE;

	switch(wMsgCmd)
	{
	case WM_CMTI_COMING:
	case WM_CDSI_COMING:
		{
			/* 短消息到来（标识存储区和索引）|| 短消息状态报告到来（标识存储区和索引）*/
			int iRet;
			ME_CMTI_ITEM item;

            printf("\r\n SMSDispatch : Receive CMTI or CDSI \r\n");

			memset( &item, 0, sizeof(ME_CMTI_ITEM) );

			ME_GetResult(&item, sizeof(ME_CMTI_ITEM));

            printf("\r\n SMSDispatch : CMTI or CDSI index = %d \r\n",item.Index);

//			devtype = item.DevType;
//			iIndex = item.Index;
			/*队列满，从SIM卡中直接删除，不通知上层。不过应该确保足够队列空间，
			以保证不会出现此种情况*/
			if(g_stQueue.bFull == TRUE)
			{
				ME_SMS_Delete(g_stCtl.hwnd, WM_SMSDELETED, item.DevType, item.Index);
				break;
			}
			/*进队列*/
			g_stQueue.astSMSITEM[g_stQueue.iTail].index = item.Index;
			g_stQueue.astSMSITEM[g_stQueue.iTail].devtype = item.DevType;
			
			g_stQueue.iTail++;
			
			if(g_stQueue.iTail == QUEUE_LEN)
				g_stQueue.iTail = 0;
			/*进队列可以导致队列满。如果队列空间足够，不应该出现这种情况。*/
			if(g_stQueue.iTail == g_stQueue.iHead) 
				g_stQueue.bFull = TRUE;

            printf("\r\n SMSDispatch : Read SMS index = %d \r\n",item.Index);
			iRet = ME_SMS_Read(g_stCtl.hwnd, WM_CMTICDSI_READ, TRUE, item.DevType, item.Index); 

			break;
		
		}
	case WM_CMTICDSI_READ:
		
		if(wParam == ME_RS_SUCCESS)
		{
			SMS_INFO stSMSInfo;
			int iRet, iSMSType;
			int iDevType;

			memset((void *)&stSMSInfo, 0, sizeof(SMS_INFO));
							
			iRet = ME_GetResult(&stSMSInfo, sizeof(SMS_INFO));

			/*出队列。
			队列中一定不为空。因为只有在进队列操作后，才会发送
			WM_SMS_READ_RESULT。因此，在收到此WM_SMS_READ_RESULT后，队列一定不为空。*/
			stSMSInfo.Index = g_stQueue.astSMSITEM[g_stQueue.iHead].index;
			iDevType = g_stQueue.astSMSITEM[g_stQueue.iHead].devtype;
			
			g_stQueue.iHead++;

			if(g_stQueue.iHead == QUEUE_LEN)
				g_stQueue.iHead = 0;

			/*出队列可以导致队列非满状态。当队列为空时，无需做特殊处理。*/
			if(g_stQueue.bFull == TRUE)
				g_stQueue.bFull = FALSE;
			
			iSMSType = GetDptchType(stSMSInfo);
	
			if(iSMSType == DPTCHTYPE_NORMAL)
			{
                printf("\r\n SMSDispatch :  Dispatch SMS index = %d \r\n",stSMSInfo.Index);
				if(stSMSInfo.Type == SMS_DELIVER)
					DispatchNormalSMS(NORMAL_SMS, &stSMSInfo);
				else if(stSMSInfo.Type == STATUS_REPORT)
					DispatchNormalSMS(STATUS_REPORT_SMS, &stSMSInfo);
			}

			else if(iSMSType == DPTCHTYPE_PORT)
			{					
#ifdef TI_MODULE
				DispatchPortSMS(&stSMSInfo, TRUE);
#else
				DispatchPortSMS(&stSMSInfo);
#endif
				ME_SMS_Acknowledge(g_stCtl.hwnd, WM_ACKNOWLEDGE);	
				ME_SMS_Delete(g_stCtl.hwnd, WM_SMSDELETED, iDevType, stSMSInfo.Index);
			}	
	
		}
		break;

	case WM_CMT_COMING:
	case WM_CDS_COMING:	
		{
			/* 短消息状态报告到来（标识状态报告内容）|| 短消息到来（标识短消息内容）*/
			SMS_INFO stSMSInfo;
			int iRet, iSMSType;

			memset((void *)&stSMSInfo, 0, sizeof(SMS_INFO));
							
			iRet = ME_GetResult(&stSMSInfo, sizeof(SMS_INFO));	

			stSMSInfo.Index = 0;

			//ME_SMS_Delete(g_stCtl.hwnd, WM_SMSDELETED, devtype, stSMSInfo.Index);
			iSMSType = GetDptchType(stSMSInfo);
			if(iSMSType == DPTCHTYPE_NORMAL)
			{
				if(stSMSInfo.Type == SMS_DELIVER)
					DispatchNormalSMS(NORMAL_SMS, &stSMSInfo);
				else if(stSMSInfo.Type == STATUS_REPORT)
					DispatchNormalSMS(STATUS_REPORT_SMS, &stSMSInfo);
			}

			else if( iSMSType == DPTCHTYPE_PORT )
			{					
#ifdef TI_MODULE
				DispatchPortSMS(&stSMSInfo, FALSE);
#else
				DispatchPortSMS(&stSMSInfo);
#endif
				ME_SMS_Acknowledge(g_stCtl.hwnd, WM_ACKNOWLEDGE);	
			}		
	
			break;
		}
	case WM_CBMI_COMING:
		{
			/* 小区广播消息到来（标识存储区和索引）*/
			int iRet;
			ME_CMTI_ITEM item;

			memset( &item, 0, sizeof(ME_CMTI_ITEM) );

			ME_GetResult(&item, sizeof(ME_CMTI_ITEM));

			devtype = item.DevType;
			iIndex = item.Index;

			iRet = ME_SMS_Read(g_stCtl.hwnd, WM_CBMI_READ, TRUE, item.DevType, item.Index);

			break;
		
		}
	case WM_CBMI_READ:	
		
		if(wParam == ME_RS_SUCCESS)
		{
			SMS_INFO stSMSInfo;
			int iRet;

			memset((void *)&stSMSInfo, 0, sizeof(SMS_INFO));
							
			iRet = ME_GetResult(&stSMSInfo, sizeof(SMS_INFO));

			stSMSInfo.Index = iIndex;

			//ME_SMS_Delete(g_stCtl.hwnd, WM_SMSDELETED, devtype, stSMSInfo.Index);

			if(stSMSInfo.Type == SMS_DELIVER)
			{
				DispatchNormalSMS(BC_SMS, &stSMSInfo);
			}
			/*
			else if(stSMSInfo.Type == DATA_SMS)
			{					
				DispatchPortSMS(&stSMSInfo);
				ME_SMS_Acknowledge(g_stCtl.hwnd, WM_ACKNOWLEDGE);	
				ME_SMS_Delete(g_stCtl.hwnd, WM_SMSDELETED, devtype, stSMSInfo.Index);
			}
			*/
		}
		
		break;

	case WM_CBM_COMING:
		{
			/* 小区广播消息到来（标识广播消息内容）*/
			CBS_INFO stCBSInfo;
			int iRet;

			memset((void *)&stCBSInfo, 0, sizeof(CBS_INFO));
			iRet = ME_GetResult(&stCBSInfo, sizeof(CBS_INFO));
			DispatchNormalSMS(BC_SMS, &stCBSInfo);

			break;
		}
	case WM_SMSDELETED:

#ifdef TI_MODULE
		/*这里是方法二*/
		/*如果g_stInfo结构中没有信息，则说明还不是完整的数据报。只有在收到完整的数据报时，
		才设置g_stInfo，并且需要在PostMessage后置空。
		if(g_stInfo.hwnd != NULL)
		{
		PostMessage(g_stInfo.hwnd, g_stInfo.msg, MAKEWPARAM(g_stInfo.lWparam, g_stInfo.hWparam), NULL);		
		memset((void *)&g_stInfo, 0, sizeof(struct INFO));
		}*/
		/*队列为空的情况，不需要处理。*/
		if ( (g_stQueueForSMSDEL.iHead == g_stQueueForSMSDEL.iTail) &&
			(g_stQueueForSMSDEL.bFull == FALSE)) {
			break;
		}
		
		PostMessage(g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iHead].hwnd,
			g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iHead].msg,
			MAKEWPARAM(g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iHead].lWparam,
			g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iHead].hWparam),
			NULL);
		
		g_stQueueForSMSDEL.iHead++;
		
		if(g_stQueueForSMSDEL.iHead == DEL_QUEUE_LEN)
			g_stQueueForSMSDEL.iHead = 0;
		
		/*出队列可以导致队列非满状态。当队列为空时，无需做特殊处理。*/
		if(g_stQueueForSMSDEL.bFull == TRUE)
			g_stQueueForSMSDEL.bFull = FALSE;			
		
#endif
		
		//After deleting ,do nothing.
		break;

	case WM_ACKNOWLEDGE:
		
		//可以收到OK或者ERROR的消息参数，但目前没有处理。
		break;
	case WM_PORTSMS_SEND:
		if (wParam == ME_RS_SUCCESS) {
			int iRet;
		
			gAsynPortSend.SentSegs++;
			gAsynPortSend.SentLen += gAsynPortSend.SMSSend.ConLen;
			gAsynPortSend.offset += gAsynPortSend.SMSSend.ConLen;

			if (gAsynPortSend.SentSegs == gAsynPortSend.SumSegs) {
				/* 全部发送成功 */
//				PostMessage(gAsynPortSend.hWnd, gAsynPortSend.msg, 
//					(WPARAM)ERR_SUCCESS, 
//					MAKELPARAM(gAsynPortSend.SentLen, gAsynPortSend.SentSegs));

				PostMessage(gAsynPortSend.hWnd, gAsynPortSend.msg,
                    MAKEWPARAM(gAsynNormalSend.SentSegs,wParam),lParam);	

				memset((void *)&gAsynPortSend, 0, sizeof(struct tagAsynPortSend));			
			}
			else if (gAsynPortSend.SentSegs == (gAsynPortSend.SumSegs - 1)) {
				/* 发送最后一片 */
				gAsynPortSend.SMSSend.ConLen = gAsynPortSend.iLastLen;
				gAsynPortSend.SMSSend.UDH[gAsynPortSend.SMSSend.udhl - 1] = gAsynPortSend.SentSegs + 1;
				memcpy(gAsynPortSend.SMSSend.Context, 
					gAsynPortSend.PortSend.pData + gAsynPortSend.offset,
					gAsynPortSend.iLastLen);
				iRet = ME_SMS_Send(g_stCtl.hwnd, WM_PORTSMS_SEND, &gAsynPortSend.SMSSend);
				if (iRet == -1) {
					PostMessage(gAsynPortSend.hWnd, gAsynPortSend.msg,
						(WPARAM)ERR_FAILURE, MAKELPARAM(gAsynPortSend.SentLen, gAsynPortSend.SentSegs));
					memset((void *)&gAsynPortSend, 0, sizeof(struct tagAsynPortSend));
				}
				
			}
			else{
				gAsynPortSend.SMSSend.ConLen = gAsynPortSend.iMidLen;
				gAsynPortSend.SMSSend.UDH[gAsynPortSend.SMSSend.udhl - 1] = gAsynPortSend.SentSegs + 1;
				memcpy(&gAsynPortSend.SMSSend.Context, 
					gAsynPortSend.PortSend.pData + gAsynPortSend.offset,
					gAsynPortSend.iMidLen);
				iRet = ME_SMS_Send(g_stCtl.hwnd, WM_PORTSMS_SEND, &gAsynPortSend.SMSSend);
				if (iRet == -1) {
					PostMessage(gAsynPortSend.hWnd, gAsynPortSend.msg,
						(WPARAM)ERR_FAILURE, MAKELPARAM(gAsynPortSend.SentLen, gAsynPortSend.SentSegs));
					memset((void *)&gAsynPortSend, 0, sizeof(struct tagAsynPortSend));
				}

			}
		}
		else {
			if (gAsynPortSend.SentLen == 0) {
				PostMessage(gAsynPortSend.hWnd, gAsynPortSend.msg, 
					(WPARAM)ERR_FAILURE, 
					MAKELPARAM(0,0));
			}
			else
//				PostMessage(gAsynPortSend.hWnd, gAsynPortSend.msg, 
//					(WPARAM)ERR_SUCCESS, 
//					MAKELPARAM(gAsynPortSend.SentLen, gAsynPortSend.SentSegs));
                PostMessage(gAsynPortSend.hWnd, gAsynPortSend.msg,
                MAKEWPARAM(gAsynNormalSend.SentSegs,wParam),lParam);
            
			memset((void *)&gAsynPortSend, 0, sizeof(struct tagAsynPortSend));
		}
		
		break;
	case WM_NORMALSMS_SEND:
		if (wParam == ME_RS_SUCCESS) {
			int iRet;
			
			gAsynNormalSend.SentSegs++;
			gAsynNormalSend.SentLen += gAsynNormalSend.SMSSend.ConLen;
			gAsynNormalSend.offset += gAsynNormalSend.SMSSend.ConLen;
			
			if (gAsynNormalSend.SentSegs == gAsynNormalSend.SumSegs) {
				/* 全部发送成功 */
//				PostMessage(gAsynNormalSend.hWnd, gAsynNormalSend.msg, 
//					(WPARAM)ERR_SUCCESS, 
//					MAKELPARAM(gAsynNormalSend.SentLen, gAsynNormalSend.SentSegs));
				PostMessage(gAsynNormalSend.hWnd, gAsynNormalSend.msg,
                    MAKEWPARAM(gAsynNormalSend.SentSegs,wParam),lParam);
				memset((void *)&gAsynNormalSend, 0, sizeof(struct tagAsynNormalSend));
				
			}
			else if (gAsynNormalSend.SentSegs == (gAsynNormalSend.SumSegs - 1)) {
				/* 发送最后一片 */
				gAsynNormalSend.SMSSend.ConLen = gAsynNormalSend.iLastLen;
				gAsynNormalSend.SMSSend.UDH[gAsynNormalSend.SMSSend.udhl - 1] = gAsynNormalSend.SentSegs + 1;
				memcpy(gAsynNormalSend.SMSSend.Context, 
					gAsynNormalSend.NormalSend.pData + gAsynNormalSend.offset,
					gAsynNormalSend.iLastLen);
				iRet = ME_SMS_Send(g_stCtl.hwnd, WM_NORMALSMS_SEND, &gAsynNormalSend.SMSSend);
				if (iRet == -1) {
//					PostMessage(gAsynNormalSend.hWnd, gAsynNormalSend.msg,
//						(WPARAM)ERR_SUCCESS, MAKELPARAM(gAsynNormalSend.SentLen, gAsynNormalSend.SentSegs));
                    PostMessage(gAsynNormalSend.hWnd, gAsynNormalSend.msg,
                        MAKEWPARAM(gAsynNormalSend.SentSegs,wParam),lParam);
					memset((void *)&gAsynNormalSend, 0, sizeof(struct tagAsynNormalSend));
				}
				
			}
			else{
				gAsynNormalSend.SMSSend.ConLen = gAsynNormalSend.iMidLen;				
				gAsynNormalSend.SMSSend.UDH[gAsynNormalSend.SMSSend.udhl - 1] = gAsynNormalSend.SentSegs + 1;
				memcpy(&gAsynNormalSend.SMSSend.Context, 
					gAsynNormalSend.NormalSend.pData + gAsynNormalSend.offset,
					gAsynNormalSend.iMidLen);
				iRet = ME_SMS_Send(g_stCtl.hwnd, WM_NORMALSMS_SEND, &gAsynNormalSend.SMSSend);
				if (iRet == -1) {
//					PostMessage(gAsynNormalSend.hWnd, gAsynNormalSend.msg,
//						(WPARAM)ERR_SUCCESS, MAKELPARAM(gAsynNormalSend.SentLen, gAsynNormalSend.SentSegs));
                    PostMessage(gAsynNormalSend.hWnd, gAsynNormalSend.msg,
                        MAKEWPARAM(gAsynNormalSend.SentSegs,wParam),lParam);
					memset((void *)&gAsynNormalSend, 0, sizeof(struct tagAsynNormalSend));
				}
				
			}
		}
		else {
			if (gAsynNormalSend.SentLen == 0) {
				PostMessage(gAsynNormalSend.hWnd, gAsynNormalSend.msg, 
					(WPARAM)ERR_FAILURE, 
					MAKELPARAM(0,0));
			}
			else
//				PostMessage(gAsynNormalSend.hWnd, gAsynNormalSend.msg, 
//				(WPARAM)ERR_SUCCESS, 
//				MAKELPARAM(gAsynNormalSend.SentLen, gAsynNormalSend.SentSegs));
				PostMessage(gAsynNormalSend.hWnd, gAsynNormalSend.msg,
                MAKEWPARAM(gAsynNormalSend.SentSegs,wParam),lParam);
			memset((void *)&gAsynNormalSend, 0, sizeof(struct tagAsynNormalSend));
		}
		
		break;
	case WM_ASYN_PORT_SEND:
		{
			int	iRet;

			if (gAsynPortSend.SMSSend.udhl == PORT_MIN_HEADER_LEN) {
				/* 不需要分片 */
				gAsynPortSend.SMSSend.ConLen = gAsynPortSend.PortSend.DataLen;
				memcpy(gAsynPortSend.SMSSend.Context, gAsynPortSend.PortSend.pData, gAsynPortSend.PortSend.DataLen);
				
				iRet = ME_SMS_Send(g_stCtl.hwnd, WM_PORTSMS_SEND, &gAsynPortSend.SMSSend);
				if(iRet == -1){
					PostMessage(gAsynPortSend.hWnd, gAsynPortSend.msg, (WPARAM)ERR_FAILURE, MAKELPARAM(0, 0));
					memset((void *)&gAsynPortSend, 0, sizeof(struct tagAsynPortSend));					
				}
			}
			else{
					
				gAsynPortSend.SMSSend.ConLen = gAsynPortSend.iMidLen;				
				gAsynPortSend.SMSSend.UDH[gAsynPortSend.SMSSend.udhl - 1] = gAsynPortSend.SentSegs + 1;
				memcpy(gAsynPortSend.SMSSend.Context, 
					gAsynPortSend.PortSend.pData + gAsynPortSend.offset, 
					gAsynPortSend.iMidLen);
				
				iRet = ME_SMS_Send(g_stCtl.hwnd, WM_PORTSMS_SEND, &gAsynPortSend.SMSSend);
				if(iRet == -1){
					PostMessage(gAsynPortSend.hWnd, gAsynPortSend.msg, (WPARAM)ERR_FAILURE, MAKELPARAM(0, 0));					
					memset((void *)&gAsynPortSend, 0, sizeof(struct tagAsynPortSend));
				}					

			}

			break;
		}
	case WM_ASYN_NORMAL_SEND:
		{
			int	iRet;
			
			if (gAsynNormalSend.SumSegs == 1) {
				/* 不需要分片 */
				gAsynNormalSend.SMSSend.ConLen = gAsynNormalSend.NormalSend.DataLen;
				memcpy(gAsynNormalSend.SMSSend.Context, gAsynNormalSend.NormalSend.pData, gAsynNormalSend.NormalSend.DataLen);
				
				iRet = ME_SMS_Send(g_stCtl.hwnd, WM_NORMALSMS_SEND, &gAsynNormalSend.SMSSend);
				if(iRet == -1){
					PostMessage(gAsynNormalSend.hWnd, gAsynNormalSend.msg, (WPARAM)ERR_FAILURE, MAKELPARAM(0, 0));
					memset((void *)&gAsynNormalSend, 0, sizeof(struct tagAsynNormalSend));					
				}
			}
			else{
                printf("\r\n gAsynNormalSend.SentSegs = %d \r\n",gAsynNormalSend.SentSegs);
				
				gAsynNormalSend.SMSSend.ConLen = gAsynNormalSend.iMidLen;				
				gAsynNormalSend.SMSSend.UDH[gAsynNormalSend.SMSSend.udhl - 1] = gAsynNormalSend.SentSegs + 1;
				memcpy(gAsynNormalSend.SMSSend.Context, 
					gAsynNormalSend.NormalSend.pData + gAsynNormalSend.offset, 
					gAsynNormalSend.iMidLen);
				
				iRet = ME_SMS_Send(g_stCtl.hwnd, WM_NORMALSMS_SEND, &gAsynNormalSend.SMSSend);
				if(iRet == -1){
					PostMessage(gAsynNormalSend.hWnd, gAsynNormalSend.msg, (WPARAM)ERR_FAILURE, MAKELPARAM(0, 0));					
					memset((void *)&gAsynNormalSend, 0, sizeof(struct tagAsynNormalSend));
				}					
				
			}
			
			break;
		}
	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;
}

/*********************************************************************************
 函数功能：当短信分发模块接收到文本短信消息到的消息时，调用这个函数来完成文本短信的分发。
 输入参数：
		pstSMSInfo：通过ME模块得到的数据短信信息结构。

**************************************************************************************/

static void DispatchNormalSMS(int type, void *pInfo)
{
	REGISTER *pCur = g_pRegLstHeader;
	PSMSTransact_f pFun = NULL;
	
	if((type == NORMAL_SMS) || (type == STATUS_REPORT_SMS))
	{		
		while(pCur != NULL)
		{
			if(IsMatching(((SMS_INFO *)pInfo)->SenderNum, pCur->condition) == TRUE)
			{
				pFun = pCur->pf;
				break;
			}
			else
				pCur = pCur->pNext;
		}
		if(pFun == NULL)
			pFun = pfDefault;

        if(pFun == NULL)
            return;

		pFun(type, pInfo);
	}
	else if(type == BC_SMS)
	{
        if(pfDefault == NULL)
            return;
        
		pfDefault(BC_SMS, (BC_INFO *)pInfo);
	}

	return;
}

/*
 目前仅实现包含条件子串。
*/
static BOOL IsMatching(char *srcstr, char *condition)
{
	return (strstr(srcstr, condition) == srcstr)?TRUE:FALSE;
}

/*********************************************************************************
函数功能：当短信分发模块接收到端口短信时，调用这个函数来完成端口短信的重组和分发。
输入参数：
	pstSMSInfo：通过ME模块得到的短信信息结构。
输出参数：
	无
返回值：
	无
函数说明：
	为了提供JAVA应用编码方式和接收时间等信息，在DATAGRAM结构中增加了dcs和Time分量。dcs
	取值为第一个分片中的值，time取值为最后一个分片的值。
问题：如果一个分片和后面分片的dcs不同，应该丢弃整个datagram？

**************************************************************************************/
#ifdef TI_MODULE
static void DispatchPortSMS(SMS_INFO *pstSMSInfo, BOOL bQueue)
#else
static void DispatchPortSMS(SMS_INFO *pstSMSInfo)
#endif
{
	int i;
	unsigned short destport = 0xffff,srcport = 0xffff;
	DATAGRAM *pDatagram;
	unsigned char maxfragment;
	unsigned char sequence;
	unsigned short refnum;
	BOOL sarflg = FALSE;

	for(i=0; i<pstSMSInfo->udhl; )
	{
		switch(pstSMSInfo->UDH[i]) 
		{
		case IE_PORT_ID:
			
			if((i == pstSMSInfo->udhl - 1) || ((i + 1 + pstSMSInfo->UDH[i+1]) > pstSMSInfo->udhl - 1))
				/*没有了长度域或者域值长度不正确，返回。*/
				return;
			
			if(pstSMSInfo->UDH[i+1] == IE_PORT_LEN)
			{
				destport = MAKE_PORT_ADDR(pstSMSInfo->UDH[i+2], pstSMSInfo->UDH[i+3]);
				srcport = MAKE_PORT_ADDR(pstSMSInfo->UDH[i+4], pstSMSInfo->UDH[i+5]);
				
				i += IE_PORT_LEN + IE_FIELD_LEN + LIE_FIELD_LEN; /*下一个UHD IE标志的下标值*/
			}
			else /*如果地址的长度不为4，错误返回。*/
			{
				return;
			}
			break;
		case IE_SAR_ID:

			if((i == pstSMSInfo->udhl - 1) || ((i + 1 + pstSMSInfo->UDH[i+1]) > pstSMSInfo->udhl - 1))
				return;
			
			if(pstSMSInfo->UDH[i+1] == IE_SAR_LEN)
			{
				refnum = (unsigned short)pstSMSInfo->UDH[i+2];
				maxfragment = pstSMSInfo->UDH[i+3];
				sequence = pstSMSInfo->UDH[i+4];
				if( (sequence == 0) || (maxfragment == 0) )
					return;
				
				i += 1 + IE_SAR_LEN + 1; /*下一个UHD IE标志的下标值*/
				sarflg = (maxfragment <= 1) ? FALSE : TRUE;
			}
			else /*如果地址的长度不为4，错误返回。*/
				
			{
				return;
			}

			break;
		case IE_SAR_ID8:
			if((i == pstSMSInfo->udhl - 1) || ((i + 1 + pstSMSInfo->UDH[i+1]) > pstSMSInfo->udhl - 1))
				return;
			
			if(pstSMSInfo->UDH[i+1] == IE_SAR_LEN8)
			{
				refnum = (unsigned short)(((pstSMSInfo->UDH[i+3]) << 8) | (unsigned char)(pstSMSInfo->UDH[i+2]));
				maxfragment = pstSMSInfo->UDH[i+4];
				sequence = pstSMSInfo->UDH[i+5];
				if ((sequence == 0) || (maxfragment == 0) )
					return;
				
				i += 1 + IE_SAR_LEN8 + 1; /*下一个UHD IE标志的下标值*/
				sarflg = (maxfragment <= 1) ? FALSE : TRUE;
			}
			else /*如果地址的长度不为4，错误返回。*/
				
			{
				return;
			}
			break;
		default:
			
			if((i == pstSMSInfo->udhl - 1) || ((i + 1 + pstSMSInfo->UDH[i+1]) > pstSMSInfo->udhl - 1))
				return;
			
			i += pstSMSInfo->UDH[i+1] + IE_FIELD_LEN + LIE_FIELD_LEN; /*下一个UHD IE标志的下标值*/
		}

	}

	if((destport == 0xffff) || (srcport == 0xffff))
		/*如果没有目的地址域或源地址域的值，返回。*/
		return;
		
	for(i=1; i<=CTLBLK_CNT; i++)
	{
		if(g_stCtl.handles[i].addr == destport)
		{
			/*Find out control block.*/
			DATAGRAM *pCurDatagram;

			/*没有SAR,只有一片数据，同时一定不存在datagram结点。*/
			if(sarflg == FALSE)		
			{
				pDatagram = (DATAGRAM *)SMSDptchMalloc(sizeof(DATAGRAM)+sizeof(SEGMENT));
				if(pDatagram == NULL)
					return;
				memset(pDatagram, 0, sizeof(DATAGRAM)+sizeof(SEGMENT));
				
				pDatagram->maxfragment = 1;
				pDatagram->arrivedfragment = 1;
				strcpy(pDatagram->srcaddr.loweraddr, pstSMSInfo->SenderNum);
				pDatagram->srcaddr.loweraddr[sizeof(pstSMSInfo->SenderNum)] = '\0';
				pDatagram->srcaddr.port = srcport;
				pDatagram->dcs = pstSMSInfo->dcs;
				strcpy(pDatagram->SendTime, pstSMSInfo->SendTime);
				pDatagram->pNext = NULL;
				
				pDatagram->parrSegs = (SEGMENT *)(pDatagram + 1);		
				pDatagram->parrSegs->len = pstSMSInfo->ConLen;
				memcpy(pDatagram->parrSegs->pdata, pstSMSInfo->Context, pDatagram->parrSegs->len);

				pDatagram->pNext = g_stCtl.handles[i].pstDatagramLst; 
				g_stCtl.handles[i].pstDatagramLst = pDatagram;
                
                SMS_FillTrafficLog(pstSMSInfo->SenderNum,DRT_RECIEVE,0,1,SMS_RECEIVE);
#ifdef TI_MODULE
				
				/*这里是方法1：
				PostMessage(g_stCtl.CtlBlks[i].hwnd, g_stCtl.CtlBlks[i].msg, MAKEWPARAM((pstSMSInfo->ConLen - NO_SAR_HEADER_LEN), i), NULL);
				这里是方法2：
				g_stInfo.hwnd = g_stCtl.CtlBlks[i].hwnd;
				g_stInfo.msg = g_stCtl.CtlBlks[i].msg;
				g_stInfo.lWparam = pstSMSInfo->ConLen - NO_SAR_HEADER_LEN;
				g_stInfo.hWparam = i;*/
				
				if(bQueue == TRUE)
				{			
				/*队列满，不通知WDP。不过应该确保足够队列空间，
					以保证不会出现此种情况*/
					if(g_stQueueForSMSDEL.bFull == TRUE)
					{
						return;
					}
					/*进队列*/
					
					g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].hwnd = g_stCtl.handles[i].hwnd;
					g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].msg = g_stCtl.handles[i].msg;
					g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].lWparam = pstSMSInfo->ConLen;
					g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].hWparam = i;
					
					g_stQueueForSMSDEL.iTail++;
					
					if(g_stQueueForSMSDEL.iTail == DEL_QUEUE_LEN)
						g_stQueueForSMSDEL.iTail = 0;
					/*进队列可以导致队列满。如果队列空间足够，不应该出现这种情况。*/
					if(g_stQueueForSMSDEL.iTail == g_stQueueForSMSDEL.iHead) 
						g_stQueueForSMSDEL.bFull = TRUE;
					
				}
				else
				{
					PostMessage(g_stCtl.handles[i].hwnd, g_stCtl.handles[i].msg, MAKEWPARAM((pstSMSInfo->ConLen), i), NULL);
				}
#else

				PostMessage(g_stCtl.handles[i].hwnd, g_stCtl.handles[i].msg, MAKEWPARAM(pstSMSInfo->ConLen, i), 0);
#endif

				return;
				
			}

			pCurDatagram = g_stCtl.handles[i].pstDatagramLst;

			/*有多片，但己经存在datagram结点*/
			while(pCurDatagram != NULL)
			{
				if((strcmp(pCurDatagram->srcaddr.loweraddr, pstSMSInfo->SenderNum) == 0) &&
					(pCurDatagram->srcaddr.port == srcport) &&
					(pCurDatagram->refnum == refnum))
				{
					/*当datagram结点已经存在时，表明segment的内存已经分配。并且只有地址
					和reference number全部都相同时，才说明找到正确的datagram位置。*/
                    SEGMENT* parrSegsTemp = NULL;

                    parrSegsTemp = pCurDatagram->parrSegs;

                    parrSegsTemp += (sequence-1);

					if(parrSegsTemp->len == 0)
						/*等于0，表明没有收到相应的片，接收。否则该片重复收到，丢弃。*/
					{
						/* 如果编码不一致，则需要丢弃整个Datagram吗？ */
						//if(pCurDatagram->dcs != pstSMSInfo->dcs)

						(pCurDatagram->parrSegs+sequence-1)->len = pstSMSInfo->ConLen;
						memcpy((pCurDatagram->parrSegs+sequence-1)->pdata, 
								pstSMSInfo->Context, 
								(pCurDatagram->parrSegs+sequence-1)->len);
						
						pCurDatagram->arrivedfragment += 1;

						if(pCurDatagram->arrivedfragment == pCurDatagram->maxfragment)
						{
						
							int len = 0;
							int j;

							for(j=0; j<sequence; j++)
							{
								len += (pCurDatagram->parrSegs+j)->len;
							}	
							
							strcpy(pCurDatagram->SendTime, pstSMSInfo->SendTime);

                            SMS_FillTrafficLog(pstSMSInfo->SenderNum,DRT_RECIEVE,0,pCurDatagram->maxfragment,SMS_RECEIVE);
#ifdef TI_MODULE
							/*这里是方法一：
							*PostMessage(g_stCtl.CtlBlks[i].hwnd, g_stCtl.CtlBlks[i].msg, MAKEWPARAM(len, i), NULL);	
							*/
							/*这里是方法二：
							g_stInfo.hwnd = g_stCtl.CtlBlks[i].hwnd;
							g_stInfo.msg = g_stCtl.CtlBlks[i].msg;
							g_stInfo.lWparam = len;
							g_stInfo.hWparam = i;*/
							
							if(bQueue == TRUE)
							{
							/*队列满，不通知WDP。不过应该确保足够队列空间，
								以保证不会出现此种情况*/
								if(g_stQueueForSMSDEL.bFull == TRUE)
								{
									return;
								}
								/*进队列*/
								g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].hwnd = g_stCtl.handles[i].hwnd;
								g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].msg = g_stCtl.handles[i].msg;
								g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].lWparam = len;
								g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].hWparam = i;
								
								g_stQueueForSMSDEL.iTail++;
								
								if(g_stQueueForSMSDEL.iTail == DEL_QUEUE_LEN)
									g_stQueueForSMSDEL.iTail = 0;
								/*进队列可以导致队列满。如果队列空间足够，不应该出现这种情况。*/
								if(g_stQueueForSMSDEL.iTail == g_stQueueForSMSDEL.iHead) 
									g_stQueueForSMSDEL.bFull = TRUE;
							}
							else
							{
								PostMessage(g_stCtl.handles[i].hwnd, g_stCtl.handles[i].msg, MAKEWPARAM(len, i), NULL);	
							}
#else
							
							PostMessage(g_stCtl.handles[i].hwnd, g_stCtl.handles[i].msg, MAKEWPARAM(len, i), 0);
#endif
						}
						
						return;
					
					}
					else
						/* 丢弃返回 */
						return;
				}
				else
				{
					pCurDatagram = pCurDatagram->pNext;
					
				}
		
			}//end while(pCurDatagram != NULL)
				
			//有多片，但不存在datagram结点，即第一片分片。
			pDatagram = (DATAGRAM *)SMSDptchMalloc(sizeof(DATAGRAM)+maxfragment*sizeof(SEGMENT));
			if(pDatagram == NULL)
				return;
			memset(pDatagram, 0, sizeof(DATAGRAM)+maxfragment*sizeof(SEGMENT));
	
			pDatagram->dcs = pstSMSInfo->dcs;
			pDatagram->refnum = refnum;			
			pDatagram->maxfragment = maxfragment;
			pDatagram->arrivedfragment = 1;
			strcpy(pDatagram->srcaddr.loweraddr, pstSMSInfo->SenderNum);
			pDatagram->srcaddr.loweraddr[sizeof(pstSMSInfo->SenderNum)] = '\0';
			pDatagram->srcaddr.port = srcport;

			pDatagram->parrSegs = (SEGMENT *)(pDatagram + 1);	
			
			pDatagram->parrSegs->len = pstSMSInfo->ConLen;
			memcpy(pDatagram->parrSegs->pdata, pstSMSInfo->Context, pDatagram->parrSegs->len);
			
			pDatagram->pNext = g_stCtl.handles[i].pstDatagramLst; 
			g_stCtl.handles[i].pstDatagramLst = pDatagram;

			return;	
		}
	}
	return;
}
/************************************************************************/
/* 
函数功能：
	通过分析SMS_INFO结构，得到短信类型，短信分发模块根据此类型来进行分片重组和短信分发。
输入参数：
	stSMSInfo：短信分发模块从ME得到的短信结构信息。
输出参数：
	无
返回值：
	目前只有两种类型：
	DPTCHTYPE_NORMAL：表明分发给普通短信应用。
	SMMTYPE_PORT：表明分发给带有端口信息的短信应用。
*/
/************************************************************************/
static int GetDptchType(SMS_INFO stSMSInfo)
{
	int iCurPos = 0;	

	if( stSMSInfo.udhl == 0 )
		return DPTCHTYPE_NORMAL;
	
	while( iCurPos != stSMSInfo.udhl )
	{
		if(stSMSInfo.UDH[iCurPos++] != IE_PORT_ID)
		{
			iCurPos += stSMSInfo.UDH[iCurPos] + LIE_FIELD_LEN;
		}
		else
			return DPTCHTYPE_PORT;
	}
	return DPTCHTYPE_NORMAL;
}
