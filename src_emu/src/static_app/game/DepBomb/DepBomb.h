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
*	��������
*
***********************************************/

#define CreateCapButton(hwnd, lowparam, hiwparam, lparam) \
	SendMessage(hwnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(lowparam,hiwparam), (LPARAM)lparam)
//���Ͻ�ͼ��
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

#define HERONAMEFORMAT		"%-8s %5d"	//"%d. %-8s%5d"	//Ӣ�����������ʽ

#define	DIR_LEFT				0			//����
#define	DIR_RIGHT				1			//����
#define	DIR_NONE				2			//ֹͣ����
#define	MENU_ITEM_NUM			4			//�˵������
#define	BASE_GAME_TIME			30			//������Ϸʱ��
#define	GAME_TIME_STEP			30			//��Ϸʱ�����ӵĲ���
#define	GAME_TIME(i)			(BASE_GAME_TIME+i*GAME_TIME_STEP)		//��Ϸ��ʱ�䳤��
#define	GAME_TIMER_ID			1			//һ����Ϸʱ�䳤�ȵ�ID
#define	MAX_WAVE_FRAME			5			//���ƶ��������ֵ��0��4
#define	WAVE_MOVE_TIMER_ID		2			//���ƶ�TIMER��ID
#define	WAVE_MOVE_TIMER_ELAPSE	200			//���ƶ���ʱ����
#define	SHIP_MOVE_TIMER_ID		3			//�ƶ�TIMER��ID
//#define	UPSHIP_MOVE_TIMER_ID	8			//�����ƶ�TIMER��ID
#define	SHIP_MOVE_ELAPSE		250			//�ڴ��ƶ���ʱ����
//#define	UPSHIP_MOVE_ELAPSE					//�ڴ��ƶ���ʱ����

#define	SUBNUM_PERPRIZE			10			//ÿ����Ǳͧ��һ������
#define	SUB_MOVE_STEP			3			//Ǳͧÿ���ƶ�ֵ
#define	BOMB_DROP_STEP			3			//ը��ÿ������ֵ
#define	SCORE_PER_SUB			10			//ÿըһ��Ǳͧ�ĵ÷�
#define	PRIZE_ASCEND_INTERVAL	2			//���������ļ��
#define	TIMES_PER_TORPETO		18			//ÿ����һ�����׵�Ƶ��
#define	TORPEDO_MOVE_STEP		3			//�����ƶ��Ĳ���
#define	GUNSHIP_START_LIVE		5			//�ڴ��ĳ�ʼ����
#define	NO_ENEMY_TIME			15			//�޵е�ʱ�䣨������
#define	SHIP_MIN_TIME			15			//����Сʱ�䣨������
#define	SCORE_PER_PLANE			1000		//ÿ�ö��ٷ���һ�ܷɻ�

/**********************************************
*
*	���ݽṹ
*
***********************************************/
#define	BOMB_HERO_NUM			5			//Ӣ�۸���
typedef struct	tagBombHero
{
	char	Name[15];
	int		nScore;
}BOMBHERO;

#define	MAX_SUBNUM				8			//���Ǳͧ��
typedef	struct	tagSubmarine
{
#define		STATE_ALIVE		0
#define		STATE_DIED		1
#define		STATE_DYING		2
	int		state;			//Ǳͧ״̬
#define		DIEFRAMENUM		3
//����Ǳͧ��������
#define	X_SUBTOR_DIEFRAME(i)	(SUBMARINE_WIDTH*(2+DIEFRAMENUM-i-1))
//����Ǳͧ��������
#define	X_SUBTOL_DIEFRAME(i)	(SUBMARINE_WIDTH*(2+DIEFRAMENUM*2-i-1))
	int		DieFrame;		//��ȥ����
#define		SUB_TIERNUM		4
#define		Y_FIRSTSUB		82//100
#define		SUB_Y_INTERVAL	20
#define		SUB_Y(i)		(Y_FIRSTSUB+i*SUB_Y_INTERVAL)
	int		y_Sub;			//������
	int		x_Sub;			//������
	int		nDirect;		//����������
#define		SHOT_NORMAL		0
#define		SHOT_MISSIL		1
	int		SubShotStyle;	//Ǳͧ���ڵ�����
#define		NO_PRIZE		0
#define		PRIZE_MIN		1
#define		PRIZE_KILLALL	2
#define		PRIZE_NOENEMY	3
#define		PRIZE_NUM		3//����������
	int		Prize;			//Ǳͧ��ը���Ư����
	struct	tagSubmarine	*pNext;
}SUBMARINE,*PSUBMARINE;

