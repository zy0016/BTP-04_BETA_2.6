#include    "MB_GSM.h"
#include    "PhoneBookExt.h"
#include    "setup.h"
#define DEBUG_MB_GSM


#if defined(SIEMENS_TC35)
#define GSM_CALLBAR_NONESERVICE     133 
#define GSM_CALLBAR_INVALIDCODE     16  
#define GSM_CALLBAR_INVALIDSTRING   260 
#endif
#if defined(WAVECOM_2C)
#define GSM_CALLBAR_NONESERVICE     30  
#define GSM_CALLBAR_INVALIDCODE     16  
#define GSM_CALLBAR_INVALIDSTRING   3   
#endif

#define GSM_CB_PWREGISTRATIONFAILURE  37
#define GSM_CB_NEGATIVEPWCHECK        38
#define GSM_CB_BLOCKED                43

static HINSTANCE    hInstance;
static GSMTYPE      GSMType;
static int          iGSMBSType = -1;
static BOOL cbflag;
static BOOL         bGSMAutoClose;
static HWND         hWndGSM = NULL;

static char       * pSucc;
static char       * pFail;
static char       * pParam1;
static char       * pParam2;
static char       * pParam3;
static char       * pParam4;

static int          iCallForward;
static RECT         hEditRect;

static const char * pNetworkName    = "/rom/mbphone/netname.dat";
static const char * pImei           = "IMEI:%s";
static const char * pClassName      = "MBGSMOperateWndClass";
static const char   Ch_Asterisk     = '*';
static const char   Ch_well         = '#';
static const char   Ch_End          = '\0';
static const char   Ch_Num1         = '1';
static const char * pAsterisk       = "*";
static const char * pWell           = "#";
static const char * pNewLine        = "\n";
static const char * pColon          = ":";

static const int    iGSMProjectMode  = 11;
static const int    iGSMPM           = 10;
static const int    iGSMDEACTIVESPSL = 9;
static const int    iGSMACTIVESPSL = 8;
static const int    iGSMCheck    = 7;
static const int    iGSMDEACTIVENSLC = 6;
static const int    iGSMACTIVENSLC   = 5;
static const int    iGSMBT           = 4;
static const int    iGSMSW           = 3;
static const int    iGSMLang         = 2;
static const int    iGSM            = 1;
static const int    iUnGSM          = 0;
static const int    iGSM_Error      = -1;
static const int    iGSM_Param      = -2;
static const int    iGSM_PINOFF     = -3;
static const int    iGSM_MEError    = -4;
static const int    iUnBSParam      = -1;
static const int    iClewMaxLen     = 2048;
static const int    iSuccMaxLen     = 256;
static const int    iFailMaxLen     = 256;
static const int    iParamMaxLen    = 100;

static int Codes_iremains = 0;
static   int  nQueryCodeType; //require code type
static int nSecurity =0;
//static int InputNCKCounter = 0;
//static int InputSPCKCounter = 0;
static const enum
{
    GSM_CF_ACTIVATE,                
    GSM_CF_ACTIVATELOGIN,           
    GSM_CF_DEACTIVATE,              
    GSM_CF_DEACTIVATELOGIN          
};
static const enum
{
    GSM_CB_CHECK,                   
    GSM_CB_CHECKBS                  
};
static const char * pGSMCode_T_SC[] = 
{
    "002",      
    "004",      
    "61",       
    ""
};

static const char * pGSMCode_BS[] = 
{
    "10",       
    "19",       
    "20",       
    "21",       
    "22",       
    "11",       
    "12",       
    "13",       
    "16",       
    "24",       
    "25",       
    "26",       
    "27",       
    ""
};
static const char * pBSContent[] =
{
    "(Voice + SMS + FAX)",                      
    "(Voice + FAX)",                            
    "(Data circuit asynchron + syncron. + PAD)",
    "(Data circuit asynchron + PAD)",           
    "(Data circuit synchron+packet)",           
    "(Voice)",                                  
    "(SMS + FAX)",                              
    "(FAX)",                                    
    "(SMS)",                                    
    "(Data circuit synchron)",                  
    "(Data circuit asynchron)",                 
    "(Packet)",                                 
    "(PAD)",                                    
    ""
};
static const char * pBSContentSearch[] =
{
    "(Telephony)",                              
    "(All data teleservices)",                  
    "(Fax services)",                           
    "(Short Message Services)",                 
    "(All data circuit synchronous)",           
    "(All data circuit asynchronous)",          
    "(All dedicated packet access)",            
    "(All dedicated PAD access)",               
    ""
};
static const int iBSME[] =
{
    BSVOICE,    
    BSDATA,     
    BSFAX,      
    BSSMS,      
    BSDCS,      
    BSDCA,      
    BSDPAC,     
    BSDPAD      
};

static const char * pActivateNSLC[] =
{
	"**02221*",
	""
};
static const char * pDeactivateNSLC[] =
{
	"##02221*",
	""
};
static const char * pCheckNSLC[] =
{
	"*#02221#",
	""
};
static const char * pActivateSPSL[] =
{
	"**02222*",
	""
};
static const char * pDeactivateSPSL[] =
{
	"##02222*",
	""
};
static const char * pCheckSPSL[] =
{
	"*#02222#",
	""
};

static const char * pSW[] =
{
    "*#0000#",    
    ""
};
static const char * pSetLang[] =
{
    "**9999#",    
    ""
};
static const char * pALS[] = 
{
	"**1#",
	"**2#",
	""
};
static const char * pBTAddr[] =
{
    "*#25884#",    
    ""
};
static const char * pPM[] =
{
    "*#2288379#",    
    ""
};

static const char * pProjectMode[] =
{
    "*#20050309#",    
    ""
};

static const char * pIMEI[] =
{
    "*#06#",    
    ""
};
static const char * pSecurity[] =
{
    "**04*",    
    "**042*",   
    "**05*",    
    "**052*",   
    ""
};
static const char * pCallBarring[] =
{
    "**03*330*",
    "**03**",   
    "*03*330*", 
    "*03**",    
    ""
};
static const char * pUnlockPS[] =
{
    "*#0003*",   
    ""
};

static const char * pForwardingCheck[] = 
{
    "*#002",   
    "*#004",   
    "*#21",    
    "*#61",    
    "*#62",    
    "*#67",    
    ""
};
static const char * pForwardingActive[] = 
{
    "*002",    
    "*004",    
    "*21",     
    "*61",     
    "*62",     
    "*67",     
    ""
};
static const char * pForwardingActiveLogin[] =
{
    "**002",   
    "**004",   
    "**21",    
    "**61",    
    "**62",    
    "**67",    
    ""
};
static const char * pForwardingDeactive[] = 
{
    "#002",    
    "#004",    
    "#21",     
    "#61",     
    "#62",     
    "#67",     
    ""
};
static const char * pForwardingDeactiveUnlogin[] = 
{
    "##002",   
    "##004",   
    "##21",    
    "##61",    
    "##62",    
    "##67",    
    ""
};

static const char * pCallBarringActive[] = 
{
    "*33*",     
    "*331*",    
    "*332*",    
    "*35*",     
    "*351*",    
    ""
};
static const char * pCallBarringDeactive[] = 
{
    "#33*",     
    "#331*",    
    "#332*",    
    "#35*",     
    "#351*",    
    "#330*",    
    "#333*",    
    "#353*",    
    ""
};
static const char * pCallBarringCheck[] = 
{
    "*#33#",    
    "*#331#",   
    "*#332#",   
    "*#35#",    
    "*#351#",   
    ""
};
static const char * pCallBarringCheckBS[] = 
{
    "*#33**",    
    "*#331**",   
    "*#332**",   
    "*#35**",    
    "*#351**",   
    ""
};

static const char * pCallWaiting[] = 
{
    "*43",     
    "*#43",    
    "#43",     
    ""
};

static const char * pPNPCheck[] =
{
    "*#30#",
    "*#31#",
    "*#76#",
    "*#77#",
    ""
};
static const char * pGSMSpecific[] =
{
    "*31#",
    "#31#",
    ""
};

static const char * pCallTransferTest[] =
{
    "*4#",
    "#4#",
    "*#4#",
    ""
};

static const  GSMOPERATE  GsmOperate[] = 
{   
    {pIMEI                     ,GsmIMEI              ,NULL                  ,TRUE  ,TRUE},
    {pSecurity                 ,GsmSecurity          ,NULL                  ,TRUE  ,FALSE},
    {pUnlockPS                 ,GsmUnlockPS          ,NULL                  ,TRUE  ,FALSE},

    {pForwardingCheck          ,GsmForwardingCheck   ,NULL                  ,FALSE ,FALSE},
    {pForwardingActive         ,GsmForwardingOperate ,GSM_CF_ACTIVATE       ,FALSE ,FALSE},
    {pForwardingActiveLogin    ,GsmForwardingOperate ,GSM_CF_ACTIVATELOGIN  ,FALSE ,FALSE},
    {pForwardingDeactive       ,GsmForwardingOperate ,GSM_CF_DEACTIVATE     ,FALSE ,FALSE},
    {pForwardingDeactiveUnlogin,GsmForwardingOperate ,GSM_CF_DEACTIVATELOGIN,FALSE ,FALSE},

    {pCallBarring              ,GsmModifyCallBarring ,NULL                  ,FALSE ,FALSE},
    {pCallBarringActive        ,GsmCallBarringOperate,TRUE                  ,FALSE ,FALSE},
    {pCallBarringDeactive      ,GsmCallBarringOperate,FALSE                 ,FALSE ,FALSE},
    {pCallBarringCheck         ,GsmCallBarringCheck  ,GSM_CB_CHECK          ,FALSE ,FALSE},
    {pCallBarringCheckBS       ,GsmCallBarringCheck  ,GSM_CB_CHECKBS        ,FALSE ,FALSE},

    {pCallWaiting              ,GsmCallWaitingOperate,NULL                  ,FALSE ,FALSE},

    {pPNPCheck                 ,GsmPNPCheck          ,NULL                  ,FALSE ,TRUE},
    {pGSMSpecific              ,GsmCLIRDeal          ,NULL                  ,FALSE ,TRUE},
    {pSW                       ,GsmSW                ,NULL                  ,TRUE ,FALSE},
    {pSetLang                  ,GsmSetLang           ,NULL                  ,TRUE ,FALSE},
    {pALS                      ,GsmALS               ,NULL                  ,TRUE ,FALSE},
    {pBTAddr                   ,GsmBTAddr            ,NULL                   ,TRUE ,FALSE},
    {pPM                       ,GsmPM                ,NULL                   ,TRUE ,FALSE},

    {pActivateNSLC             ,GsmActivateNSLC      ,NULL                   ,TRUE ,FALSE},
    {pDeactivateNSLC           ,GsmDeactivateNSLC    ,NULL                   ,TRUE ,FALSE},
    {pCheckNSLC                ,GsmCheckNSLC         ,NULL                   ,TRUE ,FALSE},
    {pActivateSPSL             ,GsmActivateSPSL      ,NULL                   ,TRUE ,FALSE},
    {pDeactivateSPSL           ,GsmDeactivateSPSL    ,NULL                   ,TRUE ,FALSE},
    {pCheckSPSL                ,GsmCheckSPSL         ,NULL                   ,TRUE ,FALSE},
    {pProjectMode              ,GsmProjectMode       ,NULL                   ,TRUE ,FALSE},	
    {pCallTransferTest         ,CallTransferTest     ,NULL                   ,TRUE ,TRUE},
    {NULL                      ,NULL                 ,NULL                  ,NULL  ,NULL}
};

static	PNSLC	pHeadNode = NULL;

static void * pUssd = NULL;
#define USSDREPLYMAXLEN     41

static int          iPhoneBookSIMMaxLen;
static PHONEBOOK    phonebook;
#define GSM_PHONEBOOKMAX    4
extern void SetUSSDNotify(HWND hWnd, UINT wMsg);
extern void SetUSSDReplyNotify(HWND hWnd, UINT wMsg);
extern BOOL    CallReplyControlWindow(HWND hPWnd);
extern void CloseUSSDReplyWnd();
extern void CloseUSSDShowWnd();
extern BOOL GetAutoLangName(char * AutoLangName);
extern BOOL Get_DeviceLocalAddr( char * addr);
static int GetNCKParam(const char * pGsmSrcCode, PNSLC pHead, char * pNCK);
static void	ReleaseMem();
/*
extern BOOL CallReplyControlWindow(HWND hwndCall,const char * pTitle,
                               unsigned int iEditMaxLen,char * pResult,
                               const char * pBLeftTxt,const char * pBRightTxt,
                               DWORD dwFlags);*/

BOOL    CreateMBGSMWindow(void)
{
    WNDCLASS wc;

    if (hWndGSM != NULL)
        return TRUE;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
        return FALSE;

    hWndGSM = CreateWindow(pClassName,TITLECAPTION, 
        WS_CAPTION |PWS_STATICBAR,PLX_WIN_POSITION , 
        NULL, NULL, NULL, NULL);

    if (NULL == hWndGSM)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT lResult;
	RECT    rClient;
    HWND    hGSMEdit;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);
		GetClientRect(hWnd, &rClient);
        hGSMEdit = CreateWindow (  
            "EDIT", NULL, 
            WS_VISIBLE | WS_CHILD |ES_MULTILINE|ES_UNDERLINE | WS_VSCROLL|ES_READONLY,
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
            hWnd,(HMENU)IDC_GSMEDIT,NULL,NULL);

        if (hGSMEdit == NULL)
            return FALSE;

        iGSMBSType = iUnBSParam;
        GetClientRect(hGSMEdit,&hEditRect);
        hEditRect.right -= 7;

        pSucc   = (char *)malloc(iSuccMaxLen );
        pFail   = (char *)malloc(iFailMaxLen );
        pParam1 = (char *)malloc(iParamMaxLen);
        pParam2 = (char *)malloc(iParamMaxLen);
        pParam3 = (char *)malloc(iParamMaxLen);
        pParam4 = (char *)malloc(iParamMaxLen);

        if ((pSucc == NULL) || (pFail == NULL) || (pParam1 == NULL) || 
            (pParam2 == NULL) || (pParam3 == NULL) || (pParam4 == NULL))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM malloc fail\r\n");
