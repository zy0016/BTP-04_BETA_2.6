    #include    "winpda.h"
#include    "window.h"
#include    "stdio.h"
#include  "mullang.h"
#include    "plx_pdaex.h"
#include    "MBPublic.h"
#include    "hpimage.h"
#include    "setting.h"
#include    "pubapp.h"
#include "MB_control.h"
#include    "RcCommon.h"

#define     DEBUG_MBDIALLINK

#define     TIMER_ID_FRESH          4
#define     IDC_BUTTON_MIDDLE       7
#define     IDC_BUTTON_QUIT_LINK    3
#define     IDC_ME_VOICEDIAL        WM_USER + 100
#define     IDC_ME_CURCALLS         WM_USER + 110
#define     IDC_ME_CUTALL           WM_USER + 120
#define     ME_MSG_GETLASTERROR		WM_USER + 130  //Used for get last ME's error
#define		IDC_BT_DIALOUT			WM_USER + 140
#define     IDC_CLOSEWINDOW         WM_USER + 150

#define     IDC_TIMER_LINK          150
#define     IDC_TIMER_INIT          160
#define     IDC_TIMER_EXIT          180

#define     TITLECAPTION            ML("Calling")
#define     BUTTONQUIT              (LPARAM)ML("End call") 
#define     BUTTON_MIDDLE_TEXT      ML("Dialup") 

#define     DIALLINKINGBUSY         ML("Busy") 
#define     DIALLINKINGHALT         ML("Halt") 
#define     DIALLINKINGHALTFAIL     ML("Call fail") 
#define     DIALLINKINGWAIT         ML("Please wait") 
#define     DIALLING_CALLFORWARD    ML("Cfactivate") 
#define     DIALLING_CALLFORWARDUN  ML("Cfuactivate") 
#define     DIALLING_BARRING        ML("No permission\nto call this number") 
#define     DIALLING_FARE           ML("Call cost limit exceeded") 
#define     STATUS_INFO_EMERGENTCALL           ML("Emergency call") 
#define     STATUS_INFO_OUTGOING    "Calling"
#define     NOTIFY_CALLDIVERTS      ML("call diverts on")
#define     NOTIFY_NONETWORK        "No service"

static const int    iPicY             = 5;
static const char * pClassName        = "MBDialLinkingWndClass";
//static const char * pMBLINKINGPICTURE = MBPIC_DIALLINK;

static HINSTANCE    hInstance;
static char         cPhoneNumber[PHONENUMMAXLEN + 10]="";
static BOOL         bsExtension;
static char         cPhoneNumberExtension[PHONENUMMAXLEN + 10]="";
static char         cPhoneNumberEx[PHONENUMMAXLEN + 10]="";
static SYSTEMTIME   sDialupTime;
static int          iCalling;
static MBConfig     mbconfig;
static HWND         hDailLink = NULL;
static SIZE         PicSize = {0,0};
static PHONEITEM    PhoneItem;
static HWND         hPreWnd;
static BOOL         fStar = FALSE;
static BOOL			bSTKNumber = FALSE;
static int          NetworkType ;
static int          iGsmSpecific;
static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  int     GetActiveCall(CALLS_INFO * Info,int nLimmit);
static  int     GetAlertingCall(CALLS_INFO * Info,int nLimmit);
static  BOOL    IfExitActiveCall(CALLS_INFO * Info,int nLimmit);
static  BOOL    AutoRedialup(void);
static  void    AfterHaltDeal(WPARAM wParam,LPARAM lParam);
static  BOOL    IfExistExtension(char * pPhoneNumber,char * pExtension, BOOL * pSF);
static  BOOL    IfExistAlertingCall(CALLS_INFO * pInfo,int nLimmit);
extern BOOL IsTimeChanged (void);
extern BOOL GetChangeLockStatus();
extern void SetChangeLockStatus();
extern void SetShowSummaryFlag(BOOL f);
extern BOOL GetShowSummaryFlag();
void GetCallErrorString(ME_EXERROR ME_Error,char *strErrorInfo, int * ff);
BOOL GetFlagStar(void);
void SetFlagStarInValid(void);

