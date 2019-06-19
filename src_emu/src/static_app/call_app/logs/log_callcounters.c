#include    "window.h"
#include    "mullang.h"
#include    "setting.h"
#include    "RcCommon.h"
#include    "plx_pdaex.h"
#include    "MBPublic.h"
#include    "setup.h"

#define		EMMAXLEN				8

#define		IDRM_RCODE				(WM_USER+1001)
#define     IDC_BUTTON_BACK         3
#define     IDC_BUTTON_RESET        4
#define     IDC_CC_LIST             5
#define     CAP_CALLCOUNTERS        ML("Call counters") 
#define     BUTTON_RESET            (LPARAM)ML("Reset") 
#define     BUTTON_BACK             (LPARAM)ML("Back") 
#define		ERROR1					ML("Create window fail")
#define     IDS_LASTCALLTIME        ML("Last call:") 
#define     IDS_OUTSUM              ML("Dialled calls:")   
#define     IDS_INSUM               ML("Received calls:") 

static const char * pClassName  = "CallCountersWndClass";
static const char * pTime       = "%s:%02d:%02d:%02d";
static const int    iRectX      = 10;
static const int    iNumber60   = 60;
static HWND hCCWnd;
static HWND hPWnd;
static HWND hCCList;
static BOOL ShortCut;
//static BOOL bFirst = TRUE;
static  LRESULT CallCountersWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void LoadCallCounters(MBRecordCostTime * pccdata);
static  BOOL  CreateControl(HWND hWnd);

extern void CloseCountersWnd();

BOOL CreateCallCountersWindow(HWND hParent,BOOL bShortCut)
{
//    HWND        hWnd;
    WNDCLASS    wc;
	RECT  rClient;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = CallCountersWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

	ShortCut = bShortCut;
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
	hPWnd = hParent;
	GetClientRect(hPWnd, &rClient);
    hCCWnd = CreateWindow(pClassName,"", 
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hPWnd, NULL, NULL, NULL);
    if (NULL == hCCWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	ShowWindow(hPWnd, SW_SHOW); 
	UpdateWindow(hPWnd);  
	if (hCCList != NULL)
		SetFocus(hCCList);
	else
		SetFocus(hCCWnd);
    return (TRUE);
}

static LRESULT CallCountersWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    MBRecordCostTime    MRCostTime;
//    RECT        rTitleTime;
//    RECT        rLastTime;
//    RECT        rOutTime;
//    RECT        rInTime;
//    int         iSW,iy[4],iFontHeight;
//    int         TimeAllHour = 0;
//    int         TimeAllMin = 0;
//    int         TimeAllSec = 0;
//    char        cTotalTime[50] = "";
//    char        cLastTime[50] = "";
//    char        cOutTime[50] = "";
//    char        cInTime[50] = "";
    LRESULT     lResult;
//    HDC         hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			if (!CreateControl(hWnd))
			{
				AppMessageBox(NULL,ERROR1, "", WAITTIMEOUT);
				return -1;
			}
			LoadCallCounters(&MRCostTime);
			//bFirst = TRUE;
			//lanlan fix
			SendMessage(hCCList, LB_SETCURSEL, 0, 0);
			//end
//			ReadRecord_Cost(&MRCostTime); 
		}
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");  
		SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK, BUTTON_BACK );
		SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BUTTON_RESET,1),BUTTON_RESET );
		SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
		SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
		SetWindowText(hPWnd, CAP_CALLCOUNTERS);
		SetFocus(hWnd); 
		break;
//lanlan fix
	case WM_SETFOCUS:
		SetFocus(hCCList);
		break;
