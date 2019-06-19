  /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 通用直接预览接口
 *
 * Purpose  : 提供一个通用的媒体直接预览接口
 *            
\**************************************************************************/

#include	"winpda.h"
#include	"string.h"
#include	"stdio.h"
#include	"stdlib.h"
//#include	"fapi.h"
#include	"sys/types.h"
#include	"sys/stat.h"
#include	"fcntl.h"
#include	"unistd.h" 
#include	"plx_pdaex.h"
#include	"hpimage.h"
#include	"str_public.h"
#include	"str_plx.h"
#include	"PreBrow.h"
#include	"PreBrowHead.h"
#include    "setting.h"
#include "mullang.h"

/*@**#---2005-04-21 15:08:38 (mxlin)---#**@

#define IDS_PREBROW		    GetString(STR_PREBROW_PREBROW)	//"预览"
#define SH_IDS_OK			GetString(STR_WINDOW_OK)
#define SH_IDS_BACK		    GetString(STR_WINDOW_BACK)
#define SH_IDS_RINGSTART	GetString(STR_PREBROW_BEEP)		//"播放"
#define SH_IDS_RINGEND		GetString(STR_PREBROW_STOP)		//"停止" 
*/
#define IDS_PREBROW		    ML("Browse")	//"预览"
#define SH_IDS_OK			ML("OK")
#define SH_IDS_BACK		    ML("Back")
#define SH_IDS_RINGSTART	ML("Play")		//"播放"
#define SH_IDS_RINGEND		ML("Stop")		//"停止" 


#define SH_ID_OK                   WM_USER+1000
#define SH_ID_EXIT                 WM_USER+1001

#define	WM_MMSTOP				(WM_USER + 1)
#define	WM_MMWRITE				(WM_USER + 2)
#define WM_MMGETDATA			(WM_USER + 3)

static	SetupHS_PlayMusic	hs;

//static  char   cfname[PREBROW_MAXFILENAMELEN];
static  char   cfname[PREBROW_MAXFULLNAMELEN];
static	BYTE    itype;

static	BYTE   bMusicType;


//图片直接预览

static LRESULT CALLBACK PicturesPreBrowseWndProc(HWND hWnd,UINT wMsgCmd,
												 WPARAM wParam,LPARAM lParam);


#define  IDC_SH_EDIT   101
static LRESULT CALLBACK TextPreBrowseWndProc(HWND hWnd,UINT wMsgCmd,
												 WPARAM wParam,LPARAM lParam);



static LRESULT CALLBACK MusicPreBrowseWndProc(HWND hWnd,UINT wMsgCmd,
												 WPARAM wParam,LPARAM lParam);

static	void	fnPaintBkFunc(HGIFANIMATE hGIFAnimate, int x, int y, HDC hdcMem);
/**************************************************************
*铃声停止的回调函数，被国电提供的函数调用
**************************************************************/
//static	void CALLBACK WinStopRing( BOOL bRingIsEnd, DWORD);

static  HWND   hWndMusicPreBrow;			/*铃声预览窗口*/


static  BOOL   bMusicPreBrowseWndClass;
static  BOOL   bPicturesPreBrowseWndClass;
static  BOOL   bTextPreBrowseWndClass;

static	HBRUSH	hWhiteBrush;


