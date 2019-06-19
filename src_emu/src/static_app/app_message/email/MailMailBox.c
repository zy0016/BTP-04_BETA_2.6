/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved
*
* Module   : MailMailBox.c
*
* Purpose  : 
*
\**************************************************************************/

#include "MailMailBox.h"

HWND    HwndMailInBox;
extern MailSendHead mMailSendHead;
char retr_option[128] = {'d','i','g','e','s','t','\0'};

/*********************************************************************\
* Function	   RegisterMailInBoxClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL RegisterMailInBoxClass(void)
{
    WNDCLASS wc;
        
    wc.style         = 0;
    wc.lpfnWndProc   = MailInBoxWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MAIL_INBOXCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "MailInBoxWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}

void UnRegisterMailInBoxClass(void)
{
    UnregisterClass("MailInBoxWndClass", NULL);
}
/*********************************************************************\
* Function	   CreateMailInBoxWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL CreateMailInBoxWnd(HWND hParent, const char *SZMAILBOXNAME)
{
    HMENU hMenu;
    MAIL_INBOXCREATEDATA CreateData;
    RECT rClient;
    char szDis[MAX_MAIL_BOXNAME_LEN + 10];
    
#ifdef MAIL_DEBUG
    StartObjectDebug();
#endif
    
    memset(&CreateData, 0x0, sizeof(MAIL_INBOXCREATEDATA));
    strcpy(CreateData.GLMailBoxName, SZMAILBOXNAME);
    
    CreateData.hFrameWnd = MuGetFrame();
    hMenu = CreateMenu();
    CreateData.hMenu = hMenu;
    GetClientRect(CreateData.hFrameWnd, &rClient);

    HwndMailInBox = CreateWindow(
        "MailInBoxWndClass",
        "",//IDP_MAIL_TITLE_EMAIL,
        WS_VISIBLE | WS_CHILD,// | WS_VSCROLL | PWS_STATICBAR | WS_CAPTION | WS_VISIBLE | WS_VSCROLL,
        rClient.left, 
        rClient.top, 
        rClient.right - rClient.left,
        rClient.bottom - rClient.top,
        CreateData.hFrameWnd, //hParent,
        NULL, //hMenu,
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!HwndMailInBox)
    {
        UnRegisterMailInBoxClass();
        return FALSE;
    }

    //show window
    //SetFocus(hList);
    PDASetMenu(CreateData.hFrameWnd, CreateData.hMenu);
    SetWindowText(CreateData.hFrameWnd, CreateData.GLMailBoxName);
    ShowWindow(CreateData.hFrameWnd, SW_SHOW);
    UpdateWindow(CreateData.hFrameWnd);

    memset(szDis, 0x0, MAX_MAIL_BOXNAME_LEN + 10);
    sprintf(szDis, "%s:\n%s", CreateData.GLMailBoxName, IDP_MAIL_STRING_CONNECT);
    
    PLXConfirmWinEx(CreateData.hFrameWnd, HwndMailInBox, szDis, Notify_Request, IDP_MAIL_TITLE_MAILBOX, 
        IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_BOX_RETURN);

    return TRUE;
}

/*********************************************************************\
* Function	MailInBoxWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT MailInBoxWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MailInBox_OnCreate(hWnd, (LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        MailInBox_OnActivate(hWnd, (UINT)LOWORD(wParam));
        break;
        
    case WM_INITMENU:
        MailInBox_OnInitmenu(hWnd);
        break;

    case WM_PAINT:
        MailInBox_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        MailInBox_OnKey(hWnd, (UINT)(wParam), (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        {
            PMAIL_INBOXCREATEDATA pCreateData;

            pCreateData = GetUserData(hWnd);

            if(wParam == (WPARAM)GetMenu(pCreateData->hFrameWnd))
            {
                //WM_INITMENU
                MailInBox_OnInitmenu(hWnd);
                break;
            }
            MailInBox_OnCommand(hWnd, (int)(LOWORD(wParam)), (UINT)HIWORD(wParam));
        }
        break;
        
    case WM_CLOSE:
        {
            PMAIL_INBOXCREATEDATA pCreateData;
            
            pCreateData = GetUserData(hWnd);

            SendMessage(pCreateData->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            MailInBox_OnClose(hWnd);
        }
        break;

    case WM_DESTROY:
        MailInBox_OnDestroy(hWnd);
        break;

    case WM_MAIL_BOX_RETURN:
        MailInBox_OnBoxReturn(hWnd, (WPARAM)wParam, (LPARAM)lParam);
        break;

    case WM_MAIL_FOLDER_RETURN:
        MailInBox_OnFolderReturn(hWnd, (WPARAM)wParam, (LPARAM)lParam);
        break;

    case PWM_MSG_MU_PRE:
        MailInBox_OnOpenPre(hWnd, (LPARAM)lParam);
        break;

    case PWM_MSG_MU_NEXT:
        MailInBox_OnOpenNext(hWnd, (LPARAM)lParam);
        break;

    case WM_MAIL_DELETE_RETURN:
        MailInBox_OnDeleteAtt(hWnd);
        break;
        
    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	MailInBox_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MailInBox_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    RECT rect;

    HWND hList;
    int hFile;
    MailConfigNode GetListNode;
    char szOutPath[PATH_MAXLEN];

    pCreateData = GetUserData(hWnd);
    memcpy(pCreateData, lpCreateStruct->lpCreateParams, sizeof(MAIL_INBOXCREATEDATA));
    if(!GlobalMailConfigHead.GLNetUsed)
    {
        GlobalMailConfigHead.GLInboxHandle = 0;
    }
    
    memset(&GetListNode, 0x0, sizeof(MailConfigNode));
    
    MAIL_GetConfigNodebyName(&GlobalMailConfigHead, &GetListNode, pCreateData->GLMailBoxName);
    strcpy(pCreateData->GLMailBoxSaveName, GetListNode.GLMailBoxSaveName);
    strcpy(pCreateData->GLMailConfigSaveName, GetListNode.GLMailConfigSaveName);
    pCreateData->GLFileNum = GetListNode.GLMailNum;

    memset(&pCreateData->GetConfig, 0x0, sizeof(MailConfig));
    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    hFile = GARY_open(pCreateData->GLMailBoxSaveName, O_RDONLY, -1);
    if(hFile < 0)
    {
        printf("\r\nMailInBox_OnCreate : open error = %d\r\n", errno);

        chdir(szOutPath);
        return FALSE;
    }
    if(read(hFile, &pCreateData->GetConfig, sizeof(MailConfig)) != sizeof(MailConfig))
    {
        GARY_close(hFile);
        chdir(szOutPath);
        return FALSE;
    }
    GARY_close(hFile);
    chdir(szOutPath);

    pCreateData->hRead      = LoadImage(NULL, MAIL_ICON_READ, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hReadAtt   = LoadImage(NULL, MAIL_ICON_READATT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hUnread    = LoadImage(NULL, MAIL_ICON_UNREAD, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hUnreadAtt = LoadImage(NULL, MAIL_ICON_UNREADATT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hUnretrRead = LoadImage(NULL, MAIL_ICON_UNRETRREAD, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hUnretr    = LoadImage(NULL, MAIL_ICON_UNRETR, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hUnretrAtt = LoadImage(NULL, MAIL_ICON_UNRETRATT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hDelete    = LoadImage(NULL, MAIL_ICON_DELETE, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);

    pCreateData->bConnect = FALSE;
    pCreateData->bConStatus = FALSE;
    pCreateData->bCreate = TRUE;
    pCreateData->bChange = FALSE;
    pCreateData->bReadAlreadyDel = FALSE;
    pCreateData->bRetrDigest = FALSE;

    MAIL_ReleaseGetList(&pCreateData->GetListHead);

    memset(&pCreateData->DelListHead, 0x0, sizeof(MailGetListHead));
    pCreateData->DelListHead.pNext = NULL;
    memset(&pCreateData->SelListHead, 0x0, sizeof(MailGetListHead));
    pCreateData->SelListHead.pNext = NULL;
    pCreateData->ReadNode = NULL;
    pCreateData->DelNode = NULL;

    MAIL_GetGetList(hWnd, &pCreateData->GetListHead, pCreateData->GLMailConfigSaveName);

    GetClientRect(pCreateData->hFrameWnd, &rect);

    hList = CreateWindow(
        "LISTBOX", 
        "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP | LBS_MULTILINE,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top, 
        hWnd, 
        (HMENU)IDC_MAIL_BOXMAIL_LIST, 
        NULL, 
        NULL);
    
    if(hList == NULL)
        return FALSE;

    SetFocus(hList);

    MAIL_AddNodeToList(hList, pCreateData);

    pCreateData->hSubReply = CreateMenu();
    AppendMenu(pCreateData->hSubReply, MF_STRING | MF_ENABLED, IDM_MAIL_REPLY_TOSENDER, IDP_MAIL_BUTTON_SELECT);
    AppendMenu(pCreateData->hSubReply, MF_STRING | MF_ENABLED, IDM_MAIL_REPLY_TOALL, IDP_MAIL_BUTTON_ALL);
    
    pCreateData->hSubMove = CreateMenu();
    AppendMenu(pCreateData->hSubMove, MF_STRING | MF_ENABLED, IDM_MAIL_MOVE_SELECT, IDP_MAIL_BUTTON_SELECT);
    AppendMenu(pCreateData->hSubMove, MF_STRING | MF_ENABLED, IDM_MAIL_MOVE_ALL, IDP_MAIL_BUTTON_ALL);
    
    pCreateData->hSubDelete = CreateMenu();
    AppendMenu(pCreateData->hSubDelete, MF_STRING | MF_ENABLED, IDM_MAIL_DELETE_SELECT, IDP_MAIL_BUTTON_SELECT);
    AppendMenu(pCreateData->hSubDelete, MF_STRING | MF_ENABLED, IDM_MAIL_DELETE_ALL, IDP_MAIL_BUTTON_ALL);

    SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), (LPARAM)IDP_MAIL_BUTTON_BACK);
    SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_OPEN, 1), (LPARAM)IDP_MAIL_BUTTON_OPEN);
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);

    pCreateData->bConfirm = Con_Create;

    return TRUE;
}

/*********************************************************************\
* Function	MailInBox_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MailInBox_OnActivate(HWND hWnd, UINT state)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    HWND hList;

    pCreateData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_BOXMAIL_LIST);

    /*if(state == WA_ACTIVE)
    {
        if(pCreateData->bCreate)
        {
            HWND tempFocus;

            pCreateData->bCreate = FALSE;
            tempFocus = GetDlgItem(hWnd, IDC_MAIL_BOXMAIL_LIST);
            SetFocus(tempFocus);
            return;
        }
    }*/

    SetFocus(hList);
    PDASetMenu(pCreateData->hFrameWnd, pCreateData->hMenu);
    SetWindowText(pCreateData->hFrameWnd, pCreateData->GLMailBoxName);
    if(pCreateData->GetListHead.pNext == NULL)
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    }
    else
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_OPEN);
    }
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_BACK);
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);

    SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    //PDADefWindowProc(hWnd, WM_ACTIVATE, state, 0);
    
    return;
}

