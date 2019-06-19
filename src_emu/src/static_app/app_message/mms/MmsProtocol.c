/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS
 *
 * Purpose  : implements the interface with wap protocol
 *            
\**************************************************************************/
#include "MmsGlobal.h"
#include "smilparse.h"
#include "MmsProtocol.h"
#include "MmsInterface.h"
#include "log.h"

#define  MMS_GETDIAL_MSG   WM_USER + 801
#define  WM_CONFIRMREC	   WM_USER + 802

/*********************************************************************\
*      
*   常量定义
*      
**********************************************************************/
#define MAX_SENDHEAD_SIZE   6*1024
#define MAX_SENDBODY_SIZE   94*1024//48*1024

#define BODY_HEAD_SIZE      200
#define SMIL_BODY_SIZE      1024 * 5

// the length will be added after packed 
#define PACKLENGTH          10
#define REPORT_WAITFILE     "/mnt/flash/reportw.mms"
#define MMS_READREPLY       ML(STR_MMS_REPLYMMS)

#define HANGUP_DELAY_TIME   6000
#define HANGUP_TIMER_ID     2

// the multiple of utf8 packed length of chinese(probably)
#define  TEXT_CODEMULT      2
#define  BODY_ADDLEN        300
/*********************************************************************\
*      
*   struncture defination
*      
**********************************************************************/
static HWND         hwndProtocol;
static MMSSETTING   mSet;
static MMSMSGINFO   MsgInfo;

// define the status of dialing and connect
enum
{
    CS_IDLE,                    
    CS_DIALING,                 
    CS_CONNECTING,              
    CS_CONNECTED,               
    CS_DISCONNECTING,           
    CS_WAITINGHANGUP,           
    CS_HANGUPING,               
    CS_HANGUPEDDISCONNECTING    
};

static int nConnectState;

// the status of single msg in MMS queue
enum
{
    RSS_UNUSED,                 // no use
    RSS_WAITINGRECV,            // wait receive（wait for connected）
    RSS_WAITINGSEND,            // wait for send
    RSS_RECVING,                // receiving
    RSS_SENDING                 // sending
};

#define MAX_ACCEPT_BUF  512

// the structure of msg
typedef struct tagRSITEM
{
    int     nState;
    int     hMms;
    char    achFile[MAX_FILENAME];
    BOOL    bDelay;
    BOOL    bAutoRecv;           //auto or manul
    int     iRecvCount;          //1:the first receive

	int     size;
	SYSTEMTIME begintime;
} RSITEM,*PRSITEM;

#define MAX_RSITEMS         1//2

static RSITEM RSItemTable[MAX_RSITEMS];

typedef struct tagRSCHAIN
{
    //RSS_WAITINGRECV or RSS_WAITINGSEND
    int     nState;
    char    Filename[MAX_FILENAME];
    BOOL    bAutoRecv;
    struct tagRSCHAIN *pNextNode;
} RSCHAIN,*PRSCHAIN;

static RSCHAIN *pRSChainHead = NULL;

typedef struct tagFAILURERSITEM
{
     char achFile[MAX_FILENAME];
} FAILURERSITEM,*PFAILURERSITEM;

// the valid item in send and receive table
static int nRSItems;

// the failure table
#define MAX_FAILURE_RSITEMS  4
static FAILURERSITEM FailureRSItemTable[MAX_FAILURE_RSITEMS];

static WSPHead      wspheadbuf;

static int nFailureRSItems;

#define  MAX_QUENUM 10

typedef struct tagRECVQUE
{
    int front;
    int rear;
    int nCount;
    char recvName[MAX_QUENUM][MAX_FILENAME];
} RECVQUE,*PRECVQUE;

static RECVQUE RecvQue;
static PMMSPENDINGREC pPendingRecHead = NULL;

#define MAX_DIALTRYNUM  3
#define MAX_RECVCOUNT  2
static int dialnum = 1;
static int m_TypeNum = 0;

/*********************************************************************\
* 
*       function defination
* 
**********************************************************************/

LRESULT MMSProtocolWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static int  IsMmsDelay(void);
static BOOL saveBufToFile(char* pBuf);
static int  MMS_FillSendHead(WSPHead wspheadBuf, char* pHead, BOOL bReadReply);
static int  MMS_FillSendBody(TotalMMS tmms, char *pbody);
static int  MMS_Gb2Utf8(char *gbSrc, int txtlen);
static void MMS_FillSmil(MultiMMS *pMMS, MMSMultiBody* pSmil, int nNum);
static void MMS_FillImageAttatch(mmsMetaNode *pNode, MMSMultiBody *pImg, int nNum);
static void MMS_FillTextAttatch(mmsMetaNode *pNode, MMSMultiBody *pTxt, int nNum);
static void MMS_FillAudioAttatch(mmsMetaNode *pNode, MMSMultiBody *pAudio, int nNum );
static int  SaveMmsRecv(int headLen, char *pHead, int bodyLen, const char *pBody,
                   const char *pFileName);
static BOOL IsRSItemTableEmpty(void);
static BOOL IsRSItemTableFull(void);
static PRSITEM AllocRSItem(void);
static void FreeRSItem(PRSITEM pRSItem);
static PRSITEM GetRSItemByHandle(int hMms);
static void EmptyRSItemTable(void);

static BOOL GetNewRSItemFromFile(PRSITEM pRSItem);
static BOOL StartNewRSItemFromFile(PRSITEM pRSItem);
static BOOL StartNewItemsFromFile(void);
static BOOL IsFailureTableFull(void);
static void SetFailureTableFull(void);
static BOOL AddRSItemToFailureTable(PRSITEM pRSItem);
static void EmptyFailureTable(void);

static BOOL StartRecving(PRSITEM pRSItem);
static BOOL StartSending(PRSITEM pRSItem);

static void TryToDisconnect(void);
static BOOL StartDialing(void);
static BOOL Try_StartDialing(void);

static int  GetSetTime(int settime);
static int  GetSetTimeForSef(int settime);
static BOOL SaveHeadBuf(char* pFile, const char* pHeadBuf, int nHeadLen, 
                        BOOL bDelay);
static void MMS_OnDeliveryReport(char *pHeadBuf, int nHeadLen);    
static int GetMmsRecSet(void);
static BOOL IsAnonyAndRecv(const char *pHeadBuf);

extern void SetFileTransferSleepFlag( BOOL bSleepFlag );
extern void MMS_FreeFolderInfo();
extern void	MMSC_ModifyFolderUnread(int nFolder, int sigh, int nCount);
extern BOOL mms_delete_message(HWND hwndmu, DWORD msghandle);

static BOOL AddMmsToSent(const char *FileName, DWORD handle);
void ModifyMsgStatus(const char *pFileName, UINT nType, UINT nStatus);
static void initqueue(PRECVQUE q);
static BOOL queueempty(PRECVQUE q);
static BOOL enqueue(PRECVQUE q, char *fileName); 
static char* dequeue(PRECVQUE q);
static char* queuefront(PRECVQUE q);

int  PackMmsBody(const char *pFileName, TotalMMS tmms);
HWND MMSPro_GetHandle(void);
int  MMSPro_Send(const char* MmsSendName);
BOOL MMSPro_IsInRSTable(const char *szFilename);

//useragent/useprofile
BOOL SetUagentPorfile(void);
//case function
void MMS_SendedFunction(WPARAM wParam,LPARAM lParam);
BOOL MMS_AcceptFunction(HWND hWnd, WPARAM wParam);
void MMS_ReadFunction(WPARAM wParam ,LPARAM lParam);
void MMS_SendReadreply();
BOOL MMS_TimerFunction(HWND hWnd,WPARAM wParam);

//chain Function
static PRSCHAIN InsertRSChainNode(const char* achFile,int nState, BOOL bAutoRecv);
static BOOL DeleteRSChainNode(RSCHAIN *pRsChainNode);
static BOOL EmptyRSchain(void);
static BOOL GetRSChainNode(PRSITEM pRSItem);
static BOOL GetRSChainNodeToRSItem(void);
static BOOL GetReadreply(PRSITEM pRSItem);
static BOOL NotifyProgram(char *Filename);
static BOOL NotifyProgramForSuc(char * Filename);
static void NotifyUnibox(const char *filename, char* pHeadBuf, int buflen);
static BOOL MMS_CreateReport(char* pSendedFile, char*  msgID);
extern BOOL MMS_GetPendingDReport(char* pFindFile, char* msgID);
static BOOL MMS_UpdateDReport (PDREPORT pReport);
static BOOL MMS_GetFirstPendingReceive(PMMSPENDINGREC pGet, BOOL bRemove);
static BOOL MMS_AddPendingReceive(const char* mmsPendingFile);
static BOOL ReceivePendingMMS(HWND hWnd);

static void MMS_GprsCount(void);
static BOOL MMS_LogWrite(int direction, int size, SYSTEMTIME * pBegintime);

int  GetMTypeNum();
extern void MMSC_ClearAutoRecv(void);

#ifdef _DEBUG
static void CheckRSItemTable(void)
{
    int i;
    int nItems = 0;

    for (i = 0; i < MAX_RSITEMS; i++)
    {
        if (RSItemTable[i].nState != RSS_UNUSED)
            nItems++;
    }
}

#define CHECKRSITEMTABLE CheckRSItemTable
#else
#define CHECKRSITEMTABLE() ((void)0)
#endif

/*********************************************************************\
*       初始化窗口     
**********************************************************************/
BOOL MMSPro_Initial(void)
{
    WNDCLASS    wc;

    // create directory
   	mkdir(FLASHPATH,0);
	mkdir(MMSSET_FILEPATH,0);

#ifdef _SMARTPHONE
    if (GetSIMState() == -1)
    {
        MsgWin(STR_INIT_NOSIMCARD, STR_MMS_MMS,Notify_Alert,
			STR_MMS_OK,NULL,MMS_WAITTIME);
        return FALSE;
    }
#endif

    wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.lpszMenuName  = NULL;     
    wc.lpfnWndProc   = MMSProtocolWndProc;
    wc.lpszClassName = "MmsProClassName";
    wc.hbrBackground = NULL; 
            
    if  (!RegisterClass(&wc))
        return FALSE;

    hwndProtocol = CreateWindow("MmsProClassName","", NULL,
        0, 0, 0, 0, NULL, NULL, NULL, NULL);
    if (hwndProtocol == NULL)
        return FALSE;

    return TRUE;
}//
/*********************************************************************\
 * Function    init mSet
**********************************************************************/
void InitMmsSet(HWND hProtocol)
{    
    if (MMS_GetSetting(&mSet) != 1)
    {
        mSet.homerec = 1;       // confirm
        mSet.indexExpire = 4;   //max
        mSet.indexReply = 0;    //no use
        mSet.nAd = 1;           //no
        mSet.nAnonymity = 1;    //no
        mSet.nImageSize = 0;    //small
        mSet.nPriority = 0;     // no use
        mSet.nReportRec = 1;    // off
        mSet.nReportSend = 1;   // off
        mSet.visitrec = 1;      // confirm
        mSet.szIp[0] = 0;
        mSet.szUrl[0] = 0;

		mSet.port = 9201;
		mSet.DtType = 1;
		strcpy(mSet.ISPPhoneNum1, "cmwap");
		mSet.ISPID = 0;
		mSet.ISPName[0] = 0;
		mSet.ISPPassword[0] = 0;
		mSet.ISPUserName[0] = 0;
		// for debug
		//strcpy(mSet.szIp, MMS_GATEWAY_IP);
		//strcpy(mSet.szUrl, MMS_MMSC_URL);
		// end

        MMS_SaveSetting(&mSet);
    }
}

/*
void modifyurlip(PSTR url, PSTR ip)
{
    strcpy(mSet.szIp, ip);
    strcpy(mSet.szUrl, url);
}*/

/*********************************************************************\
* Function     MMS_RegisterMsg
* Purpose      register the msg of unibox
* Params       mInfo
* Return       void
**********************************************************************/
void MMSPro_RegisterMsg(HWND hWndCall, UINT msg, UINT nType)
{
    MsgInfo.hWnd = hWndCall;
    MsgInfo.msg = msg;
    MsgInfo.nType = nType;
}
/*********************************************************************\
* Function     MMS_UnRegisterMsg
* Purpose      unregister the msg of unibox
* Params       mInfo
* Return       void
**********************************************************************/
void MMSPro_UnRegisterMsg()
{
    MsgInfo.hWnd = NULL;
    MsgInfo.msg = 0;
    MsgInfo.nType = -1;
}
/*********************************************************************\
* Function     ModifySetting
* Purpose      modify the global variable :mSet
* Params       newset: the new setting
* Return           void
**********************************************************************/
void MMSPro_ModifySetting(MMSSETTING    *newset)
{
    memcpy(&mSet, newset, sizeof(MMSSETTING));
}

BOOL MMSPro_IsReply(void)
{
    return mSet.indexReply;
}
/*********************************************************************\
* Function     MMSPro_Send
* Purpose      prepare to send the msg
* Params       MmsSendName: the filename
* Return           void
* remark       draft call this function
**********************************************************************/
int MMSPro_PackSend(char* MmsSendName, TotalMMS tmms, BOOL bTran)
{
    DWORD msghandle;
    MU_MsgNode msgnode;
    
    // pack mms data
    if (PackMmsBody(MmsSendName, tmms) != RETURN_OK)
	{
		MMSMSGWIN("MMS:Packmmsbody failure\r\n");
        return 3;
	}

    msghandle = GetHandleByName(MmsSendName);
    if(!MMS_ModifyType(MmsSendName, TYPE_UNSEND, MU_OUTBOX))
	{
		MMSMSGWIN("MMS:mms_modifytype failure\r\n");
        return 2;
	}

	// msghandle should not be null!!
    if (msghandle != NULL)
        ModifyMsgNode(msghandle, MmsSendName, MMFT_UNSEND); 
    else
        msghandle = AllocMsgHandle(MmsSendName, MMFT_UNSEND, FALSE);

    if (MsgInfo.nType == MU_OUTBOX)
    {   
        FillMsgNode(&msgnode, MU_MSG_MMS, MMFT_UNSEND, tmms.mms.wspHead.date,
            msghandle, tmms.mms.wspHead.subject, tmms.mms.wspHead.to, NULL);
        
        SendMessage(MMSPro_GetHandle(), PWM_MSG_MU_NEWMTMSG, 
            MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), (LPARAM)&msgnode);
    }
    return( MMSPro_Send(MmsSendName) );
}
/*********************************************************************\
* Function     MMSPro_Send
* Return           void
**********************************************************************/
int MMSPro_Send(const char* MmsSendName)
{
    PRSITEM pRSItem;
    BOOL    bReadReply = FALSE, bReport = FALSE;
    int     msghandle;
    
    if (mSet.szIp[0] == '\0')
    {
//        MsgWin(NULL, NULL, 0, STR_MMS_SETIP, STR_MMS_MMS, Notify_Info,
//			STR_MMS_OK, NULL,MMS_WAITTIME);
		MsgWin(NULL, NULL, 0, "Invalid MMS settings", STR_MMS_MMS,
					Notify_Failure, STR_MMS_OK, NULL, MMS_WAITTIME);
		MMSMSGWIN("MMS:szip == NULL\r\n");
        return -1;
    }

    if (mSet.szUrl[0] == '\0')
    {
//        MsgWin(NULL, NULL, 0, STR_MMS_SETURL, STR_MMS_MMS, Notify_Info,
//			STR_MMS_OK, NULL,MMS_WAITTIME);
		MsgWin(NULL, NULL, 0, "Invalid MMS settings", STR_MMS_MMS,
					Notify_Failure, STR_MMS_OK, NULL, MMS_WAITTIME);
		MMSMSGWIN("MMS:szurl == NULL\r\n");
        return -1;
    }

    if (IsFailureTableFull())
	{
		MMSMSGWIN("MMS:failuretable full\r\n");
        return FALSE;
	}
    
    if (MMSPro_IsInRSTable(MmsSendName))
	{
		MMSMSGWIN("MMS:inrstable\r\n");
        return FALSE;
	}

    pRSItem = AllocRSItem();
    if (!pRSItem)
    {
        InsertRSChainNode(MmsSendName,RSS_WAITINGSEND,FALSE);
        msghandle = GetHandleByName(MmsSendName);
//        if(MsgInfo.nType == MU_OUTBOX)
        {
            ModifyMsgStatus(MmsSendName, MU_SEND_WAITING, MMFT_WAITINGSEND);
        }
        MsgWin(NULL, NULL, 0, STR_MMS_SENDING, STR_MMS_MMS, Notify_Info, NULL,
            NULL,MMS_WAITTIME);
        return TRUE;
    }

    MsgWin(NULL, NULL, 0, STR_MMS_SENDING, STR_MMS_MMS, Notify_Info,
		NULL, NULL,MMS_WAITTIME);

    strcpy(pRSItem->achFile, MmsSendName);

    if (MmsSendName[MMS_TYPE_POS] == TYPE_READREPLY)
        bReadReply = TRUE;

    bReport = !mSet.nReportRec;
    
    if (bReadReply)
        bReport = 0;

    if (nConnectState == CS_CONNECTED)
    {
        if (!StartSending(pRSItem))
        {
            msghandle = GetHandleByName(MmsSendName);
            ModifyMsgNode(msghandle,MmsSendName,MMFT_UNSEND);
            FreeRSItem(pRSItem);
			MMSMSGWIN("MMS:startsending failure\r\n");
            return FALSE;
        }

        ModifyMsgStatus(MmsSendName, MU_SEND_START, MMFT_SENDING);
    }
    else
    {
        if (nConnectState == CS_IDLE)
        {
            if (!StartDialing())
            {
                msghandle = GetHandleByName(MmsSendName);
                ModifyMsgNode(msghandle,MmsSendName,MMFT_UNSEND);
                FreeRSItem(pRSItem);
				MMSMSGWIN("MMS:startdisling failure\r\n");
				MsgWin(NULL, NULL, 0, STR_MMS_SENDFAI, STR_MMS_MMS, Notify_Failure,
					STR_MMS_OK, NULL,MMS_WAITTIME);
                return FALSE;
            }

            nConnectState = CS_DIALING;

#ifndef _SMARTPHONE
            SetFileTransferSleepFlag(FALSE);
#endif
        }
        else if (nConnectState == CS_WAITINGHANGUP)
        {
            nConnectState = CS_CONNECTING;
            KillTimer(hwndProtocol, HANGUP_TIMER_ID);
            
            MMS_Connect(mSet.szIp, MMS_PORT);
        }

        pRSItem->nState = RSS_WAITINGSEND;

        ModifyMsgStatus(MmsSendName, MU_SEND_START, MMFT_SENDING);
    }
    return TRUE;
}
// empty the queue
static void initqueue(PRECVQUE q)
{
    q->rear = 0;
    q->front = 0;
    q->nCount = 0;
}
static BOOL queueempty(PRECVQUE q) 
{
    return (q->nCount == 0);
}

