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
*	��������
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


//���Ͻ�ͼ��
//#include "hp_icon.h"
#define	DIRICON	            "/rom/game/pushbox/pushbox.ico"


/**********************************************
*
*	���ݽṹ
*
***********************************************/
//(��Ϸ��ȫ�����ƹ���,ʵ�����е��ж�����)
typedef struct tagGAME_STATE//������Ϸ״̬�Ľṹ
{
	BOOL  IsBoxOnMan;           //С��ǰ�Ƿ�������
	BOOL  IsWallOnMan;  	    //С��ǰ�Ƿ���ǽ
	BOOL  IsTerminiOnMan;	    //С��ǰ�Ƿ�ΪĿ�ĵ�
	BOOL  IsAreaOnMan;			//С��ǰ�Ƿ�Ϊ�յ�
	BOOL  IsWallOnBox;    		//����ǰ�Ƿ���ǽ
	BOOL  IsTerminiOnBox;	    //����ǰ�Ƿ�ΪĿ�ĵ�
	BOOL  IsBoxOnBox;			//����ǰ�Ƿ�������
	BOOL  IsAreaOnBox;			//����ǰ�Ƿ�Ϊ�յ�
	BOOL  IsPass;		        //�Ƿ����
	POINT Man;					//��¼С�˵�ǰ���߼�����
	char  pCurManState;			//��¼С�˵�ǰλ���ƶ�ǰ��״̬��
}GAME_STATE;


//(��ź����������������Ϣ)
typedef struct tagListUnit
{
	POINT ManPoint;				//�ƶ�����ǰ����λ��
	POINT TheLastPoint;			//�ƶ�ǰ������λ��
	POINT AndTheLastPoint;		//�ƶ��������λ��
	char TheFirstPointIsWhat;	//�ƶ�����ǰ�˵�λ����ʲô
	char TheSecondPointIsWhat;	//�ƶ�ǰ��������ʲô���棨�ж��ǿյػ���Ŀ�ĵأ�
	char TheThirdPointIsWhat;	//�ƶ����������ʲô���棨�ж��ǿյػ���Ŀ�ĵأ�
	char TheStateManAfter;		//�ƶ�ǰ������λ���ƶ��Ժ�״̬
	struct tagListUnit *pNext;	//ָ��ýṹ��ָ��

}ListUnit;


/**********************************************
*
*	����˵��
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
*	ȫ�ֱ���
*
***********************************************/
static HINSTANCE hInst;						//Ӧ�ý���
static HWND hPushBoxWnd;					//��Ϸ�����ھ��
static HWND hPushBoxGameWnd;				//��Ϸ���ھ��
static HWND hPushBoxOptionSetupWnd;			//���ô��ھ��
static HWND hLevelCombo = NULL;				//���ô�����Ŀؼ����
static HWND hPushBoxSuccessWnd;				//���ش��ھ��
static HWND hPushBoxEndGameWnd;				//�ص״��ھ��
static HWND	hWndHelp;						//�������ھ��
//static HMENU hMenu;							//��Ϸ�����µĲ˵�

static BOOL LevelSetupCreate = FALSE;		//�Ƿ��������ô���
static BOOL GameCreate = FALSE;				//�Ƿ�������Ϸ����
static BOOL SuccessCreate = FALSE;			//�Ƿ����ɹ��ش���
static BOOL EndGameCreate = FALSE;			//�Ƿ����ɹص״���
static BOOL ReadSuccess = FALSE;			//��ͼ�ļ��Ƿ��ȡ�ɹ�
static BOOL	bTimerAlready = FALSE;			//�Ƿ��Ѿ�������Timer
static GAME_STATE gamestate;				//��Ϸ���̿��Ƶ����ݽṹ
//static ListUnit *ppList = NULL;				//����ͷ�ڵ�

static int curnLevel = 1;					//��ǰ������ʼ��Ϊ1
static char GameMap[12][12];					//�洢��ͼ�Ķ�λ����
static char* pChar = NULL;					//��ŵ�ͼ���ڴ�ָ��
static char row,line;						//��ͼ��������
//static char TerminiCount;					//Ŀ�ĵصĸ���
static int BackCount = 0;					//���˵Ĳ���
static POINT MyPen;							//�ʵ�����
static short ShortPath[9*12];				//��¼����㵽�յ�����·����������ڿɵ����·����
static short Distance;						//��¼���·���Ľڵ����
static GAME_STATE gamestate;				//��Ϸ���̿��Ƶ����ݽṹ
static ListUnit *ppList = NULL;				//����ͷ�ڵ�
static ListUnit pList;						//�̻������Ӳ����Ľڵ�ṹ��Ϣ
static int nSelMenuItem = 0;				//ѡ�в˵���0--2

static HBITMAP hPushBoxCover = NULL;		//ͼƬ������
static HBITMAP hPushBoxbg = NULL;			//����ͼ
static HBITMAP hPushBoxArea = NULL;			//��Ϸ�����еĿյ�
static HBITMAP hPushBoxBox = NULL;			//��Ϸ�����е�����
static HBITMAP hPushBoxWall = NULL;			//��Ϸ�����е�ǽ
static HBITMAP hPushBoxMan1 = NULL;			//��Ϸ�����е�С��
//static HBITMAP hPushBoxMan2 = NULL;			//��Ϸ�����е�С��
static HBITMAP hPushBoxTemini = NULL;		//��Ϸ�����е�Ŀ�ĵ�
//static HBITMAP hPushBoxCoverWords = NULL;	//�����е�����
//static HBITMAP hPushBoxCoverMan = NULL;		//�����е�С��
static HBITMAP hPushBoxPassWords = NULL;	//���ؽ����е�����
static HBITMAP hPushBoxEndGameWords = NULL;	//�ص׽����е�����

static	HMENU	hPushBoxMenu = NULL;
static COLORREF clrCoverMan;                 
static COLORREF clrCover;                   //����ͼ
static COLORREF clr1;						//͸��ɫ����С��
static COLORREF clr2;						//���ڹ�������
static COLORREF clr3;						//���ڹص�����
static COLORREF clr4;						//���ڷ�������
static BOOL	bclarity;						//�Ƿ�����͸��ɫ

static BOOL	bRSetupCLA = FALSE;
static BOOL	bRGameCLA = FALSE;
static BOOL	bRSuccessCLA = FALSE;
static BOOL	bREndgameCLA = FALSE;

//static int	MoveNum = NULL;					//ÿһ����Ϸ��С�˵��ƶ�����
static char LevelString[LEVEL_COUNT][10]=
{
	"��һ��",
	"�ڶ���",
	"������",
	"���ľ�",
	"�����",
	"������",
	"���߾�",
	"�ڰ˾�",
	"�ھž�",
	"��ʮ��",/*
	"��ʮһ��",
	"��ʮ����",
	"��ʮ����",
	"��ʮ�ľ�",
	"��ʮ���",
	"��ʮ����",
	"��ʮ�߾�",
	"��ʮ�˾�",
	"��ʮ�ž�",
	"�ڶ�ʮ��",
	*/
};
/**********************************************
*
*	�˵��ؼ�ID
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
*	�ؼ�ID
*
***********************************************/
#define	TIMER_IDM			1


//#define ID_MENU_HERO		(WM_USER+5)


/**********************************************
*
*	�˵�ģ��
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
