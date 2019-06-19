/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MailSend.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "MailHeader.h"

MailSendHead mMailSendHead;
extern HWND HwndMailInBox;

static int nMailOutQueue;
static MailSendNode *pCurSendNode;
static BOOL bReciErr;

extern MailListHead MailListCurHead;

BOOL RegisterMailSendClass();
void UnregisterMailSendClass();
BOOL CreateMailSendWnd();
static LRESULT MailSendWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                               LPARAM lParam);
static void MAIL_GetSendList(MailSendHead *pHead);
static BOOL MAIL_SendOneMail(HWND hWnd);
static BOOL MAIL_GLSendMail(char *szMailFile);
static BOOL MAIL_SaveOneSendMail(MailSendNode *pMailSendNode);
static void MAIL_SaveAllSendMail(void);
BOOL MAIL_AddSendMailNode(HWND hWnd, char *pMailSendName, char *MsgId);
int MAIL_GetSendNodebyHandle(MailSendNode **pMailSendNode, char *SendHandle);
static void MAIL_DelSendMailNode(MailSendNode *pMailSendNode);
void MAIL_DeleteSendNode(MailSendNode *pMailSendNode);
void MAIL_DelAllSendMail(void);
static void MAIL_GetSendDate(char * szSendDate);
static void MAIL_DealSendMessage(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void MAIL_DealSendCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
char *Mail_MultiDecodeOnString(char *pSrc, int nSize);
BOOL MAIL_IsNodeSending(char *NodeHandle);

extern BOOL MAIL_CheckReceiverValid(char *szReceiver);
extern BOOL MAIL_CheckMultiAddr(char *strMailAddr);

BOOL RegisterMailSendClass()
{
    WNDCLASS wc;

    wc.style            = 0;
    wc.lpfnWndProc      = MailSendWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.hbrBackground    = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "MailSendWndClass";

    if(!RegisterClass(&wc))
        return FALSE;
    else
        return TRUE;
}

void UnregisterMailSendClass()
{
    UnregisterClass("MailSendWndClass", NULL);
}

BOOL CreateMailSendWnd()
{
    if(IsWindow(HwndMailSend))
    {
    }
    else
    {
        if(!RegisterMailSendClass())
            return FALSE;

        HwndMailSend = CreateWindow(
            "MailSendWndClass",
            "",
            0,
            0, 0, 0, 0,
            NULL,
            NULL,
            NULL,
            NULL);
    }
    
    if(HwndMailSend == NULL)
    {
        return FALSE;
    }
    
    return TRUE;
}

static LRESULT MailSendWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                               LPARAM lParam)
{
    LRESULT lResult;
    lResult = (LRESULT)TRUE;

    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
            //initialize
            mMailSendHead.len = 0;
            mMailSendHead.OpLock = FALSE;
            mMailSendHead.pNext = NULL;
            mMailSendHead.SendingFlag = FALSE;
            MAIL_GetSendList(&mMailSendHead);
        }
    	break;

    case WM_MAIL_MSG:
        {
            //deal mail protocol message
            MAIL_DealSendMessage(hWnd, wParam, lParam);
        }
    	break;

    case WM_COMMAND:
        {
            //deal dial message
            MAIL_DealSendCommand(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_MAIL_BEGIN_SEND:
        {
            //try to send one mail
            if(mMailSendHead.SendingFlag == FALSE)
            {
                MAIL_SendOneMail(hWnd);
            }
        }
        break;

    case WM_MAIL_TERMINATE_SEND:
        {
            if(nMailOutQueue != 0)
            {
                MAIL_DestroyOutQueue(nMailOutQueue);
            }
            nMailOutQueue = 0;

            pCurSendNode->SendStatus = MU_STU_SUSPENDED;

            MAIL_SaveOneSendMail(pCurSendNode);
            MAIL_SendOneMail(hWnd);
        }
        break;

    case WM_CLOSE:
        {
            MAIL_DelAllSendMail();
        }        
        break;

    case WM_DESTROY:
        {
        }
        break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    }
    return lResult;
}

