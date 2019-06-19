/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : general phone setting
 *
 * Purpose  : 
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "genphosetting.h"
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

static  const   char * pClassName = "GenPhoSettingWndClass";
static  HWND    hWndApp=NULL;
static  HWND    hFrameWin=NULL;

static  TCHAR** LangName;
static  int     nLang;
static  int     iLangFlag = 0;
static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND ,HWND * ,HWND * ,HWND * );

extern  BOOL 	CallLangSettingWindow(HWND hFrame);
extern  BOOL    CallWriLangSettingWindow(HWND hwndCall);
extern  BOOL    SetWriLanguage(iIndex);
extern  BOOL    CallPreInputWindow(HWND hwndCall);
extern  BOOL    SetPreInputSetting(SWITCHTYPE predictinput);
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);

BOOL GenPhoSetting(HWND hwndCall)
{
    WNDCLASS    wc;
    RECT rClient;

/*******Get the language string and amount*****/

    if(!GetLanguageInfo(NULL,&nLang)) //get count
        return FALSE;

	
    if(!GetLanguageInfo(&LangName,NULL)) //get language string
        return FALSE;

/******************End*************************/

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

    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
                (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back") );
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SetWindowText(hFrameWin,ML("Language"));


    ShowWindow(hFrameWin, SW_SHOW);
    UpdateWindow(hFrameWin); 
    SetFocus(hWndApp);

    return (TRUE);
}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HWND        hPhoLang = NULL, hWritLang = NULL, hPreInput = NULL;
    static      HWND        hFocus = 0;
    static      int         i,iButtonJust,iIndex,iCurIndex;
    LRESULT     lResult;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
        case PWM_SHOWWINDOW:    
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0,(LPARAM)ML("Back"));                
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
            SetWindowText(hFrameWin,ML("Language"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
 
			if(iLangFlag == 1)
			{
				SendMessage(hPhoLang, SSBM_SETTITLE, 0, (LPARAM)ML("Phone language"));
				SendMessage(hWritLang, SSBM_SETTITLE, 0, (LPARAM)ML("Writing language"));
				SendMessage(hPreInput, SSBM_SETTITLE, 0, (LPARAM)ML("Predictive input"));
				SendMessage(hPreInput, SSBM_DELETESTRING, 0, 0);
				SendMessage(hPreInput, SSBM_DELETESTRING, 0, 0);
				SendMessage(hPreInput, SSBM_ADDSTRING, 0,(LPARAM)ML("On"));
				SendMessage(hPreInput, SSBM_ADDSTRING, 0,(LPARAM)ML("Off"));		
				
				iLangFlag = 0;
			}
            SendMessage(hPhoLang,SSBM_DELETESTRING,0,0); 
            
            if(TRUE == GetAutoLangState()) //selected automatic
                SendMessage(hPhoLang,SSBM_ADDSTRING,0,(LPARAM)ML("Automatic"));
            else
            {
                for(i=0;i<nLang;i++)
                {
                    if(strcmp(GetActiveLanguage(),LangName[i])==0)
                        iIndex = i;
                }
                if( iIndex<nLang && iIndex>=0 )
				{
					if (strcmp(LangName[iIndex], "Chinese") == 0)
						SendMessage(hPhoLang,SSBM_ADDSTRING,0,(LPARAM)ML("Chinese"));
					else
						SendMessage(hPhoLang,SSBM_ADDSTRING,0,(LPARAM)LangName[iIndex]);
				}
             }          
            

            iIndex = GetWriLanguage();
            SendMessage(hWritLang,SSBM_SETCURSEL,(WPARAM)iIndex,0);
            
            iIndex = GetPreInputSetting();
            SendMessage(hPreInput,SSBM_SETCURSEL,(WPARAM)iIndex,0);
            SetFocus(hWnd);
            break;

        
        case WM_CREATE :
			CreateControl(hWnd, &hPhoLang, &hWritLang, &hPreInput);
			hFocus=hPhoLang;
            
            if(TRUE == GetAutoLangState()) //selected automatic
                SendMessage(hPhoLang,SSBM_ADDSTRING,0,(LPARAM)ML("Automatic"));
            else
            {
                for(i=0;i<nLang;i++)
                {
                    if(strcmp(GetActiveLanguage(),LangName[i])==0)
                        iIndex = i;
                }
                if( iIndex<nLang && iIndex>=0 )
				{
					if (strcmp(LangName[iIndex], "Chinese") == 0)
						SendMessage(hPhoLang,SSBM_ADDSTRING,0,(LPARAM)ML("Chinese"));
					else
						SendMessage(hPhoLang,SSBM_ADDSTRING,0,(LPARAM)LangName[iIndex]);
				}
             }          
            
            iIndex = GetWriLanguage();
            SendMessage(hWritLang,SSBM_SETCURSEL,(WPARAM)iIndex,0);
            
            iIndex = GetPreInputSetting();
            SendMessage(hPreInput,SSBM_SETCURSEL,(WPARAM)iIndex,0);
		    break;

		case WM_SETFOCUS:
            SetFocus(hFocus);
            break;

		case WM_DESTROY : 
            hWndApp = NULL;
            UnregisterClass(pClassName,NULL);
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
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
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
		    default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
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

		case PWM_ACTIVATE:
			if (wParam == WA_INACTIVE)
			{
				KillTimer(hWnd, TIMER_ID);
				RepeatFlag = 0;
				wKeyCode   = 0;
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
            switch( LOWORD( wParam ))
		    {
		    case IDC_BUTTON_SET:
			    switch(iButtonJust)
			    {
			    case IDC_PHOLANG:
				    CallLangSettingWindow(hFrameWin);
				    break;
                case IDC_WRILANG:
                    CallWriLangSettingWindow(hFrameWin);
                    break;
                case IDC_PREINPUT:
                    CallPreInputWindow(hFrameWin);
                    break;
			    }
                break;
        
		    case IDC_PHOLANG:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                {
                    iButtonJust = IDC_PHOLANG;
                    hFocus = hPhoLang;
                }
                break;
		    case IDC_WRILANG:
			    if(HIWORD( wParam ) == SSBN_CHANGE)
			    {
				iCurIndex = SendMessage(hWritLang,SSBM_GETCURSEL,0,0);
				SetWriLanguage(iCurIndex);
			    }
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                {
                    iButtonJust = IDC_WRILANG;
                    hFocus = hWritLang;
                }
                break;
		    case IDC_PREINPUT:
			    if(HIWORD( wParam ) == SSBN_CHANGE)
			    {
				iCurIndex = SendMessage(hPreInput,SSBM_GETCURSEL,0,0);
				SetPreInputSetting(iCurIndex);
			    }
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                {
                    iButtonJust = IDC_PREINPUT;
                    hFocus = hPreInput;
                }
                break;
		    }
		    break;

	    default :
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
	}
	 return lResult;
}

static  BOOL    CreateControl(HWND hwnd,HWND * hPhoLang,HWND * hWritLang,HWND * hPreInput)
{
	int     i,xzero=0,yzero=0;
    int iControlH,iControlW;
    RECT rect;
    GetClientRect(hwnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;

	
    * hPhoLang = CreateWindow( "SPINBOXEX", ML("Phone language"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hwnd, (HMENU)IDC_PHOLANG, NULL, NULL);

    if (* hPhoLang == NULL)
        return FALSE;
   
	* hWritLang = CreateWindow( "SPINBOXEX", ML("Writing language"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH), iControlW, iControlH, 
        hwnd, (HMENU)IDC_WRILANG, NULL, NULL);

    if (* hWritLang == NULL)
        return FALSE;

	for(i=0;i<nLang;i++)
    {
		if (strcmp(LangName[i], "Chinese")==0)
			SendMessage(* hWritLang, SSBM_ADDSTRING, i,(LPARAM)ML("Chinese"));
		else
			SendMessage(* hWritLang, SSBM_ADDSTRING, i,(LPARAM)LangName[i]);
	}

    * hPreInput = CreateWindow( "SPINBOXEX", ML("Predictive input"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 2, iControlW, iControlH, 
        hwnd, (HMENU)IDC_PREINPUT, NULL, NULL);

    if (* hPreInput == NULL)
        return FALSE;

    SendMessage(* hPreInput, SSBM_ADDSTRING, 0,(LPARAM)ML("On"));
    SendMessage(* hPreInput, SSBM_ADDSTRING, 0,(LPARAM)ML("Off"));

    return TRUE;
}
void Sett_ProcLangChanged(void)
{
	iLangFlag = 1;
}
