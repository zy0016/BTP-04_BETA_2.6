/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailGet.c
*
* Purpose  : 
*
\**************************************************************************/

#include "MailHeader.h"

MailGetListHead mMailGetListHead;       //get from savefile
extern int errno;

//individual
BOOL RegisterMailGetClass();
void UnregisterMailGetClass();
BOOL CreateMailGetWnd();

static LRESULT MailGetWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                              LPARAM lParam);
static void MAIL_DealGetMessage(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void MAIL_DealGetCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void MAIL_DealGetTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void MAIL_DealCancelReturn(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

void MAIL_ReleaseConfigList(MailConfigHeader *pHead);
void MAIL_AddConfigNode(MailConfigHeader *pHead, MailConfigNode *pNewNode);
void MAIL_DelConfigNode(MailConfigHeader *pHead, int nTempNode);
void MAIL_RemoveConfigNode(MailConfigHeader *pHead, int nTempNode);
BOOL MAIL_GetConfigNodebyInt(MailConfigHeader *pHead, MailConfigNode *pConfigNode, int iTmp);
int MAIL_GetConfigNodebyName(MailConfigHeader *pHead, MailConfigNode *pConfigNode, const char *szTmpName);
int MAIL_GetConfigNodebyID(MailConfigHeader *pHead, MailConfigNode **pConfigNode, int iTmp);

BOOL MAIL_SaveMailContent(int hMailObj, char *DownMailName, char *BoxName, int iReadFileSize, BOOL *bAtt);
char *Mail_UTF8DecodeOnString(char *pSrc, int nSize);
BOOL MAIL_ConnectDial(HWND hWnd, MailConfigNode *pConConfig);

extern void MAIL_GetServerHead();
extern void MAIL_DelServerHead();
extern void MAIL_RetrieveDigest(BOOL bSucc);
extern void MAIL_QuitServerHead();
extern BOOL MAIL_ConnectServer(HWND hWnd, BYTE bConType);
extern BOOL MAIL_GetHeaderFromServer(HWND hWnd, int start, int num);

extern void MAIL_PLXTipsWin(const char *cTips, NOTIFYTYPE Tipspic);

/******************************************************************** 
  * Function	:	RegisteMailGetClass 
  *	      
  * Purpose	    :        
  *           
\**********************************************************************/
BOOL RegisterMailGetClass()
{
    WNDCLASS wc;
    
    wc.style            = 0;
    wc.lpfnWndProc      = MailGetWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.hbrBackground    = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "MailGetWndClass";
    
    if(!RegisterClass(&wc))
        return FALSE;
    else
        return TRUE;
}

/******************************************************************** 
  * Function	:	UnRegisteMailGetClass  
  *	       
  * Purpose	    :        
  *        
  * Remarks	    :             
\**********************************************************************/
void UnregisterMailGetClass()
{
    UnregisterClass("MailGetWndClass", NULL);
}

/******************************************************************** 
* Function	:	MAIL_CreateGetWnd
*	   
* Purpose	:    
*
\**********************************************************************/
BOOL CreateMailGetWnd()
{
    if(IsWindow(HwndMailGet))
    {
    }
    else
    {
        if(!RegisterMailGetClass())
            return FALSE;
        
        HwndMailGet = CreateWindow(
            "MailGetWndClass",
            "",
            0,
            0, 0, 0, 0,
            NULL,
            NULL,
            NULL,
            NULL);
    }
    
    if(HwndMailGet == NULL)
    {
        return FALSE;
    }
    
    return TRUE;
}

/******************************************************************** 
* Function	:	MailGetWndProc
*	   
* Purpose	:   Get mail window procedure 
*
* Remarks	:   
\**********************************************************************/
static LRESULT MailGetWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                               LPARAM lParam)
{
    LRESULT lResult;
    lResult = (LRESULT)TRUE;

    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
        }
    	break;

    case WM_COMMAND:
        {
            MAIL_DealGetCommand(hWnd, wMsgCmd, wParam, lParam);
        }
    	break;

    case WM_MAIL_MSG:
        {
            MAIL_DealGetMessage(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_RTCTIMER:
        {
            MAIL_DealGetTimer(hWnd, wParam, lParam);
        }
        break;

    case WM_CLOSE:
        {
        }
        break;
        
    case WM_DESTROY:
        {
        }
        break;

    case WM_MAIL_CANCEL_RETURN:
        {
            MAIL_DealCancelReturn(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    
    return lResult;
}

static void MAIL_DealGetMessage(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{    
    switch(LOWORD(lParam))
    {
    case MSG_MAIL_GETHOST:
        {
            if(HIWORD(lParam) == MAIL_SUCCESS)
            {
            }
            else
            {
                printf("\r\nGARY_MAILPROTOCOL_GET:lParam is %x\r\n", (unsigned int)lParam);

                if(GlobalMailConfigHead.GLInboxHandle != 0)
                {
                    MAIL_DestroyInbox(GlobalMailConfigHead.GLInboxHandle);
                    GlobalMailConfigHead.GLInboxHandle = 0;
                }

                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
                    
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTFAL, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                
                MAIL_DealNoReturnConnect();
            }
        }
    	break;

    case MSG_MAIL_CONNECT:
        {
            if(HIWORD(lParam) == MAIL_SUCCESS)
            {
            }
            else
            {
                printf("\r\nGARY_MAILPROTOCOL_GET:lParam is %x\r\n", (unsigned int)lParam);

                if(GlobalMailConfigHead.GLInboxHandle != 0)
                {
                    MAIL_DestroyInbox(GlobalMailConfigHead.GLInboxHandle);
                    GlobalMailConfigHead.GLInboxHandle = 0;
                }
                
                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
                    
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTFAL, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                
                MAIL_DealNoReturnConnect();
            }
        }
    	break;

    case MSG_MAIL_LOGIN:
        {
            if(HIWORD(lParam) == MAIL_SUCCESS)
            {
                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    UpdateWaitwinContext(IDP_MAIL_STRING_DOWNLOADING);
                }
            }
            else
            {
                printf("\r\nGARY_MAILPROTOCOL_GET:lParam is %x\r\n", (unsigned int)lParam);
                
                if(GlobalMailConfigHead.GLInboxHandle != 0)
                {
                    MAIL_DestroyInbox(GlobalMailConfigHead.GLInboxHandle);
                    GlobalMailConfigHead.GLInboxHandle = 0;
                }
                
                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
                    
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTFAL, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                
                MAIL_DealNoReturnConnect();
            }
        }
        break;

    case MSG_MAIL_REFRESH:
        {
        }
        break;

    case MSG_MAIL_LISTINFO:
        {
            /*if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
            {
                UpdateWaitwinContext(IDP_MAIL_STRING_DOWNLOADING);
            }*/
        }
        break;

    case MSG_MAIL_LIST:
        {
            if(HIWORD(lParam) == MAIL_SUCCESS)
            {
                MAIL_GetServerHead();
            }
            else
            {
                printf("\r\nGARY_MAILPROTOCOL_GET:lParam is %x\r\n", (unsigned int)lParam);
                
                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTFAL, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }

                MAIL_ConnectServer(NULL, MAIL_GET_DISCONNECT);
            }
        }
        break;
        
    case MSG_MAIL_DELE:
        {
            if(HIWORD(lParam) == MAIL_SUCCESS)
            {
                MAIL_DelServerHead();
            }
            else
            {
                printf("\r\nGARY_MAILPROTOCOL_GET:lParam is %x\r\n", (unsigned int)lParam);

                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELFAIL, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                
                MAIL_ConnectServer(NULL, MAIL_GET_DISCONNECT);
            }            
        }
        break;

    case MSG_MAIL_RECVDATA:
        {
        }
        break;

    case MSG_MAIL_RECV:
        {
            if(HIWORD(lParam) == MAIL_SUCCESS)
            {
                MAIL_RetrieveDigest(TRUE);
            }
            else
            {
                printf("\r\nGARY_MAILPROTOCOL_GET:lParam is %x\r\n", (unsigned int)lParam);
                
                MAIL_RetrieveDigest(FALSE);
            } 
        }
        break;

    case MSG_MAIL_BROKEN:
        {
        }
        break;

    case MSG_MAIL_QUIT:
        {
            if(HIWORD(lParam) == MAIL_FAILURE)
            {
                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DISCONFAL, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                break;
            }
            if(GlobalMailConfigHead.GLInboxHandle != 0)
            {
                MAIL_DestroyInbox(GlobalMailConfigHead.GLInboxHandle);
                GlobalMailConfigHead.GLInboxHandle = 0;
            }

            if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
            {
                WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DISCONNECTED, NULL, Notify_Success, 
                    IDP_MAIL_BOOTEN_OK, NULL, 20);
            }
            
            GlobalMailConfigHead.GLInboxHandle = 0;
            if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
            {
                MAIL_QuitServerHead();
            }
            else
            {
                //MU_NewMsgArrival();
            }
            
            MAIL_DealNoReturnConnect();
        }
        break;

    default:
        PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return;
}

static void MAIL_DealGetCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case WM_MAIL_DIAL_SUCCESS:
        {
            BOOL bReturn;
            
            if(GlobalMailConfigHead.CurConnectConfig == NULL)
            {
                MAIL_DealNoReturnConnect();
                break;
            }

            if(GlobalMailConfigHead.CurConnectConfig->GLRetrHead == MAIL_RET_HEADER_ALL)
            {
                bReturn = MAIL_GetHeaderFromServer(NULL, -1, MAX_PROTOCOL_FILE);
            }
            else if(GlobalMailConfigHead.CurConnectConfig->GLRetrHead == MAIL_RET_HEADER_5)
            {
                bReturn = MAIL_GetHeaderFromServer(NULL, -1, 5);
            }
            else if(GlobalMailConfigHead.CurConnectConfig->GLRetrHead == MAIL_RET_HEADER_10)
            {
                bReturn = MAIL_GetHeaderFromServer(NULL, -1, 10);
            }
            else if(GlobalMailConfigHead.CurConnectConfig->GLRetrHead == MAIL_RET_HEADER_30)
            {
                bReturn = MAIL_GetHeaderFromServer(NULL, -1, 30);
            }
            else if(GlobalMailConfigHead.CurConnectConfig->GLRetrHead == MAIL_RET_HEADER_50)
            {
                bReturn = MAIL_GetHeaderFromServer(NULL, -1, 50);
            }
            else if(GlobalMailConfigHead.CurConnectConfig->GLRetrHead == MAIL_RET_HEADER_100)
            {
                bReturn = MAIL_GetHeaderFromServer(NULL, -1, 100);
            }
            else
            {
                bReturn = MAIL_GetHeaderFromServer(NULL, -1, MAX_PROTOCOL_FILE);
            }
            
            if(!bReturn)
            {
                SendMessage(HwndMailGet, WM_MAIL_MSG, 0, MAKELPARAM(MSG_MAIL_QUIT, 0));
                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTFAL, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                break;
            }
        }
        break;

    case WM_MAIL_DIAL_CANCEL:
        {
            printf("\r\nGARY_MAILPROTOCOL_GET:dial failure\r\n");

            if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
            {
                WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
                
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTFAL, NULL, Notify_Failure, 
                    IDP_MAIL_BOOTEN_OK, NULL, 20);
            }

            if(GlobalMailConfigHead.GLInboxHandle)
            {
                MAIL_ConnectServer(NULL, MAIL_GET_DISCONNECT);
            }

            MAIL_DealNextConnect();
        }
        break;
        
    default:
        PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return;
}
static void MAIL_DealCancelReturn(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    switch(lParam)
    {
    case 0:
        {
            if((GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
                || (GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SEND))
            {
                MailConWaitNode *pTemp;

                WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);

                pTemp = GlobalMailConfigHead.pWaitNext;
                while(pTemp)
                {
                    if(pTemp->GLConType & MAIL_CON_TYPE_SHOWREC)
                    {
                        MAIL_DelDialWaitNode(&GlobalMailConfigHead, pTemp);
                        break;
                    }
                    pTemp = pTemp->pNext;
                }
                break;
            }

            if(MAIL_GetState(GlobalMailConfigHead.GLInboxHandle) == BS_RETRIEVE)
            {
                break;
            }

            if(GlobalMailConfigHead.GLInboxHandle != 0)
            {
                MAIL_DestroyInbox(GlobalMailConfigHead.GLInboxHandle);
            }
            
            GlobalMailConfigHead.GLInboxHandle = 0;
            MAIL_QuitServerHead();

            MAIL_DealNoReturnConnect();
        }
    	break;

    case 1:
        {
        }
    	break;

    default:
        break;
    }
    
    return;
}

void MAIL_ReleaseConfigList(MailConfigHeader *pHead)
{
    MailConfigNode *pTemp;
    
    pTemp = pHead->pNext;
    //only delete first node 
    //until the list is empty
    while(pTemp)
    {
        pHead->pNext = pTemp->pNext;
        GARY_FREE(pTemp);
        pTemp = NULL;
        pHead->len --;
        pTemp = pHead->pNext;
    }
    pHead->len = 0;
    pHead->pNext = NULL;
    
    return;
}

void MAIL_AddConfigNode(MailConfigHeader *pHead, MailConfigNode *pNewNode)
{
    MailConfigNode *pPreTemp;
    MailConfigNode *pCurTemp;
    
    pNewNode->pNext = NULL;

    //new node should add to pPreTemp->pNext
    pPreTemp = pHead->pNext;
    if(pPreTemp == NULL)
    {
        //new node is the first node in list
        pHead->pNext = pNewNode;
        pHead->len ++;
        
        return;
    }
    if((stricmp(pNewNode->GLMailBoxName, pPreTemp->GLMailBoxName) < 0)
        && (strcmp(pPreTemp->GLMailBoxName, IDP_MAIL_TITLE_NEWMAILBOX)))
    {
        pHead->pNext = pNewNode;
        pNewNode->pNext = pPreTemp;
        pHead->len ++;

        return;
    }

    pCurTemp = pPreTemp->pNext;
    while (pCurTemp != NULL)
    {
        if(stricmp(pNewNode->GLMailBoxName, pCurTemp->GLMailBoxName) < 0
            && (strcmp(pCurTemp->GLMailBoxName, IDP_MAIL_TITLE_NEWMAILBOX)))
        {
            pPreTemp->pNext = pNewNode;
            pNewNode->pNext = pCurTemp;
            pHead->len ++;
            
            return;
        }
        pPreTemp = pCurTemp;
        pCurTemp = pPreTemp->pNext;
    }
    pPreTemp->pNext = pNewNode;
    pHead->len ++;
    
    return;
}

void MAIL_DelConfigNode(MailConfigHeader *pHead, int nTempNode)
{
    MailConfigNode *pPreNode;
    MailConfigNode *pCurNode;
    int i;
    
    if(nTempNode > pHead->len)
    {
        return;
    }
    
    pCurNode = pHead->pNext;
    if(nTempNode == 0)
    {
        pHead->pNext = pCurNode->pNext;
        GARY_FREE(pCurNode);
        pHead->len --;
        return;
    }
    
    for(i=0;i<nTempNode;i++)
    {
        pPreNode = pCurNode;
        pCurNode = pPreNode->pNext;
    }
    
    pPreNode->pNext = pCurNode->pNext;
    GARY_FREE(pCurNode);
    pHead->len --;
    
    return;
}

void MAIL_RemoveConfigNode(MailConfigHeader *pHead, int nTempNode)
{
    MailConfigNode *pPreNode;
    MailConfigNode *pCurNode;
    int i;
    
    if(nTempNode > pHead->len)
    {
        return;
    }
    
    pCurNode = pHead->pNext;
    if(nTempNode == 0)
    {
        pHead->pNext = pCurNode->pNext;
        pHead->len --;
        return;
    }
    
    for(i=0;i<nTempNode;i++)
    {
        pPreNode = pCurNode;
        pCurNode = pPreNode->pNext;
    }
    
    pPreNode->pNext = pCurNode->pNext;
    pHead->len --;
    
    return;
}

BOOL MAIL_GetConfigNodebyInt(MailConfigHeader *pHead, MailConfigNode *pConfigNode, int iTmp)
{
    int i;
    MailConfigNode *pTemp;
    
    if(iTmp > (pHead->len - 1))
        return FALSE;

    if(pHead->pNext == NULL)
        return FALSE;
    
    pTemp = pHead->pNext;
    for(i=0; i<iTmp; i++)
    {
        pTemp = pTemp->pNext;
    }
    memcpy(pConfigNode, pTemp, sizeof(MailConfigNode));
    
    return TRUE;
}

int MAIL_GetConfigNodebyName(MailConfigHeader *pHead, MailConfigNode *pConfigNode, const char *szTmpName)
{
    int i;
    MailConfigNode *pTemp;

    i = 0;
    pTemp = pHead->pNext;

    while (pTemp)
    {
        if((strcmp(pTemp->GLMailBoxName, szTmpName)) == 0)
        {
            memcpy(pConfigNode, pTemp, sizeof(MailConfigNode));
            return i;
        }
        pTemp = pTemp->pNext;
        i ++;
    }
    
    return -1;
}

int MAIL_GetConfigNodebyID(MailConfigHeader *pHead, MailConfigNode **pConfigNode, int iTmp)
{
    int i;
    MailConfigNode *pTemp;
    
    *pConfigNode = NULL;
    i = 0;
    pTemp = pHead->pNext;
    
    while (pTemp)
    {
        if(pTemp->GLMailBoxID == iTmp)
        {
            *pConfigNode = pTemp;
            return i;
        }
        pTemp = pTemp->pNext;
        i ++;
    }
    
    return -1;
}

BOOL MAIL_SaveMailContent(int hMailObj, char *DownMailName, char *BoxName, int iReadFileSize, BOOL *bAtt)
{
    char *chTemp;
    char *chTotalTemp;
    char *szAffixNameString;

    unsigned int len;
    unsigned int nSavedAttachNum = 0;
    int i;
    int hFileProtocol;
    int hFileLocal;

    MailFile mInMail;
    struct stat buf;
    char szOutPath[PATH_MAXLEN];

    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    //get new inbox file name
    memset(DownMailName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
    sprintf(DownMailName, "%s", MAIL_MAIL_FILE);

    MAIL_GetNewFileName(DownMailName, MAX_MAIL_SAVE_FILENAME_LEN + 1);

    memset(&mInMail, 0x0, sizeof(MailFile));
    
    //message-id
    chTemp = NULL;
    chTemp = MAIL_GetMailHeader(hMailObj, FIELD_MESSAGEID);
    if(chTemp != NULL)
    {
        len = strlen(BoxName) + strlen(chTemp);
        chTotalTemp = NULL;
        chTotalTemp = malloc(len + 1);
        if(chTotalTemp == NULL)
        {
            chdir(szOutPath);
            return FALSE;
        }
        memset(chTotalTemp, 0x0, len + 1);
        sprintf(chTotalTemp, "%s%s", BoxName, chTemp);
        if(strlen(chTotalTemp) > MAX_MAIL_MSGID_LEN)
        {
            strncpy(mInMail.MsgId, chTotalTemp, MAX_MAIL_MSGID_LEN);
        }
        else
        {
            strcpy(mInMail.MsgId, chTotalTemp);
        }
        GARY_FREE(chTotalTemp);
        len = 0;
    }
    mInMail.MsgId[MAX_MAIL_MSGID_LEN] = '\0';

    //ownaddress
    strcpy(mInMail.OwnAddress, GlobalMailConfigHead.CurConnectConfig->GLMailAddr);

    //from
    chTemp = NULL;
    chTemp = MAIL_GetMailHeader(hMailObj, FIELD_FROM);
    if(chTemp != NULL)
    {
        char *pMailContent;
        char *chTrun;
        int chLen;
        
        chLen = strlen(chTemp);
        chTrun = malloc(chLen + 1);
        if(!chTrun)
        {
            chdir(szOutPath);
            return FALSE;
        }
        memset(chTrun, 0x0, chLen + 1);
        MAIL_TruncateRec(chTrun, chTemp);

        pMailContent = Mail_UTF8DecodeOnString(chTrun, strlen(chTrun));
        GARY_FREE(chTrun);

        if(!pMailContent)
        {
            chdir(szOutPath);
            return FALSE;
        }
        
        if(strlen(chTemp) > MAX_MAIL_ADDRESS_SIZE)
        {
            strncpy(mInMail.From, pMailContent, MAX_MAIL_ADDRESS_SIZE);
            strncpy(mInMail.address, pMailContent, MAX_MAIL_ADDRESS_SIZE);
        }
        else
        {
            strcpy(mInMail.From, pMailContent);
            strcpy(mInMail.address, pMailContent);
        }

        GARY_FREE(pMailContent);
    }
    mInMail.From[MAX_MAIL_ADDRESS_SIZE] = '\0';
    mInMail.address[MAX_MAIL_ADDRESS_SIZE] = '\0';

    //to
    chTemp = NULL;
    chTemp = MAIL_GetMailHeader(hMailObj, FIELD_TO);
    if(chTemp != NULL)
    {
        char *pMailContent;
        
        pMailContent = Mail_UTF8DecodeOnString(chTemp, strlen(chTemp));
        if(!pMailContent)
        {
            chdir(szOutPath);
            return FALSE;
        }

        MAIL_CommaToSemicolon(pMailContent);

        if(strlen(chTemp) > MAX_MAIL_ADDRESS_SIZE)
        {
            strncpy(mInMail.To, pMailContent, MAX_MAIL_ADDRESS_SIZE);
        }
        else
        {
            strcpy(mInMail.To, pMailContent);
        }

        GARY_FREE(pMailContent);
    }
    mInMail.To[MAX_MAIL_ADDRESS_SIZE] = '\0';

    //Cc
    chTemp = NULL;
    chTemp = MAIL_GetMailHeader(hMailObj, FIELD_CC);
    if(chTemp != NULL)
    {
        char *pMailContent;
        
        pMailContent = Mail_UTF8DecodeOnString(chTemp, strlen(chTemp));
        if(!pMailContent)
        {
            chdir(szOutPath);
            return FALSE;
        }

        MAIL_CommaToSemicolon(pMailContent);
        
        if(strlen(chTemp) > MAX_MAIL_ADDRESS_SIZE)
        {
            strncpy(mInMail.Cc, pMailContent, MAX_MAIL_ADDRESS_SIZE);
        }
        else
        {
            strcpy(mInMail.Cc, pMailContent);
        }
        
        GARY_FREE(pMailContent);
    }
    mInMail.Cc[MAX_MAIL_ADDRESS_SIZE] = '\0';

    //date
    chTemp = NULL;
    chTemp = MAIL_GetMailHeader(hMailObj, FIELD_DATE);
    if(chTemp != NULL)
    {
        SYSTEMTIME sy;

        memset(&sy, 0x0, sizeof(SYSTEMTIME));
        MAIL_CharToSystem(chTemp, &sy);
        memcpy(&mInMail.Date, &sy, sizeof(SYSTEMTIME));
        /*if(strlen(chTemp) > MAX_MAIL_DATE_LEN)
        {
            strncpy(mInMail.Date, chTemp, MAX_MAIL_DATE_LEN);
        }
        else
        {
            strcpy(mInMail.Date, chTemp);
        }*/
    }
    //mInMail.Date[MAX_MAIL_DATE_LEN] = '\0';

    //subject
    chTemp = NULL;
    chTemp = MAIL_GetMailHeader(hMailObj, FIELD_SUBJECT);
    if(chTemp != NULL)
    {
        char *pMailContent;
        
        pMailContent = Mail_UTF8DecodeOnString(chTemp, strlen(chTemp));
        if(!pMailContent)
        {
            chdir(szOutPath);
            return FALSE;
        }

        if(strlen(chTemp) > MAX_MAIL_SUBJECE_LEN)
        {
            strncpy(mInMail.Subject, pMailContent, MAX_MAIL_SUBJECE_LEN);
        }
        else
        {
            strcpy(mInMail.Subject, pMailContent);
        }

        GARY_FREE(pMailContent);
    }
    mInMail.Subject[MAX_MAIL_SUBJECE_LEN] = '\0';

    //body
    len = MAIL_GetMailTextLength(hMailObj);
    len = (len > MAX_MAIL_BODY_SIZE) ? MAX_MAIL_BODY_SIZE : len;
    chTemp = NULL;
    chTemp = (char *)malloc(len + 1);
    if(chTemp == NULL)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        chdir(szOutPath);
        return FALSE;
    }
    memset(chTemp, 0x0, len + 1);
    len = MAIL_GetMailText(hMailObj, 0, chTemp, len);
    mInMail.TextSize = len;
    //decode body
    if(MAIL_GetMailTextLength(hMailObj) > 0)
    {
        /*char *content_type;
        
        content_type = MAIL_GetMailTextInfo(hMailObj, "CHARSET", NULL);
        if(content_type != NULL)
        {
            MAIL_UpperCase(content_type);
        }
        if((content_type != NULL) &&(strstr(content_type, MAIL_CHARSET_UTF8) != NULL))*/
        {
            char *pMailContent;

            pMailContent = Mail_UTF8DecodeOnString(chTemp, len);
            if(!pMailContent)
            {
                GARY_FREE(chTemp);
                chdir(szOutPath);
                return FALSE;
            }
            strcpy(chTemp, pMailContent);
            GARY_FREE(pMailContent);
        }
    }

    //attachments
    mInMail.AttNum = MAIL_GetAttachmentCount(hMailObj);
    szAffixNameString = NULL;
    if(mInMail.AttNum > 0)
    {
        szAffixNameString = malloc(mInMail.AttNum * sizeof(MailAttachmentNode));
        if(szAffixNameString == NULL)
        {
            GARY_FREE(chTemp);
            chdir(szOutPath);
            return FALSE;
        }
        memset(szAffixNameString, 0x0, mInMail.AttNum * sizeof(MailAttachmentNode));
    }
    
    nSavedAttachNum = 0;
    //copy attachment and get attachment file name string
    //deal all attachments
    for(i=0; i<(int)mInMail.AttNum; i++)
    {
        int ilen;
        int size;
        char *tmpatt = NULL;
        char *tmpatt1 = NULL;
        ATTACHMENT *att = NULL;
        
        char strShowFileName[MAX_MAIL_SHOW_FILENAME_LEN + 1];
        char strSaveFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
        char strMaxFileName[256];

        memset(strShowFileName, 0x0, MAX_MAIL_SHOW_FILENAME_LEN + 1);
        memset(strSaveFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        memset(strMaxFileName, 0x0, 256);

        size = MAIL_GetAttachmentInfo(hMailObj, i, NULL, 0);
        if(size == 0)
        {
            continue;
        }
        if(size >= sizeof(ATTACHMENT))
        {
            att = (ATTACHMENT *)malloc(size);
            memset(att, 0x0, sizeof(ATTACHMENT));
            if((att != NULL) && (MAIL_GetAttachmentInfo(hMailObj, i, att, size) == MAIL_SUCCESS))
            {
                char FullPathColon[MAX_MAIL_SAVE_FULLNAME_LEN + 1];

                //path file save name(from protocol)
                if(att->filename == NULL)
                {
                    GARY_FREE(att);
                    continue;
                }
                //file show name
                if(att->name != NULL)
                {
                    strcpy(strMaxFileName, att->name);
                }
                ilen = strlen(strMaxFileName);
                if(ilen > MAX_MAIL_SHOW_FILENAME_LEN)
                {
                    int nPos;
                    
                    nPos = MAIL_StringCut(strMaxFileName, ilen - MAX_MAIL_SHOW_FILENAME_LEN + 1);
                    strcpy(strShowFileName, (strMaxFileName + nPos));
                }
                else
                {
                    strcpy(strShowFileName, strMaxFileName);
                }
                
                //get new file save name
                MAIL_GetNewAffixFileName(strSaveFileName);

                //showname###savename***showname###savename\0
                if(i == 0)
                {
                    strcpy(szAffixNameString, strShowFileName);
                }
                else
                {
                    strcat(szAffixNameString, "***");
                    strcat(szAffixNameString, strShowFileName);
                }
                strcat(szAffixNameString, "###");
                strcat(szAffixNameString, strSaveFileName);

                //copy attachment from protocol
                memset(FullPathColon, 0x0, MAX_MAIL_SAVE_FULLNAME_LEN + 1);
                MAIL_ConvertPath(FullPathColon, att->filename, RAM_COLON_TO_MNT);
                MAIL_LowerCase(FullPathColon);
                
                hFileProtocol = GARY_open(FullPathColon, O_RDONLY, -1);
                if(hFileProtocol < 0)
                {
                    printf("\r\nMAIL_SaveMailContent1 : open error = %d\r\n", errno);

                    GARY_FREE(att);
                    continue;
                }
                hFileLocal = GARY_open(strSaveFileName, O_WRONLY | O_CREAT, S_IRWXU);
                if(hFileLocal < 0)
                {
                    printf("\r\nMAIL_SaveMailContent2 : open error = %d\r\n", errno);

                    GARY_FREE(att);
                    GARY_close(hFileProtocol);
                    continue;
                }
                if(stat(FullPathColon, &buf) >= 0)
                {
                    tmpatt = (char *)malloc(buf.st_size + 1);
                    if(tmpatt == NULL)
                    {
                        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
                            IDP_MAIL_BOOTEN_OK, NULL, 20);
                        GARY_FREE(att);
                        GARY_close(hFileProtocol);
                        GARY_close(hFileLocal);
                        continue;
                    }
                    memset(tmpatt, 0x0, buf.st_size + 1);
                    if(read(hFileProtocol, tmpatt, buf.st_size) != (int)buf.st_size)
                    {
                        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
                            IDP_MAIL_BOOTEN_OK, NULL, 20);
                        GARY_FREE(att);
                        GARY_FREE(tmpatt);
                        GARY_close(hFileProtocol);
                        GARY_close(hFileLocal);
                        continue;
                    }
                    if(att->charset != NULL)
                    {
                        MAIL_UpperCase(att->charset);
                    }
                    if((att->charset != NULL) && (strstr(att->charset, MAIL_CHARSET_UTF8) != NULL))
                    {
                        tmpatt1 = (char *)malloc(buf.st_size * 2 + 1);
                        if(tmpatt1 == NULL)
                        {
                            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
                                IDP_MAIL_BOOTEN_OK, NULL, 20);
                            GARY_FREE(att);
                            GARY_FREE(tmpatt);
                            GARY_close(hFileProtocol);
                            GARY_close(hFileLocal);
                            continue;
                        }
                        memset(tmpatt1, 0x0, buf.st_size * 2 + 1);
                        ilen = UTF8ToMultiByte(0, 0, tmpatt, buf.st_size, tmpatt1, buf.st_size * 2, NULL, NULL);
                        if(write(hFileLocal, tmpatt1, ilen) != ilen)
                        {
                            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
                                IDP_MAIL_BOOTEN_OK, NULL, 20);
                            GARY_FREE(att);
                            GARY_FREE(tmpatt);
                            GARY_FREE(tmpatt1);
                            GARY_close(hFileProtocol);
                            GARY_close(hFileLocal);
                            continue;
                        }

                    }//end if(MAIL_CHARSET_UTF8)
                    else
                    {
                        if(write(hFileLocal, tmpatt, buf.st_size) != (int)buf.st_size)
                        {
                            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
                                IDP_MAIL_BOOTEN_OK, NULL, 20);
                            GARY_FREE(att);
                            GARY_close(hFileProtocol);
                            GARY_close(hFileLocal);
                            continue;
                        }
                    }
                }//end if(stat(att->filename, &buf) >= 0)
                nSavedAttachNum ++;
                if(att != NULL)
                {
                    GARY_FREE(att);
                }
                if(tmpatt != NULL)
                {
                    GARY_FREE(tmpatt);
                }
                if(tmpatt1 != NULL)
                {
                    GARY_FREE(tmpatt1);
                }
                if(hFileProtocol >= 0)
                {
                    GARY_close(hFileProtocol);
                }
                if(hFileLocal >= 0)
                {
                    GARY_close(hFileLocal);
                }
            }//end if(MAIL_GetAttachmentInfo(hMailObj, i, att, size) == MAIL_SUCCESS))
        }// end if (size >= sizeof(ATTACHMENT))
    }// end for (i = 0; i < mInMail.AttNum; i ++)
    
    mInMail.AttNum = nSavedAttachNum;
    if(mInMail.AttNum > 0)
    {
        *bAtt = TRUE;
        mInMail.AttFileNameLen = strlen(szAffixNameString);
    }
    else
    {
        *bAtt = FALSE;
        mInMail.AttFileNameLen = 0;
    }
    //mail size
    mInMail.MailSize = iReadFileSize;
    mInMail.ReadFlag = UNREAD;

    //localtime--download time
    GetLocalTime(&mInMail.LocalDate);

    //write mail head
    hFileLocal = GARY_open(DownMailName, O_RDWR | O_CREAT, S_IRWXU);
    if(hFileLocal < 0)
    {
        printf("\r\nMAIL_SaveMailContent3 : open error = %d\r\n", errno);

        GARY_FREE(chTemp);
        if((mInMail.AttNum > 0) && (szAffixNameString != NULL))
        {
            MAIL_DeleteAttachment(szAffixNameString);
        }
        if(szAffixNameString != NULL)
        {
            GARY_FREE(szAffixNameString);
        }
        chdir(szOutPath);
        return FALSE;
    }
    
    if(write(hFileLocal, &mInMail, sizeof(MailFile)) != sizeof(MailFile))
    {
        GARY_close(hFileLocal);
        GARY_FREE(chTemp);
        if((mInMail.AttNum > 0) && (szAffixNameString != NULL))
        {
            MAIL_DeleteAttachment(szAffixNameString);
        }
        if(szAffixNameString != NULL)
        {
            GARY_FREE(szAffixNameString);
        }
        chdir(szOutPath);
        return FALSE;
    }
    
    //write body
    if(len > 0)
    {
        if(write(hFileLocal, chTemp, len) != (int)len)
        {
            GARY_close(hFileLocal);
            GARY_FREE(chTemp);
            if((mInMail.AttNum > 0) && (szAffixNameString != NULL))
            {
                MAIL_DeleteAttachment(szAffixNameString);
            }
            if(szAffixNameString != NULL)
            {
                GARY_FREE(szAffixNameString);
            }
            chdir(szOutPath);
            return FALSE;
        }
    }
    GARY_FREE(chTemp);

    //write attachment
    if(mInMail.AttNum > 0)
    {
        if(write(hFileLocal, szAffixNameString, mInMail.AttFileNameLen) != (int)mInMail.AttFileNameLen)
        {
            GARY_close(hFileLocal);
            MAIL_DeleteAttachment(szAffixNameString);
            if(szAffixNameString != NULL)
            {
                GARY_FREE(szAffixNameString);
            }
            chdir(szOutPath);
            return FALSE;
        }
        GARY_FREE(szAffixNameString);
    }
    GARY_close(hFileLocal);
    chdir(szOutPath);

    return TRUE;
}