/******************************************************************** 
* Function	:	SendOneMail
*	   
* Purpose	:  try to get one node from list chain to send 
*
* Remarks	:      
\**********************************************************************/
static BOOL MAIL_SendOneMail(HWND hWnd)
{
    int boxtype;
    MailListNode *ListNode;
    MailSendNode *pTempSend;

    if(mMailSendHead.OpLock)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NETBROKEN, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);

        if(nMailOutQueue != 0)
        {
            MAIL_QuitOutQueueSession(nMailOutQueue);
        }
        
        if(nMailOutQueue != 0)
        {
            MAIL_DestroyOutQueue(nMailOutQueue);
        }
        nMailOutQueue = 0;

        MAIL_SaveAllSendMail();
        MAIL_DealNoReturnConnect();

        return FALSE;
    }

    if(strlen(GlobalMailConfigActi.GLLogin) == 0)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOMAILBOX, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);

        if(nMailOutQueue != 0)
        {
            MAIL_QuitOutQueueSession(nMailOutQueue);
        }
        
        if(nMailOutQueue != 0)
        {
            MAIL_DestroyOutQueue(nMailOutQueue);
        }
        nMailOutQueue = 0;
        
        MAIL_SaveAllSendMail();
        MAIL_DealNoReturnConnect();

        return FALSE;
    }

    if(GlobalMailConfigHead.GLConStatus == MAILBOX_CON_IDLE)
    {
        //call dialing...
        SendMessage(HwndMailDial, WM_MAIL_DIAL_AGAIN, (WPARAM)hWnd, 0);
        return TRUE;
    }

    if(mMailSendHead.SendingFlag)
    {
        //is sending...
        return TRUE;
    }

    if(mMailSendHead.len == 0)
    {
        //finish send all mail
        if(nMailOutQueue != 0)
        {
            MAIL_QuitOutQueueSession(nMailOutQueue);
        }
        
        if(nMailOutQueue != 0)
        {
            MAIL_DestroyOutQueue(nMailOutQueue);
        }
        nMailOutQueue = 0;

        //hang up net connect
        /*MAIL_bNetFlag = MAIL_bNetFlag & 0xFE;
        SendMessage(HwndMailDial, WM_MAIL_DIAL_HANDUP, 0, 0);

        if(MAIL_GetToSendDial)
        {
            MAIL_GetToSendDial = FALSE;
            PostMessage(HwndMailInBox, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_CONNECT, 0), 0);
        }*/
        MAIL_DealNoReturnConnect();

        return TRUE;
    }

    pTempSend = mMailSendHead.pNext;
    while (1)
    {
        if(pTempSend == NULL)
        {
            //finish send all mail
            if(nMailOutQueue != 0)
            {
                MAIL_QuitOutQueueSession(nMailOutQueue);
            }
            
            if(nMailOutQueue != 0)
            {
                MAIL_DestroyOutQueue(nMailOutQueue);
            }
            nMailOutQueue = 0;
            
            MAIL_DealNoReturnConnect();

            return TRUE;
        }
        if((pTempSend->SendStatus != MU_STU_WAITINGSEND)
            && (pTempSend->SendStatus != MU_STU_DEFERMENT)
            && (pTempSend->SendStatus != MU_STU_SENDING))
        {
            pTempSend = pTempSend->pNext;
            continue;
        }
        else
            break;
    }

    mMailSendHead.SendingFlag = TRUE;
    pTempSend->SendStatus = MU_STU_SENDING;
    pCurSendNode = pTempSend;

    boxtype = MU_GetCurFolderType();
    if(boxtype == MU_OUTBOX)
    {
        MU_MsgNode msgnode;
        
        MAIL_GetNodeByHandle(&MailListCurHead, &ListNode, pCurSendNode->SendHandle);
        ListNode->ListSendFlag = MU_STU_SENDING;

        memset(&msgnode, 0x0, sizeof(MU_MsgNode));
        msgnode.msgtype = MU_MSG_EMAIL;
        msgnode.storage_type = MU_STORE_IN_FLASH;
        strncpy(msgnode.addr, ListNode->ListAddress, MU_ADDR_MAX_LEN);
        msgnode.handle = (DWORD)ListNode->ListHandle;
        msgnode.maskdate = ListNode->ListMaskdate;
        strncpy(msgnode.subject, ListNode->ListSubject, MU_SUBJECT_DIS_LEN);
        msgnode.attachment = ListNode->ListAttFlag;
        msgnode.status = ListNode->ListSendFlag;

        printf("\r\nMAIL_SendOneMail:modify msgnode\r\n");

        SendMessage(UniGlobalHwnd, PWM_MSG_MU_MODIFIED, 
            MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)&msgnode);
    }

    printf("\r\nMAIL_SendOneMail:send mail\r\n");
    //get onemail to send
    bReciErr = FALSE;
    if(MAIL_GLSendMail(pTempSend->SendMailFileName) == FALSE)
    {
        if(!bReciErr)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDFAIL, IDP_MAIL_TITLE_EMAIL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
        }
        
        mMailSendHead.SendingFlag = FALSE;
        //pCurSendNode->SendFlag = TRUE;
        pCurSendNode->SendStatus = MU_STU_UNSENT;
        MAIL_SaveOneSendMail(pCurSendNode);
        
        MAIL_SendOneMail(hWnd);
    }

    return TRUE;
}

