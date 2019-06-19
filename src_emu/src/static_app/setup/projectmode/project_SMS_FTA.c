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
#include "string.h"
#include "plx_pdaex.h"

#define IDC_SMSMODELIST	0x301

#define PROJECT_SMSFTAWNDCLASS  "Project_SMSFTAWndClass"

static	LRESULT  Project_SMS_FTAWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

extern  void	SMS_SetMode(BOOL bFTAMode);
extern  BOOL	SMS_GetMode(void);

extern	HWND	GetProjectModeFrameWnd();
extern	int		GetSIMState();

static  HWND		hProjectModeFrame  = NULL;

BOOL    CallProjectSMSFTAWindow(void * iFTA)
{
    HWND hwnd;
    WNDCLASS wc;
	RECT rClient;
	
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = Project_SMS_FTAWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;	
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = PROJECT_SMSFTAWNDCLASS;
    
    if( !RegisterClass(&wc) )
    {
        return FALSE;
    }
	
	hProjectModeFrame = GetProjectModeFrameWnd();
	GetClientRect(hProjectModeFrame,&rClient);
	
    hwnd = CreateWindow(
        PROJECT_SMSFTAWNDCLASS,
        ML("SMS MODE"), 
        WS_VISIBLE | WS_CHILD, 
        rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top, 
        hProjectModeFrame, NULL, NULL, (PVOID)iFTA);
	
    if (!hwnd)
        return FALSE;
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd); 
	
    return TRUE;
}

static LRESULT  Project_SMS_FTAWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hProjectModeFrame;
	HDC hdc;
	static HWND hList;
	static HBITMAP hRadioOn,hRadioOff;
	static int iSelected;
	
	hProjectModeFrame=GetProjectModeFrameWnd();
	
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			int i;
			PCREATESTRUCT pCreateStruct = (PCREATESTRUCT)lParam;

			memcpy(&iSelected,pCreateStruct->lpCreateParams,sizeof(int));

			GetClientRect(hWnd,&rClient);
			
			hList = CreateWindow("LISTBOX","",
				WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY | LBS_BITMAP,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)IDC_SMSMODELIST,NULL,NULL);
			
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("FTA MODE"));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("NORMAL MODE"));
						
			hdc=GetDC(hWnd);
			hRadioOn=CreateBitmapFromImageFile(hdc,"/rom/ime/radioon.bmp",NULL,NULL);
			hRadioOff=CreateBitmapFromImageFile(hdc,"/rom/ime/radiooff.bmp",NULL,NULL);
			ReleaseDC(hWnd,hdc);

			for(i=0;i<2;i++)
			{
				SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hRadioOff);
			}
			
				SendMessage(hList,LB_SETCURSEL,iSelected,0);
			
			SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)iSelected),(LPARAM)hRadioOn);
			SendMessage(hList,LB_SETCURSEL,iSelected,0);
			
			SendMessage(hProjectModeFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			SendMessage(hProjectModeFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hProjectModeFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
			SetWindowText(hProjectModeFrame,ML("SMS MODE"));

			SetFocus(hList);
		
		}
        break;
		
	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		break;
		
    case WM_SETFOCUS:
        SetFocus(hList);
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hProjectModeFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hProjectModeFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hProjectModeFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
		SetWindowText(hProjectModeFrame,ML("SMS MODE"));
		SetFocus(hList);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass(PROJECT_SMSFTAWNDCLASS,NULL);
		DeleteObject(hRadioOn);
		DeleteObject(hRadioOff);
        break;
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hProjectModeFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;	
			
		case VK_F5:
			{
				int i;
				int bCur;
				bCur = SendMessage(hList,LB_GETCURSEL,0,0);

				if ( bCur == 0 )
				{
					SendMessage(hList,LB_SETCURSEL,bCur,0);
					for ( i = 0; i < 2; i++ )
					{
						SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hRadioOff);
					}
					SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)bCur),(LPARAM)hRadioOn);
					SMS_SetMode(TRUE);
				}
				else
				{
					SendMessage(hList,LB_SETCURSEL,bCur,0);
					for ( i = 0; i < 2; i++ )
					{
						SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hRadioOff);
					}
					SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)bCur),(LPARAM)hRadioOn);
					SMS_SetMode(FALSE);
				}

				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hProjectModeFrame,PWM_CLOSEWINDOW,  (WPARAM)hWnd, (LPARAM)0);
			}
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);			
			break;
		}
		break;
		
	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
    return lResult;
}
