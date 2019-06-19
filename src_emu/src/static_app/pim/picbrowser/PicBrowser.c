/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : public
 *
 * Purpose  : 图片预览
 *            
\**************************************************************************/
#define __MODULE__ "PICTURE"
#define	debugbylmx
#include "PicBrowser.h"
#include "plxdebug.h"
#include "sys/stat.h"

#define	RENAMEWCLS	"PICRENAMEWNDCLASS"
#define	MOVETOFOLDER	"MOVETOFOLDERWNDCLASS"
#define	PICICONHEIGHT	40
/*************************************************
	全局变量
*************************************************/
/*
 *	window handle
 */
static	HINSTANCE	hInstance;
static	BOOL	bBrowserImgWndClass = FALSE;

static	HWND	hFrameWnd = NULL;
static	HWND	hMainView = NULL;
static	HWND	hRenameWnd = NULL;
static	HWND	hWndListBox = NULL;
static	HWND	hEditParent	= NULL;

//static	HWND	hMoveToFolder = NULL;
static	HWND	hFolderList = NULL;
static	HWND	hMemDeviceList = NULL;
static	HWND	hWallPaperList	=	NULL;

static	HMENU	hViewMenu = NULL;
HMENU	hSendSelMenu = NULL;
static	HMENU	hSendMenu	=	NULL;
static	HMENU	hSendManyMenu	=	NULL;
static	HMENU	hDelManyMenu	=	NULL;
static	HMENU	hMoveManyMenu	=	NULL;
static	HMENU	hFolderHLMenu	=	NULL;
static	HMENU	hSimplexMenu	=	NULL;
static	HMENU	hMMCHLMenu		=	NULL;

static	HBITMAP	hRBoffbmp = NULL;
static	HBITMAP	hRBonbmp = NULL;
static	BOOL	bUpdated = FALSE;
static	HBITMAP	hPhoneIcon	=	NULL;
static	HBITMAP	hMMCIcon	=	NULL;




typedef	struct tagMOVEDATA
{
	HWND	hRecMsg;
	UINT	rmsg;
	BOOL	bPhonePath;
	char	szCurPath[PREBROW_MAXFULLNAMELEN];
}MOVEDATA,* PMOVEDATA;
/*
 *	viriable
 */
static  char	FolderCaption[PREBROW_MAXFILENAMELEN] = "";
static	char	FolderPath[PREBROW_MAXFULLNAMELEN] = "";
static	char	cFileName[PREBROW_MAXFILENAMELEN] = "";
static	char	pFileFullName[PREBROW_MAXFULLNAMELEN] = "";
//static	char	pWorkPath[PREBROW_MAXFULLNAMELEN] = "";

static	int		nFileType;
static	int		nMainListIndex	=	0;

static	LISTBUF	ListBuffer;


static	HBITMAP	hOpenFolder	=	NULL;
static	HBITMAP	hFolderBmp = NULL;
static	HBITMAP	hSubFolder	=	NULL;
static	HBITMAP	hMemoryBmp = NULL;
HBITMAP	hSpareMiniature	=	NULL;
static	BOOL	bDir;
static	BOOL	bFolderOpen = FALSE;
static	BOOL	bSendAll = FALSE;
static	BOOL	bMMCOpen = FALSE;


