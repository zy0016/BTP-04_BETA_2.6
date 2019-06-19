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

#include "window.h"
#include "string.h"
#include "dialmgr.h"
#include "assert.h"
#include "plx_pdaex.h"

#define DIALMGRCLASS    "DialMgrClass"
#define WM_DIAL_NOTIFY  WM_USER

//���Ų���
typedef struct tagDIALPARAM
{
    HWND    hWnd;
    UINT    msg;
    int     type;
    union
    {
        DIALER_GPRSINFO gprs;
        DIALER_DATAINFO data;
    } info;
} DIALPARAM, *PDIALPARAM;

#define QUEUE_SIZE 10               // ���еĴ�С����ʵ�ʴ�С��1 : MMS + WAP + EMAIL 

typedef struct tagDIALQUEUE
{
    int head;
    int tail;
    DIALPARAM params[QUEUE_SIZE];
} DIALQUEUE, *PDIALQUEUE;

// ���岦��״̬
enum
{
    DS_IDLE,                            // δ����
    DS_DIALING,                         // ���ڲ���
    DS_CONNECTED,                       // �������ӳɹ�
    DS_HANGUPING                        // ���ڹҶ�
};

static int          nCurrentState;    // ��ǰ����״̬

static DIALQUEUE    CurrentDialQueue; // ��ǰ���ŵ��ö���
static DIALQUEUE    DialWaitQueue;    // ���ŵȴ�����

static HWND			hwndDialMgr;

static void FillDialParam(DIALPARAM * pDialParam, HWND hWnd, UINT msg, 
								 int nSelectType, void *pDialInfo);
// �Ƚϲ��ŵĲ���
static BOOL CompareDialParam(PDIALPARAM pCurrentDial, PDIALPARAM pWaitDial);		
static LRESULT DialMgrWindowProc(HWND hWnd, UINT message, WPARAM wParam, 
                                 LPARAM lParam);

/*
**  Function : DIALMGR_Initialize
**  Purpose  :
**      ��ϵͳ��ʼ��ʱ�����ã���DIALERģ����л����������á�
**  Remarks  :
**      ��Ҫ��֤���óɹ������򲦺Ų����������С�
*/
int DIALMGR_Initialize(void)
{
    WNDCLASS wc;

    // ��ֹ�ظ�����
    if (IsWindow(hwndDialMgr))
        return -1;

    if (DIALER_Initialize() == -1)
        return -1;

    wc.style            = 0;
    wc.lpfnWndProc      = DialMgrWindowProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = DIALMGRCLASS;

    if (!RegisterClass(&wc))
    {
        ASSERT(0);
        return -1;
    }

    hwndDialMgr = CreateWindow(DIALMGRCLASS, "", WS_POPUP, 0, 0, 0, 0, 
        NULL, NULL,	NULL, NULL);
    ASSERT(hwndDialMgr != NULL);

    return 0;
}

