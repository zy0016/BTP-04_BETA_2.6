/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements caret support functions.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "windowx.h"

#include "wsobj.h"
#include "wsthread.h"
#include "wscaret.h"
#include "wsgraph.h"
#include "wstimer.h"

static PWINOBJ pCaretWin = NULL;           // The owner window of current caret
static PBMPOBJ pCaretBmp = NULL;           // The bitmap of current caret
static int nCaretWidth;             // The width of current caret
static int nCaretHeight;            // The height of current caret
static int nCaretX;                 // The x-coordinate(client coordinate)
static int nCaretY;                 // The y-coordinate(client coordinate)
static int nCaretShow;              // The show state of current caret

static BOOL bBlinkShow;             // Whether the caret is showed
static UINT uCaretBlinkTime = 500;  // The blink time(in milliseconds)
static UINT uCaretTimerID;          // Caret timer ID

static CARETPROC pCaretProc;

static HDC      hdcMem = NULL;
static BOOL     bUsrClr = FALSE;
static COLORREF crUsrClr = RGB(128, 128, 128);

// Internal function prototypes
static void CALLBACK CaretBlinkProc(HWND hWnd, UINT message, UINT uIDEvent, 
                                    DWORD dwTime);
static void DrawCaret(void);

/*
**  Function : WS_CreateCaret
**  Purpose  :
**      Creates a new shape for the system caret and assigns ownership of 
**      the caret to the specified window. The caret shape can be a line, 
**      a block, or a bitmap. 
*/
void WS_CreateCaret(PWINOBJ pWin, PBMPOBJ pBmp, int nWidth, int nHeight)
{
    ASSERT(pWin != NULL);

    if (uCaretTimerID == 0)
    {
        uCaretTimerID = WS_CreateTimer(g_pRootWin, 1, uCaretBlinkTime, CaretBlinkProc, TRUE);
/*
        LEAVEMONITOR;

        uCaretTimerID = SetSystemTimer(NULL, 0, uCaretBlinkTime, 
            CaretBlinkProc);

        ENTERMONITOR;
*/
    }

    // Hide the caret if the caret is showing
    if (pCaretWin && nCaretShow > 0 && bBlinkShow)
    {
        bBlinkShow = FALSE;
        DrawCaret();        
    }

    pCaretWin = pWin;

    if ((int)pBmp == 1)
    {
        pCaretBmp = NULL;
        nCaretWidth = nWidth;
        nCaretHeight = nHeight;
        bUsrClr = TRUE;
    }
    else if (pBmp != NULL)
    {
        pCaretBmp = pBmp;
        nCaretWidth = pBmp->bitmap.bmWidth;
        nCaretHeight = pBmp->bitmap.bmHeight;
        bUsrClr = FALSE;
    }
    else
    {
        pCaretBmp = NULL;
        nCaretWidth = nWidth;
        nCaretHeight = nHeight;
        bUsrClr = FALSE;
    }

    nCaretX = 0;
    nCaretY = 0;
    nCaretShow = 0;

    // Set the in-use flag of the bitmap
    if (pCaretBmp)
    {
        WOT_SelectObj((PXGDIOBJ)pCaretBmp);
    }
}

/*
**  Function : WS_DestroyCaret
**  Purpose  :
**      Destroys the caret’s current shape, frees the caret from the 
**      window, and removes the caret from the screen. 
*/
void WS_DestroyCaret(void)
{
    // Hide the caret if the caret is showing
    if (pCaretWin && nCaretShow > 0 && bBlinkShow)
    {
        bBlinkShow = FALSE;
        DrawCaret();
    }

    // Clear the in-use flag of the bitmap
    
    if (pCaretBmp)
    {
        WOT_UnselectObj((PXGDIOBJ)pCaretBmp);
    }

    pCaretWin = NULL;
    pCaretBmp = NULL;
    nCaretWidth = 0;
    nCaretHeight = 0;

    nCaretX = 0;
    nCaretY = 0;
    nCaretShow = 0;
    bUsrClr = FALSE;

    if (uCaretTimerID != 0)
    {
        if (WS_DestroyTimer(g_pRootWin, uCaretTimerID, TRUE))
            uCaretTimerID = 0;
    }
    if (hdcMem != NULL)
    {
        DeleteDC(hdcMem);
        hdcMem = NULL;
    }
}

