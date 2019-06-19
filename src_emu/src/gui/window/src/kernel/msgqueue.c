/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : manage the message pool and queue
 *            
\**************************************************************************/

#include "hpwin.h"
#include "string.h"
#include "winmem.h"
#include "wsobj.h"
#include "wsthread.h"
#include "msgqueue.h"

/* Internal function declaration. */
static BOOL         InitSendMsgPool(void);
static void         DestroySendMsgPool(void);
static PSENDMSG     GetSendMsgObjFromPool(void);
static void         ReleaseSendMsgObjToPool(PSENDMSG pSendMsg);

static BOOL         InitPostMsgPool(void);
static void         DestroyPostMsgPool(void);
static PPOSTMSG     GetPostMsgObjFromPool(void);
static void         ReleasePostMsgObjToPool(PPOSTMSG pPostMsg);

static BOOL         InitPaintMsgPool(void);
static void         DestroyPaintMsgPool(void);
static PPAINTMSG    GetPaintMsgObjFromPool(void);
static void         ReleasePaintMsgObjToPool(PPAINTMSG pPaintMsg);

/*********************************************************************\
* Function	   MSQ_SendMessage
* Purpose      
* Params	   
* Return	 	   
* Remarks	   �ܳ��ں���
**********************************************************************/
LRESULT MSQ_SendMessage(PWINOBJ pWin, UINT wMsgCmd, WPARAM wParam, 
                           LPARAM lParam)
{
    DWORD       dwThreadId;
    PTHREADINFO pThreadInfoSrc, pThreadInfoDst;
    PSENDMSG    pSendMsg, pMsg;
    LRESULT     nRetVal;

    // Ensures the window object is a valid object
    ASSERT(pWin != NULL && WND_IsWindow(pWin));

    dwThreadId = WS_GetCurrentThreadId();
    if (pWin->dwThreadId == dwThreadId)
    {
        ASSERT(pWin != NULL && WND_IsWindow(pWin));
        /* �߳���Send��Ϣ��ֱ�ӵ��� */
        LEAVEMONITOR;

        nRetVal = pWin->pfnWndProc((HWND)WOT_GetHandle((PWSOBJ)pWin), wMsgCmd, wParam, lParam);

        ENTERMONITOR;

        return nRetVal;
    }

    if (THREAD_ISDESTROYED(pWin->dwThreadId))
    {
        //Ŀ���߳��Ѿ�������
        ASSERT(0);
        return 0;
    }

    pThreadInfoSrc = WS_GetThreadInfo(dwThreadId);
    ASSERT(pThreadInfoSrc != NULL);

    pThreadInfoDst = WS_GetThreadInfo(pWin->dwThreadId);
    ASSERT(pThreadInfoDst != NULL);

    /* ����Ϣ���еõ�һ����Ϣ�ṹ����û�г�ʼ���� */
    pSendMsg = (PSENDMSG)GetMsgObj(MSGOBJ_SEND);
    if (pSendMsg == NULL)
    {
        SetLastError(1);
        return 0;
    }

    pSendMsg->dwThreadId    = dwThreadId;
    pSendMsg->type          = SENDMSG_SEND;
    pSendMsg->msg.hwnd      = (HWND)WOT_GetHandle((PWSOBJ)pWin);
    pSendMsg->msg.message   = wMsgCmd;
    pSendMsg->msg.wParam    = wParam;
    pSendMsg->msg.lParam    = lParam;
    pSendMsg->retval        = 0;

    /* �����Ѿ����͵���Ϣ������type��ΪSENDMSG_REPLYʱ������Ӧ����Ϣ�ˡ�
    ** ������֤��Ӧ����Ϣ��˳��ÿ��ȡӦ����Ϣʱֻ�ж�ͷ�ڵ��Ƿ�Ӧ��
    */
    MSQ_InsertReplyMsg(pThreadInfoSrc, pSendMsg);

    /* ���뵽Ŀ���̵߳���Ϣ�����У��ڹܳ��ڲ����������ᱻ��ϡ���������Ӧ
    ** �Ļ��Ѳ��� 
    */
    MSQ_InsertSendMsg(pThreadInfoDst, pSendMsg);

    while (TRUE)
    {
        /* �ȴ��ش���Ϣ����Send��Ϣ���е�sem�ϵȴ������ܱ�Ӧ����Ϣ������
        ** �̵߳�Send��Ϣ���ѡ�������!���ڲ�ʵ���˳��̵ܳȴ���
        */
        pMsg = MSQ_WaitReplyMessage();

        ASSERT(pMsg != NULL);
        if (pMsg == NULL)
            continue;

        if (pMsg->type == SENDMSG_REPLY)
        {
            /* �õ������ڵȴ���Ӧ����Ϣ�����ع̴ܳ��� 
            ** ��εõ���Ӧ����Ϣ��Ȼ�Ǳ��εȴ�����Ϣ��Ҳ��Ȼ��ǰ������Ӧ
            ** ������е�Send��Ϣ��ֻ����Ŀ���̸߳ı��������Ϣ������Ϊ
            ** SENDMSG_REPLY��������˷���ֵ 
            */
            ASSERT(pMsg == pSendMsg);
            break;
        }

        MSQ_ReplyMessage(pMsg);
    }

    nRetVal = pMsg->retval;
    /* �ͷ����Ӧ����Ϣ����Ϣ�� */
    ReleaseMsgObj(pMsg, MSGOBJ_SEND);

    return nRetVal;
}