/*************************************************
	函数声明
*************************************************/
static	LRESULT	EditNameProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
static	BOOL	PreviewPicture(HWND hWnd, UINT returnmsg, DWORD dwMask);
static	LRESULT CALLBACK BrowserImgWndProc (HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
static	LRESULT CALLBACK MoveToFolderWndProc (HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
static	LRESULT CALLBACK SelectDestinationWndProc (HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
static	LRESULT CALLBACK SetWallpaperProc (HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
int	AddPicturesToList (HWND hList,PCSTR pPath, PLISTBUF pltb, HBITMAP hFolder);
int	AddFileToList(HWND hList, const char* pFilePath, LISTBUF* ListBuffer, HBITMAP hFolderIcon);
static	HBITMAP	ZoomOutBmp(HBITMAP	hBmp, PCSTR pFIle);
int		GetFileNumInDir(PCSTR pPath);
extern	void	LOG_STtoFT(SYSTEMTIME* pSysTime, unsigned long* pTime);

BOOL	IsPicture(PSTR	pFileName, int *nItemData);
static	int	InitMainViewList();
static	int	InitFolderPicList(PCSTR pPath, BOOL bMemoryCard);

//extern BOOL		MMC_CheckCardStatus();
extern char *Mail_UTF8DecodeOnString(char *pSrc, int nSize);

/*
 *	menu template
 */
static MENUITEMTEMPLATE SendMenu[] =
{
	{ MF_STRING, IDC_VIAMMS, "Via MMS", NULL},	
	{ MF_STRING, IDC_VIAEMAIL, "Via Email", NULL},
	{ MF_STRING, IDC_VIABLUETOOTH, "Via Bluetooth", NULL},
	{ MF_END, 0, NULL, NULL }
};

static MENUITEMTEMPLATE SendManyMenu[] =
{
	{ MF_STRING, IDC_SENDSELECT, "Select", NULL},
	{ MF_STRING, IDC_SENDALL, "All in folder", NULL},
	{ MF_END, 0, NULL, NULL }
};

static MENUITEMTEMPLATE DelManyMenu[] =
{
	{ MF_STRING, IDC_DELSELECT, "Select", NULL},
	{ MF_STRING, IDC_DELALL, "All in folder", NULL},
	{ MF_END, 0, NULL, NULL }
};

static MENUITEMTEMPLATE MoveManyMenu[] =
{
	{ MF_STRING, IDC_MOVESELECT, "Select", NULL},
	{ MF_STRING, IDC_MOVEALL, "All in folder", NULL},
	{ MF_END, 0, NULL, NULL }
};


static MENUITEMTEMPLATE CopyManyMenu[] =
{
	{ MF_STRING, IDC_COPYSELECT, "Select", NULL},
	{ MF_STRING, IDC_COPYALL, "All in folder", NULL},
	{ MF_END, 0, NULL, NULL }
};
/*
static MENUITEMTEMPLATE WallPaperMenu[] =
{
	{ MF_STRING, IDC_SET, "Set", NULL},
	{ MF_STRING, IDC_CLEAR, "Clear", NULL},
	{ MF_END, 0, NULL, NULL }                                                                                                  
};
*/

static MENUITEMTEMPLATE MainviewMenu[] =
{
	{ MF_STRING, IDC_OPEN, "Open", NULL},
	{ MF_STRING|MF_POPUP, 0, "Send...", SendMenu },
	{ MF_STRING|MF_POPUP, 0, "Send many...", SendManyMenu},
	{ MF_STRING, IDC_RENAME, "Rename", NULL},
	{ MF_STRING, IDC_CREATEFOLDER, "Create folder", NULL},
	{ MF_STRING, IDC_WALLPAPER, "Wallpaper", NULL},
	{ MF_STRING, IDC_MOVETOFOLDER, "Move to folder", NULL},
	{ MF_STRING|MF_POPUP, 0, "Move many...", MoveManyMenu},
	{ MF_STRING, IDC_COPY, "Copy", NULL},
	{ MF_STRING|MF_POPUP, 0, "Copy many", CopyManyMenu},
	{ MF_STRING, IDC_SHOWDETAIL, "Details", NULL},	
	{ MF_STRING, IDC_DELETE, "Delete", NULL},
	{ MF_STRING|MF_POPUP, 0, "Delete many...", DelManyMenu},
    { MF_END, 0, NULL, NULL }
};


static MENUITEMTEMPLATE SimplexFileMenu[] =
{
	{ MF_STRING, IDC_OPEN, "Open", NULL},
	{ MF_STRING|MF_POPUP, 0, "Send...", SendMenu },
	{ MF_STRING, IDC_RENAME, "Rename", NULL},
	{ MF_STRING, IDC_CREATEFOLDER, "Create folder", NULL},
	{ MF_STRING, IDC_WALLPAPER, "Wallpaper", NULL},
	{ MF_STRING, IDC_MOVETOFOLDER, "Move to folder", NULL},
	{ MF_STRING, IDC_COPY, "Copy", NULL},
	{ MF_STRING, IDC_SHOWDETAIL, "Details", NULL},	
	{ MF_STRING, IDC_DELETE, "Delete", NULL},
    { MF_END, 0, NULL, NULL }
};

static MENUITEMTEMPLATE MMCHighLightMwnu[] =
{
	{ MF_STRING, IDC_OPEN, "Open", NULL},	
	{ MF_STRING, IDC_CREATEFOLDER, "Create folder", NULL},
    { MF_END, 0, NULL, NULL }
};

static MENUITEMTEMPLATE FolderHighLightMenu[] =
{
	{ MF_STRING, IDC_OPEN, "Open", NULL},
	{ MF_STRING, IDC_RENAME, "Rename", NULL},
	{ MF_STRING, IDC_CREATEFOLDER, "Create folder", NULL},
	{ MF_STRING, IDC_DELETE, "Delete", NULL},
    { MF_END, 0, NULL, NULL }
};

static const MENUTEMPLATE MMCHLMenuTemplate =
{
    0,
    MMCHighLightMwnu
};
	
static const MENUTEMPLATE MainViewListMenuTemplate =
{
    0,
    MainviewMenu
};

static const MENUTEMPLATE SimplexMenuTemplate =
{
    0,
    SimplexFileMenu
};

static const MENUTEMPLATE SendMenuTemplate =
{
    0,
    SendMenu
};

static const MENUTEMPLATE FolderHLMenuTemplate =
{
    0,
    FolderHighLightMenu
};
/*********************************************************************
* Function	   EnterTextsBrow
* Purpose      进入图片预浏览界面
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD PICBROWSER_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
	switch(nCode)
	{
	case APP_INIT:
		hInstance = (HINSTANCE)pInstance;
		break;
	case APP_ACTIVE:
		if(!PreviewPicture(NULL, 0, PREBROW_MASKINSERT|PREBROW_MASKEDIT|PREBROW_MASKDEL))
		{
			//printf("failed to create windows\n");
			return 0;
		}
		break;
	case APP_GETOPTION:
		if (wParam == AS_APPWND) {
			return (DWORD)hFrameWnd;
		}
		break;
	case APP_INACTIVE:
		ShowOwnedPopups(hFrameWnd, FALSE);
		ShowWindow(hFrameWnd, SW_HIDE);
		f_sleep_unregister(PICBROWSE);
//		DestroyWindow(hMainView);
//		DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);
		break;
	}
	return 1;
}

BOOL	PreviewPicture(HWND hWnd, UINT returnmsg, DWORD dwMask)
{
    WNDCLASS wc;
	RECT		rc;
	HMENU	hCopyManyMenu = NULL;

//	dwMaskStyle = dwMask;
   
	if (!bBrowserImgWndClass)
	{
		wc.style            = CS_OWNDC;
		wc.lpfnWndProc      = BrowserImgWndProc;
		wc.cbClsExtra       = 0;
		wc.cbWndExtra       = NULL;
		wc.hInstance        = NULL;
		wc.hIcon            = NULL;
		wc.hCursor          = NULL;//LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground    = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName     = NULL;
		wc.lpszClassName    = "BrowserImgWndClass";
		
		if (!RegisterClass(&wc))
			return FALSE;
		
		bBrowserImgWndClass = TRUE;
	}

	if (IsWindow(hMainView))
	{
		f_sleep_register(PICBROWSE);
		ShowWindow(hFrameWnd,SW_SHOW);
		ShowOwnedPopups(hFrameWnd, TRUE);
		UpdateWindow(hFrameWnd);
	}
	else
	{
#ifdef _MODULE_DEBUG_
		StartObjectDebug();
#endif		 
		hFrameWnd = CreateFrameWindow(WS_VISIBLE| WS_CAPTION |PWS_STATICBAR);
		SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)"");
		SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OPEN,1), (LPARAM)"");
		GetClientRect(hFrameWnd, &rc);

		hViewMenu = LoadMenuIndirect(&MainViewListMenuTemplate);
		hSendSelMenu = LoadMenuIndirect(&SendMenuTemplate);
		hFolderHLMenu = LoadMenuIndirect(&FolderHLMenuTemplate);
		hSimplexMenu	= LoadMenuIndirect(&SimplexMenuTemplate);
		hMMCHLMenu	=	LoadMenuIndirect(&MMCHLMenuTemplate);

		//begin initializing menu
		hSendMenu	=	GetSubMenu(hViewMenu, 1);
		hSendManyMenu = GetSubMenu(hViewMenu, 2);
		hDelManyMenu =	GetSubMenu(hViewMenu, 12);
		hMoveManyMenu = GetSubMenu(hViewMenu, 7);
		hCopyManyMenu = GetSubMenu(hViewMenu, 9);
		//hWallPaper = GetSubMenu(hViewMenu,5);

		ModifyMenu(hMMCHLMenu, IDC_OPEN, MF_BYCOMMAND, IDC_OPEN, IDS_OPEN);
		ModifyMenu(hMMCHLMenu, IDC_CREATEFOLDER, MF_BYCOMMAND, IDC_CREATEFOLDER, IDS_CREATEFOLDER);


		ModifyMenu(hFolderHLMenu, IDC_OPEN, MF_BYCOMMAND, IDC_OPEN, IDS_OPEN);
		ModifyMenu(hFolderHLMenu, IDC_DELETE, MF_BYCOMMAND, IDC_DELETE, IDS_DELETE);
		ModifyMenu(hFolderHLMenu, IDC_RENAME, MF_BYCOMMAND, IDC_RENAME, IDS_RENAME);
		ModifyMenu(hFolderHLMenu, IDC_CREATEFOLDER, MF_BYCOMMAND, IDC_CREATEFOLDER, IDS_CREATEFOLDER);

		ModifyMenu(hViewMenu, IDC_OPEN, MF_BYCOMMAND, IDC_OPEN, IDS_OPEN);
		ModifyMenu(hViewMenu, 1, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hSendMenu, IDS_SEND);
		ModifyMenu(hViewMenu, 2, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hSendManyMenu, IDS_SENDMANY);
		ModifyMenu(hViewMenu, IDC_DELETE, MF_BYCOMMAND, IDC_DELETE, IDS_DELETE);
		ModifyMenu(hViewMenu, 12, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hDelManyMenu, IDS_DELETEMANY);
		ModifyMenu(hViewMenu, IDC_RENAME, MF_BYCOMMAND, IDC_RENAME, IDS_RENAME);
		ModifyMenu(hViewMenu, IDC_MOVETOFOLDER, MF_BYCOMMAND, IDC_MOVETOFOLDER, IDS_MOVETOFOLDER);
		ModifyMenu(hViewMenu, 7, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hMoveManyMenu, IDS_MOVEMANY);
		ModifyMenu(hViewMenu, IDC_CREATEFOLDER, MF_BYCOMMAND, IDC_CREATEFOLDER, IDS_CREATEFOLDER);
		ModifyMenu(hViewMenu, IDC_WALLPAPER, MF_BYCOMMAND, IDC_WALLPAPER, IDS_WALLPAPER);
		ModifyMenu(hViewMenu, 9, MF_BYPOSITION|MF_POPUP,(UINT_PTR)hCopyManyMenu, IDS_COPYMANY);
		//ModifyMenu(hViewMenu, 5, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hWallPaper, IDS_WALLPAPER);
		ModifyMenu(hViewMenu, IDC_SHOWDETAIL, MF_BYCOMMAND, IDC_SHOWDETAIL, IDS_SHOWDETAIL);

		ModifyMenu(hSimplexMenu, IDC_OPEN, MF_BYCOMMAND, IDC_OPEN, IDS_OPEN);
		ModifyMenu(hSimplexMenu, 1, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hSendMenu, IDS_SEND);
		ModifyMenu(hSimplexMenu, IDC_DELETE, MF_BYCOMMAND, IDC_DELETE, IDS_DELETE);
		ModifyMenu(hSimplexMenu, IDC_RENAME, MF_BYCOMMAND, IDC_RENAME, IDS_RENAME);
		ModifyMenu(hSimplexMenu, IDC_MOVETOFOLDER, MF_BYCOMMAND, IDC_MOVETOFOLDER, IDS_MOVETOFOLDER);		
		ModifyMenu(hSimplexMenu, IDC_CREATEFOLDER, MF_BYCOMMAND, IDC_CREATEFOLDER, IDS_CREATEFOLDER);
		ModifyMenu(hSimplexMenu, IDC_WALLPAPER, MF_BYCOMMAND, IDC_WALLPAPER, IDS_WALLPAPER);		
		ModifyMenu(hSimplexMenu, IDC_SHOWDETAIL, MF_BYCOMMAND, IDC_SHOWDETAIL, IDS_SHOWDETAIL);


		ModifyMenu(hSendMenu, IDC_VIAMMS, MF_BYCOMMAND, IDC_VIAMMS, IDS_VIAMMS);
		ModifyMenu(hSendMenu, IDC_VIAEMAIL, MF_BYCOMMAND, IDC_VIAEMAIL, IDS_VIAEMAIL);
		ModifyMenu(hSendMenu, IDC_VIABLUETOOTH, MF_BYCOMMAND, IDC_VIABLUETOOTH, IDS_VIABLUETOOTH);

		ModifyMenu(hSendSelMenu, IDC_VIAMMS, MF_BYCOMMAND, IDC_VIAMMS, IDS_VIAMMS);
		ModifyMenu(hSendSelMenu, IDC_VIAEMAIL, MF_BYCOMMAND, IDC_VIAEMAIL, IDS_VIAEMAIL);
		ModifyMenu(hSendSelMenu, IDC_VIABLUETOOTH, MF_BYCOMMAND, IDC_VIABLUETOOTH, IDS_VIABLUETOOTH);

		ModifyMenu(hSendManyMenu, IDC_SENDSELECT, MF_BYCOMMAND, IDC_SENDSELECT, IDS_SELECT);
		ModifyMenu(hSendManyMenu, IDC_SENDALL, MF_BYCOMMAND, IDC_SENDALL, IDS_ALLINFOLDER);

		ModifyMenu(hDelManyMenu, IDC_DELSELECT, MF_BYCOMMAND, IDC_DELSELECT, IDS_SELECT);
		ModifyMenu(hDelManyMenu, IDC_DELALL, MF_BYCOMMAND, IDC_DELALL, IDS_ALLINFOLDER);

		ModifyMenu(hMoveManyMenu, IDC_MOVESELECT, MF_BYCOMMAND, IDC_MOVESELECT, IDS_SELECT);
		ModifyMenu(hMoveManyMenu, IDC_MOVEALL, MF_BYCOMMAND, IDC_MOVEALL, IDS_ALLINFOLDER);
		
		ModifyMenu(hCopyManyMenu, IDC_COPYSELECT, MF_BYCOMMAND, IDC_COPYSELECT, IDS_SELECT);
		ModifyMenu(hCopyManyMenu, IDC_COPYALL, MF_BYCOMMAND, IDC_COPYALL, IDS_ALLINFOLDER);
//		ModifyMenu(hWallPaper, IDC_SET, MF_BYCOMMAND, IDC_SET, IDS_SET);
//		ModifyMenu(hWallPaper, IDC_CLEAR, MF_BYCOMMAND, IDC_CLEAR, IDS_CLEAR);

		PDASetMenu(hFrameWnd, hViewMenu);
		SetWindowText(hFrameWnd, IDS_CAPTION);
		
		hMainView = CreateWindow(
			"BrowserImgWndClass", 
			NULL, 
			WS_VISIBLE|WS_CHILD,
			//PLX_WIN_POSITION,
			rc.left,
			rc.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			hFrameWnd, 
			NULL,
			NULL, 
			NULL
			);

		if (!IsWindow(hMainView))
		{
			return FALSE;
		}
		
		ShowWindow(hFrameWnd,SW_SHOW);
		UpdateWindow(hFrameWnd);
		SetFocus(hMainView);
	}
    
	return TRUE;
}

/*********************************************************************
* Function	   BrowserImgWndProc
* Purpose      浏览图片窗口过程
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK BrowserImgWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
	LRESULT lResult;
	int	index;

	char	szTemp[PREBROW_MAXFILENAMELEN] = "";
	char	curFileName[PREBROW_MAXFULLNAMELEN] = "";
	char	delprompt[PREBROW_MAXFULLNAMELEN+20] = "";
	char	*p;

	int		num, iType, oriIndex;
	//int		nIndex,
	RECT	rClient;
	//DIR		*dir;
	
//	PLISTDATA	pNode;
	struct	stat	filestat;
	int			AvailSpace;
//	float		tmpsize;
//	char	PicSize[18];
	FILE	*fp = NULL;

	lResult = TRUE;
	switch(wMsgCmd)
	{
	case WM_CREATE:
//		SendMessage( hWnd, PWM_SETSCROLLSTATE,
//                SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );
#ifdef _EMULATE_
		mkdir("/mnt/flash/mmc", 0666);
#endif
		f_sleep_register(PICBROWSE);
		hRBoffbmp	=	LoadImage(NULL, RADIO_OFF, IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE);
		hRBonbmp	=	LoadImage(NULL, RADIO_ON, IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE);
		//WaitWin(hWnd, TRUE, IDS_OPENING, IDS_CAPTION, NULL, IDS_CANCEL, IDRM_OPENING);
		GetClientRect(hWnd, &rClient);

		hWndListBox = CreateWindow("PICLISTEX","",
			WS_CHILD |WS_VISIBLE|LBS_HASSTRINGS|WS_VSCROLL|LBS_BITMAP|LBS_MULTILINE|LBS_ICON,
			0, 0, rClient.right, rClient.bottom,
			hWnd,(HMENU)IDC_PREPIC_LIST,NULL,NULL);
		hFolderBmp = LoadImage(NULL, FOLDERPATH, IMAGE_BITMAP, 43, 28, LR_LOADFROMFILE);
		hMemoryBmp = LoadImage(NULL, PICMMCICON, IMAGE_BITMAP, 43, 28, LR_LOADFROMFILE);
		hOpenFolder = LoadImage(NULL, OPENFOLDER, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
		hSubFolder = LoadImage(NULL, SUBFOLDER, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
		hSpareMiniature = LoadImage(NULL, DEFAULTPICTURE, IMAGE_BITMAP, 43,28, LR_LOADFROMFILE);

		if (!hWndListBox) {
			return	FALSE;
		}
		if(InitMainViewList() > 0)
		{
			SendMessage(hWndListBox, LB_SETCURSEL, 0, 0);
			nMainListIndex	=	0;
//			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
//			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
//			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
		}
		//WaitWin(hWnd, FALSE, IDS_OPENING, IDS_CAPTION, NULL, IDS_CANCEL, IDRM_OPENING);
	
		break;
		
	case IDRM_OPENING:		
		PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case WM_SETFOCUS:
		if(SendMessage(hWndListBox, LB_GETCOUNT, 0, 0)>0)
			SetFocus(hWndListBox);
		else
		{
			SetFocus(hWnd);
			if (IsWindowVisible(hWndListBox))
			{
				ShowWindow(hWndListBox, SW_HIDE);
			}
		}			
		break;

	case PWM_SHOWWINDOW:
		{
			char	Textbuf[PREBROW_MAXFULLNAMELEN+6] = "";
			int		nImageType, nPicCount;
			//int		nFolderNum;
			
			oriIndex = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
			if (oriIndex == LB_ERR)
			{
				oriIndex = nMainListIndex;
			}
			//SendMessage(hWndListBox, LB_RESETCONTENT, 0, 0);
			/*
			pNode = ListBuffer.pDataHead;
			if (bUpdated) 
			{
				bUpdated = FALSE;
				if (bFolderOpen&&(FolderPath[0] !=0)) 
				{
					InitFolderPicList(FolderPath, FALSE);
				}
				else if(bMMCOpen)
				{
					InitFolderPicList(FolderPath, TRUE);
				}
				else
				{					
					InitMainViewList();
				}
			}
			else
			{
				SendMessage(hWndListBox, LB_RESETCONTENT, 0, 0);
				if (pNode) 
				{
					do 
					{			
						//nIndex = SendMessage(hWndListBox, LB_ADDSTRING, 0, (LPARAM)pNode->szData);
						stat(pNode->szFullData, &filestat);
						memset(PicSize, 0, 15);
						tmpsize = (float)filestat.st_size/1024;
						//sprintf(PicSize, "%d Kb", tmpsize);
						floattoa(tmpsize, PicSize);
						strcat(PicSize, " kB");
						
						IsPicture(pNode->szFullData, &iType);						
						if (iType == PREBROW_FILETYPE_FOLDER)//include mmc
						{
							num = GetFileNumInDir(pNode->szFullData);
							sprintf(Textbuf, "%s", pNode->szData);
							
							nIndex = SendMessage(hWndListBox, LB_ADDSTRING, 0, (LPARAM)Textbuf);

							nFolderNum = GetSubFolderNum(pNode->szFullData);
							sprintf(Textbuf, "%s%d %s%d", IDS_SUBFOLDER, nFolderNum, IDS_PICNUM, num);
							SendMessage(hWndListBox, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)Textbuf);
							if ((stricmp(pNode->szFullData, MMCPICFOLDERPATH) == 0)||(stricmp(pNode->szFullData, "/mnt/flash/mmc/pictures/") == 0))
							{
								SendMessage(hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(nIndex, 0)), (LPARAM)hMemoryBmp);
								SendMessage(hWndListBox, LB_SETITEMDATA, nIndex, PREBROW_FILETYPE_MEMEORY);
							}
							else
							{
								SendMessage(hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(nIndex, 0)), (LPARAM)hFolderBmp);
								SendMessage(hWndListBox, LB_SETITEMDATA, nIndex, iType);
							}
						}
						else
						{
							nIndex = SendMessage(hWndListBox, LB_ADDSTRING, 0, (LPARAM)pNode->szData);
							SendMessage(hWndListBox,LB_SETAUXTEXT,MAKEWPARAM(nIndex,-1),(LPARAM)PicSize);
							SendMessage(hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(nIndex, 0)), (LPARAM)pNode->hbmp);					
							SendMessage(hWndListBox, LB_SETITEMDATA, nIndex, iType);
						}						
						
						pNode = pNode->pNext;
					}while (pNode!= ListBuffer.pDataHead &&pNode);
				}
			}
			*/
			if (bFolderOpen) 
			{
				//GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType);
				SetWindowText(hFrameWnd, FolderCaption);
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
			}
			else if (bMMCOpen)
			{
				//GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType);
				SetWindowText(hFrameWnd, FolderCaption);
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
			}
			else
			{
				SetWindowText(hFrameWnd, IDS_CAPTION);
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
			}
			nPicCount = SendMessage(hWndListBox, LB_GETCOUNT, 0, 0);
			if ( nPicCount > 0 )
			{
//				if(SendMessage(hWndListBox, LB_SETCURSEL, oriIndex, 0) == LB_ERR)
//					SendMessage(hWndListBox, LB_SETCURSEL, 0, 0);
				GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType);
				if (nFileType == PREBROW_FILETYPE_FOLDER) 
				{
					PDASetMenu(hFrameWnd, hFolderHLMenu);
				}
				else if (nFileType == PREBROW_FILETYPE_MEMEORY)
				{
					PDASetMenu(hFrameWnd, hMMCHLMenu);
				}
				else
				{
					nImageType = SendMessage(hWndListBox, LB_GETITEMDATA, 1, 0);

					if ((nImageType == PREBROW_FILETYPE_FOLDER)||(nImageType == PREBROW_FILETYPE_MEMEORY)||nImageType == LB_ERR)
					{						
						PDASetMenu(hFrameWnd, hSimplexMenu);
					}
					else
					{
						PDASetMenu(hFrameWnd, hViewMenu);
					}
					
				}
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
				//SendMessage(hWndListBox, LB_SETCURSEL, oriIndex, 0);
				ShowWindow(hWndListBox, SW_SHOW);
			}
			else
			{
				PDASetMenu(hFrameWnd, NULL);
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				ShowWindow(hWndListBox, SW_HIDE);
			}
			SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), NULL);
			SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), NULL);
			loadBitmapForList(hWndListBox, 0, &ListBuffer);
			SetFocus(hWnd);
		}
		break;
		
	case PIC_LOADBMP:
		{
			Image_LoadMiniature(LOWORD(wParam), HIWORD(wParam), hWndListBox, &ListBuffer);
		}
		break;

	case IDC_SET:
		if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
		{
			if(SetBKPicture(curFileName))
			{				
				CallIdleBkPicChange();
				PLXTipsWin(NULL, hWnd, 0, IDS_WALLPAPERSET, szTemp, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
			}
		}
		break;
		
	case IDC_CLEAR:
		if(SetBKPicture("Default"))
		{
			CallIdleBkPicChange();
			PLXTipsWin(NULL, hWnd, 0, IDS_WALLPAPERCLEAR, "Default", Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
		}
		break;

	case WM_COMMAND:
		{
			int	fImageType;
			if (HIWORD(wParam) == LBN_SELCHANGE) 
			{				
				loadBitmapForList(hWndListBox, 0, &ListBuffer);
				
				if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
				{
					if (nFileType == PREBROW_FILETYPE_FOLDER) 
					{
						PDASetMenu(hFrameWnd, hFolderHLMenu);
					}
					else if (nFileType == PREBROW_FILETYPE_MEMEORY)
					{
						PDASetMenu(hFrameWnd, hMMCHLMenu);
					}
					else
					{	
						fImageType = SendMessage(hWndListBox, LB_GETITEMDATA, 1, 0);
						if (fImageType == PREBROW_FILETYPE_MEMEORY || fImageType == PREBROW_FILETYPE_FOLDER||fImageType == LB_ERR)
						{
							PDASetMenu(hFrameWnd, hSimplexMenu);
						}
						else
						{
							PDASetMenu(hFrameWnd, hViewMenu);
						}
					}
				}
				//printf("\n %s %d ***************LBN_SELCHANGE end***************\n", __FILE__, __LINE__);
				break;
			}
			switch(LOWORD(wParam))
			{
			case IDC_OPEN:
				nMainListIndex = index = SendMessage(hWndListBox,LB_GETCURSEL,0,0);
				//nFileType = SendMessage(hWndListBox,LB_GETITEMDATA,(WPARAM)index,NULL);
				if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
				{
					if (nFileType == PREBROW_FILETYPE_FOLDER) 
					{
						/*
						if (bMMCOpen)
						{
							bFolderOpen = FALSE;
							strcpy(FolderCaption, szTemp);
							InitFolderPicList(curFileName, TRUE);
						}
						else
						{
							bFolderOpen = TRUE;
							strcpy(FolderCaption, szTemp);
							InitFolderPicList(curFileName, FALSE);
						}
						*/
						strcpy(FolderCaption, szTemp);
						InitFolderPicList(curFileName, bMMCOpen);
						//AddPicturesToList(hWndListBox, cFileName);
					}
					else if(nFileType == PREBROW_FILETYPE_MEMEORY)
					{
						bMMCOpen = TRUE;
						strcpy(FolderCaption, szTemp);
						InitFolderPicList(curFileName, TRUE);
					}
					else
					{				
						BrowsePicture(hWndListBox, hFrameWnd, NULL, 0, NULL, &ListBuffer, FALSE, -1, NULL);
					}
				}
				break;

			case IDC_SENDALL:
				{
					bSendAll = TRUE;
					PDASetMenu(hFrameWnd, hSendSelMenu);
					PostMessage(hFrameWnd, WM_KEYDOWN, VK_F5, lParam );
				}
				break;

			case IDC_SENDSELECT:
				ProcessMultiSel(hFrameWnd, hWnd, &ListBuffer, IDC_SENDSELECT, hWndListBox);
				break;

			case IDC_VIAMMS:
				{
					if (!bSendAll) 
					{
						GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType);
						APP_EditMMS(hFrameWnd,hWnd, 0, MMS_CALLEDIT_IMAGE, curFileName);
					}
					else
					{
						int	i;
						PLISTATTNODE pHead = NULL;
						num = SendMessage(hWndListBox, LB_GETCOUNT, 0, 0);
						//iOldSel = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
						for (i=0; i<num; i++)
						{
							iType = SendMessage(hWndListBox, LB_GETITEMDATA, i, 0);
							if ((iType != PREBROW_FILETYPE_FOLDER)&&(iType != PREBROW_FILETYPE_MEMEORY)) 
							{
								if(PREBROW_GetData(&ListBuffer, i, szTemp, curFileName))
									AddMultiSelPicToList(&pHead, szTemp, curFileName);
									//APP_EditMMS(hFrameWnd,hWnd, 0, MMS_CALLEDIT_IMAGE, curFileName);
							}
							
						}
						APP_EditMMS(hFrameWnd,hWnd, IDRM_EDITMMS, MMS_CALLEDIT_MULTIIMAGE, (PSTR)pHead);
						FreeMultiSelList(&pHead);
						//	SendMessage(hWndListBox, LB_SETCURSEL, iOldSel, 0);
						bSendAll = FALSE;
					}
				}
				break;

			case IDC_VIAEMAIL:
				if (!bSendAll) 
				{
					if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
					{
						p = strrchr(curFileName,'/');
						strcpy(szTemp, p+1);
						MAIL_CreateEditInterface(hFrameWnd, NULL, curFileName, szTemp, 1);
					}
				}
				else
				{
					int	i;
					PSTR	p;
					PLISTATTNODE pHead = NULL;
					num = SendMessage(hWndListBox, LB_GETCOUNT, 0, 0);
					//iOldSel = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
					for (i=0; i<num; i++)
					{
						iType = SendMessage(hWndListBox, LB_GETITEMDATA, i, 0);
						if ((iType != PREBROW_FILETYPE_FOLDER)&&(iType != PREBROW_FILETYPE_MEMEORY)) 
						{
							if(PREBROW_GetData(&ListBuffer, i, szTemp, curFileName))
							{
								p = strrchr(curFileName, '/');
								p++;
								AddMultiSelPicToList(&pHead, p, curFileName);
							}
							//APP_EditMMS(hFrameWnd,hWnd, 0, MMS_CALLEDIT_IMAGE, curFileName);
						}
						
					}
					MAIL_CreateMultiInterface(hFrameWnd, pHead);
					//APP_EditMMS(hFrameWnd,hwnd, IDRM_EDITMMS, MMS_CALLEDIT_MULTIIMAGE, (PSTR)pHead);
					FreeMultiSelList(&pHead);
					//	SendMessage(hWndListBox, LB_SETCURSEL, iOldSel, 0);
					bSendAll = FALSE;
				}
				break;

			case IDC_VIABLUETOOTH:
				if (!bSendAll) 
				{
					if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
						BtSendData(hFrameWnd, curFileName, szTemp, BTPICTURE);
				}
				else
				{
					int	i;
					PLISTATTNODE pHead = NULL;
					num = SendMessage(hWndListBox, LB_GETCOUNT, 0, 0);
					//iOldSel = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
					for (i=0; i<num; i++)
					{
						iType = SendMessage(hWndListBox, LB_GETITEMDATA, i, 0);
						if ((iType != PREBROW_FILETYPE_FOLDER)&&(iType != PREBROW_FILETYPE_MEMEORY)) 
						{
							if(PREBROW_GetData(&ListBuffer, i, szTemp, curFileName))
								AddMultiSelPicToList(&pHead, szTemp, curFileName);
							//APP_EditMMS(hFrameWnd,hWnd, 0, MMS_CALLEDIT_IMAGE, curFileName);
						}
						
					}
					BtSendData(hFrameWnd, (PSTR)pHead, NULL, BTMULTIPICTURE);
					//MAIL_CreateMultiInterface(hFrameWnd, pHead);
					//APP_EditMMS(hFrameWnd,hwnd, IDRM_EDITMMS, MMS_CALLEDIT_MULTIIMAGE, (PSTR)pHead);
					FreeMultiSelList(&pHead);
					//	SendMessage(hWndListBox, LB_SETCURSEL, iOldSel, 0);
					bSendAll = FALSE;
				}
				break;

			case IDC_CREATEFOLDER:
				PicBrowser_Edit(hWnd, NULL, FolderPath, TRUE);
				/*
				if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
				{
					if (nFileType == PREBROW_FILETYPE_MEMEORY)
					{
						PicBrowser_Edit(hWnd, NULL, PHONEPICFILEDIR, TRUE);
					}
					else
					{
						if (curFileName[strlen(curFileName) - 1] == '/')
						{
							curFileName[strlen(curFileName) - 1] = 0;
						}
						p = strrchr(curFileName, '/');
//						strncpy(pWorkPath, curFileName, p - curFileName);
						chdir(pWorkPath);
						PicBrowser_Edit(hWnd, NULL, pWorkPath, TRUE);
					}
				}
				*/
				break;

			case IDC_RENAME:
				{				
					index = SendMessage(hWndListBox,LB_GETCURSEL,0,0);
					if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
						PicBrowser_Edit(hWnd, szTemp, curFileName, FALSE);					
				}
				break;				

			case IDC_DELETE:
				szTemp[0] = 0;
				curFileName[0] = 0;
				if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
				{
					if (nFileType == PREBROW_FILETYPE_FOLDER) 
					{
						sprintf(delprompt, "%s:\n%s", szTemp, IDS_DELFOLDER);
						PLXConfirmWinEx(hFrameWnd, hWnd, delprompt, Notify_Request, IDS_CAPTION, IDS_YES, IDS_NO, IDC_PROCESSDEL);
					}
					else
					{
						sprintf(delprompt, "%s:\n%s?", szTemp, IDS_DELETE);
						PLXConfirmWinEx(hFrameWnd, hWnd, delprompt, Notify_Request, IDS_CAPTION, IDS_YES, IDS_NO, IDC_PROCESSDEL);				
					}
				}
				break;

			case IDC_COPY:
				PicMoveToFolder(hWnd, IDRM_COPYTOFOLER);
				break;

			case IDC_DELALL:
				PLXConfirmWinEx(hFrameWnd, hWnd, IDS_DELSELPIC, Notify_Request, IDS_CAPTION, IDS_YES, IDS_NO, IDRM_DELALL);							
				break;

			case IDC_DELSELECT:
				ProcessMultiSel(hFrameWnd, hWnd, &ListBuffer, IDC_DELSELECT, hWndListBox);
				break;

			case IDC_MOVETOFOLDER:
				PicMoveToFolder(hWnd, IDRM_MOTOFOLDER);
				break;

			case IDC_COPYSELECT:
				ProcessMultiSel(hFrameWnd, hWnd, &ListBuffer, IDC_COPYSELECT, hWndListBox);
				break;

			case IDC_MOVESELECT:
				ProcessMultiSel(hFrameWnd, hWnd, &ListBuffer, IDC_MOVESELECT, hWndListBox);
				break;

			case IDC_COPYALL:
				PicMoveToFolder(hWnd, IDRM_COPYALL);
				break;

			case IDC_MOVEALL:
				PicMoveToFolder(hWnd, IDRM_MOVEALL);
				break;
			
			case IDC_WALLPAPER:
				PicSetWallPaper(hWnd);
				break;
				
			case IDC_SHOWDETAIL:
				if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
					ShowPicDetail(hFrameWnd,szTemp, curFileName, nFileType, FALSE);
				break;

			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;

			}

		}
		break;
	
	case IDRM_COPYALL:
		{
			char	path[PREBROW_MAXFULLNAMELEN] = "";
			char	bakpath[PREBROW_MAXFULLNAMELEN] = "";
			char	*name;
			int		i,  nCount;
			strcpy(path, (char *)lParam);
			if (path[strlen(path) -1 ] != '/') {
				strcat(path, "/");
			}
			strcpy(bakpath , path);
			//index = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
			nCount = SendMessage(hWndListBox, LB_GETCOUNT, 0, 0);
			//WaitWin(hWnd, TRUE, IDS_COPING, )
			WaitWindowStateEx(hWnd, TRUE, IDS_COPING, NULL, NULL, NULL);
			for (i = nCount -1; i >=0 ; i--) 
			{
				SendMessage(hWndListBox, LB_SETCURSEL, i, 0);
				if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
				{
					if ((nFileType != PREBROW_FILETYPE_FOLDER) &&(nFileType != PREBROW_FILETYPE_MEMEORY))
					{						
						name = strrchr(curFileName, '/');
						name++;
						strcat(path, name);
						
						stat(curFileName, &filestat);
						if (strnicmp(path, MMCPICFOLDERPATH, strlen(MMCPICFOLDERPATH)) == 0)
						{
#ifdef _EMULATE_
							AvailSpace = GetAvailFlashSpace();
#else
							AvailSpace = GetAvailMMCSpace();
#endif
						}
						else
						{
							AvailSpace = GetAvailFlashSpace();
						}
						if ((int)filestat.st_size/1024 > AvailSpace)
						{
							WaitWindowStateEx(hWnd, FALSE, IDS_COPING, NULL, NULL, NULL);
							PLXTipsWin(hFrameWnd, hWnd, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
							//SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
							//DestroyWindow(hwnd);
							return FALSE;
						}
						if ((fp = fopen(path, "r")) != NULL) 
						{
							fclose(fp);
							//WaitWindowStateEx(hWnd, FALSE, IDS_COPING, NULL, NULL, NULL);
							if(!DefAnotherName(NULL, hWnd, curFileName, path, szTemp, 0, 0))
								continue;
							//PLXTipsWin(hFrameWnd, hWnd, IDRM_DEFOTHERNAME, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
							//return FALSE;
						}
						PicCopyFile(curFileName, path);						
					}
				}
				strcpy(path , bakpath);
			}
			WaitWindowStateEx(hWnd, FALSE, IDS_COPING, NULL, NULL, NULL);
			PLXTipsWin(hFrameWnd, hWnd, 0, ML("Copied"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
		}
		break;
		
	case IDRM_MOVEALL:
		{
			char	path[PREBROW_MAXFULLNAMELEN] = "";
			char	bakpath[PREBROW_MAXFULLNAMELEN] = "";
			char	*name;
			int		i,  nCount;
			strcpy(path, (char *)lParam);
			if (path[strlen(path) -1 ] != '/') {
				strcat(path, "/");
			}
			strcpy(bakpath , path);
			//index = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
			WaitWindowStateEx(hWnd, TRUE, IDS_MOVING, NULL, NULL, NULL);
			nCount = SendMessage(hWndListBox, LB_GETCOUNT, 0, 0);
			for (i = nCount -1; i >=0 ; i--) 
			{
				SendMessage(hWndListBox, LB_SETCURSEL, i, 0);
				if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
				{
					if ((nFileType != PREBROW_FILETYPE_FOLDER) &&(nFileType != PREBROW_FILETYPE_MEMEORY))
					{
						
						
						name = strrchr(curFileName, '/');
						name++;
						strcat(path, name);
						
						if (strnicmp(curFileName, path, 10) != 0)
						{
							stat(curFileName, &filestat);
							if (strnicmp(path, MMCPICFOLDERPATH, strlen(MMCPICFOLDERPATH)) == 0)
							{
#ifdef _EMULATE_
								AvailSpace = GetAvailFlashSpace();
#else
								AvailSpace = GetAvailMMCSpace();
#endif
							}
							else
							{
								AvailSpace = GetAvailFlashSpace();
							}
							if ((int)filestat.st_size/1024 > AvailSpace)
							{
								WaitWindowStateEx(hWnd, FALSE, IDS_COPING, NULL, NULL, NULL);
								PLXTipsWin(hFrameWnd, hWnd, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
								//SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
								//DestroyWindow(hwnd);
								return FALSE;
							}
						}
						if ((fp = fopen(path, "r")) != NULL) 
						{
							fclose(fp);
							//WaitWindowStateEx(hWnd, FALSE, IDS_MOVING, NULL, NULL, NULL);
							if(!DefAnotherName(NULL, hWnd, curFileName, path, szTemp, 0, 0))
								continue;
							//PLXTipsWin(hFrameWnd, hWnd, IDRM_DEFOTHERNAME, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
							//return FALSE;
						}
						if(PicMoveFile(curFileName, path))
						{
							printf("\n%d move file from %s to %s successfully\n", __LINE__, curFileName, path);
							PREBROW_DelData(&ListBuffer, i);
							SendMessage(hWndListBox, LB_DELETESTRING, i, 0);
						}
						else
							printf("\n%d failed to rename file %s to %s\n", __LINE__, curFileName, path);

						
							//PLXTipsWin(hFrameWnd, hWnd, 0, ML("Moved"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
						
					}
				}
				strcpy(path , bakpath);
			}
			WaitWindowStateEx(hWnd, FALSE, IDS_MOVING, NULL, NULL, NULL);
			PLXTipsWin(hFrameWnd, hWnd, 0, ML("Moved"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
		}
		break;

	case IDRM_DEFOTHERNAME:
		break;
		
	case IDRM_COPYTOFOLER:
		{
			char	path[PREBROW_MAXFULLNAMELEN] = "";
			char	*name;
			int		index;
			BOOL	bWait	= FALSE;
			strcpy(path, (char *)lParam);
			index = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
			if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
			{
				if (path[strlen(path) -1 ] != '/') {
					strcat(path, "/");
				}
				name = strrchr(curFileName, '/');
				name++;
				strcat(path, name);
				
				
				//BtCopyFile
				stat(curFileName, &filestat);
				if (strnicmp(path, MMCPICFOLDERPATH, strlen(MMCPICFOLDERPATH)) == 0)
				{
#ifdef _EMULATE_
					AvailSpace = GetAvailFlashSpace();
#else
					AvailSpace = GetAvailMMCSpace();
#endif
				}
				else
				{
					AvailSpace = GetAvailFlashSpace();
				}
				if ((int)filestat.st_size/1024 > AvailSpace)
				{
					
					PLXTipsWin(hFrameWnd, hWnd, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
					//SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
					//DestroyWindow(hwnd);
					return FALSE;
				}
				if ((fp = fopen(path, "r")) != NULL) 
				{
					fclose(fp);
					//PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
					if(!DefAnotherName(NULL, hWnd, curFileName, path, szTemp, 0, 0))
					{
						SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
						DestroyWindow(hWnd);
						break;
					}
				}
				if (IsBigFileFromSD(curFileName)||IsBigFileFromSD(path))
				{
					WaitWindowStateEx(hWnd, TRUE, IDS_COPING, NULL, NULL, NULL);
					bWait	=	TRUE;
				}
				
				PicCopyFile(curFileName, path);
				if (bWait)
				{
					bWait = FALSE;
					WaitWindowStateEx(hWnd, FALSE, IDS_COPING, NULL, NULL, NULL);
				}
				PLXTipsWin(hFrameWnd, hWnd, 0, ML("Copied"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
				
			}
		}
		break;
		
	case IDRM_MOTOFOLDER:
		{
			char	path[PREBROW_MAXFULLNAMELEN] = "";
			char	*name;
			int		index;
			
			strcpy(path, (char *)lParam);
			index = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
			if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
			{
				if (path[strlen(path) -1 ] != '/') {
					strcat(path, "/");
				}
				name = strrchr(curFileName, '/');
				name++;
				strcat(path, name);
				//printf("oldname:\t%s\n newname:\t%s\n",curFileName,path);
				
				
				if (strnicmp(curFileName, path, 10) != 0)
				{
					stat(curFileName, &filestat);
					if (strnicmp(path, MMCPICFOLDERPATH, strlen(MMCPICFOLDERPATH)) == 0)
					{
#ifdef _EMULATE_
						AvailSpace = GetAvailFlashSpace();
#else
						AvailSpace = GetAvailMMCSpace();
#endif							
					}
					else
					{
						AvailSpace = GetAvailFlashSpace();
					}
					if ((int)filestat.st_size/1024 > AvailSpace)
					{
						//WaitWindowStateEx(hWnd, FALSE, IDS_COPING, NULL, NULL, NULL);
						PLXTipsWin(hFrameWnd, hWnd, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
						//SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
						//DestroyWindow(hwnd);
						return FALSE;
					}
				}
				if ((fp = fopen(path, "r")) != NULL)
				{
					fclose(fp);
					if(!DefAnotherName(NULL, hWnd, curFileName, path, szTemp, 0, 0))
					{
						SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
						DestroyWindow(hWnd);
						break;
					}
					//PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
				}
				if(PicMoveFile(curFileName, path))
				{
					//printf("\n%d move file from %s to %s successfully\n", __LINE__,curFileName, path);
					PREBROW_DelData(&ListBuffer, index);
					SendMessage(hWndListBox, LB_DELETESTRING, index, 0);
					PLXTipsWin(hFrameWnd, hWnd, 0, ML("Moved"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
				}
				else
					printf("\n%d failed to move file from %s to %s\n", __LINE__, curFileName, path);
				
			}
		}	
		break;
		
	case IDRM_DELALL:
		if (lParam == 1)
		{
			char	path[100];
			PSTR	ptr;
			memset(path, 0, sizeof(path));
			WaitWindowStateEx(hFrameWnd, TRUE, ML("Deleting pictures..."), NULL, NULL, NULL);
			UpdateWaitwinContext(IDS_DELETING);
			GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType);
			ptr = strrchr(curFileName,'/');
			strncpy(path, curFileName, ptr-curFileName);
			DeleteDirectory(path, FALSE);
			PREBROW_FreeListBuf(&ListBuffer);
			SendMessage(hWndListBox, LB_RESETCONTENT, 0, 0);
			if (bMMCOpen)
			{
				InitFolderPicList(FolderPath, TRUE);
			}
			else if (bFolderOpen)
			{
				InitFolderPicList(FolderPath, FALSE);
			}
			else
				InitMainViewList();
			WaitWindowStateEx(hFrameWnd, FALSE, IDS_DELETING, NULL, NULL, NULL);
			PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DELETED, NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
		}			
		break;

	case IDC_PROCESSDEL:
		if (lParam == 1) 
		{
			WaitWindowStateEx(hFrameWnd, TRUE, IDS_DELETING, NULL, NULL, NULL);
			index = SendMessage(hWndListBox,LB_GETCURSEL,0,0);
			GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType);
			SendMessage(hWndListBox, LB_DELETESTRING, index, 0);
//			if (index-1 >= 0) {
//				SendMessage(hWndListBox, LB_SETCURSEL, index-1, 0);
//			}
			PREBROW_DelData(&ListBuffer, index);
			if (nFileType == PREBROW_FILETYPE_FOLDER) {
				DeleteDirectory(curFileName,TRUE);
			}
			else
			{
				remove(curFileName);
			}
			WaitWindowStateEx(hFrameWnd, FALSE, IDS_DELETING, NULL, NULL, NULL);
			PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DELETED, NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
		}
		break;

	case WM_KEYDOWN:
	    switch(wParam)
		{
		case VK_F10:
			if (bFolderOpen) {
//				FolderCaption[0] = NULL;
//				FolderPath[0] = 0;
				InitMainViewList();
				InvalidateRect(hWnd, NULL, TRUE);
				SetFocus(hWnd);
				//SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
				break;
			}
			else if (bMMCOpen)
			{
				if ((strcmp(FolderPath, MMCPICFOLDERPATH) == 0)
					||(strcmp(FolderPath, MMCPICFOLDERPATHEX) == 0))
				{
//					FolderCaption[0] = NULL;
//					FolderPath[0] = 0;
					InitMainViewList();
					InvalidateRect(hWnd, NULL, TRUE);
					SetFocus(hWnd);
				}
				else
				{
					char	NewPath[PREBROW_MAXFULLNAMELEN] = "";
					GetLastPath(FolderPath, NewPath);

					memset(FolderCaption, 0, PREBROW_MAXFILENAMELEN);
					if ((strcmp(NewPath, MMCPICFOLDERPATH) == 0)
						||(strcmp(NewPath, MMCPICFOLDERPATHEX) == 0))
					{
						GetMMCName(FolderCaption);
					}
					else
					{
						p = strrchr(NewPath, '/');
						strncpy(FolderCaption, p+1, strlen(p)-1);
					}
					FolderCaption[strlen(FolderCaption)] = 0;
					//InitFolderPicList(MMCPICFOLDERPATH, TRUE);
					InitFolderPicList(NewPath, TRUE);
				}
			}
			else
			{
				PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;

		case VK_RETURN:
			num = SendMessage(hWndListBox, LB_GETCOUNT, 0, 0);
			if (num>0) {
				
				PostMessage(hWnd, WM_COMMAND, IDC_OPEN, 0);
			}
			break;

		case VK_F5:
			PostMessage(hFrameWnd, WM_KEYDOWN, wParam, lParam );
			break;

		default:
			//PostMessage(GetParent(hWnd),wMsgCmd,wParam,lParam);
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow (hWnd);

		break;

	case WM_PAINT:
		{
			HDC	hdc;
			int	oldbm;
			RECT	rc;
//			if (SendMessage(hWndListBox, LB_GETCOUNT, 0, 0) <=0) {
//				break;
//			}
			//printf("\r\n ****%s ####%d ^^^^^begin paint");
			if (SendMessage(hWndListBox, LB_GETCOUNT, 0, 0) > 0) {
//				SetFocus(hWndListBox);
				ShowWindow(hWndListBox, SW_SHOW);
				/*
				GetClientRect(hWnd,&rc);
				hdc = BeginPaint(hWnd,NULL);
				//DrawImageFromFile(hdc,"/rom/progman/grpbg.bmp",0,0,ROP_SRC);
				oldbm = SetBkMode(hdc,TRANSPARENT);
				DrawText(hdc, IDS_NOPIC, -1, &rc, DT_CENTER|DT_VCENTER);
				SetBkMode(hdc,oldbm);
				EndPaint(hWnd,NULL);
				*/
			}
			else
			{
				ShowWindow(hWndListBox, SW_HIDE);
//				SetFocus(hWnd);
			}
			
			GetClientRect(hWnd,&rc);
			hdc = BeginPaint(hWnd,NULL);
			//DrawImageFromFile(hdc,"/rom/progman/grpbg.bmp",0,0,ROP_SRC);
			oldbm = SetBkMode(hdc,TRANSPARENT);
			DrawText(hdc, IDS_NOPIC, -1, &rc, DT_CENTER|DT_VCENTER);
			SetBkMode(hdc,oldbm);
			EndPaint(hWnd,NULL);
			
			//printf("\r\n ****%s ####%d ////////////////end paint");
		}
		break;


	case WM_DESTROY:
		if (hFolderBmp) 
		{
			DeleteObject(hFolderBmp);
		}		
		if (hRBoffbmp) {
			DeleteObject(hRBoffbmp);
		}		
		if (hRBonbmp) {
			DeleteObject(hRBonbmp);
		}
		if (hOpenFolder)
		{
			DeleteObject(hOpenFolder);
		}
		if (hSubFolder)
		{
			DeleteObject(hSubFolder);
		}

		if (hMemoryBmp)
		{
			DeleteObject(hMemoryBmp);
		}
		if (hFolderHLMenu) 
		{
			DestroyMenu(hFolderHLMenu);
		}

		if (hSimplexMenu)
		{
			DestroyMenu(hSimplexMenu);
		}
		if (hMMCHLMenu)
		{
			DestroyMenu(hMMCHLMenu);
		}
		if (hSpareMiniature)
		{
			DeleteObject(hSpareMiniature);
			hSpareMiniature = NULL;
		}
		//clockdegree	=	0;
		DestroyMenu(hViewMenu);
		DestroyMenu(hSendSelMenu);

		PREBROW_FreeListBuf (&ListBuffer);
		hMainView = NULL;
		UnregisterClass ("BrowserImgWndClass", NULL);
		bBrowserImgWndClass = FALSE;
		DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );

		f_sleep_unregister(PICBROWSE);
#ifdef _MODULE_DEBUG_
		EndObjectDebug();
#endif
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;
}

static	int	InitMainViewList()
{
	DIR		*dir;
	int		num, nFlags, nIndex, nFolder;
	char	szMMCName[31] = "";
	char	Textbuf[PREBROW_MAXFULLNAMELEN] = "";
	//SendMessage(hWndListBox, LB_RESETCONTENT, 0, 0);
	bFolderOpen = FALSE;
	bMMCOpen	= FALSE;
	//strcpy(FolderPath, PHONEPICFILEDIR);
	WaitWin(hMainView, TRUE, IDS_OPENING, IDS_CAPTION, NULL, IDS_CANCEL, IDRM_OPENING);
	num = AddPicturesToList(hWndListBox,PHONEPICFILEDIR, &ListBuffer, hFolderBmp);
	if (MMC_CheckCardStatus()) 
	{
		if ((dir = opendir(MMCPICFOLDERPATH)) != NULL) 
		{
			closedir(dir);
#ifdef _EMULATE_
			strcpy(szMMCName, "Memory card");
#else
			GetMMCName(szMMCName);
#endif
			nFlags = GetFileNumInDir(MMCPICFOLDERPATH);
			nFolder	=	GetSubFolderNum(MMCPICFOLDERPATH);
			sprintf(Textbuf, "%s%d %s%d", IDS_SUBFOLDER, nFolder, IDS_PICNUM, nFlags);

			nIndex = SendMessage(hWndListBox, LB_ADDSTRING, 0, (LPARAM)szMMCName);
			SendMessage(hWndListBox, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)Textbuf);
			
			SendMessage(hWndListBox, LB_SETITEMDATA, nIndex, PREBROW_FILETYPE_MEMEORY);
			PREBROW_AddPicData(&ListBuffer, szMMCName, MMCPICFOLDERPATH, NULL, 0);
			SendMessage(hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(nIndex,0)), (LPARAM)hMemoryBmp);
			//num += AddFileToList(hWndListBox, MMCPICFOLDERPATH, &ListBuffer, hFolderBmp);
		}
		else
		{
			mkdir(MMCPICFOLDERPATH,0666);
//			mkdir("/mnt/fat16/picture/folder", 0666);
#ifdef _EMULATE_
			strcpy(szMMCName, "Memory card");
#else
			GetMMCName(szMMCName);
#endif
	//		GetMMCName(szMMCName);
			nFlags = GetFileNumInDir(MMCPICFOLDERPATH);
			nFolder	=	GetSubFolderNum(MMCPICFOLDERPATH);
			sprintf(Textbuf, "%s%d %s%d", IDS_SUBFOLDER, nFolder, IDS_PICNUM, nFlags);

			nIndex = SendMessage(hWndListBox, LB_ADDSTRING, 0, (LPARAM)szMMCName);
			SendMessage(hWndListBox, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)Textbuf);
			
			SendMessage(hWndListBox, LB_SETITEMDATA, nIndex, PREBROW_FILETYPE_MEMEORY);
			PREBROW_AddPicData(&ListBuffer, szMMCName, MMCPICFOLDERPATH, NULL, 0);
			SendMessage(hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(nIndex,0)), (LPARAM)hMemoryBmp);
		}
		num ++;
		
	}
	SetWindowText(hFrameWnd,IDS_CAPTION);
	if ( num > 0 )
	{
		nFlags = GetMenuState(hViewMenu, IDC_CREATEFOLDER, MF_BYCOMMAND);
		if (-1 == nFlags) {
			InsertMenu(hViewMenu, 4, MF_BYPOSITION, IDC_CREATEFOLDER, IDS_CREATEFOLDER);
		}
		nFlags = GetMenuState(hSimplexMenu, IDC_CREATEFOLDER, MF_BYCOMMAND);
		if (-1 == nFlags) {
			InsertMenu(hSimplexMenu, 3, MF_BYPOSITION, IDC_CREATEFOLDER, IDS_CREATEFOLDER);
		}
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
		//SendMessage(hWndListBox, LB_SETCURSEL, 0, 0);
		ShowWindow(hWndListBox, SW_SHOW);
		PDASetMenu(hFrameWnd, hViewMenu);
	}
	else
	{
		PDASetMenu(hFrameWnd, NULL);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		ShowWindow(hWndListBox, SW_HIDE);
	}
	nIndex = PREBROW_FindData(&ListBuffer, FolderPath);
	if (nIndex != -1)
	{
		SendMessage(hWndListBox, LB_SETCURSEL, nIndex, 0);
	}
	else
		SendMessage(hWndListBox, LB_SETCURSEL, 0, 0);
	
	strcpy(FolderPath, PHONEPICFILEDIR);
	WaitWin(hMainView, FALSE, IDS_OPENING, IDS_CAPTION, NULL, IDS_CANCEL, IDRM_OPENING);
	return	num;
}

static	int	InitFolderPicList(PCSTR pPath, BOOL bSD)
{
//	DIR		*dir;
	DWORD		nFlags, nSimPlexMenu;
	int		num, nIndex;
	//SendMessage(hWndListBox, LB_RESETCONTENT, 0, 0);
	bFolderOpen = !bSD;
	//strcpy(FolderPath, pPath);
	//printf("folderpath:\t%s\n",pPath);

	bMMCOpen = bSD;
	WaitWin(NULL, TRUE, IDS_OPENING, IDS_CAPTION, NULL, NULL, 0);

	num = AddPicturesToList(hWndListBox,pPath, &ListBuffer, hFolderBmp);
	
	if ( num != 0 )
	{
		nFlags = GetMenuState(hViewMenu, IDC_CREATEFOLDER, MF_BYCOMMAND);
		nSimPlexMenu = GetMenuState(hSimplexMenu, IDC_CREATEFOLDER, MF_BYCOMMAND);
		
		//printf("%d init folder:\t nFlags: %d   bSd:%d", __LINE__,nFlags, bSD);
		if ((-1 != nFlags)&&(!bSD)) {
			DeleteMenu(hViewMenu, IDC_CREATEFOLDER, MF_BYCOMMAND);
			//	printf("Delete menu item Createfolder");
		}
		if ((-1 != nSimPlexMenu)&&(!bSD)) {
			DeleteMenu(hSimplexMenu, IDC_CREATEFOLDER, MF_BYCOMMAND);
			//	printf("Delete menu item Createfolder");
		}
		PDASetMenu(hFrameWnd, hViewMenu);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
		//SendMessage(hWndListBox, LB_SETCURSEL, 0, 0);
	}
	else
	{
		PDASetMenu(hFrameWnd, NULL);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
//		ShowWindow(hWndListBox, SW_HIDE);
	}
	nIndex = PREBROW_FindData(&ListBuffer, FolderPath);
	if (nIndex != -1)
	{
		SendMessage(hWndListBox, LB_SETCURSEL, nIndex, 0);
	}
	
	strcpy(FolderPath, pPath);
	WaitWin(NULL, FALSE, IDS_OPENING, IDS_CAPTION, NULL, NULL, 0);
	
	InvalidateRect(hMainView, NULL, TRUE);
	SetWindowText(hFrameWnd, FolderCaption);
	return	num;
}
/*********************************************************************
* Function	   AddPicturesToList
* Purpose      把图片文件加入列表
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int	AddPicturesToList(HWND hList , PCSTR pPath, PLISTBUF pLtB, HBITMAP hFolder)
{
//	char szTemp[PREBROW_MAXFILENAMELEN];
	int n;	
	SendMessage (hList, LB_RESETCONTENT, NULL, NULL);
	PREBROW_FreeListBuf (pLtB);
	
	AddFileToList(hList , pPath, pLtB, hFolder);
//	AddFileToList(hList, "/rom/game/pao/", pLtB);
	
	n = pLtB->nDataNum;
	if (LB_ERR != n)
	{
		SendMessage(hList,LB_SETCURSEL,(WPARAM)0,NULL);
	}

	return n;
}
/*********************************************************************\
* Function	   AddFileToList
* Purpose      把某类型文件加入列表
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int	AddFileToList(HWND hList, const char* pFilePath, LISTBUF* ListBuffer, HBITMAP	hFolderIcon)
{

	struct dirent *dirinfo = NULL;
	struct stat	filestat;

	struct tm *StTime;
	
	int	nRtnNum;	
	char path[PREBROW_MAXPATHLEN] = "";
	char PicSize[18] = "";
	float		tmpsize;
	DIR  *dirtemp = NULL;
	PLISTDATA	pFirstNode = NULL, pLastNode = NULL;

	int		nItemData;
	
	

	if (!hList)
	{
		return -1;
	}

	memset(path, 0x0, PREBROW_MAXPATHLEN);

	strcpy(path, pFilePath);

    nRtnNum = 0;

	dirtemp = opendir(path);

	if(dirtemp == NULL)
		return nRtnNum;
	

	while((dirinfo = readdir(dirtemp))!=NULL)
	{
		int index,nFileNum, nSubFolderNum;
		HBITMAP	hbitbmp = NULL;
		char	FullName[PREBROW_MAXFULLNAMELEN] = "";
		char	FileName[PREBROW_MAXFILENAMELEN+6] = "";
		char	szAuxtext[PREBROW_MAXFILENAMELEN+6] = "";
		char	*Suffix;

		
		
		memset(FileName, 0, PREBROW_MAXFILENAMELEN + 6);
		memset(FullName, 0, PREBROW_MAXFULLNAMELEN);
		strcpy ( FullName, pFilePath );
		if (FullName[strlen(FullName) -1 ] != '/') 
		{
			strcat(FullName,"/");
		}
		strcat ( FullName, dirinfo->d_name);
		if (strlen(FullName) > 255 ||strlen(dirinfo->d_name) > 40)
		{
			continue;
		}
		stat(FullName,&filestat);
		StTime = gmtime(&filestat.st_mtime);
		/*--------------if FullName is a dir----------------------------*/
		if ((((filestat.st_mode&S_IFMT)!=S_IFLNK)&&((filestat.st_mode&S_IFMT)==S_IFDIR))) 
		{

			if((strcmp(dirinfo->d_name,".")==0)||(strcmp(dirinfo->d_name,"..")==0))
				continue;
			nFileNum = GetFileNumInDir(FullName);
			nSubFolderNum	= GetSubFolderNum(FullName);
			
			nItemData = PREBROW_FILETYPE_FOLDER;
			//chdir(pFilePath);
			sprintf(FileName,"%s",dirinfo->d_name);
			sprintf(szAuxtext,"%s%d %s%d",IDS_SUBFOLDER,nSubFolderNum,IDS_PICNUM, nFileNum);
			//pLastNode = PREBROW_GetPDATA(ListBuffer, ListBuffer->nDataNum - 1);
			
			if (ListBuffer->nDataNum == 0) 
			{
				index = SendMessage(hList,LB_ADDSTRING,NULL,(LPARAM)FileName);
				SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(index,-1), (LPARAM)szAuxtext);
				SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(index,0)), (LPARAM)hFolderIcon);
				PREBROW_AddPicData(ListBuffer, dirinfo->d_name, FullName, NULL, 0);
				if (LB_ERR != index)
				{
					SendMessage(hList,LB_SETITEMDATA,(WPARAM)index,(LPARAM)nItemData);
				}
			}
			else
			{
				pLastNode = PREBROW_GetPDATA(ListBuffer, ListBuffer->nDataNum - 1);
				if (pLastNode) 
				{
					//文件家列表按字母顺序排序
					if (pLastNode->atime)
					{
						index = SendMessage(hList, LB_INSERTSTRING, pLastNode->nIndex + 1, (LPARAM)FileName);
						if (LB_ERR != index)
						{
							SendMessage(hList,LB_SETITEMDATA,(WPARAM)index,(LPARAM)nItemData);
							SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(index,0)), (LPARAM)hFolderIcon);
							SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(index,-1), (LPARAM)szAuxtext);
							PREBROW_InsertPicData(ListBuffer, index, dirinfo->d_name, FullName, NULL, 0);
						}
					}
					else
					{
						while (!pLastNode->atime) //the param hbmp for folder is Null
						{
							if (stricmp(FileName, pLastNode->szData) > 0) 
							{
								index = SendMessage(hList, LB_INSERTSTRING, pLastNode->nIndex + 1, (LPARAM)FileName);
								if (LB_ERR != index)
								{
									SendMessage(hList,LB_SETITEMDATA,(WPARAM)index,(LPARAM)nItemData);
									SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(index,0)), (LPARAM)hFolderIcon);
									SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(index,-1), (LPARAM)szAuxtext);
									PREBROW_InsertPicData(ListBuffer, index, dirinfo->d_name, FullName, NULL, 0);
								}
								break;
							}
							else
								pLastNode = pLastNode->pPre;
							if (pLastNode->atime) 
							{
								index = SendMessage(hList, LB_INSERTSTRING, pLastNode->nIndex + 1, (LPARAM)FileName);
								if (LB_ERR != index)
								{
									SendMessage(hList,LB_SETITEMDATA,(WPARAM)index,(LPARAM)nItemData);
									SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(index,0)), (LPARAM)hFolderIcon);
									SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(index,-1), (LPARAM)szAuxtext);
									PREBROW_InsertPicData(ListBuffer, index, dirinfo->d_name, FullName, NULL, 0);
								}
								break;
							}
							if (pLastNode == ListBuffer->pDataHead->pPre) 
							{
								index = SendMessage(hList, LB_INSERTSTRING, 0, (LPARAM)FileName);
								if (LB_ERR != index)
								{
									SendMessage(hList,LB_SETITEMDATA,(WPARAM)index,(LPARAM)nItemData);
									SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(index,0)), (LPARAM)hFolderIcon);
									SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(index,-1), (LPARAM)szAuxtext);
									PREBROW_InsertPicData(ListBuffer, index, dirinfo->d_name, FullName, NULL, 0);
								}
								break;
							}
						}
					}				
					/*
					index = SendMessage(hList,LB_ADDSTRING,NULL,(LPARAM)FileName);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(index,0)), (LPARAM)hFolderIcon);
					PREBROW_AddPicData(ListBuffer, dirinfo->d_name, FullName, NULL, filestat.st_mtime);
					if (LB_ERR != index)
					{
					SendMessage(hList,LB_SETITEMDATA,(WPARAM)index,(LPARAM)nItemData);
					}
					*/
				}
			}
		}
		else
		{
			if(!IsPicture(FullName, &nItemData)) 
			{
				continue;
			}
			Suffix = strrchr(dirinfo->d_name,'.');
			strncpy(FileName, dirinfo->d_name, strlen(dirinfo->d_name)- strlen(Suffix));
			FileName[strlen(FileName)]=NULL;
			
			
			//图片按时间排序
			pFirstNode = ListBuffer->pDataHead;
			if (ListBuffer->nDataNum == 0)
			{
				index = SendMessage(hList,LB_INSERTSTRING,0,(LPARAM)FileName);
				
				tmpsize = (float)filestat.st_size/1024;
				
				floattoa(tmpsize, PicSize);
				strcat(PicSize, " kB");
				SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(index,-1),(LPARAM)PicSize);
				/*-------------------------------------------------------*/
				//pLastNode = PREBROW_AddPicData(ListBuffer, pDest, FullName, hbitbmp, filestat.st_mtime);
				PREBROW_InsertPicData(ListBuffer, 0, FileName, FullName, hbitbmp, filestat.st_mtime);
				nRtnNum++;
				if (LB_ERR != index)
				{
					SendMessage(hList,LB_SETITEMDATA,(WPARAM)index,(LPARAM)nItemData);
				}
				//DeleteObject(hbiticon);
			}
			else
			{
				if (!pFirstNode) {
					continue;
				}
				if (!pFirstNode->atime) 
				{
					index = SendMessage(hList,LB_INSERTSTRING,0,(LPARAM)FileName);
					
					
					/*-------------------------------------------------------------*/
//					hbiticon = CreateBitmapFromImageFile(NULL,FullName,NULL,NULL);					
//					hbitbmp = ZoomOutBmp(hbiticon);					
//					SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,MAKEWORD(index, 0)),(LPARAM)hbitbmp);
					//SendMessage(hList, PLB_SETPICPATH, index, (LPARAM)FullName);
					//-------get the stat of current file------------------
					tmpsize = (float)filestat.st_size/1024;
					//sprintf(PicSize, "%d Kb", tmpsize);
					floattoa(tmpsize, PicSize);
					strcat(PicSize, " kB");
					SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(index,-1),(LPARAM)PicSize);
					/*-------------------------------------------------------*/
					//pLastNode = PREBROW_AddPicData(ListBuffer, pDest, FullName, hbitbmp, filestat.st_atime);
					PREBROW_InsertPicData(ListBuffer, 0, FileName, FullName, hbitbmp, filestat.st_mtime);
					nRtnNum++;
					if (LB_ERR != index)
					{
						SendMessage(hList,LB_SETITEMDATA,(WPARAM)index,(LPARAM)nItemData);
					}
					//DeleteObject(hbiticon);
				}
				else
				{
					while (pFirstNode->atime)
					{
						if (lmxDifftime(filestat.st_mtime, pFirstNode->atime) < 0)
						{							
							pFirstNode = pFirstNode->pNext;
						}
						else
						{
							index = SendMessage(hList,LB_INSERTSTRING, pFirstNode->nIndex,(LPARAM)FileName);
							
							
							/*-------------------------------------------------------------*/
//							hbiticon = CreateBitmapFromImageFile(NULL,FullName,NULL,NULL);
//							
//							hbitbmp = ZoomOutBmp(hbiticon);
//							
//							
//							SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,MAKEWORD(index, 0)),(LPARAM)hbitbmp);
							//SendMessage(hList, PLB_SETPICPATH, index, (LPARAM)FullName);
							//-------get the stat of current file------------------
							tmpsize = (float)filestat.st_size/1024;
							//sprintf(PicSize, "%d Kb", tmpsize);
							floattoa(tmpsize, PicSize);
							strcat(PicSize, " kB");
							SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(index,-1),(LPARAM)PicSize);
							/*-------------------------------------------------------*/
							//pLastNode = PREBROW_AddPicData(ListBuffer, pDest, FullName, hbitbmp, filestat.st_mtime);
							PREBROW_InsertPicData(ListBuffer, pFirstNode->nIndex, FileName, FullName, hbitbmp, filestat.st_mtime);
							nRtnNum++;
							if (LB_ERR != index)
							{
								SendMessage(hList,LB_SETITEMDATA,(WPARAM)index,(LPARAM)nItemData);
							}
							//DeleteObject(hbiticon);
							break;
						}
						if (!pFirstNode->atime) 
						{
							index = SendMessage(hList,LB_INSERTSTRING, pFirstNode->nIndex,(LPARAM)FileName);
							
							
							/*-------------------------------------------------------------*/
//							hbiticon = CreateBitmapFromImageFile(NULL,FullName,NULL,NULL);
//							
//							hbitbmp = ZoomOutBmp(hbiticon);
//							
//							
//							SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,MAKEWORD(index, 0)),(LPARAM)hbitbmp);
							//SendMessage(hList, PLB_SETPICPATH, index, (LPARAM)FullName);
							//-------get the stat of current file------------------
							tmpsize = (float)filestat.st_size/1024;
							//sprintf(PicSize, "%d Kb", tmpsize);
							floattoa(tmpsize, PicSize);
							strcat(PicSize, " kB");
							SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(index,-1),(LPARAM)PicSize);
							/*-------------------------------------------------------*/
							//pLastNode = PREBROW_AddPicData(ListBuffer, pDest, FullName, hbitbmp, filestat.st_mtime);
							PREBROW_InsertPicData(ListBuffer, pFirstNode->nIndex, FileName, FullName, hbitbmp, filestat.st_mtime);
							nRtnNum++;
							if (LB_ERR != index)
							{
								SendMessage(hList,LB_SETITEMDATA,(WPARAM)index,(LPARAM)nItemData);
							}
							//DeleteObject(hbiticon);
							break;
						}
						if (pFirstNode == ListBuffer->pDataHead) 
						{
							index = SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)FileName);							
							
							/*-------------------------------------------------------------*/
