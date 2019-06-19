/**************************************************************************\
*
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
*
* Model   :	WMUX
*
* Purpose : wavecom multiplexer protocol
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
*	��������: wireless modules multiplex driver on WAVECOM 2C
*
*	�ļ�����: wave_mux.h
*
*	��������: 
*
*	������: multiplex physical serial interface on WAVECOM 2C
*
*	�������ڣ�2002.07.24
*
\**************************************************************************/

#ifndef _WAVECOM_MUX_H_
#define _WAVECOM_MUX_H_

/* include header files here */
#include "DI_Uart.h"
#include "device.h"

/**************************************************************************\
*
*	��������
*
\**************************************************************************/

/* define constant here */

/* ���庯������ֵ */
#define WMUX_SUCCESS			0		/* �ɹ� */
#define WMUX_FAILURE			1		/* ʧ�� */
#define WMUX_ASYNC				2		/* �첽ִ�� */

/* ����ͨ������ */
#define WMUX_CHANNEL_AT			"MUX-WAVECOM-AT"	/* AT Channel*/
#define WMUX_CHANNEL_DATA		"MUX-WAVECOM-DATA"	/* data Channel */

/* ����ص��¼� */
#define WMUX_DATAIN				EV_DATAIN		/* �������������ɶ� */
#define WMUX_DATAOUT			EV_DATAOUT		/* ����������գ���д */
#define WMUX_MODEM				EV_MODEM		/* ͨ���ر� */

/* ��·�ź� */
#define WMUX_RLSD				MS_RLSD			/* Data Carrier */
#define WMUX_RING				MS_RING			/* Ring signal */

#define WMUX_OP_CALLBACK		IO_SETCALLBACK		/* �ص����� */
#define WMUX_OP_DUMPRXBUF		IO_DUMPBUF			/* ���ͨ�����ջ����� */
#define WMUX_OP_DATABROKEN		IO_UART_SETMODEMCTL	/* �ж����ݴ��� */
#define WMUX_OP_LINESTATUS		IO_UART_GETMODEMSTATUS /* �����·״̬ */

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

/* interface function prototype declaration */
int WMUX_Initial(void);

int WMUX_Startup(IOCOM fpread, IOCOM fpwrite);
int WMUX_Closedown(void);
int WMUX_OpenChannel(char* name);
int WMUX_CloseChannel(int handle);
int WMUX_WriteChannel(int handle, unsigned char* data, int datalen);
int WMUX_ReadChannel(int handle, unsigned char* buf, int bufsize);
int WMUX_IoControl(int handle, unsigned opcode, int value, int size);

/* ���ݻص����� */
int WMUX_OnDatain(void);
int WMUX_OnDataout(void);

/**************************** End Of Head File ****************************/
#endif	/* _WAVECOM_MUX_H_ */
