     /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setup
 *
 * Purpose  : 短语预览
 *            
\**************************************************************************/

//#include	"SMPreBrow.h"
#include	"PreBrow.h"
#include	"pubapp.h"
#include	"PreBrowHead.h"
//#include    "hpime.h"
#include	"imesys.h"
#include "mullang.h"

#define		PRESM_PRESMMAXLEN	160			/*	最大短语长度		*/


typedef struct tagSMTEMPLATESAVESTRU {
	int	Len;
	char	cSmTemplate[PRESM_PRESMMAXLEN+1];
}SMTPLSTRUC, *PSMTPLSTRUC;


#define	TMPICON	"/rom/message/unibox/msg_template_22x16.bmp"
#define	TMPSICON	"/rom/message/unibox/msg_templates_22x16.bmp"
#define		NEWTMPICON	"/rom/message/unibox/msgnewtemplate_22x16.bmp"
#define RADIO_OFF   "ROM:ime/radiooff.bmp"
/************************************************************************/
/* 消息参数宏                                                           */
/************************************************************************/

#define		IDC_PRESM_LIST		100
#define		IDC_EDITSM			101
#define		IDM_PRESM_BACK		WM_USER+101
#define		IDM_PRESM_NEW		WM_USER+102
#define		IDM_PRESM_EDIT		WM_USER+103
#define		IDM_PRESM_DELETE	WM_USER+104
#define		IDM_PRESM_GET		WM_USER+105

/************************************************************************/
/* 字符串定义宏                                                         */
/************************************************************************/

//@**#---2005-04-21 15:49:52 (mxlin)---#**@

#define		IDS_PRESM_CAPTION	ML("Templates")
#define		IDS_PRESM_BACK		ML("Back")
#define		IDS_PRESM_NEW		ML("New template")
#define		IDS_PRESM_EDIT		ML("Edit")
#define		IDS_PRESM_DELETE	ML("Delete")
#define		IDS_PRESM_GET		ML("Insert")
#define		IDS_SAVE			ML("Save")
#define		IDS_CANCEL			ML("Cancel")
#define		IDS_CLEAR			ML("Clear")
#define		IDS_PRESM_DELTMP	ML("Delete template?")
#define		IDS_YES				ML("Yes")
#define		IDS_NO				ML("No")


/************************************************************************/
/* 全局参数宏                                                           */
/************************************************************************/
#define		PRESMFILENAME		"/mnt/flash/pubcontrol/presmfile.ini"



/************************************************************************/
/* define window data structure                                         */                 
/************************************************************************/
typedef struct tagPRESMTMP {
	HWND	hFrameWnd;
	HWND	hRecMsgWnd;
	HWND	hList;
	HBITMAP	hRBoffbmp;
	HMENU	hMainMenu;
	UINT	returnmsg;
	char	pCaption[51];
}SMTEMPLATE, *PSMTEMPLATE;

static	HWND	hEditSm = NULL;
static	char	szTemplate[PRESM_PRESMMAXLEN+1] = "";
static	BOOL	bNewTemplate ;
static	HBITMAP	hBitmapIcon = NULL;
static	HDC			MemoryDCIcon = NULL;

static	HBITMAP	hNewTmpBmp	= NULL;
static	HBITMAP	hTemplate	=	NULL;
//static	HBITMAP	hRBoffbmp	=	NULL;
/************************************************************************/
/* 函数声明                                                             */
/************************************************************************/
static	BOOL	RegisterPresmWndCls();
static	BOOL	RegisterEditSmWndCls();
static	LRESULT	CALLBACK	PRESM_SmPreBrowProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
static	LRESULT	CALLBACK	EditSmProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
static	LRESULT	CALLBACK	EditTemplateProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
static	BOOL	InitTemplateListFromFile(HWND	hList, BOOL b, HBITMAP hbmp);
static	BOOL	SaveTemplateToFile(HWND hList);
static	BOOL	CallEditTemplateWnd(PSMTEMPLATE pSmTmplate, PCSTR szText, BOOL bNew);
static void SMTEMPLATE_SetIndicator(HWND hWnd,int nFrag, int nRemain);
/************************************************************************/
/* 菜单模板                                                             */
/************************************************************************/
MENUITEMTEMPLATE PRESM_MainMenuItem[]=
{
	{MF_STRING, IDM_PRESM_EDIT,		NULL,	NULL},
	{MF_STRING, IDM_PRESM_DELETE,	NULL,	NULL},
	{MF_END,0,NULL,NULL}
};

