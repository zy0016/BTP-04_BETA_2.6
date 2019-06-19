/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 *    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
 *                       All Rights Reserved
 *
 * Module   :  langsel.c
 *
 * Purpose  :  Language selection interface
 *
\**************************************************************************/

#include "window.h"
#include "winpda.h"
#include "stdlib.h"
#include "string.h"
#include "plx_pdaex.h"

#include "imm.h"

#define ID_LANGLIST     1003
#define IDT_SELECT      1
#define ET_SELECT       500

#define MAX_LEN_CAPTION  64
#define MAX_LEN_SOFTKEY  32

#define RES_RADIO_OFF   "ROM:ime/radiooff.bmp"
#define RES_RADIO_ON    "ROM:ime/radioon.bmp"
#define RES_STR_LANG    ML("Writing Language")
#define RES_STR_CANCEL  ML("Cancel")

typedef struct tagIMELANGSEL
{
    HWND     hwndParent;
    HWND     hwndList;
    HBITMAP  hbmpRadio[2];
    int      nTimerID;
    int      iWriLang;
    char     szCaption[MAX_LEN_CAPTION];
    char     szLSoftKey[MAX_LEN_SOFTKEY];
    char     szRSoftKey[MAX_LEN_SOFTKEY];
    char     szMenuKey[MAX_LEN_SOFTKEY];
}
IMELANGSEL, *PIMELANGSEL;

static LRESULT CALLBACK ImeLangSelWndProc(HWND hWnd, UINT uMsg,
                                              WPARAM wParam, LPARAM lParam);
static void OnCreate(HWND hWnd, LPARAM lParam);
static void OnShowWindow(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnCommand(HWND hWnd, int nID, int nNotify, HWND hwndCtrl);
static void OnTimer(HWND hWnd, WPARAM wParam);
static void OnDestroy(HWND hWnd);
static void OnKeyDownF5(HWND hWnd);

static void FillListBox(PIMELANGSEL pUserData);

extern BOOL GetAutoLangState(void);
extern int  GetWriLanguage(void);
extern BOOL SetWriLanguage(int iIndex);

/**********************************************************************
 * Function     ImeLanguageSelWinMain
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

int ImeLangSelWinMain(void)
{
    HWND         hwndLangSel = NULL;
    RECT         rc;
    PIMELANGSEL  pUserData = NULL;

    pUserData = (PIMELANGSEL)malloc(sizeof(IMELANGSEL));
    memset((void*)pUserData, 0, sizeof(IMELANGSEL));

    pUserData->hwndParent = GetFocus();
    while (GetParent(pUserData->hwndParent) != NULL)
    {
        pUserData->hwndParent = GetParent(pUserData->hwndParent);
    }

    GetClientRect(pUserData->hwndParent, &rc);

    hwndLangSel = CreateWindow(
        "IMELANGSEL",
        "",
        WS_VISIBLE | WS_CHILD,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        pUserData->hwndParent,
        NULL,
        NULL,
        (LPVOID)pUserData
        );

    return 0;
}

/**********************************************************************
 * Function     RegisterImeLangSelClass
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/

BOOL RegisterImeLangSelClass(HINSTANCE hInst)
{
    WNDCLASS wc;

    wc.style         = 0;
    wc.lpfnWndProc   = ImeLangSelWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInst;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "IMELANGSEL";

    return RegisterClass(&wc);
}

/**********************************************************************
 * Function     ImeLangSelWndProc
 * Purpose      
 * Params       hWnd, uMsg, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT CALLBACK ImeLangSelWndProc(HWND hWnd, UINT uMsg,
                                          WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        OnCreate(hWnd, lParam);
        return 0;

    case WM_SHOWWINDOW:
        OnShowWindow(hWnd, wParam, lParam);
        return 0;

    case WM_KEYDOWN:
        OnKeyDown(hWnd, wParam, lParam);
        return 0;

    case WM_COMMAND:
        OnCommand(hWnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
        return 0;

    case WM_TIMER:
        OnTimer(hWnd, wParam);
        return 0;

    case WM_DESTROY:
        OnDestroy(hWnd);
        return 0;
        
    default:
        return PDADefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

/**********************************************************************
 * Function     OnCreate
 * Purpose      
 * Params       hWnd, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static void OnCreate(HWND hWnd, LPARAM lParam)
{
    PIMELANGSEL  pUserData = NULL;
    RECT         rc;

    pUserData = (PIMELANGSEL)GetWindowLong(hWnd, GWL_USERDATA);

    GetClientRect(hWnd, &rc);

    pUserData->hwndList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        hWnd,
        (HMENU)ID_LANGLIST,
        NULL,
        NULL);

    pUserData->hbmpRadio[0] = LoadImage(NULL, RES_RADIO_OFF, IMAGE_BITMAP,
        20, 20, LR_LOADFROMFILE);
    pUserData->hbmpRadio[1] = LoadImage(NULL, RES_RADIO_ON, IMAGE_BITMAP,
        20, 20, LR_LOADFROMFILE);

    FillListBox(pUserData);

    GetWindowText(pUserData->hwndParent, (PSTR)pUserData->szCaption,
        MAX_LEN_CAPTION);
    SendMessage(pUserData->hwndParent, PWM_GETBUTTONTEXT, 0,
        (LPARAM)pUserData->szRSoftKey);
    SendMessage(pUserData->hwndParent, PWM_GETBUTTONTEXT, 1,
        (LPARAM)pUserData->szLSoftKey);
    SendMessage(pUserData->hwndParent, PWM_GETBUTTONTEXT, 2,
        (LPARAM)pUserData->szMenuKey);
}

/**********************************************************************
 * Function     OnShowWindow
 * Purpose      
 * Params       HWND hWnd, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static void OnShowWindow(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PIMELANGSEL pUserData = NULL;

    pUserData = (PIMELANGSEL)GetWindowLong(hWnd, GWL_USERDATA);

    if ((BOOL)wParam)
    {
        SetWindowText(pUserData->hwndParent, (PSTR)RES_STR_LANG);
        SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 0,
            (LPARAM)RES_STR_CANCEL);
        SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 1,
            (LPARAM)"");
        SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 2,
            (LPARAM)"Select");
        SendMessage(pUserData->hwndParent, PWM_SETAPPICON,
            MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
        SetFocus(pUserData->hwndList);
    }
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
    PIMELANGSEL pUserData = NULL;

    pUserData = (PIMELANGSEL)GetWindowLong(hWnd, GWL_USERDATA);

    if (pUserData->nTimerID != 0)
        return;

    switch (wParam)
    {
    case VK_F5:
        OnKeyDownF5(hWnd);
        break;

    case VK_F10:
        SendMessage(hWnd, WM_CLOSE, 0, 0);
        break;

    default:
        break;
    }
}

/**********************************************************************
 * Function     OnCommand
 * Purpose      
 * Params       HWND hWnd, int nID, int nNotify, HWND hwndControl
 * Return       
 * Remarks      
 **********************************************************************/

static void OnCommand(HWND hWnd, int nID, int nNotify, HWND hwndCtrl)
{
    switch (nNotify)
    {
    case LBN_KILLFOCUS:
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        break;

    default:
        break;
    }
}

/**********************************************************************
 * Function     OnKeyDownF5
 * Purpose      
 * Params       HWND hWnd
 * Return       
 * Remarks      
 **********************************************************************/

static void OnKeyDownF5(HWND hWnd)
{
    PIMELANGSEL pUserData = NULL;
    int nIndex = 0;

    pUserData = (PIMELANGSEL)GetWindowLong(hWnd, GWL_USERDATA);

    nIndex = SendMessage(pUserData->hwndList, LB_GETCURSEL, 0, 0);
    if (nIndex != pUserData->iWriLang)
    {
        SetWriLanguage(nIndex);
        SendMessage(pUserData->hwndList, LB_SETIMAGE,
            MAKEWPARAM(IMAGE_BITMAP, nIndex), (LPARAM)pUserData->hbmpRadio[1]);
        SendMessage(pUserData->hwndList, LB_SETIMAGE,
            MAKEWPARAM(IMAGE_BITMAP, pUserData->iWriLang),
            (LPARAM)pUserData->hbmpRadio[0]);
    }
    pUserData->nTimerID = SetTimer(hWnd, IDT_SELECT, ET_SELECT, NULL);
}

/**********************************************************************
 * Function     OnDestroy
 * Purpose      
 * Params       hWnd
 * Return       
 * Remarks      
 **********************************************************************/

static void OnDestroy(HWND hWnd)
{
    PIMELANGSEL pUserData = NULL;

    pUserData = (PIMELANGSEL)GetWindowLong(hWnd, GWL_USERDATA);

    DeleteObject((HGDIOBJ)pUserData->hbmpRadio[0]);
    DeleteObject((HGDIOBJ)pUserData->hbmpRadio[1]);

    SetWindowText(pUserData->hwndParent, pUserData->szCaption);
    SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 0,
        (LPARAM)pUserData->szRSoftKey);
    SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 1,
        (LPARAM)pUserData->szLSoftKey);
    SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 2,
        (LPARAM)pUserData->szMenuKey);
}