#define		SUBPRIZE_NUM	3			//�������
#define		X_SUBPRIZE(i)	(BOMB_SHOT_WIDTH+NOENEMY_WIDTH+(i-1)*PRIZE_WIDTH)
typedef	struct	tagSubPrize
{
	int PrizeStyle;			//����������
#define	PRIZESTATE_ALIVE	0
#define	PRIZESTATE_DIED		1
	int	PrizeState;			//������״̬
	int x_Prize;
	int y_Prize;
}SUBPRIZE,*PSUBPRIZE;

#define		DEPTHBOMB_NUM	5			//���ը����
#define		BASIC_BOMB_NUM	3			//��Сը����
#define		PLANE_BOMB_NUM	10			//�ɻ�Ͷ����ը����
typedef	struct	tagDepthBomb
{
#define		STATE_AVAILABLE		0		//����
#define		STATE_SHOTTED		1		//�Ѿ�Ͷ��
#define		STATE_INAVAILABLE	2		//������
	int		state;						//ը��״̬
	int		x_Bomb;
	int		y_Bomb;
}DEPTHBOMB,*PDEPTHBOMB;

#define	MAX_TORPEDO_NUM			10		//���������
typedef struct	tagTorpedo
{
#define		TORPEDO_STATE_ALIVE	0
#define		TORPEDO_STATE_DIED	1
	int		nState;				//״̬
	int		nStyle;				//SHOT_NORMAL��SHOT_MISSIL
	int		x;
	int		y;
	double	x_save;				//X����С�����Ժ��������1��-1����������
	double	y_save;				//Y����С�����Ժ��������1����������
}TORPEDO,*PTORPEDO;

typedef struct	tagSubPlane
{
#define		PLANE_STATE_ALIVE	0
#define		PLANE_STATE_DIED	1
	int		nState;				//״̬
	int		x;
	int		y;
}SUBPLANE,*PSUBPLANE;