const MENUTEMPLATE PRESM_MainMenuTemplate=
{
	0,
	PRESM_MainMenuItem
};

static	BOOL	RegisterPresmWndCls()
{
	WNDCLASS wc; 
	
	wc.style            = CS_OWNDC;
	wc.lpfnWndProc      = PRESM_SmPreBrowProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = sizeof(SMTEMPLATE);
	wc.hInstance        = NULL;
	wc.hIcon            = NULL;
	wc.hCursor          = NULL;//LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground    = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = "SmPreBrowWndClass";
	
	if (!RegisterClass(&wc))
		return FALSE;
	return	TRUE;
}
/*********************************************************************
* Function	SmPreBrow   
* Purpose   外部调用接口   
* Params	父窗口句柄，父窗口的接收消息，要保存的短语，要保存的短语的长度
* Return	过向父窗口发送消息returnmsg，Wparam为0 ，returnmsg中lParam存放短语字符串指针
* Remarks	dwMask: 1---select templates; 0---edit;  2---insert template,InsertedTm is the template content;
			MsLen is length;
**********************************************************************/
BOOL	APP_PreviewPhrase(HWND	hFrame, HWND hWnd, UINT returnmsg, PCSTR szCaption)
{
	RECT	rf;
	SMTEMPLATE	smtmp;
	HWND	hWndMain;

	//init window data
	memset(&smtmp, 0, sizeof(SMTEMPLATE));
	smtmp.hFrameWnd = hFrame;
	smtmp.hRecMsgWnd = hWnd;
	smtmp.returnmsg = returnmsg;
	if (szCaption) {
		strcpy(smtmp.pCaption, szCaption);
	}
	
	

	
	//creat main window
	RegisterPresmWndCls();
	GetClientRect(hFrame, &rf);
	if (hFrame) {
		
		hWndMain = CreateWindow(
			"SmPreBrowWndClass", 
			NULL, 
			WS_VISIBLE|WS_CHILD,
			rf.left,
			rf.top,
			rf.right - rf.left,
			rf.bottom - rf.top,
			hFrame, 
			NULL,
			NULL, 
			(PVOID)&smtmp);
		if (szCaption) {
			SetWindowText(smtmp.hFrameWnd, szCaption);
		}
		else
			SetWindowText(smtmp.hFrameWnd, ML("Select template"));
		
		SendMessage(smtmp.hFrameWnd, PWM_SETBUTTONTEXT, 1, NULL);
		
		SendMessage(smtmp.hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP,1), NULL);	
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP,0), NULL);	
	}
	else
	{
		hWndMain = CreateWindow(
			"SmPreBrowWndClass", 
			szCaption, 
			WS_VISIBLE|WS_CHILD|WS_CAPTION|PWS_STATICBAR,
			rf.left,
			rf.top,
			rf.right - rf.left,
			rf.bottom - rf.top,
			hFrame, 
			NULL,
			NULL, 
			(PVOID)&smtmp);
		//SendMessage(pSmTmp->hFrameWnd, PWM_SETBUTTONTEXT, 1, NULL);
		SendMessage(hWndMain, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hWndMain, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_PRESM_BACK,0), (LPARAM)IDS_CANCEL);	
	}
	if (!IsWindow(hWndMain))
	{
		return FALSE;
	}		
	
	
	ShowWindow(hFrame,SW_SHOW);
	UpdateWindow(hFrame);
	SetFocus(hWndMain);
	
	
	return	TRUE;
}

/*********************************************************************
* Function	PRESM_MainWndProc
* Purpose   主窗口函数
* ParaSM	窗口句柄
* Return	
* Remarks	
**********************************************************************/
static	LRESULT	CALLBACK	PRESM_SmPreBrowProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
	
	LRESULT	lResult;
//	int    i;
	RECT	rc;
