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

#define IDM_SMSEDIT_PASTE               101
#define IDM_SMSEDIT_SEND                102
#define IDM_SMSEDIT_SAVETODRAFT         103
#define IDM_SMSEDIT_ADDRECIPIENT        104
#define IDM_SMSEDIT_INSERTTEMPLATE      105
#define IDM_SMSEDIT_MSGINFO             106
#define IDM_SMSEDIT_OPTIONS             107 
#define IDM_SMSEDIT_CANCEL              108     
#define IDC_EXIT                        200
#define IDC_SMSEDIT_RECIPIENT           300
#define IDC_SMSEDIT_CONTENT             400

#define SMSEDIT_RECIPIENT_MAXLEN        SMS_MALLOC_LEN
#define SMSEDIT_CONTENT_MAXLEN          SMS_MALLOC_LEN

#define SMSEDIT_RECIPIENT_MAXNUM        20

#define SMSEDIT_TIMER_F1                1

static HWND hEditWnd = NULL;
static HWND hTelMsgWnd = NULL;
static UINT uTelMsgCmd = 0;

LRESULT SMSEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL SMSEdit_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void SMSEdit_OnActivate(HWND hwnd, UINT state);
static void SMSEdit_OnSetFocus(HWND hWnd);
static void SMSEdit_OnInitmenu(HWND hwnd);
static void SMSEdit_OnPaint(HWND hWnd);
static void SMSEdit_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void SMSEdit_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void SMSEdit_OnDestroy(HWND hWnd);
static void SMSEdit_OnClose(HWND hWnd);
static void SMSEdit_OnMoveWindow(HWND hWnd,WPARAM wParam);
static void SMSEdit_OnAddRecipient(HWND hWnd,BOOL bAdd,int nCount,ABNAMEOREMAIL* pArray);
static void SMSEdit_OnSave(HWND hWnd,BOOL bSave);
static BOOL SMSEdit_OnSaveToDraft(HWND hWnd);
static void SMSEdit_OnSureFragSend(HWND hWnd,BOOL bSure);
static void SMSEdit_OnSend(HWND hWnd);
static void SMSEdit_OnSaveSetting(HWND hWnd,BOOL bSave,SMS_SETTINGCHAIN* pSettingChain);
static void SMSEdit_OnSendWithOptions(HWND hWnd,BOOL bSend,SMS_SETTING * pSettingChain);
static void SMSEdit_OnInsertTemplate(HWND hWnd,int nLen,char* pszInsertString);
static void SMSEdit_InitVScroll(HWND hWnd);
static void SMSEdit_OnSetLBtnText(HWND hWnd, int nID, BOOL bEmpty, LPSTR pszText);
static BOOL SMS_EvaluateFragNum(char* pszContent,int nLen, int *pnFrag,int MsgType);
static BOOL SMS_CheckPhone(char* pPhoneString);
static BOOL SMS_CheckSetting(HWND hParent);
static void SMSEdit_OnReplaceSel(HWND hWnd);
static LRESULT NewEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
BOOL SMS_SaveFile(int folderid,SMS_STORE *psmsstore,char* szFileName,MU_MSG_TYPE nMsgType);
BOOL SMS_NewFileName(int folderid, char* pszFileName);

void SMS_CaretProc(const RECT* rc);

