   /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : mobile setting
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
#include    "mobilesetting.h"
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
#include    "hopen/soundcard.h"
#include    "hopen/lcd.h"
#include	"compatable.h"
static  HWND    hAppWnd = NULL;
static  HWND    hFrameWin=NULL;

static int      nCurFocus;
static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;
static int      iLangFlag = 0;

static LRESULT  AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL     CreateControl (HWND hwnd, HWND *hLanguage, HWND *hShortcut, HWND *hMenuMgr, HWND *hDisplay,
                           HWND *hDateTime, HWND *hUnits, HWND *hPowerOn,
                           HWND *hKeyLock);

static void MobileSet_InitVScrolls(HWND hWnd,int iItemNum);
static void MobileSet_OnVScroll(HWND hWnd,  UINT wParam);

BOOL SetKeyLockMode(KEYLOCKMODE iKeylockmode);
extern  BOOL    GenPhoSetting(HWND hwndCall);
extern  BOOL    TimeSetting(HWND hwndCall);
extern  BOOL    CallDisplaySettingWindow(HWND hwndCall);
extern  BOOL    CallShortCutSetting(HWND hwndCall);
extern  BOOL    CallAutoOSWindow(HWND hwndCall,int icallmsg);
extern  BOOL    CallUnitSetting(HWND hFatherWnd);
extern  BOOL    CallKeyLockSetting(HWND hWnd);
extern  BOOL	MenuManagerCtrl( HWND hParentWnd);

static void OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus);
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);

