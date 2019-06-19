/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Mobile Engine
 *
 * Purpose  : 
 *
\**************************************************************************/
#if defined MULTI_THREAD 

#include "..\tcpip\include\tilib.h"
#endif
#include "stdio.h"
#include "fapi.h"
#include "string.h"
#include "device.h"
#include "window.h"
#include "me_at.h"
#include "me_func.h"
#include "me_wnd.h"
#include "muxagent.h"
#include "DI_Uart.h"

#include "sys\task.h"

#define RD_BUFSIZE	1024
#define BUFSIZE		256

static int com_fd;
extern HWND ME_hWnd;
static MAT_Indication mat_indication;

unsigned int volatile nTickCount;

typedef struct com_inbuf
{
	char	buf[RD_BUFSIZE];
	char*	data;
	char*	write;
	int		datalen;
}RD_BUFFER;

RD_BUFFER RdBuf;

#define MAX_MUXNAMELEN		50
#define MAX_CHNL			3

typedef struct tagMUXChnl
{
    int             hmux;
    int             enable;
    char            name[MAX_MUXNAMELEN];
    RD_BUFFER       rcvbuf;
    MAT_Indication  notify;
} MUXCHNL, *PMUXCHNL;

const char * devname[MAX_CHNL] = {
    MUX_CHANNEL1,
    MUX_CHANNEL2,
    MUX_CHANNEL3
};

MUXCHNL gMuxChnl[MAX_CHNL];


#if defined MULTI_THREAD
static OSHANDLE hEvent;
#if (_HOPEN_VER >= 200)
	MUTEX		mutex;
#else
	OSHANDLE	mutex;
#endif

#endif

static int BUF_Cleanup(RD_BUFFER* pbuf);
static int BUF_GetData(RD_BUFFER* pbuf, char* buf, int buflen);
static int BUF_PutData(RD_BUFFER* pbuf, char* data, int datalen);
static void USBIndication (int event, void * pdata, int datalen);
typedef struct UART_Control
{
	_CALLBACK	Event_CallBack;
	LONG		para;
	DWORD		EventCode;
}UART_CONTROL, *PUART_CONTROL;

static int mux_status;

#define EVENT_DATAIN	1
#define EVENT_DATAOUT	2
#define EVENT_MODEM		3

UINT ME_EventProc(LONG lUserData, DWORD dwEventMask, DWORD param1, LONG param2);
UINT ME_EventProc_Modem(LONG lUserData, DWORD dwEventMask, DWORD param1, LONG param2);
void ME_UssdIndication (int event, void * pdata, int datalen);
extern int LIB_WaitEvent ( unsigned int handle );
extern int LIB_SignalEvent ( unsigned int handle, unsigned long event );
extern int LIB_DestroyEvent ( unsigned int handle );
int MAT_Callback (int chnl)
{
	char buf[BUFSIZE];
	int count;
    static int ready = 1;

	while (1)
	{
		count = MUX_ReadFile (gMuxChnl[chnl].hmux, buf, BUFSIZE);

		if ( count <= 0 )
			break;

#ifdef _ME_DEBUG
	{
//		int i;
		char buf2[BUFSIZE] = "";

		MsgOut ("\r\nchannel: [%d]", chnl);
		MsgOut ("\r\nME recieve count:%d Data:",count);
//		for ( i=0; i < count; i++ )
//			MsgOut ("%c", buf[i]);				
		memcpy(buf2, buf, count);
		buf2[count] = 0;
//		for ( i=0; i < count; i++ )
		MsgOut ("%s", buf2);
	}
#endif
        BUF_PutData (&gMuxChnl[chnl].rcvbuf, buf, count);
	}

   if ( !ready )
       return 0;

    while ( (count = BUF_GetData (&gMuxChnl[chnl].rcvbuf, buf, BUFSIZE)) > 0 ) {
        ready = 0;
        gMuxChnl[chnl].notify (MAT_DATAIN, buf, count);
        ready = 1;
    }

    return 0;
}

