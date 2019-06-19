/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "hpwin.h"
#include "string.h"

typedef struct
{
    BOOL  bPenDown;			// if pen down
    //PSTR pszText;			// label
    BOOL  bDisabled;
	WORD  bColor;			// if BLACK-WHITE
	DWORD wState;			// style
    WORD  wID;
	BOOL  bFocus;
    HANDLE hImage;
} STATICDATA, *PSTATICDATA;

// Internal function prototypes
LRESULT CALLBACK STATIC_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                LPARAM lParam);
static void TEXT_Paint(HWND hWnd, HDC hdc, PSTATICDATA pStaticData, 
                       PSTR pszText, RECT* pRect);
static void FRAME_Paint(HWND hWnd, HDC hdc, PSTATICDATA pStaticData, RECT* pRect);
static void RECT_Paint(HWND hWnd, HDC hdc, PSTATICDATA pStaticData, RECT* pRect);
static void BITMAP_Paint(HWND hWnd, HDC hdc, PSTATICDATA pStaticData, RECT* pRect);
static void EraseRect(HWND hWnd, HDC hdc, RECT* pRect);

BOOL STATIC_RegisterClass(void)
{
    WNDCLASS wc;

    wc.style            = CS_OWNDC | CS_DEFWIN;//CS_PARENTDC;
    wc.lpfnWndProc      = STATIC_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(STATICDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "STATIC";

    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}

LRESULT CALLBACK STATIC_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                LPARAM lParam)
{
    LRESULT         lResult;
    PSTATICDATA     pStaticData;
    PCREATESTRUCT   pCreateStruct;
    HDC             hdc;
	HANDLE	        hImage;
	WORD	        fImageType;
    PSTR	        pszText;
	int		        nCount;
    BITMAP          bitmap;
    RECT            rect, rcParent;
    HFONT           hFont;
    BOOL            fRedraw;

    pStaticData = (PSTATICDATA)GetUserData(hWnd);
	lResult = 0;
	
    switch (wMsgCmd)
    {
    case WM_NCCREATE :

        pCreateStruct = (PCREATESTRUCT)lParam;

        memset(pStaticData, 0, sizeof(STATICDATA));

        // Initializes the internal data
		pStaticData->wState = LOWORD(pCreateStruct->style);
		pStaticData->wID = (WORD)(DWORD)pCreateStruct->hMenu;

		if (pStaticData->wState == SS_BITMAP || pStaticData->wState == SS_ICON)
		{
			pStaticData->hImage = (HANDLE)(pCreateStruct->lpCreateParams);

            if (pStaticData->hImage)
            {
                GetObject((HGDIOBJ)pStaticData->hImage, sizeof(BITMAP), &bitmap);
                pCreateStruct->cx = bitmap.bmWidth;
                pCreateStruct->cy = bitmap.bmHeight;
            }
		}
        // Creating succeed.
        lResult = (LRESULT)TRUE;

        break;

    case WM_SIZE :
        
        InvalidateRect(hWnd, NULL, TRUE);			
        break;

    case WM_NCDESTROY :

		if (pStaticData->hImage)
			DeleteObject((HDC)pStaticData->hImage);
/*
        // Free title text memory
        if (pStaticData->pszText)
        {
            MemFree(pStaticData->pszText);
            pStaticData->pszText = NULL;
        }
*/
        break;

    case WM_SETFONT:
        hFont = (HFONT)wParam;
        fRedraw = lParam;
        hdc = GetDC(hWnd);
        SelectObject(hdc, hFont);
        ReleaseDC(hWnd, hdc);

        if (fRedraw && IsWindowVisible(hWnd))
            InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT :

        hdc = BeginPaint(hWnd, NULL);
/*		
		nBitsPixel = GetDeviceCaps(hdc, BITSPIXEL);
		if (nBitsPixel == 1)
			pStaticData->bColor = FALSE;
		else 
			pStaticData->bColor = TRUE;
*/        
        GetClientRect(hWnd, &rect);

		switch (pStaticData->wState)
		{
			case SS_LEFT:
			case SS_CENTER:
			case SS_RIGHT:
			case SS_SIMPLE:

				nCount = DefWindowProc(hWnd, WM_GETTEXTLENGTH, (WPARAM)0, (LPARAM)0);

				pszText = (PSTR)LocalAlloc(LMEM_FIXED, nCount + 1);
				
				ASSERT(pszText != NULL);
				
				DefWindowProc(hWnd, WM_GETTEXT, (WPARAM)(nCount + 1),
                    (LPARAM)pszText);
				TEXT_Paint(hWnd, hdc, pStaticData, pszText, &rect);
				LocalFree((HANDLE)pszText);

				break;

			case SS_GRAYFRAME:
			case SS_BLACKFRAME:
			case SS_WHITEFRAME:
			
				FRAME_Paint(hWnd, hdc, pStaticData, &rect);
				break;

			case SS_BLACKRECT:
			case SS_GRAYRECT:
			case SS_WHITERECT:

				RECT_Paint(hWnd, hdc, pStaticData, &rect);
				break;
		
			case SS_BITMAP:
				
				BITMAP_Paint(hWnd, hdc, pStaticData, &rect);
				break;
/*
		case SS_ICON:

			ICON_Paint(hWnd, hdc, pStaticData->bColor, pStaticData->bDisabled,
							  pStaticData->pszText, pStaticData->wCheckState, 
							  pStaticData->bFocus);
			break;

*/
		default :
	
			break;
		}
        EndPaint(hWnd, NULL);

        break;
	
    case WM_ERASEBKGND:
        
        hdc = (HDC)wParam;

        GetClientRect(hWnd, &rect);

        EraseRect(hWnd, hdc, &rect);
        
        lResult = (LRESULT)TRUE;
        break;
  
    case WM_NCHITTEST:

        lResult = (LRESULT)HTTRANSPARENT;

        break;

    case WM_GETDLGCODE:

        lResult = (LRESULT)DLGC_STATIC;
        break;

	case STM_GETIMAGE :
		
		fImageType = (WORD)wParam;       // image-type flag 
		
		if (((fImageType == IMAGE_BITMAP) && (pStaticData->wState == SS_BITMAP)) ||
			((fImageType == IMAGE_ICON) && (pStaticData->wState == SS_ICON)))
		{
			lResult = (LRESULT)pStaticData->hImage;
		}

		break;

	case STM_SETIMAGE :
		
		fImageType = (WORD)wParam;       // image-type flag 
		hImage = (HANDLE) lParam;  // handle of the image 
 		
		if (((fImageType == IMAGE_BITMAP) && (pStaticData->wState == SS_BITMAP)) ||
			((fImageType == IMAGE_ICON) && (pStaticData->wState == SS_ICON)))
		{
			lResult = (LRESULT)pStaticData->hImage;
			pStaticData->hImage = hImage;

            if (pStaticData->hImage)
            {
                GetObject((HGDIOBJ)pStaticData->hImage, sizeof(BITMAP), &bitmap);
                
                GetWindowRectEx(hWnd, &rect, W_WINDOW, XY_SCREEN);
                GetWindowRectEx(GetParent(hWnd), &rcParent, W_CLIENT, XY_SCREEN);
                
                MoveWindow(hWnd, rect.left - rcParent.left, rect.top - rcParent.top, 
                    bitmap.bmWidth, bitmap.bmHeight, TRUE);
            }
		}

		break;

	case WM_SETTEXT :

        DefWindowProc(hWnd, wMsgCmd, wParam, lParam);

        hdc = GetDC(hWnd);

		if (pStaticData->wState == SS_LEFT || pStaticData->wState == SS_RIGHT || 
			pStaticData->wState == SS_CENTER || pStaticData->wState == SS_SIMPLE)
		{
            GetClientRect(hWnd, &rect);
            
            EraseRect(hWnd, hdc, &rect);
            
			TEXT_Paint(hWnd, hdc, pStaticData, (PSTR)lParam, &rect);
		}

        ReleaseDC(hWnd, hdc);

		break;

	default :

        lResult = DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
}

static void TEXT_Paint(HWND hWnd, HDC hdc, PSTATICDATA pStaticData, 
                       PSTR pszText, RECT* pRect)
{
    if (pStaticData->bColor)
	{
	}
	else
	{
		if (pszText)
		{
			switch (pStaticData->wState)
			{
				case SS_LEFT:
					DrawText(hdc, pszText, -1, pRect, DT_LEFT | DT_TOP);
					break;
				
				case SS_CENTER:
					DrawText(hdc, pszText, -1, pRect, DT_CENTER | DT_TOP);
					break;
				
				case SS_RIGHT:
					DrawText(hdc, pszText, -1, pRect, DT_RIGHT | DT_TOP);
					break;
				
				case SS_SIMPLE:
					DrawText(hdc, pszText, -1, pRect, DT_LEFT //| DT_SINGLELINE
							 | DT_TOP | DT_NOCLIP);
					break;
			}
		}
	}
}

static void FRAME_Paint(HWND hWnd, HDC hdc, PSTATICDATA pStaticData, RECT* pRect)
{
	HBRUSH	hBrush = NULL, hBrushOld;

	if (pStaticData->bColor)
	{
	}
	else
	{
		switch (pStaticData->wState)
		{
		case SS_BLACKFRAME :
			
			hBrush = GetStockObject(BLACK_BRUSH);
			break;

		case SS_GRAYFRAME :

			hBrush = GetStockObject(LTGRAY_BRUSH);
			break;

		case SS_WHITEFRAME :

			hBrush = GetStockObject(WHITE_BRUSH);
			break;

		default :

			break;
		}
		hBrushOld = SelectObject(hdc, hBrush);
		FrameRect(hdc, pRect, hBrush);
		SelectObject(hdc, hBrushOld);
	}
}

static void RECT_Paint(HWND hWnd, HDC hdc, PSTATICDATA pStaticData, RECT* pRect)
{
	HBRUSH	hBrush = NULL, hBrushOld;

	if (pStaticData->bColor)
	{
	}
	else
	{
		switch (pStaticData->wState)
		{
		case SS_BLACKRECT :
			
			hBrush = GetStockObject(BLACK_BRUSH);
			break;

		case SS_GRAYRECT :

			hBrush = GetStockObject(GRAY_BRUSH);
			break;

		case SS_WHITERECT :

			hBrush = GetStockObject(WHITE_BRUSH);
			break;

		default :

			break;
		}
		hBrushOld = SelectObject(hdc, hBrush);
		FillRect(hdc, pRect, hBrush);
		SelectObject(hdc, hBrushOld);
	}
}

static void BITMAP_Paint(HWND hWnd, HDC hdc, PSTATICDATA pStaticData, RECT* pRect)
{
	if (pStaticData->bColor)
	{
	}
	else
	{
        if (pStaticData->hImage)
        {
            BitBlt(hdc, pRect->left, pRect->top, 
                pRect->right - pRect->left, pRect->bottom - pRect->top,
                (HDC)pStaticData->hImage, 0, 0, ROP_SRC);
        }
	}
}

static void EraseRect(HWND hWnd, HDC hdc, RECT* pRect)
{
    HBRUSH  hBrush;
    HWND    hWndParent;
	DWORD	dwStyle;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    
    if (dwStyle & WS_CHILD)
        hWndParent = GetParent(hWnd);
    else
        hWndParent = GetWindow(hWnd, GW_OWNER);
    
    if (!hWndParent)
    {
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
        hBrush = (HBRUSH)(COLOR_WINDOW + 1);
    }
    else
    {
        hBrush = (HBRUSH)SendMessage(hWndParent, (WORD)WM_CTLCOLORSTATIC, 
            (WPARAM)hdc, (LPARAM)hWnd);
    }
    
    FillRect(hdc, pRect, hBrush);
}
