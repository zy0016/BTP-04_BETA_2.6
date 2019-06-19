/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : ¿Õ¼ä²é¿´
 *            
\**************************************************************************/
#define __MODULE__ "FAVORITE"
#include "favorite.h"
//#include "../../public/pub_control/Receivebox.h"
#include "unistd.h"
#include "plxdebug.h"


typedef	struct tagITEMDATA 
{
	int		nIndex;				//shortcur index;
	char	showName[41];		//display name;
	char	lpName[41];			//app name;

}ITEMDATA,* PITEMDATA;

typedef struct tagFEATUREDATA
{
	HWND	hParent;
	char	szFeatureAppName[41];
	char	**pNewFeature;
	UINT	uMsgCloseParent;
}FEATUREDATA, *PFEATUREDATA;

static	char *appName[]={
"Bluetooth",	//feature
"Calculator",
"Calendar",
"Clock",
"Contacts",
"Currency converter",
"GPS monitor",
"Help",
"Logs",
"Memory management",
//"Memory",
"Messaging",
"Navigation client",
"Network services",
//"Network",
"Notepad",
"Pictures",
//"PNS",

"Program manager",
//"Sound profiles",
"Settings",
"Sounds",
"Wap browser",
"Diamond",
"GoBang",
"Pao",
"Poker",
"Pushbox",
"DepthBomb",
""
};
static MENUITEMTEMPLATE FavoriteMenu[] =
{
	{ MF_STRING, IDC_OPEN, "Open", NULL},
	//{ MF_STRING, IDC_ADD, "Add", NULL },
	{ MF_STRING, IDC_RENAME, "Rename", NULL},
	{ MF_STRING, IDC_MOVE, "Move", NULL},
	{ MF_STRING, IDC_REMOVE, "Remove", NULL},
    { MF_END, 0, NULL, NULL }
};
//#endif

static const MENUTEMPLATE FavoriteMenuTemplate =
{
    0,
    FavoriteMenu
};
/*
static MENUITEMTEMPLATE FavoriteEmptyListMenu[] =
{
	{ MF_STRING, IDC_ADD, "Add", NULL },
    { MF_END, 0, NULL, NULL }
};
//#endif

static const MENUTEMPLATE FavoriteEmptyListMenuTemplate =
{
    0,
    FavoriteEmptyListMenu
};
*/
static	HMENU	hMenu = NULL;
//static	HMENU	hEmptyListMenu	=	NULL;
static	HWND	hFrameWnd = NULL;
static	HWND	hMainWnd = NULL;
static	HWND	hShortcurList = NULL;
static	HWND	hFeatureList	=	NULL;
static	HWND	hAddShortcurWnd = NULL;
static	HWND	hRenameWnd = NULL;
static	HINSTANCE	hInstance;

//static	PITEMDATA	pItemdata;
//static	PITEMDATA	pTailNode;


//static	PRECIPIENTLISTBUF	pListbuf;
static	HBITMAP		hRBonbmp;
static	HBITMAP		hRBoffbmp;
static	HBITMAP		hShortCurIon;
static	HBITMAP		hNewShortCurIon;
static	int		fp;
static	BOOL	bFileExist	= FALSE;
static	BOOL	bRegister	= FALSE;
static	BOOL	bEnaleAdd	= FALSE;
static	BOOL	bMove		= FALSE;
static	BOOL	bEmpty		= FALSE;
static	BOOL	bException	= FALSE;
static	int		iSelindex = 0;
//static	char	**pNewFeature	=	NULL;
//static	char	szFeatureAppName[41] = "";


static	LRESULT	Favorite_MainWndProc(HWND,UINT,WPARAM,LPARAM);
static	LRESULT	FavorAddShortCurProc(HWND,UINT,WPARAM,LPARAM);
static	LRESULT	Favor_RenameProc(HWND,UINT,WPARAM,LPARAM);
static	LRESULT	MyListProc(HWND,UINT,WPARAM,LPARAM);
static	LRESULT	AddFeatureProc(HWND,UINT,WPARAM,LPARAM);
static	WNDPROC	OldListBoxProc;

static	void    FavorList_AddString(HWND hList,char ** p,HBITMAP * hIconNormal,BOOL bAppend);
static	PITEMDATA	CreateAndAddNodeToChain(PCSTR, PCSTR, int);
static	PITEMDATA	FindCurNode(int	nIndex);
static	BOOL	Favor_AddShortCur(HWND hParent);
static	BOOL	Favor_RenameShorcur(HWND hWnd);
static	BOOL	Favor_AddFeature(HWND	hParent, char **feature, PCSTR pAppName, UINT uMsg);
static	BOOL	ChangeMoveStat(BOOL bMoveStat);
extern  BOOL	CallAppEntryEx(PSTR pAppName, WPARAM wparam, LPARAM lparam);

extern	BOOL	findstr(char **p, char *ptr);



