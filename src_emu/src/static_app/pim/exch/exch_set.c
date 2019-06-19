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
#include <ctype.h>
#include <math.h>

#include "exch_set.h"

DWORD Exch_EditRateWnd()
{
	WNDCLASS	    wc;
	RECT			rClient;
	wc.style         = 0;
	wc.lpfnWndProc   = EditRateWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ExchRateWndClass";
	if (!RegisterClass(&wc))
	{
		UnregisterClass("ExchRateWndClass", NULL);
		return FALSE;
	}
	GetClientRect(hExchFrameWnd,&rClient);
	hWndMainRate =CreateWindow(
				"ExchRateWndClass",
				"",//ML(pNodeCur->ExchMoneyInfo.name),
				WS_VISIBLE | WS_CHILD ,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hExchFrameWnd, NULL, NULL, NULL);
	SendMessage(hFocus, EM_SETSEL, -1, -1);
	if (!hWndMainRate)
		return FALSE;	
	
	ShowWindow(hExchFrameWnd, SW_SHOW);
	UpdateWindow(hExchFrameWnd); 
	return TRUE;
}
static LRESULT EditRateWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
		CreateEditCntrl(hWnd);
		hFocus = hWndNameEdit;		
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);	
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SetWindowText(hExchFrameWnd, ML(pExchNodeCur->ExchMoneyInfo.name));	
		SetFocus(hFocus);	
	
		break;

	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;
	case PWM_SHOWWINDOW:
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SetWindowText(hExchFrameWnd, ML(pExchNodeCur->ExchMoneyInfo.name));
		SetFocus(hFocus);
		
		break;
	
	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case ID_SAVE:
			SaveEditData();
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			SendMessage(hExchFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			SendMessage(hWndCurrAndRate, WM_COMMAND, ID_EDIT_SAVEBACK, 0);
			break;
		case ID_CANCEL:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			SendMessage(hExchFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			break;

		default:
			break;
		}
	
		switch(HIWORD(wParam)) 
			{
			case EN_CHANGE:
				{
					char cName[CURRENCY_MAX_LENGTH +1];
					hFocus = GetFocus();
					if (hFocus == hWndNameEdit) 
					{
						GetWindowText(hWndNameEdit, cName, CURRENCY_MAX_LENGTH);
						SetWindowText(hExchFrameWnd, ML(cName));
					}
				}
				break;
			}
	
			break;
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
			case VK_F10:
				SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				break;
			case VK_RETURN:
				SendMessage(hWnd, WM_COMMAND, ID_SAVE, 0);
				break;
			case VK_UP:
			case VK_DOWN:
				hFocus = GetFocus();
				if (hWndNameEdit == hFocus)
					hFocus = hWndRateEdit;
				else
					hFocus = hWndNameEdit;
				SetFocus(hFocus);
				SendMessage(hFocus, EM_SETSEL, -1, -1);
				break;
			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
		}
		break;
		
	case WM_CLOSE:
		DestroyWindow(hWnd);
        
		break;

    case WM_DESTROY:
		UnregisterClass("ExchRateWndClass", NULL);
		break;

	default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

	return lResult;
}
static void CreateEditCntrl(HWND hWndParent)
{
	IMEEDIT ie;
	RECT	rc;
	char	cRate[RATE_MAX_LENGTH +1];
	
	memset((void*)&ie, 0, sizeof(IMEEDIT));
	ie.hwndNotify   = hWndParent;
	ie.pszImeName	= "ABC";
	GetClientRect(hWndParent, &rc);
	hWndNameEdit = CreateWindow(
			"IMEEDIT",
			"",
			WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_TITLE | CS_NOSYSCTRL,
			rc.left, rc.top ,
			rc.right - rc.left, 49,
			hWndParent,
			(HMENU)ID_NameEdit,
			NULL,
			&ie);
	SendMessage(hWndNameEdit, EM_LIMITTEXT, CURRENCY_MAX_LENGTH, 0);
	SendMessage(hWndNameEdit, EM_SETTITLE, 0, (LPARAM)ML("Name:"));
	SetWindowText(hWndNameEdit, pExchNodeCur->ExchMoneyInfo.name);

	ie.pszImeName	= "Digit";
	ie.pszCharSet	= "0123456789.";
	hWndRateEdit = CreateWindow(
		"IMEEDIT",
		"",
		WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_TITLE| CS_NOSYSCTRL |ES_AUTOHSCROLL,
		rc.left, rc.top + 49 ,
		rc.right - rc.left, 49,
		hWndParent,
		(HMENU)ID_RateEdit,
		NULL,
		&ie);

	SendMessage(hWndRateEdit, EM_LIMITTEXT, RATE_MAX_LENGTH, 0);
	SendMessage(hWndRateEdit, EM_SETTITLE, 0, (LPARAM)ML("Rate:"));

	memset(cRate, 0, RATE_MAX_LENGTH +1);

	Exch_NumberToStr(cRate,pExchNodeCur->ExchMoneyInfo.rate, RATE_MAX_LENGTH);

	SetWindowText(hWndRateEdit, cRate);
}

