/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : imenum.c
 *
 * Purpose  : Pollex number (including special number) input module.
 *            
\**************************************************************************/

#include "window.h"
#include "malloc.h"
#include "string.h"

#include "imm.h"

#define NUMIME_STATUS_SHOW    0x00000001
#define CARET_POS_START       0x00010000
#define CHAR_OUTPUT_OVERFLOW  0x00020000

#define NUM_KEYTYPE0 ((int)(sizeof(tabKeyType0) / sizeof(tabKeyType0[0])))
#define NUM_KEYTYPE1 ((int)(sizeof(tabKeyType1) / sizeof(tabKeyType1[0])))
#define NUM_KEYTYPE2 ((int)(sizeof(tabKeyType2) / sizeof(tabKeyType2[0])))

typedef struct tagNUMIME
{
    DWORD     fdwStatus;
    int       nRepeat;
    char      *pszCharTable;
    int       nType;
}
NUMIME, *PNUMIME, **PPNUMIME;

static BOOL NumImeOpaCreate(PPNUMIME ppNum, WORD wType);
static BOOL NumImeOpaShow(PNUMIME pNum, BOOL bShow);
static BOOL NumImeOpaEvent(PNUMIME pNum, WPARAM wParam, LPARAM lParam);
static BOOL NumImeOpaDestroy(PNUMIME pNum);
static DWORD NumImeOpaCommand(PNUMIME pNum, WPARAM wParam, LPARAM lParam);

static BOOL NumImeKeyEvent(PNUMIME pNum, INT32 nKeyCode, DWORD dwKeyData);
static BOOL NumImeKeyDown(PNUMIME pNum, INT32 nKeyCode, DWORD dwKeyData);
static BOOL NumImeKeyUp(PNUMIME pNum, INT32 nKeyCode, DWORD dwKeyData);
static void CallNextIme(void);
static DWORD GetKeyType(WORD nKeyCode, int nType);
static void StrRemoveChar(char* str, char ch);
static void FilterSeparator(char* str, char ch);
static void FilterPrefix(char* str, char ch);

