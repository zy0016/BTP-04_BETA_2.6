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
#include "ABGlobal.h"

#define IDM_ABSIM_EDIT              101
#define IDM_ABSIM_DELETE            102
#define IDM_ABSIM_COPYTOCONTACTS    104
#define IDM_ABSIM_SIMINFO           105

#define IDM_ABSIM_DELSELECT         1031
#define IDM_ABSIM_DELALL            1032

#define IDM_ABSIM_COPYTOCONTACTSELECT   1041
#define IDM_ABSIM_COPYTOCONTACTALL      1042


#define IDC_OPEN                    100
#define IDC_EXIT                    200
#define IDC_ABSIM_LIST              300

typedef struct tagABSIM_Data
{
	HWND		hFrameWnd;
	HMENU		hMenu;
    PVOID       handle;
    DWORD       *pId;
    int         nCount;
    HBITMAP     hNewSIM;
    HBITMAP     hSIMItem;
    WNDPROC     OldListWndProc;
}ABSIM_DATA,*PABSIM_DATA;

BOOL AB_CreateSIMWnd(HWND hWnd);
LRESULT ABSIMWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABSIM_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABSIM_OnActivate(HWND hwnd, UINT state);
static void ABSIM_OnSetFocus(HWND hwnd);
static void ABSIM_OnPaint(HWND hWnd);
static void ABSIM_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABSIM_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABSIM_OnDestroy(HWND hWnd);
static void ABSIM_OnClose(HWND hWnd);
static void ABSIM_OnDataChange(HWND hWnd,PPHONEBOOK pPB,int nMode);
static void ABSIM_OnSIMDeleted(HWND hWnd,BOOL bDelete);
static void ABSIM_OnDeleteAll(HWND hWnd,BOOL bDeleteAll);
static void ABSIM_OnDeleteSelect(HWND hWnd,BOOL bDel,int nCount,DWORD* pnId);
static void ABSIM_OnSureDeleteSel(HWND hWnd,BOOL bDeleteSel);
static void ABSIM_OnCopySelect(HWND hWnd,BOOL bCopy,int nCount,DWORD* pnId);
static void ABSIM_OnCopyAll(HWND hWnd,BOOL bCopy);
static BOOL AB_CopyFromSIM(PPHONEBOOK pPB,CONTACT_ITEMCHAIN **ppItem,int* nTelID);

static LRESULT CallListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
BOOL AB_IsIndexUsed(int nIDTemp);
int AB_FindUseableIndex(void);
PPHONEBOOK AB_FindUseableSIMPB(void);



#define IDC_OK              100
#define IDC_ABSIMINFO_LIST  300

BOOL AB_CreateSIMInfoWnd(HWND hWnd);
LRESULT ABSIMInfoWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABSIMInfo_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABSIMInfo_OnActivate(HWND hwnd, UINT state);
static void ABSIMInfo_OnSetFocus(HWND hwnd);
static void ABSIMInfo_OnPaint(HWND hWnd);
static void ABSIMInfo_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABSIMInfo_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABSIMInfo_OnDestroy(HWND hWnd);
static void ABSIMInfo_OnClose(HWND hWnd);

#define IDC_SAVE            100
#define IDC_BACK            200
#define IDC_SIMEDIT_TEXT    301
#define IDC_SIMEDIT_NUMBER  302

BOOL AB_CreateSIMEditWnd(HWND hWnd,PPHONEBOOK pPhoneBook,char* pszFocus,BOOL bNew);
LRESULT ABSIMEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABSIMEdit_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABSIMEdit_OnActivate(HWND hwnd, UINT state);
static void ABSIMEdit_OnSetFocus(HWND hwnd);
static void ABSIMEdit_OnPaint(HWND hWnd);
static void ABSIMEdit_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABSIMEdit_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABSIMEdit_OnDestroy(HWND hWnd);
static void ABSIMEdit_OnClose(HWND hWnd);
static void ABSIMEdit_OnSave(HWND hWnd,BOOL bSave);


BOOL AB_CreateViewWnd(HWND hWnd,PPHONEBOOK pPhoneBook);
LRESULT ABSIMViewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABSIMView_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABSIMView_OnActivate(HWND hwnd, UINT state);
static void ABSIMView_OnSetFocus(HWND hwnd);
static void ABSIMView_OnPaint(HWND hWnd);
static void ABSIMView_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static LRESULT ABSIMView_OnCommand(HWND hWnd, int id, UINT codeNotify,LPARAM lParam);
static void ABSIMView_OnDestroy(HWND hWnd);
static void ABSIMView_OnClose(HWND hWnd);
static void ABSIMView_OnDataChange(HWND hWnd,PPHONEBOOK pPB,int nMode);
static void ABSIM_OnRefreshList(HWND hWnd,char* pszSearch,int nLen,BOOL bExit);

BOOL AB_CreateSIMWnd(HWND hFrameWnd)
{
    WNDCLASS    wc;
    HMENU       hDelMenu,hCopytoContactMenu;
    ABSIM_DATA  Data;
    HWND        hABSIMWnd;
	RECT		rcClient;
    
    wc.style         = 0;
    wc.lpfnWndProc   = ABSIMWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(ABSIM_DATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABSIMWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;
     
    memset(&Data,0,sizeof(ABSIM_DATA));
    
	GetClientRect(hFrameWnd,&rcClient);

    Data.hFrameWnd = hFrameWnd;
    Data.hMenu = CreateMenu();

    hABSIMWnd = CreateWindow(
        "ABSIMWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
        hFrameWnd,
        (HMENU)IDC_AB_SIM, 
        NULL, 
        (PVOID)&Data
        );
    
    if (!hABSIMWnd)
    {
		DestroyMenu(Data.hMenu);
        UnregisterClass("ABSIMWndClass",NULL);
        return FALSE;
    }

    AppendMenu(Data.hMenu, MF_ENABLED, IDC_OPEN, IDS_OPEN);
    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABSIM_EDIT, IDS_EDIT);

    hCopytoContactMenu = CreateMenu();
    AppendMenu(hCopytoContactMenu, MF_ENABLED, IDM_ABSIM_COPYTOCONTACTSELECT, IDS_SELECT);
    AppendMenu(hCopytoContactMenu, MF_ENABLED, IDM_ABSIM_COPYTOCONTACTALL, IDS_ALL);

    AppendMenu(Data.hMenu, MF_POPUP|MF_ENABLED, (DWORD)hCopytoContactMenu, IDS_COPYTOCONTACTS);

    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABSIM_SIMINFO, IDS_SIMINFO);

	AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABSIM_DELETE, IDS_DELETE);
    
    hDelMenu = CreateMenu();
    AppendMenu(hDelMenu, MF_ENABLED, IDM_ABSIM_DELSELECT, IDS_SELECT);
    AppendMenu(hDelMenu, MF_ENABLED, IDM_ABSIM_DELALL, IDS_ALL);
    
    AppendMenu(Data.hMenu, MF_POPUP|MF_ENABLED, (DWORD)hDelMenu, IDS_DELETEMANY);
	
	SetFocus(hABSIMWnd);

	PDASetMenu(hFrameWnd,Data.hMenu);
    
    if(AB_GetFlag())
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT, 0), (LPARAM)IDS_EXIT);
    else
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT, 0), (LPARAM)IDS_BACK);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OPEN, 1), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
   
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

	SetWindowText(hFrameWnd, IDS_SIM);

    ShowWindow(hABSIMWnd,SW_SHOW);
    UpdateWindow(hABSIMWnd);

    return TRUE;
}

