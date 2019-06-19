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

#include "exch_global.h"
#include <mullang.h>

#define ID_CANCEL		1
#define ID_EDIT			2
#define ID_NameEdit		3
#define ID_RateEdit		4
#define ID_SAVE			5
#define ID_OK			6
#define ID_DELETE		7
#define ID_DELETELIST	8
#define CONFIRM_DELETE	9


#define IDS_CANCEL		ML("Cancel")
#define IDS_EDIT		ML("Edit")
#define STR_DEFINE		ML("Please define name")
#define STR_SAMENAME	ML("Name already in use")
#define IDS_SAVE		ML("Save")
#define IDS_DELETE		ML("Delete")


static HWND hWndMainRate;
static HWND hWndNameEdit;
static HWND hWndRateEdit;
static HWND hFocus;
static HWND hWndMainNew;
static HWND	hWndMainAsBase;
static HWND hWndDelList;
static HWND hWndDele_Select;
extern HWND hWndCurrAndRate;

static LRESULT EditRateWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT NewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT AsBaseWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void CreateEditCntrl(HWND hWndParent);
static BOOL SaveEditData();
static void StrToUpper(char *pSrc);
static BOOL SaveNewData();
static void CreateNewCntrl(HWND hWndParent);
DWORD Exch_EditRateWnd();
DWORD Exch_NewWnd();
DWORD Exch_Dele_SelectWin();

static LRESULT Dele_SelectWinProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static int CreateList(HWND hWndParent);

extern int Exch_Queue(HWND hWnd, char *pNameNext, int nBegin);
extern EXCHMONEYINFONODE *pExchNodeHead;
extern EXCHMONEYINFONODE *pExchNodeEnd;
extern EXCHMONEYINFONODE *pExchNodeCur;
extern EXCHMONEYINFONODE *pExchNodeFirst;
extern EXCHMONEYINFONODE *pExchNodeSecond;
extern EXCHMONEYINFONODE *pExchNodeBase;


double dRate_Max = 999999999999;
double dRate_Min = 0.0000000001;

double dValue_Max = 999999999999;
double dValue_Min = 0.0000000001;