HINSTANCE APP_PreviewDirect(HWND hwnd, LPCTSTR lpOperation, LPCTSTR lpFile, 
                LPCTSTR lpFileType, LPCTSTR lpDirectory, INT nShowCmd)
{
    if(lpFile == NULL)
		return (HINSTANCE)FALSE;

	//memset(cfname, 0x00, PREBROW_MAXFILENAMELEN);
	memset(cfname, 0x00, PREBROW_MAXFULLNAMELEN);
	strcpy(cfname, lpFile);

	if ( ( strlen(lpFileType) == 0 ) || lpFileType == NULL)
	{
		itype = GetFileTypeByName(lpFile, TRUE);
	}
	else
		itype = GetFileTypeByName (lpFileType, FALSE);

	switch (itype)
	{	
		
	case PRBROW_FILETYPE_TEXT:
		return (HINSTANCE)APP_PreDirectTxt(hwnd, lpFile);
		
	case PREBROW_FILETYPE_MMF:
		//return (HINSTANCE)APP_PreDirectRing(hwnd, lpFile, 3);
		break;
	case PREBROW_FILETYPE_MID:
		//return (HINSTANCE)APP_PreDirectRing(hwnd, lpFile, 1);
		break;

	case PREBROW_FILETYPE_WAV:
		//return (HINSTANCE)APP_PreDirectRing(hwnd, lpFile, 2);
//		PlaySound(lpFile, NULL, 1);
		break;	
	case PREBROW_FILETYPE_AMR:
		//return (HINSTANCE)APP_PreDirectRing(hwnd, lpFile, 4);		
//		PlaySound(lpFile, NULL, 1);
		break;

	case PREBROW_FILETYPE_BMP:
	case PREBROW_FILETYPE_JPG:
	case PREBROW_FILETYPE_JPEG:
	case PREBROW_FILETYPE_GIF:
	case PREBROW_FILETYPE_WBMP:
		//return (HINSTANCE)APP_PreDirectPic(hwnd, lpFile, itype);	
		//BrowsePicture(NULL, hwnd, NULL, NULL, lpFile, NULL, FALSE, -1);
		break;

	default:
		return (HINSTANCE)FALSE;		
	}
	return (HINSTANCE)TRUE;
}


BOOL APP_PreDirectPic(HWND hWnd, LPCTSTR pFileName, int FileType)
{
	static  HWND   hWndPicturesPreBrow;
	DWORD	dStyle;


    WNDCLASS  wc;	

	//memset(cfname, 0x00, PREBROW_MAXFILENAMELEN);
	memset(cfname, 0x00, PREBROW_MAXFULLNAMELEN);
	strcpy(cfname, pFileName);
	itype = FileType;

	dStyle = WS_BORDER|WS_VISIBLE|PWS_STATICBAR|WS_CAPTION;//|WS_VSCROLL|WS_HSCROLL;
	if ( itype == PREBROW_FILETYPE_GIF)
	{
		dStyle = WS_BORDER|WS_VISIBLE|PWS_STATICBAR|WS_CAPTION;//|WS_VSCROLL|WS_HSCROLL;
	}

	if (!bPicturesPreBrowseWndClass)
	{
		wc.style            = NULL;
		wc.lpfnWndProc      = PicturesPreBrowseWndProc;
		wc.cbClsExtra       = 0;
		wc.cbWndExtra       = NULL;
		wc.hInstance        = NULL;
		wc.hIcon            = NULL;
		wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground    = NULL;
		wc.lpszMenuName     = NULL;
		wc.lpszClassName    = "SH_PicturesPreBrowseWndClass";
		
		if (!RegisterClass(&wc))
			return FALSE;
		
		bPicturesPreBrowseWndClass = TRUE;
	}
	
	hWndPicturesPreBrow = CreateWindow(
		"SH_PicturesPreBrowseWndClass", 
		IDS_PREBROW, 
		dStyle,//WS_VSCROLL,
		PLX_WIN_POSITION,
		hWnd, 
		NULL,
		NULL, 
		NULL
		);
	
	if (!IsWindow(hWndPicturesPreBrow))
	{
		return FALSE;
	}
	
	ShowWindow(hWndPicturesPreBrow,SW_SHOW);
	UpdateWindow(hWndPicturesPreBrow);
    
	return TRUE;
}


