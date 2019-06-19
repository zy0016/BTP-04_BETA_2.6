/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailView.c
*
* Purpose  : 
*
\**************************************************************************/

#include "MailHeader.h"

HWND  HwndMailView;

BOOL RegisterMailViewClass(void);
void UnRegisterMailViewClass(void);
BOOL MAIL_CreateViewWnd(HWND hParent, const char *pszOutFile, DWORD handle, int folderid, BOOL bPre, BOOL bNext);
static LRESULT MailViewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL MailView_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MailView_OnActivate(HWND hWnd, UINT state);
static void MailView_OnInitmenu(HWND hWnd);
static void MailView_OnPaint(HWND hWnd);
static void MailView_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MailView_OnCommand(HWND hWnd, int id, UINT codeNotify, LPARAM lParam);
static void MailView_OnDestroy(HWND hWnd);
static void MailView_OnClose(HWND hWnd);
static void MailView_OnMoveToFolder(HWND hWnd,BOOL bMove,int nNewFolder);
static void MailView_OnDeleteRet(HWND hWnd, WPARAM wParam, LPARAM lParam);

static void MAIL_SetViewIndicator(HWND hWnd, int nFrag);
void MAIL_SetViewNumber(HWND hWnd, int leftNum, int rightNum);

extern BOOL CreateMailDetailWnd(HWND hParent, MailFile *szOutFile);
extern BOOL CreateMailEditWnd(HWND hParent, const char *PSZRECIPIENT, const char *PSZRECIPIENT2, 
                              const char *PSZTITLE, const char *PSZCONTENT, const char *PSZATTENMENT, 
                              const SYSTEMTIME *PTIME, DWORD handle, int folderid);
extern BOOL CreateMailDisplayWnd(HWND hFrameWnd, HWND hParent, char *pAttHead, int AttNum, int nAttSize, BOOL bEdit);

extern BOOL mail_delete_message(HWND hwndmu, DWORD msghandle);
extern BOOL mail_move_message(HWND hwndmu, DWORD handle, int nFolder);
extern void MailInBox_OnFolderReturn(HWND hWnd, WPARAM wParam, LPARAM lParam);
extern BOOL APP_EditSMS(HWND hParent,const char* PSZRECIPIENT,const char* PSZCONTENT);
extern BOOL MAIL_GetGetNodebyInt(MailGetListHead *pHead, MailGetListNode **pListNode, int iTmp);

extern BOOL APP_CallPhoneNumber(const char * pPhoneNumber);
extern int WBM_ADD_FROMURL(char *szinput, HWND hWnd);
typedef void* HBROWSER;
extern HBROWSER App_WapRequestUrl(char *szurl);

