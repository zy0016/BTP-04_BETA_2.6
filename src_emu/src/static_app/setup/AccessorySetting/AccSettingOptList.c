#include    "AccSettingOptList.h"
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
ACCESSORY_TYPE Acc_Type;
static int iProfileFocus;
static int iAutoAnsFocus;
static char * pClassName = "AccessorySettingOptClass";
static char * pAutoAnsClassName = "AccessoryAutoSettingOptClass";

static LRESULT  AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  AutoAnsAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL     CreateControl (HWND hWnd, HWND * hProfileList);

extern void GetScreenUsableWH2 (int * pwidth,int * pheight);
extern	void Sett_SetActiveAccSM(ACCESSORY_TYPE AccType, int iProfileIndex);

void SaveAccessoryProfileInfo(ACCESSORY_TYPE AccType, int iProfileIndex);
void SaveAutoAnsState(ACCESSORY_TYPE AccType, SWITCHTYPE AutoAnsState);



BOOL CallAccProfileList(HWND hParentWnd, ACCESSORY_TYPE acc_type, int iIndex)
{
    WNDCLASS    wc;
    RECT rClient;
    Acc_Type = acc_type;
    hFrameWin = hParentWnd;
    iProfileFocus = iIndex;
    
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
        WS_VISIBLE | WS_CHILD , 
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
        (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );
    
    SetWindowText(hFrameWin, ML("Use profile"));
    SetFocus(hWndApp);
    
    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;

	HDC                 hdc;

	static  int     iIndex=0,i, nItemNum;
    static  HWND    hProfileList;
    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
    {

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
            (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
    
        SetWindowText(hFrameWin, ML("Use profile"));
        break;
   
    
    case WM_CREATE :
        CreateControl (hWnd, &hProfileList);
        
		hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);
        nItemNum = GetUserProfileNo() + FIXEDPROFILENUMBER + 1;
		Load_Icon_SetupList(hProfileList,hIconNormal,hIconSel,nItemNum,iProfileFocus);
		SendMessage(hProfileList,LB_SETCURSEL,iProfileFocus,0);
  
		break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_PROFILELIST));
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
            break;

        }
        break;  

   case WM_COMMAND :
       switch (LOWORD(wParam))
        {
        case IDM_OK:   
             iIndex = SendMessage(hProfileList,LB_GETCURSEL,0,0);              //get the current selected item number
			 Load_Icon_SetupList(hProfileList,hIconNormal,hIconSel,nItemNum,iIndex);     //change the radio button of the current selected item		
             SaveAccessoryProfileInfo(Acc_Type, iIndex-1);
		 	 Sett_SetActiveAccSM(Acc_Type, iIndex-1);
	    	 SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
             break;
        }
        break;

    case WM_DESTROY :
	     DeleteObject(hIconNormal);
	     DeleteObject(hIconSel);
		 KillTimer(hWnd, TIMER_ASURE);
         hWndApp = NULL;
         UnregisterClass(pClassName,NULL);
         break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
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
static BOOL  CreateControl (HWND hWnd, HWND * hProfileList)
{
	int iscreenw,iscreenh;
    SCENEMODE sm;
    int nItemNum, i;
	RECT rect;
	GetClientRect(hWnd, &rect);
    nItemNum = GetUserProfileNo() + FIXEDPROFILENUMBER + 1;
    GetScreenUsableWH2(&iscreenw,&iscreenh);

    

    * hProfileList = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL/*|CS_NOSYSCTRL*/,   
        rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
        hWnd, (HMENU)IDC_PROFILELIST, NULL, NULL);
    if (* hProfileList == NULL )
        return FALSE;

    SendMessage(*hProfileList, LB_ADDSTRING, 0, (LPARAM)ML("Keep profile"));
    for(i = 0; i < nItemNum - 1; i++)
    {
		if (i==0)
			SendMessage(* hProfileList, LB_ADDSTRING, 0, (LPARAM)ML("Normal"));
		else if (i==1)
			SendMessage(* hProfileList, LB_ADDSTRING, 0, (LPARAM)ML("Silent"));
		else if (i==2)
			SendMessage(* hProfileList, LB_ADDSTRING, 0, (LPARAM)ML("Noisy"));
		else if (i==3)
			SendMessage(* hProfileList, LB_ADDSTRING, 0, (LPARAM)ML("Discreet"));
		else
		{
			GetSM(&sm, i);
			SendMessage(*hProfileList, LB_ADDSTRING, 0, (LPARAM)sm.cModeName);
		}
    }
    return TRUE;
    
}


BOOL CallAccAutoAnsList(HWND hParentWnd, ACCESSORY_TYPE acc_type, int iIndex)
{
    WNDCLASS    wc;
    RECT rClient;
    Acc_Type = acc_type;
    hFrameWin = hParentWnd;
    iAutoAnsFocus = iIndex;
    
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AutoAnsAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pAutoAnsClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);

    hWndApp = CreateWindow(pAutoAnsClassName,NULL, 
        WS_VISIBLE | WS_CHILD , 
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hWndApp)
    {
        UnregisterClass(pAutoAnsClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
        (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );
    
    SetWindowText(hFrameWin, ML("Automatic answer"));
    SetFocus(hWndApp);
    
    return (TRUE);
}
static LRESULT  AutoAnsAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;

	HDC                 hdc;
    static  HWND    hList;
    static int iIndex;
    LRESULT lResult;
	RECT rect;

    lResult = TRUE;   

    switch ( wMsgCmd )
    {

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
            (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
    
        SetWindowText(hFrameWin, ML("Automatic answer"));
        break;
   
    
    case WM_CREATE :
		GetClientRect(hWnd, &rect);
        hList = CreateWindow("LISTBOX", 0, 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL/*|CS_NOSYSCTRL*/,   
			rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
            hWnd, (HMENU)IDC_LIST, NULL, NULL);
        if (hList == NULL )
            return FALSE;
        
        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ML("On"));
        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ML("Off"));
        
		hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);

        Load_Icon_SetupList(hList,hIconNormal,hIconSel,2,iAutoAnsFocus);
		SendMessage(hList,LB_SETCURSEL,iAutoAnsFocus,0);
  
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
            SendMessage(hWnd,WM_COMMAND,IDM_OK,0);
             break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;  

   case WM_COMMAND :
       switch (LOWORD(wParam))
        {
        case IDM_OK:   
             iIndex = SendMessage(hList,LB_GETCURSEL,0,0);              //get the current selected item number
			 Load_Icon_SetupList(hList,hIconNormal,hIconSel,2,iIndex);     //change the radio button of the current selected item		
             SaveAutoAnsState(Acc_Type, iIndex);
	    	 SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
             break;
        }
        break;

    case WM_DESTROY :
	     DeleteObject(hIconNormal);
	     DeleteObject(hIconSel);
		 KillTimer(hWnd, TIMER_ASURE);
         hWndApp = NULL;
         UnregisterClass(pAutoAnsClassName,NULL);
         break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
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
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    } 
    return lResult;    
}