//end
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY :
		hCCWnd = NULL;
		hCCList = NULL;
		if (ShortCut)
		{
			CloseCountersWnd();

//			DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)GetAPPInstance() );
		}
        UnregisterClass(pClassName,NULL);
        break;

	case IDRM_RCODE:
		{
			char strCode[EMMAXLEN+1];
			char cLockcode[EMMAXLEN +1];
			strCode[0]= 0;
			cLockcode[0] = 0;
			if (SSPLXVerifyPassword(hPWnd, NULL, ML("Reset counters.\nEnter phone\nlock code:"), 
				strCode, MAKEWPARAM(4,8), ML("Ok"), ML("Cancel"), -1))
			{
				GetSecurity_code(cLockcode);
				if (strCode[0] != 0 && cLockcode[0] !=0)
				{
					if(strcmp(strCode,cLockcode) != 0)
					{
						PLXTipsWin(hPWnd, hWnd, IDRM_RCODE, ML("Code wrong"), ML("Call counters"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);								
					}
					else
					{	
						memset(&MRCostTime,0x00,sizeof(MBRecordCostTime));
						SaveRecord_Cost(&MRCostTime);
						UpdateCallCounters();
						//bFirst = TRUE;
						PLXTipsWin(hPWnd, hWnd, 0, ML("Counters reset"), ML("Call counters"), Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
						ShowWindow(hPWnd, SW_SHOW); 
						UpdateWindow(hPWnd);  
						if (hCCList != NULL)
							SetFocus(hCCList);
						else
							SetFocus(hCCWnd);
					}
				}					
			}
		}						
		break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
			SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);			
			SendMessage(hWnd, WM_CLOSE, 0,0);
            break;

        case VK_RETURN:
			{
				PostMessage(hWnd, IDRM_RCODE, 0, 0);
				/*
				char strCode[EMMAXLEN+1];
				char cLockcode[EMMAXLEN +1];
				strCode[0]= 0;
				cLockcode[0] = 0;
				if (SSPLXVerifyPassword(hWnd, NULL, ML("Reset counters.\nEnter phone\nlock code:"), 
						strCode, MAKEWPARAM(4,8), ML("Ok"), ML("Cancel"), -1))
				{
					
					GetSecurity_code(cLockcode);
					
					if (strCode[0] != 0 && cLockcode[0] !=0)
					{
						if(strcmp(strCode,cLockcode) != 0)
						{
							PLXTipsWin(hPWnd, hWnd, IDRM_RCODE, ML("Code wrong"), "", Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);								
							
						}
						else
						{	
							memset(&MRCostTime,0x00,sizeof(MBRecordCostTime));
							SaveRecord_Cost(&MRCostTime);
							UpdateCallCounters(&MRCostTime);
							bFirst = TRUE;
							PLXTipsWin(hPWnd, hWnd, 0, ML("Counters reset"), "", Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
							ShowWindow(hPWnd, SW_SHOW); 
							UpdateWindow(hPWnd);  
							if (hCCList != NULL)
								SetFocus(hCCList);
							else
								SetFocus(hCCWnd);
						}
					}					
				}*/
			}
            break;

        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static  BOOL  CreateControl(HWND hWnd)
{
	RECT rClient;
	GetClientRect(hWnd, &rClient);
	SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");  
	SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK, BUTTON_BACK );
    SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BUTTON_RESET,1),BUTTON_RESET );
	SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
	SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
	SetWindowText(hPWnd, CAP_CALLCOUNTERS);
	hCCList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD|WS_VSCROLL | LBS_NOTIFY | LBS_MULTILINE,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hWnd, (HMENU)IDC_CC_LIST, NULL, NULL);
    if (hCCList == NULL)
        return FALSE;	
    return TRUE;
}

