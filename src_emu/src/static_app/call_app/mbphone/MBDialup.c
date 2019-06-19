#include    "MBDialup.h"
#include    "pubapp.h"
#include    "MB_control.h"

#define DEBUG_MBDIALUP

#define TIMER_ID_FRESH      2
#define EN_EMPTYTEXT    0x0505
static const enum BtnState
{
    BTN_UP,     
    BTN_DOWN    
};
static const char * pClassName = "MBDialupWndClass";
static HINSTANCE    hInstance;
static char         cPhoneNumber[PHONENUMMAXLEN + 1];
static HBITMAP      hBmpNumberLarge = NULL; 
static HBITMAP      hBmpNumberSmall = NULL; 

static int          iPicNumberHeightLarge;  
static int          iPicNumberHeightSmall;  
static int          iNumberLinesSmall;      
static int          iNumberLinesLarge;      
static DIALITEMBTN  dialitembtn[BTN_NUM];   
static HWND         hMBDialUp;              
static HWND         hwndDialupImeEdit;
static HWND         hFocus;
static BOOL         bShow;

static int          iChWidthLarge;          
static int          iChWidthSmall;          
static int          iNumberCountLarge;      
static int          iNumberCountSmall;
static BOOL         beToLink = FALSE;      
static BOOL         bEmergency = FALSE;//是否在锁键盘的时候紧急拨号
/*
static const char * pEmergentNumber[] =     
{
    "110",
	"114",
	"119",
	"112",
	"120",
	"911",
    ""
};*/
static char * pEmergentNumber[] =     
{
	"112",
	"911",
    "999",
    "08",
    ""
};

static const char * pProjectMode        = "*#38266#";//"2003P2P";
static const char * pCh_Num0            = "0";
static const char * pCh_Num1            = "1";
static const char * pCh_Num2            = "2";
static const char * pCh_Num3            = "3";
static const char * pCh_Num4            = "4";
static const char * pCh_Num5            = "5";
static const char * pCh_Num4Asterick    = "4*";

static const char   cCh_Well            = '#';
static const char   cCh_Plus            = '+';
static const char   cCh_Asterisk        = '*';
static const char   cCh_Num0            = '0';
static const char   cCh_Num9            = '9';
static const char   cCh_None            = '\0';

static const int    iLINENUMBERNUMSMALL = 10;
static const int    iLINENUMBERNUMLARGE = 6;
static const int    iNUMBERTOPY         = 2;
static const int    iNUMBERTOPX         = 5;
static const int    iNUMBERLINESTEP     = 3;


static const char * pPICNUMBERLARGE = "/rom/mbphone/dialnuml.gif";
static const char * pPICNUMBERSMALL = "/rom/mbphone/dialnums.gif";


static HWND hPWnd;
static HWND hPreWnd;
static BOOL blockemerg;
static BOOL bSuperAdd;
static BOOL bSuperAddDial;           
static BOOL bAddCanSendNumber;      
static BOOL bSpecial;    // for other application to call 
static BOOL bNumber;
static BOOL bFindInSIMPB = FALSE;
static BOOL FindHash(char * ptr);
static BOOL APPFRAME_CallPhoneWindow(HWND hframe, const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend);
static BOOL MBDialup_OnCreate(HWND hWnd);
static void MBDialup_OnShowWindow(HWND hWnd);
static void MBDialup_OnKeyDown(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
static void MBDialup_OnCommand(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
static void MBDialup_OnTimer(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
static void MBDialup_CloseWnd(HWND hWnd, BOOL beCreatNew);
static LRESULT DialupAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
BOOL IsTimeChanged(void);
void NotifyEmergencyCall();
extern HWND GetCallFrameHandle();
extern BOOL GetChangeLockStatus();
extern void SetChangeLockStatus();
extern int		Matchstr(char **src,char *dec);
extern void SetShowSummaryFlag(BOOL f);
extern void SetMMIHangUpFlag(BOOL f);
extern BOOL CallAppEntryEx (PSTR pAppName, WPARAM wparam, LPARAM lparam);
/********************************************************************
* Function   APP_CallPhone  
* Purpose    Idle to call
* Params    
* Return    
* Remarks   
**********************************************************************/
BOOL APP_CallPhone(const char * pPhoneNumber)
{
//	AppActiveCallApp(CALL_DIAL);  // APP_ACITVE to call APP_Control
    if (IsWindow(hMBDialUp))
    {
        PostMessage(hMBDialUp,WM_CLOSE,0,0);
        return FALSE;
    }
    else
    {
        hPWnd=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR|PWS_NOTSHOWPI );
    }
	hPreWnd = NULL;
//	Set_CallWndOrder(hPreWnd, hPWnd);
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hPWnd);
    if (APPFRAME_CallPhoneWindow(hPWnd, pPhoneNumber,FALSE,TRUE))	
//    if (APP_CallPhoneWindow(pPhoneNumber,FALSE,TRUE))
    {
        bSpecial = FALSE; // set special dial flag false
        return TRUE;
    }
    return FALSE;
}
/********************************************************************
* Function   SuperAddDialNumber  
* Purpose   Add dial interface 
* Params    
* Return    
* Remarks   
\**********************************************************************/
BOOL SuperAddDialNumber(HWND hpre, const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend, WORD keycode)
{
	if (!IsWindow(hPWnd))
	{
		hPWnd=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR|PWS_NOTSHOWPI  );
		hPreWnd = hpre;
//		Set_CallWndOrder(hPreWnd, hPWnd);
		CALLWND_InsertDataByHandle(pGlobalCallListBuf, hPWnd);
		if (APPFRAME_CallPhoneWindow(hPWnd, pPhoneNumber,bSuperAdd,bCanSend))
			//    if (APP_CallPhoneWindow(pPhoneNumber,bSuperAdd,bCanSend))
		{
			bSpecial = FALSE;
			return TRUE;
		}
	}
	else
	{
		Set_CallWndOrder(hPreWnd, hPWnd);
		MoveWindowToTop(hPWnd);
		CALLWND_InsertDataByHandle(pGlobalCallListBuf, hPWnd);
		ShowWindow(hPWnd, SW_SHOW); 
		UpdateWindow(hPWnd);  
		if (hwndDialupImeEdit != NULL)
		{
			SetFocus(hwndDialupImeEdit);
			keybd_event(keycode, 0, 0, 0);
		}
		else
			SetFocus(hMBDialUp);
		
	}

    return FALSE;
}
/*
BOOL SuperAddDialNumber(const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend)
{
    if (APP_CallPhoneWindow(pPhoneNumber,bSuperAdd,bCanSend))
    {
        bSpecial = FALSE;
        return TRUE;
    }
    return FALSE;
}*/


