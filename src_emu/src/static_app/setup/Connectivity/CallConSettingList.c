 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : connectivity setting
 *
 * Purpose  : setting list of connection setting(GPRS & CSD)
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "CallConSettingList.h"
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "str_plx.h"
#include    "me_wnd.h"
#include    "plx_pdaex.h"
#include    "str_public.h"
#include    "setting.h"
#include    "setup.h"
#include    "mullang.h"
#include    "pubapp.h"

static HWND hWndApp; 
static HWND hFrameWin;

static char * pClassName = "GPRSSettingListClass";

static int  nCurFocus;
static int  nCSD_CurFocus;

static int iCurConIndex;

static UDB_ISPINFO CurConInfo;

static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static int GPRS_CSD_Flag;
static HWND hParentWnd = NULL;
static BOOL CreateControl(HWND hWnd, HWND * hConName, HWND * hBearer, HWND * hAccess_Dial, 
                          HWND * hUserName, HWND * hPassWord, HWND * hAuthen, HWND * hGateway,
                          HWND * hPort, HWND * hHomepage, HWND * hDatacallType, HWND * hMaxDataSpeed);

static BOOL CreateExtraControl(HWND hWnd,HWND * hDatacallType, HWND * hMaxDataSpeed);

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void GPRSConnectSet_OnVScroll ( HWND hWnd,  UINT wParam );
static void GPRSConnectSet_InitVScrolls ( HWND hWnd,int iItemNum );
static void ResetScrollState(HWND hWnd,int iItemNum);