/*********************************************************************\
* Function	MailInBox_OnInitmenu
* Purpose   WM_INITMENU message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailInBox_OnInitmenu(HWND hWnd)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    HMENU   hMenu;
    int iMenuCount;
    
    pCreateData = GetUserData(hWnd);
    //hMenu = PDAGetMenu(pCreateData->hFrameWnd);
    hMenu = pCreateData->hMenu;

    iMenuCount = GetMenuItemCount(hMenu);
    while(iMenuCount-- > 0)
    {
        RemoveMenu(hMenu, iMenuCount, MF_BYPOSITION);
    }
    if(pCreateData->GetListHead.pNext != NULL)
    {
        AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_OPEN, IDP_MAIL_BUTTON_OPEN);
        AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_REPLY, IDP_MAIL_BUTTON_REPLY);
        if(MAIL_AnalyseMailAddr(pCreateData->CurNode->GetFrom) > 1)
        {
            AppendMenu(hMenu, MF_STRING | MF_ENABLED | MF_POPUP, (UINT_PTR)pCreateData->hSubReply, IDP_MAIL_BUTTON_REPLYSUB);
        }
        if(pCreateData->CurNode->GetStatus != UNDOWN)
        {
            AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_FORWARD, IDP_MAIL_BUTTON_FORWARD);
        }
        if(pCreateData->CurNode->GetStatus != UNDOWN)//(MU_CanMoveToFolder())
        {
            AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_MOVE, IDP_MAIL_BUTTON_MOVETOFOLDER);
            AppendMenu(hMenu, MF_STRING | MF_ENABLED | MF_POPUP, (UINT_PTR)pCreateData->hSubMove, IDP_MAIL_BUTTON_MOVEMANY);
        }
        AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_INFO, IDP_MAIL_BUTTON_INFO);
    }
    if(pCreateData->bConStatus)
    {
        AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_DISCONNECT, IDP_MAIL_BUTTON_DISCONNECT);
    }
    else
    {
        AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_CONNECT, IDP_MAIL_BUTTON_CONNECT);
    }
    if(pCreateData->CurNode != NULL)
    {
        if(pCreateData->CurNode->GetStatus == UNDOWN)
        {
            AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_RETRIEVE, IDP_MAIL_BUTTON_RETRIEVE);
        }
        if((pCreateData->CurNode->GetStatus != UNDOWN) && (pCreateData->CurNode->GetReadFlag == UNREAD))
        {
            AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_MARK, IDP_MAIL_BUTTON_MARK);
        }
    }
    if(pCreateData->GetListHead.pNext != NULL)
    {
        AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_DELETE, IDP_MAIL_BUTTON_DELETE);
        AppendMenu(hMenu, MF_STRING | MF_ENABLED | MF_POPUP, (UINT_PTR)pCreateData->hSubDelete, IDP_MAIL_BUTTON_DELETEMANY);
    }
    
    return;
}

/*********************************************************************\
* Function	MailInBox_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailInBox_OnPaint(HWND hWnd)
{
	HDC hdc;
    COLORREF OldClr;
    RECT rcClient;
    
    hdc = BeginPaint(hWnd, NULL);
    OldClr = SetBkColor(hdc, COLOR_TRANSBK);
  
    GetClientRect(hWnd, &rcClient);
    DrawText(hdc, IDP_MAIL_STRING_NOEMAILS, -1, &rcClient, DT_CLEAR | DT_VCENTER | DT_CENTER);
    
    SetBkColor(hdc, OldClr);
    EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	MailInBox_OnKey
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
static void MailInBox_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    HWND hList;
    int CurSel;
    
    pCreateData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_BOXMAIL_LIST);

	switch (vk)
	{
    case VK_F10:
    //case VK_F2:
        {
            PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), 0);
        }
		break;

    case VK_RETURN:
        {
            PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_OPEN, 0), 0);
        }
        break;

    case VK_F5:
        {
            if(pCreateData->GetListHead.pNext != NULL)
            {
                CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
                MAIL_GetGetNodebyInt(&pCreateData->GetListHead, &pCreateData->CurNode, CurSel);
            }
            else
            {
                pCreateData->CurNode = NULL;
            }
            PDADefWindowProc(pCreateData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        }
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}

/*********************************************************************\
* Function	MailInBox_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MailInBox_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    HWND hList;
    int CurSel;
    MailGetListNode *pCurNode;
    int hFile;

    pCreateData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_BOXMAIL_LIST);

	switch(id)
	{
    case IDM_MAIL_BUTTON_OPEN:
        {
            if(pCreateData->GetListHead.pNext == NULL)
            {
                break;
            }

            CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
            pCurNode = NULL;
            MAIL_GetGetNodebyInt(&pCreateData->GetListHead, &pCurNode, CurSel);
            
            if(pCurNode->GetStatus != UNDOWN)
            {
                char szOutPath[PATH_MAXLEN];

                memset(szOutPath, 0x0, PATH_MAXLEN);
                getcwd(szOutPath, PATH_MAXLEN);
                chdir(MAIL_FILE_PATH);

                hFile = GARY_open(pCurNode->GetMailFileName, O_RDONLY, -1);
                if(hFile >= 0)
                {
                    BOOL ipre, inext;

                    GARY_close(hFile);
                    chdir(szOutPath);

                    if(pCurNode->GetReadFlag == UNREAD)
                    {
                        pCurNode->GetReadFlag = READ;
                        MAIL_GetSaveSaveNode(hWnd, pCurNode);
                        MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, 0);
                        MAIL_SetImage(hList, pCreateData, pCurNode, CurSel);
                    }

                    inext = (pCurNode->pNext != NULL) ? TRUE : FALSE;
                    ipre = (pCurNode == pCreateData->GetListHead.pNext) ? FALSE : TRUE;

                    MAIL_CreateViewWnd(hWnd, pCurNode->GetMailFileName, (DWORD)pCurNode->GetMsgId, MU_BOXMAIL, ipre, inext);
                    break;
                }
                else
                {
                    printf("\r\nMailInBox_OnCommand.IDM_MAIL_BUTTON_OPEN : open error = %d\r\n", errno);
                }
                chdir(szOutPath);
            }

            if(pCurNode->GetSize > MAX_MAIL_SIZE)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_TOOLARGE, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
                break;
            }
            if(MAIL_IsFlashEnough() == 0)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DRAFTFULL, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
                break;
            }
            
            pCreateData->ReadNode = pCurNode;
            pCreateData->ReadNode->GetReadFlag = READ;
            
            if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
                && GlobalMailConfigHead.GLInboxHandle && pCreateData->bConStatus)
            {
                pCreateData->bConnect = FALSE;
                MAIL_RetriHeaderFromServer(hWnd);
            }
            else
            {
                pCreateData->bConfirm = Con_Open;
                PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_MAIL_STRING_RETRNOW, 
                    Notify_Request, NULL, IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_BOX_RETURN);
            }
        }
        break;

    case IDM_MAIL_BUTTON_REPLY:
        {
            SYSTEMTIME sy;
            char szOutPath[PATH_MAXLEN];
            int hFile;
            char *cText;
            MailFile TempFile;

            CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
            pCurNode = NULL;
            MAIL_GetGetNodebyInt(&pCreateData->GetListHead, &pCurNode, CurSel);
            
            GetLocalTime(&sy);
            if(pCurNode->GetStatus == UNDOWN)
            {
                CreateMailEditWnd(MuGetFrame(), pCurNode->GetFrom, NULL, pCurNode->GetSubject, NULL, NULL, 
                    &sy, -1, -1);
                break;
            }
            
            memset(szOutPath, 0x0, PATH_MAXLEN);
            getcwd(szOutPath, PATH_MAXLEN);
            chdir(MAIL_FILE_PATH);
            hFile = GARY_open(pCurNode->GetMailFileName, O_RDONLY, -1);
            if(hFile < 0)
            {
                printf("\r\nMailInBox_OnCommand.IDM_MAIL_BUTTON_REPLY : open error = %d\r\n", errno);
                
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                CreateMailEditWnd(MuGetFrame(), pCurNode->GetFrom, NULL, pCurNode->GetSubject, NULL, NULL, 
                    &sy, -1, -1);
                chdir(szOutPath);

                break;
            }
            memset(&TempFile, 0x0, sizeof(MailFile));
            if(read(hFile, &TempFile, sizeof(MailFile)) != sizeof(MailFile))
            {
                GARY_close(hFile);
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                CreateMailEditWnd(MuGetFrame(), pCurNode->GetFrom, NULL, pCurNode->GetSubject, NULL, NULL, 
                    &sy, -1, -1);
                chdir(szOutPath);
                
                break;
            }
            cText = NULL;
            if(TempFile.TextSize > 0)
            {
                cText = (char *)malloc(TempFile.TextSize + 1);
                if(cText == NULL)
                {
                    GARY_close(hFile);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                    CreateMailEditWnd(MuGetFrame(), pCurNode->GetFrom, NULL, pCurNode->GetSubject, NULL, NULL, 
                        &sy, -1, -1);
                    chdir(szOutPath);
                    
                    break;
                }
                memset(cText, 0x0, TempFile.TextSize + 1);
                if(read(hFile, cText, TempFile.TextSize) != (int)TempFile.TextSize)
                {
                    GARY_FREE(cText);
                    GARY_close(hFile);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                    CreateMailEditWnd(MuGetFrame(), pCurNode->GetFrom, NULL, pCurNode->GetSubject, NULL, NULL, 
                        &sy, -1, -1);
                    chdir(szOutPath);
                    
                    break;
                }
            }
            GARY_close(hFile);
            chdir(szOutPath);

            CreateMailEditWnd(MuGetFrame(), pCurNode->GetFrom, NULL, pCurNode->GetSubject, cText, NULL, 
                &sy, -1, -1);
            GARY_FREE(cText);
        }
        break;

    case IDM_MAIL_REPLY_TOSENDER:
        {
            SYSTEMTIME sy;
            char szOutPath[PATH_MAXLEN];
            int hFile;
            char *cText;
            MailFile TempFile;
            
            CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
            pCurNode = NULL;
            MAIL_GetGetNodebyInt(&pCreateData->GetListHead, &pCurNode, CurSel);
            
            GetLocalTime(&sy);
            if(pCurNode->GetStatus == UNDOWN)
            {
                CreateMailEditWnd(MuGetFrame(), pCurNode->GetFrom, NULL, pCurNode->GetSubject, NULL, NULL, 
                    &sy, -1, -1);
                break;
            }

            memset(szOutPath, 0x0, PATH_MAXLEN);
            getcwd(szOutPath, PATH_MAXLEN);
            chdir(MAIL_FILE_PATH);
            hFile = GARY_open(pCurNode->GetMailFileName, O_RDONLY, -1);
            if(hFile < 0)
            {
                printf("\r\nMailInBox_OnCommand.IDM_MAIL_REPLY_TOSENDER : open error = %d\r\n", errno);

                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                CreateMailEditWnd(MuGetFrame(), pCurNode->GetFrom, NULL, pCurNode->GetSubject, NULL, NULL, 
                    &sy, -1, -1);
                chdir(szOutPath);
                
                break;
            }
            memset(&TempFile, 0x0, sizeof(MailFile));
            if(read(hFile, &TempFile, sizeof(MailFile)) != (int)sizeof(MailFile))
            {
                GARY_close(hFile);
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                CreateMailEditWnd(MuGetFrame(), pCurNode->GetFrom, NULL, pCurNode->GetSubject, NULL, NULL, 
                    &sy, -1, -1);
                chdir(szOutPath);
                
                break;
            }

            cText = NULL;
            if(TempFile.TextSize > 0)
            {
                cText = (char *)malloc(TempFile.TextSize + 1);
                if(cText == NULL)
                {
                    GARY_close(hFile);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                    CreateMailEditWnd(MuGetFrame(), pCurNode->GetFrom, NULL, pCurNode->GetSubject, NULL, NULL, 
                        &sy, -1, -1);
                    chdir(szOutPath);
                    
                    break;
                }
                memset(cText, 0x0, TempFile.TextSize + 1);
                if(read(hFile, cText, TempFile.TextSize) != (int)TempFile.TextSize)
                {
                    GARY_FREE(cText);
                    GARY_close(hFile);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                    CreateMailEditWnd(MuGetFrame(), pCurNode->GetFrom, NULL, pCurNode->GetSubject, NULL, NULL, 
                        &sy, -1, -1);
                    chdir(szOutPath);
                    
                    break;
                }
            }
            
            GARY_close(hFile);
            chdir(szOutPath);
            
            CreateMailEditWnd(MuGetFrame(), pCurNode->GetFrom, NULL, pCurNode->GetSubject, cText, NULL, 
                &sy, -1, -1);
            GARY_FREE(cText);
        }
        break;

    case IDM_MAIL_REPLY_TOALL:
        {
            char szOutPath[PATH_MAXLEN];

            int hFile;
            char *cText;
            MailFile TempFile;

            SYSTEMTIME sy;
            char *TempName;
            char *ToName;
            char *CcName;
            int NameLen;
            
            CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
            pCurNode = NULL;
            MAIL_GetGetNodebyInt(&pCreateData->GetListHead, &pCurNode, CurSel);
            
            GetLocalTime(&sy);
            if(MAIL_AnalyseMailAddr(pCurNode->GetTo) > 0)
            {
                //To
                NameLen = strlen(pCurNode->GetFrom) + strlen(pCurNode->GetTo);
                TempName = malloc(NameLen + 2);
                if(TempName == NULL)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 0);
                    break;
                }
                memset(TempName, 0x0, NameLen);
                strcpy(TempName, pCurNode->GetFrom);
                if(*(char *)(TempName + strlen(pCurNode->GetFrom)) != ';')
                {
                    strcat(TempName, ";");
                }
                strcat(TempName, pCurNode->GetTo);
                if(!MAIL_CombineReplyRec(&ToName, TempName, pCreateData->GetConfig.GLMailAddr))
                {
                    GARY_FREE(TempName);
                    
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 0);
                    break;
                }
                GARY_FREE(TempName);
            }
            else
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 0);
                break;
            }

            if(pCurNode->GetStatus == UNDOWN)
            {
                CreateMailEditWnd(MuGetFrame(), ToName, NULL, pCurNode->GetSubject, NULL, NULL, 
                    &sy, -1, -1);
                GARY_FREE(ToName);
                break;
            }
            
            memset(szOutPath, 0x0, PATH_MAXLEN);
            getcwd(szOutPath, PATH_MAXLEN);
            chdir(MAIL_FILE_PATH);
            hFile = GARY_open(pCurNode->GetMailFileName, O_RDONLY, -1);
            if(hFile < 0)
            {
                printf("\r\nMailInBox_OnCommand.IDM_MAIL_REPLY_TOALL : open error = %d\r\n", errno);

                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                CreateMailEditWnd(MuGetFrame(), ToName, NULL, pCurNode->GetSubject, NULL, NULL, 
                    &sy, -1, -1);
                chdir(szOutPath);
                GARY_FREE(ToName);
                
                break;
            }
            memset(&TempFile, 0x0, sizeof(MailFile));
            if(read(hFile, &TempFile, sizeof(MailFile)) != sizeof(MailFile))
            {
                GARY_close(hFile);
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                CreateMailEditWnd(MuGetFrame(), ToName, NULL, pCurNode->GetSubject, NULL, NULL, 
                    &sy, -1, -1);
                chdir(szOutPath);
                GARY_FREE(ToName);
                
                break;
            }
            cText = NULL;
            if(TempFile.TextSize > 0)
            {
                cText = (char *)malloc(TempFile.TextSize + 1);
                if(cText == NULL)
                {
                    GARY_close(hFile);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                    CreateMailEditWnd(MuGetFrame(), ToName, NULL, pCurNode->GetSubject, NULL, NULL, 
                        &sy, -1, -1);
                    chdir(szOutPath);
                    GARY_FREE(ToName);
                
                    break;
                }
                memset(cText, 0x0, TempFile.TextSize + 1);
                if(read(hFile, cText, TempFile.TextSize) != (int)TempFile.TextSize)
                {
                    GARY_FREE(cText);
                    GARY_close(hFile);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                    CreateMailEditWnd(MuGetFrame(), ToName, NULL, pCurNode->GetSubject, NULL, NULL, 
                        &sy, -1, -1);
                    chdir(szOutPath);
                    GARY_FREE(ToName);
                
                    break;
                }
            }
            GARY_close(hFile);
            chdir(szOutPath);

            //cc
            NameLen = strlen(TempFile.Cc);
            TempName = malloc(NameLen + 1);
            if(TempName == NULL)
            {
                GARY_FREE(ToName);
                
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 0);
                break;
            }
            memset(TempName, 0x0, NameLen);
            strcpy(TempName, TempFile.Cc);
            if(!MAIL_CombineReplyRec(&CcName, TempName, pCreateData->GetConfig.GLMailAddr))
            {
                GARY_FREE(TempName);
                
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                CreateMailEditWnd(MuGetFrame(), ToName, NULL, pCurNode->GetSubject, cText, NULL, 
                    &sy, -1, -1);
                GARY_FREE(ToName);
                GARY_FREE(cText);
                break;
            }
            GARY_FREE(TempName);
            
            CreateMailEditWnd(MuGetFrame(), ToName, CcName, pCurNode->GetSubject, cText, NULL, 
                &sy, -1, -1);
            GARY_FREE(ToName);
            GARY_FREE(CcName);
            GARY_FREE(cText);
        }
        break;
        
    case IDM_MAIL_BUTTON_FORWARD:
        {
            SYSTEMTIME sy;
            char szOutPath[PATH_MAXLEN];

            int hFile;
            char *cText;
            char *cAtt;
            MailFile TempFile;
            
            CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
            pCurNode = NULL;
            MAIL_GetGetNodebyInt(&pCreateData->GetListHead, &pCurNode, CurSel);
            
            GetLocalTime(&sy);

            if(pCurNode->GetStatus == UNDOWN)
            {
                break;
            }
            
            memset(szOutPath, 0x0, PATH_MAXLEN);
            getcwd(szOutPath, PATH_MAXLEN);
            chdir(MAIL_FILE_PATH);
            hFile = GARY_open(pCurNode->GetMailFileName, O_RDONLY, -1);
            if(hFile < 0)
            {
                printf("\r\nMailInBox_OnCommand.IDM_MAIL_BUTTON_FORWARD : open error = %d\r\n", errno);

                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                chdir(szOutPath);
                break;
            }
            memset(&TempFile, 0x0, sizeof(MailFile));
            if(read(hFile, &TempFile, sizeof(MailFile)) != sizeof(MailFile))
            {
                GARY_close(hFile);
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                chdir(szOutPath);
                break;
            }

            cText = NULL;
            cAtt = NULL;
            if(TempFile.TextSize > 0)
            {
                cText = (char *)malloc(TempFile.TextSize + 1);
                if(cText == NULL)
                {
                    GARY_close(hFile);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                    chdir(szOutPath);
                    break;
                }
                memset(cText, 0x0, TempFile.TextSize + 1);
                if(read(hFile, cText, TempFile.TextSize) != (int)TempFile.TextSize)
                {
                    GARY_FREE(cText);
                    GARY_close(hFile);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                    chdir(szOutPath);
                    break;
                }
            }
            if(TempFile.AttFileNameLen > 0)
            {
                cAtt = (char *)malloc(TempFile.AttFileNameLen + 1);
                if(cAtt == NULL)
                {
                    GARY_close(hFile);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                    CreateMailEditWnd(MuGetFrame(), NULL, NULL, pCurNode->GetSubject, cText, NULL, 
                        &sy, -1, -1);
                    GARY_FREE(cText);
                    chdir(szOutPath);
                    break;
                }
                memset(cAtt, 0x0, TempFile.AttFileNameLen + 1);
                if(read(hFile, cAtt, TempFile.AttFileNameLen) != (int)TempFile.AttFileNameLen)
                {
                    GARY_close(hFile);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                    CreateMailEditWnd(MuGetFrame(), NULL, NULL, pCurNode->GetSubject, cText, NULL, 
                        &sy, -1, -1);
                    GARY_FREE(cText);
                    GARY_FREE(cAtt);
                    chdir(szOutPath);
                    break;
                }
            }
            
            GARY_close(hFile);
            chdir(szOutPath);
            
            CreateMailEditWnd(MuGetFrame(), NULL, NULL, pCurNode->GetSubject, cText, cAtt, 
                &sy, -1, -1);
            GARY_FREE(cText);
            GARY_FREE(cAtt);
        }
        break;
        
    case IDM_MAIL_BUTTON_MOVE:
        {
            CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
            pCurNode = NULL;
            MAIL_GetGetNodebyInt(&pCreateData->GetListHead, &pCurNode, CurSel);
            if(pCurNode->GetStatus == UNDOWN)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CANNOTMOVE, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                break;
            }
            pCreateData->bConfirm = Con_MoveOne;
            MU_FolderSelection(NULL, hWnd, WM_MAIL_FOLDER_RETURN, -1);
        }
        break;
        
    case IDM_MAIL_MOVE_SELECT: // wait
        {
            BOOL Res = FALSE;
            
            if(pCreateData->SelListHead.pNext != NULL)
            {
                pCreateData->SelListHead.pNext = NULL;
            }
            pCurNode = pCreateData->GetListHead.pNext;
            while(pCurNode)
            {
                if(pCurNode->GetStatus != UNDOWN)
                {
                    Res = TRUE;
                    pCurNode->pSelNext = NULL;
                    MAIL_AddGetListNode(&pCreateData->SelListHead, pCurNode, REMOVE_SEL);
                }
                pCurNode = pCurNode->pNext;
            }
            if(!Res)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NORETRMAIL, NULL, 
                    Notify_Alert, IDP_MAIL_BOOTEN_OK, NULL, 20);
                break;
            }
            pCreateData->bConfirm = Con_MoveSelect;
            MAIL_SelectMsg(pCreateData->hFrameWnd, hWnd, 1, 0);
        }
        break;

    case IDM_MAIL_MOVE_ALL:
        {
            BOOL Res = FALSE;
            
            pCurNode = pCreateData->GetListHead.pNext;
            while(pCurNode)
            {
                if(pCurNode->GetStatus != UNDOWN)
                {
                    Res = TRUE;
                    break;
                }
                pCurNode = pCurNode->pNext;
            }
            if(!Res)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NORETRMAIL, NULL, 
                    Notify_Alert, IDP_MAIL_BOOTEN_OK, NULL, 20);
                break;
            }
            pCreateData->bConfirm = Con_MoveAll;
            MU_FolderSelection(NULL, hWnd, WM_MAIL_FOLDER_RETURN, -1);
        }
        break;

    case IDM_MAIL_BUTTON_INFO: 
        {
            MailFile detailFile;

            CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
            pCurNode = NULL;
            MAIL_GetGetNodebyInt(&pCreateData->GetListHead, &pCurNode, CurSel);

            memset(&detailFile, 0x0, sizeof(MailFile));
            strcpy(detailFile.address, pCurNode->GetFrom);
            strcpy(detailFile.From, pCurNode->GetFrom);
            strcpy(detailFile.Subject, pCurNode->GetSubject);
            MAIL_CharToSystem(pCurNode->GetDate, &detailFile.LocalDate);
            detailFile.MailSize = pCurNode->GetSize;

            CreateMailDetailWnd(pCreateData->hFrameWnd, &detailFile);
        }
        break;

    case IDM_MAIL_BUTTON_CONNECT:
        {
            MailConfigNode *pTemp;
            
            pCreateData->ReadNode = NULL;

            pTemp = GlobalMailConfigHead.pNext;
            while(pTemp)
            {
                if(strcmp(pTemp->GLMailBoxName, pCreateData->GetConfig.GLMailBoxName) == 0)
                {
                    break;
                }
                pTemp = pTemp->pNext;
            }
            if(pTemp)
            {
                if(GlobalMailConfigHead.GLNetUsed)
                {
                    WaitWin(HwndMailGet, TRUE, IDP_MAIL_STRING_CONNECTING, NULL, NULL, IDP_MAIL_BUTTON_CANCEL, WM_MAIL_CANCEL_RETURN);
                    MAIL_AddDialWaitNode(&GlobalMailConfigHead, pTemp, MAIL_CON_TYPE_SHOWREC);
                }
                else
                {
                    GlobalMailConfigHead.GLNetUsed = TRUE;
                    GlobalMailConfigHead.GLNetFlag = MAIL_CON_TYPE_SHOWREC;
                    strcpy(GlobalMailConfigHead.GLMailBoxName, pTemp->GLMailBoxName);
                    GlobalMailConfigHead.GLConStatus = MAILBOX_CON_IDLE;
                    GlobalMailConfigHead.CurConnectConfig = pTemp;
                    GlobalMailConfigHead.GLConnect = pTemp->GLConnect;
                    
                    pTemp->GLConType |= MAIL_CON_TYPE_SHOWREC;
                    pTemp->GLConStatus = MAILBOX_CON_IDLE;
                    
                    MAIL_ConnectServer(hWnd, MAIL_GET_CONNECT);
                }
            }
            else
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTFAL, NULL, Notify_Failure, 
                    IDP_MAIL_BOOTEN_OK, NULL, 20);
            }
        }
        break;

    case IDM_MAIL_BUTTON_DISCONNECT:
        {
            pCreateData->ReadNode = NULL;
            MAIL_ConnectServer(hWnd, MAIL_GET_DISCONNECT);
        }
        break;

    case IDM_MAIL_BUTTON_RETRIEVE:
        {
            CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
            pCurNode = NULL;
            MAIL_GetGetNodebyInt(&pCreateData->GetListHead, &pCurNode, CurSel);
            if(pCurNode->GetSize > MAX_MAIL_SIZE)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_TOOLARGE, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
                break;
            }
            if(!MAIL_IsSpaceEnough())
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DRAFTFULL, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
                return;
            }
            if(pCurNode->GetStatus != UNDOWN)
            {
                break;
            }
            pCreateData->ReadNode = pCurNode;
            pCreateData->ReadNode->GetReadFlag = RETRIEVE;
            if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
                && GlobalMailConfigHead.GLInboxHandle && pCreateData->bConStatus)
            {
                pCreateData->bConnect = FALSE;
                MAIL_RetriHeaderFromServer(hWnd);
            }
            else
            {
                pCreateData->bConfirm = Con_Open;
                PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_MAIL_STRING_RETRNOW, Notify_Request, NULL, 
                    IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_BOX_RETURN);
            }
        }
        break;

    case IDM_MAIL_BUTTON_MARK:
        {
            CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
            pCurNode = NULL;
            MAIL_GetGetNodebyInt(&pCreateData->GetListHead, &pCurNode, CurSel);
            if((pCurNode->GetReadFlag == UNREAD) && (pCurNode->GetStatus != UNDOWN))
            {
                pCurNode->GetReadFlag = READ;
                MAIL_GetSaveSaveNode(hWnd, pCurNode);
                MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, 0);
                MAIL_SetImage(hList, pCreateData, pCurNode, CurSel);
            }
        }
        break;

    case IDM_MAIL_BUTTON_DELETE:
        {
            CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
            pCurNode = NULL;
            MAIL_GetGetNodebyInt(&pCreateData->GetListHead, &pCurNode, CurSel);
            pCreateData->DelNode = pCurNode;
            if(pCurNode->GetStatus == UNDOWN)
            {
                pCreateData->bConfirm = Con_Delete;
                //pCreateData->bConfirm = Con_DelSelect;
                PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_MAIL_STRING_DELETEMSG,
                    Notify_Request, NULL, IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_BOX_RETURN);
            }
            else if(pCurNode->GetStatus == DOWNSER)
            {
                pCreateData->bConfirm = Con_Delete;
                PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_MAIL_STRING_DELETEMSG,
                    Notify_Request, NULL, IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_BOX_RETURN);
            }
            else if(pCurNode->GetStatus == DOWNLOCAL)
            {
                pCreateData->bConfirm = Con_Delete;
                //pCreateData->bConfirm = Con_DelSelect;
                PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_MAIL_STRING_DELETEMSG,
                    Notify_Request, NULL, IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_BOX_RETURN);
            }
        }
        break;

    case IDM_MAIL_DELETE_SELECT:
        {
            pCreateData->bConfirm = Con_DelSelectSel;
            MAIL_SelectMsg(pCreateData->hFrameWnd, hWnd, 0, 1);
        }
        break;

    case IDM_MAIL_DELETE_ALL:
        {
            pCreateData->bConfirm = Con_DeleteAll;
            PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_MAIL_STRING_DELALL,
                Notify_Request, NULL, IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_BOX_RETURN);
        }
        break;
    
    case IDM_MAIL_BUTTON_EXIT:
        {
            if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
                && GlobalMailConfigHead.GLInboxHandle && pCreateData->bConStatus)
            {
                char Display[MAX_MAIL_BOXNAME_LEN + 30];

                pCreateData->bConfirm = Con_Close;

                memset(Display, 0x0, MAX_MAIL_BOXNAME_LEN + 30);
                sprintf(Display, "%s:\n%s", pCreateData->GLMailBoxName, IDP_MAIL_STRING_DISCONNECTTO);
                PLXTipsWin(NULL, hWnd, WM_MAIL_BOX_RETURN, Display, NULL, 
                    Notify_Request, IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WAITTIMEOUT);
            }
            else
            {
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
        }
        break;
        
    case ID_MAIL_LIST_REFRESH:
        {
            MAIL_InstallMailBoxCount(pCreateData->GLMailBoxName);
            MAIL_AddNodeToList(hList, pCreateData);
        }
        break;

    default:
        break;
	}
    
	return;
}
/*********************************************************************\
* Function	MailInBox_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailInBox_OnDestroy(HWND hWnd)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    HMENU hMenu;

    pCreateData = GetUserData(hWnd);

    MAIL_ReleaseGetList(&pCreateData->GetListHead);
    
    if(pCreateData->hRead)
    {
        DeleteObject(pCreateData->hRead);
        pCreateData->hRead = NULL;
    }
    if(pCreateData->hReadAtt)
    {
        DeleteObject(pCreateData->hReadAtt);
        pCreateData->hReadAtt = NULL;
    }
    if(pCreateData->hUnread)
    {
        DeleteObject(pCreateData->hUnread);
        pCreateData->hUnread = NULL;
    }
    if(pCreateData->hUnreadAtt)
    {
        DeleteObject(pCreateData->hUnreadAtt);
        pCreateData->hUnreadAtt = NULL;
    }
    if(pCreateData->hUnretrRead)
    {
        DeleteObject(pCreateData->hUnretrRead);
        pCreateData->hUnretrRead = NULL;
    }
    if(pCreateData->hUnretr)
    {
        DeleteObject(pCreateData->hUnretr);
        pCreateData->hUnretr = NULL;
    }
    if(pCreateData->hUnretrAtt)
    {
        DeleteObject(pCreateData->hUnretrAtt);
        pCreateData->hUnretrAtt = NULL;
    }
    if(pCreateData->hDelete)
    {
        DeleteObject(pCreateData->hDelete);
        pCreateData->hDelete = NULL;
    }

    DestroyMenu(pCreateData->hSubDelete);
    DestroyMenu(pCreateData->hSubReply);
    DestroyMenu(pCreateData->hSubMove);

    hMenu = pCreateData->hMenu;
    //hMenu = PDAGetMenu(pCreateData->hFrameWnd);
    DestroyMenu(hMenu);

    if(!GlobalMailConfigHead.GLNetUsed)
    {
        GlobalMailConfigHead.CurConnectConfig = NULL;
    }
    
    HwndMailInBox = NULL;
    
#ifdef MAIL_DEBUG
    EndObjectDebug();
#endif
	
    return;
}
/*********************************************************************\
* Function	MailInBox_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailInBox_OnClose(HWND hWnd)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);

    //MU_NewMsgArrival();

    DestroyWindow(hWnd);
    
    return;
}

/*********************************************************************\
* Function	MailInBox_OnBoxReturn
* Purpose   WM_MAIL_BOX_RETURN message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MailInBox_OnBoxReturn(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    HWND hList;
    MailGetListNode *pDelNode;
    char szOutPath[PATH_MAXLEN];

    pCreateData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_BOXMAIL_LIST);

    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    switch(lParam)
    {
    case 0:
        {
            switch(pCreateData->bConfirm)
            {
            case Con_Create:
                {
                    SendMessage(hList, LB_SETCURSEL, 0, 0);
                }
                break;
                
            case Con_Open:
                {
                    pCreateData->ReadNode->GetReadFlag = UNREAD;
                    pCreateData->ReadNode = NULL;
                }
                break;

            case Con_Delete:
                {
                    pCreateData->DelNode = NULL;
                }
                break;

            case Con_DeleteSel:
                {
                }
                break;

            case Con_DeleteAll:
                {
                }
                break;

            case Con_DelSelect:
                {
                    if((pCreateData->DelNode->GetStatus == DOWNLOCAL) || (pCreateData->DelNode->GetStatus == DOWNSER))
                    {
                        if(MAIL_DeleteFile(pCreateData->DelNode->GetMailFileName))
                        {
                            int CurSel;

                            CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);

                            if(pCreateData->DelNode->GetStatus == DOWNLOCAL)
                            {
                                if(pCreateData->DelNode->GetReadFlag == UNREAD)
                                {
                                    MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, -1);
                                }
                                else
                                {
                                    MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, 0, -1);
                                }
                                pCreateData->DelNode->GetStatus = UNDOWN;
                                MAIL_DelSaveSaveNode(hWnd, pCreateData->DelNode);
                                SendMessage(hList, LB_DELETESTRING, CurSel, 0);
                                if(CurSel == 0)
                                {
                                    SendMessage(hList, LB_SETCURSEL, 0, 0);
                                }
                                else
                                {
                                    SendMessage(hList, LB_SETCURSEL, CurSel - 1, 0);
                                }
                                MAIL_DelGetListNode(&pCreateData->GetListHead, pCreateData->DelNode);
                                pCreateData->GetListHead.downlocal --;
                            }
                            if(pCreateData->DelNode->GetStatus == DOWNSER)
                            {
                                if(pCreateData->DelNode->GetReadFlag == UNREAD)
                                {
                                    MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, 0);
                                }

                                pCreateData->DelNode->GetStatus = UNDOWN;
                                MAIL_GetSaveSaveNode(hWnd, pCreateData->DelNode);
                                MAIL_SetImage(hList, pCreateData, pCreateData->DelNode, CurSel);
                                pCreateData->GetListHead.down --;
                            }

                            if((pCreateData->GetListHead.len == 0) || (pCreateData->GetListHead.pNext == NULL))
                            {
                                SendMessage(hList, LB_RESETCONTENT, 0, 0);
                                ShowWindow(hList, SW_HIDE);
                                //SendMessage(hList, LB_ADDSTRING, (WPARAM)NULL, (LPARAM)(LPCTSTR)IDP_MAIL_STRING_NOEMAILS);
                                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                                //SendMessage(hList, LB_SETCURSEL, 0, 0);
                            }

                            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELETED, NULL, 
                                Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                        }
                        else
                        {
                            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELFAIL, NULL, 
                                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                            pCreateData->bConfirm = Con_None;
                            return FALSE;
                        }
                    }
                    else
                    {
                        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELETED, NULL, 
                            Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                        pCreateData->bConfirm = Con_None;
                        return FALSE;
                    }
                }
                break;

            case Con_DelSelectSel:
                {                    
                    pDelNode = pCreateData->SelListHead.pNext;
                    while (pDelNode)
                    {
                        if((pDelNode->GetStatus == DOWNLOCAL) || (pDelNode->GetStatus == DOWNSER))
                        {
                            if(MAIL_DeleteFile(pDelNode->GetMailFileName))
                            {
                                int i;

                                i = MAIL_GetGetNodeInt(&pCreateData->GetListHead, pDelNode);
                                
                                if(pDelNode->GetStatus == DOWNLOCAL)
                                {
                                    MailGetListNode *pTemp;
                                    
                                    if(pDelNode->GetReadFlag == UNREAD)
                                    {
                                        MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, -1);
                                    }
                                    else
                                    {
                                        MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, 0, -1);
                                    }
                                    
                                    pDelNode->GetStatus = UNDOWN;
                                    MAIL_DelSaveSaveNode(hWnd, pDelNode);
                                    pTemp = pDelNode->pSelNext;
                                    SendMessage(hList, LB_DELETESTRING, i, 0);
                                    MAIL_DelGetListNode(&pCreateData->GetListHead, pDelNode);
                                    pCreateData->GetListHead.downlocal --;
                                    pDelNode = pTemp;
                                    
                                    continue;
                                }
                                if(pDelNode->GetStatus == DOWNSER)
                                {
                                    if(pDelNode->GetReadFlag == UNREAD)
                                    {
                                        MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, 0);
                                    }

                                    pDelNode->GetStatus = UNDOWN;
                                    MAIL_GetSaveSaveNode(hWnd, pDelNode);
                                    MAIL_SetImage(hList, pCreateData, pDelNode, i);
                                    pCreateData->GetListHead.down --;
                                }
                            }
                        }
                        pDelNode = pDelNode->pSelNext;
                    }
                    if((pCreateData->GetListHead.len == 0) || (pCreateData->GetListHead.pNext == NULL))
                    {
                        SendMessage(hList, LB_RESETCONTENT, 0, 0);
                        ShowWindow(hList, SW_HIDE);
                        //SendMessage(hList, LB_ADDSTRING, (WPARAM)NULL, (LPARAM)(LPCTSTR)IDP_MAIL_STRING_NOEMAILS);
                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                    }
                    SendMessage(hList, LB_SETCURSEL, 0, 0);
                    
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELETED, NULL, 
                        Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                }
                break;

            case Con_DelSelectAll:
                {
                    int i;

                    i = 0;
                    pDelNode = pCreateData->GetListHead.pNext;
                    while (pDelNode)
                    {
                        if((pDelNode->GetStatus == DOWNLOCAL) || (pDelNode->GetStatus == DOWNSER))
                        {
                            if(MAIL_DeleteFile(pDelNode->GetMailFileName))
                            {
                                if(pDelNode->GetStatus == DOWNLOCAL)
                                {
                                    MailGetListNode *pTemp;
                                    
                                    if(pDelNode->GetReadFlag == UNREAD)
                                    {
                                        MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, -1);
                                    }
                                    else
                                    {
                                        MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, 0, -1);
                                    }
                                    
                                    pDelNode->GetStatus = UNDOWN;
                                    MAIL_DelSaveSaveNode(hWnd, pDelNode);
                                    pTemp = pDelNode->pNext;
                                    SendMessage(hList, LB_DELETESTRING, i, 0);
                                    i --;
                                    MAIL_DelGetListNode(&pCreateData->GetListHead, pDelNode);
                                    pCreateData->GetListHead.downlocal --;
                                    pDelNode = pTemp;

                                    continue;
                                }
                                if(pDelNode->GetStatus == DOWNSER)
                                {
                                    if(pDelNode->GetReadFlag == UNREAD)
                                    {
                                        MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, 0);
                                    }

                                    pDelNode->GetStatus = UNDOWN;
                                    MAIL_GetSaveSaveNode(hWnd, pDelNode);
                                    MAIL_SetImage(hList, pCreateData, pDelNode, i);
                                    pCreateData->GetListHead.down --;
                                }
                            }
                        }
                        pDelNode = pDelNode->pNext;
                        i ++;
                    }
                    
                    if((pCreateData->GetListHead.len == 0) || (pCreateData->GetListHead.pNext == NULL))
                    {
                        SendMessage(hList, LB_RESETCONTENT, 0, 0);
                        ShowWindow(hList, SW_HIDE);
                        //SendMessage(hList, LB_ADDSTRING, (WPARAM)NULL, (LPARAM)(LPCTSTR)IDP_MAIL_STRING_NOEMAILS);
                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                    }
                    SendMessage(hList, LB_SETCURSEL, 0, 0);

                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELETED, NULL, 
                        Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                }
                break;

            case Con_Close:
                {
                }
                break;
            }
            pCreateData->bConfirm = Con_None;
        }
        break;
        
    case 1:
        {
            switch(pCreateData->bConfirm)
            {
            case Con_Create:
                {
                    MailConfigNode *pTemp;

                    pCreateData->ReadNode = NULL;

                    pTemp = GlobalMailConfigHead.pNext;
                    while(pTemp)
                    {
                        if(strcmp(pTemp->GLMailBoxName, pCreateData->GetConfig.GLMailBoxName) == 0)
                        {
                            break;
                        }
                        pTemp = pTemp->pNext;
                    }
                    if(pTemp)
                    {
                        if(GlobalMailConfigHead.GLNetUsed)
                        {
                            WaitWin(HwndMailGet, TRUE, IDP_MAIL_STRING_CONNECTING, NULL, NULL, IDP_MAIL_BUTTON_CANCEL, WM_MAIL_CANCEL_RETURN);
                            MAIL_AddDialWaitNode(&GlobalMailConfigHead, pTemp, MAIL_CON_TYPE_SHOWREC);
                        }
                        else
                        {
                            GlobalMailConfigHead.GLNetUsed = TRUE;
                            GlobalMailConfigHead.GLNetFlag = MAIL_CON_TYPE_SHOWREC;
                            strcpy(GlobalMailConfigHead.GLMailBoxName, pTemp->GLMailBoxName);
                            GlobalMailConfigHead.GLConStatus = MAILBOX_CON_IDLE;
                            GlobalMailConfigHead.CurConnectConfig = pTemp;
                            GlobalMailConfigHead.GLConnect = pTemp->GLConnect;

                            pTemp->GLConType |= MAIL_CON_TYPE_SHOWREC;
                            pTemp->GLConStatus = MAILBOX_CON_IDLE;
                            
                            MAIL_ConnectServer(hWnd, MAIL_GET_CONNECT);
                        }
                    }
                    else
                    {
                        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTFAL, NULL, Notify_Failure, 
                            IDP_MAIL_BOOTEN_OK, NULL, 20);
                    }
                }
                break;

            case Con_Open:
                {
                    if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
                        && pCreateData->bConStatus)
                    {
                        MAIL_RetriHeaderFromServer(hWnd);
                    }
                    else
                    {
                        MailConfigNode *pTemp;
                        
                        pTemp = GlobalMailConfigHead.pNext;
                        while(pTemp)
                        {
                            if(strcmp(pTemp->GLMailBoxName, pCreateData->GetConfig.GLMailBoxName) == 0)
                            {
                                break;
                            }
                            pTemp = pTemp->pNext;
                        }
                        if(pTemp)
                        {
                            if(GlobalMailConfigHead.GLNetUsed)
                            {
                                WaitWin(HwndMailGet, TRUE, IDP_MAIL_STRING_CONNECTING, NULL, NULL, IDP_MAIL_BUTTON_CANCEL, WM_MAIL_CANCEL_RETURN);
                                MAIL_AddDialWaitNode(&GlobalMailConfigHead, pTemp, MAIL_CON_TYPE_SHOWREC);
                            }
                            else
                            {
                                GlobalMailConfigHead.GLNetUsed = TRUE;
                                GlobalMailConfigHead.GLNetFlag = MAIL_CON_TYPE_SHOWREC;
                                strcpy(GlobalMailConfigHead.GLMailBoxName, pTemp->GLMailBoxName);
                                GlobalMailConfigHead.GLConStatus = MAILBOX_CON_IDLE;
                                GlobalMailConfigHead.CurConnectConfig = pTemp;
                                GlobalMailConfigHead.GLConnect = pTemp->GLConnect;
                                
                                pTemp->GLConType |= MAIL_CON_TYPE_SHOWREC;
                                pTemp->GLConStatus = MAILBOX_CON_IDLE;
                                
                                MAIL_ConnectServer(hWnd, MAIL_GET_CONNECT);
                            }
                        }
                        else
                        {
                            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RETRIEVRAL, NULL, Notify_Failure, 
                                IDP_MAIL_BOOTEN_OK, NULL, 20);
                        }
                    }
                }
                break;

            case Con_Delete:
                {
                    pCreateData->bConfirm = Con_DelSelect;
                    PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_MAIL_STRING_DELSERVER, 
                        Notify_Request, NULL, IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_BOX_RETURN);
                }
                break;

            case Con_DeleteSel:
                {
                    pCreateData->bConfirm = Con_DelSelectSel;
                    PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_MAIL_STRING_DELSERVERS, 
                        Notify_Request, NULL, IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_BOX_RETURN);
                }
                break;

            case Con_DeleteAll:
                {
                    pCreateData->bConfirm = Con_DelSelectAll;
                    PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_MAIL_STRING_DELSERVERS, 
                        Notify_Request, NULL, IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_BOX_RETURN);
                }
                break;

            case Con_DelSelect:
                {
                    int CurSel;

                    CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
                    
                    if(pCreateData->DelNode->GetStatus == DOWNLOCAL)
                    {
                        if(MAIL_DeleteFile(pCreateData->DelNode->GetMailFileName))
                        {
                            if(pCreateData->DelNode->GetReadFlag == UNREAD)
                            {
                                MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, -1);
                            }
                            else
                            {
                                MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, 0, -1);
                            }
                            
                            pCreateData->DelNode->GetStatus = UNDOWN;
                            MAIL_DelSaveSaveNode(hWnd, pCreateData->DelNode);
                            SendMessage(hList, LB_DELETESTRING, CurSel, 0);
                            if(CurSel == 0)
                            {
                                SendMessage(hList, LB_SETCURSEL, 0, 0);
                            }
                            else
                            {
                                SendMessage(hList, LB_SETCURSEL, CurSel - 1, 0);
                            }
                            MAIL_DelGetListNode(&pCreateData->GetListHead, pCreateData->DelNode);
                            pCreateData->GetListHead.downlocal --;
                        }
                        else
                        {
                            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELFAIL, NULL, 
                                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                        }
                    }
                    else if(pCreateData->DelNode->GetStatus == DOWNSER)
                    {
                        MAIL_DeleteFile(pCreateData->DelNode->GetMailFileName);
                        pCreateData->DelNode->GetStatus = UNDOWN;

                        if(pCreateData->DelNode->GetReadFlag == UNREAD)
                        {
                            MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, 0);
                        }
                        
                        if(pCreateData->DelNode->GetServerFlag == UNDELSERVER)
                        {
                            pCreateData->DelNode->GetServerFlag = DELSERVER;
                            if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
                                && GlobalMailConfigHead.GLInboxHandle && pCreateData->bConStatus)
                            {
                                MAIL_AddGetListNode(&pCreateData->DelListHead, pCreateData->DelNode, REMOVE_DEL);
                            }
                            else
                            {
                                MAIL_GetSaveSaveNode(hWnd, pCreateData->DelNode);
                            }
                        }

                        MAIL_SetImage(hList, pCreateData, pCreateData->DelNode, CurSel);
                        pCreateData->GetListHead.down --;
                    }
                    else if(pCreateData->DelNode->GetStatus == UNDOWN)
                    {
                        if(pCreateData->DelNode->GetServerFlag == UNDELSERVER)
                        {
                            pCreateData->DelNode->GetServerFlag = DELSERVER;
                            if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
                                && GlobalMailConfigHead.GLInboxHandle && pCreateData->bConStatus)
                            {
                                MAIL_AddGetListNode(&pCreateData->DelListHead, pCreateData->DelNode, REMOVE_DEL);
                            }
                            else
                            {
                                MAIL_GetSaveSaveNode(hWnd, pCreateData->DelNode);
                            }
                        }

                        MAIL_SetImage(hList, pCreateData, pCreateData->DelNode, CurSel);
                    }
                    else
                    {
                        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELFAIL, NULL, 
                            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                        return FALSE;
                    }
                    
                    if((pCreateData->GetListHead.len == 0) || (pCreateData->GetListHead.pNext == NULL))
                    {
                        SendMessage(hList, LB_RESETCONTENT, 0, 0);
                        ShowWindow(hList, SW_HIDE);
                        //SendMessage(hList, LB_ADDSTRING, (WPARAM)NULL, (LPARAM)(LPCTSTR)IDP_MAIL_STRING_NOEMAILS);
                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                        //SendMessage(hList, LB_SETCURSEL, 0, 0);
                    }
                    
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELETED, NULL, 
                        Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                }
                break;

            case Con_DelSelectSel:
                {
                    pDelNode = pCreateData->SelListHead.pNext;
                    while (pDelNode)
                    {
                        int i;
                        
                        i = MAIL_GetGetNodeInt(&pCreateData->GetListHead, pDelNode);

                        if(pDelNode->GetStatus == DOWNLOCAL)
                        {
                            if(MAIL_DeleteFile(pDelNode->GetMailFileName))
                            {
                                MailGetListNode *pTemp;
                                
                                if(pDelNode->GetReadFlag == UNREAD)
                                {
                                    MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, -1);
                                }
                                else
                                {
                                    MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, 0, -1);
                                }
                                
                                pDelNode->GetStatus = UNDOWN;
                                MAIL_DelSaveSaveNode(hWnd, pDelNode);
                                pTemp = pDelNode->pSelNext;
                                SendMessage(hList, LB_DELETESTRING, i, 0);
                                MAIL_DelGetListNode(&pCreateData->GetListHead, pDelNode);
                                pCreateData->GetListHead.downlocal --;
                                pDelNode = pTemp;
                                
                                continue;
                            }
                        }
                        else if(pDelNode->GetStatus == DOWNSER)
                        {
                            if(MAIL_DeleteFile(pDelNode->GetMailFileName))
                            {
                                pDelNode->GetStatus = UNDOWN;
                                
                                if(pDelNode->GetReadFlag == UNREAD)
                                {
                                    MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, 0);
                                }
                            }
                            
                            if(pDelNode->GetServerFlag == UNDELSERVER)
                            {
                                pDelNode->GetServerFlag = DELSERVER;
                                if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
                                    && GlobalMailConfigHead.GLInboxHandle && pCreateData->bConStatus)
                                {
                                    MAIL_AddGetListNode(&pCreateData->DelListHead, pDelNode, REMOVE_DEL);
                                }
                                else
                                {
                                    MAIL_GetSaveSaveNode(hWnd, pDelNode);
                                }
                            }
                            
                            MAIL_SetImage(hList, pCreateData, pDelNode, i);
                            pCreateData->GetListHead.down --;
                        }
                        else if((pDelNode->GetStatus == UNDOWN) && (pDelNode->GetServerFlag != DELSERVER))
                        {
                            if(pDelNode->GetServerFlag == UNDELSERVER)
                            {
                                pDelNode->GetServerFlag = DELSERVER;
                                if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
                                    && GlobalMailConfigHead.GLInboxHandle && pCreateData->bConStatus)
                                {
                                    MAIL_AddGetListNode(&pCreateData->DelListHead, pDelNode, REMOVE_DEL);
                                }
                                else
                                {
                                    MAIL_GetSaveSaveNode(hWnd, pDelNode);
                                }
                            }
                            
                            MAIL_SetImage(hList, pCreateData, pDelNode, i);
                        }
                        pDelNode = pDelNode->pSelNext;
                    }

                    if((pCreateData->GetListHead.len == 0) || (pCreateData->GetListHead.pNext == NULL))
                    {
                        SendMessage(hList, LB_RESETCONTENT, 0, 0);
                        ShowWindow(hList, SW_HIDE);
                        //SendMessage(hList, LB_ADDSTRING, (WPARAM)NULL, (LPARAM)(LPCTSTR)IDP_MAIL_STRING_NOEMAILS);
                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                    }
                    SendMessage(hList, LB_SETCURSEL, 0, 0);

                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELETED, NULL, 
                        Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                }
                break;

            case Con_DelSelectAll:
                {
                    int i;

                    i = 0;
                    pDelNode = pCreateData->GetListHead.pNext;
                    while (pDelNode)
                    {
                        if(pDelNode->GetStatus == DOWNLOCAL)
                        {
                            if(MAIL_DeleteFile(pDelNode->GetMailFileName))
                            {
                                MailGetListNode *pTemp;

                                if(pDelNode->GetReadFlag == UNREAD)
                                {
                                    MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, -1);
                                }
                                else
                                {
                                    MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, 0, -1);
                                }
                                
                                pDelNode->GetStatus = UNDOWN;
                                MAIL_DelSaveSaveNode(hWnd, pDelNode);
                                pTemp = pDelNode->pNext;
                                SendMessage(hList, LB_DELETESTRING, i, 0);
                                i --;
                                MAIL_DelGetListNode(&pCreateData->GetListHead, pDelNode);
                                pCreateData->GetListHead.downlocal --;
                                pDelNode = pTemp;

                                continue;
                            }
                        }
                        else if(pDelNode->GetStatus == DOWNSER)
                        {
                            if(MAIL_DeleteFile(pDelNode->GetMailFileName))
                            {
                                pDelNode->GetStatus = UNDOWN;

                                if(pDelNode->GetReadFlag == UNREAD)
                                {
                                    MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, 0);
                                }
                            }
                            
                            if(pDelNode->GetServerFlag == UNDELSERVER)
                            {
                                pDelNode->GetServerFlag = DELSERVER;
                                if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
                                    && GlobalMailConfigHead.GLInboxHandle && pCreateData->bConStatus)
                                {
                                    MAIL_AddGetListNode(&pCreateData->DelListHead, pDelNode, REMOVE_DEL);
                                }
                                else
                                {
                                    MAIL_GetSaveSaveNode(hWnd, pDelNode);
                                }
                            }
                            
                            MAIL_SetImage(hList, pCreateData, pDelNode, i);
                            pCreateData->GetListHead.down --;
                        }
                        else if((pDelNode->GetStatus == UNDOWN) && (pDelNode->GetServerFlag != DELSERVER))
                        {
                            if(pDelNode->GetServerFlag == UNDELSERVER)
                            {
                                pDelNode->GetServerFlag = DELSERVER;
                                if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE) 
                                    && GlobalMailConfigHead.GLInboxHandle && pCreateData->bConStatus)
                                {
                                    MAIL_AddGetListNode(&pCreateData->DelListHead, pDelNode, REMOVE_DEL);
                                }
                                else
                                {
                                    MAIL_GetSaveSaveNode(hWnd, pDelNode);
                                }
                            }
                            
                            MAIL_SetImage(hList, pCreateData, pDelNode, i);
                        }
                        i ++;
                        pDelNode = pDelNode->pNext;
                    }

                    if((pCreateData->GetListHead.len == 0) || (pCreateData->GetListHead.pNext == NULL))
                    {
                        SendMessage(hList, LB_RESETCONTENT, 0, 0);
                        ShowWindow(hList, SW_HIDE);
                        //SendMessage(hList, LB_ADDSTRING, (WPARAM)NULL, (LPARAM)(LPCTSTR)IDP_MAIL_STRING_NOEMAILS);
                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                    }
                    SendMessage(hList, LB_SETCURSEL, 0, 0);
                    
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELETED, NULL, 
                        Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                }
                break;
                
            case Con_Close:
                {
                    pCreateData->ReadNode = NULL;
                    MAIL_ConnectServer(hWnd, MAIL_GET_DISCONNECT);
                }
                break;
            }
        }
        break;

    case 2:
        {
            if(pCreateData->bConfirm == Con_Close)
            {
                pCreateData->ReadNode = NULL;
                MAIL_ConnectServer(hWnd, MAIL_GET_DISCONNECT);
            }
        }
        break;
        
    default:
        break;
    }

    chdir(szOutPath);
    return TRUE;
}

void MailInBox_OnFolderReturn(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    HWND hList;

    int CurList;
    MailGetListNode *pTemp;
    
    char szOutPath[PATH_MAXLEN];
    char szNewFile[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    char szTempFile[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    
    pCreateData = GetUserData(hWnd);
    
    if(!wParam)
    {
        return;
    }

    if(pCreateData->bConfirm == Con_MoveOne)
    {
        hList = GetDlgItem(hWnd, IDC_MAIL_BOXMAIL_LIST);
        CurList = SendMessage(hList, LB_GETCURSEL, 0, 0);
        MAIL_GetGetNodebyInt(&pCreateData->GetListHead, &pTemp, CurList);
        
        if(pTemp->GetStatus == UNDOWN)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CANNOTMOVE, NULL, 
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            
            return;
        }
    }
    else if(pCreateData->bConfirm == Con_MoveAll)
    {
        pTemp = pCreateData->GetListHead.pNext;
    }
    else if(pCreateData->bConfirm == Con_MoveSelect)
    {
        pTemp = pCreateData->SelListHead.pNext;
    }
    else
    {
        return;
    }
    
    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    memset(szTempFile, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
    memset(szNewFile, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
    
    switch(lParam)
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
            sprintf(szTempFile, szNewFile, (int)lParam);
            sprintf(szNewFile, "%s%s", szTempFile, MAIL_FOLDER_FILE_TAIL);
        }
    }

    if(pCreateData->bConfirm == Con_MoveOne)
    {
        MAIL_GetNewFileName(szNewFile, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        MAIL_CopyMail(szNewFile, pTemp->GetMailFileName);
        
        if(pTemp->GetReadFlag == UNREAD)
        {
            MAIL_AddCountNode(&GlobalMailCountHead, lParam, 1);
        }
        else if(pTemp->GetReadFlag == READ)
        {
            MAIL_AddCountNode(&GlobalMailCountHead, lParam, 0);
        }

        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MSGMOVED, NULL, 
            Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
    }
    else if(pCreateData->bConfirm == Con_MoveAll)
    {
        memset(szTempFile, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        strcpy(szTempFile, szNewFile);
        while(pTemp)
        {
            if(pTemp->GetStatus != UNDOWN)
            {
                memset(szNewFile, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
                strcpy(szNewFile, szTempFile);
                
                MAIL_GetNewFileName(szNewFile, MAX_MAIL_SAVE_FILENAME_LEN + 1);
                MAIL_CopyMail(szNewFile, pTemp->GetMailFileName);
                
                if(pTemp->GetReadFlag == UNREAD)
                {
                    MAIL_AddCountNode(&GlobalMailCountHead, lParam, 1);
                }
                else if(pTemp->GetReadFlag == READ)
                {
                    MAIL_AddCountNode(&GlobalMailCountHead, lParam, 0);
                }
            }
            pTemp = pTemp->pNext;
        }
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MSGSMOVED, NULL, 
            Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
    }
    else if(pCreateData->bConfirm == Con_MoveSelect)
    {
        memset(szTempFile, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        strcpy(szTempFile, szNewFile);
        while(pTemp)
        {
            if(pTemp->GetStatus != UNDOWN)
            {
                memset(szNewFile, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
                strcpy(szNewFile, szTempFile);
                
                MAIL_GetNewFileName(szNewFile, MAX_MAIL_SAVE_FILENAME_LEN + 1);
                MAIL_CopyMail(szNewFile, pTemp->GetMailFileName);
                
                if(pTemp->GetReadFlag == UNREAD)
                {
                    MAIL_AddCountNode(&GlobalMailCountHead, lParam, 1);
                }
                else if(pTemp->GetReadFlag == READ)
                {
                    MAIL_AddCountNode(&GlobalMailCountHead, lParam, 0);
                }
            }
            pTemp = pTemp->pSelNext;
        }
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MSGSMOVED, NULL, 
            Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
    }

    chdir(szOutPath);
    return;
}

static void MailInBox_OnOpenPre(HWND hWnd, LPARAM lParam)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    HWND hList;
    int CurSel;

    pCreateData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_BOXMAIL_LIST);

    CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
    if(CurSel > 0)
    {
        SendMessage(hList, LB_SETCURSEL, CurSel - 1, 0);
        SendMessage(hWnd, WM_COMMAND, IDM_MAIL_BUTTON_OPEN, 0);
    }
    
    return;
}

static void MailInBox_OnOpenNext(HWND hWnd, LPARAM lParam)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    HWND hList;
    int CurSel;
    
    pCreateData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_BOXMAIL_LIST);

    CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
    if(CurSel < (int)pCreateData->GetListHead.len)
    {
        SendMessage(hList, LB_SETCURSEL, CurSel + 1, 0);
        SendMessage(hWnd, WM_COMMAND, IDM_MAIL_BUTTON_OPEN, 0);
    }
    
    return;
}

static void MailInBox_OnDeleteAtt(HWND hWnd)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    HWND hList;
    int CurSel;
    MailGetListNode *pNode;
    
    pCreateData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_BOXMAIL_LIST);
    
    CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
    if(CurSel < (int)pCreateData->GetListHead.len)
    {
        pNode = NULL;
        MAIL_GetGetNodebyInt(&pCreateData->GetListHead, &pNode, CurSel);
        if(pNode != NULL)
        {
            pNode->GetAttFlag = 0;
            MAIL_SetImage(hList, pCreateData, pNode, CurSel);
        }
    }
    
    return;
}

static BOOL MAIL_GetGetList(HWND hWnd, MailGetListHead *pHead, char *pTmp)
{
    PMAIL_INBOXCREATEDATA pCreateData;

    int hFile;
    int hf;
    char szOutPath[PATH_MAXLEN];
    
    MailGetSaveFile TmpGetSaveFile;
    MailFile TmpMailFile;
    MailGetListNode *pTempNode;
    
    if(hWnd != NULL)
    {
        pCreateData = GetUserData(hWnd);
    }

    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    hFile = GARY_open(pTmp, O_RDWR | O_CREAT, S_IRWXU);
    if(hFile < 0)
    {
        printf("\r\nMAIL_GetGetList1 : open error = %d\r\n", errno);

        chdir(szOutPath);
        return FALSE;
    }
    
    memset(&TmpGetSaveFile, 0x0, sizeof(MailGetSaveFile));
    while(read(hFile, &TmpGetSaveFile, sizeof(MailGetSaveFile)) == sizeof(MailGetSaveFile)) 
    {
        if(TmpGetSaveFile.GetLocalFlag)
        {
            pTempNode = NULL;
            pTempNode = (MailGetListNode *)malloc(sizeof(MailGetListNode));
            if(pTempNode == NULL)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
                    IDP_MAIL_BOOTEN_OK, NULL, 20);
                GARY_close(hFile);
                chdir(szOutPath);
                return FALSE;
            }
            memset(pTempNode, 0x0, sizeof(MailGetListNode));
            
            if((TmpGetSaveFile.GetStatus == DOWNSER) || (TmpGetSaveFile.GetStatus == DOWNLOCAL))
            {               
                hf = GARY_open(TmpGetSaveFile.GetMailFileName, O_RDONLY, -1);
                if(hf >= 0)
                {
                    if(read(hf, &TmpMailFile, sizeof(MailFile)) == sizeof(MailFile))
                    {
                        strcpy(pTempNode->GetMailFileName, TmpGetSaveFile.GetMailFileName);
                        pTempNode->GetAttFlag = (TmpMailFile.AttNum) ? 1 : 0;
                    }
                    else
                    {
                        memset(pTempNode->GetMailFileName, 0x0, sizeof(MAX_MAIL_SAVE_FILENAME_LEN) + 1);
                        pTempNode->GetAttFlag = 0;
                    }
                    GARY_close(hf);
                }
                else
                {
                    printf("\r\nMAIL_GetGetList2 : open error = %d\r\n", errno);
                }
            }
            
            pTempNode->GetId = -1;
            strcpy(pTempNode->GetDate, TmpGetSaveFile.GetDate);
            strcpy(pTempNode->GetFrom, TmpGetSaveFile.GetFrom);
            strcpy(pTempNode->GetMsgId, TmpGetSaveFile.GetMsgId);
            pTempNode->GetServerFlag = TmpGetSaveFile.GetServerFlag;
            pTempNode->GetSize = TmpGetSaveFile.GetSize;
            pTempNode->GetStatus = TmpGetSaveFile.GetStatus;
            pTempNode->GetReadFlag = TmpGetSaveFile.GetReadFlag;
            strcpy(pTempNode->GetSubject, TmpGetSaveFile.GetSubject);
            strcpy(pTempNode->GetTo, TmpGetSaveFile.GetTo);
            strcpy(pTempNode->GetUidl, TmpGetSaveFile.GetUidl);
            pTempNode->pNext = NULL;
            pTempNode->pDelNext = NULL;
            pTempNode->pSelNext = NULL;
            
            MAIL_AddGetListNode(pHead, pTempNode, REMOVE_GET);
        }
        else
        {
            pHead->dellocal ++;
        }
        memset(&TmpGetSaveFile, 0x0, sizeof(MailGetSaveFile));
    }
    GARY_close(hFile);
    chdir(szOutPath);
    return TRUE;
}

static void MAIL_AddNodeToList(HWND hList, MAIL_INBOXCREATEDATA *pCreateData)
{
    int curIndex;
    SYSTEMTIME sy;
    SYSTEMTIME syLocal;
    MailGetListNode *pTemp;
    char szTimestamp[100];
    char sztemp[122];
    ABNAMEOREMAIL pABName;

    ShowWindow(hList, SW_SHOW);
    SendMessage(hList, LB_RESETCONTENT, 0, 0);

    if((pCreateData->GetListHead.len == 0) || (pCreateData->GetListHead.pNext == NULL))
    {
        //SendMessage(hList, LB_ADDSTRING, (WPARAM)NULL, (LPARAM)(LPCTSTR)IDP_MAIL_STRING_NOEMAILS);
        //SendMessage(hList, LB_SETCURSEL, 0, 0);
        ShowWindow(hList, SW_HIDE);

        return;
    }

    GetLocalTime(&syLocal);
    
    pTemp = pCreateData->GetListHead.pNext;
    while(pTemp)
    {
        memset(&sy, 0x0, sizeof(SYSTEMTIME));
        MAIL_CharToSystem(pTemp->GetDate, &sy);
        memset(szTimestamp, 0x0, 100);
        memset(sztemp, 0x0, 122);

        if(sy.wYear != syLocal.wYear)
        {
            sprintf(szTimestamp, "%d", sy.wYear);
        }
        else if(sy.wMonth != syLocal.wMonth || sy.wDay != syLocal.wDay)
        {
            sy.wYear = 0;
            GetTimeDisplay(sy, sztemp, szTimestamp);
        }
        else
        {
            GetTimeDisplay(sy, szTimestamp, sztemp);
        }
        sztemp[0] = 0;
        sprintf(sztemp, "%s %s", szTimestamp, pTemp->GetSubject);

        memset(&pABName, 0x0, sizeof(ABNAMEOREMAIL));
        if(APP_GetNameByEmail(pTemp->GetFrom, &pABName))
        {
            curIndex = SendMessage(hList, LB_ADDSTRING, (WPARAM)NULL, (LPARAM)pABName.szName);
        }
        else
        {
            curIndex = SendMessage(hList, LB_ADDSTRING, (WPARAM)NULL, (LPARAM) (LPCTSTR)pTemp->GetFrom);
        }
        
        MAIL_SetImage(hList, pCreateData, pTemp, curIndex);
        SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), (LPARAM)sztemp);

        pTemp = pTemp->pNext;
    }
    if(pCreateData->GetListHead.pNext != NULL)
    {
        SendMessage(hList, LB_SETCURSEL, 0, 0);
    }
    
    return;
}

static void MAIL_SetImage(HWND hList, MAIL_INBOXCREATEDATA *pCreateData, MailGetListNode *pTemp, int curIndex)
{
    if(pTemp->GetServerFlag == DELSERVER)
    {
        SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)curIndex),
            (LPARAM)pCreateData->hDelete);
        return;
    }

    if(pTemp->GetStatus == UNDOWN)
    {
        /*if(pTemp->GetReadFlag == READ)
        {
            SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)curIndex),
                (LPARAM)pCreateData->hUnretrRead);
        }
        else*/
        {
            SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)curIndex),
                (LPARAM)pCreateData->hUnretr);
        }
    }
    else if(pTemp->GetStatus == DOWNSER)
    {
        if(pTemp->GetAttFlag == 1)
        {
            if(pTemp->GetReadFlag == READ)
            {
                SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)curIndex),
                    (LPARAM)pCreateData->hReadAtt);
            }
            else
            {
                SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)curIndex),
                    (LPARAM)pCreateData->hUnreadAtt);
            }
        }
        else
        {
            if(pTemp->GetReadFlag == READ)
            {
                SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)curIndex),
                    (LPARAM)pCreateData->hRead);
            }
            else
            {
                SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)curIndex),
                    (LPARAM)pCreateData->hUnread);
            }
        }
    }
    else if(pTemp->GetStatus == DOWNLOCAL)
    {
        if(pTemp->GetAttFlag == 1)
        {
            if(pTemp->GetReadFlag == READ)
            {
                SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)curIndex),
                    (LPARAM)pCreateData->hReadAtt);
            }
            else
            {
                SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)curIndex),
                    (LPARAM)pCreateData->hUnreadAtt);
            }
        }
        else
        {
            if(pTemp->GetReadFlag == READ)
            {
                SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)curIndex),
                    (LPARAM)pCreateData->hRead);
            }
            else
            {
                SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)curIndex),
                    (LPARAM)pCreateData->hUnread);
            }
        }
    }

    return;
}

