/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Notebook
 *
 * Purpose  : main window
 *            
\**************************************************************************/

#include "pollex.h"
#include "window.h"
#include "stdio.h"
#include "string.h"

#include "plx_pdaex.h"
#include "pubapp.h"
#include "winpda.h"
#include "imesys.h"
#include "malloc.h"

#include "Notepad.h"
#include "TextFile.h"
#include "MmsUi.h"
//////////////////////////////////////////////////////////////////////////////
//
//			Global Variables
//
//////////////////////////////////////////////////////////////////////////////
HWND hNotepadFrameWnd = NULL;
static HWND hWndNotepadMain = NULL;
static HWND hWndViewTxt = NULL;
static HWND hImeTextNotepad;
static HINSTANCE NotepadhIns;
static HWND hTextList, hDelList;
static PTEXTINFO pCurInfo;
static LONG curSel;
static HBITMAP hBmpNewNote, hBmpNote;
static BOOL bNoRealQuit = FALSE;//for shortcut, only destroy windows and create them again but not need to notify progman
static BOOL bCurShortCut = FALSE;


//////////////////////////////////////////////////////////////////////////////
//
//			Functions
//
//////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK NotepadMainWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void NotepadMainWinCreate(HWND hWnd);
static HMENU CreateMainWinMenu();
static void AddNewRecordToList(PTEXTINFO pInfo, BOOL MoreLong);
static void AddNewRecordToDeleList(PTEXTINFO pInfo,  BOOL MoreLong);
static void NotepadMainWinCommad(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnEditText(HWND hWnd,char *szContent);
static void OnModifyText(BOOL bIsViewWnd);
static void RemoveCurSelListItem();
static void OnRename(HWND hWnd);
static void OnDelete();
static void OnDeleteAll();
static void OnSendMMS(HWND hWnd);
static void OnSendSMS(HWND hWnd);
static void OnSendMail(HWND hWnd);
static void OnSendBlueTooth();
static void ReListTextRecord();
static DWORD TextDeleteWin(void);
static LRESULT CALLBACK NotepadDeleteWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void NotepadDeleteWinCreate(HWND hWnd);
static void NotepadDeleteWinCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void DoTextFileDeletion(HWND hWnd);
static DWORD ViewTextWin(void);
static DWORD NotepadEditWnd(char *pContent, BOOL bIsViewWnd, BOOL bIsNewNote);
static LRESULT EditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void CreateEdit(HWND hWnd,char *pContent, BOOL bIsNewNote);
static void RefreshViewArrow(int selindex, int total);

static LRESULT CALLBACK NotepadViewWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void NotepadViewWinCommad(HWND hWnd, WPARAM wParam, LPARAM lParam);
void SaveFileToNotepad(char *pFileName, char *pDesPath);
extern BOOL APP_EditSMS(HWND hParent,const char* PSZRECIPIENT,const char* PSZCONTENT);
extern BOOL MAIL_CreateEditInterface(HWND hFrameWindow, char *PSZRECIPIENT, char *PSZATTENMENT, char *PSZATTNAME, BOOL bAtt);
extern BOOL BtSendData(HWND hAppFrameWnd,char* szAppPathName,char* szViewFileName,int iFileType);
extern BOOL NoteBrowserRegisterClass(void);

//////////////////////////////////////////////////////////////////////////////
// Function	Notepad_AppCtrl
// Purpose	entry function
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
DWORD Notepad_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam){
	DWORD dwRet;
	WNDCLASS wc;
	RECT rClient;
	
	dwRet = TRUE;
	switch(nCode){
	case APP_INIT:
		NoteBrowserRegisterClass();
		NotepadhIns = (HINSTANCE)pInstance;
	
		break;
	case APP_GETOPTION:
		switch(wParam)
		{
		case AS_APPWND:
			dwRet = (DWORD)hNotepadFrameWnd;
			break;
		}
		break;
	case APP_ACTIVE:
		////////////////////for test/////////////////////	
		
		if ( lParam == 1 )		//favourite & shortcut for logs
		{
			if(bCurShortCut)				
			{
				ShowWindow(hNotepadFrameWnd, SW_SHOW);
				ShowOwnedPopups(hNotepadFrameWnd, SW_SHOW);
				UpdateWindow(hNotepadFrameWnd);
			}
			else
			{
				if (hNotepadFrameWnd )
				{	
					bNoRealQuit = TRUE;
					DestroyWindow(hNotepadFrameWnd);				
					hNotepadFrameWnd = NULL;
					
					
				}
				bCurShortCut = TRUE;
				wc.cbClsExtra = 0;
				wc.cbWndExtra = 0;
				wc.hbrBackground = GetStockObject(WHITE_BRUSH);
				wc.hCursor = NULL;
				wc.hIcon = NULL;
				wc.hInstance = NULL;
				wc.lpfnWndProc = NotepadMainWinProc;
				wc.lpszClassName = "NotepdaMainWinClass";
				wc.lpszMenuName = NULL;
				wc.style = 0;
				if(!RegisterClass(&wc))
				{
					UnregisterClass("NotepdaMainWinClass", NULL);
					return FALSE;
				}
				hNotepadFrameWnd = CreateFrameWindow( WS_CAPTION | PWS_STATICBAR);
				GetClientRect(hNotepadFrameWnd,&rClient);
				
				hWndNotepadMain = CreateWindow(
					"NotepdaMainWinClass",
					"",//IDS_NOTEPAD,
					WS_VISIBLE | WS_CHILD,
					rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
					hNotepadFrameWnd,
					NULL,
					NULL,
					NULL
					);
				NotepadEditWnd(NULL, FALSE, TRUE);//call write new notepad view
			}
		
			break;
			
		}

        if(IsWindow(hNotepadFrameWnd))
		{
			ShowWindow(hNotepadFrameWnd, SW_SHOW);
			ShowOwnedPopups(hNotepadFrameWnd, SW_SHOW);
			UpdateWindow(hNotepadFrameWnd);
		}
        else
		{
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hbrBackground = GetStockObject(WHITE_BRUSH);
			wc.hCursor = NULL;
			wc.hIcon = NULL;
			wc.hInstance = NULL;
			wc.lpfnWndProc = NotepadMainWinProc;
			wc.lpszClassName = "NotepdaMainWinClass";
			wc.lpszMenuName = NULL;
			wc.style = 0;
			if(!RegisterClass(&wc))
			{
				UnregisterClass("NotepdaMainWinClass", NULL);
				return FALSE;
			}
			hNotepadFrameWnd = CreateFrameWindow( WS_CAPTION | PWS_STATICBAR);
			GetClientRect(hNotepadFrameWnd,&rClient);

			hWndNotepadMain = CreateWindow(
                "NotepdaMainWinClass",
                "",//IDS_NOTEPAD,
                WS_VISIBLE | WS_CHILD,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
                hNotepadFrameWnd,
                NULL,
                NULL,
                NULL
                );

			ShowWindow(hNotepadFrameWnd, SW_SHOW);
			UpdateWindow(hNotepadFrameWnd);
			
		}
	    break;
	case APP_INACTIVE:
		ShowOwnedPopups(hNotepadFrameWnd,SW_HIDE);
		ShowWindow(hNotepadFrameWnd, SW_HIDE);
        break;
    }

    return dwRet;
}