static int  SMS_RecipientGetTextLength(HWND hEdt);
static void SMS_RecipientGetText(HWND hEdt,PSTR pszText, int nMaxCount);
static int SMS_RecipientSetText(HWND hEdt,char* pszText);
static void SMS_SetIndicator(HWND hWnd,int nFrag, int nRemain);
/*********************************************************************\
* Function	   SMS_EditRegisterClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_EditRegisterClass(void)
{
    WNDCLASS wc;
        
    wc.style         = 0;
    wc.lpfnWndProc   = SMSEditWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(SMS_EDITCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "SMSEditWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}
/*********************************************************************\
* Function	   SMS_CreateEditWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_CreateEditWnd(HWND hFrameWnd,HWND hwndmu,const char* PSZRECIPIENT,const char* PSZCONTENT,int nConLen,
                       DWORD handle,int folderid,MU_MSG_TYPE nMsgType,HINSTANCE hWriteSMSInstance,BOOL bForward)
{
    SMS_EDITCREATEDATA CreateData;
	RECT			   rcClient;
    
    memset(&CreateData, 0, sizeof(SMS_EDITCREATEDATA));

    CreateData.bForward = bForward;
    CreateData.nMsgType = nMsgType;
    CreateData.hWriteSMSInstance = hWriteSMSInstance;
	CreateData.hFrameWnd = hFrameWnd;
    CreateData.hwndmu = hwndmu;
    CreateData.folderid = folderid;
    CreateData.handle = handle;
    CreateData.bChange = FALSE;
    CreateData.nContentLine = 1;
    if(handle != -1)
    {
        CreateData.bSaved = TRUE;
        strcpy(CreateData.szFileName,((PSMS_INDEX)handle)->szFileName);
    }
    else
        CreateData.bSaved = FALSE;

    if( PSZRECIPIENT && strlen(PSZRECIPIENT) > 0 )
    {
        CreateData.nReciLen = strlen(PSZRECIPIENT);
        CreateData.byMenuStatus |= MASK_MENUSTATUS_SEND;
        CreateData.byMenuStatus |= MASK_MENUSTATUS_SAVETODRAFT;
    }
    else
        CreateData.nReciLen = 0;
    
    CreateData.nReciLen = CreateData.nReciLen > SMS_MALLOC_LEN ? CreateData.nReciLen : SMS_MALLOC_LEN;
    CreateData.pszRecipient = (char*)malloc(CreateData.nReciLen+1);
    if(CreateData.pszRecipient)
    {
        CreateData.pszRecipient[0] = 0;
        CreateData.bChange = TRUE;
        if(PSZRECIPIENT)
            strcpy(CreateData.pszRecipient,PSZRECIPIENT);
        else
            memset(CreateData.pszRecipient,0,CreateData.nReciLen+1);
    }
    else
    {
        return FALSE;
    }

    
    if( PSZCONTENT )
    {
        CreateData.nConLen = nConLen;
        CreateData.byMenuStatus |= MASK_MENUSTATUS_SAVETODRAFT;
    }
    else
        CreateData.nConLen = 0;
    
    if(CreateData.nMsgType == MU_MSG_SMS)
        CreateData.nConLen = CreateData.nConLen > SMS_MALLOC_LEN ? CreateData.nConLen : SMS_MALLOC_LEN;
    else
        CreateData.nConLen = nConLen;
    
    CreateData.pszContent = (char*)malloc(CreateData.nConLen+1);
    if(CreateData.pszContent)
    {
        CreateData.pszContent[0] = 0;
        CreateData.bChange = TRUE;
        if(PSZCONTENT)
            strcpy(CreateData.pszContent,PSZCONTENT);
        else
            memset(CreateData.pszContent,0,CreateData.nConLen+1);
    }
    else
    {
        SMS_FREE(CreateData.pszRecipient);
        return FALSE;
    }

    if(folderid == MU_DRAFT && CreateData.bChange == TRUE)
        CreateData.bChange = FALSE;

	GetClientRect(hFrameWnd,&rcClient);
    
    CreateData.hMenu = CreateMenu();

    hEditWnd = CreateWindow(
        "SMSEditWndClass", 
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL,
        rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
        hFrameWnd,
        NULL,
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!hEditWnd)
    {
		DestroyMenu(CreateData.hMenu);

        SMS_FREE(CreateData.pszRecipient);
        SMS_FREE(CreateData.pszContent);

        return FALSE;
    }

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)"");

	PDASetMenu(hFrameWnd,CreateData.hMenu);
 
	SetFocus(hEditWnd);  
    
	SetWindowText(hFrameWnd,IDS_SMS);
    //show window
    ShowWindow(hEditWnd, SW_SHOW);
    UpdateWindow(hEditWnd);
    
//    if(CreateData.bForward && CreateData.nMsgType == MU_MSG_SMS)
//    {
//        if(CreateData.nConLen != 0)
//            SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)IDS_SAVE);
//        else
//            SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)"");
//    }
//    else
//        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)IDS_ADD);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);

    //SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return TRUE;
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
static LRESULT SMSEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = SMSEdit_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        SMSEdit_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

	case WM_SETFOCUS:
        SMSEdit_OnSetFocus(hWnd);
		break;
        
    case WM_INITMENU:
        SMSEdit_OnInitmenu(hWnd);
        break;

    case WM_PAINT:
        SMSEdit_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        SMSEdit_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        SMSEdit_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        SMSEdit_OnClose(hWnd);
        break;

    case WM_DESTROY:
        SMSEdit_OnDestroy(hWnd);
        break;

	case REC_SIZE:
        SMSEdit_OnMoveWindow(hWnd,wParam);
        break;

	case WM_ADDRECIPIENT:
        SMSEdit_OnAddRecipient(hWnd,(BOOL)(LOWORD(wParam)),
			(int)(HIWORD(wParam)),(ABNAMEOREMAIL*)lParam);
		break;

	case WM_SAVETODRAFT:
        SMSEdit_OnSave(hWnd,(BOOL)lParam);
		break;

	case WM_SUREFRAGSEND:
        SMSEdit_OnSureFragSend(hWnd,(BOOL)lParam);
		break;

	case WM_SAVEACTIVATE:
        SMSEdit_OnSaveSetting(hWnd,(BOOL)wParam,(SMS_SETTINGCHAIN*)lParam);
		break;

    case WM_SENDWITHOPTIONS:
        SMSEdit_OnSendWithOptions(hWnd,(BOOL)wParam,(SMS_SETTING *)lParam);
        break;

    case PWM_MSG_FORCE_CLOSE:
        SMSEdit_OnSaveToDraft(hWnd);
        break;

    case WM_INSERTTEMPLATE:
        //MAKEWPARAM(Len, RTN_QTEXT), (LPARAM)Content
        SMSEdit_OnInsertTemplate(hWnd,(int)LOWORD(wParam),(char*)lParam);
        break;

    case WM_SETLBTNTEXT:
        SMSEdit_OnSetLBtnText(hWnd, (int)LOWORD(wParam), (BOOL)HIWORD(wParam),(LPSTR)lParam);
        return 0;

    case WM_SMSREPLACESEL:
        SMSEdit_OnReplaceSel(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	SMSEdit_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL SMSEdit_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    PSMS_EDITCREATEDATA pCreateData;
    HWND hRecipientEdt = NULL;
    HWND hContentEdt = NULL;
    IMEEDIT ie;
    int  x,y,nWidth,nHeight;
    RECT rc;
    SIZE Size;
    HDC  hdc;
    int  nY,nLine = 0;
    int  nContentLen,nFrag,nRemain;

    pCreateData = GetUserData(hWnd);

    memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(SMS_EDITCREATEDATA));

    GetClientRect(hWnd,&rc);

    x = 0;
    y = 0;
    nWidth = rc.right - rc.left;
    nHeight = (rc.bottom - rc.top) * 2 / 5;

    hRecipientEdt = CreateWindow(
        WC_RECIPIENT, 
        "", 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        x,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_SMSEDIT_RECIPIENT, 
        NULL, 
        NULL);

    if(hRecipientEdt == NULL)
        return FALSE;

    SendMessage(hRecipientEdt,REC_SETMAXREC,SMSEDIT_RECIPIENT_MAXNUM,0);//?????65535

    SendMessage(hRecipientEdt, EM_SETTITLE, 0, (LPARAM)IDS_TO);
    if(strlen(pCreateData->pszRecipient) != 0)
    {
        SMS_RecipientSetText(hRecipientEdt,pCreateData->pszRecipient);
    }

      
    if(pCreateData->nMsgType == MU_MSG_SMS)
    {
        y = nHeight;
        
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
                int nLen = pCreateData->nConLen;
                int nLineChars = 0;

                GetTextExtentPoint(hdc, "W", 1, &Size);
                
                nLineChars = (nWidth - 8) / Size.cx;

                p = pCreateData->pszContent;
                while( nLen > 0 )
                {
#if 0
                    GetTextExtentExPoint(hdc,p,strlen(p),nWidth-4, &nFit,NULL,NULL);
                    nLine++;
                    p += nFit;
#else
                    nLine++;
                    p += nLineChars;
                    nLen -= nLineChars;
#endif
                }
            }
        }
        
        GetTextExtentPoint(hdc, "W", 1, &Size);

        ReleaseDC(hWnd,hdc);
        
        nY = Size.cy + SMS_EDIT_SPACE;
        
        nHeight = (nY + 1) * nLine + 30;
        
        pCreateData->nHeight = nHeight;
        
        memset(&ie, 0, sizeof(IMEEDIT));
        
        ie.hwndNotify	= (HWND)hWnd;    
        ie.dwAttrib	    = 0;                
        ie.dwAscTextMax	= 0;
        ie.dwUniTextMax	= 0;
        ie.wPageMax	    = 0;        
        ie.pszCharSet	= NULL;
        ie.pszTitle	    = NULL;
        ie.pszImeName	= NULL;
        
        hContentEdt = CreateWindow(
            "IMEEDIT", 
            "", 
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE ,
            x,y,nWidth,nHeight,
            hWnd, 
            (HMENU)IDC_SMSEDIT_CONTENT, 
            NULL, 
            (PVOID)&ie);
        
        if(hContentEdt == NULL)
            return FALSE;
        
//        SendMessage(hContentEdt,EM_LIMITTEXT,MAX_TXT_SIZE,0);
        
        SetWindowText(hContentEdt,pCreateData->pszContent);

        SendMessage(hContentEdt, EM_SETSEL, 0, 0);

        nContentLen = strlen(pCreateData->pszContent);
    }
    else
        nContentLen = pCreateData->nConLen;    
    
    nFrag = (nContentLen / 160)+1;
    
    nRemain = nContentLen % 160;
    
    nRemain = 160 - nRemain;
    
    SMS_SetIndicator(hWnd,nFrag,nRemain);

    SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
        MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)pCreateData->hBitmapIcon);
    
    if(pCreateData->bForward && pCreateData->nMsgType == MU_MSG_SMS)
        pCreateData->hFocus = hContentEdt;
    else
        pCreateData->hFocus = hRecipientEdt;
    
    SMSEdit_InitVScroll(hWnd);

    return TRUE;
    
}
/*********************************************************************\
* Function	SMSEdit_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSEdit_OnActivate(HWND hWnd, UINT state)
{
    PSMS_EDITCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);
	
	SetFocus(pCreateData->hFocus);

	PDASetMenu(pCreateData->hFrameWnd,pCreateData->hMenu);
             	
//    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
//    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
//
    SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)pCreateData->hBitmapIcon);
//    SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

	SetWindowText(pCreateData->hFrameWnd,IDS_SMS);

    return;
}
/*********************************************************************\
* Function	SMSEdit_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSEdit_OnSetFocus(HWND hWnd)
{
    PSMS_EDITCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);
	
	SetFocus(pCreateData->hFocus);
	
    return;
}
/*********************************************************************\
* Function	SMSEdit_OnInitmenu
* Purpose   WM_INITMENU message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSEdit_OnInitmenu(HWND hWnd)
{
    HWND hRecipientEdt = NULL;
    HWND hContentEdt = NULL;
    HMENU hMenu = NULL;
    PSMS_EDITCREATEDATA pCreateData;
    int  nMenuCount;

    pCreateData = GetUserData(hWnd);

    hRecipientEdt = GetDlgItem(hWnd,IDC_SMSEDIT_RECIPIENT);
    hContentEdt = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);
    
    hMenu = pCreateData->hMenu;

    nMenuCount = GetMenuItemCount(hMenu);

    while(nMenuCount > 0)
    {
        nMenuCount--;
        DeleteMenu(hMenu,nMenuCount,MF_BYPOSITION);
    }

    if(pCreateData->byMenuStatus & MASK_MENUSTATUS_SEND)
    {
        AppendMenu(hMenu,MF_ENABLED, IDM_SMSEDIT_SEND, IDS_SEND);
        AppendMenu(hMenu,MF_ENABLED, IDM_SMSEDIT_OPTIONS, IDS_OPTIONS);
    }
    
    if(pCreateData->byMenuStatus & MASK_MENUSTATUS_SAVETODRAFT)
    {
        AppendMenu(hMenu,MF_ENABLED, IDM_SMSEDIT_SAVETODRAFT, IDS_SAVETODRAFTS);
    }

    if(pCreateData->byMenuStatus & MASK_MENUSTATUS_ADDRECIPIENT)
    {
        AppendMenu(hMenu,MF_ENABLED, IDM_SMSEDIT_ADDRECIPIENT, IDS_ADDRECIPIENT);
    }

    if(pCreateData->byMenuStatus & MASK_MENUSTATUS_INSERT)
    {
        if(CanInsertTemplate() == TRUE)
        {
            AppendMenu(hMenu,MF_ENABLED, IDM_SMSEDIT_INSERTTEMPLATE, IDS_INSERTTEMPLATE);
        }
    }

    AppendMenu(hMenu,MF_ENABLED, IDM_SMSEDIT_MSGINFO, IDS_MSGINFO);

    return;

}
/*********************************************************************\
* Function	SMSEdit_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSEdit_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	SMSEdit_OnKey
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
static void SMSEdit_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
//    HWND hWndFocus = NULL;
    PSMS_EDITCREATEDATA pCreateData;
    HWND hRecipientEdt = NULL;
    HWND hContentEdt = NULL;
    int nLen;

    pCreateData = GetUserData(hWnd);

    hRecipientEdt = GetDlgItem(hWnd,IDC_SMSEDIT_RECIPIENT);
    hContentEdt = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);

	switch (vk)
	{
    case VK_F10:
    case VK_F2:
        if(pCreateData->nMsgType == MU_MSG_SMS)
        {
            if((GetWindowTextLength(hRecipientEdt) || GetWindowTextLength(hContentEdt)) 
                && ((pCreateData->bSaved == TRUE && pCreateData->bChange == TRUE) 
                || (pCreateData->bSaved == FALSE && pCreateData->bChange == TRUE) ))
            {
                PLXConfirmWinEx(pCreateData->hFrameWnd,hWnd,IDS_SAVE2DRAFTS, Notify_Request, 
                    IDS_SMS, IDS_YES, IDS_NO,WM_SAVETODRAFT);
                
                break;
            }
            PostMessage(hWnd,WM_CLOSE,0,0);

            if(pCreateData->hWriteSMSInstance && hTelMsgWnd != NULL)
                PostMessage(hTelMsgWnd,uTelMsgCmd,FALSE,0);
        }
        else
        {
            if((pCreateData->bSaved == TRUE && pCreateData->bChange == TRUE) 
                || (pCreateData->bSaved == FALSE && pCreateData->bChange == TRUE) )
            {
                PLXConfirmWinEx(pCreateData->hFrameWnd,hWnd,IDS_SAVE2DRAFTS, Notify_Request, 
                    IDS_SMS, IDS_YES, IDS_NO,WM_SAVETODRAFT);
                
                break;
            }
            PostMessage(hWnd,WM_CLOSE,0,0);

            if(pCreateData->hWriteSMSInstance && hTelMsgWnd != NULL)
                PostMessage(hTelMsgWnd,uTelMsgCmd,FALSE,0);
        }
		break;

    case VK_F5:
		PDADefWindowProc(pCreateData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        break;

    case VK_RETURN:
        if(pCreateData->nMsgType == MU_MSG_SMS)
        {
            if(GetFocus() == hContentEdt)
            {
                if(SMS_RecipientGetTextLength(hRecipientEdt) != 0 )
                    SendMessage(hWnd,WM_COMMAND,IDM_SMSEDIT_SEND,NULL);
                else if(GetWindowTextLength(hContentEdt) != 0)
                    SendMessage(hWnd,WM_COMMAND,IDM_SMSEDIT_SAVETODRAFT,NULL);
            }
            else
                SendMessage(hWnd,WM_COMMAND,IDM_SMSEDIT_ADDRECIPIENT,NULL);
        }
        else
        {
            SendMessage(hWnd,WM_COMMAND,IDM_SMSEDIT_ADDRECIPIENT,NULL);
        }
        break;

    case VK_F1:
        nLen = GetWindowTextLength(hRecipientEdt);
        if(nLen == 0)
            SendMessage(hWnd,WM_COMMAND,IDM_SMSEDIT_ADDRECIPIENT,NULL);
        else
            SendMessage(hWnd,WM_COMMAND,IDM_SMSEDIT_SEND,NULL);
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}

/*********************************************************************\
* Function	SMSEdit_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void SMSEdit_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    PSMS_EDITCREATEDATA pCreateData;
    HWND hRecipientEdt = NULL;
    HWND hContentEdt = NULL;
    int nLen;
    SMS_STORE smsstore;
    SYSTEMTIME sy;

    pCreateData = GetUserData(hWnd);
    hRecipientEdt = GetDlgItem(hWnd,IDC_SMSEDIT_RECIPIENT);
    hContentEdt = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);

	switch(id)
	{
    case IDM_SMSEDIT_SEND:
        {           
            int  nFrag;
            char szWarning[100];

            nLen = SMS_RecipientGetTextLength(hRecipientEdt);

            if(nLen >= pCreateData->nReciLen)
            {
                SMS_FREE(pCreateData->pszRecipient);
                pCreateData->pszRecipient = (char*)malloc(nLen+1);
                if(!pCreateData->pszRecipient)
                    return;
                pCreateData->pszRecipient[0] = 0;
            }
            pCreateData->nReciLen = nLen+1;

            SMS_RecipientGetText(hRecipientEdt,pCreateData->pszRecipient,nLen+1);

            if(SMS_CheckPhone(pCreateData->pszRecipient) == FALSE)
                return;
            
            if(pCreateData->nMsgType == MU_MSG_SMS)
            {
                nLen = GetWindowTextLength(hContentEdt);
                if(nLen >= pCreateData->nConLen)
                {
                    SMS_FREE(pCreateData->pszContent);
                    pCreateData->pszContent = (char*)malloc(nLen+1);
                    if(!pCreateData->pszContent)
                        return;
                    pCreateData->pszContent[0] = 0;
                }
                //pCreateData->nConLen = nLen+1;
                GetWindowText(hContentEdt,pCreateData->pszContent,nLen+1);
            }
            else
                nLen = pCreateData->nConLen;

            nFrag = 0;
            
            if(nLen > 120)
                SMS_EvaluateFragNum(pCreateData->pszContent,pCreateData->nConLen,&nFrag,pCreateData->nMsgType);
            else
                nFrag = 1;

            szWarning[0] = 0 ;

            sprintf(szWarning,IDS_NEEDFRAG,nFrag);
            
            if( nFrag > 1 )
                PLXConfirmWinEx(pCreateData->hFrameWnd,hWnd,szWarning, Notify_Request, IDS_SMS,
				IDS_YES, IDS_NO, WM_SUREFRAGSEND);
			else
				SMSEdit_OnSend(hWnd);
		}
        break;
        
    case IDM_SMSEDIT_SAVETODRAFT:
        SMSEdit_OnSaveToDraft(hWnd);
        break;
        
    case IDM_SMSEDIT_ADDRECIPIENT:
		{
			ABNAMEOREMAIL * pArray;
			int nMax,nCount;
            HWND hRecipientEdt = NULL;
            RECIPIENTLISTBUF Recipient;	
            PRECIPIENTLISTNODE pTemp = NULL;	
            int i = 0;

			nMax = SMSEDIT_RECIPIENT_MAXNUM;
            
			hRecipientEdt = GetDlgItem(hWnd,IDC_SMSEDIT_RECIPIENT);

			pArray = (ABNAMEOREMAIL*)malloc(sizeof(ABNAMEOREMAIL)*nMax);

			if(pArray == NULL)
				break;

            memset(pArray, 0, sizeof(ABNAMEOREMAIL) * nMax);
    
            memset(&Recipient,0,sizeof(RECIPIENTLISTBUF));
    
            SendMessage(hRecipientEdt,GHP_GETREC,0,(LPARAM)&Recipient);
            
            pTemp = Recipient.pDataHead;
            
            for(i = 0,nCount = 0; i < Recipient.nDataNum ; i++)
            {
                if(pTemp->bExistInAB)
                {
                    strcpy(pArray[nCount].szName, pTemp->szShowName);
                    strcpy(pArray[nCount].szTelOrEmail, pTemp->szPhoneNum);
                    nCount++;
                }
                pTemp = pTemp->pNext;
            }

			if(APP_GetMultiPhoneOrEmail(pCreateData->hFrameWnd,hWnd,WM_ADDRECIPIENT,
				PICK_NUMBER,pArray,nCount,nMax - (Recipient.nDataNum - nCount)) == FALSE)
				SMS_FREE(pArray);
		}
        break;
        
    case IDM_SMSEDIT_INSERTTEMPLATE: 
        APP_PreviewPhrase(pCreateData->hFrameWnd,hWnd,WM_INSERTTEMPLATE,IDS_SELECTTEMPLATE);
        break;
        
    case IDM_SMSEDIT_MSGINFO: 
        {           
            nLen = SMS_RecipientGetTextLength(hRecipientEdt);
            if(nLen >= pCreateData->nReciLen)
            {
                SMS_FREE(pCreateData->pszRecipient);
                pCreateData->pszRecipient = (char*)malloc(nLen+1);
                if(!pCreateData->pszRecipient)
                    return;
                pCreateData->pszRecipient[0] = 0;
            }
            pCreateData->nReciLen = nLen+1;
            
            SMS_RecipientGetText(hRecipientEdt,pCreateData->pszRecipient,nLen+1);

            if(pCreateData->nMsgType == MU_MSG_SMS)
            {
                nLen = GetWindowTextLength(hContentEdt);
                if(nLen >= pCreateData->nConLen)
                {
                    SMS_FREE(pCreateData->pszContent);
                    pCreateData->pszContent = (char*)malloc(nLen+1);
                    if(!pCreateData->pszContent)
                        return;
                    pCreateData->pszContent[0] = 0;
                }
                //pCreateData->nConLen = nLen+1;
                GetWindowText(hContentEdt,pCreateData->pszContent,nLen+1);
            }
            else
                nLen = pCreateData->nConLen;
            
            GetLocalTime(&sy);
            memset(&smsstore,0,sizeof(SMS_STORE));
            smsstore.fix.Conlen = nLen+1;
            smsstore.fix.Phonelen = pCreateData->nReciLen;
            smsstore.fix.dwDateTime = Datetime2INT(sy.wYear,sy.wMonth,sy.wDay,sy.wHour,sy.wMinute,sy.wSecond);
            smsstore.fix.Stat = MU_STU_DRAFT;
            smsstore.pszContent = pCreateData->pszContent;
            smsstore.pszPhone = pCreateData->pszRecipient;
            SMS_CreateDetailWnd(pCreateData->hFrameWnd,&smsstore);
        }
        break;

    case IDM_SMSEDIT_OPTIONS:
		SMS_CreateSetting(pCreateData->hFrameWnd,hWnd,WM_SENDWITHOPTIONS,FALSE);
        break;

    case IDM_SMSEDIT_CANCEL:
        if((GetWindowTextLength(hRecipientEdt) || GetWindowTextLength(hContentEdt)) 
            && ((pCreateData->bSaved == TRUE && pCreateData->bChange == TRUE) 
            || (pCreateData->bSaved == FALSE && pCreateData->bChange == TRUE)) )
        {
            //if(PLXConfirmWin(IDS_SAVE2DRAFTS, Notify_Request, IDS_SMS, IDS_YES, IDS_NO) == TRUE)
                SMSEdit_OnSaveToDraft(hWnd);
        }
        PostMessage(hWnd,WM_CLOSE,0,0);
        
        if(pCreateData->hWriteSMSInstance && hTelMsgWnd != NULL)
            PostMessage(hTelMsgWnd,uTelMsgCmd,FALSE,0);
        break;

    case IDC_SMSEDIT_RECIPIENT:
        if(codeNotify == EN_CHANGE)
        {
            int nRecipientLen = 0,nContentLen = 0;
            HWND hRecipientEdt = NULL,hContentEdt = NULL;
            
            pCreateData->bChange = TRUE;

            hRecipientEdt = GetDlgItem(hWnd,IDC_SMSEDIT_RECIPIENT);
            hContentEdt = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);
            
            nRecipientLen = GetWindowTextLength(hRecipientEdt);
            
            if(pCreateData->nMsgType == MU_MSG_SMS)
                nContentLen = GetWindowTextLength(hContentEdt);
            else
                nContentLen = pCreateData->nConLen;

            if(nRecipientLen > 0)
                pCreateData->byMenuStatus |= MASK_MENUSTATUS_SEND;
            else
                pCreateData->byMenuStatus &= ~MASK_MENUSTATUS_SEND;

            if(nRecipientLen == 0 && nContentLen == 0)
                pCreateData->byMenuStatus &= ~MASK_MENUSTATUS_SAVETODRAFT;
            else
                pCreateData->byMenuStatus |= MASK_MENUSTATUS_SAVETODRAFT;
			
        }
        else if(codeNotify == EN_SETFOCUS)
        {
            HWND hRecipientEdt = NULL,hContentEdt = NULL;
            char szBtnText[16];
            
			pCreateData->OldCaretProc = SetCaretProc(SMS_CaretProc);

            hRecipientEdt = GetDlgItem(hWnd,IDC_SMSEDIT_RECIPIENT);
            hContentEdt = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);
			
			pCreateData->hFocus = hRecipientEdt;
				
            SendMessage(pCreateData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szBtnText);
            
            if(strcmp(szBtnText,IDS_ADD) != 0 )
                SendMessage(pCreateData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_ADD);
			
            pCreateData->byMenuStatus |= MASK_MENUSTATUS_ADDRECIPIENT;
            pCreateData->byMenuStatus &= ~MASK_MENUSTATUS_INSERT;

            SendMessage(hContentEdt, EM_SETSEL, 0, 0);

        }
		else if(codeNotify == EN_KILLFOCUS)
		{			
            if(GetFocus() != GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT))
                SetCaretProc(NULL);
		}
        break;
        
    case IDC_SMSEDIT_CONTENT:
        if(codeNotify == EN_CHANGE)
        {
            int nRecipientLen = 0,nContentLen = 0;
            HWND hRecipientEdt = NULL,hContentEdt = NULL;
            char szBtnText[16];
            int  nFrag,nRemain;

            szBtnText[0] = 0;
            hRecipientEdt = GetDlgItem(hWnd,IDC_SMSEDIT_RECIPIENT);
            hContentEdt = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);
            SendMessage(pCreateData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szBtnText);
            
            nRecipientLen = GetWindowTextLength(hRecipientEdt);
            nContentLen = GetWindowTextLength(hContentEdt);

//            if(nRecipientLen == 0 && nContentLen == 0 && stricmp(szBtnText,"") != 0)
//                SendMessage(pCreateData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
//            
//            if(nRecipientLen != 0 && stricmp(szBtnText,IDS_SEND) != 0)
//                SendMessage(pCreateData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_SEND);
//            
//            if(nRecipientLen == 0 && nContentLen != 0 && stricmp(szBtnText,IDS_SAVE) != 0)
//                SendMessage(pCreateData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_SAVE);
            
            pCreateData->bChange = TRUE;

            if(nRecipientLen == 0 && nContentLen == 0)
                pCreateData->byMenuStatus &= ~MASK_MENUSTATUS_SAVETODRAFT;
            else
                pCreateData->byMenuStatus |= MASK_MENUSTATUS_SAVETODRAFT;

            //SMS_EvaluateFragNum(pCreateData->pszContent,&nFrag);
            
            nFrag = nContentLen / 160;

            nRemain = nContentLen % 160;

            nRemain = 160 - nRemain;
            
            SMS_SetIndicator(hWnd,nFrag+1,nRemain);
			
            SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, 
                MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)pCreateData->hBitmapIcon);
        }
        else if(codeNotify == EN_SETFOCUS)
        {
            int nRecipientLen = 0,nContentLen = 0;
            HWND hRecipientEdt = NULL,hContentEdt = NULL;
            char szBtnText[100];

			pCreateData->OldCaretProc = SetCaretProc(SMS_CaretProc);

            szBtnText[0] = 0;

            hRecipientEdt = GetDlgItem(hWnd,IDC_SMSEDIT_RECIPIENT);
            hContentEdt = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);
            
			pCreateData->hFocus = hContentEdt;
			
			SendMessage(pCreateData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szBtnText);
            
            nRecipientLen = GetWindowTextLength(hRecipientEdt);
            nContentLen = GetWindowTextLength(hContentEdt);
            
            if(nRecipientLen == 0 && nContentLen == 0 && stricmp(szBtnText,"") != 0)
                SendMessage(pCreateData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
            
            if(nRecipientLen != 0 && stricmp(szBtnText,IDS_SEND) != 0)
                SendMessage(pCreateData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_SEND);
            
            if(nRecipientLen == 0 && nContentLen != 0 && stricmp(szBtnText,IDS_SAVE) != 0)
                SendMessage(pCreateData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_SAVE);
			
            pCreateData->byMenuStatus &= ~MASK_MENUSTATUS_ADDRECIPIENT;
            pCreateData->byMenuStatus |= MASK_MENUSTATUS_INSERT;
            
        }
        else if(codeNotify == EN_MAXTEXT)
        {
            RECT rcEdt,rc;
            SIZE Size;
            HDC  hdc;
            int  nY,nLine;
            int  nMaxLen = 0;
            
            GetWindowRect(hContentEdt,&rcEdt);
            GetWindowRectEx(hWnd,&rc,W_CLIENT,XY_SCREEN);

            hdc = GetDC(hWnd);
            GetTextExtent(hdc, "L", 1, &Size);
            ReleaseDC(hWnd,hdc);
            
            nLine = 50;
            
            nY = nLine * (Size.cy + SMS_EDIT_SPACE + 1);
            pCreateData->nHeight += nY;

            MoveWindow(hContentEdt,rcEdt.left,rcEdt.top-rc.top,rcEdt.right-rcEdt.left,
                rcEdt.bottom-rcEdt.top+nY,TRUE);
            
            nMaxLen = SendMessage(hContentEdt,EM_GETLIMITTEXT,NULL,NULL);
            
            if(nMaxLen >= MAX_TXT_SIZE)
                SendMessage(hContentEdt,EM_LIMITTEXT,MAX_TXT_SIZE,NULL);
        }
		else if(codeNotify == EN_KILLFOCUS)
		{	
            if(GetFocus() != GetDlgItem(hWnd,IDC_SMSEDIT_RECIPIENT))
                SetCaretProc(NULL);
		}
        break;

    default:
        break;
	}
    
	return;
}
/*********************************************************************\
* Function	SMSEdit_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSEdit_OnDestroy(HWND hWnd)
{
    PSMS_EDITCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);

    SMS_FREE(pCreateData->pszContent);
    SMS_FREE(pCreateData->pszRecipient);
    
    if(pCreateData->hBitmapIcon)
        DeleteObject(pCreateData->hBitmapIcon);
    if(pCreateData->MemoryDCIcon)
        DeleteDC(pCreateData->MemoryDCIcon);

    hEditWnd = NULL;

    SetCaretProc(NULL);
	
    return;

}
/*********************************************************************\
* Function	SMSEdit_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSEdit_OnClose(HWND hWnd)
{

    PSMS_EDITCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);

	SendMessage(pCreateData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    if(pCreateData->hWriteSMSInstance != NULL)
        DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)(pCreateData->hWriteSMSInstance));

    DestroyWindow (hWnd);

    return;

}

/*********************************************************************\
* Function	SMSEdit_OnMoveWindow
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void SMSEdit_OnMoveWindow(HWND hWnd,WPARAM wParam)
{
    HWND hContentEdt = NULL;
    RECT rc,rcEdt;

    hContentEdt = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);
    GetClientRect(hWnd,&rc);
    GetWindowRect(hContentEdt,&rcEdt);
    
    MoveWindow(hContentEdt,0,wParam-1,rcEdt.right-rcEdt.left,rcEdt.bottom-rcEdt.top,TRUE);
}
/*********************************************************************\
* Function	SMSEdit_OnMoveWindow
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void SMSEdit_OnAddRecipient(HWND hWnd,BOOL bAdd,int nCount,ABNAMEOREMAIL* pArray)
{
    int i;
    HWND hRecipentEdt = NULL;

	if(bAdd == FALSE)
    {
		SMS_FREE(pArray);
        
        return;
    }

    hRecipentEdt = GetDlgItem(hWnd,IDC_SMSEDIT_RECIPIENT);

    SendMessage(hRecipentEdt,REC_CLEAR,0,0);

    for(i = 0 ; i < nCount ; i++)
    {
        SendMessage(hRecipentEdt,GHP_ADDREC,0,(LPARAM)&pArray[i]);
    }

    SMS_FREE(pArray);
}
/*********************************************************************\
* Function	SMSEdit_OnSave
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void SMSEdit_OnSave(HWND hWnd,BOOL bSave)
{
    PSMS_EDITCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);

	if(bSave == FALSE)
    {
        PostMessage(hWnd,WM_CLOSE,0,0);
        if(pCreateData->hWriteSMSInstance && hTelMsgWnd != NULL)
            PostMessage(hTelMsgWnd,uTelMsgCmd,FALSE,0);
    }
	else
	{
		if(SMSEdit_OnSaveToDraft(hWnd))
        {
            PostMessage(hWnd,WM_CLOSE,0,0);
            
            if(pCreateData->hWriteSMSInstance && hTelMsgWnd != NULL)
                PostMessage(hTelMsgWnd,uTelMsgCmd,FALSE,0);
        }
	}
}
/*********************************************************************\
* Function	SMSEdit_OnSaveToDraft
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL SMSEdit_OnSaveToDraft(HWND hWnd)
{   
    PSMS_EDITCREATEDATA pCreateData;
    HWND hRecipientEdt = NULL;
    HWND hContentEdt = NULL;
    int nLen;
    SMS_STORE smsstore;
    SYSTEMTIME sy;
    
    pCreateData = GetUserData(hWnd);
    hRecipientEdt = GetDlgItem(hWnd,IDC_SMSEDIT_RECIPIENT);
    hContentEdt = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);
    
    if( (pCreateData->bSaved == TRUE && pCreateData->bChange == TRUE) 
        || (pCreateData->bSaved == FALSE && pCreateData->bChange == TRUE))
    {
        char    szFileName[SMS_FILENAME_MAXLEN];
//        if(IsFlashEnough() == FALSE)
//        {
//            PLXTipsWin(NULL,NULL,0,IDS_DRAFTFULL,IDS_SMS,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
//            return FALSE;
//         }

        nLen = SMS_RecipientGetTextLength(hRecipientEdt);

        if(nLen >= pCreateData->nReciLen)
        {
            SMS_FREE(pCreateData->pszRecipient);
            pCreateData->pszRecipient = (char*)malloc(nLen+1);
            if(!pCreateData->pszRecipient)
                return FALSE;
        }
        pCreateData->nReciLen = nLen+1;
        
        SMS_RecipientGetText(hRecipientEdt,pCreateData->pszRecipient,nLen+1);
        
        if(pCreateData->nMsgType == MU_MSG_SMS)
        {
            nLen = GetWindowTextLength(hContentEdt);
            if(nLen >= pCreateData->nConLen)
            {
                SMS_FREE(pCreateData->pszContent);
                pCreateData->pszContent = (char*)malloc(nLen+1);
                if(!pCreateData->pszContent)
                    return FALSE;
            }
            //pCreateData->nConLen = nLen+1;
            GetWindowText(hContentEdt,pCreateData->pszContent,nLen+1);
        }
        else
            nLen = pCreateData->nConLen;
        
        if(pCreateData->nConLen > 1 || pCreateData->nReciLen > 1)
        {
//            if(IsFlashEnough())
            {
                GetLocalTime(&sy);
                memset(&smsstore,0,sizeof(SMS_STORE));
                if(pCreateData->nMsgType == MU_MSG_SMS)
                    smsstore.fix.Conlen = nLen+1;
                else
                    smsstore.fix.Conlen = pCreateData->nConLen;
                smsstore.fix.Phonelen = pCreateData->nReciLen;
                smsstore.fix.dwDateTime = Datetime2INT(sy.wYear,sy.wMonth,sy.wDay,sy.wHour,sy.wMinute,sy.wSecond);
                smsstore.fix.Stat = MU_STU_DRAFT;
                smsstore.pszContent = pCreateData->pszContent;
                smsstore.pszPhone = pCreateData->pszRecipient;
                
                //SMS_CodingContent(&smsstore);
                
                memset(szFileName,0,SMS_FILENAME_MAXLEN);
                
                if(SMS_SaveFile(MU_DRAFT,&smsstore,szFileName,pCreateData->nMsgType))
                {
                    if(pCreateData->bSaved == FALSE && pCreateData->bChange == TRUE)//bNew
                    {
                        SMS_ChangeCount(MU_DRAFT,SMS_COUNT_ALL,1);
                        smsapi_New(szFileName,&smsstore,&(pCreateData->handle),&(pCreateData->folderid));//if in draftbox notify new msg
                        pCreateData->folderid = MU_DRAFT;
                    }
                    else
                    {
                        char    szOldDirPath[PATH_MAXLEN];
                        
                        smsapi_Modify(szFileName,pCreateData->handle,pCreateData->folderid,&smsstore);//delete file rename notify
                        
                        szOldDirPath[0] = 0;                    
                        getcwd(szOldDirPath,PATH_MAXLEN);  
                        chdir(PATH_DIR_SMS);                      
                        //delete older file
                        remove(pCreateData->szFileName);
                        chdir(szOldDirPath);
                    }
                    
                    pCreateData->bSaved = TRUE;
                    pCreateData->bChange = FALSE;
                    
                    nLen = strlen(szFileName);
                    nLen = nLen > SMS_FILENAME_MAXLEN ? SMS_FILENAME_MAXLEN : nLen;
                    strncpy(pCreateData->szFileName,szFileName,nLen);
                    pCreateData->szFileName[SMS_FILENAME_MAXLEN-1] = 0;
                    
                    PLXTipsWin(NULL,NULL,0,IDS_SAVEDTODRAFT,IDS_SMS,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
                }
            }
//            else
//                return FALSE;
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
static void SMSEdit_OnSureFragSend(HWND hWnd,BOOL bSure)
{
	if(bSure == FALSE)
		return;

	SMSEdit_OnSend(hWnd);
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSEdit_OnSend(HWND hWnd)
{        
	char szFileName[SMS_FILENAME_MAXLEN];
	char *p,*p1,*pSrc;
	DWORD dwhandle;
    PSMS_EDITCREATEDATA pCreateData;
    HWND hRecipientEdt = NULL;
    HWND hContentEdt = NULL;
    int nLen;
    SMS_STORE smsstore;
    SYSTEMTIME sy;
	SMS_SETTING Setting;
    PSMS_INITDATA pInitData;

    pInitData = SMS_GetInitData();

    pCreateData = GetUserData(hWnd);
    hRecipientEdt = GetDlgItem(hWnd,IDC_SMSEDIT_RECIPIENT);
    hContentEdt = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);
	
	szFileName[0] = 0;
	
	nLen = SMS_RecipientGetTextLength(hRecipientEdt);
	
	if(nLen >= pCreateData->nReciLen)
	{
		SMS_FREE(pCreateData->pszRecipient);
		pCreateData->pszRecipient = (char*)malloc(nLen+1);
		if(!pCreateData->pszRecipient)
			return;
		pCreateData->pszRecipient[0] = 0;
	}
	pCreateData->nReciLen = nLen+1;
	
	SMS_RecipientGetText(hRecipientEdt,pCreateData->pszRecipient,nLen+1);
	
	if(SMS_CheckPhone(pCreateData->pszRecipient) == FALSE)
		return;
	
    if(pCreateData->nMsgType == MU_MSG_SMS)
    {
        nLen = GetWindowTextLength(hContentEdt);
        if(nLen >= pCreateData->nConLen)
        {
            SMS_FREE(pCreateData->pszContent);
            pCreateData->pszContent = (char*)malloc(nLen+1);
            if(!pCreateData->pszContent)
                return;
            pCreateData->pszContent[0] = 0;
        }
        //pCreateData->nConLen = nLen+1;
        GetWindowText(hContentEdt,pCreateData->pszContent,nLen+1);
    }
    else
        nLen = pCreateData->nConLen;
    
	memset(&Setting,0,sizeof(SMS_SETTING));

	if(SMS_GetActivateSetting(&Setting) == FALSE)
	{			
		SMS_CreateSetting(pCreateData->hFrameWnd,hWnd,WM_SENDWITHOPTIONS,FALSE);

        return;
	}
	GetLocalTime(&sy);

	memset(&smsstore,0,sizeof(SMS_STORE));
	smsstore.fix.Conlen = nLen+1;
	smsstore.fix.Phonelen = pCreateData->nReciLen;
	smsstore.fix.dwDateTime = Datetime2INT(sy.wYear,sy.wMonth,sy.wDay,sy.wHour,sy.wMinute,sy.wSecond);
	smsstore.fix.Stat = MU_STU_WAITINGSEND;
	smsstore.pszContent = pCreateData->pszContent;
	smsstore.pszPhone = pCreateData->pszRecipient;
	smsstore.fix.PID = Setting.nPID;
	smsstore.fix.ReplyPath = Setting.bReplyPath;
	smsstore.fix.Report = Setting.bReport;
	smsstore.fix.VPvalue = Setting.nValidity;
	strcpy(smsstore.fix.SCA,Setting.szSCA);

    if(GetSIMState() == 0)
    {
        smsstore.fix.Stat = MU_STU_DEFERMENT;
        
        PLXTipsWin(NULL,NULL,0,IDS_SENDFAILEDNOSIM,IDS_SMS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
    }
    else
    {    
        if(Setting.nConnection == 0)//GSM
        {
            if(pInitData->nService != SMS_GSMpre)
            {
                SMS_SetService(SMS_GSMpre);
            }
        }
        else if(Setting.nConnection == 1)//GPRS
        {
            if(pInitData->nService != SMS_GPRSpre)
            {
                SMS_SetService(SMS_GPRSpre);
            }
        }
        
        WaitWinWithTimer(NULL, TRUE, IDS_SENDING, IDS_SMS,NULL,NULL,0,WAITTIMEOUT);
//        PLXTipsWin(NULL,NULL,0,IDS_SENDING,IDS_SMS,Notify_Info,NULL,NULL,WAITTIMEOUT);
    }
    
	pSrc = p1 = p = smsstore.pszPhone;
	
	while(*p !='\0')
	{
		if(*p == ';')
		{
			*p = '\0';
			
// 			if(IsFlashEnough())
			{                                      
				smsstore.fix.Phonelen = strlen(p1)+1;
				smsstore.pszPhone = p1;
				
				if(SMS_SaveFile(MU_OUTBOX,&smsstore,szFileName,pCreateData->nMsgType))
				{
					SMS_ChangeCount(MU_OUTBOX,SMS_COUNT_ALL,1);
					
                    SMS_NotifyIdle();

					if(MU_GetCurFolderType() == MU_OUTBOX)//bNew
						dwhandle = smsapi_New(szFileName,&smsstore,NULL,NULL);//if in draftbox notify new msg
					else
						dwhandle = pCreateData->handle;
                    
                    if(GetSIMState() != 0)
                    {
                        SMS_Send(szFileName,dwhandle);
                    }
				}
			}
			
			if(strlen(p1) > ME_PHONENUM_LEN-1) //now
				continue;
			
			p1 = p+1;
			if( *p1 == '\0')
				break;
		}
		p++;
	}
	
	if(*p1 != '\0')
	{                   
// 		if(IsFlashEnough())
		{
			szFileName[0] = 0;
			smsstore.fix.Phonelen = strlen(p1)+1;
			smsstore.pszPhone = p1;
			if(SMS_SaveFile(MU_OUTBOX,&smsstore,szFileName,pCreateData->nMsgType))
			{
				SMS_ChangeCount(MU_OUTBOX,SMS_COUNT_ALL,1);
				
                SMS_NotifyIdle();

				if(MU_GetCurFolderType() == MU_OUTBOX)//bNew
					dwhandle = smsapi_New(szFileName,&smsstore,NULL,NULL);//if in draftbox notify new msg
				else
					dwhandle = pCreateData->handle;
                
                if(GetSIMState() != 0)
                {
                    SMS_Send(szFileName,dwhandle);
                }
			}
		}
	}
	
	smsstore.pszPhone = pSrc;
	
	if( pCreateData->handle != -1 || (pCreateData->handle == -1 && pCreateData->bSaved) )
	{
		char szOldPath[PATH_MAXLEN];
		
		szOldPath[0] = 0;
		
		SMS_ChangeCount(MU_DRAFT,SMS_COUNT_ALL,-1);
		
		getcwd(szOldPath,PATH_MAXLEN);
		
		chdir(PATH_DIR_SMS);  
		
		remove(pCreateData->szFileName);
		
		chdir(szOldPath); 
		
		if(MU_GetCurFolderType() == MU_DRAFT)
			smsapi_Delete(pCreateData->handle);//if in draftbox notify new msg
	}

	PostMessage(hWnd,WM_CLOSE,0,0);
    
    if(pCreateData->hWriteSMSInstance && hTelMsgWnd != NULL)
        PostMessage(hTelMsgWnd,uTelMsgCmd,TRUE,0);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSEdit_OnSaveSetting(HWND hWnd,BOOL bSave,SMS_SETTINGCHAIN* pSettingChain)
{
	if(bSave == TRUE)
	{
		SMS_WriteSetting(&(pSettingChain->Setting),&(pSettingChain->dwoffset));  
		
		SMS_SaveActivateID(pSettingChain->Setting.nID);
		
		SMSEdit_OnSend(hWnd);
	}
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSEdit_OnSendWithOptions(HWND hWnd,BOOL bSend,SMS_SETTING * pSetting)
{
	char szFileName[SMS_FILENAME_MAXLEN];
	char *p,*p1,*pSrc;
	DWORD dwhandle;
    PSMS_EDITCREATEDATA pCreateData;
    HWND hRecipientEdt = NULL;
    HWND hContentEdt = NULL;
    int nLen;
    SMS_STORE smsstore;
    SYSTEMTIME sy;
    PSMS_INITDATA pInitData;

    pInitData = SMS_GetInitData();

    if(bSend == FALSE)
        return;
        
    pCreateData = GetUserData(hWnd);
    hRecipientEdt = GetDlgItem(hWnd,IDC_SMSEDIT_RECIPIENT);
    hContentEdt = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);
	
	szFileName[0] = 0;
	
	nLen = SMS_RecipientGetTextLength(hRecipientEdt);
	
	if(nLen >= pCreateData->nReciLen)
	{
		SMS_FREE(pCreateData->pszRecipient);
		pCreateData->pszRecipient = (char*)malloc(nLen+1);
		if(!pCreateData->pszRecipient)
			return;
		pCreateData->pszRecipient[0] = 0;
	}
	pCreateData->nReciLen = nLen+1;
	
	SMS_RecipientGetText(hRecipientEdt,pCreateData->pszRecipient,nLen+1);
	
	if(SMS_CheckPhone(pCreateData->pszRecipient) == FALSE)
		return;
	
    if(pCreateData->nMsgType == MU_MSG_SMS)
    {
        nLen = GetWindowTextLength(hContentEdt);
        if(nLen >= pCreateData->nConLen)
        {
            SMS_FREE(pCreateData->pszContent);
            pCreateData->pszContent = (char*)malloc(nLen+1);
            if(!pCreateData->pszContent)
                return;
            pCreateData->pszContent[0] = 0;
        }
        //pCreateData->nConLen = nLen+1;
        GetWindowText(hContentEdt,pCreateData->pszContent,nLen+1);
    }
    else
        nLen = pCreateData->nConLen;

	GetLocalTime(&sy);

	memset(&smsstore,0,sizeof(SMS_STORE));
	smsstore.fix.Conlen = nLen+1;
	smsstore.fix.Phonelen = pCreateData->nReciLen;
	smsstore.fix.dwDateTime = Datetime2INT(sy.wYear,sy.wMonth,sy.wDay,sy.wHour,sy.wMinute,sy.wSecond);
	smsstore.fix.Stat = MU_STU_WAITINGSEND;
	smsstore.pszContent = pCreateData->pszContent;
	smsstore.pszPhone = pCreateData->pszRecipient;
	smsstore.fix.PID = pSetting->nPID;
	smsstore.fix.ReplyPath = pSetting->bReplyPath;
	smsstore.fix.Report = pSetting->bReport;
	smsstore.fix.VPvalue = pSetting->nValidity;
	strcpy(smsstore.fix.SCA,pSetting->szSCA);
	
    if(GetSIMState() == 0)
    {
        smsstore.fix.Stat = MU_STU_DEFERMENT;
        
        PLXTipsWin(NULL,NULL,0,IDS_SENDFAILEDNOSIM,IDS_SMS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
    }
    else
    {   
        if(pSetting->nConnection == 0)//GSM
        {
            if(pInitData->nService != SMS_GSMpre)
            {
                SMS_SetService(SMS_GSMpre);
            }
        }
        else if(pSetting->nConnection == 1)//GPRS
        {
            if(pInitData->nService != SMS_GPRSpre)
            {
                SMS_SetService(SMS_GPRSpre);
            }
        }
        WaitWinWithTimer(NULL, TRUE, IDS_SENDING, IDS_SMS,NULL,NULL,0,WAITTIMEOUT);      
//        PLXTipsWin(NULL,NULL,0,IDS_SENDING,IDS_SMS,Notify_Info,NULL,NULL,WAITTIMEOUT);
    }

	pSrc = p1 = p = smsstore.pszPhone;
	
	while(*p !='\0')
	{
		if(*p == ';')
		{
			*p = '\0';
			
//			if(IsFlashEnough())
			{                                      
				smsstore.fix.Phonelen = strlen(p1)+1;
				smsstore.pszPhone = p1;
				
				if(SMS_SaveFile(MU_OUTBOX,&smsstore,szFileName,pCreateData->nMsgType))
				{
					SMS_ChangeCount(MU_OUTBOX,SMS_COUNT_ALL,1);
					
                    SMS_NotifyIdle();

					if(MU_GetCurFolderType() == MU_OUTBOX)//bNew
						dwhandle = smsapi_New(szFileName,&smsstore,NULL,NULL);//if in draftbox notify new msg
					else
						dwhandle = pCreateData->handle;
				
                    if(GetSIMState() != 0)
                    {
                        SMS_Send(szFileName,dwhandle);
                    }
				}
			}
			
			if(strlen(p1) > ME_PHONENUM_LEN-1) //now
				continue;
			
			p1 = p+1;
			if( *p1 == '\0')
				break;
		}
		p++;
	}
	
	if(*p1 != '\0')
	{                   
//		if(IsFlashEnough())
		{
			szFileName[0] = 0;
			smsstore.fix.Phonelen = strlen(p1)+1;
			smsstore.pszPhone = p1;
			if(SMS_SaveFile(MU_OUTBOX,&smsstore,szFileName,pCreateData->nMsgType))
			{
				SMS_ChangeCount(MU_OUTBOX,SMS_COUNT_ALL,1);
				
                SMS_NotifyIdle();

				if(MU_GetCurFolderType() == MU_OUTBOX)//bNew
					dwhandle = smsapi_New(szFileName,&smsstore,NULL,NULL);//if in draftbox notify new msg
				else
					dwhandle = pCreateData->handle;
				
                if(GetSIMState() != 0)
                {
                    SMS_Send(szFileName,dwhandle);
                }
			}
		}
	}
	
	smsstore.pszPhone = pSrc;
	
    
	if( pCreateData->handle != -1 || (pCreateData->handle == -1 && pCreateData->bSaved) )
	{
		char szOldPath[PATH_MAXLEN];
		
		szOldPath[0] = 0;
		
		SMS_ChangeCount(MU_DRAFT,SMS_COUNT_ALL,-1);
		
		getcwd(szOldPath,PATH_MAXLEN);
		
		chdir(PATH_DIR_SMS);  
		
		remove(pCreateData->szFileName);
		
		chdir(szOldPath); 
		
		if(MU_GetCurFolderType() == MU_DRAFT)
			smsapi_Delete(pCreateData->handle);//if in draftbox notify new msg
		
	}

	PostMessage(hWnd,WM_CLOSE,0,0);
    
    if(pCreateData->hWriteSMSInstance && hTelMsgWnd != NULL)
        PostMessage(hTelMsgWnd,uTelMsgCmd,TRUE,0);
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_SaveFile(int folderid,SMS_STORE *psmsstore,char* szFileName,MU_MSG_TYPE nMsgType)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    char *pszCon = NULL;
    int nConLen = 0;
    int nOrgLen = 0;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    szFileName[0] = 0;
    
    if(!SMS_NewFileName(folderid,szFileName))
    {
        chdir(szOldPath);
        return FALSE;
    }

    SMS_CodingContent(psmsstore,&pszCon,&nConLen,nMsgType);

    f = open(szFileName,O_RDWR|O_CREAT,S_IRWXU);

    if( f == -1 )
    {
        chdir(szOldPath);

        return FALSE;
    }
    
#ifdef _SMS_DEBUG_
    printf("\r\n*****SMS Debug Info*****  SMS_SaveFile  file handle = %d \r\n",f);
#endif

    nOrgLen = psmsstore->fix.Conlen;
    psmsstore->fix.Conlen = nConLen;

    write(f,&(psmsstore->fix),sizeof(SMS_STOREFIX));
    write(f,psmsstore->pszPhone,psmsstore->fix.Phonelen);

    psmsstore->fix.Conlen = nOrgLen;
    if(pszCon)
    {
        write(f,pszCon,nConLen);
        SMS_FREE(pszCon);
    }
    else
        write(f,psmsstore->pszContent,psmsstore->fix.Conlen);
    write(f,psmsstore->pszUDH,psmsstore->fix.Udhlen);

    close(f);

    chdir(szOldPath);

    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_CodingContent(SMS_STORE *psmsstore,char** ppszCon,int* pnlen,MU_MSG_TYPE nMsgType)
{
    BOOL bGSM;

    if(nMsgType == MU_MSG_SMS)
    {
        if (psmsstore->fix.Conlen > 1)
        {
            bGSM = IsGSMString(psmsstore->pszContent, psmsstore->fix.Conlen-1);

            if (bGSM)
            {	
                psmsstore->fix.dcs = DCS_GSM;
                
                *pnlen = MultiByteToGSM(psmsstore->pszContent, psmsstore->fix.Conlen-1, NULL, 0, NULL, NULL);

                if(*pnlen > 0)
                {
                    *ppszCon = (char*)malloc(*pnlen);
                    
                    if(*ppszCon == NULL)
                        return FALSE;
                    
                    MultiByteToGSM(psmsstore->pszContent, psmsstore->fix.Conlen-1, (LPWSTR)*ppszCon, *pnlen, NULL, NULL);
                }
            }
            else
            {		
                psmsstore->fix.dcs = DCS_UCS2;
                
                *pnlen = MultiByteToWideChar(CP_ACP, 0, psmsstore->pszContent, psmsstore->fix.Conlen-1, NULL, 0);
                
                *pnlen *= 2;

                if(*pnlen > 0)
                {
                    *ppszCon = (char*)malloc(*pnlen);
                    
                    if(*ppszCon == NULL)
                        return FALSE;
                        
                    MultiByteToWideChar(CP_ACP, 0, psmsstore->pszContent, psmsstore->fix.Conlen-1, 
                        (LPWSTR)*ppszCon, *pnlen);
                }
            }
        }
        else
        {
            *ppszCon = NULL;
            *pnlen = 0;
            psmsstore->fix.dcs = DCS_GSM;
        }
    }
    else
    {   
        *ppszCon = NULL;
        *pnlen = psmsstore->fix.Conlen;
        psmsstore->fix.dcs = DCS_8BIT;
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
BOOL SMS_NewFileName(int folderid, char* pszFileName)
{
    /*
     *  ASCII
     0~9 : 48~57
     A~Z : 65~90
     a~z : 97~122
     */
    char mark;
    SYSTEMTIME t;
    int i,j;
    char ichar;
    int f;
    BOOL bFirst = TRUE;

    switch(folderid) 
    {
    case MU_DRAFT:
        mark = SMS_FILE_DRAFT_MARK;
    	break;

    case MU_OUTBOX:
        mark = SMS_FILE_OUTBOX_MARK;
    	break;

    case MU_INBOX:
        mark = SMS_FILE_INBOX_MARK;
        break;

    default:
        return FALSE;
    }    
    
    GetLocalTime(&t);
    srand((unsigned)t.wMilliseconds);

    do 
    {
        if(!bFirst)
            close(f);
        pszFileName[0] = mark;
        for (i = 0; i < 9; i++)
        {
            j = rand()%3;
            switch(j) 
            {
            case 0:
                ichar = 48 + rand()%(57 - 48);
                break;
            case 1:
                ichar = 65 + rand()%(90 - 65);
                break;
            case 2:
                ichar = 97 + rand()%(122 - 97);
                break;
            default:
                break;
            }
            pszFileName[i+1] = ichar;
        }
        pszFileName[i+1] = '\0';
        strcat(pszFileName, SMS_FILE_POSTFIX);
        bFirst = FALSE;
    }while((f = open(pszFileName,O_RDONLY)) != -1);

    return TRUE;
}

