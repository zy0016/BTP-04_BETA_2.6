/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#include "PWBE.h"
#include "wUipub.h"
#include "stdlib.h"
#include "string.h"
#include "plx_pdaex.h"
#include "winpda.h"
#include "imesys.h"
//#include "JamInstall.h"
#include "AuthProc.h"

#define IDM_CONFIRM				1
#define IDM_EXIT				2
#define IDM_EXITLOOP			3

#define CONTROL_WIDTH           150
#define CONTROL_HEIGHT          ((176/5) - 5)
#define CONTROL_LEFT            10

#define AUTH_AUTHTEXT_X			CONTROL_LEFT
#define AUTH_AUTHTEXT_Y			0

#define AUTH_AUTHID_X			CONTROL_LEFT
#define AUTH_AUTHID_Y			(CONTROL_HEIGHT * 1)
#define AUTH_AUTHID_WIDTH		CONTROL_WIDTH
#define AUTH_AUTHID_HEIGHT		CONTROL_HEIGHT

#define AUTH_PWDTEXT_X			CONTROL_LEFT
#define AUTH_PWDTEXT_Y			(CONTROL_HEIGHT * 2)

#define AUTH_PWD_X				CONTROL_LEFT
#define AUTH_PWD_Y				(CONTROL_HEIGHT * 3)
#define AUTH_PWD_WIDTH			CONTROL_WIDTH
#define AUTH_PWD_HEIGHT			CONTROL_HEIGHT

static char *pName;
static int  nMaxNameLen;
static char *pPsw;
static int  nMaxPswLen;
static const char *PSHOWREALM;
static int  nShowRealmLen;
static const char *PSHOWROOT;
static int  nShowRootLen;
static int  nType;
static BOOL bResult = FALSE;

extern HFONT hViewFont;

static BOOL WIE_CreateAuthInput(void);
static LRESULT CALLBACK WIE_AuthWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK WIE_PermitWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

BOOL WIE_WapAuth(char *szName, int nMaxName, char *szPsw, int nMaxPsw,
				 const char *PREALM, int nRealmLen, const char *PROOT, int Rootlen, int type)
{
	pName = szName;
	pPsw  = szPsw;
	nMaxNameLen = nMaxName;
	nMaxPswLen  = nMaxPsw;

	PSHOWREALM = PREALM;
	nShowRealmLen  = nRealmLen;
	PSHOWROOT = PROOT;
	nShowRootLen = Rootlen;

	nType = type;

	if (!WIE_CreateAuthInput())
		return FALSE;

	return bResult;
}


static BOOL WIE_CreateAuthInput(void)
{
	HWND hAuthWnd;
	MSG  msg;
    WNDCLASS wc;	
	
    wc.style         = 0;
	wc.lpfnWndProc   = WIE_AuthWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "WieAuthClass";

    if (!RegisterClass(&wc))
        return FALSE;

    hAuthWnd = CreateWindow(
		"WieAuthClass",
		"",
		WS_VISIBLE|WS_POPUP|WS_CAPTION|PWS_STATICBAR,
		PLX_WIN_POSITION, 
		NULL,
		NULL,
		NULL,
		NULL); 
	
	SendMessage(hAuthWnd, PWM_CREATECAPTIONBUTTON, (WPARAM)IDM_EXIT, (LPARAM)WML_CC);
	SendMessage(hAuthWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)WML_INBOX_DELETE_OK);	

	while (TRUE)
	{
		GetMessage(&msg, 0, 0, 0);

        if (msg.hwnd == hAuthWnd && (msg.message == IDM_EXITLOOP))
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!IsWindowVisible(hAuthWnd))
            break;
	}

	UnregisterClass("WieAuthClass", NULL);

	return TRUE;
}

