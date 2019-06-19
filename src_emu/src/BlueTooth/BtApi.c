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

#include "dirent.h"
#include "msgunibox.h" 
#include "setting.h"
#include "prioman.h"

#include "sys/types.h"
#include "sys/stat.h"
#include "sys/statfs.h"
#include "fcntl.h"
#include "unistd.h"

#include "BtMsg.h"
#include "BtFileOper.h"
#include "BtString.h"

#define BTMSG_RESETMAX 20

static HWND hBtWndmu = NULL;
PBTMSGHANDLENAME pBtMsgHandleHead=NULL;
PBTFOLDERINFO pBtFolderHead=NULL;
static int  nBtCurFolder=-1;

static BOOL Bt_init_msg(void);
static BOOL Bt_get_messages(HWND hwndmu , int folder);
static BOOL Bt_get_one_message(MU_MsgNode *pmsgnode);
static BOOL Bt_release_messages(HWND hwndmu);
static BOOL Bt_read_message(HWND hwndmu , DWORD handle,BOOL bPre, BOOL bNext);
static BOOL Bt_delete_message(HWND hwndmu , DWORD handle);
static BOOL Bt_delete_multi_messages(HWND hwndmu , DWORD handle[],int nCount);
static BOOL Bt_delete_all_messages(HWND hwndmu , int folder,MU_DELALL_TYPE ntype);
static BOOL Bt_get_detail(HWND hwndmu,DWORD handle);
static BOOL Bt_get_count(int nfolder, int *pnunread, int *pncount);
static BOOL Bt_new_folder(int folder);  
static BOOL Bt_move_message(HWND hwndmu, DWORD handle , int nFolder);

static DWORD AllocBtMsgHandle(char *filename);
PBTMSGHANDLENAME BtGetNodeByHandle(DWORD msghandle);
void SetDelFlagToFile(const char *BtRealFileName);
void FreeBtMsgHandle(PBTMSGHANDLENAME pBtMsgHandle);
int GetBtFileFolder(char *BtRealFileName);
PBTFOLDERINFO GetBtFolderByID(int nFolder);
BOOL BtDeleteMsgFile(char* filename);
static BOOL BtInitFolder(void);

void BtComeNewMsg(char* szRealFileName,char *szViewFileName);
LONG GetBtAllMsgSize(void);

static void AllocBtFolderData(int nFolder,int nUnread);
static void GetBtMsgInfoFromFile(PBTMSGFILEINFO pBtMsgFileInfo,const char *BtRealFileName);
static BOOL BtIsPicture(char* pFileName);
static BOOL BtIsText(char* pFileName);
static int GetDeletedMsgNum();
static void BtResetMsgFileInfo();

extern BOOL BtMsgInfoWindow(HWND hWndFrame, HWND hWnd, PBTMSGHANDLENAME pBtMsghandle);

extern BOOL BtViewMsgWindow(HWND hWndFrame, HWND hwnd, PBTMSGHANDLENAME pBtMsghandle, 
					                 BOOL bPre, BOOL bNext);

static MU_IMessage mu_bluetooth = 
{
    Bt_init_msg,
	Bt_get_messages,
	Bt_get_one_message,
	Bt_release_messages,
	Bt_read_message,
	Bt_delete_message,
    Bt_delete_multi_messages,
	Bt_delete_all_messages,
    NULL,
	NULL,
	Bt_get_detail,
	NULL,                       //msg_newmsg_continue,
	NULL,
	NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Bt_move_message,
	NULL,
// 	Bt_destroy,
    NULL,
    Bt_get_count,
    Bt_new_folder,
};

static HWND BtGetMuHwnd(void)
{
    return hBtWndmu;
}

static void BtSetMuHwnd(HWND hWnd)
{
    hBtWndmu = hWnd;
}

