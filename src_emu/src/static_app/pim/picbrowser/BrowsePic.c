/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : public
 *
 * Purpose  : Í¼Æ¬Ô¤ÀÀ
 *            
\**************************************************************************/

#include "PicBrowser.h"
#include "math.h"


typedef enum {
	BLARGE =1,
	BNORMAL	
	 
}ZMSTATUS;

typedef enum
{
	dirLeft = 1,
	dirRight,
	dirUp,
	dirDown
}DIRECTION;

static	ZMSTATUS	sCurZoomState = BNORMAL;
static		int		clockdegree	=	0;

typedef struct tagBROWSEPICTURE {
	char	cFileFullName[PREBROW_MAXFULLNAMELEN];
	int     itype;
	HWND	hFrame;
	HWND	hListBox;
	HWND	hWndPicturesPreBrow;
	HMENU	hNormalViewmenu;
	HMENU	hSendSelMenu;
	int		nIndex;
	double	dOriginalScaling;
	double	dZoomedScaling;
//	ZMSTATUS	sCurZoomState;
	char	pCurFileName[PREBROW_MAXFILENAMELEN];
	PVOID	pImgData;
	DWORD	dwImgSize;
	HBITMAP	hBmpData;
	PLISTBUF pListBuf;
	BOOL	bFull;
	BOOL	bPan;				//current picture is static gif
	HICON	hRightBmp;
	HICON	hLeftBmp;
	int		x;
	int		y;
	int		width;
	int		height;
//	int		clockdegree;
	int		xpan;
	int		ypan;
}STRUCBROWSEPICTURE, * PSTRBROWSEPICTURE;

#define	MOVESCOPE	5
static LRESULT CALLBACK PicturesPreBrowseWndProc(HWND hWnd,UINT wMsgCmd,
												 WPARAM wParam,LPARAM lParam);

BOOL	ZoomPicture(HWND hWnd,HDC hdc, const char *cFileName, int nFileType,
							 int nHdcShowX, int nHdcShowY, int nHdcShowWidth, int nHdcShowHeight,
							 int degree, int x, int y, HBITMAP hBmp);
double	SetImageStrechSize ( int ScreenWidth, int ScreenHeight, int ImageWidth, int ImageHeight,
							 int *Stretchx, int *Stretchy, int *StretchWidth, int *StretchHeight, double StretchRate);
static	BOOL	RegisterBrowsePicClass();
static	BOOL	PanPicture(DIRECTION dir);
static	void	ZoomImgData(HDC hdc, PVOID pImgData, DWORD dwImgSize, int x, int y, int width, int height);
void PIC_PaintBkFunc(HGIFANIMATE hGIFAnimate, BOOL End, int x, int y, HDC hdcMem);

static MENUITEMTEMPLATE SendMenu[] =
{
	{ MF_STRING, IDC_VIAMMS, "Via MMS", NULL},
	{ MF_STRING, IDC_VIAEMAIL, "Via e-mail", NULL},
	{ MF_STRING, IDC_VIABLUETOOTH, "Via Bluetooth", NULL},
	
	{ MF_END, 0, NULL, NULL }
};

static MENUITEMTEMPLATE RotateMenu[] =
{
	{ MF_STRING, IDC_CLOCKWISE, "Clockwise", NULL},
	{ MF_STRING, IDC_COUNTERCLKWISE, "Counterclockwise", NULL},
	{ MF_END, 0, NULL, NULL }                                                                                                  
};

static MENUITEMTEMPLATE WallPaperMenu[] =
{
	{ MF_STRING, IDC_SET, "Set", NULL},
	{ MF_STRING, IDC_CLEAR, "clear", NULL},
	{ MF_END, 0, NULL, NULL }                                                                                                  
};

static MENUITEMTEMPLATE NormalViewMenu[] =
{
	{ MF_STRING, IDC_FULLSCREEN, "Full screen", NULL},
	{ MF_STRING|MF_POPUP, 0, "Rotate", RotateMenu},
	{ MF_STRING, IDC_ZOOMIN, "Zoom in", NULL},
	{ MF_STRING|MF_POPUP, 0, "send", SendMenu },
	{ MF_STRING, IDC_RENAME, "Rename", NULL},
	{ MF_STRING, IDC_WALLPAPER, "wallpaper", NULL},
	{ MF_STRING, IDC_MOVETOFOLDER, "Move to folder", NULL},	
	{ MF_STRING, IDC_SHOWDETAIL, "Show details", NULL},	
	{ MF_STRING, IDC_DELETE, "Delete", NULL},	
    { MF_END, 0, NULL, NULL }
};

static const MENUTEMPLATE FullViewMenuTemplate =
{
    0,
    NormalViewMenu
};

static const MENUTEMPLATE SendTemplate =
{
    0,
    SendMenu
};

