/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : connectivity setting
 *
 * Purpose  : main window
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "Connectivity.h"
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

static int nCurFocus = 0;
static HWND  hAppWnd; //connectivity setting window
static HWND  hFrameWin;
static char* pClassName = "ConnectSettingClass";

static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static BOOL CreateControl(HWND hWnd, HWND * hMessaging, HWND * hWAP, HWND * hConnections,
                          HWND * hGPRS, HWND * hDataCalls);
static void ConnectSet_InitVScrolls(HWND hWnd,int iItemNum);
static void ConnectSet_OnVScroll(HWND hWnd,  UINT wParam);
static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

extern      BOOL     CallGSMRadioSetting(HWND hFatherWnd);
extern      void     SetGSMRadioPartMode(SWITCHTYPE OnOrOff);
extern      BOOL     CallMsgingSetting(HWND hWnd);
extern      BOOL     CallConnectionSetting(HWND hwndCall);
extern      BOOL     CallGPRSSetting(HWND hwndCall);
extern      BOOL     CallDatacallSetting(HWND hwndCall);
extern      BOOL     WAP_Public_Setting(HWND hParent);
static void OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus);
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);

BOOL    CallConnectSetting(HWND hFatherWnd)
{
	WNDCLASS wc;

    RECT rClient;

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
    hFrameWin = hFatherWnd;

    SetWindowText(hFrameWin, ML("Connectivity"));
    
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
        IDC_BUTTON_BACK, (LPARAM)ML("Back"));//RSK
        
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif

    GetClientRect(hFrameWin, &rClient);

    hAppWnd = CreateWindow(pClassName,NULL, 
        WS_VISIBLE | WS_CHILD | WS_VSCROLL,  
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFatherWnd, NULL, NULL, NULL);

    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    SetFocus(hAppWnd);


    return TRUE;
    
}
static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hGSMRadio, hMessaging, hWAP, hConnections, hGPRS, hDataCalls;
    static HWND hFocus;
    static HWND hWndFocus;
    static int  iButtonJust, iCurIndex;
    
    LRESULT     lResult;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case PWM_SHOWWINDOW:
        SetWindowText(hFrameWin, ML("Connectivity"));
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_BACK, (LPARAM)ML("Back"));//RSK        
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        
        SetFocus(hWnd);
        
        break;
    
    case WM_CREATE:
        CreateControl(hWnd, &hMessaging, &hWAP, &hConnections, &hGPRS, &hDataCalls);
        ConnectSet_InitVScrolls(hWnd, 5);
        hFocus = hMessaging;
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
    	break;
    
    case WM_VSCROLL:
        ConnectSet_OnVScroll(hWnd, wParam);
        break;
    case WM_DESTROY:
        hAppWnd = NULL;
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
            SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_SEL, 0);
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
            case IDC_MSGING:
                CallMsgingSetting(hFrameWin);
                break;
            case IDC_WAP:
                WAP_Public_Setting(hFrameWin);
                break;
            case IDC_CONNECTION:
                CallConnectionSetting(hFrameWin);
                break;
            case IDC_GPRS:
                CallGPRSSetting(hFrameWin);
                break;
            case IDC_DATA_CALLS:
                CallDatacallSetting(hFrameWin);
                break;                
            }
            break;

        case IDC_MSGING:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_MSGING;
            break;
        case IDC_WAP:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_WAP;
            break;
        case IDC_CONNECTION:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_CONNECTION;
            break;
        case IDC_GPRS:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_GPRS;
            break;
        case IDC_DATA_CALLS:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_DATA_CALLS;
            break;                
        }
        break;
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static BOOL CreateControl(HWND hWnd, HWND * hMessaging, HWND * hWAP, HWND * hConnections,
                          HWND * hGPRS, HWND * hDataCalls)
{
    int iControlH,iControlW;
	int xzero=0,yzero=0;

	RECT    rect;
    GetClientRect(hWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;  

    * hMessaging = CreateWindow( "SPINBOXEX", ML("Messaging"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST  | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hWnd, (HMENU)IDC_MSGING, NULL, NULL);

    if (* hMessaging == NULL)
        return FALSE;  
    
    * hWAP = CreateWindow( "SPINBOXEX", ML("WAP"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST  | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH, iControlW, iControlH, 
        hWnd, (HMENU)IDC_WAP, NULL, NULL);

    if (* hWAP == NULL)
        return FALSE;  
   
    * hConnections = CreateWindow( "SPINBOXEX", ML("Connections"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST  | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 2, iControlW, iControlH, 
        hWnd, (HMENU)IDC_CONNECTION, NULL, NULL);

    if (* hConnections == NULL)
        return FALSE;  

    * hGPRS = CreateWindow( "SPINBOXEX", ML("GPRS"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 3, iControlW, iControlH, 
        hWnd, (HMENU)IDC_GPRS, NULL, NULL);

    if (* hGPRS == NULL)
        return FALSE;  
    
    * hDataCalls = CreateWindow( "SPINBOXEX", ML("Data call"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 4, iControlW, iControlH, 
        hWnd, (HMENU)IDC_DATA_CALLS, NULL, NULL);

    if (* hDataCalls == NULL)
        return FALSE;  
	return TRUE;
}
/*********************************************************************\
* Function        MobileSet_InitVScrolls
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
static void ConnectSet_InitVScrolls(HWND hWnd,int iItemNum)
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
	nCurFocus = 0;
	return;
}
/*********************************************************************\
* Function        TimeSet_OnVScroll
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/

static void ConnectSet_OnVScroll(HWND hWnd,  UINT wParam)
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