/********************************************************************
* Function   APP_CallPhoneSpecial  
* Purpose   
* Params    
* Return    
* Remarks   
\**********************************************************************/

BOOL    APP_CallPhoneSpecial(const char * pPhoneNumber)
{ 
	hPWnd=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR|PWS_NOTSHOWPI  );
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hPWnd);
    if (APPFRAME_CallPhoneWindow(hPWnd, pPhoneNumber,FALSE,TRUE))		
		//    if (APP_CallPhoneWindow(pPhoneNumber,FALSE,TRUE))
    {
        bSpecial = TRUE;
        SendMessage(hMBDialUp,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)"");
        return TRUE;
    }
    return FALSE;
}

/********************************************************************
* Function   APP_CallPhoneNumber  
* Purpose   Other application to call
* Params    
* Return    
* Remarks   
\**********************************************************************/

BOOL    APP_CallPhoneNumber(const char * pPhoneNumber)
{
    RESULT_BYSEND iSendNumber;

    bSuperAddDial = FALSE;
    bSpecial      = FALSE;

    iSendNumber = DealWithNumberBySend(pPhoneNumber);
    switch (iSendNumber)
    {
    case RESULT_SUCC:       
    case RESULT_GSM:        
        return TRUE;
    case RESULT_NOINIT:     
    case RESULT_PHONENUM:   
    case RESULT_QUICKDIAL:  
    case RESULT_ERRORNUMBER:
    case RESULT_NONESIM:    
    case RESULT_WILDCARD:
        return FALSE;
    }
    return TRUE;
}
BOOL    APP_CallPhoneNumberSTK(const char * pPhoneNumber)
{
	SYSTEMTIME  sDialupTime;
	PHONEITEM PhoneItem;

    bSuperAddDial = FALSE;
    bSpecial      = FALSE;

    if (pPhoneNumber == NULL)
		return FALSE;
	if (strlen(pPhoneNumber) == 0)
		return FALSE;

	GetLocalTime(&sDialupTime);
	memset(&PhoneItem,0x00,sizeof(PHONEITEM));
	
	PhoneItem.Calls_Info.index = 1;
	PhoneItem.Calls_Info.Stat = CALL_STAT_ACTIVE;
	PhoneItem.Calls_Info.Mode = CALL_MODE_VOICE;
	PhoneItem.Calls_Info.Dir = CALL_DIR_ORIGINATED;
	PhoneItem.Calls_Info.Mpty = 0;
	strncpy(PhoneItem.Calls_Info.PhoneNum,pPhoneNumber,
		sizeof(PhoneItem.Calls_Info.PhoneNum) - 1);

	memcpy(&PhoneItem.TimeInDial,&sDialupTime,sizeof(SYSTEMTIME));
	MBCallingWindow(NULL, &PhoneItem,NULL,FALSE);
    return TRUE;
}
static  BOOL    APPFRAME_CallPhoneWindow(HWND hframe, const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend)
{
	RECT        rClient;
    WNDCLASS    wc;
    int         iSour;
	
	if (IsWindow(hMBDialUp))
    {
        PostMessage(hMBDialUp,WM_CLOSE,0,0);
        return FALSE;
//		ShowWindow(hPWnd, SW_SHOW); 
//		UpdateWindow(hPWnd);  
//		//SetFocus(hMBDialUp); 
//        if (hwndDialupImeEdit != NULL)
//            SetFocus(hwndDialupImeEdit);
//        else
//            SetFocus(hMBDialUp); 
//		return TRUE;
    }

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = DialupAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;
    
    if(!RegisterClass(&wc))
    {
        return FALSE;
    }
    memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));

    if (pPhoneNumber != NULL)
    {
        iSour = strlen(pPhoneNumber);
        if (iSour > 0)
            strncpy(cPhoneNumber,pPhoneNumber,sizeof(cPhoneNumber) > iSour ? iSour + 1 : sizeof(cPhoneNumber) - 1);
    }

    bSuperAddDial       = bSuperAdd;
    bAddCanSendNumber   = bCanSend;
	GetClientRect(hPWnd, &rClient);
    hMBDialUp = CreateWindow(
        pClassName,
        "", 
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hPWnd, NULL, NULL, NULL);

    if (NULL == hMBDialUp)
        return FALSE;
	
	ShowWindow(hPWnd, SW_SHOW); 
	UpdateWindow(hPWnd);  
	if (hwndDialupImeEdit != NULL)
		SetFocus(hwndDialupImeEdit);
	else
		SetFocus(hMBDialUp); 
    return TRUE;
}

