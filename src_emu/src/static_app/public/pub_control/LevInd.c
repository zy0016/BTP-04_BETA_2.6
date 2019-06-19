 /***************************************************************************
*
*                      Pollex Mobile Platform
*
*    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
*                       All Rights Reserved
*
* Module   :  LevInd.c
*
* Purpose  :  Level indicator Implementation
*
\**************************************************************************/

#include "window.h"
#include "malloc.h"
#include "plx_pdaex.h"
#include "string.h"
#include "setup.h"

#define SPIN_STATUS_FOCUS  0x00000001
#define MAX_PATH           256

#define CX_LEFTMARGIN      5
#define CX_RIGHTMARGIN     3
#define CY_TOPMARGIN1      4
#define CY_TOPMARGIN2      0

#define LEVIND_BLACK    "ROM:setup/levind_black_23x16.bmp"
#define LEVIND_WHITE    "ROM:setup/levind_white_23x16.bmp"
#define LEVIND_BLUE     "ROM:setup/levind_blue_23x16.bmp"

#define LEVIND_SIZE_W	23
#define LEVIND_SIZE_H	12
#define LEVIND_BRIT_WHITE "ROM:setup/Sett_Brightness_15x16.bmp"
#define LEVIND_BRIT_BLUE  "ROM:setup/Sett_Brightness_15x16.bmp"
#define LEVIND_VOL_WHITE  "ROM:setup/Sett_Volume_15x16.bmp"
#define LEVIND_VOL_BLUE   "ROM:setup/Sett_Volume_15x16.bmp"

#define LARROW_WIDTH 8
#define LARROW_HEIGHT 7

#define TRANS_BK_PIC (RGB(255,0,255))
#define CR_BLACK      (RGB(0, 0, 0))
#define CR_FOCUS_UNSEL (RGB(166, 202, 240))
#define CR_WHITE (RGB(255,255,255))

#define ROUND_RECT_RAD 8

#define GPS_ICON_WIDTH		  21
#define GPS_ICON_HEIGHT		  13

typedef struct tagSPINBOXEXDATA
{
    HWND    hwndSpinBox;
    DWORD   dwStatus;
    ADJTYPE adjType;
    PSTR    pszTitle;
    int     nTitleLen;
	RECT    rcArrow;
    HMENU   dwBoxID;
    HBITMAP hBritWhite;
    HBITMAP hVolWhite;
	HBITMAP hIndBlack;
	HBITMAP hIndWhite;
	HBITMAP hIndBlue;
	HBITMAP hGPSIcon;
    BOOL    iLimitFlag; //1: 0~5, 0: 1~5;
    int     nCurLev;
	char    GPSIconName[GPSICONNAMEMAXLEN + 1];
}
SPINBOXEXDATA, *PSPINBOXDATA;

static HBITMAP hbmpFocus = NULL;
static HBITMAP hBmpLeftArrow, hBmpRightArrow;
static BOOL iFocusFlag;

static LRESULT CALLBACK LEVIND_WndProc(HWND hWnd, UINT uMsgCmd,
                                       WPARAM wParam, LPARAM lParam);
