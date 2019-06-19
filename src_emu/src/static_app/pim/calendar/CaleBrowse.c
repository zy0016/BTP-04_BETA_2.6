/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleBrowse.c
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleBrowse.h"

static HINSTANCE hInstance = NULL;
static HWND hCaleFrameWin = NULL;

//app window
static HWND HwndCaleAPP;                //main window
static HWND HwndCaleJump;

/**********************************************************************
* Function	Date_AppControl
* Purpose   Application Entry(Register wnd class and create main wnd)
* Params	
*			nCode: Entry flag
*			pInstance: App instance(useful when support multi instance)
* Return	
*			TRUE: Success
*			FALSE: Fail
* Remarks	Called by Program manager
**********************************************************************/
DWORD Date_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
    WNDCLASS wc;
    DWORD dwRet = TRUE;

    SYSTEMTIME CurAlarmSys;
    CALE_BROWSECREATEDATA CreateData;

    switch(nCode)
    {
    case APP_INIT:
        {
            hInstance = (HINSTANCE)pInstance;
            mkdir(CALE_FILE_PATH, S_IRWXU);
            CALE_CleanApi();
            CALE_RegisterAlarm(0);

			RTC_RegisterApp("Calendar");
        }
    	break;

    case APP_INITINSTANCE:
    	break;

    case APP_ALARM:
        {
            switch(wParam)
            {
            case ALMN_TIMEOUT:
                {
                    GetLocalTime(&CurAlarmSys);
                    CALE_AlarmOut(&CurAlarmSys, lParam);
					CALE_Print("\r\n[calendar]: ALMN_TIMEOUT, lParam = 0x%08x\r\n", lParam);
                }
                break;

			case ALMN_SYSTIME_CHANGE:
				if(IsWindow(HwndCaleAPP))
				{
					PCALE_BROWSECREATEDATA pCreateData;
					GetLocalTime(&CurAlarmSys);
					pCreateData = GetUserData(HwndCaleAPP);
					if(pCreateData->CurView == CALE_VIEW_WEEK)
					{
						SendMessage(HwndCaleAPP, WM_COMMAND, 
							MAKEWPARAM(IDM_BUTTON_WEEK, CALE_CALC_CHANGED), (LPARAM)(&CurAlarmSys));
					}
					else
					{
						SendMessage(HwndCaleAPP, WM_COMMAND, MAKEWPARAM(0, CALE_CALC_CHANGED), (LPARAM)(&CurAlarmSys));
					}
				}	// NO break here
            case ALMN_TIMECHANGED:
                {
                    CALE_Print("CALE_APP_CALC: ALAN_TIME_CHANGE\r\n");
                    CALE_RegisterAlarm(ALMF_REPLACE);
					CALE_Print("\r\n[calendar]: APP_ALMN_TIMECHANGED, lParam = 0x%08x\r\n", lParam);
                }
                break;

			case ALMN_SETERROR:
				if(IsWindow(hCaleFrameWin))
					PLXTipsWin(hCaleFrameWin, NULL, 0, ML("Set alarm failed!"), ML("Calendar"), 
						Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
				else
					PLXTipsWin(NULL, NULL, 0, ML("Set alarm failed!"), ML("Calendar"), 
						Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
				CALE_Print("\r\n[calendar]: Set alarm error.\r\n");
				break;

            default:
                break;
            }
        }
        break;

    case APP_GETOPTION:
        {
            switch(wParam)
            {
            case AS_ICON:
            	break;

            case AS_STATE:
            	break;

            case AS_HELP:
                break;

            case AS_APPWND:
                dwRet = (DWORD)hCaleFrameWin;
                break;

            default:
                break;
            }
        }
        break;

    case APP_ACTIVE:
        {
            if(IsWindow(hCaleFrameWin))
            {
                ShowWindow(hCaleFrameWin, SW_SHOW);
                ShowOwnedPopups(hCaleFrameWin, SW_SHOW);
                UpdateWindow(hCaleFrameWin);
            }
            else
            {
                RECT rClient;

                wc.style        = 0;
                wc.lpfnWndProc  = CaleAPPWndProc;
                wc.cbClsExtra   = 0;
                wc.cbWndExtra   = sizeof(CALE_BROWSECREATEDATA);;
                wc.hInstance    = NULL;
                wc.hIcon        = NULL;
                wc.hCursor      = NULL;
                wc.hbrBackground= NULL;//GetStockObject(WHITE_BRUSH);
                wc.lpszMenuName = NULL;
                wc.lpszClassName= "CaleAPPWndClass";
                
                if (!RegisterClass(&wc))
                {
                    UnregisterClass("CalcWndClass", NULL);
                    if(!RegisterClass(&wc))
                        return FALSE;
                }

                hCaleFrameWin = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);
                memset(&CreateData, 0x0, sizeof(CALE_BROWSECREATEDATA));
                CreateData.hFrameWnd = hCaleFrameWin;
                CreateData.hMenu = CreateMenu();
                CreateData.hInstance = hInstance;
	
				// for favorite and shortcut
				if(1 == lParam)
					CreateData.DefaultView = CALE_VIEW_DAY;
				else
					CreateData.DefaultView = CALE_GetDefaultView();

                GetClientRect(hCaleFrameWin, &rClient);
                
                HwndCaleAPP = CreateWindow(
                    "CaleAPPWndClass",
                    "",
                    WS_VISIBLE | WS_CHILD, 
                    rClient.left,
                    rClient.top,
                    rClient.right-rClient.left,
                    rClient.bottom-rClient.top,
                    hCaleFrameWin, 
                    NULL,
                    NULL,
                    (PVOID)&CreateData
                    );


				SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_EXIT);
				if(1 != lParam)
				{
					SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_BUTTON_OPEN, 1), (LPARAM)IDP_IME_OPEN) ;
					SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
				}
 
				PDASetMenu(CreateData.hFrameWnd, CreateData.hMenu);
				ShowWindow(CreateData.hFrameWnd, SW_SHOW);
				UpdateWindow(CreateData.hFrameWnd);
            }
        }
        break;

    case APP_INACTIVE:
        {
            ShowOwnedPopups(hCaleFrameWin, SW_HIDE);
            ShowWindow(hCaleFrameWin, SW_HIDE);
        }
        break;

    default:
        break;
    }

    return dwRet;
}

