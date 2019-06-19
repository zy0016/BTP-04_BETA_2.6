/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : imeedit.c
 *
 * Purpose  : IMEEDIT Class Implementation
 *            
\**************************************************************************/

#include "window.h"
#include "winpda.h"
#include "plx_pdaex.h"
#include "malloc.h"
#include "string.h"
#include "stdio.h"
#include "str_public.h"
#include "str_plx.h"

#include "imesys.h"
#include "imm.h"

#define WND_CAPTIONSPACE 20
#define WND_TRUNCATEDTITLE 12
#define STR_IMEICON_PATH  "ROM:ime/"

#define CX_IMEICON       20
#define CY_IMEICON       28

#define IME_ATTRIB_DIGIT 0x00010000L

typedef struct tagEDITWNDLONG
{
    HWND   hwndNotify;    // 当创建IMEEDITEX控件时的父窗口句柄
    DWORD  dwAttrib;      // IMEEDIT控件属性
    DWORD  dwAscTextMax;  // 当IME_ATTRIB_GSM标识位置位时，该参数指定最大纯英文字符数
    DWORD  dwUniTextMax;  // 当IME_ATTRIB_GSM标识位置位时，该参数指定最大含中文字符数
    DWORD  dwSBCCount;    // Single Byte Character，英文字符数目(包括GSM符号)
    DWORD  dwDBCCount;    // Double Byte Character，中文字符数目
    WORD   wPageMax;
    WORD   wPageCount;
    WORD   wImeId;
    BOOL   bImeShow;
    PSTR   pszTitle;      // IMEEDIT控件标题名称
    PSTR   pszRBnCaption;
    PSTR   pszCharSet;    // 允许输入的字符集合
    char   chPrefix;
    UINT   uMsgSetText;
    int    nWordMode;
    HDC     hdcMem;
    HBITMAP hbmpImeIcon;
    char   szLeftKey[32];
}
EDITWNDLONG, *PEDITWNDLONG;

typedef struct tagEDITINFO
{
    WNDPROC  EditWndProc;
    int      cbWndExtra;
}
EDITINFO, *PEDITINFO;

typedef struct tagTEXTINFO
{
    int    nLength;
    DWORD  dwCaretPos;
    PSTR   pszText;
}
TEXTINFO, *PTEXTINFO;

static EDITINFO g_ei;

static LRESULT CALLBACK ImeEditWndProc(HWND hWnd, UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam);
static BOOL RegisterImeEditClass(HINSTANCE hInst);
extern BOOL RegisterImeEditExClass(HINSTANCE hInst);
extern BOOL RegisterImeLangSelClass(HINSTANCE hInst);
extern BOOL RegisterImeZiSpellClass(HINSTANCE hInst);
extern BOOL CALLEDIT_RegisterClass(void);
extern int GetPreInputSetting(void);
extern int SetPreInputSetting(int);
extern TCHAR* GetWriLangStr(void);
static LRESULT CallWindowProcEx(HWND hWnd, UINT uMsg,
                                WPARAM wParam, LPARAM lParam,
                                BOOL bRecall);

