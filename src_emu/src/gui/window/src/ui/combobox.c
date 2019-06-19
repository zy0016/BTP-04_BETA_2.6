/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "hpwin.h"

#include "control.h"

#include "hpfile.h"

#include "string.h"

#include "plx_pdaex.h"

#include "winpda.h"



#define COMBO_EDIT_HEIGHT_MORE    2



typedef struct

{

    WORD		wState;

    WORD		wID;

    HWND		hListBoxWnd;

    HWND		hEditWnd;

	BYTE		bHasSetFocus;

	BYTE		bPenDown;

    BYTE		bMoveInComboList;

    BYTE		bDropDown;

    BYTE		bLeaveArrow;

    BYTE		bAddNewString;

	BYTE		bSelectInSimple;//笔点simple中某项.

} COMBOBOXDATA, *PCOMBOBOXDATA;



static HBITMAP hbmpDownArrow;



#define ARROW_BMP_WIDTH     21

#define ARROW_BMP_HEIGHT    21



// Internal function prototypes

LRESULT CALLBACK COMBOBOX_WndProc(HWND hWnd, UINT wMsgCmd, 

                                  WPARAM wParam, LPARAM lParam);



static int  ComboMessageLoop(HWND hWnd, PCOMBOBOXDATA pComboBoxData);

static void AddOrFindString(HWND hWnd, PCOMBOBOXDATA pComboBoxData, BOOL bAddString);

static void PaintText(HWND hWnd, HDC hdc, PSTR pszString, BOOL bReversed);

#if (!INTERFACE_MONO)

static void DrawFocus(HDC hdc, RECT* pRect);



static void Draw3DComboRect_W95(HDC hdc, const RECT* pRect);

static void Draw3DArrowRect_W95(HDC hdc, const RECT* pRect, 

                                BOOL bDown, BOOL bFillRect, int nBitsPixel);

#endif



static void ProcessKeyDown(HWND hWnd, PCOMBOBOXDATA pComboBoxData, UINT wMsgCmd, 

                           WPARAM wParam, LPARAM lParam);

static void ProcessPenDown(HWND hWnd, PCOMBOBOXDATA pComboBoxData, UINT wMsgCmd, 

                           WPARAM wParam, LPARAM lParam);

static void ProcessCommand(HWND hWnd, PCOMBOBOXDATA pComboBoxData, 

                           WPARAM wParam, LPARAM lParam);

static BOOL InitCombo(HWND hWnd, PCOMBOBOXDATA pComboBoxData, LPARAM lParam);

static HBRUSH SetCtlColorComboBox(HWND hWnd, HDC hdc);

static void SelChange(HWND hWnd, PCOMBOBOXDATA pComboBoxData);//处理LBN_SELCHANGE消息



void WindowToParent(HWND hWnd, int* left, int* top);



// External funciton prototypes

void ResendPenMessage(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 

                      LPARAM lParam);

#define MAX_PATH 256

BOOL COMBOBOX_RegisterClass(void)

{

    WNDCLASS wc;

    char PathName[MAX_PATH];



    wc.style            = CS_OWNDC | CS_DEFWIN;//CS_PARENTDC;

    wc.lpfnWndProc      = COMBOBOX_WndProc;

    wc.cbClsExtra       = 0;

    wc.cbWndExtra       = sizeof(COMBOBOXDATA);

    wc.hInstance        = NULL;

    wc.hIcon            = NULL;

    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);

    wc.hbrBackground    = GetStockObject(WHITE_BRUSH);

    wc.lpszMenuName     = NULL;

    wc.lpszClassName    = "COMBOBOX";



    if (!RegisterClass(&wc))

        return FALSE;



    //hbmpDownArrow = CreateBitmap(ARROW_BMP_WIDTH, ARROW_BMP_HEIGHT,

    //  1, 1, DownArrowBmpData);

    PLXOS_GetResourceDirectory(PathName);

    strcat(PathName, "caplist.bmp");

    hbmpDownArrow = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    

    return TRUE;

}



LRESULT CALLBACK COMBOBOX_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)

{

    LRESULT         lResult;

    PCOMBOBOXDATA   pComboBoxData;

    IWORD           x, y;//Is signed.

    RECT            rect, rect1, rcListWindow, rcParent, rcWindow, rcClient, rcCreateVal; 

    RECT            rcCombo, rcList, rcListClient, rcListOld, rcArrow;

    int             nComboTop, nComboBottom, nListTop, nListBottom;

    HDC             hdc;

	DWORD	        dwStyle;

	IWORD	        wIndex;

	BOOL	        fShow;

//#ifndef NOSENDCTLCOLOR

    HWND            hWndParent;

//#endif

    HWND            hWndFocus;

    int             surplus = 0, n3DBorder, nGap, nListBorder;

    HBRUSH          hBrush;

	SIZE            size;

	int             nBitsPixel, nArrowMore, nWidth, nHeight, nScreenHeight;

    static int      nMaxNum;

    HFONT           hFont;

    BOOL            fRedraw;

#if (!INTERFACE_MONO)

    COLORREF        crBkOld, crTextOld;

    HBRUSH          hBrushOld;

#endif



	lResult = 0;

    pComboBoxData = (PCOMBOBOXDATA)GetUserData(hWnd);



    switch (wMsgCmd)

    {

    case WM_CREATE :



        lResult = (LRESULT)InitCombo(hWnd, pComboBoxData, lParam);



        break;



    case WM_SIZE :



        nWidth = LOWORD(lParam);  // width of client area 

        nHeight = HIWORD(lParam); // height of client area 



        hdc = GetDC(hWnd);

        GetTextExtentPoint32(hdc, "TEXT", -1, &size);

        ReleaseDC(hWnd, hdc);

        nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);



#if (INTERFACE_MONO)



        nGap = 1;

        n3DBorder = 0;

        nArrowMore = 2;



#else // INTERFACE_MONO



        if (pComboBoxData->wState == CBS_SIMPLE)

        {

            if (nBitsPixel == 2)

                nGap = 1;

            else

                nGap = 0;

        }

        else

            nGap = 2;



        if (nBitsPixel == 1)

        {

            n3DBorder = 0;

            nArrowMore = 2;

        }

        else if (nBitsPixel == 2)

        {

            n3DBorder = 1;

            nArrowMore = 6;//3D border.

        }

        else

        {

            n3DBorder = 2;

            nArrowMore = 9;

        }



#endif // INTERFACE_MONO



        GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);

        GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_SCREEN);

        GetWindowRectEx(GetParent(hWnd), &rcParent, W_CLIENT, XY_SCREEN);



        GetClientRect(hWnd, &rect);



        // 2 is the two border height of listbox.

        nListBorder = 2;



#if (INTERFACE_MONO)

#else // INTERFACE_MONO

        if (nBitsPixel != 1 && nBitsPixel != 2 &&

            pComboBoxData->wState == CBS_SIMPLE)

        {

            nListBorder += 4;

        }

#endif // INTERFACE_MONO



        if ((rect.bottom - rect.top - 2 * n3DBorder) > 

            (size.cy + COMBO_EDIT_HEIGHT_MORE))

        {

            surplus = (rect.bottom - rect.top - nListBorder - 

                2 * n3DBorder - nGap) % (size.cy + COMBO_EDIT_HEIGHT_MORE);

        }

        

        rect.bottom -= surplus;

        rcClient.bottom -= surplus;

        rcWindow.bottom -= surplus;

        nHeight -= surplus;



        rcCreateVal.left = rect.left + n3DBorder;

        rcCreateVal.top = rect.top + n3DBorder;

        rcCreateVal.right = rect.right - n3DBorder;

        rcCreateVal.bottom = rect.bottom;



        //To color or 4-gray 3D effect,

        //left and top border are 2 pixel, right and bottom border are 1 pixel. 

        

        if (rect.bottom - rect.top > 

            size.cy + COMBO_EDIT_HEIGHT_MORE + 2 * n3DBorder + rect.top)

        {

            if (pComboBoxData->wState == CBS_DROPDOWN)

            {

                MoveWindow(pComboBoxData->hEditWnd, 

                    rcCreateVal.left, rcCreateVal.top, 

                    rcCreateVal.right - rcCreateVal.left - 

                    ARROW_BMP_WIDTH - nArrowMore, 

                    size.cy + COMBO_EDIT_HEIGHT_MORE, 

                    TRUE);

            }

            else if (pComboBoxData->wState == CBS_SIMPLE)

            {

                MoveWindow(pComboBoxData->hEditWnd, 

                    rcCreateVal.left, rcCreateVal.top, 

                    rcCreateVal.right - rcCreateVal.left, 

                    size.cy + COMBO_EDIT_HEIGHT_MORE, 

                    TRUE);

            }

        }



        if (rect.bottom - rect.top - (size.cy + COMBO_EDIT_HEIGHT_MORE + 3) >

            size.cy + 4)

            //2 of 4 is listbox border, size.cy + 2 is listbox item height.

        {

            nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

            y = (IWORD)rect.bottom - rect.top - (size.cy +

                COMBO_EDIT_HEIGHT_MORE + 2 * n3DBorder + nGap);

            if (y > nScreenHeight)

                y = (IWORD)nScreenHeight;

            

            if (pComboBoxData->wState != CBS_SIMPLE)

            {

                MoveWindow(pComboBoxData->hListBoxWnd, 

                    rcClient.left, 

                    rcClient.top + size.cy + COMBO_EDIT_HEIGHT_MORE + 

                    nGap + 2 * n3DBorder + rcClient.top - rcWindow.top, 

                    rect.right - rect.left, 

                    y, 

                    TRUE);

            }

            else

            {

                MoveWindow(pComboBoxData->hListBoxWnd, 

                    rect.left, 

                    rect.top + size.cy + COMBO_EDIT_HEIGHT_MORE + 

                    nGap + 2 * n3DBorder, 

                    rect.right - rect.left, 

                    y, 

                    TRUE);

            }

        }



        if (pComboBoxData->wState == CBS_SIMPLE)

        {

            MoveWindow(hWnd, rcWindow.left - rcParent.left, 

                rcWindow.top - rcParent.top, 

                rcWindow.right - rcWindow.left, 

                rcWindow.bottom - rcWindow.top, 

                TRUE);

        }

        else if (!pComboBoxData->bDropDown &&

            (rect.right - rect.left > ARROW_BMP_WIDTH + nArrowMore) &&

            (rect.bottom - rect.top > 

             size.cy + COMBO_EDIT_HEIGHT_MORE + 2 * n3DBorder))

        {

            MoveWindow(hWnd, rcWindow.left - rcParent.left, 

                rcWindow.top - rcParent.top, 

                rcWindow.right - rcWindow.left, 

                size.cy + COMBO_EDIT_HEIGHT_MORE + 2 * n3DBorder + 

                rcWindow.bottom - rcWindow.top - rect.bottom, 

                TRUE);

        }

        if (pComboBoxData->wState != CBS_DROPDOWNLIST)

            nMaxNum = (rcCreateVal.right - rcCreateVal.left)/size.cx;



        break;



    case WM_DESTROY :

		if (pComboBoxData->wState != CBS_SIMPLE)

		{

			DestroyWindow(pComboBoxData->hListBoxWnd);

			pComboBoxData->hListBoxWnd = NULL;

		}

        break;



    case WM_PAINT :



        hdc = BeginPaint(hWnd, NULL);

		

        //COMBOBOX_Paint(hWnd, hdc, pComboBoxData);



        GetTextExtentPoint32(hdc, "TEXT", -1, &size);

        nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);

        GetClientRect(hWnd, &rect);



