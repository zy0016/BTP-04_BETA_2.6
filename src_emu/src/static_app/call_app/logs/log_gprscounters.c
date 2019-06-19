#include    "window.h"
#include    "mullang.h"
#include    "setting.h"
#include    "RcCommon.h"
#include    "plx_pdaex.h"
#include    "MBPublic.h"
#include    "setup.h"
#include    "gprsdata.h"

#define		EMMAXLEN				8
#define  MAX_ITEMLEN 30
#define     IDC_BUTTON_BACK         3
#define     IDC_BUTTON_RESET        4
#define     IDC_GPRS_LIST           5
#define     CAP_GPRSCOUNTERS        ML("GPRS counters") 
#define     BUTTON_RESET            (LPARAM)ML("Reset") 
#define     BUTTON_BACK             (LPARAM)ML("Back") 
#define     ERROR1					ML("Create window fail")
#define     IDS_RECEIVEDDATA        ML("Received data:") 
#define     IDS_SENTDATA            ML("Sent data:")   

#define		IDRM_RCODE			WM_USER+1010

static const char * pClassName  = "GPRSCountersWndClass";
static HWND hGPRSWnd;
static HWND hPWnd;
static HWND hGPRSList;
static BOOL bFirst = TRUE;
static GPRSDATA gprs;
static char rdItem[MAX_ITEMLEN];
static char sdItem[MAX_ITEMLEN];

static  LRESULT GPRSCountersWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void LoadGPRSCounters();
static  BOOL  CreateControl(HWND hWnd);
void ByteToSzKiloByte(unsigned long src, char * pdst);

BOOL CreateGPRSCountersWindow(HWND hParent)
{
//    HWND        hWnd;
    WNDCLASS    wc;
	RECT  rClient;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = GPRSCountersWndProc;
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
	hPWnd = hParent;
	GetClientRect(hPWnd, &rClient);
    hGPRSWnd = CreateWindow(pClassName,"", 
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hPWnd, NULL, NULL, NULL);
    if (NULL == hGPRSWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	ShowWindow(hPWnd, SW_SHOW); 
	UpdateWindow(hPWnd);  
	if (hGPRSList != NULL)
		SetFocus(hGPRSList);
	else
		SetFocus(hGPRSWnd);
    return (TRUE);
}

static LRESULT GPRSCountersWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
//    MBRecordCostTime    MRCostTime;
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
			memset(&gprs, 0, sizeof(GPRSDATA));
			rdItem[0]=0;
			sdItem[0]=0;
			LoadGPRSCounters();
			bFirst = TRUE;
			//lanlan fix
			SendMessage(hGPRSList, LB_SETCURSEL, 0, 0);
//			ReadRecord_Cost(&MRCostTime); 
		}
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");  
		SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK, BUTTON_BACK );
		SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BUTTON_RESET,1),BUTTON_RESET );
		SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
		SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
		SetWindowText(hPWnd, CAP_GPRSCOUNTERS);
		SetFocus(hWnd); 
		break;
//lanlan fix
	case WM_SETFOCUS:
		SetFocus(hGPRSList);
		break;