BOOL bt_register(MU_IMessage **Imsg)
{	    
#ifdef _EMULATE_
				FILE *fp;
				BTMSGFILEINFO BtMsgFileInfoTemp;
				struct dirent  *dirinfo = NULL;
				struct DIR	*dirtemp = NULL;

				mkdir(BTRECV_FILEPATH,0x666);
				
				fp=fopen(BTMSGINFOFILENAME,"ab+");
				
                if(fp==NULL)
					return FALSE;
				
				dirtemp = opendir(BTRECV_FILEPATH);
				if(dirtemp == NULL)
					return FALSE;
				do 
				{
					dirinfo = readdir(dirtemp);
					if (NULL == dirinfo)
						break;
					
					if(!strcmp(dirinfo->d_name,".") || !strcmp(dirinfo->d_name,".."))
						continue;
					
					memset(&BtMsgFileInfoTemp,0,sizeof(BTMSGFILEINFO));
					
					BtMsgFileInfoTemp.nBtMsgDeleted=1;
					BtMsgFileInfoTemp.nFolderid=MU_INBOX;
					BtMsgFileInfoTemp.nUnread=1;
					strcpy(BtMsgFileInfoTemp.szBtMsgRealName,dirinfo->d_name);
					strcpy(BtMsgFileInfoTemp.szBtMsgViewName,dirinfo->d_name);
					
					fwrite(&BtMsgFileInfoTemp,sizeof(BTMSGFILEINFO),1,fp);
					
				} while(dirinfo);
				
				closedir(dirtemp);
				
				fclose(fp);
#endif
	
#ifndef _EMULATE_
	mkdir(BTRECV_FILEPATH,0x666);
#endif 

	*Imsg = &mu_bluetooth;

	return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL Bt_init_msg(void)
{
  struct dirent  *dirinfo = NULL;
  struct DIR	*dirtemp = NULL;

  chdir(BTRECV_FILEPATH);
  
  dirtemp = opendir(BTRECV_FILEPATH);
  if(dirtemp == NULL)
	return FALSE;

    BtInitFolder();   //初始化Folder
	
  do 
  {
     dirinfo = readdir(dirtemp);
     if (NULL == dirinfo)
	    break;

	 if(!strcmp(dirinfo->d_name,".") || !strcmp(dirinfo->d_name,".."))
		 continue;
		
     AllocBtMsgHandle(dirinfo->d_name);

  } while(dirinfo);

  closedir(dirtemp);

  return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static PBTMSGHANDLENAME  pBtCurNode = NULL;
static BOOL Bt_get_messages(HWND hwndmu , int folderid)
{
	int nDeletedMsgNum;
    nBtCurFolder = folderid;
    
	BtSetMuHwnd(hwndmu);
	
	nDeletedMsgNum=GetDeletedMsgNum();

	if(nDeletedMsgNum>BTMSG_RESETMAX)
       BtResetMsgFileInfo();

    PostMessage(hwndmu, PWM_MSG_MU_GETMSGS_RESP, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_BT), 0);

	pBtCurNode = pBtMsgHandleHead;

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL Bt_get_one_message(MU_MsgNode *pmsgnode)
{
	BTMSGTIME lastWritetime;
	
	 chdir(BTRECV_FILEPATH);

	    while(pBtCurNode)
		{
			if(nBtCurFolder!=GetBtFileFolder(pBtCurNode->szBtMsgRealName))
			{
				pBtCurNode = pBtCurNode->Next;
				continue;
			}

			switch (pBtCurNode->nBtMsgType)
			{
			case MU_MSG_BT_VCARD:
				pmsgnode->msgtype=MU_MSG_BT_VCARD;
				pmsgnode->handle=(DWORD)pBtCurNode;
				pmsgnode->attachment=0;

				BtGetMsgTime(pBtCurNode->szBtMsgRealName, &lastWritetime);
				pmsgnode->maskdate=lastWritetime.dwLowDateTime;

				pmsgnode->storage_type=MU_STORE_IN_FLASH;

				pmsgnode->status=pBtCurNode->nBtMsgStatus;

				strcpy(pmsgnode->addr,IDP_BT_TITLE_BLUETOOTH);
				strcpy(pmsgnode->subject,IDP_BT_TITLE_BUSINESSCARD);
			break;

			case MU_MSG_BT_VCAL:
				pmsgnode->msgtype=MU_MSG_BT_VCAL;
				pmsgnode->handle=(DWORD)pBtCurNode;
				pmsgnode->attachment=0;
				
				BtGetMsgTime(pBtCurNode->szBtMsgRealName, &lastWritetime);				
				pmsgnode->maskdate=lastWritetime.dwLowDateTime;	
				
				pmsgnode->storage_type=MU_STORE_IN_FLASH;

                pmsgnode->status=pBtCurNode->nBtMsgStatus;

				strcpy(pmsgnode->addr,IDP_BT_TITLE_BLUETOOTH);
		        strcpy(pmsgnode->subject,IDP_BT_TITLE_CALENDARENTRY);
			break;

			case MU_MSG_BT_NOTEPAD:
				pmsgnode->msgtype=MU_MSG_BT_NOTEPAD;
				pmsgnode->handle=(DWORD)pBtCurNode;
				pmsgnode->attachment=0;

				BtGetMsgTime(pBtCurNode->szBtMsgRealName, &lastWritetime);				
				pmsgnode->maskdate=lastWritetime.dwLowDateTime;	

				pmsgnode->storage_type=MU_STORE_IN_FLASH;

                pmsgnode->status=pBtCurNode->nBtMsgStatus;
                
				strcpy(pmsgnode->addr,IDP_BT_TITLE_BLUETOOTH);	
                strcpy(pmsgnode->subject,IDP_BT_TITLE_NOTE);
			break;

			case MU_MSG_BT_PICTURE:
				pmsgnode->msgtype=MU_MSG_BT_PICTURE;
				pmsgnode->handle=(DWORD)pBtCurNode;
				pmsgnode->attachment=0;
				
				BtGetMsgTime(pBtCurNode->szBtMsgRealName, &lastWritetime);				
				pmsgnode->maskdate=lastWritetime.dwLowDateTime;	
				
				pmsgnode->storage_type=MU_STORE_IN_FLASH;
				
                pmsgnode->status=pBtCurNode->nBtMsgStatus;
				
                strcpy(pmsgnode->addr,IDP_BT_TITLE_BLUETOOTH);
                strcpy(pmsgnode->subject,pBtCurNode->szBtMsgViewName);
			break;
				
			default:
				pmsgnode->msgtype=MU_MSG_BT;
				pmsgnode->handle=(DWORD)pBtCurNode;
				pmsgnode->attachment=0;
				
				BtGetMsgTime(pBtCurNode->szBtMsgRealName, &lastWritetime);				
				pmsgnode->maskdate=lastWritetime.dwLowDateTime;	
				
				pmsgnode->storage_type=MU_STORE_IN_FLASH;
                
                pmsgnode->status=pBtCurNode->nBtMsgStatus;
				
                strcpy(pmsgnode->addr,IDP_BT_TITLE_BLUETOOTH);
                strcpy(pmsgnode->subject,pBtCurNode->szBtMsgViewName);
			break;
				
			}
			
			pBtCurNode = pBtCurNode->Next;
            
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
static BOOL Bt_release_messages(HWND hwndmu)
{
    int nDeletedMsgNum;

	BtSetMuHwnd(NULL);

	nDeletedMsgNum=GetDeletedMsgNum();
	
	if(nDeletedMsgNum>BTMSG_RESETMAX)
		BtResetMsgFileInfo();
   return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL Bt_read_message(HWND hwndmu , DWORD handle,BOOL bPre, BOOL bNext)
{
	FILE *fp;
	PBTMSGHANDLENAME  pBtMsghandle;
	BTMSGFILEINFO MsgInfoTemp;
    PBTFOLDERINFO pBtFolderInfo;

	int nUnreadMsg=0;   //已被读

	MU_MsgNode  msgnode;
	BTMSGTIME lastWritetime;
	
	pBtMsghandle = BtGetNodeByHandle(handle);

	if(pBtMsghandle->nBtMsgStatus==MU_STU_UNREAD)    //以前没有被读过
	{
		pBtMsghandle->nBtMsgStatus=MU_STU_READ;
		
		fp=fopen(BTMSGINFOFILENAME,"rb+");
		
		if(fp==NULL)
			return FALSE;
		
		while(!feof(fp))
		{
			fread(&MsgInfoTemp,sizeof(BTMSGFILEINFO),1,fp);
			
			if(!strcmp(MsgInfoTemp.szBtMsgRealName,pBtMsghandle->szBtMsgRealName))
				break;
		}
		
		fseek(fp,-8,SEEK_CUR);
		
		fwrite(&nUnreadMsg,sizeof(int),1,fp);     //写log文件
		
		fclose(fp);
	}

	pBtFolderInfo=GetBtFolderByID(MU_INBOX);

	pBtFolderInfo->nBtUnreadNum=pBtFolderInfo->nBtUnreadNum-1;
	
    MsgNotify(MU_BT_NOTIFY,FALSE,pBtFolderInfo->nBtUnreadNum,0);

	BtViewMsgWindow(MuGetFrame(),hwndmu, pBtMsghandle, bPre, bNext);

	chdir(BTRECV_FILEPATH);
	
	msgnode.status=MU_STU_READ;
	msgnode.handle=handle;
    msgnode.attachment=0;
	
	BtGetMsgTime(pBtMsghandle->szBtMsgRealName, &lastWritetime);
	msgnode.maskdate=lastWritetime.dwLowDateTime;
	
	msgnode.msgtype=pBtMsghandle->nBtMsgType;
	msgnode.storage_type=MU_STORE_IN_FLASH;
	strcpy(msgnode.addr,IDP_BT_TITLE_BLUETOOTH);
	
	if(pBtMsghandle->nBtMsgType==MU_MSG_BT_VCARD)
		strcpy(msgnode.subject,IDP_BT_TITLE_BUSINESSCARD);
	else if(pBtMsghandle->nBtMsgType==MU_MSG_BT_VCAL)
		strcpy(msgnode.subject,IDP_BT_TITLE_CALENDARENTRY);
	else if(pBtMsghandle->nBtMsgType==MU_MSG_BT_NOTEPAD)
		strcpy(msgnode.subject,IDP_BT_TITLE_NOTE);
	else
		strcpy(msgnode.subject,pBtMsghandle->szBtMsgViewName);
	
	SendMessage(hwndmu, PWM_MSG_MU_MODIFIED,   //告诉Unibox此信息已读
		MAKEWPARAM(MU_ERR_SUCC, MU_MDU_BT), 
		(LPARAM)&msgnode);
	
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL Bt_delete_message(HWND hwndmu , DWORD handle)
{
   BOOL bRet;

   PBTMSGHANDLENAME  pBtMsghandle;

   pBtMsghandle = BtGetNodeByHandle(handle);
   
   if(pBtMsghandle == NULL)
	   return FALSE;

   chdir(BTRECV_FILEPATH);
   
   bRet=BtDeleteMsgFile(pBtMsghandle->szBtMsgRealName);

   if (!bRet)
   {
	   SendMessage(hwndmu, PWM_MSG_MU_DELETED_RESP, MAKEWPARAM(MU_ERR_FAILED, MU_MDU_BT),
		   (LPARAM)handle);
	   return FALSE;
   }
   else
   {
	   FreeBtMsgHandle(pBtMsghandle);  //释放链表中的相应节点
	   
	   SendMessage(hwndmu, PWM_MSG_MU_DELETED_RESP, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_BT),
		   (LPARAM)handle);
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
static BOOL Bt_delete_multi_messages(HWND hwndmu , DWORD handle[],int nCount)
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
static BOOL Bt_delete_all_messages(HWND hwndmu, int folder, MU_DELALL_TYPE ntype)
{
	int nFolder;
	BOOL bRet;
	
	PBTMSGHANDLENAME pBtMsgHandleTemp =NULL,pNodeNext=NULL;
	PBTFOLDERINFO p=NULL, pBtFolderTemp = NULL;
	
	if(pBtMsgHandleHead == NULL)
		return FALSE;
	
	if(pBtFolderHead == NULL)
		return FALSE;
	
	chdir(BTRECV_FILEPATH);
	
	pBtMsgHandleTemp = pBtMsgHandleHead;
	
	while(pBtMsgHandleTemp)
	{
		nFolder =GetBtFileFolder(pBtMsgHandleTemp->szBtMsgRealName);
		
		if (nFolder == folder)
		{ 
			bRet=BtDeleteMsgFile(pBtMsgHandleTemp->szBtMsgRealName);

			if(!bRet)
			{
				SendMessage(hwndmu,PWM_MSG_MU_DELALL_RESP,
					MAKEWPARAM(MU_ERR_FAILED,MU_MDU_BT),(LPARAM)folder);
				return FALSE;				
			}
            else
			{
				pNodeNext = pBtMsgHandleTemp->Next;
				FreeBtMsgHandle(pBtMsgHandleTemp);  //释放链表中的相应节点
				pBtMsgHandleTemp = pNodeNext;
			}
		}
		else
			pBtMsgHandleTemp = pBtMsgHandleTemp->Next;
	}

	pBtFolderTemp=pBtFolderHead;
			
	if(ntype == MU_DELALL_FOLDER)
	{
		while (pBtFolderTemp->nBtFolderID!= folder && pBtFolderTemp->Next!=NULL)				 
		{
			p = pBtFolderTemp;
			pBtFolderTemp = pBtFolderTemp->Next; 
		}
		
		if(pBtFolderTemp->nBtFolderID==folder) 
		{
			if(pBtFolderTemp==pBtFolderHead) 
			{
				pBtFolderHead=pBtFolderHead->Next;
				free(pBtFolderTemp); 
			}
			else
			{
				p->Next=pBtFolderTemp->Next;
				free(pBtFolderTemp);
			}
		}
	}
		
    SendMessage(hwndmu, PWM_MSG_MU_DELALL_RESP, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_BT), 0);
		
    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL Bt_get_detail(HWND hwndmu,DWORD handle)
{
	PBTMSGHANDLENAME  pBtMsghandle;
	
    pBtMsghandle = BtGetNodeByHandle(handle);

	BtMsgInfoWindow(MuGetFrame(), hwndmu, pBtMsghandle);
     
    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL Bt_move_message(HWND hwndmu, DWORD handle , int nFolder)
{
	FILE *fp;
	PBTMSGHANDLENAME  pBtMsghandle;
    BTMSGFILEINFO MsgInfoTemp;
	PBTFOLDERINFO pBtFolderInfo;
		
	fp=fopen(BTMSGINFOFILENAME,"rb+");

	if(fp==NULL)
		return FALSE;
	
	pBtMsghandle = BtGetNodeByHandle(handle);

	pBtFolderInfo=GetBtFolderByID(pBtMsghandle->nCurFolder);
	
	//这个蓝牙信息原来所在的箱子的数目减少一个
	if(pBtMsghandle->nBtMsgStatus)  //未读   
	{
		pBtFolderInfo->nBtUnreadNum--;
		pBtFolderInfo->nBtTotalNum--;
	}
	else
		pBtFolderInfo->nBtTotalNum--;

	pBtMsghandle->nCurFolder=nFolder;
	
	while(!feof(fp))
	{
		fread(&MsgInfoTemp,sizeof(BTMSGFILEINFO),1,fp);
        
		if(!strcmp(MsgInfoTemp.szBtMsgRealName,pBtMsghandle->szBtMsgRealName))
            break;
	}
    
	fseek(fp,-4,SEEK_CUR);

	fwrite(&nFolder,sizeof(int),1,fp);     //写log文件

	fclose(fp);

	pBtFolderInfo=GetBtFolderByID(nFolder);
    
	if(pBtMsghandle->nBtMsgStatus)  //未读
	{
		pBtFolderInfo->nBtUnreadNum++;
		pBtFolderInfo->nBtTotalNum++;
	}
	else
		pBtFolderInfo->nBtTotalNum++;
    
    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL Bt_get_count(int nfolder, int *pnunread, int *pncount)
{
     PBTFOLDERINFO pBtFolderInfo;
     
     *pnunread=0;
     *pncount=0;

	 pBtFolderInfo=GetBtFolderByID(nfolder);

	 if(pBtFolderInfo == NULL)
		 return FALSE;
	 else
	 {
		 *pnunread=pBtFolderInfo->nBtUnreadNum;
		 *pncount=pBtFolderInfo->nBtTotalNum;
		 
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

static BOOL Bt_new_folder(int folder)
{
	PBTFOLDERINFO pBtFolderInfo = NULL, p = NULL;
	
	pBtFolderInfo =(PBTFOLDERINFO)malloc(sizeof(BTFOLDERINFO));
	
	if(pBtFolderInfo == NULL)
		return FALSE;

    pBtFolderInfo->nBtFolderID=folder;
	pBtFolderInfo->nBtTotalNum=0;
	pBtFolderInfo->nBtUnreadNum=0;
    pBtFolderInfo->Next=NULL;
	
	if(NULL==pBtFolderHead)
	{
		pBtFolderHead = pBtFolderInfo;
		return TRUE;
	}
	else
	{
		p = pBtFolderHead;
		while(p->Next)
		{
			p = p->Next;
		}
		p->Next = pBtFolderInfo;
	}
	
	return TRUE;
}


DWORD AllocBtMsgHandle(char *filename)   //为蓝牙所在文件夹中的每一个文件分配一个handle
{
	 BTMSGFILEINFO BtMsgFileInfo;

     PBTMSGHANDLENAME p = NULL,pTemp = NULL;
     char suffix[16];

     p = (PBTMSGHANDLENAME)malloc(sizeof(BTMSGHANDLENAME));

     if(p == NULL)
	    return NULL;    

     memset(p, 0, sizeof(BTMSGHANDLENAME));

     GetBtMsgInfoFromFile(&BtMsgFileInfo,filename);   //从log文件中得到消息的信息

     strcpy(p->szBtMsgViewName, BtMsgFileInfo.szBtMsgViewName);
     strcpy(p->szBtMsgRealName, BtMsgFileInfo.szBtMsgRealName);

	 p->nCurFolder=BtMsgFileInfo.nFolderid;
     
	 p->nBtMsgStatus=BtMsgFileInfo.nUnread;   //已读未读标志,未读是1，已读是0
     
     AllocBtFolderData(BtMsgFileInfo.nFolderid,BtMsgFileInfo.nUnread);
	 //为Folder初始化数据

     GetSuffixFileName(filename,NULL,suffix);
     
     if(!strcmp(suffix,"vcf") || !strcmp(suffix,"VCF")) 
         p->nBtMsgType=MU_MSG_BT_VCARD;
	 else if(!strcmp(suffix,"vcs") || !strcmp(suffix,"VCS"))
		 p->nBtMsgType=MU_MSG_BT_VCAL;
	 else if(BtIsPicture(filename))
         p->nBtMsgType=MU_MSG_BT_PICTURE;
	 else if(BtIsText(filename))
         p->nBtMsgType=MU_MSG_BT_NOTEPAD; 
	 else
	     p->nBtMsgType=MU_MSG_BT;
	
     if(!pBtMsgHandleHead)
	     pBtMsgHandleHead = p;
     else
     {
         pTemp=pBtMsgHandleHead;
		 
		 while(pTemp->Next)
			 pTemp=pTemp->Next;
		 
		 pTemp->Next=p;	    
     }

	p->Next = NULL;

    return (DWORD)p;
}

PBTMSGHANDLENAME BtGetNodeByHandle(DWORD msghandle)
{
	PBTMSGHANDLENAME pBtMsgHandle = NULL;
	
	pBtMsgHandle = (PBTMSGHANDLENAME)msghandle;
	
    return pBtMsgHandle;
}

void GetBtMsgInfoFromFile(PBTMSGFILEINFO pBtMsgFileInfo,const char *BtRealFileName)
{
	FILE *fp;
    BTMSGFILEINFO MsgInfoTemp;

	memset(&MsgInfoTemp,0,sizeof(BTMSGFILEINFO));

	chdir(BTMSGINFO_FILEPATH);

	printf("\r\n#####In GetBtMsgInfoFromFile######\r\n");

	printf("\r\n#####BtRealFileName=%s######\r\n",BtRealFileName);
	
	fp=fopen(BTMSGINFOFILENAME,"rb+");

	printf("\r\n#####fp=0x%x######\r\n",fp);

    if(fp==NULL)
		return;

    while(!feof(fp))
	{
		fread(&MsgInfoTemp,sizeof(BTMSGFILEINFO),1,fp);

        printf("\r\n#####MsgInfoTemp.szBtMsgRealName=%s######\r\n",MsgInfoTemp.szBtMsgRealName);
        printf("\r\n#####MsgInfoTemp.szBtMsgViewName=%s######\r\n",MsgInfoTemp.szBtMsgViewName);
        printf("\r\n#####MsgInfoTemp.nBtMsgDeleted=%d######\r\n",MsgInfoTemp.nBtMsgDeleted);
        printf("\r\n#####MsgInfoTemp.nFolderid=%d######\r\n",MsgInfoTemp.nFolderid);
        printf("\r\n#####MsgInfoTemp.nUnread=%d######\r\n",MsgInfoTemp.nUnread);

		if(!strcmp(MsgInfoTemp.szBtMsgRealName,BtRealFileName))
            break;
	}

	memcpy(pBtMsgFileInfo,&MsgInfoTemp,sizeof(BTMSGFILEINFO));

	fclose(fp);
}

void SetDelFlagToFile(const char *BtRealFileName)
{
   FILE *fp;
   int nDeletFlag=0;

   BTMSGFILEINFO MsgInfoTemp;

   chdir(BTMSGINFO_FILEPATH);

   fp=fopen(BTMSGINFOFILENAME,"rb+");

   if(fp==NULL)
	   return;
   
   while(!feof(fp))
   {
	   fread(&MsgInfoTemp,sizeof(BTMSGFILEINFO),1,fp);
	   
	   if(!strcmp(MsgInfoTemp.szBtMsgRealName,BtRealFileName))
	   {
          fwrite(&nDeletFlag,sizeof(int),1,fp);
          break;
	   }
   }
   
   fclose(fp);
}

void FreeBtMsgHandle(PBTMSGHANDLENAME pBtMsgHandle)
{
    PBTMSGHANDLENAME pBtMsgHandleTemp,p=NULL;
	
	pBtMsgHandleTemp=pBtMsgHandleHead;
	
	while (pBtMsgHandleTemp!=pBtMsgHandle && pBtMsgHandleTemp->Next!=NULL)				 
	{
		p = pBtMsgHandleTemp;
		pBtMsgHandleTemp = pBtMsgHandleTemp->Next; 
	}
	
	if(pBtMsgHandleTemp==pBtMsgHandle) 
	{
	    if(pBtMsgHandleTemp==pBtMsgHandleHead) 
		{
			pBtMsgHandleHead=pBtMsgHandleHead->Next;
			free(pBtMsgHandleTemp); 
		}
		else
		{
			p->Next=pBtMsgHandleTemp->Next;
			free(pBtMsgHandleTemp);
		}
	}
}

int GetBtFileFolder(char *BtRealFileName)
{
	PBTMSGHANDLENAME pBtMsgHandleTemp;

	pBtMsgHandleTemp=pBtMsgHandleHead;

    while(pBtMsgHandleTemp!=NULL)
	{
		if(!strcmp(pBtMsgHandleTemp->szBtMsgRealName,BtRealFileName))
			break;
       
        pBtMsgHandleTemp=pBtMsgHandleTemp->Next;
	}
	
	return pBtMsgHandleTemp->nCurFolder;
}

BOOL BtDeleteMsgFile(char* filename)
{	    
	BOOL bRet;
	int  nStatus,nFolder;
    BTMSGFILEINFO BtMsgFileInfoTemp;
	PBTFOLDERINFO pBtFolderInfoTemp;

	char szDelFileName[MAX_BTPATHNAME];

	memset(szDelFileName,0,MAX_BTPATHNAME);

	GetBtMsgInfoFromFile(&BtMsgFileInfoTemp,filename);

    nStatus=BtMsgFileInfoTemp.nUnread;
	nFolder=GetBtFileFolder(filename);

// 	chdir(BTRECV_FILEPATH);

    strcpy(szDelFileName,"/mnt/flash/bluetooth/inbox/");
	strcat(szDelFileName,filename);
	
    bRet = remove(szDelFileName);        //删除实际的的文件
		     
	if (bRet == 0)
	{
	  SetDelFlagToFile(filename);   //在log文件中设置删除标志

      pBtFolderInfoTemp=GetBtFolderByID(nFolder);     //同步Folder信息

	  if(nStatus)      //此信息没有被读过  
	  {
         pBtFolderInfoTemp->nBtUnreadNum--;
         pBtFolderInfoTemp->nBtTotalNum--;
	  }
	  else
		 pBtFolderInfoTemp->nBtTotalNum--;
	}
	
   return (bRet == 0);
}


PBTFOLDERINFO GetBtFolderByID(int nFolder)
{
	PBTFOLDERINFO p = NULL;
	
	if(pBtFolderHead == NULL)
		return NULL;
	
	p = pBtFolderHead;
	while(p)
	{
		if(p->nBtFolderID == nFolder)
			return p;
		else
		  p = p->Next;
	}

	return NULL;	
}

static BOOL BtInitFolder(void)
{
	PMU_FOLDERINFO pInfo =NULL, pTemp =NULL;
	int nCount;
	PBTFOLDERINFO  pBtFolderInfo = NULL, p =NULL;
	
	pBtFolderInfo =(PBTFOLDERINFO)malloc(sizeof(BTFOLDERINFO));
		
	if(pBtFolderInfo== NULL)
			return FALSE;
		
	pBtFolderInfo->nBtFolderID = MU_INBOX;
	
	pBtFolderInfo->nBtUnreadNum=0;
    pBtFolderInfo->nBtTotalNum=0;
	pBtFolderInfo->Next=NULL;
		
	if(pBtFolderHead == NULL)
		pBtFolderHead = pBtFolderInfo;
	else
	{
		p = pBtFolderHead;
		while(p->Next)
		{
			p= p->Next;
		}
			p->Next = pBtFolderInfo;
	}
	
	
	MU_GetFolderInfo(NULL, &nCount);
	
	if(nCount !=0)
	{
		pInfo =(PMU_FOLDERINFO)malloc(sizeof(MU_FOLDERINFO)*nCount);
		if(pInfo == NULL)
			return FALSE;		
	}
	
	MU_GetFolderInfo(pInfo, &nCount);
	
	pTemp = pInfo;
	
	while(nCount>0)
	{
		pBtFolderInfo =(PBTFOLDERINFO)malloc(sizeof(BTFOLDERINFO));

		if(pBtFolderInfo == NULL)
			return FALSE;

		pBtFolderInfo->nBtFolderID = pInfo->nFolderID;
		
        pBtFolderInfo->nBtUnreadNum=0;
		pBtFolderInfo->nBtTotalNum=0;
		pBtFolderInfo->Next=NULL;
		
		if(pBtFolderHead == NULL)
			pBtFolderHead = pBtFolderInfo;
		else
		{
			p = pBtFolderHead;
			while(p->Next)
			{
				p= p->Next;
			}
			p->Next = pBtFolderInfo;
		}
		
		pInfo++;
		nCount--;
	}
	
	free(pTemp);

    return TRUE;
	
}

static void AllocBtFolderData(int nFolder,int nUnread)
{
	PBTFOLDERINFO  pBtFolderInfo = NULL;

	pBtFolderInfo=pBtFolderHead;

	while(pBtFolderInfo!=NULL)
	{
		if(pBtFolderInfo->nBtFolderID==nFolder)
		{
          if(nUnread)
		  {
			  pBtFolderInfo->nBtUnreadNum++;
			  pBtFolderInfo->nBtTotalNum++;
		  }
		  else
			  pBtFolderInfo->nBtTotalNum++;
		}
       
      pBtFolderInfo=pBtFolderInfo->Next;
	}
}

static BOOL BtIsPicture(char* pFileName)     //判断是否是张图片
{
	char *Suffix;
	
	Suffix = strrchr(pFileName, '.');
	if (!Suffix) 
		return FALSE;
	
	if ((strcmp(Suffix, ".bmp") == 0)
		|| (strcmp(Suffix, ".BMP") == 0))
		return TRUE;

	else if((strcmp(Suffix, ".gif") == 0) 
		|| (strcmp(Suffix, ".GIF") == 0))
		return TRUE;

	else if((strcmp(Suffix, ".jpg") == 0) 
		|| (strcmp(Suffix, ".JPG") == 0))
		return TRUE;

	else if((strcmp(Suffix, ".jpeg") == 0) 
		|| (strcmp(Suffix, ".JPEG") == 0))
		return TRUE;

	else if((strcmp(Suffix, ".wbmp") == 0) 
		|| (strcmp(Suffix, ".WBMP") == 0))
		return TRUE;

	else if((strcmp(Suffix, ".png") == 0) 
		|| (strcmp(Suffix, ".PNG") == 0))
		return TRUE;
	else
		return FALSE;	
}

static BOOL BtIsText(char* pFileName)     //判断是否是文本
{
	char *Suffix;
	
	Suffix = strrchr(pFileName, '.');
	if (!Suffix) 
		return FALSE;

    if ((strcmp(Suffix, ".txt") == 0)
		|| (strcmp(Suffix, ".TXT") == 0))
		return TRUE;
	else
		return FALSE;
}

static int GetDeletedMsgNum()
{
	int nDeletedMsgNum=0;
    
	FILE *fp;
    BTMSGFILEINFO MsgInfoTemp;

	chdir(BTMSGINFO_FILEPATH);
    
    fp=fopen(BTMSGINFOFILENAME,"rb+");

	if(fp==NULL)
		return 0;
   
	while(!feof(fp))
	{
	  fread(&MsgInfoTemp,sizeof(BTMSGFILEINFO),1,fp);
      
	  if(MsgInfoTemp.nBtMsgDeleted==0)
         nDeletedMsgNum++;
	}
    
	fclose(fp);

    return nDeletedMsgNum;
}

static void BtResetMsgFileInfo()
{
   FILE *fpOld,*fpTemp;
   BTMSGFILEINFO MsgInfoTemp;
   
   chdir(BTMSGINFO_FILEPATH);
   
   fpOld=fopen(BTMSGINFOFILENAME,"rb+");
   
   if(fpOld==NULL)
	   return;

   fpTemp=fopen("/mnt/flash/bluetooth/btmsginfotmp.txt","wb+");

   if(fpTemp==NULL)
	   return;

   while(!feof(fpOld))
   {
	   fread(&MsgInfoTemp,sizeof(BTMSGFILEINFO),1,fpOld);
	   
	   if(MsgInfoTemp.nBtMsgDeleted==1)
		  fwrite(&MsgInfoTemp,sizeof(BTMSGFILEINFO),1,fpTemp);
   }

   fclose(fpOld);   
   fclose(fpTemp);

   remove(BTMSGINFOFILENAME);  //删除原来的log文件
   
   rename("/mnt/flash/bluetooth/btmsginfotmp.txt",BTMSGINFOFILENAME);
}

//来了一条新的蓝牙信息
void BtComeNewMsg(char* szRealFileName,char *szViewFileName)
{
	BTMSGTIME lastWritetime;
    MU_MsgNode  msgnode;	
	FILE *fp;				
	BTMSGFILEINFO MsgInfoTemp;
	PBTFOLDERINFO pBtFolderInfo;

	int nBtOldUnreadNum;

	PBTMSGHANDLENAME p = NULL,pTemp = NULL;
	char suffix[16];

	memset(&MsgInfoTemp,0,sizeof(BTMSGFILEINFO));
	
    PrioMan_CallMusic(PRIOMAN_PRIORITY_BLUETOOTH, 1);

    pBtFolderInfo=GetBtFolderByID(MU_INBOX);

	nBtOldUnreadNum=pBtFolderInfo->nBtUnreadNum;

	MsgNotify(MU_BT_NOTIFY,FALSE,nBtOldUnreadNum+1,0);
	
// 	chdir(BTMSGINFO_FILEPATH);

	fp=fopen(BTMSGINFOFILENAME,"ab+");

	if(fp==NULL)
	      return;
			 
	MsgInfoTemp.nBtMsgDeleted=1;
	MsgInfoTemp.nFolderid=MU_INBOX;
	MsgInfoTemp.nUnread=1;
	strcpy(MsgInfoTemp.szBtMsgRealName,szRealFileName);
	strcpy(MsgInfoTemp.szBtMsgViewName,szViewFileName);
			 
	fwrite(&MsgInfoTemp,sizeof(BTMSGFILEINFO),1,fp);
			 
	fclose(fp);

	p = (PBTMSGHANDLENAME)malloc(sizeof(BTMSGHANDLENAME));
	
	if(p == NULL)
	      return; 

	GetSuffixFileName(szViewFileName,NULL,suffix);
		
	if(!strcmp(suffix,"vcf") || !strcmp(suffix,"VCF")) 
	{
		p->nBtMsgType=MU_MSG_BT_VCARD;
        p->nBtMsgStatus=MU_STU_UNREAD;
		p->nCurFolder=MU_INBOX;
        strcpy(p->szBtMsgRealName,szRealFileName);
		strcpy(p->szBtMsgViewName,szViewFileName);	
	}
	else if(!strcmp(suffix,"vcs") || !strcmp(suffix,"VCS"))
	{
		p->nBtMsgType=MU_MSG_BT_VCAL;
		p->nBtMsgStatus=MU_STU_UNREAD;
		p->nCurFolder=MU_INBOX;
        strcpy(p->szBtMsgRealName,szRealFileName);
		strcpy(p->szBtMsgViewName,szViewFileName);
	}
	
	else if(BtIsPicture(szViewFileName))
	{
		p->nBtMsgType=MU_MSG_BT_PICTURE;
		p->nBtMsgStatus=MU_STU_UNREAD;
		p->nCurFolder=MU_INBOX;
        strcpy(p->szBtMsgRealName,szRealFileName);
		strcpy(p->szBtMsgViewName,szViewFileName);

	}
	else if(BtIsText(szViewFileName))
	{
		p->nBtMsgType=MU_MSG_BT_NOTEPAD;
		p->nBtMsgStatus=MU_STU_UNREAD;
		p->nCurFolder=MU_INBOX;
        strcpy(p->szBtMsgRealName,szRealFileName);
		strcpy(p->szBtMsgViewName,szViewFileName);

	}
	else
	{
		p->nBtMsgType=MU_MSG_BT;
		p->nBtMsgStatus=MU_STU_UNREAD;
		p->nCurFolder=MU_INBOX;
        strcpy(p->szBtMsgRealName,szRealFileName);
		strcpy(p->szBtMsgViewName,szViewFileName);
    }
	
	if(!pBtMsgHandleHead)
		pBtMsgHandleHead = p;
	else
	{
		pTemp=pBtMsgHandleHead;
		
		while(pTemp->Next)
			pTemp=pTemp->Next;
		
		pTemp->Next=p;	    
	}
	
	p->Next = NULL;

	chdir(BTRECV_FILEPATH);

	if(MU_GetCurFolderType()==MU_INBOX)
	{
		memset(&msgnode,0,sizeof(MU_MsgNode));

        if(p->nBtMsgType==MU_MSG_BT_VCARD)
		{
			msgnode.msgtype=MU_MSG_BT_VCARD;
			msgnode.handle=(DWORD)p;
			msgnode.attachment=0;
			
			BtGetMsgTime(szRealFileName, &lastWritetime);				
			msgnode.maskdate=lastWritetime.dwLowDateTime;	
			
			msgnode.storage_type=MU_STORE_IN_FLASH;			
			msgnode.status=MU_STU_UNREAD;
			
			strcpy(msgnode.addr,IDP_BT_TITLE_BLUETOOTH);
			strcpy(msgnode.subject,IDP_BT_TITLE_BUSINESSCARD);
		}
		else if(p->nBtMsgType==MU_MSG_BT_VCAL)
		{
			msgnode.msgtype=MU_MSG_BT_VCAL;
			msgnode.handle=(DWORD)p;
			msgnode.attachment=0;
			
			BtGetMsgTime(szRealFileName, &lastWritetime);				
			msgnode.maskdate=lastWritetime.dwLowDateTime;	
			
		    msgnode.storage_type=MU_STORE_IN_FLASH;			
			msgnode.status=MU_STU_UNREAD;
			
			strcpy(msgnode.addr,IDP_BT_TITLE_BLUETOOTH);
			strcpy(msgnode.subject,IDP_BT_TITLE_CALENDARENTRY);
		}
		else if(p->nBtMsgType==MU_MSG_BT_NOTEPAD)
		{
			msgnode.msgtype=MU_MSG_BT_NOTEPAD;
			msgnode.handle=(DWORD)p;
			msgnode.attachment=0;
			
			BtGetMsgTime(szRealFileName, &lastWritetime);				
			msgnode.maskdate=lastWritetime.dwLowDateTime;	
			
			msgnode.storage_type=MU_STORE_IN_FLASH;			
			msgnode.status=MU_STU_UNREAD;
			
			strcpy(msgnode.addr,IDP_BT_TITLE_BLUETOOTH);
			strcpy(msgnode.subject,IDP_BT_TITLE_NOTE);
		}
		else if(p->nBtMsgType==MU_MSG_BT_PICTURE) 
		{
            msgnode.msgtype=MU_MSG_BT_PICTURE;
			msgnode.handle=(DWORD)p;
			msgnode.attachment=0;
			
			BtGetMsgTime(szRealFileName, &lastWritetime);				
			msgnode.maskdate=lastWritetime.dwLowDateTime;	
			
			msgnode.storage_type=MU_STORE_IN_FLASH;			
			msgnode.status=MU_STU_UNREAD;
			
			strcpy(msgnode.addr,IDP_BT_TITLE_BLUETOOTH);
			strcpy(msgnode.subject,szViewFileName);
		}
		else
		{
			msgnode.msgtype=MU_MSG_BT;
			msgnode.handle=(DWORD)p;
			msgnode.attachment=0;
			
			BtGetMsgTime(szRealFileName, &lastWritetime);				
			msgnode.maskdate=lastWritetime.dwLowDateTime;	
			
			msgnode.storage_type=MU_STORE_IN_FLASH;			
			msgnode.status=MU_STU_UNREAD;
			
			strcpy(msgnode.addr,IDP_BT_TITLE_BLUETOOTH);
			strcpy(msgnode.subject,szViewFileName);			
		}
        
		SendMessage(BtGetMuHwnd(),PWM_MSG_MU_NEWMTMSG,
			MAKEWPARAM(MU_ERR_SUCC,MU_MDU_BT),(LPARAM)&msgnode);
	}
    
	AllocBtFolderData(MU_INBOX,1);  //未读的信息加1

	MU_NewMsgArrival(MU_BT_NOTIFY);

}

LONG GetBtAllMsgSize(void)
{
    static struct dirent *dirinfo = NULL;
    static DIR  *diropen = NULL;
    struct stat SourceStat;
	
    char szOldPath[MAX_BTPATHNAME];
	
    LONG lsize = 0;
    int lCount = 0;
	
    memset(szOldPath, 0x0, MAX_BTPATHNAME);
    getcwd(szOldPath, MAX_BTPATHNAME); 
	
    chdir(BTRECV_FILEPATH); 
	
    diropen = opendir((char *)BTRECV_FILEPATH);

    if(diropen == NULL)
        return 0;
    
    while(dirinfo = readdir(diropen))
    {
        if(stat(dirinfo->d_name, &SourceStat) == -1)
        {
            closedir(diropen);
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

    closedir(diropen);

    chdir(szOldPath);
	
    return lsize;
}
