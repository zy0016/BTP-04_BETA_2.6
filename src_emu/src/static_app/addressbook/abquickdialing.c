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

typedef struct tagAB_QDialData
{ 
	HWND	   hFrameWnd;
	HMENU	   hMenu;
    AB_QDIAL   QDail[AB_QDIAL_MAXNUM];
    HBITMAP    hQuickDial[AB_QDIAL_MAXNUM];
}AB_QDIALDATA,*PAB_QDIALDATA;


#define AB_QDIAL_FRISTNUM       2


const char* const ab_quickdial[AB_QDIAL_MAXNUM] =
{
    "/rom/contact/quickdial2.bmp",
    "/rom/contact/quickdial3.bmp",
    "/rom/contact/quickdial4.bmp",
    "/rom/contact/quickdial5.bmp",
    "/rom/contact/quickdial6.bmp",
    "/rom/contact/quickdial7.bmp",
    "/rom/contact/quickdial8.bmp",
    "/rom/contact/quickdial9.bmp",
	//"/rom/not_available_43x28.bmp",
};

#define IDM_ABQDIAL_REMOVE      101

#define IDC_ASSIGN              100
#define IDC_EXIT                200
#define IDC_ABQDIAL_LIST        300

BOOL AB_CreateQDialWindow(HWND hWnd);
LRESULT ABQDialWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABQDial_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABQDial_OnActivate(HWND hwnd, UINT state);
static void ABQDial_OnSetFocus(HWND hwnd);
static void ABQDial_OnPaint(HWND hWnd);
static void ABQDial_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABQDial_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABQDial_OnDestroy(HWND hWnd);
static void ABQDial_OnClose(HWND hWnd);
static void ABQDial_OnAssign(HWND hWnd,int nTelID,DWORD id);
static void ABQDial_OnRemove(HWND hWnd,BOOL bRemove);

static BOOL AB_QDialGenArray(AB_QDIAL* Array);
int AB_GetSubfix(DWORD id);


BOOL AB_QDialRegisterClass(void)
{
    WNDCLASS    wc;

    wc.style         = 0;
    wc.lpfnWndProc   = ABQDialWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(AB_QDIALDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABQDialWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}

BOOL AB_CreateQDialWindow(HWND hFrameWnd)
{
    HWND        hQDialWnd;
	RECT		rcClient;
	AB_QDIALDATA Data;
    
	memset(&Data,0,sizeof(AB_QDIALDATA));

	Data.hFrameWnd = hFrameWnd;
     
	GetClientRect(hFrameWnd,&rcClient);

    Data.hMenu = CreateMenu();

    hQDialWnd = CreateWindow(
        "ABQDialWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
        hFrameWnd,
        NULL, 
        NULL, 
        (PVOID)&Data
        );
    
    if (!hQDialWnd)
    {
		DestroyMenu(Data.hMenu);

        return FALSE;
    }

    AppendMenu(Data.hMenu, MF_ENABLED, IDC_ASSIGN, IDS_ASSIGN);
    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABQDIAL_REMOVE, IDS_REMOVE);

	SetFocus(hQDialWnd);

	PDASetMenu(hFrameWnd,Data.hMenu);

	SetWindowText(hFrameWnd,IDS_QUICKDIAL);

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_BACK);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_ASSIGN,1), (LPARAM)IDS_ASSIGN);

    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);

    ShowWindow(hQDialWnd,SW_SHOW);
    UpdateWindow(hQDialWnd);

    return TRUE;
}

