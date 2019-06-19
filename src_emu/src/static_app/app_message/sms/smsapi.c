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

#define COUNT_ARRAY     0
#define COUNT_CHAIN     1

typedef struct tagSMS_COUNT
{
    int     nfolderid;
    int     nunread;
    int     ncount;
    int     ndelete;
}SMS_COUNT,*PSMS_COUNT;

typedef struct tagSMS_COUNTNODE
{
    struct tagSMS_COUNT      smscount;
    struct tagSMS_COUNTNODE* pPioneer;
    struct tagSMS_COUNTNODE* pNext; 
}SMS_COUNTNODE,*PSMS_COUNTNODE;

static BOOL sms_initialization(void);
static BOOL sms_get_messages(HWND hwndmu , int folder);
static BOOL sms_get_one_message(MU_MsgNode *pmsgnode);
static BOOL sms_release_messages(HWND hwndmu);
static BOOL sms_read_message(HWND hwndmu , DWORD handle,BOOL bPre, BOOL bNext);
static BOOL sms_delete_message(HWND hwndmu , DWORD handle);
static BOOL sms_delete_multi_messages(HWND hwndmu , DWORD handle[],int nCount);
static BOOL sms_delete_all_messages(HWND hwndmu , int folder,MU_DELALL_TYPE ntype);
static BOOL sms_new_message(HWND hwndmu);
BOOL sms_setting(HWND hwndmu);
static BOOL sms_get_detail(HWND hwndmu,DWORD handle);
static BOOL sms_forward(HWND hwndmu, DWORD handle);
static BOOL sms_reply_from(HWND hwndmu, DWORD handle);
static BOOL sms_reply_all(HWND hwndmu, DWORD handle);
static BOOL sms_get_number(HWND hwndmu, DWORD handle);
static BOOL sms_resend(HWND hwndmu, DWORD handle);
static BOOL sms_defer2send_message(HWND hwndmu, DWORD handle);
static BOOL sms_move_message(HWND hwndmu, DWORD handle , int nFolder);
static BOOL sms_destroy(void);
static BOOL sms_sim_create(HWND hwndmu);
static BOOL sms_voicemail_call(HWND hwndmu);
static BOOL sms_cb_create(HWND hwndmu);
static BOOL sms_get_count(int nfolder, int *pnunread, int *pncount);
static BOOL sms_new_folder(int nfolderid);
static int  sms_get_simcounter(void);
static int  sms_get_cbcounter(void);

static BOOL SMS_SearchFile(MU_MsgNode *pmsgnode,char* pszFolderMask);
static BOOL SMS_SearchRecord(MU_MsgNode *pmsgnode);
static SMS_INDEX* SMS_New(void);
static void SMS_Insert(SMS_INDEX* pChainNode);
static void SMS_Delete(SMS_INDEX* pChainNode);
static void SMS_Erase(void);
static BOOL SMS_ReadMsgNodeFromFile(char* pszFileName,MU_MsgNode *pmsgnode);
BOOL SMS_ReadStatusFromFile(char* pszFileName,SMS_STOREFIX *psms_storefix);
BOOL SMS_ModifyFileStatus(char* pszFileName,SMS_STOREFIX *psms_storefix);
BOOL SMS_ReadDataFromFile(char* szFileName,SMS_STORE *psms_store,BOOL bOrg);
int  DeleteWithOffset(char* pszFileName,DWORD dwOffest);

BOOL sms_register(MU_IMessage **Imsg);
BOOL smsex_register(MU_ISMSEx **Ismsex);
DWORD String2DWORD(char* pszTime);
void smsapi_Delete(DWORD handle);
BOOL SMS_GetRecordCount(int nfolder, int *pnunread, int *pncount, int *pndel);

void SMS_ChangeCount(int nFolderID,SMS_COUNT_TYPE nType,int nChangeNum);

static PVOID SMS_FindCountAddr(int nFolderID,int* pnType);
static void Count_Delete(SMS_COUNTNODE* pChainNode);
BOOL SMS_DeleteCount(int nFolderID);
BOOL SMS_ResetFile(int folderid);
BOOL SMS_ResetALLFile(void);
BOOL SMS_GetSumCount(int *pnunread, int *pncount);
void SMS_NotifyIdle(void);
static BOOL SMS_Clear(int nForderID);

static BOOL SMS_ModifyRecordStatus(char* pszFileName,DWORD* pdwoffset,int nStatus);
static BOOL SMS_SearchME(MU_MsgNode *pmsgnode,int nFolderID);
BOOL SMS_ChangeStruct(SMS_STORE *psmsstore,SMS_INFO* pMESMSInfo);
BOOL SMS_WriteMEStoreInfo(MESTORE_INFO* pMEStoreInfo,int MEMaxCount);
BOOL SMS_ReadMEStoreInfo(MESTORE_INFO* pMEStoreInfo,int MEMaxCount);
static BOOL SMS_DeleteMESMS(int index);
static void UnConfirmReport(void);
static void Report_Erase(void);
static REPORT_NODE* Report_New(void);
static void Report_Insert(REPORT_NODE* pChainNode);
void Report_Delete(REPORT_NODE* pChainNode);
static BOOL SMS_GetMEStatus(void);
static void SMS_SetMEStatus(BOOL bOK);

static BOOL bReadMEOK;

static BOOL bFTAMode;


static MU_IMessage mu_sms = 
{
    sms_initialization,
	sms_get_messages,
	sms_get_one_message,
	sms_release_messages,
	sms_read_message,
	sms_delete_message,
    sms_delete_multi_messages,
	sms_delete_all_messages,
    sms_new_message,
	sms_setting,
	sms_get_detail,
	NULL,                       //msg_newmsg_continue,
	sms_forward,
	sms_reply_from,
    sms_reply_all,
    sms_get_number,
    sms_resend,
    sms_defer2send_message,
    sms_move_message,
	NULL,
	sms_destroy,
    sms_get_count,
    sms_new_folder,
};

static MU_ISMSEx mu_smsex =
{
	sms_sim_create,
	sms_voicemail_call,
    sms_cb_create,
    sms_get_simcounter,
    sms_get_cbcounter,
};

static char szOldPath[PATH_MAXLEN];
static int  nFolderID;
static PSMS_INDEX pHeader = NULL;
static DIR* pDir;
static int  hFile;
static HWND hWndmu = NULL;
static DWORD dwOffset;
static PSMS_COUNT pCountArray;
static int   nCountArrayNum;
static PSMS_COUNTNODE pCountChain = NULL;
extern CB_LANGUANGE CB_Language[CB_LANGUAGE_MAX];
static REPORT_NODE *pReportHeader = NULL;

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static HWND smsapi_GetMuHwnd(void)
{
    return hWndmu;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void smsapi_SetMuHwnd(HWND hWnd)
{
    hWndmu = hWnd;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL sms_register(MU_IMessage **Imsg)
{
	*Imsg = &mu_sms;

	return TRUE;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL smsex_register(MU_ISMSEx **Ismsex)
{
	*Ismsex = &mu_smsex;

	return TRUE;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_initialization(void)
{
    struct dirent* pData;
    int   nLen = 0;
    PSMS_INDEX   pChainNode = NULL,pTmp = NULL; 
    SMS_STOREFIX storefix;
    char szOldPath[PATH_MAXLEN];

    mkdir(PATH_DIR_SMS,S_IRWXU);
    
    szOldPath[0] = 0;

    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  

    CB_Language[0].pszLanguage = IDS_GERMAN;
    CB_Language[1].pszLanguage = IDS_ENGLISH;   
    CB_Language[2].pszLanguage = IDS_ITALIAN;   
    CB_Language[3].pszLanguage = IDS_FRENCH;    
    CB_Language[4].pszLanguage = IDS_SPANISH;   
    CB_Language[5].pszLanguage = IDS_DUTCH;     
    CB_Language[6].pszLanguage = IDS_SWEDISH;   
    CB_Language[7].pszLanguage = IDS_DANISH;    
    CB_Language[8].pszLanguage = IDS_PORTUGUESE;
    CB_Language[9].pszLanguage = IDS_FINNISH;   
    CB_Language[10].pszLanguage = IDS_NORWEGIAN; 
    CB_Language[11].pszLanguage = IDS_GREEK;     
    CB_Language[12].pszLanguage = IDS_TURKISH;   
    CB_Language[13].pszLanguage = IDS_HUNGARIAN; 
    CB_Language[14].pszLanguage = IDS_POLISH;    

    pDir = opendir(PATH_DIR_SMS);
    if(pDir == NULL)
    {
        chdir(szOldPath);
        return FALSE;
    }

    while( (pData = readdir(pDir)) != NULL )
    {
        if(SMS_FILE_OUTBOX_MARK == pData->d_name[0])
        {
            pChainNode = SMS_New();
            if(pChainNode == NULL)
            {
                SMS_Erase();
                
                closedir(pDir);
                pDir = NULL;
                chdir(szOldPath);
                return FALSE;
            }

            pChainNode->byStoreType = STORE_FILE;
			nLen = strlen(pData->d_name);
			nLen = nLen > SMS_FILENAME_MAXLEN ? SMS_FILENAME_MAXLEN : nLen;
            strncpy(pChainNode->szFileName,pData->d_name,nLen);
            pChainNode->szFileName[SMS_FILENAME_MAXLEN-1] = 0;
            SMS_Insert(pChainNode);    
        }
    }

    pTmp = pHeader;

    while(pTmp)
    {
        memset(&storefix,0,sizeof(SMS_STOREFIX));
        if(SMS_ReadStatusFromFile(pTmp->szFileName,&storefix))
        {
            if(storefix.Stat == MU_STU_SENDING || storefix.Stat == MU_STU_WAITINGSEND)
            {
                storefix.Stat = MU_STU_UNSENT;
                SMS_ModifyFileStatus(pTmp->szFileName,&storefix);
            }
        }
        pTmp = pTmp->pNext;
    }

    SMS_Erase();
    
    closedir(pDir);
    pDir = NULL;
    chdir(szOldPath);
    
    UnConfirmReport();
    
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_get_messages(HWND hwndmu , int folder)
{
    char* pFileName;
    char  szFileName[PATH_MAXLEN];

    szOldPath[0] = 0;
    dwOffset = 0;
    
    nFolderID = folder;

    SMS_ResetALLFile();

    SMSMM_Construct();

    SMS_SetMEStatus(FALSE);
  
    switch(nFolderID)
    {
    case MU_OUTBOX: // more small temporary file
    case MU_DRAFT:
        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_SMS);  
        pDir = opendir(PATH_DIR_SMS);
        if(pDir == NULL)
        {
            SendMessage(hwndmu,PWM_MSG_MU_GETMSGS_RESP,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),(LPARAM)NULL);
            chdir(szOldPath);
            return FALSE;
        }
        else
        {
            smsapi_SetMuHwnd(hwndmu);
            SendMessage(hwndmu,PWM_MSG_MU_GETMSGS_RESP,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)NULL);
            closedir(pDir);
            pDir = NULL;
            chdir(szOldPath);
            return TRUE;
        }
        
    case MU_INBOX: // a big file
        pFileName = SMS_FILENAME_INBOX;
        break;

    case MU_SENT:
        pFileName = SMS_FILENAME_SENT;
        break;

    case MU_REPORT:
        pFileName = SMS_FILENAME_REPORT;
        break;

    case MU_MYFOLDER:
        pFileName = SMS_FILENAME_MYFOLDER;
        break;

    default:
        szFileName[0] = 0;
        sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nFolderID,SMS_FILE_POSTFIX);
        if(strlen(szFileName) > (SMS_FILENAME_MAXLEN-1))   
        {
            SendMessage(hwndmu,PWM_MSG_MU_GETMSGS_RESP,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),(LPARAM)NULL);
            return FALSE;
        }
        pFileName = szFileName;
        break;
    }
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  

    hFile = open(pFileName,O_RDONLY);
    if(hFile == -1)
    {             
        if( GetError() == ENOENT)
        {
            hFile = -2;
            smsapi_SetMuHwnd(hwndmu);
            SendMessage(hwndmu,PWM_MSG_MU_GETMSGS_RESP,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)NULL);
            chdir(szOldPath);
            return TRUE;
        }
        SendMessage(hwndmu,PWM_MSG_MU_GETMSGS_RESP,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),(LPARAM)NULL);
        chdir(szOldPath);
        return FALSE;
    }
    else
    {    
#ifdef _SMS_DEBUG_
        printf("\r\n*****SMS Debug Info*****  sms_get_messages file handle = %d \r\n",hFile);
#endif
        smsapi_SetMuHwnd(hwndmu);
        SendMessage(hwndmu,PWM_MSG_MU_GETMSGS_RESP,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)NULL);
        close(hFile);
        chdir(szOldPath);
        return TRUE;
    }
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_get_one_message(MU_MsgNode *pmsgnode)
{
    char FolderMask[10];
    switch(nFolderID)
    {
    case MU_OUTBOX: // more small temporary file
    case MU_DRAFT:
        sprintf(FolderMask,"%d",nFolderID);
        
        if(SMS_SearchME(pmsgnode,nFolderID) == FALSE)
            return (SMS_SearchFile(pmsgnode,FolderMask));
        else
            return TRUE;
    	break;
        
    case MU_INBOX: // a big file
    case MU_SENT:
    case MU_MYFOLDER:
    default:
        pmsgnode->msgtype = MU_MSG_SMS;
        break;

    case MU_REPORT:
        pmsgnode->msgtype = MU_MSG_STATUS_REPORT;
        break;
    }

    if(SMS_SearchME(pmsgnode,nFolderID) == FALSE)
        return (SMS_SearchRecord(pmsgnode));
    else
        return TRUE;

}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL SMS_SearchFile(MU_MsgNode *pmsgnode,char* pszFolderMask)
{
    struct dirent *pData;
    char    szTemp;
    SMS_INDEX*    pChainNode;
    int     nLen = 0;

    while( (pData = readdir(pDir)) != NULL )
    {
        szTemp = pData->d_name[0];
        if(szTemp == pszFolderMask[0])
        {
            pChainNode = SMS_New();
            if(pChainNode == NULL)
            {
                return FALSE;
            }

            pChainNode->byStoreType = STORE_FILE;
			nLen = strlen(pData->d_name);
			nLen = nLen > SMS_FILENAME_MAXLEN ? SMS_FILENAME_MAXLEN : nLen;
            strncpy(pChainNode->szFileName,pData->d_name,nLen);
            pChainNode->szFileName[SMS_FILENAME_MAXLEN-1] = 0;
            SMS_Insert(pChainNode);
            
            if(!SMS_ReadMsgNodeFromFile(pData->d_name,pmsgnode))
            {
                return FALSE;
            }
            else
            {
                pmsgnode->handle = (DWORD)pChainNode;
                return TRUE;
            }         
        }
    }
    
    return FALSE;
}

