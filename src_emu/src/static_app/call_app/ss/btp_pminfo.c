#include   "winpda.h"
#include  "plx_pdaex.h"
#include  "mullang.h"
#include  "MBPublic.h"
#include  "prebrowhead.h"
#include   "MB_control.h"

#define     MAX_ITEM                180
#define     IDC_OK                  5
#define     IDS_OK                  ML("Ok")
#define     CAP_PM                  ML("Battery info")
#define     IDS_LAB_STATUS          ML("Charging status:")
#define     IDS_LAB_CAPACITY        ML("Battery capacity:")
#define     IDS_LAB_CONSUMPTION     ML("Power consumption:")
#define     IDS_LAB_VOLTAGE         ML("Voltage:")

static HINSTANCE        hInstance;
static HWND             hPMInfoWnd;
static HWND             hPMInfoFormView;
static HWND             hFocus;
static int              iBsCounter;
static int              idxBS;
static const char * pClassName      = "PMInfoWndClass";
static ChargeCtl_t * pPMInfoinfo = NULL;
static LRESULT  PMInfoAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
extern int GetBatValue(void);

BOOL ShowPMInfo(HWND hPWnd, ChargeCtl_t * pcc, int len)
{
    WNDCLASS    wc;
	RECT        rClient;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = PMInfoAppWndProc;
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
	if (pcc != NULL)
	{
		pPMInfoinfo = malloc(len+1);
		if (pPMInfoinfo == NULL)
			return FALSE;
		memcpy(pPMInfoinfo, pcc, len);
	}
	else
		return FALSE;
	
	if (hPWnd != NULL) 
	{
		GetClientRect(hPWnd, &rClient);
		hPMInfoWnd = CreateWindow(pClassName, CAP_PM,
			PWS_STATICBAR, 
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
			hPWnd, NULL, NULL, NULL);
	}
	else
	{
		hPMInfoWnd = CreateWindow(pClassName, CAP_PM,
			PWS_STATICBAR|WS_CAPTION|PWS_NOTSHOWPI, 
			PLX_WIN_POSITION,
			hPWnd, NULL, NULL, NULL);
	}
    if (NULL == hPMInfoWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hPMInfoWnd);
    ShowWindow(hPMInfoWnd, SW_SHOW);
    UpdateWindow(hPMInfoWnd); 
/*
	if (hCBFormView != NULL)
		SetFocus(hCBFormView);
	else
		SetFocus(hCBWnd);*/

    return TRUE;
}


static LRESULT PMInfoAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
    LRESULT     lResult;
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			LISTBUF	Listtmp;
			int vol;
			char dspstatus[30] = "";
			char dspcap[30] = "";
			char dsppower[30] = "";
			char dspvol[30] = "";
			static const char *pPec ="%"; 
			GetClientRect(hWnd, &rClient);
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_OK);
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
			PDASetMenu(hWnd, NULL);
			switch(pPMInfoinfo->ChargeStatus)
			{
			case CHG_OFF:
				strcpy(dspstatus, ML("Charger not\nconnected"));
				break;
			case CHG_CONNECT:
				strcpy(dspstatus, ML("Charger connected"));
				break;
			case CHG_INPROG:
				strcpy(dspstatus, ML("Charging"));
				break;
			case CHG_FINISH:
				strcpy(dspstatus, ML("Charging finished"));
				break;
			case CHG_HIGHTMP:
				strcpy(dspstatus, ML("Temperature out of range"));
				break;
			case CHG_ERROR:
			default:
				strcpy(dspstatus, ML("Charging error"));
				break;
			}
			sprintf(dspcap, "%d%s", pPMInfoinfo->BatteryCapacity, pPec);
			sprintf(dsppower, "%d mA", pPMInfoinfo->PowerConsumption);
			vol = GetBatValue();
			sprintf(dspvol, "%d mV", vol);
			PREBROW_InitListBuf (&Listtmp);
			PREBROW_AddData(&Listtmp, IDS_LAB_STATUS, dspstatus);
			PREBROW_AddData(&Listtmp, IDS_LAB_CAPACITY, dspcap);
			PREBROW_AddData(&Listtmp, IDS_LAB_CONSUMPTION, dsppower);
			PREBROW_AddData(&Listtmp, IDS_LAB_VOLTAGE, dspvol);
			FormView_Register();
			hPMInfoFormView = CreateWindow(WC_FORMVIEW,
				NULL,
				WS_VISIBLE  |WS_CHILD |WS_VSCROLL,
				rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
				hWnd, NULL, NULL, (PVOID)&Listtmp);
			if (hPMInfoFormView == NULL)
				return FALSE;
			hFocus = hPMInfoFormView;
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
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPMInfoWnd);
		if (pPMInfoinfo != NULL)
		{
			free(pPMInfoinfo);
			pPMInfoinfo = NULL;
		}
		hPMInfoFormView = NULL;
        hPMInfoWnd = NULL;
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