#if 0
int MAT_Callback(void)
{
#if defined MULTI_THREAD
	char buf[RD_BUFSIZE];
	int  l;

	LIB_WaitMutex(&mutex);

	while ( (l = BUF_GetData( &RdBuf, buf, RD_BUFSIZE )) > 0 )
	{
#if defined _ME_DEBUG
	{
		int i;
		MsgOut( "\r\nME recieve: " );
		for ( i=0; i<l; i++)
			MsgOut( "%c", buf[i] );
	}
#endif
		mat_indication ( MAT_DATAIN, buf, l );
	}

	LIB_SignalMutex(&mutex);

#else
	char buf[BUFSIZE];
	int count;
    static int ready = 1;

	while (1)
	{
		count = MUX_ReadFile (com_fd, buf, BUFSIZE);

		if ( count < 0 )
			break;

#if defined _ME_DEBUG
	{
		int i;
		MsgOut( "\r\nME recieve: " );
		for ( i=0; i<count; i++)
			MsgOut( "%c", buf[i] );
	}
#endif
        BUF_PutData (&RdBuf, buf, count);
	}

   if ( !ready )
       return 0;

    while ( (count = BUF_GetData (&RdBuf, buf, BUFSIZE)) > 0 ) {
        ready = 0;
        mat_indication (MAT_DATAIN, buf, count);
        ready = 1;
    }

#endif
    return 0;
}


#endif


#if defined MULTI_THREAD
void WINAPI MAT_Thread ( LPVOID lpParameter )
{
    int len, l;
	char buf[BUFSIZE];

	for (;;)
	{
		while ( 1 )
		{
			len = MUX_ReadFile( com_fd, buf, BUFSIZE );

			if ( len == 0 )
				continue;

			if ( len < 0 )
				break;

			LIB_WaitMutex(&mutex);
			l = BUF_PutData(&RdBuf, buf, len );
			LIB_SignalMutex(&mutex);

			// buffer full
			if ( l <= 0 )
			{
				MAT_Reset( 0 );
				LIB_WaitMutex(&mutex);
				BUF_Cleanup(&RdBuf);
				LIB_SignalMutex(&mutex);
				LIB_WaitEvent( hEvent );
			}
		}

		PostMessage( ME_hWnd, WM_MAT_MESSAGE, 0, 0);
		LIB_WaitEvent( hEvent );
	}

    return;
}

#endif

int MAT_Init (int mode, int para1, int para2, MAT_Indication indication)
{
	UART_CONTROL Uart_Control;
	int 	UartStatus = -1;

	if(para1 == 0){
		memset (gMuxChnl, 0, sizeof(MUXCHNL) * MAX_CHNL);
		
		//	mat_indication = indication;
		//	BUF_Cleanup(&RdBuf);
		
#if defined MULTI_THREAD
		LIB_CreateMutex(&mutex);
		
		hEvent = LIB_CreateEvent ("MEdrv");
		
		if ( hEvent == NULL )
			return -1;
		
		LIB_CreateTask("MEdrv", TS_REALTIME|32, MAT_Thread, 2048, NULL);
#endif
		
#if 0
		#ifndef _EMULATE_
		MsgOut("============== AutoSetMeBaudRate  ===============\r\n");
		UartStatus = AutoSetMeBaudRate();

		if(UartStatus > 0)
		{
			MsgOut("============== ClearCNMIsetting  ===============\r\n");
			ClearCNMIsetting();
		}
		else
		{
		#endif
			MsgOut("============== mutliplexer is on  ===============\r\n");

			CMUX_ClosedownFlow();

			MsgOut("============== close multiplexer ===============\r\n");
			
			#ifndef _EMULATE_
				MsgOut("============== AutoSetMeBaudRate  ===============\r\n");
				UartStatus = AutoSetMeBaudRate();
				
				if(UartStatus > 0)
				{
					MsgOut("============== ClearCNMIsetting  ===============\r\n");
					ClearCNMIsetting();
				}
		}
		#endif
#endif

		com_fd = MUX_OpenFile("DEV:UART1", ACCESS_READ|ACCESS_WRITE, 0);
		
		if ( com_fd < 0 )
		{
			return -1;
		}

		Uart_Control.Event_CallBack = ME_EventProc;
		Uart_Control.para = 1;
		Uart_Control.EventCode = EV_DATAIN | EV_DATAOUT | EV_MODEM;
		
		MUX_FileIoctl( com_fd, IO_SETCALLBACK, &Uart_Control, sizeof(UART_CONTROL) );
		MUX_FileIoctl( com_fd, IO_DUMPBUF, NULL, 0 );
		
		if ( mode == MOD_MUX )
        {
#if defined(_EMULATE_)
            int iMuxLen = strlen(MUX_ON);
            int iMux;
            iMux = MUX_FileIoctl(com_fd, IO_UART_SETMODEMCTL, MUX_ON, iMuxLen);
#endif
        }
		gMuxChnl[1].hmux = com_fd;
		gMuxChnl[1].enable = 1;
		gMuxChnl[1].notify = indication;
    	BUF_Cleanup (&gMuxChnl[1].rcvbuf);
        strcpy (gMuxChnl[1].name, devname[1]);

#if defined(_EMULATE_)
        if (Modem_OpenCnl(USBIndication) != -1)
        {
            Modem_Send("AT&F\r",-1);
            Modem_CloseCnl();
        }
#endif
	}
	else if(para1 == 1){	
    
		Uart_Control.Event_CallBack = ME_EventProc;
		Uart_Control.EventCode = EV_DATAIN | EV_DATAOUT | EV_MODEM;
        gMuxChnl[2].notify = indication;
    	BUF_Cleanup (&gMuxChnl[2].rcvbuf);
        strcpy (gMuxChnl[2].name, devname[2]);

		if(gMuxChnl[2].hmux <= 0)
	        gMuxChnl[2].hmux = MUX_OpenFile (gMuxChnl[2].name, ACCESS_READ|ACCESS_WRITE, 0);
        gMuxChnl[2].enable = 1;
        Uart_Control.para = 2;
        MUX_FileIoctl (gMuxChnl[2].hmux, IO_SETCALLBACK, &Uart_Control, sizeof (Uart_Control));
    }
    return 0;
}

