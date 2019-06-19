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


//#include <fcntl.h>
//#include <sys/poll.h>
//#include "hopen/comm.h"
#include "plx_pdaex.h"

#include "stdlib.h"
#include "window.h"
#include "string.h"
#include "stdio.h"
#include "me_func.h"
#include "me_at.h"
#include "me_wnd.h"
#include "me_comn.h"
#include "smspdu.h"
#include "cbspdu.h"
#include "unistd.h"
#include "ctype.h"
#include "assert.h"

static const char FRC_String[][20] = 
{
    {"OK"},
    {"ERROR"},
    {"+CME ERROR:"},
    {"+CMS ERROR:"},
    {"BUSY"},
    {"NO DIALTONE"},
    {"NO CARRIER"},
    {"CONNECT"},
    {""}
};

static const char URC_String[][20] = 
{
    {"+CALA:"},       // Alarm notification 
    {"+CBM:"},        // Cell  Broadcast Message directly displayed 
    {"+CBMI:"},       // Cell Broadcast Message stored in mem at location <index> 
    {"+CCCM:"},       // Current Call Meter value 
    {"+CCED:"},       // Cell Environment Description indication 
    {"+CCWA:"},       // Call Waiting number 
    {"+CDS:"},        // SMS status report after sending a SMS 
    {"+CDSI:"},       // Incoming  SMS Status Report after sending a SMS, stored in <mem> (.SR.) at location <index> 
    {"+CKEV:"},       // Key press or release 
    {"+CLIP:"},       // Incoming Call Presentation 
    {"+CMT:"},        // Incoming message directly displayed 
    {"+CMTI:"},       // Incoming  message  stored  in  <mem> (.SM.) at location <index> 
    {"+CREG:"},       // Network registration indication 
    {"+CGREG:"},
    {"+CRING:"},      // Incoming call type (VOICE, FAX ...) 
    {"+CSQ:"},        // Automatic RxLev indication with AT+CCED=1,8  command 
    {"+CSSI:"},
    {"+CSSU:"},       // Supplementary  service  notification during a call 
    {"^SSTN:"},       // SIM Toolkit Indication 
    {"^SMGO:"},
    {"+RUI:"},        // Receive  additional  User  Info  on  a  MT call setup, or a remote call release 
    {"RING"},         // Incoming call signal from network
    {"NO CARRIER"},
    {"^SYSSTART"},
    {"^SBC: Undervoltage"},
    {"+CIEV:"},		
    {""}
};



#ifdef _ME_DEBUG
static const char BR_String[BR_Total][10] = 
{
	{"300"},
	{"600"},
	{"1200"},
	{"2400"},
	{"4800"},
	{"9600"},
	{"14400"},
	{"19200"},
	{"28800"},
	{"38400"},
	{"57600"},
	{"115200"},
	{"230400"}	
};
#endif

#ifndef NULL
#define NULL    (void *)0
#endif

#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | \
                            (((DWORD)((WORD)(high))) << 16)))

#define ATCMD_BUFSIZE       (256)
#define INPUTLINE_SIZE      (1024)

#define FSM_STATE_INIT      0
#define FSM_STATE_DONE      11
#define FSM_STATE_STEP1     1
#define FSM_STATE_STEP2     2
#define FSM_STATE_STEP3     3
#define FSM_STATE_STEP4     4
#define FSM_STATE_STEP5     5
#define FSM_STATE_STEP6     6

#define USSD_IDLE         FSM_STATE_INIT
#define USSD_CONFIRM      FSM_STATE_STEP1
#define USSD_LISTEN       FSM_STATE_STEP2
#define USSD_INPUT        FSM_STATE_STEP3
#define USSD_RESPONSE     FSM_STATE_STEP4
#define USSD_ABORT        FSM_STATE_STEP5
#define USSD_DISC         FSM_STATE_STEP6

struct ME_INDTABLE IndTable;
HWND ME_hWnd;
static int gInitDone;
static int gReqID = 0;
int gFsmState;
static int gUssdState;
UINT    gTimerID, gUssdTimer;
static char gUssdBuf[512];
MEREQUEST * gReqHead;
static MEREQUEST * gReqTail, * gReqUssd;
static struct ME_INDICATION * current_ind; 

extern int MAT_Callback (int);
extern int Me_GetInitValue (PME_INIT_GETVALUE pMeValue);
extern int MAT_AutoSetMeBaudRate(void);

LRESULT CALLBACK ME_WndProc (HWND, UINT, WPARAM, LPARAM);
void    CALLBACK ME_TimerProc (HWND, UINT, UINT, DWORD);
void    CALLBACK USSD_TimerProc (HWND, UINT, UINT, DWORD);

void ME_MatIndication (int event, void * pdata, int datalen);
void ME_UssdIndication (int event, void * pdata, int datalen);
static int DispatchIndicator (char* data, int datalen);
static int OnATDataIn (void * pdata, int datalen);
static int OnUssdDataIn (void * pdata, int datalen);
static int InsertIndQueue (int index, int Mode, ME_IndCallback notify);
static int FindFreeIndPosition (struct ME_INDICATION* pNode);

static int IsAsyncCmd (int cmd);
static int me_do_cmd (MEREQUEST * request);
static int FSM_Entry (int event, char * data, int datalen);

static int FSM_GetCPHSInfo(int event,char *data, int datalen);
static int FSM_GetCPHSparas( int event, char* data, int datalen);
static int FSM_ReadCPHSFiles( int event, char* data, int datalen);
static int FSM_ReadCPHSRecord( int event, char* data, int datalen);
static int FSM_UpdateCPHSFiles( int event, char* data, int datalen);
static int FSM_UpdateCPHSRecord( int event, char* data, int datalen);


static int FSM_SpecialCall      (int event, char* data, int datalen);
static int FSM_USSD             (int event, char* data, int datalen);
static int FSM_SetCallFwding    (int event, char* data, int datalen);
static int FSM_GetCallFwding    (int event, char* data, int datalen);
static int FSM_SetCallBarring   (int event, char* data, int datalen);
static int FSM_GetCallBarring   (int event, char* data, int datalen);
static int FSM_SMSSend          (int event, char* data, int datalen);
static int FSM_SMSRead          (int event, char* data, int datalen);
static int FSM_SMSWrite 	(int event, char* data, int datalen);
static int FSM_SMSList          (int event, char* data, int datalen);
static int FSM_SMSDelete        (int event, char* data, int datalen);
static int FSM_PBookRead        (int event, char* data, int datalen);
static int FSM_SetPhoneLock     (int event, char* data, int datalen);
static int FSM_GetPhoneLock     (int event, char* data, int datalen);
static int FSM_ChangePassword   (int event, char* data, int datalen);
int        Me_Fsm_Init          (int event, int param, char * data, int datalen);

static int IsUslctResultCode (char* buf, unsigned int buflen);
static int IsFinalResultCode (char* buf, unsigned int buflen);
static BOOL IsTwoLineIndicatior (int index);

void       GetResultLength (MEREQUEST* pRequest);
static int GetResultErrorCode (char* data, int datalen);
static int GetUssdErrorCode (char* data, int datalen);
static int FillResultField (void* result, int result_len, MEREQUEST* pRequest);

static void GetURCLength (struct ME_INDICATION* pInd, char* pdata, int datalen);
static int FillURCField (void* result, int result_len, struct ME_INDICATION* pInd);

static int AnaURC_CRING (char *SrcStr, int* Type);
static int AnaURC_CLIP  (char *SrcStr,  PME_PHONE_ITEM pItem);
static int AnaURC_CCWA  (char * SrcStr, PME_PHONE_ITEM pItem);
static int AnaURC_CSSI  (char *SrcStr, PME_CSSI_ITEM pItem);
static int AnaURC_CSSU  (char *SrcStr, PME_CSSU_ITEM pItem);

static int  AnaURC_SIND( char *SrcStr, PME_EXINFO_ITEM pInfo );
static int AnaURC_SMGO  (char *SrcStr, int* Type);
static int AnaURC_CMTI  (char *SrcStr, PME_CMTI_ITEM pItem);
static int AnaURC_STK   (char *SrcStr, int* event);
static int AnaURC_CREG  (char *SrcStr, PME_CREG_URC pItem);
static int AnaURC_CGREG (char *SrcStr, PME_CGREG_URC pItem);
static int AnaURC_CMT   (char* SrcStr, PSMS_INFO info);
static int AnaURC_CBM   (char* SrcStr, PCBS_INFO info);
static int AnaURC_CDS   (char* SrcStr, PSMS_INFO info);
static int AnaURC_CCCM  (char * src, unsigned long * ccm);

static int GetCLIPResult(void * result, char * str, int len);
static int GetCLIRResult(void * result, char * str, int len);
static int GetCOLPResult(void * result, char * str, int len);
static int GetCOLRResult(void * result, char * str, int len);
static int GetCFResult  (void * result, char * str, int len);
static int GetCWResult  (void * result, char * str, int len);
static int GetCBResult  (void * result, char * str, int len);
static int USSD_Decode (PUSSD_INFO info, char * src, unsigned char dcs);

static int FSM_SwitchCH0(BOOL bOpenChnl);

#define INIT_RETRY      3
#define INIT_EXPIRE     3000
#define INIT_TIMERID    101

#if 0
//int ClearCNMIsetting(void)
//{
//	int fd;
//	int ret;
//	int Result = 0;
//	int ResultLen = 0;
//	unsigned char i =0;
//	unsigned char Responsebuf[64];	
//	struct pollfd ufds[1];
//	
//    fd = open(ME_PORT_NAME, O_RDWR);
//    if (fd < 0)
//    {
//		MsgOut("open com2\r\n");
//		return -1;
//    }	

//	ret = ioctl(fd, SIOSETBAUDRATE, 115200);
//	if(ret)
//	{
//		return -1;
//	}

//	for(i=0; i<3; i++)	
//	{
//		write(fd, "AT&F\r", strlen("AT&F\r"));
//		
//		//wait response
//		ResultLen = 0;
//		memset(Responsebuf, 0, 64);
//	    while(1)
//	    {
//			ufds[0].fd = fd;
//			ufds[0].events = POLLIN;
//			ufds[0].revents = 0;
//
//			ret = poll(ufds, 1, 1000);
//			if(ret)
//			{
//			    MsgOut("return value is %d\r\n", ret);
//			    ret = read(fd, Responsebuf+ResultLen, 64);
//			    Responsebuf[ResultLen+ret] = 0;
//			    ResultLen += ret;
//			    MsgOut("%d:%s\r\n", ret, Responsebuf);
//
//			    if(strstr(Responsebuf, "OK\r"))
//			    {
//			    	MsgOut("clear cnmi successfully\r\n");
//			    	Result = 1;
//			    	break;
//			    }
//			    else if(strstr(Responsebuf, "ERROR\r"))
//			    {
//			    	MsgOut("clear cnmi fail\r\n");
//					Result = 0;
//					break;
//			    }
//			}
//			else
//			{
//			    MsgOut("clear cnmi timeout\r\n");
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


//fufu 0629
//int AutoSetMeBaudRate(void)
//{
//	int fd;
//	int ret;
//	int Result = 0;
//	int ResultLen = 0;
//	char i = 0;
//	int Baud = 0;
//	unsigned char ATbuf[64];
//	unsigned char Responsebuf[64];	
//	struct pollfd ufds[1];
//	
//    fd = open(ME_PORT_NAME, O_RDWR);
//    if (fd < 0)
//    {
//		MsgOut("open com2 fail\r\n");
//		return -1;
//    }
//		
//	for(i=BR_Total-1; i>=BR_9600; i--)
//	{
//		//set mcu com2 baud rate
//		switch(i)
//		{
//			case BR_230400:
//				Baud = 230400;
//				break;
//			case BR_115200:
//				Baud = 115200;
//				break;
//			case BR_57600:
//				Baud = 57600;
//				break;
//			case BR_38400:
//				Baud = 38400;
//				break;
//			case BR_28800:
//				Baud = 28800;
//				break;
//			case BR_19200:
//				Baud = 19200;
//				break;
//			case BR_14400:
//				Baud = 14400;
//				break;
//			case BR_9600:
//				Baud = 9600;
//				break;
//			case BR_4800:
//				Baud = 4800;
//				break;
//			case BR_2400:
//				Baud = 2400;
//				break;
//			case BR_1200:
//				Baud = 1200;
//				break;
//			case BR_600:
//				Baud = 600;
//				break;
//			case BR_300:
//				Baud = 300;
//				break;
//			default:
//				goto Failure;
//		}
//		ret = ioctl(fd, SIOSETBAUDRATE, Baud);
//		if(ret)
//		{
//			continue;
//		}
//
//		//send at command to set mc55 baud rate
//		memset(ATbuf, 0, sizeof(ATbuf));
//		
//		strcpy(ATbuf, "AT+IPR=115200\r");
//
//		write(fd, ATbuf, strlen(ATbuf));
//		
//		//wait response
//		ResultLen = 0;
//		
//	    while(1)
//	    {
//			ufds[0].fd = fd;
//			ufds[0].events = POLLIN;
//			ufds[0].revents = 0;
//
//			ret = poll(ufds, 1, 1000);
//			if(ret)
//			{
//			    MsgOut("return value is %d\r\n", ret);
//			    ret = read(fd, Responsebuf+ResultLen, 64);
//			    Responsebuf[ResultLen+ret] = 0;
//			    ResultLen += ret;
//			    MsgOut("%d:%s\r\n", ret, Responsebuf);
//
//			    if(strstr(Responsebuf, "OK\r"))
//			    	goto Success;
//			}
//			else
//			{
//			#ifdef _ME_DEBUG	
//			    MsgOut("baud rate %s fail\r\n", BR_String[i]);
//			#endif
//			    goto NoResponse;
//			}
//	    }
//	    
//		//Failure
//Failure:
//		Result = 0;
//		break;
//		
//		//success
//Success:
////		ret = ioctl(fd, SIOSETBAUDRATE, 115200);
//		
////		if(ret)
////		{
////			Result = 0;
////		}
////		else
////		{
//			Result = 1;
////		}
//		break;
//		
//		//no response
//NoResponse:
//		Result = -1;
//		continue;
//		
//	}
//
//	ResultLen = 3;
//	while(ResultLen-- > 0)
//	{
//		ret = ioctl(fd, SIOSETBAUDRATE, 115200);
//		if(!ret)
//		{
//			break;
//		}
//	}
//
//	close (fd);
//
//	if(i < 0)
//		return -1;
//	else
//		return Result;
//
////	if(Result)
////		return 1;
////	else
////		return -1;
//}
//end 0629
#endif
static int me_init (MEREQUEST * pRequest)
{
    WNDCLASS wc;
    MSG      msg;
    int     i = 0;

    gReqHead = gReqTail = NULL;

    memset (&wc, 0, sizeof(wc));
    wc.lpfnWndProc   =  ME_WndProc;
    wc.lpszClassName =  "MEWndClass";
    if ( !RegisterClass(&wc) )
        return -1;

    ME_hWnd = CreateWindow ("MEWndClass" ,"", 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
    if ( ME_hWnd == NULL)
        return -1;

    if ( MAT_Init (pRequest->arg1, 0, 0, ME_MatIndication) < 0 )
        return -1;

	gFsmState = FSM_STATE_INIT;

    while ( i < INIT_RETRY )
    {
        SetTimer (ME_hWnd, INIT_TIMERID, INIT_EXPIRE, NULL);
#if defined(_EMULATE_)
        MAT_Send (1, "AT\r", -1);
#else
        gInitDone = 1;
#endif
        while ( GetMessage(&msg, NULL, 0, 0) )
        {
            if ( msg.message == WM_TIMER && msg.hwnd == ME_hWnd
                && msg.wParam == INIT_TIMERID )
            {
                KillTimer (ME_hWnd, INIT_TIMERID);
                i++;
                break;
            }

            TranslateMessage (&msg);
            DispatchMessage (&msg);

            if ( gInitDone )
                return 0;
        }
    }

    return 0;
}

int USSD_Init (MEREQUEST * pRequest)
{
    if ( MAT_Init (pRequest->arg1, 1, 0, ME_UssdIndication) < 0 )
        return -1;

    if ( pRequest == NULL )
        return -1;

    pRequest->cmd = ME_USSD;
    gReqUssd = pRequest;
    gUssdState = USSD_IDLE;

    return 1;
}

static int check_executing_queue()
{
#ifdef _ME_DEBUG_DETAILS
	int i = 0;
	MEREQUEST *p = gReqHead;
	MsgOut("\r\n[ME]: current request queue:\r\n");
	while(p)
	{
		MsgOut("0x%08x = %03d, ", p, p->cmd);
		p = p->next;
		i ++;
		if(i%4 == 0)
			MsgOut("\r\n");
	}
	MsgOut("\r\n[ME]: pTail = 0x%08x\r\n", gReqTail);
	return i;
#else
	return 0;
#endif
}

extern int check_request_queue();

int ME_Request (MEREQUEST * pRequest)
{
    int retval = 0;

	if(gReqID > 0x0ffffffe)
		gReqID = 0;

    if ( !gInitDone )
    {
        /* Only init can be called */
        if ( pRequest->cmd != ME_INIT )
            return -1;

        me_init( pRequest );

        return gInitDone ? 1 : -1;
    }
    else if ( pRequest->cmd == ME_INIT )
        return gInitDone;

    pRequest->next = NULL;

    if ( !IsAsyncCmd (pRequest->cmd) )
    {
        /* This is not async command */
        retval = me_do_cmd (pRequest);
    }
    else if ( gReqHead )
    {
        /* We are in command exec, add command into list tail */
#if 0
        if ( pRequest->index >= 0 ) {
            if ( gReqHead->index == pRequest->index ) {
                FSM_Entry (RESPONSE, pRequest->cmddata, pRequest->cmddata_len);
            }
            else {
                MEREQUEST * current, * before;
                before = gReqHead;
                current = gReqHead->next;
                while ( current != NULL ) {
                    if ( current->index == pRequest->index ) {
                        before->next = current->next;
                        if ( current == gReqTail )
                            gReqTail = before;
                        break;
                    }
                    before = current;
                    current = current->next;
                }
                if ( gReqHead->next = NULL)
                    gReqTail = pRequest;
                pRequest->next = gReqHead->next;
                gReqHead->next = pRequest;
            }
            return pRequest->index;
        }
        if ( pRequest->cmd == ME_USSD ) {
            if ( gReqHead->next = NULL)
                gReqTail = pRequest;
            pRequest->next = gReqHead->next;
            gReqHead->next = pRequest;
            pRequest->index = gReqID;
        }
        else {
#endif
            gReqTail->next = pRequest;
            gReqTail = pRequest;
            pRequest->index = gReqID;
			ShowDetail("\r\n[ME]: new request added, 0x%08x = %d\r\n", pRequest, pRequest->cmd);
			ASSERT(check_request_queue() == check_executing_queue()+1);
#if 0
        }
#endif
    }
    else
    {
        /* We are not in command exec */
        gReqHead = gReqTail = pRequest;
        /* Start do this command */
        gFsmState = FSM_STATE_INIT;
        PostMessage (ME_hWnd, WM_FSM_START, 0, 0);
        pRequest->index = gReqID;
		ShowDetail("\r\n[ME]: first request added, 0x%08x = %d\r\n", pRequest, pRequest->cmd);
		ASSERT(check_request_queue() == check_executing_queue()+1);
    }

    return gReqID++;
}

int USSD_Request (int mode, char * data, int datalen)
{
    int retval = 0;
    switch ( mode ) {
    case ME_USSD_REQ:

        FSM_USSD (REQUEST, data, datalen);

        break;

    case ME_USSD_RSP:

        retval = FSM_USSD (RESPONSE, data, datalen);

        break;

    case ME_USSD_ABT:

        retval = FSM_USSD (USERSTOP, data, datalen);

        break;

    default:
        return -1;
    }

    return retval;
}

int ME_CancelRequest (MEREQUEST * pRequest)
{
	if ( NULL == gReqHead)
	{
		MsgOut("\r\n[ME]: cancel request. request is NULL\r\n");
		return -1;
	}
	
    if ( pRequest->index == gReqHead->index )
	{
        FSM_Entry (USERSTOP, 0, 0);
        if ( gReqHead != NULL && gFsmState == FSM_STATE_DONE )
		{
            if ( gReqHead->reply )
                gReqHead->reply (gReqHead);

            gReqHead = gReqHead->next;
			ShowDetail("\r\n[ME]: cancel request. head of queue updated, gRegHean = 0x%08x\r\n", gReqHead);

            if ( gReqHead )
			{
                gFsmState = FSM_STATE_INIT;
                PostMessage (ME_hWnd, WM_FSM_START, 0, 0);
            }
        }
    }
    else
	{
        MEREQUEST *cur_req, *pri_req;
        pri_req = gReqHead;
        cur_req = gReqHead->next;
        while ( cur_req != NULL )
		{
            if ( cur_req->index == pRequest->index )
			{
                if ( cur_req->reply )
				{
                    cur_req->error = ME_RS_USERSTOP;
                    cur_req->reply (cur_req);
					ShowDetail("\r\n[ME]: cancel request. this request is replyed: 0x%08x = %d\r\n", cur_req, cur_req->cmd);
                }
				ShowDetail("\r\n[ME]: cancel request. this request is released: 0x%08x = %d\r\n", cur_req, cur_req->cmd);
                if(cur_req == gReqTail)
					gReqTail = pri_req;
				pri_req->next = cur_req->next;
                break;
            }
            pri_req = pri_req->next;
            cur_req = cur_req->next;
        }
        if ( cur_req == NULL )
        {
			ShowDetail("\r\n[ME]: cancel request. request not found: 0x%08x = %d\r\n", pRequest, pRequest->cmd);
			return -1;
		}
    }

    return 0;
}

int ME_GetCurrentResult( void* result, int result_len )
{
    // 当前结果为URC数据
    if ( current_ind != NULL )
    {
        if ( NULL == result )
            return current_ind->msgresult_len;

        if ( result_len < 0 || current_ind->msgresult_len < 0 )
            return -1;

        if ( result_len < current_ind->msgresult_len )
            return-1;

        return FillURCField( result, result_len, current_ind );
    }
	
    // 当前结果为请求返回结果
    if ( gReqHead != NULL && gFsmState == FSM_STATE_DONE )
    {
        if ( NULL == result )
            return gReqHead->result_len;

        if ( result_len < 0 || gReqHead->result_len < 0 )
            return -1;

        if ( result_len < gReqHead->result_len )
            return -1;

        return FillResultField( result, result_len, gReqHead );
    }

    return -1;
}

int ME_RegisterInd (int msg_class, int mode, ME_IndCallback notify)
{
    int i, index, count, first;

    WAIT_FOR_MUTEX();

    count = IndTable.Count;
    first = MAX_QUEUE_INDICATS;

    if ( count == MAX_QUEUE_INDICATS )
        goto ERROR_TAG;

    for( i=0; i < MAX_QUEUE_INDICATS; i++ )
    {
        if ( IndTable.Inds[i].reg_head != NULL )
        {
            if ( msg_class == IndTable.Inds[i].msgclass )
                break;
        }
        else
        {
            first = ( i < first )?i:first;
        }
    }

    if ( i < MAX_QUEUE_INDICATS )
    {
        if ( IND_EXCLUSIVE == mode )
            goto ERROR_TAG;
        else
        {
            index = InsertIndQueue (i, mode, notify);
            if ( index < 0 )
                goto ERROR_TAG;
        }

        RELEASE_MUTEX();
        return i*100+index;
    }
    else
    {
        if ( first == MAX_QUEUE_INDICATS )
            goto ERROR_TAG;

        index = InsertIndQueue (first, mode, notify);
        if ( index < 0 )
            goto ERROR_TAG;

        IndTable.Inds[first].msgclass = msg_class;
        IndTable.Count++;

        RELEASE_MUTEX();

        return first*100+index;
    }

ERROR_TAG:
    RELEASE_MUTEX();
    return -1;
}

int ME_UnRegisterInd( int Handle )
{
    int id1, id2;
    struct ME_REGISTER *pInd, *pPrior;
    int count;

    WAIT_FOR_MUTEX();

    count = IndTable.Count;

    id1 = (int)Handle%100;
    id2 = (int)Handle/100;

    if ( id2 < 0 || id2 >= MAX_QUEUE_INDICATS )
        goto ERROR_TAG;

    if ( id1 < 0 || id1 >= MAX_QUEUE_REGISTER )
        goto ERROR_TAG;

    if ( IndTable.Inds[id2].reg_count == 0 )
        goto ERROR_TAG;

    pInd = pPrior = IndTable.Inds[id2].reg_head;

    while ( pInd->pNext != NULL )
    {
        if ( pInd == &(IndTable.Inds[id2].regs[id1]) )
        {
            pInd->used = FALSE;

            if ( pInd == IndTable.Inds[id2].reg_head )
                IndTable.Inds[id2].reg_head = pInd->pNext;
            else
                pPrior->pNext = pInd->pNext;

            break;
        }
        pPrior = pInd;
        pInd = pInd->pNext;
    }

    if ( pInd->pNext == NULL )
    {
        if ( pInd == &(IndTable.Inds[id2].regs[id1]) )
        {
            pInd->used = FALSE;

            if ( pInd == IndTable.Inds[id2].reg_head )
                IndTable.Inds[id2].reg_head = NULL;
            else
                pPrior->pNext = NULL;
        }
    }

    IndTable.Inds[id2].reg_count --;

    if ( IndTable.Inds[id2].reg_count == 0 )
        IndTable.Count--;

    RELEASE_MUTEX();
    return 0;

ERROR_TAG:
    RELEASE_MUTEX();
    return -1;
}

static int InsertIndQueue( int index, int Mode, ME_IndCallback notify )
{
    struct ME_REGISTER *pFree, *pMove;
    struct ME_INDICATION *pNode = &(IndTable.Inds[index]);
    int retval,index2;

    if ( pNode->reg_head == NULL )
    {
        pNode->regs[0].notify = notify;
        pNode->regs[0].mode = Mode;
        pNode->regs[0].used = TRUE;
        pNode->regs[0].pNext = NULL;
        pNode->reg_head = &(pNode->regs[0]);

        retval = 0;
    }
    else
    {
        index2 = FindFreeIndPosition (pNode);

        if ( index2 < 0 )
            return -1;

        pFree = &(pNode->regs[index2]);

        pFree->notify = notify;
        pFree->used = TRUE;
        pFree->mode = Mode;
        pFree->pNext = NULL;

        if ( IND_PRIORITY == Mode )
        {
            pFree->pNext = pNode->reg_head;
            pNode->reg_head = pFree;
        }
        else
        {
            pMove = pNode->reg_head;

            while( pMove->pNext != NULL)
                pMove = pMove->pNext;

            pMove->pNext = pFree;
        }

        retval = index2;
    }

    pNode->reg_count++;

    return retval;
}


static int FindFreeIndPosition (struct ME_INDICATION* pNode)
{
    int i;
    if ( pNode->reg_count >= MAX_QUEUE_REGISTER )
        return -1;

    for ( i = 0; i < MAX_QUEUE_REGISTER; i++ )
    {
        if ( !(pNode->regs[i].used) )
            break;

        if ( pNode->regs[i].mode == IND_EXCLUSIVE )
            return -1;
    }

    return i;
}

void ME_MatIndication(int event, void * pdata, int datalen)
{
    int len;
    static char buf[INPUTLINE_SIZE];
    static int size = 0;

    if ( event != MAT_DATAIN ) 
        return;

    if ( size > 0 ) {
        char * ptr = memchr (pdata, '\n', datalen);
        if ( ptr == NULL ) {
            memcpy (&buf[size], pdata, datalen);
            size += datalen;
            return;
        }
        else {
            len = ptr - (char*)pdata+1;
            memcpy (&buf[size], pdata, len);
            OnATDataIn (buf, size+len);
            datalen -= len;
            pdata = ptr + 1;
            size = 0;
        }
    }

    len = OnATDataIn (pdata, datalen);

    /* Here got some data left, we save it and process next time */
    datalen -= len;
    if ( datalen > 0 ) {
        memcpy (buf, &((char*)pdata)[len], datalen);
        size = datalen;
    }

    return;
}

static int OnATDataIn (void * pdata, int datalen)
{
    int i, retval;
    char * ptr1 = pdata, * ptr2 = pdata;

    for ( i = 0; i < datalen; i++, ptr2++ ) {
        if ( *ptr2  == '\0' )
            continue;

        if ( (ptr2-ptr1) == INPUTLINE_SIZE ) {
            MAT_Reset (0);
            ptr1 = (char*)pdata + datalen;
            break;
        }

        if ( *ptr2 != '\n' ) {
            /* When sending short message we got '>' and no \r\n followed */
            if ( (*ptr2 == '>') && (ptr2 == ptr1) ) {
                char escape = 0x1b;
                if ( gReqHead != NULL ) {
                    retval = FSM_Entry (DATAIN, ptr1, 1);

                    if ( retval >= 0 ) {
                        ptr1 = ptr2+1;
                        goto check_cmdqueue;
                    }
                }
                ptr1 = ptr2 + 1;
                MAT_Send (1, &escape, 1);
            }
            continue;
        }

        /* Empty line just "\r\n" */
        if ( ptr2 - ptr1 < 2 ) {
            ptr1 = ptr2 + 1;
            continue;
        }

        *ptr2 = '\0';

        /* We here send data to state machine first */
        if ( gReqHead != NULL || !gInitDone ) {
		//当pdata 是命令回应时，gFsmState不应该是FSM_STATE_DONE
		if(FSM_STATE_DONE != gFsmState)
		{
	            retval = FSM_Entry (DATAIN, ptr1, ptr2-ptr1+1);

	            if ( retval >= 0 ) {
	            	
	                ptr1 = ptr2+1;
		                goto check_cmdqueue;
		        }
		}
        }

        /* We here dispatch the indication message */
        retval = DispatchIndicator (ptr1, ptr2-ptr1+1);

        /* Discard data */
        ptr1 = ptr2 + 1;

        continue;
        
check_cmdqueue:
        if ( gReqHead != NULL && gFsmState == FSM_STATE_DONE )
        {
            /* Call reply function */
            if ( gReqHead->reply )
            {
                gReqHead->reply (gReqHead);
                ME_MemInitial();
            }
            /* Unlink it from list */
            gReqHead = gReqHead->next;
			ShowDetail("\r\n[ME]: head of queue updated, gRegHean = 0x%08x\r\n", gReqHead);
            
            /* Check for start next command */
            if ( gReqHead )
            {
                /* Start do this command */
                gFsmState = FSM_STATE_INIT;
                PostMessage (ME_hWnd, WM_FSM_START, 0, 0);
            }
            else
            {
                f_sleep_unregister(ME_WAITRESP);
            }
        }
    }
    
    return ptr1 - (char*)pdata;
}

void ME_UssdIndication (int event, void * pdata, int datalen)
{
    int len;
    static char buf[INPUTLINE_SIZE];
    static int size = 0;

    if ( event != MAT_DATAIN ) 
        return;

    if ( size > 0 ) {
        char * ptr = memchr (pdata, '\n', datalen);
        if ( ptr == NULL ) {
            memcpy (&buf[size], pdata, datalen);
            size += datalen;
            return;
        }
        else {
            len = ptr - (char*)pdata+1;
            memcpy (&buf[size], pdata, len);
            OnUssdDataIn (buf, size+len);
            datalen -= len;
            pdata = ptr + 1;
            size = 0;
        }
    }

    len = OnUssdDataIn (pdata, datalen);

    /* Here got some data left, we save it and process next time */
    datalen -= len;
    if ( datalen > 0 ) {
        memcpy (buf, &((char*)pdata)[len], datalen);
        size = datalen;
    }

    return;
}

static int OnUssdDataIn (void * pdata, int datalen)
{
    int i;
    char * ptr1 = pdata, * ptr2 = pdata;

    for ( i = 0; i < datalen; i++, ptr2++ ) {
        if ( *ptr2  == '\0' )
            continue;

        if ( (ptr2-ptr1) == INPUTLINE_SIZE ) {
            MAT_Reset (0);
            ptr1 = (char*)pdata + datalen;
            break;
        }

        if ( *ptr2 != '\n' ) {
            /* When USSD message we got '>' and no \r\n followed */
            if ( (*ptr2 == '>') && (ptr2 == ptr1) ) {
                char escape = 0x1b;
                if ( gUssdState == USSD_INPUT )
                    FSM_USSD (DATAIN, ptr1, 1);
                else
                    MAT_Send (1, &escape, 1);
                ptr1 = ptr2 + 1;
            }
            continue;
        }

        /* Empty line just "\r\n" */
        if ( ptr2 - ptr1 < 2 ) {
            ptr1 = ptr2 + 1;
            continue;
        }

        *ptr2 = '\0';

        FSM_USSD (DATAIN, ptr1, ptr2-ptr1+1);
        ptr1 = ptr2+1;
    }
    
    return ptr1 - (char*)pdata;
}

static int DispatchIndicator (char* data, int datalen)
{
    struct ME_REGISTER* preg;
    int idx;
    static char part[512];
    static int waitidx = -1;
    unsigned long para1, para2;

    data[datalen-1] = '\n';

    /* We here waiting for the second line of indicator */
    if ( waitidx >= 0 ) {
        int len = strlen (part);
        memcpy (&part[len], data, datalen);
        idx = waitidx;
        data = part;
        datalen += len;
        waitidx = -1;
    }
    else {
        idx = IsUslctResultCode (data, datalen);
        if ( idx < 0 )
            return idx;
        if ( IsTwoLineIndicatior (idx) ) {
            memcpy (part, data, datalen);
            part[datalen] = '\0';
            waitidx = idx;
            return idx;
        }
    }

    current_ind = &IndTable.Inds[idx];

    GetURCLength (current_ind, data, datalen);

    preg = current_ind->reg_head;
    para1 = (unsigned long)MAKELONG(current_ind->msgclass, 0);
    para2 = (unsigned long)current_ind->msgresult_len;

    while ( preg )
    {
        preg->notify (preg, para1, para2);
        preg = preg->pNext;
    }
    current_ind = NULL;

    return idx;
}
 
LRESULT CALLBACK ME_WndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    if ( wMsgCmd == WM_MAT_MESSAGE )
        MAT_Callback ((int)wParam);
    else if ( wMsgCmd == WM_FSM_START )
    {
        while ( gReqHead != NULL )
        {
            FSM_Entry ( START, 0, 0 );
            if ( gFsmState != FSM_STATE_DONE )
                break;
            {
                MEREQUEST * request = gReqHead;
                if ( request->reply )
                {
                    request->reply (request);
                    ME_MemInitial();
                }
                gReqHead = request->next;
            }
        }
    }
    else
        return DefWindowProc( hWnd, wMsgCmd, wParam, lParam);

    return 0;
}

