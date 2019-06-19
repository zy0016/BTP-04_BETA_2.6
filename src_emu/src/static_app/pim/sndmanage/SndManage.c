/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Pim
 *
 * Purpose  : Sound Application
 *            
\**************************************************************************/
#define __MODULE__ "SOUNDAPP"
#define	debugbylmx
#include "SndManage.h"
#include "plxdebug.h"

#define	RENAMEWCLS	"SNDRENAMEWNDCLASS"
#define	MOVETOFOLDER	"MOVETOFOLDERWNDCLASS"
#define	SNDICONHEIGHT	20

#define ISMULTISOUNDS(h) (SND_CheckSingalSound(h)>1)

typedef	struct tagMOVEDATA
{
	HWND	hRecMsg;
	UINT	rmsg;
}MOVEDATA,* PMOVEDATA;

typedef struct tagSNDEDITDATA
{
	short nEditType;
	char szName[64];
}SNDEDITDATA, *PSNDEDITDATA;

/*************************************************
	全局变量
*************************************************/
/*
 *	window handle
 */
static	HINSTANCE	hInstance;
static	BOOL	bSndManageWndClass = FALSE;

static	HWND	hFrameWnd = NULL;
static	HWND	hMainView = NULL;
static	HWND	hRenameWnd = NULL;
static	HWND	hWndListBox = NULL;

static	HWND	hMoveToFolder = NULL;
static	HWND	hFolderList = NULL;

static	HMENU	hSoundMenu = NULL;	// hSoundMenu == hViewMenu or hSingalViewMenu
static	HMENU	hViewMenu = NULL;
static	HMENU	hSingalViewMenu = NULL;
static	HMENU	hDelManyMenu	=	NULL;
static	HMENU	hMoveManyMenu	=	NULL;
static	HMENU	hCopyManyMenu	=	NULL;
static	HMENU	hFolderHLMenu	=	NULL;
//static	HBITMAP	hRBoffbmp = NULL;
//static	HBITMAP	hRBonbmp = NULL;
static	BOOL	bUpdated = FALSE;

static	char szCurPath[512] = "";
static	char szDstPath[512] = "";

/*
 *	variable
 */
static	char	FolderPath[MAXFULLNAMELEN] = "";
static	char	cFileName[MAXFULLNAMELEN] = "";
static	char	pFileFullName[MAXFULLNAMELEN] = "";

static	int		nFileType;

static	LISTBUF	ListBuffer;

static char *pPlayingItem = NULL;
static int   nPlayingIndex = -1;

static	HBITMAP	hPhoneBmp = NULL;
static	HBITMAP	hMMCBmp = NULL;
static	HBITMAP	hFolderBmp = NULL;
static	HBITMAP hOpenDirBmp = NULL;
static	HBITMAP hIconStatic = NULL;
static	HBITMAP hIconPlaying = NULL;

static	BOOL	bDir;
static	BOOL	bFolderOpen = FALSE;
static	BOOL	bSendAll = FALSE;



/*************************************************
	函数声明
*************************************************/
static	LRESULT	EditSndNameProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
static	BOOL	PreviewSound(HWND hWnd);
static	LRESULT CALLBACK BrowserSndWndProc (HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
static	LRESULT CALLBACK MoveToFolderWndProc (HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
int	AddSoundsToList (HWND hList,PCSTR pPath, PLISTBUF pltb, BOOL bShowSubDir);
extern	void	LOG_STtoFT(SYSTEMTIME* pSysTime, unsigned long* pTime);

static void SND_InitMenus(void);
static BOOL SND_SetNCArea(HWND hFrameWnd, HWND hWndListBox, const char *szCurrentPath);

static int SND_CheckSingalSound(HWND hListBox);
static void SND_UpdateDirInfo(HWND hListBox, LISTBUF *pListBuf, PCSTR szPath);

static void SND_OnDestroy(void);
static LRESULT SND_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

static BOOL LoadFolderList(HWND hFrame, HWND hListBox, PLISTBUF pFolderListbuf, char *szPath, UINT wMsg);
static int LoadListBoxFromBuffer(HWND hList, LISTBUF* ListBuffer);

static	int	LoadMainViewList(const char *szPath);
extern BOOL	MMC_CheckCardStatus();

/*
 *	menu template
 */

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

static MENUITEMTEMPLATE MainviewMenu[] =
{
	{ MF_STRING, IDC_PLAY, "Play", NULL},
	{ MF_STRING, IDC_RENAME, "Rename", NULL},
	{ MF_STRING, IDC_CREATEFOLDER, "Create folder", NULL},
	{ MF_STRING, IDC_MOVETOFOLDER, "Move to folder", NULL},
	{ MF_STRING|MF_POPUP, 0, "Move many...", MoveManyMenu},
	{ MF_STRING, IDC_CPTOFOLDER, "Copy", NULL},
	{ MF_STRING|MF_POPUP, 0, "Copy many...", CopyManyMenu},
	{ MF_STRING, IDC_SHOWDETAIL, "Details", NULL},	
	{ MF_STRING, IDC_DELETE, "Delete", NULL},
	{ MF_STRING|MF_POPUP, 0, "Delete many...", DelManyMenu},
    { MF_END, 0, NULL, NULL }
};

static MENUITEMTEMPLATE MainSingalviewMenu[] =	// shown when there's fewer than one sound
{
	{ MF_STRING, IDC_PLAY, "Play", NULL},
	{ MF_STRING, IDC_RENAME, "Rename", NULL},
	{ MF_STRING, IDC_CREATEFOLDER, "Create folder", NULL},
	{ MF_STRING, IDC_MOVETOFOLDER, "Move to folder", NULL},
	{ MF_STRING, IDC_CPTOFOLDER, "Copy", NULL},
	{ MF_STRING, IDC_SHOWDETAIL, "Details", NULL},	
	{ MF_STRING, IDC_DELETE, "Delete", NULL},
    { MF_END, 0, NULL, NULL }
};

//*
static MENUITEMTEMPLATE FolderHighLightMenu[] =
{
	{ MF_STRING, IDC_PLAY, "Open", NULL},
	{ MF_STRING, IDC_RENAME, "Rename", NULL},
	{ MF_STRING, IDC_CREATEFOLDER, "Create folder", NULL},
	{ MF_STRING, IDC_DELETE, "Delete", NULL},
    { MF_END, 0, NULL, NULL }
};
//*/

static const MENUTEMPLATE MainViewListMenuTemplate =
{
    0,
    MainviewMenu
};

static const MENUTEMPLATE MainSingalViewListMenuTemplate =
{
    0,
    MainSingalviewMenu
};

static const MENUTEMPLATE FolderHLMenuTemplate =
{
    0,
    FolderHighLightMenu
};

/*********************************************************************
* Function	   EnterTextsBrow
* Purpose      进入声音浏览界面
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD SNDMANAGE_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
	switch(nCode)
	{
	case APP_INIT:
		hInstance = (HINSTANCE)pInstance;
		break;
	case APP_ACTIVE:
/*
		{
		char foldername[512];
		int i;
		for(i=0;i<15;i++)
		{
			strcat(foldername, "newfolder/");
			if(0 == mkdir(foldername, 0666))
			{
				printf("\r\n new folder created: %s\r\n", foldername);
			}
			else
				printf("\r\n new folder not created: %s\r\n", foldername);
		}
		if(0==rmdir(foldername))
			printf("\r\n removed: %s \r\n", foldername);
		else
			printf("\r\n failed to remove %s\r\n", foldername);

		}//*/
		
		if(!PreviewSound(NULL))
		{
			//printf("failed to create windows\n");
			return 0;
		}
		break;
	case APP_GETOPTION:
		if (wParam == AS_APPWND)
		{
			return (DWORD)hFrameWnd;
		}
		break;
	case APP_INACTIVE:

		ShowOwnedPopups(hFrameWnd, FALSE);
		ShowWindow(hFrameWnd, SW_HIDE);
		
		if(IsWindow(hMainView))
		{
			PostMessage(hMainView, PRIOMAN_MESSAGE_PLAYOVER, 0, 0);
		}
		break;
	}
	return 1;
}

