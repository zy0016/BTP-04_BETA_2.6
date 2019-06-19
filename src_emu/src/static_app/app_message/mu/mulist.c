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
#include "muglobal.h"

#define WM_SELECTFOLDER         WM_USER+100
#define WM_ALLMOVE				WM_USER+101
#define WM_DELSELECT			WM_USER+102
#define WM_DELALL				WM_USER+103
#define WM_DELONE				WM_USER+104
#define WM_DIRECTEXIT           WM_USER+105
#define WM_CALLPHONE            WM_USER+106

#define IDC_OK          100
#define IDC_EXIT        200
#define IDC_MULSIT_LIST 300
#define IDC_SELDEL_LIST	400

#define IDM_FOLDER_OPEN         101
#define IDM_FOLDER_CREATEMSG_SMS    1021
#define IDM_FOLDER_CREATEMSG_MMS    1022
#define IDM_FOLDER_CREATEMSG_EMAIL  1023
#define IDM_FOLDER_REPLY_SENDER     1031
#define IDM_FOLDER_REPLY_ALL        1032
#define IDM_FOLDER_DELETE       104
#define IDM_FOLDER_MSGDETAIL    105
#define IDM_FOLDER_MOVE         106
#define IDM_FOLDER_DEFER        107
#define IDM_FOLDER_SENDING      108
#define IDM_FOLDER_CALL         109
#define IDM_FOLDER_COPY         110
#define IDM_FOLDER_NEWFOLDER    111
#define IDM_FOLDER_RENAMEFOLDER 112
#define IDM_FOLDER_DELETE_SELECT    1131
#define IDM_FOLDER_DELETE_ALL       1132
#define IDM_FOLDER_FORWARD      114
#define IDM_FOLDER_MOVE_SELECT      1151
#define IDM_FOLDER_MOVE_ALL         1152
#define IDM_FOLDER_SUSPEND      116
#define IDM_FOLDER_REMOVEALL    117
#define IDM_FOLDER_REPLY        118

static HWND hListWnd = NULL;
//static HWND hFolderListWnd = NULL;
static HBITMAP hBmp = NULL;
extern HINSTANCE   hMUInstance;

typedef enum
{   
    GIF_MU_SMS_READ = 0,
    GIF_MU_SMS_UNREAD, 
	GIF_MU_MMS_READ,
	GIF_MU_MMS_UNREAD,
	GIF_MU_BT_READ,
	GIF_MU_BT_UNREAD,
	GIF_MU_EMAIL_READ,
	GIF_MU_EMAIL_UNREAD,
    GIF_MU_FOLDER, 
    GIF_MU_DRAFT, 
    GIF_MU_SENT, 
    GIF_MU_SENDING,
    GIF_MU_SUPU,
    GIF_MU_FAIL,
    GIF_MU_DR,
	GIF_MU_REPORT_PENDING,
	GIF_MU_REPORT_DELIEVERED,
	GIF_MU_REPORT_FAILED,
    GIF_MU_MAX,
};
static HBITMAP mu_hBmp[GIF_MU_MAX];

const char* const mu_PathGif[] =
{
    "/rom/message/unibox/sms_read.bmp",
    "/rom/message/unibox/sms_unre.bmp",
	"/rom/message/unibox/mms_read.bmp",
    "/rom/message/unibox/mms_unre.bmp",
	"/rom/message/unibox/mu_btmessageread.bmp",
    "/rom/message/unibox/mu_btmessageunread.bmp",
	"/rom/message/unibox/email_read.bmp",
    "/rom/message/unibox/email_unre.bmp",
    "/rom/message/unibox/mu_myfolders.bmp",
    "/rom/message/unibox/mu_draft.bmp",
    "/rom/message/unibox/mu_sent.bmp",
    "/rom/message/unibox/mu_out.bmp",
    "/rom/message/unibox/mu_out.bmp",
    "/rom/message/unibox/mu_failed.bmp",
    "/rom/message/unibox/mu_draft.bmp",
	"/rom/message/unibox/msging_i_pending_22x16.bmp",
    "/rom/message/unibox/msging_i_delivered_22x16.bmp",
    "/rom/message/unibox/msging_i_failed_22x16.bmp",
};

extern MU_INITSTATUS   mu_initstatus;
extern MU_IEmailbox    *mu_emailbox_interface;
extern MU_IMessage *mu_mms_interface;
extern MU_IMessage *mu_sms_interface;
extern MU_IMessage *mu_push_interface;
extern MU_IMessage *mu_email_interface;
extern MU_ISMSEx   *mu_smsex_interface;
extern MU_IMessage *mu_file_interface;
extern MU_IMessage *mu_bt_interface;

extern BOOL bDirectExit;