BOOL    CallMBDialLinkingWindow(HWND hpre, const char * pPhoneCode,
								const SYSTEMTIME * pDialupTime,BOOL bSTK)
{
    WNDCLASS    wc;

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

    switch (GetNetworkStatus())
    {
    case NOSERVICE:
        PLXTipsWin(NULL, NULL, NULL, NOTIFY_NONETWORK, "", Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
        return FALSE;
    case REJECTSIMCARD:
        if (NumberIsEmergent(pPhoneCode))
            break;

        PLXTipsWin(NULL, NULL, NULL, ML("SIM card rejected"), "", Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
        return FALSE;
    }
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    if ((pPhoneCode == NULL) || (pDialupTime == NULL))
    {
        return FALSE;
    }
    else
    {
		bSTKNumber = bSTK;
        strcpy(cPhoneNumber,pPhoneCode);
        memcpy(&sDialupTime,pDialupTime,sizeof(SYSTEMTIME));
    }

    hDailLink = CreateWindow(pClassName,"", 
        WS_CAPTION|PWS_STATICBAR|PWS_NOTSHOWPI, 
        PLX_WIN_POSITION , NULL, NULL, NULL, NULL);
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hDailLink);
	hPreWnd = hpre;
	Set_CallWndOrder(hPreWnd, hDailLink);	
    if (NULL == hDailLink)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	if (!bSTKNumber)
    {
		ShowWindow(hDailLink, SW_SHOW);
		UpdateWindow(hDailLink); 
	}
    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  CALLS_INFO  calls_info[MAX_COUNT];
    static  BOOL        bHangup;
    static  BOOL        bListCurCalls;
    static  BOOL        bCanDail;
    static  BOOL        bEmergent;
	int         iPhoneNum;
	int         nActiveCall;
	LRESULT     lResult;
    static BOOL finab;
	static ABINFO abinfo;
	static BOOL bSetPortrait;
	static  INITPAINTSTRUCT  outgoingif_initpaint;
	static  HGIFANIMATE     hOutgoingGif;
    static  HBITMAP         hBitPortrait;   
    static  SIZE            sPortrait;      
    static  SIZE            sCartoon;       
	COLORREF        Color;
	BOOL            bTran;
	HDC             hdc;
			
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			SYSTEMTIME	SystemTime;
			char aTimeBuf[25], aDateBuf[25];
			
			GetLocalTime(&SystemTime);	
			GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
			SetWindowText(hWnd, aTimeBuf);
			SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
			SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SetTimer(hWnd, TIMER_ID_FRESH, 5000, NULL);

			bCanDail        = FALSE;
			bHangup         = FALSE;
			bListCurCalls   = FALSE;
			SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT_LINK,BUTTONQUIT);
			memset(&abinfo,0x00,sizeof(ABINFO));
			finab = FALSE;
			memset(&PhoneItem,0x00,sizeof(PHONEITEM));
            memset(cPhoneNumberEx,0x00,sizeof(cPhoneNumberEx));
			memset(cPhoneNumberExtension,0x00,sizeof(cPhoneNumberExtension));
			
			iGsmSpecific = GsmSpecific(cPhoneNumber);
			bsExtension  = IfExistExtension(cPhoneNumberEx,cPhoneNumberExtension, &fStar);
            NetworkType = MB_GetSendLocalNumber();
			
			if (bsExtension)
			{
				finab =  GetPhoneItem(&PhoneItem,cPhoneNumberEx + iGsmSpecific, &abinfo); 
				if (bSTKNumber)
				{
					PostMessage(hWnd,IDC_ME_VOICEDIAL,ME_RS_SUCCESS,0);
				}    
				else
				{
                    if (iGsmSpecific != 0)
                    {//已经包含前缀,号码直接拨出
                        iCalling  = ME_VoiceDial(hWnd,IDC_ME_VOICEDIAL,
                            cPhoneNumberEx,NULL);
                    }
                    else
                    {
                        if ((NetworkType == 0) || (NetworkType == 1))
                        {
                            iCalling  = ME_VoiceDial(hWnd,IDC_ME_VOICEDIAL,
                                cPhoneNumberEx,NULL);
                        }
                        else
                        {
                            char NetworkChar[PHONENUMMAXLEN + 10] = "#31#";
                            strcat(NetworkChar,cPhoneNumberEx);
                            iCalling  = ME_VoiceDial(hWnd,IDC_ME_VOICEDIAL,
                                NetworkChar,NULL);
                        }
                    }
				}
				bEmergent = NumberIsEmergent(cPhoneNumberEx + iGsmSpecific);
				SaveMBRecordByPhoneNum(DIALED_REC,cPhoneNumberEx,&sDialupTime);
				MBRecrodList_Refresh(DIALED_REC);
			}
			else
			{
				finab =  GetPhoneItem(&PhoneItem,cPhoneNumber + iGsmSpecific, &abinfo);
				if (bSTKNumber)
				{
					PostMessage(hWnd,IDC_ME_VOICEDIAL,ME_RS_SUCCESS,0);
				}
				else
				{
                    if (iGsmSpecific != 0)//已经包含前缀,号码直接拨出                    
                    {
                        iCalling  = ME_VoiceDial(hWnd,IDC_ME_VOICEDIAL,
                            cPhoneNumber,NULL);
                    }
                    else
                    {
                        if ((NetworkType == 0) || (NetworkType == 1))
                        {
                            iCalling  = ME_VoiceDial(hWnd,IDC_ME_VOICEDIAL,
                                cPhoneNumber,NULL);
                        }
                        else
                        {
                            char NetworkChar[PHONENUMMAXLEN + 10] = "#31#";
                            strcat(NetworkChar,cPhoneNumber);
                            iCalling  = ME_VoiceDial(hWnd,IDC_ME_VOICEDIAL,
                                NetworkChar,NULL);
                        }
                    }
				}
				bEmergent = NumberIsEmergent(cPhoneNumber + iGsmSpecific);
				SaveMBRecordByPhoneNum(DIALED_REC,cPhoneNumber,&sDialupTime);
				MBRecrodList_Refresh(DIALED_REC);
			}			
            //BT_DialOut();//蓝牙拨出
			PostMessage(hWnd,IDC_BT_DIALOUT,0,0);
			if (-1 == iCalling)
			{
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
				printf("\nMB MBDialLink语音拨号失败:%s\n",cPhoneNumber);
#endif
#ifndef _EMULATE_
				MBCallEndWindow(hPreWnd, PhoneItem.cName,cPhoneNumber,NULL,NULL);
				PostMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT_LINK, 0 );
#endif
			}
			
			bSetPortrait = FileIfExist(abinfo.szIcon);
			InitPaint(&outgoingif_initpaint, bSetPortrait, FALSE);
			hdc = GetDC(hWnd);
			GetImageDimensionFromFile(MBPIC_OUTGOINGCALL,&sCartoon);
			hOutgoingGif = StartAnimatedGIFFromFile(hWnd,MBPIC_OUTGOINGCALL,
				(GetScreenUsableWH1(SM_CXSCREEN) - sCartoon.cx) / 2,//outgoingif_initpaint.subcapmid.left,
				outgoingif_initpaint.subcapmid.top, DM_NONE);
			if (bSetPortrait) {
				GetImageDimensionFromFile(abinfo.szIcon,&sPortrait);
				InitPhoto(&outgoingif_initpaint, &sPortrait);
				hBitPortrait = CreateBitmapFromImageFile(hdc,abinfo.szIcon,&Color,&bTran);
			}
			ReleaseDC(hWnd,hdc);        
			SetTimer(hWnd,IDC_TIMER_INIT,1000,NULL);
			
			if (GetUsedItemNum() == 0)
				DisableState();

		}
        break;

	case IDC_BT_DIALOUT:
		BT_DialOut();//蓝牙拨出
		break;
		
    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hWnd);

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;

    case WM_TIMER:
        switch (wParam)
        {
        case IDC_TIMER_EXIT:
            PostMessage(hDailLink,IDC_CLOSEWINDOW,0,0);
            break;
		case TIMER_ID_FRESH:
			{
				SYSTEMTIME	SystemTime;
				char aTimeBuf[25], aDateBuf[25];
				//update time
				if(IsTimeChanged())
				{
					GetLocalTime(&SystemTime);	
					GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
					SetWindowText(hWnd, aTimeBuf);
				}
			}
		break;
		
        case IDC_TIMER_INIT:
            KillTimer(hWnd,IDC_TIMER_INIT);
            ReadMobileConfigFile(&mbconfig);

			if (GetForward_Condition() || GetForward_Uncdtional())
				PLXTipsWin(NULL, NULL, NULL, NOTIFY_CALLDIVERTS, "", Notify_Info, NULL, NULL, WAITTIMEOUT);
#if 0
            calls_info[0].index = 1;
            calls_info[0].Stat = CALL_STAT_ACTIVE;
            calls_info[0].Mode = CALL_MODE_VOICE;
            calls_info[0].Dir = CALL_DIR_ORIGINATED;
            calls_info[0].Mpty = 1;
            strcpy(calls_info[0].PhoneNum,"1234567890");
            memcpy(&PhoneItem.Calls_Info,&calls_info[0],sizeof(CALLS_INFO));
            memcpy(&PhoneItem.TimeInDial,&sDialupTime,sizeof(SYSTEMTIME));
            MBCallingWindow(hPreWnd, &PhoneItem,cPhoneNumberExtension,FALSE);
            DestroyWindow(hWnd);
#endif
            break;

        case IDC_TIMER_LINK:
            KillTimer(hWnd,IDC_TIMER_LINK);

            bListCurCalls = TRUE;
            if (-1 == ME_ListCurCalls(hWnd,IDC_ME_CURCALLS))
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLink,ME_ListCurCalls失败\n");
#endif
                bListCurCalls = FALSE;
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(hPreWnd, PhoneItem.cName,cPhoneNumber,NULL,NULL);
            }
            break;
        }
        break;

    case WM_CLOSE:
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT_LINK, 0 );  
        break;

    case IDC_CLOSEWINDOW:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY :
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hDailLink);
        hDailLink       = NULL;
        iCalling        = 0;
        bHangup         = FALSE;
        bListCurCalls   = FALSE;
        if (hOutgoingGif != NULL)
            EndAnimatedGIF(hOutgoingGif);
        hOutgoingGif = NULL;

        UnregisterClass(pClassName,NULL);
