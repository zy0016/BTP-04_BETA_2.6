#include   "winpda.h"
#include  "plx_pdaex.h"
#include  "mullang.h"
#include  "MBPublic.h"
#include  "prebrowhead.h"
#include   "MB_control.h"

#define     MAX_ITEM                180
#define     IDC_OK                  5
#define     IDS_OK                  ML("Ok")
#define     CAP_BT                  ML("Bluetooth")
#define     IDS_LAB_ADDR            ML("Bluetooth device address:")

static HINSTANCE        hInstance;
static HWND             hBTWnd;
static HWND             hBTFormView;
static HWND             hFocus;
static const char * pClassName      = "BTWndClass";
static char pBT[32] = "";
static LRESULT  BTAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);

BOOL ShowBTInfo(HWND hPWnd, char * pbt, int len)
{
    WNDCLASS    wc;
	RECT        rClient;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = BTAppWndProc;
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
	if(pbt == NULL)
		return FALSE;

	if ( len > sizeof(pBT))
	{
		strncpy(pBT, pbt, sizeof(pBT)-1);
	}
	else
	{
		strcpy(pBT, pbt);
	}
	
	if (hPWnd != NULL) 
	{
		GetClientRect(hPWnd, &rClient);
		hBTWnd = CreateWindow(pClassName, CAP_BT,
			PWS_STATICBAR, 
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
			hPWnd, NULL, NULL, NULL);
	}
	else
	{
		hBTWnd = CreateWindow(pClassName, CAP_BT,
			PWS_STATICBAR|WS_CAPTION|PWS_NOTSHOWPI, 
			PLX_WIN_POSITION,
			hPWnd, NULL, NULL, NULL);
	}
    if (NULL == hBTWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hBTWnd);
    ShowWindow(hBTWnd, SW_SHOW);
    UpdateWindow(hBTWnd); 
/*
	if (hCBFormView != NULL)
		SetFocus(hCBFormView);
	else
		SetFocus(hCBWnd);*/

    return TRUE;
}


static LRESULT BTAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
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
			PREBROW_AddData(&Listtmp, IDS_LAB_ADDR, pBT);
			FormView_Register();
			hBTFormView = CreateWindow(WC_FORMVIEW,
				NULL,
				WS_VISIBLE  |WS_CHILD |WS_VSCROLL,
				rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
				hWnd, NULL, NULL, (PVOID)&Listtmp);
			if (hBTFormView == NULL)
				return FALSE;
			hFocus = hBTFormView;
		}
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
        {
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
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hBTWnd);
		hBTFormView = NULL;
        hBTWnd = NULL;
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