//////////////////////////////////////////////////////////////////////////////
// Function	NotepadMainWinProc
// Purpose	deal main window function
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK NotepadMainWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	LRESULT lRet = TRUE;
	static HMENU hMenu;
	

	switch(message)
	{
	case WM_CREATE:
		{
			HDC hdc;
			COLORREF Color;
			BOOL bTrans;
			
			hdc = GetDC(hWnd);
			hBmpNewNote = CreateBitmapFromImageFile(hdc, ICON_NEWNOTE, &Color, &bTrans);
			hBmpNote = CreateBitmapFromImageFile(hdc, ICON_NOTE, &Color, &bTrans);	
			ReleaseDC(hWnd, hdc);
		}
	
		NotepadMainWinCreate(hWnd);
		hMenu = CreateMainWinMenu();
		SendMessage(hNotepadFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_OPEN, 1), (LPARAM)"");
		SendMessage(hNotepadFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_EXIT,0),(LPARAM)IDS_EXIT);
		SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SetWindowText(hNotepadFrameWnd, IDS_NOTEPAD);
		SetFocus(hTextList);
		break;

	case PWM_SHOWWINDOW:
		{
			int nIndex;
			PDASetMenu(hNotepadFrameWnd, hMenu);
			SetFocus(hTextList);			
			nIndex = SendMessage(hTextList, LB_GETCURSEL, 0, 0 );
			if (nIndex)
			{
				SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
				SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");  
			}
			else
			{
				SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			}
			SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);
			SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
			SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
			SetWindowText(hNotepadFrameWnd, IDS_NOTEPAD);	
		}
		break;		


	case WM_PAINT:
		{
			HDC hdc;			
			hdc = BeginPaint(hWnd, NULL);
			EndPaint(hWnd, NULL);
		}
	
		break;

	case WM_COMMAND:
		NotepadMainWinCommad(hWnd, wParam, lParam);
		switch(HIWORD(wParam)) 
		{
		case LBN_SELCHANGE:
			{
				int nIndex;
				nIndex = SendMessage(hTextList, LB_GETCURSEL, 0, 0 );
				if (nIndex)
				{
					SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
					SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");  
				}
				else
				{
					SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
					SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				}
			}
			break;
		}
		break;

	case CONFIRM_DELETE:
		if (lParam)
		{
			OnDelete();
			PLXTipsWin(hNotepadFrameWnd, hWnd, NULL, IDS_DELSUCCESS, IDS_NOTEPAD, Notify_Success,  IDS_OK, NULL, WAITTIMEOUT);
		}		
		break;	
	case CONFIRM_DELETEALL:
		if (lParam)
		{
			OnDeleteAll();
			PLXTipsWin(hNotepadFrameWnd, hWnd, NULL, IDS_DELSUCCESS, IDS_NOTEPAD, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F5:
			curSel = SendMessage(hTextList, LB_GETCURSEL, 0, 0);
			if (0 == curSel) 
				SendMessage(hWnd, WM_COMMAND, ID_NEW, NULL);
			else
				SendMessage(hNotepadFrameWnd, message, wParam, lParam);
			break;
		case VK_F10:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case VK_RETURN:
			curSel = SendMessage(hTextList, LB_GETCURSEL, 0, 0);
			if (curSel) 
				SendMessage(hWnd, WM_COMMAND, ID_OPEN, 0);
			break;

		default:
			lRet = PDADefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
		break;

	case WM_CLOSE:	
		DestroyWindow(hWnd);	
		break;

	case WM_DESTROY:
		if(hBmpNote)
			DeleteObject(hBmpNote);
		if(hBmpNewNote)
			DeleteObject(hBmpNewNote);	
		ReleaseAllTextInfo();
		DestroyMenu(hMenu);
		hMenu = NULL;
		UnregisterClass("NotepdaMainWinClass", NULL);	
		SendMessage(hNotepadFrameWnd, PWM_CLOSEWINDOW, 0, 0);
		if(	!bNoRealQuit)
			DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)NotepadhIns);
		bNoRealQuit = FALSE;
		bCurShortCut = FALSE;
		hWndNotepadMain = NULL;
		hNotepadFrameWnd = NULL;
		break;
	default:
		lRet = PDADefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return lRet;
}