//							hbiticon = CreateBitmapFromImageFile(NULL,FullName,NULL,NULL);
//							
//							hbitbmp = ZoomOutBmp(hbiticon);
//							
//							
//							SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,MAKEWORD(index, 0)),(LPARAM)hbitbmp);
							//SendMessage(hList, PLB_SETPICPATH, index, (LPARAM)FullName);
							//-------get the stat of current file------------------
							tmpsize = (float)filestat.st_size/1024;
							//sprintf(PicSize, "%d Kb", tmpsize);
							floattoa(tmpsize, PicSize);
							strcat(PicSize, " kB");
							SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(index,-1),(LPARAM)PicSize);
							/*-------------------------------------------------------*/
							//pLastNode = PREBROW_AddPicData(ListBuffer, pDest, FullName, hbitbmp, filestat.st_mtime);
							PREBROW_AddPicData(ListBuffer, FileName, FullName, hbitbmp, filestat.st_mtime);
							nRtnNum++;
							if (LB_ERR != index)
							{
								SendMessage(hList,LB_SETITEMDATA,(WPARAM)index,(LPARAM)nItemData);
							}
							//DeleteObject(hbiticon);
							break;
						}
					}
				}
			}
			
		}
	}
	closedir(dirtemp);
/*
 *	debug sort bu

	pLastNode = ListBuffer->pDataHead;

	while (pLastNode != ListBuffer->pDataHead->pPre)
	{
		struct tm*	gtime;
		if (pLastNode->atime)
		{
			gtime = gmtime(&pLastNode->atime);
			printf("\r\n%s: %d year %d month %d day %d hour %d minuter %d seconds\r\n",pLastNode->szData, gtime->tm_year, gtime->tm_mon, gtime->tm_mday, gtime->tm_hour, gtime->tm_min, gtime->tm_sec);
		}
		pLastNode = pLastNode->pNext;
	}
 */
	chdir(path);
	loadBitmapForList(hList, 0, ListBuffer);