/*********************************************************************\
* Function	   SMS_ReadMsgNodeFromFile
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL SMS_ReadMsgNodeFromFile(char* pszFileName,MU_MsgNode *pmsgnode)
{
    SMS_STORE sms_store;
    int       nType;

    memset(&sms_store,0,sizeof(SMS_STORE));

    if(!SMS_ReadDataFromFile(pszFileName,&sms_store,FALSE))
    {
        SMS_FREE(sms_store.pszContent);
        SMS_FREE(sms_store.pszPhone);
        SMS_FREE(sms_store.pszUDH);
        return FALSE;
    }
    
    
    nType = SMS_IsVcradOrVcal(&sms_store);

    if( sms_store.fix.Phonelen > 0 )
    {        
		int nLen;
		nLen = sms_store.fix.Phonelen > MU_ADDR_MAX_LEN ? MU_ADDR_MAX_LEN : sms_store.fix.Phonelen;

        strncpy(pmsgnode->addr,sms_store.pszPhone,nLen);
		
		printf("\r\n SMS api SMS_ReadMsgNodeFromFile  -------> pmsgnode->addr = %s\r\n",pmsgnode->addr);

        pmsgnode->addr[MU_ADDR_MAX_LEN] = 0;    
    }
    else
        pmsgnode->addr[0] = 0;
    
    switch(nType)
    {
    case MU_MSG_VCARD:
        strcpy(pmsgnode->subject,IDS_VCARD);
        break;
        
    case MU_MSG_VCAL:
        strcpy(pmsgnode->subject,IDS_VCAL);
        break;

    case MU_MSG_SMS:
    default:
        if( sms_store.fix.Conlen > 0 )
        {
            int nLen;
            nLen = sms_store.fix.Conlen > MU_SUBJECT_DIS_LEN ? MU_SUBJECT_DIS_LEN : sms_store.fix.Conlen;
            
            strncpy(pmsgnode->subject,sms_store.pszContent,nLen);
            
            pmsgnode->subject[MU_SUBJECT_DIS_LEN] = 0;   
        }
        else
            pmsgnode->subject[0] = 0;
        break;
    }
    
    SMS_FREE(sms_store.pszContent);
    SMS_FREE(sms_store.pszPhone);
    SMS_FREE(sms_store.pszUDH);

    pmsgnode->storage_type = MU_STORE_IN_FLASH;
    pmsgnode->msgtype = nType;
    if(sms_store.fix.Stat == MU_STU_DEFERMENT)
        pmsgnode->status = MU_STU_UNSENT;
    else
    pmsgnode->status = sms_store.fix.Stat; // attention
    pmsgnode->maskdate = sms_store.fix.dwDateTime;

    return TRUE;
    
}


/*********************************************************************\
* Function	   SMS_ReadDataFromFile
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_ReadDataFromFile(char* pszFileName,SMS_STORE *psms_store,BOOL bOrg)
{
    int f;

    f = open(pszFileName,O_RDONLY);
    if(f == -1)
        return FALSE;
      
#ifdef _SMS_DEBUG_
    printf("\r\n*****SMS Debug Info*****  SMS_ReadDataFromFile  file handle = %d \r\n",f);
#endif
    
    if(read(f,&(psms_store->fix),sizeof(SMS_STOREFIX)) == -1)
    {
        close(f);
        return FALSE;
    }

    if( psms_store->fix.Phonelen > 0)
    {
        psms_store->pszPhone = (char*)malloc(psms_store->fix.Phonelen);
        
        if( psms_store->pszPhone == NULL )
        {
            close(f);
            return FALSE;
        }
        
        if(-1 == read(f,psms_store->pszPhone,psms_store->fix.Phonelen))
        {
            SMS_FREE(psms_store->pszPhone);
            close(f);
            return FALSE;
        }
    }

    if( psms_store->fix.Conlen > 0)
    {
        if(bOrg == TRUE)
        {
            psms_store->pszContent = (char*)malloc(psms_store->fix.Conlen);
            
            if(psms_store->pszContent == NULL)
            {
                SMS_FREE(psms_store->pszPhone);
                close(f);
                return FALSE;
            }
            
            memset(psms_store->pszContent,0,psms_store->fix.Conlen);
            
            if(-1 == read(f,psms_store->pszContent,psms_store->fix.Conlen))
            {
                SMS_FREE(psms_store->pszPhone);
                SMS_FREE(psms_store->pszContent);
                close(f);
                return FALSE;
            } 
        }
        else
        {
            char* pszCon = NULL;
            
            pszCon = (char*)malloc(psms_store->fix.Conlen);
            
            if(pszCon == NULL)
            {
                SMS_FREE(psms_store->pszPhone);
                close(f);
                return FALSE;
            }
            
            memset(pszCon,0,psms_store->fix.Conlen);
            
            if(-1 == read(f,pszCon,psms_store->fix.Conlen))
            {
                SMS_FREE(psms_store->pszPhone);
                SMS_FREE(pszCon);
                close(f);
                return FALSE;
            } 
            
            if(SMS_ParseContent(psms_store->fix.dcs,pszCon,psms_store->fix.Conlen,psms_store) == FALSE)
            {
                SMS_FREE(psms_store->pszPhone);
                SMS_FREE(pszCon);
                close(f);
                return FALSE;
            }
            SMS_FREE(pszCon);
        }
    }

    if( psms_store->fix.Udhlen > 0)
    {
        psms_store->pszUDH = (char*)malloc(psms_store->fix.Udhlen);
        
        if( psms_store->pszUDH == NULL )
        {
            SMS_FREE(psms_store->pszPhone);
            SMS_FREE(psms_store->pszContent);
            close(f);
            return FALSE;
        }
        
        if(-1 == read(f,psms_store->pszUDH,psms_store->fix.Udhlen))
        {
            SMS_FREE(psms_store->pszPhone);
            SMS_FREE(psms_store->pszContent);
            SMS_FREE(psms_store->pszUDH);
            close(f);
            return FALSE;
        }      
    }

    close(f);

    return TRUE;
}
/*********************************************************************\
* Function	   SMS_ReadStatusFromFile
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_ReadStatusFromFile(char* pszFileName,SMS_STOREFIX *psms_storefix)
{
    int f;

    f = open(pszFileName,O_RDONLY);
    if(f == -1)
        return FALSE;
    
#ifdef _SMS_DEBUG_
    printf("\r\n*****SMS Debug Info*****  SMS_ReadStatusFromFile  file handle = %d \r\n",f);
#endif
    
    if(read(f,psms_storefix,sizeof(SMS_STOREFIX)) == -1)
    {
        close(f);
        return FALSE;
    }
    close(f);

    return TRUE;
}
/*********************************************************************\
* Function	   SMS_ModifyFileStatus
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_ModifyFileStatus(char* pszFileName,SMS_STOREFIX *psms_storefix)
{
    int f;

    f = open(pszFileName,O_WRONLY);

    if(f == -1)
        return FALSE;

#ifdef _SMS_DEBUG_
    printf("\r\n*****SMS Debug Info*****  SMS_ModifyFileStatus  file handle = %d \r\n",f);
#endif
    
    if(write(f,psms_storefix,sizeof(SMS_STOREFIX)) == -1)
    {
        close(f);
        return FALSE;
    }
    close(f);

    return TRUE;
}
/*********************************************************************\
* Function	   SMS_ReadDataFromOffset
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL SMS_ReadDataFromOffset(char* pszFileName,DWORD dwoffset,SMS_STORE *psms_store,BOOL bOrg)
{
    int f;
    int nreadret;
    BYTE bydel;
    DWORD datalen;

    f = open(pszFileName,O_RDONLY);
    if(f == -1)
        return FALSE;
    
#ifdef _SMS_DEBUG_
    printf("\r\n*****SMS Debug Info*****  SMS_ReadDataFromOffset file handle = %d \r\n",f);
#endif
    
    lseek(f,dwoffset,SEEK_SET);

    nreadret = read(f,&bydel,sizeof(BYTE));
    
    if(-1 == nreadret)
    {
        close(f);
        return FALSE;
    }
    
    if(0 == nreadret) //read end 
    {
        close(f);
        return FALSE;
    }
    
    read(f,&datalen,sizeof(DWORD));
    
    if(bydel != SMS_USED)
    {
        close(f);
        return FALSE;
    }
    
    if(read(f,&(psms_store->fix),sizeof(SMS_STOREFIX)) == -1)
    {
        close(f);
        return FALSE;
    }
    
    if( psms_store->fix.Phonelen > 0)
    {
        psms_store->pszPhone = (char*)malloc(psms_store->fix.Phonelen);
        
        if( psms_store->pszPhone == NULL )
        {
            close(f);
            return FALSE;
        }

        memset(psms_store->pszPhone,0,psms_store->fix.Phonelen);
        
        if(-1 == read(f,psms_store->pszPhone,psms_store->fix.Phonelen))
        {
            SMS_FREE(psms_store->pszPhone);
            close(f);
            return FALSE;
        }
    }

    if( psms_store->fix.Conlen > 0)
    {
        if(bOrg)
        {
            psms_store->pszContent = (char*)malloc(psms_store->fix.Conlen);
            
            if(psms_store->pszContent == NULL)
            {
                SMS_FREE(psms_store->pszPhone);
                close(f);
                return FALSE;
            }
            
            memset(psms_store->pszContent,0,psms_store->fix.Conlen);
            
            if(-1 == read(f,psms_store->pszContent,psms_store->fix.Conlen))
            {
                SMS_FREE(psms_store->pszPhone);
                SMS_FREE(psms_store->pszContent);
                close(f);
                return FALSE;
            } 
        }
        else
        {
            char* pszCon = NULL;
            
            pszCon = (char*)malloc(psms_store->fix.Conlen);
            
            if(pszCon == NULL)
            {
                SMS_FREE(psms_store->pszPhone);
                close(f);
                return FALSE;
            }
            
            memset(pszCon,0,psms_store->fix.Conlen);
            
            if(-1 == read(f,pszCon,psms_store->fix.Conlen))
            {
                SMS_FREE(psms_store->pszPhone);
                SMS_FREE(pszCon);
                close(f);
                return FALSE;
            } 
            
            if(SMS_ParseContent(psms_store->fix.dcs,pszCon,psms_store->fix.Conlen,psms_store) == FALSE)
            {
                SMS_FREE(psms_store->pszPhone);
                SMS_FREE(pszCon);
                close(f);
                return FALSE;
            }
            SMS_FREE(pszCon);
        }

    }

    if( psms_store->fix.Udhlen > 0)
    {
        psms_store->pszUDH = (char*)malloc(psms_store->fix.Udhlen);
        
        if( psms_store->pszUDH == NULL )
        {
            SMS_FREE(psms_store->pszPhone);
            SMS_FREE(psms_store->pszContent);
            close(f);
            return FALSE;
        }
        
        memset(psms_store->pszUDH,0,psms_store->fix.Udhlen);

        if(-1 == read(f,psms_store->pszUDH,psms_store->fix.Udhlen))
        {
            SMS_FREE(psms_store->pszPhone);
            SMS_FREE(psms_store->pszContent);
            SMS_FREE(psms_store->pszUDH);
            close(f);
            return FALSE;
        }      
    }
    
    close(f);
    return TRUE;
}
/*********************************************************************\
* Function	   String2DWORD
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD String2DWORD(char* pszTime)
{
    SYSTEMTIME	SystemTime;
    DWORD	dwTime;
    char szTempTime[ME_SMS_TIME_LEN],szTempYear[10];
    const char* p;
    
    memset(szTempTime,0,ME_SMS_TIME_LEN);
    strcpy(szTempTime,pszTime);
    p = szTempTime;
    p += 10;
    szTempTime[12] = 0;
    SystemTime.wSecond = (WORD)atoi((const char*)p);
    p -= 2;
    szTempTime[10] = 0;
    SystemTime.wMinute = (WORD)atoi((const char*)p);
    p -= 2;
    szTempTime[8] = 0;
    SystemTime.wHour = (WORD)atoi((const char*)p);
    p -= 2;
    szTempTime[6] = 0;
    SystemTime.wDay = (WORD)atoi((const char*)p);
    p -= 2;
	szTempTime[4] = 0;
	SystemTime.wMonth = (WORD)atoi((const char*)p);
	p -= 2;
	szTempTime[2] = 0;
	szTempYear[0] = 0;	
	strcpy(szTempYear,"20");
	strcat(szTempYear,szTempTime);
	SystemTime.wYear = (WORD)atoi((const char*)szTempYear);
	
    dwTime = Datetime2INT(SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,
        SystemTime.wMinute,SystemTime.wSecond);

	return(dwTime);
}
/*********************************************************************\
* Function	   SMS_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static SMS_INDEX* SMS_New(void)
{
    SMS_INDEX* p = NULL;

    p = (SMS_INDEX*)SMSMM_malloc();
    memset(p,0,sizeof(SMS_INDEX));

    return p;
}
/*********************************************************************\
* Function	   SMS_Insert
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMS_Insert(SMS_INDEX* pChainNode)
{
    SMS_INDEX* p;

    if(pHeader)
    {
        p = pHeader;

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
        pHeader = pChainNode;
        pChainNode->pNext = NULL;
        pChainNode->pPioneer = NULL;
    }
}
/*********************************************************************\
* Function	   SMS_Delete
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMS_Delete(SMS_INDEX* pChainNode)
{
    if( pChainNode == pHeader )
    {
        if(pChainNode->pNext)
        {
            pHeader = pChainNode->pNext;
            pChainNode->pNext->pPioneer = NULL;
        }
        else
            pHeader = NULL;
    }
    else if( pChainNode->pNext == NULL )
    {
        pChainNode->pPioneer->pNext = NULL;
    }
    else
    {
        pChainNode->pPioneer->pNext = pChainNode->pNext;
        pChainNode->pNext->pPioneer = pChainNode->pPioneer;
    }

    SMSMM_free(pChainNode);
    pChainNode = NULL;
}
/*********************************************************************\
* Function	   SMS_Erase
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMS_Erase(void)
{
    SMS_INDEX* p;    
    SMS_INDEX* ptemp;

    p = pHeader;

    while( p )
    {
        ptemp = p->pNext;       
        SMSMM_free(p);
        p = ptemp;
    }

    pHeader = NULL;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static SMS_INDEX* SMS_FindByFileName(char* szFileName)
{
    SMS_INDEX* ptemp;

    ptemp = pHeader;

    while( ptemp )
    {
        if(ptemp->byStoreType == STORE_FILE)
        {
            if(stricmp(szFileName,ptemp->szFileName) == 0)
                return ptemp;
        }
        ptemp = ptemp->pNext;  
    }

    return NULL;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL SMS_SearchRecord(MU_MsgNode *pmsgnode)
{
    BYTE bydel;
    DWORD   datalen;
    int  nreadret;
    SMS_STORE sms_store;
    SMS_INDEX* pChainNode;
    
    if(hFile == -2)
        return FALSE;
    
    while( TRUE )
    {
        
#ifndef _EMULATE_
        KickDog();
#endif
        nreadret = read(hFile,&bydel,sizeof(BYTE));
        
        if(-1 == nreadret)
        {
            close(hFile);
            return FALSE;
        }
        
        if(0 == nreadret) //read end 
        {
            close(hFile);
            return FALSE;
        }
        
        read(hFile,&datalen,sizeof(DWORD));
        
        if(bydel != SMS_USED)
        {
            lseek(hFile,datalen,SEEK_CUR);
            dwOffset += sizeof(BYTE) + sizeof(DWORD) + datalen;
            continue;
        }
        
        pChainNode = SMS_New();
        if(pChainNode == NULL)
        {
            close(hFile);
            return FALSE;
        }
        
        pChainNode->byStoreType = STORE_OFFSET;
        pChainNode->dwOffest = dwOffset;
        SMS_Insert(pChainNode);
        
        memset(&sms_store,0,sizeof(SMS_STORE));
        
        if(read(hFile,&(sms_store.fix),sizeof(SMS_STOREFIX)) == -1)
        {
            close(hFile);
            return FALSE;
        }
        
        if( sms_store.fix.Phonelen > 0)
        {
			int nLen;
			
            sms_store.pszPhone = (char*)malloc(sms_store.fix.Phonelen);
            
            if( sms_store.pszPhone == NULL )
            {
                close(hFile);
                return FALSE;
            }
            memset(sms_store.pszPhone,0,sms_store.fix.Phonelen);
            
            if(-1 == read(hFile,sms_store.pszPhone,sms_store.fix.Phonelen))
            {
                close(hFile);
                return FALSE;
            }
            
			nLen = sms_store.fix.Phonelen > MU_ADDR_MAX_LEN ? MU_ADDR_MAX_LEN : sms_store.fix.Phonelen;

            strncpy(pmsgnode->addr,sms_store.pszPhone,nLen);
            pmsgnode->addr[MU_ADDR_MAX_LEN] = 0;
            
            free(sms_store.pszPhone);
            sms_store.pszPhone = NULL;
            
        }
        else
            pmsgnode->addr[0] = 0;
        
        if( sms_store.fix.Conlen > 0)
        {
            char* pszCon = NULL;
            int nLen;
            
            pszCon = (char*)malloc(sms_store.fix.Conlen);
            
            if(pszCon == NULL)
            {
                SMS_FREE(sms_store.pszPhone);
                close(hFile);
                return FALSE;
            }
            
            memset(pszCon,0,sms_store.fix.Conlen);
            
            if(-1 == read(hFile,pszCon,sms_store.fix.Conlen))
            {
                SMS_FREE(sms_store.pszPhone);
                SMS_FREE(pszCon);
                close(hFile);
                return FALSE;
            } 
            
            if(SMS_ParseContent(sms_store.fix.dcs,pszCon,sms_store.fix.Conlen,&sms_store) == FALSE)
            {
                SMS_FREE(sms_store.pszPhone);
                SMS_FREE(pszCon);
                close(hFile);
                return FALSE;
            }   
            
            SMS_FREE(pszCon);  

            if(pmsgnode->msgtype != MU_MSG_STATUS_REPORT)
            {
                pmsgnode->msgtype = SMS_IsVcradOrVcal(&sms_store);

                switch(pmsgnode->msgtype)
                {
                case MU_MSG_VCARD:
                    strcpy(pmsgnode->subject,IDS_VCARD);
                	break;

                case MU_MSG_VCAL:
                    strcpy(pmsgnode->subject,IDS_VCAL);
                	break;

                case MU_MSG_SMS:
                default:        
                    nLen = sms_store.fix.Conlen > MU_SUBJECT_DIS_LEN ? MU_SUBJECT_DIS_LEN : sms_store.fix.Conlen;
                    
                    strncpy(pmsgnode->subject,sms_store.pszContent,nLen);
                    pmsgnode->subject[MU_SUBJECT_DIS_LEN] = 0;
                    break;
                }
            }

            free(sms_store.pszContent);
            sms_store.pszContent = NULL;
            
        }
        else
            pmsgnode->subject[0] = 0;
        
        pmsgnode->handle = (DWORD)pChainNode;
        pmsgnode->storage_type = MU_STORE_IN_FLASH;
        if(pmsgnode->msgtype == MU_MSG_STATUS_REPORT)
        {
            if(sms_store.fix.Status == -1)
            {
                Report_InsertHandle(sms_store.fix.MR,(DWORD)pChainNode);
                pmsgnode->status = MU_STU_WAITING;
            }
            else if(sms_store.fix.Status == 0)
                pmsgnode->status = MU_STU_CONFIRM;
            else
                pmsgnode->status = MU_STU_FAILED;
        }
        else
            pmsgnode->status = sms_store.fix.Stat; // attention
        pmsgnode->maskdate = sms_store.fix.dwDateTime;        
        
        dwOffset += sizeof(BYTE) + sizeof(DWORD) + datalen;

        lseek(hFile,dwOffset,SEEK_SET);
        
        return TRUE;
    }

    return FALSE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_release_messages(HWND hwndmu)
{
    chdir(szOldPath);

    dwOffset = 0;

    SMS_Erase();

    SMSMM_Destroy();

    smsapi_SetMuHwnd(NULL);

    pHeader = NULL;

    SMS_ResetALLFile();

    SMS_SetMEStatus(FALSE);
    
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_read_message(HWND hwndmu , DWORD handle,BOOL bPre, BOOL bNext)
{
    BOOL bRet;
    SMS_STORE smsstore;
    char* pszFileName;
    char  szFileName[PATH_MAXLEN];
    char szOldPath [PATH_MAXLEN];
    int nMsgType;
    SMS_INFO* pMESMSInfo = NULL;

    memset(&smsstore,0,sizeof(SMS_STORE));

    if(((SMS_INDEX*)handle)->byStoreType == STORE_FILE)
    {
        szOldPath[0] = 0;      
        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_SMS);  
        bRet = SMS_ReadDataFromFile(((SMS_INDEX*)handle)->szFileName,&smsstore,FALSE);
        chdir(szOldPath);  
        if( !bRet )
        {
            SMS_FREE(smsstore.pszContent);
            SMS_FREE(smsstore.pszUDH);
            SMS_FREE(smsstore.pszPhone);
            return FALSE;
        }
    }
    else if(((SMS_INDEX*)handle)->byStoreType == STORE_OFFSET)
    {
        switch(nFolderID)
        {   
        case MU_INBOX: // a big file
            pszFileName = SMS_FILENAME_INBOX;
            break;
            
        case MU_SENT:
            pszFileName = SMS_FILENAME_SENT;
            break;
            
        case MU_REPORT:
            pszFileName = SMS_FILENAME_REPORT;
            break;
            
        case MU_MYFOLDER:
            pszFileName = SMS_FILENAME_MYFOLDER;
            break;
            
        default:
            szFileName[0] = 0;
            sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nFolderID,SMS_FILE_POSTFIX);
            pszFileName = szFileName;
        }

        szOldPath[0] = 0;      
        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_SMS);  
        bRet = SMS_ReadDataFromOffset(pszFileName,((SMS_INDEX*)handle)->dwOffest,&smsstore,FALSE);
        chdir(szOldPath);  
        if( !bRet )
        {
            SMS_FREE(smsstore.pszContent);
            SMS_FREE(smsstore.pszUDH);
            SMS_FREE(smsstore.pszPhone);
              
            return FALSE;
        }
    }
    else
    {
        SMS_INITDATA *pData;
        int i;
        
        pData = SMS_GetInitData();
        
        for(i = 0 ; i < pData->nMEMaxCount ; i++)
        {
            if(pData->pMESMSInfo[i].Index == ((SMS_INDEX*)handle)->index)
            {
                pMESMSInfo = &(pData->pMESMSInfo[i]);
                
                break;
            }
        }

        if(pMESMSInfo == NULL)
            return FALSE;

        if(FALSE == SMS_ChangeStruct(&smsstore,pMESMSInfo))
            return FALSE;
    }

    nMsgType = SMS_IsVcradOrVcal(&smsstore);
    
    if(nFolderID != MU_DRAFT)
    {
        switch(nMsgType)
        {
        case MU_MSG_SMS:
            SMS_CreateViewWnd(hwndmu,&smsstore,handle,nFolderID,bPre,bNext);
            break;

        case MU_MSG_VCARD:
        case MU_MSG_VCAL:
            SMS_CreateViewVcardOrVcalWnd(MuGetFrame(),hwndmu,&smsstore,nMsgType,handle,nFolderID,bPre,bNext);
        	break;

        case MU_MSG_STATUS_REPORT:
            SMS_CreateReportWnd(hwndmu,&smsstore);
            break;

        default:
            break;
        }
    }
    else
    {
        SMS_CreateEditWnd(MuGetFrame(),hwndmu,smsstore.pszPhone,smsstore.pszContent,smsstore.fix.Conlen,
            handle,nFolderID,nMsgType,NULL,FALSE);
    }

    if(smsstore.fix.Type != STATUS_REPORT && smsstore.fix.Stat == MU_STU_UNREAD)
    {
        MU_MsgNode msgnode; 
        int nLen = 0;
            
        memset(&msgnode,0,sizeof(MU_MsgNode));

        if(pMESMSInfo != NULL && pMESMSInfo->Stat == SMS_UNREAD)
        {
            ME_SMS_Read(NULL,0, FALSE, SMS_MEM_MT,((SMS_INDEX*)handle)->index);
            pMESMSInfo->Stat = SMS_READED;
        }
        
        if(pMESMSInfo == NULL)
        {
            if(SMS_ModifyRecordStatus(pszFileName,&(((SMS_INDEX*)handle)->dwOffest),MU_STU_READ) == FALSE)
            {
                SMS_FREE(smsstore.pszContent);
                SMS_FREE(smsstore.pszUDH);
                SMS_FREE(smsstore.pszPhone);
                return FALSE;
            }
        }
        
        smsstore.fix.Stat = MU_STU_READ;

        SMS_ChangeCount(nFolderID,SMS_COUNT_ALL,1);
        SMS_ChangeCount(nFolderID,SMS_COUNT_UNREAD,-1);

        SMS_ChangeCount(nFolderID,SMS_COUNT_DEL,1);
        SMS_ChangeCount(nFolderID,SMS_COUNT_ALL,-1);
        
        msgnode.handle = handle;
        msgnode.storage_type = MU_STORE_IN_FLASH;
        if(smsstore.fix.Stat == MU_MSG_STATUS_REPORT)
        {
            if(smsstore.fix.Status == -1)
                msgnode.status = MU_STU_WAITING;
            else if(smsstore.fix.Status == 0)
                msgnode.status = MU_STU_CONFIRM;
            else if(smsstore.fix.Status == 1)
                msgnode.status = MU_STU_FAILED;
        }
        else
            msgnode.status = smsstore.fix.Stat;
        msgnode.maskdate = smsstore.fix.dwDateTime;   
        msgnode.msgtype = nMsgType;

        if(smsstore.pszContent != NULL)
        {
            switch(msgnode.msgtype)
            {
            case MU_MSG_VCARD:
                strcpy(msgnode.subject,IDS_VCARD);
                break;
                
            case MU_MSG_VCAL:
                strcpy(msgnode.subject,IDS_VCAL);
                break;
                
            case MU_MSG_SMS:
            default:        
                nLen = smsstore.fix.Conlen > MU_SUBJECT_DIS_LEN ? MU_SUBJECT_DIS_LEN  : smsstore.fix.Conlen;
                strncpy(msgnode.subject,smsstore.pszContent,nLen);
                break;
            }
        }
        msgnode.subject[MU_SUBJECT_DIS_LEN] = 0;
        nLen = smsstore.fix.Phonelen > MU_ADDR_MAX_LEN ? MU_ADDR_MAX_LEN  : smsstore.fix.Phonelen;
        strncpy(msgnode.addr,smsstore.pszPhone,nLen);
        msgnode.addr[MU_ADDR_MAX_LEN] = 0;

        SendMessage(hwndmu,PWM_MSG_MU_MODIFIED,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)&msgnode);
        
        SMS_NotifyIdle();
    }

    SMS_FREE(smsstore.pszContent);
    SMS_FREE(smsstore.pszUDH);
    SMS_FREE(smsstore.pszPhone);

    return bRet;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_delete_message(HWND hwndmu , DWORD handle)
{
    char* pszFileName;
    char  szFileName[SMS_FILENAME_MAXLEN];
    int   ret;
    SMS_INDEX* p;
    SMS_STORE smsstore;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  

    p = (SMS_INDEX*)handle;

    if(p->byStoreType == STORE_FILE)
    {
        ret = remove(p->szFileName);
        if( ret == -1 )
        {
            SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),handle);
            chdir(szOldPath);  
            return FALSE;
        }
        else
        {
            SMS_ChangeCount(nFolderID,SMS_COUNT_ALL,-1);
            
            if(nFolderID == MU_OUTBOX)
                SMS_NotifyIdle();

            SMS_Delete((SMS_INDEX*)handle);
            SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),handle);
            chdir(szOldPath);  

            SMS_TransferData();
            
            return TRUE;
        }
    }
    else if(((SMS_INDEX*)handle)->byStoreType == STORE_OFFSET)
    {
        switch(nFolderID)
        {   
        case MU_INBOX: // a big file
            pszFileName = SMS_FILENAME_INBOX;
            break;
            
        case MU_SENT:
            pszFileName = SMS_FILENAME_SENT;
            break;
            
        case MU_REPORT:
            pszFileName = SMS_FILENAME_REPORT;
            break;
            
        case MU_MYFOLDER:
            pszFileName = SMS_FILENAME_MYFOLDER;
            break;
            
        default:
            szFileName[0] = 0;
            sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nFolderID,SMS_FILE_POSTFIX);
            pszFileName = szFileName;
        }
        
        memset(&smsstore,0,sizeof(SMS_STORE));

        if(SMS_ReadDataFromOffset(pszFileName,((SMS_INDEX*)handle)->dwOffest,&smsstore,FALSE) == FALSE)
        {
            
            SMS_FREE(smsstore.pszContent);
            SMS_FREE(smsstore.pszUDH);
            SMS_FREE(smsstore.pszPhone);

            SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),handle);
            chdir(szOldPath);  
            return FALSE;
        }

        ret = DeleteWithOffset(pszFileName,((SMS_INDEX*)handle)->dwOffest);

        if( ret == -1 )
        {
            SMS_FREE(smsstore.pszContent);
            SMS_FREE(smsstore.pszUDH);
            SMS_FREE(smsstore.pszPhone);
            
            SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),handle);
            chdir(szOldPath);  
            return FALSE;
        }
        else
        {
            SMS_ChangeCount(nFolderID,SMS_COUNT_ALL,-1);
            SMS_ChangeCount(nFolderID,SMS_COUNT_DEL,1);
            if(smsstore.fix.Stat == MU_STU_UNREAD)
                SMS_ChangeCount(nFolderID,SMS_COUNT_UNREAD,-1);
            SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),handle);
            SMS_Delete((SMS_INDEX*)handle);
            chdir(szOldPath); 

            SMS_TransferData();

            SMS_FREE(smsstore.pszContent);
            SMS_FREE(smsstore.pszUDH);
            SMS_FREE(smsstore.pszPhone);
            
            return TRUE;
        }
    }
    else
    {
        SMS_INITDATA *pData;
        int i;
        SMS_INFO* pMESMSInfo = NULL;
        MSG myMsg;
        MESTORE_INFO MEStoreInfo;
        int stat;

        pData = SMS_GetInitData();
        
        for(i = 0 ; i < pData->nMEMaxCount ; i++)
        {
            if(pData->pMESMSInfo[i].Index == ((SMS_INDEX*)handle)->index)
            {
                pMESMSInfo = &(pData->pMESMSInfo[i]);
                
                break;
            }
        }

        if(pMESMSInfo == NULL)
        {
            chdir(szOldPath);  
            return FALSE;
        }

        memset(&MEStoreInfo,0,sizeof(MESTORE_INFO));
        
        SMS_ReadOneMEStore(&MEStoreInfo,pMESMSInfo->Index);
        
        stat = pMESMSInfo->Stat;

        if(SIM_Delete(pMESMSInfo->Index,SMS_MEM_MT))
        {
            WaitWindowStateEx(NULL,TRUE,IDS_DELETING,IDS_SMS,NULL,NULL);

            while(GetMessage(&myMsg, NULL, 0, 0)) 
            {
                if((myMsg.message == MSG_SIM_DELETE_SUCC) && (myMsg.hwnd == SMS_GetSIMCtrlWnd()))
                {
                    pData->nMECount--;
        
                    SMS_ChangeCount(nFolderID,SMS_COUNT_ALL,-1);
                    if(stat == SMS_UNREAD)
                        SMS_ChangeCount(nFolderID,SMS_COUNT_UNREAD,-1);                    

                    SMS_SetFull(FALSE);

                    SMS_NotifyIdle();
                    
                    SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),handle);
                    
                    chdir(szOldPath);  

                    MEStoreInfo.nfolderid = 0;

                    SMS_WriteOneMEStore(&MEStoreInfo,pMESMSInfo->Index);

                    memset(pMESMSInfo,0,sizeof(SMS_INFO));

                    WaitWindowStateEx(NULL,FALSE,IDS_DELETING,IDS_SMS,NULL,NULL);
                    
                    return TRUE;
                }
                else if(myMsg.hwnd == SMS_GetSIMCtrlWnd())
                {
                    SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),handle);
                    
                    chdir(szOldPath);
                    
                    WaitWindowStateEx(NULL,FALSE,IDS_DELETING,IDS_SMS,NULL,NULL);

                    return FALSE;
                }
                TranslateMessage(&myMsg);
                DispatchMessage(&myMsg);
            }
        }     
    }
    
    chdir(szOldPath);  
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int DeleteWithOffset(char* pszFileName,DWORD dwOffest)
{
    int f;
    BYTE byDel;
  
    {
        struct stat buf;
        
        memset(&buf,0,sizeof(struct stat));
        stat(pszFileName,(struct stat*)&buf);

        printf("\r\n Delete file %s ,from offset %d, its size is %d \r\n",pszFileName,dwOffest,buf.st_size);
    }

    f = open(pszFileName,O_WRONLY);
    if(f == -1)
        return -1;

#ifdef _SMS_DEBUG_
    printf("\r\n*****SMS Debug Info*****  DeleteWithOffset file handle = %d \r\n",f);
#endif
    
    lseek(f,dwOffest,SEEK_SET);

    byDel = SMS_DEL;

    write(f,&byDel,sizeof(BYTE));

    close(f);

    return 1;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_delete_multi_messages(HWND hwndmu , DWORD* phandle,int nCount)
{
    char* pszFileName;
    char  szFileName[SMS_FILENAME_MAXLEN];
    int   ret;
    SMS_INDEX* p;
    SMS_STORE smsstore;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  

    p = (SMS_INDEX*)(*phandle);

    if(p->byStoreType == STORE_FILE)
    {
        ret = remove(p->szFileName);
        if( ret == -1 )
        {
            SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),(LPARAM)(*phandle));
            chdir(szOldPath);  
            return FALSE;
        }
        else
        {
            SMS_ChangeCount(nFolderID,SMS_COUNT_ALL,-1);
            
            if(nFolderID == MU_OUTBOX)
                SMS_NotifyIdle();

            SMS_Delete((SMS_INDEX*)(*phandle));
            SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)(*phandle));
            chdir(szOldPath);  

            SMS_TransferData();
            
            return TRUE;
        }
    }
    else if(((SMS_INDEX*)(*phandle))->byStoreType == STORE_OFFSET)
    {
        switch(nFolderID)
        {   
        case MU_INBOX: // a big file
            pszFileName = SMS_FILENAME_INBOX;
            break;
            
        case MU_SENT:
            pszFileName = SMS_FILENAME_SENT;
            break;
            
        case MU_REPORT:
            pszFileName = SMS_FILENAME_REPORT;
            break;
            
        case MU_MYFOLDER:
            pszFileName = SMS_FILENAME_MYFOLDER;
            break;
            
        default:
            szFileName[0] = 0;
            sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nFolderID,SMS_FILE_POSTFIX);
            pszFileName = szFileName;
        }
        
        memset(&smsstore,0,sizeof(SMS_STORE));

        if(SMS_ReadDataFromOffset(pszFileName,((SMS_INDEX*)(*phandle))->dwOffest,&smsstore,FALSE) == FALSE)
        {
            
            SMS_FREE(smsstore.pszContent);
            SMS_FREE(smsstore.pszUDH);
            SMS_FREE(smsstore.pszPhone);

            SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),(LPARAM)(*phandle));
            chdir(szOldPath);  
            return FALSE;
        }

        ret = DeleteWithOffset(pszFileName,((SMS_INDEX*)(*phandle))->dwOffest);

        if( ret == -1 )
        {
            SMS_FREE(smsstore.pszContent);
            SMS_FREE(smsstore.pszUDH);
            SMS_FREE(smsstore.pszPhone);
            
            SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),(LPARAM)(*phandle));
            chdir(szOldPath);  
            return FALSE;
        }
        else
        {
            SMS_ChangeCount(nFolderID,SMS_COUNT_ALL,-1);
            SMS_ChangeCount(nFolderID,SMS_COUNT_DEL,1);
            if(smsstore.fix.Stat == MU_STU_UNREAD)
                SMS_ChangeCount(nFolderID,SMS_COUNT_UNREAD,-1);
            SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)(*phandle));
            SMS_Delete((SMS_INDEX*)(*phandle));
            chdir(szOldPath); 

            SMS_TransferData();

            SMS_FREE(smsstore.pszContent);
            SMS_FREE(smsstore.pszUDH);
            SMS_FREE(smsstore.pszPhone);
            
            return TRUE;
        }
    }
    else
    {
        SMS_INITDATA *pData;
        int i;
        SMS_INFO* pMESMSInfo = NULL;
        MSG myMsg;
        MESTORE_INFO MEStoreInfo;
        int stat;

        pData = SMS_GetInitData();
        
        for(i = 0 ; i < pData->nMEMaxCount ; i++)
        {
            if(pData->pMESMSInfo[i].Index == ((SMS_INDEX*)(*phandle))->index)
            {
                pMESMSInfo = &(pData->pMESMSInfo[i]);
                
                break;
            }
        }

        if(pMESMSInfo == NULL)
        {
            chdir(szOldPath);  
            return FALSE;
        }

        memset(&MEStoreInfo,0,sizeof(MESTORE_INFO));
        
        SMS_ReadOneMEStore(&MEStoreInfo,pMESMSInfo->Index);
        
        stat = pMESMSInfo->Stat;

        if(SIM_Delete(pMESMSInfo->Index,SMS_MEM_MT))
        {
            while(GetMessage(&myMsg, NULL, 0, 0)) 
            {
                if((myMsg.message == MSG_SIM_DELETE_SUCC) && (myMsg.hwnd == SMS_GetSIMCtrlWnd()))
                {
                    pData->nMECount--;
        
                    SMS_ChangeCount(nFolderID,SMS_COUNT_ALL,-1);
                    if(stat == SMS_UNREAD)
                        SMS_ChangeCount(nFolderID,SMS_COUNT_UNREAD,-1);                    

                    SMS_SetFull(FALSE);

                    SMS_NotifyIdle();
                    
                    SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)(*phandle));
                    
                    chdir(szOldPath);  

                    MEStoreInfo.nfolderid = 0;

                    SMS_WriteOneMEStore(&MEStoreInfo,pMESMSInfo->Index);

                    memset(pMESMSInfo,0,sizeof(SMS_INFO));

                    return TRUE;
                }
                else if(myMsg.hwnd == SMS_GetSIMCtrlWnd())
                {
                    SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),(LPARAM)(*phandle));
                    
                    chdir(szOldPath);
                    
                    return FALSE;
                }
                else if(myMsg.message == WM_KEYDOWN && myMsg.wParam == VK_F10)
                {
                    pData->nMECount--;
        
                    SMS_ChangeCount(nFolderID,SMS_COUNT_ALL,-1);
                    if(stat == SMS_UNREAD)
                        SMS_ChangeCount(nFolderID,SMS_COUNT_UNREAD,-1);                    

                    SMS_SetFull(FALSE);

                    SMS_NotifyIdle();
                    
                    SendMessage(hwndmu,PWM_MSG_MU_DELETED_RESP,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)(*phandle));
                    
                    chdir(szOldPath);  

                    MEStoreInfo.nfolderid = 0;

                    SMS_WriteOneMEStore(&MEStoreInfo,pMESMSInfo->Index);

                    memset(pMESMSInfo,0,sizeof(SMS_INFO));
   
                    chdir(szOldPath);

                    return FALSE;
                }
                TranslateMessage(&myMsg);
                DispatchMessage(&myMsg);
            }
        }     
    }
    
    chdir(szOldPath);  
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_delete_all_messages(HWND hwndmu , int folder ,MU_DELALL_TYPE ntype)
{
    char* pszFileName;
    char  szFileName[SMS_FILENAME_MAXLEN];
    int   ret;
    MESTORE_INFO *pMEStoreInfo = NULL;
    SMS_INITDATA *pData = NULL;
    int i;
    int nstat;
    int nCount,nUnread;
        
    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  

    pData = SMS_GetInitData();
    
    pMEStoreInfo = (MESTORE_INFO*)malloc(sizeof(MESTORE_INFO)*(pData->nMEMaxCount));

    if(pMEStoreInfo == NULL)
    {    
        SendMessage(hwndmu,PWM_MSG_MU_DELALL_RESP,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),(LPARAM)folder);
        chdir(szOldPath);  
        return FALSE;
    }

    memset(pMEStoreInfo,0,sizeof(MESTORE_INFO)*(pData->nMEMaxCount));

    SMS_ReadMEStoreInfo(pMEStoreInfo,pData->nMEMaxCount);

    sms_get_count(folder,&nUnread,&nCount);

    printf("\r\n ###### delete all begin ######\r\n");

    for(i = 0 ; i < pData->nMEMaxCount ; i++)
    {
        if(pMEStoreInfo[i].nfolderid == folder)
        {
            printf("\r\n ###### delete ME SMS ###### index = %d\r\n",i+1);
            nstat = pData->pMESMSInfo[i].Stat;
            if(SMS_DeleteMESMS(i+1))
            {
                pMEStoreInfo[i].nfolderid = 0;
                SMS_ChangeCount(folder,SMS_COUNT_ALL,-1);
                nCount--;
                if(nstat == SMS_UNREAD)
                {
                    SMS_ChangeCount(folder,SMS_COUNT_UNREAD,-1);
                    nUnread--;
                }
            }
        }
    }

    printf("\r\n ###### delete all end ######\r\n");

    SMS_WriteMEStoreInfo(pMEStoreInfo,pData->nMEMaxCount);

    SMS_FREE(pMEStoreInfo);

    switch(folder)
    {   
    case MU_INBOX: // a big file
        pszFileName = SMS_FILENAME_INBOX;
        break;
        
    case MU_SENT:
        pszFileName = SMS_FILENAME_SENT;
        break;
        
    case MU_REPORT:
        pszFileName = SMS_FILENAME_REPORT;
        break;
        
    case MU_MYFOLDER:
        pszFileName = SMS_FILENAME_MYFOLDER;
        break;

    case MU_DRAFT:
        chdir(szOldPath);  
        return SMS_Clear(MU_DRAFT);

    case MU_OUTBOX:
        chdir(szOldPath);  
        return FALSE;
        
    default:
        szFileName[0] = 0;
        sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,folder,SMS_FILE_POSTFIX);
        pszFileName = szFileName;
    }

    ret = remove(pszFileName);

    if( ret == -1 )
    {
        
        if(ntype == MU_DELALL_FOLDER)
        {
            SMS_DeleteCount(folder);
        }
        else
        {
            SMS_ChangeCount(folder,SMS_COUNT_DEL,0);
            SMS_ChangeCount(folder,SMS_COUNT_ALL,(-1)*nCount);
            SMS_ChangeCount(folder,SMS_COUNT_UNREAD,(-1)*nUnread);
        }

        SMS_SetFull(FALSE);
        
        SMS_NotifyIdle();
        
        SendMessage(hwndmu,PWM_MSG_MU_DELALL_RESP,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)folder);
        chdir(szOldPath);  
        return FALSE;
    }
    else
    {
        SMS_TransferData();

        if(folder == MU_REPORT)
            Report_Erase();

        if(ntype == MU_DELALL_FOLDER)
        {
            SMS_DeleteCount(folder);
        }
        else
        {
            SMS_ChangeCount(folder,SMS_COUNT_DEL,0);
            SMS_ChangeCount(folder,SMS_COUNT_ALL,(-1)*nCount);
            SMS_ChangeCount(folder,SMS_COUNT_UNREAD,(-1)*nUnread);
        }
        
        SMS_SetFull(FALSE);
        
        SMS_NotifyIdle();

        SendMessage(hwndmu,PWM_MSG_MU_DELALL_RESP,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)folder);
        chdir(szOldPath);  
        return TRUE;
    }

    chdir(szOldPath);  
    return TRUE;
}
 
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL SMS_DeleteMESMS(int index)
{
    SMS_INITDATA *pData;
    int i;
    SMS_INFO* pMESMSInfo = NULL;
    MSG myMsg;
    
    pData = SMS_GetInitData();
    
    for(i = 0 ; i < pData->nMEMaxCount ; i++)
    {
        if(pData->pMESMSInfo[i].Index == index)
        {
            pMESMSInfo = &(pData->pMESMSInfo[i]);
            
            break;
        }
    }
    
    if(pMESMSInfo == NULL)
        return FALSE;
    
    if(SIM_Delete(pMESMSInfo->Index,SMS_MEM_MT))
    {        
        while(GetMessage(&myMsg, NULL, 0, 0)) 
        {
            if((myMsg.message == MSG_SIM_DELETE_SUCC) && (myMsg.hwnd == SMS_GetSIMCtrlWnd()))
            {
                memset(pMESMSInfo,0,sizeof(SMS_INFO));
                
                pData->nMECount--;

                printf("\r\n ###### Delete ME SMS ###### pData->nMECount = %d\r\n",pData->nMECount);
                
                SMS_SetFull(FALSE);

                SMS_NotifyIdle();
                
                return TRUE;
            }
            else if(myMsg.hwnd == SMS_GetSIMCtrlWnd())
            {                
                return FALSE;
            }
            else if(myMsg.message == WM_KEYDOWN && myMsg.wParam == VK_F10)
            {
                continue;
            }
            TranslateMessage(&myMsg);
            DispatchMessage(&myMsg);
        }
    } 
    
    return FALSE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_new_message(HWND hwndmu)
{
    return SMS_CreateEditWnd(MuGetFrame(),hwndmu,NULL,NULL,0,-1,-1,MU_MSG_SMS,NULL,FALSE);
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL sms_setting(HWND hFrameWnd)
{
    SMS_CreateSettingListWnd(hFrameWnd);

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_get_detail(HWND hwndmu,DWORD handle)
{
    BOOL bRet;
    SMS_STORE smsstore;
    char* pszFileName;
    char  szFileName[SMS_FILENAME_MAXLEN];

    memset(&smsstore,0,sizeof(SMS_STORE));

    if(((SMS_INDEX*)handle)->byStoreType == STORE_FILE)
    {   
        szOldPath[0] = 0;      
        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_SMS); 
        bRet = SMS_ReadDataFromFile(((SMS_INDEX*)handle)->szFileName,&smsstore,FALSE);
        chdir(szOldPath);
        if( !bRet )
            return FALSE;
    }
    else if(((SMS_INDEX*)handle)->byStoreType == STORE_OFFSET)
    {
        switch(nFolderID)
        {   
        case MU_INBOX: // a big file
            pszFileName = SMS_FILENAME_INBOX;
            break;
            
        case MU_SENT:
            pszFileName = SMS_FILENAME_SENT;
            break;
            
        case MU_REPORT:
            pszFileName = SMS_FILENAME_REPORT;
            break;
            
        case MU_MYFOLDER:
            pszFileName = SMS_FILENAME_MYFOLDER;
            break;
            
        default:
            szFileName[0] = 0;
            sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nFolderID,SMS_FILE_POSTFIX);
            pszFileName = szFileName;
        }

        szOldPath[0] = 0;      
        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_SMS); 
        bRet = SMS_ReadDataFromOffset(pszFileName,((SMS_INDEX*)handle)->dwOffest,&smsstore,FALSE);
        chdir(szOldPath);
        if( !bRet )
            return FALSE;
    }
    else
    {
        SMS_INITDATA *pData;
        int i;
        SMS_INFO* pMESMSInfo = NULL;
        
        pData = SMS_GetInitData();
        
        for(i = 0 ; i < pData->nMEMaxCount ; i++)
        {
            if(pData->pMESMSInfo[i].Index == ((SMS_INDEX*)handle)->index)
            {
                pMESMSInfo = &(pData->pMESMSInfo[i]);
                
                break;
            }
        }

        if(pMESMSInfo == NULL)
            return FALSE;

        if(FALSE == SMS_ChangeStruct(&smsstore,pMESMSInfo))
            return FALSE;
    }

    bRet = SMS_CreateDetailWnd(MuGetFrame(),&smsstore);

    SMS_FREE(smsstore.pszContent);
    SMS_FREE(smsstore.pszUDH);
    SMS_FREE(smsstore.pszPhone);

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_forward(HWND hwndmu, DWORD handle)
{
    BOOL bRet;
    SMS_STORE smsstore;
    char* pszFileName;
    char  szFileName[SMS_FILENAME_MAXLEN];

    memset(&smsstore,0,sizeof(SMS_STORE));

    if(((SMS_INDEX*)handle)->byStoreType == STORE_FILE)
    {
        szOldPath[0] = 0;      
        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_SMS); 
        bRet = SMS_ReadDataFromFile(((SMS_INDEX*)handle)->szFileName,&smsstore,FALSE);
        chdir(szOldPath);
        if( !bRet )
            return FALSE;
    }
    else if(((SMS_INDEX*)handle)->byStoreType == STORE_OFFSET)
    {
        switch(nFolderID)
        {   
        case MU_INBOX: // a big file
            pszFileName = SMS_FILENAME_INBOX;
            break;
            
        case MU_SENT:
            pszFileName = SMS_FILENAME_SENT;
            break;
            
        case MU_REPORT:
            pszFileName = SMS_FILENAME_REPORT;
            break;
            
        case MU_MYFOLDER:
            pszFileName = SMS_FILENAME_MYFOLDER;
            break;
            
        default:
            szFileName[0] = 0;
            sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nFolderID,SMS_FILE_POSTFIX);
            pszFileName = szFileName;
        }

        szOldPath[0] = 0;      
        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_SMS); 
        bRet = SMS_ReadDataFromOffset(pszFileName,((SMS_INDEX*)handle)->dwOffest,&smsstore,FALSE);
        chdir(szOldPath);
        if( !bRet )
            return FALSE;
    }
    else
    {
        SMS_INITDATA *pData;
        int i;
        SMS_INFO* pMESMSInfo = NULL;
        
        pData = SMS_GetInitData();
        
        for(i = 0 ; i < pData->nMEMaxCount ; i++)
        {
            if(pData->pMESMSInfo[i].Index == ((SMS_INDEX*)handle)->index)
            {
                pMESMSInfo = &(pData->pMESMSInfo[i]);
                
                break;
            }
        }

        if(pMESMSInfo == NULL)
            return FALSE;

        if(FALSE == SMS_ChangeStruct(&smsstore,pMESMSInfo))
            return FALSE;
    }

    bRet = SMS_CreateEditWnd(MuGetFrame(),hwndmu,NULL,smsstore.pszContent,smsstore.fix.Conlen,-1,-1,MU_MSG_SMS,NULL,TRUE);

    SMS_FREE(smsstore.pszContent);
    SMS_FREE(smsstore.pszUDH);
    SMS_FREE(smsstore.pszPhone);

    return bRet;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_reply_from(HWND hwndmu, DWORD handle)
{
    BOOL bRet;
    SMS_STORE smsstore;
    char* pszFileName;
    char  szFileName[SMS_FILENAME_MAXLEN];

    memset(&smsstore,0,sizeof(SMS_STORE));

    if(((SMS_INDEX*)handle)->byStoreType == STORE_FILE)
    { 
        szOldPath[0] = 0;      
        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_SMS); 
        bRet = SMS_ReadDataFromFile(((SMS_INDEX*)handle)->szFileName,&smsstore,FALSE);
        chdir(szOldPath);
        if( !bRet )
            return FALSE;
    }
    else if(((SMS_INDEX*)handle)->byStoreType == STORE_OFFSET)
    {
        switch(nFolderID)
        {   
        case MU_INBOX: // a big file
            pszFileName = SMS_FILENAME_INBOX;
            break;
            
        case MU_SENT:
            pszFileName = SMS_FILENAME_SENT;
            break;
            
        case MU_REPORT:
            pszFileName = SMS_FILENAME_REPORT;
            break;
            
        case MU_MYFOLDER:
            pszFileName = SMS_FILENAME_MYFOLDER;
            break;
            
        default:
            szFileName[0] = 0;
            sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nFolderID,SMS_FILE_POSTFIX);
            pszFileName = szFileName;
        }

        szOldPath[0] = 0;      
        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_SMS); 
        bRet = SMS_ReadDataFromOffset(pszFileName,((SMS_INDEX*)handle)->dwOffest,&smsstore,FALSE);
        chdir(szOldPath);
        if( !bRet )
            return FALSE;
    }
    else
    {
        SMS_INITDATA *pData;
        int i;
        SMS_INFO* pMESMSInfo = NULL;
        
        pData = SMS_GetInitData();
        
        for(i = 0 ; i < pData->nMEMaxCount ; i++)
        {
            if(pData->pMESMSInfo[i].Index == ((SMS_INDEX*)handle)->index)
            {
                pMESMSInfo = &(pData->pMESMSInfo[i]);
                
                break;
            }
        }

        if(pMESMSInfo == NULL)
            return FALSE;

        if(FALSE == SMS_ChangeStruct(&smsstore,pMESMSInfo))
            return FALSE;
    }

    bRet = SMS_CreateEditWnd(MuGetFrame(),hwndmu,smsstore.pszPhone,NULL,0,-1,-1,MU_MSG_SMS,NULL,FALSE);

    SMS_FREE(smsstore.pszContent);
    SMS_FREE(smsstore.pszUDH);
    SMS_FREE(smsstore.pszPhone);

    return bRet;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_reply_all(HWND hwndmu, DWORD handle)
{
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_get_number(HWND hwndmu, DWORD handle)
{
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_resend(HWND hwndmu, DWORD handle)
{
    SMS_INDEX* p;

    p = (SMS_INDEX*)handle;

    if ( GetSIMState() != 1 )
    {
        if(p->byStoreType == STORE_FILE)
        {   
            SMS_STOREFIX   storetmp;
            char        szOldPath[PATH_MAXLEN];
            
            PLXTipsWin(NULL,NULL,0,IDS_SENDFAILEDNOSIM,IDS_SMS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
            
            memset(&storetmp,0,sizeof(SMS_STOREFIX));
            
            szOldPath[0] = 0;   
            
            getcwd(szOldPath,PATH_MAXLEN);  
            
            chdir(PATH_DIR_SMS);  
            
            if(SMS_ReadStatusFromFile(p->szFileName,&storetmp))
            {            
                if(storetmp.Stat == MU_STU_SUSPENDED || storetmp.Stat == MU_STU_UNSENT)
                {
                    storetmp.Stat = MU_STU_DEFERMENT;
                    
                    SMS_ModifyFileStatus(p->szFileName,&storetmp);
                    
                    if(MU_GetCurFolderType() == MU_OUTBOX)
                    {
                        smsapi_ModifyByFileName(p->szFileName,MU_SEND_FAILURE);
                    }
                }
            }
            
            chdir(szOldPath); 
        }
                 
        return FALSE;
    }

    if(p->byStoreType == STORE_FILE)
    {
        return SMS_Send(p->szFileName,handle);
    }

    return FALSE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_defer2send_message(HWND hwndmu, DWORD handle)
{
    SMS_INDEX* p;

    p = (SMS_INDEX*)handle;

    if(p->byStoreType == STORE_FILE)
    {
        return SMS_Suspend(p->szFileName);
    }

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_move_message(HWND hwndmu, DWORD handle , int nFolder)
{
    int nCurFolder;
    char* pszOldFileName,*pszNewFileName;
    DWORD dwOffset;
    SMS_STORE smsstore;
    char szOldFileName[SMS_FILENAME_MAXLEN],szNewFileName[SMS_FILENAME_MAXLEN];

    nCurFolder = MU_GetCurFolderType();

    if(nCurFolder == -1)
        return FALSE;

    if(nCurFolder == nFolder)
        return FALSE;

    if(((SMS_INDEX*)handle)->byStoreType == STORE_INDEX)
    {
        MESTORE_INFO MEStoreInfo;
        SMS_INITDATA *pData;
        int Stat = MU_STU_READ;
        int i;
        
        pData = SMS_GetInitData();
        
        memset(&MEStoreInfo,0,sizeof(MESTORE_INFO));

        SMS_ReadOneMEStore(&MEStoreInfo,((SMS_INDEX*)handle)->index);

        MEStoreInfo.nfolderid = nFolder;
        
        SMS_WriteOneMEStore(&MEStoreInfo,((SMS_INDEX*)handle)->index);

        SMS_Delete((SMS_INDEX*)handle);
        //smsapi_Delete(handle);
        
        SMS_ChangeCount(nFolder,SMS_COUNT_ALL,1);
        
        for(i = 0 ; i < pData->nMEMaxCount ; i++)
        {
            if(pData->pMESMSInfo[i].Index == ((SMS_INDEX*)handle)->index)
            {
                Stat = pData->pMESMSInfo[i].Stat;
                break;
            }
        }

        if(Stat == SMS_UNREAD)
            SMS_ChangeCount(nFolder,SMS_COUNT_UNREAD,1);
        
        SMS_ChangeCount(nCurFolder,SMS_COUNT_ALL,-1);
        
        if(Stat == SMS_UNREAD)
            SMS_ChangeCount(nCurFolder,SMS_COUNT_UNREAD,-1);
        
        return TRUE;
    }

    memset(&smsstore,0,sizeof(SMS_STORE));
    
    switch(nCurFolder)
    {   
    case MU_REPORT:
    case MU_DRAFT:
        return FALSE;

    case MU_INBOX: // a big file
        pszOldFileName = SMS_FILENAME_INBOX;
        break;
        
    case MU_SENT:
        pszOldFileName = SMS_FILENAME_SENT;
        break;
        
    case MU_MYFOLDER:
        pszOldFileName = SMS_FILENAME_MYFOLDER;
        break;
        
    case MU_OUTBOX:
        if(nFolder != MU_DRAFT)
            return FALSE;//small to small

        szOldPath[0] = 0;
        
        getcwd(szOldPath,PATH_MAXLEN);  
        
        chdir(PATH_DIR_SMS);  
        
        if(SMS_ReadDataFromFile(((SMS_INDEX*)handle)->szFileName,&smsstore,FALSE) == FALSE)
        {
            chdir(szOldPath);

            SMS_FREE(smsstore.pszContent);
            SMS_FREE(smsstore.pszUDH);
            SMS_FREE(smsstore.pszPhone);
            
            return FALSE;
        }

        smsstore.fix.Stat = MU_STU_DRAFT;

        if(SMS_SaveFile(MU_DRAFT,&smsstore,szNewFileName,SMS_IsVcradOrVcal(&smsstore)) == FALSE)
        {
            chdir(szOldPath);
            
            SMS_FREE(smsstore.pszContent);
            SMS_FREE(smsstore.pszUDH);
            SMS_FREE(smsstore.pszPhone);
            
            return FALSE;
        }

        SMS_ChangeCount(MU_DRAFT,SMS_COUNT_ALL,1);
                
        remove(((SMS_INDEX*)handle)->szFileName);

        SMS_ChangeCount(MU_OUTBOX,SMS_COUNT_ALL,-1);
        
        SMS_NotifyIdle();

        chdir(szOldPath); 

        SMS_Delete((SMS_INDEX*)handle);
        //smsapi_Delete(handle);
        
        SMS_FREE(smsstore.pszContent);
        SMS_FREE(smsstore.pszUDH);
        SMS_FREE(smsstore.pszPhone);
        
        return TRUE;
        
    default:
        szOldFileName[0] = 0;
        sprintf(szOldFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nCurFolder,SMS_FILE_POSTFIX);
        pszOldFileName = szOldFileName;
        break;
    }


    
    switch(nFolder)
    {   
    case MU_INBOX: // a big file
        pszNewFileName = SMS_FILENAME_INBOX;
        break;
                
    case MU_MYFOLDER:
        pszNewFileName = SMS_FILENAME_MYFOLDER;
        break;

    case MU_DRAFT:
    case MU_OUTBOX:
    case MU_SENT:
    case MU_REPORT:
        return FALSE;
        
    default:
        szNewFileName[0] = 0;
        sprintf(szNewFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nFolder,SMS_FILE_POSTFIX);
        pszNewFileName = szNewFileName;
        break;
    }
    
    szOldPath[0] = 0;      
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS); 
    
    if(SMS_ReadDataFromOffset(pszOldFileName,((SMS_INDEX*)handle)->dwOffest,&smsstore,TRUE) == FALSE)
    {
        chdir(szOldPath);

        SMS_FREE(smsstore.pszContent);
        SMS_FREE(smsstore.pszUDH);
        SMS_FREE(smsstore.pszPhone);
        
        return FALSE;
    }
    
    if(SMS_SaveRecord(pszNewFileName,&smsstore,&dwOffset) == FALSE)
    {
        chdir(szOldPath);
        
        SMS_FREE(smsstore.pszContent);
        SMS_FREE(smsstore.pszUDH);
        SMS_FREE(smsstore.pszPhone);
        
        return FALSE;
    }

    SMS_ChangeCount(nFolder,SMS_COUNT_ALL,1);

    if(smsstore.fix.Stat == MU_STU_UNREAD)
        SMS_ChangeCount(nFolder,SMS_COUNT_UNREAD,1);

    if(-1 == DeleteWithOffset(pszOldFileName,((SMS_INDEX*)handle)->dwOffest))
    {
        chdir(szOldPath);

        SMS_FREE(smsstore.pszContent);
        SMS_FREE(smsstore.pszUDH);
        SMS_FREE(smsstore.pszPhone);
        
        return FALSE;
    }

    SMS_ChangeCount(nCurFolder,SMS_COUNT_ALL,-1);

    if(smsstore.fix.Stat == MU_STU_UNREAD)
        SMS_ChangeCount(nCurFolder,SMS_COUNT_UNREAD,-1);

    SMS_ChangeCount(nCurFolder,SMS_COUNT_DEL,1);

    chdir(szOldPath);
    
    SMS_Delete((SMS_INDEX*)handle);
    //smsapi_Delete(handle);

    SMS_FREE(smsstore.pszContent);
    SMS_FREE(smsstore.pszUDH);
    SMS_FREE(smsstore.pszPhone);


    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_destroy(void)
{
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_sim_create(HWND hwndmu)
{
    BOOL bRet;

    bRet = SMS_CreateSIMWnd(hwndmu);
    
    return bRet;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_voicemail_call(HWND hwndmu)
{
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_cb_create(HWND hwndmu)
{
    CB_CreateWnd(hwndmu);

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void smsapi_ModifyByFileName(char* pszFileName,int nType)
{
    if(MU_GetCurFolderType() == MU_OUTBOX)
    {
        SMS_INDEX* handle;
        MU_Daemon_Notify  muDaemon;
        
        handle = SMS_FindByFileName(pszFileName);

        if(handle == NULL)
            return;

        muDaemon.handle = (DWORD)handle;
        muDaemon.type = nType;

        SendMessage(smsapi_GetMuHwnd(),PWM_MSG_MU_DAEMON,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)&muDaemon);     
    }
    return;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void smsapi_Modify(char* pszFileName,DWORD handle,int folderid,SMS_STORE *psmsstore)
{
    PSMS_INDEX pindex;
    MU_MsgNode msgnode;
	int nLen;

    if(folderid != MU_DRAFT && folderid != MU_OUTBOX)
        return;
    
    pindex = (PSMS_INDEX)handle;

    if(MU_GetCurFolderType() == MU_DRAFT || MU_GetCurFolderType() == MU_OUTBOX)
    {
		nLen = strlen(pszFileName);
		nLen = nLen > SMS_FILENAME_MAXLEN ? SMS_FILENAME_MAXLEN : nLen;
        //rename file name in index
        strncpy(pindex->szFileName,pszFileName,nLen);
        pindex->szFileName[SMS_FILENAME_MAXLEN-1] = 0;

        //fill the modify node
        memset(&msgnode,0,sizeof(MU_MsgNode));
        
        if( psmsstore->fix.Phonelen > 0)
        {        
			nLen = psmsstore->fix.Phonelen > MU_ADDR_MAX_LEN ? MU_ADDR_MAX_LEN : psmsstore->fix.Phonelen;
            
			strncpy(msgnode.addr,psmsstore->pszPhone,nLen);
            msgnode.addr[MU_ADDR_MAX_LEN] = 0;    
        }
        else
            msgnode.addr[0] = 0;
        
        if( psmsstore->fix.Conlen > 0)
        {
			
			nLen = psmsstore->fix.Conlen > MU_SUBJECT_DIS_LEN ? MU_SUBJECT_DIS_LEN : psmsstore->fix.Conlen;

            strncpy(msgnode.subject,psmsstore->pszContent,nLen);
            msgnode.subject[MU_SUBJECT_DIS_LEN] = 0;   
        }
        else
            msgnode.subject[0] = 0;
        
        msgnode.storage_type = MU_STORE_IN_FLASH;
        msgnode.msgtype = MU_MSG_SMS;
        msgnode.status = psmsstore->fix.Stat; // attention
        msgnode.maskdate = psmsstore->fix.dwDateTime;
        msgnode.handle = handle;

        //notify unibox
        SendMessage(smsapi_GetMuHwnd(),PWM_MSG_MU_MODIFIED,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)&msgnode);
    }    
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD smsapi_New(char* szFileName,SMS_STORE *psmsstore,DWORD* phandle,int* pfolderid)
{
    SMS_INDEX*  pChainNode;
    MU_MsgNode  msgnode;
	int nLen;
    
    switch(MU_GetCurFolderType())
    {
    case MU_DRAFT:
        *pfolderid = MU_DRAFT;
    case MU_OUTBOX:
        pChainNode = SMS_New();
        
        if(pChainNode == NULL)
            return NULL;
        
        if(pfolderid && *pfolderid == MU_DRAFT)
            *phandle = (DWORD)pChainNode;
        
        pChainNode->byStoreType = STORE_FILE;
		nLen = strlen(szFileName);
		nLen = nLen > SMS_FILENAME_MAXLEN ? SMS_FILENAME_MAXLEN : nLen;
        strncpy(pChainNode->szFileName,szFileName,nLen);
        pChainNode->szFileName[SMS_FILENAME_MAXLEN-1] = 0;
        SMS_Insert(pChainNode);

        //fill the modify node
        memset(&msgnode,0,sizeof(MU_MsgNode));
        
        if( psmsstore->fix.Phonelen > 0)
        {        
			nLen = psmsstore->fix.Phonelen > MU_ADDR_MAX_LEN ? MU_ADDR_MAX_LEN : psmsstore->fix.Phonelen;
            strncpy(msgnode.addr,psmsstore->pszPhone,nLen);
            msgnode.addr[MU_ADDR_MAX_LEN] = 0;    
        }
        else
            msgnode.addr[0] = 0;
        
        if( psmsstore->fix.Conlen > 0)
        {
			nLen = psmsstore->fix.Conlen > MU_SUBJECT_DIS_LEN ? MU_SUBJECT_DIS_LEN : psmsstore->fix.Conlen;
            strncpy(msgnode.subject,psmsstore->pszContent,nLen);
            msgnode.subject[MU_SUBJECT_DIS_LEN] = 0;   
        }
        else
            msgnode.subject[0] = 0;
        
        msgnode.storage_type = MU_STORE_IN_FLASH;
        msgnode.msgtype = MU_MSG_SMS;
        msgnode.status = psmsstore->fix.Stat; // attention
        msgnode.maskdate = psmsstore->fix.dwDateTime;
        msgnode.handle = (DWORD)pChainNode;

        //notify unibox
        SendMessage(smsapi_GetMuHwnd(),PWM_MSG_MU_NEWMTMSG,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)&msgnode);
        
        return ((DWORD)pChainNode);
        
        break;

    default:

        return NULL;

        break;
    }

    return NULL;

}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD smsapi_NewRecord(DWORD dwOffset,SMS_STORE *psmsstore)
{
    SMS_INDEX*  pChainNode;
    MU_MsgNode  msgnode;
	int nLen;
    int nType;

    switch(MU_GetCurFolderType())
    {
    case MU_INBOX:
    case MU_SENT:
        
        pChainNode = SMS_New();
        
        if(pChainNode == NULL)
            return 0;
        
        pChainNode->byStoreType = STORE_OFFSET;
        pChainNode->dwOffest = dwOffset;

        SMS_Insert(pChainNode);
        
        //fill the modify node
        memset(&msgnode,0,sizeof(MU_MsgNode));
        
        nType = SMS_IsVcradOrVcal(psmsstore);

        if( psmsstore->fix.Phonelen > 0)
        {        
			nLen = psmsstore->fix.Phonelen > MU_ADDR_MAX_LEN ? MU_ADDR_MAX_LEN : psmsstore->fix.Phonelen;
            strncpy(msgnode.addr,psmsstore->pszPhone,nLen);
            msgnode.addr[MU_ADDR_MAX_LEN] = 0;    
        }
        else
            msgnode.addr[0] = 0;
        
        switch(nType) 
        {
        case MU_MSG_VCARD:
            strcpy(msgnode.subject,IDS_VCARD);
        	break;

        case MU_MSG_VCAL:
            strcpy(msgnode.subject,IDS_VCAL);
            break;

        case MU_MSG_SMS:
        default:
            if( psmsstore->fix.Conlen > 0 )
            {
                nLen = psmsstore->fix.Conlen > MU_SUBJECT_DIS_LEN ? MU_SUBJECT_DIS_LEN : psmsstore->fix.Conlen;
                strncpy(msgnode.subject,psmsstore->pszContent,nLen);
                msgnode.subject[MU_SUBJECT_DIS_LEN] = 0;   
            }
            else
                msgnode.subject[0] = 0;
        	break;
        }
        
        msgnode.storage_type = MU_STORE_IN_FLASH;
        msgnode.msgtype = nType;
        msgnode.status = psmsstore->fix.Stat; // attention
        msgnode.maskdate = psmsstore->fix.dwDateTime;
        msgnode.handle = (DWORD)pChainNode;
        
        //notify unibox
        SendMessage(smsapi_GetMuHwnd(),PWM_MSG_MU_NEWMTMSG,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)&msgnode);
        
        break;

        
    case MU_REPORT:
        
        pChainNode = SMS_New();
        
        if(pChainNode == NULL)
            return 0;
        
        pChainNode->byStoreType = STORE_OFFSET;
        pChainNode->dwOffest = dwOffset;

        SMS_Insert(pChainNode);
        
        //fill the modify node
        memset(&msgnode,0,sizeof(MU_MsgNode));
        
        if( psmsstore->fix.Phonelen > 0)
        {        
			nLen = psmsstore->fix.Phonelen > MU_ADDR_MAX_LEN ? MU_ADDR_MAX_LEN : psmsstore->fix.Phonelen;
            strncpy(msgnode.addr,psmsstore->pszPhone,nLen);
            msgnode.addr[MU_ADDR_MAX_LEN] = 0;    
        }
        else
            msgnode.addr[0] = 0;
        
        if( psmsstore->fix.Conlen > 0)
        {
			nLen = psmsstore->fix.Conlen > MU_SUBJECT_DIS_LEN ? MU_SUBJECT_DIS_LEN : psmsstore->fix.Conlen;
            strncpy(msgnode.subject,psmsstore->pszContent,nLen);
            msgnode.subject[MU_SUBJECT_DIS_LEN] = 0;   
        }
        else
            msgnode.subject[0] = 0;
        
        msgnode.storage_type = MU_STORE_IN_FLASH;
        msgnode.msgtype = MU_MSG_STATUS_REPORT;
        if(psmsstore->fix.Status == 0)
            msgnode.status = MU_STU_CONFIRM; // attention
        else if(psmsstore->fix.Status == 1)
            msgnode.status = MU_STU_FAILED;
        else if(psmsstore->fix.Status == -1)
            msgnode.status = MU_STU_WAITING;
        msgnode.maskdate = psmsstore->fix.dwDateTime;
        msgnode.handle = (DWORD)pChainNode;
        
        //notify unibox
        SendMessage(smsapi_GetMuHwnd(),PWM_MSG_MU_NEWMTMSG,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)&msgnode);
        break;

    default:
        break;
    }

    return((DWORD)pChainNode);     
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void smsapi_NewME(int index,SMS_INFO *psmsinfo)
{
    SMS_INDEX*  pChainNode;
    MU_MsgNode  msgnode;
	int nLen;
//     int nType;

    switch(MU_GetCurFolderType())
    {
    case MU_INBOX:
    case MU_SENT:
        
        pChainNode = SMS_New();
        
        if(pChainNode == NULL)
            return;
        
        pChainNode->byStoreType = STORE_INDEX;
        pChainNode->dwOffest = index;

        SMS_Insert(pChainNode);
        
        //fill the modify node
        memset(&msgnode,0,sizeof(MU_MsgNode));
        
//        if(SMS_IsVcradOrVcal(psmsstore))
//            nType = MU_MSG_VCARD;
//        else
//            nType = MU_MSG_SMS;

        if( strlen(psmsinfo->SenderNum) )
        {        
			nLen = strlen(psmsinfo->SenderNum) > MU_ADDR_MAX_LEN ? MU_ADDR_MAX_LEN : strlen(psmsinfo->SenderNum);
            strncpy(msgnode.addr,psmsinfo->SenderNum,nLen);
            msgnode.addr[MU_ADDR_MAX_LEN] = 0;    
        }
        else
            msgnode.addr[0] = 0;
        
//         switch(nType) 
        {
//        case MU_MSG_VCARD:
//            strcpy(msgnode.subject,IDS_VCARD);
//        	break;
//
//        case MU_MSG_VCAL:
//            strcpy(msgnode.subject,IDS_VCAL);
//            break;
//
//        case MU_MSG_SMS:
//        default:
            if( psmsinfo->ConLen > 0 )
            {
                char *pszContent = NULL;
                int nLen = 0;

                SMS_ParseContentEx(psmsinfo->dcs, psmsinfo->Context , psmsinfo->ConLen ,&pszContent,&nLen);
                nLen = nLen > MU_SUBJECT_DIS_LEN ? MU_SUBJECT_DIS_LEN : nLen;
                strncpy(msgnode.subject,pszContent,nLen);
                msgnode.subject[MU_SUBJECT_DIS_LEN] = 0;   
                SMS_FREE(pszContent);
            }
            else
                msgnode.subject[0] = 0;
//         	break;
        }
        
        msgnode.storage_type = MU_STORE_IN_FLASH;
        msgnode.msgtype = MU_MSG_SMS;//nType;
        if(psmsinfo->Stat == SMS_UNREAD)
            msgnode.status = MU_STU_UNREAD;
        else
            msgnode.status = MU_STU_READ;
        msgnode.maskdate = String2DWORD(psmsinfo->SendTime);
        msgnode.handle = (DWORD)pChainNode;
        
        //notify unibox
        SendMessage(smsapi_GetMuHwnd(),PWM_MSG_MU_NEWMTMSG,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)&msgnode);
        
        break;

        
    case MU_REPORT:
        
        pChainNode = SMS_New();
        
        if(pChainNode == NULL)
            return;
        
        pChainNode->byStoreType = STORE_INDEX;
        pChainNode->dwOffest = index;

        SMS_Insert(pChainNode);
        
        //fill the modify node
        memset(&msgnode,0,sizeof(MU_MsgNode));
        
        if( strlen(psmsinfo->SenderNum) )
        {        
			nLen = strlen(psmsinfo->SenderNum) > MU_ADDR_MAX_LEN ? MU_ADDR_MAX_LEN : strlen(psmsinfo->SenderNum);
            strncpy(msgnode.addr,psmsinfo->SenderNum,nLen);
            msgnode.addr[MU_ADDR_MAX_LEN] = 0;    
        }
        else
            msgnode.addr[0] = 0;
        
        if( psmsinfo->ConLen > 0 )
        {
			nLen = psmsinfo->ConLen > MU_SUBJECT_DIS_LEN ? MU_SUBJECT_DIS_LEN : psmsinfo->ConLen;
            strncpy(msgnode.subject,psmsinfo->Context,nLen);
            msgnode.subject[MU_SUBJECT_DIS_LEN] = 0;   
        }
        else
            msgnode.subject[0] = 0;
        
        msgnode.storage_type = MU_STORE_IN_FLASH;
        msgnode.msgtype = MU_MSG_STATUS_REPORT;
        
        if(psmsinfo->Status == 0)
            msgnode.status = MU_STU_CONFIRM; // attention
        else if(psmsinfo->Status == 1)
            msgnode.status = MU_STU_FAILED;
        else if(psmsinfo->Status == -1)
            msgnode.status = MU_STU_WAITING;
        
        msgnode.maskdate = String2DWORD(psmsinfo->SendTime);
        msgnode.handle = (DWORD)pChainNode;
        
        //notify unibox
        SendMessage(smsapi_GetMuHwnd(),PWM_MSG_MU_NEWMTMSG,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)&msgnode);
        break;

    default:
        break;
    }
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void smsapi_Delete(DWORD handle)
{            
    SMS_Delete((SMS_INDEX*)handle);
    SendMessage(smsapi_GetMuHwnd(),PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),handle);
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void smsapi_DeleteByFileName(char* szFileName)
{
    SMS_INDEX* handle;

    switch(MU_GetCurFolderType())
    {
    case MU_OUTBOX:
        handle = SMS_FindByFileName(szFileName);
        if(handle != NULL)
            smsapi_Delete((DWORD)handle);
    	break;

    default:
        break;
    }
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_get_count(int nfolder, int *pnunread, int *pncount)
{
    if(nfolder == -1)
    {
        return(SMS_GetSumCount(pnunread,pncount));        
    }
    else
    {        
        int nType;
        PVOID *pTemp;
        
        pTemp = SMS_FindCountAddr(nfolder,&nType);
        
        if(pTemp == NULL)
            return FALSE;

        if(nType == COUNT_ARRAY)
        {
            *pnunread = ((SMS_COUNT*)pTemp)->nunread;
            *pncount = ((SMS_COUNT*)pTemp)->ncount;
        }
        else
        {
            *pnunread = ((SMS_COUNTNODE*)pTemp)->smscount.nunread;
            *pncount = ((SMS_COUNTNODE*)pTemp)->smscount.ncount;
        }

        return TRUE;
    }
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_GetSumCount(int *pnunread, int *pncount)
{
    int i;
    SMS_COUNTNODE *pTemp;
    
    *pnunread = 0;
    *pncount = 0;

    for( i = 0 ; i < nCountArrayNum ; i++ )
    {
        if(pCountArray[i].nfolderid == -1)
            continue;
        *pncount += pCountArray[i].ncount;
        *pnunread += pCountArray[i].nunread;
    }

    pTemp = pCountChain;
    
    while( pTemp )
    {
        *pncount += pTemp->smscount.ncount;
        *pnunread += pTemp->smscount.nunread;

        pTemp = pTemp->pNext;
    }

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_GetRecordCount(int nfolder, int *pnunread, int *pncount, int *pndelete)
{
    int f;  
    struct stat buf;
    int nfilesize;
    DWORD  dwoffset;
    BYTE   bydel;
    DWORD  dwlen;
    SMS_STOREFIX storefix;
    struct dirent *pData;
    static DIR* pDir;
    char   szTemp;
    char   FolderMask[10];
    char szFileName[SMS_FILENAME_MAXLEN];
    char* pszFileName;
    int i;

    *pnunread = 0;
    *pncount = 0;
    *pndelete = 0;
    
    if(SMS_IsInitOK() == TRUE)
    {
        MESTORE_INFO* pMEStoreInfo = NULL;
        SMS_INITDATA *pSIMData = NULL;

        pSIMData = SMS_GetInitData();
        
        pMEStoreInfo = (MESTORE_INFO*)malloc(sizeof(MESTORE_INFO)*(pSIMData->nMEMaxCount));
        
        if(pMEStoreInfo == NULL)
        {    
            return FALSE;
        }
        
        memset(pMEStoreInfo,0,sizeof(MESTORE_INFO)*(pSIMData->nMEMaxCount));
        
        SMS_ReadMEStoreInfo(pMEStoreInfo,pSIMData->nMEMaxCount);
        
        for(i = 0 ; i < pSIMData->nMEMaxCount ; i++)
        {
            if(pSIMData->pMESMSInfo[i].Index == 0)
                continue;
            
            if(pMEStoreInfo[pSIMData->pMESMSInfo[i].Index - 1].nfolderid == nfolder)
            {
                (*pncount)++;
                
                if(pSIMData->pMESMSInfo[i].Stat == SMS_UNREAD)
                    (*pnunread)++;
            }
        }
        
        SMS_FREE(pMEStoreInfo);
    }

    switch(nfolder)
    {
    case MU_OUTBOX: // more small temporary file
    case MU_DRAFT:
        
        sprintf(FolderMask,"%d",nfolder);

        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_SMS);
         
        pDir = opendir(PATH_DIR_SMS);
        if(pDir != NULL)
        {
            while( (pData = readdir(pDir)) != NULL )
            {
                szTemp = pData->d_name[0];
                if(szTemp == FolderMask[0])
                {                                                
                    f = open(pData->d_name,O_RDONLY);

                    if(f == -1)
                    {
                        closedir(pDir);
                        pDir = NULL;
                        chdir(szOldPath);
                        return FALSE;
                    }
                    
                    (*pncount)++;

                    read(f,&storefix,sizeof(SMS_STOREFIX));

                    if(storefix.Stat == MU_STU_UNREAD)
                        (*pnunread)++;

                    close(f);
                }
            }
            closedir(pDir);
            chdir(szOldPath);
            return TRUE;
        }
        chdir(szOldPath);
        return FALSE;
        
    case MU_INBOX: // a big file
        pszFileName = SMS_FILENAME_INBOX;
        break;
        
    case MU_SENT:
        pszFileName = SMS_FILENAME_SENT;
        break;
        
    case MU_REPORT:
        pszFileName = SMS_FILENAME_REPORT;
        break;
        
    case MU_MYFOLDER:
        pszFileName = SMS_FILENAME_MYFOLDER;
        break;
        
    default:
        szFileName[0] = 0;
        sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nfolder,SMS_FILE_POSTFIX);
        if(strlen(szFileName) > (SMS_FILENAME_MAXLEN-1))   
        {
            return FALSE;
        }
        pszFileName = szFileName;
        break;
    }
    
    
    szOldPath[0] = 0;
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  

    memset(&buf,0,sizeof(struct stat));
    stat(pszFileName,(struct stat*)&buf);

    nfilesize = buf.st_size;

    f = open(pszFileName,O_RDONLY);
    if(f == -1)
    {
        chdir(szOldPath);
        return TRUE;
    }
 
    dwoffset = 0;
    
    while(nfilesize > 0)
    {        
        lseek(f,dwoffset,SEEK_SET);

        read(f,&bydel,sizeof(BYTE));

        read(f,&dwlen,sizeof(DWORD));

        if(bydel == SMS_USED)
        {
            (*pncount)++;   
            
            memset(&storefix,0,sizeof(SMS_STOREFIX));
            
            read(f,&storefix,sizeof(SMS_STOREFIX));
            
            if(storefix.Stat == MU_STU_UNREAD)
                (*pnunread)++;
        }
        else
            (*pndelete)++;
        
        dwoffset += sizeof(BYTE) + sizeof(DWORD) + dwlen;
        nfilesize -= sizeof(BYTE) + sizeof(DWORD) + dwlen;
    }
    
    close(f);
    chdir(szOldPath);
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_CounterInit(void)
{
    int nfoldernum = 0;
    BOOL bSucc;
    PMU_FOLDERINFO pFolderInfo = NULL;
    int i,n;

    if(MU_GetFolderInfo(NULL,&nfoldernum))
    {
        if(nfoldernum != 0)
        {
            pFolderInfo = (PMU_FOLDERINFO)malloc(nfoldernum*sizeof(MU_FOLDERINFO));
            
            if(pFolderInfo != NULL)
            {
                if(MU_GetFolderInfo(pFolderInfo,&nfoldernum) == FALSE)
                {
                    SMS_FREE(pFolderInfo);
                    return FALSE;
                }
            }
            else
                return FALSE;
        }
    }

    pCountArray = (SMS_COUNT*)malloc((nfoldernum+MU_REPORT+1)*sizeof(SMS_COUNT));

    nCountArrayNum = nfoldernum+MU_REPORT+1;

    memset(pCountArray,0,nCountArrayNum*sizeof(SMS_COUNT));

    bSucc = TRUE;
    for(i=0 ; i <= MU_REPORT ; i++)
    {
        pCountArray[i].nfolderid = i;
        bSucc = SMS_GetRecordCount(i,&(pCountArray[i].nunread),&(pCountArray[i].ncount),&(pCountArray[i].ndelete));

        if( bSucc == FALSE )
        {
            SMS_FREE(pFolderInfo);
            return FALSE;
        }
    }
    
    for(i=MU_REPORT+1,n=0 ; i <= nfoldernum+MU_REPORT ; i++,n++)
    {
        pCountArray[i].nfolderid = pFolderInfo[n].nFolderID;
        bSucc = SMS_GetRecordCount(pFolderInfo[n].nFolderID,&(pCountArray[i].nunread),
            &(pCountArray[i].ncount),&(pCountArray[i].ndelete));
        
        if( bSucc == FALSE )
        {
            SMS_FREE(pFolderInfo);
            return FALSE;
        }
    }

    SMS_FREE(pFolderInfo);
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static PVOID SMS_FindCountAddr(int nFolderID,int* pnType)
{
    int i;
    SMS_COUNTNODE *pTemp;

    for(i = 0 ; i < nCountArrayNum ; i++)
    {
        if(pCountArray[i].nfolderid == nFolderID)
        {
            *pnType = COUNT_ARRAY;
            return (PVOID)&pCountArray[i];
        }
    }
    
    pTemp = pCountChain;
    
    while( pTemp )
    {
        if(pTemp->smscount.nfolderid == nFolderID)
        {
            *pnType = COUNT_CHAIN;
            return (PVOID)pTemp;
        }
        pTemp = pTemp->pNext;
    }

    return NULL;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_DeleteCount(int nFolderID)
{
    int nType;
    void *pTemp;

    pTemp = SMS_FindCountAddr(nFolderID,&nType);
    
    if(pTemp == NULL)
        return FALSE;

    if(nType == COUNT_ARRAY)
    {
        ((SMS_COUNT*)pTemp)->nfolderid = -1;
    }
    else
    {
        Count_Delete((SMS_COUNTNODE*)pTemp);
    }
    return TRUE;
}
/*********************************************************************\
* Function	   Count_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static SMS_COUNTNODE* Count_New(void)
{
    SMS_COUNTNODE* p = NULL;

    p = (SMS_COUNTNODE*)malloc(sizeof(SMS_COUNTNODE));
    memset(p,0,sizeof(SMS_COUNTNODE));

    return p;
}
/*********************************************************************\
* Function	   Count_Fill
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL Count_Fill(SMS_COUNTNODE* pChainNode,int nFolderID)
{
    if(!pChainNode)
        return FALSE;

    pChainNode->smscount.nfolderid = nFolderID;

    return TRUE;
}
/*********************************************************************\
* Function	   Count_Insert
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Count_Insert(SMS_COUNTNODE* pChainNode)
{
    SMS_COUNTNODE* p;

    if(pCountChain)
    {
        p = pCountChain;

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
        pCountChain = pChainNode;
        pChainNode->pNext = NULL;
        pChainNode->pPioneer = NULL;
    }
}
/*********************************************************************\
* Function	   Count_Delete
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Count_Delete(SMS_COUNTNODE* pChainNode)
{
    if( pChainNode == pCountChain )
    {
        if(pChainNode->pNext)
        {
            pCountChain = pChainNode->pNext;
            pChainNode->pNext->pPioneer = NULL;
        }
        else
            pCountChain = NULL;
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
* Function	   Count_Erase
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Count_Erase(void)
{
    SMS_COUNTNODE* p;    
    SMS_COUNTNODE* ptemp;

    p = pCountChain;

    while( p )
    {
        ptemp = p->pNext;       
        free(p);
        p = ptemp;
    }

    pCountChain = NULL;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL sms_new_folder(int nfolderid)
{
    SMS_COUNTNODE* pNewNode;  

    pNewNode = Count_New();
    
    if(!pNewNode)
        return FALSE;
    
    Count_Fill(pNewNode,nfolderid);
    
    Count_Insert(pNewNode); 

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_ChangeCount(int nFolderID,SMS_COUNT_TYPE nType,int nChangeNum)
{   
    void *pTemp;
    int temp;
    
    pTemp = SMS_FindCountAddr(nFolderID,&temp);
    
    if(pTemp == NULL)
        return;

    switch(nType)
    {
    case SMS_COUNT_UNREAD:
        if(temp == COUNT_ARRAY)
        {
            if(nChangeNum == 0)
                ((SMS_COUNT*)pTemp)->nunread = 0;
            else
                ((SMS_COUNT*)pTemp)->nunread += nChangeNum;
#ifdef _SMS_DEBUG_
            printf("\r\n SMS Folder = %d ,unread msg change = %d\r\n",nFolderID,((SMS_COUNT*)pTemp)->nunread);
#endif
            if(((SMS_COUNT*)pTemp)->nunread <= 0)
                ((SMS_COUNT*)pTemp)->nunread = 0;
        }
        else
        {
            if(nChangeNum == 0)
                ((SMS_COUNTNODE*)pTemp)->smscount.nunread = 0;
            else
                ((SMS_COUNTNODE*)pTemp)->smscount.nunread += nChangeNum;
#ifdef _SMS_DEBUG_
            printf("\r\n SMS Folder = %d ,unread msg change = %d\r\n",nFolderID,((SMS_COUNTNODE*)pTemp)->smscount.nunread);
#endif
            
            if(((SMS_COUNTNODE*)pTemp)->smscount.nunread <= 0)
                ((SMS_COUNTNODE*)pTemp)->smscount.nunread = 0;
        }
    	break;

    case SMS_COUNT_ALL:       
        if(temp == COUNT_ARRAY)
        {
            if(nChangeNum == 0)
                ((SMS_COUNT*)pTemp)->ncount = 0;
            else
                ((SMS_COUNT*)pTemp)->ncount += nChangeNum;
#ifdef _SMS_DEBUG_
            printf("\r\n SMS Folder = %d ,count of all msg change = %d\r\n",nFolderID,((SMS_COUNT*)pTemp)->ncount);
#endif
            if(((SMS_COUNT*)pTemp)->ncount <= 0)
                ((SMS_COUNT*)pTemp)->ncount = 0;
        }
        else
        {
            if(nChangeNum == 0)
                ((SMS_COUNTNODE*)pTemp)->smscount.ncount = 0;
            else
                ((SMS_COUNTNODE*)pTemp)->smscount.ncount += nChangeNum;
#ifdef _SMS_DEBUG_
            printf("\r\n SMS Folder = %d ,count of all msg change = %d\r\n",nFolderID,((SMS_COUNTNODE*)pTemp)->smscount.ncount);
#endif
            if(((SMS_COUNTNODE*)pTemp)->smscount.ncount <= 0)
                ((SMS_COUNTNODE*)pTemp)->smscount.ncount = 0;
        }
    	break;

    case SMS_COUNT_DEL:     
        if(temp == COUNT_ARRAY)
        {
            if(nChangeNum == 0)
                ((SMS_COUNT*)pTemp)->ndelete = 0;
            else
                ((SMS_COUNT*)pTemp)->ndelete += nChangeNum;
#ifdef _SMS_DEBUG_
            printf("\r\n SMS Folder = %d ,count of del msg change = %d\r\n",nFolderID,((SMS_COUNT*)pTemp)->ndelete);
#endif
            if(((SMS_COUNT*)pTemp)->ndelete <= 0)
                ((SMS_COUNT*)pTemp)->ndelete = 0;
        }
        else
        {
            if(nChangeNum == 0)
                ((SMS_COUNTNODE*)pTemp)->smscount.ndelete = 0;
            else
                ((SMS_COUNTNODE*)pTemp)->smscount.ndelete += nChangeNum;
#ifdef _SMS_DEBUG_
            printf("\r\n SMS Folder = %d ,count of del msg change = %d\r\n",nFolderID,((SMS_COUNTNODE*)pTemp)->smscount.ndelete);
#endif
            if(((SMS_COUNTNODE*)pTemp)->smscount.ndelete <= 0)
                ((SMS_COUNTNODE*)pTemp)->smscount.ndelete = 0;
        }
        break;

    default:
        break;
    }
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_ResetFile(int folderid)
{
#define RESET_BUFFER_LEN   1024
    
    int nType,ndelete;
    PVOID *pTemp;
    char* pszFileName;
    char szFileName [SMS_FILENAME_MAXLEN];
    int fOld,fNew;
    int nfilesize;
    struct stat buf;
    BYTE byBuffer[RESET_BUFFER_LEN];
    BYTE bydel;
    DWORD dwlen;
    
    pTemp = SMS_FindCountAddr(folderid,&nType);
    
    if(pTemp == NULL)
        return FALSE;
    
    if(nType == COUNT_ARRAY)
    {
        ndelete = ((SMS_COUNT*)pTemp)->ndelete;
    }
    else
    {
        ndelete = ((SMS_COUNTNODE*)pTemp)->smscount.ndelete;
    }

    if(ndelete < SMS_RESET_MAX)
        return FALSE;

    switch(folderid)
    {
    case MU_OUTBOX: // more small temporary file
    case MU_DRAFT:
        return FALSE;
        
    case MU_INBOX: // a big file
        pszFileName = SMS_FILENAME_INBOX;
        break;
        
    case MU_SENT:
        pszFileName = SMS_FILENAME_SENT;
        break;
        
    case MU_REPORT:
        pszFileName = SMS_FILENAME_REPORT;
        break;
        
    case MU_MYFOLDER:
        pszFileName = SMS_FILENAME_MYFOLDER;
        break;
        
    default:
        szFileName[0] = 0;
        sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,folderid,SMS_FILE_POSTFIX);
        if(strlen(szFileName) > (SMS_FILENAME_MAXLEN-1))   
        {
            return FALSE;
        }
        pszFileName = szFileName;
        break;
    }
    
    szOldPath[0] = 0;
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  

    fOld = open(pszFileName,O_RDONLY);
    if(fOld == -1)
    {
        SMS_ChangeCount(folderid,SMS_COUNT_DEL,0);
        chdir(szOldPath);
        return TRUE;
    }

    memset(&buf,0,sizeof(struct stat));
    stat(pszFileName,(struct stat*)&buf);

    nfilesize = buf.st_size;

    remove(SMS_FIMENAME_TEMP);

    fNew = open(SMS_FIMENAME_TEMP,O_RDWR|O_CREAT,S_IRWXU);
    if(fNew == -1)
    {
        close(fOld);
        chdir(szOldPath);
        return FALSE;
    }
     
    while(nfilesize > 0)
    {      
#ifndef _EMULATE_
        KickDog();
#endif
        read(fOld,&bydel,sizeof(BYTE));

        read(fOld,&dwlen,sizeof(DWORD));

        if(bydel == SMS_USED)
        {
            write(fNew,&bydel,sizeof(BYTE));
        
            write(fNew,&dwlen,sizeof(DWORD));

            if(dwlen > RESET_BUFFER_LEN)
            {
                DWORD readlen = 0;
                DWORD datalen = dwlen;
                
                do
                {
                    readlen = datalen > RESET_BUFFER_LEN ? RESET_BUFFER_LEN : datalen;

                    read(fOld,&byBuffer,readlen);
                    write(fNew,&byBuffer,readlen);
                }
                while((datalen -= readlen) > 0);
            }
            else
            {
                read(fOld,&byBuffer,dwlen);
                write(fNew,&byBuffer,dwlen);
            }
        }
        else
            lseek(fOld,dwlen,SEEK_CUR);
            
        nfilesize -= sizeof(BYTE) + sizeof(DWORD) + dwlen;
    }
    
    close(fOld);
    close(fNew);

    remove(pszFileName);
    rename(SMS_FIMENAME_TEMP,pszFileName);
    chdir(szOldPath);
    SMS_ChangeCount(folderid,SMS_COUNT_DEL,0);
    return TRUE;
   
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_ResetALLFile(void)
{
    PMU_FOLDERINFO pFolderInfo = NULL;
    int nfoldernum,i;

    SMS_ResetFile(MU_INBOX);
    SMS_ResetFile(MU_SENT);
    SMS_ResetFile(MU_MYFOLDER);
    SMS_ResetFile(MU_REPORT);
    
    if(MU_GetFolderInfo(NULL,&nfoldernum))
    {
        if(nfoldernum != 0)
        {
            pFolderInfo = (PMU_FOLDERINFO)malloc(nfoldernum*sizeof(MU_FOLDERINFO));
            
            if(pFolderInfo != NULL)
            {
                if(MU_GetFolderInfo(pFolderInfo,&nfoldernum) == FALSE)
                {
                    SMS_FREE(pFolderInfo);
                    return FALSE;
                }
            }
            else
                return FALSE;
        }
    }

    for(i = 0 ; i < nfoldernum ; i++)
        SMS_ResetFile(pFolderInfo[i].nFolderID);
    SMS_FREE(pFolderInfo);

    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_NotifyIdle(void)
{
    int nUnread = 0,nAll = 0,nUnSend = 0;
    
    sms_get_count(MU_INBOX,&nUnread,&nAll);
	
	sms_get_count(MU_OUTBOX,&nAll,&nUnSend);

	MsgNotify(MU_SMS_NOTIFY,SMS_IsFull(),nUnread,nUnSend);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL APP_EditSMS(HWND hParent,const char* PSZRECIPIENT,const char* PSZCONTENT)
{   
    int nConLen;

    if( SMS_Opening() == TRUE )
    { 
        if(PSZCONTENT)
            nConLen = strlen(PSZCONTENT);
        else
            nConLen = 0;
        
        return(SMS_CreateEditWnd(hParent,NULL,PSZRECIPIENT,PSZCONTENT,nConLen,-1,-1,MU_MSG_SMS,NULL,FALSE));
    }
    else
        return FALSE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL APP_EditSMSVcardVcal(HWND hParent,const char* PSZCONTENT,int nContentLen)
{   
    int nMsgType;
    SMS_STORE smsstore;
    BOOL bRet;

    if( SMS_Opening() == TRUE )
    {
        
        memset(&smsstore,0,sizeof(SMS_STORE));
        
        smsstore.fix.Type = SMS_DELIVER;
        smsstore.fix.Conlen = nContentLen;
        smsstore.pszContent = (char*)malloc(nContentLen+1);
        
        if(smsstore.pszContent == NULL)
            return FALSE;
        
        memcpy(smsstore.pszContent,PSZCONTENT,nContentLen);
        smsstore.pszContent[nContentLen] = 0;
        
        nMsgType = SMS_IsVcradOrVcal(&smsstore);
        
        bRet = SMS_CreateEditWnd(hParent,NULL,NULL,smsstore.pszContent,smsstore.fix.Conlen,-1,-1,nMsgType,NULL,FALSE);
        
        SMS_FREE(smsstore.pszContent);
        
        return bRet;
    }
    else
        return FALSE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL APP_EntryEditSMS(HWND hParent,const char* PSZRECIPIENT,HINSTANCE hWriteSMSInstance)
{   
    return(SMS_CreateEditWnd(hParent,NULL,PSZRECIPIENT,NULL,0,
        -1,-1,MU_MSG_SMS,hWriteSMSInstance,FALSE));
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL CanMoveToFolder(void)
{
    int nFolderCount = 0;

    if(MU_GetFolderInfo(NULL,&nFolderCount))
    {
        if(nFolderCount > 0)
            return TRUE;
        else
            return FALSE;
    }
    return FALSE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_SetSIMCounter(int nCount)
{
    SMS_INITDATA* pData;

    pData = SMS_GetInitData();

    if(nCount < 0)
        nCount = 0;

    pData->nSIMCount = nCount;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int SMS_GetSIMCounter(void)
{
    SMS_INITDATA* pData;

    pData = SMS_GetInitData();

    return pData->nSIMCount;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int  sms_get_simcounter(void)
{
    return(SMS_GetSIMCounter());
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int  sms_get_cbcounter(void)
{
    return (SMS_GetCBCounter());
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL SMS_Clear(int nForderID)
{
    struct dirent *pData;
    char    szTemp;
    char FolderMask[10];
    char szOldPath[PATH_MAXLEN];

    if(nForderID != MU_DRAFT)
        return FALSE;
    
    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_SMS);  

    pDir = opendir(PATH_DIR_SMS);

    sprintf(FolderMask,"%d",nFolderID);

    while( (pData = readdir(pDir)) != NULL )
    {
        szTemp = pData->d_name[0];
        if(szTemp == FolderMask[0])
        {   
            remove(pData->d_name);
            SMS_ChangeCount(nForderID,SMS_COUNT_DEL,1);
            SMS_ChangeCount(nForderID,SMS_COUNT_ALL,-1);
        }
    }

    closedir(pDir);

    pDir = NULL;

    chdir(szOldPath);
    
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL SMS_ModifyRecordStatus(char* pszFileName,DWORD* pdwoffset,int nStatus)
{
    int f;

    char* p = NULL;

    SMS_STOREFIX storesave;
    
    char szOldPath [PATH_MAXLEN];

    DWORD datalen;

    BYTE byDel;

    DWORD nNofixLen;

    struct stat statbuf;
    
    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_SMS);  

    memset(&storesave,0,sizeof(SMS_STOREFIX));
    
    memset(&statbuf,0,sizeof(struct stat));

    stat(pszFileName,&statbuf);

    f = open(pszFileName,O_RDWR);
    
    if(f == -1)
    {
        chdir(szOldPath); 
        
        return FALSE;
    }

#ifdef _SMS_DEBUG_
    printf("\r\n*****SMS Debug Info*****  SMS_ModifyRecordStatus file handle = %d \r\n",f);
#endif
    
    lseek(f,*pdwoffset,SEEK_SET);

    read(f,&byDel,sizeof(BYTE));

    read(f,&datalen,sizeof(DWORD));
    
    if(read(f,&storesave,sizeof(SMS_STOREFIX)) == -1)
    {
        close(f);

        chdir(szOldPath); 
        
        return FALSE;
    }

    storesave.Stat = nStatus;

    nNofixLen = datalen - sizeof(SMS_STOREFIX);

    if(nNofixLen > 0)
    {
        p = (char*)malloc(nNofixLen);

        if(p == NULL)
        {
            close(f);
            
            chdir(szOldPath); 

            return FALSE;
        }

        read(f,p,nNofixLen);
    }

    lseek(f,0,SEEK_END);

    write(f,&byDel,sizeof(BYTE));

    write(f,&datalen,sizeof(DWORD));

    write(f,&storesave,sizeof(SMS_STOREFIX));

    if(nNofixLen > 0)
        write(f,p,nNofixLen);

    lseek(f,*pdwoffset,SEEK_SET);

    byDel = SMS_DEL;

    write(f,&byDel,sizeof(BYTE));

    close(f);
        
    *pdwoffset = statbuf.st_size;

    chdir(szOldPath); 
    
    SMS_FREE(p);

    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL SMS_SearchME(MU_MsgNode *pmsgnode,int nFolderID)
{    
    static int i;

    int nLen;

    SMS_INDEX* pChainNode;

    PSMS_INITDATA pData;

    SMS_INFO* psmsinfo = NULL;

    MESTORE_INFO* pMEStoreInfo = NULL;

    if(SMS_GetMEStatus() == TRUE)
        return FALSE;

    pData = SMS_GetInitData();

    if(i >= pData->nMEMaxCount)
    {
        i = 0;
        SMS_SetMEStatus(TRUE);
        return FALSE;
    }
    
    pMEStoreInfo = (MESTORE_INFO*)malloc(sizeof(MESTORE_INFO)*(pData->nMEMaxCount));

    if(pMEStoreInfo == NULL)
    {    
        i = 0;
        SMS_SetMEStatus(TRUE);
        return FALSE;
    }

    memset(pMEStoreInfo,0,sizeof(MESTORE_INFO)*(pData->nMEMaxCount));

    SMS_ReadMEStoreInfo(pMEStoreInfo,pData->nMEMaxCount);

    for(; i < pData->nMEMaxCount ; i++)
    {
        if(pData->pMESMSInfo[i].Index == 0 
            || pMEStoreInfo[pData->pMESMSInfo[i].Index - 1].nfolderid != nFolderID)
            continue;

        if(pData->pMESMSInfo[i].Index > 0)
        {
            
            psmsinfo = &(pData->pMESMSInfo[i]);

            switch(pData->pMESMSInfo[i].Stat)
            {
            case SMS_UNREAD:
            case SMS_READED:
                psmsinfo = &(pData->pMESMSInfo[i]);
                i++;
            	break;

            case SMS_UNSEND:
                if(nFolderID == MU_DRAFT)
                {
                    psmsinfo = &(pData->pMESMSInfo[i]);
                    i++;
                    break;
                }
            	break;

            case SMS_SENDED:
                psmsinfo = &(pData->pMESMSInfo[i]);
                i++;
                break;

            default:
                psmsinfo = &(pData->pMESMSInfo[i]);
                i++;
                break;
            }

            if(psmsinfo != NULL)
            {
                break;
            }
        }
    }
    
    SMS_FREE(pMEStoreInfo);

    if( psmsinfo == NULL)
    {
        i = 0;
        SMS_SetMEStatus(TRUE);
        return FALSE;
    }

    pChainNode = SMS_New();
    
    if(pChainNode == NULL)
    {
        i = 0;
        SMS_SetMEStatus(TRUE);
        return FALSE;
    }
    
    pChainNode->byStoreType = STORE_INDEX;
    pChainNode->dwOffest = psmsinfo->Index;
    
    SMS_Insert(pChainNode);
    
    //fill the modify node
    //memset(pmsgnode,0,sizeof(MU_MsgNode));
    
    //        if(SMS_IsVcradOrVcal(psmsstore))
    //            nType = MU_MSG_VCARD;
    //        else
    //            nType = MU_MSG_SMS;
    
    if( strlen(psmsinfo->SenderNum) )
    {        
        nLen = strlen(psmsinfo->SenderNum) > MU_ADDR_MAX_LEN ? MU_ADDR_MAX_LEN : strlen(psmsinfo->SenderNum);
        strncpy(pmsgnode->addr,psmsinfo->SenderNum,nLen);
        pmsgnode->addr[MU_ADDR_MAX_LEN] = 0;    
    }
    else
        pmsgnode->addr[0] = 0;
    
    //         switch(nType) 
    {
        //        case MU_MSG_VCARD:
        //            strcpy(msgnode.subject,IDS_VCARD);
        //        	break;
        //
        //        case MU_MSG_VCAL:
        //            strcpy(msgnode.subject,IDS_VCAL);
        //            break;
        //
        //        case MU_MSG_SMS:
        //        default:
        if( psmsinfo->ConLen > 0)
        {
            char* pszContent = NULL;
            int nLen;

            SMS_ParseContentEx(psmsinfo->dcs, psmsinfo->Context , psmsinfo->ConLen ,
                &pszContent,&nLen);

            nLen = nLen > MU_SUBJECT_DIS_LEN ? MU_SUBJECT_DIS_LEN : nLen;
            strncpy(pmsgnode->subject,pszContent ,nLen);
            pmsgnode->subject[MU_SUBJECT_DIS_LEN] = 0;   

            SMS_FREE(pszContent);
        }
        else
            pmsgnode->subject[0] = 0;
        //         	break;
    }
    
    pmsgnode->storage_type = MU_STORE_IN_FLASH;
    pmsgnode->msgtype = MU_MSG_SMS;//nType;
    switch(nFolderID)
    {
    case MU_INBOX:
        if(psmsinfo->Stat == SMS_UNREAD)
            pmsgnode->status = MU_STU_UNREAD;
        else
            pmsgnode->status = MU_STU_READ;
    	break;

    case MU_REPORT:
        i = 0;
        SMS_SetMEStatus(TRUE);
        return FALSE;

    case MU_DRAFT:
        pmsgnode->status = MU_STU_DRAFT;
    	break;

    case MU_SENT:
        pmsgnode->status = MU_STU_SENT;
        break;

    default:
        if(psmsinfo->Stat == SMS_UNREAD)
            pmsgnode->status = MU_STU_UNREAD;
        else
            pmsgnode->status = MU_STU_READ;
        break;
    }
    pmsgnode->maskdate = String2DWORD(psmsinfo->SendTime);
    pmsgnode->handle = (DWORD)pChainNode;

    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_ChangeStruct(SMS_STORE *psmsstore,SMS_INFO* pMESMSInfo)
{   
    memset(psmsstore,0,sizeof(SMS_STORE));

    psmsstore->fix.dcs = pMESMSInfo->dcs;
    psmsstore->fix.dwDateTime = String2DWORD(pMESMSInfo->SendTime);
    psmsstore->fix.Phonelen = strlen(pMESMSInfo->SenderNum)+1;
    strcpy(psmsstore->fix.SCA,pMESMSInfo->SCA);
    psmsstore->fix.Status = pMESMSInfo->Status;
    if(pMESMSInfo->Stat == SMS_UNREAD)
        psmsstore->fix.Stat = MU_STU_UNREAD;
    else
        psmsstore->fix.Stat = MU_STU_READ;
    psmsstore->fix.Type = pMESMSInfo->Type;
    psmsstore->fix.Udhlen  = pMESMSInfo->udhl;
    psmsstore->fix.Conlen = pMESMSInfo->ConLen;
    psmsstore->pszContent = pMESMSInfo->Context;
    
    psmsstore->pszPhone = (char*)malloc(psmsstore->fix.Phonelen+1);
    
    if(psmsstore->pszPhone == NULL)
        return FALSE;

    memset(psmsstore->pszPhone,0,psmsstore->fix.Phonelen+1);

    strcpy(psmsstore->pszPhone,pMESMSInfo->SenderNum);

    psmsstore->pszUDH = (char*)malloc(psmsstore->fix.Udhlen+1);

    if(psmsstore->pszUDH == NULL)
    {
        SMS_FREE(psmsstore->pszPhone);

        return FALSE;
    }

    memset(psmsstore->pszUDH,0,psmsstore->fix.Udhlen+1);

    strcpy(psmsstore->pszUDH,pMESMSInfo->UDH);

    if(FALSE == SMS_ParseContent(pMESMSInfo->dcs, pMESMSInfo->Context , pMESMSInfo->ConLen ,psmsstore))
    {
        SMS_FREE(psmsstore->pszPhone);

        SMS_FREE(psmsstore->pszUDH);

        return FALSE;
    }
    else
        return TRUE;
}


/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_ReadMEStoreInfo(MESTORE_INFO* pMEStoreInfo,int MEMaxCount)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    
    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_SMS);  

    f = open(SMS_FILENAME_MESTOREINFO,O_RDONLY);

    if(f == -1)
    {
        f = open(SMS_FILENAME_MESTOREINFO,O_CREAT | O_RDWR , S_IRWXU);

        if(f == -1)
        {
            chdir(szOldPath);

            return FALSE;
        }

        memset(pMEStoreInfo,0,MEMaxCount*sizeof(MESTORE_INFO));

        write(f,pMEStoreInfo,MEMaxCount*sizeof(MESTORE_INFO));

        close(f);
    }
    else
    {
        read(f,pMEStoreInfo,MEMaxCount*sizeof(MESTORE_INFO));

        close(f);
    }

    chdir(szOldPath);
    
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_WriteMEStoreInfo(MESTORE_INFO* pMEStoreInfo,int MEMaxCount)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    
    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_SMS);  

    f = open(SMS_FILENAME_MESTOREINFO,O_CREAT | O_RDWR,S_IRWXU);
    
    if(f == -1)
    {
        chdir(szOldPath);
        
        return FALSE;
    }
    else
    {
        write(f,pMEStoreInfo,MEMaxCount*sizeof(MESTORE_INFO));

        close(f);
    }

    chdir(szOldPath);
    
    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_ReadOneMEStore(MESTORE_INFO* pMEStoreInfo,int index)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    int i;
    PSMS_INITDATA pData;
    
    pData = SMS_GetInitData();
    
    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_SMS);  

    f = open(SMS_FILENAME_MESTOREINFO,O_RDONLY);

    if(f == -1)
    {
        f = open(SMS_FILENAME_MESTOREINFO,O_CREAT | O_RDWR , S_IRWXU);

        if(f == -1)
        {
            chdir(szOldPath);

            return FALSE;
        }

        memset(pMEStoreInfo,0,sizeof(MESTORE_INFO));
        
        for(i = 0 ; i < pData->nMEMaxCount ; i++)
        {            
            write(f,pMEStoreInfo,sizeof(MESTORE_INFO));
        }

        pMEStoreInfo->nfolderid = 0;

        close(f);
    }
    else
    {
        lseek(f,sizeof(MESTORE_INFO)*(index-1),SEEK_SET);

        read(f,pMEStoreInfo,sizeof(MESTORE_INFO));

        close(f);
    }

    chdir(szOldPath);
    
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_WriteOneMEStore(MESTORE_INFO* pMEStoreInfo,int index)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    
    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_SMS);  

    f = open(SMS_FILENAME_MESTOREINFO,O_RDWR);
    
    if(f == -1)
    {
        chdir(szOldPath);
        
        return FALSE;
    }
    else
    {
        lseek(f,sizeof(MESTORE_INFO)*(index-1),SEEK_SET);

        write(f,pMEStoreInfo,sizeof(MESTORE_INFO));

        close(f);
    }

    chdir(szOldPath);
    
    return TRUE;
}
/*********************************************************************\
* Function	   SMS_NewReport
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_NewReport(SMS_STORE* psmsstore,int MR)
{
    if(psmsstore->fix.Report == FALSE)
        return TRUE;

//    if(IsFlashEnough())
    {
        SMS_STORE   smsstore;
        DWORD       dwOffset = 0;
        REPORT_NODE* pNewNode = NULL;
        DWORD       handle;        
        
        memset(&smsstore,0,sizeof(SMS_STORE));

        smsstore.fix.MR = MR;
        smsstore.fix.dcs = DCS_8BIT;
        smsstore.fix.Conlen = 0;
        smsstore.fix.dwDateTime = psmsstore->fix.dwDateTime;
        smsstore.fix.Phonelen = psmsstore->fix.Phonelen;
        strcpy(smsstore.fix.SCA,psmsstore->fix.SCA);
        smsstore.fix.Stat = MU_STU_READ;
        smsstore.fix.Status = -1;
        smsstore.fix.Type = STATUS_REPORT;
        smsstore.fix.Udhlen  = 0;
        smsstore.pszContent = NULL;
        smsstore.pszPhone = psmsstore->pszPhone;
        smsstore.pszUDH = NULL;
        
        if(SMS_SaveRecord(SMS_FILENAME_REPORT,&smsstore,&dwOffset))
        {
            SMS_ChangeCount(MU_REPORT,SMS_COUNT_ALL,1);
            
            if(MU_GetCurFolderType() == MU_REPORT)
                handle = smsapi_NewRecord(dwOffset,&smsstore);

            MU_NewMsgArrival(MU_SMS_NOTIFY);   

            pNewNode = Report_New();
            
            if(pNewNode == NULL)
                return FALSE;
            
            pNewNode->dwOffset = dwOffset;
            if(MU_GetCurFolderType() == MU_REPORT)
                pNewNode->handle = handle;
            pNewNode->MR = MR;
            
            Report_Insert(pNewNode);
            
            return TRUE;
        }
        else
            return FALSE;

    }

    return FALSE;
}
/*********************************************************************\
* Function	   SMS_UpdateReport
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_UpdateReport(SMS_INFO* pSMSInfo)
{    
    SMS_STORE smsstore;
    DWORD     dwOffset;
    REPORT_NODE* pNode = NULL;
    char szOldPath[PATH_MAXLEN];

    pNode = Report_FindNode(pSMSInfo->MR);

    if(pNode == NULL)
        return FALSE;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);
    
    chdir(PATH_DIR_SMS);

    if(-1 == DeleteWithOffset(SMS_FILENAME_REPORT,pNode->dwOffset))
    {
        chdir(szOldPath);

        return FALSE;
    }

    chdir(szOldPath);

    if(MU_GetCurFolderType() == MU_REPORT)
    {
        smsapi_Delete(pNode->handle);
    }
    
    memset(&smsstore,0,sizeof(SMS_STORE));
    
    smsstore.fix.MR = pSMSInfo->MR;
    smsstore.fix.dcs = pSMSInfo->dcs;
    smsstore.fix.Conlen = pSMSInfo->ConLen;
    smsstore.fix.dwDateTime = String2DWORD(pSMSInfo->SendTime);
    smsstore.fix.Phonelen = strlen(pSMSInfo->SenderNum)+1;
    strcpy(smsstore.fix.SCA,pSMSInfo->SCA);
    smsstore.fix.Stat = MU_STU_READ;
    smsstore.fix.Status = pSMSInfo->Status;
    smsstore.fix.Type = pSMSInfo->Type;
    smsstore.fix.Udhlen  = pSMSInfo->udhl;
    smsstore.pszContent = pSMSInfo->Context;
    smsstore.pszPhone = pSMSInfo->SenderNum;
    smsstore.pszUDH = pSMSInfo->UDH;
    
    if(SMS_SaveRecord(SMS_FILENAME_REPORT,&smsstore,&dwOffset))
    {
        if(pSMSInfo->Status == 0)
            LOG_UpdateSMSStatus(pSMSInfo->MR,SMS_SEND_CONFIRM);
        else
            LOG_UpdateSMSStatus(pSMSInfo->MR,SMS_SEND_FAIL);

        pNode->dwOffset = dwOffset;
        if(MU_GetCurFolderType() == MU_REPORT)
            pNode->handle = smsapi_NewRecord(dwOffset,&smsstore);
    }    
    else
        return FALSE;

    return TRUE;
}

/*********************************************************************\
* Function	   Report_FindNode
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
REPORT_NODE* Report_FindNode(int MR)
{
    REPORT_NODE *pNode = NULL;

    pNode = pReportHeader;

    while(pNode)
    {
        if(pNode->MR == MR)
        {
            return pNode;
        }

        pNode = pNode->pNext;
    }

    return NULL;
}
/*********************************************************************\
* Function	   Report_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static REPORT_NODE* Report_New(void)
{
    REPORT_NODE* p = NULL;

    p = (REPORT_NODE*)malloc(sizeof(REPORT_NODE));
    memset(p,0,sizeof(REPORT_NODE));

    return p;
}
/*********************************************************************\
* Function	   Report_Insert
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Report_Insert(REPORT_NODE* pChainNode)
{
    REPORT_NODE* p;

    if(pReportHeader)
    {
        p = pReportHeader;

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
        pReportHeader = pChainNode;
        pChainNode->pNext = NULL;
        pChainNode->pPioneer = NULL;
    }
}
/*********************************************************************\
* Function	   Report_Delete
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Report_Delete(REPORT_NODE* pChainNode)
{
    if( pChainNode == pReportHeader )
    {
        if(pChainNode->pNext)
        {
            pReportHeader = pChainNode->pNext;
            pChainNode->pNext->pPioneer = NULL;
        }
        else
            pReportHeader = NULL;
    }
    else if( pChainNode->pNext == NULL )
    {
        pChainNode->pPioneer->pNext = NULL;
    }
    else
    {
        pChainNode->pPioneer->pNext = pChainNode->pNext;
        pChainNode->pNext->pPioneer = pChainNode->pPioneer;
    }

    SMS_FREE(pChainNode);
    pChainNode = NULL;
}
/*********************************************************************\
* Function	   Report_Erase
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Report_Erase(void)
{
    REPORT_NODE* p;    
    REPORT_NODE* ptemp;

    p = pReportHeader;

    while( p )
    {
        ptemp = p->pNext;       
        SMS_FREE(p);
        p = ptemp;
    }

    pReportHeader = NULL;
}
/*********************************************************************\
* Function	   UnConfirmReport
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void UnConfirmReport(void)
{
    char szOldPath [PATH_MAXLEN];
    int f;
    BYTE bydel;
    SMS_STOREFIX sms_storefix;
    DWORD datalen;
    struct stat buf;
    LONG filesize;
    REPORT_NODE* pChainNode = NULL;
    
    szOldPath[0] = 0;      
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_SMS);  
    
    memset(&buf,0,sizeof(struct stat));

    stat(SMS_FILENAME_REPORT,&buf);

    f = open(SMS_FILENAME_REPORT,O_RDONLY);

    if(f == -1)
    {
        chdir(szOldPath);  

        return;
    }
        
    filesize = buf.st_size;

    while( filesize > 0 )
    {
        read(f,&bydel,sizeof(BYTE));
               
        read(f,&datalen,sizeof(DWORD));
        
        if(bydel != SMS_USED)
        {
            lseek(f,datalen,SEEK_CUR);
            
            filesize -= (sizeof(BYTE) + sizeof(DWORD) + datalen);
            
            continue;
        }
      
        memset(&sms_storefix,0,sizeof(SMS_STOREFIX));
        
        if(read(f,&sms_storefix,sizeof(SMS_STOREFIX)) == -1)
        {
            close(f);

            chdir(szOldPath);  

            Report_Erase();

            return;
        }
        
        if(sms_storefix.Status == -1)
        {    
            pChainNode = Report_New();
            
            if(pChainNode == NULL)
            {
                close(f);
                
                chdir(szOldPath);
                
                Report_Erase();
                
                return;
            }
            
            pChainNode->dwOffset = buf.st_size - filesize;

            pChainNode->MR = sms_storefix.MR;
            
            Report_Insert(pChainNode);          
        }

        filesize -= (sizeof(BYTE) + sizeof(DWORD) + datalen);

        lseek(hFile,datalen- sizeof(SMS_STOREFIX),SEEK_CUR);
        
    }

    close(f);

    chdir(szOldPath);  

    return;    
}

/*********************************************************************\
* Function	   Report_InsertHandle
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Report_InsertHandle(int MR,DWORD handle)
{
    REPORT_NODE* p;  

    p = pReportHeader;

    while( p )
    {
        if(p->MR == MR)
        {
            p->handle = handle;
            return;
        }

        p = p->pNext;
    }

    return;
}
/*********************************************************************\
* Function	   SMS_GetMEStatus
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL SMS_GetMEStatus(void)
{
    return bReadMEOK;
}
/*********************************************************************\
* Function	   SMS_SetMEStatus
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMS_SetMEStatus(BOOL bOK)
{
    bReadMEOK = bOK;
}

/*********************************************************************\
* Function	   SMS_TransferData
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_TransferData(void)
{
    if(IsFlashEnough() == TRUE && SMS_IsFull())
    {
        SMS_STORE smsstore;
        DWORD     dwOffset;
        MESTORE_INFO MEStoreInfo;
        SMS_INITDATA *pData;
        int Stat = MU_STU_READ;
        int nFolder;
        SMS_INFO* pSMSInfo = NULL;
        char szFileName [SMS_FILENAME_MAXLEN];
        char* pszFileName = NULL;
        int index;
        MSG myMsg;
        
        pData = SMS_GetInitData();
        
        memset(&MEStoreInfo,0,sizeof(MESTORE_INFO));

        pSMSInfo = pData->pMESMSInfo;

        SMS_ReadOneMEStore(&MEStoreInfo,pSMSInfo->Index);

        nFolder = MEStoreInfo.nfolderid;
       
        memset(&smsstore,0,sizeof(SMS_STORE));
        
        smsstore.fix.dcs = pSMSInfo->dcs;
        smsstore.fix.dwDateTime = String2DWORD(pSMSInfo->SendTime);
        smsstore.fix.Phonelen = strlen(pSMSInfo->SenderNum)+1;
        strcpy(smsstore.fix.SCA,pSMSInfo->SCA);
        smsstore.fix.Status = pSMSInfo->Status;
        if(pSMSInfo->Stat == SMS_UNREAD)
            smsstore.fix.Stat = MU_STU_UNREAD;
        else
            smsstore.fix.Stat = MU_STU_READ;
        smsstore.fix.Type = pSMSInfo->Type;
        smsstore.fix.Udhlen  = pSMSInfo->udhl;
        smsstore.pszPhone = pSMSInfo->SenderNum;
        smsstore.pszUDH = pSMSInfo->UDH;
        smsstore.fix.Conlen = pSMSInfo->ConLen;
        smsstore.pszContent = pSMSInfo->Context;
        
        switch(nFolder)
        {            
        case MU_INBOX: // a big file
            pszFileName = SMS_FILENAME_INBOX;
            break;
            
        case MU_SENT:
            pszFileName = SMS_FILENAME_SENT;
            break;
            
        case MU_REPORT:
            pszFileName = SMS_FILENAME_REPORT;
            break;
            
        case MU_MYFOLDER:
            pszFileName = SMS_FILENAME_MYFOLDER;
            break;
            
        default:
            szFileName[0] = 0;
            sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nFolder,SMS_FILE_POSTFIX);
            if(strlen(szFileName) > (SMS_FILENAME_MAXLEN-1))   
            {
                return;
            }
            pszFileName = szFileName;
            break;
        }
                
        if(SMS_SaveRecord(pszFileName,&smsstore,&dwOffset))
        {
            index = pSMSInfo->Index;

            if(SIM_Delete(pSMSInfo->Index,SMS_MEM_MT))
            {   
                while(GetMessage(&myMsg, NULL, 0, 0)) 
                {
                    if((myMsg.message == MSG_SIM_DELETE_SUCC) && (myMsg.hwnd == SMS_GetSIMCtrlWnd()))
                    {
                        memset(pSMSInfo,0,sizeof(SMS_INFO));
                        
                        pData->nMECount--;               
                        
                        SMS_SetFull(FALSE);
                        
                        SMS_NotifyIdle();
                        
                        MEStoreInfo.nfolderid = 0;
                        
                        SMS_WriteOneMEStore(&MEStoreInfo,index);
                                                
                        break;
                    }
                    else if(myMsg.hwnd == SMS_GetSIMCtrlWnd())
                    {                        
                        break;
                    }
                    TranslateMessage(&myMsg);
                    DispatchMessage(&myMsg);
                }
            }
        }
        
        if(MU_GetCurFolderType() == nFolder)
        {
            PSMS_INDEX pTemp = NULL;
            
            pTemp = pHeader;

            while(pTemp)
            {
                if(pTemp->byStoreType == STORE_INDEX && pTemp->index == index)
                {
                    pTemp->byStoreType = STORE_OFFSET;
                    pTemp->dwOffest = dwOffset;
                    return;
                }

                pTemp = pTemp->pNext;
            }
        }
    }
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_AutoSend(void)
{
    struct dirent* pData;
    SMS_STOREFIX storefix;
    char szOldPath[PATH_MAXLEN];
    
    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  

    pDir = opendir(PATH_DIR_SMS);
    if(pDir == NULL)
    {
        chdir(szOldPath);
        return FALSE;
    }

    while( (pData = readdir(pDir)) != NULL )
    {
        if(SMS_FILE_OUTBOX_MARK == pData->d_name[0])
        {
            memset(&storefix,0,sizeof(SMS_STOREFIX));
            
            if(SMS_ReadStatusFromFile(pData->d_name,&storefix))
            {
                if(storefix.Stat == MU_STU_DEFERMENT)
                {
                    storefix.Stat = MU_STU_WAITINGSEND;
                    
                    SMS_ModifyFileStatus(pData->d_name,&storefix);
                    
                    SMS_Send(pData->d_name,NULL);
                }
            }
        }
    }
    
    closedir(pDir);
    pDir = NULL;
    chdir(szOldPath);
    
    return TRUE;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_SetMode(BOOL bFTA)
{
    int f;

    char szOldPath[PATH_MAXLEN];
    
    bFTAMode = bFTA;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);
    
    chdir(PATH_DIR_SMS);  

    f = open("smsmode.inf",O_WRONLY|O_CREAT,S_IRWXU);

    if(f == -1)
    {
        chdir(szOldPath);  

        return;
    }

    write(f,&bFTAMode,sizeof(BOOL));

    close(f);

    chdir(szOldPath);  

    return;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_GetMode(void)
{
    return bFTAMode;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_InitMode(void)
{
    int f;

    char szOldPath[PATH_MAXLEN];

    BOOL bMode;
    
    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);
    
    chdir(PATH_DIR_SMS);  

    f = open("smsmode.inf",O_RDONLY);

    if(f == -1)
    {
        f = open("smsmode.inf",O_WRONLY|O_CREAT,S_IRWXU);

        if(f == -1)
        {
            chdir(szOldPath);  
            
            SMS_SetMode(TRUE);
            
            return;
        }

        bMode = TRUE;

        write(f,&bMode,sizeof(BOOL));

        close(f);

        chdir(szOldPath);  

        SMS_SetMode(bMode);

        return;
    }

    read(f,&bMode,sizeof(BOOL));

    close(f);

    chdir(szOldPath);  

    SMS_SetMode(bMode);

    return;
}
/*********************************************************************\
* Function	   SMS_GetFileSize
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LONG SMS_GetFileSize(char *pszFileName)
{
    char szOldPath[PATH_MAXLEN];
    struct stat buf;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  

    memset(&buf,0,sizeof(struct stat));

    stat(pszFileName,&buf);

    chdir(szOldPath);
    return buf.st_size;
}
/*********************************************************************\
* Function	   SMS_GetSpace
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
LONG APP_GetSMSSize(void)
{
    struct dirent* pData;
    DIR* pDir;
    char szOldPath[PATH_MAXLEN];
    LONG lsize = 0;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  

    pDir = opendir(PATH_DIR_SMS);
    if(pDir == NULL)
    {
        chdir(szOldPath);
        return lsize;
    }

    while( (pData = readdir(pDir)) != NULL )
    {
        if(strstr(pData->d_name,".sms") != NULL)
            lsize += SMS_GetFileSize(pData->d_name);
        else if(strstr(pData->d_name,".cb") != NULL 
            && stricmp(pData->d_name,SMS_FILENAME_CBTOPIC) != 0
            && stricmp(pData->d_name,SMS_FILENAME_CBSETTING/**/) != 0)
            lsize += SMS_GetFileSize(pData->d_name);
    }
    
    closedir(pDir);
    pDir = NULL;
    chdir(szOldPath);
    return lsize;
}
