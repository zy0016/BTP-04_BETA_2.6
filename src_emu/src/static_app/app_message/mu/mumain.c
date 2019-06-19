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

#define WM_MSG_REFRESHMAILBOX   WM_USER+100
#define ML_KILLFOCUSNOTIFY      20

#define IDM_MU_CREATEMSG_SMS    1021 
#define IDM_MU_CREATEMSG_MMS    1022
#define IDM_MU_CREATEMSG_EMAIL  1023

#define IDM_MU_OPEN             101
#define IDM_MU_CREATEMSG        102
#define IDM_MU_SIM              103
#define IDM_MU_CB               104
#define IDM_MU_VOICEMAILBOX     105
#define IDM_MU_SETTING          106

#define IDC_EXIT                200

#define IDC_MU_MENULIST         300        
/**********************Create Message Menu Begin***************************/
static const MENUITEMTEMPLATE CreateMsgMenu[] = 
{
    { MF_STRING,    IDM_MU_CREATEMSG_SMS,      "",     NULL },
    { MF_STRING,    IDM_MU_CREATEMSG_MMS,      "",     NULL },
    { MF_STRING,    IDM_MU_CREATEMSG_EMAIL,    "",     NULL },
    { MF_END,       0,                         NULL,   NULL }
};

static const MENUTEMPLATE MUCreateMsgMenuTemplate = 
{
    0,
        (PMENUITEMTEMPLATE)CreateMsgMenu
};
/**********************Create Message Menu End***************************/

/**********************AB Main Menu Begin***************************/
static const MENUITEMTEMPLATE MUMainMenu[] = 
{
    { MF_STRING,            IDM_MU_OPEN,            "",     NULL },
    { MF_STRING|MF_POPUP,   IDM_MU_CREATEMSG,       "",     (PMENUITEMTEMPLATE)CreateMsgMenu },
    { MF_STRING,            IDM_MU_SIM,             "",     NULL },
    { MF_STRING,            IDM_MU_CB,              "",     NULL },
    { MF_STRING,            IDM_MU_VOICEMAILBOX,    "",     NULL },
    { MF_STRING,            IDM_MU_SETTING,         "",     NULL },
    { MF_END,               0,                      NULL,   NULL }
};

static const MENUTEMPLATE MUMainMenuTemplate = 
{
    0,
    (PMENUITEMTEMPLATE)MUMainMenu
};
/**********************AB Main Menu End***************************/

typedef struct tagMU_CreateData
{
    int         nFocus;
    HINSTANCE   hInstance;
	HMENU		hMenu;
}MU_CREATEDATA,*PMU_CREATEDATA;

/************************ Var Begin ************************/
static HWND hMainWnd = NULL;
static HWND hwndMenuList = NULL;
static HWND hFocus = NULL;
extern BOOL bDirectExit;

#define MU_MLFIXITEM_MAX   10
typedef enum{
IDC_MU_MAILBOX = 300,
IDC_MU_NEW = 301,		
IDC_MU_INBOX,
IDC_MU_MYFOLDERS,	
IDC_MU_DRAFT,
IDC_MU_SENT,
IDC_MU_OUTBOX,
IDC_MU_CB,
IDC_MU_SIM,
IDC_MU_DELIVERYREPORTS,
IDC_MU_TEMPLATES,
IDC_MU_MAX_DEFAULT,
};

const char* const mu_MenuListIcon[] =
{	
	"ROM:/message/unibox/mu_mail.ico",
	"/rom/message/unibox/mu_new.ico",
	"ROM:/message/unibox/mu_in.ico",		
	"ROM:/message/unibox/mu_myfolders.ico",		
	"ROM:/message/unibox/mu_draft.ico",		
	"ROM:/message/unibox/mu_sent.ico",			
	"ROM:/message/unibox/mu_out.ico",
    "ROM:/message/unibox/mu_cb.ico",
    "ROM:/message/unibox/mu_sim.ico",
	"ROM:/message/unibox/mu_reports.ico",
	"ROM:/message/unibox/mu_templates.ico",
};


static MENULISTTEMPLATE MyMenuList = 
{
    MLI_CENTER | MLI_NOTIFY ,
	0,
	NULL,
	0,
	240,
	29,//10,//20,
	15,
	0,
	0,
	10,
	10,
	240,
	5,
    NULL,
	240,
	280,
	10,
	10,
	NULL,
	0
};

static WNDPROC OldMLWndProc;
static MU_EMAILBOXINFO *pMU_EmailboxInfo = NULL;
static int nMailbox;
extern HINSTANCE   hMUInstance;


extern MU_INITSTATUS   mu_initstatus;
extern MU_IEmailbox    *mu_emailbox_interface;
extern MU_IMessage *mu_mms_interface;
extern MU_IMessage *mu_sms_interface;
extern MU_IMessage *mu_push_interface;
extern MU_IMessage *mu_email_interface;
extern MU_ISMSEx   *mu_smsex_interface;
extern MU_IMessage *mu_bt_interface;

/************************ Var End ************************/

/************************ Function Begin ************************/
HWND MU_CreateMainWindow(HINSTANCE hInstance);
LRESULT MUMainWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT CallMLWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MUMain_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MUMain_OnActivate(HWND hwnd, UINT state);
static void MUMain_OnInitmenu(HWND hwnd);
static void MUMain_OnPaint(HWND hWnd);
static void MUMain_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MUMain_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void MUMain_OnDestroy(HWND hWnd);
static void MUMain_OnClose(HWND hWnd);
static int  MU_MenuListRefresh(void);


void MU_Prompt(int nErrno);

extern BOOL MU_CreateList(HWND hParent,void* pFolderTypeOrName,BOOL bEmailBox);
extern BOOL MU_CreateFolderList(HWND hParent);
extern BOOL APP_EditPhrase(HWND hFrame);
extern int	GetTemplateNum();

#define IDC_OK              100
#define IDC_CANCEL          200
#define IDC_MUNEWSEL_LIST   300
#define MU_NEWSEL_POSITION  0,100,176,120