LRESULT ABQDialWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABQDial_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        ABQDial_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

	case WM_SETFOCUS:
        ABQDial_OnSetFocus(hWnd);
		break;

    case WM_PAINT:
        ABQDial_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABQDial_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABQDial_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABQDial_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABQDial_OnDestroy(hWnd);
        break;

	case WM_QDIALASSIGN:
        ABQDial_OnAssign(hWnd,(int)wParam,(DWORD)lParam);
		break;

	case WM_QDIALREMOVE:
        ABQDial_OnRemove(hWnd,(BOOL)lParam);
		break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABQDial_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABQDial_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    AB_QDIALDATA* pData;
    HWND hList;
    int  i,curIndex;
	int  nsubfix;
	CONTACT_ITEMCHAIN *pItem,*pTemp;
	int  nTelID;
    BOOL bExistNum;
    BOOL bExistName;
            
    pData = (AB_QDIALDATA*)GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(AB_QDIALDATA));

    GetClientRect(hWnd,&rect);
    
    hList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | LBS_BITMAP | WS_VSCROLL | LBS_MULTILINE,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_ABQDIAL_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;
    
    for(i = 0 ; i < AB_QDIAL_MAXNUM ; i++)
        pData->hQuickDial[i] = LoadImage(NULL, ab_quickdial[i], IMAGE_BITMAP,
        ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);

    AB_ReadQDial(pData->QDail);

    for(i = 0 ; i < AB_QDIAL_MAXNUM ; i++ )
    {
        bExistNum = FALSE;

        bExistName = FALSE;

        if(pData->QDail[i].nNO >= 2 && pData->QDail[i].nNO <= 9)
        {
			nsubfix = AB_GetSubfix(pData->QDail[i].id);

			if(nsubfix == -1)
			{
				pData->QDail[i].id = 0;
				pData->QDail[i].nTelID = 0;
				pData->QDail[i].nNO = 0;
				i--;
				continue;
			}

            bExistName = TRUE;

            curIndex = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[nsubfix]));	
            
			pItem = NULL;
            AB_ReadRecord(pIndexName[nsubfix]->dwoffset,&pItem,&nTelID);
			pTemp = pItem;
			while(pTemp)
			{
				if(AB_IsTel(pTemp->dwMask))
				{
					if(pTemp->nID == pData->QDail[i].nTelID)
					{
						SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(curIndex, -1), 
						(LPARAM)(pTemp->pszData));
                        bExistNum = TRUE;
						break;
					}
				}
				pTemp = pTemp->pNext;
			}
			Item_Erase(pItem);
        }

        if(bExistName == FALSE)
        {
            pData->QDail[i].nNO = 0;
            pData->QDail[i].id = 0;
            pData->QDail[i].nTelID = 0;
			curIndex = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_EMPTY);
        }

        if(bExistName == TRUE && bExistNum == FALSE)
        {
            SendMessage(hList,LB_DELETESTRING,curIndex,NULL);
            pData->QDail[i].nNO = 0;
            pData->QDail[i].id = 0;
            pData->QDail[i].nTelID = 0;
			curIndex = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_EMPTY);
        }

		SendMessage(hList,LB_SETITEMDATA,curIndex,(LPARAM)&(pData->QDail[i]));
        SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, curIndex),(LPARAM)pData->hQuickDial[i]);		
    }

    SendMessage(hList, LB_SETCURSEL, 0, 0);
    
    if(pData->QDail[0].nNO >= 2 && pData->QDail[0].nNO <= 9)
    {
        MEMORY_NAME *Name;
        
        Name = Memory_Find((PVOID)(pData->QDail[0].id),AB_FIND_ID);
        
        if(Name != NULL)
            SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
        else
            SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    }
    else
        SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        
    return TRUE;
    
}
/*********************************************************************\
* Function	ABQDial_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABQDial_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;
    AB_QDIALDATA* pData;
    int  nIndex;
            
    pData = (AB_QDIALDATA*)GetUserData(hwnd);

    hLst = GetDlgItem(hwnd,IDC_ABQDIAL_LIST);

	SetFocus(hLst);

	PDASetMenu(pData->hFrameWnd,pData->hMenu);

	SetWindowText(pData->hFrameWnd,IDS_QUICKDIAL);

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_ASSIGN);
    
    nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
    
    if(nIndex != LB_ERR)
    {    
        if(pData->QDail[nIndex].nNO >= 2 && pData->QDail[nIndex].nNO <= 9)
        {
            MEMORY_NAME *Name;
            
            Name = Memory_Find((PVOID)(pData->QDail[nIndex].id),AB_FIND_ID);
            
            if(Name != NULL)
                SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
            else
                SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        }
        else
            SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    }

    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);	
    return;
}
/*********************************************************************\
* Function	ABQDial_OnSetFocus
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABQDial_OnSetFocus(HWND hwnd)
{
    HWND hLst;
            
    hLst = GetDlgItem(hwnd,IDC_ABQDIAL_LIST);

	SetFocus(hLst);

    return;
}
/*********************************************************************\
* Function	ABQDial_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABQDial_OnPaint(HWND hWnd)
{

	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABQDial_OnKey
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
static void ABQDial_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    AB_QDIALDATA* pData;
            
    pData = (AB_QDIALDATA*)GetUserData(hWnd);
    switch (vk)
	{
	case VK_F10:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

	case VK_RETURN:
		PostMessage(hWnd,WM_COMMAND,IDC_ASSIGN,NULL);
		break;

	case VK_F5:  
        {
            int  nIndex;
            HWND hLst;
            
            hLst = GetDlgItem(hWnd,IDC_ABQDIAL_LIST);
            
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR)
                break;

            if(pData->QDail[nIndex].nNO >= 2 && pData->QDail[nIndex].nNO <= 9)
            {
                MEMORY_NAME *Name;

                Name = Memory_Find((PVOID)(pData->QDail[nIndex].id),AB_FIND_ID);

                if(Name != NULL)
                    PDADefWindowProc(pData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
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
* Function	ABQDial_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABQDial_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    AB_QDIALDATA* pData;
    
    pData = (AB_QDIALDATA*)GetUserData(hWnd);

	switch(id)
	{
    case IDC_EXIT:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

    case IDC_ASSIGN:
        {
            int  nIndex;
            HWND hLst;
            
            hLst = GetDlgItem(hWnd,IDC_ABQDIAL_LIST);
            
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR)
                break;
            
            ABCreateContactPickerWnd(pData->hFrameWnd,hWnd, IDS_QUICKDIAL, WM_QDIALASSIGN,PICK_NUMBER,FALSE,pData->QDail[nIndex].id,FALSE, FALSE);

        }
        break;

    case IDM_ABQDIAL_REMOVE:
		{		
            int  nIndex;
            HWND hLst;

			hLst = GetDlgItem(hWnd,IDC_ABQDIAL_LIST);

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR)
                break;

            if(pData->QDail[nIndex].nNO >= 2 && pData->QDail[nIndex].nNO <= 9)
            {
                MEMORY_NAME *Name;

                Name = Memory_Find((PVOID)(pData->QDail[nIndex].id),AB_FIND_ID);

                if(Name != NULL)
                    PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_REMOVEASSIGNMENT, Notify_Request, 
                    NULL/*AB_GetNameString(Name)*/, IDS_YES, IDS_NO,WM_QDIALREMOVE);                    
            }
		}
        break;

    case IDC_ABQDIAL_LIST:
        if(codeNotify == LBN_SELCHANGE)
        {
            int  nIndex;
            HWND hLst;
            
            hLst = GetDlgItem(hWnd,IDC_ABQDIAL_LIST);
            
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR)
                break;

            if(pData->QDail[nIndex].nNO >= 2 && pData->QDail[nIndex].nNO <= 9)
            {
                MEMORY_NAME *Name;

                Name = Memory_Find((PVOID)(pData->QDail[nIndex].id),AB_FIND_ID);

                if(Name != NULL)
                    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
                else
                    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            }
            else
                SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        }
        break;

    default:
        break;
    }
}
/*********************************************************************\
* Function	ABQDial_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABQDial_OnDestroy(HWND hWnd)
{

    AB_QDIALDATA* pData;

    int i;
    
    pData = (AB_QDIALDATA*)GetUserData(hWnd);
    
    for(i = 0 ; i < AB_QDIAL_MAXNUM ; i++)
        if(pData->hQuickDial[i])
            DeleteObject(pData->hQuickDial[i]);

	DestroyMenu(pData->hMenu);

    return;

}
/*********************************************************************\
* Function	ABQDial_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABQDial_OnClose(HWND hWnd)
{
    AB_QDIALDATA* pData;
    
    pData = (AB_QDIALDATA*)GetUserData(hWnd);

	SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow(hWnd);
    
    return;
}
/*********************************************************************\
* Function	ABQDial_OnAssign
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABQDial_OnAssign(HWND hWnd,int nTelID,DWORD id)
{
    HWND hLst;
	int  nIndex;
	AB_QDIAL* pDial;
	AB_QDIALDATA* pData;
	int  nsubfix;
	CONTACT_ITEMCHAIN *pItem,*pTemp;
	int  ntempid;

	if(nTelID == -1 || id == -1)
	{
		return;
	}

	pData = GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_ABQDIAL_LIST);

	nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);

	if(nIndex == LB_ERR)
		return;

	pDial = (AB_QDIAL*)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);

	nsubfix = AB_GetSubfix(id);
	if(nsubfix == -1)
		return;

	pDial->nNO = nIndex+AB_QDIAL_FRISTNUM;
	pDial->id = id;
	pDial->nTelID = nTelID;
	
	SendMessage(hLst,LB_SETTEXT,nIndex,(LPARAM)AB_GetNameString(pIndexName[nsubfix]));			
            
	pItem = NULL;
	AB_ReadRecord(pIndexName[nsubfix]->dwoffset,&pItem,&ntempid);
	pTemp = pItem;
	while(pTemp)
	{
		if(AB_IsTel(pTemp->dwMask))
		{
			if(pTemp->nID == nTelID)
			{
				SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), 
				(LPARAM)(pTemp->pszData));
				break;
			}
		}
		pTemp = pTemp->pNext;
	}
	Item_Erase(pItem);

	SendMessage(hLst,LB_SETCURSEL,nIndex,0);

	AB_WriteQDial(pData->QDail);

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
}
/*********************************************************************\
* Function	ABQDial_OnRemove
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABQDial_OnRemove(HWND hWnd,BOOL bRemove)
{
    HWND hLst;
	int  nIndex;
	AB_QDIAL* pDial;
	AB_QDIALDATA* pData;
	char szCaption[50];

	if(bRemove == FALSE)
		return;

	pData = GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_ABQDIAL_LIST);

	nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);

	if(nIndex == LB_ERR)
		return;

	pDial = (AB_QDIAL*)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);

	pDial->nNO = 0;
	pDial->id = 0;
	pDial->nTelID = 0;
	
	if(AB_WriteQDial(pData->QDail))
	{
		szCaption[0] = 0;
		GetWindowText(pData->hFrameWnd,szCaption,49);
        szCaption[49] = 0;
		PLXTipsWin(NULL,NULL,0,IDS_REMOVED,szCaption,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
		SendMessage(hLst,LB_SETTEXT,nIndex,(LPARAM)IDS_EMPTY);
		SendMessage(hLst,LB_SETAUXTEXT,nIndex,(LPARAM)"");
		
		SendMessage(hLst,LB_SETCURSEL,nIndex,0);
        
        SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
	}
}
/*********************************************************************\
* Function	AB_QDialGenArray
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL AB_QDialGenArray(AB_QDIAL* Array)
{
#ifdef _TEST_
    Array[0].bUsed = TRUE;
    strcpy(Array[0].szFirstName,"Q");
    strcpy(Array[0].szLastName,"Q");
    strcpy(Array[0].szTel,"1234567890");

    
    Array[5].bUsed = TRUE;
    strcpy(Array[5].szFirstName,"5wenmlkerjlrmtjretlkmer,ym;lretlrke");
    strcpy(Array[5].szLastName,"lairr8439j458f4mdfloermsdmd");
    strcpy(Array[5].szTel,"09876544323345398579857982572987598273598472587295872398472");
#endif
    return TRUE;
}
/*********************************************************************\
* Function	   AB_GetSubfix
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int AB_GetSubfix(DWORD id)
{
    int i;
    
    for( i = 1 ; i <= nName ; i++ )
    {
        if(id == ((MEMORY_NAME*)pIndexName[i])->id)
        {            
            return i;
        }
    }

    return -1;
}