static BOOL SaveEditData()
{
	EXCHMONEY money;
	EXCHMONEYINFONODE *pNode;
	char cRate[RATE_MAX_LENGTH +1];
	
	GetWindowText(hWndNameEdit, money.name, CURRENCY_MAX_LENGTH + 1);
	if(NULL ==strlen(money.name))
	{
		PLXTipsWin(hExchFrameWnd, 0, 0, (char *)STR_DEFINE, NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
		return 0;
	}

	StrToUpper(money.name);
	pNode = Exch_FindNode(money.name);
	
	if (pNode != pExchNodeCur && pNode != NULL)
	{
		PLXTipsWin(hExchFrameWnd, 0, 0, (char *)STR_SAMENAME, NULL, Notify_Failure, NULL, NULL, WAITTIMEOUT);
		return 0;
	}
	
	GetWindowText(hWndRateEdit, cRate, RATE_MAX_LENGTH + 1);

	if (NULL == strlen(cRate))
		money.rate = 0;
	else
		money.rate = Exch_StrToNumber(cRate);
	
	strcpy(pExchNodeCur->ExchMoneyInfo.name, money.name);
	pExchNodeCur->ExchMoneyInfo.rate = money.rate;
	pExchNodeCur->ExchMoneyInfo.bBase = 0;
	Exch_SaveAllNodeToFile();
	return 1;
}
double Exch_StrToNumber(const char *buff)
{
	double ret;
	double act;

	ret=0.0;

	if(strstr(buff, "E") || strstr(buff, "e"))
	{
		ret = strtod(buff, NULL);
		return (ret);
	}
	else if (strstr(buff, "+") || strstr(buff, "-"))
	{
		ret = 0.0;
		return ret;
	}

	while(*buff)
	{
		if(*buff=='.')
			break;
		ret*=10;
		ret+=(*buff-'0');
		buff++;
	}
	if(*buff=='.')
	{
		act=10;
		buff++;
		while(*buff)
		{
			ret+=((double)(*buff-'0'))/act;
			act*=10;
			buff++;
		}
	}
	return (ret);
}

void Exch_NumberToStr(char* buf,double op, int iMaxLength)
{
	char tmp[30];
	int decimal = 0;
	int sign = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	char *str = NULL, *strtmp = NULL;
	int iLength = 0;
	int iRange = 0;
	char cLength[5];
	char* pdot = NULL;
	
	memset(tmp, 0, iMaxLength *2);
	
	if(op == 0)
	{
		strcpy(buf, "0");
		return;
	}
    
	if( op > dValue_Max)
	{
 		str = ecvt(op, iMaxLength, &decimal, &sign);
		iLength = decimal -1;
		op = op / pow(10, iLength);
		str = ecvt(op, 4, &decimal, &sign);
		str[5] = '\0';
		iRange = 1;
	}

	if (op < dValue_Min)
	{
		ecvt(op, iMaxLength, &decimal, &sign);
		iLength = (decimal -1) * (-1);
		op = op * pow(10, iLength);
		str = ecvt(op, 4, &decimal, &sign);
		str[5] = '\0';
		iRange = -1;
	}

	
	
	if (iRange == 0 )
		{
		
			str = ecvt(op, iMaxLength, &decimal, &sign);
					if (decimal == 9 && str[iMaxLength] > 52 && str[iMaxLength] < 58)
					{
						strtmp = ecvt(op, iMaxLength +1, &decimal, &sign);
						if (decimal == 9 && str[iMaxLength] > 52 && str[iMaxLength] < 58)
						{
							
							while (strtmp[k] == '9')
							{
								k++;
							}
							if (k == 8)
							{
								str = ecvt(pow(10, iMaxLength -1), iMaxLength, &decimal, &sign);
							
							}
						}
					}	

		}

	
	if(decimal <= 0)
	{
		tmp[i++] = '0';
		tmp[i++] = '.';
		while(decimal < 0)
		{
			tmp[i++] = '0';
			decimal++;
		}
	}
	else 
	{
		while(j < decimal)
		{
			tmp[i++] = str[j++];
		}
		tmp[i++] = '.';
	}
	
	while(str[j] != '\0')
	{
		tmp[i++] = str[j++];
	}
	tmp[i] = '\0';
	
	i--;
	while(tmp[i] == '0')
	{
		tmp[i--] = '\0';
		if(tmp[i] == '.')
		{
			tmp[i++] = 0;
			break;
		}
	}
	
	if (tmp[iMaxLength -1] == '.')
	{
		tmp[iMaxLength -1] = 0;
	}

	tmp[iMaxLength] = 0;	
	
	strncpy(buf, tmp, iMaxLength);	
	
	if (iRange == 1)
	{
		sprintf(cLength , "%d", iLength);
		strcat(buf, "E+");	
		strcat(buf, cLength);
	}
	if (iRange == -1) 
	{
		sprintf(cLength , "%d", iLength);
		strcat(buf, "E-");
		strcat(buf, cLength);
	}
}
	
/*
void Exch_NumberToStr(char* buf,double op, int iMaxLength)
{
	char tmp[RATE_MAX_LENGTH + 1];
	int decimal = 0;
	int sign = 0;	
	int iLength = 0;
	char cLength[5];

	
	memset(tmp, 0, RATE_MAX_LENGTH + 1);
	
	if(op == 0)
	{
		strcpy(buf, "0");
		return;
	}
    
	if( op > dValue_Max)
	{
	//	gcvt(op,12,tmp);
 		ecvt(op, iMaxLength, &decimal, &sign);
		iLength = decimal -1;
		op = op / pow(10, iLength);
		if(iLength > 9)
			gcvt(op, (iMaxLength - 4), tmp);
		else
			gcvt(op, (iMaxLength - 3), tmp);
		sprintf(cLength , "%d", iLength);
		strcat(tmp, "E+");	
		strcat(tmp, cLength);
		strcat(tmp, "\0");
	}
	else if (op < dValue_Min)
	{
		ecvt(op, iMaxLength, &decimal, &sign);
		iLength = (decimal -1) * (-1);
		op = op * pow(10, iLength);
		if(iLength > 9)
			gcvt(op, (iMaxLength - 4), tmp);
		else
			gcvt(op, (iMaxLength - 3), tmp);
		sprintf(cLength , "%d", iLength);
		strcat(tmp, "E-");	
		strcat(tmp, cLength);
		strcat(tmp, "\0");	
	}
	else
	   gcvt(op, iMaxLength , tmp);	

	strncpy(buf, tmp, iMaxLength);	


	
}
*/
static void StrToUpper(char *pSrc)
{
	while(*pSrc != NULL)
	{
		if(islower(*pSrc))
			*pSrc = toupper(*pSrc);
		pSrc++;
	}
}

DWORD Exch_NewWnd()
{
	WNDCLASS	    wc;
	RECT rClient;

	wc.style         = 0;
	wc.lpfnWndProc   = NewWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ExchNewWndClass";
	if (!RegisterClass(&wc))
	{
		UnregisterClass("ExchNewWndClass", NULL);
		return FALSE;
	}
	GetClientRect(hExchFrameWnd,&rClient);
	hWndMainNew =CreateWindow(
				"ExchNewWndClass",
				"",//ML("New currency"),
				WS_VISIBLE | WS_CHILD ,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hExchFrameWnd, NULL, NULL, NULL);

	if (!hWndMainNew)
		return FALSE;
	SendMessage(hFocus, EM_SETSEL, -1, -1);
	ShowWindow(hExchFrameWnd, SW_SHOW);
	UpdateWindow(hExchFrameWnd); 
	return TRUE;
}
static LRESULT NewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
		CreateNewCntrl(hWnd);
		hFocus = hWndNameEdit;
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SetWindowText(hExchFrameWnd, ML("New currency"));
		SetFocus(hFocus);

		break;

	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;
	
	case PWM_SHOWWINDOW:
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SetWindowText(hExchFrameWnd, ML("New currency"));
		SetFocus(hFocus);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case ID_SAVE:
			if(SaveNewData())
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				SendMessage(hExchFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				SendMessage(hWndCurrAndRate, WM_COMMAND, ID_NEW_SAVEBACK, 0);
			}
			break;
		case ID_CANCEL:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			SendMessage(hExchFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			break;

		default:
			break;
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
			case VK_F10:
				SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				break;
			case VK_RETURN:
				SendMessage(hWnd, WM_COMMAND, ID_SAVE, 0);
				break;
			case VK_UP:
			case VK_DOWN:
				hFocus = GetFocus();
				if (hWndNameEdit == hFocus)
					hFocus = hWndRateEdit;
				else
					hFocus = hWndNameEdit;
				SetFocus(hFocus);
				SendMessage(hFocus, EM_SETSEL, -1, -1);
				break;
			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
		}
		break;
		
	case WM_CLOSE:
		DestroyWindow(hWnd);
        
		break;

    case WM_DESTROY:
		UnregisterClass("ExchNewWndClass", NULL);
		break;

	default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

	return lResult;
}

