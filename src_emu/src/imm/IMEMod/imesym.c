/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : imesym.c
 *
 * Purpose  : Pollex alphabet input module.
 *            
\**************************************************************************/

#include "window.h"
#include "winpda.h"
#include "plx_pdaex.h"
#include "malloc.h"
#include "string.h"

#include "imm.h"

#define SYMIME_STATUS_SHOW    0x00000001
#define CARET_POS_START       0x00010000
#define CHAR_OUTPUT_OVERFLOW  0x00020000

#define NUM_GRID_ROW          5
#define NUM_GRID_COL          8
#define NUM_CHAR_ROW          ((int)(sizeof(awCharTable) / sizeof(awCharTable[0])))
#define NUM_CHAR_COL          NUM_GRID_COL
#define NUM_KEYTYPE           ((int)(sizeof(tabKeyType) / sizeof(tabKeyType[0])))
#define LINE_PRESCROLL        1

#define IDM_CANCEL    1001
#define RES_FOCUSBK   "ROM:ime/symfocus.bmp"
#define RES_FONT_LF   "ROM:ime/fontlf.bmp"
#define RES_FONT_SP   "ROM:ime/fontsp.bmp"

#define X_CHAR        (X_GRID + 1)
#define Y_CHAR        (Y_GRID + 1)
#define X_GRID        2
#define Y_GRID        4
#define CX_CHAR       20
#define CY_CHAR       28
#define CX_GRID       (CX_CHAR + 1)
#define CY_GRID       (CY_CHAR + 1)

#define CR_WHITE      RGB(255, 255, 255)
#define CR_BK         GetSysColor(COLOR_MENU) // RGB(239, 241, 246)
#define CR_GRID       RGB(185, 190, 210)
#define CR_TOP        RGB(160, 175, 230)

#define BB_X_CENTER(rc, bmp)   ((rc.left + rc.right - bmp.bmWidth) / 2)
#define BB_Y_CENTER(rc, bmp)   ((rc.top + rc.bottom - bmp.bmHeight) / 2)

typedef struct tagSYMIME
{
    DWORD    fdwStatus;
    int      nRepeat;
    char     *pszCharTable;
}
SYMIME, *PSYMIME, **PPSYMIME;

typedef struct tagSYMWNDDATA
{
    HWND     hwndEdit;
    HBITMAP  hbmpFocus;
    HBITMAP  hbmpFontLF;
    HBITMAP  hbmpFontSP;
    RECT     rcFocus;
    int      nRow;
    int      nCol;
    int      nMaxRow;
    char     *pszCharTable;
}
SYMWNDDATA, *PSYMWNDDATA;

static const WORD awCharTable[][NUM_CHAR_COL] =
{
#ifdef IME_NO_CHINESE
    {'.', ',', '"', '\'', '?', '!', ':', ';'},

    {0xFD, '+', '*', '-', '&', '/', '(', ')'},
    /* @ */
    {'<', '>', '=', '%', 0x11, 0x40, 0x60, '#'},

    {0xFB, 0x02, 0x01, 0x03, 0x5F, 0x24, 0xFF, 0x10},
          /* $ */
    {0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19},

    {0x1A, ' ', 0x0A0D}
#else
    {'.', ',', '"', '\'', '?', '!', ':', ';'},

    {'@', '+', '*', '-', '&', '/', '(', ')'},

    {'<', '>', '=', '%', '_', 0xBDAA, 0xC3AA, '#'},

    {0x80, '$', 0xA1AA, 0xA2AA, 0xC2AA, 0xBCAA, '~', 0xADAA},

    {0xAEAA, 0xAFAA, 0xB0AA, 0xB1AA, 0xB2AA, 0xB3AA, 0xB4AA, 0xB5AA},

    {0xB6AA, ' ', 0x0A0D}
#endif
};