//        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        if (GetUsedItemNum() == 0)
            EnableState();

        break;
		
	case WM_PAINT:
		{
			HBRUSH  hbrush;
			HBRUSH  holdbrush;
			HFONT   hfont;
			HFONT   holdfont;
			char  showinfo[ME_PHONENUM_LEN];
			char  statusinfo[20];
			
			hdc = BeginPaint(hWnd, NULL);
			hbrush = CreateBrush(BS_SOLID, RGB(166,202,240),0);
			holdbrush = SelectObject(hdc, hbrush);
			FillRect(hdc, &(outgoingif_initpaint.subcap), hbrush);
			SelectObject(hdc, holdbrush);
			DeleteObject(hbrush);
			PaintAnimatedGIF(hdc,hOutgoingGif);
			if (bEmergent) 
			{
				sprintf(statusinfo, STATUS_INFO_EMERGENTCALL);
				DrawText(hdc,statusinfo,strlen(statusinfo),&(outgoingif_initpaint.line2),DT_HCENTER|DT_VCENTER);
			}
			else
			{
				if (bSetPortrait)
				{ 
					StretchBlt(hdc, 
						outgoingif_initpaint.photorect.left,
						outgoingif_initpaint.photorect.top,
						outgoingif_initpaint.photorect.right-outgoingif_initpaint.photorect.left,
						outgoingif_initpaint.photorect.bottom-outgoingif_initpaint.photorect.top,
						(HDC)hBitPortrait,0,0,sPortrait.cx,sPortrait.cy,
						SRCCOPY);
					if (finab == FALSE) 
					{
						if (bsExtension) 
							InitTxt(hdc,cPhoneNumberEx + iGsmSpecific,showinfo, 176, FALSE);
						else
							InitTxt(hdc,cPhoneNumber + iGsmSpecific,showinfo, 176, FALSE);						
					}
					else
						InitTxt(hdc,PhoneItem.cName,showinfo, 176, TRUE);
					if (GetFontHandle(&hfont, 1))
					{
						holdfont = SelectObject(hdc, hfont);
					}
					DrawText(hdc,showinfo,strlen(showinfo),&(outgoingif_initpaint.line4),DT_HCENTER|DT_VCENTER);
					sprintf(statusinfo, STATUS_INFO_OUTGOING);
					DrawText(hdc,statusinfo,strlen(statusinfo),&(outgoingif_initpaint.line3),DT_HCENTER|DT_VCENTER);
					if (GetFontHandle(&hfont, 1))
					{
						SelectObject(hdc, holdfont);
					}
				}
				else
				{
					if (finab == FALSE) 
					{
						if (bsExtension) 
							InitTxt(hdc,cPhoneNumberEx + iGsmSpecific,showinfo, 176, FALSE);
						else
							InitTxt(hdc,cPhoneNumber + iGsmSpecific,showinfo, 176, FALSE);						
					}
					else
						InitTxt(hdc,PhoneItem.cName,showinfo, 176, TRUE);
					if (GetFontHandle(&hfont, 1))
					{
						holdfont = SelectObject(hdc, hfont);
					}
					DrawText(hdc,showinfo,strlen(showinfo),&(outgoingif_initpaint.line3),DT_HCENTER|DT_VCENTER);
					sprintf(statusinfo, STATUS_INFO_OUTGOING);
					DrawText(hdc,statusinfo,strlen(statusinfo),&(outgoingif_initpaint.line2),DT_HCENTER|DT_VCENTER);
					if (GetFontHandle(&hfont, 1))
					{
						SelectObject(hdc, holdfont);
					}
				}
			}
			EndPaint(hWnd, NULL);
		}
		break;

    case WM_TODESKTOP:
        if (0 == wParam)
            return FALSE;
        SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT_LINK,0);
        return FALSE;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_QUIT_LINK:
            bHangup = TRUE;
            if (bListCurCalls)
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLink选择了挂断键,正在发送列当前通话的消息\n");
#endif
                AppMessageBox(hWnd,DIALLINKINGWAIT,"", WAITTIMEOUT);
                return TRUE;
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,ME_CancelFunction,iCalling:%d\n",iCalling);
#endif
            BT_Hangup();//蓝牙挂断
            if (-1 == ME_CancelFunction(iCalling))
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLink,挂断失败,挂断激活的通话\n");
#endif
                
                if (-1 == ME_SwitchCallStatus(hWnd,IDC_ME_CUTALL,CUT_ACTIVED,0))
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                    printf("\nMB MBDialLink,ME_SwitchCallStatus,挂断激活的通话失败\n");
#endif
                    DestroyWindow(hWnd);
                }
                if (GetChangeLockStatus()) 
                {
                    SetChangeLockStatus();
                    DlmNotify(PS_LOCK_ENABLE, TRUE);
                }
                PostMessage(hWnd,IDC_CLOSEWINDOW,0,0);
                return TRUE;
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,挂断\n");
#endif
			if (GetChangeLockStatus()) 
			{
				SetChangeLockStatus();
				DlmNotify(PS_LOCK_ENABLE, TRUE);
			}
            AfterHaltDeal(wParam,lParam);
            DestroyWindow(hWnd);
            break;
        }
        break;
		
	case ME_MSG_GETLASTERROR:
        {
            ME_EXERROR LastError;
            char ErrorInfo[50] = "";
            int tiptype = 0;
            if(ME_GetResult(&LastError,sizeof(ME_EXERROR))!=-1)
            {
                GetCallErrorString(LastError,ErrorInfo, &tiptype);
                if (tiptype != 0)
                {
                    AfterHaltDeal(wParam,lParam);
                    PLXTipsWin(NULL,NULL,0,ErrorInfo,"",tiptype,ML("Ok"),NULL,WAITTIMEOUT);
                }
                else
                {
                    AfterHaltDeal(wParam,lParam);
                    MBCallEndWindow(hPreWnd, PhoneItem.cName,cPhoneNumber,NULL,NULL);
                }
            }
        }		
        break;

    case IDC_ME_VOICEDIAL:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (NumberIsEmergent(cPhoneNumber))
            {
                BT_RemoteAnswer();//远端接听
                calls_info[0].Stat = CALL_STAT_ACTIVE;
                strcpy(PhoneItem.cName,STATUS_INFO_EMERGENTCALL);
                memcpy(&PhoneItem.Calls_Info,&calls_info[0],sizeof(CALLS_INFO));
                memcpy(&PhoneItem.TimeInDial,&sDialupTime,sizeof(SYSTEMTIME));
                MBCallingWindow(hPreWnd, &PhoneItem,cPhoneNumberExtension,FALSE);
                DestroyWindow(hWnd);
            }
            else
            {
                SetTimer(hWnd,IDC_TIMER_LINK,500,NULL);
            }
            break;

		case ME_RS_FAILURE:
            BT_HangupNetwork();//蓝牙无信号挂断调用
			if (!AutoRedialup())
			{
                if (OperateLinkError(hWnd,wParam,lParam))
				    AfterHaltDeal(wParam,lParam);
			}
			break;

        case ME_RS_USERSTOP:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("mb MBDialLink 用户中断操作成功,取消拨号\r\n");
