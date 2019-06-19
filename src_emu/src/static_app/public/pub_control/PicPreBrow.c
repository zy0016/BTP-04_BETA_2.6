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

#include	"PreBrow.h"
#include	"hpimage.h"
#include	"PreBrowHead.h"
#include	"mullang.h"
#include "pubapp.h"
#include "time.h"

#define		FOLDERPATH	"/rom/progman/app/mdefaultfolder_43x28.bmp"
#define PICMMCICON "/rom/pim/na_43x28.bmp"
/************************************************************************/
/* 消息参数宏                                                           */
/************************************************************************/

#define		IDC_PREPIC_LIST		20
#define		IDM_PREPIC_BACK		2001
#define		IDM_PREPIC_GET		2005
#define		IDM_PREPIC_FULLPRE	2006

#define		IDC_PREPIC_GRPLIST	2007


/************************************************************************/
/* 控件位置宏                                                           */
/************************************************************************/

#define		PREPIC_LIST_X			PREBROW_WND_X
#define		PREPIC_LIST_Y			PREBROW_WND_Y
#define		PREPIC_LIST_WIDTH		PREBROW_WND_WIDTH
#define		PREPIC_LIST_HEIGHT		(PREBROW_WND_HEIGHT * 2 ) / 6//5

#define		PREPIC_PIC_X			PREBROW_WND_X
#define		PREPIC_PIC_Y			PREBROW_WND_Y + PREPIC_LIST_HEIGHT + 1
#define		PREPIC_PIC_WIDTH		PREBROW_WND_WIDTH
#define		PREPIC_PIC_HEIGHT		(PREBROW_WND_HEIGHT * 4 ) / 6

#define		PREPIC_GRPNAMELIST_WIDTH	150//( PREBROW_WND_WIDTH / 3 )
#define		PREPIC_GRPNAMELIST_HEIGHT	( PREBROW_WND_HEIGHT * 3) /5


/*************************************************
	字符串定义宏
*************************************************/
#define		IDS_PREPIC_CAPTION	ML("Select picture") //GetString(STR_PREPIC_CAPTION)
#define		IDS_PREPIC_BACK		ML("Cancel")//GetString(STR_WINDOW_BACK)
#define		IDS_PREPIC_GET		ML("Insert")//GetString(STR_PREPIC_GET)
#define		IDS_PREPIC_OPEN		ML("Open")


/*************************************************
	全局变量宏
*************************************************/
//#define		DEFAULT_PICFILEDIR	"/mnt/flash/picture"
#define		PHONEPICFILEDIR		"/mnt/flash/pictures"

#ifdef _EMULATE_
#define		MMC_CheckCardStatus()	1
#define		MMCPICFOLDERPATH	"/mnt/flash/mmc/pictures"
#else
extern BOOL		MMC_CheckCardStatus();
#define		MMCPICFOLDERPATH	"/mnt/fat16/pictures"
#endif

#define IDS_SUBFOLDER "Folder: "
#define IDS_PICNUM "Pictures: "
//#define		PREBROW_MAX_GRPNAMELEN	50
//#define		PREPIC_MAXGRP			4

#define		IMAGE_LOADBMP			(WM_USER+152)

#define		DEFAULTPICTURE	"/rom/progman/desk1.bmp"
/*************************************************
	全局变量
*************************************************/

typedef struct tagPREVIEWPIC {
	HWND	hRecMsg;	
	HWND	hWndListBox;
	HWND	hFrame;
	UINT	uPicReturn;	
	LISTBUF	ListBuffer;
	BOOL	bFolderOpen;
	HBITMAP	hFolderBmp;
	HBITMAP	hSDCBmp;
	PSTR	pCaption;
	char	szCurCaption[51];
	char	pFirstItem[51];
}VIEWPICTURE, *PVIEWPICTURE;

