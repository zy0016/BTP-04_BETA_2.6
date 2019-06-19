	/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
 * Author  :     WangXinshe
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/hopen/devmajor.h,v $
 * $Name:  $
 *
 * $Revision: 1.10 $     $Date: 2004/02/17 06:04:18 $
 * 
\**************************************************************************/

#ifndef _HOPEN_DEVMAJOR_H_
#define	_HOPEN_DEVMAJOR_H_

#define ZERO_MAJOR		0
#define	CONSOLE_MAJOR		1
#define	IPC_MAJOR		2
#define RTC_MAJOR		3
#define MTD_CHAR_MAJOR	4
#define NULL_MAJOR		5
#define LP_MAJOR		6
#define	SERIAL_MAJOR		9
#define	KBD_MAJOR		10
#define	MOUSE_MAJOR		11
#define SOUND_MAJOR		14
#define TOUCHPEN_MAJOR		15
#define LCD_MAJOR 16
#define AUDIO_MAJOR		17
#define BUZZER_MAJOR	18
#define PPP_MAJOR	19
#define FB_MAJOR	20
#define	UART_MAJOR	21
#define VCOMM_MAJOR	22
#define USBD_CHAR_MAJOR	23
#define MMA_MAC_MAJOR   24
#define CSI_MAJOR	25
#define LEDMAN_MAJOR    26
#define PM_MAJOR		27
#define INFO_MAJOR		28
#define BLUETOOTH_MAJOR		29

#define	ROMDEV_MAJOR	0x8001
#define FATDEV_MAJOR	0x8002
#define MTD_BLOCK_MAJOR 0x0004
#define CF_MAJOR		0x8003
#define SYNCFLASH_MAJOR 0x8005
#define MMC_MAJOR	0x8006
#define DOC_MAJOR	0x8007
#endif /* _HOPEN_DEVMAJOR_H_ */
