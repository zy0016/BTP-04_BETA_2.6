/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#include <pubapp.h>

#include <plx_pdaex.h>
#include <imesys.h>
#include <string.h>
#include <stdlib.h>
#include <window.h>
#include <stdio.h>
#include <hpdef.h>

#include "exch_global.h"
#include "exch_currandrate.h"
#include "exch_file.h"


DWORD Exch_CurrAndRateWnd(HWND hWnd)
{
	WNDCLASS	    wc;
	RECT rClient;

	hWndBefore = hWnd;

	wc.style         = 0;
	wc.lpfnWndProc   = CurrAndRateWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ExchCurrAndRateWndClass";
	if (!RegisterClass(&wc))
	{
		UnregisterClass("ExchCurrAndRateWndClass", NULL);
		return FALSE;
	}
	GetClientRect(hExchFrameWnd, &rClient);
	hWndCurrAndRate=CreateWindow(
				"ExchCurrAndRateWndClass",
				"",//ML("Currencies & rates"),
				WS_VISIBLE | WS_CHILD,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hExchFrameWnd, NULL, NULL, NULL);

	if (!hWndCurrAndRate)
		return FALSE;
	
	ShowWindow(hExchFrameWnd, SW_SHOW);
	UpdateWindow(hExchFrameWnd); 
	return TRUE;
}
static LRESULT CurrAndRateWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;
	static HMENU hMenu;
	switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			HDC hdc;
			hdc = GetDC(hWnd);
			hBmpCurrency = LoadImage(NULL, ICON_CURRENCY, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			hBmpNewCurrency = LoadImage(NULL, ICON_NEWCURRENCY, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			ReleaseDC(hWnd, hdc);
		}
		CreateCurrAndRateList(hWnd);
		InitListBox(hWndList);
		SendMessage(hWndList, LB_SETCURSEL, 0, 0);
		hMenu = CreateMainWinMenu();
		hFocus = hWndList;	
		SetFocus(hFocus);
		
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)" ");
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SetWindowText(hExchFrameWnd,ML("Currencies & rates"));
		
		break;

	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;	

	case PWM_SHOWWINDOW:
		SetFocus(hFocus);
		{
			int nIndex;
			nIndex = SendMessage(hWndList, LB_GETCURSEL, 0, 0 );
			if (nIndex)
			{
				SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
				SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
			}
			else
			{
				SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");  
			}
			PDASetMenu(hExchFrameWnd, hMenu);
			SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
			SetWindowText(hExchFrameWnd, ML("Currencies & rates"));
		}
		break;
	
	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case ID_BACK:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			SendMessage(hExchFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			break;
		case ID_EDIT:
			{
				int nIndex;
				nIndex = SendMessage(hWndList, LB_GETCURSEL, 0, 0 );
				pExchNodeCur = (EXCHMONEYINFONODE *)SendMessage(hWndList, LB_GETITEMDATA, nIndex, 0);
				if (pExchNodeCur->ExchMoneyInfo.bBase) 
				{
					PLXTipsWin(hExchFrameWnd, hWnd, 0, ML("Cannot edit\nbase currency"), ML("Currencies & rates"), Notify_Alert, ML("Ok"), 0, WAITTIMEOUT);
				}
				else
					Exch_EditRateWnd();
				
			}
			break;
		case ID_EDIT_SAVEBACK:
			{
				int iIndex = 0;					
				InitListBox(hWndList);
				iIndex = SetCurrIndexItem(hWndList, EDIT);
				SendMessage(hWndList, LB_SETCURSEL, iIndex, 0);
			}
			break;
		case ID_NEW_SAVEBACK:
			{
				int iIndex = 0;		
			
				InitListBox(hWndList);
				iIndex = SetCurrIndexItem(hWndList, NEW);
				SendMessage(hWndList, LB_SETCURSEL, iIndex, 0);
			}
			break;
		case ID_DELEOK_BACK:
			{
				int iIndex = 0;			
				InitListBox(hWndList);
				if (DELETEALL == lParam || DELETEMANY == lParam)
				{
					SendMessage(hWndList, LB_SETCURSEL, 0, 0);
				}
				else if (DELETEONE == LOWORD(lParam))
				{
					SendMessage(hWndList, LB_SETCURSEL, HIWORD(lParam), 0);
				}
				
			}
			break;
		case ID_NEW:
			Exch_NewWnd();
			break;
		case ID_ASBASE:
			PLXConfirmWinEx(hExchFrameWnd, hWnd, ML("All rates will be\nrecalculated.\nSet anyway?"), Notify_Request,
				ML("Currencies & rates"), ML("Yes"),ML("No"), CONFIRM_RECLCLT);
			break;
		case ID_DELETE:
			{
				int nIndex;
				char cPrompt[CURRENCY_MAX_LENGTH + 10];
				nIndex = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
				pExchNodeCur = (EXCHMONEYINFONODE *)SendMessage(hWndList, LB_GETITEMDATA, nIndex, 0);
				
				if (pExchNodeCur->ExchMoneyInfo.bBase)
				{
					PLXTipsWin(0, 0, 0, ML("Cannot delete\nbase currency"),
						ML("Currencies & rates"), Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
				}
				else
				{
					strcpy(cPrompt, pExchNodeCur->ExchMoneyInfo.name);
					strcat(cPrompt, ":\n");
					strcat(cPrompt, ML("Delete?"));
					PLXConfirmWinEx(hExchFrameWnd, hWnd, cPrompt, Notify_Request, ML("Currencies & rates"), ML("Yes"), ML("No"), CONFIRM_DELETE);
				}
			}
		
			break;
		case ID_DELE_SELECT:
			{
				int iCount;
				
				iCount = SendMessage(hWndList, LB_GETCOUNT, 0, 0);

				if (iCount > 2)
				{
					Exch_Dele_SelectWin();
				}
				else
				{
					PLXTipsWin(0, 0, 0, ML("Cannot delete\nbase currency"),
						ML("Currencies & rates"), Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
				
				}
			}
			break;

		case ID_DELE_ALL:
			{
				int iCount;
				
				iCount = SendMessage(hWndList, LB_GETCOUNT, 0, 0);

				if (iCount > 2)
				{
					PLXConfirmWinEx(hExchFrameWnd, hWnd, ML("Delete\n all currencies?"), Notify_Request, ML("Currencies & rates"), ML("Yes"), ML("No"), CONFIRM_DELETEALL);
				}
			}
		
			break;
		default:
			break;
		}
		switch(HIWORD(wParam)) 
		{
		case LBN_SELCHANGE:
			{
				int nIndex;
				nIndex = SendMessage(hWndList, LB_GETCURSEL, 0, 0 );
				if (nIndex)
				{
					SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
					SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
				}
				else
				{
					SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
					SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");  
				}
			}
			break;
		}
		break;
	case CONFIRM_RECLCLT:
		{
			int nIndex;
			EXCHMONEYINFONODE *pNode;
			if (lParam)
			{
				nIndex = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
				pExchNodeCur = (EXCHMONEYINFONODE *)SendMessage(hWndList, LB_GETITEMDATA, nIndex, 0);
				
				pNode = pExchNodeHead;
				for(;pNode != NULL; pNode = pNode->pNextInfo)
				{
					pNode->ExchMoneyInfo.bBase = 0;
					pNode->ExchMoneyInfo.rate = 0;
				}
				pExchNodeCur->ExchMoneyInfo.bBase = 1;
				pExchNodeCur->ExchMoneyInfo.rate = 1;
				pExchNodeBase = pExchNodeCur;
				if(Exch_SaveAllNodeToFile())
				{
					PLXTipsWin(hExchFrameWnd, hWnd, CONFIRM_RECLCLT_OK, ML("Base currency\n set and rates\n recalculated"),
						ML("Currencies & rates"), Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
				}
				
			}
		}
		break;
	case CONFIRM_RECLCLT_OK:
	
	
		InitListBox(hWndList);
		SendMessage(hWndList, LB_SETCURSEL, 0, 0);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");  
		break;
	case CONFIRM_DELETE:
		{
			int iCurIndex;
			
			if (lParam)
			{
				if (pExchNodeFirst == pExchNodeCur) 
				{
					pExchNodeFirst = pExchNodeBase;
					pExchNodeFirst->ExchMoneyInfo.nDefault = 1;
				}
				if (pExchNodeSecond == pExchNodeCur) 
				{
					pExchNodeSecond = pExchNodeBase;
					pExchNodeSecond->ExchMoneyInfo.nDefault = 1;
				}
				iCurIndex = SetCurrIndexItem(hWndList ,DELETEONE);
				Exch_DeleteNode(pExchNodeCur);
				if(Exch_SaveAllNodeToFile())
				{
					PLXTipsWin(hExchFrameWnd, hWnd, 0, ML("Deleted"),ML("Currencies & rates"), 
						Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
					SendMessage(hWnd, WM_COMMAND, ID_DELEOK_BACK, MAKELPARAM(DELETEONE, iCurIndex));
				}
			}
		}
		break;
	case CONFIRM_DELETEALL:
		{
			if (lParam)
			{
				if(Exch_DeleteAllNodeToFile())
					PLXTipsWin(hExchFrameWnd, hWnd, 0, ML("Deleted"),ML("Currencies & rates"), 
							Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
				pExchNodeBase->ExchMoneyInfo.nDefault = 1;
				Exch_SaveAllNodeToFile();
				SendMessage(hWnd, WM_COMMAND, ID_DELEOK_BACK, MAKELPARAM(DELETEALL, 0));
			}
		}
		break;
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
			case VK_F10:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				SendMessage(hExchFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				PostMessage(hWndBefore, WM_COMMAND, ID_CURR_RATE_BACK, 0);
				break;
			case VK_F5:
				{
					int nIndex;
				
					nIndex = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
					if (0 == nIndex)
					{
						SendMessage(hWnd, WM_COMMAND, ID_NEW, NULL);
					}
					else
					{
						SendMessage(hExchFrameWnd, wMsgCmd, wParam, lParam);
					}

				}
				
				break;
			case VK_RETURN:
				{
					int curSel;

					curSel = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
					if (curSel > 0)
						SendMessage(hWnd, WM_COMMAND, ID_EDIT, 0);
				}
				break;

			default:
				lResult =  PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
		}
		break;
	case WM_CLOSE:
		DestroyMenu(hMenu);
		DeleteObject(hBmpCurrency);
		DeleteObject(hBmpNewCurrency);
		DestroyWindow(hWnd);  
		break;

    case WM_DESTROY:
		UnregisterClass("ExchCurrAndRateWndClass", NULL);
		break;

	default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

	return lResult;
}
static void CreateCurrAndRateList(HWND hWndParent)
{
	RECT rc;

	GetClientRect(hWndParent, &rc);
	hWndList = CreateWindow("LISTBOX",
		"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_MULTILINE | LBS_BITMAP| LBS_SORT,
		rc.left, rc.top,
		rc.right - rc.left, rc.bottom - rc.top,
		hWndParent,
		(HMENU)ID_MAINLIST,
		NULL,
		NULL);
}

static void InitListBox(HWND hWnd)
{
	EXCHMONEYINFONODE	*pNode;
	char	cNameCur[CURRENCY_MAX_LENGTH + 10];
	int		nIndex = 1;
	char	cRate[RATE_MAX_LENGTH+1];



	pNode = pExchNodeHead;
	if (NULL == pNode)
		return;
	SendMessage(hWnd, LB_RESETCONTENT, 0, 0);
	for(; pNode != NULL; pNode =pNode->pNextInfo)
	{
		strcpy(cNameCur, pNode->ExchMoneyInfo.name);
		if (pNode->ExchMoneyInfo.bBase)
			strcat(cNameCur, " (base):");
		else
			strcat(cNameCur, ":");
		nIndex = SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM)cNameCur);

		SendMessage(hWnd, LB_SETITEMDATA, nIndex, (LPARAM)pNode);
		
		memset(cRate, 0, RATE_MAX_LENGTH+1);
		Exch_NumberToStr(cRate, pNode->ExchMoneyInfo.rate, RATE_MAX_LENGTH);
		if (pNode->ExchMoneyInfo.bBase)
			
		{
			strcat(cRate, ".00");
		}

		SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)cRate);
		SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nIndex), (LPARAM)hBmpCurrency);
	}

	SendMessage(hWnd, LB_INSERTSTRING, 0 , (LPARAM)IDS_NEW);	
	SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)hBmpNewCurrency);
	
	SendMessage(hWnd, LB_SETCURSEL, 0, 0);


}