void MAIL_ReleaseGetList(MailGetListHead *pHead)
{
    MailGetListNode *pTemp;
    
    pTemp = pHead->pNext;
    //only delete first node 
    //until the list is empty
    while(pTemp)
    {
        pHead->pNext = pTemp->pNext;
        GARY_FREE(pTemp);
        pTemp = pHead->pNext;
    }
    pHead->len = 0;
    pHead->dellocal = 0;
    pHead->delserver = 0;
    pHead->down = 0;
    pHead->downlocal = 0;
    pHead->pNext = NULL;
    
    return;
}

void MAIL_AddGetListNode(MailGetListHead *pHead, MailGetListNode *pTmpNode, int style)
{
    MailGetListNode *pTemp;
    MailGetListNode *pPre;

    switch(style)
    {
    case REMOVE_GET:
        {
            pTmpNode->pNext = NULL;

            if(!pTmpNode->GetServerFlag)
            {
                pHead->delserver ++;
            }
            if(pTmpNode->GetStatus == DOWNSER)
            {
                pHead->down ++;
            }
            if(pTmpNode->GetStatus == DOWNLOCAL)
            {
                pHead->downlocal ++;
            }
            pHead->len ++;
            
            if(pHead->pNext == NULL)
            {
                pHead->pNext = pTmpNode;
                return;
            }
            pTemp = pHead->pNext;
            if((strlen(pTmpNode->GetDate) != 0) && (strlen(pTemp->GetDate) != 0))
            {
                if(MAIL_DateCmpChar(pTemp->GetDate, pTmpNode->GetDate) == Early)
                {
                    pHead->pNext = pTmpNode;
                    pTmpNode->pNext = pTemp;
                    return;
                }
            }
            if(pTemp->pNext == NULL)
            {
                pTemp->pNext = pTmpNode;
                return;
            }
            pPre = pTemp;
            pTemp = pPre->pNext;
            while(pTemp->pNext)
            {
                if((strlen(pTmpNode->GetDate) != 0) && (strlen(pTemp->GetDate) != 0))
                {
                    if(MAIL_DateCmpChar(pTemp->GetDate, pTmpNode->GetDate) == Early)
                    {
                        pPre->pNext = pTmpNode;
                        pTmpNode->pNext = pTemp;
                        return;
                    }
                }
                pPre = pTemp;
                pTemp = pPre->pNext;
            }
            pTemp->pNext = pTmpNode;
        }
    	break;

    case REMOVE_DEL:
        {
            pTmpNode->pDelNext = NULL;

            if(pHead->pNext == NULL)
            {
                pHead->pNext = pTmpNode;
                return;
            }
            pTemp = pHead->pNext;
            while(pTemp->pDelNext)
            {
                pTemp = pTemp->pDelNext;
            }
            pTemp->pDelNext = pTmpNode;
        }
    	break;

    case REMOVE_SEL:
        {
            pTmpNode->pSelNext = NULL;
            
            if(pHead->pNext == NULL)
            {
                pHead->pNext = pTmpNode;
                return;
            }
            pTemp = pHead->pNext;
            while(pTemp->pSelNext)
            {
                pTemp = pTemp->pSelNext;
            }
            pTemp->pSelNext = pTmpNode;
        }
        break;
    }

    return;
}