//	DeleteObject(hbiticon);

	return nRtnNum;
}

/*********************************************************************\
* Function	   PREBROW_AddData
* Purpose      添加数据
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
PLISTDATA	PREBROW_AddPicData (PLISTBUF pListBuf, const char* szData, const char* szFullData, HBITMAP hBmp, unsigned long atime)
{
	PLISTDATA	tempdata;
	PLISTDATA	pTail;

	if (pListBuf->nDataNum == 0)//Initialize the chain
	{
		if ( NULL == ( pListBuf->pDataHead = malloc ( sizeof (LISTDATA) )) )
			return NULL;
		if (NULL == (pListBuf->pDataHead->szData = malloc(strlen(szData)+1))) {
			return FALSE;
		}
		if (NULL == (pListBuf->pDataHead->szFullData = malloc(strlen(szFullData)+1))) {
			return FALSE;
		}

		pListBuf->pCurrentData	=	pListBuf->pDataHead;

		pListBuf->pCurrentData->nIndex	=	pListBuf->nDataNum;
		strcpy( pListBuf->pCurrentData->szData, szData );
		strcpy( pListBuf->pCurrentData->szFullData, szFullData );
		pListBuf->pCurrentData->hbmp = hBmp;
		pListBuf->pCurrentData->atime = atime;
		pListBuf->pCurrentData->pFormViewerContent = NULL;

		pListBuf->pCurrentData->pNext =	pListBuf->pDataHead;
		pListBuf->pCurrentData->pPre  = pListBuf->pDataHead;		


		pListBuf->nCurrentIndex	=	pListBuf->pCurrentData->nIndex;
		pListBuf->nDataNum++;
		return pListBuf->pDataHead;
	}
	//add node to the chain
	if (NULL == (tempdata = malloc ( sizeof(LISTDATA) ) ) )
		return NULL;	
	if (NULL == (tempdata->szData = malloc(strlen(szData)+1))) {
		return FALSE;
	}
	if (NULL == (tempdata->szFullData = malloc(strlen(szFullData)+1))) {
		return FALSE;
	}
	tempdata->nIndex = pListBuf->nDataNum;
	strcpy(tempdata->szData , szData);
	strcpy(tempdata->szFullData , szFullData);
	tempdata->hbmp = hBmp;
	tempdata->atime = atime;
	tempdata->pFormViewerContent = NULL;

	pTail = pListBuf->pDataHead->pPre;

	pTail->pNext = tempdata;
	tempdata->pNext = pListBuf->pDataHead;

	tempdata->pPre  = pTail;
	pListBuf->pDataHead->pPre = tempdata;	
	
	pListBuf->nDataNum++;

	return tempdata;
}


BOOL	PREBROW_InsertPicData (PLISTBUF pListBuf, int nIndex, char* szData, char* szFullData, HBITMAP hBmp, unsigned long atime)
{
	PLISTDATA	pInsertData;
	PLISTDATA	pRepairIndex;
	PLISTDATA	pCurData;
	int					i;

	if ((nIndex > pListBuf->nDataNum)||(nIndex < 0)) 
	{
		return FALSE;
	}
	if (nIndex == pListBuf->nDataNum) {
		if(PREBROW_AddPicData(pListBuf, szData, szFullData, hBmp, atime))
			return TRUE;
		else
			return FALSE;
	}
	//create inserted node 
	if (NULL == (pInsertData = ((PLISTDATA)malloc(sizeof(LISTDATA)))))
	{
		return FALSE;
	}
	pInsertData->szFullData = malloc(strlen(szFullData)+1);
	if (!pInsertData->szFullData) {
		return FALSE;
	}
	strcpy(pInsertData->szFullData,szFullData);
	pInsertData->szData = malloc(strlen(szData)+1);
	if (!pInsertData->szData) {
		return FALSE;
	}
	strcpy(pInsertData->szData,szData);
	pInsertData->hbmp	=	hBmp;
	pInsertData->nIndex	= nIndex;
	pInsertData->atime = atime;
	pInsertData->pFormViewerContent = NULL;

	//get the original node which index equal to nindex;
	pCurData = PREBROW_GetPDATA(pListBuf,nIndex);
	
	//insert node 
//	pCurData->pPre->pNext = pInsertData;
//	pInsertData->pPre = pCurData->pPre;
//	pCurData->pPre = pInsertData;
//	pInsertData->pNext = pCurData;

	if (nIndex == 0)
	{
		pListBuf->pDataHead = pInsertData;		
	}
	//repair index
	pRepairIndex = pCurData;
	for (i = pCurData->nIndex; i<pListBuf->nDataNum; i++)
	{
		pRepairIndex->nIndex++;
		pRepairIndex = pRepairIndex->pNext;
		if (pRepairIndex == pListBuf->pDataHead)
		{
			break;
		}
	}
	pCurData->pPre->pNext = pInsertData;
	pInsertData->pPre = pCurData->pPre;
	pCurData->pPre = pInsertData;
	pInsertData->pNext = pCurData;
	
	pListBuf->nDataNum++;
	return TRUE;
}
/*********************************************************************
 * Function Declare : ZoomOutBmp
 * Explain : 
 * Parameters : 
 * HBITMAP hBmp -- 
 * Return : 
 * static HBITMAP  -- 
 * Author : mxlin 
 * Time : 2005-07-21 19:47:19 
*********************************************************************/
static	HBITMAP	ZoomOutBmp(HBITMAP	hBmp, PCSTR pFileName)
{
	HDC	hdcSrc,hdcDst;
	HBITMAP	hBitmap;
	BITMAP  bmp;
	int		width,height;
	PSTR	pBakBmpName = NULL;
//	PSTR	p;
//	FILE	*fp;
	

	if (!hBmp)
	{
		return NULL;
	}
	GetObject(hBmp, sizeof(BITMAP), (void*)&bmp);
	if (bmp.bmHeight > bmp.bmWidth) {
		height = PICICONHEIGHT;
		width = bmp.bmWidth*PICICONHEIGHT/bmp.bmHeight;
	}
	else
	{
		width = PICICONHEIGHT;
		height = bmp.bmHeight*PICICONHEIGHT/bmp.bmWidth;
	}
	hdcDst = CreateCompatibleDC(NULL);
	hdcSrc = CreateCompatibleDC(NULL);
	hBitmap = CreateCompatibleBitmap(hdcDst, 43, 28);
	if (!hBitmap) {
		return NULL;
	}
	SelectObject(hdcSrc, hBmp);
	SelectObject(hdcDst, hBitmap);	
	//StretchBlt(hdcDst, 0, 0, width, height, hdcSrc, 0, 0, bmp.bmWidth, bmp.bmHeight, ROP_SRC);
	StretchBlt(hdcDst, 0, 0, 43, 28, hdcSrc, 0, 0, bmp.bmWidth, bmp.bmHeight, ROP_SRC);
/*
	pBakBmpName = malloc(strlen(pFileName) + 4);
	memset(pBakBmpName, 0, strlen(pFileName) + 4);
	if (pBakBmpName)
	{
		p = strrchr(pFileName, '.');
		strncpy(pBakBmpName, pFileName, p - pFileName + 1);
		//sprintf(pBakBmpName, "%stmp", pFileName);
		strcat(pBakBmpName, "bmp");
		//if ((fp = fopen(pBakBmpName, "r")) == NULL)
		{
			//fclose(fp);
			SavePictureFromHandle(hdcDst, hBitmap, pBakBmpName);
		}
		//else
			//fclose(fp);
		free(pBakBmpName);	
		pBakBmpName = NULL;
	}
*/
	DeleteDC(hdcSrc);
	DeleteDC(hdcDst);
	return hBitmap;
}

