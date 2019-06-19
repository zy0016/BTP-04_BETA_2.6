/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2005 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "winpda.h"
#include "hpimage.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
//#include "fapi.h"
#include "plx_pdaex.h"
#include <stdlib.h>
#include <stdio.h>
//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//to invoke PLXConfirmWin
#include <pubapp.h>

//#ifdef SCP_SMARTPHONE
//#include "msgwin.h"
//#endif

#include "malloc.h"
//#include "hpdebug.h"
#include "str_plx.h"
#include "PushBoxrs.h"
#include "GameHelp.h"
#include "mullang.h"
/**********************************************
*
*	常量定义
*
***********************************************/
#define MENU_ITEM_NUM		4
#define ID_LEVEL_COMBO		501
#define LEVEL_COUNT			10
#define INTERVAL			150

/*
#define	PushBoxCover		"ROM:/game/pushbox/pb_cover.gif"
#define	PushBoxbg			"ROM:/game/pushbox/pb_bg.gif"
#define	PushBoxArea			"ROM:/game/pushbox/pb_area.gif"
#define	PushBoxBox			"ROM:/game/pushbox/pb_box.gif"
#define	PushBoxWall			"ROM:/game/pushbox/pb_wall.gif"
#define	PushBoxMan1			"ROM:/game/pushbox/pb_man1.gif"
#define	PushBoxTemini		"ROM:/game/pushbox/pb_temi.gif"
#define	PushBoxCoverMan		"ROM:/game/pushbox/pb_coman.gif"

#define PushBoxMap          "ROM:/game/pushbox/mymap.txt"
#define PushBoxLevel        "FLASH2:/game/pushbox/boxlevel.dat"*/

#define	PushBoxCover		"/rom/game/pushbox/push_box.bmp"
#define	PushBoxbg			"/rom/game/pushbox/pb_bg.gif"
#define	PushBoxArea			"/rom/game/pushbox/pb_area.gif"
#define	PushBoxBox			"/rom/game/pushbox/pb_box.gif"
#define	PushBoxWall			"/rom/game/pushbox/pb_wall.gif"
#define	PushBoxMan1			"/rom/game/pushbox/pb_man1.gif"
#define	PushBoxTemini		"/rom/game/pushbox/pb_temi.gif"
#define	PushBoxCoverMan		"/rom/game/pushbox/pb_coman.gif"

#define PushBoxMap          "/rom/game/pushbox/mymap.txt"
#define PushBoxLevel        "/mnt/flash/game/boxlevel.ini"


//左上角图标
//#include "hp_icon.h"
#define	DIRICON	            "/rom/game/pushbox/pushbox.ico"


/**********************************************
*
*	数据结构
*
***********************************************/
//(游戏的全部控制过程,实现所有的判断条件)
typedef struct tagGAME_STATE//保存游戏状态的结构
{
	BOOL  IsBoxOnMan;           //小人前是否有箱子
	BOOL  IsWallOnMan;  	    //小人前是否有墙
	BOOL  IsTerminiOnMan;	    //小人前是否为目的地
	BOOL  IsAreaOnMan;			//小人前是否为空地
	BOOL  IsWallOnBox;    		//箱子前是否有墙
	BOOL  IsTerminiOnBox;	    //箱子前是否为目的地
	BOOL  IsBoxOnBox;			//箱子前是否有箱子
	BOOL  IsAreaOnBox;			//箱子前是否为空地
	BOOL  IsPass;		        //是否过关
	POINT Man;					//记录小人当前的逻辑坐标
	char  pCurManState;			//记录小人当前位置移动前的状态。
}GAME_STATE;


//(存放后退链表里的所有信息)
typedef struct tagListUnit
{
	POINT ManPoint;				//推动箱子前的人位置
	POINT TheLastPoint;			//推动前的箱子位置
	POINT AndTheLastPoint;		//推动后的箱子位置
	char TheFirstPointIsWhat;	//推动箱子前人的位置是什么
	char TheSecondPointIsWhat;	//推动前的箱子在什么上面（判断是空地还是目的地）
	char TheThirdPointIsWhat;	//推动后的箱子在什么上面（判断是空地还是目的地）
	char TheStateManAfter;		//推动前的箱子位置推动以后状态
	struct tagListUnit *pNext;	//指向该结构的指针

}ListUnit;


/**********************************************
*
*	函数说明
*
***********************************************/
static	DWORD	PushBoxOptionSetupProc(HWND ParenthWnd);
static	DWORD	PushBoxGameProc(HWND ParenthWnd);
static	DWORD	PushBoxSuccessProc(HWND ParenthWnd);
static	DWORD	PushBoxEndGameProc(HWND ParenthWnd);
static	DWORD	PushBoxGameHelpProc(HWND hWnd);

