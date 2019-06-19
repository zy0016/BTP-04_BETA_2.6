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

#define WM_NEWFOLDER            WM_USER+100
#define WM_RENAMEFOLDER         WM_USER+101
#define WM_DELFOLDER			WM_USER+102
#define WM_SETCURSEL            WM_USER+103

#define IDM_FOLDER_OPENFOLDER   110
#define IDM_FOLDER_RENAMEFOLDER 111
#define IDM_FOLDER_DELETEFOLDER 112

#define IDC_EXIT        200
#define IDC_MULSIT_LIST 300

#define MAX_LIST_DIS    90

#define MU_TEMPSPACE_MAX    100*1024 

#define FILE_NEWFOLDER_BMP	"ROM:/message/unibox/newfolder.bmp"
#define FILE_FOLDER_BMP		"ROM:/message/unibox/folder.bmp"

typedef struct tagMU_SmallData
{
    HWND hWnd;
    UINT wMsgCmd;
	BOOL bNew;
    MU_FOLDERCHAINNODE* pFirst;
    char szFolderName[MU_FOLDER_NAME_LEN+1];
}MU_SMALLDATA,*PMU_SMALLDATA;


static HWND hFolderListWnd;
static HWND hFolderList;
static HWND hFocus = NULL;
static HBITMAP hBmp;
static HBITMAP	hNewBmp;

extern MU_INITSTATUS mu_initstatus;
extern MU_IMessage *mu_mms_interface;
extern MU_IMessage *mu_sms_interface;
extern MU_IMessage *mu_push_interface;
extern MU_IMessage *mu_email_interface;
extern MU_IMessage* mu_bt_interface;

BOOL MU_CreateFolderList(HWND hParent);
static LRESULT MUFolderWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MUFolder_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MUFolder_OnActivate(HWND hwnd, UINT state);
static void MUFolder_OnInitmenu(HWND hwnd);
static void MUFolder_OnPaint(HWND hWnd);
static void MUFolder_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MUFolder_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void MUFolder_OnDestroy(HWND hWnd);
static void MUFolder_OnClose(HWND hWnd);
static BOOL MU_FolderLoad(hWnd);
static void Folder_AddNodeToList(HWND hWnd,MU_FOLDERCHAINNODE *pNewNode,int nInsert);
static void Folder_FormatDisplay(HWND hWnd, char *szDisplay, int nDisplayLen, char *szString);
static void MUFolder_New(HWND hWnd, BOOL bNew, char* pszFileName);
static void MUFolder_Rename(HWND hWnd, BOOL bRename, char* pszFileName);

MU_FOLDERCHAINNODE* Folder_New(void);
BOOL Folder_Fill(MU_FOLDERCHAINNODE* pChainNode,MU_FOLDERINFO FolderInfo,DWORD dwoffset);
int  Folder_Insert(MU_FOLDERCHAINNODE** ppHeader,MU_FOLDERCHAINNODE* pChainNode);
void Folder_Delete(MU_FOLDERCHAINNODE** ppHeader,MU_FOLDERCHAINNODE* pChainNode);
void Folder_Erase(MU_FOLDERCHAINNODE** ppHeader);


#define IDC_OK      100
#define IDC_CANCEL  200
#define IDC_MUSMALL_EDIT    300


static BOOL MU_CreateSmallWnd(HWND hParent,HWND hWnd,UINT wMsgCmd ,char* szName, BOOL bNew,MU_FOLDERCHAINNODE* pHeader);
static LRESULT MUSmallWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MUSmall_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MUSmall_OnActivate(HWND hwnd, UINT state);
static void MUSmall_OnPaint(HWND hWnd);
static void MUSmall_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MUSmall_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void MUSmall_OnDestroy(HWND hWnd);
static void MUSmall_OnClose(HWND hWnd);
static void MUSmall_OnSetLBtnText(HWND hWnd, int nID, BOOL bEmpty, LPSTR pszText);
static BOOL MU_FindUseableName(MU_FOLDERCHAINNODE* pHeader,char* szFolderName);
static BOOL MU_IsFolderNameUsed(MU_FOLDERCHAINNODE* pHeader,char* pszFolderName);
static int  MU_FindUseableID(MU_FOLDERCHAINNODE* pHeader);
static BOOL MU_IsFolderIDUsed(MU_FOLDERCHAINNODE* pHeader,int ID);
static BOOL MU_ClearUp(void);
static DWORD Folder_AppendRecord(MU_FOLDERINFO *pFolderinfo);
static BOOL Folder_DeleteRecord(DWORD dwoffset);

BOOL MU_GetFolderInfo(PMU_FOLDERINFO pInfo,int* pnCount);
extern BOOL CreateSubFolderWnd(HWND hParent,MU_FOLDERINFO *pFolderInfo);
static int MU_MoveBlankFromString(char * str);