extern BOOL IspReadInfo ( UDB_ISPINFO * uIspInfo ,int iNum );
extern BOOL CallEditConnName(HWND hWndApp,int icurMode);
extern BOOL CallSelConnBearer(HWND hwndCall, HWND hCallbackHwnd, UINT iCallbackMsg, int icurMode);
extern BOOL CallEditGPRSAccPoint(HWND hWndApp,int icurMode);
extern BOOL CallEditConnUserName(HWND hWndApp,int icurMode);
extern BOOL CallEditConnPasswd(HWND hWndApp,int icurMode);
extern BOOL CallSelConnAuthen(HWND hwndCall, int icurMode);
extern BOOL CallEditGateWay(HWND hwndCall, int icurMode);
extern BOOL CallEditConnPort(HWND hwndCall, int icurMode);
extern BOOL CallEditConnHomepage(HWND hwndCall, int icurMode);
extern BOOL CallEditDialupNum(HWND hWndApp,int icurMode);
extern BOOL CallSelConnDatacalTp(HWND hWndApp,int icurMode);
extern BOOL CallSelConnDatacalSpd(HWND hWndApp,int icurMode);
static void OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus);
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);
BOOL IspSaveInfo (UDB_ISPINFO * uIspInfo ,int iNum);
BOOL GPRSSettingList(HWND hWnd, HWND hParent, int GPRSConnectIndex)
{
    WNDCLASS    wc;
    RECT rect;
    
    iCurConIndex = GPRSConnectIndex;
	hParentWnd = hParent;
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;
    
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    hFrameWin = hWnd;

    GetClientRect(hFrameWin, &rect);
    memset(&CurConInfo, 0, sizeof(UDB_ISPINFO));
    IspReadInfo(&CurConInfo, iCurConIndex);
   
    GPRS_CSD_Flag = CurConInfo.DtType;
    
    hWndApp = CreateWindow(pClassName,"", 
        WS_VISIBLE | WS_CHILD | WS_VSCROLL,          
        rect.left,
        rect.top,
        rect.right-rect.left,
        rect.bottom-rect.top,
        hFrameWin,
        NULL, NULL, NULL);
    
    if (NULL == hWndApp)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    
    
    SetWindowText(hFrameWin, CurConInfo.ISPName);

    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
                (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back") );
    
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");

    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;

    SetFocus(hWndApp);

    return (TRUE);    
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HWND        hConName, hBearer, hAccess_Dial, hUserName, hPassWord; 
    static      HWND        hAuthen, hGateway, hPort, hHomepage, hDatacallType, hMaxDataSpeed;
    static      HWND        hFocus = 0;
    static      int         i, iButtonJust, iIndex, iCurIndex;
    static      HWND        hWndFocus;
    DWORD       dwOctet[4] = {0};
    static      char        szGateWayIPAddr[UDB_K_DNS_SIZE+1];
 
    LRESULT     lResult;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{        
    case PWM_SHOWWINDOW:

        IspReadInfo(&CurConInfo, iCurConIndex);

        SendMessage(hConName, SSBM_SETTEXT, 0, (LPARAM)CurConInfo.ISPName);
        SendMessage(hUserName, SSBM_SETTEXT, 0, (LPARAM)CurConInfo.ISPUserName);
        SendMessage(hAuthen, SSBM_SETCURSEL, CurConInfo.AuthenType, 0);
        SendMessage(hPort, SSBM_SETTEXT, 0, (LPARAM)CurConInfo.ConnPort);
        
        SetWindowText(hFrameWin, CurConInfo.ISPName);
        
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
            (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back") );
        
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
        
        SendMessage(hConName, SSBM_DELETESTRING, 0, 0);
        SendMessage(hConName, SSBM_ADDSTRING, 0, (LPARAM)CurConInfo.ISPName);
        
        SendMessage(hGateway, SSBM_DELETESTRING, 0, 0);
        if ( CurConInfo.ConnectGateway != 0 )
        {
            dwOctet[0]=FIRST_IPADDRESS(CurConInfo.ConnectGateway);
            dwOctet[1]=SECOND_IPADDRESS(CurConInfo.ConnectGateway);
            dwOctet[2]=THIRD_IPADDRESS(CurConInfo.ConnectGateway);
            dwOctet[3]=FOURTH_IPADDRESS(CurConInfo.ConnectGateway);
            sprintf(szGateWayIPAddr, "%03d.%03d.%03d.%03d",dwOctet[0],dwOctet[1],dwOctet[2],dwOctet[3]);
            SendMessage(hGateway, SSBM_ADDSTRING, 0, (LPARAM)szGateWayIPAddr);
        }

        SendMessage(hHomepage, SSBM_SETTEXT, 0, (LPARAM)CurConInfo.ConnectHomepage);

        if(CurConInfo.DtType == 1)
        {
            SendMessage(hAccess_Dial, SSBM_SETTITLE, 0, (LPARAM)ML("Access point name"));
            SendMessage(hAccess_Dial, SSBM_SETTEXT, 0, (LPARAM)CurConInfo.ISPPhoneNum1);
            SendMessage(hBearer, SSBM_SETTEXT, 0, (LPARAM)ML("GPRS"));
        }
        else
        {
            SendMessage(hAccess_Dial, SSBM_SETTITLE, 0, (LPARAM)ML("Dialtoup number"));
            SendMessage(hAccess_Dial, SSBM_SETTEXT, 0, (LPARAM)CurConInfo.ISPPhoneNum1);
            SendMessage(hBearer, SSBM_SETTEXT, 0, (LPARAM)ML("Data call"));
        }
        

        if(CurConInfo.DtType == 0)
        {
            if(CurConInfo.DatacallType == 0)
			{
				SendMessage(hDatacallType, SSBM_DELETESTRING, 0, 0);
				SendMessage(hDatacallType, SSBM_ADDSTRING, 0, (LPARAM)ML("Analogue"));
			}
            if(CurConInfo.DatacallType == 1)
			{
				SendMessage(hDatacallType, SSBM_DELETESTRING, 0, 0);
				SendMessage(hDatacallType, SSBM_ADDSTRING, 0, (LPARAM)ML("ISDN v.110"));
			}
            if(CurConInfo.DatacallType == 2)
			{
				SendMessage(hDatacallType, SSBM_DELETESTRING, 0, 0);
				SendMessage(hDatacallType, SSBM_ADDSTRING, 0, (LPARAM)ML("ISDN v.120"));
			}
            
            if(CurConInfo.MaxDataSpeed == 0)
			{
				SendMessage(hMaxDataSpeed, SSBM_DELETESTRING, 0, 0);
				SendMessage(hMaxDataSpeed, SSBM_ADDSTRING, 0, (LPARAM)ML("Automatic"));
			}
            if(CurConInfo.MaxDataSpeed == 1)
			{
				SendMessage(hMaxDataSpeed, SSBM_DELETESTRING, 0, 0);
				SendMessage(hMaxDataSpeed, SSBM_ADDSTRING, 0, (LPARAM)ML("9600 bps"));
			}
            if(CurConInfo.MaxDataSpeed == 2)
			{
				SendMessage(hMaxDataSpeed, SSBM_DELETESTRING, 0, 0);
				SendMessage(hMaxDataSpeed, SSBM_ADDSTRING, 0, (LPARAM)ML("14400 bps"));
			}            
        }
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif

        
        SetFocus(hWnd);
        break;
        
    case WM_VSCROLL:
        GPRSConnectSet_OnVScroll(hWnd,wParam);
        break;
        
    case WM_CREATE :

printf("\r\n ->>>>>> Begin to creat spinbox \r\n");
        memset(szGateWayIPAddr, 0, UDB_K_DNS_SIZE+1);


        CreateControl(hWnd, &hConName, &hBearer, &hAccess_Dial, 
                        &hUserName, &hPassWord, &hAuthen, &hGateway, 
                        &hPort, &hHomepage, &hDatacallType, &hMaxDataSpeed);
printf("\r\n ->>>>>> Creat spinbox success \r\n");
        
        if(CurConInfo.DtType == 1)
            GPRSConnectSet_InitVScrolls(hWnd, 9);
        else
            GPRSConnectSet_InitVScrolls(hWnd, 11);

printf("\r\n ->>>>>> Begin to set the connection name \r\n");

        SendMessage(hConName, SSBM_ADDSTRING, 0, (LPARAM)CurConInfo.ISPName);
        
        if(CurConInfo.DtType == 1)
        {
            SendMessage(hAccess_Dial, SSBM_ADDSTRING, 0, (LPARAM)CurConInfo.ISPPhoneNum1);
            SendMessage(hBearer, SSBM_ADDSTRING, 0, (LPARAM)ML("GPRS"));
        }
        else
        {
            SendMessage(hAccess_Dial, SSBM_ADDSTRING, 0, (LPARAM)CurConInfo.ISPPhoneNum1);
            SendMessage(hBearer, SSBM_ADDSTRING, 0, (LPARAM)ML("Data call"));
        }
   
        SendMessage(hUserName, SSBM_ADDSTRING, 0, (LPARAM)CurConInfo.ISPUserName);
        SendMessage(hAuthen, SSBM_SETCURSEL, CurConInfo.AuthenType, 0);

        if ( CurConInfo.ConnectGateway != 0 )
        {
            dwOctet[0]=FIRST_IPADDRESS(CurConInfo.ConnectGateway);
            dwOctet[1]=SECOND_IPADDRESS(CurConInfo.ConnectGateway);
            dwOctet[2]=THIRD_IPADDRESS(CurConInfo.ConnectGateway);
            dwOctet[3]=FOURTH_IPADDRESS(CurConInfo.ConnectGateway);
            
            sprintf(szGateWayIPAddr, "%03d.%03d.%03d.%03d",dwOctet[0],dwOctet[1],dwOctet[2],dwOctet[3]);
            SendMessage(hGateway, SSBM_ADDSTRING, 0, (LPARAM)szGateWayIPAddr);
        }

        SendMessage(hPort, SSBM_ADDSTRING, 0, (LPARAM)CurConInfo.ConnPort);
        SendMessage(hHomepage, SSBM_ADDSTRING, 0, (LPARAM)CurConInfo.ConnectHomepage);

        if(CurConInfo.DtType == 0)
        {
            if(CurConInfo.DatacallType == 0)
                SendMessage(hDatacallType, SSBM_ADDSTRING, 0, (LPARAM)ML("Analogue"));
            if(CurConInfo.DatacallType == 1)
                SendMessage(hDatacallType, SSBM_ADDSTRING, 0, (LPARAM)ML("ISDN v.110"));
            if(CurConInfo.DatacallType == 2)
                SendMessage(hDatacallType, SSBM_ADDSTRING, 0, (LPARAM)ML("ISDN v.120"));
            
            if(CurConInfo.MaxDataSpeed == 0)
                SendMessage(hMaxDataSpeed, SSBM_ADDSTRING, 0, (LPARAM)ML("Automatic"));
            if(CurConInfo.MaxDataSpeed == 1)
                SendMessage(hMaxDataSpeed, SSBM_ADDSTRING, 0, (LPARAM)ML("9600 bps"));
            if(CurConInfo.MaxDataSpeed == 2)
                SendMessage(hMaxDataSpeed, SSBM_ADDSTRING, 0, (LPARAM)ML("14400 bps"));
            
        }
        hFocus = hConName;
printf("\r\n ->>>>>> Finish to init the value of control \r\n");
        break;
        
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;
	case PWM_ACTIVATE:
		if (wParam == WA_INACTIVE)
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
			wKeyCode   = 0;
		}
		break;
    case WM_DESTROY : 
        hWndApp = NULL;
        UnregisterClass(pClassName,NULL);
        break;
        
    case WM_KEYDOWN:
		if (RepeatFlag > 0 && wKeyCode != LOWORD(wParam))
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
		}

		wKeyCode = LOWORD(wParam);
		RepeatFlag++;
        
		switch(LOWORD(wParam))
        {            
        case VK_F10:
			if (hParentWnd != NULL)
			{
				PostMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hParentWnd,0);
				PostMessage(hParentWnd, WM_CLOSE, 0, 0);
			}
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
		case VK_DOWN:
            SettListProcessKeyDown(hWnd, &hFocus);            
			
		    SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
			if ( 1 == RepeatFlag )
			{
	    		SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
			}
			break;
            
        case VK_UP:
            SettListProcessKeyUp(hWnd, &hFocus);            

		    SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
			if ( 1 == RepeatFlag )
			{
	    		SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
			}
			break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;  
        
    case WM_TIMER:
        OnTimeProcess(hWnd, wParam, hFocus);
        break;
    case WM_KEYUP:
		RepeatFlag = 0;
        switch(LOWORD(wParam))
        {
        case VK_UP:
        case VK_DOWN:
			KillTimer(hWnd, TIMER_ID);
            break;
        default:
            break;
        }        
        break;
	case WM_COMMAND:
        switch( LOWORD( wParam ))
            {
            case IDC_BUTTON_SET:
                switch(iButtonJust)
                {
                case IDC_CONNAME:
                    CallEditConnName(hFrameWin, iCurConIndex);
                    break;
                case IDC_BEARER:
                    CallSelConnBearer(hFrameWin, hWnd, CALLBACK_BEARERCHANGE, iCurConIndex);
                    break;
                case IDC_ACCESS_DIAL:
                    if(CurConInfo.DtType == 1)
                    {
                        CallEditGPRSAccPoint(hFrameWin, iCurConIndex);
                        break;
                    }
                    else
                    {
                        CallEditDialupNum(hFrameWin, iCurConIndex);
                        break;
                    }                

                case IDC_USERNAME:
                    CallEditConnUserName(hFrameWin, iCurConIndex);
                    break;
                case IDC_PASSWORD:
                    CallEditConnPasswd(hFrameWin, iCurConIndex);
                    break;
                case IDC_AUTHEN:
                    CallSelConnAuthen(hFrameWin, iCurConIndex);
                    break;
                case IDC_GATEWAY:
                    CallEditGateWay(hFrameWin, iCurConIndex);
                    break;
                case IDC_PORT:
                    CallEditConnPort(hFrameWin, iCurConIndex);
                    break;
                case IDC_HOMEPAGE:
                    CallEditConnHomepage(hFrameWin, iCurConIndex);
                    break;
                
                case IDC_CSD_DATACALLTYPE:
                    CallSelConnDatacalTp(hFrameWin, iCurConIndex);
                    break;
                case IDC_CSD_MAXDATASPEED:
                    CallSelConnDatacalSpd(hFrameWin, iCurConIndex);
                    break;
                }
                break;
            case IDC_CONNAME:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_CONNAME;
                break;
            case IDC_BEARER:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_BEARER;
                break;
            case IDC_ACCESS_DIAL:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_ACCESS_DIAL;
                break;
            case IDC_USERNAME:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_USERNAME;
                break;
            case IDC_PASSWORD:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_PASSWORD;
                break;
            case IDC_AUTHEN:
				if(HIWORD( wParam ) == SSBN_CHANGE)
				{
					CurConInfo.AuthenType = SendMessage(hAuthen, SSBM_GETCURSEL, 0, 0);
					IspSaveInfo(&CurConInfo, iCurConIndex);
				}
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_AUTHEN;
                break;
            case IDC_GATEWAY:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_GATEWAY;
                break;
            case IDC_PORT:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_PORT;
                break;
            case IDC_HOMEPAGE:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_HOMEPAGE;
                break;
            case IDC_CSD_DATACALLTYPE:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_CSD_DATACALLTYPE;
                break;
            case IDC_CSD_MAXDATASPEED:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_CSD_MAXDATASPEED;
                break;
            }
            break;

    case CALLBACK_BEARERCHANGE:
        if(wParam == GPRS_CSD_Flag) //bearer not changed
            break; 
        else
        {
            if(wParam == 1)//change to GPRS
            {
                GPRS_CSD_Flag = 1;
                DestroyWindow(hDatacallType);
                DestroyWindow(hMaxDataSpeed);
                ResetScrollState(hWnd, 9);
                break;
            }
            else //change to CSD
            {
                GPRS_CSD_Flag = 0;
                CreateExtraControl(hWnd, &hDatacallType, &hMaxDataSpeed);
                ResetScrollState(hWnd, 11);
                break;
            }
        }

            
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
static BOOL CreateExtraControl(HWND hWnd,HWND * hDatacallType, HWND * hMaxDataSpeed)
{
	int xzero=0,yzero=0;
    int iControlH,iControlW;
    RECT rect;
    GetClientRect(hWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;
    
    * hDatacallType = CreateWindow( "SPINBOXEX", ML("Data call type"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 9, iControlW, iControlH, 
        hWnd, (HMENU)IDC_CSD_DATACALLTYPE, NULL, NULL);
    
    if (* hDatacallType == NULL)
        return FALSE;
    
    * hMaxDataSpeed = CreateWindow( "SPINBOXEX", ML("Max. data speed"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 10, iControlW, iControlH, 
        hWnd, (HMENU)IDC_CSD_MAXDATASPEED, NULL, NULL);
    
    if (* hMaxDataSpeed == NULL)
        return FALSE;

	if(CurConInfo.DtType == 0)
	{
		if(CurConInfo.DatacallType == 0)
		{
			SendMessage(*hDatacallType, SSBM_ADDSTRING, 0, (LPARAM)ML("Analogue"));
		}
		if(CurConInfo.DatacallType == 1)
		{
			SendMessage(*hDatacallType, SSBM_ADDSTRING, 0, (LPARAM)ML("ISDN v.110"));
		}
		if(CurConInfo.DatacallType == 2)
		{
			SendMessage(*hDatacallType, SSBM_ADDSTRING, 0, (LPARAM)ML("ISDN v.120"));
		}
		if(CurConInfo.MaxDataSpeed == 0)
		{
			SendMessage(*hMaxDataSpeed, SSBM_ADDSTRING, 0, (LPARAM)ML("Automatic"));
		}
		if(CurConInfo.MaxDataSpeed == 1)
		{
			SendMessage(*hMaxDataSpeed, SSBM_ADDSTRING, 0, (LPARAM)ML("9600 bps"));
		}
		if(CurConInfo.MaxDataSpeed == 2)
		{
			SendMessage(*hMaxDataSpeed, SSBM_ADDSTRING, 0, (LPARAM)ML("14400 bps"));
		}
		
	}	
	return TRUE;
}

static BOOL CreateControl(HWND hWnd, HWND * hConName, HWND * hBearer, HWND * hAccess_Dial, 
                          HWND * hUserName, HWND * hPassWord, HWND * hAuthen, HWND * hGateway,
                          HWND * hPort, HWND * hHomepage, HWND * hDatacallType, HWND * hMaxDataSpeed)
{
	int xzero=0,yzero=0;
    int iControlH,iControlW;
    RECT rect;
    GetClientRect(hWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;


    * hConName = CreateWindow( "SPINBOXEX", ML("Connection name"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hWnd, (HMENU)IDC_CONNAME, NULL, NULL);

    if (* hConName == NULL)
        return FALSE;

    * hBearer = CreateWindow( "SPINBOXEX", ML("Bearer"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH, iControlW, iControlH, 
        hWnd, (HMENU)IDC_BEARER, NULL, NULL);

    if (* hBearer == NULL)
        return FALSE;

    if(CurConInfo.DtType == 1)
    {
        * hAccess_Dial = CreateWindow( "SPINBOXEX", ML("Access point name"), //
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
            xzero, yzero + iControlH * 2, iControlW, iControlH, 
            hWnd, (HMENU)IDC_ACCESS_DIAL, NULL, NULL);

        if (* hAccess_Dial == NULL)
            return FALSE;
        
    }
    else
    {
        * hAccess_Dial = CreateWindow( "SPINBOXEX", ML("Dialtoup number"), //
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
            xzero, yzero + iControlH * 2, iControlW, iControlH, 
            hWnd, (HMENU)IDC_ACCESS_DIAL, NULL, NULL);
        
        if (* hAccess_Dial == NULL)
            return FALSE;

    }
    
    * hUserName = CreateWindow( "SPINBOXEX", ML("User name"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 3, iControlW, iControlH, 
        hWnd, (HMENU)IDC_USERNAME, NULL, NULL);

    if (* hUserName == NULL)
        return FALSE;

    * hPassWord = CreateWindow( "SPINBOXEX", ML("Password"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 4, iControlW, iControlH, 
        hWnd, (HMENU)IDC_PASSWORD, NULL, NULL);

    if (* hPassWord == NULL)
        return FALSE;

    * hAuthen = CreateWindow( "SPINBOXEX", ML("Authentication"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 5, iControlW, iControlH, 
        hWnd, (HMENU)IDC_AUTHEN, NULL, NULL);

    if (* hAuthen == NULL)
        return FALSE;

    SendMessage(* hAuthen, SSBM_ADDSTRING, 0, (LPARAM)ML("Secure"));
    SendMessage(* hAuthen, SSBM_ADDSTRING, 0, (LPARAM)ML("ConnNormal"));

    * hGateway = CreateWindow( "SPINBOXEX", ML("Gateway"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 6, iControlW, iControlH, 
        hWnd, (HMENU)IDC_GATEWAY, NULL, NULL);

    if (* hGateway == NULL)
        return FALSE;

    * hPort = CreateWindow( "SPINBOXEX", ML("Port"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 7, iControlW, iControlH, 
        hWnd, (HMENU)IDC_PORT, NULL, NULL);

    if (* hPort == NULL)
        return FALSE;

    * hHomepage = CreateWindow( "SPINBOXEX", ML("Homepage"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero + iControlH * 8, iControlW, iControlH, 
        hWnd, (HMENU)IDC_HOMEPAGE, NULL, NULL);

    if (* hHomepage == NULL)
        return FALSE;

    if(CurConInfo.DtType == 0)//CSD
    {
        * hDatacallType = CreateWindow( "SPINBOXEX", ML("Data call type"), //
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
            xzero, yzero + iControlH * 9, iControlW, iControlH, 
            hWnd, (HMENU)IDC_CSD_DATACALLTYPE, NULL, NULL);
        
        if (* hDatacallType == NULL)
            return FALSE;
        
        * hMaxDataSpeed = CreateWindow( "SPINBOXEX", ML("Max. data speed"), //
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
            xzero, yzero + iControlH * 10, iControlW, iControlH, 
            hWnd, (HMENU)IDC_CSD_MAXDATASPEED, NULL, NULL);
        
        if (* hMaxDataSpeed == NULL)
            return FALSE;
        
    }
    return TRUE;
}
static void ResetScrollState(HWND hWnd,int iItemNum)
{
    static SCROLLINFO   vsi;

    memset(&vsi, 0, sizeof(SCROLLINFO));

    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 3;
    vsi.nMax   = (iItemNum-1);
    vsi.nPos   = 0;

    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
	nCurFocus =1;
	return;
}
/*********************************************************************\
* Function        GPRSConnectSet_InitVScrolls
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
static void GPRSConnectSet_InitVScrolls(HWND hWnd,int iItemNum)
{
    static SCROLLINFO   vsi;

    memset(&vsi, 0, sizeof(SCROLLINFO));

    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 3;
    vsi.nMax   = (iItemNum-1);
    vsi.nPos   = 0;

    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
	nCurFocus =0;
	return;
}
/*********************************************************************\
* Function        GPRSConnectSet_OnVScroll
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/

static void GPRSConnectSet_OnVScroll(HWND hWnd,  UINT wParam)
{
    static int  nY;
    static RECT rcClient;
    static SCROLLINFO      vsi;

    switch(wParam)
    {
    case SB_LINEDOWN:
		memset(&vsi, 0, sizeof(SCROLLINFO));
		
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus++;
		
        if(nCurFocus > vsi.nMax)	
        {
            ScrollWindow(hWnd,0,(vsi.nMax-vsi.nPage+1)*nY,NULL,NULL);	
            UpdateWindow(hWnd);
            nCurFocus = 0;
			vsi.nPos = 0;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
	
        if(((int)(vsi.nPos + vsi.nPage - 1) <= nCurFocus) && nCurFocus != vsi.nMax)	//modified for UISG
        { 
            ScrollWindow(hWnd,0,-nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos++;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
		break;
		
    case SB_LINEUP:
		
        memset(&vsi, 0, sizeof(SCROLLINFO));
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
		
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus--;
		
        if(nCurFocus < vsi.nMin)	
        {
            ScrollWindow(hWnd,0,(vsi.nPage-vsi.nMax-1)*nY,NULL,NULL);
			
            UpdateWindow(hWnd);
			nCurFocus = vsi.nMax;
		    vsi.nPos = vsi.nMax-vsi.nPage+1;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;	
        }
		
        if((int)vsi.nPos == nCurFocus && nCurFocus != vsi.nMin)	//modified for UISG
        { 
            ScrollWindow(hWnd,0,nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos--;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);   
            break;	
        }
		break;
    case SB_PAGEDOWN:
        break;
    case SB_PAGEUP:
        break;
    default:
        break;	
    }	
}
static void OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus)
{
	    switch(wParam)
		{
		case TIMER_ID:
            if (1 == RepeatFlag)
            {
                KillTimer(hWnd, TIMER_ID);
                SetTimer(hWnd, TIMER_ID, ET_REPEAT_LATER, NULL);
            }
			keybd_event(wKeyCode, 0, 0, 0);
			break;
       default:
			KillTimer(hWnd, wParam);
            break;
		}

}

