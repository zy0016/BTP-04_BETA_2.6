#include "stdio.h"
#include "string.h"
#include "window.h"
#include "winpda.h"
#include "pubapp.h"
#include "plx_pdaex.h"
#include "me_wnd.h"
#include "network.h"

#define  IDC_REVEALOWNNUM_LIST		0x101

#define  Notify_Alert				4

#define  ME_MSG_GETCLIRSTATUS		(WM_USER+101)
#define  ME_MSG_SETCLIR				(WM_USER+102)

static LRESULT RevealOwnNumProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

extern int  GetSIMState ();
extern BOOL WriteToNetWorkFile(int i);
extern int  ReadFromNetWorkFile(void);

void RevealOwnNumWin(void *info)
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = RevealOwnNumProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "RevealOwnNum";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("RevealOwnNum","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		(PVOID)info);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
	
}

static LRESULT RevealOwnNumProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	HDC hdc;
	static HWND hList;
	static HBITMAP hRadioOn,hRadioOff;
	static int iSelected;
	static int status;
	HWND hPrevWin;

#define DEFAULT		0x0
#define HIDE		0x1
#define SHOW        0x2
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			int i;
			PCREATESTRUCT pCreateStruct=(PCREATESTRUCT)lParam;

			memcpy(&iSelected,pCreateStruct->lpCreateParams,sizeof(int));

			GetClientRect(hWnd,&rClient);

			hList=CreateWindow("LISTBOX","",
				WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)IDC_REVEALOWNNUM_LIST,NULL,NULL);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Decided by network"));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Yes"));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("No"));
			
			hdc=GetDC(hWnd);
			hRadioOn=CreateBitmapFromImageFile(hdc,"/rom/network/radio_button_on.bmp",NULL,NULL);
			hRadioOff=CreateBitmapFromImageFile(hdc,"/rom/network/radio_button_off.bmp",NULL,NULL);
			ReleaseDC(hWnd,hdc);

			for(i=0;i<3;i++)
				SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hRadioOff);
			
			SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)iSelected),(LPARAM)hRadioOn);
			SendMessage(hList,LB_SETCURSEL,iSelected,0);
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
			SetWindowText(hNetworkFrame,ML("Reveal own nr"));

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
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
		SetWindowText(hNetworkFrame,ML("Reveal own nr"));
		SetFocus(hList);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("RevealOwnNum",NULL);
		DeleteObject(hRadioOn);
		DeleteObject(hRadioOff);
        break;
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;
			
		case VK_F5:
			{
				int i;

				if(!GetSIMState())
				{
					PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Reveal own nr"),Notify_Alert,ML("Ok"),NULL,20);
					return FALSE;
				}

				i=SendMessage(hList,LB_GETCURSEL,0,0); 
				SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hRadioOn);

				if(i==0)
				{
						printf("Can not set CLIR Enable\r\n");
					status=DEFAULT;
				}
				else if(i==1)
				{
						printf("Can not set CLIR Enable\r\n");
					status=HIDE;
				}
				else if(i==2)
				{
						printf("Can not set CLIR Disable\r\n");
					status=SHOW;
				}

				WriteToNetWorkFile(status);
				
				SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,iSelected),(LPARAM)hRadioOff);
				SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,i),(LPARAM)hRadioOn);
				iSelected=i;

				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				SendMessage(hPrevWin,WM_REVEALOWNNR_CHANGED,status,0);
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
				break;
			}
			break;
		}
		break;
		
		
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
	
    return lResult;
}

void GetErrorString(ME_EXERROR ME_Error,char *strErrorInfo)
{
	switch(ME_Error.LocationID) 
	{
	case 35: //SS Network Error
		switch(ME_Error.SSrelease) 
		{
		case 1: //UnknownSUbscriber
			strcpy(strErrorInfo,ML("Unknow subscriber"));
			break;
		case 9://IllegalSubscriber
			strcpy(strErrorInfo,ML("Illegal subscriber"));
			break;
		case 10: //BearerServiceNotProvisioned
			strcpy(strErrorInfo,ML("Service not available"));
			break;
		case 11: //TeleserviceNotProvisioned
			strcpy(strErrorInfo,ML("Service not available"));
			break;
		case 12://IllegalEquipment
			strcpy(strErrorInfo,ML("Illegal equipment"));
			break;
		case 13://CallBarred
			strcpy(strErrorInfo,ML("Call barred"));
			break;
		case 16://IllegalSSOperation
			strcpy(strErrorInfo,ML("Illegal operation"));
			break;
		case 17://SSDerrorStatus
			strcpy(strErrorInfo,ML("Status error"));
			break;
		case 18://SSNotAvailable
			strcpy(strErrorInfo,ML("Service not available"));
			break;
		case 19: //SSSubscriptionViolation
			strcpy(strErrorInfo,ML("Subscription violation"));
			break;
		case 20://SSincompatibility
			strcpy(strErrorInfo,ML("Incompatibility error"));
			break;
		case 21://FacilityNotSupported
			strcpy(strErrorInfo,ML("Service not available"));
			break;
		case 27://AbsentSubscriber
			strcpy(strErrorInfo,ML("Absent subscriber"));
			break;
		case 29://ShortTermDenial
			strcpy(strErrorInfo,ML("Short term denial"));
			break;
		case 30://LongTermDenial
			strcpy(strErrorInfo,ML("Long term denial"));
			break;
		case 34://SystemFailure
			strcpy(strErrorInfo,ML("Network system\nfailure"));
			break;
		case 35://DataMissing
			strcpy(strErrorInfo,ML("Illegal data"));
			break;
		case 36://UnexpectedDataValur
			strcpy(strErrorInfo,ML("Illegal data"));
			break;
		case 37://PWRegistrationFailure
			strcpy(strErrorInfo,ML("Barring code\nchanging failed"));
			break;
		case 38://NegativePWCheck
			strcpy(strErrorInfo,ML("Barring code\nwrong"));
			break;
		case 43://NumberOfPWAttemptsViolation
			strcpy(strErrorInfo,ML("Barring code\nblocked"));
			break;
		case 71://Unknow Alphabet
			strcpy(strErrorInfo,ML("Unknown alphabet"));
			break;
		case 72://USSDBusy
			strcpy(strErrorInfo,ML("USSD busy"));
			break;
		case 126://MaxNumsOfMPTYCallsExceeded
			strcpy(strErrorInfo,ML("No more calls"));
			break;
		case 127://ResourcesNotAvailable
			strcpy(strErrorInfo,ML("Resource\nnot available"));
			break;
		default:
			strcpy(strErrorInfo,ML("Other errors"));
			break;
		}
		break;

	default:  //Non SS Error
		strcpy(strErrorInfo,ML("Other errors"));
		break;
	}
	
}