static int queuefull(PRECVQUE q)
{
    return (q->nCount == MAX_QUENUM);
}

static BOOL enqueue(PRECVQUE q, char *fileName)
{
    if(queuefull(q))
    {
        return FALSE;
    }
    q->nCount++;
    strcpy(q->recvName[q->rear] , fileName);
    q->rear=(q->rear + 1)%MAX_QUENUM;
    return TRUE;
}

static char* dequeue(PRECVQUE q)
{
    char *temp;

    if(queueempty(q))
        return NULL;

    temp = q->recvName[q->front];
    q->nCount--;
    q->front=(q->front + 1)%MAX_QUENUM;
    return temp;
}


static char* queuefront(PRECVQUE q)
{
    if(queueempty(q))
    {
        return NULL;
    }

    return q->recvName[q->front];
}

void GetQueFront(char *filename)
{
    strcpy(filename, dequeue(&RecvQue));
}

static void DelQueFront()
{
    dequeue(&RecvQue);
}

HWND MMSPro_GetHandle(void)
{
    return MsgInfo.hWnd;
}
/*********************************************************************\
* Function     MMS_OnRead
* Return           void
**********************************************************************/
static BOOL MMS_OnRead(PRSITEM pRSItem)
{
    int headLen = 0, bodyLen = 0;
    char *pHead, *pBody;
    int nret, ret;
    BOOL bIcon;
#if 0
    char tipMsg[SIZE_2];
#endif
    int nType;
    int msghandle;
    
    MMS_GetData(pRSItem->hMms, NULL, &headLen, NULL, &bodyLen);
    pHead = (char*)MMS_malloc(headLen + 1);
    pBody = (char*)MMS_malloc(bodyLen);
    //malloc failure
    if (NULL == pBody || NULL == pHead)
    {
        if (pBody)
            MMS_free(pBody);
        if (pHead)
            MMS_free(pHead);
        return FALSE;
    }

    nret = MMS_GetData(pRSItem->hMms, pHead, &headLen, pBody, &bodyLen);
   
    msghandle = GetHandleByName(pRSItem->achFile);
	if (IsAnonyAndRecv(pHead))
	{
		MMS_free(pBody);
		MMS_free(pHead);
		mms_delete_message(MsgInfo.hWnd, msghandle);
		return FALSE;
	}

    nType = pRSItem->achFile[MMS_TYPE_POS];       

	pRSItem->size = headLen + bodyLen;

    SaveMmsRecv(headLen, pHead, bodyLen, pBody, pRSItem->achFile);

    ret = MMS_ModifyType(pRSItem->achFile, TYPE_UNREAD, -1);
    
    if (ret && msghandle != NULL)
    {
        ModifyMsgNode(msghandle, pRSItem->achFile, MMFT_UNREAD);
    }
    MsgWin(NULL, NULL, 0, STR_MMS_RECVOK, STR_MMS_MMS, Notify_Success, 
		STR_MMS_OK, NULL, MMS_WAITTIME);
        
    MMS_free(pBody);    
    MMS_free(pHead);
      
    bIcon = TRUE;

    if(ret)
    {
        if (MsgInfo.nType != MU_INBOX)
        {
            // show the number of unread mms
            MMSC_ModifyUnconfirmed(COUNT_ADD, 0);
        }
        //if manual，because Unreceive number has been added，now need to decrease；
        //if auto,no change
        if(!pRSItem->bAutoRecv)//manul
        {
            ASSERT(msghandle != -1);

            MMSC_ModifyUnreceive(COUNT_DEC, 0);
            
            enqueue(&RecvQue, pRSItem->achFile);
            
            mms_newmsg_continue();
        }
        
        MMSC_ModifyUnread(COUNT_ADD, 0);
		MMSC_ModifyFolderUnread(MU_INBOX, COUNT_ADD, 0);
        return TRUE;
    }

    return FALSE;
}
/*********************************************************************\
* Function     IsValidNotification
* Purpose      
* Params       pHeadbuf, nheadlen
* Return       True or false       
* Remarks      Mandatory field
               1. X-Mms-Message-Type
               2. X-Mms-Transaction-ID
               3. X-Mms-MMS-Version
               4. X-Mms-Message-Class
               5. X-Mms-Message-Size
               6. X-Mms-Expiry
               7. X-Mms-Content-Location
**********************************************************************/
static BOOL IsValidNotification(const char* pHeadBuf, int nHeadLen)
{
    if (strstr(pHeadBuf, "X-Mms-Message-Type") == NULL)
        return FALSE;
    if (strstr(pHeadBuf, "X-Mms-Transaction-ID") == NULL)
        return FALSE;
    if (strstr(pHeadBuf, "X-Mms-MMS-Version") == NULL)
        return FALSE;
    if (strstr(pHeadBuf, "X-Mms-Message-Class") == NULL)
        return FALSE;
    if (strstr(pHeadBuf, "X-Mms-Message-Size") == NULL)
        return FALSE;
    if (strstr(pHeadBuf, "X-Mms-Expiry") == NULL)
        return FALSE;
    if (strstr(pHeadBuf, "X-Mms-Content-Location") == NULL)
        return FALSE;

    return TRUE;
}
/*********************************************************************\
* Function     save unreceive and delayed data 
* Params       pheadbuf:    data
                nheadlen:   datalen
                sfix    :  the type of file
* Return        true: success; false: failure   
**********************************************************************/
static BOOL SaveHeadBuf(char* pFile, const char* pHeadBuf, int nHeadLen, 
                        BOOL bDelay)
{
    char    Filename[MAX_FILENAME];
    int     hFile;
    char    *pExpiry, *pFrom = NULL;
    int     len, lensize ;
    SYSTEMTIME  sysTime, expireTime;
    FILETIME    sysFileTime, expireFileTime;
//    char    From[PB_FLASH_ENTRY_EMAIL_MAXLEN + 1];
    BOOL    bEmpired = FALSE, bOverSize = FALSE;
    char    *pSizeField = NULL;

    if (FALSE == IsValidNotification(pHeadBuf, nHeadLen))
        return FALSE;

    pExpiry = FindContentByKey(pHeadBuf, "X-Mms-Expiry", &len);
    ASSERT(pExpiry != NULL);    
    MMS_StrToSysTime(pExpiry, &expireTime);
    // the expiretime should past the current time, 
	//if the time is invalid ,then default is two days
    GetLocalTime(&sysTime);
    MMS_STtoFT(&expireTime, &expireFileTime);
    MMS_STtoFT(&sysTime, &sysFileTime);
    if (expireFileTime.dwLowDateTime <= sysFileTime.dwLowDateTime)
    {
        sysFileTime.dwLowDateTime += 2*48*3600;
        MMS_FTtoST(&sysFileTime, &expireTime);
    }
    
    pSizeField = FindContentByKey(pHeadBuf, "X-Mms-Message-Size:",
        &lensize);
    if (pSizeField)
    {
        char tempSize[SIZE_2];
        
        tempSize[0] = 0;
        strncpy(tempSize, pSizeField, lensize);
        tempSize[lensize] = '\0';
        if(atoi(tempSize) > MAX_MMS_SIZE)
            bOverSize = TRUE;
    }

    ASSERT(pHeadBuf[0] != '\0');

    if (bDelay)
        MMS_CreateFileName(Filename, TYPE_DELAYRECV, MU_INBOX);
    else 
    {
        //if(mSet.indexTime == MMSSET_AUTO)
            MMS_CreateFileName(Filename, TYPE_FIRSTAOTORECV, MU_INBOX);
			AllocMsgHandle(Filename, MMFT_FIRSTAOTORECV, FALSE);
        //else
        //    MMS_CreateFileName(Filename, TYPE_UNRECV, MU_INBOX);
    }
    ASSERT(Filename[0] != 0);

	chdir(FLASHPATH);

    hFile = MMS_CreateFile(Filename, O_RDWR|O_CREAT);
    if (hFile == -1)
        return FALSE;
    //write the valid time in the last of the file
    write(hFile, (PVOID)pHeadBuf, nHeadLen);    
    write(hFile, &expireTime, sizeof(SYSTEMTIME));
    MMS_CloseFile(hFile);

    strcpy(pFile, Filename);
        
    //when receive notification，if manual,then notify programe ring ,Unreceive++
    //if(mSet.indexTime == MMSSET_MANUAL)                     
    /*if (0)
    {
        MMSC_ModifyUnreceive(COUNT_ADD, 0);

        pFrom = FindContentByKey(pHeadBuf, "From", &lenfrom);
        
        if(pFrom != NULL)
        {
            char *pdest;
            pdest = strstr(pFrom, "+86");
            if (pdest != NULL)
            {
                pFrom = pdest + 3;
                lenfrom -= 3;
            }
            strncpy(From, pFrom, lenfrom);
            From[lenfrom] = 0;
        }
        mu_newmsg_ring(From);
    }*/

    if(bEmpired || bOverSize)
    {
        //if(MsgInfo.nType == MU_INBOX)
            NotifyUnibox((const char*)pFile, (char*)pHeadBuf, nHeadLen);
        if(bEmpired)
            MsgWin(NULL, NULL, 0, STR_MMS_EXPIRED, STR_MMS_MMS, Notify_Alert,
			STR_MMS_OK, NULL,MMS_WAITTIME);
        else 
            MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, STR_MMS_MMS, Notify_Alert,
			STR_MMS_OK, NULL,MMS_WAITTIME);
        return FALSE;    
    }
    return TRUE;
}

/*when receive notification，if manul, then notify unibox add an unreceive msg
*/
static void NotifyUnibox(const char *filename, char* pHeadBuf, int buflen)
{
    MU_MsgNode      msgnode;
    ACCEPTINFO      AcceptInfo;
    DWORD           msghandle;

    memset(&AcceptInfo, 0, sizeof(ACCEPTINFO));
    GetLocalTime(&AcceptInfo.acceptTime);
    MMS_ParseAccept(pHeadBuf, buflen, &AcceptInfo);
    AcceptInfo.infoType = INFO_ACCEPT;
    
    msghandle = GetHandleByName(filename);
	if (msghandle == NULL)
		msghandle = AllocMsgHandle(filename, MMFT_UNRECV, FALSE);
    
    if (msghandle == NULL)
    {
        MsgWin(NULL, NULL, 0, STR_MMS_OVERNUM, STR_MMS_MMS,Notify_Alert,
			STR_MMS_OK, NULL,MMS_WAITTIME);
        return;
    }

    if(MsgInfo.nType == MU_INBOX)
	{
		FillMsgNode(&msgnode, MU_MSG_MMS, MMFT_UNRECV, AcceptInfo.acceptTime, msghandle,
			AcceptInfo.Subject, AcceptInfo.From, NULL);
		
		SendMessage(MsgInfo.hWnd, PWM_MSG_MU_NEWMTMSG, 
			MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), (LPARAM)&msgnode);
	}
	// add for progman will get the msg number for display in idle
	MU_NewMsgArrival(MU_MMS_NOTIFY);
}


static BOOL NotifyProgram(char *Filename)
{
    static char achHeadBuf[MAX_ACCEPT_BUF + 1];
    int   hFile;       
    char* pHeadBuf,*pFrom;
    int nHeadLen, lenfrom;
    char From[PB_FLASH_ENTRY_EMAIL_MAXLEN + 1];
    
    
    pHeadBuf = achHeadBuf;
    nHeadLen = MAX_ACCEPT_BUF;
    
	chdir(MMS_FILEPATH);
    hFile = MMS_CreateFile(Filename, O_RDONLY);
    if (hFile == -1)
        return FALSE;
    read(hFile, (PVOID)pHeadBuf, nHeadLen);
    MMS_CloseFile(hFile);   
    
    pFrom = FindContentByKey(pHeadBuf, "From", &lenfrom);
    if(pFrom != NULL)
    {
        char *pdest;
        pdest = strstr(pFrom, "+86");
        if (pdest != NULL)
        {
            pFrom = pdest + 3;
            lenfrom -= 3;
        }
        strncpy(From, pFrom, lenfrom);
        From[lenfrom] = 0;
    }               
    mu_newmsg_ring(From);

    //if(MsgInfo.nType == MU_INBOX)
		NotifyUnibox(Filename,pHeadBuf,nHeadLen);

    return TRUE;
}
// receive successfully in auto method need to notify Programe
static BOOL NotifyProgramForSuc(char * Filename)
{
    int		        hFile;
    WSPHead         wsphead;
    DWORD           msghandle;
    MU_MsgNode      msgnode;

	mu_newmsg_ring(wsphead.from);

    chdir(MMS_FILEPATH);
    hFile = MMS_CreateFile(Filename, O_RDONLY);
	printf("NotifyProgramForSuc hFile = %d\r\n", hFile);

    if (hFile == -1)
        return FALSE;
    lseek(hFile, MMS_HEADER_SIZE, SEEK_SET);
    read(hFile, &wsphead, sizeof(WSPHead));    
    MMS_CloseFile(hFile);
	
    msghandle = GetHandleByName(Filename);
	if (msghandle == NULL)
		msghandle = AllocMsgHandle(Filename, MMFT_UNREAD, FALSE);	
	if (msghandle == NULL)
	{
		MsgWin(NULL, NULL, 0, STR_MMS_OVERNUM, STR_MMS_MMS, Notify_Alert,
			STR_MMS_OK, NULL,MMS_WAITTIME);
		return FALSE;
	}
	if(MsgInfo.nType == MU_INBOX)
	{
		FillMsgNode(&msgnode, MU_MSG_MMS, MMFT_UNREAD, wsphead.date, msghandle,
			wsphead.subject, wsphead.from, NULL);
		
		SendMessage(MsgInfo.hWnd, PWM_MSG_MU_NEWMTMSG, 
			MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), (LPARAM)&msgnode);
	}
	// add for progman will get the msg number for display in idle
	MU_NewMsgArrival(MU_MMS_NOTIFY);
    return TRUE;   
}
/*********************************************************************\
* Function     addmmstosent
* Purpose      
* Params       const char fileName
* Return       BOOL
**********************************************************************/
static BOOL AddMmsToSent(const char *FileName, DWORD handle)
{
    int    hFile;
    MU_MsgNode  msgnode;
    WSPHead  wsphead;
    
    if (MsgInfo.nType == MU_SENT)
    {
        chdir(FLASHPATH);
        hFile = MMS_CreateFile(FileName, O_RDONLY);
        if (hFile == -1)
            return FALSE;
        lseek(hFile, MMS_HEADER_SIZE, SEEK_SET);
        read(hFile, &wsphead, sizeof(WSPHead));
        MMS_CloseFile(hFile);

        FillMsgNode(&msgnode, MU_MSG_MMS, MMFT_SENDED, wsphead.date,
            handle, wsphead.subject, wsphead.to, NULL);
        
        SendMessage(MsgInfo.hWnd, PWM_MSG_MU_NEWMTMSG, 
            MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), (LPARAM)&msgnode);
    }
    return TRUE;
}
/*********************************************************************\
* Function     ModifyMsgStatus
* Purpose      modify mms message status
* Params       filename
               nType
               nStatus: mmft_
* Return        void
**********************************************************************/
void ModifyMsgStatus(const char *pFileName, UINT nType, UINT nStatus)
{
    MU_Daemon_Notify  muDaemon;
    
    if (nType == MU_DISCONNETED || nType == MU_CONN_START)
        muDaemon.handle = 0;
    else
    {
        muDaemon.handle = GetHandleByName(pFileName);
        if (muDaemon.handle == NULL)
            muDaemon.handle = AllocMsgHandle(pFileName, nStatus, FALSE);
    }

    muDaemon.type = nType;
    SendMessage(MsgInfo.hWnd, MsgInfo.msg, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), 
        (LPARAM)&muDaemon);

    if (pFileName != NULL)
        ModifyMsgNode(muDaemon.handle, pFileName, nStatus);    
}
/*********************************************************************\
* Function     ModifyMsgStatusByHandle
* Purpose      modify mms message status
* Params       filename
               nType
               nStatus: mmft_
* Return        void
**********************************************************************/
void ModifyMsgStatusByHandle(DWORD handle, const char *pFileName, UINT nType, UINT nStatus)
{
    MU_Daemon_Notify  muDaemon;
    
	if (handle == 0)
		return;

    muDaemon.handle = handle;
    muDaemon.type = nType;

    SendMessage(MsgInfo.hWnd, MsgInfo.msg, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), 
        (LPARAM)&muDaemon);

    ModifyMsgNode(handle, pFileName, nStatus);    
}

/*********************************************************************\
* Function     judge adverts
* Params       pheadbuf: accept data
* Return       if this mms is advert and the seeting is reject，
			   return true，else return false
**********************************************************************/
static BOOL IsAdAndRecv(const char *pHeadBuf)
{
    int  len;
    char *pClass;

    pClass = FindContentByKey(pHeadBuf, "X-Mms-Message-Class", &len);
    
    if (pClass == NULL)
        return FALSE;

    if (strnicmp(pClass, MSG_CLASS_AD, 13) == 0)
    {
        if (mSet.nAd == 1)
            return TRUE;
    }

    return FALSE;
}
/*********************************************************************\
* Function     judge wether this mms is anonymity
* Params       pheadbuf: accept data
* Return       if this mms is anonymity and the setting is reject，
               then return true，else return false
**********************************************************************/
static BOOL IsAnonyAndRecv(const char *pHeadBuf)
{
    int  len;
    char *pFrom;

    pFrom = FindContentByKey(pHeadBuf, "From", &len);
    
    if (pFrom == NULL && mSet.nAnonymity)
        return TRUE;

    return FALSE;
}
/*********************************************************************\
* Function     IsInFilter
* Purpose      
* Params       pHeadBuf
* Return       yes：true， no：false
**********************************************************************/
static BOOL IsInFilter(const char* pHeadBuf)
{
    int  hFilter;
    char FilterString[FILTER_NUM][MAX_MAIL_LEN];
    int i, len;
    char *szFrom;
    char *pdest;
    
    szFrom = FindContentByKey(pHeadBuf, "From", &len);
    if (szFrom == NULL)
        return FALSE;

    hFilter = MMS_CreateFile(MMSFILTER_FILE, O_RDONLY);
    if (-1 == hFilter)
        return FALSE;

    pdest = strstr(szFrom, "+86");
    if (pdest != NULL)
    {
        szFrom = pdest + 3;
        len -= 3;
    }

    if (0 != (int)read(hFilter, FilterString, MAX_MAIL_LEN * FILTER_NUM))
    {
        for (i = 0; i < FILTER_NUM; i++)
        {
            if ((FilterString[i][0] != '\0') &&
                (strnicmp(FilterString[i], szFrom, len) == 0))
            {
                MMS_CloseFile(hFilter);
                return TRUE;
            }
        }
    }

    MMS_CloseFile(hFilter);

    return FALSE;
}

