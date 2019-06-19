/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 *    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
 *                       All Rights Reserved
 *
 * Module   :  zichn.c
 *
 * Purpose  :  Chinese input method module.
 *
\**************************************************************************/

#include "window.h"
#include "malloc.h"
#include "string.h"
#include "plx_pdaex.h"

#include "imm.h"
#include "ziime.h"

#define ZICHNIME_STATUS_SHOW     0x00000001
#define ZICHNIME_STATUS_PRDT     0x00000002
#define ZICHNIME_STATUS_SEL      0x00000004
#define ZICHNIME_STATUS_CNT      0x00000008

#define CX_GAP                   1
#define CY_GAP                   1
#define CX_MARGIN                8
#define CY_MARGIN                2
#define CY_ASSOCAREA             16
#define CY_CANDIAREA             20
#define CX_IMEWND                (PLX_WIN_WIDTH - 2 * CX_GAP)
#define CY_IMEWND                (CY_ASSOCAREA + CY_CANDIAREA + 2 * CY_MARGIN)
#define CX_ASSOCSPACE            6
#define Y_IMEWND(p) ((p) == 0 ? (PLX_WIN_TOP_Y + PLX_WIN_HIGH                  \
                    - GetSystemMetrics(SM_CYMENU) - CY_IMEWND - CY_GAP)        \
                    : (PLX_WIN_TOP_Y + GetSystemMetrics(SM_CYCAPTION) + CY_GAP))

#define NUM_KEYTYPE ((int)(sizeof(tabKeyType) / sizeof(tabKeyType[0])))

#define WM_SHOWIMEWND            (WM_USER + 0x0604)

#define RES_STR_CLEAR            ML("Clear")
#define RES_STR_CANCEL           ML("Cancel")

#define ENTERPREDICTMODE  SendMessage(pChn->hWnd, IME_MSG_CHANGING_SEL, 0, 0); \
                          pChn->fdwStatus |= ZICHNIME_STATUS_PRDT
#define LEAVEPREDICTMODE  SendMessage(pChn->hWnd, IME_MSG_CHANGED_SEL, 0, 0); \
                          pChn->fdwStatus &= ~ZICHNIME_STATUS_PRDT

static BOOL ZiChnImeOpaInit(void);
static BOOL ZiChnImeOpaCreate(PPZICHNIME ppChn, WORD wType);
static BOOL ZiChnImeOpaShow(PZICHNIME pChn, BOOL bShow);
static BOOL ZiChnImeOpaEvent(PZICHNIME pChn, WPARAM wParam, LPARAM lParam);
static BOOL ZiChnImeOpaDestroy(PZICHNIME pChn);
static DWORD ZiChnImeOpaCommand(PZICHNIME pChn, WPARAM wParam, LPARAM lParam);

static BOOL ZiChnImeKeyEvent(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData);
static BOOL ZiChnImeKeyDown(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData);
static BOOL ZiChnImeKeyUp(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData);
static DWORD GetKeyType(WORD nKeyCode);
static void ResetZiParam(PZICHNIME pChn);
static int GetContextPage(PZICHNPARAM pZiParam);