static LRESULT DialupAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		lResult = MBDialup_OnCreate(hWnd);
        break;

    case IDC_INIT112:
		PLXTipsWin(NULL, NULL, NULL, EMERGENT, NULL ,Notify_Alert,NULL, NULL,WAITTIMEOUT);
        break;

    case WM_INITMENU:
        if (IsCallingWnd())
        {
            HMENU hmenu;
            hmenu = PDAGetMenu(hPWnd);

            InsertMenu(hmenu,0,MF_BYPOSITION,IDC_SENDDTMF,NOTIFY_SENDDTMF);

            if (GetMuteStatus())
                InsertMenu(hmenu,0,MF_BYPOSITION,IDC_UNMUTE,NOTIFY_UNMUTE);
            else
                InsertMenu(hmenu,0,MF_BYPOSITION,IDC_MUTE,NOTIFY_MUTE);

            if (GetItemTypeNum(CALL_STAT_ACTIVE) > 0)
                InsertMenu(hmenu,0,MF_BYPOSITION,IDC_HOLD,NOTIFY_HOLD);
            else
                InsertMenu(hmenu,0,MF_BYPOSITION,IDC_ACTIVE,NOTIFY_ACTIVE);
        }
        break;

	case PWM_SHOWWINDOW:
		MBDialup_OnShowWindow(hWnd);
		break;

    case WM_CLOSE:        
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY :
		{
			HMENU hmenu;

            printf("\r\n ################## MBDialup destroy###################\r\n");
			if ((GetChangeLockStatus() && (!beToLink)) || bEmergency)
			{
				SetChangeLockStatus();
				DlmNotify(PS_LOCK_ENABLE, TRUE);				
				CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPWnd);
				beToLink = FALSE;
			}
			KillTimer(hWnd, TIMER_ID_FRESH);
			hmenu = (HMENU) GetWindowLong(hWnd, GWL_USERDATA);
			DestroyMenu(hmenu);
			hwndDialupImeEdit =NULL;
			hMBDialUp = NULL;        
			UnregisterClass(pClassName,NULL);

            SMS_Unregister();
            MMS_EntryRegister(NULL,0);

            PostMessage(hPWnd,WM_CLOSE,0,0);
            bEmergency = FALSE;
		}
        break;        

	case WM_EMERGENCY:
			if (DialNumber(hWnd))
			{
				CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPWnd);
				SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
				// add after frame modifyed
				SendMessage(hWnd, WM_CLOSE, 0,0);
			}
		break;

    case WM_KEYDOWN:
		MBDialup_OnKeyDown(hWnd,wMsgCmd, wParam, lParam);
        break;

    case WM_COMMAND:
		MBDialup_OnCommand(hWnd,wMsgCmd, wParam, lParam);
        break;

	case WM_SETRBTNTEXT:
		SendMessage(hPWnd, PWM_SETBUTTONTEXT, 0, lParam); 
		break;

    case WM_TIMER:	
		MBDialup_OnTimer(hWnd,wMsgCmd, wParam, lParam);
        break;

    case IDC_ME_SUPERADDDIAL:
        bSuperAdd = TRUE;
        break;

    case IDC_ME_SHORTCUT:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
        printf("mb MBDialup 快捷操作 wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
        switch (wParam)
        {
        case ME_RS_SUCCESS:
			CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPWnd);
			SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			// add after frame modifyed
			SendMessage(hWnd, WM_CLOSE, 0,0);
           break;
        default:
			SetMMIHangUpFlag(FALSE);
            AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
			CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPWnd);
			SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			// add after frame modifyed
			SendMessage(hWnd, WM_CLOSE, 0,0);
            break;
        }
        break;

    case IDC_ADDNEWCONTACT:
        if (wParam)
        {
            memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
            PostMessage(hWnd,WM_CLOSE,0,0);
        }
        break;

    case IDC_SENDMMSINFO:
    case IDC_SENDSMSINFO:
        if (wParam)
        {
            memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
            PostMessage(hWnd,WM_CLOSE,0,0);
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
/*********************************************************************\
* Function	MBDialup_CloseWnd
* Purpose   close window in 3 conditons
* Params
* Return    
* Remarks
**********************************************************************/
static void MBDialup_CloseWnd(HWND hWnd, BOOL beCreatNew)
{
	HWND hChildLast;
	if (!beCreatNew) 
	{
		hChildLast = GetWindow((HWND)hWnd, GW_HWNDPREV);
		if (hChildLast)
		{
			SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		else
		{
			SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			 // add after frame modifyed
			 SendMessage(hWnd, WM_CLOSE, 0,0);
		}
	}
	else
	{
		SendMessage(hWnd, WM_CLOSE, 0, 0);
	}
}
/*********************************************************************\
* Function	MBDialup_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
* Return    SUCCESS  FAIL
* Remarks
**********************************************************************/
static BOOL MBDialup_OnCreate(HWND hWnd)
{
	HMENU hmenu;
	HMENU hsubmenu, hsubmenu2;
	RECT imesize;
	int  iNumberLen;
	char chCurrent;
	SYSTEMTIME	SystemTime;
    char aTimeBuf[25], aDateBuf[25];

    SMS_Register(hWnd,IDC_SENDSMSINFO);
    MMS_EntryRegister(hWnd,IDC_SENDMMSINFO);
    
	GetLocalTime(&SystemTime);	
	GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
	SetWindowText(hPWnd, aTimeBuf);
	SendMessage(hPWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
	SendMessage(hPWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
	GetClientRect(hWnd, &imesize);
//	if (strlen(cPhoneNumber)==3 && NumberIsEmergent(cPhoneNumber))
	if(Matchstr(pEmergentNumber, cPhoneNumber) == 1)
		blockemerg = TRUE;
	else
		blockemerg = FALSE;
	if (blockemerg == FALSE)
	{
		hwndDialupImeEdit = CreateWindow(
            "CALLEDIT", "",
            WS_VISIBLE | WS_CHILD | ES_MULTILINE,
        	0,0,imesize.right,imesize.bottom,
            (HWND)hWnd,
            (HMENU)IDC_CALLEDIT, NULL, NULL);
	}
	else
	{
		hwndDialupImeEdit = CreateWindow(
            "CALLEDIT", "",
            WS_VISIBLE | WS_CHILD | ES_MULTILINE|ES_READONLY,
        	0,0,imesize.right,imesize.bottom,
            (HWND)hWnd,
            (HMENU)IDC_CALLEDIT, NULL, NULL);
	}        
	if (hwndDialupImeEdit == NULL)
		return FALSE;
	SendMessage(hwndDialupImeEdit, EM_LIMITTEXT, PHONENUMMAXLEN, 0);
    SetWindowText(hwndDialupImeEdit, cPhoneNumber);
	hFocus = hwndDialupImeEdit;
	if (blockemerg == FALSE) 
	{
		if (strlen(cPhoneNumber) == 0)
		{
			SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)"");
			SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);
		}
		else if (strlen(cPhoneNumber) >=1) 
		{
            if (!bEmergency)
			    SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONSAVE);

			SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_CLEAR,BUTTONDELETE);
		}
        if (!bEmergency)
		    SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options"); 
	}
	else
	{
		SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_CALL,1),(LPARAM)IDS_MENU_CALL);
		SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,(LPARAM)BUTTONQUIT);
	}
	hmenu = PDAGetMenu(hPWnd);
	if (( hmenu == NULL) && (!bEmergency))
	{
		hmenu = CreateMenu();
		if (FALSE == PDASetMenu(hPWnd, hmenu))
		{
			DestroyMenu(hmenu);
			hmenu = NULL;
		}
		hsubmenu = CreateMenu();
        
		InsertMenu(hmenu   ,0 ,MF_BYPOSITION|MF_POPUP, (DWORD)hsubmenu ,(LPCSTR)IDS_MENU_SAVE);   
		InsertMenu(hsubmenu,0       ,MF_BYPOSITION        ,IDM_NEWCONTACT,(LPCSTR)IDS_MENU_NEWCONTACT); 
		InsertMenu(hsubmenu,1       ,MF_BYPOSITION        ,IDM_ADDTOCONTACT  ,(LPCSTR)IDS_MENU_ADDTOCONTACT); 
		//			InsertMenu(hmenu   ,1 ,MF_BYPOSITION, IDM_CALL ,(LPCSTR)IDS_MENU_CALL);   
		hsubmenu2 = CreateMenu();
		InsertMenu(hmenu   ,1 ,MF_BYPOSITION|MF_POPUP, (DWORD)hsubmenu2 ,(LPCSTR)IDS_MENU_MESSAGE);   
		InsertMenu(hsubmenu2,0       ,MF_BYPOSITION        ,IDM_SMS,(LPCSTR)IDS_MENU_SMS); 
		InsertMenu(hsubmenu2,1       ,MF_BYPOSITION        ,IDM_MMS  ,(LPCSTR)IDS_MENU_MMS); 				
		
	}
	SetWindowLong(hWnd, GWL_USERDATA, (LONG)hmenu);
	SetTimer(hWnd, TIMER_ID_FRESH, 5000, NULL);
	bNumber = (strlen(cPhoneNumber) != 0);
	if ((bSuperAddDial) && (!GetListCurCalling()) && (!IsAllType_Specific(CALL_STAT_HELD)))
	{
		bSuperAdd   = TRUE;
		iNumberLen  = strlen(cPhoneNumber);
		if (iNumberLen > 0)
		{
			chCurrent = cPhoneNumber[iNumberLen - 1];
            if (chCurrent != '*')
			    ME_SendDTMF(hWnd,IDC_ME_SUPERADDDIAL,chCurrent);
		}
		else
		{
			PostMessage(hWnd,IDC_ME_SUPERADDDIAL,ME_RS_SUCCESS,0);
		}
	}
    if (!GetInitState())//电话部分初始化尚未完成
        MessageMBPostMsgCallForward();//重新开始查询有关呼叫转移的信息
	return TRUE;
}

