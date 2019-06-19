/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2005 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : DLM application
 *
 * Purpose  : dynamic load apps
 *
\**************************************************************************/
#include "plx_pdaex.h"
#include "imesys.h"
#include "string.h"
#include "winpda.h"
#include "pubapp.h"
#include "stdlib.h"
#include "stdio.h"
#include "statfs.h"

#include "sys/stat.h"
#include "unistd.h"
#include "fcntl.h"
#include "dirent.h"
#include "dir.h"
#include "fapi.h"
#include "public/PreBrowHead.h" 
#include "pthread.h"
#include "project/pmi.h"
#include "dlmui.h"

#include "zlib.h"

extern int zlib_inflate_workspacesize (void);
extern BOOL MMC_CheckCardStatus() ;
extern BOOL DeleteDlmApp(char *strIconName) ;
extern BOOL RunDlmApp (unsigned char *path) ;
extern BOOL MAIL_CreateEditInterface(HWND hFrameWindow, char *PSZRECIPIENT, char *PSZATTENMENT, char *PSZATTNAME, BOOL bAtt);
extern BOOL BtSendData (HWND hAppFrameWnd,char* szAppPathName,char* szViewFileName,int iFileType);

static LRESULT MainWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static void AddFileToList (unsigned char *path, int canInstall) ;
static void ShowDeviceList () ;
static void DlmMainWinCreate(HWND hWnd) ;
static char *GetFileExt(char *name) ;
static void DlmMainWinCommad(HWND hWnd, WPARAM wParam, LPARAM lParam) ;
static int GetDirInfomation (char* path) ;
static void ResetMenu() ;
static void InstallPLX (int dest) ;
static void RunELF () ;
static void ShowEmptyFolder (unsigned char *path) ;
static int InstallDestSelect () ;
static int CheckMemory(int dest, unsigned char* plxpath) ;
static int SetMenuAndSoftKeys () ;
static int bCancel = FALSE ;
static void* InstallThread (void *para) ;
static BOOL DLM_CopyFile(PCSTR pOldFileName, PCSTR pNewFileName) ;
static void ComfirmDelete () ;
static void DLM_DeleteFile() ;
static BOOL DLM_DeleteDir (unsigned char *pathname) ;
static void ShowDetail () ;
void CreateListBox(HWND hWnd, unsigned lbs_sort) ;
void Dlm_MakeIconName(unsigned char *destpath, unsigned char *g_fn, unsigned char *IconName, unsigned char *IconName1) ;
static int GetPlxFileInfo (unsigned char *plxFile) ;
static int DLM_GetDirSize (unsigned char *pPath) ;

static void SendViaBT () ;
static void SendViaEML () ;



static BOOL bInInstall = FALSE ;
static HINSTANCE hInstance = NULL;
static HWND Dlm_MainWndApp;
static HWND hDlmFrameWnd;
static HMENU hMenu, hMenuSend;
static HBITMAP hBmpMMC, hBmpFLS, hBmpInstalled, hBmpUninstalled, 
				hBmpEmpty, hBmpLeftArrow, hBmpRightArrow, 
				hBMPRadioSelect, hBMPRadioUnselect; 

static HWND hFileList=NULL;
static HWND hFormViewer=NULL ;

// winlevel : 0: device list view, 1: file list view, 2:empty folder view.
//            3: select install destination, 4: detail
static int WinLevel = 0 ;
static unsigned char *CurrentDir = NULL ;
static installState = INSTALL_OK ;
static BOOL installOnly = FALSE ;

static unsigned char g_fn[_MAX_FNAME] = "";					// 被安装文件的名字
static unsigned char path[_MAX_FNAME] = "" ;				// 被安装文件所在路径
static unsigned char destpath[_MAX_FNAME] = "" ;			// 安装的目标路径
static 	pthread_t thread;									// 安装线程
static APPGROUP app ;
/**********************************************************************
* Function	 Dlm_AppControl
* Purpose    application main function
* Params	
* Return	
* Remarks
**********************************************************************/
DWORD Dlm_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
	WNDCLASS	    wc;
	DWORD		    dwRet;
	RECT			rClient;
    dwRet = TRUE;
	
//	dwRet = DLMGetOccupiedSize (0) ;
//	dwRet = DLMGetOccupiedSize (1) ;

    switch (nCode)
    {
		case APP_INIT:
			hInstance = (HINSTANCE)pInstance;
			break;

		case APP_GETOPTION:
			switch (wParam)
			{
			case AS_APPWND:
				dwRet = (DWORD)Dlm_MainWndApp;
				break;
			}
			break;

		case APP_ACTIVE:

			if(IsWindow(hDlmFrameWnd))
			{
				ShowWindow(hDlmFrameWnd, SW_SHOW);
				ShowOwnedPopups(hDlmFrameWnd, SW_SHOW);
				UpdateWindow(hDlmFrameWnd);
			}
			else
			{
				if(wParam)
				{
					installOnly = TRUE ;
					CurrentDir = NULL ;
					//strcpy (path, (const char*)wParam) ;
				}
				else
					installOnly = FALSE ;
				
				wc.cbClsExtra = 0;
				wc.cbWndExtra = 0;
				wc.hbrBackground = NULL;
				wc.hCursor = NULL;
				wc.hIcon = NULL;
				wc.hInstance = NULL;
				wc.lpfnWndProc = MainWndProc;
				wc.lpszClassName = IDS_CLASSNAME;
				wc.lpszMenuName = NULL;
				wc.style = 0;
				if(!RegisterClass(&wc))
					return FALSE;
				hMenu= CreateMenu () ;
				
				// 创建send菜单
				hMenuSend = CreateMenu() ;
				AppendMenu(hMenuSend, MF_STRING, ID_VIAEMAIL, IDS_VIAEMAIL) ;
				AppendMenu(hMenuSend, MF_STRING, ID_VIABT, IDS_VIABT) ;
				
				hDlmFrameWnd = CreateFrameWindow( WS_CAPTION | PWS_STATICBAR);
				GetClientRect(hDlmFrameWnd,&rClient);
				Dlm_MainWndApp = CreateWindow(
					IDS_CLASSNAME,
					WIN_TITLE,
					WS_VISIBLE | WS_CHILD,
					rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
					hDlmFrameWnd,
					NULL,
					NULL,
					NULL
					);

				
				ShowWindow(Dlm_MainWndApp, SW_SHOW);
				UpdateWindow(Dlm_MainWndApp);

			}
			break;

		case APP_INACTIVE:
			ShowOwnedPopups(hDlmFrameWnd, SW_HIDE);
			ShowWindow(hDlmFrameWnd,SW_HIDE); 
			break;

		default:
			break;
    }

    return dwRet;
}

/**********************************************************************
* Function	 MainWndProc
* Purpose     main application function
* Params	
* Return	
* Remarks
**********************************************************************/
static LRESULT MainWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = TRUE;
	
	switch (wMsgCmd)
    {
		case WM_CREATE:
			
			// 创建左右菜单
		
			SendMessage(hDlmFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_ENTER, 1), (LPARAM)"");
			SendMessage(hDlmFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_EXIT, 0), (LPARAM)"");
			SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)IDS_SELECT); 
		
			SetWindowText(hDlmFrameWnd,WIN_TITLE) ;
			// 设置菜单
			PDASetMenu(hDlmFrameWnd, hMenu);
			
			// 设置焦点
			SetFocus(hWnd);

			// 创建窗口的操作
			DlmMainWinCreate (hWnd) ;

			break;

		case PWM_SHOWWINDOW:
			SetMenuAndSoftKeys () ;
			break;

		case WM_COMMAND:
			DlmMainWinCommad(hWnd, wParam, lParam) ;
			break;
		
		case ID_DELETED:
			AddFileToList(CurrentDir, 0) ;
			break ;

		case ID_DELETE_COFIRM:
		
			if (lParam == 1)
			{
				DLM_DeleteFile () ;
				PLXTipsWin(hDlmFrameWnd,Dlm_MainWndApp,ID_DELETED,IDS_DELETED,NULL,Notify_Success,IDS_OK, NULL, SECOND(2)) ;
			}
			else
				PostMessage(hWnd, ID_DELETED, 0, 0 ) ;
			
			break ;
		case ID_INSTALL_COMPLETE:
			if (installOnly)
			{
				PostMessage(hWnd,WM_CLOSE, 0 ,0) ;
			}
			else
				AddFileToList(CurrentDir, 0) ;
			break ;

		case ID_INSTALL_OK:
			printf ("\r\nGet ID_INSTALL_OK.") ;

			