static BOOL OnKeyEventKB0(PNUMIME pNum, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventF3(PNUMIME pNum, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventF4(PNUMIME pNum, INT32 nKeyCode, DWORD dwKeyData);

static const char *szCharTable1 = "*+PS?";
static const char *szCharTable2 = ".-";

// Numeric keyboard
static const KEYTYPE tabKeyType0[] =
{
    {IME_EVT_KB0, IME_EVT_KB0, IME_HKEY_INSTANT | IME_HKEY_LONG,
        OnKeyEventKB0}
};

// Phone number keyboard
static const KEYTYPE tabKeyType1[] =
{
    {IME_EVT_F3,  IME_EVT_F3,  IME_HKEY_MULTITAP | IME_HKEY_LONG,
        OnKeyEventF3},

    {IME_EVT_F4,  IME_EVT_F4,  IME_HKEY_INSTANT,
        OnKeyEventF4}
};

// Decimal number and integer keyboard
static const KEYTYPE tabKeyType2[] =
{
    {IME_EVT_F3,  IME_EVT_F3,  IME_HKEY_MULTITAP,
        OnKeyEventF3}
};

static int g_nNumKeyType[] = {NUM_KEYTYPE0, NUM_KEYTYPE1, NUM_KEYTYPE2};
static PKEYTYPE g_pKeyType[] = {(PKEYTYPE)tabKeyType0, (PKEYTYPE)tabKeyType1,
                                (PKEYTYPE)tabKeyType2};

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

DWORD NumberImeWinMain(PVOID hThis, INT32 nCode, WPARAM nOne, LPARAM nTwo)
{
    switch (LOWORD(nCode))
    {
    case IME_OPA_INIT:
        return (DWORD)TRUE;

    case IME_OPA_CREATE:
        return (DWORD)NumImeOpaCreate((PPNUMIME)hThis, HIWORD(nCode));

    case IME_OPA_SHOW:
        return (DWORD)NumImeOpaShow((PNUMIME)hThis, TRUE);

    case IME_OPA_HIDE:
        return (DWORD)NumImeOpaShow((PNUMIME)hThis, FALSE);

    case IME_OPA_EVENT:
        return (DWORD)NumImeOpaEvent((PNUMIME)hThis, nOne, nTwo);

    case IME_OPA_DESTROY:
        return (DWORD)NumImeOpaDestroy((PNUMIME)hThis);

    case IME_OPA_COMMAND:
        return (DWORD)NumImeOpaCommand((PNUMIME)hThis, nOne, nTwo);
    }
    
    return (DWORD)FALSE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL NumImeOpaCreate(PPNUMIME ppNum, WORD wType)
{
    PNUMIME pNum = NULL;

    if (ppNum == NULL)
    {
        return FALSE;
    }

    pNum = (PNUMIME)malloc(sizeof(NUMIME));
    if (pNum == NULL)
    {
        return FALSE;
    }
    memset(pNum, 0, sizeof(NUMIME));

    DefImeProc((PVOID)&pNum, MAKELONG(IME_OPA_CREATE, wType), 0, 0);

    pNum->fdwStatus = 0;
    pNum->nType     = (int)wType;

    *ppNum = pNum;

    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL NumImeOpaShow(PNUMIME pNum, BOOL bShow)
{
    if (pNum == NULL)
    {
        return FALSE;
    }

    if (bShow)
    {
        pNum->fdwStatus |= NUMIME_STATUS_SHOW;
        pNum->fdwStatus |= CARET_POS_START;
        if (pNum->nType == 0)
        {
            DefImeProc((PVOID)pNum, IME_OPA_SHOW, 0, 0);
        }
        else if (pNum->nType == 2)
        {
            ImcLockIme(NULL, IMC_LOCK_IME);
        }
    }
    else
    {
        pNum->fdwStatus &= ~NUMIME_STATUS_SHOW;
        pNum->fdwStatus &= ~CARET_POS_START;
        if (pNum->nType == 2)
        {
            ImcLockIme(NULL, IMC_UNLOCK_IME);
        }
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

static BOOL NumImeOpaEvent(PNUMIME pNum, WPARAM wParam, LPARAM lParam)
{
    if (pNum == NULL)
    {
        return FALSE;
    }

    return NumImeKeyEvent(pNum, (INT32)wParam, (DWORD)lParam);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL NumImeOpaDestroy(PNUMIME pNum)
{
    if (pNum == NULL)
    {
        return FALSE;
    }

    free(pNum);

    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL NumImeKeyEvent(PNUMIME pNum, INT32 nKeyCode, DWORD dwKeyData)
{
    if (!ISKEYUP(dwKeyData))
    {
        return NumImeKeyDown(pNum, nKeyCode, dwKeyData);
    }
    else
    {
        return NumImeKeyUp(pNum, nKeyCode, dwKeyData);
    }
}

/**********************************************************************
 * Function     NumImeOpaCommand
 * Purpose      
 * Params       PNUMIME pNum, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static DWORD NumImeOpaCommand(PNUMIME pNum, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case IME_FN_GETKEYTYPE:
        return GetKeyType((WORD)lParam, pNum->nType);

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

static BOOL NumImeKeyDown(PNUMIME pNum, INT32 nKeyCode, DWORD dwKeyData)
{
    int nIndex = 0;

    for (nIndex = 0; nIndex < g_nNumKeyType[pNum->nType]; nIndex++)
    {
        if ((g_pKeyType[pNum->nType][nIndex].wKeyEvtMin <= (WORD)nKeyCode)
            && ((WORD)nKeyCode <= g_pKeyType[pNum->nType][nIndex].wKeyEvtMax))
        {
            if (g_pKeyType[pNum->nType][nIndex].pfnOnKeyEvent == NULL)
                return FALSE;

            g_pKeyType[pNum->nType][nIndex].pfnOnKeyEvent(pNum, nKeyCode,
                dwKeyData);
            return TRUE;
        }
    }

    return DefImeProc((PVOID)pNum, IME_OPA_EVENT, (WPARAM)nKeyCode,
        (LPARAM)dwKeyData);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL NumImeKeyUp(PNUMIME pNum, INT32 nKeyCode, DWORD dwKeyData)
{
    BOOL bRet = FALSE;
    char chCode = '\0';

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

static DWORD GetKeyType(WORD wKeyCode, int nType)
{
    int i = 0;

    for (i = 0; i < g_nNumKeyType[nType]; i++)
    {
        if ((g_pKeyType[nType][i].wKeyEvtMin <= wKeyCode)
            && (wKeyCode <= g_pKeyType[nType][i].wKeyEvtMax))
        {
            return g_pKeyType[nType][i].wKeyType;
        }
    }

    return DefImeProc(NULL, IME_OPA_COMMAND, IME_FN_GETKEYTYPE, wKeyCode);
}

/**********************************************************************
 * Function     OnKeyEventKB1
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventKB0(PNUMIME pNum, INT32 nKeyCode, DWORD dwKeyData)
{
    DWORD dwPrevPos = 0;

    if (GETEVTSTATE(dwKeyData) == IME_HKEY_LONG)
    {
        if (pNum->fdwStatus & CHAR_OUTPUT_OVERFLOW)
            return FALSE;

        SendMessage(GetFocus(), WM_KEYDOWN, VK_BACK, 0);
        SendMessage(GetFocus(), WM_CHAR, (WPARAM)' ', 0);
    }
    else
    {
        dwPrevPos = SendMessage(GetFocus(), EM_GETSEL, NULL, NULL);
        SendMessage(GetFocus(), WM_CHAR, (WPARAM)'0', 0);
        if ((DWORD)SendMessage(GetFocus(), EM_GETSEL, NULL, NULL) == dwPrevPos)
        {
            pNum->fdwStatus |= CHAR_OUTPUT_OVERFLOW;
        }
        else
        {
            pNum->fdwStatus &= ~CHAR_OUTPUT_OVERFLOW;
        }
    }

    return TRUE;
}

/**********************************************************************
 * Function     OnKeyEventKB1
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventF3(PNUMIME pNum, INT32 nKeyCode, DWORD dwKeyData)
{
    static char  szCharBuf[7] = "";
    char*        pszLimit = NULL;
    int          nNumChar = 0, i = 0;
    BOOL         bPrefix = FALSE;
    char         chRet = '\0';

    if (pNum->nType == 1)
    {
        if (GETEVTSTATE(dwKeyData) == 0)
        {
            strcpy(szCharBuf, szCharTable1);
            FilterPrefix(szCharBuf, '+');
            SendMessage(GetFocus(), IME_MSG_GETLIMITSTR, 0, (LPARAM)&pszLimit);
            if (pszLimit != NULL)
            {
                i = 0;
                while (szCharBuf[i] != '\0')
                {
                    if (strchr(pszLimit, szCharBuf[i]) == NULL)
                    {
                        StrRemoveChar(szCharBuf, szCharBuf[i]);
                    }
                    else
                    {
                        i++;
                    }
                }
            }
            pNum->pszCharTable = (char*)szCharBuf;
        }
    }
    else
    {
        if (GETEVTSTATE(dwKeyData) == 0)
        {
            strcpy(szCharBuf, szCharTable2);
            FilterPrefix(szCharBuf, '-');
            FilterSeparator(szCharBuf, '.');
            pNum->pszCharTable = (char*)szCharBuf;
        }
    }

    if (pNum->pszCharTable[0] == '\0')
        return FALSE;

    return DefImeProc((PVOID)pNum, IME_OPA_EVENT, (WPARAM)nKeyCode,
        (LPARAM)dwKeyData);
}

/**********************************************************************
 * Function     OnKeyEventKB1
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventF4(PNUMIME pNum, INT32 nKeyCode, DWORD dwKeyData)
{
    SendMessage(GetFocus(), WM_CHAR, (WPARAM)'#', 0);
    return TRUE;
}

/**********************************************************************
 * Function     StrRemoveChar
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static void StrRemoveChar(char* str, char ch)
{
    int i = 0, n = 0;

    for (i = 0; i < (int)strlen(str); i++)
    {
        if (str[i] != ch)
        {
            str[n++] = str[i];
        }
    }

    str[n] = '\0';
}

/**********************************************************************
 * Function     FilterSeparator
 * Purpose      
 * Params       char* pDst, char ch
 * Return       
 * Remarks      
 **********************************************************************/

static void FilterSeparator(char* str, char ch)
{
    HWND  hWnd = NULL;
    int   nLength = 0;
    PSTR  pszText = NULL;

    hWnd = GetFocus();
    nLength = GetWindowTextLength(hWnd);
    pszText = (PSTR)malloc(nLength + 1);
    if (pszText == NULL)
        return;
    GetWindowText(hWnd, pszText, nLength + 1);
    if (strchr(pszText, (int)ch) != NULL)
    {
        StrRemoveChar(str, ch);
    }
    free(pszText);
}

#define IsLineChar(c) (((c) == ';') || ((c) == '\r') || ((c) == '\n'))

/**********************************************************************
 * Function     FilterPrefix
 * Purpose      
 * Params       char* pDst, char ch
 * Return       
 * Remarks      
 **********************************************************************/

static void FilterPrefix(char* str, char ch)
{
    HWND  hWnd = NULL;
    int   nLength = 0;
    PSTR  pszText = NULL;
    DWORD dwCaretPos = 0;

    hWnd = GetFocus();
    nLength = GetWindowTextLength(hWnd);
    pszText = (PSTR)malloc(nLength + 1);
    if (pszText == NULL)
        return;
    GetWindowText(hWnd, pszText, nLength + 1);

    dwCaretPos = LOWORD(SendMessage(hWnd, EM_GETSEL, 0, 0));

    if (((dwCaretPos == 0) || IsLineChar(pszText[dwCaretPos - 1]))
        && (pszText[dwCaretPos] != ch))
    {
        free(pszText);
        return;
    }

    StrRemoveChar(str, ch);
    free(pszText);
}
