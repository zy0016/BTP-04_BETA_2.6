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
#include "string.h"

#include <fcntl.h>
#include <sys/poll.h>
#include <unistd.h>
#include "hopen/comm.h"
#include "stdio.h"
#include "plx_pdaex.h"

#include "window.h"
#include "me_func.h"
#include "me_wnd.h"
#include "me_comn.h"
#include "me_at.h"


static struct ME_REQUEST_WIN * FindRequestNode (int idx);
static struct ME_REQUEST_WIN * GetRequestNode (void);
static int gInit;
struct ME_REQUEST_WIN	request_buf[MAX_QUEUE_REQUESTS];
struct ME_REQUEST_WIN * gUssdObj;
extern struct ME_INDTABLE IndTable;

extern unsigned int volatile nTickCount;

int check_request_queue()
{
#ifdef _ME_DEBUG_DETAILS
	int i, n;
	MsgOut("\r\n[ME]: me request queue :\r\n");
	for(i=0, n=0; i<MAX_QUEUE_REQUESTS; i++)
	{
		if(request_buf[i].used_flag)
		{
			MsgOut("0x%08x = %03d  ", &(request_buf[i].request), request_buf[i].request.cmd);
			n ++;
		}
		else
			MsgOut("0x%08x = [_]  ", &(request_buf[i].request));
		if((i+1)%4 == 0)
			MsgOut("\r\n");
	}
	MsgOut("\r\n");
	return n;
#else
	return 1;
#endif
}

static struct ME_REQUEST_WIN * GetRequestNode (void)
{
    int i;

    for ( i = 0; i < MAX_QUEUE_REQUESTS; i ++ )
    {
        if ( request_buf[i].used_flag == 0 ) {
            f_sleep_register(ME_WAITRESP);
            memset (&request_buf[i], 0, sizeof(struct ME_REQUEST_WIN));
            request_buf[i].request.index = -1;
            request_buf[i].used_flag = 1;
            return &request_buf[i];
        }
    }
	MsgOut("\r\n[ME]: me request queue is full now !!!!!!\r\n");
	for(i=0; i<MAX_QUEUE_REQUESTS; i++)
	{
		MsgOut("0x%08x = %03d  ", &(request_buf[i].request), request_buf[i].request.cmd);
		if((i+1)%4 == 0)
			MsgOut("\r\n");
	}
	MsgOut("\r\n");
	
    return NULL;
}

static struct ME_REQUEST_WIN * FindRequestNode (int idx)
{
    int i;

    if ( idx < 0 )
        return NULL;
    
    for ( i = 0; i < MAX_QUEUE_REQUESTS; i ++ ) {
        if ( request_buf[i].used_flag == 0 )
            continue;

        if ( request_buf[i].request.index == idx )
            return &request_buf[i];
    }

    return NULL;
}

void ME_WndCallback (MEREQUEST * pRequest)
{
    struct ME_REQUEST_WIN * request = FindRequestNode (pRequest->index);

	if ( request == NULL)
	{
		ShowDetail("\r\n[ME]: replying, can't find the request node!\r\n");
		return;
	}

    SendMessage (request->hWnd, request->message, (WORD)pRequest->error, (DWORD)pRequest->errcode);

    /* Free this request structure */
    request->used_flag = 0;
	ShowDetail("\r\n[ME]: request node released, cmd = %d, request = 0x%08x\r\n", request->request.cmd, &(request->request));
	check_request_queue();
}

void ME_UssdCallback (MEREQUEST * pRequest)
{
    if ( gUssdObj == NULL )
        return;

    SendMessage (gUssdObj->hWnd, gUssdObj->message, (DWORD)gUssdObj->request.error, (DWORD)gUssdObj);
}

void ME_URCCallback (struct ME_REGISTER * pRegister, unsigned long param1, unsigned long param2)
{
    SendMessage (pRegister->hwnd, pRegister->msg, param1, param2);
}

int ME_Initialize (int Mode)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;
    
    if ( gInit )
        return gInit;

	nTickCount = 0;

    request = GetRequestNode ();

    if ( request == NULL )
        return -1;

	request->request.cmd = ME_INIT;

	request->request.arg1 = Mode;

	retval = ME_Request( &request->request );

	request->used_flag = 0;

    request = GetRequestNode ();
    retval = USSD_Init (&(request->request));

    if ( retval < 0 )
        return retval;

    request->request.reply = ME_UssdCallback;
    gUssdObj = request;

    gInit = TRUE;

	return retval;
}

