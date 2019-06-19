 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : display setting
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

#include    "displaysetting.h"


static HWND     hFrameWin = NULL;

static HWND     hWndApp = NULL;
static HWND     hSSWndApp = NULL;
static HWND     hSEWndApp = NULL;    
static HWND     hWSTWndApp = NULL;    
static HWND     hBLWndApp = NULL;    
static HWND     hDIS_BK_LIT_WndApp = NULL;
static HWND     hDIS_KP_LIT_WndApp = NULL;
static HWND     hInfo_WndApp = NULL;
static HWND     hFather = NULL;
static HWND		hIndLEDApp = NULL;
static int      nCurFocus;

static int      iCurBrit;
static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static LRESULT  AppWndProc   ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  SSAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  SEAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  WSTAppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  BLAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  DIS_BK_LIT_AppWndProc   ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  DIS_KP_LIT_AppWndProc   ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  Info_AppWndProc   ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  IndLED_WndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

static void     DisplaySet_InitVScrolls(HWND hWnd,int iItemNum);
static void     DisplaySet_OnVScroll(HWND hWnd,  UINT wParam);
static BOOL     CreateControl(HWND hwnd,HWND * hBright,HWND * hScrnSaver,HWND * hColorTheme,
                              HWND * hStartup,HWND * hIndicatorLed,HWND * hLights, HWND * hInfoline);
static BOOL     CallScreenSaverSetting(HWND hwndCall);
static BOOL     CallWriteStartupText(HWND hwndCall, HWND hFatherWnd);
static BOOL     CallBackLitSetting(HWND hwndCall);
static BOOL     CallDisBackLitSetting(HWND hFatherWnd);
static BOOL     CallDisKeypadLitSetting(HWND hFatherWnd);
static BOOL     CallInfoLineSetting(HWND hFatherWnd);
static BOOL     CallStartUpEffectSetting(HWND hwndCall);
static BOOL CallIndLEDSetting(HWND hFrame);

static void     SetStartupEffect(STARTUP_EFFECT Seffect);
static void     SetGreetingTxt(char * szGreetingTxt);

void SetScrnSaverTime(int iTime);
void SetBrightness(LEVELVALUE iLevel);
BOOL SetBKPicture(char* szPicFile);
BOOL SetStartupPic(char* szPicFile);

extern  BOOL SetIdleShowInfo(IDLESHOW idleShow);
extern BOOL CallLevelAdjuster(HWND hFrame, HWND hcallWnd, ADJTYPE adjType, int nlevel,UINT wCallbackMsg,const char*szTitle);
extern  void IntToString( int nValue, char *buf );
extern BOOL Sett_BritAdjusting(LEVELVALUE level);
extern BOOL CallColorThemeSetting(HWND hWnd);
static void OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus);
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);
void SetDisplayBackLitMode(LIGHTMODE litMode);
void SetKeypadLitMode(LIGHTMODE litMode);
void CallIdleBkPicChange(void);