static BOOL SymImeOpaInit(void);
static BOOL SymImeOpaCreate(PPSYMIME ppSym, WORD wType);
static BOOL SymImeOpaShow(PSYMIME pSym, BOOL bShow);
static BOOL SymImeOpaActivate(PSYMIME pSym, WORD wIDPrev, PVOID pPrev);
static BOOL SymImeOpaEvent(PSYMIME pSym, WPARAM wParam, LPARAM lParam);
static BOOL SymImeOpaDestroy(PSYMIME pSym);
static DWORD SymImeOpaCommand(PSYMIME pSym, WPARAM wParam, LPARAM lParam);

static BOOL SymImeKeyEvent(PSYMIME pSym, INT32 nKeyCode, DWORD dwKeyData);
static BOOL SymImeKeyDown(PSYMIME pSym, INT32 nKeyCode, DWORD dwKeyData);
static BOOL SymImeKeyUp(PSYMIME pSym, INT32 nKeyCode, DWORD dwKeyData);
static void CallNextIme(void);
static DWORD GetKeyType(WORD nKeyCode);
static WORD GetCharByRC(PSYMWNDDATA pSymWndData, int r, int c);
static int  GetCharTableRow(PSYMWNDDATA pSymWndData);

static LRESULT CALLBACK SymImeWndProc(HWND hWnd, UINT uMsg,
                                      WPARAM wParam, LPARAM lParam);
static BOOL RegisterSymImeClass(void);
static void OnCreate(HWND hWnd, LPARAM lParam);
static void OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnSize(HWND hWnd, LPARAM lParam);
static void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void OnPaint(HWND hWnd);
static void OnDestroy(HWND hWnd);
static void OnDirectionKey(HWND hWnd, int nKeyCode);
static void OnSelectKey(HWND hWnd, int nKeyCode);
static void OnVerticalKey(HWND hWnd, int nStyle);
static void OnHorizontalKey(HWND hWnd, int nStyle);
static void DrawCharacters(HWND hWnd, HDC hdc, PSYMWNDDATA pSymWndData);

static const KEYTYPE tabKeyType[] =
{
    {IME_EVT_F3,    IME_EVT_F3,    IME_HKEY_INSTANT, NULL},
    {IME_EVT_F4,    IME_EVT_F4,    IME_HKEY_INSTANT, NULL},
    {IME_EVT_CLEAR, IME_EVT_CLEAR, IME_HKEY_INSTANT, NULL}
};

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

DWORD SymImeWinMain(PVOID hThis, INT32 nCode, WPARAM nOne, LPARAM nTwo)
{
    switch (LOWORD(nCode))
    {
    case IME_OPA_INIT:
        return (DWORD)SymImeOpaInit();

    case IME_OPA_CREATE:
        return (DWORD)SymImeOpaCreate((PPSYMIME)hThis, HIWORD(nCode));

    case IME_OPA_SHOW:
        return (DWORD)SymImeOpaShow((PSYMIME)hThis, TRUE);

    case IME_OPA_HIDE:
        return (DWORD)SymImeOpaShow((PSYMIME)hThis, FALSE);

    case IME_OPA_ACTIVATE:
        return (DWORD)SymImeOpaActivate((PSYMIME)hThis, (WORD)nOne, (PVOID)nTwo);

    case IME_OPA_EVENT:
        return (DWORD)SymImeOpaEvent((PSYMIME)hThis, nOne, nTwo);

    case IME_OPA_DESTROY:
        return (DWORD)SymImeOpaDestroy((PSYMIME)hThis);

    case IME_OPA_COMMAND:
        return (DWORD)SymImeOpaCommand((PSYMIME)hThis, nOne, nTwo);
    }
    
    return (DWORD)FALSE;
}

