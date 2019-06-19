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
\**************************************************************************/

#include "window.h"    
#include "device.h"  
#include "fapi.h"
#include "malloc.h"

#include "string.h"
#include "stdio.h"

#include "tsmain.h"

extern TS_FTPBLOCK TS_ftpblock;
extern TS_APIREGISTER	TS_ApiRegister[2];

extern void		JudgeCPUType(void);
extern short	SwapShort(short wValue);
extern long		SwapLong(long dwValue);
extern unsigned short docrc (unsigned char *p, int len );

static int  TS_OnDatain( void );
extern int  TS_Stream_fsm( BYTE *pBuf, int offset, int size );
static BYTE TS_Stream_CheckHead ( PTS_PACKETBUF pTempPacket,BYTE * Recvbuf );
static int  TS_PacketData_CollectData ( BYTE * Recvbuf );
static int  TS_PacketData_Checksum ( BYTE *Recvbuf);
extern int  TS_Stream_AnalyseData (void);
extern int  TS_Stream_PreDecode(BYTE * pData, int Len);


extern UINT TS_DevCallBack(LONG Data, DWORD UartEvent, DWORD p1, LONG p2)
{
	if (UartEvent & EV_DATAIN)
		TS_OnDatain();

//	if (UartEvent & EV_DATAOUT)
//		TS_OnDataout();

	return 0;
}


static int TS_OnDatain( void )
{
	int len;
	PTS_RECVBUF	pRecvbuf = &(TS_ftpblock.TS_Recvbuf);

	// Recv data in the end of buf
	len = ts_ReadFile( TS_ftpblock.Devhandle, pRecvbuf->pRecvData+pRecvbuf->length, TS_RECVBUF_LENGTH-pRecvbuf->length);
	if( len <=0 )
		return 0;

	TS_Stream_fsm( (pRecvbuf->pRecvData), (pRecvbuf->length), len);

	//when recv a correct packet, length reset to 0 ???
	pRecvbuf->length = 0;

	return 0;
}

#define TS_FSM_STATE_INIT		0	// fsm no recv any packet
#define TS_FSM_STATE_ZERO		1	// recved 0xFE and waiting 0x00
#define TS_FSM_STATE_RECV		2	// recving
#define TS_FSM_STATE_INITFE		3	// recving
#define TS_FSM_STATE_END		4	// recved
#define TS_FSM_STATE_FE			5	// double 0xFE, Transparentfilter, len--

extern int TS_Stream_fsm( BYTE *pBuf, int offset, int size )
{
	static int		Fsm_State = TS_FSM_STATE_INIT;
	static int		Len = 0;
	static BYTE		pData[TS_PACKBUF_LENGTH+64];// = (TS_ftpblock.TS_Procedbuf.pRecvData);
	BYTE	cur;

	if( pBuf == NULL || size ==0 )
		return 0;

	// Init filter buf

	while( size-- )
	{
		cur = *( pBuf + offset++ );
		switch( Fsm_State )
		{
		case TS_FSM_STATE_INIT:
			if( cur == 0xFE )
			{
				Fsm_State = TS_FSM_STATE_INITFE;
			}
			break;

		case TS_FSM_STATE_INITFE:
			if( cur != 0xFE )
			{
				Len = 0;
				pData[Len++] = cur;
				Fsm_State = TS_FSM_STATE_RECV;
			}
			else
			{
				Fsm_State = TS_FSM_STATE_INIT;
			}
			break;
		case TS_FSM_STATE_RECV:
			if( cur == 0xFE )
			{
				Fsm_State = TS_FSM_STATE_FE;
			}
			else
			{
				pData[Len++] = cur;
			}
			break;

		case TS_FSM_STATE_FE:
			if( cur == 0xFE )
			{
				pData[Len++] = cur;
			}

			// End of the Packet & Ready to Recv a New
			else
			{
				if( 1 == TS_Stream_PreDecode( pData, Len ) )
				{
					// Analyse Packet
					TS_ftpblock.TS_Procedbuf.length = Len;
					memcpy( TS_ftpblock.TS_Procedbuf.pRecvData, pData, Len );

					// Call the uplayer to analyse, jump out the read thread
					PostMessage( TS_ApiRegister[1].hWnd, TS_ApiRegister[1].Msg, 0, 0L);
				}
				// Recv New
				Len = 0;
				pData[Len++] = cur;
			}

			Fsm_State = TS_FSM_STATE_RECV;
			break;

		}
	}

	//----- End of RecvData, Check Data as Packet---
	if( 1 == TS_Stream_PreDecode( pData, Len ) )
	{
		TS_ftpblock.TS_Procedbuf.length = Len;
		memcpy( TS_ftpblock.TS_Procedbuf.pRecvData, pData, Len );

		// Call the uplayer to analyse, jump out the read thread
		PostMessage(TS_ApiRegister[1].hWnd, TS_ApiRegister[1].Msg, 0, 0L);

		// Recv New
		Len = 0;
		pData[Len++] = cur;
	}

	return 0;
}