static BOOL SaveNewData()
{
	EXCHMONEY money;
	EXCHMONEYINFONODE *pNode,Node;
	char cRate[RATE_MAX_LENGTH +1];
	
	GetWindowText(hWndNameEdit, money.name, CURRENCY_MAX_LENGTH + 1);
	if(NULL ==strlen(money.name))
	{
		PLXTipsWin(hExchFrameWnd, 0, 0, (char *)STR_DEFINE, NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
		return FALSE;
	}
	StrToUpper(money.name);
	pNode = Exch_FindNode(money.name);
	
	if (pNode != NULL)
	{
		PLXTipsWin(hExchFrameWnd, 0, 0, (char *)STR_SAMENAME, NULL, Notify_Alert,ML("Ok"), NULL, WAITTIMEOUT);
		return FALSE;
	}
	GetWindowText(hWndRateEdit, cRate, RATE_MAX_LENGTH + 1);

	if (NULL == strlen(cRate))
		money.rate = 0;
	else
		money.rate = Exch_StrToNumber(cRate);
	strcpy(Node.ExchMoneyInfo.name, money.name);
	Node.ExchMoneyInfo.rate = money.rate;
	Node.ExchMoneyInfo.bBase = 0;
	Node.ExchMoneyInfo.nDefault = 0;
	Exch_AddNode(&Node.ExchMoneyInfo);
	Exch_SaveAllNodeToFile();
	return TRUE;
}
static void CreateNewCntrl(HWND hWndParent)
{
	IMEEDIT ie;
	RECT	rc;
	
	memset((void*)&ie, 0, sizeof(IMEEDIT));
	ie.hwndNotify   = hWndParent;
	ie.pszImeName	= "ABC";

	GetClientRect(hWndParent, &rc);
	hWndNameEdit = CreateWindow(
			"IMEEDIT",
			"",
			WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_TITLE | CS_NOSYSCTRL,
			rc.left, rc.top ,
			rc.right - rc.left, 49,
			hWndParent,
			(HMENU)ID_NameEdit,
			NULL,
			&ie);
	SendMessage(hWndNameEdit, EM_LIMITTEXT, CURRENCY_MAX_LENGTH, 0);
	SendMessage(hWndNameEdit, EM_SETTITLE, 0, (LPARAM)ML("Name:"));

	ie.pszImeName	= "Digit";
	ie.pszCharSet	= "0123456789.";
	hWndRateEdit = CreateWindow(
		"IMEEDIT",
		"",
		WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_TITLE | CS_NOSYSCTRL | ES_AUTOHSCROLL,
		rc.left, rc.top + 49 ,
		rc.right - rc.left, 49,
		hWndParent,
		(HMENU)ID_RateEdit,
		NULL,
		&ie);

	SendMessage(hWndRateEdit, EM_LIMITTEXT, RATE_MAX_LENGTH, 0);
	SendMessage(hWndRateEdit, EM_SETTITLE, 0, (LPARAM)ML("Rate:"));
	SetWindowText(hWndRateEdit, "0");
}

DWORD Exch_Dele_SelectWin()
{
	WNDCLASS wc;
	RECT rClient;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.hCursor = NULL;
	wc.hIcon = NULL;
	wc.hInstance = NULL;
	wc.lpfnWndProc = Dele_SelectWinProc;
	wc.lpszClassName = "Dele_SelectWinClass";
	wc.lpszMenuName = NULL;
	wc.style = 0;

	if(!RegisterClass(&wc))
		return FALSE;
	GetClientRect(hExchFrameWnd,&rClient);
	hWndDele_Select = CreateWindow(
		"Dele_SelectWinClass",
		"",//ML("Select currencies"),
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hExchFrameWnd,
		NULL,
		NULL,
		NULL
		);
	if (!hWndDele_Select)
		return FALSE;
	ShowWindow(hExchFrameWnd, SW_SHOW);
	UpdateWindow(hExchFrameWnd);
	
	return TRUE;
}

static LRESULT Dele_SelectWinProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = TRUE;

	switch(wMsgCmd)
	{
	
	case WM_CREATE:
		if (CreateList(hWnd) == 0)
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		else
		{
			SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
			SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_DELETE);
			SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			SetWindowText(hExchFrameWnd,ML("Select currencies"));
			SetFocus(hWndDelList);
		}
		
		break;

 	case PWM_SHOWWINDOW:
		SetFocus(hWndDelList);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_DELETE);
		SendMessage(hExchFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SetWindowText(hExchFrameWnd,ML("Select currencies"));
		
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case ID_DELETE:
			{
				int nCount, i;
				int iSelectedNum = 0;

				nCount = SendMessage(hWndDelList, LB_GETCOUNT, 0, 0);
				if(nCount > 0)
				{	
					for(i = 0; i < nCount; i++)
					{
						if(SendMessage(hWndDelList, LB_GETSEL, (WPARAM)i, 0))
						{
							iSelectedNum ++;
						}
					}
				}
				if ( 0 == iSelectedNum)
				{
					PLXTipsWin(hExchFrameWnd, hWnd, 0, ML("Please select currencies"), ML("Select currencies"), 
						Notify_Alert, ML("Ok"), 0, WAITTIMEOUT);
				}
				else if(iSelectedNum > 1)
				{
					PLXConfirmWinEx(hExchFrameWnd, hWnd, ML("Delete selected\n currencies?"), Notify_Request, 
						ML("Currencies & rates"), ML("Yes"),ML("No"), CONFIRM_DELETE);
				}
				else if (1 == iSelectedNum) 
				{
					char cPrompt[CURRENCY_MAX_LENGTH + 10];
					EXCHMONEYINFONODE *pNode = NULL;
					int id;
					id = SendMessage(hWndDelList, LB_GETCURSEL, 0, 0);				
					pNode = (EXCHMONEYINFONODE *)SendMessage(hWndDelList, LB_GETITEMDATA, (WPARAM)id, 0);
					strcpy(cPrompt, pNode->ExchMoneyInfo.name);
					strcat(cPrompt, ":\n");
					strcat(cPrompt, ML("Delete?"));
					PLXConfirmWinEx(hExchFrameWnd, hWnd, cPrompt, Notify_Request, ML("Currencies & rates"), ML("Yes"), ML("No"), CONFIRM_DELETE);
				}
			}
			break;
		case ID_CANCEL:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			SendMessage(hExchFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			break;

		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
			case VK_F10:
				SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				break;
			case VK_RETURN:
				SendMessage(hWnd, WM_COMMAND, ID_DELETE, 0);
				break;
			default:
				lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
		}
		break;
	case CONFIRM_DELETE:
		{
			int nCount, i;
			EXCHMONEYINFONODE *pNode;
			if(lParam)
			{
				nCount = SendMessage(hWndDelList, LB_GETCOUNT, 0, 0);
				if(nCount > 0)
				{	
					for(i = 0; i < nCount; i++)
					{
						if(SendMessage(hWndDelList, LB_GETSEL, (WPARAM)i, 0))
						{
							pNode = (EXCHMONEYINFONODE *)SendMessage(hWndDelList, LB_GETITEMDATA, (WPARAM)i, 0);
							if (pExchNodeFirst == pNode) 
							{
								pExchNodeFirst = pExchNodeBase;
								pExchNodeFirst->ExchMoneyInfo.nDefault = 1;
							}
							if (pExchNodeSecond == pNode) 
							{
								pExchNodeSecond = pExchNodeBase;
								pExchNodeSecond->ExchMoneyInfo.nDefault = 1;
							}
							
							Exch_DeleteNode(pNode);
						}
					}
					if(Exch_SaveAllNodeToFile())
					{
						PLXTipsWin(hExchFrameWnd, hWnd, 0, ML("Deleted"), ML("Currencies & rates"), Notify_Success, ML("Ok"), 0, WAITTIMEOUT);
						SendMessage(hWndCurrAndRate, WM_COMMAND, ID_DELEOK_BACK, MAKELPARAM(4,0));
					}
				}
				else
					PLXTipsWin(hExchFrameWnd, hWnd, 0, ML("No currencies/n  selected"), ML("Currencies & rates"), Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
			}
			SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
		}
		break;

	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		UnregisterClass("Dele_SelectWinClass", NULL);
		break;
	default:
		lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lRet;
}
static int CreateList(HWND hWndParent)
{
	RECT rc;
	EXCHMONEYINFONODE *pNode;
	char  cNameNext[CURRENCY_MAX_LENGTH + 10];
	int nIndex = 0;

	GetClientRect(hWndParent, &rc);
	hWndDelList = CreateWindow("MULTILISTBOX",
		"",
		WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_SORT| LBS_BITMAP,
		rc.left, rc.top,
		rc.right - rc.left, rc.bottom - rc.top,
		hWndParent,
		(HMENU)ID_DELETELIST,
		NULL,
		NULL);
	
	pNode = pExchNodeHead;
	
	if (NULL == pNode)
		return 0;

	for(; pNode != NULL; pNode = pNode->pNextInfo)
	{
		if (!pNode->ExchMoneyInfo.bBase)
		{
			strcpy(cNameNext, pNode->ExchMoneyInfo.name);
			nIndex = SendMessage(hWndDelList, LB_ADDSTRING, 0, (LPARAM)cNameNext);
			SendMessage(hWndDelList, LB_SETITEMDATA, nIndex, (LPARAM)pNode);
		}
	}

	SendMessage(hWndDelList, LB_ENDINIT, 0, 0);
	SendMessage(hWndDelList, LB_SETCURSEL, 0, 0);

	return 1;
}