//			while(bInInstall) ;
			pthread_join(thread,NULL) ;
			
			if(installState == INSTALL_FAILED)
			{
				PLXTipsWin(hDlmFrameWnd,Dlm_MainWndApp,ID_INSTALL_COMPLETE,IDS_INSTALL_FAILED,NULL,Notify_Failure,IDS_OK,NULL,SECOND(2)) ;
			}
			else
			{
				if(wParam==1)	// reinstall
				{
					unsigned char IconName[256] ;
					Dlm_MakeIconName(destpath, g_fn, IconName, NULL) ;
					DeleteDlmApp(IconName) ;
				}
				AddDlmApp(&app) ;
				PLXTipsWin(hDlmFrameWnd,Dlm_MainWndApp,ID_INSTALL_COMPLETE,IDS_COMPLETE,NULL,Notify_Success,IDS_OK, NULL, SECOND(2)) ;
			}
			WaitWin(Dlm_MainWndApp, FALSE, IDS_INSTALLING, WIN_TITLE,NULL,IDS_CANCEL, ID_INSTALL_CANCEL);
			
			printf ("\r\nID_INSTALL_OK Over.") ;
			break ;
		
		case ID_INSTALL_CANCEL:
			printf ("\r\nGet ID_INSTALL_CANCEL.") ;
			bCancel = TRUE ;
			
//			while(bInInstall) ;
			pthread_join(thread,NULL) ;
			
			WaitWin(Dlm_MainWndApp, FALSE, IDS_INSTALLING, WIN_TITLE,NULL,IDS_CANCEL, ID_INSTALL_CANCEL);
			
			if (installOnly)
			{
				PostMessage(hWnd,WM_CLOSE, 0 ,0) ;
			}
			else
				AddFileToList(CurrentDir, 0) ;

			printf ("\r\nID_INSTALL_CANCEL Over.") ;
			break ;
		case ID_NOT_PLXFILE :
			PostMessage(Dlm_MainWndApp, WM_KEYDOWN, VK_RETURN, 0) ;
			break ;
		case WM_KEYDOWN:
			switch(LOWORD(wParam))
			{
				case VK_RIGHT:
					if(WinLevel==1||WinLevel==2)
					{
						if (MMC_CheckCardStatus())
						{
							AddFileToList(DLM_DIR_CARD, 0) ;
						}
					}
					break ;
				case VK_LEFT:
					if(WinLevel==1||WinLevel==2)
					{
						AddFileToList(DLM_DIR_MOBILE, 0) ;
					}
					break ;
					
				case VK_F10:
					// 右 softkey
					if(WinLevel==3)
					{
						if (installOnly)
						{
							PostMessage(hWnd,WM_CLOSE, 0 ,0) ;
						}
						else
							AddFileToList(CurrentDir, 0) ;
					}
					else if(WinLevel==1||WinLevel==2)
						PostMessage (hWnd, WM_COMMAND, ID_BACK, 0) ;
					else if(WinLevel==0)
					{	
						PostMessage(hWnd, WM_CLOSE, 0, 0);
					//	SendMessage(hDlmFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					}
					break;
				case VK_F5:
					// 中 softkey
					if(WinLevel==3)
						PostMessage(hWnd, WM_COMMAND, ID_INSTALLTO, 0) ;
					else if(WinLevel==2)
						break ;
					else if(WinLevel==1)
						SendMessage(hDlmFrameWnd, wMsgCmd, wParam, lParam);
					else if(WinLevel==0)
						SendMessage(hWnd,WM_COMMAND,ID_ENTER,0) ;
					break;
				case VK_RETURN:
					// 左 softkey
					if(WinLevel==2)
						break ;
					else if (WinLevel==1)
						PostMessage (hWnd, WM_COMMAND, ID_RUN, 0) ;
					else if (WinLevel == 4)
					{
						DestroyWindow(hFormViewer) ;
						hFormViewer = NULL ;

						AddFileToList(CurrentDir,0) ;
						break ;
					}
					break;
				
				default:
					lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
					break;
			}
			break;
		case WM_PAINT:
			BeginPaint(hWnd, NULL);
			
			if(WinLevel==2)
				ShowEmptyFolder(CurrentDir) ;
			
			EndPaint(hWnd, NULL);
			break;
			
		case WM_CLOSE:

			SendMessage(hDlmFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			DestroyWindow(hWnd);
			
			break;
		case WM_DESTROY:
			DestroyMenu(hMenuSend);
			DestroyMenu(hMenu);

			DeleteObject(hBmpMMC);
			DeleteObject(hBmpFLS);
			DeleteObject(hBmpInstalled);
			DeleteObject(hBmpUninstalled);
			DeleteObject(hBmpEmpty) ;
			DeleteObject(hBmpLeftArrow) ;
			DeleteObject(hBmpRightArrow) ;
			DeleteObject(hBMPRadioSelect) ;
			DeleteObject(hBMPRadioUnselect) ;
			
			hBmpMMC = hBmpFLS = NULL ;
			hBmpInstalled = hBmpUninstalled = NULL ;
			hBmpEmpty = NULL ;
			hBmpLeftArrow = hBmpRightArrow = NULL ;
			hBMPRadioUnselect = hBMPRadioSelect = NULL ;
			hFileList = NULL;
			hMenu = hMenuSend = NULL ;
			hFormViewer = NULL ;

			UnregisterClass(IDS_CLASSNAME, NULL);
			
			DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance); 

			hDlmFrameWnd = NULL ;
			break ;

		default:
			lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
	}

	return lRet;
}

//////////////////////////////////////////////////////////////////////////////
// Function	DlmMainWinCreate
// Purpose	主窗口处理WM_CREATE消息
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static void DlmMainWinCreate(HWND hWnd)
{
	Dlm_MainWndApp = hWnd ;
	CreateListBox(hWnd, 0) ;
	{
		HDC hdc ;
		COLORREF Color;
		BOOL bTrans;

		hdc = GetDC(hWnd);

		// 5个用到的图标
		hBmpMMC = CreateBitmapFromImageFile(hdc, ICON_MMC, &Color, &bTrans);
		hBmpFLS = CreateBitmapFromImageFile(hdc, ICON_FLS, &Color, &bTrans);
		hBmpInstalled = CreateBitmapFromImageFile(hdc, ICON_INSTALLED, &Color, &bTrans);
		hBmpUninstalled = CreateBitmapFromImageFile(hdc, ICON_UNINSTALLED, &Color, &bTrans);
		hBmpEmpty  = CreateBitmapFromImageFile(hdc, ICON_EMPTY, &Color, &bTrans);
		hBmpLeftArrow = CreateBitmapFromImageFile(hdc, LEFT_ICON, &Color, &bTrans);
		hBmpRightArrow = CreateBitmapFromImageFile(hdc, RIGHT_ICON, &Color, &bTrans);
		hBMPRadioUnselect = CreateBitmapFromImageFile(hdc, UNSELECT_ICON, &Color, &bTrans);
		hBMPRadioSelect = CreateBitmapFromImageFile(hdc, SELECT_ICON, &Color, &bTrans);
		
		ReleaseDC(hWnd,hdc) ;
	}

	// 设置焦点
//	SetFocus(hFileList) ;
	// 列出设备表
	if (!installOnly)
		ShowDeviceList () ;
	else
	{
		if (InstallDestSelect ())
		{
			InstallPLX(1) ;
		}
	}


//	DLM_CopyFile ("/rom/crc32.plx", "/mnt/flash/dlm/crc32.plx") ;
//	DLM_CopyFile ("/rom/gamerus.plx", "/mnt/flash/dlm/gamerus.plx") ;
	DLM_CopyFile ("/rom/gamerus.plx", "/mnt/flash/dlm/gamerus.plx") ;
/*
	{
		int hf;
		hf = open("/mnt/flash/dlm/aa.plx",O_WRONLY | O_CREAT,0) ;
		write (hf, "aaaaa", 5) ;
		close(hf) ;
		hf = open("/mnt/flash/dlm/bb.plx",O_WRONLY | O_CREAT,0) ;
		write (hf, "bbbbb", 5) ;
		close(hf) ;

		hf = open("/mnt/flash/dlm/cc.plx",O_WRONLY | O_CREAT,0) ;
		write (hf, "ccccc", 5) ;
		close(hf) ;
	}
*/

}