//	int		nIndex;
	PSMTEMPLATE	pSmTmp;
	PCREATESTRUCT	pcs;

	lResult	=	TRUE;	
	pSmTmp = GetUserData(hWnd);
	switch(wMsgCmd)
	{
	case WM_CREATE:
		{
			HWND	hWndList;
			pcs	= (PCREATESTRUCT)lParam;
			memcpy(pSmTmp, (PSMTEMPLATE)pcs->lpCreateParams, sizeof(SMTEMPLATE));
			
			GetClientRect(hWnd, &rc);
			hWndList	=	CreateWindow("LISTBOX","",
				WS_CHILD |WS_VISIBLE|LBS_HASSTRINGS|WS_VSCROLL|LBS_BITMAP,
				rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
				hWnd, NULL, NULL, NULL);
			if ( !hWndList )
				return FALSE;
			pSmTmp->hList =	hWndList;			
			
			pSmTmp->hRBoffbmp	=	LoadImage(NULL, RADIO_OFF, IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE);
			//pSmTmp->hRBoffbmp	= hRBoffbmp;
			InitTemplateListFromFile(hWndList, FALSE, pSmTmp->hRBoffbmp);
			SendMessage(pSmTmp->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			if (SendMessage(hWndList, LB_GETCOUNT, 0, 0) > 0) {
				SendMessage(pSmTmp->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				SendMessage(hWndList, LB_SETCURSEL, 0, 0);
			}
			else
				SendMessage(pSmTmp->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");			
		}
		break;

	case WM_SETFOCUS:
		if (SendMessage(pSmTmp->hList, LB_GETCOUNT, 0, 0) > 0) {
			SetFocus(pSmTmp->hList);
		}		
		break;

	case PWM_SHOWWINDOW:
		//nIndex = SendMessage(pSmTmp->hList, LB_GETCURSEL, 0, 0);
		if (pSmTmp->pCaption[0] != 0) {
			SetWindowText(pSmTmp->hFrameWnd, pSmTmp->pCaption);
		}
		else
			SetWindowText(pSmTmp->hFrameWnd, ML("Select template"));
		SendMessage(pSmTmp->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		//SendMessage(pSmTmp->hList, LB_RESETCONTENT, 0, 0);
		//InitTemplateListFromFile(pSmTmp->hList, FALSE, pSmTmp->hRBoffbmp);
		if (SendMessage(pSmTmp->hList, LB_GETCOUNT, 0, 0) > 0)
		{
			//SendMessage(pSmTmp->hList, LB_SETCURSEL, nIndex, 0);
			SendMessage(pSmTmp->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		}
		else
			SendMessage(pSmTmp->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");	
		SendMessage(pSmTmp->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);	
		SetFocus(hWnd);
		break;
		
	case WM_DESTROY:
		if (pSmTmp->hRBoffbmp)
		{
			DeleteObject(pSmTmp->hRBoffbmp);
		}
		UnregisterClass("SmPreBrowWndClass", NULL);
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;	

	case WM_PAINT: 
		{
			HDC		hdc;
			int		oldbm;
			if (SendMessage(pSmTmp->hList, LB_GETCOUNT, 0, 0) <= 0)
			{
				ShowWindow(pSmTmp->hList, SW_HIDE);
				GetClientRect(hWnd, &rc);
				hdc	=	BeginPaint( hWnd, NULL);
				oldbm = SetBkMode(hdc, TRANSPARENT);
				DrawText(hdc, ML("No templates"), -1, &rc, DT_VCENTER|DT_CENTER);
				SetBkMode(hdc, oldbm);
				EndPaint(hWnd, NULL);			
			}
			else
			{
				ShowWindow(pSmTmp->hList, SW_SHOW);
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			}
		}
		break;


	case WM_KEYDOWN:

	    switch(wParam)
		{
		case VK_F10:
			SendMessage(pSmTmp->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case VK_F5:
			{
//				char	*Content;
				int		i,	Len;
				if (SendMessage(pSmTmp->hList, LB_GETCOUNT, 0, 0) <= 0)
				{
					break;
				}
				i = SendMessage(pSmTmp->hList, LB_GETCURSEL, 0, 0);
				if (i == LB_ERR) 
				{
					PostMessage(pSmTmp->hRecMsgWnd, pSmTmp->returnmsg, 0, 0);
					PostMessage(pSmTmp->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				}
				Len = SendMessage(pSmTmp->hList, LB_GETTEXTLEN, i, 0);
//				Content = malloc(Len+1);
//				if (!Content)
//				{
//					return FALSE;
//				}
				SendMessage(pSmTmp->hList, LB_GETTEXT, i, (LPARAM)szTemplate);				
				PostMessage(pSmTmp->hRecMsgWnd, pSmTmp->returnmsg, MAKEWPARAM(Len, RTN_QTEXT), (LPARAM)szTemplate);
				PostMessage(pSmTmp->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				//free(Content);
			}
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;	
}

static BOOL InitTemplateListFromFile(HWND	hList, BOOL bEdit, HBITMAP hbmp)
{
	FILE	*fp;
//	char	buf[256];
	SMTPLSTRUC	smps;
	int		nData,nIndex;
	if (bEdit) {
		SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)IDS_PRESM_NEW);
		SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)hNewTmpBmp);
	}
	if((fp = fopen(PRESMFILENAME, "r")) == NULL)
	{
		if((fp = fopen(PRESMFILENAME, "w+"))!= NULL)
			fclose(fp);
		return FALSE;
	}
	else
	{
		fseek(fp, 0, SEEK_SET);
		fseek(fp, 4, SEEK_SET);		
		while(!feof(fp))
		{
			memset(&smps, 0, sizeof(SMTPLSTRUC));
			nData = fread(&smps, sizeof(SMTPLSTRUC), 1, fp);
			if (nData < 1) {
				break;
			}
			nIndex = SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)smps.cSmTemplate);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(nIndex, 0)), (LPARAM)hbmp);
		}
	}
	fclose(fp);
	return	TRUE;
}

static	BOOL	RegisterEditSmWndCls()
{
	WNDCLASS wc; 
	
	wc.style            = NULL;
	wc.lpfnWndProc      = EditSmProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = sizeof(SMTEMPLATE);
	wc.hInstance        = NULL;
	wc.hIcon            = NULL;
	wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground    = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = "EditSmWndClass";
	
	if (!RegisterClass(&wc))
		return FALSE;
	return	TRUE;
}


BOOL	APP_EditPhrase(HWND	hFrame)
{
	RECT	rf;
	SMTEMPLATE smtmp;
	HWND	hMainWnd;

	memset(&smtmp, 0, sizeof(SMTEMPLATE));
	smtmp.hFrameWnd = hFrame;

	if(!RegisterEditSmWndCls())
		return FALSE;

	smtmp.hMainMenu	=	LoadMenuIndirect((PMENUTEMPLATE)&PRESM_MainMenuTemplate);
	ModifyMenu(smtmp.hMainMenu, IDM_PRESM_EDIT,	MF_STRING,	IDM_PRESM_EDIT,		IDS_PRESM_EDIT);
	ModifyMenu(smtmp.hMainMenu, IDM_PRESM_DELETE,	MF_STRING,	IDM_PRESM_DELETE,	IDS_PRESM_DELETE);
	GetClientRect(hFrame, &rf);
	hMainWnd = CreateWindow(
		"EditSmWndClass", 
		NULL, 
		WS_VISIBLE|WS_CHILD,
		rf.left,
		rf.top,
		rf.right - rf.left,
		rf.bottom - rf.top,
		hFrame, 
		NULL,
		NULL, 
		(PVOID)&smtmp);
	if (!hMainWnd) {
		UnregisterClass("EditSmWndClass", NULL);
		return FALSE;
	}
	SetWindowText(smtmp.hFrameWnd, IDS_PRESM_CAPTION);
	PDASetMenu(hFrame, smtmp.hMainMenu);
	SendMessage(smtmp.hFrameWnd, PWM_SETBUTTONTEXT, 1, NULL);
	SendMessage(smtmp.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	SendMessage(smtmp.hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_PRESM_BACK);	
	ShowWindow(hFrame, SW_SHOW);
	SetFocus(hMainWnd);
	return	TRUE;
}

static	LRESULT	CALLBACK	EditSmProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	LRESULT	lResult;
	PSMTEMPLATE pSmTmplate;
	PCREATESTRUCT pcs;
	char		strTemplate[PRESM_PRESMMAXLEN];
	int		nSelIndex;

	lResult = TRUE;
	pSmTmplate = GetUserData(hWnd);
	switch(uMsg) 
	{
	case WM_CREATE:
		{	
			RECT	rc;
			HWND	hWndList;
			pcs	= (PCREATESTRUCT)lParam;
			memcpy(pSmTmplate, (PSMTEMPLATE)pcs->lpCreateParams, sizeof(SMTEMPLATE));
			
			GetClientRect(hWnd, &rc);
			hWndList	=	CreateWindow("LISTBOX","",
				WS_CHILD |WS_VISIBLE|LBS_HASSTRINGS|WS_VSCROLL|LBS_BITMAP,
				rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
				hWnd, (HMENU)IDC_PRESM_LIST, NULL, NULL);
			if ( !hWndList )
				return FALSE;
			pSmTmplate->hList =	hWndList;
			if (!hNewTmpBmp)
			{
				hNewTmpBmp =	LoadImage(NULL, NEWTMPICON, IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE);
			}
			if (!hTemplate)
			{
				hTemplate =	LoadImage(NULL, TMPICON, IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE);
			}
			InitTemplateListFromFile(hWndList, TRUE, hTemplate);	
			SendMessage(hWndList, LB_SETCURSEL, 0, 0);
			SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 2 , (LPARAM)"Select");
		}
		break;

	case PWM_SHOWWINDOW:
		SetFocus(pSmTmplate->hList);
		SetWindowText(pSmTmplate->hFrameWnd, IDS_PRESM_CAPTION);
		PDASetMenu(pSmTmplate->hFrameWnd, pSmTmplate->hMainMenu);
		//SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 1, NULL);
		SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_PRESM_BACK);	
		if (SendMessage(pSmTmplate->hList, LB_GETCURSEL, 0, 0) == 0) 
		{
			SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 2 , (LPARAM)"Select");
			SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 1 , (LPARAM)"");
		}
		if (SendMessage(pSmTmplate->hList, LB_GETCURSEL, 0, 0) > 0)
		{
			SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 1 , (LPARAM)IDS_PRESM_EDIT);
			SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 2 , (LPARAM)"Options");
		}
		break;

	case WM_SETFOCUS:
		SetFocus(pSmTmplate->hList);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case IDC_PRESM_LIST:
			switch(HIWORD(wParam)) 
			{
			case LBN_SELCHANGE:
				if (SendMessage(pSmTmplate->hList, LB_GETCURSEL, 0, 0) == 0) 
				{
					SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 2 , (LPARAM)"Select");
					SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 1 , (LPARAM)"");
					break;
				}
				if (SendMessage(pSmTmplate->hList, LB_GETCURSEL, 0, 0) > 0)
				{
					SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 2 , (LPARAM)"Options");
					SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 1 , (LPARAM)IDS_PRESM_EDIT);
				}
				break;
			default:
				lResult = PDADefWindowProc(hWnd, uMsg, wParam, lParam);
				break;
			}
			break;
			
		case IDM_PRESM_EDIT:
			nSelIndex = SendMessage(pSmTmplate->hList, LB_GETCURSEL, 0, 0);
			if ((nSelIndex == LB_ERR)||(nSelIndex == 0)) {
				break;
			}
			SendMessage(pSmTmplate->hList, LB_GETTEXT, nSelIndex, (LPARAM)strTemplate);
			CallEditTemplateWnd(pSmTmplate, strTemplate, FALSE);					
			break;

		case IDM_PRESM_DELETE:
			nSelIndex = SendMessage(pSmTmplate->hList, LB_GETCURSEL, 0, 0);
			if (nSelIndex == LB_ERR) {
				break;
			}
			if (PLXConfirmWin(pSmTmplate->hFrameWnd, hWnd, IDS_PRESM_DELTMP, Notify_Request, NULL, IDS_YES, IDS_NO)) 
			{
				SendMessage(pSmTmplate->hList, LB_DELETESTRING, nSelIndex, 0);
				SendMessage(pSmTmplate->hList, LB_SETCURSEL, nSelIndex-1, 0);
				if (nSelIndex == 1) {
					SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				}
				PLXTipsWin(pSmTmplate->hFrameWnd, hWnd, 0, ML("Deleted"), NULL, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			}
			
			break;

	
		default:
			lResult = PDADefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
		}		
		break;

	case WM_KEYDOWN:
		{
			switch(LOWORD(wParam)) 
			{
			case VK_RETURN:
				//nSelIndex = SendMessage(pSmTmplate->hList, LB_GETCURSEL, 0, 0);
				PostMessage(hWnd, WM_COMMAND, IDM_PRESM_EDIT, 0);
				break;

			case VK_F5:
				{
					if (SendMessage(pSmTmplate->hList, LB_GETCURSEL, 0, 0) == 0) 
					{
						CallEditTemplateWnd(pSmTmplate, "", TRUE);
						break;
					}
					if (SendMessage(pSmTmplate->hList, LB_GETCURSEL, 0, 0) > 0)
					{
						PostMessage(pSmTmplate->hFrameWnd, WM_KEYDOWN, wParam, lParam);
					}
				}
				break;
			case VK_F10:
				{
					SendMessage(pSmTmplate->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					DestroyWindow(hWnd);
				}
				break;
			}
		}
		break;

	case WM_DESTROY:
		if (hNewTmpBmp)
		{
			DeleteObject(hNewTmpBmp);
			hNewTmpBmp = NULL;
		}
		if (hTemplate)
		{
			DeleteObject(hTemplate);
			hTemplate = NULL;
		}
		SaveTemplateToFile(pSmTmplate->hList);
		UnregisterClass("EditSmWndClass", NULL);
		break;



	default:
		lResult = PDADefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}
	return lResult;
}
static	BOOL	SaveTemplateToFile(HWND hList)
{
	
	FILE	*fp;
	char	buf[256];
	SMTPLSTRUC	smps;
	int		nData, nCount, i;
	
	if((fp = fopen(PRESMFILENAME, "w+")) == NULL)
	{
		return FALSE;
	}
	else
	{
		nCount = SendMessage(hList, LB_GETCOUNT, 0, 0);
		nCount --;
		fseek(fp, 0, SEEK_SET);
		fwrite(&nCount, sizeof(nCount), 1, fp);
		for(i = 1; i<=nCount; i++)
		{
			memset(&smps, 0, sizeof(SMTPLSTRUC));
			memset(buf, 0, sizeof(buf));
			SendMessage(hList, LB_GETTEXT, i, (LPARAM)smps.cSmTemplate);
			smps.Len = strlen(smps.cSmTemplate);
			nData = fwrite(&smps, sizeof(SMTPLSTRUC), 1, fp);
			if (nData < 1) {
				ferror(fp);
				break;
			}
			//SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)smps.cSmTemplate);
		}
	}
	fclose(fp);
	return	TRUE;

}

