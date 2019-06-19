   
#include "MBRing.h"
#include "MB_control.h"
#include "setup.h"

#define DEBUG_MBRING
#define TIMER_ID_FRESH 3
static const char * pClassName          = "MBRingAppClass";
static const char * pClassNameConfirm   = "RingConfirmWndClass";
static const char * pCFString           = "%s%s";
static const int    iPortraitX          = 3;
static const int    iPortraitY          = 3;
static const int    iPorCartoonSpace    = 3;
static const int    iPortraitXSpace     = 3;
static const int    iWordSpace          = 3;

static int          iPortraitW;         
static int          iPortraitH;         
static HWND         hWndRingApp = NULL; 
static PHONEITEM    NewPhoneItem;       

static ABINFO       abinfo;         
static BOOL         bHangUp = FALSE;    
static int          nUnanswered = 0;    
static BOOL         bExistEarphone;     
static HINSTANCE    hInstance;
static HWND         hConfirmParent = NULL;
static HWND         hWnd_Confirm = NULL;
static MBConfig     mbconfig;           
static BOOL         bCallForwarding;
static HWND hPreWnd;
static BOOL bSilence;
static BOOL bMMIHangup;

static ACCESSORY_SETUP Accessory_setup_speaker;
static ACCESSORY_SETUP Accessory_setup_headset;
static ACCESSORY_SETUP Accessory_setup_carkit;

extern HWND Get_CallingWnd();
extern HWND Get_DialupWnd();
extern BOOL IsTimeChanged(void);
extern BOOL GetChangeLockStatus();
extern void SetChangeLockStatus();
void SetMMIHangUpFlag(BOOL f);


BOOL    MBPhoneRing(const CALLS_INFO * pNewItemInfo)
{
    WNDCLASS wc;

    if (IsWindow(hWndRingApp))
        return FALSE;

    wc.style         = 0;
    wc.lpfnWndProc   = MBRingAppWndProc;
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

    memset(&NewPhoneItem, 0x00, sizeof(PHONEITEM));
    memcpy(&NewPhoneItem.Calls_Info, pNewItemInfo, sizeof(CALLS_INFO));

    hWndRingApp = CreateWindow(pClassName, 
        "", WS_CAPTION|PWS_STATICBAR|PWS_NOTSHOWPI, 
        PLX_WIN_POSITION, NULL, NULL,NULL, NULL);

	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hWndRingApp);	

    if (NULL == hWndRingApp)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    if (bExistEarphone && (Accessory_setup_headset.auto_ans == SWITCH_ON))
        return TRUE;

    if (bExistEarphone && (Accessory_setup_carkit.auto_ans == SWITCH_ON))
        return TRUE;

    if (Accessory_setup_speaker.auto_ans == SWITCH_ON)
        return TRUE;

    ShowWindow(hWndRingApp, SW_SHOW);
    UpdateWindow(hWndRingApp);
    return TRUE;
}

static LRESULT CALLBACK MBRingAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
    static  HGIFANIMATE     hRingGif;
    static  BOOL            bSetPortrait;   
    static  HBITMAP         hBitPortrait;   
    static  HBITMAP         hBitCartoon;    
    static  SIZE            sPortrait;      
    static  SIZE            sCartoon;       
    static  char            cName[PHONENUMMAXLEN + 1] = "";
    static  char            cPhone[PHONENUMMAXLEN + 1] = "";
	static  INITPAINTSTRUCT ringif_initpaint;
    static  BOOL            bCFClew;        
    static  int             iSW;           
            
            COLORREF        Color;
            BOOL            bTran;
            HDC             hdc;
            LONG            lResult;
    static  int             PhoneGroupNum;  
            int             iSour;
	static  BOOL            bIcon;
	static  BOOL            finab;
	        HMENU           hmenu;      

    lResult = (LRESULT)TRUE;

    switch (wMsgCmd)
    {
    case WM_CREATE :
		{
			SYSTEMTIME	SystemTime;
			char aTimeBuf[25], aDateBuf[25];	
            MBRingInit();
			GetLocalTime(&SystemTime);	
			GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
			SetWindowText(hWnd, aTimeBuf);
			SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
			SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SetTimer(hWnd, TIMER_ID_FRESH, 5000, NULL);
			
			SetMMIHangUpFlag(FALSE);
			ReadMobileConfigFile(&mbconfig);

			bExistEarphone = EarphoneStatus();

            GetAccessorySettingInfo(&Accessory_setup_headset,ACC_HEADSET);//耳机状态
            if (Accessory_setup_headset.auto_ans == SWITCH_ON)//打开
            {
                if (bExistEarphone)//插入一般耳机
                {
                    mbconfig.bModeAuto = TRUE;
					PostMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);
					return TRUE;
                }
            }
            GetAccessorySettingInfo(&Accessory_setup_carkit,ACC_CARKIT);//车载状态
            if (Accessory_setup_carkit.auto_ans == SWITCH_ON)
            {
                if (bExistEarphone)//插入一般耳机
                {
                    mbconfig.bModeAuto = TRUE;
					PostMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);
					return TRUE;
                }
            }
            memset(&Accessory_setup_speaker,0x00,sizeof(ACCESSORY_SETUP));
            GetAccessorySettingInfo(&Accessory_setup_speaker,ACC_SPEAKER);//speaker phone 状态
            if (Accessory_setup_speaker.auto_ans == SWITCH_ON)
            {
                mbconfig.bModeAuto = TRUE;
                PostMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);
                return TRUE;
            }
