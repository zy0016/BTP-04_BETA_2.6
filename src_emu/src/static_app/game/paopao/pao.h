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
#include "winpda.h"
#include "window.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"
#include "app.h"
//#include "fapi.h"
#include "math.h"
#include "plx_pdaex.h"
#include "hpimage.h"
#ifdef SCP_SMARTPHONE
#include "msgwin.h"
#endif
#include "paors.h"
#include "GameHelp.h"
#include "mullang.h"//multi language
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
#define	MENU_ITEM_NUM			4
#define	Pao_Ball_Num			4					//球的种类数
#define	MIN_GUN_DEGREE			30					//枪与x轴反方向最小夹角
#define	MAX_GUN_DEGREE			150					//枪与x轴反方向最大夹角
#define	NORMAL_DEGREE			90					//枪与x轴反方向初始夹角
#define	BALL_STEP				(-RADIUS_BALL)		//球移动步长
#define	PI						3.1415926525
#define	D_INTERVAL				3					//每次转动角度
#define	BALL_PER_LINE			8					//每行球数
#define	NORMAL_DEADLINE			9//11					//正常情况,死亡的行,0为基数
#define	Y_DEADLINE				((NORMAL_DEADLINE+1)*Y_BALL_INTERVAL+Y_GAMEBACK)	//死亡的行的纵坐标
#define	DEL_FRAME_NUM			3					//消去球的动画帧数
#define	MOVE_TIMER_ID			1					//球移动TIMER的ID
#define	DEL_TIMER_ID			2					//删除球TIMER的ID
#define	PRESS_TIMER_ID			3					//定时发射TIMER的ID
#define	GUN_TIMER_ID			4					//枪移动TIMER的ID
#define	DRAGEN_TIMER_ID			5					//龙动作TIMER的ID
#define DOWNWALL_TIMER_ID		6					//墙下落TIMER的ID
#define MOVE_GUN_ID				7					//枪口移动TIMER的ID
#define ROUND_TIMER_ID			8					//显示每一关的关数
#define	MOVE_TIMER_ELAPSE		30					//球移动帧的长度
#define	DEL_TIMER_ELAPSE		30					//删除球TIMER的长度
#define	PRESS_TIMER_ELAPSE		4000				//定时发射TIMER的长度
//#define	GUN_TIMER_ELAPSE		200					//枪移动帧的长度
#define	DRAGEN_TIMER_ELAPSE		200					//龙动作间隔
#define	SCORE_PER_BALL			2					//每球得分
#define	SCORE_PER_DROP			3					//每掉下一球得分
#define	TOP_REC_NUM				5					//英雄数
#define HERONAMEFORMAT			"%-8s %5d"		//英雄名字输出格式
#define CreateCapButton(hwnd, lowparam, hiwparam, lparam) \
	SendMessage(hwnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(lowparam,hiwparam), (LPARAM)lparam)

//#if defined SCP_NOKEYBOARD
#define	PAO_SETUP_FILE			"/mnt/flash/game/paosetup.ini"
#define	PAO_HERO_FILE			"/mnt/flash/game/paohero.ini"
/*#else
#define	PAO_SETUP_FILE			"Flash3:/PaoSetup.dat"
#define	PAO_HERO_FILE			"Flash3:/PaoHero.dat"
#endif*/

#define	PAO_LDRANGE1			"/rom/game/pao/pao_ldr1.gif"
#define	PAO_LDRANGE2			"/rom/game/pao/pao_ldr2.gif"
//#define	PAO_RDRANGE1			"Rom:Pao_Rdr1.gif"
//#define	PAO_RDRANGE2			"Rom:Pao_Rdr2.gif"
#define	BALLPATH0				"/rom/game/pao/pao_ball.gif"
#define	BALLPATH1				"/rom/game/pao/pao_blu2.gif"
#define	BALLPATH2				"/rom/game/pao/pao_gre2.gif"
#define	BALLPATH3				"/rom/game/pao/pao_red2.gif"
#define	BALLPATH4				"/rom/game/pao/pao_yel2.gif"
#define	BALLPATH5				"/rom/game/pao/pao_blu3.gif"
#define	BALLPATH6				"/rom/game/pao/pao_gre3.gif"
#define	BALLPATH7				"/rom/game/pao/pao_red3.gif"
#define	BALLPATH8				"/rom/game/pao/pao_yel3.gif"
#define	PAOBACK_FILE			"/rom/game/pao/pao_back.gif"
#define	PAOGUN_FILE				"/rom/game/pao/pao_gun.gif"
#define	COVERBG					"/rom/game/pao/bubble_shooter.bmp"
#define PAOWALL					"/rom/game/pao/pao_wall.gif"
#define PAOROUNDBACK			"/rom/game/pao/pao_rbg.gif"
#define PAOROUNDNUM				"/rom/game/pao/pao_rnum.gif"
#define PAOGAMEOVER				"/rom/game/pao/gameover.gif"
#define PAOBACKGROUND			"/rom/game/pao/score_bg.gif"
#define	PAOCOVSEL				"/rom/game/pao/pao_sel.gif"
#define PAOMAP					"/rom/game/pao/paomap.ini"
//#define PAOSCORE				ML(STR_PAO_SCORET)
//#define PAOSCORE				"/rom/game/pao/pao_scoe.gif"
//左上角图标
#ifdef SCP_SMARTPHONE
#include "hp_icon.h"
#define	DIRICON		"/rom/game/pao/pao.ico"//GAME_PAO_ICON
#endif