static	BOOL	RegisterBrowsePicClass()
{
	WNDCLASS  wc;
	
	wc.style            = NULL;
	wc.lpfnWndProc      = PicturesPreBrowseWndProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = sizeof(STRUCBROWSEPICTURE);
	wc.hInstance        = NULL;
	wc.hIcon            = NULL;
	wc.hCursor          = NULL;//LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);//NULL;
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = "BrowsePictureWndClass";
	
	if (!RegisterClass(&wc))
		return FALSE;
	return TRUE;
}
BOOL BrowsePicture(HWND hList, HWND hParentWnd, PVOID pImgData, DWORD dwDataSize,PSTR pFileFullName, PLISTBUF pListBuf, BOOL bFullScreen, DWORD dFileType, HBITMAP hBmp)
{
	STRUCBROWSEPICTURE	BPData;
	RECT		rf;
	HWND		hMainWnd;
	PSTR		pCaption;
	PSTR		tmp;
	HMENU		hSend;// hWallPaper;
	HMENU		hRotate;
	int			fullheight;
	PLISTDATA	pNodeData;

//	SIZE		rsize;

	RegisterBrowsePicClass();

	GetClientRect(hParentWnd,&rf);
	BPData.bFull = bFullScreen;
	BPData.hFrame = hParentWnd;
	BPData.dwImgSize = dwDataSize;
	BPData.hLeftBmp = BPData.hRightBmp = NULL;
	//picture preview call this function
	if (hList) 
	{
		GetFileNameFromList(hList, pListBuf, NULL, BPData.pCurFileName, BPData.cFileFullName, &BPData.itype);
		
		BPData.hListBox = hList;
		BPData.pListBuf = pListBuf;
		
		BPData.pImgData = NULL;		
		BPData.nIndex = SendMessage(hList, LB_GETCURSEL, 0, 0);
		pNodeData = PREBROW_GetPDATA(pListBuf, BPData.nIndex);
		BPData.hBmpData = hBmp;
		
	}
	else	//MMS or Email call 
	{
		BPData.hListBox = NULL;
		BPData.pListBuf = NULL;		
		BPData.pImgData = NULL;		
		BPData.nIndex = -1;
		
		if (pFileFullName) 
		{		
			strcpy(BPData.cFileFullName, pFileFullName);
			BPData.pImgData = NULL;
			
			if (dFileType == -1) {
				BPData.itype = GetFileTypeByName(pFileFullName, TRUE);
			}
			else
			{
				BPData.itype = dFileType;
			}
			if (BPData.itype != FILE_TYPE_UNKNOW) {
				
				tmp = strrchr(pFileFullName, '/');
				tmp++;
				pCaption = strrchr(tmp, '.');
				strncpy(BPData.pCurFileName, tmp, pCaption - tmp);
			}
			else
			{
				strcpy(BPData.pCurFileName,"");
			}
		}
		if (pImgData) 
		{
			BPData.itype = PREBROW_FILETYPE_BMP;
			BPData.pImgData = pImgData;
			BPData.cFileFullName[0] = NULL;
			//BPData.pCurFileName[0]= NULL;
			strcpy(BPData.pCurFileName, "");
		}
		
	}
	
	BPData.hNormalViewmenu = LoadMenuIndirect(&FullViewMenuTemplate);
	BPData.hSendSelMenu = LoadMenuIndirect(&SendTemplate);
	/*
	 *	modify menu
	 */
	hSend = GetSubMenu(BPData.hNormalViewmenu, 3);
	hRotate	= GetSubMenu(BPData.hNormalViewmenu, 1);
//	hWallPaper = GetSubMenu(BPData.hNormalViewmenu, 5);

	ModifyMenu(BPData.hNormalViewmenu, IDC_FULLSCREEN, MF_BYCOMMAND, IDC_FULLSCREEN, IDS_FULLSCREEN);
	ModifyMenu(BPData.hNormalViewmenu, 1, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hRotate, IDS_ROTATE);
	ModifyMenu(BPData.hNormalViewmenu, IDC_ZOOMIN, MF_BYCOMMAND, IDC_ZOOMIN, IDS_ZOOMIN);
	ModifyMenu(BPData.hNormalViewmenu, 3, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hSend, IDS_SEND);
	ModifyMenu(BPData.hNormalViewmenu, IDC_DELETE, MF_BYCOMMAND, IDC_DELETE, IDS_DELETE);
	ModifyMenu(BPData.hNormalViewmenu, IDC_RENAME, MF_BYCOMMAND, IDC_RENAME, IDS_RENAME);
	ModifyMenu(BPData.hNormalViewmenu, IDC_MOVETOFOLDER, MF_BYCOMMAND, IDC_MOVETOFOLDER, IDS_MOVETOFOLDER);
	ModifyMenu(BPData.hNormalViewmenu, IDC_WALLPAPER, MF_BYCOMMAND, IDC_WALLPAPER, IDS_WALLPAPER);
	ModifyMenu(BPData.hNormalViewmenu, IDC_SHOWDETAIL, MF_BYCOMMAND, IDC_SHOWDETAIL, IDS_SHOWDETAIL);

	ModifyMenu(BPData.hSendSelMenu, IDC_VIAMMS, MF_BYCOMMAND, IDC_VIAMMS, IDS_VIAMMS);
	ModifyMenu(BPData.hSendSelMenu, IDC_VIABLUETOOTH, MF_BYCOMMAND, IDC_VIABLUETOOTH, IDS_VIABLUETOOTH);
	ModifyMenu(BPData.hSendSelMenu, IDC_VIAEMAIL, MF_BYCOMMAND, IDC_VIAEMAIL, IDS_VIAEMAIL);

	ModifyMenu(hSend, IDC_VIAMMS, MF_BYCOMMAND, IDC_VIAMMS, IDS_VIAMMS);
	ModifyMenu(hSend, IDC_VIABLUETOOTH, MF_BYCOMMAND, IDC_VIABLUETOOTH, IDS_VIABLUETOOTH);
	ModifyMenu(hSend, IDC_VIAEMAIL, MF_BYCOMMAND, IDC_VIAEMAIL, IDS_VIAEMAIL);

	ModifyMenu(hRotate, IDC_CLOCKWISE, MF_BYCOMMAND, IDC_CLOCKWISE, IDS_CLKWISE);
	ModifyMenu(hRotate, IDC_COUNTERCLKWISE, MF_BYCOMMAND, IDC_COUNTERCLKWISE, IDS_COUCLKWISE);

//	ModifyMenu(hWallPaper, IDC_SET, MF_BYCOMMAND, IDC_SET, IDS_SET);
//	ModifyMenu(hWallPaper, IDC_CLEAR, MF_BYCOMMAND, IDC_CLEAR, IDS_CLEAR);

	if(!bFullScreen)
	{
		hMainWnd = CreateWindow(
			"BrowsePictureWndClass", 
			NULL, 
			WS_VISIBLE|WS_CHILD,
			rf.left,
			rf.top,
			rf.right - rf.left,
			rf.bottom - rf.top,
			hParentWnd, 
			NULL,
			NULL, 
			(PVOID)&BPData);
		if (BPData.pCurFileName) 
		{		
			SetWindowText(hParentWnd, BPData.pCurFileName);
		}
		SendMessage(hParentWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_FULLSCREEN);
		SendMessage(hParentWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hParentWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		if (sCurZoomState == BLARGE)
		{
			ModifyMenu(BPData.hNormalViewmenu, IDC_ZOOMIN, MF_BYCOMMAND, IDC_ZOOMOUT, IDS_ZOOMOUT);
		}
		else
		{
			ModifyMenu(BPData.hNormalViewmenu, IDC_ZOOMOUT, MF_BYCOMMAND, IDC_ZOOMIN, IDS_ZOOMIN);
		}
		//
		PDASetMenu(hParentWnd,BPData.hNormalViewmenu);
		UpdateWindow(hParentWnd);
		ShowWindow(hParentWnd,SW_SHOW);
		
	}
	else
	{
		//BPData.pCurFileName[0] = NULL;
		fullheight = GetSystemMetrics(SM_CYMENU);
		hMainWnd = CreateWindow(
			"BrowsePictureWndClass", 
			NULL, 
			WS_VISIBLE|PWS_STATICBAR,
			0, 15, 176, 205+fullheight,
			hParentWnd, 
			BPData.hNormalViewmenu,
			NULL, 
			(PVOID)&BPData);
		SendMessage(hParentWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hParentWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		SendMessage(hParentWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		ModifyMenu(BPData.hNormalViewmenu, IDC_FULLSCREEN, MF_BYCOMMAND, IDC_NORMALSCREEN, IDS_NORMALSCREEN);
		if (sCurZoomState == BLARGE)
		{
			ModifyMenu(BPData.hNormalViewmenu, IDC_ZOOMIN, MF_BYCOMMAND, IDC_ZOOMOUT, IDS_ZOOMOUT);
		}
		else
		{
			ModifyMenu(BPData.hNormalViewmenu, IDC_ZOOMOUT, MF_BYCOMMAND, IDC_ZOOMIN, IDS_ZOOMIN);
		}
		PDASetMenu(hParentWnd,NULL);
		UpdateWindow(hParentWnd);
		//ShowWindow(hParentWnd,SW_SHOW);
	}
	if (!IsWindow(hMainWnd))
	{
		return FALSE;
	}
    SetFocus(hMainWnd);
	return TRUE;
}


static LRESULT CALLBACK PicturesPreBrowseWndProc(HWND hWnd,UINT wMsgCmd,
												 WPARAM wParam,LPARAM lParam)
{
	LRESULT     lResult;
	HDC		    hdc;
	PSTRBROWSEPICTURE	pBrowPic;
	PCREATESTRUCT	pcrstr;	
	RECT rcClient;	
	HDC hdcMem;
	HDC			hGifDc;
	PAINTSTRUCT	ps;
	int		Stretchx, Stretchy, StretchWidth, StretchHeight;
	int		index, nPicture;
	SIZE	imgSize;
	char	delprompt[PREBROW_MAXFULLNAMELEN+20];
	char	*p;
	HBITMAP		hOrigBmp = NULL;

	
	lResult = TRUE;
	pBrowPic = GetUserData(hWnd);

	switch(wMsgCmd)
	{
	case WM_CREATE:
		{
			int	iPicType;
			pcrstr = (PCREATESTRUCT)lParam;
			memcpy(pBrowPic, pcrstr->lpCreateParams, sizeof(STRUCBROWSEPICTURE));
			//set caption icon
			GetImageDimensionFromFile(RIGHTARROWICON, &imgSize);
			
//			if (pBrowPic->hRightBmp == NULL)		
//				pBrowPic->hRightBmp =LoadImage(NULL, RIGHTARROWICON, IMAGE_ICON,imgSize.cx,imgSize.cy, LR_LOADFROMFILE);

			
			//SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)pBrowPic->hRightBmp);

			GetImageDimensionFromFile(LEFTARROWICON, &imgSize);
//			if (pBrowPic->hLeftBmp == NULL)		
//				pBrowPic->hLeftBmp =LoadImage(NULL, LEFTARROWICON, IMAGE_ICON,imgSize.cx,imgSize.cy, LR_LOADFROMFILE);
			//SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)pBrowPic->hLeftBmp);
			
			if (pBrowPic->hListBox) 
			{
				index = SendMessage(pBrowPic->hListBox, LB_GETCURSEL, 0, 0);
				iPicType = SendMessage(pBrowPic->hListBox, LB_GETITEMDATA, index-1, 0);
				if ((iPicType != PREBROW_FILETYPE_FOLDER)&&(iPicType != LB_ERR)&&(iPicType != PREBROW_FILETYPE_MEMEORY)) 
				{
					SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)LEFTARROWICON);
				}
				else
				{
					SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
				}
				iPicType = SendMessage(pBrowPic->hListBox, LB_GETITEMDATA, index +1, 0);
				if ((iPicType != PREBROW_FILETYPE_FOLDER)&&(iPicType != LB_ERR)&&(iPicType != PREBROW_FILETYPE_MEMEORY)) 
				{
					SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)RIGHTARROWICON);
				}
				else
				{
					SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
				}

			}

			//get initial size suit to screen
			pBrowPic->hWndPicturesPreBrow = hWnd;
			GetClientRect(hWnd, &rcClient);
			if (pBrowPic->pImgData) 
			{
				GetImageDimensionFromData(pBrowPic->pImgData, sizeof(pBrowPic->pImgData), &imgSize);
			}
			else
			{				
				GetImageDimensionFromFile(pBrowPic->cFileFullName, &imgSize);
				pBrowPic->dOriginalScaling = GetImageStrechSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, imgSize.cx, imgSize.cy,
					&pBrowPic->x, &pBrowPic->y, &pBrowPic->width, &pBrowPic->height);
				if(sCurZoomState == BNORMAL)
					pBrowPic->dZoomedScaling = pBrowPic->dOriginalScaling;
				else
					pBrowPic->dZoomedScaling = 2*pBrowPic->dOriginalScaling;

				SetImageStrechSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, 
					imgSize.cx, imgSize.cy,	&pBrowPic->x, &pBrowPic->y, 
					&pBrowPic->width, &pBrowPic->height, pBrowPic->dZoomedScaling);
				
				if (pBrowPic->itype != PREBROW_FILETYPE_GIF)
				{
					//pBrowPic->hBmpData = CreateBitmapFromImageFile(NULL, pBrowPic->cFileFullName, NULL,NULL);
					hOrigBmp = CreateBitmapFromImageFile(NULL, pBrowPic->cFileFullName, NULL,NULL);
					pBrowPic->hBmpData = AdjustBmpToScreen(hOrigBmp, 0, 0, pBrowPic->width, pBrowPic->height, pBrowPic->cFileFullName);
					if (hOrigBmp)
					{
						DeleteObject(hOrigBmp);
						hOrigBmp = NULL;
					}
				}
				else
				{
					if (!IsGIFDynamic(pBrowPic->cFileFullName, TRUE, -1))
					{
						pBrowPic->bPan = TRUE;
						//pBrowPic->hBmpData = CreateBitmapFromImageFile(NULL, pBrowPic->cFileFullName, NULL,NULL);
						hOrigBmp = CreateBitmapFromImageFile(NULL, pBrowPic->cFileFullName, NULL,NULL);
						pBrowPic->hBmpData = AdjustBmpToScreen(hOrigBmp, 0, 0, pBrowPic->width, pBrowPic->height, pBrowPic->cFileFullName);
						if (hOrigBmp)
						{
							DeleteObject(hOrigBmp);
							hOrigBmp = NULL;
						}
					}
					else
					{
						pBrowPic->bPan = FALSE;
						pBrowPic->hBmpData = (HBITMAP)StartAnimatedGIFFromFileEx(hWnd, pBrowPic->cFileFullName,
							pBrowPic->x, pBrowPic->y, pBrowPic->width, pBrowPic->height, DM_NONE);
						SetPaintBkFunc((HGIFANIMATE)pBrowPic->hBmpData, PIC_PaintBkFunc);
						hGifDc = GetDC(hWnd);
						PaintAnimatedGIF(hGifDc, (HGIFANIMATE)pBrowPic->hBmpData);
						ReleaseDC(hWnd, hGifDc);
					}
					
				}
				//pBrowPic->sCurZoomState = BNORMAL;
			
			}
			//pBrowPic->clockdegree = 0;
			pBrowPic->xpan = 0;
			pBrowPic->ypan = 0;
		}
		break;


	case PWM_SHOWWINDOW:
		if (pBrowPic->bFull) 
		{
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
			//PDASetMenu(hWnd,pBrowPic->hNormalViewmenu);
			//UpdateWindow(pBrowPic->hFrame);
			//ShowOwnedPopups(hWnd, TRUE);
		}
		else
		{
			index = SendMessage(pBrowPic->hListBox, LB_GETCURSEL, 0, 0);
			SendMessage(pBrowPic->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_FULLSCREEN);
			SendMessage(pBrowPic->hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
			SendMessage(pBrowPic->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
			
			if (sCurZoomState == BNORMAL) 
			{
				if (index > 0) {
					SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)LEFTARROWICON);
				}
				nPicture = SendMessage(pBrowPic->hListBox, LB_GETITEMDATA, index+1, 0);
				if ((nPicture != LB_ERR)&&(nPicture != PREBROW_FILETYPE_FOLDER)&&(nPicture != PREBROW_FILETYPE_MEMEORY)) 
				{
					SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)RIGHTARROWICON);
				}
			}
			
			PDASetMenu(pBrowPic->hFrame,pBrowPic->hNormalViewmenu);
			SetWindowText(pBrowPic->hFrame,pBrowPic->pCurFileName);
			//UpdateWindow(pBrowPic->hFrame);
		}
		SetFocus(hWnd);
		break;
		
	case WM_PAINT:
		{
			HBRUSH	hWhiteBrush;
			if (pBrowPic->pImgData) 
			{
				hWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
				hdc=BeginPaint(hWnd,&ps);
				
				GetClientRect(hWnd, &rcClient);
				hdcMem = CreateMemoryDC(rcClient.right, rcClient.bottom);
				SetClipRect(hdcMem, &ps.rcPaint);		
				FillRect ( hdcMem, &rcClient, hWhiteBrush );
				
				GetImageDimensionFromData(pBrowPic->pImgData, pBrowPic->dwImgSize, &imgSize);
				
				
				SetImageStrechSize (rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, imgSize.cx, imgSize.cy,
					&Stretchx, &Stretchy, &StretchWidth, &StretchHeight, pBrowPic->dZoomedScaling);
				
				ZoomImgData(hdcMem, pBrowPic->pImgData, pBrowPic->dwImgSize, Stretchx, Stretchy, StretchWidth, StretchHeight);
				BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hdcMem, pBrowPic->xpan, pBrowPic->ypan, ROP_SRC);
				DeleteDC(hdcMem);
				EndPaint(hWnd,&ps);
			}
			else
			{
//				if (!pBrowPic->hBmpData)
//				{
//					PLXTipsWin(NULL, hWnd, IDRM_NOMEMORY, IDS_MEMORYFULL,pBrowPic->pCurFileName, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
////#ifdef _DEBUG
//					printf("\r\nFILE:%s \t LINE:%d\n", __FILE__,  __LINE__);
////#endif
//					break;
//				}
				hWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
				hdc=BeginPaint(hWnd,&ps);
				
				GetClientRect(hWnd, &rcClient);
				//ClearRect(hdc, &rcClient, COLOR_WHITE);
				///*
				hdcMem = CreateMemoryDC(rcClient.right, rcClient.bottom);
				SetClipRect(hdcMem, &ps.rcPaint);		
				FillRect ( hdcMem, &rcClient, hWhiteBrush );
				//*/
				GetFileNameFromList(pBrowPic->hListBox, pBrowPic->pListBuf, NULL, pBrowPic->pCurFileName,
								pBrowPic->cFileFullName, &pBrowPic->itype);
				GetImageDimensionFromFile(pBrowPic->cFileFullName, &imgSize);
				
				SetImageStrechSize (rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, imgSize.cx, imgSize.cy,
						&Stretchx, &Stretchy, &StretchWidth, &StretchHeight, pBrowPic->dZoomedScaling);

				if ((pBrowPic->itype != PREBROW_FILETYPE_GIF)||(pBrowPic->bPan))
				{
					//SetImageStrechSize (rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, imgSize.cx, imgSize.cy,
						//&Stretchx, &Stretchy, &StretchWidth, &StretchHeight, pBrowPic->dZoomedScaling);
					
					if(!ZoomPicture(hWnd, hdcMem, pBrowPic->cFileFullName, pBrowPic->itype,
						Stretchx, Stretchy, StretchWidth, StretchHeight, 
						clockdegree, pBrowPic->xpan, pBrowPic->ypan, pBrowPic->hBmpData))
					{
//						TRACEERROR();
						PLXTipsWin(NULL, hWnd, IDRM_NOMEMORY, IDS_MEMORYFULL,pBrowPic->pCurFileName, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
//#ifdef _DEBUG
						//printf("\r\nFILE:%s  \t LINE:%d\t Memory:%d kB\n", __FILE__,  __LINE__, GetFreeMemory());
//#endif
					}
				}
				else
				{
					if (!pBrowPic->hBmpData)
					{
						pBrowPic->hBmpData = (HBITMAP)StartAnimatedGIFFromFileEx(hWnd, pBrowPic->cFileFullName, Stretchx, Stretchy, StretchWidth, StretchHeight,DM_NONE);
						if (!pBrowPic->hBmpData)
						{
						//	TRACEERROR();
							DeleteDC(hdcMem);
							EndPaint(hWnd,&ps);
							return FALSE;
						}
						SetPaintBkFunc((HGIFANIMATE)pBrowPic->hBmpData, PIC_PaintBkFunc);
						PaintAnimatedGIF(hdcMem, (HGIFANIMATE)pBrowPic->hBmpData);
					}
					else
					{
						PaintAnimatedGIF(hdcMem, (HGIFANIMATE)pBrowPic->hBmpData);
					}
				}
				
				BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hdcMem, 0, 0, ROP_SRC);
				DeleteDC(hdcMem);
				EndPaint(hWnd,&ps);
			}
		}
		break;

	case WM_ERASEBKGND:
		break;
		
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDC_FULLSCREEN:				
				SendMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				PostMessage(hWnd,WM_CLOSE,0,0);
				//DestroyWindow(hWnd);
				BrowsePicture(pBrowPic->hListBox, pBrowPic->hFrame, pBrowPic->pImgData, pBrowPic->dwImgSize, 
					pBrowPic->cFileFullName, pBrowPic->pListBuf, TRUE, pBrowPic->itype, pBrowPic->hBmpData);
				break;

			case IDC_NORMALSCREEN:
				//DestroyWindow(hWnd);
				PostMessage(hWnd,WM_CLOSE,0,0);
				BrowsePicture(pBrowPic->hListBox, pBrowPic->hFrame, pBrowPic->pImgData, pBrowPic->dwImgSize, 
					pBrowPic->cFileFullName, pBrowPic->pListBuf, FALSE, pBrowPic->itype, pBrowPic->hBmpData);
				break;

			case IDC_ZOOMIN:
				if (sCurZoomState == BNORMAL) {
					pBrowPic->dZoomedScaling = 2*pBrowPic->dOriginalScaling;
					sCurZoomState = BLARGE;
					pBrowPic->xpan = 0;
					pBrowPic->ypan = 0;

					GetClientRect(hWnd, &rcClient);
					if (pBrowPic->pImgData) {
						GetImageDimensionFromData(pBrowPic->pImgData, pBrowPic->dwImgSize, &imgSize);
					}
					else
						GetImageDimensionFromFile(pBrowPic->cFileFullName, &imgSize);
					
					
					SetImageStrechSize (rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, imgSize.cx, imgSize.cy,
						&Stretchx, &Stretchy, &StretchWidth, &StretchHeight, pBrowPic->dZoomedScaling);
					pBrowPic->x = Stretchx;
					pBrowPic->y = Stretchy;
					ModifyMenu(pBrowPic->hNormalViewmenu, IDC_ZOOMIN, MF_BYCOMMAND, IDC_ZOOMOUT, IDS_ZOOMOUT);
					SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
					SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
					if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
					{
						EndAnimatedGIF((HGIFANIMATE)pBrowPic->hBmpData);

						pBrowPic->hBmpData = (HBITMAP)StartAnimatedGIFFromFileEx(hWnd, pBrowPic->cFileFullName,
							Stretchx, Stretchy, StretchWidth, StretchHeight, DM_NONE);

						SetPaintBkFunc((HGIFANIMATE)pBrowPic->hBmpData, PIC_PaintBkFunc);
						hGifDc = GetDC(hWnd);
						PaintAnimatedGIF(hGifDc, (HGIFANIMATE)pBrowPic->hBmpData);
						ReleaseDC(hWnd, hGifDc);
					}
					InvalidateRect(hWnd, NULL, TRUE);
				}				
				break;

			case IDC_ZOOMOUT:
				if (sCurZoomState == BLARGE) 
				{
					pBrowPic->dZoomedScaling = pBrowPic->dOriginalScaling;
					sCurZoomState = BNORMAL;
					pBrowPic->xpan = 0;
					pBrowPic->ypan = 0;
					ModifyMenu(pBrowPic->hNormalViewmenu, IDC_ZOOMOUT, MF_BYCOMMAND, IDC_ZOOMIN, IDS_ZOOMIN);
					index = SendMessage(pBrowPic->hListBox, LB_GETCURSEL, 0, 0);
					if (index > 0) {
						SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)LEFTARROWICON);
					}
					nPicture = SendMessage(pBrowPic->hListBox, LB_GETITEMDATA, index+1, 0);
					if ((nPicture != LB_ERR)&&(nPicture != PREBROW_FILETYPE_FOLDER)&&(nPicture != PREBROW_FILETYPE_MEMEORY)) 
					{
						SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)RIGHTARROWICON);
					}
					
					if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
					{
						GetImageDimensionFromFile(pBrowPic->cFileFullName, &imgSize);
						GetClientRect(hWnd, &rcClient);
						SetImageStrechSize (rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, imgSize.cx, imgSize.cy,
							&Stretchx, &Stretchy, &StretchWidth, &StretchHeight, pBrowPic->dZoomedScaling);
						
						EndAnimatedGIF((HGIFANIMATE)pBrowPic->hBmpData);
						
						pBrowPic->hBmpData = (HBITMAP)StartAnimatedGIFFromFileEx(hWnd, pBrowPic->cFileFullName,
							Stretchx, Stretchy, StretchWidth, StretchHeight, DM_NONE);
						
						SetPaintBkFunc((HGIFANIMATE)pBrowPic->hBmpData, PIC_PaintBkFunc);
						hGifDc = GetDC(hWnd);
						PaintAnimatedGIF(hGifDc, (HGIFANIMATE)pBrowPic->hBmpData);
						ReleaseDC(hWnd, hGifDc);
					}
					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;

			case IDC_DELETE:
				if (!pBrowPic->hListBox) 
				{
					sprintf(delprompt, "%s:\n%s", pBrowPic->pCurFileName, IDS_DELETE);
					PLXConfirmWinEx(NULL, hWnd, delprompt, Notify_Request, IDS_CAPTION, IDS_YES, IDS_NO, IDC_PROCESSDEL);
				}
				if(GetFileNameFromList(pBrowPic->hListBox, pBrowPic->pListBuf, NULL, pBrowPic->pCurFileName,
								pBrowPic->cFileFullName, &pBrowPic->itype))
				{
					if (pBrowPic->itype == PREBROW_FILETYPE_FOLDER) 
					{
						sprintf(delprompt, "%s:\n%s", pBrowPic->pCurFileName, IDS_DELFOLDER);
						PLXConfirmWinEx(NULL, hWnd, delprompt, Notify_Request, IDS_CAPTION, IDS_YES, IDS_NO, IDC_PROCESSDEL);
					}
					else
					{
						sprintf(delprompt, "%s:\n%s?", pBrowPic->pCurFileName, IDS_DELETE);
						PLXConfirmWinEx(NULL, hWnd, delprompt, Notify_Request, IDS_CAPTION, IDS_YES, IDS_NO, IDC_PROCESSDEL);				
					}
				}
				break;

			case IDC_SHOWDETAIL:
				if(GetFileNameFromList(pBrowPic->hListBox, pBrowPic->pListBuf, NULL, pBrowPic->pCurFileName,
								pBrowPic->cFileFullName, &pBrowPic->itype))
				{
					if (pBrowPic->bFull) 
					{
						ShowPicDetail(hWnd, pBrowPic->pCurFileName, pBrowPic->cFileFullName, pBrowPic->itype, TRUE);
					}
					else
					{
						ShowPicDetail(pBrowPic->hFrame, pBrowPic->pCurFileName, pBrowPic->cFileFullName, pBrowPic->itype, FALSE);
					}
					
				}
				break;
				
			case IDC_SET:
				if(GetFileNameFromList(pBrowPic->hListBox, pBrowPic->pListBuf, NULL, pBrowPic->pCurFileName,
					pBrowPic->cFileFullName, &pBrowPic->itype))
				{
					if(SetBKPicture(pBrowPic->cFileFullName))
					{
						CallIdleBkPicChange();
						PLXTipsWin(NULL, hWnd, 0, IDS_WALLPAPERSET, NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
					}
				}
				break;

			case IDC_CLEAR:
				if(SetBKPicture("Default"))
				{
					CallIdleBkPicChange();
					PLXTipsWin(NULL, hWnd, 0, IDS_WALLPAPERCLEAR, NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
				}
				break;

			case IDC_VIABLUETOOTH:
				if(GetFileNameFromList(pBrowPic->hListBox, pBrowPic->pListBuf, NULL, pBrowPic->pCurFileName,
					pBrowPic->cFileFullName, &pBrowPic->itype))
				{
					SendMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					BtSendData(pBrowPic->hFrame, pBrowPic->cFileFullName, pBrowPic->pCurFileName, BTPICTURE);
				}
				break;

			case IDC_VIAMMS:
				if(GetFileNameFromList(pBrowPic->hListBox, pBrowPic->pListBuf, NULL, pBrowPic->pCurFileName,
					pBrowPic->cFileFullName, &pBrowPic->itype))
				{
					SendMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					APP_EditMMS(pBrowPic->hFrame, hWnd, IDRM_EDITMMS, MMS_CALLEDIT_IMAGE, pBrowPic->cFileFullName);
				}
				break;

			case IDC_VIAEMAIL:
				if(GetFileNameFromList(pBrowPic->hListBox, pBrowPic->pListBuf, NULL, pBrowPic->pCurFileName,
					pBrowPic->cFileFullName, &pBrowPic->itype))
				{
					p = strrchr(pBrowPic->cFileFullName,'/');
					//strcpy(szTemp, p+1);
					SendMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					MAIL_CreateEditInterface(pBrowPic->hFrame, NULL, pBrowPic->cFileFullName, p+1, 1);
				}
				break;

			case IDC_MOVETOFOLDER:
				PicMoveToFolder(hWnd, IDRM_MOTOFOLDER);
				break;

			case IDC_WALLPAPER:
				PicSetWallPaper(hWnd);
				break;
				
			case IDC_CLOCKWISE:
				if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
				{
					PLXTipsWin(NULL, hWnd, 0, ML("Dynamic Gif can't rotate"), pBrowPic->pCurFileName, Notify_Info, 
						IDS_OK, NULL, WAITTIMEOUT);
					break;
				}
				if (clockdegree == 0) {
					clockdegree = 3;
				}
				else
					clockdegree --;
				InvalidateRect(hWnd, NULL, TRUE);
				break;

			
			case IDC_COUNTERCLKWISE:
				if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
				{
					PLXTipsWin(NULL, hWnd, 0, ML("Dynamic Gif can't rotate"), pBrowPic->pCurFileName, Notify_Info, 
						IDS_OK, NULL, WAITTIMEOUT);
					break;
				}
				if (clockdegree == 3) {
					clockdegree = 0;
				}
				else
					clockdegree ++;
				InvalidateRect(hWnd, NULL, TRUE);
				break;

			case IDC_RENAME:
				if(GetFileNameFromList(pBrowPic->hListBox, pBrowPic->pListBuf, NULL, pBrowPic->pCurFileName,
					pBrowPic->cFileFullName, &pBrowPic->itype))
					PicBrowser_Edit(hWnd,  pBrowPic->pCurFileName, pBrowPic->cFileFullName, FALSE);
				break;

			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
			}

		}
		break;

	case IDRM_MOTOFOLDER:
		{
			char	path[200];
			char	*name;
			FILE	*fp;
			int		index, AvailSpace;
			struct	stat	filestat;
			strcpy(path, (char *)lParam);
			
			if (path[strlen(path) -1 ] != '/') {
				strcat(path, "/");
			}
			name = strrchr(pBrowPic->cFileFullName, '/');
			name++;
			strcat(path, name);
			if (strcmp(path, pBrowPic->cFileFullName) == 0) 
			{
				//PLXTipsWin(pBrowPic->hFrame, hWnd, 0, ML("Moved"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
				PostMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				PostMessage(hWnd,WM_CLOSE,0,0);
			}
			else
			{
				if (strnicmp(pBrowPic->cFileFullName, path, 10) != 0)
				{
					stat(pBrowPic->cFileFullName, &filestat);
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
						PLXTipsWin(NULL, hWnd, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
						//SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
						//DestroyWindow(hwnd);
						return FALSE;
					}
				}
				if ((fp = fopen(path, "r")) != NULL)
				{
					fclose(fp);
					if(!DefAnotherName(NULL, hWnd, pBrowPic->cFileFullName, path, pBrowPic->pCurFileName, 0, 0))
					{
						PostMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
						PostMessage(hWnd,WM_CLOSE,0,0);
						break;
					}
					//PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
				}
				
				if(PicMoveFile(pBrowPic->cFileFullName, path))
				{
					
					index = SendMessage(pBrowPic->hListBox, LB_GETCURSEL, 0, 0);
					PREBROW_DelData(pBrowPic->pListBuf, index);
					SendMessage(pBrowPic->hListBox, LB_DELETESTRING, index, 0);
					//if (index > 0) {
					SendMessage(pBrowPic->hListBox, LB_SETCURSEL, (index > 0)?index -1: 0, 0);
					//}
					PLXTipsWin(pBrowPic->hFrame, hWnd, 0, ML("Moved"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
					PostMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					PostMessage(hWnd,WM_CLOSE,0,0);
				}
			}
		}
		break;
	
	case IDRM_NOMEMORY:
	case IDRM_EDITMMS:	
	case IDRM_CLOSEWINDOW:
		SendMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		PostMessage(hWnd,WM_CLOSE,0,0);
		break;

	case IDC_PROCESSDEL:
		if (lParam == 1) 
		{
			if (pBrowPic->hListBox) 
			{				
				WaitWindowStateEx(pBrowPic->hFrame, TRUE, IDS_DELETING, NULL, NULL, NULL);
				index = SendMessage(pBrowPic->hListBox,LB_GETCURSEL,0,0);
				GetFileNameFromList(pBrowPic->hListBox, pBrowPic->pListBuf, NULL, pBrowPic->pCurFileName,
					pBrowPic->cFileFullName, &pBrowPic->itype);
				SendMessage(pBrowPic->hListBox, LB_DELETESTRING, index, 0);
				PREBROW_DelData(pBrowPic->pListBuf, index);
				if (pBrowPic->itype == PREBROW_FILETYPE_FOLDER) {
					DeleteDirectory(pBrowPic->cFileFullName, TRUE);
				}
				else
				{
					remove(pBrowPic->cFileFullName);
				}
				WaitWindowStateEx(pBrowPic->hFrame, FALSE, IDS_DELETING, NULL, NULL, NULL);
				PLXTipsWin(pBrowPic->hFrame, hWnd, 0, IDS_DELETED, NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
				PostMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				PostMessage(hWnd,WM_CLOSE,0,0);
				/*
				if (index > 0) {
					SendMessage(pBrowPic->hListBox, LB_SETCURSEL, index -1, 0);
				}
				else
				{
					if(SendMessage(pBrowPic->hListBox, LB_SETCURSEL, index+1, 0) == LB_ERR)
					{
						SendMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
						PostMessage(hWnd,WM_CLOSE,0,0);
					}
					nPicture = SendMessage(pBrowPic->hListBox, LB_GETITEMDATA, index+1, 0);
					if (nPicture == PREBROW_FILETYPE_FOLDER) {
						SendMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
						PostMessage(hWnd,WM_CLOSE,0,0);
					}
				}
				*/
				
			}
			else
			{
				WaitWindowStateEx(pBrowPic->hFrame, TRUE, IDS_DELETING, NULL, NULL, NULL);
				remove(pBrowPic->cFileFullName);
				WaitWindowStateEx(pBrowPic->hFrame, FALSE, IDS_DELETING, NULL, NULL, NULL);
				SendMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				PostMessage(hWnd,WM_CLOSE,0,0);
			}
		}
		break;

	

	case WM_CLOSE:
		DestroyWindow (hWnd);		
		break;
		
	case WM_DESTROY:
//		if (pBrowPic->hLeftBmp) {
//			DestroyIcon(pBrowPic->hLeftBmp);
//		}
//		if (pBrowPic->hRightBmp) {
//			DestroyIcon(pBrowPic->hRightBmp);
//		}
		if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
		{
			if (pBrowPic->hBmpData)
			{
				EndAnimatedGIF((HGIFANIMATE)pBrowPic->hBmpData);
			}
		}
		else
		{
			if (pBrowPic->hBmpData)
			{
				DeleteObject(pBrowPic->hBmpData);
			}			
		}
		DestroyMenu(pBrowPic->hNormalViewmenu);
		DestroyMenu(pBrowPic->hSendSelMenu);
		UnregisterClass ("BrowsePictureWndClass", NULL);		
		break;

	case WM_KEYDOWN : 
		{
			switch(LOWORD(wParam))
			{
			case VK_RETURN:
				if (pBrowPic->bFull) {
					
					BrowsePicture(pBrowPic->hListBox, pBrowPic->hFrame, pBrowPic->pImgData, pBrowPic->dwImgSize, pBrowPic->cFileFullName, 
						pBrowPic->pListBuf, FALSE, pBrowPic->itype, pBrowPic->hBmpData);
					
					PostMessage(hWnd,WM_CLOSE,0,0);
					break;
				}
				else
				{
					SendMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					BrowsePicture(pBrowPic->hListBox, pBrowPic->hFrame, pBrowPic->pImgData, pBrowPic->dwImgSize, 
						pBrowPic->cFileFullName, pBrowPic->pListBuf, TRUE, pBrowPic->itype, pBrowPic->hBmpData);
					
					PostMessage(hWnd,WM_CLOSE,0,0);
					break;
				}
				break;
				
			case VK_F10:
				if (sCurZoomState != BNORMAL)
				{
					pBrowPic->dZoomedScaling = pBrowPic->dOriginalScaling;
					sCurZoomState = BNORMAL;
					//clockdegree	=	0;
					pBrowPic->xpan = 0;
					pBrowPic->ypan = 0;
					ModifyMenu(pBrowPic->hNormalViewmenu, IDC_ZOOMOUT, MF_BYCOMMAND, IDC_ZOOMIN, IDS_ZOOMIN);

					index = SendMessage(pBrowPic->hListBox, LB_GETCURSEL, 0, 0);
					if (index > 0) {
						SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)LEFTARROWICON);
					}
					nPicture = SendMessage(pBrowPic->hListBox, LB_GETITEMDATA, index+1, 0);
					if ((nPicture != LB_ERR)&&(nPicture != PREBROW_FILETYPE_FOLDER)&&(nPicture != PREBROW_FILETYPE_MEMEORY)) 
					{
						SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)RIGHTARROWICON);
					}
					
					if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
					{
						GetClientRect(hWnd, &rcClient);
						GetImageDimensionFromFile(pBrowPic->cFileFullName, &imgSize);
						pBrowPic->dOriginalScaling = GetImageStrechSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, imgSize.cx, imgSize.cy,
							&pBrowPic->x, &pBrowPic->y, &pBrowPic->width, &pBrowPic->height);
						
						EndAnimatedGIF((HGIFANIMATE)pBrowPic->hBmpData);

						pBrowPic->hBmpData = (HBITMAP)StartAnimatedGIFFromFileEx(hWnd, pBrowPic->cFileFullName, pBrowPic->x, pBrowPic->y,
							pBrowPic->width, pBrowPic->height, DM_NONE);

						SetPaintBkFunc((HGIFANIMATE)pBrowPic->hBmpData, PIC_PaintBkFunc);
					}
					InvalidateRect(hWnd, NULL, TRUE);
					break;
				}
