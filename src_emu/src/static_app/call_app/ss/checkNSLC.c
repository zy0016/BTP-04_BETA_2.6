#include    "winpda.h"
#include    "plx_pdaex.h"
#include  "mullang.h"
#include "MBPublic.h"
#include "prebrowhead.h"
#include   "MB_control.h"


#define     IDC_OK                  5
#define     IDS_OK                  ML("Ok")
#define     CAP_NSCL                ML("Network simlock")
#define     MAX_LEN     30

static	PNSLC	pHeadNode = NULL;

static const char *     pDispItem  = "%s %s";           
static HINSTANCE        hInstance;
static HWND             hVnslcWnd;
static HWND             hVnslcFormView;
static HWND             hFocus;
static int              nCounter;
static const char * pClassName      = "VNSLCWndClass";
static char * pOperateName = NULL;
static LRESULT VnslcAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
BOOL ShowNetSimLock(PNSLC  pHead, int counter,const char * pOperName)
{
    WNDCLASS    wc;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = VnslcAppWndProc;
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
	if (pHead!= NULL && counter >0)
	{
		nCounter = counter;
		pHeadNode = malloc(sizeof(NSLC)*counter);
		memset(pHeadNode, 0, sizeof(NSLC)*counter);
		memcpy(pHeadNode, pHead, sizeof(NSLC)*counter);

        if (pOperName != NULL)
        {
            int iLen;
            iLen = strlen(pOperName);
            if (iLen != 0)
            {
                pOperateName = (char*)malloc(iLen + 1);
                if (pOperateName != NULL)
                {
                    memset(pOperateName,0x00,iLen +1);
                    strcpy(pOperateName,pOperName);
                }
            }
        }
	}
	else
		return FALSE;
	hVnslcWnd = CreateWindow(pClassName, CAP_NSCL,
		PWS_STATICBAR|WS_CAPTION|PWS_NOTSHOWPI, 
		PLX_WIN_POSITION,
		NULL, NULL, NULL, NULL);
    if (NULL == hVnslcWnd)
    {
        UnregisterClass(pClassName,NULL);
        if (pOperateName != NULL)
        {
            free(pOperateName);
            pOperateName = NULL;
        }
        return FALSE;
    }
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hVnslcWnd);
	ShowWindow(hVnslcWnd, SW_SHOW);
    UpdateWindow(hVnslcWnd); 
/*
	if (hIMEIFormView != NULL)
		SetFocus(hIMEIFormView);
	else
		SetFocus(hIMEIWnd);*/

//	hFocus = GetFocus();
    return TRUE;
}

static LRESULT VnslcAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
    LRESULT     lResult;
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			LISTBUF	Listtmp;
			int i;
			char dspitem[50]= "";

			GetClientRect(hWnd, &rClient);
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_OK);
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
			PDASetMenu(hWnd, NULL);
			PREBROW_InitListBuf (&Listtmp);
            if (pOperateName != NULL)
            {
                PREBROW_AddData(&Listtmp, NULL, pOperateName);
            }
            else
            {
                for( i = 0; i<nCounter; i++)
                {
                    sprintf(dspitem, pDispItem, pHeadNode[i].strMCC, pHeadNode[i].strMNC);
                    PREBROW_AddData(&Listtmp, NULL, dspitem);
                }
            }
			FormView_Register();
			hVnslcFormView = CreateWindow(WC_FORMVIEW,
				NULL,
				WS_VISIBLE  |WS_CHILD,
				rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
				hWnd, NULL, NULL, (PVOID)&Listtmp);
			if (hVnslcFormView == NULL)
				return FALSE;
			hFocus = hVnslcFormView;
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
		if (pHeadNode != NULL)
		{
			free(pHeadNode);
			pHeadNode = NULL;
		}
        if (pOperateName != NULL)
        {
            free(pOperateName);
            pOperateName = NULL;
        }
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hVnslcWnd);
		hVnslcFormView = NULL;
        hVnslcWnd = NULL;
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