int ME_GSMInit_Module_3( HWND hWnd, UINT Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_INIT_EVERYTIME;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GSMInit_Module( HWND hWnd, UINT Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_INIT_EQUIP;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GSMInit_Module_2( HWND hWnd, UINT Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_RESET_EQUIP;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_InitRate(HWND hWnd, UINT Msg )
{
    int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_INIT_INITRATE;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_InitSTK(HWND hWnd, UINT Msg )
{
    int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_INIT_INITSTK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_RegisterIndication( int MsgClass, int Mode, HWND hwnd, UINT msg )
{
	int ret, id1, id2;

	if ( hwnd == NULL )
		return -1;

	ret = ME_RegisterInd (MsgClass, Mode, ME_URCCallback);

	if ( ret < 0 )
		return -1;

	id1 = (int)ret/100;
	id2 = (int)ret%100;

	IndTable.Inds[id1].regs[id2].hwnd = hwnd;
	IndTable.Inds[id1].regs[id2].msg = msg;

	return ret;
}

BOOL ME_UnRegisterIndication (int Handle)
{
	int retval;

	retval = ME_UnRegisterInd (Handle);

	if ( retval < 0 )
		return FALSE;

	return TRUE;
}

int ME_CancelFunction (int Handle)
{
	struct ME_REQUEST_WIN * request;
	int retval;

	if ( Handle < 0 )
		return -1;

	request = FindRequestNode (Handle);

	if ( request == NULL )
		return -1;

	retval = ME_CancelRequest (&(request->request));

	if ( retval < 0 ) {
		PostMessage (request->hWnd, request->message, ME_RS_USERSTOP, 0);
		request->used_flag = 0;
	}

	return 0;
}

int ME_GetResult (void* result, int result_len)
{
	return ME_GetCurrentResult (result, result_len);
}

int ME_VoiceDial (HWND hWnd, int Msg, char* PhoneNum, char* Extention)
{
	int retval = 0;
	int	len;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	len = strlen (PhoneNum);
	strcpy (request->cmddata, PhoneNum);
	if ( Extention != NULL )
	{
		memcpy (&request->cmddata[len+1], Extention, strlen(Extention));
		request->request.cmddata_len = len + strlen(Extention) + 1;
	}
	else
		request->request.cmddata_len = len;

    /* Fill the request */
	request->request.cmd = ME_VOICE_DIAL;
	request->request.cmddata = request->cmddata;
    request->request.reply = ME_WndCallback;

	retval = ME_Request (&(request->request));

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}
int ME_DataDial (HWND hWnd, int Msg, char* PhoneNum, char* Extention)
{
	int retval = 0;
	int	len;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	len = strlen (PhoneNum);
	strcpy (request->cmddata, PhoneNum);
	if ( Extention != NULL )
	{
		memcpy (&request->cmddata[len+1], Extention, strlen(Extention));
		request->request.cmddata_len = len + strlen(Extention) + 1;
	}
	else
		request->request.cmddata_len = len;

    /* Fill the request */
	request->request.cmd = ME_VOICE_DATADIAL;
	request->request.cmddata = request->cmddata;
    request->request.reply = ME_WndCallback;

	retval = ME_Request (&(request->request));

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}
int ME_GetSALSstatus( HWND hWnd, int Msg)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;
	
	request->hWnd = hWnd;
	request->message = Msg;

    	/* Fill the request */
	request->request.cmd = ME_GET_SALSstatus;
	request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SelectOutLine( HWND hWnd, int Msg, int LineNum, int Indication )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;
	
	request->hWnd = hWnd;
	request->message = Msg;

    	/* Fill the request */
	request->request.arg1 = LineNum;
	request->request.arg2 = Indication;
	request->request.cmd = ME_SET_OUTLINE;
	request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}
int ME_VoiceHangup( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_VOICE_HANGUP;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_CallAnswer( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_VOICE_ANSWER;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_ListCurCalls( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_CURRENT_CALLS;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SwitchCallStatus( HWND hWnd, int Msg, int Style, int Index )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Style;
	request->request.arg2 = Index;
	request->request.cmd = ME_SWITCH_CALLLINE;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetAutoAnswerStatus( HWND hWnd, int Msg, int Times )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Times;

	request->request.cmd = ME_SET_AUTOANSWER;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetAutoAnswerStatus( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_AUTOANSWER;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetCallWaitingStatus( HWND hWnd, int Msg, BOOL bEnable, int Class )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = bEnable;
	request->request.arg2 = Class;
	request->request.cmd = ME_SET_CALLWAITING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetCallWaitingStatus( HWND hWnd, int Msg, int Class )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Class;
	request->request.cmd = ME_GET_CALLWAITING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetCallForwardStatus( HWND hWnd, int Msg, PCALL_FWD pCallFwd )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;;

	if ( pCallFwd == NULL )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	memcpy( request->cmddata, pCallFwd, sizeof(CALL_FWD));

    /* Fill the request */
	request->request.cmd = ME_SET_CALLFWDING;

	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof(CALL_FWD);
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetCallForwardStatus( HWND hWnd, int Msg, int Style, int Class )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Style;
	request->request.arg2 = Class;

	request->request.cmd = ME_GET_CALLFWDING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetCallBarringStatus( HWND hWnd, int Msg, int Style,BOOL bEnable, char* Password, int Class )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	
	if ( Password != NULL )
	{
		strcpy( request->cmddata, Password );
		request->request.cmddata = request->cmddata;
		request->request.cmddata_len = strlen( Password );
	}
    else
        request->request.cmddata = NULL;

    /* Fill the request */
	request->request.arg1 = Style;
	request->request.arg2 = bEnable;
	request->request.arg3 = Class;

	request->request.cmd = ME_SET_CALLBARRING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetCallBarringStatus (HWND hWnd, int Msg, int Style, int Class, char* Password)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	
	request->request.cmddata = NULL;

    /* Fill the request */
	request->request.arg1 = Style;
	request->request.arg2 = Class;

	request->request.cmd = ME_GET_CALLBARRING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_Send( HWND hWnd, int Msg, PSMS_SEND pSend )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( pSend == NULL )
		return -1;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	memcpy( request->cmddata, pSend, sizeof(SMS_SEND) );
	
    /* Fill the request */
    request->request.arg1 = 0;
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof(SMS_SEND);

	request->request.cmd = ME_SMS_SEND;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_Read( HWND hWnd, int Msg, BOOL bPreRead, int MemType, int Index )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	
    /* Fill the request */
	request->request.arg1 = bPreRead;
	request->request.arg2 = MemType;
	request->request.arg3 = Index;

	request->request.cmd = ME_SMS_READ;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_Delete( HWND hWnd, int Msg, int MemType, int Index )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	
    /* Fill the request */
	request->request.arg1 = MemType;
	request->request.arg2 = Index;

	request->request.cmd = ME_SMS_DELETE;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_List( HWND hWnd, int Msg, int Type, BOOL bPreRead, int MemType )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = bPreRead;
	request->request.arg2 = MemType;
	request->request.arg3 = Type;

	request->request.cmd = ME_SMS_LIST;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_Write( HWND hWnd, int Msg, PSMS_SEND pSend, int Location, int StoreStat)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( pSend == NULL )
		return -1;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

	if ( StoreStat > SMS_STO_DEFAULT )
		return -1;
	
    	request->hWnd = hWnd;
    	request->message = Msg;
	memcpy( request->cmddata, pSend, sizeof(SMS_SEND) );
	
    	/* Fill the request */
    	request->request.arg1 = Location;
	request->request.arg2 = StoreStat;	
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof(SMS_SEND);

	request->request.cmd = ME_SMS_WRITE;
    	request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}
int ME_SMS_SetSCA( HWND hWnd, int Msg, char* SCA )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( SCA == NULL )
		return -1;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	strcpy( request->cmddata, SCA );

    /* Fill the request */
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = strlen( SCA );

	request->request.cmd = ME_SMS_SET_SCA;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_GetSCA( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SMS_GET_SCA;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_SetIndStatus( HWND hWnd, int Msg, PIND_STATUS pStatus )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( pStatus == NULL )
		return -1;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	memcpy( request->cmddata, pStatus, sizeof(IND_STATUS) );

    /* Fill the request */
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof(IND_STATUS);

	request->request.cmd = ME_SMS_SETINDSTATUS;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_GetIndStatus( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SMS_GETINDSTATUS;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_SetCellBroadcast( HWND hWnd, int Msg, PCELL_BROADCAST pStatus )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( pStatus == NULL )
		return -1;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	memcpy( request->cmddata, pStatus, sizeof(CELL_BROADCAST) );

    /* Fill the request */
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof(CELL_BROADCAST);

	request->request.cmd = ME_SET_CELLBROADCAST;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_GetCellBroadcast( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_CELLBROADCAST;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_GetMemStatus( HWND hWnd, int Msg, int MemType )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	
    /* Fill the request */
	request->request.arg1 = MemType;

	request->request.cmd = ME_SMS_MEMCOUNT;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_ReadPhonebook(HWND hWnd, int Msg, int Type, int IndexBegin, int IndexEnd)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Type;
	request->request.arg2 = IndexBegin;
	request->request.arg3 = IndexEnd;

	request->request.cmd = ME_PBOOK_READ;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_WritePhonebook( HWND hWnd, int Msg, int type, PPHONEBOOK Record )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( NULL == Record )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;	
	memcpy( request->cmddata, Record, sizeof(PHONEBOOK) );

    /* Fill the request */
    request->request.arg1 = type;
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof( PHONEBOOK );

	request->request.cmd = ME_PBOOK_WRITE;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetPhonebookMemStatus( HWND hWnd, int Msg, int Type )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Type;

	request->request.cmd = ME_PBOOK_MEMCOUNT;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}
//获得联系人可以保存的号码长度,文字长度
int ME_GetPhonebookNumberTextMaxLen(HWND hWnd,int Msg)
{
    int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_PHONEBOOKNUMBERTEXTMAXLEN;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetSIMCardStatus(HWND hWnd,int Msg)//获得sim卡状态
{
    int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GETSIMCARDSTATUS;
    request->request.reply = ME_WndCallback;
	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetPhoneLockStatus( HWND hWnd, int Msg, int Type, char* Password, BOOL bEnable )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	if ( Password != NULL )
	{
		strcpy( request->cmddata, Password );
		request->request.cmddata = request->cmddata;
		request->request.cmddata_len = strlen( Password );
	}
    else {
        request->request.cmddata = NULL;
    }

    /* Fill the request */
	request->request.arg1 = Type;
	request->request.arg2 = bEnable;

	request->request.cmd = ME_SET_LOCK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetPhoneLockStatus (HWND hWnd, int Msg, int Type, char* Password)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
    request->request.cmddata = NULL;

    /* Fill the request */
	request->request.arg1 = Type;

	request->request.cmd = ME_GET_LOCK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_ChangeLockPassword( HWND hWnd, int Msg, int Type, char* OldPass, char* NewPass )
{
	int retval = 0;
	int	len;
	struct ME_REQUEST_WIN * request;

	if ( OldPass == NULL || NewPass == NULL )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	len = strlen( OldPass );
	strcpy( request->cmddata, OldPass );
	memcpy( &request->cmddata[len+1], NewPass, strlen(NewPass) );

    /* Fill the request */
	request->request.cmd = ME_PASS_CHANGE;
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = len + strlen(NewPass) + 1;
	request->request.arg1 = Type;

    request->request.reply = ME_WndCallback;
	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}


int ME_PasswordValidation( HWND hWnd, int Msg, int Type, char* Password, char* NewPass )
{
	int retval = 0, len;
	struct ME_REQUEST_WIN * request;

	if ( Password == NULL )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	len = strlen( Password );
	strcpy( request->cmddata, Password );
	if ( NewPass != NULL )
		memcpy (&request->cmddata[len+1], NewPass, strlen(NewPass));

    /* Fill the request */
	request->request.cmddata = request->cmddata;
	if ( NewPass != NULL )
		request->request.cmddata_len = len + strlen(NewPass) + 1;
	else
		request->request.cmddata_len = len;

	request->request.arg1 = Type;

	request->request.cmd = ME_PASS_VALID;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetCurWaitingPassword( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_PASS;
    request->request.reply = ME_WndCallback;
	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetCurPasswordStatus( HWND hWnd, int Msg)//获得当前sim卡锁状态,主要是pin2码的状态
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_CURPASSWORD_STATUS;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetPassInputLimit( HWND hWnd, int Msg, int Type )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_PASS_REMAIN;
    request->request.reply = ME_WndCallback;
	request->request.arg1 = Type;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetOpratorMode( HWND hWnd, int Msg, int Mode, char* OperId )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	if ( OperId != NULL )
	{
		strcpy( request->cmddata, OperId );
		request->request.cmddata = request->cmddata;
		request->request.cmddata_len = strlen( OperId);
	}
	else
		request->request.cmddata = NULL;

    /* Fill the request */
	request->request.arg1 = Mode;

	request->request.cmd = ME_SET_OPRATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetCurOprator ( HWND hWnd, int Msg, int Format )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Format;

	request->request.cmd = ME_GET_OPRATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SearchNetOprator ( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SRCH_OPRATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_ReadPrefOperList( HWND hWnd, int Msg, int BeginIndex, int EndIndex )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = BeginIndex;
	request->request.arg2 = EndIndex;
	request->request.cmd = ME_GET_POPRATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_WritePrefOperList( HWND hWnd, int Msg, int index, char* oper_id )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;


    /* Fill the request */
	if ( oper_id != NULL )
	{
		strcpy( request->cmddata, oper_id );
		request->request.cmddata = request->cmddata;
		request->request.cmddata_len = strlen(oper_id);
	}
	else
		request->request.cmddata = NULL;

	request->request.arg1 = index;

	request->request.cmd = ME_SET_POPRATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetEquipmentId ( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_IMEI;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetSubscriberId ( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_IMSI;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetBatteryChargeControl(HWND hWnd,int Msg,int iCurrent)
{
    int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = Msg;
	
	request->request.arg1 = iCurrent;
	request->request.cmd = ME_SETBATTERYCHARGECONTROL;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_SetClockTime ( HWND hWnd, int Msg, char* DateTime )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( DateTime == NULL )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	strcpy( request->cmddata, DateTime );

    /* Fill the request */
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = strlen( DateTime );

	request->request.cmd = ME_SET_CLOCK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetClockTime ( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_CLOCK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_MuteControl( HWND hWnd, int Msg, BOOL bEnable )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = bEnable;
	request->request.cmd = ME_MUTE_CONTROL;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_ClearSMSindication( HWND hWnd, int Msg)
{
	int retval = 0;
	//struct ME_REQUEST_WIN * request;

	/*request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	request->request.cmd = ME_CLEARCNMI;
	
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;*/

	PostMessage(hWnd,Msg,ME_RS_SUCCESS,0);
	return retval;	
}

int ME_GetCPHSstatus( HWND hWnd, int Msg)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	request->request.cmd = ME_GET_CPHSstatus;
	
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;	
	
}

int ME_GetCPHSInfo(HWND hWnd,int Msg)//获得restrict sim access全部字符串
{
    int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	request->request.cmd = ME_GET_CPHSInfo;
	
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;	
}

int ME_GetCPHSparameters( HWND hWnd, int Msg, unsigned int CPHS_Field )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
    request->request.arg1 = CPHS_Field;
	request->request.cmd = ME_GET_CPHSparas;
	
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;	
	
}

int ME_GetCPHSparametersExt( HWND hWnd, int Msg, unsigned int CPHS_Field )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
    request->request.arg1 = CPHS_Field;
	request->request.cmd = ME_GET_CPHSparasExt;
	
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;	
	
}

int ME_ReadCPHSFile( HWND hWnd, int Msg, unsigned int CPHS_Field, unsigned int FileSize )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	request->request.arg1 = CPHS_Field;	//
	request->request.arg2 = FileSize;	//
	
	request->request.cmd = ME_READ_CPHSFile;
	
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;	
}

int ME_GetCoNetworkList(HWND hWnd, UINT Msg)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	request->request.arg1 = 0x6f32;	//
	request->request.arg2 = 8*6;	// ?????
	
	request->request.cmd = ME_GET_CONETWORKLIST;
	
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;	
}

int ME_ReadCPHSRecord( HWND hWnd, int Msg, unsigned int CPHS_Field, unsigned int RecSize, unsigned int RecNo )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	request->request.arg1 = CPHS_Field;	//
	request->request.arg2 = RecSize;	//
	request->request.arg3 = RecNo;
	
	request->request.cmd = ME_READ_CPHSRecord;
	
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;	
}

int ME_ReadCPHSRecordSMS( HWND hWnd, int Msg, unsigned int CPHS_Field, unsigned int RecSize, unsigned int RecNo )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

    if (CPHS_Field != 28482)
        return -1;
	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	request->request.arg1 = CPHS_Field;	//
	request->request.arg2 = RecSize;	//
	request->request.arg3 = RecNo;
	
	request->request.cmd = ME_READ_CPHSRecordSMS;
	
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;	
}

int ME_UpdateCPHSFile( HWND hWnd, int Msg, unsigned int CPHS_Field, unsigned char *FileContent, unsigned int FileSize, int Offset )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

	if( FileSize != strlen(FileContent) || FileSize > CMD_DATALEN || FileSize%2 == 1 )
		return -2;
	
    	request->hWnd = hWnd;
    	request->message = Msg;

	memcpy( request->cmddata, FileContent, FileSize );
	
	request->request.arg1 = CPHS_Field;	//
	request->request.arg2 = Offset;	//

	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = FileSize;
	
	request->request.cmd = ME_UPDATE_CPHSFile;
	
    	request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;	
}


int ME_UpdateCPHSRecord( HWND hWnd, int Msg, unsigned int CPHS_Field, unsigned char *RecContent, unsigned int RecSize, unsigned int RecNo )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

	if( RecSize != strlen(RecContent) || RecSize > CMD_DATALEN || RecSize%2 == 1 )
		return -2;
	
    	request->hWnd = hWnd;
    	request->message = Msg;

	memcpy( request->cmddata, RecContent, RecSize );
	
	request->request.arg1 = CPHS_Field;	//
	request->request.arg2 = RecNo;

	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = RecSize;
	
	request->request.cmd = ME_UPDATE_CPHSRecord;
	
    	request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}
void ParseMailboxNumber(unsigned char *SrcData, unsigned int ResultLen, unsigned char *DesData)
{
	unsigned char *NumberLocation = NULL;
	unsigned int i = 0;

	#define DEFINELEN	14
	#define NUMBERLEN	10
	//ResultLen = x + DEFINELEN; Location = x+3 ~ x+12

	NumberLocation = SrcData + ((ResultLen-DEFINELEN) + (3-1))*2;

	for(i=0; i<NUMBERLEN; i++)
	{
		if((DesData[i*2] = *(NumberLocation+(i*2+1))) == 'F')
		{
			DesData[i*2] = '\0';
			break;
		}
		
		if((DesData[i*2+1] = *(NumberLocation+i*2)) == 'F')
		{
			DesData[i*2+1] = '\0';
			break;
		}
	}
}

int ME_GetSMSservice( HWND hWnd, int Msg)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	request->request.cmd = ME_GET_SMSSERVICE;
	
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;	
}

int ME_SetSMSservice( HWND hWnd, int Msg, SMS_Service Service)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	request->request.cmd = ME_SET_SMSSERVICE;
	request->request.arg1 = Service;
		
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;	
}


int ME_GetExIndicator( HWND hWnd, int Msg, Ex_Indicator Indi)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	switch(Indi)
	{
		case Vmwait1:
			request->request.cmd = ME_GET_VMWAIT1;
			break;
		case Vmwait2:
			request->request.cmd = ME_GET_VMWAIT2;
			break;
		case Ciphcall:
			request->request.cmd = ME_GET_CIPH;
			break;			
	}
	
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;	
}

int ME_GetHomeZone( HWND hWnd, int Msg)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	request->request.cmd = ME_GET_HOMEZONE;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetServierProvider( HWND hWnd, int Msg)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	request->request.cmd = ME_GET_EXOPERATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}


int ME_SendDTMF ( HWND hWnd, int Msg, char SendChar )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = SendChar;
	request->request.cmd = ME_SEND_DTMF;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_FactoryDefaultSetting( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_FAC_SETTING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_ResetModule( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_RESET;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SleepModule( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SLEEP;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_PowerOff( HWND hWnd, int Msg )
{
    int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SHUTDOWN;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SwitchAudioInterface(HWND hWnd,int Msg,AudioInterfaceType AudioType)
{
    int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = Msg;
	
	request->request.arg1 = AudioType;
	request->request.cmd = ME_AUDIOINTERFACETYPE;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_ShutdownModule( HWND hWnd, int Msg )
{
	return ME_PowerOff(hWnd,Msg);
}

int ME_GetChargeStatus( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_CHARGE;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetBatteryInfo( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_BATTERY;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetSignalInfo( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_SIGNAL;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetModuleInfo(HWND hWnd, UINT Msg)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_MODULEINFO;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_STK_Send( HWND hwnd, int msg, STK_PARAM* param )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	if ( param == NULL )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmddata = param;
	request->request.cmddata_len = sizeof( STK_PARAM );

	request->request.cmd = ME_STK_SEND;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_STK_GetRemoteSAT( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_STK_GETSAT;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_STK_ReturnIdle( HWND hwnd, int msg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

	request->hWnd = hwnd;
	request->message = msg;

	/* Fill the request */
	request->request.cmd = ME_STK_RETURNIDLE;
	request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;	
}

int ME_SMS_Acknowledge( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_SMS_ACK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetSpeakerVolume( HWND hwnd, int msg, int level )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.arg1 = level;
	request->request.cmd = ME_SET_VOLUME;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetSpeakerVolumeSNFV( HWND hwnd, int msg, int level )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.arg1 = level;
	request->request.cmd = ME_SET_VOLUMESNFV;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}
int ME_GetSpeakerVolumeSNFV( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_GET_VOLUMESNFV;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}
int ME_GetSpeakerVolume( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_GET_VOLUME;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetServCellInfo( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_MONI_SERV;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetNeibCellInfo( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_MONI_NEIB;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetAlarm (HWND hwnd, int msg, ME_ALARMTIME* time, int index)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

	if ( time == NULL )
		return -1;

	memcpy (request->cmddata, time, sizeof(ME_ALARMTIME));

	request->request.arg1 = index;
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof(ME_ALARMTIME);
	request->request.cmd = ME_SETALARM;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetAlarm (HWND hwnd, int msg, int index)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

	request->request.arg1 = index;
	request->request.cmd = ME_GETALARM;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SelectAudioMode (HWND hwnd, int msg, int mode)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hwnd;
    request->message = msg;
	
	request->request.arg1 = mode;
	request->request.cmd = ME_AUDIO;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_SelectAudioModeEx	(HWND hwnd, int msg, int mode)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hwnd;
    request->message = msg;
	
	request->request.arg1 = mode;
	request->request.cmd = ME_AUDIO_EX;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_SpecialDial (HWND hWnd, int nMsg, char * ssCode, BOOL callControl)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	if ( ssCode == NULL )
        return -1;

    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    strcpy (request->cmddata, ssCode);
    request->request.cmddata = request->cmddata;
    request->request.cmddata_len = strlen (ssCode)+1;
    request->request.arg1 = callControl;
    request->request.arg2 = -1;
    request->request.arg3 = 0;
    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_SSCALL;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_GetCCM (HWND hWnd, int nMsg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_GETCCM;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_SetCCM (HWND hWnd, int nMsg, unsigned long mode)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
	request->hWnd = hWnd;
	request->message = nMsg;

	request->request.arg1 = mode;
	request->request.result = NULL;
	request->request.result_len = -1;
	request->request.cmd = ME_SETCCM;
	request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_GetACM (HWND hWnd, int nMsg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_GETACM;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_TestACMmax (HWND hWnd, int nMsg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
	request->hWnd = hWnd;
	request->message = nMsg;

	request->request.result = NULL;
	request->request.result_len = -1;
	request->request.cmd = ME_TESTACMMAX;
	request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_GetACMmax (HWND hWnd, int nMsg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_GETACMAX;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_ResetACM (HWND hWnd, int nMsg, char * Password)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	if ( Password == NULL )
        return -1;

    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    strcpy (request->cmddata, Password);
    request->request.cmddata = request->cmddata;
    request->request.cmddata_len = strlen (Password)+1;
    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_RESETACM;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_SetACMmax (HWND hWnd, int nMsg, unsigned long ACMmax, char * Password)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	if ( Password == NULL )
        return -1;

    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    strcpy (request->cmddata, Password);
    request->request.cmddata = request->cmddata;
    request->request.cmddata_len = strlen (Password)+1;
    request->request.arg1 = ACMmax;
    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_SETACMAX;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_TestSACM (HWND hWnd, int nMsg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
	request->hWnd = hWnd;
	request->message = nMsg;

	request->request.result = NULL;
	request->request.result_len = -1;
	request->request.cmd = ME_TESTSACM;
	request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_GetSACM (HWND hWnd, int nMsg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
	request->hWnd = hWnd;
	request->message = nMsg;

	request->request.result = NULL;
	request->request.result_len = -1;
	request->request.cmd = ME_GETSACM;
	request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_SetSACM (HWND hWnd, int nMsg, unsigned long mode)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
	request->hWnd = hWnd;
	request->message = nMsg;

	request->request.arg1 = mode;
	request->request.result = NULL;
	request->request.result_len = -1;
	request->request.cmd = ME_SETSACM;
	request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_GetPricePerUnit (HWND hWnd, int nMsg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_GETPUC;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_GetGPRSAttachSta (HWND hWnd, int nMsg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

	request->hWnd = hWnd;
	request->message = nMsg;

	request->request.result = NULL;
	request->request.result_len = -1;
	request->request.cmd = ME_GETGPRSAttachSta;
	request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetGPRSAttach (HWND hWnd, int nMsg, int AttachSta)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

	request->hWnd = hWnd;
	request->message = nMsg;
	

	request->request.result = NULL;
	request->request.result_len = -1;
	request->request.arg1 = AttachSta;
	request->request.cmd = ME_SETGPRSAttach;
	request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetPricePerUnit (HWND hWnd, int nMsg, PME_PUCINFO Puc, char * Password)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	if ( Password == NULL || Puc == NULL )
        return -1;

    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

//	memcpy (request->cmddata, Puc, sizeof(Puc));
//	strcpy (&(request->cmddata[sizeof(Puc)]), Password);
	memcpy (request->cmddata, Puc, sizeof(ME_PUCINFO));
	strcpy (&(request->cmddata[sizeof(ME_PUCINFO)]), Password);
	request->request.cmddata = request->cmddata;
//	request->request.cmddata_len = sizeof(Puc) + strlen (Password)+1;
	request->request.cmddata_len = sizeof(ME_PUCINFO) + strlen (Password)+1;
	request->request.result = NULL;
	request->request.result_len = -1;
	request->request.cmd = ME_SETPUC;
	request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_USSD_SetNotify (HWND hWnd, int nMsg)
{
    if ( gUssdObj == NULL )
        return -1;

    gUssdObj->hWnd = hWnd;
    gUssdObj->message = nMsg;

    return 0;
}

void * ME_USSD_Request (char * str)
{
    int retval;

    if ( str == NULL )
        return NULL;

    gUssdObj->request.result = NULL;
    gUssdObj->request.result_len = 0;

    retval = USSD_Request (ME_USSD_REQ, str, strlen(str));

    if ( retval < 0 )
        return NULL;

	return gUssdObj;
}

int ME_USSD_Response (void * Obj, char * str)
{
    int retval;

    if ( str == NULL || Obj != gUssdObj )
        return -1;

    gUssdObj->request.cmddata = gUssdObj->cmddata;
    retval = USSD_Request (ME_USSD_RSP, str, strlen (str));

	return retval;
}

int ME_USSD_Abort (void * Obj)
{
    int retval;

    if ( Obj != gUssdObj )
        return -1;

    retval = USSD_Request (ME_USSD_ABT, NULL, 0);

    return retval;
}

int ME_SMS_SetFormat (HWND hwnd, int msg, BOOL bPDU)
{
    if ( bPDU )
        PostMessage (hwnd, msg, ME_RS_SUCCESS, 0);
    else
        PostMessage (hwnd, msg, ME_RS_FAILURE, 0);

    return 0;
}

int ME_SMS_SetProtocol(HWND hwnd, int msg, BOOL bPhasePlus)
{
    PostMessage (hwnd, msg, ME_RS_SUCCESS, 0);
    return 0;
}

int ME_SendATCmd (HWND hwnd, int msg, char* cmd, int timeout)
{
	int retval = 0;
	int len;
	struct ME_REQUEST_WIN* request;

	if ( cmd == NULL || timeout <= 0 )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

	len = strlen( cmd );

	if ( len+1 >= CMD_DATALEN )
		return -1;

	strcpy( request->cmddata, cmd );
	strcat( request->cmddata, "\r" );

	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = len + 2;

	request->request.arg1 = timeout;

	request->request.cmd = ME_AT_SEND;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetCallLineIndRestriction(HWND hwnd, int msg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_GET_CLIR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetCallLineIndRestriction(HWND hwnd, int msg, CLIR_MODE para)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.arg1 = para;
	request->request.cmd = ME_SET_CLIR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetLastError (HWND hWnd, int Msg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = Msg;

    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_GETLASTERROR;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}
int ME_GPRS_Monitor( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GPRS_MONITOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}
#define ATCMD_BUFSIZE       256
//int PW_ReadClock(char *P_Time)
//{
//	int fd;
//	int ret;
//	int Result = 0;
//	int ResultLen = 0;
//	unsigned char i =0;
//	unsigned char Responsebuf[128];	
//	struct pollfd ufds[1];
//
//	if(NULL == P_Time)
//		return -1;
//	
//	fd = open(ME_PORT_NAME, O_RDWR);
//	
//	if (fd < 0)
//	{
//		MsgOut("open com2 fail\r\n");
//		return -1;
//	}	
//
//	for(i=0; i<3; i++)	
//	{
//		write(fd, "AT+CCLK?\r", strlen("AT+CCLK?\r"));
//		
//		//wait response
//		ResultLen = 0;
//		memset(Responsebuf, 0, 128);
//	    while(1)
//	    {
//			ufds[0].fd = fd;
//			ufds[0].events = POLLIN;
//			ufds[0].revents = 0;
//
//			ret = poll(ufds, 1, 1500);
//			if(ret)
//			{
//			    MsgOut("return value is %d\r\n", ret);
//			    ret = read(fd, Responsebuf+ResultLen, 128);
//			    Responsebuf[ResultLen+ret] = 0;
//			    ResultLen += ret;
//			    MsgOut("%d:%s\r\n", ret, Responsebuf);
//
//			    if(strstr(Responsebuf, "OK\r"))
//			    {
//				Result = ME_GetStrSection( Responsebuf, (char*)P_Time, ME_SMS_TIME_LEN, "+CCLK:", 1 );
//				
//			    	//Result = 1;
//			    	break;
//			    }
//			    else if(strstr(Responsebuf, "ERROR\r"))
//			    {
//					Result = 0;
//					break;
//			    }
//			}
//			else
//			{
//			    MsgOut("timeout\r\n");
//			    Result = -1;
//			    break;
//			}
//	    }
//		
//		if(Result >= 0)
//			break;
//	}
//
//	close (fd);
//
//	return Result;
//}
//
//int PW_GetCharge(void *ChargeCtl)
//{
//	int fd;
//	int ret;
//	int Result = 0;
//	int ResultLen = 0;
//	unsigned char i =0;
//	unsigned char Responsebuf[128];	
//	struct pollfd ufds[1];
//
//	if(NULL == ChargeCtl)
//		return -1;
//	
//	fd = open(ME_PORT_NAME, O_RDWR);
//	if (fd < 0)
//	{
//		MsgOut("open com2 fail\r\n");
//		return -1;
//	}
//
//	for(i=0; i<3; i++)	
//	{
//		write(fd, "AT^SBC?\r", strlen("AT^SBC?\r"));
//		
//		//wait response
//		ResultLen = 0;
//		memset(Responsebuf, 0, 128);
//	    while(1)
//	    {
//			ufds[0].fd = fd;
//			ufds[0].events = POLLIN;
//			ufds[0].revents = 0;
//
//			ret = poll(ufds, 1, 1500);
//			
//			if(ret)
//			{
//			    MsgOut("return value is %d\r\n", ret);
//			    ret = read(fd, Responsebuf+ResultLen, 128);
//			    Responsebuf[ResultLen+ret] = 0;
//			    ResultLen += ret;
//			    MsgOut("%d:%s\r\n", ret, Responsebuf);
//
//			    if(strstr(Responsebuf, "OK\r"))
//			    {
//
//			            Result = ME_GetIntSection( Responsebuf, "^SBC:", 1 );
//
//			            if ( Result == -1 )
//			                break;
//
//					((ChargeCtl_t *)ChargeCtl)->ChargeStatus = Result;
//
//			            Result = ME_GetIntSection( Responsebuf, "^SBC:", 2 );
//
//			            if ( Result == -1 )
//			                break;
//
//					((ChargeCtl_t *)ChargeCtl)->BatteryCapacity = Result;            
//
//			             Result = ME_GetIntSection( Responsebuf, "^SBC:", 3 );
//
//			            if ( Result == -1 )
//			                break;
//
//					((ChargeCtl_t *)ChargeCtl)->PowerConsumption = Result;           
//		
//				
//			    	//Result = 1;
//			    	break;
//			    }
//			    else if(strstr(Responsebuf, "ERROR\r"))
//			    {
//					Result = 0;
//					break;
//			    }
//			}
//			else
//			{
//			    Result = -1;
//			    break;
//			}
//	    }
//		
//		if(Result >= 0)
//			break;
//	}
//
//	close (fd);
//
//	return Result;
//}
//
//
//
//int PW_SetAlarm(ME_ALARMTIME* AlarmTime)
//{
//	int fd;
//	int ret;
//	int Result = 0;
//	int ResultLen = 0;
//	int len = 0;
//	int offset = 0;
//	int sent = 0;	
//	unsigned char i =0;
//	unsigned char Responsebuf[64];	
//	unsigned char TimeStr[40];
//	unsigned char AlarmCmd[ATCMD_BUFSIZE];
//	struct pollfd ufds[1];
//
//	if(NULL == AlarmTime)
//		return -1;
//	
//	fd = open(ME_PORT_NAME, O_RDWR);
//	
//	if (fd < 0)
//	{
//        printf("\r\n Open /dev/com2 fail,fd:%d",fd);
//		return -2;
//	}
//
//	strcpy (AlarmCmd, "AT+CALA=");
//	sprintf (TimeStr, "\"%02d/%02d/%02d,%02d:%02d:%02d\"", 
//		AlarmTime->year, AlarmTime->month, AlarmTime->day, AlarmTime->hour, AlarmTime->minute, AlarmTime->second);
//	strcat (AlarmCmd, TimeStr);
//	strcat (AlarmCmd, "\r");
//
//    printf("\r\n AT:%s",AlarmCmd);
//
//	write(fd, AlarmCmd, strlen(AlarmCmd));
//
//	len = strlen(AlarmCmd);
//		
//	for(i=0; i<3; i++)	
//	{
//		offset = 0;
//		
//		while (offset < len)
//		{
//			sent = write(fd, AlarmCmd+offset, len - offset);
//			
//			if (sent < 0)
//			{
//                printf("\r\n write return :%d",sent);
//				Result = -4;
//				break;
//			}
//			else
//			{
//				offset += sent;				
//			}
//		}
//
//		if(Result < 0)
//        {
//            printf("\r\n if(Result < 0)");
//            break;
//        }
//		//wait response
//		ResultLen = 0;
//		memset(Responsebuf, 0, 64);
//		while(1)
//		{
//			ufds[0].fd = fd;
//			ufds[0].events = POLLIN;
//			ufds[0].revents = 0;
//
//			ret = poll(ufds, 1, 1000);
//			if(ret)
//			{
//				ret = read(fd, Responsebuf+ResultLen, 64);
//				Responsebuf[ResultLen+ret] = 0;
//				ResultLen += ret;
//
//                printf("\r\n read:%s",Responsebuf);
//				if(strstr(Responsebuf, "OK\r"))
//				{
//                    printf("\r\nReceive OK");
//					Result = 1;
//					break;
//				}
//				else if(strstr(Responsebuf, "ERROR\r"))
//				{
//                    printf("\r\nReceive ERROR");
//					Result = 0;
//					break;
//				}
//			}
//			else
//			{
//                printf("\r\n while(1) ret fail! ret:%d",ret);
//				Result = -3;
//				break;
//			}
//		}
//		
//		if(Result > 0)
//			break;
//	}
//
//	close (fd);
//
//	return Result;
//}
//////////////////////////////////////////////////////////////////////////
//static int ME_fd;
//
//int ME_PowerOff_Init(void)
//{
//    ME_fd = open(ME_PORT_NAME, O_RDWR);
//    return ME_fd;
//}
//int ME_PowerOff_Close(void)
//{
//    return close(ME_fd);
//}
//int ME_PowerOff_Operate(void)
//{
//    struct pollfd ufds[1];
//    int ret;
//    int ResultLen = 0;
//    unsigned char Responsebuf[128] = "";
//
//    ufds[0].fd = ME_fd;
//    ufds[0].events = POLLIN;
//    ufds[0].revents = 0;
//
//    while (1)
//    {
//        ret = poll(ufds, 1, 1500);
//        if(ret)
//        {
//            ret = read(fd, Responsebuf + ResultLen,sizeof(Responsebuf));
//            Responsebuf[ResultLen + ret] = '\0';
//            ResultLen += ret;
//            MsgOut("%d:%s\r\n", ret, Responsebuf);
//            if (strstr(Responsebuf,"^SYSSTART ALARM") != NULL)
//            {
//                //通知程序管理器
//            }
//            else
//            {
//                if (strstr(Responsebuf,"\r") != NULL)
//                    ResultLen = 0;
//            }
//        }
//    }
//}