#if (INTERFACE_MONO)



        if (pComboBoxData->wState != CBS_SIMPLE)

        {

            SetRect(&rect1, rect.right - ARROW_BMP_WIDTH, rect.top, 

                rect.right, rect.top + size.cy + COMBO_EDIT_HEIGHT_MORE);



            BitBlt(hdc,	(rect1.right + rect1.left - ARROW_BMP_WIDTH) / 2,

                (rect1.bottom + rect1.top - ARROW_BMP_HEIGHT) / 2,

                ARROW_BMP_WIDTH, ARROW_BMP_HEIGHT, (HDC)hbmpDownArrow, 

                0, 0, ROP_SRC);

        }

		

#else // INTERFACE_MONO



        if (nBitsPixel == 1)

            n3DBorder = 0;

        else if (nBitsPixel == 2)

            n3DBorder = 1;

        else

            n3DBorder = 2;



        if (pComboBoxData->wState != CBS_SIMPLE)

        {

            if (nBitsPixel == 1)    // mono

            {

                SetRect(&rect1, rect.right - ARROW_BMP_WIDTH, rect.top, 

                    rect.right, rect.top + size.cy + 2 * n3DBorder + 

                    COMBO_EDIT_HEIGHT_MORE);

                crBkOld = SetBkColor(hdc, COLOR_WHITE);

            }

            else

            {

                nArrowMore = 5;

                if (nBitsPixel == 2)

                {

                    nArrowMore = 2;

                }

                

                //1 is the scroll right border.

                SetRect(&rect1, rect.right - n3DBorder - ARROW_BMP_WIDTH - 

                    nArrowMore - 4, 

                    rect.top + n3DBorder, rect.right - n3DBorder, 

                    rect.top + size.cy + COMBO_EDIT_HEIGHT_MORE + 

                    n3DBorder);

                

                Draw3DArrowRect_W95(hdc, &rect1, FALSE, TRUE, nBitsPixel);

                

                InflateRect(&rect1, -2, -2);

                crBkOld = SetBkColor(hdc, GetWindowColor(FOCUS_COLOR));

            }

            

            crTextOld = SetTextColor(hdc, COLOR_BLACK);

            BitBlt(hdc,	(rect1.right + rect1.left - ARROW_BMP_WIDTH) / 2,

                (rect1.bottom + rect1.top - ARROW_BMP_HEIGHT) / 2,

                ARROW_BMP_WIDTH, ARROW_BMP_HEIGHT, (HDC)hbmpDownArrow, 

                0, 0, ROP_SRC);

            

            SetBkColor(hdc, crBkOld);

            SetTextColor(hdc, crTextOld);

        }

		

        if (nBitsPixel == 2)

            n3DBorder = 2;



        SetRect(&rect1, rect.left, rect.top, rect.right,

            rect.top + size.cy + COMBO_EDIT_HEIGHT_MORE + 2 * n3DBorder);

        

        if (nBitsPixel == 1 || nBitsPixel == 2)

        {

            if (nBitsPixel == 2)

                rect1.bottom -= 2;



            hBrush = GetStockObject(BLACK_BRUSH);

            hBrushOld = SelectObject(hdc, hBrush);

            //FrameRect(hdc, &rect1, hBrush);

            SelectObject(hdc, hBrushOld);

        }

        else

        {

            Draw3DComboRect_W95(hdc, &rect1);

        }



#endif // INTERFACE_MONO



        if (pComboBoxData->wState == CBS_DROPDOWNLIST)

        {

            int     nLength, wIndex;

            char    str[SINGLE_DEFLENGTH];



            nLength = 0;

            wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 0, 0);

            

            if (wIndex != LB_ERR)

            {

                nLength = SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

                    (WPARAM)wIndex, (LPARAM)0);

            }

            

            if (nLength)

            {

                if (nLength >= SINGLE_DEFLENGTH)

                    break;

                

                SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, (WPARAM)wIndex, 

                    (LPARAM)str);

                

                PaintText(hWnd, hdc, str, GetFocus() == hWnd);

            }

        }



        EndPaint(hWnd, NULL);



        break;



//#ifndef SCP_NOKEYBOARD

    case WM_KEYDOWN :



        ProcessKeyDown(hWnd, pComboBoxData, wMsgCmd, wParam, lParam);



        //An application should return zero if it processes this message. 



        break;

