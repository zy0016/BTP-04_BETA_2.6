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

#include "exch_editvalue.h"


DWORD Exch_EditValueWnd(HWND hWndParend, int iCurItem)
{
	WNDCLASS	    wc;
	RECT			rClient;
	
	wc.style         = 0;
	wc.lpfnWndProc   = EditValueWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ExchEditValueWndClass";
	if (!RegisterClass(&wc))
	{
		UnregisterClass("ExchEditValueWndClass", NULL);
		return FALSE;
	}
	GetClientRect(hExchFrameWnd,&rClient);
	iFocusItem = iCurItem;
	hWndEditValueMain =CreateWindow(
				"ExchEditValueWndClass",
				"",//ML("Currency converter"),
				WS_VISIBLE | WS_CHILD,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hExchFrameWnd, NULL, NULL, NULL);
	
	if (!hWndEditValueMain)
		return FALSE;
	SendMessage(hFocus, EM_SETSEL, -1, -1);		
	ShowWindow(hExchFrameWnd, SW_SHOW);
	UpdateWindow(hExchFrameWnd); 
	return TRUE;
}
static LRESULT EditValueWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;
	static HMENU hMenu;
	switch (wMsgCmd)
    {
    case WM_CREATE:
		hMenu = CreateWinMenu();
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_CALCULATE);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		SetWindowText(hExchFrameWnd,ML("Currency converter"));		
		CreateWinEdit(hWnd); // please follow the step: create--setfocus--SetWindowText
		if (iFocusItem == 1)
		{
			hFocus = hWndCurr1;
		}
		else
		{
			hFocus = hWndCurr2;
		}
		SetFocus(hFocus);	
		InitEditCtrl();	
		printf("EditValueWndProc WM_CREATE finished\r\n");
		
		break;


	case WM_PAINT:	
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;
	
	case PWM_SHOWWINDOW:

	
		PDASetMenu(hExchFrameWnd, hMenu);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_CALCULATE);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		SetWindowText(hExchFrameWnd,ML("Currency converter"));
		SetFocus(hFocus);
		break;
	
	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case ID_SELECT:
			if (iTimer)
			{
				KillTimer(hWnd, ID_TIMER);
				iTimer = 0;
			}
			hFocus = GetFocus();
			if (hWndCurr1 == hFocus)
			{
				Exch_SelectWnd(hWnd,1);
			}
			else
			{
				Exch_SelectWnd(hWnd,2);
			}
			break;
		case ID_CURR_RATE:
			if (iTimer)
			{
				KillTimer(hWnd, ID_TIMER);
				iTimer = 0;
			}
			Exch_CurrAndRateWnd(hWnd);
			break;
		case ID_CANCEL:
			{
				int iTextLength;
				
				if (iTimer)
				{
					KillTimer(hWnd, ID_TIMER);
					iTimer = 0;
				}

				iTextLength = GetWindowTextLength(hWndCurr1);
				GetWindowText(hWndCurr1, cExch_Value1, iTextLength + 1);
				
				iTextLength = GetWindowTextLength(hWndCurr2);
				GetWindowText(hWndCurr2, cExch_Value2, iTextLength + 1);
				
				if (strlen(cExch_Value1) == 0)
					strcpy(cExch_Value1, "0");
				
				if (strlen(cExch_Value2) == 0)
					strcpy(cExch_Value2, "0");

				DeleteZeroInPreString(cExch_Value1);
				DeleteZeroInPreString(cExch_Value2);
				
				Exch_SaveTwoValueToFile();
				
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				SendMessage(hExchFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			}
			break;
		case ID_SELECT_BACK:
			{
				hFocus = GetFocus();

				if (hWndCurr1 == hFocus && pExchNodeCur)
				{
					pExchNodeFirst->ExchMoneyInfo.nDefault = 0;
					pExchNodeFirst = pExchNodeCur;
				}
				if (hWndCurr2 == hFocus && pExchNodeCur)
				{
					pExchNodeSecond->ExchMoneyInfo.nDefault = 0;
					pExchNodeSecond = pExchNodeCur;
				}

				Exch_SaveAllNodeToFile();	
			
				InitEditCtrl();
				
			}
			break;
		case ID_CURR_RATE_BACK:
			{
			
				InitEditCtrl();
				
			}
			break;
		case ID_CALCULATE:
			if (iTimer)
			{
				KillTimer(hWnd, ID_TIMER);
				iTimer = 0;
			}
			DoCompute(hFocus);
			
			SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
			SendMessage(Exch_hWndApp, WM_COMMAND , ID_EditValue_Back, 0);
			
			break;
		default:
			break;
		}
		break;
	
	case WM_KEYDOWN:
		if (iTimer)
		{
			KillTimer(hWnd, ID_TIMER);
			iTimer = 0;
		}
		switch(LOWORD(wParam))
		{
			case VK_F10:
				SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				SendMessage(Exch_hWndApp, WM_COMMAND , ID_EditValue_Back, 0);
				break;
			case VK_F5:
				SendMessage(hExchFrameWnd, wMsgCmd, wParam, lParam);
				break;
			case VK_UP:
			case VK_DOWN:
				
				if (hWndCurr1 == hFocus)
				{
					hFocus = hWndCurr2;
				}
				else
				{
					hFocus = hWndCurr1;
				}
				SetFocus(hFocus);
				SendMessage(hFocus, EM_SETSEL, -1, -1);
				break;
			case VK_RETURN:
				SendMessage(hWnd, WM_COMMAND, ID_CALCULATE, 0);
				break;
			default:
				lResult =  PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
		}
		break;
	case WM_KEYUP:

		hFocus = GetFocus();
		
		if(GetValue(hFocus) > 0)
			iTimer = SetTimer(hWnd, ID_TIMER, 5000, NULL);
		break;
			
	case WM_TIMER:
		{
			switch(wParam) 
			{
			case ID_TIMER:
				KillTimer(hWnd, ID_TIMER);
				iTimer = 0;
				DoCompute(hFocus);
				SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				SendMessage(Exch_hWndApp, WM_COMMAND , ID_EditValue_Back, 0);
				
				break;	
				
			}
			
		}
		break;
	case WM_CLOSE:
		
		DestroyMenu(hMenu);
		DestroyWindow(hWnd);
        
		break;

    case WM_DESTROY:
		UnregisterClass("ExchEditValueWndClass", NULL);
		break;

	default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

	return lResult;
}
/**********************************************************************
* Function	CreateWinMenu
* Purpose   create menu
* Params	
* Return	
* Remarks
**********************************************************************/
static HMENU CreateWinMenu()
{
	HMENU hMenu;
	
	hMenu = CreateMenu();
		
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_SELECT, IDS_SELECT);
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_CURR_RATE, IDS_CURR_RATE);
	PDASetMenu(hExchFrameWnd, hMenu);
	return hMenu;
}

