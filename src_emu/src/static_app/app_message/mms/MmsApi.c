/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS
 *
 * Purpose  : the interface between MMS and Unibox
 *            
\**************************************************************************/

#include "MmsGlobal.h"
#include "MmsInterface.h"

//////////////////////////////////////////////////////////////////////
#define MAX_FILETYPE		5
//outbox
static int OutType[MAX_FILETYPE] = 
        { MMFT_UNSEND, MMFT_SENDING, MMFT_DELAYSEND, -1, -1};

//sentbox
static int SentType[MAX_FILETYPE] = 
        {MMFT_SENDED, -1, -1, -1, -1};

//inbox  read、unread, delay
static int InType[MAX_FILETYPE] = 
        { MMFT_READED, MMFT_UNREAD, MMFT_DELAYRECV, MMFT_UNRECV,MMFT_SENDED};

//draft  outboxt->draftbox
static int DraftType[MAX_FILETYPE] = 
        { MMFT_DRAFT, MMFT_UNSEND, MMFT_DELAYSEND, -1,-1};

//Report
static int ReportType[MAX_FILETYPE] = 
        { MMFT_REPORTED, MMFT_REPORTING, MMFT_UNREPORT, -1,-1};

//My folder
static int MyFolder[MAX_FILETYPE] = 
        { MMFT_READED, MMFT_UNREAD, MMFT_SENDED, -1, -1};

static int  nCurFolder = -1;
static int  nMmsNums;
static MMSCOUNT g_MmsCount;
PMSGHANDLENAME pMsgHandleHead = NULL, pMsgHandleEnd = NULL;
PFolderInfo pFolderHead  =NULL;

static void mms_init_handlechain(void);
static int GetMmftType(char* pszFileName);
static int TypeMMSMatch(char* filename , int nCurFolder);
static BOOL	MMS_InitFolder(void);
static BOOL	IsLegalFolderID(nFolder);
static PFolderInfo MMS_GetFolderInfoByID(int nFolderId);
static void  CALLBACK timerProc(HWND , UINT , UINT , DWORD);

void MMS_FreeFolderInfo();
void MMSC_ModifyFolderUnread(int nFolder, int nsign, int nCount);

BOOL mms_init(HWND hwnd);
BOOL mms_get_messages(HWND hwndmu, int folderid);
BOOL mms_get_one_message(MU_MsgNode *msgnode);
BOOL mms_release_messages(HWND hwndmu);
BOOL mms_read_message(HWND hwndmu, DWORD msghandle, BOOL bPre, BOOL bNext);
BOOL mms_delete_message(HWND hwndmu, DWORD msghandle);
BOOL mms_delete_all_messages(HWND hwndmu, int folderid, MU_DELALL_TYPE ntype);
BOOL mms_delete_multi_messages(HWND hwndmu , DWORD handle[],int nCount);
BOOL mms_new_message(HWND hwndmu);
BOOL mms_setting(HWND hwndmu);
BOOL mms_get_detail(HWND hwndmu,DWORD handle);
BOOL mms_newmsg_continue(void);
BOOL mms_forward(HWND hwndmu, DWORD handle);
BOOL mms_reply_from(HWND hwndmu, DWORD handle);
BOOL mms_reply_all(HWND hwndmu, DWORD handle);
BOOL mms_get_number(HWND hwndmu, DWORD handle);
BOOL mms_resend_message(HWND hwndmu, DWORD handle);
BOOL mms_suspend_message(HWND hwndmu, DWORD handle);
BOOL mms_move_message(HWND hwndmu, DWORD handle , int nFolder);
BOOL mms_copy_message(HWND hwndmu, DWORD handle , int nFolder);
BOOL mms_get_count(int folder,int* pnunread,int* pncount);
BOOL mms_new_folder(int folder);  
BOOL MMSC_AutoSend (void);
extern BOOL mms_destroy(void);
extern int	MMS_GetFileFolder(const char *pFileName);
extern void ModifyMsgStatus(const char *pFileName, UINT nType, UINT nStatus);
extern BOOL DeleteRSChainNodeFromName(const char* pName);
extern void FreeMeta (mmsMetaNode *pMeta);
BOOL MMS_GetDiskInfo(STORAGEINFO* storageinfo);

static MU_IMessage mu_mms = 
{
	mms_init,
    mms_get_messages,
    mms_get_one_message,
    mms_release_messages,
    mms_read_message,
    mms_delete_message,
	mms_delete_multi_messages,
    mms_delete_all_messages,
    mms_new_message,
    mms_setting,
    mms_get_detail,
    mms_newmsg_continue,    
    mms_forward,
    mms_reply_from,
    mms_reply_all,
    mms_get_number,
    mms_resend_message,
	mms_suspend_message,
	mms_move_message,
	mms_copy_message,
    mms_destroy,
	mms_get_count,
    mms_new_folder
};

static void CALLBACK timerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	KillTimer(NULL, idEvent);
	
	if(GetSIMState() != 0)
		MMSC_AutoSend();
}

BOOL mms_init(HWND hwndmu)
{
	//MMS_CopyFile("/rom/$a0003052941b.ms$", "/mnt/flash/mms/$A0003052941b.ms$");
	//MMS_CopyFile("/rom/mn.bmp", "/mnt/flash/picture/mn.bmp");
	
	if (!MMSPro_Initial())
		return FALSE;

	mms_init_handlechain();

	SetTimer(NULL, 1, 4000, timerProc);

    return TRUE;
}

/******************************************************************** 
* Function	   mms_init_handlechain
* Purpose      initialize the msghandle chain*
* Params	   void
* Return	   BOOL
**********************************************************************/
static void mms_init_handlechain(void)
{
	struct dirent  *dirinfo = NULL;
	struct DIR		*dirtemp = NULL;
	int     mmftType = 0;
	
	dirtemp = opendir(MMS_FILEPATH);
	if(dirtemp == NULL)
		return;	
	do 
	{
		dirinfo = readdir(dirtemp);
		if (NULL == dirinfo)
			break;
		mmftType = GetMmftType(dirinfo->d_name);
		if (-1 == mmftType)
			continue;
		AllocMsgHandle(dirinfo->d_name, mmftType, FALSE);
	} while(dirinfo);

	closedir(dirtemp);
}
/******************************************************************** 
* Function	   mms_get_messages
*
* Purpose      It is an interface for one message type. Get all messages, 
               which stored in a special folder, such as inbox, outbox, or
               sent.
*
* Params	   hwndmu: the handle of one of Unibox
               folderid: the id of the folder
*
* Return	   BOOL
**********************************************************************/
static PMSGHANDLENAME  pCurNode = NULL;
BOOL mms_get_messages(HWND hwndmu, int folderid)
{
    nCurFolder = folderid;
       
    MMSMSGWIN("Enter mms_get_message, folderid = %d\r\n", folderid);
	//List window message
    MMSPro_RegisterMsg(hwndmu, PWM_MSG_MU_DAEMON, folderid);

	//display item
    PostMessage(hwndmu, PWM_MSG_MU_GETMSGS_RESP, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), 0);

	pCurNode = pMsgHandleHead;

    return TRUE;
}

