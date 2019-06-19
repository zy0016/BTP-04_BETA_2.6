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
#include    "CallEditGateWay.h"

static HWND hAppWnd;
static HWND hFrameWin;
static int iCurMode;
static UDB_ISPINFO Global_IspInfo;
static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL		CreateControl (HWND hWnd, HWND *hwndEdit);

extern  BOOL IspReadInfo (UDB_ISPINFO * uIspInfo ,int iNum);
extern  BOOL IspSaveInfo (UDB_ISPINFO * uIspInfo ,int iNum);



BOOL CallEditGateWay(HWND hWndApp,int icurMode)
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
    static HWND hFocus,hwndEdit; 
    DWORD dwIP = 0;
    
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
        SetFocus(hWnd);
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
            break;
        }
        break;  
        
    case WM_COMMAND:            
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_CANCEL:
            DestroyWindow(hWnd);
            break;      
                
        case IDC_BUTTON_SAVE: 
            SendMessage(hwndEdit, UEM_GETDATA, 0, (LPARAM)&dwIP);
            Global_IspInfo.ConnectGateway = dwIP;
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
   RECT rect;
   GetClientRect(hWnd, &rect);

   * hwndEdit = CreateWindow(
        "UNITEDIT",
        ML("Gatewaycolon"),
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | UES_IPADDR | UES_TITLE,
        0,
        0,
        rect.right - rect.left,
        (rect.bottom - rect.top) / 3,
        hWnd,
        (HMENU)ID_EDIT,
        NULL,
        (LPVOID) Global_IspInfo.ConnectGateway);
   
   return TRUE;
}
