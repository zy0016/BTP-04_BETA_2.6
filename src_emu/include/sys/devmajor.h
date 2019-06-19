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
 * $Source: /cvs/hopencvs/src/include/sys/devmajor.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2004/02/27 06:17:06 $
 * 
\**************************************************************************/

#ifndef _SYS_DEVMAJOR_H_
#define	_SYS_DEVMAJOR_H_

#define ZERO_MAJOR		0
#define	CONSOLE_MAJOR	1
#define	IPC_MAJOR		2
#define AUDIO_MAJOR		3
#define BUZZER_MAJOR	7
#define RTC_MAJOR		8
#define	SERIAL_MAJOR	9
#define	KBD_MAJOR		10
#define	MOUSE_MAJOR		11
#define SOUND_MAJOR		14
#define TOUCHPEN_MAJOR 	15
#define LCD_MAJOR		16
#define	UART_MAJOR	9
#define PPP_MAJOR	19


#define	ROMDEV_MAJOR	0x8001
#define FATDEV_MAJOR	0x8002
#define SYNCFLASH_MAJOR 0x8003

#endif /* _SYS_DEVMAJOR_H_ */
