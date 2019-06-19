/**************************************************************************\
*
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
*
* Model   : CMUX
*
* Purpose : siemens multiplexer protocol
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
*	��������: Digital cellular telecommunications system
*			  Terminal Equipment to Mobile Station multiplexer protocol 
*			  (3GPP TS 07.10 version 7.2.0 Release 1998) 
*			  Implementation on Siemens wireless modules TC35/MC35
*
*	�ļ�����: siem_mux.c
*
*	��������: 
*
*	������: impementation of Multiplexer Protocol (MP) on TC35/MC35
*
*	�������ڣ�2002.04.22
*
\**************************************************************************/

/**************************************************************************\
*
*	��Ŀ�ܽ᣺
*
*	2002.04.08 ���� 2002.04.12	���н׶�	( 5 days)
*	2002.04.22 ���� 2002.??.??	��Ʊ���	(?? days)
*
*	�����ܼ�������
*   ����ע��������
*
*	˵����
*	1.
*   2.
*
\**************************************************************************/

/**************************************************************************\
*
*	�޸���Ϣ
*
*	version CMUX_01 --------------------------------------------------------
*
*	(1) ���⣺��
*		�������
*		���ڣ�2002.04.??
*
\**************************************************************************/

#include "mux_lib.h"
#include "siem_mux.h"

/**************************************************************************\
*
*	Debugoutput����
*
\**************************************************************************/

#define WARNING

#ifdef WARNING
#define warning     printf
#else
#define warning     mux_printf
void mux_printf(char* format, ...){format = NULL;}
#endif

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

/* ����ģ��ͨ��״̬ */
#define STATE_INVALID		0	/* invalided modular */
#define STATE_CLOSEDOWN		1	/* initialized OK */
#define STATE_ATCMDCMUX		2	/* AT+CMUX */
#define STATE_DISCONNECTED	3	/* Startup OK */
#define STATE_CONNECTING	4	/* start connect multiplexer */
#define STATE_CONNECTED		5	/* DLC established */
#define STATE_DISCONNECTING	6	/* multiplexer start OK */
#define STATE_CLOSING		7	/* closedown a virtual channel */

/* ����ͨ������������һ������ͨ�� */
#define MAX_DLC_NUM			4

/* ���֡����������Siemens���óɹ̶���97�������������һЩ */
#define MAX_FRAME_SIZE		100

/* ����������ʱ�¼� */
#define BLOCK_TIMEOUT       (3 * 1000)  /* 3 second */
#define CMUX_TIMEOUT		(3 * 1500)

/* ֡���Ͷ��� */
#define FRAME_SABM			0x2F	/* DLC�������� */
#define FRAME_UA			0x63	/* DLC����ȷ�� */
#define FRAME_DM			0x0F	/* DLC�������� */
#define FRAME_DISC			0x43	/* DLC�������� */
#define FRAME_UIH			0xEF	/* DLC���� */

/* ����λ���� */
#define CONTROL_PF_BIT		0x10	/* ��ѯbit */
#define ADDRESS_CR_BIT		0x02	/* Ӧ��bit */
#define EXTENSION_BIT		0x01	/* ��չbit */

#define BASIC_FLAG			0xF9	/* HDLC FLAG in basic option */

/* ���������� */
#define BUF_SIZE			(1024 * 1)
#define FRM_BUFSIZE			90

/* �����ٽ�β��� */
#define ENTER_MUTEX			LIB_WaitMutex  (&cmux.mutex)
#define LEAVE_MUTEX			LIB_SignalMutex(&cmux.mutex)

/* ����multiplexerģʽ�������� */
#define AT_CMUX_COMMAND		"AT+CMUX=0\r"

/**************************************************************************\
*
*	���ݽṹ���������Ͷ���
*
\**************************************************************************/

typedef unsigned char	UINT8;
typedef enum {S_CMUX, S_FLAG, S_ADDR, S_CTRL, S_LEN1, S_LEN2, S_DATA, S_FCS} SCANSTATE;

/* UART���ƽṹ */
typedef struct
{
	_CALLBACK	userfunc;	/* ���ڻص�����ָ�� */
	void*		para;
	DWORD		EventCode;
}	UART_CALLBK;

/* ����֡�ṹ */
typedef struct
{
	int		DLCI;				/* ͨ����ʶ		*/
	int		type;				/* ֡����		*/
	int		CR_bit, PF_bit;		/* C/R, P/Fλ	*/
	UINT8	data[MAX_FRAME_SIZE];	/* ֡��Ϣ		*/
	int		datalen;			/* ��Ϣ����		*/
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
	int			state;			/* ͨ��״̬		*/
	APPTIMER	timer;			/* ͨ����ʱ��	*/
	OSHANDLE	event;			/* ͨ���¼�		*/
	UART_CALLBK callback;		/* �ص�����	*/
	BUFFER		rxbuf;			/* ���ջ�����	*/
	int			flowctrl;		/* ���ر��		*/
	int			timeoutflag;
}	CHANNEL;

/* ����Multiplexer����ṹ */
typedef struct
{
#if (_HOPEN_VER >= 200)
	MUTEX		mutex;			/* ������		*/
#else
	OSHANDLE	mutex;			/* ������	*/
#endif
	IOCOM		write;			/* д���ں��� */
	IOCOM		read;			/* �����ں��� */
	CHANNEL		DLC[MAX_DLC_NUM];	/* DLC0 �� DLC3 */
	UINT8		txbuf[BUF_SIZE];	/* ģ�鷢�ͻ����� */
	UINT8*		data;			/* ����������ָ�� */
	int			datalen;		/* �������������� */
	int			flowctrl;		/* ���������ر�� */
	SCANSTATE	scanstate;		/* recevive frame state */
	FRAME		frame;			/* MUX frame structure */
}	CMUX;

/**************************************************************************\
*
*	ȫ�ֱ�������
*
\**************************************************************************/