static LRESULT CALLBACK PicturesPreBrowseWndProc(HWND hWnd,UINT wMsgCmd,
												 WPARAM wParam,LPARAM lParam)
{
	LRESULT     lResult;
	HDC		    hdc;
	//static char cFilename[PREBROW_MAXFILENAMELEN];
	static char cFilename[PREBROW_MAXFULLNAMELEN];
	static int  iType;
	static RECT rcClient;
	
	static	HDC hdcMem;
	static	PAINTSTRUCT	ps;	

	/********************************************************/
	/*	Gif图片显示的相关变量								*/
	/********************************************************/

	static	HGIFANIMATE	hGif = NULL;				/*Gif图片句柄				*/
	static	SCROLLINFO	SiVert, SiHorz;				/*分别控制垂直和水平滚动	*/
	static	int			nGifx, nGify;				/*Gif图片的显示起始位置		*/
	static	int			nGifHeight, nGifWidth;		/*Gif图片的高度和宽度		*/
	static	int			nPageWidth, nPageHeight;	/*翻一个页面的高度和宽度	*/			
	static	int			nWndHeight, nWndWidth;		/*显示区域的高度和宽度		*/	
	static	char		szError[200];
	static	BOOL		bError = FALSE;
	static	COLORREF	OldColor;



	lResult = TRUE;

	switch(wMsgCmd)
	{
	case WM_CREATE:
		strcpy(cFilename, cfname);
		iType = itype;
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(SH_ID_EXIT,0), (LPARAM)SH_IDS_BACK);
		GetClientRect(hWnd,&rcClient);

		hWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
		hGif = NULL;


		/*如果是动态Gif文件，则初始化Gif文件相关变量，以便滚动使用*/
		if	(iType == PREBROW_FILETYPE_GIF)
		{	
			HBITMAP     hBitmap;  			
			BITMAP      bm;
			int			Stretchx, Stretchy, StretchWidth, StretchHeight;
			BOOL		bGet = TRUE;
			SIZE		Size;

			nWndHeight	=	rcClient.bottom - rcClient.top;
			nWndWidth	=	rcClient.right	- rcClient.left;

			
			//((( 判断是否为合法文件
			
			bGet	=	GetImageDimensionFromFile(cFilename, &Size);
			if (!bGet)
			{
				bError = TRUE;
				strcpy ( szError , ML("Invalid File") );
			}
					 
			else if ( (Size.cx > PREBROW_IMAGELIMIT_WIDTH) || (Size.cy > PREBROW_IMAGELIMIT_HEIGHT) ) 
			{
				//bError = TRUE;
				strcpy ( szError , IDS_TOOBIG );
			}
			
			
			//))) 判断是否为合法文件
		
			
			if ( bGet )
			{
				////以下得到Gif文件的宽度和高度,存储在结构bm中
				hBitmap = CreateBitmapFromImageFile(NULL, cFilename, NULL, NULL);
				GetObject(hBitmap, sizeof(BITMAP), &bm);
				DeleteObject(hBitmap);
				
				Stretchx = 0;
				Stretchy = 0;
				
				if ( bm.bmWidth < nWndWidth && bm.bmHeight < nWndHeight)
				{
					GetImageStrechSize (nWndWidth, nWndHeight, bm.bmWidth, bm.bmHeight,
						&Stretchx, &Stretchy, &StretchWidth, &StretchHeight);				
				}
				else
				{
					if ( bm.bmWidth < nWndWidth )
					{
						GetImageStrechSize (nWndWidth, nWndHeight, bm.bmWidth, 0,
							&Stretchx, &Stretchy, &StretchWidth, &StretchHeight);				
						Stretchy = 0;
					}
					if ( bm.bmHeight < nWndHeight)
					{
						GetImageStrechSize (nWndWidth, nWndHeight, 0, bm.bmHeight,
							&Stretchx, &Stretchy, &StretchWidth, &StretchHeight);				
						Stretchx = 0;
					}
				}				
				
				
				hGif = StartAnimatedGIFFromFile(hWnd,cFilename, Stretchx, Stretchy, DM_NONE);
				SetPaintBkFunc(hGif,  (PAINTBKFUNC)fnPaintBkFunc);
				
				
				//((( 纵向滚动条初始化
				
				if ( nWndHeight > bm.bmHeight )
				{
					EnableScrollBar (hWnd, SB_VERT, ESB_DISABLE_BOTH );					
					
					memset(&SiVert,0,sizeof(SCROLLINFO));
					SiVert.cbSize	=	sizeof(SCROLLINFO);			
					SiVert.fMask	=	SIF_POS | SIF_PAGE |SIF_DISABLENOSCROLL |SIF_RANGE; 
					SiVert.nPage	=	2;				
					SiVert.nPos		=	nGify;	
					SiVert.nMin		=	0;
					SiVert.nMax		=	1;
					
					SetScrollInfo (hWnd, SB_VERT, &SiVert, TRUE);				

				}
				else
				{			
					nPageHeight		=	nWndHeight ;
					nGify			=	0;
					nGifHeight		=	bm.bmHeight;
					
					memset(&SiVert,0,sizeof(SCROLLINFO));
					SiVert.cbSize	=	sizeof(SCROLLINFO);			
					SiVert.fMask	=	SIF_POS | SIF_PAGE |SIF_DISABLENOSCROLL |SIF_RANGE; 
					SiVert.nPage	=	nPageHeight;				
					SiVert.nPos		=	nGify;	
					SiVert.nMin		=	0;
					SiVert.nMax		=	nGifHeight;
					
					SetScrollInfo (hWnd, SB_VERT, &SiVert, TRUE);				
				}
				
				//))) 纵向滚动条初始化
				
				//((( 横向滚动条初始化
				
				if ( nWndWidth > bm.bmWidth )
				{
					EnableScrollBar (hWnd, SB_HORZ, ESB_DISABLE_BOTH );			
										
					memset(&SiHorz,0,sizeof(SCROLLINFO));
					SiHorz.cbSize	=	sizeof(SCROLLINFO);
					SiHorz.fMask	=	SIF_POS | SIF_PAGE |SIF_DISABLENOSCROLL |SIF_RANGE; 
					SiHorz.nPage	=	2;
					SiHorz.nPos		=	nGifx;		
					SiHorz.nMin		=	0;
					SiHorz.nMax		=	1;
					
					SetScrollInfo (hWnd, SB_HORZ, &SiHorz, TRUE);

				}
				else
				{
					nPageWidth		=	nWndWidth ;
					nGifx			=	0;
					nGifWidth		=	bm.bmWidth;
					
					memset(&SiHorz,0,sizeof(SCROLLINFO));
					SiHorz.cbSize	=	sizeof(SCROLLINFO);
					SiHorz.fMask	=	SIF_POS | SIF_PAGE |SIF_DISABLENOSCROLL |SIF_RANGE; 
					SiHorz.nPage	=	(int) nPageWidth;
					SiHorz.nPos		=	nGifx;		
					SiHorz.nMin		=	0;
					SiHorz.nMax		=	nGifWidth;
					
					SetScrollInfo (hWnd, SB_HORZ, &SiHorz, TRUE);
				}
			
				//))) 横向滚动条初始化
				
			}
			else
			{
				EnableScrollBar (hWnd, SB_BOTH, ESB_DISABLE_BOTH );		
			}
		}
		


		break;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case SH_ID_EXIT:
				PostMessage(hWnd,WM_CLOSE,0,0);
				break;
			}
		}
		break;

	case WM_PAINT:

		hdc=BeginPaint(hWnd,&ps);

		GetClientRect(hWnd, &rcClient);
		hdcMem = CreateMemoryDC(rcClient.right, rcClient.bottom);
		SetClipRect(hdcMem, &ps.rcPaint);		
		FillRect ( hdcMem, &rcClient, hWhiteBrush );

		if ( bError )	/*显示动态Gif图片*/
		{		
			OldColor = SetBkColor	( hdc, RGB(255,255,255));
			DrawText(hdcMem, szError, -1, &rcClient ,DT_VCENTER| DT_CENTER |DT_CLEAR);
			SetBkColor (hdc, OldColor);				
		}	
		else
		{
			if ( hGif != NULL )
				PaintAnimatedGIF(hdcMem, hGif);					
			else
			{
				PictureBrowPaint(hWnd, hdcMem, cFilename, iType,
					rcClient.left, rcClient.top, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top);
			}
		}
			
			
		BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hdcMem, 0, 0, ROP_SRC);
		DeleteDC(hdcMem);
		EndPaint(hWnd,&ps);

		break;

	case WM_CLOSE:
		DestroyWindow (hWnd);
		UnregisterClass ("SH_PicturesPreBrowseWndClass", NULL);
		bPicturesPreBrowseWndClass = FALSE;		
			

	case WM_DESTROY:
		if ( hGif != NULL )
		{
			EndAnimatedGIF(hGif);
			DeleteObject((HGDIOBJ)hWhiteBrush);
		}
		bError = FALSE;
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;

		case WM_VSCROLL:		
		{	
			int	opr, move;
			move	=	0;
			opr		=	(int) LOWORD(wParam);
			switch ( opr )
			{
			case SB_LINEUP:
				if ( nGify > 0 )
				{
					move	= -1;					
				}
				break;
				
			case SB_LINEDOWN:
				if ( nGify < nGifHeight - nWndHeight )					
				{
					move	=	1;					
				}	
				break;
			case SB_PAGEUP:
				{
					if ( ( nGify ) > nPageHeight)
					{						
						move = nPageHeight * (-1);
					}
					else if ( nGify > 0)
					{						
						move = nGify * ( -1 );
					}
					break;
				}				

			case SB_PAGEDOWN:
				{
					if ( ( nGifHeight - nWndHeight - nGify ) > nPageHeight)
					{						
						move	=	nPageHeight ;
					}
					else if (( nGifHeight - nWndHeight - nGify ) > 0  )
					{
						move = (nGifHeight - nWndHeight - nGify) ;
					}
					break;
				}
			case SB_THUMBPOSITION:
				{
					int nPos;
					nPos = (short int) HIWORD(wParam);
					move = nPos - SiVert.nPos ;					
					break;
				}

				break;
			}
			

			nGify	= nGify + move;
			SiVert.nPos = nGify;	

			SetScrollInfo (hWnd, SB_VERT, &SiVert, TRUE);
			//ScrollAnimatedGIF(hGif, 0, move * (-1));
			ScrollAnimatedGIF(hGif, 0, move * (-5));
			
			InvalidateRect ( hWnd, NULL, TRUE);
			UpdateWindow (hWnd );
			
			break;
		}
		case WM_HSCROLL:		
		{	
			int	opr, move;
			opr		=	(int) LOWORD(wParam);
			move	=	0;
			switch ( opr )
			{
			case SB_LINELEFT:
				if ( nGifx > 0 )
				{
					move = -1;										
				}
				break;
				
			case SB_LINERIGHT:
				if ( nGifx < nGifWidth - nWndWidth )					
				{
					move = 1;					
				}	
				break;
			
			case SB_PAGELEFT:
				if ( nGifx > nPageWidth )
				{
					move = nPageWidth * (-1);
				}
				else if ( nGifx > 0 )
				{
					move = nGifx * (-1);
				}

				break;

			case SB_PAGERIGHT:
				if ( ( nGifWidth - nGifx - nWndWidth) > nPageWidth )
				{
					move =	nPageWidth;
				}
				else if ( (nGifWidth - nGifx - nWndWidth) > 0 )
				{
					move = nGifWidth - nGifx - nWndWidth;
				}				
				break;

			case SB_THUMBPOSITION:
				{
					int nPos;
					nPos = (short int) HIWORD(wParam);
					move = nPos - SiHorz.nPos ;					
					break;
				}
			}

			nGifx	=	nGifx + move;				
			SiHorz.nPos	=	nGifx;

			SetScrollInfo (hWnd, SB_HORZ, &SiHorz, TRUE);
			ScrollAnimatedGIF(hGif, move * (-1), 0);		
			InvalidateRect ( hWnd, NULL, TRUE);
			UpdateWindow (hWnd );
			
			break;
		}

		
	//	(((		针对有键盘的手机
	
		case WM_KEYDOWN : 
			{
				switch(LOWORD(wParam))
				{
				case VK_UP:
					SendMessage (hWnd, WM_VSCROLL, MAKEWPARAM ( SB_LINEUP, SiVert.nPos ), NULL);
					break;

				case VK_DOWN:
					SendMessage (hWnd, WM_VSCROLL, MAKEWPARAM ( SB_LINEDOWN, SiVert.nPos ), NULL);
					break;

				case VK_LEFT:
					SendMessage (hWnd, WM_HSCROLL, MAKEWPARAM ( SB_LINELEFT, SiHorz.nPos ), NULL);
					break;

				case VK_RIGHT:
					SendMessage (hWnd, WM_HSCROLL, MAKEWPARAM ( SB_LINERIGHT, SiHorz.nPos ), NULL);
					break;

				case VK_2 :
					SendMessage (hWnd, WM_VSCROLL, MAKEWPARAM ( SB_PAGEUP, SiVert.nPos ), NULL);
					break;
				case VK_8 :
					SendMessage (hWnd, WM_VSCROLL, MAKEWPARAM ( SB_PAGEDOWN, SiVert.nPos ), NULL);
					break;
				case VK_4 :
					SendMessage (hWnd, WM_HSCROLL, MAKEWPARAM ( SB_PAGELEFT, SiHorz.nPos ), NULL);
					break;
				case VK_6 :
					SendMessage (hWnd, WM_HSCROLL, MAKEWPARAM ( SB_PAGERIGHT, SiHorz.nPos ), NULL);
					break;

				case VK_F2:

					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;

				case VK_F10:

					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;

				default:
					return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
					
				}
				break;
			}

		//)))  针对有键盘的手机

	}

	return lResult;
}



