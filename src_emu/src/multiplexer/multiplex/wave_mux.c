/**************************************************************************\
*
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
*
* Model   :	WMUX
*
* Purpose : wavecom multiplexer protocol
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
*	��������: wireless modules multiplex driver on WAVECOM 2C
*
*	�ļ�����: wave_mux.c
*
*	��������: 
*
*	������: multiplex physical serial interface on WAVECOM 2C
*
*	�������ڣ�2002.07.24
*
\**************************************************************************/

/* include header files here */
#include "stdio.h"
#include "mux_lib.h"
#include "wave_mux.h"

/**************************************************************************\
*
*	Debugoutput����
*
\**************************************************************************/

#ifdef _EMULATE_
extern void MsgOut(char *format, ... );
#else
static void MsgOut(char* format, ... ){format=NULL;}
#endif

/**************************************************************************\
*
*	��������
*
\**************************************************************************/

/* define constant here */
/* ����ģ��ͨ��״̬ */
#define STATE_INVALID		0	/* invalided modular */
#define STATE_INITIAL		1	/* initialized OK */
#define STATE_STARTING		2	/* starting up multiplex mode */
#define STATE_MULTIPLEX		3	/* multiplex mode */
#define STATE_DOWNING		4	/* closing down multiplex mode */

/* ���֡�������� */
#define MAX_FRAME_SIZE		1020 /* length field have 11 bits */

/* ���������� */
#define BUF_SIZE			(1024 * 4)
#define FRM_BUFSIZE			(MAX_FRAME_SIZE + 4)

/* �����ٽ�β��� */
#define ENTER_MUTEX			LIB_WaitMutex  (&wmux.mutex)
#define LEAVE_MUTEX			LIB_SignalMutex(&wmux.mutex)

/* ��������֡���� */
#define FRAME_AT			0xAA	/* AT Command Frame */
#define FRAME_DATA			0xDD	/* Data service Frame */

/* �������ݰ����� */
#define PACKET_AT			0x1D	/* AT command */
#define PACKET_DATA			0		/* data service */
#define PACKET_STATUS		1		/* signal emulate */
#define PACKET_READY		2		/* data channel is enabled */
#define PACKET_BUSY			3		/* data channel is disabled  */

/* WAVECOM �����״̬λ */
#define STATUS_DTR			(1 << 7)	/* Data Terminal Ready (SA) */
#define STATUS_RTS			(1 << 6)	/* Ready to Send (SB) */
#define STATUS_DSR			STATUS_DTR	/* Data Set Ready (����SA) */
#define STATUS_DCD			STATUS_RTS	/* Data Carrier Detected (����SB) */
#define STATUS_CTS			(1 << 5)	/* Clear to Send (X) */
#define STATUS_BRK			(1 << 4)	/* Break signal */
#define STATUS_RING			(1 << 3)	/* Ring Singal */

/* ����multiplexerģʽ�������� */
#define AT_WMUX_ON			"AT+WMUX=1\r"
#define AT_WMUX_OFF			"ATH+WMUX=0\r"

/* ����multiplex mode ON/OFF */
#define MUX_ON				1
#define MUX_OFF				0

/* ����������ʱ�¼� */
#define BLOCK_TIMEOUT		(1000)	/* 1 second */
#define WMUX_TIMEOUT		(1500)

/**************************************************************************\
*
*	���ݽṹ���������Ͷ���
*
\**************************************************************************/

/* define any data structure here */
typedef unsigned char	UINT8;

/* �������״̬ */
typedef enum 
{
	S_START_PATTERN,
	S_LSB_OF_LENGTH, 
	S_MSB_AND_TYPE,
	S_USER_DATA, 
	S_CHECK_SUM
} SCANSTATE;

/* ����channel��ʶ */
typedef enum { CHNL_AT, CHNL_DATA, CHNL_NUM };

/* UART���ƽṹ */
typedef struct
{
	_CALLBACK	userfunc;	/* ���ڻص�����ָ�� */
	void *		para;
	DWORD		EventCode;
}	UART_CALLBK;

/* ����֡�ṹ */
typedef struct
{
	int		frm_type;			/* frame   ����		*/
	int		pkg_type;			/* package ����		*/
	UINT8	data[2048];			/* user data erea */
	int		datalen;			/* data length */
}	FRAME;

/* ���建�����ṹ */
typedef struct
{
	UINT8	buf[BUF_SIZE];		/* ������		*/
	UINT8*	data;				/* ����ָ��		*/
	UINT8*	write;				/* дָ��		*/
	int		datalen;			/* ���ݳ���		*/
}	BUFFER;

/* ����ͨ������ṹ */
typedef struct
{
	int			opened;			/* channel opened flag */
	int			flowctrl;		/* channel flow control flag */
	UART_CALLBK	callback;		/* �ص�����	*/
	BUFFER		rxbuf;			/* ���ջ�����	*/
}	CHANNEL;

/* ����Multiplexer����ṹ */
typedef struct
{
	int			state;			/* mux driver state */
	UINT8		status;			/* mux driver status */
#if (_HOPEN_VER >= 200)
	MUTEX		mutex;			/* ������		*/
#else
	OSHANDLE	mutex;			/* ������	*/
#endif
	APPTIMER	timer;			/* ͨ����ʱ��	*/
	OSHANDLE	event;			/* ͨ���¼�		*/

	IOCOM		write;			/* д���ں��� */
	IOCOM		read;			/* �����ں��� */
	CHANNEL		chanl[CHNL_NUM];	/* AT & DATA channel */
	UINT8		mybuf[FRM_BUFSIZE];
	UINT8		txbuf[BUF_SIZE];	/* ģ�鷢�ͻ����� */
	UINT8*		data;			/* ����������ָ�� */
	int			datalen;		/* �������������� */
	SCANSTATE	scanstate;		/* frame scan state */
	FRAME		frame;			/* MUX frame structure */
	int			flowctrl;		/* uart flow control flag */
	int			timeoutflag;	/* timeout flag */
	int			readyflag;		/* Data Ready Flag */
}	WMUX;