static void OnCreate(HWND hWnd, LPARAM lParam);
static void OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnKeyDown(HWND hWnd, UINT uMsgCmd,WPARAM wParam, LPARAM lParam);
static void OnKeyUp(HWND hWnd, UINT uMsgCmd, WPARAM wParam, LPARAM lParam);
static void OnPaint(HWND hWnd);
static void OnCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void OnDestroy(HWND hWnd);
static BOOL OnSetTitle(HWND hWnd, LPARAM lParam);
static LRESULT OnCtlColorEdit(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT OnSSBM(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static BOOL GetTitleRect(HWND hWnd, PRECT pRect);
static void ChildRectToClient(HWND hWnd, HWND hwndChild, PRECT pRect);
extern int PLXOS_GetResourceDirectory(char *pathname);

/**********************************************************************
* Function     LEVIND_RegisterClass
* Purpose      
* Params       void
* Return       
* Remarks      
**********************************************************************/

BOOL LEVIND_RegisterClass(void)
{
    WNDCLASS wc;
    char szPathName[MAX_PATH] = "";
    int  nPathLen;
    char PathName[MAX_PATH];
    
    wc.style          = CS_OWNDC;
    wc.lpfnWndProc    = LEVIND_WndProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = sizeof(SPINBOXEXDATA);
    wc.hInstance      = NULL;
    wc.hIcon          = NULL;
    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground  = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = "LEVIND";
    
    PLXOS_GetResourceDirectory(szPathName);
    strcat(szPathName, "listfocus.bmp");
    hbmpFocus = LoadImage(NULL, szPathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
    PLXOS_GetResourceDirectory(PathName);
    nPathLen = strlen(PathName);
    
    strcat(PathName, "strspinleft.bmp");
    hBmpLeftArrow = LoadImage(NULL, PathName, IMAGE_BITMAP, LARROW_WIDTH,
        LARROW_HEIGHT, LR_LOADFROMFILE);
    
    PathName[nPathLen] = '\0';
    strcat(PathName, "strspinright.bmp");
    hBmpRightArrow = LoadImage(NULL, PathName, IMAGE_BITMAP, LARROW_WIDTH, 
        LARROW_HEIGHT, LR_LOADFROMFILE);
    
    return RegisterClass(&wc);
}

/**********************************************************************
* Function     SPINBOXEX_WndProc
* Purpose      
* Params       
* Return       
* Remarks      
**********************************************************************/

static LRESULT CALLBACK LEVIND_WndProc(HWND hWnd, UINT uMsgCmd,
                                       WPARAM wParam, LPARAM lParam)
{
    switch (uMsgCmd)
    {
    case WM_CREATE:
        OnCreate(hWnd, lParam);
        return 0;
        
    case WM_SETFOCUS:
        OnSetFocus(hWnd, wParam, lParam);
        return 0;
        
    case WM_KILLFOCUS:
        OnKillFocus(hWnd, wParam, lParam);
        return 0;
        
    case WM_KEYDOWN:
        OnKeyDown(hWnd, uMsgCmd,wParam, lParam);
        return 0;
        
    case WM_KEYUP:
        OnKeyUp(hWnd, uMsgCmd,wParam, lParam);
        return (0);

    case WM_PAINT:
        OnPaint(hWnd);
        return 0;
        
    case WM_COMMAND:
        OnCommand(hWnd, wParam, lParam);
        return 0;
        
    case WM_DESTROY:
        OnDestroy(hWnd);
        return 0;
        
    case WM_CTLCOLOREDIT:
        return OnCtlColorEdit(hWnd, wParam, lParam);
        
    case SSBM_SETTITLE:
        return OnSetTitle(hWnd, lParam);

    case SSBM_ADDSTRING:
    case SSBM_DELETESTRING:
    case SSBM_SETCURSEL:
    case SSBM_GETCURSEL:
    case SSBM_SETTEXT:
    case SSBM_GETTEXT:
    case SSBM_GETTEXTLEN:
    case SSBM_GETCOUNT:
        return OnSSBM(hWnd, uMsgCmd, wParam, lParam);
        
    default:
        return DefWindowProc(hWnd, uMsgCmd, wParam, lParam);
    }
}

/**********************************************************************
* Function     OnCreate
* Purpose      
* Params       HWND hWnd
* Return       
* Remarks      
**********************************************************************/

static void OnCreate(HWND hWnd, LPARAM lParam)
{
    PSPINBOXDATA    pData = NULL;
    LPCREATESTRUCT  lpcs = NULL;
    RECT            rcTitle, rcClient;
    HDC hdc;
    WORD wSpinStyle = 0;
    lpcs = (LPCREATESTRUCT)lParam;
    pData = (PSPINBOXDATA)GetUserData(hWnd);
    
    memset((void*)pData, 0, sizeof(SPINBOXEXDATA));

    
	GetClientRect(hWnd, &rcClient);

	pData->rcArrow.left = 148 + (rcClient.right - 171);

    GetTitleRect(hWnd, &rcTitle);
    
    wSpinStyle = LOWORD(lpcs->style);
    pData->adjType = (ADJTYPE)(lpcs->lpCreateParams);
    
	pData->dwBoxID = lpcs->hMenu;
    
    pData->hwndSpinBox = CreateWindow(
        "STRSPINBOX",
        "",
        /*WS_VISIBLE | */WS_CHILD /*| CS_NOSYSCTRL*/ | wSpinStyle,
        rcTitle.left,
        rcTitle.bottom + CY_TOPMARGIN2,
        rcTitle.right - rcTitle.left,
        lpcs->cy - rcTitle.bottom - 2 * CY_TOPMARGIN2,
        hWnd,
        NULL,
        NULL,
        NULL);
    
    
    OnSetTitle(hWnd, (LPARAM)lpcs->lpszName);
	switch(pData->adjType)
    {
    case LAS_LIT: //1~5
        pData->iLimitFlag = FALSE;
    	break;
    case LAS_VOL: //0~5
        pData->iLimitFlag = TRUE;
    	break;
	case LAS_ICON_SHOW:
		pData->hIndBlack = NULL;
		pData->hIndWhite = NULL;
		pData->hIndBlue  = NULL;		
		pData->hBritWhite= NULL;
		pData->hVolWhite = NULL;
		pData->hGPSIcon  = NULL;
		memset(pData->GPSIconName, 0, sizeof(pData->GPSIconName));
		return;
    default:
        break;
    }
    
    hdc = GetDC(hWnd);
    pData->hIndBlack = LoadImageEx(hdc,NULL,LEVIND_BLACK,IMAGE_BITMAP,LEVIND_SIZE_W,LEVIND_SIZE_H,LR_LOADFROMFILE);
    pData->hIndWhite = LoadImageEx(hdc,NULL,LEVIND_WHITE,IMAGE_BITMAP,LEVIND_SIZE_W,LEVIND_SIZE_H,LR_LOADFROMFILE);
    pData->hIndBlue  = LoadImageEx(hdc,NULL,LEVIND_BLUE, IMAGE_BITMAP,LEVIND_SIZE_W,LEVIND_SIZE_H,LR_LOADFROMFILE);
    
    pData->hBritWhite= LoadImageEx( hdc,NULL,LEVIND_BRIT_WHITE, IMAGE_BITMAP,15,15,LR_LOADFROMFILE);
    pData->hVolWhite = LoadImageEx( hdc,NULL,LEVIND_VOL_WHITE, IMAGE_BITMAP,15,15,LR_LOADFROMFILE);
    ReleaseDC(hWnd, hdc);
}
  
    

/**********************************************************************
* Function     OnSetFocus
* Purpose      
* Params       hWnd, wParam, lParam
* Return       
* Remarks      
**********************************************************************/

static void OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    
    PSPINBOXDATA pData = NULL;
    DWORD dwStyle = 0;
    HWND          hwndParent = NULL;
    
    pData = (PSPINBOXDATA)GetUserData(hWnd);    
    
    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    
    if (dwStyle & WS_CHILD)
        hwndParent = GetParent(hWnd);
    else
        hwndParent = GetWindow(hWnd, GW_OWNER);
    SendMessage(hwndParent, WM_COMMAND, 
        MAKELONG(pData->dwBoxID, SSBN_SETFOCUS), (LPARAM)hWnd);
    
    iFocusFlag = TRUE;
    
    InvalidateRect(hWnd, NULL, TRUE);
    
}

/**********************************************************************
* Function     OnKillFocus
* Purpose      
* Params       hWnd, wParam, lParam
* Return       
* Remarks      
**********************************************************************/

static void OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    iFocusFlag = FALSE;
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
}
/**********************************************************************
* Function     OnKeyUp
* Purpose      
* Params       HWND hWnd, WPARAM wParam, LPARAM lParam
* Return       
* Remarks      
**********************************************************************/
static void OnKeyUp(HWND hWnd, UINT uMsgCmd, WPARAM wParam, LPARAM lParam)
{
    PSPINBOXDATA  pData = NULL;
    HWND          hwndParent = NULL;
    
    pData = (PSPINBOXDATA)GetUserData(hWnd);
    
    hwndParent = GetParent(hWnd);
    if (hwndParent == NULL)
        return;
    if (GetWindowLong(hWnd, GWL_STYLE) & CS_NOSYSCTRL)
    {
        SendMessage(hwndParent, WM_KEYUP, wParam, lParam);
        return;
    }
    
    
}

/**********************************************************************
* Function     OnKeyDown
* Purpose      
* Params       HWND hWnd, WPARAM wParam, LPARAM lParam
* Return       
* Remarks      
**********************************************************************/

static void OnKeyDown(HWND hWnd, UINT uMsgCmd, WPARAM wParam, LPARAM lParam)
{
    PSPINBOXDATA  pData = NULL;
    HWND          hwndParent = NULL;
    RECT rectRef;
	
    pData = (PSPINBOXDATA)GetUserData(hWnd);
    
    hwndParent = GetParent(hWnd);
    if (hwndParent == NULL)
        return;
    
	
    switch (wParam)
    {
    case VK_LEFT:

		if (pData->adjType == LAS_ICON_SHOW)
			break;

		rectRef.left = 25 + (pData->nCurLev - 1) * 24;
		rectRef.top = 33;
		rectRef.right = rectRef.left + 23;
		rectRef.bottom = 45;
        if(pData->iLimitFlag) //0~5
        {
            if(pData->nCurLev > 0)
            {
                pData->nCurLev--;
                InvalidateRect(hWnd, &rectRef, TRUE);
            }
        }
        else //1~5
        {
            if(pData->nCurLev > 1)
            {
                pData->nCurLev--;
                InvalidateRect(hWnd, &rectRef, TRUE);
            }

        }
        SendMessage(hwndParent, WM_COMMAND, 
                MAKELONG(pData->dwBoxID, SSBN_CHANGE), (LPARAM)hWnd);

        break;
        
    case VK_RIGHT:
		if (pData->adjType == LAS_ICON_SHOW)
			break;

        if(pData->nCurLev < 5)
        {
            pData->nCurLev++;
			rectRef.left = 25 + (pData->nCurLev - 1) * 24;
			rectRef.top = 33;
			rectRef.right = rectRef.left + 23;
			rectRef.bottom = 45;
            InvalidateRect(hWnd, &rectRef, TRUE);
            
        }
        SendMessage(hwndParent, WM_COMMAND, 
            MAKELONG(pData->dwBoxID, SSBN_CHANGE), (LPARAM)hWnd);
        
        break;
        
    case VK_DOWN:
        if (GetWindowLong(hWnd, GWL_STYLE) & CS_NOSYSCTRL)
        {
            SendMessage(hwndParent, WM_KEYDOWN, wParam, lParam);
            return;
        }
        SetFocus(GetNextDlgTabItem(hwndParent, hWnd, FALSE));
        break;

    case VK_UP:
        if (GetWindowLong(hWnd, GWL_STYLE) & CS_NOSYSCTRL)
        {
            SendMessage(hwndParent, WM_KEYDOWN, wParam, lParam);
            return;
        }
        SetFocus(GetNextDlgTabItem(hwndParent, hWnd, TRUE));
        break;
    default:
        SendMessage(hwndParent, WM_KEYDOWN, wParam, lParam);
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
    PSPINBOXDATA  pData = NULL;
    HDC           hdc = NULL;
    PAINTSTRUCT   ps;
    RECT          rc;
    int           nMode = 0;
    COLORREF      crOld = 0;
    COLORREF      cr = 0;
    BITMAP        bmp;
    int i;
    HBRUSH        hBrush = NULL;
	HBRUSH hIndBrush = NULL, hOldBrush = NULL;
	HPEN hOldPen = NULL;
    pData = (PSPINBOXDATA)GetUserData(hWnd);
    
    hdc = BeginPaint(hWnd, &ps);      

	GetClientRect(hWnd, &rc);
	
    if(iFocusFlag)
    {
			GetObject(hbmpFocus, sizeof(BITMAP), (void*)&bmp);
			StretchBlt(hdc, rc.left, rc.top, rc.right - rc.left,
				rc.bottom - rc.top, (HDC)hbmpFocus, 0, 0, bmp.bmWidth,
				bmp.bmHeight, ROP_SRC);
			
			cr = SetTextColor(hdc, COLOR_WHITE); 
			
			/************ Paint the icon (with focus)***********/
			switch(pData->adjType)
			{
            case LAS_LIT:
                nMode = SetBkMode(hdc, NEWTRANSPARENT);
                crOld = SetBkColor(hdc, CR_BLACK);
                StretchBlt(hdc, 5, 31, 15, 15, (HDC)pData->hBritWhite, 0, 0, 15, 15, ROP_SRC);
                SetBkColor(hdc, crOld);
                SetBkMode(hdc, nMode);
				for(i=0;i<pData->nCurLev;i++)
				{
					StretchBlt(hdc, 25 + 24*i, 33, LEVIND_SIZE_W, LEVIND_SIZE_H, (HDC)pData->hIndBlack, 0, 0, LEVIND_SIZE_W, LEVIND_SIZE_H, ROP_SRC);
				}
				for (;i<5;i++)
				{
					StretchBlt(hdc, 25 + 24 * i, 33, LEVIND_SIZE_W, LEVIND_SIZE_H, (HDC)pData->hIndWhite, 0, 0, LEVIND_SIZE_W, LEVIND_SIZE_H, ROP_SRC);
				}
				/************ Paint the toggle icon ***********/
				nMode = SetBkMode(hdc, NEWTRANSPARENT);
				cr = SetBkColor(hdc, RGB(0, 0, 0));
				BitBlt(hdc, pData->rcArrow.left, 35, LARROW_WIDTH, LARROW_HEIGHT,
					(HDC)hBmpLeftArrow, 0, 0, ROP_SRC);
				
				
				BitBlt(hdc, pData->rcArrow.left + 11, 35, LARROW_WIDTH, LARROW_HEIGHT,
					(HDC)hBmpRightArrow, 0, 0, ROP_SRC);
				SetBkColor(hdc, cr);
				SetBkMode(hdc, nMode);
				/************ Paint the toggle icon ***********/        
                break;
            case LAS_VOL:
                nMode = SetBkMode(hdc, NEWTRANSPARENT);
                crOld = SetBkColor(hdc, CR_BLACK);
                StretchBlt(hdc, 5, 31, 15, 15, (HDC)pData->hVolWhite, 0, 0, 15, 15, ROP_SRC);
                SetBkColor(hdc, crOld);
                SetBkMode(hdc, nMode);
				for(i=0;i<pData->nCurLev;i++)
				{
					StretchBlt(hdc, 25 + 24*i, 33, LEVIND_SIZE_W, LEVIND_SIZE_H, (HDC)pData->hIndBlack, 0, 0, LEVIND_SIZE_W, LEVIND_SIZE_H, ROP_SRC);
				}
				for (;i<5;i++)
				{
					StretchBlt(hdc, 25 + 24 * i, 33, LEVIND_SIZE_W, LEVIND_SIZE_H, (HDC)pData->hIndWhite, 0, 0, LEVIND_SIZE_W, LEVIND_SIZE_H, ROP_SRC);
				}
				/************ Paint the toggle icon ***********/
				nMode = SetBkMode(hdc, NEWTRANSPARENT);
				cr = SetBkColor(hdc, RGB(0, 0, 0));
				BitBlt(hdc, pData->rcArrow.left, 35, LARROW_WIDTH, LARROW_HEIGHT,
					(HDC)hBmpLeftArrow, 0, 0, ROP_SRC);
				
				
				BitBlt(hdc, pData->rcArrow.left + 11, 35, LARROW_WIDTH, LARROW_HEIGHT,
					(HDC)hBmpRightArrow, 0, 0, ROP_SRC);
				SetBkColor(hdc, cr);
				SetBkMode(hdc, nMode);
				/************ Paint the toggle icon ***********/        
                break;
			case LAS_ICON_SHOW:
				if(pData->hGPSIcon)
				{
					nMode = SetBkMode(hdc, NEWTRANSPARENT);
					crOld = SetBkColor(hdc, CR_WHITE);
				    BitBlt(hdc, 5, 31, GPS_ICON_WIDTH, GPS_ICON_HEIGHT, (HDC)pData->hGPSIcon, 0, 0, ROP_NSRC);
	                SetBkColor(hdc, crOld);
	                SetBkMode(hdc, nMode);					
				}

				break;
            default:
                break;
			}
    }
    else
    {
			hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLOREDIT, (WPARAM)hdc,
				(LPARAM)hWnd);
			FillRect(hdc, &rc, hBrush);
			
			cr = SetTextColor(hdc, COLOR_BLACK); 
			
			/************ Paint the icon (with focus)***********/
			
			switch(pData->adjType)
			{
            case LAS_LIT:
                nMode = SetBkMode(hdc, NEWTRANSPARENT);
                crOld = SetBkColor(hdc, CR_BLACK);
                StretchBlt(hdc, 5, 31, 15, 15, (HDC)pData->hBritWhite, 0, 0, 15, 15, ROP_SRC);
                SetBkColor(hdc, crOld);
                SetBkMode(hdc, nMode);
				for(i=0;i<pData->nCurLev;i++)
				{
					StretchBlt(hdc, 25 + 24*i, 33, LEVIND_SIZE_W, LEVIND_SIZE_H, (HDC)pData->hIndBlack, 0, 0, LEVIND_SIZE_W, LEVIND_SIZE_H, ROP_SRC);
				}
				for (;i<5;i++)
				{
					StretchBlt(hdc, 25 + 24 * i, 33, LEVIND_SIZE_W, LEVIND_SIZE_H, (HDC)pData->hIndBlue, 0, 0, LEVIND_SIZE_W, LEVIND_SIZE_H, ROP_SRC);
				}
                break;
            case LAS_VOL:
                nMode = SetBkMode(hdc, NEWTRANSPARENT);
                crOld = SetBkColor(hdc, CR_BLACK);
                StretchBlt(hdc, 5, 31, 15, 15, (HDC)pData->hVolWhite, 0, 0, 15, 15, ROP_SRC);
                SetBkColor(hdc, crOld);
                SetBkMode(hdc, nMode);
				for(i=0;i<pData->nCurLev;i++)
				{
					StretchBlt(hdc, 25 + 24*i, 33, LEVIND_SIZE_W, LEVIND_SIZE_H, (HDC)pData->hIndBlack, 0, 0, LEVIND_SIZE_W, LEVIND_SIZE_H, ROP_SRC);
				}
				for (;i<5;i++)
				{
					StretchBlt(hdc, 25 + 24 * i, 33, LEVIND_SIZE_W, LEVIND_SIZE_H, (HDC)pData->hIndBlue, 0, 0, LEVIND_SIZE_W, LEVIND_SIZE_H, ROP_SRC);
				}
                break;
			case LAS_ICON_SHOW:
                nMode = SetBkMode(hdc, NEWTRANSPARENT);
                crOld = SetBkColor(hdc, CR_WHITE);
				if(pData->hGPSIcon)
				{
				    BitBlt(hdc, 5, 31, GPS_ICON_WIDTH, GPS_ICON_HEIGHT, (HDC)pData->hGPSIcon, 0, 0, ROP_SRC);
				}
                SetBkColor(hdc, crOld);
                SetBkMode(hdc, nMode);
            default:
                break;
			}
    }

    
    if (pData->pszTitle != NULL)
    {
        GetTitleRect(hWnd, &rc);
        
        nMode = SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, pData->pszTitle, pData->nTitleLen, &rc,
            DT_LEFT | DT_VCENTER);
        SetBkMode(hdc, nMode);
    }
    
    SetTextColor(hdc, cr);
    
    EndPaint(hWnd, &ps);
}




