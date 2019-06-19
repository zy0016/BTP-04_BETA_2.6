 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : progman.lib
 *
 * Purpose  : handle idle window (that is screen save window)
 *            status window
 *            
\**************************************************************************/

#include "progman.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include "plx_pdaex.h"

#define ICON_WIDTH       13
#define ICON_HEIGHT      13
#define MAX_SELAPPNUM    3

//icon gps profile
#define PROFILE_ICON_OFF "/rom/setup/gps/gps_off.ico"   
#define ICON_BACKGROUND "/rom/progman/gps/gps_background_process.ico" 
//gps status 
#define ICON_STATUS_VALID "/rom/progman/gps/gps_icon.ico"
#define ICON_STATUS_SLEEP "/rom/progman/gps/gps_sleep.ico"
#define ICON_STATUS_ERROR "/rom/progman/gps/gps_error.ico"

//gps quality
#define ICON_QUALITY_LEVEL1 "/rom/progman/gps/gps_balloon1_13x13.ico"
#define ICON_QUALITY_LEVEL2 "/rom/progman/gps/gps_balloon2_11x13.ico"
#define ICON_QUALITY_LEVEL3 "/rom/progman/gps/gps_balloon3_9x13.ico"
#define ICON_QUALITY_LEVEL4 "/rom/progman/gps/gps_balloon4_7x13.ico"

typedef struct tagGPSInfoData
{
	int    iQuality;//Positioning quality
	BOOL   bTracking;
	BOOL   bBoost;
    BYTE   iPStatus;//Positioning status 
	BYTE   iGPSProfile;//GPS profile
	BYTE   iGPSStatus;//GPS Status

}GPSInfoData, *PGPSINFODATA;

static GPSInfoData *pGpsInfo = NULL;
static HWND  hGPSWnd ;