//				if (clockdegree != 0)
//				{
//					if (pBrowPic->bFull) {
//						BrowsePicture(pBrowPic->hListBox, pBrowPic->hFrame, pBrowPic->pImgData, pBrowPic->dwImgSize, 
//							pBrowPic->cFileFullName, pBrowPic->pListBuf, FALSE, pBrowPic->itype, pBrowPic->hBmpData);
//						PostMessage(hWnd,WM_CLOSE,0,0);
//						break;
//					}
//					else
//						PostMessage(hWnd,WM_CLOSE,0,0);
//					break;
//				}
				if (pBrowPic->bFull) {
					BrowsePicture(pBrowPic->hListBox, pBrowPic->hFrame, pBrowPic->pImgData, pBrowPic->dwImgSize, 
						pBrowPic->cFileFullName, pBrowPic->pListBuf, FALSE, pBrowPic->itype, pBrowPic->hBmpData);
					PostMessage(hWnd,WM_CLOSE,0,0);
					break;
				}
				PostMessage(pBrowPic->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				PostMessage(hWnd,WM_CLOSE,0,0);
				break;

			case VK_LEFT:
				{
					int	nCount, iPicType;
					HDC		hdc;
					//Parallel scrolling,previous picture is shown, all pictures can be browsed and seen 
					//until the first pinture in the list;
					if (sCurZoomState != BLARGE) 
					{
						if (!pBrowPic->hListBox) {
							break;
						}
						nCount = SendMessage(pBrowPic->hListBox, LB_GETCOUNT, 0, 0);
						pBrowPic->nIndex = SendMessage(pBrowPic->hListBox, LB_GETCURSEL, 0, 0);
						//if
						if (pBrowPic->nIndex==0)  {
							break;
						}
						//pBrowPic->clockdegree =0;
						iPicType = SendMessage(pBrowPic->hListBox, LB_GETITEMDATA, pBrowPic->nIndex-1, 0);
//						if (iPicType == -1) {
//							SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), NULL);
//						}
						if ((iPicType != PREBROW_FILETYPE_FOLDER) &&(iPicType != -1)&&(iPicType != PREBROW_FILETYPE_MEMEORY))
						{
							
							SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)RIGHTARROWICON);

							GetClientRect(hWnd, &rcClient);
							
							//free current picture resource
							if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
							{
								if (pBrowPic->hBmpData)
								{
									EndAnimatedGIF((HGIFANIMATE)pBrowPic->hBmpData);
								}
							}
							else
							{
								if (pBrowPic->hBmpData)
								{
									DeleteObject(pBrowPic->hBmpData);
								}			
							}

							pBrowPic->nIndex--;
							SendMessage(pBrowPic->hListBox, LB_SETCURSEL, pBrowPic->nIndex, 0);
							if (pBrowPic->nIndex == 0) 
							{
								SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
							}
							else
								SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)LEFTARROWICON);
							
							GetFileNameFromList(pBrowPic->hListBox, pBrowPic->pListBuf, NULL, pBrowPic->pCurFileName,
								pBrowPic->cFileFullName, &pBrowPic->itype);

							if (pBrowPic->itype == PREBROW_FILETYPE_GIF)
							{
								if (!IsGIFDynamic(pBrowPic->cFileFullName, TRUE, -1))
								{
									pBrowPic->bPan = TRUE;
								}
								else
									pBrowPic->bPan = FALSE;
								
							}
							else
								pBrowPic->bPan = FALSE;
							
							if (IsBigFileFromSD(pBrowPic->cFileFullName))
							{
								WaitWindowStateEx(hWnd, TRUE, IDS_OPENING, IDS_CAPTION, NULL, NULL);
							}
							SetWindowText(pBrowPic->hFrame, pBrowPic->pCurFileName);
							GetImageDimensionFromFile(pBrowPic->cFileFullName, &imgSize);
							pBrowPic->dOriginalScaling = GetImageStrechSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, imgSize.cx, imgSize.cy,
								&pBrowPic->x, &pBrowPic->y, &pBrowPic->width, &pBrowPic->height);
							
							if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
							{
								pBrowPic->hBmpData = (HBITMAP)StartAnimatedGIFFromFileEx(hWnd, pBrowPic->cFileFullName, pBrowPic->x, pBrowPic->y, pBrowPic->width, pBrowPic->height, DM_NONE);
								//PaintAnimatedGIF()
								SetPaintBkFunc((HGIFANIMATE)pBrowPic->hBmpData, PIC_PaintBkFunc);
								hdc = GetDC(hWnd);
								PaintAnimatedGIF(hdc, (HGIFANIMATE)pBrowPic->hBmpData);
								ReleaseDC(hWnd, hdc);
							}
							else
							{
								//pBrowPic->hBmpData = CreateBitmapFromImageFile(NULL, pBrowPic->cFileFullName, NULL, NULL);
								hOrigBmp = CreateBitmapFromImageFile(NULL, pBrowPic->cFileFullName, NULL,NULL);
								pBrowPic->hBmpData = AdjustBmpToScreen(hOrigBmp, 0, 0, pBrowPic->width, pBrowPic->height, pBrowPic->cFileFullName);
								if (hOrigBmp)
								{
									DeleteObject(hOrigBmp);
									hOrigBmp = NULL;
								}
							}
							pBrowPic->dZoomedScaling = pBrowPic->dOriginalScaling;
							pBrowPic->xpan = 0;
							pBrowPic->ypan = 0;
							if (IsBigFileFromSD(pBrowPic->cFileFullName))
							{
								WaitWindowStateEx(hWnd, FALSE, IDS_OPENING, IDS_CAPTION, NULL, NULL);
							}
							else
							{
								InvalidateRect(hWnd, NULL, TRUE);
							}
							//pBrowPic->sCurZoomState = BNORMAL;
						}
						//pBrowPic->xpan = 0;
						//pBrowPic->ypan = 0;
						
					}
					//Pan the picture 
					else
					{
						if (pBrowPic->x > 0)
						{
							break;
						}
						pBrowPic->xpan -= MOVESCOPE;
						if (abs(pBrowPic->xpan) >= abs(pBrowPic->x)) 
						{
							pBrowPic->xpan += MOVESCOPE;
							break;							
						}
						if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
						{
							ScrollAnimatedGIF((HGIFANIMATE)pBrowPic->hBmpData, MOVESCOPE, 0);
							//pBrowPic->xpan -= MOVESCOPE;
							//InvalidateRect(hWnd, NULL, TRUE);
						}
						else
						{
							//pBrowPic->xpan -= MOVESCOPE;							
							InvalidateRect(hWnd, NULL, TRUE);
						}
					}
					
				}
				break;
			case VK_RIGHT:
				{
					int	nCount, iPicType;
					HDC		hdc;
					//Parallel scrolling,previous picture is shown, all pictures can be browsed and seen 
					//until the first pinture in the list;
					if (sCurZoomState != BLARGE) 
					{
						if (!pBrowPic->hListBox) {
							break;
						}
						nCount = SendMessage(pBrowPic->hListBox, LB_GETCOUNT, 0, 0);
						pBrowPic->nIndex = SendMessage(pBrowPic->hListBox, LB_GETCURSEL, 0, 0);
						//if
						/*
						if (pBrowPic->nIndex==0)  {
							break;
						}
						*/
						iPicType = SendMessage(pBrowPic->hListBox, LB_GETITEMDATA, pBrowPic->nIndex+1, 0);
						
						if ((iPicType != PREBROW_FILETYPE_FOLDER)&&(iPicType != -1)&&(iPicType != PREBROW_FILETYPE_MEMEORY) )
						{
							//WaitWindowStateEx(hWnd, TRUE, IDS_OPENING, IDS_CAPTION, NULL, NULL);
							SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)LEFTARROWICON);
							//pBrowPic->clockdegree = 0;
							GetClientRect(hWnd, &rcClient);
							//free current picture resource
							if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
							{
								if (pBrowPic->hBmpData)
								{
									EndAnimatedGIF((HGIFANIMATE)pBrowPic->hBmpData);
								}
							}
							else
							{
								if (pBrowPic->hBmpData)
								{
									DeleteObject(pBrowPic->hBmpData);
								}			
							}

							pBrowPic->nIndex++;
							SendMessage(pBrowPic->hListBox, LB_SETCURSEL, pBrowPic->nIndex, 0);
							GetFileNameFromList(pBrowPic->hListBox, pBrowPic->pListBuf, NULL, pBrowPic->pCurFileName,
								pBrowPic->cFileFullName, &pBrowPic->itype);

							if (pBrowPic->itype == PREBROW_FILETYPE_GIF)
							{
								if (!IsGIFDynamic(pBrowPic->cFileFullName, TRUE, -1))
								{
									pBrowPic->bPan = TRUE;
								}
								else
									pBrowPic->bPan = FALSE;
								
							}
							else
								pBrowPic->bPan = FALSE;
							
							if (IsBigFileFromSD(pBrowPic->cFileFullName))
							{
								WaitWindowStateEx(hWnd, TRUE, IDS_OPENING, IDS_CAPTION, NULL, NULL);
							}

							iPicType = SendMessage(pBrowPic->hListBox, LB_GETITEMDATA, pBrowPic->nIndex+1, 0);
							if ((iPicType == PREBROW_FILETYPE_FOLDER)||(iPicType == -1)||(iPicType == PREBROW_FILETYPE_MEMEORY) )
							{
								SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
							}
							
							SetWindowText(pBrowPic->hFrame, pBrowPic->pCurFileName);
							GetImageDimensionFromFile(pBrowPic->cFileFullName, &imgSize);
							pBrowPic->dOriginalScaling = GetImageStrechSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, imgSize.cx, imgSize.cy,
								&pBrowPic->x, &pBrowPic->y, &pBrowPic->width, &pBrowPic->height);
							pBrowPic->dZoomedScaling = pBrowPic->dOriginalScaling;

							if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
							{
								pBrowPic->hBmpData = (HBITMAP)StartAnimatedGIFFromFileEx(hWnd, pBrowPic->cFileFullName, pBrowPic->x, pBrowPic->y, pBrowPic->width, pBrowPic->height, DM_NONE);
								//PaintAnimatedGIF()
								SetPaintBkFunc((HGIFANIMATE)pBrowPic->hBmpData, PIC_PaintBkFunc);
								hdc = GetDC(hWnd);
								PaintAnimatedGIF(hdc, (HGIFANIMATE)pBrowPic->hBmpData);
								ReleaseDC(hWnd, hdc);
							}
							else
							{
								//pBrowPic->hBmpData = CreateBitmapFromImageFile(NULL, pBrowPic->cFileFullName, NULL, NULL);
								hOrigBmp = CreateBitmapFromImageFile(NULL, pBrowPic->cFileFullName, NULL,NULL);
								pBrowPic->hBmpData = AdjustBmpToScreen(hOrigBmp, 0, 0, pBrowPic->width, pBrowPic->height, pBrowPic->cFileFullName);
								if (hOrigBmp)
								{
									DeleteObject(hOrigBmp);
									hOrigBmp = NULL;
								}
							}
							//pBrowPic->sCurZoomState = BNORMAL;
							pBrowPic->xpan = 0;
							pBrowPic->ypan = 0;
							if (IsBigFileFromSD(pBrowPic->cFileFullName))
							{
								WaitWindowStateEx(hWnd, FALSE, IDS_OPENING, IDS_CAPTION, NULL, NULL);
							}
							else
								InvalidateRect(hWnd, NULL, TRUE);
						}
						else
						{
							SendMessage(pBrowPic->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
						}
						//pBrowPic->xpan = 0;
						//pBrowPic->ypan = 0;
						
					}
					//Pan the picture 
					else
					{
						if (pBrowPic->x > 0)
						{
							break;
						}
						pBrowPic->xpan += MOVESCOPE;
						if (abs(pBrowPic->xpan) >= abs(pBrowPic->x)) 
						{
							pBrowPic->xpan -= MOVESCOPE;
							break;
							//pBrowPic->xpan = pBrowPic->x;

						}
						if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
						{
							ScrollAnimatedGIF((HGIFANIMATE)pBrowPic->hBmpData, -MOVESCOPE, 0);
							//pBrowPic->xpan += MOVESCOPE;
							//InvalidateRect(hWnd, NULL, TRUE);
						}
						else
						{
							//pBrowPic->xpan += MOVESCOPE;
							
							InvalidateRect(hWnd, NULL, TRUE);
						}
					}
					
				}
				break;

			case VK_UP:
				if (sCurZoomState == BLARGE) 
				{
					if (pBrowPic->y >= 0)
					{
						break;
					}
					pBrowPic->ypan -= MOVESCOPE;
					if (abs(pBrowPic->ypan) >= abs(pBrowPic->y)) 
					{
						pBrowPic->ypan += MOVESCOPE;
						break;
					}
					if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
					{
						ScrollAnimatedGIF((HGIFANIMATE)pBrowPic->hBmpData, 0, MOVESCOPE);
						//pBrowPic->ypan += MOVESCOPE;
						//InvalidateRect(hWnd, NULL, TRUE);
					}
					else
					{
						//pBrowPic->ypan += MOVESCOPE;
						
						InvalidateRect(hWnd, NULL, TRUE);
					}
				}
				break;

			case VK_DOWN:
				if (sCurZoomState == BLARGE) 
				{
					if (pBrowPic->y >= 0)
					{
						break;
					}
					pBrowPic->ypan += MOVESCOPE;
					if (abs(pBrowPic->ypan) >= abs(pBrowPic->y)) 
					{
						pBrowPic->ypan -= MOVESCOPE;
						break;
						//pBrowPic->ypan = pBrowPic->y;
					}
					if ((pBrowPic->itype == PREBROW_FILETYPE_GIF)&&(!pBrowPic->bPan))
					{
						ScrollAnimatedGIF((HGIFANIMATE)pBrowPic->hBmpData, 0, -MOVESCOPE);
						//pBrowPic->ypan -= MOVESCOPE;
						//InvalidateRect(hWnd, NULL, TRUE);
					}
					else
					{
						//pBrowPic->ypan -= MOVESCOPE;
						//					if (abs(pBrowPic->ypan) >= abs(pBrowPic->y)) {
						//						pBrowPic->ypan = pBrowPic->y;
						//					}
						InvalidateRect(hWnd, NULL, TRUE);
					}
				}
				break;

			case VK_F5:
				if (pBrowPic->bFull) 
				{
					lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				}
				else
				{
					PostMessage(pBrowPic->hFrame, wMsgCmd, wParam, lParam);
				}
				break;
				
			default:
				return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				
			}
			break;
		}
		break;		

	

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;


	}

	return lResult;
}

