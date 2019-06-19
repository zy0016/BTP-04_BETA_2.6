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
#include <dirent.h>
#include <stat.h>
#include <unistd.h>

#include "exch_global.h"
#include "exch.h"



/**********************************************************************
* Function	 Exch_AppControl
* Purpose    application main function
* Params	
* Return	
* Remarks
**********************************************************************/
DWORD Exch_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
	WNDCLASS	    wc;
	DWORD		    dwRet;
	RECT rClient;

    dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT:
		
        hInstance = (HINSTANCE)pInstance;
		{
			DIR *pdir = NULL;

			chdir(EXCH_FLASH_PATH);
			pdir = opendir(EXCH_FLASH_PATH);
			if(pdir == NULL)
				mkdir(EXCH_FLASH_PATH, 0);
			else
				closedir(pdir);
		}
        break;

   case APP_GETOPTION:
		switch (wParam)
		{
		case AS_STATE:
			break;
		case AS_HELP:
			break;
		case AS_APPWND:
			dwRet = (DWORD)Exch_hWndApp;
			break;
		}
		break;

	case APP_SETOPTION:
		break;

    case APP_ACTIVE:

		if(IsWindow(hExchFrameWnd))
		{
			ShowWindow(hExchFrameWnd, SW_SHOW);
			ShowOwnedPopups(hExchFrameWnd, SW_SHOW);
			UpdateWindow(hExchFrameWnd);
		}
		else
		{
			wc.style         = 0;
			wc.lpfnWndProc   = Exch_MainWndProc;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = NULL;
			wc.hIcon         = NULL;
			wc.hCursor       = NULL;
			wc.hbrBackground = GetStockObject(WHITE_BRUSH);
			wc.lpszMenuName  = NULL;
			wc.lpszClassName = "ExchWndClass";
			if (!RegisterClass(&wc))
			{
				UnregisterClass("ExchWndClass", NULL);
				return FALSE;
			}
			hExchFrameWnd = CreateFrameWindow( WS_CAPTION | PWS_STATICBAR);
			GetClientRect(hExchFrameWnd,&rClient);

			Exch_hWndApp = CreateWindow(
				"ExchWndClass",
				"",//ML("Currency converter"),
				WS_VISIBLE | WS_CHILD,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hExchFrameWnd, NULL, NULL, NULL);

			if (!Exch_hWndApp)
				return FALSE;
			
			ShowWindow(hExchFrameWnd, SW_SHOW);
			UpdateWindow(hExchFrameWnd);
		}
        break;

    case APP_INACTIVE:
		ShowOwnedPopups(hExchFrameWnd, SW_HIDE);
        ShowWindow(hExchFrameWnd,SW_HIDE); 
        break;

    default:
        break;
    }

    return dwRet;
}
/**********************************************************************
* Function	ExchMainWndProc
* Purpose   main window function
* Params
* Return	
* Remarks
**********************************************************************/
static LRESULT Exch_MainWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;
	static HMENU hMenu;
	switch (wMsgCmd)
    {
    case WM_CREATE:
 		if (!Exch_GetAllNodeFromFile())
		{
			Exch_InitRateFile();
			Exch_GetAllNodeFromFile();
		}

		if (!Exch_GetValueFromFile())
		{
			Exch_InitValueFile();
			Exch_GetValueFromFile();
		}
		
		memset(cExch_Value1, 0, VALUE_MAX_LENGTH +1);
		memset(cExch_Value2, 0, VALUE_MAX_LENGTH +1);
		
		strcpy(cExch_Value1, "0");
		strcpy(cExch_Value2, "0");

		Exch_SaveTwoValueToFile();
		

		hMenu = CreateWinMenu();
		CreateList(hWnd);
		InitListBox(hWndList);
		SetFocus(hWndList);
		SendMessage(hExchFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_EDIT, 1), (LPARAM)IDS_EDIT);
		SendMessage(hExchFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_EXIT, 0), (LPARAM)IDS_EXIT);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		SetWindowText(hExchFrameWnd,ML("Currency converter"));
		printf("Exch_MainWndProc WM_CREATE finishe\r\n");
		break;

	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;
	case PWM_SHOWWINDOW:
		SetFocus(hWndList);
		PDASetMenu(hExchFrameWnd, hMenu);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		SetWindowText(hExchFrameWnd,ML("Currency converter"));
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
			case ID_EDIT:
				{
					int iIndex;
					iIndex = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
					printf("Exch_MainWndProc WM_COMMAND ID_EDIT \r\n");
					Exch_EditValueWnd(hWnd, iIndex +1);
				}
				break;
			case ID_EditValue_Back:
				printf("Exch_MainWndProc WM_COMMAND ID_EditValue_Back \r\n");
				ModifyListBox(hWndList);
				break;
			case ID_SELECT:
				{
					int nIndex;
					nIndex = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
					printf("Exch_MainWndProc WM_COMMAND ID_SELECT \r\n");
					Exch_SelectWnd(hWnd,nIndex +1);
				}
				
				break;
			case ID_CURR_RATE:
				printf("Exch_MainWndProc WM_COMMAND ID_CURR_RATE \r\n");
				Exch_CurrAndRateWnd(hWnd);
				break;
			case ID_SELECT_BACK:
			
				{
					int nIndex;
					double dRate, dValue, dResult;
					printf("Exch_MainWndProc WM_COMMAND ID_SELECT_BACK \r\n");
					nIndex = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
				
					if (0 == nIndex)
					{
						if(pExchNodeFirst !=pExchNodeCur && pExchNodeFirst != pExchNodeSecond)
							pExchNodeFirst->ExchMoneyInfo.nDefault = 0;
						else
						{
							if (pExchNodeFirst == pExchNodeSecond) 
								pExchNodeSecond->ExchMoneyInfo.nDefault = 2;
						}
						pExchNodeFirst = pExchNodeCur;
						if (pExchNodeSecond->ExchMoneyInfo.rate > 0) 
							dRate =  pExchNodeSecond->ExchMoneyInfo.rate/ pExchNodeFirst->ExchMoneyInfo.rate;
						else
							dRate = 0;
						dValue = Exch_StrToNumber(cExch_Value1);
						dResult = dRate*dValue;
						memset(cExch_Value2, 0, VALUE_MAX_LENGTH +1);
						Exch_NumberToStr(cExch_Value2, dResult, VALUE_MAX_LENGTH);
					}
					else
					{
						if(pExchNodeFirst !=pExchNodeCur && pExchNodeFirst != pExchNodeSecond)
							pExchNodeSecond->ExchMoneyInfo.nDefault = 0;
						else
						{
							if (pExchNodeFirst == pExchNodeSecond) 
								pExchNodeSecond->ExchMoneyInfo.nDefault = 2;
						}
						pExchNodeSecond = pExchNodeCur;
						if (pExchNodeFirst->ExchMoneyInfo.rate>0) 
							dRate = pExchNodeFirst->ExchMoneyInfo.rate / pExchNodeSecond->ExchMoneyInfo.rate;
						else
							dRate = 0;
						dValue = Exch_StrToNumber(cExch_Value2);
						dResult = dRate*dValue;
						memset(cExch_Value1, 0, VALUE_MAX_LENGTH +1);
						Exch_NumberToStr(cExch_Value1, dResult,VALUE_MAX_LENGTH);

					}

					Exch_SaveAllNodeToFile();
					ModifyListBox(hWndList);
				}
				break;
			case ID_CURR_RATE_BACK:
				{
					int nIndex;
					double dRate, dValue, dResult;
					printf("Exch_MainWndProc WM_COMMAND ID_CURR_RATE_BACK \r\n");
					nIndex = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
					if (0 == nIndex)
					{
						if (pExchNodeFirst->ExchMoneyInfo.rate>0) 
							dRate = pExchNodeSecond->ExchMoneyInfo.rate / pExchNodeFirst->ExchMoneyInfo.rate;
						else
							dRate = 0;
						
						dValue = Exch_StrToNumber(cExch_Value1);
						dResult = dRate*dValue;
						memset(cExch_Value2, 0, VALUE_MAX_LENGTH +1);
						Exch_NumberToStr(cExch_Value2, dResult, VALUE_MAX_LENGTH);

					}
					else
					{
						if (pExchNodeSecond->ExchMoneyInfo.rate > 0) 
							dRate = pExchNodeFirst->ExchMoneyInfo.rate / pExchNodeSecond->ExchMoneyInfo.rate;
						else
							dRate = 0;						
						dValue = Exch_StrToNumber(cExch_Value2);
						dResult = dRate*dValue;
						memset(cExch_Value1, 0, VALUE_MAX_LENGTH +1);
						Exch_NumberToStr(cExch_Value1, dResult,VALUE_MAX_LENGTH);
					}
					ModifyListBox(hWndList);
				}
				
				break;
			default:
				break;
		}
		break;		

	
	case WM_KEYDOWN:
		
		switch(LOWORD(wParam))
		{
			
			case VK_RETURN:
				printf("Exch_MainWndProc WM_KEYDOWN VK_RETURN \r\n");
				SendMessage(hWnd, WM_COMMAND, ID_EDIT, 0);
				break;
			case VK_F10:
				printf("Exch_MainWndProc WM_KEYDOWN VK_F10 \r\n");
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				SendMessage(hExchFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				break;
			case VK_F5:
				printf("Exch_MainWndProc WM_KEYDOWN VK_F5 \r\n");
				SendMessage(hExchFrameWnd, wMsgCmd, wParam, lParam);
				break;
			case VK_1:
			case VK_2:
			case VK_3:
			case VK_4:
			case VK_5:
			case VK_6:
			case VK_7:
			case VK_8:
			case VK_9:
			case VK_0:
				{
					printf("Exch_MainWndProc WM_KEYDOWN VK_NUMBER!!!!!!!!!!!!!!! \r\n");
					SendMessage(hWnd, WM_COMMAND, ID_EDIT, 0);
					printf("Exch_MainWndProc  VK_CLEAR!!!!!!!!!!!!!!! \r\n");
					keybd_event(VK_CLEAR, 0, 0, 0);
					keybd_event(VK_CLEAR, 0, KEYEVENTF_KEYUP, 0);
					keybd_event(wParam, 0, 0, 0);
					keybd_event(wParam, 0, KEYEVENTF_KEYUP, 0);

				}
				break;
			default:
				lResult =  PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
		}
		break;
	
	case WM_CLOSE:
		{
			EXCHMONEYINFONODE *pNodeTmp1, *pNodeTmp2;
			
			if (pExchNodeHead != NULL)
			{
				pNodeTmp1 = pExchNodeHead;
				pNodeTmp2 = pExchNodeHead;
				
				while (pNodeTmp1 != NULL) 
				{
					pNodeTmp2 = pNodeTmp1->pNextInfo;
					free(pNodeTmp1);
					pNodeTmp1 = pNodeTmp2;
				}
			}
		}
		nCurrencyNum = 0;
		pExchNodeHead = NULL;
		pExchNodeCur = NULL;
		pExchNodeEnd = NULL;
		pExchNodeFirst = NULL;
		pExchNodeSecond = NULL;
		DestroyMenu(hMenu);
		DestroyWindow(hWnd);
        
		break;

    case WM_DESTROY:
		 UnregisterClass("ExchWndClass", NULL);
		 SendMessage(hExchFrameWnd, PWM_CLOSEWINDOW, 0, 0);
		DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance); 
		break;

	default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

	return lResult;
}
static void CreateList(HWND hWndParent)
{
	RECT rc;

	GetClientRect(hWndParent, &rc);
	hWndList = CreateWindow("LISTBOX",
		"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL| LBS_MULTILINE | LBS_BITMAP,
		rc.left, rc.top,
		rc.right - rc.left, rc.bottom - rc.top,
		hWndParent,
		(HMENU)ID_LIST,
		NULL,
		NULL);
	
	OldListWndProc = (WNDPROC)SetWindowLong(hWndList,GWL_WNDPROC,(LONG)CallListWndProc);
}

