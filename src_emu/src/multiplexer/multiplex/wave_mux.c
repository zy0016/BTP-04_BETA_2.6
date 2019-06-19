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
*	程序名称: wireless modules multiplex driver on WAVECOM 2C
*
*	文件名称: wave_mux.c
*
*	作者姓名: 
*
*	程序功能: multiplex physical serial interface on WAVECOM 2C
*
*	开发日期：2002.07.24
*
\**************************************************************************/

/* include header files here */
#include "stdio.h"
#include "mux_lib.h"
#include "wave_mux.h"

/**************************************************************************\
*
*	Debugoutput定义
*
\**************************************************************************/

#ifdef _EMULATE_
extern void MsgOut(char *format, ... );
#else
static void MsgOut(char* format, ... ){format=NULL;}
#endif

/**************************************************************************\
*
*	常量定义
*
\**************************************************************************/

/* define constant here */
/* 定义模块通道状态 */
#define STATE_INVALID		0	/* invalided modular */
#define STATE_INITIAL		1	/* initialized OK */
#define STATE_STARTING		2	/* starting up multiplex mode */
#define STATE_MULTIPLEX		3	/* multiplex mode */
#define STATE_DOWNING		4	/* closing down multiplex mode */

/* 最大帧数据数量 */
#define MAX_FRAME_SIZE		1020 /* length field have 11 bits */

/* 缓冲区定义 */
#define BUF_SIZE			(1024 * 4)
#define FRM_BUFSIZE			(MAX_FRAME_SIZE + 4)

/* 定义临界段操作 */
#define ENTER_MUTEX			LIB_WaitMutex  (&wmux.mutex)
#define LEAVE_MUTEX			LIB_SignalMutex(&wmux.mutex)

/* 定义数据帧类型 */
#define FRAME_AT			0xAA	/* AT Command Frame */
#define FRAME_DATA			0xDD	/* Data service Frame */

/* 定义数据包类型 */
#define PACKET_AT			0x1D	/* AT command */
#define PACKET_DATA			0		/* data service */
#define PACKET_STATUS		1		/* signal emulate */
#define PACKET_READY		2		/* data channel is enabled */
#define PACKET_BUSY			3		/* data channel is disabled  */

/* WAVECOM 定义的状态位 */
#define STATUS_DTR			(1 << 7)	/* Data Terminal Ready (SA) */
#define STATUS_RTS			(1 << 6)	/* Ready to Send (SB) */
#define STATUS_DSR			STATUS_DTR	/* Data Set Ready (复用SA) */
#define STATUS_DCD			STATUS_RTS	/* Data Carrier Detected (复用SB) */
#define STATUS_CTS			(1 << 5)	/* Clear to Send (X) */
#define STATUS_BRK			(1 << 4)	/* Break signal */
#define STATUS_RING			(1 << 3)	/* Ring Singal */

/* 定义multiplexer模式启动命令 */
#define AT_WMUX_ON			"AT+WMUX=1\r"
#define AT_WMUX_OFF			"ATH+WMUX=0\r"

/* 定义multiplex mode ON/OFF */
#define MUX_ON				1
#define MUX_OFF				0

/* 定义阻塞超时事件 */
#define BLOCK_TIMEOUT		(1000)	/* 1 second */
#define WMUX_TIMEOUT		(1500)

/**************************************************************************\
*
*	数据结构及数据类型定义
*
\**************************************************************************/

/* define any data structure here */
typedef unsigned char	UINT8;

/* 定义接收状态 */
typedef enum 
{
	S_START_PATTERN,
	S_LSB_OF_LENGTH, 
	S_MSB_AND_TYPE,
	S_USER_DATA, 
	S_CHECK_SUM
} SCANSTATE;

/* 定义channel标识 */
typedef enum { CHNL_AT, CHNL_DATA, CHNL_NUM };

/* UART控制结构 */
typedef struct
{
	_CALLBACK	userfunc;	/* 串口回调函数指针 */
	void *		para;
	DWORD		EventCode;
}	UART_CALLBK;

/* 定义帧结构 */
typedef struct
{
	int		frm_type;			/* frame   类型		*/
	int		pkg_type;			/* package 类型		*/
	UINT8	data[2048];			/* user data erea */
	int		datalen;			/* data length */
}	FRAME;

