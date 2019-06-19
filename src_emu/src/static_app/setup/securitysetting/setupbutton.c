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

#include "plx_pdaex.h"
#include "string.h"

#define SETUPBTN_STATUS_FOCUS  1
#define MUBTN_TITLE_LEN     128


typedef struct
{
	BYTE  bState;			// if push button is highlight.
	BYTE  bFocus;
	BYTE  byStyle;			// style
    WORD  wID;
	BOOL  bArrow;
    char  szTitle[MUBTN_TITLE_LEN];
	char  szValue[MUBTN_TITLE_LEN];
	HBITMAP hBmpFocus;
	HBITMAP hBmpLeft;
	HBITMAP hBmpRight;
}SETUPBUTTONDATA, *PSETUPBUTTONDATA;


static LRESULT CALLBACK SETUPBUTTON_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void OnPaint(HWND hWnd);
static void OnSize(HWND hWnd);
static void OnKeyDown(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void OnSetFocus(HWND hWnd);
static void OnKillFocus(HWND hWnd);
static void OnDestroy(HWND hWnd);
static void OnSetText(HWND hWnd,LPCTSTR lpszText);

extern BOOL GetFontHandle(HFONT *hFont, int nType);
//static HBITMAP  hBgFocus, hLeftBmp, hRightBmp;
/**********************************************************************
 * Function     SETUPBUTTON_RegisterClass
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/
BOOL SETUPBUTTON_RegisterClass(void)
{
    WNDCLASS wc;

    wc.style            = CS_OWNDC;
    wc.lpfnWndProc      = SETUPBUTTON_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(SETUPBUTTONDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "SETUPBUTTON";

    return RegisterClass(&wc);
}
/**********************************************************************
 * Function     SETUPBUTTON_WndProc
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/
static LRESULT CALLBACK SETUPBUTTON_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
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

	case WM_KEYUP:
		if (GetWindowLong(hWnd, GWL_STYLE) & CS_NOSYSCTRL)
        {
			HWND    hwndParent = NULL;
			
			hwndParent = GetParent(hWnd);
			if (hwndParent == NULL)
				break;

            SendMessage(hwndParent, WM_KEYUP, LOWORD(wParam),0);
        }
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
	case WM_SIZE:
	   // OnSize(hWnd);
		break;

    case WM_SETTEXT:
        OnSetText(hWnd,(LPCTSTR)(lParam));   
	case WM_SETVALUE:
		{
			SETUPBUTTONDATA *pSetupButtonData;
			
			if(lParam == NULL)
				break;

			pSetupButtonData = (SETUPBUTTONDATA*)GetUserData(hWnd);
			
			if(strlen((char*)lParam) < MUBTN_TITLE_LEN)
				strcpy(pSetupButtonData->szValue, (char*)lParam);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
		}
		break;
	case WM_SETARROW:
		{
			SETUPBUTTONDATA *pSetupButtonData;
			
			pSetupButtonData = (SETUPBUTTONDATA*)GetUserData(hWnd);

			if(pSetupButtonData)
				pSetupButtonData->bArrow = TRUE;
		}
		break;
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
    SETUPBUTTONDATA *pSetupButtonData;
    HDC             hdc;
	COLORREF Color;
	BOOL	bTran;

    pSetupButtonData = (SETUPBUTTONDATA*)GetUserData(hWnd);

    memset(pSetupButtonData, 0, sizeof(SETUPBUTTONDATA));

    pSetupButtonData->byStyle = (BYTE)(LOWORD(lpCreateStruct->style) & 0x000F);
    pSetupButtonData->wID = (WORD)(lpCreateStruct->hMenu);
    
    if(strlen(lpCreateStruct->lpszName) < MUBTN_TITLE_LEN)
        strcpy(pSetupButtonData->szTitle,lpCreateStruct->lpszName);

   	hdc = GetDC(hWnd);
	pSetupButtonData->hBmpFocus = LoadImage(NULL, "/rom/progman/listfocus.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	pSetupButtonData->hBmpLeft = CreateBitmapFromImageFile(hdc,"/rom/progman/strspinleft.bmp",&Color, &bTran);
	pSetupButtonData->hBmpRight = CreateBitmapFromImageFile(hdc,"/rom/progman/strspinright.bmp",&Color, &bTran);
	ReleaseDC(hWnd, hdc);

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
    SETUPBUTTONDATA *pSetupButtonData;

    
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

        pSetupButtonData = (SETUPBUTTONDATA*)GetUserData(hWnd);
        
        hwndParent = GetParent(hWnd);
        if (hwndParent == NULL)
            return;
        
        SendMessage(hwndParent, WM_COMMAND, 
            MAKELONG(pSetupButtonData->wID, BN_CLICKED), (LPARAM)hWnd);
        
        break;
        
    case VK_F2:
    case VK_F10:
        
        hwndParent = GetParent(hWnd);
        
        SendMessage(hwndParent, WM_KEYDOWN, vk,MAKELPARAM(cRepeat,flags));
		break;
	case VK_LEFT:
	case VK_RIGHT:
		{
			hwndParent = GetParent(hWnd);
			
			SendMessage(hwndParent, WM_KEYDOWN, vk,MAKELPARAM(cRepeat,flags));
			
		}
		break;
                
    default :
        
        break;
        
    }

}
/**********************************************************************
 * Function     OnSize
 * Purpose      
 * Params       HWND hWnd
 * Return       
 * Remarks      
 **********************************************************************/
static void OnSize(HWND hWnd)
{  
    
    HDC           hdc = NULL;
    RECT          rc;

    hdc = GetDC(hWnd);
    GetClientRect(hWnd, &rc);
    MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom- rc.top,TRUE);
    ReleaseDC(hWnd, hdc);

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
    SETUPBUTTONDATA *pSetupButtonData;
    HDC           hdc = NULL;
    PAINTSTRUCT   ps;
    RECT          rc, rcText, rcIcon;
    int           nMode = 0;
    COLORREF      cr = 0, bcr;
	HFONT         hSmallFont, hOldFont;
	BITMAP        bmp;
	//RECT		  rcClient;

    pSetupButtonData = (SETUPBUTTONDATA*)GetUserData(hWnd);

    hdc = BeginPaint(hWnd, &ps);
    
    GetClientRect(hWnd, &rc);

    if (pSetupButtonData->bFocus & SETUPBTN_STATUS_FOCUS)
    {
		if(pSetupButtonData->hBmpFocus!=NULL)
		{
			GetObject((HDC)pSetupButtonData->hBmpFocus, sizeof(BITMAP), (void*)&bmp);
			StretchBlt(hdc, rc.left, rc.top, rc.right - rc.left,
				rc.bottom - rc.top, (HDC)pSetupButtonData->hBmpFocus, 0, 0, bmp.bmWidth,
				bmp.bmHeight, ROP_SRC);
		}
		cr = SetTextColor(hdc, COLOR_WHITE);
    }
    else
    {
		SetTextColor(hdc, cr);
    }

    if (pSetupButtonData->szTitle != NULL)
    {
        rc.left += 4;
		SetRect(&rcText, rc.left, rc.top+20, rc.right,rc.bottom);
		rc.bottom = rc.top+25;

        nMode = SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, ML(pSetupButtonData->szTitle), -1, &rc,
            DT_LEFT | DT_VCENTER);

		GetFontHandle(&hSmallFont, SMALL_FONT);

		hOldFont = SelectObject(hdc, hSmallFont);
		if(pSetupButtonData->szValue[0] != 0)
			DrawText(hdc, ML(pSetupButtonData->szValue), -1, &rcText, DT_LEFT|DT_VCENTER);

		if(pSetupButtonData->bArrow && (pSetupButtonData->bFocus & SETUPBTN_STATUS_FOCUS))
		{
			rcIcon.left = rc.left + 140;
			rcIcon.top = rcText.top + 10;
			rcIcon.right = rcIcon.left + 10;
			rcIcon.bottom = rcIcon.top + 10;

			if(pSetupButtonData->hBmpLeft!=NULL && pSetupButtonData->hBmpRight != NULL)
			{
				SetBkMode(hdc, NEWTRANSPARENT);
				bcr = SetBkColor(hdc, RGB(0, 0, 0));
				BitBlt(hdc, rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom, 
				(HDC)pSetupButtonData->hBmpLeft, 0, 0, ROP_SRC);
				BitBlt(hdc, rcIcon.right + 2, rcIcon.top, rcIcon.right +12, rcIcon.bottom, 
				(HDC)pSetupButtonData->hBmpRight, 0, 0, ROP_SRC);
			}
		}
		SelectObject(hdc, hOldFont);
    }