static BOOL MU_NewSelectWnd(HWND hParent);
static LRESULT MUNewSelectWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MUNewSelect_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MUNewSelect_OnActivate(HWND hwnd, UINT state);
static void MUNewSelect_OnPaint(HWND hWnd);
static void MUNewSelect_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MUNewSelect_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void MUNewSelect_OnDestroy(HWND hWnd);
static void MUNewSelect_OnClose(HWND hWnd);
/************************ Function End ************************/

HWND MU_CreateMainWindow(HINSTANCE hInstance)
{
    WNDCLASS    wc;
    HMENU       hMenu;
    int         menupos = 0;
    MU_CREATEDATA CreateData;
	RECT		rClient;

    memset(&CreateData,0,sizeof(MU_CREATEDATA));
    CreateData.hInstance = hInstance;
    
    wc.style         = 0;
    wc.lpfnWndProc   = MUMainWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MU_CREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "MUMainWndClass";
    
    if (!RegisterClass(&wc))
        return NULL;
    
    hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MUCreateMsgMenuTemplate);
    CreateData.hMenu = hMenu;

	hMuFrame = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);
	GetClientRect(hMuFrame, &rClient);
	
    hMainWnd = CreateWindow(
        "MUMainWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hMuFrame,
        NULL, 
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!hMainWnd)
        return NULL;
	
    SetWindowText(hMuFrame, IDS_MESSAGING);

    ModifyMenu(hMenu, IDM_MU_CREATEMSG_SMS, MF_BYCOMMAND, IDM_MU_CREATEMSG_SMS, IDS_SMS);
    menupos++;
    ModifyMenu(hMenu, IDM_MU_CREATEMSG_MMS, MF_BYCOMMAND, IDM_MU_CREATEMSG_MMS, IDS_MMS);
    menupos++;
    ModifyMenu(hMenu, IDM_MU_CREATEMSG_EMAIL, MF_BYCOMMAND, IDM_MU_CREATEMSG_EMAIL, IDS_EMAIL);
    
    msg_registerrefreshmailbox(hMainWnd,WM_MSG_REFRESHMAILBOX);
    
	PDASetMenu(hMuFrame, hMenu);
    SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS); 
    
    return hMuFrame;
}
/*********************************************************************\
* Function	MUMainWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT MUMainWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MUMain_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        MUMain_OnActivate(hWnd,(UINT)LOWORD(wParam));
		{
			MU_CREATEDATA *pCreateData;
			HWND hMenuLst;
			int nSel;
			
			pCreateData = (MU_CREATEDATA*)GetUserData(hWnd);

			SetWindowText(hMuFrame, IDS_MESSAGING);

			PDASetMenu(hMuFrame, pCreateData->hMenu);

			hMenuLst = GetDlgItem(hWnd,IDC_MU_MENULIST);
			nSel = SendMessage(hMenuLst,ML_GETCURSEL,0,0);

			SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);

			if(nSel == IDC_MU_NEW)
			{
				SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SMS);
				SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS); 
			}
			else
			{
				SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 
			}
		}
		
        break;
        
    case WM_INITMENU:
        MUMain_OnInitmenu(hWnd);
        break;

    case WM_PAINT:
        MUMain_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        MUMain_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
		{
			if(wParam == (WPARAM)GetMenu(hMuFrame))
			{
				//WM_INITMENU
				MUMain_OnInitmenu(hWnd);
				break;
			}
		}
        MUMain_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        MUMain_OnClose(hWnd);
        break;

    case WM_DESTROY:
		{
			MU_CREATEDATA* pData;

			pData = (MU_CREATEDATA*)GetUserData(hWnd);

			DestroyMenu(pData->hMenu);

		}
		
		//DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)(pCreateData->hInstance));
		
        MUMain_OnDestroy(hWnd);
//		EndObjectDebug();
		hMuFrame = NULL;
        break;

    case WM_MSG_REFRESHMAILBOX:
        MU_MenuListRefresh();
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	MUMain_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MUMain_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    POINT npoint;
    SIZE  nsize;
    HMENULIST hMenuList;
    RECT rect;
    int  nErrno;
    MU_CREATEDATA *pCreateData;

    pCreateData = (MU_CREATEDATA*)GetUserData(hWnd);

    memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(MU_CREATEDATA));

    pCreateData->nFocus = IDC_MU_NEW;
    
    hMenuList = LoadMenuListIndirect((const MENULISTTEMPLATE *)&MyMenuList);
    
    GetClientRect(hWnd,&rect);
    
    hwndMenuList = CreateWindow(
        "MENULIST", 
        "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL,
        0,
        0,
        rect.right-rect.left,
        rect.bottom-rect.top, 
        hWnd, 
        (HMENU)IDC_MU_MENULIST, 
        NULL, 
        (PVOID)hMenuList);
    
    if(hwndMenuList == NULL)
        return FALSE;
        
    nsize.cx = 176;
    nsize.cy = 29;
    SendMessage(hwndMenuList, ML_SETITEMSIZE, 0, (LPARAM)&nsize);
    
    npoint.x = 0;
    npoint.y = 0;
    SendMessage(hwndMenuList, ML_SETITEMSTARTPOS, 0, (LPARAM)&npoint);
    
    OldMLWndProc = (WNDPROC)SetWindowLong(hwndMenuList,GWL_WNDPROC,
        (LONG)CallMLWndProc);

    nErrno = MU_MenuListRefresh();

    MU_Prompt(nErrno);
    
    SendMessage(hMuFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_SMS);

    SendMessage(hMuFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT, 0), (LPARAM)IDS_EXIT);
    
//    SendMessage(hMuFrame, PWM_SETAPPICON, 0, (LPARAM)ICON_MU_MAIN);
    
    SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLUP|SCROLLDOWN, MASKALL);//?????
    
    hFocus = hwndMenuList;
    
	SetFocus(hFocus);
    return TRUE;
    
}
/*********************************************************************\
* Function	MUMain_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MUMain_OnActivate(HWND hwnd, UINT state)
{
/*
        if(state == WA_ACTIVE)
        { */
    
	HWND hMenuLst;
	int  nFolder;
	char *pszTitle;
	int  nUnread,nCount,nUnreadSum = 0,nCountSum = 0;
	char szDiaplay [100];
	MU_CREATEDATA *pCreateData;
	
	pCreateData = (MU_CREATEDATA*)GetUserData(hwnd);
	hMenuLst = GetDlgItem(hwnd,IDC_MU_MENULIST);
	
	SetFocus(hMenuLst);
	
	switch(pCreateData->nFocus)
	{
	case IDC_MU_INBOX:
		nFolder = MU_INBOX;
		pszTitle = (char*)IDS_INBOX;
		break;
		
	case IDC_MU_OUTBOX:
		nFolder = MU_OUTBOX;
		pszTitle = (char*)IDS_OUTBOX;
		break;
		
	case IDC_MU_SENT:
		nFolder = MU_SENT;
		pszTitle = (char*)IDS_SENT;
		break;
		
	case IDC_MU_DRAFT:
		nFolder = MU_DRAFT;
		pszTitle = (char*)IDS_DRAFT;
		break;
		
	case IDC_MU_DELIVERYREPORTS:
		nFolder = MU_REPORT;
		pszTitle = (char*)IDS_DELIVERYREPORT;
		break;
		
	case IDC_MU_MYFOLDERS:
		MU_GetFolderInfo(NULL,&nCount);            
		sprintf(szDiaplay,"%s -t %d",IDS_MYFOLDER,nCount);
		SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDiaplay);
		return;
		
	case IDC_MU_SIM:  //???????????
		if(mu_initstatus.bSMS)
		{
			nCount = mu_smsex_interface->sms_get_simcounter();
		}

		sprintf(szDiaplay,"%s -t %d",IDS_SIMMSG,nCount);
		SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDiaplay);
		return;
		
	case IDC_MU_CB:  //waiting
		if(mu_initstatus.bSMS)
		{
			nCount = mu_smsex_interface->sms_get_cbcounter();
		}
		sprintf(szDiaplay,"%s -t %d",IDS_CB,nCount);
		SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDiaplay);
		return;
		
	case IDC_MU_TEMPLATES:
		//nCount = MU_GetTemplatesCount();    
		nCount = GetTemplateNum();
		sprintf(szDiaplay,"%s -t %d",IDS_TEMPLATES,nCount);
		SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDiaplay);
		return;
		
	default:
        if(pCreateData->nFocus >= IDC_MU_MAX_DEFAULT)
        {
			if(mu_initstatus.bEmail)
            {
				mu_emailbox_interface->email_get_count(&pMU_EmailboxInfo[pCreateData->nFocus-IDC_MU_TEMPLATES-1], &nUnread, &nCount);
                sprintf(szDiaplay,"%s -t %d",pMU_EmailboxInfo[pCreateData->nFocus-IDC_MU_TEMPLATES-1].szEmailBoxName, nCount);
                SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDiaplay);
            }
        }
		return;
	}

	if(mu_initstatus.bSMS)
	{
        nUnread = 0;
        nCount = 0;
		mu_sms_interface->msg_get_count(nFolder,&nUnread,&nCount);
		printf("mu_sms_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
		nUnreadSum += nUnread;
		nCountSum += nCount;
	}
	if(mu_initstatus.bMMS)
	{
        nUnread = 0;
        nCount = 0;
		mu_mms_interface->msg_get_count(nFolder,&nUnread,&nCount);
		printf("mu_mms_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
		nUnreadSum += nUnread;
		nCountSum += nCount;
	}
	if(mu_initstatus.bEmail)
	{
        nUnread = 0;
        nCount = 0;
		mu_email_interface->msg_get_count(nFolder,&nUnread,&nCount);
		printf("mu_email_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
		nUnreadSum += nUnread;
		nCountSum += nCount;
	}
	if(mu_initstatus.bPush)
	{
        nUnread = 0;
        nCount = 0;
		mu_push_interface->msg_get_count(nFolder,&nUnread,&nCount);
		printf("mu_push_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
		nUnreadSum += nUnread;
		nCountSum += nCount;
	}
	if(mu_initstatus.bBT)
	{
        nUnread = 0;
        nCount = 0;
		mu_bt_interface->msg_get_count(nFolder, &nUnread, &nCount);
		printf("mu_bt_interface->msg_get_count nUnread = %d nCount = %d", nUnread, nCount);
		nUnread += nUnread;
		nCountSum += nCount;
	}
	sprintf(szDiaplay,"%s -t %d",pszTitle,nCountSum);
	SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDiaplay);
        
/*
            }
            else if(state == WA_INACTIVE)
                hFocus = GetFocus();*/
        

    return;
}
/*********************************************************************\
* Function	MUMain_OnInitmenu
* Purpose   WM_INITMENU message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUMain_OnInitmenu(HWND hwnd)
{
#ifdef _TEST_
    
    /*
    HMENU hMainMenu,hSubMenu;
    
        hMainMenu = PDAGetMenu(hwnd);
        hSubMenu = GetSubMenu(hMainMenu,1);
    
        if(mu_initstatus.bEmail == FALSE)
            EnableMenuItem(hSubMenu,IDM_MU_CREATEMSG_EMAIL,MF_GRAYED);
        
        if(mu_initstatus.bSMS == FALSE)
        {
            EnableMenuItem(hSubMenu,IDM_MU_CREATEMSG_SMS,MF_GRAYED);
            EnableMenuItem(hMainMenu,IDM_MU_CB,MF_GRAYED);
            EnableMenuItem(hMainMenu,IDM_MU_SIM,MF_GRAYED);
            EnableMenuItem(hMainMenu,IDM_MU_VOICEMAILBOX,MF_GRAYED);
        }
        
        if(mu_initstatus.bMMS == FALSE)
            EnableMenuItem(hSubMenu,IDM_MU_CREATEMSG_MMS,MF_GRAYED);*/
    
#endif
    
    return;

}
/*********************************************************************\
* Function	MUMain_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUMain_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	MUMain_OnKey
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
static void MUMain_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    HWND hMenuLst;
    int  nSel;
    
    switch (vk)
    {
    case VK_F5:
        hMenuLst = GetDlgItem(hWnd,IDC_MU_MENULIST);
        nSel = SendMessage(hMenuLst,ML_GETCURSEL,0,0);
        if(nSel == IDC_MU_NEW)
            PDADefWindowProc(hMuFrame, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        else
            SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(nSel,ML_SELNOTIFY),NULL);
        break;
        
    case VK_F10:
        PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

    case VK_RETURN:
        hMenuLst = GetDlgItem(hWnd,IDC_MU_MENULIST);
        nSel = SendMessage(hMenuLst,ML_GETCURSEL,0,0);
        if(nSel == IDC_MU_NEW)
            SendMessage(hWnd,WM_COMMAND,IDM_MU_CREATEMSG_SMS,NULL);
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	MUMain_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MUMain_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    int n;
    int  nUnread,nCount,nUnreadSum = 0,nCountSum = 0;
    HWND hMenuLst;
    char szDiaplay[100],szBtnText[50];
    MU_CREATEDATA *pCreateData;
        
    pCreateData = (MU_CREATEDATA*)GetUserData(hWnd);
    hMenuLst = GetDlgItem(hWnd,IDC_MU_MENULIST);

	switch(id)
	{	
	case IDC_MU_NEW:
        switch(codeNotify) 
        {
        case ML_SELNOTIFY:
//            MU_NewSelectWnd(hWnd);
        	break;

        case ML_FOCUSNOTIFY:
            SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(pCreateData->nFocus,ML_KILLFOCUSNOTIFY),NULL);
            pCreateData->nFocus = id;
            szBtnText[0] = 0;
            SendMessage(hMuFrame,PWM_GETBUTTONTEXT,1,(LPARAM)szBtnText);
            if(strcmp(IDS_SMS,szBtnText) != 0)
                SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SMS);
            SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS); 
        	break;

        case ML_KILLFOCUSNOTIFY:
            SendMessage(hMuFrame,PWM_GETBUTTONTEXT,1,(LPARAM)szBtnText);
            if(strcmp(IDS_SMS,szBtnText) == 0)
                SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            break;

        default:
            break;
        }
		break;
		
	case IDC_MU_INBOX:
        switch(codeNotify) 
        {
        case ML_SELNOTIFY:
            n = MU_INBOX;
			MU_CreateList(hWnd,&n,FALSE);
        	break;

        case ML_FOCUSNOTIFY:
            SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(pCreateData->nFocus,ML_KILLFOCUSNOTIFY),NULL);
            pCreateData->nFocus = id;
            if(mu_initstatus.bSMS)
            {
                nUnread = 0;
                nCount = 0;
                mu_sms_interface->msg_get_count(MU_INBOX,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bMMS)
            {
                nUnread = 0;
                nCount = 0;
                mu_mms_interface->msg_get_count(MU_INBOX,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bEmail)
            {
                nUnread = 0;
                nCount = 0;
                mu_email_interface->msg_get_count(MU_INBOX,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bPush)
            {
                nUnread = 0;
                nCount = 0;
                mu_push_interface->msg_get_count(MU_INBOX,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
			if(mu_initstatus.bBT)
            {
                nUnread = 0;
                nCount = 0;
                mu_bt_interface->msg_get_count(MU_INBOX,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }

            sprintf(szDiaplay,"%s -t %d",IDS_INBOX,nCountSum);
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)szDiaplay);
            
            SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 
        	break;

        case ML_KILLFOCUSNOTIFY:
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)IDS_INBOX);
            break;

        default:
            break;
        }
		break;

    case IDC_MU_MYFOLDERS:
        switch(codeNotify) 
        {
        case ML_SELNOTIFY:
            MU_CreateFolderList(hWnd);
        	break;

        case ML_FOCUSNOTIFY:
            SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(pCreateData->nFocus,ML_KILLFOCUSNOTIFY),NULL);
            pCreateData->nFocus = id;
            
            MU_GetFolderInfo(NULL,&nCount);
            nCountSum += nCount;

            sprintf(szDiaplay,"%s -t %d",IDS_MYFOLDER,nCountSum);
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)szDiaplay);
            SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 
        	break;

        case ML_KILLFOCUSNOTIFY:
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)IDS_MYFOLDER);
            break;

        default:
            break;
        }

        break;

	case IDC_MU_DRAFT:
        
        switch(codeNotify) 
        {
        case ML_SELNOTIFY:
            n = MU_DRAFT;
            MU_CreateList(hWnd,&n,FALSE);
        	break;

        case ML_FOCUSNOTIFY:
            SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(pCreateData->nFocus,ML_KILLFOCUSNOTIFY),NULL);
            pCreateData->nFocus = id;
            if(mu_initstatus.bSMS)
            {
                nUnread = 0;
                nCount = 0;
                mu_sms_interface->msg_get_count(MU_DRAFT,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bMMS)
            {
                nUnread = 0;
                nCount = 0;
                mu_mms_interface->msg_get_count(MU_DRAFT,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bEmail)
            {
                nUnread = 0;
                nCount = 0;
                mu_email_interface->msg_get_count(MU_DRAFT,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
/*
                        if(mu_initstatus.bPush)
                        {
                            mu_push_interface->msg_get_count(MU_DRAFT,&nUnread,&nCount);
                            nUnreadSum += nUnread;
                            nCountSum += nCount;
                        }*/
            
            sprintf(szDiaplay,"%s -t %d",IDS_DRAFT,nCountSum);
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)szDiaplay);
            SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 
        	break;

        case ML_KILLFOCUSNOTIFY:
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)IDS_DRAFT);
            break;

        default:
            break;
        }
		break;	

	case IDC_MU_SENT:
        switch(codeNotify) 
        {
        case ML_SELNOTIFY:
            n = MU_SENT;
            MU_CreateList(hWnd,&n,FALSE);
        	break;

        case ML_FOCUSNOTIFY:
            SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(pCreateData->nFocus,ML_KILLFOCUSNOTIFY),NULL);
            pCreateData->nFocus = id;
            if(mu_initstatus.bSMS)
            {
                nUnread = 0;
                nCount = 0;
                mu_sms_interface->msg_get_count(MU_SENT,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bMMS)
            {
                nUnread = 0;
                nCount = 0;
                mu_mms_interface->msg_get_count(MU_SENT,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bEmail)
            {
                nUnread = 0;
                nCount = 0;
                mu_email_interface->msg_get_count(MU_SENT,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
/*
                        if(mu_initstatus.bPush)
                        {
                            mu_push_interface->msg_get_count(MU_SENT,&nUnread,&nCount);
                            nUnreadSum += nUnread;
                            nCountSum += nCount;
                        }*/
            
            sprintf(szDiaplay,"%s -t %d",IDS_SENT,nCountSum);
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)szDiaplay);
            SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 
        	break;

        case ML_KILLFOCUSNOTIFY:
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)IDS_SENT);
            break;

        default:
            break;
        }
		break;

	case IDC_MU_OUTBOX:
        switch(codeNotify) 
        {
        case ML_SELNOTIFY:
            n = MU_OUTBOX;
            MU_CreateList(hWnd,&n,FALSE);
        	break;

        case ML_FOCUSNOTIFY:
            SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(pCreateData->nFocus,ML_KILLFOCUSNOTIFY),NULL);
            pCreateData->nFocus = id;
            if(mu_initstatus.bSMS)
            {
                nUnread = 0;
                nCount = 0;
                mu_sms_interface->msg_get_count(MU_OUTBOX,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bMMS)
            {
                nUnread = 0;
                nCount = 0;
                mu_mms_interface->msg_get_count(MU_OUTBOX,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bEmail)
            {
                nUnread = 0;
                nCount = 0;
                mu_email_interface->msg_get_count(MU_OUTBOX,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
/*
                        if(mu_initstatus.bPush)
                        {
                            mu_push_interface->msg_get_count(MU_OUTBOX,&nUnread,&nCount);
                            nUnreadSum += nUnread;
                            nCountSum += nCount;
                        }*/
            
            sprintf(szDiaplay,"%s -t %d",IDS_OUTBOX,nCountSum);
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)szDiaplay);
            SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 
        	break;

        case ML_KILLFOCUSNOTIFY:
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)IDS_OUTBOX);
            break;

        default:
            break;
        }
        break;

    case IDC_MU_CB:
        switch(codeNotify) 
        {
        case ML_SELNOTIFY:
            if(mu_initstatus.bSMS)
            {
                mu_smsex_interface->sms_cb_create(hWnd);
            }
            break;
            
        case ML_FOCUSNOTIFY:
            SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(pCreateData->nFocus,ML_KILLFOCUSNOTIFY),NULL);
            pCreateData->nFocus = id;
            
            if(mu_initstatus.bSMS)
            {
                nCount = mu_smsex_interface->sms_get_cbcounter();
            }
            sprintf(szDiaplay,"%s -t %d",IDS_CB,nCount);
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDiaplay);
            SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 
            break;
            
        case ML_KILLFOCUSNOTIFY:
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)IDS_CB);
            break;
            
        default:
            break;
        }
        break;

    case IDC_MU_SIM:
        switch(codeNotify) 
        {
        case ML_SELNOTIFY:
            mu_smsex_interface->sms_sim_create(hWnd);
            break;
            
        case ML_FOCUSNOTIFY:
            SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(pCreateData->nFocus,ML_KILLFOCUSNOTIFY),NULL);
            pCreateData->nFocus = id;
            if(mu_initstatus.bSMS)
            {
                nCount = mu_smsex_interface->sms_get_simcounter();
            }
            sprintf(szDiaplay,"%s -t %d",IDS_SIMMSG,nCount);
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDiaplay);
            SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 
            break;
            
        case ML_KILLFOCUSNOTIFY:
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)IDS_SIMMSG);
            break;
            
        default:
            break;
        }
        break;
        
	case IDC_MU_DELIVERYREPORTS:
        switch(codeNotify) 
        {
        case ML_SELNOTIFY:
            n = MU_REPORT;
            MU_CreateList(hWnd,&n,FALSE);
        	break;

        case ML_FOCUSNOTIFY:
            SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(pCreateData->nFocus,ML_KILLFOCUSNOTIFY),NULL);
            pCreateData->nFocus = id;
            if(mu_initstatus.bSMS)
            {
                nUnread = 0;
                nCount = 0;
                mu_sms_interface->msg_get_count(MU_REPORT,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            if(mu_initstatus.bMMS)
            {
                nUnread = 0;
                nCount = 0;
                mu_mms_interface->msg_get_count(MU_REPORT,&nUnread,&nCount);
                nUnreadSum += nUnread;
                nCountSum += nCount;
            }
            sprintf(szDiaplay,"%s -t %d",IDS_DELIVERYREPORT,nCountSum);
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)szDiaplay);
            SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 
        	break;

        case ML_KILLFOCUSNOTIFY:
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)IDS_DELIVERYREPORT);
            break;

        default:
            break;
        }
		break;

    case IDC_MU_TEMPLATES:
        switch(codeNotify) 
        {
        case ML_SELNOTIFY:
            //MU_CreateTemplates(hWnd);
			APP_EditPhrase(hMuFrame);
        	break;

        case ML_FOCUSNOTIFY:
            SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(pCreateData->nFocus,ML_KILLFOCUSNOTIFY),NULL);
            pCreateData->nFocus = id;

            //nCountSum = MU_GetTemplatesCount();
            nCountSum = GetTemplateNum();
			
            sprintf(szDiaplay,"%s -t %d",IDS_TEMPLATES,nCountSum);
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)szDiaplay);
            SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 
        	break;

        case ML_KILLFOCUSNOTIFY:
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)IDS_TEMPLATES);
            break;

        default:
            break;
        }
        break;

    case IDM_MU_CREATEMSG_SMS:
        if(mu_initstatus.bSMS)
        {
            mu_sms_interface->msg_new_message(hWnd);
        }
        break;
        
    case IDM_MU_CREATEMSG_MMS:
//        SMS_PleaseToWait();
        if(mu_initstatus.bMMS)
        {
            mu_mms_interface->msg_new_message(hWnd);
        }
        break;
        
    case IDM_MU_CREATEMSG_EMAIL:
//        SMS_PleaseToWait();
        if(mu_initstatus.bEmail)
        {
            mu_email_interface->msg_new_message(hWnd);
        }
        break;

    case IDC_EXIT:
        PostMessage(hWnd,WM_CLOSE,0,0);
        break;

    default:
        switch(codeNotify) 
        {
        case ML_SELNOTIFY:
			if(mu_initstatus.bEmail)
			{
				mu_emailbox_interface->email_read(hWnd, &pMU_EmailboxInfo[id-IDC_MU_TEMPLATES-1]);
			}
        	break;

        case ML_FOCUSNOTIFY:
            SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(pCreateData->nFocus,ML_KILLFOCUSNOTIFY),NULL);
            pCreateData->nFocus = id;
            if(mu_initstatus.bEmail)
            {
                nUnread = 0;
                nCount = 0;
//                mu_email_interface->msg_get_count(SendMessage(hMenuLst,ML_GETITEMDATA,id,0),&nUnread,&nCount);
				mu_emailbox_interface->email_get_count(&pMU_EmailboxInfo[id-IDC_MU_TEMPLATES-1], &nUnread, &nCount);
                sprintf(szDiaplay,"%s -t %d",pMU_EmailboxInfo[id-IDC_MU_TEMPLATES-1].szEmailBoxName, nCount);
                SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)szDiaplay);
            }
            SendMessage(hMuFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT); 
        	break;

        case ML_KILLFOCUSNOTIFY:
            SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)id,(LPARAM)pMU_EmailboxInfo[id-IDC_MU_TEMPLATES-1].szEmailBoxName);
            break;

        default:
            break;
        }
        break;
	}

	return;
}

