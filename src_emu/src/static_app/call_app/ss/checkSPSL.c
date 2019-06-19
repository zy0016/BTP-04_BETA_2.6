#include    "winpda.h"
#include    "plx_pdaex.h"
#include  "mullang.h"
#include "MBPublic.h"
#include "prebrowhead.h"
#include   "MB_control.h"


#define     IDC_OK                  5
#define     IDS_OK                  ML("Ok")
#define     CAP_SPSL                ML("SP simlock")
#define     MAX_LEN     30


static	PSPSIMLOCKCODE	pHeadNode = NULL;

static const char *     pDispItem  = "%s %s %s";           
static HINSTANCE        hInstance;
static HWND             hVspslWnd;
static HWND             hVspslFormView;
static HWND             hFocus;
static const char * pClassName      = "VSPSLWndClass";
static LRESULT VspslAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
BOOL ShowSPSimLock(PSPSIMLOCKCODE pHead)
{
    WNDCLASS    wc;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = VspslAppWndProc;
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
	if (pHead!= NULL)
	{
		pHeadNode = malloc(sizeof(SPSIMLOCKCODE));
		memset(pHeadNode, 0, sizeof(SPSIMLOCKCODE));
		memcpy(pHeadNode, pHead, sizeof(SPSIMLOCKCODE));
	}
	else
		return FALSE;
	hVspslWnd = CreateWindow(pClassName, CAP_SPSL,
		PWS_STATICBAR|WS_CAPTION|PWS_NOTSHOWPI, 
		PLX_WIN_POSITION,
		NULL, NULL, NULL, NULL);
    if (NULL == hVspslWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hVspslWnd);
	ShowWindow(hVspslWnd, SW_SHOW);
    UpdateWindow(hVspslWnd); 
    return TRUE;
}

static LRESULT VspslAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
    LRESULT     lResult;
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			LISTBUF	Listtmp;
			char dspitem[100]= "";

			GetClientRect(hWnd, &rClient);
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_OK);
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
			PDASetMenu(hWnd, NULL);
			PREBROW_InitListBuf (&Listtmp);
			sprintf(dspitem, pDispItem, pHeadNode->strMCC, pHeadNode->strMNC,pHeadNode->strGID1);
			PREBROW_AddData(&Listtmp, NULL, dspitem);
			FormView_Register();
			hVspslFormView = CreateWindow(WC_FORMVIEW,
				NULL,
				WS_VISIBLE  |WS_CHILD,
				rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
				hWnd, NULL, NULL, (PVOID)&Listtmp);
			if (hVspslFormView == NULL)
				return FALSE;
			hFocus = hVspslFormView;
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
		if (pHeadNode != NULL)
		{
			free(pHeadNode);
			pHeadNode = NULL;
		}
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hVspslWnd);
		hVspslFormView = NULL;
        hVspslWnd = NULL;
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
