/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailDetail.c
*
* Purpose  : 
*
\**************************************************************************/

#include "MailHeader.h"

static HWND HwndMailDetail = NULL;
static HWND HwndMailDisplay = NULL;
static HWND hFrameWnd;

//detail
BOOL CreateMailDetailWnd(HWND hParent, MailFile *szOutFile);
static LRESULT MailDetailWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MailDetail_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MailDetail_OnActivate(HWND hWnd, UINT state);
static void MailDetail_OnPaint(HWND hWnd);
static void MailDetail_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MailDetail_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void MailDetail_OnDestroy(HWND hWnd);
static void MailDetail_OnClose(HWND hWnd);

//attachment
BOOL CreateMailDisplayWnd(HWND hFrameWnd, HWND hParent, char *pAttHead, int AttNum, int nAttSize, BOOL bEdit);
static LRESULT MailDisplayWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MailDisplay_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MailDisplay_OnActivate(HWND hWnd, UINT state);
static void MailDisplay_OnInitmenu(HWND hWnd);
static void MailDisplay_OnPaint(HWND hWnd);
static void MailDisplay_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MailDisplay_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void MailDisplay_OnDestroy(HWND hWnd);
static void MailDisplay_OnClose(HWND hWnd);
static void MailDisplay_OnInsertAtt(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void MailDisplay_OnDeleteRet(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void MailDisplay_OnMusicOff(HWND hWnd, WPARAM wParam, LPARAM lParam);

void MAIL_ReleaseDisList(MailAttachmentHead *pHead);
void MAIL_AddDisNode(MailAttachmentHead *pHead, MailAttachmentNode *pTempNode);
void MAIL_DelAttachmentNode(MailAttachmentHead *pHead, MailAttachmentNode *pNode);
static void MAIL_DelDisNode(MailAttachmentHead *pHead, int nTempNode);

static BOOL MAIL_SaveToPath(HWND hWnd);
static void MAIL_FreeObject(HWND hWnd);

extern void MAIL_SetNumber(HWND hWnd, int leftNum, int rightNum);
extern void MAIL_SetViewNumber(HWND hWnd, int leftNum, int rightNum);
extern BOOL MMS_SaveObject(HWND hWndFrame, HWND hWnd, int msg, PCSTR title, 
                           PCSTR pName, PCSTR pdata, int datalen);
extern BOOL MMS_SelectDestination(HWND hWndFrame, HWND hWnd, int msg, PCSTR title, 
                                  PCSTR pName, PCSTR pdata, int datalen, int metaType);
extern BOOL CALE_SaveFromSms (vCal_Obj *pObj);
/*********************************************************************\
* Function	   CreateMailDetailWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL CreateMailDetailWnd(HWND hParent, MailFile *szOutFile)
{
    WNDCLASS wc;
    RECT rClient;

#ifdef MAIL_DEBUG
    StartObjectDebug();
#endif
    
    wc.style         = 0;
    wc.lpfnWndProc   = MailDetailWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MailFile);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "MailDetailWndClass";
    
    RegisterClass(&wc);
    /*if (!RegisterClass(&wc))
        return FALSE;*/

    hFrameWnd = hParent;
    GetClientRect(hFrameWnd, &rClient);

    HwndMailDetail = CreateWindow(
        "MailDetailWndClass", 
        "",//IDP_MAIL_TITLE_MSGINFO,
        WS_VISIBLE | WS_CHILD,//PWS_STATICBAR | WS_VISIBLE | WS_CAPTION,
        rClient.left, 
        rClient.top, 
        rClient.right - rClient.left,
        rClient.bottom - rClient.top,
        hFrameWnd, //hParent,
        NULL,
        NULL, 
        (PVOID)szOutFile
        );
    
    if (!HwndMailDetail)
    {
        UnregisterClass("MailDetailWndClass", NULL);
        return FALSE;
    }
             
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_OK, 1), 
        (LPARAM)IDP_MAIL_BOOTEN_OK);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
    SetWindowText(hFrameWnd, IDP_MAIL_TITLE_MSGINFO);
    ShowWindow(hFrameWnd, SW_SHOW);
    UpdateWindow(hFrameWnd);

    return TRUE;
}

