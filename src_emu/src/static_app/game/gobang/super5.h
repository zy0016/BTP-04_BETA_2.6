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
#include "plx_pdaex.h"

#if defined SCP_NOKEYBOARD
#define WND_X		0
#define WND_Y		0
#define TOPSPACE	24
#define WND_WIDTH	176
#define WND_HEIGHT	220-TOPSPACE
#else
#define WND_X		0
#define WND_Y		0
#define TOPSPACE	24
#define WND_WIDTH	176
#define WND_HEIGHT	240-TOPSPACE
#endif

#define BOTTOMSPACE	20
#define X_BACK		0
#define Y_BACK		0
#define BACK_BMP_WIDTH	176
#define BACK_BMP_HEIGHT	181
//#endif

//#if defined SCP_NOKEYBOARD
#include	"str_public.h"
#include	"str_plx.h"
#include	<pubapp.h>
//lmx modify
//#define PATH_MENU			GetString(STR_GOBANG_COVMENU)//"ROM:GobMenuC.gif"
//#define PATH_MENU			ML("/rom/game/gobang/gobmenue.gif")
/*
#if defined SCP_NOKEYBOARD
#define COVERBG				"/rom/game/gobang/gobcover.gif"
#define PATH_GOBANGVICTORY	"/rom/game/gobang/gobvict.gif"
#define PATH_GOBANGMAN		"/rom/game/gobang/gobanmen.gif"
#define PATH_GOBANGBOARD	"/rom/game/gobang/gobboard.gif"
#define PATH_GOBANGCHESS	"/rom/game/gobang/gobchess.gif"
#else
#define COVERBG				"/rom/game/gobang/goband_4.gif"
#define PATH_GOBANGVICTORY	"/rom/game/gobang/goband_5.gif"
#define PATH_GOBANGMAN		"/rom/game/gobang/goband_1.gif"
#define PATH_GOBANGBOARD	"/rom/game/gobang/goband_2.gif"
#define PATH_GOBANGCHESS	"/rom/game/gobang/goband_3.gif"
#define TEXTCH              "/rom/game/gobang/goband_6.gif"
#define TEXTEN              "/rom/game/gobang/goband_7.gif"
#endif
*/
#define COVERBG				"/rom/game/gobang/tictactoe.bmp"
#define PATH_GOBANGVICTORY	"/rom/game/gobang/goband_5.gif"
#define PATH_GOBANGMAN		"/rom/game/gobang/goband_1.gif"
#define PATH_GOBANGBOARD	"/rom/game/gobang/goband_2.gif"
#define PATH_GOBANGCHESS	"/rom/game/gobang/goband_3.gif"
//#define TEXTCH              "/rom/game/gobang/goband_6.gif"
//#define TEXTEN              "/rom/game/gobang/goband_7.gif"
void PaintInit(HWND hWnd, int x, int y);
void PaintBoard(HDC hdc);
void PaintChess(HDC hdc, int x, int y, int color);
void PaintDestroy();
int Board_HitTest(int x, int y, int* col, int* row);

#define BOARD_SIZE		15

typedef unsigned short BITBOARD[BOARD_SIZE];

void BB_Init(BITBOARD bb);
void BB_Set(BITBOARD bb, int x, int y);
int BB_Test(BITBOARD bb, int x, int y);
void BB_Clear(BITBOARD bb, int x, int y);
int BB_IsEmpty(BITBOARD bb);
int BB_Match(BITBOARD bb1, BITBOARD bb2);
void BB_Stat(BITBOARD bb, int x, int y, int* stat);
int BB_Fetch(BITBOARD bb, int *x, int *y);
int BB_Match(BITBOARD bb1, BITBOARD bb2);
void BB_And(BITBOARD r, BITBOARD bb1, BITBOARD bb2);
void BB_Or(BITBOARD r, BITBOARD bb1, BITBOARD bb2);
void BB_Not(BITBOARD r, BITBOARD bb);
void UnPaintChess(HDC hdc, int x, int y);

enum 
{
	empty = -1,
	black = 0,
	white = 1
};
/*------------------------------------*/
typedef struct tagCOORDINATEPOINT
{
   int x,y;
}COORDINATEPOINT,*PCOORDINATEPOINT;

/*------------------------------------*/
typedef struct tagGAME5
{
	int winner;
	int side;
	BITBOARD bb[3];
	int steps;
	BYTE histroy[BOARD_SIZE*BOARD_SIZE];//∆Â≈Ã
}GAME5;

#define CHESS_BMP_SIZE	11//13
#define BOARD_BMP_SIZE	165//240
#define	X_CHESS_OFFSET	0
#define	Y_CHESS_OFFSET	-3

static int xBoard, yBoard;
static HBITMAP hBmpBoard, hBmpChess;
GAME5 game5;

void Game5_New();
int Game5_Put(int x, int y);
int Game5_Color(int x, int y);
int Game5_Status();
int Game5_Won();
int Game5_Side();
int Game5_Undo(int steps);
int Game5_RobotPlay(int* x, int* y);
void Game5_RobotBreak();