static HMENU CreateMainWinMenu(HWND hWndParent)
{
	HMENU hMenu, hDeleMany;

	hDeleMany = CreateMenu();
	AppendMenu(hDeleMany, MF_STRING, (UINT_PTR)ID_DELE_SELECT, IDS_DELE_SELECT);
	AppendMenu(hDeleMany, MF_STRING, (UINT_PTR)ID_DELE_ALL, IDS_DELE_ALL);
	
	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_EDIT, IDS_EDIT);
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_ASBASE, IDS_ASBASE);
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_DELETE, IDS_DELETE);
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hDeleMany, IDS_DELEMANY);
	PDASetMenu(hExchFrameWnd, hMenu);
	return hMenu;
}


static int SetCurrIndexItem(HWND hWnd, int iMode)
{
	int iCount;
	int i;
	EXCHMONEYINFONODE *pNode;

	if (DELETEMANY == iMode || DELETEALL == iMode)
	{
		return 0;		
	}

	iCount = SendMessage(hWnd, LB_GETCOUNT, 0, 0);
	
	for(i =1; i < iCount; i++)
	{
		pNode = (EXCHMONEYINFONODE *)SendMessage(hWnd, LB_GETITEMDATA, i, 0);
		if (NEW == iMode && pNode == pExchNodeEnd)
		{
			return i;
		}

		if (EDIT == iMode && pNode == pExchNodeCur)
		{
			return i;
		}
		if (DELETEONE == iMode && pNode == pExchNodeCur)
		{
			if (i == iCount - 1)
			{
				return i -1;
			}
			else if (i < iCount -1)
			{
				return i;
			}
		}
	}
	return 0;
}