/*********************************************************************\
* Function	   RegisterMailViewClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL RegisterMailViewClass(void)
{
    WNDCLASS wc;
        
    wc.style         = 0;
    wc.lpfnWndProc   = MailViewWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MAIL_VIEWCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "MailViewWndClass";
    
    if(!RegisterClass(&wc))
    {
        UnRegisterMailViewClass();
        if(!RegisterClass(&wc))
            return FALSE;
    }

    return TRUE;
}

void UnRegisterMailViewClass(void)
{
    UnregisterClass("MailViewWndClass", NULL);
}

/*********************************************************************\
* Function	   MAIL_CreateViewWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MAIL_CreateViewWnd(HWND hParent, const char *pszOutFile, DWORD handle, int folderid, BOOL bPre, BOOL bNext)
{ 
    MAIL_VIEWCREATEDATA CreateData;
    RECT    rClient;
    char    szOutFile[PATH_MAXLEN];
    int     hFile;

#ifdef MAIL_DEBUG
    StartObjectDebug();
#endif
    //if (!RegisterMailViewClass())
    //   return FALSE;

    memset(&CreateData, 0x0, sizeof(MAIL_VIEWCREATEDATA));
    CreateData.folderid = folderid;
    CreateData.handle = handle;
    CreateData.hFrameWnd = MuGetFrame();
    CreateData.hParent = hParent;
    CreateData.hMenu = CreateMenu();
    CreateData.bPre = bPre;
    CreateData.bNext = bNext;
    strcpy(CreateData.szFileName, pszOutFile);

    memset(szOutFile, 0x0, PATH_MAXLEN);
    getcwd(szOutFile, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    hFile = GARY_open(CreateData.szFileName, O_RDWR, -1);
    if(hFile < 0)
    {
        printf("\r\nMAIL_CreateViewWnd : open error = %d\r\n", errno);

        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        chdir(szOutFile);
        return FALSE;
    }
    //mailfile header
    if(read(hFile, &CreateData.ViewFile, sizeof(MailFile)) != sizeof(MailFile))
    {
        GARY_close(hFile);
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        chdir(szOutFile);
        return FALSE;
    }
    //mail body(text)
    CreateData.pText = (char *)malloc(CreateData.ViewFile.TextSize + 1);
    if(CreateData.pText == NULL)
    {
        GARY_close(hFile);
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        chdir(szOutFile);
        return FALSE;
    }
    memset(CreateData.pText, 0x0, CreateData.ViewFile.TextSize + 1);
    if(read(hFile, CreateData.pText, CreateData.ViewFile.TextSize) != (int)CreateData.ViewFile.TextSize)
    {
        GARY_FREE(CreateData.pText);
        GARY_close(hFile);
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        chdir(szOutFile);
        return FALSE;
    }
    //mail attachment
    CreateData.pAtt = (char *)malloc(CreateData.ViewFile.AttFileNameLen + 1);
    if(CreateData.pAtt == NULL)
    {
        GARY_FREE(CreateData.pText);
        GARY_close(hFile);
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        chdir(szOutFile);
        return FALSE;
    }
    memset(CreateData.pAtt, 0x0, CreateData.ViewFile.AttFileNameLen + 1);
    if(read(hFile, CreateData.pAtt, CreateData.ViewFile.AttFileNameLen) 
        != (int)CreateData.ViewFile.AttFileNameLen)
    {
        GARY_FREE(CreateData.pText);
        GARY_FREE(CreateData.pAtt);
        GARY_close(hFile);
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        chdir(szOutFile);
        return FALSE;
    }

    if(CreateData.folderid == MU_BOXMAIL)
    {
        CreateData.ViewFile.ReadFlag = READ;
    }
    else if(CreateData.folderid > 4)
    {
        CreateData.ViewFile.ReadFlag = MU_STU_READ;
    }
    else if(CreateData.folderid == MU_INBOX)
    {
        CreateData.ViewFile.ReadFlag = MU_STU_READ;
    }
    
    lseek(hFile, 0, SEEK_SET);
    write(hFile, &CreateData.ViewFile, sizeof(MailFile));
    GARY_close(hFile);
    chdir(szOutFile);

    GetClientRect(CreateData.hFrameWnd, &rClient);

    HwndMailView = CreateWindow(
        "MailViewWndClass", 
        "",//need according to addressbook
        WS_VISIBLE | WS_CHILD,
        rClient.left, 
        rClient.top, 
        rClient.right - rClient.left,
        rClient.bottom - rClient.top, 
        CreateData.hFrameWnd,//hParent,
        NULL, //hMenu,
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!HwndMailView)
    {
        return FALSE;
    }

    //show window
    PDASetMenu(CreateData.hFrameWnd, CreateData.hMenu);
    {
        char *p1, *p;
        ABNAMEOREMAIL ABName;
        char  Array[MAX_MAIL_RECIPIENT_NUM * 102];
        int nCount = 0;
        int i;
        char *Dis1;
        int nlen;

        Dis1 = NULL;
        memset(Array, 0, MAX_MAIL_RECIPIENT_NUM * 102);

        if(strcmp(CreateData.ViewFile.To, CreateData.ViewFile.address) == 0)
        {
            p1 = p = CreateData.ViewFile.To;
        }
        else if(strcmp(CreateData.ViewFile.From, CreateData.ViewFile.address) == 0)
        {
            p1 = p = CreateData.ViewFile.From;
        }
        
        while(*p !='\0')
        {
            if(*p == ';')
            {
                *p = '\0';
                
                if(strlen(p1) > ME_PHONENUM_LEN - 1) //now
                    continue;
                
                memset(&ABName, 0x0, sizeof(ABNAMEOREMAIL));
                
                if(APP_GetNameByEmail(p1, &ABName))
                    strcpy(Array + (nCount * 102), ABName.szName);
                else
                    strcpy(Array + (nCount * 102), p1);
                
                nCount++;
                p1 = p+1;
                if( *p1 == '\0')
                {
                    *p = ';';
                    break;
                }
            }
            p++;
        }
        
        if(*p1 != '\0')
        {   
            memset(&ABName,0,sizeof(ABNAMEOREMAIL));
            
            if(APP_GetNameByEmail(p1, &ABName))
                strcpy(Array + (nCount * 102), ABName.szName);
            else
                strcpy(Array + (nCount * 102), p1);
            
            nCount++;
        }
        
        nlen = 0;
        for(i = 0 ; i < nCount ; i++)
        {
            nlen += strlen(Array + ( i * 102));
            nlen += 1;
        }
        
        Dis1 = (char *)malloc(nlen + 1);
        if(Dis1 == NULL)
            return FALSE;
        
        memset(Dis1, 0, nlen + 1);
        
        for(i = 0 ; i < nCount ; i++)
        {
            strcat(Dis1, Array + (i * 102));
            if(i != nCount -1)
            {
                strcat(Dis1, ";");
            }
        }
        Dis1[nlen] = '\0';

        SetWindowText(CreateData.hFrameWnd, Dis1);

        GARY_FREE(Dis1);
    }
    
    ShowWindow(CreateData.hFrameWnd, SW_SHOW);
    UpdateWindow(CreateData.hFrameWnd);
	
    return TRUE;
}


/*********************************************************************\
* Function	MailViewWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT MailViewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MailView_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        MailView_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;
        
    case WM_INITMENU:
        MailView_OnInitmenu(hWnd);
        break;

    case WM_PAINT:
        MailView_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        MailView_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        MailView_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam), lParam);
        break;
        
    case WM_CLOSE:
        MailView_OnClose(hWnd);
        break;

    case WM_DESTROY:
        MailView_OnDestroy(hWnd);
        break;

    case WM_MAIL_FOLDER_RETURN:
        MailView_OnMoveToFolder(hWnd,(BOOL)wParam,(int)lParam);
        break;

    case WM_MAIL_DIS_RETURN:
        MAIL_SetViewNumber(hWnd, (int)wParam, (int)lParam);
        break;

    case WM_MAIL_DELETE_RETURN:
        MailView_OnDeleteRet(hWnd, wParam, lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	MailView_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MailView_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    PMAIL_VIEWCREATEDATA pCreateData;
    RECT rcClient;
        
    pCreateData = GetUserData(hWnd);

    memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(MAIL_VIEWCREATEDATA));
         
    GetClientRect(pCreateData->hFrameWnd, &rcClient);
    
    //pCreateData->hLeftIcon = LoadImage(NULL, MAIL_ICON_LEFT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    //pCreateData->hRightIcon = LoadImage(NULL, MAIL_ICON_RIGHT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);

    pCreateData->hWndTextView = PlxTextView(pCreateData->hFrameWnd, hWnd, pCreateData->pText,
        pCreateData->ViewFile.TextSize, FALSE, NULL, NULL, 0);

    if(pCreateData->bPre)
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)MAIL_ICON_LEFT);//pCreateData->hLeftIcon
    }
    else
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    }

    if(pCreateData->ViewFile.AttNum > 0)
    {
        MAIL_SetViewIndicator(hWnd, pCreateData->ViewFile.AttNum);
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)pCreateData->hAttIcon);
    }
    else if(pCreateData->bNext)
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)MAIL_ICON_RIGHT);
    }
    else
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
    }
    
    SetFocus(pCreateData->hWndTextView);

    if(strcmp(pCreateData->ViewFile.To, pCreateData->ViewFile.address) == 0)//pCreateData->folderid == MU_DRAFT
    {
        SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_VIEW_FORWARD, 1), (LPARAM)IDP_MAIL_BUTTON_FORWARD);
    }
    else if(strcmp(pCreateData->ViewFile.From, pCreateData->ViewFile.address) == 0)//pCreateData->folderid == MU_BOXMAIL
    {
        SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_VIEW_REPLY, 1), (LPARAM)IDP_MAIL_BUTTON_REPLY);
    }
    SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), (LPARAM)IDP_MAIL_BUTTON_BACK);
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    return TRUE;
}
/*********************************************************************\
* Function	MailView_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MailView_OnActivate(HWND hWnd, UINT state)
{
    PMAIL_VIEWCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);
    
    SetFocus(pCreateData->hWndTextView);

    PDASetMenu(pCreateData->hFrameWnd, pCreateData->hMenu);
    
    {
        char *p1, *p;
        ABNAMEOREMAIL ABName;
        char  Array[MAX_MAIL_RECIPIENT_NUM * 102];
        int nCount = 0;
        int i;
        char *Dis1;
        int nlen;
        
        Dis1 = NULL;
        memset(Array, 0, MAX_MAIL_RECIPIENT_NUM * 102);
        
        if(strcmp(pCreateData->ViewFile.To, pCreateData->ViewFile.address) == 0)
        {
            p1 = p = pCreateData->ViewFile.To;
        }
        else if(strcmp(pCreateData->ViewFile.From, pCreateData->ViewFile.address) == 0)
        {
            p1 = p = pCreateData->ViewFile.From;
        }
        
        while(*p !='\0')
        {
            if(*p == ';')
            {
                *p = '\0';
                
                if(strlen(p1) > ME_PHONENUM_LEN - 1) //now
                    continue;
                
                memset(&ABName, 0x0, sizeof(ABNAMEOREMAIL));
                
                if(APP_GetNameByEmail(p1, &ABName))
                    strcpy(Array + (nCount * 102), ABName.szName);
                else
                    strcpy(Array + (nCount * 102), p1);
                
                nCount++;
                p1 = p+1;
                if( *p1 == '\0')
                {
                    *p = ';';
                    break;
                }
            }
            p++;
        }
        
        if(*p1 != '\0')
        {   
            memset(&ABName,0,sizeof(ABNAMEOREMAIL));
            
            if(APP_GetNameByEmail(p1, &ABName))
                strcpy(Array + (nCount * 102), ABName.szName);
            else
                strcpy(Array + (nCount * 102), p1);
            
            nCount++;
        }
        
        nlen = 0;
        for(i = 0 ; i < nCount ; i++)
        {
            nlen += strlen(Array + ( i * 102));
            nlen += 1;
        }
        
        Dis1 = (char *)malloc(nlen + 1);
        if(Dis1 == NULL)
            return;
        
        memset(Dis1, 0, nlen + 1);
        
        for(i = 0 ; i < nCount ; i++)
        {
            strcat(Dis1, Array + (i * 102));
            if(i != nCount -1)
            {
                strcat(Dis1, ";");
            }
        }
        Dis1[nlen] = '\0';
        
        SetWindowText(pCreateData->hFrameWnd, Dis1);
        
        GARY_FREE(Dis1);
    }

    if(strcmp(pCreateData->ViewFile.To, pCreateData->ViewFile.address) == 0)//pCreateData->folderid == MU_DRAFT
    {
        SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_VIEW_FORWARD, 1), (LPARAM)IDP_MAIL_BUTTON_FORWARD);
    }
    else if(strcmp(pCreateData->ViewFile.From, pCreateData->ViewFile.address) == 0)//pCreateData->folderid == MU_BOXMAIL
    {
        SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_VIEW_REPLY, 1), (LPARAM)IDP_MAIL_BUTTON_REPLY);
    }
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_BACK);
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    
    if(pCreateData->bPre)
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)MAIL_ICON_LEFT);//pCreateData->hLeftIcon
    }
    else
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    }
    
    if(pCreateData->ViewFile.AttNum > 0)
    {
        MAIL_SetViewIndicator(hWnd, pCreateData->ViewFile.AttNum);
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)pCreateData->hAttIcon);
    }
    else if(pCreateData->bNext)
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)MAIL_ICON_RIGHT);
    }
    else
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
    }
    
    return;
}

/*********************************************************************\
* Function	MailView_OnInitmenu
* Purpose   WM_INITMENU message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailView_OnInitmenu(HWND hWnd)
{
    HMENU hMenu, hReplyMenu, hSendMenu, hAddMenu, hFindMenu;
    PMAIL_VIEWCREATEDATA pCreateData;
    int   nMenuItem = 0;
	DWORD  high, low;
	int    ret;
        
    pCreateData = GetUserData(hWnd);

    hMenu = pCreateData->hMenu;

    nMenuItem = GetMenuItemCount(hMenu);
    while(nMenuItem > 0)
    {
        nMenuItem--;
        DeleteMenu(hMenu, nMenuItem, MF_BYPOSITION);
    }

	ret = SendMessage(pCreateData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);

    if(ret != -1 && low == TVS_URL)
    {
        AppendMenu(hMenu, MF_ENABLED, IDM_MAIL_VIEW_GOTOURL, IDP_MAIL_VIEW_GOTOURL);
        AppendMenu(hMenu, MF_ENABLED, IDM_MAIL_VIEW_ADDTOBM, IDP_MAIL_VIEW_ADDTOBM);
    }
    
    if(ret != -1 && low == TVS_COORDINATE)
    {
        AppendMenu(hMenu, MF_ENABLED, IDM_MAIL_VIEW_VIEWONMAP, IDP_MAIL_VIEW_VIEWONMAP);
        AppendMenu(hMenu, MF_ENABLED, IDM_MAIL_VIEW_SETASDESTINATION, IDP_MAIL_VIEW_SETASDESTINATION);
        AppendMenu(hMenu, MF_ENABLED, IDM_MAIL_VIEW_SAVEASWAYPOINT, IDP_MAIL_VIEW_SAVEASWAYPOINT);
    }
    
    if(ret != -1 && low == TVS_NUMBER)
    {
        hSendMenu = CreateMenu();   
        AppendMenu(hSendMenu, MF_ENABLED, IDM_MAIL_VIEW_SEND_SMS, IDP_MAIL_VIEW_SEND_SMS);
        AppendMenu(hSendMenu, MF_ENABLED, IDM_MAIL_VIEW_SEND_MMS, IDP_MAIL_VIEW_SEND_MMS);
        AppendMenu(hMenu, MF_POPUP | MF_ENABLED, (DWORD)hSendMenu, IDP_MAIL_VIEW_SEND);
    }
    else if(ret != -1 && low == TVS_EMAIL)
    {
        hSendMenu = CreateMenu();   
        AppendMenu(hSendMenu, MF_ENABLED, IDM_MAIL_VIEW_SEND_MMS, IDP_MAIL_VIEW_SEND_MMS);
        AppendMenu(hSendMenu, MF_ENABLED, IDM_MAIL_VIEW_SEND_EMAIL, IDP_MAIL_VIEW_SEND_EMAIL);
        AppendMenu(hMenu, MF_POPUP | MF_ENABLED, (DWORD)hSendMenu, IDP_MAIL_VIEW_SEND);
    }

    if(pCreateData->ViewFile.AttFileNameLen > 0)
    {
        AppendMenu(hMenu, MF_ENABLED, IDM_MAIL_VIEW_ATTACHMENTS, IDP_MAIL_VIEW_ATTACHMENTS);
    }

    if(strcmp(pCreateData->ViewFile.From, pCreateData->ViewFile.address) == 0)//pCreateData->folderid == MU_BOXMAIL
    {
        int AddrNum;
        
        AppendMenu(hMenu, MF_ENABLED, IDM_MAIL_VIEW_REPLY, IDP_MAIL_VIEW_REPLY);

        AddrNum = MAIL_AnalyseMailAddr(pCreateData->ViewFile.From);
        AddrNum += MAIL_AnalyseMailAddr(pCreateData->ViewFile.Cc);
        if(AddrNum > 1)
        {
            hReplyMenu = CreateMenu();   
            AppendMenu(hReplyMenu, MF_ENABLED, IDM_MAIL_REPLY_TOSENDER, IDP_MAIL_BUTTON_REPLYTO);
            AppendMenu(hReplyMenu, MF_ENABLED, IDM_MAIL_REPLY_TOALL, IDP_MAIL_BUTTON_REPLYALL);
            AppendMenu(hMenu, MF_POPUP | MF_ENABLED, (DWORD)hReplyMenu, IDP_MAIL_VIEW_REPLYMANY);
        }
    }

    AppendMenu(hMenu, MF_ENABLED, IDM_MAIL_VIEW_FORWARD, IDP_MAIL_VIEW_FORWARD);

    if((ret != -1) && (low != TVS_COORDINATE) && 
        (strcmp(pCreateData->ViewFile.From, pCreateData->ViewFile.address) == 0))
    {
        hAddMenu = CreateMenu();
        AppendMenu(hAddMenu, MF_ENABLED, IDM_MAIL_VIEW_ADDTOCONTACTS_CREATE, IDP_MAIL_VIEW_ADDTOCONTACTS_CREATE);
        AppendMenu(hAddMenu, MF_ENABLED, IDM_MAIL_VIEW_ADDTOCONTACTS_UPDATE, IDP_MAIL_VIEW_ADDTOCONTACTS_UPDATE);
        AppendMenu(hMenu, MF_POPUP|MF_ENABLED, (DWORD)hAddMenu, IDP_MAIL_VIEW_ADDTOCONTACTS);
    }
    
    if( ret != -1 )
        AppendMenu(hMenu,MF_ENABLED, IDM_MAIL_VIEW_HIDEFOUNDITEMS, IDP_MAIL_VIEW_HIDEFOUNDITEMS);
    else
    {
        hFindMenu = CreateMenu();   
        AppendMenu(hFindMenu,MF_ENABLED, IDM_MAIL_VIEW_FIND_PHONE, IDP_MAIL_VIEW_FIND_PHONE);
        AppendMenu(hFindMenu,MF_ENABLED, IDM_MAIL_VIEW_FIND_EMAIL, IDP_MAIL_VIEW_FIND_EMAIL);
        AppendMenu(hFindMenu,MF_ENABLED, IDM_MAIL_VIEW_FIND_WEB, IDP_MAIL_VIEW_FIND_WEB);
        AppendMenu(hFindMenu,MF_ENABLED, IDM_MAIL_VIEW_FIND_COORDINATE, IDP_MAIL_VIEW_FIND_COORDINATE);
        AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hFindMenu, IDP_MAIL_VIEW_FIND);
    }

    //if(MU_CanMoveToFolder())
    if(pCreateData->folderid != MU_BOXMAIL)
    {
        AppendMenu(hMenu,MF_ENABLED, IDM_MAIL_VIEW_MOVETOFOLDER, IDP_MAIL_VIEW_MOVETOFOLDER);
    }
    else
    {
        AppendMenu(hMenu,MF_ENABLED, IDM_MAIL_VIEW_MOVETOFOLDER, IDP_MAIL_BUTTON_MOVETOFOLDER);
    }
    
    AppendMenu(hMenu,MF_ENABLED, IDM_MAIL_VIEW_MESSAGEINFO, IDP_MAIL_VIEW_MESSAGEINFO);    
    AppendMenu(hMenu,MF_ENABLED, IDM_MAIL_VIEW_DELETE, IDP_MAIL_VIEW_DELETE);

    return;

}
/*********************************************************************\
* Function	MailView_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailView_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

    EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	MailView_OnKey
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
static void MailView_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PMAIL_VIEWCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);
    
    switch (vk)
    {
    case VK_RETURN:
        if(strcmp(pCreateData->ViewFile.To, pCreateData->ViewFile.address) == 0)//pCreateData->folderid == MU_DRAFT
        {
            SendMessage(hWnd, WM_COMMAND, IDM_MAIL_VIEW_FORWARD, (LPARAM)NULL);
        }
        else if(strcmp(pCreateData->ViewFile.From, pCreateData->ViewFile.address) == 0)//pCreateData->folderid == MU_BOXMAIL
        {
            SendMessage(hWnd, WM_COMMAND, IDM_MAIL_VIEW_REPLY, (LPARAM)NULL);
        }
        break;
        
    case VK_F10:
        {
            SendMessage(hWnd, WM_COMMAND, IDM_MAIL_BUTTON_EXIT, (LPARAM)NULL);
        }
        break;
        
    case VK_F5:
        {
            PDADefWindowProc(pCreateData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        }
        break;
        
    case VK_LEFT:
        if(pCreateData->bPre)
        {
            PostMessage(pCreateData->hParent, PWM_MSG_MU_PRE, NULL, (LPARAM)pCreateData->handle);
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;
        
    case VK_RIGHT:
        if(pCreateData->bNext)
        {
            PostMessage(pCreateData->hParent, PWM_MSG_MU_NEXT, NULL, (LPARAM)pCreateData->handle);
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
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
                //APP_CallPhoneNumber(pCreateData->Store.pszPhone);
            }
            else
            {		
                offset = LOWORD(wparam);
                len = HIWORD(wparam);
                
                phonenum = malloc(len + 1);
                strncpy(phonenum, pCreateData->pText + offset,len);
                phonenum[len] = 0;	
                APP_CallPhoneNumber(phonenum);
                free(phonenum);
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
* Function	MailView_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MailView_OnCommand(HWND hWnd, int id, UINT codeNotify, LPARAM lParam)
{
    PMAIL_VIEWCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);

	if((HMENU)MAKEWPARAM(id, codeNotify) == pCreateData->hMenu)
	{
		MailView_OnInitmenu(hWnd);
		
		return;
	}
	
    switch(id)
	{   
    case IDM_MAIL_VIEW_GOTOURL:
    case IDM_MAIL_VIEW_ADDTOBM:
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
                strncpy(pUrl, pCreateData->pText + offset, len);
                pUrl[len] = 0;
                // open browser and go to high lighted web address...(goto url)
                // add highlighted web address to browser bookmark...(add to bk)
                if(id == IDM_MAIL_VIEW_GOTOURL)
                    App_WapRequestUrl(pUrl);
                else
                    WBM_ADD_FROMURL(pUrl, pCreateData->hFrameWnd);
                
                GARY_FREE(pUrl);
            }
        }
        break;

    case IDM_MAIL_VIEW_VIEWONMAP:
        break;

    case IDM_MAIL_VIEW_SETASDESTINATION:
        break;

    case IDM_MAIL_VIEW_SAVEASWAYPOINT:
        break;

    case IDM_MAIL_VIEW_SEND_SMS:
    case IDM_MAIL_VIEW_SEND_MMS:
    case IDM_MAIL_VIEW_SEND_EMAIL:
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
            strncpy(pAddr, pCreateData->pText + offset, len);

            pAddr[len] = 0;
            
            if (low == TVS_NUMBER)	// highlighted
            {
                if (id == IDM_MAIL_VIEW_SEND_SMS)
                    APP_EditSMS(MuGetFrame(), pAddr, NULL);
                else if (id == IDM_MAIL_VIEW_SEND_MMS)
                    APP_EditMMS(MuGetFrame(), hWnd, 0, MMS_CALLEDIT_MOBIL,pAddr);		
            }
            else if (low == TVS_EMAIL)
            {
                if (id == IDM_MAIL_VIEW_SEND_MMS)
                    APP_EditMMS(MuGetFrame(), hWnd, 0, MMS_CALLEDIT_MOBIL,pAddr);
                else if (id == IDM_MAIL_VIEW_SEND_EMAIL)
                {
                    GetLocalTime(&time);
                    CreateMailEditWnd(MuGetFrame(), pAddr, NULL, NULL, NULL, NULL, 
                        &time, -1, -1);
                }
            }
            free(pAddr);
        }
        break;

    case IDM_MAIL_VIEW_ATTACHMENTS:
        {            
            CreateMailDisplayWnd(pCreateData->hFrameWnd, hWnd, pCreateData->pAtt,
                pCreateData->ViewFile.AttNum, 0, 0);
        }
        break;
       
    case IDM_MAIL_VIEW_REPLY:
        {
            SYSTEMTIME time;

            GetLocalTime(&time);
            CreateMailEditWnd(MuGetFrame(), pCreateData->ViewFile.From, NULL, pCreateData->ViewFile.Subject, 
                pCreateData->pText, NULL, &time, -1, -1);
        }        
        break;

    case IDM_MAIL_REPLY_TOSENDER:
        {
            SYSTEMTIME time;
            
            GetLocalTime(&time);
            CreateMailEditWnd(MuGetFrame(), pCreateData->ViewFile.From, NULL, pCreateData->ViewFile.Subject, 
                pCreateData->pText, NULL, &time, -1, -1);
        }
        break;

    case IDM_MAIL_REPLY_TOALL:
        {
            SYSTEMTIME time;
            char *TempName;
            char *ToName;
            char *CcName;
            int NameLen;
            
            GetLocalTime(&time);
            if(MAIL_AnalyseMailAddr(pCreateData->ViewFile.To) > 0)
            {
                //To
                NameLen = strlen(pCreateData->ViewFile.From) + strlen(pCreateData->ViewFile.To);
                TempName = malloc(NameLen + 2);
                if(TempName == NULL)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                    break;
                }
                memset(TempName, 0x0, NameLen);
                strcpy(TempName, pCreateData->ViewFile.From);
                if(*(char *)(TempName + strlen(pCreateData->ViewFile.From)) != ';')
                {
                    strcat(TempName, ";");
                }
                strcat(TempName, pCreateData->ViewFile.To);
                if(!MAIL_CombineReplyRec(&ToName, TempName, pCreateData->ViewFile.OwnAddress))
                {
                    GARY_FREE(TempName);

                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                    break;
                }
                GARY_FREE(TempName);

                //cc
                NameLen = strlen(pCreateData->ViewFile.Cc);
                TempName = malloc(NameLen + 1);
                if(TempName == NULL)
                {
                    GARY_FREE(ToName);

                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                    break;
                }
                memset(TempName, 0x0, NameLen);
                strcpy(TempName, pCreateData->ViewFile.Cc);
                if(!MAIL_CombineReplyRec(&CcName, TempName, pCreateData->ViewFile.OwnAddress))
                {
                    GARY_FREE(ToName);
                    GARY_FREE(TempName);
                    
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                    break;
                }
                GARY_FREE(TempName);

                CreateMailEditWnd(MuGetFrame(), ToName, CcName, pCreateData->ViewFile.Subject, 
                    pCreateData->pText, NULL, &time, -1, -1);
            }
            else
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECIERROR, NULL,
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            }
        }
        break;
        
    case IDM_MAIL_VIEW_FORWARD:
        {
            SYSTEMTIME time;
            char *szNewAtt;

            szNewAtt = NULL;
            szNewAtt = (char *)malloc(pCreateData->ViewFile.AttFileNameLen + 1);
            if(szNewAtt == NULL)
            {
                break;
            }

            GetLocalTime(&time);
            MAIL_CopyAttachment(szNewAtt, pCreateData->pAtt, pCreateData->ViewFile.AttNum);
            CreateMailEditWnd(MuGetFrame(), NULL, NULL, pCreateData->ViewFile.Subject, 
                pCreateData->pText, szNewAtt, &time, -1, -1);
            GARY_FREE(szNewAtt);
        }
        break;
 
    case IDM_MAIL_VIEW_ADDTOCONTACTS_UPDATE:
    case IDM_MAIL_VIEW_ADDTOCONTACTS_CREATE:
        {
            int		ret = 0, offset = 0,len = 0;
            DWORD   high = 0, low = 0;
            ABNAMEOREMAIL ABName;
            int     nMode;
            
            if(id == IDM_MAIL_VIEW_ADDTOCONTACTS_UPDATE)
                nMode = AB_UPDATE;
            else
                nMode = AB_NEW;
            
            memset(&ABName, 0, sizeof(ABNAMEOREMAIL));
            
            ret = SendMessage(pCreateData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);
            
            if(ret == -1 )
            {
                if(strcmp(pCreateData->ViewFile.To, pCreateData->ViewFile.address) == 0)//pCreateData->folderid == MU_DRAFT
                {
                    strcpy(ABName.szTelOrEmail,pCreateData->ViewFile.To);
                }
                else if(strcmp(pCreateData->ViewFile.From, pCreateData->ViewFile.address) == 0)//pCreateData->folderid == MU_BOXMAIL
                {
                    strcpy(ABName.szTelOrEmail,pCreateData->ViewFile.From);
                }
                ABName.nType = AB_EMAIL;
                
                APP_SaveToAddressBook(pCreateData->hFrameWnd, NULL, NULL, &ABName, nMode);
                break;
            }
            // get highlighted items
            offset = LOWORD(high);
            len = HIWORD(high);
            
            strncpy(ABName.szTelOrEmail, pCreateData->pText + offset, len);
            
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
            
            APP_SaveToAddressBook(pCreateData->hFrameWnd, NULL, 0, &ABName, nMode);
        }
        break;
   
    case IDM_MAIL_VIEW_FIND_PHONE:
        {
            int ret = 0;
            
            ret = SendMessage(pCreateData->hWndTextView, TVM_FINDNHL, 0, TVS_NUMBER);
            if (ret == -1)		// search found no results
            {
                char chCaption[50];

                memset(chCaption, 0x0, 50);
                GetWindowText(pCreateData->hFrameWnd, chCaption, 49);
                PLXTipsWin(NULL, NULL, 0, ML("No matches found"), chCaption,
                Notify_Info, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            }
        }
        break;

    case IDM_MAIL_VIEW_FIND_EMAIL: 
        {
            int ret = 0;
            
            ret = SendMessage(pCreateData->hWndTextView, TVM_FINDNHL, 0, TVS_EMAIL);
            if (ret == -1)		// search found no results
            {
                char chCaption[50];
                
                memset(chCaption, 0x0, 50);
                GetWindowText(pCreateData->hFrameWnd, chCaption, 49);
                PLXTipsWin(NULL, NULL, 0, ML("No matches found"), chCaption,
                    Notify_Info, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            }
        }
        break;

    case IDM_MAIL_VIEW_FIND_WEB: 
        {
            int ret = 0;
            
            ret = SendMessage(pCreateData->hWndTextView, TVM_FINDNHL, 0, TVS_URL);
            if (ret == -1)		// search found no results
            {
                char chCaption[50];
                
                memset(chCaption, 0x0, 50);
                GetWindowText(pCreateData->hFrameWnd, chCaption, 49);
                PLXTipsWin(NULL, NULL, 0, ML("No matches found"), chCaption,
                    Notify_Info, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            }
        }
        break;

    case IDM_MAIL_VIEW_FIND_COORDINATE: 
        {
            int ret = 0;
            
            ret = SendMessage(pCreateData->hWndTextView, TVM_FINDNHL, 0, TVS_COORDINATE);
            if (ret == -1)		// search found no results
            {
                char chCaption[50];
                
                memset(chCaption, 0x0, 50);
                GetWindowText(pCreateData->hFrameWnd, chCaption, 49);
                PLXTipsWin(NULL, NULL, 0, ML("No matches found"), chCaption,
                    Notify_Info, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            }
        }
        break;

    case IDM_MAIL_VIEW_HIDEFOUNDITEMS:
		// remove highlights from the msg body
		SendMessage(pCreateData->hWndTextView, TVM_HIDEHL, 0, 0);
        break;

    case IDM_MAIL_VIEW_MOVETOFOLDER:
        if(!MU_FolderSelection(hWnd, hWnd, WM_MAIL_FOLDER_RETURN, pCreateData->folderid))      
            return;
        break;

    case IDM_MAIL_VIEW_MESSAGEINFO:
        CreateMailDetailWnd(pCreateData->hFrameWnd, &pCreateData->ViewFile);
        break;
        

    case IDM_MAIL_VIEW_DELETE://more attention
        {
            PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_MAIL_STRING_DELETEMSG,
                Notify_Request, NULL, IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_DELETE_RETURN);
        }
        break;

    case IDM_MAIL_BUTTON_EXIT:
        {
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;

    case ID_MAILBOX_MODIFY:
        {
            if(lParam != NULL)
            {
                char szOutFile[PATH_MAXLEN];
                int hFile;

                pCreateData->ViewFile.AttFileNameLen = ((MailAttString *)lParam)->AttLen;
                pCreateData->ViewFile.AttNum = ((MailAttString *)lParam)->AttNum;
                memset(pCreateData->pAtt, 0x0, pCreateData->ViewFile.AttFileNameLen + 1);
                strcpy(pCreateData->pAtt, ((MailAttString *)lParam)->AttName);

                //save to flash
                memset(szOutFile, 0x0, PATH_MAXLEN);
                getcwd(szOutFile, PATH_MAXLEN);
                chdir(MAIL_FILE_PATH);
                
                hFile = GARY_open(pCreateData->szFileName, O_RDWR, -1);
                if(hFile < 0)
                {
                    chdir(szOutFile);
                    return;
                }
                write(hFile, &pCreateData->ViewFile, sizeof(MailFile));
                if(pCreateData->ViewFile.AttFileNameLen != 0)
                {
                    lseek(hFile, sizeof(MailFile) + pCreateData->ViewFile.TextSize, SEEK_SET);
                    write(hFile, pCreateData->pAtt, pCreateData->ViewFile.AttFileNameLen);
                }
                else
                {
                    SendMessage(pCreateData->hParent, WM_MAIL_DELETE_RETURN, 0, 0);
                }
                GARY_close(hFile);
                chdir(szOutFile);
            }
        }
        break;

    default:
        break;
	}

	return;
}

/*********************************************************************\
* Function	MailView_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailView_OnDestroy(HWND hWnd)
{
	
    PMAIL_VIEWCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);

    GARY_FREE(pCreateData->pText);
    GARY_FREE(pCreateData->pAtt);
    //UnRegisterMailViewClass();

    if(pCreateData->hAttIcon)
    {
        DeleteObject(pCreateData->hAttIcon);
        pCreateData->hAttIcon = NULL;
    }
    if(pCreateData->AttDCIcon)
    {
        DeleteDC(pCreateData->AttDCIcon);
        pCreateData->AttDCIcon = NULL;
    }

    /*DeleteObject(pCreateData->hLeftIcon);
    pCreateData->hLeftIcon = NULL;
    DeleteObject(pCreateData->hRightIcon);
    pCreateData->hRightIcon = NULL;*/