/*********************************************************************\
* Function	MBDialup_OnShowWindow
* Purpose   WM_CREATE message handler of the main window
* Params
* Return    
* Remarks
**********************************************************************/
static void MBDialup_OnShowWindow(HWND hWnd)
{
	HMENU hmenu;
//	HMENU hsubmenu, hsubmenu2;
	SYSTEMTIME	SystemTime;
    char aTimeBuf[25], aDateBuf[25];

	GetLocalTime(&SystemTime);	
	GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
	SetWindowText(hPWnd, aTimeBuf);
	SendMessage(hPWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
	SendMessage(hPWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
	if (blockemerg == FALSE) 
	{
		if (strlen(cPhoneNumber) == 0)
		{
			SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)"");
			SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);
		}
		else if (strlen(cPhoneNumber) >=1) 
		{
            char cClassName [30] = "";
            GetClassName(hwndDialupImeEdit,cClassName,sizeof(cClassName));
            if (strcmp(cClassName,"WILDCARD") == 0)
            {
                if (GetWildCard(cPhoneNumber) == 0)
                {
                    SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,
                        MAKEWPARAM(IDC_WILDCARD_CALL,1),(LPARAM)WILDCALLL);
                    SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,
                        (WPARAM)IDC_BUTTON_CLEAR,BUTTONDELETE);
                }
            }
            else
            {
                int nPos = 0;
                nPos = SendMessage(hwndDialupImeEdit, EM_GETSEL, 0, 0);
                nPos = LOWORD(nPos);

                SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONSAVE);
                if (0 == nPos)
                    SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,(LPARAM)BUTTONQUIT);
                else
                    SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_CLEAR,BUTTONDELETE);
            }
            SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options"); 
        }
	}
	else
	{
		SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_CALL,1),(LPARAM)IDS_MENU_CALL);
		SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,(LPARAM)BUTTONQUIT);
	}
	hmenu = (HMENU)GetWindowLong(hWnd, GWL_USERDATA);
	PDASetMenu(hPWnd, hmenu);
	if (hwndDialupImeEdit != NULL)
		SetFocus(hwndDialupImeEdit);
	else
		SetFocus(hWnd); 
}