/*********************************************************************\
* Function	MailDetailWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT MailDetailWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MailDetail_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        MailDetail_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;
        
    case WM_PAINT:
        MailDetail_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        MailDetail_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        MailDetail_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        {
            SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            MailDetail_OnClose(hWnd);
        }
        break;

    case WM_DESTROY:
        MailDetail_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;
}
/*********************************************************************\
* Function	MailDetail_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MailDetail_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    MailFile *pCreateData;
    HWND hFromView = NULL;
    int  nIndex = 0, nKilobyte = 0;
    char szData[20], szTime[20], szSize[20];
    RECT rect;
    LISTBUF Listtmp;

    ABNAMEOREMAIL ABName;
    char  Array[MAX_MAIL_RECIPIENT_NUM * 102];    
    char *p1, *p;
    int nCount = 0;
    int i;
    char *Dis1, *Dis2;
    int nlen;
    BOOL DisFrom;

    pCreateData = GetUserData(hWnd);

    memcpy(pCreateData, lpCreateStruct->lpCreateParams, sizeof(MailFile));
    GetClientRect(hFrameWnd, &rect);
        
	PREBROW_InitListBuf(&Listtmp);
    
    Dis1 = NULL;
    Dis2 = NULL;
    memset(Array, 0, MAX_MAIL_RECIPIENT_NUM * 102);

    if(strcmp(pCreateData->address, pCreateData->From) == 0)
    {
        p1 = p = pCreateData->From;
        DisFrom = TRUE;
    }
    else if(strcmp(pCreateData->address, pCreateData->To) == 0)
    {
        p1 = p = pCreateData->To;
        DisFrom = FALSE;
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
                break;
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
        strcat(Dis1, ";");
    }
    Dis1[nlen] = '\0';

    if(!DisFrom)
    {
        p1 = p = pCreateData->Cc;

        memset(Array, 0, MAX_MAIL_RECIPIENT_NUM * 102);

        nCount = 0;
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
                    break;
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
        
        Dis2 = (char *)malloc(nlen + 1);
        if(Dis2 == NULL)
        {
            GARY_FREE(Dis1);
            return FALSE;
        }
        
        memset(Dis2, 0, nlen + 1);
        
        for(i = 0 ; i < nCount ; i++)
        {
            strcat(Dis2, Array + (i * 102));
            strcat(Dis2, ";");
        }
        Dis2[nlen] = '\0';
    }
    
    if(DisFrom)
    {
        if(Dis1 == NULL)
        {
            PREBROW_AddData(&Listtmp, IDP_MAIL_TITLE_FROM, "");
        }
        else
        {
            PREBROW_AddData(&Listtmp, IDP_MAIL_TITLE_FROM, Dis1);
        }
    }
    else
    {
        if(Dis1 == NULL)
        {
            PREBROW_AddData(&Listtmp, IDP_MAIL_TITLE_TO, "");
        }
        else
        {
            PREBROW_AddData(&Listtmp, IDP_MAIL_TITLE_TO, Dis1);
        }

        if(Dis2 == NULL)
        {
            PREBROW_AddData(&Listtmp, IDP_MAIL_TITLE_CC, "");
        }
        else
        {
            PREBROW_AddData(&Listtmp, IDP_MAIL_TITLE_CC, Dis2);
        }
    }
    
    PREBROW_AddData(&Listtmp, IDP_MAIL_TITLE_SUBJECT, pCreateData->Subject);
    
    memset(szData, 0x0, 20);
    memset(szTime, 0x0, 20);
    GetTimeDisplay(pCreateData->LocalDate, szTime, szData);

    PREBROW_AddData(&Listtmp, IDP_MAIL_TITLE_DATE, szData);
    PREBROW_AddData(&Listtmp, IDP_MAIL_TITLE_TIME, szTime);
    PREBROW_AddData(&Listtmp, IDP_MAIL_TITLE_TYPE, IDP_MAIL_TITLE_EMAIL);

    szSize[0] = 0;
    memset(szSize, 0x0, 20);
    nKilobyte = pCreateData->MailSize / 1024;
    if(nKilobyte > 0)
        sprintf(szSize, "%d KB", nKilobyte);
    else
    {
        nKilobyte = pCreateData->MailSize * 10 / 1024;
        if(nKilobyte > 0)
            sprintf(szSize, "0.%d KB", nKilobyte);
        else
        {
            nKilobyte = pCreateData->MailSize * 100 / 1024;
            sprintf(szSize, "0.0%d KB", nKilobyte);
        }
    }
    PREBROW_AddData(&Listtmp, IDP_MAIL_TITLE_SIZE, szSize);

	hFromView = CreateWindow(
        FORMVIEWER,
        "",
		WS_VISIBLE | WS_CHILD | WS_VSCROLL,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
		hWnd,
        (HMENU)IDC_MAIL_VIEW_LIST,
        NULL, 
        (PVOID)&Listtmp);

	if (hFromView == NULL)
    {
        GARY_FREE(Dis1);
        GARY_FREE(Dis2);
        return FALSE;
    }

    GARY_FREE(Dis1);
    GARY_FREE(Dis2);

    SetFocus(hFromView);
    
    return TRUE;
}
/*********************************************************************\
* Function	MailDetail_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MailDetail_OnActivate(HWND hWnd, UINT state)
{
    HWND hList;

    hList = GetDlgItem(hWnd, IDC_MAIL_VIEW_LIST);
    //if(state == WA_ACTIVE)
    SetFocus(hList);

    SetWindowText(hFrameWnd, IDP_MAIL_TITLE_MSGINFO);

    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BOOTEN_OK);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return;
}

/*********************************************************************\
* Function	MailDetail_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailDetail_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	MailDetail_OnKey
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
static void MailDetail_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
	switch (vk)
	{
    case VK_RETURN:
        {
            PostMessage(hWnd, WM_CLOSE, NULL, NULL);
        }
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	MailDetail_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MailDetail_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{
    case IDM_MAIL_BUTTON_OK:
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
* Function	MailDetail_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailDetail_OnDestroy(HWND hWnd)
{
    MailFile *pCreateData;

    pCreateData = GetUserData(hWnd);
    HwndMailDetail = NULL;
	//UnregisterClass("MailDetailWndClass", NULL);

#ifdef MAIL_DEBUG
    EndObjectDebug();
#endif
	
    return;
}
/*********************************************************************\
* Function	MailDetail_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailDetail_OnClose(HWND hWnd)
{
    DestroyWindow(hWnd);

    return;
}

/*******************DisplayWindow*********************************/
/*********************************************************************\
* Function	   CreateMailDisplayWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL CreateMailDisplayWnd(HWND hFrameWnd, HWND hParent, char *pAttHead, int AttNum, int nAttSize, BOOL bEdit)
{
    MAIL_DISPLAYCREATEDATA CreateData;

    WNDCLASS wc;
    RECT rClient;

    int i;
    MailAttachmentNode *pAttNode;
        
#ifdef MAIL_DEBUG
    StartObjectDebug();
#endif

    wc.style         = 0;
    wc.lpfnWndProc   = MailDisplayWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MAIL_DISPLAYCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "MailDisplayWndClass";

    RegisterClass(&wc);
    /*if (!RegisterClass(&wc))
        return FALSE;*/

    memset(&CreateData, 0x0, sizeof(MAIL_DISPLAYCREATEDATA));
    CreateData.hFrameWnd = hFrameWnd;
    CreateData.hParent = hParent;
    CreateData.bEdit = bEdit;
    CreateData.AttNum = AttNum;
    CreateData.nAttSize = nAttSize;

    for(i = 0; i < AttNum; i++)
    {
        pAttNode = NULL;
        pAttNode = (MailAttachmentNode *)malloc(sizeof(MailAttachmentNode));
        if(pAttNode == NULL)
        {
            MAIL_ReleaseDisList(&CreateData.AttHead);
            //UnregisterClass("MailDisplayWndClass", NULL);
            return FALSE;
        }

        memset(pAttNode, 0x0, sizeof(MailAttachmentNode));
        MAIL_GetMailAttachmentShowName(pAttHead, pAttNode->AttShowName, i);
        MAIL_GetMailAttachmentSaveName(pAttHead, pAttNode->AttSaveName, i);
        pAttNode->AttSize = MAIL_GetMailAttachmentSize(pAttNode->AttSaveName);
        MAIL_IsAttSupported(pAttNode->AttShowName, NULL, &pAttNode->AttType);
        pAttNode->pNext = NULL;

        MAIL_AddDisNode(&CreateData.AttHead, pAttNode);
    }
    CreateData.hMenu = CreateMenu();

    GetClientRect(hFrameWnd, &rClient);

    HwndMailDisplay = CreateWindow(
        "MailDisplayWndClass", 
        "",
        WS_VISIBLE | WS_CHILD,
        rClient.left, 
        rClient.top, 
        rClient.right - rClient.left,
        rClient.bottom - rClient.top,
        hFrameWnd,
        NULL,
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!HwndMailDisplay)
    {
        UnregisterClass("MailDisplayWndClass", NULL);
        return FALSE;
    }

    PDASetMenu(hFrameWnd, CreateData.hMenu);

    if(CreateData.AttHead.pNext != NULL)
    {
        if(CreateData.AttHead.pNext->AttType == INSERT_RING)
        {
            SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_OK, 1), 
                (LPARAM)IDP_MAIL_BUTTON_PLAY);
        }
        else
        {
            SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_OK, 1), 
                (LPARAM)IDP_MAIL_BUTTON_OPEN);
        }
    }
    else
    {
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_OK, 1), 
            (LPARAM)IDP_MAIL_BUTTON_OPEN);
    }
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), 
        (LPARAM)IDP_MAIL_BUTTON_BACK);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    SendMessage(hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    SetWindowText(hFrameWnd, IDP_MAIL_BUTTON_ATTACHMENT);
    SendMessage(CreateData.hParent, WM_MAIL_DIS_RETURN, 
        (WPARAM)CreateData.nAttSize, (LPARAM)CreateData.AttNum);
    ShowWindow(hFrameWnd, SW_SHOW);
    UpdateWindow(hFrameWnd);

    return TRUE;
}

