#ifndef _FMENU_H
#define _FMENU_H

#include "window.h"
#include "winpda.h"
#include "string.h"
#include "stdio.h"
#include "malloc.h"
#include "stdlib.h"
#include "ctype.h"

#include "plx_pdaex.h"
#include "progman.h"
#include "AppDirs.h"
#include "AppFile.h"
#include "math.h"
#include "hpimage.h"
#include "setting.h"
#include "setup.h"
#include "callpub.h"

#include "sys/types.h"
#include "sys/stat.h"
#include "sys/statfs.h"
#include "fcntl.h"
#include "unistd.h"
#include "menu.h"

#define  ITEMHIG  50
#define  ITEMWIN  57
#define  ICONHIG  28
#define  ICONWIN  43
#define  TEXTHIN  13
#define  TEXTWIN  53

#define  PMRENAME_IME_MSG  WM_USER + 1000
#define  PMNEW_IME_MSG     WM_USER + 1001
#define  PM_DELETEDIR      WM_USER + 1002

#define IDS_OPEN		ML("Open")
#define IDS_MOVE		ML("Move")
#define IDS_MOVEFOLDER  ML("Move to folder")
#define IDS_NEWFOLDER   ML("New folder")
#define IDS_RENAME      ML("Rename")
#define IDS_DELETE      ML("Delete")

#define IDM_OPEN		101
#define IDM_MOVE		102
#define IDM_NEWFOLDER	103
#define IDM_RENAME		104
#define IDM_DELETE		105
#define IDM_MOVEFOLDER	106
#define IDM_FOLDER		107
#define IDM_OPENMOVE    108

#define MAX_KEYNUM  4//deal with arrow key

static PFOLDERLINK pHead = NULL;

static  int     nGroupNum;
static  int     ItemNum;

static  int     nFocus = 4;
static  int     nSrcFocus = 0;

static int  imove = 0; 
static int  iMax,iPos,iPage,nPage,iWinH,iLine,ioldPos;
static SCROLLINFO scinfo;

static BOOL bMoveReady = FALSE;
static  HMENU hMenu, hSubMenu;

static PAPPNODE  pCurAppNode = NULL, pSrcAppNode =NULL;
static PDIRSNODE pCurDirsNode = NULL, pSrcDirsNode = NULL;
static short     nCurType = DIR_TYPE;
static short     nSrcType = DIR_TYPE;

static HWND  hNewfolderFocus = NULL;
static HWND  hRenamefolderFocus = NULL;
static HWND  hGrpWnd;

HWND hFrameWin = NULL;

static int      DirShow(HWND hWnd, HDC hdc); 
static void     OnGrpKeyDown(HWND hWnd,UINT wMsgCmd,WPARAM wParam, LPARAM lParam);
static BOOL     OpenAppFunc(DWORD fileID);
static void     InitVscrollInfo(HWND hWnd);
static void		OnPmVscroll(HWND hwnd, WPARAM wParam);
static BOOL		DirsAppRename(HWND hParentWnd, short nType);
static BOOL     DirsNew(HWND hWnd);
static void     DirsAppMove(HWND hWnd, int nOldFocus);
static BOOL     AppRunHideFile(struct appMain * pAdm);
static BOOL     FindItem(struct appMain *pAppMain, int nFocus);
static BOOL		InsertFolderItem(WORD mId, short nIndex);
static BOOL		DeleteFolderItem(WORD mId);
static WORD  	GetIdByIndex(short nIndex);
static short	GetIndexById(WORD mId);
static BOOL		FreeFolderLink(void);
static void		FreeNameItem(PSTR pDirName);
static int GetFocusType( struct appMain * pAppMain, int nFocus);

static LRESULT	ShowGrp_WndProc(HWND, UINT, WPARAM, LPARAM);
extern int      DeskMenu_ShowApp(struct appMain * pAdm, short CurdirId,HWND hParentWnd);
extern PCURAPPLIST *	AppFileGetListHead( struct appMain * pAdm );

static void		PM_MakeHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void		OnArrowKeyDown(HWND hWnd, int nKeyCode);
static void CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                DWORD dwTime);
static int MenuPM_IsFlashAvailable(int nSize);
static BOOL CreateMoveFolderWnd(HWND hWnd);
static BOOL MenuGetCurFolderRunApp(struct appMain *pAppMain, short dirID);
static BOOL MenuIsRunApp(struct appMain *pAppMain, short fileID);

static int  v_nRepeats;
static PKEYEVENTDATA pCurKeydata;
static HWND hGrpFocus;
static unsigned int  iRepeatKeyTimerId = 0;

#endif