BOOL APP_PreDirectTxt(HWND hWnd, LPCTSTR pFileName)
{
	static  HWND   hWndTextPreBrow;
	

    WNDCLASS  wc;	

	memset(cfname, 0x00, PREBROW_MAXFILENAMELEN);
	strcpy(cfname, pFileName);


	if (!bTextPreBrowseWndClass)
	{
		wc.style            = NULL;
		wc.lpfnWndProc      = TextPreBrowseWndProc;
		wc.cbClsExtra       = 0;
		wc.cbWndExtra       = NULL;
		wc.hInstance        = NULL;
		wc.hIcon            = NULL;
		wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName     = NULL;
		wc.lpszClassName    = "SH_TextPreBrowseWndClass";
		
		if (!RegisterClass(&wc))
			return FALSE;
		
		bTextPreBrowseWndClass = TRUE;
	}
	
	hWndTextPreBrow = CreateWindow(
		"SH_TextPreBrowseWndClass", 
		IDS_PREBROW, 
		WS_BORDER|WS_VISIBLE|PWS_STATICBAR|WS_CAPTION,
		PLX_WIN_POSITION,
		hWnd, 
		NULL,
		NULL, 
		NULL
		);
	
	if (!IsWindow(hWndTextPreBrow))
	{
		return FALSE;
	}
	
		ShowWindow(hWndTextPreBrow,SW_SHOW);
		UpdateWindow(hWndTextPreBrow);
    
	return TRUE;
}