/* У�������� */
static const UINT8 fcstab8[256] = 
{   //reversed, 8-bit, poly=0x07
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


#define ATCMD_LEN		40
static char at_buf[ATCMD_LEN + 1];

/* multiplexer���ƽṹ */
static CMUX cmux;

/**************************************************************************\
*
*	��������
*
\**************************************************************************/

/* �ⲿ�������� */
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
/* �ڲ�������һ�㺯�� */
static int	 CMUX_GetDLCIByName(char* channel_name);
static UINT8 CMUX_FrameCheckSum(UINT8 *p, int len);
static int   CMUX_SendFrameData(UINT8 frametype, int DLCI, UINT8* data, int datalen);
static int   CMUX_ProcRxFrame(FRAME *frame);
static int   CMUX_WaitEvent(int DLCI, int microsecond);
static void  CMUX_TimeOut(void* DLC);
static void	 CMUX_CallBack(int DLCI, unsigned int event);
static int   CMUX_ResetChannel(int DLC);
static int   CMUX_ResetMultiplexer(void);

/* �ڲ������������շ����� */
static int CMUX_ReadDevice (UINT8* buf, int bufsize);
static int CMUX_WriteDevice(UINT8* data, int datalen);

/* �ڲ�������BUFFER������ */
#define MPBUF_IsEmpty(pbuf)		((pbuf)->data == (pbuf)->write)
//static int MPBUF_GetDataSize(BUFFER* buf);
static int MPBUF_CleanupData(BUFFER* buf);
static int MPBUF_GetBufData(BUFFER* buf, UINT8* pbuf, int buflen);
static int MPBUF_PutBufData(BUFFER* buf, UINT8* data, int datalen);

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
*	�������ƣ�CMUX_Initial (System Function)
*
*	�������ܣ���ʼ��Multiplexerģ��
*
*   ������������
*
*	�������أ�CMUX_SUCCESS	�ɹ�
*			  CMUX_FAILURE	ʧ��
*
\*========================================================================*/
 
int CMUX_Initial(void)
{
	int i;
	char name[] = "DLC0";
	
	/* ֻ�и���ͨ������δ��ʼ��״̬���ܶ�multiplexerģ����г�ʼ�� */
	for (i = 0; i < MAX_DLC_NUM; i ++)
		if (cmux.DLC[i].state != STATE_INVALID)
			return CMUX_FAILURE;

	memset(&cmux, 0, sizeof(CMUX));
	memset(at_buf, 0, sizeof(at_buf));

	/* ����ģ��MUTEX LOCK */
	if (LIB_CreateMutex(&cmux.mutex) == -1)
		return CMUX_FAILURE;

	cmux.data = cmux.txbuf;

	/* ��ʼ������ͨ�� */
	for (i = 0; i < MAX_DLC_NUM; i ++)
	{
		/* ����ͨ����ʱ�� */
		LIB_CreateTimer(&cmux.DLC[i].timer);
		/* ����ͨ���¼� */
		if ((cmux.DLC[i].event = LIB_CreateEvent(name)) == NULL)
			return CMUX_FAILURE;
		/* ��ʼ�������ͨ�����ջ����� */
		MPBUF_CleanupData(&cmux.DLC[i].rxbuf);
		/* �����һ��ͨ������ */
		name[3] ++;
	}

	/* �Ը���DLC����CLOSEDOWN״̬ */
	for (i = 0; i < MAX_DLC_NUM; i ++)
		cmux.DLC[i].state = STATE_CLOSEDOWN;

    warning("[MUX] Initial success!\r\n");
	return CMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_Startup
*
*	�������ܣ�����Multiplexer����ģ�飬ʹ����ģ�����Multiplexerģʽ
*
*   ����������fpread		�����ں�����ڵ�ַ
*			  fpwrite		д���ں�����ڵ�ַ
*
*	�������أ�CMUX_SUCCESS	�ɹ�
*			  CMUX_FAILURE	ʧ��
*
\*========================================================================*/

int CMUX_Startup(IOCOM fpread, IOCOM fpwrite)
{
	UINT8	buf[256];
	int		i, len, cmdlen = strlen(AT_CMUX_COMMAND);
	/* �������ô��ڶ�д������� */
	if (fpread == NULL || fpwrite == NULL)
		return CMUX_FAILURE;

	ENTER_MUTEX;

	/* ֻ����ͨ��DLC0ΪCLOSEDOWNʱ����ִ�иú��� */
	if (cmux.DLC[0].state != STATE_CLOSEDOWN)
    {
        warning("[MUX] startup DLC0 state1[%d]!\r\n", cmux.DLC[0].state);
		goto ERROR_NOOP;
    }

	CMUX_ResetMultiplexer();
	cmux.read  = fpread;
	cmux.write = fpwrite;
	cmux.DLC[0].state = STATE_ATCMDCMUX;

	/* ��մ��ڻ����� */
	while (fpread(buf, 256) >= 0)
		;

	/* ����AT+CMUX����multiplexerģʽ */
	strcpy((char*)buf, AT_CMUX_COMMAND);
	at_buf[0] = '\0';
	if ((len = fpwrite(buf, cmdlen)) != cmdlen)
	{
		len = (len < 0 ? 0 : len);
		cmux.datalen = cmdlen - len;
		memcpy(cmux.txbuf, &buf[len], cmux.datalen);
	}

	/* �ȴ��ش�OK */
	CMUX_WaitEvent(0, CMUX_TIMEOUT * 10);

	if (cmux.DLC[0].state != STATE_CONNECTING)
    {
        warning("[MUX] startup DLC0 state2[%d]!\r\n", cmux.DLC[0].state);
		goto ERROR_HANDLE;
    }

	/* ��ʼ����֡ */
	cmux.scanstate = S_FLAG;
	
	/* �Է���SABM֡������DLC0 */
	if (CMUX_SendFrameData(FRAME_SABM, 0, NULL, 0) == CMUX_FAILURE)
    {
        warning("[MUX] startup sendframedata fail!\r\n");
		goto ERROR_HANDLE;
    }

    CMUX_WaitEvent(0, BLOCK_TIMEOUT * 5);

	if (cmux.DLC[0].state != STATE_CONNECTED)
    {
        warning("[MUX] startup DLC0 state3[%d]!\r\n", cmux.DLC[0].state);
		goto ERROR_HANDLE;
    }

	for (i = 1; i < MAX_DLC_NUM; i ++)
	{
		cmux.DLC[i].state = STATE_DISCONNECTED;
		CMUX_ResetChannel(i);
	}
	MsgOut("MUX startup OK\r\n");
	LEAVE_MUTEX;
    warning("[MUX] startup success!\r\n");
	return CMUX_SUCCESS;

ERROR_HANDLE:

	CMUX_ResetMultiplexer();
	cmux.DLC[0].state = STATE_CLOSEDOWN;

ERROR_NOOP:
	MsgOut("MUX startup Error\r\n");
	LEAVE_MUTEX;
    warning("[MUX] startup fail!\r\n");
	return CMUX_FAILURE;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_Closedown
*
*	�������ܣ��ر�mulitplexerģ�飬ʹ����ģ��ָ�AT����״̬
*
*   ������������
*
*	�������أ�CMUX_SUCCESS	�ɹ�
*			  CMUX_FAILURE	ʧ��
*
\*========================================================================*/

int CMUX_Closedown(void)
{
	int i;
	UINT8	close[] = {0xc3, 0x01};

	ENTER_MUTEX;

	/* ֻ����DLC0ΪCONNECTED״̬ʱ����ִ�� */
	if (cmux.DLC[0].state != STATE_CONNECTED)
		goto ERROR_NOOP;

	/* ��DLC0����CLOSEDOWN���� */
	if (CMUX_SendFrameData(FRAME_UIH, 0, close, sizeof(close)) == CMUX_FAILURE)
		goto ERROR_HANDLE;

	cmux.DLC[0].state = STATE_CLOSING;
	
    CMUX_WaitEvent(0, BLOCK_TIMEOUT * 2);

	if (cmux.DLC[0].state != STATE_CLOSEDOWN)
		goto ERROR_HANDLE;

	/* ��ͨ���رճɹ� */
	CMUX_ResetMultiplexer();

	/* �������ͨ�� */
	for (i = 0; i < MAX_DLC_NUM; i++)
	{
		cmux.DLC[i].state = STATE_CLOSEDOWN;
		CMUX_ResetChannel(i);
	}
	MsgOut("MUX closedown OK\r\n");
	LEAVE_MUTEX;
	return CMUX_SUCCESS;

ERROR_HANDLE:

	cmux.DLC[0].state = STATE_CONNECTED;

ERROR_NOOP:
	MsgOut("MUX closedown Error\r\n");
	LEAVE_MUTEX;
	return CMUX_FAILURE;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_OpenChannel
*
*	�������ܣ���һ��ָ��������ͨ��
*
*   ����������name	����ͨ��������
*
*	�������أ�>=0	ͨ�����
*			  -1	ʧ��
*
\*========================================================================*/

int CMUX_OpenChannel(char* name)
{
	int DLCI;

	/* �����ƻ��ͨ����ʶ */
	if ((DLCI = CMUX_GetDLCIByName(name)) == -1)
    {
        warning("[MUX] get DLCI by name[%s] failure!\r\n", name);
		return -1;
    }

	ENTER_MUTEX;

	/* ֻ���ڸ�ͨ��ΪDISCONNECTED״̬��ʱ�����ִ�д� */
	if (cmux.DLC[DLCI].state != STATE_DISCONNECTED)
	{
        warning("[MUX] before open: DLCI[%d] state[%d]!\r\n", DLCI, cmux.DLC[DLCI].state);
		goto ERROR_HANDLE;
	}

	CMUX_ResetChannel(DLCI);

	/* ���ͨ������SABM֡���������� */
	if (CMUX_SendFrameData(FRAME_SABM, DLCI, NULL, 0) == CMUX_FAILURE)
	{
        warning("[MUX] DLCI[%d] send FRAME_SABM failure!\r\n", DLCI);
		goto ERROR_HANDLE;
	}

	cmux.DLC[DLCI].state = STATE_CONNECTING;

    CMUX_WaitEvent(DLCI, BLOCK_TIMEOUT * 2);

	if (cmux.DLC[DLCI].state != STATE_CONNECTED)
	{
        warning("[MUX] after open: DLCI[%d] state[%d]!\r\n", DLCI, cmux.DLC[DLCI].state);
		goto ERROR_HANDLE;
	}
	
	/* ���ӳɹ�������ͨ����� */
	MsgOut("MUX open DLCI(%s)=%d OK\r\n", name, DLCI);
	LEAVE_MUTEX;
    warning("[MUX] open DLCI[%d] name[%s] success!\r\n", DLCI, name);
	return DLCI;

ERROR_HANDLE:

	MsgOut("MUX open DLCI(%s) Error\r\n", name);
	LEAVE_MUTEX;
    warning("[MUX] open DLCI[%d] name[%s] failure!\r\n", DLCI, name);
	return -1;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_CloseChannel
*
*	�������ܣ��ر�һ������ͨ��
*
*   ����������handle	ͨ�����
*
*	�������أ�CMUX_SUCCESS	�ɹ�
*			  CMUX_FAILURE	ʧ��
*
\*========================================================================*/

int CMUX_CloseChannel(int handle)
{
	UINT8 MSC_DTR[] = {0xe3, 0x05, 0x07, 0x01};

	/* ������Ϸ��� */
	if (handle < 1 || handle > 3)
    {
        warning("[MUX] handle[%d] failure!\r\n", handle);
		return CMUX_FAILURE;
    }

	ENTER_MUTEX;

	/* �����ͨ������CONNECTED״̬������ִ�йرղ��� */
	if (cmux.DLC[handle].state != STATE_CONNECTED)
    {
        warning("[MUX] before close: DLCI[%d] state[%d]!\r\n", handle, cmux.DLC[handle].state);
		goto ERROR_HANDLE;
    }

	/* ���multiplexer���Ǵ��ڹر�״̬���������ͱ�ͨ���Ĺر����� */
	if (cmux.DLC[0].state != STATE_CLOSING)
	{
		/* ����ر�ͨ��1�������Ҫ��DTR(RTC)����Ϊ0 */
		if (handle == 1)
		{
			MSC_DTR[2] = (UINT8)((handle << 2) | 0x03);
			if (CMUX_SendFrameData(FRAME_UIH, 0, MSC_DTR, sizeof(MSC_DTR)) 
				== CMUX_FAILURE)
            {
                warning("[MUX] send MSC_DTR failure!\r\n");
				goto ERROR_HANDLE;
		}
        }
		/* �Ը�ͨ�����Ͷ������� */
		if (CMUX_SendFrameData(FRAME_DISC, handle, NULL, 0) == CMUX_FAILURE)
        {
            warning("[MUX] DLCI[%d] send FRAME_DISC failure!\r\n", handle);
			goto ERROR_HANDLE;
	}
    }

	cmux.DLC[handle].state = STATE_DISCONNECTING;

	CMUX_WaitEvent(handle, BLOCK_TIMEOUT * 2);

	if (cmux.DLC[handle].state != STATE_DISCONNECTED
		&& cmux.DLC[handle].state != STATE_CLOSEDOWN)
    {
        warning("[MUX] after close: DLCI[%d] state[%d]!\r\n", handle, cmux.DLC[handle].state);
		goto ERROR_HANDLE;
    }

	/* �رճɹ� */
	CMUX_ResetChannel(handle);

	MsgOut("MUX close DLCI=%d OK\r\n", handle);
	LEAVE_MUTEX;
    warning("[MUX] close DLCI[%d] success!\r\n", handle);
	return CMUX_SUCCESS;

ERROR_HANDLE:

	MsgOut("MUX close DLCI=%d Error\r\n", handle);
	LEAVE_MUTEX;
    warning("[MUX] close DLCI[%d] failure!\r\n", handle);
	return CMUX_FAILURE;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_WriteChannel
*
*	�������ܣ���ָ��������ͨ����������
*
*   ����������handle	ͨ�����
*			  data		����ָ��
*			  datalen	���ݳ���
*
*	�������أ�>=0	ʵ��д�����ݵ�����
*			  -1	����
*
\*========================================================================*/

int CMUX_WriteChannel(int handle, UINT8* data, int datalen)
{
	int size = 0;

	/* ������Ϸ��� */
	if (handle < 1 || handle > 3)
		return -1;
	if (data == NULL || datalen <= 0)
		return -1;

	ENTER_MUTEX;

	/* ֻ���ڸ�ͨ����CONNECTED״̬�²��ܷ������� */
	if (cmux.DLC[handle].state != STATE_CONNECTED)
		goto ERROR_HANDLE;

	/* �����ز��ܷ������� */
	if (cmux.flowctrl == 1 || cmux.DLC[handle].flowctrl == 1)
		goto ERROR_HANDLE;

	/* ��ʼͨ��UIH֡���ͨ�������û����� */
	size = (FRM_BUFSIZE > datalen ? datalen : FRM_BUFSIZE);
	if (CMUX_SendFrameData(FRAME_UIH, handle, data, size) == CMUX_FAILURE)
		size = 0;

	LEAVE_MUTEX;
	return size;

ERROR_HANDLE:

	LEAVE_MUTEX;
	return -1;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_ReadChannel
*
*	�������ܣ���ͨ���ж�ȡ����
*
*   ����������handle	ͨ�����
*			  buf		������
*			  bufsize	����������
*
*	�������أ�>=0	ʵ�ʶ������ݵ�����
*			  -1	����
*
\*========================================================================*/

int CMUX_ReadChannel(int handle, UINT8* buf, int bufsize)
{
	int readlen;

	/* ������Ϸ��� */
	if (handle < 1 || handle > 3)
		return -1;
	if (buf == NULL || bufsize <= 0)
		return -1;

	ENTER_MUTEX;

	/* ֻ���ڸ�ͨ����CONNECTED״̬�²��ܽ������� */
	if (cmux.DLC[handle].state != STATE_CONNECTED)
		goto ERROR_HANDLE;

	/* �Ӹ�ͨ�������ݽ��ջ������ж�ȡ���� */
	readlen = MPBUF_GetBufData(&cmux.DLC[handle].rxbuf, buf, bufsize);
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
*	�������ƣ�CMUX_SetOption
*
*	�������ܣ����ù���ѡ��
*
*   ����������handle	ͨ�����
*			  option	ѡ�����
*			  value		ѡ��ֵ
*			  len		ѡ��ֵ����
*
*	�������أ�CMUX_SUCCESS	�ɹ�
*			  CMUX_FAILURE	ʧ��
*
\*========================================================================*/

int CMUX_IoControl(int handle, unsigned opcode, int value, int size)
{
	UINT8 MSC_ESC[] = {	0xe3, /* type of UIH frame: ((0x38 << 2)|CR|EA) */
						0x07, /* length of MSC: ((3 << 1)|EA) */
						0x07, /* DLCI : ((1 << 2)|0x20|EA) */
						0x01, /* V.24 : ((0x00 << 2)|EA) */
						0x01};/* Break: (1) */

	/* ������Ϸ��� */
	if (handle < 1 || handle > 3)
		return CMUX_FAILURE;

	ENTER_MUTEX;

	/* ֻ���ڸ�ͨ����CONNECTED״̬�²��ܽ������� */
	if (cmux.DLC[handle].state != STATE_CONNECTED)
		goto ERROR_HANDLE;

	switch (opcode)
	{
	case CMUX_OP_CALLBACK: /* ���ûص����� */
		if (size != sizeof(UART_CALLBK) || value == 0)
			goto ERROR_HANDLE;
		memcpy(&cmux.DLC[handle].callback, (void*)value, sizeof(UART_CALLBK));
		if (!MPBUF_IsEmpty(&cmux.DLC[handle].rxbuf))
			CMUX_CallBack(handle, CMUX_DATAIN);
		break;

	case CMUX_OP_DUMPRXBUF: /* ���ͨ�����ջ����� */
		MPBUF_CleanupData(&cmux.DLC[handle].rxbuf);
		break;

	case CMUX_OP_DATABROKEN:
		if (strcmp((char*)value, "+++") != 0 || size != 3)
			goto ERROR_HANDLE;
		/* ����break signal */
		MSC_ESC[2] = (UINT8)((handle << 2)|0x02|0x01);
		if (CMUX_SendFrameData(FRAME_UIH, 0, MSC_ESC, sizeof(MSC_ESC)) 
			== CMUX_FAILURE)
			goto ERROR_HANDLE;
		break;

	default:
		goto ERROR_HANDLE;
	}
	
	LEAVE_MUTEX;
	return CMUX_SUCCESS;

ERROR_HANDLE:

	LEAVE_MUTEX;
	return CMUX_FAILURE;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_OnDatain
*
*	�������ܣ����ݵ����ص�����
*
*   ������������
*
*	�������أ�CMUX_SUCCESS	�ɹ�
*			  CMUX_FAILURE	ʧ��
*
\*========================================================================*/

int CMUX_OnDatain(void)
{
	/* ��DLC0�Ľ��ջ���������������ջ����� */
	static UINT8*	  buf = cmux.DLC[0].rxbuf.buf; 
	static int		  datalen = 0;
	static UINT8	  fcs = 0xff;

	int		readlen, i;

	/* ��ȡ���� */
	while ((readlen = CMUX_ReadDevice(buf, BUF_SIZE)) >= 0)
	{
		/* process incoming data */
		if (readlen == 0)
			continue;
		if (cmux.scanstate == S_CMUX)
		{
			char* p = (char*)buf;
			int atlen, len, freelen, tag = 0;
			while (readlen > 0)
			{
				while (strlen(at_buf) >= ATCMD_LEN)
					strcpy(at_buf, at_buf + ATCMD_LEN/2);
				atlen = strlen(at_buf);
				freelen = ATCMD_LEN - atlen;
				len = strlen(p);
				len = (len > freelen ? freelen : len);
				memcpy(at_buf + atlen, p, len);
				at_buf[atlen + len] = '\0';
				if (strstr((char*)at_buf, "\r\nOK\r\n") != NULL)
					tag = 1;
				else if (strstr((char*)at_buf, "\r\nERROR\r\n") != NULL)
					tag = 2;

				if (tag != 0)
				{
					at_buf[0] = '\0';
					ENTER_MUTEX;
					if (cmux.DLC[0].state == STATE_ATCMDCMUX)
					{
						if (tag == 1)
							cmux.DLC[0].state = STATE_CONNECTING;
						LIB_SignalEvent(cmux.DLC[0].event, 1);
					}
					LEAVE_MUTEX;
				}
				p += len;
				readlen -= len;
			}
			continue;
		}
		for (i = 0; i < readlen; )
		{
			switch (cmux.scanstate)
			{
			case S_FLAG:
				if (buf[i] == BASIC_FLAG)
				{
					cmux.scanstate = S_ADDR;
					datalen = 0;
					fcs = 0xff;
				}
				else
					MsgOut("MUX discard char [%c] !!!\r\n", buf[i]);
				i++;
				break;
			case S_ADDR:
				if (buf[i] == BASIC_FLAG)
				{
					cmux.scanstate = S_FLAG;
					break;
				}
				cmux.frame.DLCI = buf[i] >> 2;
				cmux.frame.CR_bit = (buf[i] & ADDRESS_CR_BIT) >> 1;
				if (cmux.frame.DLCI >= 0 && cmux.frame.DLCI <= 3 && (buf[i] & EXTENSION_BIT))
				{
					cmux.scanstate = S_CTRL;
					fcs = fcstab8[fcs^buf[i++]];
				}
				else
				{
					MsgOut("MUX get data through unknow DLCI=%d\r\n", cmux.frame.DLCI);
					cmux.scanstate = S_FLAG;
				}
				break;
			case S_CTRL:
				cmux.frame.type = (buf[i] & ~CONTROL_PF_BIT);
				cmux.frame.PF_bit = (buf[i] & CONTROL_PF_BIT);
				if (cmux.frame.type == FRAME_SABM
					|| cmux.frame.type == FRAME_UA
					|| cmux.frame.type == FRAME_DM
					|| cmux.frame.type == FRAME_DISC
					|| cmux.frame.type == FRAME_UIH)
				{
					cmux.scanstate = S_LEN1;
					fcs = fcstab8[fcs^buf[i++]];
				}
				else
				{
					MsgOut("MUX get unknow frame type=%02x\r\n", cmux.frame.type);
					cmux.scanstate = S_FLAG;
				}
				break;
			case S_LEN1:
				if (buf[i] & EXTENSION_BIT)
				{
					cmux.frame.datalen = buf[i] >> 1;
					if (cmux.frame.datalen > MAX_FRAME_SIZE)
					{
						MsgOut("MUX frame1 size(=%d) exceed the limit\r\n", 
							cmux.frame.datalen);
						cmux.scanstate = S_FLAG;
					}
					else
					{
						datalen = 0; 
						cmux.scanstate = S_DATA;
						fcs = fcstab8[fcs^buf[i++]];
					}
				}
				else
				{
					cmux.frame.datalen = buf[i];
					cmux.scanstate = S_LEN2;
					fcs = fcstab8[fcs^buf[i++]];
				}
				break;
			case S_LEN2:
				cmux.frame.datalen += (buf[i] << 8);
				cmux.frame.datalen >>= 1;
				if (cmux.frame.datalen > MAX_FRAME_SIZE)
				{
					MsgOut("MUX frame2 size(=%d) exceed the limit\r\n", 
						cmux.frame.datalen);
					cmux.scanstate = S_FLAG;
				}
				else
				{
					datalen = 0;
					cmux.scanstate = S_DATA;
					fcs = fcstab8[fcs^buf[i++]];
				}
				break;
			case S_DATA:
				/* read data octet one by one */
				if (datalen < cmux.frame.datalen)
				{
					cmux.frame.data[datalen++] = buf[i++];
					//fcs = fcstab8[fcs^buf[i++]];
				}
				else
					cmux.scanstate = S_FCS;
				break;
			case S_FCS:
				if ((UINT8)(0xff - fcs) == buf[i])
				{
					/* �ɹ��Ļ��һ֡���д��� */
					ENTER_MUTEX;
					CMUX_ProcRxFrame(&cmux.frame);
					LEAVE_MUTEX;

					i ++;
				}
				else
					MsgOut("MUX FCS(=%02x) error!\r\n", (unsigned char)buf[i]);
				cmux.scanstate = S_FLAG;
				break;
			case S_CMUX:
				break;
			default:
				cmux.scanstate = S_FLAG;
			}
		}
	}
	return CMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_OnDataout
*
*	�������ܣ����Լ����������ݵĻص�����
*
*   ������������
*
*	�������أ�CMUX_SUCCESS	�ɹ�
*			  CMUX_FAILURE	ʧ��
*
\*========================================================================*/

int CMUX_OnDataout(void)
{
	int sendlen, i;

	ENTER_MUTEX;

	/* ���ͻ���������δ��������ϵ����� */
	while (cmux.datalen > 0)
	{
		if ((sendlen = CMUX_WriteDevice(cmux.data, cmux.datalen)) <= 0)
			break;

		cmux.datalen -= sendlen;
		cmux.data    += sendlen;
		if (cmux.datalen == 0)
		{
			cmux.data = cmux.txbuf;
			cmux.flowctrl = 0;
			for (i = 1; i < MAX_DLC_NUM; i ++)
			{
				if (cmux.DLC[i].flowctrl == 0)
					CMUX_CallBack(i, CMUX_DATAOUT);
			}
		} /* if (cmux.datalen == 0) */
	} /* while (cmux.datalen > 0) */

	LEAVE_MUTEX;
	return CMUX_SUCCESS;
}

/**************************************************************************\
*
*	�ڲ�����ʵ�ֲ��� (internal function)
*
\**************************************************************************/

/*========================================================================*\
*
*	�������ƣ�CMUX_GetDLCIByName
*
*	�������ܣ�����ͨ�������ƻ��ͨ����ʶ
*
*   ����������channel_name	ͨ��������
*
*	�������أ�ͨ���ı�ʶ
*
\*========================================================================*/

static int CMUX_GetDLCIByName(char* channel_name)
{
	if (strcmp(channel_name, CMUX_CHANNEL1) == 0)
		return 1;
	else if (strcmp(channel_name, CMUX_CHANNEL2) == 0)
		return 2;
	else if (strcmp(channel_name, CMUX_CHANNEL3) == 0)
		return 3;
	else
		return -1;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_FrameCheckSum
*
*	�������ܣ��������ݵ�У���
*
*   ����������data		����ָ��
*			  datalen	���ݳ���
*
*	�������أ�CMUX_SUCCESS	�ɹ�
*			  CMUX_FAILURE	ʧ��
*
\*========================================================================*/

static UINT8 CMUX_FrameCheckSum(UINT8 *data, int datalen)
{
    UINT8 fcs = 0xff;
    while (datalen--)
    {
        fcs = fcstab8[fcs^(*data++)];
    }
    fcs = (UINT8)(0xff - fcs);            // FCS8INIT = 0xFF
    return fcs;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_SendFrameData
*
*	�������ܣ���ָ����֡�ŵ�cmux��txbuf�У������������ʼ����
*
*   ����������frametype		֡������ (SABM, UA, MD, DISC, UIH)
*			  DLCI			ͨ����ʶ (0��3)
*			  data			����ָ��
*			  datalen		���ݳ���
*
*	�������أ�CMUX_SUCCESS	�ɹ�
*			  CMUX_FAILURE	ʧ��
*			  CMUX_ASYNC		�첽����
*
\*========================================================================*/

/**************************************************************************
 * MUX Frame
 * format: | flag | addr | ctrl | len1 | len2 | infor.(n octets) | FCS | flag |
 * flag  : 0xF9
 * addr  : I6 I5 I4 I3 I2 I1 CR EA
 * ctrl  : T7 T6 T5 PF T4 T3 T2 T1
 * len_1 : L7 L6 L5 L4 L3 L2 L1 EA
 * len_2 : Lf Le Ld Lc Lb La L9 L8 (optional based on len_1 EA bit)
 * infor : 0 - n octets (n==len)
 * FCS   : addrress & control field calculated FCS
 * flag  : 0xF9
 **************************************************************************
 */

static int CMUX_SendFrameData(UINT8 frametype, int DLCI, 
							UINT8* data, int datalen)
{
	UINT8 SABM[4][6] = 
	{
		{0xf9, 0x03, 0x3f, 0x01, 0x1c, 0xf9},
		{0xf9, 0x07, 0x3f, 0x01, 0xde, 0xf9},
		{0xf9, 0x0b, 0x3f, 0x01, 0x59, 0xf9},
		{0xf9, 0x0f, 0x3f, 0x01, 0x9b, 0xf9}
	};
	UINT8 DISC[4][6] = 
	{
		{0xf9, 0x03, 0x53, 0x01, 0xfd, 0xf9},
		{0xf9, 0x07, 0x53, 0x01, 0x3f, 0xf9},
		{0xf9, 0x0b, 0x53, 0x01, 0xb8, 0xf9},
		{0xf9, 0x0f, 0x53, 0x01, 0x7a, 0xf9}
	};
	//UINT8 CLSDOWN[] = {0xf9, 0x03, 0xef, 0x05, 0xc3, 0x01, 0xf2, 0xf9};

	UINT8 buf[FRM_BUFSIZE+10];
	int writelen, len = 0, off = 0;

	if (DLCI < 0 || DLCI > 3)
		return CMUX_FAILURE;

	/* �������� */
	if (frametype == FRAME_SABM)
	{
		len = sizeof(SABM[DLCI]);
		memcpy(buf, SABM[DLCI], len);
	}
	else if (frametype == FRAME_DISC)
	{
		len = sizeof(DISC[DLCI]);
		memcpy(buf, DISC[DLCI], len);
	}
	else if (frametype == FRAME_UIH)
	{
		if (data == NULL || datalen <= 0 || datalen > FRM_BUFSIZE)
			return CMUX_FAILURE;

		buf[len++] = BASIC_FLAG;
		buf[len++] = (UINT8)((DLCI << 2) | 0x03); /* EA = 1; C/R = 1 */
		buf[len++] = FRAME_UIH;		/* P/F = 0 */
		/* ����length ����Ҫ��С��172 */
		buf[len++] = (UINT8)((datalen << 1) | 0x01);
		memcpy(buf + len, data, datalen);
		len += datalen;
		buf[len++] = CMUX_FrameCheckSum(&buf[1], 3);
		buf[len++] = BASIC_FLAG;
	}
	else
		return CMUX_FAILURE;

	/* ���û��������ֱ�ӷ������� */
	if (cmux.flowctrl == 0)
	{
		cmux.data = cmux.txbuf;
		while (len > 0)
		{
			writelen = CMUX_WriteDevice(buf+off, len);
			if (writelen < 0)
				break;
			len -= writelen;
			off += writelen;
		}
	}
	/* ���û�з�����ϣ���ʣ������ݱ�����txbuf�ȴ��첽���ͣ�
	 * ���������� 
	 */
	if (len > 0)
	{
		if (len > BUF_SIZE - cmux.datalen)
			return CMUX_FAILURE;

		memcpy(cmux.data, buf+off, len);
		cmux.datalen += len;
		cmux.flowctrl = 1;
		return CMUX_ASYNC;
	}
	return CMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_ProcRxFrame
*
*	�������ܣ�������յ���һ֡����
*
*   ����������frame		���յ���֡�ṹ������
*
*	�������أ�CMUX_SUCCESS	�ɹ�
*			  CMUX_FAILURE	ʧ��
*
\*========================================================================*/

static int CMUX_ProcRxFrame(FRAME *frame)
{
	int tag = 0;
	switch (frame->type)
	{
	case FRAME_UA:
		if (cmux.DLC[frame->DLCI].state == STATE_CONNECTING)
		{
			cmux.DLC[frame->DLCI].state  = STATE_CONNECTED;
			LIB_SignalEvent(cmux.DLC[frame->DLCI].event, 1);
		}
		else if (cmux.DLC[frame->DLCI].state == STATE_DISCONNECTING)
		{
			cmux.DLC[frame->DLCI].state  = STATE_DISCONNECTED;
			LIB_SignalEvent(cmux.DLC[frame->DLCI].event, 1);
		}
		break;
	case FRAME_DM:
		if (cmux.DLC[frame->DLCI].state == STATE_CONNECTING)
		{
			cmux.DLC[frame->DLCI].state  = STATE_DISCONNECTED;
			LIB_SignalEvent(cmux.DLC[frame->DLCI].event, 1);
		}
		else if (cmux.DLC[frame->DLCI].state == STATE_CONNECTED)
		{
			cmux.DLC[frame->DLCI].state  = STATE_DISCONNECTED;
			CMUX_ResetChannel(frame->DLCI);
			CMUX_CallBack(frame->DLCI, CMUX_MODEM);
		}
		break;
	case FRAME_UIH:
		if (frame->DLCI == 0)
		{
			/* CloseDown�ɹ� */
			if (frame->datalen == 2 
				&& frame->data[0] == 0xc1 /* command type == Closedown */
				&& frame->data[1] == 0x01)/* value length == 0 */
			{
				int i;
				for (i = 1; i < MAX_DLC_NUM; i ++) /* close all channel */
				{
					/* Q(03) Ӧ�����괦����ٽ�״̬�ó�STATE_CLOSEDOWN */
					//cmux.DLC[i].state = STATE_CLOSEDOWN;
					switch (cmux.DLC[i].state)
					{
					case STATE_CONNECTING:
						LIB_SignalEvent(cmux.DLC[i].event, 1);
						break;
					case STATE_CONNECTED:
						CMUX_ResetChannel(i);
						CMUX_CallBack(i, CMUX_MODEM);
						break;
					case STATE_DISCONNECTING:
						LIB_SignalEvent(cmux.DLC[i].event, 1);
						break;
					}
					/* Q(03) */
					cmux.DLC[i].state = STATE_CLOSEDOWN;
				}

				if (cmux.DLC[0].state == STATE_CLOSING)
					tag = 1;

				cmux.DLC[0].state = STATE_CLOSEDOWN;

				if (tag == 1)
					LIB_SignalEvent(cmux.DLC[0].event, 1);
				else
					CMUX_ResetMultiplexer();
			}
			/* ���������� */
			else if (frame->datalen >= 4 && frame->datalen <= 5
				&& ((frame->data[0] & ~0x02) == 0xe1) /* command type == Modem State Command */
				&& (frame->data[1] >> 1) + 2 == frame->datalen) /* value length == 2 */
			{
				int DLCI = frame->data[2] >> 2;
				if (frame->data[0] & 0x02)
				{
					MsgOut("\r\nReceive MSC command\r\n");
					/* ����module��MSC����,��Ҫ����Ӧ�� */
					if (DLCI >= 0 && DLCI <= 3)
					{
						/* �������� */
						if (frame->data[3] & 0x02)
							cmux.DLC[DLCI].flowctrl = 1;
						else
						{
							int tag = cmux.DLC[DLCI].flowctrl;
							cmux.DLC[DLCI].flowctrl = 0;
							/* �����ͨ����������ON��OFF����������ͨ��û���أ���֪ͨDATAOUT */
							if (tag == 1 && cmux.flowctrl == 0)
								CMUX_CallBack(DLCI, CMUX_DATAOUT);
						}
						/* ��command frame ��� response frame */
						frame->data[0] = 0xe1;	/* MSC response */

						/* ����MSC ��Ӧ�� */
						CMUX_SendFrameData(FRAME_UIH, 0, frame->data, frame->datalen);
					}
				}
				else
				{
					/* ����module��MSCӦ��,����Ҫ����Ӧ�� */
					MsgOut("\r\nReceive MSC response\r\n");
				}
			}
			else
			{
				MsgOut("\r\n\r\nDLC0 receive a invalid frame !!!!!!!!!!\r\n\r\n");
			}
		}
		else
		{
			int tag = MPBUF_IsEmpty(&cmux.DLC[frame->DLCI].rxbuf);
			/* ������ͨ��������� */
			MPBUF_PutBufData(&cmux.DLC[frame->DLCI].rxbuf, frame->data, frame->datalen);
			if (tag)
				CMUX_CallBack(frame->DLCI, CMUX_DATAIN);
		}
		break;
	default:
		break;
	}
	return CMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_WaitEvent
*
*	�������ܣ��ȴ�����UA��MD���Լ���CLOSEDOWN�Ļ�Ӧ�¼�����
*
*   ����������DLCI			ͨ����ʶ
*			  microsecond	����ʱ��
*
*	�������أ�CMUX_SUCCESS	�ɹ����¼�����
*			  CMUX_FAILURE	ʧ�ܣ���ʱ
*
\*========================================================================*/

static int CMUX_WaitEvent(int DLCI, int microsecond)
{
	/* �����ʱ��� */
	cmux.DLC[DLCI].timeoutflag = 0;

	/* ���ü�ʱ�� */
	LIB_SetTimerFunc(&cmux.DLC[DLCI].timer, CMUX_TimeOut, &cmux.DLC[DLCI]);
	LIB_SetTimerExpire(&cmux.DLC[DLCI].timer, microsecond);

	/* ������ʱ�� */
	LIB_StartTimer(&cmux.DLC[DLCI].timer);

	LEAVE_MUTEX;
	LIB_WaitEvent(cmux.DLC[DLCI].event);
	ENTER_MUTEX;

	/* ��ֹʱ�� */
	LIB_StopTimer(&cmux.DLC[DLCI].timer);

	/* �����ʱ */
	if (cmux.DLC[DLCI].timeoutflag == 1)
		return CMUX_FAILURE;

	return CMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_TimeOut
*
*	�������ܣ���ʱ�ص�����
*
*   ����������DLC	ͨ��
*
*	�������أ���
*
\*========================================================================*/

static void CMUX_TimeOut(void* channel)
{
	CHANNEL* DLC = (CHANNEL*)channel;
	/* ����timeout��� */
	DLC->timeoutflag = 1;
    
    warning("[MUX] Timeout!\r\n");

	/* ���ѵȴ��¼� */
	ENTER_MUTEX;
	if (DLC->state == STATE_ATCMDCMUX
		|| DLC->state == STATE_CONNECTING
		|| DLC->state == STATE_DISCONNECTING
		|| DLC->state == STATE_CLOSING)
	LIB_SignalEvent(DLC->event, 1);
	LEAVE_MUTEX;
	return;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_CallBack
*
*	�������ܣ��û��ص�����
*
*   ����������DLC		ͨ��
*			  event		�ص��¼� (DATAIN��DATAOUT��BROKEN)
*
*	�������أ���
*
\*========================================================================*/

static void CMUX_CallBack(int DLCI, unsigned int event)
{	
	if (cmux.DLC[DLCI].callback.userfunc != NULL)
	{
		if (cmux.DLC[DLCI].callback.EventCode & event)
		{
			LEAVE_MUTEX;
			cmux.DLC[DLCI].callback.userfunc
				((long)cmux.DLC[DLCI].callback.para, event, DLCI, 0L);
			ENTER_MUTEX;
		}
	}	
	return;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_ResetChannel
*
*	�������ܣ���λָ����ͨ��
*
*   ����������DLCI		ͨ����ʶ
*
*	�������أ�CMUX_SUCCESS	�ɹ�
*			  CMUX_FAILURE	ʧ��
*
\*========================================================================*/

static int CMUX_ResetChannel(int DLC)
{
	cmux.DLC[DLC].flowctrl = 0;
	memset(&cmux.DLC[DLC].callback, 0, sizeof(UART_CALLBK));
	MPBUF_CleanupData(&cmux.DLC[DLC].rxbuf);
	return CMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_ResetMultiplexer
*
*	�������ܣ���λmultiplexerģ��
*
*   ������������
*
*	�������أ�CMUX_SUCCESS	�ɹ�
*			  CMUX_FAILURE	ʧ��
*
\*========================================================================*/

static int CMUX_ResetMultiplexer(void)
{
	cmux.read     = NULL;
	cmux.write    = NULL;
	cmux.data     = cmux.txbuf;
	cmux.datalen  = 0;
	cmux.flowctrl = 0;
	cmux.scanstate = S_CMUX;
	return CMUX_SUCCESS;
}

/*========================================================================*\
*
*	�������ƣ�CMUX_WriteDevice
*
*	�������ܣ��򴮿��豸д������
*
*   ����������data		����ָ��
*			  datalen	���ݳ���
*
*	�������أ�ʵ��д�������
*
\*========================================================================*/

static int CMUX_WriteDevice(UINT8* data, int datalen)
{
	if (cmux.write == NULL || data == NULL || datalen < 0)
		return -1;
	return cmux.write(data, datalen);
}

/*========================================================================*\
*
*	�������ƣ�CMUX_ReadDevice
*
*	�������ܣ��Ӵ����豸�ж�ȡ����
*
*   ����������buf		��������ַ
*			  bufsize	�������Ĵ�С
*
*	�������أ�ʵ�ʶ�������������
*
\*========================================================================*/

static int CMUX_ReadDevice(UINT8* buf, int bufsize)
{
	if (cmux.read == NULL || buf == NULL || bufsize < 0)
		return -1;
	return cmux.read(buf, bufsize);
}

/**************************************************************************\
*	BUFFER������
\**************************************************************************/
/*========================================================================*\
*
*	�������ƣ�MPBUF_GetDataSize
*
*	�������ܣ������������
*
*   ����������pbuf	ָ�򻺳�������
*
*	�������أ���������
*
\*========================================================================*/
#if 0
static int MPBUF_GetDataSize(BUFFER* pbuf)
{
	if (pbuf == NULL)
		return 0;
	return pbuf->datalen;
}
#endif
/*========================================================================*\
*
*	�������ƣ�MPBUF_CleanupData
*
*	�������ܣ�����������е�����
*
*   ����������pbuf	ָ�򻺳�������
*
*	�������أ�0		�ɹ�
*			  -1	ʧ��
*
\*========================================================================*/

static int MPBUF_CleanupData(BUFFER* pbuf)
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
*	�������ƣ�MPBUF_GetBufData
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

static int MPBUF_GetBufData(BUFFER* pbuf, UINT8* buf, int buflen)
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
*	�������ƣ�MPBUF_PutBufData
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

static int MPBUF_PutBufData(BUFFER* pbuf, UINT8* data, int datalen)
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
/* end of program siem_mux.c */