static LRESULT CaleAPPWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    PCALE_BROWSECREATEDATA pCreateData;
    
    LRESULT lResult;
    HDC hdc;
    static HWND hwndFocus;
    static HWND oldFocus;
    static SYSTEMTIME APPStartSys;
    static SYSTEMTIME GotoReSys;
    char DisDate[11];
    BOOL ListRes;
    int iMenuCount;

    HPEN hPen, hOldPen;

    HMENU hAppMenu;
    static HMENU hAppSendMenu;
    static HMENU hAppDelMenu;

    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
            pCreateData = GetUserData(hWnd);
            memcpy(pCreateData, ((LPCREATESTRUCT)lParam)->lpCreateParams, sizeof(CALE_BROWSECREATEDATA));

            pCreateData->DelType = -1;
		//	pCreateData->DefaultView = CALE_GetDefaultView();
            pCreateData->CurView = pCreateData->DefaultView;
            GetLocalTime(&APPStartSys);
		//	CALE_DateSysToChar(&APPStartSys, DisDate);
			CALE_GetFormatedDate(&APPStartSys, DisDate);

#ifdef _NOKIA_CALE_
            //nokia style
            pCreateData->HwndCaleSpin = CreateWindow(
                "STRSPINBOX",
                "",
                WS_CHILD | WS_VISIBLE,
                50, 0, 120, 18,
                hWnd,
                NULL,
                NULL,
                NULL);
            SendMessage(pCreateData->HwndCaleSpin, SSBM_ADDSTRING, 0, (LPARAM)(DisDate));
#endif
            
            CALE_RegisterMonthClass(NULL);
            CALE_RegisterWeekClass(NULL);
            CALE_RegisterDayClass(NULL);
            
            if(pCreateData->DefaultView == CALE_VIEW_MONTH)
            {
                char szDis[15];
                
                memset(szDis, 0x0, 15);
                pCreateData->HwndCaleMonth = CreateWindow(
                    "CALEMONTHCLASS",
                    "",
                    WS_VISIBLE | WS_CHILD,
                    SUBWND_X, SUBWND_Y, SUBWND_WIDTH, SUBWND_HEIGHT,
                    hWnd,
                    (HMENU)ID_CALC_MONTH,
                    NULL, NULL);

                CALE_MonthViewDisplay(&APPStartSys, szDis);
                SetWindowText(pCreateData->hFrameWnd, szDis);

                SetFocus(pCreateData->HwndCaleMonth);
            }

            if(pCreateData->DefaultView == CALE_VIEW_WEEK)
            {
                char szDis[15];

                memset(szDis, 0x0, 15);
                pCreateData->HwndCaleWeek = CreateWindow(
                    "CALEWEEKCLASS",
                    "",
                    WS_VISIBLE | WS_CHILD,
                    SUBWND_X, SUBWND_Y, SUBWND_WIDTH, SUBWND_HEIGHT,
                    hWnd,
                    (HMENU)ID_CALC_WEEK,
                    NULL, NULL);

                CALE_WeekViewDisplay(&APPStartSys, szDis);
                SetWindowText(pCreateData->hFrameWnd, szDis);

                SetFocus(pCreateData->HwndCaleWeek);
            }

            if(pCreateData->DefaultView == CALE_VIEW_DAY)
            {
                char szDis[15];
                
                memset(szDis, 0x0, 15);
                pCreateData->HwndCaleDay = CreateWindow(
                    "CALEDAYCLASS",
                    "",
                    WS_VISIBLE | WS_CHILD,
                    0, 0, 176, 150,
                    hWnd,
                    (HMENU)ID_CALC_DAY,
                    NULL, NULL);

                CALE_DayViewDisplay(&APPStartSys, szDis);
                SetWindowText(pCreateData->hFrameWnd, szDis);

                SetFocus(pCreateData->HwndCaleDay);
            }

            pCreateData->bMenuChanged = TRUE;
            hAppSendMenu = LoadMenuIndirect((PMENUTEMPLATE)&CaleSendMenuTemplate);
            hAppDelMenu = LoadMenuIndirect((PMENUTEMPLATE)&CaleDelMenuTemplate);
            ModifyMenu(hAppSendMenu, IDM_BUTTON_BYSMS, MF_STRING, IDM_BUTTON_BYSMS, IDP_IME_BYSMS);
            ModifyMenu(hAppSendMenu, IDM_BUTTON_BYMMS, MF_STRING, IDM_BUTTON_BYMMS, IDP_IME_BYMMS);
            ModifyMenu(hAppSendMenu, IDM_BUTTON_BYEMAIL, MF_STRING, IDM_BUTTON_BYEMAIL, IDP_IME_BYEMAIL);
            ModifyMenu(hAppSendMenu, IDM_BUTTON_BYBLUE, MF_STRING, IDM_BUTTON_BYBLUE, IDP_IME_BYBLUE);
            ModifyMenu(hAppDelMenu, IDM_BUTTON_DELSELECT, MF_STRING, IDM_BUTTON_DELSELECT, IDP_IME_DELSELECT);
            ModifyMenu(hAppDelMenu, IDM_BUTTON_DELDATE, MF_STRING, IDM_BUTTON_DELDATE, IDP_IME_DELDATE);
            ModifyMenu(hAppDelMenu, IDM_BUTTON_DELALL, MF_STRING, IDM_BUTTON_DELALL, IDP_IME_DELALL);

			CALE_SetSystemIcons(pCreateData->hFrameWnd);
        }
    	break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        {
            pCreateData = GetUserData(hWnd);

			CALE_SetSystemIcons(pCreateData->hFrameWnd);

#ifdef _NOKIA_CALE_
            if(pCreateData->HwndCaleGoto)
            {
                SetFocus(pCreateData->HwndCaleGoto);

                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_IME_BACK);
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_IME_JUMP) ;
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

                SetWindowText(pCreateData->hFrameWnd, IDP_IME_GOTO);

                PDASetMenu(pCreateData->hFrameWnd, pCreateData->hMenu);

                break;
            }