DWORD Favor_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
	WNDCLASS	wc;
	DWORD	dRet;
	dRet = TRUE;
	switch(nCode) 
	{
	case APP_INIT:
		hInstance = (HINSTANCE)pInstance;
		break;
	case APP_ACTIVE:
		if (!bRegister) 
		{
			wc.style         = CS_OWNDC;
			wc.lpfnWndProc   = Favorite_MainWndProc;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = NULL;
			wc.hIcon         = NULL;
			wc.hCursor       = NULL;
			wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
			wc.lpszMenuName  = NULL;
			wc.lpszClassName = FAVORITEWC;

			if (!RegisterClass(&wc))
			{
				return  FALSE;
			}
			else
			{
				bRegister = TRUE;
			}
		}
		if (IsWindow(hFrameWnd)) 
		{
			UpdateWindow(hFrameWnd);
			ShowWindow(hFrameWnd,SW_SHOW);
		}
		else
		{

			RECT	rcFrameClient;
#ifdef _MODULE_DEBUG_
			StartObjectDebug();
#endif
			hFrameWnd	= CreateFrameWindow(WS_CAPTION|PWS_STATICBAR);
			
			SendMessage(hFrameWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_EXIT,0),(LPARAM)IDS_EXIT);
			SendMessage(hFrameWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_OPEN,1),(LPARAM)IDS_OPEN);

			GetClientRect(hFrameWnd,&rcFrameClient);

			hMenu	= LoadMenuIndirect(&FavoriteMenuTemplate);
			//hEmptyListMenu = LoadMenuIndirect(&FavoriteEmptyListMenuTemplate);

			hMainWnd = CreateWindow(FAVORITEWC,NULL,
				WS_VISIBLE|WS_CHILD,
				rcFrameClient.left,
				rcFrameClient.top,
				rcFrameClient.right - rcFrameClient.left,
				rcFrameClient.bottom - rcFrameClient.top,
				
				//PLX_WIN_POSITION,
				hFrameWnd,NULL,NULL,NULL);

			if (!hMainWnd)
			{
				return FALSE;
			}
			ModifyMenu(hMenu, IDC_OPEN, MF_BYCOMMAND, IDC_OPEN, IDS_OPEN);
			//ModifyMenu(hMenu, IDC_ADD, MF_BYCOMMAND, IDC_ADD, IDS_ADD);
			ModifyMenu(hMenu, IDC_RENAME, MF_BYCOMMAND, IDC_RENAME, IDS_RENAME);
			ModifyMenu(hMenu, IDC_MOVE, MF_BYCOMMAND, IDC_MOVE, IDS_MOVE);
			ModifyMenu(hMenu, IDC_REMOVE, MF_BYCOMMAND, IDC_REMOVE, IDS_REMOVE);

			//ModifyMenu(hEmptyListMenu, IDC_ADD, MF_BYCOMMAND, IDC_ADD, IDS_ADD);

			
			//SendMessage(hFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");

			PDASetMenu(hFrameWnd, hMenu);
			UpdateWindow(hFrameWnd);
			ShowWindow(hFrameWnd,SW_SHOW);
			
			SetFocus(hMainWnd);
	
		}
		break;
	case APP_GETOPTION:
		switch(wParam) {
		case AS_APPWND:
			dRet	= (DWORD)hFrameWnd;
			break;		
		}
		break;

	case APP_INACTIVE:
		ShowWindow(hFrameWnd,SW_HIDE);
		//DestroyWindow(hMainWnd);
		//DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance); 
		break;

	}
	return dRet;
}


static	LRESULT	Favorite_MainWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	LRESULT	lResult;
	int	listWidth,listHeight;
	int		index,i;// nCurSel;
//	APPENTRY	appsel;
//	HANDLE	hTemp;
	PITEMDATA	pTempItem,pExtra;
	RECT	rcClient;
	char	bufPrompt[50];
	BOOL    bMoveStat = FALSE, bResult = FALSE;
