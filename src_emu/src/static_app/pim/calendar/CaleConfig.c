/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleConfig.c
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleHeader.h"

static HWND HwndCaleConfig;
static HWND HwndSubSnooze;

static LRESULT CaleConfigWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
int CALE_GetDefaultView(void);
int CALE_GetWeekStart(void);
int CALE_GetDayStart(void);
int CALE_GetSnooze(void);
BOOL CALE_GetDefault(int *CurDefView, int *CurWeekStart, int *CurDayStart, int *CurSnooze);
BOOL CALE_SetDefault(int CurDefView, int CurWeekStart, int CurDayStart, int CurSnooze);
BOOL CreateSnoozeWnd(HWND hwnd, int CurSpin, int CurItem);
static LRESULT CaleSnoozeWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

extern TIMEFORMAT GetTimeFormt (void);

static DWORD RegisterCaleConfigClass(void)
{
    WNDCLASS wc;
    
    wc.style        = 0;
    wc.lpfnWndProc  = CaleConfigWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = sizeof(CALE_SETTINGCREATEDATA);
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CaleConfigWndClass";
    
    if(!RegisterClass(&wc))
    {
        UnregisterClass("CaleConfigWndClass", NULL);
        if(!RegisterClass(&wc))
            return FALSE;
    }
    return TRUE;
}

void UnRegisterCaleConfigClass(void)
{
    UnregisterClass("CaleConfigWndClass", NULL);
}

BOOL CreateConfigWnd(HWND hwnd)
{
    CALE_SETTINGCREATEDATA CreateData;
    
    if(IsWindow(HwndCaleConfig))
    {
        ShowWindow(HwndCaleConfig, SW_SHOW);
        BringWindowToTop(HwndCaleConfig);
        UpdateWindow(HwndCaleConfig);
    }
    else
    {
        RECT rClient;

        if(!RegisterCaleConfigClass())
            return FALSE;
        
        memset(&CreateData, 0x0, sizeof(CALE_SETTINGCREATEDATA));

        CreateData.hParent = hwnd;
        CreateData.hFrameWnd = CALE_GetFrame();
        
        GetClientRect(CreateData.hFrameWnd, &rClient);

        HwndCaleConfig = CreateWindow(
            "CaleConfigWndClass",
            "",
            WS_VISIBLE | WS_CHILD | WS_VSCROLL, 
            rClient.left,
            rClient.top,
            rClient.right-rClient.left,
            rClient.bottom-rClient.top,
            CreateData.hFrameWnd,
            NULL,
            NULL,
            (PVOID)&CreateData
            );

        SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_BUTTON_COMPLETE, 0), (LPARAM)(IDP_IME_BACK));
        SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
        SetWindowText(CreateData.hFrameWnd, IDP_IME_CALSET);

        ShowWindow(CreateData.hFrameWnd, SW_SHOW);
        UpdateWindow(CreateData.hFrameWnd);        
    }
    
    return TRUE;
}

static LRESULT CaleConfigWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    PCALE_SETTINGCREATEDATA pCreateData;

    LRESULT lResult = TRUE;
    HDC hdc;

    SCROLLINFO vsi;
	static BOOL bKeyDown =FALSE;

    pCreateData = GetUserData(hWnd);

    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
            char cDayStart[16];
            int i;

			bKeyDown =FALSE;
            memcpy(pCreateData, ((LPCREATESTRUCT)lParam)->lpCreateParams, sizeof(CALE_SETTINGCREATEDATA));

            vsi.cbSize = sizeof(SCROLLINFO);
            vsi.fMask = SIF_ALL;
            vsi.nMin = 0;
            vsi.nMax = 3;
            vsi.nPage = 3;
            vsi.nPos = 0;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

            pCreateData->hDefView = CreateWindow(
                "SPINBOXEX",
                IDP_IME_DEFVIEW,
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
                PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 0, PARTWND_WIDTH, PARTWND_HEIGHT,
                hWnd,
                NULL,
                NULL,
                NULL
                );
            SendMessage(pCreateData->hDefView, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_MONTHVIEW));
            SendMessage(pCreateData->hDefView, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_WEEKVIEW));