/*********************************************************************\
* Function	MBDialup_OnKeyDown
* Purpose   WM_KEYDOWN message handler of the main window
* Params
* Return    
* Remarks
\**********************************************************************/
static void MBDialup_OnKeyDown(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	int iNumberLen = 0;
	char chCurrent,cClassName[30] = "";
	
	switch(LOWORD(wParam))
	{
	case VK_1:
	case VK_2:
	case VK_3:
	case VK_4:
	case VK_5:
	case VK_6:
	case VK_7:
	case VK_8:
	case VK_9:
	case VK_0:
	case VK_F3:
	case VK_F4:
		{
			 if ((bSuperAddDial) && (bSuperAdd) && (!GetListCurCalling()) && (!IsAllType_Specific(CALL_STAT_HELD)))
			{
				if ((LOWORD(wParam) >= VK_0) && (LOWORD(wParam) <=VK_9)) 
				{
					chCurrent = (LOWORD(wParam) - VK_0 + 48);
				}
				else if (LOWORD(wParam) == VK_F3)
				{
					chCurrent = '*';
				}
				else if (LOWORD(wParam) == VK_F4) 
				{
					chCurrent = '#';
				}
				bSuperAdd   = TRUE;
				ME_SendDTMF(hWnd,IDC_ME_SUPERADDDIAL,chCurrent);
			}
		}
		break;
		
	case VK_F5:
        SendMessage(hPWnd, WM_KEYDOWN, wParam, lParam);
		break;
		
	case VK_F1:
        GetClassName(hwndDialupImeEdit,cClassName,sizeof(cClassName));
        if (strcmp(cClassName,"WILDCARD") == 0)
        {
            GetWindowText(hwndDialupImeEdit,cPhoneNumber,sizeof(cPhoneNumber));
        }
		if(Matchstr(pEmergentNumber, cPhoneNumber) == 1)
		{
			if (IsCallingWnd()) 
			{
				SetShowSummaryFlag(TRUE);
				CloseCallingWnd();			
			}
			else if (DialNumber(hWnd))
			{
				beToLink = TRUE;
				SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
				// add after frame modifyed
				SendMessage(hWnd, WM_CLOSE, 0,0);

			}
		}
		else
		{
			if (!(IsWindow_MBRing()))
			{
				if (DialNumber(hWnd))
				{
					CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPWnd);
					SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
					// add after frame modifyed
					SendMessage(hWnd, WM_CLOSE, 0,0);
				}
			}
			else
			{
				if (!DialNumber(hWnd))
					MessageMBRing(WM_KEYDOWN, wParam, lParam);
			}
		}
		break;
		
	case VK_RETURN:
		// call AB interface
		{
			ABNAMEOREMAIL abnameinfo;
			if (GetChangeLockStatus()) 
			{
				/*if (DialNumber(hWnd))
				{
//					CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPWnd);
					beToLink = TRUE;
					SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
					// add after frame modifyed
					SendMessage(hWnd, WM_CLOSE, 0,0);
				}*/
				GetClassName(hwndDialupImeEdit,cClassName,sizeof(cClassName));
                if (strcmp(cClassName,"WILDCARD") == 0)
                {
                    if (GetWildCard(cPhoneNumber) == 0)
                        SendMessage(hWnd,WM_KEYDOWN,VK_F1,0);

                    break;
                }
				memset(&abnameinfo, 0, sizeof(ABNAMEOREMAIL));
				abnameinfo.nType = AB_NUMBER;
				strcpy(abnameinfo.szTelOrEmail, cPhoneNumber);
				APP_SaveToAddressBook(hPWnd, hWnd, IDC_ADDNEWCONTACT, &abnameinfo, AB_NEW);
			}
			else
			{
                GetClassName(hwndDialupImeEdit,cClassName,sizeof(cClassName));
                if (strcmp(cClassName,"WILDCARD") == 0)
                {
                    if (GetWildCard(cPhoneNumber) == 0)
                        SendMessage(hWnd,WM_KEYDOWN,VK_F1,0);

                    break;
                }
				memset(&abnameinfo, 0, sizeof(ABNAMEOREMAIL));
				abnameinfo.nType = AB_NUMBER;
				strcpy(abnameinfo.szTelOrEmail, cPhoneNumber);
				APP_SaveToAddressBook(hPWnd, hWnd, IDC_ADDNEWCONTACT, &abnameinfo, AB_NEW);
			}
		}
		break;
		
	case VK_F10:
		{
			if (GetChangeLockStatus()) 
			{
				SetChangeLockStatus();
				DlmNotify(PS_LOCK_ENABLE, TRUE);				
			}
			CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPWnd);			
			SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			// add after frame modifyed
			SendMessage(hWnd, WM_CLOSE, 0,0);
		}		
		break;
		
	default:
		PDADefWindowProc(hWnd, wMsg, wParam, lParam);
		break;
	}
	return;
}

/*********************************************************************\
* Function	MBDialup_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
* Return    
* Remarks
\**********************************************************************/
static void MBDialup_OnCommand(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	int iDealNum;
	int iNumberLen;
    char cClassName [30] = "";

	switch( LOWORD( wParam ))
	{
    case IDC_WILDCARD_CALL:
        SendMessage(hWnd,WM_KEYDOWN,VK_F1,0);
        break;
    case IDC_HOLD:
    case IDC_ACTIVE:
        MBCalling_Shortcut(ACTIVE_SWITCH,NULL);
        break;
    case IDC_MUTE:
        CallingMute(TRUE);
        break;
    case IDC_UNMUTE:
        CallingMute(FALSE);
        break;
    case IDC_SENDDTMF:
        CreateSendDTMFWindow(NULL);
        break;
	case IDC_CALLEDIT:
		if (HIWORD(wParam) == EN_CHANGE) 
		{
            GetClassName(hwndDialupImeEdit,cClassName,sizeof(cClassName));
            if (strcmp(cClassName,"WILDCARD") == 0)
            {
                GetWindowText(hwndDialupImeEdit,cPhoneNumber,sizeof(cPhoneNumber));
                if (GetWildCard(cPhoneNumber) == 0)
                {
                    SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,
                        MAKEWPARAM(IDC_WILDCARD_CALL,1),(LPARAM)WILDCALLL);
                }
                else
                {
                    SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,
                        MAKEWPARAM(IDC_WILDCARD_CALL,1),(LPARAM)"");
                }
                break;
            }
			if (bSpecial)
			{
//				DestroyWindow(hWnd);
				//SendMessage(hWnd, WM_CLOSE, 0,0);
				CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPWnd);
				SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
				// add after frame modifyed
				SendMessage(hWnd, WM_CLOSE, 0,0);
				break;
			}
			iNumberLen = GetWindowTextLength(hwndDialupImeEdit);
			//if (iNumberLen < PHONENUMMAXLEN)
            GetWindowText(hwndDialupImeEdit,cPhoneNumber,sizeof(cPhoneNumber)/*iNumberLen+1*/);
            if (bEmergency)
            {
                int iEmergencyString;
                iEmergencyString = IfEmergencyString(pEmergentNumber,cPhoneNumber);
                switch (iEmergencyString)
                {
                case -1:
                    memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
                    SetTimer(hWnd,IDC_TIMER_EXIT,10,NULL);
                    //PostMessage(hWnd,WM_CLOSE,0,0);
                    break;
                case 0:
                    SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,
                        MAKEWPARAM(WM_EMERGENCY,1),(LPARAM)"");
                    break;
                case 1:
                    SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,
                        MAKEWPARAM(WM_EMERGENCY,1),(LPARAM)IDS_MENU_CALL);
                    break;
                }
                break;
            }
			iDealNum = DealWithNumber(hWnd);
			if ((iDealNum == 1) && (bFindInSIMPB == FALSE))
			{
				bNumber = FALSE;
				memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
				CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPWnd);
				SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
				// add after frame modifyed
				SendMessage(hWnd, WM_CLOSE, 0,0);
//				DestroyWindow(hWnd);
				//SendMessage(hWnd, WM_CLOSE, 0,0);
				return;
			}
			bNumber = TRUE;
		}
        else
        {
            if (EN_EMPTYTEXT == HIWORD(wParam))
            {
                if (GetWindowTextLength(hwndDialupImeEdit) == 0)
                    PostMessage(hWnd,WM_CLOSE,0,0);
            }    
        }
		break;
		
	case IDC_BUTTON_CALL:
		if (DialNumber(hWnd))
		{
			CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPWnd);
			SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			// add after frame modifyed
			SendMessage(hWnd, WM_CLOSE, 0,0);