BOOL	ZoomPicture(HWND hWnd,HDC hdc, const char *cFileName, int nFileType,
							 int nHdcShowX, int nHdcShowY, int nHdcShowWidth, int nHdcShowHeight, 
							 int degree,int nBeginLeft, int nBeginTop, HBITMAP hBmp)
{
	HBITMAP hBitmap;
	BITMAP bm;
	COLORREF Color;
	BOOL	bTran;
//	int		midtmp;
/*	
	if (degree%2 == 1) {
		midtmp = nHdcShowX;
		nHdcShowX = nHdcShowY;
		nHdcShowY = midtmp;
		midtmp = nHdcShowWidth;
		nHdcShowWidth = nHdcShowHeight;
		nHdcShowHeight = midtmp;
	}
*/
	switch(nFileType)
	{
	case PREBROW_FILETYPE_GIF:
	case PREBROW_FILETYPE_BMP:
	case PREBROW_FILETYPE_JPG:
	case PREBROW_FILETYPE_JPEG:
	case PREBROW_FILETYPE_WBMP:
	case PREBROW_FILETYPE_PNG:
		if (hBmp)
		{
			hBitmap = hBmp;
		}
		else
		{
			hBitmap = CreateBitmapFromImageFile(hdc,cFileName,&Color,&bTran);
		}		
		if (!hBitmap) {
			return FALSE;
		}
		GetObject(hBitmap, sizeof(BITMAP), &bm);
		/*
		StretchBlt(
			hdc,
			nHdcShowX,
			nHdcShowY, 
			nHdcShowWidth, nHdcShowHeight, 
			(HDC)hBitmap,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY	);
			*/
		if(!SetAngleStretchBltBmp(hdc,
			nHdcShowX - nBeginLeft,
			nHdcShowY - nBeginTop, 
			nHdcShowWidth, nHdcShowHeight, 
			hBitmap, 0, 0, bm.bmWidth,bm.bmHeight,SRCCOPY, degree * 90))
		{
			if (!hBmp)
			{
				DeleteObject(hBitmap);
			}			
			return FALSE;
		}
		if (!hBmp)
		{
			DeleteObject(hBitmap);
		}
		
		break;
/*
	case PREBROW_FILETYPE_WBMP:
		StretchWBMP(hdc,cFileName,nHdcShowX,nHdcShowY,nHdcShowWidth,nHdcShowHeight,SRCCOPY);
		break;
*/	
	}

	return TRUE;
}