//	HDC		hdc;



	switch(uMsg) 
	{
	case WM_CREATE:
		SetWindowText(hFrameWnd, IDS_FAVOR);
		WaitWin(hWnd, TRUE, IDS_OPENING, NULL, NULL, IDS_CANCEL, IDRM_OPENING);
		hRBoffbmp	=	LoadImage(NULL, RADIO_OFF, IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE);
		hRBonbmp	=	LoadImage(NULL, RADIO_ON, IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE);
		hShortCurIon = LoadImage(NULL, SHORTCURICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
		hNewShortCurIon = LoadImage(NULL, NEWSHORTCURICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
		GetClientRect(hWnd, &rcClient);
		hShortcurList = CreateWindow("LISTBOX",NULL,
			WS_VISIBLE | WS_CHILD | LBS_BITMAP|WS_VSCROLL,
			rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			hWnd,(HMENU)IDC_LIST,NULL,NULL);
		OldListBoxProc = (WNDPROC)GetWindowLong(hShortcurList,GWL_WNDPROC);
		SetWindowLong(hShortcurList,GWL_WNDPROC,(LONG)MyListProc);

		SendMessage(hShortcurList, LB_ADDSTRING, 0, (LPARAM)IDS_NEWSHORTCUT);
		SendMessage(hShortcurList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)hNewShortCurIon);
		
		if (-1 == (fp = open(FAVORCONF,O_RDONLY))) 
		{
			bFileExist = FALSE;
			index = SendMessage(hShortcurList,LB_ADDSTRING,0,(LPARAM)ML("Inbox"));
			pTempItem = CreateAndAddNodeToChain(ML("Inbox"),"Messaging", 1);
			SendMessage(hShortcurList,LB_SETITEMDATA,index,(LPARAM)pTempItem);
			SendMessage(hShortcurList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hShortCurIon);

			index = SendMessage(hShortcurList,LB_ADDSTRING,1,(LPARAM)ML("Alarms"));
			pTempItem = CreateAndAddNodeToChain(ML("Alarms"),"Clock", 1);
			SendMessage(hShortcurList,LB_SETITEMDATA,index,(LPARAM)pTempItem);
			SendMessage(hShortcurList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hShortCurIon);

			index = SendMessage(hShortcurList,LB_ADDSTRING,2,(LPARAM)ML("Bluetooth"));
			pTempItem = CreateAndAddNodeToChain(ML("Bluetooth"),"Bluetooth", 1);
			SendMessage(hShortcurList,LB_SETITEMDATA,index,(LPARAM)pTempItem);
			SendMessage(hShortcurList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hShortCurIon);
/*
			index = SendMessage(hShortcurList,LB_ADDSTRING,3,(LPARAM)"GoBang");
			pTempItem = CreateAndAddNodeToChain(ML("GoBang"),ML("GoBang"), index);
			SendMessage(hShortcurList,LB_SETITEMDATA,index,(LPARAM)pTempItem);
*/
		}
		else
		{
			bFileExist	=	TRUE;
			lseek(fp, 0L, SEEK_SET);
			pTempItem = (PITEMDATA)malloc(sizeof(ITEMDATA));
			if (!pTempItem) {
				return FALSE;
			}
			listHeight = sizeof(ITEMDATA);
			while ((listWidth = read(fp, pTempItem, sizeof(ITEMDATA))) == listHeight) 
			{				
				index = SendMessage(hShortcurList,LB_ADDSTRING,0,(LPARAM)pTempItem->showName);
				pExtra = CreateAndAddNodeToChain(pTempItem->showName,pTempItem->lpName, pTempItem->nIndex);
				SendMessage(hShortcurList,LB_SETITEMDATA,index,(LPARAM)pExtra);
				SendMessage(hShortcurList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hShortCurIon);
				
			}
			free(pTempItem);
			pTempItem = NULL;
			
		}
		if (bFileExist) {
			close(fp);
//			bFileExist = FALSE;
		}
		
		
		SendMessage(hShortcurList, LB_SETCURSEL, iSelindex, 0);
		if (iSelindex == 0) {
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		}
		else
		{
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		}
		WaitWin(hWnd, FALSE, IDS_OPENING, NULL, NULL, IDS_CANCEL, IDRM_OPENING);
		
		
		break;
		
	case IDRM_OPENING:
		bException = TRUE;
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case PWM_SHOWWINDOW:
		SetWindowText(hFrameWnd, IDS_FAVOR);
		iSelindex = SendMessage(hShortcurList, LB_GETCURSEL, 0, 0);
		if (iSelindex == 0) {
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		}
		else
		{
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		}
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
		
		if (bMove) 
		{
			//Enter Move stat
			bResult = SetWindowText(hFrameWnd,IDS_MOVECAPTION);
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		}
	
		break;
	

	case WM_SETFOCUS:
		SetFocus(hShortcurList);
		
		break;

	case WM_COMMAND:		
		switch(LOWORD(wParam)) 
		{
		case IDC_OPEN:
			iSelindex = SendMessage(hShortcurList,LB_GETCURSEL,0,0);
			pTempItem = (PITEMDATA)SendMessage(hShortcurList,LB_GETITEMDATA,iSelindex,0);
			PostMessage(hWnd,WM_CLOSE,0,0);
			if(!findstr(appName, pTempItem->lpName)) {
				break;
			}
			if (stricmp(pTempItem->lpName, "Messaging") == 0)
			{
				switch(pTempItem->nIndex) {
				case 5:
					CallAppEntryEx("Edit MMS", 0, 0);
					break;

				case 3:
					CallAppEntryEx("Edit SMS", 0, 0);
					break;

				case 4:
					CallAppEntryEx("Edit Email", 0, 0);
					break;
				default:
					CallAppEntryEx(pTempItem->lpName, 0, pTempItem->nIndex);
					break;
				}				
			}
			else if (stricmp(pTempItem->lpName, "Memory management") == 0) 
			{
				CallAppEntryEx("Memory", 0, 0);
			}
			else if (stricmp(pTempItem->lpName, "Network services") == 0)
			{
				CallAppEntryEx("Network", 0, pTempItem->nIndex);
			}
			else if (stricmp(pTempItem->lpName, "Wap Browser") == 0)
			{
				CallAppEntryEx("WAP", 0, 1);
			}
			else
				CallAppEntryEx(pTempItem->lpName, 0, pTempItem->nIndex);
			/*
			hTemp	= (HANDLE)DlmNotify((WPARAM)PES_APP_CREATE, (LPARAM)pTempItem->lpName);
			appsel.nCode = APP_ACTIVE;
			appsel.hApp = hTemp;
			appsel.wParam = NULL;
			appsel.lParam = NULL;			
			DlmNotify(PES_APP_CALL,(LPARAM)&appsel);
			*/
			break;

		case IDC_ADD:
			Favor_AddShortCur(hWnd);
			break;

		case IDC_MOVE:
			bMove = TRUE;
			ChangeMoveStat(bMove);
			break;

		case IDC_REMOVE:

			iSelindex	=	SendMessage(hShortcurList,LB_GETCURSEL,0,0);
			pTempItem	=	(PITEMDATA)SendMessage(hShortcurList,LB_GETITEMDATA,iSelindex,0);
			if (!pTempItem) {
				break;
			}
			if (pTempItem->nIndex != 0) {
				sprintf(bufPrompt,"%s\n%s\n%s", pTempItem->lpName, pTempItem->showName, IDS_REMOVESHORTCUT);
			}
			else
				sprintf(bufPrompt,"%s\n%s", pTempItem->showName, IDS_REMOVESHORTCUT);
			
			if (PLXConfirmWin(hFrameWnd, hWnd, bufPrompt, Notify_Request, IDS_FAVOR, IDS_YES, IDS_NO)) 
			{
				free(pTempItem);
				pTempItem = NULL;
				SendMessage(hShortcurList, LB_DELETESTRING, iSelindex, 0);
				PLXTipsWin(hFrameWnd, hWnd, 0, IDS_SCREMOVED, IDS_FAVOR, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
				if (0 == iSelindex)
					SendMessage(hShortcurList, LB_SETCURSEL, 0, 0);
				else
					SendMessage(hShortcurList, LB_SETCURSEL, iSelindex-1, 0);
			}
			/*
			if (SendMessage(hShortcurList, LB_GETCOUNT, 0, 0) < 1) 
			{
				ShowWindow(hShortcurList,SW_HIDE);
				//SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_ADD);
				//PDASetMenu(hFrameWnd,hEmptyListMenu);
				GetClientRect(hWnd, &rcClient);
				InvalidateRect(hWnd, &rcClient, TRUE);
				UpdateWindow(hWnd);
				//bEnaleAdd = TRUE;
			}
			*/
			break;

		case IDC_RENAME:
			iSelindex = SendMessage(hShortcurList,LB_GETCURSEL,0,0);
			Favor_RenameShorcur(hWnd);

			break;

		case IDC_LIST:
			switch(HIWORD(wParam)) 
			{
			case LBN_SELCHANGE:
//				bEnaleAdd = FALSE;
				iSelindex	=	SendMessage(hShortcurList,LB_GETCURSEL,0,0);
				if (iSelindex == 0) {
					SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
					SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				}
				else
				{
					SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
					SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
				}
//				pItemdata	=	(PITEMDATA)SendMessage(hShortcurList,LB_GETITEMDATA,iSelindex,0);
/*				if (SendMessage(hShortcurList,LB_GETTEXTLEN,iSelindex,0) <= 0)
				{
					SendMessage(hWnd,PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_ADD);
					bEnaleAdd = TRUE;
				}
				if (!bEnaleAdd) {
					SendMessage(hWnd,PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
				}
				*/
				break;
			default:
				lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
				break;
			}	
			break;

		default:
			lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
			break;
		}

		break;

	case WM_KEYDOWN:
		switch(wParam) 
		{
		case VK_F10:
			if (bMove) {
				bMove = FALSE;
				ChangeMoveStat(bMove);
			}
			else{			
				PostMessage(hWnd,WM_CLOSE,0,0);
			}
			break;

		case VK_RETURN:
			if (iSelindex != 0) 
			{
				PostMessage(hWnd,WM_COMMAND,IDC_OPEN,0);
			}
			/*
		if (bEnaleAdd) {
				PostMessage(hWnd,WM_COMMAND,IDC_ADD,0);
			}
			else
			{			
				PostMessage(hWnd,WM_COMMAND,IDC_OPEN,0);
			}
			*/
			break;

		case VK_F5:
			if (bMove) {
				bMove = FALSE;
				ChangeMoveStat(bMove);
			}
			else
			{
				if (iSelindex != 0) {
					lResult = PDADefWindowProc(hFrameWnd,uMsg,wParam,lParam);
				}
				else
				{
					Favor_AddShortCur(hWnd);
				}
				
			}
			break;

		default:
			lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
			break;
		}
		break;

	case WM_CLOSE:
		
		SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		DestroyWindow(hMainWnd);
		if (UnregisterClass(FAVORITEWC,NULL)) {	
			bRegister	=	FALSE;
		}
		break;

	case WM_DESTROY:
		if (hShortCurIon) {
			DeleteObject(hShortCurIon);
		}
		if (hNewShortCurIon) {
			DeleteObject(hNewShortCurIon);
		}
		if (hRBoffbmp) {
			DeleteObject(hRBoffbmp);
			hRBoffbmp = NULL;
		}
		if (hRBonbmp) {
			DeleteObject(hRBonbmp);
			hRBonbmp = NULL;
		}
//		MultiByteToUTF8(CP_ACP, 0, (LPCSTR)szMulti, -1, (LPSTR)wszChar, nLenWideChar);
		if (bFileExist) 
		{
		//save current config list.write data to file
			fp = open(FAVORCONF,O_WRONLY | O_TRUNC);
			index = SendMessage(hShortcurList, LB_GETCOUNT, 0 ,0);
			for (i=1; i<index; i++)
			{
				pTempItem = (PITEMDATA)SendMessage(hShortcurList, LB_GETITEMDATA, i, 0);
				if (pTempItem) {				
					write(fp, pTempItem, sizeof(ITEMDATA));
					free(pTempItem);
					//pItemdata = NULL;
				}
			}

		}
		else
		{
			fp	= creat(FAVORCONF,0x0666);
			lseek(fp, 0L, SEEK_SET);
			index = SendMessage(hShortcurList, LB_GETCOUNT, 0 ,0);
			for (i=1; i<index; i++)
			{
				pTempItem = (PITEMDATA)SendMessage(hShortcurList, LB_GETITEMDATA, i, 0);
				write(fp, pTempItem, sizeof(ITEMDATA));
				free(pTempItem);
				//pTempItem = NULL;
			}
			
			//save current config list.save data to file
		}
		close(fp);	
		DestroyMenu(hMenu);
		//DestroyMenu(hEmptyListMenu);
		hMainWnd	=	NULL;
		DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
#ifdef _MODULE_DEBUG_
		EndObjectDebug();
#endif
		break;
	default:
		lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
		break;
	}
	return lResult;
}

static	LRESULT	MyListProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	LRESULT	lResult;
	PITEMDATA	pTempItem;
	switch(uMsg)
	{
	case WM_KEYDOWN:
		switch(wParam) 
		{
		case VK_DOWN:
			if (bMove) {
				iSelindex = OldListBoxProc(hShortcurList, LB_GETCURSEL, 0, 0);
				if (iSelindex == (OldListBoxProc(hShortcurList, LB_GETCOUNT, 0 ,0) -1)) {
					break;
				}
				pTempItem = (PITEMDATA)OldListBoxProc(hShortcurList, LB_GETITEMDATA, iSelindex, 0);
				OldListBoxProc(hShortcurList, LB_DELETESTRING, iSelindex, 0);
				OldListBoxProc(hShortcurList, LB_INSERTSTRING, iSelindex+1, (LPARAM)pTempItem->showName);
				OldListBoxProc(hShortcurList, LB_SETITEMDATA, iSelindex+1, (LPARAM)pTempItem);
				OldListBoxProc(hShortcurList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, iSelindex+1), (LPARAM)hShortCurIon);
				OldListBoxProc(hShortcurList, LB_SETCURSEL, iSelindex+1, 0);
				UpdateWindow(hWnd);
			}
			else
				lResult = OldListBoxProc(hWnd,uMsg,wParam,lParam);
			break;

		case VK_UP:
			if (bMove) {
				iSelindex = OldListBoxProc(hShortcurList, LB_GETCURSEL, 0, 0);
				if (iSelindex == 0) {
					break;
				}
				pTempItem = (PITEMDATA)OldListBoxProc(hShortcurList, LB_GETITEMDATA, iSelindex, 0);
				OldListBoxProc(hShortcurList, LB_DELETESTRING, iSelindex, 0);
				OldListBoxProc(hShortcurList, LB_INSERTSTRING, iSelindex-1, (LPARAM)pTempItem->showName);
				OldListBoxProc(hShortcurList, LB_SETITEMDATA, iSelindex-1, (LPARAM)pTempItem);
				OldListBoxProc(hShortcurList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, iSelindex-1), (LPARAM)hShortCurIon);
				OldListBoxProc(hShortcurList, LB_SETCURSEL, iSelindex -1, 0);
				UpdateWindow(hWnd);
			}
			else
				lResult = OldListBoxProc(hWnd,uMsg,wParam,lParam);
			break;
		default:
			lResult = OldListBoxProc(hWnd,uMsg,wParam,lParam);
			break;
		}
		break;
	default:
		lResult = OldListBoxProc(hWnd,uMsg,wParam,lParam);
		break;
	}
	return lResult;
}

