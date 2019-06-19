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

#include <project/plx_pdaex.h>
#include <string.h>

#define MYBTN_STATUS_FOCUS  1
#define MUBTN_TITLE_LEN     128

typedef struct
{
	BYTE  bState;			// if push button is highlight.
	BYTE  bFocus;
	BYTE  byStyle;			// style
    WORD  wID;
    char  szTitle[MUBTN_TITLE_LEN];
}MYBUTTONDATA, *PMYBUTTONDATA;


static LRESULT CALLBACK MYBUTTON_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void OnPaint(HWND hWnd);
static void OnKeyDown(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void OnSetFocus(HWND hWnd);
static void OnKillFocus(HWND hWnd);
static void OnDestroy(HWND hWnd);
static void OnSetText(HWND hWnd,LPCTSTR lpszText);


/**********************************************************************
 * Function     MYBUTTON_RegisterClass
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/
BOOL MYBUTTON_RegisterClass(void)
{
    WNDCLASS wc;

    wc.style            = CS_OWNDC;
    wc.lpfnWndProc      = MYBUTTON_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(MYBUTTONDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "MYBUTTON";

    return RegisterClass(&wc);
}
/**********************************************************************
 * Function     MYBUTTON_WndProc
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/
static LRESULT CALLBACK MYBUTTON_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_PAINT:
        OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        OnKeyDown(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_SETFOCUS:
        OnSetFocus(hWnd);
        break;

    case WM_KILLFOCUS:
        OnKillFocus(hWnd);
        break;

    case WM_DESTROY:
        OnDestroy(hWnd);
        break;

    case WM_SETTEXT:
        OnSetText(hWnd,(LPCTSTR)(lParam));        
    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;
}

#define HIGHLIGHT_COLOR (RGB(0, 0, 255))

/**********************************************************************
 * Function     OnCreate
 * Purpose      
 * Params       HWND hWnd
 * Return       
 * Remarks      
 **********************************************************************/
static BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    MYBUTTONDATA *pMyButtonData;

    pMyButtonData = (MYBUTTONDATA*)GetUserData(hWnd);

    memset(pMyButtonData, 0, sizeof(MYBUTTONDATA));

    pMyButtonData->byStyle = (BYTE)(LOWORD(lpCreateStruct->style) & 0x000F);
    pMyButtonData->wID = (WORD)(lpCreateStruct->hMenu);
    
    if(strlen(lpCreateStruct->lpszName) < MUBTN_TITLE_LEN)
        strcpy(pMyButtonData->szTitle,lpCreateStruct->lpszName);
    
    return TRUE;         
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
    HWND    hwndParent = NULL;
    MYBUTTONDATA *pMyButtonData;

    
    switch (vk)
    {   
    case VK_RETURN: 
        
        SendMessage(GetParent(hWnd),WM_KEYDOWN,vk,MAKELPARAM(cRepeat,flags));
        
        break;
        
    case VK_UP:
           
        hwndParent = GetParent(hWnd);
        if (hwndParent == NULL)
            return;
        
        if (GetWindowLong(hWnd, GWL_STYLE) & CS_NOSYSCTRL)
        {
            SendMessage(hwndParent, WM_KEYDOWN, vk,MAKELPARAM(cRepeat,flags));
            return;
        }

        SetFocus(GetNextDlgTabItem(hwndParent, hWnd, TRUE));
        
        break;
        
    case VK_DOWN:
        
        hwndParent = GetParent(hWnd);
        if (hwndParent == NULL)
            return;
        
        if (GetWindowLong(hWnd, GWL_STYLE) & CS_NOSYSCTRL)
        {
            SendMessage(hwndParent, WM_KEYDOWN, vk,MAKELPARAM(cRepeat,flags));
            return;
        }

        SetFocus(GetNextDlgTabItem(hwndParent, hWnd, FALSE));
        
        break;
        
    case VK_F5:

        pMyButtonData = (MYBUTTONDATA*)GetUserData(hWnd);
        
        hwndParent = GetParent(hWnd);
        if (hwndParent == NULL)
            return;
        
        SendMessage(hwndParent, WM_COMMAND, 
            MAKELONG(pMyButtonData->wID, BN_CLICKED), (LPARAM)hWnd);
        
        break;
        
    case VK_F2:
    case VK_F10:
        
        hwndParent = GetParent(hWnd);
        
        SendMessage(hwndParent, WM_KEYDOWN, vk,MAKELPARAM(cRepeat,flags));
                
    default :
        
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
    MYBUTTONDATA *pMyButtonData;
    HDC           hdc = NULL;
    PAINTSTRUCT   ps;
    RECT          rc;
    int           nMode = 0;
    COLORREF      cr = 0;

    pMyButtonData = (MYBUTTONDATA*)GetUserData(hWnd);

    hdc = BeginPaint(hWnd, &ps);
    
    GetClientRect(hWnd, &rc);

    if (pMyButtonData->bFocus & MYBTN_STATUS_FOCUS)
    {
        ClearRect(hdc, &rc, HIGHLIGHT_COLOR);
        cr = SetTextColor(hdc, COLOR_WHITE);
    }
    else
    {
        ClearRect(hdc, &rc, COLOR_WHITE);
    }

    if (pMyButtonData->szTitle != NULL)
    {
        GetClientRect(hWnd, &rc);
        rc.left += 4;

        nMode = SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, pMyButtonData->szTitle, -1, &rc,
            DT_LEFT | DT_VCENTER);
        SetBkMode(hdc, nMode);
    }

    if (pMyButtonData->bFocus & MYBTN_STATUS_FOCUS)
    {
        SetTextColor(hdc, cr);
    }
    
    EndPaint(hWnd, &ps);
}
/**********************************************************************
 * Function     OnSetFocus
 * Purpose      
 * Params       HWND hWnd
 * Return       
 * Remarks      
**********************************************************************/
static void OnSetFocus(HWND hWnd)
{
    
    MYBUTTONDATA *pMyButtonData;
    
    pMyButtonData = (MYBUTTONDATA*)GetUserData(hWnd);
    
    pMyButtonData->bFocus = MYBTN_STATUS_FOCUS; 
    
    InvalidateRect(hWnd,NULL,TRUE);
}
/**********************************************************************
 * Function     OnKillFocus
 * Purpose      
 * Params       HWND hWnd
 * Return       
 * Remarks      
**********************************************************************/
static void OnKillFocus(HWND hWnd)
{
    
    MYBUTTONDATA *pMyButtonData;
    
    pMyButtonData = (MYBUTTONDATA*)GetUserData(hWnd);
    
    pMyButtonData->bFocus = 0; 
    
    InvalidateRect(hWnd,NULL,TRUE);
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
	UnregisterClass("MYBUTTON", NULL);
}
/**********************************************************************
 * Function     OnSetText
 * Purpose      
 * Params       HWND hWnd, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/
static void OnSetText(HWND hWnd,LPCTSTR lpszText)
{
    MYBUTTONDATA *pMyButtonData;
    
    pMyButtonData = (MYBUTTONDATA*)GetUserData(hWnd);

    if(strlen(lpszText) < MUBTN_TITLE_LEN)
        strcpy(pMyButtonData->szTitle,lpszText);
}