static LRESULT CALLBACK WIE_AuthWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
	LRESULT	lResult = TRUE;
	static HWND hAuthID, hPsw;
	static IMEEDIT InputAuthID, InputPsw;

	switch (wMsgCmd)	
    {
    case WM_CREATE:
		memset(&InputAuthID, 0, sizeof(IMEEDIT));	
		InputAuthID.hwndNotify   = (HWND)hWnd;
		InputAuthID.dwAscTextMax = 0;
		InputAuthID.dwAttrib     = 0;//IME_ATTRIB_GENERAL;
		InputAuthID.dwUniTextMax = 0;
		InputAuthID.pszCharSet   = NULL;
		InputAuthID.pszImeName   = NULL;//"×ÖÄ¸";
		InputAuthID.pszTitle     = NULL;
		InputAuthID.uMsgSetText  = 0;
		InputAuthID.wPageMax     = 0;
        hAuthID = CreateWindow(
			"IMEEDIT", 
			"",
			WS_VISIBLE | WS_CHILD | ES_UNDERLINE | WS_TABSTOP|ES_AUTOHSCROLL, 
			AUTH_AUTHID_X, AUTH_AUTHID_Y, AUTH_AUTHID_WIDTH, AUTH_AUTHID_HEIGHT, 
			hWnd, 
			(HMENU)NULL,
			NULL, 
			(PVOID)&InputAuthID); 
        if (hAuthID == NULL) 
			return FALSE;

		SendMessage(hAuthID, EM_LIMITTEXT, nMaxNameLen, NULL);

		memset(&InputPsw, 0, sizeof(IMEEDIT));	
		InputPsw.hwndNotify   = (HWND)hWnd;
		InputPsw.dwAscTextMax = 0;
		InputPsw.dwAttrib     = 0;//IME_ATTRIB_GENERAL;;
		InputPsw.dwUniTextMax = 0;
		InputPsw.pszCharSet   = NULL;
		InputPsw.pszImeName   = NULL;//"×ÖÄ¸";
		InputPsw.pszTitle     = NULL;
		InputPsw.uMsgSetText  = 0;
		InputPsw.wPageMax     = 0;
		
        hPsw = CreateWindow(
			"IMEEDIT", 
			"",
			WS_VISIBLE | WS_CHILD | ES_UNDERLINE | WS_TABSTOP|ES_AUTOHSCROLL|ES_PASSWORD , 
			AUTH_PWD_X, AUTH_PWD_Y, AUTH_PWD_WIDTH, AUTH_PWD_HEIGHT, 
			hWnd, 
			(HMENU)NULL,
			NULL, 
			(PVOID)&InputPsw); 
        if (hAuthID == NULL) 
			return (FALSE);
		SendMessage(hPsw, EM_LIMITTEXT, nMaxPswLen, NULL);			
		bResult = FALSE;
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_CONFIRM:
			{
				int len;

				if (0 == (len = GetWindowTextLength(hAuthID)))
					strcpy(pName, "");
				else
					GetWindowText(hAuthID, pName, len+1);

				if (0 == (len = GetWindowTextLength(hPsw)))
					strcpy(pPsw, "");
				else
					GetWindowText(hPsw, pPsw, len+1);

				bResult = TRUE;

				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;

		case IDM_EXIT:
			{
				bResult = FALSE;
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;

		default:
			break;
		}
		break;

	case WM_CLOSE:
		{
			PostMessage(hWnd, IDM_EXITLOOP, 0, 0);
			DestroyWindow(hWnd);
		}
		break;

	case WM_PAINT:	
		{
			HDC hdc;
			int bkmodeold;

			hdc = BeginPaint(hWnd, NULL);
			bkmodeold = SetBkMode(hdc, TRANSPARENT);
			SelectObject(hdc, hViewFont);
			TextOut(hdc, AUTH_AUTHTEXT_X, AUTH_AUTHTEXT_Y, (PCSTR)WML_AUTH_USER, -1);
			TextOut(hdc, AUTH_PWDTEXT_X, AUTH_PWDTEXT_Y, (PCSTR)WML_AUTH_PWS, -1);
			SetBkMode(hdc, bkmodeold);
			EndPaint(hWnd,NULL);
		}
		break;		

	default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

	return lResult;
}

/////////////////////////////////////////////////////////////////////////////
///Permit Window
static BOOL WIE_CreatePermitWin(void);
//static LRESULT CALLBACK WIE_PermitWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
//                              LPARAM lParam);

static char *szShow = NULL;
static char *c1 = NULL;
static char *c2 = NULL;

BOOL WIE_WapPermit(const char *SHOWINFOR, int inforlen, const char* CHAR1, int c1len,
				   const char *CHAR2, int c2len)
{	
	szShow = malloc(inforlen + 1);
	memmove(szShow, SHOWINFOR, inforlen);
	szShow[inforlen] = 0;

	if (c1len != 0)
		c1 = (char*)CHAR1;
	if (c2len != 0)
		c2 = (char*)CHAR2;
	
	WIE_CreatePermitWin();

	if (szShow != NULL)
	{
		free(szShow);
		szShow = NULL;
	}	

	return bResult;
}

static BOOL WIE_CreatePermitWin(void)
{
	HWND hPermitWnd;
	MSG  msg;
    WNDCLASS wc;	
	
    wc.style         = 0;
	wc.lpfnWndProc   = WIE_PermitWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "PermitAuthClass";

    if (!RegisterClass(&wc))
        return FALSE;

    hPermitWnd = CreateWindow(
		"PermitAuthClass",
		"",
		WS_VISIBLE|WS_POPUP|WS_CAPTION|PWS_STATICBAR,
		PLX_WIN_POSITION, 
		NULL,
		NULL,
		NULL,
		NULL);   
	
	if (c2 == NULL)
		SendMessage(hPermitWnd, PWM_CREATECAPTIONBUTTON, (WPARAM)IDM_EXIT, (LPARAM)WML_CC);
	else
		SendMessage(hPermitWnd, PWM_CREATECAPTIONBUTTON, (WPARAM)IDM_EXIT, (LPARAM)c2);

	if (c1 == NULL)
		SendMessage(hPermitWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)WML_INBOX_DELETE_OK);
	else
		SendMessage(hPermitWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)c1);

	while (TRUE)
	{
		GetMessage(&msg, 0, 0, 0);

        if (msg.hwnd == hPermitWnd && (msg.message == IDM_EXITLOOP))
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!IsWindowVisible(hPermitWnd))
            break;
	}
	UnregisterClass("PermitAuthClass", NULL);

	return TRUE;
}