/**********************************************
*
*	����˵��
*
***********************************************/
//�����ڹ���
static LRESULT CALLBACK BombMainWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
//���ô��ڹ���
static LRESULT CALLBACK BombSetupWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
//��Ϸ���ڹ���
static LRESULT CALLBACK BombGameWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
//Ӣ�۰񴰿ڹ���
static LRESULT CALLBACK BombHeroWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
//Ӣ���������ڹ���
static LRESULT CALLBACK BombSetHeroWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static	BOOL	ShowArrowState(HWND hWnd,int nSel);			//�������ͷ��ʾ
static	int	GetAreaByPos(int x,int y);						//�õ�������
static BOOL MyDrawMenuItem(HDC hdc,int i,int nSel);			//���˵���
static DWORD	BombMainSetup(HWND hWnd);					//�������ô���
static DWORD	BombNewGameProc(HWND hWnd);					//������Ϸ����
static DWORD	BombHeroProc(HWND hWnd);					//Ӣ�۰����
static	DWORD   SetHeroProc(HWND hWnd);						//Ӣ���������
static BOOL	OnDepthBombGamePaint(HWND hWnd,WPARAM wParam,LPARAM lParam);	//��Ϸ����paint����
//static BOOL	DrawWave(HDC hdc);								//����
static void	InitGunShip();									//��ʼ���ڴ�
static BOOL	DrawGunShip(HDC hdc);							//���ڴ�
static int	GetShipDir(BOOL bLeft,BOOL bRight);				//���ڴ��ƶ��ķ���
static BOOL	EndGameDepthBomb(HWND hWnd);					//������Ϸ
static int random(int n);									//��0��n-1�������
static BOOL	InitSubmarine();								//��ʼ��ȫ��Ǳͧ
static int	GetFirstDiedSubInd();							//�õ�һ��δ��Ǳͧ
static BOOL	ProduceSub(const int i);						//���Ǳͧ�ṹ
static BOOL	MoveSubmarine();								//�ƶ����Ǳͧ
static BOOL	ScanSubDispear();								//ɨ�裬ʹ��ʧ��Ǳͧ����
static int	LayOverSub(const int i);						//�жϵ�i��Ǳͧ�Ƿ����������ŵ�Ǳͧ�ص�
static BOOL	DrawSubmarine(HDC hdc);							//��Ǳͧ
static int	FindFirstBombByState(const int state);			//�ҵ�һ��ָ��״̬��ը��
static BOOL	ShotBomb(int i);								//����ը��
static BOOL LayOverDepBomb(const int i);					//�жϵ�i��ը���Ƿ񸲸�����ը��
static BOOL	OnGameDestroy(HWND hWnd,WPARAM wParam,LPARAM lParam);			//��Ϸ�������ٵĴ���
static BOOL	DropDepthBomb();								//ʹ�����ը������
static BOOL	JudgeBombDispear(const int i);					//�жϵ�i��ը���Ƿ���ʧ
static BOOL	DrawShotBomb(HDC hdc);							//�������ը��
static BOOL	InitDepthBomb();								//��ʼ��ը��
static BOOL DrawAvailableBomb(HDC hdc);						//����Ͷ��ը��
static BOOL	TwoRectLayOver(RECT rect1,RECT rect2);			//�ж����������Ƿ��ص�
static BOOL	DepBombKillSub();								//ը��ըǱͧ
static BOOL	ChangeSubDyingFrame();							//�ı�Ǳͧ��������
static BOOL	InitPrizeStete();								//��ʼ��������״̬
static int	FindFirstDiedPrize();							//Ѱ�ҵ�һ��δ�õĽ���
static BOOL	ShotPrize(int nSub);							//���佱��
static BOOL	LayOverPrize(const int nPrize);					//�жϽ����Ƿ��ص�
static BOOL	PrizeAscend();									//��������
static BOOL	ShipEatPrize(int nPrize);						//�Խ���
static BOOL	DispearPrize(int nPrize);						//������ˮ����ʧ
static BOOL	DrawPrize(HDC hdc);								//������
static BOOL	InitTorpedoState();								//��ʼ������״̬
static int	FindFirstDiedTorpedo();							//�ҵ�һ��δ������
static BOOL	DispearTorpedo(const int nTorpedo);				//�ж�ָ��������ʧ
static BOOL	ShotTorpedo();									//��������
static BOOL	LayOverTorpedo(const int nTorpedo);				//�ж������Ƿ��ص�
static BOOL	TorpetoAscend();								//��������
static BOOL	DrawTorpedo(HDC hdc);							//�����ף���������
static BOOL	TorpedoKillShip(const int nTorpedo);			//����ɱ��
static BOOL	DrawShipLive(HDC hdc);							//��������
static BOOL	KillAllSubmarine();								//ɱ�����л��ŵ�Ǳͧ
static void MakeGunShipInRect();							//ʹ�ڴ�����Ϸ����
static	void	InitBombHero();								//��ʼ��Ӣ����Ϣ
static	BOOL	ReadBombHero();								//����ˮը��Ӣ��
static	BOOL	WriteBombHero();							//д��ˮը��Ӣ��
static	BOOL	ReadBombSetup();							//����ˮը������
static	BOOL	WriteBombSetup();							//д��ˮը������
static void	SortRecord();									//����Ӣ������
static BOOL	RetrieveGunShip();								//���µõ��ڴ�
static DWORD	OnGameHelp(HWND hWnd);						//������������
static BOOL	SetMenuItemRectLocal();
static void InitSubPlane();									//��ʼ���ɻ�
static BOOL	ActiveSubPlane();								//����ɻ�
static BOOL	KillSubPlane();									//ɱ�ɻ�
static BOOL	MoveSubPlane();									//�ɻ�����
static BOOL	DrawSubPlane(HDC hdc);							//���ɻ�
static BOOL InitPlaneBomb();								//��ʼ���ɻ�ը��
static BOOL	PlaneBombDispear(int i);						//�жϲ�ʹ��i���ɻ�ը����ʧ
static int GetFirstAvailablePlaneBomb();					//�õ�һ�����õķɻ�ը��
static BOOL	ShotPlaneBomb();								//����ɻ�ը��
static BOOL	MovePlaneBomb();								//�ƶ��ɻ�ը��
static BOOL	DrawPlaneBomb(HDC hdc);							//���ɻ�ը��

/**********************************************
*
*	ȫ�ֱ���
*
***********************************************/
static HINSTANCE	hBombInstance;				//ʵ�����
static HWND			hWnd_BombMain;				//�����ھ��
static HWND			hWndBombSetup;				//���ô��ھ��
static HWND			hWndBombGame;				//��Ϸ���ھ��
static HWND			hWndBombHero;				//Ӣ�۰񴰿ھ��
static HWND			hWnd_Max_Num;				//�ؼ����
static HWND			hWnd_Prize;					//�ؼ����
static HWND			hGameSetHero;				//Ӣ���������ھ��
static HWND			hBombHeroName;				//�����ؼ����
static HBITMAP		hCoverBg;					//�����ڱ���ͼ���
//static HBITMAP		hMenuBg;					//�˵�ͼƬ	
static int			nSelMenuItem;				//�����ڼٲ˵���ǰ������
static char MenuItemText[MENU_ITEM_NUM][9];		//�˵�����
static RECT	MenuItemRect[MENU_ITEM_NUM];		//�˵������
static RECT	SelItemRect[MENU_ITEM_NUM];			//ͼ�����
//static HBITMAP		hBmpSeaSurFace;				//ˮ��ͼ�����
static BOOL			bGameStart = FALSE;			//��Ϸ��ʼ��־
static BOOL			bGamePause = FALSE;			//��Ϸ��ͣ��־
//static int			nGameTime = GAME_TIME(0);	//��Ϸ����
static int			nWaveFrame = 0;				//���ƶ����룬0��4
static int			nGunShipWidth,nGunShipHeight;	//�ڴ��Ŀ�͸�
static int			xGunShip;					//�ڴ��ĺ�����
static int			nGunShipLive;				//�ڴ�������
#define	GUNSHIP_DIE_FRAME_NUM	11
static int			nGunShipDieFrame;			//�ڴ���������
#define	GUNSHIP_STATE_LIVE		0
#define	GUNSHIP_STATE_DYING		1
static int			nGunShipState;				//�ڴ���״̬
static HBITMAP		hBmpGunShip;				//�ڴ���ͼ�����
static int			nShipMoveDir = DIR_NONE;	//�ڴ��ƶ�����
static BOOL			bLeftDown,bRightDown;		//�Ƿ������Ҽ�
static SUBMARINE	SubMarine[MAX_SUBNUM];		//��Ǳͧ����
//static HBITMAP		hDirLeftSub,hDirRightSub;	//����Ǳͧ���
static HBITMAP		hBmpDepthSub;				//Ǳͧͼ���
static HBITMAP		hBmpGameBack;				//��Ϸ����ͼ
static HBITMAP		hBmpGameScoreText;				//��Ϸ��������
static DEPTHBOMB	DepthBomb[DEPTHBOMB_NUM];	//��ը������
static DEPTHBOMB	PlaneBomb[PLANE_BOMB_NUM];	//�ܷɻ�ը������
//static HBITMAP		hDepthBomb;					//�����ը��ͼ���
static HBITMAP		hShipBombAndLife;			//����ը�������ľ��
//static HBITMAP		hAvailableBomb;				//���õ�ը��ͼ���
//static HBITMAP		hLSubDying[DIEFRAMENUM];	//����Ǳͧը�������������
//static HBITMAP		hRSubDying[DIEFRAMENUM];	//����Ǳͧը�������������
static int			nScore = 0;					//�÷�
static int			nOldScore = 0;				//ǰһ�εĵ÷�
static SUBPRIZE		SubPrize[SUBPRIZE_NUM];		//�ܵĽ���������
//static HBITMAP		hSubPrize[SUBPRIZE_NUM];	//����ͼ�ľ��
static HBITMAP		hBmpPrizeAndBomb;			//������ը�����
static TORPEDO		Torpedo[MAX_TORPEDO_NUM];	//�ܵ���������
//static HBITMAP		hTorpedo,hMissil;			//���׺͵����ľ��
static HBITMAP		hBmpTorpedo;				//���ף����֣��ľ��
//static HBITMAP		hShipLive;					//������ͼƬ���
static int			nNoEnemyTimeLeft;			//ʣ���޵�ʱ��
static int			nMinTimeLeft;				//ʣ�ബ��Сʱ��
static HBITMAP		hBmpMinGunShip;				//��С�Ĵ���ͼƬ���
//static HBITMAP		bBmpNoEnemy;				//�޵б�־ͼ�ľ��
static BOMBHERO		BombHero[BOMB_HERO_NUM];	//Ӣ������
static int			nBombNum;					//ը����
static BOOL			bPrizePermit;				//�Ƿ������н���
//static	InputEdit InputEditNode;
static	RECT	TitleRect={10,40,170,65};
static	RECT	NameEditRect={15,90,160,120};	//�����ؼ�λ��
static	HWND		hWndHelp;					//�����ؼ����
static HBITMAP	hBmpCovSel = NULL;				//ѡ��ͼ��
static SUBPLANE	SubPlane;						//�ɻ�
static HBITMAP	hBmpPlane = NULL;				//�ɻ����
static BOOL	bGameOver = FALSE;					//��Ϸ�Ƿ����
static	HMENU		hMainMenu	=	NULL;

/**********************************************
*
*	�˵��ؼ�ID
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
*	�ؼ�ID
*
***********************************************/
#define	IDC_SPINMAX		1//WM_USER+2001
#define	IDC_BTN_PRIZE	2//WM_USER+2002
#define	IDC_HERONAME	3//WM_USER+2003

/**********************************************
*
*	�Զ�����Ϣ
*
***********************************************/
#define	IDRM_RMSG		WM_USER+1010

/**********************************************
*
*	�˵�ģ��
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
