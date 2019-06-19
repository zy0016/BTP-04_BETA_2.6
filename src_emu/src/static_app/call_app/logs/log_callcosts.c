#include    "window.h"
#include    "mullang.h"
#include    "setting.h"
#include    "RcCommon.h"
#include    "plx_pdaex.h"
#include    "MBPublic.h"

#define		EMMAXLEN		8

#define     IDC_BUTTON_BACK         3
#define     IDC_BUTTON_RESET        4
#define     IDC_CCS_LIST             5
#define     CAP_CALLCOSTS        ML("Call costs") 
#define     BUTTON_RESET            (LPARAM)ML("Reset") 
#define     BUTTON_BACK             (LPARAM)ML("Back") 
#define     ERROR1                  ML("Create window fail")
#define     IDS_LASTCALL        ML("Last call:") 
#define     IDS_ALLCALLS              ML("All calls:")   
#define     NOTIFY_OPENING            ML("Opening...")

#define     IDC_ME_RESETACM         WM_USER + 100
#define     IDC_ME_GETACM           WM_USER + 110
#define     IDC_ME_GETLAST          WM_USER + 120
#define     IDC_ME_GETPRICEUNIT     WM_USER + 130
#define     WM_PIN_REMAIN           WM_USER + 140
#define     WM_PIN_QUERY            WM_USER + 150
#define     WM_PIN_CONFIRM          WM_USER + 160
#define     WM_CHECKPIN2            WM_USER + 170
#define     OPRAND  double
typedef enum tag_PIN2_PUK2_STATE{
	FSM_INIT=0,PIN2_NONBLOCKED,PIN2_BLOCKED,PUK2_NONBLOCKED,PUK2_BLOCKED,
	PUK2_INPUT_RIGHT,PUK2_INPUT_ERROR,PIN2_CHANGED,PUK2_INPUT_EXIT
}PIN2_PUK2_FSM;

static const char * pClassName  = "CallCostsWndClass";
static HWND hCCsWnd;
static HWND hPWnd;
static HWND hCCsList;
static  unsigned long   lTotal;
static  unsigned long   lLast;
static  ME_PUCINFO      me_pucinfo;
static  double  dppu;
static BOOL bFirst = TRUE;
static BOOL bPinVerify = TRUE;
static BOOL bSetPin = FALSE;

static  LRESULT CallCostsWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL  CreateControl(HWND hWnd);
static void LoadCallCosts(unsigned long lcunits, unsigned long acunits, double ppc, char * currency, int clen);
static void UpdateCallCosts(unsigned long lcunits, unsigned long acunits, double ppc, char * currency, int clen);

static  BOOL  bshowunit = TRUE;
extern  BOOL ReadCCostSetup(void);
extern  void OprandToStr(OPRAND op, char* buf);
extern  void  CheckPin2Blocked(HWND hFrameWin,HWND hParentWnd,UINT message,const char *szCaption);

BOOL ReadCCostSetup(void)
{
	return TRUE;
}

BOOL CreateCallCostsWindow(HWND hParent)
{
    WNDCLASS    wc;
	RECT  rClient;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = CallCostsWndProc;
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
	hPWnd = hParent;
	GetClientRect(hPWnd, &rClient);
    hCCsWnd = CreateWindow(pClassName,"", 
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hPWnd, NULL, NULL, NULL);
    if (NULL == hCCsWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	ShowWindow(hPWnd, SW_SHOW); 
	UpdateWindow(hPWnd);  
	if (hCCsList != NULL)
		SetFocus(hCCsList);
	else
		SetFocus(hCCsWnd);
    return (TRUE);
}

static LRESULT CallCostsWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT     lResult;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			if (!CreateControl(hWnd))
			{
				AppMessageBox(NULL,ERROR1, "", WAITTIMEOUT);
				return -1;
			}
			bFirst = TRUE;
			bPinVerify = TRUE;
			bSetPin = TRUE;
			bshowunit = ReadCCostSetup();
			LoadCallCosts(0, 0, 0, NULL, 0);
			ME_GetCCM(hWnd,IDC_ME_GETLAST);
			WaitWindowStateEx(NULL, TRUE, NOTIFY_OPENING, NULL, NULL, NULL)	;		
			//lanlan fix
			SendMessage(hCCsList, LB_SETCURSEL, 0, 0);
			//end
		}
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");  
		SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK, BUTTON_BACK );
		SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BUTTON_RESET,1),BUTTON_RESET );
		SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
		SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
		SetWindowText(hPWnd, CAP_CALLCOSTS);
		SetFocus(hWnd);
		break;
