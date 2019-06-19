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

#include "math.h"
#include "winpda.h"
#include "window.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
//#include "fapi.h"
#include "plx_pdaex.h"
#include "hpimage.h"
#ifdef SCP_SMARTPHONE
#include "msgwin.h"
#endif
#include "BombRs.h"
#include "GameHelp.h"
#include "mullang.h"
//header file on linux file system 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pubapp.h>


#define		MsgWin				PLXTipsWin
/**********************************************
*
*	常量定义
*
***********************************************/

#define CreateCapButton(hwnd, lowparam, hiwparam, lparam) \
	SendMessage(hwnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(lowparam,hiwparam), (LPARAM)lparam)
//左上角图标
#ifdef SCP_SMARTPHONE
#include "hp_icon.h"
#define	DIRICON		GAME_DBOMB_ICON
#endif

#define	COVERBG				"/rom/game/depthbomb/depth_bomb.bmp"
#define IDS_MENUFILENAME	"/rom/game/depthbomb/bomb_em.gif"
#define	BOMB_COVER_SELECT	"/rom/game/depthbomb/bomb_sel.gif"
#define	BOMBGAMEBACK		"/rom/game/depthbomb/bomb_back.gif"
#define	BOMBGTEXT			"/rom/game/depthbomb/bomb_txen.gif"
#define	DEPTHBOMBSIGN		"/rom/game/depthbomb/bomb_sign.gif"
#define	GUNSHIP_PATH		"/rom/game/depthbomb/bomb_gshp.gif"
#define	MINSHIP_PATH		"/rom/game/depthbomb/bomb_mshp.gif"
#define	DEPTHSUB_PATH		"/rom/game/depthbomb/bomb_dsub.gif"
#define	DEPTHSUBSHOT_PATH	"/rom/game/depthbomb/bomb_sbsh.gif"
#define	DEPTHSUBPRIZE_PATH	"/rom/game/depthbomb/bomb_sbpr.gif"
#define	DEPTHBOMBPLANE		"/rom/game/depthbomb/bomb_sbpl.gif"
#define	PATH_GAMEBG0		"/rom/game/depthbomb/gam_bg.gif"
#define	PATH_GAMEOVER		"/rom/game/depthbomb/gam_over.gif"


#define	BOMB_SETUP_FILE		"/mnt/flash/game/BombData.ini"
#define	BOMB_HERO_FILE		"/mnt/flash/game/BombHero.ini"

#define HERONAMEFORMAT		"%-8s %5d"	//"%d. %-8s%5d"	//英雄名字输出格式

#define	DIR_LEFT				0			//向左
#define	DIR_RIGHT				1			//向右
#define	DIR_NONE				2			//停止不动
#define	MENU_ITEM_NUM			4			//菜单项个数
#define	BASE_GAME_TIME			30			//基础游戏时间
#define	GAME_TIME_STEP			30			//游戏时间增加的步长
#define	GAME_TIME(i)			(BASE_GAME_TIME+i*GAME_TIME_STEP)		//游戏的时间长度
#define	GAME_TIMER_ID			1			//一局游戏时间长度的ID
#define	MAX_WAVE_FRAME			5			//浪移动针数最大值，0到4
#define	WAVE_MOVE_TIMER_ID		2			//浪移动TIMER的ID
#define	WAVE_MOVE_TIMER_ELAPSE	200			//浪移动的时间间隔
#define	SHIP_MOVE_TIMER_ID		3			//移动TIMER的ID
//#define	UPSHIP_MOVE_TIMER_ID	8			//军舰移动TIMER的ID
#define	SHIP_MOVE_ELAPSE		250			//炮船移动的时间间隔
//#define	UPSHIP_MOVE_ELAPSE					//炮船移动的时间间隔

#define	SUBNUM_PERPRIZE			10			//每多少潜艇有一个奖励
#define	SUB_MOVE_STEP			3			//潜艇每步移动值
#define	BOMB_DROP_STEP			3			//炸弹每步下落值
#define	SCORE_PER_SUB			10			//每炸一个潜艇的得分
#define	PRIZE_ASCEND_INTERVAL	2			//奖励上升的间隔
#define	TIMES_PER_TORPETO		18			//每发射一颗鱼雷的频率
#define	TORPEDO_MOVE_STEP		3			//鱼雷移动的步长
#define	GUNSHIP_START_LIVE		5			//炮船的初始命数
#define	NO_ENEMY_TIME			15			//无敌的时间（秒数）
#define	SHIP_MIN_TIME			15			//船变小时间（秒数）
#define	SCORE_PER_PLANE			1000		//每得多少分来一架飞机