/*********************************************************************\
* Function	   MSQ_SendNotifyMessage
* Purpose      
* Params	   
* Return	 	   
* Remarks	   �ܳ��ں���
**********************************************************************/
BOOL MSQ_SendNotifyMessage(PWINOBJ pWin, UINT wMsgCmd, WPARAM wParam, 
                           LPARAM lParam)
{
    DWORD       dwThreadId;
    PTHREADINFO pThreadInfoSrc, pThreadInfoDst;
    PSENDMSG    pSendMsg;

    // Ensures the window object is a valid object
    ASSERT(pWin != NULL && WND_IsWindow(pWin));

    dwThreadId = WS_GetCurrentThreadId();
    if (pWin->dwThreadId == dwThreadId)
    {
        /* �߳���Send��Ϣ��ֱ�ӵ��� */
        LEAVEMONITOR;

        pWin->pfnWndProc((HWND)WOT_GetHandle((PWSOBJ)pWin), wMsgCmd, wParam, lParam);

        ENTERMONITOR;

        return TRUE;
    }

    if (THREAD_ISDESTROYED(pWin->dwThreadId))
    {
        //Ŀ���߳��Ѿ�������
        ASSERT(0);
        return FALSE;
    }

    pThreadInfoSrc = WS_GetThreadInfo(dwThreadId);
//    ASSERT(pThreadInfoSrc != NULL);

    pThreadInfoDst = WS_GetThreadInfo(pWin->dwThreadId);
    ASSERT(pThreadInfoDst != NULL);

    /* ����Ϣ���еõ�һ����Ϣ�ṹ����û�г�ʼ���� */
    pSendMsg = (PSENDMSG)GetMsgObj(MSGOBJ_SEND);
    if (pSendMsg == NULL)
    {
        SetLastError(1);
        return FALSE;
    }

    pSendMsg->dwThreadId    = dwThreadId;
    pSendMsg->type          = SENDMSG_NOTIFY;
    pSendMsg->msg.hwnd      = (HWND)WOT_GetHandle((PWSOBJ)pWin);
    pSendMsg->msg.message   = wMsgCmd;
    pSendMsg->msg.wParam    = wParam;
    pSendMsg->msg.lParam    = lParam;
    pSendMsg->retval        = 0;

    /* ���뵽Ŀ���̵߳���Ϣ�����У��ڹܳ��ڲ����������ᱻ��ϡ���������Ӧ
    ** �Ļ��Ѳ�����Notify��Ϣ��ͬPost��Ϣ����ֱ�ӷ��ء�
    */
    MSQ_InsertSendMsg(pThreadInfoDst, pSendMsg);

    /* Notify��Ϣ��Ŀ���߳��ͷŵ���Ϣ�� */
    return TRUE;
}

/*********************************************************************\
* Function	   MSQ_PostMessage
* Purpose      
* Params	   
* Return	 	   
* Remarks	   �ܳ��ں���
**********************************************************************/
BOOL MSQ_PostMessage(PWINOBJ pWin, UINT uMsgCmd, WPARAM wParam, 
                     LPARAM lParam)
{
    PTHREADINFO pThreadInfo;
    PPOSTMSG pPostMsg;
    
    if (THREAD_ISDESTROYED(pWin->dwThreadId))
    {
        //Ŀ���߳��Ѿ�������
        ASSERT(0);
        return 0;
    }
    
    pThreadInfo = WS_GetThreadInfo(pWin->dwThreadId);
    ASSERT(pThreadInfo != NULL);

    if (uMsgCmd >= WM_MOUSEFIRST && uMsgCmd <= WM_MOUSELAST)
    {
    }
    if (uMsgCmd == WM_TIMER)
    {
    }
    if (uMsgCmd >= WM_KEYFIRST && uMsgCmd <= WM_KEYLAST)
    {
    }
    if (uMsgCmd == WM_PAINT)
    {
    }

    pPostMsg = GetMsgObj(MSGOBJ_POST);
    if (pPostMsg == NULL)
    {
        SetLastError(1);
        return FALSE;
    }

    pPostMsg->dwThreadId    = WS_GetCurrentThreadId();
    pPostMsg->msg.hwnd      = (HWND)WOT_GetHandle((PWSOBJ)pWin);
    pPostMsg->msg.message   = uMsgCmd;
    pPostMsg->msg.wParam    = wParam;
    pPostMsg->msg.lParam    = lParam;

    return MSQ_InsertPostMsg(pThreadInfo, pPostMsg);
}

/*********************************************************************\
* Function	   MSQ_PostThreadMessage
* Purpose      
* Params	   
* Return	 	   
* Remarks	   �ܳ��ں���
**********************************************************************/
BOOL MSQ_PostThreadMessage(DWORD dwThreadId, UINT uMsgCmd, WPARAM wParam, 
                     LPARAM lParam)
{
    PTHREADINFO pThreadInfo;
    PPOSTMSG pPostMsg;
    
    if (THREAD_ISDESTROYED(dwThreadId))
    {
        //Ŀ���߳��Ѿ�������
        ASSERT(0);
        return FALSE;
    }
    
    pThreadInfo = WS_GetThreadInfo(dwThreadId);
    if (pThreadInfo == NULL)
        return FALSE;
    
    pPostMsg = GetMsgObj(MSGOBJ_POST);
    if (pPostMsg == NULL)
    {
        SetLastError(1);
        return FALSE;
    }

    pPostMsg->dwThreadId    = WS_GetCurrentThreadId();
    pPostMsg->msg.hwnd      = NULL;
    pPostMsg->msg.message   = uMsgCmd;
    pPostMsg->msg.wParam    = wParam;
    pPostMsg->msg.lParam    = lParam;

    return MSQ_InsertPostMsg(pThreadInfo, pPostMsg);
}

