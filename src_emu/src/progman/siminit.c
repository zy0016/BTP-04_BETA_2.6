  /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : progman
 *
 * Purpose  : implement gsm initialize
 *            
\**************************************************************************/

#include "window.h"
#include "winpda.h"
#include "string.h"
#include "stdio.h"

#include "me_wnd.h"

#include "progman.h"
#include "mullang.h"
#include "MBpublic.h"

#define IDM_OK           201
#define IDC_CODE1        202
#define IDC_CODE2        203
#define IDM_CANCEL       204

#define CTR_X			 0
#define CTR_Y			 0
#define EDIT_WIDTH       DESK_WIDTH
#define EDIT_HIGHT       50
#define WIN_HIGHT        220

#define DEFAULT_TIMEOUT  30

#define MAX_CODELEN      8
#define MIN_CODELEN		 4

#define IDS_SOS			"SOS"
#define IDS_OK			"Ok"
#define IDS_CANCEL       "Cancel"
#define TITLE_TIPWIN     "Security"
#define IDC_TEST	(WM_USER + 155)
BOOL    g_bPassWord = TRUE;
static BOOL bPinVerify = TRUE, bPinNew, bPhoneLockstate;
extern PAPPADM  pAppMain;	
extern	PROGDESKIDLE  ProgDeskIdle;
static  char strNewPin[MAX_CODELEN +1], strPass[MAX_CODELEN + 1];
static  HWND hWndMsg, hWndNewPin;
static  BOOL bPin1Open;
static  HWND hFocusWnd;
static  BOOL bRepaint;

static char	*SOSPhoneNum[]=
{
	"112",
//	"110",
//	"119",
//	"114",
//	"120",
	"911",
	"08",
	""
};

static  BOOL    SelectPowerON(void);
static  BOOL    NewPin_OnCreate(HWND hWnd, HWND *hCode1, HWND *hCode2);
static  LRESULT PasswordWndProc(HWND, UINT, WPARAM, LPARAM);
static  LRESULT	NewPinWndProc(HWND, UINT, WPARAM, LPARAM);
static  LRESULT SelectPowerOnWndProc(HWND, UINT, WPARAM, LPARAM);

extern  BOOL  PlayPowerOff(BOOL bPlay);
extern	void  f_HandSetInlineInitialize ( void );
extern  BOOL  APP_CallPhone(const char * pPhoneNumber);

static	LRESULT	NewPin_Code1WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static	WNDPROC	OldPin_Code1WndProc;

BOOL	NewPinWindow()
{    
    WNDCLASS wc;  
    
    wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;        
    wc.lpfnWndProc   = NewPinWndProc;
    wc.lpszClassName = "NewPinWndClass";
	
    RegisterClass(&wc);     
    
	hWndNewPin = CreateWindow("NewPinWndClass",
						ML("PIN code"),
						WS_VISIBLE|WS_BORDER|PWS_STATICBAR/*|WS_CAPTION*/,
						CTR_X,0,PLX_WIN_WIDTH,WIN_HIGHT,
						NULL, NULL, NULL, NULL);

	ShowWindow(hWndNewPin,SW_SHOW);
	UpdateWindow(hWndNewPin);

	return TRUE;
}