static LRESULT CALLBACK WIE_PermitWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
	LRESULT lResult = TRUE;
	HDC temDC;
	static char* pShowBuf = NULL;
	static int nShowBufLen = 0;

	switch (wMsgCmd)	
    {
    case WM_CREATE:
		temDC = GetDC(hWnd);
//		WIE_LayoutString(temDC, szShow, strlen(szShow), NULL, &nShowBufLen, CONTROL_WIDTH);
		pShowBuf = malloc(nShowBufLen + 1);
//		WIE_LayoutString(temDC, szShow, strlen(szShow), pShowBuf, &nShowBufLen, CONTROL_WIDTH);
		ReleaseDC(hWnd, temDC);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_CONFIRM:
			{				
				bResult = TRUE;
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;

		case IDM_EXIT:
			{
				bResult = FALSE;
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;

		default:
			break;
		}
		break;

	case WM_CLOSE:
		if (pShowBuf != NULL)
		{
			free(pShowBuf);
			pShowBuf = NULL;
		}
		nShowBufLen = 0;
		PostMessage(hWnd, IDM_EXITLOOP, 0, 0);
		DestroyWindow(hWnd);
		break;

	case WM_PAINT:	
		{
			HDC hdc;
			RECT rect;
			int bkmodeold;
			hdc = BeginPaint(hWnd, NULL);
			bkmodeold = SetBkMode(hdc, TRANSPARENT);
			SelectObject(hdc, hViewFont);
			rect.left	= 0;
			rect.right	= CONTROL_WIDTH;
			rect.top	= 0;
			rect.bottom	= CONTROL_HEIGHT*4;
			DrawText(hdc, pShowBuf, -1, &rect, DT_LEFT| DT_TOP);
		//	TextOut(hdc, AUTH_AUTHTEXT_X, AUTH_AUTHTEXT_Y, szShow, -1);
			SetBkMode(hdc, bkmodeold);
			EndPaint(hWnd, NULL);
		}
		break;
		

	default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

	return lResult;
}

///////////////////////////////////////////////////////////////////////////
// prompt window
static char* pResult;
static int* PromptMaxlen;
static char* pDefault;

static BOOL WIE_CreatePromptInput(void);
static LRESULT CALLBACK WIE_PromptWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);

