/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ziabc.c
 *
 * Purpose  : Zi English input module.
 *            
\**************************************************************************/

#include "window.h"
#include "malloc.h"
#include "string.h"
#include "plx_pdaex.h"

#include "imm.h"

#define ZIABCIME_STATUS_SHOW     0x00000001
#define ZIABCIME_STATUS_PRDT     0x00000002
#define CARET_POS_START          0x00010000
#define CHAR_OUTPUT_OVERFLOW     0x00020000
#define ZIABCIME_TYPE_CAPITAL    0x01000000
#define ZIABCIME_TYPE_TITLECASE  0x02000000

#define NUM_KEYTYPE ((int)(sizeof(tabKeyType) / sizeof(tabKeyType[0])))
#define CHARPERKEY                        9
#define IsAlphabetChar(c)        (('A' <= (c) && (c) <= 'Z') \
                                 || ('a' <= (c) && (c) <= 'z'))
#define RetrieveElement(c)       awKeyTable[((c) >= 'a') ? (c) - 'a' : (c) - 'A']

#define ZI_ELEM_BUF_SIZE              0x100
#define ZI_CAND_BUF_SIZE              0x200
#define ZI_SCRA_BUF_SIZE              0x100
#define ZI_MAX_LANGUAGES                  1
#define ZI_MAX_CANDIDATES                10

#define CAND_STATE_SELECT                 0
#define CAND_STATE_SPELL                  1

#define RES_STR_WORD            ML("Word")
#define RES_STR_TEACH           ML("Teach")

#define ENTERPREDICTMODE  SendMessage(pAbc->hwndEdit, IME_MSG_CHANGING_SEL, 0, 0); \
                          pAbc->fdwStatus |= ZIABCIME_STATUS_PRDT
#define LEAVEPREDICTMODE  SendMessage(pAbc->hwndEdit, IME_MSG_CHANGED_SEL, 0, 0); \
                          pAbc->fdwStatus &= ~ZIABCIME_STATUS_PRDT

typedef ZI8UCHAR (*PAZI8UCHAR)[CHARPERKEY];

typedef struct tagZIPARAM
{
    PZI8VOID            pGlobal;
	ZI8GETPARAM		    GetParam;
    ZI8WCHAR            wcElements[ZI_ELEM_BUF_SIZE];
    ZI8WCHAR            wcCandidates[ZI_CAND_BUF_SIZE];
    ZI8UCHAR            ucScratch[ZI_SCRA_BUF_SIZE];

    ZI8LONG             lTotalCand;
    ZI8UCHAR            iOffset;
    ZI8UCHAR            szWord[64];
    ZI8UCHAR            szWordBak[64];
    PAZI8UCHAR          paFkpTable; // ZI8UCHAR (*paFkpTable)[CHARPERKEY];
}
ZIPARAM, *PZIPARAM;

typedef struct tagZIABCIME
{
    DWORD     fdwStatus;
    int       nRepeat;
    char      *pszCharTable;
    PZIPARAM  pZiParam;
    HWND      hwndEdit;
}
ZIABCIME, *PZIABCIME, **PPZIABCIME;

static const BYTE aAbcTable[][CHARPERKEY] =
{
#ifdef IME_NO_CHINESE
    {'a', 'b', 'c', '2', 0x7F, 0x89, 0x1D, 0x8A},
    {'d', 'e', 'f', '3', 0x05, 0x04, 0xD2, 0xD3},
    {'i', 'g', 'h', '4', 0x8C},
    {'j', 'k', 'l', '5'},
    {'m', 'n', 'o', '6', 0x7D, 0x8E, 0x7C},
    {'p', 'q', 'r', 's', '7'},
    {'t', 'u', 'v', '8', 0xD6},
    {'w', 'x', 'y', 'z', '9', 0x90, 0xC8}
#else
    {'a', 'b', 'c', '2'},
    {'d', 'e', 'f', '3'},
    {'i', 'g', 'h', '4'},
    {'j', 'k', 'l', '5'},
    {'m', 'n', 'o', '6'},
    {'p', 'q', 'r', 's', '7'},
    {'t', 'u', 'v', '8'},
    {'w', 'x', 'y', 'z', '9'}
#endif
};

static const BYTE aABCTable[][CHARPERKEY] =
{
#ifdef IME_NO_CHINESE
    {'A', 'B', 'C', '2', 0xC9, 0x81, 0x1C, 0x09},
    {'D', 'E', 'F', '3', 0x1F, 0xCA, 0xCB, 0xCC},
    {'I', 'G', 'H', '4', 0x83, 0xCD},
    {'J', 'K', 'L', '5'},
    {'M', 'N', 'O', '6', 0x5D, 0x85, 0x5C},
    {'P', 'Q', 'R', 'S', '7'},
    {'T', 'U', 'V', '8', 0xD0},
    {'W', 'X', 'Y', 'Z', '9', 0x87, 0xC7}
#else
    {'A', 'B', 'C', '2'},
    {'D', 'E', 'F', '3'},
    {'I', 'G', 'H', '4'},
    {'J', 'K', 'L', '5'},
    {'M', 'N', 'O', '6'},
    {'P', 'Q', 'R', 'S', '7'},
    {'T', 'U', 'V', '8'},
    {'W', 'X', 'Y', 'Z', '9'}
#endif
};

