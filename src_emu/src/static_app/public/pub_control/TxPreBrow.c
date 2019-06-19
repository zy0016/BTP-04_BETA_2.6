/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : public
 *
 * Purpose  : 文本预览
 *            
\**************************************************************************/

#include	<PreBrow.h>
#include	"PreBrowHead.h"

/************************************************************************/
/* 消息参数宏                                                           */
/************************************************************************/

#define		IDC_PRETX_LIST		2000
#define		IDM_PRETX_BACK		2001//GetString(STR_WINDOW_BACK)//2001
#define		IDM_PRETX_GET		2005//GetString(STR_PRETX_GET)//2005
#define		IDM_PRETX_DETAIL	2006//GetString(STR_PRETX_DETAIL)//2006


/************************************************************************/
/* 控件位置宏                                                           */
/************************************************************************/

#define		PRETX_LIST_X			PREBROW_WND_X
#define		PRETX_LIST_Y			PREBROW_WND_Y
#define		PRETX_LIST_WIDTH		PREBROW_WND_WIDTH
#define		PRETX_LIST_HEIGHT		(PREBROW_WND_HEIGHT * 2 ) / 5

#define		PRETX_DRAWTEXT_X		PREBROW_WND_X
#define		PRETX_DRAWTEXT_Y		PREBROW_WND_Y + PRETX_LIST_HEIGHT + 1
#define		PRETX_DRAWTEXT_WIDTH	PREBROW_WND_WIDTH
#define		PRETX_DRAWTEXT_HEIGHT	(PREBROW_WND_HEIGHT * 3 ) / 5


/*************************************************\
	字符串定义宏
\*************************************************/
#define		IDS_PRETX_CAPTION	GetString(STR_PRETX_CAPTION)
#define		IDS_PRETX_BACK		GetString(STR_WINDOW_BACK)
#define		IDS_PRETX_GET		GetString(STR_PRETX_GET)

/*************************************************\
	全局变量宏
\*************************************************/
// #define		DEFAULT_TXFILEDIR	"FLASH2:/pim/notepad/"
#define		DEFAULT_TXFILEDIR	"/mnt/flash/pim/notepad/"
#define		PRETX_MAX_FILESIZE	1024*36//1024*2


/*************************************************\
	全局变量
\*************************************************/

static	BOOL	bTextsBrowseWndClass;
static	HWND	hWndTextsBrow;
static	UINT	uTextReturn;

static	HWND	hWndListBox;
static	HWND	hEditText;

static	char	cFileName[PREBROW_MAXFILENAMELEN];
static	int		nFileType;

static	LISTBUF	ListBuffer;
static	DWORD	dwMaskStyle;

/*************************************************\
	函数声明
\*************************************************/
static LRESULT CALLBACK TextsBrowseWndProc (HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);	
static int	AddTextsToList (HWND hList);
static BOOL	TextBrowPaint (HDC hdc);
static BOOL	SetBrowEditText (HWND hEditText,char* FileName);