static void MU_SelectMsg(HWND hWndOwner, BOOL bMove, BOOL bDel);
static void SelDel_OnCreate(HWND hWnd, PMU_SELLISTDATA pSelListData);
static void SelDel_OnCommand(HWND hWnd, int id, UINT codeNotify);
LRESULT MUSelDelWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void SelDel_OnKeyDown(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static int MU_DelOneMsg(HWND hWnd, PMU_CHAINNODE pChainNode);
static int MU_DelMultiOneMsg(HWND hWnd, PMU_CHAINNODE pChainNode);
static void SelDelConfirm(HWND hWnd);
static void SelMoveConfirm(HWND hWnd);
static void SelMoveToFolder(HWND hWnd,BOOL bMove,int nNewFolder);
void MoveAllMessages(HWND hLst, BOOL bMove, int nNewFolder);
static int Mu_GetIndexInList(HWND hWnd, PMU_CHAINNODE pChain);

BOOL MU_CreateList(HWND hParent,void* pFolderTypeOrName,BOOL bEmailBox);
LRESULT MUListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MUList_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MUList_OnActivate(HWND hwnd, UINT state);
static void MUList_OnInitmenu(HWND hwnd);
static void MUList_OnPaint(HWND hWnd);
static void MUList_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MUList_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void MUList_OnDestroy(HWND hWnd);
static void MUList_OnClose(HWND hWnd);

static PCSTR MU_GetCurCaption(PMU_LISTCREATEDATA CreateData);
static void On_mu_getmsgs_resp(HWND hWnd, int nMduType, int nErrType);
static void On_mu_deleted_resp(HWND hWnd,int nMduType,int nErrType, DWORD handle);
static BOOL On_mu_delete(HWND hWnd,int nMduType,int nErrType, DWORD handle);
static void On_mu_modified(HWND hWnd,int nMduType, int ErrType, MU_MsgNode *pMsgNode);
static void On_mu_detall_resp(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void On_mu_newmtmsg(HWND hWnd, int nMduType, int ErrType, MU_MsgNode *pMsgNode);
static void On_mu_daemon(HWND hWnd, int nMduType, int nErrType, MU_Daemon_Notify* daemon);
static void On_mu_movetofolder(HWND hWnd,BOOL bMove,int nNewFolder);
static HWND MU_GetListWnd(void);
static void MU_AddNodeToList(HWND hWnd,MU_MsgNode *pmsgnode,MU_CHAINNODE *pNewNode,int nInsert,SYSTEMTIME syLocal);
int  MU_BuildChain(PMU_CHAINNODE *ppFirst,MU_MsgNode *pmsgnode, PMU_CHAINNODE* ppNewNode);
static LONG MU_GetSUM(PMU_LISTCREATEDATA pCreateData);
static void MU_SetSoftLeft(HWND hWnd);
//static BOOL MU_PhraseLoad(hWnd);
//static BOOL MU_FolderLoad(hWnd);
BOOL CreateSubFolderWnd(HWND hParent,MU_FOLDERINFO *pFolderInfo);
static int FillOrderList(HWND hWnd, DWORD** ppHandleList);
static int GetPosInfo(DWORD* pList, int nCount, DWORD key, BOOL* bPre, BOOL* bNext);
static void On_mu_goto_another(HWND hWnd, BOOL bPre, BOOL bNext);
static BOOL MUListReset(HWND hWnd);
static DWORD* GetPosInArray(DWORD pNode,DWORD* pHandleList, int nCount);

static MU_CHAINNODE* MU_NewNode(void);
static BOOL MU_FillNode(MU_CHAINNODE* pChainNode,MU_MsgNode* pMsgNode);
static int  MU_InsetNode(PMU_CHAINNODE* ppFirst,MU_CHAINNODE* pChainNode,CMPFUNC pCmpFunc);
void MU_DeleteNode(PMU_CHAINNODE *ppFirst,MU_CHAINNODE* pChainNode);
void MU_Erase(PMU_CHAINNODE* ppFirst);

BOOL    MU_IsWindow(void);
void    APP_SHOWINBOX(void);
int     MU_GetCurFolderType(void);
const char* MU_GetCurEmailboxName(void);
BOOL MU_GetFolderInfo(PMU_FOLDERINFO pInfo,int* pnCount);
BOOL MU_Compare_Name(DWORD Name1, DWORD Name2);
BOOL MU_Compare_DateTime(DWORD DateTime1, DWORD DateTime2);


CMPFUNC gCmpFunc[] = 
{
    MU_Compare_Name,
    MU_Compare_DateTime,
    NULL,
};

static DWORD*  pHandleList = NULL;
static int   nMsgPos = 0;
static int   nMsgCount = 0;

extern BOOL MU_GetFolderInfo(PMU_FOLDERINFO pInfo,int* pnCount);
/*
extern BOOL mu_file_get(void);
extern BOOL mu_file_get_one(MU_MsgNode *msgnode);
extern BOOL mu_file_release(void);
extern BOOL mu_file_rename(HWND hParent,DWORD handle);
extern BOOL mu_file_new(HWND hParent);
extern BOOL mu_file_delete(HWND hParent,DWORD handle);*/

/*********************************************************************\
* Function	   MU_CreateList
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MU_CreateList(HWND hParent,void* pFolderTypeOrName,BOOL bEmailBox)
{
    MU_LISTCREATEDATA CreateData;
    HMENU hMenu;
	RECT rClient;

    if(!pFolderTypeOrName)
        return FALSE;

    if (IsWindow(hListWnd))
	{
        PMU_LISTCREATEDATA p;
        p = (PMU_LISTCREATEDATA)GetUserData(hListWnd);
       
        if(bEmailBox == p->bEmailBox)
        {
            if(bEmailBox == TRUE)
            {
                if(p->szEmailName && pFolderTypeOrName)
                {
                    if(strcmp(p->szEmailName,pFolderTypeOrName) != 0)
                        p->bChangeMenu = TRUE;
                    //refresh need memset mu_counter
                }
                else
                {
                    p->bChangeMenu = TRUE;//refresh
                }
            }
            else
            {
        if(*(int*)pFolderTypeOrName != (int)(p->nBoxType))
            p->bChangeMenu = TRUE;//refresh
            }
        }
        else
        {
            p->bChangeMenu = TRUE;//refresh
        }
       
		ShowWindow(hListWnd, SW_SHOW);
		UpdateWindow(hListWnd);
		BringWindowToTop(hListWnd);

        return TRUE;
    }
    
    memset(&CreateData,0,sizeof(MU_LISTCREATEDATA));
    CreateData.bChangeMenu = TRUE;
    CreateData.bEmailBox = bEmailBox;
    CreateData.pFirst = NULL;
    CreateData.pUnReadFirst = NULL;
    memset(&(CreateData.mu_counter),0,sizeof(MU_COUNTER));
    
    if(bEmailBox)
    {
        CreateData.nBoxType = -1;
        if(strlen(pFolderTypeOrName) >= MU_EMAILBOX_NAME_LEN)
        {
            strncpy(CreateData.szEmailName,pFolderTypeOrName,MU_EMAILBOX_NAME_LEN);
            CreateData.szEmailName[MU_EMAILBOX_NAME_LEN-1] = 0;
        }
        else
        {
            strcpy(CreateData.szEmailName,pFolderTypeOrName);
        }
    }
    else
    {
    CreateData.nBoxType = *((int*)pFolderTypeOrName);
    CreateData.szEmailName[0] = 0;
    }

    hMenu = CreateMenu();
	CreateData.hMenu = hMenu;
	
    GetClientRect(hMuFrame, &rClient);

    hListWnd = CreateWindow(
        "MUListWndClass",
        "",//MU_GetCurCaption(CreateData), 
        WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hMuFrame,
        NULL, 
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!hListWnd)
        return FALSE;

    PDASetMenu(hMuFrame, hMenu);
	SetWindowText(hMuFrame, MU_GetCurCaption(&CreateData));
    return TRUE;
}
/*********************************************************************\
* Function	MUListWndProc
* Purpose   List window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
LRESULT MUListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;


	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MUList_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
//        MUList_OnActivate(hWnd,(UINT)LOWORD(wParam));
		{
			MU_LISTCREATEDATA* pData;

			pData = (MU_LISTCREATEDATA*)GetUserData(hWnd);

			MU_SetSoftLeft(hWnd);
            
            SendMessage(hMuFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
            SendMessage(hMuFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

			SetFocus(GetDlgItem(hWnd, IDC_MULSIT_LIST));
			SetWindowText(hMuFrame, MU_GetCurCaption(pData));
			PDASetMenu(hMuFrame, pData->hMenu);
		}
        break;
        
    case WM_INITMENU:
        MUList_OnInitmenu(hWnd);
        break;

    case WM_PAINT:
        MUList_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        MUList_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        MUList_OnCommand(hWnd,(int)LOWORD(wParam),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
		SendMessage(hMuFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
        MUList_OnClose(hWnd);
        break;

    case WM_DIRECTEXIT:
        DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hMUInstance);
        DestroyWindow(hMuFrame);
        break;

    case WM_DESTROY:
		{
			MU_LISTCREATEDATA* pData;

			pData = (MU_LISTCREATEDATA*)GetUserData(hWnd);

			DestroyMenu(pData->hMenu);
		}
		//bDirectExit = FALSE;
        MUList_OnDestroy(hWnd);
        break;
        
    case PWM_MSG_MU_GETMSGS_RESP:
        On_mu_getmsgs_resp(hWnd, (int)HIWORD(wParam), (int)LOWORD(wParam));
        break;
        
    case PWM_MSG_MU_DELETED_RESP:
        On_mu_deleted_resp(hWnd, (int)HIWORD(wParam), (int)LOWORD(wParam), (DWORD)lParam);
        break;

    case PWM_MSG_MU_DELMULTI_RESP:
        break;
                
    case PWM_MSG_MU_DELALL_RESP:
        On_mu_detall_resp(hWnd, wParam, lParam);
        break;
        
    case PWM_MSG_MU_MODIFIED:
        On_mu_modified(hWnd, (int)HIWORD(wParam), (int)LOWORD(wParam), (MU_MsgNode*)lParam);
        break;

    case PWM_MSG_MU_NEWMTMSG:
        On_mu_newmtmsg(hWnd, (int)HIWORD(wParam), (int)LOWORD(wParam), (MU_MsgNode*)lParam);
        break;
        
    case PWM_MSG_MU_DAEMON:
        On_mu_daemon(hWnd,(int)HIWORD(wParam), (int)LOWORD(wParam), (MU_Daemon_Notify*)lParam);
        break;
        
    case PWM_MSG_MU_PICKUP:
        break;

    case PWM_MSG_MU_MAILBOX_REFRESH:
        break;

    case PWM_MSG_MU_DELETE:
        lResult = On_mu_delete(hWnd, (int)HIWORD(wParam), (int)LOWORD(wParam), (DWORD)lParam);
        break;
        
    case WM_SELECTFOLDER:
        On_mu_movetofolder(hWnd,(BOOL)wParam,(int)lParam);
        break;

	case PWM_MSG_MU_NEXT:
		On_mu_goto_another(hWnd, FALSE, TRUE);
		break;

	case PWM_MSG_MU_PRE:
		On_mu_goto_another(hWnd, TRUE, FALSE);
		break;

    case WM_ALLMOVE:
		{
			MoveAllMessages(GetDlgItem(hWnd, IDC_MULSIT_LIST), (BOOL)wParam, (int)lParam);
		}
		break;

	case WM_DELALL:
		{
			char    pCaption[64];
			PMU_LISTCREATEDATA p;

			
			GetWindowText(hMuFrame, pCaption, 63);
			if(lParam == 1)
			{
				p = GetUserData(hWnd);
				
				WaitWinDepExtOp(hWnd, TRUE, (char*)IDS_DELETEING, pCaption, "", (char*)IDS_CANCEL,0,0);
				
				if(p->bEmailBox)
				{
					
				}
				else
				{
					if(mu_initstatus.bSMS)
					{
						mu_sms_interface->msg_delete_all_messages(hWnd, p->nBoxType, MU_DELALL_FILE);
					}

					if(IsUserCancel())
						goto error_handle;

					if(mu_initstatus.bMMS)
					{
						mu_mms_interface->msg_delete_all_messages(hWnd, p->nBoxType, MU_DELALL_FILE);
					}

					if(IsUserCancel())
						goto error_handle;

					if(mu_initstatus.bEmail)
					{
						mu_email_interface->msg_delete_all_messages(hWnd, p->nBoxType, MU_DELALL_FILE);
					}

					if(IsUserCancel())
						goto error_handle;
					
					if(mu_initstatus.bBT)
					{
						mu_bt_interface->msg_delete_all_messages(hWnd, p->nBoxType, MU_DELALL_FILE);
					}

					SendMessage(GetDlgItem(hWnd, IDC_MULSIT_LIST), LB_RESETCONTENT, 0, 0);
					memset(&p->mu_counter, 0, sizeof(MU_COUNTER));
				}
				
				WaitWinDepExtOp(hWnd, FALSE, (char*)IDS_DELETEING, pCaption, "", (char*)IDS_CANCEL,0,0);
				PLXTipsWin(hMuFrame, NULL, 0, (char*)IDS_DELETED, pCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);

				break;

error_handle:
				WaitWinDepExtOp(hWnd, FALSE, (char*)IDS_DELETEING, pCaption, "", (char*)IDS_CANCEL,0,0);
				MUListReset(hWnd);
				break;
				
			}
		}
		break;

	case WM_DELONE:
		{
			char    pCaption[64];
            PMU_CHAINNODE pNode = NULL;
            PMU_LISTCREATEDATA pCreateData;

            pCreateData = GetUserData(hWnd);

            pNode = pCreateData->pSelect;

            pCreateData->pSelect = NULL;
			
			GetWindowText(hMuFrame, pCaption, 63);

			if(lParam == 1)
			{    
                if(pNode == NULL)
                    break;
				
				if(MU_DelOneMsg(hWnd, pNode))
                    PLXTipsWin(hMuFrame, NULL, 0, (char*)IDS_DELETED, pCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);
			}
			
		}
		break;
        
    case WM_CALLPHONE:
        {
            MU_CHAINNODE* pNode = NULL;

            PMU_LISTCREATEDATA pCreateData;
            
            pCreateData = GetUserData(hWnd);

            pNode = pCreateData->pSelect;
            
            pCreateData->pSelect = NULL;

            if(lParam == 1)
            {       
                if(pCreateData->nBoxType == MU_INBOX && MU_GetSUM(pCreateData) > 0)
                {
                    if(pNode == NULL)
                        break;

                    if(pNode->msgtype != MU_MSG_SMS
                        && pNode->msgtype != MU_MSG_MMS
                        && pNode->msgtype != MU_MSG_VCAL
                        && pNode->msgtype != MU_MSG_VCARD)
                        break;

                    APP_CallPhoneNumber(pNode->addr);
                }
            }
        }
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	MUList_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MUList_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
//    int  nErrno;
    MU_LISTCREATEDATA *pData;
    HWND hList;
    int i;
	HDC	hdc;
//	COLORREF Color;
// 	BOOL bTran;

    SendMessage(hMuFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT, 0), (LPARAM)IDS_BACK);
    SendMessage(hMuFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)"");
    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
	SendMessage(hMuFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hMuFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    pData = (MU_LISTCREATEDATA*)GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(MU_LISTCREATEDATA));

    MM_Construct();

    hdc = GetDC(hWnd);
    for(i = GIF_MU_SMS_READ ; i < GIF_MU_MAX ; i++)
        mu_hBmp[i] = LoadImage(NULL, mu_PathGif[i], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);//CreateBitmapFromImageFile(hdc, mu_PathGif[i], &Color, &bTran);
    ReleaseDC(hWnd, hdc);

    GetClientRect(hWnd,&rect);
    
    hList = CreateWindow(
        "LISTBOX", 
        "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP | LBS_MULTILINE,
        0,
        0,
        rect.right-rect.left,
        rect.bottom-rect.top, 
        hWnd, 
        (HMENU)IDC_MULSIT_LIST, 
        NULL, 
        NULL);
    
    if(hList == NULL)
        return FALSE;

	SetFocus(hList);

    {   
        hBmp = LoadImage(NULL, "/rom/message/unibox/mail_aff.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    }
	
    if(pData->bEmailBox)
    {
//        mu_emailbox_interface->email_get_messages(hWnd,pData->szEmailName);
    }
    else
    {
        /*if(pData->nBoxType == MU_MYFOLDER)
        {
            MU_PhraseLoad(hWnd);
            MU_FolderLoad(hWnd);
        }*/
    
        if(mu_initstatus.bMMS)
        {
			printf("mu_mms_interface->msg_get_messages\r\n");
            mu_mms_interface->msg_get_messages(hWnd, pData->nBoxType);
        }
    
        if(mu_initstatus.bSMS)
        {
			printf("mu_sms_interface->msg_get_messages\r\n");
            mu_sms_interface->msg_get_messages(hWnd, pData->nBoxType);
        }
    
        if(mu_initstatus.bEmail)
        {
			printf("mu_email_interface->msg_get_messages\r\n");
            mu_email_interface->msg_get_messages(hWnd, pData->nBoxType);
        }
    
        if(mu_initstatus.bPush)
        {
			printf("mu_push_interface->msg_get_messages\r\n");
            mu_push_interface->msg_get_messages(hWnd, pData->nBoxType);
        }

		if(mu_initstatus.bBT)
		{
			printf("mu_bt_interface->msg_get_messages\r\n");
			mu_bt_interface->msg_get_messages(hWnd, pData->nBoxType);
		}
    }
    
//    SendMessage(hMuFrame, PWM_SETAPPICON, 0, (LPARAM)ICON_MU_MAIN); // need modify
    printf("OK, List created!!!\r\n");
    return TRUE;
    
}
/*********************************************************************\
* Function	MUList_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MUList_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;

    if(state == WA_ACTIVE)
    {
        hLst = GetDlgItem(hwnd,IDC_MULSIT_LIST);
        SetFocus(hLst);
    }

    return;
}
/*********************************************************************\
* Function	MUList_OnInitmenu
* Purpose   WM_INITMENU message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUList_OnInitmenu(HWND hWnd)
{
    PMU_LISTCREATEDATA pCreateData;
    HMENU   hMenu,hSubReplyMenu,hSubDeleteMenu,hSubMoveMenu;
    int     nMenuItem,nIndex;
    HWND    hLst;
    PMU_CHAINNODE pNode;

    pCreateData = GetUserData(hWnd);
    hMenu = PDAGetMenu(hMuFrame);

//    if(pCreateData->nBoxType != MU_MYFOLDER && !pCreateData->bChangeMenu)
//         return;

    pCreateData->bChangeMenu = FALSE;

    nMenuItem = GetMenuItemCount(hMenu);
    while(nMenuItem-- > 0)
        DeleteMenu(hMenu,nMenuItem,MF_BYPOSITION);
    
    if(pCreateData->bEmailBox)
    {
        ;//
    }
    else
    {
        switch(pCreateData->nBoxType)
        {
        case MU_INBOX:
            if(MU_GetSUM(pCreateData) > 0)
            {
                hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
                if(nIndex == LB_ERR)
                    return;
                pNode = (MU_CHAINNODE*)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);

				if(pNode->msgtype == MU_MSG_MMS && pNode->status == MU_STU_RECEIVING)
				{
					AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_MSGDETAIL, IDS_DETAIL);
					break;					
				}
                
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_OPEN, IDS_OPEN);
				
				if(pNode->msgtype != MU_MSG_BT
                    &&pNode->msgtype != MU_MSG_VCARD
                    &&pNode->msgtype != MU_MSG_VCAL
					&&pNode->msgtype != MU_MSG_BT_VCARD
					&&pNode->msgtype != MU_MSG_BT_VCAL
					&&pNode->msgtype != MU_MSG_BT_NOTEPAD
					&&pNode->msgtype != MU_MSG_BT_PICTURE)
                {
                    if(pNode->status == MU_STU_UNREAD 
                        || pNode->status == MU_STU_READ)
					AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_REPLY, IDS_REPLY);
                }
				
                if(pNode->msgtype == MU_MSG_MMS)//&& multi recipient
                {		
                    if(pNode->status == MU_STU_UNREAD 
                        || pNode->status == MU_STU_READ)
                    {
					if(MMS_GetRecNum(hWnd, pNode->handle) > 1)
					{
						hSubReplyMenu = CreateMenu();
						AppendMenu(hSubReplyMenu,MF_ENABLED, IDM_FOLDER_REPLY_SENDER, IDS_TOSENDER);
						AppendMenu(hSubReplyMenu,MF_ENABLED, IDM_FOLDER_REPLY_ALL, IDS_TOALL);//need condition
						AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hSubReplyMenu, IDS_REPLY);
					}
                    }
					
                }

//				if(pNode->msgtype == MU_MSG_EMAIL)//&& multi recipient
//				{
//					hSubReplyMenu = CreateMenu();
//					AppendMenu(hSubReplyMenu,MF_ENABLED, IDM_FOLDER_REPLY_SENDER, IDS_TOSENDER);
//					
//					if(mail_GetRecNum(hWnd, pNode->handle) > 1)
//					{
//						AppendMenu(hSubReplyMenu,MF_ENABLED, IDM_FOLDER_REPLY_ALL, IDS_TOALL);//need condition
//						
//						AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hSubReplyMenu, IDS_REPLY);
//					}
//					
//				}                

				

				if(pNode->msgtype == MU_MSG_SMS
					|| pNode->msgtype == MU_MSG_STATUS_REPORT
					|| pNode->msgtype == MU_MSG_EMAIL
					|| pNode->msgtype == MU_MSG_EMAILHEADER
					|| (pNode->msgtype == MU_MSG_MMS && mms_can_forward(pNode->handle)))
                {
                    if(pNode->status != MU_STU_UNRECEIVED)
                    AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_FORWARD, IDS_FORWARD);
                }
                
                if(MU_CanMoveToFolder() == TRUE)
                {
                    AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_MOVE, IDS_MOVETOFOLDER);
                    hSubMoveMenu = CreateMenu();
                    AppendMenu(hSubMoveMenu,MF_ENABLED, IDM_FOLDER_MOVE_SELECT, IDS_SELECT);
                    AppendMenu(hSubMoveMenu,MF_ENABLED, IDM_FOLDER_MOVE_ALL, IDS_ALL);
                    AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hSubMoveMenu, IDS_MOVEMANY);
                }
                
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_MSGDETAIL, IDS_DETAIL);
                
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_DELETE, IDS_DELETE);
                hSubDeleteMenu = CreateMenu();
                AppendMenu(hSubDeleteMenu,MF_ENABLED, IDM_FOLDER_DELETE_SELECT, IDS_SELECT);
                AppendMenu(hSubDeleteMenu,MF_ENABLED, IDM_FOLDER_DELETE_ALL, IDS_ALL);
                AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hSubDeleteMenu, IDS_DELETEMANY);
            }
            break;
            
        case MU_DRAFT:
            if(MU_GetSUM(pCreateData) > 0)
            {
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_OPEN, IDS_EDIT);
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_MSGDETAIL, IDS_MSGINFO);
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_DELETE, IDS_DELETE);
                hSubDeleteMenu = CreateMenu();
                AppendMenu(hSubDeleteMenu,MF_ENABLED, IDM_FOLDER_DELETE_SELECT, IDS_SELECT);
                AppendMenu(hSubDeleteMenu,MF_ENABLED, IDM_FOLDER_DELETE_ALL, IDS_ALL);
                AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hSubDeleteMenu, IDS_DELETEMANY);
            }
            break;
            
        case MU_SENT:
            if(MU_GetSUM(pCreateData) > 0)
            {
                hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
                if(nIndex == LB_ERR)
                    return;
                pNode = (MU_CHAINNODE*)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_OPEN, IDS_OPEN);
                if(pNode->msgtype == MU_MSG_SMS
					|| pNode->msgtype == MU_MSG_STATUS_REPORT
					|| pNode->msgtype == MU_MSG_EMAIL
					|| pNode->msgtype == MU_MSG_EMAILHEADER
					|| (pNode->msgtype == MU_MSG_MMS && mms_can_forward(pNode->handle)))
                    AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_FORWARD, IDS_FORWARD);
                //AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_FORWARD, IDS_FORWARD);
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_MOVE, IDS_MOVETOFOLDER);
                hSubMoveMenu = CreateMenu();
                AppendMenu(hSubMoveMenu,MF_ENABLED, IDM_FOLDER_MOVE_SELECT, IDS_SELECT);
                AppendMenu(hSubMoveMenu,MF_ENABLED, IDM_FOLDER_MOVE_ALL, IDS_ALL);
                AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hSubMoveMenu, IDS_MOVEMANY);
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_MSGDETAIL, IDS_MSGINFO);
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_DELETE, IDS_DELETE);
                hSubDeleteMenu = CreateMenu();
                AppendMenu(hSubDeleteMenu,MF_ENABLED, IDM_FOLDER_DELETE_SELECT, IDS_SELECT);
                AppendMenu(hSubDeleteMenu,MF_ENABLED, IDM_FOLDER_DELETE_ALL, IDS_ALL);
                AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hSubDeleteMenu, IDS_DELETEMANY);
            }
            break;
            
        case MU_OUTBOX:
            if(MU_GetSUM(pCreateData) > 0)
            {
                hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
                if(nIndex == LB_ERR)
                    return;
                pNode = (MU_CHAINNODE*)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
                
                if(pNode->status == MU_STU_UNSENT || pNode->status == MU_STU_SUSPENDED)
                {
                    AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_SENDING, IDS_SEND);
					AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_MOVE, IDS_MOVETODRAFTS);
                    AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_DELETE, IDS_DELETE);
                }
                
                if(pNode->status == MU_STU_WAITINGSEND || pNode->status == MU_STU_DEFERMENT)
                {
                    AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_SUSPEND, IDS_SUSPEND);
                }
               
				 AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_MSGDETAIL, IDS_MSGINFO);
            }
            break;
            
        case MU_REPORT:
            if(MU_GetSUM(pCreateData) > 0)
            {   
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_OPEN, IDS_OPEN);   
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_DELETE_ALL, IDS_REMOVEALL);                
            }
            break;
            
        default:
            if(MU_GetSUM(pCreateData) > 0)
            {       
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_OPEN, IDS_OPEN);
                
                if(MU_CanMoveToFolder() == TRUE)
                {
                    AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_MOVE, IDS_MOVETOFOLDER);
                    hSubMoveMenu = CreateMenu();
                    AppendMenu(hSubMoveMenu,MF_ENABLED, IDM_FOLDER_MOVE_SELECT, IDS_SELECT);
                    AppendMenu(hSubMoveMenu,MF_ENABLED, IDM_FOLDER_MOVE_ALL, IDS_ALL);
                    AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hSubMoveMenu, IDS_MOVEMANY);
                }
                
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_MSGDETAIL, IDS_DETAIL);
                
                AppendMenu(hMenu,MF_ENABLED, IDM_FOLDER_DELETE, IDS_DELETE);
                hSubDeleteMenu = CreateMenu();
                AppendMenu(hSubDeleteMenu,MF_ENABLED, IDM_FOLDER_DELETE_SELECT, IDS_SELECT);
                AppendMenu(hSubDeleteMenu,MF_ENABLED, IDM_FOLDER_DELETE_ALL, IDS_ALL);
                AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hSubDeleteMenu, IDS_DELETEMANY); 
            }
            break;
        }
    }
    
    return;

}
/*********************************************************************\
* Function	MUList_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUList_OnPaint(HWND hWnd)
{
    int nSum = 0;
    PMU_LISTCREATEDATA pCreateData;
    HWND hLst = NULL;
    HDC hdc = BeginPaint(hWnd, NULL);
	COLORREF OldClr;

	OldClr = SetBkColor(hdc, COLOR_TRANSBK);

    pCreateData = GetUserData(hWnd);
    hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);

    if(pCreateData->bEmailBox)
        nSum = pCreateData->mu_counter.nEmail + pCreateData->mu_counter.nEmailHeader;
    else
        nSum = pCreateData->mu_counter.nEmail + pCreateData->mu_counter.nMMS
        + pCreateData->mu_counter.nSMS + pCreateData->mu_counter.nPush 
		+ pCreateData->mu_counter.nBT;

    if(nSum == 0)
    {
        RECT rcClient;
        GetClientRect(hWnd,&rcClient);
        if(IsWindowVisible(hLst) == TRUE)
            ShowWindow(hLst,SW_HIDE);
		if(pCreateData->nBoxType == MU_REPORT)
			DrawText(hdc,IDS_NO_REPORTS,-1,&rcClient,DT_CLEAR|DT_VCENTER|DT_CENTER);
		else
			DrawText(hdc,IDS_NOMESSAGES,-1,&rcClient,DT_CLEAR|DT_VCENTER|DT_CENTER);
    }

	SetBkColor(hdc, OldClr);
	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	MUList_OnKey
* Purpose   WM_KEYDOWN message handler of the main window
* Params
*			hWnd: Handle of the window
*			vk:	Virtual key code
*			fDown: Is key donw
*			cRepeat: Key repeat rate
*			flags:	flag of key down
* Return	None
* Remarks
**********************************************************************/
static void MUList_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    int nMenuItem = 0;
   
    switch (vk)
	{
	case VK_F1:
		{
			HWND hLst;
			int index;
			MU_CHAINNODE* pNode = NULL;
			PMU_LISTCREATEDATA pCreateData;
			char  number[128];

			pCreateData = GetUserData(hWnd);

			if(pCreateData->nBoxType == MU_INBOX && MU_GetSUM(pCreateData) > 0)
			{
				//
				hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);

				index = SendMessage(hLst,LB_GETCURSEL,0,0);

				if(index == LB_ERR)
                        break;
				else
				{
					pNode = (MU_CHAINNODE*)SendMessage(hLst, LB_GETITEMDATA, index, 0);              
					
					if(pNode->msgtype != MU_MSG_SMS
						&&pNode->msgtype != MU_MSG_MMS
						&&pNode->msgtype != MU_MSG_VCAL
						&&pNode->msgtype != MU_MSG_VCARD)
					{
						break;
					}

					sprintf(number,"%s%s%s",IDS_DAIL_TO, "\r\n", pNode->addr);
					
                    pCreateData->pSelect = pNode;

                    if(pCreateData->pSelect == NULL)
                        break;

                    PLXConfirmWinEx(hMuFrame, hWnd, number, Notify_Request, "", 
                        (char*)IDS_YES, (char*)IDS_NO, WM_CALLPHONE);
					
					break;
				}
			}
			
		}
		break;

	case VK_F10:
		if(!bDirectExit)
			PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		else
            PostMessage(hWnd,WM_DIRECTEXIT,NULL,NULL);
		break;

    case VK_F5:
        {
            MU_LISTCREATEDATA* pCreateData;
            
            pCreateData = GetUserData(hWnd);

            if(MU_GetSUM(pCreateData) > 0)
                PDADefWindowProc(hMuFrame, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        }
        break;

    case VK_RETURN:
        {
            HWND hLst = NULL;
            MU_LISTCREATEDATA* pCreateData;
            int index;
            MU_CHAINNODE* pNode = NULL; 

            pCreateData = GetUserData(hWnd);
            
            if(pCreateData->bEmailBox == FALSE)
            {                
                switch(pCreateData->nBoxType)
                {
                case MU_OUTBOX:
                    hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                    index = SendMessage(hLst,LB_GETCURSEL,0,0);
                    if(index == LB_ERR)
                        break;
                    else
                    {
                        pNode = (MU_CHAINNODE*)SendMessage(hLst, LB_GETITEMDATA, index, 0);              
                        
                        switch(pNode->status) 
                        {
                        case MU_STU_WAITINGSEND:
						case MU_STU_DEFERMENT:
                            SendMessage(hWnd, WM_COMMAND, IDM_FOLDER_SUSPEND, NULL);
                            break;
                        case MU_STU_SENDING:
                            break;
                            
                        case MU_STU_UNSENT:
                        case MU_STU_SUSPENDED:
                            SendMessage(hWnd, WM_COMMAND, IDM_FOLDER_SENDING, NULL);
                            break;
                            
                        default:
                            break;
                        }     
                    }
                    break;
                    
                case MU_DRAFT:
                case MU_INBOX:
                case MU_REPORT:
                case MU_SENT:
                default:
                    if(MU_GetSUM(pCreateData) == 0 )
                        break;
                    else
                        SendMessage(hWnd, WM_COMMAND, IDM_FOLDER_OPEN, NULL);
                    break;
                }
            }
        }
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	MUList_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MUList_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    PMU_CHAINNODE pChainNode;
    HWND    hLst;
    int     nCurSel;
    PMU_LISTCREATEDATA p;
    int     nFolder;
	BOOL    bPre, bNext;

    hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);

	switch(id)
	{	
	case IDC_EXIT:
        PostMessage(hWnd,WM_CLOSE,0,0);
        break;

    case IDM_FOLDER_OPEN:
        nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
        if( nCurSel == LB_ERR )
            return;
        
        pChainNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
        if( !pChainNode )
            return;

		nMsgCount = FillOrderList(hWnd, &pHandleList);
		
		nMsgPos = GetPosInfo(pHandleList, nMsgCount, (DWORD)pChainNode, &bPre, &bNext);
		
        switch(pChainNode->msgtype) 
        {
		case MU_MSG_VCARD:
		case MU_MSG_VCAL:
        case MU_MSG_SMS:
        case MU_MSG_STATUS_REPORT:
            if(mu_initstatus.bSMS)
                mu_sms_interface->msg_read_message(hWnd,pChainNode->handle, bPre, bNext);
        	break;

        case MU_MSG_MMS:
        case MU_MSG_MMS_DELIVER_REPORT:
            if(mu_initstatus.bMMS)
                mu_mms_interface->msg_read_message(hWnd,pChainNode->handle, bPre, bNext);
            break;

        case MU_MSG_EMAIL:
        case MU_MSG_EMAILHEADER:
            if(mu_initstatus.bEmail)
                mu_email_interface->msg_read_message(hWnd,pChainNode->handle, bPre, bNext);
            break;

        case MU_MSG_PUSH:
            if(mu_initstatus.bPush)
                mu_push_interface->msg_read_message(hWnd,pChainNode->handle, bPre, bNext);
            break;
            
/*
                    case MU_MSG_FOLDER:
                        if(pChainNode->status == MU_STU_PHRASE)
                            ;
                        else if(pChainNode->status == MU_STU_DIR)
                        {
                            MU_FOLDERINFO folderinfo;
                            memset(&folderinfo,0,sizeof(MU_FOLDERINFO));
                            folderinfo.nFolderID = pChainNode->handle;
                            strncpy(folderinfo.szFolderName,(const char*)(pChainNode->maskdate),MU_FOLDER_NAME_LEN);
                            folderinfo.szFolderName[MU_FOLDER_NAME_LEN] = 0;
                            CreateSubFolderWnd(GetWindow(hWnd,GW_OWNER),&folderinfo);
                        }
                        break;*/
        case MU_MSG_BT:
		case MU_MSG_BT_VCARD:
		case MU_MSG_BT_VCAL:
		case MU_MSG_BT_NOTEPAD:
		case MU_MSG_BT_PICTURE:
			if(mu_initstatus.bBT)
                mu_bt_interface->msg_read_message(hWnd,pChainNode->handle, bPre, bNext);
			break;

        default:
            break;
        }
        break;

    case IDM_FOLDER_CREATEMSG_SMS:
        if(mu_initstatus.bSMS)
            mu_sms_interface->msg_new_message(hWnd);
        break;
        
    case IDM_FOLDER_CREATEMSG_MMS:
        if(mu_initstatus.bMMS)
            mu_mms_interface->msg_new_message(hWnd);
        break;
        
    case IDM_FOLDER_CREATEMSG_EMAIL:
        if(mu_initstatus.bEmail)
            mu_email_interface->msg_new_message(hWnd);
        break;

	case IDM_FOLDER_SUSPEND:
		nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
        if( nCurSel == LB_ERR )
            return;
        
        pChainNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
        if( !pChainNode )
            return;

        switch(pChainNode->msgtype) 
        {
		case MU_MSG_VCARD:
		case MU_MSG_VCAL:
        case MU_MSG_SMS:
            if(mu_initstatus.bSMS)
                mu_sms_interface->msg_defer2send_message(hWnd,pChainNode->handle);
        	break;

        case MU_MSG_MMS:
            if(mu_initstatus.bMMS)
                mu_mms_interface->msg_defer2send_message(hWnd,pChainNode->handle);
            break;

        case MU_MSG_EMAIL:
            if(mu_initstatus.bEmail)
                mu_email_interface->msg_defer2send_message(hWnd,pChainNode->handle);
            break;

        case MU_MSG_EMAILHEADER:
            if(mu_initstatus.bEmail)
                ;//mu_emailbox_interface->msg_forward(hWnd,pChainNode->handle);
            break;

        case MU_MSG_PUSH://????
            if(mu_initstatus.bPush)
                mu_push_interface->msg_defer2send_message(hWnd,pChainNode->handle);
            break;

        case MU_MSG_STATUS_REPORT:
        case MU_MSG_MMS_DELIVER_REPORT:
        default:
            break;
        }
		break;
		break;

	case IDM_FOLDER_SENDING:
		nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
        if( nCurSel == LB_ERR )
            return;
        
        pChainNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
        if( !pChainNode )
            return;

        switch(pChainNode->msgtype) 
        {
		case MU_MSG_VCARD:
		case MU_MSG_VCAL:
        case MU_MSG_SMS:
            if(mu_initstatus.bSMS)
                mu_sms_interface->msg_resend_message(hWnd,pChainNode->handle);
        	break;

        case MU_MSG_MMS:
            if(mu_initstatus.bMMS)
                mu_mms_interface->msg_resend_message(hWnd,pChainNode->handle);
            break;

        case MU_MSG_EMAIL:
            if(mu_initstatus.bEmail)
                mu_email_interface->msg_resend_message(hWnd,pChainNode->handle);
            break;

        case MU_MSG_EMAILHEADER:
            if(mu_initstatus.bEmail)
                ;//mu_emailbox_interface->msg_forward(hWnd,pChainNode->handle);
            break;

        case MU_MSG_PUSH://????
            if(mu_initstatus.bPush)
                mu_push_interface->msg_resend_message(hWnd,pChainNode->handle);
            break;

        case MU_MSG_STATUS_REPORT:
        case MU_MSG_MMS_DELIVER_REPORT:
        default:
            break;
        }
		break;

    case IDM_FOLDER_FORWARD:
        nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
        if( nCurSel == LB_ERR )
            return;
        
        pChainNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
        if( !pChainNode )
            return;

        switch(pChainNode->msgtype) 
        {
		case MU_MSG_VCARD:
		case MU_MSG_VCAL:
        case MU_MSG_SMS:
            if(mu_initstatus.bSMS)
                mu_sms_interface->msg_forward(hWnd,pChainNode->handle);
        	break;

        case MU_MSG_MMS:
            if(mu_initstatus.bMMS)
                mu_mms_interface->msg_forward(hWnd,pChainNode->handle);
            break;

        case MU_MSG_EMAIL:
            if(mu_initstatus.bEmail)
                mu_email_interface->msg_forward(hWnd,pChainNode->handle);
            break;

        case MU_MSG_EMAILHEADER:
            if(mu_initstatus.bEmail)
                ;//mu_emailbox_interface->msg_forward(hWnd,pChainNode->handle);
            break;

        case MU_MSG_PUSH://????
            if(mu_initstatus.bPush)
                mu_push_interface->msg_forward(hWnd,pChainNode->handle);
            break;

        case MU_MSG_STATUS_REPORT:
        case MU_MSG_MMS_DELIVER_REPORT:
        default:
            break;
        }
        break;
        
    case IDM_FOLDER_REPLY:
    case IDM_FOLDER_REPLY_SENDER:
        nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
        if( nCurSel == LB_ERR )
            return;
        
        pChainNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
        if( !pChainNode )
            return;

        switch(pChainNode->msgtype) 
        {
		case MU_MSG_VCARD:
		case MU_MSG_VCAL:
        case MU_MSG_SMS:
            if(mu_initstatus.bSMS)
                mu_sms_interface->msg_reply_from(hWnd,pChainNode->handle);
        	break;

        case MU_MSG_MMS:
            if(mu_initstatus.bMMS)
                mu_mms_interface->msg_reply_from(hWnd,pChainNode->handle);
            break;

        case MU_MSG_EMAIL:
            if(mu_initstatus.bEmail)
                mu_email_interface->msg_reply_from(hWnd,pChainNode->handle);
            break;

        case MU_MSG_EMAILHEADER:
//            if(mu_initstatus.bEmail)
//                mu_emailbox_interface->email_reply_from(hWnd,pChainNode->handle);
            break;

        case MU_MSG_PUSH://????
            if(mu_initstatus.bPush)
                mu_push_interface->msg_reply_from(hWnd,pChainNode->handle);
            break;

        case MU_MSG_STATUS_REPORT:
        case MU_MSG_MMS_DELIVER_REPORT:
        default:
            break;
        }
        break;
        
    case IDM_FOLDER_REPLY_ALL:
        nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
        if( nCurSel == LB_ERR )
            return;
        
        pChainNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
        if( !pChainNode )
            return;

        switch(pChainNode->msgtype) 
        {
		case MU_MSG_VCARD:
		case MU_MSG_VCAL:
        case MU_MSG_SMS:
            if(mu_initstatus.bSMS)
                mu_sms_interface->msg_reply_all(hWnd,pChainNode->handle);
        	break;

        case MU_MSG_MMS:
            if(mu_initstatus.bMMS)
                mu_mms_interface->msg_reply_all(hWnd,pChainNode->handle);
            break;

        case MU_MSG_EMAIL:
            if(mu_initstatus.bEmail)
                mu_email_interface->msg_reply_all(hWnd,pChainNode->handle);
            break;

        case MU_MSG_EMAILHEADER:
//           if(mu_initstatus.bEmail)
//                mu_emailbox_interface->email_reply_all(hWnd,pChainNode->handle);
            break;

        case MU_MSG_PUSH://????
            if(mu_initstatus.bPush)
                mu_push_interface->msg_reply_all(hWnd,pChainNode->handle);
            break;

        case MU_MSG_STATUS_REPORT:
        case MU_MSG_MMS_DELIVER_REPORT:
        default:
            break;
        }
        break;
        
    case IDM_FOLDER_DELETE:
        {
            PMU_LISTCREATEDATA pCreateData;
            
            pCreateData = GetUserData(hWnd);
            
            nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
            if( nCurSel == LB_ERR )
                break;
            
            pCreateData->pSelect = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
            
            if(pCreateData->pSelect == NULL)
                break;
            
            PLXConfirmWinEx(hMuFrame, hWnd, (char*)IDS_SUREDEL, Notify_Request, NULL, 
                (char*)IDS_YES, (char*)IDS_NO, WM_DELONE);
        }
        break;
        
    case IDM_FOLDER_MSGDETAIL:
        nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
        if( nCurSel == LB_ERR )
            return;
        
        pChainNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
        if( !pChainNode )
            return;

        switch(pChainNode->msgtype) 
        {
		case MU_MSG_VCARD:
		case MU_MSG_VCAL:
        case MU_MSG_SMS:
        case MU_MSG_STATUS_REPORT:
            if(mu_initstatus.bSMS)
                mu_sms_interface->msg_get_detail(hWnd,pChainNode->handle);
        	break;

        case MU_MSG_MMS:
        case MU_MSG_MMS_DELIVER_REPORT:
            if(mu_initstatus.bMMS)
                mu_mms_interface->msg_get_detail(hWnd,pChainNode->handle);
            break;

        case MU_MSG_EMAIL:
            if(mu_initstatus.bEmail)
                mu_email_interface->msg_get_detail(hWnd,pChainNode->handle);
            break;

        case MU_MSG_EMAILHEADER://need modify
//            if(mu_initstatus.bEmail)
//                mu_emailbox_interface->email_get_detail(hWnd,pChainNode->handle);
            break;

        case MU_MSG_PUSH:
            if(mu_initstatus.bPush)
                mu_push_interface->msg_get_detail(hWnd,pChainNode->handle);
            break;

		case MU_MSG_BT:
		case MU_MSG_BT_VCARD:
		case MU_MSG_BT_VCAL:	
		case MU_MSG_BT_NOTEPAD:
		case MU_MSG_BT_PICTURE:
			if(mu_initstatus.bBT)
                mu_bt_interface->msg_get_detail(hWnd,pChainNode->handle);
			break;

        default:
            break;
        }
        break;
        
    case IDM_FOLDER_MOVE:
        nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
        if( nCurSel == LB_ERR )
            return;
        
        p = GetUserData(hWnd);
        
        nFolder = -1;

        p->pSelect = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);

        if(p->pSelect == NULL)
            break;

        if( p->bEmailBox == FALSE && p->nBoxType == MU_OUTBOX )
        {
            nFolder = MU_DRAFT;
            On_mu_movetofolder(hWnd,TRUE,nFolder);
        }
        else
        {
            if( !MU_FolderSelection(hWnd,hWnd,WM_SELECTFOLDER,p->nBoxType) )
            {
                return;
            }
        }
        break;

    case IDM_FOLDER_COPY:
