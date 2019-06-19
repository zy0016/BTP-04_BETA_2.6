/**************************************************************************\
*
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
*
* Model   : MUX agent
*
* Purpose : mulitple channel agent
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
*	程序名称: MUX AGENT
*
*	文件名称: muxagent.c
*
*	作者姓名: 
*
*	程序功能: 协调管理MUX通道和物理串口
*
*	开发日期：2003.04.02
*
\**************************************************************************/

#include "fapi.h"
#include "DI_Uart.h"
#include "device.h"

//fufu 0701 mux
#include <fcntl.h>
//end 0701 mux

#include "mux_lib.h"
#include "multchnl.h"
#include "muxagent.h"


/**************************************************************************\
*
*	常量定义
*
\**************************************************************************/

/* max comm port number */
#define AGENT_NUM		2

/* agent state */
enum {COM, MUX, STATE_NUM};


extern int  write  (int fildes, const void * buf, size_t nbyte);
extern int  close (int fildes);
extern int Sleep ( int miniseconds );
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

typedef struct
{
	int (*open)(char* name, int mode, int attrib);
	int (*close)(int handle);
	int (*read)(int handle, void* buf,  int bufsize);
	int (*write)(int handle, void* data, int datalen);
	int (*ioctl)(int handle, int code, void *pbuf, int size);
}	COMFUNC;


typedef struct
{
	int state;
	int handle[STATE_NUM];
	UART_CONTROL uart_callback;
}	MUXAGENT;

/**************************************************************************\
*
*	函数声明
*
\**************************************************************************/

static int MUX_CommIoctl(MUXAGENT* agent, int code, void *pbuf, int size);
static int MUX_CommResetCallback(MUXAGENT* agent);

/* file system virtual interface */
static int FS_OpenFile(char* name, int mode, int attrib);
static int FS_CloseFile(int handle);
static int FS_ReadFile(int handle, void* buf, int bufsize);
static int FS_WriteFile	(int handle, void* data, int datalen);
static int FS_FileIoctl(int handle, int code, void *pbuf, int size);

/**************************************************************************\
*
*	全局变量定义
*
\**************************************************************************/

static COMFUNC comfunc[STATE_NUM] = 
{
	{
		FS_OpenFile,
		FS_CloseFile,
		FS_ReadFile,
		FS_WriteFile,
		FS_FileIoctl
	},
	{
		MPAPP_OpenFile,
		MPAPP_CloseFile,
		MPAPP_ReadFile,
		MPAPP_WriteFile,
		MPAPP_FileIoControl
	}
};

static int init_mux = 0;

static MUXAGENT mux_agent[AGENT_NUM] =
{
	{COM, {-1, -1}},
	{COM, {-1, -1}}
};

/**************************************************************************\
*
*	函数实现部分
*
\**************************************************************************/

/*========================================================================*\
*
*	函数名称：MUX_OpenFile
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

int MUX_OpenFile(char* name, int mode, int attrib)
{
	int i;
	for (i = 0; i < AGENT_NUM && strncmp(name, "MUX-", 4) != 0; i ++)
	{
		if (mux_agent[i].state == COM && mux_agent[i].handle[COM] == -1)
		{
			memset(&mux_agent[i].uart_callback, 0, sizeof(UART_CONTROL));
			return mux_agent[i].handle[COM] = FS_OpenFile(name, mode, attrib);
		}
	}	
	return MPAPP_OpenFile(name, mode, attrib);
}

/*========================================================================*\
*
*	函数名称：MUX_CloseFile
*
*	函数功能：关闭一个通道
*
*   函数参数：handle	通道句柄
*
*	函数返回：0			成功
*			  -1		失败
*
\*========================================================================*/

int MUX_CloseFile(int handle)
{
	int i;
	for (i = 0; i < AGENT_NUM; i++)
	{
		if (mux_agent[i].state == COM && mux_agent[i].handle[COM] == handle)
		{
			mux_agent[i].handle[COM] = -1;
			return FS_CloseFile(handle);
		}
	}
	return MPAPP_CloseFile(handle);
}

/*========================================================================*\
*
*	函数名称：MUX_ReadFile
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

int MUX_ReadFile(int handle, void* buf, int bufsize)
{
	int i, state, s_handle;
	if (buf == NULL || bufsize < 0)
		return -1;
	for (i = 0; i < AGENT_NUM; i++)
	{
		state = mux_agent[i].state;
		s_handle = mux_agent[i].handle[state];
		if (mux_agent[i].handle[COM] == handle)
			return comfunc[state].read(s_handle, buf, bufsize);
	}
	return MPAPP_ReadFile(handle, buf, bufsize);
}

/*========================================================================*\
*
*	函数名称：MUX_WriteFile
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

int MUX_WriteFile(int handle, void* data, int datalen)
{
	int i, state, s_handle;
	if (data == NULL || datalen < 0)
		return -1;
	for (i = 0; i < AGENT_NUM; i++)
	{
		state = mux_agent[i].state;
		s_handle = mux_agent[i].handle[state];
		if (mux_agent[i].handle[COM] == handle)
			return comfunc[state].write(s_handle, data, datalen);
	}
	return MPAPP_WriteFile(handle, data, datalen);
}

/*========================================================================*\
*
*	函数名称：MUX_FileIoControl
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

int MUX_FileIoctl(int handle, int code, void *pbuf, int size)
{
	int i;
	for (i = 0; i < AGENT_NUM; i++)
	{
		if (handle != -1 && mux_agent[i].handle[COM] == handle)
			return MUX_CommIoctl(&mux_agent[i], code, pbuf, size);
	}
	return MPAPP_FileIoControl(handle, code, pbuf, size);
}

/* handle comm Iocontrol function, 
 * especially for switching MUX mode ON/OFF .
 */
