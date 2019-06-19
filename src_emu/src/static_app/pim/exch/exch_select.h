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
#include "exch_file.h"

#define ID_CANCEL		1
#define ID_EDIT			2

#define ID_CurrList		6

#define IDS_CANCEL		ML("Cancel")


static HWND hWndMain;
static HWND hFocus;
static HWND hWndList;
static HWND hWndBefore;
static char	cTitle[20] = "Select currency";
static HBITMAP hBmpRadioOn, hBmpRadioOff;
static int nDefaut;


static LRESULT SelectWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void CreateCurrList(HWND hWndParent);

extern EXCHMONEYINFONODE *pExchNodeHead;
extern EXCHMONEYINFONODE *pExchNodeEnd;
extern EXCHMONEYINFONODE *pExchNodeCur;
extern EXCHMONEYINFONODE *pExchNodeFirst;
extern EXCHMONEYINFONODE *pExchNodeSecond;
extern EXCHMONEYINFONODE *pExchNodeBase;
