/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : TestApp
 *
 * Purpose  : ME demo
 *            
\**************************************************************************/

// define window control
#define IDC_BUTTON_1          201
#define IDC_BUTTON_2          202
#define IDC_BUTTON_3	      203
#define IDC_BUTTON_4	      204
#define IDC_BUTTON_5	      205
#define IDC_EDIT_1		      206
#define IDC_EDIT_2	          207

#define IDC_TEST_QUIT         210

#define IDT_TIMER1            150
#define IDT_TIMER2            151

#define TESTLIST_X            20 
#define TESTLIST_Y            5
#define TESTLIST_W            80
#define TESTLIST_H            50

// define menu item
#define IDM_BEGINDBG        106
#define IDM_ENDDBG          107
#define IDM_SHOWDBG			108

#define IDM_RESOURCE				120
#define IDM_PDASYNCH				121
#define IDM_PMALARM					122
#define IDM_SETALARM				123
#define IDM_SETSLEEPINTERVAL		124
#define IDM_WATCHDOG				125

#define IDM_WINDOW0					130
#define IDM_WINDOW1					131
#define IDM_WINDOW2					132

//#define	TCPIP_DATA_RECORD	// 记录tcpip层的收发数据

static MENUITEMTEMPLATE Window1Menu[] = 
{
	{ MF_STRING, IDM_WINDOW0, "Back to window0", NULL },
	{ MF_STRING, IDM_WATCHDOG, "Disable WatchDog", NULL },
	{ MF_END,NULL,NULL,NULL },
};

static const MENUTEMPLATE TestMenuTemplate_ =
{
    0,
    Window1Menu
};

static MENUITEMTEMPLATE Window2Menu[] = 
{
	{ MF_STRING, IDM_WINDOW0, "Back to window0", NULL },
	{ MF_STRING, IDM_SETALARM, "Set Alarm", NULL},
	{ MF_STRING, IDM_SETSLEEPINTERVAL, "Set Sleep Interval", NULL},
	{ MF_END,NULL,NULL,NULL },
};

static const MENUTEMPLATE TestMenuTemplate__ =
{
    0,
    Window2Menu
};

static const MENUITEMTEMPLATE CheckResourceMenu[] = 
{
	{ MF_STRING, IDM_BEGINDBG, "start check", NULL },
	{ MF_STRING, IDM_SHOWDBG,  "show", NULL },
	{ MF_STRING, IDM_ENDDBG,   "Stop check", NULL },
	{ MF_END,NULL,NULL,NULL },
};

static MENUITEMTEMPLATE FileMenu[] = 
{
	{ MF_STRING|MF_POPUP, IDM_RESOURCE,  "check resource", (PMENUITEMTEMPLATE)CheckResourceMenu },
	{ MF_STRING, IDM_PDASYNCH, "PDA Synch", NULL},
	{ MF_STRING, IDM_PMALARM, "Display pmalarm", NULL},
	{ MF_STRING, IDM_WINDOW1, "Change to window1", NULL},
	{ MF_STRING, IDM_WINDOW2, "Change to window2", NULL},
	{ MF_END,NULL,NULL,NULL },
};

static const MENUTEMPLATE TestMenuTemplate =
{
    0,
    FileMenu
};


// Internal function prototypes
static BOOL TestCreate(HWND );
static void TestMessInfo(HWND hWnd, char *szMess);
static void TestCommand( HWND hWnd, WPARAM wParam);
static void TestShowFun(HWND hWnd, int iId);
static void HideAll(HWND hWnd);
static void ShowBtn(HWND hWnd, int iId);
static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                   LPARAM lParam);
extern void PdaSvr_Close(void);
extern BOOL PdaSvr_Open(int connect_type);	// 1:usb   2:bluetooth

#define TEST_MESS_1 "                    "


static HBITMAP  hBmp;