/**********************************************************************
* Function     OnCommand
* Purpose      
* Params       HWND hWnd, WPARAM wParam, LPARAM lParam
* Return       
* Remarks      
**********************************************************************/

static void OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PSPINBOXDATA  pData = NULL;
    HWND          hwndParent = NULL;
    DWORD         dwStyle = 0, dwID = 0;
    //    RECT          rc;
    
    pData = (PSPINBOXDATA)GetUserData(hWnd);
    dwID = GetWindowLong(hWnd, GWL_ID);
    
    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    
    if (dwStyle & WS_CHILD)
        hwndParent = GetParent(hWnd);
    else
        hwndParent = GetWindow(hWnd, GW_OWNER);
    SendMessage(hwndParent, WM_COMMAND, 
        MAKELONG(pData->dwBoxID, SSBN_SETFOCUS), (LPARAM)hWnd);
    
    switch (HIWORD(wParam))
    {
    case SSBN_SETFOCUS:
        
        break;
        
    case SSBN_KILLFOCUS:
        break;
        
    case SSBN_ERASECTLBK:
        break;
        
    default:
        break;
    }
    
    SendMessage(hwndParent, WM_COMMAND, MAKEWPARAM(dwID, HIWORD(wParam)),
        (LPARAM)hWnd);
}