#endif
            if(pCreateData->CurView == CALE_VIEW_MONTH)
            {
                char szDis[15];

                memset(szDis, 0x0, 15);
                SetFocus(pCreateData->HwndCaleMonth);

                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_IME_EXIT);
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_IME_OPEN) ;
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
                
                CALE_MonthViewDisplay(&APPStartSys, szDis);
                SetWindowText(pCreateData->hFrameWnd, szDis);

                PDASetMenu(pCreateData->hFrameWnd, pCreateData->hMenu);
            }
            else if(pCreateData->CurView == CALE_VIEW_WEEK)
            {
                char szDis[15];
                
                memset(szDis, 0x0, 15);
                SetFocus(pCreateData->HwndCaleWeek);
                
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_IME_EXIT);
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_IME_OPEN) ;
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
                
                CALE_WeekViewDisplay(&APPStartSys, szDis);
                SetWindowText(pCreateData->hFrameWnd, szDis);
                
                PDASetMenu(pCreateData->hFrameWnd, pCreateData->hMenu);
            }
            else if(pCreateData->CurView == CALE_VIEW_DAY)
            {
                char szDis[15];
                
                memset(szDis, 0x0, 15);
                SetFocus(pCreateData->HwndCaleDay);
                
                ListRes = SendMessage(pCreateData->HwndCaleDay, CALE_CALC_GETLISTNO, 0, 0);
                if(ListRes)
                {
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_IME_OPEN) ;
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
                }
                else
                {
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"") ;
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
                }
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_IME_BACK);

                CALE_DayViewDisplay(&APPStartSys, szDis);
                SetWindowText(pCreateData->hFrameWnd, szDis);
                
                PDASetMenu(pCreateData->hFrameWnd, pCreateData->hMenu);
            }
        }
    	break;

    case WM_INITMENU:
        {
            pCreateData = GetUserData(hWnd);

            if(!pCreateData->bMenuChanged)
            {
                break;
            }

            pCreateData->bMenuChanged = FALSE;
            hAppMenu = pCreateData->hMenu;
            iMenuCount = GetMenuItemCount(hAppMenu);
            while(iMenuCount-- > 0)
            {
                RemoveMenu(hAppMenu, iMenuCount, MF_BYPOSITION);
            }

#ifdef _NOKIA_CALE_
            if(pCreateData->HwndCaleGoto)
            {
                break;
            }
#endif
            if(pCreateData->CurView == CALE_VIEW_MONTH)
            {
                AppendMenu(hAppMenu, MF_STRING | MF_ENABLED, IDM_BUTTON_OPEN, IDP_IME_OPEN);
                AppendMenu(hAppMenu, MF_STRING | MF_ENABLED, IDM_BUTTON_WEEK, IDP_IME_WEEKVIEW);
                AppendMenu(hAppMenu, MF_STRING | MF_ENABLED, IDM_BUTTON_GOTO, IDP_IME_GOTO);
                AppendMenu(hAppMenu, MF_STRING | MF_ENABLED, IDM_BUTTON_SETTING, IDP_IME_SETTING);
                break;
            }
            if(pCreateData->CurView == CALE_VIEW_WEEK)
            {
                AppendMenu(hAppMenu, MF_STRING | MF_ENABLED, IDM_BUTTON_OPEN, IDP_IME_OPEN);
                AppendMenu(hAppMenu, MF_STRING | MF_ENABLED, IDM_BUTTON_MONTH, IDP_IME_MONTHVIEW);
                AppendMenu(hAppMenu, MF_STRING | MF_ENABLED, IDM_BUTTON_GOTO, IDP_IME_GOTO);
                AppendMenu(hAppMenu, MF_STRING | MF_ENABLED, IDM_BUTTON_SETTING, IDP_IME_SETTING);
                break;
            }
            if(pCreateData->CurView == CALE_VIEW_DAY)
            {
                ListRes = SendMessage(pCreateData->HwndCaleDay, CALE_CALC_GETLISTNO, 0, 0);
                if(ListRes)
                {
                    AppendMenu(hAppMenu, MF_STRING | MF_ENABLED, IDM_BUTTON_OPEN, IDP_IME_OPEN);
                    AppendMenu(hAppMenu, MF_STRING | MF_ENABLED, IDM_BUTTON_EDIT, IDP_IME_EDIT);
                    AppendMenu(hAppMenu, MF_STRING | MF_ENABLED, IDM_BUTTON_GOTO, IDP_IME_GOTO);
                    AppendMenu(hAppMenu, MF_STRING | MF_ENABLED | MF_POPUP, (UINT_PTR)hAppSendMenu, IDP_IME_SEND);
                    AppendMenu(hAppMenu, MF_STRING | MF_ENABLED, IDM_BUTTON_DELETE, IDP_IME_DELETE);
                    AppendMenu(hAppMenu, MF_STRING | MF_ENABLED | MF_POPUP, (UINT_PTR)hAppDelMenu, IDP_IME_DELMANY);
                }
                else
                {
                    break;
                }
            }
        }
        break;

    case WM_CLOSE:
        {
            pCreateData = GetUserData(hWnd);
            
            PostMessage(pCreateData->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)0, 0);
            DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)pCreateData->hInstance);
            DestroyWindow(hWnd);
        }
        break;

    case WM_DESTROY:
        {
            pCreateData = GetUserData(hWnd);

            DestroyMenu(hAppSendMenu);
            DestroyMenu(hAppDelMenu);
            hAppSendMenu = NULL;
            hAppDelMenu = NULL;
            hAppMenu = pCreateData->hMenu;
            DestroyMenu(hAppMenu);
            HwndCaleAPP = NULL;
            CALE_UnRegisterMonthClass();
            CALE_UnRegisterWeekClass();
            CALE_UnRegisterDayClass();

			// when cleaning, the IDs will change.
			// so the alarms have to be reset.
			if(CALE_NeedClean())
			{
				CALE_Print("\r\n[calendar]: >>>> cleaning up, reset all alarms <<<<\r\n");

				CALE_CleanAlarm();
				CALE_CleanApi();
				CALE_RegisterAlarm(ALMF_REPLACE);
			}

            UnregisterClass("CaleAPPWndClass", NULL);
        }
        break;

    case WM_PAINT:
        {
            pCreateData = GetUserData(hWnd);

            hdc = BeginPaint(hWnd, NULL);
            if(pCreateData->CurView != CALE_VIEW_DAY)
            {
                hPen = CreatePen(PS_SOLID, 3, CALE_CAMBRIDGEBLUE);
                hOldPen = SelectObject(hdc, hPen);
                DrawLine(hdc, 1, 2, 173, 2);
				SelectObject(hdc, hOldPen);
				DeleteObject(hPen);
            }
            EndPaint(hWnd, NULL);
        }
        break;

    case WM_KEYDOWN:
        {
            pCreateData = GetUserData(hWnd);

            switch(LOWORD(wParam))
            {
            case VK_RETURN:
                {
                    //char i[2] = "14";
                    //Date_AppControl(CALE_APP_CALC, NULL, ALMN_TIME_OUT, (LPARAM)(&i));
                    if(pCreateData->CurView == CALE_VIEW_DAY)
                    {
                        ListRes = SendMessage(pCreateData->HwndCaleDay, CALE_CALC_GETLISTNO, 0, 0);
                        if(!ListRes)
                        {
                            break;
                        }
                    }
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_OPEN, 0), 0);
                }
                break;

            case VK_F5:
                {
#ifdef _NOKIA_CALE_
                    if(pCreateData->HwndCaleGoto)
                    {
                        break;
                    }
#endif
                    if(pCreateData->CurView == CALE_VIEW_DAY)
                    {
                        ListRes = SendMessage(pCreateData->HwndCaleDay, CALE_CALC_GETLISTNO, 0, 0);
                        if(!ListRes)
                        {
                            PostMessage(pCreateData->HwndCaleDay, WM_KEYDOWN, MAKEWPARAM(VK_RETURN, 0), 0);
                            break;
                        }
                    }
                    lResult = PDADefWindowProc(pCreateData->hFrameWnd, wMsgCmd, wParam, lParam);
                }
                break;

            case VK_F10:
                {
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_QUIT, 0), 0);
                }
                break;

            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;

    case WM_COMMAND:
        {
            pCreateData = GetUserData(hWnd);

            if(wParam == (WPARAM)pCreateData->hMenu)
            {
                SendMessage(hWnd, WM_INITMENU, 0, 0);
                break;
            }
            
            if (HIWORD(wParam) == CALE_CALC_CHANGED)
            {
			//	CALE_DateSysToChar((PSYSTEMTIME)lParam, DisDate);
				CALE_GetFormatedDate((PSYSTEMTIME)lParam, DisDate);

                memset(&APPStartSys, 0x0, sizeof(SYSTEMTIME));
                memcpy(&APPStartSys, (PSYSTEMTIME)lParam, sizeof(SYSTEMTIME));
                APPStartSys.wDayOfWeek = CALE_CalcWkday(&APPStartSys);
#ifdef _NOKIA_CALE_
                //nokia style
                SendMessage(pCreateData->HwndCaleSpin, SSBM_SETTEXT, 0, (LPARAM)DisDate);
#endif
#ifdef _BENEFON_CALE_
                //benefon style
#endif
            }
            switch (LOWORD(wParam))
            {
            case IDM_BUTTON_OPEN:
                {
                    pCreateData->bMenuChanged = TRUE;
#ifdef _NOKIA_CALE_
                    if(pCreateData->HwndCaleGoto)
                    {
                        SendMessage(pCreateData->HwndCaleGoto, TEM_GETTIME, 0, (LPARAM)&GotoReSys);
                        GotoReSys.wDayOfWeek = CALE_CalcWkday(&GotoReSys);

                        SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(0, CALE_CALC_CHANGED), (LPARAM)(&GotoReSys));
                        if((oldFocus != pCreateData->HwndCaleMonth) && (oldFocus != pCreateData->HwndCaleWeek))
                        {
                            oldFocus = pCreateData->HwndCaleDay;
                        }
                        SendMessage(oldFocus, CALE_CALC_SETCUR, 0, (LPARAM)&GotoReSys);
                        
					//	DestroyWindow(pCreateData->HwndCaleGoto);
						PostMessage(pCreateData->HwndCaleGoto, WM_CLOSE, 0, 0);
                        pCreateData->HwndCaleGoto = 0;
                        SetFocus(oldFocus);

                        SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
                        break;
                    }
#endif
                    //current view is month window
                    if(pCreateData->CurView == CALE_VIEW_MONTH)
                    {
                    //    DestroyWindow(pCreateData->HwndCaleMonth);
						PostMessage(pCreateData->HwndCaleMonth, WM_CLOSE, 0, 0);
                        pCreateData->HwndCaleMonth = 0;
                        pCreateData->CurView = CALE_VIEW_DAY;

                        pCreateData->HwndCaleDay = CreateWindow(
                            "CALEDAYCLASS",
                            "",
                            WS_VISIBLE | WS_CHILD,
                            0, 0, 176, 150,
                            hWnd,
                            (HMENU)ID_CALC_DAY,
                            NULL, NULL);
                        
                        SendMessage(pCreateData->HwndCaleDay, CALE_CALC_SETCUR, 0, (LPARAM)&APPStartSys);
                        SetFocus(pCreateData->HwndCaleDay);

                        SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
                    }
                    //current view is week window
                    else if(pCreateData->CurView == CALE_VIEW_WEEK)
                    {
                        //DestroyWindow(pCreateData->HwndCaleWeek);
						PostMessage(pCreateData->HwndCaleWeek, WM_CLOSE, 0, 0);
                        pCreateData->CurView = CALE_VIEW_DAY;
                        pCreateData->HwndCaleWeek = 0;

                        pCreateData->HwndCaleDay = CreateWindow(
                            "CALEDAYCLASS",
                            "",
                            WS_VISIBLE | WS_CHILD,
                            0, 0, 176, 150,
                            hWnd,
                            (HMENU)ID_CALC_DAY,
                            NULL, NULL);
                        
                        SendMessage(pCreateData->HwndCaleDay, CALE_CALC_SETCUR, 0, (LPARAM)&APPStartSys);
                        SetFocus(pCreateData->HwndCaleDay);

                        SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
                    }
                    //current view is day window
                    else if(pCreateData->CurView == CALE_VIEW_DAY)
                    {
                        PostMessage(pCreateData->HwndCaleDay, WM_KEYDOWN, MAKEWPARAM(VK_RETURN, 0), 0);
                    }
                }
                break;

            case IDM_BUTTON_EDIT:
                {
                    PostMessage(pCreateData->HwndCaleDay, WM_KEYDOWN, MAKEWPARAM(VK_RETURN, 1), 0);
                }
                break;

            case IDM_BUTTON_MONTH:
                {
                    pCreateData->bMenuChanged = TRUE;
                    //current view is week window
                    if(pCreateData->CurView == CALE_VIEW_WEEK)
                    {
                        //DestroyWindow(pCreateData->HwndCaleWeek);
						PostMessage(pCreateData->HwndCaleWeek, WM_CLOSE, 0, 0);
                        pCreateData->HwndCaleWeek = 0;
                    }
                    //current view is day window
                    else if(pCreateData->CurView == CALE_VIEW_DAY)
                    {
                        //DestroyWindow(pCreateData->HwndCaleDay);
						PostMessage(pCreateData->HwndCaleDay, WM_CLOSE, 0, 0);
                        pCreateData->HwndCaleDay = 0;
                    }
                    pCreateData->CurView = CALE_VIEW_MONTH;
                    pCreateData->HwndCaleMonth = CreateWindow(
                        "CALEMONTHCLASS",
                        "",
                        WS_VISIBLE | WS_CHILD,
                        SUBWND_X, SUBWND_Y, SUBWND_WIDTH, SUBWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_CALC_MONTH,
                        NULL, NULL);

                    SendMessage(pCreateData->HwndCaleMonth, CALE_CALC_SETCUR, 0, (LPARAM)&APPStartSys);
                    SetFocus(pCreateData->HwndCaleMonth);

                    SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
                }
                break;

            case IDM_BUTTON_WEEK:
                {
                    pCreateData->bMenuChanged = TRUE;
                    //current view is week window
                    if(pCreateData->CurView == CALE_VIEW_MONTH)
                    {
                        //DestroyWindow(pCreateData->HwndCaleMonth);
						PostMessage(pCreateData->HwndCaleMonth, WM_CLOSE, 0, 0);
                        pCreateData->HwndCaleMonth = 0;
                    }
                    //current view is day window
                    else if(pCreateData->CurView == CALE_VIEW_DAY)
                    {
                        //DestroyWindow(pCreateData->HwndCaleDay);
						PostMessage(pCreateData->HwndCaleDay, WM_CLOSE, 0, 0);
                        pCreateData->HwndCaleDay = 0;
                    }
                    pCreateData->CurView = CALE_VIEW_WEEK;
                    pCreateData->HwndCaleWeek = CreateWindow(
                        "CALEWEEKCLASS",
                        "",
                        WS_VISIBLE | WS_CHILD,
                        SUBWND_X, SUBWND_Y, SUBWND_WIDTH, SUBWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_CALC_WEEK,
                        NULL, NULL);

                    SendMessage(pCreateData->HwndCaleWeek, CALE_CALC_SETCUR, 0, (LPARAM)&APPStartSys);
                    SetFocus(pCreateData->HwndCaleWeek);

                    SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
                }
                break;

            case IDM_BUTTON_DELETE:
                {
                    pCreateData->DelType = CALE_DEL_DEL;
                    PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_STRING_DELETE, 
                        Notify_Request, NULL, IDP_IME_YES, IDP_IME_NO, CALE_CONFIRM_MESSAGE);
                }
                break;

            case IDM_BUTTON_DELSELECT:
                {
                    pCreateData->DelType = CALE_DEL_SELECT;
                    CreateDelSelWnd(hWnd);
                }
                break;

            case IDM_BUTTON_DELDATE:
                {
                    pCreateData->DelType = CALE_DEL_NOW;
                    PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_STRING_DELDATE,
                        Notify_Request, NULL, IDP_IME_YES, IDP_IME_NO, CALE_CONFIRM_MESSAGE);
                }
                break;

            case IDM_BUTTON_DELALL:
                {
                    pCreateData->DelType = CALE_DEL_ALL;
                    PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_STRING_DELALL,
                        Notify_Request, NULL, IDP_IME_YES, IDP_IME_NO, CALE_CONFIRM_MESSAGE);
                }
                break;

            case IDM_BUTTON_BYSMS:
                {
                    PostMessage(pCreateData->HwndCaleDay, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_BYSMS, 0), 0);
                }
                break;
                
            case IDM_BUTTON_BYMMS:
                {
                    PostMessage(pCreateData->HwndCaleDay, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_BYMMS, 0), 0);
                }
                break;
                
            case IDM_BUTTON_BYEMAIL:
                {
                    PostMessage(pCreateData->HwndCaleDay, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_BYEMAIL, 0), 0);
                }
                break;

            case IDM_BUTTON_BYBLUE:
                {
                    PostMessage(pCreateData->HwndCaleDay, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_BYBLUE, 0), 0);
                }
                break;

            case IDM_BUTTON_GOTO:
                {
                    pCreateData->bMenuChanged = TRUE;
                    oldFocus = GetFocus();
#ifdef _NOKIA_CALE_
                    pCreateData->HwndCaleGoto = CreateWindow(
                        "TIMEEDIT",
                        IDP_IME_DATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_YMD,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 2, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        NULL,
                        NULL,
                        (PVOID)&APPStartSys
                        );
                    SetFocus(pCreateData->HwndCaleGoto);

                    SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
#endif
#ifdef _BENEFON_CALE_
                    CreateJumpWnd(pCreateData->hFrameWnd, hWnd, &APPStartSys);
#endif
                }
                break;

            case IDM_BUTTON_SETTING:
                {
                    CreateConfigWnd(hWnd);
                }
                break;

            case IDM_BUTTON_QUIT:
                {
                    pCreateData->bMenuChanged = TRUE;
                    //hwndFocus = GetParent(GetFocus());
#ifdef _NOKIA_CALE_
                    if(pCreateData->HwndCaleGoto)
                    {
                        //DestroyWindow(pCreateData->HwndCaleGoto);
						PostMessage(pCreateData->HwndCaleGoto, WM_CLOSE, 0, 0);
                        pCreateData->HwndCaleGoto = 0;
                        SetFocus(oldFocus);

                        SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
                        break;
                    }
#endif
                    
                    if(pCreateData->CurView != pCreateData->DefaultView)
                    {
                        if(pCreateData->CurView == CALE_VIEW_MONTH)
                        {
                            //DestroyWindow(pCreateData->HwndCaleMonth);
							PostMessage(pCreateData->HwndCaleMonth, WM_CLOSE, 0, 0);
                            pCreateData->HwndCaleMonth = 0;
                        }
                        if(pCreateData->CurView == CALE_VIEW_WEEK)
                        {
                            //DestroyWindow(pCreateData->HwndCaleWeek);
							PostMessage(pCreateData->HwndCaleWeek, WM_CLOSE, 0, 0);
                            pCreateData->HwndCaleWeek = 0;
                        }
                        if(pCreateData->CurView == CALE_VIEW_DAY)
                        {
                            //DestroyWindow(pCreateData->HwndCaleDay);
							PostMessage(pCreateData->HwndCaleDay, WM_CLOSE, 0, 0);
                            pCreateData->HwndCaleDay = 0;
                        }

                        pCreateData->CurView = pCreateData->DefaultView;
                        if(pCreateData->DefaultView == CALE_VIEW_MONTH)
                        {
                            pCreateData->HwndCaleMonth = CreateWindow(
                                "CALEMONTHCLASS",
                                "",
                                WS_VISIBLE | WS_CHILD,
                                SUBWND_X, SUBWND_Y, SUBWND_WIDTH, SUBWND_HEIGHT,
                                hWnd,
                                (HMENU)ID_CALC_MONTH,
                                NULL, NULL);

                            SendMessage(pCreateData->HwndCaleMonth, CALE_CALC_SETCUR, 0, (LPARAM)&APPStartSys);
                            SetFocus(pCreateData->HwndCaleMonth);
                        }
                        else if(pCreateData->DefaultView == CALE_VIEW_WEEK)
                        {
                            pCreateData->HwndCaleWeek = CreateWindow(
                                "CALEWEEKCLASS",
                                "",
                                WS_VISIBLE | WS_CHILD,
                                SUBWND_X, SUBWND_Y, SUBWND_WIDTH, SUBWND_HEIGHT,
                                hWnd,
                                (HMENU)ID_CALC_WEEK,
                                NULL, NULL);

                            SendMessage(pCreateData->HwndCaleWeek, CALE_CALC_SETCUR, 0, (LPARAM)&APPStartSys);
                            SetFocus(pCreateData->HwndCaleWeek);
                        }
                        else if(pCreateData->DefaultView == CALE_VIEW_DAY)
                        {
                            pCreateData->HwndCaleDay = CreateWindow(
                                "CALEDAYCLASS",
                                "",
                                WS_VISIBLE | WS_CHILD,
                                0, 0, 176, 150,
                                hWnd,
                                (HMENU)ID_CALC_DAY,
                                NULL, NULL);
                            
                            SendMessage(pCreateData->HwndCaleDay, CALE_CALC_SETCUR, 0, (LPARAM)&APPStartSys);
                            SetFocus(pCreateData->HwndCaleDay);
                        }

                        SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
                    }
                    else
                    {
                        PostMessage(hWnd, WM_CLOSE, 0, 0);
                    }
                }
                break;

            default:
                lResult = PDADefWindowProc(hWnd,wMsgCmd,wParam,lParam);
                break;
            }
        }
        break;

    case CALE_CONFIG_CHANGED:
        {
            pCreateData = GetUserData(hWnd);

            pCreateData->DefaultView = CALE_GetDefaultView();
            if(pCreateData->CurView == CALE_VIEW_MONTH)
            {
                InvalidateRect(pCreateData->HwndCaleMonth, NULL, TRUE);
                UpdateWindow(pCreateData->HwndCaleMonth);
                break;
            }
            if(pCreateData->CurView == CALE_VIEW_WEEK)
            {
                SendMessage(pCreateData->HwndCaleWeek, CALE_CONFIG_CHANGED, NULL, NULL);
                break;
            }
        }
        break;

    case CALE_NEWSUB_RETURN:
        {
            pCreateData = GetUserData(hWnd);

            switch(LOWORD(wParam))
            {
            case 0:
                {
                    SendMessage(pCreateData->HwndCaleDay, CALE_NEW_MEETING, 0, 0);
                }
            	break;

            case 1:
                {
                    SendMessage(pCreateData->HwndCaleDay, CALE_NEW_EVENT, 0, 0);
                }
            	break;

            case 2:
                {
                    SendMessage(pCreateData->HwndCaleDay, CALE_NEW_ANNI, 0, 0);
                }
                break;

            default:
                break;
            }
        }
        break;

    case CALE_CONFIRM_MESSAGE:
        {
            pCreateData = GetUserData(hWnd);

            switch(lParam)
            {
            case 0:
                {
                }
                break;
                
            case 1:
                {
                    if(pCreateData->DelType == CALE_DEL_DEL)
                    {
                        PostMessage(pCreateData->HwndCaleDay, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_DELETE, 0), 0);
                    }
                    else if(pCreateData->DelType == CALE_DEL_SELECT)
                    {
                        PLXTipsWin(NULL, NULL, NULL, IDP_STRING_DELALLSUCC, NULL,
                            Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);
                        SendMessage(pCreateData->HwndCaleDay, CALE_APP_NEWC, 1, NULL);
                    }
                    else if(pCreateData->DelType == CALE_DEL_NOW)
                    {
                        if(CALE_DelSchedule(CALE_DEL_NOW, &APPStartSys))
                        {
                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_DELSSUCC, NULL,
                                Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                            SendMessage(pCreateData->HwndCaleDay, CALE_APP_NEWC, 1, NULL);
                        }
                        else
                        {
                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_DELFAIL, NULL,
                                Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                        }
                    }
                    else if(pCreateData->DelType == CALE_DEL_ALL)
                    {
                        if(CALE_DelSchedule(CALE_DEL_ALL, &APPStartSys))
                        {
                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_DELALLSUCC, NULL,
                                Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                            SendMessage(pCreateData->HwndCaleDay, CALE_APP_NEWC, 1, NULL);
                        }
                        else
                        {
                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_DELFAIL, NULL,
                                Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                        }
                    }
                    pCreateData->DelType = -1;
                }
                break;
                
            default:
                break;
            }
        }
        break;

    default:
        lResult=PDADefWindowProc(hWnd,wMsgCmd,wParam,lParam);
        break;
    }
    return lResult;
}

