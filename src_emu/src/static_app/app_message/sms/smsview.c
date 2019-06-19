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

#define IDC_OK                  100
#define IDC_EXIT                200

#define IDM_SMSVIEW_GOTOURL             101
#define IDM_SMSVIEW_ADDTOBM             102
#define IDM_SMSVIEW_VIEWONMAP           103
#define IDM_SMSVIEW_SETASDESTINATION    104
#define IDM_SMSVIEW_SAVEASWAYPOINT      105
#define IDM_SMSVIEW_COPY                106
#define IDM_SMSVIEW_SEND                107//sub option
#define IDM_SMSVIEW_SEND_SMS                1071
#define IDM_SMSVIEW_SEND_MMS                1072
#define IDM_SMSVIEW_SEND_EMAIL              1073
#define IDM_SMSVIEW_REPLY               108
#define IDM_SMSVIEW_FORWARD             109
#define IDM_SMSVIEW_ADDTOCONTACTS       110//sub option
#define IDM_SMSVIEW_ADDTOCONTACTS_UPDATE    1101
#define IDM_SMSVIEW_ADDTOCONTACTS_CREATE    1102
#define IDM_SMSVIEW_FIND                111//sub option
#define IDM_SMSVIEW_FIND_PHONE              1111
#define IDM_SMSVIEW_FIND_EMAIL              1112
#define IDM_SMSVIEW_FIND_WEB                1113
#define IDM_SMSVIEW_FIND_COORDINATE         1114
#define IDM_SMSVIEW_HIDEFOUNDITEMS      112
#define IDM_SMSVIEW_MOVETOFOLDER        113//?????
#define IDM_SMSVIEW_MESSAGEINFO         114
#define IDM_SMSVIEW_DELETE              115

LRESULT SMSViewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL SMSView_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void SMSView_OnActivate(HWND hwnd, UINT state);
static void SMSView_OnSetFocus(HWND hWnd);
static void SMSView_OnInitmenu(HWND hwnd);
static void SMSView_OnPaint(HWND hWnd);
static void SMSView_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void SMSView_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void SMSView_OnDestroy(HWND hWnd);
static void SMSView_OnClose(HWND hWnd);
static void SMSView_OnSaveToAB(HWND hWnd,BOOL bSaved,ABNAMEOREMAIL* pABName);
static void SMSView_OnMoveToFolder(HWND hWnd,BOOL bMove,int nNewFolder);
static void SMSView_OnSureDelete(HWND hWnd,BOOL bDel);