/**********************************************************************
* Function     OnDestroy
* Purpose      
* Params       HWND hWnd
* Return       
* Remarks      
**********************************************************************/

static void OnDestroy(HWND hWnd)
{
    PSPINBOXDATA    pData = NULL;
    pData = (PSPINBOXDATA)GetUserData(hWnd);
	if(pData->hBritWhite)
		DeleteObject(pData->hBritWhite);
	if(pData->hVolWhite)
		DeleteObject(pData->hVolWhite);
	if(pData->hIndBlack)
		DeleteObject(pData->hIndBlack);
	if(pData->hIndBlue)
		DeleteObject(pData->hIndBlue);
	if(pData->hIndWhite)
		DeleteObject(pData->hIndWhite);
	if(pData->hGPSIcon)
		DeleteObject(pData->hGPSIcon);

}

/**********************************************************************
* Function     OnSetTitle
* Purpose      
* Params       HWND hWnd, LPARAM lParam
* Return       
* Remarks      
**********************************************************************/
static BOOL OnSetTitle(HWND hWnd, LPARAM lParam)
{
    PSPINBOXDATA pData = NULL;
    int          nNewLen = 0;
    PSTR         pszNewTitle = NULL;
    RECT         rcTitle;
    
    pData = (PSPINBOXDATA)GetUserData(hWnd);
    
    pszNewTitle = (PSTR)lParam;
    
    nNewLen = strlen(pszNewTitle);
    
    if (pData->pszTitle == NULL)
    {
        pData->pszTitle = malloc(nNewLen + 1);
    }
    else if (pData->nTitleLen < nNewLen)
    {
        pData->pszTitle = realloc(pData->pszTitle, nNewLen + 1);
    }
    
    if (pData->pszTitle == NULL)
        return FALSE;
    
    strcpy((char*)pData->pszTitle, (const char*)pszNewTitle);
    pData->nTitleLen = nNewLen;
    
    GetTitleRect(hWnd, &rcTitle);
    InvalidateRect(hWnd, &rcTitle, TRUE);
    
    return TRUE;
}