static BOOL MAIL_GLSendMail(char *szMailFile)
{
    int hFile;               /* temp file (encode mail content)*/
    int hf;                  /* mail souce file */
    
    char szReciName[MAX_MAIL_TO_LEN * 2 + 1];
    char szShowName[MAX_MAIL_SHOW_FILENAME_LEN + 1];
    char szSaveName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    char szPathName[MAX_MAIL_FILE_PATH_LEN + 1];
    char szDateTime[MAX_MAIL_DATE_LEN];
    char szOutPath[PATH_MAXLEN];

    MailFile mMailFile;	        //mail file structure

    int hMailSend;
    int Result;                 //operation result flag
    char *szSendText;            //mail body pointer
    char *pMailContent;
    int AddrNum;
    int ReciLen;

    //enter into directory
    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    //open source file
    hf = GARY_open(szMailFile, O_RDONLY, -1);
    if(hf < 0)
    {
        printf("\r\nMAIL_GLSendMail1 : open error = %d\r\n", errno);

        //PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
        //    IDP_MAIL_BOOTEN_OK, NULL, 20);
        chdir(szOutPath);
        return FALSE;
    }
    
    //read mail head
    memset(&mMailFile, 0x0, sizeof(MailFile));
    if(read(hf, &mMailFile, sizeof(MailFile)) != sizeof(MailFile))
    {
        GARY_close(hf);
        chdir(szOutPath);
        return FALSE;
    }

    memset(szReciName, 0x0, MAX_MAIL_TO_LEN * 2 + 1);
    AddrNum = MAIL_AnalyseMailAddr(mMailFile.To);
    if(AddrNum > 1)
    {
        if(!MAIL_CheckMultiAddr(mMailFile.To))
        {
            char szDis[64];
            int len;

            GARY_close(hf);
            chdir(szOutPath);

            memset(szDis, 0x0, 64);
            len = strlen(mMailFile.To);
            if(len > 20)
            {
                strncpy(szDis, mMailFile.To, 17);
                strcat(szDis, "...");
            }
            else
            {
                strcpy(szDis, mMailFile.To);
            }
            strcat(szDis, IDP_MAIL_STRING_SENDRECIERR);
            bReciErr = TRUE;

            PLXTipsWin(NULL, NULL, 0, szDis, IDP_MAIL_TITLE_EMAIL, 
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
            
            return FALSE;
        }
    }
    else if(AddrNum == 1)
    {
        if(!MAIL_CheckReceiverValid(mMailFile.To))
        {
            char szDis[64];
            int len;
            
            GARY_close(hf);
            chdir(szOutPath);
            
            memset(szDis, 0x0, 64);
            len = strlen(mMailFile.To);
            if(len > 20)
            {
                strncpy(szDis, mMailFile.To, 17);
                strcat(szDis, "...");
            }
            else
            {
                strcpy(szDis, mMailFile.To);
            }
            strcat(szDis, IDP_MAIL_STRING_SENDRECIERR);
            bReciErr = TRUE;
            
            PLXTipsWin(NULL, NULL, 0, szDis, IDP_MAIL_TITLE_EMAIL, 
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
            
            return FALSE;
        }
    }
    else
    {       
        GARY_close(hf);
        chdir(szOutPath);

        bReciErr = TRUE;
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDRECIERR, IDP_MAIL_TITLE_EMAIL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);

        return FALSE;
    }
    strcpy(szReciName, mMailFile.To);
    ReciLen = strlen(szReciName);
    if((szReciName[ReciLen] != ';') && (szReciName[ReciLen - 1] != ';'))
    {
        strcat(szReciName, ";");
    }

    AddrNum = MAIL_AnalyseMailAddr(mMailFile.Cc);
    if(AddrNum > 1)
    {
        if(!MAIL_CheckMultiAddr(mMailFile.Cc))
        {
            char szDis[64];
            int len;
            
            GARY_close(hf);
            chdir(szOutPath);
            
            memset(szDis, 0x0, 64);
            len = strlen(mMailFile.Cc);
            if(len > 20)
            {
                strncpy(szDis, mMailFile.Cc, 17);
                strcat(szDis, "...");
            }
            else
            {
                strcpy(szDis, mMailFile.Cc);
            }
            strcat(szDis, IDP_MAIL_STRING_SENDRECIERR);
            bReciErr = TRUE;
            
            PLXTipsWin(NULL, NULL, 0, szDis, IDP_MAIL_TITLE_EMAIL, 
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);

            return FALSE;
        }
        strcat(szReciName, mMailFile.Cc);
    }
    else if(AddrNum == 1)
    {
        if(!MAIL_CheckReceiverValid(mMailFile.Cc))
        {
            char szDis[64];
            int len;
            
            GARY_close(hf);
            chdir(szOutPath);
            
            memset(szDis, 0x0, 64);
            len = strlen(mMailFile.Cc);
            if(len > 20)
            {
                strncpy(szDis, mMailFile.Cc, 17);
                strcat(szDis, "...");
            }
            else
            {
                strcpy(szDis, mMailFile.Cc);
            }
            strcat(szDis, IDP_MAIL_STRING_SENDRECIERR);
            bReciErr = TRUE;
            
            PLXTipsWin(NULL, NULL, 0, szDis, IDP_MAIL_TITLE_EMAIL, 
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
            
            return FALSE;
        }
        strcat(szReciName, mMailFile.Cc);
    }

    if(GlobalMailConfigActi.GLCC)
    {
        ReciLen = strlen(szReciName);
        if((szReciName[ReciLen] != ';') && (szReciName[ReciLen - 1] != ';'))
        {
            strcat(szReciName, ";");
        }
        strcat(szReciName, GlobalMailConfigActi.GLMailAddr);
    }

    if(mMailFile.MailSize > MAX_MAIL_SIZE)
    {
        bReciErr = TRUE;
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MAXMAILSIZE, NULL, Notify_Alert, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return FALSE;
    }

    //open tempfile for send (encode mail content)
    hFile = GARY_open(MAIL_FILE_SEND_NAME, O_RDWR | O_CREAT, S_IRWXU);
    if(hFile < 0)
    {
        printf("\r\nMAIL_GLSendMail2 : open error = %d\r\n", errno);

        GARY_close(hf);
        //PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Info, 
        //    IDP_MAIL_BOOTEN_OK, NULL, 20);
        chdir(szOutPath);
        return FALSE;
    }

    //fill mail content
    hMailSend = MSG822_Create((int)hFile);
    if(hMailSend == 0)
    {
        GARY_close(hf);
        GARY_close(hFile);
        chdir(szOutPath);
        return FALSE;
    }

    //From
    pMailContent = NULL;
    pMailContent = Mail_MultiDecodeOnString(GlobalMailConfigActi.GLMailAddr, strlen(GlobalMailConfigActi.GLMailAddr));
    if(!pMailContent)
    {
        MSG822_Complete(hMailSend);
        MSG822_Destroy(hMailSend);
        GARY_close(hf);
        GARY_close(hFile);
        chdir(szOutPath);
        return FALSE;
    }
    
    MSG822_SetHeader(hMailSend, MSG822_From, pMailContent);//"stbtest@sohu.com");//
    GARY_FREE(pMailContent);

    //To
    pMailContent = NULL;
    pMailContent = Mail_MultiDecodeOnString(mMailFile.To, strlen(mMailFile.To));
    if(!pMailContent)
    {
        MSG822_Complete(hMailSend);
        MSG822_Destroy(hMailSend);
        GARY_close(hf);
        GARY_close(hFile);
        chdir(szOutPath);
        return FALSE;
    }
    if(MAIL_FAILURE == MSG822_SetHeader(hMailSend, MSG822_To, 
                        (char *)pMailContent))
    {
        MSG822_Complete(hMailSend);
        MSG822_Destroy(hMailSend);
        GARY_close(hf);
        GARY_close(hFile);
        chdir(szOutPath);
        GARY_FREE(pMailContent);
        return FALSE;
    }
    GARY_FREE(pMailContent);

    //Cc
    if(AddrNum >= 1)
    {
        pMailContent = NULL;
        pMailContent = Mail_MultiDecodeOnString(mMailFile.Cc, strlen(mMailFile.Cc));
        if(!pMailContent)
        {
            MSG822_Complete(hMailSend);
            MSG822_Destroy(hMailSend);
            GARY_close(hf);
            GARY_close(hFile);
            chdir(szOutPath);
            return FALSE;
        }
        if(MAIL_FAILURE == MSG822_SetHeader(hMailSend, MSG822_Cc, 
            (char *)pMailContent))
        {
            MSG822_Complete(hMailSend);
            MSG822_Destroy(hMailSend);
            GARY_close(hf);
            GARY_close(hFile);
            chdir(szOutPath);
            GARY_FREE(pMailContent);
            return FALSE;
        }
        GARY_FREE(pMailContent);
    }
    
    //date
    memset(szDateTime, 0x0, MAX_MAIL_DATE_LEN);
    MAIL_GetSendDate(szDateTime);
    MSG822_SetHeader(hMailSend, MSG822_Date, szDateTime);
    
    //subject
    if(strlen(mMailFile.Subject) == 0)
    {
        MSG822_SetHeader(hMailSend, MSG822_Subject, "");
    }
    else
    {
        pMailContent = NULL;
        pMailContent = Mail_MultiDecodeOnString(mMailFile.Subject, strlen(mMailFile.Subject));
        if(!pMailContent)
        {
            MSG822_Complete(hMailSend);
            MSG822_Destroy(hMailSend);
            GARY_close(hf);
            GARY_close(hFile);
            chdir(szOutPath);
            return FALSE;
        }
        MSG822_SetHeader(hMailSend, MSG822_Subject, (char *)pMailContent);
        GARY_FREE(pMailContent);
    }

    //body
    szSendText = malloc(mMailFile.TextSize + 1);
    if(szSendText == NULL)
    {
        MSG822_Complete(hMailSend);
        MSG822_Destroy(hMailSend);
        GARY_close(hf);
        GARY_close(hFile);
        chdir(szOutPath);
        return FALSE;
    }
    memset(szSendText, 0x0, mMailFile.TextSize + 1);
    if(mMailFile.TextSize > 0)
    {
        if(read(hf, szSendText, mMailFile.TextSize) != (int)mMailFile.TextSize)
        {
            GARY_FREE(szSendText);
            MSG822_Complete(hMailSend);
            MSG822_Destroy(hMailSend);
            GARY_close(hf);
            GARY_close(hFile);
            chdir(szOutPath);
            return FALSE;
        }
        pMailContent = NULL;
        pMailContent = Mail_MultiDecodeOnString(szSendText, strlen(szSendText));
        if(!pMailContent)
        {
            GARY_FREE(szSendText);
            MSG822_Complete(hMailSend);
            MSG822_Destroy(hMailSend);
            GARY_close(hf);
            GARY_close(hFile);
            chdir(szOutPath);
            return FALSE;
        }
        memset(szSendText, 0x0, mMailFile.TextSize + 1);
        strcpy(szSendText, pMailContent);
        GARY_FREE(pMailContent);
    }

    //attachment
    if(mMailFile.AttNum > 0)
    {
        char *pTmp;
        char *pTempAtt;
        int i;

        //add mail body
        MSG822_AttachText(hMailSend, (char *)szSendText, "GB2312");

        //add attachment
        pTempAtt = (char *)malloc(mMailFile.AttFileNameLen + 1);
        memset(pTempAtt, 0x0, mMailFile.AttFileNameLen + 1);

        read(hf, pTempAtt, mMailFile.AttFileNameLen);
        pTmp = pTempAtt;
        //showname###savename***showname###savename\0
        while (*pTmp != '\0')
        {
            char FullPathColon[MAX_MAIL_SAVE_FULLNAME_LEN + 1];

            memset(szShowName, 0x0, MAX_MAIL_SHOW_FILENAME_LEN + 1);
            memset(szSaveName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);

            //show name
            while(*pTmp == '*')
            {
                pTmp ++;
            }
            i = 0;
            while((*pTmp != '\0') && (*pTmp != NULL) && (*pTmp != '#'))
            {
                szShowName[i] = *pTmp;
                pTmp ++;
                i ++;
            }

            //save name
            while(*pTmp == '#')
            {
                pTmp ++;
            }
            i = 0;
            while((*pTmp != '\0') && (*pTmp != NULL) && (*pTmp != '*'))
            {
                szSaveName[i] = *pTmp;
                pTmp ++;
                i ++;
            }
            
            //add attachment
            memset(szPathName, 0x0, MAX_MAIL_FILE_PATH_LEN + 1);
            sprintf(szPathName, "%s%s", MAIL_FILE_PATH_DIR, szSaveName);

            memset(FullPathColon, 0x0, MAX_MAIL_SAVE_FULLNAME_LEN + 1);
            MAIL_ConvertPath(FullPathColon, szPathName, FLASH_MNT_TO_COLON);

            if(MSG822_SUCCESS == MSG822_AttachFile(hMailSend, FullPathColon, 
                szShowName, "application/octet-stream",""))
            {
                Result = 0;
            }
        }
        GARY_FREE(pTempAtt);
    }//end if(mMailFile.AttNum>0)
    else
    {
        MSG822_SetText(hMailSend, (char *)szSendText);
    }

    MSG822_Complete(hMailSend);
    GARY_FREE(szSendText);

    //create OutboxQueue for protocol
    if(nMailOutQueue == 0)
    {
        ACCOUNT Account;
        
        Account.username = GlobalMailConfigActi.GLLogin;//"stbtest";//					/* 用户名称 */
        Account.password = GlobalMailConfigActi.GLPassword;//"wps2000";//					/* 用户密码 */
        Account.svr_ip = GlobalMailConfigActi.GLOutServer;//"smtp.sohu.com";//					/* 服务地址 */
        Account.svr_port = 0;		/* 服务端口 */
        
        pMailContent = NULL;
        pMailContent = Mail_MultiDecodeOnString(GlobalMailConfigActi.GLMailAddr, strlen(GlobalMailConfigActi.GLMailAddr));
        if(!pMailContent)
        {
            MSG822_Complete(hMailSend);
            MSG822_Destroy(hMailSend);
            GARY_close(hf);
            GARY_close(hFile);
            chdir(szOutPath);
            return FALSE;
        }
        Account.retaddr = pMailContent;//"stbtest@sohu.com";//    /* 回信地址 */
        
        if(GlobalMailConfigActi.GLSecurity == 1)
        {
            nMailOutQueue = MAIL_CreateOutQueue(HwndMailSend, WM_MAIL_MSG, &Account, "smtps");
        }
        else
        {
            nMailOutQueue = MAIL_CreateOutQueue(HwndMailSend, WM_MAIL_MSG, &Account, "smtp");
        }

        //nMailOutQueue = MAIL_CreateOutQueue(HwndMailSend, WM_MAIL_MSG, &Account);
        
        GARY_FREE(pMailContent);
    }
    //give the mail to protocol
    pMailContent = NULL;
    pMailContent = Mail_MultiDecodeOnString(szReciName, strlen(szReciName));
    if(!pMailContent)
    {
        MSG822_Complete(hMailSend);
        MSG822_Destroy(hMailSend);
        GARY_close(hf);
        GARY_close(hFile);
        chdir(szOutPath);
        return FALSE;
    }
    if(MAIL_AddMailToOutQueue(nMailOutQueue, (int)hFile, pMailContent) == -1)
    {
        GARY_FREE(pMailContent);
        MSG822_Destroy(hMailSend);
        GARY_close(hf);
        GARY_close(hFile);
        Result = remove(MAIL_FILE_SEND_NAME);
        chdir(szOutPath);
        return FALSE;
    }

    GARY_FREE(pMailContent);
    MSG822_Destroy(hMailSend);
    GARY_close(hf);
    GARY_close(hFile);
    Result = remove(MAIL_FILE_SEND_NAME);

    //ask protocol to begin send mail in OutboxQueue
    if(MAIL_SendMail(nMailOutQueue) != MAIL_BLOCKED)
    {
        chdir(szOutPath);
        return FALSE;
    }
    chdir(szOutPath);
    //PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDING, NULL, Notify_Info, 
    //    NULL, NULL, 20);
    return TRUE;
}

static BOOL MAIL_SaveOneSendMail(MailSendNode *pMailSendNode)
{
    int boxtype;
    char szNewFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    char szOutPath[PATH_MAXLEN];

    int hTemp;
    MailFile *mTemp;
    MU_MsgNode msgnode;
    MailListNode *pDelListNode;
    
    int iNode;

    SYSTEMTIME TmpSysTime;
    FILETIME   TmpFileTime;

    if(pMailSendNode == NULL)
    {
        return TRUE;
    }
    
    boxtype = MU_GetCurFolderType();

    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    if(pMailSendNode->SendStatus == MU_STU_SENDING)
    {
        //send successfully
        //get new sent-file name
        memset(szNewFileName, 0x0 ,MAX_MAIL_SAVE_FILENAME_LEN + 1);
        sprintf(szNewFileName, "%s", MAIL_SEND_FILE);
        MAIL_GetNewFileName(szNewFileName, MAX_MAIL_SAVE_FILENAME_LEN + 1);

        MAIL_DelCountNode(&GlobalMailCountHead, MU_OUTBOX, 0);
        MAIL_AddCountNode(&GlobalMailCountHead, MU_SENT, 0);

        //move outboxfile to sentboxfile
        rename(pMailSendNode->SendMailFileName, szNewFileName);
        strcpy(pMailSendNode->SendMailFileName, szNewFileName);

        hTemp = GARY_open(szNewFileName, O_RDWR, -1);
        if(hTemp < 0)
        {
            printf("\r\nMAIL_SaveOneSendMail1 : open error = %d\r\n", errno);

            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
            chdir(szOutPath);
            return FALSE;
        }
        
        //read mail
        mTemp = malloc(sizeof(MailFile));
        if(mTemp == NULL)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
            chdir(szOutPath);
            return FALSE;
        }
        memset(mTemp, 0x0, sizeof(MailFile));
        if(read(hTemp, mTemp, sizeof(MailFile)) != sizeof(MailFile))
        {
            GARY_FREE(mTemp);
            GARY_close(hTemp);
            chdir(szOutPath);
            return FALSE;
        }

        mTemp->SendStatus = MU_STU_SENDING;
        mTemp->ReadFlag = MU_STU_SENT;

        lseek(hTemp, 0, SEEK_SET);
        if(write(hTemp, mTemp, sizeof(MailFile)) != sizeof(MailFile))
        {
            GARY_FREE(mTemp);
            GARY_close(hTemp);
            chdir(szOutPath);
            return FALSE;
        }
        GARY_close(hTemp);

        if((boxtype == MU_OUTBOX) || (boxtype == MU_SENT))
        {
            if(boxtype == MU_OUTBOX)
            {
                //delete list file node(outbox)(MailInterface.c)
                iNode = (MAIL_GetNodeByHandle(&MailListCurHead, &pDelListNode, 
                                                mTemp->MsgId));
                
                SendMessage(UniGlobalHwnd, PWM_MSG_MU_DELETE, 
                    MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)((DWORD)pDelListNode->ListHandle));

                if(iNode >= 0)
                {
                    MAIL_DelBoxFileNode(&MailListCurHead, iNode);
                }
            }

            else if(boxtype == MU_SENT)
            {                
                //add list file node(sentbox)(MailInterface.c)
                pDelListNode = (MailListNode *)malloc(sizeof(MailListNode));
                if(pDelListNode == NULL)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else
                {
                    memset(pDelListNode, 0x0, sizeof(MailListNode));
                    sprintf(pDelListNode->ListMailFileName, "%s", 
                        szNewFileName);
                    pDelListNode->ListSize = mTemp->MailSize;
                    sprintf(pDelListNode->ListHandle, "%s", mTemp->MsgId);
                    sprintf(pDelListNode->ListAddress, "%s", mTemp->address);
                    sprintf(pDelListNode->ListSubject, "%s", mTemp->Subject);
                    pDelListNode->ListAttFlag = (mTemp->AttNum)?TRUE:FALSE;
                    pDelListNode->ListReadFlag = mTemp->ReadFlag;
                    pDelListNode->ListSendFlag = mTemp->SendStatus;
                    
                    GetLocalTime(&TmpSysTime);
                    memcpy(&mTemp->LocalDate, &TmpSysTime, sizeof(SYSTEMTIME));
                    MSG_STtoFT(&TmpSysTime, &TmpFileTime);
                    pDelListNode->ListMaskdate = TmpFileTime.dwLowDateTime;
                    pDelListNode->pNext = NULL;
                                       
                    MAIL_AddBoxFileNode(&MailListCurHead, pDelListNode);
                    
                    memset(&msgnode, 0x0, sizeof(MU_MsgNode));
                    strncpy(msgnode.addr, pDelListNode->ListAddress, MU_ADDR_MAX_LEN);
                    msgnode.handle = (DWORD)pDelListNode->ListHandle;
                    msgnode.maskdate = pDelListNode->ListMaskdate;
                    msgnode.attachment = pDelListNode->ListAttFlag;
                    msgnode.msgtype = MU_MSG_EMAIL;
                    msgnode.status = pDelListNode->ListReadFlag;
                    msgnode.storage_type = MU_STORE_IN_FLASH;
                    strncpy(msgnode.subject, pDelListNode->ListSubject, MU_SUBJECT_DIS_LEN);

                    SendMessage(UniGlobalHwnd, PWM_MSG_MU_NEWMTMSG, 
                        MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)&msgnode);
                }
            }
        }
        GARY_FREE(mTemp);
        MAIL_DelSendMailNode(pMailSendNode);
    }
    else
    {
        hTemp = GARY_open(pMailSendNode->SendMailFileName, O_RDWR, -1);
        if(hTemp < 0)
        {
            printf("\r\nMAIL_SaveOneSendMail2 : open error = %d\r\n", errno);

            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
        }
        
        //read mail
        mTemp = malloc(sizeof(MailFile));
        memset(mTemp, 0x0, sizeof(MailFile));
        if(read(hTemp, mTemp, sizeof(MailFile)) != sizeof(MailFile))
        {
            GARY_FREE(mTemp);
            GARY_close(hTemp);
            chdir(szOutPath);
            return FALSE;
        }
        //modify file
        mTemp->SendStatus = pMailSendNode->SendStatus;
        lseek(hTemp, 0, SEEK_SET);
        if(write(hTemp, mTemp, sizeof(MailFile)) != sizeof(MailFile))
        {
            GARY_FREE(mTemp);
            GARY_close(hTemp);
            chdir(szOutPath);
            return FALSE;
        }
        
        //modify list file(outbox)(MailInterface.c)
        if(boxtype == MU_OUTBOX)
        {
            MAIL_GetNodeByHandle(&MailListCurHead, &pDelListNode, mTemp->MsgId);
            pDelListNode->ListSendFlag = MU_STU_UNSENT;

            memset(&msgnode, 0x0, sizeof(MU_MsgNode));
            msgnode.msgtype = MU_MSG_EMAIL;
            msgnode.storage_type = MU_STORE_IN_FLASH;
            strncpy(msgnode.addr, pDelListNode->ListAddress, MU_ADDR_MAX_LEN);
            msgnode.handle = (DWORD)pDelListNode->ListHandle;
            msgnode.maskdate = pDelListNode->ListMaskdate;
            strncpy(msgnode.subject, pDelListNode->ListSubject, MU_SUBJECT_DIS_LEN);
            msgnode.attachment = pDelListNode->ListAttFlag;
            msgnode.status = pDelListNode->ListSendFlag;
            
            SendMessage(UniGlobalHwnd, PWM_MSG_MU_MODIFIED, 
                MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)&msgnode);
        }

        GARY_FREE(mTemp);
        GARY_close(hTemp);
    }
    chdir(szOutPath);

    return TRUE;
}