static BOOL	PreviewSound(HWND hWnd)
{
    WNDCLASS wc;
	RECT		rc;

	if (!bSndManageWndClass)
	{
		wc.style            = 0;
		wc.lpfnWndProc      = BrowserSndWndProc;
		wc.cbClsExtra       = 0;
		wc.cbWndExtra       = NULL;
		wc.hInstance        = NULL;
		wc.hIcon            = NULL;
		wc.hCursor          = NULL;
		wc.hbrBackground    = NULL;
		wc.lpszMenuName     = NULL;
		wc.lpszClassName    = "BrowserSndWndClass";
		
		if (!RegisterClass(&wc))
			return FALSE;
		
		bSndManageWndClass = TRUE;
	}

	if (IsWindow(hMainView))
	{
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
	//	SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_EXIT);
	//	SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_PLAY,1), (LPARAM)IDS_PLAYSND);
		GetClientRect(hFrameWnd, &rc);
		SND_InitMenus();

	//	PDASetMenu(hFrameWnd, hViewMenu);
		SetWindowText(hFrameWnd, IDS_CAPTION);
		
		hMainView = CreateWindow("BrowserSndWndClass", NULL, 
			WS_VISIBLE|WS_CHILD,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			hFrameWnd, NULL, NULL, NULL);

		if (!IsWindow(hMainView))
			return FALSE;
		
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
static LRESULT CALLBACK BrowserSndWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
	LRESULT lResult;
	int	index;

	char	szTemp[PREBROW_MAXFILENAMELEN];
	char	curFileName[512];

	int		num;
	RECT	rClient;
	
	static BOOL bShow = FALSE;
	static BOOL bKeyDown = FALSE;

	lResult = TRUE;
	switch(wMsgCmd)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rClient);

		hWndListBox = CreateWindow("LISTBOX","",
			WS_CHILD |WS_VISIBLE|LBS_HASSTRINGS|WS_VSCROLL|LBS_BITMAP|LBS_MULTILINE,
			0, 0, rClient.right, rClient.bottom,
			hWnd,(HMENU)IDC_PREPIC_LIST,NULL,NULL);
		if (!hWndListBox) 
			return	FALSE;
		
		WaitWin(hWnd, TRUE, IDS_OPENING, IDS_CAPTION, NULL, IDS_CANCEL, IDRM_OPENING);
		hFolderBmp = LoadImage(NULL, SNDFOLDER_ICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
		hMMCBmp = LoadImage(NULL, MMCFOLDER_ICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
		hIconStatic = LoadImage(NULL, STATIC_ICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
		hIconPlaying = LoadImage(NULL, PLAYING_ICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
		
		pPlayingItem = NULL;
		nPlayingIndex = -1;

		strcpy(szCurPath, PHONESNDFILEDIR);

		bShow = TRUE;
		bKeyDown = FALSE;
		LoadMainViewList(szCurPath);
		WaitWin(hWnd, FALSE, IDS_OPENING, IDS_CAPTION, NULL, IDS_CANCEL, IDRM_OPENING);
		break;
		
	case IDRM_OPENING:		
	//	PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case WM_SETFOCUS:
		SND_SetListBoxFocus(hWndListBox);		
		break;

	case PWM_ACTIVATE:
		bShow = (BOOL)wParam;
		if(!wParam)
			bKeyDown = FALSE;
		
		lResult = PDADefWindowProc(hWnd, PWM_ACTIVATE, wParam, lParam);
		break;

	case PWM_SHOWWINDOW:
		{
			char	Textbuf[MAXFULLNAMELEN] = "";
			bShow = TRUE;

			if (bUpdated) 
			{
				bUpdated = FALSE;
				LoadMainViewList(szCurPath);
			}
			else
				SND_SetNCArea(hFrameWnd, hWndListBox, szCurPath);
		}
		break;

	case WM_COMMAND:
		if(IDC_PLAY == LOWORD(wParam) || IDC_STOP == LOWORD(wParam))
		{
			if(bKeyDown)
				break;
			
			bKeyDown = TRUE;
			KillTimer(hWnd, TIMERID_KEYDOWN);
			SetTimer(hWnd, TIMERID_KEYDOWN, TIMEREL_KEYDOWN, NULL);
		}
		lResult = SND_OnCommand(hWnd, wParam, lParam);
		break;
	
	case PRIOMAN_MESSAGE_READDATA:
		PrioMan_ReadData(wParam, lParam);
		break;

	case PRIOMAN_MESSAGE_WRITEDATA:
		PrioMan_WriteData(wParam, lParam);
		break;

	case PRIOMAN_MESSAGE_PLAYOVER:
		PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_MUSICMAN);

		if(NULL != pPlayingItem)
			SendMessage(hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nPlayingIndex), (LPARAM)hIconStatic);

	//	if(hWnd == GetWindow(hFrameWnd, GW_CHILD/*GW_LASTCHILD*/))
		if(bShow)
			SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_PLAY, 1), (LPARAM)IDS_PLAYSND);

		ModifyMenu(hSoundMenu, IDC_STOP, MF_BYCOMMAND, IDC_PLAY, IDS_PLAYSND);
		pPlayingItem = NULL;
		nPlayingIndex = -1;
		break;

	case PRIOMAN_MESSAGE_BREAKOFF:
		PrioMan_BreakOffMusic(PRIOMAN_PRIORITY_MUSICMAN);
		
	//	if(GetWindow(hFrameWnd, GW_CHILD) == hWnd)
		if(bShow)
			SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_PLAY, 1), (LPARAM)IDS_PLAYSND);

		if(NULL != pPlayingItem)
			SendMessage(hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nPlayingIndex), (LPARAM)hIconStatic);

		ModifyMenu(hSoundMenu, IDC_STOP, MF_BYCOMMAND, IDC_PLAY, IDS_PLAYSND);
		pPlayingItem = NULL;
		nPlayingIndex = -1;
		break;

	case IDRM_MOVEALL:
		{
			char	path[MAXFULLNAMELEN] = "";
			char	bakpath[MAXFULLNAMELEN] = "";
			char	*name;
			int		i, nLen, nCount;
			BOOL	bSucc = TRUE;
			
			if(lParam <= 0)
				break;
			
			strcpy(path, (char *)lParam);
			if (path[strlen(path) -1 ] != '/')
				strcat(path, "/");

			nLen = strlen(path);

			nCount = SendMessage(hWndListBox, LB_GETCOUNT, 0, 0);

			WaitWindowStateEx(hFrameWnd, TRUE, ML("Moving ..."), NULL, NULL, NULL);
			for (i = nCount -1; i >=0 ; i--) 
			{
			//	SendMessage(hWndListBox, LB_SETCURSEL, i, 0);
			//	if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
				if(SND_GetInfoFromList(i, &ListBuffer, NULL, curFileName, &nFileType))
				{
					if (ISSOUNDFILE(nFileType))
					{
						path[nLen] = '\0';
						name = strrchr(curFileName, '/')+1;
						strcat(path, name);
						
						if (strcasecmp(curFileName,path) == 0) 
						{
						//	continue;
							break;
						}
						else if(SND_IsItemExist(path, TRUE))
						{
							PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
							bSucc = FALSE;
							break;
						}
						else if(!SND_IsMoveAvailable(curFileName, path))
						{
							bSucc = FALSE;
							PLXTipsWin(hFrameWnd, hWnd, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
							break;
						}
						else if(SND_MoveFile(curFileName, path))
						{
							PREBROW_DelData(&ListBuffer, i);
							SendMessage(hWndListBox, LB_DELETESTRING, i, 0);
						}
						KickDog();	// clear the watch dog
					//	GetMessage(&msg, NULL, 0, 0))
					//	TranslateMessage(&msg);
					//	DispatchMessage(&msg);
					}
				}
			}
			SND_UpdateDirInfo(hWndListBox, &ListBuffer, (char *)lParam);
			if(bSucc)
				PLXTipsWin(hFrameWnd, hWnd, 0, ML("Moved"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
			WaitWindowStateEx(hFrameWnd, FALSE, ML("Moving ..."), NULL, NULL, NULL);
		}
		break;

	case IDRM_COPYALL:
		{
			char	path[MAXFULLNAMELEN] = "";
			char	*name;
			int		i, nCount, nLen;
			BOOL	bSucc = TRUE;
			
			if(NULL == lParam)
				break;

			nCount = SendMessage(hWndListBox, LB_GETCOUNT, 0, 0);
			if(nCount <= 0)
				break;
			
			strcpy(path, (char *)lParam);
			if (!ISPATHSEPARATOR(path[strlen(path) -1]))
				strcat(path, "/");

			nLen = strlen(path);

			WaitWindowStateEx(hFrameWnd, TRUE, ML("Copying ..."), NULL, NULL, NULL);
			for (i = nCount-1; i >=0 ; i--) 
			{
			//	SendMessage(hWndListBox, LB_SETCURSEL, i, 0);
			//	if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
				if(SND_GetInfoFromList(i, &ListBuffer, NULL, curFileName, &nFileType))
				{
					if (PREBROW_FILETYPE_WAV == nFileType ||
						PREBROW_FILETYPE_AMR == nFileType ) 
					{
						path[nLen] = '\0';
						name = strrchr(curFileName, '/')+1;
						strcat(path, name);
						
						if (strcmp(curFileName, path) == 0) 
						{
							PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
							bSucc = FALSE;
							break;
						}
						else if(SND_IsItemExist(path, TRUE))
						{
							PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
							bSucc = FALSE;
							break;
						}
						else if(SND_GetFileSize(curFileName) + 500 > SND_GetFreeSpace(path))
						{
							PLXTipsWin(hFrameWnd, hWnd, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
							bSucc = FALSE;
							break;
						}
						else
							SND_CopyFile(curFileName, path);
						
						KickDog();	// clear the watch dog
					//	GetMessage(&msg, NULL, 0, 0))
					//	TranslateMessage(&msg);
					//	DispatchMessage(&msg);
					}
				}
			}
			path[nLen] = '\0';
			SND_UpdateDirInfo(hWndListBox, &ListBuffer, path);
			if(bSucc)
				PLXTipsWin(hFrameWnd, hWnd, 0, ML("Copied"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
			WaitWindowStateEx(hFrameWnd, FALSE, ML("Copying ..."), NULL, NULL, NULL);
		}
		break;

	case IDRM_MOTOFOLDER:
		{
			char	path[MAXFULLNAMELEN] = "";
			char	*name;
			int		index;
			strcpy(path, (char *)lParam);
			index = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
		//	if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
			if(SND_GetInfoFromList(index, &ListBuffer, NULL, curFileName, NULL))
			{
				if (!ISPATHSEPARATOR(path[strlen(path) -1]))
					strcat(path, "/");

				name = strrchr(curFileName, '/')+1;
				strcat(path, name);

				if (strcasecmp(curFileName, path) == 0)	// destination is the source path.
				{
					PLXTipsWin(hFrameWnd, hWnd, 0, ML("Moved"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
				}
				else if(SND_IsMoveAvailable(curFileName, path))
				{
					WaitWindowStateEx(hFrameWnd, TRUE, ML("Moving ..."), NULL, NULL, NULL);
					if(SND_IsItemExist(path, TRUE))	// item with the same name already exists
					{
						PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
					}
					else if(/*0 == rename*/SND_MoveFile(curFileName, path))
					{
						PREBROW_DelData(&ListBuffer, index);
						SendMessage(hWndListBox, LB_DELETESTRING, index, 0);
						SND_UpdateDirInfo(hWndListBox, &ListBuffer, (char *)lParam);
						PLXTipsWin(hFrameWnd, hWnd, 0, ML("Moved"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
					}
					WaitWindowStateEx(hFrameWnd, FALSE, ML("Moving ..."), NULL, NULL, NULL);
				}
				else
				{
					PLXTipsWin(hFrameWnd, hWnd, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
				}
			}
		}	
		break;
		
	case IDRM_CPTOFOLDER:
		{
			char	path[MAXFULLNAMELEN] = "";
			char	*name;
			int		index;
			
			strcpy(path, (char *)lParam);
			index = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
			
		//	if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
			if(SND_GetInfoFromList(index, &ListBuffer, NULL, curFileName, NULL))
			{
				if (!ISPATHSEPARATOR(path[strlen(path) -1]))
					strcat(path, "/");

				name = strrchr(curFileName, '/')+1;
				strcat(path, name);
				
				if (strcasecmp(curFileName, path) == 0)	// destination is the source path.
				{
				//	PLXTipsWin(hFrameWnd, hWnd, 0, ML("Copied"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
					PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
				}
				else
				{
					WaitWindowStateEx(hFrameWnd, TRUE, ML("Copying ..."), NULL, NULL, NULL);
					if(SND_GetFileSize(curFileName)+500 > SND_GetFreeSpace(path))
					{
						PLXTipsWin(hFrameWnd, hWnd, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
					}
					else if(SND_IsItemExist(path, TRUE))	// item with the same name already exists
					{	// obviously, this is NOT reasonable, but the spec says so.
						PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
					}
					else if(SND_CopyFile(curFileName, path))
					{
						SND_UpdateDirInfo(hWndListBox, &ListBuffer, (char *)lParam);
						PLXTipsWin(hFrameWnd, hWnd, 0, ML("Copied"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
					}
					WaitWindowStateEx(hFrameWnd, FALSE, ML("Copying ..."), NULL, NULL, NULL);
				}
			}
		}	
		break;
	
	case IDRM_DELALL:
		if (lParam == 1)
		{
			char	path[100];
			PSTR	ptr;

			memset(path, 0, sizeof(path));
			WaitWindowStateEx(hFrameWnd, TRUE, IDS_DELETING, NULL, NULL, NULL);
			UpdateWaitwinContext(IDS_DELETING);
			
			GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType);
			ptr = strrchr(curFileName,'/');
			strncpy(path, curFileName, ptr-curFileName);
		//	SND_DeleteDirectory(path);
			if(SND_DeleteAllInFolder(path))
			{
				bUpdated = TRUE;
			//	PREBROW_FreeListBuf(&ListBuffer);
			//	SendMessage(hWndListBox, LB_RESETCONTENT, 0, 0);
			//	WaitWindowStateEx(hFrameWnd, FALSE, IDS_DELETING, NULL, NULL, NULL);
				PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DELETED, NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
			}
		//	else
				WaitWindowStateEx(hFrameWnd, FALSE, IDS_DELETING, NULL, NULL, NULL);
		}			
		break;

	case IDC_PROCESSDEL:
		if (lParam == 1) 
		{
			WaitWindowStateEx(hFrameWnd, TRUE, IDS_DELETING, NULL, NULL, NULL);
			index = SendMessage(hWndListBox,LB_GETCURSEL,0,0);
		//	GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType);
			SND_GetInfoFromList(index, &ListBuffer, NULL, curFileName, &nFileType);
			if (nFileType == PREBROW_FILETYPE_FOLDER)
			{
				if(!SND_DeleteDirectory(curFileName))
				{
					WaitWindowStateEx(hFrameWnd, FALSE, IDS_DELETING, NULL, NULL, NULL);
					break;
				}
			}
			else
			{
				if(0 != remove(curFileName))
				{
					WaitWindowStateEx(hFrameWnd, FALSE, IDS_DELETING, NULL, NULL, NULL);
					break;
				}
			}
			SendMessage(hWndListBox, LB_DELETESTRING, index, 0);

			PREBROW_DelData(&ListBuffer, index);
			PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DELETED, NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
			WaitWindowStateEx(hFrameWnd, FALSE, IDS_DELETING, NULL, NULL, NULL);
		}
		break;

	case WM_KEYDOWN:
	    switch(wParam)
		{
		case VK_F10:	// back or exit
			{
				char szOldItem[512];
				int ntype = SND_GetFolderType(szCurPath);
				
				strcpy(szOldItem, szCurPath);

				if(NULL != pPlayingItem)
				{
					PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_MUSICMAN);
					nPlayingIndex = -1;
					pPlayingItem = NULL;
				}
				if(FOLDERTYPE_UNKNOWN == ntype)			// to the top level
					strcpy(szCurPath, PHONESNDFILEDIR);
				else if(FOLDERTYPE_ROOT == ntype)		// exit
				{
				//	PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				}
				else if(FOLDERTYPE_MMCROOT == ntype)	// to the top level
				{
					strcpy(szCurPath, PHONESNDFILEDIR);
				}
				else									// to the parent folder
				{
					if(!SND_GotoParentFolder(szCurPath))
						break;
				}
				LoadMainViewList(szCurPath);

				ntype = SearchSndData(&ListBuffer, szOldItem, PREBROW_FILETYPE_FOLDER);
				if(ntype >= 0)
				{
					SendMessage(hWndListBox, LB_SETCURSEL, (WPARAM)ntype, 0);
					SND_SetNCArea(hFrameWnd, hWndListBox, szCurPath);
				}
			}
			break;

		case VK_RETURN:	// play / stop / open
			num = SendMessage(hWndListBox, LB_GETCOUNT, 0, 0);
			if (num>0)
			{
				int nid = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
				nid = SendMessage(hWndListBox, LB_GETITEMDATA, nid, 0);
				if(LB_ERR == nid)
					break;
				
				if( NULL == pPlayingItem || 
					PREBROW_FILETYPE_FOLDER == nid || 
					SOUND_FILETYPE_MMCCARD == nid  )
				{
					PostMessage(hWnd, WM_COMMAND, IDC_PLAY, 0);
				}
				else
					PostMessage(hWnd, WM_COMMAND, IDC_STOP, 0);
			}
			break;

		case VK_F5:	// options or select
			PostMessage(hFrameWnd, WM_KEYDOWN, wParam, lParam );
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;

	case WM_TIMER:
		KillTimer(hWnd, wParam);
		if(TIMERID_KEYDOWN == wParam)
			bKeyDown = FALSE;
		break;

	case WM_CLOSE:
		PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);
		DestroyWindow (hWnd);
		break;

	case WM_PAINT:
		{
			HDC	hdc;
			int	oldbm;
			RECT	rc;

			GetClientRect(hWnd,&rc);
			hdc = BeginPaint(hWnd,NULL);

			oldbm = SetBkMode(hdc,TRANSPARENT);
			if (SendMessage(hWndListBox, LB_GETCOUNT, 0, 0) > 0)
			{
				ShowWindow(hWndListBox, SW_SHOW);
			}
			else	// show "No sound" when there really isn't any
			{
				ShowWindow(hWndListBox, SW_HIDE);
				DrawText(hdc, IDS_NOSND, -1, &rc, DT_CENTER|DT_VCENTER);
			}
			SetBkMode(hdc,oldbm);
			EndPaint(hWnd,NULL);
		}
		break;


	case WM_DESTROY:
		bKeyDown = FALSE;
		SND_OnDestroy();

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

static LRESULT SND_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int		index = -1;
	LRESULT lResult = (LRESULT)TRUE;
	char	szTemp[MAXFILENAMELEN];
	char	delprompt[MAXFULLNAMELEN+20] = "";
	char	curFileName [512];
	
	// Selected item changed
	if (HIWORD(wParam) == LBN_SELCHANGE) 
	{
		if(NULL != pPlayingItem)
			SendMessage(hWnd, WM_COMMAND, IDC_STOP, 0);
		
		SND_SetNCArea(hFrameWnd, hWndListBox, szCurPath);
		return lResult;
	}
	
	switch(LOWORD(wParam))
	{
	case IDC_PLAY:
		index = SendMessage(hWndListBox,LB_GETCURSEL,0,0);
	//	if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
		if(SND_GetInfoFromList(index, &ListBuffer, NULL, curFileName, &nFileType))
		{
			// play the sound
			if(ISSOUNDFILE(nFileType))
			{
				if(NULL != pPlayingItem)
					break;
				
				{
					PM_PlayMusic pm;
					
					pm.hCallWnd = hWnd;
					pm.nPriority = PRIOMAN_PRIORITY_MUSICMAN;
					if(PREBROW_FILETYPE_WAV == nFileType)
						pm.nRingType = PRIOMAN_RINGTYPE_WAVE;
					else if(PREBROW_FILETYPE_AMR == nFileType)
						pm.nRingType = PRIOMAN_RINGTYPE_AMR;
					else
						break;
					pm.nVolume = PRIOMAN_VOLUME_DEFAULT;
					pm.nRepeat = 1;
					pm.pMusicName = ListBuffer.pCurrentData->szFullData;
					pm.pDataBuf = NULL;
					pm.nDataLen = 0;
					
					printf("\r\n[sound]: about to play\r\n");
					if(PRIOMAN_ERROR_SUCCESS != PrioMan_PlayMusic(&pm))
					{
						printf("\r\n[sound]: Prioman_PlayMusic NOT returning PRIOMAN_ERROR_SUCCESS\r\n");
						pPlayingItem = NULL;
						nPlayingIndex = -1;
						break;
					}
					printf("\r\n[sound]: Prioman_PlayMusic called successfully\r\n");
				}
				nPlayingIndex = index;
				pPlayingItem = ListBuffer.pCurrentData->szFullData;
				SendMessage(hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hIconPlaying);
				SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_STOP, 1), (LPARAM)IDS_STOPSND);
				ModifyMenu(hSoundMenu, IDC_PLAY, MF_BYCOMMAND, IDC_STOP, IDS_STOPSND);
			}
			else if(FILE_TYPE_UNKNOW == nFileType)
				break;
			else
			{	// enter the folder
			//	SND_EnterSubFolder(szCurPath, szTemp);
				strcpy(szCurPath, curFileName);
				LoadMainViewList(szCurPath);
			}
		}
		break;
		
	case IDC_STOP:	// stop playing
		if(NULL == pPlayingItem)
			break;
		
		index = nPlayingIndex;
		
		PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_MUSICMAN);
		nPlayingIndex = -1;
		pPlayingItem = NULL;
		ModifyMenu(hSoundMenu, IDC_STOP, MF_BYCOMMAND, IDC_PLAY, IDS_PLAYSND);
		SendMessage(hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)hIconStatic);
		SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_PLAY,1), (LPARAM)IDS_PLAYSND);
		break;
		
	case IDC_CREATEFOLDER:
		SndBrowser_Edit(hWnd, NULL, NULL, SNDEDIT_CREATEDIR);			
		break;
		
	case IDC_RENAME:
		index = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
	//	if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
		if(SND_GetInfoFromList(index, &ListBuffer, szTemp, curFileName, &nFileType))
		{
			if(PREBROW_FILETYPE_FOLDER == nFileType)
				SndBrowser_Edit(hWnd, szTemp, curFileName, SNDEDIT_RENAMEDIR);
			else if(ISSOUNDFILE(nFileType))
				SndBrowser_Edit(hWnd, szTemp, curFileName, SNDEDIT_RENAMEFILE);
		}
		
		break;
		
	case IDC_DELETE:
		index = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
	//	if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
		if(SND_GetInfoFromList(index, &ListBuffer, szTemp, curFileName, &nFileType))
		{
			if (nFileType == PREBROW_FILETYPE_FOLDER) 
			{
				if(SND_IsFolderEmpty(curFileName))
					sprintf(delprompt, "%s:\n%s?", szTemp, IDS_DELETE);
				else
					sprintf(delprompt, "%s:\n%s", szTemp, IDS_DELFOLDER);
				PLXConfirmWinEx(hFrameWnd, hWnd, delprompt, Notify_Request, NULL, IDS_YES, IDS_NO, IDC_PROCESSDEL);
			}
			else if(ISSOUNDFILE(nFileType))
			{
				sprintf(delprompt, "%s:\n%s?", szTemp, IDS_DELETE);
				PLXConfirmWinEx(hFrameWnd, hWnd, delprompt, Notify_Request, NULL, IDS_YES, IDS_NO, IDC_PROCESSDEL);				
			}
		}
		break;
		
	case IDC_DELALL:
		{
//			char *pCap = NULL;
//			int nLen = GetWindowTextLength(hFrameWnd);
//			ASSERT(nLen >0);
//			pCap = (char*)malloc(nLen+1);
//			if(NULL == pCap)
//				break;
//			GetWindowText(hFrameWnd, pCap, nLen+1);
			PLXConfirmWinEx(hFrameWnd, hWnd, IDS_DELALLSND, Notify_Request, NULL/*IDS_CAPTION*/, IDS_YES, IDS_NO, IDRM_DELALL);	
		}
		break;
		
	case IDC_DELSELECT:
		SndMultiSel(hFrameWnd, &ListBuffer, IDC_DELSELECT);
		break;
		
	case IDC_MOVETOFOLDER:
		SndMoveToFolder(hWnd, IDRM_MOTOFOLDER);
		break;
		
	case IDC_MOVESELECT:
		SndMultiSel(hFrameWnd, &ListBuffer, IDC_MOVESELECT);
		break;
		
	case IDC_MOVEALL:
		SndMoveToFolder(hWnd, IDRM_MOVEALL);
		break;
		
	case IDC_CPTOFOLDER:
		SndMoveToFolder(hWnd, IDRM_CPTOFOLDER);
		break;

	case IDC_COPYSELECT:
		SndMultiSel(hFrameWnd, &ListBuffer, IDC_COPYSELECT);
		break;
		
	case IDC_COPYALL:
		SndMoveToFolder(hWnd, IDRM_COPYALL);
		break;
		
	case IDC_SHOWDETAIL:
		index = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
	//	if(GetFileNameFromList(hWndListBox, &ListBuffer, NULL, szTemp, curFileName, &nFileType))
		if(SND_GetInfoFromList(index, &ListBuffer, szTemp, curFileName, &nFileType))
			ShowSndDetail(hFrameWnd,szTemp, curFileName, nFileType);
		break;
		
	default:
		lResult = PDADefWindowProc(hWnd, WM_COMMAND, wParam, lParam);
		break;
	}
	return lResult;
}

static void SND_OnDestroy(void)
{
	if (hFolderBmp) 
	{
		DeleteObject(hFolderBmp);
		hFolderBmp = NULL;
	}
	if(hMMCBmp)
	{
		DeleteObject(hMMCBmp);
		hMMCBmp = NULL;
	}
	if(hIconStatic)
	{
		DeleteObject(hIconStatic);
		hIconStatic = NULL;
	}
	if(hIconPlaying)
	{
		DeleteObject(hIconPlaying);
		hIconPlaying = NULL;
	}
	if (hFolderHLMenu) 
	{
		DestroyMenu(hFolderHLMenu);
		hFolderHLMenu = NULL;
	}
	if(hViewMenu)
	{
		DestroyMenu(hViewMenu);
		hViewMenu = NULL;
	}
	if(hSingalViewMenu)
	{
		DestroyMenu(hSingalViewMenu);
		hSingalViewMenu = NULL;
	}
	
	if(NULL != pPlayingItem)
		PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_MUSICMAN);

	pPlayingItem = NULL;
	nPlayingIndex = -1;
	
	hMainView = NULL;
	PREBROW_FreeListBuf (&ListBuffer);
	UnregisterClass ("BrowserSndWndClass", NULL);
	bSndManageWndClass = FALSE;
	DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
}

/*********************************************************************
* Function	   AddSoundsToList
* Purpose      put sound files or folders into list box
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int	AddSoundsToList(HWND hList , PCSTR pPath, PLISTBUF pLtB, BOOL bShowSubDir)
{
	int n;
	
	if(NULL == pPath || NULL == pLtB || NULL == hList)
		return 0;
	
	PREBROW_FreeListBuf(pLtB);

	AddSndFileToList(pPath, pLtB, bShowSubDir);
	LoadListBoxFromBuffer(hList, pLtB);
	
	n = pLtB->nDataNum;
	if (n > 0)
		SendMessage(hList,LB_SETCURSEL,(WPARAM)0,NULL);

	return n;
}
/*********************************************************************\
* Function	AddSndFileToList
* Purpose	put sound files or folders into list buffer
* Params	
* Return	
* Remarks	the items are sorted by type(file/folder/MMC) and name
**********************************************************************/
BOOL AddSndFileToList(const char* pFilePath, LISTBUF* ListBuffer, BOOL bShowSubDir)
{
	struct dirent *dirinfo = NULL;
	struct stat	filestat;
	DIR		*dirtemp = NULL;
	int		nItemData;
	
	char	*Suffix;
	int		pathlen;
	char	FullName[256];
	char	FileName[64];

	if(NULL == ListBuffer || NULL == pFilePath)
		return FALSE;

//	PREBROW_FreeListBuf (ListBuffer);
	
	dirtemp = opendir(pFilePath);
	if(dirtemp == NULL)
		return FALSE;
	
	memset(FullName, 0, 256);
	strcpy(FullName, pFilePath);
		
	if (!ISPATHSEPARATOR(FullName[strlen(FullName)-1]) )
		strcat(FullName,"/");
	
	pathlen = strlen(FullName);
	
	while((dirinfo = readdir(dirtemp))!=NULL)
	{
		if(ISPRESERVEDPATH(dirinfo->d_name))
			continue;
		
		FullName[pathlen] = '\0';
		strcat (FullName, dirinfo->d_name);
		stat(FullName, &filestat);
		
		// if FullName is a dir
		if (!S_ISLNK(filestat.st_mode) && S_ISDIR(filestat.st_mode)) 
		{
			if(!bShowSubDir)
				continue;

			PREBROW_InsertSndData(ListBuffer, PREBROW_FILETYPE_FOLDER, dirinfo->d_name, FullName, filestat.st_atime);
		}
		else if(S_ISREG(filestat.st_mode))
		{
			if(!IsSound(FullName, &nItemData)) 
			{
				continue;
			}
			Suffix = strrchr(dirinfo->d_name,'.');
#ifdef _SOUND_LOCAL_TEST_
			ASSERT(Suffix > dirinfo->d_name);
#endif
			memset(FileName, 0, 64);
			strncpy(FileName, dirinfo->d_name, Suffix - dirinfo->d_name);
			
			PREBROW_InsertSndData(ListBuffer, nItemData, FileName, FullName, filestat.st_atime);
		}
	}
	closedir(dirtemp);
	chdir(pFilePath);

	return TRUE;
}

/*********************************************************************\
* Function	AddSndFileToList
* Purpose	load items into list box from list buffer
* Params	
* Return	
* Remarks	
**********************************************************************/
static int LoadListBoxFromBuffer(HWND hList, LISTBUF* ListBuffer)
{
	HBITMAP		hbitbmp = NULL;
	PLISTDATA	pNode = NULL;
    int			nRtnNum = 0;
	int			index = -1;
	struct stat	filestat;
	char		SndSize[18];
	float		tmpsize;
	DWORD		nFileNum;
	char		FileName[PREBROW_MAXFILENAMELEN];
	
	if(NULL == ListBuffer || NULL == hList)
		return 0;

	SendMessage (hList, LB_RESETCONTENT, NULL, NULL);
	
	pNode = ListBuffer->pDataHead;
	if(NULL == pNode)
		return 0;

	do
	{
		if(ISSOUNDFILE(pNode->nFVLine))	// file
		{
			index = SND_AddListBoxItem(hList, pNode->szData);
			if (LB_ERR == index)
				break;
			
			if(index == nPlayingIndex)
				hbitbmp = hIconPlaying;
			else
				hbitbmp = hIconStatic;
			
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hbitbmp);

			//-------get the stat of current file------------------
			stat((pNode->szFullData), &filestat);
			tmpsize = (float)filestat.st_size/1024;
			floattoa(tmpsize, SndSize);
			strcat(SndSize, " kB");

			SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(index,-1),(LPARAM)SndSize);
			SendMessage(hList,LB_SETITEMDATA,(WPARAM)index,(LPARAM)(pNode->nFVLine));
		}
		else	// folder
		{
			index = SND_AddListBoxItem(hList, pNode->szData);
			if (LB_ERR == index)
				break;

			nFileNum = GetSndFileNumInDir(pNode->szFullData);

			if(-1 != (long)nFileNum)
			{
				sprintf(FileName,"%s%d    %s%d", ML("Folder:"), HIWORD(nFileNum), 
					ML("Sounds:"), LOWORD(nFileNum));
				SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)FileName);
			}
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hFolderBmp);
			SendMessage(hList, LB_SETITEMDATA,(WPARAM)index,(LPARAM)PREBROW_FILETYPE_FOLDER);
		}
		nRtnNum++;
		pNode = pNode->pNext;
	} while(pNode &&(pNode != ListBuffer->pDataHead));

	return nRtnNum;
}