/*********************************************************************\
* Function	   MSQ_WaitReplyMessage
* Purpose      
* Params	   
* Return	 	   
* Remarks	    �ܳ��ں���
**********************************************************************/
PSENDMSG MSQ_WaitReplyMessage(void)
{
    PTHREADINFO pThreadInfo;
    PSENDMSG pMsg;

    pThreadInfo = WS_GetCurrentThreadInfo();

    while (TRUE)
    {
        /* ��ȡһ����Ϣ�����û�о��˳��̣ܳ�����ȴ� */
        pMsg = MSQ_GetReplyMsg(pThreadInfo, TRUE);
        if (pMsg != NULL)
            break;
        
        pMsg = MSQ_GetSendMsg(pThreadInfo, TRUE);
        if (pMsg != NULL)
            break;

//        printf("Thread %d:Want WAIT%s,%d\n", GetCurrentThreadId(),__FILE__, __LINE__);
        pThreadInfo->bBlock = BLOCK_INSENDMESSAGE;
        LEAVEMONITOR;

        BLOCK(pThreadInfo);
//        printf("Thread %d:Out of WAIT %s,%d\n", GetCurrentThreadId(),__FILE__, __LINE__);
        
        ENTERMONITOR;
//        printf("Thread %d:Enter monitor %s,%d\n", GetCurrentThreadId(),__FILE__, __LINE__);
    }

    return pMsg;
}

/*********************************************************************\
* Function	   MSQ_ReplyMessage
* Purpose      
* Params	   
* Return	 	   
* Remarks	   �ܳ��ں�����Ӧ��Send��Ϣ������Send�߳�
**********************************************************************/
BOOL MSQ_ReplyMessage(PSENDMSG pMsg)
{
    PTHREADINFO pThreadInfo;
    PWINOBJ pWin;
    LONG nRetValue;

    pWin = (PWINOBJ)WOT_GetObj((HANDLE)(pMsg->msg.hwnd), OBJ_WINDOW);
    ASSERT(pWin != NULL);

    /* ��֤��Ϣ���͵���ȷ�� */
    ASSERT(pWin->dwThreadId == WS_GetCurrentThreadId());

    LEAVEMONITOR;

    /* �ش������̵߳�Send��Ϣ */
    nRetValue = pWin->pfnWndProc((HWND)WOT_GetHandle((PWSOBJ)pWin), 
        pMsg->msg.message, pMsg->msg.wParam, pMsg->msg.lParam);

    ENTERMONITOR;

    /* Notify��Ϣ�������Ҫ�ͷ���Ϣ�ṹ��Ȼ��ֱ�ӷ��� */
    if (pMsg->type == SENDMSG_NOTIFY)
    {
        ReleaseMsgObj(pMsg, MSGOBJ_SEND);
        return TRUE;
    }

    /* ����Send��Ϣ */
    ASSERT(pMsg->type == SENDMSG_SEND);

    pMsg->type = SENDMSG_REPLY;
    pMsg->retval = nRetValue;

    /*ȡ�÷����̵߳�ThreadInfo*/
    if (THREAD_ISDESTROYED(pMsg->dwThreadId))
    {
        /* �ȴ��߳��Ѿ������ڣ��ͷ����Ӧ����Ϣ�ڵ� */
        ReleaseMsgObj(pMsg, MSGOBJ_SEND);
        return FALSE;
    }

    pThreadInfo = WS_GetThreadInfo(pMsg->dwThreadId);
    ASSERT(pThreadInfo != NULL);

    /* ��Ӧ����Ϣ�����ǵ�ǰ�ȴ�Ӧ���߳����ȴ���Ӧ����Ϣ�����ֻ��һ����ǡ�
    ** ���۵ȴ��߳��Ƿ��������������л��Ѳ�����Ȼ���ֱ�ӷ��ء�
    */
    ASSERT(pThreadInfo->SendMsgQue.ReplyMsgStack.pTop != NULL);
    if (pMsg != pThreadInfo->SendMsgQue.ReplyMsgStack.pTop)
        return TRUE;

    /* ��Ӧ����Ϣ���ǵȴ�Ӧ���߳����ڵȴ���Ӧ����Ϣ����˻�Ҫ���ѵȴ��̡߳�
    ** ��ʱ�ȴ��߳�Ҳ�п�����GetMessage�ϵȴ�,Ҳ���ܸ�����û��������������
    ** ��Ϣ���п�Ϊ�����оݣ���Ϊ�߳���ȫ���ܴ��ڴ������һ����Ϣ�Ĺ����� 
    */
    if (ISBLOCKINSEND(pThreadInfo))
    {
        pThreadInfo->bBlock = BLOCK_NO;
        WAKEUP(pThreadInfo);
    }

    return TRUE;
}

/*********************************************************************\
* Function	   MSQ_InsertSendMsg
* Purpose      
* Params	   
* Return	 	   
* Remarks	    �ܳ��ں�������Send���������һ����Ϣ������Ŀ���̡߳�
                �ɵ����߱�֤��������Ч�ԡ�
**********************************************************************/
BOOL MSQ_InsertSendMsg(PTHREADINFO pThreadInfo, PSENDMSG pMsg)
{
    PSENDMSGQUE pMsgQue;

    ASSERT(pThreadInfo != NULL && pMsg != NULL);
    ASSERT(!THREAD_ISDESTROYED(pThreadInfo->dwThreadId));

    pMsgQue = &pThreadInfo->SendMsgQue;
    ASSERT(pMsgQue != NULL);

    pMsg->pNext = NULL;

    if (pMsgQue->pFirst == NULL)
    {
        ASSERT(pMsgQue->pLast == NULL);
        pMsgQue->pFirst = pMsgQue->pLast = pMsg;
        /* ���п�ʱ����״̬��־ */
        pThreadInfo->dwState |= QS_SENDMESSAGE;
    }
    else
    {
        ASSERT(pMsgQue->pLast != NULL);
        pMsgQue->pLast->pNext = pMsg;
        pMsgQue->pLast = pMsg;
    }

        pThreadInfo->dwState |= QS_SENDMESSAGE;
    if (ISBLOCK(pThreadInfo))
    {
        pThreadInfo->bBlock = BLOCK_NO;
        WAKEUP(pThreadInfo);
    }

    return TRUE;
}