int MAT_AutoSetMeBaudRate(void)
{
    static const unsigned long iBR_String[] = 
    {
//        9600,
//        14400,
//        19200,
//        28800,
//        38400,
//        57600,
        115200,
//        230400,
        0
    };
    static int i = 0;
//#ifndef _EMULATE_
    UART_MODE para;

    para.slen		= (DWORD)sizeof(UART_MODE);
    para.baudrate	= iBR_String[0];		/* 波特率 */
    para.datalen	= 8;					/* 数据位0x000f */
    para.parity		= UART_NOPARITY;		/* 校验位 */
    para.stopbits	= UART_ONESTOPBIT;		/* 停止位 */
    para.flowctl	= 0;					/* 需要流控制 */
    
    if (0 == MUX_FileIoctl(com_fd, IO_UART_SETMODE, &para, sizeof(UART_MODE)))
        return 1;

//#endif
    return 0;
}

static void USBIndication (int event, void * pdata, int datalen)
{
}

unsigned int modemEvent;

int Modem_WaitOnDataOut()
{
    return LIB_WaitEvent(modemEvent);
}

static ModemCnl	CnlSetting;

int Modem_OpenCnl(MAT_Indication indication)
{
	UART_CONTROL 	Uart_Control;
	int 				hmux;

	if(gMuxChnl[0].hmux > 0)
	{
		MsgOut("\r\n[ME]: channel 0 already opened, second try refused. hmux0 = %d\r\n", gMuxChnl[0].hmux);
		return -1;
	}
	
#ifndef _EMULATE_
	modemEvent = LIB_CreateEvent("UsbMdm");
	if (NULL == modemEvent)
	{
	    MsgOut("###################### Create modemEvent failed ######################\r\n");
	}
#endif
	Uart_Control.Event_CallBack = ME_EventProc_Modem;
	Uart_Control.EventCode = EV_DATAIN | EV_DATAOUT | EV_MODEM;
    gMuxChnl[0].notify = indication;
	BUF_Cleanup (&gMuxChnl[0].rcvbuf);
    strcpy (gMuxChnl[0].name, devname[0]);

	hmux = MUX_OpenFile (gMuxChnl[0].name, ACCESS_READ|ACCESS_WRITE, 0);

	MsgOut("\r\nchannel 0 opened, gMuxChnl[0].hmux == %d, gMuxChnl[0].name = %s\r\n", hmux, gMuxChnl[0].name);
	if(hmux == -1)
	{
		return -1;
	}

	gMuxChnl[0].hmux = hmux;
	
    gMuxChnl[0].enable = 1;
    Uart_Control.para = 0;
    return MUX_FileIoctl (gMuxChnl[0].hmux, IO_SETCALLBACK, &Uart_Control, sizeof (Uart_Control));
}