/*********************************************************************\
* Function	   PREBROW_AddSndData
* Purpose      添加数据
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
PLISTDATA	PREBROW_AddSndData (PLISTBUF pListBuf, const char* szData, const char* szFullData, unsigned long atime)
{
	PLISTDATA	tempdata;
	PLISTDATA	pTail;

	if (NULL == (tempdata = malloc ( sizeof(LISTDATA) ) ) )
		return NULL;	
	if (NULL == (tempdata->szData = malloc(strlen(szData)+1)))
		return FALSE;
	if (NULL == (tempdata->szFullData = malloc(strlen(szFullData)+1)))
		return FALSE;

//	tempdata->nIndex = pListBuf->nDataNum;
	strcpy(tempdata->szData , szData);
	strcpy(tempdata->szFullData , szFullData);
	tempdata->hbmp = NULL;
	tempdata->atime = atime;
	tempdata->nFVLine = SOUND_FILETYPE_MMCCARD;
	tempdata->pFormViewerContent = NULL;

	if (pListBuf->nDataNum == 0)//Initialize the chain
	{
		pListBuf->pDataHead = tempdata;
		pListBuf->pCurrentData = pListBuf->pDataHead;

		pListBuf->pCurrentData->pNext =	pListBuf->pDataHead;
		pListBuf->pCurrentData->pPre  = pListBuf->pDataHead;		

		pListBuf->nCurrentIndex	=	pListBuf->pCurrentData->nIndex;
		pListBuf->nDataNum++;
		return pListBuf->pDataHead;
	}

	//add node to the chain
	pTail = pListBuf->pDataHead->pPre;
	
	pTail->pNext = tempdata;
	tempdata->pNext = pListBuf->pDataHead;

	tempdata->pPre  = pTail;
	pListBuf->pDataHead->pPre = tempdata;	
	
	pListBuf->nDataNum++;

	return tempdata;
}

// insert data to a list. order by 'szData'
PLISTDATA PREBROW_InsertSndData (PLISTBUF pListBuf, int nItemData, char* szData, char* szFullData, unsigned long atime)
{
	PLISTDATA	tempdata;
	PLISTDATA	ptmpnode;

	if (NULL == (tempdata = malloc ( sizeof(LISTDATA) ) ) )
		return NULL;	
	if (NULL == (tempdata->szData = malloc(strlen(szData)+1)))
	{
		free(tempdata);
		return FALSE;
	}
	if (NULL == (tempdata->szFullData = malloc(strlen(szFullData)+1)))
	{
		free(tempdata->szData);
		free(tempdata);
		return FALSE;
	}

	if(NULL == pListBuf->pDataHead)
		pListBuf->nDataNum = 0;
	
//	tempdata->nIndex = pListBuf->nDataNum;
	strcpy(tempdata->szData , szData);
	strcpy(tempdata->szFullData , szFullData);
	tempdata->hbmp = NULL;
	tempdata->atime = atime;
	tempdata->nFVLine = nItemData;
	tempdata->pFormViewerContent = NULL;

	if (pListBuf->nDataNum == 0)//Initialize the chain
	{
		pListBuf->pDataHead = tempdata;
		pListBuf->pCurrentData = pListBuf->pDataHead;

		pListBuf->pCurrentData->pNext =	pListBuf->pDataHead;
		pListBuf->pCurrentData->pPre  = pListBuf->pDataHead;		

		pListBuf->nCurrentIndex	= pListBuf->pCurrentData->nIndex;
		pListBuf->nDataNum++;
		return (pListBuf->pDataHead);
	}
	
	//add node to the chain

	ptmpnode = pListBuf->pDataHead;
	if(ISSOUNDFILE(nItemData))	// inserting a file
	{
		do
		{
			if(!ISSOUNDFILE(ptmpnode->nFVLine) || strcasecmp(ptmpnode->szData, szData) > 0)
			{	// insert into
				ptmpnode->pPre->pNext = tempdata;
				tempdata->pPre = ptmpnode->pPre;
				tempdata->pNext = ptmpnode;
				ptmpnode->pPre = tempdata;
				
				pListBuf->nDataNum ++;
				if(ptmpnode == pListBuf->pDataHead)
					pListBuf->pDataHead = tempdata;
				
				return tempdata;
			}
			else
			{	// search the proper position
				ptmpnode = ptmpnode->pNext;
			}
		} while(ptmpnode && (ptmpnode != pListBuf->pDataHead));

		// append to the tail
		ptmpnode = pListBuf->pDataHead;
		tempdata->pPre = ptmpnode->pPre;
		tempdata->pNext = ptmpnode;
		ptmpnode->pPre->pNext = tempdata;
		ptmpnode->pPre = tempdata;
				
		pListBuf->nDataNum ++;
				
		return tempdata;
	}
	else	// inserting a folder
	{
		do
		{
			if(ISSOUNDFILE(ptmpnode->nFVLine) || strcasecmp(ptmpnode->szData, szData) <= 0)
			{	// search the proper position
				ptmpnode = ptmpnode->pNext;
			}
			else
			{	// insert into
				ptmpnode->pPre->pNext = tempdata;
				tempdata->pPre = ptmpnode->pPre;
				tempdata->pNext = ptmpnode;
				ptmpnode->pPre = tempdata;
				
				pListBuf->nDataNum ++;
				if(ptmpnode == pListBuf->pDataHead)
					pListBuf->pDataHead = tempdata;
				
				return tempdata;
			}
		} while(ptmpnode && (ptmpnode != pListBuf->pDataHead));

		// append to the tail
		ptmpnode = pListBuf->pDataHead;
		tempdata->pPre = ptmpnode->pPre;
		tempdata->pNext = ptmpnode;
		ptmpnode->pPre->pNext = tempdata;
		ptmpnode->pPre = tempdata;
				
		pListBuf->nDataNum ++;
				
		return tempdata;	
	}

	// failed for some reason
	free(tempdata->szData);
	free(tempdata->szFullData);
	free(tempdata);
	return NULL;
}

// find the specified item, return its index
// the strategy is based on the organization of the nodes
// the order is: File, File, ..., Folder, Folder, ..., MMC.
int SearchSndData(PLISTBUF pListBuf, const char *szFullName, int nItemType)
{
	PLISTDATA	pStart;
	PLISTDATA	pCurData;
	int			i = 0;

	if(NULL == pListBuf || NULL == szFullName)
		return -1;

	if(pListBuf->nDataNum < 1)
		return -1;
	if(NULL == pListBuf->pDataHead)
		return -1;

	switch(nItemType)
	{
	case SOUND_FILETYPE_MMCCARD:	// MMC card must be the last node
		pCurData = pListBuf->pDataHead->pPre;
		if(NULL == pCurData)
			return -1;
		if(SOUND_FILETYPE_MMCCARD == pCurData->nFVLine)
			return (pListBuf->nDataNum - 1);
		else
			return -1;

	case PREBROW_FILETYPE_FOLDER:	// search from the last node to the first
		pStart = pListBuf->pDataHead->pPre;
		if(NULL == pStart)
			return -1;
		
		pCurData = pStart;
		do 
		{
			if(NULL == pCurData->szFullData)
				return -1;
			
			if(ISSOUNDFILE(nItemType))	// if encounter a file, 
				return -1;				// it means that there're no more folders
			
			if(SND_IsInFolder(szFullName, pCurData->szFullData) == 0)	// got it!
				return (pListBuf->nDataNum - 1 - i);
			
			pCurData = pCurData->pPre;	// go on ~
			i ++;
		} while(pCurData && (pCurData != pStart));
		
		break;

	case PREBROW_FILETYPE_WAV:		// search from the first node
	case PREBROW_FILETYPE_AMR:
		pStart = pListBuf->pDataHead;		
		pCurData = pStart;
		do 
		{
			if(NULL == pCurData->szFullData)
				return -1;
			
			if(!ISSOUNDFILE(nItemType))	// if encounter a folder or something else,
				return -1;				// it means that there're no more files
			
			if(strcasecmp(szFullName, pCurData->szFullData) == 0)	// got it!
				return i;
			
			pCurData = pCurData->pNext;	// go on ~
			i ++;
		} while(pCurData && (pCurData != pStart));

		break;

	default:	// unknown data
		return -1;
	}

	return -1;
}

long GetSndFileNumInDir(PCSTR pPath)
{
	struct dirent *dirinfo = NULL;
	struct stat	filestat;
	DIR		*dirtemp = NULL;
	WORD	nFile, nDir;
	PSTR	strFullName;
	int		tmp;
	
	dirtemp = opendir(pPath);
	if (dirtemp == NULL)
		return -1;

	nFile = nDir = 0;
	dirinfo = readdir(dirtemp);
	
	while (dirinfo&&dirinfo->d_name[0]) 
	{
		if(ISPRESERVEDPATH(dirinfo->d_name))
		{
			dirinfo = readdir(dirtemp);
			continue;
		}
		
		strFullName = malloc(strlen(pPath)+strlen(dirinfo->d_name)+2);
		strcpy ( strFullName, pPath );
		if (!ISPATHSEPARATOR(pPath[strlen(pPath)-1]))
			strcat(strFullName,"/");

		strcat ( strFullName, dirinfo->d_name);
		stat(strFullName,&filestat);
		if (!S_ISLNK(filestat.st_mode) && S_ISDIR(filestat.st_mode)) 
		{
			nDir ++;
		}
		else if (IsSound(dirinfo->d_name, &tmp)) 
		{
			nFile ++;
		}
		free(strFullName);
		strFullName = NULL;
		dirinfo = readdir(dirtemp);

		KickDog();	// clear the watch dog
	//	GetMessage(&msg, NULL, 0, 0))
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	}

	closedir(dirtemp);

	return	(DWORD)MAKELONG(nFile, nDir);
}

BOOL	IsSound(PSTR	pFileName, int *nItemData)
{
	char	*Suffix;
	struct stat	filestat;

	stat(pFileName,&filestat);
	if((0 == strcmp(pFileName,".")) ||(0 == strcmp(pFileName,"..")))
	{
		*nItemData = FILE_TYPE_UNKNOW;
		return FALSE;
	}
	if (!S_ISLNK(filestat.st_mode) && S_ISDIR(filestat.st_mode)) 
	{
		if(strlen(MMCSNDFOLDERPATH) >= strlen(pFileName))
		{
			if(0 == strncasecmp(MMCSNDFOLDERPATH, pFileName, strlen(MMCSNDFOLDERPATH)-1))
			{
				*nItemData = SOUND_FILETYPE_MMCCARD;
				return TRUE;
			}
		}
		*nItemData = PREBROW_FILETYPE_FOLDER;
		return TRUE;
	}
	Suffix = strrchr(pFileName, '.');
	if (!Suffix)
	{
		*nItemData = FILE_TYPE_UNKNOW;
		return FALSE;
	}
	if (strcasecmp(Suffix, PREBROW_FILEUNIVERSE_WAV) == 0)
	{
		*nItemData = PREBROW_FILETYPE_WAV;
		return TRUE;
	}
//	else if(strcasecmp(Suffix, PREBROW_FILETAIL_MID) == 0)
//	{
//		*nItemData = PREBROW_FILETYPE_MID;
//		return TRUE;
//	}
//	else if(strcasecmp(Suffix, PREBROW_FILETAIL_MMF) == 0)
//	{
//		*nItemData = PREBROW_FILETYPE_MMF;
//		return TRUE;
//	}
	else if(strcasecmp(Suffix, PREBROW_FILETAIL_AMR) == 0)
	{
		*nItemData = PREBROW_FILETYPE_AMR;
		return TRUE;
	}
	else
	{
		*nItemData = FILE_TYPE_UNKNOW;
		return FALSE;
	}
}

BOOL SndBrowser_Edit(HWND hParentWnd, PSTR szName, PSTR szFullName, short nType)
{
	WNDCLASS	wc;
	RECT		rf;
//	PSTR		ptr;

	SNDEDITDATA EditData;

	EditData.nEditType = nType;
//	bDir = bFolder;
	
	if (NULL != szName)
	{
		strcpy(pFileFullName, szFullName);
//		strcpy(cFileName, szName);
//		ptr = strrchr(szFullName, '/');
		//strcpy(strNameCpy,ptr+1);
		//strncpy(szPath, szFullName, ptr - szFullName+1);
		strcpy(EditData.szName, szName);
	}
	else
		strcpy(EditData.szName, "");

	if (IsWindow(hRenameWnd))
	{
		ShowWindow(hRenameWnd,SW_SHOW);
		UpdateWindow(hRenameWnd);
		return	TRUE;
	}

	wc.style         = 0;//CS_OWNDC;
	wc.lpfnWndProc   = EditSndNameProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof(SNDEDITDATA);//0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = RENAMEWCLS;

	RegisterClass(&wc);

	GetClientRect(hFrameWnd, &rf);
	
//	if (SNDEDIT_CREATEDIR != nType) 
//	{
//		hRenameWnd	= CreateWindow(RENAMEWCLS, NULL/*IDS_RENAMESOUND*//*szName*/,
//			WS_VISIBLE | WS_CHILD,
//			rf.left, rf.top, rf.right - rf.left, rf.bottom - rf.top,
//			hFrameWnd,NULL,NULL, &EditData);
//		SetWindowText(hFrameWnd, IDS_RENAMESOUND/*szName*/);
//	}
//	else
//	{
//		hRenameWnd	= CreateWindow(RENAMEWCLS,IDS_CREATEFOLDER,
//			WS_VISIBLE | WS_CHILD,
//			rf.left, rf.top, rf.right - rf.left, rf.bottom - rf.top,
//			hFrameWnd, NULL, NULL, &nEditType);
//		SetWindowText(hFrameWnd, IDS_CREATEFOLDER);
//	}
	
	hRenameWnd	= CreateWindow(RENAMEWCLS, NULL,
		WS_VISIBLE | WS_CHILD,
		rf.left, rf.top, rf.right - rf.left, rf.bottom - rf.top,
		hFrameWnd,NULL,NULL, &EditData);
	
	if(SNDEDIT_CREATEDIR == nType)
		SetWindowText(hFrameWnd, IDS_CREATEFOLDER);
	else if(SNDEDIT_RENAMEDIR == nType)
		SetWindowText(hFrameWnd, IDS_RENAMEFOLDER);
	else
		SetWindowText(hFrameWnd, IDS_RENAMESOUND);

	if (!hRenameWnd)
		return FALSE;

	SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
	SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
	SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
	ShowWindow(hFrameWnd, SW_SHOW);
	UpdateWindow(hFrameWnd);
	SetFocus(hRenameWnd);
	return TRUE;
}