#ifdef _NOKIA_CALE_
            SendMessage(pCreateData->hDefView, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_DAYVIEW));
#endif
            
            pCreateData->hWeekStart = CreateWindow(
                "SPINBOXEX",
                IDP_IME_WEEKSTART,
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
                PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 1, PARTWND_WIDTH, PARTWND_HEIGHT,
                hWnd,
                NULL,
                NULL,
                NULL
                );
            SendMessage(pCreateData->hWeekStart, SSBM_ADDSTRING, 0, (LPARAM)(IDP_VIEW_SUNDAY));
            SendMessage(pCreateData->hWeekStart, SSBM_ADDSTRING, 0, (LPARAM)(IDP_VIEW_MONDAY));
            SendMessage(pCreateData->hWeekStart, SSBM_ADDSTRING, 0, (LPARAM)(IDP_VIEW_TUESDAY));
            SendMessage(pCreateData->hWeekStart, SSBM_ADDSTRING, 0, (LPARAM)(IDP_VIEW_WEDNESDAY));
            SendMessage(pCreateData->hWeekStart, SSBM_ADDSTRING, 0, (LPARAM)(IDP_VIEW_THUESDAY));
            SendMessage(pCreateData->hWeekStart, SSBM_ADDSTRING, 0, (LPARAM)(IDP_VIEW_FRIDAY));
            SendMessage(pCreateData->hWeekStart, SSBM_ADDSTRING, 0, (LPARAM)(IDP_VIEW_SATURDAY));
            
            pCreateData->hDayStart = CreateWindow(
                "SPINBOXEX",
                IDP_IME_DAYSTART,
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT, //CS_NOSYSCTRL | 
                PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 2, PARTWND_WIDTH, PARTWND_HEIGHT,
                hWnd,
                NULL,
                NULL,
                NULL
                );

            for(i = 0; i < 24; i ++)
            {
				if(GetTimeFormt() == TF_24)
				{
					sprintf(cDayStart, "%02d:00", i);
				}
				else
				{
					if(i == 0)
						sprintf(cDayStart, "12 AM");
					else if(i < 12)
						sprintf(cDayStart, "%02d AM", i);
					else if(i == 12)
						sprintf(cDayStart, "12 PM");
					else
						sprintf(cDayStart, "%02d PM", (i-12));
				}
				SendMessage(pCreateData->hDayStart, SSBM_ADDSTRING, 0, (LPARAM)(cDayStart));
            }

            pCreateData->hSnooze = CreateWindow(
                "SPINBOXEX",
                IDP_IME_SNOOZE,
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
                PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 3, PARTWND_WIDTH, PARTWND_HEIGHT,
                hWnd,
                NULL,
                NULL,
                NULL
                );
            SendMessage(pCreateData->hSnooze, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_OFF));
            SendMessage(pCreateData->hSnooze, SSBM_ADDSTRING, 0, (LPARAM)(IDP_SNOOZE_5MIN));
            SendMessage(pCreateData->hSnooze, SSBM_ADDSTRING, 0, (LPARAM)(IDP_SNOOZE_10MIN));
            SendMessage(pCreateData->hSnooze, SSBM_ADDSTRING, 0, (LPARAM)(IDP_SNOOZE_30MIN));
            
            SetFocus(pCreateData->hDefView);
            pCreateData->hFocus = pCreateData->hDefView;

            CALE_GetDefault(&pCreateData->DefDefView, &pCreateData->DefWeekStart, 
                &pCreateData->DefDayStart, &pCreateData->DefSnooze);
            SendMessage(pCreateData->hDefView, SSBM_SETCURSEL, (pCreateData->DefDefView - 1), 0);
            SendMessage(pCreateData->hWeekStart, SSBM_SETCURSEL, pCreateData->DefWeekStart, 0);
            SendMessage(pCreateData->hDayStart, SSBM_SETCURSEL, pCreateData->DefDayStart, 0);
            SendMessage(pCreateData->hSnooze, SSBM_SETCURSEL, pCreateData->DefSnooze, 0);
			CALE_SetSystemIcons(pCreateData->hFrameWnd);
        }
        break;

    case PWM_ACTIVATE:
		if(!wParam)
			bKeyDown =FALSE;
		lResult = PDADefWindowProc(hWnd, PWM_ACTIVATE, wParam, lParam);
		break;

    case PWM_SHOWWINDOW:
        {
            SetFocus(pCreateData->hFocus);
			CALE_SetSystemIcons(pCreateData->hFrameWnd);

            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(IDM_BUTTON_COMPLETE, 0), (LPARAM)(IDP_IME_BACK));
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

            SetWindowText(pCreateData->hFrameWnd, IDP_IME_CALSET);
        }
        break;

    case WM_INITMENU:
        {
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
			bKeyDown = FALSE;
            HwndCaleConfig = NULL;
            UnRegisterCaleConfigClass();
        }
        break;
        
    case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, NULL);
            EndPaint(hWnd, NULL);
        }
        break;

    case WM_KEYDOWN:
        {
            switch(LOWORD(wParam)) 
            {
            case VK_RETURN:
                {
                }
                break;

            case VK_F5:
                {
                    if(pCreateData->hFocus == pCreateData->hDefView)
                    {
                        pCreateData->CurSpin = 0;
                        pCreateData->CurItem = SendMessage(pCreateData->hDefView, SSBM_GETCURSEL, NULL, NULL);
                    }
                    if(pCreateData->hFocus == pCreateData->hWeekStart)
                    {
                        pCreateData->CurSpin = 1;
                        pCreateData->CurItem = SendMessage(pCreateData->hWeekStart, SSBM_GETCURSEL, NULL, NULL);
                        pCreateData->CurItem = (pCreateData->CurItem == 0) ? 6 : (pCreateData->CurItem - 1);
                    }
                    if(pCreateData->hFocus == pCreateData->hDayStart)
                    {
                        pCreateData->CurSpin = 2;
                        pCreateData->CurItem = SendMessage(pCreateData->hDayStart, SSBM_GETCURSEL, NULL, NULL);
                    }
                    if(pCreateData->hFocus == pCreateData->hSnooze)
                    {
                        pCreateData->CurSpin = 3;
                        pCreateData->CurItem = SendMessage(pCreateData->hSnooze, SSBM_GETCURSEL, NULL, NULL);
                    }
                    CreateSnoozeWnd(hWnd, pCreateData->CurSpin, pCreateData->CurItem);
                }
                break;
                
            case VK_F10:
                {
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_COMPLETE, 0), NULL);
                }
                break;

            case VK_DOWN:
                {
                    HWND CurHwnd;

                    memset(&vsi, 0, sizeof(SCROLLINFO));
                    vsi.cbSize = sizeof(vsi);
                    vsi.fMask  = SIF_ALL;
                    GetScrollInfo(hWnd, SB_VERT, &vsi);
					
					if(pCreateData->hFocus == pCreateData->hSnooze)//vsi.nPos == vsi.nMax)
					{
						ScrollWindow(hWnd, 0, PARTWND_HEIGHT, NULL, NULL);
						vsi.nPos = 0;
					//	break;
					}
                    else if(pCreateData->hFocus == pCreateData->hDefView)
                    {
                        ScrollWindow(hWnd, 0, 0, NULL, NULL);
                    }
                    else if(GetNextDlgTabItem(hWnd, pCreateData->hFocus, FALSE) == pCreateData->hSnooze)
                    {
                        ScrollWindow(hWnd, 0, 0, NULL, NULL);
                    }
                    else
                    {
                        ScrollWindow(hWnd, 0, -PARTWND_HEIGHT, NULL, NULL);
                        vsi.nPos ++;
                    }
                    
                    CurHwnd = pCreateData->hFocus;
                    pCreateData->hFocus = GetNextDlgTabItem(hWnd, CurHwnd, FALSE);
                    SetFocus(pCreateData->hFocus);
                    UpdateWindow(hWnd);
                    
                    vsi.fMask  = SIF_POS;
                    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

					if(!bKeyDown)
					{
						bKeyDown = TRUE;
						SetTimer(hWnd, TIMERID_VK_DOWN, TIMER_KEY_LONG, NULL);
					}
                }
                break;

            case VK_UP:
                {
                    HWND CurHwnd;
                    
                    memset(&vsi, 0, sizeof(SCROLLINFO));
                    vsi.cbSize = sizeof(vsi);
                    vsi.fMask  = SIF_ALL;
                    GetScrollInfo(hWnd, SB_VERT, &vsi);
                    
					if(pCreateData->hFocus == pCreateData->hDefView)//vsi.nPos == vsi.nMin)
					{
						ScrollWindow(hWnd, 0, -PARTWND_HEIGHT, NULL, NULL);
						vsi.nPos = 1;
					//	break;
					}
                    else if(pCreateData->hFocus == pCreateData->hSnooze)
                    {
                        ScrollWindow(hWnd, 0, 0, NULL, NULL);
                    }
                    else if(GetNextDlgTabItem(hWnd, pCreateData->hFocus, TRUE) == pCreateData->hDefView)
                    {
                        ScrollWindow(hWnd, 0, 0, NULL, NULL);
                    }
                    else
                    {
                        ScrollWindow(hWnd, 0, PARTWND_HEIGHT, NULL, NULL);
                        vsi.nPos --;
                    }
                    
                    CurHwnd = pCreateData->hFocus;
                    pCreateData->hFocus = GetNextDlgTabItem(hWnd, CurHwnd, TRUE);
                    SetFocus(pCreateData->hFocus);

                    UpdateWindow(hWnd);
                    vsi.fMask  = SIF_POS;
                    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
					
					if(!bKeyDown)
					{
						bKeyDown = TRUE;
						SetTimer(hWnd, TIMERID_VK_UP, TIMER_KEY_LONG, NULL);
					}
                }
                break;
                
            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;

	case WM_KEYUP:
		bKeyDown =FALSE;
		KillTimer(hWnd, TIMERID_VK_UP);
		KillTimer(hWnd, TIMERID_VK_DOWN);
		break;

	case WM_TIMER:
		KillTimer(hWnd, wParam);
		if(TIMERID_VK_UP == wParam)
		{
			PostMessage(hWnd, WM_KEYDOWN, VK_UP, 0);
			SetTimer(hWnd, TIMERID_VK_UP, TIMER_KEY_REPEAT, NULL);
		}
		else if(TIMERID_VK_DOWN == wParam)
		{
			PostMessage(hWnd, WM_KEYDOWN, VK_DOWN, 0);
			SetTimer(hWnd, TIMERID_VK_DOWN, TIMER_KEY_REPEAT, NULL);
		}
		break;

    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDM_BUTTON_COMPLETE:
                {
                    int nDefView;
                    int nWeekStart;
                    int nDayStart;
                    int nSnooze;
                    
                    nDefView = SendMessage(pCreateData->hDefView, SSBM_GETCURSEL, NULL, NULL);
                    nDefView ++;
                    nWeekStart = SendMessage(pCreateData->hWeekStart, SSBM_GETCURSEL, NULL, NULL);
                    nDayStart = SendMessage(pCreateData->hDayStart, SSBM_GETCURSEL, NULL, NULL);
                    nSnooze = SendMessage(pCreateData->hSnooze, SSBM_GETCURSEL, NULL, NULL);
                    if((pCreateData->DefDefView != nDefView) || (pCreateData->DefWeekStart != nWeekStart) 
                        || (pCreateData->DefDayStart != nDayStart) || (pCreateData->DefSnooze != nSnooze))
                    {
                        CALE_SetDefault(nDefView, nWeekStart, nDayStart, nSnooze);
                        SendMessage(pCreateData->hParent, CALE_CONFIG_CHANGED, 0, 0);
                    }
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
                break;

            case IDM_BUTTON_ALTER:
                {
                    if(HIWORD(wParam) == 0)
                    {
                        SendMessage(pCreateData->hDefView, SSBM_SETCURSEL, LOWORD(lParam), NULL);
                    }
                    if(HIWORD(wParam) == 1)
                    {
                        pCreateData->CurItem = (LOWORD(lParam) == 6) ? 0 : (LOWORD(lParam) + 1);
                        SendMessage(pCreateData->hWeekStart, SSBM_SETCURSEL, pCreateData->CurItem, NULL);
                    }
                    if(HIWORD(wParam) == 2)
                    {
                        SendMessage(pCreateData->hDayStart, SSBM_SETCURSEL, LOWORD(lParam), NULL);
                    }
                    if(HIWORD(wParam) == 3)
                    {
                        SendMessage(pCreateData->hSnooze, SSBM_SETCURSEL, LOWORD(lParam), NULL);
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
            }
        }
        break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;        
    }

    return lResult;
}

