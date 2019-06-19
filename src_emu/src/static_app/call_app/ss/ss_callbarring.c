#include   "winpda.h"
#include  "plx_pdaex.h"
#include  "mullang.h"
#include  "MBPublic.h"
#include  "prebrowhead.h"
#include   "MB_control.h"

#define     MAX_ITEM                180
#define     IDC_OK                  5
#define     IDS_OK                  ML("Ok")
#define     CAP_CALLBARRING          "Call barring active"
#define     IDS_LAB_TYPE            ML("Type:")
#define     IDS_LAB_SERVICE         ML("Service:")
#define     CAP_NETWORKSERVICE      ML("Network service")

static HINSTANCE        hInstance;
static HWND             hCBWnd;
static HWND             hCBFormView;
static HWND             hFocus;
static int              iBsCounter;
static int              idxBS;
static const char * pClassName      = "CBWndClass";
static CBINFO * pcbinfo = NULL;
static LRESULT  CBAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
BOOL ShowCBInfo(HWND hPWnd, CBINFO * pcf, int len)
{
    WNDCLASS    wc;
	RECT        rClient;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = CBAppWndProc;
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
		pcbinfo = malloc(len);
		if (pcbinfo == NULL)
			return FALSE;
		memcpy(pcbinfo, pcf, len);
		iBsCounter = len /sizeof(CBINFO);
	}
	else
		return FALSE;
	if (hPWnd != NULL) 
	{
		GetClientRect(hPWnd, &rClient);
		hCBWnd = CreateWindow(pClassName, CAP_CALLBARRING,
			PWS_STATICBAR, 
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
			hPWnd, NULL, NULL, NULL);
	}
	else
	{
		hCBWnd = CreateWindow(pClassName, CAP_CALLBARRING,
			PWS_STATICBAR|WS_CAPTION|PWS_NOTSHOWPI, 
			PLX_WIN_POSITION,
			hPWnd, NULL, NULL, NULL);
	}
    if (NULL == hCBWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hCBWnd);
    ShowWindow(hCBWnd, SW_SHOW);
    UpdateWindow(hCBWnd); 
/*
	if (hCBFormView != NULL)
		SetFocus(hCBFormView);
	else
		SetFocus(hCBWnd);*/

    return TRUE;
}


static LRESULT CBAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
    LRESULT     lResult;
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			LISTBUF	Listtmp;
			int i,j;
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
			while ((!pcbinfo[i].Active) && ( i < iBsCounter)) 
			{
				i++;
			}
			if (i == iBsCounter)
			{
				PLXTipsWin(NULL, NULL, NULL, ML("Call barring\nnot active"), CAP_NETWORKSERVICE, Notify_Info, ML("Ok"), NULL, -1);
				return -1;
			}
			else
			{
				idxBS = i;			
				PREBROW_InitListBuf (&Listtmp);
				switch (pcbinfo[idxBS].Fac)
				{
				case BAOC:
					PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("All outgoing"));
					break;
				case BOIC:
					PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("All outgoing int'l"));
					break;
				case BOIC_exHC:
					PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, "All outgoing int'l \nexcept home");				
					break;
				case BAIC:
					PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("All incoming"));
					break;
				case BIC_Roam:
					PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, "All incoming when \nroaming");
					break;
				}
                j = 0;
                //PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, itemBS);
                memset(itemBS,0x00,sizeof(itemBS));
                while (j < iBsCounter)
                {
                    idxBS = j++;
                    if (!pcbinfo[idxBS].Active)
                        continue;
                    switch (pcbinfo[idxBS].BS)
                    {
                    case BSVOICE:
                        strcat(itemBS,ML("Voice"));
                        //sprintf(itemBS, "%s", ML("Voice"));
                        //					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Voice"));
                        break;
                    case BSDATA:
                        strcat(itemBS,ML("All data"));
                        //sprintf(itemBS, "%s", ML("FAX and SMS"));
                        //					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("FAX and SMS"));
                        break;
                    case BSFAX:
                        strcat(itemBS,ML("FAX"));
                        //sprintf(itemBS, "%s", ML("FAX"));
                        //					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("FAX"));
                        break;
                    case BSSMS:
                        strcat(itemBS,ML("SMS"));
                        //sprintf(itemBS, "%s", ML("SMS"));
                        //					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("SMS"));
                        break;
                    case BSDCS:
                        strcat(itemBS,ML("Data circuit sync"));
                        //sprintf(itemBS, "%s",  ML("Data circuit sync"));
                        //					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Data circuit sync"));
                        break;
                    case BSDCA:
                        strcat(itemBS,ML("Data circurt async"));
                        //sprintf(itemBS, "%s",  ML("Data circurt async"));
                        //					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Data circurt async"));
                        break;
                    case BSDPAC:
                        strcat(itemBS,ML("Dedicated packted access"));
                        //sprintf(itemBS, "%s",  ML("Dedicated packted access"));
                        //					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Dedicated packted access"));
                        break;
                    case BSDPAD:
                        strcat(itemBS,ML("Dedicated PAD access"));
                        //sprintf(itemBS, "%s",  ML("Dedicated PAD access"));
                        //					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Dedicated PAD access"));
                        break;
                    }
                    strcat(itemBS,"\n");
                }
                PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, itemBS);
				FormView_Register();
				hCBFormView = CreateWindow(WC_FORMVIEW,
					NULL,
					WS_VISIBLE  |WS_CHILD |WS_VSCROLL,
					rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
					hWnd, NULL, NULL, (PVOID)&Listtmp);
				if (hCBFormView == NULL)
					return FALSE;
				hFocus = hCBFormView;
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
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hCBWnd);
		if (pcbinfo != NULL)
		{
			free(pcbinfo);
			pcbinfo = NULL;
		}
		hCBFormView = NULL;
        hCBWnd = NULL;
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