/*
**  Function : DIALMGR_SelConnect
**  Purpose  :
**      
*/
int DIALMGR_SelConnect(HWND hWnd, UINT msg, int nSelectType, void *pDialInfo, 
                       BOOL bWait)
{
    int nRet;
    DIALPARAM WaitDial;

    // ��������Ч��
    if (((nSelectType == DIALDEF_SELFGPRS) || 
        (nSelectType == DIALDEF_SELFDATA)) &&
        pDialInfo == NULL)
        return DIALER_INVALID_ISP;

    // û��ģ����ò��ţ�ֱ�Ӳ��ţ��ɹ��󱣴沦����Ϣ
    if (nCurrentState == DS_IDLE)
    {
        nRet = DIALER_SelConnect(hwndDialMgr, WM_DIAL_NOTIFY, 
            nSelectType, pDialInfo);
        if (nRet == DIALER_REQ_ACCEPT)
        {
            FillDialParam(&CurrentDialQueue.params[CurrentDialQueue.tail], 
                hWnd, msg, nSelectType, pDialInfo);
			CurrentDialQueue.tail = (CurrentDialQueue.tail + 1) % QUEUE_SIZE;
            nCurrentState = DS_DIALING;
        }

        return nRet;
    }

    FillDialParam(&WaitDial, hWnd, msg, nSelectType, pDialInfo);

	// ��ǰ��ģ����ò��ţ��Ƚϲ�����Ϣ�����������Ϣ��ͬ��������ģ����벦�Ŷ���
    if (nCurrentState == DS_CONNECTED || nCurrentState == DS_DIALING)	
    {
        // ���������Ϣ��ͬ
        if (CompareDialParam(&CurrentDialQueue.params[CurrentDialQueue.head], 
            &WaitDial))
        {	
            if ((CurrentDialQueue.tail + 1) % QUEUE_SIZE == 
                CurrentDialQueue.head)
                return DIALER_REQ_REFUSE;

            FillDialParam(&CurrentDialQueue.params[CurrentDialQueue.tail], 
                hWnd, msg, nSelectType, pDialInfo);
            CurrentDialQueue.tail = (CurrentDialQueue.tail + 1) % QUEUE_SIZE;
            
            if (nCurrentState == DS_CONNECTED)
                PostMessage(hWnd, msg, 0, DIALER_RUN_NETCONNECT);

            return DIALER_REQ_ACCEPT;
        }
    }

    // ������ȴ�������DIALER_REQ_REFUSE
    if (!bWait)
        return DIALER_REQ_REFUSE;

    // �ȴ�������������DIALER_REQ_REFUSE
    if ((DialWaitQueue.tail + 1) % QUEUE_SIZE == DialWaitQueue.head)
    {
        ASSERT(0);
        return DIALER_REQ_REFUSE;
    }

    // ���벦�ŵȴ�����
    FillDialParam(&DialWaitQueue.params[DialWaitQueue.tail], hWnd, msg,
        nSelectType, pDialInfo);
    DialWaitQueue.tail = (DialWaitQueue.tail + 1) % QUEUE_SIZE;

    return DIALER_REQ_ACCEPT;
}

/*
**  Function : DIALMGR_HangUp
**  Purpose  :
**      
*/
int DIALMGR_HangUp(HWND hWnd)
{
    int i, j, nRet;
    PDIALPARAM pDial;

    i = CurrentDialQueue.head;
    while (i != CurrentDialQueue.tail)
    {
        pDial = &CurrentDialQueue.params[i];
        if (pDial->hWnd == hWnd)
        {
            if (((CurrentDialQueue.head + 1) % QUEUE_SIZE) == 
                CurrentDialQueue.tail)
            {
                if (nCurrentState == DS_DIALING)
                {
                    nRet = DIALER_DialCancel();
                    if (DIALER_REQ_ACCEPT == nRet)
                        nCurrentState = DS_HANGUPING;
                    return nRet;
                }
                
                nRet = DIALER_HangUp();
                if (DIALER_REQ_ACCEPT == nRet)
                    nCurrentState = DS_HANGUPING;
                return nRet;
            }

            // ֪ͨ��ǰ���ŵĴ��ڲ��ŹҶ�
            PostMessage(pDial->hWnd, pDial->msg, 0, 
                DIALER_RUN_NETBROKEN);
            
            // ɾ���ò���
            j = i;
            i = (i + 1) % QUEUE_SIZE;
            
            while (i != CurrentDialQueue.tail)
            {
                pDial = &CurrentDialQueue.params[i];
                
                FillDialParam(&CurrentDialQueue.params[j], pDial->hWnd, 
                    pDial->msg, pDial->type, &pDial->info);
                
                j = i;
                i = (i + 1) % QUEUE_SIZE;
            }

            CurrentDialQueue.tail = j;

            return DIALER_REQ_ACCEPT;
        }

        i = (i + 1) % QUEUE_SIZE;
    }

    i = DialWaitQueue.head;
    while (i != DialWaitQueue.tail)
    {
        if (DialWaitQueue.params[i].hWnd == hWnd)
        {
            DialWaitQueue.params[i].hWnd = NULL;
            PostMessage(hWnd, DialWaitQueue.params[i].msg, 0, 
                (LPARAM)DIALER_RUN_NETBROKEN);

            // ɾ���õȴ�����
            j = i;
            i = (i + 1) % QUEUE_SIZE;
            
            while (i != DialWaitQueue.tail)
            {
                pDial = &DialWaitQueue.params[i];
                
                FillDialParam(&DialWaitQueue.params[j], pDial->hWnd, 
                    pDial->msg, pDial->type, &pDial->info);
                
                j = i;
                i = (i + 1) % QUEUE_SIZE;
            }

            DialWaitQueue.tail = j;

            return DIALER_REQ_ACCEPT;
        }
        
        i = (i + 1) % QUEUE_SIZE;
    }

    return DIALER_REQ_REFUSE;
}