int Modem_CloseCnl(void)
{
	int result;
	LIB_DestroyEvent(modemEvent);
	result = MUX_CloseFile(gMuxChnl[0].hmux);
//	result = MPAPP_CloseFile(gMuxChnl[0].hmux);

	if(result == -1)
		return -1;

	MsgOut("\r\n[ME]: channel 0 closed.\r\n");
	gMuxChnl[0].hmux = -1;
	return 1;
}


int Modem_SetChannel (ModemCnlType CnlType, ChannelFunc Func)
{
	
	switch(CnlType)
	{
		case BlueTooth_C:
			CnlSetting.BTfunc = Func;
			break;

		case USB_C:
			CnlSetting.USBfunc = Func;
			break;

		default:
			return -1;
	}
	
	return 1;
}


ChannelFunc Modem_GetChannel (ModemCnlType CnlType)
{
		
	switch(CnlType)
	{
		case BlueTooth_C:
			return(CnlSetting.BTfunc);

		case USB_C:
			return(CnlSetting.USBfunc);

		default:
			return IllegalFunc;
	}
		
}


int Modem_Send(void * pdata, int datalen)
{
    int offset=0, len, sent;

	if ( datalen == -1 )
		len = strlen((char*)pdata);
	else
		len = datalen;

	//假如多通道未打开成功，保护措施
	if (gMuxChnl[0].hmux < 0)
		return 0;

    sent = MUX_WriteFile (gMuxChnl[0].hmux, (char*)pdata+offset, len - offset);

    return sent;
}



int MAT_Send (int mode, void * pdata, int datalen)
{
	int offset=0, len, sent;

	if ( datalen == -1 )
		len = strlen((char*)pdata);
	else
		len = datalen;

	if( mode < 0 || mode > 2 )
		return 0;
	//假如多通道未打开成功，保护措施
	if( (2 == mode) && (gMuxChnl[mode].hmux < 0) )
	{
		MsgOut("\r\n[ME]: channel %d is invalid. gMuxChnl[%d] = %d\r\n", mode, mode, gMuxChnl[mode]);
		return 0;
	}
	
	while((nTickCount<GetTickCount()) && (GetTickCount()-nTickCount<100))
	{
		Sleep(10);
	}

	while (offset < len)
	{
//		sent = MUX_WriteFile(com_fd, (char*)pdata+offset, len - offset);
		sent = MUX_WriteFile (gMuxChnl[mode].hmux, (char*)pdata+offset, len - offset);

		if (sent < 0)
		{
			nTickCount = GetTickCount();
			return sent;
		}
		else
			offset += sent;
	}
	nTickCount = GetTickCount();
#if defined _ME_DEBUG
	MsgOut( "\r\nME Send: %s", pdata );
#endif
    return offset;
}

int MAT_Reset (int mode)
{
    return 0;
}

int MAT_TurndownMux()
{
	mux_status = MUX_FileIoctl(com_fd, IO_UART_SETMODEMCTL, MUX_CHK, strlen(MUX_CHK));

	if ( mux_status == 1 )
		MUX_FileIoctl(com_fd, IO_UART_SETMODEMCTL, MUX_OFF, strlen(MUX_OFF));

	return 0;
}