#endif
            return FALSE;
        }
        memset(pParam1,0x00,iParamMaxLen);
        memset(pParam2,0x00,iParamMaxLen);
        memset(pParam3,0x00,iParamMaxLen);
        memset(pParam4,0x00,iParamMaxLen);
        memset(pSucc  ,0x00,iSuccMaxLen );
        memset(pFail  ,0x00,iFailMaxLen );
        
        if (ME_USSD_SetNotify(hWnd,IDC_USSD_NOTIFY) == -1)
        {
            SetTimer(hWnd,IDC_USSD_TIMER,1000,NULL);
        }
//		SetUSSDNotify( hWnd, USSD_SHOWWND_MSG);
//		SetUSSDReplyNotify(hWnd, USSD_REPLYWND_MSG);
//        ME_GetPhonebookMemStatus(hWnd,IDC_ME_PHONE_SIM_AMX,PBOOK_SM);
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case IDC_USSD_TIMER:
            if (ME_USSD_SetNotify(hWnd,IDC_USSD_NOTIFY) != -1)
            {
                SetUSSDNotify( hWnd, USSD_SHOWWND_MSG);
                SetUSSDReplyNotify(hWnd, USSD_REPLYWND_MSG);
                ME_GetPhonebookMemStatus(hWnd,IDC_ME_PHONE_SIM_AMX,PBOOK_SM);
                KillTimer(hWnd,IDC_USSD_TIMER);
            }
            break;
        }
        break;
    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_GSMEDIT));
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(GetDlgItem(hWnd,IDC_GSMEDIT));

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_TODESKTOP:
        SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
        return FALSE;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;
		
	case USSD_SHOWWND_MSG:
		switch(wParam) 
		{
		case 0:
			ME_USSD_Abort(pUssd);
			break;
		case 1:
			CloseUSSDReplyWnd();
			CallReplyControlWindow(NULL);
			break;
		}
		break;

	case USSD_REPLYWND_MSG:
		{
			char * cUssd = NULL;
			int len = lParam;
			if (len > 0)				
				cUssd = malloc(len+1);
			if (cUssd == NULL) 
				break;
			strcpy(cUssd, (char *)wParam);
			ME_USSD_Response(pUssd,cUssd);
//			MBWaitWindowState(TRUE);
			WaitWindowStateEx(NULL, TRUE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
		}
	    break;

	case ME_MSG_GETLASTERROR:
			{
				ME_EXERROR LastError;
				char ErrorInfo[50] = "";
				WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
				if(ME_GetResult(&LastError,sizeof(ME_EXERROR))!=-1)
				{
					GetErrorString(LastError,ErrorInfo);
                    if (strcmp(ErrorInfo,"Other errors") != 0)
					    PLXTipsWin(NULL,NULL,0,ErrorInfo,ML("Network service"),Notify_Failure,ML("Ok"),NULL,0);
                    else
                        PLXTipsWin(NULL,NULL,0,"System failure",ML("Network service"),Notify_Failure,ML("Ok"),NULL,0);
				}
				else
					return FALSE;
			}
		break;

	case CALLBACK_CODE_REMAIN:
		{
			const int iSecurity[] = {LOCK_SC,LOCK_P2,PUK1,PUK2};
			const int iGSMOperate[] = {GSM_PIN1,GSM_PIN2,GSM_PUK1,GSM_PUK2};
			Codes_iremains = 0;			
			ME_GetResult(&Codes_iremains,sizeof(int));
			OperateGSMWindow(nQueryCodeType,TRUE,FALSE);								
			if ((Codes_iremains == 0) || (Codes_iremains == PUKNUMS))
			{
				switch(nQueryCodeType)
				{
				case GSM_PIN1:
					WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
					PLXTipsWin(NULL, NULL, NULL,NOTIFY_PIN_BLOCKED,CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, -1);
					break;
					
				case GSM_PIN2:
					WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
					PLXTipsWin(NULL, NULL, NULL,NOTIFY_PIN2_BLOCKED,CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, -1);
					break;
					
				case GSM_PUK1:
					if (Codes_iremains == 0)
					{
						WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
						PLXTipsWin(NULL, NULL, NULL,NOTIFY_SIM_BLOCKED,CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, -1);
					}
					else
					{
						if(!(GetSecurity_Open()))
						{
							WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
							PLXTipsWin(NULL, NULL, NULL, NOTIFY_SWITCH_PINCODE_REQUEST,CAP_NETWORKSERVICE,
								Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
							break;;
						}
						if (-1 == ME_PasswordValidation(hWnd, IDC_GSMOPERATE, iSecurity[nSecurity], pParam1,pParam2))
						{
							WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
							break;
						}
					}
					break;

				case GSM_PUK2:
					if (Codes_iremains == 0) 
					{
						WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
						PLXTipsWin(NULL, NULL, NULL,NOTIFY_PUK2_BLOCKED,CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, -1);
					}
					else
					{
						if(!(GetSecurity_Open()))
						{
							WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
							PLXTipsWin(NULL, NULL, NULL, NOTIFY_SWITCH_PINCODE_REQUEST,CAP_NETWORKSERVICE,
								Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
							break;;
						}
						
						if (-1 == ME_PasswordValidation(hWnd, IDC_GSMOPERATE, iSecurity[nSecurity], pParam1,pParam2))
						{
							WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
							break;
						}
					}
					break;	
				}
			}
			else
			{
				if(!(GetSecurity_Open()))
				{
					WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
					PLXTipsWin(NULL, NULL, NULL, NOTIFY_SWITCH_PINCODE_REQUEST,CAP_NETWORKSERVICE,
						Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
					break;;
				}
								
				if (-1 == ME_ChangeLockPassword(hWndGSM,IDC_GSMOPERATE,
					iSecurity[nSecurity],pParam1,pParam2))
				{
					WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
					break;
				}
			}
		}
		break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET:
        case IDC_BUTTON_QUIT:
            iGSMBSType = iUnBSParam;
            ShowWindow(hWnd,SW_HIDE);
            break;
        
/*
        case IDC_USSD_LEFTBUTTON:
            {
//                char cUssd[USSDREPLYMAXLEN + 1] = "";
//                CallReplyControlWindow(hWnd,TITLECAPTION,USSDREPLYMAXLEN,cUssd,
//                    BUTTONTXTLEFT_REPLY,BUTTONTXTRIGHT,0);
//                if (strlen(cUssd) > 0)
//                {
//                    ME_USSD_Response(pUssd,cUssd);
//                    MBWaitWindowState(TRUE);
//                }
            }
            break;
        case IDC_USSD_RIGHTBUTTON:
            ME_USSD_Abort(pUssd);
            break;*/

        
        case IDC_PHONE_LEFTBUTTON:
            APP_CallPhoneNumber(phonebook.PhoneNum);
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        case IDC_PHONE_RIGHTBUTTON:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        }
        break;
    
    case IDC_GSMOPERATE:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c 收到GSM操作成功的消息,GSMType:%d\r\n",GSMType);
#endif
			WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
            if (GSMClewText_Succ(hWnd))
                memset(pFail,0x00,iFailMaxLen);
            else
                SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),pFail);

            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c 收到GSM操作失败的消息,wParam:%ld,lParam:%ld,GSMType:%d\r\n",
                wParam,lParam,GSMType);
