/**************************************************************************\
*
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
*
* Model   : MP
*
* Purpose : multiplexer protocol
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
*	程序名称: 多通道协议接口函数
*
*	文件名称: muxagent.h
*
*	作者姓名: 
*
*	程序功能: 
*
*	开发日期：2003.04.02
*
\**************************************************************************/

/*
 * REMARK:
 * 1. There are only 4 functions being provided, include open, close, 
 *    read, write, ioctl, these operations are compatible with Hoen
 *    file system API form.
 * 2. firstly, you can call open() to get a handle of physical serial
 *    port, and operate the port with the handle, once you get the handle
 *    successfully, you could not open any other physical serial port 
 *    once more by open().
 * 3. if you want to switch wireless module to mux mode, you must call
 *    ioctl() by IO_UART_SETMODEMCTL operation code, the rest parameter
 *    of IO_UART_SETMODEMCTL command should specify the detailed function
 *    to be executed, such as MUX_ON, MUX_OFF, MUX_CHK.
 * 4. When you call ioctl() switch mux mode back to normal mode, you must
 *    call ioctl() by IO_UART_SETMODEMCTL command with sub opcode MUX_OFF.
 * 5. please ensure that the mudule is normal mode before you want to
 *    close the handle.
 */


#ifndef _MUXAGENT_H
#define _MUXAGENT_H

#include <hp_modcfg.h>

/**************************************************************************\
*
*	常量定义
*
\**************************************************************************/

#ifdef SIEMENS_TC35
/* 定义Siemens TC35/MC35通道名称 */
#define MUX_CHANNEL1			"MUX-SIEMENS-DLC1"	/* DATA & AT Channel*/
#define MUX_CHANNEL2			"MUX-SIEMENS-DLC2"	/* AT Channel */
#define MUX_CHANNEL3			"MUX-SIEMENS-DLC3"	/* AT Channel */

#elif defined WAVECOM_2C
/* 定义Wavecom 2C/2B通道名称 */
#define MUX_CHANNEL_AT			"MUX-WAVECOM-AT"	/* AT Channel*/
#define MUX_CHANNEL_DATA		"MUX-WAVECOM-DATA"	/* DATA Channel */

#define MUX_CHANNEL1			MUX_CHANNEL_AT
#define MUX_CHANNEL2			MUX_CHANNEL_DATA

#elif defined TI_MODULE
/* 定义Texas Instruments通道名称 */
#define MUX_CHANNEL1			"MUX-TI-DLC1"	/* DATA & AT Channel*/
#define MUX_CHANNEL2			"MUX-TI-DLC2"	/* AT Channel */

#endif

/* 用于IO_UART_SETMODEMCTL控制码的参数 */
#define MUX_ON					"MUX_ON"	/* set MUX mode */
#define MUX_OFF					"MUX_OFF"	/* clear MUX mode */
#define MUX_CHK					"MUX_CHK"	/* check MUX mode */

/**************************************************************************\
*
*	数据结构及数据类型定义
*
\**************************************************************************/

/**************************************************************************\
*
*	函数声明
*
\**************************************************************************/

/* 用户接口函数 */
int MUX_OpenFile (char* name, int mode, int attrib);
int MUX_CloseFile(int handle);
int MUX_ReadFile (int handle, void* buf,  int bufsize);
int MUX_WriteFile(int handle, void* data, int datalen);
int MUX_FileIoctl(int handle, int code, void* arg, int size);

int CMUX_ClosedownFlow(void);

/**************************** End Of Head File ****************************/
#endif	/* _MUXAGENT_H */
