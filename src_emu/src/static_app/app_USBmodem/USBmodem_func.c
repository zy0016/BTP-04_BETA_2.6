/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : USBmodem_func.c
 *
 * Purpose  : 
 *
\**************************************************************************/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/poll.h>
#include <unistd.h>
#include "hopen/comm.h"
#include <sys/ipc.h>

#include "winpda.h"  
#include "string.h"
#include "malloc.h"
#include "stdlib.h"
#include "stdio.h" 
#include "log.h"
#include "Fapi.h"
#include "DI_Uart.h"
#include "device.h"
#include "Ioctl.h"
#include "plx_pdaex.h"
#include "Me_at.h"
//#include "multchnl.h"
#include "setup.h"
#include "pubapp.h"

#define ID_ENTER 332
#define ID_EXIT 333

#define USBDEVICENAME "/dev/com5"
#define USBBUFSIZE	        128

#define WM_MODEM_TSET				(WM_USER+600)

//#define USBMODEMDEBUG
#ifdef _EMULATE_
#ifdef USBMODEMDEBUG
#define UM_DEBUG(x...) do{printf(##x);}while(0)
#else
#define UM_DEBUG(x...) do{}while(0)
#endif
#else//_EMULATE_
#define UM_DEBUG(x) do{}while()
#endif

static int usb_handle = -1;
static pthread_t usb_thr;
static int usb_thrstatus = 0;
static int	ModemStatus = 0;
static char ring_buf[9] = {0};
struct pollfd ufds[2];

//#ifdef _EMULATE_
extern void MsgOut(char *format, ... );
//#else
//static void MsgOut(char* format, ... ){format=NULL;}
//#endif
extern void PostRingMessage(void);
extern int errno;

/* USB控制结构 */
typedef struct UART_Control
{
    _CALLBACK	Event_CallBack;
    LONG		para;
    DWORD		EventCode;
}UART_CONTROL, *PUART_CONTROL;

static void *USB_task(void* param);
//static BOOL InitUSBmodem(void);
static int USBPort_Open (const char * DeviceName);
static int USBPort_Close (void);
static void USBIndication (int event, void * pdata, int datalen);
static char usb_buf[USBBUFSIZE];
extern int Modem_WaitOnDataOut();

static void *USB_task(void* param)
{

    int ret;
    int offset;
#ifdef USBMODEMDEBUG
    int loop;
#endif

    ufds[0].fd = usb_handle;
    ufds[0].events = POLLIN;
    ufds[0].revents = 0;

    ufds[1].fd = CreateEvent(IPC_EVENT, 0);
    ufds[1].events = POLLIN;
    ufds[1].revents = 0;
	
    while(1)
    {
	ret = poll(ufds, 2, -1);

	if(0 != (ufds[0].revents & POLLIN))
	{
	    if(ret)
	    {
		//从usb通道读出数据			
		ret = read(usb_handle, usb_buf, USBBUFSIZE/2);
		if (ret > 0)
		{
#ifdef USBMODEMDEBUG
		    printf("$USB Received %d bytes\r\n", ret);
		    for (loop = 0; loop < ret; loop++)
		    {
			if ((loop % 10) == 0)
			    printf("\r\n");
			printf(" %02x", usb_buf[loop]);
		    }
		    printf("\r\n");
#endif

		    offset = 0;
		    while (ret - offset > 0)
		    {
			int sent;
			sent = Modem_Send (usb_buf + offset, ret - offset);
			while (sent < 0)
			{
			    Modem_WaitOnDataOut();

			    sent = Modem_Send(usb_buf + offset, ret - offset);
			}

			offset += sent;
		    }
#ifdef USBMODEMDEBUG
		    printf("$send to ME#:[%d]\r\n", ret);
#endif
		}
	    }
	}

	if(usb_thrstatus)
	{
#ifdef USBMODEMDEBUG
	    printf( "\r\nready to threadexit\r\n" );
#endif			
	    pthread_exit(NULL);
	}
			
    }	
}

static int USBPort_Open (const char * DeviceName)
{
    int				modemctl = 1;
    int 				result;
	
    //打开usb 设备
    usb_handle = open(DeviceName, O_RDWR|O_NONBLOCK);
	
    if (usb_handle < 0)
    {
#ifdef USBMODEMDEBUG
	printf( "\r\nopen USB fail, error number = %d\r\n", errno);
#endif
		
	return -1;
    }

#ifdef USBMODEMDEBUG
    printf( "\r\nopen USB OK!!!!!\r\n" );
#endif
	
    //设置usb 通道
    result = ioctl(usb_handle, SIOSETMODEMCTRL, &modemctl, 0 );
    if(-1 == result)
	goto ERRORHANDLE;	

    usb_thrstatus = 0;

    result = pthread_create(&usb_thr, NULL, USB_task, NULL);
    if(result)
	goto ERRORHANDLE;

    return 1;
	
ERRORHANDLE:
    return -1;
}

