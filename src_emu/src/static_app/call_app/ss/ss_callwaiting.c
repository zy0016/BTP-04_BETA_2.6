  #include   "winpda.h"
#include  "plx_pdaex.h"
#include  "mullang.h"
#include  "MBPublic.h"
#include  "prebrowhead.h"
#include   "MB_control.h"


#define     MAX_ITEM                180
#define     IDC_OK                  5
#define     IDS_OK                  ML("Ok")
#define     CAP_CALLWAITING         "Call waiting active"
#define     IDS_LAB_SERVICE         ML("Service:")
#define     CAP_NETWORKSERVICE      ML("Network service")

static HINSTANCE        hInstance;
static HWND             hCWWnd;
static HWND             hCWFormView;
static HWND             hFocus;
static int              iBsCounter;
static int              idxBS;
static const char * pClassName      = "CWWndClass";
static CWINFO * pcwinfo = NULL;
static LRESULT  CWAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
BOOL ShowCWInfo(HWND hPWnd, CWINFO * pcf, int len)
{
    WNDCLASS    wc;
	RECT        rClient;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = CWAppWndProc;
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
	if (pcf != NULL)
	{
		pcwinfo = malloc(len);
		if (pcwinfo == NULL)
			return FALSE;
		memcpy(pcwinfo, pcf, len);
		iBsCounter = len /sizeof(CWINFO);
	}
	else
		return FALSE;
	if (hPWnd != NULL) 
	{
		GetClientRect(hPWnd, &rClient);
		hCWWnd = CreateWindow(pClassName, CAP_CALLWAITING,
			PWS_STATICBAR, 
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
			hPWnd, NULL, NULL, NULL);
	}
	else
	{
		hCWWnd = CreateWindow(pClassName, CAP_CALLWAITING,
			PWS_STATICBAR|WS_CAPTION|PWS_NOTSHOWPI, 
			PLX_WIN_POSITION,
			hPWnd, NULL, NULL, NULL);
	}
    if (NULL == hCWWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hCWWnd);
    ShowWindow(hCWWnd, SW_SHOW);
    UpdateWindow(hCWWnd); 
/*
	if (hCWFormView != NULL)
		SetFocus(hCWFormView);
	else
		SetFocus(hCWWnd);*/

    return TRUE;
}

static LRESULT CWAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
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
			char itemBS[MAX_ITEM];
			//			char addBS[20];
			char oldBS[MAX_ITEM];
			itemBS[0]=0;		
			//			addBS[0] = 0;
			oldBS[0] = 0;
			GetClientRect(hWnd, &rClient);
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_OK);
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
			PDASetMenu(hWnd, NULL);
			idxBS = 0;
			i= idxBS;
			while ((!pcwinfo[i].Active) && ( i < iBsCounter)) 
			{
				i++;
/*
								if ( (i == iBsCounter -1) &&(!pcwinfo[i].Active))
								{
									PostMessage(hWnd, WM_CLOSE, 0, 0);
									//break;
								}*/
				
			}
			if (i == iBsCounter)
			{
				PLXTipsWin(NULL, NULL, NULL, ML("Call waiting\nnot active"), CAP_NETWORKSERVICE, Notify_Info, ML("Ok"), NULL, -1);
                return -1;
			}
			else
			{
                int l = 0;
                PREBROW_InitListBuf (&Listtmp);
                while (l < iBsCounter)
                {
                    idxBS = l++;			
                    if (!pcwinfo[idxBS].Active)
                        continue;
                    switch (pcwinfo[idxBS].BS)
                    {
                    case BSVOICE:
                        //sprintf(itemBS, "%s", ML("Voice"));
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Voice"));
                        break;
                    case BSDATA:
                        //					sprintf(itemBS, "%s", ML("FAX and SMS"));
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, "All data");
                        break;
                    case BSFAX:
                        //					sprintf(itemBS, "%s", ML("FAX"));
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("FAX"));
                        break;
                    case BSSMS:
                        //					sprintf(itemBS, "%s", ML("SMS"));
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("SMS"));
                        break;
                    case BSDCS:
                        //					sprintf(itemBS, "%s",  ML("Data circuit sync"));
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Data circuit sync"));
                        break;
                    case BSDCA:
                        //					sprintf(itemBS, "%s",  ML("Data circurt async"));
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Data circurt async"));
                        break;
                    case BSDPAC:
                        //					sprintf(itemBS, "%s",  ML("Dedicated packted access"));
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Dedicated packted access"));
                        break;
                    case BSDPAD:
                        //					sprintf(itemBS, "%s",  ML("Dedicated PAD access"));
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Dedicated PAD access"));
                        break;
                    }
                }            
                FormView_Register();
                hCWFormView = CreateWindow(WC_FORMVIEW,
                    NULL,
                    WS_VISIBLE  |WS_CHILD |WS_VSCROLL,
                    rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
                    hWnd, NULL, NULL, (PVOID)&Listtmp);
                if (hCWFormView == NULL)
                    return FALSE;
                hFocus = hCWFormView;
            }
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
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hCWWnd);
		if (pcwinfo != NULL)
		{
			free(pcwinfo);
			pcwinfo = NULL;
		}
		hCWFormView = NULL;
        hCWWnd = NULL;
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
