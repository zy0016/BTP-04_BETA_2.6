/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 *    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
 *                       All Rights Reserved
 *
 * Module   :  ziime.c
 *
 * Purpose  :  
 *
\**************************************************************************/

#include "window.h"
#include "ziime.h"

extern LRESULT CALLBACK ZiPhnImeProc(UINT uProcess, WPARAM wParam, LPARAM lParam);
extern LRESULT CALLBACK ZiStkImeProc(UINT uProcess, WPARAM wParam, LPARAM lParam);

/**********************************************************************
 * Function     
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

BOOL ImeLoadProc(int nType, PFNIMEPROC *ppfnImeProc)
{
    switch (nType)
    {
    case 0x00C1:
        *ppfnImeProc = ZiPhnImeProc;
        break;

    case 0x00C2:
        *ppfnImeProc = ZiStkImeProc;
        break;

    case 0x00C3:
        *ppfnImeProc = ZiStkImeProc;
        break;

    default:
        *ppfnImeProc = ZiPhnImeProc;
        break;
    }

    return TRUE;
}
