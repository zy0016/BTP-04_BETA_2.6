#include    "winpda.h"
#include    "plx_pdaex.h"
#include  "mullang.h"
#include "MBPublic.h"
#include "prebrowhead.h"
#include   "MB_control.h"


#define     IDC_OK                  5
#define     IDS_OK                  ML("Ok")
#define     CAP_IMEI                ML("IMEI")
#define     IDS_LAB_IMEI            ML("IMEI:")
#define     MAX_LEN     30

static HINSTANCE        hInstance;
static HWND             hIMEIWnd;
static HWND             hIMEIFormView;
static HWND             hFocus;

static const char * pClassName      = "IMEIWndClass";
static char szIMEI[MAX_LEN];
static LRESULT IMEIAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
BOOL ShowIMEIInfo(HWND hPWnd, char * szdip)
{
    WNDCLASS    wc;
	RECT        rClient;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = IMEIAppWndProc;
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
	if (szdip!= NULL)
	{
		if (strlen(szdip)< MAX_LEN) 
			strcpy(szIMEI,szdip);
		else
			strncpy(szIMEI, szdip, MAX_LEN - 1);
	}
	else
		return FALSE;
	if (hPWnd != NULL) 
	{
		GetClientRect(hPWnd, &rClient);
		hIMEIWnd = CreateWindow(pClassName, CAP_IMEI,
			PWS_STATICBAR, 
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
			hPWnd, NULL, NULL, NULL);
	}
	else
	{
		hIMEIWnd = CreateWindow(pClassName, CAP_IMEI,
			PWS_STATICBAR|WS_CAPTION|PWS_NOTSHOWPI, 
			PLX_WIN_POSITION,
			hPWnd, NULL, NULL, NULL);
	}
    if (NULL == hIMEIWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hIMEIWnd);
	ShowWindow(hIMEIWnd, SW_SHOW);
    UpdateWindow(hIMEIWnd); 
/*
	if (hIMEIFormView != NULL)
		SetFocus(hIMEIFormView);
	else
		SetFocus(hIMEIWnd);*/

//	hFocus = GetFocus();
    return TRUE;
}

static LRESULT IMEIAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
    LRESULT     lResult;
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			LISTBUF	Listtmp;

			GetClientRect(hWnd, &rClient);
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_OK);
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
			PDASetMenu(hWnd, NULL);
			PREBROW_InitListBuf (&Listtmp);
			PREBROW_AddData(&Listtmp, IDS_LAB_IMEI, szIMEI);
			FormView_Register();
			hIMEIFormView = CreateWindow(WC_FORMVIEW,
				NULL,
				WS_VISIBLE  |WS_CHILD,
				rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
				hWnd, NULL, NULL, (PVOID)&Listtmp);
			if (hIMEIFormView == NULL)
				return FALSE;
			hFocus = hIMEIFormView;
		}
        break;

/*
	case WM_SETFOCUS:
		SetFocus(hIMEIFormView);
		break;
*/

//	case WM_KILLFOCUS:
//		hFocus = GetFocus();
//		break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
        {
//			SetFocus(hIMEIFormView);
//			if (hFocus == hIMEIFormView)
//				ShowWindow(hFocus, SW_SHOW);
//			else
				SetFocus(hFocus);
        }
		else
        {
            PostMessage(hWnd, WM_CLOSE, 0,0);
            hFocus = GetFocus();
        }
        break;		

    case WM_CLOSE:
		DestroyWindow(hWnd);
        break;
		
    case WM_DESTROY :
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hIMEIWnd);
		hIMEIFormView = NULL;
        hIMEIWnd = NULL;
		hFocus = NULL;
        UnregisterClass(pClassName,NULL);
        break;


	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_OK:
			PostMessage(hWnd, WM_CLOSE, 0,0);
			break;
		}
		break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