/*********************************************************************\
* Function     GetSetTime
* Purpose      coount the timer according the setting
* Params       settime
* Return       the timer length
**********************************************************************/
static int GetSetTime(int settime)
{
    switch(settime) 
    {
    case 0:     //  15minutes
        return 15*60*1000;
    case 1:     //one hour
        return 60*60*1000;
    case 2:     //two hour
        return 120*60*1000;
    case 3:     //30 minutes
        return 30*60*1000;    
    default:
        break;
    }

    ASSERT(0);
    return -1;
}
// the return value is multiple of 10s
static int GetSetTimeForSef(int settime)
{
    switch(settime) 
    {
    case 0:     //  15minutes
        return 15*6;
    case 1:     //one hour
        return 60*6;
    case 2:     //two hour
        return 120*6;
    case 3:     //30 minutes
        return 30*6;    
    case 4:
        return 0;
    default:
        break;
    }

    ASSERT(0);
    return -1;
}
/*********************************************************************\
* Function     MMSProtocolWndProc
**********************************************************************/


static LRESULT MMSProtocolWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                  LPARAM lParam)
{
    LRESULT lResult;
    int     i;
	int     len;

    switch(wMsgCmd)
    {
    case WM_CREATE :
    {
        // int WAP stack，init MMS , register the hWnd and msg
        MMS_Startup(hWnd, WM_MMS_NOTIFY);

		len = strlen("User-Agent:Nokia7650/1.0 SymbianOS/6.1 Series60/0.9 Profile/MIDP-1.0\r\n");

		MMS_IOCtrl(MMS_WSPHEAD, (int *)"User-Agent:Nokia7650/1.0 SymbianOS/6.1 Series60/0.9 Profile/MIDP-1.0\r\n", 
			len);

        InitMmsSet(hWnd);
        
        initqueue(&RecvQue);

        MsgInfo.nType = -1;
        
        MMSC_InitCount();	
		
//		MMSC_TransAutoRecv();

		DlmNotify(PS_NEWMSG, 1);
        break;
    }
        
    case MSG_DIAL_RETURN :
    {
        int nRet;

        switch (lParam)
        {
        case DIALER_RUN_NETCONNECT :
            ASSERT(nConnectState == CS_DIALING);

            if (nConnectState != CS_DIALING)
                break;

            MMSMSGWIN("dialing success", "", MMS_WAITTIME);

            if (mSet.szIp[0] == '\0')
            {
				PRSITEM pRSItem;

                MsgWin(NULL, NULL, 0, STR_MMS_SETIP, STR_MMS_MMS, Notify_Info,
					STR_MMS_OK, NULL,MMS_WAITTIME);
				
				pRSItem = &RSItemTable[0];

				if(pRSItem == NULL)
					return 0;

				if (pRSItem->bAutoRecv)
				{
					MMSC_ModifyTotal(COUNT_ADD, 0);
					MMSC_ModifyMsgCount(MU_INBOX, COUNT_ADD, 0);
				}

				ModifyMsgStatus(pRSItem->achFile, MU_RECV_FAILURE, MMFT_UNRECV);
				FreeRSItem(pRSItem);
				nConnectState = CS_WAITINGHANGUP;
				SetTimer(hWnd, HANGUP_TIMER_ID, HANGUP_DELAY_TIME, NULL);
                return 0;
            }

            nConnectState = CS_CONNECTING;
            nRet = MMS_Connect(mSet.szIp, MMS_PORT);        //"10.0.0.172"
            if (nRet == MMS_SUCCESS)
                SendMessage(hWnd, WM_MMS_NOTIFY, 0, 
                    MAKELPARAM(MMS_CONNECT, MMS_SUCCESS));
            else if (nRet == MMS_SYS_ERROR)
                SendMessage(hWnd, WM_MMS_NOTIFY, 0, 
                    MAKELPARAM(MMS_DISCONNECT, MMS_SUCCESS));
            break;
            
        case DIALER_RUN_NETBROKEN :
        case DIALER_RUN_NET_ABNORMAL :

            ASSERT(nConnectState != CS_IDLE && 
                nConnectState != CS_HANGUPEDDISCONNECTING);

			// all disconnected status should send the msg for receive failue
            if (nConnectState == CS_DIALING)
            {                
                if(dialnum < MAX_DIALTRYNUM )
                {                   
                    dialnum++;
                    nConnectState = CS_IDLE;
                    if (Try_StartDialing())
                    {
                        nConnectState = CS_DIALING;
                        break;
                    }
                }                

                nConnectState = CS_IDLE;
                EmptyRSItemTable(); 
                EmptyRSchain();
                
#ifndef _SMARTPHONE
                SetFileTransferSleepFlag(TRUE);
#endif
                MMSMSGWIN("network disconnected", STR_MMS_MMS, MMS_WAITTIME);
                MMS_WaitWindowState(hwndProtocol, FALSE);
                
                ModifyMsgStatus(NULL, MU_DISCONNETED, 0);
#if LILY_DEBUG
                checknum();
#endif
            }
            else if (nConnectState == CS_CONNECTING || 
                nConnectState == CS_CONNECTED)
            {
                SetFailureTableFull();

                nConnectState = CS_HANGUPEDDISCONNECTING;
                nRet = MMS_Disconnect();
                ASSERT(nRet != MMS_FAILURE);
                if(nRet == MMS_FAILURE)
                    MMS_WaitWindowState(hwndProtocol, FALSE);
            }
            else if (nConnectState == CS_DISCONNECTING)
            {
                SetFailureTableFull();

                nConnectState = CS_HANGUPEDDISCONNECTING;
            }
            else if (nConnectState == CS_HANGUPING)   // 
            {
                nConnectState = CS_IDLE;

                if ((!IsRSItemTableEmpty() || pRSChainHead) 
                    && !IsFailureTableFull())
                {
                    if (StartDialing())
                    {
                        nConnectState = CS_DIALING;                 
                        EmptyFailureTable();
                        break;
                    }
                }
                
                EmptyFailureTable();

                EmptyRSItemTable();
                EmptyRSchain();
#ifndef _SMARTPHONE
               SetFileTransferSleepFlag(TRUE);
#endif
               MMSMSGWIN("network disconnected", STR_MMS_MMS, MMS_WAITTIME);
               MMS_WaitWindowState(hwndProtocol, FALSE);
               
               ModifyMsgStatus(NULL, MU_DISCONNETED, 0);
#if LILY_DEBUG
                checknum();
#endif
            }
            else if (nConnectState == CS_WAITINGHANGUP)
            {
                KillTimer(hWnd, HANGUP_TIMER_ID);

                nConnectState = CS_IDLE;

                EmptyFailureTable();

                EmptyRSItemTable();
                EmptyRSchain();

#ifndef _SMARTPHONE
               SetFileTransferSleepFlag(TRUE);
#endif
               MMSMSGWIN("network disconnected", STR_MMS_MMS, MMS_WAITTIME);
               MMS_WaitWindowState(hwndProtocol, FALSE);
               
               ModifyMsgStatus(NULL, MU_DISCONNETED, 0);
#if LILY_DEBUG
               checknum();
#endif
            }
            else   
            {
                ASSERT(0);
                MMS_WaitWindowState(hwndProtocol, FALSE);
            }
            break;
        
        default :
            break;
        }

        break;
    }
    case MMS_GETDIAL_MSG:
        strcpy(mSet.szIp, (char *)wParam);
        strcpy(mSet.szUrl, (char *)lParam);
        break;
        
    case WM_MMS_NOTIFY :

        switch (LOWORD(lParam))
        {
        case MMS_CONNECT :
        {
            int recnetwork = 0;
            // the status should CS_CONNECTING
            ASSERT(nConnectState == CS_CONNECTING);

            ASSERT(HIWORD(lParam) == MMS_SUCCESS);

            nConnectState = CS_CONNECTED;

            ASSERT(!IsRSItemTableEmpty());

            // deal with the waiting queue of sending
            for (i = 0 ; i < MAX_RSITEMS; i++)
            {
                switch (RSItemTable[i].nState)
                {
                case RSS_WAITINGRECV :

                    if (!StartRecving(&RSItemTable[i]))
                    {                        
                        ModifyMsgStatus(RSItemTable[i].achFile, MU_RECV_FAILURE, 
                            MMFT_UNRECV);
                        
                        AddRSItemToFailureTable(&RSItemTable[i]);
                        FreeRSItem(&RSItemTable[i]);                        
                    }
                    break;

                case RSS_WAITINGSEND :

                    if (!StartSending(&RSItemTable[i]))
                    {
                        if (RSItemTable[i].achFile[MMS_TYPE_POS] != TYPE_READREPLY)
                        {
                            ModifyMsgStatus(RSItemTable[i].achFile, MU_SEND_FAILURE, 
                                MMFT_UNSEND);
                        }
                        AddRSItemToFailureTable(&RSItemTable[i]);
                        FreeRSItem(&RSItemTable[i]);                        
                    }
                    break;

                case RSS_UNUSED :

                    break;

                default :

                    ASSERT(0);
                    break;
                }
            }

            // if the setting is receive at once, then search the msg unreceived or 
			// unsend to add them into send and receive table
            if (ROAMING == PM_GetNetWorkStatus())
                recnetwork = mSet.visitrec;
            else if (HOMEZONE == PM_GetNetWorkStatus())
                recnetwork = mSet.homerec;
            else
                recnetwork = 2;  //never

            if (recnetwork == 0)  //0: receive at once
                GetRSChainNodeToRSItem();//StartNewItemsFromFile();

            // on this condition, probabely because user delete the file in waiting
			// table, then the table is empty. so try to disconnect
            TryToDisconnect();

            break;
        }

        case MMS_DISCONNECT :
        {
            int nRet;

            if (HIWORD(lParam) == MMS_ENETERR)
                MMSMSGWIN("network error", STR_MMS_MMS, WAITTIMEOUT);
            
            switch (nConnectState)
            {
            case CS_DISCONNECTING :     // call MMS_Disconnect to disconnect normally

				// if the table is not empty, then there is new mms,so connect again.
                if ((!IsRSItemTableEmpty() || pRSChainHead)
                    && !IsFailureTableFull()) 
                  
                {
                    nConnectState = CS_CONNECTING;
                    
                    MMS_Connect(mSet.szIp, MMS_PORT);

                }
                else
                {
                    nConnectState = CS_WAITINGHANGUP;

					// must delete the timer firstly, else the timer will not set rightly
                    KillTimer(hWnd, HANGUP_TIMER_ID);
                    SetTimer(hWnd, HANGUP_TIMER_ID, HANGUP_DELAY_TIME, NULL);
                }
                
                break;

            case CS_CONNECTING :
            case CS_CONNECTED :         // the abnormal disconnected when online
                
                SetFailureTableFull();

                nConnectState = CS_HANGUPING;

                MMS_GprsCount();
                nRet = DIALMGR_HangUp(hWnd);
				printf("DIALMGR_HangUp nRet = %d\r\n", nRet);
                if (nRet != DIALER_REQ_ACCEPT)
                    PostMessage(hwndProtocol, MSG_DIAL_RETURN, 0, 
                        (LPARAM)DIALER_RUN_NET_ABNORMAL);
                break;

            case CS_HANGUPEDDISCONNECTING :

                ASSERT(IsFailureTableFull());

                nConnectState = CS_IDLE;
                
                EmptyFailureTable();
                EmptyRSItemTable();
                EmptyRSchain();
#ifndef _SMARTPHONE
                SetFileTransferSleepFlag(TRUE);
#endif
                MMSMSGWIN("network disconnected", STR_MMS_MMS, MMS_WAITTIME);
                MMS_WaitWindowState(hwndProtocol, FALSE);
                
                ModifyMsgStatus(NULL, MU_DISCONNETED, 0);
#if LILY_DEBUG
                checknum();
#endif
                break;

            default :

                ASSERT(0);
                break;
            }

            break;
        }

        case MMS_SENDED :   // send finished
        
            MMS_SendedFunction(wParam,lParam);
            break;        

        case MMS_ACCEPT :       // receive the notification
        
            MMS_AcceptFunction(hWnd,wParam);

            break;
       
        case MMS_READ :     // receive the data
            
            MMS_ReadFunction(wParam,lParam);

            break;
        
        case MMS_REDIRECT :     // redirect

            // ...

            break;

        default :

            break;
        
        }

        break;

    case WM_TIMER:

        MMS_TimerFunction(hWnd,wParam);
    
        break;

	case WM_CONFIRMREC:
		{
			MMSPENDINGREC pendingRec;

			if (lParam == 0 || IsFailureTableFull() || !MMS_IsFlashEnough())
			{
				//if (MsgInfo.nType == MU_INBOX /*&&mSet.indexTime == MMSSET_AUTO*/)
				if(!MMS_GetFirstPendingReceive(&pendingRec, TRUE))
					break;

				{
					int handle;

					handle = GetHandleByName(pendingRec.pFileName);

					MMSC_ModifyTotal(COUNT_ADD, 0);
					MMSC_ModifyMsgCount(MU_INBOX, COUNT_ADD, 0);                
					MMS_ModifyType(pendingRec.pFileName, TYPE_UNRECV, -1);
					
					if(handle)
						ModifyMsgNode(handle, NULL, MMFT_UNRECV);

					MMSC_ModifyUnreceive(COUNT_ADD, 0);
					NotifyProgram(pendingRec.pFileName);
				}
				break;
			}
			else
			{
				ReceivePendingMMS(hWnd);
			}
		}
		break;
    
    case WM_SELOK:

		if(LOWORD(lParam) == 1)
			MMS_SendReadreply();
		else if(LOWORD(lParam) == 0)
			memset(&wspheadbuf, 0, sizeof(WSPHead));        
        break;

    case WM_CLOSE :

		MMS_FreeFolderInfo();

        break;
        
    default :

        lResult = DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }

    return lResult;
}
/*********************************************************************\
* Function     IsRSItemTableEmpty
* Purpose      
* Params       void
* Return        bool   
**********************************************************************/
static BOOL IsRSItemTableEmpty(void)
{
    return nRSItems == 0;
}

/*********************************************************************\
* Function     
* Params       szFilename   
* Return       true or false
**********************************************************************/
BOOL MMSPro_IsInRSTable(const char *szFilename)
{
    int i;

    for (i = 0; i < MAX_RSITEMS; i++)
    {
        if ((RSItemTable[i].nState != RSS_UNUSED) &&
            (stricmp(RSItemTable[i].achFile, szFilename) == 0))
            return TRUE;
    }
    return FALSE;
}
/*********************************************************************\
* Function     
* Params       szFilename   
* Return       true or false
**********************************************************************/
BOOL MMSPro_IsInRSChain(const char *szFilename)
{
    RSCHAIN *pNode;

    pNode = pRSChainHead;

    while(pNode)
    {
        if (stricmp(pNode->Filename, szFilename) == 0)
            return TRUE;
        pNode = pNode->pNextNode;
    }
    return FALSE;
}

/*********************************************************************\
* Function     
* Params       szFilename   
* Return       true or false
**********************************************************************/
BOOL IsInRSFailueTable(const char *szFilename)
{
    int i;
    
    for (i = 0; i < nFailureRSItems; i++)
    {
        if (stricmp(FailureRSItemTable[i].achFile, szFilename) == 0)
            return TRUE;
    }
    return FALSE;
}
/*********************************************************************\
* Function     IsRSItemTableFull
* Purpose      
* Params       void
* Return       TRUE or FALSE
**********************************************************************/
static BOOL IsRSItemTableFull(void)
{
    return nRSItems == MAX_RSITEMS;
}
/*********************************************************************\
* Function     AllocRSItem
* Purpose      alloc a new table
* Params       void
* Return       a item
**********************************************************************/
static PRSITEM AllocRSItem(void)
{
    static int last;

    int i;

    if (nRSItems == MAX_RSITEMS)
        return NULL;

    // if the table is empty, then the first item is 0
    if (nRSItems == 0)
        last = 0;

    for (i = last; i < MAX_RSITEMS; i++)
    {
        if (RSItemTable[i].nState == RSS_UNUSED)
        {
            last = (last + 1) % MAX_RSITEMS;

            nRSItems++;
            
            return &RSItemTable[i];
        }
    }

     for (i = 0; i < last; i++)
    {
        if (RSItemTable[i].nState == RSS_UNUSED)
        {
            last = (last + 1) % MAX_RSITEMS;
            nRSItems++;
            return &RSItemTable[i];
        }
    }

    return NULL;
}
/*********************************************************************\
* Function     FreeRSItem
* Purpose      free a item
* Params       pRSItem
* Return       void
**********************************************************************/
static void FreeRSItem(PRSITEM pRSItem)
{
    ASSERT(pRSItem >= RSItemTable);
    ASSERT(pRSItem <= RSItemTable + MAX_RSITEMS - 1);

    pRSItem->nState = RSS_UNUSED;    
    pRSItem->achFile[0] = '\0';
    nRSItems--;
    ASSERT(nRSItems <= MAX_RSITEMS);
    
}
/*********************************************************************\
* Function     GetRSItemByHandle
* Purpose      get a item by handle
* Params       hMms:handle
* Return       item
**********************************************************************/
static PRSITEM GetRSItemByHandle(int hMms)
{
    int i;

    for (i = 0; i < MAX_RSITEMS; i++)
    {
        if (RSItemTable[i].nState != RSS_UNUSED && RSItemTable[i].hMms == hMms)
            return &RSItemTable[i];
    }

    return NULL;
}
/*********************************************************************\
* Function     EmptyRSItemTable
* Purpose      
* Params       void
* Return       void   
**********************************************************************/
static void EmptyRSItemTable(void)
{
    int i;
    
    if (nRSItems == 0)
        return;

    for (i = 0 ; i < MAX_RSITEMS; i++)
    {
        //if (MsgInfo.hWnd != NULL)
        {
            if (RSItemTable[i].nState == RSS_WAITINGSEND ||
                RSItemTable[i].nState == RSS_SENDING)
            {
                if (RSItemTable[i].achFile[MMS_TYPE_POS] != TYPE_READREPLY)
                    ModifyMsgStatus(RSItemTable[i].achFile, MU_SEND_FAILURE,
                        MMFT_UNSEND);
            }
            else if (RSItemTable[i].nState == RSS_WAITINGRECV ||
                RSItemTable[i].nState == RSS_RECVING)
            {
                if (RSItemTable[i].bAutoRecv)
                {
					int handle;

                    MMSC_ModifyTotal(COUNT_ADD, 0);
                    MMSC_ModifyMsgCount(MU_INBOX, COUNT_ADD, 0);       
					handle = GetHandleByName(RSItemTable[i].achFile);
                    MMS_ModifyType(RSItemTable[i].achFile, TYPE_UNRECV, -1);
					
					if(handle)
						ModifyMsgNode(handle, NULL, MMFT_UNRECV);

                    MMSC_ModifyUnreceive(COUNT_ADD, 0);
                    NotifyProgram(RSItemTable[i].achFile);
                }
                else
                    ModifyMsgStatus(RSItemTable[i].achFile, MU_RECV_FAILURE, MMFT_UNRECV);
            }
        }

        RSItemTable[i].nState = RSS_UNUSED;
    }
    
    nRSItems = 0;
}
/*********************************************************************\
* Function     InsertRSChainNode
* Purpose      
* Params       pRSChainNode
* Return        
**********************************************************************/
static PRSCHAIN InsertRSChainNode(const char* achFile,int nState, BOOL bAutoRecv)
{
    RSCHAIN *pRSChainNode;
    
    pRSChainNode = MMS_malloc(sizeof(RSCHAIN));
        
    pRSChainNode->bAutoRecv = bAutoRecv;
    
    strcpy(pRSChainNode->Filename,  achFile);

    pRSChainNode->nState = nState;

    pRSChainNode->pNextNode = pRSChainHead;
    pRSChainHead = pRSChainNode;

    return(pRSChainHead);
}
/*********************************************************************\
* Function     DeleteRSChainNode
* Purpose      delete a node
* Params       
* Return     
**********************************************************************/
BOOL DeleteRSChainNodeFromName(const char* pName)
{
	RSCHAIN *pRStempNode;
	
    pRStempNode = pRSChainHead;
	
    while(pRStempNode && strcmp(pRStempNode->Filename,pName) != 0)
    {
        pRStempNode = pRStempNode->pNextNode;
    }
    if(pRStempNode)
    {
        if(pRStempNode == pRSChainHead)
            pRSChainHead = pRStempNode->pNextNode;
        else
            pRStempNode->pNextNode = pRStempNode->pNextNode->pNextNode;
		
        MMS_free(pRStempNode);
        return TRUE;
    }
    else
        return FALSE;
}

