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

#include "exch_global.h"
#define ID_CANCEL				1
#define ID_SELECT				2
#define ID_CURR_RATE			3
#define ID_RateEdit1			4
#define ID_RateEdit2			5
#define ID_TIMER				6
#define ID_CALCULATE			7


#define IDS_CANCEL				ML("Cancel")
#define IDS_SELECT				ML("Select currency")
#define IDS_CURR_RATE			ML("Currencies && rates")
#define IDS_EDIT				ML("Edit")
#define IDS_BASE				ML("Set as base")
#define IDS_DELETE				ML("Delete")
#define IDS_DeleMany			ML("Delete many")
#define IDS_CALCULATE			ML("Calculate")


static HWND hFocus;
static HWND	hWndCurr1;
static HWND	hWndCurr2;
static int iTimer = 0;
static BOOL bNoEn_Change = FALSE;
static int iFocusItem = 1;

/*
typedef struct tagInputValue
{
	char cName[CURRENCY_MAX_LENGTH];
	char cValue[VALUE_MAX_LENGTH];
}InputValue;


InputValue Value1, Value2;*/
extern char cExch_Value1[VALUE_MAX_LENGTH +1], cExch_Value2[VALUE_MAX_LENGTH +1];


static LRESULT EditValueWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static HMENU CreateWinMenu();
static void CreateWinEdit(HWND hWndParent);
static void InitEditCtrl();
static void DoCompute(HWND hWnd);
static double GetValue(HWND hWnd);
static void DeleteZeroInPreString(char *p);

extern DWORD Exch_SelectWnd(HWND hWnd,int nCurrID);
extern DWORD Exch_CurrAndRateWnd(HWND hWnd);


extern EXCHMONEYINFONODE *pExchNodeHead;
extern EXCHMONEYINFONODE *pExchNodeEnd;
extern EXCHMONEYINFONODE *pExchNodeCur;
extern EXCHMONEYINFONODE *pExchNodeFirst;
extern EXCHMONEYINFONODE *pExchNodeSecond;
extern EXCHMONEYINFONODE *pExchNodeBase;
