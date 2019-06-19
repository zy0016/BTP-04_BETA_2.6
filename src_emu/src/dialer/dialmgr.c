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

//拨号参数
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

#define QUEUE_SIZE 10               // 队列的大小，比实际大小大1 : MMS + WAP + EMAIL 

typedef struct tagDIALQUEUE
{
    int head;
    int tail;
    DIALPARAM params[QUEUE_SIZE];
} DIALQUEUE, *PDIALQUEUE;

// 定义拨号状态
enum
{
    DS_IDLE,                            // 未连接
    DS_DIALING,                         // 正在拨号
    DS_CONNECTED,                       // 拨号连接成功
    DS_HANGUPING                        // 正在挂断
};

static int          nCurrentState;    // 当前拨号状态

static DIALQUEUE    CurrentDialQueue; // 当前拨号调用队列
static DIALQUEUE    DialWaitQueue;    // 拨号等待队列

static HWND			hwndDialMgr;

static void FillDialParam(DIALPARAM * pDialParam, HWND hWnd, UINT msg, 
								 int nSelectType, void *pDialInfo);
// 比较拨号的参数
static BOOL CompareDialParam(PDIALPARAM pCurrentDial, PDIALPARAM pWaitDial);		
static LRESULT DialMgrWindowProc(HWND hWnd, UINT message, WPARAM wParam, 
                                 LPARAM lParam);

/*
**  Function : DIALMGR_Initialize
**  Purpose  :
**      在系统初始化时被调用，对DIALER模块进行环境参数设置。
**  Remarks  :
**      需要保证调用成功，否则拨号不能正常运行。
*/
int DIALMGR_Initialize(void)
{
    WNDCLASS wc;

    // 防止重复调用
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

    // 检查参数有效性
    if (((nSelectType == DIALDEF_SELFGPRS) || 
        (nSelectType == DIALDEF_SELFDATA)) &&
        pDialInfo == NULL)
        return DIALER_INVALID_ISP;

    // 没有模块调用拨号，直接拨号，成功后保存拨号信息
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

	// 当前有模块调用拨号，比较拨号信息，如果拨号信息相同，将请求模块加入拨号队列
    if (nCurrentState == DS_CONNECTED || nCurrentState == DS_DIALING)	
    {
        // 如果拨号信息相同
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

    // 如果不等待，返回DIALER_REQ_REFUSE
    if (!bWait)
        return DIALER_REQ_REFUSE;

    // 等待队列满，返回DIALER_REQ_REFUSE
    if ((DialWaitQueue.tail + 1) % QUEUE_SIZE == DialWaitQueue.head)
    {
        ASSERT(0);
        return DIALER_REQ_REFUSE;
    }

    // 加入拨号等待队列
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

            // 通知当前拨号的窗口拨号挂断
            PostMessage(pDial->hWnd, pDial->msg, 0, 
                DIALER_RUN_NETBROKEN);
            
            // 删除该拨号
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

            // 删除该等待拨号
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

//加入相应的项
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

//比较当前拨号队列中项的拨号信息和等待拨号的信息是否相同。
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

// DialMgr窗口过程调度 
static LRESULT DialMgrWindowProc(HWND hWnd, UINT message, WPARAM wParam, 
                                 LPARAM lParam)
{
    PDIALPARAM pDial;
    int nRet, i, j;

    if (message == WM_DIAL_NOTIFY)
    {
        switch (lParam)
        {
        case DIALER_RUN_NETCONNECT :       // 连接成功

            if (nCurrentState != DS_DIALING)
                break;

			DlmNotify(PS_SETGPRS, 2); //通知程序管理器连接成功
            nCurrentState = DS_CONNECTED;

			// 给当前拨号窗口发送消息
            PostQueueMessage(&CurrentDialQueue, wParam, lParam);

            break;

        case DIALER_RUN_NETBROKEN :       // 网络断连

            ASSERT(nCurrentState != DS_IDLE);

			DlmNotify(PS_SETGPRS, 3); //通知程序管理器网络挂断

            // 转发网络异常消息给所有当前拨号窗口并清空拨号队列
            PostQueueMessage(&CurrentDialQueue, wParam, lParam);
            CurrentDialQueue.head = 0;
            CurrentDialQueue.tail = 0;

            nCurrentState = DS_IDLE;

            // 如果有等待拨号窗口，依次处理每个等待拨号请求
            while (DialWaitQueue.head != DialWaitQueue.tail)
            {
                // 从等待队列中取第一个等待拨号
                pDial = &DialWaitQueue.params[DialWaitQueue.head];
                DialWaitQueue.head = (DialWaitQueue.head + 1) % 
                    QUEUE_SIZE;

                ASSERT(pDial->hWnd != NULL);

                // 开始新的拨号
                nRet =  DIALER_SelConnect(hwndDialMgr, WM_DIAL_NOTIFY, 
                    pDial->type, &pDial->info);
                
                // 拨号请求被接受，状态改为拨号中
                if (nRet == DIALER_REQ_ACCEPT)
                {
                    // 加入当前拨号队列
                    FillDialParam(
                        &CurrentDialQueue.params[CurrentDialQueue.tail], 
                        pDial->hWnd, pDial->msg, pDial->type, &pDial->info);
                    CurrentDialQueue.tail = CurrentDialQueue.tail % QUEUE_SIZE;

                    nCurrentState = DS_DIALING;

                    break;
                }

                // 拨号请求未被接受，发送网络断连消息给当前拨号窗口
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
                    // 加入当前拨号队列
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

        case DIALER_RUN_NET_ABNORMAL :    // 网络状态异常

            nCurrentState = DS_IDLE;

			DlmNotify(PS_SETGPRS, 3); //通知程序管理器网络挂断

            // 转发网络异常消息给所有当前拨号窗口并清空拨号队列
            PostQueueMessage(&CurrentDialQueue, wParam, lParam);
            CurrentDialQueue.head = 0;
            CurrentDialQueue.tail = 0;

            // 转发网络异常消息给所有等待窗口并清空等待队列
            PostQueueMessage(&DialWaitQueue, wParam, lParam);
            DialWaitQueue.head = 0;
            DialWaitQueue.tail = 0;

            break;

        default :

			// 给当前拨号窗口发送消息
            PostQueueMessage(&CurrentDialQueue, wParam, lParam);
            break;
        }

        return 0;
    }
    else if (message == WM_CLOSE)
        return 0;

    return DefWindowProc(hWnd, message, wParam, lParam);
}