static BOOL DeleteRSChainNode(RSCHAIN * pRSChainNode)
{
    RSCHAIN *pRStempNode;

    pRStempNode = pRSChainHead;

    while(pRStempNode && strcmp(pRStempNode->Filename,pRSChainNode->Filename) != 0)
    {
        pRStempNode = pRStempNode->pNextNode;
    }
    if(pRStempNode)
    {
        if(pRStempNode == pRSChainHead)
            pRSChainHead = pRStempNode->pNextNode;
        else
            pRStempNode->pNextNode = pRStempNode->pNextNode->pNextNode;

        MMS_free(pRStempNode);
        return TRUE;
    }
    else
        return FALSE;

}
/*********************************************************************\
* Function     EmptyRSChain
* Purpose      
* Params       
* Return     
**********************************************************************/
static BOOL EmptyRSchain(void)
{
    PRSCHAIN pRSChainNode;
    int      msghandle;

    pRSChainNode = pRSChainHead;

    while(pRSChainNode)
    {
        pRSChainHead = pRSChainNode->pNextNode;
        msghandle = GetHandleByName(pRSChainNode->Filename);
        if (pRSChainNode->nState == RSS_WAITINGSEND)
        {
            ModifyMsgNode(msghandle, pRSChainNode->Filename, MMFT_UNSEND);
        }
        else if(pRSChainNode->nState == RSS_WAITINGRECV)
        {       
            if (pRSChainNode->bAutoRecv)
            {                
                MMSC_ModifyTotal(COUNT_ADD, 0);
                MMSC_ModifyMsgCount(MU_INBOX, COUNT_ADD, 0);
                
                MMS_ModifyType(pRSChainNode->Filename, TYPE_UNRECV, -1);
                MMSC_ModifyUnreceive(COUNT_ADD, 0); 
                
                NotifyProgram(pRSChainNode->Filename);
            }
            else
                ModifyMsgNode(msghandle, pRSChainNode->Filename, MMFT_UNRECV);
        }
        MMS_free(pRSChainNode);
        pRSChainNode = pRSChainHead;
    }
  
    return TRUE;
}
/*********************************************************************\
* Function     GetRSChainNode
* Purpose      get a node in chain
* Params      
* Return     
**********************************************************************/
static BOOL GetRSChainNode(PRSITEM pRSItem)
{
    RSCHAIN *pRSChainNode;

    pRSChainNode = pRSChainHead;
    
    if(pRSChainNode == NULL)
        return FALSE; 

    ASSERT(nConnectState == CS_CONNECTED);
        
    strcpy(pRSItem->achFile, pRSChainNode->Filename);    
    pRSItem->iRecvCount = 1;
    pRSItem->bDelay = 0;
    pRSItem->bAutoRecv = pRSChainNode->bAutoRecv;
    
    pRSItem->nState = pRSChainNode->nState;
    
    pRSChainHead = pRSChainNode->pNextNode;
    MMS_free(pRSChainNode);
    
    if (pRSItem->nState == RSS_WAITINGRECV)
    {       
        if (!StartRecving(pRSItem))
            return FALSE;
        
        ASSERT(pRSItem->nState == RSS_RECVING);
        
        ModifyMsgStatus(pRSItem->achFile, MU_RECV_START, MMFT_RECVING);
    }
    else
    {
        if (!StartSending(pRSItem))
            return FALSE;
        
        ASSERT(pRSItem->nState == RSS_SENDING); 
        
        ModifyMsgStatus(pRSItem->achFile, MU_SEND_START, MMFT_SENDING);
    }
    
    return TRUE;
}
/*********************************************************************\
* Function     GetRSChainNodeToRSItem
* Purpose      
* Params      
* Return     
**********************************************************************/
static BOOL GetRSChainNodeToRSItem(void)
{

    PRSITEM     pRSItem;
    PRSCHAIN    pRSChainNode;

    pRSChainNode = pRSChainHead;

    if(pRSChainNode == NULL)
        return FALSE;
    
    ASSERT(nConnectState == CS_CONNECTED);
    
    pRSItem = AllocRSItem();
    if (!pRSItem)
    {
        return FALSE;
    }
            
    strcpy(pRSItem->achFile, pRSChainNode->Filename);
    pRSItem->bDelay = 0;    
    pRSItem->iRecvCount = 1;
    pRSItem->nState = pRSChainNode->nState;
    pRSItem->bAutoRecv = pRSChainNode->bAutoRecv;
    
    pRSChainHead = pRSChainNode->pNextNode;
    MMS_free(pRSChainNode); 
    
    if (pRSItem->nState == RSS_WAITINGRECV)
    {
        if (StartRecving(pRSItem))
        {             
            ModifyMsgStatus(pRSItem->achFile, MU_RECV_START, MMFT_RECVING);             
            pRSItem = AllocRSItem();
            if (!pRSItem)
                return FALSE;
        }
    }
    else
    {
        if (StartSending(pRSItem))
        {                           
            ModifyMsgStatus(pRSItem->achFile, MU_SEND_START, MMFT_SENDING);
            pRSItem = AllocRSItem();
            if (!pRSItem)
                return FALSE;
        }
    }
    
    if (pRSItem)
        FreeRSItem(pRSItem);
    return TRUE;        
}
/*********************************************************************\
* Function     GetReadreply
* Purpose      get readreply and prepare to send
* Params     
* Return     
**********************************************************************/
static BOOL GetReadreply(PRSITEM pRSItem)
{
    //int       hFind;
    //_FIND_DATA  fd;
	DIR  *dirtemp = NULL;
	struct dirent *dirinfo  = NULL;
	    
    ASSERT(nConnectState == CS_CONNECTED);
    
    chdir(FLASHPATH);
    
	dirtemp = opendir(FLASHPATH);
	if(dirtemp == NULL)
		return FALSE;

	dirinfo = readdir(dirtemp);

     
    if (!dirinfo ||dirinfo->d_name[0] == 0)
	{
		closedir(dirtemp);
        return FALSE;
	}
    
    do
    {   
        if (MMS_GetFileType(dirinfo->d_name) != MMFT_READREPLY)
            continue;       

        if (IsInRSFailueTable(dirinfo->d_name))
            continue;

        strcpy(pRSItem->achFile, dirinfo->d_name);
        pRSItem->bDelay = 0;
        
        if (!StartSending(pRSItem))
            continue;
        
        ASSERT(pRSItem->nState == RSS_SENDING);
        
       	closedir(dirtemp);
        
        return TRUE;
    }while (dirinfo = readdir(dirtemp));

    closedir(dirtemp);  

    return FALSE;
}

/*********************************************************************\
* Function     StartRecving
* Purpose      
* Params       pRSItem:
* Return       Bool：TRUE or FALSE
**********************************************************************/
static BOOL StartRecving(PRSITEM pRSItem)
{
    int     report = !mSet.nReportSend;
    char*   achHeadBuf;
    int     nHeadLen;
    int     hRecvFile;
    int     nRet ;
    unsigned short nType;
    
    chdir(FLASHPATH);
    hRecvFile = MMS_CreateFile(pRSItem->achFile, O_RDONLY);
    if (hRecvFile == -1)
        return FALSE;
    
    nHeadLen = MMS_GetFileSize(pRSItem->achFile) - sizeof(SYSTEMTIME);
    achHeadBuf = MMS_malloc(nHeadLen + 1);
    if(achHeadBuf == NULL)
    {
        MMS_CloseFile(hRecvFile);
        return FALSE;
    }
    read(hRecvFile, achHeadBuf, nHeadLen);

    achHeadBuf[nHeadLen] = 0;

    if(!IsValidNotification(achHeadBuf, nHeadLen))
    {
        MMS_CloseFile(hRecvFile);
        MMS_free(achHeadBuf);
        return FALSE;
    }
    MMS_CloseFile(hRecvFile);

#ifndef _EMULATE_
    KickDog();
#endif

	GetLocalTime(&pRSItem->begintime);

//	MsgWin(NULL, NULL, 0, ML("Retrieving..."), STR_MMS_MMS, Notify_Alert, NULL,
//            NULL,MMS_WAITTIME);

	WaitWinWithTimer(NULL, TRUE, ML("Retrieving..."), STR_MMS_MMS, NULL, NULL,
            NULL, WAITTIMEOUT);
	//for(i=0; i<50000; i++);
    pRSItem->hMms = MMS_Open(0, MMS_RECV);
    ASSERT(pRSItem->hMms != MMS_FAILURE);
    
    nType = pRSItem->achFile[MMS_TYPE_POS];
    
    MMS_SetOption(pRSItem->hMms, MMS_ALWREPORT, &report, sizeof(int));

    if ((nType == TYPE_DELAYRECV) && (!pRSItem->bDelay))
        MMS_RecvDelay(pRSItem->hMms, achHeadBuf, nHeadLen);
    else
        nRet = MMS_Recv(pRSItem->hMms, achHeadBuf, nHeadLen, pRSItem->bDelay);

    MMS_free(achHeadBuf);

    if (nRet == MMS_SUCCESS && pRSItem->bDelay)
        return FALSE;
    
    pRSItem->nState = RSS_RECVING;
	
    return TRUE;
}
/*********************************************************************\
* Function     StartSending
* Purpose      after connected then fill the item to send
* Params       pRSItem
* Return       TRUE，or FALSE
**********************************************************************/
static BOOL StartSending(PRSITEM pRSItem)
{
    int     headLen = 0, bodyLen = 0;
    char    *pBody;
    char    *pHead;
    int     nResult;
    int     hSend;
    WSPHead wHead;
    int     nType;

    chdir(FLASHPATH);
    hSend = MMS_CreateFile(pRSItem->achFile, O_RDONLY);
    if (hSend == -1)
        return FALSE;

     if (!MMS_IsMyfile(hSend))
    {
        MMS_CloseFile(hSend);
        return FALSE;
    }

	GetLocalTime(&pRSItem->begintime);

    nType = pRSItem->achFile[MMS_TYPE_POS];
    
    bodyLen = MMS_GetFileSize(pRSItem->achFile) - sizeof(WSPHead) - MMS_HEADER_SIZE;

    pHead = MMS_malloc(MAX_SENDHEAD_SIZE);

    if(pHead == NULL)
    {
        MMS_CloseFile(hSend);
        return FALSE;
    }

    pBody = MMS_malloc(bodyLen);

    if(pBody == NULL)
    {
        MMS_free(pHead);
        MMS_CloseFile(hSend);
        return FALSE;
    }

    lseek(hSend, MMS_HEADER_SIZE, SEEK_SET);
    read(hSend, &wHead, sizeof(WSPHead));

    headLen = MMS_FillSendHead(wHead, pHead, nType == TYPE_READREPLY);

    read(hSend, pBody, bodyLen);

    MMS_CloseFile(hSend);

    if (headLen == -1)
    {
        MMS_free(pHead);
        MMS_free(pBody);
        return FALSE;
    }

#ifndef _EMULATE_
    KickDog();
#endif
    
    pRSItem->hMms = MMS_Open(0, MMS_SEND);
    
    MMS_SetOption(pRSItem->hMms, MMS_PROXYURI, mSet.szUrl, strlen(mSet.szUrl));
#if LILY_DEBUG
    checknum();
#endif
    nResult = MMS_Send(pRSItem->hMms, pHead, headLen, pBody, bodyLen);
    
    MMS_free(pHead);
    MMS_free(pBody);

	pRSItem->size = headLen + bodyLen;

    if (nResult != MMS_SUCCESS)
    {
        MMSMSGWIN("begin failure", STR_MMS_MMS, MMS_WAITTIME);

        MMS_Close(pRSItem->hMms);
        return FALSE;
    }

    MMSMSGWIN("begin success", STR_MMS_MMS, MMS_WAITTIME);
    
    pRSItem->nState = RSS_SENDING;

    return TRUE;
}
/*********************************************************************\
* Function     IsFailureTableFull
* Purpose      
* Params       void
* Return       TRUE，or FALSE
**********************************************************************/
static BOOL IsFailureTableFull(void)
{
    return nFailureRSItems == MAX_FAILURE_RSITEMS;
}
/*********************************************************************\
* Function     SetFailureTableFull
* Purpose      
* Params       void
* Return       void
**********************************************************************/
static void SetFailureTableFull(void)
{
    nFailureRSItems = MAX_FAILURE_RSITEMS;
}
/*********************************************************************\
* Function  AddRSItemToFailureTable   
* Purpose   
* Params    pRSItem
* Return    BOOL
**********************************************************************/
static BOOL AddRSItemToFailureTable(PRSITEM pRSItem)
{
    if (nFailureRSItems == MAX_FAILURE_RSITEMS)
        return FALSE;

    ASSERT(pRSItem->achFile[0] != 0);

    strcpy(FailureRSItemTable[nFailureRSItems].achFile, pRSItem->achFile);
    nFailureRSItems++;

    return TRUE;
}
/*********************************************************************\
* Function  EmptyFailureTable   
* Purpose   
* Params     void
* Return     void      
**********************************************************************/
static void EmptyFailureTable(void)
{
    nFailureRSItems = 0;
}

/*********************************************************************\
* Function  
**********************************************************************/
unsigned int GetOvertime(int overtime)
{
    switch(overtime) 
    {
    case 0:     //  12hour
        return 12*3600*1000;
    case 1:     //24 hour
        return 24*3600*1000;
    default:    //48 hour
        return 48*3600*1000;
    }
}