LRESULT	PushBoxMainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT	PushBoxOptionSetupWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT	PushBoxGameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT	PushBoxSuccessWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT	PushBoxEndGameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static	BOOL	ShowArrowState(HWND hWnd,int nSel);
static	int		GetAreaByPos(int x,int y);
//static BOOL MyDrawMenuItem(HDC hdc,int i,int nSel);

static	POINT	MoveBox(char Direction,int j,int i);
static	POINT	MoveMan(char Direction,int j,int i);
//static	int		PushBoxSelectKeyDown(WPARAM wParam);
//static	void	PushBoxJobKeyDown(HDC hdc,WPARAM wParam);
static	char	PushBoxDirectionKeyDown(WPARAM wParam);
static	POINT	FindMan(void);
static	POINT	GetNextPoint(POINT prePoint,char Direction);
static	char	GetStateByPoint(POINT curPoint);
static	char	TestPosition(POINT curPoint, char IsWhat,char Direction);
static	void	SettingSign(char Direction);
static	BOOL	IsGameOver(void);
static	char	GetBoxNum(void);
static	void	DrawMoveMan(HDC hdc,char Direction);
static	void	DrawMoveBoxAndMan(HDC hdc,char Direction);
static	void	PushBoxControlMoveK(HWND hWnd, HDC hdc,WPARAM wParam);
static	int		PushBoxControlMoveP(HWND hWnd, LPARAM lParam);
static	void	ShowGameWindowAnew(HWND hWnd);
static	void	ComeBack(HDC hdc, ListUnit *q);
static	void	GetFormerPointK(char Direction);
static	void	GetFormerPlaceIsWhatK(void);
static	void	GetFormerPointP(POINT curPoint, POINT prePoint, POINT LastPoint);
static	void	GetFormerPlaceIsWhatP(void);
static	POINT	FindMyPenInMap(void);
static	short	FindNextAdj(int IsNew, POINT Pt, POINT * NextPt);
static	short	SearchPath(POINT ResPt, POINT DesPt);
static	void	DrawAutoMoveMan(HDC hdc, short j, short i);
static	void	ShowArrowHead(HWND hWnd, int Disappear);
static	void	IsWhatAroundMan(HWND hWnd);

static	void	InsertDataToList( ListUnit **ppList,ListUnit *pList);
static	int		DeleteTail(ListUnit **ppList);
static	int		DeleteHead(ListUnit **ppList);
static	int		FreeList(ListUnit **ppList);

//void PAINTBK_COLOR(HDC hdc,COLORREF rgb, HWND hWnd);
static	void	DrawBackGround(HDC hdc);
static	void	DrawArea(HDC hdc,int j,int i);
static	void	DrawWall(HDC hdc,int j,int i);
static	void	DrawMan(HDC hdc,int j,int i);
static	void	DrawTermini(HDC hdc,int j,int i);
static	void	DrawBox(HDC hdc,int j,int i);

//static	void	InitMap(void);
static	void	ShowMap(HDC hdc);
static	void	ShowMapBlock(HDC hdc,int j,int i);
static	void	CenterShow(HWND hWnd, HDC hdc);

static	void	WriteLevelToFile(void);
static	void	ReadLevelFromFile(void);
static	void	MyIntToStr (char *pStr, int num);
static	BOOL	ReadMapFromFile();
static	void	ReadBoxMap(int nLevel);

/**********************************************
*
*	全局变量
*
***********************************************/
static HINSTANCE hInst;						//应用进程
static HWND hPushBoxWnd;					//游戏主窗口句柄
static HWND hPushBoxGameWnd;				//游戏窗口句柄
static HWND hPushBoxOptionSetupWnd;			//设置窗口句柄
static HWND hLevelCombo = NULL;				//设置窗口里的控件句柄
static HWND hPushBoxSuccessWnd;				//过关窗口句柄
static HWND hPushBoxEndGameWnd;				//关底窗口句柄
static HWND	hWndHelp;						//帮助窗口句柄
//static HMENU hMenu;							//游戏界面下的菜单

static BOOL LevelSetupCreate = FALSE;		//是否生成设置窗口
static BOOL GameCreate = FALSE;				//是否生成游戏窗口
static BOOL SuccessCreate = FALSE;			//是否生成过关窗口
static BOOL EndGameCreate = FALSE;			//是否生成关底窗口
static BOOL ReadSuccess = FALSE;			//地图文件是否读取成功
static BOOL	bTimerAlready = FALSE;			//是否已经设置了Timer
static GAME_STATE gamestate;				//游戏过程控制的数据结构
//static ListUnit *ppList = NULL;				//链表头节点