//			if (bExistEarphone)//插入一般耳机
//			{                
//				if (mbconfig.bModeAuto)
//				{
//					PostMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);
//					return TRUE;
//				}
//			}
			
			PhoneGroupNum = GetPhoneGroupNum();
			if (2 == PhoneGroupNum)
			{
				SendMessage(hWnd,PWM_SETBUTTONTEXT, 1,(LPARAM)"");
				SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_HANGUP,0),(LPARAM)IDS_DROP);
				hmenu = CreateMenu();
				if (FALSE == PDASetMenu(hWnd, hmenu))
				{
					DestroyMenu(hmenu);
					hmenu = NULL;
				}
				if (hmenu != NULL) 
                {
                    if (NewPhoneItem.Calls_Info.Mode == CALL_MODE_VOICE)
                    {
                        InsertMenu(hmenu, 0, MF_BYPOSITION, IDM_ANSWER, IDS_MENU_ANSWER);
                        InsertMenu(hmenu, 1, MF_BYPOSITION, IDM_REPLACE, IDS_MENU_REPLACE);
                        InsertMenu(hmenu, 2, MF_BYPOSITION, IDM_REJECT, IDS_MENU_REJECT);
                    }
                    else
                    {
                        InsertMenu(hmenu, 0, MF_BYPOSITION, IDM_REPLACE, IDS_MENU_REPLACE);
                        InsertMenu(hmenu, 1, MF_BYPOSITION, IDM_REJECT, IDS_MENU_REJECT);
                    }
				}
				SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options"); 
				
			}
			else if (1 == PhoneGroupNum) 
			{
				SendMessage(hWnd,PWM_SETBUTTONTEXT, 1,(LPARAM)"");
				SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_HANGUP,0),(LPARAM)IDS_DROP);
				hmenu = CreateMenu();
				if (FALSE == PDASetMenu(hWnd, hmenu))
				{
					DestroyMenu(hmenu);
					hmenu = NULL;
				}
				if (hmenu != NULL) 
                {
                    if (NewPhoneItem.Calls_Info.Mode == CALL_MODE_VOICE)
                    {
                        InsertMenu(hmenu, 0, MF_BYPOSITION, IDM_ANSWER, IDS_MENU_ANSWER);
                        InsertMenu(hmenu, 1, MF_BYPOSITION, IDM_REPLACE, IDS_MENU_REPLACE);
                        InsertMenu(hmenu, 2, MF_BYPOSITION, IDM_REJECT, IDS_MENU_REJECT);
                    }
                    else
                    {
                        InsertMenu(hmenu, 0, MF_BYPOSITION, IDM_REPLACE, IDS_MENU_REPLACE);
                        InsertMenu(hmenu, 1, MF_BYPOSITION, IDM_REJECT, IDS_MENU_REJECT);
                    }
				}
				SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options"); 
			}
			else if (0 == PhoneGroupNum)
			{
				bSilence = FALSE;
                if (NewPhoneItem.Calls_Info.Mode == CALL_MODE_VOICE)
				    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_SILENCE,1),(LPARAM)IDS_SILENCE);
                else
                    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_SILENCE,1),(LPARAM)"");

				SendMessage(hWnd,PWM_SETBUTTONTEXT,0,(LPARAM)"");
			}
			
			memset(&abinfo,0x00,sizeof(ABINFO));
			finab = FALSE;
			if (MB_GetInfoFromPhonebook(NewPhoneItem.Calls_Info.PhoneNum,&abinfo))
			{
				iSour = strlen(abinfo.szName);
				strncpy(NewPhoneItem.cName,abinfo.szName,sizeof(NewPhoneItem.cName) > iSour ? iSour + 1:sizeof(NewPhoneItem.cName) - 1);
				finab = TRUE;
			}
			
			if (FileIfExist(abinfo.szIcon))
				bSetPortrait = TRUE;
			else
				bSetPortrait = FALSE;
			if (mbconfig.ALS)
				bIcon = TRUE;
			else
				bIcon = FALSE;
			InitPaint(&ringif_initpaint, bSetPortrait, bIcon);
			hdc = GetDC(hWnd);
			GetImageDimensionFromFile(MBPIC_INCOMINGCALL,&sCartoon);
			hRingGif = StartAnimatedGIFFromFile(hWnd,MBPIC_INCOMINGCALL,
				(GetScreenUsableWH1(SM_CXSCREEN) - sCartoon.cx) / 2,//ringif_initpaint.subcapmid.left,
				ringif_initpaint.subcapmid.top, DM_NONE);
			if (bSetPortrait) {
				GetImageDimensionFromFile(abinfo.szIcon,&sPortrait);
				InitPhoto(&ringif_initpaint, &sPortrait);
				hBitPortrait = CreateBitmapFromImageFile(hdc,abinfo.szIcon,&Color,&bTran);
			}
			ReleaseDC(hWnd,hdc);		
			bHangUp         = FALSE;
			bCallForwarding = FALSE;
			MBPlayMusic(GetUsedItemNum());
			
			//        SetTimer(hWnd,IDC_TIMER_BRINGTOTOP,5000,NULL);
        }
        break;

	case WM_UPDATE:
		PhoneGroupNum = GetPhoneGroupNum();
		if (0 == PhoneGroupNum)
		{
			HMENU hmenu;
			hmenu = PDAGetMenu(hWnd);
			if (hmenu != NULL)
				DestroyMenu(hmenu);
			bSilence = FALSE;
            if (NewPhoneItem.Calls_Info.Mode == CALL_MODE_VOICE)
			    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_SILENCE,1),(LPARAM)IDS_SILENCE);
            else
                SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_SILENCE,1),(LPARAM)"");

			SendMessage(hWnd,PWM_SETBUTTONTEXT,0,(LPARAM)"");
			SendMessage(hWnd,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			InvalidateRect(hWnd,NULL,TRUE);
		}
		break;
		
    case IDC_RING_HANGUP:
        PhoneGroupNum = GetPhoneGroupNum();
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
        printf("\nMB MBRing.c IDC_RING_HANGUP,PhoneGroupNum:%d\n",PhoneGroupNum);
