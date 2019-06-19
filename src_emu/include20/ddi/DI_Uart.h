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
 * $Archive:: /hopen2.0/include/DI_Uart.h			                $
 * $Workfile::	DI_Uart.h                                           $
 * $Revision:: 4  $     $Date:: 03-04-26 3:28p						$
 *
\**************************************************************************/

/***************************************************************************
 *
 * Hopen Device Driver Interface
 *
 * $Revision: 3 $     $Date: 00-02-22 9:09 $    
 *
 * $Log: /Hopen/include/ddi/DI_Uart.h $
 * 
 * 3     00-02-22 9:09 Dling
 * 
 * 2     00-02-22 9:09 Dling
 * add declaration of MS_RING and MS_RLSD
 * 
 * 1     99-11-08 14:20 Dling
 * 
 * 1     99-11-08 13:38 Dling
 * 
 ***************************************************************************
 */

#ifndef _DDI_UART_H
#define _DDI_UART_H

#include "stype.h"


typedef struct UART_MODE{
	DWORD	slen;
	DWORD	baudrate;
	BYTE	datalen;
	BYTE	parity;
	BYTE	stopbits;
	BYTE	flowctl;
}UART_MODE, *PUART_MODE;

/* parity mode */
#define	UART_NOPARITY		0  /* 无校验 */
#define	UART_ODDPARITY		1  /* 奇校验 */
#define	UART_EVENPARITY		2  /* 偶校验 */

/* Stop bits */
#define	UART_ONESTOPBIT		0	/* 一个停止位 */
#define	UART_TWOSTOPBITS	1	/* 两个停止位 */

#define UART_FIXED			(0x23)
#define UART_RTS_CONTROL_HANDSHAKE 0x24

/* Interrupt event mask */
#define	UART_RXCHAR		0x0001	/* Charactor in		*/
#define	UART_TXEMPTY	0x0002	/* Output buffer empty */
#define	UART_RLS		0x0004	/* Receive line state change */
#define UART_TOUT		0x0008	/* Receiver Time OUT Interrupt*/
/*	note !!!!!!!
	in version 3 of uart driver, we change the event mask definition,
	we removed UART_MODEM event, instead, we add UART_DCTS, UART_DDSR,
	UART_TERI, UART_DRLSD event. */
#define	UART_DCTS		0x0010	/* CTS signal changed	*/
#define	UART_DDSR		0x0020	/* DSR signal changed	*/
#define	UART_TERI		0x0040	/* RING signal detect	*/
#define	UART_DRLSD		0x0080	/* RLSD	RLSD signal changed	*/

/* Modem control bits */
#define	UART_DTR		0x01	/* DTR output      */
#define	UART_RTS		0x02	/* RTS output      */
#define	UART_OUT1		0x04	/* OUT1 output     */
#define	UART_OUT2		0x08	/* OUT2 output     */

/* Modem status bit */
#define	UART_CTS_ON		0x10	/* CTS on			*/
#define	UART_DSR_ON		0x20	/* DSR on			*/
#define	UART_RING_ON	0x40	/* RING on			*/
#define	UART_RLSD_ON	0x80	/* RLSD	on			*/

// added by ling di:
#define MS_RING			UART_RING_ON
#define MS_RLSD			UART_RLSD_ON

enum loopback_mode { INT_LPBK	/* Internal Loopback */,
                     EXT_LPBK   /* External Loopback */
};

#define TX_BUFFER_SIZE 256
#define RX_BUFFER_SIZE 256
#define MAX_RXBD	10 
#define MAX_TXBD	10 

#define CPCR_FLAG 	( 1 << ( 15 -15 ) )
#define CPCR_OPCODE ( 0 << ( 15 -7 ) )

/** General macros for defining bit positions for pins. */
#define PORTPIN32(n)	(1 << (31-(n)))	/* 32-bit wide control registers */
#define PORTPIN16(n)	(1 << (15-(n)))	/* 16-bit wide control registers */

/************** SMC1/SCC2 ****************/
#define SMC1_PRAM 			2
#define SMC1_REG 			0
#define SCC2				1
#define CPCR_SMC1_CHNUM 	( 9 << ( 15 -11 ) )
#define CPCR_SCC2_CHNUM 	( 4 << (15-11) )


/* SMC1/SCC2 Bit in CPM In-Service Interrupt Register */
#define SMC1_INTERRUPT 		( 1 << ( 31 -27 ) )
#define SCC2_INTERRUPT 		( 1 << ( 31 -2 ) )

/* SMC1/SCC2 Interrupt Vector Code in CPM Vector Register (CIVR) */
#define SMC1_VECTOR 		( 0x04 )
#define SCC2_VECTOR 		( 0x1d )

struct setup_parameters {
    int lpbk_mode;
    unsigned char interrupt_lvl;
};

/** Buffer Descriptor Format */
struct BufferDescriptor {
  unsigned short bd_status; /* control status */
  unsigned short bd_length; /* transfer length */
  char* bd_addr; 			/* buffer address */
};

/** SMC1/SCC2 Tx and Rx buffer descriptors allocated at the
 *  immr->udata_bd address on Dual-Port RAM */
struct CommonBufferDescriptor {
  struct BufferDescriptor rxbd[MAX_RXBD]; /* Rx BD1 */
  struct BufferDescriptor txbd[MAX_TXBD]; /* Tx BD */
};

#endif // _DDI_U//////ART_H