//lanlan fix
	case WM_SETFOCUS:
		SetFocus(hCCsList);
		break;
//end
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY :
		hCCsWnd = NULL;
		hCCsList = NULL;
        UnregisterClass(pClassName,NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
			if(bFirst == FALSE)
			{
				bFirst = TRUE;
				break;
			}
			SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);			
			SendMessage(hWnd, WM_CLOSE, 0,0);
            break;
        case VK_RETURN:
			{
				if(GetSIMState() == 0)
				{
					PLXTipsWin(hPWnd, hWnd, 0, ML("Insert SIM card"), "", Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
					break;
				}
				//check if PIN2/PUK2 is blocked       
				CheckPin2Blocked(hPWnd,hWnd,WM_CHECKPIN2,CAP_CALLCOSTS);
			}
            break;
		}
		break;
		
	case WM_CHECKPIN2:
		{
			switch(wParam) 
			{
			case PIN2_NONBLOCKED:
				{	
					char strCode[EMMAXLEN+1];
					strCode[0]= 0;
	
					//Firstly,the user have to input an PIN2 code
					if(SSPLXVerifyPassword(hWnd, NULL, ML("Reset costs.\nEnter PIN2 code:"), 
						strCode, MAKEWPARAM(4,8), ML("Ok"), ML("Cancel"), -1))				
					{					
						if (-1 == ME_ResetACM(hWnd, IDC_ME_RESETACM, strCode))
						{
							bFirst = TRUE;
						}
						else
							WaitWindowState(hWnd,TRUE);
						break;
					}
				}
				break;

			case PIN2_CHANGED:
				{
					char strCode[EMMAXLEN+1];
					strCode[0]= 0;

					if (-1 == ME_ResetACM(hWnd, IDC_ME_RESETACM, strCode))
					{
						bFirst = TRUE;
					}
					else
						WaitWindowState(hWnd,TRUE);
					break;
				}
				break;

			case PUK2_BLOCKED:
				break;
				
			case PUK2_INPUT_EXIT:
				printf("user exit when input PUK2 code\r\n");
				break;
				
			default:
				break;
			}
				
		}
		break;

	case IDC_ME_RESETACM:
		{
			WaitWindowState(hWnd,FALSE);
			switch (wParam)
			{
			case ME_RS_SUCCESS:
				bFirst = TRUE;
				PLXTipsWin(hPWnd, hWnd, 0, ML("Costs reset"), CAP_CALLCOSTS, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);												
				UpdateCallCosts(0, 0, 0, NULL, 0);	
				break;
			default:
				bFirst = TRUE;
				PLXTipsWin(hPWnd, hWnd, 0, ML("PIN2 code\r\nwrong"), CAP_CALLCOSTS, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
				break;
			}
		}
		break;

	case IDC_ME_GETACM:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			ME_GetResult(&lTotal,sizeof(lTotal));
			if (bshowunit)
			{
				UpdateCallCosts(lLast, lTotal, 0, NULL, 0);
				ShowWindow(hPWnd, SW_SHOW); 
				UpdateWindow(hPWnd);  
				if (hCCsList != NULL)
					SetFocus(hCCsList);
				else
					SetFocus(hCCsWnd);
				WaitWindowStateEx(NULL, FALSE, NOTIFY_OPENING, NULL, NULL, NULL);			
			}
			else
			{	
				ME_GetPricePerUnit(hWnd,IDC_ME_GETPRICEUNIT);		
			}
			break;
			
		default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORFARE)
			printf("mb MBRecordFare fail,wParam:%ld,lParam:%d\r\n",wParam,lParam);
