/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailSetting.c
*
* Purpose  : 
*
\**************************************************************************/

#include "MailHeader.h"

HWND HwndMailSetting;
HWND HwndMailMailBox;
HWND HwndMailBoxSelect;

//setting global
BOOL RegisterMailSetClass(void);
void UnRegisterMailSetClass(void);
BOOL MAIL_CreateSetWnd(HWND hParent);
static LRESULT MailSettingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MailSetting_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MailSetting_OnActivate(HWND hWnd, UINT state);
static void MailSetting_OnPaint(HWND hWnd);
static void MailSetting_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MailSetting_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void MailSetting_OnDestroy(HWND hWnd);
static void MailSetting_OnClose(HWND hWnd);
static void MailSetting_OnRefresh(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void MailSetting_OnDelete(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void MailSetting_OnMissData(HWND hWnd, WPARAM wParam, LPARAM lParam);
//mailbox global
BOOL RegisterMailMailBoxClass(void);
void UnRegisterMailMailBoxClass(void);
BOOL MAIL_CreateMailboxWnd(HWND hFrame, HWND hParent, const char *szOutFile, BOOL bNew, BOOL bActi);
static LRESULT MailMailBoxWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MailMailBox_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MailMailBox_OnActivate(HWND hWnd, UINT state);
static void MailMailBox_OnPaint(HWND hWnd);
static void MailMailBox_OnTimer(HWND hWnd, UINT id);
static void MailMailBox_OnKeyUp(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MailMailBox_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MailMailBox_OnKillFocus(HWND hWnd);
static void MailMailBox_OnCommand(HWND hWnd, int id, UINT codeNotify, LPARAM lParam);
static void MailMailBox_OnDestroy(HWND hWnd);
static void MailMailBox_OnClose(HWND hWnd);
static void MailMailBox_OnRefresh(HWND hWnd);
//mailbox select
BOOL RegisterMailBoxSelectClass(void);
void UnRegisterMailBoxSelectClass(void);
BOOL MAIL_CreateBoxSelectWnd(HWND hFrame, HWND hParent, MAIL_BOXLISTCREATEDATA *szOut, int bNew);
static LRESULT MailBoxSelectWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MailBoxSelect_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MailBoxSelect_OnActivate(HWND hWnd, UINT state);
static void MailBoxSelect_OnPaint(HWND hWnd);
static void MailBoxSelect_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MailBoxSelect_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void MailBoxSelect_OnDestroy(HWND hWnd);
static void MailBoxSelect_OnClose(HWND hWnd);
//setting individial
static BOOL MAIL_CreateSettingList(HWND hWnd);
static BOOL MAIL_JudgeActi(char *szName);

//extern
extern void MAIL_ReleaseConfigList(MailConfigHeader *pHead);
extern void MAIL_AddConfigNode(MailConfigHeader *pHead, MailConfigNode *pNewNode);
extern void MAIL_DelConfigNode(MailConfigHeader *pHead, int nTempNode);
extern void MAIL_RemoveConfigNode(MailConfigHeader *pHead, int nTempNode);
extern BOOL MAIL_GetConfigNodebyInt(MailConfigHeader *pHead, MailConfigNode *pConfigNode, int iTmp);
extern int MAIL_GetConfigNodebyName(MailConfigHeader *pHead, MailConfigNode *pConfigNode, const char *szTmpName);

extern void MAIL_DelConfigSaveList(char *SaveName);
/*********************************************************************\
* Function	   RegisterMailSettingClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL RegisterMailSetClass(void)
{
    WNDCLASS wc;
        
    wc.style         = 0;
    wc.lpfnWndProc   = MailSettingWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MAIL_SETTINGCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "MailSetWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}

void UnRegisterMailSetClass(void)
{
    UnregisterClass("MailSetWndClass", NULL);
}

/*********************************************************************\
* Function	   MAIL_CreateSetWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MAIL_CreateSetWnd(HWND hParent)
{
    HMENU hMenu;
    RECT rClient;
    MAIL_SETTINGCREATEDATA CreateData;

    RegisterMailSetClass();

    memset(&CreateData, 0x0, sizeof(MAIL_SETTINGCREATEDATA));
    if(hParent == NULL)
    {
        return FALSE;
    }
    CreateData.hFrameWnd = hParent;
    
    hMenu = CreateMenu();
    CreateData.hMenu = hMenu;
    CreateData.bActiBox = FALSE;

    GetClientRect(CreateData.hFrameWnd, &rClient);

    HwndMailSetting = CreateWindow(
        "MailSetWndClass", 
        "",//IDP_MAIL_TITLE_EMAIL,
        WS_VISIBLE | WS_CHILD, //PWS_STATICBAR | WS_VISIBLE | WS_CAPTION,
        rClient.left, 
        rClient.top, 
        rClient.right - rClient.left,
        rClient.bottom - rClient.top,
        CreateData.hFrameWnd,//hParent,
        NULL,//hMenu,
        NULL, 
        (PVOID)&CreateData
        );

    if(HwndMailSetting == NULL)
    {
        return FALSE;
    }
    
    AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_EDIT, IDP_MAIL_BUTTON_EDIT);
    AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_ACTI, IDP_MAIL_BUTTON_ACTI);
    AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_DELETE, IDP_MAIL_BUTTON_DELETE);
    
    PDASetMenu(CreateData.hFrameWnd, hMenu);
    SetWindowText(CreateData.hFrameWnd, IDP_MAIL_TITLE_EMAIL);

    SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_ACTI, 1), (LPARAM)"");
    SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), (LPARAM)IDP_MAIL_BUTTON_BACK);
    SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    ShowWindow(CreateData.hFrameWnd, SW_SHOW);
    UpdateWindow(CreateData.hFrameWnd);
    return TRUE;
}

/*********************************************************************\
* Function	MailSettingWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT MailSettingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MailSetting_OnCreate(hWnd, (LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        MailSetting_OnActivate(hWnd, (UINT)LOWORD(wParam));
        break;
        
    case WM_PAINT:
        MailSetting_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        MailSetting_OnKey(hWnd, (UINT)(wParam), (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        MailSetting_OnCommand(hWnd, (int)(LOWORD(wParam)), (UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        {
            PMAIL_SETTINGCREATEDATA pCreateData;
            
            pCreateData = GetUserData(hWnd);
            
            SendMessage(pCreateData->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            MailSetting_OnClose(hWnd);
        }
        break;

    case WM_DESTROY:
        MailSetting_OnDestroy(hWnd);
        break;

    case WM_MAIL_BOX_RETURN:
        MailSetting_OnRefresh(hWnd, (WPARAM)wParam, (LPARAM)lParam);
        break;

    case WM_MAIL_DELETE_RETURN:
        MailSetting_OnDelete(hWnd, (WPARAM)wParam, (LPARAM)lParam);
        break;

    case WM_MAIL_FOLDER_RETURN:
        MailSetting_OnMissData(hWnd, (WPARAM)wParam, (LPARAM)lParam);
        break;
        
    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;
}
/*********************************************************************\
* Function	MailSetting_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MailSetting_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    RECT rcClient;
    HWND hSetList;
    PMAIL_SETTINGCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);
    memcpy(pCreateData, lpCreateStruct->lpCreateParams, sizeof(MAIL_SETTINGCREATEDATA));
    
    GetClientRect(pCreateData->hFrameWnd, &rcClient);
    
    hSetList = CreateWindow(
        "LISTBOX",
        "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
        rcClient.left,
        rcClient.top,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hWnd,
        (HMENU)IDC_MAIL_SETTING_LIST,
        NULL,
        NULL);

    if(hSetList == NULL)
    {
        return FALSE;
    }

    pCreateData->hMailNew = LoadImage(NULL, MAIL_ICON_NEW, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hMailIcon = LoadImage(NULL, MAIL_ICON_ICON, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hMailActi = LoadImage(NULL, MAIL_ICON_ACTI, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    
    /*SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_ACTI, 1), (LPARAM)"");
    SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), (LPARAM)IDP_MAIL_BUTTON_BACK);
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);*/

    MAIL_CreateSettingList(hWnd);

    SetFocus(hSetList);

    return TRUE;
}

/*********************************************************************\
* Function	MailSetting_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MailSetting_OnActivate(HWND hWnd, UINT state)
{
    HWND hSetList;
    int CurSel;
    PMAIL_SETTINGCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);
    
    hSetList = GetDlgItem(hWnd, IDC_MAIL_SETTING_LIST);
    CurSel = SendMessage(hSetList, LB_GETCURSEL, 0, 0);

    SetFocus(hSetList);

    if(CurSel == 0)
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    }
    else
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_ACTI);
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    }
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_BACK);

    PDASetMenu(pCreateData->hFrameWnd, pCreateData->hMenu);

    SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_EMAIL);

    return;
}

/*********************************************************************\
* Function	MailSetting_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailSetting_OnPaint(HWND hWnd)
{
	HDC hdc;
    
    hdc = BeginPaint(hWnd, NULL);
    EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	MailSetting_OnKey
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
static void MailSetting_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PMAIL_SETTINGCREATEDATA pCreateData;
    HWND hSetList = NULL;

    pCreateData = GetUserData(hWnd);
    hSetList = GetDlgItem(hWnd, IDC_MAIL_SETTING_LIST);

	switch (vk)
	{
    case VK_F10:
        {
            PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), 0);
        }
		break;

    case VK_RETURN:
        {
            char szBtnText[100];
            
            memset(szBtnText, 0x0, 100);
            SendMessage(pCreateData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szBtnText);
            if(stricmp(szBtnText, IDP_MAIL_BUTTON_ACTI) == 0 )
            {
                PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_ACTI, 0), 0);
            }
        }
        break;

    case VK_F5:
        {
            int CurListFocus;

            CurListFocus = SendMessage(hSetList, LB_GETCURSEL, 0, 0);
            if(CurListFocus == 0)
            {
                int i;
                char tempMail[MAX_MAIL_BOXNAME_LEN + 1];
                MailConfigNode *pTemp;

                if(MAIL_IsFlashEnough() == 0)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DRAFTFULL, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                    return;
                }
                
                if(pCreateData->ConfigHead.len > 10)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MAXMAILBOX, NULL, Notify_Failure, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                    break;
                }

                for(i=1; i<10000; i++)
                {
                    memset(tempMail, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
                    sprintf(tempMail, "%s %d", IDP_MAIL_TITLE_MAILBOX, i);

                    pTemp = pCreateData->ConfigHead.pNext;
                    while(pTemp)
                    {
                        if((strcmp(pTemp->GLMailBoxName, tempMail)) == 0)
                        {
                            break;
                        }
                        pTemp = pTemp->pNext;
                    }
                    if(pTemp == NULL)
                    {
                        break;
                    }
                }
                MAIL_CreateMailboxWnd(pCreateData->hFrameWnd, hWnd, tempMail, 1, 0);
            }
            else
            {
                PDADefWindowProc(pCreateData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
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
* Function	MailSetting_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MailSetting_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    PMAIL_SETTINGCREATEDATA pCreateData;
    HWND hSetList = NULL;

    pCreateData = GetUserData(hWnd);
    hSetList = GetDlgItem(hWnd, IDC_MAIL_SETTING_LIST);

	switch(id)
	{
    case IDM_MAIL_BUTTON_EDIT:
        {
            int CurListFocus;
            MailConfigNode ConTemp;
            
            CurListFocus = SendMessage(hSetList, LB_GETCURSEL, 0, 0);
            if(CurListFocus != 0)
            {
                MAIL_GetConfigNodebyInt(&pCreateData->ConfigHead, &ConTemp, CurListFocus);
                pCreateData->CurConfig = CurListFocus;
                MAIL_CreateMailboxWnd(pCreateData->hFrameWnd, hWnd, ConTemp.GLMailBoxSaveName, 0, pCreateData->bActiBox);
            }
        }
        break;

    case IDM_MAIL_BUTTON_ACTI:
        {
            int CurListFocus;
            MailConfigNode *ConTemp;
            MailConfigNode *DelTemp;

            MailConfigNode *GloConTemp;
            MailConfigNode *GloDelTemp;

            MailConfig ActiCon;
            char szOldPath[PATH_MAXLEN];
            char Display[MAX_MAIL_BOXNAME_LEN + 30];
            int hFile;
            int i, j;

            CurListFocus = SendMessage(hSetList, LB_GETCURSEL, 0, 0);
            if(CurListFocus != 0)
            {
                szOldPath[0] = 0;
                getcwd(szOldPath, PATH_MAXLEN);
                chdir(MAIL_FILE_PATH);

                ConTemp = NULL;
                ConTemp = pCreateData->ConfigHead.pNext;
                for(i=1; i<CurListFocus+1; i++)
                {
                    ConTemp = ConTemp->pNext;
                }

                if(!MAIL_JudgeActi(ConTemp->GLMailBoxSaveName))
                {
                    PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_MAIL_STRING_MISSDATA, Notify_Request, 
                        NULL, IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_FOLDER_RETURN);
                    chdir(szOldPath);
                    break;
                }
                
                if(ConTemp->GLDefSelect == MAIL_DEF_SELECTED)
                {
                    chdir(szOldPath);
                    break;
                }

                GloConTemp = NULL;
                GloConTemp = GlobalMailConfigHead.pNext;
                for(i=1; i<CurListFocus; i++)
                {
                    GloConTemp = GloConTemp->pNext;
                }

                DelTemp = NULL;
                GloDelTemp = NULL;
                DelTemp = pCreateData->ConfigHead.pNext;
                GloDelTemp = GlobalMailConfigHead.pNext;
                j = 0;
                while(DelTemp)
                {
                    if(DelTemp->GLDefSelect == MAIL_DEF_SELECTED)
                    {
                        break;
                    }
                    j ++;
                    DelTemp = DelTemp->pNext;
                }
                while(GloDelTemp)
                {
                    if(GloDelTemp->GLDefSelect == MAIL_DEF_SELECTED)
                    {
                        break;
                    }
                    GloDelTemp = GloDelTemp->pNext;
                }
                if(DelTemp != NULL)
                {
                    hFile = GARY_open(DelTemp->GLMailBoxSaveName, O_RDWR, -1);
                    if(hFile < 0)
                    {
                        printf("\r\nMailSetting_OnCommand.IDM_MAIL_BUTTON_ACTI1 : open error = %d\r\n", errno);

                        chdir(szOldPath);
                        return;
                    }
                    memset(&ActiCon, 0x0, sizeof(MailConfig));
                    if(read(hFile, &ActiCon, sizeof(MailConfig)) != sizeof(MailConfig))
                    {
                        GARY_close(hFile);
                        chdir(szOldPath);
                        return;
                    }
                    ActiCon.GLDefSelect = MAIL_DEF_UNSELECTED;

                    lseek(hFile, 0, SEEK_SET);
                    if(write(hFile, &ActiCon, sizeof(MailConfig)) != sizeof(MailConfig))
                    {
                        GARY_close(hFile);
                        chdir(szOldPath);
                        return;
                    }
                    GARY_close(hFile);

                    DelTemp->GLDefSelect = MAIL_DEF_UNSELECTED;
                    GloDelTemp->GLDefSelect = MAIL_DEF_UNSELECTED;
                    
                 //   SendMessage(hSetList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(j, 1)), (LPARAM)"");
					SendMessage(hSetList, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), j), (LPARAM)"");
                    //SendMessage(hSetList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, j), (LPARAM)pCreateData->hMailIcon);
                }

                GloConTemp->GLDefSelect = MAIL_DEF_SELECTED;

                hFile = GARY_open(ConTemp->GLMailBoxSaveName, O_RDWR, -1);
                if(hFile < 0)
                {
                    printf("\r\nMailSetting_OnCommand.IDM_MAIL_BUTTON_ACTI2 : open error = %d\r\n", errno);

                    chdir(szOldPath);
                    return;
                }
                memset(&ActiCon, 0x0, sizeof(MailConfig));
                if(read(hFile, &ActiCon, sizeof(MailConfig)) != sizeof(MailConfig))
                {
                    GARY_close(hFile);
                    chdir(szOldPath);
                    return;
                }
                ActiCon.GLDefSelect = MAIL_DEF_SELECTED;

                lseek(hFile, 0, SEEK_SET);
                if(write(hFile, &ActiCon, sizeof(MailConfig)) != sizeof(MailConfig))
                {
                    GARY_close(hFile);
                    chdir(szOldPath);
                    return;
                }
                GARY_close(hFile);
                chdir(szOldPath);
                
                memset(&GlobalMailConfigActi, 0x0, sizeof(MailConfig));
                memcpy(&GlobalMailConfigActi, &ActiCon, sizeof(MailConfig));

                {
                    int ConNum;
                    UDB_ISPINFO uIspInfo;

                    ConNum = IspGetNum();
                    if(ConNum > 0)
                    {
                        for(i=0; i<ConNum; i++)
                        {
                            if(IspReadInfo(&uIspInfo, i))
                            {
                                if(ActiCon.GLConnect == (int)uIspInfo.ISPID)
                                {
                                    SetConnectionUsage(DIALDEF_MAIL, i);
                                    break;
                                }
                            }
                        }
                    }
                }

                sprintf(Display, "%s: %s", ConTemp->GLMailBoxName, IDP_MAIL_STRING_ACTIVATED);
                PLXTipsWin(NULL, NULL, 0, Display, NULL, Notify_Success, 
                    IDP_MAIL_BOOTEN_OK, NULL, 20);

                ConTemp->GLDefSelect = MAIL_DEF_SELECTED;
                pCreateData->ConfigHead.DefSelInt = ConTemp->GLMailBoxID;
                pCreateData->CurConfig = CurListFocus;
                
                SendMessage(hSetList, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), CurListFocus), (LPARAM)pCreateData->hMailActi);
            }
        }
        break;
        
    case IDM_MAIL_BUTTON_DELETE:
        {
            int CurListFocus;
            MailConfigNode *ConTemp;
            char Display[MAX_MAIL_BOXNAME_LEN + 30];
            int i;

            CurListFocus = SendMessage(hSetList, LB_GETCURSEL, 0, 0);
            if(CurListFocus != 0)
            {
                ConTemp = NULL;
                ConTemp = pCreateData->ConfigHead.pNext;
                for(i=1; i<CurListFocus+1; i++)
                {
                    ConTemp = ConTemp->pNext;
                }
                pCreateData->CurConfig = CurListFocus;
                memset(Display, 0x0, MAX_MAIL_BOXNAME_LEN + 30);
                if(ConTemp->GLDefSelect == MAIL_DEF_SELECTED)
                {
                    sprintf(Display, "%s: %s", ConTemp->GLMailBoxName, IDP_MAIL_STRING_DELETEACTI);
                    PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, Display, Notify_Request, NULL, 
                        IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_DELETE_RETURN);
                }
                else
                {
                    sprintf(Display, "%s: %s", ConTemp->GLMailBoxName, IDP_MAIL_STRING_DELETE);
                    PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, Display, Notify_Request, NULL, 
                        IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_DELETE_RETURN);
                }
            }
        }
        break;

    case IDM_MAIL_BUTTON_EXIT:
        {
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;

    case IDC_MAIL_SETTING_LIST:
        {
            if((codeNotify == LBN_SELCHANGE) || (codeNotify == LBN_SETFOCUS))
            {
                int CurListFocus;
                
                CurListFocus = SendMessage(hSetList, LB_GETCURSEL, 0, 0);
                if(CurListFocus == 0)
                {
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
                }
                else
                {
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_ACTI);                    
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
                }
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), 0);
            }
        }
        break;

    default:
        break;
	}
    
	return;
}

/*********************************************************************\
* Function	MailSetting_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailSetting_OnDestroy(HWND hWnd)
{
    PMAIL_SETTINGCREATEDATA pCreateData;
    HMENU hMenu;
    
    pCreateData = GetUserData(hWnd);

    MAIL_ReleaseConfigList(&pCreateData->ConfigHead);
	if(pCreateData->hMailNew)
    {
        DeleteObject(pCreateData->hMailNew);
        pCreateData->hMailNew = NULL;
    }
    if(pCreateData->hMailIcon)
    {
        DeleteObject(pCreateData->hMailIcon);
        pCreateData->hMailIcon = NULL;
    }
    if(pCreateData->hMailActi)
    {
        DeleteObject(pCreateData->hMailActi);
        pCreateData->hMailActi = NULL;
    }

    hMenu = pCreateData->hMenu;
    //hMenu = PDAGetMenu(pCreateData->hFrameWnd);
    DestroyMenu(hMenu);

    //UnRegisterMailSetClass();

    return;
}

/*********************************************************************\
* Function	MailSetting_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailSetting_OnClose(HWND hWnd)
{
    DestroyWindow(hWnd);
    return;
}

/*********************************************************************\
* Function	MailSetting_OnRefresh
* Purpose   WM_MAIL_BOX_RETURN message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailSetting_OnRefresh(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HWND hSetList;
    PMAIL_SETTINGCREATEDATA pCreateData;
    char szOldPath[PATH_MAXLEN];
    int hFile;
    MailConfig mMailConfig;
    MailConfigNode TempNode;
    MailConfigNode *pMailConfigNode;
    MailConfigNode *pGlobalMailConfig;
    int j, k;
    int iSel;
    int hFocus;

    pCreateData = GetUserData(hWnd);
    hSetList = GetDlgItem(hWnd, IDC_MAIL_SETTING_LIST);

    szOldPath[0] = 0;
    getcwd(szOldPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    pCreateData->bActiBox = FALSE;
    
    hFile = GARY_open((char *)lParam, O_RDONLY, -1);
    if(hFile < 0)
    {
        printf("\r\nMailSetting_OnRefresh : open error = %d\r\n", errno);

        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        chdir(szOldPath);
        return;
    }
    memset(&mMailConfig, 0x0, sizeof(MailConfig));
    if(read(hFile, &mMailConfig, sizeof(MailConfig)) != sizeof(MailConfig))
    {
        GARY_close(hFile);
        chdir(szOldPath);
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return;
    }
    GARY_close(hFile);
    chdir(szOldPath);

    if(LOWORD(wParam) == ID_MAILBOX_NEW)
    {        
        for(iSel=0;iSel<10;iSel++)
        {
            if(!GlobalMailConfigHead.bUsed[iSel])
            {
                break;
            }
        }
        pGlobalMailConfig = NULL;
        pGlobalMailConfig = (MailConfigNode *)(GlobalMailConfigHead.StartAdd + 
            sizeof(MailConfigNode) * iSel);
        memset(pGlobalMailConfig, 0x0, sizeof(MailConfigNode));
        GlobalMailConfigHead.bUsed[iSel] = TRUE;
    }
    else if(LOWORD(wParam) == ID_MAILBOX_MODIFY)
    {        
        j = pCreateData->CurConfig;
        pMailConfigNode = NULL;
        pMailConfigNode = pCreateData->ConfigHead.pNext;
        while(j - 1)
        {
            pMailConfigNode = pMailConfigNode->pNext;
            j --;
        }
        pMailConfigNode = pMailConfigNode->pNext;
        memset(&TempNode, 0x0, sizeof(MailConfigNode));
        memcpy(&TempNode, pMailConfigNode, sizeof(MailConfigNode));

        pGlobalMailConfig = NULL;
        pGlobalMailConfig = GlobalMailConfigHead.pNext;
        j = 0;
        while(pGlobalMailConfig)
        {
            if(strcmp(pGlobalMailConfig->GLMailBoxSaveName, TempNode.GLMailBoxSaveName) == 0)
            {
                break;
            }
            pGlobalMailConfig = pGlobalMailConfig->pNext;
            j ++;
        }
        if(pGlobalMailConfig == NULL)
        {
            for(iSel=0;iSel<10;iSel++)
            {
                if(!GlobalMailConfigHead.bUsed[iSel])
                {
                    break;
                }
            }

            pGlobalMailConfig = (MailConfigNode *)(GlobalMailConfigHead.StartAdd + 
                sizeof(MailConfigNode) * iSel);
            memset(pGlobalMailConfig, 0x0, sizeof(MailConfigNode));
            GlobalMailConfigHead.bUsed[iSel] = TRUE;

            sprintf(pGlobalMailConfig->GLMailBoxSaveName, "%s", (char *)lParam);
            strcpy(pGlobalMailConfig->GLMailBoxName, mMailConfig.GLMailBoxName);
            strcpy(pGlobalMailConfig->GLMailConfigSaveName, mMailConfig.GLConfigSaveName);
            pGlobalMailConfig->GLMailBoxID = iSel;
            pGlobalMailConfig->GLDefSelect = MAIL_DEF_UNSELECTED;
            pGlobalMailConfig->pNext = NULL;

            MAIL_AddConfigNode(&GlobalMailConfigHead, pGlobalMailConfig);
        }
        else
        {
            MailConWaitNode *pTemp;

            RTC_KillAlarms("Edit Email", (u_INT2)pGlobalMailConfig->GLMailBoxID);

            pTemp = GlobalMailConfigHead.pWaitNext;
            while(pTemp)
            {
                if(pTemp->GLMailBoxID == pGlobalMailConfig->GLMailBoxID)
                {
                    MAIL_DelDialWaitNode(&GlobalMailConfigHead, pTemp);
                    pTemp = GlobalMailConfigHead.pWaitNext;

                    continue;
                }
                pTemp = pTemp->pNext;
            }
        }
    }

    pMailConfigNode = NULL;
    pMailConfigNode = (MailConfigNode *)malloc(sizeof(MailConfigNode));
    if(pMailConfigNode == NULL)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);        
        return;
    }
    memset(pMailConfigNode, 0x0, sizeof(MailConfigNode));

    sprintf(pMailConfigNode->GLMailBoxSaveName, "%s", (char *)lParam);
    strcpy(pMailConfigNode->GLMailBoxName, mMailConfig.GLMailBoxName);
    strcpy(pMailConfigNode->GLMailConfigSaveName, mMailConfig.GLConfigSaveName);

    sprintf(pGlobalMailConfig->GLMailBoxSaveName, "%s", (char *)lParam);
    strcpy(pGlobalMailConfig->GLMailBoxName, mMailConfig.GLMailBoxName);
    strcpy(pGlobalMailConfig->GLMailConfigSaveName, mMailConfig.GLConfigSaveName);
    
    if(LOWORD(wParam) == ID_MAILBOX_NEW)
    {
        pMailConfigNode->GLMailBoxID = iSel;
        pMailConfigNode->GLDefSelect = MAIL_DEF_UNSELECTED;
        pMailConfigNode->pNext = NULL;

        pGlobalMailConfig->GLMailBoxID = iSel;
        pGlobalMailConfig->GLDefSelect = MAIL_DEF_UNSELECTED;
        pGlobalMailConfig->GLMailUnread = 0;
        pGlobalMailConfig->GLMailNum = 0;
        pGlobalMailConfig->pNext = NULL;
    }
    else if(LOWORD(wParam) == ID_MAILBOX_MODIFY)
    {
        pMailConfigNode->GLMailBoxID = TempNode.GLMailBoxID;
        pMailConfigNode->GLDefSelect = TempNode.GLDefSelect;
        pMailConfigNode->pNext = NULL;

        MAIL_DelConfigNode(&pCreateData->ConfigHead, pCreateData->CurConfig);
        MAIL_RemoveConfigNode(&GlobalMailConfigHead, j);
        pGlobalMailConfig->pNext = NULL;
    }
    
    MAIL_InitConfigNode(&GlobalMailConfigHead, pGlobalMailConfig, &mMailConfig);

    MAIL_AddConfigNode(&GlobalMailConfigHead, pGlobalMailConfig);
    MAIL_AddConfigNode(&pCreateData->ConfigHead, pMailConfigNode);

    if((LOWORD(wParam) == ID_MAILBOX_MODIFY) && (pMailConfigNode->GLDefSelect == MAIL_DEF_SELECTED))
    {
        memset(&GlobalMailConfigActi, 0x0, sizeof(MailConfig));
        memcpy(&GlobalMailConfigActi, &mMailConfig, sizeof(MailConfig));
    }

    hFocus = pMailConfigNode->GLMailBoxID;

    //add to listbox
    SendMessage(hSetList, LB_RESETCONTENT, 0, 0);
    
    j = 0;
    k = 0;
    pMailConfigNode = NULL;
    pMailConfigNode = pCreateData->ConfigHead.pNext;
    while(pMailConfigNode)
    {
        SendMessage(hSetList, LB_ADDSTRING, NULL, (LPARAM)pMailConfigNode->GLMailBoxName);
        if(j == 0)
        {
            SendMessage(hSetList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, j), (LPARAM)pCreateData->hMailNew);
        }
        else
        {
            SendMessage(hSetList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, j), (LPARAM)pCreateData->hMailIcon);
        }
        if(pMailConfigNode->GLMailBoxID == pCreateData->ConfigHead.DefSelInt)
        {
            SendMessage(hSetList, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), j), (LPARAM)pCreateData->hMailActi);
        }
        if(hFocus == pMailConfigNode->GLMailBoxID)
        {
            k = j;
        }
        j ++;
        pMailConfigNode = pMailConfigNode->pNext;
    }
    SendMessage(hSetList, LB_SETCURSEL, k, 0);
    pCreateData->CurConfig = k;
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_ACTI);                    
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);

    if(UniGlobalHwnd != NULL)
    {
        SendMessage(UniGlobalHwnd, UniGlobalMsg, 0, 0);
    }

    return;
}

/*********************************************************************\
* Function	MailSetting_OnRefresh
* Purpose   WM_MAIL_DELETE_RETURN message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailSetting_OnDelete(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int CurListFocus;
    MailConfigNode *ConTemp;
    char szOldPath[PATH_MAXLEN];
    int i;

    HWND hSetList;
    PMAIL_SETTINGCREATEDATA pCreateData;
    
    switch(lParam)
    {
    case 0:
        {
        }
        break;
        
    case 1:
        {
            pCreateData = GetUserData(hWnd);
            hSetList = GetDlgItem(hWnd, IDC_MAIL_SETTING_LIST);
            
            CurListFocus = pCreateData->CurConfig;
            if(CurListFocus != 0)
            {
                ConTemp = NULL;
                ConTemp = pCreateData->ConfigHead.pNext;
                for(i=1; i<CurListFocus+1; i++)
                {
                    ConTemp = ConTemp->pNext;
                }
                
                if(GlobalMailConfigHead.GLNetUsed)
                {
                    if(ConTemp->GLMailBoxID == GlobalMailConfigHead.CurConnectConfig->GLMailBoxID)
                    {
                        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELETEENGROSS, NULL, Notify_Alert, 
                            IDP_MAIL_BOOTEN_OK, NULL, 20);
                        return;
                    }
                }
                szOldPath[0] = 0;
                getcwd(szOldPath, PATH_MAXLEN);
                chdir(MAIL_FILE_PATH);

                MAIL_DelConfigSaveList(ConTemp->GLMailConfigSaveName);
                if(!remove(ConTemp->GLMailBoxSaveName))
                {
                    MailConfigNode TempGlobalNode;
                    int iTemp;

                    memset(&TempGlobalNode, 0x0, sizeof(MailConfigNode));
                    iTemp = MAIL_GetConfigNodebyName(&GlobalMailConfigHead, 
                        &TempGlobalNode, ConTemp->GLMailBoxName);
                    if(iTemp >= 0)
                    {
                        MailConWaitNode *pTemp;

                        RTC_KillAlarms("Edit Email", (u_INT2)TempGlobalNode.GLMailBoxID);
                        
                        pTemp = GlobalMailConfigHead.pWaitNext;
                        while(pTemp)
                        {
                            if(pTemp->GLMailBoxID == TempGlobalNode.GLMailBoxID)
                            {
                                MAIL_DelDialWaitNode(&GlobalMailConfigHead, pTemp);
                                pTemp = GlobalMailConfigHead.pWaitNext;
                                
                                continue;
                            }
                            pTemp = pTemp->pNext;
                        }
                        MAIL_RemoveConfigNode(&GlobalMailConfigHead, iTemp);
                        GlobalMailConfigHead.bUsed[TempGlobalNode.GLMailBoxID] = FALSE;
                    }
                    
                    if(ConTemp->GLDefSelect == 1)
                    {
                        pCreateData->ConfigHead.DefSelInt = -2;
                        memset(&GlobalMailConfigActi, 0x0, sizeof(MailConfig));
                    }
                    MAIL_DelConfigNode(&pCreateData->ConfigHead, CurListFocus);
                    
                    SendMessage(hSetList, LB_DELETESTRING, CurListFocus, 0);
                    SendMessage(hSetList, LB_SETCURSEL, CurListFocus - 1, 0);
                    
                    pCreateData->CurConfig --;

                    if(CurListFocus == 1)
                    {
                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
                    }
                    else
                    {
                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_ACTI);                    
                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
                    }
                    
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELETED, NULL, Notify_Success, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                chdir(szOldPath);
            }
            
            if(UniGlobalHwnd != NULL)
            {
                SendMessage(UniGlobalHwnd, UniGlobalMsg, 0, 0);
            }
        }
        break;
        
    default:
        break;
    }
    
    return;
}

/*********************************************************************\
* Function	MailSetting_OnRefresh
* Purpose   WM_MAIL_DELETE_RETURN message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailSetting_OnMissData(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMAIL_SETTINGCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);

    switch(lParam)
    {
    case 0:
        {
        }
        break;
        
    case 1:
        {
            pCreateData->bActiBox = TRUE;
            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_EDIT, 0), 0);
        }
        break;
        
    default:
        break;
    }
    
    return;
}

/*********************************************************************\
* Function	MAIL_CreateSettingList
* Purpose   
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static BOOL MAIL_CreateSettingList(HWND hWnd)
{
    HWND hSetList;
    PMAIL_SETTINGCREATEDATA pCreateData;

    char szOldPath[PATH_MAXLEN];
    static struct dirent *dirinfo = NULL;
    static DIR *diropen = NULL;

    int hFile;
    int i, j;
    MailConfig mMailConfig;
    MailConfigNode *pMailConfigNode;
    
    pCreateData = GetUserData(hWnd);
    hSetList = GetDlgItem(hWnd, IDC_MAIL_SETTING_LIST);
    
    MAIL_ReleaseConfigList(&pCreateData->ConfigHead);

    //default:new
    pMailConfigNode = NULL;
    pMailConfigNode = (MailConfigNode *)malloc(sizeof(MailConfigNode));
    if(pMailConfigNode == NULL)
    {
        return FALSE;
    }
    memset(pMailConfigNode, 0x0, sizeof(MailConfigNode));
    strcpy(pMailConfigNode->GLMailBoxName, IDP_MAIL_TITLE_NEWMAILBOX);
    pMailConfigNode->GLMailBoxID = -1;
    pMailConfigNode->pNext = NULL;
    pCreateData->ConfigHead.pNext = pMailConfigNode;
    pCreateData->ConfigHead.len ++;

    //mail box N
    szOldPath[0] = 0;
    getcwd(szOldPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    diropen = GARY_opendir(MAIL_FILE_PATH);
    if(diropen == NULL)
    {
        MAIL_ReleaseConfigList(&pCreateData->ConfigHead);
        chdir(szOldPath);
        return FALSE;
    }
    
    i = 0;
    while(dirinfo = readdir(diropen))
    {
        if(strncmp(dirinfo->d_name, MAIL_CONFIG_FILE_MATCH, 8))
        {
            continue;
        }
        
        hFile = GARY_open(dirinfo->d_name, O_RDONLY, -1);
        if(hFile < 0)
        {
            printf("\r\nMAIL_CreateSettingList : open error = %d\r\n", errno);

            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
            continue;
        }
        memset(&mMailConfig, 0x0, sizeof(MailConfig));
        if(read(hFile, &mMailConfig, sizeof(MailConfig)) != sizeof(MailConfig))
        {
            GARY_close(hFile);
            continue;
        }
        GARY_close(hFile);
        
        pMailConfigNode = NULL;
        pMailConfigNode = (MailConfigNode *)malloc(sizeof(MailConfigNode));
        if(pMailConfigNode == NULL)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
            MAIL_ReleaseConfigList(&pCreateData->ConfigHead);
            GARY_closedir(diropen);
            chdir(szOldPath);
            
            return FALSE;
        }
        memset(pMailConfigNode, 0x0, sizeof(MailConfigNode));
        sprintf(pMailConfigNode->GLMailBoxSaveName, "%s", dirinfo->d_name);
        pMailConfigNode->GLMailBoxID = i;
        pMailConfigNode->GLDefSelect = mMailConfig.GLDefSelect;
        if(pMailConfigNode->GLDefSelect == MAIL_DEF_SELECTED)
        {
            j = i;
        }
        strcpy(pMailConfigNode->GLMailBoxName, mMailConfig.GLMailBoxName);
        strcpy(pMailConfigNode->GLMailConfigSaveName, mMailConfig.GLConfigSaveName);
        pMailConfigNode->pNext = NULL;
        
        MAIL_AddConfigNode(&pCreateData->ConfigHead, pMailConfigNode);
        i ++;
    }
    pCreateData->ConfigHead.DefSelInt = j;
    GARY_closedir(diropen);
    chdir(szOldPath);

    //add to listbox
    SendMessage(hSetList, LB_RESETCONTENT, 0, 0);
    
    i = 0;
    pMailConfigNode = NULL;
    pMailConfigNode = pCreateData->ConfigHead.pNext;
    while(pMailConfigNode)
    {
        SendMessage(hSetList, LB_ADDSTRING, NULL, (LPARAM)pMailConfigNode->GLMailBoxName);
        if(i == 0)
        {
            SendMessage(hSetList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)pCreateData->hMailNew);
        }
        else
        {
            SendMessage(hSetList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)pCreateData->hMailIcon);
        }
        if(j == pMailConfigNode->GLMailBoxID)
        {
            SendMessage(hSetList, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), i), (LPARAM)pCreateData->hMailActi);
        }
        i ++;
        pMailConfigNode = pMailConfigNode->pNext;
    }
    SendMessage(hSetList, LB_SETCURSEL, 0, 0);
    
    return TRUE;
}
/**********************************mailbox**************************/

/*********************************************************************\
* Function	   RegisterMailMailBoxClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL RegisterMailMailBoxClass(void)
{
    WNDCLASS wc;
        
    wc.style         = 0;
    wc.lpfnWndProc   = MailMailBoxWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MAIL_BOXCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "MailMailBoxWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}

void UnRegisterMailMailBoxClass(void)
{
    UnregisterClass("MailMailBoxWndClass", NULL);
}

/*********************************************************************\
* Function	   MAIL_CreateMailBoxWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MAIL_CreateMailboxWnd(HWND hFrame, HWND hParent, const char *szOutFile, BOOL bNew, BOOL bActi)
{
    HMENU hMenu;
    MAIL_BOXCREATEDATA pCreateData;
    RECT rClient;

    RegisterMailMailBoxClass();

    hMenu = CreateMenu();
    pCreateData.hMenu = hMenu;

    memset(&pCreateData, 0x0, sizeof(MAIL_BOXCREATEDATA));
    pCreateData.hFrameWnd = hFrame;
    pCreateData.hParent = hParent;
    GetClientRect(pCreateData.hFrameWnd, &rClient);
    
    pCreateData.bActi = bActi;
    pCreateData.bNew = bNew;
    if(!bNew)
    {
        strcpy(pCreateData.szSaveFile, szOutFile);
    }
    else
    {
        strcpy(pCreateData.BoxConfig.GLMailBoxName, szOutFile);
    }

    HwndMailMailBox = CreateWindow(
        "MailMailBoxWndClass", 
        "",//IDP_MAIL_TITLE_MAILNAME,
        WS_VISIBLE | WS_CHILD | WS_VSCROLL,//PWS_STATICBAR | WS_VISIBLE | WS_VSCROLL | WS_CAPTION,
        rClient.left, 
        rClient.top, 
        rClient.right - rClient.left,
        rClient.bottom - rClient.top,
        pCreateData.hFrameWnd,//hParent,
        NULL,//hMenu,
        NULL, 
        (PVOID)&pCreateData
        );

    if(HwndMailMailBox == NULL)
    {
        return FALSE;
    }

    PDASetMenu(pCreateData.hFrameWnd, hMenu);
    //SetWindowText(pCreateData.hFrameWnd, IDP_MAIL_TITLE_MAILNAME);
    ShowWindow(pCreateData.hFrameWnd, SW_SHOW);
    UpdateWindow(pCreateData.hFrameWnd);
    
    return TRUE;
}

/*********************************************************************\
* Function	MailMailBoxWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT MailMailBoxWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MailMailBox_OnCreate(hWnd, (LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        MailMailBox_OnActivate(hWnd, (UINT)LOWORD(wParam));
        break;
        
    case WM_PAINT:
        MailMailBox_OnPaint(hWnd);
        break;

    case WM_TIMER:
        MailMailBox_OnTimer(hWnd, (WPARAM)(UINT)(wParam));
        break;

    case WM_KEYUP:
        MailMailBox_OnKeyUp(hWnd, (UINT)(wParam), (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam));
        break;

    case WM_KEYDOWN:
        MailMailBox_OnKey(hWnd, (UINT)(wParam), (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam));
        break;

    case WM_KILLFOCUS:
        MailMailBox_OnKillFocus(hWnd);
        break;

    case WM_COMMAND:
        MailMailBox_OnCommand(hWnd, (int)(LOWORD(wParam)), (UINT)HIWORD(wParam), (LPARAM)lParam);
        break;
        
    case WM_CLOSE:
        {
            PMAIL_BOXCREATEDATA pCreateData;
            
            pCreateData = GetUserData(hWnd);
            
            SendMessage(pCreateData->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            MailMailBox_OnClose(hWnd);
        }
        break;

    case WM_DESTROY:
        MailMailBox_OnDestroy(hWnd);
        break;

    case WM_MAIL_BOX_RETURN:
        MailMailBox_OnRefresh(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;
}

/*********************************************************************\
* Function	MailMailBox_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MailMailBox_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    RECT rcClient;
    PMAIL_BOXCREATEDATA pCreateData;
    SCROLLINFO tmpScroll;
    UDB_ISPINFO uIspInfo;

    HWND hMailName;
    HWND hConnect;
    HWND hMailAddr;
    HWND hOutServer;
    HWND hSendMsg;
    HWND hCC;
    HWND hSignature;
    HWND hLogin;
    HWND hPassword;
    HWND hInServer;
    HWND hMailType;
    HWND hAutoRetrieval;
    HWND hSecurity;
#ifdef _NOKIA_MAIL_
    HWND hSecureLogin;
#endif
    HWND hRetrAtt;
    HWND hRetrHead;
    
    pCreateData = GetUserData(hWnd);
    memcpy(pCreateData, lpCreateStruct->lpCreateParams, sizeof(MAIL_BOXCREATEDATA));
    GetClientRect(hWnd, &rcClient);

    if(pCreateData->bNew)
    {
        pCreateData->bFirstEnt = TRUE;
        pCreateData->bChange = TRUE;
        
        pCreateData->BoxConfig.GLConnect = 0;
        strcpy(pCreateData->BoxConfig.GLMailAddr, IDP_MAIL_VALUE_REQUIRED);
        strcpy(pCreateData->BoxConfig.GLOutServer, IDP_MAIL_VALUE_REQUIRED);
        pCreateData->BoxConfig.GLSendMsg = MAIL_CONNECT_WHEN;
        pCreateData->BoxConfig.GLCC = 0;
        strcpy(pCreateData->BoxConfig.GLInServer, IDP_MAIL_VALUE_REQUIRED);
        pCreateData->BoxConfig.GLBoxType = MAIL_TYPE_POP3;
        pCreateData->BoxConfig.GLAutoRetrieval = MAIL_AUTORET_OFF;
        pCreateData->BoxConfig.GLSecurity = 0;
        pCreateData->BoxConfig.GLSecureLogin = 0;
        pCreateData->BoxConfig.GLRetrAtt = 0;
        pCreateData->BoxConfig.GLRetrHead = MAIL_RET_HEADER_ALL;
        pCreateData->BoxConfig.GLDefSelect = MAIL_DEF_UNSELECTED;

        PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_SAVE, 0), 0);
    }
    else
    {
        int hFile;
        char szOldPath[PATH_MAXLEN];

        pCreateData->bFirstEnt = FALSE;
        pCreateData->bChange = FALSE;

        memset(szOldPath, 0x0, PATH_MAXLEN);
        getcwd(szOldPath, PATH_MAXLEN);
        chdir(MAIL_FILE_PATH);
        
        hFile = GARY_open(pCreateData->szSaveFile, O_RDWR, -1);
        if(hFile < 0)
        {
            printf("\r\nMailMailBox_OnCreate : open error = %d\r\n", errno);

            chdir(szOldPath);
            return FALSE;
        }
        if(read(hFile, &pCreateData->BoxConfig, sizeof(MailConfig)) != sizeof(MailConfig))
        {
            GARY_close(hFile);
            chdir(szOldPath);
            return FALSE;
        }
        GARY_close(hFile);
        chdir(szOldPath);
    }

    //mailname 0
    hMailName = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_MAILNAME,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_MAILNAME,
        NULL,
        NULL);
    SendMessage(hMailName, SSBM_ADDSTRING, 0, (LPARAM)(pCreateData->BoxConfig.GLMailBoxName));

    //connection 1
    hConnect = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_CONNECT,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 1, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_CONNECT,
        NULL,
        NULL);
    if(pCreateData->BoxConfig.GLConnect == 0)
    {
        SendMessage(hConnect, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_REQUIRED);
    }
    else
    {
        memset(&uIspInfo, 0x0, sizeof(UDB_ISPINFO));
        if(IspReadInfoByID(&uIspInfo, pCreateData->BoxConfig.GLConnect))
        {
            SendMessage(hConnect, SSBM_ADDSTRING, 0, (LPARAM)(uIspInfo.ISPName));
        }
        else
        {
            SendMessage(hConnect, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_REQUIRED);
        }
    }

    //mailaddress 2
    hMailAddr = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_MAILADDR,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 2, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_MAILADDR,
        NULL,
        NULL);
    SendMessage(hMailAddr, SSBM_ADDSTRING, 0, (LPARAM)(pCreateData->BoxConfig.GLMailAddr));

    //outgoing mail server 3
    hOutServer = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_OUTSERVER,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 3, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_OUTSERVER,
        NULL,
        NULL);
    SendMessage(hOutServer, SSBM_ADDSTRING, 0, (LPARAM)(pCreateData->BoxConfig.GLOutServer));

    //message sending 4
    hSendMsg = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_SENDMSG,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 4, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_SENDMSG,
        NULL,
        NULL);
    SendMessage(hSendMsg, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_CONNECTAUTO);
    SendMessage(hSendMsg, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_WHENCONNECT);
    if(pCreateData->BoxConfig.GLSendMsg == MAIL_CONNECT_AUTO)
    {
        SendMessage(hSendMsg, SSBM_SETCURSEL, 0, 0);
    }
    else if(pCreateData->BoxConfig.GLSendMsg == MAIL_CONNECT_WHEN)
    {
        SendMessage(hSendMsg, SSBM_SETCURSEL, 1, 0);
    }
    
    //CC to self 5
    hCC = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_CCTOSELF,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 5, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_CC,
        NULL,
        NULL);
    SendMessage(hCC, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_BUTTON_NO);
    SendMessage(hCC, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_BUTTON_YES);
    if(pCreateData->BoxConfig.GLCC == 0)
    {
        SendMessage(hCC, SSBM_SETCURSEL, 0, 0);
    }
    else if(pCreateData->BoxConfig.GLCC == 1)
    {
        SendMessage(hCC, SSBM_SETCURSEL, 1, 0);
    }

    //signature 6
    hSignature = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_SIGNATURE,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 6, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_SIGNATURE,
        NULL,
        NULL);
    SendMessage(hSignature, SSBM_ADDSTRING, 0, (LPARAM)(pCreateData->BoxConfig.GLSignatrue));

    //Login 7
    hLogin = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_LOGIN,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 7, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_LOGIN,
        NULL,
        NULL);
    SendMessage(hLogin, SSBM_ADDSTRING, 0, (LPARAM)(pCreateData->BoxConfig.GLLogin));

    //password 8
    hPassword = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_PASSWORD,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 8, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_PASSWORD,
        NULL,
        NULL);
    SendMessage(hPassword, SSBM_ADDSTRING, 0, (LPARAM)NULL);

    //incoming mail server 9
    hInServer = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_INSERVER,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 9, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_INSERVER,
        NULL,
        NULL);
    SendMessage(hInServer, SSBM_ADDSTRING, 0, (LPARAM)(pCreateData->BoxConfig.GLInServer));

    //mailbox type 10
    hMailType = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_MAILTYPE,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 10, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_MAILTYPE,
        NULL,
        NULL);

    if(!pCreateData->bNew)
    {
        if(pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_POP3)
        {
            SendMessage(hMailType, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_POP3);
        }
        else if(pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_IMAP4)
        {
            SendMessage(hMailType, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_IMAP4);
        }
        SendMessage(hMailType, SSBM_SETCURSEL, 0, 0);
    }
    else
    {
        SendMessage(hMailType, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_POP3);
        SendMessage(hMailType, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_IMAP4);
        if(pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_POP3)
        {
            SendMessage(hMailType, SSBM_SETCURSEL, 0, 0);
        }
        else if(pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_IMAP4)
        {
            SendMessage(hMailType, SSBM_SETCURSEL, 1, 0);
        }
    }

    //autoretrieval 11
    hAutoRetrieval = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_AUTORET,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 11, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_AUTORET,
        NULL,
        NULL);
    SendMessage(hAutoRetrieval, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_OFF);
    SendMessage(hAutoRetrieval, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_10MIN);
    SendMessage(hAutoRetrieval, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_30MIN);
    SendMessage(hAutoRetrieval, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_1H);
    SendMessage(hAutoRetrieval, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_2H);
    SendMessage(hAutoRetrieval, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_6H);
    SendMessage(hAutoRetrieval, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_12H);
    SendMessage(hAutoRetrieval, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_24H);
    SendMessage(hAutoRetrieval, SSBM_SETCURSEL, pCreateData->BoxConfig.GLAutoRetrieval, 0);

    //security 12
    hSecurity = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_SECURITY,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 12, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_SECURITY,
        NULL,
        NULL);
    SendMessage(hSecurity, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_OFF);
    SendMessage(hSecurity, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_SSL);
    SendMessage(hSecurity, SSBM_SETCURSEL, pCreateData->BoxConfig.GLSecurity, 0);

#ifdef _NOKIA_MAIL_
    //secure login 13
    hSecureLogin = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_SECURELOGIN,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 13, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_SECURELOGIN,
        NULL,
        NULL);
    SendMessage(hSecureLogin, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_OFF);
    SendMessage(hSecureLogin, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_ON);
    SendMessage(hSecureLogin, SSBM_SETCURSEL, pCreateData->BoxConfig.GLSecureLogin, 0);
#endif

    //retrieve attachments 13
    hRetrAtt = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_RETRATT,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 13, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_RETRATT,
        NULL,
        NULL);
    SendMessage(hRetrAtt, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_BUTTON_NO);
    SendMessage(hRetrAtt, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_BUTTON_YES);
    SendMessage(hRetrAtt, SSBM_SETCURSEL, pCreateData->BoxConfig.GLRetrAtt, 0);

    //retrieve headers 14
    hRetrHead = CreateWindow(
        "SPINBOXEX",
        IDP_MAIL_TITLE_RETRHEAD,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
        CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
        rcClient.left, PARTWND_Y + PARTWND_HEIGHT * 14, rcClient.right - rcClient.left, PARTWND_HEIGHT,
        hWnd,
        (HMENU)IDC_MAIL_BOX_RETRHEAD,
        NULL,
        NULL);
    SendMessage(hRetrHead, SSBM_ADDSTRING, 0, (LPARAM)IDP_MAIL_VALUE_ALL);
    SendMessage(hRetrHead, SSBM_ADDSTRING, 0, (LPARAM)("5"));
    SendMessage(hRetrHead, SSBM_ADDSTRING, 0, (LPARAM)("10"));
    SendMessage(hRetrHead, SSBM_ADDSTRING, 0, (LPARAM)("30"));
    SendMessage(hRetrHead, SSBM_ADDSTRING, 0, (LPARAM)("50"));
    SendMessage(hRetrHead, SSBM_ADDSTRING, 0, (LPARAM)("100"));
    SendMessage(hRetrHead, SSBM_SETCURSEL, pCreateData->BoxConfig.GLRetrHead, 0);

    tmpScroll.cbSize = sizeof(SCROLLINFO);
    tmpScroll.fMask = SIF_ALL;
    tmpScroll.nMin = 0;
    tmpScroll.nPage = 3;
    
    if(pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_POP3)
    {
#ifdef _NOKIA_MAIL_
        ShowWindow(hSecureLogin, SW_SHOW);
#endif
        ShowWindow(hRetrAtt, SW_HIDE);
        ShowWindow(hRetrHead, SW_HIDE);

        tmpScroll.nMax = 12;
    }
    else if(pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_IMAP4)
    {
#ifdef _NOKIA_MAIL_
        ShowWindow(hSecureLogin, SW_HIDE);
#endif
        ShowWindow(hRetrAtt, SW_SHOW);
        ShowWindow(hRetrHead, SW_SHOW);

        tmpScroll.nMax = 14;
    }
    
    pCreateData->bCreate = TRUE;

    if(pCreateData->bActi)
    {
        if(pCreateData->BoxConfig.GLConnect == 0)
        {
            pCreateData->hFocus = hConnect;
            pCreateData->hOldFocus = hMailName;
            tmpScroll.nPos = 0;
            pCreateData->iScrollLine = 0;
        }
        else if(strcmp(pCreateData->BoxConfig.GLMailAddr, IDP_MAIL_VALUE_REQUIRED) == 0)
        {
            pCreateData->hFocus = hMailAddr;
            pCreateData->hOldFocus = hConnect;
            tmpScroll.nPos = 1;
            pCreateData->iScrollLine = 1;
        }
        else if(strcmp(pCreateData->BoxConfig.GLOutServer, IDP_MAIL_VALUE_REQUIRED) == 0)
        {
            pCreateData->hFocus = hOutServer;
            pCreateData->hOldFocus = hMailAddr;
            tmpScroll.nPos = 2;
            pCreateData->iScrollLine = 2;
        }
        else if(strcmp(pCreateData->BoxConfig.GLInServer, IDP_MAIL_VALUE_REQUIRED) == 0)
        {
            pCreateData->hFocus = hInServer;
            pCreateData->hOldFocus = hPassword;
            tmpScroll.nPos = 8;
            pCreateData->iScrollLine = 8;
        }
    }
    else
    {
        pCreateData->hFocus = hMailName;
        pCreateData->hOldFocus = hMailName;
        tmpScroll.nPos = 0;
        pCreateData->iScrollLine = 0;
    }

    SetScrollInfo(hWnd, SB_VERT, &tmpScroll, TRUE);
    SetFocus(pCreateData->hFocus);
    
    PostMessage(hWnd, WM_MAIL_BOX_RETURN, 0, 0);
    
    if(pCreateData->bNew)
    {
        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_NEWMAILBOX);
    }
    else
    {
        SetWindowText(pCreateData->hFrameWnd, pCreateData->BoxConfig.GLMailBoxName);
    }

    SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), (LPARAM)IDP_MAIL_BUTTON_BACK);
    //SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_SAVE, 1), (LPARAM)IDP_MAIL_BUTTON_SAVE);
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    return TRUE;
}

/*********************************************************************\
* Function	MailMailBox_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MailMailBox_OnActivate(HWND hWnd, UINT state)
{
    PMAIL_BOXCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);

    /*if(state == WA_ACTIVE)
    {
        if(pCreateData->bCreate)
        {
            HWND tempFocus;

            pCreateData->bCreate = FALSE;
            tempFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_MAILNAME);
            SetFocus(tempFocus);
            return;
        }
    }*/

    SetFocus(pCreateData->hFocus);

    PDASetMenu(pCreateData->hFrameWnd, pCreateData->hMenu);
    
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_BACK);
    //SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_SAVE);
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    
    //PDADefWindowProc(hWnd, WM_ACTIVATE, state, 0);
    if(pCreateData->bNew)
    {
        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_NEWMAILBOX);
    }
    else
    {
        SetWindowText(pCreateData->hFrameWnd, pCreateData->BoxConfig.GLMailBoxName);
    }
    
    return;
}