//������Ӧ����
static void FillDialParam(DIALPARAM * pDialParam, HWND hWnd, UINT msg, 
								 int nSelectType, void *pDialInfo)
{
    pDialParam->hWnd = hWnd;
    pDialParam->msg = msg;
    pDialParam->type = nSelectType;
	
    switch (nSelectType)
    {
    case DIALDEF_SELFGPRS:
        ASSERT(pDialInfo != NULL);
        memcpy(&pDialParam->info, pDialInfo, sizeof(DIALER_GPRSINFO));
        break;
        
    case DIALDEF_SELFDATA :
        ASSERT(pDialInfo != NULL);
        memcpy(&pDialParam->info, pDialInfo, sizeof(DIALER_DATAINFO));
        break;
    }
}

//�Ƚϵ�ǰ���Ŷ�������Ĳ�����Ϣ�͵ȴ����ŵ���Ϣ�Ƿ���ͬ��
static BOOL CompareDialParam(PDIALPARAM pCurrentDial, PDIALPARAM pWaitDial)
{
	if( DIALER_CompareInfo( pWaitDial->type, &(pWaitDial->info) ) == 1 )
		return TRUE;

	return FALSE;

/* Modify by jjxu, Replace by DIALER_CompareInfo()
    if (pCurrentDial->type != pWaitDial->type)
        return FALSE;

    switch (pCurrentDial->type)
    {
    case DIALDEF_SELFGPRS:
        
        return memcmp(&pCurrentDial->info, &pWaitDial->info, 
            sizeof(DIALER_GPRSINFO)) == 0;
        
    case DIALDEF_SELFDATA :
        
        return memcmp(&pCurrentDial->info, &pWaitDial->info, 
            sizeof(DIALER_DATAINFO)) == 0;
    }

    return TRUE;
*/
}

static void PostQueueMessage(PDIALQUEUE pDialQueue, WPARAM wParam, 
                             LPARAM lParam)
{
    int i;

    i = pDialQueue->head;
    while (i != pDialQueue->tail)
    {
        if (pDialQueue->params[i].hWnd)
        {
            PostMessage(pDialQueue->params[i].hWnd, 
                pDialQueue->params[i].msg, wParam, lParam);
        }
        
        i = (i + 1) % QUEUE_SIZE;
    }
}