static	LRESULT	EditSndNameProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT	lResult;
	static	HWND	hRenameEdit = NULL;
	IMEEDIT	ie;
	RECT	rcClient;
	int		txtlen,index;
	char	delprompt[MAXFULLNAMELEN] = "";
	char	*cPath = NULL;
	
	PSNDEDITDATA pEditData = (PSNDEDITDATA)GetUserData(hWnd);
	
	lResult = TRUE;

	switch(uMsg) 
	{
	case WM_CREATE:

		memcpy(pEditData, ((LPCREATESTRUCT)lParam)->lpCreateParams, sizeof(SNDEDITDATA));

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
			WS_VISIBLE| WS_CHILD|ES_AUTOHSCROLL|ES_TITLE|WS_TABSTOP,
			rcClient.left, rcClient.top, rcClient.right - rcClient.left, 52,
			hWnd, NULL, NULL, (PVOID)&ie);
		SendMessage(hRenameEdit, EM_LIMITTEXT, 30, 0);
		if(SNDEDIT_CREATEDIR == pEditData->nEditType/*bDir*/)
		{		
			SendMessage(hRenameEdit, EM_SETTITLE, 0, (LPARAM)IDS_FOLDERNAME);
			SendMessage(hRenameEdit, WM_SETTEXT, 0, (LPARAM)IDS_NEWFOLDER);
		}
		else if(SNDEDIT_RENAMEDIR == pEditData->nEditType)
		{
			SendMessage(hRenameEdit, EM_SETTITLE, 0, (LPARAM)IDS_FOLDERNAME);
			SendMessage(hRenameEdit, WM_SETTEXT, 0, (LPARAM)pEditData->szName);
		//	SND_SetWindowText(hRenameEdit, pEditData->szName);
		}
		else
		{
			SendMessage(hRenameEdit,EM_SETTITLE, 0, (LPARAM)IDS_NEWNAME);
			SendMessage(hRenameEdit, WM_SETTEXT, 0, (LPARAM)pEditData->szName);
		//	SND_SetWindowText(hRenameEdit, pEditData->szName);
		}
	//	SetFocus(hRenameEdit);
	//	SendMessage(hRenameEdit, EM_SETSEL, -1, -1);
		break;

	case PWM_SHOWWINDOW:
		if (SNDEDIT_CREATEDIR == pEditData->nEditType)
			SetWindowText(hFrameWnd,IDS_CREATEFOLDER);
		else if (SNDEDIT_RENAMEDIR == pEditData->nEditType)
			SetWindowText(hFrameWnd,IDS_RENAMEFOLDER);
		else
			SetWindowText(hFrameWnd, IDS_RENAMESOUND);

		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 1,(LPARAM)IDS_SAVE);
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 0,(LPARAM)IDS_CANCEL);
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 2,(LPARAM)"");
		break;

	case WM_SETFOCUS:
		SetFocus(hRenameEdit);		
		SendMessage(hRenameEdit,EM_SETSEL, -1, -1);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_RETURN:
			{
				char	szPath[MAXFULLNAMELEN] = "";
				char	szName[64];
				txtlen	= GetWindowTextLength(hRenameEdit);
				if (txtlen < 1) 
				{
					PLXTipsWin(NULL, NULL, 0, IDS_RNMNOTIFY, NULL, 
						Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
					break;
				}
				else
				{
				//	index = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
				//	ptr		= malloc(txtlen+1);
				//	GetWindowText(hRenameEdit, ptr, txtlen+1);
					memset(szName, 0, 64);
					SND_GetWindowText(hRenameEdit, szName, 63);
					
					if(0 == strcmp(szName, pEditData->szName))
					{
					//	free(ptr);
					//	PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
						PostMessage(hWnd,WM_CLOSE,0,0);
						break;
					}

					if(SND_IsInvalidFileName(szName))
					{
						PLXTipsWin(NULL, hWnd, 0, IDS_DEFOTHERNAME, NULL, 
							Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
						break;
					}
					memset(delprompt, 0, MAXFULLNAMELEN);
					
					if (SNDEDIT_CREATEDIR != pEditData->nEditType) 
					{
						BOOL bExist = FALSE;
						char sep = szCurPath[strlen(szCurPath)-1];
						if('/' != sep && '\\' != sep)
							sprintf(delprompt, "%s/", szCurPath);
						else
							strcpy(delprompt, szCurPath);
						
						switch(nFileType)
						{
						case PREBROW_FILETYPE_AMR:
							sprintf(delprompt,"%s%s.amr", delprompt, szName);
							bExist = SND_IsItemExist(delprompt, TRUE);
							break;
							
						case PREBROW_FILETYPE_WAV:
							sprintf(delprompt,"%s%s.wav", delprompt, szName);
							bExist = SND_IsItemExist(delprompt, TRUE);
							break;

						default:	// PREBROW_FILETYPE_FOLDER:
							sprintf(delprompt,"%s%s", delprompt, szName);
							bExist = SND_IsItemExist(delprompt, FALSE);
							break;
							
						}
						
						if (!bExist) 
						{
							if(0 == /*SND_*/rename(pFileFullName, delprompt))
							{
//								SendMessage(hWndListBox, LB_SETTEXT, index, (LPARAM)ptr);
//								PREBROW_ModifyData(&ListBuffer, index, ptr, delprompt);
								LoadMainViewList(szCurPath);
								index = SearchSndData(&ListBuffer, delprompt, nFileType);
								if(index >= 0)
								{
									SendMessage(hWndListBox, LB_SETCURSEL, index, 0);
									SND_SetNCArea(hFrameWnd, hWndListBox, szCurPath);
								}
								printf("\r\n[sound]: item name <%s> not exist, rename done!\r\n", delprompt);
							//	bUpdated = TRUE;
							}
						}
						else
						{
							printf("\r\n[sound]: item name <%s> already exist!\r\n", delprompt);
							PLXTipsWin(NULL, hWnd, 0, IDS_DEFOTHERNAME, NULL, 
								Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
						//	free(ptr);
							break;
						}
						
					}
					else
					{
						char sep = szCurPath[strlen(szCurPath)-1];
						if(!ISPATHSEPARATOR(sep))
							sprintf(delprompt,"%s/%s", szCurPath, szName);	//*****
						else
							sprintf(delprompt,"%s%s",szCurPath, szName);	//*****

						if (SND_GetFreeSpace(delprompt) < 500)
						{
							PLXTipsWin(hFrameWnd, hWnd, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
							break;
						}
						
						if (!SND_IsItemExist(delprompt, FALSE)) 
						{
							if(0 == mkdir(delprompt,0666))
							{
								LoadMainViewList(szCurPath);
								index = SearchSndData(&ListBuffer, delprompt, PREBROW_FILETYPE_FOLDER);
								if(index >= 0)
								{
									SendMessage(hWndListBox, LB_SETCURSEL, index, 0);
									SND_SetNCArea(hFrameWnd, hWndListBox, szCurPath);
								}
							}
						//	bUpdated = TRUE;
						}
						else
						{
							PLXTipsWin(NULL, hWnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
						//	free(ptr);
							break;
						}
					}					
				//	free(ptr);
				//	PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					PostMessage(hWnd,WM_CLOSE,0,0);
					break;
				}
			}
			break;

		case VK_F10:
			cFileName[0] = NULL;
			PostMessage(hWnd,WM_CLOSE,0,0);
			break;

		default:
			lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
			break;
		}
		break;

	case WM_CLOSE:
		PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		UnregisterClass(RENAMEWCLS,NULL);
		break;

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
	wc.hbrBackground    = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = MOVETOFOLDER;
	
	if (!RegisterClass(&wc))
		return FALSE;
	return TRUE;
}

BOOL	SndMoveToFolder(HWND hRecmsg, UINT rMsg)
{
	RECT	rf;
	MOVEDATA	md;

	md.hRecMsg = hRecmsg;
	md.rmsg = rMsg;

	strcpy(szDstPath, "");

	GetClientRect(hFrameWnd, &rf);
	RegisteMovetoFolder();
	
	hMoveToFolder = CreateWindow(MOVETOFOLDER, NULL,
		WS_CHILD|WS_VISIBLE,
		rf.left, rf.top, rf.right - rf.left, rf.bottom - rf.top, 
		hFrameWnd, NULL, NULL, (PVOID)&md);
	if (!hMoveToFolder)
		return FALSE;

	SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_LEFTICON), NULL);
	SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_RIGHTICON), NULL);
	ShowWindow(hFrameWnd, SW_SHOW);
	UpdateWindow(hFrameWnd);
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
			
			if(NULL == hPhoneBmp)
				hPhoneBmp = LoadImage(NULL, PHONEMEM_ICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);

			if(NULL == hOpenDirBmp)
				hOpenDirBmp = LoadImage(NULL, OPENFOLDER_ICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);

			GetClientRect(hWnd, &rc);
			hFolderList = CreateWindow("LISTBOX", NULL,
				WS_CHILD |WS_VISIBLE|LBS_HASSTRINGS|WS_VSCROLL|LBS_BITMAP|LBS_MULTILINE,
				rc.left, rc.top,
				rc.right - rc.left,
				rc.bottom - rc.top,
				hWnd, NULL, NULL,NULL);

			LoadFolderList(hFrameWnd, hFolderList, &FolderListbuf, szDstPath, pmd->rmsg);
		}
		break;
		
	case PWM_SHOWWINDOW:
		nIndex = SendMessage(hFolderList, LB_GETCURSEL, 0, 0);
		if(strlen(szDstPath) <= 1)
		{
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		}
		else if(nIndex > 0)
		{
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		}
		else
		{
			if(IDRM_MOTOFOLDER == pmd->rmsg || IDRM_MOVEALL == pmd->rmsg)
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_MOVE);
			else
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_COPY);
			
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		}

		if(strlen(szDstPath) <= 1)
			SetWindowText(hFrameWnd, IDS_SELECTDEST);
		else
			SetWindowText(hFrameWnd, IDS_SELECTFOLDER);
		SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_LEFTICON), NULL);
		SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_RIGHTICON), NULL);
		SetFocus(hMoveToFolder);
		break;

	case WM_SETFOCUS:
		SND_SetListBoxFocus(hFolderList);
		break;