/*********************************************************************\
* Function	MailMailBox_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailMailBox_OnPaint(HWND hWnd)
{
	HDC hdc;
    
    hdc = BeginPaint(hWnd, NULL);
    EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	MailMailBox_OnTimer
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
static void MailMailBox_OnTimer(HWND hWnd, UINT id)
{
    PMAIL_BOXCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);
    
    switch(id)
    {
    case MAIL_TIMER_ID:
        {
            if (pCreateData->nRepeats == 1)
            {
                KillTimer(hWnd, MAIL_TIMER_ID);   
                SetTimer(hWnd, MAIL_TIMER_ID, ET_REPEAT_LATER, NULL);
            }
            keybd_event(pCreateData->wKeyCode, 0, 0, 0);
        }
        break;                
        
    default:
        KillTimer(hWnd, id);
        break;
    }
    return;
}

/*********************************************************************\
* Function	MailMailBox_OnKeyUp
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
static void MailMailBox_OnKeyUp(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PMAIL_BOXCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);
    
    pCreateData->nRepeats = 0;
    
    switch(vk)
    {
    case VK_DOWN:
    case VK_UP:
        KillTimer(hWnd, MAIL_TIMER_ID);
        break;
        
    default:
        break;
    }
    return;
}

/*********************************************************************\
* Function	MailMailBox_OnKey
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
static void MailMailBox_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PMAIL_BOXCREATEDATA pCreateData;
    
    SCROLLINFO vsi;
    HWND TmpFocus;

    HWND hMailName;
    HWND hConnect;
    HWND hMailAddr;
    HWND hOutServer;
    HWND hSendMsg;
    HWND hCC;
    HWND hSignature;
    HWND hLogin;
    HWND hPassword;
    HWND hInServer;
    HWND hMailType;
    HWND hAutoRetrieval;
    HWND hSecurity;
    HWND hSecureLogin;
    HWND hRetrAtt;
    HWND hRetrHead;

    pCreateData = GetUserData(hWnd);

    hMailName   = GetDlgItem(hWnd, IDC_MAIL_BOX_MAILNAME);
    hConnect    = GetDlgItem(hWnd, IDC_MAIL_BOX_CONNECT);
    hMailAddr   = GetDlgItem(hWnd, IDC_MAIL_BOX_MAILADDR);
    hOutServer  = GetDlgItem(hWnd, IDC_MAIL_BOX_OUTSERVER);
    hSendMsg    = GetDlgItem(hWnd, IDC_MAIL_BOX_SENDMSG);
    hCC         = GetDlgItem(hWnd, IDC_MAIL_BOX_CC);
    hSignature  = GetDlgItem(hWnd, IDC_MAIL_BOX_SIGNATURE);
    hLogin      = GetDlgItem(hWnd, IDC_MAIL_BOX_LOGIN);
    hPassword   = GetDlgItem(hWnd, IDC_MAIL_BOX_PASSWORD);
    hInServer   = GetDlgItem(hWnd, IDC_MAIL_BOX_INSERVER);
    hMailType   = GetDlgItem(hWnd, IDC_MAIL_BOX_MAILTYPE);
    hAutoRetrieval = GetDlgItem(hWnd, IDC_MAIL_BOX_AUTORET);
    hSecurity   = GetDlgItem(hWnd, IDC_MAIL_BOX_SECURITY);
    hSecureLogin = GetDlgItem(hWnd, IDC_MAIL_BOX_SECURELOGIN);
    hRetrAtt    = GetDlgItem(hWnd, IDC_MAIL_BOX_RETRATT);
    hRetrHead   = GetDlgItem(hWnd, IDC_MAIL_BOX_RETRHEAD);

    if((pCreateData->nRepeats > 0) && (vk != pCreateData->wKeyCode))
    {
        KillTimer(hWnd, MAIL_TIMER_ID);
        pCreateData->nRepeats = 0;
    }
    
    pCreateData->wKeyCode = vk;
    pCreateData->nRepeats++;
    
	switch (vk)
	{
    case VK_F10:
        {
            PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), 0);
        }
		break;

    case VK_RETURN:
        {
            //PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_SAVE, 0), 0);
        }
        break;

    case VK_F5:
        {
            HWND CurHwnd;
            int TempID;
            MAIL_BOXLISTCREATEDATA tempData;

            CurHwnd = TmpFocus = GetFocus();
            while (GetParent(TmpFocus) != hWnd)
            {
                CurHwnd = GetParent(TmpFocus);
                TmpFocus = CurHwnd;
            }
            TempID = GetWindowLong(CurHwnd, GWL_ID);

            if((TempID == IDC_MAIL_BOX_MAILTYPE) && (!pCreateData->bNew))
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MODMAILTYPE, NULL, 
                    Notify_Alert, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);

                break;
            }
            
            memset(&tempData, 0x0, sizeof(MAIL_BOXLISTCREATEDATA));
            tempData.BoxID = TempID;
            memcpy(&tempData.SelConfig, &pCreateData->BoxConfig, sizeof(MailConfig));

            MAIL_CreateBoxSelectWnd(pCreateData->hFrameWnd, hWnd, &tempData, pCreateData->bNew);
        }
        break;

    case VK_DOWN:
        {
            if (pCreateData->nRepeats == 1)
            {
                SetTimer(hWnd, MAIL_TIMER_ID, ET_REPEAT_FIRST, NULL);
            }
            memset(&vsi, 0, sizeof(SCROLLINFO));
            vsi.cbSize = sizeof(vsi);
            vsi.fMask  = SIF_ALL;
            GetScrollInfo(hWnd, SB_VERT, &vsi);

            /*if((pCreateData->hFocus == hRetrHead) && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_IMAP4))
            {
                pCreateData->hOldFocus = hRetrHead;
                break;
            }
            if((pCreateData->hFocus == hSecurity) && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_POP3))
            {
                pCreateData->hOldFocus = hSecurity;
                break;
            }*/

            TmpFocus = GetNextDlgTabItem(hWnd, pCreateData->hFocus, FALSE);
            if((pCreateData->hFocus == hSecurity) && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_IMAP4))
            {
                TmpFocus = hRetrAtt;
            }
            /*if((pCreateData->hFocus == hSecureLogin) && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_POP3))
            {
                TmpFocus = hRetrHead;
            }*/
            pCreateData->hOldFocus = pCreateData->hFocus;
            pCreateData->hFocus = TmpFocus;
            SetFocus(TmpFocus);
            
            if((pCreateData->hFocus == hMailName) && (pCreateData->hOldFocus == hRetrHead) && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_IMAP4))
            {
                ScrollWindow(hWnd, 0, (12 * PARTWND_HEIGHT), NULL, NULL);
                vsi.nPos = 0;
            }
            else if((pCreateData->hFocus == hMailName) && (pCreateData->hOldFocus == hSecurity) && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_POP3))
            {
                ScrollWindow(hWnd, 0, (10 * PARTWND_HEIGHT), NULL, NULL);
                vsi.nPos = 0;
            }
            else if((pCreateData->hFocus == hSecurity) && (pCreateData->hOldFocus == hAutoRetrieval)
                && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_POP3))
            {
                ScrollWindow(hWnd, 0, 0, NULL, NULL);
            }
            else if((pCreateData->hFocus == hRetrHead) && (pCreateData->hOldFocus == hRetrAtt)
                && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_IMAP4))
            {
                ScrollWindow(hWnd, 0, 0, NULL, NULL);
            }
            else if((pCreateData->hFocus == hConnect) && (pCreateData->hOldFocus == hMailName))
            {
                ScrollWindow(hWnd, 0, 0, NULL, NULL);
            }
            else
            {
                ScrollWindow(hWnd, 0, -PARTWND_HEIGHT, NULL, NULL);
                vsi.nPos ++;
            }
            
            UpdateWindow(hWnd);
            vsi.fMask  = SIF_POS;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
        }
        break;

    case VK_UP:
        {
            if (pCreateData->nRepeats == 1)
            {
                SetTimer(hWnd, MAIL_TIMER_ID, ET_REPEAT_FIRST, NULL);
            }
            memset(&vsi, 0, sizeof(SCROLLINFO));
            vsi.cbSize = sizeof(vsi);
            vsi.fMask  = SIF_ALL;
            GetScrollInfo(hWnd, SB_VERT, &vsi);
            
            /*if(pCreateData->hFocus == hMailName)
            {
                pCreateData->hOldFocus = hMailName;
                break;
            }*/
            
            TmpFocus = GetNextDlgTabItem(hWnd, pCreateData->hFocus, TRUE);
            /*if((pCreateData->hFocus == hRetrHead) && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_POP3))
            {
                TmpFocus = hSecureLogin;
            }*/
            if((pCreateData->hFocus == hRetrAtt) && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_IMAP4))
            {
                TmpFocus = hSecurity;
            }
            pCreateData->hOldFocus = pCreateData->hFocus;
            pCreateData->hFocus = TmpFocus;
            SetFocus(TmpFocus);

            if((pCreateData->hFocus == hRetrHead) && (pCreateData->hOldFocus == hMailName) && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_IMAP4))
            {
                ScrollWindow(hWnd, 0, -(12 * PARTWND_HEIGHT), NULL, NULL);
                vsi.nPos = 12;
            }
            else if((pCreateData->hFocus == hSecurity) && (pCreateData->hOldFocus == hMailName) && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_POP3))
            {
                ScrollWindow(hWnd, 0, -(10 * PARTWND_HEIGHT), NULL, NULL);
                vsi.nPos = 10;
            }
            else if((pCreateData->hFocus == hMailName) && (pCreateData->hOldFocus == hConnect))
            {
                ScrollWindow(hWnd, 0, 0, NULL, NULL);
            }
            else if((pCreateData->hFocus == hAutoRetrieval) && (pCreateData->hOldFocus == hSecurity)
                && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_POP3))
            {
                ScrollWindow(hWnd, 0, 0, NULL, NULL);
            }
            else if((pCreateData->hFocus == hRetrAtt) && (pCreateData->hOldFocus == hRetrHead)
                && (pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_IMAP4))
            {
                ScrollWindow(hWnd, 0, 0, NULL, NULL);
            }
            else
            {
                ScrollWindow(hWnd, 0, PARTWND_HEIGHT, NULL, NULL);
                vsi.nPos --;
            }

            UpdateWindow(hWnd);
            vsi.fMask  = SIF_POS;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
        }
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}