static  LRESULT GPSWndProc(HWND, UINT, WPARAM, LPARAM);
static BYTE GetGpsPositionStatus(void);
static BYTE GetGpsStatus(void);
static int GetGpsQuality(void);
static BOOL GetGpsBoostStatus(void);
static BYTE Get_GpsProfile(void);
/*********************************************************************\
* Function     GetGpsRect
* Purpose      
* Params      
* Return       
**********************************************************************/
static void GetGpsRect(int num, RECT* rect)
{
	RECT rc[12] =
	{
		{3,1,25,14},//GPS profile
		{26,1,39,14},//boost

		{47,1,60,14},
		{61,1,74,14},
		{75,1,88,14},
		{89,1,102,14},
		{103,1,117,14},//other
		
		{148,1,161,15},//quality 1
		{136,1,147,15},//2
		{126,1,135,15},//3
		{118,1,125,15},//4
		
		{162, 1, 175, 14}//gps status
	};

	SetRect(rect, rc[num].left, rc[num].top, rc[num].right, rc[num].bottom);
}
/*********************************************************************\
* Function     InitGPSRectInfo
* Purpose      
* Params      
* Return       
**********************************************************************/
static void InitGPSRectInfo()
{

	pGpsInfo = malloc(sizeof(GPSInfoData));

	memset(pGpsInfo, 0, sizeof(GPSInfoData));

	pGpsInfo->iPStatus = GetGpsPositionStatus();
	if(GetActiveGPSProfile() == -1)
		pGpsInfo->iGPSProfile = GPS_PROFILE_OFF;
	else
	{
		pGpsInfo->iGPSProfile = GPS_PROFILE_USED;
	}

	pGpsInfo->iGPSStatus = GetGpsStatus();

	pGpsInfo->iQuality = GetGpsQuality();

	pGpsInfo->bBoost = GetGpsBoostStatus();

	pGpsInfo->bTracking = 0;//call gps tracking

}
/*********************************************************************\
* Function     DrawGpsRect
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL DrawGpsRect(HWND hWnd, HDC hdc)
{
	COLORREF color;
	RECT pc ;
	static HICON  hIcon_GPSProfile, hIcon_Quality[4], hIcon_GPSStatus, hIcon_Boost;
	int i =0;
	GPSMODE gs;

//	for(i = 0; i< 4;i++)
//		hIcon_Quality[i] = NULL;

	if(pGpsInfo == NULL)
		return FALSE;

	pGpsInfo->iPStatus = GetGpsPositionStatus();//call gps interface
	
	if(GetActiveGPSProfile() == -1)
		pGpsInfo->iGPSProfile = GPS_PROFILE_OFF;
	else
	{
		pGpsInfo->iGPSProfile = GPS_PROFILE_USED;
	}

	pGpsInfo->iGPSStatus = GetGpsStatus();

	pGpsInfo->iQuality = GetGpsQuality();

	pGpsInfo->bBoost = GetGpsBoostStatus();

	pGpsInfo->bTracking = 0;//call gps tracking


	switch(pGpsInfo->iPStatus)
	{
	case GPS_PS_VALID:
		color = RGB(132,211,99);//green
		break;
	case GPS_PS_NOVALID:
		color = RGB(222,219,107);//yellow
		break;
	case GPS_PS_SLEEP:
		color = RGB(173, 231,231);//blue
		break;
	case GPS_PS_OFF:
		color = RGB(206, 211,214);//grey
		break;
	case GPS_PS_ERROR:
		color = RGB(214, 56, 57);//red
		break;
	}

   	GetClientRect(hWnd, &pc);
	ClearRect(hdc, &pc, color);

	if(hIcon_GPSProfile)
	{
		DestroyIcon(hIcon_GPSProfile);
		hIcon_GPSProfile = NULL;
	}

	switch(pGpsInfo->iGPSProfile)
	{
	case GPS_PROFILE_USED:
		GetGPSprofile(&gs,GetActiveGPSProfile());
		hIcon_GPSProfile = LoadImage(NULL, gs.cGPSIconName, IMAGE_ICON, 21, 
			ICON_HEIGHT, LR_LOADFROMFILE);
		if(hIcon_GPSProfile)
		{
			DrawIcon(hdc, 3, 1, hIcon_GPSProfile);
//			DestroyIcon(hIcon_GPSProfile);
//			hIcon_GPSProfile = NULL;
		}
		break;
	case GPS_PROFILE_OFF:
		
		hIcon_GPSProfile = LoadImage(NULL, PROFILE_ICON_OFF/*ICON_OFF*/, IMAGE_ICON, 21,
			ICON_HEIGHT, LR_LOADFROMFILE);
		if(hIcon_GPSProfile)
		{
			DrawIcon(hdc, 3, 1, hIcon_GPSProfile);
		}
		break;
	}
	
	if(hIcon_Boost)
	{
		DestroyIcon(hIcon_Boost);
		hIcon_Boost = NULL;
	}

	if(pGpsInfo->bBoost)
	{
		hIcon_Boost = LoadImage(NULL, ICON_BACKGROUND/*ICON_ORTHER*/, IMAGE_ICON, ICON_WIDTH,
			ICON_HEIGHT, LR_LOADFROMFILE);
		
		if(hIcon_Boost)
		{
			DrawIcon(hdc, 26, 1, hIcon_Boost);
		}
	}

	if(hIcon_GPSStatus)
	{
		DestroyIcon(hIcon_GPSStatus);
		hIcon_GPSStatus = NULL;
	}
	
	switch(pGpsInfo->iGPSStatus)
	{
	case GPS_VALID:
		hIcon_GPSStatus = LoadImage(NULL, ICON_STATUS_VALID, IMAGE_ICON, ICON_WIDTH, 
			ICON_HEIGHT, LR_LOADFROMFILE);
		break;

	case GPS_SLEEP:
		hIcon_GPSStatus = LoadImage(NULL, ICON_STATUS_SLEEP, IMAGE_ICON, ICON_WIDTH, 
			ICON_HEIGHT, LR_LOADFROMFILE);
		break;

	case GPS_ERROR:
		hIcon_GPSStatus = LoadImage(NULL, ICON_STATUS_ERROR, IMAGE_ICON, ICON_WIDTH, 
			ICON_HEIGHT, LR_LOADFROMFILE);
		break;

	}

	if(hIcon_GPSStatus)
	{
		DrawIcon(hdc, 162, 1, hIcon_GPSStatus);
	}

	for(i = 0; i < QUALITY_LEVEL4 ;i++)
	{
		if(hIcon_Quality[i])
		{
			DestroyIcon(hIcon_Quality[i]);
			hIcon_Quality[i] = NULL;
		}
	}

	switch(pGpsInfo->iQuality)
	{
	case QUALITY_LEVEL4:
		hIcon_Quality[3] =  LoadImage(NULL, ICON_QUALITY_LEVEL4, IMAGE_ICON, 7, 
			13, LR_LOADFROMFILE);
	case QUALITY_LEVEL3:
		hIcon_Quality[2] =  LoadImage(NULL, ICON_QUALITY_LEVEL3, IMAGE_ICON, 9, 
			13, LR_LOADFROMFILE);
	case QUALITY_LEVEL2:
		hIcon_Quality[1] =  LoadImage(NULL, ICON_QUALITY_LEVEL2, IMAGE_ICON, 11, 
			13, LR_LOADFROMFILE);
	case QUALITY_LEVEL1:
		hIcon_Quality[0] =  LoadImage(NULL, ICON_QUALITY_LEVEL1, IMAGE_ICON, ICON_WIDTH, 
			ICON_HEIGHT, LR_LOADFROMFILE);
		break;
	}
    
	i = 0;
	while (i < pGpsInfo->iQuality)
	{
		RECT rect;

		memset(&rect, 0, sizeof(RECT));
		GetGpsRect(i + 7, &rect);
		if(hIcon_Quality[i])
			DrawIcon(hdc ,rect.left, rect.top, hIcon_Quality[i]);
		i++;
	}

	return TRUE;
   
}
/*********************************************************************\
* Function     AppToolCreate
* Purpose      
* Params      
* Return       
**********************************************************************/
BOOL	AppGPSCreate( void )
{
    WNDCLASS wc;
    COLORREF color;
    
    wc.style         = 0;
    wc.lpfnWndProc   = GPSWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;   
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "GPSWindowClass";
    
    color = SetupGetDeskColor(SN_GETBARCOLOR);    
    wc.hbrBackground = NULL;
    
    if (!RegisterClass(&wc))
        return FALSE;	
    
    hGPSWnd = CreateWindow("GPSWindowClass",
        "",
        NULL,
        0, 0, 
        DESK_WIDTH, TITLEBAR_HEIGHT,
        NULL, NULL, NULL, NULL);	
     
    return TRUE;
}
/*********************************************************************\
* Function     GPSWndProc
* Purpose      
* Params      
* Return       
**********************************************************************/
static LRESULT GPSWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                                  LPARAM lParam)
{
    LRESULT     lResult = FALSE;	
    HDC         hdc;
	HICON		hIcon = NULL;
    
    switch (wMsgCmd)
    {    
    case WM_QUERYSYSEXIT:
        return SYSEXIT_CONTINUE;
		
	case WM_SYSEXIT:
		DestroyWindow(hWnd);
		break;
	case WM_CREATE:
		InitGPSRectInfo();
		break;
	case WM_GPSFRASH:
		InvalidateRect(hWnd, NULL, TRUE);
		break;
        
    case WM_PAINT:
		{
			RECT pc;

			hdc = BeginPaint(hWnd, NULL);
			GetClientRect(hWnd, &pc);

			DrawGpsRect(hWnd, hdc);
			
			EndPaint(hWnd, NULL);
		}
        break;
		
    case WM_DESTROY:
		hGPSWnd = NULL;
		if(hIcon != NULL)
		{
			DestroyIcon(hIcon);
			hIcon = NULL;
		}
		UnregisterClass("GPSWindowClass", NULL);
        break;
		
	default : 
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
    }
    return lResult;
}
/*********************************************************************\
* Function     GetGPSWnd
* Purpose      
* Params      
* Return       
**********************************************************************/
HWND GetGPSWnd(void)
{
	return hGPSWnd;
}
/*********************************************************************\
* Function     SetGpsPStatus
* Purpose      
* Params      
* Return       
**********************************************************************/
static BYTE iPosStatus =  GPS_PS_OFF;
BOOL SetGpsPStatus(BYTE iPstatus)
{
	iPosStatus = iPstatus;

	return TRUE;
}
static BYTE GetGpsPositionStatus(void)
{
	return iPosStatus;
}
/*********************************************************************\
* Function     SetGpsProfile
* Purpose      
* Params      
* Return       
**********************************************************************/
static BYTE iGPS_Profile;
BOOL Set_GpsProfile(BYTE  iGPSProfile)
{
	iGPS_Profile = iGPSProfile;
	return TRUE;
}
static BYTE Get_GpsProfile(void)
{
	return iGPS_Profile;
}
/*********************************************************************\
* Function     SetGpsQuality
* Purpose      
* Params      
* Return       
**********************************************************************/
static int iGpsQuality ;
BOOL SetGpsQuality(int   iQuality)
{
	iGpsQuality = iQuality;
	return TRUE;
}
static int GetGpsQuality(void)
{
	return iGpsQuality;
}
/*********************************************************************\
* Function     SetGpsStatus
* Purpose      
* Params      
* Return       
**********************************************************************/
static BYTE iGPS_Status;
BOOL SetGpsStatus(BYTE iGpsStatus)
{
	iGPS_Status = iGpsStatus;
	return TRUE;
}
static BYTE GetGpsStatus(void)
{
	return iGPS_Status;
}
/*********************************************************************\
* Function     SetGpsBoostStatus
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL bBoost;
BOOL SetGpsBoostStatus(BOOL bOn)
{
	bBoost = bOn;
	return TRUE;
}
static BOOL GetGpsBoostStatus(void)
{
	return bBoost; 
}