BOOL CallDisplaySettingWindow(HWND hwndCall)
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
    
    hWndApp = CreateWindow(pClassName,NULL, 
        WS_VISIBLE | WS_CHILD | WS_VSCROLL, 
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
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back") );
    SetWindowText(hFrameWin, ML("Display"));

    SetFocus(hWndApp);

    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hBright, hIndicatorLed, hColorTheme, hStartup, hWallPaper, hLights, hInfoline;
    static HWND hFocus = 0;
    static int  iButtonJust,iCurIndex,nPicNameLen;
    LRESULT     lResult;
    static HWND hWndFocus;
    static char buf[1];
    STARTUP_EFFECT start_effect;
    static char Greet_txt[MAXSTARTUPTXTLEN];
    static char Greet_pic[PREBROW_MAXFULLNAMELEN];
    static char Backgrd_pic[PREBROW_MAXFULLNAMELEN];
	int IndLed;
    char *Tmp;
    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:
        CreateControl(hWnd,&hBright,&hIndicatorLed,&hColorTheme,&hStartup,&hWallPaper,&hLights,&hInfoline);
        memset(Greet_txt, 0, MAXSTARTUPTXTLEN);
        memset(Greet_pic, 0, PREBROW_MAXFULLNAMELEN);
        memset(Backgrd_pic, 0, PREBROW_MAXFULLNAMELEN);

        iCurBrit = GetBrightness();
        GetBKPicture(Backgrd_pic);
        

        if ( 0 == strcmp(Backgrd_pic, "Default") )
            SendMessage(hWallPaper, SSBM_ADDSTRING, 0, (LPARAM)ML("Default"));
        else
        {
            Tmp = strrchr(Backgrd_pic, '/');
            if (Tmp != 0)
            {
                Tmp++;
                SendMessage(hWallPaper,SSBM_ADDSTRING,0,(LPARAM)Tmp);
            }
        }


		IndLed = GetIndicatorLEDStat();
		if (IndLed == SWITCH_ON)
		{
			SendMessage(hIndicatorLed, SSBM_ADDSTRING, 0,(LPARAM)ML("On"));
		}		
		else
		{
			SendMessage(hIndicatorLed, SSBM_ADDSTRING, 0, (LPARAM)ML("Off"));
		}

        hFocus = hBright;
        SendMessage(hBright, SSBM_SETCURSEL, (WPARAM)iCurBrit, 0);
        DisplaySet_InitVScrolls(hWnd,6);        
        start_effect = GetStartupEffect();

        switch(start_effect)
        {
        case SE_DEF:
            SendMessage(hStartup, SSBM_ADDSTRING, 0, (LPARAM)ML("Default"));
            break;
        case SE_TXT:
            GetGreetingTxt(Greet_txt);
            SendMessage(hStartup, SSBM_ADDSTRING, 0, (LPARAM)Greet_txt);
            break;
        case SE_PIC:
            GetStartupPic(Greet_pic);
			if (strcmp(Greet_pic, "Default") == 0)
			{
                SendMessage(hStartup, SSBM_DELETESTRING, 0, 0);
			    SendMessage(hStartup,SSBM_ADDSTRING,0,(LPARAM)ML("Default"));
				break;
			}
            Tmp = strrchr(Greet_pic, '/');
            if (Tmp != 0)
            {
                Tmp++;
			    SendMessage(hStartup,SSBM_ADDSTRING,0,(LPARAM)Tmp);
            }
            break;
        }
//        SendMessage(hBright,SSBM_SETCURSEL, GetBrightness(), 0);
        SendMessage(hInfoline, SSBM_SETCURSEL, GetIdleShowInfo(), 0);
//        SendMessage(hScrnSaver,SSBM_SETCURSEL,GetScrnSaverTime(),0);
    	break;

    case PWM_SHOWWINDOW:
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back") );
        SetWindowText(hFrameWin, ML("Display"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif

        SendMessage(hInfoline, SSBM_SETCURSEL, GetIdleShowInfo(), 0);
//        SendMessage(hBright,SSBM_SETCURSEL, GetBrightness(), 0);
//        SendMessage(hScrnSaver,SSBM_SETCURSEL,GetScrnSaverTime(),0);
        start_effect = GetStartupEffect();
        switch(start_effect)
        {
        case SE_DEF:
            SendMessage(hStartup, SSBM_DELETESTRING, 0, 0);
            SendMessage(hStartup, SSBM_ADDSTRING, 0, (LPARAM)ML("Default"));
            break;
        case SE_TXT:
            GetGreetingTxt(Greet_txt);
            SendMessage(hStartup, SSBM_DELETESTRING, 0, 0);
            SendMessage(hStartup, SSBM_ADDSTRING, 0, (LPARAM)Greet_txt);
            break;
        case SE_PIC:
            GetStartupPic(Greet_pic);
			if (strcmp(Greet_pic, "Default") == 0)
			{
                SendMessage(hStartup, SSBM_DELETESTRING, 0, 0);
			    SendMessage(hStartup,SSBM_ADDSTRING,0,(LPARAM)ML("Default"));
				break;
			}
            Tmp = strrchr(Greet_pic, '/');
            if (Tmp != 0)
            {
                Tmp++;
                SendMessage(hStartup, SSBM_DELETESTRING, 0, 0);
			    SendMessage(hStartup,SSBM_ADDSTRING,0,(LPARAM)Tmp);
            }
            break;
        }
		
		IndLed = GetIndicatorLEDStat();
		if (IndLed == SWITCH_ON)
		{
			SendMessage(hIndicatorLed, SSBM_DELETESTRING, 0, 0);
			SendMessage(hIndicatorLed, SSBM_ADDSTRING, 0, (LPARAM)ML("On"));
		}		
		else
		{
			SendMessage(hIndicatorLed, SSBM_DELETESTRING, 0, 0);
			SendMessage(hIndicatorLed, SSBM_ADDSTRING, 0, (LPARAM)ML("Off"));
		}
        

		memset(Backgrd_pic, 0, sizeof(Backgrd_pic));
        GetBKPicture(Backgrd_pic);
        if ( 0 == strcmp(Backgrd_pic, "Default") )
        {
            SendMessage(hWallPaper, SSBM_DELETESTRING, 0, 0);
            SendMessage(hWallPaper, SSBM_ADDSTRING, 0, (LPARAM)ML("Default"));
        }
        else
        {
            Tmp = strrchr(Backgrd_pic, '/');
            if (Tmp != 0)
            {
                Tmp++;
                SendMessage(hWallPaper, SSBM_DELETESTRING, 0, 0);
                SendMessage(hWallPaper, SSBM_ADDSTRING, 0, (LPARAM)Tmp);
            }
        }

        SetFocus(hWnd);
        break;

    case WM_VSCROLL:
		DisplaySet_OnVScroll(hWnd,wParam);
		break;

	case WM_KILLFOCUS:
		KillTimer(hWnd, TIMER_ID);
		RepeatFlag = 0;
		wKeyCode   = 0;
		break;
	case PWM_ACTIVATE:
		if (wParam == WA_INACTIVE)
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
			wKeyCode   = 0;
		}
		break;
    
	case WM_SETFOCUS:
        SetFocus(hFocus);
    	break;

    case WM_DESTROY:
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
			
		    SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);

			if ( 1 == RepeatFlag )
			{
				SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
			}		   
			break;
            
        case VK_UP:
            SettListProcessKeyUp(hWnd, &hFocus);            
			
			SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
			
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
        OnTimeProcess(hWnd, wParam, hFocus);
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
        switch(LOWORD( wParam )) 
        {
        case IDC_BUTTON_SET:
            switch(iButtonJust)
            {
            case IDC_BRIGHTNESS:
                CallLevelAdjuster(hFrameWin ,hWnd,LAS_LIT,iCurBrit,CALLBACK_SETLIT,ML("Brightness"));                
                break;
            case IDC_INDICATOR_LED:
                CallIndLEDSetting(hFrameWin);
                break;
            case IDC_COLORTHEME:
                CallColorThemeSetting(hFrameWin);
                break;
            case IDC_STARTUP:
                CallStartUpEffectSetting(hFrameWin);
                break;
			case IDC_WALLPAPER:
				APP_PreviewPictureEx(hFrameWin,hWnd, CALLBACK_SETBKPIC, ML("Wallpaper"), ML("Default"), Backgrd_pic);
				break;
            case IDC_LIGHTS:
                CallBackLitSetting(hFrameWin);
                break;
            case IDC_INFOLINE:
                CallInfoLineSetting(hFrameWin);
                break;                
            }
        	break;
        case IDC_BRIGHTNESS:
           	if(HIWORD( wParam ) == SSBN_CHANGE)
	       	{
	        iCurBrit = SendMessage(hBright,SSBM_GETCURSEL,0,0);
			SetBrightness(iCurBrit);
		    }
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_BRIGHTNESS;
            break;
        case IDC_INDICATOR_LED:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_INDICATOR_LED;
            break;
        case IDC_STARTUP:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_STARTUP;
            break;
        case IDC_WALLPAPER:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_WALLPAPER;
            break;
        case IDC_LIGHTS:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_LIGHTS;
            break;
        case IDC_INFOLINE:

           	if(HIWORD( wParam ) == SSBN_CHANGE)
	       	{
	        iCurIndex = SendMessage(hInfoline,SSBM_GETCURSEL,0,0);
			SetIdleShowInfo(iCurIndex);
		    }

            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_INFOLINE;
            break;
        }
        break;

    case CALLBACK_SETLIT:
        if(1 == wParam) //save and return
        {
            iCurBrit = lParam;
            SendMessage(hBright,SSBM_SETCURSEL,(WPARAM)iCurBrit,0);
            SetBrightness(iCurBrit);
        }
        else //cancel and return
        {
            iCurBrit = SendMessage(hBright, SSBM_GETCURSEL, 0, 0);
            SetBrightness(iCurBrit);            
        }
        break;

    case CALLBACK_SETBKPIC:
        nPicNameLen = LOWORD(wParam); // get the name length of picture
        if (nPicNameLen > PREBROW_MAXFULLNAMELEN)
        {
            return FALSE;
        }

		if (0 == nPicNameLen)
		{
			memset(Backgrd_pic, 0, sizeof(Backgrd_pic));
			strcpy(Backgrd_pic, "Default");
		}
		else
		{
			memset(Backgrd_pic, 0, sizeof(Backgrd_pic));
			strncpy(Backgrd_pic, (char*)lParam, nPicNameLen);
		}
		SetBKPicture(Backgrd_pic);
		CallIdleBkPicChange();
		break;

             
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static  BOOL    CreateControl(HWND hwnd,HWND * hBright,HWND * hIndicatorLed,HWND * hColorTheme,HWND * hStartup,HWND * hWallPaper,HWND * hLights, HWND * hInfoline)
{
    int     xzero=0,yzero=0;
    int iControlH,iControlW;
    RECT rect;
    GetClientRect(hwnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;

    * hBright = CreateWindow( "LEVIND", ML("Brightness"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST/* | WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hwnd, (HMENU)IDC_BRIGHTNESS, NULL, (PVOID)LAS_LIT);

    if (* hBright == NULL)
        return FALSE;
    * hWallPaper = CreateWindow( "SPINBOXEX", ML("Wallpaper"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH, iControlW, iControlH, 
        hwnd, (HMENU)IDC_WALLPAPER, NULL, NULL);

    if (* hWallPaper == NULL)
        return FALSE;

    * hStartup = CreateWindow( "SPINBOXEX", ML("Startup effects"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*2, iControlW, iControlH, 
        hwnd, (HMENU)IDC_STARTUP, NULL, NULL);

    if (* hStartup == NULL)
        return FALSE;

	* hIndicatorLed = CreateWindow( "SPINBOXEX", ML("Indicator led"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST/* | WS_BORDER */| WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*3, iControlW, iControlH, 
        hwnd, (HMENU)IDC_INDICATOR_LED, NULL, NULL);

    if (* hIndicatorLed == NULL)
        return FALSE;

    * hLights = CreateWindow( "SPINBOXEX", ML("Lights"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*4, iControlW, iControlH, 
        hwnd, (HMENU)IDC_LIGHTS, NULL, NULL);

    if (* hLights == NULL)
        return FALSE;
    
    * hInfoline = CreateWindow( "SPINBOXEX", ML("Info line"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER |*/ WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*5, iControlW, iControlH, 
        hwnd, (HMENU)IDC_INFOLINE, NULL, NULL);

    if (* hInfoline == NULL)
        return FALSE;

    SendMessage(* hInfoline,SSBM_ADDSTRING,0,(LPARAM)ML("Cell ID"));
    SendMessage(* hInfoline,SSBM_ADDSTRING,0,(LPARAM)ML("Date"));
    SendMessage(* hInfoline,SSBM_ADDSTRING,0,(LPARAM)ML("Not used"));


return TRUE;
}
/*********************************************************************\
* Function        DisplaySet_InitVScrolls
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
static void DisplaySet_InitVScrolls(HWND hWnd,int iItemNum)
{
    static SCROLLINFO   vsi;

    memset(&vsi, 0, sizeof(SCROLLINFO));

    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 3;
    vsi.nMax   = (iItemNum-1);
    vsi.nPos   = 0;

    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
	nCurFocus =0;
	return;
}
/*********************************************************************\
* Function        MobileSet_OnVScroll
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/

static void DisplaySet_OnVScroll(HWND hWnd,  UINT wParam)
{
    static int  nY;
    static RECT rcClient;
    static SCROLLINFO      vsi;

    switch(wParam)
    {
    case SB_LINEDOWN:
		memset(&vsi, 0, sizeof(SCROLLINFO));
		
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus++;
		
        if(nCurFocus > vsi.nMax)	
        {
            ScrollWindow(hWnd,0,(vsi.nMax-vsi.nPage+1)*nY,NULL,NULL);	
            UpdateWindow(hWnd);
            nCurFocus = 0;
			vsi.nPos = 0;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
	
        if(((int)(vsi.nPos + vsi.nPage - 1) <= nCurFocus) && nCurFocus != vsi.nMax)	//modified for UISG
        { 
            ScrollWindow(hWnd,0,-nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos++;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
		break;
		
    case SB_LINEUP:
		
        memset(&vsi, 0, sizeof(SCROLLINFO));
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
		
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus--;
		
        if(nCurFocus < vsi.nMin)	
        {
            ScrollWindow(hWnd,0,(vsi.nPage-vsi.nMax-1)*nY,NULL,NULL);
			
            UpdateWindow(hWnd);
			nCurFocus = vsi.nMax;
		    vsi.nPos = vsi.nMax-vsi.nPage+1;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;	
        }
		
        if((int)vsi.nPos == nCurFocus && nCurFocus != vsi.nMin)	//modified for UISG
        { 
            ScrollWindow(hWnd,0,nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos--;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);   
            break;	
        }
		break;
    case SB_PAGEDOWN:
        break;
    case SB_PAGEUP:
        break;
    default:
        break;	
    }	
}


static BOOL CallScreenSaverSetting(HWND hwndCall)
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hwndCall;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = SSAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pSSClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);

    hSSWndApp = CreateWindow(pSSClassName,NULL, 
        WS_VISIBLE | WS_CHILD,
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, NULL, NULL, NULL);

    if (NULL == hSSWndApp)
    {
        UnregisterClass(pSSClassName,NULL);
        return FALSE;
    }
       
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel"));           
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SetWindowText(hFrameWin, ML("Screen saver"));
    SetFocus(hSSWndApp);

    return (TRUE);
}
static LRESULT  SSAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hSaverTime;
    LRESULT     lResult;
	static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;
    static      iIndex;
    HDC         hdc;
    int         iCurSel;
 	RECT rect;
    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:        
		GetClientRect(hWnd, &rect);
		hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);
        
        hSaverTime = CreateWindow("LISTBOX", 0, 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
        hWnd, (HMENU)IDC_SAVERLIST, NULL, NULL);
        
        if (hSaverTime == NULL )
            lResult = FALSE;

        SendMessage(hSaverTime,LB_ADDSTRING,0,(LPARAM)ML("Off"));
        SendMessage(hSaverTime,LB_ADDSTRING,0,(LPARAM)ML("1 min"));
        SendMessage(hSaverTime,LB_ADDSTRING,0,(LPARAM)ML("2 min"));
        SendMessage(hSaverTime,LB_ADDSTRING,0,(LPARAM)ML("5 min"));
        SendMessage(hSaverTime,LB_ADDSTRING,0,(LPARAM)ML("10 min"));
        SendMessage(hSaverTime,LB_ADDSTRING,0,(LPARAM)ML("15 min"));
        SendMessage(hSaverTime,LB_ADDSTRING,0,(LPARAM)ML("30 min"));
    
        iCurSel = GetScrnSaverTime();
        Load_Icon_SetupList(hSaverTime,hIconNormal,hIconSel,7,iCurSel);
        
        SendMessage(hSaverTime,LB_SETCURSEL,iCurSel,0);
   	    break;

    case PWM_SHOWWINDOW:
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel"));           
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SetWindowText(hFrameWin, ML("Screen saver"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        SetFocus(hWnd);        
        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_SAVERLIST));
    	break;

    case WM_DESTROY:
        hSSWndApp = NULL;
        DeleteObject(hIconNormal);
        DeleteObject(hIconSel);
        KillTimer(hWnd, TIMER_ASURE);
        UnregisterClass(pSSClassName,NULL);
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
            iIndex = SendMessage(hSaverTime,LB_GETCURSEL,0,0);
            Load_Icon_SetupList(hSaverTime,hIconNormal,hIconSel,7,iIndex);
            SetScrnSaverTime(iIndex);
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
		}
		break;
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
/***************************************************************
* Function     CallStartUpEffectSetting
* Purpose      call the setting window for start effect
* Params
* Return
* Remarks
***************************************************************/
static BOOL CallStartUpEffectSetting(HWND hwndCall)
{
    WNDCLASS    wc;

    RECT rClient;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = SEAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pSEClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);

    hSEWndApp = CreateWindow(pSEClassName, NULL, 
        WS_VISIBLE | WS_CHILD, 
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, NULL, NULL, NULL);

    if (NULL == hSEWndApp)
    {
        UnregisterClass(pSEClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;

    SetWindowText(hFrameWin, ML("Startup effects"));
    SetFocus(hSEWndApp);

    return (TRUE);
}
/***************************************************************
* Function     SEAppWndProc
* Purpose      window processing of hESWndApp
* Params
* Return
* Remarks
***************************************************************/

static LRESULT  SEAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hSelectionList;
    LRESULT     lResult;
	static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;
    HDC hdc;
    static char bufStartPic[PREBROW_MAXFULLNAMELEN+1];
    static int iIndex,iscreenw,iscreenh;
    STARTUP_EFFECT iInitIndex;
	RECT rect;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:
        memset(bufStartPic, 0, PREBROW_MAXFULLNAMELEN+1);
		GetStartupPic(bufStartPic);
		GetClientRect(hWnd, &rect);
/**************************** Draw radio button ***********************************/
        hdc         = GetDC(hWnd);

        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);

		ReleaseDC(hWnd,hdc);
/***************************** Draw radio button ***********************************/
        hSelectionList = CreateWindow("LISTBOX", 0, 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY/*|WS_VSCROLL*/,   
        rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
        hWnd, (HMENU)IDC_SELLIST, NULL, NULL);
        
        if (hSelectionList == NULL )
            lResult = FALSE;

        SendMessage(hSelectionList,LB_ADDSTRING,0,(LPARAM)ML("Default"));
        SendMessage(hSelectionList,LB_ADDSTRING,0,(LPARAM)ML("Greeting text"));
        SendMessage(hSelectionList,LB_ADDSTRING,0,(LPARAM)ML("Picture"));
        iInitIndex = GetStartupEffect();
        Load_Icon_SetupList(hSelectionList,hIconNormal,hIconSel,3,iInitIndex);

        SendMessage(hSelectionList,LB_SETCURSEL,iInitIndex,0);       
    	break;

    case PWM_SHOWWINDOW:
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;        
        SetWindowText(hFrameWin, ML("Startup effects"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        iInitIndex = GetStartupEffect();
        Load_Icon_SetupList(hSelectionList,hIconNormal,hIconSel,3,iInitIndex);
        SetFocus(hWnd);
        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_SELLIST));
    	break;
        
    case WM_DESTROY:
	     DeleteObject(hIconNormal);
	     DeleteObject(hIconSel);
         hSEWndApp = NULL;
         UnregisterClass(pSEClassName,NULL);
        break;

    case WM_KEYDOWN:
		switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
		default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
        }
        break;
        
    case WM_COMMAND:
        switch(LOWORD( wParam )) 
        {
        case IDC_BUTTON_SET:
            iIndex = SendMessage(hSelectionList,LB_GETCURSEL,0,0);
            Load_Icon_SetupList(hSelectionList,hIconNormal,hIconSel,3,iIndex);
            SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
        	break;
        }
        break;

	case WM_TIMER:
	    switch(wParam)
		{
		case TIMER_ASURE:
            switch(iIndex)
            {
            case 0:    // select default for startup
		        KillTimer(hWnd, TIMER_ASURE);
                SetStartupEffect(SE_DEF);
                SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                PostMessage(hWnd, WM_CLOSE, 0, 0);
                break;
            case 1:    // select to show text when startup
		        KillTimer(hWnd, TIMER_ASURE);
                CallWriteStartupText(hFrameWin, hWnd);
                break;
            case 2:     // select to show picture when startup
       		    KillTimer(hWnd, TIMER_ASURE);
                APP_PreviewPictureEx(hFrameWin,hWnd, CALLBACK_SET_START_PIC, ML("Startup pictures"), ML("Default"), bufStartPic);
                break;                
            }
			break;
		}
		break;
    case CALLBACK_SET_START_PIC:
		if ( 0 == LOWORD(wParam) )
		{
			memset(bufStartPic, 0, sizeof(bufStartPic));
			strcpy(bufStartPic, "Default");
			SetStartupPic(bufStartPic);
			SetStartupEffect(SE_PIC);
	        SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
		    PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		memset(bufStartPic, 0, sizeof(bufStartPic));
        strncpy(bufStartPic, (char*)lParam, LOWORD(wParam));
        SetStartupPic(bufStartPic);
        SetStartupEffect(SE_PIC);
        SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        break;
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
   
}
/***************************************************************
* Function     CallWriteStartupText
* Purpose      to edit the text show when startup
* Params
* Return
* Remarks
***************************************************************/
static BOOL CallWriteStartupText(HWND hwndCall, HWND hFatherWnd)
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hwndCall;
    hFather = hFatherWnd;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = WSTAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pWSTClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);

    hWSTWndApp = CreateWindow(pWSTClassName, NULL, 
                WS_VISIBLE | WS_CHILD,                    
                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin,
                NULL, NULL, NULL);

    if (NULL == hWSTWndApp)
    {
        UnregisterClass(pWSTClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,MAKEWPARAM((WORD)1, (WORD)IDC_BUTTON_SAVE), (LPARAM)ML("Save"));//LSK
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));//RSK  
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");           

    SetWindowText(hFrameWin, ML("Startup effects"));
    SetFocus(hWSTWndApp);
    
    return (TRUE);

}    
static LRESULT WSTAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hStartupText,hFocus;
    LRESULT     lResult;
    IMEEDIT ie;
    RECT rect;
    char szDefTxt[MAXSTARTUPTXTLEN+1];
    static char szStartupTxt[MAXSTARTUPTXTLEN + 1];

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:
        GetClientRect(hWnd, &rect);
        
	    memset((void *)&ie, 0, sizeof(IMEEDIT));

        ie.hwndNotify   = hWnd;

        GetGreetingTxt(szDefTxt);

        hStartupText = CreateWindow(
		            "IMEEDIT",
		            szDefTxt,
                    WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL | ES_TITLE,
                    rect.left,
                    rect.top,
                    rect.right,
                    rect.bottom / 3,
		            hWnd,
		            (HMENU)ID_EDITTXT,
		            NULL,
		            &ie);
        SendMessage(hStartupText, EM_LIMITTEXT, MAXSTARTUPTXTLEN, 0);

        SendMessage(hStartupText, EM_SETTITLE, 0, (LPARAM)ML("Greeting textcolon"));
		SendMessage(hStartupText, EM_SETSEL, -1, -1);
        hFocus  = hStartupText;
    	break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
    	break;

    case PWM_SHOWWINDOW:
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,MAKEWPARAM((WORD)1, (WORD)IDC_BUTTON_SAVE), (LPARAM)ML("Save"));//LSK
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));//RSK  
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");         
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        SetWindowText(hFrameWin, ML("Startup effects"));
        SetFocus(hWnd);    
        break;

    case WM_DESTROY:
        hWSTWndApp = NULL;
        UnregisterClass(pWSTClassName,NULL);
        break;

    case WM_KEYDOWN:
		switch(LOWORD(wParam))
        {
        case VK_F10:
            PostMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hFather,0); 
            PostMessage(hFather, WM_CLOSE, 0, 0);            
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
        switch(LOWORD( wParam )) 
        {
        case IDC_BUTTON_SAVE:
            GetWindowText(hStartupText,szStartupTxt,sizeof (szStartupTxt));//save the text
			Sett_SpecialStrProc(szStartupTxt);

			if(!strlen(szStartupTxt))
			{
				PLXTipsWin(NULL, NULL, NULL,ML("Please define text"),NULL,Notify_Alert,ML("Ok"),NULL,WAITTIMEOUT);
				break;
			}

            SetStartupEffect(SE_TXT);
            SetGreetingTxt(szStartupTxt);
            PostMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hFather,0); 
            PostMessage(hFather, WM_CLOSE, 0, 0);            
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
    
