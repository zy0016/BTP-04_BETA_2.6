/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailInterface.c
*
* Purpose  : 
*
\**************************************************************************/

#include "MailInterface.h"

static int CurBoxType;
static int CurFolder;
static int MsgFileNum = 0;

extern MailSendHead mMailSendHead;

BOOL email_register(MU_IMessage **Imsg)
{
    *Imsg = &mu_mail;
    
    return TRUE;
}

BOOL mail_init(HWND hwndmu)
{
    mkdir(MAIL_FILE_PATH, S_IRWXU);
    GlobalMailConfigHead.GLSIMState = GetSIMState();

    GlobalMailOpen = 0;
    GlobalMailOpendir = 0;
    printf("init GlobalMailOpen\r\n");

#ifndef _MAIL_EMU_
    MAIL_JudgeCPUType();
#endif
    MAIL_Initial();
    
    CreateMailDialWnd();
    CreateMailSendWnd();
    CreateMailGetWnd();

    RegisterMailInBoxClass();
    RegisterMailViewClass();
    RegisterMailEditClass();

    GlobalMailNumber.TotalNum = 0;
    GlobalMailNumber.TotalUnread = 0;

    MAIL_CountMailList();

    DlmNotify(PS_NEWMSG, 1);

    return TRUE;
}

BOOL mail_get_messages(HWND hwndmu, int folder)
{
    UniGlobalHwnd = hwndmu;
    MsgFileNum = 0;
    memset(&MailListCurHead, 0x0, sizeof(MailListHead));
    if(!(MAIL_GetBoxFileList(&MailListCurHead, folder)))
    {
        SendMessage(hwndmu, PWM_MSG_MU_GETMSGS_RESP,
            MAKEWPARAM(MU_ERR_FAILED,MU_MDU_EMAIL), (LPARAM)NULL);
        return FALSE;
    }
    else
    {
        SendMessage(hwndmu, PWM_MSG_MU_GETMSGS_RESP, 
            MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)NULL);
        return TRUE;
    }
}

BOOL mail_get_one_message(MU_MsgNode *msgnode)
{
    MailListNode *TmpMailListNode;
    
    if(!(MAIL_SearchBoxFileNode(&MailListCurHead, &TmpMailListNode, MsgFileNum)))
    {
        MsgFileNum = 0;
        return FALSE;
    }
    
    msgnode->msgtype = MU_MSG_EMAIL;
    msgnode->storage_type = MU_STORE_IN_FLASH;
    strncpy(msgnode->addr, TmpMailListNode->ListAddress, MU_ADDR_MAX_LEN);
    msgnode->handle = (DWORD)TmpMailListNode->ListHandle;
    msgnode->maskdate = TmpMailListNode->ListMaskdate;
    strncpy(msgnode->subject, TmpMailListNode->ListSubject, MU_SUBJECT_DIS_LEN);
    msgnode->attachment = TmpMailListNode->ListAttFlag;
    if(CurBoxType == MU_OUTBOX)
    {
        if((TmpMailListNode->ListSendFlag == MU_STU_WAITINGSEND) && 
            GlobalMailConfigHead.GLNetUsed && 
            (GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SEND))
        {
            if(MAIL_IsNodeSending(TmpMailListNode->ListHandle))
            {
                msgnode->status = MU_STU_SENDING;
            }
            else
            {
                msgnode->status = MU_STU_WAITINGSEND;
            }
        }
        else
        {
            msgnode->status = TmpMailListNode->ListSendFlag;
        }
    }
    else
    {
        msgnode->status = TmpMailListNode->ListReadFlag;
    }
    MsgFileNum ++;
    
    return TRUE;
}

BOOL mail_release_message(HWND hwndmu)
{
    MAIL_ReleaseBoxFileList(&MailListCurHead);

    return TRUE;
}

BOOL mail_read_message(HWND hwndmu, DWORD msghandle, BOOL bPre, BOOL bNext)
{
    MailListNode *pDelListNode;
    int iNode = -1;
    int TmpBoxType;

    TmpBoxType = MU_GetCurFolderType();
    iNode = MAIL_GetNodeByHandle(&MailListCurHead, &pDelListNode, (char *)msghandle);
    if(iNode < 0)
    {
        return FALSE;
    }
    if(TmpBoxType != MU_DRAFT)
    {
        MU_MsgNode msgnode;

        if((TmpBoxType > 4) || (TmpBoxType == MU_INBOX))
        {
            pDelListNode->ListReadFlag = MU_STU_READ;

            memset(&msgnode, 0x0, sizeof(MU_MsgNode));
            msgnode.msgtype = MU_MSG_EMAIL;
            msgnode.storage_type = MU_STORE_IN_FLASH;
            strncpy(msgnode.addr, pDelListNode->ListAddress, MU_ADDR_MAX_LEN);
            msgnode.handle = (DWORD)pDelListNode->ListHandle;
            msgnode.maskdate = pDelListNode->ListMaskdate;
            strncpy(msgnode.subject, pDelListNode->ListSubject, MU_SUBJECT_DIS_LEN);
            msgnode.attachment = pDelListNode->ListAttFlag;
            msgnode.status = pDelListNode->ListReadFlag;
            
            SendMessage(UniGlobalHwnd, PWM_MSG_MU_MODIFIED, 
                MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)&msgnode);
        }

        MAIL_DelUnreadNode(&GlobalMailCountHead, TmpBoxType);
        MAIL_CreateViewWnd(hwndmu, pDelListNode->ListMailFileName, (DWORD)pDelListNode->ListHandle, TmpBoxType, bPre, bNext);
    }
    else
    {
        char szOutFile[PATH_MAXLEN];
        int hFile;
        MailFile detailFile;
        char *szText;
        char *szAtt;
        SYSTEMTIME sy;

        memset(szOutFile, 0x0, PATH_MAXLEN);
        getcwd(szOutFile, PATH_MAXLEN);
        chdir(MAIL_FILE_PATH);
        hFile = GARY_open(pDelListNode->ListMailFileName, O_RDONLY, -1);
        if(hFile < 0)
        {
            printf("\r\nmail_read_message : open error = %d\r\n", errno);

            chdir(szOutFile);
            return FALSE;
        }
        memset(&detailFile, 0x0, sizeof(MailFile));
        if(read(hFile, &detailFile, sizeof(MailFile)) != sizeof(MailFile))
        {
            GARY_close(hFile);
            chdir(szOutFile);
            return FALSE;
        }
        szText = NULL;
        szAtt = NULL;
        if(detailFile.TextSize != 0)
        {
            szText = (char *)malloc(sizeof(char) * detailFile.TextSize + 1);
            if(szText == NULL)
            {
                GARY_close(hFile);
                chdir(szOutFile);
                return FALSE;
            }
            memset(szText, 0x0, sizeof(char) * detailFile.TextSize + 1);
            read(hFile, szText, sizeof(char) * detailFile.TextSize);
        }
        if(detailFile.AttNum > 0)
        {
            szAtt = (char *)malloc(sizeof(char) * detailFile.AttFileNameLen + 1);
            if(szAtt == NULL)
            {
                GARY_FREE(szText);
                GARY_close(hFile);
                chdir(szOutFile);
                return FALSE;
            }
            memset(szAtt, 0x0, sizeof(char) * detailFile.AttFileNameLen + 1);
            read(hFile, szAtt, sizeof(char) * detailFile.AttFileNameLen);
        }
        GARY_close(hFile);
        chdir(szOutFile);
    
        GetLocalTime(&sy);
        CreateMailEditWnd(MuGetFrame(), detailFile.To, detailFile.Cc, detailFile.Subject, szText, szAtt, 
            &sy, msghandle, TmpBoxType);
        GARY_FREE(szText);
        GARY_FREE(szAtt);
    }

    return TRUE;
}

BOOL mail_delete_message(HWND hwndmu, DWORD msghandle)
{
    MailListNode *pDelListNode;
    MailSendNode DelSendNode;
    int iNode = -1;
    int TmpBoxType;
    
    TmpBoxType = MU_GetCurFolderType();
    iNode = MAIL_GetNodeByHandle(&MailListCurHead, &pDelListNode, (char *)msghandle);
    if(iNode < 0)
    {
        SendMessage(hwndmu, PWM_MSG_MU_DELETED_RESP, 
            MAKEWPARAM(MU_ERR_FAILED, MU_MDU_EMAIL), (LPARAM)msghandle);
        return FALSE;
    }

    if(MAIL_DeleteFile(pDelListNode->ListMailFileName))
    {
        if(pDelListNode->ListReadFlag == MU_STU_UNREAD)
        {
            MAIL_DelCountNode(&GlobalMailCountHead, TmpBoxType, 1);
        }
        else
        {
            MAIL_DelCountNode(&GlobalMailCountHead, TmpBoxType, 0);
        }
        
        if(TmpBoxType == MU_OUTBOX)
        {
            memset(&DelSendNode, 0x0, sizeof(MailSendNode));
            strcpy(DelSendNode.SendHandle, pDelListNode->ListHandle);
            strcpy(DelSendNode.SendMailFileName, pDelListNode->ListMailFileName);
            DelSendNode.SendStatus = pDelListNode->ListSendFlag;
            DelSendNode.pNext = NULL;
            MAIL_DeleteSendNode(&DelSendNode);
        }
        MAIL_DelBoxFileNode(&MailListCurHead, iNode);

        //PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELETED, NULL, 
        //    Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);

        SendMessage(hwndmu, PWM_MSG_MU_DELETED_RESP, 
            MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)msghandle);
        return TRUE;
    }
    else
    {
        SendMessage(hwndmu, PWM_MSG_MU_DELETED_RESP, 
            MAKEWPARAM(MU_ERR_FAILED, MU_MDU_EMAIL), (LPARAM)msghandle);
        return FALSE;
    }
}

BOOL mail_delete_multi_messages(HWND hwndmu, DWORD handle[], int i)
{
    return TRUE;
}

