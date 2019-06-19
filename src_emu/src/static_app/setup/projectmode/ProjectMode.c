/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#include "window.h"
#include "plx_pdaex.h"
#include "string.h"
#include "pubapp.h"

#define  IDC_CHECK		0x200
#define  IDC_SELECT		0x201
#define  IDC_EXIT       0x202

#define  IDC_ATCOMMAND  0x301
#define  IDC_MEMORYINFO 0x302
#define  IDC_SMSFTA		0x303

#define PROJECTMODEWNDCLASS "ProjectModeWndClass"

static HWND hProjectMode = NULL;
static HWND hProjectModeMain = NULL;
static HINSTANCE hProjectModeInstance = NULL;

static int  nCurFocus;

static BOOL	   CreateControl(HWND hParentWnd,HWND *hATCommand,HWND *hMemoryInfo,HWND *hSMSFTA);
static LRESULT ProjectModeWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void	   InitVScrollBar(HWND hWnd,int iItemNum);
static void    DoVScroll(HWND hWnd,WPARAM wParam);

extern int	   GetSIMState(void);
extern void	   SMS_SetMode(BOOL bFTAMode);
extern BOOL	   SMS_GetMode(void);

extern BOOL	   CallProjectMemoryWindow(void);
extern BOOL    CallProjectATWindow(void);
extern BOOL    CallProjectSMSFTAWindow(int * j);

//*********************************************************
//程序的入口函数;
DWORD   ProjectMode_AppControl (int nCode, void* pInstance, WPARAM wParam, LPARAM lParam )
{
	WNDCLASS wc;
	RECT rClient;
    DWORD dwRet = TRUE;
	
    switch (nCode)
    {
    case APP_INIT :
        hProjectModeInstance = pInstance;
        break;

	case APP_GETOPTION:
		switch(wParam)
		{
		case AS_APPWND:
			dwRet = (DWORD)hProjectModeInstance;
			break;
		}
		break;
		
    case APP_ACTIVE :
		if(IsWindow(hProjectMode))
		{	
			ShowWindow(hProjectMode, SW_SHOW);
			ShowOwnedPopups(hProjectMode, SW_SHOW);
			UpdateWindow(hProjectMode);
		}
        else
		{
			wc.style         = 0; 
			wc.lpfnWndProc   = ProjectModeWndProc;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = NULL;
			wc.hIcon         = NULL;
			wc.hCursor       = NULL;
			wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
			wc.lpszMenuName  = NULL;
			wc.lpszClassName = PROJECTMODEWNDCLASS;
			
			if(!RegisterClass(&wc))
				return FALSE;
			
			hProjectMode = CreateFrameWindow( WS_CAPTION |PWS_STATICBAR /*|WS_VSCROLL*/);
			GetClientRect(hProjectMode,&rClient);
			
			hProjectModeMain = CreateWindow(
                PROJECTMODEWNDCLASS,
                "",
                WS_VISIBLE | WS_CHILD |WS_VSCROLL,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
                hProjectMode,
                NULL,
                NULL,
                NULL
                );
			
			ShowWindow(hProjectMode, SW_SHOW); 
			UpdateWindow(hProjectMode);  
			SetFocus(hProjectModeMain); 
			
		}
        break;
		
    case APP_INACTIVE :
		ShowOwnedPopups(hProjectMode, SW_HIDE);
        ShowWindow(hProjectMode,SW_HIDE); 
        break;
		
    default :
        break;
    }
    return dwRet;
}

BOOL CallProjectModeWindow (HWND hwndCall)
{
    WNDCLASS    wc;
    BOOL        bRet = TRUE;
	RECT		rClient;

	if(IsWindow(hProjectMode))
	{	
		ShowWindow(hProjectMode, SW_SHOW);
		ShowOwnedPopups(hProjectMode, SW_SHOW);
		UpdateWindow(hProjectMode);
	}
	else
	{
		wc.style         = 0; 
		wc.lpfnWndProc   = ProjectModeWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = PROJECTMODEWNDCLASS;
		
		if(!RegisterClass(&wc))
			return FALSE;
		
		hProjectMode = CreateFrameWindow( WS_CAPTION |PWS_STATICBAR /*|WS_VSCROLL*/);
		GetClientRect(hProjectMode,&rClient);
		
		hProjectModeMain = CreateWindow(
			PROJECTMODEWNDCLASS,
			"",
			WS_VISIBLE | WS_CHILD |WS_VSCROLL,
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
			hProjectMode,
			NULL,
			NULL,
			NULL
			);
	
		if (!hProjectModeMain)
			return FALSE;

	
		ShowWindow(hProjectModeMain, SW_SHOW);
		UpdateWindow(hProjectModeMain); 
		SetFocus(hProjectModeMain);
	}	
    return (bRet);
}

