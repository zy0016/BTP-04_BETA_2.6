/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : sound profile
 *
 * Purpose  : Missed event setting
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "CallMissedEvent.h"
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

static int iCurMode;
static HWND hAppWnd = NULL;
static HWND hMETAppWnd = NULL;
static HWND hFrameWin = NULL;

static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static SCENEMODE sm;
static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  METAppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL		CreateControl (HWND hWnd, HWND *hTimeInter,HWND *hVolume);
static BOOL    METCreateControl(HWND hWnd, HWND *hList);

BOOL   CallMissEvenTime(HWND hcallApp,int icurmode);

extern BOOL     CallLevelAdjuster(HWND hFrame, HWND hcallWnd, ADJTYPE adjType, int nlevel,UINT wCallbackMsg, char* szTitle);
extern void     IntToString( int nValue, char *buf );
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);

BOOL   CallMissedEvent(HWND hcallApp,int icurmode)
{
	WNDCLASS wc;
    RECT rClient;

    hFrameWin = hcallApp;
    iCurMode = icurmode;

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
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,IDC_BUTTON_CANCEL, (LPARAM)ML("Back"));//RSK        

    SetWindowText(hFrameWin, ML("Missed events"));
    SetFocus(hAppWnd);


    return TRUE;

}

static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{    
    LRESULT     lResult;   
	static HWND hTimeInter,hVolume;
    static HWND hFocus = 0;
    static int  iButtonJust, iCurIndex;
	static int nCurVol;
    lResult = (LRESULT)TRUE;    
    switch ( wMsgCmd )
    {
    case WM_CREATE:
        GetSM(&sm, iCurMode);
		
		nCurVol = sm.mEvents.iEventVolume;

        CreateControl(hWnd,&hTimeInter,&hVolume);
        SendMessage(hVolume, SSBM_SETCURSEL, sm.mEvents.iEventVolume, 0);
        SendMessage(hTimeInter, SSBM_SETCURSEL, sm.mEvents.iTimeInterval, 0);
        hFocus = hTimeInter;
        break;
    
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

	case PWM_ACTIVATE:
		if (wParam == WA_INACTIVE)
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
			wKeyCode   = 0;
		}
		break;
         
    case WM_DESTROY :    
        hAppWnd = NULL;
        UnregisterClass(pClassName,NULL);              
        break;

      
    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,IDC_BUTTON_CANCEL, (LPARAM)ML("Back"));//RSK        
        
        SetWindowText(hFrameWin, ML("Missed events"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        
        SendMessage(hTimeInter, SSBM_SETCURSEL, sm.mEvents.iTimeInterval, 0);
        
        break;
    
    case WM_KEYDOWN:             
		if (RepeatFlag > 0 && wKeyCode != LOWORD(wParam))
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
		}

		wKeyCode = LOWORD(wParam);
		RepeatFlag++;

        switch(LOWORD(wParam))
			 {       
	          case VK_F10:
                  SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                  PostMessage(hWnd, WM_CLOSE, 0, 0);
                  break;

              case VK_F5:
                   SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_OK,0);
                   break;
              case VK_DOWN:
                  SettListProcessKeyDown(hWnd, &hFocus);            
				  if ( 1 == RepeatFlag )
				  {
					  SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
				  }
                  break;
                  
              case VK_UP:
                  SettListProcessKeyUp(hWnd, &hFocus);            
				  if ( 1 == RepeatFlag )
				  {
					  SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
				  }
                  break;
             }
        break;
    case WM_TIMER:
	    switch(wParam)
		{
		case TIMER_ID:
            if (1 == RepeatFlag)
            {
                KillTimer(hWnd, TIMER_ID);
                SetTimer(hWnd, TIMER_ID, ET_REPEAT_LATER, NULL);
            }
			keybd_event(wKeyCode, 0, 0, 0);
			break;
       default:
			KillTimer(hWnd, wParam);
            break;
		}
		break;
        
    case WM_KEYUP:
		RepeatFlag = 0;
        switch(LOWORD(wParam))
        {
        case VK_UP:
        case VK_DOWN:
			KillTimer(hWnd, TIMER_ID);
            break;
        default:
            break;
        }        
        break;
                
	 case WM_COMMAND:
		 switch(LOWORD(wParam))
         {
         case IDC_BUTTON_CANCEL:
             DestroyWindow(hWnd);
             break;
         case IDC_BUTTON_OK:
             switch(iButtonJust)
             {
             case IDC_TIMEINTER:
                 CallMissEvenTime(hFrameWin, iCurMode);
                 break;
             case IDC_VOLUME:
                 CallLevelAdjuster(hFrameWin, hWnd,LAS_VOL,sm.mEvents.iEventVolume,CALLBACK_SETVOL,(char*)ML("Volume level"));
                 break;
             }
             break;
         case IDC_TIMEINTER:
             if(HIWORD( wParam ) == SSBN_CHANGE)
             {
                 iCurIndex = SendMessage(hTimeInter,SSBM_GETCURSEL,0,0);
                 sm.mEvents.iTimeInterval = iCurIndex;
                 SetSM(&sm, iCurMode);
             }
             if(HIWORD(wParam) == SSBN_SETFOCUS)  
             {
             iButtonJust = IDC_TIMEINTER;
             hFocus = hTimeInter;                 
             }
             break;
         case IDC_VOLUME:
			 if(HIWORD( wParam ) == SSBN_CHANGE)
			 {
				 nCurVol = SendMessage(hVolume,SSBM_GETCURSEL,0,0);
				 sm.mEvents.iEventVolume = nCurVol;
				 SetSM(&sm, iCurMode);
			 }
             if(HIWORD(wParam) == SSBN_SETFOCUS)  
             {
                iButtonJust = IDC_VOLUME;
                hFocus = hVolume;
             }
             break;              

         }
         break;    
    case CALLBACK_SETVOL:
		if (wParam == 1)
		{
			sm.mEvents.iEventVolume = lParam;
			SendMessage(hVolume, SSBM_SETCURSEL, lParam, 0);
			SetSM(&sm, iCurMode);
		}
        break;
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;         
    }
