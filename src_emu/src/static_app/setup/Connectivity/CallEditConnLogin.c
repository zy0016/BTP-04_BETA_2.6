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
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "plx_pdaex.h"
#include	"fcntl.h"
#include    "setting.h"
#include    "pubapp.h"
#include    "setup.h"
#include    "mullang.h"
#include	"imesys.h"
#include    "CallEditConnLogin.h"

static HWND hAppWnd;
static HWND hFrameWin;
static int iCurMode;
static UDB_ISPINFO Global_IspInfo;
static char cUserName[UDB_K_NAME_SIZE+1] = "";
static char cPassWord[UDB_K_PWD_SIZE+1] = "";
static const char* pClassName = "EditConnectionUNClass";
static const char* pPWClassName = "EditConnectionPWClass";

static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  PWAppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL		CreateControl (HWND hWnd, HWND *hwndEdit);
static BOOL		PWCreateControl (HWND hWnd, HWND *hwndEdit);

extern  BOOL IspReadInfo (UDB_ISPINFO * uIspInfo ,int iNum);
extern  BOOL IspSaveInfo (UDB_ISPINFO * uIspInfo ,int iNum);

BOOL CallEditConnUserName(HWND hWndApp,int icurMode)
{   
    WNDCLASS wc;
    RECT rClient;

    hFrameWin = hWndApp;
    iCurMode = icurMode;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    IspReadInfo(&Global_IspInfo, iCurMode);
    GetClientRect(hFrameWin, &rClient);
    hAppWnd = CreateWindow(pClassName, NULL, 
        WS_VISIBLE | WS_CHILD, 
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON,//LSK
            MAKEWPARAM((WORD)1, (WORD)IDC_BUTTON_SAVE), (LPARAM)ML("Save"));
        
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
            IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));//RSK  
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    SetWindowText(hFrameWin, Global_IspInfo.ISPName);
    SetFocus(hAppWnd);

    return TRUE;
}

static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT     lResult;    
    static HWND hFocus,hwndEdit;//show the editor of scenemode name

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
	case WM_CREATE:             
        CreateControl(hWnd,&hwndEdit);
        hFocus = hwndEdit;
        break;
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;
    case WM_DESTROY ://
         hAppWnd = NULL;
         UnregisterClass(pClassName,NULL);
         break;

    case PWM_SHOWWINDOW:
        SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON,//LSK
            MAKEWPARAM((WORD)1, (WORD)IDC_BUTTON_SAVE), (LPARAM)ML("Save"));
        
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
            IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));//RSK  
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif

        SetWindowText(hFrameWin, Global_IspInfo.ISPName);
        SetFocus(hWnd);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {            
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SAVE,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;  
        
    case WM_COMMAND:            
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_CANCEL:
            DestroyWindow(hWnd);
            break;      
                
        case IDC_BUTTON_SAVE: 
            GetWindowText(hwndEdit,cUserName,sizeof(cUserName));
			Sett_SpecialStrProc(cUserName);
//			if ( 0 == strlen(cUserName) )
//			{
//				PLXTipsWin(NULL, NULL, NULL,ML("Please definesnname"), NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
//				break;				
//			}
            strncpy(Global_IspInfo.ISPUserName,cUserName,sizeof(cUserName));
            IspSaveInfo(&Global_IspInfo,iCurMode);
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        break;
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
	}
return lResult;
}


static BOOL		CreateControl (HWND hWnd, HWND *hwndEdit)
{
   IMEEDIT ie;
   RECT rect;
   GetClientRect(hWnd, &rect);

   
   memset((void*)&ie, 0, sizeof(IMEEDIT));
   ie.hwndNotify   = hWnd;

   * hwndEdit = CreateWindow(
        "IMEEDIT",
        Global_IspInfo.ISPUserName,
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL | ES_TITLE,
        0,
        0,
        rect.right - rect.left,
        (rect.bottom - rect.top) / 3,
        hWnd,
        (HMENU)ID_EDIT,
        NULL,
        &ie);
         
   SendMessage(* hwndEdit, EM_LIMITTEXT, UDB_K_NAME_SIZE, 0);
   
   SendMessage(* hwndEdit, EM_SETTITLE, 0, (LPARAM)ML("User namecolon"));

   SendMessage(* hwndEdit, EM_SETSEL, -1, -1);
   return TRUE;
}