//////////////////////////////////////////////////////////////////////////////
// Function	ShowDeviceList
// Purpose 列出mmc/flash
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static void ShowDeviceList ()
{
	int ret = 1 ;
	unsigned char temp[32] ;
//	unsigned long wstyle ;
	int irsertpos ;

	// 删除列表项
	CreateListBox(Dlm_MainWndApp, 0) ;
/*	while (ret!=LB_ERR)
		ret = SendMessage (hFileList,LB_DELETESTRING,0,0) ;
	
	wstyle = GetWindowLong(hFileList,GWL_STYLE) ;
	SetWindowLong(hFileList, GWL_STYLE,wstyle&(~LBS_SORT)) ;
*/
	// 插入列表项 mmc/flash

	irsertpos = SendMessage (hFileList,LB_ADDSTRING,0,(unsigned long)IDS_MOBILE) ;
	SendMessage(hFileList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, irsertpos), (LPARAM)hBmpFLS) ;
	ret = GetDirInfomation (DLM_DIR_MOBILE) ;
	sprintf (temp,IDS_TOTALFILE, ret) ;
	SendMessage(hFileList, LB_SETAUXTEXT, MAKEWPARAM(irsertpos, -1), (LPARAM)temp);
	// 监测mmc卡
	if (MMC_CheckCardStatus())
	{
		// 有mmc卡，显示mmc
		irsertpos = SendMessage (hFileList, LB_ADDSTRING, 1, (unsigned long)IDS_CARD) ;
		SendMessage(hFileList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, irsertpos), (LPARAM)hBmpMMC) ;
		ret = GetDirInfomation (DLM_DIR_CARD) ;
		sprintf (temp,IDS_TOTALFILE, ret) ;
		SendMessage(hFileList, LB_SETAUXTEXT, MAKEWPARAM(irsertpos, -1), (LPARAM)temp);
	}
	
	// 设置列表项当前选中状态
	SendMessage (hFileList,LB_SETCURSEL,0,0) ;
	
	WinLevel = 0 ;
	
	SetMenuAndSoftKeys () ;
}

//////////////////////////////////////////////////////////////////////////////
// Function	AddFileToList
// Purpose 列出mmc/flash上面的文件
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static void AddFileToList (unsigned char *path, int canInstall)
{
	struct dirent *pdirent =NULL;
	DIR *pdir = NULL ;
	struct stat sbuf;
//	unsigned long wstyle ;

	int ret = 1 ;
	
	ShowWindow(hFileList, SW_SHOW) ;
	SetFocus(hFileList) ;
	
	CurrentDir = path ;
	// 空目录
	if (GetDirInfomation (path) == 0)
	{
		// 显示设备空界面
		ShowEmptyFolder (path) ;
		return ;
	}
	//打开目录
	chdir(path) ;
	
	pdir = opendir(path);

	if(pdir == NULL)
	{
		// 打开失败，创建
		mkdir (path, 0) ;
		pdir = opendir(path);
		if(pdir == NULL) // 不可能！！
		{
			// 显示设备空界面
			ShowEmptyFolder (path) ;
			return ;
		}
	}
	
	// 读目录
	pdirent = readdir(pdir);

	if(pdirent == NULL)	// 基本不可能
	{
		closedir(pdir);
		// 显示设备空界面
		ShowEmptyFolder (path) ;
		return ;
	}
	
	//删除 listbox 内容
	CreateListBox(Dlm_MainWndApp, LBS_SORT) ;
/*	while (ret!=LB_ERR)
		ret = SendMessage (hFileList,LB_DELETESTRING,0,0) ;

	wstyle = GetWindowLong(hFileList,GWL_STYLE) ;
	SetWindowLong(hFileList, GWL_STYLE,wstyle|LBS_SORT) ;
*/
	//重置 listbox 内容
	do {
		// 读状态
		ret = stat(pdirent->d_name, &sbuf);
		if(ret!=-1) 
		{
			// 是目录
			if(S_ISDIR(sbuf.st_mode))
			{
				// 加入名称，设置图标，设置附加数据
				if((pdirent->d_name[0]=='.'&&pdirent->d_name[1]==0)||(pdirent->d_name[0]=='.'&&pdirent->d_name[1]=='.'&&pdirent->d_name[2]==0))
				{
				}
				else
				{
					ret = SendMessage (hFileList,LB_ADDSTRING,0,(unsigned long)pdirent->d_name) ;
					SendMessage(hFileList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, ret), (LPARAM)hBmpInstalled) ;
					SendMessage(hFileList, LB_SETITEMDATA, ret, ITEM_DIR) ;
				}
			}
			else if (strcmp(GetFileExt(pdirent->d_name),DLM_EXT)==0)
			{
				// 加入名称，设置图标，设置附加数据
				ret = SendMessage (hFileList,LB_ADDSTRING,0,(unsigned long)pdirent->d_name) ;
				SendMessage(hFileList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, ret), (LPARAM)hBmpUninstalled) ;
				SendMessage(hFileList, LB_SETITEMDATA, ret, ITEM_FILE) ;
			}
		}

		pdirent = readdir(pdir);
	} while(pdirent !=NULL );
	
	closedir(pdir);
	
	// 设置列表项当前选中状态
	SendMessage (hFileList,LB_SETCURSEL,0,0) ;

	ShowWindow(hFileList, SW_SHOW) ;
	SetFocus(hFileList) ;

	WinLevel = 1 ;
	SetMenuAndSoftKeys () ;
}

//////////////////////////////////////////////////////////////////////////////
// Function	DlmMainWinCommad
// Purpose	主窗口处理WM_COMMAND消息
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static void DlmMainWinCommad(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int cursel = LB_ERR ;
	switch(LOWORD(wParam))
	{
		case ID_INSTALLTO:
			cursel = SendMessage(hFileList, LB_GETCURSEL, 0, 0) ;
			// cursel = 0	flash 
			// cursel = 1	mmc
			InstallPLX (cursel) ;
			break ;
		case ID_LISTBOX:
			if(HIWORD(wParam)==LBN_SELCHANGE)
			{
				if(WinLevel==3)
				{
					int ret ;
					ret = SendMessage(hFileList, LB_GETCURSEL, 0, 0) ;
					SendMessage(hFileList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, ret), (LPARAM)hBMPRadioSelect) ;
					ret = ret ? 0 : 1;
					SendMessage(hFileList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, ret), (LPARAM)hBMPRadioUnselect) ;
				}
				else if (WinLevel==1) 
				{
					ResetMenu() ;
				}
			}
			break ;
		case ID_BACK:
			ShowDeviceList () ;
			break ;
		
		case ID_ENTER:
			cursel = SendMessage (hFileList, LB_GETCURSEL, 0, 0) ;
			if (cursel != LB_ERR)
			{
				unsigned char temp[32];
				SendMessage(hFileList,LB_GETTEXT,(WPARAM)cursel,(LPARAM)temp) ;
				if(!strcmp(temp, IDS_FLASH))
				{
					AddFileToList(DLM_DIR_MOBILE, 0) ;	
				}
				else
				{
					AddFileToList(DLM_DIR_CARD, 1) ;
				}
			}
			break ;
		
		case ID_EXIT:

			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case ID_RUN:
		case ID_INSTALL:
			{
				int cursel ;
				int type ;

				if (strcmp(CurrentDir, DLM_DIR_CARD) == 0)
				{
					if(!MMC_CheckCardStatus())
					{
						PLXTipsWin(hDlmFrameWnd, Dlm_MainWndApp, ID_NOCARD, 
							IDS_NOCARD, NULL, Notify_Failure, IDS_OK, NULL, SECOND(2));			
						break;
					}
				}

				cursel = SendMessage (hFileList, LB_GETCURSEL, 0, 0) ;
				type =SendMessage(hFileList,LB_GETITEMDATA,cursel,0) ;
				if(type&ITEM_DIR)
				{
					RunELF () ;
				}
				else if (type&ITEM_FILE)
				{
					if (InstallDestSelect ())
					{
						InstallPLX(1) ;
					}
				}
			}
			break;
		case ID_VIAEMAIL:
			SendViaEML () ;
			break ;
		case ID_VIABT:
			SendViaBT () ;
			break ;
		case ID_DELETE:
			ComfirmDelete () ;
			break ;
		case ID_DETAIL:
			ShowDetail () ;
			break ;
	}
}