static void MAIL_SaveAllSendMail(void)
{
    MailSendNode *pTemp;

    if((mMailSendHead.len == 0) || (mMailSendHead.pNext == NULL))
        return;

    pTemp = mMailSendHead.pNext;
    while (pTemp)
    {
        pTemp->SendStatus = MU_STU_UNSENT;
        MAIL_SaveOneSendMail(pTemp);
        pTemp = pTemp->pNext;
    }
    return;
}

BOOL MAIL_AddSendMailNode(HWND hWnd, char *pMailSendName, char *MsgId)
{
    MailSendNode *pTemp;
    MailSendNode *pNew;

    pNew = (MailSendNode *)malloc(sizeof(MailSendNode));
    if(pNew == NULL)
    {
        return FALSE;
    }

    memset(pNew, 0x0, sizeof(MailSendNode));
    strcpy(pNew->SendMailFileName, pMailSendName);
    strcpy(pNew->SendHandle, MsgId);
    //pNew->SendFlag = FALSE;
    if(GlobalMailConfigActi.GLSendMsg == MAIL_CONNECT_WHEN)
    {
        pNew->SendStatus = MU_STU_DEFERMENT;
    }
    else
    {
        pNew->SendStatus = MU_STU_WAITINGSEND;
    }
    
    pNew->pNext = NULL;
    mMailSendHead.OpLock = TRUE;

    if(mMailSendHead.len == 0)
    {
        mMailSendHead.pNext = pNew;
        mMailSendHead.len ++;
        mMailSendHead.OpLock = FALSE;
        return TRUE;
    }

    pTemp = mMailSendHead.pNext;
    while(pTemp->pNext != NULL)
    {
        pTemp = pTemp->pNext;
    }
    pTemp->pNext = pNew;
    mMailSendHead.len ++;
    mMailSendHead.OpLock = FALSE;
    
    return TRUE;
}