double	SetImageStrechSize ( int ScreenWidth, int ScreenHeight, int ImageWidth, int ImageHeight,
							 int *Stretchx, int *Stretchy, int *StretchWidth, int *StretchHeight, double StretchRate)
{
	*StretchWidth	=	(int) (ImageWidth * StretchRate);
	*StretchHeight	=	(int) (ImageHeight * StretchRate);
	*Stretchx		=	(int)((double)(ScreenWidth - *StretchWidth)/(double)2);
	*Stretchy		=	(int)((double)(ScreenHeight - *StretchHeight)/(double)2);
	return StretchRate;
}

static	void	ZoomImgData(HDC hdc, PVOID pImgData, DWORD dwImgSize, int x, int y, int width, int height)
{
	HBITMAP hBitmap;
	BITMAP bm;
	COLORREF Color;
	BOOL	bTran;
	
	
	hBitmap = CreateBitmapFromImageData(hdc, pImgData, dwImgSize, &Color,&bTran);
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	StretchBlt(
		hdc,
		x,
		y, 
		width, height, 
		(HDC)hBitmap,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY
		);
	DeleteObject(hBitmap);
}
void PIC_PaintBkFunc(HGIFANIMATE hGIFAnimate, BOOL End, int x, int y, HDC hdcMem)
{
    int                width, height;
    RECT            rect;

    width = GetDeviceCaps(hdcMem, HORZRES);
    height = GetDeviceCaps(hdcMem, VERTRES);

    rect.bottom = height;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;

    ClearRect(hdcMem, &rect, COLOR_WHITE);
}

