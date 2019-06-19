/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : imeabc.c
 *
 * Purpose  : Pollex alphabet input module.
 *            
\**************************************************************************/

#include "window.h"
#include "malloc.h"
#include "string.h"

#include "imm.h"

#define ABCIME_STATUS_SHOW    0x00000001
#define CARET_POS_START       0x00010000
#define CHAR_OUTPUT_OVERFLOW  0x00020000
#define ABCIME_TYPE_CAPITAL   0x01000000
#define ABCIME_TYPE_TITLECASE 0x02000000

#define CHARPERKEY 12
#define KEYPERLANG 8

#define NUM_KEYTYPE ((int)(sizeof(tabKeyType) / sizeof(tabKeyType[0])))

typedef BYTE (*PABYTE)[KEYPERLANG][CHARPERKEY];

typedef struct tagABCIME
{
    DWORD   fdwStatus;
    int     nRepeat;
    char    *pszCharTable;
    PABYTE  paAbcTable;
}
ABCIME, *PABCIME, **PPABCIME;

static const BYTE aAbcTable[][KEYPERLANG][CHARPERKEY] =
{
    {
        {'a', 'b', 'c', 0x7F, 0x88, 0x89, 0xD4, 0x7B, 0x0F, 0x1D, 0x8A, '2'},
        {'d', 'e', 'f', 0x04, 0x05, 0xD2, 0xD3, '3'},
        {'g', 'h', 'i', 0x8B, 0x8C, '4'},
        {'j', 'k', 'l', '5'},
        {'m', 'n', 'o', 0x7D, 0x8D, 0x8E, 0xD9, 0x7C, 0x0C, '6'},
        {'p', 'q', 'r', 's', 0x1E, '7'},
        {'t', 'u', 'v', 0x06, 0x8F, 0xD6, 0x7E, '8'},
        {'w', 'x', 'y', 'z', 0x90, '9'}
    }
};

static const BYTE aABCTable[][KEYPERLANG][CHARPERKEY] =
{
    {
        {'A', 'B', 'C', 0xC9, 0x80, 0x81, 0x91, 0x5B, 0x0E, 0x1C, 0x09, '2'},
        {'D', 'E', 'F', 0xCA, 0x1F, 0xCB, 0xCC, '3'},
        {'G', 'H', 'I', 0x82, 0x83, '4'},
        {'J', 'K', 'L', '5'},
        {'M', 'N', 'O', 0x5D, 0x84, 0x85, 0xD8, 0x5C, 0x0B, '6'},
        {'P', 'Q', 'R', 'S', '7'},
        {'T', 'U', 'V', 0xCF, 0x86, 0xD0, 0x5E, '8'},
        {'W', 'X', 'Y', 'Z', 0x87, '9'}
    }
};

static BOOL AbcImeOpaCreate(PPABCIME ppAbc, WORD wType);
static BOOL AbcImeOpaShow(PABCIME pAbc, BOOL bShow);
static BOOL AbcImeOpaEvent(PABCIME pAbc, WPARAM wParam, LPARAM lParam);
static BOOL AbcImeOpaDestroy(PABCIME pAbc);
static DWORD AbcImeOpaCommand(PABCIME pAbc, WPARAM wParam, LPARAM lParam);

