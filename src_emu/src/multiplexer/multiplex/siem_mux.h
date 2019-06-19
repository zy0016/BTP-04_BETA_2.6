/**************************************************************************\
*
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
*
* Model   : CMUX
*
* Purpose : siemens multiplexer protocol
*  
* Author  : 
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
\**************************************************************************/

/**************************************************************************\
*
*	��������: Digital cellular telecommunications system
*			  Terminal Equipment to Mobile Station multiplexer protocol 
*			  (3GPP TS 07.10 version 7.2.0 Release 1998) 
*			  Implementation on Siemens wireless modules TC35/MC35
*
*	�ļ�����: multiplx.h
*
*	��������: 
*
*	������: impementation of Multiplexer Protocol (MP) on TC35/MC35
*
*	�������ڣ�2002.04.22
*
\**************************************************************************/

#ifndef _SIEMENS_MUX_H_
#define _SIEMENS_MUX_H_

#include "DI_Uart.h"
#include "device.h"

/**************************************************************************\
*
*	��������
*
\**************************************************************************/

/* ���庯������ֵ */
#define CMUX_SUCCESS			0		/* �ɹ� */
#define CMUX_FAILURE			1		/* ʧ�� */
#define CMUX_ASYNC				2		/* �첽ִ�� */

/* ����ͨ������ */
#define CMUX_CHANNEL1			"MUX-SIEMENS-DLC1"	/* DATA & AT Channel*/
#define CMUX_CHANNEL2			"MUX-SIEMENS-DLC2"	/* AT Channel */
#define CMUX_CHANNEL3			"MUX-SIEMENS-DLC3"	/* AT Channel */

/* ����ص��¼� */
#define CMUX_DATAIN				EV_DATAIN		/* �������������ɶ� */
#define CMUX_DATAOUT			EV_DATAOUT		/* ����������գ���д */
#define CMUX_MODEM				EV_MODEM		/* ͨ���ر� */

/* ��·�ź� */
#define WMUX_RLSD				MS_RLSD			/* Data Carrier */
#define WMUX_RING				MS_RING			/* Ring signal */

/* �������ѡ�� */
#define CMUX_OP_CALLBACK		IO_SETCALLBACK		/* �ص����� */
#define CMUX_OP_DUMPRXBUF		IO_DUMPBUF			/* ���ͨ�����ջ����� */
#define CMUX_OP_DATABROKEN		IO_UART_SETMODEMCTL	/* �ж����ݴ��� */
#define CMUX_OP_LINESTATUS		IO_UART_GETMODEMSTATUS /* �����·״̬ */

/**************************************************************************\
*
*	���ݽṹ���������Ͷ���
*
\**************************************************************************/

/* define any data structure here */
typedef int  (*IOCOM)(unsigned char* data, int datalen);

/**************************************************************************\
*
*	��������
*
\**************************************************************************/

/* ����ӿں��� */

/* ϵͳ��ʼ������ */
int CMUX_Initial(void);

/* �û��ӿں��� */
int CMUX_Startup(IOCOM fpread, IOCOM fpwrite);
int CMUX_Closedown(void);
int CMUX_OpenChannel(char* name);
int CMUX_CloseChannel(int handle);
int CMUX_WriteChannel(int handle, unsigned char* data, int datalen);
int CMUX_ReadChannel(int handle, unsigned char* buf, int bufsize);
int CMUX_IoControl(int handle, unsigned opcode, int value, int size);

/* ���ݻص����� */
int CMUX_OnDatain(void);
int CMUX_OnDataout(void);

/**************************** End Of Head File ****************************/
#endif	/* _SIEMENS_MUX_H_ */