static void OnCreate(HWND hWnd, LPARAM lParam);
static void OnSetFocus(HWND hWnd);
static void OnKillFocus(HWND hWnd);
static LRESULT OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
static BOOL OnChar(HWND hWnd, WPARAM wParam);
static BOOL OnSetText(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnDestroy(HWND hWnd);
static void OnEditChange(HWND hWnd, PTEXTINFO pTI);
static BOOL OnEditUpdate(HWND hWnd, PTEXTINFO ptiPrev);
static void OnImeChangedIme(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnImeChangedSel(HWND hWnd, char *pszText);
static void OnImeChangingSel(HWND hWnd);
static void OnImeGetLimitStr(HWND hWnd, char** ppszCharSet);
static BOOL OnImeSetParam(HWND hWnd, WPARAM wParam, LPARAM lParam);
static BOOL OnImeSplitPage(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnImeShowEditEx(HWND hWnd);
static char *StrAssign(const char *strSource);
static int  StrMinus(const char *str, const char *strCharSet);
static int  GetWindowTextCharNum(HWND hWnd, int nMaxCount,
                                 PDWORD pdwSBCCount,
                                 PDWORD pdwDBCCount);
static int  GetWindowTextPage(HWND hWnd);
static int  SplitTextIntoEqualCharPage(HWND hWnd, int nMaxCount,
                                       DWORD dwPageChar,
                                       PINT pwOffset);
static void CombineImeIcon(HDC hdcMem, HBITMAP hbmpImeIcon, PSTR pszIcon,
                           PSTR pszLang);

/*********************************************************************\
* Function     ImeCtrlInit
* Purpose      
* Params       hInst
* Return           
* Remarks      
**********************************************************************/

BOOL ImeCtrlInit(HINSTANCE hInst)
{
    if (!RegisterImeEditClass(hInst))
        goto err_init;

    if (!RegisterImeEditExClass(hInst))
        goto err_init;

    if (!RegisterImeLangSelClass(hInst))
        goto err_init;

    if (!RegisterImeZiSpellClass(hInst))
        goto err_init;

    if (!CALLEDIT_RegisterClass())
		goto err_init;

    return TRUE;

err_init:
    UnregisterClass("IMEEDIT", NULL);
    UnregisterClass("IMEEDITEX", NULL);
    UnregisterClass("IMELANGSEL", NULL);
    UnregisterClass("IMEZISPELL", NULL);
    UnregisterClass("CALLEDIT", NULL);

    return FALSE;
}

/*********************************************************************\
* Function     RegisterImeEditClass
* Purpose      
* Params       hInst
* Return           
* Remarks      
**********************************************************************/

static BOOL RegisterImeEditClass(HINSTANCE hInst)
{
    WNDCLASS wcImeEdit;
    
    if (g_ei.EditWndProc != NULL)
        return TRUE;

    if (!GetClassInfo(hInst, "EDIT", &wcImeEdit))
    {
        return FALSE;
    }
    
    g_ei.EditWndProc = wcImeEdit.lpfnWndProc;
    g_ei.cbWndExtra  = (wcImeEdit.cbWndExtra + (4 - 1)) / 4 * 4;

    wcImeEdit.lpfnWndProc    = ImeEditWndProc;
    wcImeEdit.cbWndExtra    += sizeof(PEDITWNDLONG);
    wcImeEdit.lpszClassName  = "IMEEDIT";
    
    return RegisterClass(&wcImeEdit);
}

/*********************************************************************\
* Function     ImeEditWndProc
* Purpose      
* Params       hWnd uMsg wParam lParam
* Return           
* Remarks      
**********************************************************************/

static LRESULT CALLBACK ImeEditWndProc(HWND hWnd, UINT uMsg,
                                       WPARAM wParam, LPARAM lParam)
{
    BOOL bRet = TRUE;

    switch (uMsg)
    {
    case WM_CREATE:
        OnCreate(hWnd, lParam);
        break;

    case WM_SETFOCUS:
        OnSetFocus(hWnd);
        break;

    case WM_KILLFOCUS:
        OnKillFocus(hWnd);
        break;

    case WM_KEYDOWN:
        return OnKeyDown(hWnd, wParam, lParam);
        
    case WM_KEYUP:
        OnKeyUp(hWnd, wParam, lParam);
        if ((int)wParam == VK_CLEAR)
        {
            return CallWindowProcEx(hWnd, uMsg, wParam, lParam,
                FALSE);
        }
        break;

    case WM_DESTROY:
        OnDestroy(hWnd);
        break;

    case WM_CHAR:
        bRet = OnChar(hWnd, wParam);
        if (!bRet)
        {
            return 0;
        }
        return CallWindowProcEx(hWnd, uMsg, wParam, lParam, FALSE);

    case WM_SETTEXT:
        bRet = OnSetText(hWnd, wParam, lParam);
        if (!bRet)
        {
            return 0;
        }
        return CallWindowProcEx(hWnd, uMsg, wParam, lParam, TRUE);
    case WM_CLEAR:
    case WM_CUT:
    case WM_PASTE:
    case EM_REPLACESEL:
        return CallWindowProcEx(hWnd, uMsg, wParam, lParam, FALSE);

    case IME_MSG_CHANGED_IME:
        OnImeChangedIme(hWnd, wParam, lParam);
        return 0;

    case IME_MSG_CHANGED_SEL:
        OnImeChangedSel(hWnd, (char*)lParam);
        return 0;

    case IME_MSG_CHANGING_SEL:
        OnImeChangingSel(hWnd);
        return 0;

    case IME_MSG_GETLIMITSTR:
        OnImeGetLimitStr(hWnd, (char**)lParam);
        return 0;

    case IME_MSG_SHOWEDITEX:
        OnImeShowEditEx(hWnd);
        return 0;

    case IME_MSG_SETPARAM:
        bRet = OnImeSetParam(hWnd, wParam, lParam);
        return (LRESULT)bRet;

    case IME_MSG_SPLITPAGE:
        bRet = OnImeSplitPage(hWnd, wParam, lParam);
        return (LRESULT)bRet;

    default:
        break;
    }

    return CallWindowProc(g_ei.EditWndProc, hWnd, uMsg, wParam,
        lParam);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static LRESULT CallWindowProcEx(HWND hWnd, UINT uMsg,
                                WPARAM wParam, LPARAM lParam,
                                BOOL bRecall)
{
    TEXTINFO ti;
    LRESULT  lRes = 0;
    BOOL bRet = TRUE;

    OnEditChange(hWnd, &ti);
    lRes = CallWindowProc(g_ei.EditWndProc, hWnd, uMsg, wParam,
        lParam);
    bRet = OnEditUpdate(hWnd, &ti);

    if (bRecall && !bRet)
    {
        return (LRESULT)FALSE;
    }

    return lRes;
}

/*********************************************************************\
* Function     OnCreate
* Purpose      
* Params       HWND hWnd, WPARAM wParam, LPARAM lParam
* Return           
* Remarks      
**********************************************************************/

static void OnCreate(HWND hWnd, LPARAM lParam)
{
    PEDITWNDLONG pEditLong = NULL;
    PIMEEDIT pUserData = NULL;
    int nLength = 0;
    LPCREATESTRUCT lpcs = NULL;
    TCHAR *pszLang = NULL;

    lpcs = (LPCREATESTRUCT)lParam;
    pUserData = (PIMEEDIT)lpcs->lpCreateParams;
    if (pUserData == NULL || pUserData->hwndNotify == NULL)
    {
        return;
    }

    pEditLong = (PEDITWNDLONG)malloc(sizeof(EDITWNDLONG));

    memset(pEditLong, 0, sizeof(EDITWNDLONG));
    pEditLong->hwndNotify = pUserData->hwndNotify;
    pEditLong->dwAttrib   = pUserData->dwAttrib;

    if (pUserData->dwAttrib & IME_ATTRIB_GSM)
    {
        if (pUserData->dwAscTextMax <= 0
            || pUserData->dwUniTextMax <= 0)
        {
            free(pEditLong);
            pEditLong = NULL;
            return;
        }
        
        pEditLong->dwAscTextMax = pUserData->dwAscTextMax;
        pEditLong->dwUniTextMax = pUserData->dwUniTextMax;

        if ((pUserData->dwAttrib & IME_ATTRIB_GSMLONG)
            == IME_ATTRIB_GSMLONG)
        {
            pEditLong->wPageMax = (WORD)((pUserData->wPageMax > 0)
                ? pUserData->wPageMax : 1);
        }
        else
        {
            pEditLong->wPageMax = 1;
        }
    }

    if (pUserData->pszImeName != NULL)
    {
        if (!ImmGetImeIDByName(&pEditLong->wImeId,
            pUserData->pszImeName))
        {
            pEditLong->wImeId = 0;
        }

        if (((strcmp(pUserData->pszImeName, "Phone") == 0)
            || (strcmp(pUserData->pszImeName, "Digit") == 0))
            && !(lpcs->style & ES_NUMBER))
        {
            if (strcmp(pUserData->pszImeName, "Phone") == 0)
            {
                pEditLong->chPrefix = '+';
            }
            else
            {
                pEditLong->chPrefix = '\0';
            }
            
            if ((pUserData->pszCharSet != NULL)
                && (strcmp(pUserData->pszCharSet, "") != 0))
            {
                pEditLong->pszCharSet = StrAssign(pUserData->pszCharSet);
            }
            pEditLong->dwAttrib |= IME_ATTRIB_DIGIT;
        }
        else
        {
            pEditLong->chPrefix = '\0';
            if (pUserData->pszCharSet != NULL)
            {
                pEditLong->pszCharSet = StrAssign(pUserData->pszCharSet);
            }
            else
            {
                pEditLong->pszCharSet = NULL;
            }
        }
    }
    else
    {
        pszLang = GetWriLangStr();
        if ((pszLang != NULL) && (stricmp(pszLang, "Chinese") == 0))
        {
            ImmGetImeIDByName(&pEditLong->wImeId, "Pinyin");
        }
        else
        {
            pEditLong->wImeId = 0;
        }
        if (pUserData->pszCharSet != NULL)
        {
            pEditLong->pszCharSet = StrAssign(pUserData->pszCharSet);
        }
        else
        {
            pEditLong->pszCharSet = NULL;
        }
    }

    pEditLong->bImeShow = FALSE;

    if (pUserData->dwAttrib & IME_ATTRIB_GENERAL)
    {
        if (pUserData->pszTitle != NULL
            && (strcmp(pUserData->pszTitle, "") != 0))
        {
            pEditLong->pszTitle = StrAssign(pUserData->pszTitle);
        }
        else
        {
            pEditLong->pszTitle = StrAssign("无标题");
        }
    }
    else
    {
        nLength = GetWindowTextLength(pEditLong->hwndNotify);
        pEditLong->pszTitle = (PSTR)malloc(nLength + 1);
        GetWindowText(pEditLong->hwndNotify, pEditLong->pszTitle,
            nLength + 1);

        pEditLong->pszRBnCaption = (PSTR)malloc(32);

        pEditLong->hdcMem = CreateCompatibleDC(NULL);
        pEditLong->hbmpImeIcon = CreateCompatibleBitmap(pEditLong->hdcMem,
            CX_IMEICON, CY_IMEICON);
        SelectObject(pEditLong->hdcMem, pEditLong->hbmpImeIcon);
    }

    if ((pUserData->dwAttrib & IME_ATTRIB_RECEIVER)
        == IME_ATTRIB_RECEIVER)
    {
        pEditLong->uMsgSetText = pUserData->uMsgSetText;
    }

    if (lpcs->style & ES_PASSWORD)
    {
        pEditLong->dwAttrib |= IME_ATTRIB_BASIC;
    }
    if (!(pEditLong->dwAttrib & IME_ATTRIB_BASIC)
        && GetPreInputSetting() == 0)
    {
        pEditLong->nWordMode = 1;
    }

    SetWindowLong(hWnd, g_ei.cbWndExtra, (LONG)pEditLong);
}

/*********************************************************************\
* Function     OnSetFocus
* Purpose      
* Params       hWnd
* Return           
* Remarks      
**********************************************************************/

static void OnSetFocus(HWND hWnd)
{
    PEDITWNDLONG pEditLong = NULL;
    char szCaption[WND_CAPTIONSPACE + 1] = "",
        szTitle[WND_TRUNCATEDTITLE + 1] = "",
        szFormat[20] = "", szIcon[32] = "";
    HIME hIme = NULL;
    int nLength = 0, nCharTotal = 0, nCharLeft = 0, nPageLeft = 0;
    char *pszLangFile = NULL;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);

    if (pEditLong == NULL)
    {
        return;
    }

    if (pEditLong->dwAttrib & IME_ATTRIB_GENERAL)
    {
        return;
    }
    
    if (!pEditLong->bImeShow)
    {
        SendMessage(pEditLong->hwndNotify, PWM_GETBUTTONTEXT,
            0, (LPARAM)pEditLong->pszRBnCaption);

        if (ImcShowImeByID(NULL, pEditLong->wImeId, TRUE))
        {
            if (ImmGetImeHandleByID(pEditLong->wImeId, &hIme))
            {
                if (pEditLong->dwAttrib & IME_ATTRIB_BASIC)
                {
                    ImcEnableWordInput(NULL, FALSE);
                }
                else
                {
                    if (pEditLong->nWordMode != -1)
                    {
                        ImcToggleTextInput(NULL, pEditLong->nWordMode);
                        pEditLong->nWordMode = -1;
                    }
                    ImcSetImeLanguage(NULL);
                }
                if (ImcGetImeLangState(NULL, &pszLangFile))
                {
                    CombineImeIcon(pEditLong->hdcMem, pEditLong->hbmpImeIcon,
                        hIme->v_aImeIcon, pszLangFile);
                    SendMessage(pEditLong->hwndNotify, WM_IMESWITCHED,
                        IMAGE_BITMAP, (LPARAM)pEditLong->hbmpImeIcon);
                }
                else
                {
                    sprintf(szIcon, "%s%s", STR_IMEICON_PATH, hIme->v_aImeIcon);
                    SendMessage(pEditLong->hwndNotify, WM_IMESWITCHED, IMAGE_ICON,
                        (LPARAM)szIcon);
                }
//                GetWindowText(pEditLong->hwndNotify, szTitle,
//                    WND_TRUNCATEDTITLE + 1);
//                sprintf(szCaption,
//#ifdef IME_ICON_USE
//                    "%s",
//#else
//                    "%s%s", hIme->v_aImeIcon,
//#endif
//                    szTitle);
//                SetWindowText(pEditLong->hwndNotify,
//                    (PCSTR)szCaption);
            }

            if (pEditLong->dwAttrib & IME_ATTRIB_GSM)
            {
                nLength = GetWindowTextLength(hWnd);
                nCharTotal = GetWindowTextCharNum(hWnd, nLength + 1,
                    &pEditLong->dwSBCCount, &pEditLong->dwDBCCount);
                GetWindowTextPage(hWnd);
                if (pEditLong->dwDBCCount == 0)
                {
                    nCharLeft = pEditLong->dwAscTextMax
                        * pEditLong->wPageCount - nCharTotal;
                }
                else
                {
                    nCharLeft = pEditLong->dwUniTextMax
                        * pEditLong->wPageCount - nCharTotal;
                }
                
                nPageLeft = pEditLong->wPageMax
                    - pEditLong->wPageCount;

                GetWindowText(pEditLong->hwndNotify, szTitle,
                    WND_TRUNCATEDTITLE + 1);

                if ((pEditLong->dwAttrib & IME_ATTRIB_GSMLONG)
                    == IME_ATTRIB_GSMLONG)
                {
                    sprintf(szFormat, "%%-%ds  %%3d/%%d", WND_TRUNCATEDTITLE);
                    // "%-12s  %3d/%d"
                    sprintf(szCaption, (const char *)szFormat, szTitle,
                        nCharLeft, nPageLeft);
                }
                else
                {
                    sprintf(szFormat, "%%-%ds    %%3d", WND_TRUNCATEDTITLE);
                    // "%-12s    %3d"
                    sprintf(szCaption, (const char *)szFormat, szTitle,
                        nCharLeft);
                }

                SetWindowText(pEditLong->hwndNotify,
                    (PCSTR)szCaption);
            }
//            else
//            {
//                GetWindowText(pEditLong->hwndNotify, szCaption,
//                    WND_CAPTIONSPACE - 5);
//                SetWindowText(pEditLong->hwndNotify,
//                    (PCSTR)szCaption);
//            }

            pEditLong->bImeShow = TRUE;
        }
    }
}

/*********************************************************************\
* Function     OnKillFocus
* Purpose      
* Params       hWnd
* Return           
* Remarks      
**********************************************************************/

static void OnKillFocus(HWND hWnd)
{
    PEDITWNDLONG pEditLong = NULL;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);

    if (pEditLong == NULL)
    {
        return;
    }
    
    if (pEditLong->dwAttrib & IME_ATTRIB_GENERAL)
    {
        return;
    }

    if (pEditLong->bImeShow)
    {
        if (pEditLong->dwAttrib & IME_ATTRIB_BASIC)
        {
            ImcEnableWordInput(NULL, TRUE);
        }
        ImcShowImeByID(NULL, pEditLong->wImeId, FALSE);
        SendMessage(pEditLong->hwndNotify, WM_IMESWITCHED, IMAGE_ICON,
            (LPARAM)"");

        SetWindowText(pEditLong->hwndNotify, pEditLong->pszTitle);
        
        SendMessage(pEditLong->hwndNotify, PWM_SETBUTTONTEXT,
            0, (LPARAM)pEditLong->pszRBnCaption);
        
        pEditLong->bImeShow = FALSE;
    }
}

/**********************************************************************
 * Function     OnKeyDown
 * Purpose      
 * Params       HWND hWnd, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    static int nCount = 0;
    DWORD dwCaretPos = 0;

    switch (wParam)
    {
    case VK_F10:
        dwCaretPos = SendMessage(hWnd, EM_GETSEL, 0, 0);
        if ((dwCaretPos == 0) && (HIWORD(lParam) != 0))
        {
            // If the caret is at the beginning of the buffer and this key
            // message is triggered by actual key-pressing action (NOT by
            // REPEATING timer), this message will return to SOFT_RIGHT
            // original process.
            return CallWindowProc(g_ei.EditWndProc, hWnd, WM_KEYDOWN, wParam,
                lParam);
        }
        SendMessage(hWnd, WM_KEYDOWN, VK_BACK, 0);
        return 0;

    default:
        return CallWindowProc(g_ei.EditWndProc, hWnd, WM_KEYDOWN, wParam,
            lParam);
    }
}

/*********************************************************************\
* Function     OnKeyUp
* Purpose      
* Params       HWND hWnd, WPARAM wParam, LPARAM lParam
* Return           
* Remarks      
**********************************************************************/

static void OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PEDITWNDLONG pEditLong = NULL;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);

    if (pEditLong == NULL)
    {
        return;
    }

    switch ((int)wParam)
    {
    case VK_F4:
        if (pEditLong->dwAttrib & IME_ATTRIB_GENERAL)
        {
            PostMessage(hWnd, IME_MSG_SHOWEDITEX, 0, 0);
        }
        else
        {
            PostMessage(pEditLong->hwndNotify, WM_KEYDOWN, wParam,
                lParam);
        }
        break;

    default:
        break;
    }
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL OnChar(HWND hWnd, WPARAM wParam)
{
    PEDITWNDLONG pEditLong = NULL;
    int nLength = 0;
    DWORD dwCaretPos = 0;
    PSTR pszText = NULL;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);
    
    if (pEditLong == NULL)
    {
        return FALSE;
    }

    nLength = GetWindowTextLength(hWnd);
    pszText = (PSTR)malloc(nLength + 1);
    if (pszText == NULL)
        return FALSE;
    GetWindowText(hWnd, pszText, nLength + 1);
    dwCaretPos = LOWORD(SendMessage(hWnd, EM_GETSEL, 0, 0));

    // The prefix character does not count towards the total length.
    if ((pEditLong->chPrefix != '\0')
        && (strchr((const char*)pszText, pEditLong->chPrefix) == NULL)
        && (nLength >= SendMessage(hWnd, EM_GETLIMITTEXT, 0, 0) - 1)
        && !(((char)wParam == pEditLong->chPrefix)
        && (dwCaretPos == 0)))
    {
        free(pszText);
        return FALSE;
    }

    // Any character can't be inputed before the prefix character.
    if ((pEditLong->chPrefix != '\0')
        && (pszText[dwCaretPos] == pEditLong->chPrefix))
//        && (strchr((const char*)pszText, pEditLong->chPrefix) != NULL)
//        && (dwCaretPos == 0))
    {
        free(pszText);
        return FALSE;
    }
    free(pszText);

    if (pEditLong->pszCharSet == NULL)
    {
        if ((wParam == 0x0A0D)
            && !(GetWindowLong(hWnd, GWL_STYLE) & ES_MULTILINE))
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }

    if (!(pEditLong->dwAttrib & IME_ATTRIB_DIGIT))
        return TRUE;

    if (strchr(pEditLong->pszCharSet, (int)wParam) != NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*********************************************************************\
* Function     OnSetText
* Purpose      
* Params       HWND hWnd, LPARAM lParam
* Return           
* Remarks      
**********************************************************************/

static BOOL OnSetText(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PEDITWNDLONG pEditLong = NULL;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);
    
    if (pEditLong == NULL)
    {
        return FALSE;
    }

    if ((pEditLong->pszCharSet != NULL) && (StrMinus(
        (const char *)lParam, pEditLong->pszCharSet) > 0))
    {
        return FALSE;
    }

    if ((pEditLong->dwAttrib & IME_ATTRIB_RECEIVER)
        == IME_ATTRIB_RECEIVER)
    {
		SendMessage(pEditLong->hwndNotify,
			pEditLong->uMsgSetText, wParam, lParam);
    }

    return TRUE;
}

/*********************************************************************\
* Function     OnDestroy
* Purpose      
* Params       hWnd
* Return           
* Remarks      
**********************************************************************/

static void OnDestroy(HWND hWnd)
{
    PEDITWNDLONG pEditLong = NULL;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);

    if (pEditLong != NULL)
    {
        if (pEditLong->bImeShow)
        {
            ImcShowImeByID(NULL, pEditLong->wImeId, FALSE);
            pEditLong->bImeShow = FALSE;
        }

        if (pEditLong->pszTitle != NULL)
        {
            free(pEditLong->pszTitle);
        }

        if (!(pEditLong->dwAttrib & IME_ATTRIB_GENERAL)
            && (pEditLong->pszRBnCaption != NULL))
        {
            free(pEditLong->pszRBnCaption);
        }

        if (pEditLong->pszCharSet != NULL)
        {
            free(pEditLong->pszCharSet);
        }

        if (pEditLong->hbmpImeIcon != NULL)
        {
            DeleteObject(pEditLong->hbmpImeIcon);
        }

        if (pEditLong->hdcMem != NULL)
        {
            DeleteDC(pEditLong->hdcMem);
        }

        free(pEditLong);
        pEditLong = NULL;
    }
}