/*
**  Function : WS_SetCaretPos
**  Purpose  :
**      Moves the caret to the specified coordinates.
*/
void WS_SetCaretPos(int x, int y)
{
    if (nCaretX != x || nCaretY != y)
    {
        // Hide the caret if the caret is showing
        if (pCaretWin && nCaretShow > 0 && bBlinkShow)
        {
            bBlinkShow = FALSE;
            DrawCaret();
        }

        if (pCaretWin && pCaretProc)
        {
            RECT rcCaret;

            rcCaret.left = x;
            rcCaret.top = y;
            WND_ClientToScreen(pCaretWin, (int*)&rcCaret.left, 
                (int*)&rcCaret.top);
            rcCaret.right = rcCaret.left + nCaretWidth;
            rcCaret.bottom = rcCaret.top + nCaretHeight;

            pCaretProc(&rcCaret);
        }

        nCaretX = x;
        nCaretY = y;
    }
}

/*
**  Function : WS_GetCaretPos
**  Purpose  :
**      Copies the caret’s position, in client coordinates, to the 
**      specified POINT structure. 
*/
void WS_GetCaretPos(PPOINT pPoint)
{
    ASSERT(pPoint != NULL);

    pPoint->x = nCaretX;
    pPoint->y = nCaretY;
}

/*
**  Function : WS_HideCaret
**  Purpose  :
**      Removes the caret from the screen. Hiding a caret does not destroy
**      its current shape or invalidate the insertion point. 
*/
BOOL WS_HideCaret(PWINOBJ pWin)
{
    if (pWin != pCaretWin)
        return FALSE;

    // Hide the caret if the caret is showing
    if (pCaretWin && nCaretShow && bBlinkShow)
    {
        bBlinkShow = FALSE;
        DrawCaret();
    }
    else
        ASSERT(!bBlinkShow);

    nCaretShow--;

    if ((nCaretShow == 0) && (hdcMem != NULL))
    {
        DeleteDC(hdcMem);
        hdcMem = NULL;
    }

    return TRUE;
}

/*
**  Function : WS_ShowCaret
**  Purpose  :
**      Creates a new shape for the system caret and assigns ownership of 
**      the caret to the specified window. The caret shape can be a line, 
**      a block, or a bitmap. 
*/
BOOL WS_ShowCaret(PWINOBJ pWin)
{
    HDC hScreenDC = NULL;
    int xCaret = 0, yCaret = 0;

    if (pWin != pCaretWin)
        return FALSE;

    if (bUsrClr && (nCaretShow == 0))
    {
        hdcMem = CreateMemoryDC(nCaretWidth, nCaretHeight);
        hScreenDC = CreateScreenDC();
        xCaret = nCaretX;
        yCaret = nCaretY;
        WND_ClientToScreen(pWin, &xCaret, &yCaret);
        BitBlt(hdcMem, 0, 0, nCaretWidth, nCaretHeight, hScreenDC, xCaret,
            yCaret, SRCCOPY);
        DeleteDC(hScreenDC);
    }

    if (nCaretShow < 1)
    {
        nCaretShow++;

        // Now the caret is not visible on screen
        ASSERT(bBlinkShow == FALSE);
    }

    return TRUE;
}

/*
**  Function : WS_GetCaretBlinkTime
**  Purpose  :
**      Returns the elapsed time, in milliseconds, required to invert the 
**      caret’s pixels. 
*/
UINT WS_GetCaretBlinkTime(void)
{
    return uCaretBlinkTime;
}

/*
**  Function : WS_SetCaretBlinkTime
**  Purpose  :
**      Sets the caret blink time to the specified number of milliseconds.
**      The blink time is the elapsed time, in milliseconds, required to 
**      invert the caret’s pixels. 
*/
void WS_SetCaretBlinkTime(UINT uMSeconds)
{
    if (uCaretBlinkTime != uMSeconds)
    {
        uCaretBlinkTime = uMSeconds;

        if (uCaretTimerID != 0)
            WS_DestroyTimer(g_pRootWin, uCaretTimerID, TRUE);

        uCaretTimerID = WS_CreateTimer(g_pRootWin, 1, uCaretBlinkTime, CaretBlinkProc, TRUE);

/*
        LEAVEMONITOR;

        if (uCaretTimerID != 0)
            KillSystemTimer(NULL, uCaretTimerID);

        uCaretTimerID = SetSystemTimer(NULL, 0, uCaretBlinkTime, 
            CaretBlinkProc);

        ENTERMONITOR;
*/
    }
}