static BOOL IsOvertime(const char *pName)
{
    SYSTEMTIME sysTime, expireTime;
    FILETIME    sysFileTime, expireFileTime;
    int         hFile;
    int         nFileSize, nPoint;

    GetLocalTime(&sysTime);
	chdir(FLASHPATH);
    hFile = MMS_CreateFile(pName, O_RDONLY);
	nFileSize = MMS_GetFileSize(pName);
    nPoint = nFileSize - sizeof(SYSTEMTIME);
    lseek(hFile, nPoint, SEEK_SET);
    read(hFile, &expireTime, sizeof(SYSTEMTIME));
    MMS_CloseFile(hFile);
    
    MMS_STtoFT(&sysTime, &sysFileTime);
    MMS_STtoFT(&expireTime, &expireFileTime);

    if (sysFileTime.dwLowDateTime > expireFileTime.dwLowDateTime)
        return TRUE;
    return FALSE;
}
/*
**  Function : GetNewRSItemFromFile
**  Purpose  :
		add a new item according to the order as follows
**          * unreceived mms  delay mms
**          * unsend mms
**          * read reply mms
**/
static BOOL GetNewRSItemFromFile(PRSITEM pRSItem)
{
    //int        hFind;
    //_FIND_DATA  fd;
    int         nFileType;
    int         nTypeMask;
	struct dirent *dirinfo = NULL;
	DIR *dirtemp = NULL;
    
    chdir(FLASHPATH);

	dirtemp = opendir(FLASHPATH);
	if(dirtemp == NULL)
		return FALSE;

	dirinfo = readdir(dirtemp);

    //hFind = FindFirstFile(MMS_FILE_NAME, &fd);
    
    if (dirinfo && dirinfo->d_name[0] != 0)
    {
        if (MMS_IsFlashEnough())
            nTypeMask = MMFT_UNSEND | MMFT_READREPLY | MMFT_UNRECV | MMFT_DELAYRECV;
        else
            nTypeMask = MMFT_UNSEND | MMFT_READREPLY;

        do
        {
            nFileType = MMS_GetFileType(dirinfo->d_name);

            if (!(nFileType & nTypeMask))
                continue;

            //should not judge the overtime if this item is unsend
            if (nFileType != MMFT_UNSEND && IsOvertime(dirinfo->d_name))
            {
              //  MMS_DeleteFile(fd.cFileName);
                continue;
            }
            
            if (!MMSPro_IsInRSTable(dirinfo->d_name) && 
                !IsInRSFailueTable(dirinfo->d_name))
            {
                strcpy(pRSItem->achFile, dirinfo->d_name);
                pRSItem->bDelay = 0;
                if (nFileType == MMFT_UNRECV || nFileType == MMFT_DELAYRECV)
                {
                    pRSItem->nState = RSS_WAITINGRECV;
                    
                    ModifyMsgStatus(pRSItem->achFile, MU_RECV_START, MMFT_RECVING);
                }
                else
                {
                    pRSItem->nState = RSS_WAITINGSEND;
                    
                    if (pRSItem->achFile[MMS_TYPE_POS] != TYPE_READREPLY)
                        ModifyMsgStatus(pRSItem->achFile, MU_SEND_START, MMFT_SENDING);
                }

                closedir(dirtemp);
                return TRUE;
            }
        } while (dirinfo = readdir(dirtemp));

        closedir(dirtemp);
    }
    else
		closedir(dirtemp);
	
    return FALSE;
}
/*********************************************************************\
* Function     add a item unsend or unreceived to send or receive
               the status should be on line
**********************************************************************/
static BOOL StartNewRSItemFromFile(PRSITEM pRSItem)
{
    //int         hFind;
    //_FIND_DATA  fd;
    int         nFileType;
    int         nTypeMask;
	struct dirent *dirinfo =NULL;
	DIR *dirtemp = NULL;
    
    ASSERT(nConnectState == CS_CONNECTED);

    chdir(FLASHPATH);

	dirtemp = opendir(FLASHPATH);
	if(dirtemp == NULL)
		return FALSE;

	dirinfo = readdir(dirtemp);

	if (!dirinfo || dirinfo->d_name[0] == 0)
	{
		closedir(dirtemp);
        return FALSE;
	}

    if (MMS_IsFlashEnough())
        nTypeMask = MMFT_UNSEND | MMFT_READREPLY | MMFT_UNRECV | MMFT_DELAYRECV;
    else
        nTypeMask = MMFT_UNSEND | MMFT_READREPLY;

    do
    {

        nFileType = MMS_GetFileType(dirinfo->d_name);

        if (!(nFileType & nTypeMask))
            continue;

        if ((nFileType != MMFT_UNSEND) && (IsOvertime(dirinfo->d_name)))
        {               
          //  MMS_DeleteFile(fd.cFileName);
            continue;
        }
        
        if (!MMSPro_IsInRSTable(dirinfo->d_name) && 
            !IsInRSFailueTable(dirinfo->d_name))
        {
            strcpy(pRSItem->achFile, dirinfo->d_name);
            pRSItem->bDelay = 0;

            if (nFileType == MMFT_UNRECV || nFileType == MMFT_DELAYRECV)
            {
                 if (!StartRecving(pRSItem))
                     continue;

                 ASSERT(pRSItem->nState == RSS_RECVING);
                 
                 ModifyMsgStatus(pRSItem->achFile, MU_RECV_START, MMFT_RECVING);
            }
            else
            {
                 if (!StartSending(pRSItem))
                     continue;

                 ASSERT(pRSItem->nState == RSS_SENDING);
                 if (pRSItem->achFile[MMS_TYPE_POS] != TYPE_READREPLY)
                     ModifyMsgStatus(pRSItem->achFile, MU_SEND_START, MMFT_SENDING);
            }

            closedir(dirtemp);
            
            return TRUE;
        }
    } while (dirinfo = readdir(dirtemp));

    closedir(dirtemp);
    
    return FALSE;
}
/*********************************************************************\
* Function  StartNewItemsFromFile   
* Purpose   search the files need to receive and send in flash,
			and start to send and receive 
* Params     void
* Return     BOOL      
**********************************************************************/
static BOOL StartNewItemsFromFile(void)
{
    //int        hFind;
   // _FIND_DATA  fd;
    int         nFileType;
    int         nTypeMask;
    PRSITEM     pRSItem;
    BOOL        bFind = FALSE;
	struct dirent *dirinfo =NULL;
	DIR *dirtemp = NULL;

    ASSERT(nConnectState == CS_CONNECTED);
    
    chdir(FLASHPATH);
	dirtemp = opendir(FLASHPATH);
	if(dirtemp == NULL)
	{
		return FALSE;	
	}

	dirinfo = readdir(dirtemp);

   // hFind = FindFirstFile(MMS_FILE_NAME, &fd);
    
    if (!dirinfo ||dirinfo->d_name[0] == 0)
	{
		closedir(dirtemp);
        return FALSE;
	}

    pRSItem = AllocRSItem();
    if (!pRSItem)
    {
        closedir(dirtemp);
        return FALSE;
    }

    if (MMS_IsFlashEnough())
        nTypeMask = MMFT_UNSEND | MMFT_READREPLY | MMFT_UNRECV | MMFT_DELAYRECV;
    else
        nTypeMask = MMFT_UNSEND | MMFT_READREPLY;

    do
    {
        nFileType = MMS_GetFileType(dirinfo->d_name);

        if (!(nFileType & nTypeMask))
            continue;

        if ((nFileType != MMFT_UNSEND) && (IsOvertime(dirinfo->d_name)))
        {               
           // MMS_DeleteFile(fd.cFileName);
            continue;
        }
        
        if (!MMSPro_IsInRSTable(dirinfo->d_name) && 
            !IsInRSFailueTable(dirinfo->d_name))
        {
            strcpy(pRSItem->achFile, dirinfo->d_name);
            pRSItem->bDelay = 0;

            if (nFileType == MMFT_UNRECV || nFileType == MMFT_DELAYRECV)
            {
                if (StartRecving(pRSItem))
                {
                    bFind = TRUE;
                    
                    ModifyMsgStatus(pRSItem->achFile, MU_RECV_START, MMFT_RECVING);

                    pRSItem = AllocRSItem();
                    if (!pRSItem)
                        break;
                }
            }
            else
            {
                if (StartSending(pRSItem))
                {
                    bFind = TRUE;

                    if (pRSItem->achFile[MMS_TYPE_POS] != TYPE_READREPLY)
                        ModifyMsgStatus(pRSItem->achFile, MU_SEND_START, MMFT_SENDING);
                    pRSItem = AllocRSItem();
                    if (!pRSItem)
                        break;
                }
            }//end else
        }// end inTable
    } while (dirinfo = readdir(dirtemp));

    closedir(dirtemp);

    if (pRSItem)
        FreeRSItem(pRSItem);

    return bFind;
}
/*********************************************************************\
* Function  TryToDisconnect   
* Purpose   
* Params    void   
* Return    void
**********************************************************************/
static void TryToDisconnect(void)
{
    int nRet;

    if (nConnectState != CS_DISCONNECTING &&
        nConnectState != CS_HANGUPEDDISCONNECTING)
    {
        ASSERT(nConnectState == CS_CONNECTED);

        // if the table is empty, then disconnected
        if (IsRSItemTableEmpty())
        {
            nConnectState = CS_DISCONNECTING;
            // if the network has been disconnected, this function will return failue
            // but there is still has this msg of Disconnect
            nRet = MMS_Disconnect();
        }
    }
}
/*********************************************************************\
            call dialing
**********************************************************************/
static BOOL Try_StartDialing(void)
{   
    int nDialr;
	DIALER_GPRSINFO dialtemp;
	int dialType;

    ASSERT(nConnectState == CS_IDLE);
	memset(&dialtemp, 0, sizeof(DIALER_GPRSINFO));
	strcpy(dialtemp.APN, mSet.ISPPhoneNum1);
	strcpy(dialtemp.PassWord, mSet.ISPPassword);
	strcpy(dialtemp.UserID, mSet.ISPUserName);
	
	if(mSet.DtType == 1)
		dialType = DIALDEF_SELFGPRS;
	else if(mSet.DtType == 0)
		dialType = DIALDEF_SELFDATA;

    nDialr = DIALMGR_SelConnect(hwndProtocol, MSG_DIAL_RETURN, dialType, 
        &dialtemp, TRUE);

	printf("DIALMGR_SelConnect return = %d\r\n", nDialr);
	
    if (nDialr == DIALER_REQ_ACCEPT)
    {
        ModifyMsgStatus(NULL, MU_CONN_START, 0);
        return TRUE;
    }
    else
        return FALSE;
}
/*********************************************************************\
            call dialing
**********************************************************************/
static BOOL StartDialing(void)
{   
    if (Try_StartDialing())
    {
        dialnum = 1;
        return TRUE;
    }
    else
        return FALSE;        
}
/*********************************************************************\
* Function    retrieve the mms 
* Purpose      
**********************************************************************/
BOOL MMSPro_RecvDelay(const char* DelayFileName)
{
    PRSITEM pRSItem;
    int     msghandle;
    int     hFile;
    char    *pSizeField = NULL;
    char    pHeadBuf[MAX_ACCEPT_BUF + 1];
    char    *pExpiry = NULL;
    int     nHeadLen = 0,lensize = 0, len = 0;
    SYSTEMTIME  expireTime, expireChangeTime;
    FILETIME    expireFileTime,expireChangeFileTime;

    if (IsFailureTableFull())
        return FALSE;

    if (MMSPro_IsInRSTable(DelayFileName))
        return FALSE;

    if (!MMS_IsFlashEnough())
        return FALSE;

    pHeadBuf[0] = 0;

    chdir(FLASHPATH);
    hFile = MMS_CreateFile(DelayFileName, O_RDONLY);
    if (hFile == -1)
        return FALSE;
    nHeadLen = MMS_GetFileSize(DelayFileName) - sizeof(SYSTEMTIME);
    read(hFile, (PVOID)pHeadBuf, nHeadLen);    
    read(hFile, &expireChangeTime, sizeof(SYSTEMTIME));
    MMS_CloseFile(hFile);

    pExpiry = FindContentByKey(pHeadBuf, "X-Mms-Expiry", &len);
    ASSERT(pExpiry != NULL);  
    MMS_StrToSysTime(pExpiry, &expireTime);
    MMS_STtoFT(&expireTime, &expireFileTime);
    MMS_STtoFT(&expireChangeTime, &expireChangeFileTime);
    if(expireFileTime.dwLowDateTime != expireChangeFileTime.dwLowDateTime)
    {
        MsgWin(NULL, NULL, 0, STR_MMS_EXPIRED, STR_MMS_MMS, Notify_Alert, 
			STR_MMS_OK, NULL,MMS_WAITTIME);
        return FALSE;
    }   
            
    pSizeField = FindContentByKey(pHeadBuf, "X-Mms-Message-Size:",
        &lensize);
    if (pSizeField)
    {
        char tempSize[SIZE_2];
        
        tempSize[0] = 0;
        strncpy(tempSize, pSizeField, lensize);
        tempSize[lensize] = '\0';
        if(atoi(tempSize) > MAX_MMS_SIZE)
        {
            MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, STR_MMS_MMS,Notify_Alert,
				STR_MMS_OK, NULL,MMS_WAITTIME);
            return FALSE;
        }
    }

    pRSItem = AllocRSItem();
    if (!pRSItem)
    {
        InsertRSChainNode(DelayFileName,RSS_WAITINGRECV, FALSE);
        msghandle = GetHandleByName(DelayFileName);
        //if (MsgInfo.nType == MU_INBOX)
        {
            ModifyMsgStatus(DelayFileName, MU_RECV_START, MMFT_RECVING);
        }
        return TRUE;
    }

    pRSItem->bAutoRecv = FALSE;
    
    strcpy(pRSItem->achFile, DelayFileName);
    pRSItem->bDelay = 0;
    
    if (nConnectState == CS_CONNECTED)
    {        
        // start receiving...
        if (!StartRecving(pRSItem))
        {
            msghandle = GetHandleByName(DelayFileName);
            ModifyMsgNode(msghandle,DelayFileName,MMFT_UNRECV);
            FreeRSItem(pRSItem);
            return FALSE;
        }
        
        ModifyMsgStatus(DelayFileName, MU_RECV_START, MMFT_RECVING);
    }
    else
    {
        if (nConnectState == CS_IDLE)
        {
            if (!StartDialing())
            {
                msghandle = GetHandleByName(DelayFileName);
                ModifyMsgNode(msghandle,DelayFileName,MMFT_UNRECV);
                FreeRSItem(pRSItem);
				MsgWin(NULL, NULL, 0, STR_MMS_RECVFAI, STR_MMS_MMS, Notify_Failure,
					STR_MMS_OK, NULL,MMS_WAITTIME);
                return FALSE;
            }
            
            nConnectState = CS_DIALING;
#ifndef _SMARTPHONE
            SetFileTransferSleepFlag(FALSE);
#endif
        }
        else if (nConnectState == CS_WAITINGHANGUP)
        {
            nConnectState = CS_CONNECTING;
            KillTimer(hwndProtocol, HANGUP_TIMER_ID);
            
            MMS_Connect(mSet.szIp, MMS_PORT);
        }

        pRSItem->nState = RSS_WAITINGRECV;
        
        ModifyMsgStatus(DelayFileName, MU_RECV_START, MMFT_RECVING);
    }

    return TRUE;
}

BOOL MMSPro_IsIdle(void)
{
    return (nConnectState == CS_IDLE);
}
/*********************************************************************\
* Function     MMSPro_Cancel
* Purpose      
* Params       void
* Return       void
**********************************************************************/
void  MMSPro_Cancel(void)
{
    int nRet;
    
    if (nConnectState == CS_IDLE)
        return;

    dialnum = MAX_DIALTRYNUM;
    MMS_WaitWindowState(hwndProtocol, TRUE);
    
    SetFailureTableFull();

    switch (nConnectState)
    {
    case CS_DIALING :
        nConnectState = CS_HANGUPING;
		MMS_GprsCount();
        nRet = DIALMGR_HangUp(hwndProtocol);
		printf("DIALMGR_HangUp nRet = %d\r\n", nRet);
        if (nRet != DIALER_REQ_ACCEPT)
            PostMessage(hwndProtocol, MSG_DIAL_RETURN, 0, (LPARAM)DIALER_RUN_NET_ABNORMAL);
        break;

    case CS_CONNECTING :
    case CS_CONNECTED : 

        nConnectState = CS_DISCONNECTING;
        nRet = MMS_Disconnect();
        ASSERT(nRet != MMS_FAILURE);
        if(nRet == MMS_FAILURE)
            MMS_WaitWindowState(hwndProtocol, FALSE);

        break;

    case CS_DISCONNECTING :           
    case CS_HANGUPING :               
    case CS_HANGUPEDDISCONNECTING :   

        break;

    case CS_WAITINGHANGUP :
        
        KillTimer(hwndProtocol, HANGUP_TIMER_ID);

        nConnectState = CS_HANGUPING;
		MMS_GprsCount();
        nRet = DIALMGR_HangUp(hwndProtocol);
		printf("DIALMGR_HangUp nRet = %d\r\n", nRet);
        if (nRet != DIALER_REQ_ACCEPT)
            PostMessage(hwndProtocol, MSG_DIAL_RETURN, 0, (LPARAM)DIALER_RUN_NET_ABNORMAL);

        break;

    default :
        
        ASSERT(0);
        MMS_WaitWindowState(hwndProtocol, FALSE);      
        break;
    }
}
/*********************************************************************\
* Function    
* Purpose     
**********************************************************************/
void  MMSPro_ReadReply(const char* pName)
{
    int       hfile;

    chdir(FLASHPATH);

    hfile = MMS_CreateFile(pName, O_RDONLY);
    if (hfile == -1)
    {
        MMSMSGWIN("open file failed ", STR_MMS_MMS, MMS_WAITTIME);
        return;
    }
    if (!MMS_IsMyfile(hfile))
    {
        MMS_CloseFile(hfile);
        return;
    }

    lseek(hfile, MMS_HEADER_SIZE, SEEK_SET);
    read(hfile, &wspheadbuf, sizeof(WSPHead));
    MMS_CloseFile(hfile);

    return;
    
//    if (!wspheadbuf.bReply || !mSet.bReadReply)     
//        return;
//    else
//    {
//        MsgSelWin(hwndProtocol, STR_MMS_SUREREPLY, STR_MMS_MMS, WM_SELOK);
//    }//end reply
}//end MMS_ReadReply

/*********************************************************************\
 * Function    SaveMmsRecv
 * Return      int
**********************************************************************/
static int SaveMmsRecv(int headLen, char *pHead, int bodyLen, const char *pBody,
                   const char *pFileName)
{
    int         hMmsFile;
    int         acceptLen;
    char        *pAcceptBuf;
    char        *pSizeField;
    int         nRet;
    WSPHead     wHead;
   
    memset(&wHead, 0, sizeof(WSPHead));
    ParseRecvHead(pHead, headLen, &wHead);

    chdir(FLASHPATH);
    hMmsFile = MMS_CreateFile(pFileName, O_RDWR);

    if (hMmsFile == -1)
    {
        hMmsFile = MMS_CreateFile(pFileName, O_CREAT | O_RDWR);
    }
    else
    {
        acceptLen = MMS_GetFileSize(pFileName);
        pAcceptBuf = (char*)MMS_malloc(acceptLen + 1);
        read(hMmsFile, pAcceptBuf, acceptLen);
        pSizeField = FindContentByKey(pAcceptBuf, "X-Mms-Message-Size:", &acceptLen);
        if (pSizeField)
        {
            pSizeField[acceptLen] = '\0';
            wHead.nMmsSize = atoi(pSizeField);
        }
        MMS_free(pAcceptBuf);
    }
    //write mms and body
    lseek(hMmsFile, 0, SEEK_SET);
    MMS_WriteHead(hMmsFile, 0);
    lseek(hMmsFile, MMS_HEADER_SIZE, SEEK_SET);
    nRet = write(hMmsFile, &wHead, sizeof(WSPHead));
    if (nRet > 0 && nRet < sizeof(WSPHead))
    {
        MMS_CloseFile(hMmsFile);
        MsgWin(NULL, NULL, 0, STR_MMS_NEGH, STR_MMS_MMS, 
			Notify_Alert, STR_MMS_OK, NULL, 10);
        return ERR_WRITEFILE;
    }
    nRet = write(hMmsFile, (PVOID)pBody, bodyLen);
    if (nRet > 0 && nRet < bodyLen)
    {
        MMS_CloseFile(hMmsFile);
        MsgWin(NULL, NULL, 0, STR_MMS_NEGH, STR_MMS_MMS, Notify_Alert,
			STR_MMS_OK, NULL, 10);
        return ERR_WRITEFILE;
    }   
    MMS_CloseFile(hMmsFile);

    return RETURN_OK;
}
/*********************************************************************\
* Function     PackMmsBody
* Purpose      pack mms file
* Params       mmsdraftfile
* Return       int
                2: not found the file
                3: not mms file
**********************************************************************/
int PackMmsBody(const char *pFileName, TotalMMS tmms)
{
    char    *pBody;
    int     bodyLen = 0;
    int     hFile;
    int     nRet;
    
    pBody = (char*)MMS_malloc(MAX_SENDBODY_SIZE);

    if (!pBody)
        return ERR_MALLOCFAI;

    bodyLen = MMS_FillSendBody(tmms, pBody);
        
    if (bodyLen < 0)
    {
        MMS_free(pBody);
        return ERR_MALLOCFAI;
    }

    chdir(FLASHPATH);

    hFile = MMS_CreateFile(pFileName, O_RDWR);
    if (hFile == -1)
    {
        hFile = MMS_CreateFile(pFileName, O_CREAT|O_RDWR  );
        if (hFile == -1)
        {
            MMS_free(pBody);
            return ERR_NOTFOUND;
        }
    }

    //SetFileSize(hSend, 0);
	truncate(pFileName, 0);
    MMS_WriteHead(hFile, 0);
    nRet = write(hFile, &tmms.mms.wspHead, sizeof(WSPHead));
    if (nRet > 0 && nRet < sizeof(WSPHead))
    {
        MMS_CloseFile(hFile);
        MMS_free(pBody);
        MsgWin(NULL, NULL, 0, STR_MMS_NEGH, STR_MMS_MMS, Notify_Alert, 
			STR_MMS_OK, NULL, 10);
        return ERR_WRITEFILE;
    }

    nRet = write(hFile, (PVOID)pBody, bodyLen);
    if (nRet > 0 && nRet < bodyLen)
    {
        MMS_CloseFile(hFile);
        MMS_free(pBody);
        MsgWin(NULL, NULL, 0, STR_MMS_NEGH, STR_MMS_MMS, Notify_Alert, 
			STR_MMS_OK, NULL, 10);
        return ERR_WRITEFILE;
    }
    MMS_CloseFile(hFile);

    MMS_free(pBody);

    return RETURN_OK;
}