HBITMAP	AdjustBmpToScreen(HBITMAP	hSrcBmp, int x, int y, int width, int height, PCSTR pFileName)
{
	HDC	hdcSrc,hdcDst;
	HBITMAP	hBitmap = NULL, hSrcFileBmp = NULL;
	BITMAP  bmp;
	//int		width,height;
	PSTR	pBakBmpName = NULL;
//	PSTR	p;
//	FILE	*fp;
	

	if (!hSrcBmp)
	{
		hSrcFileBmp = CreateBitmapFromImageFile(NULL, pFileName, NULL, NULL);
		if (!hSrcFileBmp)
		{
			return NULL;
		}
		
	}
	else
		hSrcFileBmp = hSrcBmp;

	GetObject(hSrcFileBmp, sizeof(BITMAP), (void*)&bmp);
	
	hdcDst = CreateCompatibleDC(NULL);
	hdcSrc = CreateCompatibleDC(NULL);
	hBitmap = CreateCompatibleBitmap(hdcDst, width, height);
	if (!hBitmap) 
	{
		return NULL;
	}
	SelectObject(hdcSrc, hSrcFileBmp);
	SelectObject(hdcDst, hBitmap);	
	//StretchBlt(hdcDst, 0, 0, width, height, hdcSrc, 0, 0, bmp.bmWidth, bmp.bmHeight, ROP_SRC);
	StretchBlt(hdcDst, 0, 0, width, height, hdcSrc, 0, 0, bmp.bmWidth, bmp.bmHeight, ROP_SRC);

	DeleteDC(hdcSrc);
	DeleteDC(hdcDst);
	return hBitmap;
}