#endif            
			lTotal = 0;						
			UpdateCallCosts(lLast, lTotal, 0, NULL, 0);
			ShowWindow(hPWnd, SW_SHOW); 
			UpdateWindow(hPWnd);  
			if (hCCsList != NULL)
				SetFocus(hCCsList);
			else
				SetFocus(hCCsWnd);
			WaitWindowStateEx(NULL, FALSE, NOTIFY_OPENING, NULL, NULL, NULL);			
			break;
		}
		break;
		
	case IDC_ME_GETLAST:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			ME_GetResult(&lLast,sizeof(lLast));
			ME_GetACM(hWnd,IDC_ME_GETACM);
			break;
		default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORFARE)
			printf("mb MBRecordFare fail,wParam:%ld,lParam:%d\r\n",wParam,lParam);
#endif
			lLast = 0;
			ME_GetACM(hWnd,IDC_ME_GETACM);
			break;
		}
		break;
		
	case IDC_ME_GETPRICEUNIT:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			ME_GetResult(&me_pucinfo,sizeof(ME_PUCINFO));
			if (StrIsDouble(me_pucinfo.ppu))
				dppu = MB_atof(me_pucinfo.ppu);
			else
				dppu = 0.0;
			UpdateCallCosts(lLast,lTotal, dppu,me_pucinfo.currency, strlen(me_pucinfo.currency));
			ShowWindow(hPWnd, SW_SHOW); 
			UpdateWindow(hPWnd);  
			if (hCCsList != NULL)
				SetFocus(hCCsList);
			else
				SetFocus(hCCsWnd);
			WaitWindowStateEx(NULL, FALSE, NOTIFY_OPENING, NULL, NULL, NULL);			
			break;

		default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORFARE)
			printf("mb MBRecordFare fail,wParam:%ld,lParam:%d\r\n",wParam,lParam);
#endif
			UpdateCallCosts(0, 0, 0, NULL, 0);
			ShowWindow(hPWnd, SW_SHOW); 
			UpdateWindow(hPWnd);  
			if (hCCsList != NULL)
				SetFocus(hCCsList);
			else
				SetFocus(hCCsWnd);
			WaitWindowStateEx(NULL, FALSE, NOTIFY_OPENING, NULL, NULL, NULL);			
			break;
		}
		break;
		
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static  BOOL  CreateControl(HWND hWnd)
{
	RECT rClient;
	GetClientRect(hWnd, &rClient);
	SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");  
	SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK, BUTTON_BACK );
    SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BUTTON_RESET,1),BUTTON_RESET );
	SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
	SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
	SetWindowText(hPWnd, CAP_CALLCOSTS);
	hCCsList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD|WS_VSCROLL | LBS_NOTIFY | LBS_MULTILINE,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hWnd, (HMENU)IDC_CCS_LIST, NULL, NULL);
    if (hCCsList == NULL)
        return FALSE;
    return TRUE;
}