/*********************************************************************\
* Function     OnEditChange
* Purpose      
* Params       hWnd pti
* Return           
* Remarks      
**********************************************************************/

static void OnEditChange(HWND hWnd, PTEXTINFO pti)
{
    PEDITWNDLONG pEditLong = NULL;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);

    if (pEditLong == NULL)
    {
        return;
    }

    if (!(pEditLong->dwAttrib & IME_ATTRIB_GSM))
    {
        return;
    }
    pti->nLength = CallWindowProc(g_ei.EditWndProc, hWnd,
        WM_GETTEXTLENGTH, 0, 0);

    if (pti->nLength == 0)
    {
        pti->pszText    = NULL;
        pti->dwCaretPos = 0;
    }
    else
    {
        pti->pszText = (PSTR)malloc(pti->nLength + 1);
        if (pti->pszText == NULL)
        {
            return;
        }
        
        CallWindowProc(g_ei.EditWndProc, hWnd, WM_GETTEXT,
            (WPARAM)(pti->nLength + 1), (LPARAM)(pti->pszText));
        
        pti->dwCaretPos = CallWindowProc(g_ei.EditWndProc, hWnd,
            EM_GETSEL, NULL, NULL);
    }
}

/*********************************************************************\
* Function     OnEditUpdate
* Purpose      
* Params       hWnd ptiPrev
* Return           
* Remarks      
**********************************************************************/

