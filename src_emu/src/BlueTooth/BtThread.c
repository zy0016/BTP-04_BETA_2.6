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

#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "assert.h"
#include "string.h"
#include "window.h"
#include "malloc.h"
#include "prebrow.h"

#include "BtMain.h"
#include "BtThread.h"
#include "BtSendData.h"
#include "BtFileOper.h"

#include "ag_hfap.h"
#include "sys/task.h"



static BTSVCHDL g_rmt_svc_hdls[32];
static BTINT32 g_rmt_svc_num;
static BTSVCHDL g_sel_rmt_svc = BTSDK_INVALID_HANDLE;
static BTCONNHDL g_cur_conn_hdl = BTSDK_INVALID_HANDLE;

/* global mutex for our program. */
static pthread_mutex_t request_mutex ;

static pthread_mutex_t request_num_mutex;

/* global condition variable for our program. */
static pthread_cond_t  got_request ;

extern pthread_mutex_t BtRecvData_mutex;  //接收蓝牙文件时需要的一个互斥锁
extern pthread_cond_t  BtRecvData_cond;  //接收蓝牙文件时需要的条件变量

//pthread_mutex_t BtStopSearch_mutex;
pthread_mutex_t BtDeviceNodeListMutex;

/* are we done creating new requests? */
int done_creating_requests = 0;

static struct requests_queue* bt_requests; /* pointer to requests queue */

static int g_tid;
static pthread_t  g_pthread;

/*
 * function init_requests_queue(): create a requests queue.
 * algorithm: creates a request queue structure, initialize with given
 *            parameters.
 * input:     queue's mutex, queue's condition variable.
 * output:    none.
 */
struct requests_queue* init_requests_queue(pthread_mutex_t* p_mutex, pthread_cond_t*  p_cond_var)
{
    struct requests_queue* queue =
		(struct requests_queue*)malloc(sizeof(struct requests_queue));
    if (!queue) {
	fprintf(stderr, "out of memory. exiting\n");
	exit(1);
    }
    /* initialize queue */
    queue->requests = NULL;
    queue->last_request = NULL;
    queue->num_requests = 0;
    queue->p_mutex = p_mutex;
    queue->p_cond_var = p_cond_var;

    return queue;
}


/*
 * function add_request(): add a request to the requests list
 * algorithm: creates a request structure, adds to the list, and
 *            increases number of pending requests by one.
 * input:     pointer to queue, request number.
 * output:    none.
 */
void add_request(struct requests_queue* queue/*, int request_num*/,struct request *a_request)
{
    int rc;	                    /* return code of pthreads functions.  */
//  struct request* a_request;      /* pointer to newly added request.     */
	
    /* sanity check - amke sure queue is not NULL */
     assert(queue);
	
    /* create structure with new request */
//    a_request = (struct request*)malloc(sizeof(struct request));
//    if (!a_request)
//	{ /* malloc failed?? */
//		fprintf(stderr, "add_request: out of memory\n");
//		exit(1);
//    }
//    a_request->number = request_num;

	//Make sure the new request->next=NULL;
    a_request->next = NULL;
	
    /* lock the mutex, to assure exclusive access to the list */
    rc = pthread_mutex_lock(queue->p_mutex);
	
    /* add new request to the end of the list, updating list */
    /* pointers as required */
    if (queue->num_requests == 0)
	{ /* special case - list is empty */
		queue->requests = a_request;
		queue->last_request = a_request;
    }
    else
	{
		queue->last_request->next = a_request;
		queue->last_request = a_request;
    }
	
    /* increase total number of pending requests by one. */
    queue->num_requests++;
	
#ifdef DEBUG
    printf("add_request: added request with id '%d'\n", a_request->number);
    fflush(stdout);
#endif /* DEBUG */
	
    /* unlock mutex */
    rc = pthread_mutex_unlock(queue->p_mutex);
	
    /* signal the condition variable - there's a new request to handle */
    rc = pthread_cond_signal(queue->p_cond_var);
}