//#endif //SCP_NOKEYBOARD



    case PWM_SETSCROLLSTATE:

        hWndParent = GetParent(hWnd);

        SendMessage(hWndParent, PWM_SETSCROLLSTATE, wParam, lParam);

        break;



    case WM_PENMOVE :

    case WM_PENUP :



        x = LOWORD(lParam);

        y = HIWORD(lParam);



        if ((pComboBoxData->wState == CBS_DROPDOWN || 

            pComboBoxData->wState == CBS_DROPDOWNLIST))

        {

            GetClientRect(hWnd, &rect);

            

            hdc = GetDC(hWnd);

            GetTextExtentPoint32(hdc, "TEXT", -1, &size);

            nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);

            

            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_SCREEN);

            GetWindowRect(pComboBoxData->hListBoxWnd, &rcListWindow);

            GetClientRect(pComboBoxData->hListBoxWnd, &rcListClient);

            GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);

            GetWindowRectEx(GetParent(hWnd), &rcParent, W_CLIENT, XY_SCREEN);

            

            GetWindowRectEx(pComboBoxData->hListBoxWnd, &rcListOld, W_WINDOW, XY_SCREEN);

            nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

            

            nGap = 2;

            

            //列表项向上打开。

            if (rcWindow.bottom + rcListWindow.bottom - rcListWindow.top > nScreenHeight)

            {

                nListBottom = rcWindow.top - rcClient.top - nGap;

                nComboTop = nListTop = nListBottom - 

                    (rcListClient.bottom - rcListClient.top);

                nComboBottom = rect.bottom;

                

                //向上打开高度也不够，则列表项从顶部开始。

                if (nComboTop < -rcWindow.top) 

                {

                    nComboTop = nListTop = -rcWindow.top ;

                    nComboBottom = nListBottom = 

                        nComboTop + rcListWindow.bottom - rcListWindow.top;

                }

            }

            else

            {

                nComboTop = rect.top;

                nListTop = rect.top + size.cy + COMBO_EDIT_HEIGHT_MORE + 

                    nGap + rcWindow.bottom - rcClient.bottom;

                nComboBottom = nListBottom = 

                    nListTop + rcListWindow.bottom - rcListWindow.top;

            }

            

            SetRect(&rcCombo, rect.left, nComboTop, rect.right, nComboBottom);

            SetRect(&rcList, rect.left, nListTop, rect.right, nListBottom);



            nArrowMore = 5;

            n3DBorder = 2;

            if (nBitsPixel == 2)

            {

                nArrowMore = 2;

                n3DBorder = 1;

            }

            

            SetRect(&rcArrow, rect.right - n3DBorder - ARROW_BMP_WIDTH - 

                nArrowMore - 4, 

                rect.top + n3DBorder, rect.right - n3DBorder, 

                rect.top + size.cy + COMBO_EDIT_HEIGHT_MORE + 

                n3DBorder);

            

            if (PtInRectXY(&rcList, x, y))

            {

                if (pComboBoxData->bPenDown)

                {

#if (INTERFACE_MONO)

#else // INTERFACE_MONO

                    if (nBitsPixel != 1)//not mono.

                        Draw3DArrowRect_W95(hdc, &rcArrow, FALSE, FALSE, nBitsPixel);

#endif // INTERFACE_MONO

                    

                    pComboBoxData->bPenDown = FALSE;

                    pComboBoxData->bLeaveArrow = FALSE;



                    pComboBoxData->bMoveInComboList = TRUE;

                }

            }

            else if (!PtInRectXY(&rcArrow, x, y))//Arrow region.

            {

                if (pComboBoxData->bPenDown)

                {

#if (INTERFACE_MONO)

#else // INTERFACE_MONO



                    if (nBitsPixel != 1)//not mono.

                        Draw3DArrowRect_W95(hdc, &rcArrow, FALSE, FALSE, nBitsPixel);

#endif // INTERFACE_MONO

                    

                    pComboBoxData->bLeaveArrow = TRUE;

                }

                

                if (wMsgCmd == WM_PENUP)

                {

                    if (pComboBoxData->bMoveInComboList)

                        PostMessage(hWnd, WM_ENDLOOP, 0, 0);



                    pComboBoxData->bPenDown = FALSE;

                    pComboBoxData->bLeaveArrow = FALSE;

                }

            }

            else if(wMsgCmd == WM_PENUP)

            {

#if (INTERFACE_MONO)

#else  // INTERFACE_MONO

                if (nBitsPixel != 1)//not mono.

                    Draw3DArrowRect_W95(hdc, &rcArrow, FALSE, FALSE, nBitsPixel);

#endif // INTERFACE_MONO



                if (pComboBoxData->bMoveInComboList)

                    PostMessage(hWnd, WM_ENDLOOP, 0, 0);

                

                pComboBoxData->bLeaveArrow = FALSE;

				pComboBoxData->bPenDown = FALSE;

            }

            else if (pComboBoxData->bLeaveArrow && pComboBoxData->bPenDown)

            {

#if (INTERFACE_MONO)

#else // INTERFACE_MONO

                if (nBitsPixel != 1)//not mono.

                    Draw3DArrowRect_W95(hdc, &rcArrow, TRUE, FALSE, nBitsPixel);

#endif // INTERFACE_MONO

                

                pComboBoxData->bLeaveArrow = FALSE;

            }

            

            ReleaseDC(hWnd, hdc);

        }



        GetWindowRectEx(hWnd, &rcParent, W_CLIENT, XY_SCREEN);

        if (pComboBoxData->wState == CBS_DROPDOWN)

        {

            GetWindowRectEx(pComboBoxData->hEditWnd, &rcClient, W_CLIENT, XY_SCREEN);

            SendMessage(pComboBoxData->hEditWnd, wMsgCmd, wParam, 

                MAKELPARAM(x - (IWORD)(rcClient.left - rcParent.left), 

                y - (IWORD)(rcClient.top - rcParent.top)));

        }



        if ((pComboBoxData->wState == CBS_SIMPLE) || pComboBoxData->bDropDown)

        {

            GetWindowRectEx(pComboBoxData->hListBoxWnd, &rcClient, W_CLIENT, XY_SCREEN);

            SendMessage(pComboBoxData->hListBoxWnd, wMsgCmd, 

                MAKEWPARAM(wParam, pComboBoxData->bMoveInComboList), 

                MAKELPARAM(x - (IWORD)(rcClient.left - rcParent.left), 

                y - (IWORD)(rcClient.top - rcParent.top)));

        }



        if (wMsgCmd == WM_PENUP)

            pComboBoxData->bMoveInComboList = FALSE;



		if (PtInRectXY(&rcList, x, y) && wMsgCmd == WM_PENUP)

		{

			PostMessage(GetParent(hWnd), WM_COMMAND, 

				MAKEWPARAM(pComboBoxData->wID, CBN_SELENDOK), (LPARAM)hWnd);  

		}        

        break;



    case WM_PENDOWN :



        ProcessPenDown(hWnd, pComboBoxData, wMsgCmd, wParam, lParam);

        break;



    case WM_SETFOCUS :



        DefWindowProc(hWnd, wMsgCmd, wParam, lParam);

        if (pComboBoxData->hEditWnd)

        {

            SetFocus(pComboBoxData->hEditWnd);



            SendMessage(GetParent(hWnd), PWM_SETSCROLLSTATE, SCROLLMIDDLE, MASKMIDDLE);

            //For LBS_DROPDOWN and LBS_SIMPLE, when set focus, invert the edit text, 

            //caret is at end.

            if (GetFocus() == pComboBoxData->hEditWnd)

            {

                SendMessage(pComboBoxData->hEditWnd, EM_SETSEL, (WPARAM)0,

                    (LPARAM)-1);

            }

        }

        else if (pComboBoxData->wState == CBS_DROPDOWNLIST)

        {

            int     nLength, wIndex;

            char    str[SINGLE_DEFLENGTH];



            nLength = 0;

            wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 0, 0);



            SendMessage(GetParent(hWnd), PWM_SETSCROLLSTATE, SCROLLMIDDLE, MASKMIDDLE);

            if (wIndex != LB_ERR)

            {

                nLength = SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

                    (WPARAM)wIndex, (LPARAM)0);

            }

            

            if (nLength)

            {

                if (nLength >= SINGLE_DEFLENGTH)

                    break;

                

                SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, (WPARAM)wIndex, 

                    (LPARAM)str);

                

                hdc = GetDC(hWnd);

                SetCtlColorComboBox(hWnd, hdc);

                PaintText(hWnd, hdc, str, TRUE);

                ReleaseDC(hWnd, hdc);

            }



            SendMessage(GetParent(hWnd), WM_COMMAND, 

                MAKELONG(pComboBoxData->wID, CBN_SETFOCUS), (LPARAM)hWnd);

        }

        else

            SendMessage(GetParent(hWnd), PWM_SETSCROLLSTATE, NULL, MASKMIDDLE);



        break;



    case WM_KILLFOCUS :

        

        DefWindowProc(hWnd, wMsgCmd, wParam, lParam);

        //Do the process in case EN_KILLFOCUS.

        if (pComboBoxData->wState == CBS_DROPDOWNLIST)

        {

            int     nLength, wIndex;

            char    str[SINGLE_DEFLENGTH];



            nLength = 0;

            wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 0, 0);

            

            if (wIndex != LB_ERR)

            {

                nLength = SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

                    (WPARAM)wIndex, (LPARAM)0);

            }

            

            if (nLength)

            {

                if (nLength >= SINGLE_DEFLENGTH)

                    break;

                

                SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, (WPARAM)wIndex, 

                    (LPARAM)str);

                

                hdc = GetDC(hWnd);

                SetCtlColorComboBox(hWnd, hdc);

                PaintText(hWnd, hdc, str, FALSE);

                ReleaseDC(hWnd, hdc);

            }



            if (pComboBoxData->wState != CBS_SIMPLE)

                PostMessage(hWnd, WM_ENDLOOP, 0, 0);

            

            if (pComboBoxData->wState != CBS_DROPDOWNLIST && 

                pComboBoxData->bAddNewString)

            {

                AddOrFindString(hWnd, pComboBoxData, TRUE);

                pComboBoxData->bAddNewString = FALSE;

            }



            PostMessage(GetParent(hWnd), WM_COMMAND, 

                MAKELONG(pComboBoxData->wID, CBN_KILLFOCUS), (LPARAM)hWnd);

        }



        break;



    case WM_SETFONT:

        hFont = (HFONT)wParam;

        fRedraw = lParam;

        hdc = GetDC(hWnd);

        SelectObject(hdc, hFont);

        ReleaseDC(hWnd, hdc);



        if (pComboBoxData->hEditWnd)

            SendMessage(pComboBoxData->hEditWnd, WM_SETFONT, wParam, lParam);

        if (pComboBoxData->hListBoxWnd)

            SendMessage(pComboBoxData->hListBoxWnd, WM_SETFONT, wParam, lParam);



        if (fRedraw && IsWindowVisible(hWnd))

            InvalidateRect(hWnd, NULL, TRUE);

        break;



    case WM_CANCELMODE:

        

        if (pComboBoxData->wState == CBS_SIMPLE || pComboBoxData->bDropDown)

        {

            POINT pt;



            GetCursorPos(&pt);

            ScreenToClient(hWnd, &pt);



            SendMessage(hWnd, WM_PENUP, 0, MAKELPARAM(pt.x, pt.y));

            ShowWindow(pComboBoxData->hListBoxWnd, SW_HIDE);

        }

        break;



	case WM_COMMAND:



        ProcessCommand(hWnd, pComboBoxData, wParam, lParam);

		break;



	case WM_CHAR :



        if (pComboBoxData->wState != CBS_DROPDOWNLIST) 

        {

            lResult = (LRESULT)SendMessage(pComboBoxData->hEditWnd, WM_CHAR, 

                (WPARAM)wParam, (LPARAM)lParam);

        }

        break;



	case WM_GETTEXT :         



        if (pComboBoxData->wState != CBS_DROPDOWNLIST)

        {

            lResult = (LRESULT)SendMessage(pComboBoxData->hEditWnd, WM_GETTEXT, 

                (WPARAM)wParam, (LPARAM)lParam);

        }

        else

        {

            int         nLength, wIndex, nTextMax;

            PSTR        pszText; 



            //Specifies the maximum number of characters to be copied, 

            //including the terminating null character. 

            nTextMax = (int)wParam;

            pszText = (PSTR)lParam;

            

            nLength = 0;

            wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 0, 0);

            

            if (wIndex != LB_ERR)

            {

                nLength = SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

                    (WPARAM)wIndex, (LPARAM)0);

            }

            

            if (nLength > 0 && nLength < nTextMax)

            {

                SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, (WPARAM)wIndex, 

                    (LPARAM)pszText);

                

                //The return value is the number of characters copied. 

                lResult = nLength;

            }

            else

            {

                // Make sure we return an empty string

                pszText[0] = 0;



                if (!nLength)

                {

                    lResult = LB_ERR;

                }

            }

        }



        break;



	case WM_SETTEXT :         



        //It is CB_ERR if this message is sent to a combo box without an edit control. 

		if (pComboBoxData->wState == CBS_DROPDOWNLIST)

			lResult = (LRESULT)CB_ERR;

		else

            lResult = (LRESULT)SendMessage(pComboBoxData->hEditWnd, WM_SETTEXT, 

									   (WPARAM)wParam, (LPARAM)lParam);



        //if insufficient space is available to set the text in the edit control.

        if (lResult == FALSE)

			lResult = (LRESULT)CB_ERRSPACE;



        break;



    case WM_CTLCOLOREDIT :

    case WM_CTLCOLORLISTBOX :



#if (!NOSENDCTLCOLOR)

        dwStyle = GetWindowLong(hWnd, GWL_STYLE);

	

        if (dwStyle & WS_CHILD)

            hWndParent = GetParent(hWnd);

        else

            hWndParent = GetWindow(hWnd, GW_OWNER);



        if (!hWndParent)

        {

#endif

            SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));

            SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));

            return (LRESULT)(COLOR_WINDOW + 1);

#if (!NOSENDCTLCOLOR)

        }

        else

        {

            return SendMessage(hWndParent, wMsgCmd, wParam, lParam);

        }

#endif



    case WM_ERASEBKGND :

		

        hdc = (HDC)wParam;

    

        hBrush = SetCtlColorComboBox(hWnd, hdc);

        GetClientRect(hWnd, &rect);

        FillRect(hdc, &rect, hBrush);

        

        //Returns 1, indicating that the background is erased. 

		lResult = (LRESULT)TRUE; 



		break;



    case WM_GETDLGCODE :

		

		lResult = (LRESULT)DLGC_WANTCHARS | DLGC_WANTARROWS; 

		break;



    case CB_BEGININIT :



        lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_BEGININIT, 

									   (WPARAM)wParam, (LPARAM)lParam);

        break;



    case CB_ENDINIT :



        lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_ENDINIT, 

									   (WPARAM)wParam, (LPARAM)lParam);

        break;



    case CB_SETTEXT:



		wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 

									   (WPARAM)0, (LPARAM)0);

        if (wIndex == LOWORD(wParam))

        {

            if (pComboBoxData->wState == CBS_DROPDOWNLIST)

            {

                hdc = GetDC(hWnd);

                SetCtlColorComboBox(hWnd, hdc);

                PaintText(hWnd, hdc, (LPSTR)lParam, GetFocus() == hWnd);

                ReleaseDC(hWnd, hdc);

            }

            else

            {

                SendMessage(pComboBoxData->hEditWnd, WM_SETTEXT, (WPARAM)0,

                    (LPARAM)lParam);

            }

        }



        lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_SETTEXT, 

									   (WPARAM)wParam, (LPARAM)lParam);

        break;



	case CB_ADDSTRING :         



		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_ADDSTRING, 

									   (WPARAM)wParam, (LPARAM)lParam);

        break;



    case CB_DELETESTRING :

        

		wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 

									   (WPARAM)0, (LPARAM)0);

        if (wIndex == wParam)

        {

            if (pComboBoxData->wState == CBS_DROPDOWNLIST)

            {

                hdc = GetDC(hWnd);

                SetCtlColorComboBox(hWnd, hdc);

                PaintText(hWnd, hdc, "", FALSE);

                ReleaseDC(hWnd, hdc);

            }

            else

            {

                SendMessage(pComboBoxData->hEditWnd, WM_SETTEXT, (WPARAM)0,

                    (LPARAM)"");

            }

        }



		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_DELETESTRING, 

									   (WPARAM)wParam, (LPARAM)lParam);

		break;



    case CB_FINDSTRING :



		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_FINDSTRING, 

									   (WPARAM)wParam, (LPARAM)lParam);

		break;



    case CB_FINDSTRINGEXACT :



		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_FINDSTRINGEXACT, 

									   (WPARAM)wParam, (LPARAM)lParam);

        break;



    case CB_GETDROPDOWNSTATE :



		lResult = (LRESULT)pComboBoxData->bDropDown;

		break;

		

    case CB_GETCOUNT :



		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCOUNT, 

									   (WPARAM)wParam, (LPARAM)lParam);

		break;



	case CB_GETCURSEL :

		

		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 

									   (WPARAM)wParam, (LPARAM)lParam);

		break;

	

    case CB_GETITEMDATA :



		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_GETITEMDATA, 

									   (WPARAM)wParam, (LPARAM)lParam);

		break;



    case CB_INSERTSTRING :



		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_INSERTSTRING, 

									   (WPARAM)wParam, (LPARAM)lParam);

		break;



    case CB_RESETCONTENT :



        //An application sends a CB_RESETCONTENT message to remove all items 

        //from the list box and edit control of a combo box. 

		if (pComboBoxData->wState == CBS_DROPDOWNLIST)

        {

            hdc = GetDC(hWnd);

            SetCtlColorComboBox(hWnd, hdc);

            PaintText(hWnd, hdc, "", FALSE);

            ReleaseDC(hWnd, hdc);

        }

        else

        {

            SendMessage(pComboBoxData->hEditWnd, WM_SETTEXT, (WPARAM)0,

                (LPARAM)"");

        }



        lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_RESETCONTENT, 

            (WPARAM)wParam, (LPARAM)lParam);



        //This message always returns CB_OKAY. 

		break;

