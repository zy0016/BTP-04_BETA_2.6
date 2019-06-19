/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : smsdptch.c
 *
 * Purpose  : ���ŷַ�ģ���ⲿ�ӿں������ڲ�������ʵ��.
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

/*���¶������ݽṹ���������index��ƥ�����⡣*/
#define QUEUE_LEN	10

/*
*	��Ҫ���б������Ϣ��
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
	SMSITEM astSMSITEM[QUEUE_LEN];/*���ȿ��Կ���ΪSIM���洢���ŵĸ���*/

}g_stQueue;

#ifdef TI_MODULE
/*�ýṹ���ڽ��TIģ���в��ź�MEɾ�������⡣*/

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
	/* ����ֵ*/
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
 �������ܣ���ɶ��ŷַ�ģ��ĳ�ʼ������������������MEģ��ͨѶ�Ĵ��ں����ݵĳ�ʼ���ȡ�
 ���������	��
 ���������	��
 ����ֵ��ERR_SUCCESS
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
	ME_URC_CMT 			����Ϣ��������ʶ����Ϣ���ݣ�
	ME_URC_CMTI 		����Ϣ��������ʶ�洢����������
	*/	
	g_stCtl.hMECMTI = ME_RegisterIndication(ME_URC_CMTI, IND_PRIORITY, g_stCtl.hwnd, WM_CMTI_COMING);
	
	if(g_stCtl.hMECMTI == -1)
		return ERR_SYS; 

	g_stCtl.hMECMT = ME_RegisterIndication(ME_URC_CMT, IND_PRIORITY, g_stCtl.hwnd, WM_CMT_COMING);

	if(g_stCtl.hMECMT == -1)
		return ERR_SYS; 
	/*
	ME_URC_CDS 			����Ϣ״̬���浽������ʶ״̬�������ݣ�
	ME_URC_CDSI 		����Ϣ״̬���浽������ʶ�洢����������
	*/
	g_stCtl.hMECDS = ME_RegisterIndication(ME_URC_CDS, IND_PRIORITY, g_stCtl.hwnd, WM_CDS_COMING);

	if(g_stCtl.hMECDS == -1)
		return ERR_SYS; 

	g_stCtl.hMECDSI = ME_RegisterIndication(ME_URC_CDSI, IND_PRIORITY, g_stCtl.hwnd, WM_CDSI_COMING);

	if(g_stCtl.hMECDSI == -1)
		return ERR_SYS; 
	/*
		ME_URC_CBM 			С���㲥��Ϣ��������ʶ�㲥��Ϣ���ݣ�
		ME_URC_CBMI 		С���㲥��Ϣ��������ʶ�洢����������
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
 �������ܣ����ŷַ�ģ��Ĺرսӿڡ�����ע����ϵͳ��Դ���ͷš�
 �����������
 �����������
 ����ֵ��ERR_SUCCESS
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
 �������ܣ�ͨ��GSM SMS�ȷ�ʽ�������ݺͽ�������ʱ����Ҫ��������һ�����ŷַ�ģ������
			�˺�����������������ŷַ�ģ��ľ����
 ���������	��
			
 ���������	��
 ����ֵ��	�ɹ����ض��ŷַ�ģ���������û�п��õľ������0��

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
 �������ܣ����ر�ͨ��GSM SMS�ȷ�ʽ�������ݺͽ������ݵ�Ӧ��ʱ����Ҫ���ô˺������ر�
			openʱ����Ķ��ŷַ�ģ�������ͷ���Դ��
 ���������
		handle��	Ҫ�رյĶ��ŷַ�ģ����

 �����������
 ����ֵ��ERR_SUCCESS
		 ERR_PARAM

**************************************************************************************/