static void InitListBox(HWND hWnd)
{
	char cName[CURRENCY_MAX_LENGTH + 5];
	
	if (NULL != pExchNodeFirst)
	{
		strcpy(cName, pExchNodeFirst->ExchMoneyInfo.name);
		strcat(cName, ":");
		SendMessage(hWnd, LB_ADDSTRING, 0,(LPARAM)cName);
		SendMessage(hWnd, LB_SETITEMDATA, 0, (LPARAM)pExchNodeFirst);
	}
	
	if (NULL != pExchNodeSecond) 
	{
		strcpy(cName,pExchNodeSecond->ExchMoneyInfo.name );
		strcat(cName, ":");
		SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM)cName);
		SendMessage(hWnd, LB_SETITEMDATA, 1, (LPARAM)pExchNodeSecond);
	}

	if (cExch_Value1 == "")
	{
		strcpy(cExch_Value1, "0");
		Exch_SaveTwoValueToFile();
	}

	if (cExch_Value2 == "")
	{
		strcpy(cExch_Value2, "0");
		Exch_SaveTwoValueToFile();
	}

	

	SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(0, -1), (LPARAM)cExch_Value1);
	SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(1, -1), (LPARAM)cExch_Value2);

	SendMessage(hWndList, LB_SETCURSEL, 0, 0);
	hFocus = hWndList;
}