/*

    case CB_SETTOPINDEX :



		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_SETTOPINDEX, 

									   (WPARAM)wParam, (LPARAM)lParam);

		break;

*/

    case CB_SELECTSTRING :       

		

		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_SELECTSTRING, 

									   (WPARAM)wParam, (LPARAM)lParam);



        if (lResult != CB_ERR)

        {

			SendMessage(hWnd, WM_COMMAND, 

                MAKELONG(pComboBoxData->wID, LBN_SELCHANGE), 

                (LPARAM)hWnd);

			SelChange(hWnd, pComboBoxData);

        }



		break;



    case CB_SETCURSEL :



		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_SETCURSEL, 

									   (WPARAM)wParam, (LPARAM)lParam);



        if (lResult == CB_ERR)

        {

            if (pComboBoxData->wState == CBS_DROPDOWNLIST)

            {

                hdc = GetDC(hWnd);

                SetCtlColorComboBox(hWnd, hdc);

                PaintText(hWnd, hdc, "", FALSE);

                ReleaseDC(hWnd, hdc);

            }

            else

            {

                SendMessage(pComboBoxData->hEditWnd, WM_SETTEXT, (WPARAM)0,

                    (LPARAM)"");

            }

        }

        else

        {

			SendMessage(hWnd, WM_COMMAND, 

                MAKELONG(pComboBoxData->wID, LBN_SELCHANGE), 

                (LPARAM)hWnd);

			SelChange(hWnd, pComboBoxData);

        }



		break;



    case CB_SETITEMDATA :



		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_SETITEMDATA, 

									   (WPARAM)wParam, (LPARAM)lParam);

		break;



    case CB_GETEDITSEL :



		if (pComboBoxData->wState == CBS_DROPDOWNLIST)

			lResult = (LRESULT)CB_ERR;

		else

			lResult = (LRESULT)SendMessage(pComboBoxData->hEditWnd, EM_GETSEL, 

									   (WPARAM)wParam, (LPARAM)lParam);

		break;



    case CB_GETLBTEXT :



		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, 

									   (WPARAM)wParam, (LPARAM)lParam);

		break;



    case CB_GETLBTEXTLEN :



		lResult = (LRESULT)SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

									   (WPARAM)wParam, (LPARAM)lParam);

		break;



    case CB_LIMITTEXT :



		if (pComboBoxData->wState != CBS_DROPDOWNLIST)

        {

            SendMessage(pComboBoxData->hEditWnd, EM_LIMITTEXT, 

                (WPARAM)wParam, (LPARAM)lParam);

            lResult = (LRESULT)TRUE;



        }

        else 

            lResult = (LRESULT)FALSE; 



		break;



    case CB_SETEDITSEL :

    

        //wParam = 0; not used; must be zero 

        //lParam = MAKELPARAM((ichStart), (ichEnd); start and end position 



		if (pComboBoxData->wState == CBS_DROPDOWNLIST)

			lResult = (LRESULT)CB_ERR;

		else

        {

            SendMessage(pComboBoxData->hEditWnd, EM_SETSEL, 

                (WPARAM)LOWORD(lParam), (LPARAM)HIWORD(lParam));



            lResult = (LRESULT)TRUE;

        }



        //If the message succeeds, the return value is TRUE. 

        //If the message is sent to a combo box with the CBS_DROPDOWNLIST 

        //style, it is CB_ERR. 

        break;



	case CB_SHOWDROPDOWN :



        dwStyle = GetWindowLong(hWnd, GWL_STYLE);

		//This message has no effect on a combo box created with the CBS_SIMPLE style. 

        if ( dwStyle & CBS_SIMPLE)

			break;

		

		fShow = (BOOL) wParam;     // the show/hide flag 

        if (!pComboBoxData->bDropDown && fShow)

        {

            pComboBoxData->bDropDown = TRUE;

            

            SendMessage(GetParent(hWnd), WM_COMMAND, 

                MAKELONG(pComboBoxData->wID, CBN_DROPDOWN), (LPARAM)hWnd);

            

            hWndFocus = GetFocus();

            if ((pComboBoxData->hEditWnd && hWndFocus != pComboBoxData->hEditWnd) || 

                (!pComboBoxData->hEditWnd && hWndFocus != hWnd))

            {

                SetFocus(hWnd);

            }

            SetCapture(hWnd);



            hdc = GetDC(hWnd);

            if (pComboBoxData->wState == CBS_DROPDOWNLIST)

            {

                int     nLength, wIndex;

                char    str[SINGLE_DEFLENGTH];

                

                nLength = 0;

                wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 0, 0);

                

                if (wIndex != LB_ERR)

                {

                    nLength = SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

                        (WPARAM)wIndex, (LPARAM)0);

                }

                

                if (nLength)

                {

                    if (nLength >= SINGLE_DEFLENGTH)

                        break;

                    

                    SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, (WPARAM)wIndex, 

                        (LPARAM)str);

                    

                    SetCtlColorComboBox(hWnd, hdc);

                    PaintText(hWnd, hdc, str, GetFocus() == hWnd);

                }

            }

            

            GetTextExtentPoint32(hdc, "TEXT", -1, &size);

            

            //Show ListBox

            GetWindowRect(pComboBoxData->hListBoxWnd, &rcListWindow);

            

            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_SCREEN);

            GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);

            GetWindowRectEx(GetParent(hWnd), &rcParent, W_CLIENT, XY_SCREEN);

            

            GetClientRect(hWnd, &rect);

            

            nScreenHeight = GetSystemMetrics(SM_CYSCREEN);



            if (pComboBoxData->wState == CBS_SIMPLE)

            {

                nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);



                if (nBitsPixel == 2)

                    nGap = 1;

                else

                    nGap = 0;

            }

            else

                nGap = 2;

            

            if (rcWindow.bottom + rcListWindow.bottom - rcListWindow.top > nScreenHeight)

            {

                y = (IWORD)(rcWindow.top - (rcListWindow.bottom - rcListWindow.top));

                if (y < 0) 

                    y = 0;

            }

            else

            {

                y = (IWORD)rcWindow.bottom + nGap;

            }

            

            MoveWindow(pComboBoxData->hListBoxWnd, 

                rcWindow.left, y, rcWindow.right - rcWindow.left,

                rcListWindow.bottom - rcListWindow.top, 

                FALSE);

            ShowWindow(pComboBoxData->hListBoxWnd, SW_SHOWNOACTIVATE);

            

            ComboMessageLoop(hWnd, pComboBoxData);

            

            ShowWindow(pComboBoxData->hListBoxWnd, SW_HIDE);



            if (pComboBoxData->wState == CBS_DROPDOWNLIST)

            {

                int     nLength, wIndex;

                char    str[SINGLE_DEFLENGTH];

                

                nLength = 0;

                wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 0, 0);

                

                if (wIndex != LB_ERR)

                {

                    nLength = SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

                        (WPARAM)wIndex, (LPARAM)0);

                }

                

                if (nLength)

                {

                    if (nLength >= SINGLE_DEFLENGTH)

                        break;

                    

                    SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, (WPARAM)wIndex, 

                        (LPARAM)str);

                    

                    SetCtlColorComboBox(hWnd, hdc);

                    PaintText(hWnd, hdc, str, GetFocus() == hWnd);

                }

            }

            

            pComboBoxData->bDropDown = FALSE;

            ReleaseDC(hWnd, hdc);

            ReleaseCapture();

        }

        else if (pComboBoxData->bDropDown && !fShow)

            PostMessage(hWnd, WM_ENDLOOP, 0, 0);



        //The return value is always TRUE. 

        lResult = (LRESULT)TRUE;

		break;

/*

    case CB_DIR :

    case CB_GETDROPPEDCONTROLRECT :

    case CB_GETDROPPEDSTATE :

    case CB_GETEXTENDEDUI :

    case CB_GETITEMHEIGHT :

    case CB_GETLOCALE :

    case CB_SETEXTENDEDUI :

    case CB_SETITEMHEIGHT :

    case CB_SETLOCALE :

*/

    default :



        lResult = DefWindowProc(hWnd, wMsgCmd, wParam, lParam);

    }



    return lResult;

}



static void PaintText(HWND hWnd, HDC hdc, PSTR pszString, BOOL bReversed)

{

    RECT rect, rc, rcEdit;

    SIZE size;

	int  nBitsPixel, nArrowMore, n3DBorder;

    COLORREF crBkOld, crTextOld;

    

    nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);

    GetClientRect(hWnd, &rect);

    GetTextExtentPoint32(hdc, "T", -1, &size);



#if (INTERFACE_MONO)

    n3DBorder = 0;

    nArrowMore = 1;

#else    

    if (nBitsPixel == 1)

    {

        nArrowMore = 0;

        n3DBorder = 0;

    }

    else if (nBitsPixel == 2)

    {

        nArrowMore = 6;//3D border.

        n3DBorder = 1;

    }

    else

    {

        nArrowMore = 9;

        n3DBorder = 2;

    }

#endif

    //Leave a space pixel on left, top and bottom side.

    rcEdit.left = rect.left + n3DBorder + 1;

    rcEdit.top = rect.top + n3DBorder + 1;

    rcEdit.right = rect.right - n3DBorder - ARROW_BMP_WIDTH - nArrowMore - 1;

    

    if (rcEdit.right < rcEdit.left)

        rcEdit.right = rcEdit.left;

    

    SetRect(&rc, rcEdit.left, rcEdit.top, 

        rcEdit.right, 

        rcEdit.top + size.cy + COMBO_EDIT_HEIGHT_MORE - 2);

    

    crBkOld = SetBkColor(hdc, RGB(210, 230, 255));



	InflateRect(&rc, 1, 1);

    DrawText(hdc, "", 0, &rc, DT_CLEAR);

	rc.left ++;
    if (bReversed)
    {
        SetBkColor(hdc, COLOR_BLUE);
        crTextOld = SetTextColor(hdc, COLOR_WHITE);
    }
        

    DrawText(hdc, pszString, -1,

        &rc, DT_LEFT | DT_VCENTER);

    

//	if (bReversed)

//	{

//		rc.left --;

//        DrawFocus(hdc, &rc);

//	}

//    if (bReversed)

//        SetTextColor(hdc, crTextOld);

    SetBkColor(hdc, crBkOld);
    if (bReversed)
        SetTextColor(hdc, crTextOld);

}

#if (!INTERFACE_MONO)

//The rect we can draw line is from left to right - 1, from top to bottom - 1 ?

//two pixel horizontal lines on top and bottom, two pixel vertical lines on left and right.

static void Draw3DComboRect_W95(HDC hdc, const RECT* pRect)

