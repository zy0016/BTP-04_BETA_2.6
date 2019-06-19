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
*	文件名称: multchnl.h
*
*	作者姓名: 
*
*	程序功能: 
*
*	开发日期：2002.05.20
*
\**************************************************************************/

#ifndef _MULTCHNL_H_
#define _MULTCHNL_H_

#include <hp_modcfg.h>

/**************************************************************************\
*
*	常量定义
*
\**************************************************************************/

#ifdef SIEMENS_TC35
/* 定义Siemens TC35/MC35通道名称 */
#define MPAPI_CHANNEL1			"MUX-SIEMENS-DLC1"	/* DATA & AT Channel*/
#define MPAPI_CHANNEL2			"MUX-SIEMENS-DLC2"	/* AT Channel */
#define MPAPI_CHANNEL3			"MUX-SIEMENS-DLC3"	/* AT Channel */

#elif defined WAVECOM_2C
/* 定义Wavecom 2C/2B通道名称 */
#define MPAPI_CHANNEL_AT		"MUX-WAVECOM-AT"	/* AT Channel*/
#define MPAPI_CHANNEL_DATA		"MUX-WAVECOM-DATA"	/* DATA Channel */

#define MPAPI_CHANNEL1			MPAPI_CHANNEL_AT
#define MPAPI_CHANNEL2			MPAPI_CHANNEL_DATA

#elif defined TI_MODULE
/* 定义Texas Instruments通道名称 */
#define MPAPI_CHANNEL1			"MUX-TI-DLC1"	/* DATA & AT Channel*/
#define MPAPI_CHANNEL2			"MUX-TI-DLC2"	/* AT Channel */

#endif

/* 句柄基 */
#define HANDLE_BASE				0x1723//'CMUX'

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

/* 对外接口函数 */

/* 系统初始化函数 */
int MPAPP_Initial(void);

/* 用户接口函数 */
int MPAPP_Startup	(char* dev_name, int baudrate);
int MPAPP_Startup2	(int com_handle);
int MPAPP_Closedown	(void);
int MPAPP_OpenFile	(char* name, int mode, int attrib);
int MPAPP_CloseFile	(int handle);
int MPAPP_ReadFile	(int handle, void* buf,  int bufsize);
int MPAPP_WriteFile	(int handle, void* data, int datalen);
int MPAPP_FileIoControl(int handle, int code, void *pbuf, int size);

/**************************** End Of Head File ****************************/
#endif	/* _MULTCHNL_H_ */