//			SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);			
//			SendMessage(hWnd, WM_CLOSE, 0,0);
//			MBDialup_CloseWnd(hWnd, TRUE);
		}
		break;
	case IDM_NEWCONTACT:
		{
			ABNAMEOREMAIL abnameinfo;
			memset(&abnameinfo, 0, sizeof(ABNAMEOREMAIL));
			abnameinfo.nType = AB_NUMBER;

            GetClassName(hwndDialupImeEdit,cClassName,sizeof(cClassName));
            if (strcmp(cClassName,"WILDCARD") == 0)
            {
                GetWindowText(hwndDialupImeEdit,cPhoneNumber,sizeof(cPhoneNumber));
            }
			strcpy(abnameinfo.szTelOrEmail, cPhoneNumber);
			APP_SaveToAddressBook(hPWnd,hWnd , IDC_ADDNEWCONTACT, &abnameinfo, AB_NEW);
		}
		break;
	case IDM_ADDTOCONTACT:
		{
			ABNAMEOREMAIL abnameinfo;
			memset(&abnameinfo, 0, sizeof(ABNAMEOREMAIL));
			abnameinfo.nType = AB_NUMBER;
            GetClassName(hwndDialupImeEdit,cClassName,sizeof(cClassName));
            if (strcmp(cClassName,"WILDCARD") == 0)
            {
                GetWindowText(hwndDialupImeEdit,cPhoneNumber,sizeof(cPhoneNumber));
            }
			strcpy(abnameinfo.szTelOrEmail, cPhoneNumber);
			APP_SaveToAddressBook(hPWnd, hWnd, IDC_ADDNEWCONTACT, &abnameinfo, AB_UPDATE);
		}
		break;
	case IDM_CALL:
	case IDM_SERVCMD:
		if (DialNumber(hWnd))
		{
			CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPWnd);
			SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			// add after frame modifyed
			SendMessage(hWnd, WM_CLOSE, 0,0);
		}
			//DestroyWindow(hWnd);
			//MBDialup_CloseWnd(hWnd, TRUE);
		break;
	case IDM_SMS:
		//APP_EditSMS(hPWnd, cPhoneNumber, NULL);
        CallAppEntryEx("Edit SMS", 0, (LPARAM)cPhoneNumber);
		break;
	case IDM_MMS:
	//	APP_EditMMS(hPWnd,NULL,0, MMS_CALLEDIT_MOBIL, cPhoneNumber);
        CallAppEntryEx("Edit MMS", 0, (LPARAM)cPhoneNumber);
		break;
	case IDC_BUTTON_SET:
		{
			ABNAMEOREMAIL abnameinfo;
			memset(&abnameinfo, 0, sizeof(ABNAMEOREMAIL));
			abnameinfo.nType = AB_NUMBER;
            GetClassName(hwndDialupImeEdit,cClassName,sizeof(cClassName));
            if (strcmp(cClassName,"WILDCARD") == 0)
            {
                GetWindowText(hwndDialupImeEdit,cPhoneNumber,sizeof(cPhoneNumber));
            }
			strcpy(abnameinfo.szTelOrEmail, cPhoneNumber);
			APP_SaveToAddressBook(hPWnd, hWnd, IDC_ADDNEWCONTACT, &abnameinfo, AB_NEW);
		}
		break;

	case IDC_BUTTON_QUIT:
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPWnd);
		SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
		// add after frame modifyed
		SendMessage(hWnd, WM_CLOSE, 0,0);
/*
				if(IsCallingWnd())
				{
					SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
					SendMessage(hWnd, WM_CLOSE, 0, 0);
				}
				else
				{
					SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
				}*/
		
		break;
	}
}

/*********************************************************************\
* Function	MBDialup_OnTimer
* Purpose   WM_TIME message handler of the main window
* Params
* Return    
* Remarks
\**********************************************************************/
static void MBDialup_OnTimer(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	SYSTEMTIME	SystemTime;
    char aTimeBuf[25] = "", aDateBuf[25] = "";

	if(wParam == TIMER_ID_FRESH)
	{
		//update time
		if(IsTimeChanged())
		{
			GetLocalTime(&SystemTime);	
			GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
			SetWindowText(hPWnd, aTimeBuf);
		}
	}
    else if (wParam == IDC_TIMER_EXIT)
    {
        DestroyWindow(hWnd);
    }
}
/*********************************************************************\
* Function	judge time changed
* Purpose   
* Params
* Return    
* Remarks
\**********************************************************************/
BOOL IsTimeChanged(void)
{
    static SYSTEMTIME  oldTime;
	SYSTEMTIME	SystemTime;
	
	GetSystemTime(&SystemTime);

    if (SystemTime.wYear == oldTime.wYear && 
        SystemTime.wMonth == oldTime.wMonth &&
        SystemTime.wDay == oldTime.wDay &&
        SystemTime.wHour == oldTime.wHour &&
        SystemTime.wMinute == oldTime.wMinute)
    {
        return FALSE;
    }
    else
    {
        oldTime = SystemTime;     
    }

    return TRUE;
}

