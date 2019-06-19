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
#ifndef MODIFYPASSWORD_H
#define MODIFYPASSWORD_H

#include	"stdio.H"
#include    "window.h"
#include    "winpda.h"
#include    "plx_pdaex.h"
#include    "me_wnd.h"
#include    "setting.h"
#include    "CODE.h"
#include    "setup.h"
#include     "mullang.h"

#define     IDC_BUTTON_QUIT         3
#define     IDC_BUTTON_SET          4
#define     IDC_OLD                 5
#define     IDC_NEW                 6
#define     IDC_AFFIRM              7

#define     CONTROL_X               0

#define     OLDCODE_TXT             "Enter old code"
#define     NEWCODE_TXT             "Enter new code"
#define     AFFIRMCODE_TXT          "Repeat new code"

#define     OLDPASS_TXT             "Enter old password"
#define		NEWPASS_TXT				"Enter new password"
#define     AFFIRMPASS_TXT          "Repeat new password"

#define     CODEMODIFY_PIN1         "PIN code"
#define     CODEMODIFY_PIN2         "PIN2 code"
#define     CODEMODIFY_LIMIT        "Barring password"

#define     PASSWORDMODIFY          "Phone lock code\r\n changed"
#define     CODEMODIFY_SUCC         "%s\r\n changed!"
#define     CODEMODIFY_FAIL         "Wrong \r\n%s!"

#define   REQUEST_CODE         WM_USER + 100
#define   CODE_NOTMATCH        WM_USER + 101
static const char * pClassName = "ModifyPasswordWndClass";
static HWND  hCallwnd, hModifyCodeFocusWnd;
static UINT  MsgCall;
extern HWND hFrameWin;
extern BOOL bModifyCode;

static int          iMP_Type = 0;
//iMP_Type:  code type
//iMP_Type = 0:phone code
//iMP_Type = 1:pin1
//iMP_Type = 2:PIN2
//iMP_Type = 3:call barring code

static char         cTitle[50] = "";

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hwnd,HWND * hEditOld,HWND * hEditNew,HWND * hEditAffirm);

static  BOOL    ModifyCode(HWND hwnd,HWND hEditOld,HWND hEditNew,HWND hEditAffirm);
static  BOOL    GetSIM_modify(HWND hwnd);
//********************extern function********************
extern  void    GetSecurity_code(char * cpassword);
extern  BOOL    SetSecurity_code(char * cpassword);
extern  void    SetPin2Change(void);//PIN2 changed
extern  BOOL    CallValidPin2(HWND hwndCall);
//*********************************************************

#endif