int CALE_GetDefaultView(void)
{
    int nOpen;
    int CurDefView, CurWeekStart, CurDayStart, CurSnooze;
    char szOutPath[100];
    
    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);

    nOpen = open(CALE_CONFIG_FILE, O_RDONLY,  0);
    if(nOpen == -1)
    {
        nOpen = open(CALE_CONFIG_FILE, O_RDWR | O_CREAT, S_IRWXU);
        if(nOpen == -1)
        {
            chdir(szOutPath);
            return -1;
        }
        CurDefView = CALE_VIEW_MONTH;
        CurWeekStart = START_MONDAY;
        CurDayStart = 8;
        CurSnooze = CALE_SNOOZE_5MIN;
        
        write(nOpen, (int *)&CurDefView, sizeof(int));
        write(nOpen, (int *)&CurWeekStart, sizeof(int));
        write(nOpen, (int *)&CurDayStart, sizeof(int));
        write(nOpen, (int *)&CurSnooze, sizeof(int));
        close(nOpen);
    }
    else
    {
        lseek(nOpen, 0, SEEK_SET);
        read(nOpen, (int *)&CurDefView, sizeof(int));
        close(nOpen);
    }
    chdir(szOutPath);
    
    return CurDefView;
}

int CALE_GetWeekStart(void)
{
    int nOpen;
    int CurDefView, CurWeekStart, CurDayStart, CurSnooze;
    char szOutPath[100];
    
    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);

    nOpen = open(CALE_CONFIG_FILE, O_RDONLY,  0);
    if(nOpen == -1)
    {
        nOpen = open(CALE_CONFIG_FILE, O_RDWR | O_CREAT, S_IRWXU);
        if(nOpen == -1)
        {
            chdir(szOutPath);
            return -1;
        }
        CurDefView = CALE_VIEW_MONTH;
        CurWeekStart = START_MONDAY;
        CurDayStart = 8;
        CurSnooze = CALE_SNOOZE_5MIN;
        
        write(nOpen, (int *)&CurDefView, sizeof(int));
        write(nOpen, (int *)&CurWeekStart, sizeof(int));
        write(nOpen, (int *)&CurDayStart, sizeof(int));
        write(nOpen, (int *)&CurSnooze, sizeof(int));
        close(nOpen);
    }
    else
    {
        lseek(nOpen, (sizeof(int) * 1), SEEK_SET);
        read(nOpen, (int *)&CurWeekStart, sizeof(int));
        close(nOpen);
    }
    chdir(szOutPath);
    
    return CurWeekStart;
}

