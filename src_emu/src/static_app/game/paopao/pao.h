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
*	��������
*
***********************************************/
#define	MENU_ITEM_NUM			4
#define	Pao_Ball_Num			4					//���������
#define	MIN_GUN_DEGREE			30					//ǹ��x�ᷴ������С�н�
#define	MAX_GUN_DEGREE			150					//ǹ��x�ᷴ�������н�
#define	NORMAL_DEGREE			90					//ǹ��x�ᷴ�����ʼ�н�
#define	BALL_STEP				(-RADIUS_BALL)		//���ƶ�����
#define	PI						3.1415926525
#define	D_INTERVAL				3					//ÿ��ת���Ƕ�
#define	BALL_PER_LINE			8					//ÿ������
#define	NORMAL_DEADLINE			9//11					//�������,��������,0Ϊ����
#define	Y_DEADLINE				((NORMAL_DEADLINE+1)*Y_BALL_INTERVAL+Y_GAMEBACK)	//�������е�������
#define	DEL_FRAME_NUM			3					//��ȥ��Ķ���֡��
#define	MOVE_TIMER_ID			1					//���ƶ�TIMER��ID
#define	DEL_TIMER_ID			2					//ɾ����TIMER��ID
#define	PRESS_TIMER_ID			3					//��ʱ����TIMER��ID
#define	GUN_TIMER_ID			4					//ǹ�ƶ�TIMER��ID
#define	DRAGEN_TIMER_ID			5					//������TIMER��ID
#define DOWNWALL_TIMER_ID		6					//ǽ����TIMER��ID
#define MOVE_GUN_ID				7					//ǹ���ƶ�TIMER��ID
#define ROUND_TIMER_ID			8					//��ʾÿһ�صĹ���
#define	MOVE_TIMER_ELAPSE		30					//���ƶ�֡�ĳ���
#define	DEL_TIMER_ELAPSE		30					//ɾ����TIMER�ĳ���
#define	PRESS_TIMER_ELAPSE		4000				//��ʱ����TIMER�ĳ���
//#define	GUN_TIMER_ELAPSE		200					//ǹ�ƶ�֡�ĳ���
#define	DRAGEN_TIMER_ELAPSE		200					//���������
#define	SCORE_PER_BALL			2					//ÿ��÷�
#define	SCORE_PER_DROP			3					//ÿ����һ��÷�
#define	TOP_REC_NUM				5					//Ӣ����
#define HERONAMEFORMAT			"%-8s %5d"		//Ӣ�����������ʽ
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
//���Ͻ�ͼ��
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
*	���ݽṹ
*
***********************************************/
//������ݽṹ
typedef	struct	tagTheBall
{
	int	i;			//����
	int	j;			//����
	POINT	lt;		//���Ͻ�����
	int	color;		//��ɫ,-1��ʾ�򲻴���
}THEBALL;

//�������ݽṹ
typedef	struct	tagDragen
{
#define	LEFT_DRAGEN		0	//��ߵ���
#define	RIGHT_DRAGEN	1	//�ұߵ���
	int	Dir;		//������
	int	i;			//�룬������0��1
	POINT	lt;		//���Ͻ�����
}DRAGEN;

//Ӣ�۰�����ݽṹ
typedef struct	tagPaoHero
{
	int		nScore;
	char	Name[15];
	
}PAOHERO;