static LRESULT NewPinWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT lResult  = TRUE;	
	HDC     hdc;
	static  HWND    hCode1, hCode2;
	static  int     nTimes = 0;
	HBITMAP hbmpCap =NULL ;

    switch (wMsgCmd)
    {
	 case WM_CREATE:
		 
		 SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_OK, 1),
			 (LPARAM)ML(IDS_SOS));
		 SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_CANCEL, 0),
			 (LPARAM)"");	
		 
		 NewPin_OnCreate(hWnd, &hCode1, &hCode2);

		 OldPin_Code1WndProc = (WNDPROC)GetWindowLong(hCode1, GWL_WNDPROC);
		 SetWindowLong(hCode1, GWL_WNDPROC, (long)NewPin_Code1WndProc);

		 hbmpCap = LoadImage(NULL,"/usr/local/lib/gui/resources/capbmp.bmp", IMAGE_BITMAP, 176, 24,LR_LOADFROMFILE);
		 hFocusWnd = hCode1;
		 break;
		 
	 case WM_ACTIVATE:
		 if (WA_INACTIVE != LOWORD(wParam))
		 {
			 SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_OK, 1),
				 (LPARAM)ML(IDS_SOS));
			 SetFocus(hFocusWnd);
		 }
		 break;
         
	 case WM_PAINT:
		 {
			 RECT	rGps ,rect;
			 int nMode;
		//	 COLORREF  noldcolor;


			 hdc = BeginPaint(hWnd, NULL);
			 
			 BitBlt(hdc, 0, 16, 176, 40, (HDC)hbmpCap, 0, 0, ROP_SRC);
			 SetRect(&rGps, 0, 0,176, 15);
			 SetRect(&rect, 0, 16, 176,40);
			 nMode = SetBkMode(hdc, TRANSPARENT);
			 //noldcolor = SetBkColor(hdc, COLOR_WHITE);
			 DrawText(hdc, ML("PIN code"), -1, &rect,DT_VCENTER|DT_CENTER);
			 SetBkMode(hdc, nMode);
			 //SetBkColor(hdc, noldcolor);
			 ClearRect(hdc, &rGps, RGB(206, 211, 214));//grey
			 DrawIconFromFile(hdc, "/rom/progman/icon/icon_off.ico", 0, 0, 14, 14);
			 
			 EndPaint(hWnd, NULL);
		 }		 
		 break;

	 case WM_COMMAND:
		 switch(LOWORD(wParam))
		 {
		 case IDC_CODE1:
			 if(HIWORD(wParam) == EN_CHANGE)
			 {
				 int strCodeLen1 = 0, strCodeLen2 = 0;
				 char pstrtxt[MAX_CODELEN + 1];

				 strCodeLen1 = GetWindowTextLength(hCode1);
				 strCodeLen2 = GetWindowTextLength(hCode2);

				 if(strCodeLen1 < MIN_CODELEN || strCodeLen2< MIN_CODELEN)
				 {
					SendMessage (hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(IDS_SOS));
				 }
				 else
				 {
					SendMessage (hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(IDS_OK));
				 }

                 memset(pstrtxt, 0, MAX_CODELEN + 1);
				 GetWindowText(hCode1,pstrtxt,MAX_CODELEN + 1);
				 if (findstr(SOSPhoneNum, pstrtxt)) {
					 bRepaint = TRUE;
					 InvalidateRect(hCode1, NULL, TRUE);
				 }
			 }
		 	break;
		 case IDC_CODE2:
			 if(HIWORD(wParam) == EN_CHANGE)
			 {
				 int strCodeLen1 = 0, strCodeLen2 = 0;

				 strCodeLen1 = GetWindowTextLength(hCode1);
				 strCodeLen2 = GetWindowTextLength(hCode2);
				 if(strCodeLen1 < MIN_CODELEN || strCodeLen2< MIN_CODELEN)
				 {
					SendMessage (hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(IDS_SOS));
				 }
				 else
					 SendMessage (hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(IDS_OK));
			 }
		 	break;
		 }
		 break;

	case WM_KEYDOWN:
		{
		
			switch(LOWORD(wParam))
			{
				
			case VK_RETURN:
				{
					int strCodeLen1 = 0, strCodeLen2 = 0;
					char strCode1[MAX_CODELEN+1];
					char strCode2[MAX_CODELEN+1];
					
									
					strCodeLen1 = GetWindowTextLength(hCode1);
					strCodeLen2 = GetWindowTextLength(hCode2);

					if(strCodeLen1 < MIN_CODELEN || strCodeLen2< MIN_CODELEN)
					{
						if(strCodeLen1 < MIN_CODELEN && GetFocus() == hCode1)
						{
							char strPhoneNumber[MAX_CODELEN+1];
							
							memset(strPhoneNumber, 0, MAX_CODELEN + 1);
							
							GetWindowText(hCode1,strPhoneNumber, MAX_CODELEN + 1);
							
							if(findstr(SOSPhoneNum, strPhoneNumber))
								APP_CallPhoneNumber(strPhoneNumber);
						}
						break;
					}
					else
					{	
						memset(strCode1, 0, MAX_CODELEN+1);
						memset(strCode2, 0, MAX_CODELEN+1);
						
						GetWindowText(hCode1, strCode1, MAX_CODELEN+1);
						GetWindowText(hCode2, strCode2, MAX_CODELEN+1);
						
						if (strcmp(strCode2, strCode1) == 0)
						{
							strcpy(strNewPin, strCode1);
							ME_PasswordValidation(hWndMsg, WM_PIN_CONFIRM, PUK1, strPass, strNewPin);
							DestroyWindow(hWnd);
						}
						else
						{
							PLXTipsWinOem(NULL, hWnd, NULL, ML("PIN codes \r\ndo not match"), ML(TITLE_TIPWIN), Notify_Failure,
								NULL, NULL,WAITTIMEOUT);
							SetWindowText(hCode2, "");
							SetWindowText(hCode1, "");
						}
					}
					
				}
				break;

			case VK_F1:
				{
					char strPhoneNumber[MAX_CODELEN+1];
					
					memset(strPhoneNumber, 0, MAX_CODELEN + 1);

					GetWindowText(GetFocus(),strPhoneNumber, MAX_CODELEN + 1);

					if(findstr(SOSPhoneNum, strPhoneNumber))
						APP_CallPhoneNumber(strPhoneNumber);
					
				}
				break;
			case VK_F10:
				{
					HWND hCurWin = NULL;
					int     nEnd = 0,nPos = 0;
					
					hCurWin = GetFocus();
					SendMessage(hCurWin, EM_GETSEL, (WPARAM)&nPos, (LPARAM)&nEnd);
					if (!nEnd)
						break; 
					else
						SendMessage(hCurWin, WM_KEYDOWN, VK_BACK, 0);
				}
				break;
			default:
				PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
				
			}//switch
		}

		break;
        
    case WM_DESTROY:
		hCode1 = NULL;
		hCode2 = NULL;
		if (hbmpCap) {
			DeleteObject(hbmpCap);
			hbmpCap = NULL;
		}
	    UnregisterClass("NewPinWndClass", NULL);
        hWndNewPin = NULL;
        break;
		
	case WM_SETRBTNTEXT:
		if(stricmp((char *)lParam, "Cancel") != 0)
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, lParam);
		else
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		break;

	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
	return lResult;
}