/*********************************************************************\
* Function	   EnterTextsBrow
* Purpose      进入文本预浏览界面
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

BOOL	APP_PreviewText(HWND hFrame, HWND hWnd, UINT iRetMsg, DWORD dMask)
{
    WNDCLASS wc;

	dwMaskStyle = dMask;
    
	if (!bTextsBrowseWndClass)
	{
		wc.style            = NULL;
		wc.lpfnWndProc      = TextsBrowseWndProc;
		wc.cbClsExtra       = 0;
		wc.cbWndExtra       = NULL;
		wc.hInstance        = NULL;
		wc.hIcon            = NULL;
		wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName     = NULL;
		wc.lpszClassName    = "TextsBrowseWndClass";
		
		if (!RegisterClass(&wc))
			return FALSE;
		
		bTextsBrowseWndClass = TRUE;
	}

	if (IsWindow(hWndTextsBrow))
	{
		ShowWindow(hWndTextsBrow,SW_SHOW);
		UpdateWindow(hWndTextsBrow);
	}
	else
	{
		hWndTextsBrow = CreateWindow(
			"TextsBrowseWndClass", 
			IDS_PRETX_CAPTION, 
			WS_BORDER|WS_VISIBLE|PWS_STATICBAR|WS_CAPTION,
			PLX_WIN_POSITION,
			hWnd, 
			NULL,
			NULL, 
			NULL
			);

		if (!IsWindow(hWndTextsBrow))
		{
			return FALSE;
		}
		uTextReturn = iRetMsg;

		ShowWindow(hWndTextsBrow,SW_SHOW);
		UpdateWindow(hWndTextsBrow);
	}
    
	return TRUE;
}

/*********************************************************************\
* Function	   TextsBrowseWndProc
* Purpose      浏览文本窗口过程
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK TextsBrowseWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
	LRESULT lResult;
	HDC		hdc;
	int		num;
	char szTemp[PREBROW_MAXFILENAMELEN];

	lResult = TRUE;

	switch(wMsgCmd)
	{
	case WM_CREATE:
		

		hWndListBox = CreateWindow("LISTBOX","",
			WS_CHILD |WS_VISIBLE|WS_BORDER|LBS_HASSTRINGS|WS_VSCROLL|LBS_BITMAP,
			PRETX_LIST_X,PRETX_LIST_Y,PRETX_LIST_WIDTH,PRETX_LIST_HEIGHT,
			hWnd,(HMENU)IDC_PRETX_LIST,NULL,NULL);

		PREBROW_InitListBuf (&ListBuffer);

		num = AddTextsToList(hWndListBox);		
		GetFileNameFromList(hWndListBox, &ListBuffer, DEFAULT_TXFILEDIR, szTemp, cFileName, &nFileType);


			hEditText =	CreateWindow("Edit","",
			WS_CHILD |WS_BORDER	|ES_WANTRETURN
			|ES_MULTILINE|ES_READONLY|ES_UNDERLINE|ES_LEFT,
			PRETX_DRAWTEXT_X, 
			PRETX_DRAWTEXT_Y,
			PRETX_DRAWTEXT_WIDTH, 
			PRETX_DRAWTEXT_HEIGHT, 
			hWnd, NULL , NULL, NULL);				
			
			if ( hEditText )
			{
				ShowWindow(hEditText,SW_SHOW);
				UpdateWindow(hEditText);
			}	
		SetBrowEditText(hEditText,cFileName);

		if ( !( dwMaskStyle & PREBROW_MASKGET) && num !=0 )
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_PRETX_GET,1), (LPARAM)IDS_PRETX_GET);

		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_PRETX_BACK,0), (LPARAM)IDS_PRETX_BACK);

		if ( num != 0 )
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_PRETX_DETAIL,2), (LPARAM)GetString(STR_PRETX_DETAIL));
		
		break;

	case WM_ACTIVATE:
		if (WA_ACTIVE == wParam)
		{
			SetFocus(hWndListBox);
		}
		break;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDM_PRETX_GET:
				PostMessage( hWnd, WM_CLOSE, 0, 0);
				SendMessage(GetWindow(hWnd,GW_OWNER),uTextReturn,MAKEWPARAM(strlen(cFileName),RTN_TEXT),(LPARAM)cFileName);				

				break;

			case IDM_PRETX_BACK:
				PostMessage( hWnd, WM_CLOSE, 0, 0);
				SendMessage(GetWindow(hWnd,GW_OWNER),uTextReturn,MAKEWPARAM(0,RTN_NOSELECT),0);				
				break;

			case IDM_PRETX_DETAIL:
				APP_PreDirectTxt(hWnd, cFileName);
				break;


			case IDC_PRETX_LIST:
				{
					switch(HIWORD(wParam))
					{
					case LBN_SELCHANGE:
						GetFileNameFromList(hWndListBox, &ListBuffer, DEFAULT_TXFILEDIR, szTemp, cFileName, &nFileType);
						SetBrowEditText(hEditText,cFileName);
						break;

					case LBN_DBLCLK:
						if ( !( dwMaskStyle & PREBROW_MASKGET) )
							PostMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_PRETX_GET,1),NULL);
						break;
					}
				}
				break;
			}
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd,NULL);
		TextBrowPaint(hdc);
		EndPaint(hWnd,NULL);
		break;

	case WM_CLOSE:
		DestroyWindow (hWnd);
		UnregisterClass ("TextsBrowseWndClass", NULL);
		bTextsBrowseWndClass = FALSE;

		break;


	case WM_DESTROY:
		PREBROW_FreeListBuf (&ListBuffer);
		hWndTextsBrow = NULL;
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;
}

/*********************************************************************\
* Function	   AddTextsToList
* Purpose      把文本文件读入列表
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int	AddTextsToList(HWND hList)
{
	int n;

	
	FindFileAndAddToList(hList, DEFAULT_TXFILEDIR, PRBROW_FILEUNIVERSE_TEXT, PRBROW_FILETYPE_TEXT, NULL, &ListBuffer);

	
	n = SendMessage(hList,LB_GETCOUNT,NULL,NULL);
	if (LB_ERR != n)
	{
		SendMessage(hList,LB_SETCURSEL,(WPARAM)0,NULL);
	}

	return n;
}

/*********************************************************************\
* Function	   TextBrowPaint
* Purpose      浏览文本界面PAINT过程
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	TextBrowPaint(HDC hdc)
{
	return TRUE;
}

/*********************************************************************\
* Function	   SetBrowEditText
* Purpose      设显示文字
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	SetBrowEditText(HWND hEditText,char* FileName)
{
	char* FileData;
	int  hFile;
	int nFileSize;
	struct stat *buf = NULL;

	if (!IsWindow(hEditText))
	{
		return FALSE;
	}

	if (-1 == (hFile = open( FileName, O_RDONLY)))
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
    stat(FileName, buf);

    nFileSize = buf->st_size;

    free(buf);
    buf = NULL;
	
	if (-1 == nFileSize)
	{
		close(hFile);
		return FALSE;
	}

	if ( 0 == nFileSize )
	{
		SendMessage(hEditText, WM_SETTEXT, 0, (LPARAM)(""));
		close(hFile);
		return TRUE;
	}

	if ( nFileSize > PRETX_MAX_FILESIZE)
		nFileSize = PRETX_MAX_FILESIZE;


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
/*
	SetGameHelpText(hEditText,(LPARAM)FileData);
*/
	SendMessage(hEditText, WM_SETTEXT, 0, (LPARAM)(""));
	SendMessage(hEditText, WM_SETTEXT, 0, (LPARAM)(FileData));
	


	if (FileData)
	{
		free(FileData);
		FileData = NULL;
	}
	
	return TRUE;
}
