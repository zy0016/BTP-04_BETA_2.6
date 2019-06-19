/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : StkMain.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "malloc.h"
#include "string.h"
#include "stdio.h"

#include "plx_pdaex.h"
#include "pubapp.h"
#include "winpda.h"

#include "me_wnd.h"
#include "imesys.h"
#include "assert.h"

#include "StkMain.h"
#include "StkRs.h"
#include "StkAT.h"

#ifndef _STK_DEBUG_
#define _STK_DEBUG_
#endif // _STK_DEBUG_

/////////////////////////////////////////////////////////////////////
static HWND hFrameWnd, hInputWnd;
static HINSTANCE hStkInstance;
static int hStkRegIndn;
static int LastCmdType = STK_PAC_SETUPMENU;
static HWND hMenuList, hStaticText;
static HWND hInputEdit, hInputNumber, hInputHideNumber;
static INPUTEDIT InputEditInfo, InputNumberInfo, InputHideNumberInfo;
static IMEEDIT ImeEdit, ImenumEdit, ImehidenumEdit;
static char PhoneNumber[STK_MAC_STRINGCOMMAND];
static BOOL bAccepted, bDialSucc, bSetupCall;
static HGIFANIMATE hRequestGif;
static BOOL bShowGif;

/////////////////////////////////////////////////////////////////////
HWND hStkWndApp;
BOOL bShowMenu, bActivate;
BOOL bInitOK;
STK_PARAM STK_SendStruct;
SETUPMENU SetupMenu;
SELECTITEM SelectItem;
MENUITEM SetupMenuItem[STK_MAC_MENUCOUNT], SelectMenuItem[STK_MAC_MENUCOUNT];

REFRESH Refresh;
SETUPEVENTLIST SetupEventList;
SETUPCALL SetupCall;
SENDSS SendSS;
SENDUSSD SendUSSD;
SENDSHORTMESSAGE SendShortMessage;
SENDDTMF SendDTMF;
PLAYTONE PlayTone;
DISPLAYTEXT DisplayText;
GETINKEY GetInkey;
GETINPUT GetInput;
SETUPIDLEMODETEXT SetupIdleModeText;

/////////////////////////////////////////////////////////////////////
static LRESULT STK_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static int  STK_SetButtonText(void);
static int  STK_InputChange(void);
static void STK_OnSetLBtnText(HWND hWnd, int nID, BOOL bEmpty, LPSTR pszText);
static int  STK_AudioTonePlay(int toneType, int timeout);
static char * STK_GetTextbyErrorID(int ErrID);

static int  STK_App_CreateWindow(HWND hWnd);
static int  STK_App_ShowWindow(int CmdType);
static int  STK_App_HideWindow(void);

static int  STK_App_Show_Refresh(void);
static int  STK_App_Show_EventList(void);
static int  STK_App_Show_SetupCall(void);
static int  STK_App_Show_SS(void);
static int  STK_App_Show_USSD(void);
static int  STK_App_Show_SMS(void);
static int  STK_App_Show_DTMF(void);
static int  STK_App_Show_PlayTone(void);
static int  STK_App_Show_Browser(void);
static int  STK_App_Show_DispText(void);
static int  STK_App_Show_Inkey(void);
static int  STK_App_Show_Input(void);
static int  STK_App_Show_SubMenu(void);
static int  STK_App_Show_MainMenu(void);
static int  STK_App_Show_IdleModeText(void);

static int  STK_Send_PacInfo(int CmdType);
static int  STK_Recv_PacInfo(void);
static void STK_TipsWin(int ErrCode);

static int  STK_Send_Resp_Ack(void);
static int  STK_Send_Resp_Ok(void);
static int  STK_Send_Resp_Back(void);
static int  STK_Send_Resp_End(void);
static int  STK_Send_Resp_Timeout(void);
static int  STK_Send_Resp_Help(void);

static BOOL IfHighPriority(void);
static BOOL IfMessageDelay(void);
static BOOL IfSustainedText(void);
static BOOL IfYesNoRequest(void);
static BOOL IfEcho(void);
static BOOL IfHelp(int CmdType);

/////////////////////////////////////////////////////////////////////
extern int  GetSIMState(void);
extern BOOL GetStkInitResult(void);
extern BOOL APP_CallPhoneNumberSTK(const char * pPhoneNumber);
extern BOOL DeleteSTKAppItem(void);

extern int  STK_Proat_Malloc(void);
extern int  STK_Proat_Free(void);

extern int  STK_Proat_SetupMenu(char pContent[STK_MAC_MENUCOUNT][STK_MAC_MENUCOMMAND], PSETUPMENU pSetupMenu, MENUITEM pSetupMenuItem[STK_MAC_MENUCOUNT]);
extern int  STK_Proat_SelectItem(char pContent[STK_MAC_MENUCOUNT][STK_MAC_MENUCOMMAND], PSELECTITEM pSelectItem, MENUITEM pSelectMenuItem[STK_MAC_MENUCOUNT]);
extern int  STK_Proat_String(int Indicate, char* pContent);

extern void STK_FormatStr(char *pszStrIn, char *pszStrOut);
extern void STK_WaitWin(HWND hWnd, BOOL bWait);
extern int  STK_Proat_Request(int CmdType);
extern int  STK_Proat_Response(int CmdType, int State, char *TextData, int TextLen, int ID);

extern BOOL IfStkInitSucc(void);
extern BOOL CreateStkHideWindow(void);

/*********************************************************************
* Function	Stk_AppControl
* Purpose   
* Parameter	
* Return	DWORD
* Remarks	
**********************************************************************/
DWORD Stk_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
	WNDCLASS wc;
	RECT rf;
	HMENU hMenu;
	DWORD dwRet = TRUE;

	switch (nCode)
	{
	case APP_INIT:
		hStkInstance = (HINSTANCE)pInstance;

		// Create a hide window
		CreateStkHideWindow();
		break;

	case APP_GETOPTION:
		switch (wParam)
		{
		case AS_ICON:
			break;
		case AS_STATE:
			if (hFrameWnd)
			{
				if (IsWindowVisible(hFrameWnd))
				{
					dwRet = AS_ACTIVE;
				}
				else
				{
					dwRet = AS_INACTIVE;
				}
			}
			else
			{
				dwRet = AS_NOTRUNNING;
			}
			break;
		case AS_HELP:
			break;
        case AS_APPWND:
			return (DWORD)hFrameWnd;
		}
		break;

	case APP_ACTIVE:
		/*
		由程序管理器负责是否显示STK图标,无SIM卡或插入不支持
		STK功能的卡隐藏该图标
		*/
#if 0
		// If no SIM card
		if (GetSIMState() != 1)
		{
			PLXTipsWin(NULL, NULL, 0,
				IDS_STK_NOSIMCARD, IDS_STK_SIMTOOLKIT,
				Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
			return FALSE;
		}

		// If no support for STK
		if (!GetStkInitResult())
		{
			PLXTipsWin(NULL, NULL, 0,
				IDS_STK_NOSUPPORT, IDS_STK_SIMTOOLKIT,
				Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
			return FALSE;
		}
#endif // #if 0

		if (!IfStkInitSucc())
		{
			PLXTipsWin(NULL, NULL, 0,
				IDS_STK_INITIALIZING, "",
				Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
			return FALSE;
		}

		if (hFrameWnd)
		{
			ShowWindow(hFrameWnd, SW_SHOW);
			ShowOwnedPopups(hFrameWnd, TRUE);
		}
		else
		{
			wc.style         = 0;
			wc.lpfnWndProc   = STK_WndProc;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = NULL;
			wc.hIcon         = NULL;
			wc.hCursor       = NULL;
			wc.hbrBackground = NULL;
			wc.lpszMenuName  = NULL;
			wc.lpszClassName = "StkWndClass";
			if (!RegisterClass(&wc))
				return FALSE;

			// Create frame window
			hFrameWnd = CreateFrameWindow(WS_CAPTION|PWS_STATICBAR);
			GetClientRect(hFrameWnd, &rf);

			hMenu = CreateMenu();
			hStkWndApp = CreateWindow("StkWndClass",
				IDS_STK_SIMTOOLKIT,
				WS_VISIBLE|WS_CHILD,
				rf.left,
				rf.top,
				rf.right - rf.left,
				rf.bottom - rf.top,
				hFrameWnd,
				hMenu, NULL, NULL);

			// Allocate 20*100 bytes for ME received buffer
			STK_SendStruct.revbuf = (void *)malloc(STK_MAC_MENUCOUNT*STK_MAC_MENUCOMMAND);
			STK_SendStruct.line_count = STK_MAC_MENUCOUNT;
			STK_SendStruct.line_len = STK_MAC_MENUCOMMAND;

			// Register STK Indication to Monitor
			hStkRegIndn = ME_RegisterIndication(ME_URC_SSTN, IND_PRIORITY, hStkWndApp, WM_STK_INDICATION);
			if (hStkRegIndn == -1)
			{
				// Deallocate!
				free(STK_SendStruct.revbuf);

				UnregisterClass("StkWndClass", NULL);
				hStkWndApp = NULL;
				hFrameWnd = NULL;

				// Registering failed
				PLXTipsWin(NULL, NULL, 0,
					IDS_STK_REGISTERING, IDS_STK_SIMTOOLKIT,
					Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
				return FALSE;
			}

			// Request for Main Menu
			STK_Proat_Malloc();
			LastCmdType = STK_PAC_SETUPMENU;
			STK_WaitWin(hStkWndApp, TRUE);
			STK_Proat_Request(STK_PAC_SETUPMENU);

			SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)ML(""));
			SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 0), (LPARAM)ML(""));

			PDASetMenu(hFrameWnd, hMenu);
			ShowWindow(hFrameWnd, SW_SHOW);
			UpdateWindow(hFrameWnd);
			SetFocus(hStkWndApp);
		}
		break;

	case APP_INACTIVE:
		ShowWindow(hFrameWnd, SW_HIDE);
		ShowOwnedPopups(hFrameWnd, FALSE);
		break;
	}

	return dwRet;
}

