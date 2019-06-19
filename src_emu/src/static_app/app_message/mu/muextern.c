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

typedef struct tagMU_UserData
{
    BOOL    bEmailBox;
	BOOL	bExtName;
    int     nBoxType;
	HBITMAP	hSelected;
	HBITMAP	hNormal;
//    char    szEmailName[MU_EMAILBOX_NAME_LEN];
	char	szCaption[64];
    HWND    hWnd;
    UINT    wMsgCmd;
}MU_USERDATA,*PMU_USERDATA;

#define IDC_OK          100
#define IDC_CANCEL      200
#define IDC_MUSEL_LIST  300

//#define MU_SELECTION_HEIGTHMAX  100
//#define MU_SELECTION_HEIGTHPER  30
//#define SMALL_MAX_HEIGHT        120
//
//#define MU_SELECTION_WND_X      0
//#define MU_SELECTION_WND_WIDTH  176
//#define MU_SELECTION_WND_HEIGHT 220

extern MU_INITSTATUS   mu_initstatus;
extern MU_IMessage *mu_mms_interface;
extern MU_IMessage *mu_sms_interface;
extern MU_IMessage *mu_email_interface;
extern MU_IMessage *mu_bt_interface;

static BOOL MU_FolderSelectionWnd(HWND hParent,PMU_USERDATA pmu_userdata);
static LRESULT MUSelFolderWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MUSelFolder_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MUSelFolder_OnActivate(HWND hwnd, UINT state);
static void MUSelFolder_OnPaint(HWND hWnd);
static void MUSelFolder_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MUSelFolder_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void MUSelFolder_OnDestroy(HWND hWnd);
static void MUSelFolder_OnClose(HWND hWnd);
static BOOL MU_GetOneName(char* szName, const char* pAddr);
extern BOOL MU_GetFolderInfo(PMU_FOLDERINFO pInfo,int* pnCount);

static HWND hSelFolderWnd = NULL;
static HWND hFocus = NULL;
static int  count;
static PMU_FOLDERINFO pInfo = NULL;
/*********************************************************************\
* Function	   MU_FolderSelection
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MU_FolderSelectionEx(HWND hParent, HWND hWnd, const char* pCaption, UINT wMsgCmd, int nFolderType)
{
	MU_USERDATA myUserData;

    memset(&myUserData,0,sizeof(MU_USERDATA));
    myUserData.bEmailBox = FALSE;
    myUserData.nBoxType = nFolderType;
    myUserData.hWnd = hWnd;
    myUserData.wMsgCmd = wMsgCmd;
	myUserData.bExtName = TRUE;
	strcpy(myUserData.szCaption, pCaption);

    return MU_FolderSelectionWnd(hParent,&myUserData);
}

BOOL MU_FolderSelection(HWND hParent, HWND hWnd, UINT wMsgCmd, int nFolderType)
{
    MU_USERDATA myUserData;

    memset(&myUserData,0,sizeof(MU_USERDATA));
    myUserData.bEmailBox = FALSE;
    myUserData.nBoxType = nFolderType;
    myUserData.hWnd = hWnd;
    myUserData.wMsgCmd = wMsgCmd;
	myUserData.bExtName = FALSE;

    return MU_FolderSelectionWnd(hParent,&myUserData);
}
/*********************************************************************\
* Function	   MU_EmialboxFolderSelection
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
/*
BOOL MU_EmialboxFolderSelection(HWND hParent, HWND hWnd, UINT wMsgCmd, char* pszEmailboxName)
{
    MU_USERDATA myUserData;

//    assert(pszEmailboxName);

    memset(&myUserData,0,sizeof(MU_USERDATA));
    myUserData.bEmailBox = TRUE;
    myUserData.hWnd = hWnd;
    myUserData.wMsgCmd = wMsgCmd;
    
    if(pszEmailboxName)
    {
        if(strlen(pszEmailboxName) >= MU_EMAILBOX_NAME_LEN)
        {
            strncpy(myUserData.szEmailName,pszEmailboxName,MU_EMAILBOX_NAME_LEN-1);
            myUserData.szEmailName[MU_EMAILBOX_NAME_LEN-1] = 0;
        }
        else
            strcpy(myUserData.szEmailName,pszEmailboxName);
    }
    else
        return FALSE;

    return MU_FolderSelectionWnd(hParent,&myUserData);
}*/