int BT_CancelRequest(int request_num)
{
	int rc;
	int result;
	struct request *p;
	rc = pthread_mutex_lock(bt_requests->p_mutex);
	p=bt_requests->requests;
	while(p && p->number != request_num)
		p=p->next;
	if(!p)
	{
		rc=pthread_mutex_unlock(bt_requests->p_mutex);
		return FALSE;
	}
	else if(p->state==RQ_FINISHED)
	{
		rc=pthread_mutex_unlock(bt_requests->p_mutex);
		return FALSE;
	}
	else
	{
		p->state=RQ_CANCEL;
		
		result=TRUE;
		switch(p->request_type) {
		case RT_BT_SEARCHDEVICE:
			printf("================stop search\r\n");
			StopDiscoverDevice();
			break;
			
		case RT_BT_PAIR:
			break;
			
		case RT_BT_CONNECT_FTP:
			break;
			
		case RT_BT_CONNECT_OPP:
			break;
			
		case RT_BT_CONNECT_HEADSET:
			break;

		case RT_BT_FILETRANSFER:
			StopBtFileTransfer(((PBTCONNFILENAME)p->request_param)->conn_hdl);
			break;
			
		case RT_BT_OBJECTTRANSFER:
			StopBtFileTransfer(((PBTCONNFILENAME)p->request_param)->conn_hdl);
			break;
			
		default:
			result=FALSE;
			break;
		}
		
		rc=pthread_mutex_unlock(bt_requests->p_mutex);
	}
	
	return result;
	
}

int BT_SendRequest(HWND hWnd,int message,int request_type,void *param,void *result_buffer,int result_buffer_len)
{
	static int request_num=0;
	struct request *a_request;
	unsigned int result;

	a_request = (struct request*)malloc(sizeof(struct request));
    if (!a_request)
	{ /* malloc failed?? */
		fprintf(stderr, "add_request: out of memory\n");
		return -1;
    }
    
	a_request->request_type=request_type;
	a_request->hWnd=hWnd;
	a_request->message=message;
	a_request->state=RQ_UNFINISHED;

	switch(request_type) {
	case RT_BT_SWITCHON:
        a_request->request_param=NULL;
		break;
	case RT_BT_SWITCHOFF:
		a_request->request_param=NULL;
		break;
	case RT_BT_SEARCHDEVICE:
		 a_request->request_param=NULL;
		 break;
    case RT_BT_GETREMOTEDEVINFO:
		a_request->request_param=malloc(sizeof(BTDEVHDL));
        memcpy(a_request->request_param,param,sizeof(BTDEVHDL));
		break;
	case RT_BT_UPDATEREMOTEDEVINFO:
        a_request->request_param=NULL;
		break;
	case RT_BT_PAIR:
		a_request->request_param=malloc(sizeof(BTDEVHDL));
		memcpy(a_request->request_param,param,sizeof(BTDEVHDL));
		break;
	case RT_BT_CONNECT_FTP:
	case RT_BT_CONNECT_OPP:
	case RT_BT_CONNECT_HEADSET:
        a_request->request_param=malloc(sizeof(BTDEVHDL));
		memcpy(a_request->request_param,param,sizeof(BTDEVHDL));
		break;		
	case RT_BT_FILETRANSFER:
		a_request->request_param=malloc(sizeof(BTCONNFILENAME));
        memcpy(a_request->request_param,param,sizeof(BTCONNFILENAME));
		break;
	case RT_BT_MULPICFILETRANSFER:
		a_request->request_param=malloc(sizeof(BTCONNLIST));
        memcpy(a_request->request_param,param,sizeof(BTCONNLIST));
		break;
	case RT_BT_OBJECTTRANSFER:
        a_request->request_param=malloc(sizeof(BTCONNFILENAME));
        memcpy(a_request->request_param,param,sizeof(BTCONNFILENAME));
		break;
	case RT_BT_COPYRING:
		a_request->request_param=malloc(strlen((char *)param)+1);
		strcpy(a_request->request_param,param);
		break;
	default:
		break;
	}
	
	//Here operate to static varible,so need a mutex lock to resove re-enter issue
	pthread_mutex_lock(&request_num_mutex);
	a_request->number = result = request_num;
	request_num++;
	if(request_num>=0x0FFFFFF0)
		request_num=0;
	pthread_mutex_unlock(&request_num_mutex);

	add_request(bt_requests,a_request);
	
	return result;
}
/*
 * function get_request(): gets the first pending request from the requests list
 *                         removing it from the list.
 * algorithm: creates a request structure, adds to the list, and
 *            increases number of pending requests by one.
 * input:     pointer to requests queue.
 * output:    pointer to the removed request, or NULL if none.
 * memory:    the returned request need to be freed by the caller.
 */