/**************************************************************************\
*
*	ȫ�ֱ�������
*
\**************************************************************************/

/* define globle varible here */

/* multiplexer���ƽṹ */
static WMUX wmux;

/**************************************************************************\
*
*	��������
*
\**************************************************************************/

/* function prototype declaration */

static int WMUX_ResetMultiplexer(void);
static int WMUX_ResetChannel(int i);
static int WMUX_WaitEvent(int microsecond);
static void WMUX_TimeOut(void* param);
static UINT8 WMUX_FrameCheckSum(UINT8 *data, int datalen);
static int WMUX_SendFrameData(int DLCI, UINT8 frametype, UINT8* data, int datalen);
static int WMUX_ProcRxFrame(FRAME *frame);
static int WMUX_CheckWMUXResponse(char* data, int datalen);
static void WMUX_SetDataFlowControl(void);

static int WMUX_WriteDevice	(UINT8* data, int datalen);
static int WMUX_ReadDevice	(UINT8* buf, int bufsize);
static void WMUX_CallBack	(int DLCI, unsigned int event);

/* �ڲ�������BUFFER������ */
#define WMBUF_IsEmpty(pbuf)		((pbuf)->data == (pbuf)->write)
//static int WMBUF_GetDataSize(BUFFER* buf);
static int WMBUF_CleanupData(BUFFER* buf);
static int WMBUF_GetBufData (BUFFER* buf, UINT8* pbuf, int buflen);
static int WMBUF_PutBufData (BUFFER* buf, UINT8* data, int datalen);

/**************************************************************************\
*
*	����ʵ�ֲ���
*
\**************************************************************************/

/**************************************************************************\
*
*	�ӿں���ʵ�ֲ��� (interface function)
*
\**************************************************************************/

/*========================================================================*\
*
*	�������ƣ�WMUX_Initial (system function)
*
*	�������ܣ�initialize WMUX mudole, alloc nesscery resource.
*
*   ������������
*
*	�������أ�WMUX_SUCCESS	�ɹ�
*			  WMUX_FAILURE	ʧ��
*
\*========================================================================*/

