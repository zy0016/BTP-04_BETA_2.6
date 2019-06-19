#include "log_main.h"

#define LOG_LEFT_ICON "/rom/public/arrow_left.ico"
#define LOG_RIGHT_ICON "/rom/public/arrow_right.ico"

#define MBPIC_RECORD_GPRSCOUNTER2 "/rom/network/traffic_22x16.bmp"

static LOGIFTYPE   iftype;
static HWND hPWnd;
static HWND hCounterWnd;
static const char * pClassName = "CountersWndClass";
//static HANDLE       hRightBmp;
//static HANDLE       hLeftBmp;
static MENULISTTEMPLATE MenuListElement;
static HMENULIST        MenuList;
static HWND    hMenuList;
static HINSTANCE    hInstance;

static LRESULT CountersWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
extern BOOL    CallRecentCallsWindow(HWND hParent, HINSTANCE hinst);
extern BOOL CallMBCommunicationWnd(HWND hParent, HINSTANCE hInstance);
BOOL CreateCallCountersWindow(HWND hParent,BOOL);
BOOL CreateCallCostsWindow(HWND hParent);
BOOL CreateGPRSCountersWindow(HWND hParent);

BOOL CreateLogCounters(HWND hParent, HINSTANCE hinst)
{
	WNDCLASS wc;
	RECT rClient;
    wc.style  = CS_OWNDC;
    wc.lpfnWndProc   = CountersWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;	//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;
    if(!RegisterClass(&wc))
        return FALSE;  
    hPWnd = hParent;
    hInstance = hinst;
    GetClientRect(hParent,&rClient);
    iftype = log_counters;
	hCounterWnd = CreateWindow( pClassName,"",
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hPWnd,
		NULL,
		NULL,
		NULL
		);
    if (hCounterWnd == NULL)
        return FALSE;
	ShowWindow(hPWnd, SW_SHOW); 
//	UpdateWindow(hPWnd);  
	SetFocus(hCounterWnd); 
    return TRUE;
}

static LRESULT CountersWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT lResult;
//	char    szDiaplay[30];
//	int nCount;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
//        hRightBmp = NULL;
//        hLeftBmp = NULL;
        if (!CreateControl(hWnd))
        {
            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
            return -1;
        }
        break;

	case PWM_SHOWWINDOW:
		PDASetMenu(hPWnd, NULL);
		SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, 
			(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );
//		SendMessage(hLogFrame , PWM_CREATECAPTIONBUTTON, 
//			MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK );
		SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");  
		SendMessage(hPWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage( hPWnd, PWM_SETSCROLLSTATE, SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );
		SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)LOG_RIGHT_ICON);
		SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)LOG_LEFT_ICON);
		SetWindowText(hPWnd, CAP_COUNTERS);
		SetFocus(hWnd); 
		break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd, IDC_COUNTER_MENULIST));
        break;

    case WM_CLOSE:
//		SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
//		SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
//		DeleteObject(hRightBmp);
//      DeleteObject(hLeftBmp);
//		DestroyMenuList(MenuList);
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        hCounterWnd = NULL;
//        hRightBmp = NULL;
//        hLeftBmp = NULL;
        UnregisterClass(pClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
			//			SendMessage(hLogFrame,PWM_CLOSEWINDOW,0,0);
			SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			// add after frame modifyed
			SendMessage(hWnd, WM_CLOSE, 0,0);
			//            SendMessage(hPWnd,WM_CLOSE, 0, 0);
			break;	
		case VK_F5:
			{
				int nSel;
				MenuList = (HMENULIST)GetDlgItem(hWnd,IDC_COUNTER_MENULIST);
				nSel = SendMessage(MenuList,ML_GETCURSEL,0,0);
				SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(nSel,ML_SELNOTIFY),NULL);
			}
			break;

        case VK_LEFT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            CallRecentCallsWindow(hPWnd, hInstance);
            break;

        case VK_RIGHT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            CallMBCommunicationWnd(hPWnd, hInstance);
            break;
		// lanlan fix [8/11/2005]
			//block the key msg
		case VK_RETURN:
			break;
		//end
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;        

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_MENUITEM_TIME:
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
				CreateCallCountersWindow(hPWnd,FALSE);
                break;
            }
            break;
            
        case IDC_MENUITEM_CALLCOST:
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
				if(!GetSIMState())
				{
					PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Counters"),Notify_Alert,ML("Ok"),NULL,20);
					return FALSE;
				}
				CreateCallCostsWindow(hPWnd);
                break;
            }
            break;
            
        case IDC_MENUITEM_GPRSCOUNTER:
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
				CreateGPRSCountersWindow(hPWnd);
                break;
            }
            break;

        case IDC_BUTTON_QUIT:
			SendMessage(hPWnd,PWM_CLOSEWINDOW,0,0);
			// add after frame modifyed
			SendMessage(hWnd, WM_CLOSE, 0,0);
            break;
        }
        break;
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static  void    SetMenuListText(HWND hMenuList)
{
    char szDisplay[100] = "";
    int iNum;

    iNum = GetRecorderNumber(UNANSWER_REC);
	strcpy(szDisplay,LISTTXT_UNANSWER);
    SendMessage(hMenuList, ML_SETITEMTEXT, IDC_MENUITEM_UNANSWER, (LPARAM)szDisplay);    
    iNum = GetRecorderNumber(ANSWERED_REC);
	strcpy(szDisplay,LISTTXT_ANSWER);
    SendMessage(hMenuList, ML_SETITEMTEXT, IDC_MENUITEM_ANSWER, (LPARAM)szDisplay);    
    iNum = GetRecorderNumber(DIALED_REC);
	strcpy(szDisplay,LISTTXT_DIAL);
    SendMessage(hMenuList, ML_SETITEMTEXT, IDC_MENUITEM_DIAL, (LPARAM)szDisplay);
}

static  BOOL    CreateControl(HWND hWnd)
{
	RECT rClient;
//	SIZE rsize;
	char szDiaplay[30];
//	int nCount;

	//counter list
	char *  counter_listtxt[] =
	{
		(char *)LISTTXT_TIME,
		(char *)LISTTXT_CALLCOST,
		(char *)LISTTXT_GPRSCOUNTER,
		""
	};
    int counter_idList[] = 
    {
        IDC_MENUITEM_TIME,  
        IDC_MENUITEM_CALLCOST,    
        IDC_MENUITEM_GPRSCOUNTER,      
        -1
    };
	char * counter_pic[] =
    {
        MBPIC_RECORD_CALLTIME,
		MBPIC_RECORD_CALLCOST,
		MBPIC_RECORD_GPRSCOUNTER2,
		""
    };
	szDiaplay[0]= 0;
	SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, 
		(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );
	SendMessage(hPWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
//    SendMessage(hLogFrame , PWM_CREATECAPTIONBUTTON, 
//		MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK );
	SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");  
    SendMessage( hPWnd, PWM_SETSCROLLSTATE, SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );
	SetWindowText(hPWnd, CAP_COUNTERS);
/*
	GetImageDimensionFromFile("/rom/network/arrow_right.bmp", &rsize);
	if (hRightBmp == NULL)		
		hRightBmp =LoadImage(NULL, "/rom/network/arrow_right.bmp", IMAGE_BITMAP,rsize.cx,rsize.cy, LR_LOADFROMFILE);
	SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)hRightBmp);
*/
	SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)LOG_RIGHT_ICON);
/*
	GetImageDimensionFromFile("/rom/network/arrow_left.bmp", &rsize);
	if (hLeftBmp == NULL)		
		hLeftBmp =LoadImage(NULL, "/rom/network/arrow_left.bmp", IMAGE_BITMAP,rsize.cx,rsize.cy, LR_LOADFROMFILE);
	SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)hLeftBmp);
*/
	SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)LOG_LEFT_ICON);
	GetClientRect(hWnd, &rClient);
    MenuList = LoadMenuList(&MenuListElement,FALSE);
    if (MenuList == NULL)
        return FALSE;
    hMenuList = CreateWindow("MENULIST", "", 
        WS_CHILD | WS_VISIBLE |WS_VSCROLL, 
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hWnd, (HMENU)IDC_COUNTER_MENULIST, NULL, (PVOID)MenuList);
    if ( hMenuList == NULL )
        return FALSE;	
    MenuList_AddString(hMenuList, counter_listtxt,counter_idList,counter_pic);

    return TRUE;
}

void CloseCountersWnd()
{
	SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hCounterWnd, 0);
	// add after frame modifyed
	SendMessage(hCounterWnd, WM_CLOSE, 0,0);
}



