//        nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
//        if( nCurSel == LB_ERR )
//            return;
//        
//        pChainNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
//        if( !pChainNode )
//            return;
//
//        p = GetUserData(hWnd);
//        
//        nFolder = -1;
//        if( !MU_EmialboxFolderSelection(hWnd,p->szEmailName,&nFolder) )
//        {
//           return;
//        }
//        else
//        {
//            if( nFolder == -1 )
//                return;
//        }
//
//        switch(pChainNode->msgtype) 
//        {
//        case MU_MSG_EMAILHEADER:
//            if(mu_initstatus.bEmail)
//                mu_email_interface->msg_copy_message(hWnd,pChainNode->handle,nFolder);
//            break;
//
//        default:
//            break;
//        }
        break;

    case IDC_MULSIT_LIST:
        if(codeNotify == LBN_SELCHANGE)
        {
            if(MU_GetCurFolderType() == MU_OUTBOX)
            {
                p = GetUserData(hWnd);
                p->bChangeMenu = TRUE;
            }

            if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                MU_SetSoftLeft(hWnd);
        }
        break;
		
	case IDM_FOLDER_MOVE_SELECT:
		{
			MU_SelectMsg(hWnd, TRUE, FALSE);
		}
		break;

	case IDM_FOLDER_MOVE_ALL:
		{
			p = GetUserData(hWnd);

			MU_FolderSelection(hWnd,hWnd,WM_ALLMOVE,p->nBoxType);
		}
		break;

	case IDM_FOLDER_DELETE_SELECT:
		{
			MU_SelectMsg(hWnd, FALSE, TRUE);
		}
		break;

	case IDM_FOLDER_DELETE_ALL:
		{
			p = GetUserData(hWnd);
			
			if(p->nBoxType == MU_REPORT)
				PLXConfirmWinEx(hMuFrame, hWnd, (char*)IDS_REMOVE_REPORT, Notify_Request, NULL, 
				(char*)IDS_YES, (char*)IDS_NO, WM_DELALL);
			else
				PLXConfirmWinEx(hMuFrame, hWnd, (char*)IDS_DELETE_ALL, Notify_Request, NULL, 
				(char*)IDS_YES, (char*)IDS_NO, WM_DELALL);
		}
		break;

    default:
        break;
	}

	return;
}

