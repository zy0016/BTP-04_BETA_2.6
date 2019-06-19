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
#include "window.h"
#include "winpda.h"
#include "string.h"
#include "malloc.h"
#include "plx_pdaex.h"
#include "stdio.h"
#include "stdlib.h"
#include "DiamonRs.h"
#include "GameHelp.h"
#include "hpimage.h"
#include "pubapp.h"

/**********************************************
*
*	常量定义
*
***********************************************/
#define	MENU_ITEM_NUM		2

#define	GAME_TIMER_ID		1
#define	GAME_TIMER_ELAPSE	500

//#define PATH_MENU				"/rom/game/diamond/diam_menue.gif"

#define	COVERBG					"/rom/game/diamond/diamond.bmp"
#define	DIAMONDS_PATH			"/rom/game/diamond/diam_dims.gif"
#define	DIAMOND_BACK_PATH		"/rom/game/diamond/diam_back.gif"
#define	DIAMOND_DIGITAL_PATH	"/rom/game/diamond/diam_dgtl.gif"
#define	LITTLEBALL_PATH			"/rom/game/diamond/diam_cue.gif"


#define	GET_X_DIAMONDS(i)	(i*DIAMOND_WIDTH)
#define CreateCapButton(hwnd, lowparam, hiwparam, lparam) \
	SendMessage(hwnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(lowparam,hiwparam), (LPARAM)lparam)
#define	DIAMOND_CFG_FILE	"/mnt/flash/game/Diamond.dat"

/**********************************************
*
*	数据结构
*
***********************************************/

/**********************************************
*
*	函数说明
*
***********************************************/
static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK AppMainWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
static void DrawColorBorder(HDC hdc, RECT *rc, int Color);
static int Bonus(int x);
static void Delaying(int n);
static void  InitDiamondBMP();
static void  DeleteDiamondBMP();
static int NewDiamond();
static void RedrawDiamond(int i, int j);
static void DiamondDrop(HDC hdc);
static void DrawNumber(HDC hdc, int x, int y, int Number);
static void RefreshScence(HDC hdc);
static void DrawDiamond(HDC hdc);
static void Merge(int i, int end, int sum);
static void CheckBoard();
static int CheckNeighborLeft(int i,int j, int seed);
static int CheckNeighborRight(int i,int j, int seed);
static int CheckNeighborUp(int i,int j, int seed);
static int CheckNeighborDown(int i,int j, int seed);
static int CheckNeighborUpDown(int i,int j, int seed);
static int CheckNeighborLeftRight(int i,int j, int seed);
static int CheckWay();
static void InitDiamond();
static void SwapDiamond();
static void SwapAndCalc();
static void DiamondGameOver(HDC hdc);
static void SelectOneDiamond(int ni, int nj);
static void DiamondSaveData();
static int DiamondLoadData();
static void DiamondDraw3DBorder(HDC hdc, RECT *rc, int In);
static BOOL	DiamondGameProc(HWND hWnd);
static	BOOL	ShowArrowState(HWND hWnd,int nSel);
static	int	GetAreaByPos(int x,int y);
static BOOL MyDrawMenuItem(HDC hdc,int i,int nSel);
static DWORD	OnGameHelp(HWND hWnd);
static BOOL	CfgFileExist();
static BOOL	DrawStaticBarArrowState(HWND hWnd);
static BOOL	SetMenuItemRectLocal();

/**********************************************
*
*	全局变量
*
***********************************************/
static HWND hWnd_Main;	// Save the main window  of the application
static HWND hwndApp;    // Save the game window  of the application
static HWND hWndHelp;
static BOOL bShow;      // Whether the main window is showing
static HINSTANCE hInstance;
static HINSTANCE hDiamondInstance;
//static HBITMAP  DiamondBmp[7], hNumBmp;
static HBITMAP  DiamondBmp, hNumBmp;
static HBITMAP	hGameBack;
static HBITMAP	hLittleBall;

static HBITMAP	hBmpMenu = NULL;

static int		Diamond[8][8] ;
static int		nMergeDiamondNum, LevelMergeDiamondNum,TotalMergeDiamondNum, bBegin;
static int		iFocus, jFocus; //keyboard focus 
static RECT	rcBar;	//process bar area
static DWORD	GameLevel=1; 
static int		iTip, jTip;
static int bGameOver;
#if defined _EMULATE_
static int SPEED=10;
#else
static int SPEED=2;
#endif
static int nHit;
//the first hit Diamond and the second hit one
static int ai,aj,bi,bj;
static char MenuItemText[MENU_ITEM_NUM][9];
static RECT	MenuItemRect[MENU_ITEM_NUM];			//菜单项矩形
static RECT	SelItemRect[MENU_ITEM_NUM];				//图标矩形
static int		nSelMenuItem;						//选定菜单项
static HBITMAP	hCoverBg;							//主界面背景
static BOOL bGamePause;
static BOOL	bLoadData;
static BOOL	bSaveData;
static BOOL bLeave = FALSE;	
static const RECT INFORECT = { 33, 150, 140, 177 };	
static UINT iTimerCouter;
static BOOL bBallFlash;

/**********************************************
*
*	菜单控件ID
*
***********************************************/
#define ID_FILEMGR_QUIT         WM_USER+201
#define ID_FILEMGR_OK			WM_USER+202
#define	IDM_EXIT				WM_USER+1001
#define	IDM_CONFIRM				1002+WM_USER
#define	IDM_NEWGAME				1003+WM_USER
#define	IDM_HELP				1004+WM_USER

/**********************************************
*
*	控件ID
*
***********************************************/

/**********************************************
*
*	自定义消息
*
***********************************************/


/**********************************************
*
*	菜单模板
*
***********************************************/