#endif
			BT_Hangup();
            AfterHaltDeal(wParam,lParam);
//            MBCallEndWindow(hPreWnd, PhoneItem.cName,cPhoneNumber,NULL,NULL);
            break;

        case ME_RS_BUSY: //line busy
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB DialLink线路忙\n");
#endif
            BT_Terminal();//远端忙
            if (!AutoRedialup())
            {
                //OperateLinkError(hWnd,wParam,lParam);
                PLXTipsWin(NULL, NULL, NULL, ML("Unsuccessful"), "", Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
                AfterHaltDeal(wParam,lParam);
            }
            break;

        case ME_RS_NODIALTONE:
            if (!AutoRedialup())
			{
                if (OperateLinkError(hWnd,wParam,lParam))
				    AfterHaltDeal(wParam,lParam);
            }
            break;

        case ME_RS_NOCARRIER:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB DialLink通话中断,wParam:%ld,lParam:%ld\n",wParam,lParam);
#endif
            BT_HangupNetwork();//蓝牙无信号挂断调用
            PLXTipsWin(NULL, NULL, NULL, ML("Unsuccessful"), "", Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
            AfterHaltDeal(wParam,lParam);
            //OperateLinkError(hWnd,wParam,lParam);
            break;

        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB DialLink,语音拨号,拨号返回default,wParam:%ld,lParam:%ld\n",wParam,lParam);
#endif
            BT_HangupNetwork();//蓝牙无信号挂断调用
            SendAbortCommand(wParam,lParam);
            if (OperateLinkError(hWnd,wParam,lParam))
                AfterHaltDeal(wParam,lParam);
            break;
        }
        break;

    case IDC_ME_CURCALLS:
        bListCurCalls = FALSE;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
        printf("\nMB MBDialLink列出当前所有通话,wParam=%ld\n",wParam);
#endif
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (bHangup)
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLink选择了挂断键\n");
#endif
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT_LINK,0);
                return TRUE;
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink列出当前所有通话\n");
#endif
            memset(calls_info,0x00,sizeof(calls_info));
            iPhoneNum = ME_GetResult(calls_info,sizeof(calls_info));

            if (iPhoneNum == -1)
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("MB MBDialLink,iPhoneNum == -1\r\n");
#endif
                if (!AutoRedialup())
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                    printf("mb MBDialLink iPhoneNum == -1\r\n");
#endif
                    ME_SwitchCallStatus(hWnd,IDC_ME_CUTALL,CUT_ACTIVED,0);
                    if (OperateLinkError(hWnd,wParam,lParam))
                        AfterHaltDeal(wParam,lParam);

                    //MBCallEndWindow(hPreWnd, PhoneItem.cName,cPhoneNumber,NULL,NULL);
                }
                return TRUE;
            }

            iPhoneNum = iPhoneNum / sizeof(CALLS_INFO);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,iPhoneNum:%d\n",iPhoneNum);
