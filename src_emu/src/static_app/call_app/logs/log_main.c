#include  <sys/stat.h>
#include "log_main.h"
#include "log.h"

//#define LOGS_SHORTCUT_ONCE	//if shortcut window not be off,
							//and we go to the app and end once will show idle

#define  WM_INFOUPDATE WM_USER+101

#define LOG_LEFT_ICON	"/rom/public/arrow_left.ico"
#define LOG_RIGHT_ICON	"/rom/public/arrow_right.ico"

typedef enum
{
    MBRECORD_MAIN_WS_DETAIL, 
    MBRECORD_MAIN_WS_UNDETAIL
}MBRECORDMAIN_STYLE;

static LOGIFTYPE   iftype;
static HWND         hLogFrame;
static HWND         hRecentCallWnd;
static BOOL         bShow; 
static HINSTANCE    hInstance;
static const char * pClassName = "MBLogMainWndClass";

static BOOL bShortCut1,bShortCut2,bShortCut3,bShortCut4;

static MENULISTTEMPLATE MenuListElement;
static HMENULIST        MenuList;
static HWND    hMenuList;

static const MBRECORDMAIN_STYLE MBRecordMain_Style = MBRECORD_MAIN_WS_DETAIL;
extern int  InitMissData();
extern BOOL IsDailedRecordWnd();
extern BOOL IsMissedRecordWnd();

extern void HideRecordWnd();
extern void HideRecordWnd();
extern void ShowRecordWnd();
extern HWND GetFrameHandle();
extern BOOL GetDFlag();
extern BOOL CreateCallCountersWindow(HWND,BOOL);
extern BOOL CreateLogCounters(HWND hParent, HINSTANCE hinst);
extern BOOL CallMBCommunicationWnd (HWND hParent, HINSTANCE hInstance);

extern BOOL IsCallCountersWnd();

BOOL CallRecentCallsWindow (HWND hParent, HINSTANCE hinst);
void ShowMainWnd();

static BOOL bshowricon;

DWORD CallLogs_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
    WNDCLASS wc;
	RECT rClient;
    DWORD dwRet = TRUE;
	
    switch (nCode)
    {
    case APP_INIT :
		mkdir(LOG_DIR,S_IRUSR);
        hInstance = pInstance;
        InitMBRecordData();
		InitMissData();
		if (Log_Init() == -1)
			return FALSE;

        break;
		
    case APP_ACTIVE :
/*		
		if ( lParam == 1 )		//favourite & shortcut for logs
		{
			if (hLogFrame)
			{
				bShortCut1 = TRUE;
				DestroyWindow(hLogFrame);
				DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
				bShortCut1 = FALSE;
				hLogFrame = NULL;
			}

			hLogFrame=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR);
			CallMBLogListExtWindow(hLogFrame, hRecentCallWnd, UNANSWER_REC,MBL_WS_DEFAULT, TRUE);

			break;
		}
		else if ( lParam == 2 )
		{
			if (hLogFrame)
			{
				bShortCut2 = TRUE;
				DestroyWindow(hLogFrame);
//				DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
				bShortCut2 = FALSE;
				hLogFrame = NULL;
			}
			
			hLogFrame=CreateFrameWindow( WS_CAPTION | PWS_STATICBAR);
			
			CreateCallCountersWindow(hLogFrame,TRUE);
			
			break;
		}
		else if ( lParam == 3 )
		{
			if (hLogFrame)
			{
				bShortCut3 = TRUE;
				DestroyWindow(hLogFrame);
				DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
				bShortCut3 = FALSE;
				hLogFrame = NULL;
			}
			
			hLogFrame=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR);

			CallMBCommunicationWnd(hLogFrame, hInstance);		
			break;
		}
		else if ( lParam == 4 )	//for idle to show dialled call list
		{
			if (hLogFrame)
			{
				bShortCut4 = TRUE;
				DestroyWindow(hLogFrame);
				DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
				bShortCut4 = FALSE;
				hLogFrame = NULL;
			}

			hLogFrame=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR);

			CallMBLogListExtWindow(hLogFrame,hRecentCallWnd, DIALED_REC,MBL_WS_DEFAULT, TRUE);
			
			break;
		}
*/

		if(IsWindow(hLogFrame))		//if logs application exist.
		{
			if (lParam == 4) 
				CallMBLogListExtWindow(hLogFrame,hRecentCallWnd, DIALED_REC,MBL_WS_DEFAULT, TRUE);
			else if (IsDailedRecordWnd())	//if dialedRecord window exist.
			{
				CallMBLogListExtWindow(hLogFrame,hRecentCallWnd, DIALED_REC,MBL_WS_DEFAULT, FALSE);
			}
			if (lParam == 1)
				CallMBLogListExtWindow(hLogFrame,hRecentCallWnd, UNANSWER_REC,MBL_WS_DEFAULT, TRUE);
			else if (IsMissedRecordWnd())	//if MissedRecord window exist.
			{
				CallMBLogListExtWindow(hLogFrame,hRecentCallWnd, UNANSWER_REC,MBL_WS_DEFAULT, FALSE);
			}
			else if (lParam == 2)
			{
				CreateCallCountersWindow(hLogFrame,FALSE);
			}
			else if (IsCallCountersWnd())
			{
				CreateCallCountersWindow(hLogFrame,FALSE);
			}
		
			ShowWindow(hLogFrame, SW_SHOW);
			ShowOwnedPopups(hLogFrame, SW_SHOW);
			UpdateWindow(hLogFrame);
		}
        else
		{
			if ( lParam != 2 && lParam != 3)
			{
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
			if(!RegisterClass(&wc))
				return FALSE;

			hLogFrame=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR);
			GetClientRect(hLogFrame,&rClient);
			iftype = log_recentcalls;
			if (lParam != 1 && lParam != 2)
				bshowricon = TRUE;
			else
				bshowricon = FALSE;
			hRecentCallWnd = CreateWindow(
                pClassName,
                "",
                WS_VISIBLE | WS_CHILD,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
                hLogFrame,
                NULL,
                NULL,
                NULL
                );
			}
			else if ( lParam == 2 )	//the parent window of CallCounters 
			{
				hLogFrame=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR);
				CreateLogCounters(hLogFrame, hInstance);
			}
			else if ( lParam == 3 )	//create the Communication window
			{
				hLogFrame=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR);
			}
			
			if (lParam != 1 && lParam != 2 && lParam != 3 && lParam != 4)
			{
				ShowWindow(hLogFrame, SW_SHOW);   
				SetFocus(hRecentCallWnd); 	
			}

			if (lParam == 1) 
				CallMBLogListExtWindow(hLogFrame, hRecentCallWnd, UNANSWER_REC,MBL_WS_DEFAULT, TRUE);
			else if (lParam == 2)
			{
				CreateCallCountersWindow(hLogFrame,TRUE);
			}
			else if (lParam == 3)
			{
				CallMBCommunicationWnd(hLogFrame, hInstance);
			}
			else if (lParam == 4)
			{
				CallMBLogListExtWindow(hLogFrame,hRecentCallWnd, DIALED_REC,MBL_WS_DEFAULT, TRUE);
			}

		}
        break;
		
    case APP_INACTIVE :
		ShowOwnedPopups(hLogFrame, SW_HIDE);
        ShowWindow(hLogFrame,SW_HIDE); 
        break;
		
    default :
        break;
    }
    return dwRet;
}

