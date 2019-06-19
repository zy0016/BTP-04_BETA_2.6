/**************************************************************************\
*
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
*
* Model   : TMUX
*
* Purpose : TI (Texas Instruments) multiplexer protocol
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
*	PROGRAM : Multiplexer Protocol driver of TI wireless chip
*
*	FILENAME: ti_mux.h
*
*	AUTHOR  : 
*
*	FUNCTION: The implementation of MUX driver on TI wireless chip 
*			  enables multiple applications accessing to UART serial 
*			  port simultaneously. Provide triditional device-file-
*			  formed interface, transparentize the details of 
*			  multiplexer protocol.
*
*	R&D DATE: Research	2003.07.07 - 2003.08.06 (30 days)
*			  Design	2003.08.07 - 2003.08.11 ( 5 days)
*			  Coding	2003.08.12 - 2003.08.15 ( 4 days)
*			  Debugging	2003.08.18 - 2003.08.22 ( 5 days)
*
\**************************************************************************/

#ifndef _TI_MUX_H_
#define _TI_MUX_H_

/* include header files here */

#include "DI_Uart.h"
#include "device.h"

/**************************************************************************\
*
*	CONSTANT DEFINITION
*
\**************************************************************************/

/* define constant here */

/* return value definion */
#define TMUX_SUCCESS			0		/* successful */
#define TMUX_FAILURE			1		/* failure */
#define TMUX_ASYNC				2		/* asynchorous execution */

/* channel name definition */
#define TMUX_DLCNAME			"MUX-TI-DLC"
#define TMUX_CHANNEL			TMUX_DLCNAME"%d"

/* callback event definition */
#define TMUX_DATAIN				EV_DATAIN		/* data incoming, readable */
#define TMUX_DATAOUT			EV_DATAOUT		/* out buffer emtry, writable */
#define TMUX_MODEM				EV_MODEM		/* channel status changed */
#define TMUX_CONTROL			EV_BROKEN		/* for special control */

/* line signals definition */
#define TMUX_RLSD				MS_RLSD			/* Data Carrier Detect signal */
#define TMUX_RING				MS_RING			/* Ring signal */
#define TMUX_CTS				UART_CTS_ON		/* Clear to Send signal */
#define TMUX_DSR				UART_DSR_ON		/* Data Set Ready signal */

/* operation command definition */
#define TMUX_IOCS_CALLBACK		IO_SETCALLBACK		/* set callback function entry */
#define TMUX_IOCS_DUMPBUF		IO_DUMPBUF			/* dump input buffer */
#define TMUX_IOCS_MODEMCTL		IO_UART_SETMODEMCTL	/* break data transmission */
#define TMUX_IOCG_MODEMSTATUS	IO_UART_GETMODEMSTATUS /* get line status */
//#define TMUX_IOCS_SERVICE		0x0000
//#define TMUX_IOCG_SERVICE		0x0001

/* DLC service configuration */
//#define TMUX_SRV_CHK			0x00000000
//#define TMUX_SRV_DATA			0x00000001
//#define TMUX_SRV_VOICE			0x00000002

/* define the parameter of +CMUX at command */
/*
 * <operation> multiplexer transparency mechanism
 * 0 - Basic option
 * 1 - Advanced option
 */
//#define ARG_BASIC_OPTION		0
#define ARG_ADVANCED_OPTION		1
#define ARG_OPERATION			(ARG_ADVANCED_OPTION)
/*
 * <subset>
 * 0 - UIH frames used only		[default]
 * 1 - UI frames used only
 * 2 - I frames uesd only
 */
#define ARG_UIH_FRAME			0
//#define ARG_UI_FRAME			1
//#define ARG_I_FRAME				2
#define ARG_SUBSET				(ARG_UIH_FRAME)
/*
 * <port speed> (transmission rate)
 * 1 -   9,600 bit/s
 * 2 -  19,200 bit/s
 * 3 -  38,400 bit/s
 * 4 -  57,600 bit/s
 * 5 - 115,200 bit/s
 * 6 - 230,400 bit/s
 */
#define ARG_9600bps				1
#define ARG_19200bps			2
#define ARG_38400bps			3
#define ARG_57600bps			4
#define ARG_115200bps			5
#define ARG_230400bps			6
#define ARG_PORTSPEED			(ARG_115200bps)
/*
 * <N1> (maximum frame size)
 * range from 1 to 32768
 * [default 31 (64 if advanced option is used)]
 */
#define ARG_N1					64
/*
 * <T1> (acknowledgement timer in units of ten milliseconds)
 * range from 1 to 255
 * [default 10 (100ms)]
 */
#define ARG_T1					10
/*
 * <N2> (maximum number of re-transmissions)
 * range from 0 to 100
 * [default 3]
 */
#define ARG_N2					3
/*
 * <T2> (response timer for the multiplexer control channel in units of ten milliseconds)
 * range from 2 to 255, T2 must be longer than T1.
 * [default 30 (300ms)]
 */
#define ARG_T2					30
/*
 * <T3> (wake up response timer in seconds)
 * range from 1 to 255
 * [default 10]
 */
#define ARG_T3					10
/* 
 * <k> (window size, for Advanced operation with Error Recovery options)
 * range from 1 to 7
 * [default 2]
 */
#define ARG_K					2


/**************************************************************************\
*
*	DATA STRUCTURE & DATA TYPE DEFINITION
*
\**************************************************************************/

/* define any data structure here */

/* define any data structure here */
typedef int  (*IOCOM)(unsigned char* data, int datalen);

/**************************************************************************\
*
*	FUNCTION PROTOTYPE DECLARATION
*
\**************************************************************************/

/* driver initilization */
int TMUX_Initial     (void);

/* user routine function */
int TMUX_Startup     (IOCOM read, IOCOM write);
int TMUX_Closedown   (void);
int TMUX_OpenChannel (char* name);
int TMUX_CloseChannel(int handle);
int TMUX_WriteChannel(int handle, unsigned char* data, int datalen);
int TMUX_ReadChannel (int handle, unsigned char* buf, int bufsize);
int TMUX_IoctlChannel(int handle, unsigned opcode, int value, int size);

/* data in/out callback */
int TMUX_OnDatain	 (void);
int TMUX_OnDataout	 (void);


/**************************** End Of Head File ****************************/
#endif	/* _TI_MUX_H_ */