#endif
            //如果电话正常拨出,那么clcc结构的个数应该多于当前通话个数
            if (iPhoneNum <= GetUsedItemNum())
            {
                break;
            }
            nActiveCall = GetActiveCall(calls_info,iPhoneNum);
            if (-1 == nActiveCall)
            {
                if (NumberIsEmergent(cPhoneNumber))
                {
                    nActiveCall = GetAlertingCall(calls_info,iPhoneNum);
                    if (nActiveCall != -1)
                    {
                        if (mbconfig.bGetClew)
                            PlayClewMusic(1);

                        BT_RemoteAnswer();//远端接听
                        calls_info[nActiveCall].Stat = CALL_STAT_ACTIVE;
                        //strcpy(calls_info[nActiveCall].PhoneNum,cPhoneNumber);
                        strcpy(PhoneItem.cName,STATUS_INFO_EMERGENTCALL);
                        memcpy(&PhoneItem.Calls_Info,&calls_info[nActiveCall],sizeof(CALLS_INFO));
                        memcpy(&PhoneItem.TimeInDial,&sDialupTime,sizeof(SYSTEMTIME));
                        MBCallingWindow(hPreWnd, &PhoneItem,cPhoneNumberExtension,FALSE);
                        DestroyWindow(hWnd);
                    }
                    else
                    {
                        SetTimer(hWnd,IDC_TIMER_LINK,500,NULL);
                    }
                    return TRUE;    
                }
                
                if (!AutoRedialup())
                {
                    if (!IfExitActiveCall(calls_info,iPhoneNum))
                    {
                        ME_SwitchCallStatus(hWnd,IDC_ME_CUTALL,CUT_ACTIVED,0);

                        PostMessage(hWnd,IDC_ME_VOICEDIAL,ME_RS_NOCARRIER,0);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                        printf("\nMB MBDialLink,IfExitActiveCall\n");
#endif
                        return TRUE;
                    }
                    SetTimer(hWnd,IDC_TIMER_LINK,500,NULL);
                }
                return TRUE;
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,当前电话号码:%s,%d\n",calls_info[nActiveCall].PhoneNum,strlen(calls_info[nActiveCall].PhoneNum));
#endif

            if (mbconfig.bGetClew)
                PlayClewMusic(1);

            BT_RemoteAnswer();//远端接听
            memcpy(&PhoneItem.Calls_Info,&calls_info[nActiveCall],sizeof(CALLS_INFO));
            memcpy(&PhoneItem.TimeInDial,&sDialupTime,sizeof(SYSTEMTIME));
            MBCallingWindow(hPreWnd, &PhoneItem,cPhoneNumberExtension,FALSE);
            DestroyWindow(hWnd);
            break;

        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,列出当前所有通话返回default,wParam:%ld\n",wParam);
#endif
            if (!AutoRedialup())
            {
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(hPreWnd, PhoneItem.cName,cPhoneNumber,NULL,NULL);
            }
            break;
        }
        break;

    case IDC_ME_CUTALL:
        switch(wParam)
        {
        case ME_RS_SUCCESS:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,IDC_ME_CUTALL挂断激活的通话\n");
#endif
            //AfterHaltDeal(wParam,lParam);
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,IDC_ME_CUTALL挂断激活的通话失败\n");
#endif
            //SendAbortCommand(wParam,lParam);
            //DestroyWindow(hWnd);
            break;
        }
        break;
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
void    SendMBDialLinkDialInfo(int iType)
{
    if (!NumberIsEmergent(cPhoneNumber))
    {
        PostMessage(hDailLink,IDC_ME_VOICEDIAL,iType,0);
    }
}
static  BOOL    AutoRedialup(void)
{
    char NetworkChar[PHONENUMMAXLEN + 10] = "#31#";
    if (!mbconfig.bAutoRedial)
        return FALSE;

    if (mbconfig.iAutoRedial != -1)
    {
        if (mbconfig.iAutoRedial == 0)
            return FALSE;
        mbconfig.iAutoRedial--;
    }
    if (bsExtension)
    {
        if (iGsmSpecific != 0)
        {
            iCalling = ME_VoiceDial(hDailLink,IDC_ME_VOICEDIAL,cPhoneNumberEx,NULL);
        }
        else
        {
            if ((NetworkType == 0) || (NetworkType == 1))
            {
                iCalling  = ME_VoiceDial(hDailLink,IDC_ME_VOICEDIAL,cPhoneNumberEx,NULL);
            }
            else
            {
                strcat(NetworkChar,cPhoneNumberEx);
                iCalling  = ME_VoiceDial(hDailLink,IDC_ME_VOICEDIAL,
                    NetworkChar,NULL);
            }
        }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
        printf("mb MBDialLink 自动重拨:%s\r\n",cPhoneNumberEx);
#endif
    }
    else
    {
        if (iGsmSpecific != 0)
        {
            iCalling = ME_VoiceDial(hDailLink,IDC_ME_VOICEDIAL,cPhoneNumber,NULL);
        }
        else
        {
            if ((NetworkType == 0) || (NetworkType == 1))
            {
                iCalling  = ME_VoiceDial(hDailLink,IDC_ME_VOICEDIAL,cPhoneNumber,NULL);
            }
            else
            {
                strcat(NetworkChar,cPhoneNumber);
                iCalling  = ME_VoiceDial(hDailLink,IDC_ME_VOICEDIAL,
                    NetworkChar,NULL);
            }
        }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
        printf("mb MBDialLink 自动重拨:%s\r\n",cPhoneNumber);
#endif
    }
    
    if (-1 == iCalling)
    {
        MBCallEndWindow(hPreWnd,PhoneItem.cName,cPhoneNumber,NULL,NULL);
        PostMessage(hDailLink, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT_LINK, 0 );
        return FALSE;
    }
    return TRUE;
}