//MessageBox
/*
#ifdef	SCP_SMARTPHONE
#define	MessageBox(a,b,c,d)	MsgWin(b,c,WAITTIMEOUT)
#else
#define	MessageBox(a,b,c,d)	TipMessageBox(a,b)
#endif
*/
/**********************************************
*
*	数据结构
*
***********************************************/
//球的数据结构
typedef	struct	tagTheBall
{
	int	i;			//行数
	int	j;			//列数
	POINT	lt;		//左上角坐标
	int	color;		//颜色,-1表示球不存在
}THEBALL;

//龙的数据结构
typedef	struct	tagDragen
{
#define	LEFT_DRAGEN		0	//左边的龙
#define	RIGHT_DRAGEN	1	//右边的龙
	int	Dir;		//左还是右
	int	i;			//针，可以是0或1
	POINT	lt;		//左上角坐标
}DRAGEN;

//英雄榜的数据结构
typedef struct	tagPaoHero
{
	int		nScore;
	char	Name[15];
	
}PAOHERO;

/**********************************************
*
*	函数说明
*
***********************************************/
LRESULT PaoMainWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);		//主窗口过程
LRESULT PaoSetupWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);		//设置窗口过程
LRESULT PaoGameWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);		//游戏窗口过程
LRESULT PaoHeroWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);		//英雄榜界面过程函数
LRESULT PaoSetHeroWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);	//英雄留名界面过程函数
LRESULT PaoGameOverWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);	//结束窗口过程
static	DWORD	PaoSetupProc(HWND hWnd);											//调用设置界面
static	BOOL	ReadPaoSetup();
static	BOOL	WritePaoSetup();
static	DWORD	PaoGameProc(HWND hWnd);												//调用游戏界面
static	DWORD	PaoHeroProc(HWND hWnd);												//显示英雄榜界面
static	DWORD	SetPaoHeroProc(HWND hWnd);											//显示英雄留名界面
static  DWORD   PaoGameShowProc(HWND hWnd);											//显示游戏说明界面
DWORD			PaoGameOverProc();													//结束
static	void	OnPaoGamePaint(HDC hdc);
static	void	InitPaoGame(HWND hWnd);												//初始化游戏
static	void	InitBall(HWND hWnd);												//初始化球
static	int		GetColor();															//得球颜色
static	int		OnPaoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam);				//KEYDOWN消息处理过程
static	void	DrawGun(HDC hdc);													//画枪口
static	double	DTOR(int Degree);													//角度变弧度
static	int		DTOI(double	a);														//四舍五入 
static	void	InitStaticBall();													//初始化静态球
static	void	DrawStaticBall(HDC hdc);											//画静态球
static	void	ShootBall(HWND	hWnd);												//发射
static	void	CalcNewPos();														//计算新位置
static	void	GetPosFromOffset(int k);													//从偏移量得位置
static	BOOL	ShouldMove();														//是否应该继续移动
static	int		SquarePlus(int i,int j);											//求平方和
static	int		Square(int a);														//求平方
static	BOOL	NeighborBall(int i,int j);											//判断是否与球相临
static	BOOL	IsBall(int i,int j);												//判断是否是球
static	BOOL	IsDeadPao();														//判断是否死了
static	void	EndPaoGame(HWND hWnd);												//一局结束的操作
static	void	DelPao(int i,int j);												//删除三个以上相同球
static	BOOL	CanDropPao(int i,int j);											//判断是否掉下来
static	void	DropPao(int i,int j);												//球掉下来
static	BOOL	IsABall(int i,int j);												//判断是否是球
static	BOOL	IsSameBall(int i,int j);											//判断(i,j)点是否与MoveBall颜色相同
static	int		CanDelPao(int i,int j);												//判断有否三个以上相同球
static	void	InitPaoHero();														//初始化英雄信息
static	BOOL	ReadPaoHero();														//读英雄信息
static	BOOL	WritePaoHero();														//写英雄信息
static	void	SortRecord();														//重排英雄数组
//static	int		OnPaoChar(HWND hWnd,WPARAM wParam,LPARAM lParam);				//处理WM_CHAR消息
//static	BOOL	ReadMapNum();													//读地图数
static	void	InitMapNum();														//初始化地图数
//static	BOOL	WriteMapNum();													//写地图数
//static	BOOL	WriteMap();														//写地图
//static	BOOL	ReadMap(int n);
static  BOOL	ReadMapNum(int fp);
static	void	FileReadLine(int fp, char** myString);
static  int     StrToInt ( char * pString);
static	BOOL	ReadMapNew(int n);													//读地图
static	void	UncheckBall();														//置Check数组为FALSE
static	BOOL	IsVoid(int i,int j);												//是否是空
static	void	AdjustBall();														//调整停球位置
static	BOOL	IsSetOver();														//一局结束的判断
static	void	OnDelTimer(HWND hWnd);												//消去球的处理
static	void	OnGunMove(HWND hWnd);												//枪移动的处理
static	void	InitDragenI();														//初始华龙的变量i
static	void	CalcDragenLT();														//计算龙的位置
static	void	ChangeDragenI();													//改变龙的动画
static	BOOL	MyDrawMenuItem(HDC hdc,int i,int nSel);								//画菜单项
static	BOOL	ShowArrowState(HWND hWnd,int nSel);									//主界面箭头显示
static	int	    GetAreaByPos(int x,int y);
static	void	ShowArrowInGameState(HWND hWnd);
static	void	ShowRoundNum();											//得点中区域
static	void	InitSelItem();
/**********************************************
*
*	全局变量
*
***********************************************/
static	HWND	hWnd_PaoMain = NULL;					//主窗口句柄
static	HWND	hWndPaoGame = NULL;						//游戏窗口句柄
static	HWND	hWndPaoHero = NULL;						//英雄榜窗口句柄
static	HWND	hPaoHeroName = NULL;					//Edit句柄
static	HWND	hPaoGameOver = NULL;					//游戏结束
static	HMENU	hMainMenu = NULL;						//主菜单
static	HWND	hWnd_SetPress;							//是否下降BTN句柄
static	HWND	hWnd_SetMove;							//是否移动BTN句柄
static	HWND	hWndPaoSetup = NULL;					//设置窗口句柄
static	BOOL	bSetupPress = FALSE;					//是否定时射出
//static	BOOL	bSetupMove = FALSE;						//是否移动
static	BOOL	bGameStart = FALSE;						//游戏开始
static	HBITMAP	hbmpBall[9];
static	RECT	GameFrameRect = {X_GAMEBACK-1,Y_GAMEBACK-1,
	X_GAMEBACK+GAMEBACK_WIDTH+1,Y_GAMEBACK+GAMEBACK_HEIGHT+1};			//游戏框架矩形
