/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 *    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
 *                       All Rights Reserved
 *
 * Module   :  unitedit.c
 *
 * Purpose  :  Unit Edit Implementation
 *
\**************************************************************************/

#include "window.h"
#include "malloc.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "plx_pdaex.h"
#include "pubapp.h"
#include "math.h"

#define UE_STATUS_FOCUS    0x00000001
#define MAX_PATH           256

/**********************************************************************
 * Function     pfnProcess
 * Purpose      UEP_ALLOCATE
 * Params       wParam
 *                  Style of control (WORD)
 *              lParam
 *                  Pointer to array of integer (int*)
 *                  a[0] returns number of separator and a[1] returns
 *                  number of field
 * Return       Pointer to array of integer, size of array is value of
 *              a[1] (int*)
 *              Each element value specifies number of digits of every
 *              field
 * Remarks      
 **********************************************************************/

#define UEP_ALLOCATE       0x00000001
#define UEP_INIT           0x00000002
#define UEP_VALID          0x00000003
#define UEP_LIMIT          0x00000004
#define UEP_ALLVALID       0x00000005
#define UEP_SWITCHTAG      0x00000006
#define UEP_GETDATA        0x00000007

#define CX_LEFTMARGIN      5
#define CX_RIGHTMARGIN     3
#define CY_TOPMARGIN1      4
#define CY_TOPMARGIN2      0

#define IDT_CARET          1
#define ET_CARET           500

#define IDT_REPEAT         2
#define ET_REPEAT_FIRST    300
#define ET_REPEAT_LATER    100

// PAUSE_CARET show the character at caret immediately, all of following
// command will not affect the timer
#define PAUSE_CARET        BlinkCaret(hWnd, TRUE)
// RESET_CARET make the character to be hidden when the next timer arriving
#define RESET_CARET        pData->bCaretShow = FALSE
// FLASH_CARET refresh the character at caret compulsively
#define FLASH_CARET        BlinkCaret(hWnd, FALSE);  \
                           BlinkCaret(hWnd, TRUE)

#define FLAG_TAG           0x2000
#define IsNumberField(nID) ((((nID) & 0xF000) == 0x1000) ? TRUE : FALSE)
#define IsTagField(nID)    ((((nID) & 0xF000) == 0x2000) ? TRUE : FALSE)
#define DECFRAC(d, nDigits) ((int)(((d) - (int)(d)) * pow(10, (nDigits))))

typedef LRESULT (*PFNPROCESS)(UINT, WPARAM, LPARAM);

typedef struct tagFIELD
{
    int    nID;
    int    iStart;
    int    nDigits;
    char   *szData;
    PRECT  prcDigit;
}
FIELD, *PFIELD;

typedef struct tagFIELDLIST FIELDLIST, *PFIELDLIST;

struct tagFIELDLIST
{
    FIELD       fd;
    PFIELDLIST  pflNext;
    PFIELDLIST  pflPrev;
};

typedef struct tagUNITEDIT
{
    DWORD       dwStatus;
    HWND        hwndParent;
    DWORD       dwID;
    PFIELDLIST  pflHead;
    PFIELDLIST  pflCurrent;
    int         iCaret;
    int         nTotalDigits;
    PSTR        pszTitle;
    int         nTitleLen;
    char        *szSeparator;
    PRECT       prcSeparator;
    BOOL        bCaretShow;
    WORD        wKeyCode;
    int         nRepeats;
    PFNPROCESS  pfnProcess;
}
UNITEDIT, *PUNITEDIT;

static HBITMAP hbmpFocus = NULL;
static int g_cxMaxNumChar = 0;

static LRESULT CALLBACK UnitEditWndProc(HWND hWnd, UINT uMsgCmd,
                                        WPARAM wParam, LPARAM lParam);
static void OnCreate(HWND hWnd, LPARAM lParam);
static void OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnChar(HWND hWnd, WORD wChar);
static void OnPaint(HWND hWnd);
static void OnTimer(HWND hWnd, int nTimerID);
static void OnDestroy(HWND hWnd);
static LRESULT OnGetData(HWND hWnd, PVOID pParam);
static void OnSetSeparator(HWND hWnd, char chSeparator);
static BOOL OnSetTitle(HWND hWnd, LPARAM lParam);
static void OnInputKey(HWND hWnd, int nKeyCode);

static BOOL GetTitleRect(HWND hWnd, PRECT pRect);
static PFIELD GetField(PFIELDLIST pflHead, int nID);
static PFIELD GetFieldByFlag(PFIELDLIST pflHead, WORD wFlag);
static int  GetCharMaxExtent(HDC hdc, const char* szCharset);
static void BlinkCaret(HWND hWnd, BOOL bShow);
static void MoveCaret(HWND hWnd, BOOL bForward);
static void MoveCaretToField(HWND hWnd, int nID);
static void ChildRectToClient(HWND hWnd, HWND hwndChild, PRECT pRect);
static void FillField(PFIELDLIST pflNode, int nID, int nData, int nExtent,
                      PRECT prc);
static void FillDigitField(PFIELDLIST pflNode, int nID, int nData, PRECT prc);
static void FillSeparator(PUNITEDIT pData, int iIndex, char chSeparator,
                          int nExtent, PRECT prc);
extern int PLXOS_GetResourceDirectory(char *pathname);

static LRESULT Time24hrProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT Time12hrProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT DateProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT TimeZoneProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT IPAddrProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT Period24hrProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT Period12hrProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT LatitudeDMSProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT LatitudeDMProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT LatitudeDProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT LongitudeDMSProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT LongitudeDMProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT LongitudeDProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT UTMEastingProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);
static LRESULT UTMNorthingProcess(UINT uProcess, WPARAM wParam, LPARAM lParam);

/**********************************************************************
 * Function     UNITEDIT_RegisterClass
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/

BOOL UNITEDIT_RegisterClass(void)
{
    WNDCLASS wc;
    char szPathName[MAX_PATH] = "";

    wc.style          = CS_OWNDC;
    wc.lpfnWndProc    = UnitEditWndProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = sizeof(UNITEDIT);
    wc.hInstance      = NULL;
    wc.hIcon          = NULL;
    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground  = NULL;
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = "UNITEDIT";

    PLXOS_GetResourceDirectory(szPathName);
    strcat(szPathName, "listfocus.bmp");
    hbmpFocus = LoadImage(NULL, szPathName, IMAGE_BITMAP, 0, 0,
        LR_LOADFROMFILE);

    return RegisterClass(&wc);
}

/**********************************************************************
 * Function     UnitEditWndProc
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT CALLBACK UnitEditWndProc(HWND hWnd, UINT uMsgCmd,
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
        OnKeyDown(hWnd, wParam, lParam);
        return 0;

    case WM_KEYUP:
        OnKeyUp(hWnd, wParam, lParam);
        return 0;

    case WM_CHAR:
        OnChar(hWnd, (WORD)wParam);
        return 0;

    case WM_PAINT:
        OnPaint(hWnd);
        return 0;

    case WM_ERASEBKGND:
        return 0;

    case WM_TIMER:
        OnTimer(hWnd, (int)wParam);
        return 0;

    case WM_DESTROY:
        OnDestroy(hWnd);
        return 0;

    case UEM_GETDATA:
        return OnGetData(hWnd, (PVOID)lParam);

    case UEM_SETSEPARATOR:
        OnSetSeparator(hWnd, (char)lParam);
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
    PUNITEDIT       pData = NULL;
    LPCREATESTRUCT  lpcs = NULL;
    WORD            wStyle = 0;
    HDC             hdc = NULL;
    HFONT           hFont = NULL, hfontOld = NULL;
    int             i = 0, anFields[2] = {0}, *pnDigits = NULL,
                    iFieldStart = 0, nStrLen = 0, nDigits = 0;
    PFIELDLIST      pflNode = NULL;

    lpcs = (LPCREATESTRUCT)lParam;
    pData = (PUNITEDIT)GetUserData(hWnd);

    memset((void*)pData, 0, sizeof(UNITEDIT));

    wStyle = LOWORD(lpcs->style);
    pData->hwndParent = lpcs->hwndParent;
    pData->dwID       = (DWORD)lpcs->hMenu;

    if (g_cxMaxNumChar == 0)
    {
        hdc = GetDC(hWnd);
        g_cxMaxNumChar = GetCharMaxExtent(hdc, "0123456789");
        ReleaseDC(hWnd, hdc);
    }

    switch (LOBYTE(wStyle))
    {
    case UES_TIME:
        pData->pfnProcess = (wStyle & UES_TIME_12HR)
            ? Time12hrProcess : Time24hrProcess;
        break;

    case UES_DATE:
        pData->pfnProcess = DateProcess;
        break;

    case UES_TIMEZONE:
        pData->pfnProcess = TimeZoneProcess;
        break;

    case UES_IPADDR:
        pData->pfnProcess = IPAddrProcess;
        break;

    case UES_TIMEPERIOD:
        pData->pfnProcess = (wStyle & UES_TIME_12HR)
            ? Period12hrProcess : Period24hrProcess;
        break;

    case UES_COORDINATE:
        {
            switch (wStyle & 0x0F00)
            {
            case UES_LONGITUDE:
                {
                    switch (wStyle & 0xF000)
                    {
                    case UES_COORDINATE_DM:
                        pData->pfnProcess = LongitudeDMProcess;
                        break;

                    case UES_COORDINATE_D:
                        pData->pfnProcess = LongitudeDProcess;
                        break;

                    default: // UES_COORDINATE_DMS
                        pData->pfnProcess = LongitudeDMSProcess;
                        break;
                    }
                }
                break;

            case UES_UTM_EASTING:
                pData->pfnProcess = UTMEastingProcess;
                break;

            case UES_UTM_NORTHING:
                pData->pfnProcess = UTMNorthingProcess;
                break;

            default: // UES_LATITUDE
                switch (wStyle & 0xF000)
                {
                case UES_COORDINATE_DM:
                    pData->pfnProcess = LatitudeDMProcess;
                    break;

                case UES_COORDINATE_D:
                    pData->pfnProcess = LatitudeDProcess;
                    break;

                default: // UES_COORDINATE_DMS
                    pData->pfnProcess = LatitudeDMSProcess;
                    break;
                }
                break;
            }
        }
        break;

    default:
        pData->pfnProcess = Time24hrProcess;
        break;
    }

    pnDigits = (int*)pData->pfnProcess(UEP_ALLOCATE, (WPARAM)wStyle,
        (LPARAM)anFields);

    pData->szSeparator = (char*)calloc(anFields[0] + 1, sizeof(char));
    pData->prcSeparator = (PRECT)calloc(anFields[0], sizeof(RECT));

    for (i = 0; i < anFields[1]; i++)
    {
        if (IsTagField(pnDigits[i]))
        {
            nStrLen = pnDigits[i] ^ FLAG_TAG;
            nDigits = 1;
        }
        else
        {
            nStrLen = pnDigits[i];
            nDigits = pnDigits[i];
        }

        pflNode = (PFIELDLIST)malloc(sizeof(FIELDLIST));

        pflNode->fd.nID = 0;
        pflNode->fd.iStart = iFieldStart;
        pflNode->fd.nDigits = nDigits;
        pflNode->fd.szData = (char*)calloc(nStrLen + 1, sizeof(char));
        pflNode->fd.prcDigit = (PRECT)calloc(nDigits, sizeof(RECT));
        pflNode->pflNext = NULL;

        if (i == 0)
        {
            pflNode->pflPrev = NULL;
            pData->pflHead = pflNode;
        }
        else
        {
            pflNode->pflPrev = pData->pflCurrent;
            pData->pflCurrent->pflNext = pflNode;
        }

        pData->pflCurrent = pflNode;
        iFieldStart += nDigits;
    }

    pData->pflCurrent->pflNext = pData->pflHead;
    pData->pflHead->pflPrev = pData->pflCurrent;
    pData->nTotalDigits = iFieldStart;

    free((void*)pnDigits);

    pData->pfnProcess(UEP_INIT, (WPARAM)hWnd, lParam);

    OnSetTitle(hWnd, (LPARAM)lpcs->lpszName);
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
    PUNITEDIT pData = NULL;
    pData = (PUNITEDIT)GetUserData(hWnd);

    pData->dwStatus |= UE_STATUS_FOCUS;
    pData->pflCurrent = pData->pflHead;
    pData->iCaret = 0;
    while (IsTagField(pData->pflCurrent->fd.nID))
    {
        pData->pflCurrent = pData->pflCurrent->pflNext;
        pData->iCaret = pData->pflCurrent->fd.iStart;
        if (pData->pflCurrent == pData->pflHead)
            break;
    }
    InvalidateRect(hWnd, NULL, TRUE);
    SetTimer(hWnd, IDT_CARET, ET_CARET, NULL);
    RESET_CARET;
    SendMessage(pData->hwndParent, WM_COMMAND,
        MAKEWPARAM(pData->dwID, EN_SETFOCUS), (LPARAM)hWnd);
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
    PUNITEDIT pData = NULL;
    pData = (PUNITEDIT)GetUserData(hWnd);

    pData->dwStatus &= ~UE_STATUS_FOCUS;
    InvalidateRect(hWnd, NULL, TRUE);
    KillTimer(hWnd, IDT_CARET);
    KillTimer(hWnd, IDT_REPEAT);
    SendMessage(pData->hwndParent, WM_COMMAND,
        MAKEWPARAM(pData->dwID, EN_KILLFOCUS), (LPARAM)hWnd);
}

/**********************************************************************
 * Function     OnKeyDown
 * Purpose      
 * Params       HWND hWnd, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PUNITEDIT  pData = NULL;
    HWND       hwndParent = NULL;
    PFIELD     pField = NULL;
    WORD       wVirtKey = 0;
    int        nID = 0;

    pData = (PUNITEDIT)GetUserData(hWnd);

    hwndParent = GetParent(hWnd);
    if (hwndParent == NULL)
        return;

    wVirtKey = LOWORD(wParam);
    
    if ((pData->nRepeats > 0) && (wVirtKey != pData->wKeyCode))
    {
        KillTimer(hWnd, IDT_REPEAT);
        pData->nRepeats = 0;
    }

    pData->wKeyCode = wVirtKey;
    pData->nRepeats++;

    switch (wParam)
    {
    case VK_UP:
    case VK_DOWN:
        if (pData->pfnProcess(UEP_ALLVALID, (WPARAM)pData->pflHead,
            (LPARAM)&nID))
        {
            if (GetWindowLong(hWnd, GWL_STYLE) & CS_NOSYSCTRL)
            {
                SendMessage(hwndParent, WM_KEYDOWN, wParam, lParam);
            }
            else
            {
                SetFocus(GetNextDlgTabItem(hwndParent, hWnd, (wParam == VK_UP)));
            }
        }
        else
        {
            MoveCaretToField(hWnd, nID);
        }
        break;

    case VK_LEFT:
    case VK_RIGHT:
        MoveCaret(hWnd, (wParam == VK_RIGHT));
        if (pData->nRepeats == 1)
        {
            SetTimer(hWnd, IDT_REPEAT, ET_REPEAT_FIRST, NULL);
        }
        if (GetWindowLong(hWnd, GWL_STYLE) & CS_NOSYSCTRL)
        {
            SendMessage(hwndParent, WM_KEYDOWN, wParam, lParam);
        }
        break;

    case VK_F3:
    case VK_F4:
        OnInputKey(hWnd, wParam);
        break;

    default:
        SendMessage(hwndParent, WM_KEYDOWN, wParam, lParam);
        break;
    }
}

/**********************************************************************
 * Function     OnKeyUp
 * Purpose      
 * Params       HWND hWnd, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static void OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PUNITEDIT  pData = NULL;
    HWND       hwndParent = NULL;

    pData = (PUNITEDIT)GetUserData(hWnd);
    pData->nRepeats = 0;
    hwndParent = GetParent(hWnd);

    switch (LOWORD(wParam))
    {
    case VK_UP:
    case VK_DOWN:
        if (GetWindowLong(hWnd, GWL_STYLE) & CS_NOSYSCTRL)
        {
            SendMessage(hwndParent, WM_KEYUP, wParam, lParam);
        }
        break;

    case VK_LEFT:
    case VK_RIGHT:
        KillTimer(hWnd, IDT_REPEAT);
        if (GetWindowLong(hWnd, GWL_STYLE) & CS_NOSYSCTRL)
        {
            SendMessage(hwndParent, WM_KEYUP, wParam, lParam);
        }
        break;

    default:
        break;
    }
}

/**********************************************************************
 * Function     OnChar
 * Purpose      
 * Params       HWND hWnd, WORD wChar
 * Return       
 * Remarks      
 **********************************************************************/