/*********************************************************************\
* Function	MailMailBox_OnKillFocus
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MailMailBox_OnKillFocus(HWND hWnd)
{
    PMAIL_BOXCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);
    
    KillTimer(hWnd, MAIL_TIMER_ID);
    
    pCreateData->nRepeats = 0;
    pCreateData->wKeyCode = 0;

    return;
}

/*********************************************************************\
* Function	MailMailBox_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MailMailBox_OnCommand(HWND hWnd, int id, UINT codeNotify, LPARAM lParam)
{
    PMAIL_BOXCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);

	switch(id)
	{
    case IDM_MAIL_BUTTON_SAVE:
        {
            int hFile;
            char szOldPath[PATH_MAXLEN];
            char szSaveFile[MAX_MAIL_SAVE_FILENAME_LEN + 1];
            
            if(!pCreateData->bNew  && pCreateData->bChange && GlobalMailConfigHead.GLNetUsed)
            {
                if(strcmp(pCreateData->szSaveFile, GlobalMailConfigHead.CurConnectConfig->GLMailBoxSaveName) == 0)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MODIFYENGROSS, NULL, 
                        Notify_Alert, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                    return;
                }
            }

            memset(szOldPath, 0x0, PATH_MAXLEN);
            getcwd(szOldPath, PATH_MAXLEN);
            chdir(MAIL_FILE_PATH);

            memset(szSaveFile, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
            if(pCreateData->bFirstEnt)
            {
                strcpy(szSaveFile, MAIL_CONFIG_FILE);
                MAIL_GetNewFileName(szSaveFile, MAX_MAIL_SAVE_FILENAME_LEN + 1);
                strcpy(pCreateData->szSaveFile, szSaveFile);
                memset(szSaveFile, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
                strcpy(szSaveFile, MAIL_FILE_GET_LIST);
                MAIL_GetNewFileName(szSaveFile, MAX_MAIL_SAVE_FILENAME_LEN + 1);
                strcpy(pCreateData->BoxConfig.GLConfigSaveName, szSaveFile);
                hFile = GARY_open(pCreateData->BoxConfig.GLConfigSaveName, O_CREAT, S_IRWXU);
                if(hFile < 0)
                {
                    printf("\r\nMailMailBox_OnCommand.IDM_MAIL_BUTTON_SAVE1 : open error = %d\r\n", errno);
                }
                else
                {
                    GARY_close(hFile);
                }
            }

            hFile = GARY_open(pCreateData->szSaveFile, O_RDWR | O_CREAT, S_IRWXU);
            if(hFile < 0)
            {
                printf("\r\nMailMailBox_OnCommand.IDM_MAIL_BUTTON_SAVE2 : open error = %d\r\n", errno);

                chdir(szOldPath);
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVEDFAIL, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                break;
            }
            lseek(hFile, 0, SEEK_SET);
            if(write(hFile, &pCreateData->BoxConfig, sizeof(MailConfig)) != sizeof(MailConfig))
            {
                GARY_close(hFile);
                chdir(szOldPath);
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVEDFAIL, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                break;
            }
            GARY_close(hFile);
            chdir(szOldPath);
            if(pCreateData->bFirstEnt == TRUE)
            {
                pCreateData->bFirstEnt = FALSE;
                break;
            }
            if(pCreateData->bNew)
            {
                SendMessage(pCreateData->hParent, WM_MAIL_BOX_RETURN, 
                    MAKEWPARAM(ID_MAILBOX_NEW, 0), (LPARAM)&pCreateData->szSaveFile);
            }
            else
            {
                SendMessage(pCreateData->hParent, WM_MAIL_BOX_RETURN, 
                    MAKEWPARAM(ID_MAILBOX_MODIFY, 0), (LPARAM)&pCreateData->szSaveFile);
            }
            //PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;

    case IDM_MAIL_BUTTON_EXIT:
        {
            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_SAVE, 0), 0);
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;

    case IDC_MAIL_BOX_MAILNAME:
        {
            HWND hCurFocus;

            if(codeNotify == ID_MAILBOX_SELECT_RET)
            {
                pCreateData->bChange = TRUE;

                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_MAILNAME);
                memset(pCreateData->BoxConfig.GLMailBoxName, 0x0, MAX_MAIL_BOXNAME_LEN + 1);
                strcpy(pCreateData->BoxConfig.GLMailBoxName, (char *)lParam);
                SendMessage(hCurFocus, SSBM_SETTEXT, 0, (LPARAM)lParam);
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;

    case IDC_MAIL_BOX_CONNECT:
        {
            HWND hCurFocus;
            
            if(codeNotify == ID_MAILBOX_SELECT_RET)
            {
                pCreateData->bChange = TRUE;

                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_CONNECT);

                if(lParam == NULL)
                {
                    pCreateData->BoxConfig.GLConnect = 0;
                    SendMessage(hCurFocus, SSBM_SETTEXT, 0, (LPARAM)IDP_MAIL_VALUE_REQUIRED);
                }
                else
                {
                    pCreateData->BoxConfig.GLConnect = ((UDB_ISPINFO *)lParam)->ISPID;
                    SendMessage(hCurFocus, SSBM_SETTEXT, 0, (LPARAM)((UDB_ISPINFO *)lParam)->ISPName);
                }
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;

    case IDC_MAIL_BOX_MAILADDR:
        {
            HWND hCurFocus;
            
            if(codeNotify == ID_MAILBOX_SELECT_RET)
            {
                pCreateData->bChange = TRUE;

                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_MAILADDR);
                memset(pCreateData->BoxConfig.GLMailAddr, 0x0, MAX_MAIL_MAILADDR_LEN + 1);
                strcpy(pCreateData->BoxConfig.GLMailAddr, (char *)lParam);
                SendMessage(hCurFocus, SSBM_SETTEXT, 0, (LPARAM)lParam);
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;

    case IDC_MAIL_BOX_OUTSERVER:
        {
            HWND hCurFocus;
            
            if(codeNotify == ID_MAILBOX_SELECT_RET)
            {
                pCreateData->bChange = TRUE;
                
                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_OUTSERVER);
                memset(pCreateData->BoxConfig.GLOutServer, 0x0, MAX_MAIL_OUTGOSERVER_LEN + 1);
                strcpy(pCreateData->BoxConfig.GLOutServer, (char *)lParam);
                SendMessage(hCurFocus, SSBM_SETTEXT, 0, (LPARAM)lParam);
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;
        
    case IDC_MAIL_BOX_SENDMSG:
        {
            HWND hCurFocus;
            int iFocus;
            
            if((codeNotify == ID_MAILBOX_SELECT_RET) || (codeNotify == SSBN_CHANGE))
            {
                pCreateData->bChange = TRUE;

                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_SENDMSG);
                if(codeNotify == ID_MAILBOX_SELECT_RET)
                {
                    iFocus = *((int *)lParam);
                    SendMessage(hCurFocus, SSBM_SETCURSEL, iFocus, 0);
                }
                if(codeNotify == SSBN_CHANGE)
                {
                    iFocus = SendMessage(hCurFocus, SSBM_GETCURSEL, 0, 0);
                }
                pCreateData->BoxConfig.GLSendMsg = iFocus;
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;

    case IDC_MAIL_BOX_CC:
        {
            HWND hCurFocus;
            int iFocus;
            
            if((codeNotify == ID_MAILBOX_SELECT_RET) || (codeNotify == SSBN_CHANGE))
            {
                pCreateData->bChange = TRUE;
                
                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_CC);
                if(codeNotify == ID_MAILBOX_SELECT_RET)
                {
                    iFocus = *((int *)lParam);
                    SendMessage(hCurFocus, SSBM_SETCURSEL, iFocus, 0);
                }
                if(codeNotify == SSBN_CHANGE)
                {
                    iFocus = SendMessage(hCurFocus, SSBM_GETCURSEL, 0, 0);
                }
                pCreateData->BoxConfig.GLCC = iFocus;
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;

    case IDC_MAIL_BOX_SIGNATURE:
        {
            HWND hCurFocus;
            
            if(codeNotify == ID_MAILBOX_SELECT_RET)
            {
                pCreateData->bChange = TRUE;

                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_SIGNATURE);
                memset(pCreateData->BoxConfig.GLSignatrue, 0x0, MAX_MAIL_SIGNATURE_LEN + 1);
                strcpy(pCreateData->BoxConfig.GLSignatrue, (char *)lParam);
                SendMessage(hCurFocus, SSBM_SETTEXT, 0, (LPARAM)lParam);
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;

    case IDC_MAIL_BOX_LOGIN:
        {
            HWND hCurFocus;
            
            if(codeNotify == ID_MAILBOX_SELECT_RET)
            {
                pCreateData->bChange = TRUE;

                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_LOGIN);
                memset(pCreateData->BoxConfig.GLLogin, 0x0, MAX_MAIL_LOGIN_LEN + 1);
                strcpy(pCreateData->BoxConfig.GLLogin, (char *)lParam);
                SendMessage(hCurFocus, SSBM_SETTEXT, 0, (LPARAM)lParam);
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;

    case IDC_MAIL_BOX_PASSWORD:
        {
            HWND hCurFocus;
            
            if(codeNotify == ID_MAILBOX_SELECT_RET)
            {
                pCreateData->bChange = TRUE;

                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_PASSWORD);
                memset(pCreateData->BoxConfig.GLPassword, 0x0, MAX_MAIL_LOGIN_LEN + 1);
                strcpy(pCreateData->BoxConfig.GLPassword, (char *)lParam);
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;
        
    case IDC_MAIL_BOX_INSERVER:
        {
            HWND hCurFocus;
            
            if(codeNotify == ID_MAILBOX_SELECT_RET)
            {
                pCreateData->bChange = TRUE;
                
                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_INSERVER);
                memset(pCreateData->BoxConfig.GLInServer, 0x0, MAX_MAIL_INCOMSERVER_LEN + 1);
                strcpy(pCreateData->BoxConfig.GLInServer, (char *)lParam);
                SendMessage(hCurFocus, SSBM_SETTEXT, 0, (LPARAM)lParam);
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;

    case IDC_MAIL_BOX_MAILTYPE:
        {
            HWND hCurFocus;
            //HWND hSecureLogin;
            HWND hRetrAtt;
            HWND hRetrHead;

            SCROLLINFO vsi;
            int iFocus;
            
            if((!pCreateData->bNew) && (codeNotify == SSBN_CHANGE))
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MODMAILTYPE, NULL, 
                    Notify_Alert, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                break;
            }
            
            if((codeNotify == ID_MAILBOX_SELECT_RET) || (codeNotify == SSBN_CHANGE))
            {
                pCreateData->bChange = TRUE;

                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_MAILTYPE);
                if(codeNotify == ID_MAILBOX_SELECT_RET)
                {
                    iFocus = *((int *)lParam);
                    SendMessage(hCurFocus, SSBM_SETCURSEL, iFocus, 0);
                }
                if(codeNotify == SSBN_CHANGE)
                {
                    iFocus = SendMessage(hCurFocus, SSBM_GETCURSEL, 0, 0);
                }
                pCreateData->BoxConfig.GLBoxType = iFocus;
                
                memset(&vsi, 0, sizeof(SCROLLINFO));
                vsi.cbSize = sizeof(vsi);
                vsi.fMask  = SIF_ALL;
                GetScrollInfo(hWnd, SB_VERT, &vsi);
                
                //hSecureLogin = GetDlgItem(hWnd, IDC_MAIL_BOX_SECURELOGIN);
                hRetrAtt = GetDlgItem(hWnd, IDC_MAIL_BOX_RETRATT);
                hRetrHead = GetDlgItem(hWnd, IDC_MAIL_BOX_RETRHEAD);
                if(pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_POP3)
                {
                    //ShowWindow(hSecureLogin, SW_SHOW);
                    ShowWindow(hRetrAtt, SW_HIDE);
                    ShowWindow(hRetrHead, SW_HIDE);
                    vsi.nMax = 12;
                }
                else if(pCreateData->BoxConfig.GLBoxType == MAIL_TYPE_IMAP4)
                {
                    //ShowWindow(hSecureLogin, SW_HIDE);
                    ShowWindow(hRetrAtt, SW_SHOW);
                    ShowWindow(hRetrHead, SW_SHOW);
                    vsi.nMax = 14;
                }

                vsi.fMask  = SIF_RANGE;
                SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;

    case IDC_MAIL_BOX_AUTORET:
        {
            HWND hCurFocus;
            int iFocus;
            
            if((codeNotify == ID_MAILBOX_SELECT_RET) || (codeNotify == SSBN_CHANGE))
            {
                pCreateData->bChange = TRUE;

                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_AUTORET);
                if(codeNotify == ID_MAILBOX_SELECT_RET)
                {
                    iFocus = *((int *)lParam);
                    SendMessage(hCurFocus, SSBM_SETCURSEL, iFocus, 0);
                }
                if(codeNotify == SSBN_CHANGE)
                {
                    iFocus = SendMessage(hCurFocus, SSBM_GETCURSEL, 0, 0);
                }
                pCreateData->BoxConfig.GLAutoRetrieval = iFocus;
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;

    case IDC_MAIL_BOX_SECURITY:
        {
            HWND hCurFocus;
            int iFocus;
            
            if((codeNotify == ID_MAILBOX_SELECT_RET) || (codeNotify == SSBN_CHANGE))
            {
                pCreateData->bChange = TRUE;
                
                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_SECURITY);
                if(codeNotify == ID_MAILBOX_SELECT_RET)
                {
                    iFocus = *((int *)lParam);
                    SendMessage(hCurFocus, SSBM_SETCURSEL, iFocus, 0);
                }
                if(codeNotify == SSBN_CHANGE)
                {
                    iFocus = SendMessage(hCurFocus, SSBM_GETCURSEL, 0, 0);
                }
                pCreateData->BoxConfig.GLSecurity = iFocus;
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;

#ifdef _NOKIA_MAIL_
    case IDC_MAIL_BOX_SECURELOGIN:
        {
            HWND hCurFocus;
            int iFocus;
            
            if((codeNotify == ID_MAILBOX_SELECT_RET) || (codeNotify == SSBN_CHANGE))
            {
                pCreateData->bChange = TRUE;

                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_SECURELOGIN);
                if(codeNotify == ID_MAILBOX_SELECT_RET)
                {
                    iFocus = *((int *)lParam);
                    SendMessage(hCurFocus, SSBM_SETCURSEL, iFocus, 0);
                }
                if(codeNotify == SSBN_CHANGE)
                {
                    iFocus = SendMessage(hCurFocus, SSBM_GETCURSEL, 0, 0);
                }
                pCreateData->BoxConfig.GLSecureLogin = iFocus;
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;
#endif

    case IDC_MAIL_BOX_RETRATT:
        {
            HWND hCurFocus;
            int iFocus;
            
            if((codeNotify == ID_MAILBOX_SELECT_RET) || (codeNotify == SSBN_CHANGE))
            {
                pCreateData->bChange = TRUE;

                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_RETRATT);
                if(codeNotify == ID_MAILBOX_SELECT_RET)
                {
                    iFocus = *((int *)lParam);
                    SendMessage(hCurFocus, SSBM_SETCURSEL, iFocus, 0);
                }
                if(codeNotify == SSBN_CHANGE)
                {
                    iFocus = SendMessage(hCurFocus, SSBM_GETCURSEL, 0, 0);
                }
                pCreateData->BoxConfig.GLRetrAtt = iFocus;
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;

    case IDC_MAIL_BOX_RETRHEAD:
        {
            HWND hCurFocus;
            int iFocus;
            
            if((codeNotify == ID_MAILBOX_SELECT_RET) || (codeNotify == SSBN_CHANGE))
            {
                pCreateData->bChange = TRUE;

                hCurFocus = GetDlgItem(hWnd, IDC_MAIL_BOX_RETRHEAD);
                if(codeNotify == ID_MAILBOX_SELECT_RET)
                {
                    iFocus = *((int *)lParam);
                    SendMessage(hCurFocus, SSBM_SETCURSEL, iFocus, 0);
                }
                if(codeNotify == SSBN_CHANGE)
                {
                    iFocus = SendMessage(hCurFocus, SSBM_GETCURSEL, 0, 0);
                }
                pCreateData->BoxConfig.GLRetrHead = iFocus;
            }
            else
            {
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), lParam);
            }
        }
        break;
        
    default:
        break;
	}
    
	return;
}

/*********************************************************************\
* Function	MailMailBox_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailMailBox_OnDestroy(HWND hWnd)
{
    PMAIL_BOXCREATEDATA pCreateData;
    HMENU hMenu;
    
    pCreateData = GetUserData(hWnd);
    
    hMenu = pCreateData->hMenu;
    //hMenu = PDAGetMenu(pCreateData->hFrameWnd);
    DestroyMenu(hMenu);

    //UnRegisterMailMailBoxClass();

    return;
}

/*********************************************************************\
* Function	MailMailBox_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailMailBox_OnClose(HWND hWnd)
{
    DestroyWindow(hWnd);
    return;
}

/*********************************************************************\
* Function	MailMailBox_OnRefresh
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailMailBox_OnRefresh(HWND hWnd)
{
    PMAIL_BOXCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);

    ScrollWindow(hWnd, 0, -(pCreateData->iScrollLine * PARTWND_HEIGHT), NULL, NULL);
    UpdateWindow(hWnd);

    return;
}

/****************************detail selection**************************/

