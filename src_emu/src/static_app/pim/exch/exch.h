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
#include <mullang.h>

#define ID_EXIT			1
#define ID_EDIT			2
#define ID_LIST			3
#define ID_SELECT		4
#define ID_CURR_RATE	5

#define IDS_EXIT		ML("Exit")
#define IDS_EDIT		ML("Edit")
#define IDS_SELECT		ML("Select Currency")
#define IDS_CURR_RATE	ML("Currencies && rates")

	
static HINSTANCE hInstance = NULL;
static HWND hWndList;
static HWND hFocus;
static WNDPROC OldListWndProc;

static LRESULT Exch_MainWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT CallListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void CreateList(HWND hWndParent);
static void InitListBox(HWND hWnd);
static void ModifyListBox(HWND hWnd);
static HMENU CreateWinMenu();


extern int nCurrencyNum;
char cExch_Value1[VALUE_MAX_LENGTH +1] = "0";
char cExch_Value2[VALUE_MAX_LENGTH+1] = "0";
extern DWORD Exch_EditValueWnd(HWND hWndParend, int iCurItem);


EXCHMONEYINFONODE *pExchNodeHead = NULL;
EXCHMONEYINFONODE *pExchNodeEnd = NULL;
EXCHMONEYINFONODE *pExchNodeCur = NULL;
EXCHMONEYINFONODE *pExchNodeFirst = NULL;
EXCHMONEYINFONODE *pExchNodeSecond = NULL;
EXCHMONEYINFONODE *pExchNodeBase = NULL;
