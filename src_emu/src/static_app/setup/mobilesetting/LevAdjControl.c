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

typedef struct tagLEVADJDATA
{
    HWND    hwndSpinBox;
    DWORD   dwStatus;
    ADJTYPE adjType;
    PSTR    pszTitle;
    int     nTitleLen;
    HMENU   dwBoxID;
    HBITMAP hCapIconLit;
    HBITMAP hCapIconVol;
    BOOL    iLimitFlag; //1: 0~5, 0: 1~5;
    int     nCurLev;
}
LEVADJDATA, *PLEVADJDATA;

#define ICONVOL "ROM:setup/Sett_Volume_19x19.bmp"
#define ICONLIT "ROM:setup/Sett_Brightness_19x19.bmp"
#define ICON_IND_WIDTH      26
#define ICON_IND_HEIGHT     46    

#define ICON_INDEX_WIDTH    26
#define ICON_INDEX_HEIGHT   46
#define TITLE_ICON_WIDTH    19
#define TITLE_ICON_HEIGHT   19
#define ICON_IND_POS_INIT_X 17
#define ICON_IND_POS_INIT_Y 102  
#define ICON_IND_POS_INIT_W 26
#define ICON_IND_POS_INIT_H 26

#define ICON_IND_X_GAP      28
#define ICON_IND_Y_GAP      5


#define CR_BLACK			(RGB(0, 0, 0))
#define TITLEBK_COLOR		(RGB(163, 176, 229))

#define CR_INTER_BODER		(RGB(166,202,240))
#define CR_INTER_BODER_R	(RGB(153,153,204))


#define CR_ICON_IND			(RGB(51,150,204))
#define CR_ICON_IND_BACK	(RGB(255,255,255))

#define CR_LEV_BACK_COLOR	(RGB(241,241,245))

#define TITLE_ICON_WIDTH    19
#define TITLE_ICON_HEIGHT   19

#define TITLE_AREA_HEIGHT   30

#define TITLE_ICON_X        79
#define TITLE_ICON_Y        6

#define ROUND_RECT_RAD      7
static LRESULT CALLBACK LEVADJ_WndProc(HWND hWnd, UINT uMsgCmd,
                                       WPARAM wParam, LPARAM lParam);
static void OnCreate(HWND hWnd, LPARAM lParam);
static void OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnKeyDown(HWND hWnd, UINT uMsgCmd,WPARAM wParam, LPARAM lParam);
static void OnKeyUp(HWND hWnd, UINT uMsgCmd, WPARAM wParam, LPARAM lParam);
static void OnPaint(HWND hWnd);
static void OnCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void OnDestroy(HWND hWnd);
static int StringToInt( char *str );

/**********************************************************************
* Function     LEVIND_RegisterClass
* Purpose      
* Params       void
* Return       
* Remarks      
**********************************************************************/

BOOL LEVADJ_RegisterClass(void)
{
    WNDCLASS wc;
    
    wc.style          = CS_OWNDC;
    wc.lpfnWndProc    = LEVADJ_WndProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = sizeof(LEVADJDATA);
    wc.hInstance      = NULL;
    wc.hIcon          = NULL;
    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground  = NULL/*(HBRUSH)GetStockObject(CR_LEV_BACK_COLOR)*/;
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = "LEVADJ";
    
    return RegisterClass(&wc);
}

/**********************************************************************
* Function     SPINBOXEX_WndProc
* Purpose      
* Params       
* Return       
* Remarks      
**********************************************************************/