{

    HPEN hOldPen;



    if (!pRect)

        return;



    // Draw left and top darkgray line

    hOldPen = SelectObject(hdc, GetStockObject(GRAY_PEN));



    DrawLine(hdc, pRect->left, pRect->top, pRect->left, pRect->bottom);

    DrawLine(hdc, pRect->left + 1, pRect->top, pRect->right - 1, pRect->top);



    // Draw left and top black line

    SelectObject(hdc, GetStockObject(BLACK_PEN));



    DrawLine(hdc, pRect->left + 1, pRect->top + 1, pRect->left + 1,

        pRect->bottom - 1);



    DrawLine(hdc, pRect->left + 2, pRect->top + 1, pRect->right - 2, 

        pRect->top + 1);



    // Draw right and bottom light gray line

    SelectObject(hdc, GetStockObject(WTGRAY_PEN));



    DrawLine(hdc, pRect->right - 2, pRect->top + 1, pRect->right - 2,

        pRect->bottom - 2);

    DrawLine(hdc, pRect->left + 1, pRect->bottom - 2, pRect->right - 1,

        pRect->bottom - 2);



    SelectObject(hdc, GetStockObject(WHITE_PEN));



    DrawLine(hdc, pRect->right - 1, pRect->top, pRect->right - 1,

        pRect->bottom - 1);

    DrawLine(hdc, pRect->left, pRect->bottom - 1, pRect->right,

        pRect->bottom - 1);



    // Restore the old brush

    SelectObject(hdc, hOldPen);

}



//two pixel horizontal lines on top and bottom, two pixel vertical lines on left and right.

static void Draw3DArrowRect_W95(HDC hdc, const RECT* pRect, 

                                BOOL bDown, BOOL bFillRect, int nBitsPixel)

{

    HPEN hOldPen;

    RECT rc;



    if (!pRect)

        return;



    if (bDown && (nBitsPixel != 1))

    {

        RECT    rc;

		

        FrameRect(hdc, pRect, GetStockObject(GRAY_BRUSH));



        SetRect(&rc, pRect->left + 1, pRect->top + 1, pRect->right - 1, 

            pRect->bottom - 1);

		FrameRect(hdc, &rc, GetStockObject(LTGRAY_BRUSH));



        return;

    }



    // Draw left and top black line



    if (nBitsPixel != 2)

        hOldPen = SelectObject(hdc, GetStockObject(WTGRAY_PEN));

    else

        hOldPen = SelectObject(hdc, GetStockObject(GRAY_PEN));



    DrawLine(hdc, pRect->left, pRect->top, pRect->left, pRect->bottom - 1);



    DrawLine(hdc, pRect->left + 1, pRect->top, pRect->right - 1, pRect->top);



    // Draw left and top white line or darkgray line



    SelectObject(hdc, GetStockObject(WHITE_PEN));



    DrawLine(hdc, pRect->left + 1, pRect->top + 1, pRect->left + 1,

        pRect->bottom - 2);



    DrawLine(hdc, pRect->left + 2, pRect->top + 1, pRect->right - 2, 

        pRect->top + 1);



    // Draw right and bottom gray line

    

    SelectObject(hdc, GetStockObject(GRAY_PEN));

    

    DrawLine(hdc, pRect->right - 2, pRect->top + 1, pRect->right - 2,

        pRect->bottom - 2);

    

    DrawLine(hdc, pRect->left + 1, pRect->bottom - 2, pRect->right - 1,

        pRect->bottom - 2);

    

    SelectObject(hdc, GetStockObject(BLACK_PEN));

    

    DrawLine(hdc, pRect->right - 1, pRect->top, pRect->right - 1,

        pRect->bottom - 1);

    

    DrawLine(hdc, pRect->left, pRect->bottom - 1, pRect->right,

        pRect->bottom - 1);

    

    //draw button face

    if (bFillRect)

    {

        SetRect(&rc, pRect->left + 2, pRect->top + 2, pRect->right - 2,

            pRect->bottom - 2);

        ClearRect(hdc, &rc, GetSysColor(COLOR_BTNFACE));

    }

    

    // Restore the old brush

    SelectObject(hdc, hOldPen);

}





static void DrawFocus(HDC hdc, RECT* pRect)

{

	HBRUSH  hBrush;

    int     fnOldMode;

    COLORREF crOld;



    crOld = SetBkColor(hdc, COLOR_BLACK);



    fnOldMode = SetROP2(hdc, ROP_SRC_XOR_DST);

    

    hBrush = CreateHatchBrush(HS_SIEVE, COLOR_WHITE);

    FrameRect(hdc, pRect, hBrush);



    DeleteObject(hBrush);

    

    SetROP2(hdc, fnOldMode);

    SetBkColor(hdc, crOld);

}

#endif

static int ComboMessageLoop(HWND hWnd, PCOMBOBOXDATA pComboBoxData)

{

    MSG msg;



    while (TRUE)

    {

        GetMessage(&msg, 0, 0, 0);



        if (msg.hwnd == hWnd && (msg.message == WM_ENDLOOP))

            break;



        TranslateMessage(&msg);

        DispatchMessage(&msg);



        if (!IsWindowVisible(pComboBoxData->hListBoxWnd))

            break;

    }



    return 0;

}



static void AddOrFindString(HWND hWnd, PCOMBOBOXDATA pComboBoxData, BOOL bAddString)

{

    int     nLength;

    char    str[SINGLE_DEFLENGTH];

	IWORD	wIndex;

    DWORD	dwStyle;



    nLength = SendMessage(pComboBoxData->hEditWnd, WM_GETTEXTLENGTH, (WPARAM)0,

        (LPARAM)0);

    

    if (nLength <= 0 || nLength >= SINGLE_DEFLENGTH)

        return;



    SendMessage(pComboBoxData->hEditWnd, WM_GETTEXT, (WPARAM)nLength + 1,

        (LPARAM)str);

    

    if (bAddString)

    {

        wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_FINDSTRINGEXACT,

            (WPARAM)-1, (LPARAM)str);

    }

    else

    {

        wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_FINDSTRING,

            (WPARAM)-1, (LPARAM)str);

    }

    

    if (wIndex != LB_ERR) 

    {

        SendMessage(pComboBoxData->hListBoxWnd, LB_SETTOPINDEX,

            (WPARAM)wIndex, (LPARAM)0);

        SendMessage(pComboBoxData->hListBoxWnd, LB_SETCURSEL,

            (WPARAM)wIndex, (LPARAM)0);

    }

    else

    {

        dwStyle = GetWindowLong(hWnd, GWL_STYLE);

        

        if (bAddString && (dwStyle & CBS_ADDNEWSTRINGS))

        {

            SendMessage(pComboBoxData->hListBoxWnd, LB_ADDSTRING,//LB_INSERTSTRING,

                (WPARAM)-1, (LPARAM)str);

            SendMessage(pComboBoxData->hListBoxWnd, LB_SETCURSEL,

                (WPARAM)(SendMessage(pComboBoxData->hListBoxWnd,LB_FINDSTRINGEXACT,

                -1,(LPARAM)str)), (LPARAM)0);

        }

        /*else

        {

            SendMessage(pComboBoxData->hListBoxWnd, LB_SETCURSEL,

                (WPARAM)-1, (LPARAM)0);

        }*/

        SendMessage(pComboBoxData->hListBoxWnd, LB_SETTOPINDEX,

            (WPARAM)0, (LPARAM)0);

    }

}



void WindowToParent(HWND hWnd, int* left, int* top)

{

	RECT rect, rcParent;



    GetWindowRect(hWnd, &rect);

    GetWindowRect(GetParent(hWnd), &rcParent);

	

	*left += rect.left - rcParent.left;

	*top += rect.top - rcParent.top;

}





static void ProcessKeyDown(HWND hWnd, PCOMBOBOXDATA pComboBoxData, UINT wMsgCmd, 

                           WPARAM wParam, LPARAM lParam)

{

    //HWND    hWndParent;

    //DWORD	dwStyle;

#if (!NOKEYBOARD)

    IWORD   y;//Is signed.

    HDC     hdc;

    SIZE    size;

    int     nScreenHeight, nGap, nBitsPixel;

    RECT    rect, rcListWindow, rcParent, rcWindow, rcClient; 

    HWND    hWndFocus, hWndParent;

#endif // NOKEYBOARD

    

    switch ((WORD)wParam)

    {

#if (!NOKEYBOARD)

    case VK_UP:

    case VK_DOWN:



        if (!pComboBoxData->bDropDown)

        {

            hWndParent = GetParent(hWnd);

            if (hWndParent)

            {

                DWORD dwStyle;

                dwStyle = GetWindowLong(hWnd, GWL_STYLE);

                if (dwStyle & CS_NOSYSCTRL)

                {

                    SendMessage(hWndParent, WM_KEYDOWN, wParam, lParam);

                    return;

                }

                else

                {

                    if ((WORD)wParam == VK_UP)

                        hWndFocus = GetNextDlgTabItem(hWndParent, hWnd, TRUE);

                    else

                        hWndFocus = GetNextDlgTabItem(hWndParent, hWnd, FALSE);

                    SetFocus(hWndFocus);

                    return;

                }

            }

        }

        

        SendMessage(pComboBoxData->hListBoxWnd, wMsgCmd, wParam, lParam);

        

        if ((pComboBoxData->wState == CBS_DROPDOWN || 

            pComboBoxData->wState == CBS_DROPDOWNLIST))

        {

            if (!pComboBoxData->bDropDown)

            {

                pComboBoxData->bDropDown = TRUE;

                

                SendMessage(GetParent(hWnd), WM_COMMAND, 

                    MAKELONG(pComboBoxData->wID, CBN_DROPDOWN), (LPARAM)hWnd);

                

                hWndFocus = GetFocus();

                if ((pComboBoxData->hEditWnd && hWndFocus != pComboBoxData->hEditWnd) || 

                    (!pComboBoxData->hEditWnd && hWndFocus != hWnd))

                {

                    SetFocus(hWnd);

                }

                SetCapture(hWnd);

                

                hdc = GetDC(hWnd);

                if (pComboBoxData->wState == CBS_DROPDOWNLIST)

                {

                    int     nLength, wIndex;

                    char    str[SINGLE_DEFLENGTH];

                    

                    nLength = 0;

                    wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 0, 0);

                    

                    if (wIndex != LB_ERR)

                    {

                        nLength = SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

                            (WPARAM)wIndex, (LPARAM)0);

                    }

                    

                    if (nLength)

                    {

                        if (nLength >= SINGLE_DEFLENGTH)

                            break;

                        

                        SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, (WPARAM)wIndex, 

                            (LPARAM)str);

                        

                        SetCtlColorComboBox(hWnd, hdc);

                        PaintText(hWnd, hdc, str, GetFocus() == hWnd);

                    }

                }

                

                GetTextExtentPoint32(hdc, "TEXT", -1, &size);

                

                //Show ListBox

                GetWindowRect(pComboBoxData->hListBoxWnd, &rcListWindow);

                

                GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_SCREEN);

                GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);

                GetWindowRectEx(GetParent(hWnd), &rcParent, W_CLIENT, XY_SCREEN);

                

                GetClientRect(hWnd, &rect);

                

                nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

                

                if (pComboBoxData->wState == CBS_SIMPLE)

                {

                    nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);

                    

                    if (nBitsPixel == 2)

                        nGap = 1;

                    else

                        nGap = 0;

                }

                else

                    nGap = 2;

                

                if (rcWindow.bottom + rcListWindow.bottom - rcListWindow.top > 

                    nScreenHeight)

                {

                    y = (IWORD)(rcWindow.top - (rcListWindow.bottom - rcListWindow.top));

                    if (y < 0) 

                        y = 0;

                }

                else

                {

                    y = (IWORD)rcWindow.bottom + nGap;

                }

                

                MoveWindow(pComboBoxData->hListBoxWnd, 

                    rcClient.left, y, rect.right - rect.left,

                    rcListWindow.bottom - rcListWindow.top, 

                    TRUE);

                

                ComboMessageLoop(hWnd, pComboBoxData);

                

                ShowWindow(pComboBoxData->hListBoxWnd, SW_HIDE);

                

                if (pComboBoxData->wState == CBS_DROPDOWNLIST)

                {

                    int     nLength, wIndex;

                    char    str[SINGLE_DEFLENGTH];

                    

                    nLength = 0;

                    wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 0, 0);

                    

                    if (wIndex != LB_ERR)

                    {

                        nLength = SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

                            (WPARAM)wIndex, (LPARAM)0);

                    }

                    

                    if (nLength)

                    {

                        if (nLength >= SINGLE_DEFLENGTH)

                            break;

                        

                        SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, (WPARAM)wIndex, 

                            (LPARAM)str);

                        

                        SetCtlColorComboBox(hWnd, hdc);

                        PaintText(hWnd, hdc, str, GetFocus() == hWnd);

                    }

                }

                

                pComboBoxData->bDropDown = FALSE;

                ReleaseDC(hWnd, hdc);

                ReleaseCapture();

            }

        }

        break;

    case VK_PAGEUP:

    case VK_PAGEDOWN:

        {

            BOOL bDown;



            bDown = pComboBoxData->bDropDown;

            SendMessage(pComboBoxData->hListBoxWnd, wMsgCmd, wParam, lParam);

            pComboBoxData->bDropDown = bDown;

            if (!bDown)

                ShowWindow(pComboBoxData->hListBoxWnd, SW_HIDE);

        }

        

        

        break;

