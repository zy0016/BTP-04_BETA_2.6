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
 * Author  :    
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/hal-armv/armeval7t.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/09/29 03:24:06 $
 * 
\**************************************************************************/

#ifndef _ARM_EVAL_7T_H_
#define _ARM_EVAL_7T_H_

#define ARMEVAL_IO_BASE		0x07FF0000

#define INT_NUM 			21
#define INT_I2C				20
#define INT_ETH_MAC_RX		19
#define INT_ETH_MAC_TX		18
#define INT_ETH_BDMA_RX		17
#define INT_ETH_BDMA_TX		16
#define INT_HDLC_B_RX		15
#define INT_HDLC_B_TX		14
#define INT_HDLC_A_RX		13
#define INT_HDLC_A_TX		12
#define INT_TIMER1			11
#define INT_TIMER0			10
#define INT_GDMA1			9
#define INT_GDMA0			8
#define INT_UART1_RX		7
#define INT_UART1_TX		6
#define INT_UART0_RX		5
#define INT_UART0_TX		4
#define INT_EXT_3			3
#define INT_EXT_2			2
#define INT_EXT_1			1
#define INT_EXT_0			0

//sys cfg
#define SYSCFG				(ARMEVAL_IO_BASE + 0x0000)

//uart registers
#define UART0_BASE			(ARMEVAL_IO_BASE + 0xD000)
#define UART1_BASE			(ARMEVAL_IO_BASE + 0xE000)
struct arm_eval_uart
{
	volatile unsigned long ULCON;
	volatile unsigned long UCON;
	volatile unsigned long USTAT;
	volatile unsigned long UTXBUF;
	volatile unsigned long URXBUF;
	volatile unsigned long UBRDIV;
	volatile unsigned long BRDCNT;
	volatile unsigned long BRDCLK;
};

//timer registers
#define TMOD				(ARMEVAL_IO_BASE + 0x6000)
#define TDATA0				(ARMEVAL_IO_BASE + 0x6004)
#define TDATA1				(ARMEVAL_IO_BASE + 0x6008)
#define TCNT0				(ARMEVAL_IO_BASE + 0x600C)
#define TCNT1				(ARMEVAL_IO_BASE + 0x6010)

//io port
#define IOPMOD				(ARMEVAL_IO_BASE + 0x5000)
#define IOPCON				(ARMEVAL_IO_BASE + 0x5004)
#define IOPDATA          		(ARMEVAL_IO_BASE + 0x5008) 

//int registers
#define INTMOD				(ARMEVAL_IO_BASE + 0x4000)
#define INTPND				(ARMEVAL_IO_BASE + 0x4004)
#define INTMSK				(ARMEVAL_IO_BASE + 0x4008)
#define INTPRI0				(ARMEVAL_IO_BASE + 0x400C)
#define INTPRI1				(ARMEVAL_IO_BASE + 0x4010)
#define INTPRI2				(ARMEVAL_IO_BASE + 0x4014)
#define INTPRI3				(ARMEVAL_IO_BASE + 0x4018)
#define INTPRI4				(ARMEVAL_IO_BASE + 0x401C)
#define INTPRI5				(ARMEVAL_IO_BASE + 0x4020)
#define INTOFFSET			(ARMEVAL_IO_BASE + 0x4024)
#define INTPNDPRI			(ARMEVAL_IO_BASE + 0x4028)
#define INTPNDTST			(ARMEVAL_IO_BASE + 0x402C)
#define INTOSET_FIQ			(ARMEVAL_IO_BASE + 0x4030)
#define INTOSET_IRQ			(ARMEVAL_IO_BASE + 0x4034)

/****************************************************************************/
 /*    defines in terms of leds  */

#define LEDMAN_CMD_SET		0x01	/* turn on briefly to show activity */
#define LEDMAN_CMD_ON		0x02	/* turn LED on permanently */
#define LEDMAN_CMD_OFF		0x03	/* turn LED off permanently */
#define LEDMAN_CMD_FLASH	0x04	/* flash this LED */
#define LEDMAN_CMD_RESET	0x05	/* reset LED to default behaviour */ 

/*     end of leds   */
/****************************************************************************/


#endif // _ARM_EVAL_7T_H_