//*
	case WM_COMMAND:
		switch(HIWORD(wParam)) 
		{
		case LBN_SELCHANGE:
			if(strlen(szDstPath) <= 1)
				break;

			nIndex = SendMessage(hFolderList, LB_GETCURSEL, 0, 0);
			if(nIndex > 0)
			{
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			}
			else
			{
				if(IDRM_MOTOFOLDER == pmd->rmsg || IDRM_MOVEALL == pmd->rmsg)
					SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_MOVE);
				else
					SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_COPY);

				SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			}
			break;
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;
//*/
	case WM_KEYDOWN:
		switch(wParam) 
		{
		case VK_RETURN:
			{
				char	FileName[64];
				char	path[256];
				nIndex = SendMessage(hFolderList, LB_GETCURSEL, 0, 0);
				PREBROW_GetData(&FolderListbuf, nIndex, FileName, path);
				if(strlen(szDstPath) > 1 && 0 == nIndex)
				{
					SendMessage(pmd->hRecMsg, pmd->rmsg, 0, (LPARAM)path);
				//	SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					PostMessage(hWnd, WM_CLOSE, 0, 0);
				}
//				else
//				{
//					strcpy(szDstPath, path);
//					LoadFolderList(hFrameWnd, hFolderList, &FolderListbuf, szDstPath, pmd->rmsg);
//				}	
			}	
			break;

		case VK_F5:
			{
				char	FileName[64];
				char	path[256];
				nIndex = SendMessage(hFolderList, LB_GETCURSEL, 0, 0);
				PREBROW_GetData(&FolderListbuf, nIndex, FileName, path);
				if(strlen(szDstPath) > 1 && 0 == nIndex)
				{
//					SendMessage(pmd->hRecMsg, pmd->rmsg, 0, (LPARAM)path);
//					PostMessage(hWnd, WM_CLOSE, 0, 0);
				}
				else
				{
					strcpy(szDstPath, path);
					LoadFolderList(hFrameWnd, hFolderList, &FolderListbuf, szDstPath, pmd->rmsg);
				}	
			}	
			break;

		case VK_F10:
			{
				int nType;
				if(strlen(szDstPath) <= 1)
				{
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				}
				nType = SND_GetFolderType(szDstPath);
				if(FOLDERTYPE_MMCROOT == nType || FOLDERTYPE_ROOT == nType)
				{
					if(SND_GetMMCStatus())
						strcpy(szDstPath, "");
					else
					{
						PostMessage(hWnd, WM_CLOSE, 0, 0);
						break;
					}
				}
				else
					SND_GotoParentFolder(szDstPath);
				LoadFolderList(hFrameWnd, hFolderList, &FolderListbuf, szDstPath, pmd->rmsg);
				break;
			}
			break;
		}
		break;

	case WM_DESTROY:
		if(hPhoneBmp)
		{
			DeleteObject(hPhoneBmp);
			hPhoneBmp = NULL;
		}
		if(hOpenDirBmp)
		{
			DeleteObject(hOpenDirBmp);
			hOpenDirBmp = NULL;
		}
		UnregisterClass(MOVETOFOLDER, NULL);
		PREBROW_FreeListBuf(&FolderListbuf);
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lResult;
}