static void SendViaBT ()
{
	char fn[30];
	int cursel ;

	if (strcmp(CurrentDir, DLM_DIR_CARD) == 0)
	{
		if(!MMC_CheckCardStatus())
		{
			PLXTipsWin(hDlmFrameWnd, Dlm_MainWndApp, ID_NOCARD, 
				IDS_NOCARD, NULL, Notify_Failure, IDS_OK, NULL, SECOND(2));			
			return ;
		}
	}

	cursel = SendMessage (hFileList, LB_GETCURSEL, 0, 0) ;
	SendMessage (hFileList, LB_GETTEXT, cursel, (LPARAM) fn) ;

	BtSendData(hDlmFrameWnd, CurrentDir, fn, 1);
}

static void SendViaEML ()
{
	char fn[30];
	char cAbsolutePath[_MAX_PATH];
	int cursel ;

	if (strcmp(CurrentDir, DLM_DIR_CARD) == 0)
	{
		if(!MMC_CheckCardStatus())
		{
			PLXTipsWin(hDlmFrameWnd, Dlm_MainWndApp, ID_NOCARD, 
				IDS_NOCARD, NULL, Notify_Failure, IDS_OK, NULL, SECOND(2));			
			return ;
		}
	}

	cursel = SendMessage (hFileList, LB_GETCURSEL, 0, 0) ;
	SendMessage (hFileList, LB_GETTEXT, cursel, (LPARAM) fn) ;
	strcpy(cAbsolutePath, CurrentDir) ;

	strcat(cAbsolutePath,fn) ;

	MAIL_CreateEditInterface(hDlmFrameWnd, NULL, cAbsolutePath, fn, 1);
}

//////////////////////////////////////////////////////////////////////////////
// Function	GetFileExt
// Purpose 获得文件的扩展名
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static char *GetFileExt(char *name)
{
	while (name && *name && *name!='.')
		name ++ ;
	return name ;
}

//////////////////////////////////////////////////////////////////////////////
// Function	GetDirInfomation
// Purpose 获得目录里面文件个数
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static int GetDirInfomation (char* path)
{
	struct dirent *pdirent =NULL;
	DIR *pdir = NULL ;
	struct stat sbuf;
	int ret = 1 ;
	int plxnum = 0 ;
	int dirnum = 0 ;	

	//打开目录
	chdir(path) ;
	
	pdir = opendir(path);

	if(pdir == NULL)
	{
		// 打开失败，创建
		mkdir (path, 0) ;
		pdir = opendir(path);
		if(pdir == NULL) // 不可能！！
			return 0 ;
	}
	
	// 读目录
	pdirent = readdir(pdir);

	if(pdirent == NULL)	// 基本不可能
	{
		closedir(pdir);
		return 0;
	}
	
	// 计算文件个数
	do {
		// 获得文件状态
		ret = stat(pdirent->d_name, &sbuf);
		if(ret!=-1) 
		{
			//是目录？
			if(S_ISDIR(sbuf.st_mode))
				dirnum ++ ;
			// 扩展名是plx?
			else if (strcmp(GetFileExt(pdirent->d_name),DLM_EXT)==0)
				plxnum ++ ;

		}
		// 读下一个目录项	
		pdirent = readdir(pdir);
	} while(pdirent !=NULL );
	
	closedir(pdir);
	
	// 减去'.'当前目录和 '..'上级目录
	dirnum -= 2 ; 

	//返回目录数+文件数
	return dirnum + plxnum ;
}

//////////////////////////////////////////////////////////////////////////////
// Function	ResetMenu
// Purpose 重设菜单
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static void ResetMenu()
{
	int cursel ;

	// 获得当前选中列表项，得到附加数据
	cursel = SendMessage (hFileList, LB_GETCURSEL, 0, 0) ;
	cursel = SendMessage (hFileList, LB_GETITEMDATA, cursel, 0) ;

	// 删除原有菜单
	RemoveMenu(hMenu,ID_DETAIL,MF_BYCOMMAND) ;
	RemoveMenu(hMenu,ID_DELETE,MF_BYCOMMAND) ;
	RemoveMenu(hMenu,(UINT_PTR)hMenuSend,MF_BYCOMMAND) ;

	// plx 文件时才有send菜单
	if(cursel & ITEM_FILE)
		AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuSend, IDS_SEND);

	// 重新加入
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_DETAIL, IDS_DETAIL);
	
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_DELETE, IDS_DELETE);

	// 重置左soft key
	if(WinLevel == 1) // 只有在文件列表界面才做
	{
		if(cursel & ITEM_DIR)
			SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_RUN);
		else if(cursel & ITEM_FILE)
			SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_INSTALL);
	}
	// 设置菜单
	PDASetMenu(hDlmFrameWnd, hMenu);				
}

//////////////////////////////////////////////////////////////////////////////
// Function	RunELF
// Purpose 运行elf文件
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static void RunELF ()
{
	int cursel = LB_ERR ;
	unsigned char fn[_MAX_FNAME] = "";
	unsigned char path[_MAX_FNAME] = "" ;
	cursel = SendMessage (hFileList, LB_GETCURSEL, 0, 0) ;				
	if(cursel!=LB_ERR)
	{
		SendMessage (hFileList, LB_GETTEXT, cursel, (LPARAM) fn) ;
		strcpy(path, CurrentDir) ;
		strcat(path, fn) ;
		strcat(path,"/") ;
		strcat(path,"m") ;
		strcat(path, fn) ;
		strcat(path, ".bmp") ;
//		dlm_LoadModule (path) ;
		if (RunDlmApp (path)!=TRUE)
			PLXTipsWin(hDlmFrameWnd, Dlm_MainWndApp, ID_CANNOTRUN, 
				IDS_CANNOTRUN, NULL, Notify_Failure, IDS_OK, NULL, SECOND(2));			
		
	}
}


//////////////////////////////////////////////////////////////////////////////
// Function	InstallPLX
// Purpose 安装plx文件
// Params	dest = 1 mmc, dest = 0 flash
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static void InstallPLX (int dest)
{
	// 得到plx文件名
	if(!installOnly )
	{
		strcpy(path, CurrentDir) ;
		strcat(path, g_fn) ;
	}
	else
	{
		unsigned char *p = path;
		while (*p++) ;
		while (*p--!='/') ;
		strcpy (g_fn, ++p) ;
	}
	
	// 得到安装目标路径
	if(dest==0)
		strcpy(destpath, DLM_DIR_CARD) ;
	else
		strcpy(destpath, DLM_DIR_MOBILE) ;
	
	// lack of memory
	if (CheckMemory(dest,path) == 0)
	{
		PLXTipsWin(hDlmFrameWnd, Dlm_MainWndApp, ID_LACK_OF_MEMORY, 
			IDS_LACK_OF_MEMORY, NULL, Notify_Failure, IDS_OK, NULL, SECOND(2));
		return ;
	}
	
	bCancel = FALSE ;
	pthread_create( &thread, NULL, InstallThread, (void *)NULL );
	
	WaitWin(Dlm_MainWndApp, TRUE, IDS_INSTALLING, WIN_TITLE,NULL,IDS_CANCEL, ID_INSTALL_CANCEL);
	
	// 恢复显示原来的界面
}