/******************************************************************** 
* Function	   mms_get_one_message
*
* Purpose      The message module calls this function repeatly until it
               returns FALSE.The message information is stored in the inputting
               parameter. The message module should insert the message into
               the list control
* Params	   msgnode: the struct of mms info
*
* Return	   BOOL
**********************************************************************/
BOOL mms_get_one_message(MU_MsgNode *pmsgnode)
{
    int				iStatus,buflen;
    char			*acceptBuf;
    int 			hFile;
    FILETIME        lastWritetime;
    ACCEPTINFO      AcceptInfo;
//	WSPHead			wspHead;
    int				storeType = MU_STORE_IN_FLASH;
    NAMENODE		*head = NULL, *end = NULL;
    NAMENODE		*p;
	PMSGHANDLENAME  pNodeNext = NULL;
//	char*			subject[MU_SUBJECT_DIS_LEN + 1];

	chdir(MMS_FILEPATH);

    while(pCurNode)
    {
		iStatus = pCurNode->nStatus;
        // 找到新的
        if (nCurFolder != MMS_GetFileFolder(pCurNode->msgname) || (-1 == iStatus))
		{
			pCurNode = pCurNode->pNext;
			continue;
		}
        
        MMSMSGWIN("find a mms file ,iStatus = %d\r\n", iStatus);
        //协议处理新收的彩信时，只有在彩信正文接收成功或失败的时候
        //才通知unibox新增加一条彩信，所以，对于正在接收的彩信通知，
        //不返回给unibox
//        if(iStatus == MMFT_RECVING)
//             continue;
        
        if ( MMFT_FIRSTAOTORECV == MMS_GetFileMmft(pCurNode->msgname))
		{
			pCurNode = pCurNode->pNext;
            continue;
		}
    
        hFile = open(pCurNode->msgname, O_RDONLY);
		if(hFile == -1)
		{
			printf("mms open file error!!!");
			goto error_handle;
		}

        // 未接收的彩信
        if (iStatus == MMFT_UNRECV || iStatus == MMFT_DELAYRECV || iStatus == MMFT_RECVING)
        {		
            memset(&AcceptInfo, 0, sizeof(ACCEPTINFO));
            MMS_GetFileTime(pCurNode->msgname, &lastWritetime);
            MMS_FTtoST(&lastWritetime, &AcceptInfo.acceptTime);
            buflen = MMS_GetFileSize(pCurNode->msgname) - sizeof(SYSTEMTIME);
            acceptBuf = (char*)MMS_malloc(buflen + 1);
            read(hFile, acceptBuf, buflen);
            close(hFile);

            MMS_ParseAccept(acceptBuf, buflen, &AcceptInfo);
            MMS_free(acceptBuf);
            AcceptInfo.infoType = INFO_ACCEPT;
            
            FillMsgNode(pmsgnode, MU_MSG_MMS, iStatus, AcceptInfo.acceptTime, 
                (DWORD)pCurNode, AcceptInfo.Subject, AcceptInfo.From, storeType);            
        }
        // Delivery report
        else if (iStatus == MMFT_REPORTED
			|| iStatus == MMFT_REPORTING
			|| iStatus == MMFT_UNREPORT/*|| iStatus == MMFT_REPORTED*/)
        {
            DREPORT   dReport;

            if (!MMS_IsMyfile(hFile))
            {
                close(hFile);
                p = (NAMENODE*)MMS_malloc(sizeof(NAMENODE));
                strcpy(p->filename, pCurNode->msgname);
                if (head == NULL)
                    head = p;
                else
                    end->pNext = p;
                end = p;
                if (end != NULL)
                    end->pNext = NULL;
				pNodeNext = pCurNode->pNext;
				FreeMsgHandle((DWORD)pCurNode);
				pCurNode = pNodeNext;
                continue;
            }
            lseek(hFile, MMS_HEADER_SIZE, SEEK_SET);
            read(hFile, &dReport, sizeof(DREPORT));
            close(hFile);
            FillMsgNode(pmsgnode, MU_MSG_MMS_DELIVER_REPORT, iStatus, 
                dReport.getReportTime, (DWORD)pCurNode, STR_MMS_REPORT, 
                dReport.to, storeType);
        }
        // else
        else
        {
			char* pSubject = NULL;

            if (!MMS_IsMyfile(hFile))
            {
                close(hFile);
                p = (NAMENODE*)MMS_malloc(sizeof(NAMENODE));
                strcpy(p->filename, pCurNode->msgname);
                if (head == NULL)
                    head = p;
                else
                    end->pNext = p;
                end = p;
                if (end != NULL)
                    end->pNext = NULL;  

				pNodeNext = pCurNode->pNext;
				FreeMsgHandle((DWORD)pCurNode);
				pCurNode = pNodeNext;
                continue;
            }
            
            //lseek(hFile, MMS_HEADER_SIZE, SEEK_SET);
            //read(hFile, &wspHead, sizeof(WSPHead));
            close(hFile);            

			{
				TotalMMS tmms;
//				int i;

				ReadAndParse(pCurNode->msgname, &tmms);
				
				if(tmms.mms.wspHead.subject[0] == 0)
				{
#if 0
					for(i=0; i<tmms.mms.wspHead.nSlidnum; i++)
					{
						if(tmms.mms.slide[i].pText != NULL && tmms.mms.slide[i].pText->Metadata)
						{
							pSubject = tmms.mms.slide[i].pText->Metadata;
							break;
						}
					}
					
					if(pSubject != NULL)
						strncpy(tmms.mms.wspHead.subject, pSubject, 30);
					
					tmms.mms.wspHead.subject[30] = 0;
#endif
					if(tmms.mms.slide[0].pText != NULL && tmms.mms.slide[0].pText->Metadata)
					{
						strncpy(tmms.mms.wspHead.subject, tmms.mms.slide[0].pText->Metadata, 30);
						tmms.mms.wspHead.subject[30] = 0;
					}

				}
				
			
				if (nCurFolder == MU_INBOX)
				{
					if(MMS_GetFileType(pCurNode->msgname) == TYPE_SENDED)
						FillMsgNode(pmsgnode, MU_MSG_MMS, iStatus, tmms.mms.wspHead.date, (DWORD)pCurNode,
						tmms.mms.wspHead.subject, tmms.mms.wspHead.to, storeType);
					else
						FillMsgNode(pmsgnode, MU_MSG_MMS, iStatus, tmms.mms.wspHead.date, (DWORD)pCurNode,
						tmms.mms.wspHead.subject, tmms.mms.wspHead.from, storeType);
				}
				else
				{
					int nType;

					nType = MMS_GetFileType(pCurNode->msgname);
					
					if( nType == TYPE_READED || nType == TYPE_UNREAD || nType == TYPE_UNRECV)
						FillMsgNode(pmsgnode, MU_MSG_MMS, iStatus, tmms.mms.wspHead.date, (DWORD)pCurNode,
						tmms.mms.wspHead.subject, tmms.mms.wspHead.from, storeType);
					else
						FillMsgNode(pmsgnode, MU_MSG_MMS, iStatus, tmms.mms.wspHead.date, (DWORD)pCurNode,
						tmms.mms.wspHead.subject, tmms.mms.wspHead.to, storeType);
				}

				FreeMeta(tmms.pAttatch);

			}
        }
		pCurNode = pCurNode->pNext;
        return TRUE;
    }

error_handle:
    while (head != NULL) 
    {
        p = head;
        head = p->pNext;
        unlink(p->filename);
        MMS_free(p);        
    }

    storeType = MU_STORE_IN_FLASH;
	pCurNode = NULL;
    return FALSE;
}

/******************************************************************** 
* Function	   GetMsgType
* Purpose      get the message type from the status
* Params	   int istatus
* Return	   message type
**********************************************************************/
int GetMsgType(int iStatus)
{
    int    msgType;

    switch(iStatus) 
    {
    case MMFT_UNSEND:
        msgType = MU_STU_UNSENT;
    	break;
    case MMFT_SENDING:
        msgType = MU_STU_SENDING;
		break;
	case MMFT_WAITINGSEND:
		msgType = MU_STU_WAITINGSEND;
		break;
	case MMFT_SUSPEND:
		msgType = MU_STU_SUSPENDED;
		break;
//	case MMFT_DELAYRECV :
//		msgType = MU_STU_DELAY;
//		break;
//	case MMFT_DELAYSEND:
//		msgType =MU_STU_DELAY;//??
//		break;
    case MMFT_UNREAD:
        msgType = MU_STU_UNREAD;
        break;
    case MMFT_READED:
        msgType = MU_STU_READ;
        break;
    case MMFT_UNRECV:
        msgType = MU_STU_UNRECEIVED;
        break;
    case MMFT_RECVING:
        msgType = MU_STU_RECEIVING;
        break;
    case MMFT_DRAFT:
        msgType = MU_STU_DRAFT;
        break;
    case MMFT_SENDED:
        msgType = MU_STU_SENT;
        break;
		
    case MMFT_REPORTED:
        msgType = MU_STU_CONFIRM;
        break;

	case MMFT_REPORTING:
		msgType = MU_STU_WAITING;
		break;

	case MMFT_UNREPORT:
		msgType = MU_STU_FAILED;
		break;

    default:
        msgType = -1;
        break;
    }
    return msgType;
}

// alloc a msg handle from message name
// when 1.create a new mms and saved,
//      2.recieve a new mms  call this function        
DWORD AllocMsgHandle(const char *filename, int iStatus, BOOL isRom)
{
    PMSGHANDLENAME p = NULL,pTemp = NULL;

	p = MMS_malloc(sizeof(MSGHANDLENAME));

	if(p == NULL)
		return NULL;    

	memset(p, 0, sizeof(MSGHANDLENAME));

	p->isRom= isRom;
	strcpy(p->msgname, filename);
	p->nStatus = iStatus;
	p->pNext = NULL;
	p->pPre = NULL;
	
	if(!pMsgHandleHead)
		pMsgHandleEnd = pMsgHandleHead = p;
	else
	{
		p->pPre = pMsgHandleEnd;
		pMsgHandleEnd->pNext = p;
		pMsgHandleEnd = p;
	}

    return (DWORD)p;
}

void FreeMsgHandle(DWORD msghandle)
{
    PMSGHANDLENAME pMsgHandle = NULL, pPre = NULL;
	
	pMsgHandle = (PMSGHANDLENAME)msghandle;

	pPre = pMsgHandle->pPre;
	//if pmsghandle == pMsghandlehead
	if (pPre == NULL)
	{
		pMsgHandleHead = pMsgHandle->pNext;
		if (pMsgHandleHead)
			pMsgHandleHead->pPre = NULL;
	}
	else if (pMsgHandle->pNext == NULL)
	{
		pMsgHandleEnd = pMsgHandle->pPre;
		if (pMsgHandleEnd)
			pMsgHandleEnd->pNext = NULL;
	}
	else
	{
		pPre->pNext = pMsgHandle->pNext;
		if (pMsgHandle->pNext)
			pMsgHandle->pNext->pPre = pMsgHandle->pPre;
	}

    pMsgHandle->msgname[0] = '\0';
    pMsgHandle->nStatus = 0;
    pMsgHandle->pNext = NULL;
	pMsgHandle->pPre = NULL;
    pMsgHandle->isRom = FALSE;
	MMS_free(pMsgHandle);
    return ;
}

/******************************************************************** 
* Function	   GetNodeByHandle
* Purpose      Get a node point from handle
* Params	   msghandle: 一个句柄
* Return	   一个节点
**********************************************************************/
PMSGHANDLENAME GetNodeByHandle(DWORD msghandle)
{
	PMSGHANDLENAME pMsgHandle = NULL;
		
	pMsgHandle = (PMSGHANDLENAME)msghandle;

    return pMsgHandle;
}

/******************************************************************** 
* Function	   GetHandleByName
* Purpose      get a file name by handle
* Params	   filename
* Return	   a msghandlename node
**********************************************************************/
DWORD GetHandleByName(const char *filename)
{	
	PMSGHANDLENAME pMsgHandle = NULL,pTemp =NULL;

	pTemp = pMsgHandleHead;

	while(pTemp)
	{
		if(pTemp->msgname[0] != 0 && stricmp(pTemp->msgname, filename) == 0)
		{
			return (DWORD)pTemp;
		}
		pTemp = pTemp->pNext;
	}
	
    return (DWORD)pTemp;
}

/******************************************************************** 
* Function	   ModifyMsgNode
* Purpose      modify the content of the node
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void ModifyMsgNode(DWORD msghandle, const char* filename, int iStatus)
{
	PMSGHANDLENAME pMsgHandle = NULL;

	pMsgHandle = (PMSGHANDLENAME)msghandle;
	
	if(pMsgHandle != NULL)
	{
		if (NULL != filename)
			strcpy(pMsgHandle->msgname, filename);
		if (iStatus != -1)
			pMsgHandle->nStatus = iStatus;
	}
}
/******************************************************************** 
* Function	   GetCurFolder
* Purpose      get current folder id
* Params	   void
* Return	 	int
**********************************************************************/
int GetCurFolder(void)
{
    return nCurFolder;
}