static void ModifyListBox(HWND hWnd)
{
	char cName[CURRENCY_MAX_LENGTH + 5];
	
	if (NULL != pExchNodeFirst)
	{
		strcpy(cName, pExchNodeFirst->ExchMoneyInfo.name);
		strcat(cName, ":");
		SendMessage(hWnd, LB_SETTEXT, 0,(LPARAM)cName);
		SendMessage(hWnd, LB_SETITEMDATA, 0, (LPARAM)pExchNodeFirst);
	}
	
	if (NULL != pExchNodeSecond) 
	{
		strcpy(cName,pExchNodeSecond->ExchMoneyInfo.name );
		strcat(cName, ":");
		SendMessage(hWnd, LB_SETTEXT, 1, (LPARAM)cName);
		SendMessage(hWnd, LB_SETITEMDATA, 1, (LPARAM)pExchNodeSecond);
	}

	SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(0, -1), (LPARAM)cExch_Value1);
	SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(1, -1), (LPARAM)cExch_Value2);

}

static HMENU CreateWinMenu()
{
	HMENU hMenu;
	
	hMenu = CreateMenu();
		
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_SELECT, IDS_SELECT);
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_CURR_RATE, IDS_CURR_RATE);
	PDASetMenu(hExchFrameWnd, hMenu);
	return hMenu;
}
static LRESULT CallListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HWND    hParent;

    hParent = GetParent(hWnd);
    
    switch(wMsgCmd)
    {         
    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        { 
        case VK_1:
        case VK_2:
        case VK_3:
        case VK_4:
        case VK_5:
        case VK_6:
        case VK_7:
        case VK_8:
        case VK_9:
        case VK_0:
			printf("CallListWndProc WM_KEYDOWN numberkey\r\n");
            PostMessage(hParent,wMsgCmd,wParam,lParam);
            lResult = CallWindowProc(OldListWndProc, hWnd, wMsgCmd, wParam, lParam);  
			break;
        default:
            lResult = CallWindowProc(OldListWndProc, hWnd, wMsgCmd, wParam, lParam);
			break;
        }
		break;
    default:
        lResult = CallWindowProc(OldListWndProc, hWnd, wMsgCmd, wParam, lParam);
		break;
    }
    return lResult;       
}
