/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#include "smsglobal.h"
#include <sms/smsdptch.h>

#define  MAX_QUENUM 20

typedef struct tagSENDNODE
{
    char szFileName[SMS_FILENAME_MAXLEN];
    struct tagSENDNODE* pPioneer;
    struct tagSENDNODE* pNext;
}SENDNODE,*PSENDNODE;

static SENDNODE* pSendHeader;

#define MSG_SMS_SEND        WM_USER+100
#define MSG_SMS_SEND_SUCC   WM_USER+101
#define MSG_SMS_SEND_FAIL   WM_USER+102
#define WM_AB_GETLASTERR    WM_USER+103

static HWND hSendCtlWnd = NULL;
static BOOL bBusy;
static SMS_STORE   smsstore;

LRESULT SMSSendCtlWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static void SMS_SendToDispatch(char* pszFileName);

void SendNode_Init(void)
{
    pSendHeader = NULL;
}
/*********************************************************************\
* Function	   SendNode_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static SENDNODE* SendNode_New(void)
{
    SENDNODE* p = NULL;

    p = (SENDNODE*)malloc(sizeof(SENDNODE));
    
    memset(p,0,sizeof(SENDNODE));

    return p;
}
/*********************************************************************\
* Function	   SendNode_Insert
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SendNode_Insert(SENDNODE** ppHeader,SENDNODE* pChainNode)
{
    SENDNODE* p;

    if(*ppHeader)
    {
        p = *ppHeader;

        while( p )
        {
            if( p->pNext )
                p = p->pNext;
            else
                break;
        }

        p->pNext = pChainNode;
        pChainNode->pPioneer = p;
        pChainNode->pNext = NULL;
    }
    else
    {
        *ppHeader = pChainNode;
        pChainNode->pNext = NULL;
        pChainNode->pPioneer = NULL;
    }
}
/*********************************************************************\
* Function	   SendNode_Delete
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SendNode_Delete(SENDNODE** ppHeader,SENDNODE* pChainNode)
{
    if( pChainNode == *ppHeader )
    {
        if(pChainNode->pNext)
        {
            *ppHeader = pChainNode->pNext;
            pChainNode->pNext->pPioneer = NULL;
        }
        else
            *ppHeader = NULL;
    }
    else if( !pChainNode->pNext )
    {
        pChainNode->pPioneer->pNext = NULL;
    }
    else
    {
        pChainNode->pPioneer->pNext = pChainNode->pNext;
        pChainNode->pNext->pPioneer = pChainNode->pPioneer;
    }

    free(pChainNode);
    pChainNode = NULL;
}
/*********************************************************************\
* Function	   SendNode_Erase
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SendNode_Erase(SENDNODE** ppHeader)
{
    SENDNODE* p;    
    SENDNODE* ptemp;

    p = *ppHeader;

    while( p )
    {
        ptemp = p->pNext;       
        free(p);
        p = ptemp;
    }

    *ppHeader = NULL;
}
/*********************************************************************\
* Function	   SMS_Suspend
* Purpose      
* Params	   
* Return	 	   
* Remarks	   MU_SEND_SUSPENDED
**********************************************************************/
BOOL SMS_Suspend(char* pszFileName)
{
    SENDNODE* pTmp = NULL;

    if(pSendHeader != NULL)
    {
        pTmp = pSendHeader->pNext;
        while(pTmp)
        {
            if(strcmp(pszFileName,pTmp->szFileName) == 0)
            {
                SMS_STOREFIX   storetmp;
                char        szOldPath[PATH_MAXLEN];
                
                memset(&storetmp,0,sizeof(SMS_STOREFIX));
                
                szOldPath[0] = 0;   
                
                getcwd(szOldPath,PATH_MAXLEN);  
                
                chdir(PATH_DIR_SMS);  
                
                if(SMS_ReadStatusFromFile(pszFileName,&storetmp))
                {                
                    storetmp.Stat = MU_STU_SUSPENDED;
                    
                    SMS_ModifyFileStatus(pszFileName,&storetmp);
                    
                    chdir(szOldPath); 
                    
                    if(MU_GetCurFolderType() == MU_OUTBOX)
                    {
                        smsapi_ModifyByFileName(pszFileName,MU_SEND_SUSPENDED);
                    }
                    
                    SendNode_Delete(&pSendHeader,pTmp);
                    
                    return TRUE;
                }

                chdir(szOldPath);
            }
            pTmp = pTmp->pNext;
        }
    }
    return FALSE;

}
/*********************************************************************\
* Function	   SMS_Send
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_Send(char* pszFileName,DWORD handle)
{
    SENDNODE* pNewNode;

    if(GetSIMState() != 1)
    {
        //PLXTipsWin((char*)IDS_NOSIMCARD, (char*)IDS_WARNING, WAITTIMEOUT);
        return FALSE;	
    }

    pNewNode = SendNode_New();

    if(pNewNode == NULL)
        return FALSE;

    strcpy(pNewNode->szFileName,pszFileName);
    
    SendNode_Insert(&pSendHeader,pNewNode);
 
    if(MU_GetCurFolderType() == MU_OUTBOX)
    {
        smsapi_ModifyByFileName(pszFileName,MU_SEND_WAITING);
    }

    if(bBusy == FALSE)
    {
        SMS_STOREFIX   storetmp;
        char        szOldPath[PATH_MAXLEN];
        
        memset(&storetmp,0,sizeof(SMS_STOREFIX));
        
        szOldPath[0] = 0;   
        
        getcwd(szOldPath,PATH_MAXLEN);  
        
        chdir(PATH_DIR_SMS);  
        
        if(SMS_ReadStatusFromFile(pszFileName,&storetmp))
        {        
            storetmp.Stat = MU_STU_SENDING;
            
            SMS_ModifyFileStatus(pszFileName,&storetmp);
            
            chdir(szOldPath); 
            
            if(MU_GetCurFolderType() == MU_OUTBOX)
            {
                smsapi_ModifyByFileName(pszFileName,MU_SEND_START);
            }
            
            //        WaitWindowStateEx(NULL,TRUE,IDS_SENDING,IDS_SMS,NULL,NULL);
            SMS_SendToDispatch(pszFileName);
        }
        else
            chdir(szOldPath);
    }

    return TRUE;
}
/*********************************************************************\
* Function	   SMS_CreateSendControlWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
HWND SMS_CreateSendControlWnd(void)
{
    WNDCLASS wc;

    wc.style         = 0;
	wc.lpfnWndProc   = SMSSendCtlWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName  = "SMSSendCtlWndClass";

    if (!RegisterClass(&wc))
		return NULL;

    hSendCtlWnd = CreateWindow(
		"SMSSendCtlWndClass",
		"", 
		NULL, 
		0, 0, 0, 0, 
		NULL,
		NULL, 
		NULL, 
		NULL
		);

    return hSendCtlWnd;
}
/*********************************************************************\
* Function	   SMSSendCtlWndProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
LRESULT SMSSendCtlWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    char szOldPath [PATH_MAXLEN];

    switch(wMsgCmd) 
    {
    case WM_CREATE:
        bBusy = FALSE;
    	break;

    case MSG_SMS_SEND:
        
        bBusy = FALSE;
        
        SMS_FREE(smsstore.pszContent);
        SMS_FREE(smsstore.pszUDH);
        SMS_FREE(smsstore.pszPhone);

		if (HIWORD(wParam) == ME_RS_SUCCESS)
		{
            DWORD       dwOffset;
            SMS_STORE   storetmp;

            PLXTipsWin(NULL,NULL,0,IDS_MESSAGESENT,IDS_SMS,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);

            memset(&storetmp,0,sizeof(SMS_STORE));
            
            getcwd(szOldPath,PATH_MAXLEN);  
            
            chdir(PATH_DIR_SMS);  

            if(SMS_ReadDataFromFile(pSendHeader->szFileName,&storetmp,TRUE) == FALSE)
            {
                chdir(szOldPath);
                
                SMS_FREE(storetmp.pszContent);
                SMS_FREE(storetmp.pszUDH);
                SMS_FREE(storetmp.pszPhone);
                
                return FALSE;
            }
            
            storetmp.fix.Stat = MU_STU_SENT;
            
            if(SMS_SaveRecord(SMS_FILENAME_SENT,&storetmp,&dwOffset))
            {               
                SMS_ChangeCount(MU_OUTBOX,SMS_COUNT_ALL,-1);
                SMS_ChangeCount(MU_SENT,SMS_COUNT_ALL,1);
                remove(pSendHeader->szFileName);
                                
                if(MU_GetCurFolderType() == MU_OUTBOX)
                    smsapi_DeleteByFileName(pSendHeader->szFileName);
                    
                if(MU_GetCurFolderType() == MU_SENT)
                    smsapi_NewRecord(dwOffset,&storetmp);

                SMS_NotifyIdle();
                //save to sentbox

            }
            
            SendNode_Delete(&pSendHeader,pSendHeader);
            
            SMS_NewReport(&storetmp,lParam);
            
            SMS_FillTrafficLog(storetmp.pszPhone,DRT_SEND,lParam,LOWORD(wParam),SMS_WAIT_SEND);
           
            SMS_FREE(storetmp.pszContent);
            SMS_FREE(storetmp.pszUDH);
            SMS_FREE(storetmp.pszPhone);
            
            chdir(szOldPath); 
            
		}
		else
		{
            SMS_STOREFIX   storetmp;

            ME_GetLastError(hWnd,WM_AB_GETLASTERR);

            memset(&storetmp,0,sizeof(SMS_STOREFIX));
            
            szOldPath[0] = 0;   
            
            getcwd(szOldPath,PATH_MAXLEN);  
            
            chdir(PATH_DIR_SMS);  
            
            if(SMS_ReadStatusFromFile(pSendHeader->szFileName,&storetmp))
            {            
                storetmp.Stat = MU_STU_UNSENT;
                
                SMS_ModifyFileStatus(pSendHeader->szFileName,&storetmp);
                
                chdir(szOldPath); 
                
                switch(wParam)
                {
                    //            case :
                    //                {
                    //                    char szPrompt[256];
                    //                    szPrompt[0] = 0;
                    //                    sprintf(szPrompt,"<%s>:%s",storetmp.pszPhone,IDS_INVALIDNUM);
                    //                    PLXTipsWin(szPrompt,IDS_SMS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
                    //                }
                    //            	break;
                    //
                    //            case :
                    //                PLXTipsWin(IDS_NONETWORK,IDS_SMS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
                    //            	break;
                    //
                default:
                    PLXTipsWin(NULL,NULL,0,IDS_SENDINGFAILED,IDS_SMS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
                    break;
                }
                
                if(MU_GetCurFolderType() == MU_OUTBOX)
                {
                    smsapi_ModifyByFileName(pSendHeader->szFileName,MU_SEND_FAILURE);
                }
                //modify icon   
                
                SendNode_Delete(&pSendHeader,pSendHeader);
            }
            else
                chdir(szOldPath);
		}

        if(bBusy == FALSE)
        {
            if(pSendHeader != NULL)
            {
                SMS_STOREFIX   storetmp;
                char        szOldPath[PATH_MAXLEN];
                
                memset(&storetmp,0,sizeof(SMS_STOREFIX));
                
                szOldPath[0] = 0;   
                
                getcwd(szOldPath,PATH_MAXLEN);  
                
                chdir(PATH_DIR_SMS);  
                
                if(SMS_ReadStatusFromFile(pSendHeader->szFileName,&storetmp))
                {
                    storetmp.Stat = MU_STU_SENDING;
                    
                    SMS_ModifyFileStatus(pSendHeader->szFileName,&storetmp);
                    
                    chdir(szOldPath); 
                    
                    if(MU_GetCurFolderType() == MU_OUTBOX)
                    {
                        smsapi_ModifyByFileName(pSendHeader->szFileName,MU_SEND_START);
                    }
                    
                    SMS_SendToDispatch(pSendHeader->szFileName);
                }
                else
                    chdir(szOldPath); 
            }
//            else
//                WaitWindowStateEx(NULL,FALSE,IDS_SENDING,IDS_SMS,NULL,NULL);
        }
		break;
        
    case WM_AB_GETLASTERR:
        {
            ME_EXERROR LastError;
            
            char ErrorInfo[50] = "";
            
            if(ME_GetResult(&LastError,sizeof(ME_EXERROR))!=-1)
            {
                //AB_GetErrorString(LastError,ErrorInfo);
                
                ;//PLXTipsWinEx(NULL,NULL,0, LastError,IDS_SMS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
            }
            else
                return FALSE;
        }
        break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
    
}
/*********************************************************************\
* Function	   SMS_SendToDispatch
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMS_SendToDispatch(char* pszFileName)
{
    NORMAL_SEND smssend;
    char szOldPath [PATH_MAXLEN];
    
    memset(&smsstore,0,sizeof(SMS_STORE));

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  
    
    if(SMS_ReadDataFromFile(pszFileName,&smsstore,TRUE) == FALSE)
    {
        chdir(szOldPath);

        return;
    }
    
    chdir(szOldPath);

    memset(&smssend,0,sizeof(NORMAL_SEND));
    
//    SMS_CodingContent(&smsstore);

    smssend.dcs = smsstore.fix.dcs;
    smssend.DataLen = smsstore.fix.Conlen;
    if(smsstore.pszContent)
        smssend.pData = smsstore.pszContent;
    else
        smssend.pData = "";
    smssend.Type = smsstore.fix.Type;
    strcpy(smssend.PhoneNum,smsstore.pszPhone);
    smssend.PID = smsstore.fix.PID;
    smssend.ReplyPath = smsstore.fix.ReplyPath;
    smssend.Report = smsstore.fix.Report;
    strcpy(smssend.SCA,smsstore.fix.SCA);
    smssend.VPvalue = smsstore.fix.VPvalue;
    
    //need modify
    if(smssend.dcs == DCS_8BIT)
    {
        int nMsgType;

        nMsgType = SMS_IsVcradOrVcal(&smsstore);

        SMS_SendPortSMS(hSendCtlWnd,MSG_SMS_SEND,nMsgType,&smsstore);
    }
    else
        SMSDISPATCH_SendSMS(hSendCtlWnd,MSG_SMS_SEND,smssend); 
    
    bBusy = TRUE;
}