#define  MAX_ITEMLEN 30
static void LoadCallCounters(MBRecordCostTime * pccdata)
{
	SYSTEMTIME systime;
	int day = 0;
	int index = 0;
	char  cTime[15] = "";
	char  cData[15] = "";
	char  cdspTime[MAX_ITEMLEN] = "";
	int len;

	ReadRecord_Cost(pccdata);
	index = SendMessage(hCCList,LB_ADDSTRING,0,(LPARAM)IDS_LASTCALLTIME);
	memset(&systime, 0, sizeof(SYSTEMTIME));
	day = pccdata->nLastHour / 24;
	systime.wHour = pccdata->nLastHour % 24;
	systime.wMinute = pccdata->nLastMin;
	systime.wSecond = pccdata->nLastSecond;
	GetTimeDisplay(systime, cTime, cData);
/*
			if(day > 1)
				sprintf(cdspTime,"%02dd %s:%02d", day, cTime,systime.wSecond);
			else
				sprintf(cdspTime, "%s:%02d", cTime,systime.wSecond);*/
		
	if(day > 1)
		sprintf(cdspTime,"%02dd %02d:%02d:%02d", day, systime.wHour, systime.wMinute, systime.wSecond);
	else
		sprintf(cdspTime,"%02d:%02d:%02d", systime.wHour, systime.wMinute, systime.wSecond);

	len = strlen(cdspTime) + 1;
	if (len > MAX_ITEMLEN) 
	{
		len = MAX_ITEMLEN;
	}
	cdspTime[len - 1] = 0;
	SendMessage(hCCList, LB_SETAUXTEXT, MAKEWPARAM(index, MAX_ITEMLEN),  (LPARAM)cdspTime);

	index = SendMessage(hCCList,LB_ADDSTRING,0,(LPARAM)IDS_OUTSUM);
	memset(&systime, 0, sizeof(SYSTEMTIME));
	day = pccdata->nOutHour / 24;
	systime.wHour = pccdata->nOutHour % 24;
	systime.wMinute = pccdata->nOutMin;
	systime.wSecond = pccdata->nOutSecond;
	GetTimeDisplay(systime, cTime, cData);
/*
	if(day > 1)
		sprintf(cdspTime,"%02dd %s:%02d", day, cTime,systime.wSecond);
	else
		sprintf(cdspTime, "%s:%02d", cTime,systime.wSecond);*/
	if(day > 1)
		sprintf(cdspTime,"%02dd %02d:%02d:%02d", day, systime.wHour, systime.wMinute, systime.wSecond);
	else
		sprintf(cdspTime,"%02d:%02d:%02d", systime.wHour, systime.wMinute, systime.wSecond);

	len = strlen(cdspTime) + 1;
	if (len > MAX_ITEMLEN) 
	{
		len = MAX_ITEMLEN;
	}
	cdspTime[len - 1] = 0;
	SendMessage(hCCList, LB_SETAUXTEXT, MAKEWPARAM(index, MAX_ITEMLEN),  (LPARAM)cdspTime);

	index = SendMessage(hCCList,LB_ADDSTRING,0,(LPARAM)IDS_INSUM);
	memset(&systime, 0, sizeof(SYSTEMTIME));
	day = pccdata->nInHour / 24;
	systime.wHour = pccdata->nInHour % 24;
	systime.wMinute = pccdata->nInMin;
	systime.wSecond = pccdata->nInSecond;
	GetTimeDisplay(systime, cTime, cData);
/*
	if(day > 1)
		sprintf(cdspTime,"%02dd %s:%02d", day, cTime,systime.wSecond);
	else
		sprintf(cdspTime, "%s:%02d", cTime,systime.wSecond);*/
	if(day > 1)
		sprintf(cdspTime,"%02dd %02d:%02d:%02d", day, systime.wHour, systime.wMinute, systime.wSecond);
	else
		sprintf(cdspTime,"%02d:%02d:%02d", systime.wHour, systime.wMinute, systime.wSecond);

	len = strlen(cdspTime) + 1;
	if (len > MAX_ITEMLEN) 
	{
		len = MAX_ITEMLEN;
	}
	cdspTime[len - 1] = 0;
	SendMessage(hCCList, LB_SETAUXTEXT, MAKEWPARAM(index, MAX_ITEMLEN),  (LPARAM)cdspTime);

}

