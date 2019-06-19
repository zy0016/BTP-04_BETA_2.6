/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : progman
 *
 * Purpose  : implement power off 
 *            
\**************************************************************************/

#include	<string.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/mount.h>
#include	<fcntl.h>
#include	<unistd.h> 
#include	<hopen/ipmc.h>
#include	"hpimage.h"
#include	"plx_pdaex.h"
#include	"smsdptch.h"
#include	"pmalarm.h"
#include	"progman.h"
#include	"compatable.h"

static  HGIFANIMATE hOffGif ;
static  BOOL bPowerOff;
static	BOOL bMC55IsOff;
static	BOOL bShowPowerOffWindow;
//static	BOOL bPowerKeyRelease;
static	LRESULT	PowerOffWndProc(HWND, UINT, WPARAM, LPARAM);

extern void PowerOff();
extern BOOL	f_ChangePowerMode ( BOOL bLimit );
extern void ML_Destroy(void);
extern BOOL DeleteFont(void);
extern void DHI_CloseRTC(void);
extern BOOL DHI_CheckChargerStatus(void);
extern int	SetAutoUpWhilePowerOff(void);

/********************************************************************
* Function   PowerOffWndProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static	LRESULT	PowerOffWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                               LPARAM lParam)
{
    LRESULT lResult = FALSE;
    HDC     hdc;    
    SIZE    size;
    int     x=0, y=0;
    
    switch (wMsgCmd)
    {
    case WM_CREATE:

		pAppMain->pVtbl->Destroy(pAppMain, 0);

		f_sleep_register(INPOWEROFF);

		if ( bShowPowerOffWindow )
		{
			GetImageDimensionFromFile("/rom/progman/turn off.gif",&size);
			
			if (size.cx < DESK_WIDTH)
				x = (DESK_WIDTH - size.cx)/2;
			if (size.cy < DESK_HEIGHT)
				y = (DESK_HEIGHT - size.cy)/2;
			
			hOffGif = StartAnimatedGIFFromFile(hWnd, "/rom/progman/turn off.gif", x, y, 0);
		}

		// set automatically power on alarm
		SetAutoUpWhilePowerOff();

		// set alarm when power off
		RTC_Release();

		SMSDISPATCH_Shutdown();
		
		printf("\r\n Progman call ME_ShutdownModule\r\n");
        ME_ShutdownModule( hWnd, WM_SHUTDOWN );

		SetTimer( hWnd, 1, 15*1000, NULL);
        PrioMan_CallMusic(PRIOMAN_PRIORITY_POWEROFF, 0);        
        break;
	case WM_ERASEBKGND:
		break;
/*
	case WM_KEYUP:
		switch(wParam)
		{
		case VK_F6:
			bPowerKeyRelease = TRUE;
			break;
		}
		break;
*/
    case WM_PAINT :  
		hdc= BeginPaint(hWnd, NULL);
		if ( bShowPowerOffWindow )
		{
			PaintAnimatedGIF(hdc, hOffGif);
		} 
		EndPaint(hWnd, NULL);  
        break;

	case WM_SHUTDOWN:
		// MC55 has shutdown
		printf("\r\nMC55 has shutdown successfully.\r\n");
		bMC55IsOff = TRUE;
		KillTimer(hWnd,1);
		SetTimer( hWnd, 1, 2000, NULL);
		break;

    case WM_TIMER:
		if ( !bMC55IsOff )	// shutdown MC55 compulsively if time out
		{
			printf("\r\nWe have to power off compulsively.\r\n");
			DHI_ShutDownWirelessModule();
			bMC55IsOff = TRUE;
		}

//		if ( bPowerKeyRelease )
		{
		//	pAppMain->pVtbl->Destroy(pAppMain, 0);
			printf("\r\nNow, shutdown the phone...\r\n");
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		//	DestroyWindow(hWnd);
		}
		
        break;
	case WM_CLOSE:
		{
			if (hOffGif)
			{
				EndAnimatedGIF(hOffGif);
				hOffGif = NULL;
			}
			DlmNotify (PMM_NEWS_DISABLE,PMF_KEYLIGHT|PMF_MAIN_BKLIGHT);
            
			PrioMan_EndCallMusic(PRIOMAN_PRIORITY_POWEROFF, TRUE);
			DHI_CloseRTC();
			
			// umount jffis2 and fat16
			if ( 0 != umount("/mnt/flash/") )
				printf("\r\n!!!umount /mnt/flash error!!!\r\n");
			if ( 0 != umount("/mnt/fat16/") )
				printf("\r\n!!!umount /mnt/fat16 error!!!\r\n");
			
			if (DHI_CheckChargerStatus())
			{
				printf("\r\ncharger is connected, enter charger power off...\r\n");
				
				// we should reset the phone when the charger is connected at this time
#ifndef	_EMULATE_
				DHI_Reset();
#else
				exit(0);
#endif
			}
			else
			{
				printf("\r\nNo charger is connected, shutdown right now\r\n");
				ML_Destroy();	
				DeleteFont();
				// RTC_Release(); should be called before shut down MC55
#ifndef	_EMULATE_
				PowerOff();
#else
				exit(0);
#endif
			}

		}
		break;

    case WM_DESTROY:
               
        KillTimer(hWnd, 1);
   
		UnregisterClass("PlayPowerOffWindowClass", NULL);

		break;
		
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}    
/********************************************************************
* Function   PlayPowerOff  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL PlayPowerOff(BOOL bPlay)
{
    WNDCLASS wc;

	bPowerOff = TRUE;
	bShowPowerOffWindow = bPlay;
    
    wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    
    // power on
    wc.lpfnWndProc   = PowerOffWndProc;
    wc.lpszClassName = "PlayPowerOffWindowClass";
    if (!RegisterClass(&wc))
        return FALSE;
    
	CreateWindow("PlayPowerOffWindowClass",
					"",
					WS_VISIBLE | WS_BORDER,
					0,	
					0,
					DESK_WIDTH,	
					DESK_HEIGHT,
					NULL, NULL,	NULL, NULL);	
	
	return TRUE;
}

/********************************************************************
* Function   PM_IsPowerOff 
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL PM_IsPowerOff(void)
{
	return bPowerOff;
}