int TS_InProcessing = 0;
//返回正在传述数据
//如果正在传输数据则返回TRUE
//否则返回FALSE
extern BOOL TS_IsProcessing(void)
{
	if (1 == TS_InProcessing) 
		return TRUE;
	else if (0 == TS_InProcessing) 
		return FALSE;	
	
		return FALSE;
}

extern int TS_Stream_PreDecode(BYTE * pData, int Len)
{
	TS_PACKETBUF	TempPacket;
	PTS_PACKETBUF	pPacketBuf = &(TS_ftpblock.TS_PacketBuf);

	if( TS_InProcessing == 1 )
		return 0;

	// Analyse Packet
	// Recv Packet Head, 8 Byte. check head, count the length of packet

	// Check length of packet head
	if( Len < 10 )
		return 0;

	TS_Stream_CheckHead(&TempPacket, pData);

	// Check Packet Length, should be10 + TempPacket->Length
	if( Len < 10 + TempPacket.Length )
	{
		// Not enough length, to Recv Next Data for packet
		return 0;
	}

	// Reset
	if( TempPacket.Opcode == TS_OP_OPTION )
	{
		TS_InProcessing = 1;
		return 1;
	}

	// Repeat Packet, process when idle
	if( TempPacket.Serial == pPacketBuf->Serial )
	{
		TS_InProcessing = 1;
		return 1;
	}

	// Judge Next Operate
	if( TempPacket.Serial == pPacketBuf->Serial+1 )
	{
		TS_InProcessing = 1;
		return 1;
	}

	return 0;
}

extern int TS_Stream_AnalyseData(void)
{
	TS_PACKETBUF	TempPacket;
	PTS_PACKETBUF	pPacketBuf = &(TS_ftpblock.TS_PacketBuf);
	BYTE *	pData = (TS_ftpblock.TS_Procedbuf.pRecvData);

	TS_InProcessing = 1;

	// Analyse Packet
	// Recv Packet Head, 8 Byte. check head, count the length of packet
	TS_Stream_CheckHead(&TempPacket, pData);

	// Repeat Reply Packet
	if( TempPacket.Serial == pPacketBuf->Serial && TempPacket.Opcode != TS_OP_OPTION )
	{
		ts_WriteFile(TS_ftpblock.Devhandle, TS_ftpblock.TS_Sendbuf.pSendData, 
			TS_ftpblock.TS_Sendbuf.length );
		TS_InProcessing = 0;
		return 0;
	}
	// start a new packet recv
	memcpy( pPacketBuf, &TempPacket, 12 );

	// Recv Packet Data, according to the value of Length
	TS_PacketData_CollectData ( pData );

	// Recv Packet Checksum, and verify the correctness
	if( TS_PacketData_Checksum ( pData ) == TRUE )
	{
		memcpy( pPacketBuf->pPacketData, pData+12, pPacketBuf->Length );

		//--------Enter Data Status--------------
		TS_GetPacket();
	}
	else
	{
		TS_ReplyPacket( pPacketBuf->Opcode, E_BADPACKET, NULL, 0 );
	}

	TS_InProcessing = 0;
	return 0;
}

static BYTE TS_Stream_CheckHead ( PTS_PACKETBUF pTempPacket,BYTE * Recvbuf )
{
/*	PTS_PACKETBUF	pPacketBuf;
	pPacketBuf = &(TS_ftpblock.TS_PacketBuf);
*/
	if( Recvbuf == NULL )
		return -1;

	memcpy( pTempPacket, Recvbuf, 10);
	pTempPacket->Length = SwapShort(pTempPacket->Length);
	pTempPacket->Serial = SwapLong(pTempPacket->Serial);
	pTempPacket->Errcode = SwapShort(pTempPacket->Errcode);

	return pTempPacket->Opcode;
}

