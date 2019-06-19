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
��Ϣ�ַ�������
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
 ��Ϣ���й�������������нڵ����Ӻ�ɾ����
 1���Բ������������Ŀ���߳���Ϣ״̬�֣����ж�Ŀ������Ƿ�Ϊ�գ�
    �Ա���л��Ѳ���
 2����ȡ��Ϣ�������Ӷ�����ժ����Ϣ���ж϶����Ƿ�գ���ȡ���߳���Ϣ״̬��
 3���ڵ�ɾ��������ɾ�������й���ĳ�����ڵ���Ϣ������Send��Ϣ����Ӧ�ö�
    ���е���Ϣ����Ӧ��1������ȴ��߳��Ѿ������ڣ���ֱ���ͷŸ���Ϣ�ڵ㣻
    ����Reply��ϢӦ�ý���Щ��Ϣ��������Ϊ��Щ��Ϣ����Ӧ��Ŀ���߳��д��б�
    �ݣ�����Щ�̻߳ش���Щ��Ϣʱ����������߳��Ծ������ڣ��ͻ��ͷ���Щ�ڵ㡣
**********************************************************************/
/* 
    Send��Ϣ����
*/
BOOL        MSQ_InsertSendMsg(PTHREADINFO pThreadInfo, PSENDMSG pMsg);
PSENDMSG    MSQ_GetSendMsg(PTHREADINFO pThreadInfo, BOOL bDelete);
BOOL        MSQ_DeleteSendMsg(PWINOBJ pWin);
BOOL        MSQ_EmptySendMsgQue(PTHREADINFO pThreadInfo);

/* 
    Post��Ϣ����
*/
BOOL        MSQ_InsertPostMsg(PTHREADINFO pThreadInfo, PPOSTMSG pMsg);
PPOSTMSG    MSQ_GetPostMsg(PTHREADINFO pThreadInfo, BOOL bDelete);
BOOL        MSQ_DeletePostMsg(PWINOBJ pWin);
BOOL        MSQ_EmptyPostMsgQue(PTHREADINFO pThreadInfo);

/* 
    Reply��Ϣ����
*/
BOOL        MSQ_InsertReplyMsg(PTHREADINFO pThreadInfo, PREPLYMSG pMsg);
/* Ӧ����Ϣ�ǲ��ܱ�ɾ���ģ���Ϊ������߳�Ҫ���������Ϣ 
** ���Կ��ǽ�Ӧ����Ϣ��Ϊ��������Ϣ���з��ͣ�����ʹ��ԭSend�̵߳�Ӧ����С�
** ������ά��Ӧ��˳��Ͳ��ô�����
*/
//BOOL        MSQ_DeleteReplyMsg(PWINOBJ pWin);
PSENDMSG    MSQ_GetReplyMsg(PTHREADINFO pThreadInfo, BOOL bDelete);
BOOL        MSQ_EmptyReplyMsgQue(PTHREADINFO pThreadInfo);

/* 
    Paint��Ϣ����
*/
BOOL        MSQ_InsertPaintMsg(PWINOBJ pWin, BOOL bNCPaint);
PPAINTMSG   MSQ_GetPaintMsg(PTHREADINFO pThreadInfo, BOOL bDelete);
BOOL        MSQ_DeletePaintMsg(PWINOBJ pWin);
BOOL        MSQ_EmptyPaintMsgQue(PTHREADINFO pThreadInfo);

/*
**    ��Ϣ�س�ʼ����������Ϣ�ڵ�Ĺ�����
*/
BOOL        MessagePoolInit(void);

/* Message object type */
#define MSGOBJ_SEND     1
#define MSGOBJ_POST     2
#define MSGOBJ_PAINT    3
void*       GetMsgObj(int type);
void        ReleaseMsgObj(void* pMsgObj, int type);

#endif //__MSGQUEUE_H