return lResult; 
}
static BOOL CreateControl(HWND hWnd, HWND * hTimeInter, HWND * hVolume)
{    
    int iControlH,iControlW;
	int xzero=0,yzero=0;

	RECT    rect;
    GetClientRect(hWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;
	

    
    * hTimeInter = CreateWindow( "SPINBOXEX", ML("Time interval"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hWnd, (HMENU)IDC_TIMEINTER, NULL, NULL);

    if (* hTimeInter == NULL)
        return FALSE;  
    SendMessage(*hTimeInter,SSBM_ADDSTRING,0,(LPARAM)ML("Off"));
    SendMessage(*hTimeInter,SSBM_ADDSTRING,0,(LPARAM)ML("30 sec"));
    SendMessage(*hTimeInter,SSBM_ADDSTRING,0,(LPARAM)ML("1 min"));
    SendMessage(*hTimeInter,SSBM_ADDSTRING,0,(LPARAM)ML("2 min"));
    SendMessage(*hTimeInter,SSBM_ADDSTRING,0,(LPARAM)ML("5 min"));
    SendMessage(*hTimeInter,SSBM_ADDSTRING,0,(LPARAM)ML("10 min"));
 
    * hVolume = CreateWindow( "LEVIND", ML("Volume level"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH, iControlW, iControlH, 
        hWnd, (HMENU)IDC_VOLUME, NULL, (PVOID)LAS_VOL);

    if (* hVolume == NULL)
        return FALSE;  
    return TRUE;
}

BOOL   CallMissEvenTime(HWND hcallApp,int icurmode)
{
	WNDCLASS wc;
    RECT rClient;

    hFrameWin = hcallApp;
    iCurMode = icurmode;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = METAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pMETClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);

    hMETAppWnd = CreateWindow(pMETClassName, NULL, 
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
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));//RSK        

    SetWindowText(hFrameWin, ML("Time interval"));
    SetFocus(hMETAppWnd);


    return TRUE;

}
static LRESULT  METAppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
	static  int     iIndex=0,i;
    static  HWND    hList,hFocus;
    LRESULT lResult;

    lResult = TRUE;   

    switch (wMsgCmd)
	{
		case WM_CREATE :
            METCreateControl (hWnd, &hList);
            
		    hdc         = GetDC(hWnd);
            hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		    ReleaseDC(hWnd,hdc);	
            
            iIndex = sm.mEvents.iTimeInterval;
		    Load_Icon_SetupList(hList,hIconNormal,hIconSel,6,iIndex);
		    SendMessage(hList,LB_SETCURSEL,iIndex,0);
		    break;

		case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_LIST));
            break;

        case PWM_SHOWWINDOW:
            SetFocus(hWnd);
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));//RSK        
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            
            SetWindowText(hFrameWin, ML("Time interval"));
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
         
		case WM_DESTROY : 
	        DeleteObject(hIconNormal);
	        DeleteObject(hIconSel);
		    KillTimer(hWnd, TIMER_ASURE);
		    hMETAppWnd = NULL;
            UnregisterClass(pMETClassName,NULL);
            break;

	   case WM_COMMAND :
            switch (LOWORD(wParam))
		    {
                case IDC_BUTTON_OK:   
	    	        iIndex = SendMessage(hList,LB_GETCURSEL,0,0);              
		            Load_Icon_SetupList(hList,hIconNormal,hIconSel,6,iIndex);
                    sm.mEvents.iTimeInterval = iIndex;
                    SetSM(&sm, iCurMode);
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

static BOOL    METCreateControl(HWND hWnd, HWND *hList)
{
    RECT rect;
    GetClientRect(hWnd, &rect);

    * hList = CreateWindow("LISTBOX", 0, 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0,0, rect.right - rect.left,rect.bottom - rect.top,
        hWnd, (HMENU)IDC_LIST, NULL, NULL);
    if (* hList == NULL )
        return FALSE;

    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Off"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("30 sec"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("1 min"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("2 min"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("5 min"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("10 min"));
   return TRUE;
}