/*********************************************************************\
* Function	MUMain_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUMain_OnDestroy(HWND hWnd)
{

    hMainWnd = NULL;

    if(pMU_EmailboxInfo)
    {
        free(pMU_EmailboxInfo);
        pMU_EmailboxInfo = NULL;
    }

	UnregisterClass("MUMainWndClass",NULL);

    return;

}
/*********************************************************************\
* Function	MUMain_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUMain_OnClose(HWND hWnd)
{
    
    MU_CREATEDATA *pCreateData;
    
    pCreateData = (MU_CREATEDATA*)GetUserData(hWnd);

	SendMessage(hMuFrame,PWM_CLOSEWINDOW,0,0);

	DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)(pCreateData->hInstance));

    DestroyWindow (hWnd);

    return;

}

static int MU_MenuListRefresh(void)
{
    int     i;
    int     nMenuTemplateSize ;
    char*	szItemCaption[MU_MLFIXITEM_MAX];
    MENULISTITEMTEMPLATE tmpValue;

    SendMessage(hwndMenuList,ML_RESETCONTENT,0,0); 
    
    nMenuTemplateSize = sizeof(MENULISTITEMTEMPLATE);
    
    szItemCaption[0] = (char*)IDS_NEW;
    szItemCaption[1] = (char*)IDS_INBOX;
    szItemCaption[2] = (char*)IDS_MYFOLDER;	
    szItemCaption[3] = (char*)IDS_DRAFT;	
    szItemCaption[4] = (char*)IDS_SENT;
    szItemCaption[5] = (char*)IDS_OUTBOX;	
    szItemCaption[6] = (char*)IDS_CELLBROADCAST;
    szItemCaption[7] = (char*)IDS_SIMMSG;
    szItemCaption[8] = (char*)IDS_DELIVERYREPORT;
    szItemCaption[9] = (char*)IDS_TEMPLATES;
    
    if(mu_initstatus.bEmail)
    {
        if(pMU_EmailboxInfo != NULL)
            free(pMU_EmailboxInfo);
        pMU_EmailboxInfo = NULL;
        if(!mu_emailbox_interface->email_get_mailboxinfo(pMU_EmailboxInfo,&nMailbox))
            return MSG_ERR_EMAIL;
        
        if(nMailbox != 0)
		{
            pMU_EmailboxInfo = (MU_EMAILBOXINFO*)malloc(nMailbox * sizeof(MU_EMAILBOXINFO));
			if(pMU_EmailboxInfo == NULL)
				return MSG_ERR_LOWMEMORY;
			
			memset(pMU_EmailboxInfo,0,nMailbox * sizeof(MU_EMAILBOXINFO));
			if(!mu_emailbox_interface->email_get_mailboxinfo(pMU_EmailboxInfo,&nMailbox))
			{
				free(pMU_EmailboxInfo);
				pMU_EmailboxInfo = NULL;
				return MSG_ERR_EMAIL;
			}
		}
    }
    else
        nMailbox = 0;
    
/*
#ifndef _TEST_SMS_
    nMailbox = 3;
    pMU_EmailboxInfo = (MU_EMAILBOXINFO*)malloc(nMailbox * sizeof(MU_EMAILBOXINFO));
    if(pMU_EmailboxInfo == NULL)
        return MSG_ERR_LOWMEMORY;
    memset(pMU_EmailboxInfo,0,nMailbox * sizeof(MU_EMAILBOXINFO));
    strcpy(pMU_EmailboxInfo[0].szEmailBoxName,"xge");
    strcpy(pMU_EmailboxInfo[1].szEmailBoxName,"abcdefjhigklmnopqrstu");
    strcpy(pMU_EmailboxInfo[2].szEmailBoxName,"A3");
#endif*/

    
    /* new/inbox/My folder */
    for(i = IDC_MU_NEW; i <= IDC_MU_OUTBOX; i++)
    {
        memset(&tmpValue, 0x0, nMenuTemplateSize);
        tmpValue.dwFlags = MLI_LEFT | MLI_NOTIFY;
        tmpValue.wID  = i;
        tmpValue.lpszItemName = szItemCaption[i-IDC_MU_NEW];
        tmpValue.nPicAreaWidth = 20;
        tmpValue.lpszIconName = mu_MenuListIcon[i-IDC_MU_MAILBOX];
        SendMessage(hwndMenuList, ML_ADDITEM, 0, (LPARAM)&tmpValue);
    }  
	
	/* emailbox */
    for(i = 0; i < nMailbox; i++)
    {
        memset(&tmpValue, 0x0, nMenuTemplateSize);
        tmpValue.dwFlags = MLI_LEFT | MLI_NOTIFY;
        tmpValue.wID  = IDC_MU_TEMPLATES+i+1;
        tmpValue.lpszItemName = pMU_EmailboxInfo[i].szEmailBoxName;
        tmpValue.nPicAreaWidth = 20;
        tmpValue.lpszIconName = mu_MenuListIcon[0];//need modify
        SendMessage(hwndMenuList, ML_ADDITEM, 0, (LPARAM)&tmpValue);
    }   
	
    /* draft/sent/outbox/delivery reports */
    for(i = IDC_MU_CB; i <= IDC_MU_TEMPLATES; i++)
    {
        memset(&tmpValue, 0x0, nMenuTemplateSize);
        tmpValue.dwFlags = MLI_LEFT | MLI_NOTIFY;
        tmpValue.wID  = i;
        tmpValue.lpszItemName = szItemCaption[i-IDC_MU_NEW];
        tmpValue.nPicAreaWidth = 20;
        tmpValue.lpszIconName = mu_MenuListIcon[i-IDC_MU_MAILBOX];
        SendMessage(hwndMenuList, ML_ADDITEM, 0, (LPARAM)&tmpValue);
    }   

    return MSG_SUCCEED;
}