/*********************************************************************\
* Function	   MU_CreateFloderList
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MU_CreateFolderList(HWND hParent)
{
    WNDCLASS    wc;
    HMENU       hMenu;
    MU_FOLDERDATA CreateData;
	RECT rClient;
	
    memset(&CreateData,0,sizeof(MU_FOLDERDATA));
    CreateData.pFirst = NULL;
    CreateData.nBoxType = MU_MYFOLDER;

    wc.style         = 0;
    wc.lpfnWndProc   = MUFolderWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MU_FOLDERDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "MUFolderWndClass";
    
    if (!RegisterClass(&wc))
        return NULL;
    
    hMenu = CreateMenu();
	CreateData.hMenu = hMenu;

    GetClientRect(hMuFrame, &rClient);

    hFolderListWnd = CreateWindow(
        "MUFolderWndClass",
        IDS_MYFOLDER, 
        WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hMuFrame,
        NULL, 
        NULL, 
        (PVOID)&CreateData
        );
    
    if(!hFolderListWnd)
        return FALSE;
	
    SetWindowText(hMuFrame, IDS_MYFOLDER);

    AppendMenu(hMenu, MF_ENABLED, IDM_FOLDER_OPENFOLDER, IDS_OPEN);
    AppendMenu(hMenu, MF_ENABLED, IDM_FOLDER_RENAMEFOLDER, IDS_RENAMEFOLDER);
    AppendMenu(hMenu, MF_ENABLED, IDM_FOLDER_DELETEFOLDER, IDS_DELETEFOLDER);

    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 
    PDASetMenu(hMuFrame, hMenu);
    return TRUE;
}

/*********************************************************************\
* Function	MUFolderWndProc
* Purpose   Folder window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT MUFolderWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MUFolder_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
		{
			MU_FOLDERDATA* pData;

			pData = (MU_FOLDERDATA*)GetUserData(hWnd);
			
			MUFolder_OnActivate(hWnd,(UINT)LOWORD(wParam));

			SetWindowText(hMuFrame, IDS_MYFOLDER);
			
			SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);

			if(pData->nFocus == 0)
			{
				SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
			}
			else
			{
				SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
				SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
			}
			
			PDASetMenu(hMuFrame, pData->hMenu);
			SetFocus(GetDlgItem(hWnd, IDC_MULSIT_LIST));
		}
        
        break;
        
    case WM_INITMENU:
        MUFolder_OnInitmenu(hWnd);
        break;

    case WM_PAINT:
        MUFolder_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        MUFolder_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
		{
			if(wParam == (WPARAM)GetMenu(hMuFrame))
			{
				//WM_INITMENU
				MUFolder_OnInitmenu(hWnd);
				break;
			}
		}
        MUFolder_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
		SendMessage(hMuFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
        MUFolder_OnClose(hWnd);
        break;

    case WM_DESTROY:
		{
			MU_FOLDERDATA* pData;

			pData = (MU_FOLDERDATA*)GetUserData(hWnd);

			DestroyMenu(pData->hMenu);
		}

        MUFolder_OnDestroy(hWnd);
        break;

    case WM_NEWFOLDER:
        MUFolder_New(hWnd, (BOOL)(wParam), (char*)(lParam));
        break;

    case WM_RENAMEFOLDER:
        MUFolder_Rename(hWnd, (BOOL)(wParam), (char*)(lParam));
        break;

    case WM_SETCURSEL:
        {
            char    szBtnText [20];
            HWND    hLst;
            MU_FOLDERDATA* pCreateData;
            int     nfolderid = -1;
            PMU_FOLDERCHAINNODE pChainNode = NULL;
            int nUnread,nCount,nUnreadSum = 0,nCountSum = 0;
            char szDisplay [100];
            
            pCreateData = GetUserData(hWnd);
            
            hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
            
            if(pCreateData->nFocus != 0)
            {
                pChainNode = (PMU_FOLDERCHAINNODE)SendMessage(hLst, LB_GETITEMDATA, pCreateData->nFocus, 0);
                
                SendMessage(hLst,LB_SETTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)pChainNode->folderinfo.szFolderName);
            }
            
            SendMessage(hLst,LB_SETCURSEL,pCreateData->nCurSel,0);
            
            pCreateData->nFocus = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            szBtnText[0] = 0;
            
            SendMessage(hMuFrame,PWM_GETBUTTONTEXT,1,(LPARAM)szBtnText);
            
            if(pCreateData->nFocus == 0 && strcmp(IDS_OPEN,szBtnText) == 0)
            {
                SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
            }
            else if(pCreateData->nFocus != 0 && strcmp(IDS_OPEN,szBtnText) != 0)
            {
                SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
                SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
            }
            
            pChainNode = (PMU_FOLDERCHAINNODE)SendMessage(hLst, LB_GETITEMDATA, pCreateData->nFocus, 0); 
            
            nfolderid = pChainNode->folderinfo.nFolderID;
            
            if(mu_initstatus.bSMS)
            {
                nUnread = 0;
                nCount = 0;
                mu_sms_interface->msg_get_count(nfolderid,&nUnread,&nCount);
                printf("mu_sms_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bMMS)
            {
                nUnread = 0;
                nCount = 0;
                mu_mms_interface->msg_get_count(nfolderid,&nUnread,&nCount);
                printf("mu_mms_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bEmail)
            {
                nUnread = 0;
                nCount = 0;
                mu_email_interface->msg_get_count(nfolderid,&nUnread,&nCount);
                printf("mu_email_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bPush)
            {
                nUnread = 0;
                nCount = 0;
                mu_push_interface->msg_get_count(nfolderid,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bBT)
            {
                nUnread = 0;
                nCount = 0;
                mu_bt_interface->msg_get_count(nfolderid,&nUnread,&nCount);
                printf("mu_bt_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            szDisplay[0] = 0;

            sprintf(szDisplay, "%s_/%d", pChainNode->folderinfo.szFolderName, nCountSum);
            
            SendMessage(hLst,LB_SETTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDisplay);
            
        }
        break;

	case WM_DELFOLDER:
		{
			int nfoldid = -1,nCount = 0;
            char szBtnText[20];
			PMU_FOLDERCHAINNODE pChainNode;
			MU_FOLDERDATA *pCreateData;
			int nCurSel;
			HWND    hLst;

			pCreateData = (MU_FOLDERDATA*)GetUserData(hWnd);
			hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);

            nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
            if( nCurSel == LB_ERR || nCurSel == 0)
                break;

			if(lParam == 1)
			{
				pChainNode = (PMU_FOLDERCHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
				if( !pChainNode )
					break;
				
				WaitWindowStateEx(hWnd, TRUE, (char*)IDS_DELETEING, NULL, "", (char*)IDS_CANCEL);

				nfoldid = pChainNode->folderinfo.nFolderID;
				
				//delete file
				if(mu_initstatus.bSMS)
					mu_sms_interface->msg_delete_all_messages(hWnd,nfoldid,MU_DELALL_FOLDER);

				if(IsUserCancel())
						goto error_handle;

				if(mu_initstatus.bMMS)
					mu_mms_interface->msg_delete_all_messages(hWnd,nfoldid,MU_DELALL_FOLDER);

				if(IsUserCancel())
						goto error_handle;

				if(mu_initstatus.bEmail)
					mu_email_interface->msg_delete_all_messages(hWnd,nfoldid,MU_DELALL_FOLDER);

				if(IsUserCancel())
						goto error_handle;

				if(mu_initstatus.bPush)
					mu_push_interface->msg_delete_all_messages(hWnd,nfoldid,MU_DELALL_FOLDER);
				
				if(IsUserCancel())
						goto error_handle;

				if(mu_initstatus.bBT)
					mu_bt_interface->msg_delete_all_messages(hWnd,nfoldid,MU_DELALL_FOLDER);
				
				SendMessage(hLst,LB_DELETESTRING,nCurSel,0);
				
				nCount = SendMessage(hLst,LB_GETCOUNT,0,0);
				
				if(nCurSel == nCount)
					SendMessage(hLst,LB_SETCURSEL,nCurSel-1,0);
				else
					SendMessage(hLst,LB_SETCURSEL,nCurSel,0);
				
				pCreateData->nFocus = SendMessage(hLst,LB_GETCURSEL,0,0);
				
				szBtnText[0] = 0;
				
				SendMessage(hMuFrame,PWM_GETBUTTONTEXT,1,(LPARAM)szBtnText);
				
				if(pCreateData->nFocus == 0 && strcmp(IDS_OPEN,szBtnText) == 0)
				{
					SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
					SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
				}
				else if(pCreateData->nFocus != 0 && strcmp(IDS_OPEN,szBtnText) != 0)
				{
					SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
					SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
				}
				
				Folder_DeleteRecord(pChainNode->dwoffset);
				
				Folder_Delete(&(pCreateData->pFirst),pChainNode); //need modify

				WaitWindowStateEx(hWnd, FALSE, (char*)IDS_DELETEING, NULL, "", (char*)IDS_CANCEL);

				PLXTipsWin(hMuFrame, NULL, 0, (char*)IDS_DELETED, NULL, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);

error_handle:
				MUFolder_OnActivate(hWnd,0);

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
* Function	MUFolder_OnCreate
* Purpose   WM_CREATE message handler of the Folder window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MUFolder_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    RECT rect;
    MU_FOLDERDATA *pCreateData;

    MU_ClearUp();
    
    pCreateData = (MU_FOLDERDATA*)GetUserData(hWnd);

    memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(MU_FOLDERDATA));
    
    pCreateData->nFocus = 0;
    
    hNewBmp = LoadImage(NULL, FILE_NEWFOLDER_BMP, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hBmp = LoadImage(NULL, FILE_FOLDER_BMP, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    GetClientRect(hWnd,&rect);
    
    hFolderList = CreateWindow(
        "piclist", 
        "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_ICON | LBS_BITMAP,
        0,
        0,
        rect.right-rect.left,
        rect.bottom-rect.top, 
        hWnd, 
        (HMENU)IDC_MULSIT_LIST, 
        NULL, 
        NULL);
    
    if(hFolderList == NULL)
        return FALSE;

    SendMessage(hFolderList, LB_ADDSTRING, NULL, (LPARAM)IDS_NEWFOLDER);
    SendMessage(hFolderList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_ICON,0),(LPARAM)hNewBmp);//for transparent
    
    MU_FolderLoad(hWnd);
    
    SendMessage(hFolderList, LB_SETCURSEL, 0, 0);

    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
	SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        
//    SendMessage(hWnd, PWM_SETAPPICON, 0, (LPARAM)ICON_MU_MAIN);
    
    hFocus = hFolderList;
    SetFocus(hFocus);
    return TRUE;
}
/*********************************************************************\
* Function	MUFolder_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MUFolder_OnActivate(HWND hwnd, UINT state)
{
    /*
    if(state == WA_ACTIVE)
        { */
    
        HWND hLst;