BOOL SMS_ViewRegisterClass(void)
{
    WNDCLASS    wc;
    
    wc.style         = 0;
    wc.lpfnWndProc   = SMSViewWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(SMS_VIEWCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "SMSViewWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}

/*********************************************************************\
* Function	   SMS_CreateViewWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int SMS_CreateViewWnd(HWND hmuwnd,SMS_STORE* psms_store,DWORD handle,int folderid,BOOL bPre,BOOL bNext)
{ 
    SMS_VIEWCREATEDATA CreateData;
    HWND hViewWnd = NULL;
	RECT	 rcClient;
    SMS_VIEWCREATEDATA* pData;
    ABNAMEOREMAIL   ABName;
    char* p = NULL;

    memset(&CreateData,0,sizeof(SMS_VIEWCREATEDATA));
    CreateData.bPre = bPre;
    CreateData.bNext = bNext;
    CreateData.hMuWnd = hmuwnd;
	CreateData.hFrameWnd = MuGetFrame();
    CreateData.folderid = folderid;
    CreateData.handle = handle;
    CreateData.dwDateTime = psms_store->fix.dwDateTime;
    memcpy(&(CreateData.Store), psms_store,sizeof(SMS_STORE));
    CreateData.bChangeMenu = TRUE;

	CreateData.hMenu = CreateMenu();

	GetClientRect(MuGetFrame(),&rcClient);
	
    hViewWnd = CreateWindow(
        "SMSViewWndClass", 
        "",//need according to addressbook
        WS_VISIBLE | WS_CHILD,
        rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
        MuGetFrame(),
        NULL,
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!hViewWnd)
    {
		DestroyMenu(CreateData.hMenu);

        return FALSE;
    }

	PDASetMenu(MuGetFrame(),CreateData.hMenu);

	SetFocus(hViewWnd);

    memset(&ABName,0,sizeof(ABNAMEOREMAIL));

    if(APP_GetNameByPhone(psms_store->pszPhone,&ABName))
    {
        p = ABName.szName;
    }
    else
    {
        p = psms_store->pszPhone;
    }

	SetWindowText(MuGetFrame(),p);

    switch(CreateData.Store.fix.Stat) 
    {
    case MU_STU_UNREAD:
    case MU_STU_READ:
        SendMessage(MuGetFrame(), PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)IDS_REPLY);
    	break;
    default:
        SendMessage(MuGetFrame(), PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)IDS_FORWARD);
        break;
    }
    SendMessage(MuGetFrame(), PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_BACK);   
    SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);

    pData = GetUserData(hViewWnd);

    if(pData->bPre)
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)SMS_ICO_ARROWLEFT);
    else
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)"");

    if(pData->bNext)
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)SMS_ICO_ARROWRIGHT);
    else
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)"");

	
    //show window
    ShowWindow(hViewWnd, SW_SHOW);
    UpdateWindow(hViewWnd);
	
    return TRUE;;
}


/*********************************************************************\
* Function	SMSViewWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
LRESULT SMSViewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = SMSView_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        SMSView_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;
		
	case WM_SETFOCUS:
		SMSView_OnSetFocus(hWnd);
		break;
        
    case WM_INITMENU:
        SMSView_OnInitmenu(hWnd);
        break;

    case WM_PAINT:
        SMSView_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        SMSView_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        SMSView_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        SMSView_OnClose(hWnd);
        break;

    case WM_DESTROY:
        SMSView_OnDestroy(hWnd);
        break;

    case WM_SELECTFOLDER:
        SMSView_OnMoveToFolder(hWnd,(BOOL)wParam,(int)lParam);
        break;

    case WM_SAVETOAB:
        SMSView_OnSaveToAB(hWnd,(BOOL)wParam,(ABNAMEOREMAIL*)lParam);
        break;

    case WM_SUREDELETE:
        SMSView_OnSureDelete(hWnd,(BOOL)lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	SMSView_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL SMSView_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    PSMS_VIEWCREATEDATA pCreateData;
    int nlen;
    SMS_STORE* psms_store;

    pCreateData = GetUserData(hWnd);

    memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(SMS_VIEWCREATEDATA));

    psms_store = &(((PSMS_VIEWCREATEDATA)(lpCreateStruct->lpCreateParams))->Store);

    if(psms_store->fix.Conlen > 0)
    {
        pCreateData->Store.pszContent = (char*)malloc(psms_store->fix.Conlen);
        
        if(pCreateData->Store.pszContent == NULL)
            return FALSE;
        
        memcpy(pCreateData->Store.pszContent,psms_store->pszContent,psms_store->fix.Conlen);
    }
    else
    {
        if(pCreateData->Store.fix.Type == STATUS_REPORT)
        {
            if(pCreateData->Store.fix.Status = 0)
                pCreateData->Store.pszContent = (char*)IDS_CONFIRM;
            else
                pCreateData->Store.pszContent = (char*)IDS_WAITING;
        }
        else
        {
            pCreateData->Store.pszContent = (char*)malloc(1);
            
            if(pCreateData->Store.pszContent == NULL)
                return FALSE;
            
            pCreateData->Store.pszContent[0] = '\0';
        }
    }

    if(psms_store->fix.Phonelen > 0)
    {
        pCreateData->Store.pszPhone = (char*)malloc(psms_store->fix.Phonelen);
        
        if(pCreateData->Store.pszPhone == NULL)
        {
            SMS_FREE(pCreateData->Store.pszContent);
            return FALSE;
        }
        memcpy(pCreateData->Store.pszPhone,psms_store->pszPhone,psms_store->fix.Phonelen);
    }
    else
    {
        pCreateData->Store.pszPhone = (char*)malloc(1);

        if(pCreateData->Store.pszPhone == NULL)
            return FALSE;

        pCreateData->Store.pszPhone[0] = '\0';
    }
    
    nlen = strlen(pCreateData->Store.pszContent);

	pCreateData->hWndTextView = PlxTextView(pCreateData->hFrameWnd, hWnd, pCreateData->Store.pszContent,
        nlen,FALSE, NULL, NULL, 0);
    
    return TRUE;
    
}
/*********************************************************************\
* Function	SMSView_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSView_OnActivate(HWND hWnd, UINT state)
{
    PSMS_VIEWCREATEDATA pCreateData;

    ABNAMEOREMAIL ABName;

    char *p;
        
    pCreateData = GetUserData(hWnd);

    SetFocus(pCreateData->hWndTextView);

	PDASetMenu(pCreateData->hFrameWnd,pCreateData->hMenu);

    memset(&ABName,0,sizeof(ABNAMEOREMAIL));

    if(APP_GetNameByPhone(pCreateData->Store.pszPhone,&ABName))
    {
        p = ABName.szName;
    }
    else
    {
        p = pCreateData->Store.pszPhone;
    }

	SetWindowText(pCreateData->hFrameWnd,p);

    switch(pCreateData->Store.fix.Stat) 
    {
    case MU_STU_UNREAD:
    case MU_STU_READ:
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_REPLY);
    	break;
    default:
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_FORWARD);
        break;
    }
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);   
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);

    if(pCreateData->bPre)
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)SMS_ICO_ARROWLEFT);
    else
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)"");

    if(pCreateData->bNext)
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)SMS_ICO_ARROWRIGHT);
    else
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)"");

    return;
}
/*********************************************************************\
* Function	SMSView_OnSetFocus
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSView_OnSetFocus(HWND hWnd)
{

    PSMS_VIEWCREATEDATA pCreateData;
        
    pCreateData = GetUserData(hWnd);

    SetFocus(pCreateData->hWndTextView);

	return;
}
/*********************************************************************\
* Function	SMSView_OnInitmenu
* Purpose   WM_INITMENU message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSView_OnInitmenu(HWND hWnd)
{
    HMENU hMenu,hSendMenu,hAddMenu,hFindMenu;
    PSMS_VIEWCREATEDATA pCreateData;
    int   nMenuItem = 0;
	DWORD  high, low;
	int    ret;
        
    pCreateData = GetUserData(hWnd);

    hMenu = pCreateData->hMenu;

    pCreateData->bChangeMenu = FALSE;

    nMenuItem = GetMenuItemCount(hMenu);
    while(nMenuItem > 0)
    {
        nMenuItem--;
        DeleteMenu(hMenu,nMenuItem,MF_BYPOSITION);
    }

	ret = SendMessage(pCreateData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);

    if(ret != -1 && low == TVS_URL)
    {
        AppendMenu(hMenu,MF_ENABLED, IDM_SMSVIEW_GOTOURL, IDS_GOTOURL);
        AppendMenu(hMenu,MF_ENABLED, IDM_SMSVIEW_ADDTOBM, IDS_ADDTOBOOKMARK);
    }
    
    if(ret != -1 && low == TVS_COORDINATE)
    {
        AppendMenu(hMenu,MF_ENABLED, IDM_SMSVIEW_VIEWONMAP, IDS_VIEWONMAP);
        AppendMenu(hMenu,MF_ENABLED, IDM_SMSVIEW_SETASDESTINATION, IDS_SETASDESTINATION);
        AppendMenu(hMenu,MF_ENABLED, IDM_SMSVIEW_SAVEASWAYPOINT, IDS_SAVEASWAYPOINT);
    }
    
    if(ret != -1 && low == TVS_NUMBER)
    {
        hSendMenu = CreateMenu();   
        AppendMenu(hSendMenu,MF_ENABLED, IDM_SMSVIEW_SEND_SMS, IDS_SMS);
        AppendMenu(hSendMenu,MF_ENABLED, IDM_SMSVIEW_SEND_MMS, IDS_MMS);
        AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hSendMenu, IDS_WRITE);
    }
    else if(ret != -1 && low == TVS_EMAIL)
    {
        hSendMenu = CreateMenu();   
        AppendMenu(hSendMenu,MF_ENABLED, IDM_SMSVIEW_SEND_MMS, IDS_MMS);
        AppendMenu(hSendMenu,MF_ENABLED, IDM_SMSVIEW_SEND_EMAIL, IDS_EMAIL);
        AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hSendMenu, IDS_WRITE);
    }

    switch(pCreateData->Store.fix.Stat)
    {
    case MU_STU_UNREAD:
    case MU_STU_READ:
        AppendMenu(hMenu,MF_ENABLED, IDM_SMSVIEW_REPLY, IDS_REPLY);
    	break;

    default:
        break;
    }

    AppendMenu(hMenu,MF_ENABLED, IDM_SMSVIEW_FORWARD, IDS_FORWARD);

    if((ret != -1 && low != TVS_COORDINATE)
        || pCreateData->Store.fix.Stat == MU_STU_UNREAD
        || pCreateData->Store.fix.Stat == MU_STU_READ)
    {
        hAddMenu = CreateMenu();   
        AppendMenu(hAddMenu,MF_ENABLED, IDM_SMSVIEW_ADDTOCONTACTS_CREATE, IDS_CREATENEW);
        AppendMenu(hAddMenu,MF_ENABLED, IDM_SMSVIEW_ADDTOCONTACTS_UPDATE, IDS_UPDATEEXISTING);
        AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hAddMenu, IDS_ADDTOCONTACTS);
    }
    
    if( ret != -1 )
        AppendMenu(hMenu,MF_ENABLED, IDM_SMSVIEW_HIDEFOUNDITEMS, IDS_HIDEFOUNDITEMS);
    else
    {
        hFindMenu = CreateMenu();   
        AppendMenu(hFindMenu,MF_ENABLED, IDM_SMSVIEW_FIND_PHONE, IDS_PHONENO);
        AppendMenu(hFindMenu,MF_ENABLED, IDM_SMSVIEW_FIND_EMAIL, IDS_EMAILADDRESS);
        AppendMenu(hFindMenu,MF_ENABLED, IDM_SMSVIEW_FIND_WEB, IDS_WEB);
        AppendMenu(hFindMenu,MF_ENABLED, IDM_SMSVIEW_FIND_COORDINATE, IDS_COORDINATE);
        AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hFindMenu, IDS_FIND);
    }

    if(pCreateData->folderid != MU_INBOX || (pCreateData->folderid == MU_INBOX && CanMoveToFolder()) )
        AppendMenu(hMenu,MF_ENABLED, IDM_SMSVIEW_MOVETOFOLDER, IDS_MOVE);
    
    AppendMenu(hMenu,MF_ENABLED, IDM_SMSVIEW_MESSAGEINFO, IDS_MSGINFO);    
    AppendMenu(hMenu,MF_ENABLED, IDM_SMSVIEW_DELETE, IDS_DELETE);

    return;

}
/*********************************************************************\
* Function	SMSView_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSView_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

    EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	SMSView_OnKey
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
static void SMSView_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PSMS_VIEWCREATEDATA pCreateData;
        
    pCreateData = GetUserData(hWnd);

	switch (vk)
	{
    case VK_RETURN:
        if(pCreateData->Store.fix.Stat == MU_STU_UNREAD 
            || pCreateData->Store.fix.Stat == MU_STU_READ)
            SendMessage(hWnd,WM_COMMAND,IDM_SMSVIEW_REPLY,NULL);
        else
            SendMessage(hWnd,WM_COMMAND,IDM_SMSVIEW_FORWARD,NULL);
        break;

	case VK_F10:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

    case VK_F5:
		PDADefWindowProc(MuGetFrame(), WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        break;

    case VK_F1:
        {
            char              *phonenum;
            int               ret, offset, len;
            DWORD             wparam, lparam;
            
            // if there is high lighted
            ret = SendMessage(pCreateData->hWndTextView, TVM_GETCURHL, (WPARAM)&wparam, (LPARAM)&lparam);
            if (ret == -1 || (ret != -1 && lparam != TVS_NUMBER))
            {
                //if (pCreateData->Store.fix.Type == 2)	//?????????????????????????
                    APP_CallPhoneNumber(pCreateData->Store.pszPhone);
             
            }
            else
            {		
                offset = LOWORD(wparam);
                len = HIWORD(wparam);
                
                phonenum = malloc(len + 1);
                strncpy(phonenum, pCreateData->Store.pszContent + offset,len);
                phonenum[len] = 0;	
                APP_CallPhoneNumber(phonenum);
                free(phonenum);
            }
        }
        break;
        
        
    case VK_LEFT:
        if(pCreateData->bPre)
        {
            PostMessage(pCreateData->hMuWnd,PWM_MSG_MU_PRE,NULL,NULL); 
            PostMessage(hWnd,WM_CLOSE,0,0);
        }
        break;

    case VK_RIGHT:
        if(pCreateData->bNext)
        {
            PostMessage(pCreateData->hMuWnd,PWM_MSG_MU_NEXT,NULL,NULL);
            PostMessage(hWnd,WM_CLOSE,0,0);
        }
        break;

    default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	SMSView_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void SMSView_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    PSMS_VIEWCREATEDATA pCreateData;
    char szCaption[50];
    
    pCreateData = GetUserData(hWnd);

    switch(id)
	{   
    case IDM_SMSVIEW_GOTOURL:
    case IDM_SMSVIEW_ADDTOBM:
        {
            int ret, offset, len;
            DWORD high, low;
            char  *pUrl = NULL;
            
            // get the high lighted items
            ret = SendMessage(pCreateData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);
            if (ret != -1 && low == TVS_URL)	// highlighted
            {
                offset = LOWORD(high);
                len = HIWORD(high);
                pUrl = malloc(len + 1);
                strncpy(pUrl, pCreateData->Store.pszContent + offset,len);
                pUrl[len] = 0;
                // open browser and go to high lighted web address...(goto url)
                // add highlighted web address to browser bookmark...(add to bk)
                if(id == IDM_SMSVIEW_GOTOURL)
                    App_WapRequestUrl(pUrl);
                else
                    WBM_ADD_FROMURL(pUrl,pCreateData->hFrameWnd);
                
                free(pUrl);
            }
        }
        break;

    case IDM_SMSVIEW_VIEWONMAP:
        break;

    case IDM_SMSVIEW_SETASDESTINATION:
        break;

    case IDM_SMSVIEW_SAVEASWAYPOINT:
        break;

    case IDM_SMSVIEW_SEND_SMS:
    case IDM_SMSVIEW_SEND_MMS:
    case IDM_SMSVIEW_SEND_EMAIL:
        {		
            // get the high lighted items...
            // select highlighted phone number or email address to recipient
            // of the msg and open mail editor....
            SYSTEMTIME time;
            int		ret = 0, offset = 0,len = 0;
            DWORD   high = 0, low = 0;
            char    *pAddr = NULL;
            
            ret = SendMessage(pCreateData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);
            if (ret == -1)
                break;
            // get highlighted items
            offset = LOWORD(high);
            len = HIWORD(high);
            pAddr = malloc(len + 1);
            strncpy(pAddr, pCreateData->Store.pszContent + offset, len);

            pAddr[len] = 0;
            
            if (low == TVS_NUMBER)	// highlighted
            {
                if (id == IDM_SMSVIEW_SEND_SMS)
                    APP_EditSMS(MuGetFrame(), pAddr, NULL);
                else if (id == IDM_SMSVIEW_SEND_MMS)
                    APP_EditMMS(MuGetFrame(), hWnd, 0, MMS_CALLEDIT_MOBIL,pAddr);		
            }
            else if (low == TVS_EMAIL)
            {
                if (id == IDM_SMSVIEW_SEND_MMS)
                    APP_EditMMS(MuGetFrame(), hWnd, 0, MMS_CALLEDIT_MOBIL,pAddr);
                else if (id == IDM_SMSVIEW_SEND_EMAIL)
                {
                    GetLocalTime(&time);
                    CreateMailEditWnd(MuGetFrame(), pAddr, NULL, NULL, NULL, NULL, 
                        &time, -1, -1);
                }
            }
            free(pAddr);
        }
        break;
       
    case IDM_SMSVIEW_REPLY:
        SMS_CreateEditWnd(MuGetFrame(),GetParent(hWnd),pCreateData->Store.pszPhone,NULL,0,-1,-1,MU_MSG_SMS,NULL,FALSE);
        break;
        
    case IDM_SMSVIEW_FORWARD:
        SMS_CreateEditWnd(MuGetFrame(),GetParent(hWnd),NULL,pCreateData->Store.pszContent,
            pCreateData->Store.fix.Conlen,-1,-1,MU_MSG_SMS,NULL,TRUE);
        break;
 
    case IDM_SMSVIEW_ADDTOCONTACTS_UPDATE:
    case IDM_SMSVIEW_ADDTOCONTACTS_CREATE:
        {
            int		ret = 0, offset = 0,len = 0;
            DWORD   high = 0, low = 0;
            ABNAMEOREMAIL ABName;
            int     nMode;

            if(id == IDM_SMSVIEW_ADDTOCONTACTS_UPDATE)
                nMode = AB_UPDATE;
            else
                nMode = AB_NEW;

            memset(&ABName,0,sizeof(ABNAMEOREMAIL));
            
            ret = SendMessage(pCreateData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);
            
            if(ret == -1 )
            {
                if(pCreateData->Store.fix.Stat == MU_STU_UNREAD
                    || pCreateData->Store.fix.Stat == MU_STU_READ)
                {
                    ABName.nType = AB_NUMBER;
                    strcpy(ABName.szTelOrEmail,pCreateData->Store.pszPhone);
                    
                    APP_SaveToAddressBook(pCreateData->hFrameWnd,hWnd,WM_SAVETOAB,&ABName,nMode);

                    break;
                }
                else
                    break;
            }
            // get highlighted items
            offset = LOWORD(high);
            len = HIWORD(high);
            
            len = min(len,AB_MAXLEN_EMAILADDR-1);

            strncpy(ABName.szTelOrEmail,pCreateData->Store.pszContent+offset,len);

            ABName.szTelOrEmail[len] = 0;
            
            switch(low)
            {
            case TVS_NUMBER:
                ABName.nType = AB_NUMBER;
            	break;

            case TVS_EMAIL:
                ABName.nType = AB_EMAIL;
            	break;

            case TVS_URL:
                ABName.nType = AB_URL;
                break;

            default:
                return;
            }
            
            APP_SaveToAddressBook(pCreateData->hFrameWnd,NULL,0,&ABName,nMode);
        }
        break;
   
    case IDM_SMSVIEW_FIND_PHONE:   
        {
            int ret = 0;
            
            ret = SendMessage(pCreateData->hWndTextView, TVM_FINDNHL, 0, TVS_NUMBER);
            if (ret == -1)		// search found no results
            {
                char szTitle[50];

                szTitle[0] = 0;

                GetWindowText(pCreateData->hFrameWnd,szTitle,49);
                
                szTitle[49] = 0;
                
                PLXTipsWin(NULL, NULL, 0, IDS_NOMATCHESFOUND, szTitle,
                Notify_Info, IDS_OK, NULL, WAITTIMEOUT);
            }
        }
        break;

    case IDM_SMSVIEW_FIND_EMAIL: 
        {
            int ret = 0;
            
            ret = SendMessage(pCreateData->hWndTextView, TVM_FINDNHL, 0, TVS_EMAIL);
            if (ret == -1)		// search found no results
            {
                char szTitle[50];

                szTitle[0] = 0;

                GetWindowText(pCreateData->hFrameWnd,szTitle,49);
                
                szTitle[49] = 0;
                
                PLXTipsWin(NULL, NULL, 0, IDS_NOMATCHESFOUND, szTitle,
                Notify_Info, IDS_OK, NULL, WAITTIMEOUT);
            }
        }
        break;

    case IDM_SMSVIEW_FIND_WEB: 
        {
            int ret = 0;
            
            ret = SendMessage(pCreateData->hWndTextView, TVM_FINDNHL, 0, TVS_URL);
            if (ret == -1)		// search found no results
            {
                char szTitle[50];

                szTitle[0] = 0;

                GetWindowText(pCreateData->hFrameWnd,szTitle,49);
                
                szTitle[49] = 0;
                
                PLXTipsWin(NULL, NULL, 0, IDS_NOMATCHESFOUND, szTitle,
                Notify_Info, IDS_OK, NULL, WAITTIMEOUT);
            }
        }
        break;

    case IDM_SMSVIEW_FIND_COORDINATE: 
        {
            int ret = 0;
            
            ret = SendMessage(pCreateData->hWndTextView, TVM_FINDNHL, 0, TVS_COORDINATE);
            if (ret == -1)		// search found no results
            {
                char szTitle[50];

                szTitle[0] = 0;

                GetWindowText(pCreateData->hFrameWnd,szTitle,49);
                
                szTitle[49] = 0;
                
                PLXTipsWin(NULL, NULL, 0, IDS_NOMATCHESFOUND, szTitle,
                Notify_Info, IDS_OK, NULL, WAITTIMEOUT);
            }
        }
        break;

    case IDM_SMSVIEW_HIDEFOUNDITEMS:
		// remove highlights from the msg body
		SendMessage(pCreateData->hWndTextView, TVM_HIDEHL, 0, 0);
        break;

    case IDM_SMSVIEW_MOVETOFOLDER:
        if(!MU_FolderSelection(hWnd, hWnd, WM_SELECTFOLDER, pCreateData->folderid))      
            return;
        break;

    case IDM_SMSVIEW_MESSAGEINFO:
        SMS_CreateDetailWnd(pCreateData->hFrameWnd,&(pCreateData->Store));
        break;
        

    case IDM_SMSVIEW_DELETE://more attention
        pCreateData = (PSMS_VIEWCREATEDATA)GetUserData(hWnd);
               
        if(pCreateData->folderid == -1)
            break;

        GetWindowText(pCreateData->hFrameWnd,szCaption,50);

        PLXConfirmWinEx(GetParent(hWnd),hWnd,IDS_DELETEMESSAGE, Notify_Request, szCaption, IDS_YES, IDS_NO,WM_SUREDELETE);

        break;
                     
    default:
        break;
	}

	return;
}

/*********************************************************************\
* Function	SMSView_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSView_OnDestroy(HWND hWnd)
{
	
    PSMS_VIEWCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);

    SMS_FREE(pCreateData->Store.pszContent);
    
    SMS_FREE(pCreateData->Store.pszPhone);

    return;

}
/*********************************************************************\
* Function	SMSView_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSView_OnClose(HWND hWnd)
{
	SendMessage(GetParent(hWnd),PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);
    
    return;

}

/*********************************************************************\
* Function     SMSView_OnSaveToAB
* Purpose      
* Params      
* Return       
**********************************************************************/
static void SMSView_OnSaveToAB(HWND hWnd,BOOL bSaved,ABNAMEOREMAIL* pABName)
{
    PSMS_VIEWCREATEDATA pCreateData;

    if(bSaved == FALSE)
        return;

    pCreateData = GetUserData(hWnd);

    SetWindowText(pCreateData->hFrameWnd,pABName->szName);
}
/*********************************************************************\
* Function     SMSView_OnMoveToFolder
* Purpose      
* Params      
* Return       
**********************************************************************/
static void SMSView_OnMoveToFolder(HWND hWnd,BOOL bMove,int nNewFolder)
{
    PSMS_VIEWCREATEDATA pCreateData;
    char* pszOldFileName,*pszNewFileName;
    DWORD dwOffset;
    char szFileName[SMS_FILENAME_MAXLEN];
    char szOldPath[PATH_MAXLEN];
    SMS_INDEX* handle;
    char* pszCaption = NULL;
    ABNAMEOREMAIL ABName;
    
    if(bMove == FALSE)
        return;

    if(nNewFolder == -1)
        return;

    pCreateData = GetUserData(hWnd);

    if(pCreateData->folderid == -1)
        return;
    
    if(pCreateData->folderid == nNewFolder)
        return;
    
    memset(&ABName,0,sizeof(ABNAMEOREMAIL));

    if(APP_GetNameByPhone(pCreateData->Store.pszPhone,&ABName))
    {
        pszCaption = ABName.szName;
    }
    else
    {
        pszCaption = pCreateData->Store.pszPhone;
    }

    handle = (SMS_INDEX*)(pCreateData->handle);
    if(handle->byStoreType == STORE_INDEX)
    {
        MESTORE_INFO MEStoreInfo;
        SMS_INITDATA *pData;
        int Stat = MU_STU_READ;
        int i;
        
        pData = SMS_GetInitData();
        
        memset(&MEStoreInfo,0,sizeof(MESTORE_INFO));

        SMS_ReadOneMEStore(&MEStoreInfo,handle->index);

        MEStoreInfo.nfolderid = nNewFolder;
        
        SMS_WriteOneMEStore(&MEStoreInfo,handle->index);
        
        smsapi_Delete((DWORD)handle);
        
        SMS_ChangeCount(nNewFolder,SMS_COUNT_ALL,1);
        
        for(i = 0 ; i < pData->nMEMaxCount ; i++)
        {
            if(pData->pMESMSInfo[i].Index == handle->index)
            {
                Stat = pData->pMESMSInfo[i].Stat;
                break;
            }
        }
        
        SMS_ChangeCount(pCreateData->folderid,SMS_COUNT_ALL,-1);
                       
        //SendMessage(pCreateData->hMuWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)pCreateData->handle);
        
        PostMessage(hWnd,WM_CLOSE,0,0);
        
        PLXTipsWin(NULL, NULL, 0, (char*)IDS_MOVED, pszCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);
        
        return;
    }
    
    switch(pCreateData->folderid)
    {   
    case MU_REPORT:
    case MU_DRAFT:
        return;
        
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
        if(nNewFolder != MU_DRAFT)
            return;//small to small
        
        pCreateData->Store.fix.Stat = MU_STU_DRAFT;
        
        if(SMS_SaveFile(MU_DRAFT,&(pCreateData->Store),szFileName,MU_MSG_SMS) == FALSE)
        {
            return;
        }
        
        szOldPath[0] = 0;
        
        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_SMS);  
        
        remove(((SMS_INDEX*)(pCreateData->handle))->szFileName);
        
        chdir(szOldPath); 
        
        pCreateData = (PSMS_VIEWCREATEDATA)GetUserData(hWnd);
        
        SendMessage(pCreateData->hMuWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)pCreateData->handle);
        
        PLXTipsWin(NULL, NULL, 0, (char*)IDS_MOVED, pszCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);

        PostMessage(hWnd,WM_CLOSE,0,0);
        
        return;
        
    default:
        szFileName[0] = 0;
        sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,pCreateData->folderid,SMS_FILE_POSTFIX);
        pszOldFileName = szFileName;
        break;
    }
    
    
    
    switch(nNewFolder)
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
        return;
        
    default:
        szFileName[0] = 0;
        sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nNewFolder,SMS_FILE_POSTFIX);
        pszNewFileName = szFileName;
        break;
    }

    pCreateData->Store.fix.Conlen--;
    pCreateData->Store.fix.Stat = MU_STU_READ;
    if(SMS_SaveRecord(pszNewFileName,&(pCreateData->Store),&dwOffset) == FALSE)
    {
        pCreateData->Store.fix.Conlen++;
        return;
    }
    pCreateData->Store.fix.Conlen++;
    
    SMS_ChangeCount(nNewFolder,SMS_COUNT_ALL,1);
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  
    
    if(DeleteWithOffset(pszOldFileName,((SMS_INDEX*)(pCreateData->handle))->dwOffest) == -1)
    {
        chdir(szOldPath); 
        return;
    }
    
    SMS_ChangeCount(pCreateData->folderid,SMS_COUNT_ALL,-1);
    
    SMS_ChangeCount(pCreateData->folderid,SMS_COUNT_DEL,1);
    
    chdir(szOldPath); 
    pCreateData = (PSMS_VIEWCREATEDATA)GetUserData(hWnd);
    
    SendMessage(pCreateData->hMuWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)pCreateData->handle);
    PostMessage(hWnd,WM_CLOSE,0,0);

    PLXTipsWin(NULL, NULL, 0, (char*)IDS_MOVED, pszCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);
}








