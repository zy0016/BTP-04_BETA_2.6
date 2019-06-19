/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Bluetooth module
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef __BTTHREAD_H__
#define __BTTHREAD_H__

#include "BtLowCall.h"
#include "pthread.h"

//macro for request_type in struct request
#define RT_BT_SEARCHDEVICE      0x1       //搜索设备请求
#define RT_BT_PAIR			    0x2       //配对设备请求
#define RT_BT_CONNECT_FTP		0x3       //连接设备请求(FTP)
#define RT_BT_CONNECT_OPP		0x4       //连接设备请求(OPP)
#define RT_BT_FILETRANSFER	    0x5       //文件传输请求(FTP)
#define RT_BT_MULPICFILETRANSFER 0x6      //多个图片文件传输(FTP)
#define RT_BT_OBJECTTRANSFER    0x7       //文件传输请求(OPP)
#define RT_BT_GETREMOTEDEVINFO  0x8       //得到远程蓝牙设备名
#define RT_BT_SWITCHON          0x9       //启动蓝牙
#define RT_BT_SWITCHOFF			0x10	  //switch off bluetooth device	
#define RT_BT_COPYRING		    0x11      //Copy RING signal to BT device
#define RT_BT_UPDATEREMOTEDEVINFO  0x12    //更新远程蓝牙设备名
#define RT_BT_CONNECT_HEADSET      0x13    //连接Audio设备

//macro for state in struct request
#define RQ_UNFINISHED		0x0
#define RQ_FINISHED			0x1
#define RQ_CANCEL			0x2

/* format of a single request. */
struct request {
	HWND hWnd;			
	int message;
	int request_type;
	void *request_param;
	void *result_buffer;
	int result_buffer_len;
	int state;
    int number;		    /* number of the request                  */
    struct request* next;   /* pointer to next request, NULL if none. */
};

/* structure for a requests queue */
struct requests_queue {
    struct request* requests;       /* head of linked list of requests. */
    struct request* last_request;   /* pointer to last request.         */
    int num_requests;		    /* number of requests in queue.     */
    pthread_mutex_t* p_mutex;	    /* queue's mutex.                   */
    pthread_cond_t*  p_cond_var;    /* queue's condition variable.      */
};

void InitBtThread(void);

int BT_SendRequest(HWND hWnd,int message,int request_type,void *param,void *result_buffer,int result_buffer_len);
int BT_CancelRequest(int request_num);

#endif 