//		int i, j;
        //int  nCurSel;
        PMU_FOLDERCHAINNODE pChainNode;
        int  nUnread,nCount,nUnreadSum = 0,nCountSum = 0,nfolderid = -1;
        MU_FOLDERDATA *pCreateData;
        char szDisplay[100]/*,szDisplayAndNum[120]*/;
        
        pCreateData = (MU_FOLDERDATA*)GetUserData(hwnd);

        SetFocus(hFocus);
        if(pCreateData->nFocus == 0)
            return;
        
        hLst = GetDlgItem(hwnd,IDC_MULSIT_LIST);
        //nCurSel = SendMessage(hLst, LB_GETCURSEL, 0, 0);
        pChainNode = (PMU_FOLDERCHAINNODE)SendMessage(hLst, LB_GETITEMDATA, pCreateData->nFocus, 0);       
        
		nfolderid = pChainNode->folderinfo.nFolderID;
        if(mu_initstatus.bSMS)
        {
            nUnread = 0;
            nCount = 0;
            mu_sms_interface->msg_get_count(nfolderid,&nUnread,&nCount);
            printf("mu_sms_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
            nUnreadSum += nUnread;
            nCountSum += nCount;
        }
        if(mu_initstatus.bMMS)
        {
            nUnread = 0;
            nCount = 0;
            mu_mms_interface->msg_get_count(nfolderid,&nUnread,&nCount);
            printf("mu_mms_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
            nUnreadSum += nUnread;
            nCountSum += nCount;
        }
        if(mu_initstatus.bEmail)
        {
            nUnread = 0;
            nCount = 0;
            mu_email_interface->msg_get_count(nfolderid,&nUnread,&nCount);
            printf("mu_email_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
            nUnreadSum += nUnread;
            nCountSum += nCount;
        }
        if(mu_initstatus.bPush)
        {
            nUnread = 0;
            nCount = 0;
            mu_push_interface->msg_get_count(nfolderid,&nUnread,&nCount);
            nUnreadSum += nUnread;
            nCountSum += nCount;
        }
		if(mu_initstatus.bBT)
		{
            nUnread = 0;
            nCount = 0;
			mu_bt_interface->msg_get_count(nfolderid,&nUnread,&nCount);
            printf("mu_bt_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
            nUnreadSum += nUnread;
            nCountSum += nCount;
		}
        
//		strcpy(szDisplay, pChainNode->folderinfo.szFolderName);
		sprintf(szDisplay, "%s_/%d", pChainNode->folderinfo.szFolderName, nCountSum);
/*
				if(nCountSum < 10)
					i = 17;
				else if(nCountSum < 100)
					i = 16;
				else if(nCountSum < 1000)
					i = 15;
				else
					i = 14;
				
				if(strlen(szDisplay) <= (unsigned int)i)
				{
					for(j=strlen(szDisplay); j <i; j++)
						szDisplay[j] = ' ';
					
					sprintf(&szDisplay[i], "%d", nCountSum);
				}*/
		

//        szDisplay[0] = 0;
//        Folder_FormatDisplay(hwnd,szDisplay,100,pChainNode->folderinfo.szFolderName);
//        szDisplayAndNum[0] = 0;
//        sprintf(szDisplayAndNum,"%s %d",szDisplay,nCountSum);
        
        SendMessage(hLst,LB_SETTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDisplay/*pChainNode->folderinfo.szFolderName*/);
    /*
    }
        else if(state == WA_INACTIVE)
            hFocus = GetFocus();*/
    
    
    return;
}
/*********************************************************************\
* Function	MUFolder_OnInitmenu
* Purpose   WM_INITMENU message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUFolder_OnInitmenu(HWND hwnd)
{
    return;
}
/*********************************************************************\
* Function	MUFolder_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUFolder_OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd, NULL);
    
    EndPaint(hWnd, NULL);
    
    return;
}
/*********************************************************************\
* Function	MUFolder_OnKey
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
static void MUFolder_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    HWND hLst;
    int  nIndex;
    char szFileName[MU_FOLDER_NAME_LEN+1];
    MU_FOLDERDATA *pCreateData;
    
    pCreateData = (MU_FOLDERDATA*)GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);

    switch (vk)
    {
    case VK_F10:
        PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

    case VK_F5:
        nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
        if(nIndex == LB_ERR)
            break;
        else if(nIndex == 0)
        {
            szFileName[0] = 0;
            if(!MU_CreateSmallWnd(hWnd,hWnd,WM_NEWFOLDER,szFileName, TRUE,pCreateData->pFirst))
                return;
        }
        else
            PDADefWindowProc(hMuFrame, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        break;

    case VK_RETURN:
        nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
        if(nIndex == LB_ERR)
            break;
        else if(nIndex != 0)
            SendMessage(hWnd,WM_COMMAND,IDM_FOLDER_OPENFOLDER,NULL);
        break;
        
    default:
        PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        break;
    }
    
    return;
}
/*********************************************************************\
* Function	MUFolder_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MUFolder_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    PMU_FOLDERCHAINNODE pChainNode;
    HWND    hLst;
    int     nCurSel;
    MU_FOLDERDATA *pCreateData;
    
    pCreateData = (MU_FOLDERDATA*)GetUserData(hWnd);
    
    hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
    
    switch(id)
    {
    case IDC_EXIT:
        PostMessage(hWnd,WM_CLOSE,0,0);
        break;

    case IDM_FOLDER_OPENFOLDER:
        {
            nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if( nCurSel == LB_ERR || nCurSel == 0 )
                return;
            
            pChainNode = (PMU_FOLDERCHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
            if( !pChainNode )
                return;

            CreateSubFolderWnd(hWnd,&(pChainNode->folderinfo));
        }
        break;

    case IDM_FOLDER_RENAMEFOLDER:
        {
            nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
            if( nCurSel == LB_ERR || nCurSel == 0)
                return;
            
            pChainNode = (PMU_FOLDERCHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
            if( !pChainNode )
                return;

            MU_CreateSmallWnd(hWnd,hWnd,WM_RENAMEFOLDER,pChainNode->folderinfo.szFolderName, FALSE,pCreateData->pFirst);
        }
        break;

    case IDM_FOLDER_DELETEFOLDER:
        {
			char temp[256];
			char name[64];
			HDC  hdc;
			int  len, nFit;
			
            nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
            if( nCurSel == LB_ERR || nCurSel == 0)
                break;

			pChainNode = (PMU_FOLDERCHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
				if( !pChainNode )
					break;
				
			hdc = GetDC(hWnd);
			len = strlen(pChainNode->folderinfo.szFolderName);
			GetTextExtentExPoint(hdc, pChainNode->folderinfo.szFolderName,
                    len, 176, &nFit, NULL, NULL);
			ReleaseDC(hWnd, hdc);
			if(len > nFit)
			{
				strncpy(name, pChainNode->folderinfo.szFolderName, nFit-3);
				name[nFit-4] = '.';
				name[nFit-3] = '.';
				name[nFit-2] = '.';
				name[nFit-1]   = 0;
			}
			else
			{
				strcpy(name, pChainNode->folderinfo.szFolderName);
			}
			sprintf(temp, "%s:\r\n%s", name, IDS_SURE_DELFOLDER);
            PLXConfirmWinEx(hMuFrame, hWnd, temp, Notify_Request, NULL, 
                (char*)IDS_YES, (char*)IDS_NO, WM_DELFOLDER);
        }
        break;
           
    case IDC_MULSIT_LIST:
        {
            if(codeNotify == LBN_SELCHANGE)
            {
                int  nCurSel;
                PMU_FOLDERCHAINNODE pChainNode;
                int  nUnread,nCount,nUnreadSum = 0,nCountSum = 0,nfolderid = -1;
                MU_FOLDERDATA *pCreateData;
                char szDisplay[100];
                char szBtnText[20];
                
                pCreateData = (MU_FOLDERDATA*)GetUserData(hWnd);
                nCurSel = SendMessage(hLst, LB_GETCURSEL, 0, 0);
                if(pCreateData->nFocus == nCurSel)
                    break;
                
                if(pCreateData->nFocus != 0)
                {
                    //szDisplay[0] = 0;
                    pChainNode = (PMU_FOLDERCHAINNODE)SendMessage(hLst, LB_GETITEMDATA, pCreateData->nFocus, 0);
                    
                    //Folder_FormatDisplay(hWnd,szDisplay,100,pChainNode->folderinfo.szFolderName);
                    SendMessage(hLst,LB_SETTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)pChainNode->folderinfo.szFolderName);
                }
                
                pCreateData->nFocus = nCurSel;
                
                szBtnText[0] = 0;
                
                SendMessage(hMuFrame,PWM_GETBUTTONTEXT,1,(LPARAM)szBtnText);
                
                if(pCreateData->nFocus == 0 && strcmp(IDS_OPEN,szBtnText) == 0)
                {
                    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
                }
                else if(pCreateData->nFocus != 0 && strcmp(IDS_OPEN,szBtnText) != 0)
                {
                    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
                    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
                }
                
                if(nCurSel == 0)
                    break;
                
                pChainNode = (PMU_FOLDERCHAINNODE)SendMessage(hLst, LB_GETITEMDATA, nCurSel, 0);
                nfolderid = pChainNode->folderinfo.nFolderID;
                if(mu_initstatus.bSMS)
                {
                    nUnread = 0;
                    nCount = 0;
                    mu_sms_interface->msg_get_count(nfolderid,&nUnread,&nCount);
                    printf("mu_sms_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
                    nUnreadSum += nUnread;
                    nCountSum += nCount;
                }
                if(mu_initstatus.bMMS)
                {
                    nUnread = 0;
                    nCount = 0;
                    mu_mms_interface->msg_get_count(nfolderid,&nUnread,&nCount);
                    printf("mu_mms_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
                    nUnreadSum += nUnread;
                    nCountSum += nCount;
                }
                if(mu_initstatus.bEmail)
                {
                    nUnread = 0;
                    nCount = 0;
                    mu_email_interface->msg_get_count(nfolderid,&nUnread,&nCount);
                    printf("mu_email_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
                    nUnreadSum += nUnread;
                    nCountSum += nCount;
                }
                if(mu_initstatus.bPush)
                {
                    nUnread = 0;
                    nCount = 0;
                    mu_push_interface->msg_get_count(nfolderid,&nUnread,&nCount);
                    nUnreadSum += nUnread;
                    nCountSum += nCount;
                }
				if(mu_initstatus.bBT)
				{
                    nUnread = 0;
                    nCount = 0;
					mu_bt_interface->msg_get_count(nfolderid,&nUnread,&nCount);
                    printf("mu_bt_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
					nUnreadSum += nUnread;
					nCountSum += nCount;
				}
				sprintf(szDisplay, "%s_/%d", pChainNode->folderinfo.szFolderName, nCountSum);

                /*
                strcpy(szDisplay, pChainNode->folderinfo.szFolderName);
                				
                				if(nCountSum < 10)
                					i = 17;
                				else if(nCountSum < 100)
                					i = 16;
                				else if(nCountSum < 1000)
                					i = 15;
                				else
                					i = 14;
                
                				if(strlen(szDisplay) <= (unsigned int)i)
                				{
                					for(j=strlen(szDisplay); j <i; j++)
                						szDisplay[j] = ' ';
                
                					sprintf(&szDisplay[i], "%d", nCountSum);
                				}*/
                
				
//                Folder_FormatDisplay(hWnd,szDisplay,100,pChainNode->folderinfo.szFolderName);
//                szDisplayAndNum[0] = 0;
//                sprintf(szDisplayAndNum,"%s %d",szDisplay,nCountSum);
                SendMessage(hLst,LB_SETTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDisplay/*pChainNode->folderinfo.szFolderName*/);
            }
        }
        break;

    default:
        break;
    }
}
/*********************************************************************\
* Function	MUFolder_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUFolder_OnDestroy(HWND hWnd)
{
    PMU_FOLDERDATA p;

    p = GetUserData(hWnd);

    Folder_Erase(&(p->pFirst));

    UnregisterClass("MUFolderWndClass", NULL);
    DeleteObject(hBmp);
	hBmp = NULL;
}
/*********************************************************************\
* Function	MUFolder_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUFolder_OnClose(HWND hWnd)
{
    DestroyWindow(hWnd);

    //UnregisterClass("MUFolderWndClass", NULL);
    
    return;
}

/*********************************************************************\
* Function	   MU_FolderLoad
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL MU_FolderLoad(HWND hWnd)
{
    int nFolderCount = 0,nArray = 0,i,nInsert;
    PMU_FOLDERINFO pFolderInfo = NULL;
    MU_FOLDERCHAINNODE *pNewNode = NULL;
    HWND hLst = NULL;
    MU_FOLDERDATA *pData;
    
    pData = (MU_FOLDERDATA*)GetUserData(hWnd);
    
    if(MU_GetFolderInfo(NULL,&nFolderCount))
    {
        if(nFolderCount != 0)
        {
            pFolderInfo = (PMU_FOLDERINFO)malloc(nFolderCount*sizeof(MU_FOLDERINFO));
            
            if(pFolderInfo != NULL)
            {
                if(MU_GetFolderInfo(pFolderInfo,&nFolderCount))
                {
                    nArray = nFolderCount;
                }
                else
                    return FALSE;
            }
            else
                return FALSE;
        }
        else
        {
            nArray = nFolderCount;
            return TRUE;
        }
    }
    else
        return FALSE;

    hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);

    for(i = 0 ; i < nFolderCount ; i++)
    {
        pNewNode = Folder_New();

        if(!pNewNode)
            return FALSE;
        
        Folder_Fill(pNewNode,pFolderInfo[i],i*sizeof(MU_FOLDERINFO));

        nInsert = Folder_Insert(&(pData->pFirst),pNewNode); 

        if(nInsert == -1)
        {
            Folder_Erase(&(pData->pFirst));
            return FALSE;
        }
        
        Folder_AddNodeToList(hLst,pNewNode,nInsert+1);
    }

    if(pFolderInfo != NULL)
    {
        free(pFolderInfo);
        pFolderInfo = NULL;
    }
    return TRUE;
}
/*********************************************************************\
* Function	   Folder_AddNodeToList
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Folder_AddNodeToList(HWND hWnd,MU_FOLDERCHAINNODE *pNewNode,int nInsert)
{
    int curIndex;
//    char szDisplay[100];

//    Folder_FormatDisplay(hWnd,szDisplay,100,pNewNode->folderinfo.szFolderName);
    curIndex = SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)nInsert, (LPARAM) (LPCTSTR)pNewNode->folderinfo.szFolderName);
    SendMessage(hWnd, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_ICON,(WORD)curIndex),(LPARAM)hBmp);
    SendMessage(hWnd, LB_SETITEMDATA, (WPARAM)curIndex, (LPARAM)pNewNode);
    return;
}
/*********************************************************************\
* Function	   Folder_FormatDisplay
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Folder_FormatDisplay(HWND hWnd, char *szDisplay, int nDisplayLen, char *szString)
{
#define LIST_WIDTH  (176 - 50 - 25)
    HDC hdc;
    char szTempStr[100];
    int nLen,nFit = 0;
    SIZE size;
    int SpaceWidth = 0,nSpaceCount = 0;

    szTempStr[0] = 0;

    hdc = GetDC(hWnd);
    
    nLen = strlen(szString);

    GetTextExtentExPoint(hdc, szString, nLen, LIST_WIDTH, &nFit, NULL, NULL);

    if(nFit == nLen)//add space
    {
        strcpy(szDisplay,szString);

        GetTextExtent(hdc, szString, nLen, &size);

        SpaceWidth = LIST_WIDTH - size.cx;

        GetTextExtent(hdc, " ", 1, &size);

        nSpaceCount = SpaceWidth/size.cx;

        while(nSpaceCount-- > 0)
            strcat(szDisplay," ");
    }
    else
        GetExtentFittedText(hdc, szString, nLen, szDisplay, nDisplayLen, LIST_WIDTH , '.', 3);

    ReleaseDC(hWnd, hdc);
    return;
}
/*********************************************************************\
* Function	   MUFolder_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void MUFolder_New(HWND hWnd, BOOL bNew, char* pszFileName)
{
    PMU_FOLDERCHAINNODE pNewNode;
    int nInsert = -1;
    HWND    hLst;
    MU_FOLDERDATA* pCreateData;
	int  nfolderid;
    
    if(bNew == FALSE)
        return;

    pCreateData = GetUserData(hWnd);
    
    hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);

    pNewNode = Folder_New();
    
    if(!pNewNode)
        return;

    pNewNode->folderinfo.nFolderID = MU_FindUseableID(pCreateData->pFirst);
    strcpy(pNewNode->folderinfo.szFolderName,pszFileName);
    pNewNode->folderinfo.bUsed = FOLDER_USED;

    pNewNode->dwoffset = Folder_AppendRecord(&pNewNode->folderinfo);
        
    nInsert = Folder_Insert(&(pCreateData->pFirst),pNewNode); 
    
    Folder_AddNodeToList(hLst,pNewNode,nInsert+1);

    pCreateData->nCurSel = nInsert+1;

    PostMessage(hWnd,WM_SETCURSEL,NULL,NULL);
	
    nfolderid = pNewNode->folderinfo.nFolderID;
    if(mu_initstatus.bSMS)
    {
        mu_sms_interface->msg_new_folder(nfolderid);
    }
    if(mu_initstatus.bMMS)
    {
        mu_mms_interface->msg_new_folder(nfolderid);
    }
    if(mu_initstatus.bEmail)
    {
        mu_email_interface->msg_new_folder(nfolderid);
    }
    if(mu_initstatus.bPush)
    {
        mu_push_interface->msg_new_folder(nfolderid);
    }
	if(mu_initstatus.bBT)
    {
        mu_bt_interface->msg_new_folder(nfolderid);
    }
}
/*********************************************************************\
* Function	   MUFolder_Rename
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void MUFolder_Rename(HWND hWnd, BOOL bRename, char* pszFileName)
{
    PMU_FOLDERCHAINNODE pChainNode = NULL,pNewNode = NULL;
    HWND    hLst;
    int     nCurSel,nInsert,nfolderid = -1;
    MU_FOLDERDATA* pCreateData;
        
    if(bRename == FALSE)
        return;
    
    pCreateData = GetUserData(hWnd);
    
    hLst = GetDlgItem(hWnd,IDC_MULSIT_LIST);
    
    nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
    if( nCurSel == LB_ERR || nCurSel == 0)
        return;
    
    pChainNode = (PMU_FOLDERCHAINNODE)SendMessage(hLst,LB_GETITEMDATA,nCurSel,0);
    if( !pChainNode )
        return;

    pNewNode = Folder_New();
    
    if(!pNewNode)
        return;

    pNewNode->folderinfo.bUsed = FOLDER_USED;
    pNewNode->folderinfo.nFolderID = pChainNode->folderinfo.nFolderID;
    strcpy(pNewNode->folderinfo.szFolderName,pszFileName);

    Folder_DeleteRecord(pChainNode->dwoffset);
    
    pNewNode->dwoffset = Folder_AppendRecord(&pNewNode->folderinfo);

    SendMessage(hLst,LB_DELETESTRING,nCurSel,0);

    Folder_Delete(&(pCreateData->pFirst),pChainNode);

    nInsert = Folder_Insert(&(pCreateData->pFirst),pNewNode); 
    
    Folder_AddNodeToList(hLst,pNewNode,nInsert+1);

    pCreateData->nCurSel = nInsert+1;

    PostMessage(hWnd,WM_SETCURSEL,NULL,NULL);

    nfolderid = pNewNode->folderinfo.nFolderID;
    if(mu_initstatus.bSMS)
    {
        mu_sms_interface->msg_new_folder(nfolderid);
    }
    if(mu_initstatus.bMMS)
    {
        mu_mms_interface->msg_new_folder(nfolderid);
    }
    if(mu_initstatus.bEmail)
    {
        mu_email_interface->msg_new_folder(nfolderid);
    }
    if(mu_initstatus.bPush)
    {
        mu_push_interface->msg_new_folder(nfolderid);
    }
	if(mu_initstatus.bBT)
    {
        mu_bt_interface->msg_new_folder(nfolderid);
    }
}
/*********************************************************************\
* Function	   Folder_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
MU_FOLDERCHAINNODE* Folder_New(void)
{
    MU_FOLDERCHAINNODE* p = NULL;

    p = (MU_FOLDERCHAINNODE*)malloc(sizeof(MU_FOLDERCHAINNODE));
    memset(p,0,sizeof(MU_FOLDERCHAINNODE));

    return p;
}
/*********************************************************************\
* Function	   Folder_Fill
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL Folder_Fill(MU_FOLDERCHAINNODE* pChainNode,MU_FOLDERINFO FolderInfo,DWORD dwoffset)
{
    if(!pChainNode)
        return FALSE;

    pChainNode->dwoffset = dwoffset;
    memcpy(&pChainNode->folderinfo,&FolderInfo,sizeof(MU_FOLDERINFO));

    return TRUE;
}
/*********************************************************************\
* Function	   Folder_Insert
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int Folder_Insert(MU_FOLDERCHAINNODE** ppHeader,MU_FOLDERCHAINNODE* pChainNode)
{
    if(*ppHeader == NULL)
    {
        *ppHeader = pChainNode;
        pChainNode->pPioneer = NULL;
        pChainNode->pNext = NULL;

        return 0;
    }
    else
    {
        MU_FOLDERCHAINNODE *p;
        int k;
        BOOL bIsTail;

        k = 0;
        bIsTail = FALSE;
        p = *ppHeader;
    
        while( stricmp(pChainNode->folderinfo.szFolderName,p->folderinfo.szFolderName) > 0 )
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
            if( p == *ppHeader)
            {
                pChainNode->pNext = *ppHeader;
                pChainNode->pPioneer = NULL;
                (*ppHeader)->pPioneer = pChainNode;
                *ppHeader = pChainNode;
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
* Function	   Folder_Delete
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Folder_Delete(MU_FOLDERCHAINNODE** ppHeader,MU_FOLDERCHAINNODE* pChainNode)
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
* Function	   Folder_Erase
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Folder_Erase(MU_FOLDERCHAINNODE** ppHeader)
{
    MU_FOLDERCHAINNODE* p;    
    MU_FOLDERCHAINNODE* ptemp;

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
* Function	MU_GetFolderInfo
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL MU_GetFolderInfo(PMU_FOLDERINFO pInfo,int* pnCount)
{
    char szOldPath[PATH_MAXLEN];

    szOldPath[0] = 0;

    if(pInfo)
    {
        struct stat buf;
        int f;
        int i,nCount = 0;   
        char szOldPath[PATH_MAXLEN];
        
        szOldPath[0] = 0;
        
        getcwd(szOldPath,PATH_MAXLEN);
        
        chdir(PATH_DIR_MU);
        
        memset(&buf,0,sizeof(struct stat));
        stat(PATH_FILE_FOLDER,(struct stat*)&buf);
        
        if(buf.st_size == 0)
        {
            chdir(szOldPath);
            return TRUE;
        }
        
        f = open(PATH_FILE_FOLDER,O_RDONLY);
        if(f == -1)
        {
            f = open(PATH_FILE_FOLDER, O_RDWR | O_CREAT, S_IRWXU);
            if(f == -1)
            {
                chdir(szOldPath);
                return FALSE;
            }
            else
            {
                close(f);
                chdir(szOldPath);
                return TRUE;
            }
        }
        
        if(buf.st_size <= MU_TEMPSPACE_MAX)
        {
            PMU_FOLDERINFO p;
            p = (PMU_FOLDERINFO)malloc(buf.st_size);
            
            if(p == NULL)
            {
                close(f);
                chdir(szOldPath);
                return FALSE;
            }
            
            read(f,p,buf.st_size);
            
            for(i = 0 ; i < (int)(buf.st_size / sizeof(MU_FOLDERINFO)) ; i++)
            {
                if(p[i].bUsed == FOLDER_UNUSED)
                    continue;
                else
                {
                    memcpy(&(pInfo[nCount]),&p[i],sizeof(MU_FOLDERINFO));
                    nCount++;
                }
            }
            
            free(p);
        }
        else
        {
            MU_FOLDERINFO temp;
            
            for(i = 0 ; i < (int)(buf.st_size / sizeof(MU_FOLDERINFO)) ; i++)
            {
                read(f,&temp,sizeof(MU_FOLDERINFO));
                if(temp.bUsed == FOLDER_UNUSED)
                    continue;
                else   
                {
                    memcpy(&(pInfo[nCount]),&temp,sizeof(MU_FOLDERINFO));
                    nCount++;
                }
            }
        }
        
        close(f);
        chdir(szOldPath);
        return TRUE;
    }
    else
    {
        struct stat buf;
        int f;
        int i;   
        char szOldPath[PATH_MAXLEN];
        
        szOldPath[0] = 0;
        
		*pnCount = 0;
        
        getcwd(szOldPath,PATH_MAXLEN);
        
        chdir(PATH_DIR_MU);
        
        memset(&buf,0,sizeof(struct stat));
        stat(PATH_FILE_FOLDER,(struct stat*)&buf);
        
        if(buf.st_size == 0)
        {
            chdir(szOldPath);
            *pnCount = 0;
            return TRUE;
        }
        
        f = open(PATH_FILE_FOLDER,O_RDONLY);
        if(f == -1)
        {
            f = open(PATH_FILE_FOLDER, O_RDWR | O_CREAT, S_IRWXU);
            if(f == -1)
            {
                chdir(szOldPath);
                return FALSE;
            }
            else
            {
                close(f);
                *pnCount = 0;
                chdir(szOldPath);
                return TRUE;
            }
        }
               
        if(buf.st_size <= MU_TEMPSPACE_MAX)
        {
            PMU_FOLDERINFO p;
            p = (PMU_FOLDERINFO)malloc(buf.st_size);
            
            if(p == NULL)
            {
                close(f);
                chdir(szOldPath);
                return FALSE;
            }

            read(f,p,buf.st_size);
            
            for(i = 0 ; i < (int)(buf.st_size / sizeof(MU_FOLDERINFO)) ; i++)
            {
                if(p[i].bUsed == FOLDER_UNUSED)
                    continue;
                else
                    (*pnCount)++;
            }
            
            free(p);
        }
        else
        {
            MU_FOLDERINFO temp;
            
            for(i = 0 ; i < (int)(buf.st_size / sizeof(MU_FOLDERINFO)) ; i++)
            {
                read(f,&temp,sizeof(MU_FOLDERINFO));
                if(temp.bUsed == FOLDER_UNUSED)
                    continue;
                else
                    (*pnCount)++;
            }
        }
        
        close(f);
        chdir(szOldPath);
        return TRUE;
    }
}

/*********************************************************************\
* Function	MU_CreateSmallWnd
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL MU_CreateSmallWnd(HWND hParent,HWND hWnd,UINT wMsgCmd ,char* szName, BOOL bNew,MU_FOLDERCHAINNODE* pHeader)
{
    WNDCLASS wc;
    MU_SMALLDATA SmallData;
    HWND hSmallWnd = NULL;
	RECT rClient;

    SmallData.hWnd = hWnd;
    SmallData.wMsgCmd = wMsgCmd;
	SmallData.bNew = bNew;
    SmallData.pFirst = pHeader;
    strcpy(SmallData.szFolderName,szName);

    wc.style         = 0;
    wc.lpfnWndProc   = MUSmallWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MU_SMALLDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "MUSMALLWndClass";
    
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

	GetClientRect(hMuFrame, &rClient);

	hSmallWnd = CreateWindow(
		"MUSMALLWndClass", 
        IDS_NEWFOLDER,
        WS_CHILD | WS_VISIBLE,
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hMuFrame,
        NULL,
		NULL, 
		(PVOID)&SmallData
		);
	
	if (!hSmallWnd)
	{
		UnregisterClass("MUSMALLWndClass", NULL);
		return FALSE;
	}
	
	if(bNew)
		SetWindowText(hMuFrame, IDS_NEWFOLDER);
	else
		SetWindowText(hMuFrame, IDS_RENAME_FOLDER);
	
	//show window
	ShowWindow(hSmallWnd, SW_SHOW);
	UpdateWindow(hSmallWnd);

    return TRUE;
}
/*********************************************************************\
* Function	MUSmallWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT MUSmallWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
//	HWND hEdt;
//	int Len;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MUSmall_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
		{
			PMU_SMALLDATA pUserData;
			pUserData = (PMU_SMALLDATA)GetUserData(hWnd);

			SetFocus(GetDlgItem(hWnd, IDC_MUSMALL_EDIT));
			MUSmall_OnActivate(hWnd,(UINT)LOWORD(wParam));
			
			if(pUserData->bNew)
				SetWindowText(hMuFrame, IDS_NEWFOLDER);
			else
				SetWindowText(hMuFrame, IDS_RENAME_FOLDER);
			
			//SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
			SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

			//hEdt = GetDlgItem(hWnd,IDC_MUSMALL_EDIT);
			
			//Len = GetWindowTextLength(hEdt);
        
            //if(Len == 0)
            //    SendMessage(hMuFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
            //else if(Len == 1) // if from 2 to 1 ,should not be care of
            SendMessage(hMuFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_SAVE);
		}
		
        break;
        
    case WM_PAINT:
        MUSmall_OnPaint(hWnd);
        break;

	case WM_SETLBTNTEXT:
		MUSmall_OnSetLBtnText(hWnd, (int)LOWORD(wParam), (BOOL)HIWORD(wParam),
            (LPSTR)lParam);

        lResult = 0;
		break;

    case WM_KEYDOWN:
        MUSmall_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        MUSmall_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
		SendMessage(hMuFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
        MUSmall_OnClose(hWnd);
        break;

    case WM_DESTROY:
        MUSmall_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;
}
/*********************************************************************\
* Function	MUSmall_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MUSmall_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    HWND hEdt;
    PMU_SMALLDATA p;
    RECT rc;
    IMEEDIT ie;

	//SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
	SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
	SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
	
    p = GetUserData(hWnd);

    memcpy(p,(PMU_SMALLDATA)lpCreateStruct->lpCreateParams,sizeof(MU_SMALLDATA));

    GetClientRect(hWnd,&rc);

    memset(&ie, 0, sizeof(IMEEDIT));
    
    ie.hwndNotify	= (HWND)hWnd;    
    ie.dwAttrib	    = 0;                
    ie.dwAscTextMax	= 0;
    ie.dwUniTextMax	= 0;
    ie.wPageMax	    = 0;        
    ie.pszCharSet	= NULL;
    ie.pszTitle	    = NULL;
    ie.pszImeName	= NULL;
    
    hEdt = CreateWindow(
        "IMEEDIT", 
        "", 
        WS_CHILD | WS_VISIBLE /*| WS_BORDER*/ | ES_AUTOHSCROLL | ES_TITLE ,
        rc.left,
        rc.top,
        rc.right - rc.left,
        (rc.bottom - rc.top)/3,
        hWnd, 
        (HMENU)IDC_MUSMALL_EDIT, 
        NULL, 
        (PVOID)&ie);
    
    if(hEdt == NULL)
        return FALSE;

    SendMessage(hEdt,EM_LIMITTEXT,MU_FOLDER_NAME_LEN,0);
    
    SendMessage(hEdt, EM_SETTITLE, 0, (LPARAM)IDS_FOLDERNAME);

    if(strlen(p->szFolderName) == 0)
    {
        if(MU_FindUseableName(p->pFirst,p->szFolderName))
        {
            SetWindowText(hEdt,p->szFolderName);
            SendMessage(hEdt,EM_SETSEL,-1,-1);
        }
    }
    else
    {
        SetWindowText(hEdt,p->szFolderName);
        SendMessage(hEdt,EM_SETSEL,-1,-1);
    }
    
    hFocus = hEdt;
    SetFocus(hFocus);
    return TRUE;
    
}
/*********************************************************************\
* Function	MUSmall_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MUSmall_OnActivate(HWND hwnd, UINT state)
{
    HWND hEdt;
    
    hEdt = GetDlgItem(hwnd,IDC_MUSMALL_EDIT);

    if(state == WA_ACTIVE)
    {
        SetFocus(hEdt);
    }

    return;
}

/*********************************************************************\
* Function	MUSmall_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUSmall_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);
        COLORREF OldClr;

	OldClr = SetBkColor(hdc, COLOR_TRANSBK);

	SetBkColor(hdc, OldClr);
	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	MUSmall_OnKey
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
static void MUSmall_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PMU_SMALLDATA pUserData;
    HWND hEdt;
//    char szRbtn[20];
    
    hEdt = GetDlgItem(hWnd,IDC_MUSMALL_EDIT);
    pUserData = (PMU_SMALLDATA)GetUserData(hWnd);

	switch (vk)
	{
	case VK_F10:
        SendMessage(pUserData->hWnd,pUserData->wMsgCmd,FALSE,NULL);

		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

    case VK_RETURN:
                
        GetWindowText(hEdt,pUserData->szFolderName,MU_FOLDER_NAME_LEN+1);

        pUserData->szFolderName[MU_FOLDER_NAME_LEN] = 0;

        if(MU_MoveBlankFromString(pUserData->szFolderName) == 0)
        {            
            if(pUserData->bNew)
                PLXTipsWin(NULL, NULL, 0, (char*)IDS_DEFINENAME, (char*)IDS_NEWFOLDER, Notify_Alert, (char*)IDS_OK, "", WAITTIMEOUT);
            else
                PLXTipsWin(NULL, NULL, 0, (char*)IDS_DEFINENAME, (char*)IDS_RENAME_FOLDER, Notify_Alert, (char*)IDS_OK, "", WAITTIMEOUT);
            break;
        }

//        if(MU_IsFolderNameUsed(pUserData->szFolderName))
//        {
////            PLXTipsWin((char*)IDS_FOLDERNAMEUSED,(char*)IDS_PROMPT,WAITTIMEOUT);
//
//            SendMessage(hEdt,EM_SETSEL,0,-1);
//        }
//        else
        {                
            PostMessage(hWnd,WM_CLOSE,0,0);

            SendMessage(pUserData->hWnd,pUserData->wMsgCmd,(WPARAM)TRUE,(LPARAM)pUserData->szFolderName);
            
        }
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	MUSmall_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MUSmall_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
//    PMU_SMALLDATA pUserData;
    HWND hEdt;
//    char szRbtn[20];
    
    hEdt = GetDlgItem(hWnd,IDC_MUSMALL_EDIT);
    
	switch(id)
	{	
    case IDC_MUSMALL_EDIT:
        
//        if(codeNotify == EN_CHANGE)
//        {
//            Len = GetWindowTextLength(hEdt);
//        
//            if(Len == 0)
//                SendMessage(hMuFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
//            else if(Len == 1) // if from 2 to 1 ,should not be care of
//                SendMessage(hMuFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_SAVE);
//         }
        
        break;

    default:
        break;
	}

	return;
}

static void MUSmall_OnSetLBtnText(HWND hWnd, int nID, BOOL bEmpty, LPSTR pszText)
{
	PMU_SMALLDATA pCreateData;

	pCreateData = GetUserData(hWnd);

	switch(nID)
	{
	case IDC_MUSMALL_EDIT:
		{
				//if(bEmpty)
				//{
				//	SendMessage(hMuFrame, PWM_SETBUTTONTEXT,1,(LPARAM)"");
				//}
				//else
				//{
					SendMessage(hMuFrame, PWM_SETBUTTONTEXT,1,(LPARAM)IDS_SAVE);
				//}
		}
		break;

	default:
		SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1,
            (LPARAM)pszText);
		break;
	}

	return;
}

/*********************************************************************\
* Function	MUSmall_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUSmall_OnDestroy(HWND hWnd)
{    
    
	UnregisterClass("MUSMALLWndClass",NULL);

    return;
}
/*********************************************************************\
* Function	MUSmall_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUSmall_OnClose(HWND hWnd)
{
    DestroyWindow (hWnd);

    return;
}
/*********************************************************************\
* Function	MU_FindUseableName
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL MU_FindUseableName(MU_FOLDERCHAINNODE* pHeader,char* szFolderName)
{
    int nLoop = 1;

    while( TRUE )
    {
        sprintf(szFolderName,IDS_FOLDERNAMETEMP2,nLoop);

        if(MU_IsFolderNameUsed(pHeader,szFolderName))
            nLoop++;
        else
            return TRUE;
    }

    return FALSE;
}
/*********************************************************************\
* Function	MU_IsFolderNameUsed
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL MU_IsFolderNameUsed(MU_FOLDERCHAINNODE* pHeader,char* pszFolderName)
{
    PMU_FOLDERCHAINNODE pTemp;
    
    pTemp = pHeader;
    
    while( pTemp )
    {
        //assert(pTemp->folderinfo.szFolderName);
        if(pTemp->folderinfo.nFolderID > MU_REPORT 
            && strcmp(pTemp->folderinfo.szFolderName,pszFolderName) == 0)
        {
            return TRUE;
        }
        pTemp = pTemp->pNext;
    }

    return FALSE;
}
/*********************************************************************\
* Function	MU_FindUseableID
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static int MU_FindUseableID(MU_FOLDERCHAINNODE* pHeader)
{
    int nIDTemp = MU_REPORT+1;

    while( nIDTemp )
    {
        if(MU_IsFolderIDUsed(pHeader,nIDTemp))
            nIDTemp++;
        else
            return nIDTemp;
    }

    return nIDTemp;
}

/*********************************************************************\
* Function	MU_IsFolderIDUsed
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL MU_IsFolderIDUsed(MU_FOLDERCHAINNODE* pHeader,int ID)
{
    PMU_FOLDERCHAINNODE pTemp;
    
    pTemp = pHeader;
    
    while( pTemp )
    {
        if(pTemp->folderinfo.nFolderID == ID)
        {
            return TRUE;
        }
        pTemp = pTemp->pNext;
    }

    return FALSE;
}
/*********************************************************************\
* Function	MU_ClearUp
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL MU_ClearUp(void)
{
    struct stat buf;
    int fold,fnew;
    int i;   
    char szOldPath[PATH_MAXLEN];

    szOldPath[0] = 0;

    getcwd(szOldPath,PATH_MAXLEN);
    
    chdir(PATH_DIR_MU);
    
    memset(&buf,0,sizeof(struct stat));
    stat(PATH_FILE_FOLDER,(struct stat*)&buf);

    if(buf.st_size == 0)
    {
        chdir(szOldPath);
        return TRUE;
    }
        
    fold = open(PATH_FILE_FOLDER,O_RDONLY);
    if(fold == -1)
    {
        fold = open(PATH_FILE_FOLDER, O_RDWR | O_CREAT, S_IRWXU);
        if(fold == -1)
        {
            chdir(szOldPath);
            return FALSE;
        }
        else
        {
            close(fold);
            chdir(szOldPath);
            return TRUE;
        }
    }
    
    fnew = open(PATH_FILE_TEMP, O_RDWR | O_CREAT, S_IRWXU);
    if(fnew == -1)
    {
        close(fold);
        chdir(szOldPath);
        return FALSE;
    }
    
    if(buf.st_size <= MU_TEMPSPACE_MAX)
    {
        PMU_FOLDERINFO p;
        p = (PMU_FOLDERINFO)malloc(buf.st_size);
        
        if(p == NULL)
        {
            close(fold);
            close(fnew);
            chdir(szOldPath);
            return FALSE;
        }
        
        read(fold,p,buf.st_size);
        
        for(i = 0 ; i < (int)(buf.st_size / sizeof(MU_FOLDERINFO)) ; i++)
        {
            if(p[i].bUsed == FOLDER_UNUSED)
                continue;
            else
                write(fnew,&p[i],sizeof(MU_FOLDERINFO));
        }
        
        free(p);
    }
    else
    {
        MU_FOLDERINFO temp;
        
        for(i = 0 ; i < (int)(buf.st_size / sizeof(MU_FOLDERINFO)) ; i++)
        {
            read(fold,&temp,sizeof(MU_FOLDERINFO));
            if(temp.bUsed == FOLDER_UNUSED)
                continue;
            else
                write(fnew,&temp,sizeof(MU_FOLDERINFO));
        }
    }
    
    close(fold);
    close(fnew);
    remove(PATH_FILE_FOLDER);
    rename(PATH_FILE_TEMP,PATH_FILE_FOLDER);
    chdir(szOldPath);
    return TRUE;
}
/*********************************************************************\
* Function	MU_CanMoveToFolder
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL MU_CanMoveToFolder(void)
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
* Function	Folder_AppendRecord
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static DWORD Folder_AppendRecord(MU_FOLDERINFO *pFolderinfo)
{
    struct stat buf;
    int f;
    char szOldPath[PATH_MAXLEN];

    szOldPath[0] = 0;

    getcwd(szOldPath,PATH_MAXLEN);
    
    chdir(PATH_DIR_MU);
    
    memset(&buf,0,sizeof(struct stat));
    stat(PATH_FILE_FOLDER,(struct stat*)&buf);
       
    f = open(PATH_FILE_FOLDER,O_RDWR);

    if(f == -1)
    {
        f = open(PATH_FILE_FOLDER, O_RDWR | O_CREAT, S_IRWXU);
        if(f == -1)
        {
            chdir(szOldPath);
            return -1;
        }
    }
    
    lseek(f,buf.st_size,SEEK_SET);

    write(f,pFolderinfo,sizeof(MU_FOLDERINFO));

    close(f);

    chdir(szOldPath);

    return(buf.st_size);
}
/*********************************************************************\
* Function	Folder_DeleteRecord
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL Folder_DeleteRecord(DWORD dwoffset)
{
    int f;
    char szOldPath[PATH_MAXLEN];
    BOOL bUsed = FOLDER_UNUSED;

    szOldPath[0] = 0;

    getcwd(szOldPath,PATH_MAXLEN);
    
    chdir(PATH_DIR_MU);
    
    f = open(PATH_FILE_FOLDER,O_RDWR);

    if(f == -1)
    {
        return FALSE;
    }
    
    lseek(f,dwoffset,SEEK_SET);

    write(f,&bUsed,sizeof(BOOL));

    close(f);

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	 MU_MoveBlankFromString  
* Purpose    
* Params	 
* Return	 	   
* Remarks	   
**********************************************************************/
static int MU_MoveBlankFromString(char * str)
{
	char * p;
	int slen;
	char stmp[MU_FOLDER_NAME_LEN+1];

	p = str;
	slen = strlen(str);
	if(0 == slen)
		return 0;
	while(p <= str + slen) 
	{
		if(' ' == *p)
		{
			p++;
		}
		else
		{
			break;
		}
	} 

	if(p == str + slen)
	{
		return 0;
	}
	else
	{
        stmp[MU_FOLDER_NAME_LEN] = 0;
		strcpy(stmp, p);
		strcpy(str, stmp);
	}

	slen = strlen(str);
	p = str + slen - 1;

	while(p >= str)
	{
		if(' ' == *p)
		{
			*p = '\0';
			p--;
		}
		else
			break;
	}
	return(strlen(str));
}
