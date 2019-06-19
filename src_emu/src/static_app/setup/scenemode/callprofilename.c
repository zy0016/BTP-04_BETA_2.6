/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : sound profile
 *
 * Purpose  : editing the name of profile & edit the vibration state  
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
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
#include    "callprofilename.h"

#define SETT_VIBRA_TIME 300
static HWND hAppWnd= NULL;
static HWND hVAppWnd= NULL;
static HWND hFrameWin = NULL;
static int iCurMode;
static SCENEMODE sm;
static char cCurMode[MODENAMEMAXLEN+1] = "";
static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  OnOffAppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL		CreateControl (HWND hWnd, HWND *hwndEdit);

BOOL CallVibraSettingWnd(HWND hParentWnd, int icurmode);
int Sett_SpecialStrProc(char *str);

BOOL CallProfileName(HWND hWndApp,int icurMode)//set the name of user defined scenemode
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

    GetSM(&sm, iCurMode);

    GetClientRect(hFrameWin, &rClient);
    
    hAppWnd = CreateWindow(pClassName,NULL, 
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
    
    SetWindowText(hFrameWin, sm.cModeName);
    
    SetFocus(hAppWnd);
    
    return TRUE;
}

static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT     lResult;    
    static HWND hFocus,hwndEdit;//show the editor of scenemode name
    static char cTitleName[MODENAMEMAXLEN];
	lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
	case WM_CREATE:
        CreateControl(hWnd,&hwndEdit);
        break;
    case WM_SETFOCUS:
        SetFocus(hwndEdit);
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
		GetWindowText(hwndEdit,cCurMode,sizeof(cCurMode));
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
            break;
        }
        break;  

    	case WM_COMMAND:

		    switch (LOWORD(wParam))
			{
            case IDC_BUTTON_SAVE: 				
				 GetWindowText(hwndEdit,cCurMode,sizeof(cCurMode));
				 Sett_SpecialStrProc(cCurMode);
                 
                 if(!strlen(cCurMode))
                 {
                     PLXTipsWin(hFrameWin, NULL, NULL,ML("Please define profile name"),cCurMode,Notify_Alert,ML("Ok"),NULL,WAITTIMEOUT);
                     break;
                 }
                 strncpy(sm.cModeName,cCurMode,sizeof(cCurMode));                 
				 SetSM(&sm,iCurMode);
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

//   strncpy(cCurMode,sm.cModeName,sizeof(cCurMode));
   memset((void*)&ie, 0, sizeof(IMEEDIT));
   ie.hwndNotify   = hWnd;

   * hwndEdit = CreateWindow(
        "IMEEDIT",
        sm.cModeName,
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL | ES_TITLE,
        0,
        0,
        rect.right - rect.left,
        (rect.bottom - rect.top) / 3,
        hWnd,
        (HMENU)ID_EDIT,
        NULL,
        &ie);
         
   SendMessage(* hwndEdit, EM_LIMITTEXT, MODENAMEMAXLEN, 0);
   
   SendMessage(* hwndEdit, EM_SETTITLE, 0, (LPARAM)ML("Namecolon"));
   SendMessage(* hwndEdit, EM_SETSEL, -1, -1);
   return TRUE;
}

BOOL CallVibraSettingWnd(HWND hParentWnd, int icurmode)
{	

	WNDCLASS wc;
    RECT rClient;
    hFrameWin = hParentWnd;
    iCurMode = icurmode;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = OnOffAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pOnOffClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);

    hVAppWnd = CreateWindow(pOnOffClassName,NULL, 
        WS_VISIBLE | WS_CHILD,          
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hVAppWnd)
    {
        UnregisterClass(pOnOffClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));//RSK
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
           
    
    SetWindowText(hFrameWin, ML("Vibration"));
    SetFocus(hVAppWnd);
    return TRUE;
}
static LRESULT  OnOffAppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static      HWND        hOnOffList;
    static      SCENEMODE scenemode;
    LRESULT     lResult;
    static int iscreenw,iscreenh,iIndex;
    HDC hdc;
 	RECT rect;
    static HBITMAP hIconNormal,hIconSel;
    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:
		GetClientRect(hWnd, &rect);
        
        hOnOffList = CreateWindow("LISTBOX", 0, 
                                    WS_VISIBLE | WS_CHILD | LBS_BITMAP|LBS_NOTIFY,   
									rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
                                    hWnd, (HMENU)IDC_ONOFFLIST, NULL, NULL);
        if (hOnOffList == NULL )
        hOnOffList = FALSE;

        SendMessage(hOnOffList,LB_ADDSTRING,0,(LPARAM)ML("On"));
        SendMessage(hOnOffList,LB_ADDSTRING,0,(LPARAM)ML("Off"));

        GetSM(&scenemode, iCurMode);
        SendMessage(hOnOffList,LB_SETCURSEL,scenemode.iVibraSwitch,0);
        
        hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);

		Load_Icon_SetupList(hOnOffList,hIconNormal,hIconSel,2,scenemode.iVibraSwitch);
        

        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_ONOFFLIST));
        break;

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));//RSK
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");          
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
    
        SetWindowText(hFrameWin, ML("Vibration"));
        break;
        
    case WM_DESTROY:
        hVAppWnd = NULL;
	    DeleteObject(hIconNormal);
	    DeleteObject(hIconSel);
	    KillTimer(hWnd, TIMER_ASURE);
        UnregisterClass(pOnOffClassName,NULL);
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
            iIndex = SendMessage(hOnOffList,LB_GETCURSEL,0,0);             //get the current selected item number
			if ( (scenemode.iVibraSwitch = iIndex) == SWITCH_ON )
			{
				PrioMan_Vibration(1);
			}
            SetSM(&scenemode, iCurMode);
		    Load_Icon_SetupList(hOnOffList,hIconNormal,hIconSel,2,iIndex);     //change the radio button of the current selected item		
            SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
        	break;
        }
        break;
	case WM_TIMER:
	    switch(wParam)
		{
		case TIMER_ASURE:
			KillTimer(hWnd, TIMER_ASURE);
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		default:
			break;
		}
		break;
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