#define  MAX_ITEMLEN 30
#define  MAX_CURLEN  4
static void LoadCallCosts(unsigned long lcunits, unsigned long acunits, double ppc, char * currency, int clen)
{
	int index = 0;
	char  cCost[MAX_ITEMLEN];
	char  tmpCost[MAX_ITEMLEN-5];
	double tmpprice;
	char cCur[MAX_CURLEN];

	cCost[0]= 0;
	if (currency != NULL && clen > 0) 
	{
		if (clen > MAX_CURLEN) 
		{
			strncpy(cCur, currency, MAX_CURLEN);
			cCur[MAX_CURLEN - 1] = 0;
		}
		else
		{
			strncpy(cCur, currency, clen);
			cCur[clen - 1] = 0;
		}
	}
	else
	{
		cCur[0] = 0;
	}

	index = SendMessage(hCCsList,LB_ADDSTRING,0,(LPARAM)IDS_LASTCALL);
	if(bshowunit)
		sprintf(cCost,"%ld units", lcunits);
	else
	{
		tmpprice = lcunits * ppc;
#ifndef _EMULATE_
		sprintf(cCost, "%5.2f %s", tmpprice, cCur);
#else
		OprandToStr(tmpprice, tmpCost);
		sprintf(cCost, "%s %s", tmpCost, cCur);
#endif

	}
	SendMessage(hCCsList, LB_SETAUXTEXT, MAKEWPARAM(index, MAX_ITEMLEN),  (LPARAM)cCost);
	index = SendMessage(hCCsList,LB_ADDSTRING,0,(LPARAM)IDS_ALLCALLS);
	if(bshowunit)
		sprintf(cCost,"%ld units", acunits);
	else
	{
		tmpprice = acunits * ppc;
#ifndef _EMULATE_
		sprintf(cCost, "%5.2f %s", tmpprice, cCur);
#else
		OprandToStr(tmpprice, tmpCost);
		sprintf(cCost, "%s %s", tmpCost, cCur);
#endif
	}
	SendMessage(hCCsList, LB_SETAUXTEXT, MAKEWPARAM(index, MAX_ITEMLEN),  (LPARAM)cCost);
}

static void UpdateCallCosts(unsigned long lcunits, unsigned long acunits, double ppc, char * currency, int clen)
{
	int index = 0;
	char  cCost[MAX_ITEMLEN];
	char  tmpCost[MAX_ITEMLEN-5];
	double tmpprice;
	char cCur[MAX_CURLEN];

	cCost[0]= 0;
	if (currency != NULL && clen > 0) 
	{
		if (clen > MAX_CURLEN) 
		{
			strncpy(cCur, currency, MAX_CURLEN);
			cCur[MAX_CURLEN - 1] = 0;
		}
		else
		{
			strncpy(cCur, currency, clen);
			cCur[clen - 1] = 0;
		}
	}
	else
	{
		cCur[0] = 0;
	}

//	index = SendMessage(hCCsList,LB_ADDSTRING,0,(LPARAM)IDS_LASTCALL);
	if(bshowunit)
		sprintf(cCost,"%ld units", lcunits);
	else
	{
		tmpprice = lcunits * ppc;
#ifndef _EMULATE_
		sprintf(cCost, "%5.2f %s", tmpprice, cCur);
#else
		OprandToStr(tmpprice, tmpCost);
		sprintf(cCost, "%s %s", tmpCost, cCur);
#endif

	}
	SendMessage(hCCsList, LB_SETAUXTEXT, MAKEWPARAM(0, MAX_ITEMLEN),  (LPARAM)cCost);
//	index = SendMessage(hCCsList,LB_ADDSTRING,0,(LPARAM)IDS_ALLCALLS);
	if(bshowunit)
		sprintf(cCost,"%ld units", acunits);
	else
	{
		tmpprice = acunits * ppc;
#ifndef _EMULATE_
		sprintf(cCost, "%5.2f", tmpprice, cCur);
#else
		OprandToStr(tmpprice, tmpCost);
		sprintf(cCost, "%s %s", tmpCost, cCur);
#endif
	}
	SendMessage(hCCsList, LB_SETAUXTEXT, MAKEWPARAM(1, MAX_ITEMLEN),  (LPARAM)cCost);
}

void MessageUpdateCost(void)
{
	if (IsWindow(hCCsWnd))		
		ME_GetCCM(hCCsWnd,IDC_ME_GETLAST);
}