struct request* get_request(struct requests_queue* queue)
{
    int rc;	                    /* return code of pthreads functions.  */
    struct request* a_request;      /* pointer to request.                 */
	
    /* sanity check - amke sure queue is not NULL */
    assert(queue);
	
    /* lock the mutex, to assure exclusive access to the list */
    rc = pthread_mutex_lock(queue->p_mutex);
	
    if (queue->num_requests > 0)
	{
		a_request = queue->requests;
// 		queue->requests = a_request->next;
//		if (queue->requests == NULL)
//		{ /* this was last request on the list */
//			queue->last_request = NULL;
//		}
//		/* decrease the total number of pending requests */
//		queue->num_requests--;
    }
    else 
	{ /* requests list is empty */
		a_request = NULL;
    }
	
    /* unlock mutex */
    rc = pthread_mutex_unlock(queue->p_mutex);
	
    /* return the request to the caller. */
    return a_request;
}

/*
 * function get_requests_number(): get the number of requests in the list.
 * input:     pointer to requests queue.
 * output:    number of pending requests on the queue.
 */
int get_requests_number(struct requests_queue* queue)
{
    int rc;	                    /* return code of pthreads functions.  */
    int num_requests;		    /* temporary, for result.              */

    /* sanity check */ 
    assert(queue);

    /* lock the mutex, to assure exclusive access to the list */
    rc = pthread_mutex_lock(queue->p_mutex);

    num_requests = queue->num_requests;

    /* unlock mutex */
    rc = pthread_mutex_unlock(queue->p_mutex);

    return num_requests;
}

/*
 * function delete_requests_queue(): delete a requests queue.
 * algorithm: delete a request queue structure, and free all memory it uses.
 * input:     pointer to requests queue.
 * output:    none.
 */
void delete_requests_queue(struct requests_queue* queue)
{
    struct request* a_request;      /* pointer to a request.               */
	
    /* sanity check - make sure queue is not NULL */
    assert(queue);
	
    /* first free any requests that might be on the queue */
    while (queue->num_requests > 0)
	{
		a_request = get_request(queue);
		free(a_request);
    }
	
    /* finally, free the queue's struct itself */
    free(queue);
}

/*
 * function handle_request(): handle a single given request.
 * algorithm: prints a message stating that the given thread handled
 *            the given request.
 * input:     request pointer, id of calling thread.
 * output:    none.
 */