#endif // NOKEYBOARD

        

    case VK_ESCAPE:

        if ((pComboBoxData->wState != CBS_SIMPLE) && 

            pComboBoxData->bDropDown)

        {

            PostMessage(hWnd, WM_ENDLOOP, 0, 0);

        }

/*  

        else

        {

            dwStyle = GetWindowLong(hWnd, GWL_STYLE);

            

            if (dwStyle & WS_CHILD)

                hWndParent = GetParent(hWnd);

            else

                hWndParent = GetWindow(hWnd, GW_OWNER);

            

            PostMessage(hWndParent, wMsgCmd, wParam, lParam);

            PostMessage(hWndParent, WM_KEYUP, wParam, lParam);

        }

*/

        break;



    case VK_TAB:



        if ((pComboBoxData->wState != CBS_SIMPLE) && 

            pComboBoxData->bDropDown)

        {

            PostMessage(hWnd, WM_ENDLOOP, 0, 0);

        }

/*当Combo处于下拉状态时,由于不向父窗口转发,只能把下拉框收起,不能切换焦点.

        dwStyle = GetWindowLong(hWnd, GWL_STYLE);

        

        if (dwStyle & WS_CHILD)

            hWndParent = GetParent(hWnd);

        else

            hWndParent = GetWindow(hWnd, GW_OWNER);

        

        PostMessage(hWndParent, wMsgCmd, wParam, lParam);

        PostMessage(hWndParent, WM_KEYUP, wParam, lParam);

*/

        break;



    case VK_RETURN:




        if (!pComboBoxData->bDropDown)

        {

//            SendMessage(hWnd, CB_SHOWDROPDOWN, TRUE, 0);
            SendMessage(GetParent(hWnd), wMsgCmd, wParam, lParam);

            return;

        }
        else

        {

            SendMessage(hWnd, CB_SHOWDROPDOWN, FALSE, 0);

            SendMessage(GetParent(hWnd), WM_COMMAND, 

			    MAKEWPARAM(pComboBoxData->wID, CBN_SELENDOK), (LPARAM)hWnd); 

            return;

        }



       /* else

        {

            hWndParent = GetParent(hWnd);

            SendMessage(hWndParent, WM_KEYDOWN, wParam, lParam);

        }

        if (pComboBoxData->wState != CBS_DROPDOWNLIST && 

            pComboBoxData->bAddNewString)

        {

            AddOrFindString(hWnd, pComboBoxData, TRUE);

            pComboBoxData->bAddNewString = FALSE;

        }



        if ((pComboBoxData->wState != CBS_SIMPLE) && 

            pComboBoxData->bDropDown)

        {

            PostMessage(hWnd, WM_ENDLOOP, 0, 0);

        }

		

		SendMessage(GetParent(hWnd), WM_COMMAND, 

			MAKEWPARAM(pComboBoxData->wID, CBN_SELENDOK), (LPARAM)hWnd);  



		//PostMessage(hWndParent, wMsgCmd, wParam, lParam);//Must use PostMessage.

		//PostMessage(hWndParent, WM_KEYUP, wParam, lParam);*/

        break;



    case VK_F5:

        if (!pComboBoxData->bDropDown)

        {

            SendMessage(hWnd, CB_SHOWDROPDOWN, TRUE, 0);

            return;

        }

        else

        {

            hWndParent = GetParent(hWnd);

            SendMessage(hWnd, WM_KEYDOWN, VK_RETURN, lParam);

            SendMessage(hWndParent, PWM_SETSCROLLSTATE, SCROLLUP | SCROLLDOWN,

                MASKUP | MASKDOWN);

        }

        break;



    case VK_F10:

        if (pComboBoxData->bDropDown)

        {

            SendMessage(hWnd, CB_SHOWDROPDOWN, FALSE, 0);

            return;

        }

        else

        {

            hWndParent = GetParent(hWnd);

            SendMessage(hWndParent, WM_KEYDOWN, wParam, lParam);

        }

        break;

        

    default:

        break;

    }

}



static BOOL InitCombo(HWND hWnd, PCOMBOBOXDATA pComboBoxData, LPARAM lParam)

{

    PCREATESTRUCT   pCreateStruct;

    RECT            rect, rcParent, rcWindow, rcClient, rcCreateVal; 

	SIZE            size;

    int             surplus = 0, n3DBorder, nGap, nListBorder;

	int             nBitsPixel, nArrowMore, nWidth, nScreenHeight;

	DWORD	        dwStyle, dwState;

    IWORD           y;//Is signed.

    HDC             hdc;



    pCreateStruct = (PCREATESTRUCT)lParam;

    

    //Initializes the internal data.

    memset(pComboBoxData, 0, sizeof(COMBOBOXDATA));

    

    pComboBoxData->wID = (WORD)(DWORD)pCreateStruct->hMenu;

    

    nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);

    hdc = GetDC(hWnd);

    GetTextExtentPoint32(hdc, "TEXT", -1, &size);

    ReleaseDC(hWnd, hdc);

    

    GetClientRect(hWnd, &rect);

    

    //MS windows define CBS_DROPDOWNLIST as 0x0011, but we define it as 0x0100.

    if ((pCreateStruct->style & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)

        pComboBoxData->wState = CBS_DROPDOWNLIST;

    else if (pCreateStruct->style & CBS_DROPDOWN)//0x0010

        pComboBoxData->wState = CBS_DROPDOWN;

    else if (pCreateStruct->style & CBS_SIMPLE)//0x0001

        pComboBoxData->wState = CBS_SIMPLE;

    

    GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);

    GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_SCREEN);

    GetWindowRectEx(GetParent(hWnd), &rcParent, W_WINDOW, XY_SCREEN);



#if (INTERFACE_MONO)



    nGap = 1;

    n3DBorder = 0;

    nArrowMore = 1;



#else // INTERFACE_MONO

    

    if (pComboBoxData->wState == CBS_SIMPLE)

    {

        if (nBitsPixel == 2)

            nGap = 1;

        else

            nGap = 0;

    }

    else

        nGap = 2;

    

    if (nBitsPixel == 1)

    {

        n3DBorder = 0;

        nArrowMore = 0;

    }

    else if (nBitsPixel == 2)

    {

        n3DBorder = 1;

        nArrowMore = 6;//3D border.

    }

    else

    {

        n3DBorder = 2;

        nArrowMore = 9;

    }



#endif // INTERFACE_MONO

    

    if (rect.bottom < (size.cy + COMBO_EDIT_HEIGHT_MORE) * 2 + 

        2 * n3DBorder + nGap)

    {

        rect.bottom = (size.cy + COMBO_EDIT_HEIGHT_MORE) * 2 + 

            2 * n3DBorder + nGap;

        

        pCreateStruct->cy = (size.cy + COMBO_EDIT_HEIGHT_MORE) * 2 + 

            2 * n3DBorder + nGap + rcClient.top - rcWindow.top + 

            rcWindow.bottom - rcClient.bottom;

        rcWindow.bottom = (size.cy + COMBO_EDIT_HEIGHT_MORE) * 2 + 

            2 * n3DBorder + nGap +

            rcWindow.top + rcWindow.bottom - rcClient.bottom;

    }

    

    // 2 is the two border height of listbox.

    nListBorder = 2;

    if (nBitsPixel != 1 && nBitsPixel != 2 &&

        pComboBoxData->wState == CBS_SIMPLE)

    {

        nListBorder += 4;

    }

    

    if ((rect.bottom - rect.top - 2 * n3DBorder) > 

        (size.cy + COMBO_EDIT_HEIGHT_MORE))

    {

        surplus = (rect.bottom - rect.top - nListBorder - 

            (2 * n3DBorder + nGap)) % (size.cy + COMBO_EDIT_HEIGHT_MORE);

    }

    

    rect.bottom -= surplus;

    rcClient.bottom -= surplus;

    rcWindow.bottom -= surplus;

    

    rcCreateVal.left = rect.left + n3DBorder;

    rcCreateVal.top = rect.top + n3DBorder;

    rcCreateVal.right = rect.right - n3DBorder;

    rcCreateVal.bottom = rect.bottom;

    

    if (rcCreateVal.left > rcCreateVal.right)

        rcCreateVal.right = rcCreateVal.left;

    

    //To color or 4-gray 3D effect,

    //left and top border are 2 pixel, right and bottom border are 2 pixel. 

    dwState = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | ES_COMBOEDIT;

    

    if(pCreateStruct->style & CBS_AUTOHSCROLL)

        dwState |= ES_AUTOHSCROLL;

    

    if (pComboBoxData->wState == CBS_DROPDOWN)

    {

        nWidth = rcCreateVal.right - rcCreateVal.left - ARROW_BMP_WIDTH - 

            nArrowMore;

        

        if (nWidth < 0)

            nWidth = 0;

       

        pComboBoxData->hEditWnd = CreateWindow("EDIT", "", dwState,

            rcCreateVal.left, rcCreateVal.top, nWidth, 

            size.cy + COMBO_EDIT_HEIGHT_MORE, hWnd, NULL, NULL, NULL);



        if (!pComboBoxData->hEditWnd)

            return FALSE;

    }

    else if (pComboBoxData->wState == CBS_SIMPLE)

    {

        nWidth = rcCreateVal.right - rcCreateVal.left;

        if (nWidth < 0)

            nWidth = 0;

        

        pComboBoxData->hEditWnd = CreateWindow("EDIT", "", dwState,

            rcCreateVal.left, rcCreateVal.top, nWidth, 

            size.cy + COMBO_EDIT_HEIGHT_MORE, hWnd, NULL, NULL, NULL);



        if (!pComboBoxData->hEditWnd)

            return FALSE;

    }

    

    nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    y = (IWORD)rect.bottom - rect.top - (size.cy + 

        COMBO_EDIT_HEIGHT_MORE + 2 * n3DBorder + nGap);

    

    if (y > nScreenHeight)

        y = (IWORD)nScreenHeight;

    

    dwStyle = WS_BORDER | LBS_HASSTRINGS | WS_VSCROLL;



    if (pCreateStruct->style & CBS_SORT)

        dwStyle |= LBS_SORT;



    if (pCreateStruct->style & CBS_USERSTRINGS)

        dwStyle |= LBS_USERSTRINGS;



    //CBS_AUTOHSCROLL, CBS_HASSTRINGS, WS_VSCROLL, CBS_DISABLENOSCROLL directly 

    //effect the style of edit and listbox.

    if (pComboBoxData->wState != CBS_SIMPLE)

    {

        dwStyle |= LBS_COMBOLBOX | WS_TOPMOST;

        //Create a invisible popup listbox.

        pComboBoxData->hListBoxWnd = CreateWindow("LISTBOX", "",

            dwStyle,

            rcClient.left, 

            rcClient.top + size.cy + COMBO_EDIT_HEIGHT_MORE + 

            nGap + 2 * n3DBorder + rcClient.top - rcWindow.top, 

            rect.right - rect.left, 

            y, 

            hWnd, NULL, NULL, NULL);



        if (!pComboBoxData->hListBoxWnd)

        {

            return FALSE;

        }

    }

    else

    {

        dwStyle |= WS_CHILD | WS_VISIBLE ;

        

        pComboBoxData->hListBoxWnd = CreateWindow("LISTBOX", "",

            dwStyle,

            rect.left, 

            rect.top + size.cy + COMBO_EDIT_HEIGHT_MORE + 

            nGap + 2 * n3DBorder, 

            rect.right - rect.left, 

            y, 

            hWnd, NULL, NULL, NULL);



        if (!pComboBoxData->hListBoxWnd)

            return FALSE;

    }

    

    //Decrease the combo height, so the background is the color of 

    //window or dialog. Also let combobox border disappear. 



    // Creating succeed.

    return TRUE;

}