static BOOL OnEditUpdate(HWND hWnd, PTEXTINFO ptiPrev)
{
    PEDITWNDLONG pEditLong = NULL;
    char szCaption[WND_CAPTIONSPACE + 1] = "",
        szTitle[WND_TRUNCATEDTITLE + 1] = "",
        szFormat[20] = "";
    int nLength = 0, nCharTotal = 0, nCharLeft = 0, nPageLeft = 0;
    PSTR pszText = NULL;
    BOOL bRet = TRUE;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);

    if (pEditLong == NULL)
    {
        return FALSE;
    }

    if (!(pEditLong->dwAttrib & IME_ATTRIB_GSM))
    {
        return TRUE;
    }

    nLength = CallWindowProc(g_ei.EditWndProc, hWnd,
        WM_GETTEXTLENGTH, 0, 0);

    if (nLength == 0)
    {
        if (ptiPrev->nLength == 0)
        {
            return TRUE;
        }
    }
    else
    {
        pszText = (PSTR)malloc(nLength + 1);
        if (pszText == NULL)
        {
            return FALSE;
        }
        
        CallWindowProc(g_ei.EditWndProc, hWnd, WM_GETTEXT,
            (WPARAM)(nLength + 1), (LPARAM)pszText);
    }
    
    if (nLength != ptiPrev->nLength
        || strcmp((const char *)pszText,
        (const char *)ptiPrev->pszText) != 0)
    {
        nCharTotal = GetWindowTextCharNum(hWnd, nLength + 1,
            &pEditLong->dwSBCCount, &pEditLong->dwDBCCount);
        GetWindowTextPage(hWnd);

        if ((pEditLong->dwDBCCount == 0 && pEditLong->dwSBCCount
            > pEditLong->dwAscTextMax * pEditLong->wPageMax)
            || (pEditLong->dwDBCCount > 0
            && (pEditLong->dwSBCCount + pEditLong->dwDBCCount) >
            pEditLong->dwUniTextMax * pEditLong->wPageMax))
        {
            CallWindowProc(g_ei.EditWndProc, hWnd, WM_SETTEXT, 0,
                (LPARAM)(ptiPrev->pszText));
            CallWindowProc(g_ei.EditWndProc, hWnd, EM_SETSEL,
                (WPARAM)LOWORD(ptiPrev->dwCaretPos),
                (LPARAM)HIWORD(ptiPrev->dwCaretPos));
            bRet = FALSE;
        }
        else if (!(pEditLong->dwAttrib & IME_ATTRIB_GENERAL))
        {
            if (pEditLong->dwDBCCount == 0)
            {
                nCharLeft = pEditLong->dwAscTextMax
                    * pEditLong->wPageCount - nCharTotal;
            }
            else
            {
                nCharLeft = pEditLong->dwUniTextMax
                    * pEditLong->wPageCount - nCharTotal;
            }
            
            nPageLeft = pEditLong->wPageMax
                - pEditLong->wPageCount;
            GetWindowText(pEditLong->hwndNotify, (PSTR)szTitle,
                WND_TRUNCATEDTITLE + 1);
            
            if ((pEditLong->dwAttrib & IME_ATTRIB_GSMLONG)
                == IME_ATTRIB_GSMLONG)
            {
                sprintf(szFormat, "%%-%ds  %%3d/%%d", WND_TRUNCATEDTITLE);
                // "%-12s  %3d/%d"
                sprintf(szCaption, (const char *)szFormat, szTitle,
                    nCharLeft, nPageLeft);
            }
            else
            {
                sprintf(szFormat, "%%-%ds    %%3d", WND_TRUNCATEDTITLE);
                // "%-12s  %3d"
                sprintf(szCaption, (const char *)szFormat, szTitle,
                    nCharLeft);
            }

            SetWindowText(pEditLong->hwndNotify, (PCSTR)szCaption);

            bRet = TRUE;
        }
    }

    if (pszText != NULL)
    {
        free(pszText);
    }

    if (ptiPrev->pszText != NULL)
    {
        free(ptiPrev->pszText);
    }

    return bRet;
}