void MU_Prompt(int nErrno)
{    
    switch(nErrno)
    {
    case MSG_ERR_LOWMEMORY:
        ;//PLXTipsWin((char*)IDS_ERR_LOWMEMORY,(char*)IDS_WARNING,WAITTIMEOUT);
        break;

    case MSG_ERR_EMAIL:
        ;//PLXTipsWin((char*)IDS_ERR_EMAIL,(char*)IDS_WARNING,WAITTIMEOUT);
        break;

    case MSG_ERR_SMS:
        ;//PLXTipsWin((char*)IDS_ERR_SMS,(char*)IDS_WARNING,WAITTIMEOUT);
        break;

    case MSG_ERR_MMS:
        ;//PLXTipsWin((char*)IDS_ERR_MMS,(char*)IDS_WARNING,WAITTIMEOUT);
        break;
        
    case MSG_ERR_PUSH:
        ;//PLXTipsWin((char*)IDS_ERR_PUSH,(char*)IDS_WARNING,WAITTIMEOUT);
        break;

    case MSG_SUCCEED:
    default:
        break;
    }    
}

static LRESULT CallMLWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult=TRUE;
    
    switch(wMsgCmd)
    {         
    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        { 
        case VK_RETURN:
            PostMessage(GetParent(hWnd),wMsgCmd,wParam,lParam);
            return lResult;     

        default:
            return CallWindowProc(OldMLWndProc, hWnd, wMsgCmd, wParam, lParam);
        }
    default:
        return CallWindowProc(OldMLWndProc, hWnd, wMsgCmd, wParam, lParam);
    }
    return lResult;       
}