static BOOL LoadFolderList(HWND hFrame, HWND hListBox, PLISTBUF pFolderListbuf, char *szPath, UINT wMsg)
{
	int index = -1;

	if(NULL == szPath)
		return FALSE;

	PREBROW_InitListBuf(pFolderListbuf);
	PREBROW_FreeListBuf(pFolderListbuf);
	memset(pFolderListbuf, 0, sizeof(LISTBUF));

	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
	
	if((strlen(szPath) <= 1) && SND_GetMMCStatus())
	{
		float fSpace;
		char szSpace[16];
		index = SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)IDS_PHONEMEM);
		SendMessage(hListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hPhoneBmp);
		PREBROW_AddData(pFolderListbuf, IDS_PHONEMEM, PHONESNDFILEDIR);

#ifndef _EMULATE_
		fSpace = (float)GetAvailFlashSpace();
#else
		fSpace = 1230.0;
#endif
//		fSpace /= 1024;
		if(fSpace < 1)
			sprintf(szSpace, "0.%dkB", (int)(10*fSpace));
		else
			sprintf(szSpace, "%dkB", (int)fSpace);
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)szSpace);
		
		index = SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)IDS_MMCCARD);
		SendMessage(hListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hMMCBmp);
		PREBROW_AddData(pFolderListbuf, IDS_MMCCARD, MMCSNDFOLDERPATH);