static	RECT	GameBackRect = {X_GAMEBACK,Y_GAMEBACK,
				X_GAMEBACK+GAMEBACK_WIDTH-1,Y_GAMEBACK+GAMEBACK_HEIGHT-1};			//游戏背景矩形
static	HBITMAP	hPaoBack = NULL;						//背景图句柄
static	HBITMAP	hPaoGun = NULL;							//枪图画句柄
static	HBITMAP	hPaoWall = NULL;						//下落的墙的句柄		
static	HBITMAP	hDragen[2][2];							//泡泡龙的句柄
static	int		nGunOffset = -1;						//枪偏移量
static	int		nScore = 0;								//分数
static	int		Ball_x_Offset = -1;						//当前球x偏移量
static	int		Ball_y_Offset = -1;						//当前球y偏移量
static	double	Plus_x = 0;								//水平方向多余量
static	double	Plus_y = 0;								//竖直方向多余量
static	int		Gun_Degree	= NORMAL_DEGREE;			//枪与x轴反方向夹角
static	int		Move_Degree	= NORMAL_DEGREE;			//球移动方向与x轴反方向夹角	
static	THEBALL	StaticBall[NORMAL_DEADLINE+1][BALL_PER_LINE];			//静止球
static	BOOL	bChecked[NORMAL_DEADLINE+1][BALL_PER_LINE];				//检查标志
static	THEBALL	MoveBall;								//运动球
static  THEBALL ToMoveBall;								//下一个的运动球
static	int		DeadLine = 	NORMAL_DEADLINE;			//死亡的行
static	int		nDelBall = 0;							//消去的球数
static	int		nDropBall = 0;							//掉下来的球数
static	PAOHERO	PaoRecord[TOP_REC_NUM];					//英雄记录
static	RECT	TitleRect={10,40,170,65};
static	RECT	NameEditRect={15,90,160,120};
//static	InputEdit InputEditNode;
static	char	CurChar;								//记录秘计当前char
static	int		nMapNum;								//地图数
static	int		nCurMap;								//当前地图,0为基数,-1表示无地图
static	int		nCurFrame = 0;							//当前帧数1
//static	int		nSecFrame = 0;						//当前帧数2
static	BOOL	bFirst = FALSE;							//第一次进入游戏
static	HINSTANCE	hPaoInstance;						//实例句柄
static	DRAGEN	Dragen;									//泡泡龙
static	BOOL	bDragenTimer = FALSE;					//泡泡龙Timer设了
static  HBITMAP	hCoverBg;
static	HBITMAP hCovSel;								//游标
static  RECT	SELRECT[MENU_ITEM_NUM];
static  int     nSelMenuItem = 0;						//选中菜单项0--2
static  char    MenuItemText[MENU_ITEM_NUM][9];
static  HWND    hWndHelp;
static  const	int   Interval_Wall = 25000;			//墙落的速度
static  const	int	  Interval_Gun = 200;				//枪口移动的速度
static  const	int	  Interval_Round = 1500;			//每一关的关数在游戏前显示两秒钟
static  int     DropLine = 0;							//墙下落的行数
static  BOOL	bGamePause = FALSE;						//焦点不在游戏界面
static  BOOL	bLeftDown,bRightDown;					//是否按下左右键
static  char	LastKeyDown = 'L';
static  HBITMAP	hRoundBack;								//显示分数的背景位图的句柄
static  HBITMAP	hRoundNum;								//显示分数的数字位图的句柄	
static  BOOL	bRoundBN = FALSE;						//是否该显示分数
static  BOOL	bDownWallTID;							//各种定时器是否设置
static  BOOL	bMoveTID;
static  BOOL	bDelTID;
static  BOOL	bPressTID;
static  BOOL	bGunTID;
static  BOOL	bDragenTID;
static  BOOL	bMoveGunTID;
static  BOOL	bRoundTID;
static  BOOL	IsDead;
static  COLORREF	clr1;
static  COLORREF	clr2;
static  COLORREF	clr3;
static  COLORREF	clr4;
static  COLORREF	clr5;
static  COLORREF	clr6;
static  BOOL	bclarity;