int MAT_TurnOnMux()
{
	if ( mux_status != 1 )
		return 0;

	MUX_FileIoctl(com_fd, IO_UART_SETMODEMCTL, MUX_ON, strlen(MUX_ON));

	mux_status = 0;

	return 0;
}
int MUX_Init(void)//开启多通道
{
    UART_CONTROL Uart_Control;
    int iMUX = 0,iLen;
    iLen = strlen(MUX_ON);
    iMUX = MUX_FileIoctl(com_fd, IO_UART_SETMODEMCTL, MUX_ON, iLen);

    if (Modem_OpenCnl(USBIndication) != -1)
    {
        Modem_Send("AT&F\r",-1);
        Modem_CloseCnl();
    }
    Uart_Control.Event_CallBack = ME_EventProc;
    Uart_Control.EventCode = EV_DATAIN | EV_DATAOUT | EV_MODEM;
    gMuxChnl[2].notify = ME_UssdIndication;
    BUF_Cleanup (&gMuxChnl[2].rcvbuf);
    strcpy (gMuxChnl[2].name, devname[2]);
    
	if(gMuxChnl[2].hmux <= 0)
	    gMuxChnl[2].hmux = MUX_OpenFile (gMuxChnl[2].name, ACCESS_READ|ACCESS_WRITE, 0);
    gMuxChnl[2].enable = 1;
    Uart_Control.para = 2;
    MUX_FileIoctl (gMuxChnl[2].hmux, IO_SETCALLBACK, &Uart_Control, sizeof (Uart_Control));
    return iMUX;
}

UINT ME_EventProc(LONG lUserData, DWORD dwEventMask, DWORD param1, LONG param2)
{
	if ( dwEventMask == EV_DATAIN )
#if defined MULTI_THREAD
		LIB_SignalEvent( hEvent, 0 );
#else
		PostMessage( ME_hWnd, WM_MAT_MESSAGE, (WPARAM)lUserData, 0);
#endif

	return 0;
}

UINT ME_EventProc_Modem(LONG lUserData, DWORD dwEventMask, DWORD param1, LONG param2)
{
	if ( dwEventMask & EV_DATAIN )
	{
#if defined MULTI_THREAD
		LIB_SignalEvent( hEvent, 0 );
#else
		PostMessage( ME_hWnd, WM_MAT_MESSAGE, (WPARAM)lUserData, 0);
#endif
	}

	if (dwEventMask & EV_DATAOUT)
	{
	    MsgOut(/*__FUNCTION__*/":EV_DATAOUT\r\n");
	    LIB_SignalEvent(modemEvent, 0);
	}

    return 0;
}

int ME_MuxPowerUp()
{
	if ( com_fd < 0 )
		return -1;

	return MUX_FileIoctl(com_fd, IO_UART_SETMODEMCTL, MUX_ON, strlen(MUX_ON));
}

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

static int BUF_Cleanup(RD_BUFFER* pbuf)
{
	if (pbuf == NULL)
		return -1;
	memset(pbuf->buf, 0, sizeof(RD_BUFSIZE));
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

static int BUF_GetData(RD_BUFFER* pbuf, char* buf, int buflen)
{
	char *front, *tail;
	int count = 0, copyed;

	if (pbuf == NULL || buf == NULL || buflen <= 0)
		return 0;

	front = pbuf->data;
	while (buflen > 0 && (tail = pbuf->write) != front)
	{
		if (tail > front)
			copyed = tail - front;
		else
			copyed = pbuf->buf + RD_BUFSIZE - front;
		if (copyed > buflen)
			copyed = buflen;
		memcpy(buf + count, front, copyed);
		buflen -= copyed;
		count += copyed;
		front += copyed;

		if (front >= pbuf->buf + RD_BUFSIZE)
			front -= RD_BUFSIZE;
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

static int BUF_PutData(RD_BUFFER* pbuf, char* data, int datalen)
{
	char *front, *tail;
	int copyed, count = 0;

	if (pbuf == NULL || data == NULL || datalen <= 0)
		return 0;

	tail = pbuf->write;
	while (datalen > 0)
	{
		front = pbuf->data;
		if (tail + 1 == front || (tail + 1 == pbuf->buf + RD_BUFSIZE 
			&& front == pbuf->buf))
			break;

		if (front > tail)
			copyed = front - tail - 1;
		else if (front == 0)
			copyed = pbuf->buf + RD_BUFSIZE - tail - 1;
		else
			copyed = pbuf->buf + RD_BUFSIZE - tail;

		if (copyed > datalen)
			copyed = datalen;
		memcpy(tail, data + count, copyed);
		datalen -= copyed;
		count += copyed;
		tail += copyed;

		if (tail >= pbuf->buf + RD_BUFSIZE)
			tail -= RD_BUFSIZE;
		pbuf->write = tail;
	}
	pbuf->datalen += count;
	return count;
}