#ifndef _EMULATE_
		fSpace = (float)GetAvailMMCSpace();
#else
		fSpace = 956.0;
#endif
//		fSpace /= 1024;
		if(fSpace<1)
			sprintf(szSpace, "0.%dkB", (int)(10*fSpace));
		else
			sprintf(szSpace, "%dkB", (int)fSpace);
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)szSpace);

		SendMessage(hListBox, LB_SETCURSEL, 0, 0);
		SetWindowText(hFrame, IDS_SELECTDEST);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	}
	else
	{
		char szName[64] = "";
		int nType;

		if(strlen(szPath) <= 1)
			strcpy(szPath, PHONESNDFILEDIR);
		
		nType = SND_GetFolderType(szPath);
		if(FOLDERTYPE_ROOT == nType)
			strcpy(szName, IDS_CAPTION);
		else if(FOLDERTYPE_MMCROOT == nType)
			strcpy(szName, IDS_MMCCARD);
		else
			SND_GetFolderName(szDstPath, szName);

	//	index = SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)szName);
		index = SND_AddListBoxItem(hListBox, szName);
		SendMessage(hListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hOpenDirBmp);
		PREBROW_AddData(pFolderListbuf, szName, szDstPath);

		SND_AddFolderToList(hListBox, szDstPath, hFolderBmp, pFolderListbuf);
		SendMessage(hListBox, LB_SETCURSEL, 0, 0);

		SetWindowText(hFrame, IDS_SELECTFOLDER);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		if(IDRM_MOTOFOLDER == wMsg || IDRM_MOVEALL == wMsg)
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_MOVE);
		else
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_COPY);
	}
	return TRUE;
}

BOOL	RealtimeUpdateSound()
{
	if (IsWindow(hWndListBox)) 
	{
		bUpdated = TRUE;
		return TRUE;
	}
	return FALSE;
}

// Get the type of a specified folder
int SND_GetFolderType(const char *szPath)
{
	struct stat	filestat;
	DIR  *dirtemp = NULL;
	int nRootLen;

	if(NULL == szPath)
		return FOLDERTYPE_UNKNOWN;
	if(0 != stat(szPath, &filestat))
		return FOLDERTYPE_UNKNOWN;

	if(!S_ISDIR(filestat.st_mode))
		return FOLDERTYPE_UNKNOWN;

	nRootLen = strlen(PHONESNDFILEDIR);

	if(0 == strncasecmp(szPath, PHONESNDFILEDIR, nRootLen-1))
	{
		if((int)strlen(szPath) <= nRootLen)
			return FOLDERTYPE_ROOT;
		else
			return FOLDERTYPE_SUB;
	}

	nRootLen = strlen(MMCSNDFOLDERPATH);
	if(0 == strncasecmp(szPath, MMCSNDFOLDERPATH, nRootLen-1))
	{
		if((int)strlen(szPath) <= nRootLen)
			return FOLDERTYPE_MMCROOT;
		else
			return FOLDERTYPE_MMCSUB;
	}

	nRootLen = strlen(BENESNDFILEDIR);
	if(0 == strncasecmp(szPath, MMCSNDFOLDERPATH, nRootLen-1))
	{
		return FOLDERTYPE_BENE;
	}
	
	return FOLDERTYPE_UNKNOWN;
}

static void SND_InitMenus(void)
{
	hViewMenu = LoadMenuIndirect(&MainViewListMenuTemplate);
	hFolderHLMenu = LoadMenuIndirect(&FolderHLMenuTemplate);
	hSingalViewMenu = LoadMenuIndirect(&MainSingalViewListMenuTemplate);
	
	//begin initializing menu
	hDelManyMenu = GetSubMenu(hViewMenu, 9);
	hMoveManyMenu = GetSubMenu(hViewMenu, 4);
	hCopyManyMenu = GetSubMenu(hViewMenu,6);
	
	ModifyMenu(hFolderHLMenu, IDC_PLAY, MF_BYCOMMAND, IDC_PLAY, IDS_OPEN);
	ModifyMenu(hFolderHLMenu, IDC_DELETE, MF_BYCOMMAND, IDC_DELETE, IDS_DELETE);
	ModifyMenu(hFolderHLMenu, IDC_RENAME, MF_BYCOMMAND, IDC_RENAME, IDS_RENAME);
	ModifyMenu(hFolderHLMenu, IDC_CREATEFOLDER, MF_BYCOMMAND, IDC_CREATEFOLDER, IDS_CREATEFOLDER);
	
	ModifyMenu(hViewMenu, IDC_PLAY, MF_BYCOMMAND, IDC_PLAY, IDS_PLAYSND);
	ModifyMenu(hViewMenu, IDC_RENAME, MF_BYCOMMAND, IDC_RENAME, IDS_RENAME);
	ModifyMenu(hViewMenu, IDC_CREATEFOLDER, MF_BYCOMMAND, IDC_CREATEFOLDER, IDS_CREATEFOLDER);
	ModifyMenu(hViewMenu, IDC_MOVETOFOLDER, MF_BYCOMMAND, IDC_MOVETOFOLDER, IDS_MOVETOFOLDER);
	ModifyMenu(hViewMenu, 4, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hMoveManyMenu, IDS_MOVEMANY);
	ModifyMenu(hViewMenu, IDC_CPTOFOLDER, MF_BYCOMMAND, IDC_CPTOFOLDER, IDS_COPY);
	ModifyMenu(hViewMenu, 6, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hCopyManyMenu, IDS_COPYMANY);
	ModifyMenu(hViewMenu, IDC_SHOWDETAIL, MF_BYCOMMAND, IDC_SHOWDETAIL, IDS_SHOWDETAIL);
	ModifyMenu(hViewMenu, IDC_DELETE, MF_BYCOMMAND, IDC_DELETE, IDS_DELETE);
	ModifyMenu(hViewMenu, 9, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hDelManyMenu, IDS_DELETEMANY);
		
	ModifyMenu(hDelManyMenu, IDC_DELSELECT, MF_BYCOMMAND, IDC_DELSELECT, IDS_SELECT);
	ModifyMenu(hDelManyMenu, IDC_DELALL, MF_BYCOMMAND, IDC_DELALL, IDS_ALLINFOLDER);
	
	ModifyMenu(hMoveManyMenu, IDC_MOVESELECT, MF_BYCOMMAND, IDC_MOVESELECT, IDS_SELECT);
	ModifyMenu(hMoveManyMenu, IDC_MOVEALL, MF_BYCOMMAND, IDC_MOVEALL, IDS_ALLINFOLDER);

	ModifyMenu(hCopyManyMenu, IDC_COPYSELECT, MF_BYCOMMAND, IDC_COPYSELECT, IDS_SELECT);
	ModifyMenu(hCopyManyMenu, IDC_COPYALL, MF_BYCOMMAND, IDC_COPYALL, IDS_ALLINFOLDER);

	ModifyMenu(hSingalViewMenu, IDC_PLAY, MF_BYCOMMAND, IDC_PLAY, IDS_PLAYSND);
	ModifyMenu(hSingalViewMenu, IDC_RENAME, MF_BYCOMMAND, IDC_RENAME, IDS_RENAME);
	ModifyMenu(hSingalViewMenu, IDC_CREATEFOLDER, MF_BYCOMMAND, IDC_CREATEFOLDER, IDS_CREATEFOLDER);
	ModifyMenu(hSingalViewMenu, IDC_MOVETOFOLDER, MF_BYCOMMAND, IDC_MOVETOFOLDER, IDS_MOVETOFOLDER);
	ModifyMenu(hSingalViewMenu, IDC_CPTOFOLDER, MF_BYCOMMAND, IDC_CPTOFOLDER, IDS_COPY);
	ModifyMenu(hSingalViewMenu, IDC_SHOWDETAIL, MF_BYCOMMAND, IDC_SHOWDETAIL, IDS_SHOWDETAIL);
	ModifyMenu(hSingalViewMenu, IDC_DELETE, MF_BYCOMMAND, IDC_DELETE, IDS_DELETE);
}

