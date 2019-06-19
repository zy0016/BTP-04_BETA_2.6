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

#define IDC_EXIT                200
#define IDC_ABSEARCH_EDIT       300

#define AB_SEARCHEDIT_MAXLEN    (100+1)
#define AB_SEARCH_CAPTIONBK     RGB(163,176,229)

#define AB_SEARCH_TIMER         600

typedef struct tagABSearchData
{
    HWND hFrameWnd;
    HWND hMsgWnd;
    UINT wMsgCmd;
    BOOL bDealArrow;
    HPEN   hNewPen;
    HBRUSH hNewBrush;
    HBITMAP hBmpSearch;
    char szSearch[AB_SEARCHEDIT_MAXLEN];
    WNDPROC OldListWndProc;
}ABSEARCHDATA,*PABSEARCHDATA;


BOOL AB_CreateSearchPopUpWnd(HWND hFrameWnd,HWND hMsgWnd,UINT wMsgCmd,BOOL bDealArrow);
LRESULT ABSearchPopUpWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL ABSearch_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABSearch_OnActivate(HWND hwnd, UINT state);
static void ABSearch_OnPaint(HWND hWnd);
static void ABSearch_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABSearch_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABSearch_OnDestroy(HWND hWnd);
static void ABSearch_OnClose(HWND hWnd);
static void ABSearch_OnSetFocus(HWND hWnd);
static LRESULT CallEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

