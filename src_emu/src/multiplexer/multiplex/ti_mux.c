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
*	FILENAME: ti_mux.c
*
*	AUTHOR  : 
*
*	FUNCTION: The implementation of MUX driver on TI wireless chip 
*			  enables multiple applications accessing to UART serial 
*			  port simultaneously. Provide triditional device-file-
*			  formed interface, transparentize the details of 
*			  multiplexer protocol.
*
*	REMARKS	: with TI wireless chip, we cannot configurate uart
*			  to flow control, or it maybe connot receive data morally.
*
*	R&D DATE: Research	2003.07.07 - 2003.08.06 (30 days)
*			  Design	2003.08.07 - 2003.08.11 ( 5 days)
*			  Coding	2003.08.12 - 2003.08.15 ( 4 days)
*			  Debugging	2003.08.18 - 2003.08.22 ( 5 days)
*
\**************************************************************************/
/**************************************************************************\
*
* Revision Information
*
* VERSION 1.0 -------------------------------------------------------------
* REC :	Add a new member function BUF_Transmit() to buffer class, in order 
*		to integrate transmission capability into buffer operations, it 
*		will take easier way to handle DATAOUT indication, at meanwhile, 
*		it could hide the details of the buffer operation.
* DATE: 2003.08.18
*
* VERSION 2.0 -------------------------------------------------------------
* REC :	
* DATE:	2003.XX.XX
*
\**************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "mux_lib.h"
#include "ti_mux.h"

/**************************************************************************\
*
*	Debug output
*
\**************************************************************************/

//#define DATAIO_DEBUG
#define MUX_WARNING
//#define MUX_MSGOUT

#ifdef _EMULATE_
	extern void MsgOut(char *format, ... );
#endif
	
#if defined MUX_MSGOUT && defined _EMULATE_
	#define msgout		MsgOut
#else
	static void msgout(char* aa, ... ){aa=NULL;}
#endif

#if defined MUX_WARNING && defined _EMULATE_
	#define warning		MsgOut
#else
	static void warning(char* aa, ... ){aa=NULL;}
#endif


/**************************************************************************\
*
*	CONSTANT DEFINITION
*
\**************************************************************************/

/* set DLC1 with data service configuration */
#define DATA_SERVICE_CONFIG

/* DLC (Data Link Connection) state definition */
#define STATE_INVALID		0		/* invalided modular */
#define STATE_CLOSEDOWN		1		/* initialized OK */
#define STATE_DISCONNECTED	2		/* Startup OK */
#define STATE_CONNECTING	3		/* Start connect multiplexer */
#define STATE_CONNECTED		4		/* DLC established */
#define STATE_DISCONNECTING	5		/* multiplexer Start OK */
#define STATE_CLOSING		6		/* closedown a virtual channel */
#define STATE_ATCOMMAND		7		/* sending at command */

/* DLC number defintion (a control channel (DLC0) included) */
#define MAX_DLC_NUM			3

/* system parameter definition */
#define MAX_FRAME_SIZE		64		/* maximum frame size */
#define ACK_TIMEOUT			400//100		/* acknowledgement timeout */
#define RETRY_TIMES			3		/* maximum number of re-transmissions */
#define DLC0_TIMEOUT		900//300		/* response time for DLC0 */
#define WAKEUP_TIMEOUT		(10*1000)	/* wake up response timeout */

/* frame type definitions */
#define FRAME_SABM			0x2F	/* Command for establishing a DLC */
#define FRAME_UA			0x63	/* Acknowledgement of command */
#define FRAME_DM			0x0F	/* Disconnected indication */
#define FRAME_DISC			0x43	/* Command for Disconnection */
#define FRAME_UIH			0xEF	/* Information */

/* message type of UIH frame */
#define UIH_PN				0x20	/* DLC Parameter Negotiation */
#define UIH_PSC				0x10	/* Power Saving Control (length==0) */
#define UIH_CLD				0x30	/* MUX Close Down   (length==0) */
#define UIH_Test			0x08	/* Test Command */			
#define UIH_FCon			0x28	/* Flow Control on  (length==0) */
#define UIH_FCoff			0x18	/* Flow Control off (length==0) */
#define UIH_MSC				0x38	/* Modem status Command */
#define UIH_NSC				0x04	/* Non supported Command Response */
#define UIH_RPN				0x24	/* Remote Port Negotiation Command */
#define UIH_RLS				0x14	/* Remote Line Status Command */
#define UIH_SNC				0x34	/* Service Negotiation Command */

/* Supported Service mask */
#define MSK_PN				0x0001
#define MSK_PSC				0x0002
#define MSK_CLD				0x0004
#define MSK_Test			0x0008
#define MSK_FCon			0x0010
#define MSK_FCoff			0x0020
#define MSK_MSC				0x0040
#define MSK_NSC				0x0080
#define MSK_RPN				0x0100
#define MSK_RLS				0x0200
#define MSK_SNC				0x0400

/* MSC message */
/* modem signal bit definition */
#define FC_BIT				0x02	/* Flow Control bit */
#define RTC_BIT				0x04	/* Ready to Communicate */
#define RTR_BIT				0x08	/* Ready to Receive */
#define IC_BIT				0x40	/* Ring */
#define DV_BIT				0x80	/* Data Carrier Detect */
/* a break signal bit */
#define BRK_BIT				0x20	/* break bit */

/* SNC message */
/* service bit */
#define SRV_DATA			(1<<1)	/* data service bit */
#define SRV_VOICE			(1<<2)	/* voice service bit */
/* voice codec */
#define VC_GSM0621			(1<<1)	/* GSM 06.21 coded */
#define VC_PCM64U			(1<<2)	/* PCM 64 bits/s U-law coded */
#define VC_ADPCM32			(1<<3)	/* ADPCM 32 bits/s coded (ITU-T G.726) */
#define VC_HALFRATE			(1<<4)	/* halfrate coded */
#define VC_PCM64A			(1<<5)	/* PCM 64 bits/s A-law coded */
#define VC_PCM128			(1<<6)	/* PCM 128 bits/s coded */
/* data codec */
#define DC_CS				(1<<1)	/* Circuit-switchd data */
#define DC_GPRS				(1<<2)	/* GPRS data */
#define DC_DEBUG			(1<<3)	/* Trace/Debug output */

/* special bit definitions */
#define PF_BIT				0x10	/* poll/finial bit */
#define CR_BIT				0x02	/* command/respond bit */
#define EA_BIT				0x01	/* extension bit */

/* HDLC transparence mechanism */
#define HDLC_FLAG			0x7e	/* HDLC flag */
#define HDLC_ESCAPE			0x7d	/* HDLC escape charactor */
#define HDLC_SPCHAR			0x20	/* control charactor map */

/* Software flow control charactor */
#define SP_XON				0x11
#define SP_XOFF				0x13

/* flags on DLC */
#define F_FLOWCTRL			0x0001	/* flow control */
#define F_TIMEOUT			0x0002	/* timeout */
#define F_POLLING			0x0004	/* polling */
#define F_POLLWAIT			0x0008	/* poll wait */
#define F_REMOTESLEEP		0x0010	/* remote sleep */
#define F_WAKEUPREMOTE		0x0020	/* remote wakeup */
#define F_LOCALSLEEP		0x0040	/* local sleep */
#define F_WAKEUPLOCAL		0x0080	/* local wakeup */

/* buffer definition */
#define BUF_SIZE			(1024 * 4)

/* critical operation definition */
#define ENTER_MUTEX			LIB_WaitMutex  (&tmux.mutex)
#define LEAVE_MUTEX			LIB_SignalMutex(&tmux.mutex)

/* define AT+CMUX command */
#define CMUX_AT_COMMAND		"AT+CMUX=1,0,%d,64,10,3,30,10,2\r"

/**************************************************************************\
*
*	DATA STRUCTURE & DATA TYPE DEFINITION
*
\**************************************************************************/

typedef unsigned char	UINT8;

/* HDLC frame structure analysis state definition */
typedef enum {S_FLAG, S_ADDR, S_CTRL, S_DATA} SCANSTATE;

/* UART callback data structure */
typedef struct
{
	_CALLBACK	function;	/* the entry of callback function */
	void*		para;		/* the parameter of callback function */
	DWORD		EventCode;	/* uart event mask */
}	UART_CALLBACK;

/* MUX frame data structure */
typedef struct
{
	int		DLCI;				/* DLC identifier */
	int		type;				/* frame type */
	int		CR_bit, PF_bit;		/* C/R, P/F bit	*/
	UINT8	data[MAX_FRAME_SIZE + 1];	/* frame information */
	int		datalen;			/* frame length */
}	FRAME;

/* buffer data structure */
typedef struct
{
	UINT8	buf[BUF_SIZE];		/* buffer start	*/
	UINT8*	data;				/* data pointer */
	UINT8*	write;				/* start to write */
	int		datalen;			/* data length	*/
}	BUFFER;

typedef struct PollQ
{
	struct PollQ *next;			/* the next queued poll frame */
	UINT8	msg_type;			/* UIH message type */
	UINT8	DLCI;				/* control message related DLCI */
	UINT8	datalen;			/* message length */
	UINT8   data[1];			/* start of the message */
}	POLLQ;

/* DLC management data structure */
typedef struct
{
	int			state;			/* DLC state	*/
	APPTIMER	timer;			/* DLC timer	*/
	OSHANDLE	event;			/* wait event for thread sleep */
	UART_CALLBACK callback;		/* user callback */
	BUFFER		rxbuf;			/* reception buffer	*/
	int			status;			/* line status, e.g. RING, DCD, RTX, CTX */
	int			service;		/* availible service, e.g. AT command, data, voice */
	int			flags;			/* flag record */
	POLLQ		*pollque;		/* poll frame queue */
	int			retry;			/* retry times */
	int			DLCI;			/* Channel Identifier */
}	CHANNEL;

typedef struct
{
	char*	name;				/* UIH message name */
	UINT8	type;				/* UIH message type */
	UINT	mask;				/* UIH message mask */
}	UIH_MSG;

