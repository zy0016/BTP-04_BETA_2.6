/**************************************************************************\
*
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
*
* Model   : MP
*
* Purpose : multiplexer protocol
*  
* 
\**************************************************************************/


#define PLX_MUX_VERSON "PLX-MUX-1.1.1(2004.03.30)Q03"


#ifdef OLD_FILE_API
#include "fileapi.h"
#else
#include "fapi.h"
#endif
#include "DI_Uart.h"
#include "device.h"

#include "mux_lib.h"
#include "multchnl.h"

/**************************************************************************\
*
*	定义Driver
*
\**************************************************************************/

#ifdef SIEMENS_TC35 /* DRIVER */

	#include "siem_mux.h"

	/* 定义函数返回值 */
	#define MP_SUCCESS			CMUX_SUCCESS	/* 成功 */
	#define MP_FAILURE			CMUX_FAILURE	/* 失败 */

	/* 定义函数名称 */
	#define MP_Initial			CMUX_Initial
	#define MP_Startup			CMUX_Startup
	#define MP_Closedown		CMUX_Closedown
	#define MP_OpenChannel		CMUX_OpenChannel
	#define MP_CloseChannel		CMUX_CloseChannel
	#define MP_WriteChannel		CMUX_WriteChannel
	#define MP_ReadChannel		CMUX_ReadChannel
	#define MP_IoControl		CMUX_IoControl
	#define MP_OnDatain			CMUX_OnDatain
	#define MP_OnDataout		CMUX_OnDataout

#elif defined WAVECOM_2C /* DRIVER */

	#include "wave_mux.h"

	/* 定义函数返回值 */
	#define MP_SUCCESS			WMUX_SUCCESS	/* 成功 */
	#define MP_FAILURE			WMUX_FAILURE	/* 失败 */

	/* 定义函数名称 */
	#define MP_Initial			WMUX_Initial
	#define MP_Startup			WMUX_Startup
	#define MP_Closedown		WMUX_Closedown
	#define MP_OpenChannel		WMUX_OpenChannel
	#define MP_CloseChannel		WMUX_CloseChannel
	#define MP_WriteChannel		WMUX_WriteChannel
	#define MP_ReadChannel		WMUX_ReadChannel
	#define MP_IoControl		WMUX_IoControl
	#define MP_OnDatain			WMUX_OnDatain
	#define MP_OnDataout		WMUX_OnDataout

#elif defined TI_MODULE /* DRIVER */

	#include "ti_mux.h"

	/* 定义函数返回值 */
	#define MP_SUCCESS			TMUX_SUCCESS	/* 成功 */
	#define MP_FAILURE			TMUX_FAILURE	/* 失败 */

	/* 定义函数名称 */
	#define MP_Initial			TMUX_Initial
	#define MP_Startup			TMUX_Startup
	#define MP_Closedown		TMUX_Closedown
	#define MP_OpenChannel		TMUX_OpenChannel
	#define MP_CloseChannel		TMUX_CloseChannel
	#define MP_WriteChannel		TMUX_WriteChannel
	#define MP_ReadChannel		TMUX_ReadChannel
	#define MP_IoControl		TMUX_IoctlChannel
	#define MP_OnDatain			TMUX_OnDatain
	#define MP_OnDataout		TMUX_OnDataout

#endif /* DRIVER */


/**************************************************************************\
*
*	常量定义
*
\**************************************************************************/

/* 定义异步任务模式 */
#define ASYNC_TASK_MODE


/* 定义模块通道状态 */
#define STATE_INVALID		0
#define STATE_INITIAL		1
#define STATE_STARTED		2

/* 定义事件 */
#define EVENT_DATAIN		1
#define EVENT_DATAOUT		2
#define EVENT_MODEM			3

#define EVENT_QUEUE_LEN		256//50

/**************************************************************************\
*
*	数据结构及数据类型定义
*
\**************************************************************************/

/* UART控制结构 */
typedef struct UART_Control
{
	_CALLBACK	MAC_Dev_Event;	/* 串口回调函数指针 */
	void *		para;
	DWORD		EventCode;
}UART_CONTROL, *PUART_CONTROL;

/**************************************************************************\
*
*	全局变量定义
*
\**************************************************************************/

#ifdef ASYNC_TASK_MODE

static int multiplexer_task = 0;		/* 多通道事物处理实时任务 */
static OSHANDLE app_event = NULL;		/* 多通道事件 */

static int event_queue[EVENT_QUEUE_LEN];	/* 事件队列 */
static int queue_head = 0;					/* 队列头 */
static int queue_tail = 0;					/* 队列尾 */