/******************************************************************** 
* Function	   msg_release_messages
* Purpose      After calling msg_get_messages(…) , sub modules perhaps
               MMS_malloc memory for the messages. If message unibox module 
               does not need these dynamic information , call the interface, 
               and sub module can MMS_free malloced memory.
* Params	   hwndmu: the handle the Unibox
* Return	   Bool
**********************************************************************/
BOOL mms_release_messages(HWND hwndmu)
{
    MMSPro_UnRegisterMsg();
    return TRUE;
}

/******************************************************************** 
* Function	   mms_read_message
* Purpose      It is an interface. Read a special message according to the
               message id which is set as a input parameter. Note that the 
               status of the message has been changed after the operation, 
               so if the original status of the message is UNREAD, it is 
               necessary for the sub module to send a modified message to 
               Message module to update the status of the message.
* Params	   hwndmu: the handle of the message Unibox
* Return	   BOOL
**********************************************************************/
BOOL mms_read_message(HWND hwndmu, DWORD msghandle, BOOL bPre, BOOL bNext)
{
    ACCEPTINFO  headInfo;
    WSPHead     wsphead;
    int        hFile;
    DREPORT     dReport;
    FILETIME  lastWritetime;
    int         buflen;
    char        *acceptBuf;
    int         iStatus;
    PMSGHANDLENAME  pMsghandle;

    pMsghandle = GetNodeByHandle(msghandle);

    if (nCurFolder == MU_DRAFT)
    {
        CallMMSEditWnd(pMsghandle->msgname, NULL, hwndmu,
            msghandle, MMS_CALLEDIT_OPEN, NULL, NULL);

        return TRUE;
    }
        
    memset(&headInfo, 0, sizeof(ACCEPTINFO));
    memset(&wsphead, 0, sizeof(WSPHead));
    
    // 收件箱，发件箱，已发信息
	chdir(MMS_FILEPATH);

    hFile = open(pMsghandle->msgname, O_RDONLY);
    if (-1 == hFile)
    {
        MMSMSGWIN("open file failure", STR_MMS_MMS, MMS_WAITTIME);
        return ERR_NOTFOUND;
    }
    // 判断是否mms文件
    iStatus = pMsghandle->nStatus;

    if (iStatus == MMFT_UNRECV || iStatus == MMFT_DELAYRECV || iStatus == MMFT_RECVING)
    {
        // Null
    }
    else
    {
        if (!MMS_IsMyfile(hFile))
        {
            close(hFile);
            return ERR_NOTMMS;
        }
    }
    
    if (iStatus == MMFT_REPORTED
		|| iStatus == MMFT_REPORTING
		|| iStatus == MMFT_UNREPORT/*|| iStatus == MMFT_REPORTED*/)
    {        
        lseek(hFile, MMS_HEADER_SIZE, SEEK_SET);
        read(hFile, &dReport, sizeof(DREPORT));
        close(hFile);
        
        headInfo.acceptTime = dReport.getReportTime;
        headInfo.MsgType = dReport.MsgType;
        strcpy(headInfo.To, dReport.to);
        strcpy(headInfo.Version, dReport.Version);
        // 以结构的tranid存储report的msgid
        strcpy(headInfo.TranId, dReport.msgId);
        // 以结构的msgclass存储report的status
        strcpy(headInfo.MsgClass, dReport.Status);
        headInfo.infoType = INFO_REPORT;

        CallDisplayInfo(MuGetFrame(), hwndmu, msghandle, headInfo, bPre, bNext);
    }
    else if (iStatus == MMFT_UNRECV || iStatus == MMFT_DELAYRECV || iStatus == MMFT_RECVING)
    {
        if (iStatus != MMFT_RECVING)
        {
		   
            MMS_GetFileTime(pMsghandle->msgname, &lastWritetime);
            MMS_FTtoST(&lastWritetime, &headInfo.acceptTime);
            buflen = MMS_GetFileSize(pMsghandle->msgname) - sizeof(SYSTEMTIME);
            acceptBuf = (char*)MMS_malloc(buflen + 1);
			read(hFile, acceptBuf, buflen);

            close(hFile);

            MMS_ParseAccept(acceptBuf, buflen, &headInfo);
            MMS_free(acceptBuf);
            headInfo.infoType = INFO_RETRIEVE;
			CallDisplayInfo(MuGetFrame(), hwndmu, msghandle, headInfo, bPre, bNext);
        }
    }
    else
    {
        close(hFile);
        MMS_ReadMsg(hwndmu, pMsghandle, bPre, bNext);
    }
    return TRUE;
}
/******************************************************************** 
* Function	   msg_delete_message
* Purpose      Delete a special message ideitified by the handle parameter.
* Params	   hwndmu: the handle of one of Unibox
               msghandle: the handle of the msg
* Return	   BOOL
**********************************************************************/
BOOL mms_delete_message(HWND hwndmu, DWORD msghandle)
{
    PMSGHANDLENAME  pMsghandle;

    pMsghandle = GetNodeByHandle(msghandle);

    if(pMsghandle == NULL)
        return FALSE;
    
   	chdir(MMS_FILEPATH);
    
    if (pMsghandle->nStatus == MMFT_RECVING || pMsghandle->nStatus == MMFT_SENDING)
        return FALSE;
    
    if (MMS_DeleteFile(pMsghandle->msgname))
    {
        SendMessage(hwndmu, PWM_MSG_MU_DELETED_RESP, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS),
        (LPARAM)msghandle);

        FreeMsgHandle(msghandle);        
        return TRUE;
    }
    else
        return FALSE;    
}
/******************************************************************** 
* Function	   mms_delete_all_messages
* Purpose      Delete all messages stored in inbox/draft/outbox/sent. 
* Params	   hwndmu: the handle of one of Unibox
               folderid: the id of the folder
* Return	   BOOL   
* Remarks	   
**********************************************************************/
BOOL mms_delete_all_messages(HWND hwndmu, int folderid, MU_DELALL_TYPE ntype)
{
    PMSGHANDLENAME pTemp =NULL;
	PFolderInfo p = NULL, pFolderInfo = NULL, pPrev = NULL;
	int nFolder;
	PMSGHANDLENAME  pNodeNext = NULL;
	
	if(pMsgHandleHead == NULL)
		return FALSE;

	if(pFolderHead == NULL)
		return FALSE;
	
	chdir(MMS_FILEPATH);

	pTemp = pMsgHandleHead;

	p = pFolderHead;

	while(pTemp)
	{
		nFolder =MMS_GetFileFolder(pTemp->msgname);

		if (nFolder == folderid)
		{
			MMS_DeleteFile(pTemp->msgname);
			pNodeNext = pTemp->pNext;
			FreeMsgHandle((DWORD)pTemp);
			pTemp = pNodeNext;
		}
		else
			pTemp = pTemp->pNext;
	}

	if(ntype == MU_DELALL_FOLDER)
	{
		while(p)
		{
			if(p->nFolderID == folderid)
			{
				pFolderInfo = p;
				if(pPrev)
					pPrev->pNext = p->pNext;
				else
					pFolderHead = p->pNext;
				
				MMS_free(pFolderInfo);
				break;
			}
			pPrev = p;
			p= p->pNext;			
		}
	}
	   
    SendMessage(hwndmu, PWM_MSG_MU_DELALL_RESP, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), 0);
    return TRUE;
}
/******************************************************************** 
* Function	   mms_delete_multi_messages
* Purpose      Delete multi messages that selected by user stored in inbox/draft/outbox/sent. 
* Params	   hwndmu: the handle of one of Unibox
               folderid: the id of the folder
* Return	   BOOL   
* Remarks	   
**********************************************************************/
BOOL mms_delete_multi_messages(HWND hwndmu , DWORD handle[],int nCount)
{

	PMSGHANDLENAME pHead =NULL, pTemp =NULL;
	PMSGHANDLENAME  pNodeNext = NULL;
	int i ;
	
	if(handle == 0 ||pMsgHandleHead == NULL)
		return FALSE;
	
	chdir(MMS_FILEPATH);
	
	pTemp = pMsgHandleHead;
	pHead = (PMSGHANDLENAME)handle;

	for(i = 0; i < nCount; i++,pHead++)
	{
		pTemp = pMsgHandleHead;

		while(pTemp)
		{
			if(pTemp == pHead)
			{
				MMS_DeleteFile(pTemp->msgname);
				pNodeNext = pTemp->pNext;
				FreeMsgHandle((DWORD)pTemp);
				pTemp = pNodeNext;
				break;
			}
			else
				pTemp = pTemp->pNext;
		}
	}

	SendMessage(hwndmu, PWM_MSG_MU_DELMULTI_RESP, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), 0);
	return FALSE;
}
/******************************************************************** 
* Function	   mms_new_message
* Purpose      Write a new message.
* Params	   hwndmu: the handle of one of Unibox
* Return	   BOOL
**********************************************************************/
BOOL mms_new_message(HWND hwndmu)
{
    BOOL bRet;

    bRet = CallMMSEditWnd(NULL,NULL, hwndmu, 0, MMS_CALLEDIT_NEW, NULL, NULL);
    return bRet;
}