#ifdef MAIL_DEBUG
    EndObjectDebug();
#endif
    
    return;

}
/*********************************************************************\
* Function	MailView_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailView_OnClose(HWND hWnd)
{
	SendMessage(GetParent(hWnd), PWM_CLOSEWINDOW, (WPARAM)hWnd, NULL);

    DestroyWindow(hWnd);
    
    return;

}
/*********************************************************************\
* Function     MailView_OnMoveToFolder
* Purpose      
* Params      
* Return       
**********************************************************************/
static void MailView_OnMoveToFolder(HWND hWnd, BOOL bMove, int nNewFolder)
{
    PMAIL_VIEWCREATEDATA pCreateData;
    
    if(bMove == FALSE)
        return;
    
    if(nNewFolder == -1)
        return;

    pCreateData = GetUserData(hWnd);

    if(pCreateData->folderid != MU_BOXMAIL)
    {
        if(mail_move_message(pCreateData->hParent, pCreateData->handle, nNewFolder))
        {
            SendMessage(pCreateData->hParent, PWM_MSG_MU_DELETE, 
                MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)pCreateData->handle);
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MSGMOVED, NULL, 
                Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        }
        else
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MOVEFAIL, NULL, 
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        }
        PostMessage(hWnd,WM_CLOSE,0,0);
    }
    else
    {
        PMAIL_INBOXCREATEDATA pInboxData;

        pInboxData = GetUserData(pCreateData->hParent);
        pInboxData->bConfirm = Con_MoveOne;
        
        MailInBox_OnFolderReturn(pCreateData->hParent, bMove, nNewFolder);
    }
}