static BOOL AbcImeKeyEvent(PABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static BOOL AbcImeKeyDown(PABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static BOOL AbcImeKeyUp(PABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static void CallNextIme(void);
static DWORD GetKeyType(WORD nKeyCode);
static void OnMultapPause(PABCIME pAbc, INT32 nKeyCode);

static BOOL OnKeyEventKB1(PABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventKB2_9(PABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventKB0(PABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);

static const KEYTYPE tabKeyType[] =
{
    {IME_EVT_KB1,    IME_EVT_KB1,       IME_HKEY_SHORT | IME_HKEY_LONG,
        OnKeyEventKB1},

    {IME_EVT_KB2,    IME_EVT_KB9,       IME_HKEY_MULTITAP | IME_HKEY_LONG,
        OnKeyEventKB2_9},

    {IME_EVT_KB0,    IME_EVT_KB0,       IME_HKEY_INSTANT | IME_HKEY_LONG,
        OnKeyEventKB0}
};

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

DWORD AbcImeWinMain(PVOID hThis, INT32 nCode, WPARAM nOne, LPARAM nTwo)
{
    switch (LOWORD(nCode))
    {
    case IME_OPA_INIT:
        return (DWORD)TRUE;

    case IME_OPA_CREATE:
        return (DWORD)AbcImeOpaCreate((PPABCIME)hThis, HIWORD(nCode));

    case IME_OPA_SHOW:
        return (DWORD)AbcImeOpaShow((PABCIME)hThis, TRUE);

    case IME_OPA_HIDE:
        return (DWORD)AbcImeOpaShow((PABCIME)hThis, FALSE);

    case IME_OPA_EVENT:
        return (DWORD)AbcImeOpaEvent((PABCIME)hThis, nOne, nTwo);

    case IME_OPA_DESTROY:
        return (DWORD)AbcImeOpaDestroy((PABCIME)hThis);

    case IME_OPA_COMMAND:
        return (DWORD)AbcImeOpaCommand((PABCIME)hThis, nOne, nTwo);
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

static BOOL AbcImeOpaCreate(PPABCIME ppAbc, WORD wType)
{
    PABCIME pAbc = NULL;

    if (ppAbc == NULL)
    {
        return FALSE;
    }

    pAbc = (PABCIME)malloc(sizeof(ABCIME));
    if (pAbc == NULL)
    {
        return FALSE;
    }
    memset(pAbc, 0, sizeof(ABCIME));

    DefImeProc((PVOID)&pAbc, MAKELONG(IME_OPA_CREATE, wType), 0, 0);

    switch (wType)
    {
    case 0x00A0:
        pAbc->fdwStatus = 0;
        pAbc->paAbcTable = (PABYTE)aAbcTable;
        break;

    case 0x00A1:
        pAbc->fdwStatus = ABCIME_TYPE_TITLECASE;
        pAbc->paAbcTable = (PABYTE)aABCTable;
        break;

    case 0x00A2:
        pAbc->fdwStatus = ABCIME_TYPE_CAPITAL;
        pAbc->paAbcTable = (PABYTE)aABCTable;
        break;
    }

//    pAbc->fdwStatus = (wType == 0x00A0) ? 0 : ABCIME_TYPE_CAPITAL;
//    if (wType == 0x00A1)
//        pAbc->fdwStatus |= ABCIME_TYPE_TITLECASE;

    *ppAbc = pAbc;

    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL AbcImeOpaShow(PABCIME pAbc, BOOL bShow)
{
    if (pAbc == NULL)
    {
        return FALSE;
    }

    if (bShow)
    {
        pAbc->fdwStatus |= ABCIME_STATUS_SHOW;
        pAbc->fdwStatus |= CARET_POS_START;
        DefImeProc((PVOID)pAbc, IME_OPA_SHOW, 0, 0);
    }
    else
    {
        pAbc->fdwStatus &= ~ABCIME_STATUS_SHOW;
        pAbc->fdwStatus &= ~CARET_POS_START;
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

static BOOL AbcImeOpaEvent(PABCIME pAbc, WPARAM wParam, LPARAM lParam)
{
    if (pAbc == NULL)
    {
        return FALSE;
    }

    return AbcImeKeyEvent(pAbc, (INT32)wParam, (DWORD)lParam);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL AbcImeOpaDestroy(PABCIME pAbc)
{
    if (pAbc == NULL)
    {
        return FALSE;
    }

    free(pAbc);

    return TRUE;
}


/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL AbcImeKeyEvent(PABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
{
    if (!ISKEYUP(dwKeyData))
    {
        return AbcImeKeyDown(pAbc, nKeyCode, dwKeyData);
    }
    else
    {
        return AbcImeKeyUp(pAbc, nKeyCode, dwKeyData);
    }
}

/**********************************************************************
 * Function     AbcImeOpaCommand
 * Purpose      
 * Params       PABCIME pAbc, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static DWORD AbcImeOpaCommand(PABCIME pAbc, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case IME_FN_GETKEYTYPE:
        return GetKeyType((WORD)lParam);

    case IME_NT_MULTAPPAUSE:
        OnMultapPause(pAbc, (INT32)lParam);
        return 0;

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

static BOOL AbcImeKeyDown(PABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
{
    int nIndex = 0;

    for (nIndex = 0; nIndex < NUM_KEYTYPE; nIndex++)
    {
        if ((tabKeyType[nIndex].wKeyEvtMin <= (WORD)nKeyCode)
            && ((WORD)nKeyCode <= tabKeyType[nIndex].wKeyEvtMax))
        {
            if (tabKeyType[nIndex].pfnOnKeyEvent == NULL)
                return FALSE;

            tabKeyType[nIndex].pfnOnKeyEvent(pAbc, nKeyCode, dwKeyData);
            return TRUE;
        }
    }

    return DefImeProc((PVOID)pAbc, IME_OPA_EVENT, (WPARAM)nKeyCode,
        (LPARAM)dwKeyData);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL AbcImeKeyUp(PABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
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

/**********************************************************************
 * Function     OnMultapPause
 * Purpose      
 * Params       PABCIME pAbc
 * Return       
 * Remarks      
 **********************************************************************/

static void OnMultapPause(PABCIME pAbc, INT32 nKeyCode)
{
    WORD wID = 0;

    pAbc->nRepeat = 0;

    if ((IME_EVT_KB2 <= nKeyCode) && (nKeyCode <= IME_EVT_KB9)
        && (pAbc->fdwStatus & ABCIME_TYPE_TITLECASE))
    {
        ImmGetImeIDByName(&wID, (LPSTR)"abc");
        ImcShowImeByID(NULL, wID, TRUE);
        ImmGetImeIDByName(&wID, (LPSTR)"Abc");
        ImcShowImeByID(NULL, wID, FALSE);
    }
}

/**********************************************************************
 * Function     OnKeyEventKB1
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventKB1(PABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
{
    HIMC hImc = NULL;

    if (GETEVTSTATE(dwKeyData) == IME_HKEY_SHORT)
    {
        ImmGetDefaultImc(&hImc);
        if (hImc->v_nAttrib & IMC_ATR_NOWORD)
        {
            SendMessage(GetFocus(), WM_CHAR, (WPARAM)'1', 0);
        }
        else
        {
            ImcToggleTextInput(hImc, TRUE);
            SendMessage(GetFocus(), IME_MSG_CHANGED_IME, 0, 0);
        }
    }
    else if (GETEVTSTATE(dwKeyData) == IME_HKEY_LONG)
    {
        SendMessage(GetFocus(), WM_CHAR, (WPARAM)'1', 0);
    }

    return TRUE;
}

/**********************************************************************
 * Function     OnKeyEventKB2_9
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventKB2_9(PABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
{
    char   chRet = '\0';
    DWORD  dwPrevPos = 0;
    WORD   wID = 0;
    static HIMC hImc = NULL;

    if (GETEVTSTATE(dwKeyData) == IME_HKEY_MULTITAP)
    {
        if (pAbc->fdwStatus & CHAR_OUTPUT_OVERFLOW)
            return FALSE;

        SendMessage(GetFocus(), WM_KEYDOWN, VK_BACK, 0);

        pAbc->nRepeat = (pAbc->nRepeat + 1) % CHARPERKEY;
        if (pAbc->paAbcTable[hImc->v_iLang][nKeyCode - IME_EVT_KB2][pAbc->nRepeat] == 0)
        {
            pAbc->nRepeat = 0;
        }
        chRet = pAbc->paAbcTable[hImc->v_iLang][nKeyCode - IME_EVT_KB2][pAbc->nRepeat];

        SendMessage(GetFocus(), WM_CHAR, (WPARAM)chRet, 0);
    }
    else if (GETEVTSTATE(dwKeyData) == IME_HKEY_LONG)
    {
        if (pAbc->fdwStatus & CHAR_OUTPUT_OVERFLOW)
            return FALSE;

        SendMessage(GetFocus(), WM_KEYDOWN, VK_BACK, 0);

        chRet = (char)('0' + nKeyCode);
        SendMessage(GetFocus(), WM_CHAR, (WPARAM)chRet, 0);
    }
    else
    {
        ImmGetDefaultImc(&hImc);
        pAbc->nRepeat = 0;
        chRet = pAbc->paAbcTable[hImc->v_iLang][nKeyCode - IME_EVT_KB2]
            [pAbc->nRepeat];
        dwPrevPos = SendMessage(GetFocus(), EM_GETSEL, NULL, NULL);
        SendMessage(GetFocus(), WM_CHAR, (WPARAM)chRet, 0);
        if ((DWORD)SendMessage(GetFocus(), EM_GETSEL, NULL, NULL) == dwPrevPos)
        {
            pAbc->fdwStatus |= CHAR_OUTPUT_OVERFLOW;
        }
        else
        {
            pAbc->fdwStatus &= ~CHAR_OUTPUT_OVERFLOW;
        }
    }

    return TRUE;
}

/**********************************************************************
 * Function     OnKeyEventKB0
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventKB0(PABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
{
    DWORD dwPrevPos = 0;

    if (GETEVTSTATE(dwKeyData) == IME_HKEY_LONG)
    {
        if (pAbc->fdwStatus & CHAR_OUTPUT_OVERFLOW)
            return FALSE;

        SendMessage(GetFocus(), WM_KEYDOWN, VK_BACK, 0);
        SendMessage(GetFocus(), WM_CHAR, (WPARAM)'0', 0);
    }
    else
    {
        dwPrevPos = SendMessage(GetFocus(), EM_GETSEL, NULL, NULL);
        SendMessage(GetFocus(), WM_CHAR, (WPARAM)' ', 0);
        if ((DWORD)SendMessage(GetFocus(), EM_GETSEL, NULL, NULL) == dwPrevPos)
        {
            pAbc->fdwStatus |= CHAR_OUTPUT_OVERFLOW;
        }
        else
        {
            pAbc->fdwStatus &= ~CHAR_OUTPUT_OVERFLOW;
        }
    }

    return TRUE;
}