/******************************************************************** 
* Function	   mms_forward
* Purpose      forward mms
**********************************************************************/
BOOL mms_forward(HWND hwndmu, DWORD handle)
{
    PMSGHANDLENAME  pMsghandle;

	char		    szFilePath[MAX_PATH_LEN];
    
	unsigned int    sfMmsSize;
	int   	hFile;
	struct stat *buf = NULL;

	sfMmsSize  = 0;
		
    pMsghandle = GetNodeByHandle(handle);
	
	memset(szFilePath, 0x0, MAX_PATH_LEN);
	sprintf(szFilePath, "%s%s", MMS_FILEPATH, pMsghandle->msgname);

    hFile = open(szFilePath, O_RDONLY);
    if (-1 == hFile)
    {
        return FALSE;
    }
    //判断是否mms文件
    if (!MMS_IsMyfile(hFile))
    {
        close(hFile);
        return FALSE;
    }
	
	buf = MMS_malloc(sizeof(struct stat));
	
	if(buf == NULL)
	{
		close(hFile);
		return NULL;
	}
	
	memset(buf, 0, sizeof(struct stat));
	stat(szFilePath, buf);
	
	sfMmsSize = buf->st_size;
	
	MMS_free(buf);
	buf = NULL;

	close(hFile);
		
	if(sfMmsSize >= MAX_MMS_LEN)
	{		
		PLXTipsWin(MuGetFrame(), NULL, 0, STR_MMS_OVERSIZE,STR_MMS_MMS, Notify_Alert, NULL,
			NULL, WAITTIMEOUT);
		return FALSE;
	}
    pMsghandle = GetNodeByHandle(handle);

    CallMMSEditWnd(pMsghandle->msgname, NULL, hwndmu, 
                0, MMS_CALLEDIT_TRAN, NULL, NULL);

    return TRUE;
}

/******************************************************************** 
* Function	   mms_reply_from
* Purpose      reply 
**********************************************************************/
BOOL mms_reply_from(HWND hwndmu, DWORD handle)
{
    PMSGHANDLENAME  pMsghandle;

    pMsghandle = GetNodeByHandle(handle);
    
    if (pMsghandle->nStatus == MMFT_UNRECV)
        return CallMMSEditWnd(pMsghandle->msgname,NULL, NULL, 0, 
			NOTI_CALLEDIT_REPLY, NULL, NULL);
    else
        return CallMMSEditWnd(pMsghandle->msgname, NULL, NULL, 0, 
			MMS_CALLEDIT_REPLY, NULL, NULL);
}

/******************************************************************** 
* Function	   mms_reply_all
* Purpose      reply all
**********************************************************************/
BOOL mms_reply_all(HWND hwndmu, DWORD handle)
{
    PMSGHANDLENAME  pMsghandle;

    pMsghandle = GetNodeByHandle(handle);
    
    if (pMsghandle->nStatus == MMFT_UNRECV)
        return CallMMSEditWnd(pMsghandle->msgname,NULL, NULL, 0, 
			NOTI_CALLEDIT_REPLY, NULL, NULL);
	else
		return CallMMSEditWnd(pMsghandle->msgname, NULL, NULL, 0, 
			MMS_CALLEDIT_REPLYALL, NULL, NULL);
}

/******************************************************************** 
* Function	   mms_get_number
* Purpose      save number of who sent mms
**********************************************************************/
BOOL mms_get_number(HWND hwndmu, DWORD handle)
{
    PMSGHANDLENAME  pMsghandle;
    int            hFileMms;
    WSPHead         wHead;
	int             buflen;
	char            *acceptBuf;
	ACCEPTINFO      aInfo;
	char            *from;

    pMsghandle = GetNodeByHandle(handle);

    chdir(MMS_FILEPATH);
    hFileMms = open(pMsghandle->msgname, O_RDONLY);
    if (hFileMms == -1)
        return FALSE;

	if (pMsghandle->nStatus == MMFT_UNRECV || pMsghandle->nStatus == MMFT_DELAYRECV
		|| pMsghandle->nStatus == MMFT_RECVING)
	{
		buflen = MMS_GetFileSize(pMsghandle->msgname) - 2 * sizeof(SYSTEMTIME);
		acceptBuf = (char*)MMS_malloc(buflen + 1);
		read(hFileMms, acceptBuf, buflen);
		close(hFileMms);
		
		MMS_ParseAccept(acceptBuf, buflen, &aInfo);
		MMS_free(acceptBuf);
		from = aInfo.From;
	}
	else
	{		
		if (!MMS_IsMyfile(hFileMms))
		{
			close(hFileMms);
			return FALSE;
		}
		lseek(hFileMms, MMS_HEADER_SIZE, SEEK_SET);
		read(hFileMms, &wHead, sizeof(WSPHead));
		close(hFileMms);
		from = wHead.from;
	}

    return MMS_SaveAddrToPB(hwndmu, from);
}
/******************************************************************** 
* Function	   mms_setting
* Purpose      Set the properties of a special message type
* Params	   void
* Return	   BOOL
**********************************************************************/
BOOL mms_setting(HWND hwndmu)
{
    return MMS_CreateSettingWnd(MuGetFrame(), hwndmu);
}
/******************************************************************** 
* Function	   mms_get_storageinfo
* Purpose      Get the storage information of a special message type.
* Params	   hwndmu: the handle of one of Unibox
* Return	   BOOL
**********************************************************************/
BOOL mms_get_storageinfo(HWND hwndmu)
{
    STORAGEINFO    storageinfo;
    int		        hFile;
    int            nType;
	struct dirent  *dirinfo = NULL;
	DIR  *dirtemp = NULL;

    storageinfo.nCount = 0;
    storageinfo.nTotalSize = 0;

    chdir(MMS_FILEPATH);
    dirtemp = opendir(MMS_FILEPATH);
	if(dirtemp == NULL)
		return FALSE;
	
	dirinfo = readdir(dirtemp);
    if (dirinfo != NULL && dirinfo->d_name[0] != 0)
    {
        do 
        {
            hFile = open(dirinfo->d_name, O_RDONLY);
            if (hFile == -1)
                break;

            nType = MMS_GetFileType(dirinfo->d_name);
            if (nType != TYPE_UNRECV  && nType != TYPE_DELAYRECV)
            {
                if (MMS_IsMyfile(hFile))
                {
                    if (nType != TYPE_READREPLY)
                    {
                        storageinfo.nCount ++;
                        storageinfo.nTotalSize += MMS_GetFileSize(dirinfo->d_name);
                    }
                }
            }
            else
            {
                storageinfo.nCount ++;
                storageinfo.nTotalSize += MMS_GetFileSize(dirinfo->d_name);
            }
            close(hFile);
        	
        } while(dirinfo = readdir(dirtemp));        
    }

    closedir(dirtemp);

    storageinfo.nTotalSize = storageinfo.nTotalSize/1024;

    if (storageinfo.nTotalSize == 0 && storageinfo.nCount > 0)
        storageinfo.nTotalSize = 1;
    
  /*
    SendMessage(hwndmu, PWM_MSG_MU_STORAGEINFO_RESP, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS),
          (LPARAM)&storageinfo);*/
    return TRUE;
}
/******************************************************************** 
* Function	   mms_get_detail
* Purpose      Get the information of a special message.
* Params	   hwndmu: the handle of one of Unibox
* Return	   BOOL
**********************************************************************/
BOOL mms_get_detail(HWND hwndmu,DWORD handle)
{
    ACCEPTINFO  headInfo;
    WSPHead     wsphead;
    int         hFile;
    FILETIME   lastWritetime;
    int         buflen;
    char        *acceptBuf;
    int         iStatus;
    PMSGHANDLENAME  pMsghandle;
	
    pMsghandle = GetNodeByHandle(handle);

    memset(&headInfo, 0, sizeof(ACCEPTINFO));
    memset(&wsphead, 0, sizeof(WSPHead));
    
    // 收件箱，发件箱，已发信息,draft
	chdir(MMS_FILEPATH);
	
    hFile = open(pMsghandle->msgname, O_RDONLY);
    if (-1 == hFile)
    {
        MMSMSGWIN("open file failure", STR_MMS_MMS, MMS_WAITTIME);
        return ERR_NOTFOUND;
    }
    // 判断是否mms文件
    iStatus = pMsghandle->nStatus;
	
    if (iStatus == MMFT_UNRECV || iStatus == MMFT_DELAYRECV || iStatus == MMFT_RECVING)
    {
        // Null
    }
    else
    {
        if (!MMS_IsMyfile(hFile))
        {
            close(hFile);
            return ERR_NOTMMS;
        }
    }
	
   if (iStatus == MMFT_UNRECV || iStatus == MMFT_DELAYRECV || iStatus == MMFT_RECVING)
    {
        if (iStatus != MMFT_RECVING)
        {
			
            MMS_GetFileTime(pMsghandle->msgname, &lastWritetime);
            MMS_FTtoST(&lastWritetime, &headInfo.acceptTime);
            buflen = MMS_GetFileSize(pMsghandle->msgname) - sizeof(SYSTEMTIME);
            acceptBuf = (char*)MMS_malloc(buflen + 1);
			read(hFile, acceptBuf, buflen);

            MMS_ParseAccept(acceptBuf, buflen, &headInfo);
            MMS_free(acceptBuf);
            headInfo.infoType = INFO_ACCEPT;
        }
    }
	else
    {        
        lseek(hFile, MMS_HEADER_SIZE, SEEK_SET);
        read(hFile, &wsphead, sizeof(WSPHead));
        
        headInfo.acceptTime = wsphead.date;
        strcpy(headInfo.From, wsphead.from);
        strcpy(headInfo.To, wsphead.to);
        strcpy(headInfo.Subject, wsphead.subject);
		headInfo.MsgSize = wsphead.nMmsSize;
        //itoa(wsphead.nMmsSize, headInfo.MsgSize, 10);
		if (iStatus == MMFT_SENDED)
			headInfo.infoType = INFO_SENT;
		else if (iStatus == MMFT_UNREAD || iStatus == MMFT_READED)
			headInfo.infoType = INFO_RECEIVE;
        else
			headInfo.infoType = INFO_HEAD;
	}
	
	close(hFile);
	CallDisplayInfo(MuGetFrame(), hwndmu, handle, headInfo, FALSE, FALSE);
	
    return TRUE;
}
/******************************************************************** 
* Function	   mms_newmsg_continue
* Purpose      After a new message is accepted by a sub module, then the
               sub module call mu_newmsg_ring(…) . Later message module 
               should call the callback function to notice the sub module 
               to continue this new message(for example, insert it into a 
               linkage table).
* Params	   void
* Return	   BOOL
**********************************************************************/
BOOL mms_newmsg_continue(void)
{
    char  filename[MAX_FILENAME];
    MU_MsgNode msgnode;
    int     hFile;
    WSPHead wspHead;
    int      nType;
    DWORD      msghandle;
//    DREPORT   dReport; 
    
    GetQueFront(filename);

    nType = MMS_GetFileMmft(filename);

	chdir(MMS_FILEPATH);

    hFile = open(filename, O_RDONLY);
	if(hFile == -1)
	{
		printf("mms open file error!!!!!!!!!!!");
		return FALSE;
	}
	
    if (!MMS_IsMyfile(hFile))
    {
        close(hFile);
        return FALSE;
    }    
    lseek(hFile, MMS_HEADER_SIZE, SEEK_SET);

//    if (nType == MMFT_REPORT)
//    {	
//        msghandle = AllocMsgHandle(filename, nType, FALSE);
//        if (msghandle == NULL)
//        {
//            MsgWin(NULL, NULL, 0, STR_MMS_OVERNUM, STR_MMS_MMS, Notify_Alert,
//				STR_MMS_OK, NULL, MMS_WAITTIME);
//            return FALSE;
//        }
//		if (nCurFolder == MU_REPORT)
//		{
//			read(hFile, &dReport, sizeof(DREPORT));
//			close(hFile);
//			
//			FillMsgNode(&msgnode, MU_MSG_MMS_DELIVER_REPORT, nType, 
//				dReport.getReportTime, msghandle, STR_MMS_REPORT, 
//				dReport.to, MU_STORE_IN_FLASH);
//			SendMessage(MMSPro_GetHandle(), PWM_MSG_MU_NEWMTMSG, 
//				MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), (LPARAM)&msgnode);
//		}
//     }
    if (nType == MMFT_UNREAD && nCurFolder == MU_INBOX)
    {
        read(hFile, &wspHead, sizeof(WSPHead));
        close(hFile);
        
        msghandle = GetHandleByName(filename);
        
        FillMsgNode(&msgnode, MU_MSG_MMS, MMFT_UNREAD, wspHead.date, msghandle, 
            wspHead.subject, wspHead.from, MU_STORE_IN_FLASH);

        SendMessage(MMSPro_GetHandle(), PWM_MSG_MU_MODIFIED, 
            MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), (LPARAM)&msgnode);        
    }

	close(hFile);
    return TRUE;
}