BOOL CallEditConnPasswd(HWND hWndApp,int icurMode)
{   
    WNDCLASS wc;
    RECT rClient;

    hFrameWin = hWndApp;
    iCurMode = icurMode;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = PWAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pPWClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    IspReadInfo(&Global_IspInfo, iCurMode);
    GetClientRect(hFrameWin, &rClient);
    hAppWnd = CreateWindow(pPWClassName, NULL, 
        WS_VISIBLE | WS_CHILD, 
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hAppWnd)
    {
        UnregisterClass(pPWClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON,//LSK
            MAKEWPARAM((WORD)1, (WORD)IDC_BUTTON_SAVE), (LPARAM)ML("Save"));
        
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
            IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));//RSK  
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    SetWindowText(hFrameWin, Global_IspInfo.ISPName);
    SetFocus(hAppWnd);

    return TRUE;
}

static LRESULT  PWAppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT     lResult;    
    static HWND hFocus,hwndEdit;//show the editor of scenemode name

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
	case WM_CREATE:             
        PWCreateControl(hWnd,&hwndEdit);
        hFocus = hwndEdit;
        break;
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;
    case WM_DESTROY ://
         hAppWnd = NULL;
         UnregisterClass(pPWClassName,NULL);
         break;

    case PWM_SHOWWINDOW:
        SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON,//LSK
            MAKEWPARAM((WORD)1, (WORD)IDC_BUTTON_SAVE), (LPARAM)ML("Save"));
        
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
            IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));//RSK  
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif

        SetWindowText(hFrameWin, Global_IspInfo.ISPName);
        SetFocus(hWnd);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {            
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SAVE,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;  
        
    case WM_COMMAND:            
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_CANCEL:
            DestroyWindow(hWnd);
            break;      
                
        case IDC_BUTTON_SAVE: 
            GetWindowText(hwndEdit,cPassWord,sizeof(cPassWord));
/*
			if ( strlen(cPassWord) > 0 )
			{
*/
				strncpy(Global_IspInfo.ISPPassword,cPassWord,sizeof(cPassWord));
				IspSaveInfo(&Global_IspInfo,iCurMode);
				SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
				PostMessage(hWnd, WM_CLOSE, 0, 0);
/*
			}
			else 
			{
				PLXTipsWin(NULL, NULL, NULL,ML("Please define password"), NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
			}
*/
			break;
        }
        break;
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
	}
return lResult;
}


static BOOL		PWCreateControl (HWND hWnd, HWND *hwndEdit)
{
   IMEEDIT ie;
   RECT rect;
   GetClientRect(hWnd, &rect);

   
   memset((void*)&ie, 0, sizeof(IMEEDIT));
   ie.hwndNotify   = hWnd;

   * hwndEdit = CreateWindow(
        "IMEEDIT",
        Global_IspInfo.ISPPassword,
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL | ES_TITLE | ES_PASSWORD,
        0,
        0,
        rect.right - rect.left,
        (rect.bottom - rect.top) / 3,
        hWnd,
        (HMENU)ID_EDIT,
        NULL,
        &ie);
         
   SendMessage(* hwndEdit, EM_LIMITTEXT, UDB_K_PWD_SIZE, 0);
   
   SendMessage(* hwndEdit, EM_SETTITLE, 0, (LPARAM)ML("Passwordcolon"));
   SendMessage(* hwndEdit, EM_SETSEL, -1, -1);
   return TRUE;
}