static BOOL OnKeyEventKB1_9(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventKB0(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventF3_F4(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventF5(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventClear(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventLeft_Right(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData);
static BOOL OnKeyEventUp_Down(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData);

static BOOL RegisterChnImeClass(void);

static const KEYTYPE tabKeyType[] =
{
    {IME_EVT_KB1,    IME_EVT_KB9,       IME_HKEY_INSTANT,
        OnKeyEventKB1_9},

    {IME_EVT_KB0,    IME_EVT_KB0,       IME_HKEY_INSTANT | IME_HKEY_LONG,
        OnKeyEventKB0},

    {IME_EVT_F3,     IME_EVT_F3,        IME_HKEY_MULTITAP | IME_HKEY_LONG,
        OnKeyEventF3_F4},

    {IME_EVT_F4,     IME_EVT_F4,        IME_HKEY_SHORT | IME_HKEY_LONG,
        OnKeyEventF3_F4},

    {IME_EVT_F5,     IME_EVT_F5,        IME_HKEY_INSTANT,
        OnKeyEventF5},

    {IME_EVT_CLEAR,  IME_EVT_CLEAR,     IME_HKEY_REPEATING,
        OnKeyEventClear},

    {IME_EVT_PAGELEFT, IME_EVT_PAGERIGHT, IME_HKEY_REPEATING,
        OnKeyEventLeft_Right},

    {IME_EVT_PAGEUP, IME_EVT_PAGEDOWN,  IME_HKEY_REPEATING,
        OnKeyEventUp_Down}
};

/*********************************************************************\
* Function	   ZiChnImeWinMain
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

DWORD ZiChnImeWinMain(PVOID hThis, INT32 nCode, WPARAM nOne, LPARAM nTwo)
{
    switch (LOWORD(nCode))
    {
    case IME_OPA_INIT:
        return (DWORD)ZiChnImeOpaInit();

    case IME_OPA_CREATE:
        return (DWORD)ZiChnImeOpaCreate((PPZICHNIME)hThis, HIWORD(nCode));

    case IME_OPA_SHOW:
        return (DWORD)ZiChnImeOpaShow((PZICHNIME)hThis, TRUE);

    case IME_OPA_HIDE:
        return (DWORD)ZiChnImeOpaShow((PZICHNIME)hThis, FALSE);

    case IME_OPA_EVENT:
        return (DWORD)ZiChnImeOpaEvent((PZICHNIME)hThis, nOne, nTwo);

    case IME_OPA_DESTROY:
        return (DWORD)ZiChnImeOpaDestroy((PZICHNIME)hThis);

    case IME_OPA_COMMAND:
        return (DWORD)ZiChnImeOpaCommand((PZICHNIME)hThis, nOne, nTwo);
    }

    return (DWORD)FALSE;
}


/**********************************************************************
 * Function     ZiChnImeOpaInit
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL ZiChnImeOpaInit(void)
{
    return RegisterChnImeClass();
}

/*********************************************************************\
* Function	   ZiChnImeOpaCreate
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL ZiChnImeOpaCreate(PPZICHNIME ppChn, WORD wType)
{
    PZICHNIME pChn = NULL;

    if (ppChn == NULL)
    {
        return FALSE;
    }

    pChn = (PZICHNIME)malloc(sizeof(ZICHNIME));
    if (pChn == NULL)
    {
        return FALSE;
    }
    memset(pChn, 0, sizeof(ZICHNIME));

    DefImeProc((PVOID)&pChn, MAKELONG(IME_OPA_CREATE, wType), 0, 0);

    pChn->pZiParam = (PZICHNPARAM)malloc(sizeof(ZICHNPARAM));
    memset((void*)pChn->pZiParam, 0, sizeof(ZICHNPARAM));

    pChn->pZiParam->pGlobal = ImcGetZiGlobal();
    ImeLoadProc((int)wType, &pChn->pfnImeProc);

    pChn->hWnd = CreateWindow(
        "CHNIME",
        "",
        WS_POPUP,
        CX_GAP,
        Y_IMEWND(0),
        CX_IMEWND,
        CY_IMEWND,
        NULL,
        NULL,
        NULL,
        (PVOID)pChn);

    *ppChn = pChn;

    return TRUE;
}

/*********************************************************************\
* Function	   ZiChnImeOpaShow
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL ZiChnImeOpaShow(PZICHNIME pChn, BOOL bShow)
{
    if (pChn == NULL)
    {
        return FALSE;
    }

    if (bShow)
    {
        pChn->fdwStatus |= ZICHNIME_STATUS_SHOW;
        pChn->hwndEdit = GetFocus();
        pChn->pfnImeProc(IMEP_INITZIGETPARAM, 0, (LPARAM)(pChn->pZiParam));
    }
    else
    {
        pChn->fdwStatus &= ~ZICHNIME_STATUS_SHOW;
        ResetZiParam(pChn);
    }

    return TRUE;
}

/*********************************************************************\
* Function	   ZiChnImeOpaEvent
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL ZiChnImeOpaEvent(PZICHNIME pChn, WPARAM wParam, LPARAM lParam)
{
    if (pChn == NULL)
    {
        return FALSE;
    }

    return ZiChnImeKeyEvent(pChn, (INT32)wParam, (DWORD)lParam);
}

/*********************************************************************\
* Function	   ZiChnImeOpaDestroy
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL ZiChnImeOpaDestroy(PZICHNIME pChn)
{
    if (pChn != NULL)
    {
        if (pChn->pZiParam != NULL)
        {
            free(pChn->pZiParam);
        }
        free(pChn);
    }

    return TRUE;
}


/*********************************************************************\
* Function	   ZiChnImeKeyEvent
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL ZiChnImeKeyEvent(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData)
{
    if (!ISKEYUP(dwKeyData))
    {
        return ZiChnImeKeyDown(pChn, nKeyCode, dwKeyData);
    }
    else
    {
        return ZiChnImeKeyUp(pChn, nKeyCode, dwKeyData);
    }
}

/**********************************************************************
 * Function     ZiChnImeOpaCommand
 * Purpose      
 * Params       PZICHNIME pChn, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static DWORD ZiChnImeOpaCommand(PZICHNIME pChn, WPARAM wParam, LPARAM lParam)
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
* Function	   ZiChnImeKeyDown
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL ZiChnImeKeyDown(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData)
{
    int nIndex = 0;

    for (nIndex = 0; nIndex < NUM_KEYTYPE; nIndex++)
    {
        if ((tabKeyType[nIndex].wKeyEvtMin <= (WORD)nKeyCode)
            && ((WORD)nKeyCode <= tabKeyType[nIndex].wKeyEvtMax))
        {
            if (tabKeyType[nIndex].pfnOnKeyEvent == NULL)
                return FALSE;

            return tabKeyType[nIndex].pfnOnKeyEvent(pChn, nKeyCode, dwKeyData);
        }
    }

    return DefImeProc((PVOID)pChn, IME_OPA_EVENT, (WPARAM)nKeyCode,
        (LPARAM)dwKeyData);
}

/*********************************************************************\
* Function	   ZiChnImeKeyUp
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL ZiChnImeKeyUp(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData)
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
 * Function     OnKeyEventKB1_9
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventKB1_9(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData)
{
    int nLength = 0;
    ZI8WCHAR wChar = 0;

    if (!(pChn->fdwStatus & ZICHNIME_STATUS_SEL))
    {
        if (pChn->pfnImeProc(IMEP_INVALIDINPUT, (WPARAM)nKeyCode, 0))
            return TRUE;

        if (pChn->fdwStatus & ZICHNIME_STATUS_CNT)
        {
            pChn->fdwStatus &= ~ZICHNIME_STATUS_CNT;
            InvalidateRect(pChn->hWnd, NULL, TRUE);
        }
        pChn->pZiParam->wcElements[pChn->pZiParam->nElement++]
            = (ZI8WCHAR)pChn->pfnImeProc(IMEP_INPUTELEMENT, (WPARAM)nKeyCode,
            0);
        pChn->pZiParam->GetParam.FirstCandidate = 0;
        pChn->pfnImeProc(IMEP_ENTERASSOCMODE, 0, (LPARAM)pChn->pZiParam);
        nLength = Zi8GetCandidatesCount(&pChn->pZiParam->GetParam,
            pChn->pZiParam->pGlobal);
        if (nLength == 0)
        {
            pChn->pZiParam->nElement--;
            return TRUE;
        }
        pChn->pZiParam->lTotalAssoc = pChn->pfnImeProc(IMEP_GETASSOCIATES, 0,
            (LPARAM)(pChn->pZiParam));
        // Get next candidates page
        pChn->pfnImeProc(IMEP_ENTERCANDIMODE, 0, (LPARAM)(pChn->pZiParam));
        pChn->pZiParam->lTotalChinese = Zi8GetCandidates(&pChn->pZiParam->GetParam,
            pChn->pZiParam->pGlobal);
        WideCharToMultiByte(CP_ACP, 0,
            (LPCWSTR)pChn->pZiParam->GetParam.pCandidates,
            pChn->pZiParam->lTotalChinese, (LPSTR)pChn->pZiParam->wcChinese,
            ZI_MAX_CANDIDATES * sizeof(ZI8WCHAR), NULL, NULL);

        if (pChn->pZiParam->nElement == 1)
        {
            SendMessage(pChn->hWnd, WM_SHOWIMEWND, 0, 0);
            ENTERPREDICTMODE;
        }
        else if (pChn->pZiParam->nElement > 1)
        {
            InvalidateRect(pChn->hWnd, NULL, TRUE);
        }
    }
    else
    {
        if (nKeyCode > ZI_MAX_CANDIDATES
            || pChn->pZiParam->GetParam.FirstCandidate + nKeyCode
            - IME_EVT_KB1 > pChn->pZiParam->lTotalChinese - 1)
        {
            return TRUE;
        }
        pChn->pZiParam->iChineseOffset = (ZI8UCHAR)(nKeyCode - IME_EVT_KB1);
        wChar = pChn->pZiParam->wcChinese[pChn->pZiParam->iChineseOffset];
        SendMessage(pChn->hwndEdit, WM_CHAR, (WPARAM)wChar, 0);
        pChn->pZiParam->nElement = 0;
        if (!(pChn->fdwStatus & ZICHNIME_STATUS_CNT))
        {
            pChn->pZiParam->lTotalChinese = GetContextPage(pChn->pZiParam);
            pChn->fdwStatus |= ZICHNIME_STATUS_CNT;
            pChn->fdwStatus &= ~ZICHNIME_STATUS_SEL;
            SendMessage(pChn->hWnd, WM_SHOWIMEWND, 1, 0);
            SendMessage(pChn->hWnd, IME_MSG_CHANGED_SEL, 0,
                (LPARAM)RES_STR_CANCEL);
        }
        else
        {
            ResetZiParam(pChn);
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

static BOOL OnKeyEventKB0(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData)
{
    DWORD dwCaretPos = 0;

    if (GETEVTSTATE(dwKeyData) == IME_HKEY_LONG)
    {
    }
    else
    {
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

static BOOL OnKeyEventF3_F4(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData)
{
    ResetZiParam(pChn);

    return DefImeProc((PVOID)pChn, IME_OPA_EVENT, (WPARAM)nKeyCode,
        (LPARAM)dwKeyData);
}

/**********************************************************************
 * Function     OnKeyEventF5
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventF5(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData)
{
    WORD wChar = 0;
    int nDiff = 0;
    ZI8WCHAR wcBak[ZI_MAX_CANDIDATES] = {0};

    if ((pChn->fdwStatus & ZICHNIME_STATUS_SEL)
        || (pChn->fdwStatus & ZICHNIME_STATUS_CNT))
    {
        wChar = pChn->pZiParam->wcChinese[pChn->pZiParam->iChineseOffset];
        SendMessage(pChn->hwndEdit, WM_CHAR, (WPARAM)wChar, 0);
        pChn->pZiParam->nElement = 0;
        if (!(pChn->fdwStatus & ZICHNIME_STATUS_CNT))
        {
            pChn->pZiParam->lTotalChinese = GetContextPage(pChn->pZiParam);
            pChn->fdwStatus |= ZICHNIME_STATUS_CNT;
            pChn->fdwStatus &= ~ZICHNIME_STATUS_SEL;
            SendMessage(pChn->hWnd, WM_SHOWIMEWND, 1, 0);
            SendMessage(pChn->hWnd, IME_MSG_CHANGED_SEL, 0,
                (LPARAM)RES_STR_CANCEL);
        }
        else
        {
            ResetZiParam(pChn);
        }
    }
    else
    {
        if (!(pChn->fdwStatus & ZICHNIME_STATUS_SEL)
            && !(pChn->fdwStatus & ZICHNIME_STATUS_CNT)
            && (pChn->pZiParam->nElement == 0))
        {
            return FALSE;
        }
        pChn->fdwStatus |= ZICHNIME_STATUS_SEL;
        pChn->pZiParam->iChineseOffset = 0;
        pChn->pfnImeProc(IMEP_ENTERCANDIMODE, 0, (LPARAM)(pChn->pZiParam));
        pChn->pZiParam->lTotalChinese = Zi8GetCandidatesCount(
            &pChn->pZiParam->GetParam, pChn->pZiParam->pGlobal);

        pChn->pZiParam->GetParam.pCandidates = wcBak;
        pChn->pZiParam->GetParam.FirstCandidate
            = (ZI8WCHAR)pChn->pZiParam->lTotalChinese - 1;
        nDiff = Zi8GetCandidates(&pChn->pZiParam->GetParam,
            pChn->pZiParam->pGlobal);
        if (nDiff > 1)
        {
            while (nDiff >= ZI_MAX_CANDIDATES)
            {
                pChn->pZiParam->GetParam.FirstCandidate
                    += ZI_MAX_CANDIDATES;
                nDiff = Zi8GetCandidates(&pChn->pZiParam->GetParam,
                    pChn->pZiParam->pGlobal);
            }
            pChn->pZiParam->lTotalChinese
                = pChn->pZiParam->GetParam.FirstCandidate + nDiff;
        }
        else if (nDiff <= 0)
        {
            pChn->pZiParam->GetParam.FirstCandidate
                = (ZI8WCHAR)pChn->pZiParam->lTotalChinese;
            while (nDiff <= 0)
            {
                pChn->pZiParam->GetParam.FirstCandidate -= ZI_MAX_CANDIDATES;
                nDiff = Zi8GetCandidates(&pChn->pZiParam->GetParam,
                    pChn->pZiParam->pGlobal);
            }
            pChn->pZiParam->lTotalChinese
                = pChn->pZiParam->GetParam.FirstCandidate + nDiff;
        }
        pChn->pZiParam->GetParam.FirstCandidate = 0;
        pChn->pZiParam->GetParam.pCandidates = pChn->pZiParam->wcCandidates;
        InvalidateRect(pChn->hWnd, NULL, TRUE);
        SendMessage(pChn->hWnd, IME_MSG_CHANGED_SEL, 0, (LPARAM)RES_STR_CANCEL);
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

static BOOL OnKeyEventClear(PZICHNIME pChn, INT32 nKeyCode, DWORD dwKeyData)
{
    if ((pChn->fdwStatus & ZICHNIME_STATUS_SEL)
        || (pChn->fdwStatus & ZICHNIME_STATUS_CNT))
    {
        ResetZiParam(pChn);
    }
    else
    {
        if (pChn->pZiParam->nElement == 0)
            return FALSE;

        pChn->pfnImeProc(IMEP_ENTERASSOCMODE, 0, (LPARAM)(pChn->pZiParam));
        pChn->pZiParam->nElement--;
        if (pChn->pZiParam->nElement > 0)
        {
            pChn->pZiParam->GetParam.FirstCandidate = 0;
            pChn->pZiParam->lTotalAssoc = pChn->pfnImeProc(IMEP_GETASSOCIATES,
                0, (LPARAM)(pChn->pZiParam));
            // Get next candidates page
            pChn->pfnImeProc(IMEP_ENTERCANDIMODE, 0, (LPARAM)(pChn->pZiParam));
            pChn->pZiParam->lTotalChinese = Zi8GetCandidates(
                &pChn->pZiParam->GetParam, pChn->pZiParam->pGlobal);
            WideCharToMultiByte(CP_ACP, 0,
                (LPCWSTR)pChn->pZiParam->GetParam.pCandidates,
                pChn->pZiParam->lTotalChinese, (LPSTR)pChn->pZiParam->wcChinese,
                ZI_MAX_CANDIDATES * sizeof(ZI8WCHAR), NULL, NULL);
            pChn->pZiParam->iChineseOffset = 0;
            InvalidateRect(pChn->hWnd, NULL, TRUE);
        }
        else
        {
            ResetZiParam(pChn);
        }
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

static BOOL OnKeyEventLeft_Right(PZICHNIME pChn, INT32 nKeyCode,
                                 DWORD dwKeyData)
{
    int nMaxInPage = 0;

    if (!(pChn->fdwStatus & ZICHNIME_STATUS_SEL)
        && !(pChn->fdwStatus & ZICHNIME_STATUS_CNT))
    {
        if (pChn->pZiParam->nElement == 0)
            return FALSE;

        if (pChn->pZiParam->lTotalAssoc <= 1)
            return TRUE;

        if (nKeyCode == IME_EVT_PAGELEFT)
        {
            if (pChn->pZiParam->iAssocOffset > 0)
            {
                pChn->pZiParam->iAssocOffset--;
            }
            else
            {
                pChn->pZiParam->iAssocOffset = pChn->pZiParam->lTotalAssoc - 1;
            }
        }
        else
        {
            if (pChn->pZiParam->iAssocOffset < pChn->pZiParam->lTotalAssoc - 1)
            {
                pChn->pZiParam->iAssocOffset++;
            }
            else
            {
                pChn->pZiParam->iAssocOffset = 0;
            }
        }
        pChn->pZiParam->GetParam.FirstCandidate = 0;
        // Get next candidates page
        pChn->pfnImeProc(IMEP_ENTERCANDIMODE, 0, (LPARAM)(pChn->pZiParam));
        pChn->pZiParam->lTotalChinese = Zi8GetCandidates(
            &pChn->pZiParam->GetParam, pChn->pZiParam->pGlobal);
        WideCharToMultiByte(CP_ACP, 0,
            (LPCWSTR)pChn->pZiParam->GetParam.pCandidates,
            pChn->pZiParam->lTotalChinese, (LPSTR)pChn->pZiParam->wcChinese,
            ZI_MAX_CANDIDATES * sizeof(ZI8WCHAR), NULL, NULL);
    }
    else
    {
        nMaxInPage = pChn->pZiParam->lTotalChinese
            - pChn->pZiParam->GetParam.FirstCandidate;
        nMaxInPage = min(nMaxInPage, ZI_MAX_CANDIDATES);

        if (nMaxInPage <= 1)
            return TRUE;

        if (nKeyCode == IME_EVT_PAGELEFT)
        {
            if (pChn->pZiParam->iChineseOffset > 0)
            {
                pChn->pZiParam->iChineseOffset--;
            }
            else
            {
                pChn->pZiParam->iChineseOffset = nMaxInPage - 1;
            }
        }
        else
        {
            if (pChn->pZiParam->iChineseOffset < nMaxInPage - 1)
            {
                pChn->pZiParam->iChineseOffset++;
            }
            else
            {
                pChn->pZiParam->iChineseOffset = 0;
            }
        }
        if (!(pChn->fdwStatus & ZICHNIME_STATUS_SEL))
        {
            pChn->fdwStatus |= ZICHNIME_STATUS_SEL;
        }
    }
    InvalidateRect(pChn->hWnd, NULL, TRUE);

    return TRUE;
}

/**********************************************************************
 * Function     OnKeyEventUp_Down
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnKeyEventUp_Down(PZICHNIME pChn, INT32 nKeyCode,
                              DWORD dwKeyData)
{
    int nCand = 0;

    if (pChn->fdwStatus & ZICHNIME_STATUS_SEL)
    {
        if (pChn->pZiParam->lTotalChinese <= ZI_MAX_CANDIDATES)
            return TRUE;

        if (nKeyCode == IME_EVT_PAGEDOWN)
        {
            if (pChn->pZiParam->GetParam.FirstCandidate + ZI_MAX_CANDIDATES
                < pChn->pZiParam->lTotalChinese)
            {
                pChn->pZiParam->GetParam.FirstCandidate += ZI_MAX_CANDIDATES;
            }
            else
            {
                pChn->pZiParam->GetParam.FirstCandidate = 0;
            }
        }
        else
        {
            if (pChn->pZiParam->GetParam.FirstCandidate > 0)
            {
                pChn->pZiParam->GetParam.FirstCandidate -= ZI_MAX_CANDIDATES;
            }
            else
            {
                nCand = pChn->pZiParam->lTotalChinese % ZI_MAX_CANDIDATES;
                nCand = (nCand == 0) ? ZI_MAX_CANDIDATES : nCand;
                pChn->pZiParam->GetParam.FirstCandidate
                    = pChn->pZiParam->lTotalChinese - nCand;
            }
        }

        nCand = Zi8GetCandidates(&pChn->pZiParam->GetParam,
            pChn->pZiParam->pGlobal);
        WideCharToMultiByte(CP_ACP, 0,
            (LPCWSTR)pChn->pZiParam->GetParam.pCandidates, nCand,
            (LPSTR)pChn->pZiParam->wcChinese,
            ZI_MAX_CANDIDATES * sizeof(ZI8WCHAR), NULL, NULL);

        if (pChn->pZiParam->iChineseOffset > nCand - 1)
        {
            pChn->pZiParam->iChineseOffset = nCand - 1;
        }
        InvalidateRect(pChn->hWnd, NULL, TRUE);
    }
    else
    {
        if (pChn->fdwStatus & ZICHNIME_STATUS_CNT)
        {
            ResetZiParam(pChn);
            return FALSE;
        }
        else
        {
            if (pChn->pZiParam->nElement == 0)
                return FALSE;
        }
    }

    return TRUE;
}

/**********************************************************************
 * Function     GetContextPage
 * Purpose      
 * Params       PZICNSTKPARAM pZiParam
 * Return       
 * Remarks      
 **********************************************************************/

static int GetContextPage(PZICHNPARAM pZiParam)
{
    int nCand = 0;
    PZI8WCHAR pwcCandi = NULL;

    pwcCandi = &pZiParam->wcCandidates[pZiParam->iChineseOffset];

    pZiParam->GetParam.GetOptions = ZI8_GETOPTION_DEFAULT;
    pZiParam->GetParam.ElementCount = 0;
    pZiParam->GetParam.pCurrentWord = pwcCandi;
    pZiParam->GetParam.WordCharacterCount = 1;
    pZiParam->GetParam.pCandidates = pZiParam->wcCandidates;
    pZiParam->GetParam.FirstCandidate = 0;

    nCand = Zi8GetCandidates(&pZiParam->GetParam, pZiParam->pGlobal);

    WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pZiParam->GetParam.pCandidates,
        nCand, (LPSTR)pZiParam->wcChinese,
        ZI_MAX_CANDIDATES * sizeof(ZI8WCHAR), NULL, NULL);

    pZiParam->iChineseOffset = 0;

    return nCand;
}

/**********************************************************************
 * Function     ResetZiParam
 * Purpose      
 * Params       pZiParam
 * Return       
 * Remarks      
 **********************************************************************/

static void ResetZiParam(PZICHNIME pChn)
{
    pChn->pZiParam->GetParam.ElementCount = 0;
    pChn->pZiParam->GetParam.FirstCandidate = 0;
    pChn->pZiParam->lTotalAssoc = 0;
    pChn->pZiParam->lTotalChinese = 0;
    pChn->pZiParam->iAssocOffset = 0;
    pChn->pZiParam->iChineseOffset = 0;
    pChn->pZiParam->nElement = 0;
    pChn->fdwStatus &= ~ZICHNIME_STATUS_SEL;
    pChn->fdwStatus &= ~ZICHNIME_STATUS_CNT;
    if (pChn->fdwStatus & ZICHNIME_STATUS_PRDT)
    {
        LEAVEPREDICTMODE;
        SendMessage(pChn->hWnd, WM_SHOWIMEWND, 2, 0);
    }
}

// UI

#define RES_FOCUSBK              "ROM:ime/symfocus.bmp"
#define RES_WNDBKGND             "ROM:ime/imewnd.bmp"

#define MAX_LEN_SOFTKEY  32

#define CR_WHITE    RGB(255, 255, 255)
#define EXIT_TEXT   ((pData->szRightKey[0] != '\0') ? pData->szRightKey : RES_STR_CANCEL)

typedef struct tagCHNWNDDATA
{
    HDC      hdcMem;
    HBITMAP  hbmpMem;
    HBITMAP  hbmpWnd;
    HBITMAP  hbmpFocus;
    int      nPos;
    POINT    ptCaret;
    char     szRightKey[MAX_LEN_SOFTKEY];
    char     szMidKey[MAX_LEN_SOFTKEY];
}
CHNWNDDATA, *PCHNWNDDATA;

static LRESULT CALLBACK ChnImeWndProc(HWND hWnd, UINT uMsg,
                                      WPARAM wParam, LPARAM lParam);
static void OnCreate(HWND hWnd, LPCREATESTRUCT lpcs);
static void OnPaint(HWND hWnd);
static void OnDestroy(HWND hWnd);
static void OnShowImeWnd(HWND hWnd, int nShow);
static void OnImeChangingSel(HWND hWnd);
static void OnImeChangedSel(HWND hWnd, char* pszText);

/*********************************************************************\
* Function	   RegisterChnImeClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static BOOL RegisterChnImeClass(void)
{
    WNDCLASS wc;
    
    wc.style         = CS_NOFOCUS;
    wc.lpfnWndProc   = ChnImeWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(CHNWNDDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);//NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "CHNIME";

    return RegisterClass(&wc);
}

/*********************************************************************\
* Function	   ChnImeWndProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static LRESULT CALLBACK ChnImeWndProc(HWND hWnd, UINT uMsg,
                                      WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        OnCreate(hWnd, (LPCREATESTRUCT)lParam);
        return 0;

//    case WM_MOUSEACTIVATE:
//        return MA_NOACTIVATE;

    case WM_ACTIVATE:
        return 0;

    case WM_ERASEBKGND:
        return 0;

    case WM_PAINT:
        OnPaint(hWnd);
        return 0;

    case WM_SHOWIMEWND:
        OnShowImeWnd(hWnd, (int)wParam);
        return 0;

    case WM_DESTROY:
        OnDestroy(hWnd);
        return 0;

    case IME_MSG_CHANGING_SEL:
        OnImeChangingSel(hWnd);
        return 0;

    case IME_MSG_CHANGED_SEL:
        OnImeChangedSel(hWnd, (char*)lParam);
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

static void OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PCHNWNDDATA     pData = NULL;

    pData = (PCHNWNDDATA)GetUserData(hWnd);

    memset((void*)pData, 0, sizeof(CHNWNDDATA));
    pData->hdcMem = CreateCompatibleDC(NULL);
    pData->hbmpMem = CreateCompatibleBitmap(pData->hdcMem, CX_IMEWND,
        CY_IMEWND);
    SelectObject(pData->hdcMem, pData->hbmpMem);
    pData->hbmpWnd = LoadImage(NULL, RES_WNDBKGND, IMAGE_BITMAP, 0, 0,
        LR_LOADFROMFILE);
    pData->hbmpFocus = LoadImage(NULL, RES_FOCUSBK, IMAGE_BITMAP, 0, 0,
        LR_LOADFROMFILE);
}

static BOOL GetFontHandle(HFONT *hFont, int nType)
{
    static HFONT hfontLarge = NULL, hfontSmall = NULL;
    LOGFONT lf;

    memset((void*)&lf, 0, sizeof(LOGFONT));

    if (nType == SMALL_FONT)
    {
        if (hfontSmall == NULL)
        {
            lf.lfHeight = 12;
            lf.lfCharSet = DEFAULT_CHARSET;
            strcpy(lf.lfFaceName, "ËÎÌו");
            hfontSmall = CreateFontIndirect(&lf);
        }
 
        *hFont = hfontSmall;
    }
    else
    {
        if (hfontLarge == NULL)
        {
            lf.lfHeight = 16;
            lf.lfCharSet = 4;
            strcpy(lf.lfFaceName, "ËÎÌו-18030");
            hfontLarge = CreateFontIndirect(&lf);
        }

        *hFont = hfontLarge;
    }

    return TRUE;
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
    PCHNWNDDATA pData = NULL;
    PZICHNIME pImeData = NULL;
    HDC hdc = NULL;
    PAINTSTRUCT ps;
    RECT rcClient, rcFocus, arcAssoc[16], rcCandi;
    SIZE asizeAssoc[16];
    HFONT hFont = NULL;
    int i = 0, nMode = 0, cxArea = 0, xAssocPage = 0, cxAssocPage = 0,
        xCandi = 0, cxCandi = 0, iAssocOffset = 0, iCandiOffset = 0,
        nFit = 0, iStart = 0;
    char *pszAssoc = NULL, chIndex = 0;
    BOOL bFocus = FALSE;
    COLORREF crText, crBk = 0;
    BITMAP bmp;

    pData = (PCHNWNDDATA)GetUserData(hWnd);
    pImeData = (PZICHNIME)GetWindowLong(hWnd, GWL_USERDATA);

    hdc = BeginPaint(hWnd, &ps);

    GetClientRect(hWnd, &rcClient);

    if (pData->hbmpWnd != NULL)
    {
        GetObject(pData->hbmpWnd, sizeof(BITMAP), (void*)&bmp);
        StretchBlt(pData->hdcMem, 0, 0, rcClient.right, rcClient.bottom,
            (HDC)pData->hbmpWnd, 0, 0, bmp.bmWidth, bmp.bmHeight, ROP_SRC);
    }
    else
    {
        ClearRect(pData->hdcMem, &rcClient, GetSysColor(COLOR_MENU));
    }

    cxArea = rcClient.right - 2 * CX_MARGIN;

    GetObject(pData->hbmpFocus, sizeof(BITMAP), (void*)&bmp);

    GetFontHandle(&hFont, SMALL_FONT);
    hFont = SelectObject(pData->hdcMem, hFont);

    cxCandi = cxArea / ZI_MAX_CANDIDATES;
    xCandi = (rcClient.right - ZI_MAX_CANDIDATES * cxCandi) / 2;

    if (!(pImeData->fdwStatus & ZICHNIME_STATUS_SEL))
    {
        nMode = SetBkMode(pData->hdcMem, TRANSPARENT);

        if (!(pImeData->fdwStatus & ZICHNIME_STATUS_CNT))
        {
            for (i = 0, pszAssoc = pImeData->pZiParam->szAssoc;
                 i < pImeData->pZiParam->lTotalAssoc;
                 i++, pszAssoc += strlen(pszAssoc) + 1)
            {
                GetTextExtentExPoint(pData->hdcMem, pszAssoc, -1, cxArea, &nFit, NULL,
                    &asizeAssoc[i]);
                if (cxAssocPage + CX_ASSOCSPACE + asizeAssoc[i].cx >= cxArea)
                    break;
                if (i > 0)
                    cxAssocPage += CX_ASSOCSPACE;
                cxAssocPage += asizeAssoc[i].cx;
            }
            if (i > 0)
            {
                pImeData->pZiParam->lTotalAssoc = i;
                iStart = 0;
                xAssocPage = (rcClient.right - cxAssocPage) / 2;
                SetRect(arcAssoc, xAssocPage, CY_MARGIN, xAssocPage + asizeAssoc[0].cx,
                    CY_MARGIN + CY_ASSOCAREA);
            }
            else
            {
                iStart = strlen(pszAssoc) - nFit;
                SetRect(arcAssoc, CX_MARGIN, CY_MARGIN, CX_MARGIN + cxArea,
                    CY_MARGIN + CY_ASSOCAREA);
            }

            for (i = 0, pszAssoc = &pImeData->pZiParam->szAssoc[iStart];
                 i < pImeData->pZiParam->lTotalAssoc;
                 i++, pszAssoc += strlen(pszAssoc) + 1)
            {
                bFocus = (pImeData->pZiParam->iAssocOffset == i);//EqualRect(&arcAssoc[i], &pData->rcFocus);
                if (bFocus)
                {
                    CopyRect(&rcFocus, &arcAssoc[i]);
                    InflateRect(&rcFocus, 1, 1);
                    StretchBlt(pData->hdcMem, rcFocus.left, rcFocus.top,
                        rcFocus.right - rcFocus.left,
                        rcFocus.bottom - rcFocus.top,
                        (HDC)pData->hbmpFocus, 0, 0, bmp.bmWidth, bmp.bmHeight, ROP_SRC);

                    crText = SetTextColor(pData->hdcMem, CR_WHITE);
                }
                DrawText(pData->hdcMem, pszAssoc, -1, &arcAssoc[i], DT_CENTER | DT_VCENTER);
                if (bFocus)
                    SetTextColor(pData->hdcMem, crText);

                arcAssoc[i + 1].left = arcAssoc[i].right + CX_ASSOCSPACE;
                arcAssoc[i + 1].top = arcAssoc[i].top;
                arcAssoc[i + 1].right = arcAssoc[i + 1].left + asizeAssoc[i + 1].cx;
                arcAssoc[i + 1].bottom = arcAssoc[i].bottom;
            }
        }
    }
    else
    {
        SetRect(&rcCandi, xCandi, CY_MARGIN, CX_MARGIN + cxCandi,
            CY_MARGIN + CY_ASSOCAREA);
        crText = SetTextColor(pData->hdcMem, CR_WHITE);
        crBk = SetBkColor(pData->hdcMem, RGB(0, 128, 0));
        for (i = 0; i < ZI_MAX_CANDIDATES; i++)
        {
            if (pImeData->pZiParam->GetParam.FirstCandidate + i
                >= pImeData->pZiParam->lTotalChinese)
                break;
            chIndex = (char)(i + '1');
            DrawText(pData->hdcMem, (PCSTR)&chIndex, 1, &rcCandi, DT_CENTER | DT_VCENTER);
            OffsetRect(&rcCandi, cxCandi, 0);
        }
        SetBkColor(pData->hdcMem, crBk);
        SetTextColor(pData->hdcMem, crText);

        nMode = SetBkMode(pData->hdcMem, TRANSPARENT);
    }
    SelectObject(pData->hdcMem, hFont);

    bFocus = FALSE;
    SetRect(&rcCandi, xCandi, CY_MARGIN + CY_ASSOCAREA, CX_MARGIN + cxCandi,
        CY_MARGIN + CY_ASSOCAREA + CY_CANDIAREA);
    for (i = 0; i < ZI_MAX_CANDIDATES; i++)
    {
        if (pImeData->pZiParam->GetParam.FirstCandidate + i
            >= pImeData->pZiParam->lTotalChinese)
            break;
        if ((pImeData->fdwStatus & ZICHNIME_STATUS_SEL)
            || (pImeData->fdwStatus & ZICHNIME_STATUS_CNT))
        {
            bFocus = (pImeData->pZiParam->iChineseOffset == i);
        }
        if (bFocus)
        {
            CopyRect(&rcFocus, &rcCandi);
            StretchBlt(pData->hdcMem, rcFocus.left, rcFocus.top,
                rcFocus.right - rcFocus.left,
                rcFocus.bottom - rcFocus.top,
                (HDC)pData->hbmpFocus, 0, 0, bmp.bmWidth, bmp.bmHeight,
                ROP_SRC);
            crText = SetTextColor(pData->hdcMem, CR_WHITE);
        }
        DrawText(pData->hdcMem, (PCSTR)&pImeData->pZiParam->wcChinese[i], sizeof(WORD),
            &rcCandi, DT_CENTER | DT_VCENTER);
        if (bFocus)
            SetTextColor(pData->hdcMem, crText);
        OffsetRect(&rcCandi, cxCandi, 0);
    }

    SetBkMode(pData->hdcMem, nMode);

    BitBlt(hdc, 0, 0, rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top, pData->hdcMem, 0, 0, ROP_SRC);

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
    PCHNWNDDATA pData = NULL;

    pData = (PCHNWNDDATA)GetUserData(hWnd);
    DeleteObject(pData->hbmpMem);
    DeleteObject(pData->hdcMem);
    DeleteObject(pData->hbmpFocus);
    DeleteObject(pData->hbmpWnd);
}

/**********************************************************************
 * Function     OnShowImeWnd
 * Purpose      
 * Params       HWND hWnd, BOOL bShow
 * Return       
 * Remarks      
 **********************************************************************/

static void OnShowImeWnd(HWND hWnd, int nShow)
{
    PCHNWNDDATA pData = NULL;
    PZICHNIME pImeData = NULL;
    RECT rcCaret, rcWnd, rc;
    POINT ptNewCaret;
    BOOL bOverlap = FALSE;

    pData = (PCHNWNDDATA)GetUserData(hWnd);
    pImeData = (PZICHNIME)GetWindowLong(hWnd, GWL_USERDATA);

    switch (nShow)
    {
    case 0:
        GetCaretPos(&ptNewCaret);
        ClientToScreen(pImeData->hwndEdit, &ptNewCaret);
        SetRect(&rcCaret, ptNewCaret.x, ptNewCaret.y,
            ptNewCaret.x + GetSystemMetrics(SM_CXCURSOR),
            ptNewCaret.y + GetSystemMetrics(SM_CYCURSOR));
        SetRect(&rcWnd, CX_GAP, Y_IMEWND(0), CX_GAP + CX_IMEWND,
            Y_IMEWND(0) + CY_IMEWND);
        bOverlap = IntersectRect(&rc, &rcCaret, &rcWnd);
        if ((bOverlap && (pData->nPos == 0)) || (!bOverlap && (pData->nPos == 1)))
        {
            pData->nPos = !pData->nPos;
            MoveWindow(hWnd, CX_GAP, Y_IMEWND(pData->nPos), CX_IMEWND,
                CY_IMEWND, FALSE);
        }
        ShowWindow(hWnd, SW_SHOW);
        break;

    case 1:
        GetCaretPos(&ptNewCaret);
        ClientToScreen(pImeData->hwndEdit, &ptNewCaret);
        SetRect(&rcCaret, ptNewCaret.x, ptNewCaret.y,
            ptNewCaret.x + GetSystemMetrics(SM_CXCURSOR),
            ptNewCaret.y + GetSystemMetrics(SM_CYCURSOR));
        GetWindowRectEx(hWnd, &rcWnd, W_WINDOW, XY_SCREEN);
        if ((ptNewCaret.y != pData->ptCaret.y)
            && (IntersectRect(&rc, &rcCaret, &rcWnd)))
        {
            pData->nPos = !pData->nPos;
            MoveWindow(hWnd, CX_GAP, Y_IMEWND(pData->nPos), CX_IMEWND,
                CY_IMEWND, TRUE);
        }
        else
        {
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

    default:
        ShowWindow(hWnd, SW_HIDE);
        break;
    }
}

/**********************************************************************
 * Function     OnImeChangingSel
 * Purpose      
 * Params       HWND hWnd
 * Return       
 * Remarks      
 **********************************************************************/

static void OnImeChangingSel(HWND hWnd)
{
    PZICHNIME pImeData = NULL;
    PCHNWNDDATA pData = NULL;
    HWND hwndFrame = NULL;

    pImeData = (PZICHNIME)GetWindowLong(hWnd, GWL_USERDATA);
    pData = (PCHNWNDDATA)GetUserData(hWnd);

    hwndFrame = GetParent(pImeData->hwndEdit);
    while (GetParent(hwndFrame) != NULL)
    {
        hwndFrame = GetParent(hwndFrame);
    }
    SendMessage(hwndFrame, PWM_GETBUTTONTEXT, 0,
        (LPARAM)pData->szRightKey);
    SendMessage(hwndFrame, PWM_GETBUTTONTEXT, 2, (LPARAM)pData->szMidKey);
    SendMessage(hwndFrame, WM_SETRBTNTEXT, 0, (LPARAM)RES_STR_CLEAR);
    SendMessage(hwndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
}

/**********************************************************************
 * Function     OnImeChangedSel
 * Purpose      
 * Params       HWND hWnd
 * Return       
 * Remarks      
 **********************************************************************/

static void OnImeChangedSel(HWND hWnd, char* pszText)
{
    PZICHNIME pImeData = NULL;
    PCHNWNDDATA pData = NULL;
    HWND hwndFrame = NULL;
    DWORD dwCaretPos = 0;

    pImeData = (PZICHNIME)GetWindowLong(hWnd, GWL_USERDATA);
    pData = (PCHNWNDDATA)GetUserData(hWnd);

    hwndFrame = GetParent(pImeData->hwndEdit);
    while (GetParent(hwndFrame) != NULL)
    {
        hwndFrame = GetParent(hwndFrame);
    }
    if (pszText != NULL)
    {
        SendMessage(hwndFrame, WM_SETRBTNTEXT, 0, (LPARAM)pszText);
    }
    else
    {
        dwCaretPos = SendMessage(pImeData->hwndEdit, EM_GETSEL, 0, 0);
        if (dwCaretPos == 0)
        {
            SendMessage(hwndFrame, WM_SETRBTNTEXT, 0, (LPARAM)EXIT_TEXT);
        }
        else
        {
            SendMessage(hwndFrame, WM_SETRBTNTEXT, 0, (LPARAM)RES_STR_CLEAR);
        }
        SendMessage(hwndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)pData->szMidKey);
    }
}