// DialMgr���ڹ��̵��� 
static LRESULT DialMgrWindowProc(HWND hWnd, UINT message, WPARAM wParam, 
                                 LPARAM lParam)
{
    PDIALPARAM pDial;
    int nRet, i, j;

    if (message == WM_DIAL_NOTIFY)
    {
        switch (lParam)
        {
        case DIALER_RUN_NETCONNECT :       // ���ӳɹ�

            if (nCurrentState != DS_DIALING)
                break;

			DlmNotify(PS_SETGPRS, 2); //֪ͨ������������ӳɹ�
            nCurrentState = DS_CONNECTED;

			// ����ǰ���Ŵ��ڷ�����Ϣ
            PostQueueMessage(&CurrentDialQueue, wParam, lParam);

            break;

        case DIALER_RUN_NETBROKEN :       // �������

            ASSERT(nCurrentState != DS_IDLE);

			DlmNotify(PS_SETGPRS, 3); //֪ͨ�������������Ҷ�

            // ת�������쳣��Ϣ�����е�ǰ���Ŵ��ڲ���ղ��Ŷ���
            PostQueueMessage(&CurrentDialQueue, wParam, lParam);
            CurrentDialQueue.head = 0;
            CurrentDialQueue.tail = 0;

            nCurrentState = DS_IDLE;

            // ����еȴ����Ŵ��ڣ����δ���ÿ���ȴ���������
            while (DialWaitQueue.head != DialWaitQueue.tail)
            {
                // �ӵȴ�������ȡ��һ���ȴ�����
                pDial = &DialWaitQueue.params[DialWaitQueue.head];
                DialWaitQueue.head = (DialWaitQueue.head + 1) % 
                    QUEUE_SIZE;

                ASSERT(pDial->hWnd != NULL);

                // ��ʼ�µĲ���
                nRet =  DIALER_SelConnect(hwndDialMgr, WM_DIAL_NOTIFY, 
                    pDial->type, &pDial->info);
                
                // �������󱻽��ܣ�״̬��Ϊ������
                if (nRet == DIALER_REQ_ACCEPT)
                {
                    // ���뵱ǰ���Ŷ���
                    FillDialParam(
                        &CurrentDialQueue.params[CurrentDialQueue.tail], 
                        pDial->hWnd, pDial->msg, pDial->type, &pDial->info);
                    CurrentDialQueue.tail = CurrentDialQueue.tail % QUEUE_SIZE;

                    nCurrentState = DS_DIALING;

                    break;
                }

                // ��������δ�����ܣ��������������Ϣ����ǰ���Ŵ���
                PostMessage(pDial->hWnd, pDial->msg, 0, lParam);
            }

            if (nCurrentState == DS_IDLE)
                break;

            i = DialWaitQueue.head;
            j = DialWaitQueue.head;

            while (i != DialWaitQueue.tail)
            {
                pDial = &DialWaitQueue.params[i];
                ASSERT(pDial->hWnd != NULL);

                if (CompareDialParam(pDial, 
                    &CurrentDialQueue.params[CurrentDialQueue.head]))
                {
                    // ���뵱ǰ���Ŷ���
                    FillDialParam(&CurrentDialQueue.params[CurrentDialQueue.tail], 
                        pDial->hWnd, pDial->msg, pDial->type, &pDial->info);
                    CurrentDialQueue.tail = ( CurrentDialQueue.tail + 1 ) % QUEUE_SIZE;
                }
                else
                {
                    if (i != j)
                    {
                        FillDialParam(&DialWaitQueue.params[j], pDial->hWnd, 
                            pDial->msg, pDial->type, &pDial->info);
                    }

                    j = (j + 1) % QUEUE_SIZE;
                }

                i = (i + 1) %  QUEUE_SIZE;
            }

            DialWaitQueue.tail = j;

            break;

        case DIALER_RUN_NET_ABNORMAL :    // ����״̬�쳣

            nCurrentState = DS_IDLE;

			DlmNotify(PS_SETGPRS, 3); //֪ͨ�������������Ҷ�

            // ת�������쳣��Ϣ�����е�ǰ���Ŵ��ڲ���ղ��Ŷ���
            PostQueueMessage(&CurrentDialQueue, wParam, lParam);
            CurrentDialQueue.head = 0;
            CurrentDialQueue.tail = 0;

            // ת�������쳣��Ϣ�����еȴ����ڲ���յȴ�����
            PostQueueMessage(&DialWaitQueue, wParam, lParam);
            DialWaitQueue.head = 0;
            DialWaitQueue.tail = 0;

            break;

        default :

			// ����ǰ���Ŵ��ڷ�����Ϣ
            PostQueueMessage(&CurrentDialQueue, wParam, lParam);
            break;
        }

        return 0;
    }
    else if (message == WM_CLOSE)
        return 0;

    return DefWindowProc(hWnd, message, wParam, lParam);
}