static int curnLevel = 1;					//当前关数初始化为1
static char GameMap[12][12];					//存储地图的二位数组
static char* pChar = NULL;					//存放地图的内存指针
static char row,line;						//地图的行列数
//static char TerminiCount;					//目的地的个数
static int BackCount = 0;					//后退的步数
static POINT MyPen;							//笔的坐标
static short ShortPath[9*12];				//记录从起点到终点的最短路径（如果存在可到达的路径）
static short Distance;						//记录最短路径的节点个数
static GAME_STATE gamestate;				//游戏过程控制的数据结构
static ListUnit *ppList = NULL;				//链表头节点
static ListUnit pList;						//刻画推箱子操作的节点结构信息
static int nSelMenuItem = 0;				//选中菜单项0--2

static HBITMAP hPushBoxCover = NULL;		//图片：封面
static HBITMAP hPushBoxbg = NULL;			//背景图
static HBITMAP hPushBoxArea = NULL;			//游戏界面中的空地
static HBITMAP hPushBoxBox = NULL;			//游戏界面中的箱子
static HBITMAP hPushBoxWall = NULL;			//游戏界面中的墙
static HBITMAP hPushBoxMan1 = NULL;			//游戏界面中的小人
//static HBITMAP hPushBoxMan2 = NULL;			//游戏界面中的小人
static HBITMAP hPushBoxTemini = NULL;		//游戏界面中的目的地
//static HBITMAP hPushBoxCoverWords = NULL;	//封面中的文字
//static HBITMAP hPushBoxCoverMan = NULL;		//封面中的小人
static HBITMAP hPushBoxPassWords = NULL;	//过关界面中的文字
static HBITMAP hPushBoxEndGameWords = NULL;	//关底界面中的文字

static	HMENU	hPushBoxMenu = NULL;
static COLORREF clrCoverMan;                 
static COLORREF clrCover;                   //背景图
static COLORREF clr1;						//透明色用于小人
static COLORREF clr2;						//用于过关文字
static COLORREF clr3;						//用于关底文字
static COLORREF clr4;						//用于封面文字
static BOOL	bclarity;						//是否设置透明色

static BOOL	bRSetupCLA = FALSE;
static BOOL	bRGameCLA = FALSE;
static BOOL	bRSuccessCLA = FALSE;
static BOOL	bREndgameCLA = FALSE;

//static int	MoveNum = NULL;					//每一局游戏中小人的移动步数
static char LevelString[LEVEL_COUNT][10]=
{
	"第一局",
	"第二局",
	"第三局",
	"第四局",
	"第五局",
	"第六局",
	"第七局",
	"第八局",
	"第九局",
	"第十局",/*
	"第十一局",
	"第十二局",
	"第十三局",
	"第十四局",
	"第十五局",
	"第十六局",
	"第十七局",
	"第十八局",
	"第十九局",
	"第二十局",
	*/
};
/**********************************************
*
*	菜单控件ID
*
***********************************************/
#define IDM_NEWGAME			WM_USER+101
#define IDM_SELECT			WM_USER+102
#define IDM_GOONGAME		WM_USER+103
#define IDM_EXIT			WM_USER+104
#define IDM_RETURN			WM_USER+105
#define IDM_START			WM_USER+106
#define	IDM_CONFIRM			WM_USER+107
#define	IDM_CLEAR			WM_USER+108
#define	IDM_HELP			WM_USER+109
#define IDM_REPLAY          WM_USER+110
#define IDM_CANCEL          WM_USER+111

#define	IDRM_RMSG			WM_USER+112
#define	IDRM_SELNEXT		WM_USER+113
#define	IDRM_GAMEOVER		WM_USER+114

/**********************************************
*
*	控件ID
*
***********************************************/
#define	TIMER_IDM			1


//#define ID_MENU_HERO		(WM_USER+5)


/**********************************************
*
*	菜单模板
*
***********************************************/
/*static MENUITEMTEMPLATE PushBox_MainMenu[] = 
{
    { MF_STRING, ID_MENU_SELECT, IDS_MENU_SELECT, NULL },
	{ MF_STRING, ID_MENU_GOONGAME, IDS_MENU_START, NULL },
	{ MF_END, 0, NULL, NULL }
};

static const MENUTEMPLATE PushBoxMainMenuTemplate =
{
    0,
    PushBox_MainMenu
};

static MENUITEMTEMPLATE PushBox_GameMenu[] = 
{
	{ MF_STRING, ID_MENU_BACK, NULL, NULL},
	{ MF_STRING, ID_MENU_REPLAY, NULL, NULL},
	{ MF_END, 0, NULL, NULL }
};

static const MENUTEMPLATE PushBoxGameMenuTemplate =
{
    0,
    PushBox_GameMenu
};
*/