static  int     GetActiveCall(CALLS_INFO * Info,int nLimmit)
{
    int i;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
    
#endif
    for (i = 0;i < nLimmit;i++)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
        
#endif
        if (CALL_STAT_ACTIVE == Info[i].Stat)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,GetActiveCall,号码:%s,i:%d\n",Info[i].PhoneNum,i);
#endif
            return i;
        }
    }
    return -1;
}
static  int     GetAlertingCall(CALLS_INFO * Info,int nLimmit)
{
    int i;

    for (i = 0;i < nLimmit;i++)
    {
        if (CALL_STAT_ALERTING == Info[i].Stat)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,GetActiveCall,号码:%s,i:%d\n",Info[i].PhoneNum,i);
#endif
            return i;
        }
    }
    return -1;
}
static  BOOL    IfExitActiveCall(CALLS_INFO * Info,int nLimmit)
{
    int i;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
    
#endif
    for (i = 0;i < nLimmit;i++)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
        
#endif
		if (bsExtension)
		{
			if (strcmp(Info[i].PhoneNum,cPhoneNumberEx) == 0)
			{
				return TRUE;
			}
		}
		else
		{
			if (strcmp(Info[i].PhoneNum,cPhoneNumber) == 0)
			{
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
				printf("\nMB MBDialLink,IfExitActiveCall,号码:%s,i:%d\n",Info[i].PhoneNum,i);
#endif
				return TRUE;
			}
		}
	}
    return FALSE;
}