static LRESULT CALLBACK LEVADJ_WndProc(HWND hWnd, UINT uMsgCmd,
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
    PLEVADJDATA    pData = NULL;
    LPCREATESTRUCT  lpcs = NULL;
    HDC hdc;
    WORD wStyle = 0;
    lpcs = (LPCREATESTRUCT)lParam;
    pData = (PLEVADJDATA)GetUserData(hWnd);
    
    memset((void*)pData, 0, sizeof(LEVADJDATA));
    
    
    wStyle = LOWORD(lpcs->style);
    pData->adjType = (ADJTYPE)(lpcs->lpCreateParams);
	pData->nCurLev = StringToInt((char*)(lpcs->lpszName));
	//pData->pszTitle = lpcs->lpszName;

    switch(pData->adjType)
    {
    case LAS_LIT: //1~5
        pData->iLimitFlag = FALSE;
		hdc = GetDC(hWnd);
		pData->hCapIconLit = LoadImageEx(hdc,NULL,ICONLIT,IMAGE_BITMAP,TITLE_ICON_WIDTH,TITLE_ICON_HEIGHT,LR_LOADFROMFILE);
		pData->hCapIconVol = NULL;
		ReleaseDC(hWnd, hdc);   
    	break;
	case LAS_KEY:
    case LAS_VOL: //0~5
	case LAS_RING:
	case LAS_NOTIFY:
        pData->iLimitFlag = TRUE;
		hdc = GetDC(hWnd);
		pData->hCapIconLit = NULL;
		pData->hCapIconVol = LoadImageEx(hdc,NULL,ICONVOL,IMAGE_BITMAP,TITLE_ICON_WIDTH,TITLE_ICON_HEIGHT,LR_LOADFROMFILE);
		ReleaseDC(hWnd, hdc);   
		break;
    default:
        break;
    }
    pData->dwBoxID = lpcs->hMenu;
    
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
    
    PLEVADJDATA pData = NULL;
    DWORD dwStyle = 0;
    HWND          hwndParent = NULL;
    
    pData = (PLEVADJDATA)GetUserData(hWnd);    
    
    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    
    if (dwStyle & WS_CHILD)
        hwndParent = GetParent(hWnd);
    else
        hwndParent = GetWindow(hWnd, GW_OWNER);
    SendMessage(hwndParent, WM_COMMAND, 
        MAKELONG(pData->dwBoxID, SSBN_SETFOCUS), (LPARAM)hWnd);

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
    PLEVADJDATA  pData = NULL;
    HWND          hwndParent = NULL;
    
    pData = (PLEVADJDATA)GetUserData(hWnd);
    
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
    PLEVADJDATA  pData = NULL;
    HWND          hwndParent = NULL;
	RECT refRect;
    
	pData = (PLEVADJDATA)GetUserData(hWnd);
   
    hwndParent = GetParent(hWnd);
    if (hwndParent == NULL)
        return;   
    
    switch (wParam)
    {
    case VK_LEFT:
		refRect.left = ICON_IND_POS_INIT_X + (pData->nCurLev - 1)*ICON_IND_X_GAP;
		refRect.top = ICON_IND_POS_INIT_Y - (pData->nCurLev - 1)*ICON_IND_Y_GAP;
		refRect.right = refRect.left + ICON_IND_POS_INIT_H;
		refRect.bottom = refRect.top + ICON_IND_POS_INIT_W + (pData->nCurLev - 1)* ICON_IND_Y_GAP;
        switch(pData->adjType)
        {
        case LAS_LIT:
            if(pData->nCurLev != 1)
            {
                pData->nCurLev  -= 1;
				InvalidateRect(hWnd, &refRect, TRUE);
            }
			break;
		case LAS_KEY:
		case LAS_RING:
        case LAS_VOL:
		case LAS_NOTIFY:
            if(pData->nCurLev  != 0)
            {
                pData->nCurLev  -= 1;
				InvalidateRect(hWnd, &refRect, TRUE);
            }
			break;
        default:
            break;           
        }
		SendMessage(hwndParent, WM_KEYDOWN, wParam, (LPARAM)(pData->nCurLev));
        break;
        
    case VK_RIGHT:
        switch(pData->adjType)
        {
        case LAS_LIT:
            if(pData->nCurLev  != 5)
            {
                pData->nCurLev  += 1;
				refRect.left = ICON_IND_POS_INIT_X + (pData->nCurLev - 1)*ICON_IND_X_GAP;
				refRect.top = ICON_IND_POS_INIT_Y - (pData->nCurLev - 1)*ICON_IND_Y_GAP;
				refRect.right = refRect.left + ICON_IND_POS_INIT_H;
				refRect.bottom = refRect.top + ICON_IND_POS_INIT_W + (pData->nCurLev - 1)* ICON_IND_Y_GAP;
				InvalidateRect(hWnd, &refRect, TRUE);
            }
            break;
		case LAS_KEY:
		case LAS_RING:
        case LAS_VOL:
		case LAS_NOTIFY:
            if(pData->nCurLev  != 5)
            {
                pData->nCurLev  += 1;
				refRect.left = ICON_IND_POS_INIT_X + (pData->nCurLev - 1)*ICON_IND_X_GAP;
				refRect.top = ICON_IND_POS_INIT_Y - (pData->nCurLev - 1)*ICON_IND_Y_GAP;
				refRect.right = refRect.left + ICON_IND_POS_INIT_H;
				refRect.bottom = refRect.top + ICON_IND_POS_INIT_W + (pData->nCurLev - 1)* ICON_IND_Y_GAP;
				InvalidateRect(hWnd, &refRect, TRUE);
            }
            break;
        default:
            break;
        }    
		SendMessage(hwndParent, WM_KEYDOWN, wParam, (LPARAM)(pData->nCurLev));
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
    PLEVADJDATA   pData = NULL;
    HDC           hdc = NULL;
    PAINTSTRUCT   ps;
    RECT          rc, rcTitle /*rcBrush*/;
    int           nMode = 0;
    COLORREF      crOld = 0;
    COLORREF      cr = 0;
    int i;
    HBRUSH        hBrush = NULL;
    int iIndPos, iIndWidth, iIndHeight, iIndY;
    int iBKIndNum = 5;
	HBRUSH hIndBrush = NULL, hOldBrush = NULL;
	HPEN hOldPen =  NULL, hBoderPen = NULL, hBoderPen_R = NULL;
    pData = (PLEVADJDATA)GetUserData(hWnd);
    
    hdc = BeginPaint(hWnd, &ps);       

    GetClientRect(hWnd, &rc);
	ClearRect(hdc, &rc, CR_LEV_BACK_COLOR);
    rcTitle.bottom = TITLE_AREA_HEIGHT;
    rcTitle.right = rc.right;
    rcTitle.left = 0;
    rcTitle.top = rc.top;
    
    ClearRect(hdc, &rcTitle, TITLEBK_COLOR);
	
	nMode = SetBkMode(hdc, NEWTRANSPARENT);
	crOld = SetBkColor(hdc, CR_BLACK);
	switch(pData->adjType)
	{
	case LAS_LIT:
		BitBlt(hdc, TITLE_ICON_X, TITLE_ICON_Y, TITLE_ICON_WIDTH, TITLE_ICON_HEIGHT, (HDC)pData->hCapIconLit, 0, 0, ROP_SRC);
		break;
	case LAS_RING:
	case LAS_KEY:
	case LAS_VOL:
	case LAS_NOTIFY:
		BitBlt(hdc, TITLE_ICON_X, TITLE_ICON_Y, TITLE_ICON_WIDTH, TITLE_ICON_HEIGHT, (HDC)pData->hCapIconVol, 0, 0, ROP_SRC);
		break;
	default:
		break;
	}
	SetBkColor(hdc, crOld);
	SetBkMode(hdc, nMode);

    iIndWidth = ICON_IND_POS_INIT_W - 2;
    iIndHeight = ICON_IND_POS_INIT_H - 2;
    iIndY = ICON_IND_POS_INIT_Y + 1;
    iIndPos=ICON_IND_POS_INIT_X + 1;

	hIndBrush = CreateBrush(BS_SOLID,CR_ICON_IND,NULL);
	hBoderPen = CreatePen(PS_SOLID, 1, CR_INTER_BODER);
	hBoderPen_R = CreatePen(PS_SOLID, 1, CR_INTER_BODER_R);
	hOldBrush = SelectObject(hdc,hIndBrush);
    
	for(i=0;i<pData->nCurLev;i++)
    {
		RoundRect(hdc, iIndPos, iIndY, iIndPos + iIndWidth, iIndY + iIndHeight, ROUND_RECT_RAD, ROUND_RECT_RAD);

		hOldPen = SelectObject(hdc, hBoderPen);
		DrawLine(hdc, iIndPos+1, iIndY+1, iIndPos + iIndWidth - 1, iIndY+1);
		DrawLine(hdc, iIndPos+1, iIndY+1, iIndPos+1, iIndY + iIndHeight - 1);
		SelectObject(hdc, hOldPen);

		hOldPen = SelectObject(hdc, hBoderPen_R);
		DrawLine(hdc, iIndPos + iIndWidth - 2, iIndY + iIndHeight - 2, iIndPos + iIndWidth - 2, iIndY + 1);
		DrawLine(hdc, iIndPos + iIndWidth - 2, iIndY + iIndHeight - 2, iIndPos, iIndY + iIndHeight - 2);
		SelectObject(hdc, hOldPen);

        iIndPos += ICON_IND_X_GAP;    
        iIndHeight += ICON_IND_Y_GAP;
        iIndY -= ICON_IND_Y_GAP;
    }
	hIndBrush = SelectObject(hdc,hOldBrush);
    

	if (i<iBKIndNum)
	{
//		hOldPen = SelectObject(hdc, GetStockObject(BLACK_PEN));
		for(;i<iBKIndNum;i++)
		{
			RoundRect(hdc, iIndPos, iIndY, iIndPos + iIndWidth, iIndY + iIndHeight, ROUND_RECT_RAD, ROUND_RECT_RAD);

			hOldPen = SelectObject(hdc, hBoderPen);
			DrawLine(hdc, iIndPos+1, iIndY+1, iIndPos + iIndWidth - 1, iIndY+1);
			DrawLine(hdc, iIndPos+1, iIndY+1, iIndPos+1, iIndY + iIndHeight - 1);
			SelectObject(hdc, hOldPen);

			hOldPen = SelectObject(hdc, hBoderPen_R);
			DrawLine(hdc, iIndPos + iIndWidth - 2, iIndY + iIndHeight - 2, iIndPos + iIndWidth - 2, iIndY + 1);
			DrawLine(hdc, iIndPos + iIndWidth - 2, iIndY + iIndHeight - 2, iIndPos, iIndY + iIndHeight - 2);
			SelectObject(hdc, hOldPen);

			iIndPos += ICON_IND_X_GAP;    
			iIndHeight += ICON_IND_Y_GAP;
			iIndY -= ICON_IND_Y_GAP;
		}
//		SelectObject(hdc,hOldPen);
	}
//	SelectObject(hdc,hOldPen);
	DeleteObject(hIndBrush);
	DeleteObject(hBoderPen);
	DeleteObject(hBoderPen_R);
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
    PLEVADJDATA  pData = NULL;
    HWND          hwndParent = NULL;
    DWORD         dwStyle = 0, dwID = 0;
    //    RECT          rc;
    
    pData = (PLEVADJDATA)GetUserData(hWnd);
    dwID = GetWindowLong(hWnd, GWL_ID);
    
    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    
    if (dwStyle & WS_CHILD)
        hwndParent = GetParent(hWnd);
    else
        hwndParent = GetWindow(hWnd, GW_OWNER);
    SendMessage(hwndParent, WM_COMMAND, 
        MAKELONG(pData->dwBoxID, SSBN_SETFOCUS), (LPARAM)hWnd);  
    
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
    PLEVADJDATA    pData = NULL;
    pData = (PLEVADJDATA)GetUserData(hWnd);
	if (pData->hCapIconLit)
	{
		DeleteObject(pData->hCapIconLit);
	}
	if (pData->hCapIconVol)
	{
		DeleteObject(pData->hCapIconVol);
	}
}

static int StringToInt( char *str )
{
	int ret = 0;
	char *temp = str;
	if( *str == '-' || *str == '+')
		str++;
	while( *str >='0' && *str <= '9' )
	{
		ret = ret * 10;
		ret += *str - '0';
		str++;
	}
	if( *temp == '-' )
		ret = -ret;
	return ret;
}
