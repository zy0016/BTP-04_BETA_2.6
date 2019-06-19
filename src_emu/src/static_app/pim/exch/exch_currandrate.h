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

#define ID_EDIT				1
#define ID_BACK				2
#define ID_MAINLIST			3
#define ID_NEW				4
#define ID_ASBASE			5
#define ID_DELETE			6
#define ID_DELEMANY			7
#define ID_DELE_SELECT		8
#define ID_DELE_ALL			9
#define CONFIRM_DELETEALL	WM_USER + 400
#define CONFIRM_DELETE		WM_USER + 401
#define CONFIRM_RECLCLT		WM_USER	+ 402
#define CONFIRM_RECLCLT_OK   WM_USER + 403

#define IDS_NEW			ML("New currency")
#define IDS_EDIT		ML("Edit")
#define IDS_BACK		ML("Back")
#define IDS_ASBASE		ML("Set as base")
#define IDS_DELEMANY	ML("Delete many")
#define IDS_DELE_SELECT	ML("Select")
#define IDS_DELE_ALL	ML("All")
#define IDS_DELETE		ML("Delete")

#define NEW			1
#define EDIT		2
#define DELETEONE	3
#define DELETEMANY	4
#define DELETEALL	5



#define ICON_CURRENCY			"/rom/pim/exch/currency.bmp"
#define ICON_NEWCURRENCY		"/rom/pim/exch/newcurrency.bmp"



HWND hWndCurrAndRate;
static HWND hWndList;
static HWND hFocus;
static HWND hWndBefore;
static HBITMAP hBmpCurrency, hBmpNewCurrency;


static LRESULT CurrAndRateWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void CreateCurrAndRateList(HWND hWndParent);
static void InitListBox(HWND hWnd);
static HMENU CreateMainWinMenu();
static int SetCurrIndexItem(HWND hWnd, int iMode);

extern DWORD Exch_EditRateWnd();
extern DWORD Exch_NewWnd();
extern DWORD Exch_Dele_SelectWin();

extern EXCHMONEYINFONODE *pExchNodeHead;
extern EXCHMONEYINFONODE *pExchNodeEnd;
extern EXCHMONEYINFONODE *pExchNodeCur;
extern EXCHMONEYINFONODE *pExchNodeFirst;
extern EXCHMONEYINFONODE *pExchNodeSecond;
extern EXCHMONEYINFONODE *pExchNodeBase;
