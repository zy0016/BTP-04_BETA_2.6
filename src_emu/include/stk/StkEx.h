/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : StkEx.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _STK_EX_H_
#define _STK_EX_H_

BOOL IfStkMainMenu(void);
//����TRUE��ʾ��STK���˵����棬����ص��������棻
//����FALSE��ʾ����STK���˵����棬��ֹ�ص��������棻

BOOL IfStkInitSucc(void);
//����STK�Ƿ��ʼ����ϣ�����ĳ�ʼ��ָ����ȡ���˵���Ϣ�������Ӧ��
//�ú�������SIM����֧��STK���ܵ�ǰ���¿��Ե��á�
//���������������STK��ʼ�����ǰ����ʾSTKͼ�꣬Ҳ�����ڳɹ���ʼ��֮����SHOW��

void STK_GetMainText(char* pMainText);
//����STK���������⣬�����������ʹ�á�

void STK_GetIdleModeText(char* pIdleModeText);
//���յ�URC: 40ʱ�����õ����ִ���

BOOL CreateStkHideWindow(void);
//����STK���ش��ڡ�

#endif // _STK_EX_H_