/********************************************************************
* Function   DialNumber  
* Purpose   excute dial 
* Params    
* Return    
* Remarks   1 to close dial, 2 to not close
\**********************************************************************/
static  BOOL    DialNumber(HWND hWnd)
{
    RESULT_BYSEND iSendNumber;

    if (bSuperAddDial)
    {
        if (!bAddCanSendNumber)
            return FALSE;
    }
    iSendNumber = DealWithNumberBySend(cPhoneNumber);
    switch (iSendNumber)
    {
    case RESULT_SUCC:       
    case RESULT_GSM:        
    case RESULT_PHONENUM:
	case RESULT_SW:
	case RESULT_SETLG:
	case RESULT_PM:
    case RESULT_ALS:
	case RESULT_BTADDR:		
        return TRUE;
    case RESULT_NOINIT:     
    case RESULT_QUICKDIAL:  
    case RESULT_ERRORNUMBER:
    case RESULT_NONESIM:
	case RESULT_RING:
    case RESULT_WILDCARD:
        return FALSE;
    }
    return TRUE;
}
/********************************************************************\
* Function   DealWithNumberBySend  
* Purpose   excute after pressing send
* Params    
* Return    
* Remarks   
\**********************************************************************/
static RESULT_BYSEND DealWithNumberBySend(const char * pPhoneNumber)
{
    SYSTEMTIME  sDialupTime;
    DWORD       wPhoneNumber;
    int         iUsedItemNum,iSour;
	ABNAMEOREMAIL abnameinfo; 
    char cClassName[30] = "";

    if (pPhoneNumber == NULL)
        return RESULT_ERRORNUMBER;

    iSour = strlen(pPhoneNumber);
    if (iSour == 0)
        return RESULT_ERRORNUMBER;
    
    strncpy(cPhoneNumber,pPhoneNumber,sizeof(cPhoneNumber) > iSour ? iSour + 1 : sizeof(cPhoneNumber) - 1);

    iUsedItemNum = GetUsedItemNum();
    if (0 == iUsedItemNum)
    {        
		if (IsWindow_MBRing())
		{
			if (strcmp(cPhoneNumber,pCh_Num0) == 0)
			{
				SetMMIHangUpFlag(TRUE);
				if (-1 == ME_VoiceDial(hMBDialUp,IDC_ME_SHORTCUT,(char *)cPhoneNumber,NULL))
				{
					AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
					return RESULT_ERRORNUMBER;
				}
				return RESULT_GSM;
			}
			/*if (strcmp(cPhoneNumber,pCh_Num1) == 0)
			{
				SetMMIHangUpFlag(TRUE);
				if (-1 == ME_VoiceDial(hMBDialUp,IDC_ME_SHORTCUT,(char *)cPhoneNumber,NULL))
				{
					AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
					return RESULT_ERRORNUMBER;
				}
				return RESULT_GSM;
			}
            if ((strcmp(cPhoneNumber,pCh_Num2) == 0) ||
                (strcmp(cPhoneNumber,pCh_Num3) == 0) ||
                (strcmp(cPhoneNumber,pCh_Num4) == 0))
            {
                SetMMIHangUpFlag(TRUE);
				if (-1 == ME_VoiceDial(hMBDialUp,IDC_ME_SHORTCUT,(char *)cPhoneNumber,NULL))
				{
					AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
					return RESULT_ERRORNUMBER;
				}
				return RESULT_GSM;
            }*/
            if (NumberIsEmergent(cPhoneNumber))
            {
                CloseRingWnd();//BTAnswerHangup();
            }
            else			
            {
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
                return RESULT_GSM;//RESULT_RING;
            }
		}

        if (StrIsNumber(cPhoneNumber))
        {
            wPhoneNumber = atoi(cPhoneNumber);
            if ((1 <= wPhoneNumber) && (wPhoneNumber <= QDIAL_MAX) && (iSour == 1))
            {
				memset(&abnameinfo, 0, sizeof(ABNAMEOREMAIL));
                if (0 == APP_GetQuickDial((WORD)wPhoneNumber, &abnameinfo))
                {
                    AppMessageBox(NULL,QUICKDIAL_FAIL,TITLECAPTION,WAITTIMEOUT);
                    return RESULT_QUICKDIAL;
                }
				if (abnameinfo.nType != AB_NUMBER) 
				{
                    AppMessageBox(NULL,QUICKDIAL_FAIL,TITLECAPTION,WAITTIMEOUT);
                    return RESULT_QUICKDIAL;
				}
				strcpy(cPhoneNumber, abnameinfo.szTelOrEmail);
            }
        }
    }
    else
    {
        if (strcmp(cPhoneNumber,pCh_Num0) == 0)
        {
            if (!MBCalling_Shortcut(CUT_HELD,NULL))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);

            return RESULT_GSM;
        }
        if (strcmp(cPhoneNumber,pCh_Num1) == 0)
        {
            if (!MBCalling_Shortcut(CUT_ACTIVED,NULL))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);

            return RESULT_GSM;
        }
        if ((strlen(cPhoneNumber) == 2) && (strncmp(cPhoneNumber,pCh_Num1,1) == 0))
        {
            if ((cCh_Num0 <= cPhoneNumber[1]) && (cPhoneNumber[1] <= cCh_Num9))
            {
                if (!MBCalling_Shortcut(CUT_ACTIVED_X,cPhoneNumber))
                    AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);

                return RESULT_GSM;
            }
        }
        if (strcmp(cPhoneNumber,pCh_Num2) == 0)
        {
            if (!MBCalling_Shortcut(ACTIVE_SWITCH,NULL))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);

            return RESULT_GSM;
        }
        if ((strlen(cPhoneNumber) == 2) && (strncmp(cPhoneNumber,pCh_Num2,1) == 0))
        {
            if ((cCh_Num0 <= cPhoneNumber[1]) && (cPhoneNumber[1] <= cCh_Num9))
            {
                if (!MBCalling_Shortcut(ACTIVE_SWITCH_X,cPhoneNumber))
                    AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);

                return RESULT_GSM;
            }
        }
        if (strcmp(cPhoneNumber,pCh_Num3) == 0)
        {
            if (!MBCalling_Shortcut(ADD_TO_ACTIVE,NULL))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);

            return RESULT_GSM;
        }
        if (strcmp(cPhoneNumber,pCh_Num4) == 0)
        {
            if (!MBCalling_Shortcut(CALL_TRANSFER,cPhoneNumber))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);

            return RESULT_GSM;
        }
        if ((strncmp(cPhoneNumber,pCh_Num4Asterick,2) == 0) && (strlen(cPhoneNumber) > 2))
        {
            if (!MBCalling_Shortcut(999,cPhoneNumber))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);

            return RESULT_GSM;
        }
        if (strcmp(cPhoneNumber,pCh_Num5) == 0)
        {
            if (!MBCalling_Shortcut(999,cPhoneNumber))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);

            return RESULT_GSM;
        }
        if ((IsWindow_DialLink()) || (IsWindow_MBRing()))
        {   
            if (!MBCalling_Shortcut(999,cPhoneNumber))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);

            return RESULT_GSM;
        }
    }

    if (stricmp(cPhoneNumber,pProjectMode) == 0)
    {
        CallProjectModeWindow(NULL);
        return RESUTL_PROJECTMODE;
    }

    if (!NumberIsEmergent(cPhoneNumber))
    {
        if (GSMDealWith(cPhoneNumber,FALSE))
            return RESULT_GSM;
        
        if (USSDDealWith(cPhoneNumber,iUsedItemNum != 0))
            return RESUTL_USSD;
    }
    
    if (iUsedItemNum == MAX_COUNT)
    {
		PLXTipsWin(NULL, NULL, NULL,NOTIFY_NOMORECALL, NULL, Notify_Failure, ML("Ok"), NULL,WAITTIMEOUT);
		return RESULT_PHONENUM;
    }

	if (GetPhoneGroupNum() == 2)
	{
		PLXTipsWin(NULL, NULL, NULL, NOTIFY_TOOMANYCALLS,"", Notify_Failure, ML("Ok"), NULL,WAITTIMEOUT);
		return RESULT_PHONENUM;
	}
				