/*********************************************************************\
* Function	MUList_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUList_OnDestroy(HWND hWnd)
{
    PMU_LISTCREATEDATA p;
    int                 nfolderid,i;

    p = GetUserData(hWnd);

    nfolderid = p->nBoxType;

    if(p->bEmailBox)
    {
//        if(mu_initstatus.bEmail)
//            mu_emailbox_interface->email_release_messages(hWnd);
    }
    else
    {      
        
        if(mu_initstatus.bSMS)
        {
            mu_sms_interface->msg_release_messages(hWnd);
        }
        if(mu_initstatus.bMMS)
        {
            mu_mms_interface->msg_release_messages(hWnd);
        }
        if(mu_initstatus.bEmail)
        {
            mu_email_interface->msg_release_messages(hWnd);
        }
    }

    memset(&(p->mu_counter),0,sizeof(MU_COUNTER));

    MU_Erase(&(p->pFirst));
    
    MU_Erase(&(p->pUnReadFirst));

    p->pFirst = NULL;

    p->pUnReadFirst = NULL;

    MM_Destroy();

    
    for(i = GIF_MU_SMS_READ ; i < GIF_MU_MAX ; i++)
    {
        DeleteObject(mu_hBmp[i]);
        mu_hBmp[i] = NULL;
    }

    DeleteObject(hBmp);
	hBmp = NULL;

    /*if(p->nBoxType > MU_REPORT)
    {
        int n,index;
        HWND hList;
        PMU_LISTCREATEDATA pCreateData;
        PMU_CHAINNODE pChain;

        n = MU_MYFOLDER;
        MU_CreateFolderList(GetWindow(hWnd,GW_OWNER),&n,FALSE);  

        pCreateData = GetUserData(hListWnd);

        pChain = pCreateData->pFirst;

        index = 0;

        while(pChain)
        {
            if(pChain->status == MU_STU_DIR && 
                pChain->handle == (DWORD)nfolderid)
                break;
            index++;
            pChain = pChain->pNext;
        }      
        hList = GetDlgItem(hListWnd,IDC_MULSIT_LIST);
        SendMessage(hList,LB_SETCURSEL,index,0);
    }*/

	if(pHandleList)
	{
		free(pHandleList);
		pHandleList = NULL;
	}
    return;

}
/*********************************************************************\
* Function	MUList_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUList_OnClose(HWND hWnd)
{

    DestroyWindow (hWnd);

    return;

}
/*********************************************************************\
* Function	   MU_GetCurCaption
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static PCSTR MU_GetCurCaption(PMU_LISTCREATEDATA CreateData)
{
    if(CreateData->bEmailBox)
    {
        return(CreateData->szEmailName);
    }
    else
    {
        switch(CreateData->nBoxType) 
        {
        case MU_INBOX:
            return(IDS_INBOX);
            
        case MU_OUTBOX:
            return(IDS_OUTBOX);
            
        case MU_SENT:
            return(IDS_SENT);
            
        case MU_DRAFT:
            return(IDS_DRAFT);
            
        case MU_REPORT:
            return(IDS_DELIVERYREPORT);
            
        case MU_MYFOLDER:
            return(IDS_MYFOLDER);
            
        default://new folder need to modify
			return CreateData->szFolderName;
        }
    }
    return NULL;
}
/*********************************************************************\
* Function	   APP_SHOWINBOX
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void    APP_SHOWINBOX(void)
{
    HWND hWnd,hMenuList,hList;
    int n;
    n = MU_INBOX;
    //if exist ,need close this wnd ,but i think it should save editing msg !!!!!!!
    MessageUnibox_AppControl(APP_ACTIVE,NULL,NULL,NULL);
    hWnd = (HWND)MessageUnibox_AppControl(APP_GETOPTION,NULL,AS_APPWND,NULL);
    hMenuList = GetWindow(hWnd,GW_CHILD);
    SendMessage(hMenuList,ML_SETCURSEL,1,0);
    
    //if(IsWindow(hFolderListWnd))
    //    SendMessage(hFolderListWnd,WM_CLOSE,0,0);

    if(IsWindow(hListWnd))
        SendMessage(hListWnd,WM_CLOSE,0,0);

    MU_CreateList(hWnd,&n,FALSE);

    hList = GetDlgItem(hListWnd,IDC_MULSIT_LIST);

    SendMessage(hList,LB_SETCURSEL,0,0);

    SendMessage(hListWnd,WM_COMMAND,MAKEWPARAM(IDM_FOLDER_OPEN,BN_CLICKED),NULL);        
}
/*********************************************************************\
* Function	   MU_GetListWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
HWND MU_GetListWnd(void)
{
    if(IsWindow(hListWnd))
        return hListWnd;

    //if(IsWindow(hFolderListWnd))
    //    return hFolderListWnd;

    return NULL;
}
/*********************************************************************\
* Function	   MU_GetCurBoxType
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int MU_GetCurFolderType(void)
{
    HWND hWnd;
    PMU_LISTCREATEDATA p;

    hWnd = MU_GetListWnd();

    if(IsWindow(hWnd))
    {
        p = GetUserData(hWnd);
        if(p->bEmailBox)
            return -1;
        else
            return (p->nBoxType);
    }
    return -1;
}
/*********************************************************************\
* Function	   MU_GetCurEmailboxName
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
const char* MU_GetCurEmailboxName(void)
{
    HWND hWnd;
    PMU_LISTCREATEDATA p;

    hWnd = MU_GetListWnd();

    if(IsWindow(hWnd))
    {
        p = GetUserData(hWnd);
        if(p->bEmailBox)
            return (const char*)(p->szEmailName);
        else
            return NULL;
    }
    return NULL;
}

/*********************************************************************\
* Function	   MU_IsWindow
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL    MU_IsWindow(void)
{
    return(IsWindow(MU_GetListWnd()));
}
/*********************************************************************\
* Function	   MU_IsWindow
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LONG MU_GetSUM(PMU_LISTCREATEDATA pCreateData)
{
    if( pCreateData->bEmailBox )
        return(pCreateData->mu_counter.nEmailHeader+pCreateData->mu_counter.nEmail);
    else
        return(pCreateData->mu_counter.nEmail+pCreateData->mu_counter.nMMS
        +pCreateData->mu_counter.nSMS+pCreateData->mu_counter.nPush
		+pCreateData->mu_counter.nBT);
}
/*********************************************************************\
* Function	   On_mu_getmsgs_resp
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void On_mu_getmsgs_resp(HWND hWnd, int nMduType, int nErrType)
{
    PMU_LISTCREATEDATA pCreateData;
    MU_MsgNode  msgnode;
    PMU_CHAINNODE p;
    HWND    hLst;
    int     nInsert;
    SYSTEMTIME syLocal;

    pCreateData = GetUserData(hWnd);
    hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);

	switch(nMduType) 
	{
	case MU_MDU_SMS:
		if (nErrType == MU_ERR_SUCC)
		{
            memset(&msgnode,0,sizeof(MU_MsgNode));           
            memset(&syLocal,0,sizeof(SYSTEMTIME));
            GetLocalTime(&syLocal);       
			while (mu_sms_interface->msg_get_one_message(&msgnode))
			{                
                if(msgnode.status == MU_STU_UNREAD)
                    nInsert = MU_BuildChain(&(pCreateData->pUnReadFirst), &msgnode,&p);
                else
                    nInsert = MU_BuildChain(&(pCreateData->pFirst), &msgnode,&p);
                
                if(nInsert == -1)
                    return;

				if(msgnode.status == MU_STU_UNREAD)
                    pCreateData->mu_counter.nUnread++; 
                else
                    nInsert += pCreateData->mu_counter.nUnread;


                MU_AddNodeToList(hLst,&msgnode,p,nInsert,syLocal);
				
                memset(&msgnode, 0, sizeof(MU_MsgNode));
				
                pCreateData->mu_counter.nSMS++;
			}
			SendMessage(hLst, LB_SETCURSEL, (WPARAM)0, (LPARAM)NULL);
            MU_SetSoftLeft(hWnd);
		}
		else
;//			PLXTipsWin((char*)IDS_READSMSFAILED, (char*)IDS_WARNING, WAITTIMEOUT);
		break;

	case MU_MDU_MMS:
		if (nErrType == MU_ERR_SUCC)
		{
            memset(&msgnode,0,sizeof(MU_MsgNode));         
            memset(&syLocal,0,sizeof(SYSTEMTIME));
            GetLocalTime(&syLocal);       
			while (mu_mms_interface->msg_get_one_message(&msgnode))
			{
                if(msgnode.status == MU_STU_UNREAD)
                    nInsert = MU_BuildChain(&(pCreateData->pUnReadFirst), &msgnode,&p);
                else
                    nInsert = MU_BuildChain(&(pCreateData->pFirst), &msgnode,&p);
                
                if(nInsert == -1)
                    return;

                if(msgnode.status == MU_STU_UNREAD)
                    pCreateData->mu_counter.nUnread++; 
                else
                    nInsert += pCreateData->mu_counter.nUnread;

                MU_AddNodeToList(hLst,&msgnode,p,nInsert,syLocal);

				memset(&msgnode, 0, sizeof(MU_MsgNode));

				pCreateData->mu_counter.nMMS++;     
			}
			SendMessage(hLst, LB_SETCURSEL, (WPARAM)0, (LPARAM)NULL);
            MU_SetSoftLeft(hWnd);
		}
		else
			;//PLXTipsWin((char*)IDS_READMMSFAILED, (char*)IDS_WARNING, WAITTIMEOUT);
		break;

    case MU_MDU_EMAIL:

        if(pCreateData->bEmailBox)
        {
            /*
            if (nErrType == MU_ERR_SUCC)
                        {
                            //need sort by date,subject,sender 
                            memset(&msgnode,0,sizeof(MU_MsgNode));
                            while (mu_emailbox_interface->email_get_one_message(&msgnode))
                            {
                                //AddNodeToList(hWndList, &msgnode);
                                memset(&msgnode, 0, sizeof(MU_MsgNode));
                                pCreateData->mu_counter.nEmailHeader++;
                            }
                            SendMessage(hLst, LB_SETCURSEL, (WPARAM)0, (LPARAM)NULL);
                            MU_SetSoftLeft(hWnd);
                        }
                        else
                            ;//PLXTipsWin((char*)IDS_READEMAILBOXFAILED, (char*)IDS_WARNING, WAITTIMEOUT);*/
            
        }
        else
        {
            if (nErrType == MU_ERR_SUCC)
            {
                memset(&msgnode,0,sizeof(MU_MsgNode));      
                memset(&syLocal,0,sizeof(SYSTEMTIME));
                GetLocalTime(&syLocal);       
                while (mu_email_interface->msg_get_one_message(&msgnode))
                {                    
                    if(msgnode.status == MU_STU_UNREAD)
                        nInsert = MU_BuildChain(&(pCreateData->pUnReadFirst), &msgnode,&p);
                    else
                        nInsert = MU_BuildChain(&(pCreateData->pFirst), &msgnode,&p);
                    
                    if(nInsert == -1)
                        return;
 
                    if(msgnode.status == MU_STU_UNREAD)
                    pCreateData->mu_counter.nUnread++; 
                else
                    nInsert += pCreateData->mu_counter.nUnread;
                    
                    MU_AddNodeToList(hLst,&msgnode,p,nInsert,syLocal);
                    
                    memset(&msgnode, 0, sizeof(MU_MsgNode));

                    pCreateData->mu_counter.nEmail++;
                }
                SendMessage(hLst, LB_SETCURSEL, (WPARAM)0, (LPARAM)NULL);
                MU_SetSoftLeft(hWnd);
            }
            else
                ;//PLXTipsWin((char*)IDS_READEMAILFAILED, (char*)IDS_WARNING, WAITTIMEOUT);
        }

        break;
	
	case MU_MDU_PUSH:
        if (nErrType == MU_ERR_SUCC)
        {
            memset(&msgnode,0,sizeof(MU_MsgNode));      
            memset(&syLocal,0,sizeof(SYSTEMTIME));
            GetLocalTime(&syLocal);       
            while (mu_push_interface->msg_get_one_message(&msgnode))
            {                            
                if(msgnode.status == MU_STU_UNREAD)
                    nInsert = MU_BuildChain(&(pCreateData->pUnReadFirst), &msgnode,&p);
                else
                    nInsert = MU_BuildChain(&(pCreateData->pFirst), &msgnode,&p);
                
                if(nInsert == -1)
                    return;

                if(msgnode.status == MU_STU_UNREAD)
                    pCreateData->mu_counter.nUnread++; 
                else
                    nInsert += pCreateData->mu_counter.nUnread;
                
                MU_AddNodeToList(hLst,&msgnode,p,nInsert,syLocal);

                memset(&msgnode, 0, sizeof(MU_MsgNode));
            
                pCreateData->mu_counter.nPush++;
            }
            SendMessage(hLst, LB_SETCURSEL, (WPARAM)0, (LPARAM)NULL);
            MU_SetSoftLeft(hWnd);	
        }
        else
            ;//PLXTipsWin((char*)IDS_READPUSHFAILED, (char*)IDS_WARNING, WAITTIMEOUT);
        
        break;

	case MU_MDU_BT:
		if (nErrType == MU_ERR_SUCC)
        {
            memset(&msgnode,0,sizeof(MU_MsgNode));      
            memset(&syLocal,0,sizeof(SYSTEMTIME));
            GetLocalTime(&syLocal);       
            while (mu_bt_interface->msg_get_one_message(&msgnode))
            {                            
                if(msgnode.status == MU_STU_UNREAD)
                    nInsert = MU_BuildChain(&(pCreateData->pUnReadFirst), &msgnode,&p);
                else
                    nInsert = MU_BuildChain(&(pCreateData->pFirst), &msgnode,&p);
                
                if(nInsert == -1)
                    return;

                if(msgnode.status == MU_STU_UNREAD)
                    pCreateData->mu_counter.nUnread++; 
                else
                    nInsert += pCreateData->mu_counter.nUnread;
                
                MU_AddNodeToList(hLst,&msgnode,p,nInsert,syLocal);

                memset(&msgnode, 0, sizeof(MU_MsgNode));
            
                pCreateData->mu_counter.nBT++;
            }
            SendMessage(hLst, LB_SETCURSEL, (WPARAM)0, (LPARAM)NULL);
            MU_SetSoftLeft(hWnd);	
        }
		break;

	}
}
/*********************************************************************\
* Function	   On_mu_delete
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL On_mu_delete(HWND hWnd,int nMduType,int nErrType, DWORD handle)
{
    HWND hLst;
    int  nIndex,nCount, i;
    PMU_CHAINNODE pNode = NULL;
    PMU_LISTCREATEDATA pCreateData;
    PMU_CHAINNODE p;
    BOOL bOK;

    hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
    pCreateData = GetUserData(hWnd);
	
	nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
	for(i=0; i<nCount; i++)
	{
		pNode = (PMU_CHAINNODE)SendMessage(hLst, LB_GETITEMDATA, i, 0);

		if(pNode && pNode->handle == handle)
		{
			nIndex = i;
			break;
		}

        pNode = NULL;
	}

	if(pNode == NULL)
		return FALSE;

	nCount--;

	switch (nMduType) 
	{
	case MU_MDU_SMS:
 		if (nErrType == MU_ERR_SUCC)
 		{
			p = pCreateData->pUnReadFirst;            
            bOK = FALSE;

            while ( p )
            {
                if( p == pNode )
                {
                    SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);
                    
                    MU_DeleteNode(&(pCreateData->pUnReadFirst),pNode);
                    
                    pCreateData->mu_counter.nUnread--;
                    
                    nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);
                    
                    if(nIndex >= 0)
                        SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);
                    
                    pCreateData->mu_counter.nSMS--;
                    
                    if(MU_GetSUM(pCreateData) == 0)
                    {
                        pCreateData->bChangeMenu = TRUE;

                        ShowWindow(hLst,SW_HIDE);

                        //UpdateWindow(hWnd);
                    }
                    
                    if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                        MU_SetSoftLeft(hWnd);
                    
                    bOK = TRUE;

                    return TRUE;
                }
                p = p->pNext;
            }
			
            p = pCreateData->pFirst;            

            while ( p )
            {
                if(p == pNode)
                {
                    SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);
                    
                    MU_DeleteNode(&(pCreateData->pFirst),pNode);
                    
                    nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);

                    if(nIndex >= 0)
                        SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);
                    
                    pCreateData->mu_counter.nSMS--;
                    
                    if(MU_GetSUM(pCreateData) == 0)
                    {
                        pCreateData->bChangeMenu = TRUE;

                        ShowWindow(hLst,SW_HIDE);

                        //UpdateWindow(hWnd);
                    }

                    if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                        MU_SetSoftLeft(hWnd);

                    bOK = TRUE;

                    return TRUE;
                }
                p = p->pNext;
            }

            if(!bOK)
                return FALSE;
        }
		else
 			return FALSE;
		break;

	case MU_MDU_MMS:
		if (nErrType == MU_ERR_SUCC)
		{
			p = pCreateData->pUnReadFirst;            
            bOK = FALSE;

            while ( p )
            {
                if( p == pNode )
                {
                    SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);
                    
                    MU_DeleteNode(&(pCreateData->pUnReadFirst),pNode);
                    
                    pCreateData->mu_counter.nUnread--;
                    
                    nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);
                    
                    if(nIndex >= 0)
                        SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);
                    
                    pCreateData->mu_counter.nMMS--;
                    
                    if(MU_GetSUM(pCreateData) == 0)
                    {
                        pCreateData->bChangeMenu = TRUE;

                        ShowWindow(hLst,SW_HIDE);

                        //UpdateWindow(hWnd);
                    }

                    if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                        MU_SetSoftLeft(hWnd);

                    bOK = TRUE;

                    return TRUE;
                }
                p = p->pNext;
            }

            p = pCreateData->pFirst;            

            while ( p )
            {
                if( p == pNode )
                {
                    SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);
                    
                    MU_DeleteNode(&(pCreateData->pFirst),pNode);
                    
                    nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);
                    
                    if(nIndex >= 0)
                        SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);
                    
                    pCreateData->mu_counter.nMMS--;
                    
                    if(MU_GetSUM(pCreateData) == 0)
                    {
                        pCreateData->bChangeMenu = TRUE;

                        ShowWindow(hLst,SW_HIDE);

                        //UpdateWindow(hWnd);
                    }

                    if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                        MU_SetSoftLeft(hWnd);

                    bOK = TRUE;

                    return TRUE;
                }
                p = p->pNext;
            }
            
            
            if(!bOK)
                return FALSE;
		}
		else
 			return FALSE;
		break;

    case MU_MDU_EMAIL:
        if(pCreateData->bEmailBox) // e-mailbox
        {
            if (nErrType == MU_ERR_SUCC)
            {
                pCreateData->bChangeMenu = TRUE;
            }
            else
                ;//PLXTipsWin((char*)IDS_DELEMAILERR, (char*)IDS_WARNING, WAITTIMEOUT);
        }
        else
        {
            if (nErrType == MU_ERR_SUCC)
            {
				p = pCreateData->pUnReadFirst;            
                bOK = FALSE;
                
                while ( p )
                {
                    if( p == pNode )
                    {
                        SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);
                        
                        MU_DeleteNode(&(pCreateData->pUnReadFirst),pNode);
                        
                        pCreateData->mu_counter.nUnread--;
                        
                        nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);
                        
                        if(nIndex >= 0)
                            SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);
                        
                        pCreateData->mu_counter.nEmail--;
                        
                        if(MU_GetSUM(pCreateData) == 0)
                        {
                            pCreateData->bChangeMenu = TRUE;
                            
                            ShowWindow(hLst,SW_HIDE);
                            
                            //UpdateWindow(hWnd);
                        }
                        
                        if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                            MU_SetSoftLeft(hWnd);

                        bOK = TRUE;
                        
                        return TRUE;
                    }
                    p = p->pNext;
                }
				
                p = pCreateData->pFirst;              
                
                while ( p )
                {
                    if( p == pNode )
                    {    
                        SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);                        
                        
                        MU_DeleteNode(&(pCreateData->pFirst),pNode);
                        
                        nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);
                        
                        if(nIndex >= 0)
                            SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);
                        
                        pCreateData->mu_counter.nEmail--;
                        
                        if(MU_GetSUM(pCreateData) == 0)
                        {
                            pCreateData->bChangeMenu = TRUE;
                            
                            ShowWindow(hLst,SW_HIDE);
                            
                            //UpdateWindow(hWnd);
                        }
                        
                        if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                            MU_SetSoftLeft(hWnd);

                        bOK = TRUE;
                        
                        return TRUE;
                    }
                    p = p->pNext;
                }
                
                
                if(!bOK)
                    return FALSE;
            }
            else
                return FALSE;
        }
        break;

	case MU_MDU_BT:
        if (nErrType == MU_ERR_SUCC)
        {       
			p = pCreateData->pUnReadFirst;            
            bOK = FALSE;

            while ( p )
            {
                if( p == pNode )
                {
                    SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);
                    
                    MU_DeleteNode(&(pCreateData->pUnReadFirst),pNode);
                    
                    pCreateData->mu_counter.nUnread--;
                    
                    nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);
                    
                    if(nIndex >= 0)
                        SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);
                    
                    pCreateData->mu_counter.nBT--;
                    
                    if(MU_GetSUM(pCreateData) == 0)
                    {
                        pCreateData->bChangeMenu = TRUE;

                        ShowWindow(hLst,SW_HIDE);

                        //UpdateWindow(hWnd);
                    }

                    if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                        MU_SetSoftLeft(hWnd);

                    bOK = TRUE;

                    return TRUE;
                }
                p = p->pNext;
            }

            p = pCreateData->pFirst;            

            while ( p )
            {
                if( p == pNode )
                {
                    SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);
                    
                    MU_DeleteNode(&(pCreateData->pFirst),pNode);                  
                    
                    nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);
                    
                    if(nIndex >= 0)
                        SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);
                    
                    pCreateData->mu_counter.nBT--;
                    
                    if(MU_GetSUM(pCreateData) == 0)
                    {
                        pCreateData->bChangeMenu = TRUE;

                        ShowWindow(hLst,SW_HIDE);

                        //UpdateWindow(hWnd);
                    }

                    if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                        MU_SetSoftLeft(hWnd);

                    bOK = TRUE;

                    return TRUE;
                }
                p = p->pNext;
            }
            
            if(!bOK)
                return FALSE;
        }
        else
            return FALSE;
        break;

    default:
        return FALSE;
	}

    return FALSE;
}
/*********************************************************************\
* Function	   On_mu_deleted_resp
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void On_mu_deleted_resp(HWND hWnd,int nMduType,int nErrType, DWORD handle)
{
    HWND hLst;
    int  nIndex,nCount, i;
    PMU_CHAINNODE pNode = NULL;
    PMU_LISTCREATEDATA pCreateData;

    hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
    pCreateData = GetUserData(hWnd);

	for(i=0; i<MU_GetSUM(pCreateData); i++)
	{      
		pNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,i,0);

		if(pNode->handle == handle)
		{
			nIndex = i;
			break;
		}
	}

	if(pNode == NULL)
		return;

	switch (nMduType) 
	{
	case MU_MDU_SMS:
 		if (nErrType == MU_ERR_SUCC)
 		{
//            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
//            pNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
            //assert( pNode->handle == handle );

            if(pNode->handle == handle)
			{
				SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);
                
                if(pNode->status == MU_STU_UNREAD)
                {
                    MU_DeleteNode(&(pCreateData->pUnReadFirst),pNode);
                    pCreateData->mu_counter.nUnread--;
                }
                else
                    MU_DeleteNode(&(pCreateData->pFirst),pNode);

                nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);

                nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);
                
                if(nIndex >= 0)
                    SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);
                
                pCreateData->mu_counter.nSMS--;
                
                if(MU_GetSUM(pCreateData) == 0)
                {
                    pCreateData->bChangeMenu = TRUE;
                    
                    ShowWindow(hLst,SW_HIDE);
                    
                    //UpdateWindow(hWnd);
                }

                MU_SetSoftLeft(hWnd);
            }
            else
                ;//PLXTipsWin((char*)IDS_DELSMSERR,(char*)IDS_PROMPT,WAITTIMEOUT);
		}
		else
 			;//PLXTipsWin((char*)IDS_DELSMSFAILED,(char*)IDS_WARNING,WAITTIMEOUT);
		break;

	case MU_MDU_MMS:
		if (nErrType == MU_ERR_SUCC)
		{
//			nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
//            pNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
            //assert( pNode->handle == handle );
            
            if(pNode->handle == handle)
			{
				SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);
                
                if(pNode->status == MU_STU_UNREAD)
                {
                    MU_DeleteNode(&(pCreateData->pUnReadFirst),pNode);
                    pCreateData->mu_counter.nUnread--;
                }
                else
                    MU_DeleteNode(&(pCreateData->pFirst),pNode);

                nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);

                nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);
                
                if(nIndex >= 0)
                    SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);

                pCreateData->mu_counter.nMMS--;
                
                if(MU_GetSUM(pCreateData) == 0)
                {
                    pCreateData->bChangeMenu = TRUE;
                    
                    ShowWindow(hLst,SW_HIDE);
                    
                    //UpdateWindow(hWnd);
                }
                
                MU_SetSoftLeft(hWnd);
            }
            else
                ;//PLXTipsWin((char*)IDS_DELMMSERR,(char*)IDS_PROMPT,WAITTIMEOUT);
		}
		else
 			;//PLXTipsWin((char*)IDS_DELMMSFAILED,(char*)IDS_WARNING,WAITTIMEOUT);
		break;

    case MU_MDU_EMAIL:
        if(pCreateData->bEmailBox) // e-mailbox
        {
            if (nErrType == MU_ERR_SUCC)
            {
                pCreateData->bChangeMenu = TRUE;
            }
            else
                ;//PLXTipsWin((char*)IDS_DELEMAILERR, (char*)IDS_WARNING, WAITTIMEOUT);
        }
        else
        {
            if (nErrType == MU_ERR_SUCC)
            {
//                nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
                
//                pNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
                
                //assert( pNode->handle == handle );
                
                if(pNode->handle == handle)
                {
                    SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);
                    
                    if(pNode->status == MU_STU_UNREAD)
                    {
                        MU_DeleteNode(&(pCreateData->pUnReadFirst),pNode);
                        pCreateData->mu_counter.nUnread--;
                    }
                    else
                        MU_DeleteNode(&(pCreateData->pFirst),pNode);
                    
                    nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
                    
                    nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);
                    
                    if(nIndex >= 0)
                        SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);
                    
                    pCreateData->mu_counter.nEmail--;

                    if(MU_GetSUM(pCreateData) == 0)
                    {
                        pCreateData->bChangeMenu = TRUE;

                        ShowWindow(hLst,SW_HIDE);

                        //UpdateWindow(hWnd);
                    }

                    MU_SetSoftLeft(hWnd);
                }
                else
                    ;//PLXTipsWin((char*)IDS_DELEMAILERR,(char*)IDS_PROMPT,WAITTIMEOUT);
            }
            else
                ;//PLXTipsWin((char*)IDS_DELEMAILFAILED,(char*)IDS_WARNING,WAITTIMEOUT);
        }
        break;
        
    case MU_MDU_PUSH:
        if (nErrType == MU_ERR_SUCC)
        {
//            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
//            pNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
            //assert( pNode->handle == handle );
            
            if(pNode->handle == handle)
            {
                SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);
                
                if(pNode->status == MU_STU_UNREAD)
                {
                    MU_DeleteNode(&(pCreateData->pUnReadFirst),pNode);
                    pCreateData->mu_counter.nUnread--;
                }
                else
                    MU_DeleteNode(&(pCreateData->pFirst),pNode);
                
                nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
                
                nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);
                
                if(nIndex >= 0)
                    SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);
                
                pCreateData->mu_counter.nPush--;
                
                if(MU_GetSUM(pCreateData) == 0)
                {
                    pCreateData->bChangeMenu = TRUE;
                    
                    ShowWindow(hLst,SW_HIDE);
                    
                    //UpdateWindow(hWnd);
                }
                
                MU_SetSoftLeft(hWnd);
            }
            else
                ;//PLXTipsWin((char*)IDS_DELPUSHERR,(char*)IDS_PROMPT,WAITTIMEOUT);
        }
        else
            ;//PLXTipsWin((char*)IDS_DELPUSHFAILED,(char*)IDS_WARNING,WAITTIMEOUT);		
        break;

	case MU_MDU_FOLDER:
		if (nErrType == MU_ERR_SUCC)
		{
//			nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
//            pNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
            //assert( pNode->handle == handle );
            
            if(pNode->handle == handle)
			{
				SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);
                                
                if(pNode->status == MU_STU_UNREAD)
                {
                    MU_DeleteNode(&(pCreateData->pUnReadFirst),pNode);
                    pCreateData->mu_counter.nUnread--;
                }
                else
                    MU_DeleteNode(&(pCreateData->pFirst),pNode);

                nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);

                nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);
                
                if(nIndex >= 0)
                    SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);
            }
            else
                ;//PLXTipsWin((char*)IDS_DELFOLDERERR,(char*)IDS_PROMPT,WAITTIMEOUT);
		}
		else
 			;//PLXTipsWin((char*)IDS_DELFOLDERFAILED,(char*)IDS_WARNING,WAITTIMEOUT);
		break;

	case MU_MDU_BT:
		if (nErrType == MU_ERR_SUCC)
		{
//			nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
//            pNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
            //assert( pNode->handle == handle );
            
            if(pNode->handle == handle)
			{
				SendMessage(hLst, LB_DELETESTRING, nIndex, (LPARAM)NULL);
                
                if(pNode->status == MU_STU_UNREAD)
                {
                    MU_DeleteNode(&(pCreateData->pUnReadFirst),pNode);
                    pCreateData->mu_counter.nUnread--;
                }
                else
                    MU_DeleteNode(&(pCreateData->pFirst),pNode);

                nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);

                nIndex = (nCount-1) > nIndex ? nIndex : (nCount-1);
                
                if(nIndex >= 0)
                    SendMessage(hLst,LB_SETCURSEL,nIndex,NULL);
                
                pCreateData->mu_counter.nBT--;
                
                if(MU_GetSUM(pCreateData) == 0)
                {
                    pCreateData->bChangeMenu = TRUE;
                    
                    ShowWindow(hLst,SW_HIDE);
                    
                    //UpdateWindow(hWnd);
                }
                
                MU_SetSoftLeft(hWnd);
            }
            else
                ;//PLXTipsWin((char*)IDS_DELMMSERR,(char*)IDS_PROMPT,WAITTIMEOUT);
		}
		else
 			;//PLXTipsWin((char*)IDS_DELMMSFAILED,(char*)IDS_WARNING,WAITTIMEOUT);
		break;

    default:
        break;
	}
}

/*********************************************************************\
* Function	   On_mu_modified
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void On_mu_modified(HWND hWnd, int nMduType, int ErrType, MU_MsgNode *pMsgNode)
{
    MU_LISTCREATEDATA *pData = NULL;
    DWORD* pdwArray = NULL;

    pData = GetUserData(hWnd);
    
	switch (nMduType) 
	{
	case MU_MDU_SMS:
        if (ErrType == MU_ERR_SUCC)
        {
            if(pHandleList)
            {
                HWND hLst;
                int  nCount, i;
                PMU_CHAINNODE pNode = NULL;
                
                hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                
                nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
                
                for(i=0; i<nCount; i++)
                {
                    pNode = (PMU_CHAINNODE)SendMessage(hLst, LB_GETITEMDATA, i, 0);
                    
                    if(pNode && pNode->handle == pMsgNode->handle && 
                        (pNode->msgtype == MU_MSG_STATUS_REPORT
                        || pNode->msgtype == MU_MSG_SMS))
                    {
                        break;
                    }

                    pNode = NULL;
                }
                
                if(pNode != NULL)
                    pdwArray = GetPosInArray((DWORD)pNode,pHandleList,nMsgCount);
            }

            if(SendMessage(hWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)pMsgNode->handle))
            {
                SendMessage(hWnd,PWM_MSG_MU_NEWMTMSG,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)pMsgNode);
                
                if(pHandleList)
                {     
                    if(pdwArray != NULL)
                    {    
                        HWND hLst;
                        int  nCount, i;
                        PMU_CHAINNODE pNode = NULL;
                        
                        hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                        
                        nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
                        
                        for(i=0; i<nCount; i++)
                        {
                            pNode = (PMU_CHAINNODE)SendMessage(hLst, LB_GETITEMDATA, i, 0);
                            
                            if(pNode && pNode->handle == pMsgNode->handle && 
                                (pNode->msgtype == MU_MSG_STATUS_REPORT
                                || pNode->msgtype == MU_MSG_SMS))
                            {
                                break;
                            }
                            pNode = NULL; 
                        }
                        
                        *pdwArray = (DWORD)pNode;
                        pdwArray = NULL;
                    }
                }
            }
        }
        else
        {
            ;//PLXTipsWin((char*)IDS_CHANGESTATEFAILED, (char*)IDS_WARNING, WAITTIMEOUT);
        }
        break;

	case MU_MDU_MMS:
        if (ErrType == MU_ERR_SUCC)
        {
            if(pHandleList)
            {
                HWND hLst;
                int  nCount, i;
                PMU_CHAINNODE pNode = NULL;
                
                hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                
                nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
                
                for(i=0; i<nCount; i++)
                {
                    pNode = (PMU_CHAINNODE)SendMessage(hLst, LB_GETITEMDATA, i, 0);
                    
                    if(pNode && pNode->handle == pMsgNode->handle && 
                        (pNode->msgtype == MU_MSG_MMS_DELIVER_REPORT
                        || pNode->msgtype == MU_MSG_MMS))
                    {
                        break;
                    }
                    pNode = NULL;
                }
                
                if(pNode != NULL)
                    pdwArray = GetPosInArray((DWORD)pNode,pHandleList,nMsgCount);                
            }

            if(SendMessage(hWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_MMS),(LPARAM)pMsgNode->handle))
            {
                SendMessage(hWnd,PWM_MSG_MU_NEWMTMSG,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_MMS),(LPARAM)pMsgNode);
                
                if(pHandleList)
                {     
                    if(pdwArray != NULL)
                    {    
                        HWND hLst;
                        int  nCount, i;
                        PMU_CHAINNODE pNode = NULL;
                        
                        hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                        
                        nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
                        
                        for(i=0; i<nCount; i++)
                        {
                            pNode = (PMU_CHAINNODE)SendMessage(hLst, LB_GETITEMDATA, i, 0);
                            
                            if(pNode && pNode->handle == pMsgNode->handle && 
                                (pNode->msgtype == MU_MSG_MMS_DELIVER_REPORT
                                || pNode->msgtype == MU_MSG_MMS))
                            {
                                break;
                            }
                            pNode = NULL;
                        }
                        
                        *pdwArray = (DWORD)pNode;
                        pdwArray = NULL;
                    }
                }
            }
        }
        else
        {
            ;//PLXTipsWin((char*)IDS_CHANGESTATEFAILED, (char*)IDS_WARNING, WAITTIMEOUT);
        }
		break;

	case MU_MDU_EMAIL:
		if (ErrType == MU_ERR_SUCC)
        {
            if(pHandleList)
            {
                HWND hLst;
                int  nCount, i;
                PMU_CHAINNODE pNode = NULL;
                
                hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                
                nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
                
                for(i=0; i<nCount; i++)
                {
                    pNode = (PMU_CHAINNODE)SendMessage(hLst, LB_GETITEMDATA, i, 0);
                    
                    if(pNode && pNode->handle == pMsgNode->handle && pNode->msgtype == MU_MSG_EMAIL)
                    {
                        break;
                    }

                    pNode = NULL; 
                }
                
                if(pNode != NULL)
                    pdwArray = GetPosInArray((DWORD)pNode,pHandleList,nMsgCount);                
            }

            if(SendMessage(hWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_EMAIL),(LPARAM)pMsgNode->handle))
            {
                SendMessage(hWnd,PWM_MSG_MU_NEWMTMSG,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_EMAIL),(LPARAM)pMsgNode);
                
                if(pHandleList)
                {     
                    if(pdwArray != NULL)
                    {    
                        HWND hLst;
                        int  nCount, i;
                        PMU_CHAINNODE pNode = NULL;
                        
                        hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                        
                        nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
                        
                        for(i=0; i<nCount; i++)
                        {
                            pNode = (PMU_CHAINNODE)SendMessage(hLst, LB_GETITEMDATA, i, 0);
                            
                            if(pNode && pNode->handle == pMsgNode->handle && pNode->msgtype == MU_MSG_EMAIL)
                            {
                                break;
                            }

                            pNode = NULL;
                        }
                        
                        *pdwArray = (DWORD)pNode;
                        pdwArray = NULL;
                    }
                }
            }
        }
        else
        {
            ;//PLXTipsWin((char*)IDS_CHANGESTATEFAILED, (char*)IDS_WARNING, WAITTIMEOUT);
        }
		break;

	case MU_MDU_PUSH:
		if (ErrType == MU_ERR_SUCC)
		{
            if(pHandleList)
            {
                HWND hLst;
                int  nCount, i;
                PMU_CHAINNODE pNode = NULL;
                
                hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                
                nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
                
                for(i=0; i<nCount; i++)
                {
                    pNode = (PMU_CHAINNODE)SendMessage(hLst, LB_GETITEMDATA, i, 0);
                    
                    if(pNode && pNode->handle == pMsgNode->handle)
                    {
                        break;
                    }
                    pNode = NULL;
                }
                
                if(pNode != NULL)
                    pdwArray = GetPosInArray((DWORD)pNode,pHandleList,nMsgCount);                
            }

            if(SendMessage(hWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_PUSH),(LPARAM)pMsgNode->handle))
            {
                SendMessage(hWnd,PWM_MSG_MU_NEWMTMSG,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_PUSH),(LPARAM)pMsgNode);	
                
                if(pHandleList)
                {     
                    if(pdwArray != NULL)
                    {    
                        HWND hLst;
                        int  nCount, i;
                        PMU_CHAINNODE pNode = NULL;
                        
                        hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                        
                        nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
                        
                        for(i=0; i<nCount; i++)
                        {
                            pNode = (PMU_CHAINNODE)SendMessage(hLst, LB_GETITEMDATA, i, 0);
                            
                            if(pNode && pNode->handle == pMsgNode->handle)
                            {
                                break;
                            }
                            pNode = NULL;
                        }
                        
                        *pdwArray = (DWORD)pNode;
                        pdwArray = NULL;
                    }
                }
            }
		}
		else
		{
			;//PLXTipsWin((char*)IDS_CHANGESTATEFAILED, (char*)IDS_WARNING, WAITTIMEOUT);
 		}
		break;

    case MU_MDU_FOLDER:
        if (ErrType == MU_ERR_SUCC)
        {
            SendMessage(hWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_FOLDER),(LPARAM)pMsgNode->handle);
            SendMessage(hWnd,PWM_MSG_MU_NEWMTMSG,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_FOLDER),(LPARAM)pMsgNode);
        }
        else
        {
            ;//PLXTipsWin((char*)IDS_CHANGESTATEFAILED, (char*)IDS_WARNING, WAITTIMEOUT);
        }
        break;

	case MU_MDU_BT:
		if (ErrType == MU_ERR_SUCC)
        {
            if(pHandleList)
            {
                HWND hLst;
                int  nCount, i;
                PMU_CHAINNODE pNode = NULL;
                
                hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                
                nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
                
                for(i=0; i<nCount; i++)
                {
                    pNode = (PMU_CHAINNODE)SendMessage(hLst, LB_GETITEMDATA, i, 0);
                    
                    if(pNode && pNode->handle == pMsgNode->handle &&
                        (pNode->msgtype == MU_MSG_BT 
                        || pNode->msgtype == MU_MSG_BT_VCARD
                        || pNode->msgtype == MU_MSG_BT_VCAL
                        || pNode->msgtype == MU_MSG_BT_NOTEPAD
                        || pNode->msgtype == MU_MSG_BT_PICTURE))
                    {
                        break;
                    }
                    pNode = NULL;
                }
                
                if(pNode != NULL)
                    pdwArray = GetPosInArray((DWORD)pNode,pHandleList,nMsgCount);                
            }

            if(SendMessage(hWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_BT),(LPARAM)pMsgNode->handle))
            {
                SendMessage(hWnd,PWM_MSG_MU_NEWMTMSG,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_BT),(LPARAM)pMsgNode);
                
                if(pHandleList)
                {     
                    if(pdwArray != NULL)
                    {    
                        HWND hLst;
                        int  nCount, i;
                        PMU_CHAINNODE pNode = NULL;
                        
                        hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
                        
                        nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
                        
                        for(i=0; i<nCount; i++)
                        {
                            pNode = (PMU_CHAINNODE)SendMessage(hLst, LB_GETITEMDATA, i, 0);
                            
                            if(pNode && pNode->handle == pMsgNode->handle &&
                                (pNode->msgtype == MU_MSG_BT 
                                || pNode->msgtype == MU_MSG_BT_VCARD
                                || pNode->msgtype == MU_MSG_BT_VCAL
                                || pNode->msgtype == MU_MSG_BT_NOTEPAD
                                || pNode->msgtype == MU_MSG_BT_PICTURE))
                            {
                                break;
                            }
                            pNode = NULL;
                        }
                        
                        *pdwArray = (DWORD)pNode;
                        pdwArray = NULL;
                    }
                }
            }
        }
        else
        {
            ;//PLXTipsWin((char*)IDS_CHANGESTATEFAILED, (char*)IDS_WARNING, WAITTIMEOUT);
        }
		break;

	}
}
/*********************************************************************\
* Function	   On_mu_detall_resp
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void On_mu_detall_resp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	MU_LISTCREATEDATA* pData;

	pData = (MU_LISTCREATEDATA*)GetUserData(hWnd);

	switch (HIWORD(wParam)) 
	{
	case MU_MDU_SMS:
		break;

	case MU_MDU_MMS:
		break;

	case MU_MDU_PUSH:
		break;

	case MU_MDU_BT:
		break;
	}
}

/*********************************************************************\
* Function	   On_mu_newmtmsg
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void On_mu_newmtmsg(HWND hWnd, int nMduType, int ErrType, MU_MsgNode *pMsgNode)
{
    int nInsert;
    HWND hLst;
    PMU_CHAINNODE p;
    PMU_LISTCREATEDATA pCreateData;
    SYSTEMTIME syLocal;

    hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
    pCreateData = GetUserData(hWnd);
            
	switch (nMduType) 
	{
	case MU_MDU_SMS:
		if (ErrType == MU_ERR_SUCC)
        {	      
            memset(&syLocal,0,sizeof(SYSTEMTIME));
            GetLocalTime(&syLocal);     
            
            if(pMsgNode->status == MU_STU_UNREAD)
                nInsert = MU_BuildChain(&(pCreateData->pUnReadFirst), pMsgNode,&p);
            else
                nInsert = MU_BuildChain(&(pCreateData->pFirst), pMsgNode,&p);
            
            if(nInsert == -1)
                return;
            
			if(pMsgNode->status == MU_STU_UNREAD)
				pCreateData->mu_counter.nUnread++;
			else
				nInsert += pCreateData->mu_counter.nUnread;

            MU_AddNodeToList(hLst,pMsgNode,p,nInsert,syLocal);
            
            pCreateData->mu_counter.nSMS++;

            pCreateData->bChangeMenu = TRUE;

            SendMessage(hLst,LB_SETCURSEL,nInsert,0);

            if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                MU_SetSoftLeft(hWnd);
		}
		else
		{
			;//PLXTipsWin((char*)IDS_READSMSFAILED, (char*)IDS_PROMPT, WAITTIMEOUT);
		}
		break;

	case MU_MDU_MMS:
		if (ErrType == MU_ERR_SUCC)
		{	      
            memset(&syLocal,0,sizeof(SYSTEMTIME));
            GetLocalTime(&syLocal);  

            if(pMsgNode->status == MU_STU_UNREAD)
                nInsert = MU_BuildChain(&(pCreateData->pUnReadFirst), pMsgNode,&p);
            else
                nInsert = MU_BuildChain(&(pCreateData->pFirst), pMsgNode,&p);     
            
            if(nInsert == -1)
                return;
            
            if(pMsgNode->status == MU_STU_UNREAD)
				pCreateData->mu_counter.nUnread++;
			else
				nInsert += pCreateData->mu_counter.nUnread;

            MU_AddNodeToList(hLst,pMsgNode,p,nInsert,syLocal);
            			
			pCreateData->mu_counter.nMMS++;
            
            pCreateData->bChangeMenu = TRUE;

            SendMessage(hLst,LB_SETCURSEL,nInsert,0);

            if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                MU_SetSoftLeft(hWnd);
		}
		else
		{
			;//PLXTipsWin((char*)IDS_READMMSFAILED, (char*)IDS_PROMPT, WAITTIMEOUT);
		}
		break;

	case MU_MDU_EMAIL:
        if(pCreateData->bEmailBox)// emailbox
        {
            pCreateData->bChangeMenu = TRUE;//
        }
        else
        {
            if (ErrType == MU_ERR_SUCC)
            {	      
                memset(&syLocal,0,sizeof(SYSTEMTIME));
                GetLocalTime(&syLocal);     
                
                if(pMsgNode->status == MU_STU_UNREAD)
                    nInsert = MU_BuildChain(&(pCreateData->pUnReadFirst), pMsgNode,&p);
                else
                    nInsert = MU_BuildChain(&(pCreateData->pFirst), pMsgNode,&p);
                
                if(nInsert == -1)
                    return;
            
                if(pMsgNode->status == MU_STU_UNREAD)
					pCreateData->mu_counter.nUnread++;
				else
					nInsert += pCreateData->mu_counter.nUnread;
                
                MU_AddNodeToList(hLst,pMsgNode,p,nInsert,syLocal);
                
                pCreateData->mu_counter.nEmail++;
                
                pCreateData->bChangeMenu = TRUE;

                SendMessage(hLst,LB_SETCURSEL,nInsert,0);

                if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                    MU_SetSoftLeft(hWnd);
            }
            else
            {
                ;//PLXTipsWin((char*)IDS_READEMAILFAILED, (char*)IDS_PROMPT, WAITTIMEOUT);
            }
        }
		break;

    case MU_MDU_PUSH:
        if (ErrType == MU_ERR_SUCC)
        {				        
            memset(&syLocal,0,sizeof(SYSTEMTIME));
            GetLocalTime(&syLocal);       
            
            if(pMsgNode->status == MU_STU_UNREAD)
                nInsert = MU_BuildChain(&(pCreateData->pUnReadFirst), pMsgNode,&p);
            else
                nInsert = MU_BuildChain(&(pCreateData->pFirst), pMsgNode,&p);
            
            if(nInsert == -1)
                return;
            
            if(pMsgNode->status == MU_STU_UNREAD)
				pCreateData->mu_counter.nUnread++;
			else
				nInsert += pCreateData->mu_counter.nUnread;

            MU_AddNodeToList(hLst,pMsgNode,p,nInsert,syLocal);
            
            pCreateData->mu_counter.nPush++;
            
            pCreateData->bChangeMenu = TRUE;

            SendMessage(hLst,LB_SETCURSEL,nInsert,0);

            if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                MU_SetSoftLeft(hWnd);
        }
        else
        {
            ;//PLXTipsWin((char*)IDS_READPUSHFAILED, (char*)IDS_PROMPT, WAITTIMEOUT);
        }			
        
        break;

	case MU_MDU_BT:
        if (ErrType == MU_ERR_SUCC)
        {				        
            memset(&syLocal,0,sizeof(SYSTEMTIME));
            GetLocalTime(&syLocal);       
            
            if(pMsgNode->status == MU_STU_UNREAD)
                nInsert = MU_BuildChain(&(pCreateData->pUnReadFirst), pMsgNode,&p);
            else
                nInsert = MU_BuildChain(&(pCreateData->pFirst), pMsgNode,&p);
            
            if(nInsert == -1)
                return;
            
            if(pMsgNode->status == MU_STU_UNREAD)
				pCreateData->mu_counter.nUnread++;
			else
				nInsert += pCreateData->mu_counter.nUnread;

            MU_AddNodeToList(hLst,pMsgNode,p,nInsert,syLocal);
            
            pCreateData->mu_counter.nBT ++;
            
            pCreateData->bChangeMenu = TRUE;

            SendMessage(hLst,LB_SETCURSEL,nInsert,0);

            if(hWnd == GetWindow(hMuFrame,GW_CHILD))
                MU_SetSoftLeft(hWnd);
        }
        else
        {
            ;//PLXTipsWin((char*)IDS_READPUSHFAILED, (char*)IDS_PROMPT, WAITTIMEOUT);
        }			
        
        break;

    case MU_MDU_FOLDER:
        if (ErrType == MU_ERR_SUCC)
        {				      
            memset(&syLocal,0,sizeof(SYSTEMTIME));
            GetLocalTime(&syLocal);
            
            if(pMsgNode->status == MU_STU_UNREAD)
                nInsert = MU_BuildChain(&(pCreateData->pUnReadFirst), pMsgNode,&p);
            else
                nInsert = MU_BuildChain(&(pCreateData->pFirst), pMsgNode,&p);
            
            if(nInsert == -1)
                return;
            
            if(pMsgNode->status == MU_STU_UNREAD)
				pCreateData->mu_counter.nUnread++;
			else
				nInsert += pCreateData->mu_counter.nUnread;

            MU_AddNodeToList(hLst,pMsgNode,p,nInsert,syLocal);
                        
            pCreateData->bChangeMenu = TRUE;
            
            SendMessage(hLst,LB_SETCURSEL,nInsert,0);
        }
        else
        {
            ;//PLXTipsWin((char*)IDS_READPUSHFAILED, (char*)IDS_PROMPT, WAITTIMEOUT);
        }			
        break;

    default:
        break;
    }	
}

/*********************************************************************\
* Function	   On_mu_daemon
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void On_mu_daemon(HWND hWnd, int nMduType, int nErrType, MU_Daemon_Notify* daemon)
{
	PMU_LISTCREATEDATA pCreateData;
	PMU_CHAINNODE p;
	int index;
	DWORD dwDateTime;
	SYSTEMTIME sy;
	char szTimestamp [100];
	char sztemp [MU_SUBJECT_DIS_LEN+100+2];
	SYSTEMTIME syLocal;
	HWND hLst;

	pCreateData = GetUserData(hWnd);
	
	p = pCreateData->pUnReadFirst;
	
	while(p)
	{
		if(p->handle == daemon->handle)
			break;

		p = p->pNext;
	}

	if(p == NULL)
	{
		p = pCreateData->pFirst;

		while(p)
		{
			if(p->handle == daemon->handle)
				break;
			
			p = p->pNext;
		}
	}

	if(p == NULL)
		return;

	index = Mu_GetIndexInList(hWnd, p);
	if(index == -1)
		return;

	if(daemon->type == MU_SEND_START)
		p->status = MU_STU_SENDING;
	else if(daemon->type == MU_SEND_WAITING)
		p->status = MU_STU_WAITINGSEND;
	else if(daemon->type == MU_SEND_FAILURE)
		p->status = MU_STU_UNSENT;
	else if(daemon->type == MU_RECV_START)
		p->status = MU_STU_RECEIVING;
	else if(daemon->type == MU_RECV_FAILURE)
		p->status = MU_STU_UNRECEIVED;
	else if(daemon->type == MU_SEND_SUSPENDED)
		p->status = MU_STU_SUSPENDED;
	else
		return;
	
	dwDateTime = p->maskdate;

    sy.wYear = (WORD)INTYEAR(dwDateTime);
    sy.wMonth = (WORD)INTMONTH(dwDateTime);
    sy.wDay = (WORD)INTDAY(dwDateTime);
    sy.wHour = (WORD)INTHOUR(dwDateTime);
    sy.wMinute = (WORD)INTMINUTE(dwDateTime);
    sy.wSecond = (WORD)INTSECOND(dwDateTime);
    
	sztemp[0] = 0;

	GetLocalTime(&syLocal);   

    if(sy.wYear != syLocal.wYear)
        sprintf(szTimestamp,"%d",sy.wYear);
    else if(sy.wMonth != syLocal.wMonth || sy.wDay != syLocal.wDay)
    {
        sy.wYear = 0;
        GetTimeDisplay(sy,sztemp,szTimestamp);
    }
    else
        GetTimeDisplay(sy,szTimestamp,sztemp);
	
	hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);

	switch(p->msgtype)
	{
	case MU_MSG_SMS:
	case MU_MSG_VCARD:
	case MU_MSG_VCAL: 
		switch(p->status)
		{
		case MU_STU_SENDING:		//Outbox
            SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
			sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_SENDING);
            SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
            break;
			
		case MU_STU_SUSPENDED:		//Outbox
            SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_SUSPENDED);
			SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
			break;
		
		case MU_STU_WAITINGSEND:	//Outbox
            SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_WAITING);
			SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
			break;

        case MU_STU_UNSENT:			//Outbox
            SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_FAIL]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_FAILED);
			SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
            break;
		}
		break;

	case MU_MSG_MMS:
		{
			switch(p->status)
			{
			case MU_STU_UNRECEIVED:
				SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_MMS_READ]);
				//SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
				break;
				
			case MU_STU_RECEIVING:
				SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_MMS_READ]);
				//SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
				break;
				
			case MU_STU_SENDING:		//Outbox
				SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
				sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_SENDING);
				SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
				break;
				
			case MU_STU_SUSPENDED:		//Outbox
				SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
				sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_SUSPENDED);
				SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
				break;
				
			case MU_STU_WAITINGSEND:	//Outbox
				SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
				sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_WAITING);
				SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
				break;
				
			case MU_STU_UNSENT:			//Outbox
				SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_FAIL]);
				sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_FAILED);
				SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
				break;
				
			}
		
		}
		break;
	case MU_MSG_EMAIL:
		switch(p->status)
		{
		case MU_STU_SENDING:		//Outbox
            SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
			sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_SENDING);
            SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
            break;
			
		case MU_STU_SUSPENDED:		//Outbox
            SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_SUSPENDED);
			SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
			break;

		case MU_STU_DEFERMENT:
		case MU_STU_WAITINGSEND:	//Outbox
            SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_WAITING);
			SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
			break;

        case MU_STU_UNSENT:			//Outbox
            SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)mu_hBmp[GIF_MU_FAIL]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_FAILED);
			SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)sztemp);
            break;
		}
		break;

	}
    
    if(hWnd == GetWindow(hMuFrame,GW_CHILD))
        MU_SetSoftLeft(hWnd);
}

/*********************************************************************\
* Function	   mu_OnDaemon
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void mu_OnDaemon(LPARAM lParam, HBITMAP hBmp, int nType)
{
//	int i;
//	for (i = 0; i < nIndexCount; i++)
//	{
//		if (((MU_Daemon_Notify*)lParam)->handle == mu_index[i].handle && 
//			mu_index[i].nType == MU_MDU_MMS)
//		{
//			switch (nType)
//			{
//			case MU_RECV_START:
//				mu_index[i].byStatus = MU_STU_RECEIVING;
//				break;
//
//			case MU_RECV_SUCCESS:
//				mu_index[i].byStatus = MU_STU_UNREAD;
//				break;
//
//			case MU_RECV_FAILURE:
//				mu_index[i].byStatus = MU_STU_UNRECEIVED;
//				break;
//				
//			case MU_SEND_START:				
//				mu_index[i].byStatus = MU_STU_SENDING;
//				break;
//
//			case MU_SEND_FAILURE:
//				mu_index[i].byStatus = MU_STU_UNSENT;
//				break;
//			}
//
//			SendMessage(hWndList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)i),
//				(LPARAM)hBmp);
//			
//			break;
//		}
//	}
}

/*********************************************************************\
* Function	   On_mu_movetofolder
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void On_mu_movetofolder(HWND hWnd,BOOL bMove,int nNewFolder)
{
    PMU_CHAINNODE pChainNode = NULL;
    PMU_LISTCREATEDATA pCreateData;

	pCreateData = GetUserData(hWnd);

    pChainNode = pCreateData->pSelect;

    pCreateData->pSelect = NULL;

    if(bMove == FALSE)
        return;

    if(nNewFolder == -1)
        return;
    
    if(pChainNode == NULL)
        return;

    switch(pChainNode->msgtype) 
    {
	case MU_MSG_VCARD:
	case MU_MSG_VCAL:
    case MU_MSG_SMS:
    case MU_MSG_STATUS_REPORT:
        if(mu_initstatus.bSMS)
		{
			printf("mu_sms_interface->msg_move_message pChainNode->handle=%d  nNewFolder = %d\r\n",
				pChainNode->handle, nNewFolder);
            if(mu_sms_interface->msg_move_message(hWnd,pChainNode->handle,(int)nNewFolder))
			{
				printf("mu_sms_interface->msg_move_message success!!!\r\n");
                SendMessage(hWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),
                (LPARAM)(pChainNode->handle));
				printf("PWM_MSG_MU_DELETE Sended\r\n");
			}
		}
            break;
            
    case MU_MSG_MMS:
    case MU_MSG_MMS_DELIVER_REPORT:
        if(mu_initstatus.bMMS)
            if(mu_mms_interface->msg_move_message(hWnd,pChainNode->handle,(int)nNewFolder))
                SendMessage(hWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_MMS),
                (LPARAM)(pChainNode->handle));
            break;
            
    case MU_MSG_EMAIL:
        if(mu_initstatus.bEmail)
            if(mu_email_interface->msg_move_message(hWnd,pChainNode->handle,(int)nNewFolder))
                SendMessage(hWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_EMAIL),
                (LPARAM)(pChainNode->handle));
            break;
            
    case MU_MSG_PUSH:
        if(mu_initstatus.bPush)
            if(mu_push_interface->msg_move_message(hWnd,pChainNode->handle,(int)nNewFolder))
                SendMessage(hWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_PUSH),
                (LPARAM)(pChainNode->handle));
            break;
	case MU_MSG_BT:
	case MU_MSG_BT_VCARD:
	case MU_MSG_BT_VCAL:
	case MU_MSG_BT_NOTEPAD:
	case MU_MSG_BT_PICTURE:
		if(mu_initstatus.bBT)
            if(mu_bt_interface->msg_move_message(hWnd,pChainNode->handle,(int)nNewFolder))
                SendMessage(hWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_BT),
                (LPARAM)(pChainNode->handle));
		break;
            
    default:
    case MU_MSG_EMAILHEADER://need modify
        break;
    }
	if(nNewFolder != MU_DRAFT)
		PLXTipsWin(NULL, NULL, 0, (char*)IDS_MESSAGES_MOVED, NULL, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);
	else
		PLXTipsWin(NULL, NULL, 0, (char*)IDS_MESSAGES_MOVED_TODRAFT, NULL, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);
}
/*********************************************************************\
* Function	   MU_AddNodeToList
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void MU_AddNodeToList(HWND hWnd,MU_MsgNode *pmsgnode,MU_CHAINNODE *pNewNode,int nInsert,SYSTEMTIME syLocal)
{
    char sztemp[MU_SUBJECT_DIS_LEN+100+2];
    int curIndex;
    char szTimestamp[100]; 
	char szName[256];
    SYSTEMTIME sy;
    DWORD dwDateTime;

    if(IsWindowVisible(hWnd) == FALSE)
    {
        ShowWindow(hWnd,SW_SHOW);
        // UpdateWindow(hWnd);
    }

    sztemp[0] = 0;
    memset(&sy,0,sizeof(SYSTEMTIME));

    dwDateTime = pmsgnode->maskdate;
    sy.wYear = (WORD)INTYEAR(dwDateTime);
    sy.wMonth = (WORD)INTMONTH(dwDateTime);
    sy.wDay = (WORD)INTDAY(dwDateTime);
    sy.wHour = (WORD)INTHOUR(dwDateTime);
    sy.wMinute = (WORD)INTMINUTE(dwDateTime);
    sy.wSecond = (WORD)INTSECOND(dwDateTime);
    
    if(sy.wYear != syLocal.wYear)
        sprintf(szTimestamp,"%d",sy.wYear);
    else if(sy.wMonth != syLocal.wMonth || sy.wDay != syLocal.wDay)
    {
        sy.wYear = 0;
        GetTimeDisplay(sy,sztemp,szTimestamp);
    }
    else
        GetTimeDisplay(sy,szTimestamp,sztemp);
    
    sztemp[0] = 0;
    sprintf(sztemp,"%s %s",szTimestamp,pmsgnode->subject);

	MU_GetName(szName, pmsgnode->addr);

    switch(pmsgnode->msgtype) 
    {
    case MU_MSG_SMS:
	case MU_MSG_VCARD:
	case MU_MSG_VCAL:
        
        switch(pmsgnode->status) 
        {
        case MU_STU_UNREAD:
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SMS_UNREAD]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
            
        case MU_STU_READ:
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SMS_READ]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;

        case MU_STU_SENDING:		//Outbox
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
			sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_SENDING);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
			
		case MU_STU_SUSPENDED:		//Outbox
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_SUSPENDED);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;
		
		case MU_STU_WAITINGSEND:	//Outbox
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_WAITING);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;

        case MU_STU_UNSENT:			//Outbox
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_FAIL]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_FAILED);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;

        case MU_STU_DRAFT:
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_DRAFT]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;

        case MU_STU_SENT:
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SENT]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
            
        default:
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SMS_READ]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
        }

        break;
        
    case MU_MSG_STATUS_REPORT:
        
        switch(pmsgnode->status) 
        {               
        case MU_STU_WAITING:
            sztemp[0] = 0;
            sprintf(sztemp,"%s %s",szTimestamp,IDS_PENDING);
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_REPORT_PENDING]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;

        case MU_STU_CONFIRM:
            sztemp[0] = 0;
            sprintf(sztemp,"%s %s",szTimestamp,IDS_DELIEVED);
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_REPORT_DELIEVERED]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
		
		case MU_STU_FAILED:
            sztemp[0] = 0;
            sprintf(sztemp,"%s %s",szTimestamp,IDS_FAILED);
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_REPORT_FAILED]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
			
        default:
            break;
        }
        break;
        
    case MU_MSG_MMS:
		switch(pmsgnode->status)
		{
		case MU_STU_UNRECEIVED:
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
			SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_FAIL]);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;
		
		case MU_STU_RECEIVING:
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
			SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_MMS_READ]);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;

		case MU_STU_READ:
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
			SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_MMS_READ]);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;
			
		case MU_STU_UNREAD:
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
			SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_MMS_UNREAD]);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;

		case MU_STU_SENDING:		//Outbox
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
			sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_SENDING);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
			
		case MU_STU_SUSPENDED:		//Outbox
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_SUSPENDED);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;
		
		case MU_STU_WAITINGSEND:	//Outbox
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_WAITING);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;

        case MU_STU_UNSENT:			//Outbox
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_FAIL]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_FAILED);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
		
		case MU_STU_DRAFT:
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_DRAFT]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;

        case MU_STU_SENT:
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SENT]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
			
		default:
			break;
		}
		break;
            
    case MU_MSG_MMS_DELIVER_REPORT:
        switch(pmsgnode->status) 
        {               
        case MU_STU_WAITING:
            sztemp[0] = 0;
            sprintf(sztemp,"%s %s",szTimestamp,IDS_PENDING);
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_REPORT_PENDING]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;

        case MU_STU_CONFIRM:
            sztemp[0] = 0;
            sprintf(sztemp,"%s %s",szTimestamp,IDS_DELIEVED);
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_REPORT_DELIEVERED]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
		
		case MU_STU_FAILED:
            sztemp[0] = 0;
            sprintf(sztemp,"%s %s",szTimestamp,IDS_FAILED);
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_REPORT_FAILED]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
			
        default:
            break;
        }
        break;
		//SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_DR]);
		break;
            
    case MU_MSG_EMAIL:
		switch(pmsgnode->status)
		{
		case MU_STU_READ:
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
			SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_EMAIL_READ]);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;
			
		case MU_STU_UNREAD:
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
			SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_EMAIL_UNREAD]);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;
			
		case MU_STU_SENDING:		//Outbox
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
			sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_SENDING);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;

		case MU_STU_SUSPENDED:		//Outbox
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_SUSPENDED);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;
		
		case MU_STU_DEFERMENT:	//Outbox
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_WAITING);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;

		case MU_STU_WAITINGSEND:	//Outbox
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SENDING]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_WAITING);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;

        case MU_STU_UNSENT:			//Outbox
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_FAIL]);
            sprintf(sztemp,"%s %s",szTimestamp,IDS_MSG_FAILED);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
			
		case MU_STU_DRAFT:
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_DRAFT]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;

        case MU_STU_SENT:
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_SENT]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
			
		default:
			break;
		}

		break;
            

	case MU_MSG_BT:
	case MU_MSG_BT_VCARD:
	case MU_MSG_BT_VCAL:
	case MU_MSG_BT_NOTEPAD:
	case MU_MSG_BT_PICTURE:
		switch(pmsgnode->status)
		{
		case MU_STU_READ:
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
			SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_BT_READ]);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;
			
		case MU_STU_UNREAD:
			curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
			SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_BT_UNREAD]);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
			break;
			
		default:
			break;
		}
		
		break;
/*
		    case MU_MSG_EMAILHEADER:
				curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/ *pmsgnode->addr* /);
				SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)hBmp);
				SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
				break;*/
		
            
    case MU_MSG_PUSH:
		curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/*pmsgnode->addr*/);
		SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)hBmp);
		SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
		break;