HWND CALE_GetFrame(void)
{
    return hCaleFrameWin;
}

HWND CALE_GetAppWnd(void)
{
    return HwndCaleAPP;
}

//jump to date window
static DWORD RegisterCaleJumpClass(void)
{
    WNDCLASS wc;
    
    wc.style        = 0;
    wc.lpfnWndProc  = CaleJumpWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = sizeof(CALE_JUMPCREATEDATA);
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CaleJumpWndClass";
    
    if (!RegisterClass(&wc))
    {
        UnregisterClass("CaleJumpWndClass", NULL);
        if(!RegisterClass(&wc))
            return FALSE;
    }
    return TRUE;
}

static void UnRegisterCaleJumpClass(void)
{
    UnregisterClass("CaleJumpWndClass", NULL);
}

static BOOL CreateJumpWnd(HWND hFrameWnd, HWND hwnd, SYSTEMTIME *NowTime)
{
    if(IsWindow(HwndCaleJump))
    {
        ShowWindow(HwndCaleJump, SW_SHOW);
        BringWindowToTop(HwndCaleJump);
        UpdateWindow(HwndCaleJump);
    }
    else
    {
        CALE_JUMPCREATEDATA CreateData;
        RECT rClient;
        
        if(!RegisterCaleJumpClass())
            return FALSE;
        
        memset(&CreateData, 0x0, sizeof(CALE_JUMPCREATEDATA));

        CreateData.hFrameWnd = hFrameWnd;
        CreateData.hParent = hwnd;
        memcpy(&CreateData.sDatetime, NowTime, sizeof(SYSTEMTIME));

        GetClientRect(hFrameWnd, &rClient);

        HwndCaleJump = CreateWindow(
            "CaleJumpWndClass",
            "",
            WS_VISIBLE | WS_CHILD, 
            rClient.left,
            rClient.top,
            rClient.right-rClient.left,
            rClient.bottom-rClient.top,
            hFrameWnd, 
            NULL,
            NULL,
            &CreateData
            );
        
        if(!HwndCaleJump)
        {
            UnRegisterCaleJumpClass();
            return FALSE;
        }
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)IDP_IME_JUMP);
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_CANCEL);
        SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        
        SetWindowText(hFrameWnd, IDP_IME_GOTO);
        ShowWindow(hFrameWnd, SW_SHOW);
        UpdateWindow(hFrameWnd);
    }
    
    return TRUE;
}

