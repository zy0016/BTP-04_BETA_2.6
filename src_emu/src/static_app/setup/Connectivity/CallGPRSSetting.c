/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : connectivity setting
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
#include    "CallGPRSSetting.h"
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
#include	"imesys.h"

static char* pClassName = "GPRSSettingClass";
static char* pAttClassName = "GPRSAttClass";
static char* pAccClassName = "AccPointClass";
static HWND hAppWnd = NULL;
static HWND hAttAppWnd = NULL;
static HWND hAccAppWnd = NULL;
static HWND hFrameWin = NULL;
static char cCurMode[UDB_K_NAME_SIZE+1] = "";
static int iIndex_GPRSAtt = 0;
static int iIndex_AccPoint = 0;

//for continuous scroll;
static unsigned long  RepeatFlag = 0; 
static WORD wKeyCode;

static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  AttAppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  AccAppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

static BOOL CreateControl(HWND hWnd, HWND * hGPRSAttach, HWND * hAccessPoint);
static BOOL AttCreateControl(HWND hWnd, HWND *hList);
static BOOL AccCreateControl(HWND hWnd, HWND *hwndEdit);

static BOOL CallGPRSAttachSetting(HWND hWnd);
static BOOL CallAccessPointSetting(HWND hWnd);

extern int ME_SetGPRSAttach(HWND hWnd, int nMsg, int AttachSta);
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);

BOOL    CallGPRSSetting(HWND hWnd)
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

    hFrameWin = hWnd;
    
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
        
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_BACK, (LPARAM)ML("Back"));//RSK
    SetWindowText(hFrameWin, ML("GPRS"));

    GetClientRect(hFrameWin, &rect);

    hAppWnd = CreateWindow(pClassName,NULL, 
        WS_VISIBLE| WS_CHILD,  
        rect.left,
        rect.top,
        rect.right-rect.left,
        rect.bottom-rect.top,
        hFrameWin, NULL, NULL, NULL);    

    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    SetFocus ( hAppWnd );	
    
    return TRUE;
    
}

