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

typedef struct tagSMS_SIMChain
{
    int         nMemType;
    PSMS_INFO   psmsinfo; 
    struct tagSMS_SIMChain * pPioneer;
    struct tagSMS_SIMChain * pNext;
}SMS_SIMCHAIN,*PSMS_SIMCHAIN;

typedef struct tagSMS_SIMCreateData
{
	HWND		hFrameWnd;
	HMENU		hMenu;
    PSMS_INFO   psmsinfo;
    int         ncount;       
    int         nmaxcount;
    int         MEHandle; 
    int         nSelectCount;
    BOOL        bOK;
    SMS_SIMCHAIN*   pSelectNode;
    SMS_SIMCHAIN**  ppId;
    SMS_SIMCHAIN*   pUnreadHeader;
    SMS_SIMCHAIN*   pHeader;
    HBITMAP     hBitmapRead;
    HBITMAP     hBitmapUnread;
}SMS_SIMCREATEDATA,*PSMS_SIMCREATEDATA;

static HWND hSIMWnd = NULL;
static HWND hSIMWaitWnd = NULL;
static BOOL bResult;
static HWND hSIMCtlWnd = NULL;

typedef enum 
{
    ID_TIMER_GETMEM = 0,
    ID_TIMER_LISTSMS,
}ID_TIMER;

#define OVERTIME                1000

#define IDC_EXIT                    200

#define IDM_SMSSIM_COPYTO               101
#define IDM_SMSSIM_DELETE               102
#define IDM_SMSSIM_DELETEMANY           103
#define IDM_SMSSIM_DELETEMANY_SELECT    1031
#define IDM_SMSSIM_DELETEMANY_ALL       1032

#define IDC_SIMLSIT_LIST            300

#define SIMPREREAD_WIN_POSITION     0,120,176,100

LRESULT SMSSIMWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL SMSSIM_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void SMSSIM_OnActivate(HWND hwnd, UINT state);
static void SMSSIM_OnSetFocus(HWND hWnd);
static void SMSSIM_OnInitmenu(HWND hwnd);
static void SMSSIM_OnPaint(HWND hWnd);
static void SMSSIM_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void SMSSIM_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void SMSSIM_OnDestroy(HWND hWnd);
static void SMSSIM_OnClose(HWND hWnd);
static void SMSSIM_OnPreRead(HWND hWnd);
static void SMSSIM_OnDelAll(HWND hWnd,BOOL bSure);
static void SMSSIM_OnDel(HWND hWnd,BOOL bSure);
static void SMSSIM_OnTimer(HWND hWnd,UINT id);
static void SMSSIM_OnOK(HWND hWnd);
static void SMSSIM_OnCancel(HWND hWnd);
static void SMSSIM_OnDial(HWND hWnd,BOOL bSure);
static void SMSSIM_OnCopy(HWND hWnd,BOOL bSelect,int nFolderID);
static void SMSSIM_OnDelSel(HWND hWnd,BOOL bSure,int nCount,SMS_SIMCHAIN** ppId);
static void SMSSIM_OnSureToDelSel(HWND hWnd,BOOL bSure);