static  BOOL    IfExistAlertingCall(CALLS_INFO * pInfo,int nLimmit)
{
    int i;
    for (i = 0;i < nLimmit;i++)
    {
        if (pInfo[i].Stat == CALL_STAT_ALERTING)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("MB MBDialLink IfExistAlertingCall:%s is alerting\r\n",pInfo[i].PhoneNum);
#endif
            return TRUE;
        }
    }
    return FALSE;
}

static  void    AfterHaltDeal(WPARAM wParam,LPARAM lParam)
{
	SYSTEMTIME timeused;
	memset(&timeused, 0, sizeof(SYSTEMTIME));
    MBListCurCalls(LISTCURCALLS_REPEAT,wParam,lParam);
	if (bsExtension)
		SaveMBRecordLog(DIALED_REC, cPhoneNumberEx, &sDialupTime,&timeused);
	else
	    SaveMBRecordLog(DIALED_REC, cPhoneNumber, &sDialupTime,&timeused);

    SetTimer(hDailLink,IDC_TIMER_EXIT,2000,NULL);//DestroyWindow(hDailLink);
}

static  BOOL    IfExistExtension(char * pPhoneNumber,char * pExtension, BOOL * pSF)
{
    int i,iPhoneLen;
    int iExtensionPlace[PHONENUMMAXLEN];
    int iExtension = 0;
    int iLastExtension = 0;
    BOOL bExtension = FALSE;

    iPhoneLen = strlen(cPhoneNumber);
    memset(iExtensionPlace,-1,sizeof(iExtensionPlace));

    for (i = 0;i < iPhoneLen;i++)
    {
        if (cPhoneNumber[i] == EXTENSIONCHAR || cPhoneNumber[i] == EXTENSIONSTAR)
        {
            bExtension = TRUE;
			if (cPhoneNumber[i] == EXTENSIONSTAR)
				*pSF = TRUE;
			else
				*pSF = FALSE;
            if (iExtensionPlace[iExtension] == -1)
            {
                iExtensionPlace[iExtension++] = i;
                if (i > iLastExtension)
                    iLastExtension = i;
            }
        }
    }
    if (!bExtension)
	{
		*pSF = FALSE;
        return FALSE;
	}
    strncpy(pPhoneNumber,cPhoneNumber,iExtensionPlace[0]);
	pPhoneNumber[iExtensionPlace[0]]= 0;

    for (i = 0;i < PHONENUMMAXLEN - 1;i++)
    {
        if (iExtensionPlace[i] + 1 < iExtensionPlace[i + 1])
        {   
            strncpy(pExtension,cPhoneNumber + iExtensionPlace[i] + 1,
                iExtensionPlace[i + 1] - iExtensionPlace[i] - 1);
			pExtension[iExtensionPlace[i + 1] - iExtensionPlace[i] - 1]= 0;
            return TRUE;
        }
    }
    strcpy(pExtension,cPhoneNumber + iLastExtension + 1);
    return TRUE;
}