/*********************************************************************
* Function	STK_WndProc
* Purpose   
* Parameter	
* Return	LRESULT
* Remarks	
**********************************************************************/
static LRESULT STK_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	HDC		hdc;

	lResult = (LRESULT)TRUE;

	switch (uMsg)
	{
	case WM_CREATE:
		bShowMenu = FALSE;
		STK_App_CreateWindow(hWnd);
		break;

	case PWM_ACTIVATE:
		if (wParam == WA_ACTIVE)
		{
			switch (LastCmdType)
			{
			case STK_PAC_GETINKEY:
				// SIM requests Yes/No answer
				if (IfYesNoRequest())
					SetFocus(hStkWndApp);
				else
					SetFocus(hInputWnd);
				break;

			case STK_PAC_GETINPUT:
				SetFocus(hInputWnd);
				break;

			case STK_PAC_SELECTITEM:
			case STK_PAC_SETUPMENU:
				SetFocus(hMenuList);
				break;

			default:
				SetFocus(hStkWndApp);
				break;
			}
		}
		break;

	case WM_INITMENU:
		{
			HMENU hMenu;
			int nCount;

			hMenu = GetMenu(hWnd);
			nCount = GetMenuItemCount(hMenu);

			while (nCount > 0)
			{
				nCount--;
				DeleteMenu(hMenu, nCount, MF_BYPOSITION);
			}

			// Help menu item
			AppendMenu(hMenu, MF_ENABLED, IDC_STK_HELP, IDS_STK_HELP);
		}
		break;

	case WM_TIMER:
		switch ((int)wParam)
		{
		case STK_TIMER_MESSAGE_DELAY:
			KillTimer(hStkWndApp, STK_TIMER_MESSAGE_DELAY);

			// Sustained display text
			if (!IfSustainedText())
			{
				STK_Proat_Response(STK_PAC_DISPLAYTEXT, STK_REP_SUCCESS, "", 0, 0);
			}
			break;

		case STK_TIMER_INKEY_TIMEOUT:
			if (LastCmdType == STK_PAC_GETINKEY)
			{
				KillTimer(hStkWndApp, STK_TIMER_INKEY_TIMEOUT);

				// No response from user
				STK_Proat_Response(STK_PAC_GETINKEY, STK_REP_NORESPONSE, "", 0, 0);
			}
			break;

		case STK_TIMER_INPUT_TIMEOUT:
			if (LastCmdType == STK_PAC_GETINPUT)
			{
				KillTimer(hStkWndApp, STK_TIMER_INPUT_TIMEOUT);

				// No response from user
				STK_Proat_Response(STK_PAC_GETINPUT, STK_REP_NORESPONSE, "", 0, 0);
			}
			break;

		case STK_TIMER_SELECT_TIMEOUT:
			if (LastCmdType == STK_PAC_SELECTITEM)
			{
				KillTimer(hStkWndApp, STK_TIMER_SELECT_TIMEOUT);

				// No response from user
				STK_Proat_Response(STK_PAC_SELECTITEM, STK_REP_NORESPONSE, "", 0, 0);
			}
			break;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_STK_EDIT:
		case IDC_STK_NUMBER:
		case IDC_STK_HIDENUMBER:
			switch (HIWORD(wParam))
			{
			case EN_SETFOCUS:
			case EN_CHANGE:
				PostMessage(hStkWndApp, WM_STK_IMECHANGE, wParam, lParam);
				break;
			}
			break;

		case IDC_STK_MENU:
			// Double click
			if (LBN_DBLCLK == HIWORD(wParam))
			{
				int ret;

				ret = STK_Send_Resp_Ok();
				STK_TipsWin(ret);
			}
			break;

		case IDC_OK:
			// Left button down
			{
				int ret;

				ret = STK_Send_Resp_Ok();
				STK_TipsWin(ret);
			}
			break;

		case IDC_BACK:
			// Right button down
			{
				int ret;

				ret = STK_Send_Resp_Back();
				STK_TipsWin(ret);
			}
			break;

		case IDC_TIMEOUT:
			// Timeout, used for PLXTipsWin
			{
				int ret;

				ret = STK_Send_Resp_Timeout();
				STK_TipsWin(ret);
			}
			break;

		case IDC_ENDSESSION:
			// End session, used for PLXTipsWin
			{
				int ret;

				ret = STK_Send_Resp_End();
				STK_TipsWin(ret);
			}
			break;

		case IDC_STK_HELP:
			// Help info
			{
				int ret;

				ret = STK_Send_Resp_Help();
				STK_TipsWin(ret);
			}
			break;
		}
		break;

	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_RETURN:
			// Left key down
			{
				int ret;

				ret = STK_Send_Resp_Ok();
				STK_TipsWin(ret);
			}
			break;

		case VK_F10:
			// Right key down
			{
				int ret;

				ret = STK_Send_Resp_Back();
				STK_TipsWin(ret);
			}
			break;

		case VK_F2:
			// End key down
			{
				int ret;

				ret = STK_Send_Resp_End();
				STK_TipsWin(ret);
			}
			break;

		case VK_F5:
			// OK key down
			PDADefWindowProc(hFrameWnd, uMsg, wParam, lParam);
			break;
		}
		break;

	case WM_STK_INDICATION:
		{
			int ret;

			if (ME_GetResult(&ret, sizeof(int)) == -1)
				break;

			// AT^SSTGI=
			STK_Send_PacInfo(ret);
		}
		break;

	case WM_STK_PACINFO_RECV:
		{
			int ret;

			switch (wParam)
			{
			case ME_RS_SUCCESS:
				// Unpacking parameters
				STK_Recv_PacInfo();

				// Set left/right/ok button text
				STK_SetButtonText();

				// Setup menu response
				STK_Send_Resp_Ack();
				break;
			case ME_RS_FAILURE:
				// Close waiting window
				STK_WaitWin(hStkWndApp, FALSE);

				ret = (int)lParam;
				STK_TipsWin(ret);
				break;
			default:
				// Close waiting window
				STK_WaitWin(hStkWndApp, FALSE);
				break;
			}
		}
		break;

	case WM_STK_RESP_RECV:
		{
			int ret;

			// Hide a GIF animation
			bShowGif = FALSE;

			// Open waiting window
			STK_WaitWin(hStkWndApp, TRUE);

			switch (wParam)
			{
			case ME_RS_SUCCESS:
				if (LastCmdType == STK_PAC_SETUPCALL && bAccepted)
				{
					// Dialing failed
					bDialSucc = TRUE;
				}
				break;
			case ME_RS_FAILURE:
				if (LastCmdType == STK_PAC_SETUPCALL && bAccepted)
				{
					// Dialing failed
					bDialSucc = FALSE;
				}

				// Close waiting window
				STK_WaitWin(hStkWndApp, FALSE);

				ret = (int)lParam;
				STK_TipsWin(ret);
				break;
			default:
				if (LastCmdType == STK_PAC_SETUPCALL && bAccepted)
				{
					// Dialing failed
					bDialSucc = FALSE;
				}

				// Close waiting window
				STK_WaitWin(hStkWndApp, FALSE);
				break;
			}
		}
		break;

	case WM_STK_RESP_ACK:
		{
			int ret;

			// Close waiting window
			STK_WaitWin(hStkWndApp, FALSE);

			switch (wParam)
			{
			case ME_RS_SUCCESS:
				break;
			case ME_RS_FAILURE:
				ret = (int)lParam;
				STK_TipsWin(ret);
				break;
			default:
				break;
			}
		}
		break;

	case WM_STK_CONFIRMATION:
		switch (lParam)
		{
		// Left key down
		case 1:
			STK_Proat_Response(STK_PAC_SETUPCALL, STK_REP_SUCCESS, "", 0, 0);
			bAccepted = TRUE;
			break;
		// Right key down
		case 0:
			STK_Proat_Response(STK_PAC_SETUPCALL, STK_REP_NOTACCEPT, "", 0, 0);
			bAccepted = FALSE;
			break;
		// Timeout
		case 2:
			break;
		// End session
		case 3:
			STK_Proat_Response(STK_PAC_SETUPCALL, STK_REP_ENDSESSION, "", 0, 0);
			bAccepted = FALSE;
			break;
		}
		break;

	case WM_STK_CALLSETUP:
		switch (lParam)
		{
		// Left key down
		case 1:
			break;
		// Right key down
		case 0:
			STK_Proat_Response(STK_PAC_SETUPCALL, STK_REP_ENDSESSION, "", 0, 0);
			break;
		// Timeout
		case 2:
			break;
		// End session
		case 3:
			STK_Proat_Response(STK_PAC_SETUPCALL, STK_REP_ENDSESSION, "", 0, 0);
			break;
		}
		break;

	case WM_STK_SENDDTMF:
		switch (lParam)
		{
		// Left key down
		case 1:
			STK_Proat_Response(STK_PAC_SENDDTMF, STK_REP_SUCCESS, "", 0, 0);
			break;
		// Right key down
		case 0:
			// No backward command, so response with 16
			STK_Proat_Response(STK_PAC_SENDDTMF, STK_REP_ENDSESSION, "", 0, 0);
			break;
		// Timeout
		case 2:
			break;
		// End session
		case 3:
			STK_Proat_Response(STK_PAC_SENDDTMF, STK_REP_ENDSESSION, "", 0, 0);
			break;
		}
		break;

	case WM_STK_PLAYTONE:
		switch (lParam)
		{
		// Left key down
		case 1:
			STK_Proat_Response(STK_PAC_PLAYTONE, STK_REP_SUCCESS, "", 0, 0);
			break;
		// Right key down
		case 0:
			break;
		// Timeout
		case 2:
			// No timeout command, so response with 0
			STK_Proat_Response(STK_PAC_PLAYTONE, STK_REP_SUCCESS, "", 0, 0);
			break;
		// End session
		case 3:
			STK_Proat_Response(STK_PAC_PLAYTONE, STK_REP_ENDSESSION, "", 0, 0);
			break;
		}
		break;

	case WM_STK_DISPALYTEXT:
		KillTimer(hStkWndApp, STK_TIMER_MESSAGE_DELAY);

		// Auto deley, so response with 0
		STK_Proat_Response(STK_PAC_DISPLAYTEXT, STK_REP_SUCCESS, "", 0, 0);
		break;

	case WM_STK_TIMEOUT:
		switch (lParam)
		{
		// Left key down
		case 1:
			SendMessage(hStkWndApp, WM_COMMAND, IDC_OK, 0);
			break;
		// Right key down
		case 0:
			SendMessage(hStkWndApp, WM_COMMAND, IDC_BACK, 0);
			break;
		// Timeout
		case 2:
			SendMessage(hStkWndApp, WM_COMMAND, IDC_TIMEOUT, 0);
			break;
		// End session
		case 3:
			SendMessage(hStkWndApp, WM_COMMAND, IDC_ENDSESSION, 0);
			break;
		}
		break;

	case WM_STK_IMECHANGE:
		STK_InputChange();
		break;

	case WM_PAINT:
		{
			RECT StkRect;
			int nOldMode;

			hdc = BeginPaint(hWnd, NULL);
			GetClientRect(hWnd, &StkRect);
			ClearRect(hdc, &StkRect, COLOR_TRANSBK);
			nOldMode = SetBkMode(hdc, BM_NEWTRANSPARENT);

			// Show a GIF animation...
			///////////////////////////////////////////////////////////
			//if (bShowGif)
			//	PaintAnimatedGIF(hdc, hRequestGif);
			///////////////////////////////////////////////////////////

			SetBkMode(hdc, nOldMode);
			EndPaint(hWnd, NULL);
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		STK_Proat_Free();

		// Unregister!
		ME_UnRegisterIndication(hStkRegIndn);

		// Deallocate!
		free(STK_SendStruct.revbuf);

		// End a GIF animation
		EndAnimatedGIF(hRequestGif);

		// Kill the timer we set above
		KillTimer(hStkWndApp, STK_TIMER_MESSAGE_DELAY);
		KillTimer(hStkWndApp, STK_TIMER_INKEY_TIMEOUT);
		KillTimer(hStkWndApp, STK_TIMER_INPUT_TIMEOUT);
		KillTimer(hStkWndApp, STK_TIMER_SELECT_TIMEOUT);

		// Notify PROGMAN to close window
		DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hStkInstance);

		UnregisterClass("StkWndClass", NULL);
		hStkWndApp = NULL;
		hFrameWnd = NULL;
		break;

	case WM_SETLBTNTEXT:
		STK_OnSetLBtnText(hWnd, (int)LOWORD(wParam), (BOOL)HIWORD(wParam), (LPSTR)lParam);
		break;

	case WM_SETRBTNTEXT:
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0,
			((stricmp((LPSTR)lParam, "Cancel") == 0) ? (LPARAM)ML("Back") : lParam));
		break;

	default:
		lResult = PDADefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
    }

    return lResult;
}