static void handle_request(struct request* a_request)
{
	int rc;
    if (a_request) 
	{

#ifdef DEBUG
		printf("handled request '%d'\n",a_request->number);
		fflush(stdout);
#endif
		rc = pthread_mutex_lock(bt_requests->p_mutex);
		if(a_request->state==RQ_CANCEL)
		{
			rc = pthread_mutex_unlock(bt_requests->p_mutex);
			return;
		}
		rc = pthread_mutex_unlock(bt_requests->p_mutex);

		switch(a_request->request_type) {
		case RT_BT_SWITCHON:
			{
#ifndef _EMULATE_
				SwitchOnBlueTooth();
#else
				Sleep(3*1000);
#endif
				a_request->state=RQ_FINISHED;
				PostMessage(a_request->hWnd,a_request->message,0,0);
			}
			break;
		case RT_BT_SWITCHOFF:
			{
#ifndef _EMULATE_
				SwitchOffBlueTooth();
#else
				Sleep(3*1000);			
#endif
				a_request->state=RQ_FINISHED;
				PostMessage(a_request->hWnd,a_request->message,0,0);
			}
			break;

		case RT_BT_SEARCHDEVICE:
			{
				BOOL result;
// 				BTUINT32 dev_cls;
				
				result=StartSearchDevice(0,a_request->hWnd);

				rc = pthread_mutex_lock(bt_requests->p_mutex);
				if(a_request->state==RQ_CANCEL)
				{
					rc = pthread_mutex_unlock(bt_requests->p_mutex);
					return;
				}
				a_request->state=RQ_FINISHED;
				rc = pthread_mutex_unlock(bt_requests->p_mutex);
                
//              PostMessage(a_request->hWnd,a_request->message,(WPARAM)result,0);			
			}
			break;
		case RT_BT_GETREMOTEDEVINFO:
			{
				BTDEVHDL dev_hdl;

				dev_hdl=*(BTDEVHDL*)(a_request->request_param);
				
				GetRemoteDevicesInfo(dev_hdl);
			}
			break;
		case RT_BT_UPDATEREMOTEDEVINFO:
			{               
                UpdateRemoteDevices(a_request->hWnd);
			}
			 break;
			
		case RT_BT_PAIR:
			{
				BTUINT32 result;
				BTDEVHDL dev_hdl;
				dev_hdl=*(BTDEVHDL*)(a_request->request_param);

				result=Btsdk_PairDevice(dev_hdl);

				rc = pthread_mutex_lock(bt_requests->p_mutex);
				if(a_request->state==RQ_CANCEL)
				{
					rc = pthread_mutex_unlock(bt_requests->p_mutex);
					return;
				}
				a_request->state=RQ_FINISHED;
				rc = pthread_mutex_unlock(bt_requests->p_mutex);

				PostMessage(a_request->hWnd,a_request->message,(WPARAM)result,0);
			}
			break;

		case RT_BT_CONNECT_FTP:
			{
			     BTDEVHDL dev_hdl;
				 int i;
				 BTINT32 result1,result2;
				 BtSdkRemoteServiceAttrStru rmt_svc;
				 dev_hdl=*(BTDEVHDL*)(a_request->request_param);
				 
				 g_rmt_svc_num = 32;
				 rmt_svc.mask = BTSDK_RSAM_SERVICENAME|BTSDK_RSAM_EXTATTRIBUTES;
				 memset(rmt_svc.svc_name, 0, BTSDK_SERVICENAME_MAXLENGTH);
				 
				 result1= Btsdk_BrowseRemoteServices(dev_hdl, g_rmt_svc_hdls, &g_rmt_svc_num);
				 if (result1 == BTSDK_OK)
				 {
					 for (i = 0; i < g_rmt_svc_num; i++)
					 {
						 Btsdk_GetRemoteServiceAttributes(g_rmt_svc_hdls[i], &rmt_svc);
						 if(rmt_svc.svc_class==BTSDK_CLS_OBEX_FILE_TRANS)
						 {
							 g_sel_rmt_svc=g_rmt_svc_hdls[i];
							 break;
						 }
					 }
					 
					 result2=Btsdk_StartClient(g_sel_rmt_svc, 0, &g_cur_conn_hdl);
					 
				 }

				 rc = pthread_mutex_lock(bt_requests->p_mutex);
				 if(a_request->state==RQ_CANCEL)
				 {
					 rc = pthread_mutex_unlock(bt_requests->p_mutex);
					 return;
				 }
				 a_request->state=RQ_FINISHED;
				 rc = pthread_mutex_unlock(bt_requests->p_mutex);

                 PostMessage(a_request->hWnd,a_request->message,(WPARAM)result2,(LPARAM)g_cur_conn_hdl);
			}
			break;
			
        case RT_BT_CONNECT_OPP:
			{
		        BTDEVHDL dev_hdl;
				int i;
				BTINT32 result1,result2;
				BtSdkRemoteServiceAttrStru rmt_svc;
				dev_hdl=*(BTDEVHDL*)(a_request->request_param);
				
				g_rmt_svc_num = 32;
				rmt_svc.mask = BTSDK_RSAM_SERVICENAME|BTSDK_RSAM_EXTATTRIBUTES;
				memset(rmt_svc.svc_name, 0, BTSDK_SERVICENAME_MAXLENGTH);
				
				result1= Btsdk_BrowseRemoteServices(dev_hdl, g_rmt_svc_hdls, &g_rmt_svc_num);
				if (result1 == BTSDK_OK)
				{
					for (i = 0; i < g_rmt_svc_num; i++)
					{
						Btsdk_GetRemoteServiceAttributes(g_rmt_svc_hdls[i], &rmt_svc);
						if(rmt_svc.svc_class==BTSDK_CLS_OBEX_OBJ_PUSH)
						{
							g_sel_rmt_svc=g_rmt_svc_hdls[i];
							break;
						}
					}
					
					result2=Btsdk_StartClient(g_sel_rmt_svc, 0, &g_cur_conn_hdl);
					
				}
				
				rc = pthread_mutex_lock(bt_requests->p_mutex);
				if(a_request->state==RQ_CANCEL)
				{
					rc = pthread_mutex_unlock(bt_requests->p_mutex);
					return;
				}
				a_request->state=RQ_FINISHED;
				rc = pthread_mutex_unlock(bt_requests->p_mutex);
				
				PostMessage(a_request->hWnd,a_request->message,(WPARAM)result2,(LPARAM)g_cur_conn_hdl);
			}
			break;
		case RT_BT_CONNECT_HEADSET:
			{
				BTDEVHDL dev_hdl;
				BTCONNHDL pconn_hdl;
				BTINT32 result;

				dev_hdl=*(BTDEVHDL*)(a_request->request_param);
                
				result=Btsdk_StartClientEx(dev_hdl,BTSDK_CLS_HEADSET,0,&pconn_hdl);

				printf("\n$$$$$$$$$$$In RT_BT_CONNECT_HEADSET pconn_hdl=%08x$$$$$$$$$$$$\n",pconn_hdl);
				
				rc = pthread_mutex_lock(bt_requests->p_mutex);
				if(a_request->state==RQ_CANCEL)
				{
					rc = pthread_mutex_unlock(bt_requests->p_mutex);
					return;
				}
				a_request->state=RQ_FINISHED;
				rc = pthread_mutex_unlock(bt_requests->p_mutex);

				PostMessage(a_request->hWnd,a_request->message,(WPARAM)result,(LPARAM)pconn_hdl);				
			}
			break;

		case RT_BT_FILETRANSFER:
			{
				BTINT32 result;
				BTCONNFILENAME BtConnFileName; 
				char szFileNameNoPath[64];
				
                memcpy(&BtConnFileName,a_request->request_param,sizeof(BTCONNFILENAME));

				if(BtConnFileName.iFiletype==BTPICTURE)
				{
				   GetFileNameNoPath(BtConnFileName.szFileName,szFileNameNoPath);
				   result=Btsdk_FTPPutFile(BtConnFileName.conn_hdl,BtConnFileName.szFileName, szFileNameNoPath);
				}
				else
                   result=Btsdk_FTPPutFile(BtConnFileName.conn_hdl,BtConnFileName.szFileName, BtConnFileName.szViewFileName);
                            
		        DisConnection(BtConnFileName.conn_hdl);

				rc = pthread_mutex_lock(bt_requests->p_mutex);
				if(a_request->state==RQ_CANCEL)
				{
					rc = pthread_mutex_unlock(bt_requests->p_mutex);
					return;
				}
				a_request->state=RQ_FINISHED;
				rc = pthread_mutex_unlock(bt_requests->p_mutex);
				
				PostMessage(a_request->hWnd,a_request->message,(WPARAM)result,0);
			}
			break;
		case RT_BT_MULPICFILETRANSFER:
			{
				BTINT32 result;
				BTCONNLIST  BtConnList;

				memcpy(&BtConnList,a_request->request_param,sizeof(BTCONNLIST));

				while(((PLISTATTNODE)(BtConnList.pListHead))->pNext!=NULL)
				{
					result=Btsdk_FTPPutFile(BtConnList.conn_hdl,
						((PLISTATTNODE)BtConnList.pListHead)->AttPath, 
						((PLISTATTNODE)BtConnList.pListHead)->AttName);

					if(result!=BTSDK_OK)
						break;
				}

				DisConnection(BtConnList.conn_hdl);
				
				rc = pthread_mutex_lock(bt_requests->p_mutex);
				if(a_request->state==RQ_CANCEL)
				{
					rc = pthread_mutex_unlock(bt_requests->p_mutex);
					return;
				}
				a_request->state=RQ_FINISHED;
				rc = pthread_mutex_unlock(bt_requests->p_mutex);
				
				PostMessage(a_request->hWnd,a_request->message,(WPARAM)result,0);
				
			}
			break;
		case RT_BT_OBJECTTRANSFER:
			{
				BTINT32 result;
				BTCONNFILENAME BtConnFileName; 
				
                memcpy(&BtConnFileName,a_request->request_param,sizeof(BTCONNFILENAME));

				result=Btsdk_OPPPushObj(BtConnFileName.conn_hdl,BtConnFileName.szFileName);

				DisConnection(BtConnFileName.conn_hdl);

                rc = pthread_mutex_lock(bt_requests->p_mutex);
				if(a_request->state==RQ_CANCEL)
				{
					rc = pthread_mutex_unlock(bt_requests->p_mutex);
					return;
				}
				a_request->state=RQ_FINISHED;
				rc = pthread_mutex_unlock(bt_requests->p_mutex);
				
				PostMessage(a_request->hWnd,a_request->message,(WPARAM)result,0);

			}
			break;
		case RT_BT_COPYRING:
			{
				Btsdk_AGAP_NetworkEvent(AGAP_NETWORK_INCOMING_CALL,(char*)a_request->request_param);
				
			}
			break;

		default:
			break;
		}
		
    }
}