BOOL mail_delete_all_messages(HWND hwndmu, int folder, MU_DELALL_TYPE ntype)
{
    MailListNode *pDelListNode;

    memset(&MailListCurHead, 0x0, sizeof(MailListHead));
    if(!(MAIL_GetBoxFileList(&MailListCurHead, folder)))
    {
        SendMessage(hwndmu, PWM_MSG_MU_DELALL_RESP,
            MAKEWPARAM(MU_ERR_FAILED,MU_MDU_EMAIL), (LPARAM)NULL);
        return FALSE;
    }

    pDelListNode = MailListCurHead.pNext;
    while(pDelListNode)
    {
        if(MAIL_DeleteFile(pDelListNode->ListMailFileName))
        {
            if(pDelListNode->ListReadFlag == MU_STU_UNREAD)
            {
                MAIL_DelCountNode(&GlobalMailCountHead, folder, 1);
            }
            else
            {
                MAIL_DelCountNode(&GlobalMailCountHead, folder, 0);
            }

            MAIL_DelBoxFileNode(&MailListCurHead, 0);
            
            SendMessage(hwndmu, PWM_MSG_MU_DELETE, 
                MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)(DWORD)pDelListNode->ListHandle);
        }
        else
        {
            SendMessage(hwndmu, PWM_MSG_MU_DELALL_RESP, 
                MAKEWPARAM(MU_ERR_FAILED, MU_MDU_EMAIL), (LPARAM)NULL);
            return FALSE;
        }
        pDelListNode = MailListCurHead.pNext;
    }

    if(folder == MU_OUTBOX)
    {
        MAIL_DelAllSendMail();
    }
    
    MAIL_DelCountFolder(&GlobalMailCountHead, folder);

    SendMessage(hwndmu, PWM_MSG_MU_DELALL_RESP, 
        MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)NULL);
    
    return TRUE;
}

BOOL mail_new_message(HWND hwndmu)
{
    SYSTEMTIME sy;

    memset(&sy, 0x0, sizeof(SYSTEMTIME));
    GetLocalTime(&sy);
    CreateMailEditWnd(MuGetFrame(), NULL, NULL, NULL, 
        NULL, NULL, &sy, -1, -1);
    return TRUE;
}

BOOL mail_setting(HWND hwndmu)
{
    MAIL_CreateSetWnd(MuGetFrame());
    
    return TRUE;
}

BOOL mail_get_detail(HWND hwndmu, DWORD handle)
{
    MailListNode *pDelListNode;
    MailFile detailFile;
    int iNode = -1;
    int hFile = -1;
    int TmpBoxType;

    char szOutFile[PATH_MAXLEN];
    
    TmpBoxType = MU_GetCurFolderType();
    iNode = MAIL_GetNodeByHandle(&MailListCurHead, &pDelListNode, (char *)handle);
    if(iNode < 0)
    {
        return FALSE;
    }

    memset(szOutFile, 0x0, PATH_MAXLEN);
    getcwd(szOutFile, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    hFile = GARY_open(pDelListNode->ListMailFileName, O_RDONLY, -1);
    if(hFile < 0)
    {
        printf("\r\nmail_get_detail : open error = %d\r\n", errno);

        chdir(szOutFile);
        return FALSE;
    }
    memset(&detailFile, 0x0, sizeof(MailFile));
    if(read(hFile, &detailFile, sizeof(MailFile)) != sizeof(MailFile))
    {
        GARY_close(hFile);
        chdir(szOutFile);
        return FALSE;
    }
    GARY_close(hFile);
    chdir(szOutFile);
    CreateMailDetailWnd(MuGetFrame(), &detailFile);
    return TRUE;
}

BOOL mail_newmsg_continue(void)
{
    return TRUE;
}

BOOL mail_forward(HWND hwndmu, DWORD handle)
{
    MailListNode *pDelListNode;
    MailFile detailFile;
    int iNode = -1;
    int hFile = -1;
    int TmpBoxType;
    SYSTEMTIME sy;
    
    char *szText;
    char *szAtt;
    char *szNewAtt;
    char szOutFile[PATH_MAXLEN];
    
    szText = NULL;
    szAtt = NULL;
    szNewAtt = NULL;

    TmpBoxType = MU_GetCurFolderType();
    iNode = MAIL_GetNodeByHandle(&MailListCurHead, &pDelListNode, (char *)handle);
    if(iNode < 0)
    {
        return FALSE;
    }
    
    memset(szOutFile, 0x0, PATH_MAXLEN);
    getcwd(szOutFile, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    hFile = GARY_open(pDelListNode->ListMailFileName, O_RDONLY, -1);
    if(hFile < 0)
    {
        printf("\r\nmail_forward : open error = %d\r\n", errno);

        chdir(szOutFile);
        return FALSE;
    }
    memset(&detailFile, 0x0, sizeof(MailFile));
    if(read(hFile, &detailFile, sizeof(MailFile)) != sizeof(MailFile))
    {
        GARY_close(hFile);
        chdir(szOutFile);
        return FALSE;
    }
    if(detailFile.TextSize != 0)
    {
        szText = (char *)malloc(sizeof(char) * detailFile.TextSize + 1);
        if(szText == NULL)
        {
            GARY_close(hFile);
            chdir(szOutFile);
            return FALSE;
        }
        memset(szText, 0x0, sizeof(char) * detailFile.TextSize + 1);
        read(hFile, szText, sizeof(char) * detailFile.TextSize);
    }
    if(detailFile.AttNum > 0)
    {
        szAtt = (char *)malloc(sizeof(char) * detailFile.AttFileNameLen + 1);
        if(szAtt == NULL)
        {
            GARY_FREE(szText);
            GARY_close(hFile);
            chdir(szOutFile);
            return FALSE;
        }
        szNewAtt = (char *)malloc(sizeof(char) * detailFile.AttFileNameLen + detailFile.AttNum + 1);
        if(szNewAtt == NULL)
        {
            GARY_FREE(szAtt);
            GARY_FREE(szText);
            GARY_close(hFile);
            chdir(szOutFile);
            return FALSE;
        }
        memset(szAtt, 0x0, sizeof(char) * detailFile.AttFileNameLen + 1);
        read(hFile, szAtt, sizeof(char) * detailFile.AttFileNameLen);
    }
    GARY_close(hFile);
    chdir(szOutFile);
    
    GetLocalTime(&sy);
    memset(szNewAtt, 0x0, sizeof(char) * detailFile.AttFileNameLen + detailFile.AttNum + 1);
    MAIL_CopyAttachment(szNewAtt, szAtt, detailFile.AttNum);
    CreateMailEditWnd(MuGetFrame(), NULL, NULL, detailFile.Subject, szText, szNewAtt, 
        &sy, -1, -1);
    GARY_FREE(szText);
    GARY_FREE(szAtt);
    GARY_FREE(szNewAtt);

    return TRUE;
}

BOOL mail_reply_from(HWND hwndmu, DWORD handle)
{
    MailListNode *pDelListNode;
    int iNode = -1;
    int TmpBoxType;
    SYSTEMTIME sy;
    
    TmpBoxType = MU_GetCurFolderType();
    iNode = MAIL_GetNodeByHandle(&MailListCurHead, &pDelListNode, (char *)handle);
    if(iNode < 0)
    {
        return FALSE;
    }

    GetLocalTime(&sy);
    CreateMailEditWnd(MuGetFrame(), pDelListNode->ListAddress, NULL, pDelListNode->ListSubject, NULL, NULL, 
        &sy, -1, -1);
    
    return TRUE;
}

BOOL mail_reply_all(HWND hwndmu, DWORD handle)
{
    MailListNode *pDelListNode;
    int iNode = -1;
    int TmpBoxType;
    char szOutFile[PATH_MAXLEN];
    int hFile;
    MailFile detailFile;

    SYSTEMTIME time;
    char *TempName;
    char *ToName;
    char *CcName;
    int NameLen;
    
    TmpBoxType = MU_GetCurFolderType();
    iNode = MAIL_GetNodeByHandle(&MailListCurHead, &pDelListNode, (char *)handle);
    if(iNode < 0)
    {
        return FALSE;
    }

    memset(szOutFile, 0x0, PATH_MAXLEN);
    getcwd(szOutFile, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    hFile = GARY_open(pDelListNode->ListMailFileName, O_RDONLY, -1);
    if(hFile < 0)
    {
        printf("\r\nmail_reply_all : open error = %d\r\n", errno);

        chdir(szOutFile);
        return FALSE;
    }
    memset(&detailFile, 0x0, sizeof(MailFile));
    if(read(hFile, &detailFile, sizeof(MailFile)) != sizeof(MailFile))
    {
        GARY_close(hFile);
        chdir(szOutFile);
        return FALSE;
    }
    GARY_close(hFile);
    chdir(szOutFile);
    
    GetLocalTime(&time);
    if(MAIL_AnalyseMailAddr(detailFile.To) > 0)
    {
        //To
        NameLen = strlen(detailFile.From) + strlen(detailFile.To);
        TempName = malloc(NameLen + 2);
        if(TempName == NULL)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 0);
            return FALSE;
        }
        memset(TempName, 0x0, NameLen);
        strcpy(TempName, detailFile.From);
        if(*(char *)(TempName + strlen(detailFile.From)) != ';')
        {
            strcat(TempName, ";");
        }
        strcat(TempName, detailFile.To);
        if(!MAIL_CombineReplyRec(&ToName, TempName, detailFile.OwnAddress))
        {
            GARY_FREE(TempName);
            
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 0);
            return FALSE;
        }
        GARY_FREE(TempName);
        
        //cc
        NameLen = strlen(detailFile.Cc);
        TempName = malloc(NameLen + 1);
        if(TempName == NULL)
        {
            GARY_FREE(ToName);
            
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 0);
            return FALSE;
        }
        memset(TempName, 0x0, NameLen);
        strcpy(TempName, detailFile.Cc);
        if(!MAIL_CombineReplyRec(&CcName, TempName, detailFile.OwnAddress))
        {
            GARY_FREE(ToName);
            GARY_FREE(TempName);
            
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 0);
            return FALSE;
        }
        GARY_FREE(TempName);
        
        CreateMailEditWnd(MuGetFrame(), ToName, CcName, detailFile.Subject, 
            NULL, NULL, &time, -1, -1);
    }
    else
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 0);
        return FALSE;
    }

    return TRUE;
}