static int USBPort_Close(void)
{
    int	ret;

    //关闭读usb线程	
    if(usb_handle < 0)
    {
#ifdef USBMODEMDEBUG
	printf( "\r\n-1 == usb_handle!!!!!\r\n" );
#endif		
	return -1;
    }

    usb_thrstatus = 1;

#ifdef USBMODEMDEBUG
    printf( "\r\nset event!!!!!\r\n" );
#endif	
    SetEvent (ufds[1].fd, 1);
#ifdef USBMODEMDEBUG
    printf( "\r\nreturn from set event!!!!!\r\n" );
#endif	

    ret = pthread_join(usb_thr, NULL);
	
    if(0 != ret)
    {
#ifdef USBMODEMDEBUG
	printf( "\r\nkill thread fail!!!!!\r\n" );
#endif			
	return -1;
    }			

#ifdef USBMODEMDEBUG
    printf( "\r\nkill thread OK!!!!!\r\n" );
#endif		


    ret = close(usb_handle);

    if(-1 == ret)
    {
#ifdef USBMODEMDEBUG
	printf( "\r\nclose usb fail!!!!!\r\n" );
#endif
	return -1;
    }
	
    usb_handle = -1;
#ifdef USBMODEMDEBUG
    printf( "\r\nclose usb ok!!!!!\r\n" );
#endif		
	
    return 1;
}

static char *memstr(const void *s, const char *str, size_t n)
{
    unsigned long remain = n;
    char *p = (char *)s;

    if (s == NULL)
        return NULL;
    if (str == NULL)
        return NULL;

    while ((p = memchr(p, *str, remain)) != NULL)
    {
        if (remain < strlen(str))
        {
            p = NULL;
            break;
        }
        if (memcmp(p, str, strlen(str)) == 0)
        {
            break;
        }
        p++;
        remain = n - (int)p + (int)s;
    }
    return p;
}

static void USBIndication (int event, void * pdata, int datalen)
{
    int len = 0;
    int offset = 0;
    int sent = 0;

    len = datalen;

    if ( event != MAT_DATAIN ) 
        return;

    if(usb_handle < 0)
        return;

    if (memstr(pdata, "RING\r", datalen))
	PostRingMessage();
    else
    {
	memcpy(ring_buf + 4, pdata, 4);
	if (strstr(ring_buf, "RING\r"))
	{
	    PostRingMessage();
	}
    }
    memset(ring_buf, 0, 9);
    memcpy(ring_buf, (char *)pdata + datalen - 4, 4);


    while (offset < len)
    {
#if 0
	printf("comm received %d bytes\r\n", len - offset);
#endif
        sent = write(usb_handle, (char*)pdata+offset, len - offset);

#if 0
	printf("send to usb %d bytes\r\n", sent);
#endif

        if (sent < 0)
        {
            return;
        }
        else
            offset += sent;				
    }

    return;
}

static DWORD modem_start(void)
{
    DWORD dwRet = TRUE;

    if(ModemStatus)
	return dwRet;
				
    //取usb 设置情况，若为modem 则调用modem接口
    if(-1 ==Modem_OpenCnl(USBIndication))
    {
	//错误处理
	printf("\r\n----------------channel open fail------------\r\n");
					
	dwRet = FALSE;

	//弹出提示对话框通知用户暂时不能使用usb modem
	PLXTipsWin(NULL,NULL,0,ML("Module busy,\r\nplease waitting"), NULL, Notify_Alert, NULL, NULL, WAITTIMEOUT);
    }
    //打开usb设备
    else if(-1 == USBPort_Open((PCSTR)USBDEVICENAME))
    {
					
	//打开usb设备不成功，关闭无线模块通道1
	printf("\r\n----------------usb open fail!!!------------\r\n");
					
	Modem_CloseCnl();

	//弹出提示对话框通知用户usb 暂时打不开
	PLXTipsWin(NULL,NULL,0,ML("USB busy,\r\nplease waitting"), NULL, Notify_Alert, NULL, NULL, WAITTIMEOUT);
					
	//错误处理
	dwRet = FALSE;
    }
    else
    {
	//successful!
#ifdef USBMODEMDEBUG	
	MsgOut("\r\n----------------usb modem ok!!!------------\r\n");
#endif
					
	ModemStatus = 1;
    }
    return dwRet;
}

static DWORD modem_exit(void)
{
    DWORD dwRet = TRUE;
    //usb 连接断开
    if(!ModemStatus)
	return dwRet;

    ModemStatus = 0;
    //调用modem接口关闭通道	
				
    //关闭usb设备
    if(-1 == USBPort_Close())
    {
	//错误处理
#ifdef USBMODEMDEBUG
	MsgOut("\r\n----------------usb close fail------------\r\n");
#endif
					
	dwRet = FALSE;
    }

    if(-1 == Modem_CloseCnl())
    {
	//错误处理
	printf("\r\n----------------channel close fail------------\r\n");
					
	dwRet = FALSE;
    }
    return dwRet;
}

DWORD USBmodem_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
    DWORD dwRet = TRUE;

    switch (nCode)
    {
    case APP_DESTROY:

	dwRet = modem_exit();				
	break;

    case APP_INIT :
	if (0)
	{
	    dwRet = modem_start();
	}
	break;
    case APP_ACTIVE:
	if (lParam == USB_IN)
	{
	    dwRet = modem_start();				
	}
	else if(USB_OUT == lParam)
	{
	    dwRet = modem_exit();				
	}
	break;


    default :
	break;
    }

    return dwRet;
}