/*********************************************************************\
* Function	   MU_FolderSelectionWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL MU_FolderSelectionWnd(HWND hParent,PMU_USERDATA pmu_userdata)
{
	WNDCLASS wc;
	RECT rClient;

    wc.style         = 0;
    wc.lpfnWndProc   = MUSelFolderWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MU_USERDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "MUSELFOLDERWndClass";
    
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
	
    if(!MU_GetFolderInfo(NULL,&count))
    {
        return FALSE;
    }

	GetClientRect(hMuFrame, &rClient);

	hSelFolderWnd = CreateWindow(
		"MUSELFOLDERWndClass", 
        IDS_SELECTFOLDER,
        WS_CHILD | WS_VISIBLE,
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,  
        hMuFrame,
        NULL,
		NULL, 
		(PVOID)pmu_userdata
		);
	
	if (!hSelFolderWnd)
	{
		UnregisterClass("MUSELFOLDERWndClass", NULL);
		return FALSE;
	}
	
	if(!pmu_userdata->bExtName)
		SetWindowText(hMuFrame, IDS_SELECTFOLDER);
	else
		SetWindowText(hMuFrame, pmu_userdata->szCaption);

	SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
	SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
	SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 

	SendMessage(hMuFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hMuFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
	//show window
	ShowWindow(hSelFolderWnd, SW_SHOW);
	UpdateWindow(hSelFolderWnd);

    return TRUE;

}
/*********************************************************************\
* Function	MUSelFolderWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT MUSelFolderWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MUSelFolder_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        //MUSelFolder_OnActivate(hWnd,(UINT)LOWORD(wParam));
		{
			PMU_USERDATA pUserData;

			pUserData = GetUserData(hWnd);

			SetFocus(GetDlgItem(hWnd, IDC_MUSEL_LIST));
			
			if(!pUserData->bExtName)
				SetWindowText(hMuFrame, IDS_SELECTFOLDER);
			else
				SetWindowText(hMuFrame, pUserData->szCaption);
			
			SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
			SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 
			SendMessage(hMuFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
			SendMessage(hMuFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
		}
		
        break;
        
    case WM_PAINT:
        MUSelFolder_OnPaint(hWnd);
        break;

	case WM_TIMER:
		{
			PMU_USERDATA pUserData;
			HWND hLst;
			int nIndex;
			int nfolderid = -1;
			
			KillTimer(hWnd, 1);

			pUserData = GetUserData(hWnd);
			hLst = GetDlgItem(hWnd,IDC_MUSEL_LIST);
			
			nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
			if(nIndex == LB_ERR)
			{
				PostMessage(hWnd,WM_CLOSE,0,0);
				break;
			}

			nfolderid = (int)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
			
			SendMessage(pUserData->hWnd,pUserData->wMsgCmd,(WPARAM)TRUE,(LPARAM)nfolderid);
			
			PostMessage(hWnd,WM_CLOSE,0,0);
		}
		break;

    case WM_KEYDOWN:
        MUSelFolder_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        MUSelFolder_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
		SendMessage(hMuFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
        MUSelFolder_OnClose(hWnd);
        break;

    case WM_DESTROY:
        MUSelFolder_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;
}
/*********************************************************************\
* Function	MUSelFolder_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MUSelFolder_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    HWND hLst;
    int nIndex;
    int i;
    PMU_USERDATA pUserData;
    RECT rc;
	SIZE size;
    MU_FOLDERCHAINNODE* pNewNode = NULL;
    MU_FOLDERCHAINNODE* pHeader = NULL;
    int nInsert,nStart = 0;

    pUserData = GetUserData(hWnd);

    memcpy(pUserData,(PMU_USERDATA)(lpCreateStruct->lpCreateParams),sizeof(MU_USERDATA));

    GetClientRect(hWnd,&rc);

    hLst = CreateWindow(
        "LISTBOX", 
        "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,  
        hWnd, 
        (HMENU)IDC_MUSEL_LIST, 
        NULL, 
        NULL);
    
    if(hLst == NULL)
        return FALSE;

    if(count != 0)
    {
        pInfo = (PMU_FOLDERINFO)malloc(sizeof(MU_FOLDERINFO)*count);
        
        if(!pInfo)
            return FALSE;
        
        if(!MU_GetFolderInfo(pInfo,&count))
        {
            if(pInfo)
            {
                free(pInfo);
                pInfo = NULL;
            }
            return FALSE;
        }
    }

	GetImageDimensionFromFile(MU_BMP_SELECT,&size);

    pUserData->hSelected = LoadImage(NULL, MU_BMP_SELECT, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);
    
    GetImageDimensionFromFile(MU_BMP_NORMAL,&size);

    pUserData->hNormal = LoadImage(NULL, MU_BMP_NORMAL, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);
	
    if(pUserData->bEmailBox)
    {
        nIndex = SendMessage(hLst,LB_ADDSTRING,-1,(LPARAM)IDS_INBOX);
        SendMessage(hLst,LB_SETITEMDATA,nIndex,MU_INBOX);
		SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,nIndex),(LPARAM)pUserData->hNormal);
        nStart++;
    }
    else
    {
        switch(pUserData->nBoxType)
        {
        case MU_INBOX:    
            break;
            
        case MU_SENT:
            nIndex = SendMessage(hLst,LB_ADDSTRING,-1,(LPARAM)IDS_INBOX);
            SendMessage(hLst,LB_SETITEMDATA,nIndex,MU_INBOX);
			SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,nIndex),(LPARAM)pUserData->hNormal);
            nStart++;
            break;
            
        case MU_OUTBOX:
        case MU_REPORT:
        case MU_DRAFT:
            return FALSE;
        
        default:
            nIndex = SendMessage(hLst,LB_ADDSTRING,-1,(LPARAM)IDS_INBOX);
            SendMessage(hLst,LB_SETITEMDATA,nIndex,MU_INBOX);
			SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,nIndex),(LPARAM)pUserData->hNormal);
            nStart++;
            break;
        }
    }

    
    for(i = 0 ; i < count ; i++)
    {
        if(pUserData->nBoxType != pInfo[i].nFolderID)
        {
            pNewNode = Folder_New();
            
            if(!pNewNode)
                return FALSE;
            
            Folder_Fill(pNewNode,pInfo[i],i*sizeof(MU_FOLDERINFO));
            
            nInsert = Folder_Insert(&pHeader,pNewNode); 
            
            if(nInsert == -1)
            {
                Folder_Erase(&pHeader);
                return FALSE;
            }
            
            SendMessage(hLst, LB_INSERTSTRING, (WPARAM)nInsert+nStart, (LPARAM) (LPCTSTR)pNewNode->folderinfo.szFolderName);
            SendMessage(hLst, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)nInsert+nStart),(LPARAM)pUserData->hNormal);
            SendMessage(hLst, LB_SETITEMDATA, (WPARAM)nInsert+nStart, (LPARAM)pInfo[i].nFolderID);
        }
    }

    SendMessage(hLst,LB_SETCURSEL,0,0);

    Folder_Erase(&pHeader);
    
    hFocus = hLst;
    SetFocus(hLst);
    return TRUE;
    
}
/*********************************************************************\
* Function	MUSelFolder_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MUSelFolder_OnActivate(HWND hwnd, UINT state)
{
    if(state == WA_ACTIVE)
        SetFocus(hFocus);
    else if(state == WA_INACTIVE)
        hFocus = GetFocus();

    return;
}

/*********************************************************************\
* Function	MUSelFolder_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUSelFolder_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	MUSelFolder_OnKey
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
static void MUSelFolder_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PMU_USERDATA pUserData;
    HWND hLst;
    int nIndex;
    int nfolderid = -1;

    pUserData = GetUserData(hWnd);
    hLst = GetDlgItem(hWnd,IDC_MUSEL_LIST);

	switch (vk)
	{
	case VK_F10:
        SendMessage(pUserData->hWnd,pUserData->wMsgCmd,(WPARAM)FALSE,NULL);

		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

//    case VK_RETURN:
	case VK_F5:
        nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
        if(nIndex == LB_ERR)
        {
            PostMessage(hWnd,WM_CLOSE,0,0);
            return;
        }
		SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,nIndex),(LPARAM)pUserData->hSelected);
		SetTimer(hWnd, 1, 500, NULL);

        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	MUSelFolder_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MUSelFolder_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    PMU_USERDATA pUserData;
    HWND hLst;
    int nIndex;
    int nfolderid = -1;

    pUserData = GetUserData(hWnd);
    hLst = GetDlgItem(hWnd,IDC_MUSEL_LIST);

	switch(id)
	{	
    case IDC_CANCEL:
        
        SendMessage(pUserData->hWnd,pUserData->wMsgCmd,(WPARAM)FALSE,NULL);
        
        PostMessage(hWnd,WM_CLOSE,0,0);
        break;

    case IDC_OK:
        nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
        if(nIndex == LB_ERR)
        {
            PostMessage(hWnd,WM_CLOSE,0,0);
            return;
        }

        nfolderid = (int)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);

        SendMessage(pUserData->hWnd,pUserData->wMsgCmd,(WPARAM)TRUE,(LPARAM)nfolderid);
        
        PostMessage(hWnd,WM_CLOSE,0,0);
        break;

    default:
        break;
	}

	return;
}

/*********************************************************************\
* Function	MUSelFolder_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUSelFolder_OnDestroy(HWND hWnd)
{    
	PMU_USERDATA pUserData;

	pUserData = GetUserData(hWnd);

	if(pUserData->hNormal)
		DeleteObject(pUserData->hNormal);
	if(pUserData->hSelected)
		DeleteObject(pUserData->hSelected);
	
    if(pInfo)
    {
        free(pInfo);
        pInfo = NULL;
    }

    hSelFolderWnd = NULL;
	
	UnregisterClass("MUSELFOLDERWndClass",NULL);

    return;

}
/*********************************************************************\
* Function	MUSelFolder_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUSelFolder_OnClose(HWND hWnd)
{

    DestroyWindow (hWnd);

    return;

}


HWND MuGetFrame(void)
{
	return hMuFrame;
}


void MsgNotify(int nType, BOOL bFull, int unRead, int unSend)
{
	static BOOL bMailFull = FALSE, bSMSFull = FALSE, bMMSFull = FALSE, bBTFull = FALSE;
	static int unReadMMS = 0, unReadSMS = 0, unReadMail = 0 , unReadBT = 0;
	static int unSendMMS = 0, unSendSMS = 0, unSendMail = 0 , unSendBT = 0;
	BOOL changFull = FALSE;

	switch(nType)
	{
	case MU_SMS_NOTIFY:

        printf("\r\n Unibox Debug info: MsgNotify sms bFull = %d, unRead = %d, unSend = %d \r\n",
            bFull,unRead,unSend);

		if(bSMSFull != bFull)
		{
			if(bFull)
			DlmNotify(PS_MSGOVERFLOW, ICON_SET);

			bSMSFull = bFull;
			changFull = TRUE;
		}

		if(unRead >= 0 && unReadSMS != unRead)
		{
			if(unReadSMS == 0 && unRead != 0)
				DlmNotify(PS_MSGUNREAD, ICON_SET);

			if(unReadSMS != 0 && unRead == 0)
				DlmNotify(PS_MSGUNREAD, ICON_CANCEL);

			unReadSMS = unRead;
		}

		if(unSend >= 0 && unSendSMS != unSend)
		{
			if(unSendSMS == 0 && unSend != 0)
				DlmNotify(PS_MSGOUTBOX, ICON_SET);

			if(unSendSMS != 0 && unSend == 0)
				DlmNotify(PS_MSGOUTBOX, ICON_CANCEL);

			unSendSMS = unSend;
		}
		
		break;

	case MU_MMS_NOTIFY:
        
        printf("\r\n Unibox Debug info: MsgNotify mms bFull = %d, unRead = %d, unSend = %d \r\n",
            bFull,unRead,unSend);

		if(bMMSFull != bFull)
		{
			if(bFull)
			DlmNotify(PS_MSGOVERFLOW, ICON_SET);

			bMMSFull = bFull;
			changFull = TRUE;
		}

		if(unRead >= 0 && unReadMMS != unRead)
		{
			if(unReadMMS == 0 && unRead != 0)
				DlmNotify(PS_MSGUNREAD, ICON_SET);

			if(unReadMMS != 0 && unRead == 0)
				DlmNotify(PS_MSGUNREAD, ICON_CANCEL);

			unReadMMS = unRead;
		}

		if(unSend >= 0 && unSendMMS != unSend)
		{
			if(unSendMMS == 0 && unSend != 0)
				DlmNotify(PS_MSGOUTBOX, ICON_SET);

			if(unSendMMS != 0 && unSend == 0)
				DlmNotify(PS_MSGOUTBOX, ICON_CANCEL);

			unSendMMS = unSend;
		}
		break;

	case MU_MAIL_NOTIFY:

        printf("\r\n Unibox Debug info: MsgNotify email bFull = %d, unRead = %d, unSend = %d \r\n",
            bFull,unRead,unSend);

		if(bMailFull != bFull)
		{
			if(bFull)
			DlmNotify(PS_MSGOVERFLOW, ICON_SET);

			bMailFull = bFull;
			changFull = TRUE;
		}

		if(unRead >= 0 && unReadMail != unRead)
		{
			if(unReadMail == 0 && unRead != 0)
				DlmNotify(PS_MSGUNREAD, ICON_SET);

			if(unReadMail != 0 && unRead == 0)
				DlmNotify(PS_MSGUNREAD, ICON_CANCEL);

			unReadMail = unRead;
		}

		if(unSend >= 0 && unSendMail != unSend)
		{
			if(unSendMail == 0 && unSend != 0)
				DlmNotify(PS_MSGOUTBOX, ICON_SET);

			if(unSendMail != 0 && unSend == 0)
				DlmNotify(PS_MSGOUTBOX, ICON_CANCEL);

			unSendMail = unSend;
		}
		break;

        
	case MU_BT_NOTIFY:

        printf("\r\n Unibox Debug info: MsgNotify email bFull = %d, unRead = %d, unSend = %d \r\n",
            bFull,unRead,unSend);

		if(bBTFull != bFull)
		{
			if(bFull)
			DlmNotify(PS_MSGOVERFLOW, ICON_SET);

			bBTFull = bFull;
			changFull = TRUE;
		}

		if(unRead >= 0 && unReadBT != unRead)
		{
			if(unReadBT == 0 && unRead != 0)
				DlmNotify(PS_MSGUNREAD, ICON_SET);

			if(unReadBT != 0 && unRead == 0)
				DlmNotify(PS_MSGUNREAD, ICON_CANCEL);

			unReadBT = unRead;
		}

		if(unSend >= 0 && unSendBT != unSend)
		{
			if(unSendBT == 0 && unSend != 0)
				DlmNotify(PS_MSGOUTBOX, ICON_SET);

			if(unSendBT != 0 && unSend == 0)
				DlmNotify(PS_MSGOUTBOX, ICON_CANCEL);

			unSendBT = unSend;
		}
		break;

	default:
		break;
	}

	if(changFull)
	{
		if(!bMailFull && !bMMSFull && !bSMSFull && !bBTFull)
		{
			DlmNotify(PS_MSGOVERFLOW, ICON_CANCEL);

		}
	}
}

void SendMuChildMessage(int msg, WPARAM wParam, LPARAM lParam)
{
	HWND hPreChild;
	HWND hChild;

	hChild = GetWindow(hMuFrame, GW_LASTCHILD);

	if(hChild == NULL)
		return;
	
	while(hChild)
	{
		hPreChild = GetWindow(hChild, GW_HWNDPREV);

		SendMessage(hChild, msg, wParam, lParam);

		hChild = hPreChild;
	}
	
	return;
}

int Mu_GetUnReadMsg(void)
{
	int count = 0, n, j;

    
    if(mu_initstatus.bSMS)
    {
        n= 0;
        mu_sms_interface->msg_get_count(MU_INBOX, &n, &j);
        if(n < 0)
            n = 0;
        count += n;
        
        printf("\r\n Unibox Debug info: Mu_GetUnReadMsg sms unread count = %d \r\n",n);
    }
     
    if(mu_initstatus.bMMS)
    {
        n= 0;
        mu_mms_interface->msg_get_count(MU_INBOX, &n, &j);
        if(n<0)
            n = 0;
        count += n;
        
        printf("\r\n Unibox Debug info: Mu_GetUnReadMsg mms unread count = %d \r\n",n);
    }
     
    if(mu_initstatus.bBT)
    {
        n= 0;
        mu_bt_interface->msg_get_count(MU_INBOX, &n, &j);
        if(n<0)
            n = 0;
        count += n;
        
        printf("\r\n Unibox Debug info: Mu_GetUnReadMsg bluetooth unread count = %d \r\n",n);
    }

	return count;
}

int Mu_GetUnReadEmail(void)
{
    int n = 0;
    int j = 0;

    if(mu_initstatus.bEmail)
    {
        mu_email_interface->msg_get_count(-1, &n, &j);
        
        if(n < 0)
            n = 0;
        
        printf("\r\n Unibox Debug info: Mu_GetUnReadEmail emailbox nr = %d \r\n",n);
    }
    
    return n;
}

BOOL MU_GetName(char* szName, const char* pAddr)
{
	char* p;
	char* pTemp;
	char* pName;
	int len, nCount = 0, i;

	if(*pAddr == 0)
	{
		szName[0] = 0;
		return FALSE;
	}

	
	len = strlen(pAddr);

	p = (char*)malloc(len + 1);
	strcpy(p, pAddr);

	pTemp = p;

	while(*pTemp)
	{
		if(*pTemp == ';' || *pTemp == ',')
		{
			*pTemp = 0;
			if(*(pTemp - 1) != 0)
				nCount++;
		}
		pTemp++;
	}
	
	if(p[len - 1] != 0)
		nCount++;

	pTemp = p;
	pName = szName;

	for(i=0; i<nCount; i++)
	{
		MU_GetOneName(pName, pTemp);
		pTemp += strlen(pTemp) + 1;
		pName += strlen(pName);
		if(i != nCount -1)
			*pName = ';';
		else
			*pName = 0;
		pName++;
	}
	
	free(p);

	return TRUE;
}


BOOL MU_GetOneName(char* szName, const char* pAddr)
{
	ABNAMEOREMAIL ABName;
	char	At[2] = {(char)0xFD, 0};

	if(strstr(pAddr, At))
	{
		if(APP_GetNameByEmail(pAddr, &ABName))
		{
			strcpy(szName, ABName.szName);
			return TRUE;
		}
		else
		{
			strcpy(szName, pAddr);
			return FALSE;
		}
	}
	else
	{
		if(APP_GetNameByPhone(pAddr, &ABName))
		{
			strcpy(szName, ABName.szName);
			return TRUE;
		}
		else
		{
			strcpy(szName, pAddr);
			return FALSE;
		}
	}
}


BOOL IsUserCancel(void)
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, 1))
    {	
        TranslateMessage(&msg);
        DispatchMessage(&msg);

		if(msg.message == WM_KEYDOWN && msg.wParam == VK_F10)
			return TRUE;
    }

	return FALSE;
}

LONG APP_GetMsgSize(void)
{
    LONG size = 0;
    LONG ltemp;

    ltemp = 0;
    ltemp =  APP_GetSMSSize();
    size += ltemp;
    printf("\r\n APP_GetMsgSize------SMS: %d KB",ltemp);
    
    ltemp = 0;
    ltemp =  APP_GetMMSSize();
    size += ltemp;
    printf("\r\n APP_GetMsgSize------MMS: %d KB",ltemp);
    
    ltemp = 0;
    ltemp =  APP_GetEmailSize();
    size += ltemp;
    printf("\r\n APP_GetMsgSize------Email: %d KB",ltemp);

    ltemp = 0;
    ltemp =  GetBtAllMsgSize();
    size += ltemp;
    printf("\r\n APP_GetMsgSize------Bluetooth: %d KB",ltemp);

    printf("\r\n APP_GetMsgSize------All of MSG: %d KB",size);

    return size;
}