void MAIL_DelGetListNode(MailGetListHead *pHead, MailGetListNode *pTmpNode)
{
    MailGetListNode *pTmp;
    MailGetListNode *pPre;
    
    if(pTmpNode == pHead->pNext)
    {
        pHead->pNext = pTmpNode->pNext;
        if(pTmpNode->GetServerFlag == DELSERVER)
        {
            pHead->delserver --;
        }
        pHead->len --;
        GARY_FREE(pTmpNode);
        pTmpNode = NULL;
        return;
    }
    pPre = pHead->pNext;
    pTmp = pPre->pNext;
    while (pTmp)
    {
        if(pTmp == pTmpNode)
        {
            pPre->pNext = pTmp->pNext;
            if(pTmpNode->GetServerFlag == DELSERVER)
            {
                pHead->delserver --;
            }
            pHead->len --;
            GARY_FREE(pTmpNode);
            pTmpNode = NULL;
            return;
        }
        pPre = pTmp;
        pTmp = pPre->pNext;
    }
    return;
}

void MAIL_RemoveGetListNode(MailGetListHead *pHead, MailGetListNode *pTmpNode, int style)
{
    MailGetListNode *pTmp;
    MailGetListNode *pPre;
    
    switch(style)
    {
    case REMOVE_SEL:
        {
            if(pTmpNode == pHead->pNext)
            {
                pHead->pNext = pTmpNode->pSelNext;
                pTmpNode->pSelNext = NULL;
                break;
            }

            pPre = pHead->pNext;
            pTmp = pPre->pSelNext;
            while (pTmp)
            {
                if(pTmp == pTmpNode)
                {
                    pPre->pSelNext = pTmpNode->pSelNext;
                    pTmpNode->pSelNext = NULL;
                    break;
                }
                pPre = pTmp;
                pTmp = pPre->pSelNext;
            }
        }
        break;

    case REMOVE_DEL:
        {
            if(pTmpNode == pHead->pNext)
            {
                pHead->pNext = pTmpNode->pDelNext;
                pTmpNode->pDelNext = NULL;
                break;
            }

            pPre = pHead->pNext;
            pTmp = pPre->pDelNext;
            while (pTmp)
            {
                if(pTmp == pTmpNode)
                {
                    pPre->pDelNext = pTmpNode->pDelNext;
                    pTmpNode->pDelNext = NULL;
                    break;
                }
                pPre = pTmp;
                pTmp = pPre->pDelNext;
            }
        }
        break;
        
    case REMOVE_GET:
        {
            if(pTmpNode == pHead->pNext)
            {
                pHead->pNext = pTmpNode->pNext;
                pTmpNode->pNext = NULL;
                break;
            }
            
            pPre = pHead->pNext;
            pTmp = pPre->pNext;
            while (pTmp)
            {
                if(pTmp == pTmpNode)
                {
                    pPre->pNext = pTmpNode->pNext;
                    pTmpNode->pNext = NULL;
                    break;
                }
                pPre = pTmp;
                pTmp = pPre->pNext;
            }
        }
        break;
    }

    return;
}