/*********************************************************************\
* Function	MailDisplayWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT MailDisplayWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MailDisplay_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        MailDisplay_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;
        
    case WM_INITMENU:
        MailDisplay_OnInitmenu(hWnd);
        break;

    case WM_PAINT:
        MailDisplay_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        MailDisplay_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        {
            PMAIL_DISPLAYCREATEDATA pCreateData;
            
            pCreateData = GetUserData(hWnd);
            
            if(wParam == (WPARAM)GetMenu(pCreateData->hFrameWnd))
            {
                //WM_INITMENU
                MailDisplay_OnInitmenu(hWnd);
                break;
            }
            MailDisplay_OnCommand(hWnd, (int)(LOWORD(wParam)), (UINT)HIWORD(wParam));
        }
        break;
        
    case WM_CLOSE:
        MailDisplay_OnClose(hWnd);
        break;

    case WM_DESTROY:
        MailDisplay_OnDestroy(hWnd);
        break;

    case WM_MAIL_INSERT_RETURN:
        MailDisplay_OnInsertAtt(hWnd, wParam, lParam);
        break;
        
    case WM_MAIL_SAVEOBJ_RETURN:
        MAIL_FreeObject(hWnd);
        break;

    case WM_MAIL_DELETE_RETURN:
        MailDisplay_OnDeleteRet(hWnd, wParam, lParam);
        break;

    case PRIOMAN_MESSAGE_READDATA:
        {
            PrioMan_ReadData(wParam, lParam);
        }
        break;

    case PRIOMAN_MESSAGE_WRITEDATA:
        {
            PrioMan_WriteData(wParam, lParam);
        }
        break;

    case PRIOMAN_MESSAGE_PLAYOVER:
        {
            PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_EMAIL);
            MailDisplay_OnMusicOff(hWnd, wParam, lParam);
        }
        break;
        
    case PRIOMAN_MESSAGE_BREAKOFF:
        {
            PrioMan_BreakOffMusic(PRIOMAN_PRIORITY_EMAIL);
            MailDisplay_OnMusicOff(hWnd, wParam, lParam);
        }
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;
}
/*********************************************************************\
* Function	MailDisplay_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MailDisplay_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    PMAIL_DISPLAYCREATEDATA pCreateData;
    RECT rect;
    HWND  hWndObjList;

    MailAttachmentNode *pTemp;
    int index;

    pCreateData = GetUserData(hWnd);

    memcpy(pCreateData, lpCreateStruct->lpCreateParams, sizeof(MAIL_DISPLAYCREATEDATA));
    GetClientRect(pCreateData->hFrameWnd, &rect);

    pCreateData->hBmpObj[0] = LoadImage(NULL, MAIL_ICON_PICTURE, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hBmpObj[1] = LoadImage(NULL, MAIL_ICON_SOUND, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hBmpObj[2] = LoadImage(NULL, MAIL_ICON_NODE, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hBmpObj[3] = LoadImage(NULL, MAIL_ICON_NODE, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hBmpObj[4] = LoadImage(NULL, MAIL_ICON_VCAL, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
    pCreateData->hBmpObj[5] = LoadImage(NULL, MAIL_ICON_OTHER, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);

    hWndObjList = CreateWindow(
        "LISTBOX",
        "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP | LBS_MULTILINE,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_MAIL_VIEW_LIST,
        NULL,
        NULL ); 
    
    if (NULL == hWndObjList)
        return FALSE;
    
    pTemp = pCreateData->AttHead.pNext;
    while(pTemp)
    {
        int AttSize;
        char pItem[8];

        memset(pItem, 0x0, 8);
        if ((AttSize = pTemp->AttSize/1024) == 0)
        {
            AttSize = pTemp->AttSize * 10 / 1024;
            sprintf(pItem, "0.%d K", AttSize);
        }
        else
        {
            sprintf(pItem, "%d K", AttSize);
        }

        index = SendMessage(hWndObjList, LB_ADDSTRING, 0, (LPARAM)pTemp->AttShowName);
        if(pTemp->AttType != -1)
        {
            SendMessage(hWndObjList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)index),
                (LPARAM)pCreateData->hBmpObj[pTemp->AttType]);
        }
        else
        {
            SendMessage(hWndObjList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)index),
                (LPARAM)pCreateData->hBmpObj[5]);
        }
        SendMessage(hWndObjList, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)pItem);
        
        pTemp = pTemp->pNext;
    }
    
    SetFocus(hWndObjList);
    SendMessage(hWndObjList, LB_SETCURSEL, 0, 0);
    
    return TRUE;
}
/*********************************************************************\
* Function	MailDisplay_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MailDisplay_OnActivate(HWND hWnd, UINT state)
{
    HWND hList;
    PMAIL_DISPLAYCREATEDATA pCreateData;
    int CurSel;
    int i;
    MailAttachmentNode *pCurNode;
    
    pCreateData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_VIEW_LIST);
    
    CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
    if(CurSel > pCreateData->AttHead.Num - 1)
    {
        return;
    }

    pCurNode = pCreateData->AttHead.pNext;
    for(i=0; i<CurSel; i++)
    {
        pCurNode = pCurNode->pNext;
    }
    
    SendMessage(hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    if(IsWindow(pCreateData->hTextView)) //&& (IsWindowVisible(pCreateData->hTextView)))
    {
        SetFocus(pCreateData->hTextView);

        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_BACK);
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        
        SetWindowText(pCreateData->hFrameWnd, pCurNode->AttShowName);
    }
    else if(pCreateData->AttNum == 0)
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_BACK);
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_BUTTON_ATTACHMENT);
        SendMessage(pCreateData->hParent, WM_MAIL_DIS_RETURN, 
            (WPARAM)pCreateData->nAttSize, (LPARAM)pCreateData->AttNum);
    }
    else
    {
        SetFocus(hList);

        PDASetMenu(pCreateData->hFrameWnd, pCreateData->hMenu);

        if(pCurNode->AttType == INSERT_RING)
        {
            if(pCreateData->bPlaySound)
            {
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_STOP);
            }
            else
            {
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_PLAY);
            }
        }
        else
        {
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_OPEN);
        }
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_BACK);
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
        
        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_BUTTON_ATTACHMENT);
        SendMessage(pCreateData->hParent, WM_MAIL_DIS_RETURN, 
            (WPARAM)pCreateData->nAttSize, (LPARAM)pCreateData->AttNum);
    }

    return;
}

/*********************************************************************\
* Function	MailDisplay_OnInitmenu
* Purpose   WM_INITMENU message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailDisplay_OnInitmenu(HWND hWnd)
{
    PMAIL_DISPLAYCREATEDATA pCreateData;
    HMENU hMenu;
    HMENU hInsertMenu;
    int   nMenuItem = 0;
    
    HWND hList;
    int CurSel;
    int i;
    MailAttachmentNode *pCurNode;
    
    pCreateData = GetUserData(hWnd);
    hMenu = pCreateData->hMenu;
    hList = GetDlgItem(hWnd, IDC_MAIL_VIEW_LIST);
    
    CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
    if(CurSel > pCreateData->AttHead.Num - 1)
    {
        return;
    }
    
    pCurNode = pCreateData->AttHead.pNext;
    for(i=0; i<CurSel; i++)
    {
        pCurNode = pCurNode->pNext;
    }    
    
    nMenuItem = GetMenuItemCount(hMenu);
    while(nMenuItem > 0)
    {
        nMenuItem--;
        DeleteMenu(hMenu, nMenuItem, MF_BYPOSITION);
    }

    if((IsWindow(pCreateData->hTextView)) && (IsWindowVisible(pCreateData->hTextView)))
    {
        return;
    }

    if(pCreateData->AttNum)
    {
        if(pCurNode->AttType == INSERT_RING)
        {
            if(pCreateData->bPlaySound)
            {
                AppendMenu(hMenu, MF_ENABLED, IDM_MAIL_BUTTON_OPEN, IDP_MAIL_BUTTON_STOP);
            }
            else
            {
                AppendMenu(hMenu, MF_ENABLED, IDM_MAIL_BUTTON_OPEN, IDP_MAIL_BUTTON_PLAY);
            }
        }
        else
        {
            AppendMenu(hMenu, MF_ENABLED, IDM_MAIL_BUTTON_OPEN, IDP_MAIL_BUTTON_OPEN);
        }
    }
    if(pCreateData->bEdit)
    {
        hInsertMenu = CreateMenu();
        
        AppendMenu(hInsertMenu, MF_ENABLED | MF_STRING, IDM_MAIL_INSERT_PICTURE, IDP_MAIL_BUTTON_PICTURE);
        AppendMenu(hInsertMenu, MF_ENABLED | MF_STRING, IDM_MAIL_INSERT_SOUND, IDP_MAIL_BUTTON_SOUND);
        AppendMenu(hInsertMenu, MF_ENABLED | MF_STRING, IDM_MAIL_INSERT_NOTE, IDP_MAIL_BUTTON_NOTE);
        
        AppendMenu(hMenu, MF_STRING | MF_ENABLED | MF_POPUP, (UINT_PTR)hInsertMenu, IDP_MAIL_BUTTON_INSERT);
    }
    else if(pCreateData->AttNum)
    {
        AppendMenu(hMenu, MF_ENABLED, IDM_MAIL_BUTTON_SAVE, IDP_MAIL_BUTTON_SAVE);
    }
    if(pCreateData->AttNum)// && pCreateData->bEdit)
    {
        AppendMenu(hMenu, MF_ENABLED, IDM_MAIL_BUTTON_DELETE, IDP_MAIL_BUTTON_REMOVE);
    }

    return;
}

/*********************************************************************\
* Function	MailDisplay_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailDisplay_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	MailDisplay_OnKey
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
static void MailDisplay_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PMAIL_DISPLAYCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);

	switch (vk)
	{
    case VK_RETURN:
        {
            if((IsWindow(pCreateData->hTextView)) && (IsWindowVisible(pCreateData->hTextView)))
            {
                break;
            }
            if(pCreateData->AttNum == 0)
            {
                break;
            }
            PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_OPEN, 0), NULL);
        }
        break;

    case VK_F10:
        {
            MailAttachmentNode *pTemp;
            MailAttString TepString;

            if((IsWindow(pCreateData->hTextView)) && (IsWindowVisible(pCreateData->hTextView)))
            {
                DestroyWindow(pCreateData->hTextView);
                pCreateData->hTextView = NULL;

                GARY_FREE(pCreateData->pNode);
                PostMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
                break;
            }
            //if(pCreateData->bEdit)
            {
                memset(&TepString, 0x0, sizeof(MailAttString));
                if(pCreateData->AttHead.Num != 0)
                {
                    pTemp = pCreateData->AttHead.pNext;
                    //showname###savename***showname###savename\0
                    while(pTemp)
                    {
                        TepString.AttNum ++;
                        TepString.AttSize += pTemp->AttSize;
                        strcat(TepString.AttName, pTemp->AttShowName);
                        strcat(TepString.AttName, "###");
                        strcat(TepString.AttName, pTemp->AttSaveName);
                        if(pTemp->pNext != NULL)
                        {
                            strcat(TepString.AttName, "***");
                        }
                        pTemp = pTemp->pNext;
                    }
                    TepString.AttLen = strlen(TepString.AttName);
                }
                else
                {
                    TepString.AttLen = 0;
                    TepString.AttNum = 0;
                    TepString.AttSize = 0;
                }
                SendMessage(pCreateData->hParent, WM_COMMAND, MAKEWPARAM(ID_MAILBOX_MODIFY, 0), (LPARAM)&TepString);
            }
            
            PostMessage(hWnd, WM_CLOSE, NULL, NULL);
        }
        break;

    case VK_F5:
        {
            if((IsWindow(pCreateData->hTextView)) && (IsWindowVisible(pCreateData->hTextView)))
            {
                break;
            }
            if(pCreateData->AttNum <= 0)
            {
                break;
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
* Function	MailDisplay_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MailDisplay_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    HWND hList;
    PMAIL_DISPLAYCREATEDATA pCreateData;
    
    static char FilePath[PATH_MAXLEN];
    
    pCreateData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_VIEW_LIST);

	switch(id)
	{
    case IDC_MAIL_VIEW_LIST:
        {
            switch(codeNotify)
            {
            case LBN_DBLCLK:        //double click list
                {
                    SendMessage(hWnd, WM_COMMAND, IDM_MAIL_BUTTON_OPEN, 0);
                }
                break;
            case LBN_SELCHANGE:
            //case LBN_KILLFOCUS:
                if(pCreateData->bPlaySound)
                {
                    MAIL_StopPlay();
                    pCreateData->bPlaySound = FALSE;
                }
                SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
                break;
            default:
                PDADefWindowProc(hWnd, WM_COMMAND, MAKEWPARAM(id, codeNotify), 0);
                break;
            }           
            break;
        }
        break;

    case IDM_MAIL_BUTTON_OPEN:
        {
            int CurSel;
            int i;
            MailAttachmentNode *pCurNode;

            CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
            if(CurSel > pCreateData->AttHead.Num - 1)
            {
                break;
            }
            pCurNode = pCreateData->AttHead.pNext;
            for(i=0; i<CurSel; i++)
            {
                pCurNode = pCurNode->pNext;
            }

            memset(FilePath, 0x0, PATH_MAXLEN);
            sprintf(FilePath, "%s%s", MAIL_FILE_PATH_DIR, pCurNode->AttSaveName);

            /*if(pCurNode->AttType == INSERT_PIC)
            {
                int iType;

                iType = GetFileTypeByName(pCurNode->AttShowName, TRUE);
                BrowsePicture(NULL, pCreateData->hFrameWnd, NULL, NULL, FilePath, NULL, FALSE, iType);
            }*/
            if(pCurNode->AttType == INSERT_RING)
            {
                if(pCreateData->bPlaySound)
                {
                    MAIL_StopPlay();
                    pCreateData->bPlaySound = FALSE;
                    SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
                }
                else
                {
                    MAIL_PlayAudio(hWnd, FilePath, 3, NULL, 0);
                    pCreateData->bPlaySound = TRUE;
                    SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
                }
            }
            else if((pCurNode->AttType == INSERT_NODE) || (pCurNode->AttType == INSERT_PIC)
                || (pCurNode->AttType == INSERT_VCAL) || (pCurNode->AttType == INSERT_VCARD))
            {
                char szOutPath[PATH_MAXLEN];
                int hFile;
                struct stat SourceStat;
                
                memset(szOutPath, 0x0, PATH_MAXLEN);
                getcwd(szOutPath, PATH_MAXLEN);
                chdir(MAIL_FILE_PATH);

                if(stat(pCurNode->AttSaveName, &SourceStat) == -1)
                {
                    chdir(szOutPath);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);

                    break;
                }
                hFile = GARY_open(pCurNode->AttSaveName, O_RDONLY, -1);
                if(hFile < 0)
                {
                    printf("\r\nMailDetail.IDM_MAIL_BUTTON_OPEN : open error = %d\r\n", errno);
                    
                    chdir(szOutPath);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);

                    break;
                }
                GARY_FREE(pCreateData->pNode);
                pCreateData->pNode = malloc(SourceStat.st_size + 1);
                if(pCreateData->pNode == NULL)
                {
                    GARY_close(hFile);
                    chdir(szOutPath);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                    
                    break;
                }
                memset(pCreateData->pNode, 0x0, SourceStat.st_size + 1);
                read(hFile, pCreateData->pNode, SourceStat.st_size);
                GARY_close(hFile);
                chdir(szOutPath);

                if((pCurNode->AttType == INSERT_NODE) || (pCurNode->AttType == INSERT_VCAL)
                    || (pCurNode->AttType == INSERT_VCARD))
                {
                    pCreateData->hTextView = PlxTextView(pCreateData->hFrameWnd, hWnd, pCreateData->pNode,
                        SourceStat.st_size, FALSE, NULL, NULL, 0);
                }
                else
                {
                    pCreateData->hTextView = PlxTextView(pCreateData->hFrameWnd, hWnd, NULL, 0, TRUE, NULL,
                        pCreateData->pNode, SourceStat.st_size);
                }

                if(pCreateData->hTextView == NULL)
                {
                    GARY_FREE(pCreateData->pNode);
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);

                    break;
                }
                SetFocus(pCreateData->hTextView);
                
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_BACK);
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
                
                SetWindowText(pCreateData->hFrameWnd, pCurNode->AttShowName);
            }
            else
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_UNSUPPORTMAIL, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);                
            }
        }
        break;

    case IDM_MAIL_INSERT_PICTURE:
        {
            if(pCreateData->bPlaySound)
            {
                MAIL_StopPlay();
                pCreateData->bPlaySound = FALSE;
                SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
            }
            pCreateData->bReturn = INSERT_PIC;
            APP_PreviewPicture(pCreateData->hFrameWnd, hWnd, WM_MAIL_INSERT_RETURN, IDP_MAIL_TITLE_EMAIL, NULL);
        }
        break;
        
    case IDM_MAIL_INSERT_SOUND:
        {
            if(pCreateData->bPlaySound)
            {
                MAIL_StopPlay();
                pCreateData->bPlaySound = FALSE;
                SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
            }
            pCreateData->bReturn = INSERT_RING;
            PreviewSoundEx(pCreateData->hFrameWnd, hWnd, WM_MAIL_INSERT_RETURN, (PSTR)IDP_MAIL_TITLE_EMAIL, FALSE);
        }
        break;
        
    case IDM_MAIL_INSERT_NOTE:
        {
            if(pCreateData->bPlaySound)
            {
                MAIL_StopPlay();
                pCreateData->bPlaySound = FALSE;
                SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
            }
            pCreateData->bReturn = INSERT_NODE;
            BrowserNote(pCreateData->hFrameWnd, hWnd, WM_MAIL_INSERT_RETURN, (PSTR)IDP_MAIL_TITLE_EMAIL);
        }
        break;

    case IDM_MAIL_BUTTON_SAVE:
        {
            if(pCreateData->bPlaySound)
            {
                MAIL_StopPlay();
                pCreateData->bPlaySound = FALSE;
                SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
            }
            MAIL_SaveToPath(hWnd);
            /*if(MAIL_SaveToPath(hWnd))
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVED, NULL, 
                    Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            }
            else
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVINGFAIL, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            }*/
        }
        break;

    case IDM_MAIL_BUTTON_DELETE:
        {
            char szDisName[MAX_MAIL_SHOW_FILENAME_LEN + 10];
            int CurSel;
            int i;
            MailAttachmentNode *pCurNode;
            
            if(pCreateData->bPlaySound)
            {
                MAIL_StopPlay();
                pCreateData->bPlaySound = FALSE;
                SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
            }
            CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
            if(CurSel > pCreateData->AttHead.Num - 1)
            {
                break;
            }
            pCurNode = pCreateData->AttHead.pNext;
            for(i=0; i<CurSel; i++)
            {
                pCurNode = pCurNode->pNext;
            }

            memset(szDisName, 0x0, MAX_MAIL_SHOW_FILENAME_LEN + 10);
            sprintf(szDisName, "%s:\n%s", pCurNode->AttShowName, IDP_MAIL_STRING_REMOVEJUD);

            PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, szDisName, Notify_Request, NULL, 
                IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_DELETE_RETURN);
        }
        break;
        
    default:
        break;
	}

	return;
}
/*********************************************************************\
* Function	MailDisplay_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailDisplay_OnDestroy(HWND hWnd)
{
    PMAIL_DISPLAYCREATEDATA pCreateData;
    int i;

    pCreateData = GetUserData(hWnd);
    
    if(pCreateData->bPlaySound)
    {
        MAIL_StopPlay();
        pCreateData->bPlaySound = FALSE;
    }

    MAIL_ReleaseDisList(&pCreateData->AttHead);
    GARY_FREE(pCreateData->pNode);

    for(i=0; i<6; i++)
    {
        DeleteObject(pCreateData->hBmpObj[i]);
        pCreateData->hBmpObj[i] = NULL;
    }    

    HwndMailDisplay = NULL;
	//UnregisterClass("MailDisplayWndClass", NULL);

#ifdef MAIL_DEBUG
    EndObjectDebug();
#endif
	
    return;
}
/*********************************************************************\
* Function	MailDisplay_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailDisplay_OnClose(HWND hWnd)
{
    PMAIL_DISPLAYCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);
    SendMessage(pCreateData->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
    DestroyWindow(hWnd);

    return;
}

/*********************************************************************\
* Function	MailDisplay_OnInsertAtt
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void MailDisplay_OnInsertAtt(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMAIL_DISPLAYCREATEDATA pCreateData;
    HWND hList;
    char szNewAttFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    char szShowName[MAX_MAIL_SHOW_FILENAME_LEN + 1];
    char *InsertFileName;
    int AttSize;
    MailAttachmentNode *pNewNode;

    char pItem[8];
    int index;
    
    if(lParam == 0)
        return;
    
    pCreateData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_VIEW_LIST);
    
    if(pCreateData->bReturn == INSERT_NODE)
    {
        if((lParam == 0) || (wParam == 0))
            return;
        
        InsertFileName = (char*)wParam;
    }
    else
    {
        if(lParam == 0)
            return;
        
        InsertFileName = (char*)lParam;
    }
    
    if (*InsertFileName == '\0')
    {
        return;
    }
    while (*InsertFileName == 0x20)    // space
    {
        InsertFileName++;
    }

    AttSize = MAIL_GetMailAttachmentSize(InsertFileName);
    if((AttSize + pCreateData->nAttSize) > MAX_MAIL_ATT_SIZE)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MAXMAILSIZE, NULL, 
            Notify_Alert, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        return;
    }

    pNewNode = NULL;
    pNewNode = malloc(sizeof(MailAttachmentNode));
    if(pNewNode == NULL)
    {
        return;
    }

    memset(szNewAttFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
    MAIL_GetNewAffixFileName(szNewAttFileName);

    if(!MAIL_CopyFile(InsertFileName, szNewAttFileName))
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_INSERTFAILED, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        return;
    }

    memset(szShowName, 0x0, MAX_MAIL_SHOW_FILENAME_LEN + 1);
    if(pCreateData->bReturn == INSERT_NODE)
    {
        strcpy(szShowName, (char *)lParam);
    }
    else
    {
        MAIL_GetFileNameFromPath(InsertFileName, szShowName);
    }

    memset(pNewNode, 0x0, sizeof(MailAttachmentNode));
    
    strcpy(pNewNode->AttShowName, szShowName);
    strcpy(pNewNode->AttSaveName, szNewAttFileName);
    pNewNode->AttSize = AttSize;
    pNewNode->AttType = pCreateData->bReturn;

    MAIL_AddDisNode(&pCreateData->AttHead, pNewNode);

    pCreateData->nAttSize += AttSize;
    pCreateData->AttNum ++;

    memset(pItem, 0x0, 8);
    if ((AttSize = pNewNode->AttSize/1024) == 0)
    {
        AttSize = pNewNode->AttSize * 10 / 1024;
        sprintf(pItem, "0.%d K", AttSize);
    }
    else
    {
        sprintf(pItem, "%d K", AttSize);
    }
    
    index = SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)pNewNode->AttShowName);
    if(pNewNode->AttType != -1)
    {
        SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)index),
            (LPARAM)pCreateData->hBmpObj[pNewNode->AttType]);
    }
    else
    {
        SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP, (WORD)index),
            (LPARAM)pCreateData->hBmpObj[5]);
    }
    SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)pItem);

    SendMessage(hList, LB_SETCURSEL, index, 0);
    
    SendMessage(pCreateData->hParent, WM_MAIL_DIS_RETURN, (WPARAM)pCreateData->nAttSize, (LPARAM)pCreateData->AttNum);
    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_ATTINSERTED, NULL, 
        Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
    return;
}

/*********************************************************************\
* Function	MailDisplay_OnDeleteRet
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void MailDisplay_OnDeleteRet(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMAIL_DISPLAYCREATEDATA pCreateData;
    HWND hList;
    
    char szOutPath[PATH_MAXLEN];
    int CurSel;
    int i;
    MailAttachmentNode *pCurNode;
    
    if(lParam == 0)
        return;
    
    pCreateData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_VIEW_LIST);

    CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
    if(CurSel > pCreateData->AttHead.Num - 1)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_REMOVEFAIL, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        return;
    }
    pCurNode = pCreateData->AttHead.pNext;
    for(i=0; i<CurSel; i++)
    {
        pCurNode = pCurNode->pNext;
    }
    
    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    remove(pCurNode->AttSaveName);
    chdir(szOutPath);
    
    pCreateData->AttNum --;
    pCreateData->nAttSize -= pCurNode->AttSize;
    
    MAIL_DelDisNode(&pCreateData->AttHead, CurSel);
    
    SendMessage(hList, LB_DELETESTRING, CurSel, 0);
    if(CurSel >= 1)
    {
        SendMessage(hList, LB_SETCURSEL, CurSel - 1, 0);
    }
    else
    {
        SendMessage(hList, LB_SETCURSEL, CurSel, 0);
    }
    
    if(pCreateData->AttNum == 0)
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_BACK);
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        
        SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_BUTTON_ATTACHMENT);
    }
    SendMessage(pCreateData->hParent, WM_MAIL_DIS_RETURN, (WPARAM)pCreateData->nAttSize, (LPARAM)pCreateData->AttNum);

    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_REMOVED, NULL, 
        Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);

    return;
}

/*********************************************************************\
* Function	MailDisplay_OnMusicOff
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void MailDisplay_OnMusicOff(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMAIL_DISPLAYCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);

    pCreateData->bPlaySound = FALSE;
    SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);

    return;
}

void MAIL_ReleaseDisList(MailAttachmentHead *pHead)
{
    MailAttachmentNode *pTemp;
    
    pTemp = pHead->pNext;
    //only delete first node 
    //until the list is empty
    while(pTemp)
    {
        pHead->pNext = pTemp->pNext;
        GARY_FREE(pTemp);
        pHead->Num --;
        pTemp = pHead->pNext;
    }
    pHead->Num = 0;
    pHead->pNext = NULL;

    return;
}

void MAIL_AddDisNode(MailAttachmentHead *pHead, MailAttachmentNode *pTempNode)
{
    MailAttachmentNode *pTemp;

    pTempNode->pNext = NULL;

    //new node should add to pPreTemp->pNext
    pTemp = pHead->pNext;
    if(pTemp == NULL)
    {
        //new node is the first node in list
        pHead->pNext = pTempNode;
        pHead->Num ++;
        return;
    }

    //already some other nodes in list
    while (pTemp->pNext != NULL)
    {
        pTemp = pTemp->pNext;
    }
    pTemp->pNext = pTempNode;
    pHead->Num ++;

    return;
}

void MAIL_DelAttachmentNode(MailAttachmentHead *pHead, MailAttachmentNode *pNode)
{
    MailAttachmentNode *pPreNode;
    MailAttachmentNode *pCurNode;
    
    if(pHead->Num == 0)
    {
        return;
    }
    
    pCurNode = pHead->pNext;
    if(pNode == pCurNode)
    {
        pHead->pNext = pCurNode->pNext;
        GARY_FREE(pCurNode);
        pHead->Num --;
        return;
    }

    while(pCurNode != NULL)
    {
        if(pNode == pCurNode)
        {
            pPreNode->pNext = pCurNode->pNext;
            GARY_FREE(pCurNode);
            pHead->Num --;
            return;
        }
        pPreNode = pCurNode;
        pCurNode = pPreNode->pNext;
    }
    
    return;
}

static void MAIL_DelDisNode(MailAttachmentHead *pHead, int nTempNode)
{
    MailAttachmentNode *pPreNode;
    MailAttachmentNode *pCurNode;
    int i;

    if(nTempNode > (int)pHead->Num)
    {
        return;
    }

    pCurNode = pHead->pNext;
    if(nTempNode == 0)
    {
        pHead->pNext = pCurNode->pNext;
        GARY_FREE(pCurNode);
        pHead->Num --;
        return;
    }

    for(i=0;i<nTempNode;i++)
    {
        pPreNode = pCurNode;
        pCurNode = pPreNode->pNext;
    }

    pPreNode->pNext = pCurNode->pNext;
    GARY_FREE(pCurNode);
    pHead->Num --;
    
    return;
}

static BOOL MAIL_SaveToPath(HWND hWnd)
{
    PMAIL_DISPLAYCREATEDATA pCreateData;
    
    HWND hList;
    int CurSel;
    MailAttachmentNode *pCurNode;
    int i;
    int hFile;
    int SaveType;
    char FilePath[PATH_MAXLEN];
    char szOutPath[PATH_MAXLEN];

    struct stat SourceStat;

    pCreateData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd, IDC_MAIL_VIEW_LIST);
    
    CurSel = SendMessage(hList, LB_GETCURSEL, 0, 0);
    if(CurSel > pCreateData->AttHead.Num - 1)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVINGFAIL, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        return FALSE;
    }
    pCurNode = pCreateData->AttHead.pNext;
    for(i=0; i<CurSel; i++)
    {
        pCurNode = pCurNode->pNext;
    }
    
    memset(FilePath, 0x0, PATH_MAXLEN);
    if(pCurNode->AttType == INSERT_PIC)
    {
        sprintf(FilePath, "%s%s", MAIL_FILE_PIC_PATH, pCurNode->AttShowName);
    }
    else if(pCurNode->AttType == INSERT_RING)
    {
        sprintf(FilePath, "%s%s", MAIL_FILE_AUDIO_PATH, pCurNode->AttShowName);
    }
    else if(pCurNode->AttType == INSERT_NODE)
    {
        sprintf(FilePath, "%s%s", MAIL_FILE_TEXT_PATH, pCurNode->AttShowName);
    }
    else if((pCurNode->AttType == INSERT_VCARD) || (pCurNode->AttType == INSERT_VCAL))
    {
    }
    else
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVINGFAIL, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        return FALSE;
    }

    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    
    if(stat(pCurNode->AttSaveName, &SourceStat) == -1)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVINGFAIL, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        chdir(szOutPath);
        return FALSE;
    }
    hFile = GARY_open(pCurNode->AttSaveName, O_RDONLY, -1);
    if(hFile < 0)
    {
        printf("\r\nMAIL_SaveToPath : open error = %d\r\n", errno);

        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVINGFAIL, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);

        chdir(szOutPath);
        return FALSE;
    }
    GARY_FREE(pCreateData->pNode);
    pCreateData->pNode = malloc(SourceStat.st_size + 1);
    if(pCreateData->pNode == NULL)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVINGFAIL, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        
        GARY_close(hFile);
        chdir(szOutPath);
        return FALSE;
    }
    memset(pCreateData->pNode, 0x0, SourceStat.st_size + 1);
    read(hFile, pCreateData->pNode, SourceStat.st_size);
    GARY_close(hFile);
    chdir(szOutPath);

    SaveType = 0;
    if(pCurNode->AttType == INSERT_VCARD)
    {
        if(APP_SaveVcard(pCreateData->pNode, SourceStat.st_size) == TRUE)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVED, NULL, 
                Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);

            return TRUE;
        }
        else
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVINGFAIL, NULL, 
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);

            return FALSE;
        }
    }
    else if(pCurNode->AttType == INSERT_VCAL)
    {
        vCal_Obj *pvCalObj = NULL;
        
        vCal_Reader(pCreateData->pNode, SourceStat.st_size, &pvCalObj);
        
        if(pvCalObj != NULL)
        {                
            if(CALE_SaveFromSms(pvCalObj))
            {
                vCal_Clear(VCAL_OPT_READER, (PVOID)pvCalObj);
                
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVED, NULL, 
                    Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);

                return TRUE;
            }
        }
        vCal_Clear(VCAL_OPT_READER, (PVOID)pvCalObj);

        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVINGFAIL, NULL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        
        return FALSE;
    }
    else if(pCurNode->AttType == INSERT_PIC)
    {
        SaveType = META_IMG;
    }
    else if(pCurNode->AttType == INSERT_RING)
    {
        SaveType = META_AUDIO;
    }
    else if(pCurNode->AttType == INSERT_NODE)
    {
        SaveType = META_TEXT;
    }
    //if(MMS_SaveObject(pCreateData->hFrameWnd, hWnd, WM_MAIL_SAVEOBJ_RETURN, 
    //    IDP_MAIL_BUTTON_ATTACHMENT, FilePath, pCreateData->pNode, SourceStat.st_size))
    if(MMS_SelectDestination(pCreateData->hFrameWnd, hWnd, WM_MAIL_SAVEOBJ_RETURN,
        IDP_MAIL_BUTTON_ATTACHMENT, pCurNode->AttShowName, pCreateData->pNode, SourceStat.st_size, SaveType))
    {
        return TRUE;
    }
    return FALSE;
}

static void MAIL_FreeObject(HWND hWnd)
{
    PMAIL_DISPLAYCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);
    GARY_FREE(pCreateData->pNode);

    return;
}
