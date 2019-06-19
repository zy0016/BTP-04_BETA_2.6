#include    "CallSetGPSRefIntvl.h"
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


static HWND hWndApp;
static HWND hFrameWin;
static int  iCurGPSIndex;
static char * pClassName = "SetGPSRefIntvlClass";
static GPSMODE  Global_gm;
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL CreateControl (HWND hWnd, HWND *hList);

extern BOOL SetGPSprofile(GPSMODE *gm, int iGPSModeIndex);

BOOL CallSetGPSRefIntvl(HWND hParentWnd, int iGPSIndex)
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hParentWnd;

    iCurGPSIndex = iGPSIndex;


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
    
    GetGPSprofile(&Global_gm, iCurGPSIndex);

    GetClientRect(hFrameWin, &rClient);

    hWndApp = CreateWindow(pClassName,NULL, 
        WS_VISIBLE | WS_CHILD, 
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hWndApp)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
        (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );
    
    SetWindowText(hFrameWin, ML("Refresh interval"));
    SetFocus(hWndApp);
    
    return (TRUE);

}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;

	HDC                 hdc;

	static  int     iIndex=0,i;
    static  HWND    hList;
    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
    {

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");        
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0,(LPARAM)ML("Cancel") );         
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        
        SetWindowText(hFrameWin, ML("Refresh interval"));
        break;
   
    
    case WM_CREATE :
        CreateControl (hWnd, &hList);

		hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);
        iIndex = Global_gm.iRefIntervl;
		Load_Icon_SetupList(hList,hIconNormal,hIconSel,5,iIndex);
		SendMessage(hList,LB_SETCURSEL,iIndex,0);  
		break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_LIST));
        break;
        
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
       
		case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDM_OK,0);
             break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;  

   case WM_COMMAND :
       switch (LOWORD(wParam))
        {
        case IDM_OK:   
             iIndex = SendMessage(hList,LB_GETCURSEL,0,0); //get the current selected item number
             Global_gm.iRefIntervl = iIndex;
             SetGPSprofile(&Global_gm, iCurGPSIndex);
			 Load_Icon_SetupList(hList,hIconNormal,hIconSel,5,iIndex);     //change the radio button of the current selected item		
	    	 SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
             break;
        }
        break;

    case WM_DESTROY : 
	     DeleteObject(hIconNormal);
	     DeleteObject(hIconSel);
         hWndApp = NULL;
         UnregisterClass(pClassName,NULL);
         break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
	case WM_TIMER:
	    switch(wParam)
		{
		case TIMER_ASURE:
		    KillTimer(hWnd, TIMER_ASURE);
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
static BOOL CreateControl (HWND hWnd, HWND *hList)
{
	RECT rect;
	GetClientRect(hWnd, &rect);

    * hList = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL/*|CS_NOSYSCTRL*/,   
        rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
        hWnd, (HMENU)IDC_LIST, NULL, NULL);
    if (* hList == NULL )
        return FALSE;
    
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Off"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("GPSRefInt1s"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("GPSRefInt3s"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("GPSRefInt10s"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("GPSRefInt30min"));

    return TRUE;    
}