/**********************************************
*
*	数据结构
*
***********************************************/
#define	BOMB_HERO_NUM			5			//英雄个数
typedef struct	tagBombHero
{
	char	Name[15];
	int		nScore;
}BOMBHERO;

#define	MAX_SUBNUM				8			//最大潜艇数
typedef	struct	tagSubmarine
{
#define		STATE_ALIVE		0
#define		STATE_DIED		1
#define		STATE_DYING		2
	int		state;			//潜艇状态
#define		DIEFRAMENUM		3
//向右潜艇（死亡）
#define	X_SUBTOR_DIEFRAME(i)	(SUBMARINE_WIDTH*(2+DIEFRAMENUM-i-1))
//向左潜艇（死亡）
#define	X_SUBTOL_DIEFRAME(i)	(SUBMARINE_WIDTH*(2+DIEFRAMENUM*2-i-1))
	int		DieFrame;		//死去的针
#define		SUB_TIERNUM		4
#define		Y_FIRSTSUB		82//100
#define		SUB_Y_INTERVAL	20
#define		SUB_Y(i)		(Y_FIRSTSUB+i*SUB_Y_INTERVAL)
	int		y_Sub;			//纵坐标
	int		x_Sub;			//横坐标
	int		nDirect;		//向左还是向右
#define		SHOT_NORMAL		0
#define		SHOT_MISSIL		1
	int		SubShotStyle;	//潜艇的炮弹类型
#define		NO_PRIZE		0
#define		PRIZE_MIN		1
#define		PRIZE_KILLALL	2
#define		PRIZE_NOENEMY	3
#define		PRIZE_NUM		3//奖励的种数
	int		Prize;			//潜艇被炸后的漂浮物
	struct	tagSubmarine	*pNext;
}SUBMARINE,*PSUBMARINE;

#define		SUBPRIZE_NUM	3			//最大奖励数
#define		X_SUBPRIZE(i)	(BOMB_SHOT_WIDTH+NOENEMY_WIDTH+(i-1)*PRIZE_WIDTH)
typedef	struct	tagSubPrize
{
	int PrizeStyle;			//奖励的类型
#define	PRIZESTATE_ALIVE	0
#define	PRIZESTATE_DIED		1
	int	PrizeState;			//奖励的状态
	int x_Prize;
	int y_Prize;
}SUBPRIZE,*PSUBPRIZE;

#define		DEPTHBOMB_NUM	5			//最大炸弹数
#define		BASIC_BOMB_NUM	3			//最小炸弹数
#define		PLANE_BOMB_NUM	10			//飞机投掷的炸弹数
typedef	struct	tagDepthBomb
{
#define		STATE_AVAILABLE		0		//可用
#define		STATE_SHOTTED		1		//已经投掷
#define		STATE_INAVAILABLE	2		//不可用
	int		state;						//炸弹状态
	int		x_Bomb;
	int		y_Bomb;
}DEPTHBOMB,*PDEPTHBOMB;

#define	MAX_TORPEDO_NUM			10		//最大鱼雷数
typedef struct	tagTorpedo
{
#define		TORPEDO_STATE_ALIVE	0
#define		TORPEDO_STATE_DIED	1
	int		nState;				//状态
	int		nStyle;				//SHOT_NORMAL、SHOT_MISSIL
	int		x;
	int		y;
	double	x_save;				//X方向小数点以后的量，够1或-1，加入增量
	double	y_save;				//Y方向小数点以后的量，够1，加入增量
}TORPEDO,*PTORPEDO;

typedef struct	tagSubPlane
{
#define		PLANE_STATE_ALIVE	0
#define		PLANE_STATE_DIED	1
	int		nState;				//状态
	int		x;
	int		y;
}SUBPLANE,*PSUBPLANE;