static BYTE GetLocalDayNums(PSYSTEMTIME pnowS)
{
    switch(pnowS->wMonth)
    {
    case 2:
        {
            if((pnowS->wYear%4 == 0 && pnowS->wYear%100 != 0)
                ||pnowS->wYear%400 == 0)
            {
                return 29;
            }
            return 28;
        }
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        return 31;
    default:
        return 30;
    }

    return 31;
}

static BOOL GetSomeDayTime(DWORD day,PSYSTEMTIME pSystemTime)
{
    SYSTEMTIME nowS;
    FILETIME nowF;
    DWORD offset,nowFday,localdaynum;
    DWORD mark = 0x001f0000;

    if (!pSystemTime || day > 30)
    {
        return FALSE;
    }

    offset = day<<16;
    if((offset & mark)==0)
    {
        return FALSE;
    }

    GetLocalTime(&nowS);
    MMS_STtoFT(&nowS,&nowF);
    nowFday = (nowF.dwLowDateTime & mark) >> 16;
    localdaynum = GetLocalDayNums(&nowS);
    if(nowFday+day <= localdaynum)
    {
        nowF.dwLowDateTime += offset; //add day
    }
    else
    {
        offset = (nowFday + day - localdaynum) << 16;
        nowF.dwLowDateTime &= 0xffe0ffff;
        nowF.dwLowDateTime |= offset;  //add day
        
        nowF.dwLowDateTime += 0x00200000;//add month

        if(nowS.wMonth == 12)
            nowF.dwLowDateTime += 0x02000000;//add year

    }
    MMS_FTtoST(&nowF,pSystemTime);
    if (pSystemTime->wMonth == 13)
        pSystemTime->wMonth = 1;
    pSystemTime->wDayOfWeek = (unsigned short)(nowS.wDayOfWeek + day) % 7;
    return TRUE;
}

static void ConvertChar(PSTR to)
{
	char *p;

	p = to;

	while (p)
	{
		if ((BYTE)*p == 0x00)
			break;

		if ((BYTE)*p == 0xfd)
			*p = 0x40;
		p++;
	}
}
/*********************************************************************\
* Function     fill the send head
* Params       SendFileName:
                pHead:      
* Return        headlen:    
**********************************************************************/
static int MMS_FillSendHead(WSPHead wspheadBuf, char* pHead, BOOL bReadReply)
{
//    char  headTemplate[] = "From:%s\r\n%sSubject:%s\r\nX-Mms-Message-Class:%s\r\n"
//        "X-Mms-Priority:%s\r\nX-Mms-Delivery-Report:%s\r\nX-Mms-Read-Reply:%s\r\n"
//        "%sContent-Type:application/vnd.wap.multipart.related;"
//        "type=\"application/smil\";"
//        "start=\"<0000>\"\r\n\r\n";   
    char    headTemplate[] = "From:%s\r\n%sSubject:%s\r\nX-Mms-Message-Class:%s\r\n"
        "X-Mms-Priority:%s\r\nX-Mms-Delivery-Report:%s\r\nX-Mms-Read-Reply:%s\r\n"
        "%sContent-Type:application/vnd.wap.multipart.related;"
        "Type=application/smil;"
        "Start=<0000>\r\n\r\n";

    char    headTemplateMixed[] = "From:%s\r\n%sSubject:%s\r\nX-Mms-Message-Class:%s\r\n"
        "X-Mms-Priority:%s\r\nX-Mms-Delivery-Report:%s\r\nX-Mms-Read-Reply:%s\r\n"
        "%sContent-Type:application/vnd.wap.multipart.mixed;"
        "Type=application/smil;"
        "Start=<0000>\r\n\r\n";

    char    from[SIZE_1], *to;
    char    strExpiry[100];
    char    strTime[60];
    char    *mClass[] = {"Personal", "Auto"};
    char    *Priority[] = {"High", "Normal", "Low"};
    char    *ReadReport[] = {"No", "Yes"};
    int      nExpiry[] = {1, 2, 7};
    char    *toTokens;
    BOOL    bReport, bReply;
    SYSTEMTIME      expiryTime;
    int     headlen;
    char    szUtf8Sub[SIZE_1];
    int     subLen, utf8Len;
    int     toLen = 0;

     // fill from
    if (wspheadBuf.from[0] != '\0')
    {       
        strcpy(from, wspheadBuf.from);
        if (strchr(wspheadBuf.from, '@') == NULL)
            strcat(from, "/TYPE=PLMN");
    }
    else
    {
        strcpy(from, "\r"); // from is null
    }

    // if the address is email then don`t need type of PLMN
    
    to = MMS_malloc(SIZE_6 * 2);
    if(to == NULL)
        return -1;
    
    toTokens = MMS_chrtok(wspheadBuf.to,  MMS_ASEPCHR, MMS_ASEPCHRF);   
    to[0] = '\0';
    while(toTokens != NULL )
    {
        if (*toTokens == '\0')
        {
            toTokens = MMS_chrtok(NULL,  MMS_ASEPCHR, MMS_ASEPCHRF);    
            continue;
        }

        toLen += strlen("To:");
        if (toLen >= SIZE_6*2)
            break;
        strcat(to, "To:");        
        toLen += strlen(toTokens);
        if (toLen >= SIZE_6*2)
            break;
        strcat(to, toTokens);
        if (strchr(toTokens, '@') == NULL)
        {
            toLen += strlen("/TYPE=PLMN\r\n");
            if (toLen >= SIZE_6*2)
                break;
            strcat(to, "/TYPE=PLMN\r\n");
        }
        else
        {
            toLen += strlen("\r\n");
            if (toLen >= SIZE_6*2)
                break;
            strcat(to, "\r\n");
        }
        
        /* Get next token: */
        toTokens = MMS_chrtok(NULL,  MMS_ASEPCHR, MMS_ASEPCHRF);    
    }
    
	// convert @(0xfd) to 0x40
	ConvertChar(to);
    // fill expiry X-Mms_Expiry:%s\r\n
    strExpiry[0] = '\0';
    if (mSet.indexExpire != 3)
    {
        GetSomeDayTime(nExpiry[mSet.indexExpire], &expiryTime);
        MMS_SysTimeToStr(expiryTime, strTime);
        sprintf(strExpiry, "X-Mms-Expiry:%s\r\n", strTime);
    }
    
    bReport = !mSet.nReportRec;
    bReply = mSet.indexReply;

    if (bReadReply)
    {
        if (!mSet.nReportRec)
            bReport = 0;
        if (mSet.indexReply)
            bReply = 0;
    }

    subLen = strlen(wspheadBuf.subject);    
    szUtf8Sub[0] = '\0';
    if (subLen > 0)
    {
        utf8Len = MMS_Gb2Utf8(wspheadBuf.subject, subLen);
        
        //szUtf8Sub[0] = (char)0xEA;
        //szUtf8Sub[1] = (char)0x7F;
        memcpy(szUtf8Sub, wspheadBuf.subject, utf8Len);
        strcpy(szUtf8Sub + utf8Len - 1, ";Charset=utf-8");
        //szUtf8Sub[utf8Len] = '\0';
    }
    if(stricmp(wspheadBuf.ConType, "application/vnd.wap.multipart.mixed") == 0)
    {
        headlen = sprintf(pHead, headTemplateMixed, from, to, szUtf8Sub, mClass[bReadReply], 
            Priority[mSet.nPriority], ReadReport[bReport], ReadReport[bReply], strExpiry);
    }
    else
        headlen = sprintf(pHead, headTemplate, from, to, szUtf8Sub, mClass[bReadReply], 
            Priority[mSet.nPriority], ReadReport[bReport], ReadReport[bReply], strExpiry);

     MMS_free(to);
     if (headlen >= MAX_SENDHEAD_SIZE)        
         return -1;
     return headlen;
}
/*********************************************************************\
* Function     fill the body
* Params       SendFileName: 
                pBody: 
* Return        bodylen
**********************************************************************/
static int  MMS_FillSendBody(TotalMMS tmms, char *pbody)
{
    MMSMultiBody    *sendBody;
    mmsMetaNode     *pNode = NULL;
    int             iBodynum = 1, nsNum;
    int             fillnum = 0, i;
    int             bodylen = 0;

    nsNum = tmms.mms.wspHead.nSlidnum;
    /////////////////////////////////////////////////////////
    
    pNode = tmms.pAttatch;
    while (pNode)
    {
        if (pNode->Metalen > 0)
            iBodynum++;
        pNode = pNode->pnext;
    }

    sendBody = (MMSMultiBody*)MMS_malloc(sizeof(MMSMultiBody) + (iBodynum - 1)*(sizeof(MMSBody)));
    sendBody->Bodynum = iBodynum;
    
    sendBody->body[fillnum].pHead = (char*)MMS_malloc(BODY_HEAD_SIZE);
    sendBody->body[fillnum].pBody = (char*)MMS_malloc(SMIL_BODY_SIZE);
    MMS_FillSmil(&tmms.mms, sendBody, fillnum);

    bodylen += sendBody->body[fillnum].headlen;
    bodylen += sendBody->body[fillnum].bodylen;
    
    fillnum++;
    
    // attachment
    pNode = tmms.pAttatch;
    while (pNode)
    {
        if (pNode->Metalen <= 0)
        {            
            pNode = pNode->pnext;
            continue;
        }

        sendBody->body[fillnum].pHead = (char*)MMS_malloc(BODY_HEAD_SIZE);
        if (pNode->MetaType/10 == META_TEXT)
            sendBody->body[fillnum].pBody = 
                (char*)MMS_malloc(pNode->Metalen * TEXT_CODEMULT + 1 );
        else
            sendBody->body[fillnum].pBody = (char*)MMS_malloc(pNode->Metalen + 1 );
        if (sendBody->body[fillnum].pBody == NULL)
        {
            bodylen = -1;
            goto EXIT;
        }
    
        switch(pNode->MetaType/10) 
        {
        case META_IMG:
            MMS_FillImageAttatch(pNode, sendBody, fillnum);
            break;
        case META_AUDIO:
            MMS_FillAudioAttatch(pNode, sendBody, fillnum);
            break;
        case META_TEXT:
            MMS_FillTextAttatch(pNode, sendBody, fillnum);
            break;
        default:
            break;
        }
        
        bodylen += sendBody->body[fillnum].headlen;
        bodylen += sendBody->body[fillnum].bodylen; 
        
        fillnum++;
        pNode = pNode->pnext;
    }

    if (fillnum != sendBody->Bodynum)
        sendBody->Bodynum = fillnum;

    //wap pack
    bodylen += fillnum * PACKLENGTH;
    bodylen += BODY_ADDLEN;
    if (bodylen < MAX_SENDBODY_SIZE)
    {   
        bodylen = MMS_PackMulitBody(sendBody, pbody, bodylen);
        
        ASSERT(bodylen < MAX_SENDBODY_SIZE);
    }
    else
        bodylen = -1;
    
    ASSERT(bodylen < MAX_SENDBODY_SIZE);
EXIT:   
    for (i = fillnum - 1;i>=0;i--)
    {
        if (sendBody->body[i].pBody != NULL)
            MMS_free(sendBody->body[i].pBody);
        if (sendBody->body[i].pHead != NULL)
            MMS_free(sendBody->body[i].pHead);
    }

    MMS_free(sendBody);

    return bodylen;
}
/*********************************************************************\
* Function     MMS_FillSmil   
* Purpose      create smil file
* Params       pMMS
               pSmil
               nNum
* Return        void
**********************************************************************/
static void MMS_FillSmil(MultiMMS *pMMS, MMSMultiBody* pSmil, int nNum)
{
    char smilhead[] = "Content-Type:application/smil\r\nContent-ID:\"<0000>\"\r\n";
    char *smilbody;
    int   sheadlen, i;
    int   len, bodylen;

    // smil head
    sheadlen = strlen(smilhead);

    pSmil->body[nNum].headlen = MMS_PackHeadersInBody(smilhead, sheadlen,  
        pSmil->body[nNum].pHead, sheadlen);

    // smil body
    smilbody = pSmil->body[nNum].pBody;
    bodylen = 0;
    len = sprintf(smilbody, 
            "<smil>\r\n<head>\r\n<layout>\r\n<root-layout width=\"%d\" height=\"%d\"/>",
            SCREEN_WIDTH,SCREEN_HEIGHT);
    smilbody += len;
    bodylen += len;
    
    for (i = 0; i < pMMS->wspHead.nSlidnum; i++)
    {
        if (pMMS->slide[i].pImage)
        {
            //region image
            len = sprintf(smilbody, 
                "\r\n<region id=\"image%d\" width=\"%s\" height=\"%s\" left=\"%d\" top=\"%d\"/>",
                i,"100%", "100%",pMMS->slide[i].imgPoint.x, pMMS->slide[i].imgPoint.y);
            
            smilbody += len;
            bodylen += len;
        }
        
        if (pMMS->slide[i].pText && pMMS->slide[i].pText->Metalen >0)
        {
            //region text           
            len = sprintf(smilbody, 
                "\r\n<region id=\"text%d\" width=\"%s\" height=\"%s\" left=\"%d\" top=\"%d\"/>",
                i,"100%", "100%", pMMS->slide[i].txtPoint.x, pMMS->slide[i].txtPoint.y);
            
            smilbody += len;
            bodylen += len;
        }   
    }   
    len = strlen("\r\n</layout>\r\n</head>\r\n<body>");
    memcpy(smilbody, "\r\n</layout>\r\n</head>\r\n<body>", len);
    smilbody += len;
    bodylen += len;

    for (i = 0; i<pMMS->wspHead.nSlidnum; i++)
    {
        len = sprintf(smilbody, "\r\n<par dur= \"%dms\">", pMMS->slide[i].dur);

        smilbody += len;
        bodylen += len;

        if (pMMS->slide[i].pImage)
        {
            len = sprintf(smilbody, "\r\n<img src=\"%s\" region=\"image%d\"/>",
                pMMS->slide[i].pImage->Content_Location, i);

            smilbody += len;
            bodylen += len;
        }
    
        if (pMMS->slide[i].pText && pMMS->slide[i].pText->Metalen > 0)
        {
            len = sprintf(smilbody, "\r\n<text src=\"%s\" region=\"text%d\"/>",
                pMMS->slide[i].pText->Content_Location, i);

            smilbody += len;
            bodylen += len;
        }
    
        if (pMMS->slide[i].pAudio)
        {
            len = sprintf(smilbody, "\r\n<audio src=\"%s\"/>",
                pMMS->slide[i].pAudio->Content_Location);

            smilbody += len;
            bodylen += len;
        }
        len = strlen("\r\n</par>");
        memcpy(smilbody, "\r\n</par>", len);
        smilbody += len;
        bodylen += len; 
    }
    len = strlen("\r\n</body>\r\n</smil>");
    memcpy(smilbody, "\r\n</body>\r\n</smil>", len);
    smilbody += len;
    bodylen += len; 
    
    pSmil->body[nNum].bodylen = bodylen;
    ASSERT(bodylen < SMIL_BODY_SIZE);
//  smilbody[pSmil->body[nNum].bodylen] = '\0';
}
/*********************************************************************\
* Function     MMS_FillImageAttatch   
* Purpose      fill iamge
* Return        void
**********************************************************************/
static void MMS_FillImageAttatch(mmsMetaNode *pNode, MMSMultiBody *pImg, int nNum)
{
    int  imgheadlen;
    char imgheadtemplate[] = "Content-Type: image/%s\r\nContent-Location:%s\r\nContent-ID:\"<%s>\"\r\n";
        //"Content-Type: image/%s\r\nContent-Location:%s\r\n";
    char *imgtype[] = {"*","gif","jpeg","bmp","vnd.wap.wbmp", "png"};

    //fill image head   
    imgheadlen = sprintf(pImg->body[nNum].pHead, imgheadtemplate, 
                         imgtype[pNode->MetaType%10], 
                         pNode->Content_Location, pNode->Content_Location);

    ASSERT(imgheadlen < BODY_HEAD_SIZE);

    pImg->body[nNum].headlen = MMS_PackHeadersInBody(pImg->body[nNum].pHead,
                            imgheadlen,  pImg->body[nNum].pHead, imgheadlen);

    ASSERT(pImg->body[nNum].headlen < imgheadlen);

    //image body    
    memcpy(pImg->body[nNum].pBody, pNode->Metadata, pNode->Metalen);
    pImg->body[nNum].bodylen = pNode->Metalen;
}
/*********************************************************************\
* Function     MMS_FillTextAttatch   
* Purpose      fill Text
* Return        void
**********************************************************************/
static void MMS_FillTextAttatch(mmsMetaNode *pNode, MMSMultiBody *pTxt, int nNum)
{
    int  txtheadlen;
    char txtheadtemplate[] = "Content-Type:text/%s;Charset=utf-8\r\n"
        "Content-Location:%s\r\nContent-ID:\"<%s>\"\r\n";
    char *txttype[] = {"*","plain","x-vCalendar","x-vCard"};

    //fill text head
    txtheadlen = sprintf(pTxt->body[nNum].pHead, txtheadtemplate, 
        txttype[pNode->MetaType%10], pNode->Content_Location, pNode->Content_Location);

    ASSERT(txtheadlen < BODY_HEAD_SIZE);

    pTxt->body[nNum].headlen = MMS_PackHeadersInBody(pTxt->body[nNum].pHead, 
                                txtheadlen,  pTxt->body[nNum].pHead, txtheadlen);

    ASSERT(pTxt->body[nNum].headlen < txtheadlen);
    //fill text body
    memcpy(pTxt->body[nNum].pBody, pNode->Metadata, pNode->Metalen);
    pTxt->body[nNum].pBody[pNode->Metalen] = '\0';
    pTxt->body[nNum].bodylen = MMS_Gb2Utf8(pTxt->body[nNum].pBody, pNode->Metalen);
    pTxt->body[nNum].pBody[pTxt->body[nNum].bodylen] = '\0';
}
/*********************************************************************\
* Function     MMS_FillAudioAttatch
* Purpose      fill Audio
* Return        void
**********************************************************************/
static void MMS_FillAudioAttatch(mmsMetaNode *pNode, MMSMultiBody *pAudio, int nNum )
{
    int     audioheadlen;
    char audioheadtemplate[] =
        "Content-Type: audio/%s\r\nContent-Location:%s\r\nContent-ID:\"<%s>\"\r\n";
    char *audiotype[] =
    {"*", "amr", "midi", "x-wav", "x-smaf", "mpeg","x-iMelody"};

    if(pNode->MetaType == META_AUDIO_IMY)
    {
        char audioheadtemplate1[] =
            "Content-Type: text/%s\r\nContent-Location:%s\r\nContent-ID:\"<%s>\"\r\n";

        audioheadlen = sprintf(pAudio->body[nNum].pHead, audioheadtemplate1, 
        audiotype[6], pNode->Content_Location, pNode->Content_Location);
    }
    else if(pNode->MetaType == META_AUDIO_MMF)
    {
        char audioheadtemplate2[] =
            "Content-Type: application/%s\r\nContent-Location:%s\r\nContent-ID:\"<%s>\"\r\n";

        audioheadlen = sprintf(pAudio->body[nNum].pHead, audioheadtemplate2, 
        audiotype[4], pNode->Content_Location, pNode->Content_Location);

    }
    else
        //fill audio head
        audioheadlen = sprintf(pAudio->body[nNum].pHead, audioheadtemplate, 
        audiotype[pNode->MetaType%10], pNode->Content_Location, pNode->Content_Location);

    ASSERT(audioheadlen < BODY_HEAD_SIZE);
    
    pAudio->body[nNum].headlen = MMS_PackHeadersInBody(pAudio->body[nNum].pHead,
                        audioheadlen, pAudio->body[nNum].pHead, audioheadlen);
    ASSERT(pAudio->body[nNum].headlen < audioheadlen);

    //audio body
    memcpy(pAudio->body[nNum].pBody, pNode->Metadata, pNode->Metalen);
    pAudio->body[nNum].bodylen = pNode->Metalen;
}
/*********************************************************************\
* Function      MMS_Gb2Utf8    
* Purpose       GB2312 convert to UTF-8
* Params        gbSrc: the string
* Return        the length after converted
**********************************************************************/
static int MMS_Gb2Utf8(char *gbSrc, int txtlen)
{
    char *utf8Dec = NULL;
    int len = 0, utf8len;

    if (gbSrc == NULL)
        return NULL;
    
    utf8len = txtlen*TEXT_CODEMULT;
    utf8Dec = (char*)MMS_malloc(utf8len);
    if (utf8Dec != NULL)
    {
        len = MultiByteToUTF8(0, 0, gbSrc, -1, utf8Dec, utf8len);   
    }
    if (*utf8Dec != '\0')
    {
        if (len > txtlen * TEXT_CODEMULT)
        {
            MMS_free(gbSrc);
            gbSrc = (char *)MMS_malloc(len);
        }
        memcpy(gbSrc, utf8Dec, len);
    }
    MMS_free(utf8Dec);

    return len;
}
/*********************************************************************\
* Function     mms_destroy
* Purpose      When power-off , the interface is called.
* Params       void
* Return       bool
**********************************************************************/
BOOL mms_destroy(void)
{
    return TRUE;
}
/*********************************************************************\
* Function     MMS_ParseAccept
* Purpose      parse the accept data
* Params       pAcceptBuf:  accept data
               bufLen:     the data length
               pAcceptInfo:the structure after parsed
* Return       void
**********************************************************************/
BOOL MMS_ParseDReport(char *pReportBuf, int bufLen, PDREPORT pDreport)
{
    char *token, *tokensrc; 
    int  nFieldNum, nFieldLen, i;
    char szField[SIZE_1];
    char *FieldStr[] = 
    {
        "X-MMS-MESSAGE-TYPE:", "X-MMS-MMS-VERSION:", "MESSAGE-ID:", 
        "TO:", "DATE:", "X-MMS-STATUS:"
    };
    
    if (pReportBuf == NULL || bufLen <= 0)
        return FALSE;
    
    nFieldNum = 6;
    szField[0] = '\0';
    pReportBuf[bufLen] = '\0';
    token = MMS_strtok(pReportBuf, "\r\n");
    while (token != NULL) 
    {
        while(*token == 0x20)
            token++;
        nFieldLen = 0;        
        tokensrc = token;
        while (*token)
        {
            if (*token == ':')
            {
                strncpy(szField, tokensrc, nFieldLen + 1);
                szField[nFieldLen + 1] = '\0';                
                *token++;
                break;
            }
            *token++;
            nFieldLen++;
        }
        
        while(*token == 0x20)
            token++;
        for (i = 0; i < nFieldNum; i++)
        {
            char *ptmp;
            int cplen;
            if (!(*token))
                break;
            if (stricmp(szField, FieldStr[i]) == 0)
            {
                switch(i) 
                {
                case 0:     // "X-MMS-MESSAGE-TYPE:"
                    pDreport->MsgType = atoi(token);
                    break;

                case 1:         // "X-MMS-MMS-VERSION:"                    
                    strcpy(pDreport->Version, token);
                    break;

                case 2:         // "MESSAGE-ID:"
                    strcpy(pDreport->msgId, token);
                    break;               
                                
                case 3:         // "TO:"
                    ptmp  = strstr(token, "/TYPE");
                    if ( ptmp != NULL)
                        cplen = ptmp - token;
                    else
                        cplen = strlen(token);

                    if (cplen >= SIZE_1)
                        cplen = SIZE_1 - 1;

                    strncpy(pDreport->to, token, cplen);
                    pDreport->to[cplen] = '\0';
                    break;

                case 4:         // "DATE:"
                {   
                    TIME_ZONE_INFORMATION TimeZoneInformation;
                    
                    MMS_StrToSysTime(token, &pDreport->getReportTime);
                    GetTimeZoneInformation(&TimeZoneInformation);
                    SystemTimeToTzSpecificLocalTime(&TimeZoneInformation,
                        &pDreport->getReportTime, &pDreport->getReportTime);
                    break;
                }

                case 5:         // "X-MMS-STATUS:"
                    strcpy(pDreport->Status, token);
                    break;

                default:
                    break;
                }//end switch
                break;
            }//end if
        }//end for
        token = MMS_strtok(NULL, "\r\n");   
    }//end while
    return TRUE;
}
/*********************************************************************\
* Function     MMS_OnDeliveryReport
* Purpose      deal with delivery report
* Params       
* Return           
* Remarks      
**********************************************************************/
#if 0
static void MMS_OnDeliveryReport(char *pHeadBuf, int nHeadLen)
{
    char      reportFile[MAX_FILENAME];
    int       hReportFile;
    DREPORT   dReport;
    BOOL      bRet;
    char      pNumber[SIZE_1],*pSdeto;
        
    bRet = MMS_ParseDReport(pHeadBuf, nHeadLen, &dReport);
    if (bRet)
    {       
//        if (!MMSC_IsTotalFull())
        {
            MMS_CreateFileName(reportFile, TYPE_REPORT, MU_REPORT);
            hReportFile = MMS_CreateFile(reportFile, O_CREAT |O_RDWR  );
            if (hReportFile != -1)
            {
                MMS_WriteHead(hReportFile, 0);
                write(hReportFile, &dReport, sizeof(DREPORT));
                MMS_CloseFile(hReportFile);
                //MMSC_ModifyReportUnread(COUNT_ADD, 0);
                //MMSC_ModifyTotal(COUNT_ADD, 0);
            }
            enqueue(&RecvQue, reportFile);
            //mu_newmsg_ring(MU_MDU_MMS);
            pNumber[0] = 0;
                    
            if(strcmp(dReport.Status, "Expired") == 0)
                strcpy(pNumber,"O:");
            else if(strcmp(dReport.Status, "Retrieved") == 0)
                strcpy(pNumber,"S:");
            else if(strcmp(dReport.Status,"Rejected") == 0)
                strcpy(pNumber,"R:");
            else if(strcmp(dReport.Status, "Deferred") == 0)
                strcpy(pNumber,"D:");
            
            if((pSdeto = strstr(dReport.to,"+86")) != NULL)
                pSdeto = pSdeto + 3;
            else
                pSdeto = dReport.to;
            
            if(strlen(pSdeto) > SIZE_1 - 3)
            {
                strncat(pNumber,pSdeto,SIZE_1 - 3);
                pNumber[SIZE_1 - 1] = 0;
            }
            else
            {
                strcat(pNumber,pSdeto);
            }

            mu_newmsg_ring(pNumber);
            mms_newmsg_continue();
        }
    }
}
#endif