static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hGPRSAttach, hAccessPoint;
    static HWND hFocus;
    static int  iButtonJust, iCurIndex;
    
    LRESULT     lResult;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:
        CreateControl(hWnd, &hGPRSAttach, &hAccessPoint );
        SendMessage(hGPRSAttach, SSBM_SETCURSEL, iIndex_GPRSAtt, 0);
        memset(cCurMode, 0, sizeof(cCurMode));
        FS_GetPrivateProfileString(SN_GPRSSETT, KN_GPRSACCID, "", cCurMode, sizeof(cCurMode), SETUPFILENAME);
    	SendMessage(hAccessPoint, SSBM_ADDSTRING, 0, (LPARAM)cCurMode);
        hFocus = hGPRSAttach;
        break;
        
    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_BACK, (LPARAM)ML("Back"));//RSK
        SetWindowText(hFrameWin, ML("GPRS"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        SendMessage(hGPRSAttach, SSBM_SETCURSEL, iIndex_GPRSAtt, 0);
        FS_GetPrivateProfileString(SN_GPRSSETT, KN_GPRSACCID, "", cCurMode, sizeof(cCurMode), SETUPFILENAME);
        SendMessage(hAccessPoint, SSBM_DELETESTRING, 0, 0);
        SendMessage(hAccessPoint, SSBM_ADDSTRING, 0, (LPARAM)cCurMode);
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
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

	case PWM_ACTIVATE:
		if (wParam == WA_INACTIVE)
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
			wKeyCode   = 0;
		}
		break;

	case WM_COMMAND:

        switch(LOWORD( wParam )) 
        {
        case IDC_BUTTON_SEL:
            switch(iButtonJust)
            {
            case IDC_GPRSATTACH:
                CallGPRSAttachSetting(hFrameWin);
                break;
            case IDC_ACCESSPOINT:
                CallAccessPointSetting(hFrameWin);
                break;
            }
            break;
        case IDC_GPRSATTACH:
            if(HIWORD( wParam ) == SSBN_CHANGE)
            {
               iIndex_GPRSAtt = SendMessage(hGPRSAttach,SSBM_GETCURSEL,0,0);
            }
            if(HIWORD(wParam) == SSBN_SETFOCUS)
            {
                iButtonJust = IDC_GPRSATTACH;
                hFocus = hGPRSAttach;
            }
            break;
        case IDC_ACCESSPOINT:
            if(HIWORD( wParam ) == SSBN_CHANGE)
            {
               iIndex_AccPoint = SendMessage(hGPRSAttach,SSBM_GETCURSEL,0,0);
            }
            if(HIWORD(wParam) == SSBN_SETFOCUS)
            {
                iButtonJust = IDC_ACCESSPOINT;
                hFocus = hAccessPoint;
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

static BOOL CreateControl(HWND hWnd, HWND * hGPRSAttach, HWND * hAccessPoint)
{
    int iControlH,iControlW;
	int xzero=0,yzero=0;

	RECT    rect;
    GetClientRect(hWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;
    
    * hGPRSAttach = CreateWindow( "SPINBOXEX", ML("GPRS attach"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hWnd, (HMENU)IDC_GPRSATTACH, NULL, NULL);

    if (* hGPRSAttach == NULL)
        return FALSE;  

    SendMessage(* hGPRSAttach, SSBM_ADDSTRING, 0, (LPARAM)ML("Always"));
    SendMessage(* hGPRSAttach, SSBM_ADDSTRING, 0, (LPARAM)ML("On demand"));

    * hAccessPoint = CreateWindow( "SPINBOXEX", ML("Access point"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST  | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        xzero, yzero + iControlH, iControlW, iControlH, 
        hWnd, (HMENU)IDC_ACCESSPOINT, NULL, NULL);

    if (* hAccessPoint == NULL)
        return FALSE;  
   
return TRUE;
    
}

static BOOL CallGPRSAttachSetting(HWND hWnd)
{
	WNDCLASS wc;
    RECT rect;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AttAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pAttClassName;

    hFrameWin = hWnd;
    
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
        
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_BACK, (LPARAM)ML("Cancel"));//RSK
    SetWindowText(hFrameWin, ML("GPRS attach"));

    GetClientRect(hFrameWin, &rect);

    hAttAppWnd = CreateWindow(pAttClassName,NULL, 
        WS_VISIBLE| WS_CHILD,  
        rect.left,
        rect.top,
        rect.right-rect.left,
        rect.bottom-rect.top,
        hFrameWin, NULL, NULL, NULL);    

    if (NULL == hAttAppWnd)
    {
        UnregisterClass(pAttClassName,NULL);
        return FALSE;
    }
    SetFocus ( hAttAppWnd );	
    
    return TRUE;

}

static LRESULT  AttAppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;

	HDC                 hdc;

	static  int     iIndex=0;
    static  HWND    hList;
    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
    {

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_BACK, (LPARAM)ML("Cancel"));//RSK
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        SetWindowText(hFrameWin, ML("GPRS attach"));
        break;
   
    
    case WM_CREATE :
        AttCreateControl(hWnd, &hList);

		hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);
        
        iIndex = iIndex_GPRSAtt;
		Load_Icon_SetupList(hList,hIconNormal,hIconSel,2,iIndex);
		SendMessage(hList,LB_SETCURSEL,iIndex,0);
  
		break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_LIST));
        break;
        
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
       
		case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SEL,0);
             break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;  

   case WM_COMMAND :
       switch (LOWORD(wParam))
        {
        case IDC_BUTTON_SEL:   
             iIndex = SendMessage(hList,LB_GETCURSEL,0,0);              //get the current selected item number
			 Load_Icon_SetupList(hList,hIconNormal,hIconSel,2,iIndex);     //change the radio button of the current selected item		
	    	 iIndex_GPRSAtt = iIndex;

             if ( 0 == iIndex_GPRSAtt ) //always
                ME_SetGPRSAttach(hWnd, CALLBACK_SETGPRS, 1);
             else if ( 1 == iIndex_GPRSAtt) //when need
                ME_SetGPRSAttach(hWnd, CALLBACK_SETGPRS, 0);
            
             SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
             
             break;
        }
        break;
    case WM_DESTROY : //
        DeleteObject(hIconNormal);
        DeleteObject(hIconSel);
        hAttAppWnd = NULL;
        UnregisterClass(pAttClassName,NULL);
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    
    case CALLBACK_SETGPRS:
        if ( ME_RS_SUCCESS == wParam )
        {
            FS_WritePrivateProfileInt(SN_GPRSSETT, KN_GPRSATTID, iIndex_GPRSAtt, SETUPFILENAME);
        }
        break;
        
	case WM_TIMER:
	    switch(wParam)
		{
		case TIMER_ASURE:
            KillTimer(hWnd, TIMER_ASURE);
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    } 
    return lResult;
}