//////////////////////////////////////////////////////////////////////////////
// Function	NotepadMainWinCreate
// Purpose	deal WM_CREATE message in main window
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void NotepadMainWinCreate(HWND hWnd)
{
	PTEXTINFO pInfo;
	RECT rccl;

	GetClientRect(hWnd, &rccl);
	hTextList = CreateWindow("LISTBOX",
		"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_MULTILINE | LBS_BITMAP,
		rccl.left, rccl.top,
		rccl.right - rccl.left, rccl.bottom - rccl.top,
		hWnd,
		(HMENU)ID_MAINLIST,
		NULL,
		NULL);

	SendMessage(hTextList, LB_ADDSTRING, 0 , (LPARAM)IDS_NEWNOTE);
	SendMessage(hTextList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)hBmpNewNote);
	if(GetAllTextNode(TEXT_PATH_FLASH) == 0)
	{
		SendMessage(hTextList, LB_SETCURSEL, 0, 0);
		return;
	}
	

	if(MoveToFirstTextNode())
	{
		do {	
			pInfo = GetCurTextInfo();
			if (pInfo->dwTextLength > TEXT_NAME_LENGTH  -1)
				AddNewRecordToList(pInfo, 1);
			else 
				AddNewRecordToList(pInfo, 0);
		} while(MoveToNextTextNode());
	}

	SendMessage(hTextList, LB_SETCURSEL, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////
// Function	CreateMainWinMenu
// Purpose	create main window's menu
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
HMENU CreateMainWinMenu()
{
	HMENU hMenu, hSend, hDeleMany;
	int ilenMulti =15;
	
	hSend = CreateMenu();
	AppendMenu(hSend, MF_STRING, (UINT_PTR)ID_SEND_SMS, IDS_SMS);
	AppendMenu(hSend, MF_STRING, (UINT_PTR)ID_SEND_MMS, IDS_MMS);

	AppendMenu(hSend, MF_STRING, (UINT_PTR)ID_SEND_MAIL, IDS_MAIL);

	AppendMenu(hSend, MF_STRING, (UINT_PTR)ID_SEND_BLUETOOTH, IDS_BLUETOOTH);
	
	hDeleMany = CreateMenu();
	AppendMenu(hDeleMany, MF_STRING, (UINT_PTR)ID_DELE_SELECT, IDS_DELE_SELECT);
	AppendMenu(hDeleMany, MF_STRING, (UINT_PTR)ID_DELE_ALL, IDS_DELE_ALL);
	
	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_OPEN, IDS_OPEN);
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_EDIT, IDS_EDIT);
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSend, IDS_SEND);
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_DELETE, IDS_DELETE);
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hDeleMany, IDS_DELEMANY);
	
	PDASetMenu(hNotepadFrameWnd, hMenu);
	return hMenu;
}
//////////////////////////////////////////////////////////////////////////////
// Function	CreateViewWinMenu
// Purpose	create main window's menu
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
HMENU CreateViewWinMenu(){
	HMENU hMenu, hSend;

	hSend = CreateMenu();
	AppendMenu(hSend, MF_STRING, (UINT_PTR)ID_SEND_SMS, IDS_SMS);
	AppendMenu(hSend, MF_STRING, (UINT_PTR)ID_SEND_MMS, IDS_MMS);
	AppendMenu(hSend, MF_STRING, (UINT_PTR)ID_SEND_MAIL, IDS_MAIL);
	AppendMenu(hSend, MF_STRING, (UINT_PTR)ID_SEND_BLUETOOTH, IDS_BLUETOOTH);

	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_EDIT, IDS_EDIT);
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSend, IDS_SEND);
	AppendMenu(hMenu, MF_STRING, (UINT_PTR)ID_DELETE, IDS_DELETE);
	
	PDASetMenu(hNotepadFrameWnd, hMenu);
	return hMenu;
}
//////////////////////////////////////////////////////////////////////////////
// Function	AddNewRecordToList
// Purpose	add new item in list
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void AddNewRecordToList(PTEXTINFO pInfo,  BOOL MoreLong){
	char ItemName[32];
	SYSTEMTIME systemtime;
	int i, ItemNum;
	PTEXTINFO ptmpInfo;
	char ItemTime[30];
	int lenth;
	
	GetTextFileTime(pInfo->szFileName, &systemtime);
	
	if(MoreLong)
			sprintf(ItemName, TEXT_LIST_FROMAT2, pInfo->szTextName);
		else
			sprintf(ItemName, TEXT_LIST_FROMAT, pInfo->szTextName);

	GetTimeStr(systemtime, ItemTime);
	
	ItemNum = SendMessage(hTextList, LB_GETCOUNT, NULL, NULL);
	if (ItemNum == 1) 
	{
		SendMessage(hTextList, LB_INSERTSTRING, 1, (LPARAM)ItemName);
		lenth = SendMessage(hTextList, LB_GETTEXTLEN , 1, NULL);
		SendMessage(hTextList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)hBmpNote);
		SendMessage(hTextList, LB_SETAUXTEXT, MAKEWPARAM(1, -1), (LPARAM)ItemTime);	
		SendMessage(hTextList, LB_SETITEMDATA, 1, (LPARAM)pInfo);
		SendMessage(hTextList, LB_SETCURSEL, 1, 0);
		return;
	}
	for(i = 1; i < ItemNum; i++)
	{ 
		ptmpInfo = (PTEXTINFO)SendMessage(hTextList, LB_GETITEMDATA, i, NULL);
		if(pInfo->stTime >= ptmpInfo->stTime)
		{
			SendMessage(hTextList, LB_INSERTSTRING, i, (LPARAM)ItemName);
			SendMessage(hTextList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)hBmpNote);
			SendMessage(hTextList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)ItemTime);
			SendMessage(hTextList, LB_SETITEMDATA, i, (LPARAM)pInfo);
			SendMessage(hTextList, LB_SETCURSEL, i, 0);
			break;
		}
		if (i == ItemNum -1) 
		{
			SendMessage(hTextList, LB_INSERTSTRING, ItemNum, (LPARAM)ItemName);
			SendMessage(hTextList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, ItemNum), (LPARAM)hBmpNote);
			SendMessage(hTextList, LB_SETAUXTEXT, MAKEWPARAM(ItemNum, -1), (LPARAM)ItemTime);
			SendMessage(hTextList, LB_SETITEMDATA, (WPARAM)ItemNum, (LPARAM)pInfo);
			SendMessage(hTextList, LB_SETCURSEL, ItemNum, 0);
			break;
		}
	}
	return;

}

//////////////////////////////////////////////////////////////////////////////
// Function	RemoveCurSelListItem
// Purpose	delete current list item
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void RemoveCurSelListItem(){
	int count;
	int iCurIndex;

	count = SendMessage(hTextList, LB_GETCOUNT, 0, 0);
	iCurIndex = SendMessage(hTextList, LB_GETCURSEL, 0, 0);
	if (iCurIndex < 1) 
		return;
	SendMessage(hTextList, LB_DELETESTRING, (WPARAM)iCurIndex, 0);
	if (iCurIndex == count -1) 
		SendMessage(hTextList, LB_SETCURSEL, (WPARAM)iCurIndex - 1, 0);

	if (iCurIndex < count -1 && iCurIndex > 0 ) 
		SendMessage(hTextList, LB_SETCURSEL, (WPARAM)iCurIndex, 0);

}

//////////////////////////////////////////////////////////////////////////////
// Function	NotepadMainWinCommad
// Purpose	deal WM_COMMAND message in main window
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void NotepadMainWinCommad(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	char pPath[_MAX_PATH];
	
	memset(pPath, 0, _MAX_PATH);
	curSel = SendMessage(hTextList, LB_GETCURSEL, 0, 0);

	if (curSel > 0)
		
		pCurInfo = (PTEXTINFO)SendMessage(hTextList, LB_GETITEMDATA, (WPARAM)curSel, 0);
	
	switch(LOWORD(wParam))
	{
		case ID_EXIT:
			PostMessage(hWnd, WM_CLOSE, 0, 0);		
			break;
		case ID_NEW:
 			NotepadEditWnd(NULL, FALSE, TRUE);
			break;
		case ID_OPEN:
			ViewTextWin();
			break;

		case ID_EDIT:		
			OnModifyText(FALSE);			
			break;
			
		case ID_DELETE:
			{
				char cTmp[TEXT_NAME_LENGTH + 20];
				int nIndex;
				int lenth, i;
				nIndex = SendMessage(hTextList, LB_GETCURSEL, 0, 0);
				lenth = SendMessage(hTextList, LB_GETTEXTLEN, nIndex, 0);
				SendMessage(hTextList, LB_GETTEXT, nIndex, (long)cTmp);
				i = lenth -1;
				while (cTmp[i] ==' ') 
				{ 
				    cTmp[i] = '\0';
					i--;
				}		
				strcat(cTmp, ":\n");
				strcat(cTmp, ML("Delete?"));
				PLXConfirmWinEx(hNotepadFrameWnd, hWnd, cTmp, Notify_Request, IDS_NOTEPAD, IDS_YES, IDS_NO, CONFIRM_DELETE);
			}
			break;

		case ID_DELE_SELECT:
			TextDeleteWin();
			break;
		case ID_DELE_ALL:
			PLXConfirmWinEx(hNotepadFrameWnd, hWnd, IDS_DELALLCONFIRM, Notify_Request, IDS_NOTEPAD, IDS_YES, IDS_NO, CONFIRM_DELETEALL);
			break;
		
		case ID_SEND_MMS:
			OnSendMMS(hWnd);
			break;

		case ID_SEND_MAIL:
			OnSendMail(hNotepadFrameWnd);
			break;

		case ID_SEND_SMS:
			OnSendSMS(hNotepadFrameWnd);
			break;
			
		case ID_SEND_BLUETOOTH:
			OnSendBlueTooth();
			break;
	}
}
//////////////////////////////////////////////////////////////////////////////
// Function	NotepadViewWinCommad
// Purpose	deal WM_COMMAND message in main window
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void NotepadViewWinCommad(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	char pPath[_MAX_PATH];
	
	memset(pPath, 0, _MAX_PATH);
	curSel = SendMessage(hTextList, LB_GETCURSEL, 0, 0);
	
	if (curSel > 0)
		
		pCurInfo = (PTEXTINFO)SendMessage(hTextList, LB_GETITEMDATA, (WPARAM)curSel, 0);
	
	switch(LOWORD(wParam))
	{
		
	case ID_EDIT:
		
			OnModifyText(TRUE);
		
		break;
		
	case ID_DELETE:
		{
			char cTmp[TEXT_NAME_LENGTH + 20];
			int nIndex;
			int lenth, i;
			nIndex = SendMessage(hTextList, LB_GETCURSEL, 0, 0);
			lenth = SendMessage(hTextList, LB_GETTEXTLEN, nIndex, 0);
			SendMessage(hTextList, LB_GETTEXT, nIndex, (long)cTmp);
			i = lenth -1;
			while (cTmp[i] ==' ') 
			{ 
				cTmp[i] = '\0';
				i--;
			}			
			strcat(cTmp, ":\n");
			strcat(cTmp, ML("Delete?"));
			PLXConfirmWinEx(hNotepadFrameWnd, hWnd, cTmp, Notify_Request, IDS_NOTEPAD, IDS_YES, IDS_NO, CONFIRM_DELETE);
		}
		break;
		
	case ID_SEND_MMS:
		OnSendMMS(hWnd);
		break;
		
	case ID_SEND_MAIL:
		OnSendMail(hNotepadFrameWnd);
		break;
		
	case ID_SEND_SMS:
		OnSendSMS(hNotepadFrameWnd);
		break;
		
	case ID_SEND_BLUETOOTH:
		OnSendBlueTooth();
		break;
	}
}