static void GetSIMCount(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void	GetSMSFromSIM(HWND hWnd,WPARAM wParam,LPARAM lParam);
static BOOL ReadSMSCountInSIM(HWND hWnd);
static BOOL	ReadSMSFromSIM(HWND hWnd);

static SMS_SIMCHAIN* SIM_New(void);
static int SIM_Inset(SMS_SIMCHAIN** pFirst,SMS_SIMCHAIN* pChainNode);
static void SIM_DeleteNode(PSMS_SIMCHAIN* ppFirst,PSMS_SIMCHAIN* ppUnreadFirst,SMS_SIMCHAIN* pChainNode);
static void SIM_Erase(PSMS_SIMCHAIN* ppFirst);

HWND SMS_CreateControlSIMWnd(void);
LRESULT SMSSIMCtlWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

BOOL SIMCreateMultiPickerWnd(HWND hWnd,HWND hMsgWnd,UINT uMsgCmd,SMS_SIMCHAIN* pUnreadHeader,
                             SMS_SIMCHAIN* pHeader,SMS_SIMCHAIN** ppnId);
LRESULT SIMMultiPickWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL SIMMultiPick_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void SIMMultiPick_OnActivate(HWND hwnd, UINT state);
static void SIMMultiPick_OnSetFocus(HWND hWnd);
static void SIMMultiPick_OnPaint(HWND hWnd);
static void SIMMultiPick_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void SIMMultiPick_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void SIMMultiPick_OnDestroy(HWND hWnd);
static void SIMMultiPick_OnClose(HWND hWnd);
static void SIMMultiPick_OnPick(HWND hWnd,char* szString,int nLen);

BOOL SIM_MultiPickRegisterClass(void);

BOOL SMS_ParseContentEx(int dcs , char* pszSrc, int nSrcLen,char **ppszDec , int *pDeclen);

BOOL SMS_IsUserCancel(HWND hWnd,UINT uMsgCmd);

/*********************************************************************\
* Function	   SMS_CreateSIMWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_CreateSIMWnd(HWND hFrameWnd)
{
    HMENU hDelManyMenu;
    WNDCLASS wc;
	SMS_SIMCREATEDATA Data;
	RECT	 rcClient;

    if( GetSIMState() != 1 )
    {
        PLXTipsWin(NULL,NULL,0,IDS_NOSIMCARD,IDS_SIMMSG,Notify_Info,IDS_OK,NULL,WAITTIMEOUT);

        return FALSE;
    }
    
    wc.style         = 0;
    wc.lpfnWndProc   = SMSSIMWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(SMS_SIMCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "SMSSIMWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;
    
	memset(&Data,0,sizeof(SMS_SIMCREATEDATA));
	
	Data.hFrameWnd = MuGetFrame();

    Data.hMenu = CreateMenu();

    Data.bOK = FALSE;

	GetClientRect(MuGetFrame(),&rcClient);

    hSIMWnd = CreateWindow(
        "SMSSIMWndClass", 
        "",
        WS_VISIBLE | WS_CHILD,
		rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
        MuGetFrame(),
        (HMENU)IDC_SMS_SIM,
        NULL, 
        (PVOID)&Data
        );
    
    if (!hSIMWnd)
    {
		DestroyMenu(Data.hMenu);

        UnregisterClass("SMSSIMWndClass", NULL);
        
		return FALSE;
    }

	SetFocus(hSIMWnd);

	SetWindowText(MuGetFrame(),IDS_SIMMSG);

    WaitWin(hSIMWnd,TRUE,IDS_WAITING,IDS_SIMMSG,NULL,IDS_CANCEL,WM_SIM_CANCEL);    

	PDASetMenu(MuGetFrame(),Data.hMenu);

    AppendMenu(Data.hMenu,MF_ENABLED, IDM_SMSSIM_COPYTO, IDS_COPYTOFOLDER);
    AppendMenu(Data.hMenu,MF_ENABLED, IDM_SMSSIM_DELETE, IDS_DELETE);
    hDelManyMenu = CreateMenu();
    AppendMenu(hDelManyMenu,MF_ENABLED, IDM_SMSSIM_DELETEMANY_SELECT, IDS_SELECT);
    AppendMenu(hDelManyMenu,MF_ENABLED, IDM_SMSSIM_DELETEMANY_ALL, IDS_ALL);
    AppendMenu(Data.hMenu,MF_POPUP|MF_ENABLED, (DWORD)hDelManyMenu, IDS_DELETEMANY);
         
    SendMessage(MuGetFrame(), PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_BACK);
    SendMessage(MuGetFrame(), PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)"");
    SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    
    SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    //show window
    ShowWindow(hSIMWnd, SW_SHOW);
    UpdateWindow(hSIMWnd);

    return TRUE;
}

/*********************************************************************\
* Function	MUSIMWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT SMSSIMWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = SMSSIM_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        SMSSIM_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;
        
//    case WM_SETFOCUS:
//        SMSSIM_OnSetFocus(hWnd);
//        break;

//    case WM_INITMENU:
//        SMSSIM_OnInitmenu(hWnd);
//        break;

    case WM_PAINT:
        SMSSIM_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        SMSSIM_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        SMSSIM_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        SMSSIM_OnClose(hWnd);
        break;

    case WM_DESTROY:
        SMSSIM_OnDestroy(hWnd);
        break;

	case WM_SIMDELALL:
		SMSSIM_OnDelAll(hWnd,(BOOL)lParam);
		break;

	case WM_SIMDEL:
		SMSSIM_OnDel(hWnd,(BOOL)lParam);
		break;

    case WM_SMS_MEMCOUNT:
		GetSIMCount(hWnd,wParam,lParam);
		break;
		
	case WM_SMS_PDU_LIST:
		GetSMSFromSIM(hWnd,wParam,lParam);
		break;

    case WM_TIMER:
        SMSSIM_OnTimer(hWnd,(WPARAM)(UINT)(wParam));
        break;

    case WM_SIM_OK:
        SMSSIM_OnOK(hWnd);
        break;

    case WM_SIM_CANCEL:
        SMSSIM_OnCancel(hWnd);
        break;

    case WM_SURETODIAL:
        SMSSIM_OnDial(hWnd,(BOOL)lParam);
        break;

    case WM_COPYSIM:
        SMSSIM_OnCopy(hWnd,(BOOL)wParam,(int)lParam);
        break;

    case WM_SIMDELSEL:
        SMSSIM_OnDelSel(hWnd,(BOOL)LOWORD(wParam),(int)HIWORD(wParam),(SMS_SIMCHAIN**)lParam);
        break;

    case WM_SURETODELSEL:
        SMSSIM_OnSureToDelSel(hWnd,(BOOL)lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	SMSSIM_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL SMSSIM_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    PSMS_SIMCREATEDATA pCreateData;
    HWND    hLst = NULL;
    RECT    rect;

    pCreateData = GetUserData(hWnd);

	memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(SMS_SIMCREATEDATA));

    GetClientRect(hWnd,&rect);

    hLst = CreateWindow(
        "LISTBOX", 
        "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP | LBS_MULTILINE,
        0,
        0,
        rect.right-rect.left,
        rect.bottom-rect.top, 
        hWnd, 
        (HMENU)IDC_SIMLSIT_LIST, 
        NULL, 
        NULL);
    
    if(hLst == NULL)
        return FALSE;

    //SetFocus(hLst);

    ShowWindow(hLst,SW_HIDE);
    
    pCreateData->hBitmapRead = LoadImage(NULL, IMG_READ_BMP, IMAGE_BITMAP, ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);
    pCreateData->hBitmapUnread = LoadImage(NULL, IMG_UNREAD_BMP, IMAGE_BITMAP, ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);

    //SetTimer(hWnd,ID_TIMER_GETMEM,OVERTIME,NULL);
    ReadSMSCountInSIM(hWnd);
    
    return TRUE;
    
}
/*********************************************************************\
* Function	SMSSIM_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSSIM_OnActivate(HWND hWnd, UINT state)
{
    HWND hLst;
    PSMS_SIMCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);
	
	hLst = GetDlgItem(hWnd,IDC_SIMLSIT_LIST);

	if(pCreateData->ncount != 0)
    {
		SetFocus(hLst);
        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    }
	else
    {
		SetFocus(hWnd);

        InvalidateRect(hWnd,NULL,TRUE);

        UpdateWindow(hWnd);

        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    }

	SetWindowText(pCreateData->hFrameWnd,IDS_SIMMSG);

	PDASetMenu(pCreateData->hFrameWnd,pCreateData->hMenu);

    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    
    
    SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return;
}
/*********************************************************************\
* Function	SMSSIM_OnSetFocus
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSSIM_OnSetFocus(HWND hWnd)
{
    HWND hLst;
	
	hLst = GetDlgItem(hWnd,IDC_SIMLSIT_LIST);
	if(IsWindowVisible(hLst))
		SetFocus(hLst);

    return;
}
/*********************************************************************\
* Function	SMSSIM_OnInitmenu
* Purpose   WM_INITMENU message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSIM_OnInitmenu(HWND hWnd)
{
    PSMS_SIMCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);
    
    return;

}
/*********************************************************************\
* Function	SMSSIM_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSIM_OnPaint(HWND hWnd)
{
    PSMS_SIMCREATEDATA	pCreateData;
	RECT				rect;
    int                 nOldMode;
    COLORREF            OldClr;
    HWND                hLst;

	HDC hdc = BeginPaint(hWnd, NULL);

    pCreateData = GetUserData(hWnd);

	if(pCreateData->ncount == 0)
	{
        hLst = GetDlgItem(hWnd,IDC_SIMLSIT_LIST);

//        if(IsWindowVisible(hLst) == TRUE)
  //          ShowWindow(hLst,SW_HIDE);
		GetClientRect(hWnd,&rect);
        OldClr = SetBkColor(hdc, COLOR_TRANSBK);
        nOldMode = SetBkMode(hdc,BM_TRANSPARENT);
		DrawText(hdc,IDS_NORECORD,-1,&rect,DT_CENTER|DT_VCENTER|DT_CLEAR);
        SetBkMode(hdc,nOldMode);
        SetBkColor(hdc, nOldMode);
	}

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	SMSSIM_OnKey
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
static void SMSSIM_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
	switch (vk)
	{
	case VK_F10:      
        PostMessage(hWnd,WM_CLOSE,0,0);
		break;

    case VK_F5:
        {
            HWND hList;

            hList = GetDlgItem(hWnd,IDC_SIMLSIT_LIST);

            if(IsWindowVisible(hList))
                PDADefWindowProc(GetParent(hWnd), WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        }
        break;

    case VK_F1:
        {
            SMS_SIMCHAIN *pNode;
            int index;
            char szPrompt[50];
            char szName[50];
            HDC  hdc;
            ABNAMEOREMAIL ABName;
            char *p;
            HWND hLst;

            hLst = GetDlgItem(hWnd,IDC_SIMLSIT_LIST);

            index = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(index == LB_ERR)
                break;
            
            pNode = (SMS_SIMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,index,0);
            
            memset(&ABName,0,sizeof(ABNAMEOREMAIL));
            
            if(APP_GetNameByPhone(pNode->psmsinfo->SenderNum,&ABName))
            {
                p = ABName.szTelOrEmail;
            }
            else
            {
                p = pNode->psmsinfo->SenderNum;
            }

            hdc = GetDC(hWnd);
            
            GetExtentFittedText(hdc,p,-1,szName,50,TEXT_WIDTH,OMIT_SUFFIX,SUFFIX_REPEAT);
            
            ReleaseDC(hWnd,hdc);

            szPrompt[0] = 0;
                        
            sprintf(szPrompt,"%s: \r\n %s",IDS_CALLTO,szName);

            PLXConfirmWinEx(GetParent(hWnd),hWnd,szPrompt,Notify_Request,IDS_SIMMSG,IDS_YES,IDS_NO,WM_SURETODIAL);
        }   
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	SMSSIM_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void SMSSIM_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    PSMS_SIMCREATEDATA pCreateData;
    HWND hLst = NULL;
    PSMS_INFO pSIMInfo;
    int  index;

    pCreateData = GetUserData(hWnd);
    pSIMInfo = pCreateData->psmsinfo;
    hLst = GetDlgItem(hWnd,IDC_SIMLSIT_LIST);
    index = SendMessage(hLst,LB_GETCURSEL,0,0);

	switch(id)
	{
    case IDM_SMSSIM_COPYTO:

        if(index == LB_ERR)
            break;

//        if(IsFlashEnough())
        {
            pCreateData->pSelectNode = (SMS_SIMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,index,NULL);
            
            MU_FolderSelectionEx(pCreateData->hFrameWnd,hWnd,IDS_COPYTOFOLDER,WM_COPYSIM,-1);
        }
        break;
        
    case IDM_SMSSIM_DELETE:
        
        if(index == LB_ERR)
            break;
        
        pCreateData->pSelectNode = (SMS_SIMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,index,NULL);

        PLXConfirmWinEx(MuGetFrame(),hWnd,IDS_DELETEMESSAGE,Notify_Request, IDS_SIMMSG,
            IDS_YES, IDS_NO , WM_SIMDEL);

        break;

    case IDM_SMSSIM_DELETEMANY_SELECT:
        
        pCreateData->ppId = (SMS_SIMCHAIN**)malloc(sizeof(SMS_SIMCHAIN*)*pCreateData->nmaxcount);

        if(pCreateData->ppId == NULL)
            break;

        memset(pCreateData->ppId,0,sizeof(SMS_SIMCHAIN*)*pCreateData->nmaxcount);
                    
        SIMCreateMultiPickerWnd(pCreateData->hFrameWnd,hWnd,WM_SIMDELSEL,pCreateData->pUnreadHeader,
            pCreateData->pHeader,pCreateData->ppId);
        
        break;
        
    case IDM_SMSSIM_DELETEMANY_ALL:
        
        pCreateData->nSelectCount = SendMessage(hLst,LB_GETCOUNT,0,0);
        
        pCreateData->ppId = (SMS_SIMCHAIN**)malloc(sizeof(SMS_SIMCHAIN*)*pCreateData->nSelectCount);

        if(pCreateData->ppId == NULL)
            break;

        memset(pCreateData->ppId,0,sizeof(SMS_SIMCHAIN*)*pCreateData->nSelectCount);

        {
            int i;

            for(i = 0 ; i < pCreateData->nSelectCount ; i++)
            {
                pCreateData->ppId[i] = (SMS_SIMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,i,NULL);
            }
        }

        PLXConfirmWinEx(MuGetFrame(),hWnd,IDS_DELETEALL,Notify_Request, IDS_SIMMSG, 
			IDS_YES, IDS_NO,WM_SIMDELALL);

        break;

    default:
        break;
	}

	return;
}
/*********************************************************************\
* Function	SMSSIM_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSIM_OnDestroy(HWND hWnd)
{
    PSMS_SIMCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);

    pCreateData->bOK = FALSE;

    if(pCreateData->hBitmapRead)
        DeleteObject(pCreateData->hBitmapRead);

    if(pCreateData->hBitmapUnread)
        DeleteObject(pCreateData->hBitmapUnread);

    SMS_FREE(pCreateData->psmsinfo);
    
    SMS_FREE(pCreateData->ppId);

    SIM_Erase(&(pCreateData->pHeader));
    
    SIM_Erase(&(pCreateData->pUnreadHeader));

    hSIMWnd = NULL;

	UnregisterClass("SMSSIMWndClass",NULL);
	
    return;

}
/*********************************************************************\
* Function	SMSSIM_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSIM_OnClose(HWND hWnd)
{
	SendMessage(GetParent(hWnd),PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);

    return;

}
/*********************************************************************\
* Function	 SMSSIM_OnOK  
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSIM_OnOK(HWND hWnd)
{
    PSMS_SIMCREATEDATA	pCreateData;
    PSMS_INFO			pInfo;
    HWND    hLst;
    int     i,nIndex;
    SYSTEMTIME sy,syLocal;
    char    szTimestamp[20],szTemp[20],szContext[256];
    DWORD   dwDateTime;
    SMS_SIMCHAIN * pNewNode;
    SMS_SIMCHAIN * pTemp;
    char *pszContent;
    int nLen;
    
    pCreateData = GetUserData(hWnd);
    
    pCreateData->bOK = TRUE;
    
    SetMTMemory();

    WaitWin(hWnd,FALSE,IDS_WAITING,IDS_SIMMSG,NULL,IDS_CANCEL,WM_SIM_CANCEL);
    
    hLst = GetDlgItem(hWnd,IDC_SIMLSIT_LIST);
    
    if(pCreateData->ncount > 0)
    {
        ShowWindow(hLst,SW_SHOW);

        SetFocus(hLst);

        memset(&sy,0,sizeof(SYSTEMTIME));
        GetLocalTime(&syLocal);            
        
        pInfo = pCreateData->psmsinfo;
        
        //SMS_OrderSIMRecord();
        for( i=0 ; i<pCreateData->ncount ; i++ )
        {                          
            pNewNode = SIM_New();

            if(pNewNode == NULL)
                PostMessage(hWnd,WM_SIM_CANCEL,0,0);

            pNewNode->nMemType = SMS_MEM_SM;

            pNewNode->psmsinfo = &(pInfo[i]);

            if(pInfo[i].Stat == SMS_UNREAD)
                SIM_Inset(&(pCreateData->pUnreadHeader),pNewNode);
            else
                SIM_Inset(&(pCreateData->pHeader),pNewNode);
        }

        pTemp = pCreateData->pUnreadHeader;
        
        while(pTemp)
        {   
            szTimestamp[0] = 0;

            dwDateTime = String2DWORD(pTemp->psmsinfo->SendTime);
            
            nIndex = SendMessage(hLst, LB_ADDSTRING, -1, (LPARAM)pTemp->psmsinfo->SenderNum);
            
            memset(&sy,0,sizeof(SYSTEMTIME));
            sy.wYear = (WORD)INTYEAR(dwDateTime);
            sy.wMonth = (WORD)INTMONTH(dwDateTime);
            sy.wDay = (WORD)INTDAY(dwDateTime);
            sy.wHour = (WORD)INTHOUR(dwDateTime);
            sy.wMinute = (WORD)INTMINUTE(dwDateTime);
            sy.wSecond = (WORD)INTSECOND(dwDateTime);
            
            if(sy.wYear != syLocal.wYear)
                sprintf(szTimestamp,"%d",sy.wYear);
            else if(sy.wMonth != syLocal.wMonth || sy.wDay != syLocal.wDay)
            {
                sy.wYear = 0;
                GetTimeDisplay(sy,szTemp,szTimestamp);
            }
            else
                GetTimeDisplay(sy,szTimestamp,szTemp);
            
            SMS_ParseContentEx(pTemp->psmsinfo->dcs, pTemp->psmsinfo->Context , pTemp->psmsinfo->ConLen ,
                &pszContent,&nLen);
            
            szContext[0] = 0;

            sprintf(szContext,"%s %s",szTimestamp,pszContent);
            
            SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)szContext);

            SendMessage(hLst, LB_SETITEMDATA, nIndex, (LPARAM)pTemp);
            
            SendMessage(hLst, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nIndex), (LPARAM)pCreateData->hBitmapUnread);
            
            SMS_FREE(pszContent);

            pTemp = pTemp->pNext;
        }
        
        pTemp = pCreateData->pHeader;
        
        while(pTemp)
        {   
            szTimestamp[0] = 0;

            dwDateTime = String2DWORD(pTemp->psmsinfo->SendTime);
            
            nIndex = SendMessage(hLst, LB_ADDSTRING, -1, (LPARAM)pTemp->psmsinfo->SenderNum);
            
            memset(&sy,0,sizeof(SYSTEMTIME));
            sy.wYear = (WORD)INTYEAR(dwDateTime);
            sy.wMonth = (WORD)INTMONTH(dwDateTime);
            sy.wDay = (WORD)INTDAY(dwDateTime);
            sy.wHour = (WORD)INTHOUR(dwDateTime);
            sy.wMinute = (WORD)INTMINUTE(dwDateTime);
            sy.wSecond = (WORD)INTSECOND(dwDateTime);
            
            if(sy.wYear != syLocal.wYear)
                sprintf(szTimestamp,"%d",sy.wYear);
            else if(sy.wMonth != syLocal.wMonth || sy.wDay != syLocal.wDay)
            {
                sy.wYear = 0;
                GetTimeDisplay(sy,szTemp,szTimestamp);
            }
            else
                GetTimeDisplay(sy,szTimestamp,szTemp);
            
            SMS_ParseContentEx(pTemp->psmsinfo->dcs, pTemp->psmsinfo->Context , pTemp->psmsinfo->ConLen ,
                &pszContent,&nLen);
            
            szContext[0] = 0;

            sprintf(szContext,"%s %s",szTimestamp,pszContent);
            
            SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)szContext);
            
            SendMessage(hLst, LB_SETITEMDATA, nIndex, (LPARAM)pTemp);
            
            SendMessage(hLst, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nIndex), (LPARAM)pCreateData->hBitmapRead);

            SMS_FREE(pszContent);

            pTemp = pTemp->pNext;
        }

        SendMessage(hLst,LB_SETCURSEL,0,0);
        
        SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);

    }
    else
    {
        ShowWindow(hLst,SW_HIDE);
    }
       
    //WaitWindowStateEx(pCreateData->hFrameWnd,FALSE,IDS_WAITING,IDS_SIMMSG,NULL,IDS_CANCEL);
}
/*********************************************************************\
* Function	 SMSSIM_OnCancel  
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSIM_OnCancel(HWND hWnd)
{
    PSMS_SIMCREATEDATA pCreateData;
    
    SetMTMemory();
    
    WaitWin(hWnd,FALSE,IDS_WAITING,IDS_SIMMSG,NULL,IDS_CANCEL,WM_SIM_CANCEL); 

    pCreateData = GetUserData(hWnd);

    ME_CancelFunction(pCreateData->MEHandle);
    
    PostMessage(hWnd,WM_CLOSE,0,0);
}

/*********************************************************************\
* Function	 SMSSIM_OnDial  
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSIM_OnDial(HWND hWnd,BOOL bSure)
{   
    SMS_SIMCHAIN *pNode;
    int index;
    HWND hLst;
    
    if(bSure == FALSE)
        return;
    
    hLst = GetDlgItem(hWnd,IDC_SIMLSIT_LIST);
    
    index = SendMessage(hLst,LB_GETCURSEL,0,0);
    
    if(index == LB_ERR)
        return;
    
    pNode = (SMS_SIMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,index,0);
    
    APP_CallPhoneNumber(pNode->psmsinfo->SenderNum);
}
/*********************************************************************\
* Function	 SMSSIM_OnCopy  
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSIM_OnCopy(HWND hWnd,BOOL bSelect,int nFolderID)
{
    PSMS_SIMCREATEDATA pCreateData;
    HWND hLst = NULL;
    PSMS_INFO pSIMInfo;
    SMS_STORE smsstore;
    DWORD dwOffset;
    SMS_SIMCHAIN*  pNode;
    char* pszNewFileName = NULL;
    char szNewFileName[SMS_FILENAME_MAXLEN];
    char szOldPath[PATH_MAXLEN];

    if(bSelect == FALSE)
        return;
    
    pCreateData = GetUserData(hWnd);
    pSIMInfo = pCreateData->psmsinfo;
    hLst = GetDlgItem(hWnd,IDC_SIMLSIT_LIST);

//    if(IsFlashEnough() == FALSE)
//         return;

    pNode = pCreateData->pSelectNode;
    
    WaitWin(hWnd,TRUE,(PSTR)IDS_COPYING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELCOPY);

    if(SMS_IsUserCancel(hWnd,WM_CANCELCOPY))
    {   
        WaitWin(hWnd,FALSE,(PSTR)IDS_COPYING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELCOPY);
        
        return;
    }
    
    memset(&smsstore,0,sizeof(SMS_STORE));
    
    smsstore.fix.dcs = pNode->psmsinfo->dcs;
    smsstore.fix.dwDateTime = String2DWORD(pNode->psmsinfo->SendTime);
    smsstore.fix.Phonelen = strlen(pNode->psmsinfo->SenderNum)+1;
    strcpy(smsstore.fix.SCA,pNode->psmsinfo->SCA);
    smsstore.fix.Status = pNode->psmsinfo->Status;
    smsstore.fix.Stat = MU_STU_UNREAD;
    smsstore.fix.Type = pNode->psmsinfo->Type;
    smsstore.fix.Udhlen  = pNode->psmsinfo->udhl;
    smsstore.pszPhone = pNode->psmsinfo->SenderNum;
    smsstore.pszUDH = pNode->psmsinfo->UDH;
    smsstore.fix.Conlen = pNode->psmsinfo->ConLen;
    smsstore.pszContent = pNode->psmsinfo->Context;
    
    //SMS_ParseContent(pNode->psmsinfo->dcs, pNode->psmsinfo->Context , pNode->psmsinfo->ConLen ,&smsstore);
        
    switch(nFolderID)
    {   
    case MU_INBOX: // a big file
        pszNewFileName = SMS_FILENAME_INBOX;
        break;
              
    case MU_DRAFT:
    case MU_OUTBOX:
    case MU_SENT:
    case MU_REPORT:
        return;
        
    default:
        szNewFileName[0] = 0;
        sprintf(szNewFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nFolderID,SMS_FILE_POSTFIX);
        pszNewFileName = szNewFileName;
        break;
    }
    
    szOldPath[0] = 0;      
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS); 
    
    if(SMS_SaveRecord(pszNewFileName,&smsstore,&dwOffset))
    {
        SMS_ChangeCount(nFolderID,SMS_COUNT_ALL,1);
        
        SMS_ChangeCount(nFolderID,SMS_COUNT_UNREAD,1);
        
        chdir(szOldPath);
        
        MU_NewMsgArrival(MU_SMS_NOTIFY);
        
        WaitWin(hWnd,FALSE,(PSTR)IDS_COPYING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELCOPY);
        
        PLXTipsWin(NULL,NULL,0,IDS_MESSAGECOPIED,IDS_SIMMSG,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    }
}

/*********************************************************************\
* Function	 SMSSIM_OnDelSel  
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSIM_OnDelSel(HWND hWnd,BOOL bSure,int nCount,SMS_SIMCHAIN** ppId)
{
    PSMS_SIMCREATEDATA pCreateData;
    
    if(bSure == FALSE)
    {
        SMS_FREE(ppId);

        return;
    }
    pCreateData = GetUserData(hWnd);

    pCreateData->nSelectCount = nCount;

    PLXConfirmWinEx(pCreateData->hFrameWnd,hWnd,IDS_DELSEL,Notify_Request,IDS_SIMMSG,IDS_YES,IDS_NO,WM_SURETODELSEL);

}

/*********************************************************************\
* Function	 SMSSIM_OnSureToDelSel  
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSIM_OnSureToDelSel(HWND hWnd,BOOL bSure)
{
    PSMS_SIMCREATEDATA pCreateData;
    HWND hLst = NULL;
    PSMS_INFO pSIMInfo;
    int  index,i,j;
    int  nCounter;

    pCreateData = GetUserData(hWnd);
    
    if(bSure == FALSE)
    {
        SMS_FREE(pCreateData->ppId);
        
        pCreateData->nSelectCount = 0;

        return;
    }

    pSIMInfo = pCreateData->psmsinfo;
    hLst = GetDlgItem(hWnd,IDC_SIMLSIT_LIST);
    index = SendMessage(hLst,LB_GETCURSEL,0,0);
	
	WaitWin(hWnd,TRUE,(PSTR)IDS_DELETING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELDEL);
	
    for(i = pCreateData->nSelectCount-1 ; i >= 0 ; i--)
	{
		SMS_SIMCHAIN *pTemp;
		MSG myMsg;
		int nSim;
				
        if(SMS_IsUserCancel(hWnd,WM_CANCELDEL))
        {
            WaitWin(hWnd,FALSE,(PSTR)IDS_DELETING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELDEL);

            pCreateData->ncount = SendMessage(hLst,LB_GETCOUNT,0,0);
            
            if(pCreateData->ncount == 0)
            {
                ShowWindow(hLst,SW_HIDE);
                SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            }

            SMS_FREE(pCreateData->ppId);

            pCreateData->nSelectCount = 0;

            return;
        }

        if(SIM_Delete(pCreateData->ppId[i]->psmsinfo->Index,pCreateData->ppId[i]->nMemType))
		{
			while(GetMessage(&myMsg, NULL, 0, 0)) 
			{
				if((myMsg.message == MSG_SIM_DELETE_SUCC) && (myMsg.hwnd == hSIMCtlWnd))
				{
					nSim = SMS_GetSIMCounter();
					nSim--;
					SMS_SetSIMCounter(nSim);
					
                    SMS_SetFull(FALSE);
                    
                    SMS_NotifyIdle();

                    nCounter = SendMessage(hLst,LB_GETCOUNT,0,0);

                    for(j = nCounter-1 ; j >= 0 ; j--)
                    {                        
                        pTemp = (SMS_SIMCHAIN*)SendMessage(hLst, LB_GETITEMDATA, j, NULL);
                        if(pCreateData->ppId[i] == pTemp)
                        {
                            SendMessage(hLst,LB_DELETESTRING,j,0);
                            if(j == nCounter-1)
                                SendMessage(hLst, LB_SETCURSEL, j-1, 0);
                            else
                                SendMessage(hLst, LB_SETCURSEL, j, 0); 
                            
                            memset(pTemp->psmsinfo,0,sizeof(SMS_INFO));
                            
                            pTemp->psmsinfo->Index = SMS_SIMINDEX_NULL;
                            
                            SIM_DeleteNode(&(pCreateData->pHeader),&(pCreateData->pUnreadHeader),pTemp);
                            break;
                        }
                    }
                    break;
				}
				else if((myMsg.message == MSG_SIM_DELETE_FAIL) && (myMsg.hwnd == hSIMCtlWnd))
				{
					break;
				}
                else if(myMsg.message == WM_KEYDOWN && myMsg.wParam == VK_F10)
                {
					nSim = SMS_GetSIMCounter();
					nSim--;
					SMS_SetSIMCounter(nSim);
					
                    SMS_SetFull(FALSE);
                    
                    SMS_NotifyIdle();

                    nCounter = SendMessage(hLst,LB_GETCOUNT,0,0);

                    for(j = nCounter-1 ; j >= 0 ; j--)
                    {                        
                        pTemp = (SMS_SIMCHAIN*)SendMessage(hLst, LB_GETITEMDATA, j, NULL);
                        if(pCreateData->ppId[i] == pTemp)
                        {
                            SendMessage(hLst,LB_DELETESTRING,j,0);
                            if(j == nCounter-1)
                                SendMessage(hLst, LB_SETCURSEL, j-1, 0);
                            else
                                SendMessage(hLst, LB_SETCURSEL, j, 0); 
                            
                            memset(pTemp->psmsinfo,0,sizeof(SMS_INFO));
                            
                            pTemp->psmsinfo->Index = SMS_SIMINDEX_NULL;
                            
                            SIM_DeleteNode(&(pCreateData->pHeader),&(pCreateData->pUnreadHeader),pTemp);
                            break;
                        }
                    }
                    
                    WaitWin(hWnd,FALSE,(PSTR)IDS_DELETING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELDEL);
                    
                    pCreateData->ncount = SendMessage(hLst,LB_GETCOUNT,0,0);
                    
                    pCreateData->nSelectCount = 0;
                    
                    SMS_FREE(pCreateData->ppId);
                    
                    if(pCreateData->ncount == 0)
                    {
                        ShowWindow(hLst,SW_HIDE);
                        SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
                    }
                    
                    return;
                }
				TranslateMessage(&myMsg);
				DispatchMessage(&myMsg);
			}
		}  
	}

	pCreateData->ncount = SendMessage(hLst,LB_GETCOUNT,0,0);
	
    WaitWin(hWnd,FALSE,(PSTR)IDS_DELETING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELDEL);
	
    if(pCreateData->ncount == 0)
	{
		ShowWindow(hLst,SW_HIDE);
		SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
	}

    SMS_FREE(pCreateData->ppId);
    
    pCreateData->nSelectCount = 0;
    
    PLXTipsWin(NULL,NULL,0,IDS_DELETED,IDS_SIMMSG,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    
}
/*********************************************************************\
* Function	 SMSSIM_OnDelAll  
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSIM_OnDelAll(HWND hWnd,BOOL bSure)
{
    PSMS_SIMCREATEDATA pCreateData;
    HWND hLst = NULL;
    PSMS_INFO pSIMInfo;
    int  index,i,j;
    int  nCounter;
	
    pCreateData = GetUserData(hWnd);
    
	if(bSure == FALSE)
    {
        SMS_FREE(pCreateData->ppId);

        pCreateData->nSelectCount = 0;

		return;
    }
	
    pSIMInfo = pCreateData->psmsinfo;
    hLst = GetDlgItem(hWnd,IDC_SIMLSIT_LIST);
    index = SendMessage(hLst,LB_GETCURSEL,0,0);
	
	WaitWin(hWnd,TRUE,(PSTR)IDS_DELETING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELDEL);
		
    for(i = pCreateData->nSelectCount-1 ; i >= 0 ; i--)
	{
		SMS_SIMCHAIN *pTemp;
		MSG myMsg;
		int nSim;
				
        if(SMS_IsUserCancel(hWnd,WM_CANCELDEL))
        {
            WaitWin(hWnd,FALSE,(PSTR)IDS_DELETING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELDEL);

            pCreateData->ncount = SendMessage(hLst,LB_GETCOUNT,0,0);
            
            if(pCreateData->ncount == 0)
            {
                ShowWindow(hLst,SW_HIDE);
                SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            }
            SMS_FREE(pCreateData->ppId);

            pCreateData->nSelectCount = 0;

            return;
        }

        if(SIM_Delete(pCreateData->ppId[i]->psmsinfo->Index,pCreateData->ppId[i]->nMemType))
		{
			while(GetMessage(&myMsg, NULL, 0, 0)) 
			{
				if((myMsg.message == MSG_SIM_DELETE_SUCC) && (myMsg.hwnd == hSIMCtlWnd))
				{
					nSim = SMS_GetSIMCounter();
					nSim--;
					SMS_SetSIMCounter(nSim);
					
                    SMS_SetFull(FALSE);
                    
                    SMS_NotifyIdle();

                    nCounter = SendMessage(hLst,LB_GETCOUNT,0,0);

                    for(j = nCounter-1 ; j >= 0 ; j--)
                    {                        
                        pTemp = (SMS_SIMCHAIN*)SendMessage(hLst, LB_GETITEMDATA, j, NULL);
                        if(pCreateData->ppId[i] == pTemp)
                        {
                            SendMessage(hLst,LB_DELETESTRING,j,0);
                            if(j == nCounter-1)
                                SendMessage(hLst, LB_SETCURSEL, j-1, 0);
                            else
                                SendMessage(hLst, LB_SETCURSEL, j, 0); 
                            
                            memset(pTemp->psmsinfo,0,sizeof(SMS_INFO));
                            
                            pTemp->psmsinfo->Index = SMS_SIMINDEX_NULL;
                            
                            SIM_DeleteNode(&(pCreateData->pHeader),&(pCreateData->pUnreadHeader),pTemp);
                            break;
                        }
                    }
                    break;
				}
				else if((myMsg.message == MSG_SIM_DELETE_FAIL) && (myMsg.hwnd == hSIMCtlWnd))
				{
					break;
				}
                else if(myMsg.message == WM_KEYDOWN && myMsg.wParam == VK_F10)
                {
					nSim = SMS_GetSIMCounter();
					nSim--;
					SMS_SetSIMCounter(nSim);
					
                    SMS_SetFull(FALSE);
                    
                    SMS_NotifyIdle();

                    nCounter = SendMessage(hLst,LB_GETCOUNT,0,0);

                    for(j = nCounter-1 ; j >= 0 ; j--)
                    {                        
                        pTemp = (SMS_SIMCHAIN*)SendMessage(hLst, LB_GETITEMDATA, j, NULL);
                        if(pCreateData->ppId[i] == pTemp)
                        {
                            SendMessage(hLst,LB_DELETESTRING,j,0);
                            if(j == nCounter-1)
                                SendMessage(hLst, LB_SETCURSEL, j-1, 0);
                            else
                                SendMessage(hLst, LB_SETCURSEL, j, 0); 
                            
                            memset(pTemp->psmsinfo,0,sizeof(SMS_INFO));
                            
                            pTemp->psmsinfo->Index = SMS_SIMINDEX_NULL;
                            
                            SIM_DeleteNode(&(pCreateData->pHeader),&(pCreateData->pUnreadHeader),pTemp);
                            break;
                        }
                    }
                    
                    WaitWin(hWnd,FALSE,(PSTR)IDS_DELETING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELDEL);
                    
                    pCreateData->ncount = SendMessage(hLst,LB_GETCOUNT,0,0);
                    
                    pCreateData->nSelectCount = 0;
                    
                    if(pCreateData->ncount == 0)
                    {
                        ShowWindow(hLst,SW_HIDE);
                        SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
                    }
                    
                    SMS_FREE(pCreateData->ppId);
                    
                    return;
                }
				TranslateMessage(&myMsg);
				DispatchMessage(&myMsg);
			}
		}  
	}

	pCreateData->ncount = SendMessage(hLst,LB_GETCOUNT,0,0);
	
    WaitWin(hWnd,FALSE,(PSTR)IDS_DELETING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELDEL);
	
    if(pCreateData->ncount == 0)
	{
		ShowWindow(hLst,SW_HIDE);
		SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
	}
    
    PLXTipsWin(NULL,NULL,0,IDS_DELETED,IDS_SIMMSG,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    
    SMS_FREE(pCreateData->ppId);
}
/*********************************************************************\
* Function	 SMSSIM_OnDel  
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSIM_OnDel(HWND hWnd,BOOL bSure)
{
	MSG myMsg;
    PSMS_SIMCREATEDATA pCreateData;
    HWND hLst = NULL;
    PSMS_INFO pSIMInfo;
    int  index;
    SMS_SIMCHAIN *pNode;
    int  nSim,i;
	
	if(bSure == FALSE)
		return;

    pCreateData = GetUserData(hWnd);
    pSIMInfo = pCreateData->psmsinfo;
    hLst = GetDlgItem(hWnd,IDC_SIMLSIT_LIST);
    index = SendMessage(hLst,LB_GETCURSEL,0,0);
	    
    WaitWin(hWnd,TRUE,(PSTR)IDS_DELETING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELDEL);
    
    if(SMS_IsUserCancel(hWnd,WM_CANCELDEL))
    {
        WaitWin(hWnd,FALSE,(PSTR)IDS_DELETING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELDEL);

        return;
    }

    pNode = pCreateData->pSelectNode;
    
    if(SIM_Delete(pNode->psmsinfo->Index,pNode->nMemType))
    {
        while(GetMessage(&myMsg, NULL, 0, 0)) 
        {
            if((myMsg.message == MSG_SIM_DELETE_SUCC) && (myMsg.hwnd == hSIMCtlWnd))
            {     
                nSim = SMS_GetSIMCounter();
                nSim--;
                SMS_SetSIMCounter(nSim);

                SMS_SetFull(FALSE);

                SMS_NotifyIdle();
                
                for(i = 0 ; i < pCreateData->ncount ; i++)
                {
                    if(pNode == (SMS_SIMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,i,NULL))
                    {
                        SendMessage(hLst,LB_DELETESTRING,i,0);
                        break;
                    }
                }
                
                if(index == pCreateData->ncount-1)
                    SendMessage(hLst, LB_SETCURSEL, index-1, 0);
                else
                    SendMessage(hLst, LB_SETCURSEL, index, 0);  
                
                pCreateData->ncount--;
                
                memset(pNode->psmsinfo,0,sizeof(SMS_INFO));
                
                pNode->psmsinfo->Index = SMS_SIMINDEX_NULL;
                
                SIM_DeleteNode(&(pCreateData->pHeader),&(pCreateData->pUnreadHeader),pNode);
                
                break;
            }
            else if((myMsg.message == MSG_SIM_DELETE_FAIL) && (myMsg.hwnd == hSIMCtlWnd))
            {
                break;
            }
            TranslateMessage(&myMsg);
            DispatchMessage(&myMsg);
        }    
    }

    WaitWin(hWnd,FALSE,(PSTR)IDS_DELETING,(PSTR)IDS_SIMMSG,NULL,IDS_CANCEL,WM_CANCELDEL);

    if(pCreateData->ncount == 0)
    {
        SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        ShowWindow(hLst,SW_HIDE);
    }

    PLXTipsWin(NULL,NULL,0,IDS_DELETED,IDS_SIMMSG,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    
}
/*********************************************************************\
* Function	SMSSIM_OnTimer
* Purpose   WM_TIMER message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSIM_OnTimer(HWND hWnd,UINT id)
{
    KillTimer(hWnd,id);
    switch(id)
    {
    case ID_TIMER_GETMEM:
        ReadSMSCountInSIM(hWnd);
    	break;

    case ID_TIMER_LISTSMS:
        ReadSMSFromSIM(hWnd);
    	break;

    default:
        break;
    }
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL ReadSMSCountInSIM(HWND hWnd)
{
    
    PSMS_SIMCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);
    
    pCreateData->MEHandle = ME_SMS_GetMemStatus(hWnd,WM_SMS_MEMCOUNT,SMS_MEM_SM);

	if(pCreateData->MEHandle >= 0)
		return TRUE;
	else
		return FALSE;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void GetSIMCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    SMS_SIMCREATEDATA* pCreateData;
    SMSMEM_INFO meminfo;
	int i,buflen;

    pCreateData = (SMS_SIMCREATEDATA*)GetUserData(hWnd);
	
    switch(wParam)
    {
    case ME_RS_SUCCESS:
        {
            buflen = 0;
            buflen = ME_GetResult(NULL,0);
            
            memset(&meminfo,0,sizeof(SMSMEM_INFO));
            if(buflen > 0 && buflen <= sizeof(SMSMEM_INFO))
            {
                if(-1 != ME_GetResult(&meminfo,buflen))
                {
                    pCreateData->ncount = meminfo.nMem1Used;
                    pCreateData->nmaxcount = meminfo.nMem1Total;
                    pCreateData->psmsinfo = (SMS_INFO*)malloc(pCreateData->nmaxcount*sizeof(SMS_INFO));//*(pCreateData->ppsmsinfo) = 

					SMS_SetSIMCounter(pCreateData->ncount);

                    if (pCreateData->psmsinfo == NULL)
                    {
                        PostMessage(hWnd,WM_SIM_CANCEL,0,0);
                        return;
                    }
                                        
                    memset(pCreateData->psmsinfo, 0, pCreateData->nmaxcount*sizeof(SMS_INFO));
                    
                    for( i=0 ; i<pCreateData->nmaxcount ; i++ )
                        (pCreateData->psmsinfo)[i].Index = SMS_SIMINDEX_NULL;
                    
                    if(pCreateData->ncount != 0)
                    {			
                        if(!ReadSMSFromSIM(hWnd))
                        {
                            SetTimer(hWnd,ID_TIMER_LISTSMS,OVERTIME,0);
                            return;
                        }
                        else
                            return;
                    }
                    else
                    {
                        PostMessage(hWnd,WM_SIM_OK,0,0);
                        return;
                    }
                }
            }
        }
        break;
    
    case ME_RS_USERSTOP:
    case ME_RS_NOSIMCARD:
    case ME_RS_ERRORSTATUS:
        PostMessage(hWnd,WM_SIM_CANCEL,0,0);
    	return;

    default:
        SetTimer(hWnd,ID_TIMER_GETMEM,OVERTIME,NULL);
        return;
    }
}
/********************************************************************
* Function	   ReadSMSFromSIM
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
static BOOL	ReadSMSFromSIM(HWND hWnd)
{
    
    PSMS_SIMCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);
    
    pCreateData->MEHandle = ME_SMS_List(hWnd,WM_SMS_PDU_LIST,SMS_ALL,TRUE,SMS_MEM_SM);

	if(0 <= pCreateData->MEHandle)
		return TRUE;
	else
		return FALSE;
}
/********************************************************************
* Function	   GetSMSFromSIM
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
static void	GetSMSFromSIM(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int buflen;
	PSMS_INFO pSmsInSIM;
    SMS_SIMCREATEDATA* pCreateData;

    pCreateData = (PSMS_SIMCREATEDATA)GetUserData(hWnd);

    pSmsInSIM = pCreateData->psmsinfo;
	
    switch(wParam)
    {
    case ME_RS_SUCCESS:
        {
            buflen = 0;
            buflen = ME_GetResult(NULL,0);
            if(buflen > 0 && buflen <= (pCreateData->ncount*(int)sizeof(SMS_INFO)))
            {
                if(buflen == 0)
                {      
                    PostMessage(hWnd,WM_SIM_CANCEL,0,0);
                    return;
                }
                
                if(-1 != ME_GetResult(pSmsInSIM, buflen))
                {
                    
                    pCreateData->ncount = buflen/sizeof(SMS_INFO);
                    
                    PostMessage(hWnd,WM_SIM_OK,0,0);
                    
                    return;
                }
                SetTimer(hWnd,ID_TIMER_LISTSMS,OVERTIME,NULL);
                return;
            }
        }
        break;
       
    case ME_RS_USERSTOP:
    case ME_RS_NOSIMCARD:
    case ME_RS_ERRORSTATUS:
        PostMessage(hWnd,WM_SIM_CANCEL,0,0);
        return;

    default:
        SetTimer(hWnd,ID_TIMER_LISTSMS,OVERTIME,NULL);
        return;
    }
}
/********************************************************************
* Function	   SMS_OrderByTime
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
static void SMS_OrderByTime(PSMS_INFO pSms_Info, int nSMSCountInSIM)
{
	int i,j,low,high,middle;
	SMS_INFO Sms_InfoTemp;

	for(i=0 ; i<nSMSCountInSIM ; i++)
	{
		if(pSms_Info[i].Stat == SMS_UNREAD)
		{
;//			nUnreadCount++;
		}
		if(i > 0)
		{
			memcpy(&Sms_InfoTemp,&pSms_Info[i],sizeof(SMS_INFO));
			
			/* ’€∞Î≈≈–Ú */
			low = 0;
			high = i-1;
			while(low <= high)
			{
				middle = (int)((low + high)/2);
				if(String2DWORD(pSms_Info[middle].SendTime) > String2DWORD(pSms_Info[i].SendTime))
					high = middle - 1; 
				else
					low = middle + 1;
			}
			
			for(j=i-1; j>=high+1 ; j--)
				memcpy(&pSms_Info[j+1],&pSms_Info[j],sizeof(SMS_INFO));
			memcpy(&pSms_Info[j+1],&Sms_InfoTemp,sizeof(SMS_INFO));
		}
	}
}
/*********************************************************************\
* Function	   SMS_CreateControlSIMWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
HWND SMS_CreateControlSIMWnd(void)
{
    WNDCLASS wc;

    wc.style         = 0;
	wc.lpfnWndProc   = SMSSIMCtlWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName  = "SMSSIMCtlWndClass";

    if (!RegisterClass(&wc))
		return NULL;

    hSIMCtlWnd = CreateWindow(
		"SMSSIMCtlWndClass",
		"", 
		NULL, 
		0, 0, 0, 0, 
		NULL,
		NULL, 
		NULL, 
		NULL
		);

    return hSIMCtlWnd;
}
/*********************************************************************\
* Function	   SMSSIMCtlWndProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
LRESULT SMSSIMCtlWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

    switch(wMsgCmd) 
    {
    case WM_CREATE:
    	break;

    case WM_SMS_PDU_DELETE:
        
        if(wParam == ME_RS_SUCCESS)
        {
            PostMessage(hWnd,MSG_SIM_DELETE_SUCC,0,0);
        }
        else
        {
            PostMessage(hWnd,MSG_SIM_DELETE_FAIL,0,0);
        }
        break;
            
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
    
}
/*********************************************************************\
* Function	   SIM_Delete
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SIM_Delete(int index,int MemType)
{
    if(GetSIMState() != 1)
    {
        //PLXTipsWin((char*)IDS_NOSIMCARD, (char*)IDS_WARNING, WAITTIMEOUT);
        return FALSE;	
    }
    
    if(0 > ME_SMS_Delete(hSIMCtlWnd,WM_SMS_PDU_DELETE,MemType,index))
    {	   
        return FALSE;
    }

    return TRUE;
}



#define IDC_OK		            100
#define IDC_CANCEL              200
#define IDC_SIMMULITPICK_LIST	300

typedef struct tagSIM_MultiPickData
{
    HWND                  hFrameWnd;
    HWND                  hMsgWnd;
    UINT                  uMsgCmd;
	SMS_SIMCHAIN**        ppnId;
	SMS_SIMCHAIN*         pUnreadHeader;
	SMS_SIMCHAIN*         pHeader;
}SIM_MULTIPICKDATA,*PSIM_MULTIPICKDATA;

//need to refresh according to contact update
BOOL SIMCreateMultiPickerWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,SMS_SIMCHAIN* pUnreadHeader,
                             SMS_SIMCHAIN* pHeader,SMS_SIMCHAIN** ppnId)
{
    SIM_MULTIPICKDATA CreateData;
    HWND        hMultiPickWnd;
    char        szCaption[50];
    RECT        rcClient;
    
    GetWindowText(hFrameWnd,szCaption,49);
    szCaption[49] = 0;
   
    memset(&CreateData,0,sizeof(SIM_MULTIPICKDATA));

    CreateData.hFrameWnd = hFrameWnd;
    CreateData.ppnId = ppnId;
    CreateData.pHeader = pHeader;
    CreateData.pUnreadHeader = pUnreadHeader;
    CreateData.hMsgWnd = hMsgWnd;
    CreateData.uMsgCmd = uMsgCmd;

	SIM_MultiPickRegisterClass();

    GetClientRect(hFrameWnd,&rcClient);

    hMultiPickWnd = CreateWindow(
        "SIMMultiPickWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        rcClient.left,
        rcClient.top,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hFrameWnd,
        NULL, 
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!hMultiPickWnd)
    {
        return FALSE;
    }

    SetFocus(hMultiPickWnd);
     
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_DELETE);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL, 0), (LPARAM)IDS_CANCEL);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    return TRUE;

}

BOOL SIM_MultiPickRegisterClass(void)
{
    WNDCLASS    wc;

    wc.style         = 0;
    wc.lpfnWndProc   = SIMMultiPickWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(SIM_MULTIPICKDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "SIMMultiPickWndClass";
    
    return(RegisterClass(&wc));
}

LRESULT SIMMultiPickWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = SIMMultiPick_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        SIMMultiPick_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        SIMMultiPick_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        SIMMultiPick_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        SIMMultiPick_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        SIMMultiPick_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        SIMMultiPick_OnClose(hWnd);
        break;

    case WM_DESTROY:
        SIMMultiPick_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	SIMMultiPick_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL SIMMultiPick_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    SIM_MULTIPICKDATA *pData;
    HWND hLst;
	int nIndex;
    PSMS_SIMCREATEDATA pCreateData;
    SMS_SIMCHAIN* pTemp;
    char *pszContent;
    int  nLen;

    pData = GetUserData(hWnd);

    pCreateData = GetUserData(pData->hMsgWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(SIM_MULTIPICKDATA));
    
    GetClientRect(hWnd,&rect);
    
    hLst = CreateWindow(
        "MULTILISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_SIMMULITPICK_LIST,
        NULL,
        NULL);
   
    if(hLst == NULL)
        return FALSE;
    
    pTemp = pData->pUnreadHeader;

    while(pTemp)
    {
        pszContent = NULL;

        SMS_ParseContentEx(pTemp->psmsinfo->dcs, pTemp->psmsinfo->Context, 
            pTemp->psmsinfo->ConLen,&pszContent,&nLen);
        
		nIndex = SendMessage(hLst, LB_ADDSTRING, -1, (LPARAM)pszContent);

        SMS_FREE(pszContent);
            
		SendMessage(hLst, LB_SETITEMDATA, nIndex, (LPARAM)pTemp);

        pTemp = pTemp->pNext;
    }
    
    pTemp = pData->pHeader;

    while(pTemp)
    {
        pszContent = NULL;

        SMS_ParseContentEx(pTemp->psmsinfo->dcs, pTemp->psmsinfo->Context, 
            pTemp->psmsinfo->ConLen,&pszContent,&nLen);
        
		nIndex = SendMessage(hLst, LB_ADDSTRING, -1, (LPARAM)pszContent);

        SMS_FREE(pszContent);
            
		SendMessage(hLst, LB_SETITEMDATA, nIndex, (LPARAM)pTemp);
        
        pTemp = pTemp->pNext;
    }
	
	SendMessage(hLst, LB_ENDINIT, 0, 0);

	SendMessage(hLst,LB_SETCURSEL,0,0);
            
    return TRUE;
    
}
/*********************************************************************\
* Function	SIMMultiPick_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SIMMultiPick_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;
    SIM_MULTIPICKDATA *pData;

    pData = GetUserData(hwnd);

    hLst = GetDlgItem(hwnd,IDC_SIMMULITPICK_LIST);

    SetFocus(hLst);
    
    SendMessage(pData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_DELETE);
    SendMessage(pData->hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL, 0), (LPARAM)IDS_CANCEL);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    return;
}
/*********************************************************************\
* Function	SIMMultiPick_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SIMMultiPick_OnSetFocus(HWND hWnd)
{
    HWND hLst;

    hLst = GetDlgItem(hWnd,IDC_SIMMULITPICK_LIST);

    SetFocus(hLst);
}
/*********************************************************************\
* Function	SIMMultiPick_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SIMMultiPick_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	SIMMultiPick_OnKey
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
static void SIMMultiPick_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PSIM_MULTIPICKDATA pData;

    pData = (PSIM_MULTIPICKDATA)GetUserData(hWnd);
    
    switch (vk)
	{
	case VK_F10:
		SendMessage(pData->hMsgWnd,pData->uMsgCmd,MAKEWPARAM(FALSE,0),0);
		
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

	case VK_RETURN:
		{
			HWND hLst;
			int  nCount,i,j;

			hLst = GetDlgItem(hWnd,IDC_SIMMULITPICK_LIST);

			nCount = SendMessage(hLst,LB_GETCOUNT,0,0);

			for(i = 0,j = 0; i < nCount ; i++)
			{
				if(SendMessage(hLst,LB_GETSEL,i,0))
				{
					pData->ppnId[j++] = (SMS_SIMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,i,0);
				}
			}
            if(j == 0)
            {
                PLXTipsWin(NULL,NULL,0,IDS_PLSSELECTMSGS,IDS_SIMMSG,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);

                break;
            }
    
			SendMessage(pData->hMsgWnd,pData->uMsgCmd,MAKEWPARAM(TRUE,j),(LPARAM)pData->ppnId);

			PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		}
		break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	SIMMultiPick_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void SIMMultiPick_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{
    default:
        break;
    }
}
/*********************************************************************\
* Function	SIMMultiPick_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SIMMultiPick_OnDestroy(HWND hWnd)
{

    return;

}
/*********************************************************************\
* Function	SIMMultiPick_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SIMMultiPick_OnClose(HWND hWnd)
{
    PSIM_MULTIPICKDATA pData;

    pData = (PSIM_MULTIPICKDATA)GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);

    return;

}

/*********************************************************************\
* Function	   SIM_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static SMS_SIMCHAIN* SIM_New(void)
{
    SMS_SIMCHAIN* p = NULL;

    p = (SMS_SIMCHAIN*)malloc(sizeof(SMS_SIMCHAIN));

    memset(p,0,sizeof(SMS_SIMCHAIN));

    return p;
}
/*********************************************************************\
* Function	   SIM_Inset
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int SIM_Inset(SMS_SIMCHAIN** pFirst,SMS_SIMCHAIN* pChainNode)
{
    if(*pFirst == NULL)
    {
        *pFirst = pChainNode;
        pChainNode->pPioneer = NULL;
        pChainNode->pNext = NULL;

        return 0;
    }
    else
    {
        SMS_SIMCHAIN *p;
        int k;
        BOOL bIsTail;

        k = 0;
        bIsTail = FALSE;
        p = *pFirst;
        while(  String2DWORD(p->psmsinfo->SendTime) > String2DWORD(pChainNode->psmsinfo->SendTime) )
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
            if( p == *pFirst)
            {
                pChainNode->pNext = *pFirst;
                pChainNode->pPioneer = NULL;
                (*pFirst)->pPioneer = pChainNode;
                *pFirst = pChainNode;
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
* Function	   SIM_DeleteNode
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SIM_DeleteNode(PSMS_SIMCHAIN* ppFirst,PSMS_SIMCHAIN* ppUnreadFirst,SMS_SIMCHAIN* pChainNode)
{
    if( pChainNode == *ppFirst )
    {
        if(pChainNode->pNext)
        {
            *ppFirst = pChainNode->pNext;
            pChainNode->pNext->pPioneer = NULL;
        }
        else
            *ppFirst = NULL;
    }
    else if(pChainNode == *ppUnreadFirst)
    {
        if(pChainNode->pNext)
        {
            *ppUnreadFirst = pChainNode->pNext;
            pChainNode->pNext->pPioneer = NULL;
        }
        else
            *ppUnreadFirst = NULL;
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

    SMS_FREE(pChainNode);
    pChainNode = NULL;
}
/*********************************************************************\
* Function	   SIM_Erase
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SIM_Erase(PSMS_SIMCHAIN* ppFirst)
{
    SMS_SIMCHAIN* p;    
    SMS_SIMCHAIN* ptemp;

    p = *ppFirst;

    while( p )
    {
        ptemp = p->pNext;
        
        SMS_FREE(p);
        
        p = ptemp;
    }

    *ppFirst = NULL;
}

/*********************************************************************\
* Function	   SMS_ParseContent
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_ParseContentEx(int dcs , char* pszSrc, int nSrcLen,char **ppszDec, int *pDeclen)
{
    if (dcs == DCS_GSM)
    {
        *pDeclen = GSMToMultiByte(pszSrc, nSrcLen, (LPWSTR)*ppszDec, 0, NULL, NULL);

        *ppszDec = malloc(*pDeclen+1);

        if(*ppszDec == NULL)
            return FALSE;
        
        GSMToMultiByte(pszSrc, nSrcLen, (LPWSTR)*ppszDec,*pDeclen+1, NULL, NULL);

        (*ppszDec)[*pDeclen] = 0;

        (*pDeclen)++;
    }
    else if (dcs == DCS_UCS2)
    {
        *pDeclen = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)pszSrc, nSrcLen/2, 
            *ppszDec, 0, NULL, NULL);

        *ppszDec = malloc(*pDeclen+1);

        if(*ppszDec == NULL)
            return FALSE;
        
        WideCharToMultiByte(CP_ACP, 0, (LPWSTR)pszSrc, nSrcLen/2, 
            *ppszDec, *pDeclen+1, NULL, NULL);

        (*ppszDec)[*pDeclen] = 0;

        (*pDeclen)++;
    }
    else if (dcs == DCS_8BIT)
	{
		*pDeclen = nSrcLen;

        *ppszDec = malloc(*pDeclen+1);

        if(*ppszDec == NULL)
            return FALSE;

        strcpy(*ppszDec,pszSrc);
	}
	
    return TRUE;
}
/*********************************************************************\
* Function	   SMS_IsUserCancel
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_IsUserCancel(HWND hWnd,UINT uMsgCmd)
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, 1))
    {	
        printf("\r\n PeekMessage message = %d ,hwnd = %d \r\n",msg.message,msg.hwnd);
        if(msg.message == WM_KEYDOWN && msg.wParam == VK_F10)
			return TRUE;

        TranslateMessage(&msg);
        DispatchMessage(&msg);		
    }

	return FALSE;
}
/*********************************************************************\
* Function	   SMS_GetSIMCtrlWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
HWND SMS_GetSIMCtrlWnd(void)
{
    return hSIMCtlWnd;
}
/*********************************************************************\
* Function	   SMS_NewSIMRecord
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_NewSIMRecord(PSMS_INFO pInfo)
{
    PSMS_SIMCREATEDATA pData;
    SMS_SIMCHAIN* pNewNode = NULL;
    int insert = -1;
    PSMS_INFO pNewInfo = NULL;
    char szTimestamp[20],szTemp[20],szContext[256];
    int nLen,i;
    SYSTEMTIME sy,syLocal;
    HWND hLst;
    DWORD dwDateTime;
    char* pszContent = NULL;

    if(IsWindow(hSIMWnd))
    {
        pData = GetUserData(hSIMWnd);

        hLst = GetDlgItem(hSIMWnd,IDC_SIMLSIT_LIST);
        
        if(pData->bOK)
        {
            pNewNode = SIM_New();
            
            if(pNewNode == NULL)
                return;
            
            pNewNode->nMemType = SMS_MEM_MT;
            
            for( i=0 ; i<pData->nmaxcount ; i++ )
            {
                if((pData->psmsinfo)[i].Index == SMS_SIMINDEX_NULL)
                {
                    memcpy(&((pData->psmsinfo)[i]),pInfo,sizeof(SMS_INFO));
                    pNewInfo = &((pData->psmsinfo)[i]);
                    break;
                }
            }

            if(pNewInfo == FALSE)
                return;

            pNewNode->psmsinfo = pNewInfo;
            
            insert = SIM_Inset(&(pData->pUnreadHeader),pNewNode);
            
            if(insert != -1)
            {   
                pData->ncount++;
                
                if(IsWindowVisible(hLst) == FALSE)
                    ShowWindow(hLst,SW_SHOW);
                
                szTimestamp[0] = 0;
                
                memset(&syLocal,0,sizeof(SYSTEMTIME));
                
                GetLocalTime(&syLocal);            
                
                dwDateTime = String2DWORD(pNewNode->psmsinfo->SendTime);
                
                SendMessage(hLst, LB_INSERTSTRING, insert, (LPARAM)pNewNode->psmsinfo->SenderNum);
                
                memset(&sy,0,sizeof(SYSTEMTIME));
                sy.wYear = (WORD)INTYEAR(dwDateTime);
                sy.wMonth = (WORD)INTMONTH(dwDateTime);
                sy.wDay = (WORD)INTDAY(dwDateTime);
                sy.wHour = (WORD)INTHOUR(dwDateTime);
                sy.wMinute = (WORD)INTMINUTE(dwDateTime);
                sy.wSecond = (WORD)INTSECOND(dwDateTime);
                
                if(sy.wYear != syLocal.wYear)
                    sprintf(szTimestamp,"%d",sy.wYear);
                else if(sy.wMonth != syLocal.wMonth || sy.wDay != syLocal.wDay)
                {
                    sy.wYear = 0;
                    GetTimeDisplay(sy,szTemp,szTimestamp);
                }
                else
                    GetTimeDisplay(sy,szTimestamp,szTemp);
                
                SMS_ParseContentEx(pNewNode->psmsinfo->dcs, pNewNode->psmsinfo->Context , pNewNode->psmsinfo->ConLen ,
                    &pszContent,&nLen);
                
                szContext[0] = 0;
                
                sprintf(szContext,"%s %s",szTimestamp,pszContent);
                
                SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(insert, -1), (LPARAM)szContext);
                
                SendMessage(hLst, LB_SETITEMDATA, insert, (LPARAM)pNewNode);
                
                SendMessage(hLst, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, insert), (LPARAM)pData->hBitmapUnread);

                SendMessage(hLst, LB_SETCURSEL, insert, NULL);
                
                SMS_FREE(pszContent);
            }
        }
    }

    return ;
}