static	PITEMDATA	CreateAndAddNodeToChain(PCSTR strShowName, PCSTR strRealPhone, int index)
{
	PITEMDATA	pTempNode;
	pTempNode = malloc(sizeof(ITEMDATA));
	if (!pTempNode) 
	{
		return FALSE;
	}
	strcpy(pTempNode->showName,strShowName);
	strcpy(pTempNode->lpName,strRealPhone);
	pTempNode->nIndex = index;
	
	return pTempNode;
}
/*
static	PITEMDATA	FindCurNode(int	nIndex)
{
	PITEMDATA	pTempNode;
	pTempNode	=	pHeadNode;
	if (nIndex > pTailNode->index) {
		return NULL;
	}
	while (pTempNode->index!= nIndex)
	{
		pTempNode= pTempNode->pNext;
		if (pTempNode == pHeadNode)
		{
			return NULL;
		}
	}
	return	pTempNode;
}
*/

static	BOOL ADDWND_register(void * hInst)
{
	WNDCLASS	wc;

	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = FavorAddShortCurProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = ADDSHORTWC;

	if (!RegisterClass(&wc)) 
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
static	BOOL Favor_AddShortCur(HWND hParent)
{
	RECT	rfClient;
	ADDWND_register(NULL);
	GetClientRect(hFrameWnd, &rfClient);

	hAddShortcurWnd = CreateWindow(ADDSHORTWC,NULL,
		WS_CHILD|WS_VISIBLE,
		//PLX_WIN_POSITION,
		rfClient.left,
		rfClient.top,
		rfClient.right - rfClient.left,
		rfClient.bottom - rfClient.top,
		hFrameWnd,
		NULL,
		NULL,
		NULL);
	if (!hAddShortcurWnd) {
		return	FALSE;
	}
	SetWindowText(hFrameWnd, IDS_ADDCAPTION);
	SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1,(LPARAM)"");
	SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0,(LPARAM)IDS_CANCEL);
	SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	//UpdateWindow(hFrameWnd);
	//ShowWindow(hFrameWnd, SW_SHOW);
	SetFocus(hAddShortcurWnd);
	return TRUE;
}