/*********************************************************************\
* Function     OnImeChanged
* Purpose      
* Params       hWnd wParam
* Return           
* Remarks      
**********************************************************************/

static void OnImeChangedIme(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PEDITWNDLONG pEditLong = NULL;
    WORD wNewId = 0;
    HIME hIme = NULL;
    char szCaption[WND_CAPTIONSPACE + 1] = "",
        szTitle[WND_CAPTIONSPACE + 1] = "",
        szIcon[32] = "", szLangIcon[32] = "";
    char *pszLangFile = NULL;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);
    
    if (pEditLong == NULL)
    {
        return;
    }

    if (wParam == 0)
    {
        SetPreInputSetting(lParam);
        return;
    }

    wNewId = HIWORD(wParam);
    if (pEditLong->wImeId != wNewId)
    {
        if (ImmGetImeHandleByID(wNewId, &hIme))
        {
            if (!(hIme->v_wAttrib & IME_ATR_NOSWITCH))
            {
                pEditLong->wImeId = wNewId;
//            }
//            if (strlen(hIme->v_aImeIcon) != 0)
//            {
                if (ImcGetImeLangState(NULL, &pszLangFile))
                {
                    CombineImeIcon(pEditLong->hdcMem, pEditLong->hbmpImeIcon,
                        hIme->v_aImeIcon, pszLangFile);

                    SendMessage(pEditLong->hwndNotify, WM_IMESWITCHED,
                        IMAGE_BITMAP, (LPARAM)pEditLong->hbmpImeIcon);
                }
                else
                {
                    sprintf(szIcon, "%s%s", STR_IMEICON_PATH, hIme->v_aImeIcon);
                    SendMessage(pEditLong->hwndNotify, WM_IMESWITCHED,
                        IMAGE_ICON, (LPARAM)szIcon);
                }
//                GetWindowText(pEditLong->hwndNotify, szTitle,
//                    WND_CAPTIONSPACE + 1);
//                sprintf(szCaption,
//#ifdef IME_ICON_USE
//                    "%s", (char *)szTitle
//#else
//                    "%s%s", hIme->v_aImeIcon, (char *)(szTitle + 3)
//#endif
//                    );
//                SetWindowText(pEditLong->hwndNotify,
//                    (PCSTR)szCaption);
            }
        }
    }
}

