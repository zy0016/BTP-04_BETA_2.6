/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : progman
 *
 * Purpose  : implement charge
 *            
\**************************************************************************/
#include <sys/task.h>
#include "progman.h"
#include "stdio.h"
#include "string.h"
#include <project/plx_pdaex.h>
#include "me_wnd.h"
#include <project/compatable.h>

#define ICON_WIDTH       13
#define ICON_HEIGHT      13
#define IDC_POWERON      200
#define CHARGEONLYWINDOWCLASS	"ChargeOnly_WindowClass"
#define BATTERYIMAGEPREFIX	"/rom/progman/battery/battery_"
#define WM_CHARGING_ONLY_BATTERY	(WM_USER + 1000)

static LRESULT ChargeOnly_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                                LPARAM lParam);

extern  BOOL    MobileInit2( void );
extern  void    f_CheckPowerOff(void);
extern  int		PW_GetCharge(void *ChargeCtl);
extern	BOOL	PlayPowerOff(BOOL bPlay);
extern	BOOL	Clock_IsAlarming(void);

void Elect_PowerOn(BOOL bTip)
{
    if ( bTip )
    {
        PLXTipsWin( NULL, NULL, NULL,ML("Battery voltage low"),ML("Battery"),Notify_Info,NULL ,NULL, WAITTIMEOUT );
      //  Call_PlayMusic(RING_WARNING, 3);
    }
    else
    {
        PLXTipsWin (NULL, NULL, NULL, ML("Charging"), ML("Battery"),Notify_Info, NULL ,NULL, WAITTIMEOUT );
    }
}

static HGIFANIMATE hElectingGIF;
static HWND hElectPowerOffWnd;

/*
 *	Charge-only
 */
void ChargeOnlyState(void)
{

    WNDCLASS wc;

	printf("\r\n enter charge only!\r\n");
    
    wc.style         = 0;    
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;    
    wc.lpfnWndProc   = ChargeOnly_WndProc;      //poweroff and charging
    wc.lpszClassName  = CHARGEONLYWINDOWCLASS;
    
    RegisterClass(&wc);
    
    hElectPowerOffWnd = CreateWindow(CHARGEONLYWINDOWCLASS,
        NULL,
        WS_VISIBLE|PWS_STATICBAR|WS_BORDER|WS_CAPTION,
        PLX_WIN_TOP_X,	
        PLX_WIN_TOP_Y,
        PLX_WIN_WIDTH,//DESK_WIDTH,	
        PLX_WIN_HIGH,//DESK_HEIGHT,
        NULL, NULL,	NULL, NULL);
}

static LRESULT ChargeOnly_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                                LPARAM lParam)
{

    static	BOOL	bPowerOn;
	LRESULT lResult = TRUE;
    HDC     hdc;    
	ChargeCtl_t  bChargeInfo;
    
    switch(wMsgCmd)
    {	
    case WM_CREATE:
        
        SetTimer(hWnd, 1, 5000, NULL); 
		SetTimer(hWnd, 2, 1000, NULL);
        
        DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 30);
		DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_KEYLIGHT), 5);
        DlmNotify (PMM_NEWS_ENABLE,PMF_KEYLIGHT|PMF_MAIN_BKLIGHT|
            PMF_AUTO_SHUT_MAIN_LIGHT|PMF_AUTO_SHUT_KEYLIGHT);

        break;

	case WM_CHARGING_ONLY_BATTERY:
		if (ME_GetResult(&bChargeInfo, sizeof(ChargeCtl_t)) < 0)
		{
			printf("\r\nget battery result error.\r\n");
			break;
		}
		if ( CHG_OFF==bChargeInfo.ChargeStatus )
		{
			printf("\r\nCharger detached, we should turn off the phone.\r\n");
			if ( Clock_IsAlarming() )
				// should not power off when alarm is on
				DestroyWindow(hWnd);
			else
				PlayPowerOff(FALSE);
		}
		if ( CHG_HIGHTMP==bChargeInfo.ChargeStatus )
		{
			printf("\r\nThe battery's temparature is too high!!!\r\n");
			Sleep(10*1000);
			PlayPowerOff(FALSE);
		}
		if ( CHG_FINISH==bChargeInfo.ChargeStatus )
		{
			// battery is full, stop flash the Icon
			KillTimer(hWnd, 2);
			SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
			SendMessage(hWnd, PWM_SETSIGNAL,
				MAKEWPARAM(IMAGE_ICON, POWERICON),(LPARAM)"/rom/progman/battery/battery_full.ico");
		}
		else
				SetTimer(hWnd, 2, 1000, NULL);	// charging is not finished flash the Icon
		break;
        
    case WM_TIMER:
		if(wParam == 1)
		{
			// inquery battery state
			if (ME_GetChargeStatus(hWnd, WM_CHARGING_ONLY_BATTERY) < 0)
            {
                printf("\r\nbattery inquery error\r\n");
                break;
            }
		}
		else if(wParam == 2)
		{
			char       IconName[64] = "";
            static int i = 0;
            
            strcpy(IconName, BATTERYIMAGEPREFIX);
			switch(i) {
			case 0:
				strcat(IconName,"0%.ico");
				break;
			case 25:
				strcat(IconName,"25%.ico");
				break;
			case 50:
				strcat(IconName,"50%.ico");
				break;
			case 75:
				strcat(IconName,"75%.ico");
				break;
			case 100:
				strcat(IconName,"full.ico");
				break;
			default:
				strcat(IconName,"0%.ico");
			}
			
			SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
			SendMessage(hWnd, PWM_SETSIGNAL,MAKEWPARAM(IMAGE_ICON, POWERICON),(LPARAM)IconName);
		   
            if ( i >= 100 )
                i =0;
			else
				i = i+25;
            break;
		}
		else if(wParam==3)
		{
			KillTimer(hWnd,3);
			DHI_Vibration(FALSE);
			DestroyWindow(hWnd);
			DlmNotify(PS_ALARMMODE,TRUE);	// Notify power on
			break;
		}

        break;
   
    case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, NULL);
			EndPaint(hWnd, NULL);
		}
        break;
	case WM_KEYUP:
		switch(LOWORD(wParam))
		{
		case VK_F6:
			KillTimer(hWnd,3);
			DHI_Vibration(FALSE);
			break;
		default:
			break;
		}
		
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F6:
			SetTimer(hWnd,3,1000,NULL);
			DHI_Vibration(TRUE);
			break;
		case VK_CHARGER_OUT:
			// charger detached , we should close this window or turn off phone
			if ( Clock_IsAlarming() )
				// should not power off when alarm is on
				DestroyWindow(hWnd);
			else
				PlayPowerOff(FALSE);
			break;
		default:
			break;
		}
		break;
    
	case WM_DESTROY:
		{
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);
			KillTimer(hWnd, 3);
			hElectPowerOffWnd = NULL;
			UnregisterClass(CHARGEONLYWINDOWCLASS, NULL);
		}
     break;

    default:
        return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }
    return lResult;
}
HWND GetElectPowerOffWnd(void)
{
	return hElectPowerOffWnd;
}