int CALE_GetDayStart(void)
{
    int nOpen;
    int CurDefView, CurWeekStart, CurDayStart, CurSnooze;
    char szOutPath[100];
    
    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);
    
    nOpen = open(CALE_CONFIG_FILE, O_RDONLY,  0);
    if(nOpen == -1)
    {
        nOpen = open(CALE_CONFIG_FILE, O_RDWR | O_CREAT, S_IRWXU);
        if(nOpen == -1)
        {
            chdir(szOutPath);
            return -1;
        }
        CurDefView = CALE_VIEW_MONTH;
        CurWeekStart = START_MONDAY;
        CurDayStart = 8;
        CurSnooze = CALE_SNOOZE_5MIN;
        
        write(nOpen, (int *)&CurDefView, sizeof(int));
        write(nOpen, (int *)&CurWeekStart, sizeof(int));
        write(nOpen, (int *)&CurDayStart, sizeof(int));
        write(nOpen, (int *)&CurSnooze, sizeof(int));
        close(nOpen);
    }
    else
    {
        lseek(nOpen, (sizeof(int) * 2), SEEK_SET);
        read(nOpen, (int *)&CurDayStart, sizeof(int));
        close(nOpen);
    }
    chdir(szOutPath);
    
    return CurDayStart;
}

int CALE_GetSnooze(void)
{
    int nOpen;
    int CurDefView, CurWeekStart, CurDayStart, CurSnooze;
    char szOutPath[100];
    
    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);
    
    nOpen = open(CALE_CONFIG_FILE, O_RDONLY,  0);
    if(nOpen == -1)
    {
        nOpen = open(CALE_CONFIG_FILE, O_RDWR | O_CREAT, S_IRWXU);
        if(nOpen == -1)
        {
            chdir(szOutPath);
            return -1;
        }
        CurDefView = CALE_VIEW_MONTH;
        CurWeekStart = START_MONDAY;
        CurDayStart = 8;
        CurSnooze = CALE_SNOOZE_5MIN;
        
        write(nOpen, (int *)&CurDefView, sizeof(int));
        write(nOpen, (int *)&CurWeekStart, sizeof(int));
        write(nOpen, (int *)&CurDayStart, sizeof(int));
        write(nOpen, (int *)&CurSnooze, sizeof(int));
        close(nOpen);
    }
    else
    {
        lseek(nOpen, (sizeof(int) * 3), SEEK_SET);
        read(nOpen, (int *)&CurSnooze, sizeof(int));
        close(nOpen);
    }
    chdir(szOutPath);
    
    return CurSnooze;
}