int	GetFileNumInDir(PCSTR pPath)
{
	struct dirent *dirinfo = NULL;
	struct stat	filestat;
	DIR  *dirtemp = NULL;
	int		i=0;
	PSTR	strFullName;
	int		tmp;
	dirtemp = opendir(pPath);
	if (dirtemp == NULL) {
		return 0;
	}
	dirinfo = readdir(dirtemp);
	while (dirinfo&&dirinfo->d_name[0]) 
	{
		strFullName = malloc(strlen(pPath)+strlen(dirinfo->d_name)+2);
		strcpy ( strFullName, pPath );
		if (pPath[strlen(pPath)-1] != '/') {
			strcat(strFullName,"/");
		}
		strcat ( strFullName, dirinfo->d_name);
		stat(strFullName,&filestat);
		if (!S_ISDIR(filestat.st_mode)) 
		{
			if (IsPicture(dirinfo->d_name, &tmp)) 
			{
				
				i++;
			}
		}
		free(strFullName);
		strFullName = NULL;
		dirinfo = readdir(dirtemp);
	}
	closedir(dirtemp);
	return	i;
}



BOOL	IsPicture(PSTR	pFileName, int *nItemData)
{
	char	*Suffix;
	struct stat	filestat;

	stat(pFileName,&filestat);
	if ((((filestat.st_mode&S_IFMT)!=S_IFLNK)&&((filestat.st_mode&S_IFMT)==S_IFDIR)&&(strcmp(pFileName,".")!=0)&&(strcmp(pFileName,"..")!=0))) 
	{
		*nItemData = PREBROW_FILETYPE_FOLDER;
		return TRUE;
	}
	Suffix = strrchr(pFileName, '.');
	if (!Suffix) {
		*nItemData = FILE_TYPE_UNKNOW;
		return FALSE;
	}
	if ((strcmp(Suffix, PREBROW_FILEUNIVERSE_BMP) == 0) || (strcmp(Suffix, PREBROW_FILEUNIVERSE_BMP1) == 0))
	{
		*nItemData = PREBROW_FILETYPE_BMP;
		return TRUE;
	}
	else if((strcmp(Suffix, PREBROW_FILEUNIVERSE_GIF) == 0) || (strcmp(Suffix, PREBROW_FILEUNIVERSE_GIF1) == 0))
	{
		*nItemData = PREBROW_FILETYPE_GIF;
		return TRUE;
	}
	else if((strcmp(Suffix, PREBROW_FILEUNIVERSE_JPG) == 0) || (strcmp(Suffix, PREBROW_FILEUNIVERSE_JPG1) == 0))
	{
		*nItemData = PREBROW_FILETYPE_JPG;
		return TRUE;
	}
	else if((strcmp(Suffix, PREBROW_FILEUNIVERSE_JPEG) == 0) || (strcmp(Suffix, PREBROW_FILEUNIVERSE_JPEG1) == 0))
	{
		*nItemData = PREBROW_FILETYPE_JPEG;
		return TRUE;
	}
	else if((strcmp(Suffix, PREBROW_FILEUNIVERSE_WBMP) == 0) || (strcmp(Suffix, PREBROW_FILEUNIVERSE_WBMP1) == 0))
	{
		*nItemData = PREBROW_FILETYPE_WBMP;
		return TRUE;
	}
	else if((strcmp(Suffix, PREBROW_FILEUNIVERSE_PNG) == 0) || (strcmp(Suffix, PREBROW_FILEUNIVERSE_PNG1) == 0))
	{
		*nItemData = PREBROW_FILETYPE_PNG;
		return TRUE;
	}
	else
	{
		*nItemData = FILE_TYPE_UNKNOW;
		return FALSE;
	}


}
BOOL PicBrowser_Edit(HWND	hParentWnd, PSTR szName, PSTR szFullName, BOOL bFolder)
{
	WNDCLASS	wc;
	RECT		rf;
	PSTR		ptr;
	bDir = bFolder;

	
	if (bFolder) {
		chdir(szFullName);
		//strcpy(pWorkPath, szFullName);
	}
	if (szName&&!bFolder) {
		strcpy(pFileFullName, szFullName);
		strcpy(cFileName, szName);
		ptr = strrchr(szFullName, '/');
		//strcpy(strNameCpy,ptr+1);
		//strncpy(szPath, szFullName, ptr - szFullName+1);
	}
	hEditParent	=	hParentWnd;

	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = EditNameProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = RENAMEWCLS;

	if (IsWindow(hRenameWnd)) {
		ShowWindow(hRenameWnd,SW_SHOW);
		UpdateWindow(hRenameWnd);
		return	TRUE;
	}

	if (!RegisterClass(&wc)) {
		return FALSE;
	}
	GetClientRect(hParentWnd, &rf);
	if (!bDir) 
	{
		hRenameWnd	= CreateWindow(RENAMEWCLS,IDS_RENAMEPICTITLE,
			WS_VISIBLE | WS_CAPTION|PWS_STATICBAR,
			PLX_WIN_POSITION,
			/*
			rf.left,
			rf.top,
			rf.right - rf.left,
			rf.bottom - rf.top,
			*/
			hParentWnd,NULL,NULL,NULL);
		//SetWindowText(hFrameWnd, IDS_RENAMEPICTITLE);
	}
	else
	{
		hRenameWnd	= CreateWindow(RENAMEWCLS,IDS_CREATEFOLDER,
		WS_VISIBLE | WS_CAPTION |PWS_STATICBAR,
		PLX_WIN_POSITION,
		/*
		rf.left,
		rf.top,
		rf.right - rf.left,
		rf.bottom - rf.top,
		*/
		hParentWnd,NULL,NULL,NULL);
		//SetWindowText(hFrameWnd, IDS_CREATEFOLDER);
	}
	
	if (!hRenameWnd)
	{
		return FALSE;
	}
	SendMessage(hRenameWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(1025, 1),(LPARAM)IDS_SAVE);
	SendMessage(hRenameWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(1025, 0),(LPARAM)IDS_CANCEL);
	SetFocus(hRenameWnd);
	return TRUE;
	
}

