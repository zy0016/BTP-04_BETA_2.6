/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : StkHide.c
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
#include "assert.h"

#include "StkMain.h"
#include "StkRs.h"
#include "StkAT.h"

/////////////////////////////////////////////////////////////////////
static HWND hHideStkWnd;
static int hStkRegIndn;
static int LastCmdType;
static char TitleText[STK_MAC_MENUCOMMAND];
static STK_PARAM STK_SendStruct;
static SETUPMENU SetupMenu;
static SETUPIDLEMODETEXT SetupIdleModeText;

/////////////////////////////////////////////////////////////////////
extern BOOL bShowMenu, bActivate;
extern BOOL bInitOK;

/////////////////////////////////////////////////////////////////////
extern int  STK_Search_Integer(char *SrcStr, int *DestInt, BOOL First);
extern int  STK_Search_String(char *SrcStr, BYTE *DestStr);
extern int  STK_InvertOrder(char *Str, int Len);

/////////////////////////////////////////////////////////////////////
static LRESULT STK_HideWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static int  STK_Malloc(void);
static int  STK_Free(void);

static int  STK_GetInformation(int CmdType);
static int  STK_SendPack(int CmdType);

static int  STK_PacInfo(void);
static int  STK_SendAck(void);

static int  STK_SetupMenu(char ppContent[STK_MAC_MENUCOUNT][STK_MAC_MENUCOMMAND], PSETUPMENU pSetupMenu);
static int  STK_IdleModeText(char* pContent, PSETUPIDLEMODETEXT pSetupIdleModeText);

/*********************************************************************
* Function	IfStkInitSucc
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL IfStkInitSucc(void)
{
	return bInitOK;
}

/*********************************************************************
* Function	STK_GetMainText
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void STK_GetMainText(char* pMainText)
{
	static BOOL bGet = FALSE;

	if (!bActivate || SetupMenu.titleLen == 0)
	{
		strcpy(pMainText, IDS_STK_SIMTOOLKIT);
		return;
	}

	// Return immediately
	if (bGet)
	{
		strcpy(pMainText, TitleText);
		return;
	}

	// Alpha identifier provided by SIM
	if (SetupMenu.titleLen > 0)
	{
		int len;

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)SetupMenu.titleText, SetupMenu.titleLen/2,
			(LPSTR)TitleText, SetupMenu.titleLen,
			0, 0);
		*(TitleText+len) = 0;

		bGet = TRUE;
		strcpy(pMainText, TitleText);
	}
}

/*********************************************************************
* Function	STK_GetIdleModeText
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void STK_GetIdleModeText(char* pIdleModeText)
{
	if (!bActivate || SetupIdleModeText.textLen == 0)
	{
		strcpy(pIdleModeText, "");
		return;
	}

	// Alpha identifier provided by SIM
	if (SetupIdleModeText.textLen > 0)
	{
		int len;
		char DestStr[50];

		len = WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)SetupIdleModeText.text, SetupIdleModeText.textLen/2,
			(LPSTR)DestStr, SetupIdleModeText.textLen,
			0, 0);
		*(DestStr+len) = 0;

		strcpy(pIdleModeText, DestStr);
	}
}

/*********************************************************************
* Function	CreateStkHideWindow
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL CreateStkHideWindow(void)
{
	WNDCLASS wc;

	wc.style         = 0;
	wc.lpfnWndProc   = STK_HideWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "StkHideWndClass";
	if (!RegisterClass(&wc))
		return FALSE;

	hHideStkWnd = CreateWindow("StkHideWndClass",
		"Remote-SAT",
		0,
		0,0,0,0,
		NULL, NULL, NULL, NULL);
	if (hHideStkWnd == NULL)
		return FALSE;

	// Allocate 20*100 bytes for ME received buffer
	STK_SendStruct.revbuf = (void *)malloc(STK_MAC_MENUCOUNT*STK_MAC_MENUCOMMAND);
	STK_SendStruct.line_count = STK_MAC_MENUCOUNT;
	STK_SendStruct.line_len = STK_MAC_MENUCOMMAND;

	// Register STK Indication to Monitor
	hStkRegIndn = ME_RegisterIndication(ME_URC_SSTN, IND_PRIORITY, hHideStkWnd, WM_STK_INDICATION);
	if (hStkRegIndn == -1)
	{
		// Deallocate!
		free(STK_SendStruct.revbuf);

		UnregisterClass("StkWndClass", NULL);
		hHideStkWnd = NULL;

		return FALSE;
	}

	// Request for Main Menu
	STK_Malloc();
	LastCmdType = STK_PAC_SETUPMENU;
	STK_GetInformation(STK_PAC_SETUPMENU);

	return TRUE;
}

/*********************************************************************
* Function	STK_HideWndProc
* Purpose   
* Parameter	
* Return	LRESULT
* Remarks	
**********************************************************************/
static LRESULT STK_HideWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;

	lResult = (LRESULT)TRUE;

	switch (uMsg)
	{
	case WM_CREATE:
		bActivate = FALSE;
		bInitOK = FALSE;
		break;

	case WM_DESTROY:
		/*
		一直打开,无需关闭释放资源
		*/
		STK_Free();

		// Unregister!
		ME_UnRegisterIndication(hStkRegIndn);

		// Deallocate!
		free(STK_SendStruct.revbuf);

		UnregisterClass("StkHideWndClass", NULL);
		hHideStkWnd = NULL;
		break;

	case WM_PAINT:
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_STK_INDICATION:
		{
			int ret;

			if (ME_GetResult(&ret, sizeof(int)) == -1)
				break;

			// AT^SSTGI=
			STK_SendPack(ret);
		}
		break;

	case WM_STK_PACINFO_RECV:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			// Unpacking parameters
			STK_PacInfo();

			// Setup menu response
			STK_SendAck();
			break;
		case ME_RS_FAILURE:
			break;
		default:
			break;
		}
		break;

	case WM_STK_RESP_RECV:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			break;
		case ME_RS_FAILURE:
			break;
		default:
			break;
		}
		break;

	case WM_STK_RESP_ACK:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			bInitOK = TRUE;
			break;
		case ME_RS_FAILURE:
			break;
		default:
			break;
		}
		break;

	default:
		lResult = PDADefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return lResult;
}