/*********************************************************************\
* Function     MailView_OnDeleteRet
* Purpose      
* Params      
* Return       
**********************************************************************/
static void MailView_OnDeleteRet(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMAIL_VIEWCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);

    switch(lParam)
    {
    case 0:
        break;

    case 1:
        {
            if(pCreateData->folderid != MU_BOXMAIL)
            {
                if(mail_delete_message(pCreateData->hParent, pCreateData->handle))
                {
                    SendMessage(pCreateData->hParent, PWM_MSG_MU_DELETE, 
                        MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)pCreateData->handle);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELETED, NULL,
                        Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                }
                else
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DELFAIL, NULL,
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                }
            }
            else
            {
                PMAIL_INBOXCREATEDATA pInboxData;
                HWND hList;
                int CurSel;
                MailGetListNode *pCurNode;
                
                pInboxData = GetUserData(pCreateData->hParent);
                hList = GetDlgItem(pCreateData->hParent, IDC_MAIL_BOXMAIL_LIST);
                
                CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
                pCurNode = NULL;
                MAIL_GetGetNodebyInt(&pInboxData->GetListHead, &pCurNode, CurSel);
                
                pInboxData->bConfirm = Con_DelSelect;
                pInboxData->DelNode = pCurNode;
                SendMessage(pCreateData->hParent, WM_MAIL_BOX_RETURN, 0, 0);
            }
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;
    }

    return;
}