static	LRESULT	FavorAddShortCurProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	LRESULT	lResult;
	static	HBITMAP	hSelicon = NULL;
	static	HBITMAP	hUnselicon = NULL;
	static	HWND	hAddList = NULL;
	PITEMDATA	pTempItem;
	RECT	rc;
	int		nIndex;//, itotal;
	char	appnamebuf[41];

	


	switch(uMsg) 
	{
	case WM_CREATE:
		
		GetClientRect(hWnd, &rc);
		hAddList = CreateWindow("LISTBOX",NULL,
			WS_VISIBLE | WS_CHILD | LBS_BITMAP|WS_VSCROLL,
			rc.left, rc.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			hWnd,(HMENU)IDC_ADDLIST,NULL,NULL);
		FavorList_AddString(hAddList,appName,&hRBoffbmp,FALSE);
		//SendMessage(hAddList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)hRBonbmp);
		break;

	case WM_SETFOCUS:
		SetFocus(GetDlgItem(hWnd,IDC_ADDLIST));
		break;

	case PWM_SHOWWINDOW:
		SetWindowText(hFrameWnd, IDS_ADDCAPTION);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1,(LPARAM)"");
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0,(LPARAM)IDS_EXIT);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		UpdateWindow(hFrameWnd);
		SetFocus(GetDlgItem(hWnd,IDC_ADDLIST));
		break;