/*********************************************************************\
* Function     OnImeChangedSel
* Purpose      
* Params       hWnd, pszText
* Return           
* Remarks      
**********************************************************************/

static void OnImeChangedSel(HWND hWnd, char *pszText)
{
    PEDITWNDLONG pEditLong = NULL;
    DWORD dwID = 0;
    BOOL bEmpty = FALSE;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);
    
    if (pEditLong == NULL)
    {
        return;
    }

    if (pszText != NULL)
    {
        SendMessage(pEditLong->hwndNotify, WM_SETLBTNTEXT, 0,
            (LPARAM)pszText);
    }
    else
    {
        dwID = GetWindowLong(hWnd, GWL_ID);
        bEmpty = (GetWindowTextLength(hWnd) == 0);
        SendMessage(pEditLong->hwndNotify, WM_SETLBTNTEXT,
            MAKEWPARAM(dwID, bEmpty), (LPARAM)pEditLong->szLeftKey);
    }
}

/**********************************************************************
 * Function     OnImeChangingSel
 * Purpose      
 * Params       hWnd
 * Return       
 * Remarks      
 **********************************************************************/

static void OnImeChangingSel(HWND hWnd)
{
    PEDITWNDLONG pEditLong = NULL;
    HWND hwndParent = NULL;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);
    
    if (pEditLong == NULL)
    {
        return;
    }

    hwndParent = pEditLong->hwndNotify;
    while (GetParent(hwndParent) != NULL)
    {
        hwndParent = GetParent(hwndParent);
    }

    SendMessage(hwndParent, PWM_GETBUTTONTEXT, 1,
        (LPARAM)pEditLong->szLeftKey);
}

/**********************************************************************
 * Function     OnImeGetLimitStr
 * Purpose      
 * Params       HWND hWnd, char** ppCharSet
 * Return       
 * Remarks      
 **********************************************************************/

static void OnImeGetLimitStr(HWND hWnd, char** ppszCharSet)
{
    PEDITWNDLONG pEditLong = NULL;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);
    
    if (pEditLong == NULL)
        return;

    *ppszCharSet = pEditLong->pszCharSet;
}

/*********************************************************************\
* Function     OnImeSetParam
* Purpose      
* Params       hWnd, wParam, lParam
* Return           
* Remarks      
**********************************************************************/