static BOOL AttCreateControl(HWND hWnd, HWND *hList)
{
    RECT rect;
    GetClientRect(hWnd, &rect);

    * hList = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0,0,rect.right - rect.left,rect.bottom - rect.top,
        hWnd, (HMENU)IDC_LIST, NULL, NULL);
    if (* hList == NULL )
        return FALSE;
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("Always"));
    SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML("On demand"));
   return TRUE;
}



static BOOL CallAccessPointSetting(HWND hWnd)
{
    WNDCLASS wc;
    RECT rClient;

    hFrameWin = hWnd;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AccAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pAccClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);

    hAccAppWnd = CreateWindow(pAccClassName, NULL, 
        WS_VISIBLE | WS_CHILD, 
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hAccAppWnd)
    {
        UnregisterClass(pAccClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON,//LSK
            MAKEWPARAM((WORD)1, (WORD)IDC_BUTTON_SAVE), (LPARAM)ML("Save"));
        
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
            IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));//RSK  
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    SetWindowText(hFrameWin, ML("GPRS"));
    SetFocus(hAccAppWnd);

    return TRUE;
}


static LRESULT  AccAppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT     lResult;    
    static HWND hFocus,hwndEdit;//show the editor of scenemode name

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
	case WM_CREATE:             
        AccCreateControl(hWnd,&hwndEdit);
        hFocus = hwndEdit;
        break;
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;
    case WM_DESTROY ://
         hAccAppWnd = NULL;
         UnregisterClass(pAccClassName,NULL);
         break;

    case PWM_SHOWWINDOW:
        SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON,//LSK
            MAKEWPARAM((WORD)1, (WORD)IDC_BUTTON_SAVE), (LPARAM)ML("Save"));
        
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
            IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));//RSK  
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif

        SetWindowText(hFrameWin, ML("GPRS"));
        SetFocus(hWnd);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {            
        case VK_F10:
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
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_SAVE: 
            GetWindowText(hwndEdit,cCurMode,sizeof(cCurMode));
			Sett_SpecialStrProc(cCurMode);
//			if ( 0 == strlen(cCurMode) )
//			{
//				PLXTipsWin(NULL, NULL, NULL,ML("Please definesnname"), NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
//				break;				
//			}
            FS_WritePrivateProfileString(SN_GPRSSETT, KN_GPRSACCID, cCurMode, SETUPFILENAME);
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        break;
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
	}
return lResult;
}
static BOOL		AccCreateControl(HWND hWnd, HWND *hwndEdit)
{
   IMEEDIT ie;
   RECT rect;
   GetClientRect(hWnd, &rect);
   
   memset((void*)&ie, 0, sizeof(IMEEDIT));
   ie.hwndNotify   = hWnd;
   FS_GetPrivateProfileString(SN_GPRSSETT, KN_GPRSACCID, "", cCurMode, sizeof(cCurMode), SETUPFILENAME);
   * hwndEdit = CreateWindow(
        "IMEEDIT",
        cCurMode,
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL | ES_TITLE,
        0,
        0,
        rect.right - rect.left,
        (rect.bottom - rect.top) / 3,
        hWnd,
        (HMENU)ID_EDIT,
        NULL,
        &ie);
         
   SendMessage(* hwndEdit, EM_LIMITTEXT, UDB_K_NAME_SIZE, 0);
   
   SendMessage(* hwndEdit, EM_SETTITLE, 0, (LPARAM)ML("Access pointcolon"));
   SendMessage(* hwndEdit, EM_SETSEL, -1, -1);
   return TRUE;
}