typedef struct
{
	char* rx_v24;				/* as received signal */
	char* tx_v24;				/* as transmitted signal */
	UINT8 mask;					/* signal mask */
}	MSC_SIGNAL;

/* multiplexer driver management data structure */
typedef struct
{
	MUTEX		mutex;			
	IOCOM		write;			/* output function */
	IOCOM		read;			/* input function */
	CHANNEL		DLC[MAX_DLC_NUM];	/* DLC object list */
//	UINT8		txbuf[BUF_SIZE];	/* transmission buffer */
	int			flowctrl;		/* buffer flow control */
	SCANSTATE	scanstate;		/* receive frame state */
	FRAME		frame;			/* MUX frame structure */
	UINT		unsupport;		/* unsupported message type */
}	TMUX;


/**************************************************************************\
*
*	GLOBLE VARIABLE DEFINITION
*
\**************************************************************************/

/* Frame Check Sum calculation table */
static const UINT8 fcstab8[256] = 
{   //reserved, 8-bit, poly=0x07
    0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75,  
	0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
    0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69,  
	0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
    0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D,  
	0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51,  
	0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,

    0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05,  
	0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
    0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19,  
	0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D,  
	0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
    0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21,  
	0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,

    0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95,  
	0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89,  
	0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
    0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD,  
	0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
    0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1,  
	0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,

    0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5,  
	0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
    0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9,  
	0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
    0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD,  
	0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1,  
	0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
};

static UIH_MSG UIH_msg[] =
{
	/* name */		/* type */      /* mask */
	{"PN",			UIH_PN,			MSK_PN},
	{"PSC",			UIH_PSC,		MSK_PSC},
	{"CLD",			UIH_CLD,		MSK_CLD},
	{"Test",		UIH_Test,		MSK_Test},
	{"FCon",		UIH_FCon,		MSK_FCon},
	{"FCoff",		UIH_FCoff,		MSK_FCoff},
	{"MSC",			UIH_MSC,		MSK_MSC},
	{"NSC",			UIH_NSC,		MSK_NSC},
	{"RPN",			UIH_RPN,		MSK_RPN},
	{"RLS",			UIH_RLS,		MSK_RLS},
	{"SNC",			UIH_SNC,		MSK_SNC},
	{NULL,			0,				0}
};

static MSC_SIGNAL MSC_signal[] = 
{
	/* rx */		/* tx */		/* mask */
	{"FC",			"FC",			FC_BIT},
	{"DSR(RTC)",	"DTR(RTC)",		RTC_BIT},
	{"CTS(RTR)",	"RTS(RTR)",		RTR_BIT},
	{"RING(IC)",	"",				IC_BIT},
	{"DCD(DV)",		"",				DV_BIT},
//	{"",			"BRK",			BRK_BIT},
	{NULL,			NULL,			0}
};

/* multiplexer management instance */
static TMUX tmux;

/**************************************************************************\
*
*	FUNCTION PROTOTYPE DECLARATION
*
\**************************************************************************/

/* opening  function prototypes */
static int	 TMUX_GetDLCIByName(char* channel_name);
static int	 TMUX_SendPollQueue(int DLCI);
static UINT8* TMUX_Transparence(UINT8* cp, UINT8 octet);
static int	 TMUX_SendUIHMessage(UINT8 msg_type, int cmd_tag,
							   int msg_len, UINT8* msg_data);
static int   TMUX_SendFrameData(UINT8 frametype, int DLCI, 
								UINT8* data, int datalen);
static int	 TMUX_WakeupRemote(void);
static int	 TMUX_ProcMSCChange(int DLCI, int status, int ostatus);
static int	 TMUX_ProcRxUIH(FRAME *frame);
static int   TMUX_ProcRxFrame(FRAME *frame);
static int   TMUX_WaitEvent(int DLCI, int millisecond);
static void  TMUX_TimeOut(void* DLC);
static void	 TMUX_CallBack(int DLCI, unsigned int event);
static int   TMUX_ResetChannel(int DLC);
static int   TMUX_ResetMultiplexer(void);


/* serial device I/O */
static int   TMUX_ReadDevice (UINT8* buf, int bufsize);
static int	 TMUX_WriteDevice(UINT8* data, int datalen);
/* buffer operations */
#define		 BUF_IsEmpty(pbuf)	((pbuf)->data == (pbuf)->write)
static int   BUF_Datasize(BUFFER* buf);
static int   BUF_Cleanup (BUFFER* buf);
static int   BUF_GetData (BUFFER* buf, UINT8* pbuf, int buflen);
static int   BUF_PutData (BUFFER* buf, UINT8* data, int datalen);
static int	 BUF_Transmit(BUFFER* buf);

/**************************************************************************\
*
*	INTERFACE FUNCTION IMPLEMENTATION
*	(INITIAL|ENTRY|INTERFACE)
*
\**************************************************************************/

/*========================================================================*\
*
*	FUNCTION: TMUX_Initial
*
*	PURPOSE : initialize xmux fuctional module
*
*   PARAM   : void
*
*	RETURN  : TMUX_SUCCESS	successful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

int TMUX_Initial(void)
{
	int i;
	char name[] = "DLC0";
	/* check the state of control channel, 
	 * for fear of re-initializion.
	 */
	if (tmux.DLC[0].state != STATE_INVALID)
		return TMUX_FAILURE;

	/* cleanup gloab data structure */
	memset(&tmux, 0, sizeof(TMUX));

	/* create necessary systemic resouces, 
	 * such mutex lock, timer, event, buffer management etc. 
	 */
	if (LIB_CreateMutex(&tmux.mutex) == -1)
		return TMUX_FAILURE;

	/* initialize each available channels include control channle as well,
	 * here, we have to setup data I/O buffer, tx/rx timer, event for
	 * each channel.
	 */
	for (i = 0; i < MAX_DLC_NUM; i++)
	{
		tmux.DLC[i].DLCI = i;
		/* create DLC timer */
		LIB_CreateTimer(&tmux.DLC[i].timer);
		/* create DLC wait event */
		if ((tmux.DLC[i].event = LIB_CreateEvent(name)) == NULL)
			return TMUX_FAILURE;
		/* cleanup all the reception buffer with the DLC */
		BUF_Cleanup(&tmux.DLC[i].rxbuf);
		/* get the next DLC name */
		name[3] ++;
	}

	/* set STATE_CLOSEDOWN state on all the channels */
	for (i = 0; i < MAX_DLC_NUM; i ++)
		tmux.DLC[i].state = STATE_CLOSEDOWN;

	return TMUX_SUCCESS;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_Startup
*
*	PURPOSE : start mux driver, interaction with remote module,
*			  try to eastablish control channel DLC0.
*	
*	REMARK	: this function maybe block for a while within fixed interval
*			  time.
*
*   PARAM   : read		the entry of read function on serials;
*			  write		the entry of write function on serials;
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

