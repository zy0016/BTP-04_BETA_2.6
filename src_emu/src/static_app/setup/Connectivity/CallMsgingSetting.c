/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : connectivity setting
 *
 * Purpose  : messaging related setting
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "CallMsgingSetting.h"
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

#define  TIMER_ID WM_USER+100
//#define  CALLBACK_SETVMN WM_USER+101
#define  VMBN_LEN 50
static char* pClassName = "Connect_MsgingSettingClass";
static HWND hAppWnd;
static int nCurFocus;
static HWND hMsgFrame;

static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void MsgingSet_OnVScroll(HWND hWnd,  UINT wParam);
static void MsgingSet_InitVScrolls(HWND hWnd,int iItemNum);
static BOOL CreateControl(HWND hWnd, HWND * hVoiceMailbox, HWND * hSMS, HWND * hMMS, HWND * hEmail,
                          HWND * hCellBrdkst);

extern BOOL MMS_CreateSettingWnd(HWND hWndFrame, HWND hwndParent);
extern BOOL MAIL_CreateSetWnd(HWND hParent);
extern BOOL SMS_VMNSetting(HWND hFrameWnd);

//extern BOOL SMS_VMNSettingEx(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd);

extern BOOL SMS_CreateSettingListWnd(HWND hFrameWnd);
extern BOOL CB_CreateSettingWnd(HWND hFrameWnd);

extern BOOL GetVoiceMailBox (char* pszTel,int nlen);

static void OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus);
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);

BOOL    CallMsgingSetting(HWND hWnd)
{
	WNDCLASS wc;
    RECT rect;

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

    hMsgFrame = hWnd;
    
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
        
    SendMessage(hMsgFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hMsgFrame , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SendMessage(hMsgFrame, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_BACK, (LPARAM)ML("Back"));//RSK
    SetWindowText(hMsgFrame, ML("Messaging"));

    GetClientRect(hMsgFrame, &rect);

    hAppWnd = CreateWindow(pClassName,NULL, 
        WS_VISIBLE| WS_CHILD | WS_VSCROLL,  
        rect.left,
        rect.top,
        rect.right-rect.left,
        rect.bottom-rect.top,
        hMsgFrame, NULL, NULL, NULL);    

    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
/*
    ShowWindow(hMsgFrame, SW_SHOW);
    UpdateWindow(hMsgFrame); 
*/
    SetFocus ( hAppWnd );	
    
    return TRUE;
    
}
#define CALLBACK_GETVMBN WM_USER+113
static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hVoiceMailbox, hSMS, hMMS, hEmail, hCellBrdkst;
    static HWND hFocus;
    static HWND hWndFocus;
    static int  iButtonJust, iCurIndex;
    static char szVMBN[VMBN_LEN];
    LRESULT     lResult;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:
        CreateControl(hWnd, &hVoiceMailbox, &hSMS, &hMMS, &hEmail, &hCellBrdkst);

        MsgingSet_InitVScrolls(hWnd, 5);
		memset(szVMBN, 0, sizeof(szVMBN));
		GetVoiceMailBox(szVMBN, sizeof(szVMBN));
		SendMessage(hVoiceMailbox, SSBM_ADDSTRING, 0, (LPARAM)szVMBN);

    	hFocus = hVoiceMailbox;
        break;
        
    case PWM_SHOWWINDOW:
        SendMessage(hMsgFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hMsgFrame , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
        SendMessage(hMsgFrame, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_BACK, (LPARAM)ML("Back"));//RSK
        SetWindowText(hMsgFrame, ML("Messaging"));
		GetVoiceMailBox(szVMBN, sizeof(szVMBN));
		SendMessage(hVoiceMailbox, SSBM_DELETESTRING, 0, 0);
		SendMessage(hVoiceMailbox, SSBM_ADDSTRING, 0, (LPARAM)szVMBN);
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hMsgFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hMsgFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        SetFocus(hWnd);
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
    	break;
	case WM_KILLFOCUS:
		KillTimer(hWnd, TIMER_ID);
		RepeatFlag = 0;
		wKeyCode   = 0;
		break;        
    
    case WM_VSCROLL:
        MsgingSet_OnVScroll(hWnd, wParam);
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
            SendMessage(hMsgFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
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
            case IDC_VOICEMAILBOX:
                SMS_VMNSetting(hMsgFrame);
                break;
            case IDC_SMS:
                SMS_CreateSettingListWnd(hMsgFrame);
                break;
            case IDC_MMS:
                MMS_CreateSettingWnd(hMsgFrame, hWnd);
                break;
            case IDC_EMAIL:
                MAIL_CreateSetWnd(hMsgFrame);
                break;
            case IDC_CELLBRDKST:
                CB_CreateSettingWnd(hMsgFrame);
                break;
            }
            break;
        case IDC_BUTTON_BACK:
            DestroyWindow(hWnd);
            break;
        case IDC_VOICEMAILBOX:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_VOICEMAILBOX;
            break;
        case IDC_SMS:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_SMS;
            break;
        case IDC_MMS:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_MMS;
            break;
        case IDC_EMAIL:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_EMAIL;
            break;
        case IDC_CELLBRDKST:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_CELLBRDKST;
            break;
        }
        break;   
	default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static BOOL CreateControl(HWND hWnd, HWND * hVoiceMailbox, HWND * hSMS, HWND * hMMS, HWND * hEmail,
                          HWND * hCellBrdkst)
{
    int iControlH,iControlW;
	int xzero=0,yzero=0;

	RECT    rect;
    GetClientRect(hWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;
    
    * hVoiceMailbox = CreateWindow( "SPINBOXEX", ML("Voice mailbox"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hWnd, (HMENU)IDC_VOICEMAILBOX, NULL, NULL);

    if (* hVoiceMailbox == NULL)
        return FALSE;  

    * hSMS = CreateWindow( "SPINBOXEX", ML("SMS"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST  | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH, iControlW, iControlH, 
        hWnd, (HMENU)IDC_SMS, NULL, NULL);

    if (* hSMS == NULL)
        return FALSE;  
    
    * hMMS = CreateWindow( "SPINBOXEX", ML("MMS"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST  | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 2, iControlW, iControlH, 
        hWnd, (HMENU)IDC_MMS, NULL, NULL);

    if (* hMMS == NULL)
        return FALSE;  
   
    * hEmail = CreateWindow( "SPINBOXEX", ML("Etomail"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST  | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 3, iControlW, iControlH, 
        hWnd, (HMENU)IDC_EMAIL, NULL, NULL);

    if (* hEmail == NULL)
        return FALSE;  

    * hCellBrdkst = CreateWindow( "SPINBOXEX", ML("Cell broadcasts"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 4, iControlW, iControlH, 
        hWnd, (HMENU)IDC_CELLBRDKST, NULL, NULL);

    if (* hCellBrdkst == NULL)
        return FALSE;  
    
return TRUE;
    
}
/*********************************************************************\
* Function        MsgingSet_InitVScrolls
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
static void MsgingSet_InitVScrolls(HWND hWnd,int iItemNum)
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
* Function        MsgingSet_OnVScroll
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/

static void MsgingSet_OnVScroll(HWND hWnd,  UINT wParam)
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