/*********************************************************************\
* Function	   RegisterMailBoxSelectClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL RegisterMailBoxSelectClass(void)
{
    WNDCLASS wc;
        
    wc.style         = 0;
    wc.lpfnWndProc   = MailBoxSelectWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MAIL_BOXLISTCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "MailBoxSelectWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}

void UnRegisterMailBoxSelectClass(void)
{
    UnregisterClass("MailBoxSelectWndClass", NULL);
}

/*********************************************************************\
* Function	   MAIL_CreateBoxSelectWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MAIL_CreateBoxSelectWnd(HWND hFrame, HWND hParent, MAIL_BOXLISTCREATEDATA *szOut, int bNew)
{
    //HMENU hMenu;
    MAIL_BOXLISTCREATEDATA pCreateData;
    RECT rClient;

    RegisterMailBoxSelectClass();

    //hMenu = CreateMenu();

    memset(&pCreateData, 0x0, sizeof(MAIL_BOXLISTCREATEDATA));
    pCreateData.bNew = bNew;
    pCreateData.BoxID = szOut->BoxID;
    pCreateData.hParent = hParent;
    pCreateData.hFrameWnd = hFrame;
    GetClientRect(pCreateData.hFrameWnd, &rClient);
    memcpy(&pCreateData.SelConfig, &szOut->SelConfig, sizeof(MailConfig));

    HwndMailBoxSelect = CreateWindow(
        "MailBoxSelectWndClass", 
        "",//IDP_MAIL_TITLE_MAILNAME,
        WS_VISIBLE | WS_CHILD,// | WS_VSCROLL,//PWS_STATICBAR | WS_VISIBLE | WS_VSCROLL | WS_CAPTION,
        rClient.left, 
        rClient.top, 
        rClient.right - rClient.left,
        rClient.bottom - rClient.top,  
        pCreateData.hFrameWnd,//hParent,
        NULL,//hMenu,
        NULL, 
        (PVOID)&pCreateData
        );

    if(HwndMailBoxSelect == NULL)
    {
        return FALSE;
    }
    
    if((pCreateData.BoxID == IDC_MAIL_BOX_MAILNAME) || (pCreateData.BoxID == IDC_MAIL_BOX_MAILADDR)
        || (pCreateData.BoxID == IDC_MAIL_BOX_OUTSERVER) || (pCreateData.BoxID == IDC_MAIL_BOX_SIGNATURE)
        || (pCreateData.BoxID == IDC_MAIL_BOX_LOGIN) || (pCreateData.BoxID == IDC_MAIL_BOX_PASSWORD)
        || (pCreateData.BoxID == IDC_MAIL_BOX_INSERVER))
    {
        if(pCreateData.bNew)
        {
            SetWindowText(pCreateData.hFrameWnd, IDP_MAIL_TITLE_NEWMAILBOX);
        }
        else
        {
            SetWindowText(pCreateData.hFrameWnd, pCreateData.SelConfig.GLMailBoxName);
        }
    }
    else if(pCreateData.BoxID == IDC_MAIL_BOX_CONNECT)
    {
        SetWindowText(pCreateData.hFrameWnd, IDP_MAIL_TITLE_CONNECT);
    }
    else if(pCreateData.BoxID == IDC_MAIL_BOX_SENDMSG)
    {
        SetWindowText(pCreateData.hFrameWnd, IDP_MAIL_TITLE_SENDMSG);
    }
    else if(pCreateData.BoxID == IDC_MAIL_BOX_CC)
    {
        SetWindowText(pCreateData.hFrameWnd, IDP_MAIL_TITLE_CCTOSELF);
    }
    else if(pCreateData.BoxID == IDC_MAIL_BOX_MAILTYPE)
    {
        SetWindowText(pCreateData.hFrameWnd, IDP_MAIL_TITLE_MAILTYPE);
    }
    else if(pCreateData.BoxID == IDC_MAIL_BOX_AUTORET)
    {
        SetWindowText(pCreateData.hFrameWnd, IDP_MAIL_TITLE_AUTORET);
    }
    else if(pCreateData.BoxID == IDC_MAIL_BOX_SECURITY)
    {
        SetWindowText(pCreateData.hFrameWnd, IDP_MAIL_TITLE_SECURITY);
    }
#ifdef _NOKIA_MAIL_
    else if(pCreateData.BoxID == IDC_MAIL_BOX_SECURELOGIN)
    {
        SetWindowText(pCreateData.hFrameWnd, IDP_MAIL_TITLE_SECURELOGIN);
    }
#endif
    else if(pCreateData.BoxID == IDC_MAIL_BOX_RETRATT)
    {
        SetWindowText(pCreateData.hFrameWnd, IDP_MAIL_TITLE_RETRATT);
    }
    else if(pCreateData.BoxID == IDC_MAIL_BOX_RETRHEAD)
    {
        SetWindowText(pCreateData.hFrameWnd, IDP_MAIL_TITLE_RETRHEAD);
    }
    
    ShowWindow(pCreateData.hFrameWnd, SW_SHOW);
    UpdateWindow(pCreateData.hFrameWnd);

    if((pCreateData.BoxID == IDC_MAIL_BOX_MAILNAME) || (pCreateData.BoxID == IDC_MAIL_BOX_MAILADDR)
        || (pCreateData.BoxID == IDC_MAIL_BOX_OUTSERVER) || (pCreateData.BoxID == IDC_MAIL_BOX_SIGNATURE)
        || (pCreateData.BoxID == IDC_MAIL_BOX_LOGIN) || (pCreateData.BoxID == IDC_MAIL_BOX_PASSWORD)
        || (pCreateData.BoxID == IDC_MAIL_BOX_INSERVER))
    {
        HWND hTemPublic;

        hTemPublic = GetDlgItem(HwndMailBoxSelect, IDC_MAIL_SELECT_PUBLIC);
        SendMessage(hTemPublic, EM_SETSEL, -1, -1);
    }
    
    return TRUE;
}

/*********************************************************************\
* Function	MailBoxSelectWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT MailBoxSelectWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MailBoxSelect_OnCreate(hWnd, (LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        MailBoxSelect_OnActivate(hWnd, (UINT)LOWORD(wParam));
        break;
        
    case WM_PAINT:
        MailBoxSelect_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        MailBoxSelect_OnKey(hWnd, (UINT)(wParam), (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        MailBoxSelect_OnCommand(hWnd, (int)(LOWORD(wParam)), (UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        {
            PMAIL_BOXLISTCREATEDATA pCreateData;
            
            pCreateData = GetUserData(hWnd);
            
            SendMessage(pCreateData->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            MailBoxSelect_OnClose(hWnd);
        }
        break;

    case WM_DESTROY:
        MailBoxSelect_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;
}

/*********************************************************************\
* Function	MailBoxSelect_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MailBoxSelect_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    RECT rcClient;
    PMAIL_BOXLISTCREATEDATA pCreateData;
    IMEEDIT ie;
    HWND hPublic;

    pCreateData = GetUserData(hWnd);
    memcpy(pCreateData, lpCreateStruct->lpCreateParams, sizeof(MAIL_BOXLISTCREATEDATA));
    GetClientRect(pCreateData->hFrameWnd, &rcClient);

    pCreateData->bCreate = TRUE;

    pCreateData->hSelect = LoadImage(NULL, MAIL_ICON_SELECT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hUnSelect = LoadImage(NULL, MAIL_ICON_UNSELECT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);

    memset(&ie, 0, sizeof(IMEEDIT));

    ie.hwndNotify	= (HWND)hWnd;
    ie.dwAttrib	    = 0;                
    ie.dwAscTextMax	= 0;
    ie.dwUniTextMax	= 0;
    ie.wPageMax	    = 0;        
    ie.pszCharSet	= NULL;
    ie.pszTitle	    = NULL;
    ie.pszImeName	= NULL;
    
    //mailname
    if(pCreateData->BoxID == IDC_MAIL_BOX_MAILNAME)
    {
        hPublic = CreateWindow(
            "IMEEDIT", 
            pCreateData->SelConfig.GLMailBoxName, 
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_TITLE | ES_AUTOHSCROLL,
            rcClient.left, 
            rcClient.top, 
            rcClient.right - rcClient.left, 
            PARTWND_HEIGHT,
            hWnd, 
            (HMENU)IDC_MAIL_SELECT_PUBLIC, 
            NULL, 
            (PVOID)&ie);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, EM_LIMITTEXT, MAX_MAIL_BOXNAME_LEN, 0);
        SendMessage(hPublic, EM_SETTITLE, 0, (LPARAM)IDP_MAIL_TITLE_MAILNAMECOLON);
        SetWindowText(hPublic, pCreateData->SelConfig.GLMailBoxName);
    }

    //connect
    else if(pCreateData->BoxID == IDC_MAIL_BOX_CONNECT)
    {
        int ConNum;
        UDB_ISPINFO uIspInfo;
        int i, j;

        hPublic = CreateWindow(
            "LISTBOX",
            IDP_MAIL_TITLE_CONNECT,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
            rcClient.left,
            rcClient.top,
            rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top,
            hWnd,
            (HMENU)IDC_MAIL_SELECT_PUBLIC,
            NULL,
            NULL);
        
        if(hPublic == NULL)
            return FALSE;

        ConNum = IspGetNum();//ConNum = 1;
        if(ConNum == 0)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOCONNECT, NULL, Notify_Alert, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);

            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return FALSE;
        }
        j = -1;
        for(i=0; i<ConNum; i++)
        {
            if(IspReadInfo(&uIspInfo, i))
            {
                //strcpy(uIspInfo.ISPName, "12345");
                SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)uIspInfo.ISPName);
                if(pCreateData->SelConfig.GLConnect == (int)uIspInfo.ISPID)
                {
                    SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)pCreateData->hSelect);
                    j = i;
                }
                else
                {
                    SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)pCreateData->hUnSelect);
                }
            }
        }
        if((pCreateData->SelConfig.GLConnect > 0) && (j != -1))
        {
            SendMessage(hPublic, LB_SETCURSEL, j, 0);
        }
        else
        {
            SendMessage(hPublic, LB_SETCURSEL, 0, 0);
        }
    }

    //mailaddress
    else if(pCreateData->BoxID == IDC_MAIL_BOX_MAILADDR)
    {
        hPublic = CreateWindow(
            "IMEEDIT", 
            pCreateData->SelConfig.GLMailBoxName, 
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_TITLE | ES_AUTOHSCROLL,
            rcClient.left,
            rcClient.top, 
            rcClient.right - rcClient.left, 
            PARTWND_HEIGHT,
            hWnd, 
            (HMENU)IDC_MAIL_SELECT_PUBLIC, 
            NULL, 
            (PVOID)&ie);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, EM_LIMITTEXT, MAX_MAIL_MAILADDR_LEN, 0);
        SendMessage(hPublic, EM_SETTITLE, 0, (LPARAM)IDP_MAIL_TITLE_MAILADDRCOLON);
        if(stricmp(pCreateData->SelConfig.GLMailAddr, IDP_MAIL_VALUE_REQUIRED) == 0)
        {
            SetWindowText(hPublic, "");
        }
        else
        {
            SetWindowText(hPublic, pCreateData->SelConfig.GLMailAddr);
        }
    }

    //outserver
    else if(pCreateData->BoxID == IDC_MAIL_BOX_OUTSERVER)
    {
        hPublic = CreateWindow(
            "IMEEDIT", 
            pCreateData->SelConfig.GLMailBoxName, 
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_TITLE | ES_AUTOHSCROLL,
            rcClient.left, 
            rcClient.top, 
            rcClient.right - rcClient.left, 
            PARTWND_HEIGHT,
            hWnd, 
            (HMENU)IDC_MAIL_SELECT_PUBLIC, 
            NULL, 
            (PVOID)&ie);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, EM_LIMITTEXT, MAX_MAIL_OUTGOSERVER_LEN, 0);
        SendMessage(hPublic, EM_SETTITLE, 0, (LPARAM)IDP_MAIL_TITLE_OUTSERVERCOLON);
        if(stricmp(pCreateData->SelConfig.GLOutServer, IDP_MAIL_VALUE_REQUIRED) == 0)
        {
            SetWindowText(hPublic, "");
        }
        else
        {
            SetWindowText(hPublic, pCreateData->SelConfig.GLOutServer);
        }
    }

    //message sending
    else if(pCreateData->BoxID == IDC_MAIL_BOX_SENDMSG)
    {
        hPublic = CreateWindow(
            "LISTBOX",
            IDP_MAIL_TITLE_SENDMSG,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
            rcClient.left,
            rcClient.top,
            rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top,
            hWnd,
            (HMENU)IDC_MAIL_SELECT_PUBLIC,
            NULL,
            NULL);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_CONNECTAUTO);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_WHENCONNECT);
        
        SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pCreateData->hUnSelect);
        SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pCreateData->hUnSelect);
        SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, pCreateData->SelConfig.GLSendMsg), (LPARAM)pCreateData->hSelect);
        SendMessage(hPublic, LB_SETCURSEL, pCreateData->SelConfig.GLSendMsg, 0);
    }

    //CC to self
    else if(pCreateData->BoxID == IDC_MAIL_BOX_CC)
    {
        hPublic = CreateWindow(
            "LISTBOX",
            IDP_MAIL_TITLE_CCTOSELF,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
            rcClient.left,
            rcClient.top,
            rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top,
            hWnd,
            (HMENU)IDC_MAIL_SELECT_PUBLIC,
            NULL,
            NULL);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_BUTTON_YES);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_BUTTON_NO);
        
        if(pCreateData->SelConfig.GLCC)
        {
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pCreateData->hSelect);
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pCreateData->hUnSelect);
            SendMessage(hPublic, LB_SETCURSEL, 0, 0);
        }
        else
        {
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pCreateData->hUnSelect);
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pCreateData->hSelect);
            SendMessage(hPublic, LB_SETCURSEL, 1, 0);
        }
    }

    //signature
    else if(pCreateData->BoxID == IDC_MAIL_BOX_SIGNATURE)
    {
        hPublic = CreateWindow(
            "IMEEDIT", 
            pCreateData->SelConfig.GLMailBoxName, 
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_TITLE | ES_AUTOHSCROLL,
            rcClient.left, 
            rcClient.top, 
            rcClient.right - rcClient.left, 
            PARTWND_HEIGHT,
            hWnd, 
            (HMENU)IDC_MAIL_SELECT_PUBLIC, 
            NULL, 
            (PVOID)&ie);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, EM_LIMITTEXT, MAX_MAIL_SIGNATURE_LEN, 0);
        SendMessage(hPublic, EM_SETTITLE, 0, (LPARAM)IDP_MAIL_TITLE_SIGNATURECOLON);
        SetWindowText(hPublic, pCreateData->SelConfig.GLSignatrue);
    }

    //Login
    else if(pCreateData->BoxID == IDC_MAIL_BOX_LOGIN)
    {
        hPublic = CreateWindow(
            "IMEEDIT", 
            pCreateData->SelConfig.GLMailBoxName, 
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_TITLE | ES_AUTOHSCROLL,
            rcClient.left, 
            rcClient.top, 
            rcClient.right - rcClient.left, 
            PARTWND_HEIGHT,
            hWnd, 
            (HMENU)IDC_MAIL_SELECT_PUBLIC, 
            NULL, 
            (PVOID)&ie);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, EM_LIMITTEXT, MAX_MAIL_LOGIN_LEN, 0);
        SendMessage(hPublic, EM_SETTITLE, 0, (LPARAM)IDP_MAIL_TITLE_LOGINCOLON);
        SetWindowText(hPublic, pCreateData->SelConfig.GLLogin);
    }

    //password
    else if(pCreateData->BoxID == IDC_MAIL_BOX_PASSWORD)
    {
        hPublic = CreateWindow(
            "IMEEDIT", 
            pCreateData->SelConfig.GLMailBoxName, 
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_TITLE | ES_PASSWORD | ES_AUTOHSCROLL,
            rcClient.left, 
            rcClient.top, 
            rcClient.right - rcClient.left, 
            PARTWND_HEIGHT,
            hWnd, 
            (HMENU)IDC_MAIL_SELECT_PUBLIC, 
            NULL, 
            (PVOID)&ie);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, EM_LIMITTEXT, MAX_MAIL_PASSWORD_LEN, 0);
        SendMessage(hPublic, EM_SETTITLE, 0, (LPARAM)IDP_MAIL_TITLE_PASSWORDCOLON);
        SetWindowText(hPublic, pCreateData->SelConfig.GLPassword);
    }

    //Incoming mail server
    else if(pCreateData->BoxID == IDC_MAIL_BOX_INSERVER)
    {
        hPublic = CreateWindow(
            "IMEEDIT", 
            pCreateData->SelConfig.GLMailBoxName, 
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_TITLE | ES_AUTOHSCROLL,
            rcClient.left, 
            rcClient.top, 
            rcClient.right - rcClient.left, 
            PARTWND_HEIGHT,
            hWnd, 
            (HMENU)IDC_MAIL_SELECT_PUBLIC, 
            NULL, 
            (PVOID)&ie);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, EM_LIMITTEXT, MAX_MAIL_INCOMSERVER_LEN, 0);
        SendMessage(hPublic, EM_SETTITLE, 0, (LPARAM)IDP_MAIL_TITLE_INSERVERCOLON);
        if(stricmp(pCreateData->SelConfig.GLInServer, IDP_MAIL_VALUE_REQUIRED) == 0)
        {
            SetWindowText(hPublic, "");
        }
        else
        {
            SetWindowText(hPublic, pCreateData->SelConfig.GLInServer);
        }
    }

    //mailbox type
    else if(pCreateData->BoxID == IDC_MAIL_BOX_MAILTYPE)
    {
        hPublic = CreateWindow(
            "LISTBOX",
            IDP_MAIL_TITLE_MAILTYPE,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
            rcClient.left,
            rcClient.top,
            rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top,
            hWnd,
            (HMENU)IDC_MAIL_SELECT_PUBLIC,
            NULL,
            NULL);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_POP3);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_IMAP4);
        
        SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pCreateData->hUnSelect);
        SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pCreateData->hUnSelect);
        SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, pCreateData->SelConfig.GLBoxType), (LPARAM)pCreateData->hSelect);
        SendMessage(hPublic, LB_SETCURSEL, pCreateData->SelConfig.GLBoxType, 0);
    }

    //autoretrieval
    else if(pCreateData->BoxID == IDC_MAIL_BOX_AUTORET)
    {
        int i;

        hPublic = CreateWindow(
            "LISTBOX",
            IDP_MAIL_TITLE_AUTORET,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
            rcClient.left,
            rcClient.top,
            rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top,
            hWnd,
            (HMENU)IDC_MAIL_SELECT_PUBLIC,
            NULL,
            NULL);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_OFF);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_10MIN);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_30MIN);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_1H);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_2H);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_6H);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_12H);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_24H);
        
        for(i=0; i<8; i++)
        {
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)pCreateData->hUnSelect);
        }
        SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, pCreateData->SelConfig.GLAutoRetrieval), (LPARAM)pCreateData->hSelect);
        SendMessage(hPublic, LB_SETCURSEL, pCreateData->SelConfig.GLAutoRetrieval, 0);
    }

    //security
    else if(pCreateData->BoxID == IDC_MAIL_BOX_SECURITY)
    {        
        hPublic = CreateWindow(
            "LISTBOX",
            IDP_MAIL_TITLE_SECURITY,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
            rcClient.left,
            rcClient.top,
            rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top,
            hWnd,
            (HMENU)IDC_MAIL_SELECT_PUBLIC,
            NULL,
            NULL);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_SSL);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_OFF);
        
        if(pCreateData->SelConfig.GLSecurity)
        {
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pCreateData->hSelect);
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pCreateData->hUnSelect);
            SendMessage(hPublic, LB_SETCURSEL, 0, 0);
        }
        else
        {
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pCreateData->hUnSelect);
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pCreateData->hSelect);
            SendMessage(hPublic, LB_SETCURSEL, 1, 0);
        }
    }

#ifdef _NOKIA_MAIL_
    //secure login
    else if(pCreateData->BoxID == IDC_MAIL_BOX_SECURELOGIN)
    {        
        hPublic = CreateWindow(
            "LISTBOX",
            IDP_MAIL_TITLE_SECURELOGIN,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
            rcClient.left,
            rcClient.top,
            rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top,
            hWnd,
            (HMENU)IDC_MAIL_SELECT_PUBLIC,
            NULL,
            NULL);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_ON);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_OFF);
        
        if(pCreateData->SelConfig.GLSecureLogin)
        {
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pCreateData->hSelect);
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pCreateData->hUnSelect);
            SendMessage(hPublic, LB_SETCURSEL, 0, 0);
        }
        else
        {
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pCreateData->hUnSelect);
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pCreateData->hSelect);
            SendMessage(hPublic, LB_SETCURSEL, 1, 0);
        }
    }
#endif
    
    //retrieve att
    else if(pCreateData->BoxID == IDC_MAIL_BOX_RETRATT)
    {        
        hPublic = CreateWindow(
            "LISTBOX",
            IDP_MAIL_TITLE_RETRATT,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
            rcClient.left,
            rcClient.top,
            rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top,
            hWnd,
            (HMENU)IDC_MAIL_SELECT_PUBLIC,
            NULL,
            NULL);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_BUTTON_YES);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_BUTTON_NO);
        
        if(pCreateData->SelConfig.GLRetrAtt)
        {
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pCreateData->hSelect);
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pCreateData->hUnSelect);
            SendMessage(hPublic, LB_SETCURSEL, 0, 0);
        }
        else
        {
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pCreateData->hUnSelect);
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pCreateData->hSelect);
            SendMessage(hPublic, LB_SETCURSEL, 1, 0);
        }
    }

    //retrieve headers
    else if(pCreateData->BoxID == IDC_MAIL_BOX_RETRHEAD)
    {
        int i;
        
        hPublic = CreateWindow(
            "LISTBOX",
            IDP_MAIL_TITLE_RETRHEAD,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
            rcClient.left,
            rcClient.top,
            rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top,
            hWnd,
            (HMENU)IDC_MAIL_SELECT_PUBLIC,
            NULL,
            NULL);
        
        if(hPublic == NULL)
            return FALSE;
        
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)IDP_MAIL_VALUE_ALL);
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)("5"));
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)("10"));
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)("30"));
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)("50"));
        SendMessage(hPublic, LB_ADDSTRING, NULL, (LPARAM)("100"));
        
        for(i=0; i<6; i++)
        {
            SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)pCreateData->hUnSelect);
        }
        SendMessage(hPublic, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, pCreateData->SelConfig.GLRetrHead), (LPARAM)pCreateData->hSelect);
        SendMessage(hPublic, LB_SETCURSEL, pCreateData->SelConfig.GLRetrHead, 0);
    }

    SetFocus(hPublic);
    
    //mailname
    if((pCreateData->BoxID == IDC_MAIL_BOX_MAILNAME) || (pCreateData->BoxID == IDC_MAIL_BOX_MAILADDR)
        || (pCreateData->BoxID == IDC_MAIL_BOX_OUTSERVER) || (pCreateData->BoxID == IDC_MAIL_BOX_SIGNATURE)
        || (pCreateData->BoxID == IDC_MAIL_BOX_LOGIN) || (pCreateData->BoxID == IDC_MAIL_BOX_PASSWORD)
        || (pCreateData->BoxID == IDC_MAIL_BOX_INSERVER))
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_CANCEL);
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_SAVE);
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    }
    
    //message sending
    if((pCreateData->BoxID == IDC_MAIL_BOX_CONNECT) || (pCreateData->BoxID == IDC_MAIL_BOX_SENDMSG) 
        || (pCreateData->BoxID == IDC_MAIL_BOX_CC) || (pCreateData->BoxID == IDC_MAIL_BOX_MAILTYPE)
        || (pCreateData->BoxID == IDC_MAIL_BOX_AUTORET) || (pCreateData->BoxID == IDC_MAIL_BOX_SECURITY)
        || (pCreateData->BoxID == IDC_MAIL_BOX_SECURELOGIN) || (pCreateData->BoxID == IDC_MAIL_BOX_RETRATT)
        || (pCreateData->BoxID == IDC_MAIL_BOX_RETRHEAD))
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_CANCEL);
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    }

    return TRUE;
}

/*********************************************************************\
* Function	MailBoxSelect_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MailBoxSelect_OnActivate(HWND hWnd, UINT state)
{
    PMAIL_BOXLISTCREATEDATA pCreateData;
    HWND hPublic;

    pCreateData = GetUserData(hWnd);
    hPublic = GetDlgItem(hWnd, IDC_MAIL_SELECT_PUBLIC);

    /*if(state == WA_ACTIVE)
    {
        if(pCreateData->bCreate)
        {
            HWND tempFocus;

            pCreateData->bCreate = FALSE;
            tempFocus = GetDlgItem(hWnd, IDC_MAIL_SELECT_PUBLIC);
            SetFocus(tempFocus);
            return;
        }
    }*/
    
    if((pCreateData->BoxID == IDC_MAIL_BOX_MAILNAME) || (pCreateData->BoxID == IDC_MAIL_BOX_MAILADDR)
        || (pCreateData->BoxID == IDC_MAIL_BOX_OUTSERVER) || (pCreateData->BoxID == IDC_MAIL_BOX_SIGNATURE)
        || (pCreateData->BoxID == IDC_MAIL_BOX_LOGIN) || (pCreateData->BoxID == IDC_MAIL_BOX_PASSWORD)
        || (pCreateData->BoxID == IDC_MAIL_BOX_INSERVER))
    {
        if(pCreateData->bNew)
        {
            SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_NEWMAILBOX);
        }
        else
        {
            SetWindowText(pCreateData->hFrameWnd, pCreateData->SelConfig.GLMailBoxName);
        }
    }
    else if(pCreateData->BoxID == IDC_MAIL_BOX_CONNECT)
    {
        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_CONNECT);
    }
    else if(pCreateData->BoxID == IDC_MAIL_BOX_SENDMSG)
    {
        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_SENDMSG);
    }
    else if(pCreateData->BoxID == IDC_MAIL_BOX_CC)
    {
        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_CCTOSELF);
    }
    else if(pCreateData->BoxID == IDC_MAIL_BOX_MAILTYPE)
    {
        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_MAILTYPE);
    }
    else if(pCreateData->BoxID == IDC_MAIL_BOX_AUTORET)
    {
        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_AUTORET);
    }
    else if(pCreateData->BoxID == IDC_MAIL_BOX_SECURITY)
    {
        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_SECURITY);
    }