int MAIL_GetGetNodeInt(MailGetListHead *pHead, MailGetListNode *pNode)
{
    int i;
    MailGetListNode *pTemp;

    if(pHead->pNext == NULL)
        return -1;
    
    i = 0;
    pTemp = pHead->pNext;
    while(pTemp)
    {
        if(pTemp == pNode)
        {
            return i;
        }
        i ++;
        pTemp = pTemp->pNext;
    }
    
    return -1;
}

BOOL MAIL_GetGetNodebyInt(MailGetListHead *pHead, MailGetListNode **pListNode, int iTmp)
{
    int i;
    MailGetListNode *pTemp;
    
    if(iTmp > (int)(pHead->len - 1))
        return FALSE;
    
    if(pHead->pNext == NULL)
        return FALSE;
    
    pTemp = pHead->pNext;
    for(i=0; i<iTmp; i++)
    {
        pTemp = pTemp->pNext;
    }
    //memcpy(pListNode, pTemp, sizeof(MailGetListNode));
    *pListNode = pTemp;
    
    return TRUE;
}

BOOL MAIL_GetGetNodebyHandle(MailGetListHead *pHead, MailGetListNode **pListNode, char *msghandle)
{
    MailGetListNode *pTemp;
    
    pTemp = pHead->pNext;
    while(pTemp)
    {
        if(strcmp(pTemp->GetMsgId, msghandle) == 0)
        {
            *pListNode = pTemp;
            return TRUE;
        }
        pTemp = pTemp->pNext;
    }
    
    return FALSE;
}

BOOL MAIL_GetGetNodebyID(MailGetListHead *pHead, MailGetListNode **pListNode, int msg)
{
    MailGetListNode *pTemp;
    
    pTemp = pHead->pNext;
    while(pTemp)
    {
        if((int)pTemp->GetId == msg)
        {
            *pListNode = pTemp;
            return TRUE;
        }
        pTemp = pTemp->pNext;
    }
    
    return FALSE;
}

BOOL MAIL_ConnectServer(HWND hWnd, BYTE bConType)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    BOOL bReturn;

    MailSendNode *pMailSendNode;

    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
    {
        if(hWnd == NULL)
        {
            hWnd = HwndMailInBox;
        }
        pCreateData = GetUserData(hWnd);
    }
    
    if(bConType == MAIL_GET_CONNECT)
    {
        if(GetSIMState() == 0)
        {
            if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSIMCON, NULL, Notify_Failure, 
                    IDP_MAIL_BOOTEN_OK, NULL, 20);
            }
            MAIL_DealNextConnect();
            
            return FALSE;
        }
        
        if(MAIL_IsFlashEnough() == 0)
        {
            if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DRAFTFULL, NULL, Notify_Failure, 
                    IDP_MAIL_BOOTEN_OK, NULL, 20);
            }
            
            MAIL_DealNextConnect();
            
            return FALSE;
        }

        if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
        {
            //send list
            pMailSendNode = mMailSendHead.pNext;
            while(pMailSendNode)
            {
                if(pMailSendNode->SendStatus == MU_STU_DEFERMENT)
                {
                    pMailSendNode->SendStatus = MU_STU_WAITINGSEND;
                }
                pMailSendNode = pMailSendNode->pNext;
            }
            
            pMailSendNode = mMailSendHead.pNext;
            while(pMailSendNode)
            {
                if(pMailSendNode->SendStatus == MU_STU_WAITINGSEND)
                {
                    //MAIL_GetToSendDial = TRUE;
                    
                    MAIL_AddDialWaitNode(&GlobalMailConfigHead, 
                        GlobalMailConfigHead.CurConnectConfig, MAIL_CON_TYPE_SHOWREC);
                    
                    GlobalMailConfigHead.GLNetUsed = TRUE;
                    GlobalMailConfigHead.GLNetFlag = MAIL_CON_TYPE_SEND;
                    //GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_SEND;
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

                    SendMessage(HwndMailSend, WM_MAIL_BEGIN_SEND, 0, 0);

                    WaitWin(HwndMailGet, TRUE, IDP_MAIL_STRING_CONNECTING, NULL, NULL, IDP_MAIL_BUTTON_CANCEL, WM_MAIL_CANCEL_RETURN);
                    return TRUE;
                }
                pMailSendNode = pMailSendNode->pNext;
            }
            
            pCreateData->bConnect = TRUE;
            pCreateData->bReadAlreadyDel = FALSE;

            WaitWin(HwndMailGet, TRUE, IDP_MAIL_STRING_CONNECTING, NULL, NULL, IDP_MAIL_BUTTON_CANCEL, WM_MAIL_CANCEL_RETURN);
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
            return FALSE;
        }
        
        return TRUE;
    }
    else if(bConType == MAIL_GET_DISCONNECT)
    {
        if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
        {
            pCreateData->bConnect = FALSE;
            WaitWindowStateEx(HwndMailGet, TRUE, IDP_MAIL_STRING_DISCONNECTING, NULL, NULL, NULL);
        }
        
        if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
        {
            if(pCreateData->DelListHead.pNext != NULL)
            {
                MAIL_DelHeaderFromServer(hWnd);
                return TRUE;
            }
        }
        if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE) && 
            GlobalMailConfigHead.GLInboxHandle && 
            (MAIL_QuitInboxSession(GlobalMailConfigHead.GLInboxHandle) == MAIL_BLOCKED))
        {
            printf("\r\nGARY_MAIL:MAIL_QuitInboxSession\r\n");
        }
        else
        {
            WaitWindowStateEx(HwndMailGet, FALSE, NULL, NULL, NULL, NULL);
            SendMessage(HwndMailGet, WM_MAIL_MSG, 0, MAKELPARAM(MSG_MAIL_QUIT, 0));
        }
    }
    return TRUE;
}

BOOL MAIL_GetHeaderFromServer(HWND hWnd, int start, int num)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    MailGetListNode *pMailNodeTmp;

    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
    {
        if(hWnd == NULL)
        {
            hWnd = HwndMailInBox;
        }
        pCreateData = GetUserData(hWnd);
    }
    
    if(strlen(GlobalMailConfigHead.CurConnectConfig->GLInServer) == 0)
    {
        MAIL_DealNextConnect();
        return FALSE;
    }
    if(GlobalMailConfigHead.GLConStatus == MAILBOX_CON_ONLINE)
    {
        if(GlobalMailConfigHead.GLInboxHandle == 0)
        {
            ACCOUNT Account;
            char* pMailContent;

            Account.username = GlobalMailConfigHead.CurConnectConfig->GLLogin;
            Account.password = GlobalMailConfigHead.CurConnectConfig->GLPassword;
            Account.svr_ip = GlobalMailConfigHead.CurConnectConfig->GLInServer;

            pMailContent = NULL;
            pMailContent = Mail_MultiDecodeOnString(GlobalMailConfigHead.CurConnectConfig->GLMailAddr, 
                strlen(GlobalMailConfigHead.CurConnectConfig->GLMailAddr));
            if(!pMailContent)
            {
                MAIL_DealNextConnect();
                return FALSE;
            }
            Account.retaddr = pMailContent;
            
            if(GlobalMailConfigHead.CurConnectConfig->GLBoxType == MAIL_TYPE_POP3)
            {
                if(GlobalMailConfigHead.CurConnectConfig->GLSecurity == 1)
                {
                    Account.svr_port = 995;
                    GlobalMailConfigHead.GLInboxHandle = MAIL_CreateInbox(HwndMailGet, WM_MAIL_MSG, &Account, "POP3s");
                }
                else
                {
                    Account.svr_port = 0;
                    GlobalMailConfigHead.GLInboxHandle = MAIL_CreateInbox(HwndMailGet, WM_MAIL_MSG, &Account, "POP3");
                }
            }
            else if(GlobalMailConfigHead.CurConnectConfig->GLBoxType == MAIL_TYPE_IMAP4)
            {
                if(GlobalMailConfigHead.CurConnectConfig->GLSecurity == 1)
                {
                    Account.svr_port = 993;
                    GlobalMailConfigHead.GLInboxHandle = MAIL_CreateInbox(HwndMailGet, WM_MAIL_MSG, &Account, "IMAP4s");
                }
                else
                {
                    Account.svr_port = 0;
                    GlobalMailConfigHead.GLInboxHandle = MAIL_CreateInbox(HwndMailGet, WM_MAIL_MSG, &Account, "IMAP4");
                }
            }
            
            if(GlobalMailConfigHead.GLInboxHandle == 0)
            {
                MAIL_DealNextConnect();
                return FALSE;
            }
        }

        //transfer protocol interface and import save list
        if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
        {
            pMailNodeTmp = pCreateData->GetListHead.pNext;
        }
        else
        {
            MAIL_ReleaseGetList(&CurHideGetListHead);
            memset(&CurHideGetListHead, 0x0, sizeof(MailGetListHead));
            MAIL_GetGetList(NULL, &CurHideGetListHead, GlobalMailConfigHead.CurConnectConfig->GLMailConfigSaveName);
            pMailNodeTmp = CurHideGetListHead.pNext;
        }
        while(pMailNodeTmp)
        {
            MAIL_ImportMailList(GlobalMailConfigHead.GLInboxHandle, pMailNodeTmp->GetUidl);
            pMailNodeTmp = pMailNodeTmp->pNext;
        }
        
        if(MAIL_CheckMailList(GlobalMailConfigHead.GLInboxHandle, start, num) != MAIL_BLOCKED)
        {
            SendMessage(HwndMailGet, WM_MAIL_MSG, 0, MAKELPARAM(MSG_MAIL_QUIT, 0));
            return FALSE;
        }
    }
    else
    {
        SendMessage(HwndMailDial, WM_MAIL_DIAL_AGAIN, (WPARAM)HwndMailGet, 1);
    }
    
    return TRUE;
}