BOOL    CallPhoneSettingList(HWND hwndCall)
{    
    WNDCLASS wc;            
    RECT rClient;

    hFrameWin = hwndCall;

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

    hAppWnd = CreateWindow(pClassName,NULL, 
                WS_VISIBLE | WS_CHILD|WS_VSCROLL,  
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
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_EXIT,(LPARAM)ML("Back"));                
   
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SetWindowText(hFrameWin,ML("Phone"));

    SetFocus(hAppWnd);

    return TRUE;
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  int     iIndex=0,i,iButtonJust,iCurIndex;
    static  HWND    hLanguage,hShortcut,hMenuMgr, hDisplay,hDateTime,hUnits,hPowerOn,hKeyLock;
    static  HWND    hWndFocus;
    static  HWND    hFocus;
    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
    {
    case PWM_SHOWWINDOW:    
        SetFocus(hWnd);
        SetWindowText(hFrameWin,ML("Phone"));
        
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
        
		if (1 == iLangFlag)
		{
			SendMessage(hLanguage, SSBM_SETTITLE, 0, (LPARAM)ML("Language"));
			SendMessage(hShortcut, SSBM_SETTITLE, 0, (LPARAM)ML("Shortcuts"));
			SendMessage(hMenuMgr,  SSBM_SETTITLE, 0, (LPARAM)ML("Menu manager"));
			SendMessage(hDisplay,  SSBM_SETTITLE, 0, (LPARAM)ML("Display"));
			SendMessage(hDateTime, SSBM_SETTITLE, 0, (LPARAM)ML("Date and time"));
			SendMessage(hUnits,    SSBM_SETTITLE, 0, (LPARAM)ML("Units"));
			SendMessage(hPowerOn,  SSBM_SETTITLE, 0, (LPARAM)ML("Auto switch onsloff"));
			SendMessage(hKeyLock,  SSBM_SETTITLE, 0, (LPARAM)ML("Keylock"));
			SendMessage(hKeyLock,  SSBM_DELETESTRING,0,(LPARAM)0);
			SendMessage(hKeyLock,  SSBM_DELETESTRING,0,(LPARAM)0);
			SendMessage(hKeyLock,  SSBM_ADDSTRING,0,(LPARAM)ML("Manual"));
			SendMessage(hKeyLock,  SSBM_ADDSTRING,0,(LPARAM)ML("Automatic"));
			iLangFlag = 0;
		}
        SendMessage(hKeyLock, SSBM_SETCURSEL, GetKeyLockMode(), 0);
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        
        break;

    case WM_CREATE :
        CreateControl (hWnd, &hLanguage, &hShortcut, &hMenuMgr, &hDisplay, &hDateTime, 
                        &hUnits, &hPowerOn,&hKeyLock);  
        MobileSet_InitVScrolls(hWnd,MAX_ITEMNUM);			
        SendMessage(hKeyLock, SSBM_SETCURSEL, GetKeyLockMode(), 0);
        hFocus=hLanguage;
		break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;
        
    case WM_VSCROLL:
		MobileSet_OnVScroll(hWnd,wParam);
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
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_OK,0);
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

    case WM_COMMAND :                
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_OK:
            switch(iButtonJust)
            {
            case IDC_LANGUAGE:
				GenPhoSetting(hFrameWin);
                break;
            case IDC_SHORTCUT:
                CallShortCutSetting(hFrameWin);
                break;
			case IDC_MENUMGR:
				MenuManagerCtrl(hFrameWin);
				break;
            case IDC_DISPLAY:
 				CallDisplaySettingWindow(hFrameWin);
                break;
            case IDC_DATETIME:
				TimeSetting(hFrameWin);
                break;
            case IDC_UNITS:
                CallUnitSetting(hFrameWin);
                break;
            case IDC_POWERON:
                CallAutoOSWindow(hFrameWin,CALLBACK_AUTOOS);
                break;
            case IDC_KEYLOCK:
                CallKeyLockSetting(hFrameWin);
                break;
            }
            break;
        case IDC_LANGUAGE:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_LANGUAGE;
            break;
        case IDC_SHORTCUT:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_SHORTCUT;
            break;
        case IDC_MENUMGR:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_MENUMGR;
            break;
        case IDC_DISPLAY:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_DISPLAY;
            break;
        case IDC_DATETIME:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_DATETIME;
            break;
        case IDC_UNITS:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_UNITS;
            break;
        case IDC_POWERON:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_POWERON;
            break;
        case IDC_KEYLOCK:
            if(HIWORD( wParam ) == SSBN_CHANGE)
			{
				iCurIndex = SendMessage(hKeyLock,SSBM_GETCURSEL,0,0);
				SetKeyLockMode(iCurIndex);
			}
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_KEYLOCK;
            break;
        }
	    break;

    case WM_DESTROY :
         hAppWnd = NULL;
         UnregisterClass(pClassName,NULL);
         break;
	case WM_CLOSE:
         DestroyWindow(hWnd);
         break;
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    } 
    return lResult;
}
/*==========================================================================
*   Function:  static void CreateControl ( HWND hWnd )
*   Purpose :  create the control of the window
*   Argument:  HWND hWnd
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/
static BOOL CreateControl (HWND hwnd, HWND *hLanguage, HWND *hShortcut, HWND *hMenuMgr, HWND *hDisplay,
                           HWND *hDateTime, HWND *hUnits, HWND *hPowerOn,
                           HWND *hKeyLock)
{
	int     xzero=0,yzero=0,iControlH,iControlW;
	RECT    rect;
    GetClientRect(hwnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;

    * hLanguage = CreateWindow( "SPINBOXEX", ML("Language"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |/* WS_BORDER |*/ WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hwnd, (HMENU)IDC_LANGUAGE, NULL, NULL);

    if (* hLanguage == NULL)
        return FALSE;

    * hShortcut = CreateWindow( "SPINBOXEX", ML("Shortcuts"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH, iControlW, iControlH, 
        hwnd, (HMENU)IDC_SHORTCUT, NULL, NULL);

    if (* hShortcut == NULL)
        return FALSE;

    * hMenuMgr = CreateWindow( "SPINBOXEX", ML("Menu manager"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*2, iControlW, iControlH, 
        hwnd, (HMENU)IDC_MENUMGR, NULL, NULL);

    if (* hMenuMgr == NULL)
        return FALSE;
    
	
	* hDisplay = CreateWindow( "SPINBOXEX", ML("Display"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*3, iControlW, iControlH, 
        hwnd, (HMENU)IDC_DISPLAY, NULL, NULL);

    if (* hDisplay == NULL)
        return FALSE;

     * hDateTime = CreateWindow( "SPINBOXEX", ML("Date and time"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*4, iControlW, iControlH, 
        hwnd, (HMENU)IDC_DATETIME, NULL, NULL);

    if (* hDateTime == NULL)
        return FALSE;
     
    * hUnits = CreateWindow( "SPINBOXEX", ML("Units"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*5, iControlW, iControlH, 
        hwnd, (HMENU)IDC_UNITS, NULL, NULL);

    if (* hUnits == NULL)
        return FALSE;
    
    * hPowerOn = CreateWindow( "SPINBOXEX", ML("Auto switch onsloff"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |/* WS_BORDER |*/ WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*6, iControlW, iControlH, 
        hwnd, (HMENU)IDC_POWERON, NULL, NULL);

    if (* hPowerOn == NULL)
        return FALSE;
    * hKeyLock = CreateWindow( "SPINBOXEX", ML("Keylock"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*7, iControlW, iControlH, 
        hwnd, (HMENU)IDC_KEYLOCK, NULL, NULL);

    if (* hKeyLock == NULL)
        return FALSE;
    SendMessage(* hKeyLock,SSBM_ADDSTRING,0,(LPARAM)ML("Manual"));
    SendMessage(* hKeyLock,SSBM_ADDSTRING,0,(LPARAM)ML("Automatic"));
    
  return TRUE;
}
/*********************************************************************\
* Function        MobileSet_InitVScrolls
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
static void MobileSet_InitVScrolls(HWND hWnd,int iItemNum)
{
    SCROLLINFO   vsi;

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

static void MobileSet_OnVScroll(HWND hWnd,  UINT wParam)
{
    int  nY;
    RECT rcClient;
    SCROLLINFO      vsi;

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


/***************************************************************
* Function  Sett_BritAdjusting()
* Purpose   Set the brightness of the display    
* Params
* Return    
* Remarks   brighness value is from 1 to 5;
***************************************************************/
BOOL Sett_BritAdjusting(LEVELVALUE level)
{
	DHI_SetBkLightVolume(level);
     
	return TRUE; 
}
/***************************************************************
* Function  Sett_VolumeAdjusting()
* Purpose   Set the volume    
* Params
* Return    
* Remarks   volume value is from 0 to 5; the corresponding value
*           for ioctl is from 0x00 to 0x39
***************************************************************/
//BOOL Sett_VolumeAdjusting(LEVELVALUE level)
//{
//    int fmixer;
//    int ioret = 0;
//    fmixer = open("/dev/mixer",O_WRONLY);
//	if(fmixer < 0)
//	{
//		printf("open mixer failed!!\r\n");
//		return FALSE;
//	}	
//    switch(level)
//    {
//    case LEV_ZERO:
//        ioret = ioctl(fmixer, SOUND_MIXER_WRITE_VOLUME, LEVEL_ZERO);
//    	break;
//    case LEV_ONE:
//        ioret = ioctl(fmixer, SOUND_MIXER_WRITE_VOLUME, LEVEL_ONE);
//    	break;
//    case LEV_TWO:
//        ioret = ioctl(fmixer, SOUND_MIXER_WRITE_VOLUME, LEVEL_TWO);
//        break;
//    case LEV_THREE:
//        ioret = ioctl(fmixer, SOUND_MIXER_WRITE_VOLUME, LEVEL_THREE);
//        break;
//    case LEV_FOUR:
//        ioret = ioctl(fmixer, SOUND_MIXER_WRITE_VOLUME, LEVEL_FOUR);
//        break;
//    case LEV_FIVE:
//        ioret = ioctl(fmixer, SOUND_MIXER_WRITE_VOLUME, LEVEL_FIVE);
//        break;
//    default:
//        break;
//    }
//	close(fmixer);
//
//    if (-1 == ioret)
//    {
//		printf("volume ioctl faild!!\r\n");
//        return FALSE;
//    }
//
//	return TRUE;
//
//}

void SettListProcessKeyDown(HWND hWnd, HWND *hFocus)
{
    HWND hWndFocus;

    hWndFocus = GetFocus();
    
    while(GetParent(hWndFocus) != hWnd)
        hWndFocus = GetParent(hWndFocus);
    
    hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, FALSE);
    *hFocus = hWndFocus;
    SetFocus(hWndFocus);
}

void SettListProcessKeyUp(HWND hWnd, HWND *hFocus)
{
    HWND hWndFocus;

    hWndFocus = GetFocus();
    
    while(GetParent(hWndFocus) != hWnd)
        hWndFocus = GetParent(hWndFocus);
    
    hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, TRUE);
    *hFocus = hWndFocus;
    SetFocus(hWndFocus);

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
void Sett_MobileProcLang(void)
{
	iLangFlag = 1;
}