BOOL WIE_WapPrompt(const char *PTITLE, int nTitleLen, 
				   const char *PDEFRESULT, int deflen,
				   char* result, int *nReslen)
{

	szShow = malloc(nTitleLen + 1);
	memmove(szShow, PTITLE, nTitleLen);
	szShow[nTitleLen] = 0;
	PromptMaxlen = nReslen;
	pResult = result;
	pDefault = (char*)PDEFRESULT;	
	WIE_CreatePromptInput();
	strcpy(result, pResult);
	*nReslen = *PromptMaxlen;	
	if (szShow != NULL)
	{
		free(szShow);
		szShow = NULL;
	}

	return bResult;
}


static BOOL WIE_CreatePromptInput(void)
{
	HWND hPromptWnd;
	MSG  msg;
    WNDCLASS wc;	
	
    wc.style         = 0;
	wc.lpfnWndProc   = WIE_PromptWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "WiePromptClass";

    if (!RegisterClass(&wc))
        return FALSE;

    hPromptWnd = CreateWindow(
		"WiePromptClass",
		"",
		WS_VISIBLE|WS_POPUP|WS_CAPTION|PWS_STATICBAR,
		PLX_WIN_POSITION, 
		NULL,
		NULL,
		NULL,
		NULL);  
		
	SendMessage(hPromptWnd, PWM_CREATECAPTIONBUTTON, (WPARAM)IDM_EXIT, (LPARAM)WML_CC);
	SendMessage(hPromptWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)WML_INBOX_DELETE_OK);	

	while (TRUE)
	{
		GetMessage(&msg, 0, 0, 0);

        if (msg.hwnd == hPromptWnd && (msg.message == IDM_EXITLOOP))
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!IsWindowVisible(hPromptWnd))
            break;
	}

	UnregisterClass("WiePromptClass", NULL);

	return TRUE;
}