static LRESULT ProjectModeWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static HWND hATCommand,hMemoryInfo,hSMSFTA;
	static HWND hFocus;
	static int request;
		   LRESULT lResult;
		   HWND hWndFocus;
		   int i;
 
    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			int  iItemNum = 3;
			
			SendMessage(hProjectMode,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_EXIT,0),(LPARAM)ML("EXIT"));
			SendMessage(hProjectMode,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			SetWindowText(hProjectMode,ML("Project Mode"));
			CreateControl(hWnd,&hATCommand,&hMemoryInfo,&hSMSFTA);
			InitVScrollBar(hWnd,iItemNum);
			
			if ( SMS_GetMode() )
				SendMessage(hSMSFTA,SSBM_SETCURSEL,0,0);
			else
				SendMessage(hSMSFTA,SSBM_SETCURSEL,1,0);
			
			hFocus = hATCommand;
		}
        break;

	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		break;
		
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hProjectMode,PWM_SETBUTTONTEXT,0,(LPARAM)ML("EXIT"));
		SendMessage(hProjectMode, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select"); 
		SetWindowText(hProjectMode,(PCSTR)ML("Project Mode"));

		if ( SMS_GetMode() )
			SendMessage(hSMSFTA,SSBM_SETCURSEL,0,0);
		else
			SendMessage(hSMSFTA,SSBM_SETCURSEL,1,0);

		SetFocus(hFocus);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd); 
        break;

    case WM_DESTROY :
        UnregisterClass(PROJECTMODEWNDCLASS,NULL);
//        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hProjectModeInstance);
        break;

	case WM_VSCROLL:
		DoVScroll(NULL,wParam);
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hProjectMode, PWM_CLOSEWINDOW, (WPARAM)hWnd, (LPARAM)0);
			break;	

		case VK_F5:
			{
				if(hFocus==hATCommand)
				{
					if(!GetSIMState())
					{
						PLXTipsWin(NULL,NULL,0,ML("Insert SIM card"),ML("AT Command"),Notify_Alert,ML("Ok"),NULL,20);
					}
					CallProjectATWindow();
				}

				else if(hFocus==hMemoryInfo)
				{
					CallProjectMemoryWindow();
				}

				else if(hFocus==hSMSFTA)
				{
					i = SendMessage( hSMSFTA, SSBM_GETCURSEL, 0, 0 );
					CallProjectSMSFTAWindow(&i);
				}
			}
			break;
			
		case VK_DOWN:
			{
				hWndFocus= GetFocus();
				
				while(GetParent(hWndFocus) != hWnd)
				{
					hWndFocus = GetParent(hWndFocus);
				}
				
				hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, FALSE);

				SetFocus(hWndFocus);
				hFocus=hWndFocus;

				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, (LPARAM)NULL);
			}
			break;
			
		case VK_UP:
			{
				hWndFocus = GetFocus();
				
				while(GetParent(hWndFocus) != hWnd)
				{
					hWndFocus = GetParent(hWndFocus);
				}
				
				hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, TRUE);
				SetFocus(hWndFocus);
				hFocus=hWndFocus;

				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, (LPARAM)NULL);
			}
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		}
		break;      // end keydown  

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
		case IDC_SMSFTA:
			if(HIWORD( wParam ) == SSBN_CHANGE)
			{
				if ( SendMessage(hSMSFTA,SSBM_GETCURSEL,0,0) )
					SMS_SetMode(TRUE);
				else
					SMS_SetMode(FALSE);
			}
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


static void InitVScrollBar(HWND hWnd,int iItemNum)
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

static void DoVScroll(HWND hWnd,WPARAM wParam)
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

static BOOL CreateControl(HWND hParentWnd,HWND *hATCommand,HWND *hMemoryInfo,HWND *hSMSFTA)
{
	int     xzero=0,yzero=0,iControlH,iControlW;
	RECT    rect;

    GetClientRect(hParentWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;
	
    * hATCommand= CreateWindow( "SPINBOXEX", ML("At Command"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_ATCOMMAND, NULL, NULL);
	
    if (* hATCommand == NULL)
        return FALSE;
	
    * hMemoryInfo = CreateWindow( "SPINBOXEX", ML("Memory Information"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_MEMORYINFO, NULL, NULL);
	
    if (* hMemoryInfo == NULL)
        return FALSE;

	* hSMSFTA = CreateWindow( "SPINBOXEX", ML("SMS MODE"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+2*iControlH, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_SMSFTA, NULL, NULL);
	
    if (* hSMSFTA == NULL)
        return FALSE;
	
	if ( SMS_GetMode() )
		SendMessage(* hSMSFTA,SSBM_SETCURSEL,0,0);
	else
		SendMessage(* hSMSFTA,SSBM_SETCURSEL,1,0);
	
	SendMessage(* hSMSFTA,SSBM_ADDSTRING,0,(LPARAM)ML("FTA MODE"));
	SendMessage(* hSMSFTA,SSBM_ADDSTRING,0,(LPARAM)ML("NORMAL MODE"));

	return TRUE;   
}

HWND GetProjectModeFrameWnd()
{
	return hProjectMode;
}
