/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Mobile Engine
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _ME_AT_H_
#define _ME_AT_H_

//usb modem
typedef enum
{
		BlueTooth_C,
		USB_C,
		Total_C
} ModemCnlType;

typedef enum
{
   		CommonFunc,		//普通功能
		ModemFunc,		//modem功能
		IllegalFunc		//非法功能
} ChannelFunc;

typedef struct
{
	ChannelFunc		BTfunc;			//蓝牙功能配置情况
	ChannelFunc		USBfunc;			//USB功能配置情况
} ModemCnl;
//end usb modem

typedef void (*MAT_Indication)(int event, void * pdata, int datalen);

enum mat_event { MAT_RESET, MAT_DATAIN };

int MAT_Init ( int mode, int para1, int para2, MAT_Indication indication );
int MAT_Send ( int mode, void * pdata, int datalen );
int MAT_Reset ( int mode );

#define WAIT_FOR_MUTEX()	
#define RELEASE_MUTEX()


//usb modem
int Modem_OpenCnl(MAT_Indication indication);

int Modem_CloseCnl(void);

int Modem_SetChannel (ModemCnlType CnlType, ChannelFunc Func);

ChannelFunc Modem_GetChannel (ModemCnlType CnlType);

int Modem_Send(void * pdata, int datalen);
//end usb modem

#endif