/*
**  Function : WS_GetCaretWindow
**  Purpose  :
**      Retrieves the caret window
*/
PWINOBJ WS_GetCaretWindow(void)
{
    return pCaretWin;
}

/*
**  Function : WS_ScrollCaret
**  Purpose  :
**      Scrolls the caret if the window owns the caret is the specified
**      window.
**  Purpose  :
**      pWin : Specifies the scroll window.
**      dx   : Specifies the amount of horizontal scrolling.
**      dy   : Specifies the amount of vertical scrolling. 
*/
void WS_ScrollCaret(PWINOBJ pWin, int dx, int dy)
{
    ASSERT(pWin != NULL);

    if (pWin != pCaretWin || (dx == 0 && dy == 0))
        return;

    // Hide the caret if the caret is showing
    if (pCaretWin && nCaretShow > 0 && bBlinkShow)
    {
        bBlinkShow = FALSE;
        DrawCaret();
    }

    nCaretX += dx;
    nCaretY += dy;
}

/*
**  Function : WS_DestroyWindowCaret
**  Purpose  :
**      If the current caret is owned by the specified window, destory it.
**      This function should be called when a window being destroy.
**  Params   :
**      pWin : Indicates the window to be destoryed.
*/
void WS_DestroyWindowCaret(PWINOBJ pWin)
{
    if (pWin == pCaretWin)
        WS_DestroyCaret();
}

/*
**  Function : SetCaretProc
**  Purpose  :
**      
*/
CARETPROC WINAPI SetCaretProc(CARETPROC pNewProc)
{
    CARETPROC pOldProc;

    ENTERMONITOR;

    pOldProc = pCaretProc;
    pCaretProc = pNewProc;

    LEAVEMONITOR;

    return pOldProc;
}

// Internal functions

/*
**  Function : CaretBlinkProc
**  Purpose  :
**      Blinks the caret if necessary.
*/
static void CaretBlinkProc(HWND hWnd, UINT message, UINT uIDEvent, 
                           DWORD dwTime)
{
    if (!pCaretWin || nCaretShow != 1)
        return;

    ENTERMONITOR;

    bBlinkShow = !bBlinkShow;
    DrawCaret();

    LEAVEMONITOR;
}

/*
**  Function : DrawCaret
**  Purpose  :
**      Draws or clear the caret. 
**  Remarks  :
**      This function will be called by both window task and timer task, 
**      So must use a flag to avoid function reentry.
*/
static void DrawCaret(void)
{
    PDC pDC, pSrcDC;
    RECT rect;
    int nOldRop;

    ASSERT(pCaretWin != NULL);

    // 这里使用cache中的dc，不能使用当前的窗口dc，否则可能会影响应用层的绘
    // 图操作
    pDC = DC_GetWindowDCFromCache(pCaretWin, TRUE, FALSE);
    if (!pDC)
        return;

    if (bUsrClr)
    {
        if(bBlinkShow)
        {
            SetRect(&rect, nCaretX, nCaretY, nCaretX + nCaretWidth, 
                nCaretY + nCaretHeight);
            WS_ClearRect(pDC, &rect, crUsrClr);
        }
        else
        {
            pSrcDC = (PDC)WOT_GetObj((HANDLE)hdcMem, OBJ_DC);
            WS_BitBlt(pDC, nCaretX, nCaretY, nCaretWidth, nCaretHeight, pSrcDC, 0, 0, SRCCOPY);
        }
    }
    else if (pCaretBmp)
    {
        WS_BitBlt(pDC, nCaretX, nCaretY, nCaretWidth, nCaretHeight, 
            (PDC)pCaretBmp, 0, 0, ROP_SRC_XOR_DST);
    }
    else
    {
        SetRect(&rect, nCaretX, nCaretY, nCaretX + nCaretWidth, 
            nCaretY + nCaretHeight);

        nOldRop = DC_SetROP2(pDC, ROP_SRC_XOR_DST);
        WS_ClearRect(pDC, &rect, COLOR_WHITE);
        DC_SetROP2(pDC, nOldRop);
    }

    DC_ReleaseWindowDCToCache(pDC);
}

COLORREF WINAPI SetCaretColor(COLORREF crColor)
{
    COLORREF crOld = 0;

    ENTERMONITOR;

    crOld = crUsrClr;
    crUsrClr = crColor;

    LEAVEMONITOR;

    return crOld;
}