static BOOL OnImeSetParam(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    DWORD dwMask = (DWORD)wParam;
    PIMEEDIT pEditSet = (PIMEEDIT)lParam;
    PEDITWNDLONG pEditLong = NULL;
    int nLength = 0;

    if (dwMask == 0 || pEditSet == NULL)
    {
        return FALSE;
    }

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);

    if (pEditLong == NULL)
    {
        return FALSE;
    }

    // 这里加入判断：如果正在设置具有焦点的IMEEDIT，
    // 则先将当前的输入法关闭，以便及时更新相关状态，
    // 并没有真正切换焦点
    if (GetFocus() == hWnd)
    {
        OnKillFocus(hWnd);
    }

    if ((dwMask & IMEEDIT_ALL) == IMEEDIT_ALL)
    {
        if (pEditSet->hwndNotify != NULL)
        {
            pEditLong->hwndNotify = pEditSet->hwndNotify;
        }
        else
        {
            return FALSE;
        }
    }
        
    if (dwMask & IMEEDIT_ATTRIB)
    {
        pEditLong->dwAttrib = pEditSet->dwAttrib;
    }

    if (pEditLong->dwAttrib & IME_ATTRIB_GSM)
    {
        if (dwMask & IMEEDIT_GSM)
        {
            if (pEditSet->dwAscTextMax <= 0
                || pEditSet->dwUniTextMax <= 0)
            {
                return FALSE;
            }

            pEditLong->dwAscTextMax = pEditSet->dwAscTextMax;
            pEditLong->dwUniTextMax = pEditSet->dwUniTextMax;

            if ((pEditLong->dwAttrib & IME_ATTRIB_GSMLONG)
                == IME_ATTRIB_GSMLONG)
            {
                if (pEditSet->wPageMax <= 0)
                {
                    return FALSE;
                }
                else
                {
                    pEditLong->wPageMax = pEditSet->wPageMax;
                }
            }
            else
            {
                pEditLong->wPageMax = 1;
            }
        }
        else if (pEditLong->dwAscTextMax <= 0
            || pEditLong->dwUniTextMax <= 0)
        {
            return FALSE;
        }
    }
    else
    {
        if (dwMask & IMEEDIT_GSM)
        {
            return FALSE;
        }
        else
        {
            pEditLong->dwAscTextMax = 0;
            pEditLong->dwUniTextMax = 0;
            pEditLong->wPageMax     = 0;
        }
    }

    if (dwMask & IMEEDIT_IMENAME)
    {
        if (pEditLong->pszCharSet != NULL)
        {
            free(pEditLong->pszCharSet);
        }
        
        if (pEditSet->pszImeName != NULL)
        {
            if (!ImmGetImeIDByName(&pEditLong->wImeId,
                pEditSet->pszImeName))
            {
                return FALSE;
            }
            
            if (((strcmp(pEditSet->pszImeName, "Phone") == 0)
                || (strcmp(pEditSet->pszImeName, "Digit") == 0))
                && (pEditSet->pszCharSet != NULL)
                && (strcmp(pEditSet->pszCharSet, "") != 0))
            {
                pEditLong->pszCharSet = StrAssign(pEditSet->pszCharSet);
            }
            else
            {
                pEditLong->pszCharSet = NULL;
            }
        }
        else
        {
            pEditLong->wImeId = 0;
        }
    }

    if (pEditLong->dwAttrib & IME_ATTRIB_GENERAL)
    {
        if (dwMask & IMEEDIT_TITLE)
        {
            if (pEditSet->pszTitle != NULL
                && (strcmp(pEditSet->pszTitle, "") != 0))
            {
                nLength = strlen(pEditSet->pszTitle);
//              pEditLong->pszTitle
//                  = (PSTR)realloc(pEditLong->pszTitle, nLength + 1);
                free(pEditLong->pszTitle);
                pEditLong->pszTitle = (PSTR)malloc(nLength + 1);
                strcpy(pEditLong->pszTitle, pEditSet->pszTitle);
            }
            else
            {
                return FALSE;
            }
        }
        else if (pEditLong->pszTitle == NULL)
        {
            return FALSE;
        }
    }
    else if (dwMask & IMEEDIT_ATTRIB)
    {
        nLength = GetWindowTextLength(pEditLong->hwndNotify);
//      pEditLong->pszTitle = (PSTR)realloc(pEditLong->pszTitle,
//          nLength + 1);
        free(pEditLong->pszTitle);
        pEditLong->pszTitle = (PSTR)malloc(nLength + 1);
        GetWindowText(pEditLong->hwndNotify, pEditLong->pszTitle,
            nLength + 1);
    }

    if (((pEditLong->dwAttrib & IME_ATTRIB_RECEIVER)
        == IME_ATTRIB_RECEIVER)
        && ((dwMask & IMEEDIT_ALL) == IMEEDIT_ALL))
    {
        pEditLong->uMsgSetText = pEditSet->uMsgSetText;
    }

    // 这里加入判断：如果正在设置具有焦点的IMEEDIT，
    // 则及时更新相关状态，并没有真正设置焦点
    if (GetFocus() == hWnd)
    {
        OnSetFocus(hWnd);
    }
    
    return TRUE;
}

/*********************************************************************\
* Function     OnImeSplitPage
* Purpose      
* Params       hWnd, wParam, lParam
* Return           
* Remarks      
**********************************************************************/

static BOOL OnImeSplitPage(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PEDITWNDLONG pEditLong = NULL;
    int nLength = 0, nPage = 0;

    if (wParam == 0 || lParam == 0)
    {
        return FALSE;
    }

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);
    if (pEditLong == NULL)
    {
        return FALSE;
    }

    if ((pEditLong->dwAttrib & IME_ATTRIB_GSMLONG)
        != IME_ATTRIB_GSMLONG)
    {
        return FALSE;
    }

    nLength = GetWindowTextLength(hWnd);
    if (nLength == 0
        || pEditLong->dwSBCCount + pEditLong->dwDBCCount == 0)
    {
        return FALSE;
    }

    if (pEditLong->dwDBCCount == 0)
    {
        nPage = SplitTextIntoEqualCharPage(hWnd, nLength + 1,
            pEditLong->dwAscTextMax, (PINT)wParam);
    }
    else
    {
        nPage = SplitTextIntoEqualCharPage(hWnd, nLength + 1,
            pEditLong->dwUniTextMax, (PINT)wParam);
    }

    *(PINT)lParam = nPage;

    return TRUE;
}

/*********************************************************************\
* Function     OnImeShowEditEx
* Purpose      
* Params       hWnd
* Return           
* Remarks      
**********************************************************************/

static void OnImeShowEditEx(HWND hWnd)
{
    PEDITWNDLONG pEditLong = NULL;
    HWND hwndEditEx = NULL;
    HIME hIme = NULL;
    IMEEDIT imeeditex;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);

    if (pEditLong == NULL)
    {
        return;
    }

    imeeditex.hwndNotify = hWnd;
    imeeditex.dwAttrib   = pEditLong->dwAttrib;
    
    if (pEditLong->dwAttrib & IME_ATTRIB_GSM)
    {
        imeeditex.dwAscTextMax = pEditLong->dwAscTextMax;
        imeeditex.dwUniTextMax = pEditLong->dwUniTextMax;
        
        if ((pEditLong->dwAttrib & IME_ATTRIB_GSMLONG)
            == IME_ATTRIB_GSMLONG)
        {
            imeeditex.wPageMax = pEditLong->wPageMax;
        }
        else
        {
            imeeditex.wPageMax = 1;
        }
    }
    
    if (ImmGetImeHandleByID(pEditLong->wImeId, &hIme))
    {
        imeeditex.pszImeName = hIme->v_aImeName;
    }
    else
    {
        imeeditex.pszImeName = "abc";
    }
    
    imeeditex.pszTitle    = NULL;
    imeeditex.pszCharSet  = pEditLong->pszCharSet;
    imeeditex.uMsgSetText = 0;
    
    hwndEditEx = CreateWindow(
        "IMEEDITEX",
        pEditLong->pszTitle,
        WS_VISIBLE | WS_CAPTION | WS_BORDER | PWS_STATICBAR,
        PLX_WIN_POSITION,
        hWnd,
        NULL,
        NULL,
        (PVOID)&imeeditex
        );
}