static BOOL NewPin_OnCreate(HWND hWnd, HWND *hCode1, HWND *hCode2)
{
	*hCode1= CreateWindow("EDIT", "",
		WS_VISIBLE|WS_CHILD|ES_TITLE|WS_TABSTOP|ES_NUMBER|ES_PASSWORD| ES_CENTER,
		CTR_X, CTR_Y +46, EDIT_WIDTH, EDIT_HIGHT, hWnd,(HMENU)IDC_CODE1, NULL,NULL);

	if(*hCode1 == NULL)
		return FALSE;

	SendMessage(*hCode1, EM_SETTITLE, 0, (LPARAM)ML("Enter new code"));

	
	*hCode2= CreateWindow("EDIT", "",
		WS_VISIBLE|WS_CHILD|ES_TITLE|WS_TABSTOP|ES_NUMBER|ES_PASSWORD| ES_CENTER,
		CTR_X, CTR_Y + EDIT_HIGHT +46, EDIT_WIDTH, EDIT_HIGHT, hWnd,(HMENU)IDC_CODE2, NULL,NULL);

	SendMessage(*hCode2, EM_SETTITLE, 0, (LPARAM)ML("Repeat new code"));

	if(*hCode2 == NULL)
		return FALSE;

	 SendMessage(* hCode1, EM_LIMITTEXT, MAX_CODELEN, 0);
	 SendMessage(* hCode2, EM_LIMITTEXT, MAX_CODELEN , 0);


	return TRUE;
}
/**********************************************************************
 * Function     GetTitleRect
 * Purpose      
 * Params       HWND hWnd, PRECT pRect
 * Return       
 * Remarks      
 **********************************************************************/