/*
 * function handle_requests_loop(): infinite loop of requests handling
 * algorithm: forever, if there are requests to handle, take the first
 *            and handle it. Then wait on the given condition variable,
 *            and when it is signaled, re-do the loop.
 *            increases number of pending requests by one.
 * input:     id of thread, for printing purposes.
 * output:    none.
 */
static void handle_requests_loop(void)
{
    int rc;	                    /* return code of pthreads functions.  */
    struct request* a_request;      /* pointer to a request.               */

    printf("Starting thread \r\n");
    fflush(stdout);


    /* lock the mutex, to access the requests list exclusively. */
//  	   rc=pthread_mutex_lock(&request_mutex);
	   
#ifdef DEBUG
	   printf("after pthread_mutex_lock\n");
	   fflush(stdout);
#endif /* DEBUG */

    /* do forever.... */
    while (1) 
	{
// 		int num_requests = get_requests_number(data->requests);
		int num_requests = get_requests_number(bt_requests);
		
#ifdef DEBUG
		printf("num_requests =  %d\n", num_requests);
		fflush(stdout);
#endif /* DEBUG */
		if (num_requests > 0)
		{ /* a request is pending */
			a_request = get_request(bt_requests);
			if (a_request)
			{ /* got a request - handle it and free it */
				/* unlock mutex - so other threads would be able to handle */
				/* other reqeusts waiting in the queue paralelly.          */
// 				rc = pthread_mutex_unlock( &request_mutex);
				handle_request(a_request);

				//since we handle the node,so we can delete from queue,and free it.
				rc = pthread_mutex_lock( &request_mutex);
				bt_requests->requests = a_request->next;
				if (bt_requests->requests == NULL)
				{ /* this was last request on the list */
					bt_requests->last_request = NULL;
				}
				/* decrease the total number of pending requests */
				bt_requests->num_requests--;
				rc = pthread_mutex_unlock(&request_mutex);

				free(a_request->request_param);
				free(a_request);
				/* and lock the mutex again. */
// 				rc = pthread_mutex_lock(&request_mutex);
			}
		}
		else
		{
			/* the thread checks the flag before waiting            */
			/* on the condition variable.                           */
			/* if no new requests are going to be generated, exit.  */
			if (done_creating_requests)
			{
//              pthread_mutex_unlock(&request_mutex);
				printf("thread exiting\r\n");
				free(bt_requests);
				fflush(stdout);
				pthread_exit(NULL);
			}
			/* wait for a request to arrive. note the mutex will be */
			/* unlocked here, thus allowing other threads access to */
			/* requests list.                                       */
#ifdef DEBUG
			printf("before pthread_cond_wait\r\n");
			fflush(stdout);
#endif /* DEBUG */
			
			rc=pthread_mutex_lock(&request_mutex);
			rc = pthread_cond_wait(&got_request, &request_mutex);
			rc=pthread_mutex_unlock(&request_mutex);
			/* and after we return from pthread_cond_wait, the mutex  */
			/* is locked again, so we don't need to lock it ourselves */
#ifdef DEBUG
			printf("after pthread_cond_wait\r\n");
			fflush(stdout);
#endif /* DEBUG */
		}

    } //while
}
void InitBtThread(void)
{
	bt_requests = NULL;  
	pthread_mutex_init(&request_mutex,NULL);
	pthread_cond_init(&got_request,NULL);
	pthread_mutex_init(&request_num_mutex,NULL);

	pthread_mutex_init(&BtRecvData_mutex,NULL);
	pthread_cond_init(&BtRecvData_cond,NULL);
	
//	pthread_mutex_init(&BtStopSearch_mutex,NULL);
	pthread_mutex_init(&BtDeviceNodeListMutex,NULL);
	
	/* create the requests queue */
    bt_requests = init_requests_queue(&request_mutex, &got_request);
    assert(bt_requests);
	
	g_tid=pthread_create(&g_pthread,NULL,(void*)handle_requests_loop,NULL);
}