static LRESULT CALLBACK TextPreBrowseWndProc(HWND hWnd,UINT wMsgCmd,
												 WPARAM wParam,LPARAM lParam)
{
	static char cFilename[PREBROW_MAXFILENAMELEN];
	//static int  iType;
	static HWND SH_hEdit;
	static RECT rcClient;
	static char* FileData;
	
	LRESULT     lResult;
	HDC		    hdc;
	int		    hFile;
	int         nFileSize;
	struct stat *buf =NULL;

	lResult = TRUE;
	
	switch(wMsgCmd)
	{
	case WM_CREATE:
		strcpy(cFilename, cfname);
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(SH_ID_EXIT,0), (LPARAM)SH_IDS_BACK);
		GetClientRect(hWnd,&rcClient);
		SH_hEdit= CreateWindow(
            "EDIT", 
            "", 
            WS_CHILD|WS_VISIBLE| ES_MULTILINE|ES_UNDERLINE|ES_READONLY|WS_VSCROLL, 
            0, 
            0, 
            rcClient.right-rcClient.left, 
            rcClient.bottom-rcClient.top, 
            hWnd, 
            (HMENU)IDC_SH_EDIT,
            NULL, 
            NULL
            );
		
		
		if (-1 == (hFile = open( cFilename, O_RDONLY)))
		{
			return FALSE;
		}

		buf = malloc(sizeof(struct stat));

		if(buf == NULL)
		{
			close(hFile);
			return NULL;
		}
		
		memset(buf, 0, sizeof(struct stat));
		stat(cFilename, buf);
		
		nFileSize = buf->st_size;
		
		free(buf);
		buf = NULL;
		
		
		if (-1 == (nFileSize))
		{
			close(hFile);
			return FALSE;
		}

		if ( 0 == nFileSize )
		{
			SendMessage(SH_hEdit, WM_SETTEXT, 0, (LPARAM)(""));
			close(hFile);
			return TRUE;
		}
		
		if ( nFileSize > 36*1024)
			nFileSize = 36*1024;
		
		if (NULL == (FileData = malloc(nFileSize+1)))
		{
			close(hFile);
			return FALSE;
		}
		memset(FileData,0,nFileSize+1);
		
		if (!read(hFile,FileData,nFileSize))
		{
			if (FileData)
			{
				free(FileData);
				FileData = NULL;
			}
			close(hFile);
			return FALSE;
		}
		close(hFile);

		SendMessage(SH_hEdit, EM_LIMITTEXT, nFileSize, NULL);
		SetWindowText(SH_hEdit, FileData);
	    
		break;

	case WM_ACTIVATE:
		if (WA_ACTIVE == wParam)
		{
			SetFocus(SH_hEdit);
		}
		break;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case SH_ID_EXIT:
				PostMessage(hWnd,WM_CLOSE,0,0);
				break;
			}
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd,NULL);
		EndPaint(hWnd,NULL);
		break;

	case WM_KEYDOWN:

	    switch(wParam)
		{
			
		case VK_F2:

            PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case VK_F10:

            PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		default:
			PostMessage(GetParent(hWnd),wMsgCmd,wParam,lParam);
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow (hWnd);
		UnregisterClass ("SH_TextPreBrowseWndClass", NULL);
		bTextPreBrowseWndClass = FALSE;		

	case WM_DESTROY:
		if (FileData)
		{
			free(FileData);
			FileData = NULL;
		}
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;
}