static LRESULT CaleJumpWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HDC hdc;
    
    PCALE_JUMPCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);

    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
            memcpy(pCreateData, ((LPCREATESTRUCT)lParam)->lpCreateParams, sizeof(CALE_JUMPCREATEDATA));

            pCreateData->hJumpEdit = CreateWindow(
                "TIMEEDIT",
                IDP_IME_DATE_COLON,
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                TES_TITLE | CS_NOSYSCTRL | TES_DATE_DMY,
                PARTWND_X, PARTWND_Y, PARTWND_WIDTH, PARTWND_HEIGHT,
                hWnd,
                (HMENU)ID_EDIT_STARTDATE,
                NULL,
                (PVOID)&pCreateData->sDatetime
                );

            if(pCreateData->hJumpEdit == NULL)
                return FALSE;

            SetFocus(pCreateData->hJumpEdit);
			CALE_SetSystemIcons(pCreateData->hFrameWnd);
        }
        break;
        
    case WM_INITMENU:
        {
        }
        break;
        
    case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, NULL);
            EndPaint(hWnd, NULL);
        }
        break;
        
    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        {
            SetFocus(pCreateData->hJumpEdit);
            CALE_SetSystemIcons(pCreateData->hFrameWnd);
            
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_IME_CANCEL);
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_IME_JUMP) ;
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			
            SetWindowText(pCreateData->hFrameWnd, IDP_IME_GOTO);
        }
        break;

    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDM_BUTTON_COMPLETE:
                {
				//	if(!SendMessage(pCreateData->hJumpEdit, TEM_GETTIME, 0, (LPARAM)&pCreateData->sDatetime))
                    {
                        PCALE_BROWSECREATEDATA pBrowseData;
						SendMessage(pCreateData->hJumpEdit, TEM_GETTIME, 0, (LPARAM)&pCreateData->sDatetime);

						if((pCreateData->sDatetime).wYear > MAX_YEAR || (pCreateData->sDatetime).wYear < MIN_YEAR)
						{
                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_ENDDTERROR, NULL, 
                                Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);
							PostMessage(hWnd, WM_CLOSE, 0, 0);
							break;
						}
						
                        pBrowseData = GetUserData(pCreateData->hParent);
                        
                        pCreateData->sDatetime.wDayOfWeek = CALE_CalcWkday(&pCreateData->sDatetime);
                        SendMessage(pCreateData->hParent, WM_COMMAND, MAKEWPARAM(0, CALE_CALC_CHANGED), (LPARAM)&pCreateData->sDatetime);
                        if(pBrowseData->CurView == CALE_VIEW_MONTH)
                        {
                            SendMessage(pBrowseData->HwndCaleMonth, CALE_CALC_SETCUR, 0, (LPARAM)&pCreateData->sDatetime);
                        }
                        else if(pBrowseData->CurView == CALE_VIEW_WEEK)
                        {
                            SendMessage(pBrowseData->HwndCaleWeek, CALE_CALC_SETCUR, 0, (LPARAM)&pCreateData->sDatetime);
                        }
                        else if(pBrowseData->CurView == CALE_VIEW_DAY)
                        {
                            SendMessage(pBrowseData->HwndCaleDay, CALE_CALC_SETCUR, 0, (LPARAM)&pCreateData->sDatetime);
                        }
                        
                        PostMessage(hWnd, WM_CLOSE, 0, 0);
                    }
                }
                break;
                
            case IDM_BUTTON_QUIT:
                {
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
                break;
                
            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;
        
    case WM_KEYDOWN:
        {
            switch(LOWORD(wParam))
            {
            case VK_RETURN:
                {
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_COMPLETE, 0), 0);
                }
                break;
                
            case VK_F5:
                {
                }
                break;
                
            case VK_F10:
                {
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_QUIT, 0), 0);
                }
                break;
                
            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;
        
    case WM_CLOSE:
        {
            PostMessage(pCreateData->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            DestroyWindow(hWnd);
        }
        break;
        
    case WM_DESTROY:
        {
            UnRegisterCaleJumpClass();
        }
        break;
        
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

