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
#include    "window.h"
#include    "winpda.h"
#include    "plx_pdaex.h"
#include    "str_public.h"
#include    "str_plx.h"
#include    "setting.h"
#include    "CODE.h"
#include    "me_wnd.h"
#include    "setup.h"
#include	"STDIO.H"
#include    "RcCommon.h"

#define     IDC_BUTTON_QUIT         3
#define     IDC_BUTTON_SET          4

#define     IDC_NEW                 6
#define     IDC_AFFIRM              7

 
#define     NEWCODE_TXT             "Enter new code"
#define     AFFIRMCODE_TXT          "Repeat new code"
#define     REQUEST_CODE            WM_USER+100

#define     CODEMODIFY_PIN1         "PIN1"
#define     CODEMODIFY_PIN2         "PIN2"

#define     PASSWORDMODIFY          "Phone lock\r\nchanged"
//#define     CODEMODIFY_SUCC         "%s is changed!"
//#define     CODEMODIFY_FAIL         "%s changed failly!"

#define     OLDTXT_Y                2
#define     NEWTXT_Y                37
#define     AFFTXT_Y                72

static const char * pClassName = "NewCodeWndClass";
static const int    iControlX  = EDITX;
static const int    iTxtX      = 10;
static HINSTANCE    hInstance;

static int          iMP_Type = NULL;
static UINT         CallBackMsg;
static HWND         hParentWnd, hNewCodeFocusWnd;
//iMP_Type = 1:pin1
//iMP_Type = 2:PIN2

static char         cTitle[50] = "";
extern HWND hFrameWin;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hwnd,HWND * hEditNew,HWND * hEditAffirm);

static  BOOL    ModifyCode(HWND hwnd,HWND hEditNew,HWND hEditAffirm);
static  BOOL    GetSIM_modify(HWND hwnd);
//********************extern function********************
extern  void    GetSecurity_code(char * cpassword);
extern  BOOL    SetSecurity_code(char * cpassword);
//*********************************************************
