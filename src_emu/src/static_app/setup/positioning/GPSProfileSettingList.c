  /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : GPS profile
 *
 * Purpose  : setting list of GPS profile
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "GPSProfileSettingList.h"
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
static char * pClassName = "GPSProfileSettingListClass";
static int nCurFocus;
static int iCurGPSIndex;
static GPSMODE  Global_gm;
static int iTitleFlag;
static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static BOOL CreateControl(HWND hWnd, HWND * hName, HWND * hIcon, HWND * hRefintvl, 
                          HWND * hRefintcha, HWND * hOpti, HWND * hNMEA);
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void GPSProfileSet_OnVScroll(HWND hWnd,  UINT wParam);
static void GPSProfileSet_InitVScrolls(HWND hWnd,int iItemNum);

static void OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus);

extern  BOOL GetGPSprofile(GPSMODE *gm, int iGPSModeIndex);


extern  BOOL CallEditGPSName(HWND hWnd, int iCurGPSIndex);
extern  BOOL CallSetGPSRefIntvl(HWND hWnd, int iCurGPSIndex);
extern  BOOL CallSetGPSRefIntCha(HWND hWnd, int iCurGPSIndex);
extern  BOOL CallSetGPSOpti(HWND hWnd, int iCurGPSIndex);
extern  BOOL CallSetGPSNMEA(HWND hWnd, int iCurGPSIndex);

extern  BOOL SetGPSprofile(GPSMODE *gm, int iGPSModeIndex);

extern  BOOL CallSelGPSIcon(HWND hWnd, int iCurGPSIndex);
extern  void Sett_GPS_ConvertIconName(char* szIconName);

void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);
void TransferBmpAndIcon(char* OldPath, char* NewPath);


BOOL CallGPSSettingList(HWND hParentWnd, int iGPSIndex, int iNewOrEdit) //iNewOrEdit: from edit(1) or new create(0);
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hParentWnd;
    
    iCurGPSIndex = iGPSIndex;
	iTitleFlag = iNewOrEdit;

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
        WS_VISIBLE | WS_CHILD | WS_VSCROLL, 
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
        (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back") );
	
	if ( 1 == iTitleFlag )//from edit
	{
		if ( 0 == iGPSIndex )
			SetWindowText(hFrameWin, ML("Full power"));
		else if ( 1 == iGPSIndex)
			SetWindowText(hFrameWin, ML("Economy"));
		else
			SetWindowText(hFrameWin, Global_gm.cGPSModeName);
	}
	else//if (0 == iTitleFlag)
	{
		SetWindowText(hFrameWin, ML("New GPS profile"));			
	} SetFocus(hWndApp);
    
    return (TRUE);

}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HWND        hName, hIcon, hRefintvl, hRefintcha, hOpti, hNMEA;
    static      HWND        hFocus;
    static      int         iButtonJust;
    static      int         iIndex,iCurIndex;
	LRESULT     lResult;
	char		/**Tmp, */TmpName[GPSICONNAMEMAXLEN+1];
    static      HWND hWndFocus;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
		case WM_CREATE :
            CreateControl(hWnd, &hName,&hIcon, &hRefintvl, &hRefintcha, &hOpti, &hNMEA);
			hFocus=hName;
			GPSProfileSet_InitVScrolls(hWnd,6);

            SendMessage(hName, SSBM_ADDSTRING, 0, (LPARAM)Global_gm.cGPSModeName);

            SendMessage(hRefintvl, SSBM_SETCURSEL, Global_gm.iRefIntervl, 0);
            SendMessage(hRefintcha, SSBM_SETCURSEL, Global_gm.iRefMode, 0);
            SendMessage(hOpti, SSBM_SETCURSEL, Global_gm.iOptMode, 0);
            SendMessage(hNMEA, SSBM_SETCURSEL, Global_gm.iNMEAoutput, 0);

			TransferBmpAndIcon(Global_gm.cGPSIconName, TmpName);
			SendMessage(hIcon, SSBM_SETTEXT, (WPARAM)strlen(TmpName), (LPARAM)TmpName);
			