static BOOL SND_SetNCArea(HWND hFrame, HWND hListBox, const char *szCurrentPath)
{
	int		ntype = 0;
	int		nIndex = -1;
	int		nItemType = FILE_TYPE_UNKNOW;

	if(NULL == hListBox || NULL == szCurrentPath)
		return FALSE;

	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_LEFTICON), NULL);
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_RIGHTICON), NULL);
	
	ntype = SND_GetFolderType(szCurrentPath);

	// set caption and right button text
	if(FOLDERTYPE_ROOT == ntype)
	{
		SetWindowText(hFrame, IDS_CAPTION);
	//	SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
		SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_EXIT);
	}
	else if(FOLDERTYPE_MMCROOT == ntype)
	{
		char szName[32];
		memset(szName, 0, 32);
		SND_GetMMCName(szName);
		SetWindowText(hFrame, szName);	//********
	//	SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_BACK);
	}
	else if(FOLDERTYPE_UNKNOWN != ntype)
	{
		char szDirName[64];
		if(SND_GetFolderName(szCurrentPath, szDirName))
			SND_SetWindowText(hFrame, szDirName);
		//	SetWindowText(hFrame, szDirName);
	//	SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_BACK);
	}
	else
		return FALSE;
	
	if ( SendMessage(hListBox, LB_GETCOUNT, 0, 0) > 0 )
	{
		nIndex = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
		if(LB_ERR == nIndex)
		{
			SendMessage(hWndListBox, LB_SETCURSEL, 0, 0);
			nIndex = 0;
		}
	//	GetFileNameFromList(hListBox, &ListBuffer, NULL, szTemp, curFileName, &nItemType);
		SND_GetInfoFromList(nIndex, &ListBuffer, NULL, NULL, &nItemType);
	}

	// set menu and left button text
	switch(nItemType)
	{
	case PREBROW_FILETYPE_FOLDER:
		SND_InsertMenu(hFolderHLMenu, 1, MF_BYPOSITION, IDC_RENAME, IDS_RENAME);
		if(FOLDERTYPE_SUB != ntype)
		{
			SND_InsertMenu(hFolderHLMenu, 2, MF_BYPOSITION, IDC_CREATEFOLDER, IDS_CREATEFOLDER);
			SND_InsertMenu(hFolderHLMenu, 3, MF_BYPOSITION, IDC_DELETE, IDS_DELETE);
		}
		else
		{
			DeleteMenu(hFolderHLMenu, IDC_CREATEFOLDER, MF_BYCOMMAND);
			SND_InsertMenu(hFolderHLMenu, 2, MF_BYPOSITION, IDC_DELETE, IDS_DELETE);
		}

		PDASetMenu(hFrame, hFolderHLMenu);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
	//	SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
		SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_PLAY,1), (LPARAM)IDS_OPEN);

		break;

	case SOUND_FILETYPE_MMCCARD:
		DeleteMenu(hFolderHLMenu, IDC_RENAME, MF_BYCOMMAND);
		DeleteMenu(hFolderHLMenu, IDC_DELETE, MF_BYCOMMAND);
		SND_InsertMenu(hFolderHLMenu, 1, MF_BYPOSITION, IDC_CREATEFOLDER, IDS_CREATEFOLDER);

		PDASetMenu(hFrame, hFolderHLMenu);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
	//	SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
		SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_PLAY,1), (LPARAM)IDS_OPEN);
		break;

	case PREBROW_FILETYPE_WAV:
	case PREBROW_FILETYPE_AMR:
		
		if(ISMULTISOUNDS(hListBox))
			hSoundMenu = hViewMenu;
		else
			hSoundMenu = hSingalViewMenu;
		
		// when the path is tooooo long, can't create folder in it
		if((FOLDERTYPE_SUB != ntype) && (strlen(szCurrentPath) < 480))	//********
			SND_InsertMenu(hSoundMenu, 2, MF_BYPOSITION, IDC_CREATEFOLDER, IDS_CREATEFOLDER);
		else
			DeleteMenu(hSoundMenu, IDC_CREATEFOLDER, MF_BYCOMMAND);
		
		nIndex = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
		if(LB_ERR != nIndex)
		{
			if(nIndex == nPlayingIndex)
			{
				ModifyMenu(hSoundMenu, IDC_PLAY, MF_BYCOMMAND|MF_STRING, IDC_STOP, IDS_STOPSND);
			//	SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_STOPSND);
				SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_STOP,1), (LPARAM)IDS_STOPSND);
			}
			else
			{
				ModifyMenu(hSoundMenu, IDC_STOP, MF_BYCOMMAND|MF_STRING, IDC_STOP, IDS_PLAYSND);
			//	SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PLAYSND);
				SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_PLAY,1), (LPARAM)IDS_PLAYSND);
			}
		}
		
		PDASetMenu(hFrame, hSoundMenu);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		
		break;

	default:
		PDASetMenu(hFrame, NULL);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
	//	ShowWindow(hListBox, SW_HIDE);
		break;
	}

	SND_SetListBoxFocus(hListBox);
	return TRUE;
}	

// load sounds and folders
static	int	LoadMainViewList(const char *szPath)
{
//	DIR		*dir;
	int		num/*, nFlags*/;
	BOOL	bMMC = FALSE;
	int		ntype = FOLDERTYPE_UNKNOWN;

	if(NULL == szPath)
		return 0;
	// when the path is tooooo long, can't enter it
	if(strlen(szPath) > 480)	//***********
		return 0;

	ntype = SND_GetFolderType(szPath);

//	bFolderOpen = FALSE;
	num = AddSoundsToList(hWndListBox, szPath, &ListBuffer, (FOLDERTYPE_SUB != ntype));

	// add MMC root folder
	if(FOLDERTYPE_ROOT == ntype)
	{
		if(SND_GetMMCStatus())
		{
			int index;
		//	char szMMCName[64];
			char pInfo[32];
			long nNum;
			
			nNum = GetSndFileNumInDir(MMCSNDFOLDERPATH);
			
		//	sprintf(szMMCName, "%s_/%d", IDS_MMCCARD, index);	//*******
		//	index = SendMessage(hWndListBox,LB_ADDSTRING,NULL,(LPARAM)szMMCName);
			memset(pInfo, 0, 32);
			SND_GetMMCName(pInfo);
			index = SendMessage(hWndListBox,LB_ADDSTRING,NULL,(LPARAM)pInfo);
			if (LB_ERR != index)
			{
				PREBROW_AddSndData(&ListBuffer, pInfo, MMCSNDFOLDERPATH, 0);	//********
				if(nNum != -1)
				{
					sprintf(pInfo, "%s%d    %s%d", ML("Folder:"), HIWORD(nNum), ML("Sounds:"), LOWORD(nNum));
					SendMessage(hWndListBox, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)pInfo);
				}
				SendMessage(hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hMMCBmp);
				SendMessage(hWndListBox, LB_SETITEMDATA, (WPARAM)index, (LPARAM)SOUND_FILETYPE_MMCCARD);
			}
			num ++;
		}
	}
	
//	if ( num > 0 )
//		ShowWindow(hWndListBox, SW_SHOW);
//	else
//		ShowWindow(hWndListBox, SW_HIDE);
	SND_ShowListBox(hWndListBox);

	SND_SetNCArea(hFrameWnd, hWndListBox, szCurPath);
	
	return	num;
}

// change to the parent dir
BOOL SND_GotoParentFolder(char *szPath)
{
	int nlen, ntype;
	char *p1, *p2;

	if(NULL == szPath)
		return FALSE;

	ntype = SND_GetFolderType(szPath);

	if(FOLDERTYPE_ROOT == ntype)
		return FALSE;
	else if(FOLDERTYPE_MMCROOT == ntype)
	{
		strcpy(szPath, PHONESNDFILEDIR);
		return TRUE;
	}

	nlen = strlen(szPath) - 1;

	while(('/' == szPath[nlen]) || ('\\' == szPath[nlen]))
	{
		nlen--;
		if(nlen <= 0)
			return FALSE;
	}
	szPath[nlen] = '\0';

	p1 = strrchr(szPath, '/');
	p2 = strrchr(szPath, '\\');

	if(p2 > p1)
		p1 = p2;

	if(NULL == p1)
		return	FALSE;

	*p1 = '\0';

	return TRUE;
}

BOOL SND_GetMMCStatus()
{
#ifdef _EMULATE_
#ifndef _SOUND_LOCAL_TEST_
	return FALSE;
#endif
#else
	if (MMC_CheckCardStatus())
#endif
	{
		DIR *dir = NULL;
		if ((dir = opendir(MMCSNDFOLDERPATH)) != NULL)
		{
			closedir(dir);
			return TRUE;
		}
		else
		{
			if(0 == mkdir(MMCSNDFOLDERPATH, 0666))
				return TRUE;
			else
				return FALSE;
		}
	}
	return FALSE;
}

// get free space of certain memory (in kilo bytes)
int SND_GetFreeSpace(const char *szDstPath)
{
	if(NULL == szDstPath)
		return 0;
	
#ifndef _EMULATE_
	if(strncasecmp(szDstPath, PHONESNDFILEDIR, strlen(PHONESNDFILEDIR)-1) == 0)
		return GetAvailFlashSpace();
	else if(strncasecmp(szDstPath, MMCSNDFOLDERPATH, strlen(MMCSNDFOLDERPATH)-1) == 0)
		return GetAvailMMCSpace();
#else
	return 3766;
#endif
}

static int SND_CheckSingalSound(HWND hListBox)
{
	int i, nCount;
	int nSound, nItemData;

	if(NULL == hListBox)
		return -1;

	nCount = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
	if(nCount <= 0)
		return -1;

	nSound = 0;
	for(i=0; i<nCount; i++)
	{
		nItemData = SendMessage(hListBox, LB_GETITEMDATA, i, 0);
		if(ISSOUNDFILE(nItemData))
		{
			nSound ++;
			if(nSound > 1)
				return nSound;
		}
		else
			return nSound;
	}
	return nSound;
}

// update folder's description after file operations (move/copy/delete)
static void SND_UpdateDirInfo(HWND hListBox, LISTBUF *pListBuf, PCSTR szPath)
{
	int index;
	index = SearchSndData(&ListBuffer, szPath, PREBROW_FILETYPE_FOLDER);
	if(index >= 0)
	{
		char info[32];
		long lNum = GetSndFileNumInDir(szPath);
		
		if(-1 == lNum)
			return;
		
		sprintf(info,"%s%d    %s%d", ML("Folder:"), HIWORD(lNum), 
			ML("Sounds:"), LOWORD(lNum));
		
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)info);
	}
}

// check if there's enough space to move files
BOOL SND_IsMoveAvailable(PCSTR pSrc, PCSTR pDst)
{
//	if(SND_IsInFolder(PHONESNDFILEDIR, pSrc))
//	{
//		if(SND_IsInFolder(PHONESNDFILEDIR, pDst))
//			return TRUE;
		/*else */if(SND_GetFreeSpace(pDst) < 500 + SND_GetFileSize(pSrc))
			return FALSE;
		else
			return TRUE;
//	}
//	else
//	{
//		if(SND_IsInFolder(MMCSNDFOLDERPATH, pDst))
//			return TRUE;
//		else if(SND_GetFreeSpace(pDst) < 500 + SND_GetFileSize(pSrc))
//			return FALSE;
//		else
//			return TRUE;
//	}
//	return FALSE;
}

BOOL SND_GetMMCName(char *szName)
{
#ifndef _EMULATE_
	if(!MMC_CheckCardStatus())
	{
		strcpy(szName, IDS_MMCCARD);
		return FALSE;
	}
	
	if(GetMMCName(szName))
	{
		if(strlen(szName) <= 0)
			strcpy(szName, IDS_MMCCARD);
		return TRUE;
	}
#endif
	strcpy(szName, IDS_MMCCARD);
	return TRUE;
}

BOOL SND_GetInfoFromList(int nIndex, PLISTBUF pListBuffer, char* cFileName, char* szFullFileName, int *nFileType)
{
	PLISTDATA pData = NULL;

	if(NULL == pListBuffer)
		return FALSE;

	if(nIndex < 0 || nIndex >= pListBuffer->nDataNum)
		return FALSE;

	pData = PREBROW_GetPDATA(pListBuffer, nIndex);
	pListBuffer->pCurrentData = pData;
	if(NULL == pData)
		return FALSE;

	if(NULL != cFileName)
		SND_MultibyteStrcpy(cFileName, pData->szData);
	if(NULL != szFullFileName)
		strcpy(szFullFileName, pData->szFullData);
	if(NULL != nFileType)
		*nFileType = pData->nFVLine;
	
	return TRUE;
}