/*********************************************************************
* Function	STK_SetButtonText
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_SetButtonText(void)
{
	if (LastCmdType == STK_PAC_SETUPMENU)
	{
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Exit"));
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Select"));
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
	}
	else if (LastCmdType == STK_PAC_SELECTITEM)
	{
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Select"));
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
	}
	else if (LastCmdType == STK_PAC_GETINPUT)
	{
		//SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));
		//SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Ok"));
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
	}
	else if (LastCmdType == STK_PAC_GETINKEY)
	{
		// SIM requests Yes/No answer
		if (IfYesNoRequest())
		{
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("No"));
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Yes"));
		}
		else
		{
			//SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));
			//SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Ok"));
		}
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
	}
	else
	{
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)ML(""));
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(""));
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
	}

	return 0;
}

/*********************************************************************
* Function	STK_InputChange
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_InputChange(void)
{
	int InputLen;

	if (LastCmdType == STK_PAC_GETINPUT)
	{
		int nSelStart, nSelEnd;

		SendMessage(hInputWnd, EM_GETSEL, (WPARAM)&nSelStart, (LPARAM)&nSelEnd);
		if (nSelStart == nSelEnd)
		{
			// No En
			InputLen = GetWindowTextLength(hInputWnd);
			if (InputLen < GetInput.responseMin)
			{
				// Left button hide
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(""));
			}
			else
			{
				// Left button show
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Ok"));
			}
		}
		else
		{
			// Process by ime
		}
	}
	else if (LastCmdType == STK_PAC_GETINKEY)
	{
		int nSelStart, nSelEnd;

		SendMessage(hInputWnd, EM_GETSEL, (WPARAM)&nSelStart, (LPARAM)&nSelEnd);
		if (nSelStart == nSelEnd)
		{
			// No En
			InputLen = GetWindowTextLength(hInputWnd);
			if (InputLen < 1)
			{
				// Left button hide
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(""));
			}
			else
			{
				// Left button show
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Ok"));
			}
		}
		else
		{
			// Process by ime
		}
	}

	return 0;
}

/*********************************************************************
* Function	STK_OnSetLBtnText
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
static void STK_OnSetLBtnText(HWND hWnd, int nID, BOOL bEmpty, LPSTR pszText)
{
    switch (nID)
    {
    case IDC_STK_EDIT:
	case IDC_STK_NUMBER:
	case IDC_STK_HIDENUMBER:
        STK_InputChange();
        break;
    default:
        SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)pszText);
        break;
    }
}

/*********************************************************************
* Function	STK_AudioTonePlay
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_AudioTonePlay(int toneType, int timeout)
{
	return 0;
}

/*********************************************************************
* Function	STK_GetTextbyErrorID
* Purpose   
* Parameter	
* Return	char *
* Remarks	
**********************************************************************/
static char * STK_GetTextbyErrorID(int ErrID)
{
	switch (ErrID)
	{
	case 0:
		return "Phone failure";
	case 1:
		return "No connection to phone";
	case 2:
		return "Phone-adapter link reserved";
	case 3:
		return "Operation not allowed";
	case 4:
		return "Operation not supported";
	case 5:
		return "PH-SIM PIN required";
	case 6:
		return "PH-FSIM PIN required";
	case 7:
		return "PH-FSIM PUK required";
	case 10:
		return "SIM not inserted";
	case 11:
		return "SIM PIN required";
	case 12:
		return "SIM PUK required";
	case 13:
		return "SIM failure";
	case 14:
		return "SIM busy";
	case 15:
		return "SIM wrong";
	case 16:
		return "Incorrect password";
	case 17:
		return "SIM PIN2 required";
	case 18:
		return "SIM PUK2 required";
	case 20:
		return "Memory full";
	case 21:
		return "Invalid index";
	case 22:
		return "Not found";
	case 23:
		return "Memory failure";
	case 24:
		return "Text string too long";
	case 25:
		return "Invalid characters in text string";
	case 26:
		return "Dial string too long";
	case 27:
		return "Invalid characters in dial string";
	case 30:
		return "No network service";
	case 31:
		return "Network timeout";
	case 32:
		return "Network not allowed emergency calls only";
	case 40:
		return "Network personalization PIN required";
	case 41:
		return "Network personalization PUK required";
	case 42:
		return "Network subset personalization PIN required";
	case 43:
		return "Network subset personalization PUK required";
	case 44:
		return "Service provider personalization PIN required";
	case 45:
		return "Service provider personalization PUK required";
	case 46:
		return "Corporate personalization PIN required";
	case 47:
		return "Corporate personalization PUK required";
	case 48:
		return "Master phone code required";
	case 100:
		return "Unknown";
	case 132:
		return "Service option not supported";
	case 133:
		return "Requested service option not subscribed";
	case 134:
		return "Service option temporarily out of order";
	case 256:
		return "Operation temporary not allowed";
	case 257:
		return "Call barred";
	case 258:
		return "Phone is busy";
	case 259:
		return "User abort";
	case 260:
		return "Invalid dial string";
	case 261:
		return "SS not executed";
	case 262:
		return "SIM blocked";
	case 263:
		return "Invalid block";
	default:
		ASSERT(0);
		break;
	}

	return "";
}