BOOL CALE_GetDefault(int *CurDefView, int *CurWeekStart, int *CurDayStart, int *CurSnooze)
{
    int nOpen;
    char szOutPath[100];
    
    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);
    
    nOpen = open(CALE_CONFIG_FILE, O_RDONLY,  0);
    if(nOpen == -1)
    {
        nOpen = open(CALE_CONFIG_FILE, O_RDWR | O_CREAT, S_IRWXU);
        if(nOpen == -1)
        {
            chdir(szOutPath);
            return FALSE;
        }
        *CurDefView = CALE_VIEW_MONTH;
        *CurWeekStart = START_MONDAY;
        *CurDayStart = 8;
        *CurSnooze = CALE_SNOOZE_5MIN;
        write(nOpen, (int *)CurDefView, sizeof(int));
        write(nOpen, (int *)CurWeekStart, sizeof(int));
        write(nOpen, (int *)CurDayStart, sizeof(int));
        write(nOpen, (int *)CurSnooze, sizeof(int));
        close(nOpen);
    }
    else
    {
        lseek(nOpen, 0, SEEK_SET);
        read(nOpen, (int *)CurDefView, sizeof(int));
        read(nOpen, (int *)CurWeekStart, sizeof(int));
        read(nOpen, (int *)CurDayStart, sizeof(int));
        read(nOpen, (int *)CurSnooze, sizeof(int));
        close(nOpen);
    }
    chdir(szOutPath);

    return TRUE;
}