/*********************************************************************\
* Function	   MSQ_GetSendMsg
* Purpose      
* Params	   
* Return	 	   
* Remarks	    �ܳ��ں�����
**********************************************************************/
PSENDMSG MSQ_GetSendMsg(PTHREADINFO pThreadInfo, BOOL bDelete)
{
    PSENDMSGQUE pMsgQue;
    PSENDMSG pMsg;

    ASSERT(pThreadInfo != NULL);

    pMsgQue = &pThreadInfo->SendMsgQue;

    if (pMsgQue->pFirst == NULL)
    {
        ASSERT(pMsgQue->pLast == NULL);
        pThreadInfo->dwState &= ~QS_SENDMESSAGE;
        return NULL;
    }

    ASSERT(pMsgQue->pLast != NULL);

    pMsg = pMsgQue->pFirst;

    if (bDelete)
    {
        pMsgQue->pFirst = pMsgQue->pFirst->pNext;
        if (pMsgQue->pFirst == NULL)
        {
            pMsgQue->pLast = NULL;
            pThreadInfo->dwState &= ~QS_SENDMESSAGE;
        }
    }

    return pMsg;
}

/*********************************************************************\
* Function	   MSQ_DeleteSendMsg
* Purpose      
* Params	   
* Return	 	   
* Remarks	    �ܳ��ں�����ɾ�������н���pWin�����Send��Ϣ��
                ���ô��ڽ�Ҫ����ʱ���ã�����û���������Ϣ��
**********************************************************************/
BOOL MSQ_DeleteSendMsg(PWINOBJ pWin)
{
    PSENDMSG pMsg, pPrev;
    PSENDMSGQUE pMsgQue;
    PTHREADINFO pThreadInfo;

    ASSERT(pWin != NULL && WND_IsWindow(pWin));

    pThreadInfo = WS_GetThreadInfo(pWin->dwThreadId);
    ASSERT(pThreadInfo != NULL);

    pMsgQue = &pThreadInfo->SendMsgQue;

    if (pMsgQue->pFirst == NULL)
        return TRUE;

    ASSERT(pMsgQue->pLast != NULL);

    pMsg = pMsgQue->pFirst;

    /* ͷ�ڵ� */
    while (pMsg != NULL && pMsg->msg.hwnd == (HWND)WOT_GetHandle((PWSOBJ)pWin))
    {
        pPrev = pMsg;
        pMsg = pMsg->pNext;

        pThreadInfo = WS_GetThreadInfo(pPrev->dwThreadId);
        if (pThreadInfo == NULL)
        {
            /* ԭ���ĵȴ�Ӧ���߳��Ѿ������ڣ�������ﲻ��ҪӦ��
            ** ֻҪ�ͷ���Ϣ�ڵ㡣
            */
            ReleaseMsgObj(pPrev, MSGOBJ_SEND);
        }
        else
        {
            /* Ӧ��ÿ��Send��Ϣ������ֵΪ-1 */
            pPrev->retval = -1;
            pPrev->type = SENDMSG_REPLY;

            ASSERT(pThreadInfo->SendMsgQue.ReplyMsgStack.pTop != NULL);
            /* ���ǵȴ��߳����ڵȴ�����Ϣ������! */
            if (pPrev == pThreadInfo->SendMsgQue.ReplyMsgStack.pTop &&
                ISBLOCKINSEND(pThreadInfo))
            {
                pThreadInfo->bBlock = BLOCK_NO;
                WAKEUP(pThreadInfo);
            }
        }
    }
    
    pMsgQue->pFirst = pMsg;
    if (pMsgQue->pFirst == NULL)
    {
        pMsgQue->pLast = NULL;
        WS_GetThreadInfo(pWin->dwThreadId)->dwState &= ~QS_SENDMESSAGE;
        return TRUE;
    }

    pPrev = NULL;
    while (pMsg->pNext != NULL)
    {
        pPrev = pMsg;
        pMsg = pMsg->pNext;
        if (pMsg->msg.hwnd == (HWND)WOT_GetHandle((PWSOBJ)pWin))
        {
            pPrev->pNext = pMsg->pNext;

            pThreadInfo = WS_GetThreadInfo(pMsg->dwThreadId);
            if (pThreadInfo == NULL)
            {
                ReleaseMsgObj(pMsg, MSGOBJ_SEND);
            }
            else
            {
                /* Ӧ��ÿ��Send��Ϣ������ֵΪ-1 */
                pMsg->retval = -1;
                pMsg->type = SENDMSG_REPLY;
                
                ASSERT(pThreadInfo->SendMsgQue.ReplyMsgStack.pTop != NULL);
                /* ���ǵȴ��߳����ڵȴ�����Ϣ������! */
                if (pMsg == pThreadInfo->SendMsgQue.ReplyMsgStack.pTop &&
                    ISBLOCKINSEND(pThreadInfo))
                {
                    pThreadInfo->bBlock = BLOCK_NO;
                    WAKEUP(pThreadInfo);
                }
            }
            
            pMsg = pPrev;
        }
    }

    pMsgQue->pLast = pMsg;

    return TRUE;
}