#endif /* ASYNC_TASK_MODE */

static int dev_handle = -1;				/* 串口设备句柄 */
static int app_state = 0;				/* 多通道应用状态 */
static int startup_by_handle = 0;		/* 通过com_handlel来startup() */

/**************************************************************************\
*
*	函数声明
*
\**************************************************************************/

/* 外部函数声明 */
/*
extern int LIB_CreateTimer(APPTIMER* timer);
extern int LIB_StartTimer(APPTIMER* timer);
extern int LIB_SetTimerExpire(APPTIMER* timer, int microsecond);
extern int LIB_SetTimerFunc(APPTIMER* timer, void(*timeout)(void*), void* para);
extern int LIB_StopTimer(APPTIMER* timer);

extern OSHANDLE LIB_CreateEvent(char* eventname);
extern int LIB_WaitEvent(OSHANDLE event);
extern int LIB_SignalEvent(OSHANDLE event, unsigned long s);

extern int LIB_CreateMutex(MUTEX* mutex);
extern int LIB_WaitMutex(MUTEX* mutex);
extern int LIB_SignalMutex(MUTEX* mutex);
*/
/* 内部函数：一般函数 */

#ifdef ASYNC_TASK_MODE
static void MPAPP_MultiplexerTask(void* p);
#endif /* ASYNC_TASK_MODE */

static int MPAPP_DeviceConfig(int h_device, int baudrate);
static UINT MPAPP_UartCallBack(LONG Data, DWORD dwEventMask, DWORD p1, LONG p2);

/* 内部函数：串口收发函数 */
static int MPAPP_ReadCOM (unsigned char* buf, int bufsize);
static int MPAPP_WriteCOM(unsigned char* data, int datalen);

/**************************************************************************\
*
*	函数实现部分
*
\**************************************************************************/

//#define HEX_DEBUG_OUTPUT

#ifdef HEX_DEBUG_OUTPUT
#include "stdio.h"
#include "ctype.h"
#endif

#ifdef HEX_DEBUG_OUTPUT
#define BYTES_PER_LINE		20

static void DisplayHexData(char* name, unsigned char* data, int datalen)
{
	int i = 0;
	char hexbuf[BYTES_PER_LINE*3+4], txtbuf[BYTES_PER_LINE+4], tmpbuf[5];
	if (datalen <= 0)
		return;
	MsgOut("%s {%d}:\r\n", name, datalen);
	hexbuf[0] = 0;
	txtbuf[0] = 0;
	for (i = 0; i < datalen; )
	{
		sprintf(tmpbuf, "%02x ", data[i]);
		strcat(hexbuf, tmpbuf);
		if ((i+1) % (BYTES_PER_LINE/2) == 0)
			strcat(hexbuf, " ");

		if (isprint(data[i]))
		{
			sprintf(tmpbuf, "%c", data[i]);
			strcat(txtbuf, tmpbuf);
		}
		else
			strcat(txtbuf, ".");
		if (++i >= datalen)
		{
			while (i % BYTES_PER_LINE != 0)
			{
				strcat(hexbuf, "   ");
				if ((i+1) % (BYTES_PER_LINE/2) == 0)
					strcat(hexbuf, " ");
				i++;
			}
		}
		if (i % BYTES_PER_LINE == 0)
		{
			MsgOut(hexbuf);
			MsgOut("   ");
			MsgOut(txtbuf);
			MsgOut("\r\n");
			hexbuf[0] = 0;
			txtbuf[0] = 0;
		}
	}
	MsgOut("\r\n");
}
#endif /* HEX_DEBUG_OUTPUT */ 

/**************************************************************************\
*
*	接口函数实现部分 (interface function)
*
\**************************************************************************/

/*========================================================================*\
*
*	函数名称：MPAPP_Initial
*
*	函数功能：初始化协议以及应用模块
*
*   函数参数：无
*
*	函数返回：0		成功
*			  -1	失败
*
\*========================================================================*/

int MPAPP_Initial(void)
{
	if (app_state != STATE_INVALID)
		return -1;

#ifdef ASYNC_TASK_MODE
	if ((app_event = LIB_CreateEvent("multiplexer")) == NULL)
		return -1;
	/* 创建工作任务 */
	if ((multiplexer_task = LIB_CreateTask("multiplexer", TS_REALTIME|32, 
						MPAPP_MultiplexerTask, 2048, NULL)) == -1)
		return -1;
#endif /* ASYNC_TASK_MODE */

	/* 初始化multiplexer模块 */
	if (MP_Initial() != MP_SUCCESS)
		return -1;
	/* 成功 */
	app_state = STATE_INITIAL;
	MsgOut("\r\n---- "PLX_MUX_VERSON" ----\r\n");
	return 0;
}

