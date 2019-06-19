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
*	��������: ��ͨ��Э��ӿں���
*
*	�ļ�����: multchnl.h
*
*	��������: 
*
*	������: 
*
*	�������ڣ�2002.05.20
*
\**************************************************************************/

#ifndef _MULTCHNL_H_
#define _MULTCHNL_H_

#include <hp_modcfg.h>

/**************************************************************************\
*
*	��������
*
\**************************************************************************/

#ifdef SIEMENS_TC35
/* ����Siemens TC35/MC35ͨ������ */
#define MPAPI_CHANNEL1			"MUX-SIEMENS-DLC1"	/* DATA & AT Channel*/
#define MPAPI_CHANNEL2			"MUX-SIEMENS-DLC2"	/* AT Channel */
#define MPAPI_CHANNEL3			"MUX-SIEMENS-DLC3"	/* AT Channel */

#elif defined WAVECOM_2C
/* ����Wavecom 2C/2Bͨ������ */
#define MPAPI_CHANNEL_AT		"MUX-WAVECOM-AT"	/* AT Channel*/
#define MPAPI_CHANNEL_DATA		"MUX-WAVECOM-DATA"	/* DATA Channel */

#define MPAPI_CHANNEL1			MPAPI_CHANNEL_AT
#define MPAPI_CHANNEL2			MPAPI_CHANNEL_DATA

#elif defined TI_MODULE
/* ����Texas Instrumentsͨ������ */
#define MPAPI_CHANNEL1			"MUX-TI-DLC1"	/* DATA & AT Channel*/
#define MPAPI_CHANNEL2			"MUX-TI-DLC2"	/* AT Channel */

#endif

/* ����� */
#define HANDLE_BASE				0x1723//'CMUX'

/**************************************************************************\
*
*	���ݽṹ���������Ͷ���
*
\**************************************************************************/

/**************************************************************************\
*
*	��������
*
\**************************************************************************/

/* ����ӿں��� */

/* ϵͳ��ʼ������ */
int MPAPP_Initial(void);

/* �û��ӿں��� */
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