BOOL AB_CreateSearchPopUpWnd(HWND hFrameWnd,HWND hMsgWnd,UINT wMsgCmd,BOOL bDealArrow)
{
    WNDCLASS    wc;
    HWND        hABSearchWnd;
    ABSEARCHDATA CreateData;
    RECT        rcClient;
    int         nY,nHeight,nWidth;
    
    wc.style         = 0;
    wc.lpfnWndProc   = ABSearchPopUpWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(ABSEARCHDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABSearchPopUpWndClass";
    
    if (!RegisterClass(&wc))
        return NULL;
     
    memset(&CreateData,0,sizeof(ABSEARCHDATA));

    CreateData.hFrameWnd = hFrameWnd;
    CreateData.hMsgWnd = hMsgWnd;
    CreateData.wMsgCmd = wMsgCmd;
    CreateData.bDealArrow = bDealArrow;

    GetClientRect(hFrameWnd,&rcClient);

    nHeight =  (rcClient.bottom - rcClient.top) * 2 / 5;
    nY = (rcClient.bottom - rcClient.top) * 3 / 5;
    nWidth = rcClient.right - rcClient.left;
    
    hABSearchWnd = CreateWindow(
        "ABSearchPopUpWndClass",
        "", 
        WS_VISIBLE | WS_CHILD | WS_BORDER, 
        rcClient.left,
        nY,
        nWidth,
        nHeight,
        hFrameWnd,
        NULL, 
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!hABSearchWnd)
    {
        UnregisterClass("ABSearchPopUpWndClass",NULL);
        return NULL;
    }

    ShowWindow(hABSearchWnd,SW_SHOW);
    UpdateWindow(hABSearchWnd);

    SetFocus(hABSearchWnd);
        
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    return TRUE;
}

LRESULT ABSearchPopUpWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABSearch_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABSearch_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABSearch_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABSearch_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABSearch_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABSearch_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABSearch_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABSearch_OnDestroy(hWnd);
        break;

    case WM_TIMER:
        {
            PABSEARCHDATA pData;

            KillTimer(hWnd,(int)wParam);

            pData = GetUserData(hWnd);
            switch((int)wParam)
            {
            case 1:
                SendMessage(pData->hMsgWnd,pData->wMsgCmd,(WPARAM)(pData->szSearch),
                    (LPARAM)MAKEWPARAM(strlen(pData->szSearch),FALSE));
                break;
                
            default:
                break;
            }
        }
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABSearch_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABSearch_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    PABSEARCHDATA pData;
    HWND hEdit;
    int  nY,nX,nWidth,nHeight;
    IMEEDIT ie;
    
    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(ABSEARCHDATA));
    
    GetClientRect(hWnd,&rect);

    pData->hNewBrush = CreateBrush(BS_SOLID,AB_SEARCH_CAPTIONBK,NULL);
    
    pData->hNewPen = CreatePen(PS_SOLID,0,AB_SEARCH_CAPTIONBK);

    pData->hBmpSearch = LoadImage(NULL, AB_BMP_SEARCH, IMAGE_BITMAP,ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);

    nWidth = (rect.right - rect.left) * 9 / 10;
    nHeight = (rect.bottom - rect.top) / 2;
    nY = rect.top + nHeight /*+ 1*/;
    nX = (rect.right - rect.left) / 20;   
	
	memset(&ie, 0, sizeof(IMEEDIT));
	
	ie.hwndNotify	= (HWND)hWnd;    
	ie.dwAttrib	    = IME_ATTRIB_BASIC;                
	ie.dwAscTextMax	= 0;
	ie.dwUniTextMax	= 0;
	ie.wPageMax	    = 0;        
	ie.pszCharSet	= NULL;
	ie.pszTitle	    = NULL;
	ie.pszImeName	= NULL;
	
	hEdit = CreateWindow(
		"IMEEDIT",
		"",
		WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | CS_NOSYSCTRL,
        nX,
        nY,
        nWidth,
        nHeight/*-2*/,
		hWnd,
		(HMENU)IDC_ABSEARCH_EDIT,
		NULL,
		(PVOID)&ie);
	
	if(hEdit == NULL)
		return FALSE;   

    pData->OldListWndProc = (WNDPROC)SetWindowLong(hEdit,GWL_WNDPROC,(LONG)CallEditWndProc);

    SendMessage(hEdit,EM_LIMITTEXT,AB_SEARCHEDIT_MAXLEN-1,0);
        
    return TRUE;
    
}
/*********************************************************************\
* Function	ABSearch_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSearch_OnActivate(HWND hwnd, UINT state)
{
    HWND hEdt;
    PABSEARCHDATA pData;
    
    pData = GetUserData(hwnd);

    hEdt = GetDlgItem(hwnd,IDC_ABSEARCH_EDIT);

    SetFocus(hEdt);

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    return;
}
/*********************************************************************\
* Function	ABSearch_OnSetFocus
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSearch_OnSetFocus(HWND hWnd)
{
    HWND hEdt;

    hEdt = GetDlgItem(hWnd,IDC_ABSEARCH_EDIT);

    SetFocus(hEdt);

    return;
}
/*********************************************************************\
* Function	ABSearch_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSearch_OnPaint(HWND hWnd)
{
    RECT rcClient,rcCaption;
    int  nWidth,nHeight,nMode;
    HBRUSH hOldBrush;
    HPEN   hOldPen;
    PABSEARCHDATA pData;
    COLORREF oldcolor;

    HDC hdc = BeginPaint(hWnd, NULL);
    
    pData = GetUserData(hWnd);
    
    GetClientRect(hWnd, &rcClient);

    nWidth = rcClient.right - rcClient.left;
    
    nHeight = rcClient.bottom - rcClient.top;

    SetRect(&rcCaption,rcClient.left,rcClient.top,rcClient.right,nHeight/2);
    
    hOldBrush = SelectObject(hdc,pData->hNewBrush);

    hOldPen = SelectObject(hdc,pData->hNewPen);

    DrawRect(hdc,&rcCaption);

    SelectObject(hdc,hOldBrush);

    SelectObject(hdc,hOldPen);

    oldcolor = SetBkColor(hdc,RGB(0,0,0));

    nMode = SetBkMode(hdc,NEWTRANSPARENT);
    
    if(pData->hBmpSearch != NULL)
        BitBlt(hdc,(nWidth-ICON_WIDTH)/2,  (nHeight/2 - ICON_HEIGHT)/2, ICON_WIDTH, ICON_HEIGHT, (HDC)pData->hBmpSearch, 0, 0, SRCCOPY);
    
    SetBkMode(hdc,nMode);
    
    SetBkColor(hdc,oldcolor);

    EndPaint(hWnd, NULL);
    
    return;
}

/*********************************************************************\
* Function	ABSearch_OnKey
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
static void ABSearch_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PABSEARCHDATA pData;
    
    pData = GetUserData(hWnd);    
	switch (vk)
	{
    case VK_F10:
		SendMessage(pData->hMsgWnd,pData->wMsgCmd,(WPARAM)(pData->szSearch),
			(LPARAM)MAKEWPARAM(0,TRUE));
        SendMessage(hWnd,WM_CLOSE,0,0);    
        break;

    case VK_BACK:
		SendMessage(pData->hMsgWnd,pData->wMsgCmd,(WPARAM)(pData->szSearch),
			(LPARAM)MAKEWPARAM(strlen(pData->szSearch),FALSE));
        break;

	case VK_RETURN:
	case VK_F5:
    case VK_UP:
    case VK_DOWN:
		SendMessage(pData->hMsgWnd,pData->wMsgCmd,(WPARAM)(pData->szSearch),
			(LPARAM)MAKEWPARAM(strlen(pData->szSearch),TRUE));
		SendMessage(hWnd,WM_CLOSE,0,0);
		break;

    case VK_RIGHT:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		PostMessage(GetDlgItem(pData->hFrameWnd,IDC_AB_MAIN),WM_KEYDOWN,vk,MAKELPARAM(cRepeat, flags));
        break;

    case VK_F1://Dial number waiting
		PostMessage(pData->hMsgWnd,WM_KEYDOWN,vk,MAKELPARAM(cRepeat, flags));
        break;
        
	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABSearch_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABSearch_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    PABSEARCHDATA pData;
    HWND hEdt;
    
    pData = GetUserData(hWnd);

	switch(id)
	{
    case IDC_EXIT:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

    case IDC_ABSEARCH_EDIT:
        if(codeNotify == EN_CHANGE)
        {           
            hEdt = GetDlgItem(hWnd,IDC_ABSEARCH_EDIT);

            GetWindowText(hEdt,pData->szSearch,AB_SEARCHEDIT_MAXLEN);
        }
        break;

    default:
        break;
    }
}
/*********************************************************************\
* Function	ABSearch_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSearch_OnDestroy(HWND hWnd)
{
    PABSEARCHDATA pData;
    
    pData = GetUserData(hWnd);

    if(pData->hNewBrush)
        DeleteObject(pData->hNewBrush);

    if(pData->hNewPen)
        DeleteObject(pData->hNewPen);

    if(pData->hBmpSearch)
        DeleteObject(pData->hBmpSearch);
    
	UnregisterClass("ABSearchPopUpWndClass",NULL);

    return;

}
/*********************************************************************\
* Function	ABSearch_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSearch_OnClose(HWND hWnd)
{
    PABSEARCHDATA pData;
    
    pData = GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);

    return;

}
/*********************************************************************\
* Function	CallEditWndProc
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static LRESULT CallEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    PABSEARCHDATA pData;
    HWND hParent;

    hParent = GetParent(hWnd);
    
    pData = (PABSEARCHDATA)GetUserData(hParent);
        
    switch(wMsgCmd)
    {         
    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        { 
        case VK_RIGHT:
            if(pData->bDealArrow)
                PostMessage(hParent,wMsgCmd,wParam,lParam);
            return lResult;  
            
        case VK_LEFT:
            return lResult;
			//return CallWindowProc(pData->OldListWndProc, hWnd, wMsgCmd, wParam, lParam);

        case VK_F10:
            if(strlen(pData->szSearch) == 1)
                PostMessage(hParent,wMsgCmd,wParam,lParam);
            else
                PostMessage(hParent,wMsgCmd,VK_BACK,NULL);
            return CallWindowProc(pData->OldListWndProc, hWnd, wMsgCmd, wParam, lParam);
              
        default:
            return CallWindowProc(pData->OldListWndProc, hWnd, wMsgCmd, wParam, lParam);
        }
        break;
        
    case WM_CHAR:
        SetTimer(hParent,1,AB_SEARCH_TIMER,NULL);
        return CallWindowProc(pData->OldListWndProc, hWnd, wMsgCmd, wParam, lParam);
           
    default:
        return CallWindowProc(pData->OldListWndProc, hWnd, wMsgCmd, wParam, lParam);
    }
    return lResult;       
}