static	LRESULT	EditNameProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT	lResult;
	static	HWND	hRenameEdit = NULL;
	IMEEDIT	ie;
	RECT	rcClient;
	PSTR	ptr;
	int		txtlen,index;
	char	delprompt[PREBROW_MAXFULLNAMELEN+20] = "";
	char	*cPath = NULL;
	lResult = TRUE;
	switch(uMsg) 
	{
	case WM_CREATE:
		memset(&ie, 0, sizeof(IMEEDIT));
		ie.hwndNotify	= hWnd ;    
		ie.dwAttrib	    = 0;                
		ie.dwAscTextMax	= 0;
		ie.dwUniTextMax	= 0;
		ie.wPageMax	    = 0;        
		ie.pszCharSet	= NULL;
		ie.pszTitle	    = NULL;
		ie.pszImeName	= NULL;
		
		GetClientRect(hWnd,&rcClient);
		hRenameEdit	=	CreateWindow("IMEEDIT",NULL,
			WS_VISIBLE| WS_CHILD|ES_AUTOHSCROLL|ES_TITLE,
			rcClient.left,
			rcClient.top,
			rcClient.right - rcClient.left,
			52,//rcClient.bottom - rcClient.top,
			//52,
			hWnd,NULL,NULL,(PVOID)&ie);
		SendMessage(hRenameEdit,EM_LIMITTEXT,40,0);
		if(bDir)
		{		
			SendMessage(hRenameEdit,EM_SETTITLE,0,(LPARAM)IDS_FOLDERNAME);
			SendMessage(hRenameEdit, WM_SETTEXT, 0, (LPARAM)"New folder");
			SendMessage(hRenameEdit, EM_SETSEL, -1, -1);
		}
		else
		{
			SendMessage(hRenameEdit,EM_SETTITLE,0,(LPARAM)IDS_NEWNAME);
			SendMessage(hRenameEdit, WM_SETTEXT, 0, (LPARAM)cFileName);
			SendMessage(hRenameEdit, EM_SETSEL, -1, -1);
		}				
		break;
/*
	case PWM_SHOWWINDOW:
		if (bDir) {
			SetWindowText(hWnd,IDS_CREATEFOLDER);
		}
		else
		{
			SetWindowText(hWnd, IDS_RENAMEPICTITLE);
		}
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 1,(LPARAM)IDS_SAVE);
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 0,(LPARAM)IDS_CANCEL);
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 2,(LPARAM)"");
		break;
*/
	case WM_SETFOCUS:
		SetFocus(hRenameEdit);		
		SendMessage(hRenameEdit,EM_SETSEL, -1, -1);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_RETURN:
			{
//				SYSTEMTIME	curtime;
//				long		stime;
				char	szPath[PREBROW_MAXFULLNAMELEN] = "";
				char	szSDName[41] = "";
				PSTR	pSuffix = NULL;
				txtlen	= GetWindowTextLength(hRenameEdit);
				GetWindowText(hRenameEdit, szSDName, 41);

				if (txtlen < 1) 
				{
					PLXTipsWin(NULL, NULL, 0, IDS_RNMNOTIFY,NULL,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
					break;
				}
				else if(!IsValidFileorFolderName(szSDName))
				{
					PLXTipsWin(NULL, NULL, 0, IDS_DEFOTHERNAME, NULL,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
					break;
				}
				else
				{
					index = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
					ptr		= malloc(txtlen+1);
					GetWindowText(hRenameEdit,ptr,txtlen+1);
					memset(delprompt,0,PREBROW_MAXFULLNAMELEN+20);
					
					if (!bDir) 
					{
						cPath = strrchr(pFileFullName, '/');
						strncpy(szPath, pFileFullName, cPath - pFileFullName+1);
						if (nFileType == PREBROW_FILETYPE_FOLDER)
						{
							sprintf(delprompt,"%s%s",szPath,ptr);
						}
						else
						{
							pSuffix = strrchr(pFileFullName, '.');
							if (!pSuffix)
							{
								if (ptr)
								{
									free(ptr);
									return FALSE;
								}
							}
							sprintf(delprompt,"%s%s%s",szPath,ptr,pSuffix);

						}
						//pSuffix = strrchr(pFileFullName, '.');
						/*
						switch(nFileType)
						{
						case PREBROW_FILETYPE_BMP:
							sprintf(delprompt,"%s%s.bmp",szPath,ptr);
							break;
							
						case PREBROW_FILETYPE_WBMP:
							sprintf(delprompt,"%s%s.wbmp",szPath,ptr);
							break;
							
						case PREBROW_FILETYPE_JPG:
							sprintf(delprompt,"%s%s.jpg",szPath,ptr);
							break;
							
						case PREBROW_FILETYPE_JPEG:
							sprintf(delprompt,"%s%s.jpeg",szPath, ptr);
							break;
							
						case PREBROW_FILETYPE_GIF:
							sprintf(delprompt,"%s%s.gif",szPath, ptr);
							break;
							
						case PREBROW_FILETYPE_PNG:
							sprintf(delprompt,"%s%s.png",szPath, ptr);
							break;

						case PREBROW_FILETYPE_FOLDER:
							sprintf(delprompt,"%s%s",szPath,ptr);
							break;
						}
						*/
						if (PREBROW_FindData(&ListBuffer, delprompt) == -1) 
						{
							if(rename(pFileFullName, delprompt) == 0)
							{
								printf("rename simplex file from %s to %s successfully\n", pFileFullName, delprompt);
								SendMessage(hWndListBox, LB_SETTEXT, index, (LPARAM)ptr);
								PREBROW_ModifyData(&ListBuffer, index, ptr, delprompt);
							}
							else
								printf("failed to rename from %s   %d\n", pFileFullName, __LINE__);
						}
						else
						{
							PLXTipsWin(NULL, hWnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
							free(ptr);
							break;
						}
						
					}
					else
					{
						//getcwd(szPath, PREBROW_MAXFULLNAMELEN);
						sprintf(delprompt,"%s/%s", FolderPath, ptr);
						if (GetAvailFlashSpace() < 500)
						{
							PLXTipsWin(hFrameWnd, hWnd, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
							break;
						}
						if (PREBROW_FindData(&ListBuffer, delprompt) == -1) 
						{
							if(mkdir(delprompt,0666) == 0)
							{
								printf("Create folder: %s, successfully\n", delprompt);
								GetMMCName(szSDName);
								if (stricmp(szSDName, ListBuffer.pDataHead->pPre->szData) == 0)
								{
									index = SendMessage(hWndListBox, LB_INSERTSTRING, ListBuffer.nDataNum - 1, (LPARAM)ptr);
									SendMessage(hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(index, 0)), (LPARAM)hFolderBmp);
									SendMessage(hWndListBox, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)"Folder: 0 Pictures: 0");
									SendMessage(hWndListBox, LB_SETITEMDATA, index, (LPARAM)PREBROW_FILETYPE_FOLDER);
									//SendMessage(hWndListBox, LB_SETCURSEL, index, 0);
									//GetLocalTime(&curtime);
									//LOG_STtoFT(&curtime, &stime);
									PREBROW_InsertPicData(&ListBuffer, index, ptr,delprompt, NULL, 0);
									SendMessage(hWndListBox, LB_SETCURSEL, index, 0);
									//PREBROW_AddPicData(&ListBuffer, ptr, delprompt,NULL,0);
								}
								else
								{
									index = SendMessage(hWndListBox, LB_INSERTSTRING, ListBuffer.nDataNum, (LPARAM)ptr);
									SendMessage(hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(index, 0)), (LPARAM)hFolderBmp);
									SendMessage(hWndListBox, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)"Folder: 0 Pictures: 0");
									SendMessage(hWndListBox, LB_SETITEMDATA, index, (LPARAM)PREBROW_FILETYPE_FOLDER);
									//SendMessage(hWndListBox, LB_SETCURSEL, index, 0);
									//GetLocalTime(&curtime);
									//LOG_STtoFT(&curtime, &stime);
									PREBROW_InsertPicData(&ListBuffer, index, ptr,delprompt, NULL, 0);
									SendMessage(hWndListBox, LB_SETCURSEL, index, 0);
									//PREBROW_AddPicData(&ListBuffer, ptr, delprompt,NULL,0);
								}
							}
							else
								printf("failed to create folder: %s \n", delprompt);
						}
						else
						{
							PLXTipsWin(NULL, hWnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
							free(ptr);
							ptr = NULL;
							break;
						}
					}				
					if (ptr)
					{
						free(ptr);
					}					
					PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					PostMessage(hWnd,WM_CLOSE,0,0);
					//PostMessage(hEditParent, IDRM_CLOSEWINDOW, 0, 0);
					break;
				}
			}
			break;

		case VK_F10:
			cFileName[0] = NULL;
			PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			PostMessage(hWnd,WM_CLOSE,0,0);
			break;

		default:
			lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		UnregisterClass(RENAMEWCLS,NULL);
		break;
//
//	case WM_DESTROY:
//		UnregisterClass(RENAMEWCLS,NULL);
//		break;

	default:
		lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
		break;
	}
	return lResult;
}
static	BOOL	RegisteMovetoFolder()
{
	WNDCLASS wc;
	
	wc.style            = NULL;
	wc.lpfnWndProc      = MoveToFolderWndProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = sizeof(MOVEDATA);
	wc.hInstance        = NULL;
	wc.hIcon            = NULL;
	wc.hCursor          = NULL;//LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = MOVETOFOLDER;
	
	if (!RegisterClass(&wc))
		return FALSE;
	return TRUE;
}
static	BOOL	RegisteSelectDestination()
{
	WNDCLASS wc;
	
	wc.style            = NULL;
	wc.lpfnWndProc      = SelectDestinationWndProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = sizeof(MOVEDATA);
	wc.hInstance        = NULL;
	wc.hIcon            = NULL;
	wc.hCursor          = NULL;//LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = "SELECTDESTINATIONWC";
	
	if (!RegisterClass(&wc))
		return FALSE;
	return TRUE;
}