/*========================================================================*\
*
*	函数名称：MPAPP_Startup
*
*	函数功能：启动多通道模式
*
*   函数参数：dev_name	串口设备名称
*
*	函数返回：0		成功
*			  -1	失败
*
\*========================================================================*/

int MPAPP_Startup(char* dev_name, int baudrate)
{
	int h_device;
	
	if (app_state != STATE_INITIAL)
		return -1;
	
#ifdef ASYNC_TASK_MODE
	queue_tail = queue_head = 0;
#endif

	/* 打开串口，设置串口 */
#ifdef OLD_FILE_API
	if ((h_device = OpenFile(dev_name, O_RDWR, 0, 0)) == -1)
#else
	if ((h_device = (int)CreateFile(dev_name, ACCESS_READ|ACCESS_WRITE,  0)) == -1)
#endif
		return -1;

	MPAPP_DeviceConfig(h_device, baudrate);
	dev_handle = h_device;
	
	if (MP_Startup(MPAPP_ReadCOM, MPAPP_WriteCOM) != MP_SUCCESS)
	{
#ifdef OLD_FILE_API
		CloseFile(h_device);
#else
		CloseFile((HANDLE)h_device);
#endif
		dev_handle = -1;
		return -1;
	}
	
	app_state = STATE_STARTED;
	return 0;
}

int MPAPP_Startup2(int com_handle)
{
	int h_device;
	
	if (app_state != STATE_INITIAL)
		return -1;
	
#ifdef ASYNC_TASK_MODE
	queue_tail = queue_head = 0;
#endif

	if ((h_device = com_handle) < 0)
		return -1;

	MPAPP_DeviceConfig(h_device, -1);
	dev_handle = h_device;
	
	if (MP_Startup(MPAPP_ReadCOM, MPAPP_WriteCOM) != MP_SUCCESS)
	{
		dev_handle = -1;
		return -1;
	}

	startup_by_handle = 1;
	app_state = STATE_STARTED;
	return 0;
}

/*========================================================================*\
*
*	函数名称：MPAPP_Closedown
*
*	函数功能：关闭多通道模式
*
*   函数参数：无
*
*	函数返回：0		成功
*			  -1	失败
*
\*========================================================================*/

int MPAPP_Closedown(void)
{
	if (app_state != STATE_STARTED)
		return -1;
	
	if (MP_Closedown() != MP_SUCCESS)
		return -1;

	if (!startup_by_handle)
	{
#ifdef OLD_FILE_API
		CloseFile(dev_handle);
#else
		CloseFile((HANDLE)dev_handle);
#endif
	}
	else
		startup_by_handle = 0;

	dev_handle = -1;
	app_state = STATE_INITIAL;
#ifdef ASYNC_TASK_MODE
	queue_tail = queue_head = 0;
#endif
	
	return 0;
}

/*========================================================================*\
*
*	函数名称：MPAPP_OpenFile
*
*	函数功能：打开一个通道
*
*   函数参数：name		通道名称
*			  mode		无意义
*			  attrib	无意义
*
*	函数返回：1,2,3		通道句柄
*			  -1		失败
*
\*========================================================================*/

int MPAPP_OpenFile(char* name, int mode, int attrib)
{
	int retval;
	mode = attrib = 0;
	if (app_state != STATE_STARTED)
		return -1;
	if ((retval = MP_OpenChannel(name)) < 0)
		return -1;
	return retval + HANDLE_BASE;
}

/*========================================================================*\
*
*	函数名称：MPAPP_CloseFile
*
*	函数功能：关闭一个通道
*
*   函数参数：handle	通道句柄
*
*	函数返回：0			成功
*			  -1		失败
*
\*========================================================================*/

int MPAPP_CloseFile(int handle)
{
	int retval;
	if (app_state != STATE_STARTED)
		return -1;
	handle -= HANDLE_BASE;
	retval = MP_CloseChannel(handle);
	if (retval != MP_SUCCESS)
		return -1;

	return 0;
}

/*========================================================================*\
*
*	函数名称：MPAPP_ReadFile
*
*	函数功能：从通道中读取数据
*
*   函数参数：handle	通道句柄
*			  buf		缓冲区
*			  bufsize	缓冲区长度
*
*	函数返回：>=0	实际读出数据的数量
*			  -1	失败
*
\*========================================================================*/