void CALLBACK ME_TimerProc (HWND hWnd, UINT Msg, UINT id, DWORD Time)
{
    KillTimer( NULL, id );
#if defined _ME_DEBUG
    MsgOut( "\r\nME Request Timeout!" );
#endif

    FSM_Entry (TIMEOUT, 0, 0);

    if ( gReqHead != NULL && gFsmState == FSM_STATE_DONE )
    {
        /* Call reply function */
        if ( gReqHead->reply )
        {
            gReqHead->reply (gReqHead);
            ME_MemInitial();
        }
        /* Unlink it from list */
        gReqHead = gReqHead->next;
		ShowDetail("\r\n[ME]: head of queue updated, gRegHean = 0x%08x\r\n", gReqHead);

        /* Check for start next command */
        if ( gReqHead )
        {
            /* Start do this command */
            gFsmState = FSM_STATE_INIT;
            PostMessage (ME_hWnd, WM_FSM_START, 0, 0);
        }
    }
}

void CALLBACK USSD_TimerProc (HWND hWnd, UINT Msg, UINT id, DWORD Time)
{
    KillTimer (NULL, id);
#if defined _ME_DEBUG
    MsgOut ("\r\nME USSD Request Timeout!");
#endif

    FSM_USSD (TIMEOUT, 0, 0);
}

/******************************************************************************
 *
 * Module dependent.
 *
 ******************************************************************************
 */

static int IsAsyncCmd (int cmd)
{
    return 1;
}