static const WORD awKeyTable[] =
{
    ZI8_CODE_LATIN_ABC,  ZI8_CODE_LATIN_ABC,  ZI8_CODE_LATIN_ABC,
    ZI8_CODE_LATIN_DEF,  ZI8_CODE_LATIN_DEF,  ZI8_CODE_LATIN_DEF,
    ZI8_CODE_LATIN_GHI,  ZI8_CODE_LATIN_GHI,  ZI8_CODE_LATIN_GHI,
    ZI8_CODE_LATIN_JKL,  ZI8_CODE_LATIN_JKL,  ZI8_CODE_LATIN_JKL,
    ZI8_CODE_LATIN_MNO,  ZI8_CODE_LATIN_MNO,  ZI8_CODE_LATIN_MNO,
    ZI8_CODE_LATIN_PQRS, ZI8_CODE_LATIN_PQRS, ZI8_CODE_LATIN_PQRS,
    ZI8_CODE_LATIN_PQRS,
    ZI8_CODE_LATIN_TUV,  ZI8_CODE_LATIN_TUV,  ZI8_CODE_LATIN_TUV,
    ZI8_CODE_LATIN_WXYZ, ZI8_CODE_LATIN_WXYZ, ZI8_CODE_LATIN_WXYZ,
    ZI8_CODE_LATIN_WXYZ
};

static BOOL ZiAbcImeOpaCreate(PPZIABCIME ppAbc, WORD wType);
static BOOL ZiAbcImeOpaShow(PZIABCIME pAbc, BOOL bShow);
static BOOL ZiAbcImeOpaEvent(PZIABCIME pAbc, WPARAM wParam, LPARAM lParam);
static BOOL ZiAbcImeOpaDestroy(PZIABCIME pAbc);
static DWORD ZiAbcImeOpaCommand(PZIABCIME pAbc, WPARAM wParam, LPARAM lParam);

