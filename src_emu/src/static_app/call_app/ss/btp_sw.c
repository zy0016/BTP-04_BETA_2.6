#include   "winpda.h"
#include  "plx_pdaex.h"
#include  "mullang.h"
#include  "MBPublic.h"
#include  "prebrowhead.h"
#include   "MB_control.h"
#include "benefon\locapi.h"

#define     IDC_ME_GETMOULDERSION  WM_USER + 100
#define     IDC_INIT               WM_USER + 110

#define     MAX_ITEM                180
#define     IDC_OK                  5
#define     IDS_OK                  ML("Ok")
#define     CAP_SW             ML("Software versions")
#define     IDS_LAB_UI         ML("User interface:")
#define     IDS_LAB_GSM        ML("GSM/GPRS module:")
#define     IDS_LAB_GPS        ML("GPS module:")
#define     IDS_LAB_BT         ML("Bluetooth module:")
#define     PLEASEWAITING       ML("Please wait...")

static HINSTANCE        hInstance;
static HWND             hSWWnd;
static HWND             hSWFormView;
static HWND             hFocus;
static int              iBsCounter;
static int              idxBS;
static const char * pClassName      = "SWWndClass";
static const char * pGSMEdition = "SIEMENS\r\nMC55\r\nREVISION 02.06";
static LRESULT  SWAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
extern int GetSoftWareVersion(char * strVersion);
extern int GetGSMVersion(char * strVersion);
extern int GetGPSVersion(char * strVersion);
extern int GetBluetoothVersion(char * strVersion);
static BOOL CreateInfo(HWND hWnd,const char * pVerson);
BOOL SwitchUTF8ToMultiByte(const char * pUTF8String,char * pMultiByte,size_t buflen);

BOOL ShowSWInfo(HWND hPWnd)
{
    WNDCLASS    wc;
	RECT        rClient;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = SWAppWndProc;
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
	
	if (hPWnd != NULL) 
	{
		GetClientRect(hPWnd, &rClient);
		hSWWnd = CreateWindow(pClassName, CAP_SW,
			PWS_STATICBAR, 
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
			hPWnd, NULL, NULL, NULL);
	}
	else
	{
		hSWWnd = CreateWindow(pClassName, CAP_SW,
			PWS_STATICBAR|WS_CAPTION|PWS_NOTSHOWPI, 
			PLX_WIN_POSITION,
			hPWnd, NULL, NULL, NULL);
	}
    if (NULL == hSWWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hSWWnd);
    ShowWindow(hSWWnd, SW_SHOW);
    UpdateWindow(hSWWnd); 
/*
	if (hCBFormView != NULL)
		SetFocus(hCBFormView);
	else
		SetFocus(hCBWnd);*/

    return TRUE;
}


static LRESULT SWAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
    LRESULT     lResult;
    HDC hdc;
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        CreateInfo(hWnd,pGSMEdition);
        //PostMessage(hWnd,IDC_INIT,0,0);
        //WaitWindowStateEx(NULL, TRUE, PLEASEWAITING, "NULL", NULL, NULL);
        hFocus = hSWFormView;
        break;

    case IDC_INIT:
        ME_GetModuleInfo(hWnd,IDC_ME_GETMOULDERSION);
        //WaitWindowStateEx(NULL, TRUE, PLEASEWAITING, "CAP", NULL, NULL);
        break;

    case IDC_ME_GETMOULDERSION:
        //WaitWindowStateEx(NULL, FALSE, PLEASEWAITING, "CAP", NULL, NULL);
        //WaitWindowStateEx(NULL, FALSE, PLEASEWAITING, "NULL", NULL, NULL);
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            {
                char cVerison[50] = "";
                ME_GetResult(cVerison,sizeof(cVerison));
                CreateInfo(hWnd,pGSMEdition);
            }
            break;
        default:
            CreateInfo(hWnd,"");
            break;
        }
        hFocus = hSWFormView;
        SetFocus(hFocus);
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
        {
            SetFocus(hSWFormView);
        }
		else
        {
            PostMessage(hWnd, WM_CLOSE, 0,0);
            hFocus = GetFocus();
        }
        break;		

    case WM_PAINT:
        hdc = BeginPaint(hWnd,NULL);
        //DrawText
        EndPaint(hWnd,NULL);
        break;

    case WM_CLOSE:
		DestroyWindow(hWnd);
        break;
		
    case WM_DESTROY :
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hSWWnd);
		hSWFormView = NULL;
        hSWWnd = NULL;
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
BOOL SwitchUTF8ToMultiByte(const char * pUTF8String,char * pMultiByte,size_t buflen)
{
    char *szMulti = NULL;
    int nLenMulti = 0;
    
    memset(pMultiByte,0x00,buflen);
    nLenMulti = UTF8ToMultiByte(CP_ACP, 0, pUTF8String, -1, NULL, 0, NULL, NULL);
    szMulti = (char *)malloc(sizeof(char) * nLenMulti + 1);
    if (szMulti == NULL)
        return FALSE;

    memset(szMulti,0x00,sizeof(char) * nLenMulti + 1);
    nLenMulti = UTF8ToMultiByte(CP_ACP, 0, pUTF8String, -1, szMulti, nLenMulti, NULL, NULL);
    szMulti[nLenMulti] = '\0';
    strncpy(pMultiByte, szMulti,buflen - 1);
    free(szMulti);
    return TRUE;
}
static BOOL CreateInfo(HWND hWnd,const char * pVerson)
{
    RECT rClient;
    LISTBUF	Listtmp;
    char dspui[50] = "";
    char dspgsm[50] = "";
    char dspgps[100] = "";
    char dspbt[50] = "";
    char dspui1[100] = "",dspgsm1[100] = "",dspgps1[100] = "",dspbt1[100] = "";
    SWVERSION swversion;

    strcpy(dspgsm,pVerson);

    GetClientRect(hWnd, &rClient);
    SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_OK);
    SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
    SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
    PDASetMenu(hWnd, NULL);
    
    GetSoftWareVersion(dspui);
    //GetGSMVersion(dspgsm);
    //GetGPSVersion(dspgps);
    LocGetSWVersion(&swversion);
    strcpy(dspgps,swversion.string);
    GetBluetoothVersion(dspbt);
    
    SwitchUTF8ToMultiByte(dspui,dspui1,sizeof(dspui1));
    SwitchUTF8ToMultiByte(dspgsm,dspgsm1,sizeof(dspgsm1));
    SwitchUTF8ToMultiByte(dspgps,dspgps1,sizeof(dspgps1));
    SwitchUTF8ToMultiByte(dspbt,dspbt1,sizeof(dspbt1));
    
    PREBROW_InitListBuf (&Listtmp);
    PREBROW_AddData(&Listtmp, IDS_LAB_UI, dspui1);
    PREBROW_AddData(&Listtmp, IDS_LAB_GSM, dspgsm1);
    PREBROW_AddData(&Listtmp, IDS_LAB_GPS, dspgps1);
    PREBROW_AddData(&Listtmp, IDS_LAB_BT, dspbt1);
    FormView_Register();
    hSWFormView = CreateWindow(WC_FORMVIEW,
        NULL,
        WS_VISIBLE  |WS_CHILD |WS_VSCROLL,
        rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
        hWnd, NULL, NULL, (PVOID)&Listtmp);
    if (hSWFormView == NULL)
        return FALSE;
    hFocus = hSWFormView;

    return TRUE;
}
