/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 *    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
 *                       All Rights Reserved
 *
 * Module   :  zistk.c
 *
 * Purpose  :  
 *
\**************************************************************************/

#include "window.h"
#include "imm.h"
#include "ziime.h"

#define ENTERASSOCMODE(p) (p)->GetParam.ElementCount = (p)->nElement

#define ENTERCANDIMODE(p) (p)->GetParam.GetOptions = ZI8_GETOPTION_NOCOMPONENTS; \
                          (p)->GetParam.ElementCount = (p)->nElement;            \
                          (p)->GetParam.WordCharacterCount = 0;

static const ZI8WCHAR g_wcStroke[] =
{
    ZI8_CODE_OVER,           // 0xACA5
    ZI8_CODE_DOWN,           // 0xACA2
    ZI8_CODE_LEFT,           // 0xACA8
    ZI8_CODE_DOT,            // 0xACA3
    ZI8_CODE_OVER_DOWN,      // 0xACA6
    ZI8_CODE_CURVED_HOOK,    // 0xACA4
    ZI8_CODE_DOWN_OVER,      // 0xACA7
    ZI8_CODE_WILDCARD,       // 0xACA1
    ZI8_CODE_OVER_DOWN_OVER, // 0xACA9
};

static int GetAssociates(PZICHNPARAM pZiParam);
static void InitZiGetParam(PZICHNPARAM pZiParam);

LRESULT CALLBACK ZiStkImeProc(UINT uProcess, WPARAM wParam, LPARAM lParam)
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
        lRes = FALSE;
        break;

    case IMEP_INPUTELEMENT:
        lRes = (LRESULT)g_wcStroke[wParam - IME_EVT_KB1];
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
    int nLength = 0;

    ENTERASSOCMODE(pZiParam);
    nLength = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pZiParam->wcElements,
        pZiParam->nElement, (LPSTR)pZiParam->szAssoc, ZI_ELEM_BUF_SIZE, NULL,
        NULL);
    pZiParam->szAssoc[nLength] = '\0';

    return 1;
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
    pZiParam->GetParam.GetMode = ZI8_GETMODE_STROKES;
    pZiParam->GetParam.SubLanguage = ZI8_SUBLANG_ZH;
    pZiParam->GetParam.Context = ZI8_GETCONTEXT_DEFAULT;
    pZiParam->GetParam.GetOptions = ZI8_GETOPTION_NOCOMPONENTS;
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