/*	if (IsWindow_MBRing())
		return RESULT_RING;*/

	if ( 0 == GetSIMState()) 
    {
        //if (!NumberIsEmergent(cPhoneNumber))
		if(Matchstr(pEmergentNumber, cPhoneNumber) != 1)
        {
			PLXTipsWin(NULL, NULL, NULL, NOTIFY_NOSIMCARD, NULL, Notify_Failure, ML("Ok"), NULL,WAITTIMEOUT);
            return RESULT_NONESIM;
        }
    }
//  SaveMBRecordByPhoneNum(DIALED_REC,cPhoneNumber,&sDialupTime);
//	MBRecrodList_Refresh(DIALED_REC);
    
    GetClassName(hwndDialupImeEdit,cClassName,sizeof(cClassName));
    if (GetWildCard(cPhoneNumber) != 0)
    {
        if (IsWindow(hMBDialUp) && IsWindow(hwndDialupImeEdit))
        {
            RECT imesize;
            if (strcmp(cClassName,"WILDCARD") == 0)//通配符控件已经存在
            {
                PLXTipsWin(NULL, NULL, NULL, NOTIFY_WILDCLEW,
                    NULL, Notify_Alert, ML("Ok"), NULL,WAITTIMEOUT);
                return RESULT_WILDCARD;
            }
            GetClientRect(hMBDialUp, &imesize);
            DestroyWindow(hwndDialupImeEdit);
            hwndDialupImeEdit = CreateWindow(
                "WILDCARD", cPhoneNumber,
                WS_VISIBLE | WS_CHILD | ES_MULTILINE,
                0,70,imesize.right,40,
                (HWND)hMBDialUp,
                (HMENU)IDC_CALLEDIT, NULL, NULL);

            SetFocus(hwndDialupImeEdit);
            SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,
                MAKEWPARAM(IDC_WILDCARD_CALL,1),(LPARAM)"");
            SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,
                IDC_WILDCARD_BACK,BUTTONQUIT);
        }
        else
        {
            char cPhoneNumberWild[PHONENUMMAXLEN + 1] = "";
            strcpy(cPhoneNumberWild,cPhoneNumber);
            APP_CallPhone(cPhoneNumberWild);
        }
        return RESULT_WILDCARD;
    }
    else
    {
        if ((strcmp(cClassName,"WILDCARD") == 0) && (strlen(cPhoneNumber) == 0))//通配符控件已经存在
        {
            PLXTipsWin(NULL, NULL, NULL, NOTIFY_WILDCLEWEMPTY,
                NULL, Notify_Alert, ML("Ok"), NULL,WAITTIMEOUT);
            return RESULT_WILDCARD;
        }
    }
	GetLocalTime(&sDialupTime);
    CallMBDialLinkingWindow(hPreWnd, cPhoneNumber,&sDialupTime,FALSE);
    return RESULT_SUCC;
}
//返回通配符的个数,-1表示出错
static int GetWildCard(const char * pPhoneNumber)
{
    int i = 0;
    char * p = (char*)pPhoneNumber;
    if (pPhoneNumber == NULL)
        return -1;
    while (* p)
    {
        if (* p == '?')
            i++;
        p++;
    }
    return i;
}

static BOOL DealWithNumber(HWND hWnd)
{
	HWND hDEdit;

	hDEdit = GetDlgItem(hWnd,IDC_CALLEDIT);
    if (!bSuperAddDial)
    {
        if (GSM_PhoneBookDealWith(hDEdit, cPhoneNumber, &bFindInSIMPB))
            return TRUE;
    }
    return (GSMDealWith(cPhoneNumber,TRUE));
}

void    MessageMBDialUp( UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    if ((hMBDialUp != NULL) && (IsWindow(hMBDialUp)))
        PostMessage(hMBDialUp,wMsgCmd,wParam,lParam);
}

BOOL    IsWindow_Dialup(void)
{
    return (IsWindow(hMBDialUp));
}
HWND Get_DialupWnd()
{
	if (IsWindow(hMBDialUp)) 
		return hMBDialUp;
	else
		return NULL;
}

BOOL    PhoneNumberIfExistExtension(const char * pPhoneNumber,char * pPureNumber)
{
    char * p = NULL;
    int iPureNumberLen;

    p = strchr(pPhoneNumber,EXTENSIONCHAR);
    if (p == NULL)
        return FALSE;

    if (pPureNumber != NULL)
    {
        iPureNumberLen = abs(pPhoneNumber - p);
        strncpy(pPureNumber,pPhoneNumber,iPureNumberLen);
        p = pPureNumber + iPureNumberLen;
        * p = cCh_None;
    }
    return TRUE;
}

BOOL    NumberIsEmergent(const char * pNumber)
{
    int iGsmSpecific;
    char cPureNumber[PHONENUMMAXLEN + 1] = "";

    iGsmSpecific = GsmSpecific(pNumber);

    if (PhoneNumberIfExistExtension(pNumber,cPureNumber))
        return (InStrArray(cPureNumber + iGsmSpecific,pEmergentNumber) != -1);
    else
        return (InStrArray(pNumber + iGsmSpecific,pEmergentNumber) != -1);
}

void CloseDialupWnd()
{
//	SendMessage(hMBDialUp,WM_COMMAND,(WPARAM)IDC_BUTTON_QUIT,0);
	ShowOwnedPopups(hPWnd, SW_HIDE);
	CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hPWnd);
	PostMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hMBDialUp, 0);
// add after frame modifyed
    PostMessage(hMBDialUp, WM_CLOSE, 0,0);
}

static BOOL FindHash(char * ptr)
{
	if (NULL == strchr(ptr, cCh_Well)) 
		return FALSE;
	else
		return TRUE;
}

void NotifyEmergencyCall()
{
	MessageMBDialUp(WM_EMERGENCY, 0, 0);
}

void NotifyEmergency(void)
{
    bEmergency = TRUE;
}

int    IfEmergencyString(const char ** pArray,const char * pNumber)
{
    char ** p;
    int ilen = 0;

    ilen = strlen(pNumber);
    if (ilen == 0)
        return -1;

    p = (char**)pArray;
    while (**p)
    {
        if (strcmp(* p,pNumber) == 0)
            return 1;
        if (strncmp(* p,pNumber,ilen) == 0)
            return 0;

        p++;
    }
    return -1;
}