//////////////////////////////////////////////////////////////////////////////
// Function	OnModifyText
// Purpose	deal to open text
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void OnModifyText(BOOL isViewWnd)
{
	char *pContent;
	
	if(NULL == pCurInfo){
		return;
	}
	pContent = (char *)malloc((pCurInfo->dwTextLength +1)* sizeof(char));
	if (NULL == pContent) 
	{
		return;
	}
	if(ReadText(pCurInfo, pContent, pCurInfo->dwTextLength) == -1)
		return;

	NotepadEditWnd(pContent, isViewWnd, FALSE);
	if (NULL != pContent)
	{
		free(pContent);
		pContent = NULL;
	}
	return;
}


//////////////////////////////////////////////////////////////////////////////
// Function	OnDelete
// Purpose	处理删除
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void OnDelete()
{
	DeleteText(pCurInfo);
	RemoveCurSelListItem();	
}

//////////////////////////////////////////////////////////////////////////////
// Function	OnDeleteAll
// Purpose	delete all notepad
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void OnDeleteAll(){
	int ItemNum;
	PTEXTINFO pInfo;
	int i;

	ItemNum = SendMessage(hTextList, LB_GETCOUNT, NULL, NULL);
	if (1 == ItemNum )
		return;

	for (i = 1; i < ItemNum ; i++)
	{
		pInfo = (PTEXTINFO)SendMessage(hTextList, LB_GETITEMDATA, (WPARAM)i, 0);
		if(NULL == pInfo)
		{
			PLXTipsWin(0, 0, 0,IDS_DELFAIL, NULL, Notify_Failure, NULL, NULL, WAITTIMEOUT);
			return;
		}
		if(!DeleteText(pInfo))
		{
			PLXTipsWin(0, 0, 0, IDS_DELFAIL, NULL, Notify_Failure, NULL, NULL, WAITTIMEOUT);
			return;
		}
	}
	ReListTextRecord();	
	
}

//////////////////////////////////////////////////////////////////////////////
// Function	TextDeleteWin
// Purpose	create deleting main window 
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
DWORD TextDeleteWin()
{
	WNDCLASS wc;
	RECT rClient;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.hCursor = NULL;
	wc.hIcon = NULL;
	wc.hInstance = NULL;
	wc.lpfnWndProc = NotepadDeleteWinProc;
	wc.lpszClassName = "NotepdaDeleteWinClass";
	wc.lpszMenuName = NULL;
	wc.style = 0;
	
	if(!RegisterClass(&wc))
		return FALSE;
	
	GetClientRect(hNotepadFrameWnd, &rClient);
	CreateWindow(
		"NotepdaDeleteWinClass",
		"",
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hNotepadFrameWnd,
		NULL,
		NULL,
		NULL
		);
	ShowWindow(hNotepadFrameWnd, SW_SHOW);
	UpdateWindow(hNotepadFrameWnd);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// Function	NotepadDeleteWinProc
// Purpose	deleting window function
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK NotepadDeleteWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	LRESULT lRet = TRUE;

	switch(message){
	
	case WM_CREATE:
		NotepadDeleteWinCreate(hWnd);
		SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_DELETE);
		SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SetWindowText(hNotepadFrameWnd, IDS_NULTISELECT);
		SetFocus(hDelList);
		break;

	case PWM_SHOWWINDOW:
		SetFocus(hDelList);
		SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_DELETE);
		SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);
		SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		SetWindowText(hNotepadFrameWnd, IDS_NULTISELECT);
		break;

	case WM_COMMAND:
		NotepadDeleteWinCommand(hWnd, wParam, lParam);
		break;
	case CONFIRM_DELETE:
		if (lParam)
		{
			DoTextFileDeletion(hWnd);
			PLXTipsWin(hNotepadFrameWnd, hWnd, CONFIRM_DELETED, IDS_DELSUCCESS, IDS_NULTISELECT, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
		}
		else
		{
			SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
			
		}
		break;
	case CONFIRM_DELETED:
		if (lParam) 
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			ReListTextRecord();
			InvalidateRect(hTextList, NULL, TRUE);
		}
		break;
	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_RETURN:
			SendMessage(hWnd, WM_COMMAND, ID_DELETE, 0);
			break;
		case VK_F10:
			SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
			break;
		default:
			lRet = PDADefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
		break;	
	case WM_CLOSE:	
		DestroyWindow(hWnd);	
		break;
	case WM_DESTROY:
		hDelList = NULL;	
		UnregisterClass("NotepdaDeleteWinClass", NULL);
		SendMessage(hNotepadFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		break;

	default:
		lRet = PDADefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return lRet;
}

//////////////////////////////////////////////////////////////////////////////
// Function	NotepadDeleteWinCreate
// Purpose	deal WM_CREATE message in deleting main window
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void NotepadDeleteWinCreate(HWND hWnd){
	PTEXTINFO pInfo;
	RECT rccl;

	GetClientRect(hWnd, &rccl);
	hDelList = CreateWindow("MULTILISTBOX",
		IDS_NULTISELECT,
		WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_SORT| LBS_BITMAP,
		rccl.left, rccl.top,
		rccl.right - rccl.left, rccl.bottom - rccl.top,
		hWnd,
		(HMENU)ID_DELETELIST,
		NULL,
		NULL);

	if(MoveToFirstTextNode()){
		do {	
			pInfo = GetCurTextInfo();
			if (pInfo->dwTextLength > TEXT_NAME_LENGTH  -1)
				AddNewRecordToDeleList(pInfo, 1);
			else 
				AddNewRecordToDeleList(pInfo, 0);
		} while(MoveToNextTextNode());
		
		SendMessage(hDelList, LB_ENDINIT, 0, 0);
		SendMessage(hDelList, LB_SETCURSEL, 0, 0);
		
	}
}