typedef struct tagPICLISTITEMDATA {
	PSTR	szFileName;
	PSTR	szFullPath;
	float	fImageSize;
	HBITMAP	hMiniature;
}PICITEMDATA, *PPICITEMDATA;
/*************************************************
	函数声明
*************************************************/
static	BOOL RegisterPreviewClass();
static LRESULT CALLBACK PictureBrowseWndProc (HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);	
int	PreBrowAddPicturesToList(HWND hList , PCSTR pPath, PLISTBUF pLtB, HBITMAP hfolder);
int	PreBrowAddFileToList(HWND hList, PCSTR pFilePath, LISTBUF* ListBuffer, HBITMAP hFolderIcon);
extern	int	GetSubFolderNum(PCSTR szFolderPath);
extern	int		GetFileNumInDir(PCSTR pPath);
extern	float	GetFileSizeByte(PCSTR pFileName);
extern	BOOL	Image_LoadMiniature(int	nTopindex, int nPos, HWND hPicList, PLISTBUF pbuffer);
/*static BOOL	PictureBrowPaint(HDC hdc, const char *cFileName, int nFileType,
							 int nHdcShowX, int nHdcShowY, int nHdcShowWidth, int nHdcShowHeight);

*/
//static	HBITMAP	hFolderBmp = NULL;
extern	long	lmxDifftime(unsigned long tm1, unsigned long tm2);
extern	BOOL	loadBitmapForList(HWND hPicList, int nPos, PLISTBUF pbuffer);
extern	BOOL	IsPicture(PSTR	pFileName, int *nItemData);
/*********************************************************************
* Function	   EnterTextsBrow
* Purpose      进入图片预浏览界面
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	BOOL RegisterPreviewClass()
{
	WNDCLASS wc;	
    
	
	wc.style            = CS_OWNDC;
	wc.lpfnWndProc      = PictureBrowseWndProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = sizeof(VIEWPICTURE);
	wc.hInstance        = NULL;
	wc.hIcon            = NULL;
	wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground    = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = "PicBrowseWndClass";
	
	if (!RegisterClass(&wc))
		return FALSE;
	
	return TRUE;
	
	//bPicBrowseWndClass = TRUE;
	
}
BOOL	APP_PreviewPictureEx(HWND hFrame, HWND hWnd, UINT returnmsg, PCSTR szCapion, PCSTR pFirstItem, PCSTR pFocusPic)
{
	
	RECT	rc;
	VIEWPICTURE	ViewPic;
	HWND	hmainWnd;
	int		nIndex;

	memset(&ViewPic, 0, sizeof(VIEWPICTURE));
	ViewPic.hFrame = hFrame;
	ViewPic.uPicReturn = returnmsg;
	ViewPic.hRecMsg = hWnd;
	ViewPic.bFolderOpen = FALSE;
	GetClientRect(hFrame, &rc);
	//ViewPic.hFolderBmp = LoadImage(NULL, "/rom/progman/app/mdefaultfolder_43x28.bmp", IMAGE_BITMAP, 43, 28, LR_LOADFROMFILE);
	if (szCapion&&(strlen(szCapion) > 0)) 
	{
		ViewPic.pCaption = malloc(strlen(szCapion) + 1);
		if (ViewPic.pCaption)
		{
			strcpy(ViewPic.pCaption, szCapion);
		}
		
		strncpy(ViewPic.szCurCaption, szCapion, 50);
	}
	if (pFirstItem)
	{
		strncpy(ViewPic.pFirstItem, pFirstItem, 50);
	}
	

	RegisterPreviewClass();
	hmainWnd = CreateWindow(
		"PicBrowseWndClass", 
		szCapion, 
		WS_VISIBLE|WS_CHILD,
		rc.left,
		rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		hFrame, 
		NULL,
		NULL, 
		(PVOID)&ViewPic
		);
	
	if (!IsWindow(hmainWnd))
	{
		UnregisterClass("PicBrowseWndClass",NULL);
		return FALSE;
	}
	nIndex = PREBROW_FindData(&ViewPic.ListBuffer, (PSTR)pFocusPic);
	if (nIndex != -1)
	{
		nIndex = SendMessage(ViewPic.hWndListBox, LB_SETCURSEL, nIndex, 0);
	}	
	ShowWindow(hFrame,SW_SHOW);
	UpdateWindow(hFrame);
	SetFocus(hmainWnd);
	
	
    
	return TRUE;
}

BOOL	APP_PreviewPicture(HWND hFrame, HWND hWnd, UINT returnmsg, PCSTR szCapion, PCSTR pFirstItem)
{
	
	RECT	rc;
	VIEWPICTURE	ViewPic;
	HWND	hmainWnd;

	memset(&ViewPic, 0, sizeof(VIEWPICTURE));
	ViewPic.hFrame = hFrame;
	ViewPic.uPicReturn = returnmsg;
	ViewPic.hRecMsg = hWnd;
	ViewPic.bFolderOpen = FALSE;
	GetClientRect(hFrame, &rc);
	//ViewPic.hFolderBmp = LoadImage(NULL, "/rom/progman/app/mdefaultfolder_43x28.bmp", IMAGE_BITMAP, 43, 28, LR_LOADFROMFILE);
	if (szCapion&&(strlen(szCapion) > 0)) 
	{
		ViewPic.pCaption = malloc(strlen(szCapion) + 1);
		if (ViewPic.pCaption)
		{
			strcpy(ViewPic.pCaption, szCapion);
		}
		
		strncpy(ViewPic.szCurCaption, szCapion, 50);
	}
	if (pFirstItem)
	{
		strncpy(ViewPic.pFirstItem, pFirstItem, 50);
	}
	

	RegisterPreviewClass();
	hmainWnd = CreateWindow(
		"PicBrowseWndClass", 
		szCapion, 
		WS_VISIBLE|WS_CHILD,
		rc.left,
		rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		hFrame, 
		NULL,
		NULL, 
		(PVOID)&ViewPic
		);
	
	if (!IsWindow(hmainWnd))
	{
		UnregisterClass("PicBrowseWndClass",NULL);
		return FALSE;
	}
	
	
	ShowWindow(hFrame,SW_SHOW);
	UpdateWindow(hFrame);
	SetFocus(hmainWnd);
	
	
    
	return TRUE;
}

/*********************************************************************
* Function	   PictureBrowseWndProc
* Purpose      浏览图片窗口过程
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK PictureBrowseWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
	LRESULT lResult;
//	HDC		hdc;
	char	szTemp[PREBROW_MAXFILENAMELEN] = "";
	char	cFileName[PREBROW_MAXFULLNAMELEN] = "";
	int		nFileType;
	RECT	rClient;
	DIR		*dir;
	char	szMMCName[41] = "";
//	HDC		hdc;
	PCREATESTRUCT	pCreatstruct;
	PVIEWPICTURE	pViewPic;
	char	Textbuf[PREBROW_MAXFULLNAMELEN] ="";
	int		nIndex;
	int		nFlags, nFolder;
	int		num;
	DWORD	nItemType;


	lResult = TRUE;
	pViewPic = GetUserData(hWnd);
	switch(wMsgCmd)
	{
	case WM_CREATE:
		{
			
			//char	Textbuf[PREBROW_MAXFULLNAMELEN] ="";
			
			
			pCreatstruct = (PCREATESTRUCT)lParam;
			memcpy(pViewPic, (PVIEWPICTURE)pCreatstruct->lpCreateParams, sizeof(VIEWPICTURE));
			
			GetClientRect(hWnd, &rClient);
			
			pViewPic->hWndListBox = CreateWindow("PICLISTEX","",
				WS_CHILD |WS_VISIBLE|LBS_HASSTRINGS|WS_VSCROLL|LBS_BITMAP|LBS_MULTILINE|LBS_ICON,
				rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom- rClient.top,
				hWnd,(HMENU)IDC_PREPIC_LIST,NULL,NULL);
			
			
			PREBROW_InitListBuf (&pViewPic->ListBuffer);
			pViewPic->hFolderBmp = LoadImage(NULL, FOLDERPATH, IMAGE_BITMAP, 43, 28, LR_LOADFROMFILE);
			pViewPic->hSDCBmp = LoadImage(NULL, PICMMCICON, IMAGE_BITMAP, 43, 28, LR_LOADFROMFILE);
			//WaitWin(hWnd, TRUE, ML("Opening"), NULL, NULL, NULL, 0);
			//num = AddPicturesToList(pViewPic->hWndListBox,PHONEPICFILEDIR);
			num = PreBrowAddFileToList(pViewPic->hWndListBox,PHONEPICFILEDIR, &pViewPic->ListBuffer, pViewPic->hFolderBmp);
			
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
					nIndex = SendMessage(pViewPic->hWndListBox, LB_ADDSTRING, 0, (LPARAM)szMMCName);
					SendMessage(pViewPic->hWndListBox, LB_SETITEMDATA, nIndex, PREBROW_FILETYPE_FOLDER);
					SendMessage(pViewPic->hWndListBox, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)Textbuf);

					PREBROW_AddPicData(&pViewPic->ListBuffer, szMMCName, MMCPICFOLDERPATH, NULL, 0);
					SendMessage(pViewPic->hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(nIndex,0)), (LPARAM)pViewPic->hSDCBmp);
					//num += PreBrowAddFileToList(hWndListBox, MMCPICFOLDERPATH, &ListBuffer, hFolderBmp);
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
					nFlags = GetFileNumInDir(MMCPICFOLDERPATH);
					nFolder	=	GetSubFolderNum(MMCPICFOLDERPATH);
					sprintf(Textbuf, "%s%d %s%d", IDS_SUBFOLDER, nFolder, IDS_PICNUM, nFlags);
					nIndex = SendMessage(pViewPic->hWndListBox, LB_ADDSTRING, 0, (LPARAM)szMMCName);
					SendMessage(pViewPic->hWndListBox, LB_SETITEMDATA, nIndex, PREBROW_FILETYPE_FOLDER);
					SendMessage(pViewPic->hWndListBox, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)Textbuf);
					
					PREBROW_AddPicData(&pViewPic->ListBuffer, szMMCName, MMCPICFOLDERPATH, NULL, 0);
					SendMessage(pViewPic->hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(nIndex,0)), (LPARAM)pViewPic->hSDCBmp);
				}				
			}
			
			if (pViewPic->pFirstItem[0] != 0) 
			{
				if (strcmp(pViewPic->pFirstItem, "Default") == 0)
				{
					nIndex = SendMessage(pViewPic->hWndListBox, LB_INSERTSTRING, 0, (LPARAM)pViewPic->pFirstItem);
					floattoa(GetFileSizeByte(DEFAULTPICTURE)/1024, Textbuf);
					strcat(Textbuf, " kB");
					SendMessage(pViewPic->hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(nIndex,0)), (LPARAM)pViewPic->hSDCBmp);
					SendMessage(pViewPic->hWndListBox, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)Textbuf);
					PREBROW_InsertPicData(&pViewPic->ListBuffer, 0, pViewPic->pFirstItem, DEFAULTPICTURE, NULL, 0);	
				}
				else
				{
					nIndex = SendMessage(pViewPic->hWndListBox, LB_INSERTSTRING, 0, (LPARAM)pViewPic->pFirstItem);
					SendMessage(pViewPic->hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(nIndex,0)), (LPARAM)pViewPic->hSDCBmp);
					PREBROW_InsertPicData(&pViewPic->ListBuffer, 0, pViewPic->pFirstItem, "", NULL, 0);				
				}
			}
			SendMessage(pViewPic->hWndListBox, LB_SETCURSEL, 0, 0);	
			SendMessage(pViewPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 1), NULL);
			SendMessage(pViewPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 0), NULL);

			nIndex = SendMessage(pViewPic->hWndListBox, LB_GETCURSEL, 0, 0);
			nItemType = SendMessage(pViewPic->hWndListBox, LB_GETITEMDATA, nIndex, 0);
			SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_PREPIC_BACK);
			if (nItemType == PREBROW_FILETYPE_FOLDER)
			{
				SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PREPIC_OPEN);
			}
			else
			{
				SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			}
			if ((pViewPic->pCaption)&&(strlen(pViewPic->pCaption) > 0))
			{
				SetWindowText(pViewPic->hFrame, pViewPic->pCaption);
			}
			else
				SetWindowText(pViewPic->hFrame, IDS_PREPIC_CAPTION);
			//WaitWin(hWnd, FALSE, ML("Opening"), NULL, NULL, NULL, 0);
		}
		break;

	case PWM_SHOWWINDOW:
		SendMessage(pViewPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 1), NULL);
		SendMessage(pViewPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 0), NULL);
		nIndex = SendMessage(pViewPic->hWndListBox, LB_GETCURSEL, 0, 0);
		nItemType = SendMessage(pViewPic->hWndListBox, LB_GETITEMDATA, nIndex, 0);
		if (nItemType == PREBROW_FILETYPE_FOLDER)
		{
			SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PREPIC_OPEN);
		}
		else
		{
			SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		}
		//SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_PREPIC_BACK);
		if (pViewPic->bFolderOpen)
		{
			SetWindowText(pViewPic->hFrame, pViewPic->szCurCaption);
			/*
			if (pViewPic->pCaption[0] != 0) {
				SetWindowText(pViewPic->hFrame, pViewPic->pCaption);
			}
			else
				SetWindowText(pViewPic->hFrame, IDS_PREPIC_CAPTION);
				*/
		}
		else
		{
			if (pViewPic->pCaption&&(strlen(pViewPic->pCaption) > 0)) 
			{
				SetWindowText(pViewPic->hFrame, pViewPic->pCaption);
			}
			else
				SetWindowText(pViewPic->hFrame, IDS_PREPIC_CAPTION);
		}
		loadBitmapForList(pViewPic->hWndListBox, 0, &pViewPic->ListBuffer);
		SetFocus(pViewPic->hWndListBox);
		break;

	case WM_ACTIVATE:
		if (WA_ACTIVE == wParam)
		{
			SetFocus(pViewPic->hWndListBox);
		}
		break;

	case WM_SETFOCUS:
		if(SendMessage(pViewPic->hWndListBox, LB_GETCOUNT, 0, 0)>0)
			SetFocus(pViewPic->hWndListBox);
		//SetFocus(pViewPic->hWndListBox);
		break;

	case IMAGE_LOADBMP:
		Image_LoadMiniature(LOWORD(wParam), HIWORD(wParam), pViewPic->hWndListBox, &pViewPic->ListBuffer);
		break;
		
	case WM_COMMAND:
		if (HIWORD(wParam) == LBN_SELCHANGE)
		{
			loadBitmapForList(pViewPic->hWndListBox, 0, &pViewPic->ListBuffer);
			nIndex = SendMessage(pViewPic->hWndListBox, LB_GETCURSEL, 0, 0);
			nItemType = SendMessage(pViewPic->hWndListBox, LB_GETITEMDATA, nIndex, 0);
			if (nItemType == PREBROW_FILETYPE_FOLDER)
			{
				SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PREPIC_OPEN);
			}
			else
			{
				SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			}
		}
		else
		{
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		}
		break;

	case WM_PAINT:
		{
			HDC	hdc;
			int	oldbm;
			RECT	rc;
			if (SendMessage(pViewPic->hWndListBox, LB_GETCOUNT, 0, 0) > 0) {
				ShowWindow(pViewPic->hWndListBox, SW_SHOW);
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			}
			else
			{
				ShowWindow(pViewPic->hWndListBox, SW_HIDE);
				GetClientRect(hWnd,&rc);
				hdc = BeginPaint(hWnd,NULL);
				//DrawImageFromFile(hdc,"/rom/progman/grpbg.bmp",0,0,ROP_SRC);
				oldbm = SetBkMode(hdc,TRANSPARENT);
				DrawText(hdc, ML("No pictures"), -1, &rc, DT_CENTER|DT_VCENTER);
				SetBkMode(hdc,oldbm);
				EndPaint(hWnd,NULL);
			}
			
		}
		/*
		if (SendMessage(pViewPic->hWndListBox, LB_GETCOUNT, 0, 0) <=0 ) {
			ShowWindow(pViewPic->hWndListBox, SW_HIDE);
			hdc = BeginPaint(hWnd,NULL);
			GetClientRect(hWnd, &rClient);
			DrawText(hdc, ML("No pictures"), -1, &rClient, DT_VCENTER|DT_HCENTER);
			
			EndPaint(hWnd,NULL);
		}
		else
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			*/
		break;

	case WM_KEYDOWN:

	    switch(wParam)
		{
		case VK_RETURN:
			GetFileNameFromList(pViewPic->hWndListBox, &pViewPic->ListBuffer, NULL, szTemp, cFileName, &nFileType);
			//lack judgement about whether the file is directory.
			
			if(nFileType == PREBROW_FILETYPE_FOLDER)
			{
				PreBrowAddPicturesToList(pViewPic->hWndListBox,cFileName,&pViewPic->ListBuffer, pViewPic->hFolderBmp);
				pViewPic->bFolderOpen = TRUE;
				SetWindowText(pViewPic->hFrame, szTemp);
				strncpy(pViewPic->szCurCaption, szTemp, strlen(szTemp));

				nIndex = SendMessage(pViewPic->hWndListBox, LB_GETCURSEL, 0, 0);
				nItemType = SendMessage(pViewPic->hWndListBox, LB_GETITEMDATA, nIndex, 0);
				if (nItemType == PREBROW_FILETYPE_FOLDER)
				{
					SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
					SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PREPIC_OPEN);
				}
				else
				{
					SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
					SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				}
				InvalidateRect(hWnd, NULL, TRUE);				
			}
			break;
			
		case VK_F5:
