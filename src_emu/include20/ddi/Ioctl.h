/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	HOPEN Device Drive
 *
 * Purpose :	Device Drive
 *  
 * Author  :	Di Ling
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /hopen2.0/include/Ioctl.h								$
 * $Workfile::	Ioctl.h												$
 * $Revision:: 4  $     $Date:: 03-04-26 3:28p						$
 *
\**************************************************************************/

/***************************************************************************
 *
 * Hopen Device Driver Interface
 *
 * $Revision: 6 $     $Date: 01-04-05 13:57 $    
 *
 * $Log: /Hopen/include/ddi/Ioctl.h $
 * 
 * 6     01-04-05 13:57 Dling
 * 
 * 5     00-03-24 9:21 Dling
 * bak for kernel 2.0
 * 
 * 4     00-02-22 9:06 Dling
 * add IO_FORMATDRIVE
 * 
 * 3     99-12-02 13:59 Dling
 * add IO_FORMATBLOCK
 * 
 * 2     99-11-29 13:36 Dling
 * Add " IO_GETBLKSIZE "
 * 
 * 1     99-11-08 14:20 Dling
 * 
 * 1     99-11-08 13:38 Dling
 * 
 ***************************************************************************
 */

#ifndef _IO_CONTROL_H
#define _IO_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * 
 * General I/O control:
 *
 ***************************************************************************/

#define IO_GETCALLBACK		0x0001	/*	fuction 0001H: get callback function 
										pBuf:	pointer to CALLBACKDATA struct
										size:	size of SETCALLBACK
									*/
#define IO_SETCALLBACK		0x8001	/*	fuction 8001H: set callback function
										pBuf:	pointer to CALLBACKDATA struct
										size:	size of SETCALLBACK
									*/
#define	IO_SETINPUTBUF		0x0002
#define	IO_SETOUTPUTBUF		0x0003

#define	IO_DUMPBUF			0x0004
#define	IO_CHECKBUF			0x0005

#define	IO_SETSYNCCHAR		0x0006

/* event mask code in set event mask*/
#define EV_DATAIN			0x80000000
#define EV_DATAOUT			0x40000000
#define EV_CONNECT			0x20000000
#define EV_BROKEN			0x10000000
#define	EV_MODEM			0x08000000

#define EV_READBLOCK		0x04000000
#define EV_WRITEBLOCK		0x02000000
#define EV_DEVICEREADY		0x01000000
#define EV_DEVICEINVALID	0x00800000
#define EV_IOREQCONFIRM		0x00400000

/* for memeory dev */
#define	IO_GETDEVADDR		0x0007
#define	IO_GETDEVSIZE		0x0008
#define	IO_GETBLKSIZE		0x0009
#define	IO_FORMATBLOCK		0x000a
#define	IO_FORMATDRIVE		0x000b

/***************************************************************************
 * 
 * UART I/O control
 *
 ***************************************************************************/

#define IO_UART_GETMODE		0x0030	/*	funtion 0010H: get uart device mode
										pBuf:	pointer to a COMMSTATE structure
										size:	buffer size
									*/
#define IO_UART_SETMODE		0x8030	/*	funtion 8010H: set uart device mode
										pBuf:	pointer to a COMMSTATE structure
										size:	buffer size
									*/
#define IO_UART_GETMODEMCTL	0x0032	/*			*/
#define IO_UART_SETMODEMCTL	0x8032	/*	funtion */

#define IO_UART_GETMODEMSTATUS	0x0033	/*	fuction 0012H: get modem status
											pBuf:	pointer to a double word
											size:	size of double word
										*/

/***************************************************************************
 * 
 * Keyboard I/O control
 *
 ***************************************************************************/

#define IO_KBD_GETSTATUS	0x0040	/*	funtion 0010H: get device attribute 
										pBuf:	pointer to a COMMSTATE structure
										size:	buffer size
									*/
#define IO_KBD_SETSTATUS	0x8040	/*	funtion 8010H: set device attribute 
										pBuf:	pointer to a COMMSTATE structure
										size:	buffer size
									*/
/***************************************************************************
 * 
 * Display I/O control
 *
 ***************************************************************************/
#define IO_DISP_GETGENGINE	0x0050	/*	funtion 0010H: get graphic engine 
										pBuf:	pointer to a GRAPHENGINE structure
										size:	buffer size
									*/

/***************************************************************************
 * 
 * FLASH I/O control
 *
 ***************************************************************************/
#define IO_FLH_SETMODE		0x0060	/*	funtion 0010H: get graphic engine 
										pBuf:	pointer to a double word
										size:	buffer size = 4
									*/
#define IO_FLH_GETMODE		0x8060	/*	funtion 0010H: get graphic engine 
										pBuf:	pointer to a double word
										size:	buffer size = 4
									*/
#ifdef __cplusplus
} /* extern C */
#endif


#endif // _IO_CONTROL_H