/**************************************************************************** 
* Funcation: TypeMMSMatch
*
* Purpose  : 类型匹配
*           
\*****************************************************************************/
static int TypeMMSMatch(char* pszFileName, int nCurFolder)
{
	int  nType, nFolder;
	PFolderInfo pFolderInfo = NULL;

    nType = MMS_GetFileMmft(pszFileName);
    
    if(nType == MMFT_FIRSTAOTORECV)
    {
        if((MMSPro_IsInRSTable(pszFileName) || MMSPro_IsInRSChain(pszFileName)))
            return nType;

        MMS_ModifyType(pszFileName, TYPE_UNRECV, -1);
        nType = MMFT_UNRECV;
    }

	nFolder = MMS_GetFileFolder(pszFileName);

	if(nFolder != nCurFolder)
		return -1;
	if(!IsLegalFolderID(nFolder))
		return -1;
	
	if ((nType == MMFT_UNSEND) && (MMSPro_IsInRSTable(pszFileName) ||
		MMSPro_IsInRSChain(pszFileName)))
		nType = MMFT_SENDING;
	
	else if ((nType == MMFT_UNRECV || nType == MMFT_DELAYRECV) 
		&& (MMSPro_IsInRSTable(pszFileName) || MMSPro_IsInRSChain(pszFileName)))
		nType = MMFT_RECVING;
	
	return nType;
	
}
/******************************************************************** 
* Function	   GetMmftType
* Params	   pszFileName:mms file name               
* Return	   mmft type
**********************************************************************/
static int GetMmftType(char* pszFileName)
{
	int  nType, nFolder;
	PFolderInfo pFolderInfo = NULL;

    nType = MMS_GetFileMmft(pszFileName);
    
    if(nType == MMFT_FIRSTAOTORECV)
    {
        if((MMSPro_IsInRSTable(pszFileName) || MMSPro_IsInRSChain(pszFileName)))
            return nType;

        MMS_ModifyType(pszFileName, TYPE_UNRECV, -1);
        nType = MMFT_UNRECV;
    }

	nFolder = MMS_GetFileFolder(pszFileName);
	
	if(!IsLegalFolderID(nFolder))
		return -1;
	
	if ((nType == MMFT_UNSEND) && (MMSPro_IsInRSTable(pszFileName) ||
		MMSPro_IsInRSChain(pszFileName)))
		nType = MMFT_SENDING;
	
	else if ((nType == MMFT_UNRECV || nType == MMFT_DELAYRECV) 
		&& (MMSPro_IsInRSTable(pszFileName) || MMSPro_IsInRSChain(pszFileName)))
		nType = MMFT_RECVING;
	
	return nType;	
}

/******************************************************************** 
* Function	   fillMsgNode
* Purpose      fill the structrue of msgnode
* Params	   MU_MsgNode* pmsgnode:the structure
               BYTE msgtype: MU_MSG_MMS, MU_MSG_MMS_DELIVER_REPORT
	           int iStatus:  MMFT_
	           SYSTEMTIME date
	           int handle
	           PCSTR subject
	           PCSTR address
* Return	   BOOL
**********************************************************************/
BOOL FillMsgNode(MU_MsgNode* pmsgnode, BYTE msgtype, int iStatus, SYSTEMTIME date, 
                 DWORD  handle, PCSTR subject, PCSTR address, int storeType)
{
    FILETIME  tmpFTime;
    int       subLen, len;    
#ifdef MMS_18030
    int       subWidth;
    int       nFit = 0;
#endif
    char      dupSub[SIZE_5];
    //char      dupAddr[SIZE_1];
    //char      *addrtok;

    if (pmsgnode == NULL)
        return FALSE;

    if (subject == NULL || address == NULL)
        return FALSE;

    pmsgnode->msgtype = msgtype;
    pmsgnode->status = GetMsgType(iStatus);
    pmsgnode->storage_type = storeType;
    MMS_STtoFT(&date, &tmpFTime);
    pmsgnode->maskdate = tmpFTime.dwLowDateTime;
    pmsgnode->handle = handle;

    subLen = strlen(subject);
	
    if (subLen >= MU_SUBJECT_DIS_LEN)
    {
#ifndef MMS_18030
        strncpy(dupSub, subject, MU_SUBJECT_DIS_LEN);
        dupSub[MU_SUBJECT_DIS_LEN] = '\0';
        MMS_EditTitle(dupSub, MU_SUBJECT_DIS_LEN);
        strcpy(pmsgnode->subject, dupSub);
#else
        subWidth = ENG_FONT_WIDTH * MU_SUBJECT_DIS_LEN;
        GetTextExtentExPoint(NULL, subject, subLen, subWidth, &nFit, NULL, NULL);
        strncpy(pmsgnode->subject, subject, nFit);
        pmsgnode->subject[nFit] = '\0';
#endif
    }
    else
        strcpy(pmsgnode->subject, subject);
    
	subLen = UTF8ToMultiByte(CP_ACP, 0, pmsgnode->subject, -1, pmsgnode->subject, subLen, NULL, NULL);
	pmsgnode->subject[subLen] = 0;

	pmsgnode->addr[0] = 0;
    /*strcpy(dupAddr, address);
    addrtok = MMS_chrtok(dupAddr, MMS_ASEPCHR, MMS_ASEPCHRF);

    if (addrtok != NULL)
    {
        if (strlen(addrtok) >= MU_ADDR_MAX_LEN)
        {
            strncpy(pmsgnode->addr, addrtok, MU_ADDR_MAX_LEN);
            pmsgnode->addr[MU_ADDR_MAX_LEN] = '\0';
        }
        else
            strcpy(pmsgnode->addr, addrtok);
}*/
	if(strlen(address) > MU_ADDR_MAX_LEN)
	{
		strncpy(pmsgnode->addr, address,MU_ADDR_MAX_LEN);
		pmsgnode->addr[MU_ADDR_MAX_LEN] = 0;
		len = UTF8ToMultiByte(CP_ACP, 0, pmsgnode->addr, -1, pmsgnode->addr, MU_ADDR_MAX_LEN - 1, NULL, NULL);
		pmsgnode->addr[len] = 0;
	}
	else
	{
		len = UTF8ToMultiByte(CP_ACP, 0, address, -1, pmsgnode->addr, MU_ADDR_MAX_LEN - 1, NULL, NULL);
		//strcpy(pmsgnode->addr, address);
		pmsgnode->addr[len] = 0;
	}

    return TRUE;
}

BOOL mms_register(MU_IMessage **Imsg)
{
    *Imsg = &mu_mms;

    return TRUE;
}


BOOL mms_resend_message(HWND hwndmu, DWORD handle)
{
    PMSGHANDLENAME pmsgNode;

    pmsgNode = GetNodeByHandle(handle);

    if (GetMEIsSim() == 0)
    {
        MsgWin(NULL, NULL, 0, STR_MMS_NOSIM, STR_MMS_MMS, Notify_Alert, 
			STR_MMS_OK, NULL,	MMS_WAITTIME);
        return FALSE;
    }
    
    MMSPro_Send(pmsgNode->msgname);
    
    return TRUE;
}