static BOOL CallBackLitSetting(HWND hwndCall)
{
    WNDCLASS    wc;
    RECT rClient;
    hFrameWin = hwndCall;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = BLAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pBLClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    GetClientRect(hFrameWin, &rClient);

    hBLWndApp = CreateWindow(pBLClassName,NULL, 
                WS_VISIBLE | WS_CHILD,                    
                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin,
                NULL, NULL, NULL);

    if (NULL == hBLWndApp)
    {
        UnregisterClass(pBLClassName,NULL);
        return FALSE;
    }
        
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK,(LPARAM)ML("Back"));         
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
                
    SetWindowText(hFrameWin, ML("Lights"));
    SetFocus(hBLWndApp); 

    return (TRUE);
}

static LRESULT  BLAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hDisplayBKLit, hKeypadBKLit,hFocus;
    static int  iButtonJust, iCurIndex;
    LRESULT     lResult;
	int     xzero=0,yzero=0,iControlH,iControlW;
	RECT    rect;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:        
        GetClientRect(hWnd, &rect);
	    iControlH = rect.bottom/3;
        iControlW = rect.right;
        
        hDisplayBKLit = CreateWindow( "SPINBOXEX", ML("Display backlight"), 
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
            xzero, yzero, iControlW, iControlH, 
            hWnd, (HMENU)IDC_DISBKLIT, NULL, NULL);

        if (hDisplayBKLit == NULL)
        {
            lResult = FALSE;
            break;
        }
        SendMessage(hDisplayBKLit,SSBM_ADDSTRING,0,(LPARAM)ML("Automatic"));
        SendMessage(hDisplayBKLit,SSBM_ADDSTRING,0,(LPARAM)ML("Always on"));

        hKeypadBKLit = CreateWindow( "SPINBOXEX", ML("Keypad backlight"), 
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
            xzero, yzero+iControlH, iControlW, iControlH, 
            hWnd, (HMENU)IDC_KEYPADBKLIT, NULL, NULL);

        if (hKeypadBKLit == NULL)
        {
            lResult = FALSE;
            break;
        }
        SendMessage(hKeypadBKLit,SSBM_ADDSTRING,0,(LPARAM)ML("Automatic"));
        SendMessage(hKeypadBKLit,SSBM_ADDSTRING,0,(LPARAM)ML("Always on"));
        SendMessage(hKeypadBKLit,SSBM_ADDSTRING,0,(LPARAM)ML("Always off"));
        
        SendMessage(hDisplayBKLit, SSBM_SETCURSEL, GetDisplayBackLitMode(), 0);        
        SendMessage(hKeypadBKLit, SSBM_SETCURSEL, GetKeypadLitMode(), 0);

        hFocus = hDisplayBKLit;
    	break;
    
    case WM_SETFOCUS:
    	SetFocus(hFocus);
        break;
    
    case PWM_SHOWWINDOW:
        SendMessage(hDisplayBKLit, SSBM_SETCURSEL, GetDisplayBackLitMode(), 0);        
        SendMessage(hKeypadBKLit, SSBM_SETCURSEL, GetKeypadLitMode(), 0);
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK,(LPARAM)ML("Back"));         
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;        
        SetWindowText(hFrameWin, ML("Lights"));        
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        SetFocus(hWnd);
        break;
    
    case WM_DESTROY:
        hBLWndApp = NULL;
        UnregisterClass(pBLClassName,NULL);
        break;
	case PWM_ACTIVATE:
		if (wParam == WA_INACTIVE)
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
			wKeyCode   = 0;
		}
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
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SEL,0);
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

	case WM_KILLFOCUS:
		KillTimer(hWnd, TIMER_ID);
		RepeatFlag = 0;
		wKeyCode   = 0;
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
        switch(LOWORD( wParam )) 
        {
        case IDC_BUTTON_SEL:
            switch(iButtonJust)
            {
            case IDC_DISBKLIT:
                CallDisBackLitSetting(hFrameWin);
                break;
            case IDC_KEYPADBKLIT:
                CallDisKeypadLitSetting(hFrameWin);
                break;
            }
        	break;
        case IDC_DISBKLIT:
           	if(HIWORD( wParam ) == SSBN_CHANGE)
	       	{
	        iCurIndex = SendMessage(hDisplayBKLit,SSBM_GETCURSEL,0,0);
            SetDisplayBackLitMode(iCurIndex);
            }
            if(HIWORD(wParam) == SSBN_SETFOCUS)
            {
                iButtonJust = IDC_DISBKLIT;
                hFocus = hDisplayBKLit;
            }
            break;
        case IDC_KEYPADBKLIT:
           	if(HIWORD( wParam ) == SSBN_CHANGE)
	       	{
	        iCurIndex = SendMessage(hKeypadBKLit,SSBM_GETCURSEL,0,0);
            SetKeypadLitMode(iCurIndex);
            }
            if(HIWORD(wParam) == SSBN_SETFOCUS)
            {
                iButtonJust = IDC_KEYPADBKLIT;
                hFocus = hKeypadBKLit;
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
static BOOL CallDisBackLitSetting(HWND hFatherWnd)
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hFatherWnd;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = DIS_BK_LIT_AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pDIS_BK_LIT_ClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    
    GetClientRect(hFrameWin, &rClient);

    hDIS_BK_LIT_WndApp = CreateWindow(pDIS_BK_LIT_ClassName,NULL, 
                WS_VISIBLE | WS_CHILD,  
                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin, 
                NULL, NULL, NULL);

    if (NULL == hDIS_BK_LIT_WndApp)
    {
        UnregisterClass(pDIS_BK_LIT_ClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_CANCEL,(LPARAM)ML("Back") );
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;

    
    SetWindowText(hFrameWin,ML("Display backlight"));
    SetFocus(hDIS_BK_LIT_WndApp);
    
    return (TRUE);

}

static LRESULT  DIS_BK_LIT_AppWndProc   ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
    static  HWND    hList;
    static  int     iIndex;
    LRESULT lResult;
    RECT rect;
    lResult = TRUE;   

    switch ( wMsgCmd )
	{
		case WM_CREATE :
            GetClientRect(hWnd, &rect);
            hList = CreateWindow("LISTBOX", 0, 
                WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL/*|CS_NOSYSCTRL*/,   
                0,0,rect.right - rect.left, rect.bottom - rect.top,
                hWnd, (HMENU)IDC_LIST, NULL, NULL);

            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ML("Automatic"));
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ML("Always on"));
            
            
            hdc         = GetDC(hWnd);
            hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            ReleaseDC(hWnd,hdc);
            iIndex = GetDisplayBackLitMode();
            
            Load_Icon_SetupList(hList,hIconNormal,hIconSel,2,iIndex);
            
            SendMessage(hList,LB_SETCURSEL,iIndex,0);            
            
		    break;

		case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_LIST));
            break;

        case PWM_SHOWWINDOW:
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_CANCEL,(LPARAM)ML("Back") );
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;     
            
            SetWindowText(hFrameWin,ML("Display backlight"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            SetFocus(hWnd);
            break;

		case WM_KEYDOWN:
            switch(LOWORD(wParam))
            {       
		        case VK_F10:
                    SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;
                case VK_F5:
                    SendMessage(hWnd,WM_COMMAND,IDC_OK,0);
                    break;
                default:
                    lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            }
            break;  
         
		case WM_DESTROY : 
	        DeleteObject(hIconNormal);
	        DeleteObject(hIconSel);
		    KillTimer(hWnd, TIMER_ASURE);
		    hDIS_BK_LIT_WndApp = NULL;
            UnregisterClass(pDIS_BK_LIT_ClassName,NULL);
            break;

	    case WM_COMMAND :    	    
            switch (LOWORD(wParam))
		    {
                case IDC_CANCEL:
                    DestroyWindow(hWnd);
                    break;      
                case IDC_OK:
                    iIndex = SendMessage(hList,LB_GETCURSEL,0,0);              //get the current selected item number
                    SetDisplayBackLitMode(iIndex);
		    	    Load_Icon_SetupList(hList,hIconNormal,hIconSel,2,iIndex);     //change the radio button of the current selected item		
                    SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
                    break;
                default:
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


static BOOL CallDisKeypadLitSetting(HWND hFatherWnd)
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hFatherWnd;


    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = DIS_KP_LIT_AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pDIS_KP_LIT_ClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);
    
    hDIS_KP_LIT_WndApp = CreateWindow(pDIS_KP_LIT_ClassName,NULL, 
                WS_VISIBLE | WS_CHILD,  
                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin, 
                NULL, NULL, NULL);

    if (NULL == hDIS_KP_LIT_WndApp)
    {
        UnregisterClass(pDIS_KP_LIT_ClassName,NULL);
        return FALSE;
    }

    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_CANCEL,(LPARAM)ML("Back") );
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SetWindowText(hFrameWin,ML("Keypad backlight"));

    SetFocus(hDIS_KP_LIT_WndApp);

    return (TRUE);

}