void MAIL_GetServerHead()
{
    int i;
    int num;
    int hf;
    char szOutPath[PATH_MAXLEN];

    MAILLIST tmpList;
    MailGetListNode *pTmpNode;
    MailGetListNode *pNewNodeTmp;
    MailGetListHead GetHeadNew;

    HWND hWnd;

    PMAIL_INBOXCREATEDATA pCreateData;

    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
    {
        hWnd = HwndMailInBox;
        if(hWnd == NULL)
        {
            GlobalMailConfigHead.GLNetFlag = MAIL_CON_TYPE_HIDEREC;
            
            if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE) && 
                GlobalMailConfigHead.GLInboxHandle && 
                (MAIL_QuitInboxSession(GlobalMailConfigHead.GLInboxHandle) == MAIL_BLOCKED))
            {
                printf("\r\nGARY_MAIL:MAIL_QuitInboxSession\r\n");
            }
            return;
        }
        pCreateData = GetUserData(hWnd);
    }

    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    memset(&GetHeadNew, 0x0, sizeof(MailGetListHead));
    GetHeadNew.pNext = NULL;
    num = MAIL_GetMailCount(GlobalMailConfigHead.GLInboxHandle);
    if(num == -1)
    {
        MAIL_ConnectServer(NULL, MAIL_GET_DISCONNECT);

        if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
        {
            WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
            
            pCreateData->ReadNode = NULL;
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTFAL, NULL, Notify_Success, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
        }
        chdir(szOutPath);
        return;
    }

    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
    {
        pCreateData->bConStatus = TRUE;
    }
    
    if(num == 0)
    {
        if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
        {
            memset(&pCreateData->DelListHead, 0x0, sizeof(MailGetListHead));
            pCreateData->DelListHead.pNext = NULL;
            pTmpNode = pCreateData->GetListHead.pNext;
        }
        else
        {
            pTmpNode = CurHideGetListHead.pNext;
        }
        while(pTmpNode)
        {
            if((pTmpNode->GetServerFlag != UNDOWN) && (strlen(pTmpNode->GetMailFileName) != 0))
            {
                hf = GARY_open(pTmpNode->GetMailFileName, O_RDONLY, -1);
                if(hf >= 0)
                {
                    GARY_close(hf);
                    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                    {
                        pCreateData->GetListHead.pNext = pTmpNode->pNext;
                    }
                    else
                    {
                        CurHideGetListHead.pNext = pTmpNode->pNext;
                    }
                    pTmpNode->GetStatus = DOWNLOCAL;
                    pTmpNode->GetId = -1;
                    pTmpNode->pNext = NULL;
                    MAIL_AddGetListNode(&GetHeadNew, pTmpNode, REMOVE_GET);
                }
                else
                {
                    printf("\r\nMAIL_GetServerHead1 : open error = %d\r\n", errno);

                    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                    {
                        if(pTmpNode == pCreateData->ReadNode)
                        {
                            pCreateData->bReadAlreadyDel = TRUE;
                        }
                        if(pTmpNode->bSelect == MAIL_DEF_SELECTED)
                        {
                            MAIL_RemoveGetListNode(&pCreateData->SelListHead, pTmpNode, REMOVE_SEL);
                        }
                        MAIL_DelGetListNode(&pCreateData->GetListHead, pTmpNode);
                    }
                    else
                    {
                        MAIL_DelGetListNode(&CurHideGetListHead, pTmpNode);
                    }
                }
            }
            else
            {
                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    if(pTmpNode == pCreateData->ReadNode)
                    {
                        pCreateData->bReadAlreadyDel = TRUE;
                    }
                    if(pTmpNode->bSelect == MAIL_DEF_SELECTED)
                    {
                        MAIL_RemoveGetListNode(&pCreateData->SelListHead, pTmpNode, REMOVE_SEL);
                    }
                    MAIL_DelGetListNode(&pCreateData->GetListHead, pTmpNode);
                }
                else
                {
                    MAIL_DelGetListNode(&CurHideGetListHead, pTmpNode);
                }
            }
            if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
            {
                pTmpNode = pCreateData->GetListHead.pNext;
            }
            else
            {
                pTmpNode = CurHideGetListHead.pNext;
            }
        }

        remove(MAIL_FILE_GET_TMPSAVE);
        hf = GARY_open(MAIL_FILE_GET_TMPSAVE, O_RDWR | O_CREAT, S_IRWXU);
        if(hf < 0)
        {
            printf("\r\nMAIL_GetServerHead2 : open error = %d\r\n", errno);

            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
        }
        else
        {
            MailGetSaveFile mMailGetSaveFile;

            pTmpNode = GetHeadNew.pNext;
            while(pTmpNode)
            {
                memset(&mMailGetSaveFile, 0x0, sizeof(MailGetSaveFile));

                mMailGetSaveFile.GetLocalFlag = UNDELLOCAL;
                mMailGetSaveFile.GetServerFlag = pTmpNode->GetServerFlag;
                mMailGetSaveFile.GetSize = pTmpNode->GetSize;
                mMailGetSaveFile.GetStatus = pTmpNode->GetStatus;
                mMailGetSaveFile.GetReadFlag = pTmpNode->GetReadFlag;
                sprintf(mMailGetSaveFile.GetDate, "%s", pTmpNode->GetDate);
                sprintf(mMailGetSaveFile.GetFrom, "%s", pTmpNode->GetFrom);
                sprintf(mMailGetSaveFile.GetMailFileName, "%s", pTmpNode->GetMailFileName);
                sprintf(mMailGetSaveFile.GetMsgId, "%s", pTmpNode->GetMsgId);
                sprintf(mMailGetSaveFile.GetSubject, "%s", pTmpNode->GetSubject);
                sprintf(mMailGetSaveFile.GetTo, "%s", pTmpNode->GetTo);
                sprintf(mMailGetSaveFile.GetUidl, "%s", pTmpNode->GetUidl);
                
                write(hf, &mMailGetSaveFile, sizeof(MailGetSaveFile));
                pTmpNode = pTmpNode->pNext;
            }
            GARY_close(hf);
            remove(GlobalMailConfigHead.CurConnectConfig->GLMailConfigSaveName);
            rename(MAIL_FILE_GET_TMPSAVE, GlobalMailConfigHead.CurConnectConfig->GLMailConfigSaveName);
        }
        
        if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
        {
            if(pCreateData->GetListHead.pNext != NULL)
            {
                MAIL_ReleaseGetList(&pCreateData->GetListHead);
            }
            pCreateData->GetListHead.len = GetHeadNew.len;
            pCreateData->GetListHead.dellocal = 0;
            pCreateData->GetListHead.delserver = GetHeadNew.delserver;
            pCreateData->GetListHead.down = GetHeadNew.down;
            pCreateData->GetListHead.downlocal = GetHeadNew.downlocal;
            pCreateData->GetListHead.pNext = GetHeadNew.pNext;
        
            GetHeadNew.dellocal = 0;
            GetHeadNew.delserver = 0;
            GetHeadNew.down = 0;
            GetHeadNew.downlocal = 0;
            GetHeadNew.len = 0;
            GetHeadNew.pNext = NULL;

            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_MAIL_LIST_REFRESH, 0), 0);
            WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);

            if(pCreateData->ReadNode != NULL)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MAILALDEL, NULL, Notify_Failure, 
                    IDP_MAIL_BOOTEN_OK, NULL, 20);
            }
            else
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTED, NULL, Notify_Success, 
                    IDP_MAIL_BOOTEN_OK, NULL, 20);
            }
        }
        else
        {
            MAIL_ReleaseGetList(&CurHideGetListHead);
            MAIL_ReleaseGetList(&GetHeadNew);

            MAIL_InstallMailBoxCount(GlobalMailConfigHead.GLMailBoxName);

            MAIL_ConnectServer(NULL, MAIL_GET_DISCONNECT);
        }

        chdir(szOutPath);
        return;
    }
    
    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
    {
        memset(&pCreateData->DelListHead, 0x0, sizeof(MailGetListHead));
        pCreateData->DelListHead.pNext = NULL;
        memset(&pCreateData->SelListHead, 0x0, sizeof(MailGetListHead));
        pCreateData->SelListHead.pNext = NULL;
    }
    else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
    {
        memset(&CurHideRetrListHead, 0x0, sizeof(MailGetListHead));
        CurHideRetrListHead.pNext = NULL;
    }
    for(i=num-1; i>=0; i--)
    {
        memset(&tmpList, 0x0, sizeof(MAILLIST));
        if(MAIL_GetMailListItem(GlobalMailConfigHead.GLInboxHandle, i, &tmpList) == MAIL_FAILURE)
        {
            continue;
        }
        
        pNewNodeTmp = NULL;
        pNewNodeTmp = (MailGetListNode *)malloc(sizeof(MailGetListNode));
        if(pNewNodeTmp == NULL)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
            break;
        }
        memset(pNewNodeTmp, 0x0, sizeof(MailGetListNode));
        pNewNodeTmp->pNext = NULL;
        pNewNodeTmp->pSelNext = NULL;
        pNewNodeTmp->pDelNext = NULL;
        pNewNodeTmp->GetId = tmpList.id;
        strcpy(pNewNodeTmp->GetUidl, tmpList.uidl);
        
        //synchronization protocol and application
        if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
        {
            pTmpNode = pCreateData->GetListHead.pNext;
        }
        else
        {
            pTmpNode = CurHideGetListHead.pNext;
        }
        while(pTmpNode)
        {
            if(strcmp(pTmpNode->GetUidl, tmpList.uidl) == 0)
            {
                strcpy(pNewNodeTmp->GetFrom, pTmpNode->GetFrom);
                strcpy(pNewNodeTmp->GetTo, pTmpNode->GetTo);
                strcpy(pNewNodeTmp->GetDate, pTmpNode->GetDate);
                strcpy(pNewNodeTmp->GetSubject, pTmpNode->GetSubject);
                strcpy(pNewNodeTmp->GetMsgId, pTmpNode->GetMsgId);
                strcpy(pNewNodeTmp->GetMailFileName, pTmpNode->GetMailFileName);

                pNewNodeTmp->GetSize = pTmpNode->GetSize;
                pNewNodeTmp->GetStatus = pTmpNode->GetStatus;
                pNewNodeTmp->GetAttFlag = pTmpNode->GetAttFlag;
                pNewNodeTmp->GetServerFlag = pTmpNode->GetServerFlag;
                pNewNodeTmp->GetReadFlag = pTmpNode->GetReadFlag;
                pNewNodeTmp->bSelect = pTmpNode->bSelect;

                if(pNewNodeTmp->GetStatus == DOWNLOCAL)
                {
                    pNewNodeTmp->GetStatus = DOWNSER;
                }
                
                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    if(pNewNodeTmp->GetServerFlag == DELSERVER)
                    {
                        MAIL_AddGetListNode(&pCreateData->DelListHead, pNewNodeTmp, REMOVE_DEL);
                    }
                    if(pNewNodeTmp->bSelect == MAIL_DEF_SELECTED)
                    {
                        MAIL_AddGetListNode(&pCreateData->SelListHead, pNewNodeTmp, REMOVE_SEL);
                    }
                    if(pTmpNode == pCreateData->ReadNode)
                    {
                        pCreateData->ReadNode = NULL;
                        pCreateData->ReadNode = pNewNodeTmp;
                    }
                }

                MAIL_AddGetListNode(&GetHeadNew, pNewNodeTmp, REMOVE_GET);
                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    MAIL_DelGetListNode(&pCreateData->GetListHead, pTmpNode);
                }
                else
                {
                    MAIL_DelGetListNode(&CurHideGetListHead, pTmpNode);
                }
                
                break;
            }
            pTmpNode = pTmpNode->pNext;
        }
        if(pTmpNode == NULL)
        {
            char *pMailContent;
            char *chTrun;
            int chLen;
            
            chLen = strlen(tmpList.from);
            chTrun = malloc(chLen + 1);
            if(!chTrun)
            {
                chdir(szOutPath);
                break;
            }
            memset(chTrun, 0x0, chLen + 1);
            MAIL_TruncateRec(chTrun, tmpList.from);
            
            pMailContent = Mail_UTF8DecodeOnString(chTrun, strlen(chTrun));
            GARY_FREE(chTrun);
            
            if(!pMailContent)
            {
                break;
            }
            strcpy(pNewNodeTmp->GetFrom, pMailContent);
            GARY_FREE(pMailContent);
            
            pMailContent = Mail_UTF8DecodeOnString(tmpList.to, strlen(tmpList.to));
            if(!pMailContent)
            {
                break;
            }
            strcpy(pNewNodeTmp->GetTo, pMailContent);
            GARY_FREE(pMailContent);
            
            strcpy(pNewNodeTmp->GetDate, tmpList.date);
            
            pMailContent = Mail_UTF8DecodeOnString(tmpList.subject, strlen(tmpList.subject));
            if(!pMailContent)
            {
                break;
            }
            strcpy(pNewNodeTmp->GetSubject, pMailContent);
            GARY_FREE(pMailContent);
            
            sprintf(pNewNodeTmp->GetMsgId, "%s%s", GlobalMailConfigHead.GLMailBoxName, tmpList.uidl);
            
            pNewNodeTmp->GetSize = tmpList.size;
            pNewNodeTmp->GetStatus = UNDOWN;
            pNewNodeTmp->GetAttFlag = 0;
            pNewNodeTmp->GetServerFlag = UNDELSERVER;
            pNewNodeTmp->GetReadFlag = UNREAD;
            pNewNodeTmp->bSelect = MAIL_DEF_UNSELECTED;

            MAIL_AddGetListNode(&GetHeadNew, pNewNodeTmp, REMOVE_GET);
            if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
            {
                MAIL_AddGetListNode(&CurHideRetrListHead, pNewNodeTmp, REMOVE_SEL);
            }
        }
    }//end (i=0; i<num; i++)

    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
    {
        pTmpNode = pCreateData->GetListHead.pNext;
    }
    else
    {
        pTmpNode = CurHideGetListHead.pNext;
    }
    while(pTmpNode)
    {
        if(pTmpNode->GetServerFlag != UNDOWN)
        {
            hf = GARY_open(pTmpNode->GetMailFileName, O_RDONLY, -1);
            if(hf >= 0)
            {
                GARY_close(hf);
                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    pCreateData->GetListHead.pNext = pTmpNode->pNext;
                }
                else
                {
                    CurHideGetListHead.pNext = pTmpNode->pNext;
                }
                pTmpNode->GetStatus = DOWNLOCAL;
                pTmpNode->GetId = -1;
                pTmpNode->pNext = NULL;
                MAIL_AddGetListNode(&GetHeadNew, pTmpNode, REMOVE_GET);

                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    pTmpNode = pCreateData->GetListHead.pNext;
                }
                else
                {
                    pTmpNode = CurHideGetListHead.pNext;
                }

                continue;
            }
            else
            {
                printf("\r\nMAIL_GetServerHead3 : open error = %d\r\n", errno);
            }
        }

        if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
        {
            if(pTmpNode == pCreateData->ReadNode)
            {
                pCreateData->bReadAlreadyDel = TRUE;
            }
            if(pTmpNode->bSelect == MAIL_DEF_SELECTED)
            {
                MAIL_RemoveGetListNode(&pCreateData->SelListHead, pTmpNode, REMOVE_SEL);
            }
            MAIL_DelGetListNode(&pCreateData->GetListHead, pTmpNode);
            pTmpNode = pCreateData->GetListHead.pNext;
        }
        else
        {
            MAIL_DelGetListNode(&CurHideGetListHead, pTmpNode);
            pTmpNode = CurHideGetListHead.pNext;
        }
    }

    remove(MAIL_FILE_GET_TMPSAVE);
    hf = GARY_open(MAIL_FILE_GET_TMPSAVE, O_RDWR | O_CREAT, S_IRWXU);
    if(hf < 0)
    {
        printf("\r\nMAIL_GetServerHead4 : open error = %d\r\n", errno);

        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
    }
    else
    {
        MailGetSaveFile mMailGetSaveFile;

        pTmpNode = GetHeadNew.pNext;
        while(pTmpNode)
        {
            memset(&mMailGetSaveFile, 0x0, sizeof(MailGetSaveFile));

            mMailGetSaveFile.GetLocalFlag = UNDELLOCAL;
            mMailGetSaveFile.GetServerFlag = pTmpNode->GetServerFlag;
            mMailGetSaveFile.GetSize = pTmpNode->GetSize;
            mMailGetSaveFile.GetStatus = pTmpNode->GetStatus;
            mMailGetSaveFile.GetReadFlag = pTmpNode->GetReadFlag;
            sprintf(mMailGetSaveFile.GetDate, "%s", pTmpNode->GetDate);
            sprintf(mMailGetSaveFile.GetFrom, "%s", pTmpNode->GetFrom);
            sprintf(mMailGetSaveFile.GetMailFileName, "%s", pTmpNode->GetMailFileName);
            sprintf(mMailGetSaveFile.GetMsgId, "%s", pTmpNode->GetMsgId);
            sprintf(mMailGetSaveFile.GetSubject, "%s", pTmpNode->GetSubject);
            sprintf(mMailGetSaveFile.GetTo, "%s", pTmpNode->GetTo);
            sprintf(mMailGetSaveFile.GetUidl, "%s", pTmpNode->GetUidl);
            
            write(hf, &mMailGetSaveFile, sizeof(MailGetSaveFile));
            pTmpNode = pTmpNode->pNext;
        }
        GARY_close(hf);
        remove(GlobalMailConfigHead.CurConnectConfig->GLMailConfigSaveName);
        rename(MAIL_FILE_GET_TMPSAVE, GlobalMailConfigHead.CurConnectConfig->GLMailConfigSaveName);
    }
 
    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
    {
        MAIL_ReleaseGetList(&CurHideGetListHead);
        
        CurHideGetListHead.len = GetHeadNew.len;
        CurHideGetListHead.dellocal = 0;
        CurHideGetListHead.delserver = 0;
        CurHideGetListHead.down = 0;
        CurHideGetListHead.downlocal = 0;
        CurHideGetListHead.pNext = GetHeadNew.pNext;
        
        GetHeadNew.dellocal = 0;
        GetHeadNew.delserver = 0;
        GetHeadNew.down = 0;
        GetHeadNew.downlocal = 0;
        GetHeadNew.len = 0;
        GetHeadNew.pNext = NULL;

        chdir(szOutPath);

        if(CurHideRetrListHead.pNext != NULL)
        {
            MAIL_RetriHeaderFromServer(NULL);
        }
        else
        {
            MAIL_ReleaseGetList(&CurHideGetListHead);
            MAIL_InstallMailBoxCount(GlobalMailConfigHead.GLMailBoxName);
            MAIL_ConnectServer(NULL, MAIL_GET_DISCONNECT);
        }
        
        return;
    }

    if(pCreateData->GetListHead.pNext != NULL)
    {
        MAIL_ReleaseGetList(&pCreateData->GetListHead);
    }
    pCreateData->GetListHead.len = GetHeadNew.len;
    pCreateData->GetListHead.dellocal = 0;
    pCreateData->GetListHead.delserver = GetHeadNew.delserver;
    pCreateData->GetListHead.down = GetHeadNew.down;
    pCreateData->GetListHead.downlocal = GetHeadNew.downlocal;
    pCreateData->GetListHead.pNext = GetHeadNew.pNext;
    
    GetHeadNew.dellocal = 0;
    GetHeadNew.delserver = 0;
    GetHeadNew.down = 0;
    GetHeadNew.downlocal = 0;
    GetHeadNew.len = 0;
    GetHeadNew.pNext = NULL;
    
    if(pCreateData->DelListHead.pNext != NULL)
    {
        MAIL_DelHeaderFromServer(hWnd);
    }
    else if(pCreateData->ReadNode != NULL)
    {
        MAIL_RetriHeaderFromServer(hWnd);
    }
    else
    {
        MAIL_GetSaveAllFile(hWnd);

        SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_MAIL_LIST_REFRESH, 0), 0);
        WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
        
        if(pCreateData->bReadAlreadyDel)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MAILALDEL, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
        }
        else
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTED, NULL, Notify_Success, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
        }
    }
    
    chdir(szOutPath);
    return;
}

static BOOL MAIL_DelHeaderFromServer(HWND hWnd)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    
    MailGetListNode *pDelNode;
    int iState;
    
    pCreateData = GetUserData(hWnd);
    
    pDelNode = pCreateData->DelListHead.pNext;
    if(pDelNode != NULL)
    {
        iState = MAIL_DeleteMail(GlobalMailConfigHead.GLInboxHandle, pDelNode->GetId);
        if(iState == MAIL_BLOCKED)
        {
        }
        else if(iState == MAIL_FAILURE)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELFAIL, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
            pDelNode->GetServerFlag = UNDELSERVER;
            
            MAIL_GetSaveAllFile(hWnd);

            if(!pCreateData->bConnect)
            {
                if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
                    && GlobalMailConfigHead.GLInboxHandle 
                    && (MAIL_QuitInboxSession(GlobalMailConfigHead.GLInboxHandle) == MAIL_BLOCKED))
                {
                    printf("\r\nGARY_MAIL:MAIL_QuitInboxSession\r\n");
                    WaitWindowStateEx(HwndMailGet, TRUE, IDP_MAIL_STRING_DISCONNECTING, NULL, NULL, NULL);
                }
            }
            else
            {
                if(pCreateData->bReadAlreadyDel)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MAILALDEL, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTED, NULL, Notify_Success, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
            }
        }
        else if(iState == MAIL_SUCCESS)
        {
            MAIL_DelServerHead();
        }
    }
    else
    {
        if(pCreateData->ReadNode != NULL)
        {
            MAIL_RetriHeaderFromServer(hWnd);
        }
        else
        {
            MAIL_GetSaveAllFile(hWnd);
            
            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_MAIL_LIST_REFRESH, 0), 0);
            WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);

            if(!pCreateData->bConnect)
            {
                if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
                    && GlobalMailConfigHead.GLInboxHandle 
                    && (MAIL_QuitInboxSession(GlobalMailConfigHead.GLInboxHandle) == MAIL_BLOCKED))
                {
                    printf("\r\nGARY_MAIL:MAIL_QuitInboxSession\r\n");
                    WaitWindowStateEx(HwndMailGet, TRUE, IDP_MAIL_STRING_DISCONNECTING, NULL, NULL, NULL);
                }
            }
            else
            {
                if(pCreateData->bReadAlreadyDel)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MAILALDEL, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTED, NULL, Notify_Success, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
            }
        }
    }

    return TRUE;
}

void MAIL_DelServerHead()
{
    PMAIL_INBOXCREATEDATA pCreateData;
    HWND hWnd;
    MailGetListNode *pDelNode;

    hWnd = HwndMailInBox;
    pCreateData = GetUserData(hWnd);

    pDelNode = pCreateData->DelListHead.pNext;
    if(pDelNode == NULL)
    {
        if(pCreateData->ReadNode != NULL)
        {
            MAIL_RetriHeaderFromServer(hWnd);
        }
        else
        {
            MAIL_GetSaveAllFile(hWnd);
            
            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_MAIL_LIST_REFRESH, 0), 0);
            WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);

            if(!pCreateData->bConnect)
            {
                if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
                    && GlobalMailConfigHead.GLInboxHandle 
                    && (MAIL_QuitInboxSession(GlobalMailConfigHead.GLInboxHandle) == MAIL_BLOCKED))
                {
                    printf("\r\nGARY_MAIL:MAIL_QuitInboxSession\r\n");
                    WaitWindowStateEx(HwndMailGet, TRUE, IDP_MAIL_STRING_DISCONNECTING, NULL, NULL, NULL);
                }
            }
            else
            {
                if(pCreateData->bReadAlreadyDel)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MAILALDEL, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTED, NULL, Notify_Success, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
            }
        }
        return;
    }

    MAIL_RemoveGetListNode(&pCreateData->DelListHead, pDelNode, REMOVE_DEL);
    if(pDelNode->GetStatus == DOWNSER)
    {
        pDelNode->GetStatus = DOWNLOCAL;
    }
    else if(pDelNode->GetStatus == UNDOWN)
    {
        if(pCreateData->ReadNode == pDelNode)
        {
            pCreateData->ReadNode = NULL;
            pCreateData->bReadAlreadyDel = TRUE;
        }
        MAIL_DelGetListNode(&pCreateData->GetListHead, pDelNode);
    }
    
    pDelNode = pCreateData->DelListHead.pNext;
    if(pDelNode == NULL)
    {
        if(pCreateData->ReadNode != NULL)
        {
            MAIL_RetriHeaderFromServer(hWnd);
        }
        else
        {
            MAIL_GetSaveAllFile(hWnd);

            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_MAIL_LIST_REFRESH, 0), 0);
            WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);

            if(!pCreateData->bConnect)
            {
                if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
                    && GlobalMailConfigHead.GLInboxHandle && 
                    (MAIL_QuitInboxSession(GlobalMailConfigHead.GLInboxHandle) == MAIL_BLOCKED))
                {
                    printf("\r\nGARY_MAIL:MAIL_QuitInboxSession\r\n");
                    WaitWindowStateEx(HwndMailGet, TRUE, IDP_MAIL_STRING_DISCONNECTING, NULL, NULL, NULL);
                }
            }
            else
            {
                if(pCreateData->bReadAlreadyDel)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MAILALDEL, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTED, NULL, Notify_Success, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
            }
        }
        return;
    }
    else
    {
        MAIL_DelHeaderFromServer(hWnd);
    }
    return;
}