BOOL mms_can_forward(DWORD handle)
{
	PMSGHANDLENAME pmsgNode;
	TotalMMS       mms;
	mmsMetaNode    *p;

	memset(&mms, 0, sizeof(TotalMMS));
    pmsgNode = GetNodeByHandle(handle);

	ReadAndParse(pmsgNode->msgname, &mms);

	p = mms.pAttatch;
	// not shown if the msg contains ringring tone??,MIDI or java.
	// this kind of MMS msg can not be forwarded.
	while (p)
	{
		if (p->MetaType == META_OTHER)
			return FALSE;
		p = p->pnext;
	}
	
	FreeMeta(mms.pAttatch);
	return TRUE;
}
// the interface for other apps
BOOL APP_EditMMS(HWND hWndFrame, HWND hWndPre, UINT msg, UINT nType,
                    PSTR  InsertFileName)
{
    return CallMMSEditWnd(NULL, hWndFrame, hWndPre, msg, nType, InsertFileName, NULL);
}


/*This function has been changed to return byte, not kbyte*/
BOOL MMS_GetDiskInfo(STORAGEINFO* storageinfo)
{
    int            hFile;
    int            nType;
	struct dirent *dirinfo = NULL;
	DIR  *dirtemp = NULL;
	
    
    storageinfo->nCount = 0;
    storageinfo->nTotalSize = 0;
    
    chdir(MMS_FILEPATH);
	dirtemp = opendir(MMS_FILEPATH);
	if(dirtemp == NULL)
		return FALSE;	

	dirinfo = readdir(dirtemp);
	
    if (dirinfo && dirinfo->d_name[0])
    {
        do 
        {
            hFile = open(dirinfo->d_name, O_RDONLY);
            if (hFile == -1)
                break;
            
            nType = MMS_GetFileType(dirinfo->d_name);
            if (nType != TYPE_UNRECV  && nType != TYPE_DELAYRECV)
            {
                if (MMS_IsMyfile(hFile))
                {
                    if (nType != TYPE_READREPLY)
                    {
                        storageinfo->nCount ++;
                        storageinfo->nTotalSize += MMS_GetFileSize(dirinfo->d_name);
                    }
                }
            }
            else
            {
                storageinfo->nCount ++;
                storageinfo->nTotalSize += MMS_GetFileSize(dirinfo->d_name);
            }
            close(hFile);
            
        } while(dirinfo = readdir(dirtemp));        
    }
    
    closedir(dirtemp);
    
	
//    storageinfo->nTotalSize = storageinfo->nTotalSize/1024;
    
//    if (storageinfo->nTotalSize == 0 && storageinfo->nCount > 0)
//        storageinfo->nTotalSize = 1;

    return TRUE;
}
/******************************************************************** 
* Function	   APP_GetMMSSize
* Purpose      Get the storage information
* Return	   int:nSize
**********************************************************************/
int APP_GetMMSSize(void)
{
    STORAGEINFO    storageinfo;

    MMS_GetDiskInfo(&storageinfo);
   
    return storageinfo.nTotalSize;
}
/* **************************************************************************\
                count process
/* **************************************************************************/

/*********************************************************************\
* Function     MMSC_InitCount
* Purpose      search all mms
* Params       void
* Return       void
**********************************************************************/
void MMSC_InitCount(void)
{

    int    hFileMms;
    int    nType, nFolder;
    NAMENODE  *head = NULL, *end = NULL;
    NAMENODE  *p;
    BOOL   ret = TRUE ;
//     BOOL bFavorite, bDelete;
	struct dirent *dirinfo = NULL;
	DIR *dirtemp = NULL;
	PFolderInfo pTemp =NULL;

	//init folder
	MMS_InitFolder();

    memset(&g_MmsCount, 0, sizeof(MMSCOUNT));

    chdir(FLASHPATH);

	dirtemp = opendir(FLASHPATH);
	if(dirtemp == NULL)
		return;	

	dirinfo = readdir(dirtemp);
    
    if (!dirinfo || dirinfo->d_name[0] == 0)
	{
		closedir(dirtemp);
        return;
	}

    do 
    {       
        nType = dirinfo->d_name[MMS_TYPE_POS];

		if(nType == TYPE_FIRSTAOTORECV)
			continue;
		
		nFolder = MMS_GetFileFolder(dirinfo->d_name);

        hFileMms = MMS_CreateFile(dirinfo->d_name, O_RDONLY);
        if (hFileMms == -1 || !ret 
            || nType == TYPE_READREPLY)
        {
            p = (NAMENODE*)MMS_malloc(sizeof(NAMENODE));
            strcpy(p->filename, dirinfo->d_name);
            if (head == NULL)
                head = p;
            else
                end->pNext = p;
            end = p;
            if (end != NULL)
                end->pNext = NULL;

            MMS_CloseFile(hFileMms);
            continue;
        }
        // judge the file head
        if (nType != TYPE_UNRECV  && nType != TYPE_DELAYRECV)
        {
            if (!MMS_IsMyfile(hFileMms))
            {
                MMS_CloseFile(hFileMms);
                continue;
            }
        }
        MMS_CloseFile(hFileMms);
//        //统计是否为收藏
//          bFavorite = (fd.cFileName[MMS_FAV_POS] == MMS_HAS_FAVORITE) ? TRUE : FALSE;
//        //是否已经删除，仅存在于收藏夹中
//        bDelete = (fd.cFileName[MMS_REF_POS] == MMS_HAS_DELETE) ? TRUE : FALSE;

//        if(bFavorite)
//            g_MmsCount.nFavorite++;
     
        // readreply should not be counted
        if(nType != TYPE_READREPLY)
            g_MmsCount.nTotalnum++;
		
		pTemp = MMS_GetFolderInfoByID(nFolder);
		
		if(pTemp != NULL)
		{
			pTemp->nMmsNum++;	
		}
        
        switch(nType) 
        {
        case TYPE_UNREAD:
            g_MmsCount.nUnread++;
			pTemp->nUnreadNum++;
        case TYPE_READED:
        case TYPE_UNRECV:
        case TYPE_DELAYRECV:
        case TYPE_RECVING:
            if (nType == TYPE_UNRECV || nType == TYPE_DELAYRECV)
                g_MmsCount.nUnreceive++;    
            break;
        default:
            break;        
        }
    } while(dirinfo = readdir(dirtemp));
	
    closedir(dirtemp);

    while (head != NULL) 
    {
        p = head;
        head = p->pNext;
        unlink(p->filename);
        MMS_free(p);
    }

    MMS_Setindbar(g_MmsCount.nUnread, 0, 0, 0, g_MmsCount.nTotalnum);
}

/*********************************************************************\
* Function     MMSC_IsTotalFull
* Purpose      judge wether the mms reach the max number
* Params       void
* Return       bool
* remarks      no use in btp-04
**********************************************************************/
BOOL MMSC_IsTotalFull(void)
{
    if (g_MmsCount.nTotalnum >= MAX_MSG_NUM)
    {
        MsgWin(NULL, NULL, 0, STR_MMS_OVERNUM, STR_MMS_MMS, Notify_Alert,
			 STR_MMS_OK, NULL,WAITTIMEOUT);
        return TRUE;
    }
    else
        return FALSE;
}
/*********************************************************************\
* Function     MMSC_ModifyTotal
* Purpose      change the total number
* Params       nsign
* Return       void
**********************************************************************/
void MMSC_ModifyTotal(int nSign, int nCount)
{
//    STORAGEINFO storageinfo;

    if (nSign == COUNT_DEC)
    {
        g_MmsCount.nTotalnum--;
        if (g_MmsCount.nTotalnum < 0)
            g_MmsCount.nTotalnum = 0;
    }
    else if (nSign == COUNT_ADD)
    {
        g_MmsCount.nTotalnum++;
//        if (g_MmsCount.nTotalnum >= (MAX_MSG_NUM - MMS_NUM_TIP))
//        {
//            //sometimes the nTotalnum is wrong ,we count it again
//            MMS_GetDiskInfo(&storageinfo);
//            g_MmsCount.nTotalnum = storageinfo.nCount;
//
//            if(g_MmsCount.nTotalnum >= (MAX_MSG_NUM - MMS_NUM_TIP))
//                MsgWin(NULL, NULL, 0, STR_MMS_FULLING, STR_MMS_MMS, Notify_Alert,
//				STR_MMS_OK, NULL, WAITTIMEOUT);
//        }
    }
    else if (nSign == COUNT_SET)
    {
        g_MmsCount.nTotalnum  = nCount;
//        if (g_MmsCount.nTotalnum >= (MAX_MSG_NUM - MMS_NUM_TIP))
//            MsgWin(NULL, NULL, 0, STR_MMS_FULLING, STR_MMS_MMS, Notify_Alert,
//			STR_MMS_OK, NULL, WAITTIMEOUT);
    }

    MMS_Setindbar(g_MmsCount.nUnread, 0, 0, 0, g_MmsCount.nTotalnum);
}
/*********************************************************************\
* Function     MMSC_ModifyUnread
* Purpose      modify the number of unread mms
* Params       int
* Return       void
**********************************************************************/
void MMSC_ModifyUnread(int nSign, int nCount)
{   
    if (nSign == COUNT_SET)
        g_MmsCount.nUnread = nCount;
    else if (nSign == COUNT_DEC)
    {
        g_MmsCount.nUnread--;
        if (g_MmsCount.nUnread < 0)
            g_MmsCount.nUnread = 0;
    }
    else if (nSign == COUNT_ADD)
        g_MmsCount.nUnread++;

    MMS_Setindbar(g_MmsCount.nUnread, 0, 0, 0, g_MmsCount.nTotalnum);
}
/*********************************************************************\
* Function     MMSC_ModifyUnreceive
* Purpose      modify the number of unreive mms
* Params       int
* Return       void
**********************************************************************/
void MMSC_ModifyUnreceive(int nSign, int nCount)
{   
    if (nSign == COUNT_SET)
        g_MmsCount.nUnreceive = nCount;
    else if (nSign == COUNT_DEC)
    {
        g_MmsCount.nUnreceive--;
        if (g_MmsCount.nUnreceive < 0)
            g_MmsCount.nUnreceive = 0;
    }
    else if (nSign == COUNT_ADD)
        g_MmsCount.nUnreceive++;

    MMS_Setindbar(g_MmsCount.nUnread, 0, 0, 0, g_MmsCount.nTotalnum);
}
/*********************************************************************\
* Function     MMSC_ModifyUnconfirmed
* Purpose      modify the number of unconfirmed mms
* Params       int
* Return       void
**********************************************************************/
void MMSC_ModifyUnconfirmed(int nSign, int nCount)
{   
    if (nSign == COUNT_SET)
        g_MmsCount.nUnconfirmed = nCount;
    else if (nSign == COUNT_DEC)
    {
        g_MmsCount.nUnconfirmed--;
        if (g_MmsCount.nUnconfirmed < 0)
            g_MmsCount.nUnconfirmed = 0;
    }
    else if (nSign == COUNT_ADD)
        g_MmsCount.nUnconfirmed++;

    MMS_Setindbar(g_MmsCount.nUnread, 0, 0, 0, g_MmsCount.nTotalnum);
}