static void ProcessPenDown(HWND hWnd, PCOMBOBOXDATA pComboBoxData, UINT wMsgCmd, 

                           WPARAM wParam, LPARAM lParam)

{

    IWORD           x, y;//Is signed.

    RECT            rect, rect1, rcListWindow, rcParent, rcWindow, rcClient; 

    HDC             hdc;

	

    HWND            hWndFocus;

    int             n3DBorder, nGap;

	SIZE            size;

	int             nArrowMore, nScreenHeight;

#if (!INTERFACE_MONO)

    int             nBitsPixel;

#endif

	

//#ifndef _EMULATE_

//	if (IsWindowEnabled(hWnd))

//		

//#endif //_EMULATE_



    x = LOWORD(lParam);

    y = HIWORD(lParam);

    

    GetClientRect(hWnd, &rect);

    

    hdc = GetDC(hWnd);

    GetTextExtentPoint32(hdc, "TEXT", -1, &size);



#if (INTERFACE_MONO)



    nArrowMore = 2;



#else // INTERFACE_MONO



    nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);

    

    if (nBitsPixel == 1)

        nArrowMore = 0;

    else if (nBitsPixel == 2)

        nArrowMore = 6;//3D border.

    else

        nArrowMore = 9;



#endif

    

    if (pComboBoxData->wState == CBS_DROPDOWNLIST)

    {

        SetRect(&rect1, rect.left, rect.top, rect.right, 

            rect.top + size.cy + 3);

    }

    else if (pComboBoxData->wState == CBS_DROPDOWN)

    {

        SetRect(&rect1, rect.right - ARROW_BMP_WIDTH - nArrowMore, 

            rect.top, rect.right, rect.top + size.cy + 3);

    }

    

    if ((pComboBoxData->wState == CBS_DROPDOWN || 

        pComboBoxData->wState == CBS_DROPDOWNLIST))

    {

        RECT rcCombo, rcList, rcListClient, rcListOld;

        int nComboTop, nComboBottom, nListTop, nListBottom;

        

#if (INTERFACE_MONO)



        n3DBorder = 0;



#else // INTERFACE_MONO



        if (nBitsPixel == 1) 

            n3DBorder = 0;

        else if (nBitsPixel == 2)

            n3DBorder = 1;

        else

            n3DBorder = 2;



        if (nBitsPixel != 1)

        {

            RECT rcArrow;



            nArrowMore = 5;

            if (nBitsPixel == 2)

            {

                nArrowMore = 2;

            }

            

            SetRect(&rcArrow, rect.right - n3DBorder - ARROW_BMP_WIDTH - 

                nArrowMore - 4, 

                rect.top + n3DBorder, rect.right - n3DBorder, 

                rect.top + size.cy + COMBO_EDIT_HEIGHT_MORE + 

                n3DBorder);



            if (pComboBoxData->wState == CBS_DROPDOWN)

            {

                SetRect(&rect1, rcArrow.left, rcArrow.top, 

                    rcArrow.right, rcArrow.bottom);

            }



            if (PtInRectXY(&rcArrow, x, y) && !pComboBoxData->bDropDown)//Arrow region.

            {

                Draw3DArrowRect_W95(hdc, &rcArrow, TRUE, FALSE, nBitsPixel);

                pComboBoxData->bPenDown = TRUE;

            }

        }



#endif // INTERFACE_MONO



        GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_SCREEN);

        GetWindowRect(pComboBoxData->hListBoxWnd, &rcListWindow);

        GetClientRect(pComboBoxData->hListBoxWnd, &rcListClient);

        GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);

        GetWindowRectEx(GetParent(hWnd), &rcParent, W_CLIENT, XY_SCREEN);

        

        GetWindowRectEx(pComboBoxData->hListBoxWnd, &rcListOld, W_WINDOW, XY_SCREEN);

        nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

        

        nGap = 1;

        

		//列表项向上打开。

        if (rcWindow.bottom + rcListWindow.bottom - rcListWindow.top > nScreenHeight)

        {

            nListBottom = rcWindow.top - rcClient.top - nGap;

            nComboTop = nListTop = nListBottom - 

                (rcListClient.bottom - rcListClient.top);

            nComboBottom = rect.bottom;

            

            //向上打开高度也不够，则列表项从顶部开始。

            if (nComboTop < -rcWindow.top) 

            {

                nComboTop = nListTop = -rcWindow.top ;

                nComboBottom = nListBottom = 

                    nComboTop + rcListWindow.bottom - rcListWindow.top;

            }

        }

        else

        {

            nComboTop = rect.top;

            nListTop = rect.top + size.cy + COMBO_EDIT_HEIGHT_MORE + 

                nGap + rcWindow.bottom - rcClient.bottom;

            nComboBottom = nListBottom = 

                nListTop + rcListWindow.bottom - rcListWindow.top;

        }

        

        SetRect(&rcCombo, rect.left, nComboTop, rect.right, nComboBottom);

        SetRect(&rcList, rect.left, nListTop, rect.right, nListBottom);

        

        if (PtInRectXY(&rect1, x, y))//Drop down region.

        {

            pComboBoxData->bDropDown = !pComboBoxData->bDropDown;

            

            if (pComboBoxData->bDropDown)

            {

                hWndFocus = GetFocus();

                if ((pComboBoxData->hEditWnd && 

                    hWndFocus != pComboBoxData->hEditWnd) || 

                    (!pComboBoxData->hEditWnd && 

                    hWndFocus != hWnd))

                {

                    SetFocus(hWnd);

                }

				//changed at 5/22

				if (pComboBoxData->wState == CBS_DROPDOWN)

				{

					AddOrFindString(hWnd, pComboBoxData, FALSE);

				}



                SendMessage(GetParent(hWnd), WM_COMMAND,

                    MAKELONG(pComboBoxData->wID, CBN_DROPDOWN), (LPARAM)hWnd);

                SetCapture(hWnd);

                

                //Show ListBox

                

                //If drop down list bottom is bigger than screen bottom, 

                //re arrange the popup listbox position.

                if (rcWindow.bottom + rcListWindow.bottom - 

                    rcListWindow.top > nScreenHeight)

                {

                    y = (IWORD)(rcWindow.top - (rcListWindow.bottom - 

                        rcListWindow.top) - nGap);

                    if (y < 0) 

                        y = 0;

                }

                else

                {

                    y = (IWORD)(rcClient.top + size.cy + COMBO_EDIT_HEIGHT_MORE + 

                        2 * n3DBorder + nGap + rcWindow.bottom - rcClient.bottom);

                }

                

                hWndFocus = GetFocus();

                if ((pComboBoxData->hEditWnd && 

                    hWndFocus != pComboBoxData->hEditWnd) || 

                    (!pComboBoxData->hEditWnd && 

                    hWndFocus != hWnd))

                {

                    pComboBoxData->bPenDown = FALSE;

                    pComboBoxData->bDropDown = FALSE;

                    ReleaseCapture();

                    return;

                }



                if (rcWindow.left != rcListOld.left || y != rcListOld.top || 

                    ((rect.right - rect.left) != (rcListOld.right - rcListOld.left)))

                {

                    MoveWindow(pComboBoxData->hListBoxWnd, 

                        rcWindow.left, y, rcWindow.right - rcWindow.left,

                        rcListWindow.bottom - rcListWindow.top, 

                        FALSE);

                }

                ShowWindow(pComboBoxData->hListBoxWnd, SW_SHOWNOACTIVATE);

                

                if (pComboBoxData->wState == CBS_DROPDOWNLIST)

                {

                    int     nLength = 0, wIndex;

                    char    str[SINGLE_DEFLENGTH];



                    wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 0, 0);

                    

                    if (wIndex != LB_ERR)

                    {

                        nLength = SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

                            (WPARAM)wIndex, (LPARAM)0);

                    }

                    

                    if (nLength)

                    {

                        if (nLength >= SINGLE_DEFLENGTH)

                            return;

                        

                        SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, (WPARAM)wIndex, 

                            (LPARAM)str);

                        

                        SetCtlColorComboBox(hWnd, hdc);

                        PaintText(hWnd, hdc, str, GetFocus() == hWnd);

                    }

                }

                

                ComboMessageLoop(hWnd, pComboBoxData);

                ShowWindow(pComboBoxData->hListBoxWnd, SW_HIDE);

                

                if (pComboBoxData->wState == CBS_DROPDOWNLIST)

                {

                    int     nLength, wIndex;

                    char    str[SINGLE_DEFLENGTH];

                    

                    nLength = 0;

                    wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 0, 0);

                    

                    if (wIndex != LB_ERR)

                    {

                        nLength = SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

                            (WPARAM)wIndex, (LPARAM)0);

                    }

                    

                    if (nLength)

                    {

                        if (nLength >= SINGLE_DEFLENGTH)

                            return;



                        SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, (WPARAM)wIndex, 

                            (LPARAM)str);

                        

                        SetCtlColorComboBox(hWnd, hdc);

                        PaintText(hWnd, hdc, str, GetFocus() == hWnd);

                    }

                }

                

                pComboBoxData->bDropDown = FALSE;

                ReleaseCapture();

            }

            else

            {

                PostMessage(hWnd, WM_ENDLOOP, 0, 0);

                

                if (pComboBoxData->wState == CBS_DROPDOWNLIST)

                {

                    int     nLength, wIndex;

                    char    str[SINGLE_DEFLENGTH];

                    

                    nLength = 0;

                    wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 0, 0);

                    

                    if (wIndex != LB_ERR)

                    {

                        nLength = SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

                            (WPARAM)wIndex, (LPARAM)0);

                    }

                    

                    if (nLength)

                    {

                        if (nLength >= SINGLE_DEFLENGTH)

                            return;



                        SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, (WPARAM)wIndex, 

                            (LPARAM)str);

                        

                        SetCtlColorComboBox(hWnd, hdc);

                        PaintText(hWnd, hdc, str, GetFocus() == hWnd);

                    }

                }

            }

        }

        else if(PtInRectXY(&rcList, x, y))//In listbox region.

        {

            RECT rcListScreen;

            

            GetWindowRectEx(pComboBoxData->hListBoxWnd, &rcListScreen, 

                W_CLIENT, XY_SCREEN);

            

            x -= (IWORD)(rcListScreen.left - rcClient.left);

            y -= (IWORD)(rcListScreen.top - rcClient.top);

            SendMessage(pComboBoxData->hListBoxWnd, wMsgCmd, wParam, MAKELPARAM(x, y));

        }

        else //In edit region or outside of comboBox.

        {

            PostMessage(hWnd, WM_ENDLOOP, 0, 0);

        }

    }



    ReleaseDC(hWnd, hdc);

}



