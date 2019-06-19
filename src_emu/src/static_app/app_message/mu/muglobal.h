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
#ifndef _MU_GLOBAL_H
#define _MU_GLOBAL_H

#ifndef _TEST_SMS_
#define _TEST_SMS_
#endif

#define _TEST_EMAIL_
#define _TEST_MMS_
#define _TEST_BLUETOOTH_

#include <project/msgpublic.h>
#include <project/msgunibox.h>
#include <public/pubapp.h>
#include <project/mullang.h>
#include <project/imesys.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "plx_pdaex.h"
#include "plxdebug.h"
#include "muconst.h"
#include "murs.h"
#include "mumemory.h"
#include "PhonebookExt.h"

typedef struct tagMU_InitStatus
{
    BOOL    bSMS;
    BOOL    bMMS;
    BOOL    bEmail;
    BOOL    bPush;
	BOOL    bBT;
}MU_INITSTATUS,*PMU_INITSTATUS;

typedef struct tagMU_ChainNode
{
	BYTE	msgtype;	 /* NOT module type. *///MU_MSG_TYPE
	BYTE	status;       
	BYTE	storage_type;
	BYTE	attachment;
	DWORD	maskdate;	  /* For MT message, means receiving date; For MO message, means writing date. */
	DWORD	handle;  /* It is the individual identifier of the message, it maybe a id number or a pointer. */
	char    addr[32];
    struct tagMU_ChainNode* pPioneer;  //the pointer of the pioneer node
    struct tagMU_ChainNode* pNext;  //the pointer of the next node
}MU_CHAINNODE,*PMU_CHAINNODE;

typedef struct tagMU_Counter
{
    LONG    nSMS;
    LONG    nMMS;
    LONG    nEmail;
    LONG    nEmailHeader;
    LONG    nPush;  
    LONG    nUnread;
	LONG    nBT;
}MU_COUNTER;

typedef struct tagMU_ListCreateData
{
    BOOL    bEmailBox;
    int     nBoxType;
    char    szEmailName[MU_EMAILBOX_NAME_LEN];
	char    szFolderName[64];
    BOOL    bChangeMenu;
	HMENU	hMenu;
    MU_COUNTER mu_counter;
    PMU_CHAINNODE pSelect;
    PMU_CHAINNODE pFirst;
    PMU_CHAINNODE pUnReadFirst;
}MU_LISTCREATEDATA,*PMU_LISTCREATEDATA;

typedef struct tagMu_SelListData
{
#define DEL_SELECT		1
#define MOVE_SELECT		2
	int    SelType;
	HWND   hListOwner;
	PMU_LISTCREATEDATA pOwner;	
}MU_SELLISTDATA,*PMU_SELLISTDATA;

typedef struct tagMU_FolderChainNode
{
    DWORD dwoffset;
    MU_FOLDERINFO folderinfo;
    struct tagMU_FolderChainNode* pPioneer;  //the pointer of the pioneer node
    struct tagMU_FolderChainNode* pNext;  //the pointer of the next node
}MU_FOLDERCHAINNODE,*PMU_FOLDERCHAINNODE;

typedef struct tagMU_FolderData
{
    int     nBoxType;
    int     nFocus;
    int     nCurSel;
	HMENU   hMenu;
    PMU_FOLDERCHAINNODE pFirst;
}MU_FOLDERDATA,*PMU_FOLDERDATA;

extern MU_FOLDERCHAINNODE* Folder_New(void);
extern BOOL Folder_Fill(MU_FOLDERCHAINNODE* pChainNode,MU_FOLDERINFO FolderInfo,DWORD dwoffset);
extern int  Folder_Insert(MU_FOLDERCHAINNODE** ppHeader,MU_FOLDERCHAINNODE* pChainNode);
extern void Folder_Delete(MU_FOLDERCHAINNODE** ppHeader,MU_FOLDERCHAINNODE* pChainNode);
extern void Folder_Erase(MU_FOLDERCHAINNODE** ppHeader);

extern DWORD MessageUnibox_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
extern BOOL GetTimeDisplay (SYSTEMTIME st, char* cTime, char* cDate);
extern BOOL MU_CanMoveToFolder(void);
extern BOOL MU_CreateList(HWND hParent,void* pFolderTypeOrName,BOOL bEmailBox);
extern int GetSIMState();
extern void SMS_PleaseToWait(void);
extern void SendMuChildMessage(int msg, WPARAM wParam, LPARAM lParam);
extern BOOL MU_GetName(char* szName, const char* pAddr);
extern BOOL APP_CallPhoneNumber (const char * pPhoneNumber);
extern BOOL SMS_IsInitOK(void);
extern HWND hMuFrame;
extern BOOL IsUserCancel(void);
extern BOOL mms_can_forward(DWORD handle);
BOOL MU_IsUserCancel(HWND hWnd,UINT uMsgCmd);
extern BOOL SMS_Opening(void);
extern int MMS_GetRecNum(HWND hwndmu, DWORD msghandle);
extern int mail_GetRecNum(HWND hwndmu, DWORD msghandle);
extern LONG APP_GetSMSSize(void);
extern LONG APP_GetMMSSize(void);
extern LONG APP_GetEmailSize(void);
extern LONG GetBtAllMsgSize(void);

extern LONG APP_GetMsgSize(void);
extern void WaitWinDepExtOp(HWND hParent, BOOL bShow, PCSTR cPrompt, PCSTR szCaption,PCSTR szOk,
                            PCSTR szCancel, UINT rMsg, UINT uTimer);

#endif // _MU_GLOBAL_H