/*********************************************************************\
* Function	   MSQ_DestroySendMsgQue
* Purpose      
* Params	   
* Return	 	   
* Remarks	   �ܳ��ں�����
**********************************************************************/
BOOL MSQ_EmptySendMsgQue(PTHREADINFO pThreadInfo)
{
    PSENDMSG pMsg, pPrev;
    PSENDMSGQUE pMsgQue;

    ASSERT(pThreadInfo != NULL);

    pMsgQue = &pThreadInfo->SendMsgQue;
    if (pMsgQue->pFirst == NULL)
        return TRUE;

    ASSERT(pMsgQue->pLast != NULL);

    pMsg = pMsgQue->pFirst;

    /* ͷ�ڵ� */
    while (pMsg != NULL)
    {
        pPrev = pMsg;
        pMsg = pMsg->pNext;

        pThreadInfo = WS_GetThreadInfo(pPrev->dwThreadId);
        if (pThreadInfo == NULL)
        {
            /* ԭ���ĵȴ�Ӧ���߳��Ѿ������ڣ�������ﲻ��ҪӦ��
            ** ֻҪ�ͷ���Ϣ�ڵ㡣
            */
            ReleaseMsgObj(pPrev, MSGOBJ_SEND);
        }
        else
        {
            /* Ӧ��ÿ��Send��Ϣ������ֵΪ-1 */
            pPrev->retval = -1;
            pPrev->type = SENDMSG_REPLY;

            ASSERT(pThreadInfo->SendMsgQue.ReplyMsgStack.pTop != NULL);
            /* ���ǵȴ��߳����ڵȴ�����Ϣ������! */
            if (pPrev == pThreadInfo->SendMsgQue.ReplyMsgStack.pTop &&
                ISBLOCKINSEND(pThreadInfo))
            {
                pThreadInfo->bBlock = BLOCK_NO;
                WAKEUP(pThreadInfo);
            }
        }
    }
    
    pMsgQue->pFirst = pMsgQue->pLast = NULL;
    pThreadInfo->dwState &= ~QS_SENDMESSAGE;

    return TRUE;
}
/*********************************************************************\
* Function	   MSQ_InsertPostMsg
* Purpose      
* Params	   
* Return	 	   
* Remarks	    �ܳ��ں�����
**********************************************************************/
BOOL MSQ_InsertPostMsg(PTHREADINFO pThreadInfo, PPOSTMSG pMsg)
{
    PPOSTMSGQUE pMsgQue;

    ASSERT(pThreadInfo != NULL && pMsg != NULL);

    pMsgQue = &pThreadInfo->PostMsgQue;
    pMsg->pNext = NULL;

    if (pMsgQue->pFirst == NULL)
    {
        ASSERT(pMsgQue->pLast == NULL);
        pMsgQue->pFirst = pMsgQue->pLast = pMsg;
        pThreadInfo->dwState |= QS_POSTMESSAGE;

    }
    else
    {
        ASSERT(pMsgQue->pLast != NULL);
        pMsgQue->pLast->pNext = pMsg;
        pMsgQue->pLast = pMsg;
        
    }

    if (ISBLOCKINGET(pThreadInfo))
    {
        pThreadInfo->bBlock = BLOCK_NO;
        WAKEUP(pThreadInfo);
    }

    return TRUE;
}

/*********************************************************************\
* Function	   MSQ_GetPostMsg
* Purpose      
* Params	   
* Return	 	   
* Remarks	    �ܳ��ں�����
**********************************************************************/
PPOSTMSG MSQ_GetPostMsg(PTHREADINFO pThreadInfo, BOOL bDelete)
{
    PPOSTMSGQUE pMsgQue;
    PPOSTMSG pMsg;

    ASSERT(pThreadInfo != NULL);

    pMsgQue = &pThreadInfo->PostMsgQue;

    if (pMsgQue->pFirst == NULL)
    {
        ASSERT(pMsgQue->pLast == NULL);
        pThreadInfo->dwState &= ~QS_POSTMESSAGE;
        return NULL;
    }

    ASSERT(pMsgQue->pLast != NULL);

    pMsg = pMsgQue->pFirst;

    if (bDelete)
    {
        pMsgQue->pFirst = pMsgQue->pFirst->pNext;
        if (pMsgQue->pFirst == NULL)
        {
            pMsgQue->pLast = NULL;
            pThreadInfo->dwState &= ~QS_POSTMESSAGE;
        }
    }

    return pMsg;
}

/*********************************************************************\
* Function	   MSQ_DeletePostMsg
* Purpose      
* Params	   
* Return	 	   
* Remarks	    �ܳ��ں�����
**********************************************************************/
BOOL MSQ_DeletePostMsg(PWINOBJ pWin)
{
    PPOSTMSG pMsg, pPrev;
    PPOSTMSGQUE pMsgQue;
    PTHREADINFO pThreadInfo;

    ASSERT(pWin != NULL);

    pThreadInfo = WS_GetThreadInfo(pWin->dwThreadId);
    pMsgQue = &pThreadInfo->PostMsgQue;

    if (pMsgQue->pFirst == NULL)
        return TRUE;

    ASSERT(pMsgQue->pLast != NULL);

    pMsg = pMsgQue->pFirst;

    /* ͷ�ڵ� */
    while (pMsg != NULL && pMsg->msg.hwnd == (HWND)WOT_GetHandle((PWSOBJ)pWin))
    {
        pPrev = pMsg;
        pMsg = pMsg->pNext;

        ReleaseMsgObj(pPrev, MSGOBJ_POST);

    }

    pMsgQue->pFirst = pMsg;
    if (pMsgQue->pFirst == NULL)
    {
        pMsgQue->pLast = NULL;
        pThreadInfo->dwState &= ~QS_POSTMESSAGE;
        return TRUE;
    }

    pPrev = NULL;
    while (pMsg->pNext != NULL)
    {
        pPrev = pMsg;
        pMsg = pMsg->pNext;
        if (pMsg->msg.hwnd == (HWND)WOT_GetHandle((PWSOBJ)pWin))
        {
            pPrev->pNext = pMsg->pNext;

            ReleaseMsgObj(pMsg, MSGOBJ_POST);
            
            pMsg = pPrev;
        }
    }

    pMsgQue->pLast = pMsg;

    return TRUE;
}