/*********************************************************************
* Function	STK_Malloc
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_Malloc(void)
{
	SetupMenu.titleText = (char *)malloc(STK_MAC_MENUCOMMAND);
	SetupIdleModeText.text = (char *)malloc(STK_MAC_STRINGCOMMAND);

	return 0;
}

/*********************************************************************
* Function	STK_Free
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_Free(void)
{
	free(SetupMenu.titleText);
	free(SetupIdleModeText.text);

	return 0;
}

/*********************************************************************
* Function	STK_GetInformation
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_GetInformation(int CmdType)
{
	char atCmd[20];

	sprintf(atCmd, "AT^SSTGI=%d\r", CmdType);
	strcpy(STK_SendStruct.cmd, atCmd);
	STK_SendStruct.timeout = 10000;
	if (-1 == ME_STK_Send(hHideStkWnd, WM_STK_PACINFO_RECV, &STK_SendStruct))
		return -1;

	return 0;
}

/*********************************************************************
* Function	STK_SendPack
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_SendPack(int CmdType)
{
	if (CmdType != STK_PAC_SETUPIDLEMODETEXT)
		return 0;

	// Request...
	LastCmdType = CmdType;
	return STK_GetInformation(CmdType);
}

/*********************************************************************
* Function	STK_PacInfo
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_PacInfo(void)
{
	char RecvBuf1[STK_MAC_STRINGCOMMAND];
	char RecvBuf2[STK_MAC_MENUCOUNT][STK_MAC_MENUCOMMAND];

	switch (LastCmdType)
	{
	case STK_PAC_SETUPMENU:
		// ME received buffer length
		memcpy(RecvBuf2, STK_SendStruct.revbuf, STK_MAC_MENUCOUNT*STK_MAC_MENUCOMMAND);
		STK_SetupMenu(RecvBuf2, &SetupMenu);
		break;

	case STK_PAC_SETUPIDLEMODETEXT:
		// ME received buffer length
		memcpy(RecvBuf1, STK_SendStruct.revbuf, STK_MAC_STRINGCOMMAND);
		STK_IdleModeText(RecvBuf1, &SetupIdleModeText);
		DlmNotify(PS_IDLEMODETXT, NULL);
		break;

	default:
		break;
	}

	return 0;
}

/*********************************************************************
* Function	STK_SendAck
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int STK_SendAck(void)
{
	char SendStr[32];

	switch (LastCmdType)
	{
	case STK_PAC_SETUPMENU:
		if (bActivate)
		{
			// 不需显示菜单了
			bShowMenu = TRUE;
			return 0;
		}

		bActivate = TRUE;
		sprintf(SendStr, "AT^SSTR=%d,0\r", LastCmdType);
		break;

	default:
		return 0;
	}

	strcpy(STK_SendStruct.cmd, SendStr);
	STK_SendStruct.timeout = 10000;
	if (-1 == ME_STK_Send(hHideStkWnd, WM_STK_RESP_ACK, &STK_SendStruct))
		return -1;

	return 0;
}

/*********************************************************************
* Function	STK_SetupMenu
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails>, <numOfItems>, <titleText>,
// <menuItemIconsPresent>, <menuItemIconsQualifier>, <titleIconQualifier>,
// <titleIconId> <CR> <LF>
// ^SSTGI: <cmdType>, <itemId>, <itemText>, <nextActionId>, <iconId> <CR>
// <LF>
static int STK_SetupMenu(char ppContent[STK_MAC_MENUCOUNT][STK_MAC_MENUCOMMAND], PSETUPMENU pSetupMenu)
{
	int cmdType, commandDetails, numOfItems, offset;
	BYTE titleText[STK_LEN_STRING+1];
	int titleLen;
	char* pBuffer;

	pBuffer = ppContent[0];
	offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &numOfItems, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, titleText);
	titleLen = (offset - 3) / 2;
	// 解决无匹配双引号titleLen为-1
	if (titleLen < 0)
		titleLen = 0;

	pSetupMenu->preference = commandDetails & 0x01;
	pSetupMenu->help = (commandDetails & 0x80) >> 7;
	pSetupMenu->numOfItems = numOfItems;
	memcpy(pSetupMenu->titleText, titleText, titleLen);
	pSetupMenu->titleLen = titleLen;
	STK_InvertOrder(pSetupMenu->titleText, pSetupMenu->titleLen);

	return 0;
}

/*********************************************************************
* Function	STK_IdleModeText
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails>, <text>, <iconQualifier>, <iconId>
// <CR> <LF>
static int STK_IdleModeText(char* pContent, PSETUPIDLEMODETEXT pSetupIdleModeText)
{
	int cmdType, commandDetails, offset;
	BYTE text[STK_LEN_STRING+1];
	int textLen;
	char* pBuffer;

	pBuffer = pContent;
	offset = STK_Search_Integer(pContent, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, text);
	textLen = (offset - 3) / 2;
	// 解决无匹配双引号textLen为-1
	if (textLen < 0)
		textLen = 0;

	memcpy(pSetupIdleModeText->text, text, textLen);
	pSetupIdleModeText->textLen = textLen;
	STK_InvertOrder(pSetupIdleModeText->text, pSetupIdleModeText->textLen);

	return 0;
}