/**********************************************************************
 * Function     SymImeOpaInit
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL SymImeOpaInit(void)
{
    return RegisterSymImeClass();
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL SymImeOpaCreate(PPSYMIME ppSym, WORD wType)
{
    PSYMIME pSym = NULL;

    if (ppSym == NULL)
    {
        return FALSE;
    }

    pSym = (PSYMIME)malloc(sizeof(SYMIME));
    if (pSym == NULL)
    {
        return FALSE;
    }
    memset(pSym, 0, sizeof(SYMIME));

    DefImeProc((PVOID)&pSym, IME_OPA_CREATE, 0, 0);

    pSym->fdwStatus = 0;

    *ppSym = pSym;

    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL SymImeOpaShow(PSYMIME pSym, BOOL bShow)
{
    if (pSym == NULL)
    {
        return FALSE;
    }

    if (bShow)
    {
        pSym->fdwStatus |= SYMIME_STATUS_SHOW;
        pSym->fdwStatus |= CARET_POS_START;
    }
    else
    {
        pSym->fdwStatus &= ~SYMIME_STATUS_SHOW;
        pSym->fdwStatus &= ~CARET_POS_START;
    }

    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL SymImeOpaActivate(PSYMIME pSym, WORD wIDPrev, PVOID pPrev)
{
    WORD wIDPhone = 0, wID = 0;
    PSYMWNDDATA pSymWndData = NULL;
    HWND hWnd = NULL;
    
    if (pPrev == NULL)
        return FALSE;
    
    pSymWndData = (PSYMWNDDATA)malloc(sizeof(SYMWNDDATA));
    memset((void*)pSymWndData, 0, sizeof(SYMWNDDATA));
    
    ImmGetImeIDByName(&wIDPhone, "Phone");
    if (wIDPrev == (int)wIDPhone)
    {
        DefImeProc(pPrev, IME_OPA_COMMAND, IME_FN_GETCHARTAB,
            (LPARAM)&pSymWndData->pszCharTable);
        if (pSymWndData->pszCharTable[0] == '\0')
        {
            return (BOOL)-1;
        }
    }
    else
    {
        pSymWndData->pszCharTable = NULL;
    }
    
    pSymWndData->hwndEdit = GetFocus();
    pSymWndData->nRow = 0;
    pSymWndData->nCol = 0;
    pSymWndData->nMaxRow = GetCharTableRow(pSymWndData);
    pSymWndData->rcFocus.left = X_CHAR;
    pSymWndData->rcFocus.top  = Y_CHAR;
    pSymWndData->rcFocus.right = pSymWndData->rcFocus.left + CX_CHAR;
    pSymWndData->rcFocus.bottom = pSymWndData->rcFocus.top + CY_CHAR;
    
    hWnd = CreateWindow(
        "SYMIME",
        "Characters",
        WS_VISIBLE | WS_CAPTION | WS_VSCROLL | PWS_STATICBAR,
        PLX_WIN_POSITION,
        NULL,
        NULL,
        NULL,
        (LPVOID)pSymWndData);
    
    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL SymImeOpaEvent(PSYMIME pSym, WPARAM wParam, LPARAM lParam)
{
    if (pSym == NULL)
    {
        return FALSE;
    }

    return SymImeKeyEvent(pSym, (INT32)wParam, (DWORD)lParam);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL SymImeOpaDestroy(PSYMIME pSym)
{
    if (pSym == NULL)
    {
        return FALSE;
    }

    free(pSym);

    return TRUE;
}


/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL SymImeKeyEvent(PSYMIME pSym, INT32 nKeyCode, DWORD dwKeyData)
{
    if (!ISKEYUP(dwKeyData))
    {
        return SymImeKeyDown(pSym, nKeyCode, dwKeyData);
    }
    else
    {
        return SymImeKeyUp(pSym, nKeyCode, dwKeyData);
    }
}

/**********************************************************************
 * Function     SymImeOpaCommand
 * Purpose      
 * Params       PSYMIME pSym, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static DWORD SymImeOpaCommand(PSYMIME pSym, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case IME_FN_GETKEYTYPE:
        return GetKeyType((WORD)lParam);

    default:
        return 0;
    }
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL SymImeKeyDown(PSYMIME pSym, INT32 nKeyCode, DWORD dwKeyData)
{
    int nIndex = 0;

    for (nIndex = 0; nIndex < NUM_KEYTYPE; nIndex++)
    {
        if ((tabKeyType[nIndex].wKeyEvtMin <= (WORD)nKeyCode)
            && ((WORD)nKeyCode <= tabKeyType[nIndex].wKeyEvtMax))
        {
            if (tabKeyType[nIndex].pfnOnKeyEvent == NULL)
                return FALSE;

            tabKeyType[nIndex].pfnOnKeyEvent(pSym, nKeyCode, dwKeyData);
            return TRUE;
        }
    }

    return DefImeProc((PVOID)pSym, IME_OPA_EVENT, (WPARAM)nKeyCode,
        (LPARAM)dwKeyData);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL SymImeKeyUp(PSYMIME pSym, INT32 nKeyCode, DWORD dwKeyData)
{
    BOOL bRet = FALSE;

    bRet = FALSE;
    return bRet;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static void CallNextIme(void)
{
    keybd_event(VK_F4, 0, 0, 0);
    keybd_event(VK_F4, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_F4, 0, 0, 0);
    keybd_event(VK_F4, 0, KEYEVENTF_KEYUP, 0);
}

/**********************************************************************
 * Function     GetKeyType
 * Purpose      
 * Params       wKeyCode
 * Return       
 * Remarks      
 **********************************************************************/