int SMSDISPATCH_Close(int handle)
{
	if( (handle<1) || (handle>CTLBLK_CNT) || 
		(g_stCtl.handles[handle].isused != 1))
		return ERR_PARAM;

	/*������Ҫ�����ͷ�datagram list���ڴ���*/
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
 �������ܣ�ͨ��GSM SMS�ȷ�ʽ�������ݺͽ�������ʱ����Ҫ��������һ�����ŷַ�ģ������
			Ȼ��ͨ���˺������øþ��,���þ�����ϲ��һ���˿ڽ��а󶨡�
			һ�����ŷַ�ģ����ֻ����һ���ϲ�˿ڰ󶨡�Ҳ����˵����Ӧһ�����ŷַ�
			ģ����ֻ�ܵ���һ��SMSDISPATCH_Ioctl���������úͰ󶨹�����
 ���������
		handle��	���ŷַ�ģ����
		port��		�þ���󶨵Ķ˿�	
		hwnd		�þ���󶨵Ķ˿ڽ��յ�����ʱ��������Ϣ��������ھ��	
		len��		�þ���󶨵Ķ˿ڽ��յ�����ʱ�����������Ϣ�����ھ��
 �����������
 ����ֵ��ERR_SUCCESS
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
�������ܣ����ݴ��������ݳ��ȵõ���Ҫ��Ƭ��Ŀ��
���������
	DataLen�����������ݳ��ȣ�
���������
	pnSegs��������Ҫ�ķ�Ƭ��
����ֵ��
	ERR_PARAM:�����������
	ERR_SUCCESS:����ִ�гɹ���
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
		/* ��Ҫ��Ƭ */
		nTmplen -= 6; /*6ΪUDHI+SARID+LEN��*/
		if(DataLen % nTmplen == 0)
			*pnSegs = DataLen/nTmplen;
		else
			*pnSegs = DataLen/nTmplen + 1;
	}
	else
		/* ����Ҫ��Ƭ */
		*pnSegs = 1;

	return ERR_SUCCESS;
}
/*********************************************************************************
 �������ܣ�ͨ��GSM SMS�ȷ�ʽ���ʹ��ж˿���Ϣ�����ݡ�������ݴ���һ���ײ��������ݰ��ĳ���ʱ��
			��Ҫ��Ƭ���͡�
 ���������
		handle��	���ŷַ�ģ����
		destaddr��	���ݵķ��͵�ַ
		SendInfo��	��Ҫ���͵����ݵ���Ϣ���������ݡ����ݳ��Ⱥ�ָ���ı��뷽ʽ��
		hWnd:		�ϲ������Ϣ�Ĵ��ھ����
		msg��		�ϲ���յ���Ϣ��
	
 ����ֵ��
	ERR_SUCCESS:����ִ�гɹ���
	ERR_FAILURE:����ִ��ʧ��
	ERR_PARAM:�����������

  ����˵�����˺���Ϊ�첽�������ϲ�Ӧ��ͨ��hWnd���ڽ���msg���õ�����������յ���Ϣ��
			�����ͷ�SendInfo�����ݻ�������

**************************************************************************************/