/*********************************************************************\
* Function     StrAssign
* Purpose      
* Params       strSource
* Return           
* Remarks      
**********************************************************************/

static char *StrAssign(const char *strSource)
{
    char *strDestination = NULL;
    size_t nLength = 0;

    if (strSource == NULL)
    {
        return NULL;
    }

    nLength = strlen(strSource);
    strDestination = (char *)malloc(nLength + 1);
    strcpy(strDestination, strSource);

    return strDestination;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static int StrMinus(const char *str, const char *strCharSet)
{
    size_t uIndex = 0;

    if (str == NULL)
    {
        return 0;
    }

    uIndex = strspn(str, strCharSet);

    return (uIndex < strlen(str)) ? 1 : 0;
}

/*********************************************************************\
* Function     GetWindowTextCharNum
* Purpose      
* Params       HWND hWnd, int nMaxCount,
*              PDWORD pdwSBCCount,
*              PDWORD pdwDBCCount
* Return           
* Remarks      
**********************************************************************/

static int GetWindowTextCharNum(HWND hWnd, int nMaxCount,
                                PDWORD pdwSBCCount,
                                PDWORD pdwDBCCount)
{
    PSTR pszText = NULL, pchCurrent = NULL;
    
    pszText = (PSTR)malloc(nMaxCount);
    if (pszText == NULL)
    {
        return -1;
    }

    *pdwSBCCount = 0;
    *pdwDBCCount = 0;

    CallWindowProc(g_ei.EditWndProc, hWnd, WM_GETTEXT,
        (WPARAM)nMaxCount, (LPARAM)pszText);
    pchCurrent = pszText;

    while ((*pchCurrent) != '\0')
    {
        if (!InRange((BYTE)(*pchCurrent), 0xA1, 0xFE))
        {
            pchCurrent ++;
            (*pdwSBCCount) ++;
        }
        else if (ISGSMDBCode(*pchCurrent, *(pchCurrent + 1)))
        {
            pchCurrent += 2;
            (*pdwSBCCount) ++;
        }
        else
        {
            pchCurrent += 2;
            (*pdwDBCCount) ++;
        }
    }

    free(pszText);

    return ((*pdwSBCCount) + (*pdwDBCCount));
}

/*********************************************************************\
* Function     GetWindowTextPage
* Purpose      
* Params       HWND hWnd
* Return           
* Remarks      
**********************************************************************/

static int GetWindowTextPage(HWND hWnd)
{
    PEDITWNDLONG pEditLong = NULL;

    pEditLong = (PEDITWNDLONG)GetWindowLong(hWnd, g_ei.cbWndExtra);
    
    if (pEditLong == NULL)
    {
        return -1;
    }

    if ((pEditLong->dwAttrib & IME_ATTRIB_GSMLONG)
        == IME_ATTRIB_GSMLONG)
    {
        if (pEditLong->dwDBCCount == 0)
        {
            pEditLong->wPageCount = (WORD)((pEditLong->dwSBCCount
                + pEditLong->dwAscTextMax - 1) / pEditLong->dwAscTextMax);
        }
        else
        {
            pEditLong->wPageCount = (WORD)((pEditLong->dwSBCCount
                + pEditLong->dwDBCCount
                + pEditLong->dwUniTextMax - 1) / pEditLong->dwUniTextMax);
        }
    }
    else
    {
        pEditLong->wPageCount = 1;
    }

    return pEditLong->wPageCount;
}

/*********************************************************************\
* Function     SplitTextIntoEqualCharPage
* Purpose      
* Params       HWND hWnd, int nMaxCount,
*              DWORD dwPageChar,
*              PINT pnOffset
* Return           
* Remarks      
**********************************************************************/

static int SplitTextIntoEqualCharPage(HWND hWnd, int nMaxCount,
                                      DWORD dwPageChar,
                                      PINT pnOffset)
{
    PSTR pszText = NULL, pchCurrent = NULL;
    int nCharIndex = 0, nPage = 0;
    
    pszText = (PSTR)malloc(nMaxCount);
    if (pszText == NULL)
    {
        return -1;
    }
    GetWindowText(hWnd, pszText, nMaxCount);
    pchCurrent = pszText;

    for (nCharIndex = 0; (*pchCurrent) != '\0'; nCharIndex ++)
    {
        if (nCharIndex % dwPageChar == 0)
        {
            pnOffset[nPage ++] = (int)(pchCurrent - pszText);
        }

        if (!InRange((BYTE)(*pchCurrent), 0xA1, 0xFE))
        {
            pchCurrent ++;
        }
        else
        {
            pchCurrent += 2;
        }
    }

    return nPage;
}

/**********************************************************************
 * Function     CombineImeIcon
 * Purpose      
 * Params       PSTR pszIcon, PSTR pszLang
 * Return       
 * Remarks      
 **********************************************************************/

static void CombineImeIcon(HDC hdcMem, HBITMAP hbmpImeIcon,
                              PSTR pszIconFile, PSTR pszLangFile)
{
    HBITMAP hbmpLangIcon = NULL;
    BITMAP bmp;
    RECT rc;
    char szIcon[32] = "", szLangIcon[32] = "";

    SetRect(&rc, 0, 0, CX_IMEICON, CY_IMEICON);

    ClearRect(hdcMem, &rc, COLOR_TRANSBK);
    sprintf(szIcon, "%s%s", STR_IMEICON_PATH, pszIconFile);
    DrawIconFromFile(hdcMem, szIcon, 0, 0, rc.right,
        rc.bottom / 2);

    sprintf(szLangIcon, "%s%s", STR_IMEICON_PATH, pszLangFile);
    hbmpLangIcon = LoadImage(NULL, szLangIcon, IMAGE_BITMAP,
        0, 0, LR_LOADFROMFILE);
    GetObject(hbmpLangIcon, sizeof(BITMAP), (void*)&bmp);
    BitBlt(hdcMem, 0, rc.bottom / 2, bmp.bmWidth, bmp.bmHeight,
        (HDC)hbmpLangIcon, 0, 0, ROP_SRC);
    DeleteObject(hbmpLangIcon);
}