static	LRESULT CALLBACK SelectDestinationWndProc (HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
	LRESULT	lResult;
	RECT	rc;
	int		nIndex,nSpace;
	PMOVEDATA	pmd;
	PCREATESTRUCT	pcs;
	char	AvailSpace[32] = "";
	pmd = GetUserData(hWnd);

	lResult = TRUE;
	switch(wMsgCmd) 
	{
	case WM_CREATE:
		{
			pcs = (PCREATESTRUCT)lParam;
			hPhoneIcon = LoadImage(NULL, PHONEICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			hMMCIcon = LoadImage(NULL, MMCICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			memcpy(pmd, (PMOVEDATA)pcs->lpCreateParams, sizeof(MOVEDATA));			
			GetClientRect(hWnd, &rc);
			hMemDeviceList = CreateWindow("LISTBOX", NULL, 
				WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_BITMAP|LBS_MULTILINE,
				rc.left, rc.top,
				rc.right - rc.left,
				rc.bottom - rc.top,
				hWnd, NULL, NULL,NULL);
			nIndex = SendMessage(hMemDeviceList, LB_ADDSTRING, 0, (LPARAM)IDS_PHONEMEM);
			nSpace = GetAvailFlashSpace();
			//floattoa((float)nSpace, AvailSpace);
			sprintf(AvailSpace, "%d kB", nSpace);
			SendMessage(hMemDeviceList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(nIndex, 0)), (LPARAM)hPhoneIcon);
			SendMessage(hMemDeviceList, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)AvailSpace);
			nIndex = SendMessage(hMemDeviceList, LB_ADDSTRING, 0, (LPARAM)IDS_MMCMEM);
			SendMessage(hMemDeviceList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(nIndex, 0)), (LPARAM)hMMCIcon);
			nSpace = GetAvailMMCSpace();
			//floattoa((float)nSpace, AvailSpace);
			sprintf(AvailSpace, "%d kB", nSpace);
			SendMessage(hMemDeviceList, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)AvailSpace);
			SendMessage(hMemDeviceList, LB_SETCURSEL, 0, 0);
		}
		break;

	case WM_SETFOCUS:
		SetFocus(hMemDeviceList);
		break;



	case WM_KEYDOWN:
		switch(wParam) 
		{
		case VK_F5:
			{				
				nIndex = SendMessage(hMemDeviceList, LB_GETCURSEL, 0, 0);
				if (nIndex == 0) {
					ListFolder(pmd->hRecMsg, pmd->rmsg, TRUE);
				}
				if (nIndex == 1) {
					ListFolder(pmd->hRecMsg, pmd->rmsg, FALSE);
				}
				DestroyWindow(hWnd);				
			}	
			break;

		case VK_F10:
			SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_DESTROY:	
		if (hMMCIcon) {
			DeleteObject(hMMCIcon);
		}
		if (hPhoneIcon) {
			DeleteObject(hPhoneIcon);
		}
		UnregisterClass("SELECTDESTINATIONWC", NULL);
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lResult;
}

BOOL	PicMoveToFolder(HWND hRecmsg, UINT rMsg)
{
	HWND	hSelDest;
	MOVEDATA	md;

	memset(&md, 0, sizeof(MOVEDATA));
	md.hRecMsg = hRecmsg;
	md.rmsg = rMsg;
	//md.bPhonePath = bPhone;
	if (MMC_CheckCardStatus()) 
	{
		RegisteSelectDestination();
		hSelDest = CreateWindow("SELECTDESTINATIONWC", IDS_SELDEST,
		WS_CAPTION|WS_VISIBLE|PWS_STATICBAR,
		PLX_WIN_POSITION,		
		hRecmsg,
		NULL,
		NULL,
		(PVOID)&md);
		if (!hSelDest) {
			return FALSE;
		}
		SendMessage(hSelDest, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hSelDest, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(1025,0), (LPARAM)IDS_CANCEL);	
		SetFocus(hSelDest);
	}
	else
	{
		ListFolder(hRecmsg, rMsg, TRUE);
	}
	return TRUE;
}

BOOL	ListFolder(HWND hRecmsg, UINT rMsg, BOOL bPhone)
{
	RECT	rf;
	MOVEDATA	md;
	HWND	hMoveToFolder = NULL;
	int		nIndex;

	md.hRecMsg = hRecmsg;
	md.rmsg = rMsg;
	md.bPhonePath = bPhone;
	
	GetClientRect(hFrameWnd, &rf);
	RegisteMovetoFolder();
	hMoveToFolder = CreateWindow(MOVETOFOLDER, IDS_SELECTFOLDER,
		WS_CAPTION|WS_VISIBLE|PWS_STATICBAR,
		PLX_WIN_POSITION,		
		hRecmsg,
		NULL,
		NULL,
		(PVOID)&md);
	if (!hMoveToFolder) {
		return FALSE;
	}
	SendMessage(hMoveToFolder, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	SendMessage(hMoveToFolder, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(1025,0), (LPARAM)IDS_CANCEL);

	nIndex = SendMessage(hFolderList, LB_GETCURSEL, 0, 0);
	if (nIndex != 0)
	{
		SendMessage(hMoveToFolder, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(1025,1), (LPARAM)"");
		SendMessage(hMoveToFolder, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	}
	else
	{
		switch(rMsg) 
		{
		case IDRM_COPYTOFOLER:
		case IDRM_COPYALL:
			SendMessage(hMoveToFolder, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(1025,1), (LPARAM)IDS_COPY);
			break;
			
		case IDRM_MOTOFOLDER:
		case IDRM_MOVEALL:
			SendMessage(hMoveToFolder, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(1025,1), (LPARAM)IDS_MOVE);
			break;			
		}
		SendMessage(hMoveToFolder, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
	}
	SetFocus(hMoveToFolder);
	return TRUE;
}

static	LRESULT CALLBACK MoveToFolderWndProc (HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
	LRESULT	lResult;
	RECT	rc;
	int		nIndex;
	static	LISTBUF	FolderListbuf;
	PMOVEDATA	pmd;
	PCREATESTRUCT	pcs;
	pmd = GetUserData(hWnd);

	lResult = TRUE;
	switch(wMsgCmd) 
	{
	case WM_CREATE:
		{
			pcs = (PCREATESTRUCT)lParam;
			memcpy(pmd, (PMOVEDATA)pcs->lpCreateParams, sizeof(MOVEDATA));
			
			memset(&FolderListbuf, 0, sizeof(LISTBUF));
			GetClientRect(hWnd, &rc);
			hFolderList = CreateWindow("LISTBOX", NULL, WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_BITMAP,
				rc.left, rc.top,
				rc.right - rc.left,
				rc.bottom - rc.top,
				hWnd, NULL, NULL,NULL);
			SendMessage(hFolderList, LB_RESETCONTENT, 0, 0);
			PREBROW_InitListBuf(&FolderListbuf);
			//nIndex = SendMessage(hFolderList, LB_ADDSTRING, 0, (LPARAM)"Top level");
			
			//SendMessage(hFolderList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(nIndex, 0)), (LPARAM)hFolderBmp);
			if (pmd->bPhonePath) {
				//PREBROW_AddData(&FolderListbuf, "Top level", PHONEPICFILEDIR);
				strcpy(pmd->szCurPath, PHONEPICFILEDIR);
				AddFolderToList(hFolderList, PHONEPICFILEDIR, hOpenFolder, hSubFolder,&FolderListbuf);
				
			}
			else
			{
				//PREBROW_AddData(&FolderListbuf, "Top level", MMCPICFOLDERPATH);
				strcpy(pmd->szCurPath, MMCPICFOLDERPATH);
				AddFolderToList(hFolderList, MMCPICFOLDERPATH, hOpenFolder, hSubFolder, &FolderListbuf);
				
			}
			
			SendMessage(hFolderList, LB_SETCURSEL, 0, 0);
		}
		break;
		
	case PWM_SHOWWINDOW:
		
		nIndex = SendMessage(hFolderList, LB_GETCURSEL, 0, 0);
		if (nIndex != 0)
		{
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		}
		else
		{
			switch(pmd->rmsg) 
			{
			case IDRM_COPYTOFOLER:
			case IDRM_COPYALL:
				SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_COPY);
				break;
				
			case IDRM_MOTOFOLDER:
			case IDRM_MOVEALL:
				SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_MOVE);
				break;			
			}
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		}
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		//SetWindowText(hWnd, IDS_MOVETOFOLDER);
		//SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), NULL);
		//SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), NULL);
		SetFocus(hWnd);
		break;

	case WM_SETFOCUS:
		SetFocus(hFolderList);
		break;

	case WM_COMMAND:
		switch(HIWORD(wParam)) 
		{
		case LBN_SELCHANGE:
			
			nIndex = SendMessage(hFolderList, LB_GETCURSEL, 0, 0);
			if (nIndex != 0)
			{
				SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			}
			else
			{
				switch(pmd->rmsg) 
				{
				case IDRM_COPYTOFOLER:
				case IDRM_COPYALL:
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_COPY);
					break;
					
				case IDRM_MOTOFOLDER:
				case IDRM_MOVEALL:
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_MOVE);
					break;			
				}
				SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			}
			break;
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;
/*
	case WM_TIMER:
		if (wParam == 1) 		
		{
			char	FileName[100];
			char	path[80];
			KillTimer(hWnd, 1);
			nIndex = SendMessage(hFolderList, LB_GETCURSEL, 0, 0);
			PREBROW_GetData(&FolderListbuf, nIndex, FileName, path);
			SendMessage(pmd->hRecMsg, pmd->rmsg, 0, (LPARAM)path);
			SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			DestroyWindow(hWnd);
		}
		else
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
*/
	case WM_KEYDOWN:
		switch(wParam) 
		{
		case VK_RETURN:
			{
				char	FileName[100] = "";
				char	path[1024] = "";
				nIndex = SendMessage(hFolderList, LB_GETCURSEL, 0, 0);
				if (nIndex == 0)
				{
					PREBROW_GetData(&FolderListbuf, nIndex, FileName, path);
					SendMessage(pmd->hRecMsg, pmd->rmsg, 0, (LPARAM)path);
					//SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					DestroyWindow(hWnd);
				}
			}
			break;

		case VK_F5:
			{
//				SetTimer(hWnd, 1, 300, NULL);
//				nIndex = SendMessage(hFolderList, LB_GETCURSEL, 0, 0);
//				SendMessage(hFolderList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nIndex), (LPARAM)hRBonbmp);				
				char	FileName[100];
				char	path[80];
				nIndex = SendMessage(hFolderList, LB_GETCURSEL, 0, 0);
				PREBROW_GetData(&FolderListbuf, nIndex, FileName, path);
				//SendMessage(pmd->hRecMsg, pmd->rmsg, 0, (LPARAM)path);
				//SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				//DestroyWindow(hWnd);
				strcpy(pmd->szCurPath, path);
				AddFolderToList(hFolderList, path, hOpenFolder, hSubFolder, &FolderListbuf);
				SendMessage(hFolderList, LB_SETCURSEL, 0, 0);
				switch(pmd->rmsg) 
				{
				case IDRM_COPYTOFOLER:
				case IDRM_COPYALL:
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_COPY);
					break;
					
				case IDRM_MOTOFOLDER:
				case IDRM_MOVEALL:
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_MOVE);
					break;			
				}
				SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			}	
			break;

		case VK_F10:
			{
				//char	szCurpath[1025] = "";
				PSTR	pLasDir = NULL;
				//getcwd(szCurpath, 1024);
				
				if (pmd->szCurPath[strlen(pmd->szCurPath) -1] == '/')
				{
					pmd->szCurPath[strlen(pmd->szCurPath) -1] = 0;
				}

				if (stricmp(pmd->szCurPath, PHONEPICFILEDIR) == 0)
				{
					DestroyWindow(hWnd);
					break;
				}
				else if (stricmp(pmd->szCurPath, MMCPICFOLDERPATH) == 0)
				{
					strcpy(pmd->szCurPath, PHONEPICFILEDIR);	
				}
				else
				{
					pLasDir = strrchr(pmd->szCurPath, '/');
					pmd->szCurPath[pLasDir - pmd->szCurPath] = 0;
				}
				
				AddFolderToList(hFolderList, pmd->szCurPath, hOpenFolder, hSubFolder, &FolderListbuf);
				SendMessage(hFolderList, LB_SETCURSEL, 0, 0);
				switch(pmd->rmsg) 
				{
				case IDRM_COPYTOFOLER:
				case IDRM_COPYALL:
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_COPY);
					break;
					
				case IDRM_MOTOFOLDER:
				case IDRM_MOVEALL:
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_MOVE);
					break;			
				}
				SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				//SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				//DestroyWindow(hWnd);
			}
			break;
		}
		break;

	case WM_DESTROY:
		
		UnregisterClass(MOVETOFOLDER, NULL);
		PREBROW_FreeListBuf(&FolderListbuf);
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lResult;
}
BOOL	RealtimeUpdatePicture()
{
	return TRUE;
	/*
	if (IsWindow(hWndListBox)) 
	{
		bUpdated = TRUE;
		SendMessage(hMainView, PWM_SHOWWINDOW, 0, 0);
		return TRUE;
	}
	return FALSE;
	*/
}