static void MMS_OnDeliveryReport(char *pHeadBuf, int nHeadLen)
{
	DREPORT   dReport;
    BOOL      bRet;
    char      pNumber[SIZE_1];
	
    bRet = MMS_ParseDReport(pHeadBuf, nHeadLen, &dReport);
    if (bRet)
    {       
		if(!MMS_UpdateDReport(&dReport))
			return;
		pNumber[0] = 0;
		mu_newmsg_ring(pNumber);
    }

	return;
}

void MMS_SendedFunction(WPARAM wParam,LPARAM lParam)
{
    PRSITEM pRSItem;
    DWORD   handle = 0;
    char    msgID[128];

    pRSItem = GetRSItemByHandle((int)wParam);
    ASSERT(pRSItem != NULL);
    // send failure
    if (HIWORD(lParam) != MMS_SUCCESS)
    {
        if (pRSItem->achFile[MMS_TYPE_POS] != TYPE_READREPLY)
        {
            ModifyMsgStatus(pRSItem->achFile, MU_SEND_FAILURE, MMFT_UNSEND);
        }
        AddRSItemToFailureTable(pRSItem);
        if (pRSItem->achFile[MMS_TYPE_POS] != TYPE_READREPLY)
		{
			char msg[64];
			char *pHead, *pBody;
			int  headLen = 0,bodyLen = 0, nret;

			MMS_GetData(pRSItem->hMms, NULL, &headLen, NULL, &bodyLen);
			pHead = (char*)MMS_malloc(headLen + 1);
			pBody = (char*)MMS_malloc(bodyLen);
			
			nret = MMS_GetData(pRSItem->hMms, pHead, &headLen, pBody, &bodyLen);
			MMS_free(pHead);
			MMS_free(pBody);

			if (HIWORD(lParam) == MMS_ENETERR)
				sprintf(msg, "%s%s","No network.", STR_MMS_SENDFAI);
			// should add invalid phone number
			DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
            MsgWin(NULL, NULL, 0, STR_MMS_SENDFAI, STR_MMS_MMS, Notify_Failure,
			STR_MMS_OK, NULL,MMS_WAITTIME);
		}
    }
    else
    {
		// deal with send success, and the status should be connected
        ASSERT((nConnectState == CS_CONNECTED));
        
        if (pRSItem->achFile[MMS_TYPE_POS] == TYPE_READREPLY)
            MMS_DeleteFile(pRSItem->achFile);
        else
        {               
			DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
            MsgWin(NULL, NULL, 0, STR_MMS_SENDOK, STR_MMS_MMS, Notify_Success,
				STR_MMS_OK, NULL, MMS_WAITTIME);              
            handle = GetHandleByName(pRSItem->achFile);	
			// modify filename for type changing
            MMS_ModifyType(pRSItem->achFile, TYPE_SENDED, MU_SENT);
			if (MsgInfo.nType == MU_OUTBOX)
				SendMessage(MsgInfo.hWnd, PWM_MSG_MU_DELETED_RESP, 
					MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS),
					(LPARAM)handle);
			// notify unibox(send), and modify the msghandle chain
			ModifyMsgStatusByHandle(handle, pRSItem->achFile, MU_SEND_SUCCESS, MMFT_SENDED);
			// notify sent to add a new msg
            AddMmsToSent(pRSItem->achFile, handle);

			MMS_GetOption(pRSItem->hMms, MMS_MSGID, msgID, 127);

			if(mSet.nReportRec != 1)
				MMS_CreateReport(pRSItem->achFile, msgID);
        }
    }
    
    // free the handle of the mms
    MMS_Close(pRSItem->hMms);
    
	MMS_LogWrite(DRT_SEND, pRSItem->size, &pRSItem->begintime);

    if (/*(mSet.homerec == 0) ||*/ IsFailureTableFull() 
        /*|| !StartNewRSItemFromFile(pRSItem))*/
        ||(!GetRSChainNode(pRSItem)&&!GetReadreply(pRSItem)))
    {
		// the failure table is full or there is no msg need to send and receive
        
        // free the item
        FreeRSItem(pRSItem);
        
        // try to disconnect
        TryToDisconnect();
    }
}

BOOL MMS_AcceptFunction( HWND hWnd ,WPARAM wParam)
{   
    BOOL    bDelivery;
    BOOL    bDelay = FALSE;
    char    achFile[MAX_FILENAME];
    
    static char achHeadBuf[MAX_ACCEPT_BUF + 1];
    char* pHeadBuf;
    int nHeadLen;
    int nResult;
    int i = 1000;
    BOOL bConfirmTrue = FALSE;
    
    if (/*(mSet.indexTime == 0) || */(GetMmsRecSet() == MMS_REC_NEVER))      //refuse
    {
        MMSMSGWIN("refuse receive msg", STR_MMS_MMS, MMS_WAITTIME);
		return FALSE;
    }
    
    MMSMSGWIN("receive accept", STR_MMS_MMS, MMS_WAITTIME);
    
//    if (mSet.homerec == 0)        // delay
//        bDelay = TRUE;
    
    pHeadBuf = achHeadBuf;
    nHeadLen = MAX_ACCEPT_BUF;

    nResult = MMS_Accept((int)wParam, pHeadBuf, &nHeadLen, 
        &bDelivery);

    if (nResult != MMS_SUCCESS)
    {
        // MMS_Accept return failue ,then the buffer is not enough
		// need to malloc again and accept again.
        pHeadBuf = MMS_malloc(nHeadLen);
        nResult = MMS_Accept((int)wParam, pHeadBuf, &nHeadLen, 
            &bDelivery);
        ASSERT(nResult == MMS_SUCCESS);
    }
    
    // accept sucessfully
    pHeadBuf[nHeadLen] = 0;
    
    if (bDelivery)
    {
        MMS_OnDeliveryReport(pHeadBuf, nHeadLen);
    }
    else    //new msg
    {                
        if (IsAdAndRecv(pHeadBuf) || IsInFilter(pHeadBuf))
        {
            if (pHeadBuf != achHeadBuf)
                MMS_free(pHeadBuf);
            
            return FALSE;
        }
        
		//        if (MMSC_IsTotalFull())
		//        {
		//            if (pHeadBuf != achHeadBuf)
		//                MMS_free(pHeadBuf);
		//            
		//            return FALSE;
		//        }
		
        if (!SaveHeadBuf(achFile, pHeadBuf, nHeadLen, bDelay))
        {
            if (pHeadBuf != achHeadBuf)
                MMS_free(pHeadBuf);
            
            return FALSE;
        }
        
        //if (MsgInfo.nType == MU_INBOX /*&& mSet.indexTime == MMSSET_MANUAL*/)
        //    NotifyUnibox(achFile, pHeadBuf, nHeadLen);
        // confirm win
		
		MMS_AddPendingReceive(achFile);
		
		if(GetMmsRecSet() != MMS_REC_CONFIRM)
		{
			ReceivePendingMMS(hWnd);
		}
		else
		{
			DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
			
			PLXConfirmWinEx(NULL, hWnd, STR_MMS_RECCONFIRM, Notify_Request, 
				ML("New MMS"), STR_MMS_YES, STR_MMS_NO, WM_CONFIRMREC);
		}
        
	}// end else new information
 
 if (pHeadBuf != achHeadBuf)
     MMS_free(pHeadBuf);
 
 return TRUE;
}

void MMS_ReadFunction(WPARAM wParam ,LPARAM lParam)
{
    
    PRSITEM pRSItem;
    BOOL    bUnRecv = FALSE;
    
    pRSItem = GetRSItemByHandle((int)wParam);
    ASSERT(pRSItem != NULL);
    
    MMSMSGWIN("receive read message", "mms", MMS_WAITTIME);
    
    if (pRSItem->bAutoRecv)
    {
        MMSC_ModifyTotal(COUNT_ADD, 0);
        MMSC_ModifyMsgCount(MU_INBOX, COUNT_ADD, 0);
    }

	MMSMSGWIN("after modify msg count\r\n");
    if (HIWORD(lParam) != MMS_SUCCESS)
    {
        MsgWin(NULL, NULL, 0, STR_MMS_RECVFAI, STR_MMS_MMS,Notify_Failure,
			STR_MMS_OK, NULL, MMS_WAITTIME);
        if(pRSItem->bAutoRecv)
        {
            MMS_ModifyType(pRSItem->achFile, TYPE_UNRECV, -1);
            MMSC_ModifyUnreceive(COUNT_ADD, 0);
            bUnRecv = TRUE;
        }
        else
            ModifyMsgStatus(pRSItem->achFile, MU_RECV_FAILURE, MMFT_UNRECV);
        AddRSItemToFailureTable(pRSItem);
        MMSMSGWIN("after deal with the read failue\r\n");
    }
    else
    {
        ASSERT((nConnectState == CS_CONNECTED));
        
        ASSERT(pRSItem->achFile[0] != '\0');
        
        if (!MMS_OnRead(pRSItem))
        {
			MMSMSGWIN("mms_onread failue\r\n");
            if(pRSItem->bAutoRecv)
            {
                MMS_ModifyType(pRSItem->achFile, TYPE_UNRECV, -1);
                MMSC_ModifyUnreceive(COUNT_ADD, 0);
                bUnRecv = TRUE;
            }
            else
                ModifyMsgStatus(pRSItem->achFile, MU_RECV_FAILURE, MMFT_UNRECV);
            AddRSItemToFailureTable(pRSItem);
			MMSMSGWIN("after mms_onread failue\r\n");
        }
        else
        {
			DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
			
            //EmptyFailureTable();
            if(!pRSItem->bAutoRecv)
                ModifyMsgStatus(pRSItem->achFile, MU_RECV_SUCCESS, MMFT_UNREAD);
			MMSMSGWIN("mms_onread success\r\n");
        }
        
    }
	printf("pRSItem->bAutoRecv = %d\r\n", pRSItem->bAutoRecv);

    if ( pRSItem->bAutoRecv)
    {
        if(bUnRecv)
		{
			printf("calling NotifyProgram\r\n");
            NotifyProgram(pRSItem->achFile);
		}
        else
		{
			printf("calling NotifyProgramForSuc\r\n");
            NotifyProgramForSuc(pRSItem->achFile);
		}
    }
    
    MMS_Close((int)wParam);
    MMSMSGWIN("befor log write\r\n");
	MMS_LogWrite(DRT_RECIEVE, pRSItem->size, &pRSItem->begintime);
    MMSMSGWIN("befor log write\r\n");
    if ( /*(mSet.homerec == 0) ||*/ IsFailureTableFull() 
        /*|| !StartNewRSItemFromFile(pRSItem))*/
        ||(!GetRSChainNode(pRSItem)&&!GetReadreply(pRSItem)))
    {
        FreeRSItem(pRSItem);
        
        TryToDisconnect();
    }
	MMSMSGWIN("mms read function end\r\n");
}