BOOL mail_get_number(HWND hwndmu, DWORD handle)
{
    return TRUE;
}

BOOL mail_resend_message(HWND hwndmu, DWORD handle)
{
    MailSendNode *pSendNode;
    MailListNode *pListNode;
    int iNode = -1;
    int hTemp;
    MailFile *mTemp;
    MU_MsgNode msgnode;
    char szOutPath[PATH_MAXLEN];

    if(GetSIMState() == 0)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSIM, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return FALSE;
    }
    
    if(strlen(GlobalMailConfigActi.GLMailBoxName) == 0)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOMAILBOX, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return FALSE;
    }
    
    iNode = MAIL_GetSendNodebyHandle(&pSendNode, (char *)handle);
    if(iNode < 0)
    {
        return FALSE;
    }

    iNode = MAIL_GetNodeByHandle(&MailListCurHead, &pListNode, (char *)handle);
    if(iNode < 0)
    {
        return FALSE;
    }

    if((GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SEND) &&
        (!mMailSendHead.SendingFlag))
    {
        pSendNode->SendStatus = MU_STU_SENDING;
        pListNode->ListSendFlag = MU_STU_SENDING;
    }
    else
    {
        pSendNode->SendStatus = MU_STU_WAITINGSEND;
        pListNode->ListSendFlag = MU_STU_WAITINGSEND;
    }

    memset(&msgnode, 0x0, sizeof(MU_MsgNode));
    msgnode.msgtype = MU_MSG_EMAIL;
    msgnode.storage_type = MU_STORE_IN_FLASH;
    strncpy(msgnode.addr, pListNode->ListAddress, MU_ADDR_MAX_LEN);
    msgnode.handle = (DWORD)pListNode->ListHandle;
    msgnode.maskdate = pListNode->ListMaskdate;
    strncpy(msgnode.subject, pListNode->ListSubject, MU_SUBJECT_DIS_LEN);
    msgnode.attachment = pListNode->ListAttFlag;
    msgnode.status = pListNode->ListSendFlag;

    SendMessage(UniGlobalHwnd, PWM_MSG_MU_MODIFIED, 
        MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)&msgnode);
    
    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    //save to flash
    hTemp = GARY_open(pListNode->ListMailFileName, O_RDWR, -1);
    if(hTemp >= 0)
    {
        //read mail
        mTemp = malloc(sizeof(MailFile));
        if(mTemp != NULL)
        {
            memset(mTemp, 0x0, sizeof(MailFile));
            if(read(hTemp, mTemp, sizeof(MailFile)) == sizeof(MailFile))
            {
                //modify file
                mTemp->SendStatus = MU_STU_WAITINGSEND;
                lseek(hTemp, 0, SEEK_SET);
                write(hTemp, mTemp, sizeof(MailFile));
            }
            GARY_FREE(mTemp);
        }
        GARY_close(hTemp);
    }
    else
    {
        printf("\r\nmail_resend_message : open error = %d\r\n", errno);
    }
    chdir(szOutPath);

    if(GlobalMailConfigHead.GLNetUsed)
    {
        if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SEND)
        {
            return TRUE;
        }
        else if(GlobalMailConfigActi.GLSendMsg == MAIL_CONNECT_AUTO)
        {
            MailConfigNode *pTemp;
            
            pTemp = GlobalMailConfigHead.pNext;
            while(pTemp)
            {
                if(strcmp(pTemp->GLMailBoxName, GlobalMailConfigActi.GLMailBoxName) == 0)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDING, NULL, Notify_Info, 
                        NULL, NULL, 20);

                    MAIL_AddDialWaitNode(&GlobalMailConfigHead, pTemp, MAIL_CON_TYPE_SEND);
                    return TRUE;
                }
            }
            return FALSE;
        }
    }
    else if(GlobalMailConfigActi.GLSendMsg == MAIL_CONNECT_AUTO)
    {
        GlobalMailConfigHead.GLNetUsed = TRUE;
        GlobalMailConfigHead.GLNetFlag = MAIL_CON_TYPE_SEND;
        strcpy(GlobalMailConfigHead.GLMailBoxName, GlobalMailConfigActi.GLMailBoxName);
        GlobalMailConfigHead.GLConStatus = MAILBOX_CON_IDLE;
        GlobalMailConfigHead.GLConnect = GlobalMailConfigActi.GLConnect;
        
        GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.pNext;
        while(GlobalMailConfigHead.CurConnectConfig)
        {
            if(strcmp(GlobalMailConfigHead.CurConnectConfig->GLMailBoxName,
                GlobalMailConfigActi.GLMailBoxName) == 0)
            {
                break;
            }
            GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.CurConnectConfig->pNext;
        }

        //MAIL_GetToSendDial = FALSE;
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDING, NULL, Notify_Info, 
            NULL, NULL, 20);
        SendMessage(HwndMailSend, WM_MAIL_BEGIN_SEND, 0, 0);
    }

    return TRUE;
}

BOOL mail_defer2send_message(HWND hwndmu, DWORD handle)
{   
    MailSendNode *pSendNode;
    MailListNode *pListNode;
    int iNode = -1;
    int hTemp;
    MailFile *mTemp;
    MU_MsgNode msgnode;
    char szOutPath[PATH_MAXLEN];
    
    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    
    iNode = MAIL_GetSendNodebyHandle(&pSendNode, (char *)handle);
    if(iNode < 0)
    {
        return FALSE;
    }
    pSendNode->SendStatus = MU_STU_SUSPENDED;
    
    iNode = MAIL_GetNodeByHandle(&MailListCurHead, &pListNode, (char *)handle);
    if(iNode < 0)
    {
        return FALSE;
    }
    pListNode->ListSendFlag = MU_STU_SUSPENDED;

    memset(&msgnode, 0x0, sizeof(MU_MsgNode));
    msgnode.msgtype = MU_MSG_EMAIL;
    msgnode.storage_type = MU_STORE_IN_FLASH;
    strncpy(msgnode.addr, pListNode->ListAddress, MU_ADDR_MAX_LEN);
    msgnode.handle = (DWORD)pListNode->ListHandle;
    msgnode.maskdate = pListNode->ListMaskdate;
    strncpy(msgnode.subject, pListNode->ListSubject, MU_SUBJECT_DIS_LEN);
    msgnode.attachment = pListNode->ListAttFlag;
    msgnode.status = pListNode->ListSendFlag;

    SendMessage(UniGlobalHwnd, PWM_MSG_MU_MODIFIED, 
        MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)&msgnode);

    //save to flash
    hTemp = GARY_open(pListNode->ListMailFileName, O_RDWR, -1);
    if(hTemp >= 0)
    {
        //read mail
        mTemp = malloc(sizeof(MailFile));
        if(mTemp != NULL)
        {
            memset(mTemp, 0x0, sizeof(MailFile));
            if(read(hTemp, mTemp, sizeof(MailFile)) == sizeof(MailFile))
            {
                //modify file
                mTemp->SendStatus = MU_STU_SUSPENDED;
                lseek(hTemp, 0, SEEK_SET);
                write(hTemp, mTemp, sizeof(MailFile));
            }
            GARY_FREE(mTemp);
        }
        GARY_close(hTemp);
    }
    else
    {
        printf("\r\nmail_defer2send_message : open error = %d\r\n", errno);
    }
    chdir(szOutPath);
    
    return TRUE;
}