BOOL CALE_SetDefault(int CurDefView, int CurWeekStart, int CurDayStart, int CurSnooze)
{
    int nOpen;
    char szOutPath[100];
    
    memset(szOutPath, 0x0, 100);
    getcwd(szOutPath, 100);
    chdir(CALE_FILE_PATH);
    
    nOpen = open(CALE_CONFIG_FILE, O_RDWR, 0);
    if(nOpen == -1)
    {
        chdir(szOutPath);
        return FALSE;
    }

    write(nOpen, (int *)&CurDefView, sizeof(int));
    write(nOpen, (int *)&CurWeekStart, sizeof(int));
    write(nOpen, (int *)&CurDayStart, sizeof(int));
    write(nOpen, (int *)&CurSnooze, sizeof(int));
    close(nOpen);
    chdir(szOutPath);

    return TRUE;
}

static DWORD RegisterCaleSnoozeClass(void)
{
    WNDCLASS wc;
    
    wc.style        = 0;
    wc.lpfnWndProc  = CaleSnoozeWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = sizeof(CALE_SNOOZECREATEDATA);
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CaleSnoozeWndClass";
    
    if (!RegisterClass(&wc))
    {
        UnregisterClass("CaleSnoozeWndClass", NULL);
        if(!RegisterClass(&wc))
            return FALSE;
    }
    return TRUE;
}