/*********************************************************************\
* Function	   MU_NewSelectWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
/*
BOOL MU_NewSelectWnd(HWND hParent)
{
	WNDCLASS wc;
	MSG msg;
    HWND hNewSelWnd;
	RECT rClient;

    wc.style         = 0;
    wc.lpfnWndProc   = MUNewSelectWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "MUNewSelectWndClass";
    
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
	
	GetClientRect(hMuFrame, &rClient);
	
	hNewSelWnd = CreateWindow(
		"MUNewSelectWndClass", 
        "",
        WS_CHILD | WS_VISIBLE,
        MU_NEWSEL_POSITION,  
        hMuFrame,
        NULL,
		NULL, 
		NULL
		);
	
	if (!hNewSelWnd)
	{
		UnregisterClass("MUNewSelectWndClass", NULL);
		return FALSE;
	}
	
	SendMessage(hNewSelWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)IDS_OK);
    SendMessage(hNewSelWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL,0), (LPARAM)IDS_CANCEL);
	
	//show window
	ShowWindow(hNewSelWnd, SW_SHOW);
	UpdateWindow(hNewSelWnd);

	while(GetMessage(&msg, 0, 0, 0))
	{
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!IsWindowVisible(hNewSelWnd))
            break;
	}

	UnregisterClass("MUNewSelectWndClass", NULL);

    return TRUE;

}*/