static	BOOL	CallEditTemplateWnd(PSMTEMPLATE pSmTmplate, PCSTR szText, BOOL bNew)
{
	WNDCLASS	wc;
	RECT		rfClient;

	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = EditTemplateProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof(SMTEMPLATE);
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "EDITTEMPLATE";



	if (!RegisterClass(&wc)) {
		return FALSE;
	}
	bNewTemplate = bNew;
	GetClientRect(pSmTmplate->hFrameWnd, &rfClient);
	strcpy(szTemplate, szText);
	hEditSm	= CreateWindow("EDITTEMPLATE",NULL,
		WS_VISIBLE | WS_CHILD,
		//PLX_WIN_POSITION,
		rfClient.left,
		rfClient.top,
		rfClient.right - rfClient.left,
		rfClient.bottom - rfClient.top,
		pSmTmplate->hFrameWnd,NULL,NULL,(PVOID)pSmTmplate);
	if (!hEditSm)
	{
		szTemplate[0] = '\0';
		UnregisterClass("EDITTEMPLATE", NULL);
		return FALSE;
	}
	SetWindowText( pSmTmplate->hFrameWnd,IDS_PRESM_NEW);
	if (!bNew) {
		SendMessage(pSmTmplate->hFrameWnd,PWM_SETBUTTONTEXT,  1,(LPARAM)IDS_SAVE);
	}
	
	SendMessage(pSmTmplate->hFrameWnd,PWM_SETBUTTONTEXT, 0,(LPARAM)IDS_CANCEL);
	SendMessage(pSmTmplate->hFrameWnd,PWM_SETBUTTONTEXT, 2,(LPARAM)"");
	
	ShowWindow(pSmTmplate->hFrameWnd,SW_SHOW);
	UpdateWindow(pSmTmplate->hFrameWnd);
	SetFocus(hEditSm);
	return TRUE;	
}
static	LRESULT	CALLBACK	EditTemplateProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	LRESULT lResult;
	static	HWND	hRenameEdit = NULL;
	IMEEDIT	ie;
	RECT	rcClient;
	PSMTEMPLATE pSmTmplate;
	int		nSelIndex, nLen, nRemain;

	lResult = TRUE;
	pSmTmplate = GetUserData(hWnd);
	switch(uMsg) 
	{
	case WM_CREATE:
		{
			PCREATESTRUCT pcs;
			pcs	= (PCREATESTRUCT)lParam;
			memcpy(pSmTmplate, (PSMTEMPLATE)pcs->lpCreateParams, sizeof(SMTEMPLATE));

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
				WS_VISIBLE| WS_CHILD|ES_MULTILINE|WS_VSCROLL|WS_TABSTOP,
				rcClient.left,
				rcClient.top,
				rcClient.right - rcClient.left,
				rcClient.bottom - rcClient.top,
				hWnd,(HMENU)IDC_EDITSM,NULL,(PVOID)&ie);
			SendMessage(hRenameEdit,EM_LIMITTEXT,PRESM_PRESMMAXLEN,0);
			if(!bNewTemplate)
				SendMessage(hRenameEdit, WM_SETTEXT, 0, (LPARAM)szTemplate);
			nLen = GetWindowTextLength(hRenameEdit);
			nRemain = PRESM_PRESMMAXLEN - nLen;
			SMTEMPLATE_SetIndicator(hWnd, 1, nRemain);
			SendMessage(pSmTmplate->hFrameWnd, PWM_SETAPPICON, 
                MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)hBitmapIcon);
			SendMessage(hRenameEdit, EM_SETSEL, -1, -1);
		}
		break;

	case PWM_SHOWWINDOW:
		SetFocus(hWnd);
		SetWindowText( pSmTmplate->hFrameWnd,IDS_PRESM_NEW);
		nLen = GetWindowTextLength(hRenameEdit);
		if (nLen > 0) 
		{
			SendMessage(pSmTmplate->hFrameWnd,PWM_SETBUTTONTEXT,  1,(LPARAM)IDS_SAVE);
			SendMessage(pSmTmplate->hFrameWnd,PWM_SETBUTTONTEXT, 0,(LPARAM)IDS_CLEAR);
		}
		else
		{
			SendMessage(pSmTmplate->hFrameWnd,PWM_SETBUTTONTEXT,  1,(LPARAM)"");
			SendMessage(pSmTmplate->hFrameWnd,PWM_SETBUTTONTEXT, 0,(LPARAM)IDS_CANCEL);
		}
		
		//SendMessage(pSmTmplate->hFrameWnd,PWM_SETBUTTONTEXT, 0,(LPARAM)IDS_CANCEL);
		SendMessage(pSmTmplate->hFrameWnd,PWM_SETBUTTONTEXT, 2,(LPARAM)"");
		nLen = GetWindowTextLength(hRenameEdit);
		nRemain = PRESM_PRESMMAXLEN - nLen;
		SMTEMPLATE_SetIndicator(hWnd, 1, nRemain);
		break;

	case WM_SETFOCUS:
		SetFocus(hRenameEdit);
		break;

	case WM_SETLBTNTEXT:
		{
			BOOL	bEmpty;
			int		nId;
			bEmpty = (BOOL)HIWORD(wParam);
			nId	= LOWORD(wParam);
			if (nId == IDC_EDITSM)
			{
				if (bEmpty) {
					SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				}
				else
					SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
			}
			else
			{
				SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 1, lParam);				
			}

		}
		break;

	case WM_COMMAND:
		if (HIWORD(wParam) == EN_CHANGE) 
		{
			
			nLen = GetWindowTextLength(hRenameEdit);
			/*
			if (nLen > 0) {
				SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
			}
			else
				SendMessage(pSmTmplate->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				*/
			nRemain = PRESM_PRESMMAXLEN - nLen;
			SMTEMPLATE_SetIndicator(hWnd, 1, nRemain);
			SendMessage(pSmTmplate->hFrameWnd, PWM_SETAPPICON, 
                MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)hBitmapIcon);
		}
		else
			lResult = PDADefWindowProc(hWnd, uMsg, wParam, lParam);
		break;

		
	case WM_DESTROY:
		if (hBitmapIcon) {
			DeleteObject(hBitmapIcon);
		}
		if (MemoryDCIcon) {
			DeleteDC(MemoryDCIcon);
			MemoryDCIcon = NULL;
		}
		
		UnregisterClass("EDITTEMPLATE", NULL);
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam)) 
		{
		case VK_RETURN:
			{
				if (GetWindowTextLength(hRenameEdit) < 1) {
					break;
				}
				SendMessage(hRenameEdit, WM_GETTEXT,  PRESM_PRESMMAXLEN+1, (LPARAM)szTemplate);
				
				if (bNewTemplate) {
					nSelIndex = SendMessage(pSmTmplate->hList, LB_ADDSTRING, 0, (LPARAM)szTemplate);
					SendMessage(pSmTmplate->hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(nSelIndex, 0)), (LPARAM)hTemplate);
					SendMessage(pSmTmplate->hList, LB_SETCURSEL, nSelIndex, 0);
					SaveTemplateToFile(pSmTmplate->hList);
					SendMessage(pSmTmplate->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					DestroyWindow(hWnd);
				}
				else
				{
					nSelIndex = SendMessage(pSmTmplate->hList, LB_GETCURSEL, 0, 0);
					SendMessage(pSmTmplate->hList, LB_SETTEXT, nSelIndex, (LPARAM)szTemplate);
					SendMessage(pSmTmplate->hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(nSelIndex, 0)), (LPARAM)hTemplate);
					SaveTemplateToFile(pSmTmplate->hList);
					SendMessage(pSmTmplate->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					DestroyWindow(hWnd);
				}
			}
			break;

		case VK_F10:
			SendMessage(pSmTmplate->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			DestroyWindow(hWnd);
			break;	
		}
		break;
	default:
		lResult = PDADefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}
	return lResult;
}