BOOL APP_PreDirectRing(HWND hWnd, LPCTSTR pFileName, BYTE bType)
{
	
    WNDCLASS  wc;	
   
	memset(cfname, 0x00, PREBROW_MAXFILENAMELEN);
	strcpy(cfname, pFileName);

	bMusicType = bType;
	if (!bMusicPreBrowseWndClass)
	{
		wc.style            = NULL;
		wc.lpfnWndProc      = MusicPreBrowseWndProc;
		wc.cbClsExtra       = 0;
		wc.cbWndExtra       = NULL;
		wc.hInstance        = NULL;
		wc.hIcon            = NULL;
		wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground    = NULL;
		wc.lpszMenuName     = NULL;
		wc.lpszClassName    = "SH_MusicPreBrowseWndClass";
		
		if (!RegisterClass(&wc))
			return FALSE;
		
		bMusicPreBrowseWndClass = TRUE;
	}
	
	hWndMusicPreBrow = CreateWindow(
		"SH_MusicPreBrowseWndClass", 
		IDS_PREBROW, 
		WS_BORDER|WS_VISIBLE|PWS_STATICBAR|WS_CAPTION,
		PLX_WIN_POSITION,
		hWnd, 
		NULL,
		NULL, 
		NULL
		);
	
	if (!IsWindow(hWndMusicPreBrow))
	{
		return FALSE;
	}
	
		ShowWindow(hWndMusicPreBrow,SW_SHOW);
		UpdateWindow(hWndMusicPreBrow);

	return TRUE;
}