//    if (pSetupButtonData->bFocus & SETUPBTN_STATUS_FOCUS)
//    {
//        SetTextColor(hdc, cr);
//    }
	SetBkColor(hdc,bcr);
	SetBkMode(hdc, nMode);
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
    
    SETUPBUTTONDATA *pSetupButtonData;
    
    pSetupButtonData = (SETUPBUTTONDATA*)GetUserData(hWnd);
    
    pSetupButtonData->bFocus = SETUPBTN_STATUS_FOCUS; 
    
    InvalidateRect(hWnd,NULL,TRUE);
	UpdateWindow(hWnd);
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
    
    SETUPBUTTONDATA *pSetupButtonData;
    
    pSetupButtonData = (SETUPBUTTONDATA*)GetUserData(hWnd);
    
    pSetupButtonData->bFocus = 0; 
    
    InvalidateRect(hWnd,NULL,TRUE);
	UpdateWindow(hWnd);
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
	SETUPBUTTONDATA *pSetupButtonData;

    pSetupButtonData = (SETUPBUTTONDATA*)GetUserData(hWnd);

	if(pSetupButtonData)
	{
		DeleteObject(pSetupButtonData->hBmpFocus);
		DeleteObject(pSetupButtonData->hBmpLeft);
		DeleteObject(pSetupButtonData->hBmpRight);	
	}

	UnregisterClass("SETUPBUTTON", NULL);
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
    SETUPBUTTONDATA *pSetupButtonData;
    
    pSetupButtonData = (SETUPBUTTONDATA*)GetUserData(hWnd);

    if(strlen(lpszText) < MUBTN_TITLE_LEN)
        strcpy(pSetupButtonData->szTitle,lpszText);
}