static LRESULT  DIS_KP_LIT_AppWndProc   ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
    static  HWND    hList;
    static  int     iIndex;
    LRESULT lResult;
    RECT rect;
    lResult = TRUE;   

    switch ( wMsgCmd )
	{
		case WM_CREATE :
            GetClientRect(hWnd, &rect);
            hList = CreateWindow("LISTBOX", 0, 
                WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL/*|CS_NOSYSCTRL*/,   
                0,0,rect.right - rect.left, rect.bottom - rect.top,
                hWnd, (HMENU)IDC_KPLIST, NULL, NULL);

            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ML("Automatic"));
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ML("Always on"));
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ML("Always off"));
                        
            hdc         = GetDC(hWnd);
            hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            ReleaseDC(hWnd,hdc);
            iIndex = GetKeypadLitMode();
            
            Load_Icon_SetupList(hList,hIconNormal,hIconSel,3,iIndex);
            
            SendMessage(hList,LB_SETCURSEL,iIndex,0);           
            
		    break;

		case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_KPLIST));
            break;

        case PWM_SHOWWINDOW:
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_CANCEL,(LPARAM)ML("Back") );
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
            SetWindowText(hFrameWin,ML("Keypad backlight"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            SetFocus(hWnd);    
            break;

		case WM_KEYDOWN:
            switch(LOWORD(wParam))
            {       
		        case VK_F10:
                    SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;
                case VK_F5:
                    SendMessage(hWnd,WM_COMMAND,IDC_OK,0);
                    break;
                default:
                    lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            }
            break;  
         
		case WM_DESTROY : 
	        DeleteObject(hIconNormal);
	        DeleteObject(hIconSel);
		    KillTimer(hWnd, TIMER_ASURE);
		    hDIS_KP_LIT_WndApp = NULL;
            UnregisterClass(pDIS_KP_LIT_ClassName,NULL);
            break;

	    case WM_COMMAND : 	    
           switch (LOWORD(wParam))
		    {
                case IDC_CANCEL:
                    DestroyWindow(hWnd);
                    break;      
                case IDC_OK:
                    iIndex = SendMessage(hList,LB_GETCURSEL,0,0);              //get the current selected item number
                    SetKeypadLitMode(iIndex);
		    	    Load_Icon_SetupList(hList,hIconNormal,hIconSel,3,iIndex);     //change the radio button of the current selected item		
                    SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
                    break;
                default:
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
static BOOL CallIndLEDSetting(HWND hFrame)
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hFrame;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = IndLED_WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pIndLED_ClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);
    
    hIndLEDApp = CreateWindow(pIndLED_ClassName,NULL, 
                WS_VISIBLE | WS_CHILD,  
                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin, 
                NULL, NULL, NULL);

    if (NULL == hIndLEDApp)
    {
        UnregisterClass(pIndLED_ClassName,NULL);
        return FALSE;
    }

    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_CANCEL,(LPARAM)ML("Back") );
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SetWindowText(hFrameWin,ML("Indicator led"));

    SetFocus(hIndLEDApp);

    return (TRUE);
}