static void ProcessCommand(HWND hWnd, PCOMBOBOXDATA pComboBoxData, 

                           WPARAM wParam, LPARAM lParam)

{

    HDC             hdc;

	IWORD	        wIndex;

    HWND            hWndChild;

    WORD	        msgStyle;

    int             nLength;

    char            str[SINGLE_DEFLENGTH];



    msgStyle = HIWORD(wParam);

    hWndChild = (HWND)lParam;

    

    switch (msgStyle)

    {

    case LBN_SELCHANGE: 

        

        if (!hWndChild && !(pComboBoxData->wState & CBS_DROPDOWNLIST))

        {

            //Point not in rcComboLBox, need restore the item 

            //whose text same as ComboEdit.

            AddOrFindString(hWnd, pComboBoxData, FALSE);

            break;

        }

        

        nLength = 0;

        wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 0, 0);

        

        if (wIndex != LB_ERR)

        {

            nLength = SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

                (WPARAM)wIndex, (LPARAM)0);

        }

        

        if (nLength)

        {

            if (nLength >= SINGLE_DEFLENGTH)

                break;

            if (pComboBoxData->wState == CBS_SIMPLE)

				pComboBoxData->bSelectInSimple = 1;



            SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, (WPARAM)wIndex, 

                (LPARAM)str);

			

            SendMessage(pComboBoxData->hListBoxWnd, LB_SETTOPINDEX, (WPARAM)wIndex, 0);

            

            if (pComboBoxData->wState == CBS_DROPDOWNLIST && lParam)

            {

                hdc = GetDC(hWnd);

                

                SetCtlColorComboBox(hWnd, hdc);

                PaintText(hWnd, hdc, str, GetFocus() == hWnd);

                

                ReleaseDC(hWnd, hdc);

            }

            else if (!(pComboBoxData->wState == CBS_DROPDOWNLIST))

            {

                SendMessage(pComboBoxData->hEditWnd, WM_SETTEXT, (WPARAM)0,

                    (LPARAM)str);

                

                if (hWndChild == pComboBoxData->hListBoxWnd)

                {

                    SendMessage(pComboBoxData->hEditWnd, EM_SETSEL, (WPARAM)0,

                        (LPARAM)-1);

                }

            }

        }

        

        SendMessage(GetParent(hWnd), WM_COMMAND, 

            MAKELONG(pComboBoxData->wID, CBN_SELCHANGE), (LPARAM)hWnd);

        

        if (pComboBoxData->bDropDown)

            SetCapture(hWnd);



        //InvalidateRect(hWnd, NULL, TRUE);			

        

        break;

        

    case EN_CHANGE :

        

        pComboBoxData->bAddNewString = TRUE;



		if (pComboBoxData->wState == CBS_DROPDOWN)

			ShowWindow(pComboBoxData->hListBoxWnd, SW_HIDE);

		//changed 5/22

			/*

        if (pComboBoxData->wState != CBS_DROPDOWNLIST && 

            pComboBoxData->bAddNewString)

        {

            AddOrFindString(hWnd, pComboBoxData, FALSE);

        }

        */

        if (!pComboBoxData->bSelectInSimple && (pComboBoxData->wState & CBS_SIMPLE))

		{

			SendMessage(pComboBoxData->hListBoxWnd, LB_SETCURSEL, -1, 0);

		}

		pComboBoxData->bSelectInSimple = 0;



        SendMessage(GetParent(hWnd), WM_COMMAND, 

            MAKELONG(pComboBoxData->wID, CBN_EDITCHANGE), (LPARAM)hWnd);



        if (pComboBoxData->bDropDown)

            SetCapture(hWnd);



		if (pComboBoxData->wState == CBS_SIMPLE)

			SetFocus(pComboBoxData->hEditWnd);



        break;

        

    case EN_UPDATE :

        

        SendMessage(GetParent(hWnd), WM_COMMAND, 

            MAKELONG(pComboBoxData->wID, CBN_EDITUPDATE), (LPARAM)hWnd);



        if (pComboBoxData->bDropDown)

            SetCapture(hWnd);



        break;

        

    case LBN_SETFOCUS :

        

        if (!pComboBoxData->bHasSetFocus)

        {

            pComboBoxData->bHasSetFocus = TRUE;

            SendMessage(GetParent(hWnd), WM_COMMAND, 

                MAKELONG(pComboBoxData->wID, CBN_SETFOCUS), (LPARAM)hWnd);

        }

        break;

        

    case LBN_KILLFOCUS :

        

        //Here(only in ComboBox) lParam is handle of window receiving focus.

        if ((HWND)lParam != pComboBoxData->hEditWnd)

        {

            if (pComboBoxData->wState != CBS_SIMPLE)

                PostMessage(hWnd, WM_ENDLOOP, 0, 0);

            

            pComboBoxData->bHasSetFocus = FALSE;

            

            if (pComboBoxData->wState != CBS_DROPDOWNLIST && 

                pComboBoxData->bAddNewString)

            {

                AddOrFindString(hWnd, pComboBoxData, TRUE);

                pComboBoxData->bAddNewString = FALSE;

            }



            PostMessage(GetParent(hWnd), WM_COMMAND, 

                MAKELONG(pComboBoxData->wID, CBN_KILLFOCUS), (LPARAM)hWnd);

        }

        break;

        

    case EN_SETFOCUS :

        

        if (!pComboBoxData->bHasSetFocus)

        {

            pComboBoxData->bHasSetFocus = TRUE;

			SendMessage(pComboBoxData->hEditWnd, EM_SETSEL, (WPARAM)0,

				(LPARAM)-1);

            SendMessage(GetParent(hWnd), WM_COMMAND, 

                MAKELONG(pComboBoxData->wID, CBN_SETFOCUS), (LPARAM)hWnd);

        }

        break;

        

    case EN_KILLFOCUS :

        

        //For LBS_DROPDOWN and LBS_SIMPLE, when kill focus,

        //current selection is removed.

        if (pComboBoxData->wState != CBS_DROPDOWNLIST)

        {

            SendMessage(pComboBoxData->hEditWnd, EM_SETSEL, (WPARAM)-1,

                (LPARAM)0);

        }

        

        //Here(only in ComboBox) lParam is handle of window receiving focus.

        if ((HWND)lParam != hWnd && (HWND)lParam != pComboBoxData->hListBoxWnd)

        {

            if (pComboBoxData->wState != CBS_SIMPLE)

                PostMessage(hWnd, WM_ENDLOOP, 0, 0);

            

            if (pComboBoxData->wState != CBS_DROPDOWNLIST && 

                pComboBoxData->bAddNewString)

            {

                AddOrFindString(hWnd, pComboBoxData, TRUE);

                pComboBoxData->bAddNewString = FALSE;

            }

            

            pComboBoxData->bHasSetFocus = FALSE;

            SendMessage(GetParent(hWnd), WM_COMMAND, 

                MAKELONG(pComboBoxData->wID, CBN_KILLFOCUS), (LPARAM)hWnd);

        }

        break;

        

    default:

        break;

    }

}



static HBRUSH SetCtlColorComboBox(HWND hWnd, HDC hdc)

{

    HBRUSH  hBrush;



#if (!NOSENDCTLCOLOR)

    HWND    hWndParent;

    DWORD   dwStyle;



    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    

    if (dwStyle & WS_CHILD)

        hWndParent = GetParent(hWnd);

    else

        hWndParent = GetWindow(hWnd, GW_OWNER);

    

    if (!hWndParent)

    {

#endif

        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));

        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));

        hBrush = (HBRUSH)(COLOR_WINDOW + 1);

#if (!NOSENDCTLCOLOR)

    }

    else

    {

        hBrush = (HBRUSH)SendMessage(hWndParent, WM_CTLCOLOREDIT, 

            (WPARAM)hdc, (LPARAM)hWnd);

    }

#endif



    return hBrush;

}



static void SelChange(HWND hWnd, PCOMBOBOXDATA pComboBoxData)

{

    HDC             hdc;

    IWORD	        wIndex;

    int             nLength;

    char            str[SINGLE_DEFLENGTH];

    

    nLength = 0;

    wIndex = (IWORD)SendMessage(pComboBoxData->hListBoxWnd, LB_GETCURSEL, 0, 0);

    

    if (wIndex != LB_ERR)

    {

        nLength = SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXTLEN, 

            (WPARAM)wIndex, (LPARAM)0);

    }

    

    if (nLength)

    {

        if (nLength >= SINGLE_DEFLENGTH)

            return ;

        

        SendMessage(pComboBoxData->hListBoxWnd, LB_GETTEXT, (WPARAM)wIndex, 

            (LPARAM)str);

        

        if (pComboBoxData->wState == CBS_DROPDOWNLIST)

        {

            hdc = GetDC(hWnd);

            

            SetCtlColorComboBox(hWnd, hdc);

            PaintText(hWnd, hdc, str, GetFocus() == hWnd);

            

            ReleaseDC(hWnd, hdc);

        }

        else

        {

            SendMessage(pComboBoxData->hEditWnd, WM_SETTEXT, (WPARAM)0,

                (LPARAM)str);

        }

    }

}