/*********************************************************************
* Function	STK_App_CreateWindow
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_CreateWindow(HWND hWnd)
{
	RECT StkRect;
	int StkWidth, StkHeight;

	GetClientRect(hWnd, &StkRect);
	StkWidth = StkRect.right - StkRect.left;
	StkHeight = StkRect.bottom - StkRect.top;

	// 140*71
	hRequestGif = StartAnimatedGIFFromFile(hWnd, IDS_STK_REQUESTING,
		(StkWidth - 140) / 2,
		(StkHeight - 71) / 2,
		DM_NONE);

	// Create listbox window
	hMenuList = CreateWindow("LISTBOX", "",
		WS_CHILD|LBS_BITMAP|WS_VSCROLL,
		0, 0, StkWidth, StkHeight,
		hWnd, (HMENU)IDC_STK_MENU, NULL, NULL);
	if (hMenuList == NULL)
		return -1;

	// Create static control
	hStaticText = CreateWindow("STATIC", "",
		WS_CHILD,
		5, 5, StkWidth-10, StkHeight-10,
		hWnd, (HMENU)IDC_STK_TEXT, NULL, NULL);

	// For characters from SMS default alphabet & UCS2 alphabet
	memset(&ImeEdit, 0x0, sizeof(IMEEDIT));
	ImeEdit.hwndNotify = (HWND)hWnd;
	ImeEdit.dwAttrib = 0;
	ImeEdit.dwAscTextMax = 0;
	ImeEdit.dwUniTextMax = 0;
	ImeEdit.wPageMax = 0;
	ImeEdit.pszImeName = NULL;
	ImeEdit.pszCharSet = NULL;
	ImeEdit.pszTitle = NULL;
	ImeEdit.uMsgSetText = NULL;

	// For digits only(0-9, *, #, and +)
	memset(&ImenumEdit, 0x0, sizeof(IMEEDIT));
	ImenumEdit.hwndNotify = (HWND)hWnd;
	ImenumEdit.dwAttrib = 0;
	ImenumEdit.dwAscTextMax = 0;
	ImenumEdit.dwUniTextMax = 0;
	ImenumEdit.wPageMax = 0;
	ImenumEdit.pszImeName = "Phone";
	ImenumEdit.pszCharSet = "1234567890*#+";
	ImenumEdit.pszTitle = NULL;
	ImenumEdit.uMsgSetText = NULL;

	// For hidden mode(0-9, *, #)
	memset(&ImehidenumEdit, 0x0, sizeof(IMEEDIT));
	ImehidenumEdit.hwndNotify = (HWND)hWnd;
	ImehidenumEdit.dwAttrib = 0;
	ImehidenumEdit.dwAscTextMax = 0;
	ImehidenumEdit.dwUniTextMax = 0;
	ImehidenumEdit.wPageMax = 0;
	ImehidenumEdit.pszImeName = "Phone";
	ImehidenumEdit.pszCharSet = "1234567890*#";
	ImehidenumEdit.pszTitle = NULL;
	ImehidenumEdit.uMsgSetText = NULL;

	// Create alphabet IMEEDIT
	hInputEdit = CreateWindow("IMEEDIT", "",
		WS_CHILD|WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL,
		2, 60, StkWidth, StkHeight-60,
		hWnd, (HMENU)IDC_STK_EDIT, NULL, (PVOID)&ImeEdit);

	InputEditInfo.lStyle = GetWindowLong(hInputEdit, GWL_STYLE);
	InputEditInfo.nKeyBoardType = hInputEdit;
	strcpy(InputEditInfo.szName, IDS_STK_SIMTOOLKIT);
	SetWindowLong(hInputEdit, GWL_USERDATA, (LONG)&InputEditInfo);

	// Create digits IMEEDIT
	hInputNumber = CreateWindow("IMEEDIT", "",
		WS_CHILD|WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL,
		2, 60, StkWidth, StkHeight-60,
		hWnd, (HMENU)IDC_STK_NUMBER, NULL, (PVOID)&ImenumEdit);

	InputNumberInfo.lStyle = GetWindowLong(hInputNumber, GWL_STYLE);
	InputNumberInfo.nKeyBoardType = hInputNumber;
	strcpy(InputNumberInfo.szName, IDS_STK_SIMTOOLKIT);
	SetWindowLong(hInputNumber, GWL_USERDATA, (LONG)&InputNumberInfo);

	// Create hidden mode IMEEDIT
	hInputHideNumber = CreateWindow("IMEEDIT", "",
		WS_CHILD|ES_AUTOHSCROLL|ES_PASSWORD,
		5, 60, StkWidth-10, 28,
		hWnd, (HMENU)IDC_STK_HIDENUMBER, NULL, (PVOID)&ImehidenumEdit);

	InputHideNumberInfo.lStyle = GetWindowLong(hInputHideNumber, GWL_STYLE);
	InputHideNumberInfo.nKeyBoardType = hInputHideNumber;
	strcpy(InputHideNumberInfo.szName, IDS_STK_SIMTOOLKIT);
	SetWindowLong(hInputHideNumber, GWL_USERDATA, (LONG)&InputHideNumberInfo);

	return 0;
}

/*********************************************************************
* Function	STK_App_ShowWindow
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_ShowWindow(int CmdType)
{
	// First hide all windows
	STK_App_HideWindow();

	switch (CmdType)
	{
	case STK_PAC_REFRESH:
		STK_App_Show_Refresh();
		break;

	case STK_PAC_SETUPEVENTLIST:
		STK_App_Show_EventList();
		break;

	case STK_PAC_SETUPCALL:
		STK_App_Show_SetupCall();
		break;

	case STK_PAC_SENDSS:
		STK_App_Show_SS();
		break;

	case STK_PAC_SENDUSSD:
		STK_App_Show_USSD();
		break;

	case STK_PAC_SENDSHORTMESSAGE:
		STK_App_Show_SMS();
		break;

	case STK_PAC_SENDDTMF:
		STK_App_Show_DTMF();
		break;

	case STK_PAC_LAUNCHBROWSER:
		STK_App_Show_Browser();
		break;

	case STK_PAC_PLAYTONE:
		STK_App_Show_PlayTone();
		break;

	case STK_PAC_DISPLAYTEXT:
		STK_App_Show_DispText();
		break;

	case STK_PAC_GETINKEY:
		STK_App_Show_Inkey();
		break;

	case STK_PAC_GETINPUT:
		STK_App_Show_Input();
		break;

	case STK_PAC_SELECTITEM:
		STK_App_Show_SubMenu();
		break;

	case STK_PAC_SETUPMENU:
		STK_App_Show_MainMenu();
		break;

	case STK_PAC_SETUPIDLEMODETEXT:
		STK_App_Show_IdleModeText();
		break;

	default:
		break;
	}

	return 0;
}

/*********************************************************************
* Function	STK_App_HideWindow
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_HideWindow(void)
{
	ShowWindow(hMenuList, SW_HIDE);
	ShowWindow(hStaticText, SW_HIDE);
	ShowWindow(hInputEdit, SW_HIDE);
	ShowWindow(hInputNumber, SW_HIDE);
	ShowWindow(hInputHideNumber, SW_HIDE);

	// Kill the timeout timer
	KillTimer(hStkWndApp, STK_TIMER_INKEY_TIMEOUT);
	KillTimer(hStkWndApp, STK_TIMER_INPUT_TIMEOUT);
	KillTimer(hStkWndApp, STK_TIMER_SELECT_TIMEOUT);

	return 0;
}

/*********************************************************************
* Function	STK_App_Show_Refresh
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_Refresh(void)
{
	return 0;
}

/*********************************************************************
* Function	STK_App_Show_EventList
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_EventList(void)
{
	return 0;
}

/*********************************************************************
* Function	STK_App_Show_SetupCall
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_SetupCall(void)
{
	int callCount = 0;
	BOOL doCall = FALSE, endActive = FALSE, putOnHold = FALSE;
	char Prompt[80];

	// Judge whether this call shoulded by setuped
	switch (SetupCall.callType)
	{
	case IfNoOtherCalls:
		if (callCount == 0)
			doCall = TRUE;
		break;
	case PutOnHold:
		doCall = TRUE;
		if (callCount > 0)
			putOnHold = TRUE;
		break;
	case Disconnect:
		doCall = TRUE;
		if (callCount > 0)
			endActive = TRUE;
		break;
	}

	if (!doCall)
	{
		// Screen is busy
		STK_Proat_Response(STK_PAC_SETUPCALL, STK_REP_SCREENBUSY, "", 0, 0);
		return 0;
	}

	// Alpha identifier provided by SIM
	if (SetupCall.calledNumberLen > 0)
	{
		int len;
		char DestStr[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)SetupCall.calledNumber, SetupCall.calledNumberLen/2,
			(LPSTR)DestStr, SetupCall.calledNumberLen,
			0, 0);
		*(DestStr+len) = 0;

		strcpy(PhoneNumber, DestStr);
#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_App_Show_SetupCall: calledNumber = %s\r\n", DestStr);
#endif
	}
	else
	{
		PhoneNumber[0] = '\0';
	}

	// Alpha identifier provided by SIM
	if (SetupCall.confirmationLen > 0)
	{
		int len;
		char DestStr[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)SetupCall.confirmationText, SetupCall.confirmationLen/2,
			(LPSTR)DestStr, SetupCall.confirmationLen,
			0, 0);
		*(DestStr+len) = 0;

#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_App_Show_SetupCall: confirmationText = %s\r\n", DestStr);
#endif
		// Requesting window
		PLXTipsWinForSTK(hFrameWnd, hStkWndApp, WM_STK_CONFIRMATION,
			DestStr, NULL,
			Notify_Request, ML("Ok"), ML("Back"), STK_WAITTIME_NONE);
	}
	else
	{
		strcpy(Prompt, "Do you wish to continue dialing?\r\n");
		if (endActive)
			strcat(Prompt, "All active calls will be ended.");

		// Requesting window
		PLXTipsWinForSTK(hFrameWnd, hStkWndApp, WM_STK_CONFIRMATION,
			Prompt, NULL,
			Notify_Request, ML("Ok"), ML("Back"), STK_WAITTIME_NONE);
	}

	return 0;
}

/*********************************************************************
* Function	STK_App_Show_SS
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_SS(void)
{
	// Alpha identifier provided by SIM
	if (SendSS.textLen > 0)
	{
		int len;
		char DestStr[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)SendSS.text, SendSS.textLen/2,
			(LPSTR)DestStr, SendSS.textLen,
			0, 0);
		*(DestStr+len) = 0;

#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_App_Show_SS: text = %s\r\n", DestStr);
#endif
		// Notify info window
		PLXTipsWin(hFrameWnd, hStkWndApp, WM_STK_TIMEOUT,
			DestStr, NULL,
			Notify_Info, ML("Ok"), NULL, STK_WAITTIME_NONE);
	}
	else
	{
		// Show a GIF animation
		bShowGif = TRUE;

		// To close a blank window
		STK_Proat_Response(STK_PAC_SENDSS, STK_REP_SUCCESS, "", 0, 0);
	}

	return 0;
}

/*********************************************************************
* Function	STK_App_Show_USSD
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_USSD(void)
{
	// Alpha identifier provided by SIM
	if (SendUSSD.textLen > 0)
	{
		int len;
		char DestStr[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)SendUSSD.text, SendUSSD.textLen/2,
			(LPSTR)DestStr, SendUSSD.textLen,
			0, 0);
		*(DestStr+len) = 0;

#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_App_Show_USSD: text = %s\r\n", DestStr);
#endif
		// Notify info window
		PLXTipsWin(hFrameWnd, hStkWndApp, WM_STK_TIMEOUT,
			DestStr, NULL,
			Notify_Info, ML("Ok"), NULL, STK_WAITTIME_NONE);
	}
	else
	{
		// Show a GIF animation
		bShowGif = TRUE;

		// To close a blank window
		STK_Proat_Response(STK_PAC_SENDUSSD, STK_REP_SUCCESS, "", 0, 0);
	}

	return 0;
}

/*********************************************************************
* Function	STK_App_Show_SMS
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_SMS(void)
{
	// Alpha identifier provided by SIM
	if (SendShortMessage.infoLen > 0)
	{
		int len;
		char DestStr[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)SendShortMessage.textInfo, SendShortMessage.infoLen/2,
			(LPSTR)DestStr, SendShortMessage.infoLen,
			0, 0);
		*(DestStr+len) = 0;

#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_App_Show_SMS: textInfo = %s\r\n", DestStr);
#endif
		// Notify info window
		PLXTipsWin(hFrameWnd, hStkWndApp, WM_STK_TIMEOUT,
			DestStr, NULL,
			Notify_Info, ML("Ok"), NULL, STK_WAITTIME_NONE);
	}
	else
	{
		// Show a GIF animation
		bShowGif = TRUE;

		// To close a blank window
		STK_Proat_Response(STK_PAC_SENDSHORTMESSAGE, STK_REP_SUCCESS, "", 0, 0);
	}

	return 0;
}

/*********************************************************************
* Function	STK_App_Show_DTMF
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_DTMF(void)
{
	// Alpha identifier provided by SIM
	if (SendDTMF.textLen >= 0)
	{
		int len;
		char DestStr[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)SendDTMF.text, SendDTMF.textLen/2,
			(LPSTR)DestStr, SendDTMF.textLen,
			0, 0);
		*(DestStr+len) = 0;

#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_App_Show_DTMF: text = %s\r\n", DestStr);
#endif
		// Notify info window
		PLXTipsWinForSTK(hFrameWnd, hStkWndApp, WM_STK_SENDDTMF,
			DestStr, NULL,
			Notify_Info, ML("Ok"), ML("End"), STK_WAITTIME_NONE);
	}

	return 0;
}

/*********************************************************************
* Function	STK_App_Show_Browser
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_Browser(void)
{
	return 0;
}

/*********************************************************************
* Function	STK_App_Show_PlayTone
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_PlayTone(void)
{
	int toneType, timeout;

	// Get toneType
	toneType = PlayTone.tone;

	// Calculate timeout
	switch (PlayTone.durationUnit)
	{
	case unitMinute:
		timeout = 60 * 1000 * PlayTone.duration;
		break;
	case unitSecond:
		timeout = 1000 * PlayTone.duration;
		break;
	case unit10Secs:
		timeout = 100 * PlayTone.duration;
		break;
	default:
		timeout = 0;
		break;
	}

	// At least 4 seconds
	if (timeout < 1000 * 4)
		timeout = 1000 * 4;

	// Alpha identifier provided by SIM
	if (PlayTone.infoLen >= 0)
	{
		int len, waittime;
		char DestStr[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)PlayTone.infoText, PlayTone.infoLen/2,
			(LPSTR)DestStr, PlayTone.infoLen,
			0, 0);
		*(DestStr+len) = 0;

		waittime = timeout / 100;
#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_App_Show_PlayTone: infoText = %s\r\n", DestStr);
#endif
		// Notify info window
		PLXTipsWinForSTK(hFrameWnd, hStkWndApp, WM_STK_PLAYTONE,
			DestStr, NULL,
			Notify_Info, ML("Ok"), NULL, waittime);

		// Playing...
		STK_AudioTonePlay(toneType, timeout);
	}

	return 0;
}

/*********************************************************************
* Function	STK_App_Show_DispText
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_DispText(void)
{
	// Alpha identifier provided by SIM
	if (DisplayText.textLen >= 0)
	{
		int len;
		char DestStr[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)DisplayText.text, DisplayText.textLen/2,
			(LPSTR)DestStr, DisplayText.textLen,
			0, 0);
		*(DestStr+len) = 0;

#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_App_Show_DispText: text = %s\r\n", DestStr);
#endif
		if (IfMessageDelay())
		{
			// Clear message after a delay
			SetTimer(hStkWndApp, STK_TIMER_MESSAGE_DELAY, 2000, NULL);

			// 2 seconds timeout
			PLXTipsWinForSTK(hFrameWnd, hStkWndApp, WM_STK_DISPALYTEXT,
				DestStr, NULL,
				Notify_Info, ML("Ok"), ML("Back"), WAITTIMEOUT);

			// Sustained display text
			if (IfSustainedText())
			{
				STK_Proat_Response(STK_PAC_DISPLAYTEXT, STK_REP_SUCCESS, "", 0, 0);
			}

			return 0;
		}

		// 60 seconds(no response from user timeout)
		PLXTipsWinForSTK(hFrameWnd, hStkWndApp, WM_STK_TIMEOUT,
			DestStr, NULL,
			Notify_Info, ML("Ok"), ML("Back"), STK_WAITTIME_60S);

		// Sustained display text
		if (IfSustainedText())
		{
			STK_Proat_Response(STK_PAC_DISPLAYTEXT, STK_REP_SUCCESS, "", 0, 0);
		}
	}

	return 0;
}

/*********************************************************************
* Function	STK_App_Show_Inkey
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_Inkey(void)
{
	int nLimitText;

	// Set timeout timer(60s)
	SetTimer(hStkWndApp, STK_TIMER_INKEY_TIMEOUT, 60000, NULL);

	// Alpha identifier provided by SIM
	if (GetInkey.textLen > 0)
	{
		int len;
		char DestStr[256];
		char /*Digits[8], */Prompt[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)GetInkey.text, GetInkey.textLen/2,
			(LPSTR)DestStr, GetInkey.textLen,
			0, 0);
		*(DestStr+len) = 0;

		///////////////////////////////////////////////////////////
		//if (GetInkey.inputFormat == fmtDigital)
		//	strcpy(Digits, "数字");
		//else
		//	strcpy(Digits, "字符");
		///////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////
		//sprintf(Prompt, "%s\r\n(%s %d个)", DestStr, Digits, 1);
		///////////////////////////////////////////////////////////

		STK_FormatStr(DestStr, Prompt);
		SetWindowText(hStaticText, Prompt);