#ifdef _NOKIA_MAIL_
    else if(pCreateData->BoxID == IDC_MAIL_BOX_SECURELOGIN)
    {
        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_SECURELOGIN);
    }
#endif
    else if(pCreateData->BoxID == IDC_MAIL_BOX_RETRATT)
    {
        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_RETRATT);
    }
    else if(pCreateData->BoxID == IDC_MAIL_BOX_RETRHEAD)
    {
        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_RETRHEAD);
    }

    //mailname
    if((pCreateData->BoxID == IDC_MAIL_BOX_MAILNAME) || (pCreateData->BoxID == IDC_MAIL_BOX_MAILADDR)
        || (pCreateData->BoxID == IDC_MAIL_BOX_OUTSERVER) || (pCreateData->BoxID == IDC_MAIL_BOX_SIGNATURE)
        || (pCreateData->BoxID == IDC_MAIL_BOX_LOGIN) || (pCreateData->BoxID == IDC_MAIL_BOX_PASSWORD)
        || (pCreateData->BoxID == IDC_MAIL_BOX_INSERVER))
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_CANCEL);
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_SAVE);
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    }

    //message sending
    else if((pCreateData->BoxID == IDC_MAIL_BOX_CONNECT) || (pCreateData->BoxID == IDC_MAIL_BOX_SENDMSG) 
        || (pCreateData->BoxID == IDC_MAIL_BOX_CC) || (pCreateData->BoxID == IDC_MAIL_BOX_MAILTYPE)
        || (pCreateData->BoxID == IDC_MAIL_BOX_AUTORET) || (pCreateData->BoxID == IDC_MAIL_BOX_SECURITY)
        || (pCreateData->BoxID == IDC_MAIL_BOX_SECURELOGIN) || (pCreateData->BoxID == IDC_MAIL_BOX_RETRATT)
        || (pCreateData->BoxID == IDC_MAIL_BOX_RETRHEAD))
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_CANCEL);
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    }

    SetFocus(hPublic);

    //PDADefWindowProc(hWnd, WM_ACTIVATE, state, 0);
    
    return;
}

