#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "str_plx.h"
#include    "me_wnd.h"
#include    "plx_pdaex.h"
#include    "str_public.h"
#include    "setting.h"
#include    "setup.h"
#include    "mullang.h"
#include	"sys/types.h"
#include	"sys/stat.h"
#include	"fcntl.h"
#include	"unistd.h" 
#include    "ioctl.h"


#define IDC_THEME_LIST 100
#define IDC_BUTTON_QUIT 101
#define IDC_BUTTON_OK  102
#define TIMER_ASURE WM_USER + 100
static HWND hWndApp  = NULL;
static HWND hFrameWin = NULL;
static const char *pClassName = "SettColorThemeClass";
static LRESULT  AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

extern HDC GetBackDisplayhDC(void);

BOOL CallColorThemeSetting(HWND hWnd)
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hWnd;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    
    GetClientRect(hFrameWin, &rClient);
    
    hWndApp = CreateWindow(pClassName,NULL, 
        WS_VISIBLE | WS_CHILD, 
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, NULL, NULL, NULL);

    if (NULL == hWndApp)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );
    SetWindowText(hFrameWin, ML("Colour theme"));

    SetFocus(hWndApp);

    return (TRUE);

}
static LRESULT  AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hThemeList;
    LRESULT     lResult;
    RECT rect;
	static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;
    static      int iIndex = 0;
    HDC         hdc;
    int         iCurSel = 0;
    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:        
		hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);
		
        GetClientRect(hWnd, &rect);

        hThemeList = CreateWindow("LISTBOX", 0, 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
            0,0,rect.right - rect.left, rect.bottom - rect.top,
            hWnd, (HMENU)IDC_THEME_LIST, NULL, NULL);
        
        if (hThemeList == NULL )
            lResult = FALSE;

        SendMessage(hThemeList,LB_ADDSTRING,0,(LPARAM)ML("Theme 1"));
        SendMessage(hThemeList,LB_ADDSTRING,0,(LPARAM)ML("Theme 2"));
        SendMessage(hThemeList,LB_ADDSTRING,0,(LPARAM)ML("Theme 3"));
        SendMessage(hThemeList,LB_ADDSTRING,0,(LPARAM)ML("Theme 4"));
    
        Load_Icon_SetupList(hThemeList,hIconNormal,hIconSel,4,iCurSel);
        
        SendMessage(hThemeList,LB_SETCURSEL,iCurSel,0);
   	    break;

    case PWM_SHOWWINDOW:
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel"));           
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SetWindowText(hFrameWin, ML("Colour theme"));
        SetFocus(hWnd);        
        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_THEME_LIST));
    	break;

    case WM_DESTROY:
        hWndApp = NULL;
        DeleteObject(hIconNormal);
        DeleteObject(hIconSel);
        UnregisterClass(pClassName,NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_OK,0);
            break;
		default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
        }
        break;
    case WM_COMMAND:       
        switch(LOWORD( wParam )) 
        {
        case IDC_BUTTON_OK:
            iIndex = SendMessage(hThemeList,LB_GETCURSEL,0,0);
            Load_Icon_SetupList(hThemeList,hIconNormal,hIconSel,4,iIndex);
            SetTimer(hWnd, TIMER_ASURE, 500, NULL);
        	break;
        }
        break;

	case WM_TIMER:
	    switch(wParam)
		{
		case TIMER_ASURE:
            KillTimer(hWnd, TIMER_ASURE);
			{
				if ( 0 == iIndex )
				{
				    DrawBitmapFromFile(GetBackDisplayhDC(), 0, 0, SETT_BKGRD_PIC, SRCCOPY);    
				}
				if ( 1 == iIndex )
				{
				    DrawBitmapFromFile(GetBackDisplayhDC(), 0, 0, SETT_BKGRD_PIC_1, SRCCOPY);    
				}
				if ( 2 == iIndex)
				{
				    DrawBitmapFromFile(GetBackDisplayhDC(), 0, 0, SETT_BKGRD_PIC_2, SRCCOPY);    
				}
				if ( 3 == iIndex)
				{
				    DrawBitmapFromFile(GetBackDisplayhDC(), 0, 0, SETT_BKGRD_PIC_3, SRCCOPY);    
				}

			}
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

void ModifyColorTheme()
{

}