/*********************************************************************\
* Function	MUNewSelectWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
/*
static LRESULT MUNewSelectWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MUNewSelect_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
        MUNewSelect_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;
        
    case WM_PAINT:
        MUNewSelect_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        MUNewSelect_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        MUNewSelect_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        MUNewSelect_OnClose(hWnd);
        break;

    case WM_DESTROY:
        MUNewSelect_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;
}*/

/*********************************************************************\
* Function	MUNewSelect_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MUNewSelect_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    HWND hLst;
    RECT rc;

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
        (HMENU)IDC_MUNEWSEL_LIST, 
        NULL, 
        NULL);
    
    if(hLst == NULL)
        return FALSE;

    SendMessage(hLst,LB_ADDSTRING,(WPARAM)-1,(LPARAM)IDS_SMS);
    SendMessage(hLst,LB_ADDSTRING,(WPARAM)-1,(LPARAM)IDS_MMS);
    SendMessage(hLst,LB_ADDSTRING,(WPARAM)-1,(LPARAM)IDS_EMAIL);

    SendMessage(hLst,LB_SETCURSEL,0,0);

    
    return TRUE;
    
}
/*********************************************************************\
* Function	MUNewSelect_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MUNewSelect_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;

    hLst = GetDlgItem(hwnd,IDC_MUNEWSEL_LIST);

    if(state == WA_ACTIVE)
        SetFocus(hLst);

    return;
}

/*********************************************************************\
* Function	MUNewSelect_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUNewSelect_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);
    
	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	MUNewSelect_OnKey
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
static void MUNewSelect_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
	switch (vk)
	{
	case VK_F10:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	MUNewSelect_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MUNewSelect_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    HWND hLst;
    int nIndex;

    hLst = GetDlgItem(hWnd,IDC_MUNEWSEL_LIST);

	switch(id)
	{	
    case IDC_CANCEL:
        PostMessage(hWnd,WM_CLOSE,0,0);
        break;

    case IDC_OK:
        nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
        if(nIndex == LB_ERR)
        {
            break;
        }

        switch(nIndex) 
        {
        case 0:
            if(mu_initstatus.bSMS)
                mu_sms_interface->msg_new_message(GetWindow(hWnd,GW_OWNER));
        	break;
        
        case 1:
            if(mu_initstatus.bMMS)
                mu_mms_interface->msg_new_message(GetWindow(hWnd,GW_OWNER));
        	break;

        case 2:
            if(mu_initstatus.bEmail)
                mu_email_interface->msg_new_message(GetWindow(hWnd,GW_OWNER));
            break;

        default:
            break;
        }
        
        PostMessage(hWnd,WM_CLOSE,0,0);
        break;

    default:
        break;
	}

	return;
}

/*********************************************************************\
* Function	MUNewSelect_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUNewSelect_OnDestroy(HWND hWnd)
{    

	UnregisterClass("MUNewSelectWndClass",NULL);

    return;

}
/*********************************************************************\
* Function	MUNewSelect_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MUNewSelect_OnClose(HWND hWnd)
{

    DestroyWindow (hWnd);

    return;

}
/*********************************************************************\
* Function	MU_NewMsgArrival
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL MU_NewMsgArrival(MU_NOTIFY_TYPE nType)
{
    if(nType == MU_MAIL_NOTIFY)
        DlmNotify(PS_NEWMSG, 2);
    else
        DlmNotify(PS_NEWMSG, 1);

    if(IsWindow(hMainWnd) == TRUE)
    {
        HWND hMenuLst;
        int  nFolder;
        char *pszTitle;
        int  nUnread = 0,nCount = 0,nUnreadSum = 0,nCountSum = 0;
        char szDiaplay [100];
        MU_CREATEDATA *pCreateData;

        pCreateData = (MU_CREATEDATA*)GetUserData(hMainWnd);
        
        hMenuLst = GetDlgItem(hMainWnd,IDC_MU_MENULIST);
        
        if(pCreateData->nFocus == IDC_MU_INBOX)
        {
            nFolder = MU_INBOX;
            pszTitle = (char*)IDS_INBOX;
        }
        else if(pCreateData->nFocus == IDC_MU_DELIVERYREPORTS)
        {
            nFolder = MU_REPORT;
            pszTitle = (char*)IDS_DELIVERYREPORT;
        }
		else if(pCreateData->nFocus == IDC_MU_SIM)
		{
			int nCount;

			if(mu_initstatus.bSMS)
            {
				nCount = mu_smsex_interface->sms_get_simcounter();
				sprintf(szDiaplay,"%s -t %d", IDS_SIMMSG, nCount);
                SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDiaplay);
            }

			return TRUE;
		}
        else if(pCreateData->nFocus >= IDC_MU_MAX_DEFAULT)
		{
			if(mu_initstatus.bEmail)
            {
				mu_emailbox_interface->email_get_count(&pMU_EmailboxInfo[pCreateData->nFocus-IDC_MU_TEMPLATES-1], &nUnread, &nCount);
                sprintf(szDiaplay,"%s -t %d",pMU_EmailboxInfo[pCreateData->nFocus-IDC_MU_TEMPLATES-1].szEmailBoxName, nCount);
                SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDiaplay);
            }

			return TRUE;
		}
		else
            return FALSE;
        
        if(mu_initstatus.bSMS)
        {
            nUnread = 0;
            nCount = 0;
            mu_sms_interface->msg_get_count(nFolder,&nUnread,&nCount);
            nUnreadSum += nUnread;
            nCountSum += nCount;
        }
        if(mu_initstatus.bMMS)
        {
            nUnread = 0;
            nCount = 0;
            mu_mms_interface->msg_get_count(nFolder,&nUnread,&nCount);
            nUnreadSum += nUnread;
            nCountSum += nCount;
        }
        if(mu_initstatus.bEmail)
        {
            nUnread = 0;
            nCount = 0;
            mu_email_interface->msg_get_count(nFolder,&nUnread,&nCount);
            nUnreadSum += nUnread;
            nCountSum += nCount;
        }
        if(mu_initstatus.bPush)
        {
            nUnread = 0;
            nCount = 0;
            mu_push_interface->msg_get_count(nFolder,&nUnread,&nCount);
            nUnreadSum += nUnread;
            nCountSum += nCount;
        }
		if(mu_initstatus.bBT)
        {
            nUnread = 0;
            nCount = 0;
            mu_bt_interface->msg_get_count(nFolder,&nUnread,&nCount);
            nUnreadSum += nUnread;
            nCountSum += nCount;
        }
        sprintf(szDiaplay,"%s -t %d",pszTitle,nCountSum);
        SendMessage(hMenuLst,ML_SETITEMTEXT,(WPARAM)pCreateData->nFocus,(LPARAM)szDiaplay);

        return TRUE;        
    }

    return FALSE;
}

BOOL SelectMailBox(void)
{
	int n = nMailbox, i;
	int iFirst = 0, iTotal = 0;
	int id;
	int nUnread, nCount;

	for(i=0; i<n; i++)
	{
		mu_emailbox_interface->email_get_count(&pMU_EmailboxInfo[i], &nUnread, &nCount);
		if(nUnread != 0)
		{
			if(iFirst == 0)
				iFirst = i;

			iTotal++;
		}
	}

	if(iTotal == 1)
	{
		id = iFirst + 1 + IDC_MU_TEMPLATES;
		SendMessage(hMainWnd, WM_COMMAND, MAKEWPARAM(id, ML_SELNOTIFY), 
                0);
		bDirectExit = TRUE;
		return TRUE;
	}
	else if(iTotal > 1)
	{
		id = iFirst + 1 + IDC_MU_TEMPLATES;
		SendMessage(hMainWnd, ML_SETCURSEL, id, 
                0);
		bDirectExit = FALSE;
		return TRUE;
	}

	return FALSE;
}