int WMUX_Initial(void)
{
	if (wmux.state != STATE_INVALID)
		return WMUX_FAILURE;

	memset(&wmux, 0, sizeof(WMUX));

	/* ����ģ��MUTEX LOCK */
	if (LIB_CreateMutex(&wmux.mutex) == -1)
		return WMUX_FAILURE;

	/* ����ͨ����ʱ�� */
	LIB_CreateTimer(&wmux.timer);

	/* ����ͨ���¼� */
	if ((wmux.event = LIB_CreateEvent("wmux")) == NULL)
		return WMUX_FAILURE;

	/* set transnit buffer */
	wmux.data = wmux.txbuf;

	/* clean up receive buffer of AT channel and DATA channel */
	WMBUF_CleanupData(&wmux.chanl[CHNL_AT  ].rxbuf);
	WMBUF_CleanupData(&wmux.chanl[CHNL_DATA].rxbuf);

	wmux.state = STATE_INITIAL;

	return WMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_Startup
*
1*	�������ܣ�startup WAVECOM wireless module into multiplex mode.
*
*   ����������fpread		�����ں�����ڵ�ַ
*			  fpwrite		д���ں�����ڵ�ַ
*
*	�������أ�WMUX_SUCCESS	�ɹ�
*			  WMUX_FAILURE	ʧ��
*
\*========================================================================*/

int WMUX_Startup(IOCOM fpread, IOCOM fpwrite)
{
	UINT8	buf[256];
	int		len, cmdlen;
	/* �������ô��ڶ�д������� */
	if (fpread == NULL || fpwrite == NULL)
		return WMUX_FAILURE;

	ENTER_MUTEX;

	/* ֻ����STATE_INITIALʱ����ִ�иú��� */
	if (wmux.state != STATE_INITIAL)
		goto ERROR_NOOP;

	WMUX_ResetMultiplexer();
	wmux.read  = fpread;
	wmux.write = fpwrite;
	wmux.state = STATE_STARTING;

	/* ��մ��ڻ����� */
	while (fpread(buf, 256) >= 0)
		;

	/* ����AT+WMUX=1����wmuxģʽ */
	strcpy((char*)buf, AT_WMUX_ON);
	cmdlen = (int)strlen((char*)buf);
	if ((len = fpwrite(buf, cmdlen)) != cmdlen)
	{
		len = (len < 0 ? 0 : len);
		wmux.datalen = cmdlen - len;
		memcpy(wmux.txbuf, &buf[len], wmux.datalen);
	}

	/* �ȴ��ش�OK */
	WMUX_WaitEvent(WMUX_TIMEOUT);

	if (wmux.state != STATE_MULTIPLEX)
		goto ERROR_HANDLE;

	LEAVE_MUTEX;
	MsgOut("MUX Startup OK\r\n");
	return WMUX_SUCCESS;

ERROR_HANDLE:

	WMUX_ResetMultiplexer();
	wmux.state = STATE_INITIAL;

ERROR_NOOP:
	LEAVE_MUTEX;
	MsgOut("MUX Startup ERROR\r\n");
	return WMUX_FAILURE;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_Closedown
*
*	�������ܣ�recover WAVECOM wireless module to AT command mode.
*
*   ������������
*
*	�������أ�WMUX_SUCCESS	�ɹ�
*			  WMUX_FAILURE	ʧ��
*
\*========================================================================*/

int WMUX_Closedown(void)
{
	UINT8	buf[256];
	int		i, cmdlen;

	ENTER_MUTEX;

	/* ֻ����STATE_MULTIPLEX״̬ʱ����ִ�� */
	if (wmux.state != STATE_MULTIPLEX)
		goto ERROR_NOOP;

	wmux.state = STATE_DOWNING;
	
	if (wmux.readyflag && wmux.chanl[CHNL_DATA].opened)
	{
		UINT8 my_status = 0;
		WMUX_SendFrameData(CHNL_DATA, PACKET_STATUS, &my_status, 1);
		WMUX_WaitEvent(BLOCK_TIMEOUT * 2);
		if (wmux.readyflag != 0)
			goto ERROR_HANDLE;
	}

	/* ����AT+WMUX=0�ر�wmuxģʽ */
	strcpy((char*)buf, AT_WMUX_OFF);
	cmdlen = (int)strlen((char*)buf);

	if (WMUX_SendFrameData(CHNL_AT, PACKET_AT, buf, cmdlen) == WMUX_FAILURE)
		goto ERROR_HANDLE;

	WMUX_WaitEvent(WMUX_TIMEOUT);

	if (wmux.state != STATE_INITIAL)
		goto ERROR_HANDLE;

	/* ��ͨ���رճɹ� */
	WMUX_ResetMultiplexer();

	/* �������ͨ�� */
	for (i = 0; i < CHNL_NUM; i++)
		WMUX_ResetChannel(i);

	LEAVE_MUTEX;
	MsgOut("MUX closedown OK\r\n");
	return WMUX_SUCCESS;

ERROR_HANDLE:
	wmux.state = STATE_MULTIPLEX;

ERROR_NOOP:
	LEAVE_MUTEX;
	MsgOut("MUX closedown ERROR\r\n");
	return WMUX_FAILURE;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_OpenChannel
*
*	�������ܣ�Open a wavecom multiplex channel.
*
*   ����������name		ͨ������
*
*	�������أ�>=0		ͨ�����
*			  -1		ʧ��
*
\*========================================================================*/

int WMUX_OpenChannel(char* name)
{
	int	i = -1;

	if (name == NULL)
		return -1;

	if (strcmp(name, WMUX_CHANNEL_AT) == 0)
		i = CHNL_AT;
	else if (strcmp(name, WMUX_CHANNEL_DATA) == 0)
		i = CHNL_DATA;
	else
		return -1;

	ENTER_MUTEX;
	if (wmux.state == STATE_MULTIPLEX)
	{
		if (wmux.chanl[i].opened == 0)
		{
			WMUX_ResetChannel(i);
			wmux.chanl[i].opened = 1;

			/* if Data Set Ready or Data Carrier Detected */
			if (i == CHNL_DATA && (wmux.status & (STATUS_DSR|STATUS_DCD)))
			{
				/* Data Terminal Ready and Request to Send */
				UINT8 my_status = STATUS_DTR|STATUS_RTS;
				WMUX_SendFrameData(CHNL_DATA, PACKET_STATUS, &my_status, 1);
			}
			WMUX_SetDataFlowControl();
		}
		else
			i = -1;
	}
	LEAVE_MUTEX;
	MsgOut("MUX open %s = DLC%d\r\n", name, i);
	return i;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_CloseChannel
*
*	�������ܣ�Close a opened multiplex channel.
*
*   ����������handle		ͨ�����
*
*	�������أ�WMUX_SUCCESS	�ɹ�
*			  WMUX_FAILURE	ʧ��
*
\*========================================================================*/

int WMUX_CloseChannel(int handle)
{
	int retval = WMUX_FAILURE;

	if (handle != CHNL_AT && handle != CHNL_DATA)
		return WMUX_FAILURE;
	
	ENTER_MUTEX;
	if (wmux.chanl[handle].opened != 0)
	{
		/* if Data Set Ready or Data Carrier Detected */
		if (handle == CHNL_DATA && (wmux.status & (STATUS_DSR|STATUS_DCD)))
		{
			/* Data Terminal Ready and Request to Send */
			UINT8 my_status = 0;
			WMUX_SendFrameData(CHNL_DATA, PACKET_STATUS, &my_status, 1);
		}
		WMUX_ResetChannel(handle);
		retval = WMUX_SUCCESS;
	}
	LEAVE_MUTEX;
	MsgOut("MUX close channle = DLC%d\r\n", handle);
	return retval;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_WriteChannel
*
*	�������ܣ�transmit user data into wavecom multiplex channel.
*
*   ����������handle		ͨ�����
*			  data			����ָ��
*			  datalen		���ݳ���
*
*	�������أ�>=0	ʵ��д�����ݵ�����
*			  -1	����
*
\*========================================================================*/

int WMUX_WriteChannel(int handle, unsigned char* data, int datalen)
{
	int size = -1;
	UINT8 packet;

	if (handle == CHNL_AT)
		packet = PACKET_AT;
	else if (handle == CHNL_DATA)
		packet = PACKET_DATA;
	else
		return -1;
	if (data == NULL || datalen <= 0)
		return -1;

	ENTER_MUTEX;

	/* ֻ���ڸ�ͨ����OPEN����û��FLOWCTRL״̬�²��ܷ������� */
	if (wmux.chanl[handle].opened != 0
		&& wmux.chanl[handle].flowctrl == 0
		&& wmux.flowctrl == 0)
	{
		/* ��ʼ�����û����� */
		size = (MAX_FRAME_SIZE > datalen ? datalen : MAX_FRAME_SIZE);
		if (WMUX_SendFrameData(handle, packet, data, size) 
			== WMUX_FAILURE)
			size = 0;
	}
	LEAVE_MUTEX;
	return size;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_ReadChannel
*
*	�������ܣ�receive remote data to user buffer.
*
*   ����������handle		ͨ�����
*			  buf			������
*			  bufsize		����������
*
*	�������أ�>=0	ʵ�ʶ������ݵ�����
*			  -1	����
*
\*========================================================================*/

int WMUX_ReadChannel(int handle, unsigned char* buf, int bufsize)
{
	int readlen = -1;
	
	if (handle != CHNL_AT && handle != CHNL_DATA)
		return -1;
	if (buf == NULL || bufsize <= 0)
		return -1;

	ENTER_MUTEX;
	if (wmux.chanl[handle].opened != 0)
	{
		if ((readlen = WMBUF_GetBufData(&wmux.chanl[handle].rxbuf, 
			buf, bufsize)) == 0)
			readlen = -1;
	}
	LEAVE_MUTEX;
	return readlen;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_IoControl
*
*	�������ܣ�control channel special action with operation code.
*
*   ����������handle		ͨ�����
*			  opcode		������
*			  value			��ֵ
*			  size			��С
*
*	�������أ�WMUX_SUCCESS	�ɹ�
*			  WMUX_FAILURE	ʧ��
*
\*========================================================================*/

int WMUX_IoControl(int handle, unsigned opcode, int value, int size)
{	
	if (handle != CHNL_AT && handle != CHNL_DATA)
		return WMUX_FAILURE;
	
	ENTER_MUTEX;

	if (wmux.chanl[handle].opened == 0)
		goto ERROR_HANDLE;

	switch (opcode)
	{
	case WMUX_OP_CALLBACK:
		if (size != sizeof(UART_CALLBK) || value == 0)
			goto ERROR_HANDLE;
		memcpy(&wmux.chanl[handle].callback, (void*)value, sizeof(UART_CALLBK));
		if (!WMBUF_IsEmpty(&wmux.chanl[handle].rxbuf))
			WMUX_CallBack(handle, WMUX_DATAIN);
		break;
	case WMUX_OP_DUMPRXBUF:
		WMBUF_CleanupData(&wmux.chanl[handle].rxbuf);
		break;
	case WMUX_OP_DATABROKEN:/* hangup */
		if (strcmp((char*)value, "+++") == 0 && size == 3)
		{
			/* ??? which channel should have this function ? */
			UINT8 my_status = STATUS_BRK;
			if (WMUX_SendFrameData(handle, PACKET_STATUS, &my_status, 1) 
				== WMUX_FAILURE)
				goto ERROR_HANDLE;
		}
		else
			goto ERROR_HANDLE;
		break;
	case WMUX_OP_LINESTATUS:
		if (value != 0  && size == sizeof(unsigned int))
		{
			unsigned linestatus = 0;
			if (wmux.status & STATUS_RING)
				linestatus |= WMUX_RING;
			if (wmux.status & STATUS_DCD)
				linestatus |= WMUX_RLSD;
			*(unsigned int*)value = linestatus;
		}
		else
			goto ERROR_HANDLE;		
		break;
	default:
		goto ERROR_HANDLE;
	}	
		
	LEAVE_MUTEX;
	return WMUX_SUCCESS;


ERROR_HANDLE:

	LEAVE_MUTEX;
	return WMUX_FAILURE;
}


/*========================================================================*\
*
*	�������ƣ�WMUX_OnDatain
*
*	�������ܣ����ݵ����ص�����
*
*   ������������
*
*	�������أ�WMUX_SUCCESS	�ɹ�
*			  WMUX_FAILURE	ʧ��
*
\*========================================================================*/

int WMUX_OnDatain(void)
{
	/* ��DLC0�Ľ��ջ���������������ջ����� */
	static UINT8	  buf[100]; 
	static int		  datalen = 0;
	static UINT8	  fcs = 0;

	int		readlen, i, retval;

	/* ��ȡ���� */
	while ((readlen = WMUX_ReadDevice(buf, 100)) >= 0)
	{
		/* process incoming data */
		if (readlen == 0)
			continue;
		
		for (i = 0; i < readlen; )
		{
			switch (wmux.scanstate)
			{
			case S_START_PATTERN:
				if (buf[i] == FRAME_AT || buf[i] == FRAME_DATA)
				{
					memset(&wmux.frame, 0, sizeof(FRAME));
					datalen = 0;
					wmux.frame.frm_type = buf[i];
					fcs = buf[i];
					wmux.scanstate = S_LSB_OF_LENGTH;
				}
				else if (wmux.state == STATE_STARTING)
				{
					/* sent: AT+WMUX=1\r startup MUX mode.
					 * wait: normal ERROR string "\nERROR\r".
					 */
					retval = WMUX_CheckWMUXResponse((char*)&buf[i], 1);
					/* If failure, means still stay in AT mode. */
					if (retval == WMUX_FAILURE)
						LIB_SignalEvent(wmux.event, 1);
				}
				else if (wmux.state == STATE_DOWNING)
				{
					/* sent: AT+WMUX=0\r closedown MUX mode.
					 * wait: normal OK string "\nOK\r".
					 */
					retval = WMUX_CheckWMUXResponse((char*)&buf[i], 1);
					/* If success, means already leave MUX mode. */
					if (retval == WMUX_SUCCESS)
					{
						wmux.state = STATE_INITIAL;
						LIB_SignalEvent(wmux.event, 1);
					}
				}
				else
					MsgOut("MUX discard char [%c] !!!\r\n", buf[i]);
				i ++;
				break;
			case S_LSB_OF_LENGTH:
				fcs = (UINT8)(fcs + buf[i]);
				wmux.frame.datalen = buf[i];
				wmux.scanstate = S_MSB_AND_TYPE;
				i++;
				break;
			case S_MSB_AND_TYPE:
				fcs = (UINT8)(fcs + buf[i]);
				wmux.frame.pkg_type = (buf[i] >> 3);
				wmux.frame.datalen |= ((buf[i]&0x07) << 8);
				wmux.scanstate = S_USER_DATA;
				i ++;
				break;
			case S_USER_DATA:
				if (datalen < wmux.frame.datalen)
				{
					fcs = (UINT8)(fcs + buf[i]);
					wmux.frame.data[datalen++] = buf[i++];
				}
				else
					wmux.scanstate = S_CHECK_SUM;
				break;
			case S_CHECK_SUM:
				if (fcs == buf[i])
				{
					/* �ɹ��Ļ��һ֡���д��� */
					ENTER_MUTEX;
					WMUX_ProcRxFrame(&wmux.frame);
					LEAVE_MUTEX;
					i ++;
				}
				else
					MsgOut("MUX get input FCS error!!!\r\n");
				wmux.scanstate = S_START_PATTERN;
				break;
			default:
				wmux.scanstate = S_START_PATTERN;
			}
		}
	}
	return WMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_OnDataout
*
*	�������ܣ����Լ����������ݵĻص�����
*
*   ������������
*
*	�������أ�WMUX_SUCCESS	�ɹ�
*			  WMUX_FAILURE	ʧ��
*
\*========================================================================*/

int WMUX_OnDataout(void)
{
	int sendlen, i;

	ENTER_MUTEX;

	/* ���ͻ���������δ��������ϵ����� */
	while (wmux.datalen > 0)
	{
		if ((sendlen = WMUX_WriteDevice(wmux.data, wmux.datalen)) <= 0)
			break;

		wmux.datalen -= sendlen;
		wmux.data    += sendlen;
		if (wmux.datalen == 0)
		{
			wmux.data = wmux.txbuf;
			wmux.flowctrl = 0;
			for (i = 1; i < CHNL_NUM; i ++)
			{
				if (wmux.chanl[i].flowctrl == 0)
					WMUX_CallBack(i, WMUX_DATAOUT);
			}
		} /* if (wmux.datalen == 0) */
	} /* while (wmux.datalen > 0) */

	LEAVE_MUTEX;
	return WMUX_SUCCESS;
}

/**************************************************************************\
*
*	�ڲ�����ʵ�ֲ��� (internal function)
*
\**************************************************************************/

/*========================================================================*\
*
*	�������ƣ�WMUX_ResetMultiplexer
*
*	�������ܣ���λmultiplexerģ��
*
*   ������������
*
*	�������أ�WMUX_SUCCESS	�ɹ�
*			  WMUX_FAILURE	ʧ��
*
\*========================================================================*/

static int WMUX_ResetMultiplexer(void)
{
	wmux.read     = NULL;
	wmux.write    = NULL;
	wmux.data     = wmux.txbuf;
	wmux.datalen  = 0;
	wmux.status   = 0;
	wmux.flowctrl = 0;
	wmux.timeoutflag = 0;
	wmux.readyflag = 0;
	wmux.scanstate = S_START_PATTERN;
	return WMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_ResetChannel
*
*	�������ܣ���λָ����ͨ��
*
*   ����������i		ͨ����ʶ
*
*	�������أ�WMUX_SUCCESS	�ɹ�
*			  WMUX_FAILURE	ʧ��
*
\*========================================================================*/

static int WMUX_ResetChannel(int i)
{
	memset(&wmux.chanl[i], 0, sizeof(CHANNEL));
	WMBUF_CleanupData(&wmux.chanl[i].rxbuf);
	return WMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_WaitEvent
*
*	�������ܣ��ȴ�����AT+WMUX��Ӧ��
*
*   ����������microsecond	����ʱ��
*
*	�������أ�WMUX_SUCCESS	�ɹ����¼�����
*			  WMUX_FAILURE	ʧ�ܣ���ʱ
*
\*========================================================================*/

static int WMUX_WaitEvent(int microsecond)
{
	/* �����ʱ��� */
	wmux.timeoutflag = 0;

	/* ���ü�ʱ�� */
	LIB_SetTimerFunc(&wmux.timer, WMUX_TimeOut, NULL);
	LIB_SetTimerExpire(&wmux.timer, microsecond);

	/* ������ʱ�� */
	LIB_StartTimer(&wmux.timer);

	LEAVE_MUTEX;
	LIB_WaitEvent(wmux.event);
	ENTER_MUTEX;

	/* ��ֹʱ�� */
	LIB_StopTimer(&wmux.timer);

	/* �����ʱ */
	if (wmux.timeoutflag != 0)
		return WMUX_FAILURE;

	return WMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_TimeOut
*
*	�������ܣ���ʱ�ص�����
*
*   ����������param		������
*
*	�������أ���
*
\*========================================================================*/

static void WMUX_TimeOut(void* param)
{
	param = NULL;
	ENTER_MUTEX;
	if (wmux.state == STATE_STARTING || wmux.state == STATE_DOWNING)
	{
		/* ����timeout��� */
		wmux.timeoutflag = 1;
		MsgOut("MUX timeout\r\n");
		/* ���ѵȴ��¼� */
		LIB_SignalEvent(wmux.event, 1);
	}
	LEAVE_MUTEX;
	return;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_SendFrameData
*
*	�������ܣ���ָ����֡�ŵ�mplx��txbuf�У������������ʼ����
*
*   ����������DLCI			ͨ����
*			  frametype		֡������ (AT_CMD, DATA, STATUS, READY, BUSY)
*			  data			����ָ��
*			  datalen		���ݳ���
*
*	�������أ�WMUX_SUCCESS	�ɹ�
*			  WMUX_FAILURE	ʧ��
*			  WMUX_ASYNC	�첽����
*
\*========================================================================*/

/**************************************************************************
 * MUX Frame
 * format       : Bit8  Bit7  Bit6  Bit5  Bit4  Bit3  Bit2  Bit1
 * start pattern: 0xAA (AT frame) or 0xDD (data frame)
 * LSB length   : L8    L7    L6    L5    L4    L3    L2    L1
 * MSB length	:                               L11   L10   L9
 * package type : T5    T4    T3    T2    T1
 * information  : n(=MSB & LSB length) octets (0-2047)
 * check sum    : 1 octet FCS
 **************************************************************************
 * package type [length]    [information]   [meaning]
 * AT	  0x1d	n           AT command      AT command related infor.
 * DATA	  0x00	n           user data       user data related info.
 * STATUS 0x01	1           V.24 signal     V.24 signal emulation
 * READY  0x02	0           null            DATA transmission Enabled
 * BUSY	  0x03	0           null            DATA transmission Disabled
 **************************************************************************
 * V.24 signal  : Bit8  Bit7  Bit6  Bit5  Bit4  Bit3  Bit2  Bit1
 *                SA    SB    X     BRK   RI    res   res   res
 * DTE->DCE       DTR   RTS   -     break -     -     -     -
 * DCE->DTE       DSR   DCD   CTS   -     RING  -     -     -
 **************************************************************************
 */

static int WMUX_SendFrameData(int DLCI, UINT8 frametype, UINT8* data, int datalen)
{
	UINT8* cp = wmux.mybuf;
	int writelen, len = 0, off = 0;

	if (frametype == PACKET_STATUS)
	{
		MsgOut("MUX transmit a status package < ");
		if (*data & STATUS_DTR)
			MsgOut("[DTR(SA)]");
		if (*data & STATUS_RTS)
			MsgOut("[RTS(SB)]");
		if (*data & STATUS_CTS)
			MsgOut("[CTS(X)]");
		if (*data & STATUS_BRK)
			MsgOut("[BRK]");
		MsgOut(" >\r\n");
	}

	/* frame type */
	if (DLCI == CHNL_AT)
		*cp++ = FRAME_AT; /* 0xAA */
	else
		*cp++ = FRAME_DATA; /* 0xDD */
	
	/* LSB of length */
	*cp++ = (UINT8)(datalen & 0xff); 
	/* package type & MSB of length */
	*cp++ = (UINT8)(((frametype << 3) | ((datalen >> 8) & 0x07)) & 0xff); 
	
	/* copy user data into buffer */
	memcpy(cp, data, datalen);
	cp += datalen;

	/* calculate chuck sum */
	*cp++ = WMUX_FrameCheckSum(wmux.mybuf, cp - wmux.mybuf);
	len = cp - wmux.mybuf;
	
	/* ���û��������ֱ�ӷ������� */
	if (wmux.flowctrl == 0)
	{
		wmux.data = wmux.txbuf;
		while (len > 0)
		{
			writelen = WMUX_WriteDevice(wmux.mybuf+off, len);
			if (writelen < 0)
				break;
			off += writelen;
			len -= writelen;
		}
	}
	/* ���û�з�����ϣ���ʣ������ݱ�����txbuf�ȴ��첽���ͣ�
	 * ���������� 
	 */
	if (len > 0)
	{
		if (len > BUF_SIZE - wmux.datalen)
			return WMUX_FAILURE;

		memcpy(wmux.data, wmux.mybuf+off, len);
		wmux.datalen += len;
		wmux.flowctrl = 1;
		return WMUX_ASYNC;
	}
	return WMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_FrameCheckSum
*
*	�������ܣ��������ݵ�У���
*
*   ����������data		����ָ��
*			  datalen	���ݳ���
*
*	�������أ�Frame Check Sum value
*
\*========================================================================*/

static UINT8 WMUX_FrameCheckSum(UINT8 *data, int datalen)
{
    UINT8 fcs = 0;
	int i;
    for (i = 0; i < datalen; i ++)
        fcs = (UINT8)(fcs + data[i]);
    return fcs;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_ProcRxFrame
*
*	�������ܣ�������յ���һ֡����
*
*   ����������frame		���յ���֡�ṹ������
*
*	�������أ�WMUX_SUCCESS	�ɹ�
*			  WMUX_FAILURE	ʧ��
*
\*========================================================================*/

static int WMUX_ProcRxFrame(FRAME *frame)
{
	int		retval, tag = 0;
	UINT8	status = 0, diff_status = 0;
	switch (frame->pkg_type)
	{
	case PACKET_AT:
		if (frame->frm_type != FRAME_AT)
			break;
		if (wmux.state == STATE_STARTING)
		{
			/* sent: AT+WMUX=1\r startup MUX mode��
			 * wait: mux string "\r\nOK\r\n"��
			 */
			retval = WMUX_CheckWMUXResponse((char*)frame->data, frame->datalen);
			/* if success, means already enter MUX mode. */
			if (retval == WMUX_SUCCESS)
			{
				wmux.state = STATE_MULTIPLEX;
				LIB_SignalEvent(wmux.event, 1);
			}
		}
		else if (wmux.state == STATE_DOWNING)
		{
			/* sent: AT+WMUX=0\r startup MUX mode��
			 * wait: mux string "\r\nERROR\r\n"��
			 */
			retval = WMUX_CheckWMUXResponse((char*)frame->data, frame->datalen);
			/* if failure, means still stay in MUX mode. */
			if (retval == WMUX_SUCCESS)
				LIB_SignalEvent(wmux.event, 1);
		}
		else if (wmux.chanl[CHNL_AT].opened)
		{
			tag = WMBUF_IsEmpty(&wmux.chanl[CHNL_AT].rxbuf);
			WMBUF_PutBufData(&wmux.chanl[CHNL_AT].rxbuf, frame->data, frame->datalen);
			if (tag)
				WMUX_CallBack(CHNL_AT, WMUX_DATAIN);
		}
		break;
	case PACKET_DATA:
		if (frame->frm_type == FRAME_DATA && wmux.chanl[CHNL_DATA].opened)
		{
			tag = WMBUF_IsEmpty(&wmux.chanl[CHNL_DATA].rxbuf);
			WMBUF_PutBufData(&wmux.chanl[CHNL_DATA].rxbuf, frame->data, frame->datalen);
			if (tag)
				WMUX_CallBack(CHNL_DATA, WMUX_DATAIN);
		}
		break;
	case PACKET_STATUS:
		if (frame->frm_type == FRAME_DATA && frame->datalen == 1)
		{
			char* signal[] = {NULL, NULL, NULL, "RING", "BRK", "CTS(X)", "DCD(SB)", "DSR(SA)"};
			int i, tag = 0;
			status = frame->data[0];

			MsgOut("MUX receive a STATUS package ( ");
			for (i = 7; i >= 0; i--)
				if ((status & (1 << i)) && signal[i] != NULL)
					MsgOut("[%s]", signal[i]);
			MsgOut(" ) \r\n");

			diff_status = (UINT8)(status^wmux.status);
			if (diff_status & STATUS_DSR)
			{
				if (wmux.status & STATUS_DSR)
					MsgOut("MUX [-DSR(SA)] signal\r\n");
				else
				{
					tag = 1;
					MsgOut("MUX [+DSR(SA)] signal\r\n");
				}
			}
			if (diff_status & STATUS_DCD)
			{
				if (wmux.status & STATUS_DCD)
					MsgOut("MUX [-DCD(SB)] signal\r\n");
				else
				{
					tag = 1;
					MsgOut("MUX [+DCD(SB)] signal\r\n");
				}
				WMUX_CallBack(CHNL_AT, WMUX_MODEM);
			}
			if (diff_status & STATUS_CTS)
			{
				if (wmux.status & STATUS_CTS)
					MsgOut("MUX [-CTS(X)] signal\r\n");
				else
					MsgOut("MUX [+CTS(X)] signal\r\n");
			}
			if (diff_status & STATUS_BRK)
			{
				if (wmux.status & STATUS_BRK)
					MsgOut("MUX [-BRK] signal\r\n");
				else
					MsgOut("MUX [+BRK] signal\r\n");
			}			
			if (diff_status & STATUS_RING)
			{
				if (wmux.status & STATUS_RING)
					MsgOut("MUX [-RING] signal\r\n");
				else
					MsgOut("MUX [+RING] signal\r\n");
				WMUX_CallBack(CHNL_AT, WMUX_MODEM);
			}
			if (tag == 1)
			{
				/* Data Terminal Ready and Request to Send */
				UINT8 my_status = STATUS_DTR|STATUS_RTS;
				WMUX_SendFrameData(CHNL_DATA, PACKET_STATUS, &my_status, 1);
				if (!wmux.chanl[CHNL_DATA].opened)
				{
					my_status = 0;
					WMUX_SendFrameData(CHNL_DATA, PACKET_STATUS, &my_status, 1);
				}
			}
			wmux.status = status;
			WMUX_SetDataFlowControl();
		}
		break;
	case PACKET_READY:/* Flow ctrl is OFF */
		if (frame->frm_type == FRAME_DATA && frame->datalen == 0)
		{
			wmux.readyflag = 1;
			MsgOut("MUX Data channel get a READY package\r\n");
			WMUX_SetDataFlowControl();
		}
		break;
	case PACKET_BUSY:/* Flow ctrl is ON */
		if (frame->frm_type == FRAME_DATA && frame->datalen == 0)
		{
			wmux.readyflag = 0;
			MsgOut("MUX Data channel get a BUSY package\r\n");
			WMUX_SetDataFlowControl();
		}
		break;
	default:
		return WMUX_FAILURE;
	}
	return WMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_CheckWMUXResponse
*
*	�������ܣ����AT+WMUX�����Ӧ��
*
*   ����������data		Ӧ������
*			  datalen	Ӧ�����ݳ���
*
*	�������أ�WMUX_SUCCESS	����"\r\nOK\r\n"
*			  WMUX_FAILURE	����"\r\nERROR\r\n"
*			  WMUX_ASYNC	û��ָ���ķ���
*
\*========================================================================*/

#define ATCMD_LEN		10

static int WMUX_CheckWMUXResponse(char* data, int datalen)
{
	static char at_buf[ATCMD_LEN + 1];
	static int at_len = 0;
	static int keep_state = STATE_INVALID;
	int i;
	if (keep_state != wmux.state)
	{
		memset(at_buf, 0, ATCMD_LEN + 1);
		at_len = 0;
		keep_state = wmux.state;
	}
	for (i = 0; i < datalen; i ++)
	{
		MsgOut("%c", data[i]);
		if (at_len == ATCMD_LEN)
		{
			memcpy(at_buf, at_buf + 1, ATCMD_LEN);
			at_len--;
		}
		if (data[i] != '\0');
			at_buf[at_len++] = data[i];

		if (strstr(at_buf, "\nOK\r") != NULL)
			return WMUX_SUCCESS;
		else if (strstr(at_buf, "\nERROR\r") != NULL)
			return WMUX_FAILURE;
	}
	return WMUX_ASYNC;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_WriteDevice
*
*	�������ܣ��򴮿��豸д������
*
*   ����������data		����ָ��
*			  datalen	���ݳ���
*
*	�������أ�ʵ��д�������
*
\*========================================================================*/

static int WMUX_WriteDevice(UINT8* data, int datalen)
{
	if (wmux.write == NULL || data == NULL || datalen < 0)
		return -1;
	return wmux.write(data, datalen);
}

/*========================================================================*\
*
*	�������ƣ�WMUX_ReadDevice
*
*	�������ܣ��Ӵ����豸�ж�ȡ����
*
*   ����������buf		��������ַ
*			  bufsize	�������Ĵ�С
*
*	�������أ�ʵ�ʶ�������������
*
\*========================================================================*/

static int WMUX_ReadDevice(UINT8* buf, int bufsize)
{
	if (wmux.read == NULL || buf == NULL || bufsize < 0)
		return -1;
	return wmux.read(buf, bufsize);
}

/*========================================================================*\
*
*	�������ƣ�WMUX_CallBack
*
*	�������ܣ��û��ص�����
*
*   ����������DLC		ͨ��
*			  event		�ص��¼� (DATAIN��DATAOUT��BROKEN)
*
*	�������أ���
*
\*========================================================================*/

static void WMUX_CallBack(int DLCI, unsigned int event)
{	
	if (wmux.chanl[DLCI].opened 
		&& wmux.chanl[DLCI].callback.userfunc != NULL)
	{
		if (wmux.chanl[DLCI].callback.EventCode & event)
		{
			LEAVE_MUTEX;
			wmux.chanl[DLCI].callback.userfunc
				((long)wmux.chanl[DLCI].callback.para, event, DLCI, 0L);
			ENTER_MUTEX;
		}
	}	
	return;
}

/*========================================================================*\
*
*	�������ƣ�WMUX_SetDataFlowControl
*
*	�������ܣ���������DATAͨ�������ر�ǣ�����б�Ҫ�Ļ�֪ͨ�û�DATAOUT.
*
*   ������������
*
*	�������أ���
*
\*========================================================================*/

static void WMUX_SetDataFlowControl(void)
{
	int newctrl = 0, oldctrl;
	if (!wmux.chanl[CHNL_DATA].opened)
		return;

	/* ����Ƿ����������������� */
	if (wmux.readyflag == 0)
		newctrl = 1;	
	
	if ((wmux.status & STATUS_DSR) == 0	&& (wmux.status & STATUS_DCD) == 0
		/*|| (wmux.status & STATUS_CTS)*/)	/* Flow control OFF */
		newctrl = 1;
	
	if (wmux.chanl[CHNL_DATA].flowctrl != newctrl)
	{
		if (newctrl == 0)
			MsgOut("MUX DATA channel FlowCtrl OFF\r\n");
		else
			MsgOut("MUX DATA channel FlowCtrl ON\r\n");
	}
	/* save old channel flow control flag */
	oldctrl = wmux.chanl[CHNL_DATA].flowctrl;
	/* �����������ر�� */
	wmux.chanl[CHNL_DATA].flowctrl = newctrl;
	/* �������������ʧ������û��ص�����֪ͨ�û�DATAOUT���Է������� */
	if (oldctrl == 1 && wmux.flowctrl == 0 && newctrl == 0)
	{
		MsgOut("MUX call data channel WMUX_DATAOUT\r\n");
		WMUX_CallBack(CHNL_DATA, WMUX_DATAOUT);
	}
	return;
}

/**************************************************************************\
*	BUFFER������
\**************************************************************************/
/*========================================================================*\
*
*	�������ƣ�WMBUF_GetDataSize
*
*	�������ܣ������������
*
*   ����������pbuf	ָ�򻺳�������
*
*	�������أ���������
*
\*========================================================================*/
#if 0
static int WMBUF_GetDataSize(BUFFER* pbuf)
{
	if (pbuf == NULL)
		return 0;
	return pbuf->datalen;
}
#endif
/*========================================================================*\
*
*	�������ƣ�WMBUF_CleanupData
*
*	�������ܣ�����������е�����
*
*   ����������pbuf	ָ�򻺳�������
*
*	�������أ�0		�ɹ�
*			  -1	ʧ��
*
\*========================================================================*/

static int WMBUF_CleanupData(BUFFER* pbuf)
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
*	�������ƣ�WMBUF_GetBufData
*
*	�������ܣ��ӻ������н�������
*
*   ����������pbuf		ָ�򻺳�������
*			  buf		�������ݻ�����
*			  buflen	����������
*
*	�������أ�ʵ�ʵõ�����������
*
\*========================================================================*/

static int WMBUF_GetBufData(BUFFER* pbuf, UINT8* buf, int buflen)
#if 1
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
#else
{
	int size, siz, first = 0, second = 0;

	if (pbuf == NULL || buf == NULL || buflen <= 0)
		return 0;

	/* ����ʵ�ʻ�����ݳ��� */
	siz = size = (buflen > pbuf->datalen ? pbuf->datalen : buflen);
	/* ��õ�һ�����ݳ��� */
	first = (pbuf->write > pbuf->data ? pbuf->write - pbuf->data:
		pbuf->buf + BUF_SIZE - pbuf->data);
	/* ��õڶ������ݳ��� */
	second = pbuf->datalen - first;

	if (first >= siz) /* �����һ�������㹻���㻺���� */
	{
		memcpy(buf, pbuf->data, siz);
		pbuf->data += siz;
	}
	else
	{	/* �ȿ�����һ������ */
		memcpy(buf, pbuf->data, first);
		siz -= first;
		/* �ٿ����ڶ������� */
		memcpy(buf + first, pbuf->buf, siz);
		pbuf->data = pbuf->buf + siz;
	}
	/* ���������ݼ��� */
	pbuf->datalen -= size;
	return size;
}
#endif

/*========================================================================*\
*
*	�������ƣ�WMBUF_PutBufData
*
*	�������ܣ��򻺳����з�������
*
*   ����������pbuf		ָ�򻺳�������
*			  data		����ָ��
*			  datalen	���ݳ���
*
*	�������أ�ʵ�ʷ������ݵ�����
*
\*========================================================================*/

static int WMBUF_PutBufData(BUFFER* pbuf, UINT8* data, int datalen)
#if 1
{
	UINT8 *front, *tail;
	int copyed, count = 0;

	if (pbuf == NULL || data == NULL || datalen <= 0)
		return 0;

	tail = pbuf->write;
	while (datalen > 0)
	{
		front = pbuf->data;
		if (tail + 1 == front || (tail + 1 == pbuf->buf + BUF_SIZE && front == pbuf->buf))
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
#else
{
	int free, size, siz, first = 0, second = 0;

	if (pbuf == NULL || data == NULL || datalen <= 0)
		return 0;

	/* ����ʵ�ʻ�����ݳ��� */
	free = BUF_SIZE - pbuf->datalen;
	siz = size = (datalen > free ? free : datalen);
	/* ��õ�һ�λ��������� */
	first = (pbuf->data > pbuf->write ? pbuf->data - pbuf->write:
		pbuf->buf + BUF_SIZE - pbuf->write);
	/* ��õڶ��λ��������� */
	second = free - first;

	if (first >= siz) /* �����һ�λ������㹻�������� */
	{
		memcpy(pbuf->write, data, siz);
		pbuf->write += siz;
	}
	else
	{
		/* ����д��һ�λ����� */
		memcpy(pbuf->write, data, first);
		siz -= first;
		/* ����д�ڶ��λ����� */
		memcpy(pbuf->buf, data + first, siz);
		pbuf->write = pbuf->buf + siz;
	}
	/* �������������� */
	pbuf->datalen += size;
	return size;
}
#endif

/**************************** End Of Program ******************************/
/* end of program wave_mux.c */
