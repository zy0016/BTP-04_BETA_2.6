/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : sound profile
 *
 * Purpose  : call window for create new profile
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "callcreatenewmode.h"
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

static HWND hWndApp = NULL;
static HWND hFrameWin = NULL;
static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd,HWND * hListContent);

extern  BOOL CallProfileList(HWND hwndCall, HWND hFather,int icurmode);


/***************************************************************
* Function  CallCreateNewMode
* Purpose           
* Params
* Return    
* Remarks
***************************************************************/
BOOL CallCreateNewMode(HWND hwndCall)
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
        WS_VISIBLE | WS_CHILD,          
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
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,IDM_FILEEXIT, (LPARAM)ML("Cancel"));        

    SetWindowText(hFrameWin, ML("Base profile"));
    SetFocus(hWndApp);
    return (TRUE);
}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;
	HDC             hdc;
    SCENEMODE       sm;
	static  int     iIndex=0,iNewIndex,i,iUserNum,iLastProfileIndex,iProfileNum;
    static  HWND    hListContent,hFocus;
//    int iTmp, iTmpEX;
//    DWORD dword;
    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
	{
		case WM_CREATE :
            CreateControl(hWnd,&hListContent);
            
            hdc         = GetDC(hWnd);
            hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
            hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
            ReleaseDC(hWnd,hdc);		
            
            iUserNum = GetUserProfileNo();
            iProfileNum = FIXEDPROFILENUMBER + iUserNum;
            
            SendMessage(hListContent,LB_RESETCONTENT,0,0);
            
            for(i=0;i<iProfileNum;i++)
            {
				if (i==0)
					SendMessage(hListContent,LB_ADDSTRING,0,(LPARAM)ML("Normal"));
				else if (i==1)
					SendMessage(hListContent,LB_ADDSTRING,0,(LPARAM)ML("Silent"));
				else if (i==2)
					SendMessage(hListContent,LB_ADDSTRING,0,(LPARAM)ML("Noisy"));
				else if (i==3)
					SendMessage(hListContent,LB_ADDSTRING,0,(LPARAM)ML("Discreet"));
				else
                {
					GetSM(&sm,i);
					SendMessage(hListContent,LB_ADDSTRING,0,(LPARAM)sm.cModeName);
				}
            }
            SendMessage(hListContent,LB_SETCURSEL,0,0);
            Load_Icon_SetupList(hListContent,hIconNormal,hIconSel,iProfileNum,0);
            
/*
            iTmp = GetUserProfileNo();
            iTmp += FIXEDPROFILENUMBER;
            iTmpEX = SendMessage(hListContent, LB_GETCOUNT, 0, 0);
            if(iTmp - iTmpEX == 1)
            {
                GetSM(&sm, iTmp - 1); //
                SendMessage(hListContent, LB_ADDSTRING, 0, (LPARAM)sm.cModeName);
		        dword = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)iTmpEX);
		        SendMessage(hListContent,LB_SETIMAGE,(WPARAM)dword, (LPARAM)hIconNormal);
            }
 */
           
            break;
            
        case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_MODELIST));
            break;
            
        case PWM_SHOWWINDOW:
            SetFocus(hWnd);
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,IDM_FILEEXIT, (LPARAM)ML("Cancel"));       
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            
            SetWindowText(hFrameWin, ML("Base profile"));
/*
            iTmp = GetUserProfileNo();
            iTmp += FIXEDPROFILENUMBER;
            iTmpEX = SendMessage(hListContent, LB_GETCOUNT, 0, 0);
            if(iTmp - iTmpEX == 1)
            {
                GetSM(&sm, iTmp - 1); //
                SendMessage(hListContent, LB_ADDSTRING, 0, (LPARAM)sm.cModeName);
                dword = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)iTmpEX);
                SendMessage(hListContent,LB_SETIMAGE,(WPARAM)dword, (LPARAM)hIconNormal);
                
            }
*/
            break;
            
        case WM_KEYDOWN:
            switch(LOWORD(wParam))
            {
                
            case VK_F10:
                SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                PostMessage(hWnd, WM_CLOSE, 0, 0);
                break;
            case VK_F5:
                SendMessage(hWnd,WM_COMMAND,IDM_OK,0);
                break;
            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            }
            break;  
            
        case WM_DESTROY : 
            hWndApp = NULL;
            DeleteObject(hIconNormal);
            DeleteObject(hIconSel);
            KillTimer(hWnd, TIMER_ASURE);
            UnregisterClass(pClassName,NULL);
            break;
                
        case WM_COMMAND :          
            switch (LOWORD(wParam))
            {
            case IDM_OK:   
                iIndex = SendMessage(hListContent,LB_GETCURSEL,0,0);              //get the current selected item number
                Load_Icon_SetupList(hListContent,hIconNormal,hIconSel,iProfileNum,iIndex);     //change the radio button of the current selected item		
                GetSM(&sm,iIndex);
                AddSM(sm);
                SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
                break;
            }
            break;
            
        case WM_TIMER:
            switch(wParam)
            {
            case TIMER_ASURE:
                KillTimer(hWnd, TIMER_ASURE);
                iNewIndex = FIXEDPROFILENUMBER + GetUserProfileNo() - 1;
                CallProfileList(hFrameWin,hWnd, iNewIndex);
                //			DestroyWindow(hWnd);
                break;
            }
            break;
        default :
            lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
            break;
    }
    return lResult;
}

static  BOOL    CreateControl(HWND hWnd,HWND * hListContent)
{
	RECT rect;
	GetClientRect(hWnd, &rect);

    * hListContent = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
        hWnd, (HMENU)IDC_MODELIST, NULL, NULL);
    if (* hListContent == NULL )
        return FALSE;
   return TRUE;
}