static LRESULT CALLBACK MusicPreBrowseWndProc(HWND hWnd,UINT wMsgCmd,
												 WPARAM wParam,LPARAM lParam)
{
	static char cFilename[PREBROW_MAXFILENAMELEN];
	//static int  iType;
	static BOOL bPlay;  
	LRESULT     lResult;
	HDC		    hdc;
	static		HGIFANIMATE  hMusicGif;    //动态音乐gif

	static	HDC hdcMem;
	static	PAINTSTRUCT	ps;
	static	RECT rcClient;
	
	

	lResult = TRUE;
	
	switch(wMsgCmd)
	{
	case WM_CREATE:
		strcpy(cFilename, cfname);
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, 
			        MAKEWPARAM(SH_ID_OK,1), (LPARAM)SH_IDS_RINGEND);

		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, 
					MAKEWPARAM(SH_ID_EXIT,0), (LPARAM)SH_IDS_BACK);

		bPlay = TRUE;
#ifndef EMULATION
        //SEF_Play_RingEX(cFilename, bMusicType, 0);//PPPPPPPPP
		//SEF_PlayRingFromFile(cFilename, bMusicType, 0, WinStopRing);
#endif
		

		hWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
		hMusicGif = StartAnimatedGIFFromFile(hWnd,"/rom/public/prebrow/sh_music.gif", 0, 0,DM_NONE);
		SetPaintBkFunc(hMusicGif, (PAINTBKFUNC) fnPaintBkFunc);

		hs.hWnd			= hWnd;
		hs.iWM_MMWRITE	= WM_MMWRITE;
		hs.iWM_MMSTOP	= WM_MMSTOP;
		hs.iWM_MMGETDATA = WM_MMGETDATA;
		hs.iVolume		= 0;
		hs.iType		= bMusicType;//0;
		hs.uiringselect = RING_OTHER;
		hs.pFileName	= cfname;	
		//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#ifdef _EMULATE_
		
#else
		SETUP_PlayMusic(&hs);