static BOOL SMS_EvaluateFragNum(char* pszContent,int nLen, int *pnFrag,int nMsgType)
{
    BOOL bGSM;
    int  srclen,datalen,ntmplen;
    char tmp = 0;
    int  dcs;

    if(nMsgType == MU_MSG_SMS)
    {
        srclen = strlen(pszContent);
        
        bGSM = IsGSMString(pszContent,srclen);
        if (bGSM)
        {	
            dcs = DCS_GSM;
            datalen = MultiByteToGSM(pszContent, srclen,(LPWSTR)tmp, 0, NULL, NULL);
        }
        else
        {		
            dcs = DCS_UCS2;
            datalen = MultiByteToWideChar(CP_ACP, 0, pszContent, -1, NULL, 0);
            datalen *= 2;
        }
    }
    else
    {
        dcs = DCS_8BIT;
        datalen = nLen;
    }
    
    if (dcs == DCS_GSM) 
        ntmplen = 160;
    else
        ntmplen = 140;
    
    if(datalen > ntmplen)
    {
        ntmplen -= 6; /*6 = UDHI+SARID+LEN*/
        if(datalen % ntmplen == 0)
            *pnFrag = datalen/ntmplen;
        else
            *pnFrag = datalen/ntmplen + 1;
    }
    else
        /* 不需要分片 */
        *pnFrag = 1;

    return TRUE;
}

            
static BOOL SMS_CheckPhone(char * pPhoneString)
{
    int nLen;
    char* p;

    return TRUE;

    nLen = strlen(pPhoneString);
    p = pPhoneString;
    while(*p != 0)
    {
        if((*p >= '0' && *p <= '9') || *p == 'w' || *p == 'W' || *p == 'p' || *p == 'P' 
            || *p == '+' || *p == ';' || *p == '*' || *p == '#' || *p == '?' || *p == 'S')
            p++;
        else
            return FALSE;
    }
    return TRUE;
}
/*********************************************************************\
* Function	   SMSEdit_InitVScroll
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSEdit_InitVScroll(HWND hWnd)
{
    SCROLLINFO      vsi;
    PSMS_EDITCREATEDATA pData;
    HWND            hEdt;
    int             nLine;
    
    pData = GetUserData(hWnd);
    
    hEdt = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);
    nLine = SendMessage(hEdt,EM_GETLINECOUNT,0,0);
    memset(&vsi, 0, sizeof(SCROLLINFO));
   
    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 5;//5
    vsi.nMax   = nLine+2-1;
    vsi.nPos   = 0;
    
    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
    
	return;
}
/*********************************************************************\
* Function	   SMS_CaretProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_CaretProc(const RECT* rc)
{
#define  CLASS_NAMELEN          100
   HWND hFocus,hParent,hEdt;
   char pszClassName[CLASS_NAMELEN];
   PSMS_EDITCREATEDATA pCreateData;
   RECT rcClient;
   BOOL bSpecial;
   int  nLine;
   SCROLLINFO vsi;
   int  nloop;

   bSpecial = FALSE;
   pszClassName[0] = 0;
   
   hFocus = GetFocus();
   hParent = hFocus;//GetParent(hFocus);
   GetClassName(hParent,pszClassName,CLASS_NAMELEN);

   if(stricmp(pszClassName,WC_RECIPIENT)==0)
       bSpecial = TRUE;

   nloop = 0;
   while(stricmp(pszClassName,"SMSEditWndClass")!=0)
   {
       nloop++;
       hFocus = hParent;
       hParent = GetParent(hFocus);
       GetClassName(hParent,pszClassName,CLASS_NAMELEN);

       if(nloop > 3)
           return;
   }

   hEdt = GetDlgItem(hParent,IDC_SMSEDIT_CONTENT);
   pCreateData = GetUserData(hParent);

   GetWindowRectEx(hParent,&rcClient,W_CLIENT,XY_SCREEN);

   if(rcClient.bottom < rc->bottom) //down
   {
       SIZE Size;
       HDC  hdc;
       int  nY;

       hdc = GetDC(hParent);
       GetTextExtent(hdc, "L", 1, &Size);
       ReleaseDC(hParent,hdc);
       
       nY = Size.cy + SMS_EDIT_SPACE;
       
       memset(&vsi, 0, sizeof(SCROLLINFO));
       vsi.fMask  = SIF_POS;
       GetScrollInfo(hParent, SB_VERT, &vsi);
       
       vsi.nPos++;
       vsi.fMask  = SIF_POS ;
       SetScrollInfo(hParent, SB_VERT, &vsi, TRUE);

       ScrollWindow(hParent,0,-nY,NULL,NULL);
       UpdateWindow(hParent);
   }
   else if(rcClient.top >= rc->top) //up
   {
       if(bSpecial == TRUE)
       {      
           RECT rect,rcSys;
           int  nY;

           GetWindowRect(hFocus,&rect);
           GetWindowRectEx(hParent,&rcSys,W_CLIENT,XY_SCREEN);
           nY = rcSys.top - rect.top;
           
           memset(&vsi, 0, sizeof(SCROLLINFO));
           vsi.fMask  = SIF_POS ;
           GetScrollInfo(hParent, SB_VERT, &vsi);   
           vsi.nPos--;
           vsi.fMask  = SIF_POS ;
           SetScrollInfo(hParent, SB_VERT, &vsi, TRUE);        
           ScrollWindow(hParent,0,nY,NULL,NULL);
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
           
           nY = Size.cy + SMS_EDIT_SPACE;
           
           memset(&vsi, 0, sizeof(SCROLLINFO));
           vsi.fMask  = SIF_POS ;
           GetScrollInfo(hParent, SB_VERT, &vsi); 
           vsi.nPos--;
           vsi.fMask  = SIF_POS ;
           SetScrollInfo(hParent, SB_VERT, &vsi, TRUE);          
           ScrollWindow(hParent,0,nY,NULL,NULL);
           UpdateWindow(hParent);
       }
   }
   
   if(bSpecial == TRUE)
   {      
       memset(&vsi, 0, sizeof(SCROLLINFO));
       vsi.fMask  = SIF_POS ;
       GetScrollInfo(hParent, SB_VERT, &vsi);
       if(vsi.nPos != 0)
       {
           RECT rect,rcSys;
           int  nY;
           
           GetWindowRect(hFocus,&rect);
           GetWindowRectEx(hParent,&rcSys,W_CLIENT,XY_SCREEN);
           nY = rcSys.top - rect.top;           
           vsi.nPos = 0;
           vsi.fMask  = SIF_POS ;
           SetScrollInfo(hParent, SB_VERT, &vsi, TRUE);        
           ScrollWindow(hParent,0,nY,NULL,NULL);
           UpdateWindow(hParent);
       }
   }

   memset(&vsi, 0, sizeof(SCROLLINFO));
   vsi.fMask  = SIF_RANGE ;
   GetScrollInfo(hParent, SB_VERT, &vsi);
   
   nLine = SendMessage(hEdt,EM_GETLINECOUNT,0,0);
   if(nLine+2-1 != vsi.nMax)
   {
       vsi.nMax = nLine+2-1;
       vsi.fMask  = SIF_RANGE;
       SetScrollInfo(hParent, SB_VERT, &vsi, TRUE); 
   }
   
}
/*********************************************************************\
* Function	   SMS_RecipientSetText
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int SMS_RecipientSetText(HWND hEdt,char* pszText)
{
    char* p1,*p,*p2;

    p = (char*)malloc(strlen(pszText)+1);
    
    if(p == NULL)
        return -1;

    strcpy(p,pszText);

    p2 = p1 = p;
    
    while(*p !='\0')
    {
        if(*p == ';')
        {
            *p = '\0';
                        
            if(strlen(p1) > ME_PHONENUM_LEN-1) 
                continue;

            SendMessage(hEdt,GHP_ADDREC,1,(LPARAM)p1);
            
            p1 = p+1;
            if( *p1 == '\0')
                break;
        }
        p++;
    }
    
    if(*p1 != '\0')
        SendMessage(hEdt,GHP_ADDREC,1,(LPARAM)p1);

    SMS_FREE(p2);

    return 1;
}
/*********************************************************************\
* Function	   SMS_RecipientGetText
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMS_RecipientGetText(HWND hEdt,PSTR pszText, int nMaxCount)
{
    RECIPIENTLISTBUF Recipient;

    PRECIPIENTLISTNODE pTemp = NULL;

    int i;

    memset(&Recipient,0,sizeof(RECIPIENTLISTBUF));

    SendMessage(hEdt,GHP_GETREC,0,(LPARAM)&Recipient);

    pTemp = Recipient.pDataHead;

    pszText[0] = 0;

    for(i = 0; i < Recipient.nDataNum ; i++)
    {
        strcat(pszText,pTemp->szPhoneNum);

        strcat(pszText,";");

        pTemp = pTemp->pNext;
    }

    return ;
}
/*********************************************************************\
* Function	   SMS_RecipientGetTextLength
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int SMS_RecipientGetTextLength(HWND hEdt)
{
    
    RECIPIENTLISTBUF Recipient;

    PRECIPIENTLISTNODE pTemp = NULL;

    int nLen,i;

    memset(&Recipient,0,sizeof(RECIPIENTLISTBUF));

    SendMessage(hEdt,GHP_GETREC,0,(LPARAM)&Recipient);

    pTemp = Recipient.pDataHead;

    nLen = 0;

    for(i = 0; i < Recipient.nDataNum ; i++)
    {
        nLen += strlen(pTemp->szPhoneNum) + 1;

        pTemp = pTemp->pNext;
    }

    return nLen;
}

/*********************************************************************\
* Function	   SMS_SetIndicator
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMS_SetIndicator(HWND hWnd,int nFrag, int nRemain)
{
	HDC			hdc = NULL;
	HBITMAP		hBitmap = NULL;
	char        szFrag[16], szRemain[16];
	PSMS_EDITCREATEDATA  pCreateData;
    HFONT       hFont = NULL;
	RECT        rect;
	SIZE        sz1, sz2;
	int         nOldMode = 0;

	pCreateData = GetUserData(hWnd);
	
	hdc = GetDC(hWnd);

	if (pCreateData->MemoryDCIcon == NULL)
		pCreateData->MemoryDCIcon = CreateCompatibleDC(hdc);

	GetFontHandle(&hFont,SMALL_FONT);
    SelectObject(pCreateData->MemoryDCIcon,hFont);

    sprintf(szFrag, "(%d)", nFrag);
    sprintf(szRemain, "%d", nRemain);

	GetTextExtentPoint(pCreateData->MemoryDCIcon, szFrag, -1, &sz1);
	GetTextExtentPoint(pCreateData->MemoryDCIcon, szRemain, -1, &sz2);

	SetRect(&rect, 0, 0, max(sz1.cx, sz2.cx), sz1.cy + sz2.cy - 4);
	
	if (pCreateData->hBitmapIcon != NULL)
		DeleteObject(pCreateData->hBitmapIcon);

    pCreateData->hBitmapIcon = CreateCompatibleBitmap(pCreateData->MemoryDCIcon, 
		rect.right, rect.bottom);

    SelectObject(pCreateData->MemoryDCIcon, pCreateData->hBitmapIcon);

	ClearRect(pCreateData->MemoryDCIcon, &rect, COLOR_TRANSBK);

	nOldMode = SetBkMode(pCreateData->MemoryDCIcon, TRANSPARENT);
	TextOut(pCreateData->MemoryDCIcon, 0, 0, szFrag, -1);
	TextOut(pCreateData->MemoryDCIcon, 0, sz1.cy - 4, szRemain, -1);
	SetBkMode(pCreateData->MemoryDCIcon, nOldMode);

	ReleaseDC(hWnd, hdc);

}

/*********************************************************************\
* Function	   SMSEdit_OnInsertTemplate
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSEdit_OnInsertTemplate(HWND hWnd,int nLen,char* pszInsertString)
{
    HWND hwndContent = NULL;
    PSMS_EDITCREATEDATA pCreateData;
    int nSrcLen, allLen;
    
    if(nLen == 0)
        return;
    
    pCreateData = GetUserData(hWnd);
    
    hwndContent = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);
    
    nSrcLen = GetWindowTextLength(hwndContent);
    
    allLen = nSrcLen + nLen;

    if (allLen > MAX_TXT_SIZE)
    {
        PLXTipsWin(NULL, NULL, 0, IDS_OVERSIZE, IDS_SMS, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);

        return;
    }
    
    if(pCreateData->nConLen < allLen+1 )
    {
		SMS_FREE(pCreateData->pszContent);
        pCreateData->nConLen = allLen + 1 ;
		pCreateData->pszContent = (char*)malloc(pCreateData->nConLen);
		if(!pCreateData->pszContent)
			return;
		pCreateData->pszContent[0] = 0;
    }

    pCreateData->nInsertStringLen = nLen;

    pCreateData->pszInsertString = (char*)malloc(pCreateData->nInsertStringLen+1);

    if(pCreateData->pszInsertString == NULL)
    {
        pCreateData->nInsertStringLen = 0;
        return;
    }

    memset(pCreateData->pszInsertString,0,pCreateData->nInsertStringLen+1);

    strncpy(pCreateData->pszInsertString,pszInsertString,pCreateData->nInsertStringLen);

    PostMessage(hWnd,WM_SMSREPLACESEL,NULL,NULL);

    return;
}

/*********************************************************************\
* Function	   SMSEdit_OnReplaceSel
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSEdit_OnReplaceSel(HWND hWnd)
{
    HWND hwndContent = NULL;
    PSMS_EDITCREATEDATA pCreateData;
    int nSrcLen, allLen;
    int nContentLen,nFrag,nRemain;
    
    pCreateData = GetUserData(hWnd);
    
    hwndContent = GetDlgItem(hWnd,IDC_SMSEDIT_CONTENT);
    
    nSrcLen = GetWindowTextLength(hwndContent);
    
    allLen = nSrcLen + pCreateData->nInsertStringLen;
    
    if (allLen > MAX_TXT_SIZE)
    {
        PLXTipsWin(NULL, NULL, 0, IDS_OVERSIZE, IDS_SMS, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
        
        SMS_FREE(pCreateData->pszInsertString);
        
        pCreateData->nInsertStringLen = 0;

        return;
    }
    
    if(pCreateData->nConLen < allLen+1 )
    {
        SMS_FREE(pCreateData->pszContent);
        pCreateData->nConLen = allLen + 1 ;
        pCreateData->pszContent = (char*)malloc(pCreateData->nConLen);
        if(!pCreateData->pszContent)
        {
            SMS_FREE(pCreateData->pszInsertString);
            pCreateData->nInsertStringLen = 0;
            return;
        }
        pCreateData->pszContent[0] = 0;
    }
    
    if(pCreateData->pszInsertString == NULL)
    {
        pCreateData->nInsertStringLen = 0;
        return;
    }

    while(TRUE)
    {
        int nMaxLen = 0;

        nMaxLen = SendMessage(hwndContent,EM_GETLIMITTEXT,NULL,NULL);
        
        if(nMaxLen < allLen+1)
        {            
            RECT rcEdt,rc;
            SIZE Size;
            HDC  hdc;
            int  nY,nLine;
            
            GetWindowRect(hwndContent,&rcEdt);
            GetWindowRectEx(hWnd,&rc,W_CLIENT,XY_SCREEN);
            
            hdc = GetDC(hWnd);
            GetTextExtent(hdc, "L", 1, &Size);
            ReleaseDC(hWnd,hdc);
            
            nLine = 50;
            
            nY = nLine * (Size.cy + SMS_EDIT_SPACE + 1);
            pCreateData->nHeight += nY;
            
            MoveWindow(hwndContent,rcEdt.left,rcEdt.top-rc.top,rcEdt.right-rcEdt.left,
                rcEdt.bottom-rcEdt.top+nY,TRUE);
        }
        else
        {
            if(nMaxLen >= MAX_TXT_SIZE)
                SendMessage(hwndContent,EM_LIMITTEXT,MAX_TXT_SIZE,NULL);
            break;
        }

    }

    SendMessage(hwndContent, EM_REPLACESEL, NULL, (WPARAM)pCreateData->pszInsertString);

    SMS_FREE(pCreateData->pszInsertString);

    pCreateData->nInsertStringLen = 0;

    GetWindowText(hwndContent,pCreateData->pszContent,allLen);
        
    nContentLen = GetWindowTextLength(hwndContent);

    nFrag = nContentLen / 160;
    
    nRemain = nContentLen % 160;
    
    nRemain = 160 - nRemain;
    
    SMS_SetIndicator(hWnd, nFrag+1, nRemain);
    
    pCreateData->bChange = TRUE; 
}
/*********************************************************************\
* Function	   SMSEdit_OnSetLBtnText
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSEdit_OnSetLBtnText(HWND hWnd, int nID, BOOL bEmpty, LPSTR pszText)
{
    PSMS_EDITCREATEDATA pUserData = NULL;
    static BOOL bRecipientEmpty = FALSE, bContentEmpty = FALSE;

    pUserData = GetUserData(hWnd);

    switch (nID)
    {
    case IDC_SMSEDIT_RECIPIENT:
        bRecipientEmpty = bEmpty;
        SendMessage(pUserData->hFrameWnd , PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1),(LPARAM)IDS_ADD);
        break; 

    case IDC_SMSEDIT_CONTENT:
        bContentEmpty = bEmpty;
        if (bContentEmpty)
        {
            SendMessage(pUserData->hFrameWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1),(LPARAM)"");
        }
        else
        {
            SendMessage(pUserData->hFrameWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1),(LPARAM)(bRecipientEmpty ? IDS_SAVE : IDS_SEND));
        }
        break;

    default:
        SendMessage(pUserData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1),(LPARAM)pszText);
        break;
    }
}
/*********************************************************************\
* Function	   SMS_Register
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_Register(HWND hMsgWnd, UINT uMsgCmd)
{
    hTelMsgWnd = hMsgWnd;
    uTelMsgCmd = uMsgCmd;
}
/*********************************************************************\
* Function	   SMS_Unregister
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_Unregister(void)
{
    hTelMsgWnd = NULL;
    uTelMsgCmd = 0;
}