static void OnChar(HWND hWnd, WORD wChar)
{
    PUNITEDIT pData = NULL;
    PFIELD pField = NULL;
    int iDigit = 0;
    BOOL bLimited = FALSE;
    RECT rcLimit;

    if (wChar < '0' || wChar > '9')
        return;

    pData = (PUNITEDIT)GetUserData(hWnd);
    pField = &pData->pflCurrent->fd;

    if (IsNumberField(pField->nID))
    {
        iDigit = pData->iCaret - pField->iStart;

        if (!pData->pfnProcess(UEP_VALID, (WPARAM)pData, (LPARAM)wChar))
        {
            // Warning tone.
            return;
        }

        BlinkCaret(hWnd, FALSE);
        pField->szData[iDigit] = (char)wChar;
        bLimited = pData->pfnProcess(UEP_LIMIT, (WPARAM)pData,
            (LPARAM)&rcLimit);
        if (bLimited)
        {
            InvalidateRect(hWnd, &rcLimit, TRUE);
        }
        MoveCaret(hWnd, TRUE);
    }
    else
    {
        pData->pfnProcess(UEP_SWITCHTAG, MAKEWPARAM(pField->nID, wChar),
            (LPARAM)pField->szData);
        FLASH_CARET;
    }
}

/**********************************************************************
 * Function     OnInputKey
 * Purpose      
 * Params       hWnd, nKeyCode
 * Return       
 * Remarks      
 **********************************************************************/

static void OnInputKey(HWND hWnd, int nKeyCode)
{
    PUNITEDIT pData = NULL;
    PFIELD pField = NULL;

    pData = (PUNITEDIT)GetUserData(hWnd);

    pField = GetFieldByFlag(pData->pflHead, FLAG_TAG);
    if (pField == NULL)
        return;

    pData->pfnProcess(UEP_SWITCHTAG, MAKEWPARAM(pField->nID, nKeyCode),
        (LPARAM)pField->szData);
    InvalidateRect(hWnd, pField->prcDigit, TRUE);
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
    PUNITEDIT     pData = NULL;
    HDC           hdc = NULL;
    PAINTSTRUCT   ps;
    RECT          rc;
    int           nMode = 0, i = 0;
    COLORREF      cr = 0;
    BITMAP        bmp;
    PFIELDLIST    pflNode = NULL;
    HFONT         hFont = NULL, hfontOld = NULL;
    HBRUSH        hBrush = NULL;

    pData = (PUNITEDIT)GetUserData(hWnd);

    hdc = BeginPaint(hWnd, &ps);

    GetClientRect(hWnd, &rc);
    if (pData->dwStatus & UE_STATUS_FOCUS)
    {
        GetObject(hbmpFocus, sizeof(BITMAP), (void*)&bmp);
        StretchBlt(hdc, rc.left, rc.top, rc.right - rc.left,
            rc.bottom - rc.top, (HDC)hbmpFocus, 0, 0, bmp.bmWidth,
            bmp.bmHeight, ROP_SRC);

        cr = SetTextColor(hdc, COLOR_WHITE);
    }
    else
    {
        hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLOREDIT,
            (WPARAM)hdc, (LPARAM)hWnd);
        FillRect(hdc, &rc, hBrush);
    }

    nMode = SetBkMode(hdc, TRANSPARENT);

    if (pData->pszTitle != NULL)
    {
        GetFontHandle(&hFont, SMALL_FONT);
        hfontOld = SelectObject(hdc, hFont);
        GetTitleRect(hWnd, &rc);

        DrawText(hdc, pData->pszTitle, pData->nTitleLen, &rc,
            DT_LEFT | DT_VCENTER);

        SelectObject(hdc, hfontOld);
    }

    for (i = 0; i < (int)strlen(pData->szSeparator); i++)
    {
        DrawText(hdc, (PCSTR)&pData->szSeparator[i], 1,
            (PRECT)&pData->prcSeparator[i], DT_CENTER | DT_VCENTER);
    }

    pflNode = pData->pflHead;

    do
    {
        if (IsNumberField(pflNode->fd.nID))
        {
            for (i = 0; i < pflNode->fd.nDigits; i++)
            {
                //            DrawRect(hdc, (const RECT*)&pflNode->fd.prcDigit[i]);
                DrawText(hdc, (PCSTR)&pflNode->fd.szData[i], 1,
                    (PRECT)&pflNode->fd.prcDigit[i], DT_CENTER | DT_VCENTER);
            }
        }
        else
        {
            DrawText(hdc, (PCSTR)pflNode->fd.szData, -1, pflNode->fd.prcDigit,
                DT_CENTER | DT_VCENTER);
        }
        pflNode = pflNode->pflNext;
    }
    while (pflNode != pData->pflHead);

    SetBkMode(hdc, nMode);
    if (pData->dwStatus & UE_STATUS_FOCUS)
    {
        SetTextColor(hdc, cr);
    }

    EndPaint(hWnd, &ps);
}

/**********************************************************************
 * Function     OnTimer
 * Purpose      
 * Params       HWND hWnd
 * Return       
 * Remarks      
 **********************************************************************/

static void OnTimer(HWND hWnd, int nTimerID)
{
    PUNITEDIT pData = NULL;

    pData = (PUNITEDIT)GetUserData(hWnd);

    switch (nTimerID)
    {
    case IDT_CARET:
        BlinkCaret(hWnd, pData->bCaretShow);
        pData->bCaretShow = !pData->bCaretShow;
        break;

    case IDT_REPEAT:
        if (pData->nRepeats == 1)
        {
            KillTimer(hWnd, IDT_REPEAT);
            SetTimer(hWnd, IDT_REPEAT, ET_REPEAT_LATER, NULL);
        }
        keybd_event(pData->wKeyCode, 0, 0, 0);
        break;

    default:
        break;
    }
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
    PUNITEDIT pData = NULL;
    PFIELDLIST pflCrash = NULL, pflLead = NULL;
    BOOL bCrashing = FALSE;

    pData = (PUNITEDIT)GetUserData(hWnd);

    for (pflCrash = pData->pflHead, pflLead = pflCrash->pflNext;
         pflCrash != pData->pflHead || !bCrashing;
         pflCrash = pflLead, pflLead = pflLead->pflNext, bCrashing = TRUE)
    {
        if (pflCrash->fd.szData != NULL)
        {
            free((void*)pflCrash->fd.szData);
        }
        if (pflCrash->fd.prcDigit != NULL)
        {
            free((void*)pflCrash->fd.prcDigit);
        }
        free((void*)pflCrash);
    }

    if (pData->szSeparator != NULL)
    {
        free((void*)pData->szSeparator);
    }
    if (pData->prcSeparator != NULL)
    {
        free((void*)pData->prcSeparator);
    }
    if (pData->pszTitle != NULL)
    {
        free((void*)pData->pszTitle);
    }
}

/**********************************************************************
 * Function     OnGetData
 * Purpose      
 * Params       hWnd, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnGetData(HWND hWnd, PVOID pParam)
{
    PUNITEDIT  pData = NULL;
    int        nID = 0;
    BOOL       bRet = 0;

    pData = (PUNITEDIT)GetUserData(hWnd);

    bRet = (BOOL)pData->pfnProcess(UEP_ALLVALID, (WPARAM)pData->pflHead,
        (LPARAM)&nID);
    if (!bRet)
    {
        MoveCaretToField(hWnd, nID);
    }

    pData->pfnProcess(UEP_GETDATA, (WPARAM)pData->pflHead, (LPARAM)pParam);

    return (LRESULT)bRet;
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
    PUNITEDIT    pData = NULL;
    int          nNewLen = 0;
    PSTR         pszNewTitle = NULL;
    RECT         rcTitle;

    pData = (PUNITEDIT)GetUserData(hWnd);

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
 * Function     OnSetSeparator
 * Purpose      
 * Params       hWnd, chSeparator
 * Return       
 * Remarks      
 **********************************************************************/

static void OnSetSeparator(HWND hWnd, char chSeparator)
{
    PUNITEDIT  pData = NULL;
    int        i = 0;

    if (chSeparator == '\0')
        return;

    pData = (PUNITEDIT)GetUserData(hWnd);

    for (i = 0; i < (int)strlen(pData->szSeparator); i++)
    {
        pData->szSeparator[i] = chSeparator;
        InvalidateRect(hWnd, &pData->prcSeparator[i], TRUE);
    }
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
    HFONT  hFont = NULL, hfontOld = NULL;
    SIZE   size;

    GetClientRect(hWnd, &rcClient);

    hdc = GetDC(hWnd);
    GetFontHandle(&hFont, SMALL_FONT);
    hfontOld = SelectObject(hdc, hFont);
    GetTextExtentPoint32(hdc, "T", -1, &size);
    SelectObject(hdc, hfontOld);
    ReleaseDC(hWnd, hdc);

    pRect->left   = rcClient.left + CX_LEFTMARGIN;
    pRect->top    = rcClient.top + CY_TOPMARGIN1;
    pRect->right  = rcClient.right - CX_RIGHTMARGIN;
    pRect->bottom = pRect->top + size.cy;

    return TRUE;
}

/**********************************************************************
 * Function     GetField
 * Purpose      
 * Params       int nID
 * Return       
 * Remarks      
 **********************************************************************/

static PFIELD GetField(PFIELDLIST pflHead, int nID)
{
    PFIELDLIST pflNode = NULL;

    pflNode = pflHead;
    do
    {
        if (pflNode->fd.nID == nID)
            return &pflNode->fd;

        pflNode = pflNode->pflNext;
    }
    while (pflNode != pflHead);

    return NULL;
}

/**********************************************************************
 * Function     GetField
 * Purpose      
 * Params       int nID
 * Return       
 * Remarks      
 **********************************************************************/

static PFIELD GetFieldByFlag(PFIELDLIST pflHead, WORD wFlag)
{
    PFIELDLIST pflNode = NULL;

    pflNode = pflHead;
    do
    {
        if (pflNode->fd.nID & wFlag)
            return &pflNode->fd;

        pflNode = pflNode->pflNext;
    }
    while (pflNode != pflHead);

    return NULL;
}

/**********************************************************************
 * Function     BlinkCaret
 * Purpose      
 * Params       bShow
 * Return       
 * Remarks      
 **********************************************************************/