int MPAPP_ReadFile(int handle, void* buf, int bufsize)
{
	if (app_state != STATE_STARTED)
		return -1;
	handle -= HANDLE_BASE;
	return MP_ReadChannel(handle, buf, bufsize);
}

/*========================================================================*\
*
*	函数名称：MPAPP_WriteFile
*
*	函数功能：将数据写入通道
*
*   函数参数：handle	通道句柄
*			  data		数据指针
*			  datalen	数据长度			  
*
*	函数返回：>=0	实际写出数据的数量
*			  -1	失败
*
\*========================================================================*/

int MPAPP_WriteFile(int handle, void* data, int datalen)
{
	if (app_state != STATE_STARTED)
    {
        printf("\r\n MPAPP_WriteFile app_state :%d\r\n",app_state);
        return -1;
    }
	handle -= HANDLE_BASE;
	return MP_WriteChannel(handle, data, datalen);
}

/*========================================================================*\
*
*	函数名称：MPAPP_FileIoControl
*
*	函数功能：设置通道控制参数
*
*   函数参数：handle	通道句柄
*			  code		操作码
*			  pbuf		指向控制结构
*			  size		结构大小
*
*	函数返回：0			成功
*			  -1		失败
*
\*========================================================================*/

int MPAPP_FileIoControl(int handle, int code, void *pbuf, int size)
{
	if (app_state != STATE_STARTED)
		return -1;

	handle -= HANDLE_BASE;
	if (MP_IoControl(handle, code, (int)pbuf, size) != MP_SUCCESS)
		return -1;

	return 0;
}

/**************************************************************************\
*
*	内部函数实现部分 (internal function)
*
\**************************************************************************/

/*========================================================================*\
*
*	函数名称：MPAPP_MultiplexerTask
*
*	函数功能：多通道任务入口
*
*   函数参数：p		无意义
*
*	函数返回：无
*
\*========================================================================*/

#ifdef ASYNC_TASK_MODE

static void MPAPP_MultiplexerTask(void* p)
{
	int event;
	p = NULL;
	for ( ; ; )
	{
		LIB_WaitEvent(app_event);

		while (queue_head != queue_tail)
		{
			event = event_queue[queue_head];
			queue_head = (queue_head + 1) % EVENT_QUEUE_LEN;

			switch (event)
			{
			case EVENT_DATAIN:
				MP_OnDatain();
				break;
			case EVENT_DATAOUT:
				MP_OnDataout();
				break;
			case EVENT_MODEM:
				break;
			}
		}
	}
}

#endif /* ASYNC_TASK_MODE */

/*========================================================================*\
*
*	函数名称：MPAPP_DeviceConfig
*
*	函数功能：打开并设置物理串口设备
*
*   函数参数：h_device		串口设备名称
*			  baudrate		串口速率
*							-1:不进行设置; 
*							0 :设置为缺省速率9600
*							>0:指定速率
*							
*
*	函数返回：0		success
*			  -1	failure
*
\*========================================================================*/

static int MPAPP_DeviceConfig(int h_device, int baudrate)
{
	int				parg = 3;
	int				modemctl = 1;
	UART_MODE		para;
	UART_CONTROL	Uart_Control;

	if (baudrate != -1)
	{
		/* set default baudrate */
		baudrate = (baudrate == 0 ? 9600 : baudrate); 
		/* Modem设置 */
		para.slen		= (DWORD)sizeof(UART_MODE);
		para.baudrate	= baudrate;				/* 波特率 */
		para.datalen	= 8;					/* 数据位0x000f */
		para.parity		= UART_NOPARITY;		/* 校验位 */
		para.stopbits	= UART_ONESTOPBIT;		/* 停止位 */
		para.flowctl	= 0;					/* 需要流控制 */
		
#ifdef OLD_FILE_API
		FILE_ioctl(h_device, IO_UART_SETMODE, &para, sizeof(UART_MODE));
#else
		DeviceIoControl((HANDLE)h_device, IO_UART_SETMODE, &para, sizeof(UART_MODE));
#endif
	}

	/* 设置串口回调函数 */
	Uart_Control.MAC_Dev_Event = MPAPP_UartCallBack;
	Uart_Control.para = NULL;
	Uart_Control.EventCode = EV_DATAIN | EV_DATAOUT | EV_MODEM;

#ifdef OLD_FILE_API
	FILE_ioctl(h_device, IO_UART_SETMODEMCTL, &modemctl, 0 );
	FILE_ioctl(h_device, IO_SETCALLBACK, &Uart_Control, sizeof(UART_CONTROL) );
	/* clean up rx/tx buffer */
	FILE_ioctl(h_device, IO_DUMPBUF, &parg, sizeof(int));
#else
	DeviceIoControl((HANDLE)h_device, IO_UART_SETMODEMCTL, &modemctl, 0 );
	DeviceIoControl((HANDLE)h_device, IO_SETCALLBACK, 
		&Uart_Control, sizeof(UART_CONTROL) );
	/* clean up rx/tx buffer */
	DeviceIoControl((HANDLE)h_device, IO_DUMPBUF, &parg, sizeof(int));
#endif

	return 0;
}