/*
	case WM_COMMAND:
		switch(HIWORD(wParam)) 
		{
		case LBN_SELCHANGE:
			itotal = SendMessage(hAddList, LB_GETCOUNT, 0, 0);
			nIndex = SendMessage(hAddList, LB_GETCURSEL, 0, 0);
			Load_Icon_SetupList(hAddList, hRBoffbmp, hRBonbmp, itotal,nIndex);
			break;
		default:
			lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
			break;
		}
		break;
*/
	case IDRM_ADDFEATURE:
//		SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case WM_TIMER:
		{
			// application published features [10/3/2005]
			char	*BluetoothFeature[]=
			{
				"Bluetooth",
				"Bt on/off",//"Bluetooth ON/OFF",
				"Bt search", //"Search for new devices",
				""
			};			
			char	*CalendarFeature[] = 
			{
				"Calendar",
				"Calendar entries", //"Calendar entrylist for current date",
					""
			};
			char	*ClockFeature[]=
			{
				"Clock",
				"Alarms",
				//"Set alarm ( or alarms already set)",
					""
			};
			char	*GPSFeature[] =
			{
				"GPS Monitor",
				"Satellite view",
					"Position refresh",
					"Position view",
					"Compass view",
					""
			};
			char	*LogsFeature[] = 
			{
				"Logs",
				"Missed calls",
					"Call counters",
					"Communications",
					""
			};
			char	*MsgFeature[] = 
			{
				"Messaging",
				"Inbox",
				"Default mailbox",//	"Inbox of default mailbox",
				"New SMS", //	"Write new SMS",
				"New e-mail",//	"Write new e-mail with default mailbox",
				"New MMS",//	"Write new MMS",
					""
			};
			char	*NetworkFeature[]=
			{
				"Network Services",
				"Call forwarding",
					""
			};
			char	*NotepadFeature[] = 
			{
				"Notepad",
				"New note",//"Write new note",
					""
			};
			char	*SettingsFeature[] = 
			{
				"Settings",
				"Sound profile",
					""
			};
			char	*WapFeature[] = 
			{
				"Wap browser",
				"Bookmarks",
					""
			};
			char	*PNSFeature[] =
			{
				"Navigation",
				"Map base view",
					"Navigate to",
					""
			};

			if (wParam == ID_RBTIMER) 
			{
				KillTimer(hWnd, ID_RBTIMER);
				memset(appnamebuf,0,sizeof(appnamebuf));
				nIndex = SendMessage(hAddList,LB_GETCURSEL,0,0);
				//SendMessage(hAddList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nIndex), (LPARAM)hRBonbmp);
				SendMessage(hAddList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nIndex), (LPARAM)hRBoffbmp);

				//SendMessage(hAddList,LB_GETTEXT,nIndex,(LPARAM)appnamebuf);

				if ((nIndex >= 0)&&(nIndex <= 24))
				{
					strcpy(appnamebuf, appName[nIndex]);
				}
				else
					return FALSE;
				

				if (stricmp(appnamebuf, "Bluetooth") == 0) 
				{
					Favor_AddFeature(hWnd, BluetoothFeature, "Bluetooth", IDRM_ADDFEATURE);
				}
				else if(stricmp(appnamebuf, "Calendar") == 0)
				{
					Favor_AddFeature(hWnd, CalendarFeature, "Calendar", IDRM_ADDFEATURE);
				}
				else if (stricmp(appnamebuf, "Clock") == 0)
				{
					Favor_AddFeature(hWnd, ClockFeature, "Clock", IDRM_ADDFEATURE);
				}
				else if (stricmp(appnamebuf, "GPS Monitor") == 0)
				{
					Favor_AddFeature(hWnd, GPSFeature, "GPS Monitor", IDRM_ADDFEATURE);
				}
				else if (stricmp(appnamebuf, "Logs") == 0)
				{
					Favor_AddFeature(hWnd, LogsFeature, "Logs", IDRM_ADDFEATURE);
				}
				else if (stricmp(appnamebuf, "Messaging") == 0)
				{
					Favor_AddFeature(hWnd, MsgFeature, "Messaging", IDRM_ADDFEATURE);
				}
				else if (stricmp(appnamebuf, "Network services") == 0)
				{
					Favor_AddFeature(hWnd, NetworkFeature, "Network services", IDRM_ADDFEATURE);
				}
				else if (stricmp(appnamebuf, "Notepad") == 0)
				{
					Favor_AddFeature(hWnd, NotepadFeature, "Notepad", IDRM_ADDFEATURE);
				}
				else if (stricmp(appnamebuf, "Settings") == 0)
				{
					Favor_AddFeature(hWnd, SettingsFeature, "Settings", IDRM_ADDFEATURE);
				}
				else if (stricmp(appnamebuf, "Wap Browser") == 0)
				{
					Favor_AddFeature(hWnd, WapFeature, "Wap Browser", IDRM_ADDFEATURE);
				}
				else if (stricmp(appnamebuf, "Navigation client") == 0)
				{
					Favor_AddFeature(hWnd, PNSFeature, "Navigation client", IDRM_ADDFEATURE);
				}
				else
				{
					nIndex = SendMessage(hShortcurList, LB_INSERTSTRING, 1, (LPARAM)ML(appnamebuf));
					pTempItem = CreateAndAddNodeToChain(ML(appnamebuf),appnamebuf, 0);
					SendMessage(hShortcurList, LB_SETCURSEL, nIndex, 0);
					SendMessage(hShortcurList, LB_SETITEMDATA,nIndex,(LPARAM)pTempItem);
					SendMessage(hShortcurList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nIndex), (LPARAM)hShortCurIon);
					iSelindex = nIndex;
					PLXTipsWin(NULL, hWnd, 0, IDS_ADDPROMT,IDS_ADDCAPTION,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
					PostMessage(hWnd,WM_CLOSE,0,0);
				}
			}
			else
			{
				lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
			}
		}
		break;


	case WM_KEYDOWN:
		switch(wParam) 
		{
		case VK_F5:
			nIndex = SendMessage(hAddList,LB_GETCURSEL,0,0);
			SendMessage(hAddList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nIndex), (LPARAM)hRBonbmp);
			SetTimer(hWnd, ID_RBTIMER, 300, NULL);
			
			break;

		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			break;
			
		default:
			lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
			break;
		}
		break;

	case WM_CLOSE:		
		SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		DestroyWindow(hWnd);
		hAddShortcurWnd = NULL;
		
		break;

	case WM_DESTROY:
		
		UnregisterClass(ADDSHORTWC,NULL);
		break;

	default:
		lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
		break;
	}
	return lResult;
}