#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_App_Show_Inkey: text = %s\r\n", DestStr);
#endif
		ShowWindow(hStaticText, SW_SHOW);
	}

	// SIM requests Yes/No answer
	if (!IfYesNoRequest())
	{
		if (GetInkey.inputFormat == fmtDigital)
		{
			hInputWnd = hInputNumber;

			nLimitText = 2;
			SendMessage(hInputWnd, EM_LIMITTEXT, nLimitText, 0);
		}
		else
		{
			hInputWnd = hInputEdit;

			nLimitText = 1;
			SendMessage(hInputWnd, EM_LIMITTEXT, nLimitText, 0);
		}

		SetWindowText(hInputWnd, "");
		ShowWindow(hInputWnd, SW_SHOW);
		SetFocus(hInputWnd);
	}

	return 0;
}

/*********************************************************************
* Function	STK_App_Show_Input
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_Input(void)
{
	int nLimitText;

	// Set timeout timer(3min)
	SetTimer(hStkWndApp, STK_TIMER_INPUT_TIMEOUT, 180000, NULL);

	// Alpha identifier provided by SIM
	if (GetInput.textLen > 0)
	{
		int len;
		char DestStr[256];
		char /*Digits[8], */Prompt[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)GetInput.text, GetInput.textLen/2,
			(LPSTR)DestStr, GetInput.textLen,
			0, 0);
		*(DestStr+len) = 0;

		///////////////////////////////////////////////////////////
		//if (GetInput.inputFormat == fmtDigital)
		//	strcpy(Digits, "数字");
		//else
		//	strcpy(Digits, "字符");
		///////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////
		//if (GetInput.responseMin != GetInput.responseMax)
		//	sprintf(Prompt, "%s\r\n(%s %d-%d个)", DestStr, Digits, GetInput.responseMin, GetInput.responseMax);
		//else
		//	sprintf(Prompt, "%s\r\n(%s %d个)", DestStr, Digits, GetInput.responseMin);
		///////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////
		//if (GetInput.responseMin != GetInput.responseMax)
		//	sprintf(Prompt, "%s\r\n(%d-%d)", DestStr, GetInput.responseMin, GetInput.responseMax);
		//else
		//	sprintf(Prompt, "%s\r\n(%d)", DestStr, GetInput.responseMin);
		///////////////////////////////////////////////////////////

		STK_FormatStr(DestStr, Prompt);
		SetWindowText(hStaticText, Prompt);
