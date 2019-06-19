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
*	程序名称: wireless modules multiplex driver on WAVECOM 2C
*
*	文件名称: wave_mux.h
*
*	作者姓名: 
*
*	程序功能: multiplex physical serial interface on WAVECOM 2C
*
*	开发日期：2002.07.24
*
\**************************************************************************/

#ifndef _WAVECOM_MUX_H_
#define _WAVECOM_MUX_H_

/* include header files here */
#include "DI_Uart.h"
#include "device.h"

/**************************************************************************\
*
*	常量定义
*
\**************************************************************************/

/* define constant here */

/* 定义函数返回值 */
#define WMUX_SUCCESS			0		/* 成功 */
#define WMUX_FAILURE			1		/* 失败 */
#define WMUX_ASYNC				2		/* 异步执行 */

/* 定义通道名称 */
#define WMUX_CHANNEL_AT			"MUX-WAVECOM-AT"	/* AT Channel*/
#define WMUX_CHANNEL_DATA		"MUX-WAVECOM-DATA"	/* data Channel */

/* 定义回调事件 */
#define WMUX_DATAIN				EV_DATAIN		/* 由数据来到，可读 */
#define WMUX_DATAOUT			EV_DATAOUT		/* 输出缓冲区空，可写 */
#define WMUX_MODEM				EV_MODEM		/* 通道关闭 */

/* 线路信号 */
#define WMUX_RLSD				MS_RLSD			/* Data Carrier */
#define WMUX_RING				MS_RING			/* Ring signal */

#define WMUX_OP_CALLBACK		IO_SETCALLBACK		/* 回调函数 */
#define WMUX_OP_DUMPRXBUF		IO_DUMPBUF			/* 清空通道接收缓冲区 */
#define WMUX_OP_DATABROKEN		IO_UART_SETMODEMCTL	/* 中断数据传输 */
#define WMUX_OP_LINESTATUS		IO_UART_GETMODEMSTATUS /* 获得线路状态 */

/**************************************************************************\
*
*	数据结构及数据类型定义
*
\**************************************************************************/

/* define any data structure here */
typedef int  (*IOCOM)(unsigned char* data, int datalen);

/**************************************************************************\
*
*	函数声明
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

/* 数据回调函数 */
int WMUX_OnDatain(void);
int WMUX_OnDataout(void);

/**************************** End Of Head File ****************************/
#endif	/* _WAVECOM_MUX_H_ */