//////////////////////////////////////////////////////////////////////////////
// Function	InstallDestSelect
// Purpose 选择安装位置界面
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static int InstallDestSelect ()
{
	int cursel ;
	if 	(!installOnly)
	{
		cursel = SendMessage (hFileList, LB_GETCURSEL, 0, 0) ;
		if(cursel!=LB_ERR)
		{
			SendMessage (hFileList, LB_GETTEXT, cursel, (LPARAM) g_fn) ;
		}
	}

	if (MMC_CheckCardStatus ()	)
	{
		int ret = 1 ;
//		unsigned long wstyle ;
		// 显示安装目标位置选择界面
			//删除 listbox 内容
		CreateListBox(Dlm_MainWndApp, 0) ;
/*		while (ret!=LB_ERR)
			ret = SendMessage (hFileList,LB_DELETESTRING,0,0) ;
		
		wstyle = GetWindowLong(hFileList,GWL_STYLE) ;
		SetWindowLong(hFileList, GWL_STYLE,wstyle&(~LBS_SORT)) ;
*/
		ret = SendMessage(hFileList,LB_ADDSTRING, 0, (LPARAM)IDS_MMC) ;
		SendMessage(hFileList, LB_SETCURSEL, ret, 0) ;
		SendMessage(hFileList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, ret), (LPARAM)hBMPRadioSelect) ;
		ret = SendMessage(hFileList,LB_ADDSTRING, 1, (LPARAM)IDS_FLASH) ;
		SendMessage(hFileList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, ret), (LPARAM)hBMPRadioUnselect) ;

		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"") ;
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL) ;
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)IDS_SELECT) ;

		SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 0);
		SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);

		SetWindowText(hDlmFrameWnd,IDS_INSTALL) ;
		
		WinLevel = 3 ;
		SetMenuAndSoftKeys() ;
		return 0 ;
	}
	else
		return 1 ;
}

//////////////////////////////////////////////////////////////////////////////
// Function	ShowEmptyFolder
// Purpose mmc/flash中没有文件的时候，显示这个界面
// Params
// Return
// Remarks
//////////////////////////////////////////////////////////////////////////////
static void ShowEmptyFolder (unsigned char *path)
{
//	int ret = 1 ; 
//	unsigned long wstyle ;
	//删除 listbox 内容

//	CreateListBox(Dlm_MainWndApp, 0) ;
/*	
	while (ret!=LB_ERR)
		ret = SendMessage (hFileList,LB_DELETESTRING,0,0) ;

	wstyle = GetWindowLong(hFileList,GWL_STYLE) ;
	SetWindowLong(hFileList, GWL_STYLE,wstyle&(~LBS_SORT)) ;
*/

//	ret = SendMessage (hFileList, LB_ADDSTRING, 0, (LPARAM)IDS_NOFILE) ;
//	SendMessage(hFileList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, ret), (LPARAM)hBmpEmpty) ;
	
//	SendMessage (hFileList,LB_SETCURSEL,0,0) ;	

	HDC theDC = GetDC(Dlm_MainWndApp) ;
	RECT rcClient;
	if(hFileList)
		DestroyWindow(hFileList) ;
	hFileList = NULL ;
	
	SetBkColor(theDC, COLOR_TRANSBK);

	GetClientRect(Dlm_MainWndApp,&rcClient);
	DrawText(theDC,IDS_NOFILE,-1,&rcClient,DT_CLEAR|DT_VCENTER|DT_CENTER);
	ReleaseDC(Dlm_MainWndApp, theDC) ;
	WinLevel = 2 ;
	SetMenuAndSoftKeys();
	return ;
}


#include "plx.h"

#define IBUFSIZE 1024
#define OBUFSIZE 1024

unsigned char ibuf[IBUFSIZE] ;
unsigned char obuf[OBUFSIZE] ;

PLXInstallFile files[64] ;
z_stream zs ;