/**********************************************
*
*	函数说明
*
***********************************************/
//主窗口过程
static LRESULT CALLBACK BombMainWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
//设置窗口过程
static LRESULT CALLBACK BombSetupWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
//游戏窗口过程
static LRESULT CALLBACK BombGameWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
//英雄榜窗口过程
static LRESULT CALLBACK BombHeroWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
//英雄留名窗口过程
static LRESULT CALLBACK BombSetHeroWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static	BOOL	ShowArrowState(HWND hWnd,int nSel);			//主界面箭头显示
static	int	GetAreaByPos(int x,int y);						//得点中区域
static BOOL MyDrawMenuItem(HDC hdc,int i,int nSel);			//画菜单项
static DWORD	BombMainSetup(HWND hWnd);					//启动设置窗口
static DWORD	BombNewGameProc(HWND hWnd);					//启动游戏窗口
static DWORD	BombHeroProc(HWND hWnd);					//英雄榜入口
static	DWORD   SetHeroProc(HWND hWnd);						//英雄留名入口
static BOOL	OnDepthBombGamePaint(HWND hWnd,WPARAM wParam,LPARAM lParam);	//游戏窗口paint过程
//static BOOL	DrawWave(HDC hdc);								//画浪
static void	InitGunShip();									//初始化炮船
static BOOL	DrawGunShip(HDC hdc);							//画炮船
static int	GetShipDir(BOOL bLeft,BOOL bRight);				//得炮船移动的方向
static BOOL	EndGameDepthBomb(HWND hWnd);					//结束游戏
static int random(int n);									//得0到n-1的随机数
static BOOL	InitSubmarine();								//初始化全部潜艇
static int	GetFirstDiedSubInd();							//得第一个未用潜艇
static BOOL	ProduceSub(const int i);						//填充潜艇结构
static BOOL	MoveSubmarine();								//移动活的潜艇
static BOOL	ScanSubDispear();								//扫描，使消失的潜艇死亡
static int	LayOverSub(const int i);						//判断第i个潜艇是否与其他活着的潜艇重叠
static BOOL	DrawSubmarine(HDC hdc);							//画潜艇
static int	FindFirstBombByState(const int state);			//找第一个指定状态的炸弹
static BOOL	ShotBomb(int i);								//发射炸弹
static BOOL LayOverDepBomb(const int i);					//判断第i个炸弹是否覆盖其他炸弹
static BOOL	OnGameDestroy(HWND hWnd,WPARAM wParam,LPARAM lParam);			//游戏界面销毁的处理
static BOOL	DropDepthBomb();								//使发射的炸弹下落
static BOOL	JudgeBombDispear(const int i);					//判断第i个炸弹是否消失
static BOOL	DrawShotBomb(HDC hdc);							//画发射的炸弹
static BOOL	InitDepthBomb();								//初始化炸弹
static BOOL DrawAvailableBomb(HDC hdc);						//画待投的炸弹
static BOOL	TwoRectLayOver(RECT rect1,RECT rect2);			//判断两个矩形是否重叠
static BOOL	DepBombKillSub();								//炸弹炸潜艇
static BOOL	ChangeSubDyingFrame();							//改变潜艇死亡的针
static BOOL	InitPrizeStete();								//初始化奖励的状态
static int	FindFirstDiedPrize();							//寻找第一个未用的奖励
static BOOL	ShotPrize(int nSub);							//发射奖励
static BOOL	LayOverPrize(const int nPrize);					//判断奖励是否重叠
static BOOL	PrizeAscend();									//奖励上升
static BOOL	ShipEatPrize(int nPrize);						//吃奖励
static BOOL	DispearPrize(int nPrize);						//奖励出水后消失
static BOOL	DrawPrize(HDC hdc);								//画奖励
static BOOL	InitTorpedoState();								//初始化鱼雷状态
static int	FindFirstDiedTorpedo();							//找第一个未用鱼雷
static BOOL	DispearTorpedo(const int nTorpedo);				//判断指定鱼雷消失
static BOOL	ShotTorpedo();									//发射鱼雷
static BOOL	LayOverTorpedo(const int nTorpedo);				//判断鱼雷是否重叠
static BOOL	TorpetoAscend();								//鱼雷上升
static BOOL	DrawTorpedo(HDC hdc);							//画鱼雷（含导弹）
static BOOL	TorpedoKillShip(const int nTorpedo);			//鱼雷杀船
static BOOL	DrawShipLive(HDC hdc);							//画船的命
static BOOL	KillAllSubmarine();								//杀掉所有活着的潜艇
static void MakeGunShipInRect();							//使炮船在游戏区域
static	void	InitBombHero();								//初始化英雄信息
static	BOOL	ReadBombHero();								//读深水炸弹英雄
static	BOOL	WriteBombHero();							//写深水炸弹英雄
static	BOOL	ReadBombSetup();							//读深水炸弹设置
static	BOOL	WriteBombSetup();							//写深水炸弹设置
static void	SortRecord();									//重排英雄数组
static BOOL	RetrieveGunShip();								//重新得到炮船
static DWORD	OnGameHelp(HWND hWnd);						//启动帮助窗口
static BOOL	SetMenuItemRectLocal();
static void InitSubPlane();									//初始化飞机
static BOOL	ActiveSubPlane();								//激活飞机
static BOOL	KillSubPlane();									//杀飞机
static BOOL	MoveSubPlane();									//飞机飞行
static BOOL	DrawSubPlane(HDC hdc);							//画飞机
static BOOL InitPlaneBomb();								//初始化飞机炸弹
static BOOL	PlaneBombDispear(int i);						//判断并使第i个飞机炸弹消失
static int GetFirstAvailablePlaneBomb();					//得第一个可用的飞机炸弹
static BOOL	ShotPlaneBomb();								//发射飞机炸弹
static BOOL	MovePlaneBomb();								//移动飞机炸弹
static BOOL	DrawPlaneBomb(HDC hdc);							//画飞机炸弹