BOOL mail_move_message(HWND hwndmu, DWORD handle, int nFolder)
{
    MailListNode *pDelListNode;
    int iNode = -1;
    int TmpBoxType;
    int hFile;
    MailFile mMailFile;
    
    char szOutFile[PATH_MAXLEN];
    char szNewFile[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    char szTempFile[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    
    TmpBoxType = MU_GetCurFolderType();
    iNode = MAIL_GetNodeByHandle(&MailListCurHead, &pDelListNode, (char *)handle);
    if(iNode < 0)
    {
        return FALSE;
    }
    
    memset(szTempFile, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
    memset(szNewFile, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
    
    switch(nFolder)
    {
    case MU_INBOX:
        sprintf(szNewFile, "%s", MAIL_IN_FILE);
    	break;

    case MU_DRAFT:
        sprintf(szNewFile, "%s", MAIL_DRAFT_FILE);
    	break;

    case MU_SENT:
        sprintf(szNewFile, "%s", MAIL_SEND_FILE);
        break;

    case MU_OUTBOX:
        sprintf(szNewFile, "%s", MAIL_OUT_FILE);
        break;

    default:
        {
            sprintf(szNewFile, "%s", MAIL_FOLDER_FILE_HEAD);
            sprintf(szTempFile, szNewFile, nFolder);
            sprintf(szNewFile, "%s%s", szTempFile, MAIL_FOLDER_FILE_TAIL);
        }
    }
    MAIL_GetNewFileName(szNewFile, MAX_MAIL_SAVE_FILENAME_LEN + 1);
    
    memset(szOutFile, 0x0, PATH_MAXLEN);
    getcwd(szOutFile, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    
    hFile = GARY_open(pDelListNode->ListMailFileName, O_RDWR, -1);
    if(hFile < 0)
    {
        printf("\r\nmail_move_message : open error = %d\r\n", errno);

        chdir(szOutFile);
        return FALSE;
    }
    memset(&mMailFile, 0x0, sizeof(MailFile));
    read(hFile, &mMailFile, sizeof(MailFile));
    if(nFolder == MU_DRAFT)
    {
        mMailFile.ReadFlag = MU_STU_DRAFT;
    }
    else
    {
        mMailFile.ReadFlag = MU_STU_READ;
    }

    lseek(hFile, 0x0, SEEK_SET);
    write(hFile, &mMailFile, sizeof(MailFile));
    GARY_close(hFile);

    rename(pDelListNode->ListMailFileName, szNewFile);
    chdir(szOutFile);

    if(nFolder == MU_DRAFT)
    {
        MailSendNode DelSendNode;

        memset(&DelSendNode, 0x0, sizeof(MailSendNode));
        strcpy(DelSendNode.SendHandle, pDelListNode->ListHandle);
        strcpy(DelSendNode.SendMailFileName, pDelListNode->ListMailFileName);
        DelSendNode.SendStatus = pDelListNode->ListSendFlag;
        DelSendNode.pNext = NULL;
        MAIL_DeleteSendNode(&DelSendNode);
    }

    if(pDelListNode->ListReadFlag == MU_STU_READ)
    {
        MAIL_ModifyCountNode(&GlobalMailCountHead, TmpBoxType, nFolder, 0);
    }
    else
    {
        MAIL_ModifyCountNode(&GlobalMailCountHead, TmpBoxType, nFolder, 1);
    }

    MAIL_DelBoxFileNode(&MailListCurHead, iNode);
    //SendMessage(hwndmu, PWM_MSG_MU_DELETE, 
    //    MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)handle);
    return TRUE;
}

BOOL mail_copy_message(HWND hwndmu, DWORD handle, int nFolder)
{
    MailListNode *pDelListNode;
    BYTE *tempData;
    int iNode = -1;
    int hFile = -1;
    int hf = -1;
    int TmpBoxType;
    struct stat statFl;

    char szOutFile[PATH_MAXLEN];
    char szNewFile[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    char szTempFile[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    
    if(MAIL_IsFlashEnough() == 0)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DRAFTFULL, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return FALSE;
    }
    
    TmpBoxType = MU_GetCurFolderType();
    iNode = MAIL_GetNodeByHandle(&MailListCurHead, &pDelListNode, (char *)handle);
    if(iNode < 0)
    {
        return FALSE;
    }

    memset(szTempFile, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
    memset(szNewFile, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
    sprintf(szNewFile, "%s", MAIL_FOLDER_FILE_HEAD);
    sprintf(szTempFile, szNewFile, nFolder);
    sprintf(szNewFile, "%s%s", szTempFile, MAIL_FOLDER_FILE_TAIL);
    MAIL_GetNewFileName(szNewFile, MAX_MAIL_SAVE_FILENAME_LEN + 1);

    memset(szOutFile, 0x0, PATH_MAXLEN);
    getcwd(szOutFile, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    memset(&statFl, 0x0, sizeof(struct stat));
    stat(pDelListNode->ListMailFileName, &statFl);
    hFile = GARY_open(pDelListNode->ListMailFileName, O_RDONLY, -1);
    if(hFile < 0)
    {
        printf("\r\nmail_copy_message1 : open error = %d\r\n", errno);
        
        chdir(szOutFile);
        return FALSE;
    }
    
    hf = GARY_open(szNewFile, O_RDWR | O_CREAT, S_IRWXU);
    if(hf < 0)
    {
        printf("\r\nmail_copy_message2 : open error = %d\r\n", errno);

        GARY_close(hFile);
        chdir(szOutFile);
        return FALSE;
    }
    memset(&tempData, 0x0, statFl.st_size);
    if(read(hFile, &tempData, statFl.st_size) != (int)statFl.st_size)
    {
        GARY_close(hFile);
        GARY_close(hf);
        chdir(szOutFile);
        return FALSE;
    }
    if(write(hf, &tempData, statFl.st_size) != (int)statFl.st_size)
    {
        GARY_close(hFile);
        GARY_close(hf);
        chdir(szOutFile);
        return FALSE;
    }
    GARY_close(hFile);
    GARY_close(hf);
    chdir(szOutFile);
    MAIL_AddCountNode(&GlobalMailCountHead, nFolder, 0);
    return TRUE;
}

BOOL mail_destroy()
{
    return TRUE;
}

BOOL mail_get_count(int folder, int *pnunread, int* pncount)
{
    CountMailListNode *tmp;

    if(folder == 5)
    {
        *pnunread = 0;
        *pncount = 0;
        
        return TRUE;
    }
    
    tmp = GlobalMailCountHead.pNext;
    while(tmp)
    {
        if(tmp->GLMailType == folder)
        {
            *pnunread = tmp->GLMailUnread;
            *pncount = tmp->GLMailNum;
            return TRUE;
        }
        if((tmp->GLMailType == 5) && (tmp->GLMailFolder == folder))
        {
            *pnunread = tmp->GLMailUnread;
            *pncount = tmp->GLMailNum;
            return TRUE;
        }
        tmp = tmp->pNext;
    }
    *pnunread = 0;
    *pncount = 0;

    return FALSE;
}
BOOL mail_new_folder(int folder)
{
    return TRUE;
}

static BOOL MAIL_GetBoxFileList(MailListHead *pHead, int folder)
{
    char        szBoxFilePath[MAX_MAIL_FILE_PATH_LEN + 1];
    char        *szBoxFileTmpPath;
    int         hFile;

    MailFile    mMailFile;
    MailListNode    *pTemp;

    static struct dirent    *dirinfo = NULL;
    static DIR  *diropen = NULL;

    SYSTEMTIME TmpSysTime;
    FILETIME   TmpFileTime;
    char szOldPath[PATH_MAXLEN];

    //first GARY_FREE box file lisst memory
    MAIL_ReleaseBoxFileList(pHead);

    //get match file information for searching
    //such as "flash2:/message/mail/ml020000*.ml"
    memset(szBoxFilePath, 0x0, (MAX_MAIL_FILE_PATH_LEN + 1));
    switch(folder)
    {
    case MU_INBOX:
        {
            sprintf(szBoxFilePath, "%s", MAIL_IN_FILE_MATCH);
            CurBoxType = MU_INBOX;
        }
    	break;

    case MU_MYFOLDER:
        {
            szBoxFileTmpPath = malloc(MAX_MAIL_FILE_PATH_LEN + 1);
            if(szBoxFileTmpPath == NULL)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
                return FALSE;
            }
            sprintf(szBoxFileTmpPath, "%s", MAIL_FOLDER_FILE_MATCH);
            sprintf(szBoxFilePath, szBoxFileTmpPath, 0);
            GARY_FREE(szBoxFileTmpPath);
            CurBoxType = MU_MYFOLDER;
            CurFolder = 0;
        }
        break;

    case MU_DRAFT:
        {
            sprintf(szBoxFilePath, "%s", MAIL_DRAFT_FILE_MATCH);
            CurBoxType = MU_DRAFT;
        }
    	break;

    case MU_SENT:
        {
            sprintf(szBoxFilePath, "%s", MAIL_SEND_FILE_MATCH);
            CurBoxType = MU_SENT;
        }
        break;

    case MU_OUTBOX:
        {
            sprintf(szBoxFilePath, "%s", MAIL_OUT_FILE_MATCH);
            CurBoxType = MU_OUTBOX;
        }
        break;

    default:
        {
            szBoxFileTmpPath = malloc(MAX_MAIL_FILE_PATH_LEN + 1);
            if(szBoxFileTmpPath == NULL)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL,
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
                return FALSE;
            }
            sprintf(szBoxFileTmpPath, "%s", MAIL_FOLDER_FILE_MATCH);
            sprintf(szBoxFilePath, szBoxFileTmpPath, folder);
            GARY_FREE(szBoxFileTmpPath);
            CurBoxType = MAIL_FOLDER_TYPE;
            CurFolder = folder;
        }
    }

    szOldPath[0] = 0;
    getcwd(szOldPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    diropen = GARY_opendir(MAIL_FILE_PATH);
    if(diropen == NULL)
        return FALSE;

    while(dirinfo = readdir(diropen))
    {
        if(MAIL_FileTypeMatch(dirinfo->d_name, szBoxFilePath))
            continue;

//        MAIL_PopMessage(NULL, ML("find a mail file"));
        hFile = GARY_open(dirinfo->d_name, O_RDONLY, -1);
        if(hFile < 0)
        {
            printf("\r\nMAIL_GetBoxFileList : open error = %d\r\n", errno);
            
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL,
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
            continue;
        }

        memset(&mMailFile, 0x0, sizeof(MailFile));
        if(read(hFile,&mMailFile,sizeof(MailFile)) != sizeof(MailFile))
        {
            GARY_close(hFile);
            continue;
        }
        GARY_close(hFile);
        
        pTemp = (MailListNode *)malloc(sizeof(MailListNode));
        if(pTemp == NULL)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL,
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
            MAIL_ReleaseBoxFileList(pHead);
            GARY_closedir(diropen);
            chdir(szOldPath);
            return FALSE;
        }
        memset(pTemp, 0x0, sizeof(MailListNode));
        sprintf(pTemp->ListMailFileName, "%s", dirinfo->d_name);
        pTemp->ListSize = mMailFile.MailSize;
        sprintf(pTemp->ListHandle, "%s", mMailFile.MsgId);
        sprintf(pTemp->ListAddress, "%s", mMailFile.address);
        sprintf(pTemp->ListSubject, "%s", mMailFile.Subject);
        pTemp->ListAttFlag = (mMailFile.AttNum)?TRUE:FALSE;
        pTemp->ListReadFlag = mMailFile.ReadFlag;
        pTemp->ListSendFlag = mMailFile.SendStatus;

        memcpy(&TmpSysTime, &mMailFile.Date, sizeof(SYSTEMTIME));
        //MAIL_CharToSystem(&mMailFile.Date, &TmpSysTime);
        MSG_STtoFT(&TmpSysTime, &TmpFileTime);
        pTemp->ListMaskdate = TmpFileTime.dwLowDateTime;
        pTemp->pNext = NULL;

        MAIL_AddBoxFileNode(pHead, pTemp);
    }
    GARY_closedir(diropen);
    chdir(szOldPath);

    return TRUE;
}

BOOL MAIL_FileTypeMatch(char* szBoxSavePath, char* szBoxFindPath)
{
    return (strncmp(szBoxSavePath, szBoxFindPath, 8));
}

static void MAIL_ReleaseBoxFileList(MailListHead *pHead)
{
    MailListNode *pTemp;
    
    pTemp = pHead->pNext;
    //only delete first node 
    //until the list is empty
    while(pTemp)
    {
        if(pTemp->ListReadFlag == FALSE)
        {
            pHead->UnReadNum --;
        }
        pHead->pNext = pTemp->pNext;
        GARY_FREE(pTemp);
        pTemp = NULL;
        pHead->TotalNum --;
        pTemp = pHead->pNext;
    }
    pHead->TotalNum = 0;
    pHead->UnReadNum = 0;
    pHead->pNext = NULL;

    return;
}

void MAIL_AddBoxFileNode(MailListHead *pHead, MailListNode *pTempNode)
{
    MailListNode *pPreTemp;
    MailListNode *pCurTemp;

    pTempNode->pNext = NULL;

    //new node should add to pPreTemp->pNext
    pPreTemp = pHead->pNext;
    
    if(pPreTemp == NULL)
    {
        //new node is the first node in list
        pHead->pNext = pTempNode;
        if(pTempNode->ListReadFlag == FALSE)
        {
            pHead->UnReadNum ++;
        }
        pHead->TotalNum ++;

        return;
    }

    //already some other nodes in list
    pPreTemp = pHead->pNext;
    pCurTemp = pPreTemp;
    while (pCurTemp->pNext != NULL)
    {
        //larger datatime , closer to head
        if((pCurTemp->ListMaskdate) > (pTempNode->ListMaskdate))
        {
            //new node is just the previous node of pSubTemp node
            //then pPreTemp->pNext == pTempNode and pTempNode->pNext = pSubTemp
            break;
        }
        pPreTemp = pCurTemp;
        pCurTemp = pPreTemp->pNext;
    }

    //then pPreTemp->pNext == pTempNode and pTempNode->pNext = pSubTemp
    if(pCurTemp == pHead->pNext)
    {
        //new node should be the first node in the list
        pHead->pNext = pTempNode;
        pTempNode->pNext = pCurTemp;
    }
    else if((pCurTemp->ListMaskdate) > (pTempNode->ListMaskdate))
    {
        pPreTemp->pNext = pTempNode;
        pTempNode->pNext = pCurTemp;
    }
    else
    {
        pCurTemp->pNext = pTempNode;
        pTempNode->pNext = NULL;
    }
    //add the node number
    if(pTempNode->ListReadFlag == FALSE)
    {
        pHead->UnReadNum ++;
    }
    pHead->TotalNum ++;

    return;
}

void MAIL_DelBoxFileNode(MailListHead *pHead, int nTempNode)
{
    MailListNode *pPreNode;
    MailListNode *pCurNode;
    int i;

    if(nTempNode > (int)pHead->TotalNum)
    {
        return;
    }

    pCurNode = pHead->pNext;
    if(nTempNode == 0)
    {
        pHead->pNext = pCurNode->pNext;
        if(pCurNode->ListReadFlag == FALSE)
        {
            pHead->UnReadNum --;
        }
        GARY_FREE(pCurNode);
        pHead->TotalNum --;
        return;
    }

    for(i=0;i<nTempNode;i++)
    {
        pPreNode = pCurNode;
        pCurNode = pPreNode->pNext;
    }

    pPreNode->pNext = pCurNode->pNext;
    if(pCurNode->ListReadFlag == FALSE)
    {
        pHead->UnReadNum --;
    }
    GARY_FREE(pCurNode);
    pHead->TotalNum --;
    
    return;
}

static BOOL MAIL_SearchBoxFileNode(MailListHead *pHead, MailListNode **pFileNode, int nTempNode)
{
    int i;
    MailListNode *pTemp;

    if(nTempNode > (int)(pHead->TotalNum - 1))
        return FALSE;

    if(pHead->pNext == NULL)
        return FALSE;
    
    pTemp = pHead->pNext;
    for(i=0; i<nTempNode; i++)
    {
        pTemp = pTemp->pNext;
    }
    *pFileNode = pTemp;

    return TRUE;
}

int MAIL_GetNodeByHandle(MailListHead *pHead, MailListNode **pDelListNode, char *msghandle)
{
    MailListNode *pTempNode;
    int nTemNode;

    pTempNode = pHead->pNext;
    nTemNode = 0;

    while (pTempNode) 
    {
        if(strcmp(pTempNode->ListHandle, msghandle) == 0)
        {
            *pDelListNode = pTempNode;
            return nTemNode;
        }
        nTemNode ++;
        pTempNode = pTempNode->pNext;
    }

    nTemNode = -1;
    return nTemNode;
}

static void MAIL_CountMailList(void)
{
    char szOutPath[PATH_MAXLEN];
    int CmpRes = 0;
    int i, j, k;
    CountMailListNode *tmp;
    int OutNum, OutUnread;

    MailAttachmentHead ExistAtt;
    MailAttachmentHead AllAtt;

    static struct dirent    *dirinfo = NULL;
    static DIR  *diropen = NULL;

    int hFile;
    MailConfig mMailConfig;
    MailFile mMailFile;
    MailConfigNode *pMailConfigNode;

    memset(&GlobalMailCountHead, 0x0, sizeof(CountMailListHead));
    memset(&GlobalMailConfigHead, 0x0, sizeof(MailConfigHeader));
    memset(&GlobalMailConfigActi, 0x0, sizeof(MailConfig));

    MAIL_AddDefCountList(&GlobalMailCountHead);
    GlobalMailConfigHead.StartAdd = (long)(MailConfigNode *)malloc(sizeof(MailConfigNode) * 100);
    if(GlobalMailConfigHead.StartAdd == NULL)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL,
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
        return;
    }
    memset((MailConfigNode *)GlobalMailConfigHead.StartAdd, 0x0, sizeof(MailConfigNode) * 100);

    GlobalMailConfigHead.pNext = NULL;
    GlobalMailConfigHead.pWaitNext = NULL;

    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    diropen = GARY_opendir(MAIL_FILE_PATH);
    if(diropen == NULL)
        return;

    memset(&ExistAtt, 0x0, sizeof(MailAttachmentHead));
    memset(&AllAtt, 0x0, sizeof(MailAttachmentHead));
    
    j = 0;
    k = -1;
    while(dirinfo = readdir(diropen))
    {
        //attachment
        CmpRes = strncmp(dirinfo->d_name, MAIL_ATTACH_FILE_NAME_HEAD, 6);
        if(CmpRes == 0)
        {
            MailAttachmentNode *pNewNode;

            pNewNode = NULL;
            pNewNode = malloc(sizeof(MailAttachmentNode));
            if(pNewNode != NULL)
            {
                memset(pNewNode, 0x0, sizeof(MailAttachmentNode));
                strcpy(pNewNode->AttSaveName, dirinfo->d_name);
                pNewNode->pNext = NULL;
                MAIL_AddDisNode(&AllAtt, pNewNode);
            }
            continue;
        }
        //box
        CmpRes = strncmp(dirinfo->d_name, MAIL_IN_FILE_MATCH, 8);
        if(CmpRes == 0)
        {
            tmp = GlobalMailCountHead.pNext;
            tmp->GLMailNum ++;
            memset(&mMailFile, 0x0, sizeof(MailFile));
            hFile = GARY_open(dirinfo->d_name, O_RDONLY, -1);
            if(hFile < 0)
            {
                printf("\r\nMAIL_CountMailList1 : open error = %d\r\n", errno);

                continue;
            }
            read(hFile, &mMailFile, sizeof(MailFile));
            if(mMailFile.ReadFlag == MU_STU_UNREAD)
            {
                tmp->GLMailUnread ++;
            }
            if(mMailFile.AttNum > 0)
            {
                MailAttachmentNode *pNewNode;
                int z;
                char *FileAtt;

                FileAtt = NULL;
                FileAtt = malloc(mMailFile.AttFileNameLen + 1);
                if(FileAtt != NULL)
                {
                    memset(FileAtt, 0x0, mMailFile.AttFileNameLen + 1);
                    lseek(hFile, sizeof(MailFile) + mMailFile.TextSize, SEEK_SET);
                    read(hFile, FileAtt, mMailFile.AttFileNameLen);
                    for(z=0; z<(int)mMailFile.AttNum; z++)
                    {
                        pNewNode = NULL;
                        pNewNode = (MailAttachmentNode *)malloc(sizeof(MailAttachmentNode));
                        if(pNewNode != NULL)
                        {
                            memset(pNewNode, 0x0, sizeof(MailAttachmentNode));
                            MAIL_GetMailAttachmentSaveName(FileAtt, pNewNode->AttSaveName, z);
                            pNewNode->pNext = NULL;
                            MAIL_AddDisNode(&ExistAtt, pNewNode);
                        }
                    }
                    GARY_FREE(FileAtt);
                }
            }
            GARY_close(hFile);
            continue;
        }
        CmpRes = strncmp(dirinfo->d_name, MAIL_MAIL_FILE_MATCH, 8);
        if(CmpRes == 0)
        {
            memset(&mMailFile, 0x0, sizeof(MailFile));
            hFile = GARY_open(dirinfo->d_name, O_RDONLY, -1);
            if(hFile < 0)
            {
                printf("\r\nMAIL_CountMailList1 : open error = %d\r\n", errno);
                
                continue;
            }
            read(hFile, &mMailFile, sizeof(MailFile));
            if(mMailFile.AttNum > 0)
            {
                MailAttachmentNode *pNewNode;
                int z;
                char *FileAtt;
                
                FileAtt = NULL;
                FileAtt = malloc(mMailFile.AttFileNameLen + 1);
                if(FileAtt != NULL)
                {
                    memset(FileAtt, 0x0, mMailFile.AttFileNameLen + 1);
                    lseek(hFile, sizeof(MailFile) + mMailFile.TextSize, SEEK_SET);
                    read(hFile, FileAtt, mMailFile.AttFileNameLen);
                    for(z=0; z<(int)mMailFile.AttNum; z++)
                    {
                        pNewNode = NULL;
                        pNewNode = (MailAttachmentNode *)malloc(sizeof(MailAttachmentNode));
                        if(pNewNode != NULL)
                        {
                            memset(pNewNode, 0x0, sizeof(MailAttachmentNode));
                            MAIL_GetMailAttachmentSaveName(FileAtt, pNewNode->AttSaveName, z);
                            pNewNode->pNext = NULL;
                            MAIL_AddDisNode(&ExistAtt, pNewNode);
                        }
                    }
                    GARY_FREE(FileAtt);
                }
            }
            GARY_close(hFile);
            continue;
        }
        CmpRes = strncmp(dirinfo->d_name, MAIL_DRAFT_FILE_MATCH, 8);
        if(CmpRes == 0)
        {
            tmp = GlobalMailCountHead.pNext;
            for(i=0; i<2; i++)
            {
                tmp = tmp->pNext;
            }
            tmp->GLMailNum ++;
            
            memset(&mMailFile, 0x0, sizeof(MailFile));
            hFile = GARY_open(dirinfo->d_name, O_RDONLY, -1);
            if(hFile < 0)
            {
                printf("\r\nMAIL_CountMailList1 : open error = %d\r\n", errno);
                
                continue;
            }
            read(hFile, &mMailFile, sizeof(MailFile));
            if(mMailFile.AttNum > 0)
            {
                MailAttachmentNode *pNewNode;
                int z;
                char *FileAtt;
                
                FileAtt = NULL;
                FileAtt = malloc(mMailFile.AttFileNameLen + 1);
                if(FileAtt != NULL)
                {
                    memset(FileAtt, 0x0, mMailFile.AttFileNameLen + 1);
                    lseek(hFile, sizeof(MailFile) + mMailFile.TextSize, SEEK_SET);
                    read(hFile, FileAtt, mMailFile.AttFileNameLen);
                    for(z=0; z<(int)mMailFile.AttNum; z++)
                    {
                        pNewNode = NULL;
                        pNewNode = (MailAttachmentNode *)malloc(sizeof(MailAttachmentNode));
                        if(pNewNode != NULL)
                        {
                            memset(pNewNode, 0x0, sizeof(MailAttachmentNode));
                            MAIL_GetMailAttachmentSaveName(FileAtt, pNewNode->AttSaveName, z);
                            pNewNode->pNext = NULL;
                            MAIL_AddDisNode(&ExistAtt, pNewNode);
                        }
                    }
                    GARY_FREE(FileAtt);
                }
            }
            GARY_close(hFile);
            continue;
        }
        CmpRes = strncmp(dirinfo->d_name, MAIL_SEND_FILE_MATCH, 8);
        if(CmpRes == 0)
        {
            tmp = GlobalMailCountHead.pNext;
            for(i=0; i<3; i++)
            {
                tmp = tmp->pNext;
            }
            tmp->GLMailNum ++;

            memset(&mMailFile, 0x0, sizeof(MailFile));
            hFile = GARY_open(dirinfo->d_name, O_RDONLY, -1);
            if(hFile < 0)
            {
                printf("\r\nMAIL_CountMailList1 : open error = %d\r\n", errno);
                
                continue;
            }
            read(hFile, &mMailFile, sizeof(MailFile));
            if(mMailFile.AttNum > 0)
            {
                MailAttachmentNode *pNewNode;
                int z;
                char *FileAtt;
                
                FileAtt = NULL;
                FileAtt = malloc(mMailFile.AttFileNameLen + 1);
                if(FileAtt != NULL)
                {
                    memset(FileAtt, 0x0, mMailFile.AttFileNameLen + 1);
                    lseek(hFile, sizeof(MailFile) + mMailFile.TextSize, SEEK_SET);
                    read(hFile, FileAtt, mMailFile.AttFileNameLen);
                    for(z=0; z<(int)mMailFile.AttNum; z++)
                    {
                        pNewNode = NULL;
                        pNewNode = (MailAttachmentNode *)malloc(sizeof(MailAttachmentNode));
                        if(pNewNode != NULL)
                        {
                            memset(pNewNode, 0x0, sizeof(MailAttachmentNode));
                            MAIL_GetMailAttachmentSaveName(FileAtt, pNewNode->AttSaveName, z);
                            pNewNode->pNext = NULL;
                            MAIL_AddDisNode(&ExistAtt, pNewNode);
                        }
                    }
                    GARY_FREE(FileAtt);
                }
            }
            GARY_close(hFile);
            continue;
        }
        CmpRes = strncmp(dirinfo->d_name, MAIL_OUT_FILE_MATCH, 8);
        if(CmpRes == 0)
        {
            tmp = GlobalMailCountHead.pNext;
            for(i=0; i<4; i++)
            {
                tmp = tmp->pNext;
            }
            tmp->GLMailNum ++;

            memset(&mMailFile, 0x0, sizeof(MailFile));
            hFile = GARY_open(dirinfo->d_name, O_RDONLY, -1);
            if(hFile < 0)
            {
                printf("\r\nMAIL_CountMailList1 : open error = %d\r\n", errno);
                
                continue;
            }
            read(hFile, &mMailFile, sizeof(MailFile));
            if(mMailFile.AttNum > 0)
            {
                MailAttachmentNode *pNewNode;
                int z;
                char *FileAtt;
                
                FileAtt = NULL;
                FileAtt = malloc(mMailFile.AttFileNameLen + 1);
                if(FileAtt != NULL)
                {
                    memset(FileAtt, 0x0, mMailFile.AttFileNameLen + 1);
                    lseek(hFile, sizeof(MailFile) + mMailFile.TextSize, SEEK_SET);
                    read(hFile, FileAtt, mMailFile.AttFileNameLen);
                    for(z=0; z<(int)mMailFile.AttNum; z++)
                    {
                        pNewNode = NULL;
                        pNewNode = (MailAttachmentNode *)malloc(sizeof(MailAttachmentNode));
                        if(pNewNode != NULL)
                        {
                            memset(pNewNode, 0x0, sizeof(MailAttachmentNode));
                            MAIL_GetMailAttachmentSaveName(FileAtt, pNewNode->AttSaveName, z);
                            pNewNode->pNext = NULL;
                            MAIL_AddDisNode(&ExistAtt, pNewNode);
                        }
                    }
                    GARY_FREE(FileAtt);
                }
            }
            GARY_close(hFile);
            continue;
        }
        CmpRes = strncmp(dirinfo->d_name, MAIL_FOLDER_FILE_MATCH, 4);
        if(CmpRes == 0)
        {
            int iFolder;
            BOOL Unread;
            char *temp;
            char chFile[5];

            temp = dirinfo->d_name;
            temp += 4;
            memset(chFile, 0x0, 5);
            strncpy(chFile, temp, 4);
            iFolder = atoi(chFile);

            hFile = GARY_open(dirinfo->d_name, O_RDONLY, -1);
            if(hFile < 0)
            {
                printf("\r\nMAIL_CountMailList2 : open error = %d\r\n", errno);

                continue;
            }
            read(hFile, &mMailFile, sizeof(MailFile));
            if(mMailFile.ReadFlag == MU_STU_UNREAD)
            {
                Unread = TRUE;
            }
            else
            {
                Unread = FALSE;
            }
            
            if(mMailFile.AttNum > 0)
            {
                MailAttachmentNode *pNewNode;
                int z;
                char *FileAtt;
                
                FileAtt = NULL;
                FileAtt = malloc(mMailFile.AttFileNameLen + 1);
                if(FileAtt != NULL)
                {
                    memset(FileAtt, 0x0, mMailFile.AttFileNameLen + 1);
                    lseek(hFile, sizeof(MailFile) + mMailFile.TextSize, SEEK_SET);
                    read(hFile, FileAtt, mMailFile.AttFileNameLen);
                    for(z=0; z<(int)mMailFile.AttNum; z++)
                    {
                        pNewNode = NULL;
                        pNewNode = (MailAttachmentNode *)malloc(sizeof(MailAttachmentNode));
                        if(pNewNode != NULL)
                        {
                            memset(pNewNode, 0x0, sizeof(MailAttachmentNode));
                            MAIL_GetMailAttachmentSaveName(FileAtt, pNewNode->AttSaveName, z);
                            pNewNode->pNext = NULL;
                            MAIL_AddDisNode(&ExistAtt, pNewNode);
                        }
                    }
                    GARY_FREE(FileAtt);
                }
            }
            GARY_close(hFile);
            
            MAIL_AddCountNode(&GlobalMailCountHead, iFolder, Unread);
            continue;
        }

        //mailbox
        if(strncmp(dirinfo->d_name, MAIL_CONFIG_FILE_MATCH, 8))
        {
            continue;
        }
        
        hFile = GARY_open(dirinfo->d_name, O_RDONLY, -1);
        if(hFile < 0)
        {
            printf("\r\nMAIL_CountMailList3 : open error = %d\r\n", errno);

            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL,
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
            continue;
        }
        memset(&mMailConfig, 0x0, sizeof(MailConfig));
        if(read(hFile, &mMailConfig, sizeof(MailConfig)) != sizeof(MailConfig))
        {
            GARY_close(hFile);
            continue;
        }
        GARY_close(hFile);
        
        pMailConfigNode = (MailConfigNode *)(GlobalMailConfigHead.StartAdd + 
            sizeof(MailConfigNode) * GlobalMailConfigHead.len);
        memset(pMailConfigNode, 0x0, sizeof(MailConfigNode));

        sprintf(pMailConfigNode->GLMailBoxSaveName, "%s", dirinfo->d_name);
        pMailConfigNode->GLMailBoxID = j;

        MAIL_InitConfigNode(&GlobalMailConfigHead, pMailConfigNode, &mMailConfig);
        
        if(pMailConfigNode->GLDefSelect == MAIL_DEF_SELECTED)
        {
            memcpy(&GlobalMailConfigActi, &mMailConfig, sizeof(MailConfig));
            k = j;
        }
        
        pMailConfigNode->pNext = NULL;
        
        MAIL_AddConfigNode(&GlobalMailConfigHead, pMailConfigNode);
        GlobalMailConfigHead.bUsed[j] = TRUE;
        j++;
    }
    GARY_closedir(diropen);
    if(AllAtt.Num > 0)
    {
        MailAttachmentNode *pNowExNode;
        MailAttachmentNode *pNowAlNode;

        pNowAlNode = NULL;
        pNowExNode = NULL;
        pNowExNode = ExistAtt.pNext;
        if(pNowExNode != NULL)
        {
            while(ExistAtt.Num > 0)
            {
                if(pNowExNode == NULL)
                {
                    break;
                }
                pNowAlNode = AllAtt.pNext;
                while(pNowAlNode)
                {
                    if(strcmp(pNowExNode->AttSaveName, pNowAlNode->AttSaveName) == 0)
                    {
                        MAIL_DelAttachmentNode(&ExistAtt, pNowExNode);
                        pNowExNode = NULL;
                        MAIL_DelAttachmentNode(&AllAtt, pNowAlNode);
                        pNowAlNode = NULL;

                        break;
                    }
                    pNowAlNode = pNowAlNode->pNext;
                }
                if(pNowExNode == NULL)
                {
                    pNowExNode = ExistAtt.pNext;
                }
                else
                {
                    pNowExNode = pNowExNode->pNext;
                }
            }
        }
        
        while(AllAtt.Num > 0)
        {
            pNowAlNode = NULL;
            pNowAlNode = AllAtt.pNext;
            if(pNowAlNode == NULL)
            {
                break;
            }
            remove(pNowAlNode->AttSaveName);
            MAIL_DelAttachmentNode(&AllAtt, pNowAlNode);
        }
    }
    chdir(szOutPath);
    MAIL_ReleaseDisList(&ExistAtt);
    MAIL_ReleaseDisList(&AllAtt);

    GlobalMailConfigHead.DefSelInt = k;
    MAIL_InitMailBoxCount(&GlobalMailConfigHead);

    OutNum = 0;
    OutUnread = 0;
    mail_get_count(MU_OUTBOX, &OutUnread, &OutNum);

    MsgNotify(MU_MAIL_NOTIFY, FALSE, GlobalMailNumber.TotalUnread, OutNum);

    return;
}

static void MAIL_AddDefCountList(CountMailListHead *pHead)
{
    CountMailListNode *tmp;
    CountMailListNode *TmpEnd;
    int i;

    pHead->StartAdd = (long)(CountMailListNode *)malloc(sizeof(CountMailListNode) * 100);
    if(pHead->StartAdd == NULL)
    {
        return;
    }
    memset((CountMailListNode *)pHead->StartAdd, 0x0, sizeof(CountMailListNode) * 100);

    for(i=0; i<5; i++)
    {
        tmp = NULL;
        tmp = (CountMailListNode *)(pHead->StartAdd + sizeof(CountMailListNode) * pHead->TotalNum);
        memset(tmp, 0x0, sizeof(CountMailListNode));
        
        tmp->GLMailNum = 0;
        tmp->GLMailType = i;
        tmp->GLMailFolder = -1;
        tmp->pNext = NULL;

        TmpEnd = pHead->pNext;
        if(TmpEnd == NULL)
        {
            pHead->pNext = tmp;
        }
        else
        {
            while(TmpEnd->pNext)
            {
                TmpEnd = TmpEnd->pNext;
            }
            TmpEnd->pNext = tmp;
        }
        pHead->TotalNum ++;
    }
    return;
}

void MAIL_AddCountNode(CountMailListHead *pHead, int MailNum, BOOL UnRead)
{
    CountMailListNode *tmp;
    CountMailListNode *TmpEnd;

    tmp = pHead->pNext;
    while(tmp)
    {
        if(tmp->GLMailType == MailNum)
        {
            tmp->GLMailNum ++;
            GlobalMailNumber.TotalNum ++;
            if(UnRead)
            {
                tmp->GLMailUnread ++;
                GlobalMailNumber.TotalUnread ++;
                MsgNotify(MU_MAIL_NOTIFY, FALSE, GlobalMailNumber.TotalUnread, -1);
            }
            if(MailNum == MU_OUTBOX)
            {
                MsgNotify(MU_MAIL_NOTIFY, FALSE, -1, tmp->GLMailNum);
            }
            return;
        }
        else if((tmp->GLMailType == 5) && (tmp->GLMailFolder == MailNum))
        {
            tmp->GLMailNum ++;
            GlobalMailNumber.TotalNum ++;
            if(UnRead)
            {
                tmp->GLMailUnread ++;
                GlobalMailNumber.TotalUnread ++;
                MsgNotify(MU_MAIL_NOTIFY, FALSE, GlobalMailNumber.TotalUnread, -1);
            }
            return;
        }
        tmp = tmp->pNext;
    }

    tmp = (CountMailListNode *)(pHead->StartAdd + sizeof(CountMailListNode) * pHead->TotalNum);
    memset(tmp, 0x0, sizeof(CountMailListNode));

    tmp->GLMailNum = 1;
    tmp->GLMailType = 5;
    tmp->GLMailFolder = MailNum;

    GlobalMailNumber.TotalNum ++;
    if(UnRead)
    {
        tmp->GLMailUnread ++;
        GlobalMailNumber.TotalUnread ++;
        MsgNotify(MU_MAIL_NOTIFY, FALSE, GlobalMailNumber.TotalUnread, -1);
    }
    
    tmp->pNext = NULL;
    
    TmpEnd = pHead->pNext;
    if(TmpEnd == NULL)
    {
        pHead->pNext = tmp;
    }
    else
    {
        while(TmpEnd->pNext)
        {
            TmpEnd = TmpEnd->pNext;
        }
        TmpEnd->pNext = tmp;
    }
    pHead->TotalNum ++;

    return;
}

void MAIL_DelCountNode(CountMailListHead *pHead, int MailNum, BOOL UnRead)
{
    CountMailListNode *tmp;
    
    tmp = pHead->pNext;
    while(tmp)
    {
        if(tmp->GLMailType == MailNum)
        {
            if(tmp->GLMailNum > 0)
            {
                tmp->GLMailNum --;
                GlobalMailNumber.TotalNum --;
            }
            if((tmp->GLMailUnread > 0) && (UnRead))
            {
                tmp->GLMailUnread --;
                GlobalMailNumber.TotalUnread --;
                MsgNotify(MU_MAIL_NOTIFY, FALSE, GlobalMailNumber.TotalUnread, -1);
            }
            if(MailNum == MU_OUTBOX)
            {
                MsgNotify(MU_MAIL_NOTIFY, FALSE, -1, tmp->GLMailNum);
            }
            return;
        }
        else if((tmp->GLMailType == 5) && (tmp->GLMailFolder == MailNum))
        {
            
            if(tmp->GLMailNum > 0)
            {
                tmp->GLMailNum --;
                GlobalMailNumber.TotalNum --;
            }
            if((tmp->GLMailUnread > 0) && (UnRead))
            {
                tmp->GLMailUnread --;
                GlobalMailNumber.TotalUnread --;
                MsgNotify(MU_MAIL_NOTIFY, FALSE, GlobalMailNumber.TotalUnread, -1);
            }
            return;
        }
        tmp = tmp->pNext;
    }
    
    return;
}

void MAIL_AddUnreadNode(CountMailListHead *pHead, int MailNum)
{
    CountMailListNode *tmp;
    
    tmp = pHead->pNext;
    while(tmp)
    {
        if(tmp->GLMailType == MailNum)
        {
            tmp->GLMailUnread ++;
            GlobalMailNumber.TotalUnread ++;
            MsgNotify(MU_MAIL_NOTIFY, FALSE, GlobalMailNumber.TotalUnread, -1);
            return;
        }
        else if((tmp->GLMailType == 5) && (tmp->GLMailFolder == MailNum))
        {
            tmp->GLMailUnread ++;
            GlobalMailNumber.TotalUnread ++;
            MsgNotify(MU_MAIL_NOTIFY, FALSE, GlobalMailNumber.TotalUnread, -1);
            return;
        }
        tmp = tmp->pNext;
    }
    
    return;
}

void MAIL_DelUnreadNode(CountMailListHead *pHead, int MailNum)
{
    CountMailListNode *tmp;
    
    tmp = pHead->pNext;
    while(tmp)
    {
        if(tmp->GLMailType == MailNum)
        {
            if(tmp->GLMailUnread > 0)
            {
                tmp->GLMailUnread --;
                GlobalMailNumber.TotalUnread --;
                MsgNotify(MU_MAIL_NOTIFY, FALSE, GlobalMailNumber.TotalUnread, -1);
            }
            return;
        }
        else if((tmp->GLMailType == 5) && (tmp->GLMailFolder == MailNum))
        {
            if(tmp->GLMailUnread < 0)
            {
                tmp->GLMailUnread --;
                GlobalMailNumber.TotalUnread --;
                MsgNotify(MU_MAIL_NOTIFY, FALSE, GlobalMailNumber.TotalUnread, -1);
            }
            return;
        }
        tmp = tmp->pNext;
    }
    
    return;
}

void MAIL_ModifyCountNode(CountMailListHead *pHead, int MailNum, int NewFol, BOOL Unread)
{
    MAIL_DelCountNode(pHead, MailNum, Unread);
    MAIL_AddCountNode(pHead, NewFol, Unread);
    
    return;
}

void MAIL_DelCountFolder(CountMailListHead *pHead, int MailNum)
{
    CountMailListNode *tmp;
    
    tmp = pHead->pNext;
    while(tmp)
    {
        if(tmp->GLMailType == MailNum)
        {
            if(tmp->GLMailNum > 0)
            {
                
                GlobalMailNumber.TotalNum -= tmp->GLMailNum;
            }
            if(tmp->GLMailUnread > 0)
            {
                GlobalMailNumber.TotalUnread -= tmp->GLMailUnread;
                MsgNotify(MU_MAIL_NOTIFY, FALSE, GlobalMailNumber.TotalUnread, -1);
            }
            if(MailNum == MU_OUTBOX)
            {
                MsgNotify(MU_MAIL_NOTIFY, FALSE, -1, tmp->GLMailNum);
            }
            tmp->GLMailUnread = 0;
            tmp->GLMailNum = 0;

            return;
        }
        else if((tmp->GLMailType == 5) && (tmp->GLMailFolder == MailNum))
        {
            if(tmp->GLMailNum > 0)
            {
                GlobalMailNumber.TotalNum -= tmp->GLMailNum;
            }
            if(tmp->GLMailUnread > 0)
            {
                GlobalMailNumber.TotalUnread -= tmp->GLMailUnread;
                MsgNotify(MU_MAIL_NOTIFY, FALSE, GlobalMailNumber.TotalUnread, -1);
            }
            tmp->GLMailNum = 0;
            tmp->GLMailUnread = 0;

            return;
        }
        tmp = tmp->pNext;
    }
    
    return;
}

static void MAIL_InitMailBoxCount(MailConfigHeader *pHead)
{
    MailConfigNode *pCurNode;
    MailGetSaveFile TmpGetSaveFile;
    char szOutPath[PATH_MAXLEN];
    int hFile;

    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    pCurNode = pHead->pNext;
    while(pCurNode)
    {
        pCurNode->GLMailUnread = 0;
        pCurNode->GLMailNum = 0;
        
        hFile = GARY_open(pCurNode->GLMailConfigSaveName, O_RDONLY, -1);
        if(hFile < 0)
        {
            printf("\r\nMAIL_InitMailBoxCount : open error = %d\r\n", errno);

            chdir(szOutPath);
            return;
        }
        
        memset(&TmpGetSaveFile, 0x0, sizeof(MailGetSaveFile));
        while(read(hFile, &TmpGetSaveFile, sizeof(MailGetSaveFile)) == sizeof(MailGetSaveFile)) 
        {
            if(TmpGetSaveFile.GetLocalFlag)
            {
                pCurNode->GLMailNum ++;
                if((TmpGetSaveFile.GetReadFlag == UNREAD) && (TmpGetSaveFile.GetStatus != UNDOWN))
                {
                    pCurNode->GLMailUnread ++;
                }
            }
        }
        GARY_close(hFile);

        GlobalMailNumber.TotalNum += pCurNode->GLMailNum;
        GlobalMailNumber.TotalUnread += pCurNode->GLMailUnread;
        
        pCurNode = pCurNode->pNext;
    }
    chdir(szOutPath);
    return;
}

void MAIL_ModifyMailBoxCount(char *MailboxName, int Unread, int boxnum)
{
    MailConfigNode *pCurNode;

    pCurNode = GlobalMailConfigHead.pNext;
    while(pCurNode)
    {
        if(strcmp(pCurNode->GLMailBoxName, MailboxName) == 0)
        {
            pCurNode->GLMailNum += boxnum;
            pCurNode->GLMailUnread += Unread;
            if(pCurNode->GLMailNum < 0)
            {
                pCurNode->GLMailNum = 0;
            }
            if(pCurNode->GLMailUnread)
            {
                pCurNode->GLMailUnread = 0;
            }

            GlobalMailNumber.TotalNum += boxnum;
            GlobalMailNumber.TotalUnread += Unread;

            return;
        }
        pCurNode = pCurNode->pNext;
    }
    return;
}

void MAIL_ReinstallMailBoxCount(char *MailboxName, int Unread, int boxnum)
{
    MailConfigNode *pCurNode;
    
    pCurNode = GlobalMailConfigHead.pNext;
    while(pCurNode)
    {
        if(strcmp(pCurNode->GLMailBoxName, MailboxName) == 0)
        {
            GlobalMailNumber.TotalNum -= pCurNode->GLMailNum;
            GlobalMailNumber.TotalUnread -= pCurNode->GLMailUnread;

            pCurNode->GLMailNum = boxnum;
            pCurNode->GLMailUnread = Unread;

            GlobalMailNumber.TotalNum += boxnum;
            GlobalMailNumber.TotalUnread += Unread;

            if(pCurNode->GLMailNum < 0)
            {
                pCurNode->GLMailNum = 0;
            }
            if(pCurNode->GLMailUnread)
            {
                pCurNode->GLMailUnread = 0;
            }
            return;
        }
        pCurNode = pCurNode->pNext;
    }
    return;
}

void MAIL_InstallMailBoxCount(char *MailboxName)
{
    MailConfigNode *pCurNode;
    MailGetSaveFile TmpGetSaveFile;
    char szOutPath[PATH_MAXLEN];
    int hFile;
    
    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    
    pCurNode = GlobalMailConfigHead.pNext;
    while(pCurNode)
    {
        if(strcmp(pCurNode->GLMailBoxName, MailboxName) == 0)
        {
            GlobalMailNumber.TotalNum -= pCurNode->GLMailNum;
            GlobalMailNumber.TotalUnread -= pCurNode->GLMailUnread;
            
            pCurNode->GLMailUnread = 0;
            pCurNode->GLMailNum = 0;
        
            hFile = GARY_open(pCurNode->GLMailConfigSaveName, O_RDONLY, -1);
            if(hFile < 0)
            {
                printf("\r\nMAIL_InstallMailBoxCount : open error = %d\r\n", errno);

                chdir(szOutPath);
                return;
            }
        
            memset(&TmpGetSaveFile, 0x0, sizeof(MailGetSaveFile));
            while(read(hFile, &TmpGetSaveFile, sizeof(MailGetSaveFile)) == sizeof(MailGetSaveFile)) 
            {
                if(TmpGetSaveFile.GetLocalFlag)
                {
                    pCurNode->GLMailNum ++;
                    if((TmpGetSaveFile.GetReadFlag == UNREAD) && (TmpGetSaveFile.GetStatus != UNDOWN))
                    {
                        pCurNode->GLMailUnread ++;
                    }
                }
            }
            GARY_close(hFile);
            chdir(szOutPath);

            GlobalMailNumber.TotalNum += pCurNode->GLMailNum;
            GlobalMailNumber.TotalUnread += pCurNode->GLMailUnread;
            
            return;
        }
        pCurNode = pCurNode->pNext;
    }

    chdir(szOutPath);
    return;
}

//mailbox
//unibox interface
BOOL emailbox_register(MU_IEmailbox **Iemailbox)
{
    *Iemailbox = &mu_mailbox;
    
    return TRUE;
}

BOOL msg_registerrefreshmailbox(HWND hWnd, UINT wMsgCmd)
{
    UniGlobalHwnd = hWnd;
    UniGlobalMsg = wMsgCmd;
    
    return TRUE;
}

static BOOL mailbox_get_mailboxinfo(MU_EMAILBOXINFO *pEmailBoxInfo, int* nMailboxCount)
{
    MailConfigNode *RetConfigNode;
    int i;
    MU_EMAILBOXINFO *temp;

    if(pEmailBoxInfo == NULL)
    {
        if(GlobalMailConfigHead.len == 0)
        {
            *nMailboxCount = 1;
        }
        else
        {
            *nMailboxCount = GlobalMailConfigHead.len;
        }
    }
    else
    {
        temp = pEmailBoxInfo;
        RetConfigNode = GlobalMailConfigHead.pNext;
        if(GlobalMailConfigHead.pNext == NULL)
        {
            strcpy(temp->szEmailBoxName, IDP_MAIL_TITLE_MAILBOX);
        }
        else
        {
            for(i = 0; i < *nMailboxCount; i++)
            {
                strncpy(temp->szEmailBoxName, RetConfigNode->GLMailBoxName, MU_EMAILBOX_NAME_LEN - 1);
                temp ++;
                RetConfigNode = RetConfigNode->pNext;
            }
        }
        
    }
    return TRUE;
}
static BOOL mailbox_read(HWND hwndmu, MU_EMAILBOXINFO *pEmailBoxInfo)
{
    if(GlobalMailConfigHead.len == 0)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOMAILSET, IDP_MAIL_TITLE_MAILBOX, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
    }
    else
    {
        if(CreateMailInBoxWnd(hwndmu, pEmailBoxInfo->szEmailBoxName))
        {
            return TRUE;
        }
        else
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOMAILSET, IDP_MAIL_TITLE_MAILBOX, 
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        }
    }

    return FALSE;
}
BOOL mailbox_read_default(HWND hwndmu)
{
    if(GlobalMailConfigHead.len == 0)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOMAILSET, IDP_MAIL_TITLE_MAILBOX, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        return FALSE;
    }
    if(strlen(GlobalMailConfigActi.GLMailBoxName) == 0)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOMAILSET, IDP_MAIL_TITLE_MAILBOX, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        return FALSE;
    }

    if(CreateMailInBoxWnd(hwndmu, GlobalMailConfigActi.GLMailBoxName))
    {
        return TRUE;
    }
    else
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOMAILSET, IDP_MAIL_TITLE_MAILBOX, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        return FALSE;
    }   
}

BOOL mailbox_get_all_count(int* pnunread, int* pncount)
{
    *pncount = GlobalMailNumber.TotalNum;
    *pnunread = GlobalMailNumber.TotalUnread;

    return TRUE;
}

static BOOL mailbox_get_count(MU_EMAILBOXINFO *pEmailBoxInfo, int* pnunread, int* pncount)
{
    MailConfigNode *tmp;
    
    tmp = GlobalMailConfigHead.pNext;
    while(tmp)
    {
        if(strcmp(tmp->GLMailBoxName, pEmailBoxInfo->szEmailBoxName) == 0)
        {
            *pnunread = tmp->GLMailUnread;
            *pncount = tmp->GLMailNum;

            return TRUE;
        }
        tmp = tmp->pNext;
    }
    *pnunread = 0;
    *pncount = 0;
    
    return FALSE;
}

BOOL MAIL_IsNetWork(void)
{
    if(GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

int mail_GetRecNum(HWND hwndmu, DWORD msghandle)
{
    MailListNode *pListNode;
    int iNode = -1;
    int iReturn;
    
    iReturn = 0;
    iNode = MAIL_GetNodeByHandle(&MailListCurHead, &pListNode, (char *)msghandle);
    if(iNode < 0)
    {
        return iReturn;
    }

    iReturn = MAIL_AnalyseMailAddr(pListNode->ListAddress);

    return iReturn;
}

LONG APP_GetEmailSize(void)
{
    static struct dirent    *dirinfo = NULL;
    static DIR  *diropen = NULL;
    struct stat SourceStat;

    char szOldPath[PATH_MAXLEN];

    LONG lsize = 0;
    int lCount = 0;
  
    memset(szOldPath, 0x0, PATH_MAXLEN);
    getcwd(szOldPath, PATH_MAXLEN);  
    chdir(MAIL_FILE_PATH); 

    diropen = GARY_opendir(MAIL_FILE_PATH);
    if(diropen == NULL)
        return 0;
    
    while(dirinfo = readdir(diropen))
    {
        if(stat(dirinfo->d_name, &SourceStat) == -1)
        {
            GARY_closedir(diropen);
            chdir(szOldPath);
            return 0;
        }
        if(SourceStat.st_mode & S_IFDIR)
        {
            continue;
        }
        lsize += SourceStat.st_size;
        lCount ++;
    }
    GARY_closedir(diropen);
    chdir(szOldPath);
    return lsize;
}


