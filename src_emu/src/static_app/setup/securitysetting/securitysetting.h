 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Setup - security setting		
 *
 * Purpose  : 
 *
\**************************************************************************/
#ifndef SECURITYSETTING_H
#define SECURITYSETTING_H 

#include    "winpda.h"
#include    "plx_pdaex.h"
#include	"sys/types.h"
#include	"sys/stat.h"
#include	"fcntl.h"
#include	"unistd.h" 
#include	"stdlib.h"
#include    "stdio.h"

#include    "code.h"
#include    "me_wnd.h"
#include    "pubapp.h"
#include    "setup.h"
#include	"setting.h"
#include    "OptionSelect.h"
#include	"imesys.h"
#include	"mullang.h"

#include	"pmi.h"
#include	"plxdebug.h"

#define     IDC_SECURITY_QUIT       6
#define     IDC_SECURITY_SET        7
#define     IDC_SECURITY_LIST       8

#define     IDC_LISTITEM0           9
#define     IDC_LISTITEM1           10
#define     IDC_LISTITEM2           11
#define     IDC_LISTITEM3           12

#define     IDC_MAINSECURITY_QUIT   13
#define     IDC_BUTTON_SET			14

#define		IDC_PINQUERY			201
#define		IDC_PIN					202
#define		IDC_PIN2				203
#define     IDC_PHONELOCK           204


#define		IDC_SIMLOCK				207
#define		IDC_FIXEDDIAL			208

#define     IDC_CODELOCK            220

#define		IDC_SIMLOCK_QUIT		501
#define		IDC_SIMLOCK_LIST		502


#define     CALLBACK_STATUS         WM_USER +106
#define     CALLBACK_FNSTATUS       WM_USER + 109
#define     REQUEST_CODE            WM_USER + 110
#define     CODE_BLOCK				WM_USER + 111
#define		WM_SIMREAD				WM_USER + 112
#define		WM_READFILE				WM_USER + 113
 
#define		MAX_ITEMNUM				5
  
		 PINSstatus iPinStatus;
static   BOOL bNotNotify = FALSE;// reqire notification or not
static   BOOL bReadStatus = FALSE, bPin1Active = FALSE;
static   BOOL bQueryPin = FALSE; 
static   BOOL bQueryPin2 = FALSE;
static   BOOL bQueryPuk = FALSE;
static   BOOL bQueryPuk2 = FALSE;
static   BOOL bQuerySimLockCode = FALSE;//require code flag
static	 BOOL bQueryFNStatus = FALSE;//FD status  require 
static   BOOL bFDStatus = FALSE;//record FD status
static   BOOL bFDAllowded = FALSE;
static   int  Codes_iremains = 0;
static   int  Contentlen, Contentlen_Lan;
static   BOOL bReadFileType = FALSE, bReadFileType_lan =FALSE;
static   SimFunctions SimFun;//record sim status

BOOL bModifyCode;

static   int  nQueryCodeType; //require code type
static char  strPin1[EDITMAXLEN+1];
static char  strPin2[EDITMAXLEN+1];
static char  strPuk[EDITMAXLEN+1];
static char  strPuk2[EDITMAXLEN+1];

static const char         * pClassName = "SecurityWndClass";
static const char         * pCodeClassName = "CodesWndClass";
static HWND hFocuswnd, hSetFocuswnd;
static int					nCurFocus; 
static BOOL    bPIN1_state; //pin code request status 

static HBITMAP  hIconNormal, hIconSel;

static HWND hSimlockFocusWnd, hSimlockParentWnd;
static UINT callsimlockmsg;
HWND  hFrameWin ;//frame window handle

static int  v_nRepeats;
static unsigned int  iRepeatKeyTimerId = 0;
static DWORD  nKeyCode;

static int  v_nRepeats_ST;
static unsigned int  iRepeatKeyTimerId_ST = 0;
static DWORD  nKeyCode_ST;

static void CALLBACK f_STTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);

static void CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);

//security settings view
	   BOOL		CreateSecuritysettingWnd(HWND hParentwnd );
static LRESULT  AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void		SecuritySet_InitVScroll(HWND hWnd);
static void		SecuritySet_OnVScroll(HWND hWnd, UINT code);
static BOOL		OnCreate(HWND hWnd);
static void		OnCallBack_FnStatus(HWND hWnd, WPARAM wParam);
static void		OnCallBack_Status(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void 	OnCallBack_SimRead(HWND hWnd);
static void 	OnCallBack_ReadFile(HWND hWnd);
static void		OnKeydown(HWND hWnd,UINT wMsgCmd,WPARAM wParam, LPARAM lParam);
static void     OnCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

//codes and locks view
static BOOL		CreateCodesandLocksWnd(HWND hParentwnd );
static LRESULT  CodesandLocksWndProc ( HWND hWnd, UINT wMsgCmd, 
									  WPARAM wParam, LPARAM lParam );
static void		Codes_OnCreate(HWND hWnd);
static void		Codes_OnActivate(HWND hWnd,WPARAM wParam);
static void     Codes_OnKeydown(HWND hWnd,UINT wMsgCmd,WPARAM wParam, LPARAM lParam);
static void     Codes_OnCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void		Codes_OnCallbackNewCode(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void     Codes_OnCallbackCode(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void     Codes_OnCallbackRemains(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void		Codes_OnPinQuery(HWND hWnd);

//Lock if sim changes view
static BOOL		CreateSimLockWnd(HWND hParentwnd,UINT msg );
static LRESULT	SimlockWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL		SimLock_OnCreate(HWND hWnd, HWND *hList);
static void 	SimLock_OnDestory(void);
static void		SimLock_OnKeydown(HWND hWnd, HWND *hList,UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void		SimLock_OnCommand(HWND hWnd, WPARAM wParam);

static void     ST_ParseStatus(unsigned char *strCode, int iconter);
static BOOL		SetSecurity_Open(BOOL bAuto); 
	   BOOL     GetSecurity_Open(void);//pin1 code request
	   void     GetSecurity_code(char * cpassword);//phone lock code
	   BOOL     SetSecurity_code(char * cpassword);

static BOOL     SetPS_Open(BOOL bAuto);//simlock status
	   BOOL     GetPS_Open(void);
	   void		SetPSCode(char* pPSCode);
	   BOOL		GetPSCode(char* pPSCode);
	   
extern BOOL		SETUPBUTTON_RegisterClass(void);
extern BOOL     CallPhoneLockWnd(HWND hWnd);//call phone lock
extern BOOL		CallModifyCodeWindow(HWND hwndCall,int itype,char * caption,UINT CallMsg);//change code view
extern BOOL		CallNewCodeWindow(HWND hwndCall,int itype,char * caption, UINT CallMsg);//new code view
extern BOOL		CreateFixedDialWnd(HWND hParentWnd, BOOL bFDStatus, UINT Msg);//call fixed dialing view
extern int		GetSIMState(void); //insert sim card or not
extern void     SetPin2Change(void);//pin2 changed status 

#endif