#endif
        if (1 == PhoneGroupNum)
        {
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDC_REPLACE_CALL,1),(LPARAM)IDS_VOICE_ANSWER);
            DestroyConfirm();
        }
        break;

    case IDC_FORWARDING:
        if (hRingGif != NULL)
        {
            SIZE PicSize = {0,0};

            EndAnimatedGIF(hRingGif);
            GetImageDimensionFromFile(PIC_INCOMINGCALLDIVERT,&PicSize);//获得动画文件尺寸
            hRingGif = StartAnimatedGIFFromFile(hWnd,PIC_INCOMINGCALLDIVERT,
                (GetScreenUsableWH1(SM_CXSCREEN) - PicSize.cx) / 2,
                ringif_initpaint.subcapmid.top, DM_NONE);
        }
        SetTimer(hWnd,IDC_TIMER_CF,1500,NULL);
        bCallForwarding = TRUE;
        bCFClew         = TRUE;
        InvalidateRect(hWnd,NULL,TRUE);
        UpdateWindow(hWnd);
        break;

    case WM_TIMER:
        switch (wParam)
        {
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

        case IDC_TIMER_CF:
            UpdateWindow(hWnd);
            break;
        case IDC_TIMER_BRINGTOTOP:
            MoveWindowToTop(hWnd);
            break;
        }
        break;

    case WM_COMMAND :
        OnCommandMsg(hWnd, wParam, lParam);
        break;
        
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hWndRingApp);
        UnregisterClass(pClassName, NULL);
		hmenu = PDAGetMenu(hWnd);
		if (hmenu != NULL)
			DestroyMenu(hmenu);
        KillTimer(hWnd,IDC_TIMER_CF);
        if (hRingGif != NULL)
        {
            EndAnimatedGIF(hRingGif);
            hRingGif = NULL;
        }
        if (hBitPortrait != NULL)
        {
            DeleteObject(hBitPortrait);
            hBitPortrait = NULL;
        }
        if (hBitCartoon != NULL)
        {
            DeleteObject(hBitCartoon);
            hBitCartoon = NULL;
        }
        hWndRingApp = NULL;
        printf("\nMB PrioMan_EndCallMusic \n");
        PrioMan_EndCallMusic(PRIOMAN_PRIORITY_PHONE,TRUE);//Call_EndPlayMusic(RING_PHONE);
//        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
        printf("\nMB MBRing向隐藏的消息分发窗口发通知消息,已经处理来电\n");