//////////////////////////////////////////////////////////////////////////////
// Function	ShowEmptyFolder
// Purpose mmc/flash中没有文件的时候，显示这个界面
// Params
// Return
// Remarks
//////////////////////////////////////////////////////////////////////////////
static void* InstallThread (void *para)
{
	int ifi ;
	int of ;
	int totalfiles = 0 ;
	int i ;
	int bytesread ;
	unsigned char fn[_MAX_FNAME] ;
	int writesize = 0 ;
	int err ;
	int bytesleft ;
	unsigned char *srcfn ;
	ProgmanInf prgInfo ;
	DIR *pdir = NULL ;
	short reinstall = -1 ;

	printf ("\r\nInstallThread Entering.\r\n") ;
	bInInstall = TRUE ;
	installState = INSTALL_OK ;
	
	srcfn = path ;
	
	{
		int i=0 ;
		unsigned char *p = destpath ;
		
		while(*p) p++ ;
		
		while(g_fn[i]!='.')
			*p++ = g_fn[i++] ;
		*p++ = '/' ;
		*p++ = 0 ;

		pdir = opendir(destpath) ;
		if(pdir)
		{
			reinstall = 1 ;
			closedir (pdir) ;
		}
		else
			mkdir(destpath, 0) ;
	}
	
	printf ("\r\nInstallThread Make dir OK.\r\n") ;

//	memset (files, 0, sizeof(files[0])*64) ;
	
//	GetPlxFileInfo (srcfn) ;

	ifi = open(srcfn, O_RDONLY) ;

	lseek (ifi, -4, SEEK_END) ;
	read (ifi, &bytesleft, 4) ;

	lseek (ifi, -8, SEEK_END) ;
	read (ifi, &totalfiles, 4) ;

	lseek (ifi, IDSTRING_SIZE, SEEK_SET) ;
	read (ifi, &prgInfo, sizeof(prgInfo)) ;

	i = 0 ;
	strcpy(fn,destpath) ;
	strcat(fn,files[i].filename) ;
	of = open(fn, O_CREAT|O_RDWR,S_IRWXU) ;
	
	printf("dlm open %s as %x\r\n", fn, of) ;
	
	memset (&zs, 0, sizeof(zs)) ;
	
	bytesread = read(ifi, ibuf, bytesleft>IBUFSIZE?IBUFSIZE:bytesleft) ;
	
	bytesleft -= bytesread ;

	zs.next_in = ibuf ;
	zs.next_out = obuf ;
	zs.avail_out = OBUFSIZE ;
	zs.avail_in = bytesread;
	printf ("\r\nInstallThread prepare inflate.\r\n") ;
	inflateInit (&zs) ;
	
	while (!bCancel)
	{
		err = inflate (&zs, Z_NO_FLUSH) ;
		if (err<0)
		{
			installState = INSTALL_FAILED ;
			break ;
		}
		if (zs.avail_out==0)
		{
			int byte_avail ;
			int byte_left ;
			byte_left = byte_avail = OBUFSIZE - zs.avail_out ;
			
			while (byte_left>0)
			{
				//chang '<' to "<="
				if(writesize+byte_left <= (int)files[i].uncompressed_size)
				{
					write(of, obuf + (byte_avail-byte_left), byte_left) ;
					writesize += byte_left ;
					byte_left = 0 ;
				}
				else
				{
					if(files[i].uncompressed_size-writesize>0)
						write(of, obuf+(byte_avail-byte_left), files[i].uncompressed_size-writesize) ;
					close(of) ;
					byte_left -= files[i].uncompressed_size-writesize ;
					writesize = 0 ;
					i ++ ;
					if(i<totalfiles)
					{
						strcpy(fn,destpath) ;
						strcat(fn,files[i].filename) ;
						of = open(fn, O_CREAT|O_RDWR,S_IRWXU) ;
						printf("dlm open %s as %x\r\n", fn, of) ;
					}
				}
			}
			zs.next_out = obuf ;
			zs.avail_out = OBUFSIZE ;
		}
		if (zs.avail_in==0)
		{
			bytesread = read(ifi, ibuf, bytesleft>IBUFSIZE?IBUFSIZE:bytesleft) ;
			zs.avail_in = bytesread ;
			zs.next_in = ibuf ;
		}
		if (err == Z_STREAM_END)
			break ;
	}

	if(!bCancel)
	{
		if (zs.avail_out!=OBUFSIZE)
		{
			int byte_avail ;
			int byte_left ;
			byte_left = byte_avail = OBUFSIZE - zs.avail_out ;
			
			while (byte_left>0)
			{
				if(writesize+byte_left < (int)files[i].uncompressed_size)
				{
					write(of, obuf + (byte_avail-byte_left), byte_left) ;
					writesize += byte_left ;
					byte_left = 0 ;
				}
				else
				{
					write(of, obuf+(byte_avail-byte_left), files[i].uncompressed_size-writesize) ;
					close(of) ;
					byte_left -= files[i].uncompressed_size-writesize ;
					writesize = 0 ;
					i ++ ;
					if(i<totalfiles)
					{
						strcpy(fn,destpath) ;
						strcat(fn,files[i].filename) ;
						of = open(fn, O_CREAT|O_RDWR, S_IRWXU) ;
						printf("dlm open %s as %x\r\n", fn, of) ;
					}
				}
			}
		}
	}
	else
		close(of) ;
	
	inflateEnd(&zs);
	close (ifi) ;
	
	printf("\r\nInstallthread inflat end.\r\n") ;
	
	if(bCancel)
	{
		installState = INSTALL_CANCELED ;
		if(reinstall==1)
		{
			Dlm_MakeIconName(destpath,g_fn,fn,NULL);
			DeleteDlmApp(fn) ;
		}
		
		DLM_DeleteDir(destpath) ;
	}
	else 
	{
		if (installState == INSTALL_OK)
		{
			int i=0 ;
			int ret ;
			struct stat sbuf;
			
			installState = installState ;
			strcpy(fn,destpath) ;
			{
				memset (&app, 0, sizeof(app)) ;
				
				while(g_fn[i]!='.')
				{
					app.achName[i] = g_fn[i] ;
					app.achTitleName[i] = g_fn[i] ;
					i++ ;
				}
				app.nType = 0 ;
				
				Dlm_MakeIconName(destpath, g_fn, fn,NULL) ;
				strcpy(app.strIconName, fn) ;
				
				ret = stat(fn, &sbuf);
				if(ret==-1)
				{
					DLM_CopyFile(DLM_DEFAULT_ICON1,fn) ;
					Dlm_MakeIconName(destpath, g_fn, NULL, fn) ;
					DLM_CopyFile(DLM_DEFAULT_ICON2,fn) ;	
				}
			}
			strcpy(fn,destpath) ;
			strcat(fn,IDS_INF_FILENAME) ;
			
			of = open(fn, O_CREAT|O_RDWR,S_IRWXU) ;
			write(of, &prgInfo, sizeof(prgInfo)) ;
			close(of) ;

			PostMessage(Dlm_MainWndApp, ID_INSTALL_OK, (WPARAM)reinstall, (LPARAM)0) ;
		}
		else
		{
			DLM_DeleteDir(destpath) ;
			PostMessage(Dlm_MainWndApp, ID_INSTALL_OK, (WPARAM)0, (LPARAM)0) ;
		}

	}

/*	{
			DIR *pdir = NULL ;
			struct dirent *pdirent =NULL;
			int dirnum = 0 ;
			int plxnum = 0 ;
			int ret ;
			struct stat sbuf;
			pdir = opendir(destpath) ;
			if(pdir)
			{
				pdirent = readdir(pdir);
				
				do {
					// 获得文件状态
					ret = stat(pdirent->d_name, &sbuf);
					if(ret!=-1) 
					{
						//是目录？
						if(S_ISDIR(sbuf.st_mode))
							dirnum ++ ;
						// 扩展名是plx?
						else if (strcmp(GetFileExt(pdirent->d_name),DLM_EXT)==0)
							plxnum ++ ;
						
					}
					// 读下一个目录项	
					pdirent = readdir(pdir);
				} while(pdirent !=NULL );
				closedir(pdir) ;
			}		
		}
*/
	printf ("\r\nInstallThread Returns.\r\n") ;
	bInInstall = FALSE ;
	pthread_exit (NULL);
	return NULL ;
}
//////////////////////////////////////////////////////////////////////////////
// Function	SetMenuAndSoftKeys
// Purpose 根据当前窗口状态设置菜单，softkey等。
// Params
// Return
// Remarks
//////////////////////////////////////////////////////////////////////////////
static int SetMenuAndSoftKeys ()
{
	switch (WinLevel)
	{
	case 0:			// device list
		// 修改左右菜单
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)IDS_SELECT);
		
		// 窗口为设备列表窗口
		SetWindowText(hDlmFrameWnd,WIN_TITLE) ;
		SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 0);
		SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);

		break ;
	case 1:			// file list
			
		// 设置窗口标题，画左右箭头
		if(strstr(CurrentDir, IDS_DEVICEMC))
		{
			SetWindowText(hDlmFrameWnd,IDS_MMC) ;
			SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 0);
			SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);
			SendMessage(hDlmFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)hBmpLeftArrow);
		}
		else
		{
			SetWindowText(hDlmFrameWnd,IDS_FLASH) ;
			if(MMC_CheckCardStatus())
			{
				SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 0);
				SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);
				SendMessage(hDlmFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)hBmpRightArrow);
			}
		}
		
		// 修改左右菜单
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)IDS_OPTION);  	

		// 设置窗口标题
		ResetMenu () ;

		break ;
	
	case 2:		// empty folder
		// 设置窗口标题，画左右箭头
		if(strstr(CurrentDir, IDS_DEVICEMC))
		{
			SetWindowText(hDlmFrameWnd,IDS_MMC) ;
			SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 0);
			SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);
			SendMessage(hDlmFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)hBmpLeftArrow);
		}
		else
		{
			SetWindowText(hDlmFrameWnd,IDS_FLASH) ;
			if(MMC_CheckCardStatus())
			{
				SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 0);
				SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);
				SendMessage(hDlmFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)hBmpRightArrow);
			}
		}

		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");  	

		break ;
	case 3:
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"") ;
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL) ;
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)IDS_SELECT) ;

		SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 0);
		SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);

		SetWindowText(hDlmFrameWnd,IDS_INSTALL) ;

		break ;

	case 4:
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK) ;
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"") ;
		SendMessage(hDlmFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"") ;

		SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 0);
		SendMessage(hDlmFrameWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);

		SetWindowText(hDlmFrameWnd,IDS_DETAILS) ;
		break ;
	}
	return 0 ;
}

/**********************************************************************
* Function	 CheckMemory
* Purpose    check available memory.
* Params	
* Return	
* Remarks	change later.
**********************************************************************/
static int CheckMemory(int dest, unsigned char* plxpath)
{
	int needsize ;
	int freesize ;
	struct statfs buf ;
	
	needsize = GetPlxFileInfo (plxpath) ;
	if(dest==0)		// 1 : mmc, 0 : flash
		statfs (DLM_DEV_CARD, &buf) ;
	else
		statfs (DLM_DEV_FLASH, &buf) ;

	freesize = buf.f_bfree * buf.f_bsize ;
	
	if(needsize < freesize + 1024*10)
	{
		return 1 ;
	}
	else
	{
		return 0 ;
	}
}