/*
    case MU_MSG_FOLDER:
        switch(pmsgnode->status)
        {
        case MU_STU_PHRASE:
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/ *pmsgnode->addr* /);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_FOLDER]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)"Phrase");
            break;
            
        case MU_STU_DIR:
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/ *pmsgnode->addr* /);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_FOLDER]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)"Folder");
            break;
            
        default:
            curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)szName/ *pmsgnode->addr* /);
            SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)mu_hBmp[GIF_MU_FOLDER]);
            SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);
            break;
        }
        break;*/

        
    default:
        break;
    }
    
    SendMessage(hWnd, LB_SETITEMDATA, (WPARAM)curIndex, (LPARAM)pNewNode);
}
/*********************************************************************\
* Function	   MU_BuildChain
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int MU_BuildChain(PMU_CHAINNODE *ppFirst,MU_MsgNode *pmsgnode, PMU_CHAINNODE* ppNewNode)
{
    int nInsert;

    *ppNewNode = MU_NewNode(); // new a node ,add to list
    
    if( !*ppNewNode )
    { 
        return -1; // warning
    }
    
    MU_FillNode(*ppNewNode,pmsgnode);

    if(pmsgnode->msgtype == MU_MSG_FOLDER)
        nInsert = MU_InsetNode(ppFirst,*ppNewNode,MU_Compare_Name); 
    else
        nInsert = MU_InsetNode(ppFirst,*ppNewNode,MU_Compare_DateTime); // index start with 0

    return nInsert;
}
/*********************************************************************\
* Function	   MU_NewNode
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static MU_CHAINNODE* MU_NewNode(void)
{
    MU_CHAINNODE* p = NULL;

    //p = (MU_CHAINNODE*)MM_malloc(sizeof(MU_CHAINNODE));
    p = (MU_CHAINNODE*)MM_malloc();
    memset(p,0,sizeof(MU_CHAINNODE));

    return p;
}
/*********************************************************************\
* Function	   MU_FillNode
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL MU_FillNode(MU_CHAINNODE* pChainNode,MU_MsgNode* pMsgNode)
{
    if(!pChainNode || !pMsgNode)
        return FALSE;

    pChainNode->maskdate = pMsgNode->maskdate;
    pChainNode->handle = pMsgNode->handle;
    pChainNode->msgtype = pMsgNode->msgtype;
    pChainNode->status = pMsgNode->status;
    pChainNode->storage_type = pMsgNode->storage_type;
	strncpy(pChainNode->addr, pMsgNode->addr, 19);
	pChainNode->addr[19] = 0;
    return TRUE;
}
/*********************************************************************\
* Function	   MU_InsetNode
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
/*
static int MU_InsetNode(MU_CHAINNODE** pFirst,MU_CHAINNODE* pChainNode,CMPFUNC pCmpFunc)
{
    if(*pFirst == NULL)
    {
        *pFirst = pChainNode;
        pChainNode->pPioneer = NULL;
        pChainNode->pNext = NULL;

        return 0;
    }
    else
    {
        MU_CHAINNODE *p,*end,*start,*pioneer,*next;
        int k;
        BOOL bIsTail;

        k = 0;

        switch(pChainNode->msgtype)
        {
        case MU_MSG_FOLDER:
            if(pChainNode->status == MU_STU_DIR)
            {
                pioneer = *pFirst;
                start = (*pFirst)->pNext;

                k++;
         
                if( start == NULL)
                {   
                    pChainNode->pPioneer = pioneer;
                    pChainNode->pNext = pioneer->pNext;
                    pioneer->pNext = pChainNode;
                    return k;
                }

                if(start->status != MU_STU_DIR)
                {
                    pChainNode->pPioneer = pioneer;
                    pChainNode->pNext = pioneer->pNext;
                    pioneer->pNext = pChainNode;
                    return k;
                }

                p = start;
                end = start;
                             
                while( p->status == MU_STU_DIR )
                {
                    if( p->pNext )
                    {
                        if(p->pNext->status == MU_STU_DIR)
                        {
                            p = p->pNext;
                            end = p;
                        }
                        else
                            break;
                    }
                    else
                        break;
                }

                next = end->pNext;
            }
        	break;

        default:
            if((*pFirst)->msgtype == MU_MSG_FOLDER)
            {
                start = (*pFirst)->pNext;
                pioneer = *pFirst;
                
                k++;
                
                if( start == NULL)
                {
                    pChainNode->pPioneer = pioneer;
                    pChainNode->pNext = pioneer->pNext;
                    pioneer->pNext = pChainNode;
                    return k;
                }
                
                p = start;
                while( p )
                {
                    if(p->msgtype != MU_MSG_FOLDER)
                        break;
                    
                    k++;
                    pioneer = p;
                    p = p->pNext;
                }
                start = p;
                
                if( start == NULL )
                {
                    pChainNode->pPioneer = pioneer;
                    pChainNode->pNext = NULL;
                    pioneer->pNext = pChainNode;
                    return k;
                }
                
                end = start;
                p = end;
                while( p )
                {
                    if( p->pNext )
                    {
                        end = p->pNext;
                    }
                    p = p->pNext;
                }
                
                pioneer = start->pPioneer;
            }
            else
            {
                start = *pFirst;
                end = NULL;
            }

            break;
        }
        
            
        bIsTail = FALSE;
        p = start;
        while( pCmpFunc(pChainNode->maskdate,p->maskdate) )
        {
            k++;
            if( p->pNext && p != end)
                p = p->pNext;
            else
            {
                bIsTail = TRUE;
                break;
            }
        }

        if( bIsTail )
        {
            p->pNext = pChainNode;
            pChainNode->pNext = NULL;
            pChainNode->pPioneer = p;
        }
        else
        {
            if( p == *pFirst)
            {
                pChainNode->pNext = *pFirst;
                pChainNode->pPioneer = NULL;
                (*pFirst)->pPioneer = pChainNode;
                *pFirst = pChainNode;
            }
            else
            {
                p->pPioneer->pNext = pChainNode;
                pChainNode->pPioneer = p->pPioneer;
                pChainNode->pNext = p;
                p->pPioneer = pChainNode;
            }
        }

        return k;
    }
}*/
static int MU_InsetNode(MU_CHAINNODE** pFirst,MU_CHAINNODE* pChainNode,CMPFUNC pCmpFunc)
{
    if(*pFirst == NULL)
    {
        *pFirst = pChainNode;
        pChainNode->pPioneer = NULL;
        pChainNode->pNext = NULL;

        return 0;
    }
    else
    {
        MU_CHAINNODE *p;
        int k;
        BOOL bIsTail;

        k = 0;
        bIsTail = FALSE;
        p = *pFirst;
        while( pCmpFunc(pChainNode->maskdate,p->maskdate) )
        {
            k++;
            if( p->pNext )
                p = p->pNext;
            else
            {
                bIsTail = TRUE;
                break;
            }
        }

        if( bIsTail )
        {
            p->pNext = pChainNode;
            pChainNode->pNext = NULL;
            pChainNode->pPioneer = p;
        }
        else
        {
            if( p == *pFirst)
            {
                pChainNode->pNext = *pFirst;
                pChainNode->pPioneer = NULL;
                (*pFirst)->pPioneer = pChainNode;
                *pFirst = pChainNode;
            }
            else
            {
                p->pPioneer->pNext = pChainNode;
                pChainNode->pPioneer = p->pPioneer;
                pChainNode->pNext = p;
                p->pPioneer = pChainNode;
            }
        }

        return k;
    }
}
/*********************************************************************\
* Function	   MU_DeleteNode
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void MU_DeleteNode(PMU_CHAINNODE* ppFirst,MU_CHAINNODE* pChainNode)
{
	printf("Enter MU_DeleteNode\r\n");
	printf("ppFirst = 0x%x, pChainNode = 0x%x, pFirst = 0x%x\r\n", ppFirst, pChainNode, *ppFirst);
    if( pChainNode == *ppFirst )
    {
        if(pChainNode->pNext)
        {
            *ppFirst = pChainNode->pNext;
            pChainNode->pNext->pPioneer = NULL;
        }
        else
            *ppFirst = NULL;
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

    MM_free(pChainNode);
    pChainNode = NULL;
}
/*********************************************************************\
* Function	   MU_Erase
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void MU_Erase(PMU_CHAINNODE* ppFirst)
{
    MU_CHAINNODE* p;    
    MU_CHAINNODE* ptemp;

    p = *ppFirst;

    while( p )
    {
        ptemp = p->pNext;       
        MM_free(p);
        p = ptemp;
    }

    *ppFirst = NULL;
}
/*********************************************************************\
* Function	   MU_PhraseLoad
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL MU_PhraseLoad(HWND hWnd)
{    
    MU_MsgNode msgnode;
    PMU_CHAINNODE pchainnode;
    int nInsert;
    HWND hLst;
    MU_LISTCREATEDATA *pCreateData;
    SYSTEMTIME syLocal;
    
    hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
    pCreateData = GetUserData(hWnd);
    
    memset(&msgnode,0,sizeof(MU_MsgNode));
    
    msgnode.msgtype = MU_MSG_FOLDER;
    msgnode.status = MU_STU_PHRASE;
    strcpy(msgnode.subject,IDS_PHRASE);
    
    memset(&syLocal,0,sizeof(SYSTEMTIME));
    GetLocalTime(&syLocal);       
    
    pchainnode = NULL;
        
    nInsert = MU_BuildChain(&(pCreateData->pFirst),&msgnode,&pchainnode);
        
    if(nInsert == -1)
        return FALSE;
        
    MU_AddNodeToList(hLst,&msgnode,pchainnode,nInsert,syLocal);
    
    SendMessage(hLst, LB_SETCURSEL, (WPARAM)0, (LPARAM)NULL);

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      Compare by datetime
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MU_Compare_DateTime(DWORD DateTime1, DWORD DateTime2)
{
    if(DateTime1 > DateTime2) 
        return FALSE;
    else 
        return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      Compare by Name
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MU_Compare_Name(DWORD Name1, DWORD Name2)
{
    if(stricmp((char*)Name1,(char*)Name2) < 0)
        return FALSE;
    else
        return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      CreateSubFolderWnd
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL CreateSubFolderWnd(HWND hParent,MU_FOLDERINFO *pFolderInfo)
{
#define CAPTION_WIDTH           120
    MU_LISTCREATEDATA CreateData;
    HMENU    hMenu;
    int nLen,nFit;
    HDC hdc;
    char pCaption[256];
	RECT rClient;
    
    nLen = strlen(pFolderInfo->szFolderName);
    hdc = GetDC(hParent);
    GetTextExtentExPoint(hdc, pFolderInfo->szFolderName, nLen, CAPTION_WIDTH, &nFit, NULL, NULL);
    ReleaseDC(hParent,hdc);

    memset(pCaption,0,256);
    if(nFit < 255)
        strncpy(pCaption,pFolderInfo->szFolderName,nFit);
    else
        strncpy(pCaption,pFolderInfo->szFolderName,255);
    pCaption[255] = 0;

	
    memset(&CreateData,0,sizeof(MU_LISTCREATEDATA));
	strcpy(CreateData.szFolderName, pCaption);
    CreateData.bChangeMenu = TRUE;
    CreateData.bEmailBox = FALSE;
    CreateData.pFirst = NULL;
    memset(&(CreateData.mu_counter),0,sizeof(MU_COUNTER));
    CreateData.nBoxType = pFolderInfo->nFolderID;
    CreateData.szEmailName[0] = 0;

    hMenu = CreateMenu();
    GetClientRect(hMuFrame, &rClient);
	CreateData.hMenu = hMenu;
    hListWnd = CreateWindow(
        "MUListWndClass",
        pCaption, 
        WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hMuFrame,
        NULL, 
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!hListWnd)
        return FALSE;

	PDASetMenu(hMuFrame, hMenu);
    SetWindowText(hMuFrame, pCaption);
    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      MU_SetSoftLeft
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void MU_SetSoftLeft(HWND hWnd)
{    
    HWND hLst = NULL;
    MU_LISTCREATEDATA* pCreateData;
    int index;
    MU_CHAINNODE* pNode = NULL; 
    char szText[20],szMenu[20];
    
    szText[0] = 0;
    szMenu[0] = 0;
    pCreateData = GetUserData(hWnd);
    
	if(bDirectExit)
		SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
	else
		SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);

    if(pCreateData->bEmailBox == FALSE)
    {
        SendMessage(hMuFrame, PWM_GETBUTTONTEXT, 1, (LPARAM)szText);
        SendMessage(hMuFrame, PWM_GETBUTTONTEXT, 2, (LPARAM)szMenu);

        switch(pCreateData->nBoxType)
        {
        case MU_OUTBOX:
            hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
            index = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(index == LB_ERR)
                SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            else
            {
                pNode = (MU_CHAINNODE*)SendMessage(hLst, LB_GETITEMDATA, index, 0);              
                
                switch(pNode->status) 
                {
				case MU_STU_DEFERMENT:
                case MU_STU_WAITINGSEND:
                    if(strcmp(szText,IDS_SUSPEND) != 0)
                        SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SUSPEND);
                    break;
                case MU_STU_SENDING:
                    if(strlen(szText) != 0)
                        SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                    break;
                    
                case MU_STU_UNSENT:
                case MU_STU_SUSPENDED:
                    if(strcmp(szText,IDS_SEND) != 0)
                        SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SEND);
                    break;
                    
                default:
                    break;
                }     
            }

            if(MU_GetSUM(pCreateData) == 0 /*&& strcmp(szMenu,"") != 0*/)
            {
                SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            }
            else if(MU_GetSUM(pCreateData) != 0 /*&& strcmp(szText,ICON_OPTIONS) != 0*/)
            {
                SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
            }
        	break;

        case MU_DRAFT:
            if(MU_GetSUM(pCreateData) == 0)
            {
//                if(strcmp(szText,IDS_EDIT) == 0)
                    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
//                if(strcmp(szMenu,"") != 0)
                    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            }
            else if(MU_GetSUM(pCreateData) != 0)
            {
//                if(strcmp(szText,IDS_EDIT) != 0)
                    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
//                if(strcmp(szText,ICON_OPTIONS) != 0)
                    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
            }
            break;

        case MU_INBOX:
        case MU_REPORT:
        case MU_SENT:
        default:
            
            if(MU_GetSUM(pCreateData) == 0)
            {
//                if(strcmp(szText,IDS_OPEN) == 0)
                    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
//                if(strcmp(szMenu,"") != 0)
                    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            }
            else if(MU_GetSUM(pCreateData) != 0)
            {
//                if(strcmp(szText,IDS_OPEN) != 0)
                    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
//                if(strcmp(szText,ICON_OPTIONS) != 0)
                    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
            }
            break;
        }
    }
}