static void MAIL_DelSendMailNode(MailSendNode *pMailSendNode)
{
    MailSendNode *pTemp;

    if(mMailSendHead.len == 0)
        return;

    if(mMailSendHead.pNext == pMailSendNode)
    {
        mMailSendHead.pNext = pMailSendNode->pNext;
        GARY_FREE(pMailSendNode);
        pMailSendNode = NULL;
        mMailSendHead.len --;
        return;
    }
    pTemp = mMailSendHead.pNext;

    while (pTemp->pNext != NULL)
    {
        if(pTemp->pNext == pMailSendNode)
        {
            pTemp->pNext = pMailSendNode->pNext;
            GARY_FREE(pMailSendNode);
            pMailSendNode = NULL;
            mMailSendHead.len --;
            break;
        }
        else
        {
            pTemp = pTemp->pNext;
        }
    }
    return;
}

void MAIL_DeleteSendNode(MailSendNode *pMailSendNode)
{
    MailSendNode *pTemp;
    MailSendNode *pPre;
    
    if(mMailSendHead.len == 0)
        return;
    
    pPre = mMailSendHead.pNext;
    if(strcmp(pPre->SendHandle, pMailSendNode->SendHandle) == 0)
    {
        mMailSendHead.pNext = pPre->pNext;
        GARY_FREE(pPre);
        mMailSendHead.len --;
        return;
    }
    
    pTemp = pPre->pNext;
    while (pTemp != NULL)
    {
        if(strcmp(pTemp->SendHandle, pMailSendNode->SendHandle) == 0)
        {
            pPre->pNext = pTemp->pNext;
            GARY_FREE(pTemp);
            mMailSendHead.len --;
            return;
        }
        else
        {
            pPre = pTemp;
            pTemp = pPre->pNext;
        }
    }
    return;
}