void UnRegisterCaleSnoozeClass(void)
{
    UnregisterClass("CaleSnoozeWndClass", NULL);
    return;
}


BOOL CreateSnoozeWnd(HWND hwnd, int CurSpin, int CurItem)
{
    if(IsWindow(HwndSubSnooze))
    {
        ShowWindow(HwndSubSnooze, SW_SHOW);
        BringWindowToTop(HwndSubSnooze);
        UpdateWindow(HwndSubSnooze);
    }
    else
    {
        CALE_SNOOZECREATEDATA CreateData;
        RECT rClient;
        
        if(!RegisterCaleSnoozeClass())
            return FALSE;
        
        memset(&CreateData, 0x0, sizeof(CALE_SNOOZECREATEDATA));
        CreateData.hFrameWnd = CALE_GetFrame();
        CreateData.hParent = hwnd;
        CreateData.CurSpin = CurSpin;
        CreateData.CurItem = CurItem;

        GetClientRect(CreateData.hFrameWnd, &rClient);
        
        HwndSubSnooze = CreateWindow(
            "CaleSnoozeWndClass",
            "",
            WS_VISIBLE | WS_CHILD, 
            rClient.left,
            rClient.top,
            rClient.right-rClient.left,
            rClient.bottom-rClient.top,
            CreateData.hFrameWnd,
            NULL,
            NULL,
            (PVOID)&CreateData
            );
        
        SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)(IDP_IME_BACK));
        SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

        switch(CreateData.CurSpin)
        {
        case 0:
            {
                SetWindowText(CreateData.hFrameWnd, IDP_IME_DEFVIEW);
            }
        	break;

        case 1:
            {
                SetWindowText(CreateData.hFrameWnd, IDP_IME_WEEKSTART);
            }
        	break;

        case 2:
            {
                SetWindowText(CreateData.hFrameWnd, IDP_IME_DAYSTART);
            }
            break;

        case 3:
            {
                SetWindowText(CreateData.hFrameWnd, IDP_IME_SNOOZE);
            }
            break;

        default:
            break;
        }
        
        ShowWindow(CreateData.hFrameWnd, SW_SHOW);
        UpdateWindow(CreateData.hFrameWnd); 
    }
    
    return TRUE;
}