static void MU_SelectMsg(HWND hWndOwner, BOOL bMove, BOOL bDel)
{
	WNDCLASS wc;
	PMU_LISTCREATEDATA p;
	MU_SELLISTDATA   SelListData;
	HWND hSelDelWnd;
	RECT rClient;

	p = (PMU_LISTCREATEDATA)GetUserData(hWndOwner);
	SelListData.pOwner = p;
	if(bMove)
		SelListData.SelType = MOVE_SELECT;
	else if(bDel)
		SelListData.SelType = DEL_SELECT;
	SelListData.hListOwner = hWndOwner;

	wc.style         = 0;
	wc.lpfnWndProc   = MUSelDelWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof(MU_SELLISTDATA);
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName  = "MUSelDelWndClass";

	RegisterClass(&wc);
	GetClientRect(hMuFrame, &rClient);

	hSelDelWnd = CreateWindow(
        "MUSelDelWndClass",
        MU_GetCurCaption(p), 
        WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hMuFrame,
        NULL, 
        NULL, 
        (PVOID)&SelListData
        );
	
	SetWindowText(hMuFrame, MU_GetCurCaption(p));
	UpdateWindow(hSelDelWnd);
}

LRESULT MUSelDelWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 1;
	
	switch(wMsgCmd)
	{
	case WM_CREATE:
		{
			PMU_SELLISTDATA  pSelListData;
			
			pSelListData = (PMU_SELLISTDATA)GetUserData(hWnd);

			memcpy(pSelListData, ((LPCREATESTRUCT)(lParam))->lpCreateParams, sizeof(MU_SELLISTDATA));
			
			SelDel_OnCreate(hWnd, pSelListData);
		}
		break;
		
	case PWM_SHOWWINDOW:
		{
			PMU_SELLISTDATA  pSelListData;
			
			pSelListData = (PMU_SELLISTDATA)GetUserData(hWnd);

			SetFocus(GetDlgItem(hWnd, IDC_SELDEL_LIST));

			SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);

			if(pSelListData->SelType == DEL_SELECT)
				SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_DELETE);
			else if(pSelListData->SelType == MOVE_SELECT)
				SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_MOVE);
			
			SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
		}
		break;

	case WM_KEYDOWN:
		SelDel_OnKeyDown(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
 		break;
		
	case WM_CLOSE:
		SendMessage(hMuFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		DestroyWindow(hWnd);
		break;

	case WM_COMMAND:
		SelDel_OnCommand(hWnd,(int)LOWORD(wParam),(UINT)HIWORD(wParam));
		break;

	case WM_SELECTFOLDER:
        SelMoveToFolder(hWnd,(BOOL)wParam,(int)lParam);
        break;

	case WM_DELSELECT:
		{
			int count;
			int* pIndex;
			int i, index;
			PMU_CHAINNODE pChainNode;
			char pCaption[64];

			GetWindowText(hMuFrame, pCaption, 63);
			
			count = SendMessage(GetDlgItem(hWnd, IDC_SELDEL_LIST), LB_GETSELCOUNT, 0, 0);

			if(lParam == 0)
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			}

			WaitWindowStateEx(hWnd, TRUE, (char*)IDS_DELETEING, pCaption, "", (char*)IDS_CANCEL);
			
			pIndex = (int*)malloc(sizeof(int) * count);
			
			SendMessage(GetDlgItem(hWnd, IDC_SELDEL_LIST), LB_GETSELITEMS, count, (LPARAM)pIndex);
			
			for(i = count - 1; i >= 0; i--)
			{	
				index = pIndex[i];
				
				pChainNode = (PMU_CHAINNODE)SendMessage(GetDlgItem(hWnd, IDC_SELDEL_LIST),
					LB_GETITEMDATA,index,0);

				if(MU_DelMultiOneMsg(hListWnd, pChainNode) == -1)
                {   
                    free(pIndex);
                    
                    WaitWindowStateEx(hWnd, FALSE, (char*)IDS_DELETEING, pCaption, "", (char*)IDS_CANCEL);

                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    return lResult;
                }
				
				if(IsUserCancel())
				{
					free(pIndex);
                    
                    WaitWindowStateEx(hWnd, FALSE, (char*)IDS_DELETEING, pCaption, "", (char*)IDS_CANCEL);

					PostMessage(hWnd, WM_CLOSE, 0, 0);
					return lResult;
				}

				//check cancel
				//????????
			}
			
			WaitWindowStateEx(hWnd, FALSE, (char*)IDS_DELETEING, pCaption, "", (char*)IDS_CANCEL);
			
			if(i < 0)
				PLXTipsWin(hMuFrame, NULL, 0, (char*)IDS_DELETED, pCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);
			
			free(pIndex);
			
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;
}

static void SelDel_OnCreate(HWND hWnd, PMU_SELLISTDATA pSelListData)
{
	HWND hWndDelList;
	RECT rc;
	int i, total;
	PMU_LISTCREATEDATA p;
	char szContent[128];

	p = (PMU_LISTCREATEDATA)pSelListData->pOwner;

	GetClientRect(hWnd, &rc);

	hWndDelList = CreateWindow("MULTILISTBOX",
		"",
		WS_CHILD | WS_VSCROLL | LBS_BITMAP,
		rc.left, rc.top,
		rc.right - rc.left, rc.bottom - rc.top,
		hWnd,
		(HMENU)IDC_SELDEL_LIST,
		NULL,
		NULL);
	
	total = MU_GetSUM(p);
	
	for(i = 0; i < total; i++)
	{
		int UserData;
		
		SendMessage(GetDlgItem(hListWnd, IDC_MULSIT_LIST), LB_GETTEXT, (WPARAM)i, (LPARAM)szContent);
		UserData = SendMessage(GetDlgItem(hListWnd, IDC_MULSIT_LIST), LB_GETITEMDATA, (WPARAM)i, 0);
		
		SendMessage(hWndDelList, LB_INSERTSTRING, i, (LPARAM)szContent);
		SendMessage(hWndDelList, LB_SETITEMDATA, i, (LPARAM)UserData);
	}

	SetFocus(hWndDelList);

	SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
	if(pSelListData->SelType == DEL_SELECT)
		SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_DELETE);
	else if(pSelListData->SelType == MOVE_SELECT)
		SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_MOVE);
	SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

	SendMessage(hWndDelList, LB_ENDINIT, 0, 0);
	SendMessage(hWndDelList, LB_SETCURSEL, 0, 0);
	ShowWindow(hWndDelList, SW_SHOW);
}

