/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : TFTP Server Module
 *
 * Purpose  : TFTP server api layer
 *            
\**************************************************************************/
#include "window.h"    
#include "device.h"  
#include "DI_UART.H"
#include "fapi.h"
#include "malloc.h"
#include "string.h"
#include "stdio.h"
#include "tsmain.h"

extern			Tftp_connect_type;
TS_FTPBLOCK		TS_ftpblock;

int PCFTP_Init(char *DevName, int Length, long DevRate)
{
	int				len;
	int				modemctl = 1;
	int				parg = 3;
	UART_MODE		para;
	UART_CONTROL	Uart_Control;

	if( DevName == NULL || DevRate ==0 )
		return TS_RETVALUE_PARAMERROR;

	// Init DevInfo
	len = strlen(DevName);//(Length>TSP_PATHLEN)? TSP_PATHLEN:Length;
	strncpy( TS_ftpblock.DevName, DevName, len );
	TS_ftpblock.DevRate = DevRate;

	// Init Recvbuf
	TS_ftpblock.TS_Recvbuf.pRecvData = (BYTE*)malloc(TS_RECVBUF_LENGTH+64);
	if( TS_ftpblock.TS_Recvbuf.pRecvData == NULL )
		return TS_RETVALUE_NOMEMORY;

	// Init Processed buf, length = TS_PACKBUF_LENGTH
	TS_ftpblock.TS_Procedbuf.pRecvData = (BYTE*)malloc(TS_PACKBUF_LENGTH+64);
	if( TS_ftpblock.TS_Procedbuf.pRecvData == NULL )
	{
		free(TS_ftpblock.TS_Recvbuf.pRecvData);
		return TS_RETVALUE_NOMEMORY;
	}

	// Init Sendbuf
	TS_ftpblock.TS_Sendbuf.pSendData = (BYTE*)malloc(TS_RECVBUF_LENGTH+64);
	if( TS_ftpblock.TS_Sendbuf.pSendData == NULL )
	{
		free(TS_ftpblock.TS_Recvbuf.pRecvData);
		free(TS_ftpblock.TS_Procedbuf.pRecvData);
		return TS_RETVALUE_NOMEMORY;
	}

	// Init Packetbuf
	TS_ftpblock.TS_PacketBuf.pPacketData = (BYTE*)malloc(TS_PACKBUF_LENGTH+64);
	if( TS_ftpblock.TS_PacketBuf.pPacketData == NULL )
	{
		free(TS_ftpblock.TS_Recvbuf.pRecvData);
		free(TS_ftpblock.TS_Procedbuf.pRecvData);
		free(TS_ftpblock.TS_Sendbuf.pSendData);
		return TS_RETVALUE_NOMEMORY;
	}

	TS_ftpblock.TS_ReplyBuf.pPacketData = (BYTE*)malloc(TS_PACKBUF_LENGTH+64);
	if( TS_ftpblock.TS_ReplyBuf.pPacketData == NULL )
	{
		free(TS_ftpblock.TS_Recvbuf.pRecvData);
		free(TS_ftpblock.TS_Procedbuf.pRecvData);
		free(TS_ftpblock.TS_Sendbuf.pSendData);
		free(TS_ftpblock.TS_PacketBuf.pPacketData);
		return TS_RETVALUE_NOMEMORY;
	}

	TS_ftpblock.TS_Recvbuf.length = 0;
	TS_ftpblock.TS_Sendbuf.length = 0;

	// Init TransNode
	TS_ftpblock.TS_TransNode.trID = -1;
	TS_ftpblock.TS_PacketBuf.Serial = -1;

	//------------------Dev Setting
	TS_ftpblock.Devhandle = ts_CreateFile(TS_ftpblock.DevName, ACCESS_READ|ACCESS_WRITE, 0);
	if( TS_ftpblock.Devhandle == INVALID_HANDLE_VALUE )
	{
		free(TS_ftpblock.TS_Recvbuf.pRecvData);
		free(TS_ftpblock.TS_Procedbuf.pRecvData);
		free(TS_ftpblock.TS_Sendbuf.pSendData);
		free(TS_ftpblock.TS_PacketBuf.pPacketData);
		free(TS_ftpblock.TS_ReplyBuf.pPacketData);
		return TS_RETVALUE_DEVERROR;
	}

	para.slen		= (DWORD)sizeof(UART_MODE);
	para.baudrate	= TS_ftpblock.DevRate;
	para.datalen	= 12;
	para.parity		= UART_NOPARITY;
	para.stopbits	= UART_ONESTOPBIT;
	para.flowctl	= 0;

	if (!DeviceIoControl(TS_ftpblock.Devhandle, IO_UART_SETMODE, &para, sizeof(UART_MODE))) 
	{
		free(TS_ftpblock.TS_Recvbuf.pRecvData);
		free(TS_ftpblock.TS_Procedbuf.pRecvData);
		free(TS_ftpblock.TS_Sendbuf.pSendData);
		free(TS_ftpblock.TS_PacketBuf.pPacketData);
		free(TS_ftpblock.TS_ReplyBuf.pPacketData);
		ts_CloseFile(TS_ftpblock.Devhandle);
		return TS_RETVALUE_DEVERROR;
	}

	// Set io callback
	Uart_Control.Dev_Event = TS_DevCallBack;
	Uart_Control.para = NULL;
	Uart_Control.EventCode = EV_DATAIN | EV_DATAOUT | EV_MODEM;

	if (!DeviceIoControl(TS_ftpblock.Devhandle, IO_SETCALLBACK, &Uart_Control, sizeof(UART_CONTROL))) 
	{
		free(TS_ftpblock.TS_Recvbuf.pRecvData);
		free(TS_ftpblock.TS_Procedbuf.pRecvData);
		free(TS_ftpblock.TS_Sendbuf.pSendData);
		free(TS_ftpblock.TS_PacketBuf.pPacketData);
		free(TS_ftpblock.TS_ReplyBuf.pPacketData);
		ts_CloseFile(TS_ftpblock.Devhandle);
		return TS_RETVALUE_DEVERROR;
	}

	JudgeCPUType();
	TS_ftpblock.TS_TransNode.pFile = NULL;

	return 0;
}

int PCFTP_Dest(void)
{
	//fclose(TS_ftpblock.Devhandle);
	ts_CloseFile(TS_ftpblock.Devhandle);
	
	free(TS_ftpblock.TS_Recvbuf.pRecvData);
	free(TS_ftpblock.TS_Procedbuf.pRecvData);
	free(TS_ftpblock.TS_Sendbuf.pSendData);
	free(TS_ftpblock.TS_PacketBuf.pPacketData);
	free(TS_ftpblock.TS_ReplyBuf.pPacketData);
	return 0;
}

int PCFTP_GetBlocksize( int* Blocksize )
{
	*Blocksize = TS_PACKBUF_LENGTH;
	return 0;
}


TS_APIREGISTER	TS_ApiRegister[2];

int PCFTP_Register( HWND hWnd, unsigned long Msg, BYTE OpcType )//int minopcode, int maxopcode, 
{
	if( OpcType == TS_OP_OPTION )
	{
		TS_ApiRegister[0].hWnd = hWnd;
		TS_ApiRegister[0].Msg = Msg;
	}
	else
	{
		TS_ApiRegister[1].hWnd = hWnd;
		TS_ApiRegister[1].Msg = Msg;
	}
	return 0;
}

int PCFTP_Readdata(void)
{
	TS_Stream_AnalyseData();
	return 0;
}

int PCFTP_Reply( unsigned short error, unsigned char* pbuf, int buflen )
{
	return 0;
}



// End of program