/*********************************************************************\
* Function	   MSQ_EmptyPostMsgQue
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MSQ_EmptyPostMsgQue(PTHREADINFO pThreadInfo)
{
    PPOSTMSG pMsg, pPrev;
    PPOSTMSGQUE pMsgQue;

    ASSERT(pThreadInfo != NULL);

    pMsgQue = &pThreadInfo->PostMsgQue;

    if (pMsgQue->pFirst == NULL)
        return TRUE;

    ASSERT(pMsgQue->pLast != NULL);

    pMsg = pMsgQue->pFirst;

    while (pMsg != NULL)
    {
        pPrev = pMsg;
        pMsg = pMsg->pNext;

        ReleaseMsgObj(pPrev, MSGOBJ_POST);

    }

    pMsgQue->pFirst = pMsgQue->pLast = NULL;
    pThreadInfo->dwState &= ~QS_POSTMESSAGE;

    return TRUE;
}

/*********************************************************************\
* Function	   MSQ_InsertReplyMsg
* Purpose      
* Params	   
* Return	 	   
* Remarks	    �ܳ��ں��������º����Ǵ�����Send��Ϣ�󣬽�����Ϣ�ŵ���
                �̵�Ӧ��ջ���ȴ�Ӧ��
**********************************************************************/
BOOL MSQ_InsertReplyMsg(PTHREADINFO pThreadInfo, PREPLYMSG pMsg)
{
    PREPLYMSGSTACK pMsgStack;
    
    ASSERT(pThreadInfo != NULL && pMsg != NULL);

    pMsgStack = &pThreadInfo->SendMsgQue.ReplyMsgStack;

    if (pMsgStack->pTop == NULL)
    {
        pMsgStack->pTop  = pMsg;
        pMsg->pReplyNext = NULL;
    }
    else
    {
        pMsg->pReplyNext = pMsgStack->pTop;
        pMsgStack->pTop = pMsg;
    }

    return TRUE;
}

/*********************************************************************\
* Function	   MSQ_GetReplyMsg
* Purpose      
* Params	   
* Return	 	   
* Remarks	    �ܳ��ں������ú�����֤Ҫ��Ӧ��ջ��ȡ�����ڵȴ���Ӧ��
                ��Ϣ�������Ϣ��Ȼλ��ջ��������Ϊ����͵���Ϣ�ڱ�
                Ӧ��ջ�Ķ�������Ҫ����Ӧ��
**********************************************************************/
PSENDMSG MSQ_GetReplyMsg(PTHREADINFO pThreadInfo, BOOL bDelete)
{
    PREPLYMSG pMsg;
    PREPLYMSGSTACK pMsgStack;

    ASSERT(pThreadInfo != NULL);

    pMsgStack = &pThreadInfo->SendMsgQue.ReplyMsgStack;

    if (pMsgStack->pTop == NULL)
        return NULL;

    pMsg = pMsgStack->pTop;

    /* ���Ӧ����Ϣ��û�б�Ӧ�����ܵõ���Ӧ����Ϣ */
    if (pMsg->type != SENDMSG_REPLY)
        return NULL;

    if (bDelete)
    {
        pMsgStack->pTop = pMsgStack->pTop->pReplyNext;
    }

    return (PSENDMSG)pMsg;
}

/*********************************************************************\
* Function	   MSQ_InsertPaintMsg
* Purpose      
* Params	   
* Return	 	   
* Remarks	    �ܳ��ں���������Ӧ���ڷ���Paint��Ϣ����ΪPaint��Ϣ�ṹ
                һ�£����ڸú����ڲ����졣
**********************************************************************/
BOOL MSQ_InsertPaintMsg(PWINOBJ pWin, BOOL bNCPaint)
{
    PPAINTMSG pMsg;
    PPAINTMSGQUE pMsgQue;
    PTHREADINFO pThreadInfo;

    ASSERT(pWin != NULL && WND_IsWindow(pWin));

    pThreadInfo = WS_GetThreadInfo(pWin->dwThreadId);
    ASSERT(pThreadInfo != NULL);

    pMsg = GetMsgObj(MSGOBJ_PAINT);
    if (pMsg == NULL)
    {
        SetLastError(1);
        return FALSE;
    }

    pMsg->msg.hwnd = (HWND)WOT_GetHandle((PWSOBJ)pWin);
    pMsg->msg.message = bNCPaint ? WM_NCPAINT : WM_PAINT;
    pMsg->msg.wParam = 0;
    pMsg->msg.lParam = 0;
    pMsg->dwThreadId = WS_GetCurrentThreadId();
    pMsg->pNext = NULL;

    //pMsgQue = &PaintMsgQue;
    pMsgQue = &pThreadInfo->PaintMsgQue;
    ASSERT(pMsgQue != NULL);

    if (pMsgQue->pFirst == NULL)
    {
        ASSERT(pMsgQue->pLast == NULL);
        pMsgQue->pFirst = pMsgQue->pLast = pMsg;
        pThreadInfo->dwState |= QS_PAINT;
    }
    else
    {
        ASSERT(pMsgQue->pLast != NULL);
        pMsgQue->pLast->pNext = pMsg;
        pMsgQue->pLast = pMsg;
    }

    if (ISBLOCKINGET(pThreadInfo))
    {
        pThreadInfo->bBlock = BLOCK_NO;
        WAKEUP(pThreadInfo);
    }

    return TRUE;
}

/*********************************************************************\
* Function	   MSQ_GetPaintMsg
* Purpose      
* Params	   
* Return	 	   
* Remarks	    �ܳ��ں�����
**********************************************************************/
PPAINTMSG MSQ_GetPaintMsg(PTHREADINFO pThreadInfo, BOOL bDelete)
{
    PPAINTMSGQUE pMsgQue;
    PPAINTMSG pMsg;

    pMsgQue = &pThreadInfo->PaintMsgQue;

    if (pMsgQue->pFirst == NULL)
    {
        ASSERT(pMsgQue->pLast == NULL);
        pThreadInfo->dwState &= ~QS_PAINT;
        return NULL;
    }

    ASSERT(pMsgQue->pLast != NULL);

    pMsg = pMsgQue->pFirst;

    if (bDelete)
    {
        pMsgQue->pFirst = pMsgQue->pFirst->pNext;
        if (pMsgQue->pFirst == NULL)
        {
            pMsgQue->pLast = NULL;
            pThreadInfo->dwState &= ~QS_PAINT;
        }
    }

    return pMsg;
}