/* 定义缓冲区结构 */
typedef struct
{
	UINT8	buf[BUF_SIZE];		/* 缓冲区		*/
	UINT8*	data;				/* 数据指针		*/
	UINT8*	write;				/* 写指针		*/
	int		datalen;			/* 数据长度		*/
}	BUFFER;

/* 定义通道管理结构 */
typedef struct
{
	int			opened;			/* channel opened flag */
	int			flowctrl;		/* channel flow control flag */
	UART_CALLBK	callback;		/* 回调函数	*/
	BUFFER		rxbuf;			/* 接收缓冲区	*/
}	CHANNEL;

/* 定义Multiplexer管理结构 */
typedef struct
{
	int			state;			/* mux driver state */
	UINT8		status;			/* mux driver status */
#if (_HOPEN_VER >= 200)
	MUTEX		mutex;			/* 互斥锁		*/
#else
	OSHANDLE	mutex;			/* 互斥锁	*/
#endif
	APPTIMER	timer;			/* 通道计时器	*/
	OSHANDLE	event;			/* 通道事件		*/

	IOCOM		write;			/* 写串口函数 */
	IOCOM		read;			/* 读串口函数 */
	CHANNEL		chanl[CHNL_NUM];	/* AT & DATA channel */
	UINT8		mybuf[FRM_BUFSIZE];
	UINT8		txbuf[BUF_SIZE];	/* 模块发送缓冲区 */
	UINT8*		data;			/* 缓冲区数据指针 */
	int			datalen;		/* 缓冲区数据数量 */
	SCANSTATE	scanstate;		/* frame scan state */
	FRAME		frame;			/* MUX frame structure */
	int			flowctrl;		/* uart flow control flag */
	int			timeoutflag;	/* timeout flag */
	int			readyflag;		/* Data Ready Flag */
}	WMUX;


/**************************************************************************\
*
*	全局变量定义
*
\**************************************************************************/

/* define globle varible here */

/* multiplexer控制结构 */
static WMUX wmux;

/**************************************************************************\
*
*	函数声明
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

/* 内部函数：BUFFER处理类 */
#define WMBUF_IsEmpty(pbuf)		((pbuf)->data == (pbuf)->write)
//static int WMBUF_GetDataSize(BUFFER* buf);
static int WMBUF_CleanupData(BUFFER* buf);
static int WMBUF_GetBufData (BUFFER* buf, UINT8* pbuf, int buflen);
static int WMBUF_PutBufData (BUFFER* buf, UINT8* data, int datalen);

/**************************************************************************\
*
*	函数实现部分
*
\**************************************************************************/

/**************************************************************************\
*
*	接口函数实现部分 (interface function)
*
\**************************************************************************/

/*========================================================================*\
*
*	函数名称：WMUX_Initial (system function)
*
*	函数功能：initialize WMUX mudole, alloc nesscery resource.
*
*   函数参数：无
*
*	函数返回：WMUX_SUCCESS	成功
*			  WMUX_FAILURE	失败
*
\*========================================================================*/