int TMUX_Startup(IOCOM read, IOCOM write)
{
	UINT8	buf[64];
	int		i, len, cmdlen = strlen(CMUX_AT_COMMAND);
	/* check the validity of input parameters */
	if (read == NULL || write == NULL)
		return TMUX_FAILURE;

	/* get mutex lock before entering criticals */
	ENTER_MUTEX;

	/* check the state of control channel DLC0, only if on CLOSEDOWN
	 * state, we could contiue further operation, otherwise we have
	 * to return with error code. */
	if (tmux.DLC[0].state != STATE_CLOSEDOWN)
		goto ERROR_NOOP;

	/* reset mux driver and all the management data structure of each
	 * channel. */
	TMUX_ResetMultiplexer();
	tmux.read  = read;
	tmux.write = write;
	tmux.DLC[0].state = STATE_CONNECTING;

	/* activate module in idle mode */
	write((UINT8*)"AT\r", 3);
	OS_Sleep(200*OS_GetTicksPerSecond()/1000);

	write((UINT8*)"AT\r", 3);
	OS_Sleep(200*OS_GetTicksPerSecond()/1000);

	/* clean up uart buffer */
	while (read(buf, sizeof(buf)) >= 0)
		;

	/* send standard AT+CMUX at command to activate remote mux agent */
	sprintf((char*)buf, CMUX_AT_COMMAND, ARG_PORTSPEED);
	if ((len = write(buf, cmdlen)) != cmdlen)
	{
		len = (len < 0 ? 0 : len);
		BUF_PutData(&tmux.DLC[0].rxbuf, buf+len, cmdlen - len);
		/* set flow control */
		tmux.flowctrl = 1;
	}
#if 0
	/* wait for a while, let the remote mux entity can execute cmux command */
	TMUX_WaitEvent(0, 1000);
#endif

	/* now we start to send SABM frame, try to establish DLC0 connection
	 * with remote mux driver. at meanwhile we should start a repeat 
	 * timer for error recovery mechanism. */
	tmux.scanstate = S_FLAG;
	for (i = 0; i < RETRY_TIMES; i ++)
	{
		int retval;
		if (TMUX_SendFrameData(FRAME_SABM, 0, NULL, 0) == TMUX_FAILURE)
			goto ERROR_HANDLE;

		retval = TMUX_WaitEvent(0, DLC0_TIMEOUT);
		/* wait for the response coming from remote, there are three kind of
		 * possibility here, the first is acknowledge response (UA frame), 
		 * the second is non-acknowledge response(DM frame),
		 * the third is no available response before timeout.
		 */
		/* the case of getting a UA frame in time */
		if (tmux.DLC[0].state == STATE_CONNECTED)
			break;
		/* the case of getting a DM in time */
		if (retval == TMUX_SUCCESS)
			goto ERROR_HANDLE;
		/* in the case of timeout, we should repeat transmission */
	}
	/* handle the case of timeout after third re-transmission */
	if (i == RETRY_TIMES)
		goto ERROR_HANDLE;

	/* success in the establishment of DLC0 */
	for (i = 1; i < MAX_DLC_NUM; i ++)
	{
		tmux.DLC[i].state = STATE_DISCONNECTED;
		TMUX_ResetChannel(i);
	}

	/* releave mutex lock before after leave criticals */
	LEAVE_MUTEX;
	warning("MUX Startup OK\r\n");
	return TMUX_SUCCESS;

ERROR_HANDLE:

	TMUX_ResetMultiplexer();
	tmux.DLC[0].state = STATE_CLOSEDOWN;

ERROR_NOOP:
	warning("MUX Startup failure!\r\n");
	LEAVE_MUTEX;
	return TMUX_FAILURE;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_Closedown
*
*	PURPOSE : closedown mux function, disconnet DLC0 channel with remote.
*	
*	REMARK	: this function maybe block for a while within fixed interval
*			  time.
*
*   PARAM   : void
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

int TMUX_Closedown(void)
{
	int i;
	/* get mutex lock before enter criticals */
	ENTER_MUTEX;

	/* check the state of DLC0, only if on correct state, we could
	 * allow user executing closedown operation.
	 */
	if (tmux.DLC[0].state != STATE_CONNECTED)
		goto ERROR_NOOP;

	tmux.DLC[0].state = STATE_CLOSING;
	for (i = 0; i < RETRY_TIMES; i ++)
	{
		int retval;
		/* envelop a closedown command in UIH frame and send the frame
		 * to remote.
		 */
		if (TMUX_SendFrameData(FRAME_DISC, 0, NULL, 0) == TMUX_FAILURE)
			goto ERROR_HANDLE;

		/* wait for its feedback, if we get a correct response, we
		 * should close all of unclosed channel without notification
		 * to user, and cleanup all the uncompleted state record.
		 */
		retval = TMUX_WaitEvent(0, DLC0_TIMEOUT);

		/* the case of getting a UA frame in time */
		if (tmux.DLC[0].state == STATE_CLOSEDOWN)
			break;
		/* in the case of timeout, we should repeat transmission */
	}
	/* handle the case of timeout after third re-transmission,
	 * if the mux cannot response for closedown command, 
	 * as a strategy, we force it close simplely. */
	if (i == RETRY_TIMES)
		//goto ERROR_HANDLE;
		msgout("MUX remote cannot response for closedown command!\r\n");

	/* cleanup all the DLCs */
	for (i = 0; i < MAX_DLC_NUM; i++)
	{
		tmux.DLC[i].state = STATE_CLOSEDOWN;
		TMUX_ResetChannel(i);
	}
	
	/* reset multiplexer driver */
	TMUX_ResetMultiplexer();

	/* releave mutex lock after leave criticals */
	LEAVE_MUTEX;
	warning("MUX Closedown OK\r\n");
	return TMUX_SUCCESS;

ERROR_HANDLE:
	tmux.DLC[0].state = STATE_CONNECTED;
ERROR_NOOP:
	warning("MUX Closedown failure\r\n");
	LEAVE_MUTEX;
	return TMUX_FAILURE;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_OpenChannel
*
*	PURPOSE : open a specified virtual channel
*
*   PARAM   : name		channel name
*
*	RETURN  : >=0	the handle of DLC
*			  -1	fail to open DLC
*
\*========================================================================*/

int TMUX_OpenChannel(char* name)
{
	int DLCI, len = 0, i;
	UINT8 MSC[10], SNC[10];
	/* analyse the channel name, figure out the channel number */
	if ((DLCI = TMUX_GetDLCIByName(name)) == -1)
		return -1;

	/* get mutex lock before entering criticals */
	ENTER_MUTEX;

	/* check if the specified channel right on a suitable state for
	 * opening. */
	if (tmux.DLC[DLCI].state != STATE_DISCONNECTED)
		goto ERROR_HANDLE;

//	if (tmux.DLC[0].flags & F_FLOWCTRL)
//		goto ERROR_HANDLE;

	/* reset the current channel management data structure, prepareing
	 * for opening up. */
	TMUX_ResetChannel(DLCI);

	tmux.DLC[DLCI].state = STATE_CONNECTING;
	for (i = 0; i < RETRY_TIMES; i++)
	{
		int retval;
		/* the SABM frame to the specified channel */
		if (TMUX_SendFrameData(FRAME_SABM, DLCI, NULL, 0) == TMUX_FAILURE)
			goto ERROR_HANDLE;

		/* wait for the response from remote, to check up if we have 
		 * successfully opened the channel. */
		retval = TMUX_WaitEvent(DLCI, ACK_TIMEOUT);
		if (tmux.DLC[DLCI].state == STATE_CONNECTED)
			break;
		/* the case of getting a DM in time */
		if (retval == TMUX_SUCCESS)
			goto ERROR_HANDLE;
		/* in the case of timeout, we should repeat transmission */
	}
	/* handle the case of timeout after third re-transmission */
	if (i == RETRY_TIMES)
		goto ERROR_HANDLE;

	/* Send DTR & RTS uart signals */
	len = 0;
	MSC[len++] = (UINT8)((DLCI << 2) | 0x02 | EA_BIT);
	MSC[len++] = (UINT8)(RTR_BIT | RTC_BIT | EA_BIT);
	TMUX_SendUIHMessage(UIH_MSC, 1, len, MSC);

#ifdef DATA_SERVICE_CONFIG
	/* we configurate the first DLCI with data service */
	if (DLCI == 1)
	{
		/* set DLC data Service command */
		len = 0;
		SNC[len++] = (UINT8)((DLCI << 2) | 0x02 | EA_BIT);
		SNC[len++] = (UINT8)(SRV_DATA|EA_BIT);
		SNC[len++] = (UINT8)(DC_CS|DC_GPRS|EA_BIT);
		TMUX_SendUIHMessage(UIH_SNC, 1, len, SNC);
	}
#else
	/* Send DLC Service inquiry */
	len = 0;
	SNC[len++] = (UINT8)((DLCI << 2) | 0x02 | EA_BIT);
	TMUX_SendUIHMessage(UIH_SNC, 1, len, SNC);
#endif
#if 0
	/* poll MSC & SNC response on DLC0 */
	tmux.DLC[DLCI].flags |= F_POLLWAIT;
	TMUX_WaitEvent(DLCI, ACK_TIMEOUT*2);
	tmux.DLC[DLCI].flags &= ~F_POLLWAIT;
#endif

	/* releave mutex lock after leaving criticals */
	LEAVE_MUTEX;
	warning("MUX Channel %d open OK\r\n", DLCI);
	/* inform user the result by return value */
	return DLCI;
	
ERROR_HANDLE:
	warning("MUX Channel %d open failure\r\n", DLCI);
	tmux.DLC[DLCI].state = STATE_DISCONNECTED;
	LEAVE_MUTEX;
	return -1;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_CloseChannel
*
*	PURPOSE : close a specified channel
*
*   PARAM   : handle	the handle of the channel to be closed
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

int TMUX_CloseChannel(int handle)
{
	UINT8 MSC[10];
	int len = 0, i;
	/* validaty check up */
	if (handle < 1 || handle > MAX_DLC_NUM)
		return TMUX_FAILURE;

	/* get mutex lock before entering criticals */
	ENTER_MUTEX;

	/* check up the state of the specified channel if we could execute
	 * close operation on it. */
	if (tmux.DLC[handle].state != STATE_CONNECTED)
		goto ERROR_HANDLE;

	/* here we should do something before closing the channel, e.g.,
	 * we should break data connection by send a emulatise break signal
	 * to the channel, cancel currect uncompleted operation on the
	 * channel, clean particular service on the channel.
	 */
	
	/* Send DTR & RTS uart signals */
	MSC[len++] = (UINT8)((handle << 2) | 0x02 | EA_BIT);
	MSC[len++] = (UINT8)(EA_BIT);
	MSC[len++] = (UINT8)((1<<4) | BRK_BIT | EA_BIT); /* break 200ms */
	TMUX_SendUIHMessage(UIH_MSC, 1, len, MSC);
	/* poll MSC response on DLC0 */
	tmux.DLC[handle].flags |= F_POLLWAIT;
	TMUX_WaitEvent(handle, ACK_TIMEOUT);
	tmux.DLC[handle].flags &= ~F_POLLWAIT;

	tmux.DLC[handle].state = STATE_DISCONNECTING;

	for (i = 0; i < RETRY_TIMES; i ++)
	{
		/* we have done all the perparation to the channel, now we can
		* assemble a DISC frame and send it to remote as a disconnect command
		*/
		if (TMUX_SendFrameData(FRAME_DISC, handle, NULL, 0) 
			== TMUX_FAILURE)
			goto ERROR_HANDLE;
		
		/* start a timer and wait for the response from remote, 
		 * once we get a positise response we can complete closing the channel
		 * immediately, however, whatever the result is, we should close
		 * the specified channel, and return to user within limited time.
		 */
		TMUX_WaitEvent(handle, ACK_TIMEOUT);
		
		if (tmux.DLC[handle].state == STATE_DISCONNECTED
			|| tmux.DLC[handle].state == STATE_CLOSEDOWN)
			break;
		/* in the case of timeout, we should repeat transmission */
	}
	/* handle the case of timeout after third re-transmission */
	if (i == RETRY_TIMES)
		goto ERROR_HANDLE;

	/* close channel successfully, we reset the channel */
	TMUX_ResetChannel(handle);
	/* releave mutex lock after leaving criticals */
	LEAVE_MUTEX;
	warning("MUX Channel %d close OK\r\n", handle);
	return TMUX_SUCCESS;

ERROR_HANDLE:
	LEAVE_MUTEX;
	warning("MUX Channel %d close failure\r\n", handle);
	return TMUX_FAILURE;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_WriteChannel
*
*	PURPOSE : sending user data via mux virtual channel
*
*   PARAM   : handle		the handle of channel;
*			  data			the pointer to data buffer;
*			  datalen		the size of the data to be sent;
*
*	RETURN  : the actual number of sending data
*
\*========================================================================*/

int TMUX_WriteChannel(int handle, UINT8* data, int datalen)
{
	int size;
	/* check the validity of parameter */
	if (handle < 1 || handle > MAX_DLC_NUM)
		return -1;
	if (data == NULL || datalen <= 0)
		return -1;

	if (handle == 1)
		msgout("MUX start write data into data channle\r\n");

	/* get mutex lock before entering criticals */
	ENTER_MUTEX;
	if (tmux.DLC[handle].state != STATE_CONNECTED)
	{
		msgout("MUX DLC is not connected\r\n");
		goto ERROR_HANDLE;
	}

	if ((tmux.DLC[0].flags & F_FLOWCTRL)
		|| (tmux.DLC[handle].flags & F_FLOWCTRL)
		|| tmux.flowctrl)
	{
		if (tmux.flowctrl)
			msgout("MUX write disable[UART FC]\r\n");
		if (tmux.DLC[0].flags & F_FLOWCTRL)
			msgout("MUX write disable[DLC0 FC]\r\n");
		if (tmux.DLC[handle].flags & F_FLOWCTRL)
			msgout("MUX write disable[DLCI%d]\r\n", handle);

		goto ERROR_HANDLE;
	}

	/* if the number of user data is above the MAX_FRAME_SIZE, we
	 * should split the data into suitable size, have assemble
	 * data into a UIH frame, start to send the frame.
	 */
	size = (MAX_FRAME_SIZE > datalen ? datalen : MAX_FRAME_SIZE);
	if (TMUX_SendFrameData(FRAME_UIH, handle, data, size) == TMUX_FAILURE)
		size = 0;
	LEAVE_MUTEX;
	return size;

ERROR_HANDLE:
	LEAVE_MUTEX;
	return -1;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_ReadChannel
*
*	PURPOSE : reading remote data via mux virtual channel
*
*   PARAM   : handle		the handle of channel;
*			  buf			the pointer to receiving buffer;
*			  bufsize		the size of data buffer;
*
*	RETURN  : the actual number of reading data
*
\*========================================================================*/

int TMUX_ReadChannel(int handle, UINT8* buf, int bufsize)
{
	int readlen;
	/* parameter validaty check up */
	if (handle < 1 || handle > 3)
		return -1;
	if (buf == NULL || bufsize <= 0)
		return -1;

	ENTER_MUTEX;

	/* we allow user to send data only on CONNETED stage */
	if (tmux.DLC[handle].state != STATE_CONNECTED)
		goto ERROR_HANDLE;

	/* get user data from the reception buffer of this channel */
	readlen = BUF_GetData(&tmux.DLC[handle].rxbuf, buf, bufsize);
	if (readlen == 0)
		readlen = -1;

	LEAVE_MUTEX;
	return readlen;

ERROR_HANDLE:
	LEAVE_MUTEX;
	return -1;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_IoctlChannel
*
*	PURPOSE : make additional I/O control on this channel
*
*   PARAM   : handle		the handle of channel;
*			  opcode		the operation command code;
*			  value			the operation value;
*			  size			the size of operation value;
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

int TMUX_IoctlChannel(int handle, unsigned opcode, int value, int size)
{
	UINT8 MSC[10];
	int len = 0, status = 0;
	/* handle validaty check up */
	if (handle < 1 || handle > 3)
		return TMUX_FAILURE;

	ENTER_MUTEX;

	/* the function is available only under CONNECTED */
	if (tmux.DLC[handle].state != STATE_CONNECTED)
		goto ERROR_HANDLE;

	switch (opcode)
	{
	case TMUX_IOCS_CALLBACK: /* set user callback */
		if (size != sizeof(UART_CALLBACK) || value == 0)
			goto ERROR_HANDLE;
		memcpy(&tmux.DLC[handle].callback, (void*)value, sizeof(UART_CALLBACK));
		if (!BUF_IsEmpty(&tmux.DLC[handle].rxbuf))
			TMUX_CallBack(handle, TMUX_DATAIN);
		break;

	case TMUX_IOCS_DUMPBUF: /* cleanup the RX buffer of the channel */
		BUF_Cleanup(&tmux.DLC[handle].rxbuf);
		break;

	case TMUX_IOCS_MODEMCTL:
		
		if (strcmp((char*)value, "+++") != 0 || size != 3)
			goto ERROR_HANDLE;

		/* transmit break signal */
		MSC[len++] = (UINT8)((handle << 2) | 0x02 | EA_BIT);
		MSC[len++] = (UINT8)(/*RTR_BIT | RTC_BIT | */EA_BIT);
		MSC[len++] = (UINT8)((1<<4) | BRK_BIT | EA_BIT); /* break 200ms */
		if (TMUX_SendUIHMessage(UIH_MSC, 1, len, MSC) 
			== TMUX_FAILURE)
			goto ERROR_HANDLE;
		break;

	case TMUX_IOCG_MODEMSTATUS:
		if (value == 0 || size != sizeof(int))
			goto ERROR_HANDLE;
		status = 0;
		if (tmux.DLC[handle].status & RTC_BIT) /* Data Set Ready */
			status |= TMUX_DSR;
		if (tmux.DLC[handle].status & RTR_BIT) /* Clear to Send */
			status |= TMUX_CTS;
		if (tmux.DLC[handle].status & IC_BIT) /* Ring */
			status |= TMUX_RING;
		if (tmux.DLC[handle].status & DV_BIT) /* Data Carrier Detect */
			status |= TMUX_RLSD;
		*(int*)value = status;
		break;

#ifndef DATA_SERVICE_CONFIG
	case TMUX_IOCS_SERVICE:
		if (size != 1)
			goto ERROR_HANDLE;
		break;

	case TMUX_IOCG_SERVICE:
		if (value == 0 || size != sizeof(int))
			goto ERROR_HANDLE;
		*(int*)value = tmux.DLC[handle].service;
		break;
#endif
	default:
		goto ERROR_HANDLE;
	}	
	LEAVE_MUTEX;
	return TMUX_SUCCESS;

ERROR_HANDLE:
	LEAVE_MUTEX;
	return TMUX_FAILURE;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_OnDatain
*
*	PURPOSE : to handle the incoming data indication
*
*   PARAM   : void
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

int TMUX_OnDatain(void)
{
	/* to have reception buffer of DLC0 as uart reception buffer */
	static UINT8*	  buf = tmux.DLC[0].rxbuf.buf; 
	static int		  esc_tag = 0, datalen = 0;
	static UINT8	  fcs = 0xff;
	int		readlen, i, esc_flag;

	/* read data though serial interface */
	while ((readlen = TMUX_ReadDevice(buf, BUF_SIZE)) >= 0)
	{
		/* process incoming data */
		if (readlen == 0)
			continue;

#ifdef DATAIO_DEBUG
		{
		msgout("\r\nMUX recv:\r\n");
		for (i = 0; i < readlen; i++)
			msgout("%02x ", (unsigned)buf[i]);
		msgout("\r\n\r\n");
		}
#endif /* DATAIO_DEBUG */
		
		for (i = 0; i < readlen; )
		{
			esc_flag = 0;
			/* transparency process */
			if (esc_tag == 0) /* in normal case */
			{
				/* current charactor is a escape char */
				if (buf[i] == HDLC_ESCAPE) 
				{
					esc_tag = 1; /* set escape flag */
					i ++;		 /* chop this char */
					continue;
				}
			}
			else /* in escape case */
			{
				/* in error case : under escape situation, 
				 * cannot 0x7e and 0x20 followed */
				if (buf[i] == HDLC_FLAG || buf[i] == HDLC_ESCAPE)
					tmux.scanstate;
				else
				{   /* recover encoded char */
					buf[i] = (UINT8)(buf[i]^HDLC_SPCHAR);
					esc_tag = 0;
					if (buf[i] == HDLC_FLAG)
						esc_flag = 1;
				}
			}

			switch (tmux.scanstate)
			{
			case S_FLAG:
				/* if we get HDLC flags during the remote power saving stage,
				 * that means remote station has been recover to active. */
				if (tmux.DLC[0].flags & F_REMOTESLEEP)
					tmux.DLC[0].flags &= ~F_REMOTESLEEP;
				/* if we get HDLC flags during the local power saving stage,
				 * that means remote want to wake me up, okay we response him. */
				if (tmux.DLC[0].flags & F_LOCALSLEEP)
				{
					UINT8 octet = HDLC_FLAG;
					TMUX_WriteDevice(&octet, 1);
				}

				if (buf[i] == HDLC_FLAG)
				{
					tmux.scanstate = S_ADDR;
					datalen = 0;
					esc_tag = 0;
					fcs = 0xff;
				}
				else
					warning("MUX discard char [%c] !!!\r\n", buf[i]);
					//warning("MUX discard char [%02x] !!!\r\n", (UINT8)buf[i]);
				i++;
				break;
			case S_ADDR:
				if (buf[i] == HDLC_FLAG)
				{
					tmux.scanstate = S_FLAG;
					break;
				}
				tmux.frame.DLCI = buf[i] >> 2;
				tmux.frame.CR_bit = (buf[i] & CR_BIT) >> 1;
				if (tmux.frame.DLCI >= 0 && tmux.frame.DLCI <= 63 && (buf[i] & EA_BIT))
				{
					tmux.scanstate = S_CTRL;
					fcs = fcstab8[fcs^buf[i++]];
				}
				else
					tmux.scanstate = S_FLAG;
				break;
			case S_CTRL:
				if (buf[i] == HDLC_FLAG)
				{
					tmux.scanstate = S_FLAG;
					break;
				}
				tmux.frame.type = (buf[i] & ~PF_BIT);
				tmux.frame.PF_bit = (buf[i] & PF_BIT);
				if (tmux.frame.type == FRAME_SABM
					|| tmux.frame.type == FRAME_UA
					|| tmux.frame.type == FRAME_DM
					|| tmux.frame.type == FRAME_DISC
					|| tmux.frame.type == FRAME_UIH)
				{
					tmux.scanstate = S_DATA;
					fcs = fcstab8[fcs^buf[i++]];
				}
				else
					tmux.scanstate = S_FLAG;
				break;
			case S_DATA:
				if (buf[i] == HDLC_FLAG && esc_flag == 0)
				{
					/* erase a FCS char and a additional char */
					tmux.frame.datalen = datalen - 1;
					/* calculate FCS */
					if ((UINT8)(0xff - fcs) == tmux.frame.data[datalen - 1])
					{
						tmux.DLC[0].flags &= ~F_LOCALSLEEP;
						/* process a completed frame */
						ENTER_MUTEX;
						TMUX_ProcRxFrame(&tmux.frame);
						LEAVE_MUTEX;
						i ++;
					}
					tmux.scanstate = S_FLAG;
					break;
				}
				/* the length of information field and 1 FCS octet */
				if (datalen > MAX_FRAME_SIZE + 1) 
				{
					warning("MUX the length of information field exceed the limit!\r\n");
					tmux.scanstate = S_FLAG;
				}
				else /* read data octet one by one */
					tmux.frame.data[datalen++] = buf[i++];
				break;
			default:
				tmux.scanstate = S_FLAG;
			}
		}
	}
	return TMUX_SUCCESS;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_OnDataout
*
*	PURPOSE : to handle the outgoing data indication
*
*   PARAM   : void
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

int TMUX_OnDataout(void)
{
	ENTER_MUTEX;

	if (tmux.DLC[0].flags & F_WAKEUPREMOTE)
	{
		if (tmux.DLC[0].flags & F_REMOTESLEEP)
		{
			/* continue to wake up remote station */
			TMUX_WakeupRemote();
			LEAVE_MUTEX;
			return TMUX_SUCCESS;
		}
		else
		{
			/* get rid of WAKUP flag, continue to send data */
			tmux.DLC[0].flags &= ~F_WAKEUPREMOTE;
		}
	}

	/* transmit the rest data in output buffer */
	if (!BUF_IsEmpty(&tmux.DLC[0].rxbuf))
	{
		/* start to transmit data in buffer */
		BUF_Transmit(&tmux.DLC[0].rxbuf);
		if (BUF_IsEmpty(&tmux.DLC[0].rxbuf))
		{
			/* clear uart flow control */
			msgout("MUX UART flowctrl off\r\n");
			tmux.flowctrl = 0;
			if ((tmux.DLC[0].flags & F_FLOWCTRL) == 0)
			{
				int i;
				for (i = 1; i < MAX_DLC_NUM; i ++)
				{
					if ((tmux.DLC[i].flags & F_FLOWCTRL) == 0)
						TMUX_CallBack(i, TMUX_DATAOUT);
					if (tmux.DLC[i].flags & F_POLLING)
						tmux.DLC[i].flags &= ~F_POLLING;
				}
			}
		} /* if (tmux.datalen == 0) */
	} /* while (tmux.datalen > 0) */

	LEAVE_MUTEX;
	return TMUX_SUCCESS;
}

/**************************************************************************\
*
*	INTERNAL FUNCTION IMPLEMENTATION
*	(TASK|WINDOW|CALLBACK)
*
\**************************************************************************/

/*========================================================================*\
*
*	FUNCTION: TMUX_GetDLCIByName
*
*	PURPOSE : get DLC identifier according to channel name
*
*   PARAM   : channel_name	name of channel
*
*	RETURN  : -1	bad channel name
*			  >1	channel identifier
*
\*========================================================================*/

static int TMUX_GetDLCIByName(char* channel_name)
{
	int pix_len = strlen(TMUX_DLCNAME);
	int DLCI;
	if (channel_name == NULL)
		return -1;
	if (strncmp(channel_name, TMUX_DLCNAME, pix_len) != 0)
		return -1;
	DLCI = atoi(channel_name + pix_len);
	if (DLCI < 1 || DLCI > MAX_DLC_NUM)
		return -1;
	return DLCI;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_SendPollQueue
*
*	PURPOSE : Send the poll data frame wait queue.
*
*   PARAM   : DLCI		Data Link Connection Identifier
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

static int TMUX_SendPollQueue(int DLCI)
{
	POLLQ* ptr;
	int retval;

	/* the queue is empty */
	if ((ptr = tmux.DLC[DLCI].pollque) == NULL)
		return TMUX_SUCCESS;

	/* It is polling now, wait */
	if (tmux.DLC[DLCI].flags & F_POLLING)
		return TMUX_SUCCESS;

	/* get up to maximum retrty times */
	if (tmux.DLC[DLCI].retry == 0)
	{
		/* ??? maybe we should do something, like callback, 
		 * to notice the failure of user former command. */

		ptr = tmux.DLC[DLCI].pollque;
		/* release queue header */
		tmux.DLC[DLCI].pollque = ptr->next;
		free(ptr);
		if (tmux.DLC[DLCI].pollque != NULL)
		{
			/* reset the retry number */
			tmux.DLC[DLCI].retry = RETRY_TIMES;
			ptr = tmux.DLC[DLCI].pollque;
		}
		else
		{
			int i;
			/* wake up the threads who wait for poll event */
			for (i = 0; i < MAX_DLC_NUM; i++)
				if (tmux.DLC[i].flags & F_POLLWAIT)
					LIB_SignalEvent(tmux.DLC[i].event, 1);
			/* poll queue is empty */
			return TMUX_SUCCESS;
		}
	}
	tmux.DLC[DLCI].retry--;
	retval = TMUX_SendFrameData(FRAME_UIH, DLCI, 
		ptr->data, ptr->datalen);
	if (retval == TMUX_ASYNC)
		tmux.DLC[DLCI].flags |= F_POLLING;

	/* restart timer */
	LIB_StartTimer(&tmux.DLC[DLCI].timer);

	return retval;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_SendUIHMessage
*
*	PURPOSE : send a UIH message to DLC0
*
*   PARAM   : msg_type		the type of UIH message
*			  cmd_tag		UIH message to be sent as a command
*			  msg_len		the length of message
*			  msg_data		the start position of message data
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

/**************************************************************************
 * UIH message 
 * format: | type | length | value 1 | value 2 | ... ... | value n |
 * type  : T6 T5 T4 T3 T2 T1 CR EA
 * length: L7 L6 L5 L4 L3 L2 L1 EA
 **************************************************************************
 *
 * PN	(DLC Parameter Negotiation)
 * type = 0x20 ; length = 8
 * format: | type | length | value1 | value2 | ... | value8 |
 *
 * PSC	(Power Saving Control)
 * type = 0x10 ; length = 0
 * format: | type | length |
 *
 * CLD	(MUX Close Down)
 * type = 0x30 ; length = 0
 * format: | type | length |
 *
 * Test	(Test Command)
 * type = 0x08 ; length = n
 * format
 * format: | type | length | value1 | value2 | ... | value n |
 *
 * FCon	(Flow Control on)
 * type = 0x28 ; length = 0
 *  format: | type | length |
 *
 * FCoff(Flow Control off)
 * type = 0x18 ; length = 0
 * format: | type | length |
 *
 * MSC	(Modem Status Command)
 * type = 0x38 ; length = 2, 3
 * format: | type | length | DLCI | v.24 signals | break signal(optional) |
 * DLCI  : I6 I5 I4 I3 I2  I1  01 EA
 * v.24  : DV IC 00 00 RTR RTC FC EA
 * break : L4 L3 L2 L1 B3  B2  B1 EA
 * 
 * NSC	(Non Supported Command Response)
 * type = 0x04 ; length = 1
 * format: | type | length | unsupported type |
 * u type: T6 T5 T4 T3 T2 T1 CR EA
 *
 * RPN	(Remote Port Negotiation Command)
 * type = 0x24 ; length = 1, 8
 * format: | type | length | DLCI | Option1 | Option2 | ... | Option7 |
 * DLCI  : I6 I5 I4 I3 I2 I1 01 EA
 * 
 * RLS	(Remote Line Status Command)
 * type = 0x14 ; length = 2
 * format: | type | length | DLCI | Status |
 * DLCI  : I6 I5 I4 I3 I2 I1 CR EA
 * Status: 00 00 00 00 L4 L3 L2 L1
 * 
 * SNC	(Service Negotiation Command)
 * type = 0x34 ; length = 1, 3
 * format: | type | length | DLCI | Service | Voice Codec |
 * DLCI  : I6 I5 I4 I3 I2 I1 CR EA
 * Srv   : S7 S6 S5 S4 S3 S2 S1 EA    (S1:Data; S2:Voice)
 * Voice : V7 V6 V5 V4 V3 V2 V1 EA
 */ 

static int TMUX_SendUIHMessage(UINT8 msg_type, int cmd_tag,
							   int msg_len, UINT8* msg_data)
{
	UINT8 buf[MAX_FRAME_SIZE];
	int i, retval;
	/* too long */
	if (msg_len + 2 > MAX_FRAME_SIZE)
		return TMUX_FAILURE;

	for (i = 0; UIH_msg[i].name != NULL; i++)
		if (msg_type == UIH_msg[i].type)
			break;
	/* unknown message */
	if (UIH_msg[i].name == NULL)
		return TMUX_FAILURE;
	/* unsupported message */
	if (UIH_msg[i].mask & tmux.unsupport)
		return TMUX_FAILURE;

	i = 0;
	buf[i++] = (UINT8)((msg_type << 2) | (cmd_tag?CR_BIT:0) | EA_BIT);
	buf[i++] = (UINT8)((msg_len << 1) | EA_BIT);
	memcpy(buf+i, msg_data, msg_len);
	i += msg_len;

	if (cmd_tag == 0)
	{
		retval = TMUX_SendFrameData(FRAME_UIH, 0, buf, i);
	}
	else
	{
		/* if the frame is UIH and it contains a poll data,
		 * we should record the data and append it to poll data queue. */
		POLLQ* ptr = (POLLQ*)malloc(sizeof(POLLQ) + i);
		if (ptr == NULL)
		{
			warning("MUX fail to malloc poll data memory!\r\n");
			return TMUX_FAILURE;
		}
		memset(ptr, 0, sizeof(POLLQ) + i);
		ptr->next = tmux.DLC[0].pollque;
		ptr->msg_type = msg_type;
		if (msg_type == UIH_MSC			/* Modem status Command */
			|| msg_type == UIH_RPN		/* Remote Port Negotiation Command */
			|| msg_type == UIH_RLS		/* Remote Line status Command */
			|| msg_type == UIH_SNC)		/* service Negotiation Command */
		{
			ptr->DLCI = (UINT8)(msg_data[0] >> 2);
		}
		ptr->datalen  = (UINT8)i;
		memcpy(ptr->data, buf, i);

		/* append the poll data to poll queue */
		tmux.DLC[0].pollque = ptr;
		tmux.DLC[0].retry = RETRY_TIMES;

		retval = TMUX_SendPollQueue(0);
	}
	return retval;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_Transparence
*
*	PURPOSE : implement HDLC transparence mechanism
*
*   PARAM   : cp		the position of starting to write
*			  octet		the octect to be transparenized
*
*	RETURN  : the new position after transparence
*
\*========================================================================*/

static UINT8* TMUX_Transparence(UINT8* cp, UINT8 octet)
{
	if (octet == HDLC_FLAG || octet == HDLC_ESCAPE
		//|| octet < HDLC_SPCHAR
		|| octet == SP_XON || octet == SP_XOFF)
	{
		*cp++ = HDLC_ESCAPE;
		*cp++ = (UINT8)(octet^HDLC_SPCHAR);
	}
	else
		*cp++ = octet;
	return cp;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_WakeupRemote
*
*	PURPOSE : wake up remote station frame power saving mode
*
*   PARAM   : void
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

static int TMUX_WakeupRemote(void)
{
	UINT8 buf[64];
	memset(buf, HDLC_FLAG, sizeof(buf));
	while (TMUX_WriteDevice(buf, sizeof(buf)) > 0)
		;
	return TMUX_SUCCESS;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_SendFrameData
*
*	PURPOSE : put a frame into tmux.txbuf, start to transmit data if
*			  without flow control.
*
*   PARAM   : frametype		the type of MUX frame, e.g. SABM, DISC, UIH
*			  DLCI			the identifier of DLC, range from 0 to MAX_DLC_NUM
*			  data			frame information
*			  datalen		the size of information
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*			  TMUX_ASYNC	asynchorous transmitted
*
\*========================================================================*/

/**************************************************************************
 * MUX Frame
 * format: | flag | addr | ctrl | infor.(n octets) | FCS | flag |
 * flag  : 0x7e
 * addr  : I6 I5 I4 I3 I2 I1 CR EA
 * ctrl  : T7 T6 T5 PF T4 T3 T2 T1
 * infor : 0 - n octets
 * FCS   : addrress & control field calculated FCS
 * flag  : 0x7e
 **************************************************************************
 */

static int TMUX_SendFrameData(UINT8 frametype, int DLCI, 
							  UINT8* data, int datalen)
{
	UINT8 buf[MAX_FRAME_SIZE * 2 + 10];
	UINT8 *cp = buf;
	int writelen, len = 0;
	int CR_mask = 0, PF_mask = 0;
	UINT8 addr, ctrl, fcs = 0xff;

	if (DLCI < 0 || DLCI > MAX_DLC_NUM)
		return TMUX_FAILURE;

	if (frametype == FRAME_SABM || frametype == FRAME_DISC)
	{
		if (data != NULL || datalen != 0)
			return TMUX_FAILURE;
		CR_mask = CR_BIT;
		PF_mask = PF_BIT;
	}
	else if (frametype == FRAME_UIH)
	{
		if (data == NULL || datalen < 1 || datalen > MAX_FRAME_SIZE)
			return TMUX_FAILURE;
		if (DLCI == 0)
		{
			if ((data[0] & EA_BIT) == 0)
				return TMUX_FAILURE;
			if (data[0] & CR_BIT)
			{
				CR_mask = CR_BIT;
				PF_mask = PF_BIT;
			}
		}
		else
		{
			/* initator should set CR bit on the 
			 * address field of UIH frame */
			CR_mask = CR_BIT;
		}
	}
	else
		return TMUX_FAILURE;

	/* start to load frame data */

	/* HDLC start flag */
	*cp++ = HDLC_FLAG;
	/* HDLC address field */
	addr = (UINT8)((DLCI << 2) | CR_mask | EA_BIT);
	fcs = fcstab8[fcs^addr];
	cp = TMUX_Transparence(cp, addr);
	/* HDLC control field */
	ctrl = (UINT8)(frametype | PF_mask);
	fcs = fcstab8[fcs^ctrl];
	cp = TMUX_Transparence(cp, ctrl);
	/* append information field if necessary */
	if (data != NULL)
	{
		for (len = 0; len < datalen; len++)
			cp = TMUX_Transparence(cp, data[len]);
	}
	/* HDLC FCS field */
	fcs = (UINT8)(0xff - fcs);
	cp = TMUX_Transparence(cp, fcs);
	/* HDLC stop flag */
	*cp++ = HDLC_FLAG;

	/* calculate the data length */
	len = cp - buf;
	cp = buf;

	/* if remote is on the power saving stage, we have wakeup him firstly */
	if (tmux.DLC[0].flags & F_REMOTESLEEP)
	{
		tmux.DLC[0].flags |= F_WAKEUPREMOTE;
		TMUX_WakeupRemote();
	}
	/* to transmit data directly if without setting flow control */
	else if (tmux.flowctrl == 0)
	{
		while (len > 0)
		{
			writelen = TMUX_WriteDevice(cp, len);
			if (writelen < 0)
				break;
			len -= writelen;
			cp += writelen;
		}
	}
	/* the rest data should save in output buffer, 
	 * for asynchorous transmission and set flow control. */
	if (len > 0)
	{
		if (BUF_SIZE - tmux.DLC[0].rxbuf.datalen - 1 < len)
			return TMUX_FAILURE;
		/* put data in buffer */
		BUF_PutData(&tmux.DLC[0].rxbuf, cp ,len);
		msgout("MUX UART flowctrl on\r\n");
		tmux.flowctrl = 1;
		return TMUX_ASYNC;
	}

	return TMUX_SUCCESS;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_ProcMSCChange
*
*	PURPOSE : Process MSC message with changed status signals
*
*   PARAM   : DLCI		DLC identifier
*			  status	the current(new) status
*			  ostatus	the old status
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

static int TMUX_ProcMSCChange(int DLCI, int status, int ostatus)
{
	int i, diff_status = (UINT8)(status ^ ostatus);
	if (DLCI == 0)
	{
		msgout("MUX MSC message cannot change the status relating to DLC0\r\n");
		return TMUX_FAILURE;
	}
	/* if no status changed */
	if (diff_status == 0)
		return TMUX_SUCCESS;
	msgout("MUX ");
	for (i = 0; MSC_signal[i].rx_v24 != NULL; i ++)
	{
		if (MSC_signal[i].mask & diff_status)
		{
			if (MSC_signal[i].mask & status)
				msgout("+%s ", MSC_signal[i].rx_v24);
			else
				msgout("-%s ", MSC_signal[i].rx_v24);
		}
	}
	msgout("\r\n");
	//???
	if (diff_status & (FC_BIT|/*RTC_BIT|*/RTR_BIT))
	{
		int new_fc = 0;
		/* update flow control */
		if ((status & FC_BIT) || /*!(status & RTC_BIT) ||*/ !(status & RTR_BIT))
			new_fc |= F_FLOWCTRL;
		if (new_fc ^ tmux.DLC[DLCI].flags)
		{
			if (tmux.DLC[DLCI].flags & F_FLOWCTRL)
			{
				/* clear flow control on this DLC */
				msgout("DLC flowctrl off\r\n");
				tmux.DLC[DLCI].flags &= ~F_FLOWCTRL;
				/* ask user continue to send data */
				if (tmux.flowctrl == 0 
					&& (tmux.DLC[0].flags & F_FLOWCTRL) == 0)
					TMUX_CallBack(DLCI, TMUX_DATAOUT);
			}
			else
			{
				msgout("DLC flowctrl on\r\n");
				//???
				tmux.DLC[DLCI].flags |= F_FLOWCTRL;
			}
		}
	}
	if (diff_status & (IC_BIT|DV_BIT))
	{
		/* callback modem status change */
		TMUX_CallBack(DLCI, TMUX_MODEM);
	}		
	return TMUX_SUCCESS;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_ProcRxUIH
*
*	PURPOSE : Process received UIH message
*
*   PARAM   : frame		the received UIH frame include DLC0 message data
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

static int TMUX_ProcRxUIH(FRAME *frame)
{
	int msg_type, msg_len, DLCI, ostatus, i, tag = 0;
	UINT8* msg_data = NULL;
	/* message validate check */
	/* 1 octet message type field & 1 octet message length field 
	 * must be existed */
	if (frame->datalen < 2)
	{
		msgout("MUX get a in non integrated UIH frame through DLC0!\r\n");
		return TMUX_FAILURE;
	}
	/* analyse message type field */
	if ((frame->data[0] & EA_BIT) == 0)
	{
		msgout("MUX get a UIH frame on DLC0 without EA bit on message type field\r\n");
		return TMUX_FAILURE;
	}
	msg_type = frame->data[0] >> 2;
	/* analyse message length field */
	if ((frame->data[1] & EA_BIT) == 0)
	{
		msgout("MUX get a UIH frame on DLC0 without EA bit on message length field, "
			"that will be too long\r\n");
		return TMUX_FAILURE;
	}
	msg_len = frame->data[1] >> 1;
	if (msg_len + 2 != frame->datalen)
	{
		msgout("MUX get a UIH frame on DLC0 with mismatche length\r\n");
		return TMUX_FAILURE;
	}
	if (msg_len > 0)
		msg_data = &frame->data[2];

	/* process messages */
	if (frame->data[0] & CR_BIT)
	{
		/* known as a command message */
		switch (msg_type)
		{
		case UIH_PSC:/* Power saving Control (length==0) */
			if (msg_len != 0)
				msgout("MUX get PSC command with non zero length\r\n");
			TMUX_SendUIHMessage(UIH_PSC, 0, 0, NULL);
			/* report user Power saving indiction */
			msgout("MUX get LOCALSLEEP\r\n");
			tmux.DLC[0].flags |= F_LOCALSLEEP;
			break;
		case UIH_FCon:/* Flow Control on  (length==0) */
			/* set flow control */
			msgout("MUX get FCon on DLC-0\r\n");
			TMUX_SendUIHMessage(UIH_FCon, 0, 0, NULL);
			tmux.DLC[0].flags &= ~F_FLOWCTRL;
			if (tmux.flowctrl == 0)
			{
				for (i = 1; i < MAX_DLC_NUM; i ++)
				{
					if ((tmux.DLC[i].flags & F_FLOWCTRL) == 0)
						TMUX_CallBack(i, TMUX_DATAOUT);
				}
			}
			break;
		case UIH_FCoff:/* Flow Control off (length==0) */
			/* clean flow control */
			msgout("MUX get FCoff on DLC-0\r\n");
			TMUX_SendUIHMessage(UIH_FCoff, 0, 0, NULL);
			tmux.DLC[0].flags |= F_FLOWCTRL;
			break;
		case UIH_MSC:/* Modem status Command */
			if (msg_len < 2)
			{
				msgout("MUX get a invalid MSC command!\r\n");
				return TMUX_FAILURE;
			}
			if ((msg_data[0] & 0x03) != 0x03)
			{
				msgout("MUX get a MSC command with a invalid DLCI field!\r\n");
				return TMUX_FAILURE;
			}
			msgout("MUX get MSC command\r\n");
			DLCI = (msg_data[0] >> 2);
			ostatus = tmux.DLC[DLCI].status;
			tmux.DLC[DLCI].status = msg_data[1];
			/* update the recorded status of remote station */
			TMUX_SendUIHMessage(UIH_MSC, 0, msg_len, msg_data);
			msgout("MUX send MSC response\r\n");
#if 1
			{	//???
				int len = 0;
				UINT8 MSC[20];
			len = 0;
			MSC[len++] = (UINT8)((DLCI << 2) | 0x02 | EA_BIT);
			MSC[len++] = (UINT8)(RTR_BIT | RTC_BIT | EA_BIT);
			TMUX_SendUIHMessage(UIH_MSC, 1, len, MSC);
			}
#endif
			/* process DLC status change */
			TMUX_ProcMSCChange(DLCI, tmux.DLC[DLCI].status, ostatus);
			break;
		default:
			/* unsupported UIH frame */
			for (i = 0; UIH_msg[i].name != NULL; i++)
				if (msg_type == UIH_msg[i].type)
					break;
			if (UIH_msg[i].name != NULL)
				msgout("MUX get a unsupported UIH command[%d]\r\n", UIH_msg[i].name);
			else
				msgout("MUX get a unsupported UIH command[? %02x]\r\n", msg_type);

			/* response a NSC frame */
			TMUX_SendUIHMessage(UIH_NSC, 0, 1, frame->data);
			break;
		}
	}
	else
	{
		POLLQ *ptr = tmux.DLC[frame->DLCI].pollque;
		/* known as a respond message */
		switch (msg_type)
		{
		case UIH_PSC:/* Power saving Control (length==0) */
			if (msg_len != 0)
				msgout("MUX get PSC respond with non zero length\r\n");
			msgout("MUX get REMOTESLEEP\r\n");
			/* enter power saving mode */
			tmux.DLC[0].flags |= F_REMOTESLEEP;
			break;
		case UIH_CLD:/* MUX Close Down   (length==0) */
			if (msg_len != 0)
				msgout("MUX get CLD respond with non zero length\r\n");
			
			msgout("MUX get CLoseDown\r\n");
			/* cleanup all the allocated resource */
			for (i = 1; i < MAX_DLC_NUM; i ++) /* close all channel */
			{
				/* Q(03) 应该做完处理后再将状态置成STATE_CLOSEDOWN */
				//tmux.DLC[i].state = STATE_CLOSEDOWN;
				switch (tmux.DLC[i].state)
				{
				case STATE_CONNECTING:
					LIB_SignalEvent(tmux.DLC[i].event, 1);
					break;
				case STATE_CONNECTED:
					TMUX_ResetChannel(i);
					TMUX_CallBack(i, TMUX_MODEM);
					break;
				case STATE_DISCONNECTING:
					LIB_SignalEvent(tmux.DLC[i].event, 1);
					break;
				}
				/* Q(03) */
				tmux.DLC[i].state = STATE_CLOSEDOWN;
			}

			if (tmux.DLC[0].state == STATE_CLOSING)
				tag = 1;
			tmux.DLC[0].state = STATE_CLOSEDOWN;
			if (tag == 1)
				LIB_SignalEvent(tmux.DLC[0].event, 1);
			else
				TMUX_ResetMultiplexer();
			break;
		case UIH_MSC:/* Modem status respond */
			msgout("MUX ************get MSC response\r\n");
			break;
		case UIH_NSC:/* Non supported Command Response */
			if (msg_len != 1)
			{
				msgout("MUX get a SNC with invalid message length = %d\r\n", msg_len);
				return TMUX_FAILURE;
			}
			/* record remote station's unsupported UIH message */
			for (i = 0; UIH_msg[i].name != NULL; i++)
				if ((msg_data[0] >> 2) == UIH_msg[i].type)
					break;
			if (UIH_msg[i].name != NULL)
			{
				if ((tmux.unsupport & UIH_msg[i].mask) == 0)
				{
					msgout("MUX remote unsupported UIH message[%s]\r\n", UIH_msg[i].name);
					tmux.unsupport |= UIH_msg[i].mask;
				}
			}
			else
				msgout("MUX remote unsupports a unknown UIH message[%02x]\r\n", msg_data[0]);
			break;
		case UIH_SNC:/* Service Negotiation respond */
			/* record supported service though this channel */
			break;
		default:
			/* unsupported UIH frame */
			for (i = 0; UIH_msg[i].name != NULL; i++)
				if (msg_type == UIH_msg[i].type)
					break;
			if (UIH_msg[i].name != NULL)
				msgout("MUX get a unsupported UIH respond[%d]\r\n", UIH_msg[i].name);
			else
				msgout("MUX get a unsupported UIH respond[? %02x]\r\n", msg_type);
			break;
		}
		/* if get response message or unsupported response,
		 * do next poll frame */
		if ((frame->PF_bit && ptr && ptr->msg_type == msg_type)
			|| (msg_type == UIH_NSC && ptr && ptr->msg_type == (msg_data[0] >> 2)))
		{
			if (msg_type == UIH_MSC			/* Modem status Command */
				|| msg_type == UIH_RPN		/* Remote Port Negotiation Command */
				|| msg_type == UIH_RLS		/* Remote Line status Command */
				|| msg_type == UIH_SNC)		/* service Negotiation Command */
			{
				if (ptr->DLCI != frame->data[2] >> 2)
					return TMUX_SUCCESS;
			}
			/* stop current timer */
			LIB_StopTimer(&tmux.DLC[frame->DLCI].timer);
			/* disable current poll */
			tmux.DLC[frame->DLCI].retry = 0;
			/* send data in poll queue */
			TMUX_SendPollQueue(frame->DLCI);
		}
	}
	return TMUX_SUCCESS;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_ProcRxFrame
*
*	PURPOSE : process received MUX frame
*
*   PARAM   : frame		MUX frame structure
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

static int TMUX_ProcRxFrame(FRAME *frame)
{
	if (tmux.DLC[0].flags & F_LOCALSLEEP)
	{
		/* now we have been successfully get a completed MUX frame,
		 * we are active now. */
		tmux.DLC[0].flags &= ~F_LOCALSLEEP;
	}
	switch (frame->type)
	{
	case FRAME_SABM:
	case FRAME_DISC:
		msgout("MUX cannot receive SABM & DISC frame!\r\n");
		break;
	case FRAME_UA:
		/* in the case where a UA response is received with the P bit
		 * set to 0 then the received frame shall be discarded. */
		if (frame->PF_bit == 0)
		{
			msgout("MUX receive a UA frame with P bit set to 0, "
				"discarded!!!\r\n");
			break;
		}
		if (tmux.DLC[frame->DLCI].state == STATE_CONNECTING)
		{
			tmux.DLC[frame->DLCI].state  = STATE_CONNECTED;
			LIB_SignalEvent(tmux.DLC[frame->DLCI].event, 1);
		}
		else if (tmux.DLC[frame->DLCI].state == STATE_DISCONNECTING)
		{
			tmux.DLC[frame->DLCI].state  = STATE_DISCONNECTED;
			LIB_SignalEvent(tmux.DLC[frame->DLCI].event, 1);
		}
		else if (tmux.DLC[frame->DLCI].state == STATE_DISCONNECTING
			|| tmux.DLC[frame->DLCI].state == STATE_CLOSING)
		{
			tmux.DLC[frame->DLCI].state  = STATE_CLOSEDOWN;
			LIB_SignalEvent(tmux.DLC[frame->DLCI].event, 1);
		}
		break;
	case FRAME_DM:
		if (tmux.DLC[frame->DLCI].state == STATE_CONNECTING)
		{
			if (frame->PF_bit == 0)
			{
				msgout("MUX received a DM frame with P bit set to 0, "
					"on CONNECTING stage\r\n");
				break;
			}
			tmux.DLC[frame->DLCI].state  = STATE_DISCONNECTED;
			LIB_SignalEvent(tmux.DLC[frame->DLCI].event, 1);
		}
		else if (tmux.DLC[frame->DLCI].state == STATE_CONNECTED)
		{
			if (frame->PF_bit == 1)
			{
				msgout("MUX received a DM frame with P bit set to 1, "
					"on CONNECTED stage\r\n");
			}
			tmux.DLC[frame->DLCI].state  = STATE_DISCONNECTED;
			TMUX_ResetChannel(frame->DLCI);
			TMUX_CallBack(frame->DLCI, TMUX_MODEM);
		}
		break;
	case FRAME_UIH:
		if (frame->DLCI == 0)
			TMUX_ProcRxUIH(frame);
		else
		{
			int tag = BUF_IsEmpty(&tmux.DLC[frame->DLCI].rxbuf);
			if (frame->CR_bit != 0)
				msgout("MUX responder should clear C/R bit "
						"in address field of UIH frame");
			/* append received data to DLC recv buffer */
			BUF_PutData(&tmux.DLC[frame->DLCI].rxbuf, frame->data, frame->datalen);
			if (tag)
				TMUX_CallBack(frame->DLCI, TMUX_DATAIN);
		}
		break;
	default:
		msgout("MUX get a unknow frame[%02x]\r\n", frame->type);
		break;
	}
	return TMUX_SUCCESS;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_WaitEvent
*
*	PURPOSE : synchorous wait event
*
*   PARAM   : DLCI			DLC identifier
*			  millisecond	the time for waiting
*
*	RETURN  : TMUX_SUCCESS	succuessful, event occured
*			  TMUX_FAILURE	failure, time expired
*
\*========================================================================*/

static int TMUX_WaitEvent(int DLCI, int millisecond)
{
	/* clear timeout flag */
	tmux.DLC[DLCI].flags &= ~F_TIMEOUT;

	/* configurate timer */
	LIB_SetTimerFunc(&tmux.DLC[DLCI].timer, TMUX_TimeOut, &tmux.DLC[DLCI]);
	LIB_SetTimerExpire(&tmux.DLC[DLCI].timer, millisecond);

	/* start timer */
	LIB_StartTimer(&tmux.DLC[DLCI].timer);

	LEAVE_MUTEX;
	LIB_WaitEvent(tmux.DLC[DLCI].event);
	ENTER_MUTEX;

	/* terminate the timer */
	LIB_StopTimer(&tmux.DLC[DLCI].timer);

	/* the time expire case */
	if (tmux.DLC[DLCI].flags & F_TIMEOUT)
		return TMUX_FAILURE;

	return TMUX_SUCCESS;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_TimeOut
*
*	PURPOSE : callback entry of timer expired
*
*   PARAM   : DLC	DLC management data structure
*
*	RETURN  : void
*
\*========================================================================*/

static void TMUX_TimeOut(void* channel)
{
	CHANNEL* DLC = (CHANNEL*)channel;
	ENTER_MUTEX;
	if (DLC->state != STATE_CONNECTED)
	{
		/* wake up wait event */
		if (DLC->state == STATE_CONNECTING
			|| DLC->state == STATE_DISCONNECTING
			|| DLC->state == STATE_CLOSING)
		{
			/* set timeout flag */
			DLC->flags |= F_TIMEOUT;
			LIB_SignalEvent(DLC->event, 1);
		}
		LEAVE_MUTEX;
		return;
	}
	if ((DLC->flags & F_POLLING) == 0)
		TMUX_SendPollQueue(DLC->DLCI);

	LEAVE_MUTEX;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_CallBack
*
*	PURPOSE : user callback function
*
*   PARAM   : DLCI		DLC identifier
*			  event		callback event, e.g. DATAIN, DATAOUT, BROKEN
*
*	RETURN  : void
*
\*========================================================================*/

static void TMUX_CallBack(int DLCI, unsigned int event)
{	
	if (tmux.DLC[DLCI].callback.function != NULL)
	{
		if (tmux.DLC[DLCI].callback.EventCode & event)
		{
			if (event == TMUX_DATAOUT)
				msgout("DLCI%d DATAOUT\r\n", DLCI);
			LEAVE_MUTEX;
			tmux.DLC[DLCI].callback.function
				((long)tmux.DLC[DLCI].callback.para, event, DLCI, 0L);
			ENTER_MUTEX;
		}
	}	
	return;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_ResetChannel
*
*	PURPOSE : reset specified DLC
*
*   PARAM   : DLCI		the identifier of DLC to be reset
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

static int TMUX_ResetChannel(int DLCI)
{
	POLLQ *ptr;
	tmux.DLC[DLCI].flags = 0;
	memset(&tmux.DLC[DLCI].callback, 0, sizeof(UART_CALLBACK));
	LIB_StopTimer(&tmux.DLC[DLCI].timer);
	while (tmux.DLC[DLCI].pollque)
	{
		ptr = tmux.DLC[DLCI].pollque;
		tmux.DLC[DLCI].pollque = ptr->next;
		free(ptr);
	}
	BUF_Cleanup(&tmux.DLC[DLCI].rxbuf);
	return TMUX_SUCCESS;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_ResetMultiplexer
*
*	PURPOSE : reset multiplexer driver
*
*   PARAM   : void
*
*	RETURN  : TMUX_SUCCESS	succuessful completion
*			  TMUX_FAILURE	failure completion
*
\*========================================================================*/

static int TMUX_ResetMultiplexer(void)
{
	tmux.read     = NULL;
	tmux.write    = NULL;
	tmux.flowctrl = 0;
	tmux.scanstate = S_FLAG;
	return TMUX_SUCCESS;
}

/*========================================================================*\
*
*	FUNCTION: TMUX_WriteDevice
*
*	PURPOSE : output data to serial device
*
*   PARAM   : data		the data pointer
*			  datalen	the size of size
*
*	RETURN  : the accepted number of actual transmited data
*
\*========================================================================*/

static int TMUX_WriteDevice(UINT8* data, int datalen)
{
	if (tmux.write == NULL || data == NULL || datalen < 0)
		return -1;
#ifdef DATAIO_DEBUG
	{
		int i;
		msgout("Send: ");
		for (i = 0; i < datalen; i++)
			msgout("%02x ", data[i]);
		msgout("\r\n");
	}
#endif /* DATAIO_DEBUG */
	return tmux.write(data, datalen);
}

/*========================================================================*\
*
*	FUNCTION: TMUX_ReadDevice
*
*	PURPOSE : read data though serial interface
*
*   PARAM   : buf		recept buffer
*			  bufsize	the size of recept buffer
*
*	RETURN  : the actual number of data which coming from serial port
*
\*========================================================================*/

static int TMUX_ReadDevice(UINT8* buf, int bufsize)
{
	if (tmux.read == NULL || buf == NULL || bufsize < 0)
		return -1;
	return tmux.read(buf, bufsize);
}

/**************************************************************************\
*	buffer class
\**************************************************************************/
/*========================================================================*\
*
*	FUNCTION: BUF_Datasize
*
*	PURPOSE : get the size of data in specified buffer
*
*   PARAM   : pbuf		buffer object
*
*	RETURN  : the size of data in buffer
*
\*========================================================================*/

#if 0
static int BUF_Datasize(BUFFER* pbuf)
{
	if (pbuf == NULL)
		return 0;
	return pbuf->datalen;
}
#endif

/*========================================================================*\
*
*	FUNCTION: BUF_Cleanup
*
*	PURPOSE : clean up all the data in the specified buffer
*
*   PARAM   : pbuf		buffer object
*
*	RETURN  : 0		succuessful completion
*			  -1	failure completion
*
\*========================================================================*/

static int BUF_Cleanup(BUFFER* pbuf)
{
	if (pbuf == NULL)
		return -1;
	memset(pbuf->buf, 0, sizeof(BUF_SIZE));
	pbuf->write = pbuf->data = pbuf->buf;
	pbuf->datalen = 0;
	return 0;
}

/*========================================================================*\
*
*	FUNCTION: BUF_GetData
*
*	PURPOSE : get some data from specified buffer
*
*   PARAM   : pbuf		buffer object
*			  buf		reception buffer
*			  buflen	the size of recept buffer
*
*	RETURN  : actual number of data from buffer
*
\*========================================================================*/

static int BUF_GetData(BUFFER* pbuf, UINT8* buf, int buflen)
{
	UINT8 *front, *tail;
	int count = 0, copyed;

	if (pbuf == NULL || buf == NULL || buflen <= 0)
		return 0;

	front = pbuf->data;
	while (buflen > 0 && (tail = pbuf->write) != front)
	{
		if (tail > front)
			copyed = tail - front;
		else
			copyed = pbuf->buf + BUF_SIZE - front;
		if (copyed > buflen)
			copyed = buflen;
		memcpy(buf + count, front, copyed);
		buflen -= copyed;
		count += copyed;
		front += copyed;

		if (front >= pbuf->buf + BUF_SIZE)
			front -= BUF_SIZE;
		pbuf->data = front;
	}
	pbuf->datalen -= count;
	return count;
}

/*========================================================================*\
*
*	FUNCTION: BUF_PutData
*
*	PURPOSE : put some data into specified buffer
*
*   PARAM   : pbuf		buffer object
*			  data		data pointer
*			  datalen	data size
*
*	RETURN  : the actual number which put into the buffer
*
\*========================================================================*/

static int BUF_PutData(BUFFER* pbuf, UINT8* data, int datalen)
{
	UINT8 *front, *tail;
	int copyed, count = 0;

	if (pbuf == NULL || data == NULL || datalen <= 0)
		return 0;

	tail = pbuf->write;
	while (datalen > 0)
	{
		front = pbuf->data;
		if (tail + 1 == front || (tail + 1 == pbuf->buf + BUF_SIZE 
			&& front == pbuf->buf))
			break;

		if (front > tail)
			copyed = front - tail - 1;
		else if (front == 0)
			copyed = pbuf->buf + BUF_SIZE - tail - 1;
		else
			copyed = pbuf->buf + BUF_SIZE - tail;

		if (copyed > datalen)
			copyed = datalen;
		memcpy(tail, data + count, copyed);
		datalen -= copyed;
		count += copyed;
		tail += copyed;

		if (tail >= pbuf->buf + BUF_SIZE)
			tail -= BUF_SIZE;
		pbuf->write = tail;
	}
	pbuf->datalen += count;
	return count;
}

/*========================================================================*\
*
*	FUNCTION: BUF_Transmit
*
*	PURPOSE : send data directly from buffer
*
*   PARAM   : pbuf		buffer object
*
*	RETURN  : the actual number to have been sent from buffer
*
\*========================================================================*/

static int BUF_Transmit(BUFFER* pbuf)
{
	UINT8 *front, *tail;
	int count = 0, copyed;

	if (pbuf == NULL)
		return -1;

	front = pbuf->data;
	while ((tail = pbuf->write) != front)
	{
		if (tail > front)
			copyed = tail - front;
		else
			copyed = pbuf->buf + BUF_SIZE - front;

		if ((copyed = TMUX_WriteDevice(front, copyed)) == -1)
			break;
		count += copyed;
		front += copyed;

		if (front >= pbuf->buf + BUF_SIZE)
			front -= BUF_SIZE;
		pbuf->data = front;
	}
	pbuf->datalen -= count;
	return count;
}

/**************************** End Of Program ******************************/
/* end of program ti_mux.c */