int MAIL_GetSendNodebyHandle(MailSendNode **pMailSendNode, char *SendHandle)
{
    MailSendNode *pTemp;
    
    if(mMailSendHead.len == 0)
        return -1;
    
    pTemp = mMailSendHead.pNext;
    while(pTemp != NULL)
    {
        if(strcmp(pTemp->SendHandle, SendHandle) == 0)
        {
            *pMailSendNode = pTemp;
            return 0;
        }
        else
        {
            pTemp = pTemp->pNext;
        }
    }
    return -1;
}

void MAIL_DelAllSendMail(void)
{
    MailSendNode *pTemp;

    while(mMailSendHead.len > 0)
    {
        pTemp = mMailSendHead.pNext;
        if(pTemp)
        {
            mMailSendHead.pNext = pTemp->pNext;
            GARY_FREE(pTemp);
            pTemp = NULL;
            mMailSendHead.len --;
        }
        else
            break;
    }
    mMailSendHead.len = 0;
    return;
}

static void MAIL_GetSendDate(char * szSendDate)
{
    SYSTEMTIME pSysTime;

    GetLocalTime(&pSysTime);
    MAIL_SystemToChar(&pSysTime, szSendDate);
    
    return;
}

static void MAIL_DealSendMessage(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(lParam)) 
    {
    case MSG_MAIL_GETHOST:
        {
            //get host message
            if(HIWORD(lParam) != MAIL_SUCCESS)
            {
                mMailSendHead.SendingFlag = FALSE;
                //pCurSendNode->SendFlag = TRUE;
                pCurSendNode->SendStatus = MU_STU_UNSENT;

                printf("\r\nGARY_MAILPROTOCOL_SEND:lParam is %x\r\n", (unsigned int)lParam);
                
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDFAIL, IDP_MAIL_TITLE_EMAIL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
                MAIL_SaveOneSendMail(pCurSendNode);
                MAIL_SendOneMail(hWnd);
            }
        }
    	break;

    case MSG_MAIL_CONNECT:
        {
            if(HIWORD(lParam) != MAIL_SUCCESS)
            {
                printf("\r\nGARY_MAILPROTOCOL_SEND:lParam is %x\r\n", (unsigned int)lParam);

                if(HIWORD(lParam) == MAIL_TIMEOUT)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDFAIL, IDP_MAIL_TITLE_EMAIL,
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDFAIL, IDP_MAIL_TITLE_EMAIL,
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                mMailSendHead.SendingFlag = FALSE;
                //pCurSendNode->SendFlag = TRUE;
                pCurSendNode->SendStatus = MU_STU_UNSENT;

                MAIL_SaveOneSendMail(pCurSendNode);
                MAIL_SendOneMail(hWnd);
            }
        }
    	break;

    case MSG_MAIL_LOGIN:
        {
            if(HIWORD(lParam) != MAIL_SUCCESS)
            {
                printf("\r\nGARY_MAILPROTOCOL_SEND:lParam is %x\r\n", (unsigned int)lParam);
                
                if(HIWORD(lParam) == MAIL_TIMEOUT)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDFAIL, IDP_MAIL_TITLE_EMAIL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDFAIL, IDP_MAIL_TITLE_EMAIL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                mMailSendHead.SendingFlag = FALSE;
                //pCurSendNode->SendFlag = TRUE;
                pCurSendNode->SendStatus = MU_STU_UNSENT;
                
                MAIL_SaveOneSendMail(pCurSendNode);
                MAIL_SendOneMail(hWnd);
            }
        }
        break;

    case MSG_MAIL_SENDDATA:
        {
//            SendMessage();
        }
        break;

    case MSG_MAIL_SEND:
        {
            //pCurSendNode->SendFlag = TRUE;
            mMailSendHead.SendingFlag = FALSE;
            if(HIWORD(lParam) == MAIL_SUCCESS)
            {
                pCurSendNode->SendStatus = MU_STU_SENDING;
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDSUCC, NULL, Notify_Success, 
                    IDP_MAIL_BOOTEN_OK, NULL, 20);
            }
            else
            {
                printf("\r\nGARY_MAILPROTOCOL_SEND:lParam is %x\r\n", (unsigned int)lParam);
                
                pCurSendNode->SendStatus = MU_STU_UNSENT;
                if(HIWORD(lParam) == MAIL_TIMEOUT)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDFAIL, IDP_MAIL_TITLE_EMAIL,
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDFAIL, IDP_MAIL_TITLE_EMAIL,
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
            }
            MAIL_SaveOneSendMail(pCurSendNode);
            MAIL_SendOneMail(hWnd);
        }
        break;

    case MSG_MAIL_BROKEN:
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NETBROKEN, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
            mMailSendHead.SendingFlag = FALSE;
        }
        break;

    case MSG_MAIL_QUIT:
        {
            mMailSendHead.SendingFlag = FALSE;
        }
        break;

    default:
        break;
    }

    return;
}