static	BOOL	ChangeMoveStat(BOOL bMoveStat)
{
	BOOL	bResult;
	int		nIndex;
	int		nCurSel = 0;
	if (bMoveStat) 
	{
		//Enter Move stat
		bResult = SetWindowText(hFrameWnd,IDS_MOVECAPTION);
		SendMessage(hShortcurList, LB_DELETESTRING, 0, 0);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	}
	else
	{
		//Cancel Move Stat
		bResult = SetWindowText(hFrameWnd,IDS_FAVOR);

		nCurSel = SendMessage(hShortcurList, LB_GETCURSEL, 0, 0);

		nIndex = SendMessage(hShortcurList, LB_INSERTSTRING, 0, (LPARAM)IDS_NEWSHORTCUT);
		SendMessage(hShortcurList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nIndex), (LPARAM)hNewShortCurIon);

		SendMessage(hShortcurList, LB_SETCURSEL, nCurSel+1, 0);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
	}
	return	TRUE;
}

static	BOOL	Favor_RenameShorcur(HWND hWnd)
{
	WNDCLASS	wc;
	RECT		rfClient;

	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = Favor_RenameProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = RENAMEWCLS;



	if (!RegisterClass(&wc)) {
		return FALSE;
	}
	GetClientRect(hFrameWnd, &rfClient);
	hRenameWnd	= CreateWindow(RENAMEWCLS,IDS_RNMCAPTION,
		WS_VISIBLE | WS_CHILD,
		//PLX_WIN_POSITION,
		rfClient.left,
		rfClient.top,
		rfClient.right - rfClient.left,
		rfClient.bottom - rfClient.top,
		hFrameWnd,NULL,NULL,NULL);
	if (!hRenameWnd)
	{
		return FALSE;
	}
	SetWindowText( hFrameWnd,IDS_RNMCAPTION);
	SendMessage(hFrameWnd,PWM_SETBUTTONTEXT,  1,(LPARAM)IDS_SAVE);
	SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 0,(LPARAM)IDS_CANCEL);
	SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 2,(LPARAM)"");
	UpdateWindow(hFrameWnd);
	ShowWindow(hFrameWnd,SW_SHOW);
	SetFocus(hRenameWnd);
	return TRUE;
//	PITEMDATA	pTempNode;
//	pTempNode = SendMessage(hShortcurList, LB_SETITEMDATA, index);
	
}

static	LRESULT	Favor_RenameProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT	lResult;
	static	HWND	hRenameEdit = NULL;
	IMEEDIT	ie;
	RECT	rcClient;
	//PSTR	ptr;
	char	ptr[31];
	int		txtlen;
	PITEMDATA	pTempNode;
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
			WS_VISIBLE| WS_CHILD|ES_MULTILINE|ES_AUTOHSCROLL|ES_TITLE|WS_TABSTOP,
			rcClient.left,
			rcClient.top,
			rcClient.right - rcClient.left,
			52,//rcClient.bottom - rcClient.top,
			hWnd,(HMENU)IDC_RNMEDIT,NULL,(PVOID)&ie);
		SendMessage(hRenameEdit,EM_LIMITTEXT,30,0);
		SendMessage(hRenameEdit,EM_SETTITLE,0,(LPARAM)IDS_NAME);
		pTempNode = (PITEMDATA)SendMessage(hShortcurList, LB_GETITEMDATA, iSelindex,0);
		SendMessage(hRenameEdit,WM_SETTEXT,0,(LPARAM)pTempNode->showName);
		
		break;

	case PWM_SHOWWINDOW:
		SetWindowText( hFrameWnd,IDS_RNMCAPTION);
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT,  1,(LPARAM)IDS_SAVE);
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 0,(LPARAM)IDS_CANCEL);
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 2,(LPARAM)"");
		UpdateWindow(hFrameWnd);
		SetFocus(hRenameEdit);
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
				txtlen	= GetWindowTextLength(hRenameEdit);
				GetWindowText(hRenameEdit,ptr,txtlen+1);

				if ((txtlen < 1)||(ptr[0] == 0x20)) {
					PLXTipsWin(hFrameWnd, hWnd, 0, IDS_RNMNOTIFY,NULL,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
					break;
				}
				else
				{			
					//ptr		= malloc(txtlen+1);
					//GetWindowText(hRenameEdit,ptr,txtlen+1);
					SendMessage(hShortcurList,LB_SETTEXT,iSelindex,(LPARAM)ptr);
					pTempNode = (PITEMDATA)SendMessage(hShortcurList,LB_GETITEMDATA,iSelindex,0);
					strcpy(pTempNode->showName,ptr);
					SendMessage(hShortcurList, LB_SETITEMDATA, iSelindex, (LPARAM)pTempNode);
					//free(ptr);
					PostMessage(hWnd,WM_CLOSE,0,0);
					break;
				}
			}
			break;

		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			break;