static DWORD GetKeyType(WORD wKeyCode)
{
    int i = 0;

    for (i = 0; i < NUM_KEYTYPE; i++)
    {
        if ((tabKeyType[i].wKeyEvtMin <= wKeyCode)
            && (wKeyCode <= tabKeyType[i].wKeyEvtMax))
        {
            return (DWORD)tabKeyType[i].wKeyType;
        }
    }

    return DefImeProc(NULL, IME_OPA_COMMAND, IME_FN_GETKEYTYPE, wKeyCode);
}

/*********************************************************************\
* Function	   RegisterSymImeClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL RegisterSymImeClass(void)
{
    WNDCLASS wc;
    
    wc.style         = CS_NOFOCUS;
    wc.lpfnWndProc   = SymImeWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "SYMIME";

    return RegisterClass(&wc);
}

/*********************************************************************\
* Function	   SymImeWndProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static LRESULT CALLBACK SymImeWndProc(HWND hWnd, UINT uMsg,
                                      WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        OnCreate(hWnd, lParam);
        return 0;   

    case WM_ACTIVATE:
        OnActivate(hWnd, wParam, lParam);
        return 0;

    case WM_SIZE:
        OnSize(hWnd, lParam);
        return 0;

    case WM_ERASEBKGND:
        return 0;

    case WM_KEYDOWN:
        OnKeyDown(hWnd, wParam, lParam);
        return 0;

    case WM_COMMAND:
        OnCommand(hWnd, wParam, lParam);
        return 0;

    case WM_PAINT:
        OnPaint(hWnd);
        return 0;

    case WM_DESTROY:
        OnDestroy(hWnd);
        return 0;

    default:
        return PDADefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

/*********************************************************************\
* Function	   OnCreate
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static void OnCreate(HWND hWnd, LPARAM lParam)
{
    PSYMWNDDATA     pSymWndData = NULL;
    LPCREATESTRUCT  lpcs = NULL;
    HDC             hdc = NULL;

    lpcs = (LPCREATESTRUCT)lParam;
    pSymWndData = (PSYMWNDDATA)lpcs->lpCreateParams;

    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CANCEL, 0),
        (LPARAM)"Cancel");

    pSymWndData->hbmpFocus = LoadImage(NULL, RES_FOCUSBK, IMAGE_BITMAP, 0, 0,
        LR_LOADFROMFILE);
    pSymWndData->hbmpFontLF = LoadImage(NULL, RES_FONT_LF, IMAGE_BITMAP, 0, 0,
        LR_LOADFROMFILE);
    pSymWndData->hbmpFontSP = LoadImage(NULL, RES_FONT_SP, IMAGE_BITMAP, 0, 0,
        LR_LOADFROMFILE);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static void OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    WORD wID = 0;

    ImmGetImeIDByName(&wID, "Sym");

    if (wParam == WA_ACTIVE)
    {
        SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Cancel"));
        SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SetFocus(hWnd);
        ImcShowImeByID(NULL, wID, TRUE);
    }
    else
    {
        ImcShowImeByID(NULL, wID, FALSE);
        PostMessage(hWnd, WM_CLOSE, 0, 0);
    }
}

/**********************************************************************
 * Function     OnSize
 * Purpose      
 * Params       HWND hWnd, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static void OnSize(HWND hWnd, LPARAM lParam)
{
    PSYMWNDDATA  pSymWndData = NULL;
    SCROLLINFO si;

    pSymWndData = (PSYMWNDDATA)GetWindowLong(hWnd, GWL_USERDATA);

    memset((void*)&si, 0, sizeof(SCROLLINFO));

    si.cbSize = sizeof(SCROLLINFO);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
    si.nMin   = 0;
    si.nMax   = pSymWndData->nMaxRow - 1;
    si.nPage  = NUM_GRID_ROW;

    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
}

/**********************************************************************
 * Function     OnKeyDown
 * Purpose      
 * Params       hWnd, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case VK_UP: case VK_DOWN: case VK_LEFT: case VK_RIGHT:
        OnDirectionKey(hWnd, wParam);
        break;

    case VK_1: case VK_2: case VK_3: case VK_4:
    case VK_5: case VK_6: case VK_7: case VK_8:
    case VK_F5:
        OnSelectKey(hWnd, wParam);
        break;

    default:
        PDADefWindowProc(hWnd, WM_KEYDOWN, wParam, lParam);
        break;
    }
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
    switch (LOWORD(wParam))
    {
    case IDM_CANCEL:
        SendMessage(hWnd, WM_CLOSE, 0, 0);
        break;

    default:
        break;
    }
}

/*********************************************************************\
* Function	   OnPaint
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static void OnPaint(HWND hWnd)
{
    PSYMWNDDATA  pSymWndData = NULL;
    HDC          hdc = NULL, hdcMem = NULL;
    PAINTSTRUCT  ps;
    RECT         rcClient;
    HPEN         hpenGrid = NULL, hpenTop = NULL, hpenLeft = NULL,
                 hpenOld = NULL;
    BITMAP       bmp;
    HBITMAP      hbmpGrid = NULL;
    int          i = 0, nPos = 0;

    pSymWndData = (PSYMWNDDATA)GetWindowLong(hWnd, GWL_USERDATA);

    hdc = BeginPaint(hWnd, &ps);

    GetClientRect(hWnd, &rcClient);

    hdcMem = CreateCompatibleDC(hdc);
    hbmpGrid = CreateCompatibleBitmap(hdcMem, rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top);
    SelectObject(hdcMem, (HGDIOBJ)hbmpGrid);

    ClearRect(hdcMem, &rcClient, CR_BK);

    hpenGrid = CreatePen(PS_SOLID, 1, CR_GRID);
    hpenTop = CreatePen(PS_SOLID, Y_GRID, CR_TOP);
    hpenLeft = CreatePen(PS_SOLID, X_GRID, CR_GRID);

    hpenOld = SelectObject(hdcMem, hpenGrid);
    for (i = 0; i < NUM_GRID_ROW + 1; i++)
    {
        nPos = rcClient.top + Y_GRID + i * CY_GRID;
        DrawLine(hdcMem, rcClient.left, nPos, rcClient.right, nPos);
    }
    for (i = 0; i < NUM_GRID_COL + 1; i++)
    {
        nPos = rcClient.left + X_GRID + i * CX_GRID;
        DrawLine(hdcMem, nPos, rcClient.top, nPos, rcClient.bottom);
    }
    SelectObject(hdcMem, hpenTop);
    DrawLine(hdcMem, rcClient.left + X_GRID, rcClient.top + (Y_GRID >> 1),
        rcClient.right, rcClient.top + (Y_GRID >> 1));
    SelectObject(hdcMem, hpenLeft);
    DrawLine(hdcMem, rcClient.left + (X_GRID >> 1), rcClient.top,
        rcClient.left + (X_GRID >> 1), rcClient.bottom);

    SelectObject(hdcMem, hpenOld);
    DeleteObject(hpenGrid);
    DeleteObject(hpenTop);
    DeleteObject(hpenLeft);

    GetObject(pSymWndData->hbmpFocus, sizeof(BITMAP), (void*)&bmp);
    StretchBlt(hdcMem, pSymWndData->rcFocus.left, pSymWndData->rcFocus.top, CX_CHAR, CY_CHAR,
        (HDC)pSymWndData->hbmpFocus, 0, 0, bmp.bmWidth, bmp.bmHeight, ROP_SRC);

    DrawCharacters(hWnd, hdcMem, pSymWndData);

    BitBlt(hdc, 0, 0, rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top, hdcMem, 0, 0, ROP_SRC);

    DeleteObject(hbmpGrid);
    DeleteDC(hdcMem);

    EndPaint(hWnd, &ps);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static void OnDestroy(HWND hWnd)
{
    PSYMWNDDATA pSymWndData = NULL;

    pSymWndData = (PSYMWNDDATA)GetWindowLong(hWnd, GWL_USERDATA);
    DeleteObject(pSymWndData->hbmpFocus);
    DeleteObject(pSymWndData->hbmpFontLF);
    DeleteObject(pSymWndData->hbmpFontSP);
    free(pSymWndData);
}

/**********************************************************************
 * Function     DrawCharacters
 * Purpose      
 * Params       hdc, pSymWndData
 * Return       
 * Remarks      
 **********************************************************************/