//////////////////////////////////////////////////////////////////////////////
// Function	NotepadDeleteWinCommand
// Purpose	deal WM_COMMAND message in deleting main window
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void NotepadDeleteWinCommand(HWND hWnd, WPARAM wParam, LPARAM lParam){

	switch(LOWORD(wParam))
	{
	case ID_CANCEL:
		PostMessage(hWnd, WM_CLOSE, 0, 0);	
		break;
	case ID_DELETE:
		{
			int  iSelectedNum = 0;
			int nCount, i;

			nCount = SendMessage(hDelList, LB_GETCOUNT, 0, 0);
			if(nCount > 0)
			{	
				for(i = 0; i < nCount; i++)
				{
					if(SendMessage(hDelList, LB_GETSEL, (WPARAM)i, 0))
						iSelectedNum ++;
				}
			}
			if (iSelectedNum == 0) 
				PLXTipsWin(hNotepadFrameWnd, hWnd, 0, IDS_NOSELECT, IDS_NULTISELECT,  Notify_Alert, IDS_OK,  NULL, WAITTIMEOUT);
			else if (iSelectedNum > 1)
			{
				PLXConfirmWinEx(hNotepadFrameWnd, hWnd, IDS_DELCONFIRM, Notify_Request, IDS_NULTISELECT, IDS_YES, IDS_NO, CONFIRM_DELETE);
			}
			else if (iSelectedNum == 1)
			{
				PLXConfirmWinEx(hNotepadFrameWnd, hWnd, ML("Delete selected note?"), Notify_Request, IDS_NULTISELECT, IDS_YES, IDS_NO, CONFIRM_DELETE);
			}
				
		}	
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
// Function	DoTextFileDeletion
// Purpose	delete selected file
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void DoTextFileDeletion(HWND hWnd){
	PTEXTINFO pInfo;
	int nCount, i;

	nCount = SendMessage(hDelList, LB_GETCOUNT, 0, 0);
	if(nCount > 0){	
		for(i = 0; i < nCount; i++){
			if(SendMessage(hDelList, LB_GETSEL, (WPARAM)i, 0)){
				pInfo = (PTEXTINFO)SendMessage(hDelList, LB_GETITEMDATA, (WPARAM)i, 0);
				DeleteText(pInfo);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Function	ReListTextRecord
// Purpose	relocate item in listbox
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void ReListTextRecord(){
	PTEXTINFO pInfo;
//	SYSTEMTIME SystemTime;
//	char cDate_Time[20];
	
	SendMessage(hTextList, LB_RESETCONTENT, 0, 0);
	SendMessage(hTextList, LB_ADDSTRING, 0 , (LPARAM)IDS_NEWNOTE);
//	GetLocalTime(&SystemTime);
//	GetTimeStr(SystemTime, cDate_Time);
//	SendMessage(hTextList, LB_SETAUXTEXT, MAKEWPARAM(0, -1), (LPARAM)cDate_Time);
	SendMessage(hTextList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)hBmpNewNote);

	if(MoveToFirstTextNode()){
		do {	
			pInfo = GetCurTextInfo();
			if (pInfo->dwTextLength > TEXT_NAME_LENGTH -1)
				AddNewRecordToList(pInfo, 1);
			else 
				AddNewRecordToList(pInfo, 0);
		} while(MoveToNextTextNode());
	}
	SendMessage(hTextList, LB_SETCURSEL, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////
// Function	
// Purpose	
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
//DWORD ViewTextWin(PTEXTINFO pInfo)
DWORD ViewTextWin(void)
{
	WNDCLASS wc;
	RECT rClient;


	if(IsWindow(hWndViewTxt))
	{
		ShowWindow(hNotepadFrameWnd, SW_SHOW);
		UpdateWindow(hNotepadFrameWnd);
	}
	else
	{	
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = GetStockObject(WHITE_BRUSH);
		wc.hCursor = NULL;
		wc.hIcon = NULL;
		wc.hInstance = NULL;
		wc.lpfnWndProc = NotepadViewWinProc;
		wc.lpszClassName = "NotepdaViewWinClass";
		wc.lpszMenuName = NULL;
		wc.style = 0;

		if(!RegisterClass(&wc))
			return (FALSE);
	
		GetClientRect(hNotepadFrameWnd,&rClient);
		hWndViewTxt = CreateWindow(
			"NotepdaViewWinClass",
			"",//IDS_VIEWTITLE,
			WS_VISIBLE | WS_CHILD,
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
			hNotepadFrameWnd,
			NULL,
			NULL,
			NULL);		
		
		if(NULL == hWndViewTxt)
			return (FALSE);
	}
	
	ShowWindow(hNotepadFrameWnd,SW_SHOW);
	UpdateWindow(hNotepadFrameWnd);
	return (TRUE);
}

//////////////////////////////////////////////////////////////////////////////
// Function	NotepadViewWinProc
// Purpose	
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK NotepadViewWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = TRUE;
	static HMENU hMenu;
	static char *pContent;
	static long selindex;
	static int total;
	static HWND hViewCtrl;


	
	switch(msg)
	{
	case WM_CREATE:		
	
		total = SendMessage(hTextList, LB_GETCOUNT, 0, 0);
		selindex = SendMessage(hTextList, LB_GETCURSEL, 0, 0);	
		pCurInfo = (PTEXTINFO)SendMessage(hTextList, LB_GETITEMDATA, (WPARAM)selindex, 0);
		pContent = (char *)malloc((pCurInfo->dwTextLength +1) * sizeof(char));
		if (NULL == pContent) 
		{
			printf("NotepadViewWinProc WM_CREATE malloc failed!!!\r\n");
			break;
		}
		hMenu = CreateViewWinMenu();		
		ReadText(pCurInfo, pContent, pCurInfo->dwTextLength);		
		hViewCtrl = PlxTextView(hNotepadFrameWnd, hWnd, pContent , strlen(pContent), FALSE, NULL, NULL, 0);
		SetFocus(hViewCtrl);
		SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
		SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		RefreshViewArrow(selindex, total);
		SetWindowText(hNotepadFrameWnd,IDS_VIEWTITLE);
		break;
		
	case PWM_SHOWWINDOW:
		SetFocus(hViewCtrl);
		PDASetMenu(hNotepadFrameWnd, hMenu);
		SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
		SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		selindex = SendMessage(hTextList, LB_GETCURSEL, 0, 0);
		total = SendMessage(hTextList, LB_GETCOUNT, 0, 0);
		RefreshViewArrow(selindex, total);	
		SetWindowText(hNotepadFrameWnd,IDS_VIEWTITLE);
		
		break;
	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;
	case VIEW_SAVE_OK:
		
		selindex = SendMessage(hTextList, LB_GETCURSEL, 0, 0);
		total = SendMessage(hTextList, LB_GETCOUNT, 0, 0);
		if(hViewCtrl)
			DestroyWindow(hViewCtrl);
		if (NULL != pContent) 
		{
			free(pContent);
			pContent = NULL;
		}	
		pCurInfo = (PTEXTINFO)SendMessage(hTextList, LB_GETITEMDATA, (WPARAM)selindex, 0);
		
		pContent = (char *)malloc((pCurInfo->dwTextLength +1) * sizeof(char));
		if (NULL == pContent) 
		{
			PLXConfirmWinEx(hNotepadFrameWnd, 0, IDS_NOFREESPACE, Notify_Failure, 
				IDS_NOTEPAD, ML("Ok"), 0, 0);
			break;
		}
		ReadText(pCurInfo, pContent, pCurInfo->dwTextLength);
		hViewCtrl = PlxTextView(hNotepadFrameWnd, hWnd, pContent , strlen(pContent), FALSE, NULL, NULL, 0);
		RefreshViewArrow(selindex, total);

		SetFocus(hViewCtrl);
		
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{	
		case VK_LEFT:				
			
			if (selindex > 1)
			{
				selindex--;
			    if(hViewCtrl)
					DestroyWindow(hViewCtrl);
				if (NULL != pContent) 
				{
					free(pContent);
					pContent = NULL;
				}
					SendMessage(hTextList, LB_SETCURSEL, (WPARAM)selindex, 0);
				pCurInfo = (PTEXTINFO)SendMessage(hTextList, LB_GETITEMDATA, (WPARAM)selindex, 0);
			
				pContent = (char *)malloc((pCurInfo->dwTextLength +1) * sizeof(char));
				if (NULL == pContent) 
				{
					PLXConfirmWinEx(hNotepadFrameWnd, 0, IDS_NOFREESPACE, Notify_Failure, 
						IDS_NOTEPAD, ML("Ok"), 0, 0);
					break;
				}
				ReadText(pCurInfo, pContent, pCurInfo->dwTextLength);
				hViewCtrl = PlxTextView(hNotepadFrameWnd, hWnd, pContent , strlen(pContent), FALSE, NULL, NULL, 0);
				RefreshViewArrow(selindex, total);
				SetFocus(hViewCtrl);

			}
			
			
			break;
		case VK_RIGHT:			
			
			if (selindex <(total - 1))
			{
				selindex++;
				if(hViewCtrl)
					DestroyWindow(hViewCtrl);
				if (NULL != pContent) 
				{
					free(pContent);
					pContent = NULL;
				}
				SendMessage(hTextList, LB_SETCURSEL, (WPARAM)selindex, 0);
				selindex = SendMessage(hTextList, LB_GETCURSEL, 0, 0);
				pCurInfo = (PTEXTINFO)SendMessage(hTextList, LB_GETITEMDATA, (WPARAM)selindex, 0);
				
				pContent = (char *)malloc((pCurInfo->dwTextLength +1) * sizeof(char));
				if (NULL == pContent) 
				{
					PLXConfirmWinEx(hNotepadFrameWnd, 0, IDS_NOFREESPACE, Notify_Failure, 
						IDS_NOTEPAD, ML("Ok"), 0, 0);
					break;
				}
				ReadText(pCurInfo, pContent, pCurInfo->dwTextLength);
				hViewCtrl = PlxTextView(hNotepadFrameWnd, hWnd, pContent , strlen(pContent), FALSE, NULL, NULL, 0);
				RefreshViewArrow(selindex, total);
				SetFocus(hViewCtrl);
			}
		
			break;
		case VK_F10:	
		
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		
			break;
		case VK_F5:
			SendMessage(hNotepadFrameWnd, msg, wParam, lParam);
			break;
		case VK_RETURN:		
			SendMessage(hWnd, WM_COMMAND, ID_EDIT, 0);		
			break;
		default:
			lRet = PDADefWindowProc(hWnd, msg, wParam, lParam);
			break;
		}
		break;
	case WM_COMMAND:		
			NotepadViewWinCommad(hWnd, wParam, lParam);
		break;
	case CONFIRM_DELETE:
		if (lParam)
		{
			OnDelete();
			PLXTipsWin(hNotepadFrameWnd, hWnd, CONFIRM_DELETE_OK, IDS_DELSUCCESS, IDS_NOTEPAD, Notify_Success,  IDS_OK, NULL, WAITTIMEOUT);
		}		
		break;
	case CONFIRM_DELETE_OK:
		if (lParam)
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		
		}
		break;
	case WM_CLOSE:		
			DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		if(hViewCtrl)
		{
			DestroyWindow(hViewCtrl);
			hViewCtrl = NULL;
		}
		
		if (NULL != pContent) 
		{
			free(pContent);
			pContent = NULL;
		}
		if(hMenu)
		{
			DestroyMenu(hMenu);
			hMenu = NULL;
		}
		
		UnregisterClass("NotepdaViewWinClass", NULL);
		hWndViewTxt = NULL;
		SendMessage(hNotepadFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		break;
	default:
		lRet = PDADefWindowProc(hWnd, msg, wParam, lParam);
		break;
	}
	return (lRet);
}

//////////////////////////////////////////////////////////////////////////////
// Function	OnSendSMS
// Purpose	send SMS
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void OnSendSMS(HWND hWnd)
{
	char *pContent;

	if(NULL == pCurInfo)
		return;
	pContent = (char *)malloc((pCurInfo->dwTextLength +1) *sizeof(char));
	if (NULL == pContent) 
	{
		PLXConfirmWinEx(hNotepadFrameWnd, hWnd, IDS_NOFREESPACE, Notify_Failure, 
			IDS_NOTEPAD, ML("Ok"), 0, 0);
		return;
	}
	ReadText(pCurInfo, pContent, pCurInfo->dwTextLength);
	APP_EditSMS(hWnd, NULL, pContent);
	if (NULL !=pContent)
	{
		free(pContent);
		pContent = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////
// Function	OnSendMMS
// Purpose	send MMS
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void OnSendMMS(HWND hWnd)
{

	if(NULL == pCurInfo)
		return;
	APP_EditMMS(hNotepadFrameWnd,hWnd, 0, MMS_CALLEDIT_TEXT, pCurInfo->szFileName);

}

//////////////////////////////////////////////////////////////////////////////
// Function	OnSendMail
// Purpose	send mail
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void OnSendMail(HWND hWnd)
{
	char cAccessoryName[TEXT_NAME_LENGTH +10];
	SYSTEMTIME systime;

	GetLocalTime(&systime);
	if(NULL == pCurInfo)
		return;
	sprintf(cAccessoryName, "%s.%s", pCurInfo->szTextName, TEXT_FILE_SUFFIX);
	MAIL_CreateEditInterface(hNotepadFrameWnd, NULL, pCurInfo->szFileName, cAccessoryName, 1);
}

//////////////////////////////////////////////////////////////////////////////
// Function	OnSendBlueTooth
// Purpose	send BlueTooth
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void OnSendBlueTooth()
{
	char cViewFileName[TEXT_NAME_LENGTH +10];

	if(NULL == pCurInfo)
		return;

	sprintf(cViewFileName, "%s.txt", pCurInfo->szTextName);
	BtSendData(hNotepadFrameWnd, pCurInfo->szFileName, cViewFileName, 1);
}

//////////////////////////////////////////////////////////////////////////////
// Function	AddNewRecordToDeleList
// Purpose	
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void AddNewRecordToDeleList(PTEXTINFO pInfo,  BOOL MoreLong)
{
	char ItemName[32];
	SYSTEMTIME systemtime;
	int i, ItemNum;
	PTEXTINFO ptmpInfo;

	GetTextFileTime(pInfo->szFileName, &systemtime);
	if(MoreLong)
		sprintf(ItemName, TEXT_LIST_FROMAT2, pInfo->szTextName);
	else
		sprintf(ItemName, TEXT_LIST_FROMAT, pInfo->szTextName);

	ItemNum = SendMessage(hDelList, LB_GETCOUNT, NULL, NULL);
	if (ItemNum == 0) 
	{
		SendMessage(hDelList, LB_INSERTSTRING, 0, (LPARAM)ItemName);
		SendMessage(hDelList, LB_SETITEMDATA, 0, (LPARAM)pInfo);
		return;
	}
	for(i = 0; i < ItemNum; i++)
	{ 
		ptmpInfo = (PTEXTINFO)SendMessage(hDelList, LB_GETITEMDATA, i, NULL);
		if(pInfo->stTime >= ptmpInfo->stTime)
		{
			SendMessage(hDelList, LB_INSERTSTRING, i, (LPARAM)ItemName);
			SendMessage(hDelList, LB_SETITEMDATA, i, (LPARAM)pInfo);
			break;
		}
		if (i == ItemNum -1) 
		{
			SendMessage(hDelList, LB_INSERTSTRING, ItemNum, (LPARAM)ItemName);
			SendMessage(hDelList, LB_SETITEMDATA, (WPARAM)ItemNum, (LPARAM)pInfo);
			break;
		}
	}	
}
DWORD NotepadEditWnd(char *pContent, BOOL bIsViewWnd, BOOL bIsNewNote)
{
	WNDCLASS wc;
    DWORD dwRet;
	HWND hEditWnd;
	RECT rClient;
	EDITWNDUSED data;

	data.bIsViewWnd = bIsViewWnd;
	data.pContent = pContent;
	data.bIsNewNote = bIsNewNote;

	if (NULL  != pContent)
		data.lenth = strlen(pContent);
	else
		data.lenth = 0;

    dwRet = (DWORD)TRUE;
	
	wc.style         = 0;
	wc.lpfnWndProc   = EditWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof(data);
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "NotepadEditWndClass";

	RegisterClass(&wc);

	GetClientRect(hNotepadFrameWnd,&rClient);
		
	hEditWnd = CreateWindow("NotepadEditWndClass",
		"",
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hNotepadFrameWnd,
		NULL,
		NULL,
		(PVOID)&data
		);

	if (!hEditWnd) 
	{
		return FALSE;
	}

	ShowWindow(hNotepadFrameWnd,SW_SHOW);
	UpdateWindow(hNotepadFrameWnd);
    return dwRet;
}
static LRESULT EditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;
	EDITWNDUSED *pData;
	static char *pOldContent = NULL;
	
	pData = GetUserData(hWnd);

    switch (wMsgCmd)
    {
		case WM_CREATE:	
			{
				LPCREATESTRUCT lpCreateStruct;
				
				lpCreateStruct = (LPCREATESTRUCT)lParam;
				memcpy(pData, lpCreateStruct->lpCreateParams, sizeof(EDITWNDUSED));
				
				if (pData->bIsNewNote)
					CreateEdit(hWnd, NULL,TRUE);
				else			
				{
					CreateEdit(hWnd, pData->pContent,FALSE);
					pOldContent = malloc(pData->lenth * sizeof(char) +1);				
					if(NULL != pOldContent &&  NULL != pData->pContent)
						strcpy(pOldContent, pData->pContent);
				}
				
				if (pData->bIsNewNote) 
					SetWindowText(hNotepadFrameWnd, IDS_NEWNOTE);
				else
					SetWindowText(hNotepadFrameWnd, IDS_MODIFYBOOK);
				
				SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
				SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
				SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);
				SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
				SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				SetFocus(hImeTextNotepad);
			}
			break;
		case PWM_SHOWWINDOW:
			
			if (pData->bIsNewNote) 
				SetWindowText(hNotepadFrameWnd, IDS_NEWNOTE);
			else
				SetWindowText(hNotepadFrameWnd, IDS_MODIFYBOOK);
			
			SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
			SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
			SendMessage(hNotepadFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);
			SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
			SetFocus(hImeTextNotepad);
			
			break;
		case WM_PAINT:
			BeginPaint(hWnd, NULL);			
			EndPaint(hWnd, NULL);
			break;			

		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_F10:
				SendMessage(hWnd, WM_COMMAND, ID_CANCEL, 0);
				break;
			case VK_RETURN:
				SendMessage(hWnd, WM_COMMAND, ID_SAVE, 0);
				break;
			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
			}
			break;
		case CONFIRM_SAVE:
			if (lParam)
				SendMessage(hWnd, WM_COMMAND, ID_SAVE, 0);
			PostMessage(hWnd, WM_CLOSE, 0, 0);			
			break;	
		case EDIT_SAVE_OK:
			PostMessage(hWnd, WM_CLOSE, 0, 0);		
			if (pData->bIsViewWnd)
				SendMessage(hWndViewTxt, VIEW_SAVE_OK, 0, 0);
			break;
		case NEW_SAVE_OK:
			PostMessage(hWnd, WM_CLOSE, 0, 0);			
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case ID_CANCEL:
				{
					int lenth;
					char *pNewContent;
					
					lenth = GetWindowTextLength(hImeTextNotepad);
					if (lenth > 0 && pData->bIsNewNote)
					{
						PLXConfirmWinEx(hNotepadFrameWnd, hWnd, IDS_SAVENBOOK, Notify_Info, 
							IDS_NOTEPAD, IDS_YES, IDS_NO, CONFIRM_SAVE);
					}
					else
					{
						if (lenth > 0 && !pData->bIsNewNote) 
						{
							pNewContent = (char *)malloc(lenth* sizeof(char) +1);
							if (NULL != pNewContent && NULL != pOldContent) 
							{
								GetWindowText(hImeTextNotepad, pNewContent, lenth +1);
								if (strcmp(pNewContent, pOldContent) != 0) 
									PLXConfirmWinEx(hNotepadFrameWnd, hWnd, IDS_SAVENBOOK, Notify_Info, 
									IDS_NOTEPAD, IDS_YES, IDS_NO, CONFIRM_SAVE);
								else
								{
									PostMessage(hWnd, WM_CLOSE, 0, 0);														
								
								}
								if (NULL != pNewContent) 
								{
									free(pNewContent);
									pNewContent = NULL;
								}
							}
							else
							{
								PostMessage(hWnd, WM_CLOSE, 0, 0);
							
							}
						
						}
						else
						{
							PostMessage(hWnd, WM_CLOSE, 0, 0);					
							
						}
					}
				}
				
				break;
			case ID_SAVE:
				{
					int lenth;
					char *pTmpContent;
					SYSTEMTIME FileTime;
					char NewFileName[TEXT_NAME_LENGTH +10];
					BOOL MoreLong = FALSE;
					TEXTINFO Info;
					PTEXTINFO pInfo;
					
					lenth = GetWindowTextLength(hImeTextNotepad);
					if (0 == lenth) 
					{
					
						if(pData->bIsNewNote)					
							PLXTipsWin(hNotepadFrameWnd, hWnd, NULL, IDS_WRITE,  IDS_NEWNOTE, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
						else
							PLXTipsWin(hNotepadFrameWnd, hWnd, NULL, IDS_WRITE,  IDS_MODIFYBOOK, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
						break;
					}
					pTmpContent = (char *)malloc(lenth * sizeof(char) +1);
					if (NULL == pTmpContent) 
						break;
					if(!IsEnoughFreeSpaceToSaveText(lenth))
					{
						PLXConfirmWinEx(hNotepadFrameWnd, hWnd, IDS_NOFREESPACE, Notify_Failure, 
							IDS_NOTEPAD, ML("Ok"), 0, 0);
						if (NULL != pTmpContent)
						{
							free(pTmpContent);
							pTmpContent = NULL;
						}
						break;
					}
			
					GetWindowText(hImeTextNotepad, pTmpContent, lenth +1);
					
					GetLocalTime(&FileTime);
					memset(NewFileName, 0, TEXT_NAME_LENGTH +10);
					sprintf(NewFileName, "%04d%02d%02d%02d%02d%02d", 
						FileTime.wYear, FileTime.wMonth, FileTime.wDay,
						FileTime.wHour, FileTime.wMinute, FileTime.wSecond);
					strcat(NewFileName, ".txt");				
					
				
					if(strlen(pTmpContent) > TEXT_NAME_LENGTH  -1)
						MoreLong = TRUE;	
					
				
					if(pData->bIsNewNote)
					{
						pInfo = SaveText(&Info, pTmpContent, NewFileName, TEXT_PATH_FLASH, SAVE_MODE_NEW);
						if(pInfo)
						{						
							PLXTipsWin(hNotepadFrameWnd, hWnd, NEW_SAVE_OK, ML("Saved"), IDS_NEWNOTE, Notify_Success, ML("Ok"), 0, WAITTIMEOUT);
							
							if(hTextList)
								AddNewRecordToList(pInfo, MoreLong);
						}
					
					}
					else
					{	

						pCurInfo = SaveText(pCurInfo, pTmpContent, NewFileName,TEXT_PATH_FLASH, SAVE_MODE_OVERWRITE);
						if(pCurInfo)
						{
							PLXTipsWin(hNotepadFrameWnd, hWnd, EDIT_SAVE_OK, ML("Saved"), IDS_MODIFYBOOK, Notify_Success, ML("Ok"), 0, WAITTIMEOUT);//hNotepadFrameWnd
							if(hTextList)
							{
								SendMessage(hTextList, LB_DELETESTRING, curSel, 0);							
								AddNewRecordToList(pCurInfo, MoreLong);
							}
						}
					
					
					
					}
					if (NULL != pTmpContent)
					{
						free(pTmpContent);
						pTmpContent = NULL;
					}
					
				}
				break;
			default:
				break;
			}
			break;

		case WM_CLOSE:
		
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
		
			if (NULL != pOldContent) 
			{
				free(pOldContent);
				pOldContent = NULL;
			}
			UnregisterClass("NotepadEditWndClass", NULL);      
			SendMessage(hNotepadFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			break;
	
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
    }

    return lResult;
}
//////////////////////////////////////////////////////////////////////////////
// Function	OnEditText
// Purpose	edit text
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static void CreateEdit(HWND hWnd,char *pContent, BOOL bIsNewNote)
{
	IMEEDIT ie;
	int bSpace = 0;
	RECT rClient;

	memset(&ie, 0, sizeof(IMEEDIT));
	ie.hwndNotify	= (HWND)hWnd;    
	ie.dwAttrib	= NULL;
	ie.pszImeName	= NULL;
	ie.uMsgSetText = IME_MSG_TEXTEXIT;

	if(bIsNewNote)
		ie.pszTitle = (char *)IDS_NEWNOTE;
	else
		ie.pszTitle = (char *)IDS_MODIFYBOOK;

	GetClientRect(hWnd, &rClient);

	hImeTextNotepad = CreateWindow(
		 "IMEEDIT",
		 "",
		 WS_VISIBLE | WS_CHILD| ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
		 rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,             
		 (HWND)hWnd,
		 NULL,
		 NULL,
		 (PVOID)&ie);

	SendMessage(hImeTextNotepad, EM_LIMITTEXT, NOTEPAD_MAX_CONTENT, 0);
	if (NULL != pContent)
	{
		SetWindowText(hImeTextNotepad, pContent);
	}
	SendMessage(hImeTextNotepad, EM_SETSEL, -1, -1);
	return;
}
//////////////////////////////////////////////////////////////////////////////
// Function	RefreshViewArrow
// Purpose	change view arrow state
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void RefreshViewArrow(int selindex, int total)
{
	if(total > 2)
	{
		if(selindex <= total - 1 && selindex >= 1)
		{
			if(selindex == (total - 1))
			{
			
				SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)NOTEPAD_ICON_ARROWLEFT);
					SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);
			}
			else if(selindex == 1)
			{
			
				SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);	
					SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)NOTEPAD_ICON_ARROWRIGHT);
			}
			else
			{
				SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)NOTEPAD_ICON_ARROWRIGHT);
				SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)NOTEPAD_ICON_ARROWLEFT);
				
			}		
			
		}
	}
	else
	{
		SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 0);
		SendMessage(hNotepadFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);
		
	}

}