int WMUX_Initial(void)
{
	if (wmux.state != STATE_INVALID)
		return WMUX_FAILURE;

	memset(&wmux, 0, sizeof(WMUX));

	/* 创建模块MUTEX LOCK */
	if (LIB_CreateMutex(&wmux.mutex) == -1)
		return WMUX_FAILURE;

	/* 创建通道计时器 */
	LIB_CreateTimer(&wmux.timer);

	/* 创建通道事件 */
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
*	函数名称：WMUX_Startup
*
1*	函数功能：startup WAVECOM wireless module into multiplex mode.
*
*   函数参数：fpread		读串口函数入口地址
*			  fpwrite		写串口函数入口地址
*
*	函数返回：WMUX_SUCCESS	成功
*			  WMUX_FAILURE	失败
*
\*========================================================================*/

int WMUX_Startup(IOCOM fpread, IOCOM fpwrite)
{
	UINT8	buf[256];
	int		len, cmdlen;
	/* 必须设置串口读写函数入口 */
	if (fpread == NULL || fpwrite == NULL)
		return WMUX_FAILURE;

	ENTER_MUTEX;

	/* 只有在STATE_INITIAL时才能执行该函数 */
	if (wmux.state != STATE_INITIAL)
		goto ERROR_NOOP;

	WMUX_ResetMultiplexer();
	wmux.read  = fpread;
	wmux.write = fpwrite;
	wmux.state = STATE_STARTING;

	/* 清空串口缓冲区 */
	while (fpread(buf, 256) >= 0)
		;

	/* 发送AT+WMUX=1启动wmux模式 */
	strcpy((char*)buf, AT_WMUX_ON);
	cmdlen = (int)strlen((char*)buf);
	if ((len = fpwrite(buf, cmdlen)) != cmdlen)
	{
		len = (len < 0 ? 0 : len);
		wmux.datalen = cmdlen - len;
		memcpy(wmux.txbuf, &buf[len], wmux.datalen);
	}

	/* 等待回答OK */
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
*	函数名称：WMUX_Closedown
*
*	函数功能：recover WAVECOM wireless module to AT command mode.
*
*   函数参数：无
*
*	函数返回：WMUX_SUCCESS	成功
*			  WMUX_FAILURE	失败
*
\*========================================================================*/

int WMUX_Closedown(void)
{
	UINT8	buf[256];
	int		i, cmdlen;

	ENTER_MUTEX;

	/* 只有在STATE_MULTIPLEX状态时才能执行 */
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

	/* 发送AT+WMUX=0关闭wmux模式 */
	strcpy((char*)buf, AT_WMUX_OFF);
	cmdlen = (int)strlen((char*)buf);

	if (WMUX_SendFrameData(CHNL_AT, PACKET_AT, buf, cmdlen) == WMUX_FAILURE)
		goto ERROR_HANDLE;

	WMUX_WaitEvent(WMUX_TIMEOUT);

	if (wmux.state != STATE_INITIAL)
		goto ERROR_HANDLE;

	/* 对通道关闭成功 */
	WMUX_ResetMultiplexer();

	/* 清除各个通道 */
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
*	函数名称：WMUX_OpenChannel
*
*	函数功能：Open a wavecom multiplex channel.
*
*   函数参数：name		通道名称
*
*	函数返回：>=0		通道句柄
*			  -1		失败
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
*	函数名称：WMUX_CloseChannel
*
*	函数功能：Close a opened multiplex channel.
*
*   函数参数：handle		通道句柄
*
*	函数返回：WMUX_SUCCESS	成功
*			  WMUX_FAILURE	失败
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
*	函数名称：WMUX_WriteChannel
*
*	函数功能：transmit user data into wavecom multiplex channel.
*
*   函数参数：handle		通道句柄
*			  data			数据指针
*			  datalen		数据长度
*
*	函数返回：>=0	实际写出数据的数量
*			  -1	出错
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

	/* 只有在该通道在OPEN并且没有FLOWCTRL状态下才能发送数据 */
	if (wmux.chanl[handle].opened != 0
		&& wmux.chanl[handle].flowctrl == 0
		&& wmux.flowctrl == 0)
	{
		/* 开始发送用户数据 */
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
*	函数名称：WMUX_ReadChannel
*
*	函数功能：receive remote data to user buffer.
*
*   函数参数：handle		通道句柄
*			  buf			缓冲区
*			  bufsize		缓冲区长度
*
*	函数返回：>=0	实际读出数据的数量
*			  -1	出错
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
*	函数名称：WMUX_IoControl
*
*	函数功能：control channel special action with operation code.
*
*   函数参数：handle		通道句柄
*			  opcode		操作码
*			  value			数值
*			  size			大小
*
*	函数返回：WMUX_SUCCESS	成功
*			  WMUX_FAILURE	失败
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
*	函数名称：WMUX_OnDatain
*
*	函数功能：数据到来回调函数
*
*   函数参数：无
*
*	函数返回：WMUX_SUCCESS	成功
*			  WMUX_FAILURE	失败
*
\*========================================================================*/

int WMUX_OnDatain(void)
{
	/* 将DLC0的接收缓冲区用作物理接收缓冲区 */
	static UINT8	  buf[100]; 
	static int		  datalen = 0;
	static UINT8	  fcs = 0;

	int		readlen, i, retval;

	/* 读取数据 */
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
					/* 成功的获得一帧进行处理 */
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
*	函数名称：WMUX_OnDataout
*
*	函数功能：可以继续发送数据的回调函数
*
*   函数参数：无
*
*	函数返回：WMUX_SUCCESS	成功
*			  WMUX_FAILURE	失败
*
\*========================================================================*/

int WMUX_OnDataout(void)
{
	int sendlen, i;

	ENTER_MUTEX;

	/* 发送缓冲区中尚未被发送完毕的数据 */
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
*	内部函数实现部分 (internal function)
*
\**************************************************************************/

/*========================================================================*\
*
*	函数名称：WMUX_ResetMultiplexer
*
*	函数功能：复位multiplexer模块
*
*   函数参数：无
*
*	函数返回：WMUX_SUCCESS	成功
*			  WMUX_FAILURE	失败
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
*	函数名称：WMUX_ResetChannel
*
*	函数功能：复位指定的通道
*
*   函数参数：i		通道标识
*
*	函数返回：WMUX_SUCCESS	成功
*			  WMUX_FAILURE	失败
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
*	函数名称：WMUX_WaitEvent
*
*	函数功能：等待接收AT+WMUX的应答
*
*   函数参数：microsecond	阻塞时间
*
*	函数返回：WMUX_SUCCESS	成功，事件发生
*			  WMUX_FAILURE	失败，超时
*
\*========================================================================*/

static int WMUX_WaitEvent(int microsecond)
{
	/* 清除超时标记 */
	wmux.timeoutflag = 0;

	/* 设置计时器 */
	LIB_SetTimerFunc(&wmux.timer, WMUX_TimeOut, NULL);
	LIB_SetTimerExpire(&wmux.timer, microsecond);

	/* 启动计时器 */
	LIB_StartTimer(&wmux.timer);

	LEAVE_MUTEX;
	LIB_WaitEvent(wmux.event);
	ENTER_MUTEX;

	/* 终止时钟 */
	LIB_StopTimer(&wmux.timer);

	/* 如果超时 */
	if (wmux.timeoutflag != 0)
		return WMUX_FAILURE;

	return WMUX_SUCCESS;
}

/*========================================================================*\
*
*	函数名称：WMUX_TimeOut
*
*	函数功能：超时回调函数
*
*   函数参数：param		无意义
*
*	函数返回：无
*
\*========================================================================*/

static void WMUX_TimeOut(void* param)
{
	param = NULL;
	ENTER_MUTEX;
	if (wmux.state == STATE_STARTING || wmux.state == STATE_DOWNING)
	{
		/* 设置timeout标记 */
		wmux.timeoutflag = 1;
		MsgOut("MUX timeout\r\n");
		/* 唤醒等待事件 */
		LIB_SignalEvent(wmux.event, 1);
	}
	LEAVE_MUTEX;
	return;
}

/*========================================================================*\
*
*	函数名称：WMUX_SendFrameData
*
*	函数功能：将指定的帧放到mplx的txbuf中，如果无流控则开始发送
*
*   函数参数：DLCI			通道号
*			  frametype		帧的类型 (AT_CMD, DATA, STATUS, READY, BUSY)
*			  data			数据指针
*			  datalen		数据长度
*
*	函数返回：WMUX_SUCCESS	成功
*			  WMUX_FAILURE	失败
*			  WMUX_ASYNC	异步发送
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
	
	/* 如果没有流控则直接发送数据 */
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
	/* 如果没有发送完毕，则将剩余的数据保留在txbuf等待异步发送，
	 * 并设置流控 
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
*	函数名称：WMUX_FrameCheckSum
*
*	函数功能：计算数据的校验和
*
*   函数参数：data		数据指针
*			  datalen	数据长度
*
*	函数返回：Frame Check Sum value
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
*	函数名称：WMUX_ProcRxFrame
*
*	函数功能：处理接收到的一帧数据
*
*   函数参数：frame		接收到的帧结构化数据
*
*	函数返回：WMUX_SUCCESS	成功
*			  WMUX_FAILURE	失败
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
			/* sent: AT+WMUX=1\r startup MUX mode，
			 * wait: mux string "\r\nOK\r\n"。
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
			/* sent: AT+WMUX=0\r startup MUX mode，
			 * wait: mux string "\r\nERROR\r\n"。
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
*	函数名称：WMUX_CheckWMUXResponse
*
*	函数功能：检查AT+WMUX命令的应答
*
*   函数参数：data		应答数据
*			  datalen	应答数据长度
*
*	函数返回：WMUX_SUCCESS	返回"\r\nOK\r\n"
*			  WMUX_FAILURE	返回"\r\nERROR\r\n"
*			  WMUX_ASYNC	没有指定的返回
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
*	函数名称：WMUX_WriteDevice
*
*	函数功能：向串口设备写入数据
*
*   函数参数：data		数据指针
*			  datalen	数据长度
*
*	函数返回：实际写入的数量
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
*	函数名称：WMUX_ReadDevice
*
*	函数功能：从串口设备中读取数据
*
*   函数参数：buf		缓冲区地址
*			  bufsize	缓冲区的大小
*
*	函数返回：实际读出的数据数量
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
*	函数名称：WMUX_CallBack
*
*	函数功能：用户回调函数
*
*   函数参数：DLC		通道
*			  event		回调事件 (DATAIN，DATAOUT，BROKEN)
*
*	函数返回：无
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
*	函数名称：WMUX_SetDataFlowControl
*
*	函数功能：重新设置DATA通道的流控标记，如果有必要的话通知用户DATAOUT.
*
*   函数参数：无
*
*	函数返回：无
*
\*========================================================================*/

static void WMUX_SetDataFlowControl(void)
{
	int newctrl = 0, oldctrl;
	if (!wmux.chanl[CHNL_DATA].opened)
		return;

	/* 检查是否重新满足流控条件 */
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
	/* 重新设置流控标记 */
	wmux.chanl[CHNL_DATA].flowctrl = newctrl;
	/* 如果流控条件消失则调用用户回调函数通知用户DATAOUT可以发送数据 */
	if (oldctrl == 1 && wmux.flowctrl == 0 && newctrl == 0)
	{
		MsgOut("MUX call data channel WMUX_DATAOUT\r\n");
		WMUX_CallBack(CHNL_DATA, WMUX_DATAOUT);
	}
	return;
}

/**************************************************************************\
*	BUFFER处理类
\**************************************************************************/
/*========================================================================*\
*
*	函数名称：WMBUF_GetDataSize
*
*	函数功能：获得数据数量
*
*   函数参数：pbuf	指向缓冲区对象
*
*	函数返回：数据数量
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
*	函数名称：WMBUF_CleanupData
*
*	函数功能：清除缓冲区中的数据
*
*   函数参数：pbuf	指向缓冲区对象
*
*	函数返回：0		成功
*			  -1	失败
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
*	函数名称：WMBUF_GetBufData
*
*	函数功能：从缓冲区中接收数据
*
*   函数参数：pbuf		指向缓冲区对象
*			  buf		接收数据缓冲区
*			  buflen	缓冲区长度
*
*	函数返回：实际得到的数据数量
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

	/* 计算实际获得数据长度 */
	siz = size = (buflen > pbuf->datalen ? pbuf->datalen : buflen);
	/* 获得第一段数据长度 */
	first = (pbuf->write > pbuf->data ? pbuf->write - pbuf->data:
		pbuf->buf + BUF_SIZE - pbuf->data);
	/* 获得第二段数据长度 */
	second = pbuf->datalen - first;

	if (first >= siz) /* 如果第一段数据足够满足缓冲区 */
	{
		memcpy(buf, pbuf->data, siz);
		pbuf->data += siz;
	}
	else
	{	/* 先拷贝第一段数据 */
		memcpy(buf, pbuf->data, first);
		siz -= first;
		/* 再拷贝第二段数据 */
		memcpy(buf + first, pbuf->buf, siz);
		pbuf->data = pbuf->buf + siz;
	}
	/* 缓冲区数据减少 */
	pbuf->datalen -= size;
	return size;
}
#endif

/*========================================================================*\
*
*	函数名称：WMBUF_PutBufData
*
*	函数功能：向缓冲区中放入数据
*
*   函数参数：pbuf		指向缓冲区对象
*			  data		数据指针
*			  datalen	数据长度
*
*	函数返回：实际放入数据的数量
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

	/* 计算实际获得数据长度 */
	free = BUF_SIZE - pbuf->datalen;
	siz = size = (datalen > free ? free : datalen);
	/* 获得第一段缓冲区长度 */
	first = (pbuf->data > pbuf->write ? pbuf->data - pbuf->write:
		pbuf->buf + BUF_SIZE - pbuf->write);
	/* 获得第二段缓冲区长度 */
	second = free - first;

	if (first >= siz) /* 如果第一段缓冲区足够满足数据 */
	{
		memcpy(pbuf->write, data, siz);
		pbuf->write += siz;
	}
	else
	{
		/* 先填写第一段缓冲区 */
		memcpy(pbuf->write, data, first);
		siz -= first;
		/* 先填写第二段缓冲区 */
		memcpy(pbuf->buf, data + first, siz);
		pbuf->write = pbuf->buf + siz;
	}
	/* 缓冲区数据增加 */
	pbuf->datalen += size;
	return size;
}
#endif

/**************************** End Of Program ******************************/
/* end of program wave_mux.c */