/*
		default:
			lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
			break;
			*/
		}
		break;

	case WM_CLOSE:
		SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		DestroyWindow(hWnd);
		UnregisterClass(RENAMEWCLS,NULL);
		break;

	default:
		lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
		break;
	}
	return lResult;
}

static	BOOL AddFeatureRegister(void * hInst)
{
	WNDCLASS	wc;

	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = AddFeatureProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof(FEATUREDATA);
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = ADDFEATURE;

	if (!RegisterClass(&wc)) 
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


static	BOOL	Favor_AddFeature(HWND	hParent, char **feature, PCSTR pAppName, UINT uMsg)
{
	HWND	hAddFeature;
	RECT	rc;
	FEATUREDATA	NewFeature;

	if (!feature) {
		return FALSE;
	}
	if (!pAppName) {
		return FALSE;
	}
	AddFeatureRegister(NULL);
	memset(&NewFeature, 0, sizeof(FEATUREDATA));

	NewFeature.hParent = hParent;
	NewFeature.pNewFeature = feature;
	NewFeature.uMsgCloseParent = uMsg;
	strcpy(NewFeature.szFeatureAppName, pAppName);
	//pNewFeature = feature;
	//strcpy(szFeatureAppName, pAppName);

	GetClientRect(hFrameWnd, &rc);

	hAddFeature = CreateWindow(ADDFEATURE, NULL,
		WS_CHILD|WS_VISIBLE,
		rc.left, rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		hFrameWnd,
		NULL,
		NULL,
		(LPVOID)&NewFeature);

	if (!hAddFeature) {
		UnregisterClass(ADDFEATURE,NULL);
		return FALSE;
	}
	SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
	SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
	ShowWindow(hFrameWnd, SW_SHOW);
	SetFocus(hAddFeature);

	return TRUE;
}

static	LRESULT	AddFeatureProc(HWND hwnd, UINT	uMsg, WPARAM wparam, LPARAM lparam)
{
	LRESULT	lResult;
	RECT	rc;
	int		nCurSel, nIndex;
	char	szItemText[121] = "";
	PITEMDATA	pItem;
	PFEATUREDATA	pFeature;
	PCREATESTRUCT	pc;

	lResult = TRUE;

	pFeature = GetUserData(hwnd);

	switch(uMsg) 
	{
	case WM_CREATE:
		{
			pc = (PCREATESTRUCT)lparam;
			memcpy(pFeature, (PFEATUREDATA)pc->lpCreateParams, sizeof(FEATUREDATA));
			GetClientRect(hwnd, &rc);
			
			hFeatureList = CreateWindow("LISTBOX", NULL,
				WS_CHILD|WS_VISIBLE|LBS_BITMAP|WS_VSCROLL,
				rc.left, rc.top,
				rc.right - rc.left,
				rc.bottom - rc.top,
				hwnd,
				(HMENU)IDC_FEATURELIST,
				NULL,
				NULL);
			if (!hFeatureList) {
				return FALSE;
			}
			FavorList_AddString(hFeatureList, pFeature->pNewFeature, &hRBoffbmp, FALSE);
			SendMessage(hFeatureList, LB_SETCURSEL, 0, 0);
		}
		break;

	case WM_SETFOCUS:
		SetFocus(hFeatureList);
		break;

	case PWM_SHOWWINDOW:
		SetWindowText(hFrameWnd, IDS_NEWSHORTCUT);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SetFocus(hFeatureList);
		break;
/*
	case WM_COMMAND:
		break;

	case WM_TIMER:
		break;
*/
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		UnregisterClass(ADDFEATURE,NULL);
		break;

	case IDRM_ADDFEATURE:
		SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		PostMessage(pFeature->hParent, pFeature->uMsgCloseParent, 0, 0);
		break;
		
	case WM_KEYDOWN:
		switch(wparam) 
		{
		case VK_F10:
			SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;

		case VK_F5:
			nCurSel = SendMessage(hFeatureList, LB_GETCURSEL, 0, 0);
			SendMessage(hFeatureList, LB_GETTEXT, nCurSel, (LPARAM)szItemText);
			nIndex = SendMessage(hShortcurList, LB_INSERTSTRING, 1, (LPARAM)szItemText);
			//nIndex = SendMessage(hShortcurList, LB_INSERTSTRING, 1, (LPARAM)pFeature->szFeatureAppName);
			SendMessage(hShortcurList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nIndex), (LPARAM)hShortCurIon);
			pItem = CreateAndAddNodeToChain(szItemText, pFeature->szFeatureAppName, nCurSel);
			SendMessage(hShortcurList, LB_SETITEMDATA, nIndex, (LPARAM)pItem);
			PLXTipsWin(NULL, hwnd, IDRM_ADDFEATURE, IDS_ADDPROMT,IDS_ADDCAPTION,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
			/*
			SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			PostMessage(pFeature->hParent, pFeature->uMsgCloseParent, 0, 0);
			*/
			break;

		default:
			lResult = PDADefWindowProc(hwnd, uMsg, wparam,lparam);
			break;
		}
		break;

	default:
		lResult = PDADefWindowProc(hwnd, uMsg, wparam,lparam);
		break;
	}

	return lResult;
	
}
static	void    FavorList_AddString(HWND hList,char ** p,HBITMAP * hIconNormal,BOOL bAppend)
{
    DWORD   dWord;
    int     i = 0;

    while (** p)
    {
        SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML(* p++));

        if (hIconNormal != NULL)
        {
            dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i++);
            SendMessage(hList, LB_SETIMAGE, (WPARAM)dWord, (LPARAM)* hIconNormal);
            if (bAppend)
                hIconNormal++;
        }
    }
    SendMessage(hList,LB_SETCURSEL,0,0);
}