static void MAIL_DealSendCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case WM_MAIL_DIAL_SUCCESS:
        {
            SendMessage(hWnd, WM_MAIL_BEGIN_SEND, 0, 0);
        }
        break;

    case WM_MAIL_DIAL_CANCEL:
        {
            printf("\r\nGARY_MAILPROTOCOL_SEND:dial failure\r\n");

            mMailSendHead.SendingFlag = FALSE;

            if(nMailOutQueue != 0)
            {
                MAIL_DestroyOutQueue(nMailOutQueue);
            }
            nMailOutQueue = 0;

            MAIL_SaveAllSendMail();
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDNONET, IDP_MAIL_TITLE_EMAIL,
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);

            MAIL_DealNextConnect();
        }
        break;

    default:
        PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return;
}

static void MAIL_GetSendList(MailSendHead *pHead)
{
    char        szBoxFilePath[MAX_MAIL_FILE_PATH_LEN + 1];
    int         hFile;
    char        szOutPath[PATH_MAXLEN];

    MailFile    mMailFile;
    MailSendNode    *pTemp;
    MailSendNode    *pNew;

    static struct dirent    *dirinfo = NULL;
    static DIR  *diropen = NULL;

    memset(szBoxFilePath, 0x0, (MAX_MAIL_FILE_PATH_LEN + 1));
    sprintf(szBoxFilePath, "%s", MAIL_OUT_FILE_MATCH);

    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    diropen = GARY_opendir(MAIL_FILE_PATH);
    if(diropen == NULL)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        chdir(szOutPath);
        return;
    }

    while (dirinfo = readdir(diropen))
    {
        if(MAIL_FileTypeMatch(dirinfo->d_name, szBoxFilePath))
            continue;

        hFile = GARY_open(dirinfo->d_name, O_RDONLY, -1);
        if(hFile < 0)
        {
            printf("\r\nMAIL_GetSendList : open error = %d\r\n", errno);

            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
            continue;
        }

        memset(&mMailFile, 0x0, sizeof(MailFile));
        if(read(hFile,&mMailFile,sizeof(MailFile)) != sizeof(MailFile))
        {
            GARY_close(hFile);
            continue;
        }
        GARY_close(hFile);
        
        pNew = (MailSendNode *)malloc(sizeof(MailSendNode));
        if(pNew == NULL)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
            MAIL_DelAllSendMail();
            GARY_closedir(diropen);
            chdir(szOutPath);
            return;
        }
        memset(pNew, 0x0, sizeof(MailSendNode));
        sprintf(pNew->SendMailFileName, "%s", dirinfo->d_name);
        sprintf(pNew->SendHandle, "%s", mMailFile.MsgId);
        pNew->SendStatus = mMailFile.SendStatus;
        pNew->pNext = NULL;

        mMailSendHead.OpLock = TRUE;
        
        if(mMailSendHead.len == 0)
        {
            mMailSendHead.pNext = pNew;
            mMailSendHead.len ++;
            mMailSendHead.OpLock = FALSE;
            continue;
        }
        
        pTemp = mMailSendHead.pNext;
        while(pTemp->pNext != NULL)
        {
            pTemp = pTemp->pNext;
        }
        pTemp->pNext = pNew;
        mMailSendHead.len ++;
        mMailSendHead.OpLock = FALSE;
    }
    GARY_closedir(diropen);
    chdir(szOutPath);

    return;
}

char *Mail_MultiDecodeOnString(char *pSrc, int nSize)
{
    char *pDest;
    int length;
    
    if (!pSrc)
        return NULL;
    
    pDest = NULL;
    length = MultiByteToUTF8(0, 0, pSrc, nSize, pDest, 0);
    
    pDest = (char *)malloc(length + 1); 
    memset(pDest, 0x00, length + 1);
    if (!pDest)
        return NULL;
    
    MultiByteToUTF8(0, 0, pSrc, nSize, pDest, length);
    
    return pDest;
}

BOOL MAIL_IsNodeSending(char *NodeHandle)
{
    if(mMailSendHead.SendingFlag)
    {
        if(strcmp(pCurSendNode->SendHandle, NodeHandle) == 0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}