/*========================================================================*\
*
*	函数名称：MPAPP_UartCallBack
*
*	函数功能：物理串口的回调函数入口
*
*   函数参数：Data		无意义
*			  UartEvent	UART事件
*			  p1,p2		无意义
*
*	函数返回：0		成功
*
\*========================================================================*/

#ifdef ASYNC_TASK_MODE

static int AppendEvent(int event)
{
	int tail;
	/* 将UART事件加入事件队列 */
	tail = (queue_tail + 1) % EVENT_QUEUE_LEN;
	if (tail == queue_head)
	{
		/* 事件队列满 */
		MsgOut("Multiplexer Event Queue full !!!The event:%d\r\n",event);
		return (UINT)(-1);
	}
	else
	{
		/* 向事件队列中添加事件 */
		event_queue[queue_tail] = event;
		queue_tail = tail;
	}
	LIB_SignalEvent(app_event, 1);
	return 0;
}

static UINT MPAPP_UartCallBack(LONG Data, DWORD UartEvent, DWORD p1, LONG p2)
{
	p1 = p2 = 0L;
	Data = 0L;

	if (UartEvent & EV_DATAIN)
		AppendEvent(EVENT_DATAIN);

	if (UartEvent & EV_DATAOUT)
		AppendEvent(EVENT_DATAOUT);

	if (UartEvent & EV_MODEM)
		AppendEvent(EVENT_MODEM);

	return 0;
}

#else /* ASYNC_TASK_MODE */

static UINT MPAPP_UartCallBack(LONG Data, DWORD UartEvent, DWORD p1, LONG p2)
{
	p1 = p2 = 0L;
	Data = 0L;

	if (UartEvent & EV_DATAIN)
		MP_OnDatain();

	if (UartEvent & EV_DATAOUT)
		MP_OnDataout();

	if (UartEvent & EV_MODEM)
		;
	return 0;
}

#endif /* ASYNC_TASK_MODE */

/*========================================================================*\
*
*	函数名称：MPAPP_ReadCOM
*
*	函数功能：串口读取函数
*
*   函数参数：buf		缓冲区
*			  bufsize	缓冲区长度
*
*	函数返回：>=0	实际读出数据的数量
*			  -1	出错
*
\*========================================================================*/

static int MPAPP_ReadCOM(unsigned char* buf, int bufsize)
{
#ifdef OLD_FILE_API
	return ReadFile(dev_handle, buf, bufsize);
#else
#ifdef HEX_DEBUG_OUTPUT
	int retval = ReadFile((HANDLE)dev_handle, buf, bufsize);
	DisplayHexData("MUX recv", buf, retval);
	return retval;
#else /* HEX_DEBUG_OUTPUT */
	return ReadFile((HANDLE)dev_handle, buf, bufsize);
#endif /* HEX_DEBUG_OUTPUT */
#endif
}

/*========================================================================*\
*
*	函数名称：MPAPP_WriteCOM
*
*	函数功能：串口发送函数
*
*   函数参数：data		数据指针
*			  datalen	数据长度
*
*	函数返回：>=0	实际写出数据的数量
*			  -1	出错
*
\*========================================================================*/

static int MPAPP_WriteCOM(unsigned char* data, int datalen)
{
#ifdef OLD_FILE_API
	return WriteFile(dev_handle, data, datalen);
#else
#ifdef HEX_DEBUG_OUTPUT
	int retval = WriteFile((HANDLE)dev_handle, data, datalen);
	DisplayHexData("MUX send", data, retval);
	return retval;
#else /* HEX_DEBUG_OUTPUT */
	return WriteFile((HANDLE)dev_handle, data, datalen);
#endif /* HEX_DEBUG_OUTPUT */
#endif
}

/**************************** End Of Program ******************************/
/* end of program multchnl.c */