/**********************************************************************
* Function	CreateWinEdit
* Purpose   create Edit
* Params	
* Return	
* Remarks
**********************************************************************/
static void CreateWinEdit(HWND hWndParent)
{
	IMEEDIT ie;
	RECT	rc;
	
	
	memset((void*)&ie, 0, sizeof(IMEEDIT));
	ie.hwndNotify   = hWndParent;
	ie.pszImeName	= "Digit";
	ie.pszCharSet	= "0123456789.E+-";

	GetClientRect(hWndParent, &rc);
	hWndCurr1 = CreateWindow(
			"IMEEDIT",
			"",
			WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_TITLE | ES_AUTOHSCROLL | CS_NOSYSCTRL,
			rc.left, rc.top +2,
			rc.right - rc.left, 49,
			hWndParent,
			(HMENU)ID_RateEdit1,
			NULL,
			&ie);


	
	hWndCurr2 = CreateWindow(
		"IMEEDIT",
		"",
		WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_TITLE | ES_AUTOHSCROLL | CS_NOSYSCTRL,
		rc.left, rc.top + 51,
		rc.right - rc.left, 49,
		hWndParent,
		(HMENU)ID_RateEdit2,
		NULL,
		&ie);

	
}
static void InitEditCtrl()
{
	char cName[CURRENCY_MAX_LENGTH + 2];

	SendMessage(hWndCurr1, EM_LIMITTEXT, VALUE_MAX_LENGTH, 0);
 	strcpy(cName, pExchNodeFirst->ExchMoneyInfo.name);
	strcat(cName, ":");
	SendMessage(hWndCurr1, EM_SETTITLE, 0, (LPARAM)ML(cName));
	SetWindowText(hWndCurr1, cExch_Value1);
	
	
	SendMessage(hWndCurr2, EM_LIMITTEXT, VALUE_MAX_LENGTH, 0);
	strcpy(cName, pExchNodeSecond->ExchMoneyInfo.name);
	strcat(cName, ":");
	SendMessage(hWndCurr2, EM_SETTITLE, 0, (LPARAM)ML(cName));
	SetWindowText(hWndCurr2, cExch_Value2);
	
	
	
}
//////////////////////////////////////////////////////////////////////////////
// Function	DoCompute
// Purpose	
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static void DoCompute(HWND hWnd)
{
	double dValue;
	double dResult;
	double dRate;
	char sResult[VALUE_MAX_LENGTH + 1];
	char sResultTmp[VALUE_MAX_LENGTH + 1];

	char * pdecimal;
	char totallength, intpartlength;

	dValue = GetValue(hWnd);
	if (hWndCurr1 == hWnd)
	{
		if (pExchNodeFirst->ExchMoneyInfo.rate > 0) 
			dRate = pExchNodeSecond->ExchMoneyInfo.rate / pExchNodeFirst->ExchMoneyInfo.rate;
		else
			dRate = 0;
	}
	else
	{
		if (pExchNodeSecond->ExchMoneyInfo.rate > 0) 
			dRate = pExchNodeFirst->ExchMoneyInfo.rate / pExchNodeSecond->ExchMoneyInfo.rate;
		else
			dRate = 0;
	}
	dResult = dRate*dValue;
	
	memset(sResult, 0, VALUE_MAX_LENGTH + 1);
	memset(sResultTmp, 0, VALUE_MAX_LENGTH + 1);
	Exch_NumberToStr(sResultTmp, dResult, VALUE_MAX_LENGTH);

	totallength = strlen(sResultTmp);//to show two digit bit for decimal part 
    if(strstr(sResultTmp, "E") || strstr(sResultTmp, "e"))
	{
	  strcpy(sResult, sResultTmp);
	}
	else
	{
		pdecimal = strstr(sResultTmp, ".");
		if(NULL != pdecimal)
		{
			char i = 0;
			intpartlength = pdecimal - sResultTmp;			
			while(*(++pdecimal))
			{ 
				i++;			
			
			}
			if(i > 2)
			{	
			
				if (sResultTmp[intpartlength + 3] > 52 && sResultTmp[intpartlength + 3] < 58)				
				{
					dResult = dResult + 0.01;
					Exch_NumberToStr(sResultTmp, dResult, VALUE_MAX_LENGTH);
					strncpy(sResult, sResultTmp, intpartlength + 3);
					
				}				   
				else
					strncpy(sResult, sResultTmp, intpartlength + 3);			
			
				strcat(sResult, "\0");

			}
			else 
			{
				strcpy(sResult, sResultTmp);
				if(i == 1)
					strcat(sResult, "0");
			}
			
			strcat(sResult, "\0");
				
		}
		else
			 strcpy(sResult, sResultTmp);			
	
	}

	if (hWndCurr1 == hWnd)
	{
		SetWindowText(hWndCurr2, sResult);
		if (sResult == "")
			strcpy(cExch_Value2, "0");
		else
			strcpy(cExch_Value2, sResult);
	}
	else
	{
		SetWindowText(hWndCurr1, sResult);
		if (sResult == "")
			strcpy(cExch_Value1, "0");
		else
			strcpy(cExch_Value1, sResult);
	}
	
}

//////////////////////////////////////////////////////////////////////////////
// Function	GetWindowValue
// Purpose	
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static double GetValue(HWND hWnd)
{
	double dValue = 0;
	int iTextLength;
	char WindowText[VALUE_MAX_LENGTH + 1];
	int iPointNum = 0;
//	int i;

	iTextLength = GetWindowTextLength(hWnd);
	GetWindowText(hWnd, WindowText, iTextLength + 1);	

	if (hWndCurr1 == hWnd)
	{
		if (iPointNum >1)
		{
			return 0;
		}
		strcpy(cExch_Value1, WindowText);
	}
	else
	{
		if (iPointNum >1)
		{
			return 0;
		}
		strcpy(cExch_Value2, WindowText);
	}

	return Exch_StrToNumber(WindowText);
}

//////////////////////////////////////////////////////////////////////////////
// Function	DeleteZeroInPreString
// Purpose	
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static void DeleteZeroInPreString(char *p)
{
	char *pTmp;

	while (*p == '0')
	{
		pTmp = p;
		pTmp++;
		
		if (*pTmp > 47 && *pTmp < 58)
		{
			if(*p != NULL && pTmp != NULL)
			{
				strcpy(p,pTmp);
			}
		}
		else
		{
			break;
		}
	}
}