HINSTANCE GetAPPInstance()
{
	return hInstance;
}

BOOL    CallRecentCallsWindow(HWND hParent, HINSTANCE hinst)
{
    WNDCLASS    wc;
	RECT        rClient;
	
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;	
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    hLogFrame = hParent;
    hInstance = hinst;
	GetClientRect(hLogFrame, &rClient);
	hRecentCallWnd = CreateWindow(
		pClassName,
		"",
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hLogFrame,
		NULL,
		NULL,
		NULL
		);
	
    if (NULL == hRecentCallWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	
	ShowWindow(hLogFrame, SW_SHOW); 
//	UpdateWindow(hLogFrame);  
	SetFocus(hRecentCallWnd); 
    return (TRUE);
}


static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT lResult;
	char    szDiaplay[30];
	int nCount;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
//        hRightBmp = NULL;
        if (!CreateControl(hWnd))
        {
            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
            return -1;
        }
        break;

	case WM_INFOUPDATE:
		szDiaplay[0]=0;
		GetRecentMissedCall(&nCount);
		if (nCount> 0)
		{
			sprintf(szDiaplay,"%s -t %d",LISTTXT_UNANSWER,nCount);
			SendMessage(hMenuList,ML_SETITEMTEXT,(WPARAM)IDC_MENUITEM_UNANSWER,(LPARAM)szDiaplay);
		}
		else
		{
			sprintf(szDiaplay,"%s",LISTTXT_UNANSWER);
			SendMessage(hMenuList,ML_SETITEMTEXT,(WPARAM)IDC_MENUITEM_UNANSWER,(LPARAM)szDiaplay);
		}
	break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd, IDC_MENULIST));
        break;

	case PWM_SHOWWINDOW:
		PDASetMenu(hLogFrame, NULL);
		//lanlan fix