static LRESULT CaleSnoozeWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    PCALE_SNOOZECREATEDATA pCreateData;
    
    LRESULT lResult = TRUE;
    HDC hdc;

    int i, j;

    pCreateData = GetUserData(hWnd);

    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
            RECT rcClient;

            memcpy(pCreateData, ((LPCREATESTRUCT)lParam)->lpCreateParams, sizeof(CALE_SNOOZECREATEDATA));

            pCreateData->hSelect = LoadImage(NULL, CALE_ICON_SELECT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            pCreateData->hUnSelect = LoadImage(NULL, CALE_ICON_UNSELECT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);

            GetClientRect(pCreateData->hFrameWnd, &rcClient);
            
            pCreateData->HwndList = CreateWindow(
                "LISTBOX",
                "",
                WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY | LBS_BITMAP,
                rcClient.left,
                rcClient.top,
                rcClient.right-rcClient.left,
                rcClient.bottom-rcClient.top,
                hWnd,
                NULL, NULL, NULL);

            if(pCreateData->CurSpin == 0)
            {
                SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_IME_MONTHVIEW));
                SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_IME_WEEKVIEW));
                j = 2;
            }
            if(pCreateData->CurSpin == 1)
            {
                SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_VIEW_MONDAY));
                SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_VIEW_TUESDAY));
                SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_VIEW_WEDNESDAY));
                SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_VIEW_THUESDAY));
                SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_VIEW_FRIDAY));
                SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_VIEW_SATURDAY));
                SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_VIEW_SUNDAY));
                j = 7;
            }
            if(pCreateData->CurSpin == 2)
            {
                int timeformat;
                char buff[16];

                timeformat = GetTimeFormt();
                //draw the No.week and date
                for(i = 0; i < 24; i ++)
                {
                    if(timeformat == TF_12)
                    {
						if(i == 0)
							sprintf(buff, "12 AM");
						else if(i < 12)
							sprintf(buff, "%02d AM", i);
						else if(i == 12)
							sprintf(buff, "12 PM");
						else
							sprintf(buff, "%02d PM", (i-12));
/*
                        if(i < 10)
                        {
                            sprintf(buff, "%0dAM", i);
                        }
                        else if(i < 12)
                        {
                            sprintf(buff, "%dAM", i);
                        }
                        else if(i < 22)
                        {
                            sprintf(buff, "%0dPM", (i - 12));
                        }
                        else
                        {
                            sprintf(buff, "%dAM", (i - 12));
                        }
 */
                   }
                    else
                    {
//                        if(i < 10)
//                        {
//                            sprintf(buff, "0%d:00", i);
//                        }
//                        else
                        {
                            sprintf(buff, "%02d:00", i);
                        }
                    }
                    SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)&buff);
                }
                j = 24;
            }
            if(pCreateData->CurSpin == 3)
            {
                SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_IME_OFF));
                SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_SNOOZE_5MIN));
                SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_SNOOZE_10MIN));
                SendMessage(pCreateData->HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_SNOOZE_30MIN));
                j = 4;
            }
            
            for(i=0; i<j; i++)
            {
                if(pCreateData->CurItem == i)
                {
                    SendMessage(pCreateData->HwndList, LB_SETIMAGE, 
                        MAKEWPARAM(IMAGE_BITMAP, pCreateData->CurItem), (LPARAM)pCreateData->hSelect);
                }
                else
                {
                    SendMessage(pCreateData->HwndList, LB_SETIMAGE, 
                        MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)pCreateData->hUnSelect);
                }
            }

            SendMessage(pCreateData->HwndList, LB_SETCURSEL, pCreateData->CurItem, 0);

            SetFocus(pCreateData->HwndList);
            pCreateData->hFocus = pCreateData->HwndList;
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
            SetFocus(pCreateData->hFocus);
            CALE_SetSystemIcons(pCreateData->hFrameWnd);

            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)(IDP_IME_BACK));
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
            
            switch(pCreateData->CurSpin)
            {
            case 0:
                {
                    SetWindowText(pCreateData->hFrameWnd, IDP_IME_DEFVIEW);
                }
                break;
                
            case 1:
                {
                    SetWindowText(pCreateData->hFrameWnd, IDP_IME_WEEKSTART);
                }
                break;
                
            case 2:
                {
                    SetWindowText(pCreateData->hFrameWnd, IDP_IME_DAYSTART);
                }
                break;
                
            case 3:
                {
                    SetWindowText(pCreateData->hFrameWnd, IDP_IME_SNOOZE);
                }
                break;
                
            default:
                break;
            }
        }
        break;
        
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {                
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
                }
                break;
                
            case VK_F5:
                {
                    int temp;
                    
                    temp = SendMessage(pCreateData->HwndList, LB_GETCURSEL, 0, 0);
                    if((temp >= 0) && (pCreateData->CurItem != temp))
                    {
                        pCreateData->CurItem = temp;
                        SendMessage(pCreateData->hParent, WM_COMMAND, 
                            MAKEWPARAM(IDM_BUTTON_ALTER, pCreateData->CurSpin), pCreateData->CurItem);
                    }
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
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
            if(pCreateData->hSelect)
            {
                DeleteObject(pCreateData->hSelect);
                pCreateData->hSelect = NULL;
            }
            if(pCreateData->hUnSelect)
            {
                DeleteObject(pCreateData->hUnSelect);
                pCreateData->hUnSelect = NULL;
            }
            HwndSubSnooze = NULL;
            UnRegisterCaleSnoozeClass();
        }
        break;
        
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
