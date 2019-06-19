/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 *    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
 *                       All Rights Reserved
 *
 * Module   :  defime.c
 *
 * Purpose  :  
 *
\**************************************************************************/

#include "window.h"
#include "string.h"
#include "imm.h"

#define CHAR_OUTPUT_OVERFLOW  0x00020000

#define NUM_KEYTYPE ((int)(sizeof(tabKeyType) / sizeof(tabKeyType[0])))

typedef struct tagDEFIME
{
    DWORD fdwStatus;
    int   nRepeat;
    char  *pszCharTable;
}
DEFIME, *PDEFIME, **PPDEFIME;

static const char *pszCharTable = ".,?!-&()/";

static BOOL DefImeOpaCreate(PPDEFIME ppDef, WORD wType);
static BOOL DefImeOpaShow(PDEFIME pDef, BOOL bShow);
static BOOL DefImeOpaEvent(PDEFIME pDef, WPARAM wParam, LPARAM lParam);
static BOOL DefImeOpaDestroy(PDEFIME pDef);
static DWORD DefImeOpaCommand(PDEFIME pDef, WPARAM wParam, LPARAM lParam);

static BOOL DefImeKeyEvent(PDEFIME pDef, INT32 nKeyCode, DWORD dwKeyData);
static BOOL DefImeKeyDown(PDEFIME pDef, INT32 nKeyCode, DWORD dwKeyData);
static BOOL DefImeKeyUp(PDEFIME pDef, INT32 nKeyCode, DWORD dwKeyData);

static DWORD GetKeyType(WORD nKeyCode);
static DWORD GetCharTab(PDEFIME pDef, char** ppszCharTable);

