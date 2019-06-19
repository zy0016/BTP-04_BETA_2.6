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
//#include "hpimage.h"
#include "string.h"
#include "plx_pdaex.h"
#include "str_plx.h"
#include "mullang.h"


#define	IDS_ERR_OVERFLOW		ML("Out of range!")
#define	IDS_ERR_DIV0			ML("Divisor cannot be zero.")
#define IDS_EQU					ML("Equal")
#define IDS_EXIT				ML("Exit")
#define IDS_APPNAME				ML("Calculator")


#ifdef _EMULATE_
#define SOUND	MessageBeep(0)     //Beep(349, 50)
#else
#define SOUND	{   \
                    WORD SoundData = 349;  \
                    WORD SoundTime = 50;  \
	    	        PlayBack(&SoundData, &SoundTime, 1); \
                }
#endif


#define UP_DIRECTION    1
#define DOWN_DIRECTION  2
#define LEFT_DIRECTION  3
#define RIGHT_DIRECTION 4

#define ID_EXIT		WM_USER +200  
#define ID_EQU		WM_USER +201
#define KEY_0		0
#define KEY_1		1
#define KEY_2		2
#define KEY_3		3 
#define KEY_4		4
#define KEY_5		5
#define KEY_6		6
#define KEY_7		7
#define KEY_8		8
#define KEY_9		9
#define KEY_DOT		10
#define KEY_MADD	11
#define KEY_EQU		22


//void ShowCalcFace(HDC hdc);
BOOL ShowResult(HDC hdc);
void Calc_ChangeFocus(HWND hWnd,int nDirection);
//void Calc_OnGetFocus();
//void Calc_OnLoseFocus();
POINT MakePointInRect();
void ShowOprationSign(HDC hdc, int nSign);

LRESULT CalcMainWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
BOOL Calc_OnCreate(HWND hWnd);
//void Calc_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
void Calc_OnDestroy(HWND hWnd);
void Calc_OnPaint(HWND hWnd);
void Calc_OnPenDown(HWND hWnd, int x, int y);
//void Calc_OnPenUp(HWND hWnd);
void Calc_OnChar(HWND hWnd, char c);
void Calc_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags);
void Calc_InitValiable();
void Calc_ConfirmInput(int i);

extern int Calc_Input(int op);
extern BOOL Calc_GetResult(char* buf);
extern void InitMachine(void);



#define ASSIST_CALC_ICON		"ROM:/toolbox.ico"

#define BTN_COUNT 12

RECT ShowBarRect = {2, 2, 173, 26};
RECT BtnsRect = {2, 26, 175,150};
RECT SignRect0 = {3, 3, 10, 13};
RECT SignRect1 = {3, 14, 10, 25};

RECT g_btns[BTN_COUNT] = {
	{59,  117,  115,  146 }, //	0/¡Â
	{2, 27, 58, 56 },    //	1/¡û
	{59, 27, 115, 56 },   //	2/+
	{116,  27, 172, 56 }, //	3/MC
	{2,  57,  58,  86 }, //	4/c
	{59, 57, 115, 86},    //	5/-
	{116,  57, 172, 86}, //		6/MS
	{2,  87,  58, 116}, //		7/CE
	{59,  87,  115,  116 },//	8/*
	{116,  87,  172,  116 }, //	9/MR
	{2,  117,  58, 146 }, //	.
	{116,  117,  172,  146 }, //	M+
};
char *pSign[BTN_COUNT] =
{
	"0/¡Â",
	"1/<-",
	"2/+",
	"3/MC",
	"4/c",
	"5/-",
	"6/MS",
	"7/CE",
	"8/*",
	"9/MR",
	".",
	"M+",
};

typedef struct tagPostion
{
	int up;
	int down;
	int left;
	int right;
}Postion;
Postion FocusPostion[BTN_COUNT] =
{
	{8, 2, 10, 11 }, //	0/¡Â
	{10, 4, 3,2},//		1/¡û
	{0, 5, 1, 3},//		2/+
	{11, 6, 2, 1},//	3/MC
	{1, 7, 6, 5},//		4/c
	{2, 8, 4, 6},//		5/-
	{3, 9, 5, 4},//		6/MS
	{4, 10, 9, 8}, //	7/CE
	{5, 0, 7, 9 }, //	8/¡Á
	{6, 11, 8, 7 }, //	9/MR
	{7, 1, 11, 0 }, //	.
	{9,  3, 0, 10}, //	M+
};

#define RES_X	        3    
#define RES_Y	        10   
#define RES_CX	        170  
#define RES_CY	        16 

#define RES_NUMW        7
#define RES_DSP         165 

#define RES_SRC_X       3  
#define RES_SRC_Y       4   
#define RES_SRC_CX	    170
#define RES_SRC_CY	    16   
#define RES_SRC_NUMW	7
#define RES_SRC_NUMH	13

#define BG_PATH			"/rom/pim/calc_bkgd.gif"	
#define NUM_PATH		"/rom/pim/calc_num.gif"
#define M_PATH			"/rom/pim/m.bmp"
#define ADD_PATH		"/rom/pim/sign_add.bmp"
#define SUB_PATH		"/rom/pim/sign_sub.bmp"
#define MULTIPLY_PATH	"/rom/pim/sign_multiply.bmp"
#define DIVIDE_PATH		"/rom/pim/sign_divide.bmp"

PCSTR  pBmpPath[BTN_COUNT] =
{
	"/rom/pim/divide.bmp",//"0/¡Â"
	"/rom/pim/backspace.bmp",//"1/<-",
	"/rom/pim/plus.bmp",//hPlus,//"2/+",
	"/rom/pim/mclear.bmp",//"3/MC",
	"/rom/pim/clear.bmp",//"4/c",
	"/rom/pim/substract.bmp",//"5/-",
	"/rom/pim/mstore.bmp",//"6/MS",
	"/rom/pim/ce.bmp",//"7/CE",
	"/rom/pim/multiply.bmp",//"8/ *",
	"/rom/pim/mretrieve.bmp",//"9/MR",
	"/rom/pim/dot.bmp",//".",
	"/rom/pim/mplus.bmp",//"M+",	
};
/*
#define BACKSPACE_PATH   
#define PLUS_PATH		
#define MCLEAR_PATH		
#define CLEAR_PATH		
#define SUBTRACT_PATH	
#define MSTORE_PATH		
#define CE_PATH			
#define MULTIPLY_PATH	
#define MRETRIEVE_PATH	
#define DOT_PATH		
#define DIVIDE_PATH		
#define MPLUS_PATH	*/
	



HBITMAP hBackGround = NULL;   
HBITMAP hDispNum    = NULL;   