//		SendMessage(hLogFrame, PWM_CREATECAPTIONBUTTON, 
//			(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );
		SendMessage(hLogFrame, PWM_SETBUTTONTEXT, 0, BUTTONQUIT );
		//end
		SendMessage(hLogFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");  
		SendMessage(hLogFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage( hLogFrame, PWM_SETSCROLLSTATE,
			SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );
		SendMessage(hLogFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)LOG_RIGHT_ICON);
		SendMessage(hLogFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
		SetWindowText(hLogFrame, CAP_RCTCALL);
		SetFocus(hWnd); 
		break;

    case WM_CLOSE:
//		SendMessage(hLogFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
//		SendMessage(hLogFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
		
//		DestroyMenuList(MenuList);
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        hRecentCallWnd = NULL;
//		hLogFrame = NULL;
        UnregisterClass(pClassName,NULL);

//		if ( bShortCut1 || bShortCut2 || bShortCut3 || bShortCut4 )
//			DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
			SendMessage(hLogFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			SendMessage(hWnd, WM_CLOSE, 0,0);
			break;	

		case VK_F5:
			{
				int nSel;
				MenuList = (HMENULIST)GetDlgItem(hWnd,IDC_MENULIST);
				nSel = SendMessage(MenuList,ML_GETCURSEL,0,0);
				SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(nSel,ML_SELNOTIFY),0);
			}
			break;

        case VK_RIGHT:
            PostMessage(hWnd,WM_CLOSE,0,0);
            CreateLogCounters(hLogFrame, hInstance);
            break;
			
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;        

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_MENUITEM_UNANSWER:
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                CallMBLogListExtWindow(hLogFrame, hWnd, UNANSWER_REC,MBL_WS_DEFAULT, FALSE);
                break;
            }
            break;
            
        case IDC_MENUITEM_ANSWER:
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                CallMBLogListExtWindow(hLogFrame,hWnd, ANSWERED_REC,MBL_WS_DEFAULT, FALSE);
                break;
            }
            break;
            
        case IDC_MENUITEM_DIAL:
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                CallMBLogListExtWindow(hLogFrame,hWnd, DIALED_REC,MBL_WS_DEFAULT,FALSE);
                break;
            }
            break;

        case IDC_BUTTON_QUIT:
			SendMessage(hLogFrame,PWM_CLOSEWINDOW,0,0);
			// add after frame modifyed
			SendMessage(hWnd, WM_CLOSE, 0,0);
//            DestroyMenuList(MenuList);
//            DestroyWindow(hWnd);  
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
	int nCount;

	//recent call list
	char *  recentcall_listtxt[] =
	{
		(char *)LISTTXT_UNANSWER,
		(char *)LISTTXT_ANSWER,
		(char *)LISTTXT_DIAL,
		""
	};
    int recentcall_idList[] = 
    {
        IDC_MENUITEM_UNANSWER,  
        IDC_MENUITEM_ANSWER,    
        IDC_MENUITEM_DIAL,      
        -1
    };
	char * recentcall_pic[] =
    {
        MBPIC_RECORD_UNANSWER,
		MBPIC_RECORD_ANSWER,
		MBPIC_RECORD_DIALED,
		""
    };
	szDiaplay[0]= 0;
	SendMessage(hLogFrame, PWM_CREATECAPTIONBUTTON, 
		(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );
	//lanlan fix
	//SendMessage(hLogFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
    SendMessage(hLogFrame , PWM_CREATECAPTIONBUTTON, 
		MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)"" );
	//end
	SendMessage(hLogFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");  
    SendMessage( hLogFrame, PWM_SETSCROLLSTATE,
		SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );
/*
	GetImageDimensionFromFile("/rom/network/arrow_right.bmp", &rsize);
	if (hRightBmp == NULL)		
		hRightBmp =LoadImage(NULL, "/rom/network/arrow_right.bmp", IMAGE_BITMAP,rsize.cx,rsize.cy, LR_LOADFROMFILE);
*/
	if (bshowricon)
	{
		SetWindowText(hLogFrame, CAP_RCTCALL);
//		SendMessage(hLogFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)hRightBmp);
		SendMessage(hLogFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)LOG_RIGHT_ICON);
		SendMessage(hLogFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
	}
	GetClientRect(hWnd, &rClient);
    MenuList = LoadMenuList(&MenuListElement,FALSE);
    if (MenuList == NULL)
        return FALSE;
    hMenuList = CreateWindow("MENULIST", "", 
        WS_CHILD | WS_VISIBLE |WS_VSCROLL, 
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hWnd, (HMENU)IDC_MENULIST, NULL, (PVOID)MenuList);
    if ( hMenuList == NULL )
        return FALSE;	
    MenuList_AddString(hMenuList,recentcall_listtxt,recentcall_idList,recentcall_pic);
	GetRecentMissedCall(&nCount);
	if (nCount> 0)
	{
		sprintf(szDiaplay,"%s -t %d",LISTTXT_UNANSWER,nCount);
		SendMessage(hMenuList,ML_SETITEMTEXT,(WPARAM)IDC_MENUITEM_UNANSWER,(LPARAM)szDiaplay);
	}
	RegisterInfoWnd(hWnd, WM_INFOUPDATE);
    return TRUE;
}

void CloseMainWnd()
{
	SendMessage(hLogFrame,PWM_CLOSEWINDOW,  (WPARAM) hRecentCallWnd, 0);
	// add after frame modifyed
	SendMessage(hRecentCallWnd, WM_CLOSE, 0,0);
}

void ShowMainWnd()
{
	ShowWindow(hLogFrame, SW_SHOW); 
	SetFocus(hRecentCallWnd); 				
}