/**********************************************
*
*	全局变量
*
***********************************************/
static HINSTANCE	hBombInstance;				//实例句柄
static HWND			hWnd_BombMain;				//主窗口句柄
static HWND			hWndBombSetup;				//设置窗口句柄
static HWND			hWndBombGame;				//游戏窗口句柄
static HWND			hWndBombHero;				//英雄榜窗口句柄
static HWND			hWnd_Max_Num;				//控件句柄
static HWND			hWnd_Prize;					//控件句柄
static HWND			hGameSetHero;				//英雄留名窗口句柄
static HWND			hBombHeroName;				//姓名控件句柄
static HBITMAP		hCoverBg;					//主窗口背景图句柄
//static HBITMAP		hMenuBg;					//菜单图片	
static int			nSelMenuItem;				//主窗口假菜单当前高亮项
static char MenuItemText[MENU_ITEM_NUM][9];		//菜单文字
static RECT	MenuItemRect[MENU_ITEM_NUM];		//菜单项矩形
static RECT	SelItemRect[MENU_ITEM_NUM];			//图标矩形
//static HBITMAP		hBmpSeaSurFace;				//水面图画句柄
static BOOL			bGameStart = FALSE;			//游戏开始标志
static BOOL			bGamePause = FALSE;			//游戏暂停标志
//static int			nGameTime = GAME_TIME(0);	//游戏长度
static int			nWaveFrame = 0;				//浪移动的针，0到4
static int			nGunShipWidth,nGunShipHeight;	//炮船的宽和高
static int			xGunShip;					//炮船的横坐标
static int			nGunShipLive;				//炮船的命数
#define	GUNSHIP_DIE_FRAME_NUM	11
static int			nGunShipDieFrame;			//炮船死亡的针
#define	GUNSHIP_STATE_LIVE		0
#define	GUNSHIP_STATE_DYING		1
static int			nGunShipState;				//炮船的状态
static HBITMAP		hBmpGunShip;				//炮船的图画句柄
static int			nShipMoveDir = DIR_NONE;	//炮船移动方向
static BOOL			bLeftDown,bRightDown;		//是否按下左右键
static SUBMARINE	SubMarine[MAX_SUBNUM];		//总潜艇数组
//static HBITMAP		hDirLeftSub,hDirRightSub;	//左右潜艇句柄
static HBITMAP		hBmpDepthSub;				//潜艇图句柄
static HBITMAP		hBmpGameBack;				//游戏背景图
static HBITMAP		hBmpGameScoreText;				//游戏背景文字
static DEPTHBOMB	DepthBomb[DEPTHBOMB_NUM];	//总炸弹数组
static DEPTHBOMB	PlaneBomb[PLANE_BOMB_NUM];	//总飞机炸弹数组
//static HBITMAP		hDepthBomb;					//发射的炸弹图句柄
static HBITMAP		hShipBombAndLife;			//可用炸弹和命的句柄
//static HBITMAP		hAvailableBomb;				//可用的炸弹图句柄
//static HBITMAP		hLSubDying[DIEFRAMENUM];	//向左潜艇炸死动画句柄数组
//static HBITMAP		hRSubDying[DIEFRAMENUM];	//向右潜艇炸死动画句柄数组
static int			nScore = 0;					//得分
static int			nOldScore = 0;				//前一次的得分
static SUBPRIZE		SubPrize[SUBPRIZE_NUM];		//总的奖励的数组
//static HBITMAP		hSubPrize[SUBPRIZE_NUM];	//奖励图的句柄
static HBITMAP		hBmpPrizeAndBomb;			//奖励和炸弹句柄
static TORPEDO		Torpedo[MAX_TORPEDO_NUM];	//总的鱼雷数组
//static HBITMAP		hTorpedo,hMissil;			//鱼雷和导弹的句柄
static HBITMAP		hBmpTorpedo;				//鱼雷（两种）的句柄
//static HBITMAP		hShipLive;					//船的命图片句柄
static int			nNoEnemyTimeLeft;			//剩余无敌时间
static int			nMinTimeLeft;				//剩余船变小时间
static HBITMAP		hBmpMinGunShip;				//变小的船的图片句柄
//static HBITMAP		bBmpNoEnemy;				//无敌标志图的句柄
static BOMBHERO		BombHero[BOMB_HERO_NUM];	//英雄数组
static int			nBombNum;					//炸弹数
static BOOL			bPrizePermit;				//是否允许有奖励
//static	InputEdit InputEditNode;
static	RECT	TitleRect={10,40,170,65};
static	RECT	NameEditRect={15,90,160,120};	//姓名控件位置
static	HWND		hWndHelp;					//帮助控件句柄
static HBITMAP	hBmpCovSel = NULL;				//选择图标
static SUBPLANE	SubPlane;						//飞机
static HBITMAP	hBmpPlane = NULL;				//飞机句柄
static BOOL	bGameOver = FALSE;					//游戏是否结束
static	HMENU		hMainMenu	=	NULL;

