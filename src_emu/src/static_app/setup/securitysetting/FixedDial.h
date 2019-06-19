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
#ifndef FIXEDDAIL_H
#define FIXEDDAIL_H 

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

#define IDC_FIXEDDIAL_QUIT   301
#define IDC_FIXEDDIALSET     302
#define IDC_FIXEDLIST        303

#define IDC_FDSET_QUIT		 304
#define IDC_FDSET_LIST		 305

#define IDC_FIXEDLIST_QUIT  306
#define IDC_FIXEDDIAL_LIST  307
#define IDM_EDIT            308
#define IDM_DELETE          309

#define IDC_NAME			 310
#define IDC_NUMBER			 311
#define IDC_NEWNUMBER_SAVE	 312
#define IDC_NEWNUMBER_QUIT	 313

#define CALLBACK_FDSET       WM_USER + 601  //set fixed dialing
#define DELETENODE_MSG		 WM_USER + 700    //delete number confirm msg
#define ADDNODE_MSG          WM_USER + 701  //add new number(refrash view)
#define EDITNODE_MSG         WM_USER + 702//edit number
#define CALLBACK_WPB         WM_USER + 703 //add new number(me)
#define CALLBACK_DPB         WM_USER + 704 //delete number
#define GETFDPBOOKMEM        WM_USER + 705 //ger fixed dial memory info
#define READFDPBOOK			 WM_USER + 706 //read FD pbook
#define DELETENODE           WM_USER + 707 //delete number(me)
#define REQUEST_CODE         WM_USER + 708
#define ADDNUMBER_FAIL       WM_USER + 709
#define NOMEMORY             WM_USER+710

#define  IDS_EDIT            "Edit"
#define  IDS_DELETE          "Delete"
#define  IDS_SAVE            "Save"

#define		NAME_MAXLEN				20//21
#define		NUMBER_MAXLEN			41
#define     MAX_FDITEM              2

typedef struct tagFN_Item
{
	char Name[NAME_MAXLEN +1];
	char Number[NUMBER_MAXLEN + 1];
	int  id;
	struct tagFN_Item *pNext; 
}FN_Item, *PFN_Item;

static char strPin2[EDITMAXLEN + 1];
static char strPuk2[EDITMAXLEN + 1];
static char strTitle[NAME_MAXLEN + 1];

static   FN_Item *pFN_NodeHead;
static BOOL bNotNotify = FALSE; //no notification
static HWND hFixedDialParentWnd, hFixedDialFocusWnd;
static UINT CallMsgMain;
static BOOL bGivePin2, nGiveCodeType, bGivePuk2; //requrie pin2, puk2
static BOOL bEdit, bDelete, bEditEnable;//edit and delete number
static BOOL bFDStatus = FALSE; //record FD status
static BOOL bSetFD = FALSE;//set fd status by left or right arrow key

static int     iremains; 

static HWND hFDSetFocusWnd, hFDSetParentWnd;

static HWND hFixedDialListWnd, hFixedDialFocusListWnd;

static HWND hNewNameNumberFocus, hNewNameNumberParentWnd;
static int iEditId, iNewId, iTotalId;
static HBITMAP  hIconNormal, hIconSel;
extern HWND hFrameWin;
extern PINSstatus iPinStatus;

static int  v_nRepeats_FD;
static unsigned int  iRepeatKeyTimerId_FD = 0;
static DWORD  nKeyCode_FD;
static BOOL   bHaveRequestPIN2 = FALSE;

//record pin1 pin2 puk puk2 status(enable or disable, blocked or unblocked)
//Fixed dialing view 
static LRESULT  FixedDialWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL		CreateFixedDialControl(HWND hWnd);
static void 	FD_OnCallback_FDSet(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void		FD_OnCallbackCodeRemain(HWND hWnd);
static void		FD_OnCallbackNewCode(HWND hWnd,WPARAM wParam, LPARAM lParam);
static void		FD_OnPinQuery(HWND hWnd);
static void		FD_OnCallbackCode(HWND hWnd,WPARAM wParam, LPARAM lParam);
static void 	FD_OnKeyDown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void		FD_OnCommand(HWND hWnd, WPARAM wParam , LPARAM lParam);

static void CALLBACK f_FDTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);

//Set fixed dialing view
static BOOL		CreateFixedDialSetWnd(HWND hParentWnd);
static LRESULT	FDSetWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL     FDSet_OnCreate(HWND hWnd, HWND *hList);
static void     FDSet_OnKeydown(HWND hWnd, HWND *hList,UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

//Fixed nr list view
static BOOL		CreateFixedDialListWnd(HWND hParentWnd);
static BOOL		CreateFixedListControl(HWND hWnd, HWND *hList);
static LRESULT	FixedListWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void     FDList_OnCreate(HWND hWnd, HWND *hList);
static void     FDList_OnKeydown(HWND hWnd, HWND *hList,UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void     FDList_OnCallBackCodeRemain(HWND hwnd);
static void		FDList_OnCallBackCode(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void		FDList_OnCommand(HWND hWnd, HWND *hList, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void     FDList_OnDelNode(HWND hWnd, HWND *hList, WPARAM wParam, LPARAM lParam);
static void     FDList_OnAddNode(HWND hWnd, HWND *hList, WPARAM wParam, LPARAM lParam);
static void		FDList_OnEditNode(HWND hWnd, HWND *hList, WPARAM wParam, LPARAM lParam);
static void     FDList_OnCallBackRPBMEM(HWND hWnd,WPARAM wParam);
static void     FDList_OnCallBackRPB(HWND hWnd, HWND * hList, WPARAM wParam);

//New number view
static BOOL		CreateNewNumberWnd(HWND hParenWnd, int id);
static BOOL		CreateNewNumberControl(HWND hWnd);
static LRESULT	NewNumberWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void     FDNewItem_OnCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void		FDNewItem_OnCallBackWPB(HWND hWnd, WPARAM wParam, LPARAM lParam);

BOOL		    CreateFixedDialWnd(HWND hParentWnd, BOOL bFDStatus, UINT Msg);

static void     FNItem_Init(void);
static void     GetFN_NodeCount(int*);
static FN_Item *GetFN_NodeFromId(int nFocus);
static void		FreeFN_NodeData(void);
static void     InitFDData(PPHONEBOOK pPhoneBookData, int count, HWND *hList);
static BOOL     DelteFN_Node(int nFocus);
static BOOL		ChangeFN_Node(int nFocus, char *strNumber);

extern BOOL		CallNewCodeWindow(HWND hwndCall,int itype,char * caption, UINT CallMsg);
#endif
