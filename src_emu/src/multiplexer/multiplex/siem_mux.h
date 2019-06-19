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
*	程序名称: Digital cellular telecommunications system
*			  Terminal Equipment to Mobile Station multiplexer protocol 
*			  (3GPP TS 07.10 version 7.2.0 Release 1998) 
*			  Implementation on Siemens wireless modules TC35/MC35
*
*	文件名称: multiplx.h
*
*	作者姓名: 
*
*	程序功能: impementation of Multiplexer Protocol (MP) on TC35/MC35
*
*	开发日期：2002.04.22
*
\**************************************************************************/

#ifndef _SIEMENS_MUX_H_
#define _SIEMENS_MUX_H_

#include "DI_Uart.h"
#include "device.h"

/**************************************************************************\
*
*	常量定义
*
\**************************************************************************/

/* 定义函数返回值 */
#define CMUX_SUCCESS			0		/* 成功 */
#define CMUX_FAILURE			1		/* 失败 */
#define CMUX_ASYNC				2		/* 异步执行 */

/* 定义通道名称 */
#define CMUX_CHANNEL1			"MUX-SIEMENS-DLC1"	/* DATA & AT Channel*/
#define CMUX_CHANNEL2			"MUX-SIEMENS-DLC2"	/* AT Channel */
#define CMUX_CHANNEL3			"MUX-SIEMENS-DLC3"	/* AT Channel */

/* 定义回调事件 */
#define CMUX_DATAIN				EV_DATAIN		/* 由数据来到，可读 */
#define CMUX_DATAOUT			EV_DATAOUT		/* 输出缓冲区空，可写 */
#define CMUX_MODEM				EV_MODEM		/* 通道关闭 */

/* 线路信号 */
#define WMUX_RLSD				MS_RLSD			/* Data Carrier */
#define WMUX_RING				MS_RING			/* Ring signal */

/* 定义操作选项 */
#define CMUX_OP_CALLBACK		IO_SETCALLBACK		/* 回调函数 */
#define CMUX_OP_DUMPRXBUF		IO_DUMPBUF			/* 清空通道接收缓冲区 */
#define CMUX_OP_DATABROKEN		IO_UART_SETMODEMCTL	/* 中断数据传输 */
#define CMUX_OP_LINESTATUS		IO_UART_GETMODEMSTATUS /* 获得线路状态 */

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

/* 对外接口函数 */

/* 系统初始化函数 */
int CMUX_Initial(void);

/* 用户接口函数 */
int CMUX_Startup(IOCOM fpread, IOCOM fpwrite);
int CMUX_Closedown(void);
int CMUX_OpenChannel(char* name);
int CMUX_CloseChannel(int handle);
int CMUX_WriteChannel(int handle, unsigned char* data, int datalen);
int CMUX_ReadChannel(int handle, unsigned char* buf, int bufsize);
int CMUX_IoControl(int handle, unsigned opcode, int value, int size);

/* 数据回调函数 */
int CMUX_OnDatain(void);
int CMUX_OnDataout(void);

/**************************** End Of Head File ****************************/
#endif	/* _SIEMENS_MUX_H_ */