static void DrawCharacters(HWND hWnd, HDC hdc, PSYMWNDDATA pSymWndData)
{
    int        nRow = 0, nCol = 0, nMode = 0;
    RECT       rc;
    WORD       wszChar[2] = {0};
    BOOL       bFocus = FALSE;
    COLORREF   crOld = 0;
    SCROLLINFO si;
    HBITMAP    hbmpFont = NULL;
    BITMAP     bmp;

    rc.left = X_CHAR;
    rc.top  = Y_CHAR;
    rc.right = rc.left + CX_CHAR;
    rc.bottom = rc.top + CY_CHAR;

    nMode = SetBkMode(hdc, TRANSPARENT);

    si.cbSize = sizeof(SCROLLINFO);
    si.fMask  = SIF_POS;

    GetScrollInfo(hWnd, SB_VERT, &si);

    for (nRow = si.nPos; nRow < si.nPos + NUM_GRID_ROW; nRow++)
    {
        for (nCol = 0; nCol < NUM_CHAR_COL; nCol++)
        {
            wszChar[0] = GetCharByRC(pSymWndData, nRow, nCol);
            if (wszChar[0] == 0)
                return;

            bFocus = EqualRect((const RECT*)&rc, (const RECT*)&pSymWndData->rcFocus);

            if ((wszChar[0] != 0x0A0D) && (wszChar[0] != (WORD)' '))
            {
                if (bFocus)
                {
                    crOld = SetTextColor(hdc, CR_WHITE);
                }
                DrawText(hdc, (PCSTR)&wszChar, -1, &rc, DT_CENTER | DT_VCENTER);
                if (bFocus)
                {
                    SetTextColor(hdc, crOld);
                }
            }
            else
            {
                hbmpFont = (wszChar[0] == 0x0A0D)
                    ? pSymWndData->hbmpFontLF : pSymWndData->hbmpFontSP;
                GetObject(hbmpFont, sizeof(BITMAP), (void*)&bmp);
                nMode = SetBkMode(hdc, NEWTRANSPARENT);
                crOld = SetBkColor(hdc, CR_WHITE);

                BitBlt(hdc, BB_X_CENTER(rc, bmp), BB_Y_CENTER(rc, bmp),
                    bmp.bmWidth, bmp.bmHeight, (HDC)hbmpFont,
                    0, 0, (bFocus ? ROP_NSRC : ROP_SRC));

                SetBkColor(hdc, crOld);
                SetBkMode(hdc, nMode);
            }

            rc.left += CX_GRID;
            rc.right += CX_GRID;
        }
        
        rc.left = X_CHAR;
        rc.right = rc.left + CX_CHAR;
        rc.top   += CY_GRID;
        rc.bottom += CY_GRID;
    }

    SetBkMode(hdc, nMode);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static void OnDirectionKey(HWND hWnd, int nKeyCode)
{
    PSYMWNDDATA  pSymWndData = NULL;
    RECT         rc;

    pSymWndData = (PSYMWNDDATA)GetWindowLong(hWnd, GWL_USERDATA);

    GetClientRect(hWnd, &rc);
    InvalidateRect(hWnd, (const RECT*)&pSymWndData->rcFocus, TRUE);

    do
    {
        switch (nKeyCode)
        {
        case VK_UP:
        case VK_DOWN:
            OnVerticalKey(hWnd, nKeyCode);
            break;

        case VK_LEFT:
        case VK_RIGHT:
            OnHorizontalKey(hWnd, nKeyCode);
            break;
        }
    }
    while (GetCharByRC(pSymWndData, pSymWndData->nRow, pSymWndData->nCol) == 0);

    InvalidateRect(hWnd, (const RECT*)&pSymWndData->rcFocus, TRUE);
}

/**********************************************************************
 * Function     OnSelectKey
 * Purpose      
 * Params       hWnd, nKeyCode
 * Return       
 * Remarks      
 **********************************************************************/

static void OnSelectKey(HWND hWnd, int nKeyCode)
{
    PSYMWNDDATA  pSymWndData = NULL;
    int          nCol = 0;
    WORD         wChar = 0;

    pSymWndData = (PSYMWNDDATA)GetWindowLong(hWnd, GWL_USERDATA);

    nCol = (nKeyCode == VK_F5) ? pSymWndData->nCol : (nKeyCode - VK_1);

    wChar = GetCharByRC(pSymWndData, pSymWndData->nRow, nCol);

    if (wChar == 0)
        return;

    PostMessage(pSymWndData->hwndEdit, WM_CHAR, (WPARAM)wChar, 0);
    SendMessage(hWnd, WM_CLOSE, 0, 0);
}

/**********************************************************************
 * Function     OnVerticalKey
 * Purpose      
 * Params       hWnd, nStyle
 * Return       
 * Remarks      
 **********************************************************************/

static void OnVerticalKey(HWND hWnd, int nStyle)
{
    PSYMWNDDATA  pSymWndData = NULL;
    SCROLLINFO   si;
    int          nPosPrev = 0;

    pSymWndData = (PSYMWNDDATA)GetWindowLong(hWnd, GWL_USERDATA);

    memset((void*)&si, 0, sizeof(SCROLLINFO));
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask  = SIF_ALL;

    GetScrollInfo(hWnd, SB_VERT, &si);
    nPosPrev = si.nPos;

    switch (nStyle)
    {
    case VK_UP:
        if (pSymWndData->nRow > si.nMin)
        {
            if ((pSymWndData->nRow <= nPosPrev + LINE_PRESCROLL)
                && (si.nPos > 0))
            {
                si.nPos--;
            }
            else
            {
                OffsetRect(&pSymWndData->rcFocus, 0, -CY_GRID);
            }
            pSymWndData->nRow--;
        }
        else
        {
            OffsetRect(&pSymWndData->rcFocus, 0,
                (min(si.nMax, NUM_GRID_ROW - 1) - pSymWndData->nRow)
                * CY_GRID);
            pSymWndData->nRow = si.nMax;
            if (NUM_CHAR_ROW > NUM_GRID_ROW)
            {
                si.nPos = si.nMax - si.nPage + 1;
            }
        }
        break;

    case VK_DOWN:
        if (pSymWndData->nRow < si.nMax)
        {
            if ((pSymWndData->nRow >= nPosPrev + NUM_GRID_ROW - 1 - LINE_PRESCROLL)
                && (si.nPos < (int)(si.nMax - si.nPage + 1)))
            {
                si.nPos++;
            }
            else
            {
                OffsetRect(&pSymWndData->rcFocus, 0, CY_GRID);
            }
            pSymWndData->nRow++;
        }
        else
        {
            OffsetRect(&pSymWndData->rcFocus, 0, (nPosPrev - pSymWndData->nRow)
                * CY_GRID);
            pSymWndData->nRow = si.nMin;
            if (NUM_CHAR_ROW > NUM_GRID_ROW)
            {
                si.nPos = si.nMin;
            }
        }
        break;
    }

    if (si.nPos != nPosPrev)
    {
        si.fMask = SIF_POS;
        SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

/**********************************************************************
 * Function     OnHorizontalKey
 * Purpose      
 * Params       HWND hWnd, int nStyle
 * Return       
 * Remarks      
 **********************************************************************/

static void OnHorizontalKey(HWND hWnd, int nStyle)
{
    PSYMWNDDATA  pSymWndData = NULL;

    pSymWndData = (PSYMWNDDATA)GetWindowLong(hWnd, GWL_USERDATA);

    switch (nStyle)
    {
    case VK_LEFT:
        if (pSymWndData->nCol > 0)
        {
            OffsetRect(&pSymWndData->rcFocus, -CX_GRID, 0);
            pSymWndData->nCol--;
        }
        else
        {
            OffsetRect(&pSymWndData->rcFocus, (NUM_GRID_COL - 1) * CX_GRID, 0);
            pSymWndData->nCol = NUM_GRID_COL - 1;
        }
        break;
        
    case VK_RIGHT:
        if (pSymWndData->nCol < NUM_GRID_COL - 1)
        {
            OffsetRect(&pSymWndData->rcFocus, CX_GRID, 0);
            pSymWndData->nCol++;
        }
        else
        {
            OffsetRect(&pSymWndData->rcFocus, -(NUM_GRID_COL - 1) * CX_GRID, 0);
            pSymWndData->nCol = 0;
        }
        break;
    }
}

/**********************************************************************
 * Function     GetCharByRC
 * Purpose      
 * Params       pSymWndData, r, c
 * Return       
 * Remarks      
 **********************************************************************/

static WORD GetCharByRC(PSYMWNDDATA pSymWndData, int r, int c)
{
    WORD wChar = 0;

    if (pSymWndData->pszCharTable == NULL)
    {
        wChar = awCharTable[r][c];
    }
    else
    {
        if (NUM_GRID_COL * r + c < (int)strlen(pSymWndData->pszCharTable))
        {
            wChar = (WORD)pSymWndData->pszCharTable[NUM_GRID_COL * r + c];
        }
    }

    return wChar;
}

/**********************************************************************
 * Function     GetCharTableRow
 * Purpose      
 * Params       PSYMWNDDATA pSym
 * Return       
 * Remarks      
 **********************************************************************/

static int GetCharTableRow(PSYMWNDDATA pSymWndData)
{
    int nRow = 0;

    if (pSymWndData->pszCharTable == NULL)
    {
        nRow = NUM_CHAR_ROW;
    }
    else
    {
        nRow = (strlen(pSymWndData->pszCharTable) + (NUM_CHAR_COL - 1))
            / NUM_CHAR_COL;
    }

    return nRow;
}