static int TS_PacketData_CollectData ( BYTE * Recvbuf )
{
	PTS_PACKETBUF	pPacketBuf;

	pPacketBuf = &(TS_ftpblock.TS_PacketBuf);

	if( Recvbuf == NULL )
		return -1;

	memcpy( pPacketBuf->pPacketData, Recvbuf+12, pPacketBuf->Length );
	
	return 0;
}

static int TS_PacketData_Checksum ( BYTE *Recvbuf)
{
	unsigned short		cCheckCrc;
	unsigned short      cPacketCrc;
	PTS_PACKETBUF	pPacketBuf;

	pPacketBuf = &(TS_ftpblock.TS_PacketBuf);

	memcpy( (BYTE*)&(pPacketBuf->Checksum), Recvbuf +12+pPacketBuf->Length, 2 );
	cPacketCrc = SwapShort(pPacketBuf->Checksum);

	// calculate CRC16
	cCheckCrc  = docrc( pPacketBuf->pPacketData, pPacketBuf->Length);

	return (cPacketCrc == cCheckCrc) ? TRUE : FALSE;
}


//----------------------------assistant function---------------------------------

int		SVR_CPU_TYPE;

/*================================================================================
*  函数名： JudgeCPUType()										
*  功  能： 判断当前环境是x86 还是 m68k
*  解  释： 令一个WORD类型的变量等于0x1234,然后另一个BYTE型的数等于这个变量。
*           如果如果读出来的是34， SVR_CPU_TYPE = CPU_INTEL
*           否则SVR_CPU_TYPEE = CPU_POWERPC
=================================================================================*/	
void	JudgeCPUType(void)
{
	BYTE        bByte;
	WORD		wWord = 0x1234;

	bByte = *((BYTE *)(&wWord));

	SVR_CPU_TYPE = (bByte == 0x34) ? CPU_INTEL : CPU_POWERPC;
}

/*===========================================================================
*   Function: SwapShort   
*   Purpose : 转换一个Short类型的字的字节顺序
*   Explain : 只有此程序在PC上运行时才转换
*   Argument: dwValue要转换的WORD类型的值
*   Return  : 转换后的值
*===========================================================================*/

short SwapShort(short wValue)
{
	return (SVR_CPU_TYPE == CPU_INTEL)? 
		((wValue << 8)&0xFF00 | (wValue >> 8)&0x00FF) : wValue;
}

/*===========================================================================
*   Function: SwapLong  
*   Purpose : 转换一个Long类型的字的字节顺序
*   Explain : 只有此程序在PC上运行时才转换
*   Argument: dwValue要转换的Long类型的值
*   Return  : 转换后的值
*===========================================================================*/

long SwapLong(long dwValue)
{
	return (SVR_CPU_TYPE == CPU_INTEL) ?
		((dwValue << 24)&0xFF000000 | ((dwValue & 0x0000FF00)<< 8) | 
		((dwValue & 0x00FF0000)>> 8) | (dwValue >> 24)&0x000000FF): dwValue;
}

/*===========================================================================
*   Function : docrc
*   Purpose  : 计算CRC校验和 
*   Argument : P 数据；len : 数据的长度
*   Return   : 返回的是CRC的校验和
*===========================================================================*/
static	unsigned short crctab[256];
static void	crcinit( );

unsigned short docrc (unsigned char *p, int len )
{
	unsigned short crc = 0;

	crcinit( );

	while (len-- > 0)
		crc = (crc << 8) ^ crctab [(crc >> 8) ^ *p++];

	return crc;
}

//Generator polynomial (X^16 + X^12 + X^5 + 1)
#define		CRC16				0x1021

// 构造循环冗余校验表
static void	crcinit( )
{
	int		   val;

	for (val = 0; val <= 255; val++)
    {
		int i;
		unsigned long crc;

		crc = val << 8;

		for (i = 0; i < 8; ++i)
		{
			crc <<= 1;

			if (crc & 0x10000)
				crc ^= CRC16;
		}
		crctab [val] = (short)crc;
    }
}
// End of program