static LRESULT CALLBACK WIE_PromptWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
	LRESULT	lResult = TRUE;
	static HWND hAuthID,hPsw;
	HDC temDC;
	static char* pShowBuf = NULL;
	static int nShowBufLen = 0;
	static IMEEDIT InputAuthID1, InputPsw1;

	switch (wMsgCmd)	
    {
    case WM_CREATE :
		memset(&InputAuthID1, 0, sizeof(IMEEDIT));	
		InputAuthID1.hwndNotify   = (HWND)hWnd;
		InputAuthID1.dwAscTextMax = 0;
		InputAuthID1.dwAttrib     = 0;//IME_ATTRIB_GENERAL;;
		InputAuthID1.dwUniTextMax = 0;
		InputAuthID1.pszCharSet   = NULL;
		InputAuthID1.pszImeName   = NULL;//"×ÖÄ¸";
		InputAuthID1.pszTitle     = NULL;
		InputAuthID1.uMsgSetText  = 0;
		InputAuthID1.wPageMax     = 0;

	    hAuthID= CreateWindow(
			"IMEEDIT", 
			"",
			WS_VISIBLE | WS_CHILD | ES_UNDERLINE | WS_TABSTOP|ES_AUTOHSCROLL, 
			AUTH_AUTHID_X, AUTH_AUTHID_Y, AUTH_AUTHID_WIDTH, AUTH_AUTHID_HEIGHT, 
			hWnd, 
			(HMENU)NULL,
			NULL, 
			(PVOID)&InputAuthID1
			); 
        if (hAuthID == NULL) 
			return (FALSE);
		SendMessage(hAuthID, EM_LIMITTEXT, nMaxNameLen, NULL);
		
		memset(&InputPsw1, 0, sizeof(IMEEDIT));	
		InputPsw1.hwndNotify   = (HWND)hWnd;
		InputPsw1.dwAscTextMax = 0;
		InputPsw1.dwAttrib     = 0;//IME_ATTRIB_GENERAL;;
		InputPsw1.dwUniTextMax = 0;
		InputPsw1.pszCharSet   = NULL;
		InputPsw1.pszImeName   = NULL;//"×ÖÄ¸";
		InputPsw1.pszTitle     = NULL;
		InputPsw1.uMsgSetText  = 0;
		InputPsw1.wPageMax     = 0;        
		hPsw = CreateWindow(
			"IMEEDIT", 
			"",
			WS_VISIBLE | WS_CHILD | ES_UNDERLINE | WS_TABSTOP|ES_AUTOHSCROLL|ES_PASSWORD, 
			AUTH_PWD_X, AUTH_PWD_Y, AUTH_PWD_WIDTH, AUTH_PWD_HEIGHT, 
			hWnd, 
			(HMENU)NULL,
			NULL, 
			(PVOID)&InputPsw1
			); 
        if ( hAuthID == NULL ) 
			return (FALSE);
		SendMessage(hPsw, EM_LIMITTEXT, nMaxPswLen, NULL);		

		temDC = GetDC(hWnd);
//		WIE_LayoutString(temDC, szShow, strlen(szShow), NULL, &nShowBufLen, CONTROL_WIDTH);
		pShowBuf = malloc(nShowBufLen + 1);
//		WIE_LayoutString(temDC, szShow, strlen(szShow), pShowBuf, &nShowBufLen, CONTROL_WIDTH);
		ReleaseDC(hWnd, temDC);
		bResult = FALSE;
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_CONFIRM:
			{
				int len;
/*
				if (0 == (len = GetWindowTextLength(hAuthID)) )
					strcpy(pName, "");
				else
					GetWindowText(hAuthID, pName, len+1 );
*/
				if (0 == (len = GetWindowTextLength(hPsw)))
					strcpy(pResult, "");
				else
					GetWindowText(hPsw, pResult, len+1);

				*PromptMaxlen = len;
				bResult = TRUE;
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;

		case IDM_EXIT:
			{
				bResult = FALSE;
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;

		default:
			break;
		}
		break;

	case WM_CLOSE:
		{
			if (pShowBuf != NULL)
			{
				free(pShowBuf);
				pShowBuf = NULL;
			}
			nShowBufLen = 0;
			PostMessage(hWnd, IDM_EXITLOOP, 0, 0);
			DestroyWindow(hWnd);
		}
		break;


	case WM_PAINT:	
		{
			HDC hdc;
			RECT rect;
			int bkmodeold;

			hdc = BeginPaint(hWnd, NULL);			
			bkmodeold = SetBkMode(hdc, TRANSPARENT);
			SelectObject(hdc, hViewFont);
			rect.left	= 0;
			rect.right	= CONTROL_WIDTH;
			rect.top	= 0;
			rect.bottom	= CONTROL_HEIGHT*4;			
			DrawText(hdc, pShowBuf, -1, &rect, DT_LEFT| DT_TOP);
			//TextOut(hdc, AUTH_AUTHTEXT_X, AUTH_AUTHTEXT_Y, szShow,-1);
			//TextOut(hdc, AUTH_PWDTEXT_X,  AUTH_PWDTEXT_Y, (PCSTR)GetString(WML_AUTH_PWS),-1);
			SetBkMode(hdc, bkmodeold);
			EndPaint(hWnd,NULL);
		}
		break;		

	default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

	return lResult;
}

BOOL WIE_WapAssert(const char *SHOWINFOR, int inforlen)
{
	char *szShow;

	szShow = malloc(inforlen + 1);
	memmove(szShow, SHOWINFOR, inforlen);
	szShow[inforlen] = 0;
	
	if (szShow != NULL)
	{
		free(szShow);
		szShow = NULL;
	}
	
	return TRUE;
}

void WIE_WapDealJad(const char *CREQURL, const char *HTTP_HEAD, unsigned int headlen,
                 const char *HTTP_CONTENT, unsigned int contentlen)
{
//    JAM_ShowJAD((char*)CREQURL,(char*)HTTP_HEAD,headlen,(char*)HTTP_CONTENT,contentlen,
//        NULL,0,0);
}

BOOL WIE_MailTo(char *szMail, char *szCc, char *szBcc, char *szSubject, char *szBody)
{
    return TRUE;
}