BOOL    IsWindow_DialLink(void)
{
    return (IsWindow(hDailLink));
}
/*
void    bInitMBLink(void)
{
    GetImageDimensionFromFile(pMBLINKINGPICTURE,&PicSize);
}*/
BOOL    DialLink_Hangup(void)//挂断拨号
{
    if (IsWindow(hDailLink))
    {
        PostMessage(hDailLink,WM_COMMAND,IDC_BUTTON_QUIT_LINK,0);
        return TRUE;
    }
    return FALSE;
}

void CloseDialLinkWnd()
{
	SendMessage(hDailLink,WM_COMMAND,IDC_BUTTON_QUIT_LINK,0);
}

// ff means type of flag 0 not tipswin
//                    flag = 4 alert flag =2 fail flag =1 info
void GetCallErrorString(ME_EXERROR ME_Error,char *strErrorInfo, int * ff)
{
    if ((ME_Error.LocationID == 0) && (ME_Error.Reason == 0) && (ME_Error.SSrelease == 0))
    {
        strcpy(strErrorInfo,"No service");
        *ff = Notify_Failure;
        return;
    }

	switch(ME_Error.LocationID) 
	{
	case 8: //call control Error
		switch(ME_Error.Reason) 
        {
		case 21: 
//			strcpy(strErrorInfo,ML("Unknow subscriber"));
//			MBCallEndWindow(hPreWnd, PhoneItem.cName,cPhoneNumber,NULL,NULL);
			strcpy(strErrorInfo,ML("Call rejected"));
			*ff = Notify_Failure;
			break;
		case 17://Busy
//			strcpy(strErrorInfo,ML("Busy"));
//			*ff = Notify_Alert;
//			MBCallEndWindow(hPreWnd, PhoneItem.cName,cPhoneNumber,NULL,NULL);
			*ff = 0;
			break;
		case 19: //No answer
			strcpy(strErrorInfo,ML("No answer"));
			*ff = Notify_Info;
			break;
		case 28: //complete number
			strcpy(strErrorInfo,ML("Please complete\nnumber"));
			*ff = Notify_Alert;
			break;
		case 68://greater ACMMAX
			strcpy(strErrorInfo, ML("Call cost\nlimit exceeded"));
			*ff = Notify_Failure;
			break;
		}
		break;
	case 11:
        switch(ME_Error.Reason) 
        {
        case 5:
        case 4:
            strcpy(strErrorInfo, ML("Call cost\nlimit exceeded"));
            *ff = Notify_Failure;
            break;
        }
        break;
    default:  
        strcpy(strErrorInfo,ML("Unsuccessful"));
        *ff = Notify_Failure;
        break;
	}	
}

BOOL GetFlagStar(void)
{
	return fStar;
}

void SetFlagStarInValid(void)
{
   fStar = FALSE;
}

BOOL OperateLinkError(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    switch( lParam)
	{
	case 257:
        PLXTipsWin(NULL, NULL, NULL, ML("No permission\nto call this number"), "", Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
        return TRUE;//break;
    case 30:
        PLXTipsWin(NULL, NULL, NULL,"No service"/*ML("No network")*/, "",Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
        return TRUE;//break;
    case 262:
    case 256:
        PLXTipsWin(NULL, NULL, NULL, ML("SIM card rejected"), "", Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
        return TRUE;//break;
    case 10:
        PLXTipsWin(NULL, NULL, NULL, ML("No SIM card"), "", Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
        return TRUE;//break;
    default:
        if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
        {
            AfterHaltDeal(wParam,lParam);
            PLXTipsWin(NULL, NULL, NULL, ML("Unsuccessful"), "", Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
        }
        return FALSE;//break;
	}
}