/*********************************************************************\
* Function	MailBoxSelect_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailBoxSelect_OnPaint(HWND hWnd)
{
	HDC hdc;
    
    hdc = BeginPaint(hWnd, NULL);
    EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	MailBoxSelect_OnKey
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
static void MailBoxSelect_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PMAIL_BOXLISTCREATEDATA pCreateData;
    HWND hPublic;
    int i;
    
    pCreateData = GetUserData(hWnd);
    hPublic = GetDlgItem(hWnd, IDC_MAIL_SELECT_PUBLIC);

	switch (vk)
	{
    case VK_F10:
        {
            PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), 0);
        }
		break;

    case VK_RETURN:
        {
            PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_SAVE, 0), 0);
        }
        break;

    case VK_F5:
        {
            if((pCreateData->BoxID == IDC_MAIL_BOX_SENDMSG) || (pCreateData->BoxID == IDC_MAIL_BOX_MAILTYPE)
                || (pCreateData->BoxID == IDC_MAIL_BOX_AUTORET) || (pCreateData->BoxID == IDC_MAIL_BOX_RETRHEAD))
            {
                i = SendMessage(hPublic, LB_GETCURSEL, 0, 0);
                SendMessage(pCreateData->hParent, WM_COMMAND, MAKEWPARAM(pCreateData->BoxID, ID_MAILBOX_SELECT_RET), (LPARAM)&i);
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
            else if((pCreateData->BoxID == IDC_MAIL_BOX_CC) || (pCreateData->BoxID == IDC_MAIL_BOX_SECURITY)
                || (pCreateData->BoxID == IDC_MAIL_BOX_SECURELOGIN) || (pCreateData->BoxID == IDC_MAIL_BOX_RETRATT))
            {
                int j;

                j = SendMessage(hPublic, LB_GETCURSEL, 0, 0);
                i = (j == 1) ? 0 : 1;
                SendMessage(pCreateData->hParent, WM_COMMAND, MAKEWPARAM(pCreateData->BoxID, ID_MAILBOX_SELECT_RET), (LPARAM)&i);
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
            else if(pCreateData->BoxID == IDC_MAIL_BOX_CONNECT)
            {
                UDB_ISPINFO uIspInfo;
                
                memset(&uIspInfo, 0x0, sizeof(UDB_ISPINFO));
                i = SendMessage(hPublic, LB_GETCURSEL, 0, 0);
                IspReadInfo(&uIspInfo, i);
                
                SendMessage(pCreateData->hParent, WM_COMMAND, MAKEWPARAM(pCreateData->BoxID, ID_MAILBOX_SELECT_RET), (LPARAM)&uIspInfo);
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
            else
            {
                //PDADefWindowProc(pCreateData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
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
* Function	MailBoxSelect_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MailBoxSelect_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    PMAIL_BOXLISTCREATEDATA pCreateData;
    HWND hPublic;

    pCreateData = GetUserData(hWnd);
    hPublic = GetDlgItem(hWnd, IDC_MAIL_SELECT_PUBLIC);

	switch(id)
	{
    case IDM_MAIL_BUTTON_SAVE:
        {
            int Num;
            char tmp[520];
            MailConfigNode *pTempNode;

            memset(tmp, 0x0, 520);
            if(pCreateData->BoxID == IDC_MAIL_BOX_MAILNAME)
            {
                Num = GetWindowTextLength(hPublic);
                GetWindowText(hPublic, tmp, Num + 1);
                if(tmp[0] == '\0')
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DEFINENAME, NULL, Notify_Alert, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                    break;
                }
                else
                {
                    pTempNode = NULL;
                    pTempNode = GlobalMailConfigHead.pNext;
                    while(pTempNode)
                    {
                        if(stricmp(pTempNode->GLMailBoxName, tmp) == 0)
                        {
                            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_ERRORNAME, NULL, Notify_Alert, 
                                IDP_MAIL_BOOTEN_OK, NULL, 20);
                            break;
                        }
                        pTempNode = pTempNode->pNext;
                    }
                    if(pTempNode == NULL)
                    {
                        SendMessage(pCreateData->hParent, WM_COMMAND, MAKEWPARAM(pCreateData->BoxID, ID_MAILBOX_SELECT_RET), (LPARAM)tmp);
                        PostMessage(hWnd, WM_CLOSE, 0, 0);
                    }
                }
            }
            else if(pCreateData->BoxID == IDC_MAIL_BOX_MAILADDR)
            {
                Num = GetWindowTextLength(hPublic);
                GetWindowText(hPublic, tmp, Num + 1);
                if(tmp[0] == '\0')
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DEFINEADDR, NULL, Notify_Alert, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else
                {
                    SendMessage(pCreateData->hParent, WM_COMMAND, MAKEWPARAM(pCreateData->BoxID, ID_MAILBOX_SELECT_RET), (LPARAM)tmp);
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
            }
            else if(pCreateData->BoxID == IDC_MAIL_BOX_OUTSERVER)
            {
                Num = GetWindowTextLength(hPublic);
                GetWindowText(hPublic, tmp, Num + 1);
                if(tmp[0] == '\0')
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DEFINEOUTGO, NULL, Notify_Alert, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else
                {
                    SendMessage(pCreateData->hParent, WM_COMMAND, MAKEWPARAM(pCreateData->BoxID, ID_MAILBOX_SELECT_RET), (LPARAM)tmp);
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
            }
            else if(pCreateData->BoxID == IDC_MAIL_BOX_LOGIN)
            {
                Num = GetWindowTextLength(hPublic);
                GetWindowText(hPublic, tmp, Num + 1);
                if(tmp[0] == '\0')
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DEFINELOGIN, NULL, Notify_Alert, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else
                {
                    SendMessage(pCreateData->hParent, WM_COMMAND, MAKEWPARAM(pCreateData->BoxID, ID_MAILBOX_SELECT_RET), (LPARAM)tmp);
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
            }
            else if(pCreateData->BoxID == IDC_MAIL_BOX_SIGNATURE)
            {
                Num = GetWindowTextLength(hPublic);
                GetWindowText(hPublic, tmp, Num + 1);
                /*if(tmp[0] == '\0')
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DEFINESIGN, NULL, Notify_Alert, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else*/
                {
                    SendMessage(pCreateData->hParent, WM_COMMAND, MAKEWPARAM(pCreateData->BoxID, ID_MAILBOX_SELECT_RET), (LPARAM)tmp);
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
            }
            else if(pCreateData->BoxID == IDC_MAIL_BOX_PASSWORD)
            {
                Num = GetWindowTextLength(hPublic);
                GetWindowText(hPublic, tmp, Num + 1);
                SendMessage(pCreateData->hParent, WM_COMMAND, MAKEWPARAM(pCreateData->BoxID, ID_MAILBOX_SELECT_RET), (LPARAM)tmp);
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
            else if(pCreateData->BoxID == IDC_MAIL_BOX_INSERVER)
            {
                Num = GetWindowTextLength(hPublic);
                GetWindowText(hPublic, tmp, Num + 1);
                if(tmp[0] == '\0')
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DEFINEINCOM, NULL, Notify_Alert, 
                        IDP_MAIL_BOOTEN_OK, NULL, 20);
                }
                else
                {
                    SendMessage(pCreateData->hParent, WM_COMMAND, MAKEWPARAM(pCreateData->BoxID, ID_MAILBOX_SELECT_RET), (LPARAM)tmp);
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
            }
        }
        break;

    case IDM_MAIL_BUTTON_EXIT:
        {
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;

    default:
        break;
	}
    
	return;
}