#define CX_LEFTMARGIN        5
#define CX_RIGHTMARGIN       3
#define CY_TOPMARGIN1        4
static BOOL GetTitleRect(HWND hWnd, PRECT pRect)
{
    RECT   rcClient;
    HDC    hdc = NULL;
    HFONT  hFont = NULL, hfontOld = NULL;
    SIZE   size;

    GetClientRect(hWnd, &rcClient);

    hdc = GetDC(hWnd);
    GetFontHandle(&hFont, SMALL_FONT);
    hfontOld = SelectObject(hdc, (HGDIOBJ)hFont);
    GetTextExtentPoint32(hdc, "T", -1, &size);
    SelectObject(hdc, (HGDIOBJ)hfontOld);
    ReleaseDC(hWnd, hdc);

    pRect->left   = rcClient.left + CX_LEFTMARGIN;
    pRect->top    = rcClient.top + CY_TOPMARGIN1;
    pRect->right  = rcClient.right - CX_RIGHTMARGIN;
    pRect->bottom = pRect->top + size.cy;

    return TRUE;
}
static	LRESULT	NewPin_Code1WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{ 
    LRESULT	lResult;
    HWND    hParent;
	HDC     hdc;
	RECT    rcClient,rcTitle;
    char    pstrtxt[MAX_CODELEN + 1];
	static HBITMAP hbmpFocus;
	COLORREF cr;
	BITMAP        bmp;
	char    szPathName[128];
	int    nMode = 0;
	HFONT  hSmallFot = NULL, hOldFot = NULL;

    lResult = TRUE;

    switch(wMsgCmd)
    {
	case WM_CHAR :  
		
        if((WORD)wParam >= '0' && (WORD)wParam <= '9')
            lResult = OldPin_Code1WndProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    case WM_PAINT:
		if (bRepaint)
		{
			hParent = GetParent(hWnd);
			memset(szPathName, 0, 128);
			memset(&bmp,0,sizeof(BITMAP));
			strcpy(szPathName, "/rom/progman/listfocus.bmp");
			GetClientRect(hWnd, &rcClient);
			memset(pstrtxt, 0, MAX_CODELEN + 1);
			GetWindowText(GetDlgItem(hParent, IDC_CODE1), pstrtxt, MAX_CODELEN + 1 );
			GetTitleRect(hWnd, &rcTitle);
			hbmpFocus = LoadImage(NULL, szPathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			hdc = BeginPaint(hWnd, NULL);
			GetObject(hbmpFocus, sizeof(BITMAP), (void*)&bmp);
			StretchBlt(hdc, rcClient.left, rcClient.top, rcClient.right - rcClient.left,
				rcClient.bottom - rcClient.top, (HDC)hbmpFocus, 0, 0, bmp.bmWidth,
				bmp.bmHeight, ROP_SRC);
			cr= SetTextColor(hdc, COLOR_WHITE);
			nMode = SetBkMode(hdc, TRANSPARENT);
			GetFontHandle(&hSmallFot, SMALL_FONT);
			hOldFot = SelectObject(hdc, hSmallFot);
			DrawText(hdc, ML("Enter new code"), -1, &rcTitle, DT_LEFT | DT_VCENTER);
			SelectObject(hdc, hOldFot);
			DrawText(hdc, pstrtxt, -1, &rcClient, DT_VCENTER|DT_CENTER);
			SetTextColor(hdc, cr);
			SetBkMode(hdc, nMode);
			EndPaint(hWnd,NULL);
			bRepaint = FALSE;
			SetTimer(hWnd, 1, 2000, NULL);
		}
		else
		{
			if(hbmpFocus)
			{
				DeleteObject(hbmpFocus);
				hbmpFocus = NULL;
			}
			lResult = OldPin_Code1WndProc( hWnd, wMsgCmd, wParam, lParam);
		}
		break;
		
	case WM_TIMER:
		KillTimer(hWnd, 1);
		bRepaint = FALSE;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
		
	default:
		lResult = OldPin_Code1WndProc( hWnd, wMsgCmd, wParam, lParam);
		break;
    }
    return lResult;
}
/******************************************************************** 
* Function	   PasswordWindow
* Purpose      It is an interface for password verify
* Params	   type: password type, it is pin or puk
* Return	   BOOL
**********************************************************************/
#define REQUEST_CODE  WM_USER +101
#define REQUEST_CODEWRONG WM_USER +102
#define REQUEST_CODERIGHT  WM_USER + 103
#define GET_PSCODE         WM_USER + 104

#define PUKCODE_CALL       WM_USER + 105
#define PINCODE_CALL       WM_USER +106
#define PHONECODE_CALL     WM_USER + 107
#define PHONECODE		   WM_USER + 108
static BOOL bSetPin = FALSE, bPhonecode = FALSE, bInitEnd =TRUE;
static int chance = 0;
extern BOOL GetPS_Open(void);
extern BOOL GetPSCode(char* pPsCode);
extern BOOL GetPSData(char*pPsData);
BOOL HavePhonecodeCheck(void)
{
	return bPhonecode;
}
BOOL InitProcessEnd(void)
{
	return bInitEnd;
}
BOOL	PasswordWindow(int type)
{    
    WNDCLASS wc;    
   
    wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;        
    wc.lpfnWndProc   = PasswordWndProc;
    wc.lpszClassName = "PasswordWindowClass";
    RegisterClass(&wc);    

    if (type == PIN1)
        bPinVerify = TRUE;
    else 
        bPinVerify = FALSE;
    
	hWndMsg = CreateWindow("PasswordWindowClass",
						"",
						PWS_STATICBAR,
						0,	
						0,
						0,	
						0,
						NULL, NULL,	NULL, NULL);
		
	return TRUE;
}

static LRESULT PasswordWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT lResult  = TRUE;	
	HDC     hdc;
	static  HWND    hError, hChance, hInput;

    switch (wMsgCmd)
    {
	 case WM_CREATE:

		 if(bPinVerify)
			ME_GetPassInputLimit(hWnd, WM_PIN_REMAIN, PIN1);
		 else
			 ME_GetPassInputLimit(hWnd, WM_PIN_REMAIN, PUK1);
		
		 bSetPin = TRUE;
		 g_bPassWord = TRUE;
		 break;
         
	 case WM_PAINT:		 
		 hdc = BeginPaint(hWnd, NULL);
         EndPaint(hWnd, NULL);		 
		 break;

     case REQUEST_CODERIGHT:
		 {
			 bInitEnd = TRUE;
			 SetSimState(1);
			 PostMessage( hWnd, WM_CLOSE, 0, 0 );
//			 PlayPowerOn(TRUE);
			 PLXPrintf("=================run ME_ClearSMSindication==============\r\n");
			 ME_ClearSMSindication(pAppMain->hDeskWnd, WM_USER+610);
			 ME_GSMInit_Module_3(pAppMain->hDeskWnd, WM_PROG_COPS);
		 }
		 break;
	 case PUKCODE_CALL:
		 {
			 
			 bPinNew = TRUE;
			 
			 if(lParam == NULL)
				 break;

			 memset(strPass,0, MAX_CODELEN +1);
			 strcpy(strPass, (char *)lParam);
			 
			 if(strlen(strPass) < MIN_CODELEN)
				 break;
			 
			 if (hWndNewPin == NULL)
			 {
				 NewPinWindow();
			 }
			 else
			 {
				 ShowWindow(hWndNewPin, SW_SHOW);
			 }
		 }

		 break;
	 case PINCODE_CALL:
		 {
			 if(lParam == NULL)
				 break;

			 ME_PasswordValidation(hWnd, WM_PIN_CONFIRM, PIN1, (char*)lParam, NULL);
		 }
		 break;

	 case REQUEST_CODEWRONG:
		 {
			 char strCode[MAX_CODELEN +1];
			 
			 memset(strCode, 0, MAX_CODELEN +1);
		
			 if(bPinVerify == FALSE)
			 {
				 PLXVerifyPassword(hWnd, NULL, ML("Enter \r\nPUK code:"),
					 strCode, MAKEWPARAM(MAX_CODELEN,MAX_CODELEN), ML(IDS_OK), NULL, -1,PUKCODE_CALL);
			 }
			 else
			 {
				 PLXVerifyPassword(hWnd, NULL, ML("Enter \r\nPIN code"),
					 strCode, MAKEWPARAM(MIN_CODELEN,MAX_CODELEN), ML(IDS_OK), NULL, -1, PINCODE_CALL);
			 }
		 }
		 break;
	 case PHONECODE:
		 {
			 char strCode[MAX_CODELEN +1];

			 memset(strCode, 0, MAX_CODELEN + 1);

			 PLXVerifyPassword(hWnd, NULL, ML("Enter Phone\r\nlock code:"), 
				 strCode, MAKEWPARAM(MIN_CODELEN, MAX_CODELEN), ML(IDS_OK),NULL, -1, PHONECODE_CALL);

		 }
		 break;
	 case PHONECODE_CALL:
		 {
			 char cLockcode[MAX_CODELEN + 1];
			 char strCode[MAX_CODELEN +1];

			 if(lParam == NULL)
				 break;
			 
			 memset(cLockcode,0, MAX_CODELEN + 1);
			 memset(strCode, 0, MAX_CODELEN + 1);
			 
			 GetSecurity_code(cLockcode);
			 strcpy(strCode, (char*)lParam);
			 
			 if (cLockcode[0] !=0)
			 {
				 if(strcmp(strCode,cLockcode) != 0)
				 {
					 PLXTipsWinOem(NULL,hWnd, PHONECODE, ML("Phone lock\r\ncode wrong "), ML(TITLE_TIPWIN), Notify_Failure, NULL,
						 NULL, WAITTIMEOUT );
				 }
				 else
				 {
					 PLXTipsWinOem(NULL, hWnd, REQUEST_CODERIGHT, ML("Phone lock\r\ncode right "), ML(TITLE_TIPWIN), Notify_Success, NULL,
						 NULL, WAITTIMEOUT);
					 
				 }
			 }
			
		 }
		 break;
	 case GET_PSCODE:
		 {
             char strCode[MAX_CODELEN +1];
			 unsigned char value[ME_MBID_LEN];
			 char strPscode[ME_MBID_LEN];
			 int i;
			 
			 for(i = 0; i< ME_MBID_LEN;i++)
				 value[i] = 0;

			 printf("\r\n recieve ME_GetSubscriberId result! ");
			 
			 if(ME_GetResult(&value, sizeof(unsigned char)*ME_MBID_LEN)<0)
				 break;
			 
			 printf("\r\n recieve ME_GetSubscriberId result failure! ");

			 memset(strPscode, 0, ME_MBID_LEN);
			 memset(strCode, 0, MAX_CODELEN +1);
			 GetPSCode(strPscode);
			 
			 bPhonecode = TRUE;
			 if(stricmp(strPscode, value) == NULL)
			 {
				 printf("\r\n ps lock pass!");
				 SendMessage(hWnd, REQUEST_CODERIGHT, 0, 0);
				 break;
			 }
			
			 PLXVerifyPassword(hWnd, NULL, ML("Enter Phone\r\nlock code:"), 
				 strCode, MAKEWPARAM(MIN_CODELEN, MAX_CODELEN), ML(IDS_OK),NULL, -1, PHONECODE_CALL);

		 }

		 break;

	 case REQUEST_CODE:
		 {
			 char strCode[MAX_CODELEN+1];
			 char cLockcode[MAX_CODELEN+1];

			 if(bPinNew)
				 bPinNew = FALSE;
			 		 
			 memset(cLockcode, 0, MAX_CODELEN +1);
			 memset(strCode, 0, MAX_CODELEN + 1);

			 printf("\r\n after pin code request!");

			 if(GetPS_Open())
			 {	
				 printf("\r\n enter phonecode request!");
				 bInitEnd = FALSE;
				 ME_GetSubscriberId(hWnd, GET_PSCODE);
			 }
			 else
			 {
				 SetSimState(1);
				 PostMessage( hWnd, WM_CLOSE, 0, 0 );
				 PLXPrintf("=================run ME_ClearSMSindication==============\r\n");
				 ME_ClearSMSindication(pAppMain->hDeskWnd, WM_USER+610);
				 ME_GSMInit_Module_3(pAppMain->hDeskWnd, WM_PROG_COPS);
			 }
			
		 }
		 break;

	case WM_PIN_CONFIRM:	
		printf("\r\n recieve WM_PIN_CONFIRM !");
		ME_GetCurWaitingPassword(hWnd, WM_PIN_QUERY);
		break;

	case WM_PIN_QUERY:
		{			
			int type;
			
			ME_GetResult(&type, sizeof(int));
			if (type == NOPIN)
			{
				printf("\r\n recieve me NOPIN !");
				if(bPinNew)
				{
					PLXTipsWinOem(NULL, hWnd, REQUEST_CODE, ML("PIN code changed"), ML(TITLE_TIPWIN), Notify_Success,
						NULL, NULL,WAITTIMEOUT);
				}
				else
					PLXTipsWinOem(NULL, hWnd, REQUEST_CODE, ML("PIN code right"), ML(TITLE_TIPWIN), Notify_Success,
					NULL,NULL, WAITTIMEOUT);
				
				break;
			}
			else if (type == PIN1)
			{
				printf("\r\n recieve me PIN1!");
				ME_GetPassInputLimit(hWnd, WM_PIN_REMAIN, PIN1);
				break;                                                
			}
			else if (type == PUK1)
			{
				printf("\r\n recieve me PUK1!");
				bPinVerify = FALSE;
				ME_GetPassInputLimit(hWnd, WM_PIN_REMAIN, PUK1);
				break;
			}
		}
		 break;
		 
	case WM_PIN_REMAIN:
		{
		   char strCode[MAX_CODELEN + 1];
                  
		   if (!bPinVerify && hWndNewPin != NULL)
			   ShowWindow(hWndNewPin, SW_HIDE);
		   
		   ME_GetResult(&chance, sizeof(int));

		   printf("\r\n recieve me remain times = %d!", chance);

		   memset(strCode, 0, MAX_CODELEN +1);

		   if(bSetPin)
		   {
			   if(bPinVerify)
			   {
				   PLXVerifyPassword(hWnd, NULL, ML("Enter \r\nPIN code:"),
					   strCode, MAKEWPARAM(MIN_CODELEN,MAX_CODELEN), ML(IDS_OK), NULL, -1, PINCODE_CALL);
			   }
			   else
			   {
				    PLXVerifyPassword(hWnd, NULL, ML("Enter \r\nPUK code:"),
					   strCode, MAKEWPARAM(MAX_CODELEN,MAX_CODELEN), ML(IDS_OK), NULL, -1, PUKCODE_CALL);
				   
			   }
		
			   bSetPin =FALSE;
			   break;
		   }
		   
           // no chance to pin code verify, need puk code verify
		   if ((chance == 10) && !bPinVerify)
		   {
			    bPinVerify = FALSE;	
			    PLXTipsWinOem(NULL, hWnd, REQUEST_CODEWRONG, ML("PIN code blocked"), ML(TITLE_TIPWIN), Notify_Failure, NULL,
				   NULL,DEFAULT_TIMEOUT);
			   
			   break;
		   }
		   if (chance == 0 && !bPinVerify)
		   {
			   strcpy(ProgDeskIdle.cPlmnStr, ML("SIM card\r\n destroied"));
               pm_sim_cop_status = pm_sim_destroy;
			   SelectPowerON();
			   PLXTipsWinOem(NULL, hWnd, REQUEST_CODE, ML("SIM card blocked"), ML(TITLE_TIPWIN), Notify_Failure, NULL,
				   NULL, WAITTIMEOUT);
					  
			   break;
		   }
		   if (chance > 0)
		   {
			   if(bPinVerify)
				   PLXTipsWinOem(NULL, hWnd, REQUEST_CODEWRONG, ML("PIN code wrong"), ML(TITLE_TIPWIN), Notify_Failure, 
				   NULL,NULL, WAITTIMEOUT);
			   else
					PLXTipsWinOem(NULL, hWnd, REQUEST_CODEWRONG, ML("PUK code wrong"), ML(TITLE_TIPWIN), Notify_Failure, 
				   NULL,NULL, WAITTIMEOUT);
			
			   break;
		   }
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F1:
		case VK_RETURN:
			{
				char strPhoneNumber[MAX_CODELEN+1];
				
				if(lParam == NULL)
					break;

				memset(strPhoneNumber, 0, MAX_CODELEN + 1);
				strcpy(strPhoneNumber, (char*)lParam);
				if(findstr(SOSPhoneNum, strPhoneNumber))
				{
				//	ME_VoiceDial(hWnd,IDC_TEST,"112",NULL);
					APP_CallPhoneNumber(strPhoneNumber);
				}
				
			}
			break;
		default:
			PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;
	        
	case IDC_TEST:
		{
			int i;
			i = 0;
		}
		break;
    case WM_DESTROY:		
        g_bPassWord = FALSE;
        hWndMsg = NULL;  
        UnregisterClass("PasswordWindowClass", NULL);
        break;

	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
	return lResult;
}
/******************************************************************** 
* Function	   SelectPowerON
* Purpose      It is an interface for select power on or power off,
               it is called when puk code verify error.
* Params	   
* Return	   BOOL
**********************************************************************/
static BOOL SelectPowerON()
{	
    WNDCLASS wc;
	HWND     hWnd;	
    
    wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;    
    wc.lpfnWndProc   = SelectPowerOnWndProc;
    wc.lpszClassName = "SelectPowerOnWindowClass";    

    RegisterClass(&wc);
    
    hWnd = CreateWindow("SelectPowerOnWindowClass",
					ML("Power on"),
					WS_VISIBLE|PWS_STATICBAR|WS_BORDER|WS_CAPTION,
					0,	
					TITLEBAR_HEIGHT,
					DESK_WIDTH,	
					DESK_HEIGHT-TITLEBAR_HEIGHT,
					NULL, NULL,	NULL, NULL);
	
	SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MYMESSAGE_RIGHTDOWN,
        (LPARAM)ML("Power off"));
	SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MYMESSAGE_LEFTDOWN,
        (LPARAM)ML("Power on"));	

	return TRUE;
}
static LRESULT SelectPowerOnWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT lResult  = TRUE;	
    HDC     hdc;
	int     bkoldMode;

    switch (wMsgCmd)
    {	 
     case WM_PAINT:
         hdc = BeginPaint(hWnd, NULL);
		 bkoldMode = SetBkMode(hdc, BM_TRANSPARENT);
         TextOut(hdc, 15, 44, ML("No SIM card"), -1);
		 SetBkMode(hdc, bkoldMode);
         EndPaint(hWnd, NULL);
         break;
         
	case WM_COMMAND:		
		switch(LOWORD(wParam))
		{
        // power on
		case LOWORD(MYMESSAGE_LEFTDOWN):
		    f_HandSetInlineInitialize();
			break;

        // power off
		case LOWORD(MYMESSAGE_RIGHTDOWN):				
			PlayPowerOff(TRUE);			
			break;
		}	
		break;		

    case WM_DESTROY:
        UnregisterClass("SelectPowerOnWindowClass", NULL);
        break;
        
	default :         
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
	return lResult;
}
/******************************************************************** 
* Function	   GetPin1Status
* Purpose      Get pin1 code request status
* Params	   
* Return	   BOOL
**********************************************************************/
BOOL GetPin1Status(void)
{
	return bPin1Open;
}
/******************************************************************** 
* Function	   SetPin1Status
* Purpose      set pin1 code request status  
* Params	   
* Return	   
**********************************************************************/
void SetPin1Status(BOOL bState)
{
	bPin1Open = bState;
}
static  int     iSsim   = 0;
/********************************************************************
* Function	   
* Purpose     Get SIM card status 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
extern int GetSIMState()
{
    return iSsim;
}
/********************************************************************
* Function	   
* Purpose     Set SIM card status 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
extern void SetSimState(int i)
{
    iSsim = i;
}