int SMSDISPATCH_Send(int handle, HWND hWnd, int msg, 
					 Address_t destaddr, PORT_SEND SendInfo)
{
	int offset=0;
	int nTmplen;

	if ( gAsynPortSend.bUsed == TRUE )
		return ERR_FAILURE;

	/* ���������� */
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
		/*��Ҫ��Ƭ����,ĿǰIE������SAR��16bit PORT */
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
		/*����ķ�����ÿһ�εķ�Ƭ��һ��*/
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
		/*�����͵����ݽ�����Ҫ��Ƭ��*/
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
	���յ�������datagramʱ��PostMessage()֪ͨ�ϲ�,���������Ϣ����ΪAddress_t��refnum��
	����Ա�֤��ȡ�����Ƿ�����Ϣ֪ͨ��ȡ���Ǹ�datagram��
	��������Ҫ����һ���������refnum��
*/
/*********************************************************************************
 �������ܣ�ͨ��GSM SMS�ȷ�ʽ�������ݡ��ϲ�ӵ��˿�����֪ͨ��Ϣ�������������������
			��ȡ�Ӷ˿ڽ��յ������ݡ���ʱ�����յ����������Ѿ��������������Ҫ��������
			���ݰ���
 ���������
		handle�����ŷַ�ģ����
		pRecvInfo��>nBufLen:�������ݵĻ������Ĵ�С

 ���������
		pRecvInfo��>pnDataLen��ʵ�ʽ��յ��Ķ˿����ݳ��ȡ����Ϊ0������������ֵ�����塣
		pRecvInfo��>pBuf���ϲ��������ڽ��ն˿����ݵĻ�����������������ʱ���û������洢
							���յ��Ķ˿����ݣ�		
		pRecvInfo��>pnCodeMode�����յ��Ķ˿����ݵı��뷽ʽ
		psraddr��	���յ����ݵ���Դ��


 ����ֵ��
	ERR_SUCCESS:��������ִ����ɡ�
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
			/*����һ�����������ݰ�������ϲ�Ľ������ݻ����������ݳ���ֵ��*/
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
			/*���Դ�˵�ַ��Ϣ*/
			strcpy(psrcaddr->loweraddr, pstDatagram->srcaddr.loweraddr);
			psrcaddr->port = pstDatagram->srcaddr.port;
            strcpy(psrcaddr->SCA,pstDatagram->srcaddr.SCA);
			pRecvInfo->nDataLen = retlen;
			pRecvInfo->nCodeMode = pstDatagram->dcs;
			strcpy(pRecvInfo->Time, pstDatagram->SendTime);

			/*�����ݰ���datagram list��ɾ��*/
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
 �������ܣ� ͨ��GSM SMS�ȷ�ʽ�����ı����š�
			�ϲ�����ı�����Ӧ����Ҫ���øýӿ�ע����ŷַ�ģ��Ļص�������

 ���������
		pCondition���绰�����ƥ������
		pCbFun	��	�ص������ĺ���ָ��

 �����������

 ����ֵ��ERR_SUCCESS
 ˵����  ��pCondition ���� NULLʱ��Ϊ��ͨ�ı����š�

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
 �������ܣ� ͨ��GSM SMS�ȷ�ʽ�����ı����š�
			�ϲ�����ı�����Ӧ����Ҫ���øýӿ���ע��ע�������Ϣ����

 ���������
		pCondition���绰�����ƥ������

 �����������

 ����ֵ��ERR_SUCCESS
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
 �������ܣ� �ϲ���ַǶ˿ڵĶ���Ӧ����Ҫ���øýӿ���ʵ��ͨ��GSM SMS�ȷ�ʽ�Ķ��ŵķ��͡�
			��������͵����ݳ���һ��������ذ��ĳ��ȣ���Ϊ�����ţ���Ҫ��Ƭ���͡�

 ���������
		hWnd: ����ִ����Ϻ�֪ͨ�Ĵ��ھ����
		Msg: ����ִ����Ϻ�֪ͨ�Ĵ�����Ϣ��
		pstSMSSend�������͵��ı����ŵĽṹ�壬�˽ṹ����Ϊ����ǰһ�汾ME�еĶ��壬��������Ӧ��
					�Ͳ���Ҫ�޸��ˡ�

 �����������

 ����ֵ��ERR_S

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
        /* ����˵����Ҫ��Ƭ���� */
        if (stSMSSend.dcs == DCS_GSM) 
            nTmplen = (140-6)*8/7;
        else
            nTmplen -= 6;/*6ΪUDHI+SARID+LEN��*/
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
			�ڲ�����ʵ�ֲ���
****************************************/

/*********************************************************************************
 �������ܣ����ں�������������MEģ�鷢��������Ϣ��
 ���������
		hWnd��		���ھ��
		wMsgCmd��	��Ϣ����	
		wParam��	��Ϣ����
		lParam��	��Ϣ����

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
			/* ����Ϣ��������ʶ�洢����������|| ����Ϣ״̬���浽������ʶ�洢����������*/
			int iRet;
			ME_CMTI_ITEM item;

            printf("\r\n SMSDispatch : Receive CMTI or CDSI \r\n");

			memset( &item, 0, sizeof(ME_CMTI_ITEM) );

			ME_GetResult(&item, sizeof(ME_CMTI_ITEM));

            printf("\r\n SMSDispatch : CMTI or CDSI index = %d \r\n",item.Index);

//			devtype = item.DevType;
//			iIndex = item.Index;
			/*����������SIM����ֱ��ɾ������֪ͨ�ϲ㡣����Ӧ��ȷ���㹻���пռ䣬
			�Ա�֤������ִ������*/
			if(g_stQueue.bFull == TRUE)
			{
				ME_SMS_Delete(g_stCtl.hwnd, WM_SMSDELETED, item.DevType, item.Index);
				break;
			}
			/*������*/
			g_stQueue.astSMSITEM[g_stQueue.iTail].index = item.Index;
			g_stQueue.astSMSITEM[g_stQueue.iTail].devtype = item.DevType;
			
			g_stQueue.iTail++;
			
			if(g_stQueue.iTail == QUEUE_LEN)
				g_stQueue.iTail = 0;
			/*�����п��Ե��¶�������������пռ��㹻����Ӧ�ó������������*/
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

			/*�����С�
			������һ����Ϊ�ա���Ϊֻ���ڽ����в����󣬲Żᷢ��
			WM_SMS_READ_RESULT����ˣ����յ���WM_SMS_READ_RESULT�󣬶���һ����Ϊ�ա�*/
			stSMSInfo.Index = g_stQueue.astSMSITEM[g_stQueue.iHead].index;
			iDevType = g_stQueue.astSMSITEM[g_stQueue.iHead].devtype;
			
			g_stQueue.iHead++;

			if(g_stQueue.iHead == QUEUE_LEN)
				g_stQueue.iHead = 0;

			/*�����п��Ե��¶��з���״̬��������Ϊ��ʱ�����������⴦��*/
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
			/* ����Ϣ״̬���浽������ʶ״̬�������ݣ�|| ����Ϣ��������ʶ����Ϣ���ݣ�*/
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
			/* С���㲥��Ϣ��������ʶ�洢����������*/
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
			/* С���㲥��Ϣ��������ʶ�㲥��Ϣ���ݣ�*/
			CBS_INFO stCBSInfo;
			int iRet;

			memset((void *)&stCBSInfo, 0, sizeof(CBS_INFO));
			iRet = ME_GetResult(&stCBSInfo, sizeof(CBS_INFO));
			DispatchNormalSMS(BC_SMS, &stCBSInfo);

			break;
		}
	case WM_SMSDELETED:

#ifdef TI_MODULE
		/*�����Ƿ�����*/
		/*���g_stInfo�ṹ��û����Ϣ����˵�����������������ݱ���ֻ�����յ����������ݱ�ʱ��
		������g_stInfo��������Ҫ��PostMessage���ÿա�
		if(g_stInfo.hwnd != NULL)
		{
		PostMessage(g_stInfo.hwnd, g_stInfo.msg, MAKEWPARAM(g_stInfo.lWparam, g_stInfo.hWparam), NULL);		
		memset((void *)&g_stInfo, 0, sizeof(struct INFO));
		}*/
		/*����Ϊ�յ����������Ҫ����*/
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
		
		/*�����п��Ե��¶��з���״̬��������Ϊ��ʱ�����������⴦��*/
		if(g_stQueueForSMSDEL.bFull == TRUE)
			g_stQueueForSMSDEL.bFull = FALSE;			
		
#endif
		
		//After deleting ,do nothing.
		break;

	case WM_ACKNOWLEDGE:
		
		//�����յ�OK����ERROR����Ϣ��������Ŀǰû�д���
		break;
	case WM_PORTSMS_SEND:
		if (wParam == ME_RS_SUCCESS) {
			int iRet;
		
			gAsynPortSend.SentSegs++;
			gAsynPortSend.SentLen += gAsynPortSend.SMSSend.ConLen;
			gAsynPortSend.offset += gAsynPortSend.SMSSend.ConLen;

			if (gAsynPortSend.SentSegs == gAsynPortSend.SumSegs) {
				/* ȫ�����ͳɹ� */
//				PostMessage(gAsynPortSend.hWnd, gAsynPortSend.msg, 
//					(WPARAM)ERR_SUCCESS, 
//					MAKELPARAM(gAsynPortSend.SentLen, gAsynPortSend.SentSegs));

				PostMessage(gAsynPortSend.hWnd, gAsynPortSend.msg,
                    MAKEWPARAM(gAsynNormalSend.SentSegs,wParam),lParam);	

				memset((void *)&gAsynPortSend, 0, sizeof(struct tagAsynPortSend));			
			}
			else if (gAsynPortSend.SentSegs == (gAsynPortSend.SumSegs - 1)) {
				/* �������һƬ */
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
				/* ȫ�����ͳɹ� */
//				PostMessage(gAsynNormalSend.hWnd, gAsynNormalSend.msg, 
//					(WPARAM)ERR_SUCCESS, 
//					MAKELPARAM(gAsynNormalSend.SentLen, gAsynNormalSend.SentSegs));
				PostMessage(gAsynNormalSend.hWnd, gAsynNormalSend.msg,
                    MAKEWPARAM(gAsynNormalSend.SentSegs,wParam),lParam);
				memset((void *)&gAsynNormalSend, 0, sizeof(struct tagAsynNormalSend));
				
			}
			else if (gAsynNormalSend.SentSegs == (gAsynNormalSend.SumSegs - 1)) {
				/* �������һƬ */
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
				/* ����Ҫ��Ƭ */
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
				/* ����Ҫ��Ƭ */
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
 �������ܣ������ŷַ�ģ����յ��ı�������Ϣ������Ϣʱ�������������������ı����ŵķַ���
 ���������
		pstSMSInfo��ͨ��MEģ��õ������ݶ�����Ϣ�ṹ��

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
 Ŀǰ��ʵ�ְ��������Ӵ���
*/
static BOOL IsMatching(char *srcstr, char *condition)
{
	return (strstr(srcstr, condition) == srcstr)?TRUE:FALSE;
}

/*********************************************************************************
�������ܣ������ŷַ�ģ����յ��˿ڶ���ʱ�����������������ɶ˿ڶ��ŵ�����ͷַ���
���������
	pstSMSInfo��ͨ��MEģ��õ��Ķ�����Ϣ�ṹ��
���������
	��
����ֵ��
	��
����˵����
	Ϊ���ṩJAVAӦ�ñ��뷽ʽ�ͽ���ʱ�����Ϣ����DATAGRAM�ṹ��������dcs��Time������dcs
	ȡֵΪ��һ����Ƭ�е�ֵ��timeȡֵΪ���һ����Ƭ��ֵ��
���⣺���һ����Ƭ�ͺ����Ƭ��dcs��ͬ��Ӧ�ö�������datagram��

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
				/*û���˳����������ֵ���Ȳ���ȷ�����ء�*/
				return;
			
			if(pstSMSInfo->UDH[i+1] == IE_PORT_LEN)
			{
				destport = MAKE_PORT_ADDR(pstSMSInfo->UDH[i+2], pstSMSInfo->UDH[i+3]);
				srcport = MAKE_PORT_ADDR(pstSMSInfo->UDH[i+4], pstSMSInfo->UDH[i+5]);
				
				i += IE_PORT_LEN + IE_FIELD_LEN + LIE_FIELD_LEN; /*��һ��UHD IE��־���±�ֵ*/
			}
			else /*�����ַ�ĳ��Ȳ�Ϊ4�����󷵻ء�*/
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
				
				i += 1 + IE_SAR_LEN + 1; /*��һ��UHD IE��־���±�ֵ*/
				sarflg = (maxfragment <= 1) ? FALSE : TRUE;
			}
			else /*�����ַ�ĳ��Ȳ�Ϊ4�����󷵻ء�*/
				
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
				
				i += 1 + IE_SAR_LEN8 + 1; /*��һ��UHD IE��־���±�ֵ*/
				sarflg = (maxfragment <= 1) ? FALSE : TRUE;
			}
			else /*�����ַ�ĳ��Ȳ�Ϊ4�����󷵻ء�*/
				
			{
				return;
			}
			break;
		default:
			
			if((i == pstSMSInfo->udhl - 1) || ((i + 1 + pstSMSInfo->UDH[i+1]) > pstSMSInfo->udhl - 1))
				return;
			
			i += pstSMSInfo->UDH[i+1] + IE_FIELD_LEN + LIE_FIELD_LEN; /*��һ��UHD IE��־���±�ֵ*/
		}

	}

	if((destport == 0xffff) || (srcport == 0xffff))
		/*���û��Ŀ�ĵ�ַ���Դ��ַ���ֵ�����ء�*/
		return;
		
	for(i=1; i<=CTLBLK_CNT; i++)
	{
		if(g_stCtl.handles[i].addr == destport)
		{
			/*Find out control block.*/
			DATAGRAM *pCurDatagram;

			/*û��SAR,ֻ��һƬ���ݣ�ͬʱһ��������datagram��㡣*/
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
				
				/*�����Ƿ���1��
				PostMessage(g_stCtl.CtlBlks[i].hwnd, g_stCtl.CtlBlks[i].msg, MAKEWPARAM((pstSMSInfo->ConLen - NO_SAR_HEADER_LEN), i), NULL);
				�����Ƿ���2��
				g_stInfo.hwnd = g_stCtl.CtlBlks[i].hwnd;
				g_stInfo.msg = g_stCtl.CtlBlks[i].msg;
				g_stInfo.lWparam = pstSMSInfo->ConLen - NO_SAR_HEADER_LEN;
				g_stInfo.hWparam = i;*/
				
				if(bQueue == TRUE)
				{			
				/*����������֪ͨWDP������Ӧ��ȷ���㹻���пռ䣬
					�Ա�֤������ִ������*/
					if(g_stQueueForSMSDEL.bFull == TRUE)
					{
						return;
					}
					/*������*/
					
					g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].hwnd = g_stCtl.handles[i].hwnd;
					g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].msg = g_stCtl.handles[i].msg;
					g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].lWparam = pstSMSInfo->ConLen;
					g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].hWparam = i;
					
					g_stQueueForSMSDEL.iTail++;
					
					if(g_stQueueForSMSDEL.iTail == DEL_QUEUE_LEN)
						g_stQueueForSMSDEL.iTail = 0;
					/*�����п��Ե��¶�������������пռ��㹻����Ӧ�ó������������*/
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

			/*�ж�Ƭ������������datagram���*/
			while(pCurDatagram != NULL)
			{
				if((strcmp(pCurDatagram->srcaddr.loweraddr, pstSMSInfo->SenderNum) == 0) &&
					(pCurDatagram->srcaddr.port == srcport) &&
					(pCurDatagram->refnum == refnum))
				{
					/*��datagram����Ѿ�����ʱ������segment���ڴ��Ѿ����䡣����ֻ�е�ַ
					��reference numberȫ������ͬʱ����˵���ҵ���ȷ��datagramλ�á�*/
                    SEGMENT* parrSegsTemp = NULL;

                    parrSegsTemp = pCurDatagram->parrSegs;

                    parrSegsTemp += (sequence-1);

					if(parrSegsTemp->len == 0)
						/*����0������û���յ���Ӧ��Ƭ�����ա������Ƭ�ظ��յ���������*/
					{
						/* ������벻һ�£�����Ҫ��������Datagram�� */
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
							/*�����Ƿ���һ��
							*PostMessage(g_stCtl.CtlBlks[i].hwnd, g_stCtl.CtlBlks[i].msg, MAKEWPARAM(len, i), NULL);	
							*/
							/*�����Ƿ�������
							g_stInfo.hwnd = g_stCtl.CtlBlks[i].hwnd;
							g_stInfo.msg = g_stCtl.CtlBlks[i].msg;
							g_stInfo.lWparam = len;
							g_stInfo.hWparam = i;*/
							
							if(bQueue == TRUE)
							{
							/*����������֪ͨWDP������Ӧ��ȷ���㹻���пռ䣬
								�Ա�֤������ִ������*/
								if(g_stQueueForSMSDEL.bFull == TRUE)
								{
									return;
								}
								/*������*/
								g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].hwnd = g_stCtl.handles[i].hwnd;
								g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].msg = g_stCtl.handles[i].msg;
								g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].lWparam = len;
								g_stQueueForSMSDEL.astInfo[g_stQueueForSMSDEL.iTail].hWparam = i;
								
								g_stQueueForSMSDEL.iTail++;
								
								if(g_stQueueForSMSDEL.iTail == DEL_QUEUE_LEN)
									g_stQueueForSMSDEL.iTail = 0;
								/*�����п��Ե��¶�������������пռ��㹻����Ӧ�ó������������*/
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
						/* �������� */
						return;
				}
				else
				{
					pCurDatagram = pCurDatagram->pNext;
					
				}
		
			}//end while(pCurDatagram != NULL)
				
			//�ж�Ƭ����������datagram��㣬����һƬ��Ƭ��
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
�������ܣ�
	ͨ������SMS_INFO�ṹ���õ��������ͣ����ŷַ�ģ����ݴ����������з�Ƭ����Ͷ��ŷַ���
���������
	stSMSInfo�����ŷַ�ģ���ME�õ��Ķ��Žṹ��Ϣ��
���������
	��
����ֵ��
	Ŀǰֻ���������ͣ�
	DPTCHTYPE_NORMAL�������ַ�����ͨ����Ӧ�á�
	SMMTYPE_PORT�������ַ������ж˿���Ϣ�Ķ���Ӧ�á�
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