#endif
            GSMClewText_Fail(hWnd,wParam,lParam);
            break;
        }
        break;

    case IDC_USSD_NOTIFY:
        if (!USSDDealRespose(hWnd,wParam,lParam))
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);

        break;

    case IDC_ME_PHONE_SIM_AMX:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            {
                PBMEM_INFO pbmen_info;
                memset(&pbmen_info,0x00,sizeof(PBMEM_INFO));
                ME_GetResult(&pbmen_info,sizeof(PBMEM_INFO));
                iPhoneBookSIMMaxLen = pbmen_info.Total;
            }
            break;
        default:
            iPhoneBookSIMMaxLen = 100;
            break;
        }
        break;

    case ME_GETPIN1STATE:
        WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            {
                BOOL bStart;
                ME_GetResult(&bStart,sizeof(BOOL));
                if (bStart)
                {
                    ME_ChangeLockPassword(hWnd,IDC_GSMOPERATE,LOCK_SC,pParam1,pParam2);
                }
                else
                {
                    PLXTipsWin(NULL, NULL, NULL, NOTIFY_SWITCH_PINCODE_REQUEST,CAP_NETWORKSERVICE,
								Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
                }
            }
            break;
        default:
            if (lParam == 12)
            {
                PLXTipsWin(NULL, NULL, NULL, NOTIFY_PIN_BLOCKED, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
            }
            break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static  BOOL    GSMClewText_Fail(HWND hWnd,WPARAM wParam,WPARAM lParam)
{
    memset(pSucc,0x00,iSuccMaxLen);
	if (lParam == 260)
	{
		WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
		PLXTipsWin(NULL, NULL, NULL, "Codes do not match", CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
		return TRUE;

	}
    switch (GSMType)
    {
	case GSM_PIN1:
		WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
		if (lParam == 16) 
			PLXTipsWin(NULL, NULL, NULL, NOTIFY_WRONG_PINCODE, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
        else if (lParam == 12)
            PLXTipsWin(NULL, NULL, NULL, NOTIFY_PIN_BLOCKED, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
		else
			PLXTipsWin(NULL, NULL, NULL, ML("Service not available"), CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, -1);
		return TRUE;
	case GSM_PIN2:
		WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
		if (lParam == 16) //pin2错误
			PLXTipsWin(NULL, NULL, NULL, NOTIFY_WRONG_PIN2, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
        else if (lParam == 3)//pin2码锁
            PLXTipsWin(NULL, NULL, NULL, NOTIFY_PIN2_BLOCKED, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
		else
			PLXTipsWin(NULL, NULL, NULL, ML("Service not available"), CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, -1);
		return TRUE;
	case GSM_PUK1:
		WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
		if ((lParam == 16) || (lParam == 25))
			PLXTipsWin(NULL, NULL, NULL, NOTIFY_WRONG_PUKCODE, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
		else
			PLXTipsWin(NULL, NULL, NULL, ML("Service not available"), CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, -1);
		return TRUE;
	case GSM_PUK2:
		WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
		if (lParam == 16) 
			PLXTipsWin(NULL, NULL, NULL, NOTIFY_WRONG_PUK2CODE, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
		else
			PLXTipsWin(NULL, NULL, NULL, ML("Service not available"), CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, -1);
		return TRUE;
    case GSM_IMEI:
		WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
		PLXTipsWin(NULL, NULL, NULL, IMEIFAIL, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, -1);
        break;
    case GSM_SETALS1:
    case GSM_SETALS2:
        if (lParam == 10)
        {
            WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
            PLXTipsWin(NULL, NULL, NULL, NOTIFY_NOSIMCARD, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, -1);
        }
        else 
        {
            ME_GetLastError(hWnd,ME_MSG_GETLASTERROR);
        }
        break;
	default:
		if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
			return FALSE;
		break;
    }
    SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),pFail);
    return TRUE;
}

BOOL ShowIMEIInfo(HWND hPWnd, char * szdip);
BOOL ShowCFInfo(HWND hPWnd, CFINFO * pcf, int len);
BOOL ShowCWInfo(HWND hPWnd, CWINFO * pcf, int len);
BOOL ShowCBInfo(HWND hPWnd, CBINFO * pcf, int len);
BOOL ShowPMInfo(HWND hPWnd, ChargeCtl_t * pcc, int len);
BOOL ShowBTInfo(HWND hPWnd, char * pbt, int len);
BOOL ShowSWInfo(HWND hPWnd);

static  BOOL    GSMClewText_Succ(HWND hWnd)
{
    HWND hEdit;

    hEdit = GetDlgItem(hWnd,IDC_GSMEDIT);
    if (GSM_IMEI == GSMType)
    {
		int len;
        char cIMEI[PHONENUMMAXLEN] = "";
        if (-1 == ME_GetResult(cIMEI,sizeof(cIMEI)))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM IMEI fail\r\n");
#endif
            return FALSE;
        }
		len = strlen(cIMEI);
		//cIMEI[len] = 0;
		ShowIMEIInfo(NULL, cIMEI);
/*
        sprintf(pSucc,pImei,cIMEI);

        SetWindowTextExt(hEdit,pSucc);
*/
    }
    else if (GSM_CALLWAIT_CHECK == GSMType)
    {
        CWINFO * pCWInfo;
        int iBS,iCount,i;
        int iLength;
        char * pClew = NULL;

        iLength = ME_GetResult(NULL,NULL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,查询呼叫等待,iLength:%lu\r\n",iLength);
#endif
        if (iLength <= 0)
            return FALSE;

        pCWInfo = (CWINFO *)malloc(iLength);
        if (NULL == pCWInfo)
            return FALSE;

        pClew = (char *)malloc(iClewMaxLen);
        if (NULL == pClew)
           return FALSE;

        memset(pCWInfo,0x00,iLength);
        memset(pClew,0x00,iClewMaxLen);

        iLength = ME_GetResult(pCWInfo,iLength);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,查询呼叫等待,ME_GetResult=%lu\r\n",iLength);
#endif
		ShowCWInfo(NULL, pCWInfo, iLength);
        iCount = iLength / sizeof(CWINFO);
        for (i = 0;i < iCount;i++)
        {
            iBS = InIntArray(pCWInfo[i].BS,iBSME,sizeof(iBSME));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c 查询呼叫等待 pCWInfo[%d].BS:%d,iBS:%d\r\n",i,pCWInfo[i].BS,iBS);
#endif
            StrAutoNewLine(NULL,GSMCALLWAITING,pParam1,&hEditRect,NULL);
            StrAutoNewLine(NULL,(iBS == iUnBSParam) ? "" : pBSContentSearch[iBS],
                pParam2,&hEditRect,NULL);
            StrAutoNewLine(NULL,pCWInfo[i].Active ? GSMOPERATEACTIVATE : GSMOPERATEDEACTIVATE,
                pParam3,&hEditRect,NULL);

            StringCombination(pSucc,pParam1,pParam2,pParam3,NULL);

            strcat(pClew,pSucc);
            strcat(pClew,pNewLine);
            strcat(pClew,pNewLine);
        }
        SetWindowTextExt(hEdit,pClew);
        free(pClew);
        free(pCWInfo);
    }
    else if ((GSM_CALLFORWARDING_CHECKBEGIN < GSMType) && 
        (GSMType < GSM_CALLFORWARDING_CHECKEND))
    {
        CFINFO * pCFInfo;
        int iLength;
        int iCount,i,iBS;
        char * pClew = NULL;

        iLength = ME_GetResult(NULL,NULL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,检查呼叫转移,iLength:%lu\r\n",iLength);
#endif
        if (iLength <= 0)
            return FALSE;

        pCFInfo = (CFINFO *)malloc(iLength);
        if (NULL == pCFInfo)
            return FALSE;

        pClew = (char *)malloc(iClewMaxLen);
        if (NULL == pClew)
        {
            free(pCFInfo);
            return FALSE;
        }
        memset(pCFInfo,0x00,iLength);
        memset(pClew,0x00,iClewMaxLen);

        iLength = ME_GetResult(pCFInfo,iLength);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,查询呼叫转移,ME_GetResult=%lu\r\n",iLength);
#endif
		ShowCFInfo(NULL, pCFInfo, iLength);
        iCount = iLength / sizeof(CFINFO);
        for (i = 0;i < iCount;i++)
        {
            iBS = InIntArray(pCFInfo[i].BS,iBSME,sizeof(iBSME));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c 查询呼叫转移 pCFInfo[%d].BS:%d,iBS:%d,Reason:%d,号码:%s,状态:%d\r\n",
                i,pCFInfo[i].BS,iBS,pCFInfo[i].Reason,pCFInfo[i].DN,pCFInfo[i].Active);
#endif
            switch (pCFInfo[i].Reason)
            {
            case CFU:
                StrAutoNewLine(NULL,GSMCALLFORWARDING3,pParam1,&hEditRect,NULL);
                break;
            case CFB:
                StrAutoNewLine(NULL,GSMCALLFORWARDING6,pParam1,&hEditRect,NULL);
                break;
            case CFNRy:
                StrAutoNewLine(NULL,GSMCALLFORWARDING4,pParam1,&hEditRect,NULL);
                break;
            case CFNRc:
                StrAutoNewLine(NULL,GSMCALLFORWARDING5,pParam1,&hEditRect,NULL);
                break;
            }
            StrAutoNewLine(NULL,(iBS == iUnBSParam) ? "" : pBSContentSearch[iBS],
                pParam2,&hEditRect,NULL);
            StrAutoNewLine(NULL,pCFInfo[i].Active ? GSMOPERATEACTIVATE : GSMOPERATEDEACTIVATE,
                pParam3,&hEditRect,NULL);
            if (strlen(pCFInfo[i].DN) != 0)
            {
                strcpy(pParam4,GSMCALLFORWARDING_NO);
                strcat(pParam4,pCFInfo[i].DN);
            }
            else
            {
                strcpy(pParam4,"");
            }
            StringCombination(pSucc,pParam1,pParam2,pParam3,pParam4);
            strcat(pClew,pSucc);
            strcat(pClew,pNewLine);
            strcat(pClew,pNewLine);
        }
        SetWindowTextExt(hEdit,pClew);
        free(pClew);
        free(pCFInfo);
    }
    else if ((GSM_CALLBARRING_CHECKBEGIN < GSMType) && 
        (GSMType < GSM_CALLBARRING_CHECKEND))
    {
        CBINFO * pCBInfo;
        int iLength;
        int iCount,i,iBS;
        char * pClew = NULL;

        iLength = ME_GetResult(NULL,NULL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,查询呼叫限制,iLength:%lu\r\n",iLength);
#endif
        if (iLength <= 0)
            return FALSE;

        pCBInfo = (CBINFO *)malloc(iLength);
        if (NULL == pCBInfo)
            return FALSE;

        pClew = (char *)malloc(iClewMaxLen);
        if (NULL == pClew)
        {
            free(pCBInfo);
            return FALSE;
        }
        memset(pCBInfo,0x00,iLength);
        memset(pClew,0x00,iClewMaxLen);

        iLength = ME_GetResult(pCBInfo,iLength);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,查询呼叫限制,ME_GetResult=%lu\r\n",iLength);
#endif
		ShowCBInfo(NULL, pCBInfo, iLength);
        iCount = iLength / sizeof(CBINFO);
        for (i = 0;i < iCount;i++)
        {
            iBS = InIntArray(pCBInfo[i].BS,iBSME,sizeof(iBSME));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c 查询呼叫限制,BS:%d,iBS:%d,Fac:%d,状态:%d\r\n",
                pCBInfo[i].BS,iBS,pCBInfo[i].Fac,pCBInfo[i].Active);
#endif
            switch (pCBInfo[i].Fac)
            {
            case BAOC:
                StrAutoNewLine(NULL,GSMCALLBARRING33,pParam1,&hEditRect,NULL);
                break;
            case BOIC:
                StrAutoNewLine(NULL,GSMCALLBARRING331,pParam1,&hEditRect,NULL);
                break;
            case BOIC_exHC:
                StrAutoNewLine(NULL,GSMCALLBARRING332,pParam1,&hEditRect,NULL);
                break;
            case BAIC:
                StrAutoNewLine(NULL,GSMCALLBARRING35,pParam1,&hEditRect,NULL);
                break;
            case BIC_Roam:
                StrAutoNewLine(NULL,GSMCALLBARRING351,pParam1,&hEditRect,NULL);
                break;
            }
            StrAutoNewLine(NULL,(iBS == iUnBSParam) ? "" : pBSContentSearch[iBS],
                pParam2,&hEditRect,NULL);
            StrAutoNewLine(NULL,pCBInfo[i].Active ? GSMOPERATEACTIVATE : GSMOPERATEDEACTIVATE,
                pParam3,&hEditRect,NULL);

            StringCombination(pSucc,pParam1,pParam2,pParam3,NULL);
            strcat(pClew,pSucc);
            strcat(pClew,pNewLine);
            strcat(pClew,pNewLine);
        }
        SetWindowTextExt(hEdit,pClew);
        free(pClew);
        free(pCBInfo);
    }
    else if (GSM_CLIP_CHECK == GSMType)
    {
        CLIPINFO clipinfo;
        ME_GetResult(&clipinfo,sizeof(CLIPINFO));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM GSM_CLIP_CHECK:%d\r\n",clipinfo.Status);
#endif
        switch (clipinfo.Status)
        {
        case CLIPNONE:
            sprintf(pParam1,GSMCLIP,pColon,GSMCLIP_NONE);
			PLXTipsWin(NULL, NULL, NULL, ML("Caller number\nis hidden"), CAP_NETWORKSERVICE, Notify_Info, ML("Ok"), NULL, -1);
            break;
        case CLIPALLOW:
            sprintf(pParam1,GSMCLIP,pColon,GSMCLIP_ALLOW);
			PLXTipsWin(NULL, NULL, NULL, ML("Caller number\nis revealed"), CAP_NETWORKSERVICE, Notify_Info, ML("Ok"), NULL, -1);
            break;
        case CLIPUNKNOWN:
            sprintf(pParam1,GSMCLIP,pColon,GSMCLIP_UNKNOWN);
			PLXTipsWin(NULL, NULL, NULL, NOTIFY_ERR_UNKNOWN, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, -1);
            break;
        }
        StrAutoNewLine(NULL,pParam1,pSucc,&hEditRect,NULL);
        SetWindowTextExt(hEdit,pSucc);
    }
    else if (GSM_CLIR_CHECK == GSMType)
    {
        CLIRINFO clirinfo;
        ME_GetResult(&clirinfo,sizeof(CLIRINFO));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM GSM_CLIR_CHECK:%d\r\n",clirinfo.Status);
#endif
        switch (clirinfo.Status)
        {
        case CLIRNONE:
        case CLIRTEMPALLOW:
            sprintf(pParam1,GSMCLIR,pColon,GSMCLIR1);
            PLXTipsWin(NULL, NULL, NULL, ML("Own number\nis revealed\nwhen calling"), CAP_NETWORKSERVICE, Notify_Info, ML("Ok"), NULL, -1);			
            break;
        case CLIRUNKNOWN:
            PLXTipsWin(NULL, NULL, NULL, NOTIFY_ERR_UNKNOWN, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, -1);
            break;
        case CLIRPERMANENT:
        case CLIRTEMPRES:
            sprintf(pParam1,GSMCLIR,pColon,GSMCLIR2);
            PLXTipsWin(NULL, NULL, NULL, ML("Own number\nis hidden\nwhen calling"), CAP_NETWORKSERVICE, Notify_Info, ML("Ok"), NULL, -1);			
            break;
        }
        StrAutoNewLine(NULL,pParam1,pSucc,&hEditRect,NULL);
        SetWindowTextExt(hEdit,pSucc);
    }
    else if (GSM_COLP_CHECK == GSMType)
    {
        COLPINFO colpinfo;
        ME_GetResult(&colpinfo,sizeof(COLPINFO));
		if (colpinfo.Active) 
			PLXTipsWin(NULL, NULL, NULL, ML("Recipient number\nis revealed\nwhen calling"), CAP_NETWORKSERVICE, Notify_Info, ML("Ok"), NULL, -1);
		else
			PLXTipsWin(NULL, NULL, NULL, ML("Recipient number\nis hidden\nwhen calling"), CAP_NETWORKSERVICE, Notify_Info, ML("Ok"), NULL, -1);			
        sprintf(pSucc,GSMCOLP,pColon,colpinfo.Active ? GSMOPERATEACTIVATE : GSMOPERATEDEACTIVATE);
        SetWindowTextExt(hEdit,pSucc);
    }
    else if (GSM_COLR_CHECK == GSMType)
    {
        COLRINFO colrinfo;
        ME_GetResult(&colrinfo,sizeof(COLRINFO));
		if (colrinfo.Active) 
			PLXTipsWin(NULL, NULL, NULL, ML("Own number\nis revealed\nwhen calling"), CAP_NETWORKSERVICE, Notify_Info, ML("Ok"), NULL, -1);
		else
			PLXTipsWin(NULL, NULL, NULL, ML("Own number\nis hidden\nwhen calling"), CAP_NETWORKSERVICE, Notify_Info, ML("Ok"), NULL, -1);			
        sprintf(pSucc,GSMCOLR,pColon,colrinfo.Active ? GSMOPERATEACTIVATE : GSMOPERATEDEACTIVATE);
        SetWindowTextExt(hEdit,pSucc);
    }
	else if (GSM_PM == GSMType)
	{
		ChargeCtl_t chargeinfo;
		ME_GetResult(&chargeinfo, sizeof(ChargeCtl_t));
        WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
		ShowPMInfo(NULL, &chargeinfo, sizeof(ChargeCtl_t));
	}
    else if (CALLTRANSFERTEST_ON == GSMType)
    {
        SetCallTransferTest(TRUE);
    }
    else if (CALLTRANSFERTEST_OFF == GSMType)
    {
        SetCallTransferTest(FALSE);
    }
    else if (GETMENUERROR == GSMType)
    {
        char cMenuError[50] = "";
        sprintf(cMenuError,"Menu Error:%d",GetErrorMenu());

        PLXTipsWin(NULL, NULL, NULL, cMenuError, 
            CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
    }
    else
    {
        SetWindowTextExt(hEdit,pSucc);
        switch (GSMType)
        {
		case GSM_SETALS1:
			PLXTipsWin(NULL, NULL, NULL, ML("Active line\nLine1"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			break;
		case GSM_SETALS2:
			PLXTipsWin(NULL, NULL, NULL, ML("Active line\nLine2"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			break;
		case GSM_CALLBAR:
			PLXTipsWin(NULL, NULL, NULL, ML("Barring code\nchanged"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			break;
		case GSM_CALLBARRING33:              
		case GSM_CALLBARRING331:             
		case GSM_CALLBARRING332:                
		case GSM_CALLBARRING35:              
		case GSM_CALLBARRING351:
			if (cbflag) 
				PLXTipsWin(NULL, NULL, NULL, ML("Call barring\nset"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			else
				PLXTipsWin(NULL, NULL, NULL, ML("Call barring\ncleared"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);			
			break;
		case GSM_CALLBARRING330:             
		case GSM_CALLBARRING333:             
        case GSM_CALLBARRING353:    
			PLXTipsWin(NULL, NULL, NULL, ML("Call barring\ncleared"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);			
			break;
		case GSM_CALLWAIT_ACTIVE:
			PLXTipsWin(NULL, NULL, NULL, ML("Call waiting\nset"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			break;
		case GSM_CALLWAIT_DEACTIVE:
			PLXTipsWin(NULL, NULL, NULL, "Call waiting \ncleared", CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			break;
        case GSM_PIN2:
			PLXTipsWin(NULL, NULL, NULL,NOTIFY_PIN2CODE_CHANGED, CAP_NETWORKSERVICE, Notify_Success,
				ML("Ok"), NULL,WAITTIMEOUT);
            break;
		case GSM_PUK2:
			PLXTipsWin(NULL, NULL, NULL,NOTIFY_PIN2CODE_CHANGED, CAP_NETWORKSERVICE, Notify_Success,
				ML("Ok"), NULL,WAITTIMEOUT);
			break;
		case GSM_PIN1:
			PLXTipsWin(NULL, NULL, NULL,NOTIFY_PINCODE_CHANGED, CAP_NETWORKSERVICE, Notify_Success,
				ML("Ok"), NULL,WAITTIMEOUT);
			break;
		case GSM_PUK1:
			PLXTipsWin(NULL, NULL, NULL,NOTIFY_PINCODE_CHANGED, CAP_NETWORKSERVICE, Notify_Success,
				ML("Ok"), NULL,WAITTIMEOUT);
			break;
        case GSM_CALLFORWARDING1:
			if (iCallForward == GSM_CF_ACTIVATELOGIN || iCallForward == GSM_CF_ACTIVATE)
			{
				SetForward_All(TRUE);
				DlmNotify(PS_SETCALLDIVERT,ICON_CANCEL);
				PLXTipsWin(NULL, NULL, NULL, ML("Call divert set"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);				
			}
			if (iCallForward == GSM_CF_DEACTIVATELOGIN || iCallForward == GSM_CF_DEACTIVATE)
			{
                SetForward_All(FALSE);
				DlmNotify(PS_SETCALLDIVERT,ICON_CANCEL);
				PLXTipsWin(NULL, NULL, NULL, ML("Call divert\ncleared"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			}
            break;
			
        case GSM_CALLFORWARDING2:
			if (iCallForward == GSM_CF_ACTIVATELOGIN || iCallForward == GSM_CF_ACTIVATE)
			{
				SetForward_Condition(TRUE);
				DlmNotify(PS_SETCALLDIVERT,ICON_CANCEL);
				PLXTipsWin(NULL, NULL, NULL, ML("Call divert set"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			}
			if (iCallForward == GSM_CF_DEACTIVATELOGIN || iCallForward == GSM_CF_DEACTIVATE)
			{
                SetForward_Condition(FALSE);
				DlmNotify(PS_SETCALLDIVERT,ICON_CANCEL);
				PLXTipsWin(NULL, NULL, NULL, ML("Call divert\ncleared"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			}
            break;
        case GSM_CALLFORWARDING3:
			if (iCallForward == GSM_CF_ACTIVATELOGIN || iCallForward == GSM_CF_ACTIVATE)
			{
                SetForward_Uncdtional(TRUE);
				DlmNotify(PS_SETCALLDIVERT,ICON_SET);
				PLXTipsWin(NULL, NULL, NULL, ML("Call divert set"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			}
			if (iCallForward == GSM_CF_DEACTIVATELOGIN || iCallForward == GSM_CF_DEACTIVATE)
			{
                SetForward_Uncdtional(FALSE);
				DlmNotify(PS_SETCALLDIVERT,ICON_CANCEL);
                MessageMBPostMsgCallForward();
				PLXTipsWin(NULL, NULL, NULL, ML("Call divert\ncleared"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			}
            break;
        case GSM_CALLFORWARDING4:
			if (iCallForward == GSM_CF_ACTIVATELOGIN || iCallForward == GSM_CF_ACTIVATE)
			{
                SetForward_NoReply(TRUE);
				DlmNotify(PS_SETCALLDIVERT,ICON_CANCEL);
				PLXTipsWin(NULL, NULL, NULL, ML("Call divert set"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			}
			if (iCallForward == GSM_CF_DEACTIVATELOGIN || iCallForward == GSM_CF_DEACTIVATE)
			{
                SetForward_NoReply(FALSE);
				DlmNotify(PS_SETCALLDIVERT,ICON_CANCEL);
				PLXTipsWin(NULL, NULL, NULL, ML("Call divert\ncleared"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			}
            break;
        case GSM_CALLFORWARDING5:
			if (iCallForward == GSM_CF_ACTIVATELOGIN || iCallForward == GSM_CF_ACTIVATE)
			{
                SetForward_NoReach(TRUE);
				DlmNotify(PS_SETCALLDIVERT,ICON_CANCEL);
				PLXTipsWin(NULL, NULL, NULL, ML("Call divert set"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			}
			if (iCallForward == GSM_CF_DEACTIVATELOGIN || iCallForward == GSM_CF_DEACTIVATE)
			{
                SetForward_NoReach(FALSE);
				DlmNotify(PS_SETCALLDIVERT,ICON_CANCEL);
				PLXTipsWin(NULL, NULL, NULL, ML("Call divert\ncleared"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			}
            break;
        case GSM_CALLFORWARDING6:
			if (iCallForward == GSM_CF_ACTIVATELOGIN || iCallForward == GSM_CF_ACTIVATE)
			{
                SetForward_Busy(TRUE);
				DlmNotify(PS_SETCALLDIVERT,ICON_CANCEL);
				PLXTipsWin(NULL, NULL, NULL, ML("Call divert set"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			}
			if (iCallForward == GSM_CF_DEACTIVATELOGIN || iCallForward == GSM_CF_DEACTIVATE)
			{
                SetForward_Busy(FALSE);
				DlmNotify(PS_SETCALLDIVERT,ICON_CANCEL);
				PLXTipsWin(NULL, NULL, NULL, ML("Call divert\ncleared"), CAP_NETWORKSERVICE, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			}
            break;
        }
    }
    return TRUE;
}

static  void    OperateGSMWindow(GSMTYPE iGSM_Type,BOOL bShow,BOOL bAutoClose)
{
    GSMType       = iGSM_Type;
    bGSMAutoClose = bAutoClose;

    if (bShow)
    {
        SetWindowText(hWndGSM,TITLECAPTION);
        SendMessage(hWndGSM,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        SendMessage(hWndGSM,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);

//        ShowWindow(hWndGSM,SW_SHOW);
//        UpdateWindow(hWndGSM);  
    }
}


BOOL    GSMDealWith(const char * cPhoneNumber,BOOL bExecuteAtOnce)
{
    int         i = 0,iPreLength,iFuncRes;
    int         iPrePlace;
    char        ** pGsmPre;
    GSMSTRUCT   GsmStruct;

    if (cPhoneNumber[strlen(cPhoneNumber) - 1] != Ch_well)
        return FALSE;

    if (hWndGSM == NULL)
    {
        CreateMBGSMWindow();
    }

    GsmStruct.hWnd      = hWndGSM;
    GsmStruct.iCallMsg  = IDC_GSMOPERATE;

    while ((GsmOperate[i].pGSMPre != NULL) && (GsmOperate[i].pFunction != NULL))
    {
        if (GsmOperate[i].bExecuteAtOnce != bExecuteAtOnce)
        {
            i++;
            continue;
        }
        pGsmPre = (char **)GsmOperate[i].pGSMPre;
        iPrePlace = 0;
        while (** pGsmPre)
        {
            iPreLength = strlen(* pGsmPre);

            if (strncmp(* pGsmPre,cPhoneNumber,iPreLength) == 0)
            {   
                if (GsmOperate[i].bGSMLengthMatch)
                {
                    if (strcmp(* pGsmPre,cPhoneNumber) != 0)
                        return FALSE;
                }
                GsmStruct.pPhoneNumber  = (char *)cPhoneNumber;
                GsmStruct.pGsmPre       = (char **)GsmOperate[i].pGSMPre;
                GsmStruct.iStation      = iPrePlace;
                GsmStruct.wParam        = GsmOperate[i].wParam;
                iFuncRes = ((GsmOperate[i].pFunction)(&GsmStruct));
				if ((iGSMLang == iFuncRes)||(iGSMSW == iFuncRes)||(iGSMBT == iFuncRes)
					||(iGSMPM == iFuncRes)||(iGSMProjectMode == iFuncRes)
					||(iGSMDEACTIVESPSL == iFuncRes)||(iGSMACTIVESPSL == iFuncRes)
					||(iGSMCheck == iFuncRes)||(iGSMDEACTIVENSLC == iFuncRes)||(iGSMACTIVENSLC == iFuncRes)) 
				{
					return TRUE;
				}
                if (iGSM == iFuncRes)
                {
                    SetWindowTextExt(GetDlgItem(hWndGSM,IDC_GSMEDIT),"");
                    WaitWindowStateEx(NULL, TRUE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
                    return TRUE;
                }
                else if (iGSM_Error == iFuncRes)
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
                    printf("mb MB_GSM.c AT命令发送失败:%s,GSMType:%d\r\n",
                        cPhoneNumber,GSMType);
#endif
                    GSMClewText_Fail(hWndGSM,0,0);
                    return TRUE;
                }
                else if (iGSM_Param == iFuncRes)
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
                    printf("mb MB_GSM.c GSM参数不合适\r\n");
#endif
                    return TRUE;
                }
				else if (iGSM_MEError == iFuncRes)
				{
					return TRUE;
				}
                else
                {
                    return FALSE;
                }
            }
            iPrePlace++;
            pGsmPre++;
        }
        i++;
    }
    return FALSE;
}

// SW
static  int     GsmSW(GSMSTRUCT * pGsmStruct)
{
    OperateGSMWindow(GSM_SW,FALSE,FALSE);
//	WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
    ShowSWInfo(NULL);
    return iGSMSW;
}

static  int     GsmSetLang(GSMSTRUCT * pGsmStruct)
{
	char pLangName[20] = "";
	char pDisp[40] = "";

    OperateGSMWindow(GSM_SETLANG,FALSE,FALSE);
//	WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
	SetAutoLanguage(SWITCH_ON);
	if (GetAutoLangName(pLangName))
	{
		sprintf(pDisp, "%s\n%s", NOTIFY_PHONELANG, pLangName);
		PLXTipsWin(NULL, NULL, NULL, pDisp, NULL, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
	}
	else
		PLXTipsWin(NULL, NULL, NULL, NOTIFY_UNSUCCESSFUL, NULL, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);	
    return iGSMLang;
}

static int  GsmALS(GSMSTRUCT * pGsmStruct)
{
	if ( 0 == pGsmStruct->iStation) 
	{
		OperateGSMWindow(GSM_SETALS1,FALSE,FALSE);
		if (-1 == ME_SelectOutLine(pGsmStruct->hWnd,pGsmStruct->iCallMsg, 1, 0))
		{
			PLXTipsWin(NULL, NULL, NULL, NOTIFY_UNSUCCESSFUL, NULL, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);	
			return iGSM_Error;
		}
		return iGSM;
	}
	if ( 1 == pGsmStruct->iStation) 
	{
		OperateGSMWindow(GSM_SETALS2,FALSE,FALSE);
		if (-1 == ME_SelectOutLine(pGsmStruct->hWnd,pGsmStruct->iCallMsg, 2, 0))
		{
			PLXTipsWin(NULL, NULL, NULL, NOTIFY_UNSUCCESSFUL, NULL, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);	
			return iGSM_Error;
		}
		return iGSM;
	}	
	return iGSM;
}
static int GsmPM(GSMSTRUCT * pGsmStruct)
{
	OperateGSMWindow(GSM_PM,FALSE,FALSE);
	if (-1 == ME_GetChargeStatus(pGsmStruct->hWnd,pGsmStruct->iCallMsg))
	{
		PLXTipsWin(NULL, NULL, NULL, NOTIFY_UNSUCCESSFUL, NULL, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);	
		return iGSM_Error;
	}
    WaitWindowStateEx(NULL, TRUE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
	return iGSMPM;
}
static int GsmBTAddr(GSMSTRUCT * pGsmStruct)
{
	char pAddr[32]= "";
	OperateGSMWindow(GSM_BTADDR,FALSE,FALSE);
	if (!Get_DeviceLocalAddr(pAddr))
	{
		PLXTipsWin(NULL, NULL, NULL, NOTIFY_UNSUCCESSFUL, NULL, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);	
		return iGSM_Error;
	}
	else
	{
		ShowBTInfo(NULL, pAddr, strlen(pAddr));
	}
	return iGSMBT;
}

static int GsmProjectMode(GSMSTRUCT * pGsmStruct)
{
	OperateGSMWindow(GSM_PROJECTMODE,FALSE,FALSE);
	TESTAPP_EngineeringMode();
	return iGSMProjectMode;
}
static int GsmActivateNSLC(GSMSTRUCT * pGsmStruct)
{
	char cNCK[10] = "";
	char * p = "";
	int iPreLength;
	int nodecounter;
	OperateGSMWindow(GSM_ACTIVATENSLC,FALSE,FALSE);
	if (GetNetSimLockBlock()) 
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Code blocked"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, 0);
		return iGSMACTIVENSLC;
	}
	else if (GetNETSimLock_Open())
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Simlock already activated"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
		return iGSMACTIVENSLC;
	}

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);
    p           = pGsmStruct->pPhoneNumber + iPreLength;
	nodecounter = GetNCKParam(p, pHeadNode, cNCK);
	if (1 >=  nodecounter)
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Wrong code"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
		ReleaseMem();
		return iGSMACTIVENSLC;
	}
	else
	{
		char strNCK[12] = "";

        if (strlen(pHeadNode->strMCC) == 0 || strlen(pHeadNode->strMNC) == 0)
        {
            ReleaseMem();
            return -10;
        }
		GetNCK(strNCK);

		if(stricmp(cNCK, strNCK) != 0)
		{
			PLXTipsWin(NULL, NULL, NULL, ML("Wrong code"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
			ReleaseMem();
			return iGSMACTIVENSLC;
		}
		
		if(IsSimLockContradict(SP_SC, pHeadNode->strMCC, pHeadNode->strMNC))
		{
			PLXTipsWin(NULL, NULL, NULL, "Simlock contradiction", ML("Network Simlock"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
			ReleaseMem();
			return iGSMACTIVENSLC;
		}
		
		if (SetNETSimLock_Open(TRUE)) 
		{
			SetNetSimLockCode(pHeadNode, nodecounter -1);
			PLXTipsWin(NULL, NULL, NULL, ML("Simlock activated"), ML("Network Simlock"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
			ReleaseMem();
			return iGSMACTIVENSLC;				
		}
		
		ReleaseMem();
		return iGSMACTIVENSLC;	

//		if (NetSimLockValidate(pHeadNode, nodecounter-1))
//		{
//			//SetNCK(cNCK);
//			if (SetNETSimLock_Open(TRUE)) 
//			{
//				SetNetSimLockCode(pHeadNode, nodecounter -1);
//				PLXTipsWin(NULL, NULL, NULL, ML("Simlock activated"), ML("Network Simlock"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
//				ReleaseMem();
//				return iGSMACTIVENSLC;				
//			}
//			else
//			{
//				PLXTipsWin(NULL, NULL, NULL, ML("Unsuccessful"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
//				ReleaseMem();
//				return iGSMACTIVENSLC;				
//			}
//		}
//		else
//		{
//			PLXTipsWin(NULL, NULL, NULL, ML("Wrong code"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
//			ReleaseMem();
//			return iGSMACTIVENSLC;			
//		}
	}
}

static int GsmDeactivateNSLC(GSMSTRUCT * pGsmStruct)
{
	char cNCK[10] = "";
	char * p = "";
	int iPreLength;
	int nodecounter;
	int remain;
	char strremain[3] = "";
	OperateGSMWindow(GSM_DEACTIVATENSLC,FALSE,FALSE);
	if (GetNetSimLockBlock()) 
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Code blocked"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, 0);
		return iGSMDEACTIVENSLC;
	}
	else if (!GetNETSimLock_Open()) 
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Simlock already deactivated"), ML("Network Simlock"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
		return iGSMDEACTIVENSLC;
	}

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);
    p           = pGsmStruct->pPhoneNumber + iPreLength;
	nodecounter = GetNCKParam(p, pHeadNode, cNCK);
//	InputNCKCounter ++;
	GetNCKRemains(strremain);
	remain = atoi(strremain);
	remain--;
	itoa(remain, strremain, 10);

	SetNCKRemains(strremain);
	if(remain == 0)
//	if (InputNCKCounter >9) 
	{
		SetNetSimLockBlock(TRUE);
		PLXTipsWin(NULL, NULL, NULL, ML("Code blocked"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, 0);
		ReleaseMem();
		return iGSMDEACTIVENSLC;
	}
	if (1 !=  nodecounter)
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Wrong code"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
		ReleaseMem();
		return iGSMDEACTIVENSLC;
	}
	else
	{
		if (NCKValidate(cNCK))
		{
			if (SetNETSimLock_Open(FALSE)) 
			{
				//InputNCKCounter = 0;
				remain = 10;
				itoa(remain, strremain, 10);
				SetNCKRemains(strremain);
				PLXTipsWin(NULL, NULL, NULL, "Simlock deactivated", ML("Network Simlock"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
				ReleaseMem();
				return iGSMDEACTIVENSLC;				
			}
//			else
//			{
//				PLXTipsWin(NULL, NULL, NULL, ML("Unsuccessful"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
//				ReleaseMem();
//				return iGSMDEACTIVENSLC;				
//			}
		}
		else
		{
			PLXTipsWin(NULL, NULL, NULL, ML("Wrong code"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
			ReleaseMem();
			return iGSMDEACTIVENSLC;			
		}
	}
    ReleaseMem();
    return iGSMDEACTIVENSLC;
}

static int GsmCheckNSLC(GSMSTRUCT * pGsmStruct)
{
	char cNCK[10] = "";
	int counter;
	char strOprName[50] = "";

#if defined(_EMULATE_)
    GetOprName("",strOprName,sizeof(strOprName));//at+copn
#endif
	OperateGSMWindow(GSM_CHECKNSLC,FALSE,FALSE);
	if (!GetNetSimLockEnableStatus()) 
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Simlock disabled"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
		return iGSMCheck;

	}
	if (GetNetSimLockBlock()) 
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Code blocked"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, 0);
		return iGSMCheck;
	}

	if (!GetNETSimLock_Open()) 
	{
		PLXTipsWin(NULL, NULL, NULL, "Simlock deactivated", ML("Network Simlock"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
		return iGSMCheck;
	}
	else
	{
		counter = ReadNetSimLock(NULL);
		if (counter<= 0)
		{
			PLXTipsWin(NULL, NULL, NULL, ML("Unsuccessful"), ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
			return iGSMCheck;
		}
		pHeadNode = malloc(sizeof(NSLC)*counter);
        if (pHeadNode == NULL)
        {
            return iGSMCheck;
        }
		ReadNetSimLock(pHeadNode);
        strcpy(cNCK,pHeadNode->strMCC);
        strcat(cNCK,pHeadNode->strMNC);
		if (GetOprName(cNCK,strOprName,sizeof(strOprName)))
		    ShowNetSimLock(pHeadNode, counter,strOprName);
        else
            ShowNetSimLock(pHeadNode, counter,cNCK);

		ReleaseMem();
		return iGSMCheck;
	}
}

static int GsmActivateSPSL(GSMSTRUCT * pGsmStruct)
{
	char cSPCK[10] = "";
	char * p = "";
	int iPreLength;
	int nodecounter;
	PSPSIMLOCKCODE pSPLC;

	OperateGSMWindow(GSM_ACTIVATESPSL,FALSE,FALSE);
	if (GetSPSimLockBlock()) 
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Code blocked"), ML("SP Simlock"), Notify_Failure, ML("Ok"), NULL, 0);
		return iGSMACTIVESPSL;
	}
	else if (GetSPSimLock_Open() /*|| GetSPSimLockBlock()*/) 
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Simlock already activated"), ML("SP Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
		return iGSMACTIVESPSL;
	}

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);
    p           = pGsmStruct->pPhoneNumber + iPreLength;
	pSPLC = malloc(sizeof(SPSIMLOCKCODE));
	nodecounter = GetSPCKParam(p, pSPLC, cSPCK);
	if (0 >=  nodecounter)
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Wrong code"), ML("SP Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
		free(pSPLC);
		return iGSMACTIVESPSL;
	}
	else
	{
		char strSPCK[12];

        if (strlen(pSPLC->strGID1) == 0)
        {
            free(pSPLC);
		    return -10;
        }
		memset(strSPCK, 0, 12);
		//compare SPCK 
		GetSPCK(strSPCK);

		if(stricmp(cSPCK, strSPCK) != 0)
		{
			PLXTipsWin(NULL, NULL, NULL, ML("Wrong code"), ML("SP Simlock"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
			free(pSPLC);
			return iGSMACTIVESPSL;
		}

		if(IsSimLockContradict(SP_SC, pSPLC->strMCC, pSPLC->strMNC))
		{
			PLXTipsWin(NULL, NULL, NULL, "Simlock contradiction", ML("SP Simlock"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
			free(pSPLC);
			return iGSMACTIVESPSL;
		}
		
		if (SetSPSimLock_Open(TRUE)) 
		{
			SetSPSimLockCode(pSPLC);
			PLXTipsWin(NULL, NULL, NULL, ML("Simlock activated"), ML("SP Simlock"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
			free(pSPLC);
			return iGSMACTIVESPSL;		
		}

//		if (SPSimLockValidate(pSPLC))
//		{
//		//	SetSPCK(cSPCK);
//			if (SetSPSimLock_Open(TRUE)) 
//			{
//				SetSPSimLockCode(pSPLC);
//				PLXTipsWin(NULL, NULL, NULL, ML("Simlock activated"), ML("SP Simlock"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
//				free(pSPLC);
//				return iGSMACTIVESPSL;		
//			}
//			else
//			{
//				PLXTipsWin(NULL, NULL, NULL, ML("Unsuccessful"), ML("SP Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
//				free(pSPLC);
//				return iGSMACTIVESPSL;			
//			}
//		}
//		else
//		{
//			PLXTipsWin(NULL, NULL, NULL, ML("Wrong code"), ML("SP Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
//			free(pSPLC);
//			return iGSMACTIVESPSL;	
//		}
	}
    ReleaseMem();
    return iGSMDEACTIVENSLC;
}

static int GsmDeactivateSPSL(GSMSTRUCT * pGsmStruct)
{
	char cSPCK[10] = "";
	char * p = "";
	int iPreLength;
	int nodecounter;
	PSPSIMLOCKCODE pSPLC;
	int remain;
	char strremain[3] = "";

	OperateGSMWindow(GSM_DEACTIVATESPSL,FALSE,FALSE);
	if (GetSPSimLockBlock()) 
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Code blocked"), ML("SP Simlock"), Notify_Failure, ML("Ok"), NULL, 0);
		return iGSMDEACTIVESPSL;
	}
	else if (!GetSPSimLock_Open()) 
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Simlock already deactivated"), ML("SP Simlock"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
		return iGSMDEACTIVESPSL;
	}

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);
    p           = pGsmStruct->pPhoneNumber + iPreLength;
	pSPLC = malloc(sizeof(SPSIMLOCKCODE));
	nodecounter = GetSPCKParam(p, pSPLC, cSPCK);
//	InputSPCKCounter ++;
	GetSPCKRemains(strremain);
	remain = atoi(strremain);
	remain--;
	itoa(remain, strremain, 10);
	SetSPCKRemains(strremain);
	if(remain == 0)
//	if (InputSPCKCounter >9) 
	{
		SetSPSimLockBlock(TRUE);
		PLXTipsWin(NULL, NULL, NULL, ML("Code blocked"), ML("SP Simlock"), Notify_Failure, ML("Ok"), NULL, 0);
		free(pSPLC);
		return iGSMDEACTIVESPSL;
	}
	if (0 >=  nodecounter)
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Wrong code"), ML("SP Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
		free(pSPLC);
		return iGSMDEACTIVESPSL;
	}
	else
	{
		if (SPCKValidate(cSPCK))
		{
			if (SetSPSimLock_Open(FALSE)) 
			{
				//InputSPCKCounter = 0;
				remain = 10;
				itoa(remain, strremain, 10);
				SetSPCKRemains(strremain);				
				PLXTipsWin(NULL, NULL, NULL, "Simlock deactivated", ML("SP Simlock"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
				free(pSPLC);
				return iGSMDEACTIVESPSL;
			}
			else
			{
				PLXTipsWin(NULL, NULL, NULL, ML("Unsuccessful"), ML("SP Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
				free(pSPLC);
				return iGSMDEACTIVESPSL;
			}
		}
		else
		{
			PLXTipsWin(NULL, NULL, NULL, ML("Wrong code"), ML("SP Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
			free(pSPLC);
			return iGSMDEACTIVESPSL;
		}
	}
}

static int GsmCheckSPSL(GSMSTRUCT * pGsmStruct)
{
	char cSPCK[10] = "";
//	int counter;
	PSPSIMLOCKCODE pSPLC;

	OperateGSMWindow(GSM_CHECKSPSL,FALSE,FALSE);
	
	if (!GetSPSimLockEnableStatus()) 
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Simlock disabled"), ML("SP Simlock"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
		return iGSMCheck;

	}
	if (GetSPSimLockBlock()) 
	{
		PLXTipsWin(NULL, NULL, NULL, ML("Code blocked"), ML("SP Simlock"), Notify_Failure, ML("Ok"), NULL, 0);
		return iGSMCheck;
	}

	if (!GetSPSimLock_Open()) 
	{
		PLXTipsWin(NULL, NULL, NULL, "Simlock deactivated", ML("SP Simlock"), Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
		return iGSMCheck;
	}
	else
	{
		pSPLC = malloc(sizeof(SPSIMLOCKCODE));
		ReadSPSimLock(pSPLC);
		//add n\operator name
		ShowSPSimLock(pSPLC);
		free(pSPLC);
		return iGSMCheck;
	}
}

static  int     GsmIMEI(GSMSTRUCT * pGsmStruct)
{
    OperateGSMWindow(GSM_IMEI,TRUE,FALSE);
    if (-1 == ME_GetEquipmentId(pGsmStruct->hWnd,pGsmStruct->iCallMsg))
    {
        strcpy(pFail,IMEIFAIL);
        return iGSM_Error;
    }
    return iGSM;
}

static  int     GsmSecurity(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;
    char * p = NULL;
    const int iSecurity[] = {LOCK_SC,LOCK_P2,PUK1,PUK2};
    const int iGSMOperate[] = {GSM_PIN1,GSM_PIN2,GSM_PUK1,GSM_PUK2};

	nSecurity = pGsmStruct->iStation;
    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);
    p           = pGsmStruct->pPhoneNumber + iPreLength;

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);

    if (3 != GetGSMParam(p,pParam1,pParam2,pParam3))
        return iUnGSM;

	nQueryCodeType = iGSMOperate[pGsmStruct->iStation];
	
	if ((strlen(pParam1) == 0) || (strlen(pParam2) == 0) || (strlen(pParam3) == 0))
	{
		switch(nQueryCodeType)
		{
		case GSM_PIN1:
			PLXTipsWin(NULL, NULL, NULL, NOTIFY_WRONG_PINCODE, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
			return iGSM_Param;
		case GSM_PIN2:
			PLXTipsWin(NULL, NULL, NULL, NOTIFY_WRONG_PIN2, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
			return iGSM_Param;
		case GSM_PUK1:
			PLXTipsWin(NULL, NULL, NULL, NOTIFY_WRONG_PUKCODE, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
			return iGSM_Param;
		case GSM_PUK2:
			PLXTipsWin(NULL, NULL, NULL, NOTIFY_WRONG_PUK2CODE, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
			return iGSM_Param;
		}					
	}
	if (strcmp(pParam2,pParam3) != 0)
	{
		//        AppMessageBox(NULL,GSM_WRONGPASSWORD,TITLECAPTION,WAITTIMEOUT);
		PLXTipsWin(NULL, NULL, NULL, NOTIFY_CODESNOTMATCH, CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"),NULL, WAITTIMEOUT);
		return iGSM_Param;
	}
    OperateGSMWindow(iGSMOperate[pGsmStruct->iStation],TRUE,FALSE);
    switch(nQueryCodeType)
    {
    case GSM_PIN1:
        ME_GetPhoneLockStatus(pGsmStruct->hWnd,ME_GETPIN1STATE,LOCK_SC,NULL);
        break;
    case GSM_PIN2:
        ME_ChangeLockPassword(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
            iSecurity[pGsmStruct->iStation],pParam1,pParam2);
        break;
    case GSM_PUK1:
        ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
            pGsmStruct->pPhoneNumber,FALSE);
        //ME_GetCurWaitingPassword(pGsmStruct->hWnd,ME_GETPINSTATE);
        //ME_PasswordValidation(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        //    PUK1,pParam1,pParam2);
        //ME_ChangeLockPassword(pGsmStruct->hWnd,pGsmStruct->iCallMsg,LOCK_SC,pParam1,pParam2);
        break;
    case GSM_PUK2:
        ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
            pGsmStruct->pPhoneNumber,FALSE);
        break;
    }
//	switch(iGSMOperate[pGsmStruct->iStation]) 
//	{
//	case GSM_PIN1:
//		if( -1 == ME_GetPassInputLimit(pGsmStruct->hWnd, CALLBACK_CODE_REMAIN, PIN1))
//			return iGSM_Error;
//		break;
//	case GSM_PIN2:
//		if( -1 == ME_GetPassInputLimit(pGsmStruct->hWnd, CALLBACK_CODE_REMAIN, PIN2))
//			return iGSM_Error;
//		break;
//	case GSM_PUK1:
//		if( -1 == ME_GetPassInputLimit(pGsmStruct->hWnd, CALLBACK_CODE_REMAIN, PUK1))
//			return iGSM_Error;
//		break;
//	case GSM_PUK2:
//		if( -1 == ME_GetPassInputLimit(pGsmStruct->hWnd, CALLBACK_CODE_REMAIN, PUK2))
//			return iGSM_Error;
//		break;
//	}
    return iGSM;
}

static  int     GsmUnlockPS(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;
    char cPassword[PHONENUMMAXLEN] = "",* p = NULL;

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);
    p           = pGsmStruct->pPhoneNumber + iPreLength;

    if (1 != GetGSMParam(p,cPassword,NULL,NULL))
        return iUnGSM;

    if (strlen(cPassword) == 0)
        return iUnGSM;

    sprintf(pParam1,GSMUNLOCKPS,GSMOPERATESUCC);
    StrAutoNewLine(NULL,pParam1,pSucc,&hEditRect,NULL);

    sprintf(pParam1,GSMUNLOCKPS,GSMOPERATEFAIL);
    StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);

    OperateGSMWindow(GSM_UNLOCKPS,TRUE,FALSE);
    if (-1 == ME_SetPhoneLockStatus(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        LOCK_PS,cPassword,FALSE))
    {
        return iGSM_Error;
    }
    return iGSM;
}


static  int     GsmForwardingOperate(GSMSTRUCT * pGsmStruct)
{
    int  iPreLength;
    char cSC[10] = "",cClew[30] = "",* p = NULL;
    int  iParam;
    const char * pCFClew[6];
    const int iGSMOperate[] = 
    {
        GSM_CALLFORWARDING1,
        GSM_CALLFORWARDING2,
        GSM_CALLFORWARDING3,
        GSM_CALLFORWARDING4,
        GSM_CALLFORWARDING5,
        GSM_CALLFORWARDING6 
    };

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);
    memset(pParam4,0x00,iParamMaxLen);

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);
    p           = pGsmStruct->pPhoneNumber + iPreLength;
    iGSMBSType  = iUnBSParam;
    iParam      = GetGSMParam(p,pParam1,pParam2,pParam3);

    switch (iParam)
    {
    case 0:
        break;
    case 1:
        if (strlen(pParam1) == 0)
            return iUnGSM;

        break;
    case 2:
        iGSMBSType = InStrArray(pParam2,pGSMCode_BS);
        if (iGSMBSType == -1)
            return iUnGSM;

        break;
    case 3:
        if (strlen(pParam3) == 0)
            return iUnGSM;

        if (strlen(pParam2) != 0)
        {
            iGSMBSType = InStrArray(pParam2,pGSMCode_BS);
            if (iGSMBSType == -1)
                return iUnGSM;
        }
        if (!StrIsNumber(pParam3))
            return iUnGSM;

        GetGSMSC(pGsmStruct->pPhoneNumber,cSC);
        if (InStrArray(cSC,pGSMCode_T_SC) == -1)
            return iUnGSM;

        break;
    default:
        return iUnGSM;
    }
    switch (pGsmStruct->wParam)
    {
    case GSM_CF_ACTIVATE:           
        strcpy(cClew,GSMOPERATEACTIVATE);
        break;
    case GSM_CF_DEACTIVATE:         
        strcpy(cClew,GSMOPERATEDEACTIVATE);
        break;
    case GSM_CF_ACTIVATELOGIN:      
        strcpy(cClew,GSMOPERATE_RA);
        break;
    case GSM_CF_DEACTIVATELOGIN:    
        strcpy(cClew,GSMOPERATE_UD);
        break;
    }
    iCallForward = pGsmStruct->wParam;

    pCFClew[0] = GSMCALLFORWARDING1;
    pCFClew[1] = GSMCALLFORWARDING2;
    pCFClew[2] = GSMCALLFORWARDING3;
    pCFClew[3] = GSMCALLFORWARDING4;
    pCFClew[4] = GSMCALLFORWARDING5;
    pCFClew[5] = GSMCALLFORWARDING6;

    StrAutoNewLine(NULL,pCFClew[pGsmStruct->iStation],pParam2,&hEditRect,NULL);
    StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
    StrAutoNewLine(NULL,cClew,pParam4,&hEditRect,NULL);

    StrAutoNewLine(NULL,GSMOPERATESUCC_1,pParam1,&hEditRect,NULL);
    StringCombination(pSucc,pParam1,pParam2,pParam3,pParam4);

    StrAutoNewLine(NULL,GSMOPERATEFAIL_1,pParam1,&hEditRect,NULL);
    StringCombination(pFail,pParam1,pParam2,pParam3,pParam4);

    OperateGSMWindow(iGSMOperate[pGsmStruct->iStation],TRUE,FALSE);

    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;
    }
    return iGSM;
}

static  int     GsmForwardingCheck(GSMSTRUCT * pGsmStruct)
{
    int     iPreLength;
    char  * p = NULL,cSC[10] = "";
    int     iParam;
    const char *  pCFClew[6];
    const int iGSMOperate[] = 
    {
        GSM_CALLFORWARDING1_CHECK,
        GSM_CALLFORWARDING2_CHECK,
        GSM_CALLFORWARDING3_CHECK,
        GSM_CALLFORWARDING4_CHECK,
        GSM_CALLFORWARDING5_CHECK,
        GSM_CALLFORWARDING6_CHECK 
    };

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);
    p           = pGsmStruct->pPhoneNumber + iPreLength;
    iGSMBSType  = iUnBSParam;
    iParam      = GetGSMParam(p,pParam1,pParam2,pParam3);

    switch (iParam)
    {
    case 0:
        break;
    case 2:
        if ((strlen(pParam1) != 0) || (strlen(pParam2) == 0))
            return iUnGSM;

        iGSMBSType = InStrArray(pParam2,pGSMCode_BS);
        if (iGSMBSType == -1)
            return iUnGSM;

        break;
    case 3:
        if ((strlen(pParam1) != 0) ||   
            (strlen(pParam2) == 0) ||   
            (strlen(pParam3) == 0))     
            return iUnGSM;

        iGSMBSType = InStrArray(pParam2,pGSMCode_BS);
        if (iGSMBSType == -1)
            return iUnGSM;

        GetGSMSC(pGsmStruct->pPhoneNumber,cSC);
        if (InStrArray(cSC,pGSMCode_T_SC) == -1)
            return iUnGSM;

        break;
    default:
        return iUnGSM;
    }

    pCFClew[0] = GSMCALLFORWARDING1;
    pCFClew[1] = GSMCALLFORWARDING2;
    pCFClew[2] = GSMCALLFORWARDING3;
    pCFClew[3] = GSMCALLFORWARDING4;
    pCFClew[4] = GSMCALLFORWARDING5;
    pCFClew[5] = GSMCALLFORWARDING6;

    StrAutoNewLine(NULL,GSMOPERATESEARCH_FAIL,pParam1,&hEditRect,NULL);
    StrAutoNewLine(NULL,pCFClew[pGsmStruct->iStation],pParam2,&hEditRect,NULL);
    StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
    StringCombination(pFail,pParam1,pParam2,pParam3,NULL);

    OperateGSMWindow(iGSMOperate[pGsmStruct->iStation],TRUE,FALSE);

    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;
    }
    return iGSM;
}


static  int     GsmModifyCallBarring(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;
    char * p = NULL;
    int iparam;

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);
    p           = pGsmStruct->pPhoneNumber + iPreLength;
    iparam      = GetGSMParam(p,pParam1,pParam2,pParam3);

    if (3 != iparam)
        return iUnGSM;

    sprintf(pParam1,GSMBARRINGMODIFY,GSMOPERATESUCC);
    StrAutoNewLine(NULL,pParam1,pSucc,&hEditRect,NULL);

    sprintf(pParam1,GSMBARRINGMODIFY,GSMOPERATEFAIL);
    StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);

    OperateGSMWindow(GSM_CALLBAR,TRUE,FALSE);
    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;
    }
    return iGSM;
}

static  int     GsmCallBarringOperate(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;
    char cCallBarring[PHONENUMMAXLEN] = "",cBS[PHONENUMMAXLEN] = "";
    char * p = NULL;
    int iparam;
    const char * pCFClew[8];
    const int iGSMOperate[] = 
    {
        GSM_CALLBARRING33, 
        GSM_CALLBARRING331,
        GSM_CALLBARRING332,
        GSM_CALLBARRING35, 
        GSM_CALLBARRING351,
        GSM_CALLBARRING330,
        GSM_CALLBARRING333,
        GSM_CALLBARRING353 
    };
    
    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);
    memset(pParam4,0x00,iParamMaxLen);

    iPreLength = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);
    iGSMBSType = iUnBSParam;

    p = pGsmStruct->pPhoneNumber + iPreLength;
    iparam = GetGSMParam(p,cCallBarring,cBS,NULL);

    if ((-1 == iparam) || (0 == iparam) || (3 == iparam))
        return iUnGSM;

    if (strlen(cCallBarring) == 0)
        return iUnGSM;

    if (2 == iparam)
    {
        iGSMBSType = InStrArray(cBS,pGSMCode_BS);
        if (iGSMBSType == -1)
            return iUnGSM;
    }
    pCFClew[0] = GSMCALLBARRING33; 
    pCFClew[1] = GSMCALLBARRING331;
    pCFClew[2] = GSMCALLBARRING332;
    pCFClew[3] = GSMCALLBARRING35; 
    pCFClew[4] = GSMCALLBARRING351;
    pCFClew[5] = GSMCALLBARRING330;
    pCFClew[6] = GSMCALLBARRING333;
    pCFClew[7] = GSMCALLBARRING353;

    StrAutoNewLine(NULL,pCFClew[pGsmStruct->iStation],pParam2,&hEditRect,NULL);
    StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
    StrAutoNewLine(NULL,pGsmStruct->wParam ? GSMOPERATEACTIVATE : GSMOPERATEDEACTIVATE,pParam4,&hEditRect,NULL);
	if (pGsmStruct->wParam)
		cbflag = TRUE;
	else
		cbflag = FALSE;
    StrAutoNewLine(NULL,GSMOPERATESUCC_1,pParam1,&hEditRect,NULL);
    StringCombination(pSucc,pParam1,pParam2,pParam3,pParam4);

    StrAutoNewLine(NULL,GSMOPERATEFAIL_1,pParam1,&hEditRect,NULL);
    StringCombination(pFail,pParam1,pParam2,pParam3,pParam4);

    OperateGSMWindow(iGSMOperate[pGsmStruct->iStation],TRUE,FALSE);
    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;
    }
    return iGSM;
}

static  int     GsmCallBarringCheck(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;
    char cBS[PHONENUMMAXLEN] = "",* p = NULL;
    int iparam;
    const char * pCFClew[5];
    const int iGSMOperate[] =
    {
        GSM_CALLBARRING33_CHECK,    
        GSM_CALLBARRING331_CHECK,   
        GSM_CALLBARRING332_CHECK,   
        GSM_CALLBARRING35_CHECK,    
        GSM_CALLBARRING351_CHECK    
    };

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);

    iGSMBSType = iUnBSParam;
    if (pGsmStruct->wParam == GSM_CB_CHECKBS)
    {
        iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);
        p           = pGsmStruct->pPhoneNumber + iPreLength;
        iparam      = GetGSMParam(p,cBS,NULL,NULL);

        if (1 != iparam)
            return iUnGSM;        

        iGSMBSType = InStrArray(cBS,pGSMCode_BS);
        if (iGSMBSType == -1)
            return iUnGSM;
    }
    pCFClew[0] = GSMCALLBARRING33;  
    pCFClew[1] = GSMCALLBARRING331; 
    pCFClew[2] = GSMCALLBARRING332; 
    pCFClew[3] = GSMCALLBARRING35;  
    pCFClew[4] = GSMCALLBARRING351; 

    StrAutoNewLine(NULL,GSMOPERATESEARCH_FAIL,pParam1,&hEditRect,NULL);
    StrAutoNewLine(NULL,pCFClew[pGsmStruct->iStation],pParam2,&hEditRect,NULL);
    StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
    StringCombination(pFail,pParam1,pParam2,pParam3,NULL);

    OperateGSMWindow(iGSMOperate[pGsmStruct->iStation],TRUE,FALSE);

    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;
    }
    return iGSM;
}


static  int     GsmCallWaitingOperate(GSMSTRUCT * pGsmStruct)
{
    char cBS[PHONENUMMAXLEN] = "",* p;
    int iPreLength;
    int iparam;

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);
    p           = pGsmStruct->pPhoneNumber + iPreLength;
    iGSMBSType  = iUnBSParam;
    iparam      = GetGSMParam(p,cBS,NULL,NULL);

    switch (iparam)
    {
    case 0:
        break;
    case 1:
        iGSMBSType = InStrArray(cBS,pGSMCode_BS);
        if (-1 == iGSMBSType)
            return iUnGSM;
        break;
    default:
        return iUnGSM;
    }
    switch (pGsmStruct->iStation)
    {
    case 0:
        StrAutoNewLine(NULL,GSMCALLWAITING,pParam2,&hEditRect,NULL);
        StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
        StrAutoNewLine(NULL,GSMOPERATEACTIVATE,pParam4,&hEditRect,NULL);

        StrAutoNewLine(NULL,GSMOPERATESUCC_1,pParam1,&hEditRect,NULL);
        StringCombination(pSucc,pParam1,pParam2,pParam3,pParam4);

        StrAutoNewLine(NULL,GSMOPERATEFAIL_1,pParam1,&hEditRect,NULL);
        StringCombination(pFail,pParam1,pParam2,pParam3,pParam4);

        OperateGSMWindow(GSM_CALLWAIT_ACTIVE,TRUE,FALSE);
        break;
    case 1:
        StrAutoNewLine(NULL,GSMOPERATESEARCH_FAIL,pParam1,&hEditRect,NULL);
        StrAutoNewLine(NULL,GSMCALLWAITING,pParam2,&hEditRect,NULL);
        StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
        StringCombination(pFail,pParam1,pParam2,pParam3,NULL);

        OperateGSMWindow(GSM_CALLWAIT_CHECK,TRUE,FALSE);
        break;
    case 2:
        StrAutoNewLine(NULL,GSMCALLWAITING,pParam2,&hEditRect,NULL);
        StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
        StrAutoNewLine(NULL,GSMOPERATEDEACTIVATE,pParam4,&hEditRect,NULL);

        StrAutoNewLine(NULL,GSMOPERATESUCC_1,pParam1,&hEditRect,NULL);
        StringCombination(pSucc,pParam1,pParam2,pParam3,pParam4);

        StrAutoNewLine(NULL,GSMOPERATEFAIL_1,pParam1,&hEditRect,NULL);
        StringCombination(pFail,pParam1,pParam2,pParam3,pParam4);

        OperateGSMWindow(GSM_CALLWAIT_DEACTIVE,TRUE,FALSE);
        break;
    }
    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;
    }
    return iGSM;
}


static  int     GsmPNPCheck(GSMSTRUCT * pGsmStruct)
{
    switch (pGsmStruct->iStation)
    {
    case 0:
        sprintf(pParam1,GSMCLIP,GSMOPERATESEARCH,GSMOPERATEFAIL);
        StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);
        OperateGSMWindow(GSM_CLIP_CHECK,TRUE,FALSE);
        break;
    case 1:
        sprintf(pParam1,GSMCLIR,GSMOPERATESEARCH,GSMOPERATEFAIL);
        StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);
        OperateGSMWindow(GSM_CLIR_CHECK,TRUE,FALSE);
        break;
    case 2:
        sprintf(pParam1,GSMCOLP,GSMOPERATESEARCH,GSMOPERATEFAIL);
        StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);
        OperateGSMWindow(GSM_COLP_CHECK,TRUE,FALSE);
        break;
    case 3:
        sprintf(pParam1,GSMCOLR,GSMOPERATESEARCH,GSMOPERATEFAIL);
        StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);
        OperateGSMWindow(GSM_COLR_CHECK,TRUE,FALSE);
        break;
    }
    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
                            pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;
    }
    return iGSM;
}

static  int     GsmCLIRDeal(GSMSTRUCT * pGsmStruct)
{
    switch (pGsmStruct->iStation)
    {
    case 0:
        sprintf(pParam1,GSMCLIRSUPPRESS,GSMOPERATESUCC);
        StrAutoNewLine(NULL,pParam1,pSucc,&hEditRect,NULL);

        sprintf(pParam1,GSMCLIRSUPPRESS,GSMOPERATEFAIL);
        StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);
        
        OperateGSMWindow(GSM_CLIR_SUPPRESS,TRUE,FALSE);
        break;
    case 1:
        sprintf(pParam1,GSMCLIRINVOKE,GSMOPERATESUCC);
        StrAutoNewLine(NULL,pParam1,pSucc,&hEditRect,NULL);

        sprintf(pParam1,GSMCLIRINVOKE,GSMOPERATEFAIL);
        StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);
        
        OperateGSMWindow(GSM_CLIR_INVOKE,TRUE,FALSE);
        break;
    }
    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
                            pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;
    }
    return iGSM;
}


static  void    GetGSMSC(const char * pGsmCode,char * pSC)
{
    char cSC[PHONENUMMAXLEN] = "",* p = NULL;

    p = (char *)pGsmCode;

    while ((* p == Ch_Asterisk) || (* p == Ch_well))
        p++;

    while ((* p != Ch_Asterisk) && (* p != Ch_well))
        strncat(cSC,p++,1);

    strcpy(pSC,cSC);
}

static  int     GetGSMParam(const char * pGsmCode,char * param1,char * param2,char * param3)
{
    char * p1,* p2,* pEnd,* p3;
    int iparam1,iparam2,iparam3;

    pEnd = strstr(pGsmCode,pWell);
    if (NULL == pEnd)
        return -1;

    p3 = pEnd;
    p3++;
    if (* p3 != Ch_End)
        return -1;

    if (* pGsmCode == Ch_Asterisk)
    {
        pGsmCode++;
        if (* pGsmCode == Ch_well)
            return -1;
    }
    if (* pGsmCode == Ch_well)
    {
        if (param1 != NULL)
            strcpy(param1,"");

        if (param2 != NULL)
            strcpy(param2,"");

        if (param3 != NULL)
            strcpy(param3,"");

        return 0;
    }

    p1 = strstr(pGsmCode,pAsterisk);
    if (NULL == p1)
    {
        iparam1 = abs(pGsmCode - pEnd);
        if (param1 != NULL)
        {
            if (0 != iparam1)
                strncpy(param1,pGsmCode,iparam1);
            else
                strcpy(param1,"");
        }
        if (param2 != NULL)
            strcpy(param2,"");

        if (param3 != NULL)
            strcpy(param3,"");
        return 1;
    }
    
    iparam1 = abs(pGsmCode - p1);
    if (param1 != NULL)
    {
        if (0 != iparam1)
            strncpy(param1,pGsmCode,iparam1);
        else
            strcpy(param1,"");
    }

    p1++;

    p2 = strstr(p1,pAsterisk);
    if (NULL == p2)
    {
        iparam2 = abs(p1 - pEnd);
        if (param2 != NULL)
        {
            if (0 != iparam2)
                strncpy(param2,p1,iparam2);
            else
                strcpy(param2,"");
        }
        if (param3 != NULL)
            strcpy(param3,"");

        return 2;
    }
    
    iparam2 = abs(p2 - p1);
    if (param2 != NULL)
    {
        if (0 != iparam2)
            strncpy(param2,p1,iparam2);
        else
            strcpy(param2,"");
    }

    p2++;
    iparam3 = abs(p2 - pEnd);

    if (param3 != NULL)
    {
        if (0 != iparam3)
            strncpy(param3,p2,iparam3);
        else
            strcpy(param3,"");
    }
    return 3;
}

static int GetNCKParam(const char * pGsmSrcCode, PNSLC pHead, char * pNCK)
{
	int		nNodeCount = 0;
	int		ValidLen;
    char *pMcc = NULL, *pMnc = NULL, *pStar = NULL, *pHash = NULL;
	char * pGsmCode, *ptr;
	PNSLC pNewNslc = NULL, pLastNslc = NULL;
	//get NCK and check validity
	if (!pHead) {
		pHead = (PNSLC)malloc(sizeof(NSLC));
		if (!pHead) {
			return 0;
		}
		memset(pHead, 0, sizeof(NSLC));
		pHeadNode = pHead;
	}
	if (!pNCK) {
		return 0;
	}
	if (!pGsmSrcCode) {
		*pNCK = '\0';
		return 0;
	}
	ptr = strchr(pGsmSrcCode, '#');
	if (!ptr) {
		return 0;
	}
	pGsmCode = (char *)malloc(ptr - pGsmSrcCode+2);
	if (!pGsmCode) {
		return 0;
	}
	strncpy(pGsmCode, pGsmSrcCode, ptr - pGsmSrcCode+1);
	pGsmCode[ptr - pGsmSrcCode+1] = '\0';
	if (pStar =strrchr(pGsmCode, '*')) 
	{
		pHash = strrchr(pGsmCode, '#');
		if (!pHash) //No '#'
		{
			*pNCK = '\0';
			return 0;
		}
//		if (pHash < (pGsmCode + strlen(pGsmCode) -1)) //some charactor exist after '#'
//		{
//			*pNCK = '\0';
//			return 0;
//		}
		ValidLen =  pHash - pStar-1;
		if ((ValidLen <=0) ||(ValidLen > 8)) 
		{
			*pNCK = '\0';
			return 0;
		}
		else
			strncpy(pNCK, pStar+1, pHash - pStar-1);
	}
	else
	{
		pHash = strrchr(pGsmCode, '#');
		if (!pHash) 
		{
			*pNCK = '\0';
			return 0;
		}
//		if (pHash < (pGsmCode + strlen(pGsmCode) -1)) //some charactor exist after '#'
//		{
//			*pNCK = '\0';
//			return 0;
//		}
		ValidLen =  pHash - pGsmCode;
		if ((ValidLen <0) ||(ValidLen > 8)) 
		{
			*pNCK = '\0';
			return 0;
		}
		else if(ValidLen == 0)//pGsmCode like: #
		{
			pHead->strMCC[0] = 0;
			pHead->strMNC[0] = 0;
			pHead->pNext = NULL;
			return 1;
		}
		else
		{
			strncpy(pNCK, pGsmCode, pHash - pGsmCode);
			return 1;
		}
	}
	//analyze pGsmCode
	//special condition:the first charactor in pGsmCode is '*'
	if (*pGsmCode == '*') {
		pHead->strMCC[0] = 0;
	}
	else
	{
		pMcc = plxstrtok(pGsmCode, "*");
		if (pMcc != NULL) 
		{
			if (strlen(pMcc)>3) 
			{
				pHead->strMCC[0] = 0;
			}
			else
				strcpy(pHead->strMCC, pMcc);
		}
	}
	if (*pGsmCode == '*') {
		pMnc = plxstrtok(pGsmCode, "*");
	}
	else
		pMnc = plxstrtok(NULL, "*");
	if (pMnc != NULL) 
	{
		if (strlen(pMnc)>2) 
		{
			pHead->strMNC[0] = 0;
		}
		else
			strcpy(pHead->strMNC, pMnc);
	}
	pHead->pNext = NULL;
	pLastNslc = pHead;
	nNodeCount++;
	while (pMnc != NULL) 
	{
		pNewNslc = (PNSLC)malloc(sizeof(NSLC));
		memset(pNewNslc, 0, sizeof(NSLC));
		if (!pNewNslc)
		{
			return 0;
		}
		pMcc = plxstrtok(NULL, "*");
		if (pMcc != NULL) 
		{
			if (strchr(pMcc,'#')) 
			{
				free(pNewNslc);
				pNewNslc = NULL;
				break;
			}
			else if (strlen(pMcc)>3) 
			{
				pNewNslc->strMCC[0] = 0;
			}
			else
				strcpy(pNewNslc->strMCC, pMcc);
		}
		pMnc = plxstrtok(NULL, "*");
		if (pMnc != NULL) 
		{
			if (strchr(pMnc,'#')) 
			{
				pNewNslc->strMNC[0] = 0;
				pLastNslc->pNext = pNewNslc;
				pLastNslc = pNewNslc;
				nNodeCount++;
				break;
			}
			else if (strlen(pMnc)>2) 
			{
				pNewNslc->strMNC[0] = 0;
			}
			else
				strcpy(pNewNslc->strMNC, pMnc);
		}
		pLastNslc->pNext = pNewNslc;
		pLastNslc = pNewNslc;
		nNodeCount++;
	}

	free(pGsmCode);
	pGsmCode = NULL;
	return nNodeCount+1;
}

static	void	ReleaseMem()
{
	PNSLC	ptmp1, ptmp2;
	ptmp2 = ptmp1 = pHeadNode;
	while(ptmp1)
	{
		ptmp2 =  ptmp2->pNext;
		free(ptmp1);
		ptmp1 = ptmp2;
	}
	pHeadNode = NULL;
}

static	int	GetStarCount(char *src)
{
	int	i = 0;
	char	*tmp;
	if (!src) {
		return 0;
	}
	tmp = src;
	while (*tmp++) {
		if (*tmp == '*') {
			i++;
		}
	}
	return i;
}

static int GetSPCKParam(const char * pGsmSrcCode, PSPSIMLOCKCODE pHead, char * pSPCK)
{
	int	iCount,ValidLen;
	char * pGsmCode, *ptr, *pTmpCode;
	char *pMcc = NULL, *pMnc = NULL, *pMidi = NULL, *pStar = NULL, *pHash = NULL;
	
	iCount = GetStarCount((char *)pGsmSrcCode);
	if (iCount > 3) {
		return 0;
	}
	if (!pSPCK) {
		return 0;
	}
	if (!pGsmSrcCode) {
		*pSPCK = '\0';
		return 0;
	}
	ptr = strchr(pGsmSrcCode, '#');
	if (!ptr) {
		return 0;
	}
	if (*pGsmSrcCode == '#') {
		return 0;
	}
	pGsmCode = (char *)malloc(ptr - pGsmSrcCode+2);
	if (!pGsmCode) {
		return 0;
	}
	strncpy(pGsmCode, pGsmSrcCode, ptr - pGsmSrcCode+1);
	pGsmCode[ptr - pGsmSrcCode+1] = '\0';

	pTmpCode = pGsmCode;
	
	if (pStar =strrchr(pGsmCode, '*')) 
	{
		pHash = strrchr(pGsmCode, '#');
		if (!pHash) //No '#'
		{
			*pSPCK = '\0';
			return 0;
		}
		ValidLen =  pHash - pStar-1;
		if ((ValidLen <=0) ||(ValidLen > 8)) 
		{
			*pSPCK = '\0';
			return 0;
		}
		else
			strncpy(pSPCK, pStar+1, pHash - pStar-1);
	}
	else
	{
		pHash = strrchr(pGsmCode, '#');
		if (!pHash) 
		{
			*pSPCK = '\0';
			return 0;
		}
		ValidLen =  pHash - pGsmCode;
		if ((ValidLen <0) ||(ValidLen > 8)) 
		{
			*pSPCK = '\0';
			return 0;
		}
		else if(ValidLen == 0)//pGsmCode like: #
		{
			return 0;
		}
		else
		{
			strncpy(pSPCK, pGsmCode, pHash - pGsmCode);
			return 1;
		}
	}
	if (*pTmpCode == '*') 
	{
		pHead->strMCC[0] = 0;
		pGsmCode++;
		pTmpCode++;
		if (*pTmpCode == '*') 
		{
			//mnc
			pHead->strMNC[0] = 0;
			//GID1
			pGsmCode++;
			pTmpCode++;
			if (*pTmpCode == '*') 
			{
				pHead->strGID1[0] = 0;
				return 1;
			}
			else
			{
				pMidi = plxstrtok(pGsmCode, "*");
				if (!pMidi) {
					return 0;
				}
				if (strchr(pMidi, '#')) { //eg: *#
					pHead->strGID1[0] = 0;
					return 1;
				}
				if (strlen(pMidi) > MECPHSINFOMAXLEN)
				{
					pHead->strGID1[0] = 0;
					return 1;
				}
				else
				{
					strcpy(pHead->strGID1, pMidi);
					return 1;
				}
			}

		}
		else
		{
			//mnc
			pMnc = plxstrtok(pGsmCode, "*");
			if (!pMnc) {
				return 0;
			}
			if (strchr(pMnc, '#')) 
			{ //eg: *#
				pHead->strMNC[0] = 0;
				return 1;
			}
			if ((strlen(pMnc) > 3) || (strlen(pMnc) < 2))//if (strlen(pMnc) > 2)
			{
				pHead->strMNC[0] = 0;
				return 0;//return 1;
			}
			else
				strcpy(pHead->strMNC, pMnc);

			pTmpCode = pTmpCode+strlen(pMnc)+1;
			if (*pTmpCode == '*') 
			{
				pHead->strGID1[0] = 0;
				return 1;
			}
			else
			{
				pMidi = plxstrtok(NULL, "*");
				if (!pMidi) {
					return 0;
				}
				if (strchr(pMidi, '#')) { //eg: *#
					pHead->strGID1[0] = 0;
					return 1;
				}
				if (strlen(pMidi) > MECPHSINFOMAXLEN)
				{
					pHead->strGID1[0] = 0;
					return 1;
				}
				else
				{
					strcpy(pHead->strGID1, pMidi);
					return 1;
				}
			}
		}
	}
	else
	{
		pMcc = plxstrtok(pGsmCode, "*");
		if (pMcc != NULL) 
		{
			if (strlen(pMcc)>3) 
			{
				pHead->strMCC[0] = 0;
			}
			else
				strcpy(pHead->strMCC, pMcc);
		}

		if (*pTmpCode == '*') 
		{
			//mnc
			pHead->strMNC[0] = 0;
			//GID1
			pTmpCode++;
			if (*pTmpCode == '*') 
			{
				pHead->strGID1[0] = 0;
				return 1;
			}
			else
			{
				pMidi = plxstrtok(NULL, "*");
				if (!pMidi) {
					return 0;
				}
				if (strchr(pMidi, '#')) { //eg: *#
					pHead->strGID1[0] = 0;
					return 1;
				}
				if (strlen(pMidi) > MECPHSINFOMAXLEN)
				{
					pHead->strGID1[0] = 0;
					return 1;
				}
				else
				{
					strcpy(pHead->strGID1, pMidi);
					return 1;
				}
			}

		}
		else
		{
			//mnc
			pMnc = plxstrtok(NULL, "*");
			if (!pMnc) {
				return 0;
			}
			if (strchr(pMnc, '#')) 
			{ //eg: *#
				pHead->strMNC[0] = 0;
				return 1;
			}
			if ((strlen(pMnc) > 3) || (strlen(pMnc) < 2))//if (strlen(pMnc) > 2)
			{
				pHead->strMNC[0] = 0;
				return 0;//return 1;
			}
			else
				strcpy(pHead->strMNC, pMnc);

			if (*pTmpCode == '*') 
			{
				pHead->strGID1[0] = 0;
				return 1;
			}
			else
			{
				pMidi = plxstrtok(NULL, "*");
				if (!pMidi) {
					return 0;
				}
				if (strchr(pMidi, '#')) { //eg: *#
					pHead->strGID1[0] = 0;
					return 1;
				}
				if (strlen(pMidi) > MECPHSINFOMAXLEN)
				{
					pHead->strGID1[0] = 0;
					return 1;
				}
				else
				{
					strcpy(pHead->strGID1, pMidi);
					return 1;
				}
			}
		}
	}
}

static  void    StringCombination(char * pResult,const char * p1,const char * p2,
                                  const char * p3,const char * p4)
{
    if (p1 != NULL)
    {
        if (strlen(p1) != 0)
            strcpy(pResult,p1);
    }
    if (p2 != NULL)
    {
        if (strlen(p2) != 0)
        {
            strcat(pResult,pNewLine);
            strcat(pResult,p2);
        }
    }
    if (p3 != NULL)
    {
        if (strlen(p3) != 0)
        {
            strcat(pResult,pNewLine);
            strcat(pResult,p3);
        }
    }
    if (p4 != NULL)
    {
        if (strlen(p4) != 0)
        {
            strcat(pResult,pNewLine);
            strcat(pResult,p4);
        }
    }
}

static  void    SetWindowTextExt(HWND hEdit,const char * pString)
{
    SetWindowText(hEdit,pString ? pString : "");
}

int     GsmSpecific(const char * pNumber)
{
    int iGSMPlace = 0;
    char ** p;

    if (pNumber == NULL)
        return 0;
    if (strlen(pNumber) == 0)
        return 0;

    p = (char**)pGSMSpecific;
    while (**p)
    {
        if (strncmp(* p,pNumber,strlen(* p)) == 0)
            return strlen(pGSMSpecific[iGSMPlace]);

        p++;
        iGSMPlace++;
    }
    return 0;
}


BOOL    USSDDealWith(const char * pString,BOOL bCalling)
{
    int iStrLen;
    BOOL bUssd = FALSE;

    if (pString == NULL)
        return FALSE;

    iStrLen = strlen(pString);
    if (0 == iStrLen)
        return FALSE;

    if (pString[iStrLen - 1] == Ch_well)
    {
        bUssd = TRUE;
    }
    else if (iStrLen < 3)
    {
        if (bCalling)
            bUssd = TRUE;
        else if (!((iStrLen == 2) && (pString[0] == Ch_Num1)))
            bUssd = TRUE;
    }

    if (bUssd)
    {
        pUssd = ME_USSD_Request((char* )pString);
        if (pUssd == NULL)
            return FALSE;

		WaitWindowStateEx(NULL, TRUE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
        return TRUE;
    }
    return FALSE;
}

static  BOOL    USSDDealRespose(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	WaitWindowStateEx(NULL, FALSE, NOTIFY_REQUEST, CAP_NETWORKSERVICE, NULL, NULL);
    switch (wParam)
    {
    case ME_USSD_ABORT:
        ShowWindow(hWnd,SW_HIDE);
        return TRUE;

    case ME_USSD_REFUSE:
		PLXTipsWin(NULL, NULL, NULL, "Unknown subscriber", CAP_NETWORKSERVICE, Notify_Failure, ML("Ok"), NULL, -1);
        StrAutoNewLine(NULL,USSD_NOSERVICE,pParam1,&hEditRect,NULL);
        SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),pParam1);

        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);
//        ShowWindow(hWnd,SW_SHOW);
//        UpdateWindow(hWnd);
        return TRUE;

    case ME_USSD_NOTIFY:
        SetUssdInfo(hWnd, FALSE);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_USSD_LEFTBUTTON,1),(LPARAM)BUTTONTXTLEFT_REPLY);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_USSD_RIGHTBUTTON,(LPARAM)BUTTONTXTRIGHT);
        return TRUE;

    case ME_USSD_REQUEST:
        SetUssdInfo(hWnd, TRUE);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_USSD_LEFTBUTTON,1),(LPARAM)BUTTONTXTLEFT_REPLY);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_USSD_RIGHTBUTTON,(LPARAM)BUTTONTXTRIGHT);
        return TRUE;

    case ME_USSD_DISCONNECT:
		CloseUSSDShowWnd();
		CloseUSSDReplyWnd();
		PLXTipsWin(NULL, NULL, NULL, ML("Operation terminated\nby network"), CAP_NETWORKSERVICE, Notify_Alert, ML("Ok"), NULL, -1);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);
        return TRUE;
    default:
        return FALSE;
    }
}

extern BOOL ShowUSSDInfo(HWND hPWnd, USSD_INFO * ussd_info, BOOL IsResponse);

static  BOOL    SetUssdInfo(HWND hWnd, BOOL isre)
{
    USSD_INFO   ussd_info;
//    char      * pUssdString;

    memset(&ussd_info,0x00,sizeof(USSD_INFO));
    if (-1 == ME_GetUSSDInfo(&ussd_info))
        return FALSE;

    if (ussd_info.DataLen == 0)
        return FALSE;

	ShowUSSDInfo(NULL, &ussd_info, isre);
/*
    pUssdString = (char *)malloc(ussd_info.DataLen + 1);
    if (NULL == pUssdString)
        return FALSE;

    memset(pUssdString,0x00,ussd_info.DataLen + 1);

    if (CBS_ALPHA_UCS2 == ussd_info.Code)
    {
        WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)ussd_info.Data,
            ussd_info.DataLen / 2, (LPSTR)pUssdString, ussd_info.DataLen + 1, 0, 0);
    }
    else
    {
        memcpy(pUssdString,ussd_info.Data,ussd_info.DataLen);
    }
    SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),pUssdString);
    free(pUssdString);
*/
//    ShowWindow(hWnd,SW_SHOW);
//    UpdateWindow(hWnd);

    return TRUE;
}


BOOL GSM_PhoneBookDealWith(HWND hEdit, const char * pPhoneNumber, BOOL * IsFind)
{
    int iLen,iIndex, findret;
    char cString[GSM_PHONEBOOKMAX] = "";
	char cErrorinfo[100]= "";
    
	*IsFind = FALSE;
    if (NULL == pPhoneNumber)
        return FALSE;

    iLen = strlen(pPhoneNumber);
    if ((0 == iLen) || (1 == iLen) || (iLen > GSM_PHONEBOOKMAX))
        return FALSE;

    if (pPhoneNumber[iLen - 1] != Ch_well)
        return FALSE;

    strncpy(cString,pPhoneNumber,iLen - 1);

    if (!StrIsNumber(cString))
        return FALSE;

    iIndex = atoi(cString);
    if (iIndex > iPhoneBookSIMMaxLen)
        return FALSE;

    if (hWndGSM == NULL)
        CreateMBGSMWindow();

    memset(&phonebook,0x00,sizeof(PHONEBOOK));
	if (0 == iIndex)
	{
		CallAppEntryEx("Contacts",ADBOOK_APP, 0);
		return TRUE;
	}
    if ( 0 == GetSIMState()) 
    {
        PLXTipsWin(NULL, NULL, NULL, NOTIFY_NOSIMCARD, NULL, Notify_Failure, ML("Ok"), NULL,WAITTIMEOUT);
        return FALSE;
    }
	findret = APP_GetSIMRecByOrder(iIndex,&phonebook); 
    if (AB_GETSIMREC_INIT == findret)
    {
//        AppMessageBox(NULL,WITHOUTDATA,TITLECAPTION,WAITTIMEOUT);
		PLXTipsWin(NULL,NULL,0, IDS_SIMNOTREADY, NULL,Notify_Alert,ML("Ok"),NULL,WAITTIMEOUT);
        return TRUE;
    }
	if (AB_GETSIMREC_NOTFOUNT == findret) 
	{
		sprintf(cErrorinfo, "%s\r\n%d", IDS_SIMNOTFIND, iIndex);
		PLXTipsWin(NULL,NULL,0, cErrorinfo, NULL,Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
        return TRUE;
	}
    if (strlen(phonebook.PhoneNum) == 0)
    {
		sprintf(cErrorinfo, "%s\r\n%d", IDS_SIMNOTFIND, iIndex);
		PLXTipsWin(NULL,NULL,0, cErrorinfo, NULL,Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
        return TRUE;
    }
    if (!IfValidPhoneNumber(phonebook.PhoneNum))
    {
		sprintf(cErrorinfo, "%s\r\n%d", IDS_SIMNOTFIND, iIndex);
		PLXTipsWin(NULL,NULL,0, cErrorinfo, NULL,Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
        return TRUE;
    }

    SetWindowText(hEdit,phonebook.PhoneNum);
//    ShowWindow(hWndGSM,SW_SHOW);
//    UpdateWindow(hWndGSM);

//    SetFocus(hEdit);
	*IsFind = TRUE;
    return TRUE;
}

BOOL IsGSMWndShow()
{
	return (IsWindowVisible(hWndGSM));
}

void HideGSMWnd()
{
	SendMessage(hWndGSM,WM_COMMAND,(WPARAM)IDC_BUTTON_QUIT,0);
}
static int CallTransferTest(GSMSTRUCT * pGsmStruct)
{
    switch (pGsmStruct->iStation)
    {
    case 0:
        OperateGSMWindow(CALLTRANSFERTEST_ON,TRUE,FALSE);
        break;
    case 1:
        OperateGSMWindow(CALLTRANSFERTEST_OFF,TRUE,FALSE);
        break;
    case 2:
        OperateGSMWindow(GETMENUERROR,TRUE,FALSE);
        break;
    }
    PostMessage(pGsmStruct->hWnd,pGsmStruct->iCallMsg,ME_RS_SUCCESS,0);
    return iGSM;
}

static BOOL GetOprName(const char * pNetNumber,char * strOprName,size_t buflen)
{
    FILE * pFile;
    char cNetNumber[10] = "",cNetName[50] ="";
    int iScanType;//扫描类型,0:运营商代码,1:运营商名称
    BOOL bFindNetNumber = FALSE;//是否找到代码为pNetNumber的运营商
    int ilen,ireadlen;
    char ch;

    pFile = fopen(pNetworkName,"r");

    if (pFile == NULL)
        return FALSE;

    iScanType = 0;
    while(1)
    {
        ireadlen = fread(&ch,sizeof(char),1,pFile);
        if (ireadlen <= 0)
            break;
        if ((ch != ',') && (ch != '\r') && (ch != '\n'))
        {
            if (0 == iScanType)
            {
                ilen = strlen(cNetNumber);
                if (ilen < sizeof(cNetNumber) - 1)
                    cNetNumber[ilen] = ch;
            }
            else
            {
                ilen = strlen(cNetName);
                if (ilen < sizeof(cNetName) - 1)
                    cNetName[ilen] = ch;
            }
        }
        else if (ch == ',')
        {
            iScanType = 1;
            if (strcmp(cNetNumber,pNetNumber) == 0)//找到运营商代码
            {
                bFindNetNumber = TRUE;
            }
        }
        else if ((ch == '\r') || (ch == '\n'))
        {
            iScanType = 0;
            if (bFindNetNumber)
            {
                memset(strOprName,0x00,buflen);
                strncpy(strOprName,cNetName,buflen - 1);
                fclose(pFile);
                return TRUE;
            }
            memset(cNetNumber,0x00,sizeof(cNetNumber));
            memset(cNetName,0x00,sizeof(cNetName));
        }
    }
    fclose(pFile);
    return FALSE;
}