#define IDC_SMSREPORT_LIST  300

typedef struct tagSMSREPORT
{
	int		Status;
    char*   pszPhone;
    DWORD   dwDateTime;
} SMSREPORT, *PSMSREPORT;

LRESULT SMSReportWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL SMSReport_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void SMSReport_OnActivate(HWND hwnd, UINT state);
static void SMSReport_OnSetFocus(HWND hWnd);
static void SMSReport_OnPaint(HWND hWnd);
static void SMSReport_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void SMSReport_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void SMSReport_OnDestroy(HWND hWnd);
static void SMSReport_OnClose(HWND hWnd);
static void SMS_FormatReport(PSMSREPORT psmsreport,PSMS_STORE psms_store);

/*********************************************************************\
* Function	   SMS_CreateReportWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_CreateReportWnd(HWND hParent,SMS_STORE* psms_store)
{
    WNDCLASS wc;
    SMSREPORT smsreport;
	RECT	 rcClient;
    HWND     hReportWnd;
        
    wc.style         = 0;
    wc.lpfnWndProc   = SMSReportWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(SMSREPORT);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "SMSReportWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    memset(&smsreport,0,sizeof(SMSREPORT));

    SMS_FormatReport(&smsreport,psms_store);

	GetClientRect(MuGetFrame(),&rcClient);

    hReportWnd = CreateWindow(
        "SMSReportWndClass", 
        "",
        WS_VISIBLE | WS_CHILD,
        rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
        MuGetFrame(),
        NULL,
        NULL, 
        (PVOID)&smsreport
        );
    
    if (!hReportWnd)
    {
        UnregisterClass("SMSReportWndClass", NULL);
        return FALSE;
    }

	SetFocus(hReportWnd);
          
	SetWindowText(MuGetFrame(),IDS_REPORTTITLE);

    SendMessage(MuGetFrame(), PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL,0), (LPARAM)"");
    SendMessage(MuGetFrame(), PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)IDS_OK);
    SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return TRUE;
}

/*********************************************************************\
* Function	SMSReportWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT SMSReportWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = SMSReport_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        SMSReport_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;
        
    case WM_SETFOCUS:
        SMSReport_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        SMSReport_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        SMSReport_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        SMSReport_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        SMSReport_OnClose(hWnd);
        break;

    case WM_DESTROY:
        SMSReport_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	SMSReport_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL SMSReport_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    PSMSREPORT pCreateData;
    HWND hFromView = NULL;
    int  nIndex = 0,nKilobyte = 0;
    SYSTEMTIME sy;
    char szData[20],szTime[20];
    RECT rect;
    LISTBUF Listtmp;
    ABNAMEOREMAIL ABName;
    char* p;

    pCreateData = GetUserData(hWnd);

    memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(SMSREPORT));
    
    GetClientRect(hWnd,&rect);
        
	PREBROW_InitListBuf(&Listtmp);
    
    memset(&ABName,0,sizeof(ABNAMEOREMAIL));
    
    if(APP_GetNameByPhone(pCreateData->pszPhone,&ABName))
    {
        p = ABName.szTelOrEmail;
    }
    else
    {
        p = pCreateData->pszPhone;
    }
    
    PREBROW_AddData(&Listtmp, IDS_RECIPIENT, p);
    
    if(pCreateData->Status == -1)
        PREBROW_AddData(&Listtmp, IDS_DELIVERYSTATUS, IDS_PENDING);
    else if(pCreateData->Status == 0)
        PREBROW_AddData(&Listtmp, IDS_DELIVERYSTATUS, IDS_DELIVERED);
    else
        PREBROW_AddData(&Listtmp, IDS_DELIVERYSTATUS, IDS_FAILED);
    
    memset(&sy,0,sizeof(SYSTEMTIME));
    sy.wYear = (WORD)INTYEAR(pCreateData->dwDateTime);
    sy.wMonth = (WORD)INTMONTH(pCreateData->dwDateTime);
    sy.wDay = (WORD)INTDAY(pCreateData->dwDateTime);
    sy.wHour = (WORD)INTHOUR(pCreateData->dwDateTime);
    sy.wMinute = (WORD)INTMINUTE(pCreateData->dwDateTime);
    sy.wSecond = (WORD)INTSECOND(pCreateData->dwDateTime);
    
    szData[0] = 0;
    szTime[0] = 0;
    GetTimeDisplay(sy,szTime,szData);

    PREBROW_AddData(&Listtmp, IDS_DATE, szData);
    PREBROW_AddData(&Listtmp, IDS_TIME, szTime);

	hFromView = CreateWindow(
        FORMVIEWER,
        "",
		WS_VISIBLE  |WS_CHILD | WS_VSCROLL,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
		hWnd,
        (HMENU)IDC_SMSREPORT_LIST,
        NULL, 
        (PVOID)&Listtmp);

	if (hFromView == NULL) 
		return FALSE;

    return TRUE;
    
}
/*********************************************************************\
* Function	SMSReport_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSReport_OnActivate(HWND hWnd, UINT state)
{
    HWND hFromView;
	HWND hFrameWnd;

    hFromView = GetDlgItem(hWnd,IDC_SMSREPORT_LIST);
        
	SetFocus(hFromView);

	hFrameWnd = GetParent(hWnd);

	SetWindowText(hFrameWnd,IDS_REPORTTITLE);

    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return;
}
/*********************************************************************\
* Function	SMSReport_OnSetFocus
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSReport_OnSetFocus(HWND hWnd)
{
    HWND hFromView;

    hFromView = GetDlgItem(hWnd,IDC_SMSREPORT_LIST);
        
	SetFocus(hFromView);

    return;
}
/*********************************************************************\
* Function	SMSReport_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSReport_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	SMSReport_OnKey
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
static void SMSReport_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
	switch (vk)
	{
    case VK_RETURN:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	SMSReport_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void SMSReport_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{        
    default:
        break;
	}

	return;
}
/*********************************************************************\
* Function	SMSReport_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSReport_OnDestroy(HWND hWnd)
{
    PSMSREPORT pCreateData;

    pCreateData = GetUserData(hWnd);

    SMS_FREE(pCreateData->pszPhone);

	UnregisterClass("SMSReportWndClass", NULL);
	
    return;

}
/*********************************************************************\
* Function	SMSReport_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSReport_OnClose(HWND hWnd)
{

	SendMessage(GetParent(hWnd),PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow(hWnd);
	
    return;

}
/*********************************************************************\
* Function	 SMS_FormatReport  
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMS_FormatReport(PSMSREPORT psmsdetail,PSMS_STORE psms_store)
{
    psmsdetail->Status = psms_store->fix.Status;
    psmsdetail->dwDateTime = psms_store->fix.dwDateTime;

    psmsdetail->pszPhone = malloc(psms_store->fix.Phonelen);
    if(psmsdetail->pszPhone)
        strcpy(psmsdetail->pszPhone,psms_store->pszPhone);

    return;
}

/*********************************************************************\
* Function	 SMSView_OnSureDelete  
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSView_OnSureDelete(HWND hWnd,BOOL bDel)
{
    PSMS_VIEWCREATEDATA pCreateData;
    char* pszOldFileName = NULL;
    char szFileName [SMS_FILENAME_MAXLEN];
    char szOldPath [PATH_MAXLEN];
    DWORD handle;
    char szCaption[64];

    if(bDel == FALSE)
        return;

    pCreateData = (PSMS_VIEWCREATEDATA)GetUserData(hWnd);
    
    if(pCreateData->folderid == -1)
        return;

    szCaption[0] = 0;

    GetWindowText(pCreateData->hFrameWnd,szCaption,63);

    szCaption[63] = 0;

    handle = pCreateData->handle;

    if(handle && ((SMS_INDEX*)handle)->byStoreType == STORE_INDEX)
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
            return;
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
                    
                    SMS_ChangeCount(pCreateData->folderid,SMS_COUNT_ALL,-1);
                    
                    SMS_SetFull(FALSE);
                    
                    SMS_NotifyIdle();
                    
                    MEStoreInfo.nfolderid = 0;
                    
                    SMS_WriteOneMEStore(&MEStoreInfo,pMESMSInfo->Index);

                    memset(pMESMSInfo,0,sizeof(SMS_INFO));
                    
                    WaitWindowStateEx(NULL,FALSE,IDS_DELETING,IDS_SMS,NULL,NULL);
                    
                    SendMessage(pCreateData->hMuWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)pCreateData->handle);
                    
                    PLXTipsWin(NULL, NULL, 0, (char*)IDS_DELETED, szCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);

                    PostMessage(hWnd,WM_CLOSE,0,0);
  
                    return;
                }
                else if(myMsg.hwnd == SMS_GetSIMCtrlWnd())
                {
                    SendMessage(pCreateData->hMuWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),handle);
                    
                    WaitWindowStateEx(NULL,FALSE,IDS_DELETING,IDS_SMS,NULL,NULL);
                    
                    return;
                }
                TranslateMessage(&myMsg);
                DispatchMessage(&myMsg);
            }
        }     
    }
    
    switch(pCreateData->folderid)
    {   
    case MU_REPORT:
    case MU_DRAFT:
    case MU_OUTBOX:
        return;
        
    case MU_INBOX: // a big file
        pszOldFileName = SMS_FILENAME_INBOX;
        break;
        
    case MU_SENT:
        pszOldFileName = SMS_FILENAME_SENT;
        break;
        
    case MU_MYFOLDER:
        pszOldFileName = SMS_FILENAME_MYFOLDER;
        break;
        
    default:
        szFileName[0] = 0;
        sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,pCreateData->folderid,SMS_FILE_POSTFIX);
        pszOldFileName = szFileName;
        break;
    }
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  
    
    DeleteWithOffset(pszOldFileName,((SMS_INDEX*)(pCreateData->handle))->dwOffest);
    
    SMS_ChangeCount(pCreateData->folderid,SMS_COUNT_ALL,-1);
    
    SMS_ChangeCount(pCreateData->folderid,SMS_COUNT_DEL,1);
    
    chdir(szOldPath); 
    
    SendMessage(pCreateData->hMuWnd,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)pCreateData->handle);

    PLXTipsWin(NULL, NULL, 0, (char*)IDS_DELETED, szCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);

    SMS_TransferData();
        
    PostMessage(hWnd,WM_CLOSE,0,0);
}






typedef struct tagSMSCLASS0
{
    HWND hFrameWnd;
	HWND hWndTextView;
    char* pszText;
    HINSTANCE hInstance;
}SMSCLASS0, *PSMSCLASS0;

LRESULT SMSClass0WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL SMSClass0_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void SMSClass0_OnActivate(HWND hwnd, UINT state);
static void SMSClass0_OnSetFocus(HWND hWnd);
static void SMSClass0_OnPaint(HWND hWnd);
static void SMSClass0_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void SMSClass0_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void SMSClass0_OnDestroy(HWND hWnd);
static void SMSClass0_OnClose(HWND hWnd);

/*********************************************************************\
* Function	   SMS_CreateClass0Wnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_CreateClass0Wnd(HWND hFrameWnd,HINSTANCE hInstance)
{
    WNDCLASS wc;
	RECT	 rcClient;
    HWND     hClass0Wnd;
    SMSCLASS0 Data;
    ABNAMEOREMAIL ABName;
    PSMS_INSTANTCHAIN pInstant = NULL;
    char*    p = NULL;

    memset(&Data,0,sizeof(SMSCLASS0));
        
    wc.style         = 0;
    wc.lpfnWndProc   = SMSClass0WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(SMSCLASS0);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "SMSClass0WndClass";
    
    RegisterClass(&wc);

    Data.hFrameWnd = hFrameWnd;
    Data.hInstance = hInstance;
    
    GetClientRect(hFrameWnd,&rcClient);

    hClass0Wnd = CreateWindow(
        "SMSClass0WndClass", 
        "",
        WS_VISIBLE | WS_CHILD,
        rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
        hFrameWnd,
        (HMENU)IDC_CLASS0_VIEW,
        NULL, 
        (PVOID)&Data
        );
    
    if (!hClass0Wnd)
    {
        UnregisterClass("SMSClass0WndClass", NULL);

        return FALSE;
    }

	SetFocus(hClass0Wnd);
          
    pInstant = GetInstant();

    if(pInstant == NULL)
    {
        UnregisterClass("SMSClass0WndClass", NULL);

        return FALSE;
    }

    memset(&ABName,0,sizeof(ABNAMEOREMAIL));

    if(APP_GetNameByPhone(pInstant->smsinfo.SenderNum,&ABName))
    {
        p = ABName.szName;
    }
    else
    {
        p = pInstant->smsinfo.SenderNum;
    }

	SetWindowText(hFrameWnd,p);

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL,0), (LPARAM)IDS_EXIT);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)IDS_SAVE);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return TRUE;
}

/*********************************************************************\
* Function	SMSClass0WndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT SMSClass0WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = SMSClass0_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        SMSClass0_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;
        
    case WM_SETFOCUS:
        SMSClass0_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        SMSClass0_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        SMSClass0_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        SMSClass0_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        SMSClass0_OnClose(hWnd);
        break;

    case WM_DESTROY:
        SMSClass0_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	SMSClass0_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL SMSClass0_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    SMSCLASS0 *pData;
    PSMS_INSTANTCHAIN p;
    int  nLen = 0;
        
    pData = (SMSCLASS0*)GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(SMSCLASS0));
    
    p = GetInstant();

    if(p == NULL)
        return FALSE;

    SMS_ParseContentEx(p->smsinfo.dcs,p->smsinfo.Context,p->smsinfo.ConLen,&(pData->pszText),&nLen);

	pData->hWndTextView = PlxTextView(pData->hFrameWnd, hWnd, pData->pszText,
        nLen,FALSE, NULL, NULL, 0);
    
    if(pData->hWndTextView == NULL)
        return FALSE;

    return TRUE;
    
}
/*********************************************************************\
* Function	SMSClass0_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSClass0_OnActivate(HWND hWnd, UINT state)
{
    ABNAMEOREMAIL ABName;
    PSMS_INSTANTCHAIN pInstant = NULL;
    char*    p = NULL;
    SMSCLASS0 *pData;
        
    pData = (SMSCLASS0*)GetUserData(hWnd);

	SetFocus(pData->hWndTextView);
   
    pInstant = GetInstant();

    if(pInstant == NULL)
    {
        return ;
    }

    memset(&ABName,0,sizeof(ABNAMEOREMAIL));

    if(APP_GetNameByPhone(pInstant->smsinfo.SenderNum,&ABName))
    {
        p = ABName.szName;
    }
    else
    {
        p = pInstant->smsinfo.SenderNum;
    }

	SetWindowText(pData->hFrameWnd,p);

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return;
}
/*********************************************************************\
* Function	SMSClass0_OnSetFocus
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSClass0_OnSetFocus(HWND hWnd)
{
    SMSCLASS0 *pData;
        
    pData = (SMSCLASS0*)GetUserData(hWnd);

	SetFocus(pData->hWndTextView);

    return;
}
/*********************************************************************\
* Function	SMSClass0_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSClass0_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	SMSClass0_OnKey
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
static void SMSClass0_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PSMS_INSTANTCHAIN pInstant = NULL;
    SMS_STORE smsstore;
    DWORD     dwOffset;
	
    switch (vk)
	{
    case VK_RETURN:   
        
        pInstant = GetInstant();

        //Save
//        if(IsFlashEnough())
        {
            memset(&smsstore,0,sizeof(SMS_STORE));
            //unread counter++
            //sms counter++
            smsstore.fix.dcs = pInstant->smsinfo.dcs;
            smsstore.fix.dwDateTime = String2DWORD(pInstant->smsinfo.SendTime);
            smsstore.fix.Phonelen = strlen(pInstant->smsinfo.SenderNum)+1;
            strcpy(smsstore.fix.SCA,pInstant->smsinfo.SCA);
            smsstore.fix.Status = pInstant->smsinfo.Status;
            smsstore.fix.Stat = MU_STU_READ;
            smsstore.fix.Type = pInstant->smsinfo.Type;
            smsstore.fix.Udhlen  = pInstant->smsinfo.udhl;
            smsstore.pszPhone = pInstant->smsinfo.SenderNum;
            smsstore.pszUDH = pInstant->smsinfo.UDH;
            smsstore.fix.Conlen = pInstant->smsinfo.ConLen;
            smsstore.pszContent = pInstant->smsinfo.Context;
            
            if(SMS_SaveRecord(SMS_FILENAME_INBOX,&smsstore,&dwOffset))
                //sms counter++
                SMS_ChangeCount(MU_INBOX,SMS_COUNT_ALL,1);
        }

        if(MU_GetCurFolderType() == MU_INBOX)
        {
            SMS_ParseContent(pInstant->smsinfo.dcs, pInstant->smsinfo.Context , pInstant->smsinfo.ConLen ,&smsstore);
            
            smsapi_NewRecord(dwOffset,&smsstore);
            
            SMS_FREE(smsstore.pszContent);
        }
        
        MU_NewMsgArrival(MU_SMS_NOTIFY);
        
        SMS_NotifyIdle();
        //free
        
        Instant_Delete(pInstant);
        
        pInstant = GetInstant();
        
        if(pInstant != NULL)
        {
            if(SMS_UpdateView(hWnd) == FALSE)
                PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        }
        else
            PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        
        
        break;

    case VK_F10:

        pInstant = GetInstant();

        Instant_Delete(pInstant);
        
        pInstant = GetInstant();

        if(pInstant != NULL)
        {
            if(SMS_UpdateView(hWnd) == FALSE)
                PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        }
        else
            PostMessage(hWnd,WM_CLOSE,NULL,NULL);

        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	SMSClass0_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void SMSClass0_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{        
    default:
        break;
	}

	return;
}
/*********************************************************************\
* Function	SMSClass0_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSClass0_OnDestroy(HWND hWnd)
{
    SMSCLASS0 *pData;

    pData = GetUserData(hWnd);

    SMS_FREE(pData->pszText);

	UnregisterClass("SMSClass0WndClass", NULL);
	
    return;

}
/*********************************************************************\
* Function	SMSClass0_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSClass0_OnClose(HWND hWnd)
{
    SMSCLASS0 *pData;

    pData = GetUserData(hWnd);

	SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)(pData->hInstance));

    DestroyWindow(hWnd);
	
    return;

}

/*********************************************************************\
* Function	SMS_UpdateView
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL SMS_UpdateView(HWND hWnd)
{
    SMSCLASS0 *pData;
    PSMS_INSTANTCHAIN p;
    int  nLen = 0;
        
    pData = (SMSCLASS0*)GetUserData(hWnd);
    
    DestroyWindow(pData->hWndTextView);

    SMS_FREE(pData->pszText);

    p = GetInstant();

    if(p == NULL)
        return FALSE;

    SMS_ParseContentEx(p->smsinfo.dcs,p->smsinfo.Context,p->smsinfo.ConLen,&(pData->pszText),&nLen);

	pData->hWndTextView = PlxTextView(pData->hFrameWnd, hWnd, pData->pszText,
        nLen,FALSE, NULL, NULL, 0);

    if(pData->hWndTextView == NULL)
        return FALSE;

    return TRUE;
}