static  BOOL	bRSetupWnd = FALSE;
static  BOOL	bRGameWnd = FALSE;
static  BOOL	bRSetHeroWnd = FALSE;
static  BOOL	bRHeroWnd = FALSE;
static  BOOL	bRGameOverWnd = FALSE;

static	RECT	Rect;
static  BOOL    bMoveBallDraw = TRUE;
static	POINT	From,To;
static  BOOL    bWallShouldDown = FALSE;
static	HBITMAP	hPaoScore = NULL;						//得分句柄		

/**********************************************
*
*	菜单控件ID
*
***********************************************/
#define	IDM_NEWGAME				WM_USER+1001			//新游戏
#define	IDM_SETUP				WM_USER+1002			//设置
#define	IDM_HEROBOARD			WM_USER+1003			//英雄榜
#define	IDM_EXIT				WM_USER+1004			//退出
#define	IDM_CONFIRM				WM_USER+1005			//确定
#define	IDM_GAMESTART			WM_USER+1006			//游戏开始
#define	ID_OVER_RETURN			WM_USER+1007			//确定
#define IDM_GAMESHOW			WM_USER+1008			//游戏说明

#define	IDRM_RMSG				WM_USER+1009
#define	IDRM_GAMEOVER			WM_USER+1010
/**********************************************
*
*	控件ID
*
***********************************************/
#define	IDC_SETPRESS			1//WM_USER+2001			//是否下降BTN的ID
#define	IDC_SETMOVE				2//WM_USER+2002			//是否移动BTN的ID
#define	IDC_HERONAME			3//WM_USER+2003			//英雄姓名EDIT的ID

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
/*#ifdef	SCP_SMARTPHONE
static MENUITEMTEMPLATE Pao_Main_Menu[] = 
{
    { MF_STRING, IDM_NEWGAME, NULL, NULL },
    { MF_STRING, IDM_SETUP,   NULL,   NULL },
	{ MF_STRING, IDM_HEROBOARD,   NULL, NULL},
	{ MF_STRING, IDM_GAMESHOW, NULL, NULL},
	{ MF_END, 0, NULL, NULL }
};
#else
static MENUITEMTEMPLATE Pao_Main_Menu[] = 
{
    { MF_STRING, IDM_NEWGAME, IDS_NEWGAME, NULL },
    { MF_STRING, IDM_SETUP, IDS_SETUP, NULL },
	{ MF_STRING, IDM_HEROBOARD, IDS_HEROBOARD, NULL},
	{ MF_STRING, IDM_GAMESHOW, IDS_GAMESHOW, NULL},
	{ MF_END, 0, NULL, NULL }
};
#endif

static const MENUTEMPLATE PaoMainMenuTemplate =
{
    0,
    Pao_Main_Menu
};*/
