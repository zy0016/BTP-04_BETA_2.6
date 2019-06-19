/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements scrollbar-controlling functions.
 *            
\**************************************************************************/

#ifndef NOSCROLL

#include "hpwin.h"

#if (PDAAPI)
#include "winpda.h"
#else
#include "defwnd.h"
#endif
/*
**  Function : SetScrollInfo
**  Purpose  :
**      Sets the parameters of a scroll bar, including the minimum and 
**      maximum scrolling positions, the page size, and the position of
**      the scroll box (thumb). The function also redraws the scroll 
**      bar, if requested. 
**  Params   :
**      hWnd        : Identifies a scroll bar control or a window with a 
**                    standard scroll bar, depending on the value of the
**                    nBar parameter. 
**      nBar        : Specifies the type of scroll bar for which to set 
**                    parameters. 
**      pScrollInfo : Points to a SCROLLINFO structure whose fMask member, 
**                    upon entry to the function, specifies the scroll 
**                    bar parameters to set. 
**      bRedraw     : Specifies whether the scroll bar is redrawn to 
**                    reflect the changes to the scroll bar. If this 
**                    parameter is TRUE, the scroll bar is redrawn, 
**                    otherwise, it is not redrawn. 
**  Return   :
**      The return value is the current position of the scroll box. 
**  Remark   :
**      The SetScrollInfo function performs range checking on the values
**      specified by the nPage and nPos members of the SCROLLINFO 
**      structure. The nPage member must specify a value from 0 to 
**      nMax - nMin + 1. The nPos member must specify a value between 
**      nMin and nMax - max(nPage - 1, 0). If either value is beyond its
**      range, the function sets it to a value that is just within the 
**      range. 
*/
int WINAPI SetScrollInfo(HWND hWnd, int nBar, PCSCROLLINFO pScrollInfo,
                         BOOL bRedraw)
{
#if (PDAAPI)
    return PDASetScrollInfo(hWnd, nBar, pScrollInfo, bRedraw);
#else
    // If the parameter is invalid, return FALSE.
    if ((nBar != SB_CTL && nBar != SB_HORZ && nBar != SB_VERT) || 
        !pScrollInfo)
    {
        SetLastError(1);
        return 0;
    }

    if (nBar != SB_CTL)
        return DEFWND_SetScrollInfo(hWnd, nBar, pScrollInfo, bRedraw);

    return SendMessage(hWnd, SBM_SETSCROLLINFO, (WPARAM)bRedraw, 
        (LPARAM)pScrollInfo);
#endif
}

/*
**  Function : GetScrollInfo
**  Purpose  :
**      Retrieves the parameters of a scroll bar, including the minimum 
**      and maximum scrolling positions, the page size, and the position
**      of the scroll box (thumb). 
**  Params   :
**      hWnd        : Identifies a scroll bar control or a window with a
**                    standard scroll bar, depending on the value of the
**                    nBar parameter. 
**      nBar        : Specifies the type of scroll bar for which to 
**                    retrieve parameters.
**      pScrollInfo : Points to a SCROLLINFO structure whose fMask member, 
**                    upon entry to the function, specifies the scroll bar
**                    parameters to retrieve. Before returning, the 
**                    function copies the specified parameters to the 
**                    appropriate members of the structure. 
**  Return  :
**      If the function retrieved any values, returnis nonzero. 
**      If the function does not retrieve any values, return zero. 
**  Remark  :
**      The GetScrollInfo function enables applications to use 32-bit 
**      scroll positions. Although the messages that indicate scroll-bar
**      position, WM_HSCROLL and WM_VSCROLL, provide only 16 bits of 
**      position data, the functions SetScrollInfo and GetScrollInfo 
**      provide 32 bits of scroll-bar position data. Thus, an application
**      can call GetScrollInfo while processing either the WM_HSCROLL or 
**      WM_VSCROLL messages to obtain 32-bit scroll-bar position data. 
*/
BOOL WINAPI GetScrollInfo(HWND hWnd, int nBar, PSCROLLINFO pScrollInfo)
{
#if (PDAAPI)
    return PDAGetScrollInfo(hWnd, nBar, pScrollInfo);
#else
    // If the parameter is invalid, return FALSE.
    if ((nBar != SB_CTL && nBar != SB_HORZ && nBar != SB_VERT) || 
        !pScrollInfo)
    {
        SetLastError(1);
        return FALSE;
    }

    if (nBar != SB_CTL)
        return DEFWND_GetScrollInfo(hWnd, nBar, pScrollInfo);

    return SendMessage(hWnd, SBM_GETSCROLLINFO, (WPARAM)0, 
        (LPARAM)pScrollInfo);
#endif
}

/*
**  Function : EnableScrollBar
**  Purpose  :
**      Enables or disables one or both scroll bar arrows. 
**  Params   :
**      hWnd     : Identifies a window or a scroll bar control, depending 
**                 on the value of the wSBflags parameter. 
**      nSBFlags : Specifies the scroll bar type. This parameter can be 
**                 SB_BOTH, SB_CTL, SB_HORZ or SB_VERT. 
**      nArrow   : Specifies whether the scroll bar arrows are enabled or 
**                 disabled and indicates which arrows are enabled or 
**                 disabled. 
**  Return   :
**      If the arrows are enabled or disabled as specified, return nonzero. 
**      If the arrows are already in the requested state or an error 
**      occurs, return zero. 
*/
BOOL WINAPI EnableScrollBar(HWND hWnd, int nBar, int nArrow)
{
#if (PDAAPI)
        return PDAEnableScrollBar(hWnd, nBar, nArrow);
#else
    // If the parameter is invalid, return FALSE.
    if (nBar != SB_CTL && nBar != SB_HORZ && nBar != SB_VERT && 
        nBar != SB_BOTH)
    {
        SetLastError(1);
        return FALSE;
    }

    if (nBar != SB_CTL)
        return DEFWND_EnableScrollBar(hWnd, nBar, nArrow);

    return SendMessage(hWnd, SBM_ENABLE_ARROWS, (WPARAM)nArrow, 0);
#endif
}

#endif  /* NOSCROLL */