int	GetTemplateNum()
{
	FILE	*fp;
	int		nData = 0;
	
	if((fp = fopen(PRESMFILENAME, "r")) == NULL)
	{
		if((fp = fopen(PRESMFILENAME, "w+"))!= NULL)
		{
			fwrite(&nData, sizeof(int), 1, fp);
			fclose(fp);
			return 0;
		}
		return 0;
	}
	else
	{
		fseek(fp, 0, SEEK_SET);		
		if(fread(&nData, sizeof(int), 1, fp) < 1)
			nData =0;		
	}
	fclose(fp);
	return	nData;
}
static void SMTEMPLATE_SetIndicator(HWND hWnd,int nFrag, int nRemain)
{
	HDC			hdc = NULL;
	
	HBITMAP		hBitmap = NULL;
	char        szFrag[16], szRemain[16];
    HFONT       hFont = NULL;
	RECT        rect;
	SIZE        sz1, sz2;
	int         nOldMode = 0;

	
	hdc = GetDC(hWnd);

	if (MemoryDCIcon == NULL)
		MemoryDCIcon = CreateCompatibleDC(hdc);

	GetFontHandle(&hFont,SMALL_FONT);
    SelectObject(MemoryDCIcon,hFont);

    sprintf(szFrag, "(%d)", nFrag);
    sprintf(szRemain, "%d", nRemain);

	GetTextExtentPoint(MemoryDCIcon, szFrag, -1, &sz1);
	GetTextExtentPoint(MemoryDCIcon, szRemain, -1, &sz2);

	SetRect(&rect, 0, 0, max(sz1.cx, sz2.cx), sz1.cy + sz2.cy - 4);
	
	if (hBitmapIcon != NULL)
		DeleteObject(hBitmapIcon);

    hBitmapIcon = CreateCompatibleBitmap(MemoryDCIcon, 
		rect.right, rect.bottom);

    SelectObject(MemoryDCIcon, hBitmapIcon);

	ClearRect(MemoryDCIcon, &rect, COLOR_TRANSBK);

	nOldMode = SetBkMode(MemoryDCIcon, TRANSPARENT);
//	TextOut(MemoryDCIcon, 0, 0, szFrag, -1);
	TextOut(MemoryDCIcon, 0, sz1.cy - 4, szRemain, -1);
	SetBkMode(MemoryDCIcon, nOldMode);

	ReleaseDC(hWnd, hdc);

}
