/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MailEdit.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "MailEdit.h"

HWND    HwndMailEdit;

/*********************************************************************\
* Function	   RegisterMailEditClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL RegisterMailEditClass(void)
{
    WNDCLASS wc;
        
    wc.style         = 0;
    wc.lpfnWndProc   = MailEditWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MAIL_EDITCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "MailEditWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}

void UnRegisterMailEditClass(void)
{
    UnregisterClass("MailEditWndClass", NULL);
}
/*********************************************************************\
* Function	   CreateMailEditWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL CreateMailEditWnd(HWND hParent, const char *PSZRECIPIENT, const char *PSZRECIPIENT2, 
        const char *PSZTITLE, const char *PSZCONTENT, const char *PSZATTENMENT, 
        const SYSTEMTIME *PTIME, DWORD handle, int folderid)
{
    HMENU hMenu;
    MAIL_EDITCREATEDATA CreateData;
    RECT rClient;
    int JudgRet;
    int iSignature;

    char szDis[MAX_MAIL_BOXNAME_LEN + 18];

#ifdef MAIL_DEBUG
    StartObjectDebug();
#endif
    
    JudgRet = MAIL_JudgeEdit();
    if(JudgRet == -1)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOMAILBOX, IDP_MAIL_TITLE_EMAIL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return FALSE;
    }
    else if(JudgRet == 0)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_INVALIDMAILSET, IDP_MAIL_TITLE_EMAIL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return FALSE;
    }
    memset(&CreateData, 0, sizeof(MAIL_EDITCREATEDATA));

    CreateData.hFrameWnd = hParent;
    GetClientRect(CreateData.hFrameWnd, &rClient);

    CreateData.folderid = folderid;
    CreateData.handle = handle;
    CreateData.bChange = FALSE;

    if((handle != -1) && (handle != -2))
    {
        CreateData.bSaved = TRUE;
        if(folderid == -1)
        {
            strcpy(CreateData.szFileName, ((MailGetListNode *)handle)->GetMailFileName);
        }
        else
        {
            MailListNode *pDelListNode;

            pDelListNode = NULL;
            MAIL_GetNodeByHandle(&MailListCurHead, &pDelListNode, (char *)handle);
            strcpy(CreateData.szFileName, pDelListNode->ListMailFileName);
        }
    }
    else
    {
        CreateData.bSaved = FALSE;
    }

    memcpy(&CreateData.date, PTIME, sizeof(SYSTEMTIME));

    if(PSZRECIPIENT)
    {
        CreateData.nReciLen = strlen(PSZRECIPIENT);
    }
    else
    {
        CreateData.nReciLen = 0;
    }

    if(PSZRECIPIENT2)
    {
        CreateData.nRec2Len = strlen(PSZRECIPIENT2);
    }
    else
    {
        CreateData.nRec2Len = 0;
    }
    
    CreateData.nReciLen = MAIL_MALLOC_LEN;
    CreateData.pszRecipient = (char*)malloc(MAIL_MALLOC_LEN + 1);
    CreateData.pszRecipient[0] = 0;
    CreateData.nRec2Len = MAIL_MALLOC_LEN;
    CreateData.pszRecipient2 = (char*)malloc(MAIL_MALLOC_LEN + 1);
    CreateData.pszRecipient2[0] = 0;
    if(CreateData.pszRecipient && CreateData.pszRecipient2)
    {
        CreateData.bChange = TRUE;
        if(PSZRECIPIENT)
        {
            strcpy(CreateData.pszRecipient, PSZRECIPIENT);
        }
        else
        {
            memset(CreateData.pszRecipient, 0, CreateData.nReciLen + 1);
        }
        if(PSZRECIPIENT2)
        {
            strcpy(CreateData.pszRecipient2, PSZRECIPIENT2);
        }
        else
        {
            memset(CreateData.pszRecipient2, 0, CreateData.nRec2Len + 1);
        }
    }
    else
    {
        GARY_FREE(CreateData.pszRecipient);
        GARY_FREE(CreateData.pszRecipient2);
        //UnRegisterMailEditClass();
        return FALSE;
    }

    if(PSZTITLE)
    {
        CreateData.nTitlLen = strlen(PSZTITLE);
        //CreateData.byMenuStatus |= MAIL_MENUSTATUS_SAVE;
    }
    else
    {
        CreateData.nTitlLen = 0;
    }
    
    CreateData.nTitlLen = MAIL_MALLOC_LEN;
    CreateData.pszTitle = (char*)malloc(CreateData.nTitlLen + 1);
    CreateData.pszTitle[0] = 0;
    if(CreateData.pszTitle)
    {
        CreateData.bChange = TRUE;
        if(PSZTITLE)
        {
            strcpy(CreateData.pszTitle, PSZTITLE);
        }
        else
        {
            memset(CreateData.pszTitle, 0x0, CreateData.nTitlLen + 1);
        }
    }
    else
    {
        GARY_FREE(CreateData.pszRecipient);
        GARY_FREE(CreateData.pszRecipient2);
        //UnRegisterMailEditClass();
        return FALSE;
    }
    
    if(PSZCONTENT)
    {
        CreateData.nConLen = strlen(PSZCONTENT);
        //CreateData.byMenuStatus |= MAIL_MENUSTATUS_SAVE;
    }
    else
    {
        CreateData.nConLen = 0;
    }

    iSignature = 0;
    if(!CreateData.bSaved)
    {
        //singnature
        iSignature = strlen(GlobalMailConfigActi.GLSignatrue);
        if(iSignature != 0)
        {
            CreateData.nConLen += iSignature;
        }
    }
    
    CreateData.nConLen = (CreateData.nConLen > MAX_MAIL_BODY_SIZE) ? 
        (CreateData.nConLen + MAIL_MALLOC_LEN) : MAX_MAIL_BODY_SIZE;
    CreateData.pszContent = (char*)malloc(CreateData.nConLen + 1);
    CreateData.pszContent[0] = 0;
    if(CreateData.pszContent)
    {
        CreateData.bChange = TRUE;
        if(PSZCONTENT)
        {
            strcpy(CreateData.pszContent, PSZCONTENT);
            strcat(CreateData.pszContent, " ");
        }
        else
        {
            memset(CreateData.pszContent, 0x0, CreateData.nConLen+1);
        }
        
        if((!CreateData.bSaved) && (iSignature != 0))
        {
            if(!PSZCONTENT)
            {
                strcpy(CreateData.pszContent, GlobalMailConfigActi.GLSignatrue);
            }
            else
            {
                strcat(CreateData.pszContent, GlobalMailConfigActi.GLSignatrue);
            }
        }
    }
    else
    {
        GARY_FREE(CreateData.pszRecipient);
        GARY_FREE(CreateData.pszRecipient2);
        GARY_FREE(CreateData.pszTitle);
        return FALSE;
    }

    if(PSZATTENMENT)
    {
        CreateData.nAttLen = strlen(PSZATTENMENT);
    }
    else
    {
        CreateData.nAttLen = 0;
    }
    
    CreateData.nAttLen = (CreateData.nAttLen > MAX_MAIL_BODY_SIZE) ? 
        (CreateData.nAttLen + MAIL_MALLOC_LEN) : MAX_MAIL_BODY_SIZE;
    CreateData.pszAttachment = (char*)malloc(CreateData.nAttLen + 1);
    CreateData.pszAttachment[0] = 0;
    if(CreateData.pszAttachment)
    {
        CreateData.bChange = TRUE;
        CreateData.pszOldAtt = NULL;
        if(PSZATTENMENT)
        {
            if(CreateData.bSaved)
            {
                int BackAttNum;

                CreateData.pszOldAtt = malloc(strlen(PSZATTENMENT) + 1);
                if(!CreateData.pszOldAtt)
                {
                    GARY_FREE(CreateData.pszRecipient);
                    GARY_FREE(CreateData.pszRecipient2);
                    GARY_FREE(CreateData.pszTitle);
                    GARY_FREE(CreateData.pszContent);
                    GARY_FREE(CreateData.pszAttachment);
                    
                    //UnRegisterMailEditClass();
                    return FALSE;
                }
                memset(CreateData.pszOldAtt, 0x0, strlen(PSZATTENMENT) + 1);
                strcpy(CreateData.pszOldAtt, PSZATTENMENT);

                BackAttNum = MAIL_GetMailAttachmentNum(CreateData.pszOldAtt);
                MAIL_CopyAttachment(CreateData.pszAttachment, CreateData.pszOldAtt, BackAttNum);
            }
            else
            {
                strcpy(CreateData.pszAttachment, PSZATTENMENT);
            }
            CreateData.nAttNum = MAIL_GetMailAttachmentNum(CreateData.pszAttachment);
            CreateData.nAttSize = MAIL_GetAllMailAttSize(CreateData.pszAttachment, CreateData.nAttNum);
        }
        else
        {
            memset(CreateData.pszAttachment, 0x0, CreateData.nAttLen + 1);
            CreateData.nAttNum = 0;
            CreateData.nAttSize = 0;
        }
    }
    else
    {
        GARY_FREE(CreateData.pszRecipient);
        GARY_FREE(CreateData.pszRecipient2);
        GARY_FREE(CreateData.pszTitle);
        GARY_FREE(CreateData.pszContent);
        //UnRegisterMailEditClass();
        return FALSE;
    }

    if((folderid != -1) && (CreateData.bChange == TRUE))
        CreateData.bChange = FALSE;
    
    hMenu = CreateMenu();
    CreateData.hMenu = hMenu;

    HwndMailEdit = CreateWindow(
        "MailEditWndClass", 
        "", //IDP_MAIL_TITLE_EMAIL,
        WS_VISIBLE | WS_CHILD | WS_VSCROLL,//PWS_STATICBAR | WS_CAPTION | WS_VISIBLE | WS_VSCROLL,
        rClient.left, 
        rClient.top, 
        rClient.right - rClient.left,
        rClient.bottom - rClient.top, 
        CreateData.hFrameWnd,//hParent,
        NULL, //hMenu,
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!HwndMailEdit)
    {
        GARY_FREE(CreateData.pszRecipient);
        GARY_FREE(CreateData.pszRecipient2);
        GARY_FREE(CreateData.pszTitle);
        GARY_FREE(CreateData.pszContent);
        GARY_FREE(CreateData.pszAttachment);
        GARY_FREE(CreateData.pszOldAtt);

        //UnRegisterMailEditClass();
        return FALSE;
    }

    //show window
    PDASetMenu(CreateData.hFrameWnd, hMenu);
    SetWindowText(CreateData.hFrameWnd, IDP_MAIL_TITLE_EMAIL);
    ShowWindow(CreateData.hFrameWnd, SW_SHOW);
    UpdateWindow(CreateData.hFrameWnd);

    {
        HWND hTitleEdt = NULL;
        HWND hContentEdt = NULL;

        hTitleEdt = GetDlgItem(HwndMailEdit, IDC_MAIL_EDIT_TITLE);
        hContentEdt = GetDlgItem(HwndMailEdit, IDC_MAIL_EDIT_CONTENT);
        SendMessage(hTitleEdt, EM_SETSEL, -1, -1);
        SendMessage(hContentEdt, EM_SETSEL, 0, 0);
    }

    memset(szDis, 0x0, MAX_MAIL_BOXNAME_LEN + 18);
    sprintf(szDis, "%s%s", IDP_MAIL_STRING_MAILBOXINUSE, GlobalMailConfigActi.GLMailBoxName);
    PLXTipsWin(NULL, NULL, 0, szDis, NULL, Notify_Info, 
        IDP_MAIL_BOOTEN_OK, NULL, 20);

    return TRUE;
}

/*********************************************************************\
* Function	MailEditWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT MailEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MailEdit_OnCreate(hWnd, (LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        MailEdit_OnActivate(hWnd, (UINT)LOWORD(wParam));
        break;
        
    case WM_INITMENU:
        MailEdit_OnInitmenu(hWnd);
        break;

    case WM_PAINT:
        MailEdit_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        MailEdit_OnKey(hWnd, (UINT)(wParam), (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        {
            PMAIL_EDITCREATEDATA pCreateData;
        
            pCreateData = GetUserData(hWnd);
        
            if(wParam == (WPARAM)GetMenu(pCreateData->hFrameWnd))
            {
                //WM_INITMENU
                MailEdit_OnInitmenu(hWnd);
                break;
            }
            MailEdit_OnCommand(hWnd, (int)(LOWORD(wParam)), (UINT)HIWORD(wParam), lParam);
        }
        break;

    case WM_SETLBTNTEXT:
        MailEdit_OnSetLBtnText(hWnd, (int)LOWORD(wParam), (BOOL)HIWORD(wParam), (LPSTR)lParam);
        break;
        
        
    case WM_CLOSE:
        {
            PMAIL_EDITCREATEDATA pCreateData;
            
            pCreateData = GetUserData(hWnd);
            
            SendMessage(pCreateData->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            MailEdit_OnClose(hWnd);
        }
        break;

    case WM_DESTROY:
        MailEdit_OnDestroy(hWnd);
        break;

	case REC_SIZE:
        MailEdit_OnMoveWindow(hWnd, wParam);
        break;

    case WM_MAIL_BOX_RETURN:
        MailEdit_OnConfirm(hWnd, wParam, lParam);
        break;

    case WM_MAIL_ADDRESS_RETURN:
        MailEdit_OnAddress(hWnd, wParam, lParam);
        break;

    case WM_MAIL_INSERT_RETURN:
        MailEdit_OnInsertAtt(hWnd, wParam, lParam);
        break;

    case WM_IMESWITCHED:
        MailEdit_OnSetRightIcon(hWnd, wParam, lParam);
        break;

    case WM_MAIL_DIS_RETURN:
        MAIL_SetNumber(hWnd, (int)wParam, (int)lParam);
        break;

    case WM_MAIL_REPLACE_SEL:
        MailEdit_OnReplaceSel(hWnd);
        break;
        
    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	MailEdit_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MailEdit_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    PMAIL_EDITCREATEDATA pCreateData;
    HWND hRecipientEdt = NULL;
    HWND hRecipient2Edt = NULL;
    HWND hTitleEdt = NULL;
    HWND hContentEdt = NULL;

    IMEEDIT ie;
    int x, y;
    int nWidth, nHeight;
    RECT rc;
    SIZE Size;
    HDC  hdc;
    int  nY, nLine = 0;

    pCreateData = GetUserData(hWnd);

    memcpy(pCreateData, lpCreateStruct->lpCreateParams, sizeof(MAIL_EDITCREATEDATA));

    SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), (LPARAM)IDP_MAIL_BUTTON_CANCEL);
    SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MAIL_BUTTON_ADD, 1), (LPARAM)IDP_MAIL_BUTTON_ADD);
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);

    GetClientRect(hWnd, &rc);

    //recipient
    x = 0;
    y = 0;
    nWidth = rc.right - rc.left;
    nHeight = RECIWND_HEIGHT;

    hRecipientEdt = CreateWindow(
        WC_RECIPIENT, 
        "", 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | RS_EMAIL | ES_UNDERLINE,
        x, y, nWidth, nHeight,
        hWnd, 
        (HMENU)IDC_MAIL_EDIT_RECIPIENT, 
        NULL, 
        NULL);

    if(hRecipientEdt == NULL)
        return FALSE;

    SendMessage(hRecipientEdt, EM_LIMITTEXT, MAX_MAIL_RECIPIENT_LEN, 0);
    SendMessage(hRecipientEdt, REC_SETMAXREC, MAX_MAIL_RECIPIENT_NUM, 0);
    SendMessage(hRecipientEdt, EM_SETTITLE, 0, (LPARAM)IDP_MAIL_TITLE_TO);
    if(strlen(pCreateData->pszRecipient) != 0)
    {
        int mailNum;
        int i;
        char szAddr[MAX_MAIL_RECIEVESOLE_LEN];

        mailNum = MAIL_AnalyseMailAddr(pCreateData->pszRecipient);
        for(i = 0; i < mailNum; i ++)
        {
            memset(szAddr, 0x0, MAX_MAIL_RECIEVESOLE_LEN);
            if(MAIL_GetMailAddress(pCreateData->pszRecipient, szAddr, i))
            {
                SendMessage(hRecipientEdt, GHP_ADDREC, 1, (LPARAM)szAddr);
            }
        }
    }
    
    //recipient2
    y = y + nHeight;

    hRecipient2Edt = CreateWindow(
        WC_RECIPIENT, 
        "", 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | RS_EMAIL | ES_UNDERLINE,
        x, y, nWidth, nHeight,
        hWnd, 
        (HMENU)IDC_MAIL_EDIT_RECIPIENT2, 
        NULL, 
        NULL);
    
    if(hRecipient2Edt == NULL)
        return FALSE;

    SendMessage(hRecipient2Edt, EM_LIMITTEXT, MAX_MAIL_RECIPIENT_LEN, 0);
    SendMessage(hRecipientEdt, REC_SETMAXREC, MAX_MAIL_RECIPIENT_NUM, 0);
    SendMessage(hRecipient2Edt, EM_SETTITLE, 0, (LPARAM)IDP_MAIL_TITLE_CC);
    if(strlen(pCreateData->pszRecipient2) != 0)
    {
        int mailNum;
        int i;
        char szAddr[MAX_MAIL_RECIEVESOLE_LEN];
        
        mailNum = MAIL_AnalyseMailAddr(pCreateData->pszRecipient2);
        for(i = 0; i < mailNum; i ++)
        {
            memset(szAddr, 0x0, MAX_MAIL_RECIEVESOLE_LEN);
            if(MAIL_GetMailAddress(pCreateData->pszRecipient2, szAddr, i))
            {
                SendMessage(hRecipient2Edt, GHP_ADDREC, 1, (LPARAM)szAddr);
            }
        }
    }

    //title
    y = y + nHeight;

    memset(&ie, 0, sizeof(IMEEDIT));
    
    ie.hwndNotify	= (HWND)hWnd;    
    ie.dwAttrib	    = 0;                
    ie.dwAscTextMax	= 0;
    ie.dwUniTextMax	= 0;
    ie.wPageMax	    = 0;        
    ie.pszCharSet	= NULL;
    ie.pszTitle	    = NULL;
    ie.pszImeName	= NULL;

    hTitleEdt = CreateWindow(
        "IMEEDIT", 
        "", 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_TITLE | ES_AUTOHSCROLL,
        x, y, nWidth, nHeight,
        hWnd, 
        (HMENU)IDC_MAIL_EDIT_TITLE, 
        NULL, 
        (PVOID)&ie);
    
    if(hTitleEdt == NULL)
        return FALSE;
    
    SendMessage(hTitleEdt, EM_LIMITTEXT, MAX_MAIL_SUBJECE_LEN, 0);
    SendMessage(hTitleEdt, EM_SETTITLE, 0, (LPARAM)IDP_MAIL_TITLE_SUBJECT);
    if(strlen(pCreateData->pszTitle) != 0)
    {
        SetWindowText(hTitleEdt, pCreateData->pszTitle);
    }

    //content
    y = y + nHeight;
    hdc = GetDC(hWnd);
    {
        if(strlen(pCreateData->pszContent) == 0)
        {
            nLine = 50;
        }
        else
        {
            char* p;
            int nFit = 0;
            int num = 0;
            int nLineChars = 0;

            GetTextExtentPoint(hdc, "W", 1, &Size);
            
            nLineChars = (nWidth - 8) / Size.cx;
            
            p = pCreateData->pszContent;
            while(*p != '\0')
            {
#if 0
                GetTextExtentExPoint(hdc, p, strlen(p), nWidth-4, &nFit, NULL, NULL);
                nLine++;
                p += nFit;
#else
                num ++;
                p ++;
                if(num == nLineChars)
                {
                    nLine++;
                    num = 0;
                }
#endif
            }
            if(num != 0)
            {
                nLine++;
            }
        }
    }
    
    GetTextExtent(hdc, "W", 1, &Size);
    ReleaseDC(hWnd, hdc);

    nY = Size.cy + MAIL_EDIT_SPACE;
    nHeight = (nY + 1) * nLine + 30;
    pCreateData->nHeight = nHeight;

    hContentEdt = CreateWindow(
        "IMEEDIT", 
        "", 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE,
        x, y, nWidth, nHeight,
        hWnd, 
        (HMENU)IDC_MAIL_EDIT_CONTENT, 
        NULL, 
        (PVOID)&ie);
    
    if(hContentEdt == NULL)
        return FALSE;

    SendMessage(hContentEdt, EM_LIMITTEXT, MAX_MAIL_BODY_SIZE - 1, 0);
    SetWindowText(hContentEdt, pCreateData->pszContent);
    
    pCreateData->hFocus = hRecipientEdt;
    pCreateData->hOldRecipient = hRecipientEdt;
    pCreateData->hOldFocus = hRecipientEdt;

    MAIL_EditInitVScroll(hWnd);

    MAIL_SetMemIndicator(hWnd, pCreateData->nAttSize + GetWindowTextLength(hContentEdt));
    SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)pCreateData->hBitmapIcon);
    
    if(pCreateData->nAttNum > 0)
    {
        MAIL_SetAttIndicator(hWnd, pCreateData->nAttNum);
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)pCreateData->hAttIcon);
    }
    else
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
    }

    pCreateData->hInsertMenu = LoadMenuIndirect((PMENUTEMPLATE)&MailInsertMenuTemplate);
    ModifyMenu(pCreateData->hInsertMenu, IDM_MAIL_INSERT_PICTURE, MF_STRING, IDM_MAIL_INSERT_PICTURE, IDP_MAIL_BUTTON_PICTURE);
    ModifyMenu(pCreateData->hInsertMenu, IDM_MAIL_INSERT_SOUND, MF_STRING, IDM_MAIL_INSERT_SOUND, IDP_MAIL_BUTTON_SOUND);
    ModifyMenu(pCreateData->hInsertMenu, IDM_MAIL_INSERT_NOTE, MF_STRING, IDM_MAIL_INSERT_NOTE, IDP_MAIL_BUTTON_NOTE);
    ModifyMenu(pCreateData->hInsertMenu, IDM_MAIL_INSERT_TEMPLATE, MF_STRING, IDM_MAIL_INSERT_TEMPLATE, IDP_MAIL_BUTTON_TEMPLATE);

    SetFocus(pCreateData->hFocus);
    pCreateData->OldCaretProc = SetCaretProc(MAIL_CaretProc);
    
    return TRUE;
    
}
/*********************************************************************\
* Function	MailEdit_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MailEdit_OnActivate(HWND hWnd, UINT state)
{
    PMAIL_EDITCREATEDATA pCreateData;
    HWND hRecipientEdt = NULL;
    HWND hRecipient2Edt = NULL;
    HWND hTitleEdt = NULL;
    HWND hContentEdt = NULL;

    int nRecipientLen = 0;
    char szBtnText[100];
    
    pCreateData = GetUserData(hWnd);
    
    hRecipientEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_RECIPIENT);
    hRecipient2Edt = GetDlgItem(hWnd, IDC_MAIL_EDIT_RECIPIENT2);
    hTitleEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_TITLE);
    hContentEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_CONTENT);

    PDASetMenu(pCreateData->hFrameWnd, pCreateData->hMenu);

    SetWindowText(pCreateData->hFrameWnd, IDP_MAIL_TITLE_EMAIL);
              
    if((pCreateData->hFocus == hRecipientEdt) || (pCreateData->hFocus == hRecipient2Edt))
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_ADD);
    }
    else
    {
        memset(szBtnText, 0x0, 100);
        SendMessage(pCreateData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szBtnText);
        nRecipientLen = GetWindowTextLength(hRecipientEdt);
        
        if((nRecipientLen == 0) && (stricmp(szBtnText, IDP_MAIL_BUTTON_SAVE) != 0))
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_SAVE);
        
        if((nRecipientLen != 0) && (stricmp(szBtnText, IDP_MAIL_BUTTON_SEND) != 0))
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_SEND);
    }
    
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_MAIL_BUTTON_CANCEL);
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    MAIL_SetMemIndicator(hWnd, pCreateData->nAttSize + GetWindowTextLength(hContentEdt));
    SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)pCreateData->hBitmapIcon);

    if(pCreateData->nAttNum > 0)
    {
        MAIL_SetAttIndicator(hWnd, pCreateData->nAttNum);

        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)pCreateData->hAttIcon);
    }
    else
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
    }

    SetFocus(pCreateData->hFocus);

    return;
}
/*********************************************************************\
* Function	MailEdit_OnInitmenu
* Purpose   WM_INITMENU message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailEdit_OnInitmenu(HWND hWnd)
{
    HWND hRecipientEdt = NULL;
    HWND hRecipient2Edt = NULL;
    HWND hTitleEdt = NULL;
    HWND hContentEdt = NULL;

    HMENU hMenu = NULL;
    int iMenuCount;
    PMAIL_EDITCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);

    hRecipientEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_RECIPIENT);
    hRecipient2Edt = GetDlgItem(hWnd, IDC_MAIL_EDIT_RECIPIENT2);
    hTitleEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_TITLE);
    hContentEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_CONTENT);

    //hMenu = PDAGetMenu(pCreateData->hFrameWnd);
    hMenu = pCreateData->hMenu;
    iMenuCount = GetMenuItemCount(hMenu);
    while(iMenuCount-- > 0)
    {
        RemoveMenu(hMenu, iMenuCount, MF_BYPOSITION);
    }
    
    if(GetWindowTextLength(hRecipientEdt) != 0)
    {
        AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_SEND, IDP_MAIL_BUTTON_SEND);
        AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_OPTION, IDP_MAIL_BUTTON_SENDOPTION);
    }
    else
    {
        if(GetWindowTextLength(hRecipient2Edt) != 0)
        {
            AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_OPTION, IDP_MAIL_BUTTON_SENDOPTION);
        }
    }
    
    if((GetWindowTextLength(hRecipientEdt) != 0) || (GetWindowTextLength(hRecipient2Edt) != 0)
        || (GetWindowTextLength(hTitleEdt) != 0) || (GetWindowTextLength(hContentEdt) != 0)
        || (pCreateData->nAttNum != 0))
    {
        AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_SAVE, IDP_MAIL_BUTTON_SAVEDRAFTS);
    }

    if((pCreateData->hFocus == hRecipientEdt) || (pCreateData->hFocus == hRecipient2Edt))
    {
        AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_ADD, IDP_MAIL_BUTTON_ADDRECIPIENT);
    }
    else if((pCreateData->hFocus == hTitleEdt) || (pCreateData->hFocus == hContentEdt))
    {
        AppendMenu(hMenu, MF_STRING | MF_ENABLED | MF_POPUP, (UINT_PTR)pCreateData->hInsertMenu, IDP_MAIL_BUTTON_INSERT);
    }

    if(pCreateData->nAttNum != 0)
    {
        AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_ATTACHMENT, IDP_MAIL_BUTTON_ATTACHMENT);
    }

    AppendMenu(hMenu, MF_STRING | MF_ENABLED, IDM_MAIL_BUTTON_INFO, IDP_MAIL_BUTTON_INFO);
    
    return;

}
/*********************************************************************\
* Function	MailEdit_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailEdit_OnPaint(HWND hWnd)
{
    PMAIL_EDITCREATEDATA pCreateData;

    //RECT rc = {2,1,169,11};

	HDC hdc = BeginPaint(hWnd, NULL);

    pCreateData = GetUserData(hWnd);

    //DrawText(hdc, pCreateData->pString, -1, &rc, DT_CLEAR | DT_RIGHT);

	EndPaint(hWnd, NULL);

	return;
}
/*********************************************************************\
* Function	MailEdit_OnKey
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
static void MailEdit_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
//    HWND hWndFocus = NULL;
    PMAIL_EDITCREATEDATA pCreateData;
    HWND hRecipientEdt = NULL;
    HWND hRecipient2Edt = NULL;
    HWND hTitleEdt = NULL;
    HWND hContentEdt = NULL;
    HWND hFocus = NULL;
    HWND hParent = NULL;
    int nLen;

    pCreateData = GetUserData(hWnd);

    hRecipientEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_RECIPIENT);
    hRecipient2Edt = GetDlgItem(hWnd, IDC_MAIL_EDIT_RECIPIENT2);
    hTitleEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_TITLE);
    hContentEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_CONTENT);

	switch (vk)
	{
    case VK_F10:
//    case VK_F2:
        {
            PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_EXIT, 0), 0);
        }
		break;

    case VK_RETURN:
        {
            char szBtnText[100];
            
            memset(szBtnText, 0x0, 100);
            SendMessage(pCreateData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szBtnText);
            if(stricmp(szBtnText, IDP_MAIL_BUTTON_ADD) == 0 )
            {
                PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_ADD, 0), 0);
            }
            if(stricmp(szBtnText, IDP_MAIL_BUTTON_SEND) == 0 )
            {
                PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_SEND, 0), 0);
            }
            if(stricmp(szBtnText, IDP_MAIL_BUTTON_SAVE) == 0 )
            {
                PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAIL_BUTTON_SAVE, 0), 0);
            }
        }
        break;

    case VK_F1:
        {
            nLen = GetWindowTextLength(hRecipientEdt);
            if(nLen == 0)
                PostMessage(hWnd, WM_COMMAND, IDM_MAIL_BUTTON_ADD, NULL);
            else
                PostMessage(hWnd, WM_COMMAND, IDM_MAIL_BUTTON_SEND,NULL);
        }
        break;

    case VK_F5:
        {
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
* Function	MailEdit_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MailEdit_OnCommand(HWND hWnd, int id, UINT codeNotify, LPARAM lParam)
{
    PMAIL_EDITCREATEDATA pCreateData;

    HWND hRecipientEdt = NULL;
    HWND hRecipient2Edt = NULL;
    HWND hTitleEdt = NULL;
    HWND hContentEdt = NULL;

    pCreateData = GetUserData(hWnd);
    hRecipientEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_RECIPIENT);
    hRecipient2Edt = GetDlgItem(hWnd, IDC_MAIL_EDIT_RECIPIENT2);
    hTitleEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_TITLE);
    hContentEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_CONTENT);

	switch(id)
	{
    case IDM_MAIL_BUTTON_SEND:
        {
            if(MAIL_SaveToBox(hWnd, MU_OUTBOX))
            {
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
        }
        break;

    case IDM_MAIL_BUTTON_OPTION:
        {
            MAIL_CreateSetWnd(pCreateData->hFrameWnd);
        }
        break;
        
    case IDM_MAIL_BUTTON_SAVE:
        {
            if(MAIL_SaveToBox(hWnd, MU_DRAFT))
            {
                pCreateData->bSaved = TRUE;
            }
            //PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;
        
    case IDM_MAIL_BUTTON_ADD: // wait
        {
            RECIPIENTLISTBUF Recipient;
            PRECIPIENTLISTNODE pTemp = NULL;
            int i,j;
            
            memset(pCreateData->ABAddr, 0x0, sizeof(ABNAMEOREMAIL) * 20);
            memset(&Recipient, 0, sizeof(RECIPIENTLISTBUF));
            
            if(pCreateData->hFocus == hRecipientEdt)
            {
                SendMessage(hRecipientEdt, GHP_GETREC, 0, (LPARAM)&Recipient);
            }
            else if(pCreateData->hFocus == hRecipient2Edt)
            {
                SendMessage(hRecipient2Edt, GHP_GETREC, 0, (LPARAM)&Recipient);
            }
            else
            {
                break;
            }

            pTemp = Recipient.pDataHead;
            j = 0;
            //Recipient.nDataNum = 0;//temporary
            for(i = 0; i < Recipient.nDataNum ; i++)
            {
                if(pTemp->bExistInAB)
                {
                    pCreateData->ABAddr[j].nType = AB_EMAIL;
                    strcpy(pCreateData->ABAddr[j].szName, pTemp->szShowName);
                    strcpy(pCreateData->ABAddr[j].szTelOrEmail, pTemp->szPhoneNum);
                    j ++;
                }
                pTemp = pTemp->pNext;
            }

            APP_GetMultiPhoneOrEmail(pCreateData->hFrameWnd, hWnd, WM_MAIL_ADDRESS_RETURN, 
                PICK_EMAIL, pCreateData->ABAddr, j, MAX_MAIL_RECIPIENT_NUM);
        }
        break;
        
    case IDM_MAIL_BUTTON_INSERT: // wait
        {
        }
        break;

    case IDM_MAIL_INSERT_PICTURE:
        {
            pCreateData->bReturn = INSERT_PIC;
            APP_PreviewPicture(pCreateData->hFrameWnd, hWnd, WM_MAIL_INSERT_RETURN, IDP_MAIL_TITLE_EMAIL, NULL);
        }
        break;

    case IDM_MAIL_INSERT_SOUND:
        {
            pCreateData->bReturn = INSERT_RING;
            PreviewSoundEx(pCreateData->hFrameWnd, hWnd, WM_MAIL_INSERT_RETURN, (PSTR)IDP_MAIL_TITLE_EMAIL, FALSE);
        }
        break;

    case IDM_MAIL_INSERT_NOTE:
        {
            pCreateData->bReturn = INSERT_NODE;
            BrowserNote(pCreateData->hFrameWnd, hWnd, WM_MAIL_INSERT_RETURN, (PSTR)IDP_MAIL_TITLE_EMAIL);
        }
        break;

    case IDM_MAIL_INSERT_TEMPLATE:
        {
            pCreateData->bReturn = INSERT_TEMLATE;
            APP_PreviewPhrase(pCreateData->hFrameWnd, hWnd, WM_MAIL_INSERT_RETURN, (PCSTR)IDP_MAIL_TITLE_EMAIL);
        }
        break;

    case IDM_MAIL_BUTTON_ATTACHMENT:
        {
            CreateMailDisplayWnd(pCreateData->hFrameWnd, hWnd, pCreateData->pszAttachment,
                pCreateData->nAttNum, pCreateData->nAttSize, 1);
        }
        break;
        
    case IDM_MAIL_BUTTON_INFO: 
        {
            int nLen;
            int AttSize;
            MailFile mMail;

            memset(&mMail, 0x0, sizeof(MailFile));
            //recipient
            nLen = MAIL_RecipientGetTextLength(hRecipientEdt);
            mMail.MailSize = nLen + 1;
            if(nLen > 0)
            {
                MAIL_RecipientGetText(hRecipientEdt, mMail.To, nLen + 1);
                strcpy(mMail.address, mMail.To);
            }
            //recipient2
            nLen = MAIL_RecipientGetTextLength(hRecipient2Edt);
            mMail.MailSize += nLen + 1;
            if(nLen > 0)
            {
                MAIL_RecipientGetText(hRecipient2Edt, mMail.Cc, nLen + 1);
            }            
            //title
            nLen = GetWindowTextLength(hTitleEdt);
            mMail.MailSize += nLen + 1;
            if(nLen > 0)
            {
                GetWindowText(hTitleEdt, mMail.Subject, nLen + 1);
            }           
            //content
            nLen = GetWindowTextLength(hContentEdt);
            mMail.TextSize = nLen + 1;
            mMail.MailSize += nLen + 1;
            //attachment
            mMail.AttFileNameLen = strlen(pCreateData->pszAttachment);
            mMail.AttNum = pCreateData->nAttNum;
            AttSize = MAIL_GetAllMailAttSize(pCreateData->pszAttachment, pCreateData->nAttNum);
            mMail.MailSize += AttSize;
            //local
            memcpy(&mMail.LocalDate, &pCreateData->date, sizeof(SYSTEMTIME));
            mMail.MailSize += 2 * sizeof(SYSTEMTIME);
            
            strcpy(mMail.From, "1");
            CreateMailDetailWnd(pCreateData->hFrameWnd, &mMail);
        }
        break;

    case IDM_MAIL_BUTTON_EXIT:
        if(((pCreateData->bSaved == TRUE) && (pCreateData->bChange == TRUE)) 
              || ((pCreateData->bSaved == FALSE) && (pCreateData->bChange == TRUE)))
            //((GetWindowTextLength(hRecipientEdt) || GetWindowTextLength(hContentEdt) 
          //  || (GetWindowTextLength(hRecipient2Edt)) || GetWindowTextLength(hTitleEdt))
          //  && ((pCreateData->bSaved == TRUE && pCreateData->bChange == TRUE) 
          //  || (pCreateData->bSaved == FALSE && pCreateData->bChange == TRUE)))
        {
            PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_MAIL_STRING_SAVETODRAFT, Notify_Request, 
                NULL, IDP_MAIL_BUTTON_YES, IDP_MAIL_BUTTON_NO, WM_MAIL_BOX_RETURN);
        }
        else
        {
            if(pCreateData->nAttNum > 0)
            {
                MAIL_DeleteAttachment(pCreateData->pszAttachment);
            }
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;

    case IDC_MAIL_EDIT_RECIPIENT:
        {
            if(codeNotify == EN_CHANGE)
            {
                pCreateData->bChange = TRUE;
            }
            else if(codeNotify == EN_SETFOCUS)
            {
                SCROLLINFO vsi;
                char szBtnText[100];

                pCreateData->hFocus = hRecipientEdt;
                pCreateData->hOldRecipient = hRecipientEdt;
                pCreateData->OldCaretProc = SetCaretProc(MAIL_CaretProc);

                memset(&vsi, 0, sizeof(SCROLLINFO));
                vsi.fMask  = SIF_POS;
                GetScrollInfo(hWnd, SB_VERT, &vsi);
                vsi.nPos = 0;
                vsi.fMask  = SIF_POS;
                SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE); 

                memset(szBtnText, 0x0, 100);
                SendMessage(pCreateData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szBtnText);
                if(stricmp(szBtnText, IDP_MAIL_BUTTON_ADD) != 0)
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_ADD);

                SendMessage(hContentEdt, EM_SETSEL, 0, 0);
            }
            else if(codeNotify == EN_KILLFOCUS)
            {
                HWND hFocus;

                pCreateData->hOldFocus = hRecipientEdt;
                
                hFocus = GetFocus();
                if((hFocus != hRecipient2Edt) && (hFocus != hTitleEdt) && (hFocus != hContentEdt))
                    SetCaretProc(NULL);
            }
        }
        break;

    case IDC_MAIL_EDIT_RECIPIENT2:
        {
            if(codeNotify == EN_CHANGE)
            {
                pCreateData->bChange = TRUE;
            }
            else if(codeNotify == EN_SETFOCUS)
            {
                char szBtnText[100];

                pCreateData->hFocus = hRecipient2Edt;
                pCreateData->hOldRecipient = hRecipient2Edt;
                pCreateData->OldCaretProc = SetCaretProc(MAIL_CaretProc);

                memset(szBtnText, 0x0, 100);
                SendMessage(pCreateData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szBtnText);
                if(stricmp(szBtnText, IDP_MAIL_BUTTON_ADD) != 0 )
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_ADD);
            
                SendMessage(hContentEdt, EM_SETSEL, 0, 0);
            }
            else if(codeNotify == EN_KILLFOCUS)
            {
                HWND hFocus;
                
                pCreateData->hOldFocus = hRecipient2Edt;
                
                hFocus = GetFocus();
                if((hFocus != hRecipientEdt) && (hFocus != hTitleEdt) && (hFocus != hContentEdt))
                    SetCaretProc(NULL);
            }
        }
        break;

    case IDC_MAIL_EDIT_TITLE:
        {
            if(codeNotify == EN_CHANGE)
            {
                pCreateData->bChange = TRUE;
            }
            else if(codeNotify == EN_SETFOCUS)
            {
                int nRecipientLen = 0;
                char szBtnText[100];

                pCreateData->hFocus = hTitleEdt; 
                pCreateData->hOldRecipient = hTitleEdt;
                pCreateData->OldCaretProc = SetCaretProc(MAIL_CaretProc);

                memset(szBtnText, 0x0, 100);
                SendMessage(pCreateData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szBtnText);
                nRecipientLen = GetWindowTextLength(hRecipientEdt);
                
                if((nRecipientLen == 0) && (stricmp(szBtnText, IDP_MAIL_BUTTON_SAVE) != 0))
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_SAVE);
                
                if((nRecipientLen != 0) && (stricmp(szBtnText, IDP_MAIL_BUTTON_SEND) != 0))
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_SEND);
            
                SendMessage(hContentEdt, EM_SETSEL, 0, 0);
            }
            else if(codeNotify == EN_KILLFOCUS)
            {
                HWND hFocus;
                
                pCreateData->hOldFocus = hTitleEdt;

                hFocus = GetFocus();
                if((hFocus != hRecipientEdt) && (hFocus != hRecipient2Edt) && (hFocus != hContentEdt))
                    SetCaretProc(NULL);
            }
        }
        break;
        
    case IDC_MAIL_EDIT_CONTENT:
        {
            if(codeNotify == EN_CHANGE)
            {
                pCreateData->bChange = TRUE;

                MAIL_SetMemIndicator(hWnd, pCreateData->nAttSize + GetWindowTextLength(hContentEdt));                
                SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
                    MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)pCreateData->hBitmapIcon);
            }
            else if(codeNotify == EN_SETFOCUS)
            {
                int nRecipientLen = 0;
                int nRecipient2Len = 0;
                int nTitleLen = 0;
                int nContentLen = 0;
                char szBtnText[100];

                pCreateData->hFocus = hContentEdt;
                pCreateData->hOldRecipient = hContentEdt;
                pCreateData->OldCaretProc = SetCaretProc(MAIL_CaretProc);

                memset(szBtnText, 0x0, 100);
                SendMessage(pCreateData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szBtnText);
                
                nRecipientLen = GetWindowTextLength(hRecipientEdt);
                nRecipient2Len = GetWindowTextLength(hRecipient2Edt);
                nTitleLen = GetWindowTextLength(hTitleEdt);
                nContentLen = GetWindowTextLength(hContentEdt);
                
                if((nRecipientLen == 0) && (stricmp(szBtnText, IDP_MAIL_BUTTON_SAVE) != 0))
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_SAVE);
                
                else if((nRecipientLen != 0) && (stricmp(szBtnText, IDP_MAIL_BUTTON_SEND) != 0))
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_SEND);
            }
            else if(codeNotify == EN_KILLFOCUS)
            {
                HWND hFocus;
                
                pCreateData->hOldFocus = hContentEdt;
                
                hFocus = GetFocus();
                if((hFocus != hRecipientEdt) && (hFocus != hRecipient2Edt) && (hFocus != hTitleEdt))
                    SetCaretProc(NULL);
            }
            else if(codeNotify == EN_MAXTEXT)
            {
                RECT rcEdt,rc;
                SIZE Size;
                HDC  hdc;
                int  nY,nLine;
                
                GetWindowRect(hContentEdt, &rcEdt);
                GetWindowRectEx(hWnd, &rc, W_CLIENT, XY_SCREEN);
                
                hdc = GetDC(hWnd);
                GetTextExtent(hdc, "L", 1, &Size);
                ReleaseDC(hWnd, hdc);
                
                nLine = 50;
                
                nY = nLine * (Size.cy + MAIL_EDIT_SPACE + 1);
                pCreateData->nHeight += nY;
                
                MoveWindow(hContentEdt, rcEdt.left, rcEdt.top - rc.top, rcEdt.right - rcEdt.left,
                    rcEdt.bottom - rcEdt.top + nY, TRUE);
            }
        }
        break;

    case ID_MAILBOX_MODIFY:
        {
            if(lParam != NULL)
            {
                //pCreateData->nAttLen = ((MailAttString *)lParam)->AttLen;
                pCreateData->nAttNum = ((MailAttString *)lParam)->AttNum;
                pCreateData->nAttSize = ((MailAttString *)lParam)->AttSize;
                memset(pCreateData->pszAttachment, 0x0, strlen(pCreateData->pszAttachment));
                strcpy(pCreateData->pszAttachment, ((MailAttString *)lParam)->AttName);
            }
            pCreateData->bChange = TRUE;
        }
        break;

    default:
        break;
	}
    
	return;
}
/*********************************************************************\
* Function	MailEdit_OnSetLBtnText
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailEdit_OnSetLBtnText(HWND hWnd, int nID, BOOL bEmpty, LPSTR pszText)
{
    PMAIL_EDITCREATEDATA pCreateData;

    char szBtnText[100];
    int nRecipientLen = 0;
    HWND hRecipientEdt = NULL;

    pCreateData = GetUserData(hWnd);
    hRecipientEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_RECIPIENT);

    memset(szBtnText, 0x0, 100);
    SendMessage(pCreateData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szBtnText);

    switch (nID)
    {
    case IDC_MAIL_EDIT_RECIPIENT:
    case IDC_MAIL_EDIT_RECIPIENT2:
        {
            if(stricmp(szBtnText, IDP_MAIL_BUTTON_ADD) != 0)
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_ADD);
        }
        break;

    case IDC_MAIL_EDIT_TITLE:
        {
            nRecipientLen = GetWindowTextLength(hRecipientEdt);
            
            if((nRecipientLen == 0) && (stricmp(szBtnText, IDP_MAIL_BUTTON_SAVE) != 0))
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_SAVE);
            
            if((nRecipientLen != 0) && (stricmp(szBtnText, IDP_MAIL_BUTTON_SEND) != 0))
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_SEND);
        }
        break;

    case IDC_MAIL_EDIT_CONTENT:
        {
            nRecipientLen = GetWindowTextLength(hRecipientEdt);
            
            if((nRecipientLen == 0) && (stricmp(szBtnText, IDP_MAIL_BUTTON_SAVE) != 0))
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_SAVE);
            
            else if((nRecipientLen != 0) && (stricmp(szBtnText, IDP_MAIL_BUTTON_SEND) != 0))
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_MAIL_BUTTON_SEND);
        }
        break;

    default:        
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)pszText);
        break;
    }

    return;
}
/*********************************************************************\
* Function	MailEdit_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailEdit_OnDestroy(HWND hWnd)
{
    PMAIL_EDITCREATEDATA pCreateData;
    HMENU hMenu;
    
    pCreateData = GetUserData(hWnd);
    
    hMenu = pCreateData->hMenu;
    //hMenu = PDAGetMenu(pCreateData->hFrameWnd);
    DestroyMenu(hMenu);

    if(pCreateData->hBitmapIcon)
    {
        DeleteObject(pCreateData->hBitmapIcon);
        pCreateData->hBitmapIcon = NULL;
    }
    if(pCreateData->MemoryDCIcon)
    {
        DeleteDC(pCreateData->MemoryDCIcon);
        pCreateData->MemoryDCIcon = NULL;
    }
    
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
    
    GARY_FREE(pCreateData->pszContent);
    GARY_FREE(pCreateData->pszTitle);
    GARY_FREE(pCreateData->pszRecipient2);
    GARY_FREE(pCreateData->pszRecipient);
    GARY_FREE(pCreateData->pszAttachment);
    GARY_FREE(pCreateData->pszOldAtt);

    HwndMailEdit = NULL;

	//UnRegisterMailEditClass();
#ifdef MAIL_DEBUG
    EndObjectDebug();
#endif

    return;
}
/*********************************************************************\
* Function	MailEdit_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MailEdit_OnClose(HWND hWnd)
{
    PMAIL_EDITCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);

    if(pCreateData->handle == -2)
    {
        SendMessage(pCreateData->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)0, 0);
        DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)(HINSTANCE)pCreateData->folderid);
    }
    
    DestroyWindow(hWnd);

    return;
}
/*********************************************************************\
* Function	MailEdit_OnMoveWindow
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void MailEdit_OnMoveWindow(HWND hWnd, WPARAM wParam)
{
    PMAIL_EDITCREATEDATA pCreateData;

    HWND hRecipientEdt = NULL;
    HWND hRecipient2Edt = NULL;
    HWND hTitleEdt = NULL;
    HWND hContentEdt = NULL;
    HWND hFocus = NULL;
    HWND hParent = NULL;

    RECT rc;
    RECT rcReci, rcRec2, rcTit, rcEdt;

    pCreateData = GetUserData(hWnd);

    hRecipientEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_RECIPIENT);
    hRecipient2Edt = GetDlgItem(hWnd, IDC_MAIL_EDIT_RECIPIENT2);
    hTitleEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_TITLE);
    hContentEdt = GetDlgItem(hWnd,IDC_MAIL_EDIT_CONTENT);
    hFocus = GetFocus();
    hParent = GetParent(hFocus);
    if(hFocus != hWnd)
    {
        while(hParent != hWnd)
        {
            hFocus = hParent;
            hParent = GetParent(hFocus);
            if(hParent == 0)
            {
                return;
            }
        }
    }
    else
    {
        return;
    }
    
    GetClientRect(hWnd, &rc);
    GetWindowRect(hRecipientEdt, &rcReci);
    GetWindowRect(hRecipient2Edt, &rcRec2);
    GetWindowRect(hTitleEdt, &rcTit);
    GetWindowRect(hContentEdt, &rcEdt);
    
    if(hFocus == hRecipientEdt)
    {
        int y;

        if(pCreateData->hOldRecipient != hRecipient2Edt)
        {
            y = (int)wParam;
            MoveWindow(hRecipient2Edt, 0, y, rcRec2.right - rcRec2.left, RECIWND_HEIGHT, TRUE);
            y += RECIWND_HEIGHT;
            MoveWindow(hTitleEdt, 0, y, rcTit.right - rcTit.left, RECIWND_HEIGHT, TRUE);
            y += RECIWND_HEIGHT;
            MoveWindow(hContentEdt, 0, y, rcEdt.right - rcEdt.left, rcEdt.bottom - rcEdt.top, TRUE);
        }
        else
        {
            y = (int)wParam - RECIWND_HEIGHT - (rcRec2.bottom - rcRec2.top);
            MoveWindow(hRecipientEdt, 0, y, rcReci.right - rcReci.left, RECIWND_HEIGHT, TRUE);
            y = (int)wParam;
            MoveWindow(hTitleEdt, 0, y, rcTit.right - rcTit.left, RECIWND_HEIGHT, TRUE);
            y += RECIWND_HEIGHT;
            MoveWindow(hContentEdt, 0, y, rcEdt.right - rcEdt.left, rcEdt.bottom - rcEdt.top, TRUE);

            pCreateData->hOldRecipient = hRecipientEdt;
        }
    }
    else if(hFocus == hRecipient2Edt)
    {
        int y;

        if(pCreateData->hOldRecipient != hRecipientEdt)
        {
            y = (int)wParam - RECIWND_HEIGHT - (rcRec2.bottom - rcRec2.top);
            MoveWindow(hRecipientEdt, 0, y, rcReci.right - rcReci.left, RECIWND_HEIGHT, TRUE);
            y = (int)wParam;
            MoveWindow(hTitleEdt, 0, y, rcTit.right - rcTit.left, RECIWND_HEIGHT, TRUE);
            y += RECIWND_HEIGHT;
            MoveWindow(hContentEdt, 0, y, rcEdt.right - rcEdt.left, rcEdt.bottom - rcEdt.top, TRUE);
        }
        else
        {
            y = (int)wParam;
            MoveWindow(hRecipient2Edt, 0, y, rcRec2.right - rcRec2.left, RECIWND_HEIGHT, TRUE);
            y += RECIWND_HEIGHT;
            MoveWindow(hTitleEdt, 0, y, rcTit.right - rcTit.left, RECIWND_HEIGHT, TRUE);
            y += RECIWND_HEIGHT;
            MoveWindow(hContentEdt, 0, y, rcEdt.right - rcEdt.left, rcEdt.bottom - rcEdt.top, TRUE);

            pCreateData->hOldRecipient = hRecipient2Edt;
        }
    }
    else if(hFocus == hTitleEdt)
    {
        int y;
        
        if(pCreateData->hOldRecipient == hRecipient2Edt)
        {
            y = (int)wParam - RECIWND_HEIGHT - (rcRec2.bottom - rcRec2.top);
            MoveWindow(hRecipientEdt, 0, y, rcReci.right - rcReci.left, RECIWND_HEIGHT, TRUE);
            y = (int)wParam;
            MoveWindow(hTitleEdt, 0, y, rcTit.right - rcTit.left, RECIWND_HEIGHT, TRUE);
            y += RECIWND_HEIGHT;
            MoveWindow(hContentEdt, 0, y, rcEdt.right - rcEdt.left, rcEdt.bottom - rcEdt.top, TRUE);

            pCreateData->hOldRecipient = hTitleEdt;
        }
    }
}
/*********************************************************************\
* Function	MailEdit_OnConfirm
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void MailEdit_OnConfirm(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMAIL_EDITCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);

    switch(lParam)
    {
    case 0:
        {
            if(pCreateData->nAttNum > 0)
            {
                MAIL_DeleteAttachment(pCreateData->pszAttachment);
            }
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;

    case 1:
        {
            if(MAIL_SaveToBox(hWnd, MU_DRAFT))
            {
                MAIL_DeleteAttachment(pCreateData->pszAttachment);
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
        }
        break;

    default:
        break;
    }

    return;
}

/*********************************************************************\
* Function	MailEdit_OnAddress
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void MailEdit_OnAddress(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMAIL_EDITCREATEDATA pCreateData;
    int num = 0, i = 0;
    
    if (LOWORD(wParam) == FALSE)
        return;
    
    pCreateData = GetUserData(hWnd);
    
    SendMessage(pCreateData->hFocus, REC_CLEAR, 0, 0);
    
    num = HIWORD(wParam);
    for (i = 0; i < num; i++)
    {
        SendMessage(pCreateData->hFocus, GHP_ADDREC, 0, (LPARAM)&pCreateData->ABAddr[i]);
    }
    return;
}

/*********************************************************************\
* Function	MailEdit_OnInsertAtt
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void MailEdit_OnInsertAtt(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMAIL_EDITCREATEDATA pCreateData;
    HWND hContentEdt;
    char szNewAttFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    char szShowName[MAX_MAIL_SHOW_FILENAME_LEN + 1];
    char *InsertFileName;
    int AttSize;

    pCreateData = GetUserData(hWnd);
    
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

    if(pCreateData->bReturn == INSERT_TEMLATE)
    {
        int allLen;
        int nSrcLen;

        hContentEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_CONTENT);
        //SetWindowText(hContentEdt, pCreateData->pszContent);

        nSrcLen = GetWindowTextLength(hContentEdt);
        allLen = nSrcLen + (int)(LOWORD(wParam));
        if (allLen > MAX_MAIL_BODY_SIZE)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MAXTEXTSIZE, NULL, 
                Notify_Alert, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            return;
        }

        if(pCreateData->nConLen < allLen + 1 )
        {
            GARY_FREE(pCreateData->pszContent);
            pCreateData->nConLen = allLen + 1 ;
            pCreateData->pszContent = (char*)malloc(pCreateData->nConLen);
            if(!pCreateData->pszContent)
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_INSERTFAILED, NULL, 
                    Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                return;
            }
            pCreateData->pszContent[0] = 0;
        }
        pCreateData->nInsertTemLen = (int)(LOWORD(wParam));
        pCreateData->pszInsertTem = (char *)malloc(pCreateData->nInsertTemLen + 1);
        if(pCreateData->pszInsertTem == NULL)
        {
            pCreateData->nInsertTemLen = 0;
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_INSERTFAILED, NULL, 
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            return;
        }
        memset(pCreateData->pszInsertTem, 0x0, pCreateData->nInsertTemLen + 1);
        strncpy(pCreateData->pszInsertTem, (char *)lParam, pCreateData->nInsertTemLen);
        PostMessage(hWnd, WM_MAIL_REPLACE_SEL, NULL, NULL);

        return;
    }

    AttSize = MAIL_GetMailAttachmentSize(InsertFileName);
    if((AttSize + pCreateData->nAttSize) > MAX_MAIL_ATT_SIZE)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MAXMAILSIZE, NULL, 
            Notify_Alert, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        return;
    }

    memset(szNewAttFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
    MAIL_GetNewAffixFileName(szNewAttFileName);
    //showname###savename***showname###savename\0
    if(pCreateData->nAttNum > 0)
    {
        strcat(pCreateData->pszAttachment, "***");
    }

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

    strcat(pCreateData->pszAttachment, szShowName);
    strcat(pCreateData->pszAttachment, "###");
    strcat(pCreateData->pszAttachment, szNewAttFileName);

    pCreateData->nAttSize += AttSize;
    //pCreateData->nAttLen = strlen(pCreateData->pszAttachment);
    pCreateData->nAttNum ++;

    pCreateData->bChange = TRUE;

    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_ATTINSERTED, NULL, 
        Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
    return;
}

/*********************************************************************\
* Function	MailEdit_OnReplaceSel
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void MailEdit_OnReplaceSel(HWND hWnd)
{
    PMAIL_EDITCREATEDATA pCreateData;
    HWND hContentEdt = NULL;
    int nSrcLen, allLen;

    pCreateData = GetUserData(hWnd);
    hContentEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_CONTENT);
    nSrcLen = GetWindowTextLength(hContentEdt);
    allLen = nSrcLen + pCreateData->nInsertTemLen;
    if (allLen > MAX_MAIL_BODY_SIZE)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MAXTEXTSIZE, NULL, 
            Notify_Alert, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);

        GARY_FREE(pCreateData->pszInsertTem);
        pCreateData->nInsertTemLen = 0;
        return;
    }
    if(pCreateData->nConLen < allLen + 1 )
    {
        GARY_FREE(pCreateData->pszContent);
        pCreateData->nConLen = allLen + 1 ;
        pCreateData->pszContent = (char*)malloc(pCreateData->nConLen);
        if(!pCreateData->pszContent)
        {
            GARY_FREE(pCreateData->pszInsertTem);
            pCreateData->nInsertTemLen = 0;
            return;
        }
        pCreateData->pszContent[0] = 0;
    }
    if(pCreateData->pszInsertTem == NULL)
    {
        pCreateData->nInsertTemLen = 0;
        return;
    }
    SendMessage(hContentEdt, EM_REPLACESEL, NULL, (WPARAM)pCreateData->pszInsertTem);
    GARY_FREE(pCreateData->pszInsertTem);
    pCreateData->nInsertTemLen = 0;
    GetWindowText(hContentEdt, pCreateData->pszContent, allLen);
    MAIL_SetMemIndicator(hWnd, pCreateData->nAttSize + GetWindowTextLength(hContentEdt));
    pCreateData->bChange = TRUE;
    
    return;
}

/*********************************************************************\
* Function	MailEdit_OnSetRightIcon
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void MailEdit_OnSetRightIcon(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMAIL_EDITCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);

    if(pCreateData->nAttNum == 0)
    {
        PDADefWindowProc(hWnd, WM_IMESWITCHED, wParam, lParam);
    }

    return;
}

/*********************************************************************\
* Function	MAIL_SaveToDraft
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL MAIL_SaveToBox(HWND hWnd, int BoxStyle)
{   
    PMAIL_EDITCREATEDATA pCreateData;
    HWND hRecipientEdt = NULL;
    HWND hRecipient2Edt = NULL;
    HWND hTitleEdt = NULL;
    HWND hContentEdt = NULL;
    
    int nLen;
    MailListNode *pTemp;

    pCreateData = GetUserData(hWnd);
    hRecipientEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_RECIPIENT);
    hRecipient2Edt = GetDlgItem(hWnd, IDC_MAIL_EDIT_RECIPIENT2);
    hTitleEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_TITLE);
    hContentEdt = GetDlgItem(hWnd,IDC_MAIL_EDIT_CONTENT);
    
    if(((pCreateData->bSaved == TRUE) && (pCreateData->bChange == TRUE)) 
        || ((pCreateData->bSaved == FALSE) && (pCreateData->bChange == TRUE)) || (BoxStyle == MU_OUTBOX))
    {
        char szFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];

        if(MAIL_IsFlashEnough() == 0)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DRAFTFULL, NULL, Notify_Failure, 
                IDP_MAIL_BOOTEN_OK, NULL, 20);
            return FALSE;
        }

        //recipient
        nLen = MAIL_RecipientGetTextLength(hRecipientEdt);
        if(nLen >= pCreateData->nReciLen)
        {
            GARY_FREE(pCreateData->pszRecipient);
            pCreateData->pszRecipient = (char *)malloc(nLen + 1);
            if(!pCreateData->pszRecipient)
                return FALSE;
        }
        pCreateData->nReciLen = nLen + 1;
        MAIL_RecipientGetText(hRecipientEdt, pCreateData->pszRecipient, nLen + 1);
        //recipient2
        nLen = MAIL_RecipientGetTextLength(hRecipient2Edt);
        if(nLen >= pCreateData->nRec2Len)
        {
            GARY_FREE(pCreateData->pszRecipient2);
            pCreateData->pszRecipient2 = (char *)malloc(nLen + 1);
            if(!pCreateData->pszRecipient2)
                return FALSE;
        }
        pCreateData->nRec2Len = nLen + 1;
        MAIL_RecipientGetText(hRecipient2Edt, pCreateData->pszRecipient2, nLen + 1);
        //title
        nLen = GetWindowTextLength(hTitleEdt);
        if(nLen >= pCreateData->nTitlLen)
        {
            GARY_FREE(pCreateData->pszTitle);
            pCreateData->pszTitle = (char *)malloc(nLen + 1);
            if(!pCreateData->pszTitle)
                return FALSE;
        }
        pCreateData->nTitlLen = nLen + 1;
        GetWindowText(hTitleEdt, pCreateData->pszTitle, nLen + 1);
        //content
        nLen = GetWindowTextLength(hContentEdt);
        if(nLen >= pCreateData->nConLen)
        {
            GARY_FREE(pCreateData->pszContent);
            pCreateData->pszContent = (char *)malloc(nLen + 1);
            if(!pCreateData->pszContent)
                return FALSE;
        }
        pCreateData->nConLen = nLen + 1;
        GetWindowText(hContentEdt, pCreateData->pszContent, nLen + 1);
        
        if((pCreateData->nConLen > 1) || (pCreateData->nReciLen > 1) 
            || (pCreateData->nRec2Len > 1) || (pCreateData->nTitlLen > 1)
            || (pCreateData->nAttLen > 1))
        {
            MailFile SaveFl;

            memset(&SaveFl, 0x0, sizeof(MailFile));
            memset(szFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
            
            if(MAIL_SaveFile(hWnd, szFileName, BoxStyle, &SaveFl))
            {
                int curbox;
                int BackAttSize;
                SYSTEMTIME TmpSysTime;
                FILETIME   TmpFileTime;
                MU_MsgNode msgnode;

                curbox = MU_GetCurFolderType();
                
                if(pCreateData->bSaved == FALSE && pCreateData->bChange == TRUE)//bNew
                {
                    nLen = strlen(szFileName);
                    nLen = nLen > MAX_MAIL_SAVE_FILENAME_LEN ? MAX_MAIL_SAVE_FILENAME_LEN : nLen;
                    strncpy(pCreateData->szFileName, szFileName, nLen);
                    pCreateData->szFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1] = 0;
                    
                    if(((BoxStyle == MU_DRAFT) && (curbox == MU_DRAFT)) 
                        || (BoxStyle == MU_OUTBOX) && (curbox == MU_OUTBOX))
                    {
                        pTemp = (MailListNode *)malloc(sizeof(MailListNode));
                        if(pTemp == NULL)
                        {
                            return FALSE;
                        }
                        memset(pTemp, 0x0, sizeof(MailListNode));
                        sprintf(pTemp->ListMailFileName, "%s", pCreateData->szFileName);
                        pTemp->ListSize = SaveFl.MailSize;
                        sprintf(pTemp->ListHandle, "%s", SaveFl.MsgId);
                        sprintf(pTemp->ListAddress, "%s", SaveFl.address);
                        sprintf(pTemp->ListSubject, "%s", SaveFl.Subject);
                        pTemp->ListAttFlag = (SaveFl.AttNum)?TRUE:FALSE;
                        pTemp->ListReadFlag = SaveFl.ReadFlag;
                        pTemp->ListSendFlag = SaveFl.SendStatus;
                        
                        memcpy(&TmpSysTime, &SaveFl.Date, sizeof(SYSTEMTIME));
                        MSG_STtoFT(&TmpSysTime, &TmpFileTime);
                        pTemp->ListMaskdate = TmpFileTime.dwLowDateTime;
                        pTemp->pNext = NULL;
                        
                        MAIL_AddBoxFileNode(&MailListCurHead, pTemp);

                        memset(&msgnode, 0x0, sizeof(MU_MsgNode));
                        msgnode.msgtype = MU_MSG_EMAIL;
                        msgnode.storage_type = MU_STORE_IN_FLASH;
                        strncpy(msgnode.addr, pTemp->ListAddress, MU_ADDR_MAX_LEN);
                        msgnode.handle = (DWORD)pTemp->ListHandle;
                        msgnode.maskdate = pTemp->ListMaskdate;
                        strncpy(msgnode.subject, pTemp->ListSubject, MU_SUBJECT_DIS_LEN);
                        msgnode.attachment = pTemp->ListAttFlag;
                        if(curbox == MU_OUTBOX)
                        {
                            msgnode.status = pTemp->ListSendFlag;
                        }
                        else
                        {
                            msgnode.status = pTemp->ListReadFlag;
                        }
                        
                        SendMessage(UniGlobalHwnd, PWM_MSG_MU_NEWMTMSG, 
                            MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)&msgnode);
                        
                        pCreateData->handle = (DWORD)pTemp->ListHandle;
                        pCreateData->folderid = BoxStyle;
                    }
                    
                    if(BoxStyle == MU_DRAFT)
                    {
                        MAIL_AddCountNode(&GlobalMailCountHead, MU_DRAFT, 0);                       
                    }
                    if(BoxStyle == MU_OUTBOX)
                    {
                        MAIL_AddCountNode(&GlobalMailCountHead, MU_OUTBOX, 0);
                    }
                }
                else
                {
                    /*char szOldDirPath[PATH_MAXLEN];

                    szOldDirPath[0] = 0;                    
                    getcwd(szOldDirPath,PATH_MAXLEN);  
                    chdir(MAIL_FILE_PATH);                      
                    //delete older file
                    MAIL_DeleteFile(pCreateData->szFileName);
                    chdir(szOldDirPath);*/

                    if((curbox == MU_DRAFT) && (BoxStyle == MU_DRAFT))
                    {
                        MAIL_GetNodeByHandle(&MailListCurHead, &pTemp, (char *)pCreateData->handle);
                        
                        sprintf(pTemp->ListMailFileName, "%s", szFileName);
                        pTemp->ListSize = SaveFl.MailSize;
                        //sprintf(pTemp->ListHandle, "%s", SaveFl.MsgId);
                        sprintf(pTemp->ListAddress, "%s", SaveFl.address);
                        sprintf(pTemp->ListSubject, "%s", SaveFl.Subject);
                        pTemp->ListAttFlag = (SaveFl.AttNum)?TRUE:FALSE;
                        pTemp->ListReadFlag = SaveFl.ReadFlag;
                        pTemp->ListSendFlag = SaveFl.SendStatus;
                        
                        memcpy(&TmpSysTime, &SaveFl.Date, sizeof(SYSTEMTIME));
                        MSG_STtoFT(&TmpSysTime, &TmpFileTime);
                        pTemp->ListMaskdate = TmpFileTime.dwLowDateTime;

                        memset(&msgnode, 0x0, sizeof(MU_MsgNode));
                        msgnode.msgtype = MU_MSG_EMAIL;
                        msgnode.storage_type = MU_STORE_IN_FLASH;
                        strncpy(msgnode.addr, pTemp->ListAddress, MU_ADDR_MAX_LEN);
                        msgnode.handle = (DWORD)pTemp->ListHandle;
                        msgnode.maskdate = pTemp->ListMaskdate;
                        strncpy(msgnode.subject, pTemp->ListSubject, MU_SUBJECT_DIS_LEN);
                        msgnode.attachment = pTemp->ListAttFlag;
                        if(curbox == MU_OUTBOX)
                        {
                            msgnode.status = pTemp->ListSendFlag;
                        }
                        else
                        {
                            msgnode.status = pTemp->ListReadFlag;
                        }

                        SendMessage(UniGlobalHwnd, PWM_MSG_MU_MODIFIED, 
                            MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)&msgnode);
                        
                        pCreateData->handle = (DWORD)(DWORD)pTemp->ListHandle;
                    }
                    if(BoxStyle == MU_OUTBOX)
                    {
                        MAIL_ModifyCountNode(&GlobalMailCountHead, MU_DRAFT, MU_OUTBOX, 0);

                        if(curbox == MU_OUTBOX)
                        {
                            MAIL_GetNodeByHandle(&MailListCurHead, &pTemp, (char *)pCreateData->handle);
                            
                            sprintf(pTemp->ListMailFileName, "%s", szFileName);
                            pTemp->ListSize = SaveFl.MailSize;
                            //sprintf(pTemp->ListHandle, "%s", SaveFl.MsgId);
                            sprintf(pTemp->ListAddress, "%s", SaveFl.address);
                            sprintf(pTemp->ListSubject, "%s", SaveFl.Subject);
                            pTemp->ListAttFlag = (SaveFl.AttNum)?TRUE:FALSE;
                            pTemp->ListReadFlag = SaveFl.ReadFlag;
                            pTemp->ListSendFlag = SaveFl.SendStatus;
                            
                            memcpy(&TmpSysTime, &SaveFl.Date, sizeof(SYSTEMTIME));
                            MSG_STtoFT(&TmpSysTime, &TmpFileTime);
                            pTemp->ListMaskdate = TmpFileTime.dwLowDateTime;

                            memset(&msgnode, 0x0, sizeof(MU_MsgNode));
                            msgnode.msgtype = MU_MSG_EMAIL;
                            msgnode.storage_type = MU_STORE_IN_FLASH;
                            strncpy(msgnode.addr, pTemp->ListAddress, MU_ADDR_MAX_LEN);
                            msgnode.handle = (DWORD)pTemp->ListHandle;
                            msgnode.maskdate = pTemp->ListMaskdate;
                            strncpy(msgnode.subject, pTemp->ListSubject, MU_SUBJECT_DIS_LEN);
                            msgnode.attachment = pTemp->ListAttFlag;
                            if(curbox == MU_OUTBOX)
                            {
                                msgnode.status = pTemp->ListSendFlag;
                            }
                            else
                            {
                                msgnode.status = pTemp->ListReadFlag;
                            }

                            SendMessage(UniGlobalHwnd, PWM_MSG_MU_MODIFIED, 
                                MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)&msgnode);

                            //pCreateData->handle = (DWORD)pTemp->ListHandle;
                        }
                        else if(curbox == MU_DRAFT)
                        {
                            int iCur;

                            iCur = MAIL_GetNodeByHandle(&MailListCurHead, &pTemp, (char *)pCreateData->handle);

                            SendMessage(UniGlobalHwnd, PWM_MSG_MU_DELETE, 
                                MAKEWPARAM(MU_ERR_SUCC, MU_MDU_EMAIL), (LPARAM)pTemp->ListHandle);

                            MAIL_DelBoxFileNode(&MailListCurHead, iCur);
                        }
                    }
                }

                //delete backup attachment
                if((pCreateData->bSaved) && (pCreateData->pszOldAtt != NULL))
                {
                    MAIL_DeleteAttachment(pCreateData->pszOldAtt);
                    GARY_FREE(pCreateData->pszOldAtt);
                }
                
                pCreateData->bSaved = TRUE;
                pCreateData->bChange = FALSE;

                if(BoxStyle == MU_OUTBOX)
                {
                    if(GetSIMState() == 0)
                    {
                        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSIM, NULL, Notify_Failure, 
                            IDP_MAIL_BOOTEN_OK, NULL, 20);
                    }

                    else if(strlen(GlobalMailConfigActi.GLMailBoxName) == 0)
                    {
                        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOMAILBOX, NULL, Notify_Failure, 
                            IDP_MAIL_BOOTEN_OK, NULL, 20);
                    }

                    else if(MAIL_AddSendMailNode(hWnd, szFileName, SaveFl.MsgId))
                    {
                        if(GlobalMailConfigHead.GLNetUsed)
                        {
                            if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SEND)
                            {
                            }
                            else if(GlobalMailConfigActi.GLSendMsg == MAIL_CONNECT_AUTO)
                            {
                                MailConfigNode *pTemp;

                                pTemp = GlobalMailConfigHead.pNext;
                                while(pTemp)
                                {
                                    if(strcmp(pTemp->GLMailBoxName, GlobalMailConfigActi.GLMailBoxName) == 0)
                                    {
                                        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDING, NULL, Notify_Info, 
                                            NULL, NULL, 20);

                                        MAIL_AddDialWaitNode(&GlobalMailConfigHead, pTemp, MAIL_CON_TYPE_SEND);
                                        break;
                                    }
                                    pTemp = pTemp->pNext;
                                }
                            }
                        }
                        else if(GlobalMailConfigActi.GLSendMsg == MAIL_CONNECT_AUTO)
                        {
                            GlobalMailConfigHead.GLNetUsed = TRUE;
                            GlobalMailConfigHead.GLNetFlag = MAIL_CON_TYPE_SEND;
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

                            //MAIL_GetToSendDial = FALSE;
                            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDING, NULL, Notify_Info, 
                                NULL, NULL, 20);
                            SendMessage(HwndMailSend, WM_MAIL_BEGIN_SEND, 0, 0);
                        }
                    }

                    else
                    {
                        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SENDFAIL, IDP_MAIL_TITLE_EMAIL, 
                            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                    }
                }
                else if(BoxStyle == MU_DRAFT)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVEDTODRAFT, NULL, 
                        Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                }

                //add attachment backup
                BackAttSize = strlen(pCreateData->pszAttachment);

                pCreateData->pszOldAtt = malloc(BackAttSize + 1);
                if(!pCreateData->pszOldAtt)
                {
                    PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOSPACE, IDP_MAIL_TITLE_EMAIL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);

                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    return FALSE;
                }
                else
                {
                    memset(pCreateData->pszOldAtt, 0x0, BackAttSize + 1);
                    strcpy(pCreateData->pszOldAtt, pCreateData->pszAttachment);
                    memset(pCreateData->pszAttachment, 0x0, BackAttSize + 1);
                    MAIL_CopyAttachment(pCreateData->pszAttachment, pCreateData->pszOldAtt, pCreateData->nAttNum);
                }
            }
            else
            {
                /*if(BoxStyle == MU_OUTBOX)
                {*/
                    PLXTipsWin(NULL, NULL, NULL, IDP_MAIL_STRING_MESNOTSAVE, NULL, 
                        Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                /*}
                else if(BoxStyle == MU_DRAFT)
                {
                    PLXTipsWin(NULL, NULL, NULL, IDP_MAIL_STRING_SAVEDFAIL, IDP_MAIL_TITLE_EMAIL, Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
                }*/
                return FALSE;
            }
        }
    }

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL MAIL_SaveFile(HWND hWnd, char* szFileName, int BoxStyle, MailFile *SaveFl)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    MailFile mSaveMail;
    SYSTEMTIME sy;
    int AttNum;
    char szSaveFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    PMAIL_EDITCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);
    GetLocalTime(&sy);

    szOldPath[0] = 0;
    getcwd(szOldPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    szFileName[0] = 0;
    memset(&mSaveMail, 0x0, sizeof(MailFile));

    if(pCreateData->bSaved)
    {
        MailFile TempFile;

        if(BoxStyle == MU_DRAFT)
        {
            strcpy(szFileName, pCreateData->szFileName);
        }
        else if(BoxStyle == MU_OUTBOX)
        {
            strcpy(szFileName, MAIL_OUT_FILE);
            MAIL_GetNewFileName(szFileName, MAX_MAIL_SAVE_FILENAME_LEN + 1);
            rename(pCreateData->szFileName, szFileName);
            strcpy(pCreateData->szFileName, szFileName);
        }

        f = GARY_open(szFileName, O_RDONLY, -1);
        if(f < 0)
        {
            printf("\r\nMAIL_SaveFile1 : open error = %d\r\n", errno);

            chdir(szOldPath);
            return FALSE;
        }
        memset(&TempFile, 0x0, sizeof(MailFile));
        read(f, &TempFile, sizeof(MailFile));
        GARY_close(f);

        strcpy(mSaveMail.MsgId, TempFile.MsgId);
    }
    else
    {
        if(BoxStyle == MU_DRAFT)
        {
            strcpy(szFileName, MAIL_DRAFT_FILE);
        }
        else if(BoxStyle == MU_OUTBOX)
        {
            strcpy(szFileName, MAIL_OUT_FILE);
        }
        MAIL_GetNewFileName(szFileName, MAX_MAIL_SAVE_FILENAME_LEN + 1);

        //msgid
        MAIL_GenerateMailID(szFileName, mSaveMail.MsgId);
    }
    
    //to and address
    strcpy(mSaveMail.To, pCreateData->pszRecipient);
    strcpy(mSaveMail.address, pCreateData->pszRecipient);
    /*if(strstr(mSaveMail.To, ";"))//many to
    {
        if(!MAIL_CheckMultiAddr(mSaveMail.To))
        {
            PLXTipsWin(NULL, NULL, NULL, IDP_MAIL_STRING_RECERROR, IDP_MAIL_TITLE_EMAIL, Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            return FALSE;
        }
    }
    else//only one to
    {
        if(!MAIL_CheckReceiverValid(mSaveMail.To))
        {
            PLXTipsWin(NULL, NULL, NULL, IDP_MAIL_STRING_RECERROR, IDP_MAIL_TITLE_EMAIL, Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            return FALSE;
        }
    }*/
    mSaveMail.MailSize = pCreateData->nReciLen;
    //cc
    strcpy(mSaveMail.Cc, pCreateData->pszRecipient2);
    /*if(strstr(mSaveMail.Cc, ";"))//many cc
    {
        if(!MAIL_CheckMultiAddr(mSaveMail.Cc))
        {
            PLXTipsWin(NULL, NULL, NULL, IDP_MAIL_STRING_RECERROR, IDP_MAIL_TITLE_EMAIL, Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            return FALSE;
        }
    }
    else//only one cc
    {
        if(!MAIL_CheckReceiverValid(mSaveMail.Cc))
        {
            PLXTipsWin(NULL, NULL, NULL, IDP_MAIL_STRING_RECERROR, IDP_MAIL_TITLE_EMAIL, Notify_Success, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            return FALSE;
        }
    }*/
    mSaveMail.MailSize += pCreateData->nRec2Len;
    //date and localdate
    memcpy(&mSaveMail.Date, &sy, sizeof(SYSTEMTIME));
    memcpy(&mSaveMail.LocalDate, &sy, sizeof(SYSTEMTIME));
    memcpy(&pCreateData->date, &sy, sizeof(SYSTEMTIME));
    mSaveMail.MailSize += 2 * sizeof(SYSTEMTIME);
    //subject
    strcpy(mSaveMail.Subject, pCreateData->pszTitle);
    mSaveMail.MailSize += strlen(pCreateData->pszTitle);
    //content
    mSaveMail.TextSize = strlen(pCreateData->pszContent);
    mSaveMail.MailSize += mSaveMail.TextSize;
    //attachment
    mSaveMail.AttFileNameLen = strlen(pCreateData->pszAttachment);
    mSaveMail.AttNum = pCreateData->nAttNum;
    AttNum = pCreateData->nAttNum;
    while(AttNum)
    {
        int tmpSize;

        memset(szSaveFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        MAIL_GetMailAttachmentSaveName(pCreateData->pszAttachment, szSaveFileName, AttNum - 1);
        tmpSize = MAIL_GetMailAttachmentSize(szSaveFileName);
        if(tmpSize == -1)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECERROR, IDP_MAIL_TITLE_EMAIL, 
                Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            return FALSE;
        }
        mSaveMail.MailSize += tmpSize;
        AttNum --;
    }

    strcpy(mSaveMail.From, "1");
    if(BoxStyle == MU_DRAFT)
    {
        mSaveMail.ReadFlag = MU_STU_DRAFT;
        mSaveMail.SendStatus = MU_STU_UNSENT;
    }
    else if(BoxStyle == MU_OUTBOX)
    {
        mSaveMail.ReadFlag = MU_STU_READ;
        if(GetSIMState() == 0)
        {
            mSaveMail.SendStatus = MU_STU_UNSENT;
        }
        else
        {
            mSaveMail.SendStatus = MU_STU_WAITINGSEND;
        }
    }
    memcpy(SaveFl, &mSaveMail, sizeof(MailFile));
    f = GARY_open(szFileName, O_RDWR | O_CREAT, S_IRWXU);
    if(f < 0)
    {
        printf("\r\nMAIL_SaveFile2 : open error = %d\r\n", errno);
        
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_SAVEDFAIL, IDP_MAIL_TITLE_EMAIL, 
            Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
        return FALSE;
    }
    
    write(f, &mSaveMail, sizeof(MailFile));
    write(f, pCreateData->pszContent, mSaveMail.TextSize);
    write(f, pCreateData->pszAttachment, mSaveMail.AttFileNameLen);
    GARY_close(f);

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	   MAIL_GenerateMailID
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void MAIL_GenerateMailID(char *szName, char *szMailID)
{
    SYSTEMTIME pSysTime;
    
    GetLocalTime(&pSysTime);
    
    sprintf(szMailID, "%04d%02d%02d%02d%02d%02d%s", 
        pSysTime.wYear,   pSysTime.wMonth, 
        pSysTime.wDay,    pSysTime.wHour,
        pSysTime.wMinute, pSysTime.wSecond,
        szName);
}
/*********************************************************************\
* Function	   MAIL_CheckReceiverValid
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MAIL_CheckReceiverValid(char *szReceiver)
{
	
	int    i, len, SwitchFlag, nIndex;
	char   *pBeforeAt, *pBetweenFirstDotAt, *pAfterFirstDot; 

	MAIL_TrimString(szReceiver);
	len = strlen(szReceiver);
	//1@1.1
	if(len < 5)
		return FALSE;
	
	pBeforeAt = malloc(len + 1);
	pBetweenFirstDotAt = malloc(len + 1);
	pAfterFirstDot = malloc(len + 1);
	if(pBeforeAt == NULL || pBetweenFirstDotAt == NULL || pAfterFirstDot == NULL)
	{
        GARY_FREE(pBeforeAt);
        GARY_FREE(pBetweenFirstDotAt);
        GARY_FREE(pAfterFirstDot);
		return FALSE;
	}
	memset(pBeforeAt, 0x0, len + 1);
	memset(pBetweenFirstDotAt, 0x0, len + 1);
	memset(pAfterFirstDot, 0x0, len + 1);
	
	SwitchFlag = 0;
 	nIndex = 0;
	for( i=0 ; i< len ; i++)
	{
		if(SwitchFlag == 0)
		{
			/* before '@'*/
			if(szReceiver[i] != MAIL_CHAR_AT)
			{
				pBeforeAt[nIndex] = szReceiver[i];
				nIndex ++;
			}
			else
			{
				SwitchFlag = 1;
				nIndex = 0;
			}
		}
		else
		{
			if(SwitchFlag == 1)
			{
				/* between '@' and first '.'*/
				if(szReceiver[i] != '.')
				{
					pBetweenFirstDotAt[nIndex] = szReceiver[i];
					nIndex ++;
				}
				else
				{
					SwitchFlag = 2;
					nIndex = 0;
				}
			}
			else
			{
				/* after first . */
				pAfterFirstDot[nIndex] = szReceiver[i];
				nIndex ++;
			}
		}
	}// end for

	len = strlen(pBeforeAt);
	if(len == 0)
	{
		GARY_FREE(pBeforeAt);
		GARY_FREE(pBetweenFirstDotAt);
		GARY_FREE(pAfterFirstDot);
		return FALSE;
	}
	for(i = 0; i < len; i++)
	{
		if(pBeforeAt[i] == MAIL_CHAR_AT || pBeforeAt[i] == 0x20)
		{
			GARY_FREE(pBeforeAt);
			GARY_FREE(pBetweenFirstDotAt);
			GARY_FREE(pAfterFirstDot);
			return FALSE;
		}
			
	}

	len = strlen(pBetweenFirstDotAt);
	if(len == 0)
	{
		GARY_FREE(pBeforeAt);
		GARY_FREE(pBetweenFirstDotAt);
		GARY_FREE(pAfterFirstDot);
		return FALSE;
	}
	for(i = 0; i < len; i++)
	{
		if(pBetweenFirstDotAt[i] == MAIL_CHAR_AT || pBetweenFirstDotAt[i] == 0x20)
		{
			GARY_FREE(pBeforeAt);
			GARY_FREE(pBetweenFirstDotAt);
			GARY_FREE(pAfterFirstDot);
			return FALSE;
		}
	}
	
	len = strlen(pAfterFirstDot);
	if(len == 0)
	{
		GARY_FREE(pBeforeAt);
		GARY_FREE(pBetweenFirstDotAt);
		GARY_FREE(pAfterFirstDot);
		return FALSE;
	}
	for(i = 0; i < len; i++)
	{
		if(pAfterFirstDot[i] == MAIL_CHAR_AT || pAfterFirstDot[i] == 0x20)
		{
			GARY_FREE(pBeforeAt);
			GARY_FREE(pBetweenFirstDotAt);
			GARY_FREE(pAfterFirstDot);
			return FALSE;
		}
	}
	GARY_FREE(pBeforeAt);
	GARY_FREE(pBetweenFirstDotAt);
	GARY_FREE(pAfterFirstDot);
	return TRUE;
}
/*********************************************************************\
* Function	   MAIL_CheckMultiAddr
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MAIL_CheckMultiAddr(char *strMailAddr)
{
    char tempAddr[MAX_MAIL_ADDRESS_SIZE + 2];
    char MailAddr[MAX_MAIL_ADDRESS_SIZE + 1];
    
    char *pFirst;
    char *pSecond;
    int len;
    
    memset(tempAddr , 0x0, MAX_MAIL_ADDRESS_SIZE + 2);
    memset(MailAddr, 0x0, MAX_MAIL_ADDRESS_SIZE + 1);
    
    strcpy(tempAddr, strMailAddr);
    
    len = strlen(tempAddr);
    
    if(tempAddr[len - 1] != ';')  //ensure the last char is ;
        tempAddr[len] = ';'; 
    
    while((pSecond = strstr(tempAddr , ";")) != NULL)
    {
        pFirst = tempAddr;
        strncpy(MailAddr , tempAddr, pSecond - pFirst);
        
        if(!MAIL_CheckReceiverValid(MailAddr))
            return FALSE;
        
        memset(MailAddr, 0x0, MAX_MAIL_ADDRESS_SIZE + 1);
        strcpy(MailAddr,tempAddr + (pSecond - pFirst) + 1);
        memset(tempAddr, 0x0, MAX_MAIL_ADDRESS_SIZE + 2);
        strcpy(tempAddr, MailAddr);
        memset(MailAddr, 0x0, MAX_MAIL_ADDRESS_SIZE + 1);
    }
    return TRUE;
}
/*********************************************************************\
* Function	   MAIL_EditInitVScroll
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void MAIL_EditInitVScroll(HWND hWnd)
{
    SCROLLINFO      vsi;
    PMAIL_EDITCREATEDATA pData;
    HWND            hEdt;
    int             nLine;
    
    pData = GetUserData(hWnd);
    
    hEdt = GetDlgItem(hWnd, IDC_MAIL_EDIT_CONTENT);
    nLine = SendMessage(hEdt, EM_GETLINECOUNT, 0, 0);
    memset(&vsi, 0, sizeof(SCROLLINFO));
   
    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 5;//6
    vsi.nMax   = nLine + 6 - 1;
    vsi.nPos   = 0;
    
    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
    
	return;
}
/*********************************************************************\
* Function	   MAIL_CaretProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void MAIL_CaretProc(const RECT* rc)
{
#define  CLASS_NAMELEN          100

   HWND hFocus, hParent;
   HWND hReci, hTit, hEdt;
   char pszClassName[CLASS_NAMELEN];
   PMAIL_EDITCREATEDATA pCreateData;
   RECT rcClient;
   RECT rcWindow;
   int  nLine;
   SCROLLINFO vsi;
   int nLoop;

   memset(pszClassName, 0x0, 100);

   hFocus = GetFocus();
   hParent = GetParent(hFocus);
   GetClassName(hParent, pszClassName, CLASS_NAMELEN);

   nLoop = 0;
   while(stricmp(pszClassName, "MailEditWndClass") != 0)
   {
       hFocus = hParent;
       hParent = GetParent(hFocus);
       GetClassName(hParent, pszClassName, CLASS_NAMELEN);
       nLoop ++;
       if(nLoop > 3)
       {
           return;
       }
   }

   hReci = GetDlgItem(hParent, IDC_MAIL_EDIT_RECIPIENT);
   hTit = GetDlgItem(hParent, IDC_MAIL_EDIT_TITLE);
   hEdt = GetDlgItem(hParent, IDC_MAIL_EDIT_CONTENT);
   pCreateData = GetUserData(hParent);

   GetWindowRectEx(hParent, &rcClient, W_CLIENT, XY_SCREEN);

   if(rcClient.bottom < rc->bottom) //down
   {
       if((pCreateData->hOldFocus == hReci) && (hFocus == hEdt))
       {
           int  nY;
           
           nY = rc->bottom - rcClient.bottom;
           nY ++;
           ScrollWindow(hParent, 0, -nY, NULL, NULL);
           InvalidateRect(hTit, NULL, TRUE);
           UpdateWindow(hParent);
       }
       else
       {
           SIZE Size;
           HDC  hdc;
           int  nY;
           
           hdc = GetDC(hParent);
           GetTextExtent(hdc, "L", 1, &Size);
           ReleaseDC(hParent,hdc);
           
           //nY = Size.cy + MAIL_EDIT_SPACE;
           nY = 30;
           ScrollWindow(hParent, 0, -nY, NULL, NULL);
           UpdateWindow(hParent);
       }
   }
   else if(rcClient.top >= rc->top) //up
   {
       if((pCreateData->hOldFocus == hEdt) && (hFocus == hReci))
       {
           RECT rect;
           int  nY;
           
           GetWindowRect(hFocus, &rect);
           nY = rcClient.top - rect.top;   
           ScrollWindow(hParent, 0, nY, NULL, NULL);
           UpdateWindow(hParent);
       }
       else
       {
           SIZE Size;
           HDC  hdc;
           int  nY;
           
           hdc = GetDC(hParent);
           GetTextExtent(hdc, "L", 1, &Size);
           ReleaseDC(hParent, hdc);
           
           //nY = Size.cy + MAIL_EDIT_SPACE;
           nY = 30;

           ScrollWindow(hParent, 0, nY, NULL, NULL);
           UpdateWindow(hParent);
       }
   }

   memset(&vsi, 0, sizeof(SCROLLINFO));
   vsi.fMask  = SIF_RANGE ;
   GetScrollInfo(hParent, SB_VERT, &vsi);
   
   nLine = SendMessage(hEdt, EM_GETLINECOUNT, 0, 0);
   if((nLine + 6 - 1) != vsi.nMax)
   {
       vsi.nMax = nLine + 6 - 1;
       vsi.fMask  = SIF_RANGE;
       SetScrollInfo(hParent, SB_VERT, &vsi, TRUE); 
   }
   
   GetWindowRectEx(hReci, &rcWindow, W_WINDOW, XY_SCREEN);

   memset(&vsi, 0, sizeof(SCROLLINFO));
   vsi.fMask  = SIF_ALL;
   GetScrollInfo(hParent, SB_VERT, &vsi); 
   vsi.nPos = (rcClient.top - rcWindow.top) / (RECIWND_HEIGHT / 2);
   if((rcClient.top - rcWindow.top) % (RECIWND_HEIGHT / 2))
   {
       vsi.nPos ++;
   }
   vsi.fMask  = SIF_POS;
   SetScrollInfo(hParent, SB_VERT, &vsi, TRUE);

   return;
}

static void MAIL_RecipientGetText(HWND hEdt, PSTR pszText, int nMaxCount)
{
    RECIPIENTLISTBUF Recipient;
    PRECIPIENTLISTNODE pTemp = NULL;
    int i;
    
    memset(&Recipient, 0, sizeof(RECIPIENTLISTBUF));
    SendMessage(hEdt, GHP_GETREC, 0, (LPARAM)&Recipient);
    pTemp = Recipient.pDataHead;
    
    pszText[0] = 0;
    for(i = 0; i < Recipient.nDataNum ; i++)
    {
        strcat(pszText, pTemp->szPhoneNum);
        strcat(pszText, ";");
        pTemp = pTemp->pNext;
    }
    
    return ;

    //GetWindowText(hEdt, pszText, nMaxCount);
    //SendMessage(hEdt, GHP_GETREC, (WPARAM)nMaxCount, (LPARAM)pszText);
}

static int MAIL_RecipientGetTextLength(HWND hEdt)
{
    RECIPIENTLISTBUF Recipient;
    PRECIPIENTLISTNODE pTemp = NULL;
    int nLen,i;
    
    memset(&Recipient, 0, sizeof(RECIPIENTLISTBUF));
    SendMessage(hEdt, GHP_GETREC, 0, (LPARAM)&Recipient);
    pTemp = Recipient.pDataHead;

    nLen = 0;
    for(i = 0; i < Recipient.nDataNum ; i++)
    {
        nLen += strlen(pTemp->szPhoneNum) + 1;
        pTemp = pTemp->pNext;
    }
    
    return nLen;

    //return(GetWindowTextLength(hEdt));
    //return(SendMessage(hEdt, GHP_GETRECLEN, NULL, NULL));
}

BOOL MAIL_CreateEditInterface(HWND hFrameWindow, char *PSZRECIPIENT, 
                              char *PSZATTENMENT, char *PSZATTNAME, BOOL bAtt)
{
    SYSTEMTIME NowTime;
    int JudgRet;

    JudgRet = MAIL_JudgeEdit();
    if(JudgRet == -1)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOMAILBOX, IDP_MAIL_TITLE_EMAIL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return FALSE;
    }
    else if(JudgRet == 0)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_INVALIDMAILSET, IDP_MAIL_TITLE_EMAIL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return FALSE;
    }

    GetLocalTime(&NowTime);

    if(bAtt)
    {
        char szNewAttFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
        char szAttName[MAX_MAIL_SHOW_FILENAME_LEN + MAX_MAIL_SAVE_FILENAME_LEN + 1];

        memset(szAttName, 0x0, MAX_MAIL_SHOW_FILENAME_LEN + MAX_MAIL_SAVE_FILENAME_LEN + 1);
        memset(szNewAttFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        MAIL_GetNewAffixFileName(szNewAttFileName);
        //showname###savename***showname###savename\0
        strcpy(szAttName, PSZATTNAME);
        strcat(szAttName, "###");
        strcat(szAttName, szNewAttFileName);

        if(!MAIL_CopyFile(PSZATTENMENT, szNewAttFileName))
        {
            return FALSE;
        }
        CreateMailEditWnd(hFrameWindow, NULL, NULL, NULL, NULL, 
            szAttName, &NowTime, -1, -1);
    }
    else
    {
        CreateMailEditWnd(hFrameWindow, PSZRECIPIENT, NULL, NULL, 
            NULL, NULL, &NowTime, -1, -1);
    }

    return TRUE;
}

BOOL MAIL_CreateMultiInterface(HWND hFrameWindow, PLISTATTNODE InsertAtt)
{
    SYSTEMTIME NowTime;
    PLISTATTNODE TempAtt;
    char szAttName[MAX_MAIL_BODY_SIZE];
    int Attsize;
    int JudgRet;
    
    JudgRet = MAIL_JudgeEdit();
    if(JudgRet == -1)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOMAILBOX, IDP_MAIL_TITLE_EMAIL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return FALSE;
    }
    else if(JudgRet == 0)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_INVALIDMAILSET, IDP_MAIL_TITLE_EMAIL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return FALSE;
    }
    
    GetLocalTime(&NowTime);
    TempAtt = InsertAtt;

    Attsize = 0;
    memset(szAttName, 0x0, MAX_MAIL_BODY_SIZE);

    while(TempAtt)
    {
        struct stat SourceStat;
        char szNewAttFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
        
        if(stat(TempAtt->AttPath, &SourceStat) == -1)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, 
                Notify_Alert, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            
            MAIL_DeleteAttachment(szAttName);
            return FALSE;
        }
        Attsize += SourceStat.st_size;

        if(Attsize > MAX_MAIL_ATT_SIZE)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_MAXMAILSIZE, NULL, 
                Notify_Alert, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            
            MAIL_DeleteAttachment(szAttName);
            return FALSE;
        }

        memset(szNewAttFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        MAIL_GetNewAffixFileName(szNewAttFileName);

        if(!MAIL_CopyFile(TempAtt->AttPath, szNewAttFileName))
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DRAFTFULL, NULL, 
                Notify_Alert, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);

            MAIL_DeleteAttachment(szAttName);
            return FALSE;
        }

        //showname###savename***showname###savename\0
        strcat(szAttName, TempAtt->AttName);
        strcat(szAttName, "###");
        strcat(szAttName, szNewAttFileName);
        if(TempAtt->pNext != NULL)
        {
            strcat(szAttName, "***");
        }
        
        TempAtt = TempAtt->pNext;
    }

    CreateMailEditWnd(hFrameWindow, NULL, NULL, NULL, NULL, 
        szAttName, &NowTime, -1, -1);
        
    return TRUE;
}

/*********************************************************************\
* Function	   MAIL_SetMemIndicator
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void MAIL_SetMemIndicator(HWND hWnd, int nFrag)//, int nRemain
{
	HDC			hdc = NULL;
	HBITMAP		hBitmap = NULL;
	char        szFrag[16];//, szRemain[16];
	PMAIL_EDITCREATEDATA  pCreateData;
    HFONT       hFont = NULL;
	RECT        rect;
	SIZE        sz1;//, sz2;
	int         nOldMode = 0;
    int         AttSize;

	pCreateData = GetUserData(hWnd);

	hdc = GetDC(hWnd);

	if (pCreateData->MemoryDCIcon == NULL)
		pCreateData->MemoryDCIcon = CreateCompatibleDC(hdc);
    
	GetFontHandle(&hFont, SMALL_FONT);
    SelectObject(pCreateData->MemoryDCIcon, hFont);

    if ((AttSize = nFrag/1024) == 0)
    {
        AttSize = nFrag * 10 / 1024;
        sprintf(szFrag, "0.%d K", AttSize);
    }
    else
    {
        sprintf(szFrag, "%d K", AttSize);
    }
    //sprintf(szFrag, "(%d)", nFrag);
    //sprintf(szRemain, "%d", nRemain);

	GetTextExtentPoint(pCreateData->MemoryDCIcon, szFrag, -1, &sz1);
	//GetTextExtentPoint(pCreateData->MemoryDCIcon, szRemain, -1, &sz2);

    SetRect(&rect, 0, 0, sz1.cx, LR_ICON_HEIGHT);
	//SetRect(&rect, 0, 0, sz1.cx, sz1.cy);
    //SetRect(&rect, 0, 0, max(sz1.cx, sz2.cx), sz1.cy + sz2.cy - 4);

	if (pCreateData->hBitmapIcon != NULL)
    {
        DeleteObject(pCreateData->hBitmapIcon);
        pCreateData->hBitmapIcon = NULL;
    }

    pCreateData->hBitmapIcon = CreateCompatibleBitmap(pCreateData->MemoryDCIcon, 
		rect.right, rect.bottom);
    
    SelectObject(pCreateData->MemoryDCIcon, pCreateData->hBitmapIcon);

	ClearRect(pCreateData->MemoryDCIcon, &rect, COLOR_TRANSBK);

	nOldMode = SetBkMode(pCreateData->MemoryDCIcon, TRANSPARENT);
	TextOut(pCreateData->MemoryDCIcon, 0, 0, szFrag, -1);
	//TextOut(pCreateData->MemoryDCIcon, 0, sz1.cy - 4, szRemain, -1);
	SetBkMode(pCreateData->MemoryDCIcon, nOldMode);

	ReleaseDC(hWnd, hdc);

    return;
}

/*********************************************************************\
* Function	   MAIL_SetAttIndicator
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void MAIL_SetAttIndicator(HWND hWnd, int nFrag)
{
	HDC			hdc = NULL;
	HBITMAP		hBitmap = NULL;
	char        szFrag[16];
	PMAIL_EDITCREATEDATA  pCreateData;
    HFONT       hFont = NULL;
	RECT        rect;
	SIZE        sz1;//, sz2;
	int         nOldMode = 0;

	pCreateData = GetUserData(hWnd);
	
	hdc = GetDC(hWnd);

	if (pCreateData->AttDCIcon == NULL)
		pCreateData->AttDCIcon = CreateCompatibleDC(hdc);

	GetFontHandle(&hFont, SMALL_FONT);
    SelectObject(pCreateData->AttDCIcon, hFont);

    sprintf(szFrag, "%d", nFrag);

	GetTextExtentPoint(pCreateData->AttDCIcon, szFrag, -1, &sz1);

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
	TextOut(pCreateData->AttDCIcon, 0, 0, szFrag, -1);
	SetBkMode(pCreateData->AttDCIcon, nOldMode);

	ReleaseDC(hWnd, hdc);

    return;
}

void MAIL_SetNumber(HWND hWnd, int leftNum, int rightNum)
{
    PMAIL_EDITCREATEDATA  pCreateData;
    
    pCreateData = GetUserData(hWnd);

    MAIL_SetMemIndicator(hWnd, leftNum);
    SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)pCreateData->hBitmapIcon);

    if(rightNum > 0)
    {
        MAIL_SetAttIndicator(hWnd, rightNum);
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)pCreateData->hAttIcon);
    }
    else
    {
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
            MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
    }
    return;
}

