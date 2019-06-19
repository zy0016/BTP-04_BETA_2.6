#ifndef _SMENU_H
#define _SMENU_H

#include "progman.h"

#include "string.h"
#include "malloc.h"

#include "plx_pdaex.h"

#include "AppDirs.h"
#include "AppFile.h"
#include "callpub.h"
#include "setup.h"
#include "menu.h"


#define  BACKUPBITMAP

#define  ITEMHIG  50
#define  ITEMWIN  57
#define  ICONHIG  28
#define  ICONWIN  43
#define  TEXTHIN  13
#define  TEXTWIN  53

#define MAX_KEYNUM  4//deal with arrow key

typedef	struct	tagFileListNode{	
    short	  dwStyle;
    short	  nID;           //app ID
    PAPPNODE pNode;
    struct tagFileListNode *pPrev;
    struct tagFileListNode *pNext;	
} FILELISTNODE, *PFILELISTNODE;

static  PFILELISTNODE pFocusNode, pDownNode; //current app

#define IDS_OPEN  ML("Open")
#define IDS_MOVE  ML("Move")
#define IDS_MOVEFOLDER  ML("Move to folder")
#define IDS_RENAME     ML("Rename")
#define IDS_DELETE     ML("Delete")

#define IDM_OPEN  201
#define IDM_MOVE  202

#define IDM_MOVEFOLDER 203
#define IDM_FOLDER     204
#define IDM_RENAME		205
#define IDM_DELETE		206

#define  PMRENAME_IME_MSG  WM_USER + 1000
#define  PM_DELETEDIR      WM_USER + 1002


static    int  nFocus = 0;    //focus
static    int  nFiles = 0;    // how many app in group


static PFOLDERLINK pGrpHead = NULL;


static    RECT    rcShort; 
static    HDC     hdcMem;

static    HMENU    hMenu, hSubMenu;

static  int  nSrcFocus;
static  BOOL bMoveReady = FALSE;
static short CurappId, CurDirId, SrcappId;

static int  imove = 0; 
static int  iMax,iPos,iPage,nPage,iWinH,iLine,ioldPos;
static SCROLLINFO scinfo;
static HWND hParantWnd;
static PAPPNODE pCurAppNode;
static HWND  hRenamefolderFocus = NULL;

static int  v_nRepeats;
static PKEYEVENTDATA pCurKeydata;
static unsigned int  iRepeatKeyTimerId = 0;
static HWND  hAppWnd;
extern  BOOL CreateKeylockWnd(HWND hWnd);

extern  PAPPADM		pAppMain;
extern  int         nScreenSave;
extern HWND hFrameWin;

static	LRESULT	 ShowApp_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                 LPARAM lParam);
static  PAPPNODE  GetAppFile( PDIRSLINK pDir, int id);
static  void      GetCurrDir(int nCurr);

static BOOL  DirsAppRename(HWND hParentWnd, short nType);
static BOOL  MenuIsRunApp(struct appMain *pAppMain, short fileID);

static  int     GetAppfileID(int iChoose);
static  BOOL	DrawAppList(HWND hWnd, HDC hdc);
static  BOOL    DrawAppMatrix(HWND hWnd, HDC hdc);
static	BOOL	DrawApp(HWND hWnd, HDC hdc, int nApp, int iChoose);
static	int		GetCurrentApp (void);
static  void	InitVScroll(HWND hWnd);
static  void    LoadGrpMenu(struct appMain * pAdm);
static  void	OnPmGrpVscroll(HWND hwnd, WPARAM wParam);
static BOOL		InsertFolderItem(WORD mId, short nIndex);
static short	GetIdByIndex(short nIndex);
static BOOL		FreeFolderLink(void);

static void PM_MakeHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnArrowKeyDown(HWND hWnd, int nKeyCode);
static void CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);
static BOOL CreateMoveFolderWnd(HWND hWnd);

extern PCURAPPLIST *	AppFileGetListHead( struct appMain * pAdm );


#endif