static int me_do_cmd (MEREQUEST * request)
{
    return 0;
}
static int FSM_InitRate( int event, char* data, int datalen )
{
    char ATCmd[ATCMD_BUFSIZE] = "";
    int retval;
    if ( FSM_STATE_INIT == gFsmState )
    {
        if ( START == event )
        {
            if ( gReqHead->arg1 < 0 )
                goto ERROR_HANDLER;
            
//            MAT_AutoSetMeBaudRate();
//
//            strcpy(ATCmd,"AT+IPR=115200\r");
            strcpy(ATCmd,"AT\\Q3\r");
            retval = MAT_Send( 1, ATCmd, -1 );
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
        }
    }
    else if ( FSM_STATE_STEP1 == gFsmState )
    {
        if ( DATAIN == event )
        {
            retval = GetResultErrorCode( data, datalen );
            
            if ( retval < 0 )
                return retval;

//            if (gReqHead->error != ME_RS_SUCCESS)
//            {
//                gFsmState = FSM_STATE_INIT;
//                return 0;
//            }
            KillTimer(NULL, gTimerID);
            
//            strcpy(ATCmd,"AT\\Q3\r");
            MAT_AutoSetMeBaudRate();

            strcpy(ATCmd,"AT+IPR=115200\r");
            retval = MAT_Send(1,ATCmd,strlen(ATCmd));
            gTimerID = SetTimer(NULL,0,DFT_TIMEOUT,ME_TimerProc);
            gFsmState = FSM_STATE_STEP2;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP2 == gFsmState ) {
        if ( DATAIN == event ) {
            retval = GetResultErrorCode( data, datalen );
            if ( retval < 0 )
                return retval;
            KillTimer(NULL, gTimerID);

            strcpy(ATCmd,"AT&W\r");
            retval = MAT_Send(1,ATCmd,strlen(ATCmd));
            gTimerID = SetTimer(NULL,0,DFT_TIMEOUT,ME_TimerProc);
            gFsmState = FSM_STATE_STEP3;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP3 == gFsmState ) {
        if ( DATAIN == event ) {
            retval = GetResultErrorCode( data, datalen );
            if ( retval < 0 )
                return retval;

            //MUX_Init();
            KillTimer(NULL, gTimerID);
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}
static int FSM_Entry (int event, char * data, int datalen)
{
    char ATCmd[ATCMD_BUFSIZE];
    int retval = 0;

#ifdef _ME_DEBUG_DETAILS
	if ( START == event )
	{
		MEREQUEST *p = gReqHead;
		MsgOut("\r\n[ME]: current request queue:\r\n");
		while(p)
		{
			MsgOut("0x%08x = %03d, ", p, p->cmd);
			p = p->next;
		}
		MsgOut("\r\n[ME]: pTail = 0x%08x\r\n", gReqTail);
	}
#endif
	
    if ( !gInitDone )
    {
        if ( event != DATAIN )
            return 0;

        if ( memcmp( data, "OK", 2 ) == 0 )
        {
            gInitDone = 1;
            PostMessage( ME_hWnd, 0, 0, 0 );
        }

        return 0;
    }

    if( FSM_STATE_INIT == gFsmState && DATAIN == event )
		return -1;
   
    gReqHead->result_len = -1;

    switch ( gReqHead->cmd )
    {
    case ME_INIT_EQUIP:
    case ME_RESET_EQUIP:
    case ME_INIT_EVERYTIME:

        if ( event == DATAIN )
            retval = IsFinalResultCode (data, datalen);

        retval = Me_Fsm_Init (event, retval, data, datalen);
        break;

	case ME_INIT_INITSTK:
		if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send (1, "AT^SSTA=1,1\r", -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
		break;

    case ME_INIT_INITRATE:
        return FSM_InitRate(event,data,datalen);

    case ME_STK_SEND:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                char* cmd = ((STK_PARAM*)(gReqHead->cmddata))->cmd;
                int  timeout = ((STK_PARAM*)(gReqHead->cmddata))->timeout;

                if ( cmd == NULL )
                    goto ERROR_HANDLER;

                retval = MAT_Send( 1, cmd, strlen(cmd) );
                gTimerID = SetTimer( NULL, 0, timeout, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            static int index = 0;
            char* p = NULL;
            int count = ((STK_PARAM*)(gReqHead->cmddata))->line_count;
            int len = ((STK_PARAM*)(gReqHead->cmddata))->line_len;
            char* linebreak;

            if ( DATAIN == event )
            {
                if ( strstr( data, "^SSTGI:") == data || strstr( data, "^SSTR:") == data )
                {
                    data[datalen-1] = '\n';
                    p = (char*)(((STK_PARAM*)(gReqHead->cmddata))->revbuf) + index*len;

                    linebreak = strchr( data, '\r' );
                    
                    if ( linebreak != NULL )
                    {
                        *linebreak = '\0';
                        datalen = linebreak-data;
                    }

                    if ( index >= count )
                    {
                        p -= count;
                        strcpy( p, "MEMFULL" );
                    }

                    if ( datalen >= len )
                    {
						if (datalen >= len * count)
						{
							memcpy( p, data, len * count -1 );
							p[len * count -1] = '\0';
						}
						else
						{
							memcpy( p, data, datalen);
							p[datalen] = '\0';
						}
                    }
                    else
                        memcpy( p, data, datalen+1 );

                    index++;

                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                //p = (char*)(((STK_PARAM*)(gReqHead->cmddata))->revbuf) + index*len;
                //memcpy( p, data, datalen );
                //p[datalen] = '\0';
                index = 0;

                KillTimer(NULL, gTimerID);
                gFsmState = FSM_STATE_DONE;
            }
            else if  ( TIMEOUT == event )
            {
                p = (char*)(((STK_PARAM*)(gReqHead->cmddata))->revbuf) + index*len;
                strcpy( p, "TIMEOUT" );
                goto TIMEOUT_HANDLER;
            }
        }

        break;

    case ME_STK_GETSAT:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT^SSTA?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "^SSTA:") == data )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_STK_RETURNIDLE:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT^SSTA=1,1\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }        		
        break;


    case ME_SSCALL:

        return FSM_SpecialCall (event, data, datalen);
#if 0
    case ME_USSD:

        return FSM_USSD (event, data, datalen);
#endif

    case ME_VOICE_DIAL:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                char* cmddata = gReqHead->cmddata;

                if ( gReqHead->cmddata_len == (int)strlen(cmddata) )
                    sprintf( ATCmd, "ATD%s;\r", cmddata );
                else
                    sprintf( ATCmd, "ATD%s+%s;\r", cmddata, &cmddata[strlen(cmddata)+1] );

                retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                gTimerID = SetTimer( NULL, 0, 120000, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( USERSTOP == event )
            {
                retval = MAT_Send( 1, "B\r", strlen("B\r") );

                KillTimer( NULL, gTimerID );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP2;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        else if ( FSM_STATE_STEP2 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer( NULL, gTimerID );
                if ( retval == FRC_OK || retval == FRC_NOCARRIER )
                    gReqHead->error = ME_RS_USERSTOP;
                else
                    gReqHead->error = ME_RS_FAILURE;

                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_VOICE_DATADIAL:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                char* cmddata = gReqHead->cmddata;

                if ( gReqHead->cmddata_len == (int)strlen(cmddata) )
                    sprintf( ATCmd, "ATD%s\r", cmddata );
                else
                    sprintf( ATCmd, "ATD%s+%s\r", cmddata, &cmddata[strlen(cmddata)+1] );

                retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                gTimerID = SetTimer( NULL, 0, 120000, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( USERSTOP == event )
            {
                retval = MAT_Send( 1, "B\r", strlen("B\r") );

                KillTimer( NULL, gTimerID );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP2;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        else if ( FSM_STATE_STEP2 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer( NULL, gTimerID );
                if ( retval == FRC_OK || retval == FRC_NOCARRIER )
                    gReqHead->error = ME_RS_USERSTOP;
                else
                    gReqHead->error = ME_RS_FAILURE;

                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

	case ME_GET_SALSstatus:
		
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT^SALS?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "^SALS:") == data )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

	case ME_SET_OUTLINE:
		if ( FSM_STATE_INIT == gFsmState )
		{
			if ( START == event )
			{
				sprintf( ATCmd, "AT^SALS=%d,%d\r", gReqHead->arg2, gReqHead->arg1 );
				retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
				gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				gFsmState = FSM_STATE_STEP1;
			}
		}		
		else if ( FSM_STATE_STEP1 == gFsmState )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, gTimerID);
				GetResultLength( gReqHead );
				gFsmState = FSM_STATE_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}		
		break;
		


    case ME_VOICE_HANGUP:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "ATH\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );
                
                if ( retval < 0 )
                    return retval;
        
                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;


    case ME_VOICE_ANSWER:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "ATA\r", strlen("ATA\r") );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );
                
                if ( retval < 0 )
                    return retval;
        
                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_CURRENT_CALLS:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT+CLCC\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
				MsgOut("\r\n[ME]: list current calls in channel 1, set timer = %d\r\n", gTimerID);
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "+CLCC:") == data )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;
                if ( !(gReqHead->error == ME_RS_SUCCESS || gReqHead->error == ME_RS_FAILURE))
                    return -1;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );

				if(gReqHead->result_len > 0)
				{
					gFsmState = FSM_STATE_DONE;
					MsgOut("\r\n[ME]: list current calls in channel 1, OK\r\n");
				}
				else
				{
					if(FSM_SwitchCH0(TRUE) < 0)
					{
						gFsmState = FSM_STATE_DONE;
						MsgOut("\r\n[ME]: Failed to open channel 0, stop list calls\r\n");
					}
					else
					{
						retval = MAT_Send( 0, "AT+CLCC\r", -1 );
						gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						gFsmState = FSM_STATE_STEP2;
						MsgOut("\r\n[ME]: no calls in channel 1, try to list in channel 0\r\n");
					}
				}
            }
		}
        else if ( FSM_STATE_STEP2 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "+CLCC:") == data )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;
                if ( !(gReqHead->error == ME_RS_SUCCESS || gReqHead->error == ME_RS_FAILURE))
                    return -1;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );

				gFsmState = FSM_STATE_DONE;
				FSM_SwitchCH0(FALSE);
				MsgOut("\r\n[ME]: list current calls in channel 0, OK\r\n");
            }
            else if ( TIMEOUT == event )
            {
				FSM_SwitchCH0(FALSE);
				MsgOut("\r\n[ME]: list current calls, TIMEOUT\r\n");
				goto TIMEOUT_HANDLER;
			}
        }
        break;

    case ME_GPRS_MONITOR:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT^SMONG\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                //if ( strstr( data, "GPRS Monitor") == data )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    //return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;
                if ( !(gReqHead->error == ME_RS_SUCCESS || gReqHead->error == ME_RS_FAILURE))
                    return -1;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_SWITCH_CALLLINE:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
				MsgOut("\r\n[ME]: hung up now. ME_SWITCH_CALLLINE ...\r\n");
                switch( gReqHead->arg1 )
                {
                case CUT_HELD:              // 呼叫等待时忙音或者挂断保持的通话

                    strcpy( ATCmd, "AT+CHLD=0\r" );

                    break;

                case CUT_ACTIVED:           // 挂断激活的通话

                    strcpy( ATCmd, "AT+CHLD=1\r" );

                    break;

                case ACTIVE_SWITCH:         // 在保持和激活的通话状态间切换

                    strcpy( ATCmd, "AT+CHLD=2\r" );

                    break;

                case ADD_TO_ACTIVE:         // 将保持的通话加入激活通话

                    strcpy( ATCmd, "AT+CHLD=3\r" );

                    break;

                case CUT_ALL:               // 挂断所有保持和激活的通话

                    strcpy( ATCmd, "AT+CHUP\r" );

                    break;

                case CALL_TRANSFER:

                    strcpy (ATCmd, "ATD4;\r");

                    break;

                case CUT_ACTIVED_X:         // 挂断指定序号的激活通话

                    if ( gReqHead->arg2 < 8 && gReqHead->arg2 > 0 )
                        sprintf( ATCmd, "AT+CHLD=1%d\r", gReqHead->arg2 );
                    else
                        goto ERROR_HANDLER;

                    break;

                case ACTIVE_SWITCH_X:       // 将除指定序号外的所有通话变为保持

                    if ( gReqHead->arg2 < 8 && gReqHead->arg2 > 0 )
                        sprintf( ATCmd, "AT+CHLD=2%d\r", gReqHead->arg2 );
                    else
                        goto ERROR_HANDLER;

                    break;

                default:

                    goto ERROR_HANDLER;
                }

                retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
				MsgOut("\r\n[ME]: hung up now. set timer = %d\r\n", gTimerID);
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                if ( !(gReqHead->error == ME_RS_SUCCESS || gReqHead->error == ME_RS_FAILURE))
                    return -1;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
				MsgOut("\r\n[ME]: hung up now. Kill timer = %d\r\n", gTimerID);
            }
            else if ( TIMEOUT == event )
			{
				MsgOut("\r\n[ME]: hung up ... TIMEOUT !!!\r\n");
                goto TIMEOUT_HANDLER;
			}
        }

        break;

    case ME_SET_AUTOANSWER:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                if ( gReqHead->arg1 < 0 )
                    goto ERROR_HANDLER;

                sprintf( ATCmd, "ATS0=%d\r", gReqHead->arg1 );

                retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_GET_AUTOANSWER:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "ATS0?\r", strlen("ATS0?\r") );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;
        
    case ME_SET_CALLWAITING:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                switch ( gReqHead->arg2 )
                {
                case CLASS_VOICE:

                    if ( (BOOL)(gReqHead->arg1) )
                        strcpy( ATCmd, "AT+CCWA=1,1,1\r" );
                    else
                        strcpy( ATCmd, "AT+CCWA=0,0,1\r" );

                    break;
                case CLASS_DATA:

                    if ( (BOOL)(gReqHead->arg1) )
                        strcpy( ATCmd, "AT+CCWA=1,1,2\r" );
                    else
                        strcpy( ATCmd, "AT+CCWA=0,0,2\r" );

                    break;

                case CLASS_FAX:

                    if ( (BOOL)(gReqHead->arg1) )
                        strcpy( ATCmd, "AT+CCWA=1,1,4\r" );
                    else
                        strcpy( ATCmd, "AT+CCWA=0,0,4\r" );

                    break;

                case CLASS_SMS:

                    if ( (BOOL)(gReqHead->arg1) )
                        strcpy( ATCmd, "AT+CCWA=1,1,8\r" );
                    else
                        strcpy( ATCmd, "AT+CCWA=0,0,8\r" );

                    break;

                case CLASS_ALL:

                    if ( (BOOL)(gReqHead->arg1) )
                        strcpy( ATCmd, "AT+CCWA=1,1,7\r" );
                    else
                        strcpy( ATCmd, "AT+CCWA=0,0,7\r" );

                    break;

                default:
                    goto ERROR_HANDLER;
                }

                retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_GET_CALLWAITING:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                switch ( gReqHead->arg1 )
                {
                case CLASS_VOICE:

                    strcpy( ATCmd, "AT+CCWA=1,2,1\r" );

                    break;
                case CLASS_DATA:

                    strcpy( ATCmd, "AT+CCWA=1,2,2\r" );

                    break;

                case CLASS_FAX:

                    strcpy( ATCmd, "AT+CCWA=1,2,4\r" );

                    break;

                case CLASS_SMS:

                    strcpy( ATCmd, "AT+CCWA=1,2,8\r" );

                    break;

				case CLASS_ALL:
					strcpy ( ATCmd, "AT+CCWA=1,2,7\r");
					break;

                default:
                    goto ERROR_HANDLER;
                }

                retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "+CCWA:") == data )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_SET_CALLFWDING:

        return FSM_SetCallFwding( event, data, datalen );

    case ME_GET_CALLFWDING:
        
        return FSM_GetCallFwding( event, data, datalen );

    case ME_SET_CALLBARRING:
        return FSM_SetCallBarring( event, data, datalen );

    case ME_GET_CALLBARRING:
        return FSM_GetCallBarring( event, data, datalen );

    case ME_SMS_SEND:
        return FSM_SMSSend( event, data, datalen );

	case ME_SMS_WRITE:
		return FSM_SMSWrite( event, data, datalen );
    case ME_SMS_READ:
        return FSM_SMSRead( event, data, datalen );

    case ME_SMS_DELETE:
        return FSM_SMSDelete( event, data, datalen );

    case ME_SMS_LIST:
        return FSM_SMSList( event, data, datalen );

    case ME_SMS_ACK:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT+CNMA\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_SMS_SET_SCA:

        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
                retval = MAT_Send (1, "AT+CSCS=\"GSM\"\r", -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {
                retval = GetResultErrorCode( data, datalen );
                if ( retval < 0 )
                    return retval;
                KillTimer( NULL, gTimerID );
                if ( retval == FRC_OK) {
                    sprintf (ATCmd, "AT+CSCA=\"%s\"\r", gReqHead->cmddata);
                    retval = MAT_Send (1, ATCmd, strlen(ATCmd));
                    gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                    gFsmState = FSM_STATE_STEP2;
                }
                else {
                    GetResultLength (gReqHead);
                    gFsmState = FSM_STATE_DONE;
                }
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        else if ( FSM_STATE_STEP2 == gFsmState ) {
            if ( DATAIN == event ) {
                retval = GetResultErrorCode( data, datalen );
                if ( retval < 0 )
                    return retval;
                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_SMS_GET_SCA:

        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
                retval = MAT_Send (1, "AT+CSCS=\"GSM\"\r", -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {
                retval = GetResultErrorCode( data, datalen );
                if ( retval < 0 )
                    return retval;
                KillTimer( NULL, gTimerID );
                if ( retval == FRC_OK) {
                    retval = MAT_Send( 1, "AT+CSCA?\r", -1 );
                    gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                    gFsmState = FSM_STATE_STEP2;
                }
                else {
                    GetResultLength( gReqHead );
                    gFsmState = FSM_STATE_DONE;
                }
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        else if ( FSM_STATE_STEP2 == gFsmState ) {
            if ( DATAIN == event ) {
                if ( strstr( data, "+CSCA:") == data && ME_MemGetOffset() == 0 ) {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_SMS_SETINDSTATUS:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                int mt, bm, ds;

                mt = ((PIND_STATUS)(gReqHead->cmddata))->Mt;
                bm = ((PIND_STATUS)(gReqHead->cmddata))->Bm;
                ds = ((PIND_STATUS)(gReqHead->cmddata))->Ds;

                strcpy (ATCmd, "AT+CNMI=2,");

                if ( mt == IND_NONE )
                    strcat (ATCmd, "0,");
                else if ( mt == IND_BUFFER )
                    strcat (ATCmd, "1,");
                else if ( mt == IND_DIRECT )
                    strcat (ATCmd, "2,");
                else
                    goto ERROR_HANDLER;

                if ( bm == IND_NONE )
                    strcat (ATCmd, "0,");
                else if ( bm == IND_DIRECT )
                    strcat (ATCmd, "2,");
                else
                    goto ERROR_HANDLER;

                if ( ds == IND_NONE )
                    strcat (ATCmd, "0,");
                else if ( ds == IND_BUFFER )
                    strcat (ATCmd, "2,");
                else if ( ds == IND_DIRECT )
                    strcat (ATCmd, "1,");
                else
                    goto ERROR_HANDLER;

                strcat (ATCmd, "1\r");

                retval = MAT_Send (1, ATCmd, strlen(ATCmd));
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP2;
            }
       }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);

                if ( retval == FRC_OK )
                {
                    int mt, bm, ds;

                    mt = ((PIND_STATUS)(gReqHead->cmddata))->Mt;
                    bm = ((PIND_STATUS)(gReqHead->cmddata))->Bm;
                    ds = ((PIND_STATUS)(gReqHead->cmddata))->Ds;

                    strcpy (ATCmd, "AT+CNMI=2,");

                    if ( mt == IND_NONE )
                        strcat (ATCmd, "0,");
                    else if ( mt == IND_BUFFER )
                        strcat (ATCmd, "1,");
                    else if ( mt == IND_DIRECT )
                        strcat (ATCmd, "2,");
                    else
                        goto ERROR_HANDLER;

                    if ( bm == IND_NONE )
                        strcat (ATCmd, "0,");
                    else if ( bm == IND_DIRECT )
                        strcat (ATCmd, "2,");
                    else
                        goto ERROR_HANDLER;

                    if ( ds == IND_NONE )
                        strcat (ATCmd, "0,");
                    else if ( ds == IND_BUFFER )
                        strcat (ATCmd, "2,");
                    else if ( ds == IND_DIRECT )
                        strcat (ATCmd, "1,");
                    else
                        goto ERROR_HANDLER;

                    strcat( ATCmd, "1\r" );

                    retval = MAT_Send (1, ATCmd, strlen(ATCmd));
                    gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                    gFsmState = FSM_STATE_STEP2;
                }
                else
                {
                    GetResultLength (gReqHead);
                    gFsmState = FSM_STATE_DONE;
                }
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        else if ( FSM_STATE_STEP2 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);

                if ( retval == FRC_OK )
                {
                    retval = MAT_Send (1, "AT&W\r", -1);
                    gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                    gFsmState = FSM_STATE_STEP4;
                }
                else
                {
					if (gReqHead->errcode == 500)
					{
						MAT_Send (0, "AT&F\r", -1);
						MAT_Send (1, "AT\r", -1);//"AT&F\r"
						MAT_Send (2, "AT&F\r", -1);
						gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
						gFsmState = FSM_STATE_STEP1;
					}
					else
					{
						GetResultLength (gReqHead);
						gFsmState = FSM_STATE_DONE;
					}
                }
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        else if ( FSM_STATE_STEP3 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);

				if ( retval == FRC_OK ) {
					MAT_Send (1, "AT^SSDA=1\r", -1);
					gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
					gFsmState = FSM_STATE_STEP4;
				}
				else {
	                GetResultLength( gReqHead );
		            gFsmState = FSM_STATE_DONE;
				}
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
		else if ( FSM_STATE_STEP4 == gFsmState ) {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);

                GetResultLength( gReqHead );
	            gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
		}
        break;

    case ME_SMS_GETINDSTATUS:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT+CNMI?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "+CNMI:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_SET_CELLBROADCAST:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                BOOL mode;
                unsigned long code;
                char *ids;

                mode = ((PCELL_BROADCAST)(gReqHead->cmddata))->mode;
                ids = ((PCELL_BROADCAST)(gReqHead->cmddata))->ids;
                code = ((PCELL_BROADCAST)(gReqHead->cmddata))->code;

                if ( mode )
                    sprintf (ATCmd, "AT+CSCB=0,\"%s\",\"", ids);
                else
                    sprintf (ATCmd, "AT+CSCB=1,\"%s\",\"", ids);
                
                if ( code & CBS_LANG_GERMAN )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x00);
                if ( code & CBS_LANG_ENGLISH )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x01);
                if ( code & CBS_LANG_ITALIAN )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x01);
                if ( code & CBS_LANG_FRENCH )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x03);
                if ( code & CBS_LANG_SPANISH )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x04);
                if ( code & CBS_LANG_DUTCH )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x05);
                if ( code & CBS_LANG_SWEDISH )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x06);
                if ( code & CBS_LANG_DANISH )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x07);
                if ( code & CBS_LANG_PORTUGUESE )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x08);
                if ( code & CBS_LANG_FINNISH )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x09);
                if ( code & CBS_LANG_NORWEGIAN )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x0a);
                if ( code & CBS_LANG_GREEK )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x0b);
                if ( code & CBS_LANG_TURKISH )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x0c);
                if ( code & CBS_LANG_HANGARIAN )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x0d);
                if ( code & CBS_LANG_POLISH )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x0e);
                if ( code & CBS_LANG_CZECH )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x20);
                if ( code & CBS_LANG_RESERVE )
                    sprintf (&ATCmd[strlen(ATCmd)], "%d,", 0x11);

                if ( code > 0 )
                    sprintf (&ATCmd[strlen(ATCmd)-1], "\"\r");
                else 
                    strcat (ATCmd, "\"\r");

                retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_GET_CELLBROADCAST:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT+CSCB?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "+CSCB:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_SMS_MEMCOUNT:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                switch( gReqHead->arg1 )
                {
                case SMS_MEM_SM:

                    strcpy( ATCmd, "AT+CPMS=\"SM\",\"SM\",\"MT\"\r" );

                    break;

                case SMS_MEM_ME:

                    strcpy( ATCmd, "AT+CPMS=\"ME\",\"ME\",\"MT\"\r" );

                    break;

                case SMS_MEM_MT:

                    strcpy( ATCmd, "AT+CPMS=\"MT\",\"MT\",\"MT\"\r" );

                    break;

                default:
                    goto ERROR_HANDLER;
                }

                retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "+CPMS:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_PBOOK_READ:
        return FSM_PBookRead( event, data, datalen );

    case ME_PBOOK_WRITE:
        {
            static int index, ret;
            static char *PhoneNum, *Text;
            static char phonetext[100];
            if ( FSM_STATE_INIT == gFsmState )
            {
                if ( START == event ) {
                    switch ( gReqHead->arg1 )
                    {
                    case PBOOK_SM:
                        retval = MAT_Send( 1, "AT+CPBS=\"SM\"\r", -1 );
                        break;
                    case PBOOK_ON:
                        retval = MAT_Send (1, "AT+CPBS=\"ON\"\r", -1);
                        break;
                    case PBOOK_FD:
                        retval = MAT_Send (1, "AT+CPBS=\"FD\"\r", -1);
                        break;
                    default:
                        goto ERROR_HANDLER;
                    }
                    gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                    gFsmState = FSM_STATE_STEP1;
                }
            }
            else if ( FSM_STATE_STEP1 == gFsmState ) {
                if ( DATAIN == event )
                {
                    retval = GetResultErrorCode (data, datalen);
                    if ( retval < 0 )
                        return retval;
                    KillTimer (NULL, gTimerID);
                    if ( retval == FRC_OK ) {
                        index = ((PPHONEBOOK)(gReqHead->cmddata))->Index;
                        PhoneNum = ((PPHONEBOOK)(gReqHead->cmddata))->PhoneNum;
                        Text = ((PPHONEBOOK)(gReqHead->cmddata))->Text;
                        if ( strlen(PhoneNum)==0 && strlen(Text)==0 )
                        {
                            sprintf( ATCmd, "AT+CPBW=%d\r", index );
                            retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                            gFsmState = FSM_STATE_STEP3;
                        }
                        else if( strlen(Text)==0 )
                        {
                            sprintf( ATCmd, "AT+CPBW=%d,\"%s\",,\"\"\r", index, PhoneNum );
                            retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                            gFsmState = FSM_STATE_STEP3;				
                        }                        
                        else
                        {
                            ret = PhoneText_Encode( phonetext, Text, 100 );
                            if ( strlen(phonetext) == 0 )
                                goto ERROR_HANDLER;
                            if ( ret == 0 )
                                strcpy( ATCmd, "AT+CSCS=\"GSM\"\r" );
                            else
                                strcpy( ATCmd, "AT+CSCS=\"UCS2\"\r" );
                            retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                            gFsmState = FSM_STATE_STEP2;
                        }
                    }
                    else
                    {
                        GetResultLength( gReqHead );
                        gFsmState = FSM_STATE_DONE;
                    }
                }
                else if ( TIMEOUT == event )
                    goto TIMEOUT_HANDLER;
            }
            else if ( FSM_STATE_STEP2 == gFsmState )
            {
                if ( DATAIN == event )
                {
                    retval = GetResultErrorCode( data, datalen );
                    if ( retval < 0 )
                        return retval;
                    KillTimer(NULL, gTimerID);
                    if ( retval == FRC_OK )
                    {
                        sprintf( ATCmd, "AT+CPBW=%d,\"%s\",,\"%s\"\r", index, PhoneNum, phonetext );
                        retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                        gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                        gFsmState = FSM_STATE_STEP3;
                    }
                    else
                        goto ERROR_HANDLER;
                }
                else if ( TIMEOUT == event )
                    goto TIMEOUT_HANDLER;
            }
            else if ( FSM_STATE_STEP3 == gFsmState )
            {
                if ( DATAIN == event )
                {
                    retval = GetResultErrorCode( data, datalen );
                    if ( retval < 0 )
                        return retval;
                    KillTimer(NULL, gTimerID);
                    GetResultLength( gReqHead );
                    gFsmState = FSM_STATE_DONE;
                }
                else if ( TIMEOUT == event )
                    goto TIMEOUT_HANDLER;
            }
        }
        break;
    
    case ME_PBOOK_MEMCOUNT:
        
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                switch ( gReqHead->arg1 )
                {
                case PBOOK_SM:
                    retval = MAT_Send (1, "AT+CPBS=\"SM\"\r", -1);
                    break;
                case PBOOK_LD:
                    retval = MAT_Send (1, "AT+CPBS=\"LD\"\r", -1);
                    break;
                case PBOOK_MC:
                    retval = MAT_Send (1, "AT+CPBS=\"MC\"\r", -1);
                    break;
                case PBOOK_RC:
                    retval = MAT_Send (1, "AT+CPBS=\"RC\"\r", -1);
                    break;
                case PBOOK_FD:
                    retval = MAT_Send (1, "AT+CPBS=\"FD\"\r", -1);
                    break;
                case PBOOK_ON:
                    retval = MAT_Send (1, "AT+CPBS=\"ON\"\r", -1);
                    break;
                default:
                    goto ERROR_HANDLER;
                }
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer( NULL, gTimerID );

                if ( retval == FRC_OK )
                {
                    retval = MAT_Send( 1, "AT+CPBS?\r", -1 );
                    gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                    gFsmState = FSM_STATE_STEP2;
                }
                else
                {
                    GetResultLength( gReqHead );
                    gFsmState = FSM_STATE_DONE;
                }
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        else if ( FSM_STATE_STEP2 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "+CPBS:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer( NULL, gTimerID );
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_GET_PHONEBOOKNUMBERTEXTMAXLEN:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send (1, "AT+CPBR=?\r", -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr(data, "+CPBR:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign (data, datalen);
                    return 0;
                }
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;
    case ME_SET_LOCK:

        return FSM_SetPhoneLock( event, data, datalen );

    case ME_GET_LOCK:
        
        return FSM_GetPhoneLock( event, data, datalen );

    case ME_PASS_CHANGE:

        return FSM_ChangePassword( event, data, datalen );

    case ME_PASS_VALID:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                char* password, *newpass;

                password = gReqHead->cmddata;
                
                if ( gReqHead->cmddata_len == (int)strlen(password) )
                    newpass = NULL;
                else
                    newpass = &password[strlen(password)+1];

                if ( gReqHead->arg1 == PUK1 || gReqHead->arg1 == PUK2 )
                {
                    if ( newpass == NULL )
                        goto ERROR_HANDLER;
                }

                if ( gReqHead->arg1 == PIN2 || gReqHead->arg1 == PUK2 )
                    strcpy( ATCmd, "AT+CPIN2=" );
                else
                    strcpy( ATCmd, "AT+CPIN=");

                strcat( ATCmd, password );
                if ( newpass != NULL )
                {
                    strcat( ATCmd, "," );
                    strcat( ATCmd, newpass );
                }
                strcat( ATCmd, "\r" );

                retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                gTimerID = SetTimer( NULL, 0, 10000, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_GET_PASS:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT+CPIN?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "+CPIN:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;
                
                KillTimer(NULL, gTimerID);
                if ( gReqHead->errcode == 256 ) {
                    MAT_Send( 1, "A/", 2 );
                    gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                    return 0;
                }
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_PASS_REMAIN:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                switch(gReqHead->arg1)
                {
                case PIN1:
                case PUK1:
                    retval = MAT_Send( 1, "AT^SPIC=\"SC\"\r", -1 );
                    break;
                case PIN2:
                case PUK2:
                    retval = MAT_Send( 1, "AT^SPIC=\"P2\"\r", -1 );
                    break;
                default:
                    retval = MAT_Send( 1, "AT^SPIC\r", -1 );
                    break;
                }
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "^SPIC:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_GETSIMCARDSTATUS:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT^SCKS?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "^SCKS:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_CURPASSWORD_STATUS:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT^SPIC?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "^SPIC:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_SET_OPRATOR:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                switch ( gReqHead->arg1 )
                {
                case MODE_AUTO:

                    strcpy( ATCmd, "AT+COPS=0\r" );
                    break;

                case MODE_UNREGIST:

                    strcpy( ATCmd, "AT+COPS=2\r" );
                    break;

                case MODE_MANUAL:

                    if ( gReqHead->cmddata == NULL )
                        goto ERROR_HANDLER;

                    sprintf( ATCmd, "AT+COPS=1,2,%s\r", gReqHead->cmddata );
                    break;

                default:
                    goto ERROR_HANDLER;
                }

                retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_GET_OPRATOR:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                switch ( gReqHead->arg1 )
                {
                case FORMAT_LONGALPHA:
                    retval = MAT_Send( 1, "AT+COPS=3,0\r", -1 );
                    break;
                case FORMAT_NUMERIC:
                    retval = MAT_Send( 1, "AT+COPS=3,2\r", -1 );
                    break;
                default:
                    goto ERROR_HANDLER;
                }
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer( NULL, gTimerID );

                if ( retval == FRC_OK)
                {
                    retval = MAT_Send( 1, "AT+COPS?\r", -1 );
                    gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                    gFsmState = FSM_STATE_STEP2;
                }
                else
                {
                    GetResultLength( gReqHead );
                    gFsmState = FSM_STATE_DONE;
                }
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        else if ( FSM_STATE_STEP2 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "+COPS:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer( NULL, gTimerID );
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_SRCH_OPRATOR:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT+COPS=?\r", -1 );
                gTimerID = SetTimer( NULL, 0, 60000, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr (data, "+COPS:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign (data, datalen);
                    return 0;
                }
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( USERSTOP == event )
            {
                retval = MAT_Send (1, "B\r", -1);
                KillTimer (NULL, gTimerID);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP2;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        else if ( FSM_STATE_STEP2 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode (data, datalen);
                
                if ( retval < 0 )
                    return retval;

                KillTimer (NULL, gTimerID);
                if ( retval == FRC_OK )
                    gReqHead->error = ME_RS_USERSTOP;

                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_GET_POPRATOR:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                int begin = gReqHead->arg1;
                int end = gReqHead->arg2;

                if ( begin == 0 )
                    goto ERROR_HANDLER;

                if ( end == 0 )
                    sprintf (ATCmd, "AT^SPLR=%d\r", begin);
                else
                    sprintf (ATCmd, "AT^SPLR=%d,%d\r", begin, end);

                retval = MAT_Send (1, ATCmd, strlen(ATCmd));
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr (data, "^SPLR:") == data )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign (data, datalen);
                    return 0;
                }
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_SET_POPRATOR:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                int index;
                char *oper;

                index = gReqHead->arg1;
                oper = gReqHead->cmddata;

                if ( index == 0 )
                    goto ERROR_HANDLER;

                if ( oper == NULL )
                    sprintf (ATCmd, "AT^SPLW=%d\r", index);
                else
                    sprintf (ATCmd, "AT^SPLW=%d,%s\r", index, oper);

                retval = MAT_Send (1, ATCmd, strlen(ATCmd));
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_GET_IMEI:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send (1, "AT+CGSN\r", -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( memchr (data, '+', datalen) != NULL )
                    return -1;

                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign (data, datalen);
                    return 0;
                }

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_GET_IMSI:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send (1, "AT+CIMI\r", -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( memchr (data, '+', datalen) != NULL )
                    return -1;

                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign (data, datalen);
                    return 0;
                }

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_SET_CLOCK:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                sprintf (ATCmd, "AT+CCLK=\"%s\"\r", gReqHead->cmddata);

                retval = MAT_Send (1, ATCmd, strlen(ATCmd));
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_GET_CLOCK:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send (1, "AT+CCLK?\r", -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr(data, "+CCLK:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign (data, datalen);
                    return 0;
                }
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_MUTE_CONTROL:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                if ( gReqHead->arg1 )
                    strcpy (ATCmd, "AT+CMUT=1\r");
                else
                    strcpy (ATCmd, "AT+CMUT=0\r");

                retval = MAT_Send (1, ATCmd, strlen(ATCmd));
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_SET_VOLUME:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                if ( gReqHead->arg1 < 0 || gReqHead->arg1 > 4 )
                    goto ERROR_HANDLER;

                sprintf (ATCmd, "AT+CLVL=%d\r", gReqHead->arg1);

                retval = MAT_Send (1, ATCmd, strlen(ATCmd));
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_SET_VOLUMESNFV:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                if ( gReqHead->arg1 < 0 || gReqHead->arg1 > 4 )
                    goto ERROR_HANDLER;

                sprintf (ATCmd, "AT^SNFV=%d\r", gReqHead->arg1);

                retval = MAT_Send (1, ATCmd, strlen(ATCmd));
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;
    case ME_GET_VOLUME:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send (1, "AT+CLVL?\r", -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr(data, "+CLVL:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_GET_VOLUMESNFV:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send (1, "AT^SNFV?\r", -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr(data, "^SNFV:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_SEND_DTMF:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                sprintf (ATCmd, "AT+VTS=%c\r", gReqHead->arg1);

                retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_FAC_SETTING:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT&F\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_RESET:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT+CFUN=1,1\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_SLEEP:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT+CFUN=7\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_SHUTDOWN:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT^SMSO\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_GET_SIGNAL:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT+CSQ\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "+CSQ:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_SETBATTERYCHARGECONTROL:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                sprintf(ATCmd,"AT^SBC=%d\r",gReqHead->arg1);
                retval = MAT_Send( 1, ATCmd, -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );
                if ( retval < 0 )
                    return retval;
                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;
//fufu 0704
    case ME_GET_CHARGE:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT^SBC?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "^SBC:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;
//end 0704

    case ME_GET_BATTERY:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT^SBV\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "^SBV:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

	case ME_GET_MODULEINFO:
		
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "ATI\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
				if( strstr(data, "SIEMENS") == data ||
					strstr(data, "MC55") == data ||
					strstr(data, "REVISION") == data )
				{
					data[datalen-1] = '\n';
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
		
		break;

    case ME_GETCCM:

        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
                retval = MAT_Send (1, "AT+CAOC\r", -1);
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {
                if ( strstr (data, "+CAOC:") == data && ME_MemGetOffset() == 0 ) {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_GETACM:

        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
                retval = MAT_Send (1, "AT+CACM?\r", -1);
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {
                if ( strstr (data, "+CACM:") == data && ME_MemGetOffset() == 0 ) {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

	case ME_SETCCM:

		if ( FSM_STATE_INIT == gFsmState ) {
			if ( START == event ) {
			//	char * pin2 = gReqHead->cmddata;
				sprintf (ATCmd, "AT+CAOC=%d\r", gReqHead->arg1);
				retval = MAT_Send (1, ATCmd, -1);
				gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				gFsmState = FSM_STATE_STEP1;
			}
		}
		else if ( FSM_STATE_STEP1 == gFsmState ) {
			if ( DATAIN == event ) {
				retval = GetResultErrorCode( data, datalen );
				if ( retval < 0 )
					return retval;
				KillTimer(NULL, gTimerID);
				GetResultLength( gReqHead );
				gFsmState = FSM_STATE_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_TESTACMMAX:

		if ( FSM_STATE_INIT == gFsmState ) {
			if ( START == event ) {
				strcpy(ATCmd, "AT+CAMM=?\r");
				retval = MAT_Send (1, ATCmd, -1);
				gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				gFsmState = FSM_STATE_STEP1;
			}
		}
		else if ( FSM_STATE_STEP1 == gFsmState ) {
			if ( DATAIN == event ) {
				retval = GetResultErrorCode( data, datalen );
				if ( retval < 0 )
					return retval;
				KillTimer(NULL, gTimerID);
				GetResultLength( gReqHead );
				gFsmState = FSM_STATE_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;
		
    case ME_GETACMAX:

        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
                retval = MAT_Send (1, "AT+CAMM?\r", -1);
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {
                if ( strstr (data, "+CAMM:") == data && ME_MemGetOffset() == 0 ) {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_RESETACM:

        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
                char * pin2 = gReqHead->cmddata;
                sprintf (ATCmd, "AT+CACM=\"%s\"\r", pin2);
                retval = MAT_Send (1, ATCmd, -1);
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {
                retval = GetResultErrorCode( data, datalen );
                if ( retval < 0 )
                    return retval;
                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_SETACMAX:

        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
                char * pin2 = gReqHead->cmddata;
                unsigned long acmax = gReqHead->arg1;
                char * pacmax = AOCHexEncode (acmax);

                sprintf (ATCmd, "AT+CAMM=\"%s\",\"%s\"\r", pacmax, pin2);
                retval = MAT_Send (1, ATCmd, -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {
                retval = GetResultErrorCode( data, datalen );
                if ( retval < 0 )
                    return retval;
                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

	case ME_TESTSACM:

        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
                retval = MAT_Send (1, "A^SACM=?\r", -1);
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {
                if ( strstr (data, "^SACM:") == data && ME_MemGetOffset() == 0 ) {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_GETSACM:
        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
                retval = MAT_Send (1, "AT^SACM\r", -1);
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {
                if ( strstr (data, "^SACM:") == data && ME_MemGetOffset() == 0 ) {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

	case ME_SETSACM:

		if ( FSM_STATE_INIT == gFsmState ) {
			if ( START == event ) {
				sprintf (ATCmd, "AT^SACM=%d\r", gReqHead->arg1);
				retval = MAT_Send (1, ATCmd, -1);
				gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				gFsmState = FSM_STATE_STEP1;
			}
		}
		else if ( FSM_STATE_STEP1 == gFsmState ) {
			if ( DATAIN == event ) {
				retval = GetResultErrorCode( data, datalen );
				if ( retval < 0 )
					return retval;
				KillTimer(NULL, gTimerID);
				GetResultLength( gReqHead );
				gFsmState = FSM_STATE_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

    case ME_GETPUC:
        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
                retval = MAT_Send (1, "AT+CPUC?\r", -1);
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {
                if ( strstr (data, "+CPUC:") == data && ME_MemGetOffset() == 0 ) {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

   case ME_GETGPRSAttachSta:
        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
                retval = MAT_Send (1, "AT+CGATT?\r", -1);
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {
                if ( strstr (data, "+CGATT:") == data && ME_MemGetOffset() == 0 ) {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }   	
   	break;

    case ME_SETGPRSAttach:
        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
		  if( 1 == gReqHead->arg1 )		
                	retval = MAT_Send (1, "AT+CGATT=1\r", -1);			//attach GPRS
		  else if( 0 == gReqHead->arg1 )
		  	retval = MAT_Send (1, "AT+CGATT=0\r", -1);			//detach GPRS
		  
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {
                retval = GetResultErrorCode( data, datalen );
                if ( retval < 0 )
                    return retval;
                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }   		
	break;
	
    case ME_SETPUC:

        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
				int i;
				char gsm_ch[16] = "";
				PME_PUCINFO puc = (PME_PUCINFO)gReqHead->cmddata;
			//	char * pin2 = &(((char *)(gReqHead->cmddata))[sizeof(*puc)]);
				char * pin2 = &(((char *)(gReqHead->cmddata))[sizeof(ME_PUCINFO)]);

				// convert to GSM alphabet. but this is not quite reliable
				for(i=0; i<(int)strlen(puc->currency); i++)
				{
					if(i >3)
						break;
					sprintf(gsm_ch + 4*i, "%04x", puc->currency[i]);
				}
			//	sprintf (ATCmd, "AT+CPUC=\"%s\",\"%s\",\"%s\"\r", puc->currency, puc->ppu, pin2);
				sprintf(ATCmd, "AT+CPUC=\"%s\",\"%s\",\"%s\"\r", gsm_ch, puc->ppu, pin2);
				retval = MAT_Send (1, ATCmd, -1);
				gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
				gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {
                retval = GetResultErrorCode( data, datalen );
                if ( retval < 0 )
                    return retval;
                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_SETALARM:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                char timestr[40];
                ME_ALARMTIME* time = (ME_ALARMTIME*)gReqHead->cmddata;
                strcpy (ATCmd, "AT+CALA=");
                sprintf (timestr, "\"%02d/%02d/%02d,%02d:%02d:%02d\"", 
                    time->year, time->month, time->day, time->hour, time->minute, time->second);
                strcat (ATCmd, timestr);
                strcat (ATCmd, "\r");
                retval = MAT_Send (1, ATCmd, strlen(ATCmd));
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_GETALARM:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT+CALA?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "+CALA:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_AUDIOINTERFACETYPE:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                if ( gReqHead->arg1 == ME_AUDIO_DIGITAL)
                    retval = MAT_Send (1, "AT^SAIC=1\r", -1);
                else
                    retval = MAT_Send (1, "AT^SAIC=2\r", -1);
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );
                
                if ( retval < 0 )
                    return retval;
                
                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        break;

    case ME_AUDIO:
        
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                if ( gReqHead->arg1 == ME_AUDIO_HEADSET)
                    retval = MAT_Send (1, "AT^SNFS=3\r", -1);
                else
                    retval = MAT_Send (1, "AT^SNFS=4\r", -1);
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );
                
                if ( retval < 0 )
                    return retval;
                
                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        
        break;

    case ME_AUDIO_EX:
        
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
				if ( gReqHead->arg1 == ME_AUDIO_BLUETOOTH)
					retval = MAT_Send (1, "AT^SAIC=1\r", -1);
                if ( gReqHead->arg1 == ME_AUDIO_HEADSET)
                    retval = MAT_Send (1, "AT^SAIC=2,1,2\r", -1);
                else
                    retval = MAT_Send (1, "AT^SAIC=2,2,2\r", -1);

                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );
                
                if ( retval < 0 )
                    return retval;
                
                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
        
        break;
    
	case ME_MONI_SERV:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT^MONI;^SMONC\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                {
                    if ( IsUslctResultCode( data, datalen ) < 0 )
                    {
                        data[datalen-1] = '\n';
                        ME_MemAssign( data, datalen );
                        return 0;
                    }
                    else
                        return retval;
                }

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

    case ME_MONI_NEIB:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT^MONP\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                {
                    if ( IsUslctResultCode( data, datalen ) < 0 )
                    {
                        data[datalen-1] = '\n';
                        ME_MemAssign( data, datalen );
                        return 0;
                    }
                    else
                        return retval;
                }

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;

	//fufu 0701 mux
	case ME_CLEARCNMI:
        if ( FSM_STATE_INIT == gFsmState ) {
            if ( START == event ) {
                retval = MAT_Send (1, "AT+CNMI=0,0,0,0,1\r", -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState ) {
            if ( DATAIN == event ) {             
                KillTimer(NULL, gTimerID);

                retval = MAT_Send (2, "AT+CNMI=0,0,0,0,1\r", -1);
//                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);   
                
                GetResultLength( gReqHead );
//                gFsmState = FSM_STATE_STEP2;
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }		
        else if ( FSM_STATE_STEP2 == gFsmState ){
            if ( DATAIN == event ) {       
            	
                KillTimer(NULL, gTimerID);

//                retval = MAT_Send (2, "AT+CNMI=0,0,0,0,1\r", -1);
//                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);   
                
                GetResultLength( gReqHead );
//                gFsmState = FSM_STATE_STEP3;
				gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;			
        }
        else if ( FSM_STATE_STEP3 == gFsmState ){
            if ( DATAIN == event ) {       
            	
                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;			
        }        
		break;
	//end 0701 mux

		//fufu 0630
	case ME_GET_CPHSstatus:
        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send( 1, "AT+CRSM=242\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr( data, "+CRSM:") == data && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode( data, datalen );

                if ( retval < 0 )
                    return retval;

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }
		
		break;
//end 0630		
    case  ME_GET_CPHSInfo:
        return FSM_GetCPHSInfo(event,data,datalen);
//fufu 0619
	case ME_GET_CPHSparas:
    case ME_GET_CPHSparasExt:
		return FSM_GetCPHSparas( event, data, datalen );

	case ME_GET_CONETWORKLIST:
	case ME_READ_CPHSFile:
		return FSM_ReadCPHSFiles( event, data, datalen );

    case ME_READ_CPHSRecordSMS:
	case ME_READ_CPHSRecord:
		return FSM_ReadCPHSRecord( event, data, datalen );
//end 0619

	case ME_UPDATE_CPHSRecord:
		return FSM_UpdateCPHSRecord( event, data, datalen );

	case ME_UPDATE_CPHSFile:
		return FSM_UpdateCPHSFiles( event, data, datalen );	

//fufu 0614
	case ME_GET_SMSSERVICE:
		if ( FSM_STATE_INIT == gFsmState )
		{
			if ( START == event )
			{
				retval = MAT_Send( 1, "AT+CGSMS?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;				
			}
		}
		else if ( FSM_STATE_STEP1 == gFsmState )
		{
			if ( DATAIN == event )
			{
                if ( strstr (data, "+CGSMS:") == data  && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                
				retval = GetResultErrorCode( data, datalen );
				
				if ( retval < 0 )
				{
					return retval;
				}

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
			}
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
            
		}			
		break;
	
	case ME_SET_SMSSERVICE	:
		if ( FSM_STATE_INIT == gFsmState )
		{
			if ( START == event )
			{
				switch( gReqHead->arg1) 
				{
					case SMS_GPRS:
						strcpy( ATCmd, "AT+CGSMS=0\r" );	
						break;
					case SMS_GSM:
						strcpy( ATCmd, "AT+CGSMS=1\r" );	
						break;
					case SMS_GPRSpre:
						strcpy( ATCmd, "AT+CGSMS=2\r" );	
						break;
					case SMS_GSMpre:
						strcpy( ATCmd, "AT+CGSMS=3\r" );	
						break;
					default:
						goto ERROR_HANDLER;
				}

				retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					gFsmState = FSM_STATE_STEP1;
				}
				else
					goto ERROR_HANDLER;			
			}
		}
		else if ( FSM_STATE_STEP1 == gFsmState )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );
				
				if ( retval < 0 )
				{
					return retval;
				}

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
			}
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
            
		}					
		break;
//end 0614


//fufu 0531
	case ME_GET_VMWAIT2:
		if ( FSM_STATE_INIT == gFsmState )
		{
			if ( START == event )
			{
				retval = MAT_Send( 1, "AT^SIND=\"vmwait2\",1\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;				
			}
		}
		else if ( FSM_STATE_STEP1 == gFsmState )
		{
			if ( DATAIN == event )
			{
                if ( strstr (data, "^SIND:") == data  && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                
				retval = GetResultErrorCode( data, datalen );
				
				if ( retval < 0 )
				{
					return retval;
				}

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
			}
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
            
		}		
		break;

	case ME_GET_VMWAIT1:
		if ( FSM_STATE_INIT == gFsmState )
		{
			if ( START == event )
			{
				retval = MAT_Send( 1, "AT^SIND=\"vmwait1\",1\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;				
			}
		}
		else if ( FSM_STATE_STEP1 == gFsmState )
		{
			if ( DATAIN == event )
			{
                if ( strstr (data, "^SIND:") == data  && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                
				retval = GetResultErrorCode( data, datalen );
				
				if ( retval < 0 )
				{
					return retval;
				}

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
			}
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
            
		}		
		break;
		
	case ME_GET_CIPH:
		if ( FSM_STATE_INIT == gFsmState )
		{
			if ( START == event )
			{
				retval = MAT_Send( 1, "AT^SIND=\"ciphcall\",1\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;				
			}
		}
		else if ( FSM_STATE_STEP1 == gFsmState )
		{
			if ( DATAIN == event )
			{
                if ( strstr (data, "^SIND:") == data  && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                
				retval = GetResultErrorCode( data, datalen );
				
				if ( retval < 0 )
				{
					return retval;
				}

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
			}
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
            
		}		
		break;

	case ME_GET_EXOPERATOR:
		if ( FSM_STATE_INIT == gFsmState )
		{
			if ( START == event )
			{
				retval = MAT_Send( 1, "AT^SOPS=?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT+5000, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;				
			}
		}
		else if ( FSM_STATE_STEP1 == gFsmState )
		{
			if ( DATAIN == event )
			{
                if ( strstr (data, "^SOPS:") == data  && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                
				retval = GetResultErrorCode( data, datalen );
				
				if ( retval < 0 )
				{
					return retval;
				}

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
			}
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
            
		}
		break;

	case ME_GET_HOMEZONE:
		if ( FSM_STATE_INIT == gFsmState )
		{
			if ( START == event )
			{
				retval = MAT_Send( 1, "AT^SHOM\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;				
			}
		}
		else if ( FSM_STATE_STEP1 == gFsmState )
		{
			if ( DATAIN == event )
			{
                if ( strstr (data, "^SHOM:") == data  && ME_MemGetOffset() == 0 )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                
				retval = GetResultErrorCode( data, datalen );
				
				if ( retval < 0 )
				{
					return retval;
				}

                KillTimer(NULL, gTimerID);
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
			}
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
            
		}
		break;
//end 0531

    case ME_GETLASTERROR:

        if ( FSM_STATE_INIT == gFsmState )
        {
            if ( START == event )
            {
                retval = MAT_Send (1, "AT+CEER\r", -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP1;
            }
        }
        else if ( FSM_STATE_STEP1 == gFsmState )
        {
            if ( DATAIN == event )
            {
                if ( strstr (data, "+CEER:") == data )
                {
                    data[datalen-1] = '\n';
                    ME_MemAssign( data, datalen );
                    return 0;
                }
                retval = GetResultErrorCode (data, datalen);

                if ( retval < 0 )
                    return retval;

                if ( !(gReqHead->error == ME_RS_SUCCESS || gReqHead->error == ME_RS_FAILURE))
                    return -1;

                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
            else if ( TIMEOUT == event )
                goto TIMEOUT_HANDLER;
        }

        break;
   	case ME_AT_SEND:

		if ( FSM_STATE_INIT == gFsmState )
		{
			if ( START == event )
			{
				char* cmd = gReqHead->cmddata;
				int  timeout = gReqHead->arg1;

				retval = MAT_Send( 1, cmd, strlen(cmd) );

				if ( retval != -1 )
				{
					gTimerID = SetTimer( NULL, 0, timeout, ME_TimerProc );
					gFsmState = FSM_STATE_STEP1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( FSM_STATE_STEP1 == gFsmState )
		{
			if ( DATAIN == event )
			{
				retval = IsUslctResultCode( data, datalen );

				if ( retval >= 0 )
					return retval;

				retval = IsFinalResultCode( data, datalen );

				if ( retval < 0 )
				{
					data[datalen-1] = '\n';
					ME_MemAssign( data, datalen );
					return 0;
				}

				KillTimer(NULL, gTimerID);
				data[datalen-1] = '\n';
				ME_MemAssign( data, datalen );
				GetResultLength( gReqHead );
				gReqHead->error = ME_RS_SUCCESS;
				gReqHead->errcode = 0;
				gFsmState = FSM_STATE_DONE;
			}
			else if  ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_SET_CLIR:

		if ( FSM_STATE_INIT == gFsmState )
		{
			if ( START == event )
			{
				switch( gReqHead->arg1) {
				case CLIR_DEFAULT:
					strcpy( ATCmd, "AT+CLIR=0\r" );
					break;
				case CLIR_ENABLE:
					strcpy( ATCmd, "AT+CLIR=1\r" );
					break;
				case CLIR_DISABLE:
					strcpy( ATCmd, "AT+CLIR=2\r" );
					break;
				}

				retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					gFsmState = FSM_STATE_STEP1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( FSM_STATE_STEP1 == gFsmState )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, gTimerID);
				GetResultLength( gReqHead );
				gFsmState = FSM_STATE_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_GET_CLIR:

		if ( FSM_STATE_INIT == gFsmState )
		{
			if ( START == event )
			{
				retval = MAT_Send( 1, "AT+CLIR?\r", -1 );
				gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				gFsmState = FSM_STATE_STEP1;
			}
		}
		else if ( FSM_STATE_STEP1 == gFsmState )
		{
			if ( DATAIN == event )
			{
				if ( strstr (data, "+CLIR:") == data && ME_MemGetOffset() == 0 )
				{
                    data[datalen-1] = '\n';
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer (NULL, gTimerID);
				GetResultLength (gReqHead);
				gFsmState = FSM_STATE_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;
	default:
		return -1;	
    }

    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}

#define FSM_STATE_CALLCONTROL    1
#define FSM_STATE_SSCONTROL      2
#define SS_TIMEOUT      12000

static int FSM_SpecialCall (int event, char * data, int datalen)
{
    char cmd[ATCMD_BUFSIZE];
    int retval;

    switch (gFsmState) {
    case FSM_STATE_INIT:
        
        if ( event == START ) {
            char * ssCode = gReqHead->cmddata;
            sprintf (cmd, "ATD%s;\r", ssCode);
            MAT_Send (1, cmd, strlen(cmd));
            gTimerID = SetTimer (NULL, 0, SS_TIMEOUT, (TIMERPROC)ME_TimerProc);
            if ( gReqHead->arg1 )
                gFsmState = FSM_STATE_CALLCONTROL;
            else
                gFsmState = FSM_STATE_SSCONTROL;
        }

        break;

    case FSM_STATE_CALLCONTROL:

        if ( event == DATAIN ) {
            retval = GetResultErrorCode (data, datalen);
            if ( retval < 0 )
                return retval;
            if ( !(retval == ME_RS_SUCCESS || retval == ME_RS_FAILURE) )
                return -1;
            KillTimer (NULL, gTimerID);
            GetResultLength (gReqHead);
            gFsmState = FSM_STATE_DONE;
        }
        else if ( event == TIMEOUT ) {
            gReqHead->error = ME_RS_TIMEOUT;
            gReqHead->errcode = 0;
            GetResultLength (gReqHead);
            gFsmState = FSM_STATE_DONE;
        }

        break;

    case FSM_STATE_SSCONTROL:

        if ( event == DATAIN ) {
            if ( memcmp (data, "+CLIP:", 6) == 0 ) {
                if ( gReqHead->arg2 < 0 )
                    gReqHead->arg2 = SSCLIP;
                gReqHead->arg3 ++;
                data[datalen-1] = '\n';
                ME_MemAssign (data, datalen);
                return 0;
            }
            if ( memcmp (data, "+CLIR:", 6) == 0 ) {
                if ( gReqHead->arg2 < 0 )
                    gReqHead->arg2 = SSCLIR;
                gReqHead->arg3 ++;
                data[datalen-1] = '\n';
                ME_MemAssign (data, datalen);
                return 0;
            }
            if ( memcmp (data, "+COLP:", 6) == 0 ) {
                if ( gReqHead->arg2 < 0 )
                    gReqHead->arg2 = SSCOLP;
                gReqHead->arg3 ++;
                data[datalen-1] = '\n';
                ME_MemAssign (data, datalen);
                return 0;
            }
            if ( memcmp (data, "+COLR:", 6) == 0 ) {
                if ( gReqHead->arg2 < 0 )
                    gReqHead->arg2 = SSCOLR;
                gReqHead->arg3 ++;
                data[datalen-1] = '\n';
                ME_MemAssign (data, datalen);
                return 0;
            }
            if ( memcmp (data, "^SCCFC:", 7) == 0 ) {
                if ( gReqHead->arg2 < 0 )
                    gReqHead->arg2 = SSCF;
                gReqHead->arg3 ++;
                data[datalen-1] = '\n';
                ME_MemAssign (data, datalen);
                return 0;
            }
            if ( memcmp (data, "+CCWA:", 6) == 0 ) {
                if ( gReqHead->arg2 < 0 )
                    gReqHead->arg2 = SSCW;
                gReqHead->arg3 ++;
                data[datalen-1] = '\n';
                ME_MemAssign (data, datalen);
                return 0;
            }
            if ( memcmp (data, "^SCLCK:", 7) == 0 ) {
                if ( gReqHead->arg2 < 0 )
                    gReqHead->arg2 = SSCB;
                gReqHead->arg3 ++;
                data[datalen-1] = '\n';
                ME_MemAssign (data, datalen);
                return 0;
            }

            retval = GetResultErrorCode (data, datalen);
            if ( retval < 0 )
                return retval;
            if ( gReqHead->error == ME_RS_SUCCESS ) {
                int size = 0;
                KillTimer (NULL, gTimerID);
                gReqHead->result = ME_MemGetPntr();
                if ( gReqHead->arg2 < 0 ) {
                    gReqHead->errcode = SSUNKNOWN;
                    gReqHead->result_len = -1;
                }
                else {
                    gReqHead->errcode = gReqHead->arg2;
                    switch (gReqHead->arg2) {
                    case SSCLIP:
                        size = sizeof (CLIPINFO);
                        break;
                    case SSCLIR:
                        size = sizeof (CLIRINFO);
                        break;
                    case SSCOLP:
                        size = sizeof (COLPINFO);
                        break;
                    case SSCOLR:
                        size = sizeof (COLRINFO);
                        break;
                    case SSCF:
                        size = sizeof (CFINFO);
                        break;
                    case SSCW:
                        size = sizeof (CWINFO);
                        break;
                    case SSCB:
                        size = sizeof (CBINFO);
                        break;
                    }
                    gReqHead->result_len = size * gReqHead->arg3;
                }
                gFsmState = FSM_STATE_DONE;
                return 0;
            }
            if ( gReqHead->error == ME_RS_FAILURE ) {
               KillTimer (NULL, gTimerID);
               gReqHead->result = NULL;
               gReqHead->result_len = -1;
               gFsmState = FSM_STATE_DONE;
            }
        }
        else if ( event == TIMEOUT ) {
            gReqHead->error = ME_RS_TIMEOUT;
            gReqHead->errcode = 0;
            GetResultLength (gReqHead);
            gFsmState = FSM_STATE_DONE;
        }

        break;
    }

    return 0;
}

static int FSM_USSD (int event, char* data, int datalen)
{
    char cmd[ATCMD_BUFSIZE];
    int retval;

    switch (gUssdState) {
    case USSD_IDLE:
        
        if ( event == REQUEST ) {
            sprintf (cmd, "AT+CUSD=1,\"%s\",15\r", data);
            MAT_Send (2, cmd, strlen(cmd));
            gUssdTimer = SetTimer (NULL, 0, DFT_TIMEOUT, (TIMERPROC)USSD_TimerProc);
            gUssdState = USSD_CONFIRM;
        }
        else if ( event == DATAIN ) {
            if ( memcmp (data, "+CUSD:", 6) == 0 ) {
                int flag = ME_GetIntSection (data, "+CUSD:", 1);
                if ( flag == 2 ) {
                    strcpy (gUssdBuf, data);
                    gReqUssd->result = gUssdBuf;
                    gReqUssd->result_len = sizeof (USSD_INFO);
                    gReqUssd->error = ME_USSD_DISCONNECT;
                    gReqUssd->reply (NULL);
                }
                else if ( flag == 1 ) {
                    strcpy (gUssdBuf, data);
                    gReqUssd->result = gUssdBuf;
                    gReqUssd->result_len = sizeof (USSD_INFO);
                    gReqUssd->error = ME_USSD_REQUEST;
                    gUssdState = USSD_INPUT;
                    gReqUssd->reply (NULL);
                }
                else if ( flag == 0 ) {
                    strcpy (gUssdBuf, data);
                    gReqUssd->result = gUssdBuf;
                    gReqUssd->result_len = sizeof (USSD_INFO);
                    gReqUssd->error = ME_USSD_NOTIFY;
                    gReqUssd->reply (NULL);
                }
                return 0;
            }
        }

        break;

    case USSD_CONFIRM:

        if ( event == DATAIN ) {
            retval = GetUssdErrorCode (data, datalen);
            if ( retval < 0 )
                return retval;
            KillTimer (NULL, gUssdTimer);
            if ( retval != FRC_OK ) {
                gReqUssd->result = NULL;
                gReqUssd->result_len = 0;
                gReqUssd->error = ME_USSD_REFUSE;
                gUssdState = USSD_IDLE;
                gReqUssd->reply (NULL);
                return 0;
            }
            gUssdState = USSD_LISTEN;
        }
        else if ( event == TIMEOUT ) {
            gReqUssd->result = NULL;
            gReqUssd->result_len = 0;
            gReqUssd->error = ME_USSD_REFUSE;
            gUssdState = USSD_IDLE;
            gReqUssd->reply (NULL);
        }

        break;

    case USSD_LISTEN:

        if ( event == DATAIN ) {
            if ( memcmp (data, "+CUSD:", 6) == 0 ) {
                int flag = ME_GetIntSection (data, "+CUSD:", 1);
                if ( flag < 0 ) {
                    MAT_Send (2, "AT+CUSD=2\r", -1);
                    gUssdTimer = SetTimer (NULL, 0, DFT_TIMEOUT, (TIMERPROC)USSD_TimerProc);
                    gUssdState = USSD_DISC;
                }
                else {
                    strcpy (gUssdBuf, data);
                    gReqUssd->result = gUssdBuf;
                    gReqUssd->result_len = sizeof (USSD_INFO);
                    if ( flag == 2 ) {
                        gReqUssd->error = ME_USSD_DISCONNECT;
                        gUssdState = USSD_IDLE;
                    }
                    else if ( flag == 1 ) {
                        gReqUssd->error = ME_USSD_REQUEST;
                        gUssdState = USSD_INPUT;
                    }
                    else {
                        gReqUssd->error = ME_USSD_NOTIFY;
                        gUssdState = USSD_IDLE;
                    }
                    gReqUssd->reply (NULL);
                }

                return 0;
            }

            retval = GetUssdErrorCode (data, datalen);
            if ( retval < 0 )
                return retval;

            if ( retval == FRC_ERROR ) {
                gReqUssd->result = NULL;
                gReqUssd->result_len = 0;
                gReqUssd->error = ME_USSD_REFUSE;
                gUssdState = USSD_IDLE;
                gReqUssd->reply (NULL);
            }
        }
        else if ( event == USERSTOP ) {
            MAT_Send (2, "AT+CUSD=2\r", -1);
            gUssdTimer = SetTimer (NULL, 0, DFT_TIMEOUT, (TIMERPROC)USSD_TimerProc);
            gUssdState = USSD_DISC;
            return 0;
        }

        break;

    case USSD_INPUT:

        if ( event == DATAIN ) {
            if ( data[0] == '>' && datalen == 1 ) {
                gUssdState = USSD_RESPONSE;
                if ( gReqUssd->cmddata_len > 0 ) {
                    FSM_USSD (RESPONSE, gReqUssd->cmddata, gReqUssd->cmddata_len);
                    gReqUssd->cmddata_len = 0;
                }
                return 0;
            }
            return -1;
        }
        else if ( event == RESPONSE ) {
            strcpy (gReqUssd->cmddata, data);
            gReqUssd->cmddata_len = strlen (data);
            return 0;
        }
        else if ( event == USERSTOP ) {
            char escape = 0x1b;
            MAT_Send (2, &escape, 1);
            gUssdTimer = SetTimer (NULL, 0, 4000, (TIMERPROC)USSD_TimerProc);
            gUssdState = USSD_ABORT;
            return 0;
        }

        break;

    case USSD_RESPONSE:

        if ( event == DATAIN ) {
            if ( memcmp (data, "+CUSD:", 6) == 0 ) {
                int flag = ME_GetIntSection (data, "+CUSD:", 1);
                if ( flag < 0 ) {
                    char escape = 0x1b;
                    MAT_Send (2, &escape, 1);
                    gUssdTimer = SetTimer (NULL, 0, 4000, (TIMERPROC)USSD_TimerProc);
                    gUssdState = USSD_ABORT;
                }
                else {
                    strcpy (gUssdBuf, data);
                    gReqUssd->result = gUssdBuf;
                    gReqUssd->result_len = sizeof (USSD_INFO);
                    if ( flag == 2 ) {
                        gReqUssd->error = ME_USSD_DISCONNECT;
                        gUssdState = USSD_IDLE;
                    }
                    else if ( flag == 1 ) {
                        gReqUssd->error = ME_USSD_REQUEST;
                        gUssdState = USSD_INPUT;
                    }
                    else {
                        gReqUssd->error = ME_USSD_NOTIFY;
                        gUssdState = USSD_IDLE;
                    }
                    gReqUssd->reply (NULL);
                }

                return 0;
            }

            retval = GetUssdErrorCode (data, datalen);
            if ( retval < 0 )
                return retval;

            if ( retval == FRC_ERROR ) {
                gReqUssd->result = NULL;
                gReqUssd->result_len = 0;
                gReqUssd->error = ME_USSD_DISCONNECT;
                gUssdState = USSD_IDLE;
                gReqUssd->reply (NULL);
            }
        }
        else if ( event == RESPONSE ) {
            data[datalen] = 0x1a;
            MAT_Send (2, data, datalen+1);
            gUssdTimer = SetTimer (NULL, 0, DFT_TIMEOUT, (TIMERPROC)USSD_TimerProc);
            gUssdState = USSD_CONFIRM;
            return 0;
        }
        else if ( event == USERSTOP ) {
            char escape = 0x1b;
            MAT_Send (2, &escape, 1);
            gUssdTimer = SetTimer (NULL, 0, 4000, (TIMERPROC)USSD_TimerProc);
            gUssdState = USSD_ABORT;
            return 0;
        }

        break;

    case USSD_ABORT:

        if ( event == DATAIN ) {
            retval = GetUssdErrorCode (data, datalen);
            if ( retval < 0 )
                return retval;

            KillTimer (NULL, gUssdTimer);
            MAT_Send (2, "AT+CUSD=2\r", -1);
            gUssdTimer = SetTimer (NULL, 0, DFT_TIMEOUT, (TIMERPROC)USSD_TimerProc);
            gUssdState = USSD_DISC;
        }
        else if ( event == TIMEOUT ) {
            gReqUssd->result = NULL;
            gReqUssd->result_len = 0;
            gReqUssd->error = USSD_ABORT;
            gUssdState = USSD_IDLE;
            gReqUssd->reply (NULL);
        }

        break;

    case USSD_DISC:

        if ( event == DATAIN ) {
            retval = GetUssdErrorCode (data, datalen);
            if ( retval < 0 )
                return retval;

            KillTimer (NULL, gUssdTimer);
            gReqUssd->result = NULL;
            gReqUssd->result_len = 0;
            gReqUssd->error = USSD_ABORT;
            gUssdState = USSD_IDLE;
            gReqUssd->reply (NULL);
        }
        else if ( event == TIMEOUT ) {
            gReqUssd->result = NULL;
            gReqUssd->result_len = 0;
            gReqUssd->error = USSD_ABORT;
            gUssdState = USSD_IDLE;
            gReqUssd->reply (NULL);
        }

        break;
    }

    return -1;
}

static int FSM_SwitchCH0(BOOL bOpenChnl)
{
	static int hmux = -1;

	if(!bOpenChnl)
	{
		if(hmux > 0)
		{
			MsgOut("\r\n[ME]: close channel 0 now.\r\n");
			Modem_CloseCnl();
			hmux = -1;
		}
		return 1;
	}
	
	if(hmux < 0 && Modem_OpenCnl(ME_MatIndication) < 0)
	{
		MsgOut("\r\n[ME]: can't open channel 0.\r\n");
		return -1;
	}

	MsgOut("\r\n[ME]: channel 0 opened.\r\n");
	hmux = 1;
	return 1;
}

static int FSM_SetCallFwding( int event, char* data, int datalen )
{
    char ATCmd[ATCMD_BUFSIZE];
    int retval;
    char* PhoneNum;
    int reason, Class, time;
    BOOL enable;

    if ( FSM_STATE_INIT == gFsmState )
    {
        if ( START == event )
        {
            reason = ((PCALL_FWD)gReqHead->cmddata)->Reason;
            enable = ((PCALL_FWD)gReqHead->cmddata)->bEnable;
            Class = ((PCALL_FWD)gReqHead->cmddata)->Class;
            PhoneNum = ((PCALL_FWD)gReqHead->cmddata)->PhoneNum;
            time = ((PCALL_FWD)gReqHead->cmddata)->Time;

            switch ( Class )
            {
            case CLASS_VOICE:

                if ( FWD_UNCDTIONAL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=0,3,\"%s\",,1\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=0,0,,,1\r" );
                }
                else if ( FWD_BUSY == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=1,3,\"%s\",,1\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=1,0,,,1\r" );
                }
                else if ( FWD_NOREPLY == reason )
                {
                    if ( enable )
                    {
                        if ( time > 30 || (time > 0 && time%5 != 0))
                            goto ERROR_HANDLER;

                        if ( time < 0 )
                            sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,1\r", PhoneNum );
                        else
                            sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,1,%d\r", PhoneNum, time );
                    }
                    else
                        sprintf( ATCmd, "AT+CCFC=2,0,,,1\r" );
                }
                else if ( FWD_NOREACH == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=3,3,\"%s\",,1\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=3,0,,,1\r" );
                }
                else if ( FWD_ALLCDTIONAL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=5,3,\"%s\",,1\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=5,0,,,1\r" );
                }
                else if ( FWD_ALL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=4,3,\"%s\",,1\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=4,4,,,1\r" );
                }
                else
                    goto ERROR_HANDLER;

                break;

            case CLASS_DATA:

                if ( FWD_UNCDTIONAL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=0,3,\"%s\",,2\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=0,0,,,2\r" );
                }
                else if ( FWD_BUSY == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=1,3,\"%s\",,2\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=1,0,,,2\r" );
                }
                else if ( FWD_NOREPLY == reason )
                {
                    if ( enable )
                    {
                        if ( time > 30 || (time > 0 && time%5 != 0))
                            goto ERROR_HANDLER;

                        if ( time < 0 )
                            sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,2\r", PhoneNum );
                        else
                            sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,2,%d\r", PhoneNum, time );
                    }
                    else
                        sprintf( ATCmd, "AT+CCFC=2,0,,,2\r" );
                }
                else if ( FWD_NOREACH == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=3,3,\"%s\",,2\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=3,0,,,2\r" );
                }
                else if ( FWD_ALLCDTIONAL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=5,3,\"%s\",,2\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=5,0,,,2\r" );
                }
                else if ( FWD_ALL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=4,3,\"%s\",,2\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=4,4,,,2\r" );
                }
                else
                    goto ERROR_HANDLER;

                break;
            case CLASS_FAX:

                if ( FWD_UNCDTIONAL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=0,3,\"%s\",,4\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=0,0,,,4\r" );
                }
                else if ( FWD_BUSY == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=1,3,\"%s\",,4\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=1,0,,,4\r" );
                }
                else if ( FWD_NOREPLY == reason )
                {
                    if ( enable )
                    {
                        if ( time > 30 || (time > 0 && time%5 != 0))
                            goto ERROR_HANDLER;

                        if ( time < 0 )
                            sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,4\r", PhoneNum );
                        else
                            sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,4,%d\r", PhoneNum, time );
                    }
                    else
                        sprintf( ATCmd, "AT+CCFC=2,0,,,4\r" );
                }
                else if ( FWD_NOREACH == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=3,3,\"%s\",,4\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=3,0,,,4\r" );
                }
                else if ( FWD_ALLCDTIONAL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=5,3,\"%s\",,4\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=5,0,,,4\r" );
                }
                else if ( FWD_ALL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=4,3,\"%s\",,4\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=4,4,,,4\r" );
                }
                else
                    goto ERROR_HANDLER;

                break;

            case CLASS_SMS:

                if ( FWD_UNCDTIONAL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=0,3,\"%s\",,8\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=0,0,,,8\r" );
                }
                else if ( FWD_BUSY == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=1,3,\"%s\",,8\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=1,0,,,8\r" );
                }
                else if ( FWD_NOREPLY == reason )
                {
                    if ( enable )
                    {
                        if ( time > 30 || (time > 0 && time%5 != 0))
                            goto ERROR_HANDLER;

                        if ( time < 0 )
                            sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,8\r", PhoneNum );
                        else
                            sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,8,%d\r", PhoneNum, time );
                    }
                    else
                        sprintf( ATCmd, "AT+CCFC=2,0,,,8\r" );
                }
                else if ( FWD_NOREACH == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=3,3,\"%s\",,8\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=3,0,,,8\r" );
                }
                else if ( FWD_ALLCDTIONAL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=5,3,\"%s\",,8\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=5,0,,,8\r" );
                }
                else if ( FWD_ALL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=4,3,\"%s\",,8\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=4,4,,,8\r" );
                }
                else
                    goto ERROR_HANDLER;

                break;

            case CLASS_ALL:

                if ( FWD_UNCDTIONAL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=0,3,\"%s\",,7\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=0,0,,,7\r" );
                }
                else if ( FWD_BUSY == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=1,3,\"%s\",,7\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=1,0,,,7\r" );
                }
                else if ( FWD_NOREPLY == reason )
                {
                    if ( enable )
                    {
                        if ( time > 30 || (time > 0 && time%5 != 0))
                            goto ERROR_HANDLER;

                        if ( time < 0 )
                            sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,7\r", PhoneNum );
                        else
                            sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,7,%d\r", PhoneNum, time );
                    }
                    else
                        sprintf( ATCmd, "AT+CCFC=2,0,,,7\r" );
                }
                else if ( FWD_NOREACH == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=3,3,\"%s\",,7\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=3,0,,,7\r" );
                }
                else if ( FWD_ALLCDTIONAL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=5,3,\"%s\",,7\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=5,0,,,7\r" );
                }
                else if ( FWD_ALL == reason )
                {
                    if ( enable )
                        sprintf( ATCmd, "AT+CCFC=4,3,\"%s\",,7\r", PhoneNum );
                    else
                        sprintf( ATCmd, "AT+CCFC=4,4,,,7\r" );
                }
                else
                    goto ERROR_HANDLER;

                break;

            default:
                goto ERROR_HANDLER;
            }

            retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
        }
    }
    else if ( FSM_STATE_STEP1 == gFsmState )
    {
        if ( DATAIN == event )
        {
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer(NULL, gTimerID);
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }

    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    gReqHead->errcode = 1;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}

static int FSM_GetCallFwding( int event, char* data, int datalen )
{
    char ATCmd[ATCMD_BUFSIZE];
    int retval;
    int reason, Class;

    if ( FSM_STATE_INIT == gFsmState )
    {
        if ( START == event )
        {
            reason = gReqHead->arg1;
            Class = gReqHead->arg2;

            switch ( Class )
            {
            case CLASS_VOICE:

                if ( FWD_UNCDTIONAL == reason )
                    strcpy( ATCmd, "AT+CCFC=0,2,,,1\r" );
                else if ( FWD_BUSY == reason )
                    strcpy( ATCmd, "AT+CCFC=1,2,,,1\r" );
                else if ( FWD_NOREPLY == reason )
                    strcpy( ATCmd, "AT+CCFC=2,2,,,1\r" );
                else if ( FWD_NOREACH == reason )
                    strcpy( ATCmd, "AT+CCFC=3,2,,,1\r" );
                else
                    goto ERROR_HANDLER;

                break;

            case CLASS_DATA:

                if ( FWD_UNCDTIONAL == reason )
                    strcpy( ATCmd, "AT+CCFC=0,2,,,2\r" );
                else if ( FWD_BUSY == reason )
                    strcpy( ATCmd, "AT+CCFC=1,2,,,2\r" );
                else if ( FWD_NOREPLY == reason )
                    strcpy( ATCmd, "AT+CCFC=2,2,,,2\r" );
                else if ( FWD_NOREACH == reason )
                    strcpy( ATCmd, "AT+CCFC=3,2,,,2\r" );
                else
                    goto ERROR_HANDLER;

                break;

            case CLASS_FAX:

                if ( FWD_UNCDTIONAL == reason )
                    strcpy( ATCmd, "AT+CCFC=0,2,,,4\r" );
                else if ( FWD_BUSY == reason )
                    strcpy( ATCmd, "AT+CCFC=1,2,,,4\r" );
                else if ( FWD_NOREPLY == reason )
                    strcpy( ATCmd, "AT+CCFC=2,2,,,4\r" );
                else if ( FWD_NOREACH == reason )
                    strcpy( ATCmd, "AT+CCFC=3,2,,,4\r" );
                else
                    goto ERROR_HANDLER;

                break;

            case CLASS_SMS:

                if ( FWD_UNCDTIONAL == reason )
                    strcpy( ATCmd, "AT+CCFC=0,2,,,8\r" );
                else if ( FWD_BUSY == reason )
                    strcpy( ATCmd, "AT+CCFC=1,2,,,8\r" );
                else if ( FWD_NOREPLY == reason )
                    strcpy( ATCmd, "AT+CCFC=2,2,,,8\r" );
                else if ( FWD_NOREACH == reason )
                    strcpy( ATCmd, "AT+CCFC=3,2,,,8\r" );
                else
                    goto ERROR_HANDLER;

                break;

            case CLASS_ALL:
				
                if ( FWD_UNCDTIONAL == reason )
                    strcpy( ATCmd, "AT+CCFC=0,2,,,7\r" );
                else if ( FWD_BUSY == reason )
                    strcpy( ATCmd, "AT+CCFC=1,2,,,7\r" );
                else if ( FWD_NOREPLY == reason )
                    strcpy( ATCmd, "AT+CCFC=2,2,,,7\r" );
                else if ( FWD_NOREACH == reason )
                    strcpy( ATCmd, "AT+CCFC=3,2,,,7\r" );
                else
                    goto ERROR_HANDLER;
				
                break;
            
			default:

                goto ERROR_HANDLER;
            }

            retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
        }
    }
    else if ( FSM_STATE_STEP1 == gFsmState )
    {
        if ( DATAIN == event )
        {
            if ( strstr( data, "+CCFC:") == data /*&& ME_MemGetOffset() == 0*/ )
            {
                data[datalen-1] = '\n';
                ME_MemAssign( data, datalen );
                return 0;
            }
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer(NULL, gTimerID);
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }

    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    gReqHead->errcode = 1;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}

static int FSM_SetCallBarring( int event, char* data, int datalen )
{
    char ATCmd[ATCMD_BUFSIZE];
    int retval;
    char* Password;
    int fac, Class;
    BOOL enable;

    if ( FSM_STATE_INIT == gFsmState )
    {
        if ( START == event )
        {
            fac = gReqHead->arg1;
            enable = gReqHead->arg2;
            Class = gReqHead->arg3;
            Password = gReqHead->cmddata;

            strcpy( ATCmd, "AT+CLCK=" );

            switch ( fac )
            {
            case CALLBAR_BAOC:

                strcat( ATCmd, "\"AO\"," );

                break;

            case CALLBAR_BOIC:

                strcat( ATCmd, "\"OI\"," );

                break;

            case CALLBAR_BOIC_EXHC:

                strcat( ATCmd, "\"OX\"," );

                break;

            case CALLBAR_BAIC:

                strcat( ATCmd, "\"AI\"," );

                break;

            case CALLBAR_BIC_ROAM:

                strcat( ATCmd, "\"IR\"," );

                break;

            case CALLBAR_UAB:

                if ( enable )
                    goto ERROR_HANDLER;
                strcat( ATCmd, "\"AB\"," );
                break;

            case CALLBAR_UAG:

                if ( enable )
                    goto ERROR_HANDLER;
                strcat( ATCmd, "\"AG\"," );
                break;

            case CALLBAR_UAC:

                if ( enable )
                    return FALSE;
                strcat( ATCmd, "\"AC\"," );
                break;

            default:
                goto ERROR_HANDLER;
            }

            if ( enable )
                strcat( ATCmd, "1," );
            else
                strcat( ATCmd, "0," );

            if ( Password != NULL )
            {
                strcat( ATCmd, "\"");
                strcat( ATCmd, Password );
                strcat( ATCmd, "\"");
            }

            strcat( ATCmd, "," );

            switch ( Class )
            {
            case CLASS_VOICE:

                strcat( ATCmd, "1" );

                break;

            case CLASS_DATA:

                strcat( ATCmd, "2" );

                break;

            case CLASS_FAX:

                strcat( ATCmd, "4" );

                break;

            case CLASS_SMS:

                strcat( ATCmd, "8" );

                break;

            case CLASS_ALL:

                strcat( ATCmd, "7" );

                break;

            default:
                goto ERROR_HANDLER;
            }

            strcat( ATCmd, "\r" );

            retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
        }
    }
    else if ( FSM_STATE_STEP1 == gFsmState )
    {
        if ( DATAIN == event )
        {
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer(NULL, gTimerID);
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }

    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    gReqHead->errcode = 1;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}

static int FSM_GetCallBarring( int event, char* data, int datalen )
{
    char ATCmd[ATCMD_BUFSIZE];
    int retval;
    char* Password;
    int fac, Class;

    if ( FSM_STATE_INIT == gFsmState )
    {
        if ( START == event )
        {
            fac = gReqHead->arg1;
            Class = gReqHead->arg2;
            Password = gReqHead->cmddata;

            strcpy( ATCmd, "AT+CLCK=" );

            switch ( fac )
            {
            case CALLBAR_BAOC:

                strcat( ATCmd, "\"AO\"," );

                break;

            case CALLBAR_BOIC:

                strcat( ATCmd, "\"OI\"," );

                break;

            case CALLBAR_BOIC_EXHC:

                strcat( ATCmd, "\"OX\"," );

                break;

            case CALLBAR_BAIC:

                strcat( ATCmd, "\"AI\"," );

                break;

            case CALLBAR_BIC_ROAM:

                strcat( ATCmd, "\"IR\"," );

                break;

            default:
                goto ERROR_HANDLER;
            }

            strcat( ATCmd, "2," );

            if ( Password != NULL )
            {
                strcat( ATCmd, "\"");
                strcat( ATCmd, Password );
                strcat( ATCmd, "\"");
            }

            strcat( ATCmd, "," );

            switch ( Class )
            {
            case CLASS_VOICE:

                strcat( ATCmd, "1" );

                break;

            case CLASS_DATA:

                strcat( ATCmd, "2" );

                break;

            case CLASS_FAX:

                strcat( ATCmd, "4" );

                break;

            case CLASS_SMS:

                strcat( ATCmd, "8" );

                break;

			case CLASS_ALL:
				strcat ( ATCmd, "7");
				break;

            default:
                goto ERROR_HANDLER;

            }

            strcat( ATCmd, "\r" );

            retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
        }
    }
    else if ( FSM_STATE_STEP1 == gFsmState )
    {
        if ( DATAIN == event )
        {
            if ( strstr( data, "+CLCK:") == data )
            {
                data[datalen-1] = '\n';
                ME_MemAssign( data, datalen );
                return 0;
            }
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer(NULL, gTimerID);
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }

    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    gReqHead->errcode = 1;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}

static int FSM_GetCPHSInfo(int event,char *data, int datalen)
{
    int retval = 0;
    if ( FSM_STATE_INIT == gFsmState )
    {
        if ( START == event )
        {
            retval = MAT_Send( 1, "AT+CRSM=242\r", -1 );
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
        }
    }
    else if ( FSM_STATE_STEP1 == gFsmState )
    {
        if ( DATAIN == event )
        {
            if ( strstr( data, "+CRSM:") == data && ME_MemGetOffset() == 0 )
            {
                data[datalen-1] = '\n';
                ME_MemAssign( data, datalen );
                return 0;
            }
            retval = GetResultErrorCode( data, datalen );
            
            if ( retval < 0 )
                return retval;
            
            KillTimer(NULL, gTimerID);
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    return 0;
TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;	
}
//fufu 0619
static int FSM_GetCPHSparas( int event, char* data, int datalen)
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	
	if ( FSM_STATE_INIT == gFsmState )
	{
		if ( START == event )
		{
			sprintf( ATCmd, "AT+CRSM=192,%d\r", gReqHead->arg1);

			retval = MAT_Send( 1, ATCmd, -1 );
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
		}
	}
	else if ( FSM_STATE_STEP1 == gFsmState )
	{
		if ( DATAIN == event )
		{
            if ( strstr (data, "+CRSM:") == data  && ME_MemGetOffset() == 0 )
            {
                data[datalen-1] = '\n';
                ME_MemAssign( data, datalen );
                return 0;
            }
            
			retval = GetResultErrorCode( data, datalen );
			
			if ( retval < 0 )
			{
				return retval;
			}

            KillTimer(NULL, gTimerID);
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
		}
      	else if ( TIMEOUT == event )
   			goto TIMEOUT_HANDLER;
	}
   
    return 0;

//ERROR_HANDLER:
//    gReqHead->error = ME_RS_FAILURE;
//    gReqHead->errcode = 1;
//    GetResultLength( gReqHead );
//    gFsmState = FSM_STATE_DONE;

//   return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;	
}


static int FSM_ReadCPHSFiles( int event, char* data, int datalen)
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	
	if ( FSM_STATE_INIT == gFsmState )
	{
		if ( START == event )
		{
			sprintf( ATCmd, "AT+CRSM=176,%d,0,0,%d\r", gReqHead->arg1, gReqHead->arg2);

			retval = MAT_Send( 1, ATCmd, -1 );
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
		}
	}
	else if ( FSM_STATE_STEP1 == gFsmState )
	{
		if ( DATAIN == event )
		{
            if ( strstr (data, "+CRSM:") == data  && ME_MemGetOffset() == 0 )
            {
                data[datalen-1] = '\n';
                ME_MemAssign( data, datalen );
                return 0;
            }
            
			retval = GetResultErrorCode( data, datalen );
			
			if ( retval < 0 )
			{
				return retval;
			}

            KillTimer(NULL, gTimerID);
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
		}
      	else if ( TIMEOUT == event )
   			goto TIMEOUT_HANDLER;
	}
   
    return 0;

//ERROR_HANDLER:
//    gReqHead->error = ME_RS_FAILURE;
//    gReqHead->errcode = 1;
//    GetResultLength( gReqHead );
//    gFsmState = FSM_STATE_DONE;

//   return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;	
}


static int FSM_ReadCPHSRecord( int event, char* data, int datalen)
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	
	if ( FSM_STATE_INIT == gFsmState )
	{
		if ( START == event )
		{
			sprintf( ATCmd, "AT+CRSM=178,%d,%d,4,%d\r", gReqHead->arg1, gReqHead->arg3, gReqHead->arg2);

			retval = MAT_Send( 1, ATCmd, -1 );
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
		}
	}
	else if ( FSM_STATE_STEP1 == gFsmState )
	{
		if ( DATAIN == event )
		{
            if ( strstr (data, "+CRSM:") == data  && ME_MemGetOffset() == 0 )
            {
                data[datalen-1] = '\n';
                ME_MemAssign( data, datalen );
                return 0;
            }
            
			retval = GetResultErrorCode( data, datalen );
			
			if ( retval < 0 )
			{
				return retval;
			}

            KillTimer(NULL, gTimerID);
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
		}
      	else if ( TIMEOUT == event )
   			goto TIMEOUT_HANDLER;
	}
   
    return 0;

//ERROR_HANDLER:
//    gReqHead->error = ME_RS_FAILURE;
//    gReqHead->errcode = 1;
//    GetResultLength( gReqHead );
//    gFsmState = FSM_STATE_DONE;

//   return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;	
}

//end 0619


static int FSM_UpdateCPHSFiles( int event, char* data, int datalen)
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	int Offset_H, Offset_L;

	if ( FSM_STATE_INIT == gFsmState )
	{
		if ( START == event )
		{
			Offset_H = (gReqHead->arg2) >> 8;
			Offset_L = (gReqHead->arg2) && 8;
				
			sprintf( ATCmd, "AT+CRSM=214,%d,%d,%d,%d,%s\r", gReqHead->arg1, Offset_H, Offset_L, (gReqHead->cmddata_len)/2, gReqHead->cmddata);

			retval = MAT_Send( 1, ATCmd, -1 );
            		gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            		gFsmState = FSM_STATE_STEP1;
		}
	}
	else if ( FSM_STATE_STEP1 == gFsmState )
	{
		if ( DATAIN == event )
		{
            		if ( strstr (data, "+CRSM:") == data  && ME_MemGetOffset() == 0 )
            		{
                		data[datalen-1] = '\n';
                		ME_MemAssign( data, datalen );
                		return 0;
            		}
            
			retval = GetResultErrorCode( data, datalen );
			
			if ( retval < 0 )
			{
				return retval;
			}

           		KillTimer(NULL, gTimerID);
            		GetResultLength( gReqHead );
            		gFsmState = FSM_STATE_DONE;
		}
      		else if ( TIMEOUT == event )
   			goto TIMEOUT_HANDLER;
	}
   
    return 0;

//ERROR_HANDLER:
//    gReqHead->error = ME_RS_FAILURE;
//    gReqHead->errcode = 1;
//    GetResultLength( gReqHead );
//    gFsmState = FSM_STATE_DONE;

//   return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;	
}


static int FSM_UpdateCPHSRecord( int event, char* data, int datalen)
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	
	if ( FSM_STATE_INIT == gFsmState )
	{
		if ( START == event )
		{
			sprintf( ATCmd, "AT+CRSM=220,%d,%d,4,%d,%s\r", gReqHead->arg1, gReqHead->arg2, (gReqHead->cmddata_len)/2, gReqHead->cmddata);

			retval = MAT_Send( 1, ATCmd, -1 );
            		gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            		gFsmState = FSM_STATE_STEP1;
		}
	}
	else if ( FSM_STATE_STEP1 == gFsmState )
	{
		if ( DATAIN == event )
		{
            		if ( strstr (data, "+CRSM:") == data  && ME_MemGetOffset() == 0 )
            		{
                		data[datalen-1] = '\n';
                		ME_MemAssign( data, datalen );
                		return 0;
            		}
            
			retval = GetResultErrorCode( data, datalen );
			
			if ( retval < 0 )
			{
				return retval;
			}

            		KillTimer(NULL, gTimerID);
            		GetResultLength( gReqHead );
            		gFsmState = FSM_STATE_DONE;
		}
      		else if ( TIMEOUT == event )
   			goto TIMEOUT_HANDLER;
	}
   
    return 0;

//ERROR_HANDLER:
//    gReqHead->error = ME_RS_FAILURE;
//    gReqHead->errcode = 1;
//    GetResultLength( gReqHead );
//    gFsmState = FSM_STATE_DONE;

//   return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;	
}

#define ME_SMS_PDU_LEN          660
static int FSM_SMSSend( int event, char* data, int datalen )
{
    char ATCmd[ATCMD_BUFSIZE];
    char PDUCode[ME_SMS_PDU_LEN];
    int retval;
    char* temp;
    static int iSendIndex = 0;//CMGS的index

    switch (gFsmState) {
    case FSM_STATE_INIT:

        if ( START == event ) {
            retval = MAT_Send( 1, "AT+CMGF?\r", -1 );
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
            iSendIndex = 0;
        }

        break;

    case FSM_STATE_STEP1:

        if ( DATAIN == event ) {
            if ( strstr (data, "+CMGF:") == data && ME_MemGetOffset() == 0 ) {
                data[datalen-1] = '\n';
                ME_MemAssign (data, datalen);
                return 0;
            }
            retval = IsFinalResultCode (data, datalen);

            if ( retval < 0 )
                return retval;

            KillTimer (NULL, gTimerID);
            temp = ME_MemGetPntr ();
            retval = ME_GetIntSection (temp, "+CMGF:", 1);
            if ( retval == 1 ) {
                retval = MAT_Send (1, "AT+CMGF=0\r", -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP2;
            }
            else if ( retval == 0 ) {
                int len;
                PSMS_SEND pSend = gReqHead->cmddata;
                memset (PDUCode, 0, ME_SMS_PDU_LEN);
                len = ME_TranStr_Code (pSend, PDUCode, ME_SMS_PDU_LEN);
                if ( len <= 0 )
                    goto ERROR_HANDLER;
                sprintf (ATCmd, "AT+CMGS=%d\r", len);
                gReqHead->cmddata = malloc (strlen(PDUCode)+1);
                strcpy (gReqHead->cmddata, PDUCode);
                gReqHead->cmddata_len = strlen (PDUCode);

                retval = MAT_Send (1, ATCmd, -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP3;
            }
            else
                goto ERROR_HANDLER;
        }
        else if ( TIMEOUT == event )
            goto ERROR_HANDLER;

        break;

    case FSM_STATE_STEP2:

        if ( DATAIN == event ) {
            retval = GetResultErrorCode (data, datalen);
            if ( retval < 0 )
                return retval;
            KillTimer (NULL, gTimerID);
            if ( retval == FRC_OK ) {
                int len;
                PSMS_SEND pSend = gReqHead->cmddata;
                memset (PDUCode, 0, ME_SMS_PDU_LEN);
                len = ME_TranStr_Code (pSend, PDUCode, ME_SMS_PDU_LEN);
                if ( len < 0 )
                    goto ERROR_HANDLER;

                sprintf (ATCmd, "AT+CMGS=%d\r", len);
                gReqHead->cmddata = malloc (strlen(PDUCode)+1);
                strcpy (gReqHead->cmddata, PDUCode);
                gReqHead->cmddata_len = strlen (PDUCode);

                retval = MAT_Send (1, ATCmd, -1);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
                gFsmState = FSM_STATE_STEP3;
            }
            else {
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;

        break;

    case FSM_STATE_STEP3:

        if ( DATAIN == event ) {
            if ( datalen == 1 && data[0] == '>' ) {
                KillTimer( NULL, gTimerID );
                retval = MAT_Send (1, gReqHead->cmddata, gReqHead->cmddata_len);
                gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT * 2, ME_TimerProc);
                gFsmState = FSM_STATE_STEP4;
            }
            else {
                retval = GetResultErrorCode (data, datalen);
                if ( retval < 0 )
                    return retval;
                free (gReqHead->cmddata);
                KillTimer (NULL, gTimerID);
                GetResultLength (gReqHead);
                gFsmState = FSM_STATE_DONE;
            }
        }
        else if ( TIMEOUT == event ) {
            free (gReqHead->cmddata);
            goto TIMEOUT_HANDLER;
        }

        break;

    case FSM_STATE_STEP4:

        if ( DATAIN == event ) {
            retval = GetResultErrorCode (data, datalen);

            if (strstr(data,"+CMGS:") != NULL)
            {
                gReqHead->errcode = ME_GetIntSection(data,"+CMGS:",1);
                iSendIndex = gReqHead->errcode;
            }
            else if (iSendIndex != 0)
            {
                gReqHead->errcode = iSendIndex;
            }

            if ( retval < 0 )
                return retval;

            free (gReqHead->cmddata);
            KillTimer (NULL, gTimerID);
            GetResultLength (gReqHead);
            gFsmState = FSM_STATE_DONE;
        }
        else if ( USERSTOP == event ) {
			char escape;
			escape = 0x1B;
			free( gReqHead->cmddata);
            retval = MAT_Send (1, &escape, 1);
    		gTimerID = SetTimer (NULL, 0, 4000, ME_TimerProc);
            gReqHead->arg1 = ME_RS_USERSTOP;
			gFsmState = FSM_STATE_STEP5;
        }
        else if ( TIMEOUT == event ) {
			char escape;
			escape = 0x1B;
			free( gReqHead->cmddata);
            retval = MAT_Send (1, &escape, 1);
    		gTimerID = SetTimer (NULL, 0, 4000, ME_TimerProc);
            gReqHead->arg1 = ME_RS_TIMEOUT;
			gFsmState = FSM_STATE_STEP5;
        }

        break;

    case FSM_STATE_STEP5:

		if ( DATAIN == event ) {
			retval = GetResultErrorCode (data, datalen);
			if ( retval < 0 )
				return retval;
			KillTimer (NULL, gTimerID);
            gReqHead->error = gReqHead->arg1;
            gReqHead->errcode = 0;
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
		}
		else if ( TIMEOUT == event ){
            gReqHead->error = gReqHead->arg1;
            gReqHead->errcode = 0;
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
		}
        break;
    }

    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    gReqHead->errcode = 1;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}

static int FSM_SMSRead( int event, char* data, int datalen )
{
    char ATCmd[ATCMD_BUFSIZE];
    int retval;
    char* temp;

    if ( FSM_STATE_INIT == gFsmState )
    {
        if ( START == event )
        {
            switch ( gReqHead->arg2 )
            {
            case SMS_MEM_SM:
                retval = MAT_Send( 1, "AT+CPMS=\"SM\"\r", -1 );
                break;
            case SMS_MEM_ME:
                retval = MAT_Send( 1, "AT+CPMS=\"ME\"\r", -1 );
                break;
            case SMS_MEM_MT:
                retval = MAT_Send( 1, "AT+CPMS=\"MT\"\r", -1 );
                break;
            default:
                goto ERROR_HANDLER;
            }
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
        }
    }
    else if ( FSM_STATE_STEP1 == gFsmState )
    {
        if ( DATAIN == event )
        {
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );

            if ( retval == FRC_OK)
            {
                retval = MAT_Send( 1, "AT+CMGF?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP2;
            }
            else if ( retval == FRC_CMSERROR && gReqHead->errcode == 522)
            {
                switch ( gReqHead->arg2 )
                {
                case SMS_MEM_SM:
                    retval = MAT_Send( 1, "AT+CPMS=\"SM\"\r", -1 );
                    break;
                case SMS_MEM_ME:
                    retval = MAT_Send( 1, "AT+CPMS=\"ME\"\r", -1 );
                    break;
                case SMS_MEM_MT:
                    retval = MAT_Send( 1, "AT+CPMS=\"MT\"\r", -1 );
                    break;
                default:
                    goto ERROR_HANDLER;
                }
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
            else
            {
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP2 == gFsmState )
    {
        if ( DATAIN == event )
        {
            if ( strstr( data, "+CMGF:") == data && ME_MemGetOffset() == 0 )
            {
                data[datalen-1] = '\n';
                ME_MemAssign( data, datalen );
                return 0;
            }
            retval = IsFinalResultCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );
            if ( retval == FRC_OK )
            {
                temp = ME_MemGetPntr();

                retval = ME_GetIntSection( temp, "+CMGF:", 1 );

                if ( retval == 1 )
                {
                    retval = MAT_Send( 1, "AT+CMGF=0\r", -1 );
                    gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                    gFsmState = FSM_STATE_STEP3;
                }
                else if ( retval == 0 )
                {
                    if ( gReqHead->arg1 )
                        sprintf( ATCmd, "AT^SMGR=%d\r", gReqHead->arg3 );
                    else
                        sprintf( ATCmd, "AT+CMGR=%d\r", gReqHead->arg3 );

                    retval = MAT_Send( 1, ATCmd, -1 );
                    gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                    gFsmState = FSM_STATE_STEP4;
                }
                else
                    goto ERROR_HANDLER;
            }
            else
                goto ERROR_HANDLER;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP3 == gFsmState )
    {
        if ( DATAIN == event )
        {
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );

            if ( retval == FRC_OK )
            {
                if ( gReqHead->arg1 )
                    sprintf( ATCmd, "AT^SMGR=%d\r", gReqHead->arg3 );
                else
                    sprintf( ATCmd, "AT+CMGR=%d\r", gReqHead->arg3 );

                retval = MAT_Send( 1, ATCmd, -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP4;
            }
            else
            {
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP4 == gFsmState )
    {
        if ( DATAIN == event )
        {
            if ( strstr( data, "+CMGR:") == data || strstr( data, "^SMGR:") == data )
            {
                data[datalen-1] = '\n';
                ME_MemAssign(data, datalen );
                gFsmState = FSM_STATE_STEP5;
                return 0;
            }
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP5 == gFsmState )
    {
        if ( DATAIN == event )
        {
            data[datalen-1] = '\n';
            ME_MemAssign( data, datalen );
            gFsmState = FSM_STATE_STEP4;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }

    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    gReqHead->errcode = 1;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}

static int FSM_SMSDelete( int event, char* data, int datalen )
{
    char ATCmd[ATCMD_BUFSIZE];
    int retval;

    if ( FSM_STATE_INIT == gFsmState )
    {
        if ( START == event )
        {
            switch ( gReqHead->arg1 )
            {
            case SMS_MEM_SM:
                retval = MAT_Send( 1, "AT+CPMS=\"SM\",\"SM\"\r", -1 );
                break;
            case SMS_MEM_ME:
                retval = MAT_Send( 1, "AT+CPMS=\"ME\",\"ME\"\r", -1 );
                break;
            case SMS_MEM_MT:
                retval = MAT_Send( 1, "AT+CPMS=\"MT\",\"MT\"\r", -1 );
                break;
            default:
                goto ERROR_HANDLER;
            }
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
        }
    }
    else if ( FSM_STATE_STEP1 == gFsmState )
    {
        if ( DATAIN == event )
        {
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );

            if ( retval == FRC_OK)
            {
                sprintf( ATCmd, "AT+CMGD=%d\r", gReqHead->arg2 );
                retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP2;
            }
            else
            {
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP2 == gFsmState )
    {
        if ( DATAIN == event )
        {
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }

    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    gReqHead->errcode = 1;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}

static int FSM_SMSWrite( int event, char* data, int datalen )
{
	char ATCmd[ATCMD_BUFSIZE];
	char PDUCode[ME_SMS_PDU_LEN];
	int retval;
	char* temp;

	if ( FSM_STATE_INIT == gFsmState )
	{
		if ( START == event )
		{
			switch ( gReqHead->arg1 )
			{
				case SMS_MEM_SM:
					retval = MAT_Send( 1, "AT+CPMS=\"SM\"\r", -1 );
					break;
				case SMS_MEM_ME:
					retval = MAT_Send( 1, "AT+CPMS=\"ME\"\r", -1 );
					break;
				case SMS_MEM_MT:
					retval = MAT_Send( 1, "AT+CPMS=\"MT\"\r", -1 );
					break;
				default:
					goto ERROR_HANDLER;
			}
			gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
			gFsmState = FSM_STATE_STEP1;
		}
	}
	else if ( FSM_STATE_STEP1 == gFsmState )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, gTimerID );

			if ( retval == FRC_OK)
			{
				retval = MAT_Send( 1, "AT+CMGF?\r", -1 );
				gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				gFsmState = FSM_STATE_STEP2;
			}
			else if ( retval == FRC_CMSERROR && gReqHead->errcode == 522)
			{
				switch ( gReqHead->arg1 )
				{
					case SMS_MEM_SM:
						retval = MAT_Send( 1, "AT+CPMS=\"SM\"\r", -1 );
						break;
					case SMS_MEM_ME:
						retval = MAT_Send( 1, "AT+CPMS=\"ME\"\r", -1 );
						break;
					case SMS_MEM_MT:
						retval = MAT_Send( 1, "AT+CPMS=\"MT\"\r", -1 );
						break;
					default:
						goto ERROR_HANDLER;
				}
				gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				gFsmState = FSM_STATE_STEP1;
			}
			else
			{
				GetResultLength( gReqHead );
				gFsmState = FSM_STATE_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}	
	else if ( FSM_STATE_STEP2 == gFsmState )
	{
		if ( DATAIN == event )
		{
			if ( strstr( data, "+CMGF:") == data && ME_MemGetOffset() == 0 )
			{
				data[datalen-1] = '\n';
				ME_MemAssign( data, datalen );
				return 0;
			}
			retval = IsFinalResultCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, gTimerID );
			if ( retval == FRC_OK )
			{
				temp = ME_MemGetPntr();

				retval = ME_GetIntSection( temp, "+CMGF:", 1 );

				if ( retval == 1 )
				{
					retval = MAT_Send( 1, "AT+CMGF=0\r", -1 );
					gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					gFsmState = FSM_STATE_STEP3;
				}
				else if ( retval == 0 )
				{
					int len;
					PSMS_SEND pSend = gReqHead->cmddata;
					memset (PDUCode, 0, ME_SMS_PDU_LEN);
					len = ME_TranStr_Code (pSend, PDUCode, ME_SMS_PDU_LEN);
					if ( len <= 0 )
						goto ERROR_HANDLER;

					if( SMS_STO_DEFAULT == gReqHead->arg2 )
						sprintf (ATCmd, "AT+CMGW=%d\r", len );
					else
						sprintf (ATCmd, "AT+CMGW=%d, %d\r", len, gReqHead->arg2 );

					gReqHead->cmddata = malloc (strlen(PDUCode)+1);
					strcpy (gReqHead->cmddata, PDUCode);
					gReqHead->cmddata_len = strlen (PDUCode);

					retval = MAT_Send (1, ATCmd, -1);
					gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
					gFsmState = FSM_STATE_STEP4;
				}
				else
					goto ERROR_HANDLER;
			}
			else
				goto ERROR_HANDLER;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
    	else if ( FSM_STATE_STEP3 == gFsmState )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, gTimerID );

			if ( retval == FRC_OK )
			{
				int len;
				PSMS_SEND pSend = gReqHead->cmddata;
				memset (PDUCode, 0, ME_SMS_PDU_LEN);
				len = ME_TranStr_Code (pSend, PDUCode, ME_SMS_PDU_LEN);
				if ( len <= 0 )
					goto ERROR_HANDLER;

				if( SMS_STO_DEFAULT == gReqHead->arg2 )
					sprintf (ATCmd, "AT+CMGW=%d\r", len );
				else
					sprintf (ATCmd, "AT+CMGW=%d, %d\r", len, gReqHead->arg2 );
					
				gReqHead->cmddata = malloc (strlen(PDUCode)+1);
				strcpy (gReqHead->cmddata, PDUCode);
				gReqHead->cmddata_len = strlen (PDUCode);

				retval = MAT_Send (1, ATCmd, -1);
				gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
				gFsmState = FSM_STATE_STEP4;
			}
			else
			{
				GetResultLength( gReqHead );
				gFsmState = FSM_STATE_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if( FSM_STATE_STEP4 == gFsmState )
	{

		if ( DATAIN == event ) 
		{
			if ( datalen == 1 && data[0] == '>' ) 
			{
				KillTimer( NULL, gTimerID );
				retval = MAT_Send (1, gReqHead->cmddata, gReqHead->cmddata_len);
				gTimerID = SetTimer (NULL, 0, DFT_TIMEOUT, ME_TimerProc);
				gFsmState = FSM_STATE_STEP5;
			}
			else 
			{
				retval = GetResultErrorCode (data, datalen);
				if ( retval < 0 )
					return retval;
				free (gReqHead->cmddata);
				KillTimer (NULL, gTimerID);
				GetResultLength (gReqHead);
				gFsmState = FSM_STATE_DONE;
			}
		}
		else if ( TIMEOUT == event )
		{
			free (gReqHead->cmddata);
				goto TIMEOUT_HANDLER;
		}		
	}
	else if( FSM_STATE_STEP5 == gFsmState )
	{
		if ( DATAIN == event ) 
		{
			retval = GetResultErrorCode (data, datalen);

			if ( retval < 0 )
				return retval;

			free (gReqHead->cmddata);
			KillTimer (NULL, gTimerID);
			GetResultLength (gReqHead);
			gFsmState = FSM_STATE_DONE;
		}
		else if ( USERSTOP == event ) 
		{
			char escape;
			escape = 0x1B;
			free( gReqHead->cmddata);
			retval = MAT_Send (1, &escape, 1);
			gTimerID = SetTimer (NULL, 0, 4000, ME_TimerProc);
			gReqHead->arg1 = ME_RS_USERSTOP;
			gFsmState = FSM_STATE_STEP6;
		}
		else if ( TIMEOUT == event ) 
		{
			char escape;
			escape = 0x1B;
			free( gReqHead->cmddata);
			retval = MAT_Send (1, &escape, 1);
			gTimerID = SetTimer (NULL, 0, 4000, ME_TimerProc);
			gReqHead->arg1 = ME_RS_TIMEOUT;
			gFsmState = FSM_STATE_STEP6;
		}		
	}
	else if( FSM_STATE_STEP6 == gFsmState )
	{
		if ( DATAIN == event ) 
		{
			retval = GetResultErrorCode (data, datalen);
			if ( retval < 0 )
				return retval;
			KillTimer (NULL, gTimerID);
			gReqHead->error = gReqHead->arg1;
			gReqHead->errcode = 0;
			GetResultLength( gReqHead );
			gFsmState = FSM_STATE_DONE;
		}
		else if ( TIMEOUT == event )
		{
			gReqHead->error = gReqHead->arg1;
			gReqHead->errcode = 0;
			GetResultLength( gReqHead );
			gFsmState = FSM_STATE_DONE;		
		}
	}
	
    	return 0;

ERROR_HANDLER:
    	gReqHead->error = ME_RS_FAILURE;
    	gReqHead->errcode = 1;
    	GetResultLength( gReqHead );
    	gFsmState = FSM_STATE_DONE;

    	return 0;

TIMEOUT_HANDLER:
    	gReqHead->error = ME_RS_TIMEOUT;
    	gReqHead->errcode = 0;
    	GetResultLength( gReqHead );
    	gFsmState = FSM_STATE_DONE;

    return 0;	
}

static int FSM_SMSList( int event, char* data, int datalen )
{
    int retval;
    char* temp;

    if ( FSM_STATE_INIT == gFsmState )
    {
        if ( START == event )
        {
            switch ( gReqHead->arg2 )
            {
            case SMS_MEM_SM:
                retval = MAT_Send( 1, "AT+CPMS=\"SM\"\r", -1 );
                break;
            case SMS_MEM_ME:
                retval = MAT_Send( 1, "AT+CPMS=\"ME\"\r", -1 );
                break;
            case SMS_MEM_MT:
                retval = MAT_Send( 1, "AT+CPMS=\"MT\"\r", -1 );
                break;
            default:
                goto ERROR_HANDLER;
            }
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
        }
    }
    else if ( FSM_STATE_STEP1 == gFsmState )
    {
        if ( DATAIN == event )
        {
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );

            if ( retval == FRC_OK )
            {
                retval = MAT_Send( 1, "AT+CMGF?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP2;
            }
            else if ( retval == FRC_CMSERROR && gReqHead->errcode == 522)
            {
                switch ( gReqHead->arg2 )
                {
                case SMS_MEM_SM:
                    retval = MAT_Send( 1, "AT+CPMS=\"SM\"\r", -1 );
                    break;
                case SMS_MEM_ME:
                    retval = MAT_Send( 1, "AT+CPMS=\"ME\"\r", -1 );
                    break;
                case SMS_MEM_MT:
                    retval = MAT_Send( 1, "AT+CPMS=\"MT\"\r", -1 );
                    break;
                default:
                    goto ERROR_HANDLER;
                }
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP1;
            }
            else
            {
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP2 == gFsmState )
    {
        if ( DATAIN == event )
        {
            if ( strstr( data, "+CMGF:") == data && ME_MemGetOffset() == 0 )
            {
                data[datalen-1] = '\n';
                ME_MemAssign( data, datalen );
                return 0;
            }
            retval = IsFinalResultCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );
            temp = ME_MemGetPntr();

            retval = ME_GetIntSection( temp, "+CMGF:", 1 );
            
            if ( retval == 1 )
            {
                retval = MAT_Send( 1, "AT+CMGF=0\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP3;
            }
            else if ( retval == 0 )
            {
                switch ( gReqHead->arg3 )
                {
                case SMS_UNREAD:
                    if ( gReqHead->arg1 )
                        retval = MAT_Send( 1, "AT^SMGL=0\r", -1 );
                    else
                        retval = MAT_Send( 1, "AT+CMGL=0\r", -1 );
                    break;

                case SMS_READED:
                    if ( gReqHead->arg1 )
                        retval = MAT_Send( 1, "AT^SMGL=1\r", -1 );
                    else
                        retval = MAT_Send( 1, "AT+CMGL=1\r", -1 );
                    break;

                case SMS_UNSEND:
                    if ( gReqHead->arg1 )
                        retval = MAT_Send( 1, "AT^SMGL=2\r", -1 );
                    else
                        retval = MAT_Send( 1, "AT+CMGL=2\r", -1 );
                    break;

                case SMS_SENDED:
                    if ( gReqHead->arg1 )
                        retval = MAT_Send( 1, "AT^SMGL=3\r", -1 );
                    else
                        retval = MAT_Send( 1, "AT+CMGL=3\r", -1 );
                    break;

                case SMS_ALL:
                    if ( gReqHead->arg1 )
                        retval = MAT_Send( 1, "AT^SMGL=4\r", -1 );
                    else
                        retval = MAT_Send( 1, "AT+CMGL=4\r", -1 );
                    break;

                default:
                    goto ERROR_HANDLER;
                }
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT * 6, ME_TimerProc );
                gFsmState = FSM_STATE_STEP4;
            }
            else
                goto ERROR_HANDLER;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP3 == gFsmState )
    {
        if ( DATAIN == event )
        {
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );

            if ( retval == FRC_OK )
            {
                switch ( gReqHead->arg3 )
                {
                case SMS_UNREAD:
                    if ( gReqHead->arg1 )
                        retval = MAT_Send( 1, "AT^SMGL=0\r", -1 );
                    else
                        retval = MAT_Send( 1, "AT+CMGL=0\r", -1 );
                    break;

                case SMS_READED:
                    if ( gReqHead->arg1 )
                        retval = MAT_Send( 1, "AT^SMGL=1\r", -1 );
                    else
                        retval = MAT_Send( 1, "AT+CMGL=1\r", -1 );
                    break;

                case SMS_UNSEND:
                    if ( gReqHead->arg1 )
                        retval = MAT_Send( 1, "AT^SMGL=2\r", -1 );
                    else
                        retval = MAT_Send( 1, "AT+CMGL=2\r", -1 );
                    break;

                case SMS_SENDED:
                    if ( gReqHead->arg1 )
                        retval = MAT_Send( 1, "AT^SMGL=3\r", -1 );
                    else
                        retval = MAT_Send( 1, "AT+CMGL=3\r", -1 );
                    break;

                case SMS_ALL:
                    if ( gReqHead->arg1 )
                        retval = MAT_Send( 1, "AT^SMGL=4\r", -1 );
                    else
                        retval = MAT_Send( 1, "AT+CMGL=4\r", -1 );
                    break;

                default:
                    goto ERROR_HANDLER;
                }
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT * 6, ME_TimerProc );
                gFsmState = FSM_STATE_STEP4;
            }
            else
            {
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP4 == gFsmState )
    {
        if ( DATAIN == event )
        {
            if ( strstr( data, "+CMGL:") == data ||
                strstr( data, "^SMGL:") == data )
            {
                data[datalen-1] = '\n';
                ME_MemAssign(data, datalen );
                gFsmState = FSM_STATE_STEP5;
                return 0;
            }
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP5 == gFsmState )
    {
        if ( DATAIN == event )
        {
            data[datalen-1] = '\n';
            ME_MemAssign( data, datalen );
            gFsmState = FSM_STATE_STEP4;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }

    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    gReqHead->errcode = 1;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;

TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}

static int FSM_PBookRead( int event, char* data, int datalen )
{
    char ATCmd[ATCMD_BUFSIZE];
    int retval;
    char* temp;

    if ( FSM_STATE_INIT == gFsmState )
    {
        if ( START == event )
        {
            switch ( gReqHead->arg1 )
            {
            case PBOOK_SM:
                retval = MAT_Send( 1, "AT+CPBS=\"SM\"\r", -1 );
                break;
            case PBOOK_LD:
                retval = MAT_Send( 1, "AT+CPBS=\"LD\"\r", -1 );
                break;
            case PBOOK_MC:
                retval = MAT_Send( 1, "AT+CPBS=\"MC\"\r", -1 );
                break;
            case PBOOK_RC:
                retval = MAT_Send( 1, "AT+CPBS=\"RC\"\r", -1 );
                break;
            case PBOOK_FD:
                retval = MAT_Send( 1, "AT+CPBS=\"FD\"\r", -1 );
                break;
            case PBOOK_ON:
                retval = MAT_Send( 1, "AT+CPBS=\"ON\"\r", -1 );
                break;
            default:
                goto ERROR_HANDLER;
            }
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
        }
    }
    else if ( FSM_STATE_STEP1 == gFsmState )
    {
        if ( DATAIN == event )
        {
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );

            if ( retval == FRC_OK)
            {
                retval = MAT_Send( 1, "AT+CSCS?\r", -1 );
                gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                gFsmState = FSM_STATE_STEP2;
            }
            else
            {
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP2 == gFsmState )
    {
        if ( DATAIN == event )
        {
            char result[20];
            if ( strstr( data, "+CSCS:") == data && ME_MemGetOffset() == 0 )
            {
                data[datalen-1] = '\n';
                ME_MemAssign( data, datalen );
                return 0;
            }
            retval = IsFinalResultCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );

            if ( retval == FRC_OK )
            {
                temp = ME_MemGetPntr();
                
                retval = ME_GetStrSection (temp, result, 20, "+CSCS:", 1);

                if ( strcmp(result, "UCS2") != 0 )
                {
                    retval = MAT_Send( 1, "AT+CSCS=\"UCS2\"\r", -1 );
                    gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
                    gFsmState = FSM_STATE_STEP3;
                }
                else
                {
                    if ( gReqHead->arg3 == 0 )
                        sprintf( ATCmd, "AT+CPBR=%d\r", gReqHead->arg2 );
                    else if ( gReqHead->arg3 > gReqHead->arg2 )
                        sprintf( ATCmd, "AT+CPBR=%d,%d\r", gReqHead->arg2, gReqHead->arg3 );
                    else
                        goto ERROR_HANDLER;
                    
                    retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                    gTimerID = SetTimer( NULL, 0, 30000, ME_TimerProc );
                    gFsmState = FSM_STATE_STEP4;
                }
            }
            else
                goto ERROR_HANDLER;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP3 == gFsmState )
    {
        if ( DATAIN == event )
        {
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );

            if ( retval == FRC_OK )
            {
                if ( gReqHead->arg3 == 0 )
                    sprintf( ATCmd, "AT+CPBR=%d\r", gReqHead->arg2 );
                else if ( gReqHead->arg3 > gReqHead->arg2 )
                    sprintf( ATCmd, "AT+CPBR=%d,%d\r", gReqHead->arg2, gReqHead->arg3 );
                else
                    goto ERROR_HANDLER;

                retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
                gTimerID = SetTimer( NULL, 0, 30000, ME_TimerProc );
                gFsmState = FSM_STATE_STEP4;
            }
            else
            {
                GetResultLength( gReqHead );
                gFsmState = FSM_STATE_DONE;
            }
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }
    else if ( FSM_STATE_STEP4 == gFsmState )
    {
        if ( DATAIN == event )
        {
            if ( strstr( data, "+CPBR:") == data )
            {
                data[datalen-1] = '\n';
                ME_MemAssign(data, datalen );
                return 0;
            }
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer( NULL, gTimerID );
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }

    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    gReqHead->errcode = 1;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}

static int FSM_SetPhoneLock( int event, char* data, int datalen )
{
    char ATCmd[ATCMD_BUFSIZE];
    int retval;
    char* Password;
    int fac;
    BOOL enable;

    if ( FSM_STATE_INIT == gFsmState )
    {
        if ( START == event )
        {
            fac = gReqHead->arg1;
            enable = gReqHead->arg2;
            Password = gReqHead->cmddata;

            strcpy( ATCmd, "AT+CLCK=" );

            switch ( fac )
            {
            case LOCK_PS:

                strcat( ATCmd, "\"PS\"," );

                break;

            case LOCK_SC:

                strcat( ATCmd, "\"SC\"," );

                break;

            case LOCK_PN:

                strcat( ATCmd, "\"PN\"," );

                break;

            case LOCK_PU:

                strcat( ATCmd, "\"PU\"," );

                break;

            case LOCK_PP:

                strcat( ATCmd, "\"PP\"," );

                break;

            case LOCK_PC:

                strcat( ATCmd, "\"PC\"," );

                break;

            case LOCK_FD:

                strcat( ATCmd, "\"FD\"," );

                break;

            default:
                goto ERROR_HANDLER;
            }

            if ( enable )
                strcat( ATCmd, "1" );
            else
                strcat( ATCmd, "0" );

            if ( Password != NULL )
            {
                strcat( ATCmd, ",");
                strcat( ATCmd, "\"" );
                strcat( ATCmd, Password );
                strcat( ATCmd, "\"" );
            }

            strcat( ATCmd, "\r" );

            retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
        }
    }
    else if ( FSM_STATE_STEP1 == gFsmState )
    {
        if ( DATAIN == event )
        {
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer(NULL, gTimerID);
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }

    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    gReqHead->errcode = 1;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}

static int FSM_GetPhoneLock( int event, char* data, int datalen )
{
    char ATCmd[ATCMD_BUFSIZE];
    int retval;
    char* Password;
    int fac;

    if ( FSM_STATE_INIT == gFsmState )
    {
        if ( START == event )
        {
            fac = gReqHead->arg1;
            Password = gReqHead->cmddata;

            strcpy( ATCmd, "AT+CLCK=" );

            switch ( fac )
            {
            case LOCK_PS:

                strcat( ATCmd, "\"PS\"," );

                break;

            case LOCK_SC:

                strcat( ATCmd, "\"SC\"," );

                break;

            case LOCK_PN:

                strcat( ATCmd, "\"PN\"," );

                break;

            case LOCK_PU:

                strcat( ATCmd, "\"PU\"," );

                break;

            case LOCK_PP:

                strcat( ATCmd, "\"PP\"," );

                break;

            case LOCK_PC:

                strcat( ATCmd, "\"PC\"," );

                break;

            case LOCK_FD:

                strcat( ATCmd, "\"FD\"," );

                break;

            default:
                goto ERROR_HANDLER;
            }

            strcat( ATCmd, "2" );

            if ( Password != NULL )
            {
                strcat( ATCmd, "," );
                strcat( ATCmd, "\"" );
                strcat( ATCmd, Password );
                strcat( ATCmd, "\"" );
            }

            strcat( ATCmd, "\r" );

            retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
        }
    }
    else if ( FSM_STATE_STEP1 == gFsmState )
    {
        if ( DATAIN == event )
        {
            if ( strstr( data, "+CLCK:") == data && ME_MemGetOffset() == 0 )
            {
                data[datalen-1] = '\n';
                ME_MemAssign( data, datalen );
                return 0;
            }
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer(NULL, gTimerID);
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }

    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    gReqHead->errcode = 1;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}

static int FSM_ChangePassword( int event, char* data, int datalen )
{
    char ATCmd[ATCMD_BUFSIZE];
    int retval;
    char *OldPass, *NewPass;
    int fac;

    if ( FSM_STATE_INIT == gFsmState )
    {
        if ( START == event )
        {
            fac = gReqHead->arg1;

            OldPass = gReqHead->cmddata;
            NewPass = &OldPass[strlen(OldPass)+1];

            strcpy( ATCmd, "AT+CPWD=" );

            switch ( fac )
            {
            case CALLBAR_BAOC:

                strcat( ATCmd, "\"AO\"," );

                break;

            case CALLBAR_BOIC:

                strcat( ATCmd, "\"OI\"," );

                break;

            case CALLBAR_BOIC_EXHC:

                strcat( ATCmd, "\"OX\"," );

                break;

            case CALLBAR_BAIC:

                strcat( ATCmd, "\"AI\"," );

                break;

            case CALLBAR_BIC_ROAM:

                strcat( ATCmd, "\"IX\"," );

                break;

            case CALLBAR_UAB:

                strcat( ATCmd, "\"AB\"," );

                break;

            case CALLBAR_UAG:

                strcat( ATCmd, "\"AG\"," );

                break;

            case CALLBAR_UAC:

                strcat( ATCmd, "\"AC\"," );

                break;

            case LOCK_PS:

                strcat( ATCmd, "\"PS\"," );

                break;

            case LOCK_SC:

                strcat( ATCmd, "\"SC\"," );

                break;

            case LOCK_PN:

                strcat( ATCmd, "\"PN\"," );

                break;

            case LOCK_PU:

                strcat( ATCmd, "\"PU\"," );

                break;

            case LOCK_PP:

                strcat( ATCmd, "\"PP\"," );

                break;

            case LOCK_PC:

                strcat( ATCmd, "\"PC\"," );

                break;

            case LOCK_P2:

                strcat( ATCmd, "\"P2\"," );

                break;

            default:
                goto ERROR_HANDLER;
            }

            strcat( ATCmd, OldPass );
            strcat( ATCmd, ",");
            strcat( ATCmd, NewPass );
            strcat( ATCmd, "\r" );

            retval = MAT_Send( 1, ATCmd, strlen(ATCmd) );
            gTimerID = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
            gFsmState = FSM_STATE_STEP1;
        }
    }
    else if ( FSM_STATE_STEP1 == gFsmState )
    {
        if ( DATAIN == event )
        {
            retval = GetResultErrorCode( data, datalen );

            if ( retval < 0 )
                return retval;

            KillTimer(NULL, gTimerID);
            GetResultLength( gReqHead );
            gFsmState = FSM_STATE_DONE;
        }
        else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }

    return 0;

ERROR_HANDLER:
    gReqHead->error = ME_RS_FAILURE;
    gReqHead->errcode = 1;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
TIMEOUT_HANDLER:
    gReqHead->error = ME_RS_TIMEOUT;
    gReqHead->errcode = 0;
    GetResultLength( gReqHead );
    gFsmState = FSM_STATE_DONE;

    return 0;
}

static BOOL IsTwoLineIndicatior( int index )
{
    if ( IndTable.Inds[index].msgclass == ME_URC_CMT )
        return TRUE;

    if ( IndTable.Inds[index].msgclass == ME_URC_CDS )
        return TRUE;

    if ( IndTable.Inds[index].msgclass == ME_URC_CBM )
        return TRUE;

    return FALSE;
}
/*********************************************************************\
* Function   IsUslctResultCode  
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static int IsUslctResultCode( char* buf, unsigned int buflen )
{
    int i;
    const char* keyword;
    unsigned int offset = 0;

    if ( buflen == 0 )
        return -1;

    while ( buf[offset] == '\r' || buf[offset] == '\n' )
        offset++;

    if ( offset >= buflen )
        return -1;

    buflen -= offset;

    for ( i = 0; i < MAX_QUEUE_INDICATS; i++ )
    {
        keyword = URC_String[IndTable.Inds[i].msgclass];

        if ( IndTable.Inds[i].reg_count == 0 )
            continue;

        if ( buflen < strlen( keyword ) )
            continue;

        if ( memcmp( keyword, &buf[offset], strlen( keyword ) ) == 0 )
            return i;
    }

    return -1;
}

/*********************************************************************\
* Function   IsFinalResultCode 
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static int IsFinalResultCode( char* buf, unsigned int buflen )
{
    int i = 0;
    unsigned int offset = 0;

    if ( buflen == 0 )
        return -1;

    while ( buf[offset] == '\r' || buf[offset] == '\n' )
        offset++;

    if ( offset >= buflen )
        return -1;

    buflen -= offset;

    while ( strlen( FRC_String[i]) > 0 )
    {
        if ( buflen < strlen( FRC_String[i] ) )
        {
            i++;
            continue;
        }

        if ( memcmp( FRC_String[i], &buf[offset], strlen( FRC_String[i]) ) == 0 )
            return i;

        i++;
    }

    return -1;
}

static int GetResultErrorCode( char* data, int datalen )
{
    int retval;

    retval = IsFinalResultCode( data, datalen );

    switch ( retval )
    {
    case FRC_OK:
        gReqHead->error = ME_RS_SUCCESS;
        gReqHead->errcode = 0;
        break;
    
    case FRC_ERROR:
        gReqHead->error = ME_RS_FAILURE;
        gReqHead->errcode = 1;
        break;

    case FRC_CMEERROR:
        gReqHead->error = ME_RS_FAILURE;
        if ( data[datalen-1] == '\n' && data[datalen-2] == '\r' )
            data[datalen-2] = '\0';

        gReqHead->errcode = atoi( &data[strlen(FRC_String[FRC_CMEERROR])] );
        break;

    case FRC_CMSERROR:
        gReqHead->error = ME_RS_FAILURE;
        if ( data[datalen-1] == '\n' && data[datalen-2] == '\r' )
            data[datalen-2] = '\0';

        gReqHead->errcode = atoi( &data[strlen(FRC_String[FRC_CMSERROR])] );
        break;

    case FRC_BUSY:
        gReqHead->error = ME_RS_BUSY;
        gReqHead->errcode = 0;
        break;

    case FRC_NODIALTONE:
        gReqHead->error = ME_RS_NODIALTONE;
        gReqHead->errcode = 0;
        break;

    case FRC_NOCARRIER:
        gReqHead->error = ME_RS_NOCARRIER;
        gReqHead->errcode = 0;
        break;

    case FRC_CONNECT:
        gReqHead->error = ME_RS_CONNECT;
        gReqHead->errcode = 0;
        break;
    }

    return retval;
}

static int GetUssdErrorCode (char* data, int datalen)
{
    int retval;

    retval = IsFinalResultCode (data, datalen);

    switch ( retval )
    {
    case FRC_OK:
        return retval;
    
    case FRC_ERROR:
    case FRC_CMEERROR:
    case FRC_CMSERROR:
        return FRC_ERROR;
    }

    return -1;
}

void GetResultLength (MEREQUEST* pRequest)
{
    unsigned int len = ME_MemGetOffset();
    char *str, *begin, *end;

    pRequest->result_len = -1;
    pRequest->result = NULL;

    if ( pRequest->error != ME_RS_SUCCESS )
        return;

    if ( pRequest->cmd == ME_INIT_EVERYTIME )
    {
        pRequest->result_len = sizeof( ME_INIT_GETVALUE );
        return;
    }

    str = ME_MemGetPntr();
    len = ME_MemGetOffset();

    if ( str == NULL )
        return;

    //去掉字符串开头的换行回车
    while( *str == '\r' || *str == '\n' ) {
        str++;
        len--;
    }

    if ( len <= 0 )
        return;

    switch( pRequest->cmd )
    {
	case ME_AT_SEND:

		pRequest->result_len = len;
		pRequest->result = str;

		break;

    case ME_GETLASTERROR:

    {
        if ( (str = strstr (str, "+CEER:")) == NULL )
            return;

        pRequest->result_len = sizeof(PME_EXERROR);//unsigned long
        pRequest->result = str;

        return;
    }

	case ME_GET_CLIR:

	{
		if ( (str = strstr( str, "+CLIR:")) == NULL )
			return;

		pRequest->result_len = sizeof( CLIR_SERVICE );
		pRequest->result = str;

		return;
	}


    case ME_STK_GETSAT:

        if ( (str = strstr (str, "^SSTA:")) == NULL )
            return;

        pRequest->result_len = sizeof(SAT_PARAM);
        pRequest->result = str;

        break;

    case ME_GET_AUTOANSWER:

        pRequest->result_len = sizeof(int);
        pRequest->result = str;

        break;

    case ME_SMS_READ:       //短信读取
    {
        char* t;

        t = strstr (str, "+CMGR:");

        if ( t == NULL )
        {
            t = strstr (str, "^SMGR:");

            if ( t == NULL )
                return;
        }
        str = t;

        pRequest->result_len = sizeof(SMS_INFO);
        pRequest->result = str;

        return;
    }

    case ME_SMS_MEMCOUNT:       //短信存储空间 +CPMS: "SM",3, 10,"SM",3,10
        
    {
        if ( (str = strstr (str, "+CPMS:")) == NULL )
            return;

        pRequest->result_len = sizeof(SMSMEM_INFO);
        pRequest->result = str;

        return;
    }

    case ME_PBOOK_MEMCOUNT:     //电话本存储空间 +CPBS: "SM",10, 20
        
    {
        if ( (str = strstr (str, "+CPBS:")) == NULL )
            return;

        pRequest->result_len = sizeof(PBMEM_INFO);
        pRequest->result = str;

        return;
    }

    case ME_GET_PHONEBOOKNUMBERTEXTMAXLEN:
    {
        if ((str = strstr(str,"+CPBR:")) == NULL)
            return;
        pRequest->result_len = sizeof(PBNUMTEXT_INFO);
        pRequest->result = str;
        return;
    }

    case ME_SMS_GETINDSTATUS:

    {
        if ( (str = strstr (str, "+CNMI:")) == NULL )
            return;

        pRequest->result_len = sizeof(IND_STATUS);
        pRequest->result = str;

        return;
    }

    case ME_GET_CELLBROADCAST:

    {
        if ( (str = strstr (str, "+CSCB:")) == NULL )
            return;
        
        pRequest->result_len = sizeof(CELL_BROADCAST);
        pRequest->result = str;

        return;
    }

    case ME_SMS_LIST:       //短信列表读取

    {
        int i = 0;
        char* t;

        t = strstr (str, "+CMGL:");

        if ( t == NULL )
        {
            t = strstr (str, "^SMGL:");

            if ( t == NULL )
                return;
        }

        str = t;

        for ( i = 0, begin = str; begin != NULL; i++ )
        {
            end = strchr (begin, '\r');

            if ( end == NULL )
                break;

            if ( (begin = strstr( end, "+CMGL:" )) == NULL )
                begin = strstr( end, "^SMGL:" );
        }

        pRequest->result_len = i * sizeof(SMS_INFO);
        pRequest->result = str;

        return;
    }

    case ME_GET_CALLFWDING:     //呼叫转移查询

    {
        int i = 0;
        if ( (str = strstr (str, "+CCFC:")) == NULL )
            return;
		
        for ( i = 0, begin = str; begin != NULL; i++ )
        {
            end = strchr (begin, '\r');
			
            if ( end == NULL )
                break;
			
            begin = strstr (end, "+CCFC:");
        }

        pRequest->result_len = i * sizeof (CALLFWD_INFO);
        pRequest->result = str;

        return;
    }

    case ME_PBOOK_READ:         //电话本读取

    {
        int i = 0;
        if ( (str = strstr (str, "+CPBR:")) == NULL )
            return;

        for ( i = 0, begin = str; begin != NULL; i++ )
        {
            end = strchr (begin, '\r');

            if ( end == NULL )
                break;

            begin = strstr (end, "+CPBR:");
        }

        pRequest->result_len = i * sizeof(PHONEBOOK);
        pRequest->result = str;

        return;
    }

    case ME_GET_CALLBARRING:

    {
        int i = 0;
        if ( (str = strstr (str, "+CLCK:")) == NULL )
            return;
		
        for ( i = 0, begin = str; begin != NULL; i++ )
        {
            end = strchr (begin, '\r');
			
            if ( end == NULL )
                break;
			
            begin = strstr (end, "+CLCK:");
        }
		
        pRequest->result_len = i * sizeof (CALLBAR_INFO);
        pRequest->result = str;
		
        return;
    }

    case ME_GETSIMCARDSTATUS://获得sim卡状态
    {
        if ( (str = strstr (str, "^SCKS:")) == NULL )
            return;
        
        pRequest->result_len = sizeof(SimCardStatus);
        pRequest->result = str;

        return;
    }
    case ME_PASS_REMAIN:            //PIN码可输入次数查询
    case ME_CURPASSWORD_STATUS://获得当前sim卡锁状态,主要是pin2码的状态
    {
        if ( (str = strstr (str, "^SPIC:")) == NULL )
            return;
        
        pRequest->result_len = sizeof(int);
        pRequest->result = str;

        return;
    }

    case ME_GET_LOCK:           //PIN码激活查询

    {
        if ( (str = strstr (str, "+CLCK:")) == NULL )
            return;

        pRequest->result_len = sizeof(BOOL);
        pRequest->result = str;

        return;
    }

    case ME_GET_PASS:

    {
        if ( (str = strstr (str, "+CPIN:")) == NULL )
            return;

        pRequest->result_len = sizeof(int);
        pRequest->result = str;

        return;
    }

    case ME_GET_IMEI:       //获取ME标识
    case ME_GET_IMSI:       //获取ME标识

    {
        end = strchr (str, '\r');

        if ( end == NULL )
            return;

        pRequest->result_len = ME_MBID_LEN;
        pRequest->result = str;

        return;
    }

    case ME_GET_CALLWAITING:

    {
        int i = 0;
        if ( (str = strstr (str, "+CCWA:")) == NULL )
            return;
		
        for ( i = 0, begin = str; begin != NULL; i++ )
        {
            end = strchr (begin, '\r');
			
            if ( end == NULL )
                break;
			
            begin = strstr (end, "+CCWA:");
        }
		
        pRequest->result_len = i * sizeof (CALLWAIT_INFO);
        pRequest->result = str;
		
        return;
    }

    case ME_GET_OPRATOR:        //当前运营商查询

    {
        if ( (str = strstr (str, "+COPS:")) == NULL )
            return;

        pRequest->result_len = sizeof(CUROPRATOR);
        pRequest->result = str;

        return;
    }

    case ME_SRCH_OPRATOR:       //搜索运营商列表

    {
        int i = 0;
        if ( (str = strstr (str, "+COPS:")) == NULL )
            return;

        begin = strchr (str, '(');

        if ( begin == NULL )
            return;

        while ( begin != NULL )
        {
            i++;

            end = strchr (begin, ')');

            if ( end == NULL )
                break;

            begin = strchr (end, '(');
        }

        pRequest->result_len = (i-2) * sizeof(NETOPRATOR);
        pRequest->result = str;

        return;
    }

    case ME_GET_POPRATOR:
    {
        int i = 0;

        if ( (str = strstr (str, "^SPLR:")) == NULL )
            return;

        for ( i = 0, begin = str; begin != NULL; i++ )
        {
            end = strchr (begin, '\r');

            if ( end == NULL )
                break;

            begin = strstr (end, "^SPLR:");
        }

        pRequest->result_len = i * sizeof(PREFER_OPRATOR);
        pRequest->result = str;

        return;
    }

	case ME_GET_SALSstatus:
	{
        	if ( (str = strstr (str, "^SALS:")) == NULL )
            		return;

       	pRequest->result_len = sizeof(int);
        	pRequest->result = str;
		return;
	}			
		break;

	case ME_GET_CPHSstatus:
	{
        if ( (str = strstr (str, "+CRSM:")) == NULL )
            return;

       	pRequest->result_len = sizeof(CPHSstatus_t);
        pRequest->result = str;
		return;
	}		

    case ME_GET_CPHSInfo:
    {
        if ( (str = strstr (str, "+CRSM:")) == NULL )
            return;

       	pRequest->result_len = ME_CPHSINFOMAXLEN;
        pRequest->result = str;
		return;
    }

	case ME_GET_CPHSparas:
	{
        if ( (str = strstr (str, "+CRSM:")) == NULL )
            return;

       	pRequest->result_len = sizeof(CPHS_struct);
        pRequest->result = str;
		return;
	}
    case ME_GET_CPHSparasExt:
    {
        char * p,* pNewLine;
        if ( (str = strstr (str, "+CRSM:")) == NULL )
            return;
        
        p = strstr(str,",");
        if (p == NULL)
        {
            pRequest->result_len = 0;
            pRequest->result = str;
            return;
        }
        p++;
        p = strstr(p,",");
        if (p == NULL)
        {
            pRequest->result_len = 0;
            pRequest->result = str;
            return;
        }
        p++;
        pNewLine = strchr(p,'\r');
        if (pNewLine != NULL)
        {
            * pNewLine = '\0';
        }
        else
        {
            pNewLine = strchr(p,'\n');
            if (pNewLine != NULL)
                * pNewLine = '\0';
        }
        pRequest->result_len = strlen(p) / 2 * sizeof(CPHSparasExt);
        pRequest->result = str;
        return;
    }

	case ME_GET_CONETWORKLIST:
	{
        if ( (str = strstr (str, "+CRSM:")) == NULL )
            return;

       	pRequest->result_len = sizeof(CONETWORKLIST);
        pRequest->result = str;
		return;
	}

	case ME_READ_CPHSFile:
	case ME_READ_CPHSRecord:
	{
        if ( (str = strstr (str, "+CRSM:")) == NULL )
            return;

       	pRequest->result_len = pRequest->arg2 * 2;
        pRequest->result = str;
		return;
	}		
    case ME_READ_CPHSRecordSMS:
    {
        if ( (str = strstr (str, "+CRSM:")) == NULL )
            return;

       	pRequest->result_len = sizeof(SMS_CPHS_INFO);
        pRequest->result = str;
		return;
    }

	case ME_UPDATE_CPHSFile:
	case ME_UPDATE_CPHSRecord:
	{
        	if ( (str = strstr (str, "+CRSM:")) == NULL )
            		return;

       	pRequest->result_len = 4;
        	pRequest->result = str;
		return;
	}		

	case ME_GET_SMSSERVICE:
    {
        if ( (str = strstr (str, "+CGSMS:")) == NULL )
            return;

        pRequest->result_len = 4;		//
        pRequest->result = str;

        return;
    }



	case ME_GET_EXOPERATOR:
    {
        if ( (str = strstr (str, "^SOPS:")) == NULL )
            return;

        pRequest->result_len = ME_SERVICEOPT_LEN;
        pRequest->result = str;

        return;
    }
		
	case ME_GET_HOMEZONE:
    {
        if ( (str = strstr (str, "+SHOM:")) == NULL )
            return;

        pRequest->result_len = ME_RESPONSE_INT;
        pRequest->result = str;

        return;
    }
	
	case ME_GET_CIPH:
	case ME_GET_VMWAIT1:
	case ME_GET_VMWAIT2:	
    {
        if ( (str = strstr (str, "^SIND:")) == NULL )
            return;

        pRequest->result_len = ME_RESPONSE_INT;
        pRequest->result = str;

        return;
    }		

	
    case ME_GET_CLOCK:      //时钟设置

    {
        if ( (str = strstr (str, "+CCLK:")) == NULL )
            return;

        pRequest->result_len = ME_SMS_TIME_LEN;
        pRequest->result = str;

        return;
    }

    case ME_SMS_GET_SCA:            //短信中心号码

    {
        int len;
        char p[ME_PHONENUM_LEN];
        if ( (str = strstr (str, "+CSCA:")) == NULL )
            return;

        len = ME_GetStrSection (str, p, ME_PHONENUM_LEN, "+CSCA:", 1);
        if ( len < 0 )
            return;

        pRequest->result_len = len;
        pRequest->result = str;

        return;
    }

    case ME_CURRENT_CALLS:      //当前通话列表

    {
        int i = 0;
        if ( (str = strstr (str, "+CLCC:")) == NULL )
            return;

        begin = str;

        for ( i = 0, begin = str; begin != NULL; i++ )
        {
            end = strchr (begin, '\r');

            if ( end == NULL )
                break;

            begin = strstr (end, "+CLCC:");
        }

        pRequest->result_len = i * sizeof(CALLS_INFO);
        pRequest->result = str;

        return;
    }

    case ME_GPRS_MONITOR:
        {
            if ( (str = strstr (str, "GPRS Monitor")) == NULL )
                return;

            pRequest->result_len = sizeof(MONITORINFO);
            pRequest->result = str;
        }
        return;
    case ME_GET_VOLUME:

    {
        if ( (str = strstr (str, "+CLVL:")) == NULL )
            return;

        pRequest->result_len = sizeof(int);
        pRequest->result = str;

        return;
    }

    case ME_GET_VOLUMESNFV:
    {
        if ( (str = strstr (str, "^SNFV:")) == NULL )
            return;
        pRequest->result_len = sizeof(int);
        pRequest->result = str;
        return;
    }
//fufu 0704
	case ME_GET_CHARGE:
		
    {
        if ( (str = strstr (str, "^SBC:")) == NULL )
            return;

        pRequest->result_len = sizeof(ChargeCtl_t);
        pRequest->result = str;

        return;
    }			
//end 0704
    
    case ME_GET_BATTERY:

    {
        if ( (str = strstr (str, "^SBV:")) == NULL )
            return;

        pRequest->result_len = sizeof(BATTERY_INFO);
        pRequest->result = str;

        return;
    }

	case ME_GET_MODULEINFO:

    {
        if ( (str = strstr (str, "SIEMENS")) == NULL )
            return;

        pRequest->result_len = strlen(str);
        pRequest->result = str;

        return;
    }

    case ME_GETCCM:

    {
        if ( (str = strstr (str, "+CAOC:")) == NULL )
            return;

        pRequest->result_len = sizeof(unsigned long);
        pRequest->result = str;

        return;
    }

    case ME_GETACM:

    {
        if ( (str = strstr (str, "+CACM:")) == NULL )
            return;

        pRequest->result_len = sizeof(unsigned long);
        pRequest->result = str;

        return;
    }

    case ME_GETACMAX:

    {
        if ( (str = strstr (str, "+CAMM:")) == NULL )
            return;

        pRequest->result_len = sizeof(unsigned long);
        pRequest->result = str;

        return;
    }

	case ME_TESTSACM:
    
	{
        if ( (str = strstr (str, "^SACM:")) == NULL )
            return;

        pRequest->result_len = sizeof(ACMMODE);
        pRequest->result = str;

        return;
    }

	case ME_GETSACM:

    {
        if ( (str = strstr (str, "^SACM:")) == NULL )
            return;

        pRequest->result_len = sizeof(ACMINFO);
        pRequest->result = str;

        return;
    }

    case ME_GETPUC:

    {
        if ( (str = strstr (str, "+CPUC:")) == NULL )
            return;

        pRequest->result_len = sizeof(ME_PUCINFO);
        pRequest->result = str;

        return;
    }

    case ME_GETGPRSAttachSta:
    {
        if ( (str = strstr (str, "+CGATT:")) == NULL )
            return;

        pRequest->result_len = sizeof(int);
        pRequest->result = str;

        return;		
    }
	
    case ME_GETALARM:
        
    {
        if ( (str = strstr (str, "+CALA:")) == NULL )
            return;

        pRequest->result_len = sizeof(ME_ALARMTIME);
        pRequest->result = str;

        return;
    }

    case ME_GET_SIGNAL:

    // +CSQ: 24,99
    //  Receive level: (first param)
    //      0           -113 dBm or less
    //      1           -111 dBm
    //      2...30      -109... -53 dBm
    //      31          -51 dBm or greater
    //      99          not known
    //  signal quality range:0~31, 4 steps


    //      0:  0, 99
    //      1:  1~7
    //      2:  8~15
    //      3:  16~23
    //      4:  24~31
    {
        if ( (str = strstr (str, "+CSQ:")) == NULL )
            return;

        pRequest->result_len = sizeof(int);
        pRequest->result = str;

        return;
    }

    case ME_MONI_SERV: 
    {
        if ( (str = strstr(str, "ChMod")) == NULL )
            return;

        if ( strstr (str, "No connection") != NULL )
            pRequest->error = ME_RS_NOCONNECTION;
        else if ( strstr (str, "Limited Service") != NULL )
            pRequest->error = ME_RS_LIMITSERVICE;
        else if ( strstr (str, "Reselection") != NULL )
            pRequest->error = ME_RS_RESELECTION;
        else if ( strstr (str, "Searching") != NULL )
            pRequest->error = ME_RS_SEARCHING;

        pRequest->result_len = sizeof( SCELL_INFO );
        pRequest->result = str;
        return;
    }

    case ME_MONI_NEIB:
    {
        if ( (str = strstr (str, "chann")) == NULL )
            return;

        pRequest->result_len = 6 * sizeof(NCELL_INFO);
        pRequest->result = str;
        return;
    }

    case ME_DATA_DIAL:
    case ME_GPRS_DIAL:

    {
        pRequest->result_len = strlen (str);
        pRequest->result = str;

        return;
    }

    }
}

static void GetURCLength (struct ME_INDICATION* pInd, char* pdata, int datalen)
{
    pInd->msgdata = pdata;
    pInd->msgdata_len = datalen;

    switch( pInd->msgclass )
    {
    case ME_URC_CCCM:
        pInd->msgresult_len = sizeof(unsigned long);
        break;

    case ME_URC_CBM:
        pInd->msgresult_len = sizeof(CBS_INFO);
        break;

    case ME_URC_CBMI:
        pInd->msgresult_len = sizeof(ME_CMTI_ITEM);
        break;

    case ME_URC_CCWA:
        pInd->msgresult_len = sizeof(ME_PHONE_ITEM);
        break;

    case ME_URC_CDS:
        pInd->msgresult_len = sizeof(SMS_INFO);
        break;

    case ME_URC_CDSI:
        pInd->msgresult_len = sizeof(ME_CMTI_ITEM);
        break;

    case ME_URC_CLIP:
        pInd->msgresult_len = sizeof(ME_PHONE_ITEM);
        break;

    case ME_URC_CMT:
        pInd->msgresult_len = sizeof(SMS_INFO);
        break;

    case ME_URC_CMTI:
        pInd->msgresult_len = sizeof(ME_CMTI_ITEM);
        break;

    case ME_URC_CREG:
        pInd->msgresult_len = sizeof(ME_CREG_URC);
        break;

    case ME_URC_CGREG:
        pInd->msgresult_len = sizeof(ME_CGREG_URC);
        break;

    case ME_URC_CRING:
        pInd->msgresult_len = sizeof(int);
        break;

    case ME_URC_CSSI:
        pInd->msgresult_len = sizeof(ME_CSSI_ITEM);
        break;

    case ME_URC_CSSU:
        pInd->msgresult_len = sizeof(ME_CSSU_ITEM);
        break;

    case ME_URC_SSTN:
        pInd->msgresult_len = sizeof(int);
        break;

    case ME_URC_SMGO:
        pInd->msgresult_len = sizeof(int);
        break;
	//fufu 0602
	case ME_URC_CIEV:
		pInd->msgresult_len = sizeof(ME_EXINFO_ITEM);
		break;
	//end 0602
        
    default:
        pInd->msgresult_len = -1;
        break;
    }
}

static int FillURCField (void* result, int result_len, struct ME_INDICATION* pInd)
{
    char data[600];

    memcpy( data, pInd->msgdata, pInd->msgdata_len );
    data[pInd->msgdata_len] = '\0';

    switch( pInd->msgclass )
    {
    case ME_URC_CCCM:
        return AnaURC_CCCM (data, result);

    case ME_URC_CBM:
        return AnaURC_CBM (data, result);

    case ME_URC_CBMI:
        return AnaURC_CMTI (data, result);

    case ME_URC_CCWA:
        return AnaURC_CCWA( data, result );

    case ME_URC_CDS:
        return AnaURC_CDS( data, result );

    case ME_URC_CDSI:
        return AnaURC_CMTI( data, result );

    case ME_URC_CLIP:
        return AnaURC_CLIP( data, result );

    case ME_URC_CMT:
        return AnaURC_CMT( data, result );      

    case ME_URC_CMTI:
        return AnaURC_CMTI( data, result );

    case ME_URC_CREG:
        return AnaURC_CREG( data, result );

    case ME_URC_CGREG:
        return AnaURC_CGREG( data, result );

    case ME_URC_CRING:
        return AnaURC_CRING( data, result );

    case ME_URC_CSSI:
        return AnaURC_CSSI (data, result);

    case ME_URC_CSSU:
        return AnaURC_CSSU( data, result );

    case ME_URC_SSTN:
        return AnaURC_STK( data, result );

    case ME_URC_SMGO:
        return AnaURC_SMGO( data, result );

    //fufu 0602
    case ME_URC_CIEV:
    	return AnaURC_SIND( data, result );
    //end 0602
    }

    memcpy( result, pInd->msgdata, pInd->msgdata_len );
    
    return pInd->msgdata_len;
}

static int FillResultField (void* result, int result_len, MEREQUEST* pRequest)
{
    int len;
    char *str, *begin, *end;

    str = pRequest->result;
    len = pRequest->result_len;

    switch( pRequest->cmd )
    {
        case ME_INIT_EVERYTIME:
        {
            len = Me_GetInitValue( (PME_INIT_GETVALUE)result );
            return len;
        }

		case ME_AT_SEND:
		{
			if ( result_len < len )
			{
				memcpy( result, str, result_len );
				return result_len;
			}
			else
			{
				memcpy( result, str, len );
				return len;
			}
		}

        case ME_GETLASTERROR:
#if 0
	{
            int lid, reason;

            lid = ME_GetIntSection (str, "+CEER:", 1);
            if ( lid < 0 )
                return -1;

            reason = ME_GetIntSection (str, "+CEER:", 2);
            if ( reason < 0 )
                return -1;

            *((unsigned long*)result) = MAKELONG(reason,lid);

            return len;
        }
#endif
	{
		int	Temp;
            
	     Temp = ME_GetIntSection (str, "+CEER:", 1);
            if ( Temp < 0 )
                return -1;
	     ((PME_EXERROR)result)->LocationID = Temp;	

	     Temp = ME_GetIntSection (str, "+CEER:", 2);
            if ( Temp < 0 )
                return -1;
	     ((PME_EXERROR)result)->Reason = Temp;

	     Temp = ME_GetIntSection (str, "+CEER:", 3);
            if ( Temp < 0 )
                return -1;
	     ((PME_EXERROR)result)->SSrelease = Temp;

            return len;
        }

        case ME_GET_CLIR:
        
        { 		
            int status;
	     CLIR_SERVICE	CLIR_temp;
		 
		status = ME_GetIntSection( str, "+CLIR:", 1 );
            
            if ( status == -1 )
                return -1;

            switch ( status ) 
		{
	            case 0:
	                CLIR_temp.CLIR_Setting = CLIR_DEFAULT;
	                break;
	            case 1:
	                CLIR_temp.CLIR_Setting = CLIR_ENABLE;
	                break;
	            case 2:
	               CLIR_temp.CLIR_Setting = CLIR_DISABLE;
	                break;
		     default:
			  return -1;						
            }
			
            
            status = ME_GetIntSection( str, "+CLIR:", 2 );
            
            if ( status == -1 )
                return -1;
            
            switch ( status ) {
            case 0:
                CLIR_temp.CLIR_Status = CLIR_NPROVIS;
                break;
            case 1:
                CLIR_temp.CLIR_Status  = CLIR_PERMANENT;
                break;
            case 2:
                CLIR_temp.CLIR_Status  = CLIR_UNKNOWN;
                break;
            case 3:
                CLIR_temp.CLIR_Status  = CLIR_TEMP_RES;
                break;
            case 4:
                CLIR_temp.CLIR_Status  = CLIR_TEMP_ALLOW;
                break;
	     default:
		  return -1;	
            }
            
            memcpy( result, &CLIR_temp, sizeof(CLIR_SERVICE) );
            
            return sizeof(CLIR_SERVICE);
        }

		//pin2
		case ME_PASS_VALID:
			*(int *)result = pRequest->errcode;
			break;

        case ME_SSCALL:
        {
            if ( pRequest->errcode == SSCLIP )
                return GetCLIPResult (result, str, len);
            if ( pRequest->errcode == SSCLIR )
                return GetCLIRResult (result, str, len);
            if ( pRequest->errcode == SSCOLP )
                return GetCOLPResult (result, str, len);
            if ( pRequest->errcode == SSCOLR )
                return GetCOLRResult (result, str, len);
            if ( pRequest->errcode == SSCF )
                return GetCFResult (result, str, len);
            if ( pRequest->errcode == SSCW )
                return GetCWResult (result, str, len);
            if ( pRequest->errcode == SSCB )
                return GetCBResult (result, str, len);

            return -1;
        }

        case ME_STK_GETSAT:
        {
            memset( result, 0, result_len );

            ((PSAT_PARAM)result)->state = ME_GetIntSection (str, "^SSTA:",  1);
            ((PSAT_PARAM)result)->alphabet = ME_GetIntSection (str, "^SSTA:", 2);
            ((PSAT_PARAM)result)->instance = ME_GetIntSection (str, "^SSTA:",  3);

            ME_GetStrSection (str, ((PSAT_PARAM)result)->profile, result_len, "^SSTA:", 1);

            return len;
        }

        case ME_GET_AUTOANSWER:
        {
            *((int*)(result)) = atoi (str);

            return len;
        }

	case ME_GET_SALSstatus:		
		memset( result, 0, result_len ); 

		*((int*)(result)) = ME_GetIntSection (str, "^SALS:", 2);
		break;
		
    case ME_GET_CPHSInfo:
        memset( result, 0, result_len );
        end = strchr( str, '\r' );
        if ( end != NULL )
            *end = '\0';

        strncpy(result,str + strlen("+CRSM: "),ME_CPHSINFOMAXLEN);
        return len;

	case ME_GET_CPHSstatus:
		memset( result, 0, result_len );
        if ( (str = strstr (str, "+CRSM:")) == NULL )
            return -1;

        begin = strchr(str, ',');

		begin = strchr(begin+1, ',');
		begin++;

		begin += 13*2;

		((CPHSstatus_t *)result)->FileChar = (((*begin >= 'A') ? (*begin-0x37) : (*begin-0x30)) << 4) + \
											((*(begin+1) >= 'A') ? (*(begin+1)-0x37) : (*(begin+1)-0x30));
		
		begin += 5*2;
		((CPHSstatus_t *)result)->CHV1Status = (((*begin >= 'A') ? (*begin-0x37) : (*begin-0x30)) << 4) + \
											((*(begin+1) >= 'A') ? (*(begin+1)-0x37) : (*(begin+1)-0x30));
		
		begin += 2;
		((CPHSstatus_t *)result)->UnblockCHV1 = (((*begin >= 'A') ? (*begin-0x37) : (*begin-0x30)) << 4) + \
											((*(begin+1) >= 'A') ? (*(begin+1)-0x37) : (*(begin+1)-0x30));

		begin += 2;
		((CPHSstatus_t *)result)->CHV2Status = (((*begin >= 'A') ? (*begin-0x37) : (*begin-0x30)) << 4) + \
											((*(begin+1) >= 'A') ? (*(begin+1)-0x37) : (*(begin+1)-0x30));		
		
		begin += 2;
		((CPHSstatus_t *)result)->UnblockCHV2 = (((*begin >= 'A') ? (*begin-0x37) : (*begin-0x30)) << 4) + \
											((*(begin+1) >= 'A') ? (*(begin+1)-0x37) : (*(begin+1)-0x30));	
		

		return sizeof(CPHSstatus_t);

		case ME_GET_CPHSparas:
			memset( result, 0, result_len );
	        if ( (str = strstr (str, "+CRSM:")) == NULL )
	            return -1;

			len = ME_GetIntSection (str, "+CRSM:", 1);

			switch(len)
			{
				case 0x90:
				case 0x91:
				case 0x9E:
				case 0x9F:
					break;
					
				default:
					((CPHS_struct *)result)->Type = FieldNotFound;
					((CPHS_struct *)result)->Content_Len = 0;
					((CPHS_struct *)result)->Record_Len = 0;
					return sizeof(CPHS_struct);
			}

			begin = strchr(str, ',');

			begin = strchr(begin+1, ',');
			begin++;
			
			end = begin + 4;
			((CPHS_struct *)result)->Content_Len = ((*end >= 'A') ? (*end-0x37) : (*end-0x30)) * 16*16*16 + \
												((*(end+1) >= 'A') ? (*(end+1)-0x37) : (*(end+1)-0x30)) * 16*16 + \
	        									((*(end+2) >= 'A') ? (*(end+2)-0x37) : (*(end+2)-0x30)) * 16 + \
	        									((*(end+3) >= 'A') ? (*(end+3)-0x37) : (*(end+3)-0x30)) * 1;

			end = begin + 28;
			((CPHS_struct *)result)->Record_Len = ((*end >= 'A') ? (*end-0x37) : (*end-0x30)) * 16 + \
												((*(end+1) >= 'A') ? (*(end+1)-0x37) : (*(end+1)-0x30)) * 1;

			if(((CPHS_struct *)result)->Record_Len)
				((CPHS_struct *)result)->Type = RecordContent;
			else
				((CPHS_struct *)result)->Type = FileContent;
			
			return sizeof(CPHS_struct);

        case ME_GET_CPHSparasExt:
            {
                char * p,* pResult;
                unsigned char cNum[10] = "";
                int CNLLen,nCount,i,iCNL;
                memset( result, 0, result_len );
                if ( (str = strstr (str, "+CRSM:")) == NULL )
                    return -1;
                p = strstr(str,",");
                if (p == NULL)
                    return -1;
                p++;
                p = strstr(p,",");
                if (p == NULL)
                    return 0;
                p++;

                CNLLen = strlen(p);
                if (CNLLen % 6 != 0)
                    return -1;
                nCount = CNLLen / 2;//数组个数
                pResult = result;
                for (i = 0;i < nCount;i++)
                {
                    memset(cNum,0x00,sizeof(cNum));
                    strncpy(cNum,p,2);
                    p += 2;

                    iCNL = atoi(cNum);
                    iCNL = iCNL << 5;
                    iCNL = iCNL >> 5;
                    sprintf(cNum,"%03d",iCNL);
                    memcpy(pResult + i * sizeof(CPHSparasExt),cNum,sizeof(CPHSparasExt));
                }
            }
            return result_len;
            
		case ME_READ_CPHSFile:
		case ME_READ_CPHSRecord:
			{
//				int length;

		        if ( (str = strstr (str, "+CRSM:")) == NULL )
		            return -1;

				switch(ME_GetIntSection (str, "+CRSM:", 1))
				{
					case 0x90:
					case 0x91:
					case 0x9E:
					case 0x9F:
						break;
						
					default:
						return -1;
				}		

				begin = strchr(str, ',');
				begin = strchr(begin+1, ',');
				begin++;

				memcpy((char *)result, begin, result_len);
			}
			return result_len;

		case ME_GET_CONETWORKLIST:
			{
				PCONETWORKLIST p;
				
				p = (PCONETWORKLIST)result;
				p->amount = 0;

		        if ( (str = strstr (str, "+CRSM:")) == NULL )
		            return -1;

				switch(ME_GetIntSection (str, "+CRSM:", 1))
				{
					case 0x90:
					case 0x91:
					case 0x9E:
					case 0x9F:
						break;
						
					default:
						return -1;
				}		

				begin = strchr(str, ',');
				begin = strchr(begin+1, ',');
				begin++;

#define ISNUMBER(c) ((c)>='0'&&(c)<='9')
#define ISLOCASE(c)	((c)>='a'&&(c)<='f')
#define ISUPCASE(c)	((c)>='A'&&(c)<='F')
#define CHARTONUM(c) (ISNUMBER(c)?((c)-'0'):(ISUPCASE(c)?(c-'A'+10):(c-'a'+10)))

				while(strlen(begin) >= 12)
				{
					if(!isalnum(begin[11]))
						break;

				//	if(mcc == 0x0fff)
					if(begin[0] == 'F' && begin[1] == 'F' && begin[2] == 'F')
						break;
					
					p->conetwork[p->amount].mcc = CHARTONUM(begin[0])*100 + CHARTONUM(begin[1])*10 + CHARTONUM(begin[2]);// byte1 to byte3 ???;
					p->conetwork[p->amount].mnc = CHARTONUM(begin[3])*100 + CHARTONUM(begin[4])*10 + CHARTONUM(begin[5]);// byte1 to byte3 ???;
					p->conetwork[p->amount].network  = CHARTONUM(begin[6]) * 10 + CHARTONUM(begin[7]);
					p->conetwork[p->amount].provider = CHARTONUM(begin[8]) * 10 + CHARTONUM(begin[9]);
					p->conetwork[p->amount].coporate = CHARTONUM(begin[10]) * 10 + CHARTONUM(begin[11]);
					p->amount ++;
					if(p->amount >= 8)
						break;
					begin += 12;
				}
			}
			return result_len;

        case ME_READ_CPHSRecordSMS:
            {
                SMS_CPHS_INFO sms_cphs_info;
                memset(&sms_cphs_info,0x00,sizeof(SMS_CPHS_INFO));
                memset( result, 0, result_len );
                AnalyseCPHSRecordSMS(&sms_cphs_info,str);
                memcpy(result,&sms_cphs_info,sizeof(SMS_CPHS_INFO));
            }
            return len;

		case ME_UPDATE_CPHSFile:
		case ME_UPDATE_CPHSRecord:
        		if ( (str = strstr (str, "+CRSM:")) == NULL )
            			return -1;
				
			switch(ME_GetIntSection (str, "+CRSM:", 1))
			{
				case 0x90:
				case 0x91:
				case 0x9E:
				case 0x9F:
					*((int *)pRequest->result) = 1;
					break;
						
				default:
					*((int *)pRequest->result) = 0;
			}
       		pRequest->result_len = result_len;
			return result_len;

		case ME_GET_SMSSERVICE:
			memset( result, 0, result_len );

			*((int*)(result)) = ME_GetIntSection (str, "+CGSMS:", 1);
			
			return len;

		case ME_GET_CIPH:
		case ME_GET_VMWAIT1:
		case ME_GET_VMWAIT2:
			memset( result, 0, result_len );

			*((int*)(result)) = ME_GetIntSection (str, "^SIND:",  3);
			
			return 1;

		case ME_GET_EXOPERATOR:
		{
			int len = 0 ;
			memset( result, 0, result_len );

			len = ME_GetStrSection (str, result, result_len, "^SOPS:", 2);

			if(len > 0)
				str[len-1] = '\0';
			else
				str[0] = '\0';
				
			return 1;
		}
		
		case ME_GET_HOMEZONE:

			memset( result, 0, result_len );

			*((int*)(result)) = ME_GetIntSection (str, "^SHOM:",  1);
			
			return 1;	
//end 0531

        case ME_SMS_READ:       //短信读取

        {
            memset( result, 0, result_len );

            ((PSMS_INFO)result)->Index = pRequest->arg1;

            end = strchr( str, '\r' );

            if ( end == NULL )
                return -1;

            *end = '\0';

            if ( ME_RecStr_Flag( (PSMS_INFO)result, str ) == -1 )
                return -1;

            begin = end + 2;
            end = strchr( begin, '\r' );

            if ( end == NULL )
                return -1;

            *end = '\0';

            ME_RecStr_Code ((PSMS_INFO)result, begin, ((PSMS_INFO)result)->Stat);

            if ( ((PSMS_INFO)result)->ConLen == -1 )
                return -1;

            return len;
        }

        case ME_SMS_MEMCOUNT:       //短信存储空间 +CPMS: "SM",3, 10,"SM",3,10

        {
            memset( result, 0, result_len );

            ((PSMSMEM_INFO)result)->nMem1Used = ME_GetIntSection( str, "+CPMS:",  1 );
            ((PSMSMEM_INFO)result)->nMem1Total = ME_GetIntSection( str, "+CPMS:", 2 );
            ((PSMSMEM_INFO)result)->nMem2Used = ME_GetIntSection( str, "+CPMS:",  3 );
            ((PSMSMEM_INFO)result)->nMem2Total = ME_GetIntSection( str, "+CPMS:", 4 );
            ((PSMSMEM_INFO)result)->nMem3Used = ME_GetIntSection( str, "+CPMS:",  5 );
            ((PSMSMEM_INFO)result)->nMem3Total = ME_GetIntSection( str, "+CPMS:", 6 );

            return len;
        }

        case ME_PBOOK_MEMCOUNT:     //电话本存储空间 +CPBS: "SM",10, 20

        {
            char memtype[3];
            memset( result, 0, result_len );

            ((PPBMEM_INFO)result)->Used = ME_GetIntSection( str, "+CPBS:", 1 );
            ((PPBMEM_INFO)result)->Total = ME_GetIntSection( str, "+CPBS:", 2 );

            ME_GetStrSection (str, memtype, 3, "+CPBS:", 1);

            if (memcmp( memtype, "SM", 2 ) == 0)
                ((PPBMEM_INFO)result)->Type = PBOOK_SM;
            else if (memcmp( memtype, "ME", 2 ) == 0)
                ((PPBMEM_INFO)result)->Type = PBOOK_ME;
            else if (memcmp( memtype, "FD", 2 ) == 0)
                ((PPBMEM_INFO)result)->Type = PBOOK_FD;
            else if (memcmp( memtype, "LD", 2 ) == 0)
                ((PPBMEM_INFO)result)->Type = PBOOK_LD;
            else if (memcmp( memtype, "MC", 2 ) == 0)
                ((PPBMEM_INFO)result)->Type = PBOOK_MC;
            else if (memcmp( memtype, "RC", 2 ) == 0)
                ((PPBMEM_INFO)result)->Type = PBOOK_RC;
            else if (memcmp( memtype, "ON", 2 ) == 0)
                ((PPBMEM_INFO)result)->Type = PBOOK_ON;
            else
                return -1;

            return len;
        }

        case ME_GET_PHONEBOOKNUMBERTEXTMAXLEN:
        {
            char cNumber[10] = "",cText[10] = "",* pNumber = NULL,* pText = NULL;
            int iLen;
            PBNUMTEXT_INFO pbinfo;
            memset( result, 0, result_len );
            pNumber = strstr(str,",");
            if (pNumber == NULL)
                return -1;
            
            pNumber++;
            pText = strstr(pNumber,",");
            if (pText == NULL)
                return -1;
            iLen = abs(pNumber - pText);
            strncpy(cNumber,pNumber,iLen);
            strcpy(cText,pText + 1);

            pbinfo.iNumberMaxLen = atoi(cNumber);
            pbinfo.iTextMaxLen = atoi(cText);
            memcpy(result,&pbinfo,sizeof(PBNUMTEXT_INFO));
            return len;
        }
        case ME_SMS_GETINDSTATUS:

        {
            PIND_STATUS pStatus;
            
            pStatus = (PIND_STATUS)result;

            pStatus->Mt = ME_GetIntSection( str, "+CNMI:", 2 );
            if ( pStatus->Mt == -1 )
                return -1;

            pStatus->Bm = ME_GetIntSection( str, "+CNMI:", 3 );
            if ( pStatus->Bm == -1 )
                return -1;

            pStatus->Ds = ME_GetIntSection( str, "+CNMI:", 4 );
            if ( pStatus->Ds == -1 )
                return -1;

            if ( pStatus->Mt == 0 )
                pStatus->Mt = IND_NONE;
            else if ( pStatus->Mt == 1 )
                pStatus->Mt = IND_BUFFER;
            else
                pStatus->Mt = IND_DIRECT;

            if ( pStatus->Bm == 0 )
                pStatus->Bm = IND_NONE;
            else if ( pStatus->Bm == 1 )
                pStatus->Bm = IND_BUFFER;
            else
                pStatus->Bm = IND_DIRECT;

            if ( pStatus->Ds == 0 )
                pStatus->Ds = IND_NONE;
            else if ( pStatus->Mt == 1 )
                pStatus->Ds = IND_DIRECT;
            else
                pStatus->Ds = IND_BUFFER;

            return len;
        }

        case ME_GET_CELLBROADCAST:

        {
            PCELL_BROADCAST pcb;
        
            pcb = (PCELL_BROADCAST)result;

            if ( ME_GetIntSection( str, "+CSCB:", 1 ) == 1 )
                pcb->mode = FALSE;
            else
                pcb->mode = TRUE;

            return len;
        }

        case ME_SMS_LIST:       //短信列表读取

        {
            int retval;

            memset( result, 0, result_len );

            retval = ME_RecLstStr_Code( result, str, result_len/sizeof(SMS_INFO) );

            return retval*sizeof(SMS_INFO);
        }

        case ME_GET_CALLFWDING:     //呼叫转移查询

        {
            int ret, count = 0;

            memset( result, 0, result_len );

            
			while ((end = strchr( str, '\r' )) != NULL ) {

	            *end = '\0';
	
		        ret = ME_GetIntSection( str, "+CCFC:", 1 );

			    if ( ret == -1 )
				    return -1;

				if ( ret == 1 )
				{
					((PCALLFWD_INFO)result)[count].Enable = TRUE;

					ME_GetStrSection (str, ((PCALLFWD_INFO)result)[count].PhoneNum, ME_PHONENUM_LEN, "+CCFC:", 1);
				}
				else
					((PCALLFWD_INFO)result)[count].Enable = FALSE;

				ret = ME_GetIntSection (str, "+CCFC:", 2);
				switch (ret) {
				case 1:		((PCALLFWD_INFO)result)[count].Class = CLASS_VOICE; break;
				case 2:		((PCALLFWD_INFO)result)[count].Class = CLASS_DATA; break;
				case 4:		((PCALLFWD_INFO)result)[count].Class = CLASS_FAX; break;
				case 8:		((PCALLFWD_INFO)result)[count].Class = CLASS_SMS; break;
				default:	return (count* sizeof(CALLFWD_INFO));
				}

				((PCALLFWD_INFO)result)[count].Time = ME_GetIntSection( str, "+CCFC:", 4 );

				count ++;
				str = end + 1;
			}

            return count * sizeof(CALLFWD_INFO);
        }

        case ME_PBOOK_READ:         //电话本读取

        {
            unsigned int i;

            end = str-2;

            for ( i=0; i < result_len/sizeof(PHONEBOOK); i++ )
            {
                begin = end + 2;

                while ( *begin == '\r' )
                    begin += 2;

                if ( strlen (begin) == 0 )
                    return i*sizeof(PHONEBOOK);

                end = strchr( begin, '\r' );

                if (end == NULL)
                    return i*sizeof(PHONEBOOK);
        
                *end = '\0';

                ME_PhoneBook_Dec( begin, (PPHONEBOOK)((char*)result+i*sizeof(PHONEBOOK)) );
            }

            return i*sizeof(PHONEBOOK);
        }

        case ME_GET_CALLBARRING:

			{
				int ret, count = 0;
				
				memset( result, 0, result_len );
				
				
				while ((end = strchr( str, '\r' )) != NULL ) {
					
					*end = '\0';
					
					ret = ME_GetIntSection( str, "+CLCK:", 1 );
					
					if ( ret == -1 )
						return -1;
					
					if ( ret == 1 )
						((PCALLBAR_INFO)result)[count].Enable = TRUE;
					else
						((PCALLBAR_INFO)result)[count].Enable = FALSE;
					
					ret = ME_GetIntSection (str, "+CLCK:", 2);
					switch (ret) {
					case 1:		((PCALLBAR_INFO)result)[count].Class = CLASS_VOICE; break;
					case 2:		((PCALLBAR_INFO)result)[count].Class = CLASS_DATA; break;
					case 4:		((PCALLBAR_INFO)result)[count].Class = CLASS_FAX; break;
					case 8:		((PCALLBAR_INFO)result)[count].Class = CLASS_SMS; break;
					default:	return (count* sizeof(CALLBAR_INFO));
					}
					count ++;
					str = end + 1;
				}
				
				return count * sizeof(CALLBAR_INFO);
			}

        case ME_PASS_REMAIN:            //PIN码可输入次数查询

        {
            int ret;

            ret = ME_GetIntSection( str, "^SPIC:", 1 );

            if ( ret == -1 )
                return -1;

            memcpy( result, &ret, sizeof( int ) );

            return len;
        }

        case ME_GETSIMCARDSTATUS:
        {
            SimCardStatus simcardstatus;
            memset( result, 0, result_len );
            if (strstr(str,"^SCKS: 1,1") != NULL)
            {
                simcardstatus.SimCardUrc = OUTPUTURC;
                simcardstatus.SimCard = SIMINSERT;
            }
            else if (strstr(str,"^SCKS: 0,1") != NULL)
            {
                simcardstatus.SimCardUrc = SUPPRESSURC;
                simcardstatus.SimCard = SIMINSERT;
            }
            else if (strstr(str,"^SCKS: 1,0") != NULL)
            {
                simcardstatus.SimCardUrc = OUTPUTURC;
                simcardstatus.SimCard = NOSIMCARD;
            }
            else
            {
                simcardstatus.SimCardUrc = SUPPRESSURC;
                simcardstatus.SimCard = NOSIMCARD;
            }
            memcpy(result,&simcardstatus,sizeof(SimCardStatus));
            return len;
        }
        case ME_CURPASSWORD_STATUS://获得当前sim卡锁状态,主要是pin2码的状态
        {
            int ret;
            if (strstr(str,"SIM PIN2") != NULL)
                ret = PIN2;
            else if (strstr(str,"SIM PUK2") != NULL)
                ret = PUK2;
            else if (strstr(str,"SIM PIN") != NULL)
                ret = PIN1;
            else if (strstr(str,"SIM PUK") != NULL)
                ret = PUK1;
            else if (strstr(str,"PH-SIM PIN") != NULL)
                ret = PHSIMPIN;
            else if (strstr(str,"PH-SIM PUK") != NULL)
                ret = PHSIMPUK;
            else if (strstr(str,"PH-NET PUK") != NULL)
                ret = PHNETPUK;
            else
                ret = -1;

            memcpy( result, &ret, sizeof( int ) );
            return len;
        }
        case ME_GET_LOCK:           //PIN码激活查询

        {
            int ret;

            ret = ME_GetIntSection( str, "+CLCK:", 1 );

            if ( ret == -1 )
                return -1;

            memcpy( result, &ret, sizeof( int ) );

            return len;
        }

        case ME_GET_PASS:

        {
            int ret;

            if ( strstr( str, "READY") != NULL )
                ret = NOPIN;
            else if ( strstr( str, "SIM PIN2" ) != NULL )
                ret = PIN2;
            else if ( strstr( str, "SIM PUK2" ) != NULL )
                ret = PUK2;
            else if ( strstr( str, "SIM PIN" ) != NULL )
                ret = PIN1;
            else if ( strstr( str, "SIM PUK" ) != NULL )
                ret = PUK1;
            else if ( strstr( str, "PH-SIM PIN" ) != NULL )
                ret = SIMPHLOCK;
            else if ( strstr( str, "PH-NET PIN" ) != NULL )
                ret = SIMNTLOCK;
            else
                return -1;

            *((int*)result) = ret;

            return len;
        }

        case ME_GET_IMEI:       //获取ME标识
        case ME_GET_IMSI:

        {
            end = strchr( str, '\r' );

            if ( end == NULL )
                return -1;

            *end = '\0';

            memcpy( result, str, len );

            return len;
        }

        case ME_GET_CALLWAITING:

			{
				int ret, count = 0;
				
				memset( result, 0, result_len );
				
				
				while ((end = strchr( str, '\r' )) != NULL ) {
					
					*end = '\0';
					
					ret = ME_GetIntSection( str, "+CCWA:", 1 );
					
					if ( ret == -1 )
						return -1;
					
					if ( ret == 1 )
						((PCALLWAIT_INFO)result)[count].Enable = TRUE;
					else
						((PCALLWAIT_INFO)result)[count].Enable = FALSE;
					
					ret = ME_GetIntSection (str, "+CCWA:", 2);
					switch (ret) {
					case 1:		((PCALLWAIT_INFO)result)[count].Class = CLASS_VOICE; break;
					case 2:		((PCALLWAIT_INFO)result)[count].Class = CLASS_DATA; break;
					case 4:		((PCALLWAIT_INFO)result)[count].Class = CLASS_FAX; break;
					case 8:		((PCALLWAIT_INFO)result)[count].Class = CLASS_SMS; break;
					default:	return (count* sizeof(CALLWAIT_INFO));
					}
					count ++;
					str = end + 1;
				}
				
				return count * sizeof(CALLWAIT_INFO);
			}
			
        case ME_GET_OPRATOR:        //当前运营商查询

        {
            ((PCUROPRATOR)result)->Mode = ME_GetIntSection( str, "+COPS:", 1 );
            ((PCUROPRATOR)result)->Format = ME_GetIntSection( str, "+COPS:", 2 );

            if ( ((PCUROPRATOR)result)->Mode == -1 )
                return -1;

            if ( ((PCUROPRATOR)result)->Format != -1 )
                ME_GetStrSection (str, ((PCUROPRATOR)result)->Name, ME_OPRATOR_LEN, "COPS:", 1);

            return len;
        }

        case ME_SRCH_OPRATOR:       //搜索运营商列表

        {
            int retval;

            memset( result, 0, result_len );

            retval = ME_NetOpratorDec( result, str, result_len/sizeof(NETOPRATOR) );

            return retval*sizeof(NETOPRATOR);
        }

        case ME_GET_POPRATOR:

        {
            int count;
            PPREFER_OPRATOR po;

            count = 0;
            po = result;
            begin = str;

            while ( (end = strstr( begin, "\r\n" )) != NULL )
            {
                *end = '\0';

                po[count].index = ME_GetIntSection( begin, "^SPLR:", 1 );

                ME_GetStrSection( begin, po[count].oper_id, ME_OPRATOR_LEN, "^SPLR:", 1 );

                count++;

                if ( count*sizeof(PREFER_OPRATOR) >= (unsigned int)result_len )
                    break;

                begin = end+2;

                while ( *begin == '\r' )
                    begin += 2;

                if ( begin == NULL )
                    break;
            }

            return count*sizeof( PREFER_OPRATOR );
        }

        case ME_GET_CLOCK:      //时钟设置

        {
            int retval;

            memset( result, 0, result_len );

            retval = ME_GetStrSection( str, (char*)result, ME_SMS_TIME_LEN, "+CCLK:", 1 );
            if ( retval < 0 )
                return -1;

            return retval;
        }

        case ME_SMS_GET_SCA:            //短信中心号码

        {
            int retval;

            memset( result, 0, result_len );

            retval = ME_GetStrSection( str, (char*)result, ME_PHONENUM_LEN, "+CSCA:", 1 );
            
            if ( retval < 0 )
                return -1;

            return retval;
        }

        case ME_CURRENT_CALLS:      //当前通话列表

        {
            unsigned int i;

            memset( result, 0, result_len );

            end = str-2;

            for ( i = 0; i < result_len/sizeof(CALLS_INFO); i++ )
            {
                begin = end + 2;

                while ( *begin == '\r' )
                    begin += 2;

                if ( strlen (begin) == 0 )
                    return i*sizeof(CALLS_INFO);

                end = strchr( begin, '\r' );

                if ( end == NULL || end == begin )
                    return i*sizeof(CALLS_INFO);
        
                *end = '\0';

                ME_CurCalls_Dec( begin, (PCALLS_INFO)((char*)result+i*sizeof(CALLS_INFO)) );
            }

            return i* sizeof( CALLS_INFO );
        }

        case ME_GPRS_MONITOR:
        {
            char * pMonitor = 0;
            MONITORINFO Monitor;
            int i;
            memset(&Monitor,0x00,sizeof(MONITORINFO));

            pMonitor = strstr(str,"# Cell #");
            if (pMonitor == NULL)
                return -1;
            if (strstr(str,MONITORSEARCHING) != NULL)
            {
                strcpy(Monitor.BCCH,MONITORSEARCHING);
                memcpy(result,&Monitor,sizeof(MONITORINFO));
                return sizeof(MONITORINFO);
            }
            pMonitor += strlen("# Cell #");

            while ((*pMonitor == '\r') || (* pMonitor == '\n') || (*pMonitor == ' '))
                pMonitor++;
            i = 0;
            while ((*pMonitor != '\r') && (* pMonitor != '\n') && (*pMonitor != ' '))
            {
                if (i == sizeof(Monitor.BCCH))
                    break;
                strncat(Monitor.BCCH,pMonitor++,1);
                i++;
            }

            while ((*pMonitor == '\r') || (* pMonitor == '\n') || (*pMonitor == ' '))
                pMonitor++;
            i = 0;
            while ((*pMonitor != '\r') && (* pMonitor != '\n') && (*pMonitor != ' '))
            {
                if (i == sizeof(Monitor.G))
                    break;
                strncat(Monitor.G,pMonitor++,1);
                i++;
            }

            while ((*pMonitor == '\r') || (* pMonitor == '\n') || (*pMonitor == ' '))
                pMonitor++;
            i = 0;
            while ((*pMonitor != '\r') && (* pMonitor != '\n') && (*pMonitor != ' '))
            {
                if (i == sizeof(Monitor.PBCCH))
                    break;
                strncat(Monitor.PBCCH,pMonitor++,1);
                i++;
            }

            while ((*pMonitor == '\r') || (* pMonitor == '\n') || (*pMonitor == ' '))
                pMonitor++;
            i = 0;
            while ((*pMonitor != '\r') && (* pMonitor != '\n') && (*pMonitor != ' '))
            {
                if (i == sizeof(Monitor.PAT))
                    break;
                strncat(Monitor.PAT,pMonitor++,1);
                i++;
            }

            while ((*pMonitor == '\r') || (* pMonitor == '\n') || (*pMonitor == ' '))
                pMonitor++;
            i = 0;
            while ((*pMonitor != '\r') && (* pMonitor != '\n') && (*pMonitor != ' '))
            {
                if (i == sizeof(Monitor.MCC))
                    break;
                strncat(Monitor.MCC,pMonitor++,1);
                i++;
            }

            while ((*pMonitor == '\r') || (* pMonitor == '\n') || (*pMonitor == ' '))
                pMonitor++;
            i = 0;
            while ((*pMonitor != '\r') && (* pMonitor != '\n') && (*pMonitor != ' '))
            {
                if (i == sizeof(Monitor.MNC))
                    break;
                strncat(Monitor.MNC,pMonitor++,1);
                i++;
            }

            while ((*pMonitor == '\r') || (* pMonitor == '\n') || (*pMonitor == ' '))
                pMonitor++;
            i = 0;
            while ((*pMonitor != '\r') && (* pMonitor != '\n') && (*pMonitor != ' '))
            {
                if (i == sizeof(Monitor.NOM))
                    break;
                strncat(Monitor.NOM,pMonitor++,1);
                i++;
            }

            while ((*pMonitor == '\r') || (* pMonitor == '\n') || (*pMonitor == ' '))
                pMonitor++;
            i = 0;
            while ((*pMonitor != '\r') && (* pMonitor != '\n') && (*pMonitor != ' '))
            {
                if (i == sizeof(Monitor.TA))
                    break;
                strncat(Monitor.TA,pMonitor++,1);
                i++;
            }

            while ((*pMonitor == '\r') || (* pMonitor == '\n') || (*pMonitor == ' '))
                pMonitor++;
            i = 0;
            while ((*pMonitor != '\r') && (* pMonitor != '\n') && (*pMonitor != ' '))
            {
                if (i == sizeof(Monitor.TAC))
                    break;
                strncat(Monitor.TAC,pMonitor++,1);
                i++;
            }
            memcpy(result,&Monitor,sizeof(MONITORINFO));
            return sizeof(MONITORINFO);
        }
        case ME_GET_VOLUME:

        {
            int level;

            level = ME_GetIntSection( str, "+CLVL:", 1 );

            if ( level == -1 )
                return -1;

            *((int*)result) = level;

            return sizeof(int);
        }

        case ME_GET_VOLUMESNFV:
        {
            int level;

            level = ME_GetIntSection( str, "^SNFV:", 1 );

            if ( level == -1 )
                return -1;

            *((int*)result) = level;

            return sizeof(int);
        }
        case ME_GETCCM:
        {
            char value[12];

            ME_GetStrSection (str, value, sizeof(value), "+CAOC:", 1);
            *((unsigned long*)result) = AOCHexDecode (value);

            return sizeof (unsigned long);
        }

        case ME_GETACM:
        {
            char value[12];

            ME_GetStrSection (str, value, sizeof(value), "+CACM:", 1);
            *((unsigned long*)result) = AOCHexDecode (value);

            return sizeof (unsigned long);
        }

        case ME_GETACMAX:
        {
            char value[12];

            ME_GetStrSection (str, value, sizeof(value), "+CAMM:", 1);
            *((unsigned long*)result) = AOCHexDecode (value);

            return sizeof (unsigned long);
        }

        case ME_TESTSACM:

        {
            int mode, i = 0;

			if(len < sizeof(ACMMODE))
				return -1;

            mode = ME_GetIntSection( str, "^SACM:", 1 );

            while( mode != -1 )
			{
				((PACMMODE)result)->mode[i] = mode;
				i ++;
				if(i >= 8)
					break;
				mode = ME_GetIntSection( str, "^SACM:", i+1 );
			}
			
			((PACMMODE)result)->nNum = i;

			if(0 == i)
                return -1;
			else
				return sizeof(ACMMODE);
        }

        case ME_GETSACM:

        {
            int mode;
			char value[12];

			if(len < sizeof(ACMINFO))
				return -1;

            mode = ME_GetIntSection( str, "^SACM:", 1 );

            if( mode == -1 )
                return -1;

			((PACMINFO)result)->mode = mode;

            mode = ME_GetStrSection( str, value, strlen(value), "^SACM:", 1 );

            if( mode == -1 )
                return -1;

			((PACMINFO)result)->acm = AOCHexDecode(value);

            mode = ME_GetStrSection( str, value, strlen(value), "^SACM:", 2 );

            if( mode == -1 )
                return -1;

			((PACMINFO)result)->acmMax = AOCHexDecode(value);

			return sizeof(ACMINFO);
        }

        case ME_GETPUC:
        {
            PME_PUCINFO info = (PME_PUCINFO)result;
            ME_GetStrSection (str, info->currency, sizeof(info->currency), "+CPUC:", 1);
            ME_GetStrSection (str, info->ppu, sizeof(info->ppu), "+CPUC:", 2);

            return sizeof (*info);
        }

		case ME_GETGPRSAttachSta:
		{
			*((int *)result) = ME_GetIntSection( str, "+CGATT:", 1 );
			
			return sizeof(int);
		}
	 
        case ME_GETALARM:
        {
            char fmt[40], *ptr1, *ptr2;
            int ret;
            ME_ALARMTIME* time = (ME_ALARMTIME*)result;

            memset (fmt, '\0', sizeof(fmt));
            ret = ME_GetStrSection( str, fmt, sizeof(fmt), "+CALA:", 1 );

            if ( ret < 0 )
                return -1;

            ptr1 = fmt;
            ptr2 = strchr (fmt, '/');
            if ( ptr2 == NULL )
                return -1;
            *ptr2 = '\0';
            time->year = (unsigned char)atoi (ptr1);

            ptr1 = ptr2+1;
            ptr2 = strchr (fmt, '/');
            if ( ptr2 == NULL )
                return -1;
            *ptr2 = '\0';
            time->month = (unsigned char)atoi (ptr1);
            
            ptr1 = ptr2+1;
            ptr2 = strchr (fmt, ',');
            if ( ptr2 == NULL )
                return -1;
            *ptr2 = '\0';
            time->day = (unsigned char)atoi (ptr1);

            ptr1 = ptr2+1;
            ptr2 = strchr (fmt, ':');
            if ( ptr2 == NULL )
                return -1;
            *ptr2 = '\0';
            time->hour = (unsigned char)atoi (ptr1);
        
            ptr1 = ptr2+1;
            ptr2 = strchr (fmt, ':');
            if ( ptr2 == NULL )
                return -1;
            *ptr2 = '\0';
            time->minute = (unsigned char)atoi (ptr1);

            ptr1 = ptr2+1;
            if ( *ptr1 == '\0' )
                return -1;
            time->second = (unsigned char)atoi (ptr1);

            time->unused1 = time->unused2 = 0;

            return len;
        }


		case ME_GET_CHARGE:
        {
            int level;

            level = ME_GetIntSection( str, "^SBC:", 1 );

            if ( level == -1 )
                return -1;

			((ChargeCtl_t *)result)->ChargeStatus = level;

            level = ME_GetIntSection( str, "^SBC:", 2 );

            if ( level == -1 )
                return -1;

			((ChargeCtl_t *)result)->BatteryCapacity = level;            

             level = ME_GetIntSection( str, "^SBC:", 3 );

            if ( level == -1 )
                return -1;

			((ChargeCtl_t *)result)->PowerConsumption = level;              

            return len;
        }
			
//end 0704

		case ME_GET_MODULEINFO:
        {
			strcpy((char*)result, str);
			return len;
        }

        case ME_GET_BATTERY:

        {
            int level;

            level = ME_GetIntSection( str, "^SBV:", 1 );

            if ( level == -1 )
                return -1;

            ((PBATTERY_INFO)result)->BCValue = level;
            level = (100L*(level-3600))/(4200-3600);
            ((PBATTERY_INFO)result)->BCLever = level;
            return len;
        }
        case ME_GET_SIGNAL:

        // +CSQ: 24,99
        //  Receive level: (first param)
        //      0           -113 dBm or less
        //      1           -111 dBm
        //      2...30      -109... -53 dBm
        //      31          -51 dBm or greater
        //      99          not known
        //  signal quality range:0~31, 4 steps


        //      0:  0, 99
        //      1:  1~7
        //      2:  8~15
        //      3:  16~23
        //      4:  24~31
        {
            int value;

            value = ME_GetIntSection( str, "+CSQ:", 1 );

            if ( value == -1 )
                return -1;

            if ( value == 99 || value == 0)
                value = 0;
            else
                value = value/8 + 1;

            *((int*)result) = value;

            return len;
        }

        case ME_MONI_SERV:

        {
            memset( result, 0, result_len );

            return ServCell_Decode( result, str, pRequest->error );
        }

        case ME_MONI_NEIB:

        {
            memset( result, 0, result_len );

            return NeibCell_Decode( result, str );
        }

        case ME_DATA_DIAL:
        case ME_GPRS_DIAL:

        {
            if ( strlen( str ) > (unsigned int)result_len )
            {
                memcpy( result, str, result_len );
                return result_len;
            }
            else
            {
                strcpy( result, str );
                return len;
            }
        }

        default:

            return -1;
    }
	return -1;
}

/*---------------------------------------------------
 *  +CRING: <type>
 *  "VOICE"     for normal speech
 *  "FAX"       for fax calls
 *  "ASYNC"     for asynchronous transparent
 *  "REL ASYNC" for asynchronous non-transparent
 *---------------------------------------------------*/
static int AnaURC_CRING( char *SrcStr, int* Type )
{
    char *pCRINGStr = SrcStr;

    if ( memcmp( pCRINGStr, "+CRING:", 7 ) != 0 )
        return -1;

    if( strcmp( pCRINGStr, "VOICE" ) ==0 )
        *Type = ME_CRING_VOICE;

    else if( strcmp( pCRINGStr, "FAX" ) ==0 )
        *Type = ME_CRING_FAX;

    else if( strcmp( pCRINGStr, "ASYNC" ) ==0 )
        *Type = ME_CRING_ASYNC;

    else if( strcmp( pCRINGStr, "REL ASYNC" ) ==0 )
        *Type = ME_CRING_REL_ASYNC;

    else
        return -1;

    return sizeof(int);
}

/*-------------------------------------------------------------
 *  +CLIP: <number>, <type>[ ,<subaddr>, <satype>, <alpha> ]
 *  +CLIP: "13910218504",161,,,"TEST2"
 *  +CLIP: "13910218504",161
 *
 *----------------------------------------------------------*/
static int  AnaURC_CLIP( char *SrcStr,  PME_PHONE_ITEM pItem )
{
    int len;
    int type;
    char PhoneNum[ME_PHONENUM_LEN], PhoneStr[ME_PHONETXT_LEN];
    char PhoneStr1[ME_PHONETXT_LEN];
    char *pClIPStr, *pBegin, *pEnd;

    pClIPStr = SrcStr;

    if ( memcmp( pClIPStr, "+CLIP:", 6 ) != 0 )
        return -1;

    // Analyse number
    if( (pBegin = strstr( pClIPStr, "\"" )) == NULL )
        return -1;

    if( (pEnd = strstr( pBegin+1, "\"" )) == NULL )
        return -1;

    len = pEnd - pBegin;

    type = ME_GetIntSection( pClIPStr, "+CLIP:", 1 );

    if ( type == 145 )
    {
        *PhoneNum = '+';
        memcpy( PhoneNum+1, pBegin+1, len-1 );
        PhoneNum[len]=0;
    }
    else
    {
        memcpy( PhoneNum, pBegin+1, len-1 );
        PhoneNum[len-1]=0;
    }

    // Analyse alpha
    if( (pBegin = strstr( pEnd+1, "\"" )) == NULL )
    {
        PhoneStr[0]=0;
        goto monitor_clip_report;
    }

    if( (pEnd = strstr( pBegin+1, "\"" )) == NULL )
    {
        PhoneStr[0]=0;
        goto monitor_clip_report;
    }

    len = pEnd - pBegin;
    memcpy( PhoneStr, pBegin+1, len-1 );
    PhoneStr[len-1]=0;

    PhoneText_Decode( PhoneStr1,  ME_PHONETXT_LEN, PhoneStr );

monitor_clip_report:
    strcpy( pItem->PhoneNum, PhoneNum );
    strcpy( pItem->PhoneStr, PhoneStr1 );

    return sizeof(ME_PHONE_ITEM);
}


/*-------------------------------------------------------------
 *  +CCWA: "13910218504",161,1,"TEST2"
 *  +CCWA: "13910218504",161,1
 *  +CCWA:,,1
 *
 *----------------------------------------------------------*/
static int  AnaURC_CCWA( char * SrcStr, PME_PHONE_ITEM pItem )
{
    int len;
    int type;
    char PhoneNum[ME_PHONENUM_LEN], PhoneStr[ME_PHONETXT_LEN];
    char PhoneStr1[ME_PHONETXT_LEN];
    char *pCCWAStr, *pBegin, *pEnd;

    pCCWAStr = SrcStr;

    if ( memcmp( pCCWAStr, "+CCWA:", 6 ) != 0 )
        return -1;
    // Analyse number
    if( (pBegin = strstr( pCCWAStr, "\"" )) == NULL )
    {
        PhoneNum[0]=0;
        PhoneStr[0]=0;
        goto monitor_ccwa_report;
    }

    if( (pEnd = strstr( pBegin+1, "\"" )) == NULL )
        return -1;

    len = pEnd - pBegin;

    type = ME_GetIntSection( pCCWAStr, "+CCWA:", 1 );

    if ( type == 145 )
    {
        *PhoneNum = '+';
        memcpy( PhoneNum+1, pBegin+1, len-1 );
        PhoneNum[len]=0;
    }
    else
    {
        memcpy( PhoneNum, pBegin+1, len-1 );
        PhoneNum[len-1]=0;
    }


    // Analyse alpha
    if( (pBegin = strstr( pEnd+1, "\"" )) == NULL )
    {
        PhoneStr[0]=0;
        goto monitor_ccwa_report;
    }

    if( (pEnd = strstr( pBegin+1, "\"" )) == NULL )
    {
        PhoneStr[0]=0;
        goto monitor_ccwa_report;
    }

    len = pEnd - pBegin;
    memcpy( PhoneStr, pBegin+1, len-1 );
    PhoneStr[len-1]=0;

    PhoneText_Decode( PhoneStr1,  ME_PHONETXT_LEN, PhoneStr );

monitor_ccwa_report:
    strcpy( pItem->PhoneNum, PhoneNum );
    strcpy( pItem->PhoneStr, PhoneStr1 );

    return sizeof(ME_PHONE_ITEM);
}

static int  AnaURC_CSSI (char *SrcStr, PME_CSSI_ITEM pItem)
{
    char * pCSSIStr;

    pCSSIStr = SrcStr;

    pItem->Action = ME_GetIntSection (pCSSIStr, "+CSSI:", 1);
    pItem->Index = 0;

    return sizeof(ME_CSSI_ITEM);;
}

/*-------------------------------------------------------------
 *  +CSSU:<code2>[,<index>[,<number>,<type>]]
 *  +CSSU: 2,,"13910218504",161
 *  +CSSU: 3,,"13910218504",161
 *  +CSSU: 133
 *
 *----------------------------------------------------------*/
static int  AnaURC_CSSU( char *SrcStr, PME_CSSU_ITEM pItem )
{
    char * pCSSUStr;

    pCSSUStr = SrcStr;

    pItem->Action = ME_GetIntSection (pCSSUStr, "+CSSU:", 1);
    pItem->PhoneNum[0] = '\0';

    return sizeof(ME_CSSU_ITEM);
}

static int  AnaURC_SIND( char *SrcStr, PME_EXINFO_ITEM pInfo )
{
    char *SINDStr;

    SINDStr = SrcStr;

    if ( memcmp( SINDStr, "+CIEV:", 6 ) != 0 )
        return -1;

	if( NULL != strstr(SINDStr, "ciphcall") )
	{
		pInfo->Indicator = Ciphcall;
	}
	else if( NULL != strstr(SINDStr, "vmwait1") )
	{
		pInfo->Indicator = Vmwait1;
	}
	else if( NULL != strstr(SINDStr, "vmwait2") )	
	{
		pInfo->Indicator = Vmwait2;
	}
	else if( NULL != strstr(SINDStr, "rssi") )	
	{
		pInfo->Indicator = Rssi;
	}
    else if (NULL != strstr(SINDStr,"service"))
    {
        pInfo->Indicator = Service;
    }
    else if (NULL != strstr(SINDStr,"battchg"))
    {
        pInfo->Indicator = Battchg;
    }

	pInfo->Value = ME_GetIntSection (SINDStr, "+CIEV:",  2);
	
    return sizeof(ME_EXINFO_ITEM);
}
//end 0602

static int  AnaURC_SMGO( char *SrcStr, int* Type )
{
    char *SMGOStr;

    SMGOStr = SrcStr;

    if ( memcmp( SMGOStr, "^SMGO:", 6 ) != 0 )
        return -1;
    
    switch ( atoi( &SMGOStr[6] ) )
    {
    case 0:
        *Type = SMS_BUF_FREE;
    
    case 1:
        *Type = SMS_BUF_OVERFLOW;

    case 2:
        *Type = SMS_BUF_WAITING;

    default:

        return -1;
    }

    return sizeof(int);
}



/*-------------------------------------------------------------
 *  +CMTI:<mem>,<index>
 *  +CMTI: "MT",3
 *
 *----------------------------------------------------------*/
static int  AnaURC_CMTI( char *SrcStr, PME_CMTI_ITEM pItem )
{
    char *CMTIStr, *p;

    CMTIStr = SrcStr;

    if ( memcmp( CMTIStr, "+CMTI:", 6 ) != 0
        && memcmp( CMTIStr, "+CBMI:", 6 ) != 0
        && memcmp( CMTIStr, "+CDSI:", 6 ) != 0 )
        return -1;

    if( (p = strstr( CMTIStr, "\"SM\"" )) != NULL )
        pItem->DevType = SMS_MEM_SM;

    else if( (p = strstr( CMTIStr, "\"ME\"" )) != NULL )
        pItem->DevType = SMS_MEM_ME;

    else if( (p = strstr( CMTIStr, "\"MT\"" )) != NULL )
        pItem->DevType = SMS_MEM_MT;

    else
        return -1;

    p += strlen( "\"SM\"") +1;  // skip"<mem>,", point to <index>
    pItem->Index = atoi( p );

    return sizeof(ME_CMTI_ITEM);
}

/*-------------------------------------------------------------
 *  +CREG:<mode>,<stat>
 *  
 *
 *----------------------------------------------------------*/
static int  AnaURC_CREG( char *SrcStr, PME_CREG_URC pItem )
{
    char *CREGStr;

    CREGStr = SrcStr;

    if ( memcmp( CREGStr, "+CREG:", 6) != 0 )
        return -1;

    pItem->Stat = ME_GetIntSection (CREGStr, "+CREG:", 1);
    ME_GetStrSection (CREGStr, pItem->lac, sizeof(pItem->lac), "CREG:", 1);
    ME_GetStrSection (CREGStr, pItem->ci, sizeof(pItem->ci), "CREG:", 2);

    return sizeof(ME_CREG_URC);
}

/*-------------------------------------------------------------
 *  +CGREG:<stat>
 *
 *
 *----------------------------------------------------------*/
static int  AnaURC_CGREG( char *SrcStr, PME_CGREG_URC pItem )
{
    char *CREGStr;

    CREGStr = SrcStr;

    if ( memcmp( CREGStr, "+CGREG:", 7 ) != 0 )
        return -1;

    pItem->Stat = ME_GetIntSection( CREGStr, "+CGREG:", 1 );
    ME_GetStrSection( CREGStr, pItem->lac, 6, "CGREG:", 1 );
    ME_GetStrSection( CREGStr, pItem->ci, 6, "CGREG:", 2 );

    return sizeof(ME_CGREG_URC);
}

/*-------------------------------------------------------------
 *  +STIN: <CmdType>
 *  +STIN: 6
 *
 *----------------------------------------------------------*/
static int  AnaURC_STK( char *SrcStr, int* event )
{
    char *STKStr;

    STKStr = SrcStr;

    if ( memcmp( STKStr, "^SSTN:", 6 ) != 0 )
        return -1;

    STKStr += strlen( "^SSTN:" );
    *event = atoi( STKStr );

    return sizeof(int);
}

static int AnaURC_CMT( char* SrcStr, PSMS_INFO info )
{
    char* CMTStr = SrcStr, *begin, *end;
    if ( memcmp( CMTStr, "+CMT:", 5 ) != 0 )
        return -1;

    end = strchr( CMTStr, '\r' );

    if ( end == NULL )
        return -1;

    begin = end + 2;
    if ( begin == NULL )
        return -1;

    while ( *begin == '\r' || *begin == '\n' )
        begin++;

    if ( begin == NULL )
        return -1;

    end = strchr( begin, '\r' );

    if ( end == NULL )
        return -1;

    *end = '\0';

    ME_RecStr_Code(info, begin, SMS_UNREAD);

    if ( info->ConLen == -1 )
        return -1;

    return sizeof(SMS_INFO);
}

static int AnaURC_CCCM (char * src, unsigned long * ccm)
{
    char * ccmstr = src;
    char value[12];


    if ( memcmp (ccmstr, "+CCCM:", 6) != 0 )
        return -1;

    ME_GetStrSection (src, value, sizeof(value), "+CCCM:", 1);

    *ccm = AOCHexDecode (value);

    return sizeof(*ccm);
}

int ME_GetUSSDInfo (PUSSD_INFO info)
{
    int dcs, ret;
    char str[512], * src = gReqUssd->result;

    if ( src == NULL )
        return -1;

    if ( memcmp (src, "+CUSD:", 6) != 0 )
        return -1;

    info->Flag = ME_GetIntSection (src, "+CUSD:", 1);
    dcs = ME_GetIntSection (src, "+CUSD:", 2);
    if ( dcs < 0 ) 
        dcs = 15;
    ret = ME_GetStrSection (src, str, sizeof(str), "+CUSD:", 1);
    if ( ret < 0 || strlen (str) == 0 ) {
        info->DataLen = 0;
        return sizeof (*info);
    }

    USSD_Decode (info, str, (unsigned char)dcs);

    return sizeof (*info);
}

static int AnaURC_CBM( char* SrcStr, PCBS_INFO info )
{
    char* CBMStr = SrcStr, *begin, *end;
    if ( memcmp( CBMStr, "+CBM:", 5 ) != 0 )
        return -1;

    end = strchr( CBMStr, '\r' );

    if ( end == NULL )
        return -1;

    while (*end == '\r' || *end == '\n')
        end ++;

    begin = end;

    if ( *begin == 0 )
        return -1;

    end = strchr( begin, '\r' );

    if ( end == NULL )
        return -1;

    *end = '\0';

    if ( !CBS_Decode (info, begin, strlen(begin)) )
        return -1;

    return sizeof(CBS_INFO);
}

static int AnaURC_CDS( char* SrcStr, PSMS_INFO info )
{
    char* CDSStr = SrcStr, *begin, *end;
    if ( memcmp( CDSStr, "+CDS:", 5 ) != 0 )
        return -1;

    end = strchr( CDSStr, '\r' );

    if ( end == NULL )
        return -1;

    begin = end + 2;
    if ( begin == NULL )
        return -1;

    while ( *begin == '\r' || *begin == '\n' )
        begin++;

    if ( begin == NULL )
        return -1;

    end = strchr( begin, '\r' );

    if ( end == NULL )
        return -1;

    *end = '\0';

    ME_RecStr_Code(info, begin, SMS_UNREAD);

    if ( info->ConLen == -1 )
        return -1;

    return sizeof(SMS_INFO);
}

static int GetCLIPResult (void * result, char * str, int len)
{
    PCLIPINFO info = result;
    int m = ME_GetIntSection (str, "+CLIP:", 2);

    if ( m == 0 )
        info->Status = CLIPNONE;
    else if ( m == 1 )
        info->Status = CLIPALLOW;
    else
        info->Status = CLIPUNKNOWN;

    return sizeof (*info);
}

static int GetCLIRResult (void * result, char * str, int len)
{
    PCLIRINFO info = result;
    int m = ME_GetIntSection (str, "+CLIR:", 2);

    if ( m == 0 )
        info->Status = CLIRNONE;
    else if ( m == 1 )
        info->Status = CLIRPERMANENT;
    else if ( m == 3 )
        info->Status = CLIRTEMPRES;
    else if ( m == 4 )
        info->Status = CLIRTEMPALLOW;
    else
        info->Status = CLIRUNKNOWN;

    return sizeof (*info);
}

static int GetCOLPResult (void * result, char * str, int len)
{
    PCOLPINFO info = result;
    int m = ME_GetIntSection (str, "+COLP:", 2);

    if ( m < 0 )
        return -1;

    if ( m == 1 )
        info->Active = TRUE;
    else 
        info->Active = FALSE;

    return sizeof (*info);
}

static int GetCOLRResult (void * result, char * str, int len)
{
    PCOLRINFO info = result;
    int m = ME_GetIntSection (str, "+COLR:", 2);

    if ( m < 0 )
        return -1;

    if ( m == 1 )
        info->Active = TRUE;
    else
        info->Active = FALSE;

    return sizeof (*info);
}

static int GetCFResult (void * result, char * str, int len)
{
    PCFINFO info = result;
    char * begin, * end;
    int ret, i, count = len/sizeof(*info);

    begin = str;

    for ( i = 0; i < count; i++ ) {
        end = strchr( begin, '\r' );

        if ( end == NULL )
            return i * sizeof (*info);

        *end = '\0';

        ret = ME_GetIntSection (begin, "^SCCFC:", 1);
        if ( ret == 0 )
            info[i].Reason = CFU;
        else if ( ret == 1 )
            info[i].Reason = CFB;
        else if ( ret == 2 )
            info[i].Reason = CFNRy;
        else if ( ret == 3 )
            info[i].Reason = CFNRc;
        else
            info[i].Reason = ret;
        
        ret = ME_GetIntSection (begin, "^SCCFC:", 2);
        if ( ret == 1 )
            info[i].Active = TRUE;
        else
            info[i].Active = FALSE;

        ret = ME_GetIntSection (begin, "^SCCFC:", 3);
        if ( ret == 1 )
            info[i].BS = BSVOICE;
        else if ( ret == 2 )
            info[i].BS = BSDATA;
        else if ( ret == 4 )
            info[i].BS = BSFAX;
        else if ( ret == 8 )
            info[i].BS = BSSMS;
        else if ( ret == 16 )
            info[i].BS = BSDCS;
        else if ( ret == 32 )
            info[i].BS = BSDCA;
        else if ( ret == 64 )
            info[i].BS = BSDPAC;
        else if ( ret == 128 )
            info[i].BS = BSDPAD;
        else
            info[i].BS = ret;

        ret = ME_GetStrSection (begin, info[i].DN, ME_PHONENUM_LEN, "^SCCFC:", 1);
        if ( ret < 0 )
            info[i].DN[0] = '\0';

        info[i].Time = ME_GetIntSection (str, "^SCCFC:", 5);

        begin = end + 1;
   
        while ( *begin == '\r' || *begin == '\n' )
            begin ++;

        if ( strlen( begin ) == 0 )
            return (i+1) * sizeof (*info);
    }

    return len;
}

static int GetCWResult (void * result, char * str, int len)
{
    PCWINFO info = result;
    char * begin, * end;
    int ret, i, count = len/sizeof(*info);

    begin = str;

    for ( i = 0; i < count; i++ ) {
        end = strchr( begin, '\r' );

        if ( end == NULL )
            return i * sizeof (*info);

        *end = '\0';

        ret = ME_GetIntSection (begin, "+CCWA:", 1);
        if ( ret == 1 )
            info[i].Active = TRUE;
        else
            info[i].Active = FALSE;

        ret = ME_GetIntSection (begin, "+CCWA:", 2);
        if ( ret == 1 )
            info[i].BS = BSVOICE;
        else if ( ret == 2 )
            info[i].BS = BSDATA;
        else if ( ret == 4 )
            info[i].BS = BSFAX;
        else if ( ret == 8 )
            info[i].BS = BSSMS;
        else if ( ret == 16 )
            info[i].BS = BSDCS;
        else if ( ret == 32 )
            info[i].BS = BSDCA;
        else if ( ret == 64 )
            info[i].BS = BSDPAC;
        else if ( ret == 128 )
            info[i].BS = BSDPAD;
        else
            info[i].BS = ret;

        begin = end + 1;
   
        while ( *begin == '\r' || *begin == '\n' )
            begin ++;

        if ( strlen( begin ) == 0 )
            return (i+1) * sizeof (*info);
    }

    return len;
}

static int GetCBResult (void * result, char * str, int len)
{
    PCBINFO info = result;
    char * begin, * end, fac[12];
    int ret, i, count = len/sizeof(*info);

    begin = str;

    for ( i = 0; i < count; i++ ) {
        end = strchr( begin, '\r' );

        if ( end == NULL )
            return i * sizeof (*info);

        *end = '\0';

        ret = ME_GetStrSection (begin, fac, sizeof(fac), "^SCLCK:", 1);
        if ( ret < 0 )
            info[i].Fac = -1;
        else {
            if ( strcmp (fac, "AO") == 0 )
                info[i].Fac = BAOC;
            else if ( strcmp (fac, "OI") == 0 )
                info[i].Fac = BOIC;
            else if ( strcmp (fac, "OX") == 0 )
                info[i].Fac = BOIC_exHC;
            else if ( strcmp (fac, "AI") == 0 )
                info[i].Fac = BAIC;
            else if ( strcmp (fac, "IR") == 0 )
                info[i].Fac = BIC_Roam;
            else
                info[i].Fac = -1;
        }
        
        ret = ME_GetIntSection (begin, "^SCLCK:", 1);
        if ( ret == 1 )
            info[i].Active = TRUE;
        else
            info[i].Active = FALSE;
        
        ret = ME_GetIntSection (begin, "^SCLCK:", 2);
        if ( ret == 1 )
            info[i].BS = BSVOICE;
        else if ( ret == 2 )
            info[i].BS = BSDATA;
        else if ( ret == 4 )
            info[i].BS = BSFAX;
        else if ( ret == 8 )
            info[i].BS = BSSMS;
        else if ( ret == 16 )
            info[i].BS = BSDCS;
        else if ( ret == 32 )
            info[i].BS = BSDCA;
        else if ( ret == 64 )
            info[i].BS = BSDPAC;
        else if ( ret == 128 )
            info[i].BS = BSDPAD;
        else
            info[i].BS = ret;

        begin = end + 1;
   
        while ( *begin == '\r' || *begin == '\n' )
            begin ++;

        if ( strlen( begin ) == 0 )
            return (i+1) * sizeof (*info);
    }

    return len;
}

static int USSD_Decode (PUSSD_INFO info, char * src, unsigned char dcs)
{
    int codegroup, code;
    
    codegroup = dcs>>4;

    if ( 0x00 == codegroup ) {
        info->Code = CBS_ALPHA_GSM;
        if ( (dcs & 0x0f) == 0x0f )
            info->Lang = CBS_LANG_RESERVE;
        else {
            code = dcs & 0x0f;
            switch (code) {
            case 0:
                info->Lang = CBS_LANG_GERMAN;
            	break;
            case 1:
                info->Lang = CBS_LANG_ENGLISH;
            	break;
            case 2:
                info->Lang = CBS_LANG_ITALIAN;
            	break;
            case 3:
                info->Lang = CBS_LANG_FRENCH;
            	break;
            case 4:
                info->Lang = CBS_LANG_SPANISH;
            	break;
            case 5:
                info->Lang = CBS_LANG_DUTCH;
            	break;
            case 6:
                info->Lang = CBS_LANG_SWEDISH;
            	break;
            case 7:
                info->Lang = CBS_LANG_DANISH;
            	break;
            case 8:
                info->Lang = CBS_LANG_PORTUGUESE;
            	break;
            case 9:
                info->Lang = CBS_LANG_FINNISH;
            	break;
            case 10:
                info->Lang = CBS_LANG_NORWEGIAN;
            	break;
            case 11:
                info->Lang = CBS_LANG_GREEK;
            	break;
            case 12:
                info->Lang = CBS_LANG_TURKISH;
            	break;
            case 13:
                info->Lang = CBS_LANG_HANGARIAN;
            	break;
            case 14:
                info->Lang = CBS_LANG_POLISH;
            	break;
            default:
                info->Lang = CBS_LANG_RESERVE;
                break;
            }
        }
    }
    else if ( 0x01 == codegroup ) {
        if ( (dcs & 0x01) == 0 ) {
            info->Code = CBS_ALPHA_GSM;
            info->Lang = CBS_LANG_RESERVE;
        }
        else if ( (dcs & 0x01) == 1 ) {
            info->Code = CBS_ALPHA_UCS2;
            info->Lang = CBS_LANG_RESERVE;
        }
        else {
            info->Code = CBS_ALPHA_GSM;
            info->Lang = CBS_LANG_RESERVE;
        }
    }
    else if ( 0x02 == codegroup ) {
        if ( dcs == 0 )
            info->Lang = CBS_LANG_CZECH;
        else
            info->Lang = CBS_LANG_RESERVE;
        info->Code = CBS_ALPHA_GSM;
    }
    else if ( 0x03 == codegroup ) {
        info->Code = CBS_ALPHA_GSM;
        info->Lang = CBS_LANG_RESERVE;
    }
    else if ( codegroup >= 0x04 && codegroup <= 0x07 ) {
        if ( ((dcs & 0x0C) >> 2) == 2 )
            info->Code = CBS_ALPHA_UCS2;
        else if ( ((dcs & 0x0C) >> 2) == 1 )
            info->Code = CBS_ALPHA_DATA;
        else
            info->Code = CBS_ALPHA_GSM;

        info->Lang = CBS_LANG_RESERVE;
    }
    else if ( codegroup >= 0x08 && codegroup <= 0x0D) {
        info->Code = CBS_ALPHA_GSM;
        info->Lang = CBS_LANG_RESERVE;
    }
    else if ( codegroup == 0x0E ) {
        info->Code = CBS_ALPHA_GSM;
        info->Lang = CBS_LANG_RESERVE;
    }
    else {
        if ( dcs & 0x04 )
            info->Code = CBS_ALPHA_DATA;
        else
            info->Code = CBS_ALPHA_GSM;

        info->Lang = CBS_LANG_RESERVE;
    }

    if ( info->Code == CBS_ALPHA_UCS2 ) {
        UCS2Decode (info->Data, src, strlen(src));
        info->DataLen = strlen (src) / 2;
    }
    else if ( info->Code == CBS_ALPHA_DATA ) {
        String2Bytes (info->Data, src, strlen(src));
        info->DataLen = strlen (src) / 2;
    }
    else {
        strcpy (info->Data, src);
        info->DataLen = strlen (src);
    }

    return 0;
}