/**********************************************************************
* Function     OnCtlColorEdit
* Purpose      
* Params       HWND hWnd, WPARAM wParam, LPARAM lParam
* Return       
* Remarks      
**********************************************************************/

static LRESULT OnCtlColorEdit(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PSPINBOXDATA pData = NULL;
    HDC hdc = NULL;
    static HFONT hFont = NULL;
    
    hdc = (HDC)wParam;
    pData = (PSPINBOXDATA)GetUserData(hWnd);
    
    if (hFont == NULL)
    {
        GetFontHandle(&hFont, SMALL_FONT);
    }
    SelectObject(hdc, (HGDIOBJ)hFont);
    
    if (pData->dwStatus & SPIN_STATUS_FOCUS)
    {
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    else
    {
        return DefWindowProc(hWnd, WM_CTLCOLOREDIT, wParam, lParam);
    }
}

/**********************************************************************
* Function     OnSSBM
* Purpose      
* Params       hWnd, uMsg, wParam, lParam
* Return       
* Remarks      
**********************************************************************/

static LRESULT OnSSBM(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PSPINBOXDATA pData = NULL;
    LRESULT lResult;
    pData = (PSPINBOXDATA)GetUserData(hWnd);
    
    switch(uMsg)
    {
    case SSBM_GETCURSEL:
        lResult = pData->nCurLev;
        break;
    case SSBM_SETCURSEL:
		if (wParam>=0 && wParam<=5)
			pData->nCurLev = wParam;
        lResult = TRUE;
//        InvalidateRect(hWnd, NULL, TRUE);
        break;
	case SSBM_SETTEXT:
		if (wParam > 0 && wParam<=sizeof(pData->GPSIconName))
		{
			memset(pData->GPSIconName, 0, sizeof(pData->GPSIconName));
			strncpy(pData->GPSIconName, (const char*)lParam, wParam);
			if (pData->hGPSIcon)
			{
				DeleteObject(pData->hGPSIcon);
				pData->hGPSIcon = NULL;
			}
			pData->hGPSIcon = LoadImage(NULL,pData->GPSIconName, IMAGE_BITMAP,GPS_ICON_WIDTH,GPS_ICON_HEIGHT,LR_LOADFROMFILE);
		}
		break;
    default:
        return SendMessage(pData->hwndSpinBox, uMsg, wParam, lParam);        
    }    
    return lResult;
}

/**********************************************************************
* Function     GetTitleRect
* Purpose      
* Params       HWND hWnd, PRECT pRect
* Return       
* Remarks      
**********************************************************************/

static BOOL GetTitleRect(HWND hWnd, PRECT pRect)
{
    RECT   rcClient;
    HDC    hdc = NULL;
    SIZE   size;
    
    GetClientRect(hWnd, &rcClient);
    
    hdc = GetDC(hWnd);
    GetTextExtentPoint32(hdc, "T", -1, &size);
    ReleaseDC(hWnd, hdc);
    
    pRect->left   = rcClient.left + CX_LEFTMARGIN;
    pRect->top    = rcClient.top + CY_TOPMARGIN1;
    pRect->right  = rcClient.right - CX_RIGHTMARGIN;
    pRect->bottom = pRect->top + size.cy;
    
    return TRUE;
}

/**********************************************************************
* Function     ChildRectToClient
* Purpose      
* Params       hWnd, hwndChild, pRect
* Return       
* Remarks      
**********************************************************************/

static void ChildRectToClient(HWND hWnd, HWND hwndChild, PRECT pRect)
{
    POINT pt;
    
    pt.x = pRect->left;
    pt.y = pRect->top;
    ClientToScreen(hwndChild, &pt);
    ScreenToClient(hWnd, &pt);
    pRect->left = pt.x;
    pRect->top = pt.y;
    
    pt.x = pRect->right;
    pt.y = pRect->bottom;
    ClientToScreen(hwndChild, &pt);
    ScreenToClient(hWnd, &pt);
    pRect->right = pt.x;
    pRect->bottom = pt.y;
}