#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_App_Show_Input: text = %s\r\n", DestStr);
#endif
		ShowWindow(hStaticText, SW_SHOW);
	}

	if (!IfEcho())
	{
		// Hidden mode
		if (GetInput.inputFormat == fmtDigital)
		{
			hInputWnd = hInputHideNumber;

			nLimitText = GetInput.responseMax+1;
			SendMessage(hInputWnd, EM_LIMITTEXT, nLimitText, 0);
		}
		else
			hInputWnd = NULL;
	}
	else
	{
		// Echo mode
		if (GetInput.inputFormat == fmtDigital)
		{
			hInputWnd = hInputNumber;

			nLimitText = GetInput.responseMax+1;
			SendMessage(hInputWnd, EM_LIMITTEXT, nLimitText, 0);
		}
		else
		{
			hInputWnd = hInputEdit;

			nLimitText = GetInput.responseMax;
			SendMessage(hInputWnd, EM_LIMITTEXT, nLimitText, 0);
		}
	}

	SetWindowText(hInputWnd, "");
	ShowWindow(hInputWnd, SW_SHOW);
	SetFocus(hInputWnd);

	return 0;
}

/*********************************************************************
* Function	STK_App_Show_SubMenu
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_SubMenu(void)
{
	int i;

	// Set timeout timer(60s)
	SetTimer(hStkWndApp, STK_TIMER_SELECT_TIMEOUT, 60000, NULL);

	// Alpha identifier provided by SIM
	if (SelectItem.titleLen > 0)
	{
		int len;
		char DestStr[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)SelectItem.titleText, SelectItem.titleLen/2,
			(LPSTR)DestStr, SelectItem.titleLen,
			0, 0);
		*(DestStr+len) = 0;

		SetWindowText(hFrameWnd, DestStr);
#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎【Sub Menu: titleText = %s...】\r\n", DestStr);
#endif
	}
 
	// Reset all contents
	SendMessage(hMenuList, LB_RESETCONTENT, 0, 0L);

	// Alpha identifier provided by SIM
	for (i=0; i<SelectItem.numOfItems; i++)
	{
		int len;
		char DestStr[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)SelectMenuItem[i].itemText, SelectMenuItem[i].itemLen/2,
			(LPSTR)DestStr, SelectMenuItem[i].itemLen,
			0, 0);
		*(DestStr+len) = 0;

		SendMessage(hMenuList, LB_INSERTSTRING, (WPARAM)i, (LPARAM)DestStr);
#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎【Sub Menu: itemText%d = %s...】\r\n", i+1, DestStr);
#endif
	}

	// Set cursor
	SendMessage(hMenuList, LB_SETCURSEL, (WPARAM)SelectItem.defaultItemId, 0L);

	ShowWindow(hMenuList, SW_SHOW);
	SetFocus(hMenuList);

	return 0;
}

/*********************************************************************
* Function	STK_App_Show_MainMenu
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_MainMenu(void)
{
	int i;

	// Alpha identifier provided by SIM
	if (SetupMenu.titleLen > 0)
	{
		int len;
		char DestStr[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)SetupMenu.titleText, SetupMenu.titleLen/2,
			(LPSTR)DestStr, SetupMenu.titleLen,
			0, 0);
		*(DestStr+len) = 0;

		SetWindowText(hFrameWnd, DestStr);
#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎【Main Menu: titleText = %s...】\r\n", DestStr);
#endif
	}

	// Reset all contents
	SendMessage(hMenuList, LB_RESETCONTENT, 0, 0L);

	// To remove the existing menu from the menu system
	if (SetupMenu.numOfItems == 0)
	{
		PLXTipsWin(NULL, NULL, 0,
			IDS_STK_MENUREMOVE, IDS_STK_SIMTOOLKIT,
			Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);

		SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (LPARAM)hStkWndApp, 0);
		SendMessage(hStkWndApp, WM_CLOSE, 0, 0);

		DeleteSTKAppItem();
		return 0;
	}

	// Alpha identifier provided by SIM
	for (i=0; i<SetupMenu.numOfItems; i++)
	{
		int len;
		char DestStr[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)SetupMenuItem[i].itemText,
			SetupMenuItem[i].itemLen/2,
			(LPSTR)DestStr, SetupMenuItem[i].itemLen,
			0, 0);
		*(DestStr+len) = 0;

		SendMessage(hMenuList, LB_INSERTSTRING, (WPARAM)i, (LPARAM)DestStr);
#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎【Main Menu: itemText%d = %s...】\r\n", i+1, DestStr);
#endif
	}

	// Set cursor
	SendMessage(hMenuList, LB_SETCURSEL, (WPARAM)0, 0L);

	ShowWindow(hMenuList, SW_SHOW);
	SetFocus(hMenuList);

	return 0;
}

/*********************************************************************
* Function	STK_App_Show_IdleModeText
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_App_Show_IdleModeText(void)
{
	/*
	由STK隐藏窗口负责通知程序管理器显示
	*/
#if 0
	// Alpha identifier provided by SIM
	if (SetupIdleModeText.textLen > 0)
	{
		int len;
		char DestStr[256];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)SetupIdleModeText.text, SetupIdleModeText.textLen/2,
			(LPSTR)DestStr, SetupIdleModeText.textLen,
			0, 0);
		*(DestStr+len) = 0;

#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_App_Show_SetupIdleModeText: text = %s\r\n", DestStr);
#endif
		// Notify info window
		PLXTipsWinForSTK(hFrameWnd, hStkWndApp, WM_STK_TIMEOUT,
			DestStr, NULL,
			Notify_Info, ML("Ok"), NULL, STK_WAITTIME_NONE);
	}
#endif // #if 0

	return 0;
}