//			if((pViewPic->pFirstItem[0] != 0)&&(!pViewPic->bFolderOpen))
//			{
//				if (SendMessage(pViewPic->hWndListBox, LB_GETCURSEL, 0, 0) == 0) 
//				{
//					SendMessage(pViewPic->hRecMsg,pViewPic->uPicReturn, 0, 0);
//					SendMessage(pViewPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
//					PostMessage(hWnd, WM_CLOSE, 0, 0);
//					break;
//				}
//			}
			GetFileNameFromList(pViewPic->hWndListBox, &pViewPic->ListBuffer, NULL, szTemp, cFileName, &nFileType);
			//lack judgement about whether the file is directory.
			
			if(nFileType == PREBROW_FILETYPE_FOLDER)
			{
//				AddPicturesToList(pViewPic->hWndListBox,cFileName,&pViewPic->ListBuffer, pViewPic->hFolderBmp);
//				pViewPic->bFolderOpen = TRUE;
//				SetWindowText(pViewPic->hFrame, szTemp);
//				InvalidateRect(hWnd, NULL, TRUE);
				break;
			}
			
			if (strcmp(cFileName, DEFAULTPICTURE) == 0)
			{
				SendMessage(pViewPic->hRecMsg,pViewPic->uPicReturn, 0, (LPARAM)cFileName);
			}
			else
			{
				SendMessage(pViewPic->hRecMsg,pViewPic->uPicReturn,MAKEWPARAM(strlen(cFileName), RTN_IMAGE), (LPARAM)cFileName);
			}
			//SendMessage(pViewPic->hRecMsg,pViewPic->uPicReturn,MAKEWPARAM(strlen(cFileName), RTN_IMAGE), (LPARAM)cFileName);
			SendMessage(pViewPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case VK_F10:
			if (pViewPic->bFolderOpen == TRUE) 
			{
				pViewPic->bFolderOpen = FALSE;
				
				PREBROW_FreeListBuf(&pViewPic->ListBuffer);
				SendMessage(pViewPic->hWndListBox, LB_RESETCONTENT, 0, 0);
				
				//SendMessage(pViewPic->hWndListBox, LB_ADDSTRING, 0, (LPARAM)"Default");
				//PREBROW_AddPicData(&pViewPic->ListBuffer, "Default", "", NULL, 0);
				num = PreBrowAddFileToList(pViewPic->hWndListBox,PHONEPICFILEDIR, &pViewPic->ListBuffer, pViewPic->hFolderBmp);
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
						//nIndex = SendMessage(pViewPic->hWndListBox, LB_ADDSTRING, 0, (LPARAM)szMMCName);
						
						nIndex = SendMessage(pViewPic->hWndListBox, LB_ADDSTRING, 0, (LPARAM)szMMCName);
						SendMessage(pViewPic->hWndListBox, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)Textbuf);

						SendMessage(pViewPic->hWndListBox, LB_SETITEMDATA, nIndex, PREBROW_FILETYPE_FOLDER);
						PREBROW_AddPicData(&pViewPic->ListBuffer, szMMCName, MMCPICFOLDERPATH, NULL, 0);
						SendMessage(pViewPic->hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(nIndex,0)), (LPARAM)pViewPic->hSDCBmp);
						//num += PreBrowAddFileToList(hWndListBox, MMCPICFOLDERPATH, &ListBuffer, hFolderBmp);
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
						nFlags = GetFileNumInDir(MMCPICFOLDERPATH);
						nFolder	=	GetSubFolderNum(MMCPICFOLDERPATH);
						sprintf(Textbuf, "%s%d %s%d", IDS_SUBFOLDER, nFolder, IDS_PICNUM, nFlags);

						nIndex = SendMessage(pViewPic->hWndListBox, LB_ADDSTRING, 0, (LPARAM)szMMCName);
						SendMessage(pViewPic->hWndListBox, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)Textbuf);
						SendMessage(pViewPic->hWndListBox, LB_SETITEMDATA, nIndex, PREBROW_FILETYPE_FOLDER);
						PREBROW_AddPicData(&pViewPic->ListBuffer, szMMCName, MMCPICFOLDERPATH, NULL, 0);
						SendMessage(pViewPic->hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(nIndex,0)), (LPARAM)pViewPic->hSDCBmp);
					}				
				}
				
				if (pViewPic->pFirstItem[0] != 0) 
				{
					nIndex = SendMessage(pViewPic->hWndListBox, LB_INSERTSTRING, 0, (LPARAM)pViewPic->pFirstItem);
					if (strcmp(pViewPic->pFirstItem, "Default") == 0)
					{						
						floattoa(GetFileSizeByte(DEFAULTPICTURE)/1024, Textbuf);
						strcat(Textbuf, " kB");	
						SendMessage(pViewPic->hWndListBox, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)Textbuf);
					}
					
					SendMessage(pViewPic->hWndListBox, LB_SETIMAGE, MAKEWPARAM(IMAGE_ICON, MAKEWORD(nIndex,0)), (LPARAM)pViewPic->hSDCBmp);
					PREBROW_InsertPicData(&pViewPic->ListBuffer, 0, pViewPic->pFirstItem, "", NULL, 0);				
				}
				if ((pViewPic->pCaption)&&(strlen(pViewPic->pCaption) > 0))
				{
					SetWindowText(pViewPic->hFrame, pViewPic->pCaption);
				}
				else
					SetWindowText(pViewPic->hFrame, IDS_PREPIC_CAPTION);
				
				SetFocus(pViewPic->hWndListBox);
				SendMessage(pViewPic->hWndListBox, LB_SETCURSEL, 0, 0);	
				nIndex = SendMessage(pViewPic->hWndListBox, LB_GETCURSEL, 0, 0);
				
				nItemType = SendMessage(pViewPic->hWndListBox, LB_GETITEMDATA, nIndex, 0);
				if (nItemType == PREBROW_FILETYPE_FOLDER)
				{
					SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
					SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PREPIC_OPEN);
				}
				else
				{
					SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
					SendMessage(pViewPic->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				}
				InvalidateRect(hWnd, NULL, TRUE);
				break;
				
			}
			PostMessage(pViewPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
			
		}
		break;

	case WM_CLOSE:
		DestroyWindow (hWnd);
		break;


	case WM_DESTROY:
		if (pViewPic->hFolderBmp)
		{
			DeleteObject(pViewPic->hFolderBmp);
		}
		if (pViewPic->hSDCBmp)
		{
			DeleteObject(pViewPic->hSDCBmp);
		}
		if (pViewPic->pCaption)
		{
			free(pViewPic->pCaption);
		}
		PREBROW_FreeListBuf (&pViewPic->ListBuffer);
		UnregisterClass ("PicBrowseWndClass", NULL);		
		
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;
}