//end
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY :
		hGPRSWnd = NULL;
		hGPRSList = NULL;
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
						PLXTipsWin(hPWnd, hWnd, IDRM_RCODE, ML("Code wrong"), NULL, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);								
						SSPLXVerifyPassword(hWnd, NULL, ML("Reset counters.\nEnter phone\nlock code:"), 
							NULL, MAKEWPARAM(4,8), ML("Ok"), ML("Cancel"), -1);							
					}
					else
					{	
						ResetData();
						UpdateGPRSCounters();
						bFirst = TRUE;
						PLXTipsWin(hPWnd, hWnd, 0, ML("Counters reset"), NULL, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
						ShowWindow(hPWnd, SW_SHOW); 
						UpdateWindow(hPWnd);  
						if (hGPRSList != NULL)
							SetFocus(hGPRSList);
						else
							SetFocus(hGPRSList);
					}
				}				
			}
		}						
		break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
			if(bFirst == FALSE)
			{
				bFirst = TRUE;				
				break;
			}
			SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);			
			SendMessage(hWnd, WM_CLOSE, 0,0);
            break;
        case VK_RETURN:
			PostMessage(hWnd, IDRM_RCODE, 0, 0);
			/*
			{
				char strCode[EMMAXLEN+1];
				char cLockcode[EMMAXLEN +1];
				strCode[0]= 0;
				cLockcode[0] = 0;
				if (bFirst)
				{
					bFirst = FALSE;
					//SSPLXVerifyPassword的使用错误,需要修改.
					if(!SSPLXVerifyPassword(hWnd, NULL, ML("Reset counters.\nEnter phone\nlock code:"), 
						NULL, MAKEWPARAM(4,8), ML("Ok"), ML("Cancel"), -1))
						bFirst = TRUE;
					break;
				}
				else
				{
					if(lParam == NULL)
						break;		
					strcpy(strCode, (char*)lParam);
					GetSecurity_code(cLockcode);
					
					if (strCode[0] != 0 && cLockcode[0] !=0)
					{
						if(strcmp(strCode,cLockcode) != 0)
						{
							PLXTipsWin(hPWnd, hWnd, 0, ML("Code wrong"), "", Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);								
							SSPLXVerifyPassword(hWnd, NULL, ML("Reset counters.\nEnter phone\nlock code:"), 
								NULL, MAKEWPARAM(4,8), ML("Ok"), ML("Cancel"), -1);							
						}
						else
						{	
							ResetData();
							UpdateGPRSCounters();
							bFirst = TRUE;
							PLXTipsWin(hPWnd, hWnd, 0, ML("Counters reset"), "", Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
							ShowWindow(hPWnd, SW_SHOW); 
							UpdateWindow(hPWnd);  
							if (hGPRSList != NULL)
								SetFocus(hGPRSList);
							else
								SetFocus(hGPRSList);
						}
					}					
				}
			}
			*/
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
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
	SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
	SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
	SetWindowText(hPWnd, CAP_GPRSCOUNTERS);
	hGPRSList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD|WS_VSCROLL | LBS_NOTIFY | LBS_MULTILINE,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hWnd, (HMENU)IDC_GPRS_LIST, NULL, NULL);
    if (hGPRSList == NULL)
        return FALSE;
    return TRUE;
}


static void LoadGPRSCounters()
{
	int index = 0;

	if (RTN_GPRS_FAILURE == FindGPRSData(&gprs))
		return ;

	ByteToSzKiloByte(gprs.rdtotal, rdItem);
	ByteToSzKiloByte(gprs.sdtotal, sdItem);
	index = SendMessage(hGPRSList,LB_ADDSTRING,0,(LPARAM)IDS_RECEIVEDDATA);
	SendMessage(hGPRSList, LB_SETAUXTEXT, MAKEWPARAM(index, MAX_ITEMLEN),  (LPARAM)rdItem);
	index = SendMessage(hGPRSList,LB_ADDSTRING,0,(LPARAM)IDS_SENTDATA);
	SendMessage(hGPRSList, LB_SETAUXTEXT, MAKEWPARAM(index, MAX_ITEMLEN),  (LPARAM)sdItem);
}

BOOL UpdateGPRSCounters()
{
	if (!hGPRSList) {
		return FALSE;
	}
	if (RTN_GPRS_FAILURE == FindGPRSData(&gprs))
		return FALSE;
	if (!hGPRSList) {
		return FALSE;
	}
	ByteToSzKiloByte(gprs.rdtotal, rdItem);
	ByteToSzKiloByte(gprs.sdtotal, sdItem);
	SendMessage(hGPRSList, LB_SETAUXTEXT, MAKEWPARAM(0, MAX_ITEMLEN),  (LPARAM)rdItem);
	SendMessage(hGPRSList, LB_SETAUXTEXT, MAKEWPARAM(1, MAX_ITEMLEN),  (LPARAM)sdItem);
	return TRUE;
}

void ByteToSzKiloByte(unsigned long src, char * pdst)
{
	int i, j;
	if (0 == (src/1000)) 
	{
		if (0 == (src/100)) 
		{
			strcpy(pdst, "0.0 kB");
			return;
		}
		else
		{
			i = src/100;
			sprintf(pdst, "0.%d kB", i);
			return;
		}
	}
	else
	{
		i = src /1000;
		j = src % 1000;
		if (0 == (j/100)) 
		{
			sprintf(pdst, "%d kB", i);
			return;
		}
		else
		{
			j = j/100;
			sprintf(pdst, "%d.%d kB", i, j);
			return;
		}
	}
}