#endif
        MessageMBPostMsg(WM_CALLING_HANGUP,NULL,NULL);
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hWnd);
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_TODESKTOP:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
        printf("\nmb MBRing WM_TODESKTOP\n");
#endif
        if (0 == wParam)
            return FALSE;
        SendMessage(hWnd,WM_KEYDOWN,VK_F2,0);
        return FALSE;

    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        {
        case VK_F1:
            SendMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);
            break;
        case VK_0:  
        case VK_1:  
        case VK_2:  
        case VK_3:  
        case VK_4:  
        case VK_5:  
        case VK_6:  
        case VK_7:  
        case VK_8:  
        case VK_9:  
			{
                char chSend[2] = "";
				if (GetChangeLockStatus()) 
					break;
                if (mbconfig.bModeAnyKey)
                    SendMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);
				else
				{
					chSend[0] = (LOWORD(wParam) - VK_0 + 48);
					SuperAddDialNumber(hWnd, chSend,FALSE,TRUE, LOWORD(wParam));
				}
			}
            break;
        case VK_F3: 
			{
                char chSend[2] = "";
				if (GetChangeLockStatus()) 
					break;
                if (mbconfig.bModeAnyKey)
                    SendMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);
				else
				{				
					chSend[0] = '*';
					SuperAddDialNumber(hWnd, chSend,FALSE,TRUE, LOWORD(wParam));
				}
			}
            break;

        case VK_F4: 
			{
                char chSend[2] = "";
				if (GetChangeLockStatus()) 
					break;
                if (mbconfig.bModeAnyKey)
                    SendMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);
				else
				{
					chSend[0] = '#';
					SuperAddDialNumber(hWnd, chSend,FALSE,TRUE, LOWORD(wParam));
				}
			}
            break;

        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
            if (0 == GetUsedItemNum())
            {
				if (GetChangeLockStatus()) 
					break;
                if (mbconfig.bModeAnyKey)
                    SendMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);
            }
            break;
        default:
            return  PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
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
			FillRect(hdc, &(ringif_initpaint.subcap), hbrush);
			SelectObject(hdc, holdbrush);
			DeleteObject(hbrush);
			PaintAnimatedGIF(hdc,hRingGif);
			if (bSetPortrait)
			{ 
				StretchBlt(hdc, 
					ringif_initpaint.photorect.left,
					ringif_initpaint.photorect.top,
					ringif_initpaint.photorect.right-ringif_initpaint.photorect.left,
					ringif_initpaint.photorect.bottom-ringif_initpaint.photorect.top,
					(HDC)hBitPortrait,0,0,sPortrait.cx,sPortrait.cy,
					SRCCOPY);
				if (finab == FALSE) 
				{
					if (strlen(NewPhoneItem.Calls_Info.PhoneNum)== 0) 
						sprintf(showinfo, UNKNOWNUMBER);
					else
						InitTxt(hdc,NewPhoneItem.Calls_Info.PhoneNum,showinfo, 176, FALSE);
				}
				else
					InitTxt(hdc,NewPhoneItem.cName,showinfo, 176, TRUE);
				if (GetFontHandle(&hfont, 1))
				{
					holdfont = SelectObject(hdc, hfont);
				}
				DrawText(hdc,showinfo,strlen(showinfo),&(ringif_initpaint.line3),DT_HCENTER|DT_VCENTER);
				if (PhoneGroupNum == 0) 
				{
                    if (NewPhoneItem.Calls_Info.Mode == CALL_MODE_VOICE)
					    sprintf(statusinfo, STATUS_INFO_CALLING);
                    else
                        strcpy(statusinfo,STATUS_INFO_DATACALL);

					DrawText(hdc,statusinfo,strlen(statusinfo),&(ringif_initpaint.line4),DT_HCENTER|DT_VCENTER);
				}
				else if (PhoneGroupNum >0) 
				{
                    if (NewPhoneItem.Calls_Info.Mode == CALL_MODE_VOICE)
                        sprintf(statusinfo, STATUS_INFO_WAITING);
                    else
                        strcpy(statusinfo,STATUS_INFO_DATAWAITING);

					DrawText(hdc,statusinfo,strlen(statusinfo),&(ringif_initpaint.line4),DT_HCENTER|DT_VCENTER);
				}
				if (GetFontHandle(&hfont, 1))
				{
					SelectObject(hdc, holdfont);
				}
			}
			else
			{
				if (finab == FALSE) 
					InitTxt(hdc,NewPhoneItem.Calls_Info.PhoneNum,showinfo, 176, FALSE);
				else
					InitTxt(hdc,NewPhoneItem.cName,showinfo, 176, TRUE);
				if (GetFontHandle(&hfont, 1))
				{
					holdfont = SelectObject(hdc, hfont);
				}
				DrawText(hdc,showinfo,strlen(showinfo),&(ringif_initpaint.line2),DT_HCENTER|DT_VCENTER);

                if (strlen(NewPhoneItem.Calls_Info.PhoneNum)== 0) 
                {
                    sprintf(showinfo, UNKNOWNUMBER);
                    DrawText(hdc,showinfo,strlen(showinfo),&(ringif_initpaint.line2),
                        DT_HCENTER|DT_VCENTER);
                }
				if (PhoneGroupNum == 0) 
				{
                    if (NewPhoneItem.Calls_Info.Mode == CALL_MODE_VOICE)
					    sprintf(statusinfo, STATUS_INFO_CALLING);
                    else
                        strcpy(statusinfo,STATUS_INFO_DATACALL);

					DrawText(hdc,statusinfo,strlen(statusinfo),&(ringif_initpaint.line3),DT_HCENTER|DT_VCENTER);
				}
				else if (PhoneGroupNum > 0) 
				{
                    if (NewPhoneItem.Calls_Info.Mode == CALL_MODE_VOICE)
					    sprintf(statusinfo, STATUS_INFO_WAITING);
                    else
                        strcpy(statusinfo,STATUS_INFO_DATAWAITING);

					DrawText(hdc,statusinfo,strlen(statusinfo),&(ringif_initpaint.line3),DT_HCENTER|DT_VCENTER);
				}
				if (GetFontHandle(&hfont, 1))
				{
					SelectObject(hdc, holdfont);
				}
			}
			EndPaint(hWnd, NULL);
		}
        break;

    case WM_CALLING_HANGUP:
        MBRingHangup();
        break;

    case WM_SWITCH_CALLLINE:
        MBListCurCalls(LISTCURCALLS_HANGUP_ACTIVE,wParam,lParam);
		break;

	case WM_SWITCH_ACTIVECALLLINE:
    case WM_VOICER_ANSWER:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (wMsgCmd == WM_VOICER_ANSWER)
            {
                if (GetUsedItemNum() == 0)
                    DisableState();
            }
            MBRingAnswerCall();
            break;
        default:
            SendAbortCommand(wParam,lParam);
			if (IsWindow(hPreWnd))
			{
			}
			else
				MBCallEndWindow(NULL, NewPhoneItem.cName,NewPhoneItem.Calls_Info.PhoneNum,NULL,NULL);
            DestroyWindow(hWnd);
            break;
        }
        break;

    case WM_VOICER_HANGUP:
        if(bHangUp)
            break;
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            {
				SYSTEMTIME sCurTime;
                int PhoneGroupNum = GetPhoneGroupNum();
                MBHangupOperate();
				bHangUp = TRUE;
                
                if (0 == PhoneGroupNum)
                    BT_Hangup();//蓝牙挂断
				
				SaveMBRecordByPhoneNum(ANSWERED_REC,NewPhoneItem.Calls_Info.PhoneNum,NULL);
				GetLocalTime(&sCurTime);
				SaveMBRecordLog(ANSWERED_REC, NewPhoneItem.Calls_Info.PhoneNum,&sCurTime,NULL);	
				DestroyWindow(hWndRingApp);								
				if (GetUsedItemNum() == 0)
					EnableState();
				
				MBRecrodList_Refresh(ANSWERED_REC);
				MBListCurCalls(LISTCURCALLS_REPEAT,wParam,lParam);
			}
            break;
        default:
            SendAbortCommand(wParam,lParam);
            bHangUp = FALSE;
            PostMessage(hWnd,WM_VOICER_HANGUP,ME_RS_SUCCESS,0);
            //AppMessageBox(hWnd,IDS_HANGUP_FAIL,TITLECAPTION,WAITTIMEOUT);
            break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

