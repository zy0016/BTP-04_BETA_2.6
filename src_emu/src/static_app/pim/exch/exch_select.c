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

#include "exch_global.h"
#include "exch_select.h"


DWORD Exch_SelectWnd(HWND hWnd , int nCurrID)
{
	WNDCLASS	    wc;
	RECT rClient;
	
	nDefaut = nCurrID;
	hWndBefore = hWnd;
	wc.style         = 0;
	wc.lpfnWndProc   = SelectWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ExchSelectWndClass";
	if (!RegisterClass(&wc))
	{
		UnregisterClass("ExchSelectWndClass", NULL);
		return FALSE;
	}

	GetClientRect(hExchFrameWnd,&rClient);
	hWndMain =CreateWindow(
				"ExchSelectWndClass",
				"",//ML(cTitle),				
				WS_VISIBLE | WS_CHILD,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hExchFrameWnd, NULL, NULL, NULL);

	if (!hWndMain)
		return FALSE;
	

	ShowWindow(hExchFrameWnd, SW_SHOW);
	UpdateWindow(hExchFrameWnd); 
	return TRUE;
}
static LRESULT SelectWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
		{
		
			HDC hdc;

			
			hdc = GetDC(hWnd);
			hBmpRadioOn = LoadImage(NULL, RADIO_ON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			hBmpRadioOff = LoadImage(NULL, RADIO_OFF, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			ReleaseDC(hWnd, hdc);
			
			CreateCurrList(hWnd);
			SetFocus(hWndList);
			SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);			
			SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)" ");
			SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");		
			SetWindowText(hExchFrameWnd,ML(cTitle));
		    printf("SelectWndProc WM_CREATE finished\r\n");
		}
		break;

	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;	

	case PWM_SHOWWINDOW:
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SetWindowText(hExchFrameWnd,ML(cTitle));	
		SetFocus(hWndList);
		break;
	
	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case ID_CANCEL:
			printf("SelectWndProc ID_CANCEL\r\n");
			PostMessage(hWnd, WM_CLOSE, 0, 0);		
			break;

		default:
			break;
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
			case VK_F10:
				printf("SelectWndProc VK_F10\r\n");
				PostMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				break;
			case VK_F5:
				{
					int nIndex;
					
					nIndex = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
					pExchNodeCur = (EXCHMONEYINFONODE *)SendMessage(hWndList, LB_GETITEMDATA, nIndex, 0);
					pExchNodeCur->ExchMoneyInfo.nDefault = nDefaut;
					PostMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
					PostMessage(hWndBefore, WM_COMMAND, ID_SELECT_BACK, 0);
				}
				
				break;
			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
		}
		break;
	case WM_CLOSE:
		printf("SelectWndProc WM_CLOSE\r\n");
		DestroyWindow(hWnd);        
		break;

    case WM_DESTROY:
		printf("SelectWndProc WM_DESTROY\r\n");
		UnregisterClass("ExchSelectWndClass", NULL);
		SendMessage(hExchFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		break;

	default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

	return lResult;
}

//////////////////////////////////////////////////////////////////////////////
// Function	CreateCurrList
// Purpose	create listbox
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static void CreateCurrList(HWND hWndParent)
{
	RECT rc;
	EXCHMONEYINFONODE	*pNode;
	char  cName[CURRENCY_MAX_LENGTH + 10];
	int nIndex = 0;


	GetClientRect(hWndParent, &rc);
	hWndList = CreateWindow("LISTBOX",
		"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP | LBS_SORT,
		rc.left, rc.top,
		rc.right - rc.left, rc.bottom - rc.top,
		hWndParent,
		(HMENU)ID_CurrList,
		NULL,
		NULL);

	pNode = pExchNodeHead;
	if (NULL == pNode)
		return;
	for(; pNode != NULL; pNode = pNode->pNextInfo)
	{
		strcpy(cName, pNode->ExchMoneyInfo.name);
		
		if (pNode->ExchMoneyInfo.bBase)
		{
			strcat(cName, " (base)");
		}
		nIndex = SendMessage(hWndList, LB_ADDSTRING, 0, (long)cName);
		SendMessage(hWndList, LB_SETITEMDATA, nIndex, (LPARAM)pNode);
		
		if (nDefaut == 1)
		{
			if (pExchNodeFirst == NULL)
				return;
			if(strcmp(pExchNodeFirst->ExchMoneyInfo.name, pNode->ExchMoneyInfo.name) == 0)
				SendMessage(hWndList, LB_SETIMAGE, MAKEWPARAM( IMAGE_BITMAP, nIndex), (LPARAM)hBmpRadioOn);
			else
			{
				SendMessage(hWndList, LB_SETIMAGE, MAKEWPARAM( IMAGE_BITMAP, nIndex), (LPARAM)hBmpRadioOff);
			}
		}
		else
		{
			if (pExchNodeSecond == NULL)
				return;
			if(strcmp(pExchNodeSecond->ExchMoneyInfo.name, pNode->ExchMoneyInfo.name) == 0)
				SendMessage(hWndList, LB_SETIMAGE, MAKEWPARAM( IMAGE_BITMAP, nIndex), (LPARAM)hBmpRadioOn);
			else
			{
				SendMessage(hWndList, LB_SETIMAGE, MAKEWPARAM( IMAGE_BITMAP, nIndex), (LPARAM)hBmpRadioOff);
			}
		}

	}

	SendMessage(hWndList, LB_SETCURSEL, 0, 0);

}