static LRESULT  IndLED_WndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
    static  HWND    hList;
    static  int     iIndex;
    LRESULT lResult;
	RECT rect;
    lResult = TRUE;   
    switch ( wMsgCmd )
	{
		case WM_CREATE :
            GetClientRect(hWnd, &rect);
            hList = CreateWindow("LISTBOX", 0, 
                WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL/*|CS_NOSYSCTRL*/,   
                0,0,rect.right - rect.left, rect.bottom - rect.top,
                hWnd, (HMENU)IDC_INDLED_LIST, NULL, NULL);

            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ML("On"));
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ML("Off"));
            
            
            hdc         = GetDC(hWnd);
            hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            ReleaseDC(hWnd,hdc);
            iIndex = GetIndicatorLEDStat();
            
            Load_Icon_SetupList(hList,hIconNormal,hIconSel,2,iIndex);
            
            SendMessage(hList,LB_SETCURSEL,iIndex,0);           
            
		    break;

		case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_INDLED_LIST));
            break;

        case PWM_SHOWWINDOW:
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_CANCEL,(LPARAM)ML("Back") );
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
            SetWindowText(hFrameWin,ML("Indicator led"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            SetFocus(hWnd);    
            break;

		case WM_KEYDOWN:
            switch(LOWORD(wParam))
            {       
		        case VK_F10:
                    SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;
                case VK_F5:
                    SendMessage(hWnd,WM_COMMAND,IDC_OK,0);
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
		    hIndLEDApp = NULL;
            UnregisterClass(pIndLED_ClassName,NULL);
            break;

	    case WM_COMMAND :    	    
           
            switch (LOWORD(wParam))
		    {
                case IDC_CANCEL:
                    DestroyWindow(hWnd);
                    break;      
                case IDC_OK:
                    iIndex = SendMessage(hList,LB_GETCURSEL,0,0);//get the current selected item number
                    SetIndicatorLEDStat(iIndex);
		    	    Load_Icon_SetupList(hList,hIconNormal,hIconSel,2,iIndex);//change the radio button of the current selected item		
                    SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
                    break;
                default:
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

static BOOL     CallInfoLineSetting(HWND hFatherWnd)
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hFatherWnd;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = Info_AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pInfo_ClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);
    
    hInfo_WndApp = CreateWindow(pInfo_ClassName,NULL, 
                WS_VISIBLE | WS_CHILD,  
                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin, 
                NULL, NULL, NULL);

    if (NULL == hInfo_WndApp)
    {
        UnregisterClass(pInfo_ClassName,NULL);
        return FALSE;
    }

    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_CANCEL,(LPARAM)ML("Cancel") );
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SetWindowText(hFrameWin,ML("Info line"));

    SetFocus(hInfo_WndApp);

    return (TRUE);
}
static LRESULT  Info_AppWndProc   ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
    static  HWND    hList;
    static  int     iIndex;
    LRESULT lResult;
    RECT rect;
    lResult = TRUE;   

    switch ( wMsgCmd )
	{
		case WM_CREATE :
            GetClientRect(hWnd, &rect);
            hList = CreateWindow("LISTBOX", 0, 
                WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL/*|CS_NOSYSCTRL*/,   
                0,0,rect.right - rect.left, rect.bottom - rect.top,
                hWnd, (HMENU)IDC_INFOLIST, NULL, NULL);

            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ML("Cell ID"));
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ML("Date"));
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ML("Not used"));
            
            
            hdc         = GetDC(hWnd);
            hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            ReleaseDC(hWnd,hdc);
            iIndex = GetIdleShowInfo();
            
            Load_Icon_SetupList(hList,hIconNormal,hIconSel,3,iIndex);
            
            SendMessage(hList,LB_SETCURSEL,iIndex,0);           
            
		    break;

		case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_INFOLIST));
            break;

        case PWM_SHOWWINDOW:
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_CANCEL,(LPARAM)ML("Cancel") );
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
            SetWindowText(hFrameWin,ML("Info line"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            SetFocus(hWnd);    
            break;

		case WM_KEYDOWN:
            switch(LOWORD(wParam))
            {       
		        case VK_F10:
                    SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;
                case VK_F5:
                    SendMessage(hWnd,WM_COMMAND,IDC_OK,0);
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
		    hInfo_WndApp = NULL;
            UnregisterClass(pInfo_ClassName,NULL);
            break;

	    case WM_COMMAND :
	    	    
           
            switch (LOWORD(wParam))
		    {
                case IDC_CANCEL:
                    DestroyWindow(hWnd);
                    break;      
                case IDC_OK:
                    iIndex = SendMessage(hList,LB_GETCURSEL,0,0);              //get the current selected item number
                    SetIdleShowInfo(iIndex);
		    	    Load_Icon_SetupList(hList,hIconNormal,hIconSel,3,iIndex);     //change the radio button of the current selected item		
                    SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
                    break;
                default:
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
static void OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus)
{
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
}


/***************************************************************
* Function  GetBrightness()
* Purpose   Get the value of brightness
* Params
* Return    from 1 to 5, default: 3
* Remarks
***************************************************************/

LEVELVALUE GetBrightness(void)
{
    return FS_GetPrivateProfileInt(SN_DISPLAY,KN_BGLIGHT,LEV_THREE,SETUPFILENAME); 
}

/***************************************************************
* Function  SetBrightness()
* Purpose   Save the value of brightness
* Params
* Return
* Remarks
***************************************************************/
void SetBrightness(LEVELVALUE iLevel)
{    
    if(iLevel < 1 || iLevel > 5)
        return;

    if(Sett_BritAdjusting(iLevel))
        FS_WritePrivateProfileInt(SN_DISPLAY,KN_BGLIGHT,iLevel,SETUPFILENAME);
}

/***************************************************************
* Function  GetStartupEffect
* Purpose   get the start up effect
* Params
* Return
* Remarks
***************************************************************/
STARTUP_EFFECT GetStartupEffect(void)
{
    return FS_GetPrivateProfileInt(SN_DISPLAY, KN_START_EFFECT, SE_DEF, SETUPFILENAME);
}
/***************************************************************
* Function  SetStartupEffect
* Purpose
* Params
* Return
* Remarks
***************************************************************/
static void SetStartupEffect(STARTUP_EFFECT Seffect)
{
    FS_WritePrivateProfileInt(SN_DISPLAY, KN_START_EFFECT, Seffect, SETUPFILENAME);
    if( Seffect == SE_DEF || Seffect == SE_PIC)
         SetGreetingTxt("");
}
/***************************************************************
* Function
* Purpose
* Params
* Return
* Remarks
***************************************************************/
//Set and Get the greeting text
void GetGreetingTxt(char * szStartupTxt)
{
    FS_GetPrivateProfileString(SN_DISPLAY, KN_STARTTXT, "", szStartupTxt, MAXSTARTUPTXTLEN, SETUPFILENAME);
}
/***************************************************************
* Function
* Purpose
* Params
* Return
* Remarks
***************************************************************/
static void SetGreetingTxt(char * szGreetingTxt)
{
    FS_WritePrivateProfileString(SN_DISPLAY, KN_STARTTXT, szGreetingTxt, SETUPFILENAME);
}


/***************************************************************
* Function      SetBKPicture
* Purpose       save the name of background picture
* Params
* Return
* Remarks
***************************************************************/
BOOL SetBKPicture(char* szPicFile)
{
    FS_WritePrivateProfileString(SN_DISPLAY,KN_BKPICNAME,szPicFile,SETUPFILENAME);
    return TRUE;
}
/***************************************************************
* Function  GetBKPicture
* Purpose   get the name of background picture
* Params
* Return
* Remarks
***************************************************************/
BOOL GetBKPicture(char * szPicFile)
{
    FS_GetPrivateProfileString(SN_DISPLAY, KN_BKPICNAME, "Default", szPicFile, PREBROW_MAXFULLNAMELEN, SETUPFILENAME);
    if (Sett_FileIfExist(szPicFile))//if the picture has been deleted;
		return TRUE;
	else
	{
		strcpy(szPicFile, "Default");
		return TRUE;
	}
}

/***************************************************************
* Function
* Purpose
* Params
* Return
* Remarks
***************************************************************/
BOOL SetStartupPic(char* szPicFile)
{
    FS_WritePrivateProfileString(SN_DISPLAY, KN_STARTPICNAME,szPicFile,SETUPFILENAME);
    return TRUE;
}
/***************************************************************
* Function
* Purpose
* Params
* Return
* Remarks
***************************************************************/
BOOL GetStartupPic(char * szPicFile)
{
    FS_GetPrivateProfileString(SN_DISPLAY, KN_STARTPICNAME, "", szPicFile, PREBROW_MAXFULLNAMELEN+1, SETUPFILENAME);
    if (Sett_FileIfExist(szPicFile))//if the picture has been deleted;
		return TRUE;
	else
	{
		strcpy(szPicFile, "Default");
		return TRUE;
	}
}