static BOOL MAIL_RetriHeaderFromServer(HWND hWnd)
{
    PMAIL_INBOXCREATEDATA pCreateData;
    int iState;
    
    printf("\r\n[GARY_MAIL] : enter into MAIL_RetriHeaderFromServer");

    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
    {
        pCreateData = GetUserData(hWnd);

        WaitWin(HwndMailGet, TRUE, IDP_MAIL_STRING_RETRIEVING, NULL, NULL, NULL, NULL);
        
        if((pCreateData->GetConfig.GLRetrAtt) || (pCreateData->GetConfig.GLBoxType == MAIL_TYPE_POP3))
        {
            pCreateData->bRetrDigest = FALSE;
            iState = MAIL_RetrieveMail(GlobalMailConfigHead.GLInboxHandle, pCreateData->ReadNode->GetId, NULL);
        }
        else
        {
            pCreateData->bRetrDigest = TRUE;
            iState = MAIL_RetrieveMail(GlobalMailConfigHead.GLInboxHandle, pCreateData->ReadNode->GetId, retr_option);//"digest");
        }
    }
    else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
    {
        if((GlobalMailConfigHead.CurConnectConfig->GLRetrAtt) || (GlobalMailConfigHead.CurConnectConfig->GLBoxType == MAIL_TYPE_POP3))
        {
            RetrDigest = FALSE;
            iState = MAIL_RetrieveMail(GlobalMailConfigHead.GLInboxHandle, CurHideRetrListHead.pNext->GetId, NULL);
        }
        else
        {
            RetrDigest = TRUE;
            iState = MAIL_RetrieveMail(GlobalMailConfigHead.GLInboxHandle, CurHideRetrListHead.pNext->GetId, retr_option);//"digest");
        }
    }

    if(iState == MAIL_BLOCKED)
    {
    }
    else if(iState == MAIL_FAILURE)
    {
        if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
        {
            pCreateData->ReadNode->GetReadFlag = UNREAD;
            pCreateData->ReadNode = NULL;
            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_MAIL_LIST_REFRESH, 0), 0);
            WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RETRIEVRAL, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
            
            MAIL_GetSaveAllFile(hWnd);
        }
        else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
        {
            CurHideRetrListHead.pNext = CurHideRetrListHead.pNext->pSelNext;
            
            if(CurHideRetrListHead.pNext != NULL)
            {
                MAIL_RetriHeaderFromServer(NULL);
            }
            else
            {
                MAIL_ReleaseGetList(&CurHideGetListHead);
                MAIL_InstallMailBoxCount(GlobalMailConfigHead.GLMailBoxName);
                MAIL_ConnectServer(NULL, MAIL_GET_DISCONNECT);

                printf("\r\n[GARY_MAIL] : leave from MAIL_RetriHeaderFromServer(disconnect)");
            }
        }
    }
    else if(iState == MAIL_SUCCESS)
    {
        MAIL_RetrieveDigest(TRUE);
    }

    printf("\r\n[GARY_MAIL] : leave from MAIL_RetriHeaderFromServer");

    return TRUE;
}

void MAIL_RetrieveDigest(BOOL bSucc)
{
    HWND hWnd;
    PMAIL_INBOXCREATEDATA pCreateData;

    int hMailObj;
    char *chDigest;
    int iState;

    printf("\r\n[GARY_MAIL] : enter into MAIL_RetrieveDigest");

    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
    {
        hWnd = HwndMailInBox;
        pCreateData = GetUserData(hWnd);
    }

    if(!bSucc)
    {
        if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
        {
            pCreateData->ReadNode->GetReadFlag = UNREAD;
            pCreateData->ReadNode = NULL;
            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_MAIL_LIST_REFRESH, 0), 0);
            WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RETRIEVRAL, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
        }
        else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
        {
            CurHideRetrListHead.pNext = CurHideRetrListHead.pNext->pSelNext;
            
            if(CurHideRetrListHead.pNext != NULL)
            {
                MAIL_RetriHeaderFromServer(NULL);
            }
            else
            {
                MAIL_ReleaseGetList(&CurHideGetListHead);
                MAIL_InstallMailBoxCount(GlobalMailConfigHead.GLMailBoxName);
                MAIL_ConnectServer(NULL, MAIL_GET_DISCONNECT);

                printf("\r\n[GARY_MAIL] : leave from MAIL_RetrieveDigest(disconnect)");
            }
        }
        printf("\r\n[GARY_MAIL] : leave from MAIL_RetrieveDigest");

        return;
    }
    
    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
    {
        if(!pCreateData->bRetrDigest)
        {
            MAIL_RetrieveServerHead(hWnd);
            printf("\r\n[GARY_MAIL] : leave from MAIL_RetrieveDigest1");
            return;
        }
    }
    else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
    {
        if(!RetrDigest)
        {
            MAIL_RetrieveHideHead();

            printf("\r\n[GARY_MAIL] : leave from MAIL_RetrieveDigest");

            return;
        }
    }
    
    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
    {
        hMailObj = MAIL_CreateMailObject(GlobalMailConfigHead.GLInboxHandle, pCreateData->ReadNode->GetId);
    }
    else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
    {
        hMailObj = MAIL_CreateMailObject(GlobalMailConfigHead.GLInboxHandle, CurHideRetrListHead.pNext->GetId);
    }
    
    if(hMailObj != 0)
    {
        if(MAIL_DecodeDigest(hMailObj, &chDigest))
        {
            MAIL_DestroyMailObject(hMailObj);
            if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
            {
                MAIL_RemoveMail(GlobalMailConfigHead.GLInboxHandle, pCreateData->ReadNode->GetId);
                pCreateData->bRetrDigest = FALSE;
                iState = MAIL_RetrieveMail(GlobalMailConfigHead.GLInboxHandle, pCreateData->ReadNode->GetId, chDigest);
            }
            else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
            {
                MAIL_RemoveMail(GlobalMailConfigHead.GLInboxHandle, CurHideRetrListHead.pNext->GetId);
                RetrDigest = FALSE;
                iState = MAIL_RetrieveMail(GlobalMailConfigHead.GLInboxHandle, CurHideRetrListHead.pNext->GetId, chDigest);
            }
            
            GARY_FREE(chDigest);
            
            if(iState == MAIL_BLOCKED)
            {
            }
            else if(iState == MAIL_FAILURE)
            {
                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_MAIL_LIST_REFRESH, 0), 0);
                    WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RETRIEVRAL, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                    pCreateData->ReadNode->GetStatus = UNDOWN;
                    pCreateData->ReadNode = NULL;
                    
                    MAIL_GetSaveAllFile(hWnd);
                }
                else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
                {
                    CurHideRetrListHead.pNext = CurHideRetrListHead.pNext->pSelNext;
                    
                    if(CurHideRetrListHead.pNext != NULL)
                    {
                        MAIL_RetriHeaderFromServer(NULL);
                    }
                    else
                    {
                        MAIL_ReleaseGetList(&CurHideGetListHead);
                        MAIL_InstallMailBoxCount(GlobalMailConfigHead.GLMailBoxName);
                        MAIL_ConnectServer(NULL, MAIL_GET_DISCONNECT);

                        printf("\r\n[GARY_MAIL] : leave from MAIL_RetrieveDigest(disconnect)");
                    }
                }
            }
            else if(iState == MAIL_SUCCESS)
            {
                if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
                {
                    MAIL_RetrieveServerHead(hWnd);
                }
                else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
                {
                    MAIL_RetrieveHideHead();
                }
            }
        }
        else
        {
            if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
            {
                pCreateData->ReadNode->GetReadFlag = UNREAD;
                pCreateData->ReadNode = NULL;
                SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_MAIL_LIST_REFRESH, 0), 0);
                WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RETRIEVRAL, NULL, Notify_Failure, 
                    IDP_MAIL_BOOTEN_OK, NULL, 20);
            }
            else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
            {
                CurHideRetrListHead.pNext = CurHideRetrListHead.pNext->pSelNext;
                
                if(CurHideRetrListHead.pNext != NULL)
                {
                    MAIL_RetriHeaderFromServer(NULL);
                }
                else
                {
                    MAIL_ReleaseGetList(&CurHideGetListHead);
                    MAIL_InstallMailBoxCount(GlobalMailConfigHead.GLMailBoxName);
                    MAIL_ConnectServer(NULL, MAIL_GET_DISCONNECT);

                    printf("\r\n[GARY_MAIL] : leave from MAIL_RetrieveDigest(disconnect)");
                }
            }
        }
    }
    else
    {
        if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC)
        {
            pCreateData->ReadNode->GetReadFlag = UNREAD;
            pCreateData->ReadNode = NULL;
            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_MAIL_LIST_REFRESH, 0), 0);
            WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RETRIEVRAL, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
        }
        else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC)
        {
            CurHideRetrListHead.pNext = CurHideRetrListHead.pNext->pSelNext;
            
            if(CurHideRetrListHead.pNext != NULL)
            {
                MAIL_RetriHeaderFromServer(NULL);
            }
            else
            {
                MAIL_ReleaseGetList(&CurHideGetListHead);
                MAIL_InstallMailBoxCount(GlobalMailConfigHead.GLMailBoxName);
                MAIL_ConnectServer(NULL, MAIL_GET_DISCONNECT);

                printf("\r\n[GARY_MAIL] : leave from MAIL_RetrieveDigest(disconnect)");
            }
        }
    }

    printf("\r\n[GARY_MAIL] : leave from MAIL_RetrieveDigest");

    return;
}

static BOOL MAIL_DecodeDigest(int hMailObj, char **chDigest)
{
    int len;
    char *chTemp;
    char *FirstBracket;
    char *NextBracket;
    char *chOneEntry;

    len = MAIL_GetMailTextLength(hMailObj);
    chTemp = NULL;
    chTemp = (char *)malloc(len + 1);
    if(chTemp == NULL)
    {
        return FALSE;
    }
    memset(chTemp, 0x0, len + 1);
    len = MAIL_GetMailText(hMailObj, 0, chTemp, len);

    FirstBracket = strstr(chTemp, "[");
    if(FirstBracket == NULL)
    {
        GARY_FREE(chTemp);
        return FALSE;
    }
    while(FirstBracket != NULL)
    {
        int len;

        NextBracket = strstr((char *)(FirstBracket + 1), "[");
        if(NextBracket == NULL)
        {
            len = strlen(FirstBracket);
        }
        else
        {
            len = (int)(NextBracket - FirstBracket);
        }
        chOneEntry = malloc(len + 1);
        if(chOneEntry == NULL)
        {
            GARY_FREE(chTemp);
            return FALSE;
        }
        memset(chOneEntry, 0x0, len + 1);
        strncpy(chOneEntry, chTemp, len);

        if((strstr(chOneEntry, "type=\"TEXT/PLAIN\"") && strstr(chOneEntry, "name=\"\"")) ||
            (strstr(chOneEntry, "type=\"text/plain\"") && strstr(chOneEntry, "name=\"\"")))
        {
            char *chBLank;

            GARY_FREE(chTemp);
            chBLank = strstr(chOneEntry, " ");
            len = (int)(chBLank - chOneEntry);
            *chDigest = malloc(len + 1);
            if(*chDigest == NULL)
            {
                return FALSE;
            }
            memset(*chDigest, 0x0, len + 1);
            strncpy(*chDigest, chOneEntry, len);

            return TRUE;
        }
        FirstBracket = NextBracket;
    }
    
    GARY_FREE(chTemp);
    return FALSE;
}

static void MAIL_RetrieveServerHead(HWND hWnd)
{
    HWND hList;
    PMAIL_INBOXCREATEDATA pCreateData;

    char szOutPath[PATH_MAXLEN];
    char szDownFile[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    int hMailObj;
    int Cursel;
    
    pCreateData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_BOXMAIL_LIST);

    if(MAIL_IsFlashEnough() == 0)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DRAFTFULL, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return;
    }
    
    hMailObj = MAIL_CreateMailObject(GlobalMailConfigHead.GLInboxHandle, pCreateData->ReadNode->GetId);
    if(hMailObj != 0)
    {
        BOOL bAtt;

        bAtt = FALSE;
        if(MAIL_SaveMailContent(hMailObj, szDownFile, pCreateData->GLMailBoxName, pCreateData->ReadNode->GetSize, &bAtt))
        {
            int hf;
            MailGetListNode *pTmpNode;
            
            MU_NewMsgArrival(MU_MAIL_NOTIFY);
            
            if(bAtt)
            {
                pCreateData->ReadNode->GetAttFlag = 1;
            }
            else
            {
                pCreateData->ReadNode->GetAttFlag = 0;
            }
            
            pCreateData->ReadNode->GetStatus = DOWNSER;
            if(pCreateData->ReadNode->GetReadFlag == RETRIEVE)
            {
                pCreateData->ReadNode->GetReadFlag = UNREAD;
            }
            strcpy(pCreateData->ReadNode->GetMailFileName, szDownFile);
            if(pCreateData->bConnect)
            {
                memset(szOutPath, 0x0, PATH_MAXLEN);
                getcwd(szOutPath, PATH_MAXLEN);
                chdir(MAIL_FILE_PATH);

                remove(MAIL_FILE_GET_TMPSAVE);
                hf = GARY_open(MAIL_FILE_GET_TMPSAVE, O_RDWR | O_CREAT, S_IRWXU);
                if(hf < 0)
                {
                    printf("\r\nMAIL_RetrieveServerHead : open error = %d\r\n", errno);

                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else
                {
                    MailGetSaveFile mMailGetSaveFile;
                    
                    pTmpNode = pCreateData->GetListHead.pNext;
                    while(pTmpNode)
                    {
                        memset(&mMailGetSaveFile, 0x0, sizeof(MailGetSaveFile));
                        
                        mMailGetSaveFile.GetLocalFlag = UNDELLOCAL;
                        mMailGetSaveFile.GetServerFlag = pTmpNode->GetServerFlag;
                        mMailGetSaveFile.GetSize = pTmpNode->GetSize;
                        mMailGetSaveFile.GetStatus = pTmpNode->GetStatus;
                        mMailGetSaveFile.GetReadFlag = pTmpNode->GetReadFlag;
                        sprintf(mMailGetSaveFile.GetDate, "%s", pTmpNode->GetDate);
                        sprintf(mMailGetSaveFile.GetFrom, "%s", pTmpNode->GetFrom);
                        sprintf(mMailGetSaveFile.GetMailFileName, "%s", pTmpNode->GetMailFileName);
                        sprintf(mMailGetSaveFile.GetMsgId, "%s", pTmpNode->GetMsgId);
                        sprintf(mMailGetSaveFile.GetSubject, "%s", pTmpNode->GetSubject);
                        sprintf(mMailGetSaveFile.GetTo, "%s", pTmpNode->GetTo);
                        sprintf(mMailGetSaveFile.GetUidl, "%s", pTmpNode->GetUidl);
                        
                        write(hf, &mMailGetSaveFile, sizeof(MailGetSaveFile));
                        pTmpNode = pTmpNode->pNext;
                    }
                    GARY_close(hf);
                    remove(pCreateData->GLMailConfigSaveName);
                    rename(MAIL_FILE_GET_TMPSAVE, pCreateData->GLMailConfigSaveName);
                }
                chdir(szOutPath);
                
                SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_MAIL_LIST_REFRESH, 0), 0);
            }
            else
            {
                MAIL_GetSaveSaveNode(hWnd, pCreateData->ReadNode);
                MAIL_InstallMailBoxCount(pCreateData->GLMailBoxName);
                //MAIL_ModifyMailBoxCount(pCreateData->GLMailBoxName, -1, 0);
            }
            
            WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RETRIEVED, NULL, Notify_Success, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);

            Cursel = MAIL_GetGetNodeInt(&pCreateData->GetListHead, pCreateData->ReadNode);
            MAIL_SetImage(hList, pCreateData, pCreateData->ReadNode, Cursel);

            if(pCreateData->ReadNode->GetReadFlag == READ)
            {
                BOOL ipre, inext;

                inext = (pCreateData->ReadNode->pNext != NULL) ? TRUE : FALSE;
                ipre = (pCreateData->ReadNode == pCreateData->GetListHead.pNext) ? FALSE : TRUE;

                MAIL_CreateViewWnd(hWnd, pCreateData->ReadNode->GetMailFileName, (DWORD)pCreateData->ReadNode->GetMsgId, MU_BOXMAIL, ipre, inext);
            }
        }
        else
        {
            pCreateData->ReadNode->GetReadFlag = UNREAD;
            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_MAIL_LIST_REFRESH, 0), 0);
            WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_CONNECTFAL, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
            //MAIL_PLXTipsWin(IDP_MAIL_STRING_CONNECTFAL, Notify_Failure);
        }
        MAIL_DestroyMailObject(hMailObj);
        MAIL_RemoveMail(GlobalMailConfigHead.GLInboxHandle, pCreateData->ReadNode->GetId);
        pCreateData->ReadNode = NULL;
    }
    else
    {
        pCreateData->ReadNode->GetReadFlag = UNREAD;
        pCreateData->ReadNode = NULL;
        SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_MAIL_LIST_REFRESH, 0), 0);
        WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RETRIEVRAL, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
    }

    return;
}