BOOL UpdateCallCounters(void)
{
	SYSTEMTIME systime;
	MBRecordCostTime  ccdata;
	int day = 0;
	int index = 0;
	char  cTime[15] = "";
	char  cData[15] = "";
	char  cdspTime[MAX_ITEMLEN] = "";
	int len;

	if (!hCCList) 
	{
		return FALSE;
	}
	memset(&ccdata, 0, sizeof(MBRecordCostTime));
	ReadRecord_Cost(&ccdata);
//	index = SendMessage(hCCList,LB_ADDSTRING,0,(LPARAM)IDS_LASTCALLTIME);
	memset(&systime, 0, sizeof(SYSTEMTIME));
	day = ccdata.nLastHour / 24;
	systime.wHour = ccdata.nLastHour % 24;
	systime.wMinute = ccdata.nLastMin;
	systime.wSecond = ccdata.nLastSecond;
	GetTimeDisplay(systime, cTime, cData);
	if(day > 1)
		sprintf(cdspTime,"%02dd %02d:%02d:%02d", day, systime.wHour, systime.wMinute, systime.wSecond);
	else
		sprintf(cdspTime,"%02d:%02d:%02d", systime.wHour, systime.wMinute, systime.wSecond);
	len = strlen(cdspTime) + 1;
	if (len > MAX_ITEMLEN) 
	{
		len = MAX_ITEMLEN;
	}
	cdspTime[len - 1] = 0;
	SendMessage(hCCList, LB_SETAUXTEXT, MAKEWPARAM(0, MAX_ITEMLEN),  (LPARAM)cdspTime);

//	index = SendMessage(hCCList,LB_ADDSTRING,0,(LPARAM)IDS_OUTSUM);
	memset(&systime, 0, sizeof(SYSTEMTIME));
	day = ccdata.nOutHour / 24;
	systime.wHour = ccdata.nOutHour % 24;
	systime.wMinute = ccdata.nOutMin;
	systime.wSecond = ccdata.nOutSecond;
	GetTimeDisplay(systime, cTime, cData);
	if(day > 1)
		sprintf(cdspTime,"%02dd %02d:%02d:%02d", day, systime.wHour, systime.wMinute, systime.wSecond);
	else
		sprintf(cdspTime,"%02d:%02d:%02d", systime.wHour, systime.wMinute, systime.wSecond);
	len = strlen(cdspTime) + 1;
	if (len > MAX_ITEMLEN) 
	{
		len = MAX_ITEMLEN;
	}
	cdspTime[len - 1] = 0;
	SendMessage(hCCList, LB_SETAUXTEXT, MAKEWPARAM(1, MAX_ITEMLEN),  (LPARAM)cdspTime);

//	index = SendMessage(hCCList,LB_ADDSTRING,0,(LPARAM)IDS_INSUM);
	memset(&systime, 0, sizeof(SYSTEMTIME));
	day = ccdata.nInHour / 24;
	systime.wHour = ccdata.nInHour % 24;
	systime.wMinute = ccdata.nInMin;
	systime.wSecond = ccdata.nInSecond;
	GetTimeDisplay(systime, cTime, cData);
	if(day > 1)
		sprintf(cdspTime,"%02dd %02d:%02d:%02d", day, systime.wHour, systime.wMinute, systime.wSecond);
	else
		sprintf(cdspTime,"%02d:%02d:%02d", systime.wHour, systime.wMinute, systime.wSecond);
	len = strlen(cdspTime) + 1;
	if (len > MAX_ITEMLEN) 
	{
		len = MAX_ITEMLEN;
	}
	cdspTime[len - 1] = 0;
	SendMessage(hCCList, LB_SETAUXTEXT, MAKEWPARAM(2, MAX_ITEMLEN),  (LPARAM)cdspTime);
	return TRUE;
}

BOOL IsCallCountersWnd()
{
	return ( IsWindow(hCCWnd) ) ;
}






