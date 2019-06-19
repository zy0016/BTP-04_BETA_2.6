  /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setting
 *
 * Purpose  : create the date format setup window
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include	"CallDateFormat.h"
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
#include    "pubapp.h"

static  HWND        DF_hwndApp = NULL;
static  HWND        hFrameWin = NULL;
static  LRESULT  AppWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL     CreateControl(HWND hWnd,HWND * hListContent);
extern  BOOL     SetDateFormt(DATEFORMAT df_format);
/***************************************************************
* Function  CallDateFormat
* Purpose   register the class and create the main window     
* Params
* Return    TRUE: Success; FALSE: Failed
* Remarks
***************************************************************/
BOOL CallDateFormat(HWND hwndCall)
{    
	WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hwndCall;

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

    GetClientRect(hFrameWin, &rClient);

    DF_hwndApp = CreateWindow(pClassName,NULL, 
        WS_VISIBLE | WS_CHILD,                    
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin,
        NULL, NULL, NULL);

    if (NULL == DF_hwndApp)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT, (LPARAM)ML("Cancel")); 
        

    SetWindowText(hFrameWin, ML("Date format"));
    SetFocus(DF_hwndApp);

    return (TRUE);
}
/***************************************************************
* Function  Window processing function
* Purpose        
* Params
* Return    
* Remarks
***************************************************************/
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
	static  int     iIndex=0,i;
    static  HWND    hDateFormat,hFocus;
    LRESULT lResult;

    lResult = TRUE;   

    switch (wMsgCmd)
	{
		case WM_CREATE :
            CreateControl (hWnd, &hDateFormat);
		    hdc         = GetDC(hWnd);
            hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		    ReleaseDC(hWnd,hdc);		
		    iIndex = GetDateFormt();
		    PostMessage(hDateFormat,LB_SETCURSEL,iIndex,0);
		    Load_Icon_SetupList(hDateFormat,hIconNormal,hIconSel,9,iIndex);
		    break;

		case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_DATEFORMAT));
            break;

        case PWM_SHOWWINDOW:
            SetFocus(hWnd);
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT, (LPARAM)ML("Cancel"));         
            SetWindowText(hFrameWin, ML("Date format"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
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
         
		case WM_DESTROY : 
	        DeleteObject(hIconNormal);
	        DeleteObject(hIconSel);
		    KillTimer(hWnd, TIMER_ASURE);
		    DF_hwndApp = NULL;
            UnregisterClass(pClassName,NULL);
            break;

	   case WM_COMMAND :
            switch (LOWORD(wParam))
		    {
                case IDM_OK:   
	    	        iIndex = SendMessage(hDateFormat,LB_GETCURSEL,0,0);              
                    //get the current selected item number
		            Load_Icon_SetupList(hDateFormat,hIconNormal,hIconSel,9,iIndex);
                         //change the radio button of the current selected item		
                    SetDateFormt(iIndex);
				    SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
                    break;
		    }
            break;

        case WM_TIMER:
	        switch(wParam)
		    {
		        case TIMER_ASURE:
                    SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
			        break;
                default:
                    break;
		    }
		    break;
        default :
            lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
            break;
	}
    return lResult;
}
/***************************************************************
* Function  CreateControl
* Purpose   create the control components of the window         
* Params
* Return    
* Remarks
***************************************************************/
static  BOOL    CreateControl(HWND hWnd,HWND * hDateFormat)
{
	int i;
	RECT rect;
    const char * szDateFormat[9] =
    {
        ML("dd.mm.yyyy"),
        ML("mm.dd.yyyy"),
        ML("yyyy.mm.dd"),
        ML("ddslmmslyyyy"),
        ML("mmslddslyyyy"),
        ML("yyyyslmmsldd"),
        ML("ddtommtoyyyy"),
        ML("mmtoddtoyyyy"),
        ML("yyyytommtodd")
    };

	GetClientRect(hWnd, &rect);
    * hDateFormat = CreateWindow("LISTBOX", 0, 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
        hWnd, (HMENU)IDC_DATEFORMAT, NULL, NULL);
    if (* hDateFormat == NULL )
        return FALSE;
    for(i=0;i<9;i++)
	SendMessage(* hDateFormat,LB_ADDSTRING,i,(LPARAM)szDateFormat[i]);

    return TRUE;
}
/***************************************************************
* Function  Load_Icon_SetupList
* Purpose   show the icons of each options         
* Params    hListContent: the list box containing options
*           hIconNormal: the icon for the option that is not being chosen
*           hIconSel: the icon for the option that is chosen
*           iIndex: the index number of the option that is chosen
* Return    
* Remarks
***************************************************************/
BOOL Load_Icon_SetupList(HWND hSetupList,HBITMAP hIconNormal,HBITMAP hIconSel,int iItemNum, int iSelIndex)
{    
	DWORD dWord;
	int i;
		    
	for(i=0;i<iItemNum;i++)
	{
		dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
		SendMessage(hSetupList,LB_SETIMAGE,(WPARAM)dWord, (LPARAM)hIconNormal);
	}

	if (iSelIndex == -1)
		return TRUE;
	else
	{
	    dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)iSelIndex);
	    SendMessage(hSetupList,LB_SETIMAGE,(WPARAM)dWord, (LPARAM)hIconSel);
	}
    return TRUE;
}