static void SelDel_OnKeyDown (HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
	LRESULT lResult = 1;

	switch(vk)
	{
	case VK_F10:
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
		
	case VK_RETURN:
		SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDC_OK, 0), 0);
		break;

	default:
		lResult = PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}

static void SelDel_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	PMU_SELLISTDATA  pSelListData;
	pSelListData = (PMU_SELLISTDATA)GetUserData(hWnd);

	switch(id)
	{
	case IDC_EXIT:
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case IDC_OK:
		{
			if(pSelListData->SelType == DEL_SELECT)
				SelDelConfirm(hWnd);
			else if(pSelListData->SelType == MOVE_SELECT)
				SelMoveConfirm(hWnd);
			else
				PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;
	}

	return;
}

int MU_DelOneMsg(HWND hWnd, PMU_CHAINNODE pChainNode)
{
	switch(pChainNode->msgtype) 
	{
	case MU_MSG_VCARD:
	case MU_MSG_VCAL:
	case MU_MSG_SMS:
	case MU_MSG_STATUS_REPORT:
		if(mu_initstatus.bSMS)
			mu_sms_interface->msg_delete_message(hWnd,pChainNode->handle);
		break;
		
	case MU_MSG_MMS:
	case MU_MSG_MMS_DELIVER_REPORT:
		if(mu_initstatus.bMMS)
			mu_mms_interface->msg_delete_message(hWnd,pChainNode->handle);
		break;
		
	case MU_MSG_EMAIL:
		if(mu_initstatus.bEmail)
			mu_email_interface->msg_delete_message(hWnd,pChainNode->handle);
		break;
		
	case MU_MSG_EMAILHEADER://need modify
		if(mu_initstatus.bEmail)
			;//                mu_emailbox_interface->email_delete(hWnd,pChainNode->handle);
		break;
		
	case MU_MSG_PUSH:
		if(mu_initstatus.bPush)
			mu_push_interface->msg_delete_message(hWnd,pChainNode->handle);
		break;

	case MU_MSG_BT:
	case MU_MSG_BT_VCARD:
	case MU_MSG_BT_VCAL:
	case MU_MSG_BT_NOTEPAD:
	case MU_MSG_BT_PICTURE:
		if(mu_initstatus.bBT)
			mu_bt_interface->msg_delete_message(hWnd, pChainNode->handle);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

int MU_DelMultiOneMsg(HWND hWnd, PMU_CHAINNODE pChainNode)
{
	switch(pChainNode->msgtype) 
	{
	case MU_MSG_VCARD:
	case MU_MSG_VCAL:
	case MU_MSG_SMS:
	case MU_MSG_STATUS_REPORT:
		if(mu_initstatus.bSMS)
			if(mu_sms_interface->msg_delete_multi_messages(hWnd,&(pChainNode->handle),1) == FALSE)
                return -1;
		break;
		
	case MU_MSG_MMS:
	case MU_MSG_MMS_DELIVER_REPORT:
		if(mu_initstatus.bMMS)
			mu_mms_interface->msg_delete_message(hWnd,pChainNode->handle);
		break;
		
	case MU_MSG_EMAIL:
		if(mu_initstatus.bEmail)
			mu_email_interface->msg_delete_message(hWnd,pChainNode->handle);
		break;
		
	case MU_MSG_EMAILHEADER://need modify
		if(mu_initstatus.bEmail)
			;//                mu_emailbox_interface->email_delete(hWnd,pChainNode->handle);
		break;
		
	case MU_MSG_PUSH:
		if(mu_initstatus.bPush)
			mu_push_interface->msg_delete_message(hWnd,pChainNode->handle);
		break;

	case MU_MSG_BT:
	case MU_MSG_BT_VCARD:
	case MU_MSG_BT_VCAL:
	case MU_MSG_BT_NOTEPAD:
	case MU_MSG_BT_PICTURE:
		if(mu_initstatus.bBT)
			mu_bt_interface->msg_delete_message(hWnd, pChainNode->handle);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

void SelDelConfirm(HWND hWnd)
{
	int count;
	char pCaption[64];
	
	GetWindowText(hMuFrame, pCaption, 63);
	
	count = SendMessage(GetDlgItem(hWnd, IDC_SELDEL_LIST), LB_GETSELCOUNT, 0, 0);
	if(count <= 0)
	{
		PLXTipsWin(hMuFrame, NULL, 0, (char*)IDS_SEL_MESSAGE, pCaption, Notify_Alert, (char*)IDS_OK, "", WAITTIMEOUT);
		return;
	}

	PLXConfirmWinEx(hMuFrame, hWnd, (char*)IDS_DELSEL, Notify_Request, NULL, 
		(char*)IDS_YES, (char*)IDS_NO, WM_DELSELECT);
}


void SelMoveConfirm(HWND hWnd)
{
	int count;
	char pCaption[64];
	PMU_SELLISTDATA  pSelListData;
	PMU_LISTCREATEDATA pCreateData;

	pSelListData = (PMU_SELLISTDATA)GetUserData(hWnd);

	pCreateData = (PMU_LISTCREATEDATA)GetUserData(hListWnd);
	strcpy(pCaption, MU_GetCurCaption(pCreateData));
	
	count = SendMessage(GetDlgItem(hWnd, IDC_SELDEL_LIST), LB_GETSELCOUNT, 0, 0);
	if(count <= 0)
	{
		PLXTipsWin(hMuFrame, NULL, 0, (char*)IDS_SEL_MESSAGE, pCaption, Notify_Alert, (char*)IDS_OK, "", WAITTIMEOUT);
		return;
	}

	if( !MU_FolderSelection(hWnd,hWnd,WM_SELECTFOLDER, pSelListData->pOwner->nBoxType) )
	{
		return;
	}
}

static void SelMoveToFolder(HWND hWnd,BOOL bMove,int nNewFolder)
{
	PMU_CHAINNODE pChainNode = NULL;
    int nCurSel = -1;
    HWND hLst = NULL;
	int count;
	int* pIndex;
	char pCaption[64];
	int i;
	PMU_SELLISTDATA  pSelListData;
	PMU_LISTCREATEDATA pCreateData;
	
	pSelListData = (PMU_SELLISTDATA)GetUserData(hWnd);
	
	pCreateData = (PMU_LISTCREATEDATA)GetUserData(hListWnd);
	strcpy(pCaption, MU_GetCurCaption(pCreateData));

    if(bMove == FALSE)
        return;

    if(nNewFolder == -1)
        return;
    
    hLst = GetDlgItem(hWnd,IDC_SELDEL_LIST);

    count = SendMessage(hLst, LB_GETSELCOUNT, 0, 0);

	WaitWindowStateEx(hWnd, TRUE, (char*)IDS_MOVING, pCaption, "", (char*)IDS_CANCEL);

	pIndex = (int*)malloc(sizeof(int) * count);
	SendMessage(hLst, LB_GETSELITEMS, count, (LPARAM)pIndex);
    
	for(i = count - 1; i >= 0; i--)
	{
		pChainNode = (PMU_CHAINNODE)SendMessage(hLst,LB_GETITEMDATA,pIndex[i],0);
        if( !pChainNode )
        {
            free(pIndex);
            
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return;
        }
		
        if(IsUserCancel())
        {
            free(pIndex);
            
            PostMessage(hWnd, WM_CLOSE, 0, 0);
			return;
        }
		
		switch(pChainNode->msgtype) 
		{
		case MU_MSG_VCARD:
		case MU_MSG_VCAL:
		case MU_MSG_SMS:
		case MU_MSG_STATUS_REPORT:
			if(mu_initstatus.bSMS)
				if(mu_sms_interface->msg_move_message(pSelListData->hListOwner,pChainNode->handle,(int)nNewFolder))
					SendMessage(pSelListData->hListOwner,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),
					(LPARAM)(pChainNode->handle));
				break;
				
		case MU_MSG_MMS:
		case MU_MSG_MMS_DELIVER_REPORT:
			if(mu_initstatus.bMMS)
				if(mu_mms_interface->msg_move_message(pSelListData->hListOwner,pChainNode->handle,(int)nNewFolder))
					SendMessage(pSelListData->hListOwner,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_MMS),
					(LPARAM)(pChainNode->handle));
				break;
				
		case MU_MSG_EMAIL:
			if(mu_initstatus.bEmail)
				if(mu_email_interface->msg_move_message(pSelListData->hListOwner,pChainNode->handle,(int)nNewFolder))
					SendMessage(pSelListData->hListOwner,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_EMAIL),
					(LPARAM)(pChainNode->handle));
				break;
				
		case MU_MSG_PUSH:
			if(mu_initstatus.bPush)
				if(mu_push_interface->msg_move_message(pSelListData->hListOwner,pChainNode->handle,(int)nNewFolder))
					SendMessage(pSelListData->hListOwner,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_PUSH),
					(LPARAM)(pChainNode->handle));
				break;
		case MU_MSG_BT:
		case MU_MSG_BT_VCARD:
		case MU_MSG_BT_VCAL:
		case MU_MSG_BT_NOTEPAD:
		case MU_MSG_BT_PICTURE:
			if(mu_initstatus.bBT)
				if(mu_bt_interface->msg_move_message(pSelListData->hListOwner,pChainNode->handle,(int)nNewFolder))
					SendMessage(pSelListData->hListOwner,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_BT),
					(LPARAM)(pChainNode->handle));
				break;
				
		default:
		case MU_MSG_EMAILHEADER://need modify
			break;
		}
	}

	WaitWindowStateEx(hWnd, FALSE, (char*)IDS_MOVING, pCaption, "", (char*)IDS_CANCEL);
	
	if(i < 0)
		PLXTipsWin(hMuFrame, NULL, 0, (char*)IDS_MESSAGES_MOVED, pCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);
	
	free(pIndex);
	
	PostMessage(hWnd, WM_CLOSE, 0, 0);
	
}