static BOOL OnKeyEventF3(PDEFIME pDef, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventF4(PDEFIME pDef, INT32 nKeyCode, DWORD dwKeyData);

static const KEYTYPE tabKeyType[] =
{
    {IME_EVT_KB1,    IME_EVT_KB0,       IME_HKEY_INSTANT,
        NULL},

    {IME_EVT_F3,     IME_EVT_F3,        IME_HKEY_MULTITAP | IME_HKEY_LONG,
        OnKeyEventF3},

    {IME_EVT_F4,     IME_EVT_F4,        IME_HKEY_SHORT | IME_HKEY_LONG,
        OnKeyEventF4},

    {IME_EVT_CLEAR,  IME_EVT_CLEAR,     IME_HKEY_REPEATING,
        NULL},

    {IME_EVT_PAGEUP, IME_EVT_PAGERIGHT, IME_HKEY_REPEATING,
        NULL},
};

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

DWORD DefImeProc(PVOID hThis, INT32 nCode, WPARAM nOne, LPARAM nTwo)
{
    switch (LOWORD(nCode))
    {
//    case IME_OPA_INIT:
//        return (DWORD)TRUE;
//
    case IME_OPA_CREATE:
        return (DWORD)DefImeOpaCreate(hThis, HIWORD(nCode));
//
    case IME_OPA_SHOW:
        return (DWORD)DefImeOpaShow(hThis, TRUE);
//
//    case IME_OPA_HIDE:
//        return (DWORD)DefImeOpaShow(hThis, FALSE);

    case IME_OPA_EVENT:
        return (DWORD)DefImeOpaEvent(hThis, nOne, nTwo);

//    case IME_OPA_DESTROY:
//        return (DWORD)DefImeOpaDestroy(hThis);

    case IME_OPA_COMMAND:
        return (DWORD)DefImeOpaCommand(hThis, nOne, nTwo);
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

static BOOL DefImeOpaCreate(PPDEFIME ppDef, WORD wType)
{
    PDEFIME pDef = NULL;

    if (ppDef == NULL)
    {
        return FALSE;
    }

    pDef = *ppDef;

    pDef->pszCharTable = (char*)pszCharTable;

    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL DefImeOpaShow(PDEFIME pDef, BOOL bShow)
{
    char* pszMultitapTable = NULL;

    if (pDef == NULL)
    {
        return FALSE;
    }

    if (bShow)
    {
        SendMessage(GetFocus(), IME_MSG_GETLIMITSTR, 0,
            (LPARAM)&pszMultitapTable);

        if (pszMultitapTable == NULL)
        {
            pDef->pszCharTable = (char*)pszCharTable;
        }
        else
        {
            pDef->pszCharTable = pszMultitapTable;
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

static BOOL DefImeOpaEvent(PDEFIME pDef, WPARAM wParam, LPARAM lParam)
{
    if (pDef == NULL)
    {
        return FALSE;
    }

    return DefImeKeyEvent(pDef, (INT32)wParam, (DWORD)lParam);
}

/**********************************************************************
 * Function     DefImeOpaCommand
 * Purpose      
 * Params       pDef, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static DWORD DefImeOpaCommand(PDEFIME pDef, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case IME_FN_GETKEYTYPE:
        return GetKeyType((WORD)lParam);

    case IME_FN_GETCHARTAB:
        return GetCharTab(pDef, (char**)lParam);

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

static BOOL DefImeKeyEvent(PDEFIME pDef, INT32 nKeyCode, DWORD dwKeyData)
{
    if (!ISKEYUP(dwKeyData))
    {
        return DefImeKeyDown(pDef, nKeyCode, dwKeyData);
    }
    else
    {
        return DefImeKeyUp(pDef, nKeyCode, dwKeyData);
    }
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL DefImeKeyDown(PDEFIME pDef, INT32 nKeyCode, DWORD dwKeyData)
{
    int nIndex = 0;

    for (nIndex = 0; nIndex < NUM_KEYTYPE; nIndex++)
    {
        if ((tabKeyType[nIndex].wKeyEvtMin <= (WORD)nKeyCode)
            && ((WORD)nKeyCode <= tabKeyType[nIndex].wKeyEvtMax))
        {
            if (tabKeyType[nIndex].pfnOnKeyEvent == NULL)
                return FALSE;

            tabKeyType[nIndex].pfnOnKeyEvent(pDef, nKeyCode, dwKeyData);
            return TRUE;
        }
    }

    return FALSE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL DefImeKeyUp(PDEFIME pDef, INT32 nKeyCode, DWORD dwKeyData)
{
    BOOL bRet = FALSE;

    bRet = FALSE;
    return bRet;
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

    return 0;
}

/**********************************************************************
 * Function     GetCharTab
 * Purpose      
 * Params       pDef ppszCharTable
 * Return       
 * Remarks      
 **********************************************************************/

static DWORD GetCharTab(PDEFIME pDef, char** ppszCharTable)
{
    *ppszCharTable = pDef->pszCharTable;
    return 0;
}

/**********************************************************************
 * Function     OnKeyEventF3
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventF3(PDEFIME pDef, INT32 nKeyCode, DWORD dwKeyData)
{
    char   chRet = '\0';
    DWORD  dwPrevPos = 0;
    int    nNumChar = 0;

    if (GETEVTSTATE(dwKeyData) == IME_HKEY_MULTITAP)
    {
        if (pDef->fdwStatus & CHAR_OUTPUT_OVERFLOW)
            return FALSE;

        nNumChar = strlen(pDef->pszCharTable);
        if (nNumChar == 0)
            return FALSE;

        SendMessage(GetFocus(), WM_KEYDOWN, VK_BACK, 0);
        pDef->nRepeat = (pDef->nRepeat + 1) % nNumChar;
        chRet = pDef->pszCharTable[pDef->nRepeat];
        SendMessage(GetFocus(), WM_CHAR, (WPARAM)chRet, 0);
    }
    else if (GETEVTSTATE(dwKeyData) == IME_HKEY_LONG)
    {
        if (pDef->fdwStatus & CHAR_OUTPUT_OVERFLOW)
            return FALSE;

        nNumChar = strlen(pDef->pszCharTable);
        if (nNumChar == 0)
            return FALSE;

        SendMessage(GetFocus(), WM_KEYDOWN, VK_BACK, 0);
    }
    else
    {
        pDef->nRepeat = 0;
        chRet = pDef->pszCharTable[pDef->nRepeat];
        dwPrevPos = SendMessage(GetFocus(), EM_GETSEL, NULL, NULL);
        SendMessage(GetFocus(), WM_CHAR, (WPARAM)chRet, 0);
        if ((DWORD)SendMessage(GetFocus(), EM_GETSEL, NULL, NULL) == dwPrevPos)
        {
            pDef->fdwStatus |= CHAR_OUTPUT_OVERFLOW;
        }
        else
        {
            pDef->fdwStatus &= ~CHAR_OUTPUT_OVERFLOW;
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

static BOOL OnKeyEventF4(PDEFIME pDef, INT32 nKeyCode, DWORD dwKeyData)
{
    if (GETEVTSTATE(dwKeyData) == IME_HKEY_LONG)
    {
        ImeLangSelWinMain();
    }

    return TRUE;
}