char *Mail_UTF8DecodeOnString(char *pSrc, int nSize)
{
    char *pDest;
    int length;
    
    if (!pSrc)
        return NULL;
    
    pDest = NULL;
    length = UTF8ToMultiByte(0, 0, pSrc, nSize, pDest, 0, NULL, NULL);

    pDest = (char *)malloc(length + 1); 
    memset(pDest, 0x00, length + 1);
    if (!pDest)
        return NULL;
    
    UTF8ToMultiByte(0, 0, pSrc, nSize, pDest, length, NULL, NULL);

    return pDest;
}

static void MAIL_DealGetTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int TimerID;
    int iSel;
    MailConfigNode *pConConfig;
    
    TimerID = (unsigned int)(wParam);

    pConConfig = NULL;
    iSel = MAIL_GetConfigNodebyID(&GlobalMailConfigHead, &pConConfig, TimerID);
    if(iSel == -1)
    {
        return;
    }

    pConConfig->GLTimer = FALSE;

    if((pConConfig->GLAutoRetrieval != -1) && (GetSIMState() == 1))
    {
        SYSTEMTIME sy;
        int nDay;
        RTCTIME AlmTime;
        
        pConConfig->GLTimer = TRUE;
        
        GetLocalTime(&sy);
        sy.wMinute += pConConfig->GLAutoRetrieval;
        
        nDay = 0;
        while((int)sy.wMinute > 59)
        {
            sy.wHour ++;
            sy.wMinute -= 60;
        }
        while((int)sy.wHour > 23)
        {
            nDay ++;
            sy.wHour -= 24;
        }
        MAIL_DayIncrease(&sy, nDay);
        
        memset(&AlmTime, 0x0, sizeof(RTCTIME));
        
        AlmTime.v_nYear		= (u_INT2)sy.wYear;
        AlmTime.v_nMonth	= (u_INT1)sy.wMonth;
        AlmTime.v_nDay		= (u_INT1)sy.wDay;
        AlmTime.v_nHour		= (u_INT1)sy.wHour;
        AlmTime.v_nMinute	= (u_INT1)sy.wMinute;
        
        RTC_SetAlarms("Edit Email", (u_INT2)pConConfig->GLMailBoxID, ALMF_REPLACE | ALMF_RELATIVE, "Edit Email", &AlmTime);
        //SetRTCTimer(HwndMailGet, pNode->GLMailBoxID, pNode->GLAutoRetrieval * 1000, NULL);
    }

    if(GlobalMailConfigHead.GLNetUsed)
    {
        /*if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
        {
            return;
        }
        else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
        {
            if(GlobalMailConfigHead.CurConnectConfig == pConConfig)
            {
                if(GlobalMailConfigHead.GLConStatus != MAILBOX_CON_CONNECT)
                {
                    MAIL_DelDialWaitNode(&GlobalMailConfigHead, pConConfig);
                    return;
                }
                else
                {
                    GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_HIDEREC;
                    pConConfig->GLConType |= MAIL_CON_TYPE_HIDEREC;
                    MAIL_GetServerHead();
                }
            }
            else
            {
                return;
            }
        }
        else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SEND)
        {
            if(GlobalMailConfigHead.GLConStatus != MAILBOX_CON_CONNECT)
            {
                return;
            }
            else if(GlobalMailConfigHead.GLConnect != pConConfig->GLConnect)
            {
                return;
            }
            else
            {
                GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_HIDEREC;
                pConConfig->GLConType |= MAIL_CON_TYPE_HIDEREC;
                MAIL_GetServerHead();
            }
        }
        else
        {
            return;
        }*/
        MAIL_AddDialWaitNode(&GlobalMailConfigHead, pConConfig, MAIL_CON_TYPE_HIDEREC);
    }
    else
    {
        MAIL_ConnectDial(hWnd, pConConfig);
    }
    return;
}

BOOL MAIL_ConnectDial(HWND hWnd, MailConfigNode *pConConfig)
{
    GlobalMailConfigHead.GLNetUsed = TRUE;
    strcpy(GlobalMailConfigHead.GLMailBoxName, pConConfig->GLMailBoxName);
    GlobalMailConfigHead.GLConStatus = MAILBOX_CON_IDLE;
    GlobalMailConfigHead.GLConnect = pConConfig->GLConnect;
    GlobalMailConfigHead.GLNetFlag = MAIL_CON_TYPE_HIDEREC;

    pConConfig->GLConType |= MAIL_CON_TYPE_HIDEREC;
    pConConfig->GLConStatus = MAILBOX_CON_IDLE;

    GlobalMailConfigHead.CurConnectConfig = pConConfig;

    if(GetSIMState() == 1)
    {
        SendMessage(HwndMailDial, WM_MAIL_DIAL_AGAIN, (WPARAM)hWnd, 0);
    }
    
    return TRUE;
}
