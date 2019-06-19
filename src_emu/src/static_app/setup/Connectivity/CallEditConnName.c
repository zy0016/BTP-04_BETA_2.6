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
#include    "CallEditConnName.h"

static HWND hAppWnd;
static HWND hFrameWin;
static int iCurMode;
static UDB_ISPINFO Global_IspInfo;
static char cCurMode[UDB_K_NAME_SIZE+1] = "";
static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL		CreateControl (HWND hWnd, HWND *hwndEdit);

extern  BOOL IspReadInfo (UDB_ISPINFO * uIspInfo ,int iNum);
extern  BOOL IspSaveInfo (UDB_ISPINFO * uIspInfo ,int iNum);



BOOL CallEditConnName(HWND hWndApp,int icurMode)
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
	strncpy(cCurMode, Global_IspInfo.ISPName, sizeof(cCurMode));
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

    SetWindowText(hFrameWin, cCurMode);
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

        SetWindowText(hFrameWin, cCurMode);
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
            GetWindowText(hwndEdit,cCurMode,sizeof(cCurMode));
			Sett_SpecialStrProc(cCurMode);
            if(!strlen(cCurMode))
            {
//                PLXTipsWin(ML("Please define name"),Global_IspInfo.ISPName,Notify_Alert,ML("Ok"),0,WAITTIMEOUT);
                PLXTipsWin(hFrameWin, hWnd, NULL,ML("Please definesnname"),cCurMode,Notify_Alert,ML("Ok"),0,WAITTIMEOUT);
                break;
            }
            
            strncpy(Global_IspInfo.ISPName,cCurMode,sizeof(cCurMode));
            IspSaveInfo(&Global_IspInfo,iCurMode);
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;

		case ID_EDIT:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				GetWindowText(hwndEdit,cCurMode,sizeof(cCurMode));
				SetWindowText(hFrameWin, cCurMode);
			}
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
        Global_IspInfo.ISPName,
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
   
   SendMessage(* hwndEdit, EM_SETTITLE, 0, (LPARAM)ML("Connection namecolon"));

   SendMessage(* hwndEdit, EM_SETSEL, -1, -1);

   return TRUE;
}