LRESULT ABSIMWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABSIM_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        ABSIM_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABSIM_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABSIM_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABSIM_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABSIM_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABSIM_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABSIM_OnDestroy(hWnd);
        break;

    case WM_DATACHANGE:
        ABSIM_OnDataChange(hWnd,(PPHONEBOOK)wParam,(int)lParam);
        break;

    case WM_SIMDELETED:
        ABSIM_OnSIMDeleted(hWnd,(BOOL)lParam);
        break;

    case AB_MSG_REFRESHLIST:
        ABSIM_OnRefreshList(hWnd,(char*)wParam,(int)LOWORD(lParam),(BOOL)HIWORD(lParam));
        break;
        
    case WM_DELETEALL:
        ABSIM_OnDeleteAll(hWnd,(BOOL)lParam);
        break;
        
	case WM_DELSELECT:
        ABSIM_OnDeleteSelect(hWnd,(BOOL)LOWORD(wParam),(int)HIWORD(wParam),(DWORD*)lParam);
		break;

    case WM_SUREELETESEL:
        ABSIM_OnSureDeleteSel(hWnd,(BOOL)lParam);
        break;

    case WM_COPYSELECT:
        ABSIM_OnCopySelect(hWnd,(BOOL)LOWORD(wParam),(int)HIWORD(wParam),(DWORD*)lParam);
        break;

    case WM_COPYALL:
        ABSIM_OnCopyAll(hWnd,(BOOL)lParam);
        break;


    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABSIM_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABSIM_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    ABSIM_DATA *pData;
    HWND hList;
    PCONTACT_INITDATA pPhoneBook;
    int i,j,nScan;
    PPHONEBOOK pPB;
    
    pPhoneBook = AB_GetSIMData();
    
    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(ABSIM_DATA));
    
    GetClientRect(hWnd,&rect);
    
    hList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_ABSIM_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;

    pData->hNewSIM = LoadImage(NULL, AB_BMP_NEWSIM, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);
    
    pData->hSIMItem = LoadImage(NULL, AB_BMP_SIMITEM, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);

    SendMessage(hList, LB_INSERTSTRING, 0, (LPARAM)IDS_NEWSIMCONTACT);
    SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hNewSIM);

    for(i = 0 ,nScan = 0; i <pPhoneBook->nTotalNum ; i++)
    {
        if(nScan == pPhoneBook->nNumber)
            break;

        if(pPhoneBook->pSIMPhoneBook[i].Index == 0)
            continue;

        for(j = 1 ; j < nScan+1 ; j++)
        {
            pPB = (PPHONEBOOK)SendMessage(hList, LB_GETITEMDATA,j,0);
            if(stricmp(pPB->Text,pPhoneBook->pSIMPhoneBook[i].Text) > 0)
                break;
        }
        SendMessage(hList, LB_INSERTSTRING, j, (LPARAM)pPhoneBook->pSIMPhoneBook[i].Text);
        SendMessage(hList, LB_SETITEMDATA, j, (LPARAM)&(pPhoneBook->pSIMPhoneBook[i]));
        SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, j),(LPARAM)pData->hSIMItem);

        nScan++;

    }
   
    SendMessage(hList, LB_SETCURSEL, 0, 0);

    pData->OldListWndProc = (WNDPROC)SetWindowLong(hList,GWL_WNDPROC,(LONG)CallListWndProc);

    pData->handle = AB_RegisterNotify(hWnd,WM_DATACHANGE,AB_OBJECT_ALL,AB_MDU_SIMCONTRACT);

    return TRUE;
    
}
/*********************************************************************\
* Function	ABSIM_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSIM_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;
    ABSIM_DATA *pData;
    int  nIndex;

    pData = GetUserData(hwnd);

    hLst = GetDlgItem(hwnd,IDC_ABSIM_LIST);

	SetFocus(hLst);

	PDASetMenu(pData->hFrameWnd,pData->hMenu);
    
    nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
    
    if(nIndex == 0)
    {
        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
    }
    else      
    {
        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
    }

    if(AB_GetFlag())
        SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
    else
        SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
   
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

	SetWindowText(pData->hFrameWnd, IDS_SIM);

    return;
}
/*********************************************************************\
* Function	ABSIM_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSIM_OnSetFocus(HWND hwnd)
{
    HWND hLst;

    hLst = GetDlgItem(hwnd,IDC_ABSIM_LIST);

	SetFocus(hLst);

    return;
}
/*********************************************************************\
* Function	ABSIM_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSIM_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABSIM_OnKey
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
static void ABSIM_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    ABSIM_DATA* pData;
    HWND hLst;
    int  nIndex;
    PPHONEBOOK pPB;

    pData = GetUserData(hWnd);

    switch (vk)
	{
    case VK_RETURN:
		SendMessage(hWnd,WM_COMMAND,IDC_OPEN,NULL);
        break;

	case  VK_F5:
        hLst = GetDlgItem(hWnd,IDC_ABSIM_LIST);
        nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);        
        switch(nIndex)
        {
        case LB_ERR:
            break;
            
/*
        case 0:// Add member
            break;*/
        case 0:   
            {
                PPHONEBOOK pPB;
                
                pPB = AB_FindUseableSIMPB();

                if(pPB == NULL)
                {
                    PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);//
                    break;
                }

                pPB->Index = AB_FindUseableIndex();
                
                AB_CreateSIMEditWnd(pData->hFrameWnd,pPB,pPB->Text,TRUE);
            }
            break;
            
        default:
            PDADefWindowProc(pData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
            break;
        }     
		break;
	case VK_F10:
        if(AB_GetFlag())
            AB_ExitApp(pData->hFrameWnd);
        else
            PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

    case VK_F1://Send dial
        hLst = GetDlgItem(hWnd,IDC_ABSIM_LIST);
        nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
        
        if(nIndex == LB_ERR || nIndex == 0)
            break;
        pPB = (PPHONEBOOK)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
        if(strlen(pPB->PhoneNum) == 0)
            break;
        else 
            APP_CallPhoneNumber(pPB->PhoneNum);
        break;
        
    case VK_1:
    case VK_2:
    case VK_3:
    case VK_4:
    case VK_5:
    case VK_6:
    case VK_7:
    case VK_8:
    case VK_9:
    case VK_0:
    case VK_F3:
    case VK_F4:
        AB_CreateSearchPopUpWnd(pData->hFrameWnd,hWnd,AB_MSG_REFRESHLIST,FALSE);
        keybd_event(vk,0,0,0);
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABSIM_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABSIM_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    ABSIM_DATA* pData;
    HWND hLst;
    int  nIndex;
    
    hLst = GetDlgItem(hWnd,IDC_ABSIM_LIST);
    
    pData = GetUserData(hWnd);

	switch(id)
	{
    case IDC_ABSIM_LIST:
        if(codeNotify == LBN_SELCHANGE)
        {
            char    szButText[20];
            char    szMenu[20];

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(nIndex == LB_ERR)
                break;
            
            memset(szButText,0,20);
            memset(szMenu,0,20);

            SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szButText);
            SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 2, (LPARAM)szMenu);

            if(nIndex == 0)
            {  
                if(strcmp(szButText,IDS_OPEN) == 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
                
                if(strcmp(szMenu,ICON_SELECT) != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
            }
            else
            {
                if(strcmp(szButText,IDS_OPEN) != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
                
                if(strcmp(szMenu,ICON_OPTIONS) != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
            }
        }
        break;

    case IDC_OPEN:
        {
            PPHONEBOOK pPB;
            PCONTACT_INITDATA pInit;
            
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(nIndex == LB_ERR || nIndex == 0)
                break;

            pInit = AB_GetSIMData();

            pPB = (PPHONEBOOK)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
            AB_CreateViewWnd(pData->hFrameWnd,pPB);
        }
        break;

    case IDM_ABSIM_EDIT:
        {
            PPHONEBOOK pPB;
            PCONTACT_INITDATA pInit;
            
            pInit = AB_GetSIMData();

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(nIndex == LB_ERR)
                break;

            pPB = (PPHONEBOOK)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
            AB_CreateSIMEditWnd(pData->hFrameWnd,pPB,pPB->Text,FALSE);
        }
        break;

    case IDM_ABSIM_DELETE:
        {
            PPHONEBOOK pPB;
            char szPrompt[50];
            char szCaption[50];
            HDC hdc;

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(nIndex == LB_ERR)
                break;

            pPB = (PPHONEBOOK)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
            szPrompt[0] = 0;

            hdc = GetDC(hWnd);
            
            GetExtentFittedText(hdc,pPB->Text,-1,szCaption,50,TEXT_WIDTH,OMIT_SUFFIX,SUFFIX_REPEAT);
            
            ReleaseDC(hWnd,hdc);

            sprintf(szPrompt,"%s:\r\n%s?",szCaption,IDS_DELETE);
            
            PLXConfirmWinEx(pData->hFrameWnd,hWnd,szPrompt, Notify_Request, pPB->Text, IDS_YES, IDS_NO,WM_SIMDELETED);
        }
        break;
         
    case IDM_ABSIM_COPYTOCONTACTS:
        AB_PleaseToWait();
        break;

    case IDM_ABSIM_SIMINFO:
        AB_CreateSIMInfoWnd(pData->hFrameWnd);
        break;

    case IDM_ABSIM_COPYTOCONTACTSELECT:
        {
            PCONTACT_INITDATA pInit;
            
            pInit = AB_GetSIMData();
            
            pData->nCount = pInit->nTotalNum;

			pData->pId = (DWORD*)malloc(pData->nCount*sizeof(DWORD));

			if(pData->pId == NULL)
				break;
			
			memset(pData->pId,0,pData->nCount*sizeof(DWORD));
			
			ABCreateMultiPickerWnd(pData->hFrameWnd,hWnd,WM_COPYSELECT,(char*)IDS_SELECTCONTACT/*IDS_COPYTOCONTACTS*/,pData->nCount,pData->pId,-2,IDS_COPY,IDS_CANCEL);
        }
        break;

    case IDM_ABSIM_COPYTOCONTACTALL:
        {

            PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_COPYALLTOCONTACTS, Notify_Request, IDS_SIM, IDS_YES, IDS_NO,WM_COPYALL);

        }
        break;

    case IDM_ABSIM_DELALL:
        {
            char szTitle[50];

            GetWindowText(pData->hFrameWnd,szTitle,49);

            szTitle[49] = 0;

            PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_DELETEALL, Notify_Request, szTitle, IDS_YES, IDS_NO,WM_DELETEALL);
        }
        break;

    case IDM_ABSIM_DELSELECT:
		{		
            PCONTACT_INITDATA pInit;
            
            pInit = AB_GetSIMData();
            
            pData->nCount = pInit->nTotalNum;

			pData->pId = (DWORD*)malloc(pData->nCount*sizeof(DWORD));

			if(pData->pId == NULL)
				break;
			
			memset(pData->pId,0,pData->nCount*sizeof(DWORD));
			
			ABCreateMultiPickerWnd(pData->hFrameWnd,hWnd,WM_DELSELECT,(char*)IDS_SELECTCONTACT,pData->nCount,pData->pId,-2,IDS_DELETE,IDS_CANCEL);
		}
        break;

    default:
        break;
    }
}
/*********************************************************************\
* Function	ABSIM_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSIM_OnDestroy(HWND hWnd)
{
    ABSIM_DATA* pData;

    pData = GetUserData(hWnd);
	
    if(pData->hNewSIM)
        DeleteObject(pData->hNewSIM);

    if(pData->hSIMItem)
        DeleteObject(pData->hSIMItem);
    
	DestroyMenu(pData->hMenu);

    AB_UnRegisterNotify(pData->handle);
    
	UnregisterClass("ABSIMWndClass",NULL);

    return;

}
/*********************************************************************\
* Function	ABSIM_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSIM_OnClose(HWND hWnd)
{
    ABSIM_DATA* pData;

    pData = GetUserData(hWnd);
	
	SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow(hWnd);

    return;
}
/*********************************************************************\
* Function	ABSIM_OnSIMDeleted
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABSIM_OnSIMDeleted(HWND hWnd,BOOL bDelete)
{
    PHONEBOOK PBTemp;
    
    PCONTACT_INITDATA pInit;
    
    MSG myMsg;
    
    HWND hFindWnd;
    
    char szMenu[20];
    
    int  nCurSel;
    
    PPHONEBOOK pPB;

    int  nIndex;

    HWND hLst;

    ABSIM_DATA* pData;

    pData = GetUserData(hWnd);

    if(bDelete == FALSE)
        return;

    hLst = GetDlgItem(hWnd,IDC_ABSIM_LIST);
    
    nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
    
    if(nIndex == LB_ERR)
        return;
    
    pPB = (PPHONEBOOK)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
    
    hFindWnd = GetDlgItem(pData->hFrameWnd,IDC_AB_SIMVIEW);
    
    pInit = AB_GetSIMData();
    
    memset(&PBTemp,0,sizeof(PHONEBOOK));
    
    PBTemp.Index = pPB->Index;
    
    WaitWindowStateEx(pData->hFrameWnd,TRUE,IDS_DELETING,IDS_SIM,NULL,NULL);
    
    if (ME_WritePhonebook(pInit->hCtrlWnd, AB_MSG_SIM_WRITE, PBOOK_SM, &PBTemp) == -1)
    {
        WaitWindowStateEx(pData->hFrameWnd,FALSE,IDS_DELETING,IDS_SIM,NULL,NULL);
        
        return;
    }
    
    memset(szMenu,0,20);
    
    while(GetMessage(&myMsg, NULL, 0, 0)) 
    {
        if((myMsg.message == AB_MSG_SIM_WRITE_SUCC) && (myMsg.hwnd == pInit->hCtrlWnd))
        {
            WaitWindowStateEx(pData->hFrameWnd,FALSE,IDS_DELETING,IDS_SIM,NULL,NULL);
            
            PLXTipsWin(NULL,NULL,0,IDS_DELETED,IDS_SIM,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);

            memset(pPB,0,sizeof(PHONEBOOK));
            SendMessage(hLst,LB_DELETESTRING,nIndex,0);
            if(nIndex == pInit->nNumber)
                SendMessage(hLst, LB_SETCURSEL, nIndex-1, 0);
            else
                SendMessage(hLst, LB_SETCURSEL, nIndex, 0);
            pInit->nNumber--;
            
            memset(szMenu,0,20);
            
            SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
            
            nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nCurSel == 0)
            {
                if(strcmp(szMenu,ICON_SELECT) != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
            }
            else
            {
                if(strcmp(szMenu,ICON_OPTIONS) != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
            }
            
            PostMessage(hFindWnd,WM_CLOSE,0,0);
            break;
        }
        else if((myMsg.message == AB_MSG_SIM_WRITE_FAIL) && (myMsg.hwnd == pInit->hCtrlWnd))
        {
            WaitWindowStateEx(pData->hFrameWnd,FALSE,IDS_DELETING,IDS_SIM,NULL,NULL);
            break;
        }
        else if((myMsg.message == AB_MSG_SIM_WRITE_FULL) && (myMsg.hwnd == pInit->hCtrlWnd))
        {
            WaitWindowStateEx(pData->hFrameWnd,FALSE,IDS_DELETING,IDS_SIM,NULL,NULL);
            PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);//
            break;
        }
        TranslateMessage(&myMsg);
        DispatchMessage(&myMsg);
    }
    
}
/*********************************************************************\
* Function	   ABSIM_OnDeleteAll
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABSIM_OnDeleteAll(HWND hWnd,BOOL bDeleteAll)
{
    char szTitle[50];
    PHONEBOOK PBTemp;
    PCONTACT_INITDATA pInit;
    MSG myMsg;
    char szMenu[20];
    int  nCurSel;
    PPHONEBOOK pPB;
    HWND hLst;
    int nCount,i;
    ABSIM_DATA* pData;
	BOOL bSuccess = TRUE;

    pData = GetUserData(hWnd);

    if(bDeleteAll == FALSE)
        return;

    hLst = GetDlgItem(hWnd,IDC_ABSIM_LIST);
    
    nCount = SendMessage(hLst,LB_GETCOUNT,0,0);
    
    pInit = AB_GetSIMData();  
    
    WaitWindowStateEx(pData->hFrameWnd,TRUE,IDS_DELETING,IDS_SIM,NULL,NULL);
        
    for(i = nCount - 1 ; i >= 1 ; i--)
    {
        pPB = (PPHONEBOOK)SendMessage(hLst,LB_GETITEMDATA,i,0);
        
        memset(&PBTemp,0,sizeof(PHONEBOOK));
        
        PBTemp.Index = pPB->Index;
                
        if (ME_WritePhonebook(pInit->hCtrlWnd, AB_MSG_SIM_WRITE, PBOOK_SM, &PBTemp) == -1)
        {
            WaitWindowStateEx(pData->hFrameWnd,FALSE,IDS_DELETING,IDS_SIM,NULL,NULL);
            
            return;
        }
        
        memset(szMenu,0,20);
        
        while(GetMessage(&myMsg, NULL, 0, 0)) 
        {
            if((myMsg.message == AB_MSG_SIM_WRITE_SUCC) && (myMsg.hwnd == pInit->hCtrlWnd))
            {
                memset(pPB,0,sizeof(PHONEBOOK));
                SendMessage(hLst,LB_DELETESTRING,i,0);
                if(i == pInit->nNumber)
                    SendMessage(hLst, LB_SETCURSEL, i-1, 0);
                else
                    SendMessage(hLst, LB_SETCURSEL, i, 0);
                pInit->nNumber--;
                
                memset(szMenu,0,20);
                
                SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
                
                nCurSel = SendMessage(hLst,LB_GETCURSEL,0,0);
                
                if(nCurSel == 0)
                {
                    if(strcmp(szMenu,ICON_SELECT) != 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
                }
                else
                {
                    if(strcmp(szMenu,ICON_OPTIONS) != 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
                }
                
                break;
            }
            else if((myMsg.message == AB_MSG_SIM_WRITE_FAIL) && (myMsg.hwnd == pInit->hCtrlWnd))
            {
				bSuccess = FALSE;
                break;
            }
            else if((myMsg.message == AB_MSG_SIM_WRITE_FULL) && (myMsg.hwnd == pInit->hCtrlWnd))
            {
                PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);//
                bSuccess = FALSE;
				break;
            }
            TranslateMessage(&myMsg);
            DispatchMessage(&myMsg);
        }
    }

    WaitWindowStateEx(pData->hFrameWnd,FALSE,IDS_DELETING,IDS_SIM,NULL,NULL);
                
    GetWindowText(pData->hFrameWnd,szTitle,49);

    szTitle[49] = 0;

	if(bSuccess)
		PLXTipsWin(NULL,NULL,0,IDS_DELETED, szTitle, Notify_Success, IDS_OK, NULL,WAITTIMEOUT);  
	else
		PLXTipsWin(NULL,NULL,0,IDS_FAILED, szTitle, Notify_Failure, IDS_OK, NULL,WAITTIMEOUT); 
	
}

/*********************************************************************\
* Function	   ABSIM_OnDeleteSelect
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABSIM_OnDeleteSelect(HWND hWnd,BOOL bDel,int nCount,DWORD* pnId)
{
    char szTitle[50];
    char szPrompt[125];
    ABSIM_DATA* pData;
    char szCaption[50];
    HDC hdc;

    pData = GetUserData(hWnd);

	if(bDel == FALSE || nCount == 0)
	{
		AB_FREE(pData->pId);

        pData->nCount = 0;

		return;
	}

    pData->nCount = nCount;

    if(nCount == 1)
    {    
        szPrompt[0] = 0;
        
        hdc = GetDC(hWnd);
        
        GetExtentFittedText(hdc,((PPHONEBOOK)pnId[0])->Text,-1,szCaption,50,TEXT_WIDTH,OMIT_SUFFIX,SUFFIX_REPEAT);
        
        ReleaseDC(hWnd,hdc);
        
        sprintf(szPrompt,"%s:\r\n%s?",szCaption,IDS_DELETE);
        
        PLXConfirmWinEx(pData->hFrameWnd,hWnd,szPrompt, Notify_Request, ((PPHONEBOOK)pnId[0])->Text, IDS_YES, IDS_NO,WM_SUREELETESEL);    
        
        return;   
    }
    else
    {
        GetWindowText(pData->hFrameWnd,szTitle,49);
        
        szTitle[49] = 0;
        
        PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_DELETESELECT, Notify_Request, szTitle, IDS_YES, IDS_NO,WM_SUREELETESEL);   
    } 
}

/*********************************************************************\
* Function	   ABSIM_OnSureDeleteSel
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABSIM_OnSureDeleteSel(HWND hWnd,BOOL bDel)
{
	int i,j,nScan;
	HWND hList;
    char szTitle[50];
    ABSIM_DATA* pData;
    PHONEBOOK PBTemp,*pPB;
    PCONTACT_INITDATA pInit;
    MSG myMsg;
    char szMenu[20];
    char szLeft[20];
    char szName[50];
    
    pInit = AB_GetSIMData();  

    pData = GetUserData(hWnd);

    if(bDel == FALSE)
    {
        AB_FREE(pData->pId);

        pData->nCount = 0;

        return;
    }

    hList = GetDlgItem(hWnd,IDC_ABSIM_LIST);

    if(pData->nCount == 1)
    {
        szName[0] = 0;
        
        strcpy(szName,((PHONEBOOK*)pData->pId[0])->Text);
    }

    WaitWindowStateEx(pData->hFrameWnd,TRUE,IDS_DELETING,IDS_SIM,NULL,NULL);

	for(i = pData->nCount-1 ; i >= 0 ; i--)
	{        
        memset(&PBTemp,0,sizeof(PHONEBOOK));
        
        PBTemp.Index = ((PHONEBOOK*)pData->pId[i])->Index;
        
        
        if (ME_WritePhonebook(pInit->hCtrlWnd, AB_MSG_SIM_WRITE, PBOOK_SM, &PBTemp) == -1)
        {
            WaitWindowStateEx(pData->hFrameWnd,FALSE,IDS_DELETING,IDS_SIM,NULL,NULL);
            
            return;
        }
        
        memset(szMenu,0,20);
        
        while(GetMessage(&myMsg, NULL, 0, 0)) 
        {
            if((myMsg.message == AB_MSG_SIM_WRITE_SUCC) && (myMsg.hwnd == pInit->hCtrlWnd))
            {                
                memset((PHONEBOOK*)pData->pId[i],0,sizeof(PHONEBOOK));
     
                pInit->nNumber--;
                                
                break;
            }
            else if((myMsg.message == AB_MSG_SIM_WRITE_FAIL) && (myMsg.hwnd == pInit->hCtrlWnd))
            {
                break;
            }
            else if((myMsg.message == AB_MSG_SIM_WRITE_FULL) && (myMsg.hwnd == pInit->hCtrlWnd))
            {
                PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);//
                break;
            }
            TranslateMessage(&myMsg);
            DispatchMessage(&myMsg);
        }
	}

    WaitWindowStateEx(pData->hFrameWnd,FALSE,IDS_DELETING,IDS_SIM,NULL,NULL);
    
    GetWindowText(pData->hFrameWnd,szTitle,49);

    if(pInit->nNumber == 0)
    {
        SendMessage(hList,LB_RESETCONTENT,0,0);
        
        SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_NEWSIMCONTACT);
        SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hNewSIM);
        
        SendMessage(hList,LB_SETCURSEL,0,0);
        
        SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
        if(strcmp(szMenu,ICON_SELECT) != 0)
            SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
        
        SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
        if(strcmp(szLeft,"") != 0)
            SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");        
    }
    else
    {
        SendMessage(hList,LB_RESETCONTENT,0,0);    
        
        SendMessage(hList, LB_INSERTSTRING,0,(LPARAM)IDS_NEWSIMCONTACT);
        SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hNewSIM);

        for(i = 0 ,nScan = 0; i <pInit->nTotalNum ; i++)
        {
            if(nScan == pInit->nNumber)
                break;

            if(pInit->pSIMPhoneBook[i].Index == 0)
                continue;
            
            for(j = 1 ; j < nScan+1 ; j++)
            {
                pPB = (PPHONEBOOK)SendMessage(hList, LB_GETITEMDATA,j,0);
                if(stricmp(pPB->Text,pInit->pSIMPhoneBook[i].Text) > 0)
                    break;
            }
            SendMessage(hList, LB_INSERTSTRING, j, (LPARAM)pInit->pSIMPhoneBook[i].Text);
            SendMessage(hList, LB_SETITEMDATA, j, (LPARAM)&(pInit->pSIMPhoneBook[i]));
            SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, j),(LPARAM)pData->hSIMItem);
            
            nScan++;
        }
        
        SendMessage(hList, LB_SETCURSEL,0,0);     
        
        SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
        if(strcmp(szMenu,ICON_SELECT) != 0)
            SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
        
        SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
        if(strcmp(szLeft,"") != 0)
            SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
    }

    szTitle[49] = 0;
    
    if(pData->nCount == 1)
        PLXTipsWin(NULL,NULL,0,IDS_DELETED,szTitle,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    else
        PLXTipsWin(NULL,NULL,0,IDS_DELETED,szTitle,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    
    AB_FREE(pData->pId);
    
    pData->nCount = 0;
}
/*********************************************************************\
* Function	   ABSIM_OnCopySelect
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABSIM_OnCopySelect(HWND hWnd,BOOL bCopy,int nCount,DWORD* pnId)
{
    int  i;
    ABSIM_DATA* pData;
    CONTACT_ITEMCHAIN* pItem;
    DWORD dwoffset;
    DWORD id;
    int   nGroup;
    int   nTelID;
            
    pData = GetUserData(hWnd);

	if(bCopy == FALSE || nCount == 0)
	{
		AB_FREE(pData->pId);

        pData->nCount = 0;

		return;
	}

    WaitWin(hWnd,TRUE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);

    pData->nCount = nCount;

	for(i = pData->nCount-1; i >= 0 ; i--)
	{
        if(AB_IsUserCancel(hWnd,WM_CANCELCOPYFROMSIM))
        {
            WaitWin(hWnd,FALSE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);
            
            AB_FREE(pData->pId);
            
            pData->nCount = 0;
            
            return;
        }

        if(AB_IsFlashEnough() == FALSE)
        {
            WaitWin(hWnd,FALSE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);
            
            PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_SIM,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
            
            AB_FREE(pData->pId);
            
            pData->nCount = 0;
            
            return;
        }

        pItem = NULL;
        
        dwoffset = -1;
        
        id = -1;

        nGroup = -1;
        
        if(AB_CopyFromSIM((PPHONEBOOK)(pData->pId[i]),&pItem,&nTelID))
        {
            if(AB_SaveRecord(&dwoffset,&id,&nGroup,FALSE,nTelID,pItem))
                AB_Insert2Table(dwoffset,id,nGroup,pItem);
            
            Item_Erase(pItem);
        }
        else
        {
            Item_Erase(pItem);

            WaitWin(hWnd,FALSE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);
            
            PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_SIM,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
            
            AB_FREE(pData->pId);
            
            pData->nCount = 0;
            
            return;
        }

	}
    
    WaitWin(hWnd,FALSE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);     
    
    PLXTipsWin(NULL,NULL,0,IDS_COPIEDTOCONTACTS,IDS_SIM,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    
    AB_FREE(pData->pId);
    
    pData->nCount = 0;
}
/*********************************************************************\
* Function	   AB_CopyFromSIM
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL AB_CopyFromSIM(PPHONEBOOK pPB,CONTACT_ITEMCHAIN **ppItem,int* nTelID)
{
    if(strlen(pPB->PhoneNum) == 0 && strlen(pPB->Text) == 0)
        return FALSE;

    *ppItem = Item_New(AB_TYPEMASK_LASTNAME);
    
    if(*ppItem == NULL)                
        return FALSE;
    
    (*ppItem)->dwMask = AB_TYPEMASK_LASTNAME;
    (*ppItem)->bChange = FALSE;
    (*ppItem)->nMaxLen = AB_MAXLEN_FIRSTNAME;
    if(strlen(pPB->Text) != 0)
        strcpy((*ppItem)->pszData,pPB->Text);
    else
        strcpy((*ppItem)->pszData,pPB->PhoneNum);
    
    (*ppItem)->nID = 1;

    *nTelID = 1;

    if(strlen(pPB->PhoneNum) != 0)
    {
        (*ppItem)->pNext = Item_New(AB_TYPEMASK_MOBILE);

        if((*ppItem)->pNext == NULL)
        {
            Item_Erase(*ppItem);
            
            *ppItem = NULL;
            
            return FALSE;
        }
        
        (*ppItem)->pNext->dwMask = AB_TYPEMASK_MOBILE;
        (*ppItem)->pNext->bChange = FALSE;
        (*ppItem)->pNext->nMaxLen = AB_MAXLEN_MOBILE;
        strcpy((*ppItem)->pNext->pszData,pPB->PhoneNum);
        
        (*ppItem)->pNext->nID = 2;

        *nTelID = 2;
    }

    (*nTelID)++;

    return TRUE;
}
/*********************************************************************\
* Function	   ABSIM_OnCopyAll
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABSIM_OnCopyAll(HWND hWnd,BOOL bCopy)
{
    int  i,nCount;
    HWND hLst;
    PPHONEBOOK pPB;
    CONTACT_ITEMCHAIN* pItem;
    DWORD dwoffset;
    DWORD id;
    int   nGroup;
    int   nTelID;
    ABSIM_DATA* pData;

    pData = GetUserData(hWnd);

	if(bCopy == FALSE)
	{
		return;
	}

    WaitWin(hWnd,TRUE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);     

    hLst = GetDlgItem(hWnd,IDC_ABSIM_LIST);

    nCount = SendMessage(hLst,LB_GETCOUNT,0,0);

	for(i = nCount-1; i >= 1 ; i--)
	{
        if(AB_IsUserCancel(hWnd,WM_CANCELCOPYFROMSIM))
        {
            WaitWin(hWnd,FALSE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);
            
            return;
        }

        if(AB_IsFlashEnough() == FALSE)
        {
            WaitWin(hWnd,FALSE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);
            
            PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_SIM,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
            
            return;
        }

        pPB = (PPHONEBOOK)SendMessage(hLst,LB_GETITEMDATA,i,0);

        pItem = NULL;
        
        dwoffset = -1;
        
        id = -1;

        nGroup = -1;
        
        if(AB_CopyFromSIM(pPB,&pItem,&nTelID))
        {
            if(AB_SaveRecord(&dwoffset,&id,&nGroup,FALSE,nTelID,pItem))
                AB_Insert2Table(dwoffset,id,nGroup,pItem);
            
            Item_Erase(pItem);
        }
        else
        {
            Item_Erase(pItem);

            WaitWin(hWnd,FALSE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);
            
            PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_SIM,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
                        
            return;
        }

	}

    WaitWin(hWnd,FALSE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);     
    
    PLXTipsWin(NULL,NULL,0,IDS_COPIEDTOCONTACTS,IDS_SIM,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    
}
/*********************************************************************\
* Function	ABSIM_OnDataChange
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABSIM_OnDataChange(HWND hWnd,PPHONEBOOK pPB,int nMode)
{  
    ABSIM_DATA *pData;
    HWND hList;
    char szButText[20],szMenu[20];
    PPHONEBOOK pPBCompare;
    int i,nCount;
    
    pData = GetUserData(hWnd);
    
    hList = GetDlgItem(hWnd,IDC_ABSIM_LIST);
    
    switch(nMode)
    {
    case AB_MODE_INSERT:
        
        nCount = SendMessage(hList,LB_GETCOUNT,0,0);
                
        for(i = 1 ; i < nCount ; i++)
        {
            pPBCompare = (PPHONEBOOK)SendMessage(hList, LB_GETITEMDATA,i,0);

            if(stricmp(pPBCompare->Text,pPB->Text) > 0)
                break;
        }
        SendMessage(hList, LB_INSERTSTRING, i, (LPARAM)pPB->Text);
        
        SendMessage(hList, LB_SETITEMDATA, i, (LPARAM)pPB);
        
        SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, i),(LPARAM)pData->hSIMItem);

        SendMessage(hList, LB_SETCURSEL, i, 0);
               
        memset(szButText,0,20);
        
        memset(szMenu,0,20);
        
        SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szButText);
        
        SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 2, (LPARAM)szMenu);
        
        if(i == 0)
        {  
            if(strcmp(szButText,IDS_OPEN) == 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
            
            if(strcmp(szMenu,ICON_SELECT) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
        }
        else
        {
            if(strcmp(szButText,IDS_OPEN) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
            
            if(strcmp(szMenu,ICON_OPTIONS) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
        }
        break;
        
    case AB_MODE_DELETE:
        
        nCount = SendMessage(hList,LB_GETCOUNT,0,0);
                
        for(i = 1 ; i < nCount ; i++)
        {
            pPBCompare = (PPHONEBOOK)SendMessage(hList, LB_GETITEMDATA,i,0);

            if(pPBCompare->Index == pPB->Index)
            {
                SendMessage(hList,LB_DELETESTRING,i,NULL);

                break;
            }
        }
        
        if(i == SendMessage(hList,LB_GETCOUNT,0,0))
            SendMessage(hList,LB_SETCURSEL,i-1,NULL);
        else
            SendMessage(hList,LB_SETCURSEL,i,NULL);
        
        memset(szButText,0,20);
        
        memset(szMenu,0,20);
        
        SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szButText);
        
        SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 2, (LPARAM)szMenu);
        
        i = SendMessage(hList,LB_GETCURSEL,0,0);
        
        if(i == 0)
        {  
            if(strcmp(szButText,IDS_OPEN) == 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
            
            if(strcmp(szMenu,ICON_SELECT) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
        }
        else
        {
            if(strcmp(szButText,IDS_OPEN) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
            
            if(strcmp(szMenu,ICON_OPTIONS) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
        }
        break;
        
    default:
        break;
    }
}
/*********************************************************************\
* Function	CallListWndProc
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static LRESULT CallListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HWND    hParent;
    ABSIM_DATA* pData;

    hParent = GetParent(hWnd);
    
    pData = GetUserData(hParent);
    
    switch(wMsgCmd)
    {         
    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        { 
        case VK_LEFT:
        case VK_RIGHT:
        case VK_1:
        case VK_2:
        case VK_3:
        case VK_4:
        case VK_5:
        case VK_6:
        case VK_7:
        case VK_8:
        case VK_9:
        case VK_0:
        case VK_F3:
        case VK_F4:
            PostMessage(hParent,wMsgCmd,wParam,lParam);
            return lResult;     

        default:
            return CallWindowProc(pData->OldListWndProc, hWnd, wMsgCmd, wParam, lParam);
        }
    default:
        return CallWindowProc(pData->OldListWndProc, hWnd, wMsgCmd, wParam, lParam);
    }
    return lResult;       
}
/*********************************************************************\
* Function	AB_FindUseableSIMPB
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
PPHONEBOOK AB_FindUseableSIMPB(void)
{
    int i;
    PCONTACT_INITDATA pInit;
    
    pInit = AB_GetSIMData();

    for( i = 0 ; i < pInit->nTotalNum ; i++ )
    {
        if(pInit->pSIMPhoneBook[i].Index == 0)
            return &(pInit->pSIMPhoneBook[i]);
    }

    return NULL;
}
/*********************************************************************\
* Function	AB_FindUseableIndex
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
int AB_FindUseableIndex(void)
{    
    int nIDTemp = 1;

    while( nIDTemp )
    {
        if(AB_IsIndexUsed(nIDTemp))
            nIDTemp++;
        else
            return nIDTemp;
    }

    return nIDTemp;
}
/*********************************************************************\
* Function	AB_IsIndexUsed
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL AB_IsIndexUsed(int nIDTemp)
{
    int i;
    PCONTACT_INITDATA pInit;
    
    pInit = AB_GetSIMData();

    for( i = 0 ; i < pInit->nTotalNum ; i++ )
    {
        if(pInit->pSIMPhoneBook[i].Index == nIDTemp)
            return TRUE;
    }

    return FALSE;
}

BOOL AB_CreateSIMInfoWnd(HWND hFrameWnd)
{
    WNDCLASS    wc;
    HWND        hSIMInfoWnd;
	RECT		rcClient;
    
    wc.style         = 0;
    wc.lpfnWndProc   = ABSIMInfoWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABSIMInfoWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;
    
	GetClientRect(hFrameWnd,&rcClient);
	
    hSIMInfoWnd = CreateWindow(
        "ABSIMInfoWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
        hFrameWnd,
        NULL, 
        NULL, 
        NULL
        );
    
    if (!hSIMInfoWnd)
    {
        UnregisterClass("ABSIMInfoWndClass",NULL);
        return FALSE;
    }

	SetFocus(hSIMInfoWnd);
	
	SetWindowText(hFrameWnd,IDS_SIMINFO);
	
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_OK);    
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL, 0), (LPARAM)"");   
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    ShowWindow(hSIMInfoWnd,SW_SHOW);
    UpdateWindow(hSIMInfoWnd);
    
    return TRUE;
}

LRESULT ABSIMInfoWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABSIMInfo_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        ABSIMInfo_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

	case WM_SETFOCUS:
        ABSIMInfo_OnSetFocus(hWnd);
		break;

    case WM_PAINT:
        ABSIMInfo_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABSIMInfo_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABSIMInfo_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABSIMInfo_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABSIMInfo_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABSIMInfo_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABSIMInfo_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    HWND hFromView;
    PCONTACT_INITDATA pData;
    char sztemp[10];
    LISTBUF Listtmp;
    
	PREBROW_InitListBuf(&Listtmp);

    pData = AB_GetSIMData();
    
    GetClientRect(hWnd,&rect);

    sprintf(sztemp,"%d",pData->nTotalNum);
    PREBROW_AddData(&Listtmp, IDS_SIMCAPACITY, sztemp);
           
    sprintf(sztemp,"%d",(pData->nTotalNum-pData->nNumber));
    PREBROW_AddData(&Listtmp, IDS_FREECAPACITY, sztemp);
    
    sprintf(sztemp,"%d",pData->nNumber);
    PREBROW_AddData(&Listtmp, IDS_USEDCAPACITY, sztemp);

	hFromView = CreateWindow(
        FORMVIEWER,
        "",
		WS_VISIBLE | WS_CHILD ,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
		hWnd,
        (HMENU)IDC_ABSIMINFO_LIST,
        NULL, 
        (PVOID)&Listtmp);

	if (hFromView == NULL) 
		return FALSE;

    return TRUE;
    
}
/*********************************************************************\
* Function	ABSIMInfo_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSIMInfo_OnActivate(HWND hwnd, UINT state)
{
    HWND hFormView = NULL;
	HWND hFrameWnd;

	hFrameWnd = GetParent(hwnd);

    hFormView = GetDlgItem(hwnd,IDC_ABSIMINFO_LIST);

	SetFocus(hFormView);

	SetWindowText(hFrameWnd,IDS_SIMINFO);
	
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);    
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");   
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return;
}
/*********************************************************************\
* Function	ABSIMInfo_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSIMInfo_OnSetFocus(HWND hwnd)
{
    HWND hFormView = NULL;

    hFormView = GetDlgItem(hwnd,IDC_ABSIMINFO_LIST);

	SetFocus(hFormView);

    return;
}
/*********************************************************************\
* Function	ABSIMInfo_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSIMInfo_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABSIMInfo_OnKey
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
static void ABSIMInfo_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
//    HWND hSIMViewWnd;

    switch (vk)
	{
	case VK_RETURN:
		PostMessage(hWnd,WM_CLOSE,0,0);
        
//        hSIMViewWnd = GetDlgItem(GetParent(hWnd),IDC_AB_SIMVIEW);
        
//        if(hSIMViewWnd)
//            PostMessage(hSIMViewWnd,WM_CLOSE,0,0);
		break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABSIMInfo_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABSIMInfo_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{
    default:
        break;
    }
}
/*********************************************************************\
* Function	ABSIMInfo_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSIMInfo_OnDestroy(HWND hWnd)
{
    
    UnregisterClass("ABSIMInfoWndClass", NULL);

    return;

}
/*********************************************************************\
* Function	ABSIM_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSIMInfo_OnClose(HWND hWnd)
{
	SendMessage(GetParent(hWnd),PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);

    return;
}



typedef struct tagSIM_EditData{
	HWND		hFrameWnd;
    PPHONEBOOK  pPBook;
    char*       pszFocus;
    HWND        hFocus;
    BOOL        bChange;
    BOOL        bNameSetText;
    BOOL        bNumSetText;
    BOOL        bNew;
}SIM_EDITDATA,*PSIM_EDITDATA;


BOOL AB_CreateSIMEditWnd(HWND hFrameWnd,PPHONEBOOK pPhoneBook,char* pszFocus,BOOL bNew)
{
    WNDCLASS    wc;
    HWND        hSIMEditWnd;
    const char  *pszCaption;
	RECT		rcClient;
    SIM_EDITDATA    simeditdata;
	SIM_EDITDATA*    pData;

    memset(&simeditdata,0,sizeof(SIM_EDITDATA));

	simeditdata.hFrameWnd = hFrameWnd;
    simeditdata.pPBook = pPhoneBook;
    simeditdata.pszFocus = pszFocus;
    simeditdata.bNew = bNew;
    simeditdata.bChange = FALSE;
    
    wc.style         = 0;
    wc.lpfnWndProc   = ABSIMEditWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(SIM_EDITDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABSIMEditWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;
    
	GetClientRect(hFrameWnd,&rcClient);

    hSIMEditWnd = CreateWindow(
        "ABSIMEditWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
        hFrameWnd,
        NULL, 
        NULL, 
        (PVOID)&simeditdata
        );
    
    if (!hSIMEditWnd)
    {
        UnregisterClass("ABSIMEditWndClass",NULL);
        return FALSE;
    }
    
    if(bNew)
        pszCaption = IDS_NEWSIMCONTACT;
    else
    {
        if(strlen(pPhoneBook->Text) == 0)
            pszCaption = IDS_UNNAMED;
        else
            pszCaption = pPhoneBook->Text;
    }

	pData = GetUserData(hSIMEditWnd);
	
	SetFocus(pData->hFocus);
	SendMessage(pData->hFocus, EM_SETSEL, -1, -1);
                
    SetWindowText(hFrameWnd,pszCaption);
	
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_SAVE, 1), (LPARAM)IDS_SAVE);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
   
    ShowWindow(hSIMEditWnd,SW_SHOW);

    UpdateWindow(hSIMEditWnd);

    return TRUE;
}

LRESULT ABSIMEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABSIMEdit_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        ABSIMEdit_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABSIMEdit_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABSIMEdit_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABSIMEdit_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABSIMEdit_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABSIMEdit_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABSIMEdit_OnDestroy(hWnd);
        break;

	case WM_SAVECONTACT:
        ABSIMEdit_OnSave(hWnd,(BOOL)lParam);
		break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABSIMEdit_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABSIMEdit_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    HWND hEdit;    
    RECT rcClient;
    IMEEDIT ie;
    int  nHeight,nWidth,nY = 0;
    SIM_EDITDATA*    pData;
    char szCaption[100];

    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(SIM_EDITDATA));

    GetClientRect(hWnd,&rcClient);

    nHeight = (rcClient.bottom - rcClient.top) / 3;

    nWidth = rcClient.right - rcClient.left;

    nY = 0;
    
    memset(&ie, 0, sizeof(IMEEDIT));
    
    ie.hwndNotify	= (HWND)hWnd;    
    ie.dwAttrib	    = 0;                
    ie.dwAscTextMax	= 0;
    ie.dwUniTextMax	= 0;
    ie.wPageMax	    = 0;        
    ie.pszCharSet	= NULL;
    ie.pszTitle	    = NULL;
    ie.pszImeName	= NULL;
    
    hEdit = CreateWindow(
        "IMEEDIT",
        "",
        WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | ES_TITLE ,
        0,
        nY,
        nWidth,
        nHeight,
        hWnd,
        (HMENU)IDC_SIMEDIT_TEXT,
        NULL,
        (PVOID)&ie);
    
    if(hEdit == NULL)
        return FALSE;
    
    SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)(AB_MAXLEN_SIMNAME-1), NULL);
    
    szCaption[0] = 0;

    sprintf(szCaption,"%s:",IDS_NAME);

    SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)szCaption);
    
    pData->bNameSetText = TRUE;

    SetWindowText(hEdit, pData->pPBook->Text);

    if(pData->pszFocus == pData->pPBook->Text)
        pData->hFocus = hEdit;

    nY += nHeight;
    
    memset(&ie, 0, sizeof(IMEEDIT));
    
    ie.hwndNotify	= (HWND)hWnd;    
    ie.dwAttrib	    = 0;                
    ie.dwAscTextMax	= 0;
    ie.dwUniTextMax	= 0;
    ie.wPageMax	    = 0;        
    ie.pszCharSet	= NULL;
    ie.pszTitle	    = NULL;
    ie.pszImeName	= NULL;
    ie.pszImeName	= "Phone";
    
    hEdit = CreateWindow(
        "IMEEDIT",
        "",
        WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | ES_TITLE ,
        0,
        nY,
        nWidth,
        nHeight,
        hWnd,
        (HMENU)IDC_SIMEDIT_NUMBER,
        NULL,
        (PVOID)&ie);
    
    if(hEdit == NULL)
        return FALSE;
    
    SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)(AB_MAXLEN_SIMNUM-1), NULL);
    
    szCaption[0] = 0;

    sprintf(szCaption,"%s:",IDS_NUMBER);

    SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)szCaption);
    
    pData->bNumSetText = TRUE;

    SetWindowText(hEdit, pData->pPBook->PhoneNum);

    if(pData->pszFocus == pData->pPBook->PhoneNum)
        pData->hFocus = hEdit;

    return TRUE;
    
}
/*********************************************************************\
* Function	ABSIMEdit_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSIMEdit_OnActivate(HWND hwnd, UINT state)
{
    SIM_EDITDATA*    pData;
	char*			 pszCaption;

    pData = GetUserData(hwnd);

	SetFocus(pData->hFocus);

	if(pData->bNew)
        pszCaption = (char*)IDS_NEWSIMCONTACT;
    else
    {
        if(strlen(pData->pPBook->Text) == 0)
            pszCaption = (char*)IDS_UNNAMED;
        else
            pszCaption = pData->pPBook->Text;
    }
        
    SetWindowText(pData->hFrameWnd,pszCaption);
	
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
       
    return;
}
/*********************************************************************\
* Function	ABSIMEdit_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSIMEdit_OnSetFocus(HWND hwnd)
{
    SIM_EDITDATA*    pData;

    pData = GetUserData(hwnd);

	SetFocus(pData->hFocus);

    return;
}
/*********************************************************************\
* Function	ABSIMInfo_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSIMEdit_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABSIMEdit_OnKey
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
static void ABSIMEdit_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    SIM_EDITDATA*   pData;
    char            szTitle[50];

    switch (vk)
	{
    case VK_F10:
        pData = GetUserData(hWnd);

        szTitle[0] = 0;

		GetWindowText(pData->hFrameWnd,szTitle,49);

        szTitle[49] = 0;
        
        if(pData->bChange)
		{
			PLXConfirmWinEx(GetParent(hWnd),hWnd,IDS_SAVECHANGES, Notify_Request, szTitle, IDS_YES, IDS_NO,WM_SAVECONTACT);
			break;
		}
        else
        {
            if(pData->bNew)
                pData->pPBook->Index = 0;

            PostMessage(hWnd,WM_CLOSE,0,0);
        }
        
        break;

    case VK_RETURN:
        ABSIMEdit_OnSave(hWnd,TRUE);
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABSIMEdit_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABSIMEdit_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    SIM_EDITDATA*   pData;

    pData = GetUserData(hWnd);

	switch(id)
	{
    case IDC_SIMEDIT_TEXT:
        if(codeNotify == EN_CHANGE)
        {            
            HDC  hdc;
            int  nFit = 0;
            int  nLen;
            HWND hEdt;
            char szCaption[ME_PHONETXT_LEN];
            char szString[ME_PHONETXT_LEN];

            if(pData->bNew)
                break;

            szCaption[0] = 0;

            hEdt = GetDlgItem(hWnd,IDC_SIMEDIT_TEXT);
            
            nLen = GetWindowTextLength(hEdt);

            if(nLen == 0)
            {
                SetWindowText(hWnd,IDS_UNNAMED);
                break;
            }

            GetWindowText(hEdt,szCaption,nLen);

            szString[0] = 0;

            hdc = GetDC(hWnd);
            
            GetExtentFittedText(hdc,szCaption,-1,szString,50,TEXT_WIDTH,OMIT_SUFFIX,SUFFIX_REPEAT);
            
            ReleaseDC(hWnd,hdc);
            
            SetWindowText(hWnd,szString);
            
            if(pData->bChange == TRUE)
                break;

            if(pData->bNameSetText == TRUE)
                pData->bNameSetText = FALSE;
            else
                pData->bChange = TRUE;
        }
		else if(codeNotify == EN_SETFOCUS)
		{
			pData->hFocus = GetDlgItem(hWnd,IDC_SIMEDIT_TEXT);
		}
        break;

    case IDC_SIMEDIT_NUMBER:
        if(codeNotify == EN_CHANGE)
        {       
            if(pData->bChange == TRUE)
                break;

            if(pData->bNumSetText == TRUE)
                pData->bNumSetText = FALSE;
            else
                pData->bChange = TRUE;
        }
		else if(codeNotify == EN_SETFOCUS)
		{
			pData->hFocus = GetDlgItem(hWnd,IDC_SIMEDIT_NUMBER);
		}
        break;

    default:
        break;
    }
}
/*********************************************************************\
* Function	ABSIMEdit_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSIMEdit_OnDestroy(HWND hWnd)
{
 
    UnregisterClass("ABSIMEditWndClass", NULL);

    return;

}
/*********************************************************************\
* Function	ABSIMEdit_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSIMEdit_OnClose(HWND hWnd)
{
    SIM_EDITDATA*   pData;

    pData = GetUserData(hWnd);
	
	SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);

    return;
}
/*********************************************************************\
* Function	   ABSIMEdit_OnSave
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABSIMEdit_OnSave(HWND hWnd,BOOL bSave)
{
    SIM_EDITDATA*   pData;
    HWND            hEdt;
    PHONEBOOK       PBTemp;
    PCONTACT_INITDATA pInit;
    MSG             myMsg;
    HWND            hSIMViewWnd;
    
    pData = GetUserData(hWnd);

    if(bSave == FALSE)
    {
        if(pData->bNew)
            pData->pPBook->Index = 0;
    }
    else
    {
        pData = GetUserData(hWnd);
        memset(&PBTemp,0,sizeof(PHONEBOOK));
        PBTemp.Index = pData->pPBook->Index;
        
        hEdt = GetDlgItem(hWnd,IDC_SIMEDIT_TEXT);
        GetWindowText(hEdt,PBTemp.Text,AB_MAXLEN_SIMNAME);
        
        hEdt = GetDlgItem(hWnd,IDC_SIMEDIT_NUMBER);
        GetWindowText(hEdt,PBTemp.PhoneNum,AB_MAXLEN_SIMNUM);

        if(strlen(PBTemp.Text) == 0)
        {
            char szTitle[50];

            szTitle[0] = 0;

            GetWindowText(pData->hFrameWnd,szTitle,49);

            szTitle[49] = 0;

            PLXTipsWin(NULL,NULL,0,IDS_DEFINENAME,szTitle,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);

            return;
        }

		if(strlen(PBTemp.PhoneNum) == 0)
		{
			char szTitle[50];
			
			szTitle[0] = 0;
			
			GetWindowText(pData->hFrameWnd,szTitle,49);
			
			szTitle[49] = 0;
			
			PLXTipsWin(NULL,NULL,0,IDS_NOPHONENUM,szTitle,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
			
			return;
		}

        pInit = AB_GetSIMData();

        if(strlen(PBTemp.PhoneNum) > pInit->MaxLenInfo.iNumberMaxLen)
        {
            PBTemp.PhoneNum[pInit->MaxLenInfo.iNumberMaxLen] = 0;
        }

        if(strlen(PBTemp.Text) > pInit->MaxLenInfo.iTextMaxLen)
        {
            PBTemp.Text[pInit->MaxLenInfo.iTextMaxLen] = 0;
        }
        
        WaitWindowStateEx(pData->hFrameWnd,TRUE,IDS_SAVING,IDS_SIM,NULL,NULL);
        
        if (ME_WritePhonebook(pInit->hCtrlWnd, AB_MSG_SIM_WRITE, PBOOK_SM, &PBTemp) == -1)
        {
            if(pData->bNew == TRUE)
                PBTemp.Index = 0;
            WaitWindowStateEx(pData->hFrameWnd,TRUE,IDS_SAVING,IDS_SIM,NULL,NULL);
            return;
        }
        
		SetTimer(pInit->hCtrlWnd, SIMWRITE_OVERTIMER, 10000, NULL);
        while(GetMessage(&myMsg, NULL, 0, 0)) 
        {
            if((myMsg.message == AB_MSG_SIM_WRITE_SUCC) && (myMsg.hwnd == pInit->hCtrlWnd))
            {
                WaitWindowStateEx(pData->hFrameWnd,FALSE,IDS_SAVING,IDS_SIM,NULL,NULL);

                memcpy(pData->pPBook,&PBTemp,sizeof(PHONEBOOK));
                PLXTipsWin(NULL,NULL,0,IDS_SAVED,pData->pPBook->Text,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
                if(pData->bNew == TRUE)
                    pInit->nNumber++;
                else
                    AB_NotifyWnd(pData->pPBook,AB_MODE_DELETE,AB_OBJECT_ALL,AB_MDU_SIMCONTRACT);
                AB_NotifyWnd(pData->pPBook,AB_MODE_INSERT,AB_OBJECT_ALL,AB_MDU_SIMCONTRACT);
                PostMessage(hWnd,WM_CLOSE,NULL,NULL);
                break;
            }
            else if((myMsg.message == AB_MSG_SIM_WRITE_FAIL) && (myMsg.hwnd == pInit->hCtrlWnd))
            {
                if(pData->bNew == TRUE)
                    pData->pPBook->Index = 0;
                WaitWindowStateEx(pData->hFrameWnd,FALSE,IDS_SAVING,IDS_SIM,NULL,NULL);
                break;
            }
            else if((myMsg.message == AB_MSG_SIM_WRITE_FULL) && (myMsg.hwnd == pInit->hCtrlWnd))
            {
                if(pData->bNew == TRUE)
                    pData->pPBook->Index = 0;
                WaitWindowStateEx(pData->hFrameWnd,FALSE,IDS_SAVING,IDS_SIM,NULL,NULL);
                PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);//
                break;
            }			
            TranslateMessage(&myMsg);
            DispatchMessage(&myMsg);
        }
    }
    PostMessage(hWnd,WM_CLOSE,0,0);

    hSIMViewWnd = GetDlgItem(pData->hFrameWnd,IDC_AB_SIMVIEW);

    if(hSIMViewWnd)
        PostMessage(hSIMViewWnd,WM_CLOSE,0,0);
}




typedef struct tagSIM_ViewData{
	HWND		hFrameWnd;
	HMENU		hMenu;
    PPHONEBOOK  pPBook;
    PVOID       handle;
}SIM_VIEWDATA,*PSIM_VIEWDATA;


#define IDC_ABSIMVIEW_LIST  300

BOOL AB_CreateViewWnd(HWND hFrameWnd,PPHONEBOOK pPhoneBook)
{
    WNDCLASS    wc;
    HWND        hSIMViewWnd;
    SIM_VIEWDATA simviewdata;
	RECT		rcClient;

    memset(&simviewdata,0,sizeof(SIM_VIEWDATA));

	simviewdata.hFrameWnd = hFrameWnd;
    simviewdata.pPBook = pPhoneBook;
    
    wc.style         = 0;
    wc.lpfnWndProc   = ABSIMViewWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(SIM_VIEWDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABSIMViewWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;
    
	GetClientRect(hFrameWnd,&rcClient);

    simviewdata.hMenu = CreateMenu();

    hSIMViewWnd = CreateWindow(
        "ABSIMViewWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
        hFrameWnd,
        (HMENU)IDC_AB_SIMVIEW, 
        NULL, 
        (PVOID)&simviewdata
        );
    
    if (!hSIMViewWnd)
    {
        UnregisterClass("ABSIMViewWndClass",NULL);
        return FALSE;
    }

	SetFocus(hSIMViewWnd);
               
	if(pPhoneBook->Text[0] == 0)
		SetWindowText(hFrameWnd, "Unnamed");
	else
		SetWindowText(hFrameWnd,pPhoneBook->Text);
    
    AppendMenu(simviewdata.hMenu, MF_ENABLED, IDM_ABSIM_EDIT, IDS_EDIT);
//    hDelMenu = CreateMenu();
//    AppendMenu(hDelMenu, MF_ENABLED, IDM_ABSIM_DELSELECT, IDS_SELECT);
//    AppendMenu(hDelMenu, MF_ENABLED, IDM_ABSIM_DELALL, IDS_ALL);
//    
//    AppendMenu(hMenu, MF_POPUP|MF_ENABLED, (DWORD)hDelMenu, IDS_DELETEMANY);
//
//    hCopytoContactMenu = CreateMenu();
//    AppendMenu(hCopytoContactMenu, MF_ENABLED, IDM_ABSIM_COPYTOCONTACTSELECT, IDS_SELECT);
//    AppendMenu(hCopytoContactMenu, MF_ENABLED, IDM_ABSIM_COPYTOCONTACTALL, IDS_ALL);
//
//     AppendMenu(hMenu, MF_POPUP|MF_ENABLED, (DWORD)hCopytoContactMenu, IDS_COPYTOCONTACTS);
    AppendMenu(simviewdata.hMenu, MF_ENABLED, IDM_ABSIM_COPYTOCONTACTS, IDS_COPYTOCONTACTS);
    AppendMenu(simviewdata.hMenu, MF_ENABLED, IDM_ABSIM_SIMINFO, IDS_SIMINFO);
	AppendMenu(simviewdata.hMenu, MF_ENABLED, IDM_ABSIM_DELETE, IDS_DELETE);
	
	PDASetMenu(hFrameWnd,simviewdata.hMenu);
	
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 0), (LPARAM)IDS_BACK);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_ABSIM_EDIT, 1), (LPARAM)IDS_EDIT);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);

    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    ShowWindow(hSIMViewWnd,SW_SHOW);
    UpdateWindow(hSIMViewWnd);

    return TRUE;
}

LRESULT ABSIMViewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABSIMView_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        ABSIMView_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABSIMView_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABSIMView_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABSIMView_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        lResult = ABSIMView_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam),lParam);
        break;
        
    case WM_CLOSE:
        ABSIMView_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABSIMView_OnDestroy(hWnd);
        break;

    case WM_DATACHANGE:
        ABSIMView_OnDataChange(hWnd,(PPHONEBOOK)wParam,(int)lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABSIMView_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABSIMView_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT    rect;
    HWND    hList;
    int     index;
    SIM_VIEWDATA*    pData;
    char    szCaption[100];

    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(SIM_VIEWDATA));

    GetClientRect(hWnd,&rect);
    
    hList = CreateWindow(
        "LISTBOX",
        "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP | LBS_MULTILINE,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_ABSIMVIEW_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;
       
//    szCaption[0] = 0;
//    sprintf(szCaption,"%s:",IDS_NAME);
//    index = SendMessage(hList, LB_ADDSTRING, -1, (LPARAM)szCaption);
//    SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)pData->pPBook->Text);
//    SendMessage(hList,LB_SETITEMDATA,index,(LPARAM)pData->pPBook->Text);

    szCaption[0] = 0;
    sprintf(szCaption,"%s:",IDS_NUMBER);
    index = SendMessage(hList, LB_ADDSTRING, -1, (LPARAM)szCaption);
    SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)pData->pPBook->PhoneNum);
    SendMessage(hList,LB_SETITEMDATA,index,(LPARAM)pData->pPBook->PhoneNum);

    SendMessage(hList, LB_SETCURSEL,0,0);

    pData->handle = AB_RegisterNotify(hWnd,WM_DATACHANGE,AB_OBJECT_ALL,AB_MDU_SIMCONTRACT);

    return TRUE;
    
}
/*********************************************************************\
* Function	ABSIMView_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSIMView_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;
    SIM_VIEWDATA*    pData;

    pData = GetUserData(hwnd);

    hLst = GetDlgItem(hwnd,IDC_ABSIMVIEW_LIST);

	SetFocus(hLst);
    
	if(pData->pPBook->Text[0] == 0)
		SetWindowText(pData->hFrameWnd, "Unnamed");
	else
		SetWindowText(pData->hFrameWnd,pData->pPBook->Text);

	PDASetMenu(pData->hFrameWnd,pData->hMenu);
	
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);

    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return;
}
/*********************************************************************\
* Function	ABSIMView_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSIMView_OnSetFocus(HWND hwnd)
{
    HWND hLst;

    hLst = GetDlgItem(hwnd,IDC_ABSIMVIEW_LIST);

	SetFocus(hLst);
    
    return;
}
/*********************************************************************\
* Function	ABSIMInfo_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSIMView_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABSIMView_OnKey
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
static void ABSIMView_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    SIM_VIEWDATA *pData;
    HWND hLst;
    int  index;
    
    pData = GetUserData(hWnd);
    switch (vk)
	{
    case VK_F10:
        PostMessage(hWnd,WM_CLOSE,0,0);
        break;

	case VK_RETURN:
		SendMessage(hWnd,WM_COMMAND,IDM_ABSIM_EDIT,NULL);
		break;

	case VK_F5:
		PDADefWindowProc(pData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;

    case VK_F1:
        hLst = GetDlgItem(hWnd,IDC_ABSIMVIEW_LIST);
        
        index = SendMessage(hLst,LB_GETCURSEL,0,0);

        if(index == LB_ERR || index == 0)
            break;
        
        pData = GetUserData(hWnd);

        if(strlen(pData->pPBook->PhoneNum) != 0)
            APP_CallPhoneNumber(pData->pPBook->PhoneNum);
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABSIMView_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static LRESULT ABSIMView_OnCommand(HWND hWnd, int id, UINT codeNotify,LPARAM lParam)
{
	switch(id)
	{
    case IDC_ABSIMVIEW_LIST:
        if(codeNotify == LBN_SETFONT)
        {            
            HFONT hFont = NULL;
            
            GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);
            
            return (LRESULT)hFont;
        }
        break;

    case IDM_ABSIM_EDIT:
        {
            SIM_VIEWDATA *pData;
            HWND hLst;
            int  nIndex;
            char* pszFocus;

            hLst = GetDlgItem(hWnd,IDC_ABSIMVIEW_LIST);

            pData = GetUserData(hWnd);
            
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);

            if(nIndex == LB_ERR)
                break;

            pszFocus = (char*)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);

            AB_CreateSIMEditWnd(pData->hFrameWnd,pData->pPBook,pszFocus,FALSE);
        }
        break;

    case IDM_ABSIM_DELETE:
        {
            HWND hFindWnd;
            SIM_VIEWDATA* pData;
            
            pData = GetUserData(hWnd);
            hFindWnd = GetDlgItem(pData->hFrameWnd,IDC_AB_SIM);
            SendMessage(hFindWnd,WM_COMMAND,IDM_ABSIM_DELETE,NULL);
        }
        break;
        
    case IDM_ABSIM_COPYTOCONTACTS:
        {
            SIM_VIEWDATA* pData;
            CONTACT_ITEMCHAIN* pItem;
            DWORD dwoffset;
            DWORD id;
            int   nGroup;
            int   nTelID;
            
            pData = GetUserData(hWnd);
            
            WaitWin(hWnd,TRUE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);            
            
            if(AB_IsUserCancel(hWnd,WM_CANCELCOPYFROMSIM))
            {
                WaitWin(hWnd,FALSE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);

                return FALSE;
            }

            if(AB_IsFlashEnough() == FALSE)
            {
                WaitWin(hWnd,FALSE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);
                
                PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_SIM,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
                
                return FALSE;
            }
            
            pItem = NULL;
            
            dwoffset = -1;
            
            id = -1;
            
            nGroup = -1;
            
            if(AB_CopyFromSIM(pData->pPBook,&pItem,&nTelID))
            {
                if(AB_SaveRecord(&dwoffset,&id,&nGroup,FALSE,nTelID,pItem))
                    AB_Insert2Table(dwoffset,id,nGroup,pItem);
                
                Item_Erase(pItem);
            }
            else
            {
                Item_Erase(pItem);
                
                WaitWin(hWnd,FALSE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);
                
                PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_SIM,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
                
                return FALSE;
            }
            
            WaitWin(hWnd,FALSE,IDS_COPYFROMSIM,IDS_SIM,NULL,IDS_CANCEL,WM_CANCELCOPYFROMSIM);         
            
            PLXTipsWin(NULL,NULL,0,IDS_COPIEDTOCONTACTS,IDS_SIM,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
            
        }
        break;
        
    case IDM_ABSIM_SIMINFO:
        {
            SIM_VIEWDATA* pData;
			pData = GetUserData(hWnd);
            
			AB_CreateSIMInfoWnd(pData->hFrameWnd);
		}
        break;

    case IDM_ABSIM_COPYTOCONTACTSELECT:
        break;

    case IDM_ABSIM_COPYTOCONTACTALL:
        break;

    case IDM_ABSIM_DELALL:
        break;

    case IDM_ABSIM_DELSELECT:
        break;

    default:
        break;
    }
    return TRUE;
}
/*********************************************************************\
* Function	ABSIMView_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSIMView_OnDestroy(HWND hWnd)
{
    SIM_VIEWDATA*    pData;

    pData = GetUserData(hWnd);

	DestroyMenu(pData->hMenu);
    
    AB_UnRegisterNotify(pData->handle);

    UnregisterClass("ABSIMViewWndClass", NULL);

    return;

}
/*********************************************************************\
* Function	ABSIMView_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSIMView_OnClose(HWND hWnd)
{
    SIM_VIEWDATA*    pData;

    pData = GetUserData(hWnd);

	SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);

    return;
}
/*********************************************************************\
* Function	ABSIMView_OnDataChange
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABSIMView_OnDataChange(HWND hWnd,PPHONEBOOK pPB,int nMode)
{
    HWND hList;
    int index;

    hList = GetDlgItem(hWnd,IDC_ABSIMVIEW_LIST);

    SendMessage(hList,LB_RESETCONTENT,0,0);

    index = SendMessage(hList, LB_ADDSTRING, -1, (LPARAM)IDS_NAME);
    SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)pPB->Text);
    SendMessage(hList,LB_SETITEMDATA,index,(LPARAM)pPB->Text);

    index = SendMessage(hList, LB_ADDSTRING, -1, (LPARAM)IDS_NUMBER);
    SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)pPB->PhoneNum);
    SendMessage(hList,LB_SETITEMDATA,index,(LPARAM)pPB->PhoneNum);

    SendMessage(hList, LB_SETCURSEL,0,0);

}

BOOL AB_SIMSaveToSIM(PHONEBOOK pb)
{
    PCONTACT_INITDATA pInit;
    MSG myMsg;

    pInit = AB_GetSIMData();
    
    if (ME_WritePhonebook(pInit->hCtrlWnd, AB_MSG_SIM_WRITE, PBOOK_SM, &pb) == -1)
    {
        return FALSE;
    }
    
    while(GetMessage(&myMsg, NULL, 0, 0)) 
    {
        if((myMsg.message == AB_MSG_SIM_WRITE_SUCC) && (myMsg.hwnd == pInit->hCtrlWnd))
        {
            pInit->nNumber++;
            return TRUE;
        }
        else if((myMsg.message == AB_MSG_SIM_WRITE_FAIL) && (myMsg.hwnd == pInit->hCtrlWnd))
        {
            return FALSE;
        }
        else if((myMsg.message == AB_MSG_SIM_WRITE_FULL) && (myMsg.hwnd == pInit->hCtrlWnd))
        {
            PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);//
            return FALSE;
        }			
        TranslateMessage(&myMsg);
        DispatchMessage(&myMsg);
    }

    return FALSE;
}
/*********************************************************************\
* Function	ABSIM_OnRefreshList
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABSIM_OnRefreshList(HWND hWnd,char* pszSearch,int nLen,BOOL bExit)
{
    int i,nCount,j;
    char szMenu[20],szLeft[20];
    HWND hList;
    int insert;
    PCONTACT_INITDATA pPhoneBook;
    PPHONEBOOK pPB,pPBCur;
    int nScan;
    ABSIM_DATA *pData;

	pData = GetUserData(hWnd);
    
    nScan = 0;
    
    hList = GetDlgItem(hWnd,IDC_ABSIM_LIST);
    
    pPhoneBook = AB_GetSIMData();
    
    if(bExit)
    {
        if(nLen == 0)
        {   
            SendMessage(hList,LB_RESETCONTENT,0,0);    
            
            SendMessage(hList,LB_INSERTSTRING,0,(LPARAM)IDS_NEWSIMCONTACT);
            SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hNewSIM);
            
            for(i = 0 ; i <pPhoneBook->nTotalNum ; i++)
            {
                if(nScan == pPhoneBook->nNumber)
                    break;

                if(pPhoneBook->pSIMPhoneBook[i].Index == 0)
                    continue;
                
                for(j = 1 ; j < nScan+1 ; j++)
                {
                    pPB = (PPHONEBOOK)SendMessage(hList, LB_GETITEMDATA,j,0);
                    if(stricmp(pPB->Text,pPhoneBook->pSIMPhoneBook[i].Text) > 0)
                        break;
                }
                SendMessage(hList, LB_INSERTSTRING, j, (LPARAM)pPhoneBook->pSIMPhoneBook[i].Text);
                SendMessage(hList, LB_SETITEMDATA, j, (LPARAM)&(pPhoneBook->pSIMPhoneBook[i]));
                SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, j),(LPARAM)pData->hSIMItem);
                nScan++;
            }
            
            SendMessage(hList,LB_SETCURSEL,0,0);     
            
            SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
            if(strcmp(szMenu,ICON_SELECT) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
            
            SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
            if(strcmp(szLeft,"") != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
        }
        else
        {
            insert = SendMessage(hList,LB_GETCURSEL,0,0);
            
            if(insert == LB_ERR)       
            {    
                SendMessage(hList,LB_RESETCONTENT,0,0);    
                
                SendMessage(hList,LB_INSERTSTRING,0,(LPARAM)IDS_NEWSIMCONTACT);  
                SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hNewSIM);

                for(i = 0 ; i <pPhoneBook->nNumber ; i++)
                {
                    if(nScan == pPhoneBook->nNumber)
                        break;

                    if(pPhoneBook->pSIMPhoneBook[i].Index == 0)
                        continue;
                    
                    for(j = 1 ; j < nScan+1 ; j++)
                    {
                        pPB = (PPHONEBOOK)SendMessage(hList, LB_GETITEMDATA,j,0);
                        if(stricmp(pPB->Text,pPhoneBook->pSIMPhoneBook[i].Text) > 0)
                            break;
                    }
                    SendMessage(hList, LB_INSERTSTRING, j, (LPARAM)pPhoneBook->pSIMPhoneBook[i].Text);
                    SendMessage(hList, LB_SETITEMDATA, j, (LPARAM)&(pPhoneBook->pSIMPhoneBook[i]));
                    SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, j),(LPARAM)pData->hSIMItem);
                    nScan++;
                }
                
                SendMessage(hList,LB_SETCURSEL,0,0);    
                
                SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
                if(strcmp(szMenu,ICON_SELECT) != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
                
                SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
                if(strcmp(szLeft,"") != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
            }
            else
            {
                if(SendMessage(hList,LB_GETCOUNT,0,0) > 1)
                    pPBCur = (PPHONEBOOK)SendMessage(hList,LB_GETITEMDATA,insert,0);
                else
                    pPBCur = NULL;
                
                SendMessage(hList,LB_RESETCONTENT,0,0);
                
                SendMessage(hList,LB_INSERTSTRING,0,(LPARAM)IDS_NEWSIMCONTACT);
                SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,0),(LPARAM)pData->hNewSIM);
                
                for(i = 0 ; i <pPhoneBook->nTotalNum ; i++)
                {
                    if(nScan == pPhoneBook->nNumber)
                        break;

                    if(pPhoneBook->pSIMPhoneBook[i].Index == 0)
                        continue;
                    
                    for(j = 1 ; j < nScan+1 ; j++)
                    {
                        pPB = (PPHONEBOOK)SendMessage(hList, LB_GETITEMDATA,j,0);
                        if(stricmp(pPB->Text,pPhoneBook->pSIMPhoneBook[i].Text) > 0)
                            break;
                    }
                    SendMessage(hList, LB_INSERTSTRING, j, (LPARAM)pPhoneBook->pSIMPhoneBook[i].Text);
                    SendMessage(hList, LB_SETITEMDATA, j, (LPARAM)&(pPhoneBook->pSIMPhoneBook[i]));
                    SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, j),(LPARAM)pData->hSIMItem);
                    if(pPBCur &&  pPhoneBook->pSIMPhoneBook[i].Index == pPBCur->Index)
                    {
                        SendMessage(hList,LB_SETCURSEL,j,0); 
                        
                        SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
                        if(strcmp(szMenu,ICON_OPTIONS) != 0)
                            SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
                        
                        SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
                        if(strcmp(szLeft,IDS_OPEN) != 0)
                            SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
                    }
                    
                    nScan++;
                }
                
                if(SendMessage(hList,LB_GETCURSEL,0,0) == LB_ERR)
                {
                    SendMessage(hList,LB_SETCURSEL,0,0);
                    
                    SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
                    if(strcmp(szMenu,ICON_SELECT) != 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
                    
                    SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
                    if(strcmp(szLeft,"") != 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
                }
            }
        }
    }
    else
    {
        SendMessage(hList,LB_RESETCONTENT,0,0);
        
        SendMessage(hList,LB_INSERTSTRING,0,(LPARAM)IDS_NEWSIMCONTACT);
        SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hNewSIM);

        for(i = 0 ; i <pPhoneBook->nTotalNum ; i++)
        {
            if(nScan == pPhoneBook->nNumber)
                break;

            if(pPhoneBook->pSIMPhoneBook[i].Index == 0)
                continue;
            
            if(strstr(pPhoneBook->pSIMPhoneBook[i].Text,pszSearch) != pPhoneBook->pSIMPhoneBook[i].Text)
                continue;
            
            for(j = 1 ; j < nScan+1 ; j++)
            {
                pPB = (PPHONEBOOK)SendMessage(hList, LB_GETITEMDATA,j,0);
                if(stricmp(pPB->Text,pPhoneBook->pSIMPhoneBook[i].Text) > 0)
                    break;
            }
            SendMessage(hList, LB_INSERTSTRING, j, (LPARAM)pPhoneBook->pSIMPhoneBook[i].Text);
            SendMessage(hList, LB_SETITEMDATA, j, (LPARAM)&(pPhoneBook->pSIMPhoneBook[i])); 
            SendMessage(hList, LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, j),(LPARAM)pData->hSIMItem);
            nScan++;
        }
        
        nCount = SendMessage(hList,LB_GETCOUNT,0,0);
        
        if(nCount > 1)
        {
            SendMessage(hList,LB_SETCURSEL,1,0);
            
//            SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
//            if(strcmp(szMenu,ICON_OPTIONS) != 0)
//                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
//            
//            SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
//            if(strcmp(szLeft,IDS_OPEN) != 0)
//                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
        }
        else
        {
            SendMessage(hList,LB_SETCURSEL,0,0);
            
//            SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
//            if(strcmp(szMenu,ICON_SELECT) != 0)
//                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
//            
//            SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
//            if(strcmp(szLeft,"") != 0)
//                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
        }
    }
}