long	lmxDifftime(unsigned long tm1, unsigned long tm2)
{
	struct tm *StTime1, *StTime2;

	StTime1 = gmtime(&tm1);
	StTime2 = gmtime(&tm2);
	if (StTime1->tm_year != StTime2->tm_year)
	{
		return StTime1->tm_year - StTime2->tm_year;
	}
	if (StTime1->tm_mon != StTime2->tm_mon)
	{
		return StTime1->tm_mon - StTime2->tm_mon;
	}
	if (StTime1->tm_mday != StTime2->tm_mday)
	{
		return StTime1->tm_mday - StTime2->tm_mday;
	}
	if (StTime1->tm_hour != StTime2->tm_hour)
	{
		return StTime1->tm_hour - StTime2->tm_hour;
	}
	if (StTime1->tm_min != StTime2->tm_min)
	{
		return StTime1->tm_min - StTime2->tm_min;
	}
	if (StTime1->tm_sec != StTime2->tm_sec)
	{
		return StTime1->tm_sec - StTime2->tm_sec;
	}
	else
		return	tm1 - tm2;
}

static	BOOL	RegisterWallpaper()
{
	WNDCLASS wc;
	
	wc.style            = NULL;
	wc.lpfnWndProc      = SetWallpaperProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = 0;
	wc.hInstance        = NULL;
	wc.hIcon            = NULL;
	wc.hCursor          = NULL;//LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = "SETWALLPAPERWC";
	
	if (!RegisterClass(&wc))
		return FALSE;
	return TRUE;
}

BOOL	PicSetWallPaper(HWND hWnd)
{
	HWND	hwall;
	RegisterWallpaper();
		hwall = CreateWindow("SETWALLPAPERWC", IDS_WALLPAPER,
		WS_CAPTION|WS_VISIBLE|PWS_STATICBAR,
		PLX_WIN_POSITION,		
		hWnd,
		NULL,
		NULL,
		NULL);
	if (!hwall) {
		return FALSE;
	}
	SendMessage(hwall, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	SendMessage(hwall, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(1025,0), (LPARAM)IDS_CANCEL);	
	SetFocus(hwall);
	return TRUE;
}

static	LRESULT CALLBACK SetWallpaperProc (HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
	LRESULT	lResult;
	RECT	rc;
	int		nIndex;

	lResult = TRUE;
	switch(wMsgCmd) 
	{
	case WM_CREATE:
		{
			GetClientRect(hWnd, &rc);
			hWallPaperList = CreateWindow("LISTBOX", NULL, 
				WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_BITMAP,
				rc.left, rc.top,
				rc.right - rc.left,
				rc.bottom - rc.top,
				hWnd, NULL, NULL,NULL);
			nIndex = SendMessage(hWallPaperList, LB_ADDSTRING, 0, (LPARAM)IDS_SET);
			SendMessage(hWallPaperList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(nIndex, 0)), (LPARAM)hRBoffbmp);
			nIndex = SendMessage(hWallPaperList, LB_ADDSTRING, 0, (LPARAM)IDS_CLEAR);
			SendMessage(hWallPaperList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(nIndex, 0)), (LPARAM)hRBoffbmp);
			SendMessage(hWallPaperList, LB_SETCURSEL, 0, 0);
			
		}
		break;

	case WM_SETFOCUS:
		SetFocus(hWallPaperList);
		break;

/*
	case WM_COMMAND:
		if (HIWORD(wParam) == LBN_SELCHANGE) 
		{
			nIndex = SendMessage(hWallPaperList, LB_GETCURSEL, 0, 0);
			if (nIndex == 0) {
				SendMessage(hWallPaperList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(0, 0)), (LPARAM)hRBonbmp);
				SendMessage(hWallPaperList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(1, 0)), (LPARAM)hRBoffbmp);
			}
			if (nIndex == 1) {
				SendMessage(hWallPaperList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(1, 0)), (LPARAM)hRBonbmp);
				SendMessage(hWallPaperList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(0, 0)), (LPARAM)hRBoffbmp);
			}			
		}
		else
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
*/
	case WM_TIMER:
		switch(wParam) 
		{
		case IDT_RADIOBUTTON:
			KillTimer(hWnd, IDT_RADIOBUTTON);
			nIndex = SendMessage(hWallPaperList, LB_GETCURSEL, 0, 0);
			if (nIndex == 0) {
				SendMessage(hWallPaperList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(0, 0)), (LPARAM)hRBonbmp);
				SendMessage(hWallPaperList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(1, 0)), (LPARAM)hRBoffbmp);
				SendMessage(hMainView, IDC_SET, 0, 0);
			}
			if (nIndex == 1) {
				SendMessage(hWallPaperList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(1, 0)), (LPARAM)hRBonbmp);
				SendMessage(hWallPaperList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(0, 0)), (LPARAM)hRBoffbmp);
				SendMessage(hMainView, IDC_CLEAR, 0, 0);
			}
			DestroyWindow(hWnd);

			break;
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;

	case WM_KEYDOWN:
		switch(wParam) 
		{
		case VK_F5:
			{
				SetTimer(hWnd, IDT_RADIOBUTTON, RBTIMEOUT, NULL);
				/*
				nIndex = SendMessage(hWallPaperList, LB_GETCURSEL, 0, 0);
				if (nIndex == 0) {
					SendMessage(hMainView, IDC_SET, 0, 0);
				}
				if (nIndex == 1) {
					SendMessage(hMainView, IDC_CLEAR, 0, 0);
				}
				DestroyWindow(hWnd);
				*/
			}	
			break;

		case VK_F10:
			SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_DESTROY:		
		UnregisterClass("SETWALLPAPERWC", NULL);
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lResult;
}
BOOL	loadBitmapForList(HWND hPicList, int nPos, PLISTBUF pbuffer)
{
	int	nTopIndex, i, nCount;
	HBITMAP	/*hbiticon, hbitbmp,*/ hBitmap = NULL;
	PLISTDATA	pNode;
	DWORD		Filetype;
	//PSTR		pBakBmpName;

	nCount	= SendMessage(hPicList, LB_GETCOUNT, 0, 0);
	if (nCount >= 3)
	{
		nCount = 3;
	}
	nTopIndex = SendMessage(hPicList, LB_GETTOPINDEX, 0, 0);
	for (i = nTopIndex; i<nTopIndex+nCount; i++)
	{
		Filetype = SendMessage(hPicList, LB_GETITEMDATA, i, 0);
		if (Filetype == LB_ERR)
		{
			break;
		}
		if ((Filetype == PREBROW_FILETYPE_FOLDER)||(Filetype == PREBROW_FILETYPE_MEMEORY))
		{
			continue;
		}

		hBitmap = (HBITMAP)SendMessage(hPicList, LB_GETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(i, nPos)), 0);
		
		if (!hBitmap)
		{
			pNode = PREBROW_GetPDATA(pbuffer, i);
			if (!pNode)
			{
				return	FALSE;
			}
			/*
			hbiticon = CreateBitmapFromImageFile(NULL,pNode->szFullData,NULL, NULL);
			
			if (!hbiticon)
			{
				hbiticon = CreateBitmapFromImageFile(NULL, DEFAULTPICTURE, NULL, NULL);
			}
			hbitbmp = ZoomOutBmp(hbiticon, pNode->szFullData);	
			*/
			SendMessage(hPicList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,MAKEWORD(i, nPos)),(LPARAM)hSpareMiniature);
			
			/*
			pNode->hbmp = hbitbmp;
						
			DeleteObject(hbiticon);
			*/
		}
	}
	PostMessage(GetParent(hPicList), PIC_LOADBMP, MAKEWPARAM(nTopIndex, nPos), 0);
	return TRUE;
}
BOOL	Image_LoadMiniature(int	nTopindex, int nPos, HWND hPicList, PLISTBUF pbuffer)
{
//	int	nTop, nMiniPos;
	int	i, nCount;

	HBITMAP	hbiticon = NULL, hbitbmp =NULL, hBitmap = NULL;
	PLISTDATA	pNode;
	DWORD		Filetype;

	nCount	= SendMessage(hPicList, LB_GETCOUNT, 0, 0);
	if (nCount > 3)
	{
		nCount = 3;
	}

	for (i = nTopindex; i<nTopindex+nCount; i++ )
	{
		Filetype = SendMessage(hPicList, LB_GETITEMDATA, i, 0);
		if (Filetype == LB_ERR)
		{
			break;
		}
		if ((Filetype == PREBROW_FILETYPE_FOLDER)||(Filetype == PREBROW_FILETYPE_MEMEORY))
		{
			continue;
		}

		hBitmap = (HBITMAP)SendMessage(hPicList, LB_GETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(i, nPos)), 0);
		
		if (!hBitmap ||(hBitmap == hSpareMiniature))
		{
			pNode = PREBROW_GetPDATA(pbuffer, i);
			if (!pNode)
			{
				return	FALSE;
			}
			
			hbiticon = CreateBitmapFromImageFile(NULL,pNode->szFullData,NULL, NULL);
			
			if (!hbiticon)
			{
				hbiticon = CreateBitmapFromImageFile(NULL, DEFAULTPICTURE, NULL, NULL);
			}
			hbitbmp = ZoomOutBmp(hbiticon, pNode->szFullData);	
			
			SendMessage(hPicList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,MAKEWORD(i, nPos)),(LPARAM)hbitbmp);
			
			
			pNode->hbmp = hbitbmp;
						
			DeleteObject(hbiticon);
			
		}
	}
	return	TRUE;
}

BOOL	SavePictureFromHandle(HDC hdc, HBITMAP hBmp, PSTR pFileName)
{
	BITMAP bmp;
	BITMAPFILEHEADER	BmpFileHeader;
	BITMAPINFO			Bmpinfo;
	BYTE*				pBits = NULL;
	FILE*				fp;
	int					nBmpSize;

	memset(&bmp, 0, sizeof(BITMAP));
	memset(&BmpFileHeader, 0, sizeof(BITMAPFILEHEADER));	
	memset(&Bmpinfo, 0, sizeof(BITMAPINFO));

	GetObject(hBmp, sizeof(BITMAP), &bmp);
	//init	file header
	*((char *)&BmpFileHeader.bfType) = 'B';
	*(((char *)&BmpFileHeader.bfType)+1) = 'M';
	BmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	nBmpSize = (bmp.bmWidth * bmp.bmHeight) * bmp.bmBitsPixel / 8;
	BmpFileHeader.bfSize = BmpFileHeader.bfOffBits + nBmpSize;

	pBits = (BYTE *)malloc((bmp.bmWidth * bmp.bmHeight) * bmp.bmBitsPixel / 8);
	if (!pBits)
	{
		return FALSE;
	}
	Bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    Bmpinfo.bmiHeader.biWidth = bmp.bmWidth;
    Bmpinfo.bmiHeader.biHeight = bmp.bmHeight;
    Bmpinfo.bmiHeader.biPlanes = 1;
    Bmpinfo.bmiHeader.biBitCount = bmp.bmBitsPixel;
    Bmpinfo.bmiHeader.biCompression = BI_RGB;
    Bmpinfo.bmiHeader.biSizeImage = nBmpSize;
    Bmpinfo.bmiHeader.biXPelsPerMeter = 0;
    Bmpinfo.bmiHeader.biYPelsPerMeter = 0;
    Bmpinfo.bmiHeader.biClrUsed = 0;
    Bmpinfo.bmiHeader.biClrImportant = 0;

	if(!GetDIBits(NULL, hBmp, 0, bmp.bmHeight, pBits, &Bmpinfo, 0))
		return FALSE;
	
	fp = fopen(pFileName, "w+");
	if (!fp)
	{
		return FALSE;
	}
	fwrite(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&Bmpinfo.bmiHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(pBits, nBmpSize, 1, fp);

	free(pBits);
	fclose(fp);
	return TRUE;
}
VOID	GetLastPath(PSTR	szPath, PSTR pNewPath)
{
	PSTR	p;
	
	if (szPath[strlen(szPath) - 1] == '/')
	{
		szPath[strlen(szPath) - 1] = 0;
	}
	p = strrchr(szPath, '/');
	strncpy(pNewPath, szPath, p-szPath);

	//szPath[p - szPath] = 0;
}
BOOL	IsValidFileorFolderName(char * fName)
{
	int		i;
	if (!fName)
	{
		return FALSE;
	}
	if (strlen(fName) > 40)
	{
		return FALSE;
	}
	if ((*fName == 0)||(*fName == 0x20)||(fName[strlen(fName) -1] == 0x20))
	{
		return FALSE;
	}
	for (i = 0; i< (int)strlen(fName); i++)
	{
		if (( *(fName + i) >0)&&(*(fName + i) <= 0x2f))
		{
			if ((*(fName + i) ==0x20)||(*(fName + i) ==0x11)||(*(fName + i) ==0x2d))
			;
			else
				return FALSE;
			
		}
	}
	return TRUE;
}