void MMS_SendReadreply()
{   
    static char     readreply[SIZE_1];
    char        replyName[MAX_FILENAME];
    static TotalMMS    totalmms;
    mmsMetaNode        mmsNode;
    
    memset(&totalmms, 0, sizeof(TotalMMS));
    memset(&mmsNode, 0, sizeof(mmsMetaNode));

    totalmms.mms.slide[0].pText = &mmsNode;    
    totalmms.pAttatch = &mmsNode;
    //wsphead
    strcpy(totalmms.mms.wspHead.to, wspheadbuf.from);
    sprintf(totalmms.mms.wspHead.subject, "Read:%s", wspheadbuf.subject);
    totalmms.mms.wspHead.Priority = 2;
    totalmms.mms.wspHead.nSlidnum = 1;
    GetLocalTime(&totalmms.mms.wspHead.date);
    //slide
    totalmms.mms.slide[0].dur = 2000;
    totalmms.mms.slide[0].pText->txtcharset = CHARSET_UTF8;
    totalmms.mms.slide[0].pText->Content_Location = My_StrDup("reply.txt");
    sprintf(readreply, STR_MMS_REPLYMMS,
        wspheadbuf.to, wspheadbuf.date.wYear, wspheadbuf.date.wMonth, 
        wspheadbuf.date.wDay, wspheadbuf.date.wHour, wspheadbuf.date.wMinute);
    totalmms.mms.wspHead.nMmsSize = strlen(readreply);
    CalTxtSize(readreply, &totalmms.mms.slide[0].txtSize);
    //create a file
    MMS_CreateFileName(replyName, TYPE_READREPLY, MU_REPORT);//???xlzhu
//  SetCurrentDirectory(FLASHPATH);
//  hfile = MMS_CreateFile(replyName, CREATE_NEW|ACCESS_WRITE|ACCESS_READ, 0);
//  MMS_WriteHead(hfile, 0);
//  WriteFile(hfile, &totalmms.mms, sizeof(MultiMMS));
//  WriteFile(hfile, readreply, totalmms.mms.wspHead.nMmsSize);
//  MMS_CloseFile(hfile);
    totalmms.mms.slide[0].pText->MetaType = META_TEXT_PLAIN;
    totalmms.mms.slide[0].pText->Metalen = totalmms.mms.wspHead.nMmsSize;
    mmsNode.Metadata = (char*)MMS_malloc(totalmms.mms.slide[0].pText->Metalen + 1);
    strcpy(mmsNode.Metadata, readreply);
    PackMmsBody(replyName, totalmms);
    MMS_free(mmsNode.Metadata);
    MMSPro_Send(replyName);
    
}

BOOL MMS_TimerFunction(HWND hWnd ,WPARAM wParam)
{
    
    PRSITEM pRSItem;
    
    if (wParam == HANGUP_TIMER_ID)
    {
        int nRet;
        
        ASSERT(nConnectState == CS_WAITINGHANGUP);
        
        nConnectState = CS_HANGUPING;
        
    	MMS_GprsCount();
        nRet = DIALMGR_HangUp(hWnd);
		printf("DIALMGR_HangUp nRet = %d\r\n", nRet);
        if (nRet != DIALER_REQ_ACCEPT)
            PostMessage(hwndProtocol, MSG_DIAL_RETURN, 0,
            (LPARAM)DIALER_RUN_NET_ABNORMAL);
        
        KillTimer(hWnd, HANGUP_TIMER_ID);
        
        return FALSE;
    }
    
    ASSERT(wParam == 1);
    
    if (nConnectState == CS_IDLE)
    {
    
//        if (mSet.homerec == 0)
//            return FALSE;
        
        ASSERT(IsRSItemTableEmpty());
        ASSERT(!IsFailureTableFull());
        
        pRSItem = AllocRSItem();
        
        if (!GetNewRSItemFromFile(pRSItem))
        {
            FreeRSItem(pRSItem);
            return FALSE;
        }
        
        if (!StartDialing())
        {
            FreeRSItem(pRSItem);
            return FALSE;
        }
        
        nConnectState = CS_DIALING;
#ifndef _SMARTPHONE
        SetFileTransferSleepFlag(FALSE);
#endif
    }
    else if (nConnectState == CS_WAITINGHANGUP)
    {
        pRSItem = AllocRSItem();
        
        if (!GetNewRSItemFromFile(pRSItem))
        {
            FreeRSItem(pRSItem);
            return FALSE;
        }
        
        KillTimer(hWnd, HANGUP_TIMER_ID);
        
        nConnectState = CS_CONNECTING;
        
        MMS_Connect(mSet.szIp, MMS_PORT);
    }

    return TRUE;
}

#define MAX_NOTIFICATION_SIZE  500 //bytes
/*no use in btp-04*/
int Space_Notification(void)
{
    int ntotalnum = 0;
    int nSize = 0;

    ntotalnum = mms_get_total_count();
    if (ntotalnum > MAX_MSG_NUM)
        ntotalnum = MAX_MSG_NUM;
    nSize = (MAX_MSG_NUM - ntotalnum) * MAX_NOTIFICATION_SIZE;
    if (nSize == 0)
        return 0;
    else
        return (nSize/1024 + 1);    
}

//
static int GetMmsRecSet(void)
{
    if (ROAMING == PM_GetNetWorkStatus())
        return mSet.visitrec;
    else if (HOMEZONE == PM_GetNetWorkStatus())
        return mSet.homerec;
    else
        return MMS_REC_NEVER;  //never
}

//int Log_Write (LOGRECORD* plogrec)
static BOOL MMS_LogWrite(int direction, int size, SYSTEMTIME * pBegintime)
{
	SYSTEMTIME  EndTime;
	LOGRECORD * plogrec;
	void      * ptmprec;
	unsigned long begintm;
	int len, apnlen;

	GetLocalTime(&EndTime);
	apnlen = strlen(mSet.ISPPhoneNum1);
	len = sizeof(LOGRECORD) + apnlen;
	ptmprec = MMS_malloc(len+1);
	memset(ptmprec, 0, len +1);
	plogrec = (LOGRECORD *)ptmprec;
	plogrec->direction = direction;
	LOG_STtoFT(pBegintime, &begintm);
		
	LOG_STtoFT(&EndTime, &plogrec->u.gprs.endtm);
	if (DRT_SEND == plogrec->direction)
		plogrec->u.gprs.sendcounter = size;
	else
		plogrec->u.gprs.recievecounter = size;

	plogrec->u.gprs.duration = plogrec->u.gprs.endtm - begintm;

	plogrec->begintm = begintm;
	if(mSet.DtType == 1)
		plogrec->type = TYPE_GPRS;
	else
		plogrec->type = TYPE_DATA;
	
	plogrec->index = 0;
    strncpy(plogrec->APN, mSet.ISPPhoneNum1, apnlen);
	plogrec->APN[apnlen] = 0;
	if (-1 == Log_Write( plogrec )) 
	{
		MMS_free(ptmprec );
		return FALSE;
	}
	else 
		MMS_free(ptmprec);
	return TRUE;
}
// GPRSCounter should be called after hangup 
extern int Get_MAC_Handle();
extern int MAC_GetOption	(int, unsigned, char*, int);
extern int AddGPRSData(unsigned long senddata, unsigned long receivedata);

static void MMS_GprsCount(void)
{
#define OPT_GPRS_TXFLOW		22		/* GPRS send flowdata */
#define OPT_GPRS_RXFLOW		23		/* GPRS receive flowdata */

	unsigned long gprs_tx;
	unsigned long gprs_rx;
	int           machandle;

	machandle = Get_MAC_Handle();
	MAC_GetOption(machandle, OPT_GPRS_TXFLOW, (char *)&gprs_tx, sizeof(long));
	MAC_GetOption(machandle, OPT_GPRS_RXFLOW, (char *)&gprs_rx, sizeof(long));
		
	AddGPRSData(gprs_tx, gprs_rx);
}

static BOOL MMS_CreateReport(char* pSendedFile, char*  msgID)
{
	DREPORT   dReport;
	int hFile;
	WSPHead wsphead;
	char reportFile [MAX_FILENAME];
	int handle;
	MU_MsgNode msgnode;

	chdir(FLASHPATH);
	hFile = MMS_CreateFile(pSendedFile, O_RDONLY);
	if (hFile == -1)
		return FALSE;

	lseek(hFile, MMS_HEADER_SIZE, SEEK_SET);
	read(hFile, &wsphead, sizeof(WSPHead));
	MMS_CloseFile(hFile);

	memset(&dReport, 0, sizeof(DREPORT));

	//create a pending report
	MMS_CreateFileName(reportFile, TYPE_REPORTING, MU_REPORT);

	strcpy(dReport.fileName, reportFile);
	strcpy(dReport.msgId, msgID);
	strcpy(dReport.to, wsphead.to);
	memcpy(&(dReport.getReportTime), &(wsphead.date), sizeof(SYSTEMTIME));
	strcpy(dReport.Status, STR_MMS_PENDING);
	hFile = MMS_CreateFile(reportFile, O_CREAT|O_RDWR);
	if(hFile == -1)
	{
		MMSC_ModifyTotal(COUNT_DEC, 0);
	
		MMSC_ModifyMsgCount(MU_REPORT, COUNT_DEC, 0);

		return FALSE;
	}
	MMS_WriteHead(hFile, 0);
	lseek(hFile, MMS_HEADER_SIZE, SEEK_SET);
	write(hFile, &dReport, sizeof(DREPORT));
	MMS_CloseFile(hFile);
	handle = AllocMsgHandle(reportFile, MMFT_REPORTING, FALSE);
	
	if(MsgInfo.nType == MU_REPORT)
	{
		FillMsgNode(&msgnode, MU_MSG_MMS_DELIVER_REPORT, MMFT_REPORTING, 
                dReport.getReportTime, handle, STR_MMS_PENDING, 
                dReport.to, MU_STORE_IN_FLASH);
		
		SendMessage(MsgInfo.hWnd, PWM_MSG_MU_NEWMTMSG, 
			MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), (LPARAM)&msgnode);
	}
	return TRUE;
}

static BOOL MMS_UpdateDReport(PDREPORT  pReport)
{
	char  reportFile[256];
	int hFile;
//	DREPORT dReport;
	int handle;
	MU_MsgNode msgnode;
	
	chdir(FLASHPATH);
	if(!MMS_GetPendingDReport(reportFile, pReport->msgId))
		return FALSE;

	//has a pending DReport
	hFile = MMS_CreateFile(reportFile, O_RDWR);
	if(hFile == -1)
		return FALSE;

	if(strcmp(pReport->Status, "Retrieved") == 0)
	{
		//success
		strcpy(pReport->Status, STR_MMS_DELIVERED);
	}
	else
	{
		//failed
		strcpy(pReport->Status, STR_MMS_FAILED);
	}

	lseek(hFile, MMS_HEADER_SIZE, SEEK_SET);
	write(hFile, pReport, sizeof(DREPORT));
	MMS_CloseFile(hFile);
	
	handle = GetHandleByName(reportFile);

	MMS_ModifyType(reportFile, TYPE_REPORTED, -1);
	
	ModifyMsgNode(handle, reportFile, MMFT_REPORTED);

	if(MsgInfo.nType == MU_REPORT)
	{
		FillMsgNode(&msgnode, MU_MSG_MMS_DELIVER_REPORT, MMFT_REPORTED, 
                pReport->getReportTime, handle, STR_MMS_DELIVERED, 
                pReport->to, MU_STORE_IN_FLASH);
		
		SendMessage(MsgInfo.hWnd, PWM_MSG_MU_MODIFIED, 
			MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), (LPARAM)&msgnode);
	}

	return TRUE;
}

static BOOL MMS_AddPendingReceive(const char* mmsPendingFile)
{
	PMMSPENDINGREC pNew;

	pNew = malloc(sizeof(MMSPENDINGREC));

	if(pNew == NULL)
		return FALSE;
	
	strcpy(pNew->pFileName, mmsPendingFile);

	pNew->pNext = pPendingRecHead;

	pPendingRecHead = pNew;
	
	return TRUE;
}

static BOOL MMS_GetFirstPendingReceive(PMMSPENDINGREC pGet, BOOL bRemove)
{
	PMMSPENDINGREC pTemp;

	if(pPendingRecHead == NULL)
		return FALSE;

	pTemp = pPendingRecHead;
	memcpy(pGet, pTemp, sizeof(MMSPENDINGREC));

	if(bRemove)
	{
		pPendingRecHead = pPendingRecHead->pNext;
		free(pTemp);
	}

	return TRUE;
}

static BOOL ReceivePendingMMS(HWND hWnd)
{
	PRSITEM pRSItem;
	MMSPENDINGREC pendingRec;
	BOOL    bDelay = FALSE;
	int hFile, nHeadLen;
	char    achHeadBuf[MAX_ACCEPT_BUF + 1];
	int nRet;
	int handle;

	//    if (mSet.homerec == 0)        // delay
	//        bDelay = TRUE;

	MMS_GetFirstPendingReceive(&pendingRec, TRUE);

	chdir(FLASHPATH);
    hFile = MMS_CreateFile(pendingRec.pFileName, O_RDONLY);
    if (hFile == -1)
        return FALSE;

    nHeadLen = MMS_GetFileSize(pendingRec.pFileName) - sizeof(SYSTEMTIME);
    read(hFile, (PVOID)achHeadBuf, nHeadLen);
	MMS_CloseFile(hFile);

	handle = GetHandleByName(pendingRec.pFileName);

	pRSItem = AllocRSItem();
	if (pRSItem)
	{
		pRSItem->bAutoRecv = TRUE;
		
		strcpy(pRSItem->achFile, pendingRec.pFileName );                    
		pRSItem->bDelay = bDelay;
		
		if (nConnectState == CS_CONNECTED)
		{
			int report = !mSet.nReportSend;
			
			GetLocalTime(&pRSItem->begintime);
			
			pRSItem->hMms = MMS_Open(0, MMS_RECV);
			ASSERT(pRSItem->hMms != MMS_FAILURE);
			
			MMS_SetOption(pRSItem->hMms, MMS_ALWREPORT, &report, sizeof(int));
			nRet = MMS_Recv(pRSItem->hMms, achHeadBuf, nHeadLen, bDelay);
			if (nRet != MMS_SUCCESS || bDelay)
			{			
				MMS_Close(pRSItem->hMms);
				
				FreeRSItem(pRSItem);
				
				//if (MsgInfo.nType == MU_INBOX /*&&mSet.indexTime == MMSSET_AUTO*/)
				{
					MMSC_ModifyTotal(COUNT_ADD, 0);
					MMSC_ModifyMsgCount(MU_INBOX, COUNT_ADD, 0);                
					MMS_ModifyType(pendingRec.pFileName, TYPE_UNRECV,-1);
					if(handle)
						ModifyMsgNode(handle, NULL, MMFT_UNRECV);
					MMSC_ModifyUnreceive(COUNT_ADD, 0);
					NotifyProgram(pendingRec.pFileName);
				}
				
				TryToDisconnect();
				
				return FALSE;
			}
			pRSItem->nState = RSS_RECVING;
			
			ModifyMsgStatus(pRSItem->achFile, MU_RECV_START, MMFT_RECVING);
		}
		else
		{
			if (nConnectState == CS_IDLE)
			{
				
				if (!StartDialing())
				{
					FreeRSItem(pRSItem);

					MsgWin(NULL, NULL, 0, STR_MMS_RECVFAI, STR_MMS_MMS, Notify_Failure,
					STR_MMS_OK, NULL,MMS_WAITTIME);

					//if (MsgInfo.nType == MU_INBOX /*&&mSet.indexTime == MMSSET_AUTO*/)
					{
						MMSC_ModifyTotal(COUNT_ADD, 0);
						MMSC_ModifyMsgCount(MU_INBOX, COUNT_ADD, 0);                
						MMS_ModifyType(pendingRec.pFileName, TYPE_UNRECV, -1);
						if(handle)
							ModifyMsgNode(handle, NULL, MMFT_UNRECV);
						MMSC_ModifyUnreceive(COUNT_ADD, 0);
						NotifyProgram(pendingRec.pFileName);
					}
					return FALSE;
				}
				
				nConnectState = CS_DIALING;
#ifndef _SMARTPHONE
				SetFileTransferSleepFlag(FALSE);
#endif
			}// end if idle
			else if (nConnectState == CS_WAITINGHANGUP)
			{
				nConnectState = CS_CONNECTING;
				KillTimer(hWnd, HANGUP_TIMER_ID);
				MMS_Connect(mSet.szIp, MMS_PORT);
			}
			
			pRSItem->nState = RSS_WAITINGRECV;
			ModifyMsgStatus(pRSItem->achFile, MU_RECV_START, MMFT_RECVING);
		}//end else
	}// end pRSItem
	else
	{
		InsertRSChainNode(pendingRec.pFileName,RSS_WAITINGRECV, TRUE);
	}

	return TRUE;
}