extern void SetMissCallIcon();
extern int SaveRecentMissedCall(const char * number);

static  void    MBRingHangup(void)
{
	SYSTEMTIME sCurTime;
    PrioMan_EndCallMusic(PRIOMAN_PRIORITY_PHONE,TRUE);//Call_EndPlayMusic(RING_PHONE);

    MBHangupOperate();

    bHangUp = TRUE;

	if (TRUE == bMMIHangup)
	{
		SetMMIHangUpFlag(FALSE);
		SaveMBRecordByPhoneNum(ANSWERED_REC,NewPhoneItem.Calls_Info.PhoneNum,NULL);
		GetLocalTime(&sCurTime);
		SaveMBRecordLog(ANSWERED_REC, NewPhoneItem.Calls_Info.PhoneNum,&sCurTime,NULL);	
		DestroyWindow(hWndRingApp);	
	}
	else
	{
		SaveMBRecordByPhoneNum(UNANSWER_REC,NewPhoneItem.Calls_Info.PhoneNum,NULL);
        SetMissCallIcon();
        DlmNotify(PS_NEWMSG, NULL);
		SaveRecentMissedCall(NewPhoneItem.Calls_Info.PhoneNum);
		GetLocalTime(&sCurTime);
		SaveMBRecordLog(UNANSWER_REC, NewPhoneItem.Calls_Info.PhoneNum,&sCurTime,NULL);	
		DestroyWindow(hWndRingApp);
	}
    if (IsWindow(hPreWnd))
        MBCallEndWindow(hPreWnd, NewPhoneItem.cName,NewPhoneItem.Calls_Info.PhoneNum,NULL,NULL);
    else
        MBCallEndWindow(NULL, NewPhoneItem.cName,NewPhoneItem.Calls_Info.PhoneNum,NULL,NULL);

    if (GetUsedItemNum() == 0)
        EnableState();

	if (TRUE == bMMIHangup)
	{
		SetMMIHangUpFlag(FALSE);
		MBRecrodList_Refresh(ANSWERED_REC);
	}
	else
		MBRecrodList_Refresh(UNANSWER_REC);
}