/*********************************************************************
* Function	STK_Send_PacInfo
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_Send_PacInfo(int CmdType)
{
	// Notification: SIM App returns to main menu
	if (CmdType == STK_NOT_SIMAPP2MAINMENU)
	{
#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_Send_PacInfo: 收到回主菜单消息\r\n");
#endif
		if (!bShowMenu)
		{
#ifdef _STK_DEBUG_
			printf("\r\n◎◎◎◎◎◎◎◎◎STK_Send_PacInfo: 不需显示菜单了\r\n");
#endif
			// 不需显示菜单了
			bShowMenu = TRUE;
			return 0;
		}

		// Setup call...
		bSetupCall = FALSE;
		if (LastCmdType == STK_PAC_SETUPCALL && bAccepted && bDialSucc)
			bSetupCall = TRUE;

		// Clear text
		SetWindowText(hStaticText, "");
		SetWindowText(hInputWnd, "");

		// Show Main Menu
		LastCmdType = STK_PAC_SETUPMENU;
		STK_App_ShowWindow(STK_PAC_SETUPMENU);

		// Set left/right/ok button text
		STK_SetButtonText();

		if (bSetupCall)
		{
			// Alpha identifier provided by SIM
			if (SetupCall.callSetupLen > 0)
			{
				int len;
				char DestStr[256];

				len = WideCharToMultiByte(CP_ACP, 0,
					(LPCWSTR)SetupCall.callSetupText, SetupCall.callSetupLen/2,
					(LPSTR)DestStr, SetupCall.callSetupLen,
					0, 0);
				*(DestStr+len) = 0;

#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_App_Show_SetupCall: callSetupText = %s\r\n", DestStr);
#endif
				// Notify info window
				PLXTipsWinForSTK(hFrameWnd, hStkWndApp, WM_STK_CALLSETUP,
					DestStr, NULL,
					Notify_Info, NULL, ML("End call"), STK_WAITTIME_NONE);
			}
			else
			{
				// Dialing...
				APP_CallPhoneNumberSTK(PhoneNumber);
			}
		}

#ifdef _STK_DEBUG_
		printf("\r\n◎◎◎◎◎◎◎◎◎STK_Send_PacInfo: 关闭等待窗口\r\n");
#endif
		// Close waiting window
		STK_WaitWin(hStkWndApp, FALSE);

		return 0;
	}
	else if (CmdType == STK_NOT_SIMRRESET)
	{
		// SIM removed/reset
		PLXTipsWin(NULL, NULL, 0,
			IDS_STK_SIMRESET, NULL,
			Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);

		return 0;
	}

	// Request...
	LastCmdType = CmdType;
	return STK_Proat_Request(CmdType);
}

/*********************************************************************
* Function	STK_Recv_PacInfo
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_Recv_PacInfo(void)
{
	char RecvBuf1[STK_MAC_STRINGCOMMAND];
	char RecvBuf2[STK_MAC_MENUCOUNT][STK_MAC_MENUCOMMAND];

	switch (LastCmdType)
	{
	case STK_PAC_REFRESH:
		// ME received buffer length
		memcpy(RecvBuf1, STK_SendStruct.revbuf, STK_MAC_STRINGCOMMAND);

		STK_Proat_String(STK_PAC_REFRESH, RecvBuf1);
		STK_App_ShowWindow(STK_PAC_REFRESH);
		break;

	case STK_PAC_SETUPEVENTLIST:
		// ME received buffer length
		memcpy(RecvBuf1, STK_SendStruct.revbuf, STK_MAC_STRINGCOMMAND);

		STK_Proat_String(STK_PAC_SETUPEVENTLIST, RecvBuf1);
		STK_App_ShowWindow(STK_PAC_SETUPEVENTLIST);
		break;

	case STK_PAC_SETUPCALL:
		// ME received buffer length
		memcpy(RecvBuf1, STK_SendStruct.revbuf, STK_MAC_STRINGCOMMAND);

		STK_Proat_String(STK_PAC_SETUPCALL, RecvBuf1);
		STK_App_ShowWindow(STK_PAC_SETUPCALL);
		break;

	case STK_PAC_SENDSS:
		// ME received buffer length
		memcpy(RecvBuf1, STK_SendStruct.revbuf, STK_MAC_STRINGCOMMAND);

		STK_Proat_String(STK_PAC_SENDSS, RecvBuf1);
		STK_App_ShowWindow(STK_PAC_SENDSS);
		break;

	case STK_PAC_SENDUSSD:
		// ME received buffer length
		memcpy(RecvBuf1, STK_SendStruct.revbuf, STK_MAC_STRINGCOMMAND);

		STK_Proat_String(STK_PAC_SENDUSSD, RecvBuf1);
		STK_App_ShowWindow(STK_PAC_SENDUSSD);
		break;

	case STK_PAC_SENDSHORTMESSAGE:
		// ME received buffer length
		memcpy(RecvBuf1, STK_SendStruct.revbuf, STK_MAC_STRINGCOMMAND);

		STK_Proat_String(STK_PAC_SENDSHORTMESSAGE, RecvBuf1);
		STK_App_ShowWindow(STK_PAC_SENDSHORTMESSAGE);
		break;

	case STK_PAC_SENDDTMF:
		// ME received buffer length
		memcpy(RecvBuf1, STK_SendStruct.revbuf, STK_MAC_STRINGCOMMAND);

		STK_Proat_String(STK_PAC_SENDDTMF, RecvBuf1);
		STK_App_ShowWindow(STK_PAC_SENDDTMF);
		break;

	case STK_PAC_PLAYTONE:
		// ME received buffer length
		memcpy(RecvBuf1, STK_SendStruct.revbuf, STK_MAC_STRINGCOMMAND);

		STK_Proat_String(STK_PAC_PLAYTONE, RecvBuf1);
		STK_App_ShowWindow(STK_PAC_PLAYTONE);
		break;

	case STK_PAC_DISPLAYTEXT:
		// ME received buffer length
		memcpy(RecvBuf1, STK_SendStruct.revbuf, STK_MAC_STRINGCOMMAND);

		STK_Proat_String(STK_PAC_DISPLAYTEXT, RecvBuf1);
		STK_App_ShowWindow(STK_PAC_DISPLAYTEXT);
		break;

	case STK_PAC_GETINKEY:
		// ME received buffer length
		memcpy(RecvBuf1, STK_SendStruct.revbuf, STK_MAC_STRINGCOMMAND);

		STK_Proat_String(STK_PAC_GETINKEY, RecvBuf1);
		STK_App_ShowWindow(STK_PAC_GETINKEY);
		break;

	case STK_PAC_GETINPUT:
		// ME received buffer length
		memcpy(RecvBuf1, STK_SendStruct.revbuf, STK_MAC_STRINGCOMMAND);

		STK_Proat_String(STK_PAC_GETINPUT, RecvBuf1);
		STK_App_ShowWindow(STK_PAC_GETINPUT);
		break;

	case STK_PAC_SELECTITEM:
		// ME received buffer length
		memcpy(RecvBuf2, STK_SendStruct.revbuf, STK_MAC_MENUCOUNT*STK_MAC_MENUCOMMAND);

		STK_Proat_SelectItem(RecvBuf2, &SelectItem, (PMENUITEM)SelectMenuItem);
		STK_App_ShowWindow(STK_PAC_SELECTITEM);
		break;

	case STK_PAC_SETUPMENU:
		// ME received buffer length
		memcpy(RecvBuf2, STK_SendStruct.revbuf, STK_MAC_MENUCOUNT*STK_MAC_MENUCOMMAND);

		STK_Proat_SetupMenu(RecvBuf2, &SetupMenu, (PMENUITEM)SetupMenuItem);
		STK_App_ShowWindow(STK_PAC_SETUPMENU);
		break;

	case STK_PAC_SETUPIDLEMODETEXT:
		// ME received buffer length
		memcpy(RecvBuf1, STK_SendStruct.revbuf, STK_MAC_STRINGCOMMAND);

		STK_Proat_String(STK_PAC_SETUPIDLEMODETEXT, RecvBuf1);
		STK_App_ShowWindow(STK_PAC_SETUPIDLEMODETEXT);
		break;

	default:
		break;
	}

	return 0;
}

/*********************************************************************
* Function	STK_TipsWin
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
static void STK_TipsWin(int ErrCode)
{
	switch (ErrCode)
	{
	case 0:
		break;
	case -1:
		ASSERT(0);
		break;
	case -2:
		// Invalid parameter
		PLXTipsWin(NULL, NULL, 0,
			IDS_STK_INVALIDPARAMETER, NULL,
			Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
		break;
	default:
		PLXTipsWin(NULL, NULL, 0,
			STK_GetTextbyErrorID(ErrCode), NULL,
			Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
		break;
	}
}

/*********************************************************************
* Function	STK_Send_Resp_Ack
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_Send_Resp_Ack(void)
{
	char SendStr[32];

	switch (LastCmdType)
	{
	case STK_PAC_SETUPMENU:
		if (bActivate)
		{
			// 不需显示菜单了
			bShowMenu = TRUE;

			STK_WaitWin(hStkWndApp, FALSE);
			return 0;
		}

		bActivate = TRUE;
		sprintf(SendStr, "AT^SSTR=%d,0\r", LastCmdType);
		break;

	default:
		if (!bShowGif)
			STK_WaitWin(hStkWndApp, FALSE);
		return 0;
	}

	strcpy(STK_SendStruct.cmd, SendStr);
	STK_SendStruct.timeout = 10000;
	if (-1 == ME_STK_Send(hStkWndApp, WM_STK_RESP_ACK, &STK_SendStruct))
		return -1;

	return 0;
}

/*********************************************************************
* Function	STK_Send_Resp_Ok
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_Send_Resp_Ok(void)
{
	int ret, len;

	switch (LastCmdType)
	{
	case STK_PAC_REFRESH:
	case STK_PAC_SETUPEVENTLIST:
	case STK_PAC_SETUPCALL:
	case STK_PAC_SENDSS:
	case STK_PAC_SENDUSSD:
	case STK_PAC_SENDSHORTMESSAGE:
	case STK_PAC_SENDDTMF:
	case STK_PAC_LAUNCHBROWSER:
	case STK_PAC_PLAYTONE:
	case STK_PAC_SETUPIDLEMODETEXT:
		ret = STK_Proat_Response(LastCmdType, STK_REP_SUCCESS, "", 0, 0);
		break;
	case STK_PAC_DISPLAYTEXT:
		// Sustained display text
		ret = 0;
		if (!IfSustainedText())
		{
			ret = STK_Proat_Response(STK_PAC_DISPLAYTEXT, STK_REP_SUCCESS, "", 0, 0);
		}
		break;
	case STK_PAC_GETINKEY:
		// SIM requests Yes/No answer
		if (IfYesNoRequest())
		{
			char EncodeText[10];

			len = MultiByteToWideChar(CP_ACP, 0,
				(LPCSTR)"Y", strlen("Y"),
				(LPWSTR)EncodeText, strlen("Y")*2);
			*(EncodeText+2*len) = 0;
			*(EncodeText+2*len+1) = 0;
			*(EncodeText+2*len+2) = 0;
			*(EncodeText+2*len+3) = 0;

			ret = STK_Proat_Response(STK_PAC_GETINKEY, STK_REP_SUCCESS, EncodeText, strlen("Y")*2, 0);
		}
		else
		{
			char InputText[STK_INPUT_LIMIT_TEXT+1];
			char EncodeText[2*STK_INPUT_LIMIT_TEXT+4];

			GetWindowText(hInputWnd, InputText, STK_INPUT_LIMIT_TEXT+1);
			if ((int)strlen(InputText) < 1)
				return -2;

			len = MultiByteToWideChar(CP_ACP, 0,
				(LPCSTR)InputText, strlen(InputText),
				(LPWSTR)EncodeText, strlen(InputText)*2);
			*(EncodeText+2*len) = 0;
			*(EncodeText+2*len+1) = 0;
			*(EncodeText+2*len+2) = 0;
			*(EncodeText+2*len+3) = 0;

			ret = STK_Proat_Response(STK_PAC_GETINKEY, STK_REP_SUCCESS, EncodeText, strlen(InputText)*2, 0);
		}
		break;
	case STK_PAC_GETINPUT:
		{
			char InputText[STK_INPUT_LIMIT_TEXT+1];
			char EncodeText[2*STK_INPUT_LIMIT_TEXT+4];

			GetWindowText(hInputWnd, InputText, STK_INPUT_LIMIT_TEXT+1);
			if ((int)strlen(InputText) < GetInput.responseMin)
				return -2;

			len = MultiByteToWideChar(CP_ACP, 0,
				(LPCSTR)InputText, strlen(InputText),
				(LPWSTR)EncodeText, strlen(InputText)*2);
			*(EncodeText+2*len) = 0;
			*(EncodeText+2*len+1) = 0;
			*(EncodeText+2*len+2) = 0;
			*(EncodeText+2*len+3) = 0;

			ret = STK_Proat_Response(STK_PAC_GETINPUT, STK_REP_SUCCESS, EncodeText, strlen(InputText)*2, 0);
		}
		break;
	case STK_PAC_SELECTITEM:
		{
			int selID;

			selID = (int)SendMessage(hMenuList, LB_GETCURSEL, 0, 0L);
			if (selID == -1)
				return 0;
			ret = STK_Proat_Response(STK_PAC_SELECTITEM, STK_REP_SUCCESS, "", 0, SelectMenuItem[selID].itemId);
		}
		break;
	case STK_PAC_SETUPMENU:
		{
			int selID;

			selID = (int)SendMessage(hMenuList, LB_GETCURSEL, 0, 0L);
			if (selID == -1)
				return 0;
			ret = STK_Proat_Response(STK_PAC_SETUPMENU, STK_REP_SUCCESS, "", 0, SetupMenuItem[selID].itemId);
		}
		break;
	default:
		return 0;
	}

	return ret;
}

/*********************************************************************
* Function	STK_Send_Resp_Back
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_Send_Resp_Back(void)
{
	int ret, len;

	switch (LastCmdType)
	{
	case STK_PAC_REFRESH:
	case STK_PAC_SETUPEVENTLIST:
	case STK_PAC_SENDSS:
	case STK_PAC_SENDUSSD:
	case STK_PAC_SENDSHORTMESSAGE:
	case STK_PAC_SENDDTMF:
	case STK_PAC_PLAYTONE:
	case STK_PAC_SETUPIDLEMODETEXT:
		ret = 0;
		break;
	case STK_PAC_SETUPCALL:
	case STK_PAC_LAUNCHBROWSER:
		ret = STK_Proat_Response(LastCmdType, STK_REP_NOTACCEPT, "", 0, 0);
		break;
	case STK_PAC_DISPLAYTEXT:
		// Sustained display text
		ret = 0;
		if (!IfSustainedText())
		{
			ret = STK_Proat_Response(STK_PAC_DISPLAYTEXT, STK_REP_BACKWARD, "", 0, 0);
		}
		break;
	case STK_PAC_GETINKEY:
		// SIM requests Yes/No answer
		if (IfYesNoRequest())
		{
			char EncodeText[10];

			len = MultiByteToWideChar(CP_ACP, 0,
				(LPCSTR)"N", strlen("N"),
				(LPWSTR)EncodeText, strlen("N")*2);
			*(EncodeText+2*len) = 0;
			*(EncodeText+2*len+1) = 0;
			*(EncodeText+2*len+2) = 0;
			*(EncodeText+2*len+3) = 0;

			ret = STK_Proat_Response(STK_PAC_GETINKEY, STK_REP_SUCCESS, EncodeText, strlen("N")*2, 0);
		}
		else
		{
			ret = STK_Proat_Response(STK_PAC_GETINKEY, STK_REP_BACKWARD, "", 0, 0);
		}
		break;
	case STK_PAC_GETINPUT:
	case STK_PAC_SELECTITEM:
		ret = STK_Proat_Response(LastCmdType, STK_REP_BACKWARD, "", 0, 0);
		break;
	case STK_PAC_SETUPMENU:
		SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (LPARAM)hStkWndApp, 0);
		SendMessage(hStkWndApp, WM_CLOSE, 0, 0);
		ret = 0;
		break;
	default:
		return 0;
	}

	return ret;
}

/*********************************************************************
* Function	STK_Send_Resp_End
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_Send_Resp_End(void)
{
	int ret;

	switch (LastCmdType)
	{
	case STK_PAC_REFRESH:
	case STK_PAC_SETUPEVENTLIST:
	case STK_PAC_SENDSS:
	case STK_PAC_SENDUSSD:
	case STK_PAC_SENDSHORTMESSAGE:
	case STK_PAC_LAUNCHBROWSER:
	case STK_PAC_SETUPMENU:
	case STK_PAC_SETUPIDLEMODETEXT:
		ret = 0;
		break;
	case STK_PAC_SETUPCALL:
	case STK_PAC_SENDDTMF:
	case STK_PAC_PLAYTONE:
		ret = STK_Proat_Response(LastCmdType, STK_REP_ENDSESSION, "", 0, 0);
		break;
	case STK_PAC_DISPLAYTEXT:
		// Sustained display text
		ret = 0;
		if (!IfSustainedText())
		{
			ret = STK_Proat_Response(STK_PAC_DISPLAYTEXT, STK_REP_ENDSESSION, "", 0, 0);
		}
		break;
	case STK_PAC_GETINKEY:
	case STK_PAC_GETINPUT:
	case STK_PAC_SELECTITEM:
		ret = STK_Proat_Response(LastCmdType, STK_REP_ENDSESSION, "", 0, 0);
		break;
	default:
		return 0;
	}

	return ret;
}

/*********************************************************************
* Function	STK_Send_Resp_Timeout
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_Send_Resp_Timeout(void)
{
	int ret;

	switch (LastCmdType)
	{
	case STK_PAC_REFRESH:
	case STK_PAC_SETUPEVENTLIST:
	case STK_PAC_SETUPCALL:
	case STK_PAC_SENDSS:
	case STK_PAC_SENDUSSD:
	case STK_PAC_SENDSHORTMESSAGE:
	case STK_PAC_SENDDTMF:
	case STK_PAC_LAUNCHBROWSER:
	case STK_PAC_PLAYTONE:
	case STK_PAC_SETUPMENU:
	case STK_PAC_SETUPIDLEMODETEXT:
		ret = 0;
		break;
	case STK_PAC_DISPLAYTEXT:
		// Sustained display text
		ret = 0;
		if (!IfSustainedText())
		{
			ret = STK_Proat_Response(STK_PAC_DISPLAYTEXT, STK_REP_NORESPONSE, "", 0, 0);
		}
		break;
	case STK_PAC_GETINKEY:
	case STK_PAC_GETINPUT:
	case STK_PAC_SELECTITEM:
		ret = STK_Proat_Response(LastCmdType, STK_REP_NORESPONSE, "", 0, 0);
		break;
	default:
		return 0;
	}

	return ret;
}

/*********************************************************************
* Function	STK_Send_Resp_Help
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_Send_Resp_Help(void)
{
	int ret;

	switch (LastCmdType)
	{
	case STK_PAC_REFRESH:
	case STK_PAC_SETUPEVENTLIST:
	case STK_PAC_SETUPCALL:
	case STK_PAC_SENDSS:
	case STK_PAC_SENDUSSD:
	case STK_PAC_SENDSHORTMESSAGE:
	case STK_PAC_SENDDTMF:
	case STK_PAC_LAUNCHBROWSER:
	case STK_PAC_PLAYTONE:
	case STK_PAC_SETUPIDLEMODETEXT:
		ret = 0;
		break;
	case STK_PAC_GETINKEY:
	case STK_PAC_GETINPUT:
		if (IfHelp(LastCmdType))
		{
			ret = STK_Proat_Response(LastCmdType, STK_REP_HELPREQUEST, "", 0, 0);
		}
		else
		{
			PLXTipsWin(NULL, NULL, 0,
				IDS_STK_NOHELPINFO, NULL,
				Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
			return 0;
		}
		break;
	case STK_PAC_SELECTITEM:
		if (IfHelp(STK_PAC_SELECTITEM))
		{
			int selID;

			selID = (int)SendMessage(hMenuList, LB_GETCURSEL, 0, 0L);
			ret = STK_Proat_Response(STK_PAC_SELECTITEM, STK_REP_HELPREQUEST, "", 0, SelectMenuItem[selID].itemId);
		}
		else
		{
			PLXTipsWin(NULL, NULL, 0,
				IDS_STK_NOHELPINFO, NULL,
				Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
			return 0;
		}
		break;
	case STK_PAC_SETUPMENU:
		if (IfHelp(STK_PAC_SETUPMENU))
		{
			int selID;

			selID = (int)SendMessage(hMenuList, LB_GETCURSEL, 0, 0L);
			ret = STK_Proat_Response(211/*STK_PAC_SETUPMENU*/, STK_REP_HELPREQUEST, "", 0, SetupMenuItem[selID].itemId);
		}
		else
		{
			PLXTipsWin(NULL, NULL, 0,
				IDS_STK_NOHELPINFO, NULL,
				Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
			return 0;
		}
	default:
		return 0;
	}

	return ret;
}