void MoveAllMessages(HWND hLst, BOOL bMove, int nNewFolder)
{
	int i, count;
	char pCaption [64];
	PMU_CHAINNODE pChainNode;
	PMU_LISTCREATEDATA pCreateData;

	if(!bMove)
		return;

	pCreateData = (PMU_LISTCREATEDATA)GetUserData(hListWnd);
	strcpy(pCaption, MU_GetCurCaption(pCreateData));
	
	WaitWindowStateEx(hListWnd, TRUE, (char*)IDS_MOVING, pCaption, "", (char*)IDS_CANCEL);
	
	count = SendMessage(hLst, LB_GETCOUNT, 0, 0);

	for(i = count -1 ; i >= 0; i--)
	{
		pChainNode = (PMU_CHAINNODE)SendMessage(hLst, LB_GETITEMDATA, i, 0);
		if( !pChainNode )
			return;
		
		if(IsUserCancel())
			return;

		switch(pChainNode->msgtype) 
		{
		case MU_MSG_VCARD:
		case MU_MSG_VCAL:
		case MU_MSG_SMS:
		case MU_MSG_STATUS_REPORT:
			if(mu_initstatus.bSMS)
				if(mu_sms_interface->msg_move_message(hListWnd,pChainNode->handle,(int)nNewFolder))
					SendMessage(hListWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),
					(LPARAM)(pChainNode->handle));
				break;
				
		case MU_MSG_MMS:
		case MU_MSG_MMS_DELIVER_REPORT:
			if(mu_initstatus.bMMS)
				if(mu_mms_interface->msg_move_message(hListWnd,pChainNode->handle,(int)nNewFolder))
					SendMessage(hListWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_MMS),
					(LPARAM)(pChainNode->handle));
				break;
				
		case MU_MSG_EMAIL:
			if(mu_initstatus.bEmail)
				if(mu_email_interface->msg_move_message(hListWnd,pChainNode->handle,(int)nNewFolder))
					SendMessage(hListWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_EMAIL),
					(LPARAM)(pChainNode->handle));
				break;
				
		case MU_MSG_PUSH:
			if(mu_initstatus.bPush)
				if(mu_push_interface->msg_move_message(hListWnd,pChainNode->handle,(int)nNewFolder))
					SendMessage(hListWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_PUSH),
					(LPARAM)(pChainNode->handle));
				break;

		case MU_MSG_BT:
		case MU_MSG_BT_VCARD:
		case MU_MSG_BT_VCAL:
		case MU_MSG_BT_NOTEPAD:
		case MU_MSG_BT_PICTURE:
			if(mu_initstatus.bBT)
				if(mu_bt_interface->msg_move_message(hListWnd,pChainNode->handle,(int)nNewFolder))
					SendMessage(hListWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_BT),
					(LPARAM)(pChainNode->handle));
				break;
				
		default:
		case MU_MSG_EMAILHEADER://need modify
			break;
		}
	}

	WaitWindowStateEx(hListWnd, FALSE, (char*)IDS_MOVING, pCaption, "", (char*)IDS_CANCEL);
	
	if(i < 0)
		PLXTipsWin(hMuFrame, NULL, 0, (char*)IDS_MESSAGES_MOVED, pCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);
	
	return;
}

static int FillOrderList(HWND hWnd, DWORD** ppHandleList)
{
	int count, i;
	HWND hList;
	DWORD* pHandlepList = *ppHandleList;

	if(pHandlepList)
	{
		free(pHandlepList);
        pHandlepList = NULL;
	}
	
	hList = GetDlgItem(hWnd, IDC_MULSIT_LIST);

	count = SendMessage(hList, LB_GETCOUNT, 0, 0);

	if(count != 0)
		pHandlepList = (DWORD*)malloc(sizeof(DWORD) * count);

	*ppHandleList = pHandlepList;

	for(i=0; i<count; i++)
	{
		*pHandlepList = SendMessage(hList, LB_GETITEMDATA, i, 0);
		pHandlepList++;
	}

	return count;
}

static DWORD* GetPosInArray(DWORD pNode,DWORD* pHandleList, int nCount)
{
    int i;

    for(i = 0 ; i < nCount ; i++)
    {
        if(pHandleList[i] == pNode)
            return &pHandleList[i];
    }

    return NULL;
}

int GetPosInfo(DWORD* pList, int nCount, DWORD key, BOOL* bPre, BOOL* bNext)
{
	DWORD* pTemp;
	int i;

	pTemp = pList;

	for(i=0; i<nCount; i++)
	{
		if(pList[i] == key)
		{
			if(i == 0)
			{
				*bPre = FALSE;
			}
			else
			{
				*bPre = TRUE;
			}

			if(i == nCount - 1)
			{
				*bNext = FALSE;
			}
			else
			{
				*bNext = TRUE;
			}

			return i;
		}
	}

	return -1;
}

void On_mu_goto_another(HWND hWnd, BOOL bPre, BOOL bNext)
{
	PMU_CHAINNODE pChainNode;
    int ncursel;

	if(bPre)
	{
		if(nMsgPos == 0)
			return;

		nMsgPos--;
	}
	else if(bNext)
	{
		if(nMsgPos == nMsgCount - 1)
			return;

		nMsgPos++;
	}

	pChainNode = (PMU_CHAINNODE)pHandleList[nMsgPos];
	if( !pChainNode )
		return;

    ncursel = Mu_GetIndexInList(hWnd,pChainNode);

    if(ncursel != -1)
    {
        HWND hList = NULL;

        hList = GetDlgItem(hWnd,IDC_MULSIT_LIST);

        SendMessage(hList,LB_SETCURSEL,ncursel,0);
    }
    	
	GetPosInfo(pHandleList, nMsgCount, (DWORD)pChainNode, &bPre, &bNext);
	
	switch(pChainNode->msgtype) 
	{
	case MU_MSG_VCARD:
	case MU_MSG_VCAL:
	case MU_MSG_SMS:
	case MU_MSG_STATUS_REPORT:
		if(mu_initstatus.bSMS)
			mu_sms_interface->msg_read_message(hWnd,pChainNode->handle, bPre, bNext);
		break;
		
	case MU_MSG_MMS:
	case MU_MSG_MMS_DELIVER_REPORT:
		if(mu_initstatus.bMMS)
			mu_mms_interface->msg_read_message(hWnd,pChainNode->handle, bPre, bNext);
		break;
		
	case MU_MSG_EMAIL:
	case MU_MSG_EMAILHEADER:
		if(mu_initstatus.bEmail)
			mu_email_interface->msg_read_message(hWnd,pChainNode->handle, bPre, bNext);
		break;
		
	case MU_MSG_PUSH:
		if(mu_initstatus.bPush)
			mu_push_interface->msg_read_message(hWnd,pChainNode->handle, bPre, bNext);
		break;
		
	default:
		break;
	}
	
	return;
}

int Mu_GetIndexInList(HWND hWnd, PMU_CHAINNODE pChain)
{
	HWND hLst;
	int nIndex = -1;
	int nCount, i;
	MU_CHAINNODE* pNode = NULL;

	hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
	
	nCount = SendMessage(hLst,LB_GETCOUNT,NULL,NULL);
	for(i=0; i<nCount; i++)
	{
		pNode = (PMU_CHAINNODE)SendMessage(hLst, LB_GETITEMDATA, i, 0);

		if(pNode == pChain)
		{
			nIndex = i;
			break;
		}
	}

	return nIndex;
}

static BOOL MUListReset(HWND hWnd)
{
	PMU_LISTCREATEDATA p;

	p = GetUserData(hWnd);

	SendMessage(GetDlgItem(hWnd, IDC_MULSIT_LIST), LB_RESETCONTENT, 0, 0);
	memset(&p->mu_counter, 0, sizeof(MU_COUNTER));

	if(mu_initstatus.bSMS)
	{
		mu_sms_interface->msg_release_messages(hWnd);
	}
	if(mu_initstatus.bMMS)
	{
		mu_mms_interface->msg_release_messages(hWnd);
	}
	if(mu_initstatus.bEmail)
	{
		mu_email_interface->msg_release_messages(hWnd);
	}


	if(mu_initstatus.bMMS)
	{
		printf("mu_mms_interface->msg_get_messages\r\n");
		mu_mms_interface->msg_get_messages(hWnd, p->nBoxType);
	}
    
	if(mu_initstatus.bSMS)
	{
		printf("mu_sms_interface->msg_get_messages\r\n");
		mu_sms_interface->msg_get_messages(hWnd, p->nBoxType);
	}
    
	if(mu_initstatus.bEmail)
	{
		printf("mu_email_interface->msg_get_messages\r\n");
		mu_email_interface->msg_get_messages(hWnd, p->nBoxType);
	}
    
	if(mu_initstatus.bPush)
	{
		printf("mu_push_interface->msg_get_messages\r\n");
		mu_push_interface->msg_get_messages(hWnd, p->nBoxType);
	}
	
	if(mu_initstatus.bBT)
	{
		printf("mu_bt_interface->msg_get_messages\r\n");
		mu_bt_interface->msg_get_messages(hWnd, p->nBoxType);
	}

	return TRUE;
}	