void MMSC_ModifyReportUnread(int nSign, int nCount)
{
	PFolderInfo pFolderInfo = NULL;

	pFolderInfo = MMS_GetFolderInfoByID(MU_REPORT);

	if(pFolderInfo == NULL)
		return;

    if (nSign == COUNT_SET)
         pFolderInfo->nMmsNum = nCount;
    else if (nSign == COUNT_DEC)
    {
        pFolderInfo->nMmsNum--;
        if (pFolderInfo->nMmsNum < 0)
            pFolderInfo->nMmsNum = 0;
    }
    else if (nSign == COUNT_ADD)
        pFolderInfo->nMmsNum++;

    MMS_Setindbar(g_MmsCount.nUnread, 0, 0, 0, g_MmsCount.nTotalnum);

}
void MMSC_ModifyFolderUnread(int nFolder, int nSign, int nCount)
{
	PFolderInfo pFolderInfo = NULL;
	int unread, unsend;
	
	pFolderInfo = MMS_GetFolderInfoByID(nFolder);

	if(pFolderInfo == NULL)
		return ;

	if (nSign == COUNT_SET)
        pFolderInfo->nUnreadNum = nCount;
    else if (nSign == COUNT_DEC)
    {
        pFolderInfo->nUnreadNum--;
        if (pFolderInfo->nUnreadNum < 0)
            pFolderInfo->nUnreadNum = 0;
    }
    else if (nSign == COUNT_ADD)
        pFolderInfo->nUnreadNum++;

	pFolderInfo = MMS_GetFolderInfoByID(MU_OUTBOX);
	unsend = pFolderInfo->nMmsNum;
	pFolderInfo = MMS_GetFolderInfoByID(MU_INBOX);
	unread = pFolderInfo->nUnreadNum;
	
	if(pFolderInfo)
		MsgNotify(MU_MMS_NOTIFY, !MMS_IsFlashEnough(), unread, unsend);
}
/*********************************************************************\
* Function     MMSC_ModifyMsgCount
* Purpose      modify the number of mms in one box
* Params       nFolder: the id of the box
               nSign: how to modify
               nCount: if (nSign == COUNT_SET), this param is useful
* Return       void
**********************************************************************/
void MMSC_ModifyMsgCount(int nFolder, int nSign, int nCount)
{
    int nCurFolderCount = 0;
	PFolderInfo pFolderInfo = NULL;
	int unread, unsend;

	pFolderInfo = MMS_GetFolderInfoByID(nFolder);

	// pFolderInfo should not be null
	if (pFolderInfo == NULL)
	{
		MMSMSGWIN("get folder error\r\n");
		return;
	}

	if (nSign == COUNT_DEC)
	{
		pFolderInfo->nMmsNum--;
		if (pFolderInfo->nMmsNum < 0)
			pFolderInfo->nMmsNum = 0;
	}
	else if (nSign == COUNT_ADD)
		pFolderInfo->nMmsNum++;
	else if (nSign == COUNT_SET)
		pFolderInfo->nMmsNum = nCount;
	
	nCurFolderCount = pFolderInfo->nMmsNum;
	
	pFolderInfo = MMS_GetFolderInfoByID(MU_OUTBOX);
	unsend = pFolderInfo->nMmsNum;
	pFolderInfo = MMS_GetFolderInfoByID(MU_INBOX);
	unread = pFolderInfo->nUnreadNum;

	if(pFolderInfo)
		MsgNotify(MU_MMS_NOTIFY, !MMS_IsFlashEnough(), unread, unsend);
}


BOOL MMSC_AutoSend(void)
{
	PMSGHANDLENAME pTemp;

	pTemp = pMsgHandleHead;

	while(pTemp)
	{
		if(MMS_GetFileFolder(pTemp->msgname) ==  MU_OUTBOX
			&& pTemp->msgname[MMS_RESEND_POS] == '1')
		{
			MMS_ModifyResend(pTemp->msgname, 0);
			MMSPro_Send(pTemp->msgname);
		}

		pTemp = pTemp->pNext;
	}

	return TRUE;
}

/*********************************************************************\
* Function     mms_get_unconfirmed
* Purpose      get the number of unconfirmed mms
* Params       void
* Return        int 
**********************************************************************/
int mms_get_unconfirmed(void)
{
    return g_MmsCount.nUnconfirmed;
}
/*********************************************************************\
* Function     mms_get_unreceived
* Purpose      get the number of unreceived mms
* Params       void
* Return        int
**********************************************************************/
int mms_get_unreceived(void)
{
    return g_MmsCount.nUnreceive;
}
/*********************************************************************\
* Function     int mms_get_unread_msg_count()
* Purpose      return the number of unread mms (including unread an unreceive)
* Params       void
* Return        int
**********************************************************************/
int mms_get_unread_msg_count(void)
{
    return (g_MmsCount.nUnread + g_MmsCount.nUnreceive);
}
/*********************************************************************\
* Function     int mms_get_total_count()
* Purpose      return the total number
* Params       void
* Return        int
**********************************************************************/
int mms_get_total_count(void)
{
    return (g_MmsCount.nTotalnum);
}
/*********************************************************************\
* Function     mms_get_message_count(int folder)
* Purpose      return the number of mms in a folder
* Params       void
* Return        int
**********************************************************************/
int mms_get_message_count(int folder)
{
	PFolderInfo pFolderInfo = NULL;

	pFolderInfo = MMS_GetFolderInfoByID(folder);

	if(pFolderInfo == NULL)
		return 0;

	return pFolderInfo->nMmsNum;
}
/*********************************************************************\
* Function     mms_clear_unconfirmed
* Purpose      clear the number of unconfirmed mms
* Params       void
* Return       void   
**********************************************************************/
void mms_clear_unconfirmed()
{
    g_MmsCount.nUnconfirmed = 0;
    MMS_Setindbar(g_MmsCount.nUnread, 0, 0, 0, g_MmsCount.nTotalnum);
}
/*********************************************************************\
* Function     mms_clear_unreceived
* Purpose      clear the number of unreceived mms
* Params       void
* Return       void
**********************************************************************/
void mms_clear_unreceived()
{
    g_MmsCount.nUnreceive = 0;
   MMS_Setindbar(g_MmsCount.nUnread, 0, 0, 0, g_MmsCount.nTotalnum);
}

BOOL mms_copy_message(HWND hwndmu, DWORD handle , int nFolder)
{
	return FALSE;
}

BOOL mms_move_message(HWND hwndmu, DWORD handle , int nFolder)
{
	PMSGHANDLENAME  pMsghandle;
	int         nType;
	
    pMsghandle = GetNodeByHandle(handle);

	nType = pMsghandle->msgname[MMS_TYPE_POS];

	if(nType == TYPE_FIRSTAOTORECV)		//autorecv mms isn't belong any box
		return FALSE;
	
	if(nType == TYPE_UNRECV)
	{
		if((MMSPro_IsInRSTable(pMsghandle->msgname) || MMSPro_IsInRSChain(pMsghandle->msgname)))
        {
			return FALSE;
        }
	}

	if(nFolder == MU_DRAFT)
	{
		ModifyMsgNode((DWORD)pMsghandle, NULL, MMFT_DRAFT);
		
		return MMS_ModifyType(pMsghandle->msgname, TYPE_DRAFT, nFolder);
	}
		
	else
		return(MMS_ModifyFolder(pMsghandle->msgname, nFolder));

}

BOOL mms_suspend_message(HWND hwndmu, DWORD handle)
{
	PMSGHANDLENAME pmsgNode;

    pmsgNode = GetNodeByHandle(handle);

    if (GetMEIsSim() == 0)
    {
        MsgWin(NULL, NULL, 0, STR_MMS_NOSIM, STR_MMS_MMS, Notify_Alert,
			STR_MMS_OK, NULL,MMS_WAITTIME);
        return FALSE;
    }

	ModifyMsgStatus(pmsgNode->msgname, MU_SEND_SUSPENDED, MMFT_SUSPEND);

	DeleteRSChainNodeFromName(pmsgNode->msgname);
	
	return TRUE;
}
BOOL mms_get_count(int folder,int* pnunread,int* pncount)
{
	PFolderInfo pFolderInfo = NULL;

	if(folder == -1)
	{
		*pnunread = g_MmsCount.nUnread;
		*pncount = g_MmsCount.nTotalnum;

		return TRUE;
	}

	if(folder >= 0)
	{
       pFolderInfo = MMS_GetFolderInfoByID(folder);

	   if(pFolderInfo == NULL)
		   return FALSE;

		*pncount = pFolderInfo->nMmsNum;
		*pnunread = pFolderInfo->nUnreadNum;

		return TRUE;
	}

	return FALSE;
}
BOOL mms_new_folder(int folder)
{
	PFolderInfo pFolderInfo = NULL, p = NULL;

	pFolderInfo = MMS_malloc(sizeof(FolderInfo));

	if(pFolderInfo == NULL)
		return FALSE;

	pFolderInfo->nFolderID = folder;
	pFolderInfo->FileType = MyFolder;
	pFolderInfo->nMmsNum = 0;
	pFolderInfo->nUnreadNum = 0;
	pFolderInfo->pNext = NULL;

	if(NULL == pFolderHead)
	{
		pFolderHead = pFolderInfo;
		return TRUE;
	}
	else
	{
		p = pFolderHead;
		while(p->pNext)
		{
			p = p->pNext;
		}
		p->pNext = pFolderInfo;
	}
		
	return TRUE;
}