/*********************************************************************
* Function	IfHighPriority
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
static BOOL IfHighPriority(void)
{
	return (DisplayText.priority == prioHigh);
}

/*********************************************************************
* Function	IfMessageDelay
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
static BOOL IfMessageDelay(void)
{
	return (DisplayText.clear == modeAutoDelay);
}

/*********************************************************************
* Function	IfSustainedText
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
static BOOL IfSustainedText(void)
{
	return (DisplayText.response == modeImmediately);
}

/*********************************************************************
* Function	IfYesNoRequest
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
static BOOL IfYesNoRequest(void)
{
	return (GetInkey.yesNo == modeRequested);
}

/*********************************************************************
* Function	IfEcho
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
static BOOL IfEcho(void)
{
	return (GetInput.hidden == modeRevealed);
}

/*********************************************************************
* Function	IfHelp
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
static BOOL IfHelp(int CmdType)
{
	BOOL hasHelp;

	switch (CmdType)
	{
	case STK_PAC_GETINKEY:
		hasHelp = GetInkey.help;
		break;
	case STK_PAC_GETINPUT:
		hasHelp = GetInput.help;
		break;
	case STK_PAC_SELECTITEM:
		hasHelp = SelectItem.help;
		break;
	case STK_PAC_SETUPCALL:
		hasHelp = SetupMenu.help;
		break;
	default:
		hasHelp = modeNoHelp;
		break;
	}

	return (hasHelp == modeHelp);
}

/*********************************************************************
* Function	IfStkMainMenu
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL IfStkMainMenu(void)
{
	return (LastCmdType == STK_PAC_SETUPMENU);
}