/*********************************************************************\
* Function	MailBoxSelect_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailBoxSelect_OnDestroy(HWND hWnd)
{
    PMAIL_BOXLISTCREATEDATA pCreateData;
    //HMENU hMenu;
    
    pCreateData = GetUserData(hWnd);
    
    //hMenu = PDAGetMenu(pCreateData->hFrameWnd);
    //DestroyMenu(hMenu);

    DeleteObject(pCreateData->hSelect);
    pCreateData->hSelect = NULL;
    DeleteObject(pCreateData->hSelect);
    pCreateData->hSelect = NULL;    

    //UnRegisterMailBoxSelectClass();

    return;
}

/*********************************************************************\
* Function	MailBoxSelect_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailBoxSelect_OnClose(HWND hWnd)
{
    DestroyWindow(hWnd);
    return;
}

static BOOL MAIL_JudgeActi(char *szName)
{
    MailConfig ActiCon;
    char szOldPath[PATH_MAXLEN];
    int hFile;

    szOldPath[0] = 0;
    getcwd(szOldPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    
    hFile = GARY_open(szName, O_RDONLY, -1);
    if(hFile < 0)
    {
        printf("\r\nMAIL_JudgeActi : open error = %d\r\n", errno);

        chdir(szOldPath);
        return FALSE;
    }
    memset(&ActiCon, 0x0, sizeof(MailConfig));
    if(read(hFile, &ActiCon, sizeof(MailConfig)) != sizeof(MailConfig))
    {
        GARY_close(hFile);
        chdir(szOldPath);
        return FALSE;
    }
    GARY_close(hFile);

    if(ActiCon.GLConnect == 0)
    {
        return FALSE;
    }
    if(strcmp(ActiCon.GLMailAddr, IDP_MAIL_VALUE_REQUIRED) == 0)
    {
        return FALSE;
    }
    if(strcmp(ActiCon.GLOutServer, IDP_MAIL_VALUE_REQUIRED) == 0)
    {
        return FALSE;
    }
    if(strcmp(ActiCon.GLInServer, IDP_MAIL_VALUE_REQUIRED) == 0)
    {
        return FALSE;
    }
    /*if(strlen(ActiCon.GLLogin) == 0)
    {
        return FALSE;
    }
    if(strlen(ActiCon.GLPassword) == 0)
    {
        return FALSE;
    }*/
    return TRUE;
}