/**********************************************************************
* Function	 DLM_CopyFile
* Purpose    copy file.
* Params	
* Return	
* Remarks
**********************************************************************/
#define BUF_SIZE 1024
BOOL DLM_CopyFile(PCSTR pOldFileName, PCSTR pNewFileName)
{
	int hfold,hfnew;
	char		buf[BUF_SIZE];
	DWORD nBytes, dwPointer = 0;
	
	hfold = open(pOldFileName, O_RDONLY);
	printf("dlm open %s as %x\r\n", pOldFileName, hfold) ;
	if(hfold == -1)
		return 0;

	hfnew = open(pNewFileName, O_RDWR|O_CREAT, S_IRWXU);
	printf("dlm open %s as %x\r\n", pNewFileName, hfnew) ;
	if(hfnew == -1)
	{
		close(hfold);
		return 0;
	}
	nBytes = read(hfold, buf, BUF_SIZE);

	while (nBytes != 0 && nBytes != -1)
	{
		int ret;

		lseek(hfnew, dwPointer, SEEK_SET);
		ret = write(hfnew, buf, nBytes);
		
		if(ret != (int)nBytes)
		{
			close(hfnew);
			close(hfold);
			return 0;
		}
		dwPointer += nBytes; 
		lseek(hfold, dwPointer, SEEK_SET);
		nBytes = read(hfold, buf, BUF_SIZE);
					
	}
	close(hfold);
	close(hfnew);
	return 1;

}

/**********************************************************************
* Function	 ComfirmDelete
* Purpose    show the dilog if user comirm to delete.
* Params	
* Return	
* Remarks
**********************************************************************/
static void ComfirmDelete ()
{
	int cursel = LB_ERR ;
	unsigned char itemname[64] = "<" ;


	if (strcmp(CurrentDir, DLM_DIR_CARD) == 0)
	{
		if(!MMC_CheckCardStatus())
		{
			PLXTipsWin(hDlmFrameWnd, Dlm_MainWndApp, ID_NOCARD, 
				IDS_NOCARD, NULL, Notify_Failure, IDS_OK, NULL, SECOND(2));			
			return ;
		}
	}

	cursel = SendMessage (hFileList, LB_GETCURSEL, 0, 0) ;
	if (cursel==LB_ERR)
		return ;
	
	SendMessage(hFileList, LB_GETTEXT, cursel, (LPARAM)itemname+1) ;

	strcat (itemname, ">:\r\n") ;
	strcat (itemname, IDS_ASKDELETE) ;
	
	PLXConfirmWinEx(hDlmFrameWnd, Dlm_MainWndApp, itemname, Notify_Request, WIN_TITLE, IDS_YES, IDS_NO, ID_DELETE_COFIRM);
}

/**********************************************************************
* Function	 DLM_DeleteFile
* Purpose    delete plx file or installed file.
* Params	
* Return	
* Remarks
**********************************************************************/
static void DLM_DeleteFile()
{
	int cursel = LB_ERR ;
	unsigned char itemname[64] = "" ;
	unsigned long type ;
	unsigned char pathname [_MAX_FNAME] = "" ;
	unsigned char filename[_MAX_FNAME] = "" ;

	WaitWin(Dlm_MainWndApp, TRUE, IDS_DELETING, WIN_TITLE,NULL,NULL, ID_NEEDNOT);	
	
	cursel = SendMessage (hFileList, LB_GETCURSEL, 0, 0) ;
	if (cursel==LB_ERR)
		return ;
	
	SendMessage(hFileList, LB_GETTEXT, cursel, (LPARAM)itemname) ;

	type = SendMessage(hFileList, LB_GETITEMDATA, cursel, (LPARAM)0) ;

	strcpy(pathname, CurrentDir) ;
	strcat(pathname, itemname) ;
	
	if(type&ITEM_DIR)
	{
		strcat(pathname, "/") ;
		Dlm_MakeIconName(pathname,itemname,filename,NULL) ;
		DeleteDlmApp(filename) ;
		DLM_DeleteDir (pathname) ;
	}
	else
	{
		unlink(pathname) ;
	}
	WaitWin(Dlm_MainWndApp, FALSE, IDS_DELETING, WIN_TITLE,NULL,NULL, ID_NEEDNOT);
}

/**********************************************************************
* Function	 DLM_DeleteDir
* Purpose    delete the specified directory
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL DLM_DeleteDir (unsigned char *pPath) 
{
	struct dirent *dirinfo = NULL;
	struct stat	filestat;
	DIR  *dirtemp = NULL;
	int		i=0;
	PSTR	strFullName;

	dirtemp = opendir(pPath);
	if(!dirtemp)
		return FALSE;

	dirinfo = readdir(dirtemp);
	while (dirinfo&&dirinfo->d_name[0]) {
		strFullName = malloc(strlen(pPath)+strlen(dirinfo->d_name)+2);
		strcpy ( strFullName, pPath );
		if (pPath[strlen(pPath)-1] != '/') {
			strcat(strFullName,"/");
		}
		strcat ( strFullName, dirinfo->d_name);
		stat(strFullName,&filestat);
		if ((((filestat.st_mode&S_IFMT)!=S_IFLNK)&&((filestat.st_mode&S_IFMT)==S_IFDIR)&&(strcmp(dirinfo->d_name,".")!=0)&&(strcmp(dirinfo->d_name,"..")!=0))) 
		{
			//unlink(strFullName);
			DLM_DeleteDir(strFullName);
		}
		else if(!S_ISDIR(filestat.st_mode))
		{
			unlink(strFullName);
		}
		free(strFullName);
		strFullName = NULL;
		dirinfo = readdir(dirtemp);
	}
	closedir(dirtemp) ;
	i = rmdir(pPath);
	if (i==0) {
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**********************************************************************
* Function	 ShowDetail
* Purpose    show detail information for the plx or elf files.
* Params	
* Return	
* Remarks
**********************************************************************/
static void ShowDetail ()
{
	int cursel = LB_ERR;
	int type ;
	unsigned char fn[_MAX_FNAME] ;
	unsigned char fullfn[_MAX_FNAME] ;
	int ifi = 0 ;
	ProgmanInf appInfo ;
	int size, ret ;
	unsigned char stemp[32] ;
	struct stat sbuf;	

	printf("\r\nShowDetail\r\n") ;

	if (strcmp(CurrentDir, DLM_DIR_CARD) == 0)
	{
		printf("\r\nShowDetail---------card\r\n") ;
		if(!MMC_CheckCardStatus())
		{	
			printf("\r\nShowDetail---------no card return \r\n") ;
			PLXTipsWin(hDlmFrameWnd, Dlm_MainWndApp, ID_NOCARD, 
				IDS_NOCARD, NULL, Notify_Failure, IDS_OK, NULL, SECOND(2));		
			return ;
		}
	}
	
	cursel = SendMessage (hFileList, LB_GETCURSEL, 0, 0) ;
	
	if (cursel == LB_ERR)
		return ;
	
	type = SendMessage(hFileList, LB_GETITEMDATA, cursel, 0) ;
	cursel = SendMessage (hFileList, LB_GETTEXT, cursel, (LPARAM)fn) ;
	
	if (cursel==LB_ERR)
		return ;
	
	strcpy(fullfn, CurrentDir) ;
	strcat(fullfn, fn) ;

	memset(&appInfo, 0, sizeof(appInfo)) ;

	if(type&ITEM_DIR)		// elf
	{
		strcat(fullfn,"/") ;
		strcat(fullfn,IDS_INF_FILENAME) ;
		strcpy (stemp, "ELF") ;
		ifi = open(fullfn, O_RDONLY) ;
		if (!ifi)
			return ;

		read(ifi, &appInfo, sizeof(appInfo)) ;
		close(ifi) ;

		strcpy(fullfn, CurrentDir) ;
		strcat(fullfn, fn) ;
		strcat(fullfn,"/") ;
		strcat(fullfn,fn) ;
		strcat(fullfn,".elf") ;
		ret = stat(fullfn, &sbuf);
		if(ret!=-1)
		{
			size=sbuf.st_size ;
		}
	}
	else
	{
		unsigned char plxfiletag[IDSTRING_SIZE] ;
		strcpy (stemp, "PLX") ;
		ifi = open(fullfn, O_RDONLY) ;
		if (!ifi)
			return ;

		read(ifi, plxfiletag, IDSTRING_SIZE) ;
		if (strcmp(plxfiletag,ID_STRING))
		{
			close(ifi) ;
			WinLevel = 4 ;
			PLXTipsWin(hDlmFrameWnd,Dlm_MainWndApp,ID_NOT_PLXFILE,IDS_NOT_PLXFILE,NULL,Notify_Failure,IDS_OK,NULL,SECOND(2)) ;
			return ;
		}
		read(ifi, &appInfo, sizeof(appInfo)) ;

		close(ifi) ;

		ret = stat(fullfn, &sbuf);
		if(ret!=-1)
		{
			size=sbuf.st_size ;
		}

	}

	// form view;
	{
		LISTBUF	ListData ;
			
		if(hFormViewer==NULL)
		{
			RECT rClient ;
			unsigned char temp[32] ;
			GetClientRect(hDlmFrameWnd,&rClient);
			PREBROW_InitListBuf(&ListData);
			
			PREBROW_AddData(&ListData,IDS_NAME,appInfo.Name);
			PREBROW_AddData(&ListData,IDS_VERSION,appInfo.Version);
			PREBROW_AddData(&ListData,IDS_COPYRIGHT,appInfo.Copyright);
			PREBROW_AddData(&ListData,IDS_FILETYPE,stemp);
			if(size>1024)
			{
				size /= 1024 ;
				sprintf(temp,"%dkB", size) ;
			}
			else
			{
				size /= 100 ;
				if(size==0)
					size = 1 ;
				sprintf(temp,"0.%dkB", size) ;
			}

			

			PREBROW_AddData(&ListData,"Size:",temp);
			if(strcmp(CurrentDir, DLM_DIR_CARD))
			{
				strcpy(stemp, IDS_FLASHMEM) ;
			}
			else
			{
				strcpy(stemp, IDS_MMCCARD) ;
			}
			PREBROW_AddData(&ListData,IDS_LOCATION,stemp);


			hFormViewer = CreateWindow(FORMVIEWER,NULL,WS_VISIBLE|WS_CHILD| WS_VSCROLL,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				Dlm_MainWndApp, NULL,NULL,(PVOID)&ListData);

			WinLevel = 4 ;
			SetMenuAndSoftKeys () ;
			SetFocus(hFormViewer) ;
		}
	}
}