static void OnCommandMsg(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int PhoneGroupNum;

    PhoneGroupNum = GetPhoneGroupNum();
    switch (LOWORD(wParam))
    {
	case IDC_SILENCE:
		if (!bSilence) 
		{
			bSilence = TRUE;
			PrioMan_EndCallMusic(PRIOMAN_PRIORITY_PHONE,TRUE);//Call_EndPlayMusic(RING_PHONE);
			SendMessage(hWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		}
		break;

	case IDM_REPLACE:
		if (-1 == ME_SwitchCallStatus(hWnd,WM_SWITCH_CALLLINE,CUT_ACTIVED,0))
			AppMessageBox(hWnd,IDS_ANSWER_FAIL,TITLECAPTION,WAITTIMEOUT);
		break;
	case IDM_ANSWER:
    case IDC_REPLACE_CALL:
        PrioMan_EndCallMusic(PRIOMAN_PRIORITY_PHONE,TRUE);//Call_EndPlayMusic(RING_PHONE);
        if (0 == PhoneGroupNum)
        {
            if (-1 == ME_CallAnswer(hWnd,WM_VOICER_ANSWER))
                AppMessageBox(hWnd,IDS_ANSWER_FAIL,TITLECAPTION,WAITTIMEOUT);
        }
        else if (1 == PhoneGroupNum)
        {
            if (NewPhoneItem.Calls_Info.Mode != CALL_MODE_VOICE)
            {
                PLXTipsWin(NULL,NULL,NULL,UNSUCCESSFUL,"",Notify_Failure, ML("Ok"), NULL,WAITTIMEOUT);
                break;
            }
            if (MAX_COUNT == GetUsedItemNum())
            {
//                GotoConfirmWindow(hWnd);
				PLXTipsWin(NULL, NULL, NULL, NOTIFY_TOOMANYCALLS,"", Notify_Failure, ML("Ok"), NULL,WAITTIMEOUT);
            }
            else
            {
                if (-1 == ME_SwitchCallStatus(hWnd,WM_SWITCH_ACTIVECALLLINE,ACTIVE_SWITCH,0))
                    AppMessageBox(hWnd,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
            }
        }
        else if (2 == PhoneGroupNum)
        {
            if (NewPhoneItem.Calls_Info.Mode != CALL_MODE_VOICE)
            {
                PLXTipsWin(NULL,NULL,NULL,UNSUCCESSFUL,"",Notify_Failure, ML("Ok"), NULL,WAITTIMEOUT);
                break;
            }
			PLXTipsWin(NULL, NULL, NULL, NOTIFY_TOOMANYCALLS,"", Notify_Failure, ML("Ok"), NULL,WAITTIMEOUT);
        }
        break;

//    case IDC_BUTTON_MIDDLE:
//        SuperAddDialNumber(hWnd, NULL,FALSE,TRUE);
//        break;
	case IDM_REJECT:
        PrioMan_EndCallMusic(PRIOMAN_PRIORITY_PHONE,TRUE);//Call_EndPlayMusic(RING_PHONE);
        if (0 == PhoneGroupNum)
        {
            if (-1 == ME_SwitchCallStatus(hWnd,WM_VOICER_HANGUP,CUT_HELD,0))//ME_VoiceHangup(hWnd,WM_VOICER_HANGUP)
                AppMessageBox(hWnd,IDS_HANGUP_FAIL,"",WAITTIMEOUT);
        }
        else if ((1 == PhoneGroupNum) || (2 == PhoneGroupNum))
        {
            if (-1 == ME_SwitchCallStatus(hWnd,WM_VOICER_HANGUP,CUT_HELD,0))
                AppMessageBox(hWnd,IDS_HANGUP_FAIL,"",WAITTIMEOUT);
        }
        break;
    case IDC_HANGUP:
        PrioMan_EndCallMusic(PRIOMAN_PRIORITY_PHONE,TRUE);//Call_EndPlayMusic(RING_PHONE);
        if (0 == PhoneGroupNum)
        {
            if (-1 == ME_SwitchCallStatus(hWnd,WM_VOICER_HANGUP,CUT_HELD,0))//ME_VoiceHangup(hWnd,WM_VOICER_HANGUP)
                AppMessageBox(hWnd,IDS_HANGUP_FAIL,"",WAITTIMEOUT);
        }
        else if ((1 == PhoneGroupNum) || (2 == PhoneGroupNum))
        {
            if (NewPhoneItem.Calls_Info.Mode != CALL_MODE_VOICE)
            {
                MBCalling_Shortcut(CUT_ALL,NULL);//挂断所有通话
                InvalidateRect(hWnd,NULL,TRUE);
                break;
            }
            if (-1 == ME_SwitchCallStatus(hWnd,WM_VOICER_HANGUP,CUT_HELD,0))
                AppMessageBox(hWnd,IDS_HANGUP_FAIL,"",WAITTIMEOUT);
        }
        break;
    }
}

void    GetNewItemInfo(CALLS_INFO * pNewItemInfo)
{
    memset(pNewItemInfo,0x00,sizeof(CALLS_INFO));
    memcpy(pNewItemInfo,&NewPhoneItem.Calls_Info,sizeof(CALLS_INFO));
}

void    MessageMBRing(UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    if ((hWndRingApp != NULL) && (IsWindow(hWndRingApp)))
    {
        if (wMsgCmd == WM_CALLING_HANGUP)
        {
            BT_RemoveCall();//来电自行挂断调用函数
        }
        PostMessage(hWndRingApp,wMsgCmd,wParam,lParam);
    }
}

BOOL    BTAnswerHangup(int iType)//执行蓝牙接听挂断操作
{
    if ((hWndRingApp != NULL) && (IsWindow(hWndRingApp)))
    {
        switch (iType)
        {
        case 1://接听
            if (GetPhoneGroupNum() == 2)
                PostMessage(hWndRingApp,WM_COMMAND,IDM_REPLACE,0);
            else
                PostMessage(hWndRingApp,WM_COMMAND,IDM_ANSWER,0);
            break;
        case 2://挂断
            PostMessage(hWndRingApp,WM_COMMAND,IDC_HANGUP,0);
            break;
        }
        return TRUE;
    }
    return FALSE;
}
BOOL    IsWindow_MBRing(void)
{
    return (IsWindow(hWndRingApp));
}

void    MBRingAnswerCall(void)
{
    SYSTEMTIME  sCurTime;
    char        cNameCF[PHONENUMMAXLEN + 10] = "";
    int         iSour;

    if (0 == GetUsedItemNum())//处理蓝牙
    {
        BT_AnswerCall();//通过来电界面接听,通知蓝牙部分
    }
    if (NewPhoneItem.Calls_Info.Mode != CALL_MODE_VOICE)
    {
        strcpy(NewPhoneItem.cName,STATUS_INFO_DATACALL);
    }
    if (bCallForwarding)
    {
        sprintf(cNameCF,pCFString,CALLFORWARD,NewPhoneItem.cName);
        iSour = strlen(cNameCF);
        strncpy(NewPhoneItem.cName,cNameCF,sizeof(NewPhoneItem.cName) > iSour ? iSour + 1 : sizeof(NewPhoneItem.cName) - 1);
    }
    NewPhoneItem.Calls_Info.Stat = CALL_STAT_ACTIVE;

    SaveMBRecordByPhoneNum(ANSWERED_REC,NewPhoneItem.Calls_Info.PhoneNum,&sCurTime);

    memcpy(&NewPhoneItem.TimeInDial,&sCurTime,sizeof(SYSTEMTIME));
	if (IsWindow(hPreWnd))
    {
        MBCallingWindow(hPreWnd, &NewPhoneItem,NULL,
            Accessory_setup_speaker.auto_ans == SWITCH_ON);
    }
	else
    {
        MBCallingWindow(NULL, &NewPhoneItem,NULL,
            Accessory_setup_speaker.auto_ans == SWITCH_ON);
    }
    MBRecrodList_Refresh(ANSWERED_REC);
    DestroyWindow(hWndRingApp);
}


static BOOL     GotoConfirmWindow(HWND hWnd)
{
    WNDCLASS wc;

    hConfirmParent = hWnd;

    wc.style         = 0;
    wc.lpfnWndProc   = ConfirmWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassNameConfirm;

    if (!RegisterClass(&wc))
        return FALSE;

    hWnd_Confirm = CreateWindow(pClassNameConfirm, 
        IDS_PL_CONFIRM, WS_CAPTION | WS_CLIPCHILDREN | PWS_STATICBAR, 
        PLX_WIN_POSITION,hConfirmParent,NULL,NULL, NULL);

    if (NULL == hWnd_Confirm)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    ShowWindow(hWnd_Confirm, SW_SHOW);
    UpdateWindow(hWnd_Confirm);

    return TRUE;
}

static LRESULT CALLBACK ConfirmWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    HDC     hdc;
    LRESULT lResult;
    RECT    rect;

    lResult = (LRESULT)TRUE;

    switch (wMsgCmd)
    {
    case WM_CREATE:
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)BUTTONOK);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)BUTTON_MIDDLE_TXT);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_QUIT,0),(LPARAM)BUTTONQUIT);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_SET:
            if (-1 == ME_SwitchCallStatus(hWnd,WM_SWITCH_CALLLINE,CUT_ACTIVED,0))
                AppMessageBox(hWnd,IDS_ANSWER_FAIL,TITLECAPTION,WAITTIMEOUT);
            break;