static void BlinkCaret(HWND hWnd, BOOL bShow)
{
    PUNITEDIT pData = NULL;
    HDC hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;
    int iDigit = 0, nMode = 0;
    RECT rc, rcCaret;
    BITMAP bmp;
    COLORREF cr = 0;

    pData = (PUNITEDIT)GetUserData(hWnd);

    GetClientRect(hWnd, &rc);

    hdc = GetDC(hWnd);

    iDigit = pData->iCaret - pData->pflCurrent->fd.iStart;
    CopyRect(&rcCaret, (const RECT*)&pData->pflCurrent->fd.prcDigit[iDigit]);

    if (bShow)
    {
        cr = SetTextColor(hdc, COLOR_WHITE);
        nMode = SetBkMode(hdc, TRANSPARENT);

        if (IsNumberField(pData->pflCurrent->fd.nID))
        {
            DrawText(hdc, (PCSTR)&pData->pflCurrent->fd.szData[iDigit], 1,
                &rcCaret, DT_CENTER | DT_VCENTER);
        }
        else
        {
            DrawText(hdc, (PCSTR)pData->pflCurrent->fd.szData, -1,
                &rcCaret, DT_CENTER | DT_VCENTER);
        }

        SetBkMode(hdc, nMode);
        SetTextColor(hdc, cr);
    }
    else
    {
        rcCaret.bottom -= 3; // Only patch for StretchBlt
        SetClipRect(hdc, &rcCaret);
        GetObject(hbmpFocus, sizeof(BITMAP), (void*)&bmp);
        StretchBlt(hdc, rc.left, rc.top, rc.right - rc.left,
            rc.bottom - rc.top, (HDC)hbmpFocus, 0, 0, bmp.bmWidth,
            bmp.bmHeight, ROP_SRC);
        SetClipRect(hdc, NULL);
    }

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     MoveCaret
 * Purpose      
 * Params       hWnd, bForward
 * Return       
 * Remarks      
 **********************************************************************/

static void MoveCaret(HWND hWnd, BOOL bForward)
{
    PUNITEDIT  pData = NULL;
    PFIELD     pField = NULL;

    PAUSE_CARET;

    pData = (PUNITEDIT)GetUserData(hWnd);
    pField = &pData->pflCurrent->fd;

    if (bForward)
    {
        if (pData->iCaret < pField->iStart + pField->nDigits - 1)
        {
            pData->iCaret++;
        }
        else
        {
            pData->pflCurrent = pData->pflCurrent->pflNext;
            if (pData->pflCurrent != pData->pflHead)
            {
                pData->iCaret++;
            }
            else
            {
                pData->iCaret = 0;
            }
        }
    }
    else
    {
        if (pData->iCaret > pField->iStart)
        {
            pData->iCaret--;
        }
        else
        {
            if (pData->pflCurrent != pData->pflHead)
            {
                pData->iCaret--;
            }
            else
            {
                pData->iCaret = pData->nTotalDigits - 1;
            }
            pData->pflCurrent = pData->pflCurrent->pflPrev;
        }
    }

    RESET_CARET;
}

/**********************************************************************
 * Function     MoveCaretToField
 * Purpose      
 * Params       hWnd, nID
 * Return       
 * Remarks      
 **********************************************************************/

static void MoveCaretToField(HWND hWnd, int nID)
{
    PUNITEDIT  pData = NULL;

    pData = (PUNITEDIT)GetUserData(hWnd);

    PAUSE_CARET;

    while (pData->pflCurrent->fd.nID != nID)
    {
        pData->pflCurrent = pData->pflCurrent->pflNext;
    }
    pData->iCaret = pData->pflCurrent->fd.iStart;

    RESET_CARET;
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

/**********************************************************************
 * Function     GetCharMaxExtent
 * Purpose      
 * Params       hdc, szCharset
 * Return       
 * Remarks      
 **********************************************************************/

static int GetCharMaxExtent(HDC hdc, const char* szCharset)
{
    SIZE size;

    GetTextExtentPoint32(hdc, "0", -1, &size);
    
    return size.cx;
}

/**********************************************************************
 * Function     FillDigitField
 * Purpose      
 * Params       pflNode, nID, nData, cySeparate, prc
 * Return       
 * Remarks      
 **********************************************************************/

static void FillField(PFIELDLIST pflNode, int nID, int nData, int nExtent,
                      PRECT prc)
{
    int i = 0;
    char szFormat[16] = "";

    pflNode->fd.nID = nID;

    if (IsNumberField(nID))
    {
        sprintf(szFormat, "%%0%dd", pflNode->fd.nDigits);
        nData %= (int)pow(10, pflNode->fd.nDigits);
        sprintf(pflNode->fd.szData, (const char*)szFormat, nData);
        for (i = 0; i < pflNode->fd.nDigits; i++)
        {
            prc->left = prc->right;
            prc->right = prc->left + nExtent;
            CopyRect((PRECT)&pflNode->fd.prcDigit[i], prc);
        }
    }
    else
    {
        pflNode->fd.szData[0] = (char)nData;
        prc->left = prc->right;
        prc->right = prc->left + nExtent;
        CopyRect(pflNode->fd.prcDigit, prc);
    }
}

/**********************************************************************
 * Function     FillDigitField
 * Purpose      
 * Params       pflNode, nID, nData, cySeparate, prc
 * Return       
 * Remarks      
 **********************************************************************/

static void FillDigitField(PFIELDLIST pflNode, int nID, int nData, PRECT prc)
{
    FillField(pflNode, nID, nData, g_cxMaxNumChar, prc);
}

/**********************************************************************
 * Function     FillSeparator
 * Purpose      
 * Params       pchSeparator, chData, prc
 * Return       
 * Remarks      
 **********************************************************************/

static void FillSeparator(PUNITEDIT pData, int iIndex, char chSeparator,
                          int nExtent, PRECT prc)
{
    pData->szSeparator[iIndex] = chSeparator;
    prc->left = prc->right;
    prc->right = prc->left + nExtent;
    CopyRect((PRECT)&pData->prcSeparator[iIndex], (const RECT*)prc);
}

/***************************************************************************
 *
 * Module   :  TIME editor process function
 *
 * Purpose  :  
 *
\**************************************************************************/

#define IDF_HOUR            0x1001
#define IDF_MIN             0x1002
#define IDF_SEC             0x1003
#define IDF_MERIDIEM        0x2004

#define NUM_SEPARATOR_TIME       1

// 24 Hour

#define NUM_FIELD_TIME_24HR      2

static int* OnTime24hrAllocate(WORD wStyle, int *pnFields);
static void OnTime24hrInit(HWND hWnd, LPCREATESTRUCT lpcs);
static BOOL OnTime24hrValid(PUNITEDIT pData, int nCode);
static BOOL OnTime24hrLimit(PUNITEDIT pData, PRECT prcLimit);
static LRESULT OnTime24hrGetData(PFIELDLIST pflNode, PVOID pParam);

/**********************************************************************
 * Function     Time24hrProcess
 * Purpose      
 * Params       uProcess, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT Time24hrProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnTime24hrAllocate((WORD)wParam, (int*)lParam);
        break;
        
    case UEP_INIT:
        OnTime24hrInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = (LRESULT)OnTime24hrValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = (LRESULT)OnTime24hrLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_GETDATA:
        lRes = OnTime24hrGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnTime24hrAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnTime24hrAllocate(WORD wStyle, int *pnFields)
{
    int *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_TIME_24HR, sizeof(int));

    anDigits[0] = 2;
    anDigits[1] = 2;

    pnFields[0] = NUM_SEPARATOR_TIME;
    pnFields[1] = NUM_FIELD_TIME_24HR;

    return anDigits;
}

/**********************************************************************
 * Function     OnTime24hrInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnTime24hrInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    LPSYSTEMTIME lpst = NULL;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;

    pData = (PUNITEDIT)GetUserData(hWnd);
    lpst = (LPSYSTEMTIME)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    if (lpst->wHour > 23)
        lpst->wHour = 0;
    if (lpst->wMinute > 59)
        lpst->wMinute = 0;

    pflNode = pData->pflHead;
    FillDigitField(pflNode, IDF_HOUR, (int)lpst->wHour, &rc);
    FillSeparator(pData, 0, ':', GetCharMaxExtent(hdc, ":"), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_MIN, (int)lpst->wMinute, &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnTime24hrValid
 * Purpose      
 * Params       pField, iDigit
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnTime24hrValid(PUNITEDIT pData, int nCode)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_HOUR:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '2'))
            {
                return TRUE;
            }
        }
        else  // iDigit == 1
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                if ((pField->szData[0] == '2') && (nCode > '3'))
                {
                    return FALSE;
                }
                return TRUE;
            }
        }
        break;

    case IDF_MIN:
    case IDF_SEC:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '5'))
            {
                return TRUE;
            }
        }
        else  // iDigit == 1
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                return TRUE;
            }
        }
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnTime24hrLimit
 * Purpose      
 * Params       pData
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnTime24hrLimit(PUNITEDIT pData, PRECT prcLimit)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_HOUR:
        if (iDigit == 0)
        {
            if ((pField->szData[0] == '2') && (pField->szData[1] > '3'))
            {
                pField->szData[1] = '0';
                CopyRect(prcLimit, &pField->prcDigit[1]);
                return TRUE;
            }
        }
        // The case of (iDigit == 1) doesn't limit any other digit.
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnTime24hrGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnTime24hrGetData(PFIELDLIST pflNode, PVOID pParam)
{
    LPSYSTEMTIME lpst = NULL;

    lpst = (LPSYSTEMTIME)pParam;

    lpst->wHour = (WORD)atoi(GetField(pflNode, IDF_HOUR)->szData);
    lpst->wMinute = (WORD)atoi(GetField(pflNode, IDF_MIN)->szData);

    return (LRESULT)TRUE;
}

// 12 Hour

#define NUM_FIELD_TIME_12HR         3

static int* OnTime12hrAllocate(WORD wStyle, int *pnFields);
static void OnTime12hrInit(HWND hWnd, LPCREATESTRUCT lpcs);
static BOOL OnTime12hrValid(PUNITEDIT pData, int nCode);
static BOOL OnTime12hrLimit(PUNITEDIT pData, PRECT prcLimit);
static void OnTime12hrSwitchTag(int nID, WORD wChar, char* szData);
static LRESULT OnTime12hrGetData(PFIELDLIST pflNode, PVOID pParam);

/**********************************************************************
 * Function     Time12hrProcess
 * Purpose      
 * Params       uProcess, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT Time12hrProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnTime12hrAllocate((WORD)wParam, (int*)lParam);
        break;
        
    case UEP_INIT:
        OnTime12hrInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = (LRESULT)OnTime12hrValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = (LRESULT)OnTime12hrLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_SWITCHTAG:
        OnTime12hrSwitchTag(LOWORD(wParam), HIWORD(wParam), (char*)lParam);
        break;

    case UEP_GETDATA:
        lRes = OnTime12hrGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnTime12hrAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnTime12hrAllocate(WORD wStyle, int *pnFields)
{
    int *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_TIME_12HR, sizeof(int));

    anDigits[0] = 2;
    anDigits[1] = 2;
    anDigits[2] = 2 | FLAG_TAG;

    pnFields[0] = NUM_SEPARATOR_TIME;
    pnFields[1] = NUM_FIELD_TIME_12HR;

    return anDigits;
}

/**********************************************************************
 * Function     OnTime12hrInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnTime12hrInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    LPSYSTEMTIME lpst = NULL;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;
    BOOL bAM = FALSE;

    pData = (PUNITEDIT)GetUserData(hWnd);
    lpst = (LPSYSTEMTIME)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    if (lpst->wHour > 23)
        lpst->wHour = 0;

    if (lpst->wMinute > 59)
        lpst->wMinute = 0;

    if (lpst->wHour < 12)
        bAM = TRUE;

    if (lpst->wHour == 0)
    {
        lpst->wHour += 12;
    }
    else if (13 <= lpst->wHour && lpst->wHour <= 23)
    {
        lpst->wHour -= 12;
    }

    pflNode = pData->pflHead;
    FillDigitField(pflNode, IDF_HOUR, (int)lpst->wHour, &rc);
    FillSeparator(pData, 0, ':', GetCharMaxExtent(hdc, ":"), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_MIN, (int)lpst->wMinute, &rc);

    pflNode = pflNode->pflNext;
    pflNode->fd.nID = IDF_MERIDIEM;
    pflNode->fd.szData[0] = bAM ? 'A' : 'P';
    pflNode->fd.szData[1] = 'M';
    rc.left = rc.right;
    rc.right = rc.left + GetCharMaxExtent(hdc, "AP")
        + GetCharMaxExtent(hdc, "M") + 6;
    CopyRect(pflNode->fd.prcDigit, &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnTime12hrValid
 * Purpose      
 * Params       pField, iDigit
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnTime12hrValid(PUNITEDIT pData, int nCode)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_HOUR:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '1'))
            {
                return TRUE;
            }
        }
        else  // iDigit == 1
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                if (((pField->szData[0] == '0') && (nCode == '0'))
                    || ((pField->szData[0] == '1') && (nCode > '2')))
                {
                    return FALSE;
                }
                return TRUE;
            }
        }
        break;

    case IDF_MIN:
    case IDF_SEC:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '5'))
            {
                return TRUE;
            }
        }
        else  // iDigit == 1
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                return TRUE;
            }
        }
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnTime12hrLimit
 * Purpose      
 * Params       pData
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnTime12hrLimit(PUNITEDIT pData, PRECT prcLimit)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_HOUR:
        if (iDigit == 0)
        {
            if ((pField->szData[0] == '1') && (pField->szData[1] > '2'))
            {
                pField->szData[1] = '0';
                CopyRect(prcLimit, &pField->prcDigit[1]);
                return TRUE;
            }
            if ((pField->szData[0] == '0') && (pField->szData[1] == '0'))
            {
                pField->szData[1] = '1';
                CopyRect(prcLimit, &pField->prcDigit[1]);
                return TRUE;
            }
        }
        // The case of (iDigit == 1) doesn't limit any other digit.
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnTime12hrSwitchTag
 * Purpose      
 * Params       int nID, char *szData
 * Return       
 * Remarks      
 **********************************************************************/

static void OnTime12hrSwitchTag(int nID, WORD wChar, char* szData)
{
    const char* szTag = "AP";

    *szData = (*szData == szTag[0]) ? szTag[1] : szTag[0];
}

/**********************************************************************
 * Function     OnTime12hrGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnTime12hrGetData(PFIELDLIST pflNode, PVOID pParam)
{
    LPSYSTEMTIME lpst = NULL;
    int nHour = 0;
    BOOL bAm = FALSE;

    lpst = (LPSYSTEMTIME)pParam;

    bAm = (GetField(pflNode, IDF_MERIDIEM)->szData[0] == 'A');
    nHour = atoi(GetField(pflNode, IDF_HOUR)->szData);

    if (bAm)
    {
        if (nHour == 12)
            nHour -= 12;
    }
    else
    {
        if (nHour != 12)
            nHour += 12;
    }

    lpst->wHour = (WORD)nHour;
    lpst->wMinute = (WORD)atoi(GetField(pflNode, IDF_MIN)->szData);

    return (LRESULT)TRUE;
}

/***************************************************************************
 *
 * Module   :  DATE editor process function
 *
 * Purpose  :  
 *
\**************************************************************************/

#define IDF_YEAR            0x1001
#define IDF_MONTH           0x1002
#define IDF_DAY             0x1003

#define NUM_FIELD_DATE           3
#define NUM_SEPARATOR_DATE  (NUM_FIELD_DATE - 1)
#define IsLeapYear(y)      (((y) % 4 == 0) && ((y) % 100 != 0) || ((y) % 400 == 0))

static int* OnDateAllocate(WORD wStyle, int *pnFields);
static void OnDateInit(HWND hWnd, LPCREATESTRUCT lpcs);
static BOOL OnDateValid(PUNITEDIT pData, int nCode);
static BOOL OnDateLimit(PUNITEDIT pData, PRECT prcLimit);
static BOOL OnDateAllValid(PFIELDLIST pflNode, int* pnID);
static LRESULT OnDateGetData(PFIELDLIST pflNode, PVOID pParam);
static int  GetMaxDay(int nYear, int nMonth);

/**********************************************************************
 * Function     DateProcess
 * Purpose      
 * Params       hWnd, uMsg, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT DateProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnDateAllocate((WORD)wParam, (int*)lParam);
        break;
        
    case UEP_INIT:
        OnDateInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = (LRESULT)OnDateValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = (LRESULT)OnDateLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = (LRESULT)OnDateAllValid((PFIELDLIST)wParam, (int*)lParam);
        break;

    case UEP_GETDATA:
        lRes = OnDateGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnDateAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnDateAllocate(WORD wStyle, int *pnFields)
{
    int *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_DATE, sizeof(int));

    if (wStyle & UES_DATE_YMD)
    {
        anDigits[0] = 4;
        anDigits[1] = 2;
        anDigits[2] = 2;
    }
    else // UES_DATE_MDY || UES_DATE_DMY
    {
        anDigits[0] = 2;
        anDigits[1] = 2;
        anDigits[2] = 4;
    }

    pnFields[0] = NUM_SEPARATOR_DATE;
    pnFields[1] = NUM_FIELD_DATE;

    return anDigits;
}

/**********************************************************************
 * Function     OnDateInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnDateInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    LPSYSTEMTIME lpst = NULL;
    int i = 0, cxSeparator = 0, anID[NUM_FIELD_DATE] = {0},
        anData[NUM_FIELD_DATE] = {0};
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;
    DWORD dwStyle = 0;

    pData = (PUNITEDIT)GetUserData(hWnd);
    lpst = (LPSYSTEMTIME)lpcs->lpCreateParams;
    dwStyle = (DWORD)LOWORD(lpcs->style);

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    cxSeparator = GetCharMaxExtent(hdc, ".");

    if (dwStyle & UES_DATE_YMD)
    {
        anID[0] = IDF_YEAR;
        anID[1] = IDF_MONTH;
        anID[2] = IDF_DAY;

        anData[0] = (int)lpst->wYear;
        anData[1] = (int)lpst->wMonth;
        anData[2] = (int)lpst->wDay;
    }
    else if (dwStyle & UES_DATE_MDY)
    {
        anID[0] = IDF_MONTH;
        anID[1] = IDF_DAY;
        anID[2] = IDF_YEAR;

        anData[0] = (int)lpst->wMonth;
        anData[1] = (int)lpst->wDay;
        anData[2] = (int)lpst->wYear;
    }
    else
    {
        anID[0] = IDF_DAY;
        anID[1] = IDF_MONTH;
        anID[2] = IDF_YEAR;

        anData[0] = (int)lpst->wDay;
        anData[1] = (int)lpst->wMonth;
        anData[2] = (int)lpst->wYear;
    }

    pflNode = pData->pflHead;

    for (i = 0; i < NUM_FIELD_DATE - 1; i++)
    {
        FillDigitField(pflNode, anID[i], anData[i], &rc);
        FillSeparator(pData, i, '.', cxSeparator, &rc);
        pflNode = pflNode->pflNext;
    }
    FillDigitField(pflNode, anID[i], anData[i], &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnDateValid
 * Purpose      
 * Params       pField, iDigit
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnDateValid(PUNITEDIT pData, int nCode)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_YEAR:
        if (('0' <= nCode) && (nCode <= '9'))
        {
            return TRUE;
        }
        break;

    case IDF_MONTH:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '1'))
            {
                return TRUE;
            }
        }
        else // (iDigit == 1)
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                if ((pField->szData[0] == '0') && (nCode == '0'))
                {
                    return FALSE;
                }
                if ((pField->szData[0] == '1') && (nCode > '2'))
                {
                    return FALSE;
                }
                return TRUE;
            }
        }
        break;

    case IDF_DAY:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '3'))
            {
                return TRUE;
            }
        }
        else // (iDigit == 1)
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                if (((pField->szData[0] == '0') && (nCode == '0'))
                    || ((pField->szData[0] == '3') && (nCode > '1')))
                {
                    return FALSE;
                }
                return TRUE;
            }
        }
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnDateAllValid
 * Purpose      
 * Params       PUNITEDIT pData, int* pnID
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnDateAllValid(PFIELDLIST pflNode, int* pnID)
{
    int nYear = 0, nMonth = 0, nDay = 0;

    nYear = atoi(GetField(pflNode, IDF_YEAR)->szData);
    nMonth = atoi(GetField(pflNode, IDF_MONTH)->szData);
    nDay = atoi(GetField(pflNode, IDF_DAY)->szData);

    if (nDay <= GetMaxDay(nYear, nMonth))
        return TRUE;

    *pnID = IDF_DAY;
    return FALSE;
}

/**********************************************************************
 * Function     OnDateLimit
 * Purpose      
 * Params       pData
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnDateLimit(PUNITEDIT pData, PRECT prcLimit)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_MONTH:
        if (iDigit == 0)
        {
            if ((pField->szData[0] == '0') && (pField->szData[1] == '0'))
            {
                pField->szData[1] = '1';
                CopyRect(prcLimit, &pField->prcDigit[1]);
                return TRUE;
            }
            if ((pField->szData[0] == '1') && (pField->szData[1] > '2'))
            {
                pField->szData[1] = '0';
                CopyRect(prcLimit, &pField->prcDigit[1]);
                return TRUE;
            }
        }
        // The case of (iDigit == 1) doesn't limit any other digit.
        break;

    case IDF_DAY:
        if (iDigit == 0)
        {
            if ((pField->szData[0] == '0') && (pField->szData[1] == '0'))
            {
                pField->szData[1] = '1';
                CopyRect(prcLimit, &pField->prcDigit[1]);
                return TRUE;
            }
            if ((pField->szData[0] == '3') && (pField->szData[1] > '1'))
            {
                pField->szData[1] = '0';
                CopyRect(prcLimit, &pField->prcDigit[1]);
                return TRUE;
            }
        }
        // The case of (iDigit == 1) doesn't limit any other digit.
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnDateGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnDateGetData(PFIELDLIST pflNode, PVOID pParam)
{
    LPSYSTEMTIME lpst = NULL;

    lpst = (LPSYSTEMTIME)pParam;

    lpst->wYear = (WORD)atoi(GetField(pflNode, IDF_YEAR)->szData);
    lpst->wMonth = (WORD)atoi(GetField(pflNode, IDF_MONTH)->szData);
    lpst->wDay = (WORD)atoi(GetField(pflNode, IDF_DAY)->szData);

    return (LRESULT)TRUE;
}

/**********************************************************************
 * Function     GetMaxDay
 * Purpose      
 * Params       nYear, nMonth
 * Return       
 * Remarks      
 **********************************************************************/

static int GetMaxDay(int nYear, int nMonth)
{
    switch (nMonth)
    {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        return 31;

    case 4: case 6: case 9: case 11:
        return 30;

    case 2:
        return ((IsLeapYear(nYear)) ? 29 : 28);

    default:
        return 0;
    }
}

/***************************************************************************
 *
 * Module   :  ZONE TIME editor process function
 *
 * Purpose  :  
 *
\**************************************************************************/

#define IDF_TAG                 0x2001
#define IDF_ZONE_HOUR           0x1002
#define IDF_ZONE_MIN            0x1003

#define NUM_FIELD_TIMEZONE           3
#define NUM_SEPARATOR_TIMEZONE       1

static int* OnTimeZoneAllocate(WORD wStyle, int *pnFields);
static void OnTimeZoneInit(HWND hWnd, LPCREATESTRUCT lpcs);
static BOOL OnTimeZoneValid(PUNITEDIT pData, int nCode);
static BOOL OnTimeZoneLimit(PUNITEDIT pData, PRECT prcLimit);
static void OnTimeZoneSwitchTag(int nID, WORD wChar, char* szData);
static LRESULT OnTimeZoneGetData(PFIELDLIST pflNode, PVOID pParam);

/**********************************************************************
 * Function     TimeZoneProcess
 * Purpose      
 * Params       hWnd, uMsg, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT TimeZoneProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnTimeZoneAllocate((WORD)wParam, (int*)lParam);
        break;

    case UEP_INIT:
        OnTimeZoneInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = OnTimeZoneValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = OnTimeZoneLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_SWITCHTAG:
        OnTimeZoneSwitchTag(LOWORD(wParam), HIWORD(wParam), (char*)lParam);
        break;

    case UEP_GETDATA:
        lRes = OnTimeZoneGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnTimeZoneAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnTimeZoneAllocate(WORD wStyle, int *pnFields)
{
    int i = 0, *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_TIMEZONE, sizeof(int));

    anDigits[0] = 1 | FLAG_TAG;
    anDigits[1] = 2;
    anDigits[2] = 2;

    pnFields[0] = NUM_SEPARATOR_TIMEZONE;
    pnFields[1] = NUM_FIELD_TIMEZONE;

    return anDigits;
}

/**********************************************************************
 * Function     OnTimeZoneInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnTimeZoneInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    LPSYSTEMTIME lpst = NULL;
    int i = 0;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;
    int nHour = 0, nMinute = 0;
    BOOL bMinus = FALSE;

    pData = (PUNITEDIT)GetUserData(hWnd);
    lpst = (LPSYSTEMTIME)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.top = rcTitle.bottom;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    nHour = (SHORT)lpst->wHour;
    nMinute = (SHORT)lpst->wMinute;

    hdc = GetDC(hWnd);

    if (nHour != 0)
    {
        if (nHour < 0)
        {
            bMinus = TRUE;
        }
    }
    else if (nMinute < 0)
    {
        bMinus = TRUE;
    }

    pflNode = pData->pflHead;
    pflNode->fd.nID = IDF_TAG;
    pflNode->fd.szData[0] = bMinus ? '-' : '+';
    rc.left = rcTitle.left;
    rc.right = rc.left + GetCharMaxExtent(hdc, "+-");
    CopyRect((PRECT)&pflNode->fd.prcDigit[0], &rc);

    pflNode = pflNode->pflNext;
    if ((-12 <= nHour) && (nHour <= 14))
    {
        if (nHour < 0)
        {
            nHour = -nHour;
        }
    }
    else
    {
        nHour = 0;
    }
    FillDigitField(pflNode, IDF_ZONE_HOUR, nHour, &rc);
    FillSeparator(pData, 0, ':', GetCharMaxExtent(hdc, ":"), &rc);

    pflNode = pflNode->pflNext;
    for (i = 0; i < 4; i++)
    {
        if (nMinute == i * 15)
            break;
    }
    if (i >= 4)
    {
        nMinute = 0;
    }
    FillDigitField(pflNode, IDF_ZONE_MIN, nMinute, &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnTimeZoneValid
 * Purpose      
 * Params       pData, nCode
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnTimeZoneValid(PUNITEDIT pData, int nCode)
{
    PFIELD pField = NULL;
    int iDigit = 0;
    BOOL bMinus = FALSE;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_ZONE_HOUR:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '1'))
            {
                return TRUE;
            }
        }
        else // (iDigit == 1)
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                bMinus = (GetField(pData->pflHead, IDF_TAG)->szData[0] == '-');
                if (bMinus && (pField->szData[0] == '1'))
                {
                    if (('3' <= nCode) && (nCode <= '9'))
                        return FALSE;
                }
                if (!bMinus && (pField->szData[0] == '1'))
                {
                    if (('5' <= nCode) && (nCode <= '9'))
                        return FALSE;
                }
                return TRUE;
            }
        }
        break;

    case IDF_ZONE_MIN:
        if (iDigit == 0)
        {
            if ((nCode == '0') || (nCode == '1') || (nCode == '3')
                || (nCode == '4'))
            {
                return TRUE;
            }
        }
        else // (iDigit == 1)
        {
            if ((pField->szData[0] == '0') || (pField->szData[0] == '3'))
            {
                if (nCode == '0')
                    return TRUE;
            }
            if ((pField->szData[0] == '1') || (pField->szData[0] == '4'))
            {
                if (nCode == '5')
                    return TRUE;
            }
        }
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnTimeZoneLimit
 * Purpose      
 * Params       pData
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnTimeZoneLimit(PUNITEDIT pData, PRECT prcLimit)
{
    PFIELD pField = NULL;
    int iDigit = 0;
    BOOL bMinus = FALSE;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_ZONE_HOUR:
        if (iDigit == 0)
        {
            if (pField->szData[0] == '1')
            {
                bMinus = (GetField(pData->pflHead, IDF_TAG)->szData[0] == '-');

                if ((bMinus && (pField->szData[1] > '2'))
                    || (!bMinus && (pField->szData[1] > '4')))
                {
                    pField->szData[1] = '0';
                    CopyRect(prcLimit, &pField->prcDigit[1]);
                    return TRUE;
                }
            }
        }
        // The case of (iDigit == 1) doesn't limit any other digit.
        break;

    case IDF_ZONE_MIN:
        if (iDigit == 0)
        {
            if ((pField->szData[0] == '0') || (pField->szData[0] == '3'))
            {
                if (pField->szData[1] != '0')
                {
                    pField->szData[1] = '0';
                    CopyRect(prcLimit, &pField->prcDigit[1]);
                    return TRUE;
                }
            }
            if ((pField->szData[0] == '1') || (pField->szData[0] == '4'))
            {
                if (pField->szData[1] != '5')
                {
                    pField->szData[1] = '5';
                    CopyRect(prcLimit, &pField->prcDigit[1]);
                    return TRUE;
                }
            }
        }
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnTimeZoneSwitchTag
 * Purpose      
 * Params       int nID, char *szData
 * Return       
 * Remarks      
 **********************************************************************/

static void OnTimeZoneSwitchTag(int nID, WORD wChar, char* szData)
{
    const char* szTag = "+-";

    *szData = (*szData == szTag[0]) ? szTag[1] : szTag[0];
}

/**********************************************************************
 * Function     OnTimeZoneGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnTimeZoneGetData(PFIELDLIST pflNode, PVOID pParam)
{
    LPSYSTEMTIME lpst = NULL;
    BOOL bMinus = FALSE;

    lpst = (LPSYSTEMTIME)pParam;

    bMinus = (*pflNode->fd.szData == '-');
    pflNode = pflNode->pflNext;
    lpst->wHour = atoi(pflNode->fd.szData);
    pflNode = pflNode->pflNext;
    lpst->wMinute = atoi(pflNode->fd.szData);
    if (bMinus)
    {
        if (lpst->wHour != 0)
        {
            lpst->wHour = (WORD)-lpst->wHour;
        }
        else if (lpst->wMinute != 0)
        {
            lpst->wMinute = (WORD)-lpst->wMinute;
        }
    }

    return (LRESULT)TRUE;
}

/***************************************************************************
 *
 * Module   :  IP ADDRESS editor process function
 *
 * Purpose  :  
 *
\**************************************************************************/

#define IDF_OCTET_1           0x1001
#define IDF_OCTET_2           0x1002
#define IDF_OCTET_3           0x1003
#define IDF_OCTET_4           0x1004

#define NUM_FIELD_IPADDR           4
#define NUM_SEPARATOR_IPADDR  (NUM_FIELD_IPADDR - 1)

static int* OnIPAddrAllocate(WORD wStyle, int *pnFields);
static void OnIPAddrInit(HWND hWnd, LPCREATESTRUCT lpcs);
static BOOL OnIPAddrValid(PUNITEDIT pData, int nCode);
static BOOL OnIPAddrLimit(PUNITEDIT pData, PRECT prcLimit);
static LRESULT OnIPAddrGetData(PFIELDLIST pflNode, PVOID pParam);

/**********************************************************************
 * Function     IPAddrProcess
 * Purpose      
 * Params       hWnd, uMsg, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT IPAddrProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = FALSE;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnIPAddrAllocate((WORD)wParam, (int*)lParam);
        break;

    case UEP_INIT:
        OnIPAddrInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = OnIPAddrValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = OnIPAddrLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_GETDATA:
        lRes = OnIPAddrGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnIPAddrAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnIPAddrAllocate(WORD wStyle, int *pnFields)
{
    int i = 0, *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_IPADDR, sizeof(int));

    for (i = 0; i < NUM_FIELD_IPADDR; i++)
    {
        anDigits[i] = 3;
    }

    pnFields[0] = NUM_SEPARATOR_IPADDR;
    pnFields[1] = NUM_FIELD_IPADDR;

    return anDigits;
}

/**********************************************************************
 * Function     OnIPAddrInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnIPAddrInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    LPSYSTEMTIME lpst = NULL;
    int i = 0, anOctet[NUM_FIELD_IPADDR] = {0}, cxSeparator = 0;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;
    DWORD dwIP = 0;

    pData = (PUNITEDIT)GetUserData(hWnd);
    dwIP = (DWORD)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    cxSeparator = GetCharMaxExtent(hdc, ".");

    pflNode = pData->pflHead;
    FillDigitField(pflNode, IDF_OCTET_1, FIRST_IPADDRESS(dwIP), &rc);
    FillSeparator(pData, 0, '.', cxSeparator, &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_OCTET_2, SECOND_IPADDRESS(dwIP), &rc);
    FillSeparator(pData, 1, '.', cxSeparator, &rc);
    
    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_OCTET_3, THIRD_IPADDRESS(dwIP), &rc);
    FillSeparator(pData, 2, '.', cxSeparator, &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_OCTET_4, FOURTH_IPADDRESS(dwIP), &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnIPAddrValid
 * Purpose      
 * Params       pField, iDigit
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnIPAddrValid(PUNITEDIT pData, int nCode)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (iDigit)
    {
    case 0:
        if (('0' <= nCode) && (nCode <= '2'))
        {
            return TRUE;
        }
        break;

    case 1:
        if ((pField->szData[0] != '2') || (('0' <= nCode) && (nCode <= '5')))
        {
            return TRUE;
        }
        break;

    default: // 2
        if (((pField->szData[0] != '2') || (pField->szData[1] != '5'))
            || (('0' <= nCode) && (nCode <= '5')))
        {
            return TRUE;
        }
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnIPAddrLimit
 * Purpose      
 * Params       pData
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnIPAddrLimit(PUNITEDIT pData, PRECT prcLimit)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    if ((iDigit == 0) || (iDigit == 1))
    {
        if (pField->szData[0] == '2')
        {
            if (pField->szData[1] > '5')
            {
                pField->szData[1] = '5';
                pField->szData[2] = '5';
                UnionRect(prcLimit, &pField->prcDigit[1], &pField->prcDigit[2]);
                return TRUE;
            }
            else if ((pField->szData[1] == '5') && (pField->szData[2] > '5'))
            {
                pField->szData[2] = '5';
                CopyRect(prcLimit, &pField->prcDigit[2]);
                return TRUE;
            }
        }
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnIPAddrGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnIPAddrGetData(PFIELDLIST pflNode, PVOID pParam)
{
    PDWORD pdwIP = NULL;
    BYTE byIP[NUM_FIELD_IPADDR] = {0};
    int i = 0;

    pdwIP = (PDWORD)pParam;

    for (i = 0; i < NUM_FIELD_IPADDR; i++)
    {
        byIP[i] = atoi(pflNode->fd.szData);
        pflNode = pflNode->pflNext;
    }

    *pdwIP = MAKEIPADDRESS(byIP[0], byIP[1], byIP[2], byIP[3]);

    return (LRESULT)TRUE;
}

/***************************************************************************
 *
 * Module   :  TIME PERIOD editor process function
 *
 * Purpose  :  
 *
\**************************************************************************/

#define IDF_PERIOD_DAY             0x1001
#define IDF_PERIOD_HOUR            0x1002
#define IDF_PERIOD_MIN             0x1003
#define IDF_PERIOD_SEC             0x1004
#define IDF_PERIOD_MERIDIEM        0x2005

#define NUM_SEPARATOR_PERIOD       3

// 24 Hour

#define NUM_FIELD_PERIOD_24HR      4

static int* OnPeriod24hrAllocate(WORD wStyle, int *pnFields);
static void OnPeriod24hrInit(HWND hWnd, LPCREATESTRUCT lpcs);
static BOOL OnPeriod24hrValid(PUNITEDIT pData, int nCode);
static BOOL OnPeriod24hrLimit(PUNITEDIT pData, PRECT prcLimit);
static LRESULT OnPeriod24hrGetData(PFIELDLIST pflNode, PVOID pParam);

/**********************************************************************
 * Function     Period24hrProcess
 * Purpose      
 * Params       uProcess, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT Period24hrProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnPeriod24hrAllocate((WORD)wParam, (int*)lParam);
        break;
        
    case UEP_INIT:
        OnPeriod24hrInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = (LRESULT)OnPeriod24hrValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = (LRESULT)OnPeriod24hrLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_GETDATA:
        lRes = OnPeriod24hrGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnPeriod24hrAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnPeriod24hrAllocate(WORD wStyle, int *pnFields)
{
    int *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_PERIOD_24HR, sizeof(int));

    anDigits[0] = 2;
    anDigits[1] = 2;
    anDigits[2] = 2;
    anDigits[3] = 2;

    pnFields[0] = NUM_SEPARATOR_PERIOD;
    pnFields[1] = NUM_FIELD_PERIOD_24HR;

    return anDigits;
}

/**********************************************************************
 * Function     OnPeriod24hrInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnPeriod24hrInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    LPSYSTEMTIME lpst = NULL;
    int i = 0, cxSeparator = 0;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;

    pData = (PUNITEDIT)GetUserData(hWnd);
    lpst = (LPSYSTEMTIME)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    pflNode = pData->pflHead;
    FillDigitField(pflNode, IDF_PERIOD_DAY, lpst->wDay, &rc);
    FillSeparator(pData, 0, 'd', GetCharMaxExtent(hdc, "d"), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_PERIOD_HOUR, lpst->wHour, &rc);
    FillSeparator(pData, 1, ':', GetCharMaxExtent(hdc, ":"), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_PERIOD_MIN, lpst->wMinute, &rc);
    FillSeparator(pData, 2, '.', GetCharMaxExtent(hdc, "."), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_PERIOD_SEC, lpst->wSecond, &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnPeriod24hrValid
 * Purpose      
 * Params       pField, iDigit
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnPeriod24hrValid(PUNITEDIT pData, int nCode)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_PERIOD_DAY:
        return TRUE;

    case IDF_PERIOD_HOUR:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '2'))
            {
                return TRUE;
            }
        }
        else  // iDigit == 1
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                if ((pField->szData[0] == '2') && (nCode > '3'))
                {
                    return FALSE;
                }
                return TRUE;
            }
        }
        break;

    case IDF_PERIOD_MIN:
    case IDF_PERIOD_SEC:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '5'))
            {
                return TRUE;
            }
        }
        else  // iDigit == 1
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                return TRUE;
            }
        }
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnPeriod24hrLimit
 * Purpose      
 * Params       pData
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnPeriod24hrLimit(PUNITEDIT pData, PRECT prcLimit)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_PERIOD_HOUR:
        if (iDigit == 0)
        {
            if ((pField->szData[0] == '2') && (pField->szData[1] > '3'))
            {
                pField->szData[1] = '0';
                CopyRect(prcLimit, &pField->prcDigit[1]);
                return TRUE;
            }
        }
        // The case of (iDigit == 1) doesn't limit any other digit.
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnPeriod24hrGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnPeriod24hrGetData(PFIELDLIST pflNode, PVOID pParam)
{
    LPSYSTEMTIME lpst = NULL;

    lpst = (LPSYSTEMTIME)pParam;

    lpst->wDay = (WORD)atoi(GetField(pflNode, IDF_PERIOD_DAY)->szData);
    lpst->wHour = (WORD)atoi(GetField(pflNode, IDF_PERIOD_HOUR)->szData);
    lpst->wMinute = (WORD)atoi(GetField(pflNode, IDF_PERIOD_MIN)->szData);
    lpst->wSecond = (WORD)atoi(GetField(pflNode, IDF_PERIOD_SEC)->szData);

    return (LRESULT)TRUE;
}

// 12 Hour

#define NUM_FIELD_PERIOD_12HR         5

static int* OnPeriod12hrAllocate(WORD wStyle, int *pnFields);
static void OnPeriod12hrInit(HWND hWnd, LPCREATESTRUCT lpcs);
static BOOL OnPeriod12hrValid(PUNITEDIT pData, int nCode);
static BOOL OnPeriod12hrLimit(PUNITEDIT pData, PRECT prcLimit);
static LRESULT OnPeriod12hrGetData(PFIELDLIST pflNode, PVOID pParam);

/**********************************************************************
 * Function     Period12hrProcess
 * Purpose      
 * Params       uProcess, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT Period12hrProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnPeriod12hrAllocate((WORD)wParam, (int*)lParam);
        break;
        
    case UEP_INIT:
        OnPeriod12hrInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = (LRESULT)OnPeriod12hrValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = (LRESULT)OnPeriod12hrLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_GETDATA:
        lRes = OnPeriod12hrGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnPeriod12hrAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnPeriod12hrAllocate(WORD wStyle, int *pnFields)
{
    int *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_PERIOD_12HR, sizeof(int));

    anDigits[0] = 2;
    anDigits[1] = 2;
    anDigits[2] = 2;
    anDigits[3] = 2;
    anDigits[4] = 2 | FLAG_TAG;

    pnFields[0] = NUM_SEPARATOR_PERIOD;
    pnFields[1] = NUM_FIELD_PERIOD_12HR;

    return anDigits;
}

/**********************************************************************
 * Function     OnPeriod12hrInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnPeriod12hrInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    LPSYSTEMTIME lpst = NULL;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;
    int nHour = 0;

    pData = (PUNITEDIT)GetUserData(hWnd);
    lpst = (LPSYSTEMTIME)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    pflNode = pData->pflHead;
    FillDigitField(pflNode, IDF_PERIOD_DAY, (int)lpst->wDay, &rc);
    FillSeparator(pData, 0, 'd', GetCharMaxExtent(hdc, "d"), &rc);

    pflNode = pflNode->pflNext;
    nHour = (int)lpst->wHour;
    if (nHour > 12)
    {
        nHour -= 12;
    }
    else if (nHour == 0)
    {
        nHour += 12;
    }
    FillDigitField(pflNode, IDF_PERIOD_HOUR, nHour, &rc);
    FillSeparator(pData, 1, ':', GetCharMaxExtent(hdc, ":"), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_PERIOD_MIN, (int)lpst->wMinute, &rc);
    FillSeparator(pData, 2, '.', GetCharMaxExtent(hdc, "."), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_PERIOD_SEC, (int)lpst->wSecond, &rc);

    pflNode = pflNode->pflNext;
    pflNode->fd.nID = IDF_MERIDIEM;
    pflNode->fd.szData[0] = (nHour < 12) ? 'A' : 'P';
    pflNode->fd.szData[1] = 'M';
    rc.left = rc.right;
    rc.right = rc.left + GetCharMaxExtent(hdc, "AP")
        + GetCharMaxExtent(hdc, "M") + 6;
    CopyRect(pflNode->fd.prcDigit, &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnPeriod12hrValid
 * Purpose      
 * Params       pField, iDigit
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnPeriod12hrValid(PUNITEDIT pData, int nCode)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_PERIOD_DAY:
        return TRUE;

    case IDF_PERIOD_HOUR:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '1'))
            {
                return TRUE;
            }
        }
        else  // iDigit == 1
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                if (((pField->szData[0] == '0') && (nCode == '0'))
                    || ((pField->szData[0] == '1') && (nCode > '2')))
                {
                    return FALSE;
                }
                return TRUE;
            }
        }
        break;

    case IDF_PERIOD_MIN:
    case IDF_PERIOD_SEC:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '5'))
            {
                return TRUE;
            }
        }
        else  // iDigit == 1
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                return TRUE;
            }
        }
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnPeriod12hrLimit
 * Purpose      
 * Params       pData
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnPeriod12hrLimit(PUNITEDIT pData, PRECT prcLimit)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_PERIOD_HOUR:
        if (iDigit == 0)
        {
            if ((pField->szData[0] == '1') && (pField->szData[1] > '2'))
            {
                pField->szData[1] = '0';
                CopyRect(prcLimit, &pField->prcDigit[1]);
                return TRUE;
            }
            if ((pField->szData[0] == '0') && (pField->szData[1] == '0'))
            {
                pField->szData[1] = '1';
                CopyRect(prcLimit, &pField->prcDigit[1]);
                return TRUE;
            }
        }
        // The case of (iDigit == 1) doesn't limit any other digit.
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnPeriod12hrGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnPeriod12hrGetData(PFIELDLIST pflNode, PVOID pParam)
{
    LPSYSTEMTIME lpst = NULL;
    int nHour = 0;
    BOOL bAm = FALSE;

    lpst = (LPSYSTEMTIME)pParam;

    bAm = (GetField(pflNode, IDF_MERIDIEM)->szData[0] == 'A');
    nHour = atoi(GetField(pflNode, IDF_HOUR)->szData);

    if (bAm)
    {
        if (nHour == 12)
            nHour -= 12;
    }
    else
    {
        if (nHour != 12)
            nHour += 12;
    }

    lpst->wDay = (WORD)atoi(GetField(pflNode, IDF_PERIOD_DAY)->szData);
    lpst->wHour = (WORD)nHour;
    lpst->wMinute = (WORD)atoi(GetField(pflNode, IDF_PERIOD_MIN)->szData);
    lpst->wSecond = (WORD)atoi(GetField(pflNode, IDF_PERIOD_SEC)->szData);

    return (LRESULT)TRUE;
}

/***************************************************************************
 *
 * Module   :  COORDINATE editor process function
 *
 * Purpose  :  
 *
\**************************************************************************/

#define IDF_NS_EW           0x2001

// Latitude DMS

#define IDF_DEG_INT_NS      0x1002
#define IDF_DEG_INT_EW      0x1003
#define IDF_DEG_DEC         0x1004
#define IDF_MIN_INT         0x1005
#define IDF_MIN_DEC         0x1006
#define IDF_SEC_INT         0x1007
#define IDF_SEC_DEC         0x1008

#define NUM_FIELD_DMS       5
#define NUM_SEPARATOR_DMS   (NUM_FIELD_DMS - 1)

static int* OnLatitudeDMSAllocate(WORD wStyle, int *pnFields);
static void OnLatitudeDMSInit(HWND hWnd, LPCREATESTRUCT lpcs);
static BOOL OnDMSValid(PUNITEDIT pData, int nCode);
static BOOL OnDMSLimit(PUNITEDIT pData, PRECT prcLimit);
static LRESULT OnLatitudeDMSGetData(PFIELDLIST pflNode, PVOID pParam);
static void OnLatitudeSwitchTag(int nID, WORD wChar, char* szData);
static BOOL ClearLowerField(PUNITEDIT pData, PRECT prcLimit);

/**********************************************************************
 * Function     LatitudeDMSProcess
 * Purpose      
 * Params       uProcess, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT LatitudeDMSProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnLatitudeDMSAllocate((WORD)wParam, (int*)lParam);
        break;
        
    case UEP_INIT:
        OnLatitudeDMSInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = (LRESULT)OnDMSValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = (LRESULT)OnDMSLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_SWITCHTAG:
        OnLatitudeSwitchTag(LOWORD(wParam), HIWORD(wParam), (char*)lParam);
        break;

    case UEP_GETDATA:
        lRes = OnLatitudeDMSGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnLatitudeDMSAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnLatitudeDMSAllocate(WORD wStyle, int *pnFields)
{
    int *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_DMS, sizeof(int));

    anDigits[0] = 1;
    anDigits[1] = 2;
    anDigits[2] = 2;
    anDigits[3] = 2;
    anDigits[4] = 1;

    pnFields[0] = NUM_SEPARATOR_DMS;
    pnFields[1] = NUM_FIELD_DMS;

    return anDigits;
}

/**********************************************************************
 * Function     OnLatitudeDMSInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnLatitudeDMSInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    PANGLEDEG pad = NULL;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;

    pData = (PUNITEDIT)GetUserData(hWnd);
    pad = (PANGLEDEG)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    pflNode = pData->pflHead;
    FillField(pflNode, IDF_NS_EW, (char)(pad->bSW ? 'S' : 'N'),
        GetCharMaxExtent(hdc, "NS") + 6, &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_DEG_INT_NS, pad->nDegree, &rc);
    FillSeparator(pData, 0, (char)0xFE, GetCharMaxExtent(hdc, "D"), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_MIN_INT, pad->nMinute, &rc);
    FillSeparator(pData, 1, '\'', GetCharMaxExtent(hdc, "'"), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_SEC_INT, pad->nSecond, &rc);
    FillSeparator(pData, 2, '.', GetCharMaxExtent(hdc, "."), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_SEC_DEC, pad->nMilliseconds / 1000, &rc);
    FillSeparator(pData, 3, '"', GetCharMaxExtent(hdc, "\""), &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnLatitudeDMSValid
 * Purpose      
 * Params       pField, iDigit
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnDMSValid(PUNITEDIT pData, int nCode)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_DEG_INT_EW:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '1'))
            {
                return TRUE;
            }
        }
        else if (iDigit == 1)
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                if ((pField->szData[0] == '1') && (nCode > '8'))
                {
                    return FALSE;
                }
                return TRUE;
            }
        }
        else // iDigit == 2
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                if ((pField->szData[0] == '1') && (pField->szData[1] == '8')
                    && (nCode > '0'))
                {
                    return FALSE;
                }
                return TRUE;
            }
        }
        break;

    case IDF_DEG_INT_NS:
        if (iDigit == 0)
        {
            return TRUE;
        }
        else  // iDigit == 1
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                if ((pField->szData[0] == '9') && (nCode > '0'))
                {
                    return FALSE;
                }
                return TRUE;
            }
        }
        break;

    case IDF_DEG_DEC:
    case IDF_MIN_DEC:
    case IDF_SEC_DEC:
        return TRUE;

    case IDF_MIN_INT:
    case IDF_SEC_INT:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '5'))
            {
                return TRUE;
            }
        }
        else  // iDigit == 1
        {
            return TRUE;
        }
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnDMSLimit
 * Purpose      
 * Params       pData
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnDMSLimit(PUNITEDIT pData, PRECT prcLimit)
{
    PFIELD pField = NULL;
    int iDigit = 0;
    BOOL bRet = FALSE;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_DEG_INT_NS:
        if (iDigit == 0)
        {
            if (pField->szData[0] == '9')
            {
                if (pField->szData[1] > '0')
                {
                    pField->szData[1] = '0';
                    CopyRect(prcLimit, &pField->prcDigit[1]);
                    bRet = TRUE;
                }
                else
                {
                    SetRectEmpty(prcLimit);
                }
                if (ClearLowerField(pData, prcLimit))
                {
                    bRet = TRUE;
                }
                return bRet;
            }
        }
        else if (iDigit == 1)
        {
            if (pField->szData[0] == '9')
            {
                SetRectEmpty(prcLimit);
                if (ClearLowerField(pData, prcLimit))
                {
                    bRet = TRUE;
                }
                return bRet;
            }
        }
        break;

    case IDF_DEG_INT_EW:
        if (iDigit == 0)
        {
            if (pField->szData[0] == '1' && pField->szData[1] >= '8')
            {
                if (pField->szData[1] > '8')
                {
                    pField->szData[1] = '8';
                    CopyRect(prcLimit, &pField->prcDigit[1]);
                    bRet = TRUE;
                }
                else
                {
                    SetRectEmpty(prcLimit);
                }
                if (pField->szData[2] > '0')
                {
                    pField->szData[2] = '0';
                    UnionRect(prcLimit, prcLimit, &pField->prcDigit[2]);
                    bRet = TRUE;
                }
                if (ClearLowerField(pData, prcLimit))
                {
                    bRet = TRUE;
                }
                return bRet;
            }
        }
        else if (iDigit == 1)
        {
            if (pField->szData[0] == '1' && pField->szData[1] == '8')
            {
                if (pField->szData[2] > '0')
                {
                    pField->szData[2] = '0';
                    CopyRect(prcLimit, &pField->prcDigit[2]);
                    bRet = TRUE;
                }
                else
                {
                    SetRectEmpty(prcLimit);
                }
                if (ClearLowerField(pData, prcLimit))
                {
                    bRet = TRUE;
                }
                return bRet;
            }
        }
        else if (iDigit == 2)
        {
            if (pField->szData[0] == '1' && pField->szData[1] == '8')
            {
                SetRectEmpty(prcLimit);
                if (ClearLowerField(pData, prcLimit))
                {
                    bRet = TRUE;
                }
                return bRet;
            }
        }
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnTimeZoneSwitchTag
 * Purpose      
 * Params       int nID, char *szData
 * Return       
 * Remarks      
 **********************************************************************/

static void OnLatitudeSwitchTag(int nID, WORD wChar, char* szData)
{
    const char* szTag = "NS";

    *szData = (*szData == szTag[0]) ? szTag[1] : szTag[0];
}

/**********************************************************************
 * Function     OnLatitudeDMSGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnLatitudeDMSGetData(PFIELDLIST pflNode, PVOID pParam)
{
    PANGLEDEG pad = NULL;

    pad = (PANGLEDEG)pParam;

    pad->bSW = (GetField(pflNode, IDF_NS_EW)->szData[0] == 'S');

    pad->nDegree = atoi(GetField(pflNode, IDF_DEG_INT_NS)->szData);
    pad->nMinute = atoi(GetField(pflNode, IDF_MIN_INT)->szData);
    pad->nSecond = atoi(GetField(pflNode, IDF_SEC_INT)->szData);
    pad->nMilliseconds = 1000 * atoi(GetField(pflNode, IDF_SEC_DEC)->szData);

    return (LRESULT)TRUE;
}

/**********************************************************************
 * Function     ClearLowerField
 * Purpose      
 * Params       PUNITEDIT pData
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL ClearLowerField(PUNITEDIT pData, PRECT prcLimit)
{
    PFIELDLIST pflCur = NULL;
    int i = 0;
    BOOL bRet = FALSE;

    pflCur = pData->pflCurrent->pflNext;
    for (i = pflCur->fd.iStart; i < pData->nTotalDigits; i++)
    {
        if (i > pflCur->fd.iStart + pflCur->fd.nDigits - 1)
        {
            pflCur = pflCur->pflNext;
        }
        if (pflCur->fd.szData[i - pflCur->fd.iStart] > '0')
        {
            pflCur->fd.szData[i - pflCur->fd.iStart] = '0';
            UnionRect(prcLimit, prcLimit,
                &pflCur->fd.prcDigit[i - pflCur->fd.iStart]);
            bRet = TRUE;
        }
    }

    return bRet;
}

// Latitude DM

#define NUM_FIELD_DM        4
#define NUM_SEPARATOR_DM    (NUM_FIELD_DM - 1)

static int* OnLatitudeDMAllocate(WORD wStyle, int *pnFields);
static void OnLatitudeDMInit(HWND hWnd, LPCREATESTRUCT lpcs);
static LRESULT OnLatitudeDMGetData(PFIELDLIST pflNode, PVOID pParam);

/**********************************************************************
 * Function     LatitudeDMProcess
 * Purpose      
 * Params       uProcess, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT LatitudeDMProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnLatitudeDMAllocate((WORD)wParam, (int*)lParam);
        break;

    case UEP_INIT:
        OnLatitudeDMInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = (LRESULT)OnDMSValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = (LRESULT)OnDMSLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_SWITCHTAG:
        OnLatitudeSwitchTag(LOWORD(wParam), HIWORD(wParam), (char*)lParam);
        break;

    case UEP_GETDATA:
        lRes = OnLatitudeDMGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnLatitudeDMAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnLatitudeDMAllocate(WORD wStyle, int *pnFields)
{
    int *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_DM, sizeof(int));

    anDigits[0] = 1;
    anDigits[1] = 2;
    anDigits[2] = 2;
    anDigits[3] = 3;

    pnFields[0] = NUM_SEPARATOR_DM;
    pnFields[1] = NUM_FIELD_DM;

    return anDigits;
}

/**********************************************************************
 * Function     OnLatitudeDMInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnLatitudeDMInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    PANGLEDEG pad = NULL;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;
    double dMinute = 0.0;

    pData = (PUNITEDIT)GetUserData(hWnd);
    pad = (PANGLEDEG)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    pflNode = pData->pflHead;
    FillField(pflNode, IDF_NS_EW, (char)(pad->bSW ? 'S' : 'N'),
        GetCharMaxExtent(hdc, "NS") + 6, &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_DEG_INT_NS, pad->nDegree, &rc);
    FillSeparator(pData, 0, (char)0xFE, GetCharMaxExtent(hdc, "D"), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_MIN_INT, pad->nMinute, &rc);
    FillSeparator(pData, 1, '.', GetCharMaxExtent(hdc, "."), &rc);

    pflNode = pflNode->pflNext;
    dMinute = MSTOMIN(pad);
    FillDigitField(pflNode, IDF_MIN_DEC, DECFRAC(dMinute, 3), &rc);
    FillSeparator(pData, 2, '\'', GetCharMaxExtent(hdc, "'"), &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnLatitudeDMGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnLatitudeDMGetData(PFIELDLIST pflNode, PVOID pParam)
{
    PANGLEDEG pad = NULL;
    double dSecond = 0.0;

    pad = (PANGLEDEG)pParam;

    pad->bSW = (GetField(pflNode, IDF_NS_EW)->szData[0] == 'S');

    pad->nDegree = atoi(GetField(pflNode, IDF_DEG_INT_NS)->szData);
    pad->nMinute = atoi(GetField(pflNode, IDF_MIN_INT)->szData);
    dSecond = atoi(GetField(pflNode, IDF_MIN_DEC)->szData) / 1000.0;
    dSecond *= 60;
    pad->nSecond = (int)dSecond;
    pad->nMilliseconds = DECFRAC(dSecond, 3);

    return (LRESULT)TRUE;
}

// Latitude D

#define NUM_FIELD_D        3
#define NUM_SEPARATOR_D    (NUM_FIELD_D - 1)

static int* OnLatitudeDAllocate(WORD wStyle, int *pnFields);
static void OnLatitudeDInit(HWND hWnd, LPCREATESTRUCT lpcs);
static LRESULT OnLatitudeDGetData(PFIELDLIST pflNode, PVOID pParam);

/**********************************************************************
 * Function     LatitudeDProcess
 * Purpose      
 * Params       uProcess, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT LatitudeDProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnLatitudeDAllocate((WORD)wParam, (int*)lParam);
        break;
        
    case UEP_INIT:
        OnLatitudeDInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = (LRESULT)OnDMSValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = (LRESULT)OnDMSLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_SWITCHTAG:
        OnLatitudeSwitchTag(LOWORD(wParam), HIWORD(wParam), (char*)lParam);
        break;

    case UEP_GETDATA:
        lRes = OnLatitudeDGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnLatitudeDAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnLatitudeDAllocate(WORD wStyle, int *pnFields)
{
    int *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_D, sizeof(int));

    anDigits[0] = 1;
    anDigits[1] = 2;
    anDigits[2] = 5;

    pnFields[0] = NUM_SEPARATOR_D;
    pnFields[1] = NUM_FIELD_D;

    return anDigits;
}

/**********************************************************************
 * Function     OnLatitudeDInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnLatitudeDInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    PANGLEDEG pad = NULL;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;
    double dDegree = 0.0;

    pData = (PUNITEDIT)GetUserData(hWnd);
    pad = (PANGLEDEG)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    dDegree = DMSTODEG(pad);

    pflNode = pData->pflHead;
    FillField(pflNode, IDF_NS_EW, (char)(pad->bSW ? 'S' : 'N'),
        GetCharMaxExtent(hdc, "NS") + 6, &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_DEG_INT_NS, pad->nDegree, &rc);
    FillSeparator(pData, 0, '.', GetCharMaxExtent(hdc, "."), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_DEG_DEC, DECFRAC(dDegree, 5), &rc);
    FillSeparator(pData, 1, (char)0xFE, GetCharMaxExtent(hdc, "D"), &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnLatitudeDGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnLatitudeDGetData(PFIELDLIST pflNode, PVOID pParam)
{
    PANGLEDEG pad = NULL;
    double dDegree = 0.0;

    pad = (PANGLEDEG)pParam;

    pad->bSW = (GetField(pflNode, IDF_NS_EW)->szData[0] == 'S');

    dDegree = atoi(GetField(pflNode, IDF_DEG_INT_NS)->szData)
        + atoi(GetField(pflNode, IDF_DEG_DEC)->szData) / 100000.0;
    DEGTODMS(dDegree, pad);

    return (LRESULT)TRUE;
}

// Longitude DMS

static int* OnLongitudeDMSAllocate(WORD wStyle, int *pnFields);
static void OnLongitudeDMSInit(HWND hWnd, LPCREATESTRUCT lpcs);
static LRESULT OnLongitudeDMSGetData(PFIELDLIST pflNode, PVOID pParam);
static void OnLongitudeSwitchTag(int nID, WORD wChar, char* szData);

/**********************************************************************
 * Function     LongitudeDMSProcess
 * Purpose      
 * Params       uProcess, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT LongitudeDMSProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnLongitudeDMSAllocate((WORD)wParam, (int*)lParam);
        break;
        
    case UEP_INIT:
        OnLongitudeDMSInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = (LRESULT)OnDMSValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = (LRESULT)OnDMSLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_SWITCHTAG:
        OnLongitudeSwitchTag(LOWORD(wParam), HIWORD(wParam), (char*)lParam);
        break;

    case UEP_GETDATA:
        lRes = OnLongitudeDMSGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnLongitudeDMSAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnLongitudeDMSAllocate(WORD wStyle, int *pnFields)
{
    int *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_DMS, sizeof(int));

    anDigits[0] = 1;
    anDigits[1] = 3;
    anDigits[2] = 2;
    anDigits[3] = 2;
    anDigits[4] = 1;

    pnFields[0] = NUM_SEPARATOR_DMS;
    pnFields[1] = NUM_FIELD_DMS;

    return anDigits;
}

/**********************************************************************
 * Function     OnLongitudeDMSInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnLongitudeDMSInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    PANGLEDEG pad = NULL;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;

    pData = (PUNITEDIT)GetUserData(hWnd);
    pad = (PANGLEDEG)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    pflNode = pData->pflHead;
    FillField(pflNode, IDF_NS_EW, (char)(pad->bSW ? 'W' : 'E'),
        GetCharMaxExtent(hdc, "EW") + 6, &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_DEG_INT_EW, pad->nDegree, &rc);
    FillSeparator(pData, 0, (char)0xFE, GetCharMaxExtent(hdc, "D"), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_MIN_INT, pad->nMinute, &rc);
    FillSeparator(pData, 1, '\'', GetCharMaxExtent(hdc, "'"), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_SEC_INT, pad->nSecond, &rc);
    FillSeparator(pData, 2, '.', GetCharMaxExtent(hdc, "."), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_SEC_DEC, pad->nMilliseconds / 1000, &rc);
    FillSeparator(pData, 3, '"', GetCharMaxExtent(hdc, "\""), &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnLongitudeSwitchTag
 * Purpose      
 * Params       int nID, char *szData
 * Return       
 * Remarks      
 **********************************************************************/

static void OnLongitudeSwitchTag(int nID, WORD wChar, char* szData)
{
    const char* szTag = "EW";

    *szData = (*szData == szTag[0]) ? szTag[1] : szTag[0];
}

/**********************************************************************
 * Function     OnLongitudeDMSGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnLongitudeDMSGetData(PFIELDLIST pflNode, PVOID pParam)
{
    PANGLEDEG pad = NULL;

    pad = (PANGLEDEG)pParam;

    pad->bSW = (GetField(pflNode, IDF_NS_EW)->szData[0] == 'W');

    pad->nDegree = atoi(GetField(pflNode, IDF_DEG_INT_EW)->szData);
    pad->nMinute = atoi(GetField(pflNode, IDF_MIN_INT)->szData);
    pad->nSecond = atoi(GetField(pflNode, IDF_SEC_INT)->szData);
    pad->nMilliseconds = 1000 * atoi(GetField(pflNode, IDF_SEC_DEC)->szData);

    return (LRESULT)TRUE;
}

// Longitude DM

static int* OnLongitudeDMAllocate(WORD wStyle, int *pnFields);
static void OnLongitudeDMInit(HWND hWnd, LPCREATESTRUCT lpcs);
static LRESULT OnLongitudeDMGetData(PFIELDLIST pflNode, PVOID pParam);

/**********************************************************************
 * Function     LongitudeDMProcess
 * Purpose      
 * Params       uProcess, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT LongitudeDMProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnLongitudeDMAllocate((WORD)wParam, (int*)lParam);
        break;
        
    case UEP_INIT:
        OnLongitudeDMInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = (LRESULT)OnDMSValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = (LRESULT)OnDMSLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_SWITCHTAG:
        OnLongitudeSwitchTag(LOWORD(wParam), HIWORD(wParam), (char*)lParam);
        break;

    case UEP_GETDATA:
        lRes = OnLongitudeDMGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnLongitudeDMAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnLongitudeDMAllocate(WORD wStyle, int *pnFields)
{
    int *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_DM, sizeof(int));

    anDigits[0] = 1;
    anDigits[1] = 3;
    anDigits[2] = 2;
    anDigits[3] = 3;

    pnFields[0] = NUM_SEPARATOR_DM;
    pnFields[1] = NUM_FIELD_DM;

    return anDigits;
}

/**********************************************************************
 * Function     OnLongitudeDMInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnLongitudeDMInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    PANGLEDEG pad = NULL;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;
    double dMinute = 0.0;

    pData = (PUNITEDIT)GetUserData(hWnd);
    pad = (PANGLEDEG)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    pflNode = pData->pflHead;
    FillField(pflNode, IDF_NS_EW, (char)(pad->bSW ? 'W' : 'E'),
        GetCharMaxExtent(hdc, "EW") + 6, &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_DEG_INT_EW, pad->nDegree, &rc);
    FillSeparator(pData, 0, (char)0xFE, GetCharMaxExtent(hdc, "D"), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_MIN_INT, pad->nMinute, &rc);
    FillSeparator(pData, 1, '.', GetCharMaxExtent(hdc, "."), &rc);

    pflNode = pflNode->pflNext;
    dMinute = MSTOMIN(pad);
    FillDigitField(pflNode, IDF_MIN_DEC, DECFRAC(dMinute, 3), &rc);
    FillSeparator(pData, 2, '\'', GetCharMaxExtent(hdc, "'"), &rc);

    ReleaseDC(hWnd, hdc);
}

static LRESULT OnLongitudeDMGetData(PFIELDLIST pflNode, PVOID pParam)
{
    PANGLEDEG pad = NULL;
    double dSecond = 0.0;

    pad = (PANGLEDEG)pParam;

    pad->bSW = (GetField(pflNode, IDF_NS_EW)->szData[0] == 'W');

    pad->nDegree = atoi(GetField(pflNode, IDF_DEG_INT_EW)->szData);
    pad->nMinute = atoi(GetField(pflNode, IDF_MIN_INT)->szData);
    dSecond = atoi(GetField(pflNode, IDF_MIN_DEC)->szData) / 1000.0;
    dSecond *= 60;
    pad->nSecond = (int)dSecond;
    pad->nMilliseconds = DECFRAC(dSecond, 3);

    return (LRESULT)TRUE;
}

// Longitude D

#define NUM_FIELD_D        3
#define NUM_SEPARATOR_D    (NUM_FIELD_D - 1)

static int* OnLongitudeDAllocate(WORD wStyle, int *pnFields);
static void OnLongitudeDInit(HWND hWnd, LPCREATESTRUCT lpcs);
static LRESULT OnLongitudeDGetData(PFIELDLIST pflNode, PVOID pParam);

/**********************************************************************
 * Function     LongitudeDProcess
 * Purpose      
 * Params       uProcess, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT LongitudeDProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnLongitudeDAllocate((WORD)wParam, (int*)lParam);
        break;
        
    case UEP_INIT:
        OnLongitudeDInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = (LRESULT)OnDMSValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = (LRESULT)OnDMSLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_SWITCHTAG:
        OnLongitudeSwitchTag(LOWORD(wParam), HIWORD(wParam), (char*)lParam);
        break;

    case UEP_GETDATA:
        lRes = OnLongitudeDGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnLongitudeDAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnLongitudeDAllocate(WORD wStyle, int *pnFields)
{
    int *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_D, sizeof(int));

    anDigits[0] = 1;
    anDigits[1] = 3;
    anDigits[2] = 5;

    pnFields[0] = NUM_SEPARATOR_D;
    pnFields[1] = NUM_FIELD_D;

    return anDigits;
}

/**********************************************************************
 * Function     OnLongitudeDInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnLongitudeDInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    PANGLEDEG pad = NULL;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;
    double dDegree = 0.0;

    pData = (PUNITEDIT)GetUserData(hWnd);
    pad = (PANGLEDEG)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    dDegree = DMSTODEG(pad);

    pflNode = pData->pflHead;
    FillField(pflNode, IDF_NS_EW, (char)(pad->bSW ? 'W' : 'E'),
        GetCharMaxExtent(hdc, "EW") + 6, &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_DEG_INT_EW, pad->nDegree, &rc);
    FillSeparator(pData, 0, '.', GetCharMaxExtent(hdc, "."), &rc);

    pflNode = pflNode->pflNext;
    FillDigitField(pflNode, IDF_DEG_DEC, DECFRAC(dDegree, 5), &rc);
    FillSeparator(pData, 1, (char)0xFE, GetCharMaxExtent(hdc, "D"), &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnLongitudeDGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnLongitudeDGetData(PFIELDLIST pflNode, PVOID pParam)
{
    PANGLEDEG pad = NULL;
    double dDegree = 0.0;

    pad = (PANGLEDEG)pParam;

    pad->bSW = (GetField(pflNode, IDF_NS_EW)->szData[0] == 'W');

    dDegree = atoi(GetField(pflNode, IDF_DEG_INT_EW)->szData)
        + atoi(GetField(pflNode, IDF_DEG_DEC)->szData) / 100000.0;
    DEGTODMS(dDegree, pad);

    return (LRESULT)TRUE;
}

// UTM

#define IDF_ZONE_NUM              0x1001
#define IDF_ZONE_ID               0x2002
#define IDF_DIRECT_PART           0x1003

#define NUM_FIELD_UTM                  3

#define NUM_SEPARATOR_UTM_EAST         2

#define ET_MULTITAP                  400

static int* OnUTMEastingAllocate(WORD wStyle, int *pnFields);
static void OnUTMEastingInit(HWND hWnd, LPCREATESTRUCT lpcs);
static BOOL OnUTMValid(PUNITEDIT pData, int nCode);
static BOOL OnUTMLimit(PUNITEDIT pData, PRECT prcLimit);
static void OnUTMSwitchTag(int nID, WORD wChar, char* szData);
static LRESULT OnUTMEastingGetData(PFIELDLIST pflNode, PVOID pParam);

/**********************************************************************
 * Function     UTMEastingProcess
 * Purpose      
 * Params       uProcess, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT UTMEastingProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnUTMEastingAllocate((WORD)wParam, (int*)lParam);
        break;
        
    case UEP_INIT:
        OnUTMEastingInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = (LRESULT)OnUTMValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = (LRESULT)OnUTMLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_SWITCHTAG:
        OnUTMSwitchTag(LOWORD(wParam), HIWORD(wParam), (char*)lParam);
        break;

    case UEP_GETDATA:
        lRes = OnUTMEastingGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnUTMEastingAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnUTMEastingAllocate(WORD wStyle, int *pnFields)
{
    int *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_UTM, sizeof(int));

    anDigits[0] = 2;
    anDigits[1] = 1;
    anDigits[2] = 6;

    pnFields[0] = NUM_SEPARATOR_UTM_EAST;
    pnFields[1] = NUM_FIELD_UTM;

    return anDigits;
}

/**********************************************************************
 * Function     OnUTMEastingInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnUTMEastingInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    PUTMZONE puz = NULL;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;
    double dDegree = 0.0;

    pData = (PUNITEDIT)GetUserData(hWnd);
    puz = (PUTMZONE)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    pflNode = pData->pflHead;
    FillField(pflNode, IDF_ZONE_NUM, puz->byZoneNum, g_cxMaxNumChar, &rc);

    pflNode = pflNode->pflNext;
    FillField(pflNode, IDF_ZONE_ID, puz->chZoneID,
        GetCharMaxExtent(hdc, "W") + 6, &rc);
    FillSeparator(pData, 0, ' ', GetCharMaxExtent(hdc, " "), &rc);
    FillSeparator(pData, 1, '0', GetCharMaxExtent(hdc, "0"), &rc);

    pflNode = pflNode->pflNext;
    FillField(pflNode, IDF_DIRECT_PART, (int)puz->dwDirectPart, g_cxMaxNumChar,
        &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnUTMValid
 * Purpose      
 * Params       pField, iDigit
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnUTMValid(PUNITEDIT pData, int nCode)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_ZONE_NUM:
        if (iDigit == 0)
        {
            if (('0' <= nCode) && (nCode <= '6'))
            {
                return TRUE;
            }
        }
        else  // iDigit == 1
        {
            if (('0' <= nCode) && (nCode <= '9'))
            {
                if ((pField->szData[0] == '6') && (nCode > '0'))
                {
                    return FALSE;
                }
                return TRUE;
            }
        }
        break;

    case IDF_ZONE_ID:
        break;

    case IDF_DIRECT_PART:
        return TRUE;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnUTMLimit
 * Purpose      
 * Params       pData
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnUTMLimit(PUNITEDIT pData, PRECT prcLimit)
{
    PFIELD pField = NULL;
    int iDigit = 0;

    pField = &pData->pflCurrent->fd;
    iDigit = pData->iCaret - pField->iStart;

    switch (pField->nID)
    {
    case IDF_ZONE_NUM:
        if (iDigit == 0)
        {
            if ((pField->szData[0] == '6') && (pField->szData[1] > '0'))
            {
                pField->szData[1] = '0';
                CopyRect(prcLimit, &pField->prcDigit[1]);
                return TRUE;
            }
        }
        // The case of (iDigit == 1) doesn't limit any other digit.
        break;

    default:
        break;
    }

    return FALSE;
}

/**********************************************************************
 * Function     OnUTMSwitchTag
 * Purpose      
 * Params       int nID, char *szData
 * Return       
 * Remarks      
 **********************************************************************/

static void OnUTMSwitchTag(int nID, WORD wChar, char* szData)
{
    static const char *szAlphabet[] =
    {
        "ABC", "DEF", "GHI", "JKL", "MNO", "PQRS", "TUV", "WXYZ"
    };
    static WORD wCharPrev = 0;
    static DWORD dwTickPrev = 0;
    static int nRepeat = 0;
    DWORD dwTickNow = 0;
    int nInterval = 0;

    if (wChar < '2' || wChar > '9')
        return;

    dwTickNow = GetTickCount();
    nInterval = dwTickNow - dwTickPrev;
    if (nInterval < 0)
        nInterval = -nInterval;

    if ((nInterval <= ET_MULTITAP) && (wChar == wCharPrev))
    {
        nRepeat++;
        if (szAlphabet[wChar - '2'][nRepeat] == 0)
        {
            nRepeat = 0;
        }
    }
    else
    {
        nRepeat = 0;
    }
    wCharPrev = wChar;
    dwTickPrev = dwTickNow;

    *szData = szAlphabet[wChar - '2'][nRepeat];
}

/**********************************************************************
 * Function     OnUTMEastingGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnUTMEastingGetData(PFIELDLIST pflNode, PVOID pParam)
{
    PUTMZONE puz = NULL;

    puz = (PUTMZONE)pParam;

    puz->byZoneNum = atoi(GetField(pflNode, IDF_ZONE_NUM)->szData);
    puz->chZoneID = atoi(GetField(pflNode, IDF_ZONE_ID)->szData);
    puz->dwDirectPart = atoi(GetField(pflNode, IDF_DIRECT_PART)->szData);

    return (LRESULT)TRUE;
}

// UTM_EAST

#define NUM_SEPARATOR_UTM_NORTH       1

static int* OnUTMNorthingAllocate(WORD wStyle, int *pnFields);
static void OnUTMNorthingInit(HWND hWnd, LPCREATESTRUCT lpcs);
static LRESULT OnUTMNorthingGetData(PFIELDLIST pflNode, PVOID pParam);

/**********************************************************************
 * Function     UTMNorthingProcess
 * Purpose      
 * Params       uProcess, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT UTMNorthingProcess(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case UEP_ALLOCATE:
        lRes = (LRESULT)OnUTMNorthingAllocate((WORD)wParam, (int*)lParam);
        break;
        
    case UEP_INIT:
        OnUTMNorthingInit((HWND)wParam, (LPCREATESTRUCT)lParam);
        break;

    case UEP_VALID:
        lRes = (LRESULT)OnUTMValid((PUNITEDIT)wParam, (int)lParam);
        break;

    case UEP_LIMIT:
        lRes = (LRESULT)OnUTMLimit((PUNITEDIT)wParam, (PRECT)lParam);
        break;

    case UEP_ALLVALID:
        lRes = TRUE;
        break;

    case UEP_SWITCHTAG:
        OnUTMSwitchTag(LOWORD(wParam), HIWORD(wParam), (char*)lParam);
        break;

    case UEP_GETDATA:
        lRes = OnUTMNorthingGetData((PFIELDLIST)wParam, (PVOID)lParam);
        break;

    default:
        break;
    }

    return lRes;
}

/**********************************************************************
 * Function     OnUTMNorthingAllocate
 * Purpose      
 * Params       wStyle, pnFields
 * Return       
 * Remarks      
 **********************************************************************/

static int* OnUTMNorthingAllocate(WORD wStyle, int *pnFields)
{
    int *anDigits = NULL;

    anDigits = (int*)calloc(NUM_FIELD_UTM, sizeof(int));

    anDigits[0] = 2;
    anDigits[1] = 1;
    anDigits[2] = 7;

    pnFields[0] = NUM_SEPARATOR_UTM_NORTH;
    pnFields[1] = NUM_FIELD_UTM;

    return anDigits;
}

/**********************************************************************
 * Function     OnUTMNorthingInit
 * Purpose      
 * Params       uProcess, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnUTMNorthingInit(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PUNITEDIT pData = NULL;
    PFIELDLIST pflNode = NULL;
    PUTMZONE puz = NULL;
    RECT rcTitle, rc;
    HDC  hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;

    pData = (PUNITEDIT)GetUserData(hWnd);
    puz = (PUTMZONE)lpcs->lpCreateParams;

    GetTitleRect(hWnd, &rcTitle);
    rc.left = rcTitle.left;
    rc.top = rcTitle.bottom;
    rc.right = rc.left;
    rc.bottom = lpcs->cy - CY_TOPMARGIN2;

    hdc = GetDC(hWnd);

    pflNode = pData->pflHead;
    FillField(pflNode, IDF_ZONE_NUM, puz->byZoneNum, g_cxMaxNumChar, &rc);

    pflNode = pflNode->pflNext;
    FillField(pflNode, IDF_ZONE_ID, puz->chZoneID,
        GetCharMaxExtent(hdc, "W") + 6, &rc);
    FillSeparator(pData, 0, ' ', GetCharMaxExtent(hdc, " "), &rc);

    pflNode = pflNode->pflNext;
    FillField(pflNode, IDF_DIRECT_PART, (int)puz->dwDirectPart, g_cxMaxNumChar,
        &rc);

    ReleaseDC(hWnd, hdc);
}

/**********************************************************************
 * Function     OnUTMNorthingGetData
 * Purpose      
 * Params       pflNode, pParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnUTMNorthingGetData(PFIELDLIST pflNode, PVOID pParam)
{
    PUTMZONE puz = NULL;

    puz = (PUTMZONE)pParam;

    puz->byZoneNum = atoi(GetField(pflNode, IDF_ZONE_NUM)->szData);
    puz->chZoneID = atoi(GetField(pflNode, IDF_ZONE_ID)->szData);
    puz->dwDirectPart = atoi(GetField(pflNode, IDF_DIRECT_PART)->szData);

    return (LRESULT)TRUE;
}