/**********************************************
*
*	菜单控件ID
*
***********************************************/
#define	IDM_NEWGAME		WM_USER+1001
#define	IDM_SETUP		WM_USER+1002
#define	IDM_HEROBOARD	WM_USER+1003
#define	IDM_EXIT		WM_USER+1004
#define	IDM_CONFIRM		WM_USER+1005
#define	IDM_GAMESTART	WM_USER+1006
#define	IDM_HELP		WM_USER+1007


/**********************************************
*
*	控件ID
*
***********************************************/
#define	IDC_SPINMAX		1//WM_USER+2001
#define	IDC_BTN_PRIZE	2//WM_USER+2002
#define	IDC_HERONAME	3//WM_USER+2003

/**********************************************
*
*	自定义消息
*
***********************************************/
#define	IDRM_RMSG		WM_USER+1010

/**********************************************
*
*	菜单模板
*
***********************************************/

static MENUITEMTEMPLATE Bomb_Main_Menu[] = 
{
    { MF_STRING, IDM_NEWGAME, NULL, NULL },
    { MF_STRING, IDM_SETUP,   NULL,   NULL },
	{ MF_STRING, IDM_HEROBOARD,   NULL, NULL},
	{ MF_STRING, IDM_HELP,   NULL, NULL},
	{ MF_END, 0, NULL, NULL }
};
#if 0
static MENUITEMTEMPLATE Bomb_Main_Menu[] = 
{
    { MF_STRING, IDM_NEWGAME, IDS_NEWGAME, NULL },
    { MF_STRING, IDM_SETUP,   IDS_SETUP,   NULL },
	{ MF_STRING, IDM_HEROBOARD,   IDS_HEROBOARD, NULL},
	{ MF_END, 0, NULL, NULL }
};
#endif

static const MENUTEMPLATE BombMainMenuTemplate =
{
    0,
    Bomb_Main_Menu
};