//////////////////////////////////////////////////////////////////////////////
// Function	SaveFileToNotepad
// Purpose	save *.txt to notepad and refresh notepad file list
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void SaveFileToNotepad(char *pFileName, char *pDesPath)
{
	char szPathFilename[_MAX_PATH];
	struct stat buff;	
	PTEXTINFO pInfo;
	TEXTINFO Info;
	BOOL MoreLong;
	int oldselindex = 0;
	
	if(hTextList == NULL)
		return;
	
	sprintf(szPathFilename, "%s%s", pDesPath, pFileName);//".", TEXT_FILE_SUFFIX
	
	if(!GetTextFileInfo(szPathFilename, &buff))
		return ;	
	Info.stTime = buff.st_mtime;
	Info.dwTextLength = buff.st_size;
	sprintf(Info.szFileName, "%s" , pFileName);	
	
	
	if(ReadText(&Info, Info.szTextName, TEXT_NAME_LENGTH) > -1)
		AddTextNode(&Info);
	pInfo = GetCurTextInfo();
	if(pInfo->dwTextLength > TEXT_NAME_LENGTH  -1)
		MoreLong = TRUE;
	oldselindex = SendMessage(hTextList, LB_GETCURSEL, 0, 0);
	
	AddNewRecordToList(pInfo, MoreLong);
	//当前选中的文件需要与新保存的文件进行时间的比较，再决定是否oldselindex + 1 or -1 or 不变
	if(oldselindex == 0)
		SendMessage(hTextList, LB_SETCURSEL, 0, 0);	
	else
		SendMessage(hTextList, LB_SETCURSEL, (oldselindex + 1), 0);	
	
	if(hDelList )
	{
		oldselindex = SendMessage(hDelList, LB_GETCURSEL, 0, 0);
		AddNewRecordToDeleList(pInfo, MoreLong);		
		SendMessage(hDelList, LB_SETCURSEL, (oldselindex + 1), 0);	
		
	}
	
	
	
}