static BOOL ZiAbcImeKeyEvent(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static BOOL ZiAbcImeKeyDown(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static BOOL ZiAbcImeKeyUp(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static DWORD GetKeyType(WORD nKeyCode);
static void OnMultapPause(PZIABCIME pAbc, INT32 nKeyCode);
static void ResetZiParam(PZIABCIME pAbc);
static void SetZiGetParam(PZIPARAM pZiParam);
static BOOL IsAllAlphabet(const char *pszWord);
static BOOL SendCandidate(char *pszWord, int nLength, DWORD dwFormat);
static void PushCandidate(PZIABCIME pAbc);
static void SetCandidateState(int iState);
static int GetNextCandidatesPage(PZIPARAM pZiParam);
static int GetNextCandidate(PZIPARAM pZiParam);
static BOOL RecoginizeWord(PZIABCIME pAbc, DWORD dwCaretPos, int nKeyCode);

static BOOL OnKeyEventKB1(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventKB2_9(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventKB0(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventF3_F4(PZIABCIME pDef, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventReturn(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventClear(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventLeft_Right(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventUp_Down(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData);

static const KEYTYPE tabKeyType[] =
{
    {IME_EVT_KB1,    IME_EVT_KB1,       IME_HKEY_SHORT | IME_HKEY_LONG,
        OnKeyEventKB1},

    {IME_EVT_KB2,    IME_EVT_KB9,       IME_HKEY_INSTANT | IME_HKEY_LONG,
        OnKeyEventKB2_9},

    {IME_EVT_KB0,    IME_EVT_KB0,       IME_HKEY_INSTANT | IME_HKEY_LONG,
        OnKeyEventKB0},

    {IME_EVT_F3,     IME_EVT_F3,        IME_HKEY_MULTITAP | IME_HKEY_LONG,
        OnKeyEventF3_F4},

    {IME_EVT_F4,     IME_EVT_F4,        IME_HKEY_SHORT | IME_HKEY_LONG,
        OnKeyEventF3_F4},

    {IME_EVT_RETURN, IME_EVT_RETURN,    IME_HKEY_INSTANT,
        OnKeyEventReturn},

    {IME_EVT_CLEAR,  IME_EVT_CLEAR,     IME_HKEY_REPEATING,
        OnKeyEventClear},

    {IME_EVT_PAGELEFT, IME_EVT_PAGERIGHT, IME_HKEY_REPEATING,
        OnKeyEventLeft_Right},

    {IME_EVT_PAGEUP, IME_EVT_PAGEDOWN,  IME_HKEY_REPEATING,
        OnKeyEventUp_Down}
};

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

DWORD ZiAbcImeWinMain(PVOID hThis, INT32 nCode, WPARAM nOne, LPARAM nTwo)
{
    switch (LOWORD(nCode))
    {
    case IME_OPA_INIT:
        return (DWORD)TRUE;

    case IME_OPA_CREATE:
        return (DWORD)ZiAbcImeOpaCreate((PPZIABCIME)hThis, HIWORD(nCode));

    case IME_OPA_SHOW:
        return (DWORD)ZiAbcImeOpaShow((PZIABCIME)hThis, TRUE);

    case IME_OPA_HIDE:
        return (DWORD)ZiAbcImeOpaShow((PZIABCIME)hThis, FALSE);

    case IME_OPA_EVENT:
        return (DWORD)ZiAbcImeOpaEvent((PZIABCIME)hThis, nOne, nTwo);

    case IME_OPA_DESTROY:
        return (DWORD)ZiAbcImeOpaDestroy((PZIABCIME)hThis);

    case IME_OPA_COMMAND:
        return (DWORD)ZiAbcImeOpaCommand((PZIABCIME)hThis, nOne, nTwo);
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

static BOOL ZiAbcImeOpaCreate(PPZIABCIME ppAbc, WORD wType)
{
    PZIABCIME pAbc = NULL;

    if (ppAbc == NULL)
    {
        return FALSE;
    }

    pAbc = (PZIABCIME)malloc(sizeof(ZIABCIME));
    if (pAbc == NULL)
    {
        return FALSE;
    }
    memset(pAbc, 0, sizeof(ZIABCIME));

    DefImeProc((PVOID)&pAbc, MAKELONG(IME_OPA_CREATE, wType), 0, 0);

    pAbc->pZiParam = (PZIPARAM)malloc(sizeof(ZIPARAM));
    memset((void*)pAbc->pZiParam, 0, sizeof(ZIPARAM));

    pAbc->pZiParam->pGlobal = ImcGetZiGlobal();

    switch (wType)
    {
    case 0x00B0:
        pAbc->fdwStatus = 0;
        pAbc->pZiParam->paFkpTable = (PAZI8UCHAR)aAbcTable;
        break;

    case 0x00B1:
        pAbc->fdwStatus = ZIABCIME_TYPE_TITLECASE;
        pAbc->pZiParam->paFkpTable = (PAZI8UCHAR)aABCTable;
        break;

    case 0x00B2:
        pAbc->fdwStatus = ZIABCIME_TYPE_CAPITAL;
        pAbc->pZiParam->paFkpTable = (PAZI8UCHAR)aABCTable;
        break;
    }

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

static BOOL ZiAbcImeOpaShow(PZIABCIME pAbc, BOOL bShow)
{
    if (pAbc == NULL)
    {
        return FALSE;
    }

    if (bShow)
    {
        pAbc->fdwStatus |= ZIABCIME_STATUS_SHOW;
        pAbc->fdwStatus |= CARET_POS_START;

        pAbc->hwndEdit = GetFocus();
        SetZiGetParam(pAbc->pZiParam);
    }
    else
    {
        pAbc->fdwStatus &= ~ZIABCIME_STATUS_SHOW;
        pAbc->fdwStatus &= ~CARET_POS_START;

        PushCandidate(pAbc);
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

static BOOL ZiAbcImeOpaEvent(PZIABCIME pAbc, WPARAM wParam, LPARAM lParam)
{
    if (pAbc == NULL)
    {
        return FALSE;
    }

    return ZiAbcImeKeyEvent(pAbc, (INT32)wParam, (DWORD)lParam);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL ZiAbcImeOpaDestroy(PZIABCIME pAbc)
{
    if (pAbc != NULL)
    {
        if (pAbc->pZiParam != NULL)
        {
            free(pAbc->pZiParam);
        }
        free(pAbc);
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

static BOOL ZiAbcImeKeyEvent(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
{
    if (!ISKEYUP(dwKeyData))
    {
        return ZiAbcImeKeyDown(pAbc, nKeyCode, dwKeyData);
    }
    else
    {
        return ZiAbcImeKeyUp(pAbc, nKeyCode, dwKeyData);
    }
}

/**********************************************************************
 * Function     ZiAbcImeOpaCommand
 * Purpose      
 * Params       PZIABCIME pAbc, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static DWORD ZiAbcImeOpaCommand(PZIABCIME pAbc, WPARAM wParam, LPARAM lParam)
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

static BOOL ZiAbcImeKeyDown(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
{
    int nIndex = 0;

    for (nIndex = 0; nIndex < NUM_KEYTYPE; nIndex++)
    {
        if ((tabKeyType[nIndex].wKeyEvtMin <= (WORD)nKeyCode)
            && ((WORD)nKeyCode <= tabKeyType[nIndex].wKeyEvtMax))
        {
            if (tabKeyType[nIndex].pfnOnKeyEvent == NULL)
                return FALSE;

            return tabKeyType[nIndex].pfnOnKeyEvent(pAbc, nKeyCode, dwKeyData);
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

static BOOL ZiAbcImeKeyUp(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
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

    return DefImeProc(NULL, IME_OPA_COMMAND, IME_FN_GETKEYTYPE, wKeyCode);
}

/**********************************************************************
 * Function     OnMultapPause
 * Purpose      
 * Params       PZIABCIME pAbc
 * Return       
 * Remarks      
 **********************************************************************/

static void OnMultapPause(PZIABCIME pAbc, INT32 nKeyCode)
{
    pAbc->nRepeat = 0;
}

/**********************************************************************
 * Function     OnKeyEventKB1
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventKB1(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
{
    PushCandidate(pAbc);

    if (GETEVTSTATE(dwKeyData) == IME_HKEY_SHORT)
    {
        ImcToggleTextInput(NULL, FALSE);
        SendMessage(GetFocus(), IME_MSG_CHANGED_IME, 0, 1);
    }
    else if (GETEVTSTATE(dwKeyData) == IME_HKEY_LONG)
    {
        SendMessage(pAbc->hwndEdit, WM_CHAR, (WPARAM)'1', 0);
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

static BOOL OnKeyEventKB2_9(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
{
    int nLength = 0;
    char chRet = '\0';

    if (GETEVTSTATE(dwKeyData) == IME_HKEY_LONG)
    {
        chRet = (char)('0' + nKeyCode);
        nLength = strlen(pAbc->pZiParam->szWordBak);
        pAbc->pZiParam->szWordBak[nLength] = chRet;
        pAbc->pZiParam->szWordBak[nLength + 1] = '\0';
        SendMessage(pAbc->hwndEdit, EM_REPLACESEL, 0,
            (LPARAM)pAbc->pZiParam->szWordBak);
        ResetZiParam(pAbc);
    }
    else
    {
        if ((pAbc->pZiParam->lTotalCand == 0)
            && (pAbc->pZiParam->GetParam.ElementCount > 0))
        {
            // In Teach Mode, do nothing.
            return TRUE;
        }

        if (pAbc->pZiParam->GetParam.ElementCount == 0)
        {
            ENTERPREDICTMODE;
        }
        pAbc->pZiParam->wcElements[pAbc->pZiParam->GetParam.ElementCount++]
            = ZI8_BASE_LATIN_KEYS + (ZI8WCHAR)nKeyCode;
        pAbc->pZiParam->GetParam.FirstCandidate = 0;
        pAbc->pZiParam->lTotalCand = Zi8GetCandidatesCount(
            &pAbc->pZiParam->GetParam, pAbc->pZiParam->pGlobal);
        strcpy(pAbc->pZiParam->szWordBak, pAbc->pZiParam->szWord);
        if (pAbc->pZiParam->lTotalCand > 0)
        {
            nLength = GetNextCandidatesPage(pAbc->pZiParam);
        }
        if (pAbc->pZiParam->lTotalCand == 0)
        {
            // Enter Teach Mode
            strcat(pAbc->pZiParam->szWord, "?");
            nLength = strlen(pAbc->pZiParam->szWord);
        }
        if (!SendCandidate(pAbc->pZiParam->szWord, nLength, pAbc->fdwStatus))
        {
            pAbc->pZiParam->GetParam.ElementCount--;
            pAbc->pZiParam->lTotalCand = Zi8GetCandidatesCount(
                &pAbc->pZiParam->GetParam, pAbc->pZiParam->pGlobal);
            if (pAbc->pZiParam->lTotalCand > 0)
            {
                nLength = GetNextCandidatesPage(pAbc->pZiParam);
            }

            return TRUE;
        }

        if (pAbc->pZiParam->GetParam.ElementCount == 1)
        {
            SetCandidateState(CAND_STATE_SELECT);
        }
        else if (pAbc->pZiParam->lTotalCand == 0)
        {
            SetCandidateState(CAND_STATE_SPELL);
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

static BOOL OnKeyEventKB0(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
{
    DWORD dwCaretPos = 0;

    if (GETEVTSTATE(dwKeyData) == IME_HKEY_LONG)
    {
        if (pAbc->fdwStatus & CHAR_OUTPUT_OVERFLOW)
            return TRUE;

        SendMessage(pAbc->hwndEdit, WM_KEYDOWN, VK_BACK, 0);
        SendMessage(pAbc->hwndEdit, WM_CHAR, (WPARAM)'0', 0);
    }
    else
    {
        PushCandidate(pAbc);
        dwCaretPos = (DWORD)SendMessage(pAbc->hwndEdit, EM_GETSEL, 0, 0);
        SendMessage(pAbc->hwndEdit, WM_CHAR, (WPARAM)' ', 0);
        if ((DWORD)SendMessage(pAbc->hwndEdit, EM_GETSEL, 0, 0) == dwCaretPos)
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
 * Function     OnKeyEventF3
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventF3_F4(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
{
    PushCandidate(pAbc);

    return DefImeProc((PVOID)pAbc, IME_OPA_EVENT, (WPARAM)nKeyCode,
        (LPARAM)dwKeyData);
}

/**********************************************************************
 * Function     OnKeyEventReturn
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventReturn(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
{
    int nLength = 0;

    // No input element, return process back to application.
    if (pAbc->pZiParam->GetParam.ElementCount == 0)
        return FALSE;

    if ((pAbc->pZiParam->lTotalCand == 0)
        && (pAbc->pZiParam->GetParam.ElementCount > 0))
    {
        // In Teach Mode, enter spell interface.
        SendMessage(pAbc->hwndEdit, WM_KEYDOWN, VK_BACK, 0);
        ResetZiParam(pAbc);
        ImeZiSpellWinMain((char*)pAbc->pZiParam->szWordBak);
        return TRUE;
    }

    // No more candidates, do nothing.
    if (pAbc->pZiParam->GetParam.FirstCandidate + pAbc->pZiParam->iOffset >=
        pAbc->pZiParam->lTotalCand - 1)
    {
        return TRUE;
    }

    strcpy(pAbc->pZiParam->szWordBak, pAbc->pZiParam->szWord);
    // Get next candidate.
    nLength = GetNextCandidate(pAbc->pZiParam);
    if (nLength > 0)
    {
        SendCandidate(pAbc->pZiParam->szWord, nLength, pAbc->fdwStatus);
    }

    return TRUE;
}

/**********************************************************************
 * Function     OnKeyEventClear
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventClear(PZIABCIME pAbc, INT32 nKeyCode, DWORD dwKeyData)
{
    int nLength = 0;
    BOOL bQuitSpell = FALSE;

    if (pAbc->pZiParam->GetParam.ElementCount == 0)
    {
        return DefImeProc(pAbc, IME_OPA_EVENT, (WPARAM)nKeyCode,
            (LPARAM)dwKeyData);
    }

    pAbc->pZiParam->GetParam.ElementCount--;
    if (pAbc->pZiParam->GetParam.ElementCount > 0)
    {
        if (pAbc->pZiParam->lTotalCand == 0)
        {
            // In Teach Mode
            bQuitSpell = TRUE;
        }

        pAbc->pZiParam->GetParam.FirstCandidate = 0;
        pAbc->pZiParam->lTotalCand = Zi8GetCandidatesCount(
            &pAbc->pZiParam->GetParam, pAbc->pZiParam->pGlobal);
        if (pAbc->pZiParam->lTotalCand > 0)
        {
            nLength = GetNextCandidatesPage(pAbc->pZiParam);
        }
        else
        {
            while (pAbc->pZiParam->lTotalCand == 0)
            {
                pAbc->pZiParam->GetParam.ElementCount--;
                pAbc->pZiParam->lTotalCand = Zi8GetCandidatesCount(
                    &pAbc->pZiParam->GetParam, pAbc->pZiParam->pGlobal);
                if (pAbc->pZiParam->GetParam.ElementCount <= 0)
                    break;
            }
            if (pAbc->pZiParam->lTotalCand > 0)
            {
                nLength = GetNextCandidatesPage(pAbc->pZiParam);
            }
        }
    }
    else
    {
        strcpy(pAbc->pZiParam->szWord, "");
        nLength = 0;
    }
    SendCandidate(pAbc->pZiParam->szWord, nLength, pAbc->fdwStatus);
    if (bQuitSpell)
    {
        SetCandidateState(CAND_STATE_SELECT);
    }
    if (pAbc->pZiParam->GetParam.ElementCount == 0)
    {
        ResetZiParam(pAbc);
    }

    return TRUE;
}

/**********************************************************************
 * Function     OnKeyEventLeft_Right
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventLeft_Right(PZIABCIME pAbc, INT32 nKeyCode,
                                 DWORD dwKeyData)
{
    DWORD dwCaretPos = 0, dwSelStart = 0, dwSelEnd = 0;
    int nLength = 0;

    if (pAbc->pZiParam->GetParam.ElementCount == 0)
    {
        nLength = GetWindowTextLength(pAbc->hwndEdit);
        dwCaretPos = SendMessage(pAbc->hwndEdit, EM_GETSEL, (WPARAM)&dwSelStart,
            (LPARAM)&dwSelEnd);
        if (((nKeyCode == IME_EVT_PAGERIGHT) && (dwSelEnd == (DWORD)nLength))
            || ((nKeyCode == IME_EVT_PAGELEFT) && (dwSelStart == 0)))
        {
            return FALSE;
        }

        if (!RecoginizeWord(pAbc, dwCaretPos, nKeyCode))
            return FALSE;
    }
    else
    {
        SendMessage(pAbc->hwndEdit, EM_GETSEL, (WPARAM)&dwSelStart,
            (LPARAM)&dwSelEnd);
        
        if (pAbc->pZiParam->lTotalCand == 0)
        {
            nLength = strlen(pAbc->pZiParam->szWord);
            pAbc->pZiParam->szWord[nLength - 1] = '\0';
            SendMessage(pAbc->hwndEdit, EM_REPLACESEL, 0,
                (LPARAM)pAbc->pZiParam->szWord);
            if (nKeyCode == IME_EVT_PAGELEFT)
            {
                SendMessage(pAbc->hwndEdit, EM_SETSEL, (WPARAM)dwSelStart,
                    (LPARAM)dwSelStart);
            }
        }
        else
        {
            if (nKeyCode == IME_EVT_PAGERIGHT)
                dwSelStart = dwSelEnd;
            SendMessage(pAbc->hwndEdit, EM_SETSEL, (WPARAM)dwSelStart,
                (LPARAM)dwSelStart);
        }

        ResetZiParam(pAbc);
    }

    return TRUE;
}

/**********************************************************************
 * Function     OnKeyEventUp_Down
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventUp_Down(PZIABCIME pAbc, INT32 nKeyCode,
                                 DWORD dwKeyData)
{
    DWORD dwCaretPos = 0, dwCaretPosPrev = 0;

    if (pAbc->pZiParam->GetParam.ElementCount > 0)
    {
        PushCandidate(pAbc);
    }

    dwCaretPosPrev = SendMessage(pAbc->hwndEdit, EM_GETSEL, 0, 0);
    SendMessage(pAbc->hwndEdit, WM_KEYDOWN,
        (nKeyCode == IME_EVT_PAGEDOWN) ? VK_DOWN : VK_UP, 0);
    dwCaretPos = SendMessage(pAbc->hwndEdit, EM_GETSEL, 0, 0);
    if (dwCaretPos == dwCaretPosPrev)
    {
        return TRUE;
    }

    RecoginizeWord(pAbc, dwCaretPos, nKeyCode);

    return TRUE;
}

/**********************************************************************
 * Function     GetNextCandidatesPage
 * Purpose      
 * Params       pZiParam
 * Return       
 * Remarks      
 **********************************************************************/

static int GetNextCandidatesPage(PZIPARAM pZiParam)
{
    int nCand = 0, nLength = 0;
    ZI8UCHAR szWordTemp[64] = "";

    if (pZiParam->GetParam.ElementCount == 1)
    {
        // Patch for Zi Text single key press, such as 3, 5, 6, 7, 8, 9,
        // these return 0x3E
        pZiParam->szWord[0] = pZiParam->paFkpTable[
            pZiParam->GetParam.pElements[0] - ZI8_BASE_LATIN_KEYS
            - IME_EVT_KB2][0];
        pZiParam->szWord[1] = '\0';

        pZiParam->iOffset = 0;
        pZiParam->lTotalCand = strlen(pZiParam->paFkpTable[pZiParam->GetParam.
            pElements[pZiParam->iOffset] - ZI8_BASE_LATIN_KEYS - IME_EVT_KB2]);
        pZiParam->GetParam.FirstCandidate = 0;

        return 1;
    }
    else
    {
        while (pZiParam->GetParam.FirstCandidate < pZiParam->lTotalCand)
        {
            nCand = Zi8GetCandidates(&pZiParam->GetParam, pZiParam->pGlobal);
            for (pZiParam->iOffset = 0; pZiParam->iOffset < nCand;
                 pZiParam->iOffset++)
            {
                nLength = Zi8Copy(szWordTemp, &pZiParam->GetParam,
                    pZiParam->iOffset, pZiParam->pGlobal);
                if (IsAllAlphabet(szWordTemp))
                {
                    strcpy(pZiParam->szWord, szWordTemp);
                    return (nLength - 1);
                }
                pZiParam->lTotalCand--;
            }

            pZiParam->GetParam.FirstCandidate += ZI_MAX_CANDIDATES;
        }
    }

    return 0;
}

/**********************************************************************
 * Function     GetNextCandidate
 * Purpose      
 * Params       pZiParam
 * Return       
 * Remarks      
 **********************************************************************/

static int GetNextCandidate(PZIPARAM pZiParam)
{
    ZI8UCHAR szWordTemp[64] = "";
    int nLength = 0;

    if (pZiParam->GetParam.ElementCount == 1)
    {
        // Patch for Zi Text single key press, such as 3, 5, 6, 7, 8, 9,
        // these return 0x3E
        if (pZiParam->iOffset < pZiParam->lTotalCand - 1)
        {
            pZiParam->iOffset++;

            pZiParam->szWord[0] = pZiParam->paFkpTable[pZiParam->GetParam.pElements[0]
                - ZI8_BASE_LATIN_KEYS - IME_EVT_KB2][pZiParam->iOffset];
            pZiParam->szWord[1] = '\0';

            nLength = 1;
        }
        else
        {
            nLength = 0;
        }
    }
    else
    {
        while (pZiParam->iOffset < ZI_MAX_CANDIDATES - 1)
        {
            pZiParam->iOffset++;
            nLength = Zi8Copy(szWordTemp, &pZiParam->GetParam,
                pZiParam->iOffset, pZiParam->pGlobal);
            if (nLength <= 1)
            {
                return 0;
            }
            else if (IsAllAlphabet(szWordTemp))
            {
                strcpy(pZiParam->szWord, szWordTemp);
                return (nLength - 1);
            }
        }
        
        pZiParam->GetParam.FirstCandidate += ZI_MAX_CANDIDATES;
        nLength = GetNextCandidatesPage(pZiParam);
    }

    return nLength;
}

/**********************************************************************
 * Function     ResetZiParam
 * Purpose      
 * Params       pZiParam
 * Return       
 * Remarks      
 **********************************************************************/

static void ResetZiParam(PZIABCIME pAbc)
{
    WORD wID = 0;

    pAbc->pZiParam->GetParam.ElementCount = 0;
    pAbc->pZiParam->GetParam.FirstCandidate = 0;
    pAbc->pZiParam->lTotalCand = 0;
    pAbc->pZiParam->iOffset = 0;
    pAbc->pZiParam->szWord[0] = '\0';
    if (pAbc->fdwStatus & ZIABCIME_STATUS_PRDT)
    {
        LEAVEPREDICTMODE;
    }
    if (pAbc->fdwStatus & ZIABCIME_TYPE_TITLECASE)
    {
        ImmGetImeIDByName(&wID, (LPSTR)"Ziabc");
        ImcShowImeByID(NULL, wID, TRUE);
        ImmGetImeIDByName(&wID, (LPSTR)"ZiAbc");
        ImcShowImeByID(NULL, wID, FALSE);
    }
}

/**********************************************************************
 * Function     SetZiGetParam
 * Purpose      
 * Params       pZiParam
 * Return       
 * Remarks      
 **********************************************************************/

static void SetZiGetParam(PZIPARAM pZiParam)
{
    pZiParam->GetParam.Language = ZI8_LANG_EN;
    pZiParam->GetParam.GetMode = ZI8_GETMODE_DEFAULT;
    pZiParam->GetParam.SubLanguage = ZI8_SUBLANG_DEFAULT;
    pZiParam->GetParam.Context = ZI8_GETCONTEXT_DEFAULT;
    pZiParam->GetParam.GetOptions = ZI8_GETOPTION_STRINGS;
    pZiParam->GetParam.pElements = pZiParam->wcElements;
    pZiParam->GetParam.ElementCount = 0;
    pZiParam->GetParam.pCurrentWord = NULL;
    pZiParam->GetParam.WordCharacterCount = 0;
    pZiParam->GetParam.pCandidates = pZiParam->wcCandidates;
    pZiParam->GetParam.MaxCandidates = ZI_MAX_CANDIDATES;
    pZiParam->GetParam.FirstCandidate = 0;
    pZiParam->GetParam.wordCandidates = 0;
    pZiParam->GetParam.count = 0;
    pZiParam->GetParam.letters = 0;
    pZiParam->GetParam.pScratch = pZiParam->ucScratch;
}

/**********************************************************************
 * Function     SendCandidate
 * Purpose      
 * Params       pszWord, nLength, dwFormat
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL SendCandidate(char *pszWord, int nLength, DWORD dwFormat)
{
    HWND hwndEdit = NULL;
    DWORD dwSelStart = 0, dwSelEnd = 0;
    int nLimitLen = 0, nTextLen = 0;

    hwndEdit = GetFocus();
    nTextLen = GetWindowTextLength(hwndEdit);
    SendMessage(hwndEdit, EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd);
    nTextLen -= dwSelEnd - dwSelStart;
    nLimitLen = SendMessage(hwndEdit, EM_GETLIMITTEXT, 0, 0);
    if (nTextLen + nLength > nLimitLen)
        return FALSE;

    if (nLength > 1)
    {
        if (dwFormat & ZIABCIME_TYPE_TITLECASE)
        {
            if (('a' <= pszWord[0]) && (pszWord[0] <= 'z'))
                pszWord[0] -= 'a' - 'A';
        }
        else if (dwFormat & ZIABCIME_TYPE_CAPITAL)
        {
            strupr(pszWord);
        }
    }

    SendMessage(hwndEdit, EM_REPLACESEL, 0, (LPARAM)pszWord);
    dwSelStart = min(dwSelStart, dwSelEnd);
    dwSelEnd = dwSelStart + nLength;
    SendMessage(hwndEdit, EM_SETSEL, (WPARAM)dwSelStart,
        (LPARAM)dwSelEnd);

    return TRUE;
}

/**********************************************************************
* Function     PushCandidate
* Purpose      
* Params       pZiParam
* Return       
* Remarks      
**********************************************************************/

static void PushCandidate(PZIABCIME pAbc)
{
    DWORD dwSelStart = 0, dwSelEnd = 0;
    int nLength = 0;

    if (pAbc->pZiParam->GetParam.ElementCount == 0)
        return;

    if (pAbc->pZiParam->lTotalCand == 0)
    {
        nLength = strlen(pAbc->pZiParam->szWord);
        pAbc->pZiParam->szWord[nLength - 1] = '\0';
        SendMessage(pAbc->hwndEdit, EM_REPLACESEL, 0,
            (LPARAM)pAbc->pZiParam->szWord);
    }
    else
    {
        SendMessage(pAbc->hwndEdit, EM_GETSEL, (WPARAM)&dwSelStart,
            (LPARAM)&dwSelEnd);
        SendMessage(pAbc->hwndEdit, EM_SETSEL, (WPARAM)dwSelEnd,
            (LPARAM)dwSelEnd);
    }

    ResetZiParam(pAbc);
}

/**********************************************************************
 * Function     SetCandidateState
 * Purpose      
 * Params       int iState
 * Return       
 * Remarks      
 **********************************************************************/

static void SetCandidateState(int iState)
{
    switch (iState)
    {
    case CAND_STATE_SELECT:
        SendMessage(GetFocus(), IME_MSG_CHANGED_SEL, 0, (LPARAM)RES_STR_WORD);
        break;

    case CAND_STATE_SPELL:
        SendMessage(GetFocus(), IME_MSG_CHANGED_SEL, 0, (LPARAM)RES_STR_TEACH);
        break;

    default:
        break;
    }
}

/**********************************************************************
 * Function     RecoginizeWord
 * Purpose      
 * Params       PZIABCIME pAbc
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL RecoginizeWord(PZIABCIME pAbc, DWORD dwCaretPos, int nKeyCode)
{
    DWORD dwSelStart = 0, dwSelEnd = 0;
    int nLength = 0, i = 0, nLow = 0, nHigh = 0;
    char *pszText = NULL;

    // 1.Get the whole text string, within which there are no non-alphabet
    // characters
    nLength = GetWindowTextLength(pAbc->hwndEdit);
    pszText = (char*)malloc(nLength + 1);
    GetWindowText(pAbc->hwndEdit, pszText, nLength + 1);
    dwSelStart = LOWORD(dwCaretPos);
    dwSelEnd = HIWORD(dwCaretPos);

    if (nKeyCode == IME_EVT_PAGERIGHT)
    {
        if (dwSelStart != 0 && IsAlphabetChar(pszText[dwSelStart - 1]))
            goto exit_false;

        dwSelEnd = dwSelStart;
        while (IsAlphabetChar(pszText[dwSelEnd]))
        {
            dwSelEnd++;
        }
    }
    else if (nKeyCode == IME_EVT_PAGELEFT)
    {
        if (dwSelEnd != (DWORD)nLength && IsAlphabetChar(pszText[dwSelEnd]))
            goto exit_false;

        dwSelStart = dwSelEnd;
        while (IsAlphabetChar(pszText[dwSelStart - 1]))
        {
            dwSelStart--;
        }
    }
    else
    {
        if (IsAlphabetChar(pszText[dwSelStart - 1])
            && IsAlphabetChar(pszText[dwSelEnd]))
        {
            while (IsAlphabetChar(pszText[dwSelStart - 1]))
            {
                dwSelStart--;
            }
            while (IsAlphabetChar(pszText[dwSelEnd]))
            {
                dwSelEnd++;
            }
        }
    }

    if (dwSelStart == dwSelEnd)
        goto exit_false;

rec_step2:
    // 2.Revert to elements stream
    pAbc->pZiParam->GetParam.ElementCount = 0;
    for (i = (int)dwSelStart; i < (int)dwSelEnd; i++)
    {
        pAbc->pZiParam->wcElements[pAbc->pZiParam->GetParam.ElementCount++]
            = RetrieveElement(pszText[i]);
        if (pAbc->pZiParam->GetParam.ElementCount >= ZI_ELEM_BUF_SIZE)
            break;
    }

    // 3.Evaluate the maximum match of elements stream by bisearch
    nLow = 0;
    nHigh = pAbc->pZiParam->GetParam.ElementCount;
    pAbc->pZiParam->GetParam.FirstCandidate = 0;

    while (nLow < nHigh)
    {
        pAbc->pZiParam->lTotalCand = Zi8GetCandidatesCount(
            &pAbc->pZiParam->GetParam, pAbc->pZiParam->pGlobal);
        if (pAbc->pZiParam->lTotalCand > 0)
        {
            nLow = pAbc->pZiParam->GetParam.ElementCount;
        }
        else
        {
            nHigh = pAbc->pZiParam->GetParam.ElementCount;
            goto exit_false;
        }
        if (pAbc->pZiParam->GetParam.ElementCount == (ZI8UCHAR)((nLow + nHigh) / 2))
            break;

        pAbc->pZiParam->GetParam.ElementCount = ((nLow + nHigh) / 2);
    }

    // 4.Get the exact elements stream according to the current text string
    while (pAbc->pZiParam->lTotalCand > 0)
    {
        nLength = GetNextCandidatesPage(pAbc->pZiParam);
        while (pAbc->pZiParam->GetParam.ElementCount != nLength
            || strnicmp(&pszText[dwSelStart], pAbc->pZiParam->szWord,
            pAbc->pZiParam->GetParam.ElementCount)
            != 0)
        {
            nLength = GetNextCandidate(pAbc->pZiParam);
            if (nLength == 0)
                break;
        }
        if (nLength > 0)
            break;

        if (nLength == 0)
            goto exit_false;

        pAbc->pZiParam->GetParam.ElementCount--;
        pAbc->pZiParam->GetParam.FirstCandidate = 0;
        pAbc->pZiParam->lTotalCand = Zi8GetCandidatesCount(
            &pAbc->pZiParam->GetParam, pAbc->pZiParam->pGlobal);
    }

    if (nKeyCode != IME_EVT_PAGERIGHT)
    {
        if (dwSelStart + pAbc->pZiParam->GetParam.ElementCount < HIWORD(dwCaretPos))
        {
            dwSelStart += pAbc->pZiParam->GetParam.ElementCount;
            goto rec_step2;
        }
    }

    ENTERPREDICTMODE;
    free(pszText);

    dwSelEnd = dwSelStart + pAbc->pZiParam->GetParam.ElementCount;
    SendMessage(pAbc->hwndEdit, EM_SETSEL, (WPARAM)dwSelStart,
        (LPARAM)dwSelEnd);
    SetCandidateState(CAND_STATE_SELECT);

    return TRUE;

exit_false:
    ResetZiParam(pAbc);
    if (pszText != NULL)
        free(pszText);

    return FALSE;
}

/**********************************************************************
 * Function     IsAllAlphabet
 * Purpose      
 * Params       const char *pszWord
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL IsAllAlphabet(const char *pszWord)
{
    char *pch = NULL;

    if (*pszWord == '\0')
        return FALSE;

    for (pch = (char*)pszWord; *pch != '\0'; pch++)
    {
        if (('0' < *pch) && (*pch < '9'))
            return FALSE;
    }

    return TRUE;
}