#endif
		//SETUP_PlayMusic(&hs);

		break;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam)) 
			{
			case SH_ID_EXIT:
				PostMessage(hWnd,WM_CLOSE,0,0);
				if(bPlay)
				{
					//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
					//Setup_EndPlayMusic(RING_OTHER);
					bPlay = FALSE;
				}
				break;
			case SH_ID_OK:
				if(bPlay)
				{
					bPlay = FALSE;
					//hs.pFileName	= cfname;
					//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
					//Setup_EndPlayMusic(RING_OTHER);
					SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, 
			        MAKEWPARAM(SH_ID_OK,1), (LPARAM)SH_IDS_RINGSTART);
				}
				else
				{
#ifndef EMULATION
					//SEF_Play_RingEX(cFilename, bMusicType, 0);//PPPPPPPPP
					//SEF_PlayRingFromFile(cFilename, bMusicType, 0, WinStopRing);
#endif
                    bPlay = TRUE;
					hs.pFileName	= cfname;
					//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
					//SETUP_PlayMusic(&hs);
#ifdef _EMULATE_

#else
				SETUP_PlayMusic(&hs);
#endif
					
					SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, 
			        MAKEWPARAM(SH_ID_OK,1), (LPARAM)SH_IDS_RINGEND);
				}
				break;
			}

		}
		break;

	case WM_PAINT:
		hdc=BeginPaint(hWnd,&ps);

		GetClientRect(hWnd, &rcClient);
		hdcMem = CreateMemoryDC(rcClient.right, rcClient.bottom);
		SetClipRect(hdcMem, &ps.rcPaint);

		FillRect ( hdcMem, &rcClient, hWhiteBrush );
		if ( hMusicGif != NULL)	/*显示动态Gif图片*/
			PaintAnimatedGIF(hdcMem, hMusicGif);		
		
		BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hdcMem, 0, 0, ROP_SRC);
		DeleteDC(hdcMem);
		EndPaint(hWnd,&ps);

		break;
	
	case WM_KEYDOWN:

	    switch(wParam)
		{

		case VK_F10:

            PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		default:
			PostMessage(GetParent(hWnd),wMsgCmd,wParam,lParam);
			break;
		}
		break;

	case WM_MMSTOP:
		{
			if(wParam == PLAY_OVER || wParam ==PLAY_BREAK)
			{
				//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
				//Setup_EndPlayMusic(RING_OTHER);
			}
			bPlay = FALSE;
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, 
				MAKEWPARAM(SH_ID_OK,1), (LPARAM)SH_IDS_RINGSTART);
		}
		break;

	case WM_MMWRITE:
		{
			//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			//Setup_WriteMusicData((LPWAVEHDR)lParam);
		}
		break;

	case WM_MMGETDATA:
		{
			//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			//Setup_GetMusicData((LPWAVEHDR)lParam);
		}
		break;

	case WM_CLOSE:
		EndAnimatedGIF(hMusicGif);
		DeleteObject((HGDIOBJ)hWhiteBrush);
		DestroyWindow (hWnd);
		UnregisterClass ("SH_MusicPreBrowseWndClass", NULL);
		bMusicPreBrowseWndClass = FALSE;

	case WM_DESTROY:
		if(bPlay)
		{
			
			bPlay = FALSE;
		}
		break;
		
	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;
}

/**************************************************************
*铃声停止的回调函数，被国电提供的函数调用
**************************************************************/
/*
static	void CALLBACK WinStopRing(BOOL bRingIsEnd, DWORD dword)
{
	if (IsWindow(hWndMusicPreBrow))
		SendMessage(hWndMusicPreBrow, PWM_CREATECAPTIONBUTTON, 
			        MAKEWPARAM(SH_ID_OK,1), (LPARAM)SH_IDS_RINGSTART);

	return;
	
}
 */

static	void	fnPaintBkFunc(HGIFANIMATE hGIFAnimate, int x, int y, HDC hdcMem)
{

	int nHdcWidth,nHdcHeight;
	RECT rect;

	nHdcWidth	=	GetDeviceCaps (hdcMem, HORZRES);
	nHdcHeight	=	GetDeviceCaps (hdcMem, VERTRES);
	
//	BitBlt(hdcMem, x - 0 , y - 0, nHdcWidth , nHdcHeight, NULL, x, y, ROP_SRC);

	rect.left = 0;
	rect.top = 0;
	rect.right = nHdcWidth;
	rect.bottom = nHdcHeight;
	FillRect ( hdcMem, &rect, hWhiteBrush );
	
	return;
}