/*********************************************************************\
* Function	   MSQ_DeletePaintMsg
* Purpose      
* Params	   
* Return	 	   
* Remarks	    �ܳ��ں��������ڴ��ڼ�����¾���ʱ�����á�
                WND_InvalidateRect����Paint��Ϣ�����Ŷӣ������п���Ҫ
                ɾ��ĳЩ�Ѿ���ӵ�������Ч����Ϣ�������Ч�ʡ�
**********************************************************************/
BOOL MSQ_DeletePaintMsg(PWINOBJ pWin)
{
    PPAINTMSG pMsg, pPrev;
    PPAINTMSGQUE pMsgQue;
    PTHREADINFO pThreadInfo;

    ASSERT(pWin != NULL && WND_IsWindow(pWin));

    pThreadInfo = WS_GetThreadInfo(pWin->dwThreadId);
    pMsgQue = &pThreadInfo->PaintMsgQue;

    if (pMsgQue->pFirst == NULL)
    {
        ASSERT((pThreadInfo->dwState & QS_PAINT) == 0);
        return TRUE;
    }

    ASSERT(pMsgQue->pLast != NULL);

    pMsg = pMsgQue->pFirst;

    /* ͷ�ڵ� */
    while (pMsg != NULL && pMsg->msg.hwnd == (HWND)WOT_GetHandle((PWSOBJ)pWin))
    {
        pPrev = pMsg;
        pMsg = pMsg->pNext;

        ReleaseMsgObj(pPrev, MSGOBJ_PAINT);
    }

    pMsgQue->pFirst = pMsg;
    if (pMsgQue->pFirst == NULL)
    {
        pMsgQue->pLast = NULL;
        pThreadInfo->dwState &= ~QS_PAINT;
        return TRUE;
    }

    pPrev = NULL;
    while (pMsg->pNext != NULL)
    {
        pPrev = pMsg;
        pMsg = pMsg->pNext;
        if (pMsg->msg.hwnd == (HWND)WOT_GetHandle((PWSOBJ)pWin))
        {
            pPrev->pNext = pMsg->pNext;

            ReleaseMsgObj(pMsg, MSGOBJ_PAINT);

            pMsg = pPrev;
        }
    }

    pMsgQue->pLast = pMsg;

    return TRUE;
}

/*********************************************************************\
* Function	   MSQ_EmptyPaintMsgQue
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MSQ_EmptyPaintMsgQue(PTHREADINFO pThreadInfo)
{
    PPAINTMSG pMsg, pPrev;
    PPAINTMSGQUE pMsgQue;

    ASSERT(pThreadInfo != NULL);

    pMsgQue = &pThreadInfo->PaintMsgQue;

    if (pMsgQue->pFirst == NULL)
    {
        ASSERT((pThreadInfo->dwState & QS_PAINT) == 0);
        return TRUE;
    }

    ASSERT(pMsgQue->pLast != NULL);

    pMsg = pMsgQue->pFirst;

    /* ͷ�ڵ� */
    while (pMsg != NULL)
    {
        pPrev = pMsg;
        pMsg = pMsg->pNext;

        ReleaseMsgObj(pPrev, MSGOBJ_PAINT);
    }

    pMsgQue->pFirst = pMsgQue->pLast = NULL;
    pThreadInfo->dwState &= ~QS_PAINT;
    return TRUE;
}

#define MAX_SENDMSG     10
static PSENDMSG SendMsgPool, SendMsgPoolHead, SendMsgPoolTail;

#define MAX_POSTMSG     10
static PPOSTMSG PostMsgPool, PostMsgPoolHead, PostMsgPoolTail;

#define MAX_PAINTMSG    10
static PPAINTMSG PaintMsgPool, PaintMsgPoolHead, PaintMsgPoolTail;
/*********************************************************************\
* Function	   MessagePoolInit
* Purpose      
* Params	   
* Return	 	   
* Remarks	   ����ϵͳ��ʼ������
**********************************************************************/
BOOL MessagePoolInit(void)
{
    if (!InitSendMsgPool())
        return FALSE;

    if (!InitPostMsgPool())
    {
        DestroySendMsgPool();
        return FALSE;
    }

    if (!InitPaintMsgPool())
    {
        DestroySendMsgPool();
        DestroyPostMsgPool();
        return FALSE;
    }

    return TRUE;
}

void MessagePoolExit(void)
{
    DestroyPaintMsgPool();
    DestroySendMsgPool();
    DestroyPostMsgPool();
}

/*********************************************************************\
* Function	   GetMsgObj
* Purpose      
* Params	   
* Return	 	   
* Remarks	   �ܳ��ں���
**********************************************************************/
void* GetMsgObj(int type)
{
    switch (type)
    {
    case MSGOBJ_SEND:

        return GetSendMsgObjFromPool();

    case MSGOBJ_POST:

        return GetPostMsgObjFromPool();

    case MSGOBJ_PAINT:

        return GetPaintMsgObjFromPool();

    default:

        return NULL;
    }
}

/*********************************************************************\
* Function	   ReleaseMsgObj
* Purpose      
* Params	   
* Return	 	   
* Remarks	   �ܳ��ں���
**********************************************************************/
void ReleaseMsgObj(void* pMsgObj, int type)
{
    switch (type)
    {
    case MSGOBJ_SEND:

        ReleaseSendMsgObjToPool((PSENDMSG)pMsgObj);
        break;

    case MSGOBJ_POST:

        ReleasePostMsgObjToPool((PPOSTMSG)pMsgObj);
        break;

    case MSGOBJ_PAINT:

        ReleasePaintMsgObjToPool((PPAINTMSG)pMsgObj);
        break;

    default:

        return;
    }

    return;
}