//			
//			Tmp = strrchr(Global_gm.cGPSIconName, '/');	
//			if(Tmp != 0)
//			{
//				Tmp++;
//				Sett_GPS_ConvertIconName(Tmp);
//				memset(TmpName, 0, GPSICONNAMEMAXLEN);
//				UTF8ToMultiByte(CP_ACP,0,Tmp,strlen(Tmp),TmpName,GPSICONNAMEMAXLEN,NULL,NULL);
//				
//				SendMessage(hIcon,SSBM_ADDSTRING,0,(LPARAM)TmpName);
//			}
			
        	break;

		case WM_SETFOCUS:
            SetFocus(hFocus);
            break;

        case WM_VSCROLL:
	    	GPSProfileSet_OnVScroll(hWnd,wParam);
		    break;

        case PWM_SHOWWINDOW:
            SetFocus(hWnd);
            GetGPSprofile(&Global_gm, iCurGPSIndex);
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back") );
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            
			if ( 1 == iTitleFlag )//from edit
			{
				if ( 0 == iCurGPSIndex )
					SetWindowText(hFrameWin, ML("Full power"));
				else if ( 1 == iCurGPSIndex)
					SetWindowText(hFrameWin, ML("Economy"));
				else
					SetWindowText(hFrameWin, Global_gm.cGPSModeName);
			}
			else//if (0 == iTitleFlag)
			{
	            SetWindowText(hFrameWin, ML("New GPS profile"));			
			}
            
            SendMessage(hName, SSBM_DELETESTRING, 0, 0);
            SendMessage(hName, SSBM_ADDSTRING, 0, (LPARAM)Global_gm.cGPSModeName);    
            SendMessage(hRefintvl, SSBM_SETCURSEL, Global_gm.iRefIntervl, 0);
            SendMessage(hRefintcha, SSBM_SETCURSEL, Global_gm.iRefMode, 0);
            SendMessage(hOpti, SSBM_SETCURSEL, Global_gm.iOptMode, 0);
            SendMessage(hNMEA, SSBM_SETCURSEL, Global_gm.iNMEAoutput, 0);

			TransferBmpAndIcon(Global_gm.cGPSIconName, TmpName);
			SendMessage(hIcon, SSBM_SETTEXT, (WPARAM)strlen(TmpName), (LPARAM)TmpName);
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
                    if (iCurGPSIndex < 2)
                    {
                        PLXTipsWin(hFrameWin, hWnd, NULL,ML("Not allowedsnfor preset profile"),Global_gm.cGPSModeName,Notify_Failure,ML("Ok"),0,WAITTIMEOUT);
                        break;
                    }

                    switch(iButtonJust)
			        {
    		            case IDC_GPS_NAME:
 			                CallEditGPSName(hFrameWin, iCurGPSIndex);
                            break;
		                case IDC_GPS_ICON:
                            CallSelGPSIcon(hFrameWin, iCurGPSIndex);
 			                break;
			            case IDC_GPS_REFINTVL:
                            CallSetGPSRefIntvl(hFrameWin, iCurGPSIndex);
 			                break;
			            case IDC_GPS_REFINTCHA:
                            CallSetGPSRefIntCha(hFrameWin, iCurGPSIndex);
 			                break;
			            case IDC_GPS_OPTI:
                            CallSetGPSOpti(hFrameWin, iCurGPSIndex);
			                break;
			            case IDC_GPS_NMEA:
                            CallSetGPSNMEA(hFrameWin, iCurGPSIndex);
			                break;
			         }
                     break;

	        	case IDC_GPS_NAME:
                    if(HIWORD(wParam) == SSBN_SETFOCUS)
    			        iButtonJust = IDC_GPS_NAME;
			        break;
            
		        case IDC_GPS_ICON:
                    if(HIWORD(wParam) == SSBN_SETFOCUS)
    			        iButtonJust = IDC_GPS_ICON;
			        break;

		        case IDC_GPS_REFINTVL:
			        if(HIWORD( wParam ) == SSBN_CHANGE)
			        {
						if (iCurGPSIndex < 2)
						{
							PLXTipsWin(hFrameWin, hWnd, NULL,ML("Not allowedsnfor preset profile"),Global_gm.cGPSModeName,Notify_Failure,ML("Ok"),0,WAITTIMEOUT);
							break;
						}
				        iCurIndex = SendMessage(hRefintvl,SSBM_GETCURSEL,0,0);
                        Global_gm.iRefIntervl = iCurIndex;
                        SetGPSprofile(&Global_gm, iCurGPSIndex);
			        }
                    if(HIWORD(wParam) == SSBN_SETFOCUS)
    			        iButtonJust = IDC_GPS_REFINTVL;
			        break;

		        case IDC_GPS_REFINTCHA:
			        if(HIWORD( wParam ) == SSBN_CHANGE)
			        {
						if (iCurGPSIndex < 2)
						{
							PLXTipsWin(hFrameWin, hWnd, NULL,ML("Not allowedsnfor preset profile"),Global_gm.cGPSModeName,Notify_Failure,ML("Ok"),0,WAITTIMEOUT);
							break;
						}
				        iCurIndex = SendMessage(hRefintcha,SSBM_GETCURSEL,0,0);
                        Global_gm.iRefMode = iCurIndex;
                        SetGPSprofile(&Global_gm, iCurGPSIndex);
			        }
                    if(HIWORD(wParam) == SSBN_SETFOCUS)
    			        iButtonJust = IDC_GPS_REFINTCHA;
		        	break;

		        case IDC_GPS_OPTI:
		        	if(HIWORD( wParam ) == SSBN_CHANGE)
		        	{
						if (iCurGPSIndex < 2)
						{
							PLXTipsWin(hFrameWin, hWnd, NULL,ML("Not allowedsnfor preset profile"),Global_gm.cGPSModeName,Notify_Failure,ML("Ok"),0,WAITTIMEOUT);
							break;
						}
		           		iCurIndex = SendMessage(hOpti,SSBM_GETCURSEL,0,0);
                        Global_gm.iOptMode = iCurIndex;
                        SetGPSprofile(&Global_gm, iCurGPSIndex);
		        	}
                    if(HIWORD(wParam) == SSBN_SETFOCUS)
    			        iButtonJust = IDC_GPS_OPTI;
			        break;

		        case IDC_GPS_NMEA:
		        	if(HIWORD( wParam ) == SSBN_CHANGE)
		        	{
						if (iCurGPSIndex < 2)
						{
							PLXTipsWin(hFrameWin, hWnd, NULL,ML("Not allowedsnfor preset profile"),Global_gm.cGPSModeName,Notify_Failure,ML("Ok"),0,WAITTIMEOUT);
							break;
						}
		           		iCurIndex = SendMessage(hNMEA,SSBM_GETCURSEL,0,0);
                        Global_gm.iNMEAoutput = iCurIndex;
                        SetGPSprofile(&Global_gm, iCurGPSIndex);
		        	}
                    if(HIWORD(wParam) == SSBN_SETFOCUS)
    		        	iButtonJust = IDC_GPS_NMEA;
		        	break;
	    	}
		    break;

	    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}
	return lResult;
}
static BOOL CreateControl(HWND hWnd, HWND * hName, HWND * hIcon, HWND * hRefintvl, 
                          HWND * hRefintcha, HWND * hOpti, HWND * hNMEA)
{
    int iControlH, iControlW;
    RECT rect;
    int xzero = 0, yzero = 0;
    GetClientRect(hWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;
    
    * hName = CreateWindow( "SPINBOXEX", ML("Name"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hWnd, (HMENU)IDC_GPS_NAME, NULL, NULL);

    if (* hName == NULL)
        return FALSE;
    * hIcon = CreateWindow( "LEVIND", ML("Icon"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH, iControlW, iControlH, 
        hWnd, (HMENU)IDC_GPS_ICON, NULL, (PVOID)LAS_ICON_SHOW);

    if (* hIcon == NULL)
        return FALSE;
    * hRefintvl = CreateWindow( "SPINBOXEX", ML("Refresh interval"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 2, iControlW, iControlH, 
        hWnd, (HMENU)IDC_GPS_REFINTVL, NULL, NULL);

    if (* hRefintvl == NULL)
        return FALSE;

    SendMessage(* hRefintvl, SSBM_ADDSTRING, 0, (LPARAM)ML("Off"));
    SendMessage(* hRefintvl, SSBM_ADDSTRING, 0, (LPARAM)ML("GPSRefInt1s"));
    SendMessage(* hRefintvl, SSBM_ADDSTRING, 0, (LPARAM)ML("GPSRefInt3s"));
    SendMessage(* hRefintvl, SSBM_ADDSTRING, 0, (LPARAM)ML("GPSRefInt10s"));
    SendMessage(* hRefintvl, SSBM_ADDSTRING, 0, (LPARAM)ML("GPSRefInt30min"));


    * hRefintcha = CreateWindow( "SPINBOXEX", ML("Ref. int. charging"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 3, iControlW, iControlH, 
        hWnd, (HMENU)IDC_GPS_REFINTCHA, NULL, NULL);

    if (* hRefintcha == NULL)
        return FALSE;
    
    SendMessage(* hRefintcha, SSBM_ADDSTRING, 0, (LPARAM)ML("Continuous"));
    SendMessage(* hRefintcha, SSBM_ADDSTRING, 0, (LPARAM)ML("Keep interval"));
    SendMessage(* hRefintcha, SSBM_ADDSTRING, 0, (LPARAM)ML("Off"));

    * hOpti = CreateWindow( "SPINBOXEX", ML("Optimized for"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 4, iControlW, iControlH, 
        hWnd, (HMENU)IDC_GPS_OPTI, NULL, NULL);

    if (* hOpti == NULL)
        return FALSE;

    SendMessage(* hOpti, SSBM_ADDSTRING, 0, (LPARAM)ML("Accuracy"));
    SendMessage(* hOpti, SSBM_ADDSTRING, 0, (LPARAM)ML("Sensitivity"));

    * hNMEA = CreateWindow( "SPINBOXEX", ML("NMEA output"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 5, iControlW, iControlH, 
        hWnd, (HMENU)IDC_GPS_NMEA, NULL, NULL);

    if (* hNMEA == NULL)
        return FALSE;

    SendMessage(* hNMEA, SSBM_ADDSTRING, 0, (LPARAM)ML("Off"));
    SendMessage(* hNMEA, SSBM_ADDSTRING, 0, (LPARAM)ML("4800 bps"));
    SendMessage(* hNMEA, SSBM_ADDSTRING, 0, (LPARAM)ML("9600 bps"));

    return TRUE;

}
/*********************************************************************\
* Function        GPSProfileSet_InitVScrolls
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
static void GPSProfileSet_InitVScrolls(HWND hWnd,int iItemNum)
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
* Function        GPSProfileSet_OnVScroll
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/

static void GPSProfileSet_OnVScroll(HWND hWnd,  UINT wParam)
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
