/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Head file of msgqueue.c
 *            
\**************************************************************************/

#ifndef __MSGQUEUE_H
#define __MSGQUEUE_H

/***********************************************************************
消息分发处理函数
************************************************************************/
LONG        MSQ_SendMessage(PWINOBJ pWin, UINT wMsgCmd, WPARAM wParam, 
                           LPARAM lParam);
BOOL        MSQ_SendNotifyMessage(PWINOBJ pWin, UINT wMsgCmd, WPARAM wParam, 
                           LPARAM lParam);
BOOL        MSQ_PostMessage(PWINOBJ pWin, UINT wMsgCmd, WPARAM wParam, 
                           LPARAM lParam);
BOOL        MSQ_PostThreadMessage(DWORD dwThreadId, UINT uMsgCmd, 
                                  WPARAM wParam, LPARAM lParam);
PSENDMSG    MSQ_WaitReplyMessage(void);
BOOL        MSQ_ReplyMessage(PSENDMSG pMsg);

/**********************************************************************
 消息队列管理函数，处理队列节点的添加和删除。
 1、对插入操作，设置目标线程消息状态字，并判断目标队列是否为空，
    以便进行唤醒操作
 2、对取消息操作，从队列中摘下消息，判断队列是否空，以取消线程消息状态字
 3、节点删除操作，删掉队列中关于某个窗口的消息，对于Send消息队列应该对
    所有的消息进行应答－1，如果等待线程已经不存在，就直接释放该消息节点；
    对于Reply消息应该将这些消息保留，因为这些消息在相应的目标线程中存有备
    份，当这些线程回答这些消息时，如果发现线程以经不存在，就会释放这些节点。
**********************************************************************/
/* 
    Send消息队列
*/
BOOL        MSQ_InsertSendMsg(PTHREADINFO pThreadInfo, PSENDMSG pMsg);
PSENDMSG    MSQ_GetSendMsg(PTHREADINFO pThreadInfo, BOOL bDelete);
BOOL        MSQ_DeleteSendMsg(PWINOBJ pWin);
BOOL        MSQ_EmptySendMsgQue(PTHREADINFO pThreadInfo);

/* 
    Post消息队列
*/
BOOL        MSQ_InsertPostMsg(PTHREADINFO pThreadInfo, PPOSTMSG pMsg);
PPOSTMSG    MSQ_GetPostMsg(PTHREADINFO pThreadInfo, BOOL bDelete);
BOOL        MSQ_DeletePostMsg(PWINOBJ pWin);
BOOL        MSQ_EmptyPostMsgQue(PTHREADINFO pThreadInfo);

/* 
    Reply消息队列
*/
BOOL        MSQ_InsertReplyMsg(PTHREADINFO pThreadInfo, PREPLYMSG pMsg);
/* 应答消息是不能被删除的，因为另外的线程要处理这个消息 
** 可以考虑将应答消息作为单独的消息进行发送，而不使用原Send线程的应答队列。
** 但这样维护应答顺序就不好处理了
*/
//BOOL        MSQ_DeleteReplyMsg(PWINOBJ pWin);
PSENDMSG    MSQ_GetReplyMsg(PTHREADINFO pThreadInfo, BOOL bDelete);
BOOL        MSQ_EmptyReplyMsgQue(PTHREADINFO pThreadInfo);

/* 
    Paint消息队列
*/
BOOL        MSQ_InsertPaintMsg(PWINOBJ pWin, BOOL bNCPaint);
PPAINTMSG   MSQ_GetPaintMsg(PTHREADINFO pThreadInfo, BOOL bDelete);
BOOL        MSQ_DeletePaintMsg(PWINOBJ pWin);
BOOL        MSQ_EmptyPaintMsgQue(PTHREADINFO pThreadInfo);

/*
**    消息池初始化函数、消息节点的管理函数
*/
BOOL        MessagePoolInit(void);

/* Message object type */
#define MSGOBJ_SEND     1
#define MSGOBJ_POST     2
#define MSGOBJ_PAINT    3
void*       GetMsgObj(int type);
void        ReleaseMsgObj(void* pMsgObj, int type);

#endif //__MSGQUEUE_H