//        case IDC_BUTTON_MIDDLE:
//            SuperAddDialNumber(hWnd,NULL,FALSE,TRUE);
//            break;

        case IDC_BUTTON_QUIT:
            DestroyWindow(hWnd);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd,NULL);
        SetRect(&rect,0,20,GetScreenUsableWH1(SM_CXSCREEN),40);
        StrAutoNewLine(hdc,IDS_CLOSECURCALL,NULL,&rect,DT_CENTER);
        EndPaint(hWnd,NULL);
        break;

    case WM_TODESKTOP:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
        printf("\nmb MBRing_ext WM_TODESKTOP\n");
#endif
        if (0 == wParam)
            return FALSE;
        SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
        return FALSE;

    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        {

        case VK_F1:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        case VK_F2:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        case VK_0:  
        case VK_1:  
        case VK_2:  
        case VK_3:  
        case VK_4:  
        case VK_5:  
        case VK_6:  
        case VK_7:  
        case VK_8:  
        case VK_9:  
            {
                char chSend[2] = "";
                chSend[0] = (LOWORD(wParam) - VK_0 + 48);
                SuperAddDialNumber(hWnd, chSend,FALSE,TRUE, LOWORD(wParam));
            }
            break;
        default:
            return  PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_SWITCH_CALLLINE:
        MBListCurCalls(LISTCURCALLS_HANGUP_ACTIVE,wParam,lParam);
        break;

    case WM_DESTROY:
        hWnd_Confirm = NULL;
        UnregisterClass(pClassNameConfirm,NULL);
//        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam); 
        break;
    }
    return lResult;
}

