                            /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : CPHS application / mailbox number
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
#include    "MailboxNum.h"
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
#include    "plxdebug.h"

#define TIMER_ID     WM_USER + 101

static  HWND        hFrameWin=NULL;
static  HWND        hWndApp=NULL;
static  HINSTANCE   hInstance;
static  int iInfoFlag = 0;
static  int nCurFocus;
static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;
static char szNumber[40];
static LRESULT  AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL CreateControl(HWND hwnd, HWND * hVoiceMailOne, HWND * hVoiceMailTwo, HWND * hDataBox, HWND * hFaxBox);
static void CPHS_OnVScroll(HWND hWnd,  UINT wParam);
static void CPHS_InitVScrolls(HWND hWnd,int iItemNum);

extern int GetMailboxNumInfo(HWND hWnd, UINT imsg);
extern BOOL GetMailboxNumResult(HWND hWnd, UINT imsg, UINT iDecodeMsg);
extern int GetMailboxData(HWND hWnd, int imsg, WPARAM wParam);
extern BOOL CPHS_Mailbox_init_space(HWND hWnd, int imsg);  
extern void CPHS_Mailbox_free_space();
extern void ShowMailboxNum(HWND hControl1, HWND hControl2, HWND hControl3, HWND hControl4);
static void OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus);
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);

BOOL    APP_CallPhoneNumber(const char * pPhoneNumber);

DWORD SIMmailbox_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
    WNDCLASS wc;
    DWORD dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT :
        hInstance = pInstance;
        break;
/*
	case APP_GETOPTION:
		switch(wParam)
        {
		case AS_APPWND:
			dwRet = (DWORD)hWndApp;
			break;
		}
		break;
*/

    case APP_ACTIVE :

#ifdef _CPHS_DEBUG_
  StartObjectDebug();
