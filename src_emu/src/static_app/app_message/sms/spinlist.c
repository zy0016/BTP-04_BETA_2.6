/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 *    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
 *                       All Rights Reserved
 *
 * Module   :  spinex.c
 *
 * Purpose  :  
 *
\**************************************************************************/

#include "smsglobal.h"

#define IDC_SPIN_LIST   300

BOOL SPINLIST_RegisterClass(void);
static LRESULT CALLBACK SPINLIST_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void OnShowWindow(HWND hWnd);
static void OnSetFocus(HWND hWnd);
static void OnPaint(HWND hWnd);
static void OnKeyDown(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void OnDestroy(HWND hWnd);
static void OnClose(HWND hWnd);

typedef struct tagSP_CreateData
{
	HWND hFrameWnd;
    HWND hWndFocus;
    HBITMAP hSelected;
    HBITMAP hNormal;
}SP_CREATEDATA, *PSP_CREATEDATA;


BOOL CreateSpinList(HWND hFrameWnd,HWND hWndFocus)
{
    HWND     hListWnd = NULL;
    RECT     rc;
    char     szTitle[50];
    SP_CREATEDATA Data;

    memset(&Data,0,sizeof(SP_CREATEDATA));

    SPINLIST_RegisterClass();

    Data.hFrameWnd = hFrameWnd;
    Data.hWndFocus = hWndFocus;

    GetClientRect(hFrameWnd,&rc);
        
    hListWnd = CreateWindow(
        "SPINLIST", 
        "",
        WS_VISIBLE | WS_CHILD,
        rc.left,  
        rc.top,  
        rc.right - rc.left,  
        rc.bottom - rc.top,  
        hFrameWnd,
        NULL,
        NULL, 
        (PVOID)&Data
        );
    
    if (!hListWnd)
    {
        UnregisterClass("SPINLIST", NULL);
        return FALSE;
    }

    SetFocus(hListWnd);

    SendMessage(hWndFocus,SSBM_GETTITLE,49,(LPARAM)szTitle);

    SetWindowText(hFrameWnd,szTitle);

    SendMessage(hFrameWnd,PWM_SETBUTTONTEXT,0,(LPARAM)IDS_CANCEL);
    SendMessage(hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
    SendMessage(hFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)ICON_SELECT);

    return TRUE;
}
/**********************************************************************
 * Function     SPINLIST_RegisterClass
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/
BOOL SPINLIST_RegisterClass(void)
{
    WNDCLASS wc;

    wc.style            = CS_OWNDC;
    wc.lpfnWndProc      = SPINLIST_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(SP_CREATEDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "SPINLIST";

    return RegisterClass(&wc);
}
/**********************************************************************
 * Function     SPINLIST_WndProc
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/
static LRESULT CALLBACK SPINLIST_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case PWM_SHOWWINDOW:
        OnShowWindow(hWnd);
        break;

    case WM_SETFOCUS:
        OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        OnKeyDown(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_DESTROY:
        OnDestroy(hWnd);
        break;

    case WM_CLOSE:
        OnClose(hWnd);  
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;
}
/**********************************************************************
 * Function     OnCreate
 * Purpose      
 * Params       HWND hWnd
 * Return       
 * Remarks      
 **********************************************************************/
static BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    RECT  rect;
    HWND  hList;
    int   nCount = 0,nSel = 0,i;
    char  szText[50];
    PSP_CREATEDATA pData;
    SIZE size;

    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(SP_CREATEDATA));

    GetClientRect(hWnd,&rect);
    
    hList = CreateWindow(
        "LISTBOX", 
        "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
        rect.left,
        rect.top,
        rect.right-rect.left,
        rect.bottom-rect.top, 
        hWnd, 
        (HMENU)IDC_SPIN_LIST, 
        NULL, 
        NULL);
    
    if(hList == NULL)
        return FALSE;
    
    GetImageDimensionFromFile(SMS_BMP_SELECT,&size);

    pData->hSelected = LoadImage(NULL, SMS_BMP_SELECT, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);
    
    GetImageDimensionFromFile(SMS_BMP_NORMAL,&size);

    pData->hNormal = LoadImage(NULL, SMS_BMP_NORMAL, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);

    nCount = SendMessage(pData->hWndFocus,SSBM_GETCOUNT,0,0);

    for(i = 0 ; i < nCount ; i++)
    {
        szText[0] = 0;
        SendMessage(pData->hWndFocus,SSBM_GETTEXT,i,(LPARAM)szText);
        SendMessage(hList,LB_INSERTSTRING,i,(LPARAM)szText);
        SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,i),(LPARAM)pData->hNormal);
    }

    nSel = SendMessage(pData->hWndFocus,SSBM_GETCURSEL,0,0);
    SendMessage(hList,LB_SETCURSEL,nSel,NULL);
    SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,nSel),(LPARAM)pData->hSelected);

    SetFocus(hList);
    
    return TRUE;         
}
/**********************************************************************
 * Function     OnShowWindow
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/
static void OnShowWindow(HWND hWnd)
{
    char  szTitle[50];
    HWND  hlist;
    PSP_CREATEDATA pData;

    pData = GetUserData(hWnd);
    
    hlist = GetDlgItem(hWnd,IDC_SPIN_LIST);

    SetFocus(hlist);

    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,0,(LPARAM)IDS_CANCEL);
    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)ICON_SELECT);

    SendMessage(pData->hWndFocus,SSBM_GETTITLE,49,(LPARAM)szTitle);
    SetWindowText(pData->hFrameWnd,szTitle);

    
}
/**********************************************************************
 * Function     OnKeyDown
 * Purpose      
 * Params       HWND hWnd, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/
static void OnKeyDown(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    int   nSel,nOldSel;
    HWND  hlist;
    PSP_CREATEDATA pData;

    pData = GetUserData(hWnd);
   
    hlist = GetDlgItem(hWnd,IDC_SPIN_LIST);

    switch (vk)
    {   
    case VK_F5:
        nSel = SendMessage(hlist,LB_GETCURSEL,0,0);
        SendMessage(hlist,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,nSel),(LPARAM)pData->hSelected);
        nOldSel = SendMessage(pData->hWndFocus,SSBM_GETCURSEL,0,0);
        SendMessage(hlist,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,nOldSel),(LPARAM)pData->hNormal);
        SendMessage(pData->hWndFocus,SSBM_SETCURSEL,nSel,0);
        SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);
        PostMessage(hWnd, WM_CLOSE,0,0);
        break;

    case VK_F10:
        SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);
        PostMessage(hWnd, WM_CLOSE,0,0);
        break;
                
    default:
        PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));        
        break;
    }
}
/**********************************************************************
 * Function     OnPaint
 * Purpose      
 * Params       HWND hWnd
 * Return       
 * Remarks      
 **********************************************************************/
static void OnPaint(HWND hWnd)
{  
    PAINTSTRUCT   ps;
    HDC hdc;
   
    hdc = BeginPaint(hWnd, &ps);
    
    EndPaint(hWnd, &ps);
}
/*********************************************************************\
* Function	OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void OnSetFocus(HWND hWnd)
{
    HWND  hlist;
    
    hlist = GetDlgItem(hWnd,IDC_SPIN_LIST);

    SetFocus(hlist);
}
/*********************************************************************\
* Function	OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void OnDestroy(HWND hWnd)
{

    PSP_CREATEDATA pData;

    pData = GetUserData(hWnd);

    if(pData->hSelected)
        DeleteObject(pData->hSelected);

    if(pData->hNormal)
        DeleteObject(pData->hNormal);

	UnregisterClass("SPINLIST", NULL);
}
/*********************************************************************\
* Function	OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void OnClose(HWND hWnd)
{
    DestroyWindow(hWnd);
}