/*
** Internal function definition. Message pool management.
*/
/*********************************************************************\
* Function	   InitSendMsgPool
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL InitSendMsgPool()
{
    int i;

    SendMsgPool = (PSENDMSG)MemAlloc(sizeof(SENDMSG) * MAX_SENDMSG);
    if (SendMsgPool == NULL)
    {
        SetLastError(1);
        return FALSE;
    }

    SendMsgPoolHead = SendMsgPool;
    for (i = 0; i < MAX_SENDMSG - 1; i ++)
    {
        SendMsgPoolHead[i].pNext = &SendMsgPoolHead[i + 1];
    }
    SendMsgPoolHead[i].pNext = NULL;
    SendMsgPoolTail = &SendMsgPoolHead[i];

    return TRUE;
}

/*********************************************************************\
* Function	   DestroySendMsgPool
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DestroySendMsgPool()
{
    MemFree(SendMsgPool);
}

/*********************************************************************\
* Function	   InitPostMsgPool
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL InitPostMsgPool()
{
    int i;

    PostMsgPool = (PPOSTMSG)MemAlloc(sizeof(POSTMSG) * MAX_POSTMSG);
    if (PostMsgPool == NULL)
    {
        MemFree(SendMsgPool);
        SetLastError(1);
        return FALSE;
    }

    PostMsgPoolHead = PostMsgPool;
    for (i = 0; i < MAX_POSTMSG - 1 ; i ++)
    {
        PostMsgPoolHead[i].pNext = &PostMsgPoolHead[i + 1];
    }
    PostMsgPoolHead[i].pNext = NULL;
    PostMsgPoolTail = &PostMsgPoolHead[i];

    return TRUE;
}

/*********************************************************************\
* Function	   DestroyPostMsgPool
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DestroyPostMsgPool()
{
    MemFree(PostMsgPool);
}

/*********************************************************************\
* Function	   InitPaintMsgPool
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL InitPaintMsgPool()
{
    int i;

    PaintMsgPool = (PPAINTMSG)MemAlloc(sizeof(PAINTMSG) * MAX_PAINTMSG);
    if (PaintMsgPool == NULL)
    {
        SetLastError(1);
        return FALSE;
    }

    PaintMsgPoolHead = PaintMsgPool;
    for (i = 0; i < MAX_PAINTMSG - 1; i ++)
    {
        PaintMsgPoolHead[i].pNext = &PaintMsgPoolHead[i + 1];
    }
    PaintMsgPoolHead[i].pNext = NULL;
    PaintMsgPoolTail = &PaintMsgPoolHead[i];

    return TRUE;
}

/*********************************************************************\
* Function	   DestroyPaintMsgPool
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DestroyPaintMsgPool()
{
    MemFree(PaintMsgPool);
}

/*********************************************************************\
* Function	   GetSendMsgObjFromPool
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static PSENDMSG GetSendMsgObjFromPool(void)
{
    PSENDMSG pSendMsg;
    if (SendMsgPoolHead != SendMsgPoolTail)
    {
        pSendMsg = SendMsgPoolHead;
        SendMsgPoolHead = SendMsgPoolHead->pNext;
    }
    else
    {
        pSendMsg = (PSENDMSG)MemAlloc(sizeof(SENDMSG));
    }

    return pSendMsg;
}

/*********************************************************************\
* Function	   ReleaseSendMsgObjToPool
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ReleaseSendMsgObjToPool(PSENDMSG pSendMsg)
{
    if (pSendMsg < SendMsgPool || pSendMsg > (SendMsgPool + MAX_SENDMSG))
    {
        /*����Ϣ�Ƕ�̬����õ���*/
        MemFree(pSendMsg);
        return;
    }

    SendMsgPoolTail->pNext = pSendMsg;
    SendMsgPoolTail = pSendMsg;
    SendMsgPoolTail->pNext = NULL;
}

/*********************************************************************\
* Function	   GetPostMsgObjFromPool
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static PPOSTMSG GetPostMsgObjFromPool(void)
{
    PPOSTMSG pPostMsg;
    if (PostMsgPoolHead != PostMsgPoolTail)
    {
        pPostMsg = PostMsgPoolHead;
        PostMsgPoolHead = PostMsgPoolHead->pNext;
        return pPostMsg;
    }
    
    pPostMsg = (PPOSTMSG)MemAlloc(sizeof(POSTMSG));

    return pPostMsg;
}

/*********************************************************************\
* Function	   ReleasePostMsgObjToPool
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ReleasePostMsgObjToPool(PPOSTMSG pPostMsg)
{
    if (pPostMsg < PostMsgPool || pPostMsg> (PostMsgPool + MAX_POSTMSG))
    {
        /*����Ϣ�Ƕ�̬����õ���*/
        MemFree(pPostMsg);
        return;
    }

    PostMsgPoolTail->pNext = pPostMsg;
    PostMsgPoolTail = pPostMsg;
    PostMsgPoolTail->pNext = NULL;
}

/*********************************************************************\
* Function	   GetPaintMsgObjFromPool
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static PPAINTMSG GetPaintMsgObjFromPool(void)
{
    PPAINTMSG pPaintMsg;
    if (PaintMsgPoolHead != PaintMsgPoolTail)
    {
        pPaintMsg = PaintMsgPoolHead;
        PaintMsgPoolHead = PaintMsgPoolHead->pNext;
    }
    else
    {
        pPaintMsg = (PPAINTMSG)MemAlloc(sizeof(PAINTMSG));
    }

    return pPaintMsg;
}

/*********************************************************************\
* Function	   ReleasePaintMsgObjToPool
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ReleasePaintMsgObjToPool(PPAINTMSG pPaintMsg)
{
    if (pPaintMsg < PaintMsgPool || pPaintMsg > (PaintMsgPool + MAX_PAINTMSG))
    {
        /*����Ϣ�Ƕ�̬����õ���*/
        MemFree(pPaintMsg);
        return;
    }

    PaintMsgPoolTail->pNext = pPaintMsg;
    PaintMsgPoolTail = pPaintMsg;
    PaintMsgPoolTail->pNext = NULL;

    return;
}

