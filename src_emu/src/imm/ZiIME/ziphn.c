/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 *    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
 *                       All Rights Reserved
 *
 * Module   :  ziphn.c
 *
 * Purpose  :  
 *
\**************************************************************************/

#include "window.h"
#include "imm.h"
#include "ziime.h"

#define ENTERASSOCMODE(p)  (p)->GetParam.GetMode = ZI8_GETMODE_1KEYPRESS_PINYIN;  \
                           (p)->GetParam.GetOptions = ZI8_GETOPTION_GET_SPELLING; \
                           (p)->GetParam.pElements = (p)->wcElements;             \
                           (p)->GetParam.ElementCount = (p)->nElement;            \
                           (p)->GetParam.pCandidates = (p)->wcCandiCode

#define ENTERCANDIMODE(p)                                   \
{                                                           \
    int i = 0;                                              \
    PZI8WCHAR pwcAssoc = NULL;                              \
    pwcAssoc = (p)->wcCandiCode;                            \
    for (i = 0; i < (p)->iAssocOffset; pwcAssoc++)          \
    {                                                       \
        if (*pwcAssoc == 0)                                 \
            i++;                                            \
    }                                                       \
    for (i = 0; pwcAssoc[i] != 0; i++);                     \
    (p)->GetParam.GetMode = ZI8_GETMODE_PINYIN;             \
    (p)->GetParam.GetOptions = ZI8_GETOPTION_DEFAULT;       \
    (p)->GetParam.pElements = pwcAssoc;                     \
    (p)->GetParam.ElementCount = (ZI8UCHAR)i;               \
    (p)->GetParam.pCandidates = (p)->wcCandidates;          \
    (p)->GetParam.WordCharacterCount = 0;                   \
}

static int GetAssociates(PZICHNPARAM pZiParam);
static void InitZiGetParam(PZICHNPARAM pZiParam);

LRESULT CALLBACK ZiPhnImeProc(UINT uProcess, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;

    switch (uProcess)
    {
    case IMEP_ENTERASSOCMODE:
        ENTERASSOCMODE((PZICHNPARAM)lParam);
        break;

    case IMEP_ENTERCANDIMODE:
        ENTERCANDIMODE((PZICHNPARAM)lParam);
        break;

    case IMEP_INVALIDINPUT:
        lRes = (wParam == IME_EVT_KB1);
        break;

    case IMEP_INPUTELEMENT:
        lRes = ZI8_BASE_LATIN_KEYS + wParam;
        break;

    case IMEP_GETASSOCIATES:
        lRes = GetAssociates((PZICHNPARAM)lParam);
        break;

    case IMEP_INITZIGETPARAM:
        InitZiGetParam((PZICHNPARAM)lParam);
        break;

    default:
        break;
    }

    return lRes;
}


/**********************************************************************
 * Function     GetAssociates
 * Purpose      
 * Params       pZiParam
 * Return       
 * Remarks      
 **********************************************************************/

static int GetAssociates(PZICHNPARAM pZiParam)
{
    int nCand = 0, iWord = 0, iChar = 0;
    PZI8WCHAR pwcSpell = NULL;

    ENTERASSOCMODE(pZiParam);
    nCand = Zi8GetCandidates(&pZiParam->GetParam, pZiParam->pGlobal);

    pwcSpell = pZiParam->GetParam.pCandidates;
    for (iWord = 0, iChar = 0; iWord < nCand; pwcSpell++, iChar++)
    {
        if (*pwcSpell != 0)
        {
            pZiParam->szAssoc[iChar] = *pwcSpell - ZI8_BASE_PINYIN + 'a';
        }
        else
        {
            pZiParam->szAssoc[iChar] = 0;
            iWord++;
        }
    }
    pZiParam->iAssocOffset = 0;

    return nCand;
}

/**********************************************************************
 * Function     SetZiGetParam
 * Purpose      
 * Params       pZiParam
 * Return       
 * Remarks      
 **********************************************************************/

static void InitZiGetParam(PZICHNPARAM pZiParam)
{
    pZiParam->GetParam.Language = ZI8_LANG_ZH;
    pZiParam->GetParam.GetMode = ZI8_GETMODE_1KEYPRESS_PINYIN;
    pZiParam->GetParam.SubLanguage = ZI8_SUBLANG_ZH;
    pZiParam->GetParam.Context = ZI8_GETCONTEXT_DEFAULT;
    pZiParam->GetParam.GetOptions = ZI8_GETOPTION_GET_SPELLING;
    pZiParam->GetParam.pElements = pZiParam->wcElements;
    pZiParam->GetParam.ElementCount = 0;
    pZiParam->GetParam.pCurrentWord = NULL;
    pZiParam->GetParam.WordCharacterCount = 0;
    pZiParam->GetParam.pCandidates = pZiParam->wcCandiCode;
    pZiParam->GetParam.MaxCandidates = ZI_MAX_CANDIDATES;
    pZiParam->GetParam.FirstCandidate = 0;
    pZiParam->GetParam.wordCandidates = 0;
    pZiParam->GetParam.count = 0;
    pZiParam->GetParam.letters = 0;
    pZiParam->GetParam.pScratch = pZiParam->ucScratch;
}

