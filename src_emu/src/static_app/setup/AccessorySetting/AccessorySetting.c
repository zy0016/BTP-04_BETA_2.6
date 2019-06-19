#include    "AccessorySetting.h"
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

static HWND hWndApp;
static HWND hACC_WndApp;
static HWND hFrameWin;
ACCESSORY_TYPE AccType;
static char * pClassName = "AccessorySettingClass";
static char * pACC_ClassName = "AccessorySettingOptionClass";

static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static LRESULT  AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL     CreateControl(HWND hWnd, HWND * hHeadset, HWND * hCarkit, HWND * hSpeaker);
static BOOL     CallAccSettingList(HWND hFrame, ACCESSORY_TYPE accType);
static LRESULT  ACC_AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL     ACC_CreateControl(HWND hWnd, ACCESSORY_TYPE accType, HWND * hUseProfile, HWND * hAutoAns);

extern BOOL CallAccProfileList(HWND hParentWnd, ACCESSORY_TYPE acc_type, int iIndex);
extern BOOL CallAccAutoAnsList(HWND hParentWnd, ACCESSORY_TYPE acc_type, int iIndex);
extern	void Sett_SetActiveAccSM(ACCESSORY_TYPE AccType, int iProfileIndex);

void SaveAccessoryProfileInfo(ACCESSORY_TYPE AccType, int iProfileIndex);
void SaveAutoAnsState(ACCESSORY_TYPE AccType, SWITCHTYPE AutoAnsState);
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);