static int MUX_CommIoctl(MUXAGENT* agent, int code, void *pbuf, int size)
{
	int state, s_handle, retval;
	
	/* process switching MUX mode ON/OFF */
	if (code == IO_UART_SETMODEMCTL && pbuf != NULL)
	{
		if (agent->state == COM 
			&& strcmp(pbuf, MUX_ON) == 0
			&& strlen(MUX_ON) == (unsigned)size)
		{
			if (init_mux == 0)
			{
				if (MPAPP_Initial() != 0)
					return -1;
				init_mux = 1;
			}
			
			if (MPAPP_Startup2(agent->handle[COM]) != 0)
			{
				MUX_CommResetCallback(agent);
				return -1;
			}

			agent->state = MUX;
			/* open first mux channel */
#ifdef WAVECOM_2C
			agent->handle[MUX] = MPAPP_OpenFile(MPAPI_CHANNEL_AT, 0, 0);
#elif defined SIEMENS_TC35
			agent->handle[MUX] = MPAPP_OpenFile(MPAPI_CHANNEL2, 0, 0);
#elif defined TI_MODULE
			agent->handle[MUX] = MPAPP_OpenFile(MPAPI_CHANNEL2, 0, 0);
#endif
			if (agent->uart_callback.MAC_Dev_Event != NULL)
			{
				MPAPP_FileIoControl(agent->handle[MUX], IO_SETCALLBACK, 
					&agent->uart_callback, sizeof(UART_CONTROL));
			}
			return 0;
		}
		else if (agent->state == MUX
			&& strcmp(pbuf, MUX_OFF) == 0
			&& strlen(MUX_OFF) == (unsigned)size)
		{
			char buf[64];
			if (MPAPP_Closedown() != 0)
				return -1;

			agent->handle[MUX] = -1;
			agent->state = COM;
			MUX_CommResetCallback(agent);
			/* cleanup serial port */
			while (comfunc[COM].read(agent->handle[COM], 
				buf, sizeof(buf)) >= 0);
			return 0;
		}
		else if (strcmp(pbuf, MUX_CHK) == 0
			&& strlen(MUX_CHK) == (unsigned)size)
			return (agent->state == MUX);
	}
	
	state = agent->state;
	s_handle = agent->handle[state];
	retval = comfunc[state].ioctl(s_handle, code, pbuf, size);

	/* backup uart callback */
	if (code == IO_SETCALLBACK && retval == 0)
		memcpy(&agent->uart_callback, pbuf, sizeof(UART_CONTROL));
	
	return retval;
}

/* reset comm callback and clean up original data in comm buffer
 * for further data incoming indication.
 */
static int MUX_CommResetCallback(MUXAGENT* agent)
{
	int buf;
	/* recover former uart callback function */
	if (agent->uart_callback.MAC_Dev_Event != NULL)
	{
		FS_FileIoctl(agent->handle[COM], IO_SETCALLBACK, 
			&agent->uart_callback, sizeof(UART_CONTROL));
	}
	/* clean rest data in input buffer */
	while (FS_ReadFile(agent->handle[COM], &buf, sizeof(buf)) >= 0)
		;
	return 0;
}

/**************************************************************************\
*
*	文件系统接口函数
*
\**************************************************************************/

static int FS_OpenFile(char* name, int mode, int attrib)
{
	HANDLE handle = CreateFile((PCSTR)name, (DWORD)mode, (DWORD)attrib);
	if (handle == INVALID_HANDLE_VALUE)
		return -1;
	return (int)handle;
}

static int FS_CloseFile(int handle)
{
	if (CloseFile((HANDLE)handle) != 0)
		return 0;
	return -1;
}

static int FS_ReadFile(int handle, void* buf, int bufsize)
{
	return (int)ReadFile((HANDLE)handle, (PVOID)buf, (DWORD)bufsize);
}

static int FS_WriteFile	(int handle, void* data, int datalen)
{
	return (int)WriteFile((HANDLE)handle, (PVOID)data, (DWORD)datalen);
}

static int FS_FileIoctl(int handle, int code, void *pbuf, int size)
{
	if (DeviceIoControl((HANDLE)handle, (UINT)code, (PVOID)pbuf, (DWORD)size) != 0)
		return 0;
	return -1;
}


//fufu 0701 mux
int CMUX_ClosedownFlow(void)
{
	unsigned char CloseDown[9];
	int i = 0;
	int fd;
	int offset = 0;
	int sent = 0;

	
	CloseDown[i++] = 0xF9;
	CloseDown[i++] = 0x03;
	CloseDown[i++] = 0xEF;
	CloseDown[i++] = 0x05;
	CloseDown[i++] = 0xC3;
	CloseDown[i++] = 0x01;
	CloseDown[i++] = 0xF2;//CMUX_FrameCheckSum(CloseDown+1, 3);
	CloseDown[i++] = 0xF9;

	fd = open("/dev/com2", O_RDWR);
	if (fd < 0)
	{
		return -1;
	}

//	fd = write(fd, CloseDown, i);

	while (offset < i)
	{
		sent = write(fd, CloseDown+offset, i - offset);

		if (sent < 0)
		{
			break;
		}
		else
		{
			offset += sent;				
		}
	}


	Sleep(2300);
	
	fd = close (fd);	
	return 1;
}
//fufu 0701 mux

/**************************** End Of Program ******************************/
/* end of program muxagent.c */