/*********************************************************************\
* Function	   MAIL_SetViewIndicator
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void MAIL_SetViewIndicator(HWND hWnd, int nFrag)
{
	HDC			hdc = NULL;
	HBITMAP		hBitmap = NULL;
	char        szFrag[16];
	PMAIL_VIEWCREATEDATA  pCreateData;
    HFONT       hFont = NULL;
	RECT        rect;
	SIZE        sz1;//, sz2;
	int         nOldMode = 0;
    //BITMAP      tmpbm;

	pCreateData = GetUserData(hWnd);
	
	hdc = GetDC(hWnd);

	if (pCreateData->AttDCIcon == NULL)
		pCreateData->AttDCIcon = CreateCompatibleDC(hdc);

	GetFontHandle(&hFont, SMALL_FONT);
    SelectObject(pCreateData->AttDCIcon, hFont);

    if(nFrag > 0)
    {
        sprintf(szFrag, "%d", nFrag);
        GetTextExtentPoint(pCreateData->AttDCIcon, szFrag, -1, &sz1);
    }

	SetRect(&rect, 0, 0, LR_ICON_WIDTH, LR_ICON_HEIGHT);
	
	if (pCreateData->hAttIcon != NULL)
    {
        DeleteObject(pCreateData->hAttIcon);
        pCreateData->hAttIcon = NULL;
    }

    pCreateData->hAttIcon = CreateCompatibleBitmap(pCreateData->AttDCIcon, 
		rect.right, rect.bottom);

    SelectObject(pCreateData->AttDCIcon, pCreateData->hAttIcon);
	ClearRect(pCreateData->AttDCIcon, &rect, COLOR_TRANSBK);

	nOldMode = SetBkMode(pCreateData->AttDCIcon, TRANSPARENT);

    //GetObject(pCreateData->hLeftIcon, sizeof(BITMAP), &tmpbm);
    if(nFrag > 0)
    {
        TextOut(pCreateData->AttDCIcon, 0, 0, szFrag, -1);
        /*if(pCreateData->bNext)
        {
            StretchBlt(pCreateData->AttDCIcon, 0, LR_ICON_HEIGHT/2, LR_ICON_WIDTH,
                LR_ICON_HEIGHT/2, (HDC)pCreateData->hRightIcon, 0, 0, 
                tmpbm.bmWidth, tmpbm.bmHeight, ROP_SRC);
        }
    }
    else
    {
        StretchBlt(pCreateData->AttDCIcon, 0, 0, LR_ICON_WIDTH,
            LR_ICON_HEIGHT, (HDC)pCreateData->hRightIcon, 0, 0, 
            tmpbm.bmWidth, tmpbm.bmHeight, ROP_SRC);*/
    }
	
	SetBkMode(pCreateData->AttDCIcon, nOldMode);
	ReleaseDC(hWnd, hdc);

    return;
}

void MAIL_SetViewNumber(HWND hWnd, int leftNum, int rightNum)
{
    PMAIL_VIEWCREATEDATA  pCreateData;
    
    pCreateData = GetUserData(hWnd);

    if(rightNum > 0)
    {
        MAIL_SetViewIndicator(hWnd, rightNum);
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)pCreateData->hAttIcon);
    }
    /*else if(pCreateData->bNext)
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)MAIL_ICON_RIGHT);
    }*/
    else
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
    }

    return;
}