static  void    DestroyConfirm(void)
{
    if (IsWindow(hWnd_Confirm))
        DestroyWindow(hWnd_Confirm);
}

void    GetCurrentPhoneNum(char * pPhoneNum,unsigned int iBuflen)
{
    strncpy(pPhoneNum,NewPhoneItem.Calls_Info.PhoneNum,iBuflen - 1);
}

void    ResetnUnanswered(void)
{
    nUnanswered = 0;
}

static  void    MBRingInit(void)
{
    DlmNotify(PMM_NEWS_ENABLE,PMF_MAIN_BKLIGHT);//点亮背光
    f_sleep_register(CALL_DATA);//禁止睡眠
    f_DisablePowerOff(INCALL);//禁止自动关机
    DlmNotify(PMM_NEWS_DISABLE, PMF_AUTO_SHUT_MAIN_LIGHT);//禁止关屏,禁止关背光
}

static  void    MBHangupOperate(void)
{
    DlmNotify(PMM_NEWS_ENABLE,PMF_MAIN_BKLIGHT);//点亮背光
    f_sleep_unregister(CALL_DATA);//允许睡眠
    f_EnablePowerOff(INCALL);//允许自动关机
    DlmNotify(PMM_NEWS_ENABLE,PMF_AUTO_SHUT_MAIN_LIGHT);//允许关屏,允许关背光
    DlmNotify(MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 15);
}

BOOL IsRingWnd()
{
	return (IsWindow(hWndRingApp)); 
}

void CloseRingWnd()
{
	SendMessage(hWndRingApp,WM_COMMAND,IDC_HANGUP,0);
}

void NotifyRingWndUpdate()
{
	MessageMBRing(WM_UPDATE, 0, 0);
}

void SetMMIHangUpFlag(BOOL f)
{
	bMMIHangup = f;
}