static void MAIL_RetrieveHideHead()
{
    char szDownFile[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    int hMailObj;

    printf("\r\n[GARY_MAIL] : enter into MAIL_RetrieveHideHead");

    if(MAIL_IsFlashEnough() == 0)
    {
        MAIL_ReleaseGetList(&CurHideGetListHead);
        MAIL_InstallMailBoxCount(GlobalMailConfigHead.GLMailBoxName);
        MAIL_ConnectServer(NULL, MAIL_GET_DISCONNECT);

        printf("\r\n[GARY_MAIL] : leave from MAIL_RetrieveHideHead(disconnect)");
        
        return;
    }
    
    hMailObj = MAIL_CreateMailObject(GlobalMailConfigHead.GLInboxHandle, CurHideRetrListHead.pNext->GetId);
    if(hMailObj != 0)
    {
        BOOL bAtt;

        bAtt = FALSE;
        printf("\r\n[GARY_MAIL] : enter into MAIL_SaveMailContent");
        
        if(MAIL_SaveMailContent(hMailObj, szDownFile, GlobalMailConfigHead.CurConnectConfig->GLMailBoxName, 
            CurHideRetrListHead.pNext->GetSize, &bAtt))
        {
            MU_NewMsgArrival(MU_MAIL_NOTIFY);

            if(bAtt)
            {
                CurHideRetrListHead.pNext->GetAttFlag = 1;
            }
            else
            {
                CurHideRetrListHead.pNext->GetAttFlag = 0;
            }
            
            CurHideRetrListHead.pNext->GetStatus = DOWNSER;
            CurHideRetrListHead.pNext->GetReadFlag = UNREAD;
            strcpy(CurHideRetrListHead.pNext->GetMailFileName, szDownFile);
            MAIL_GetSaveSaveNode(NULL, CurHideRetrListHead.pNext);
        }
        printf("\r\n[GARY_MAIL] : leave from MAIL_SaveMailContent");

        MAIL_DestroyMailObject(hMailObj);
        MAIL_RemoveMail(GlobalMailConfigHead.GLInboxHandle, CurHideRetrListHead.pNext->GetId);
    }
    CurHideRetrListHead.pNext = CurHideRetrListHead.pNext->pSelNext;

    if(CurHideRetrListHead.pNext != NULL)
    {
        MAIL_RetriHeaderFromServer(NULL);
    }
    else
    {
        MAIL_ReleaseGetList(&CurHideGetListHead);
        MAIL_InstallMailBoxCount(GlobalMailConfigHead.GLMailBoxName);
        MAIL_ConnectServer(NULL, MAIL_GET_DISCONNECT);

        printf("\r\n[GARY_MAIL] : leave from MAIL_RetrieveHideHead(disconnect)");
    }

    printf("\r\n[GARY_MAIL] : leave from MAIL_RetrieveHideHead");

    return;
}

void MAIL_QuitServerHead()
{
    HWND hWnd;
    PMAIL_INBOXCREATEDATA pCreateData;
    
    hWnd = HwndMailInBox;
    pCreateData = GetUserData(hWnd);

    pCreateData->bConStatus = FALSE;
    memset(&pCreateData->DelListHead, 0x0, sizeof(MailGetListHead));

    WaitWin(HwndMailGet, FALSE, NULL, NULL, NULL, NULL, NULL);
    
    if(pCreateData->bConfirm == Con_Close)
    {
        PostMessage(hWnd, WM_CLOSE, 0, 0);
    }
    
    return;
}

static void MAIL_GetSaveSaveNode(HWND hWnd, MailGetListNode *pTmpNode)
{
    PMAIL_INBOXCREATEDATA pCreateData;

    MailGetSaveFile mMailGetSaveFile;
    MailGetSaveFile pTmpSave;
    int hf;
    int i;
    char szOutPath[PATH_MAXLEN];
    
    if(hWnd != NULL)
    {
        pCreateData = GetUserData(hWnd);
    }
    else
    {
    }
    
    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    
    memset(&mMailGetSaveFile, 0x0, sizeof(MailGetSaveFile));
    memset(&pTmpSave, 0x0, sizeof(MailGetSaveFile));
    
    mMailGetSaveFile.GetLocalFlag = UNDELLOCAL;
    mMailGetSaveFile.GetReadFlag = pTmpNode->GetReadFlag;
    mMailGetSaveFile.GetServerFlag = pTmpNode->GetServerFlag;
    mMailGetSaveFile.GetSize = pTmpNode->GetSize;
    mMailGetSaveFile.GetStatus = pTmpNode->GetStatus;
    sprintf(mMailGetSaveFile.GetDate, "%s", pTmpNode->GetDate);
    sprintf(mMailGetSaveFile.GetFrom, "%s", pTmpNode->GetFrom);
    sprintf(mMailGetSaveFile.GetMailFileName, "%s", pTmpNode->GetMailFileName);
    sprintf(mMailGetSaveFile.GetMsgId, "%s", pTmpNode->GetMsgId);
    sprintf(mMailGetSaveFile.GetSubject, "%s", pTmpNode->GetSubject);
    sprintf(mMailGetSaveFile.GetTo, "%s", pTmpNode->GetTo);
    sprintf(mMailGetSaveFile.GetUidl, "%s", pTmpNode->GetUidl);

    if(hWnd != NULL)
    {
        hf = GARY_open(pCreateData->GLMailConfigSaveName, O_RDWR | O_CREAT, S_IRWXU);
    }
    else
    {
        hf = GARY_open(GlobalMailConfigHead.CurConnectConfig->GLMailConfigSaveName,
            O_RDWR | O_CREAT, S_IRWXU);
    }

    if(hf < 0)
    {
        printf("\r\nMAIL_GetSaveSaveNode : open error = %d\r\n", errno);

        chdir(szOutPath);
        return;
    }
    i = 0;
    while(read(hf, &pTmpSave, sizeof(MailGetSaveFile)) == sizeof(MailGetSaveFile))
    {
        if(pTmpSave.GetLocalFlag && (strcmp(pTmpSave.GetMsgId, pTmpNode->GetMsgId) == 0))
        {
            pTmpSave.GetLocalFlag = DELLOCAL;
            lseek(hf, i * sizeof(MailGetSaveFile), SEEK_SET);
            write(hf, &pTmpSave, sizeof(MailGetSaveFile));
            break;
        }
        i ++;
    }
    
    lseek(hf, 0, SEEK_END);
    write(hf, &mMailGetSaveFile, sizeof(MailGetSaveFile));
    GARY_close(hf);
    
    if(hWnd != NULL)
    {
        pCreateData->GetListHead.dellocal ++;
    }
    
    chdir(szOutPath);
    return;
}

static void MAIL_DelSaveSaveNode(HWND hWnd, MailGetListNode *pTmpNode)
{
    PMAIL_INBOXCREATEDATA pCreateData;

    MailGetSaveFile TmpSave;
    int hf;
    int i;
    char szOutPath[PATH_MAXLEN];

    pCreateData = GetUserData(hWnd);

    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    hf = GARY_open(pCreateData->GLMailConfigSaveName, O_RDWR, -1);
    if(hf < 0)
    {
        printf("\r\nMAIL_DelSaveSaveNode : open error = %d\r\n", errno);

        chdir(szOutPath);
        return;
    }
    i = 0;
    while(read(hf, &TmpSave, sizeof(MailGetSaveFile)) == sizeof(MailGetSaveFile))
    {
        if(TmpSave.GetLocalFlag && (strcmp(TmpSave.GetMsgId, pTmpNode->GetMsgId) == 0))
        {
            TmpSave.GetLocalFlag = DELLOCAL;
            lseek(hf, i * sizeof(MailGetSaveFile), SEEK_SET);
            write(hf, &TmpSave, sizeof(MailGetSaveFile));
            pCreateData->GetListHead.dellocal ++;
            break;
        }
        i ++;
    }
    GARY_close(hf);
    chdir(szOutPath);
    return;
}

void MAIL_DelConfigSaveList(char *SaveName)
{   
    MailGetSaveFile TmpSave;
    int hf;
    int i;
    char szOutPath[PATH_MAXLEN];
    
    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    
    hf = GARY_open(SaveName, O_RDWR, -1);
    if(hf < 0)
    {
        printf("\r\nMAIL_DelConfigSaveList : open error = %d\r\n", errno);

        chdir(szOutPath);
        return;
    }
    i = 0;
    while(read(hf, &TmpSave, sizeof(MailGetSaveFile)) == sizeof(MailGetSaveFile))
    {
        if(strlen(TmpSave.GetMailFileName) != 0)
        {
            MAIL_DeleteFile(TmpSave.GetMailFileName);
        }
    }
    GARY_close(hf);
    remove(SaveName);
    chdir(szOutPath);
    
    return;
}

static void MAIL_GetSaveAllFile(HWND hWnd)
{
    PMAIL_INBOXCREATEDATA pCreateData;

    char szOutPath[PATH_MAXLEN];
    int hf;
    MailGetSaveFile mMailGetSaveFile;
    MailGetListNode *pTmpNode;
    
    pCreateData = GetUserData(hWnd);
    
    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    remove(MAIL_FILE_GET_TMPSAVE);
    hf = GARY_open(MAIL_FILE_GET_TMPSAVE, O_RDWR | O_CREAT, S_IRWXU);
    if(hf < 0)
    {
        printf("\r\nMAIL_GetSaveAllFile : open error = %d\r\n", errno);

        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
    }
    else
    {
        pTmpNode = pCreateData->GetListHead.pNext;
        while(pTmpNode)
        {
            memset(&mMailGetSaveFile, 0x0, sizeof(MailGetSaveFile));
            
            mMailGetSaveFile.GetLocalFlag = UNDELLOCAL;
            mMailGetSaveFile.GetServerFlag = pTmpNode->GetServerFlag;
            mMailGetSaveFile.GetSize = pTmpNode->GetSize;
            mMailGetSaveFile.GetStatus = pTmpNode->GetStatus;
            mMailGetSaveFile.GetReadFlag = pTmpNode->GetReadFlag;
            sprintf(mMailGetSaveFile.GetDate, "%s", pTmpNode->GetDate);
            sprintf(mMailGetSaveFile.GetFrom, "%s", pTmpNode->GetFrom);
            sprintf(mMailGetSaveFile.GetMailFileName, "%s", pTmpNode->GetMailFileName);
            sprintf(mMailGetSaveFile.GetMsgId, "%s", pTmpNode->GetMsgId);
            sprintf(mMailGetSaveFile.GetSubject, "%s", pTmpNode->GetSubject);
            sprintf(mMailGetSaveFile.GetTo, "%s", pTmpNode->GetTo);
            sprintf(mMailGetSaveFile.GetUidl, "%s", pTmpNode->GetUidl);
            
            write(hf, &mMailGetSaveFile, sizeof(MailGetSaveFile));
            pTmpNode = pTmpNode->pNext;
        }
        GARY_close(hf);
        remove(pCreateData->GLMailConfigSaveName);
        rename(MAIL_FILE_GET_TMPSAVE, pCreateData->GLMailConfigSaveName);
    }

    chdir(szOutPath);
    return;
}

static void MAIL_SelectMsg(HWND hFrameWnd, HWND hParent, BOOL bMove, BOOL bDel)
{
	WNDCLASS wc;
	PMAIL_INBOXCREATEDATA pParentCreateData;
    MAIL_OPERLISTCREATEDATA CreateData;
	HWND hSelWnd;
	RECT rClient;

	pParentCreateData = GetUserData(hParent);
	memset(&CreateData, 0x0, sizeof(MAIL_OPERLISTCREATEDATA));
    CreateData.hFrameWnd = hFrameWnd;
    CreateData.hParent = hParent;
    if(bMove)
    {
        CreateData.SelType = MOVE_SELECT;
    }
    else if(bDel)
    {
        CreateData.SelType = DELETE_SELECT;
    }

	wc.style         = 0;
	wc.lpfnWndProc   = MailSelWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof(MAIL_OPERLISTCREATEDATA);
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName  = "MailSelWndClass";

    if(!RegisterClass(&wc))
        return;

	GetClientRect(hFrameWnd, &rClient);

	hSelWnd = CreateWindow(
        "MailSelWndClass",
        NULL, 
        WS_VISIBLE | WS_CHILD, 
        rClient.left, 
        rClient.top, 
        rClient.right-rClient.left, 
        rClient.bottom-rClient.top,
        hFrameWnd,
        NULL, 
        NULL, 
        (PVOID)&CreateData
        );
	
    if(!hSelWnd)
    {
        UnregisterClass("MailSelWndClass", NULL);
        return;
    }
	SetWindowText(hFrameWnd, pParentCreateData->GLMailBoxName);

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, 
        MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), (LPARAM)IDP_MAIL_BUTTON_CANCEL);
    if(CreateData.SelType == DELETE_SELECT)
    {
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_MAIL_BUTTON_OK, 1), (LPARAM)IDP_MAIL_BUTTON_DELETE);
    }
    else if(CreateData.SelType == MOVE_SELECT)
    {
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_MAIL_BUTTON_OK, 1), (LPARAM)IDP_MAIL_BUTTON_MOVE);
    }
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    ShowWindow(hFrameWnd, SW_SHOW);
	UpdateWindow(hFrameWnd);
}

static LRESULT MailSelWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MailSel_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;
        
    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        MailSel_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;
        
    case WM_PAINT:
        MailSel_OnPaint(hWnd);
        break;
        
    case WM_KEYDOWN:
        MailSel_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;
        
    case WM_COMMAND:
        MailSel_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        MailSel_OnClose(hWnd);
        break;
        
    case WM_DESTROY:
        MailSel_OnDestroy(hWnd);
        break;

    case WM_MAIL_FOLDER_RETURN:
        MailSel_OnFolder(hWnd, wParam, lParam);
        break;

    case WM_MAIL_DELETE_RETURN:
        MailSel_OnDelete(hWnd, wParam, lParam);
        break;
        
    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    
    return lResult;
}

/*********************************************************************\
* Function	MailSel_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MailSel_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    PMAIL_OPERLISTCREATEDATA  pCreateData;
    PMAIL_INBOXCREATEDATA pInboxData;
    MailGetListNode *pSelNode;

    HWND hWndDelList;
    RECT rc;
    int i;
    
    pCreateData = (PMAIL_OPERLISTCREATEDATA)GetUserData(hWnd);
    memcpy(pCreateData, lpCreateStruct->lpCreateParams, sizeof(MAIL_OPERLISTCREATEDATA));
    pInboxData = GetUserData(pCreateData->hParent);

    GetClientRect(pCreateData->hFrameWnd, &rc);
    
    hWndDelList = CreateWindow(
        "MULTILISTBOX",
        "",
        WS_CHILD | WS_VSCROLL | LBS_BITMAP,
        rc.left, 
        rc.top,
        rc.right - rc.left, 
        rc.bottom - rc.top,
        hWnd,
        (HMENU)IDC_MAIL_SELECT_LIST,
        NULL,
        NULL);
    
    i = 0;
    if(pCreateData->SelType == MOVE_SELECT)
    {
        pSelNode = pInboxData->SelListHead.pNext;
        while(pSelNode)
        {
            SendMessage(hWndDelList, LB_INSERTSTRING, i, (LPARAM)pSelNode->GetFrom);
            pSelNode = pSelNode->pSelNext;
            i ++;
        }
    }
    else if(pCreateData->SelType == DELETE_SELECT)
    {
        pSelNode = pInboxData->GetListHead.pNext;
        while(pSelNode)
        {
            SendMessage(hWndDelList, LB_INSERTSTRING, i, (LPARAM)pSelNode->GetFrom);
            pSelNode = pSelNode->pNext;
            i ++;
        }
    }

    SetFocus(hWndDelList);

    SendMessage(hWndDelList, LB_ENDINIT, 0, 0);
    SendMessage(hWndDelList, LB_SETCURSEL, 0, 0);

    ShowWindow(hWndDelList, SW_SHOW);
    
    return TRUE;
}

/*********************************************************************\
* Function	MailSel_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MailSel_OnActivate(HWND hWnd, UINT state)
{
    HWND hList;
    PMAIL_OPERLISTCREATEDATA  pCreateData;
    PMAIL_INBOXCREATEDATA pInboxData;

    
    pCreateData = (PMAIL_OPERLISTCREATEDATA)GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_SELECT_LIST);
    pInboxData = GetUserData(pCreateData->hParent);

    SetFocus(hList);
    
    SetWindowText(pCreateData->hFrameWnd, pInboxData->GLMailBoxName);
    
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_CANCEL);
    if(pCreateData->SelType == DELETE_SELECT)
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_DELETE);
    }
    else if(pCreateData->SelType == MOVE_SELECT)
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_MOVE);
    }
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    return;
}

/*********************************************************************\
* Function	MailSel_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailSel_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	MailSel_OnKey
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
static void MailSel_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
	switch (vk)
	{
    case VK_F10:
        {
            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), 0);
        }
        break;

    case VK_RETURN:
        {
            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_OK, 0), 0);
        }
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}

/*********************************************************************\
* Function	MailSel_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MailSel_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    PMAIL_OPERLISTCREATEDATA  pCreateData;
    HWND hList;    
    
    pCreateData = (PMAIL_OPERLISTCREATEDATA)GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_SELECT_LIST);

    switch(id)
    {
    case IDM_MAIL_BUTTON_EXIT:
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        break;
        
    case IDM_MAIL_BUTTON_OK:
        {
            int count;

            count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
            if(count <= 0)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SELMESSAGE, NULL, 
                    Notify_Alert, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                return;
            }
            if(pCreateData->SelType == DELETE_SELECT)
            {
                PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_MAIL_STRING_DELSEL, Notify_Request, 
                    NULL, IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_DELETE_RETURN);
            }
            else if(pCreateData->SelType == MOVE_SELECT)
            {
                MU_FolderSelection(NULL, hWnd, WM_MAIL_FOLDER_RETURN, -1);
                //PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
            else
            {
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
        }
        break;
    }

	return;
}
/*********************************************************************\
* Function	MailSel_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailSel_OnDestroy(HWND hWnd)
{
	UnregisterClass("MailSelWndClass", NULL);
    return;
}
/*********************************************************************\
* Function	MailSel_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailSel_OnClose(HWND hWnd)
{
    PMAIL_OPERLISTCREATEDATA  pCreateData;    
    
    pCreateData = (PMAIL_OPERLISTCREATEDATA)GetUserData(hWnd);
    SendMessage(pCreateData->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
    DestroyWindow(hWnd);

    return;
}

/*********************************************************************\
* Function	MailSel_OnDelete
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailSel_OnDelete(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMAIL_OPERLISTCREATEDATA  pCreateData;
    PMAIL_INBOXCREATEDATA pInboxData;
    MailGetListNode *pTemp;
    HWND hList;
    HWND hFrameWnd;
    HWND hParent;
    
    int count;
    int* pIndex;
    int i, j;
    
    if(lParam == 0)
    {
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        return;
    }
    
    pCreateData = (PMAIL_OPERLISTCREATEDATA)GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_SELECT_LIST);
    pInboxData = GetUserData(pCreateData->hParent);

    pInboxData->SelListHead.pNext = NULL;
    
    count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
    pIndex = (int*)malloc(sizeof(int) * count);
    SendMessage(hList, LB_GETSELITEMS, count, (LPARAM)pIndex);

    pTemp = pInboxData->GetListHead.pNext;
    i = 0;
    j = 0;
    while(pTemp)
    {
        if(pIndex[j] == i)
        {
            pTemp->pSelNext = NULL;
            MAIL_AddGetListNode(&pInboxData->SelListHead, pTemp, REMOVE_SEL);
            
            j ++;
            if(j == count)
            {
                break;
            }
        }
        pTemp = pTemp->pNext;
        i ++;
    }
    free(pIndex);

    hFrameWnd = pCreateData->hFrameWnd;
    hParent = pCreateData->hParent;

    PostMessage(hWnd, WM_CLOSE, 0, 0);
    if(count > 1)
    {
        PLXConfirmWinEx(hFrameWnd, hParent, IDP_MAIL_STRING_DELSERVERS, Notify_Request, NULL, 
            IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_BOX_RETURN);
    }
    else if(count == 1)
    {
        PLXConfirmWinEx(hFrameWnd, hParent, IDP_MAIL_STRING_DELSERVER, Notify_Request, NULL, 
            IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_BOX_RETURN);
    }

    return;
}

/*********************************************************************\
* Function	MailSel_OnFolder
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailSel_OnFolder(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMAIL_OPERLISTCREATEDATA  pCreateData;
    PMAIL_INBOXCREATEDATA pInboxData;
    MailGetListNode *pTemp;
    MailGetListNode *pPre;
    HWND hList;
    HWND hParent;
    
    int count;
    int* pIndex;
    int i, j;
    
    if(!wParam)
    {
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        return;
    }
    
    pCreateData = (PMAIL_OPERLISTCREATEDATA)GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_SELECT_LIST);
    pInboxData = GetUserData(pCreateData->hParent);
    
    count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
    pIndex = (int*)malloc(sizeof(int) * count);
    SendMessage(hList, LB_GETSELITEMS, count, (LPARAM)pIndex);
    
    pTemp = pInboxData->SelListHead.pNext;
    i = 0;
    j = 0;
    while(pTemp)
    {
        if((pIndex[j] == i) && (j != count))
        {
            i ++;
            j ++;
            
            pTemp = pTemp->pSelNext;

            continue;
        }
        if(i == 0)
        {
            MAIL_RemoveGetListNode(&pInboxData->SelListHead, pTemp, REMOVE_SEL);
            pTemp = pInboxData->SelListHead.pNext;
        }
        else
        {
            pPre = pTemp;
            pTemp = pPre->pSelNext;
            MAIL_RemoveGetListNode(&pInboxData->SelListHead, pPre, REMOVE_SEL);
        }
        i ++;
    }
    free(pIndex);

    hParent = pCreateData->hParent;

    PostMessage(hWnd, WM_CLOSE, 0, 0);
    PostMessage(hParent, WM_MAIL_FOLDER_RETURN, wParam, lParam);

    return;
}

void MAIL_PLXTipsWin(const char *cTips, NOTIFYTYPE Tipspic)
{
    char ConnectedDis[MAX_MAIL_BOXNAME_LEN + 20];
    
    memset(ConnectedDis, 0x0, MAX_MAIL_BOXNAME_LEN + 20);
    sprintf(ConnectedDis, "%s%s%s", GlobalMailConfigHead.CurConnectConfig->GLMailBoxName, ":\n", cTips);

    PLXTipsWin(NULL, NULL, 0, ConnectedDis, NULL, Tipspic, 
        IDP_MAIL_BOOTEN_OK, NULL, 20);

    return;
}