/**********************************************************************
* Function	 CreateListBox
* Purpose    recreate the listbox control
* Params	
* Return	
* Remarks	 'coz lbs_sort can't be changed by setwindowlong
**********************************************************************/
void CreateListBox(HWND hWnd, unsigned lbs_sort)
{
	RECT rccl;
	//创建列表
	if(hFileList)
		DestroyWindow(hFileList) ;

	GetClientRect(hWnd, &rccl);
	hFileList = CreateWindow("LISTBOX",
		"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | lbs_sort | LBS_MULTILINE | LBS_BITMAP | LBS_NOTIFY ,
		//0, 0,
		rccl.left, rccl.top,
		rccl.right, rccl.bottom,
		hWnd,
		(HMENU)ID_LISTBOX,
		NULL,
		NULL);
	
	// 设置焦点
	SetFocus(hFileList) ;
}

/**********************************************************************
* Function	 Dlm_MakeIconName
* Purpose    create the iconname
* Params	
* Return	
* Remarks	 iconname & iconname1 contains the output and  (if not null)
**********************************************************************/
void Dlm_MakeIconName(unsigned char *destpath, unsigned char *g_fn, unsigned char *IconName, unsigned char *IconName1) 
{
	int i = 0 ;

	if (IconName!=NULL) 
	{
		strcpy(IconName,destpath) ;

		while(*IconName) IconName++ ;
		*IconName++ = 'm' ;
		*IconName = 0 ;
		
		i = 0 ;
		while(g_fn[i]!='.' && g_fn[i]!=0)
			*IconName++ = g_fn[i++] ;
		*IconName++ = '.' ;
		*IconName++ = 'b' ;
		*IconName++ = 'm' ;
		*IconName++ = 'p' ;
		*IconName++ = 0 ;
	}
	if(IconName1!=NULL)
	{
		strcpy(IconName1,destpath) ;
		
		i = 0 ;
		
		while(*IconName1) IconName1++ ;
		*IconName1++ = 'm' ;
		*IconName1++ = 'g' ;
		*IconName1 = 0 ;
		
		i = 0 ;
		while(g_fn[i]!='.' && g_fn[i]!=0)
			*IconName1++ = g_fn[i++] ;
		*IconName1++ = '.' ;
		*IconName1++ = 'b' ;
		*IconName1++ = 'm' ;
		*IconName1++ = 'p' ;
		*IconName1++ = 0 ;
	}
}

/**********************************************************************
* Function	 GetPlxFileInfo
* Purpose    GetPlxFileInfo
* Params	
* Return	
* Remarks	 
**********************************************************************/
static int GetPlxFileInfo (unsigned char *plxFile)
{
	int ifi ;
	int totalfiles = 0 ;
	int i ;
	int bytesleft ;
	
	int needsize = 0 ;

	memset (files, 0, sizeof(files[0])*64) ;
	
	ifi = open(plxFile, O_RDONLY) ;

	lseek (ifi, -4, SEEK_END) ;
	read (ifi, &bytesleft, 4) ;

	lseek (ifi, -8, SEEK_END) ;
	read (ifi, &totalfiles, 4) ;
	if (totalfiles <= 64)
	{
		lseek(ifi, (unsigned long)(0-(8+sizeof(files[0])*totalfiles)),SEEK_END) ;
		for (i=0; i<totalfiles; i++)
		{
			read(ifi, files[i].filename, sizeof(files[i].filename)) ;
			read(ifi, &files[i].start_offset, sizeof(files[i].start_offset)) ;
			read(ifi, &files[i].compressed_size, sizeof(files[i].compressed_size)) ;
			read(ifi, &files[i].uncompressed_size, sizeof(files[i].uncompressed_size)) ;
			read(ifi, &files[i].trunk_size, sizeof(files[i].trunk_size)) ;

			needsize += files[i].uncompressed_size ;
		}
	}
	close (ifi) ;

	printf ("\r\nInstallThread read info OK.\r\n") ;

	return needsize ;
}

/**********************************************************************
* Function	 DLMGetOccupiedSize
* Purpose    DLMGetOccupiedSize
* Params	 type 1 mmc, 0 flash
* Return	
* Remarks	 
**********************************************************************/
int DLMGetOccupiedSize (int type)
{
	if(type==0)		//flash
		return DLM_GetDirSize(DLM_DIR_MOBILE) ;
	else
		return DLM_GetDirSize(DLM_DIR_CARD) ;
}


/**********************************************************************
* Function	 DLM_GetDirSize
* Purpose    get size of the specified directory
* Params	
* Return	
* Remarks
**********************************************************************/
static int DLM_GetDirSize (unsigned char *pPath) 
{
	struct dirent *dirinfo = NULL;
	struct stat	filestat;
	DIR  *dirtemp = NULL;
	int		i=0;
	PSTR	strFullName;
	int size = 0 ;

	dirtemp = opendir(pPath);
	if(!dirtemp)
		return FALSE;

	dirinfo = readdir(dirtemp);
	while (dirinfo&&dirinfo->d_name[0]) {
		strFullName = malloc(strlen(pPath)+strlen(dirinfo->d_name)+2);
		strcpy ( strFullName, pPath );
		if (pPath[strlen(pPath)-1] != '/') {
			strcat(strFullName,"/");
		}
		strcat ( strFullName, dirinfo->d_name);
		stat(strFullName,&filestat);
		if ((((filestat.st_mode&S_IFMT)!=S_IFLNK)&&((filestat.st_mode&S_IFMT)==S_IFDIR)&&(strcmp(dirinfo->d_name,".")!=0)&&(strcmp(dirinfo->d_name,"..")!=0))) 
		{
			size += DLM_GetDirSize(strFullName);
		}
		else if(!S_ISDIR(filestat.st_mode))
		{
			size += filestat.st_size ;
		}
		free(strFullName);
		strFullName = NULL;
		dirinfo = readdir(dirtemp);
	}
	closedir(dirtemp) ;
	return size ;	
}


// file over.