/**********************************************************************
 * Function     OnTimer
 * Purpose      
 * Params       HWND hWnd, WPARAM wParam
 * Return       
 * Remarks      
 **********************************************************************/

static void OnTimer(HWND hWnd, WPARAM wParam)
{
    PIMELANGSEL pUserData = NULL;

    pUserData = (PIMELANGSEL)GetWindowLong(hWnd, GWL_USERDATA);

    KillTimer(hWnd, wParam);
    pUserData->nTimerID = 0;

    if (wParam == IDT_SELECT)
    {
        SendMessage(hWnd, WM_CLOSE, 0, 0);
    }
}

/**********************************************************************
 * Function     FillListBox
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/

static void FillListBox(PIMELANGSEL pUserData)
{
    HIMC hImc = NULL;
    int i = 0, nTotalWriLang = 0;
    TCHAR** ppszWriLang = NULL;

    ImmGetDefaultImc(&hImc);

    if (!GetLanguageInfo(NULL, &nTotalWriLang))
        return;

    if (!GetLanguageInfo(&ppszWriLang, NULL))
        return;

    pUserData->iWriLang = GetWriLanguage();

    for (i = 0; i < nTotalWriLang; i++)
    {
        SendMessage(pUserData->hwndList, LB_ADDSTRING, i,
            (LPARAM)ppszWriLang[i]);
        SendMessage(pUserData->hwndList, LB_SETIMAGE,
            MAKEWPARAM(IMAGE_BITMAP, i),
            (LPARAM)pUserData->hbmpRadio[(i == pUserData->iWriLang) ? 1 : 0]);
    }
    SendMessage(pUserData->hwndList, LB_SETCURSEL, 0, 0);
}