int	PreBrowAddFileToList(HWND hList, const char* pFilePath, LISTBUF* ListBuffer, HBITMAP	hFolderIcon)
{

	struct dirent *dirinfo = NULL;
	struct stat	filestat;

	struct tm *StTime;
	
	int	nRtnNum;	
	char path[PREBROW_MAXPATHLEN];
	char PicSize[18];
	float		tmpsize;
	DIR  *dirtemp = NULL;

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

		PLISTDATA	pFirstNode = NULL, pLastNode = NULL;
		
		memset(FileName, 0, PREBROW_MAXFILENAMELEN + 6);
		memset(FullName, 0, PREBROW_MAXFULLNAMELEN);
		strcpy ( FullName, pFilePath );
		if (FullName[strlen(FullName) -1 ] != '/') 
		{
			strcat(FullName,"/");
		}
		strcat ( FullName, dirinfo->d_name);
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
						if (lmxDifftime(filestat.st_mtime, pFirstNode->atime) > 0)
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
						if (!pFirstNode->hbmp) 
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

	chdir(path);
	loadBitmapForList(hList, 0, ListBuffer);
//	DeleteObject(hbiticon);

	return nRtnNum;
}

/*********************************************************************
* Function	   AddPicturesToList
* Purpose      把图片文件加入列表
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int	PreBrowAddPicturesToList(HWND hList , PCSTR pPath, PLISTBUF pLtB, HBITMAP hFolder)
{
//	char szTemp[PREBROW_MAXFILENAMELEN];
	int n;	
	SendMessage (hList, LB_RESETCONTENT, NULL, NULL);
	PREBROW_FreeListBuf (pLtB);
	
	PreBrowAddFileToList(hList , pPath, pLtB, hFolder);
//	PreBrowAddFileToList(hList, "/rom/game/pao/", pLtB);
	
	n = pLtB->nDataNum;
	if (LB_ERR != n)
	{
		SendMessage(hList,LB_SETCURSEL,(WPARAM)0,NULL);
	}

	return n;
}

/*********************************************************************
* Function	   AddPicturesToList
* Purpose      把图片文件加入列表
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
/*
static int	AddPicturesToList(HWND hList)
{
	char szTemp[PREBROW_MAXFILENAMELEN];
	int n;

	
	SendMessage (hList, LB_RESETCONTENT, NULL, NULL);
	PREBROW_FreeListBuf (&pViewPic->ListBuffer);
	
	FindFileAndAddToList(hList,szCurrentGrpPath, PREBROW_FILEUNIVERSE_BMP, PREBROW_FILETYPE_BMP, NULL, &pViewPic->ListBuffer);
	FindFileAndAddToList(hList,szCurrentGrpPath, PREBROW_FILEUNIVERSE_GIF, PREBROW_FILETYPE_GIF, NULL, &ListBuffer);
	FindFileAndAddToList(hList,szCurrentGrpPath, PREBROW_FILEUNIVERSE_JPG, PREBROW_FILETYPE_JPG, NULL, &ListBuffer);
	FindFileAndAddToList(hList,szCurrentGrpPath, PREBROW_FILEUNIVERSE_JPEG, PREBROW_FILETYPE_JPEG, NULL, &ListBuffer);
	FindFileAndAddToList(hList,szCurrentGrpPath, PREBROW_FILEUNIVERSE_WBMP, PREBROW_FILETYPE_WBMP, NULL, &ListBuffer);	

	FindFileAndAddToList(hList,szCurrentGrpPath, PREBROW_FILEUNIVERSE_BMP1, PREBROW_FILETYPE_BMP, NULL, &ListBuffer);
	FindFileAndAddToList(hList,szCurrentGrpPath, PREBROW_FILEUNIVERSE_GIF1, PREBROW_FILETYPE_GIF, NULL, &ListBuffer);
	FindFileAndAddToList(hList,szCurrentGrpPath, PREBROW_FILEUNIVERSE_JPG1, PREBROW_FILETYPE_JPG, NULL, &ListBuffer);
	FindFileAndAddToList(hList,szCurrentGrpPath, PREBROW_FILEUNIVERSE_JPEG1, PREBROW_FILETYPE_JPEG, NULL, &ListBuffer);
	FindFileAndAddToList(hList,szCurrentGrpPath, PREBROW_FILEUNIVERSE_WBMP1, PREBROW_FILETYPE_WBMP, NULL, &ListBuffer);	

	

	n = SendMessage(hList,LB_GETCOUNT,NULL,NULL);
	if (LB_ERR != n)
	{
		SendMessage(hList,LB_SETCURSEL,(WPARAM)0,NULL);
	}

	if ( n > 0 )
		GetFileNameFromList(pViewPic->hWndListBox, &ListBuffer, szCurrentGrpPath, szTemp, cFileName, &nFileType);
	else
		strcpy( cFileName, "");

	return n;
}
*/