BOOL AccessorySetting(HWND hParentWnd)
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hParentWnd;

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

    GetClientRect(hFrameWin, &rClient);

    hWndApp = CreateWindow(pClassName,NULL, 
        WS_VISIBLE | WS_CHILD , 
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hWndApp)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
        (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back") );
    
    SetWindowText(hFrameWin, ML("Accessories"));
    SetFocus(hWndApp);
    
    return (TRUE);
    
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HWND        hHeadset, hCarkit, hSpeaker;
    static      HWND        hFocus;
    static      int         iButtonJust;
	LRESULT     lResult;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
		case WM_CREATE :
            CreateControl(hWnd, &hHeadset,&hCarkit, &hSpeaker);
			hFocus=hHeadset;
        	break;

		case WM_SETFOCUS:
            SetFocus(hFocus);
            break;

        case PWM_SHOWWINDOW:
            SetFocus(hWnd);
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back") );
                
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            SetWindowText(hFrameWin, ML("Accessories"));
            
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
                SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                PostMessage(hWnd, WM_CLOSE, 0, 0);
                break;
            case VK_F5:
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
                break;
            case VK_DOWN:
                SettListProcessKeyDown(hWnd, &hFocus);            
				if ( 1 == RepeatFlag )
				{
					SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
				}
				break;
				
            case VK_UP:
                SettListProcessKeyUp(hWnd, &hFocus);            
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
            break;
		case WM_KILLFOCUS:
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
			wKeyCode   = 0;
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
    		            case IDC_HANDSET:
                            CallAccSettingList(hFrameWin, ACC_HEADSET);
                            break;
		                case IDC_CARKIT:
                            CallAccSettingList(hFrameWin, ACC_CARKIT);
 			                break;
			            case IDC_SPEAKER:
                            CallAccSettingList(hFrameWin, ACC_SPEAKER);
 			                break;
			         }
                     break;
        
		        case IDC_BUTTON_QUIT:
                    DestroyWindow(hFocus);
			        DestroyWindow(hWnd);
                    break;

	        	case IDC_HANDSET:
                    if(HIWORD(wParam) == SSBN_SETFOCUS)
                    {
                        iButtonJust = IDC_HANDSET;
                        hFocus = hHeadset;
                    }
			        break;
            
		        case IDC_CARKIT:
                    if(HIWORD(wParam) == SSBN_SETFOCUS)
                    {
                        iButtonJust = IDC_CARKIT;
                        hFocus = hCarkit;
                    }
			        break;

		        case IDC_SPEAKER:
                    if(HIWORD(wParam) == SSBN_SETFOCUS)
                    {
                        iButtonJust = IDC_SPEAKER;
                        hFocus = hSpeaker;
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


static BOOL CreateControl(HWND hWnd, HWND * hHeadset, HWND * hCarkit, HWND * hSpeaker)
{
    int iControlH, iControlW;
    RECT rect;
    int xzero = 0, yzero = 0;
    GetClientRect(hWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;
    
    * hHeadset = CreateWindow( "SPINBOXEX", ML("Headset"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hWnd, (HMENU)IDC_HANDSET, NULL, NULL);

    if (* hHeadset == NULL)
        return FALSE;

    * hCarkit = CreateWindow( "SPINBOXEX", ML("Car kit"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT| CS_NOSYSCTRL,
        xzero, yzero + iControlH, iControlW, iControlH, 
        hWnd, (HMENU)IDC_CARKIT, NULL, NULL);

    if (* hCarkit == NULL)
        return FALSE;
    * hSpeaker = CreateWindow( "SPINBOXEX", ML("Speaker phone"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT| CS_NOSYSCTRL,
        xzero, yzero + iControlH * 2, iControlW, iControlH, 
        hWnd, (HMENU)IDC_SPEAKER, NULL, NULL);

    if (* hSpeaker == NULL)
        return FALSE;

    return TRUE;

}

static BOOL   CallAccSettingList(HWND hFrame, ACCESSORY_TYPE accType)
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hFrame;

    AccType = accType;

 
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = ACC_AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pACC_ClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

printf("\r\n Setting, register window class success, start create window...\r\n");   

    GetClientRect(hFrameWin, &rClient);

    hACC_WndApp = CreateWindow(pACC_ClassName,NULL, 
        WS_VISIBLE | WS_CHILD , 
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hACC_WndApp)
    {
        UnregisterClass(pACC_ClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
        (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back") );
    switch(accType)
    {
    case ACC_HEADSET:
        SetWindowText(hFrameWin, ML("Headset"));
        break;
    case ACC_CARKIT:
        SetWindowText(hFrameWin, ML("Car kit"));
        break;
    case ACC_SPEAKER:
        SetWindowText(hFrameWin, ML("Speaker phone"));
        break;
    }
    SetFocus(hACC_WndApp);
    
    return (TRUE);
}

static LRESULT ACC_AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HWND        hUseProfile, hAutoAns;
    static      HWND        hFocus;
    static      int         iButtonJust, iCurIndex;
    ACCESSORY_SETUP AccInfoTmp;
	LRESULT     lResult;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
		case WM_CREATE :

printf("\r\n Setting, starting the create message...\r\n");   

            ACC_CreateControl(hWnd, AccType,&hUseProfile,&hAutoAns);
			if(AccType == ACC_HEADSET || AccType == ACC_CARKIT)
            hFocus = hUseProfile;
            else if(AccType == ACC_SPEAKER)
            hFocus = hAutoAns;
        	break;

		case WM_SETFOCUS:
            SetFocus(hFocus);
            break;

        case PWM_SHOWWINDOW:
            SetFocus(hWnd);
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back") );
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            
            GetAccessorySettingInfo(&AccInfoTmp, AccType); 
            
            SendMessage(hUseProfile, SSBM_SETCURSEL, AccInfoTmp.use_profile + 1, 0);
            SendMessage(hAutoAns, SSBM_SETCURSEL, AccInfoTmp.auto_ans, 0);
            
            switch(AccType)
            {
            case ACC_HEADSET:
                SetWindowText(hFrameWin, ML("Headset"));
                break;
            case ACC_CARKIT:
                SetWindowText(hFrameWin, ML("Car kit"));
                break;
            case ACC_SPEAKER:
                SetWindowText(hFrameWin, ML("Speaker phone"));
                break;
            }
            
            break;

	    case WM_DESTROY : 
            hACC_WndApp = NULL;
            UnregisterClass(pACC_ClassName,NULL);
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
                SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                PostMessage(hWnd, WM_CLOSE, 0, 0);
                break;
            case VK_F5:
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
                break;
            case VK_DOWN:
                SettListProcessKeyDown(hWnd, &hFocus);            
				if ( 1 == RepeatFlag )
				{
					SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
				}
				break;
				
            case VK_UP:
                SettListProcessKeyUp(hWnd, &hFocus);            
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
            break;
		case WM_KILLFOCUS:
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
			wKeyCode   = 0;
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
    		            case IDC_USEPROFILE:
                            iCurIndex = SendMessage(hUseProfile, SSBM_GETCURSEL, 0, 0);
                            CallAccProfileList(hFrameWin, AccType, iCurIndex);
                            break;
		                case IDC_AUTOANS:
                            iCurIndex = SendMessage(hAutoAns, SSBM_GETCURSEL, 0, 0);
                            CallAccAutoAnsList(hFrameWin, AccType, iCurIndex);
 			                break;
			         }
                     break;
        
		        case IDC_BUTTON_QUIT:
                    DestroyWindow(hFocus);
			        DestroyWindow(hWnd);
                    break;

	        	case IDC_USEPROFILE:
           	        if(HIWORD( wParam ) == SSBN_CHANGE)
	       	        {
                        iCurIndex = SendMessage(hUseProfile, SSBM_GETCURSEL, 0, 0);
                        SaveAccessoryProfileInfo(AccType, iCurIndex);
					    Sett_SetActiveAccSM(AccType, iCurIndex);
		            }
                    if(HIWORD(wParam) == SSBN_SETFOCUS)
                    {
                        iButtonJust = IDC_USEPROFILE;
                        hFocus = hUseProfile;
                    }
			        break;
            
		        case IDC_AUTOANS:
           	        if(HIWORD( wParam ) == SSBN_CHANGE)
	       	        {
                        iCurIndex = SendMessage(hAutoAns, SSBM_GETCURSEL, 0, 0);
                        SaveAutoAnsState(AccType, iCurIndex);
		            }
                    if(HIWORD(wParam) == SSBN_SETFOCUS)
                    {
                        iButtonJust = IDC_AUTOANS;
                        hFocus = hAutoAns;
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

static BOOL ACC_CreateControl(HWND hWnd, ACCESSORY_TYPE accType, HWND * hUseProfile, HWND * hAutoAns)
{
    int iControlH, iControlW;
    RECT rect;
    int xzero = 0, yzero = 0, i;
    SCENEMODE sm;
    ACCESSORY_SETUP acc_info;
    int ProfileNum;
    GetClientRect(hWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;

	if (accType == ACC_HEADSET)
	{
		printf("\r\n Setting, create the widget, the accessory type is %s...\r\n", "Headset");   
	}
	else if (accType == ACC_CARKIT)
	{
		printf("\r\n Setting, create the widget, the accessory type is %s...\r\n", "car kit");   
	}
	else if (accType == ACC_SPEAKER)
	{
		printf("\r\n Setting, create the widget, the accessory type is %s...\r\n", "speaker phone");   
	}
        
	memset(&acc_info, 0, sizeof(ACCESSORY_SETUP));
    GetAccessorySettingInfo(&acc_info, accType);

    
    if(accType == ACC_HEADSET || accType == ACC_CARKIT)
    {

        * hUseProfile = CreateWindow( "SPINBOXEX", ML("Use profile"), //
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST| WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
            xzero, yzero, iControlW, iControlH, 
            hWnd, (HMENU)IDC_USEPROFILE, NULL, NULL);

        if (* hUseProfile == NULL)
            return FALSE;

        SendMessage(* hUseProfile, SSBM_ADDSTRING, 0, (LPARAM)ML("Keep profile"));
        
        ProfileNum = GetUserProfileNo() + FIXEDPROFILENUMBER;
        
        for(i=0;i<ProfileNum; i++)
        {
			if (i==0)
				SendMessage(* hUseProfile, SSBM_ADDSTRING, 0, (LPARAM)ML("Normal"));
			else if (i==1)
				SendMessage(* hUseProfile, SSBM_ADDSTRING, 0, (LPARAM)ML("Silent"));
			else if (i==2)
				SendMessage(* hUseProfile, SSBM_ADDSTRING, 0, (LPARAM)ML("Noisy"));
			else if (i==3)
				SendMessage(* hUseProfile, SSBM_ADDSTRING, 0, (LPARAM)ML("Discreet"));
			else
			{
				GetSM(&sm, i);
				SendMessage(* hUseProfile, SSBM_ADDSTRING, 0, (LPARAM)(sm.cModeName));
			}
        }        

        * hAutoAns = CreateWindow( "SPINBOXEX", ML("Automatic answer"), //
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
            xzero, yzero + iControlH, iControlW, iControlH, 
            hWnd, (HMENU)IDC_AUTOANS, NULL, NULL);

        if (* hAutoAns == NULL)
            return FALSE;

        SendMessage(*hAutoAns, SSBM_ADDSTRING, 0, (LPARAM)ML("On"));
        SendMessage(*hAutoAns, SSBM_ADDSTRING, 0, (LPARAM)ML("Off"));
	
printf("\r\n Setting, the profile index of accessory is %d...\r\n", acc_info.use_profile + 1);   
        
        SendMessage(* hUseProfile, SSBM_SETCURSEL, acc_info.use_profile + 1, 0);
        SendMessage(*hAutoAns, SSBM_SETCURSEL, acc_info.auto_ans, 0);

        return TRUE;
    }
    if(accType == ACC_SPEAKER)
    {
        * hAutoAns = CreateWindow( "SPINBOXEX", ML("Automatic answer"), //
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST| WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
            xzero, yzero, iControlW, iControlH, 
            hWnd, (HMENU)IDC_AUTOANS, NULL, NULL);

        if (* hAutoAns == NULL)
            return FALSE;
        
        SendMessage(*hAutoAns, SSBM_ADDSTRING, 0, (LPARAM)ML("On"));
        SendMessage(*hAutoAns, SSBM_ADDSTRING, 0, (LPARAM)ML("Off"));

        * hUseProfile = NULL;
        SendMessage(*hAutoAns, SSBM_SETCURSEL, acc_info.auto_ans, 0);
        return TRUE;

    }

    return FALSE;

}