#endif
        if(IsWindow(hFrameWin))
        {
            ShowWindow(hFrameWin,SW_SHOW);
            ShowOwnedPopups(hFrameWin,SW_SHOW);
            UpdateWindow(hFrameWin);
            SetFocus(hWndApp);                

        }
        else
        {
            RECT rClient;
            wc.style         = CS_OWNDC;
            wc.lpfnWndProc   = AppWndProc;
            wc.cbClsExtra    = 0;
            wc.cbWndExtra    = 0;
            wc.hInstance     = NULL;
            wc.hIcon         = NULL;
            wc.hCursor       = NULL;
            wc.hbrBackground = GetStockObject(WHITE_BRUSH);
            wc.lpszMenuName  = NULL;
            wc.lpszClassName = pClassName;

            if (!RegisterClass(&wc))
            {
                return FALSE;
            }

            hFrameWin = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);
            GetClientRect(hFrameWin, &rClient);
            
            hWndApp = CreateWindow(
                pClassName, 
                NULL, 
                WS_VISIBLE | WS_CHILD|WS_VSCROLL, 
                
                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin, 
                NULL,
                NULL, 
                NULL
                );
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, //set the left soft key
                MAKEWPARAM(IDC_BUTTON_SET,1), (LPARAM)ML("Call")) ;
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_EXIT,(LPARAM)ML("Back"));
            SetWindowText(hFrameWin,ML("Mailbox number"));

            ShowWindow(hFrameWin, SW_SHOW);
            UpdateWindow(hFrameWin);
            
            SetFocus(hWndApp);                
        }    
        break;

    case APP_INACTIVE :
		ShowOwnedPopups(hFrameWin, SW_HIDE);
        ShowWindow(hFrameWin,SW_HIDE); 
        break;
    }
    return dwRet;
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hVoiceMailOne,hVoiceMailTwo,hDataBox,hFaxBox;
    static HWND hFocus,hWndFocus;
    static int  iButtonJust;
    LRESULT lResult;
    lResult = TRUE;
    switch ( wMsgCmd )
    {
    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Call"));            
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0,(LPARAM)ML("Back"));
        SetWindowText(hFrameWin,ML("Mailbox number"));
        break;
	case PWM_ACTIVATE:
		if (wParam == WA_INACTIVE)
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
			wKeyCode   = 0;
		}
		break;
    case WM_CREATE:
		iInfoFlag = 0;
		memset(szNumber, 0, sizeof(szNumber));
        CreateControl(hWnd,&hVoiceMailOne,&hVoiceMailTwo,&hDataBox,&hFaxBox);
        CPHS_InitVScrolls(hWnd,4);
        if(GetMailboxNumInfo(hWnd, CALLBACK_INITSPACE) != -1)
		{
			WaitWin(hWnd, TRUE, ML("Communicating with SIM..."), ML("Mailbox number"), 0,
		        ML("Cancel"), CALLBACK_CANCEL);
		}
        hFocus = hVoiceMailOne;
        break;
	case CALLBACK_CANCEL:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_KILLFOCUS:
		KillTimer(hWnd, TIMER_ID);
		RepeatFlag = 0;
		wKeyCode   = 0;
		break;
    case CALLBACK_INITSPACE:
        if (!CPHS_Mailbox_init_space(hWnd, CALLBACK_GETRESULT))
		{
			PLXTipsWin(NULL, NULL, NULL, ML("No information available"), NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
			SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
        break;
    case CALLBACK_GETRESULT:
        GetMailboxNumResult(hWnd, CALLBACK_SAVEDATA1, NULL);
        break;
    case CALLBACK_SAVEDATA1:
        if (GetMailboxData(hWnd, 1, wParam) == 0)
			iInfoFlag = 1;
        break;
    case CALLBACK_SAVEDATA2:
        if (GetMailboxData(hWnd, 2, wParam) == 0 )
			iInfoFlag = 1;
        break;
    case CALLBACK_SAVEDATA3:
        if (GetMailboxData(hWnd, 3, wParam) == 0 )
			iInfoFlag = 1;
        break;
    case CALLBACK_SAVEDATA4:
        if (GetMailboxData(hWnd, 4, wParam) == 0 )
			iInfoFlag = 1;
		if (iInfoFlag == 1)
        {
			ShowMailboxNum(hVoiceMailOne,hVoiceMailTwo,hDataBox,hFaxBox);
			WaitWin(hWnd, FALSE, NULL, NULL, NULL,NULL, NULL); 
		}
		else
		{
			PLXTipsWin(NULL, NULL, NULL, ML("No information available"), NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
			SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
        break;
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;
    case WM_VSCROLL:
        CPHS_OnVScroll(hWnd,wParam);
        break;
    case WM_DESTROY : 
		WaitWin(hWnd, FALSE, NULL, NULL, NULL,NULL, NULL); 

        CPHS_Mailbox_free_space();
        hWndApp = NULL;
        UnregisterClass(pClassName,NULL);
		DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);

#ifdef _CPHS_DEBUG_
  EndObjectDebug();
#endif
  
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
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
		case VK_RETURN:
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
        switch( LOWORD( wParam ))
        {
		case IDC_BUTTON_SET://
            switch(iButtonJust)
            {
            case IDC_VOICEMAILONE:
				SendMessage(hVoiceMailOne, SSBM_GETTEXT, 0, (LPARAM)szNumber);
                break;
            case IDC_VOICEMAILTWO:
				SendMessage(hVoiceMailTwo, SSBM_GETTEXT, 0, (LPARAM)szNumber);
                break;
            case IDC_DATABOX:
				SendMessage(hDataBox, SSBM_GETTEXT, 0, (LPARAM)szNumber);
                break;
            case IDC_FAXBOX:
				SendMessage(hFaxBox, SSBM_GETTEXT, 0, (LPARAM)szNumber);
                break;
            }
			APP_CallPhoneNumber(szNumber);
            break;
		case IDC_BUTTON_EXIT://
            DestroyWindow(hFocus);
            DestroyWindow( hWnd );  
            break;
		case IDC_VOICEMAILONE:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
			{
				iButtonJust = IDC_VOICEMAILONE;
			}
            break;
		case IDC_VOICEMAILTWO:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
			{
				iButtonJust = IDC_VOICEMAILTWO;
			}
            break;
		case IDC_DATABOX:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
			{
				iButtonJust = IDC_DATABOX;
			}
            break;
		case IDC_FAXBOX:
			if(HIWORD(wParam) == SSBN_SETFOCUS)
			{
				iButtonJust = IDC_FAXBOX;
			}
            break;
        }
        break;
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
        }
    return lResult;

}
static BOOL CreateControl(HWND hwnd, HWND * hVoiceMailOne, HWND * hVoiceMailTwo, HWND * hDataBox, HWND * hFaxBox)
{
    int xzero=0,yzero=0;
    int iControlH,iControlW;
    RECT rect;
    GetClientRect(hwnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;

    * hVoiceMailOne = CreateWindow( "SPINBOXEX", ML("Line1 voice mailbox"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hwnd, (HMENU)IDC_VOICEMAILONE, NULL, NULL);

    if (* hVoiceMailOne == NULL)
        return FALSE;
    * hVoiceMailTwo = CreateWindow( "SPINBOXEX", ML("Line2 voice mailbox"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH, iControlW, iControlH, 
        hwnd, (HMENU)IDC_VOICEMAILTWO, NULL, NULL);

    if (* hVoiceMailTwo == NULL)
        return FALSE;
    * hDataBox = CreateWindow( "SPINBOXEX", ML("Data mailbox"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*2, iControlW, iControlH, 
        hwnd, (HMENU)IDC_DATABOX, NULL, NULL);

    if (* hDataBox == NULL)
        return FALSE;
    * hFaxBox = CreateWindow( "SPINBOXEX", ML("Fax mailbox"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*3, iControlW, iControlH, 
        hwnd, (HMENU)IDC_FAXBOX, NULL, NULL);

    if (* hFaxBox == NULL)
        return FALSE;
return TRUE;
}
/*********************************************************************\
* Function        CPHS_InitVScrolls
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
static void CPHS_InitVScrolls(HWND hWnd,int iItemNum)
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
* Function        CPHS_OnVScroll
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/

static void CPHS_OnVScroll(HWND hWnd,  UINT wParam)
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
		
        if((int)vsi.nPos > nCurFocus)	
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
