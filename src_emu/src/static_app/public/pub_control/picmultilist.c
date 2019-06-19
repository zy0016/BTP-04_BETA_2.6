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

//#include "hpwin.h"
//#include "control.h"
//#include "hpfile.h"
#include "string.h"
#include "stdio.h"
#include "plx_pdaex.h"
#include "winpda.h"
#include "malloc.h"

typedef struct
{
    int     nMaxCount;
    int		nMaxSelCount;
	int		nCurSelCount;
	BYTE *	pbSelected;
}MULTILISTBOXDATA, *PMULTILISTBOXDATA;

static HBITMAP hBmpSelected = NULL, hBmpUnselected = NULL,
hbmpFocusSel = NULL, hbmpFocusUnsel = NULL;
#define MAX_PATH 256
#define LARROW_WIDTH	12
#define	LARROW_HEIGHT	12
#define LocalAlloc(flag, size) malloc(size)
#define LocalFree(handle) free(handle)
static LRESULT CALLBACK PicMultiList_Proc(HWND hWnd, UINT uMsgCmd, 
                                 WPARAM wParam, LPARAM lParam);
extern int PLXOS_GetResourceDirectory(char *pathname);

BOOL PICMULTILIST_RegisterClass(void)
{
    WNDCLASS    wc;
    char PathName[MAX_PATH];
    int  nPathLen;

    GetClassInfo(NULL, "PICLIST", &wc);
	
	wc.style            = CS_OWNDC;
    wc.lpfnWndProc      = PicMultiList_Proc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = wc.cbWndExtra + sizeof(MULTILISTBOXDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "PICMULTILIST";

    if (!RegisterClass(&wc))
        return FALSE;

    PLXOS_GetResourceDirectory(PathName);
    nPathLen = strlen(PathName);
    strcat(PathName, "select.bmp");
    hBmpSelected = LoadImage(NULL, PathName,IMAGE_BITMAP, LARROW_WIDTH,
        LARROW_HEIGHT, LR_LOADFROMFILE);
    PathName[nPathLen] = '\0';
    strcat(PathName, "unselect.bmp");
    hBmpUnselected = LoadImage(NULL, PathName,IMAGE_BITMAP, LARROW_WIDTH, 
        LARROW_HEIGHT, LR_LOADFROMFILE);
    PathName[nPathLen] = '\0';
    strcat(PathName, "select_focus.bmp");
    hbmpFocusSel = LoadImage(NULL, PathName,IMAGE_BITMAP, LARROW_WIDTH,
        LARROW_HEIGHT, LR_LOADFROMFILE);
    PathName[nPathLen] = '\0';
    strcat(PathName, "unselect_focus.bmp");
    hbmpFocusUnsel = LoadImage(NULL, PathName,IMAGE_BITMAP, LARROW_WIDTH, 
        LARROW_HEIGHT, LR_LOADFROMFILE);
    return TRUE;
}

static LRESULT CALLBACK PicMultiList_Proc(HWND hWnd, UINT uMsgCmd, 
                                 WPARAM wParam, LPARAM lParam)
{
	PMULTILISTBOXDATA	pMultiListBoxData;
	WNDCLASS			WndClassList;
	WNDPROC				WndProcList;
	LRESULT				lResult;
	int					nCurIndex, i, j, nIndexLast = 0;
	static int			nMaxCount;
    WORD                cItems;
    PINT                lpnItems;
    BYTE                *p;
    BOOL                bSelected = FALSE, bSelLast = FALSE;

	GetClassInfo(NULL, "PICLIST", &WndClassList);
	pMultiListBoxData = (PMULTILISTBOXDATA)((BYTE*)GetUserData(hWnd) + 
		WndClassList.cbWndExtra);
    WndProcList	= WndClassList.lpfnWndProc;
    if ((uMsgCmd == WM_KEYDOWN) && (wParam == VK_DOWN || wParam == VK_UP)
        || (uMsgCmd == LB_SETCURSEL))
        {
            nIndexLast = CallWindowProc(WndProcList, hWnd, LB_GETCURSEL, 0, 0);
        if (nIndexLast != LB_ERR)
            bSelLast = CallWindowProc(PicMultiList_Proc, hWnd, LB_GETSEL, nIndexLast, 0);
    }
	lResult = CallWindowProc(WndProcList, hWnd, uMsgCmd, wParam, lParam);
	switch(uMsgCmd)
	{
    case WM_CREATE:
        pMultiListBoxData->nMaxCount = 0;
        pMultiListBoxData->nCurSelCount = 0;
        pMultiListBoxData->nMaxSelCount = 0;
        pMultiListBoxData->pbSelected = NULL;
        break;
	case LB_ADDSTRING:
	case LB_INSERTSTRING:
		if (lResult >= 0)
			CallWindowProc(WndProcList, hWnd, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, lResult), (LPARAM)hBmpUnselected);
		break;

	case LB_MAXSELCOUNT:
		pMultiListBoxData->nMaxSelCount = lParam;
		break;

	case LB_ENDINIT:
		nMaxCount = CallWindowProc(WndProcList, hWnd, LB_GETCOUNT, 0, 0);
        pMultiListBoxData->nMaxCount = nMaxCount;
		pMultiListBoxData->pbSelected = (BYTE*)LocalAlloc(LMEM_FIXED, 
			nMaxCount * sizeof(BYTE));
		memset(pMultiListBoxData->pbSelected, 0, nMaxCount * sizeof(BYTE));
		break;

	case WM_PENUP:
	case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_DOWN:
        case VK_UP:
            nCurIndex = CallWindowProc(WndProcList, hWnd, LB_GETCURSEL, 0, 0);
            bSelected = CallWindowProc(PicMultiList_Proc, hWnd, LB_GETSEL,
                nCurIndex, 0);
            if (nIndexLast != LB_ERR)
            {
            CallWindowProc(WndProcList, hWnd, LB_SETIMAGE, 
                MAKEWPARAM(IMAGE_BITMAP, nIndexLast),
                (LPARAM)(bSelLast ? hBmpSelected : hBmpUnselected));
            }
            CallWindowProc(WndProcList, hWnd, LB_SETIMAGE, 
                MAKEWPARAM(IMAGE_BITMAP, nCurIndex),
                (LPARAM)(bSelected ? hbmpFocusSel : hbmpFocusUnsel));
			break;
            
        case VK_F5:
		nCurIndex = CallWindowProc(WndProcList, hWnd, LB_GETCURSEL, 0, 0);
		lResult = nCurIndex; 
		if (nCurIndex >= pMultiListBoxData->nMaxCount)
			nCurIndex = pMultiListBoxData->nMaxCount - 1;
		if (*(pMultiListBoxData->pbSelected + nCurIndex))
		{
			*(pMultiListBoxData->pbSelected + nCurIndex) = 0;
			pMultiListBoxData->nCurSelCount --;
			CallWindowProc(WndProcList, hWnd, LB_SETIMAGE, 
                    MAKEWPARAM(IMAGE_BITMAP, lResult), (LPARAM)hbmpFocusUnsel);
		}
		else
		{
			if (pMultiListBoxData->nMaxSelCount &&
				pMultiListBoxData->nCurSelCount >= pMultiListBoxData->nMaxSelCount)
			{
				SendMessage(GetParent(hWnd), WM_COMMAND, 
					MAKELONG(GetDlgCtrlID(hWnd), LBN_MAXSEL), (LPARAM)hWnd);
				break;
			}
			pMultiListBoxData->nCurSelCount ++;			
			*(pMultiListBoxData->pbSelected + nCurIndex) = 1;
			CallWindowProc(WndProcList, hWnd, LB_SETIMAGE, 
                    MAKEWPARAM(IMAGE_BITMAP, lResult), (LPARAM)hbmpFocusSel);
            }
            break;
            
        default:
            break;
		}
		break;

    case LB_GETSEL:
        nCurIndex = (IWORD)wParam;
        if (nCurIndex >= 0 && nCurIndex < pMultiListBoxData->nMaxCount)
            lResult = *(pMultiListBoxData->pbSelected + nCurIndex);
        else
            lResult = (LRESULT)LB_ERR;
        break;

    case LB_SETSEL:
        nCurIndex = (int)lParam;
        bSelected = (BOOL)wParam;
        if ((0 <= nCurIndex) && (nCurIndex <= pMultiListBoxData->nMaxCount - 1))
        {
            lResult = 0;
            if (*(pMultiListBoxData->pbSelected + nCurIndex) == bSelected)
                break;

            if (bSelected)
            {
                pMultiListBoxData->nCurSelCount++;
            }
            else
            {
                pMultiListBoxData->nCurSelCount--;
            }

            *(pMultiListBoxData->pbSelected + nCurIndex) = bSelected;

            if (nCurIndex == CallWindowProc(WndProcList, hWnd, LB_GETCURSEL,
                0, 0))
            {
                CallWindowProc(WndProcList, hWnd, LB_SETIMAGE,
                    MAKEWPARAM(IMAGE_BITMAP, nCurIndex),
                    (LPARAM)((bSelected) ? hbmpFocusSel : hbmpFocusUnsel));
            }
            else
            {
                CallWindowProc(WndProcList, hWnd, LB_SETIMAGE,
                    MAKEWPARAM(IMAGE_BITMAP, nCurIndex),
                    (LPARAM)((bSelected) ? hBmpSelected : hBmpUnselected));
            }
        }
        else
        {
            lResult = (LRESULT)LB_ERR;
        }
        break;

    case LB_SETCURSEL:
        nCurIndex = (int)wParam;
        bSelected = CallWindowProc(PicMultiList_Proc, hWnd, LB_GETSEL,
            nCurIndex, 0);
        if (nIndexLast != LB_ERR)
        {
            CallWindowProc(WndProcList, hWnd, LB_SETIMAGE, 
                MAKEWPARAM(IMAGE_BITMAP, nIndexLast),
                (LPARAM)(bSelLast ? hBmpSelected : hBmpUnselected));
        }
        CallWindowProc(WndProcList, hWnd, LB_SETIMAGE, 
            MAKEWPARAM(IMAGE_BITMAP, nCurIndex),
            (LPARAM)(bSelected ? hbmpFocusSel : hbmpFocusUnsel));
        break;

    case LB_GETSELCOUNT:
        lResult = pMultiListBoxData->nCurSelCount;
        break;
        
    case LB_GETSELITEMS:
        cItems = (WORD)wParam;           // maximum number of items 
        lpnItems = (PINT)lParam; // address of buffer 
        
        p = pMultiListBoxData->pbSelected;
        i = 0;
        j = 0;

        do
        {
            if (i >= pMultiListBoxData->nMaxCount)
                break;
            
            if (j >= cItems)
                break;

            if (*p)
            {
                *(lpnItems + j) = i;
                j++;
            }
            i++;
            p++;
        }while(1);

        lResult = (LRESULT)j;
        break;

	case WM_DESTROY:
        if (pMultiListBoxData->pbSelected)
		    LocalFree(pMultiListBoxData->pbSelected);
		break;
		
	default:
		break;

	}
	return lResult;
}