static BOOL MMS_InitFolder(void)
{
	PMU_FOLDERINFO pInfo =NULL, pTemp =NULL;
	int nCount, i;
	PFolderInfo  pFolderInfo = NULL, p =NULL;

	for(i = 0; i<= MU_REPORT;i++)
	{
		pFolderInfo = MMS_malloc(sizeof(FolderInfo));

		if(pFolderInfo == NULL)
			return FALSE;

		pFolderInfo->nFolderID = i;
		switch(i) 
		{
		case MU_INBOX:
			pFolderInfo->FileType = InType;
			break;
		case MU_MYFOLDER:
			pFolderInfo->FileType = MyFolder;
			break;
		case MU_DRAFT:
			pFolderInfo->FileType = DraftType;
			break;
		case MU_SENT:
			pFolderInfo->FileType = SentType;
			break;
		case MU_OUTBOX:
			pFolderInfo->FileType = OutType;
			break;
		case MU_REPORT:
			pFolderInfo->FileType = ReportType;
			break;
		default :
			break;
		}

		pFolderInfo->pNext = NULL;
		pFolderInfo->nMmsNum = 0;
		pFolderInfo->nUnreadNum = 0;

		if(pFolderHead == NULL)
			pFolderHead = pFolderInfo;
		else
		{
			p = pFolderHead;
			while(p->pNext)
			{
				p= p->pNext;
			}
			p->pNext = pFolderInfo;
		}
	}


	MU_GetFolderInfo(NULL, &nCount);

	if(nCount !=0)
	{
		pInfo = MMS_malloc(sizeof(MU_FOLDERINFO)*nCount);
		if(pInfo == NULL)
			return FALSE;		
	}

	MU_GetFolderInfo(pInfo, &nCount);

	pTemp = pInfo;
		
	while(nCount>0)
	{
		pFolderInfo = MMS_malloc(sizeof(FolderInfo));
		if(pFolderInfo == NULL)
			return FALSE;
		pFolderInfo->nFolderID = pInfo->nFolderID;
		pFolderInfo->FileType = MyFolder;
		
		pFolderInfo->pNext = NULL;
		pFolderInfo->nMmsNum = 0;
		pFolderInfo->nUnreadNum = 0;

		if(pFolderHead == NULL)
			pFolderHead = pFolderInfo;
		else
		{
			p = pFolderHead;
			while(p->pNext)
			{
				p= p->pNext;
			}
			p->pNext = pFolderInfo;
		}
	  
		pInfo++;
		nCount--;
	}	
	MMS_free(pTemp);
    return TRUE;

}
static PFolderInfo MMS_GetFolderInfoByID(int nFolderId)
{
	PFolderInfo pFolderInfo = NULL, p = NULL;

	if(pFolderHead == NULL)
		return NULL;
	p = pFolderHead;
	while(p)
	{
		if(p->nFolderID == nFolderId)
			return p;
		p = p->pNext;
	}
	return NULL;
	
}
static BOOL IsLegalFolderID(nFolder)
{
	PFolderInfo pFolderInfo=NULL, p =NULL;

	if(pFolderHead ==NULL)
		return FALSE;
	p=pFolderHead;

	while(p)
	{
		if(p->nFolderID == nFolder)
			return TRUE;
		p = p->pNext;
	}
	return FALSE;
}

void MMS_FreeFolderInfo()
{
	PFolderInfo pFolderInfo = NULL, p = NULL;

	if(pFolderHead == NULL)
		return ;

	p = pFolderHead;
	
	while (p)
	{
		pFolderInfo = p;
		MMS_free(pFolderInfo);
		p = p->pNext;
	}
}
//////////////////////////////////////////////////////////////////////////
static HINSTANCE   hWriteMMSInstance;
static HWND        hWriteMMSFrameWnd = NULL;
BOOL APP_EntryEditMMS(HWND hWndFrame, PSTR szReceiver,HINSTANCE hWriteMMSInstance);
/*********************************************************************\
* Function        WriteMMS_AppControl
* Purpose      a new entry for favorite and shortcut
* Params       
* Return       
* Remarks      
**********************************************************************/
DWORD EditMMS_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
    DWORD   dwRet;
    BOOL    bShow;    

    dwRet = TRUE;
    switch (nCode)
    {
    case APP_INIT:
        hWriteMMSInstance = (HINSTANCE)pInstance;
        break;

    case APP_INITINSTANCE :
        break;

    case APP_GETOPTION:
        switch(wParam)
        {
        case AS_APPWND:
            dwRet = (DWORD)hWriteMMSFrameWnd;
            break;

        case AS_STATE:
            if (hWriteMMSFrameWnd)
            {
                if (IsWindowVisible(hWriteMMSFrameWnd))
                    dwRet = AS_ACTIVE;
                else
                    dwRet = AS_INACTIVE;
            }
            else
                dwRet = AS_NOTRUNNING;
            break;

        case AS_HELP:
            break;
        }
        break;

    case APP_ACTIVE :
        if (IsWindow(hWriteMMSFrameWnd))
        {
            ShowWindow(hWriteMMSFrameWnd, SW_SHOW);
            ShowOwnedPopups(hWriteMMSFrameWnd, SW_SHOW);
            UpdateWindow(hWriteMMSFrameWnd);
        }
        else
        {
            //StartObjectDebug();
            hWriteMMSFrameWnd = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);
            return APP_EntryEditMMS(hWriteMMSFrameWnd,(char*)lParam,hWriteMMSInstance);
        }
        break;

    case APP_INACTIVE :
        ShowOwnedPopups(hWriteMMSFrameWnd, SW_HIDE);
        ShowWindow(hWriteMMSFrameWnd, SW_HIDE);
        bShow = FALSE;
        break;
    }
    return dwRet;
}

static HWND hEntryCall = NULL;
static int msgEntry = 0;

BOOL MMS_EntryRegister(HWND hWnd, int msg)
{
	hEntryCall = hWnd;
	msgEntry = msg;

	return TRUE;
}

/*********************************************************************\
* Function        
* Purpose      
* Params       
* Return       
* Remarks      
**********************************************************************/
BOOL APP_EntryEditMMS(HWND hWndFrame, PSTR szReceiver,HINSTANCE hWriteMMSInstance)
{
	if (szReceiver && szReceiver[0] != 0)
		return CallMMSEditWnd(NULL, hWndFrame, hEntryCall, msgEntry, MMS_CALLEDIT_MOBIL, 
			szReceiver,	hWriteMMSInstance);
	
	return CallMMSEditWnd(NULL, hWndFrame, hEntryCall, msgEntry, MMS_CALLEDIT_MOBIL, 
		"", hWriteMMSInstance);
}

int MMS_GetRecNum(HWND hwndmu, DWORD msghandle)
{
	PMSGHANDLENAME  pMsghandle;
	int				iStatus;
	TotalMMS		tmms;
	char			dupTo [SIZE_6];
	char			*token;
	int				nSender = 0;
	
    pMsghandle = GetNodeByHandle(msghandle);

	iStatus = pMsghandle->nStatus;

	if(iStatus == MMFT_READED || iStatus == MMFT_UNREAD)
	{
		ReadAndParse(pMsghandle->msgname, &tmms);
		FreeMeta(tmms.pAttatch);

		MultiByteToUTF8(CP_ACP, 0, tmms.mms.wspHead.to, -1, dupTo, SIZE_6);

		token = MMS_chrtok(dupTo, MMS_ASEPCHR, MMS_ASEPCHRF);

		while (token != NULL) 
		{
			if (*token != '\0')
			{
				nSender++;
			}
			token = MMS_chrtok(NULL, MMS_ASEPCHR, MMS_ASEPCHRF);
		}

		return nSender;		//at least one "from"
	}
	else
		return 1;
}

BOOL MMS_GetPendingDReport(char* pFindFile, char* msgID)
{
	PMSGHANDLENAME pMsgHandle = NULL;
	DREPORT dReport;
	int hFile;

	pMsgHandle = pMsgHandleHead;

	while(pMsgHandle)
	{
		if(pMsgHandle->nStatus == MMFT_REPORTING)
		{
			hFile  = MMS_CreateFile(pMsgHandle->msgname, O_RDONLY);
			if (hFile == -1)
				continue;
			
			lseek(hFile, MMS_HEADER_SIZE, SEEK_SET);
			
			read(hFile, &dReport, sizeof(DREPORT));

			close(hFile);

			if(strcmp(dReport.msgId, msgID) == 0)
			{
				strcpy(pFindFile, pMsgHandle->msgname);
				return TRUE;
			}
		}

		pMsgHandle = pMsgHandle->pNext;
	}

	return FALSE;
}

void MMSC_ClearAutoRecv()
{
	struct dirent *dirinfo = NULL;
	DIR *dirtemp = NULL;
	int nType;
	char  filename[256];

	chdir(FLASHPATH);

	dirtemp = opendir(FLASHPATH);
	if(dirtemp == NULL)
		return;	

	dirinfo = readdir(dirtemp);
    
    if (!dirinfo || dirinfo->d_name[0] == 0)
	{
		closedir(dirtemp);
        return;
	}

	do 
    {       
        nType = dirinfo->d_name[MMS_TYPE_POS];
		
		if(nType == TYPE_FIRSTAOTORECV)
		{	
			strcpy(filename, dirinfo->d_name);
			
			AllocMsgHandle(filename, MMFT_UNRECV, FALSE);
			MMSC_ModifyTotal(COUNT_ADD, 0);
			MMSC_ModifyMsgCount(MU_INBOX, COUNT_ADD, 0);
			MMS_ModifyType(filename, TYPE_UNRECV, -1);
			MMSC_ModifyUnreceive(COUNT_ADD, 0);		
		}

	}while(dirinfo = readdir(dirtemp));
	
    closedir(dirtemp);
	return;
}