/**********************************************
*
*	����˵��
*
***********************************************/
LRESULT PaoMainWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);		//�����ڹ���
LRESULT PaoSetupWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);		//���ô��ڹ���
LRESULT PaoGameWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);		//��Ϸ���ڹ���
LRESULT PaoHeroWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);		//Ӣ�۰������̺���
LRESULT PaoSetHeroWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);	//Ӣ������������̺���
LRESULT PaoGameOverWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);	//�������ڹ���
static	DWORD	PaoSetupProc(HWND hWnd);											//�������ý���
static	BOOL	ReadPaoSetup();
static	BOOL	WritePaoSetup();
static	DWORD	PaoGameProc(HWND hWnd);												//������Ϸ����
static	DWORD	PaoHeroProc(HWND hWnd);												//��ʾӢ�۰����
static	DWORD	SetPaoHeroProc(HWND hWnd);											//��ʾӢ����������
static  DWORD   PaoGameShowProc(HWND hWnd);											//��ʾ��Ϸ˵������
DWORD			PaoGameOverProc();													//����
static	void	OnPaoGamePaint(HDC hdc);
static	void	InitPaoGame(HWND hWnd);												//��ʼ����Ϸ
static	void	InitBall(HWND hWnd);												//��ʼ����
static	int		GetColor();															//������ɫ
static	int		OnPaoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam);				//KEYDOWN��Ϣ�������
static	void	DrawGun(HDC hdc);													//��ǹ��
static	double	DTOR(int Degree);													//�Ƕȱ仡��
static	int		DTOI(double	a);														//�������� 
static	void	InitStaticBall();													//��ʼ����̬��
static	void	DrawStaticBall(HDC hdc);											//����̬��
static	void	ShootBall(HWND	hWnd);												//����
static	void	CalcNewPos();														//������λ��
static	void	GetPosFromOffset(int k);													//��ƫ������λ��
static	BOOL	ShouldMove();														//�Ƿ�Ӧ�ü����ƶ�
static	int		SquarePlus(int i,int j);											//��ƽ����
static	int		Square(int a);														//��ƽ��
static	BOOL	NeighborBall(int i,int j);											//�ж��Ƿ���������
static	BOOL	IsBall(int i,int j);												//�ж��Ƿ�����
static	BOOL	IsDeadPao();														//�ж��Ƿ�����
static	void	EndPaoGame(HWND hWnd);												//һ�ֽ����Ĳ���
static	void	DelPao(int i,int j);												//ɾ������������ͬ��
static	BOOL	CanDropPao(int i,int j);											//�ж��Ƿ������
static	void	DropPao(int i,int j);												//�������
static	BOOL	IsABall(int i,int j);												//�ж��Ƿ�����
static	BOOL	IsSameBall(int i,int j);											//�ж�(i,j)���Ƿ���MoveBall��ɫ��ͬ
static	int		CanDelPao(int i,int j);												//�ж��з�����������ͬ��
static	void	InitPaoHero();														//��ʼ��Ӣ����Ϣ
static	BOOL	ReadPaoHero();														//��Ӣ����Ϣ
static	BOOL	WritePaoHero();														//дӢ����Ϣ
static	void	SortRecord();														//����Ӣ������
//static	int		OnPaoChar(HWND hWnd,WPARAM wParam,LPARAM lParam);				//����WM_CHAR��Ϣ
//static	BOOL	ReadMapNum();													//����ͼ��
static	void	InitMapNum();														//��ʼ����ͼ��
//static	BOOL	WriteMapNum();													//д��ͼ��
//static	BOOL	WriteMap();														//д��ͼ
//static	BOOL	ReadMap(int n);
static  BOOL	ReadMapNum(int fp);
static	void	FileReadLine(int fp, char** myString);
static  int     StrToInt ( char * pString);
static	BOOL	ReadMapNew(int n);													//����ͼ
static	void	UncheckBall();														//��Check����ΪFALSE
static	BOOL	IsVoid(int i,int j);												//�Ƿ��ǿ�
static	void	AdjustBall();														//����ͣ��λ��
static	BOOL	IsSetOver();														//һ�ֽ������ж�
static	void	OnDelTimer(HWND hWnd);												//��ȥ��Ĵ���
static	void	OnGunMove(HWND hWnd);												//ǹ�ƶ��Ĵ���
static	void	InitDragenI();														//��ʼ�����ı���i
static	void	CalcDragenLT();														//��������λ��
static	void	ChangeDragenI();													//�ı����Ķ���
static	BOOL	MyDrawMenuItem(HDC hdc,int i,int nSel);								//���˵���
static	BOOL	ShowArrowState(HWND hWnd,int nSel);									//�������ͷ��ʾ
static	int	    GetAreaByPos(int x,int y);
static	void	ShowArrowInGameState(HWND hWnd);
static	void	ShowRoundNum();											//�õ�������
static	void	InitSelItem();
/**********************************************
*
*	ȫ�ֱ���
*
***********************************************/
static	HWND	hWnd_PaoMain = NULL;					//�����ھ��
static	HWND	hWndPaoGame = NULL;						//��Ϸ���ھ��
static	HWND	hWndPaoHero = NULL;						//Ӣ�۰񴰿ھ��
static	HWND	hPaoHeroName = NULL;					//Edit���
static	HWND	hPaoGameOver = NULL;					//��Ϸ����
static	HMENU	hMainMenu = NULL;						//���˵�
static	HWND	hWnd_SetPress;							//�Ƿ��½�BTN���
static	HWND	hWnd_SetMove;							//�Ƿ��ƶ�BTN���
static	HWND	hWndPaoSetup = NULL;					//���ô��ھ��
static	BOOL	bSetupPress = FALSE;					//�Ƿ�ʱ���
//static	BOOL	bSetupMove = FALSE;						//�Ƿ��ƶ�
static	BOOL	bGameStart = FALSE;						//��Ϸ��ʼ
static	HBITMAP	hbmpBall[9];
static	RECT	GameFrameRect = {X_GAMEBACK-1,Y_GAMEBACK-1,
	X_GAMEBACK+GAMEBACK_WIDTH+1,Y_GAMEBACK+GAMEBACK_HEIGHT+1};			//��Ϸ��ܾ���
static	RECT	GameBackRect = {X_GAMEBACK,Y_GAMEBACK,
				X_GAMEBACK+GAMEBACK_WIDTH-1,Y_GAMEBACK+GAMEBACK_HEIGHT-1};			//��Ϸ��������
static	HBITMAP	hPaoBack = NULL;						//����ͼ���
static	HBITMAP	hPaoGun = NULL;							//ǹͼ�����
static	HBITMAP	hPaoWall = NULL;						//�����ǽ�ľ��		
static	HBITMAP	hDragen[2][2];							//�������ľ��
static	int		nGunOffset = -1;						//ǹƫ����
static	int		nScore = 0;								//����
static	int		Ball_x_Offset = -1;						//��ǰ��xƫ����
static	int		Ball_y_Offset = -1;						//��ǰ��yƫ����
static	double	Plus_x = 0;								//ˮƽ���������
static	double	Plus_y = 0;								//��ֱ���������
static	int		Gun_Degree	= NORMAL_DEGREE;			//ǹ��x�ᷴ����н�
static	int		Move_Degree	= NORMAL_DEGREE;			//���ƶ�������x�ᷴ����н�	
static	THEBALL	StaticBall[NORMAL_DEADLINE+1][BALL_PER_LINE];			//��ֹ��
static	BOOL	bChecked[NORMAL_DEADLINE+1][BALL_PER_LINE];				//����־
static	THEBALL	MoveBall;								//�˶���
static  THEBALL ToMoveBall;								//��һ�����˶���
static	int		DeadLine = 	NORMAL_DEADLINE;			//��������
static	int		nDelBall = 0;							//��ȥ������
static	int		nDropBall = 0;							//������������
static	PAOHERO	PaoRecord[TOP_REC_NUM];					//Ӣ�ۼ�¼
static	RECT	TitleRect={10,40,170,65};
static	RECT	NameEditRect={15,90,160,120};
//static	InputEdit InputEditNode;
static	char	CurChar;								//��¼�ؼƵ�ǰchar
static	int		nMapNum;								//��ͼ��
static	int		nCurMap;								//��ǰ��ͼ,0Ϊ����,-1��ʾ�޵�ͼ
static	int		nCurFrame = 0;							//��ǰ֡��1
//static	int		nSecFrame = 0;						//��ǰ֡��2
static	BOOL	bFirst = FALSE;							//��һ�ν�����Ϸ
static	HINSTANCE	hPaoInstance;						//ʵ�����
static	DRAGEN	Dragen;									//������
static	BOOL	bDragenTimer = FALSE;					//������Timer����
static  HBITMAP	hCoverBg;
static	HBITMAP hCovSel;								//�α�
static  RECT	SELRECT[MENU_ITEM_NUM];
static  int     nSelMenuItem = 0;						//ѡ�в˵���0--2
static  char    MenuItemText[MENU_ITEM_NUM][9];
static  HWND    hWndHelp;
static  const	int   Interval_Wall = 25000;			//ǽ����ٶ�
static  const	int	  Interval_Gun = 200;				//ǹ���ƶ����ٶ�
static  const	int	  Interval_Round = 1500;			//ÿһ�صĹ�������Ϸǰ��ʾ������
static  int     DropLine = 0;							//ǽ���������
static  BOOL	bGamePause = FALSE;						//���㲻����Ϸ����
static  BOOL	bLeftDown,bRightDown;					//�Ƿ������Ҽ�
static  char	LastKeyDown = 'L';
static  HBITMAP	hRoundBack;								//��ʾ�����ı���λͼ�ľ��
static  HBITMAP	hRoundNum;								//��ʾ����������λͼ�ľ��	
static  BOOL	bRoundBN = FALSE;						//�Ƿ����ʾ����
static  BOOL	bDownWallTID;							//���ֶ�ʱ���Ƿ�����
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
static	HBITMAP	hPaoScore = NULL;						//�÷־��		

/**********************************************
*
*	�˵��ؼ�ID
*
***********************************************/
#define	IDM_NEWGAME				WM_USER+1001			//����Ϸ
#define	IDM_SETUP				WM_USER+1002			//����
#define	IDM_HEROBOARD			WM_USER+1003			//Ӣ�۰�
#define	IDM_EXIT				WM_USER+1004			//�˳�
#define	IDM_CONFIRM				WM_USER+1005			//ȷ��
#define	IDM_GAMESTART			WM_USER+1006			//��Ϸ��ʼ
#define	ID_OVER_RETURN			WM_USER+1007			//ȷ��
#define IDM_GAMESHOW			WM_USER+1008			//��Ϸ˵��

#define	IDRM_RMSG				WM_USER+1009
#define	IDRM_GAMEOVER			WM_USER+1010
/**********************************************
*
*	�ؼ�ID
*
***********************************************/
#define	IDC_SETPRESS			1//WM_USER+2001			//�Ƿ��½�BTN��ID
#define	IDC_SETMOVE				2//WM_USER+2002			//�Ƿ��ƶ�BTN��ID
#define	IDC_HERONAME			3//WM_USER+2003			//Ӣ������EDIT��ID

/**********************************************
*
*	�Զ�����Ϣ
*
***********************************************/


/**********************************************
*
*	�˵�ģ��
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
