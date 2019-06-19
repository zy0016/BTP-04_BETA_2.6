/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : AlmingWnd.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "plx_pdaex.h"
#include "pubapp.h"
#include "winpda.h"
#include "hpimage.h"
#include "Mullang.h"
#include "setting.h"
#include "pmalarm.h"
#include "pmi.h"
#include "Almglobal.h"
#include "prioman.h"

#define ID_SNOOZE		1
#define ID_OFF			2
#define TimerID_1Min	3
#define TimerID_5Min	4
#define ID_TIMER        5
#define TimerID_Poweron 6
#define TEMPWND_TIMER   7
#define SNOOZEPOWEROFF_TIMER   8

#define CONFIRM_POWON  WM_USER + 300
#define WM_SNOOZEPOWEROFF WM_USER + 301
#define IDS_SNOOZE		ML("Snooze")
#define IDS_OFF			ML("Off")
#define IDS_YES			ML("Yes")
#define IDS_NO			ML("No")
 

typedef struct tagAlmingWndNode
{
   HWND hWndAlming;
   struct tagAlmingWndNode * pPreInfo;
   struct tagAlmingWndNode * pNextInfo;
   
}AlmingWndNode, *PAlmingWndNode;
PAlmingWndNode pAlmingWndNodeHead, pAlmingWndNodeEnd, pAlmingWndNodeCur;

typedef struct tagAlmingWndGlobalData
{
	HWND hWndAlming;	// indicate alaming window handle
	BOOL bPlayMusic;
	BOOL bSnooze;		// if alarm is snooze 
	BOOL bAlarmPowerOn;	// if alarm mode poweron
	int nAlarmID;		// indicate alarm id
	int nTimer_Refresh;	// timer for refresh the display current time
	int nTimer_1Min;	// timer for snooze alarm music 
	int nTimer_5Min;	//timer for normal alarm music
	int nTimer_Poweron; //timer for long poweron key
	int nTimer_SnoozeDelay; //timer for snooze delay poweroff
	SYSTEMTIME systime;  //current system time
	AlarmSetting setting; //current alarm setting
	BOOL bChangedLock; //current key or phone lock state
}ALMINGWNDGLOBALDATA, *PALMINGWNDGLOBALDATA;


static HWND hTempWnd;
static DWORD wSecond;


static LRESULT AlmingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
HWND GetAlarmTopWnd(void);
BOOL Clock_IsAlarming(void);
DWORD TempWindow(int nID);
static LRESULT TempWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL IsNextAlmWndExist(void);
static int AddAlarmingWndNode(HWND hAlmingWnd);
static int DelTopAlarmingWndNode(PAlmingWndNode pAlmNode);
extern BOOL GetAppCallInfo(void);


/********************
 * 报警信息窗口入口
 ********************/

DWORD AlarmWindow(int nID, BOOL bSnooze, BOOL bAlarmPwrOn, BOOL bAutoPowOn)
{
	WNDCLASS wc;	// 窗口类
    DWORD dwRet;
	ALMINGWNDGLOBALDATA Data;

	
	memset(&Data, 0, sizeof(ALMINGWNDGLOBALDATA));
	Data.nAlarmID = nID;
	Data.bSnooze = bSnooze;
	Data.bAlarmPowerOn = bAlarmPwrOn;
	Configure_Get(Data.nAlarmID, &Data.setting);

    dwRet = (DWORD)TRUE;
	
	wc.style         = 0;
	wc.lpfnWndProc   = AlmingWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof(ALMINGWNDGLOBALDATA);
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "AlmingWndClass";

/*
	if (!RegisterClass(&wc))
		return FALSE;*/

	RegisterClass(&wc);
	

	if(Data.bAlarmPowerOn)
		Data.hWndAlming = CreateWindow("AlmingWndClass",
		ML("Alarm clock"),
		PWS_STATICBAR |WS_VISIBLE,
		0,0,176,220,
		NULL,
		NULL,
		NULL,
		(PVOID)&Data
		);

	else
		Data.hWndAlming = CreateWindow("AlmingWndClass",
		ML("Alarm clock"),
		PWS_STATICBAR |WS_VISIBLE| WS_CAPTION,
		PLX_WIN_POSITION,
		NULL,
		NULL,
		NULL,
		(PVOID)&Data
		);
	
	if (!Data.hWndAlming) 
	{
		return FALSE;
	}
	
	AddAlarmingWndNode(Data.hWndAlming);
	// 创建成功
	dwRet = DlmNotify(PMM_NEWS_ENABLE_SCRSAVE, 0);
	DlmNotify(PMM_NEWS_ENABLE_SCRSAVE, dwRet);
	if (Data.bSnooze ) 
		SendMessage(Data.hWndAlming, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_SNOOZE, 1), (LPARAM)IDS_SNOOZE);
	
	SendMessage(Data.hWndAlming, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_OFF, 0), (LPARAM)IDS_OFF);
	BringWindowToTop(Data.hWndAlming);
	PostMessage(GetCapture(), WM_CANCELMODE, 0, 0);
	ShowWindow(Data.hWndAlming, SW_SHOW);
	UpdateWindow(Data.hWndAlming);

    return dwRet;
}

/***************************
 * 显示报警信息窗口处理过程
 ***************************/

static LRESULT AlmingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hbmpTip = NULL;
	LRESULT lResult = TRUE;
	PALMINGWNDGLOBALDATA pData;
	
	pData = GetUserData(hWnd);
    switch (wMsgCmd)
    {
		case WM_CREATE:			
			{

				LPCREATESTRUCT lpCreateStruct;
				
				lpCreateStruct = (LPCREATESTRUCT)lParam;
				memcpy(pData, lpCreateStruct->lpCreateParams, sizeof(ALMINGWNDGLOBALDATA));
				pData->nTimer_Refresh = SetTimer(hWnd, ID_TIMER, 60000, NULL);				
				
				PrioMan_EndCallMusic(PRIOMAN_PRIORITY_ALARM, TRUE);
				pData->bPlayMusic = PrioMan_CallMusic(PRIOMAN_PRIORITY_ALARM, 0);
				
				if (pData->setting.nSnooze) 
				{
						pData->nTimer_1Min = SetTimer(hWnd, TimerID_1Min, 60000, NULL);
				//	pData->nTimer_1Min = SetTimer(hWnd, TimerID_1Min, 15000, NULL);
				}
				else
				{
						pData->nTimer_5Min = SetTimer(hWnd, TimerID_5Min, 60000 * 5, NULL);
				//	pData->nTimer_5Min = SetTimer(hWnd, TimerID_5Min, 30000, NULL);
				}
				
				
				if(PM_GetkeyLockStatus() ||  PM_GetPhoneLockStatus())
				{
					DlmNotify(PS_LOCK_ENABLE, FALSE);
					pData->bChangedLock = TRUE;
				}
			//	f_sleep_register( CLOCK_ALARM);
				f_DisablePowerOff(CLOCKALARM);	//*******
				DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);//开屏
				DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);	//屏亮20秒
				DlmNotify (PMM_NEWS_ENABLE,PMF_AUTO_SHUT_MAIN_LIGHT);//20秒后可以自动关屏	
			

		
			}
			
			break;

		case WM_TIMER:
			switch (wParam)
			{
			case ID_TIMER:	
				{		
					RECT rcClient;					
					printf("^^^^^^^^^^^^^^Begin to refresh display time!!!\r\n");
					GetClientRect(hWnd, &rcClient);
					InvalidateRect(hWnd, &rcClient, FALSE);
						
					
				}
				
				break;
			case TimerID_1Min:
				if (pData->nTimer_1Min) 
				{
					KillTimer(hWnd, TimerID_1Min);
					pData->nTimer_1Min = 0;
				}
				
				printf("^^^^^^^^^^^^^^finish 1Min alarm!!!\r\n");
				if (pData->bPlayMusic)
				{
					PrioMan_EndCallMusic(PRIOMAN_PRIORITY_ALARM, TRUE);
				}
				
				break;
			case TimerID_5Min:
				if (pData->nTimer_5Min) 
				{
					KillTimer(hWnd, TimerID_5Min);
					pData->nTimer_5Min = 0;
				}
				
				printf("^^^^^^^^^^^^^^finish to 5Min alarm!!!\r\n");
				if (pData->bPlayMusic) 
				{
					PrioMan_EndCallMusic(PRIOMAN_PRIORITY_ALARM, TRUE);
				}
				
				break;
			case TimerID_Poweron:
				if(pData->nTimer_Poweron)
				{
					KillTimer(hWnd,TimerID_Poweron);				
					if (pData->setting.nSnooze > Snooze_Off) 
						PostMessage(hWnd, WM_COMMAND, ID_SNOOZE, NULL);
					else
						PostMessage(hWnd, WM_COMMAND, ID_OFF, NULL);
				}			
				break;
			case SNOOZEPOWEROFF_TIMER:
				if(pData->nTimer_SnoozeDelay)
				{
					printf("<<<<<<<<<<<<<<<<<<clock>>>>>>>>>>>>>>>>SNOOZEPOWEROFF_TIMER coming!\r\n");
					KillTimer(hWnd,SNOOZEPOWEROFF_TIMER);
					DlmNotify(PS_ALARMMODE, FALSE); //最后一个界面，选snooze，则设置snooze alarm并关机
					PostMessage(hWnd, WM_CLOSE, 0, 0);
				}				
				break;
			}
			break;

        case WM_ACTIVATE:
		    if (wParam == WA_ACTIVE)
                SetFocus(hWnd);
            break;

		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_RETURN:
				if (pData->setting.nSnooze > Snooze_Off) 
					PostMessage(hWnd, WM_COMMAND, ID_SNOOZE, NULL);
				
				break;
			case VK_F10:
				PostMessage(hWnd, WM_COMMAND, ID_OFF, NULL);
				break;
			case VK_F6:
				if(pData->bAlarmPowerOn)
				{
					pData->nTimer_Poweron = SetTimer(hWnd,TimerID_Poweron,1000,NULL);
				
				}			
				break;
			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
			}
			break;

		case WM_KEYUP:
			switch(LOWORD(wParam))
			{
			case VK_F6:
				if(pData->bAlarmPowerOn)
				{
					KillTimer(hWnd,TimerID_Poweron);				
					
				}
				break;
			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
			}			
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case ID_SNOOZE:
				{
					
				
					if (pData->bPlayMusic) 
						PrioMan_EndCallMusic(PRIOMAN_PRIORITY_ALARM, TRUE);
					Configure_Get(pData->nAlarmID, &Alarm_Settings[pData->nAlarmID]);				
					ResetNode(pData->nAlarmID, 1, &Alarm_Settings[pData->nAlarmID]);
					
					if (IsAlarmPowerOn() && !IsNextAlmWndExist()) 
					{
						pData->nTimer_SnoozeDelay = SetTimer(hWnd, SNOOZEPOWEROFF_TIMER, 2000, NULL);	
						printf("<<<<<<<<<<<<<<<<<<clock>>>>>>>>>>>>>>>>SNOOZEPOWEROFF_TIMER set!\r\n");
					}
					else						
						PostMessage(hWnd, WM_CLOSE, 0, 0);			
				
				}
				break;
			case ID_OFF:
				{	
					
					if (pData->bPlayMusic) 
						PrioMan_EndCallMusic(PRIOMAN_PRIORITY_ALARM, TRUE);					
					Configure_Get(pData->nAlarmID, &Alarm_Settings[pData->nAlarmID]);	
					Alarm_Settings[pData->nAlarmID].nSnooze = Snooze_Off;				
					if (Alarm_Settings[pData->nAlarmID].bIsOpen && Repeat_Once == Alarm_Settings[pData->nAlarmID].nRepeat) 
					{
						Alarm_Settings[pData->nAlarmID].bIsOpen = FALSE;
						Configure_Set(pData->nAlarmID, Alarm_Settings[pData->nAlarmID]);
					}				
					ResetNode(pData->nAlarmID, 0, &Alarm_Settings[pData->nAlarmID]);					
				
					if (IsAlarmPowerOn() && !IsNextAlmWndExist()) 					
						PLXConfirmWinEx(0, hWnd, ML("Switch phone on ?"), Notify_Request, ML("Alarm Clock"), IDS_YES, IDS_NO, CONFIRM_POWON);
					else
						PostMessage(hWnd, WM_CLOSE, 0, 0);				
									
				}
				
				break;
				
			}
			break;
		case CONFIRM_POWON:
			if(IsAlarmPowerOn())
			{
				if (lParam)
					DlmNotify(PS_ALARMMODE, TRUE); //如果用户选择yes，开机
				else
					DlmNotify(PS_ALARMMODE, FALSE); //如果用户选择no。退出alarm	
			}
		
			PostMessage(hWnd, WM_CLOSE, 0, 0);	
			
			break;
	
		case WM_CLOSE:

		   
			if(!IsNextAlmWndExist())
			{
			//	f_sleep_unregister(CLOCK_ALARM);
				f_EnablePowerOff(CLOCKALARM);	//*******
			}
			else
			BringWindowToTop(pAlmingWndNodeEnd->pPreInfo->hWndAlming);
			DelTopAlarmingWndNode(pAlmingWndNodeEnd);
			
			if(pData->bChangedLock)
			{
				pData->bChangedLock = FALSE;
				DlmNotify(PS_LOCK_ENABLE, TRUE);
			}
			if (pData->nTimer_1Min) 
			{
				KillTimer(hWnd, TimerID_1Min);
				pData->nTimer_1Min = 0;
			}
			if (pData->nTimer_5Min)
			{
				KillTimer(hWnd, TimerID_5Min);
				pData->nTimer_5Min = 0;
			}
			if(pData->nTimer_Refresh)
			{
				KillTimer(hWnd, ID_TIMER);
				pData->nTimer_Refresh = 0;
			}			
			DestroyWindow(hWnd);
			
			break;

		case WM_DESTROY:
		
			printf("^^^^^^^^^^^^^^End alarm window!!!\r\n");
		
			UnregisterClass("AlmingWndClass", NULL);
		
			break;

		case WM_PAINT:
			{
				HDC hdc;
				char cTime[10];
				char cDate[20];	
			//	char cAlarmID[5];
            //	RECT rcAlarmID;
				RECT rGps, rcCaption, rcText, rcTime,rcClient;
                
				hdc = BeginPaint(hWnd, NULL);
				GetClientRect(hWnd, &rcClient);
				ClearRect(hdc, &rcClient, COLOR_TRANSBK);
				if(pData->bAlarmPowerOn)
				{			
					SetRect(&rGps, 0, 0,176, 15);
					ClearRect(hdc, &rGps, RGB(206, 211, 214));//grey			
					DrawIconFromFile(hdc, "/rom/progman/icon/icon_off.ico", 0, 0, 14, 14);
					SetBkMode(hdc, TRANSPARENT);
					SetRect(&rcCaption,rcClient.left,15,rcClient.right,46);
					DrawText(hdc, ML("Alarm clock"),-1,&rcCaption,DT_HCENTER | DT_VCENTER);
					SetRect(&rcText,rcClient.left,96,rcClient.right,119);
					DrawText(hdc, ML("Wake up"),-1,&rcText,DT_HCENTER | DT_VCENTER);					
					GetLocalTime(&pData->systime);
					GetTimeDisplay(pData->systime, cTime, cDate);
					SetRect(&rcTime,rcClient.left,119,rcClient.right,142);
					DrawText(hdc, ML(cTime),-1,&rcTime,DT_HCENTER | DT_VCENTER);	
				/*
					sprintf(cAlarmID, "ID:%d",pData->nAlarmID);
									SetRect(&rcAlarmID, rcClient.left,142,rcClient.right,165);
									DrawText(hdc, ML(cAlarmID),-1,&rcAlarmID,DT_HCENTER | DT_VCENTER);		*/
							
				
				}
				else
				{				
					SetBkMode(hdc, TRANSPARENT);
					SetRect(&rcText,rcClient.left,50,rcClient.right,(50+23));
					DrawText(hdc, ML("Wake up"),-1,&rcText,DT_HCENTER | DT_VCENTER);					
					GetLocalTime(&pData->systime);
					GetTimeDisplay(pData->systime, cTime, cDate);
					SetRect(&rcTime,rcClient.left,(50+23),rcClient.right,(50+23+23));
					DrawText(hdc, ML(cTime),-1,&rcTime,DT_HCENTER | DT_VCENTER);		
				}		  
				
				DeleteDC(hdc);				
				EndPaint(hWnd, NULL);
			}
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
    }

    return lResult;
}
//////////////////////////////////////////////////////////////////////////////
// Function	AddAlarmingWndNode
// Purpose	add a new alarming window to list
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static int AddAlarmingWndNode(HWND hAlmingWnd)
{
	PAlmingWndNode pAlmNode;
	
	pAlmNode = (PAlmingWndNode)malloc(sizeof(AlmingWndNode));
	if(NULL == pAlmNode)
		return 0;
	pAlmNode->hWndAlming = hAlmingWnd;
	if(NULL == pAlmingWndNodeEnd){	
		pAlmingWndNodeHead = pAlmingWndNodeEnd = pAlmNode;
		pAlmNode->pPreInfo = NULL;
		pAlmNode->pNextInfo = NULL;
	}
	else
	{
		pAlmNode->pNextInfo = NULL;
		pAlmNode->pPreInfo = pAlmingWndNodeEnd;
		pAlmingWndNodeEnd->pNextInfo = pAlmNode;
		pAlmingWndNodeEnd = pAlmNode;
	}
	pAlmingWndNodeCur = pAlmNode;	
	return 1;
}
//////////////////////////////////////////////////////////////////////////////
// Function	DelTopAlarmingWndNode
// Purpose	delete the top most window from list
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static int DelTopAlarmingWndNode(PAlmingWndNode pAlmNode)
{
	if(NULL == pAlmNode)
		return 0;	
	if(pAlmNode->pPreInfo == NULL)
	{
		pAlmingWndNodeHead = NULL;
		pAlmingWndNodeEnd = NULL;
	}		
	else
	{
		pAlmingWndNodeEnd = pAlmNode->pPreInfo;
		pAlmingWndNodeEnd->pNextInfo = NULL;	
	}
	free(pAlmNode);
	return 1;
}
/******************************************************************** 
* Function	   Clock_IsAlarming
* Purpose      judge if alarm window is shown.
* Params	  no
* Return	 	TRUE: alarm window shown 
*				FALSE: no alarm is coming
**********************************************************************/
BOOL Clock_IsAlarming(void)
{
	if(pAlmingWndNodeEnd)
		return TRUE;
	else
		return FALSE;
}
/******************************************************************** 
* Function	   IsNextAlmWndExist
* Purpose      judge if next alarm window exists.
* Params	  no
* Return	 	TRUE: Yes 
*				FALSE: No
**********************************************************************/
static BOOL IsNextAlmWndExist(void)
{
	
	BOOL	retval = FALSE; 
	if(pAlmingWndNodeEnd->pPreInfo)
		return TRUE;
	else
		return FALSE;	
}
/******************************************************************** 
* Function	   GetAlarmTopWnd
* Purpose      get the top most window handle.
* Params	  no
* Return	 	window handle
**********************************************************************/
HWND GetAlarmTopWnd()
{
	if(pAlmingWndNodeEnd)
		return pAlmingWndNodeEnd->hWndAlming;
	else
		return NULL;
		
}
/******************************************************************** 
* Function	   TempWindow
* Purpose      create a temp window because calling has higher priority.
* Params	  nID: alarm ID
* Return	 	
**********************************************************************/
DWORD TempWindow(int nID)
{
	WNDCLASS wc;
    DWORD dwRet;
	int nAlarmID;
	
    dwRet = (DWORD)TRUE;
	nAlarmID = nID;
	wc.style         = 0;
	wc.lpfnWndProc   = TempWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof(nAlarmID);
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "TempWndClass";
	
	RegisterClass(&wc);
	
	hTempWnd = CreateWindow("TempWndClass",
		NULL,
		NULL,
		PLX_WIN_POSITION,
		NULL,
		NULL,
		NULL,
		(PVOID)&nAlarmID
		);
	
	if (!hTempWnd) 
	{
		return FALSE;
	}
	
    return dwRet;
}


static LRESULT TempWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;
	int * pAlarmID;
	pAlarmID = GetUserData(hWnd);
    switch (wMsgCmd)
    {
	case WM_CREATE:	
		{
			LPCREATESTRUCT lpCreateStruct;
			
			lpCreateStruct = (LPCREATESTRUCT)lParam;
			memcpy(pAlarmID, lpCreateStruct->lpCreateParams, sizeof(int));
			printf("***********into TempWindow*************\n");
			SetTimer(hWnd, TEMPWND_TIMER, 2000, NULL);
		}

	
		break;	
	case WM_TIMER:
		switch (wParam)
		{
		case TEMPWND_TIMER:	
			{		
				if(!GetAppCallInfo())
				{					
					BOOL bAlarmPoweron = FALSE;
					KillTimer(hWnd, TEMPWND_TIMER);					
					bAlarmPoweron = IsAlarmPowerOn();				
					if(Alarm_Settings[*pAlarmID].nSnooze > Snooze_Off)
						AlarmWindow(*pAlarmID, 1, bAlarmPoweron, 0);
					else
						AlarmWindow(*pAlarmID, 0, bAlarmPoweron, 0);	
					PostMessage(hWnd, WM_CLOSE, 0, 0);			
					
				}
			}
			
			break;
		}
		break;	
			
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
			
		case WM_DESTROY:
			UnregisterClass("TempWndClass", NULL);
			break;
			
		case WM_PAINT:
			BeginPaint(hWnd, NULL);			
			EndPaint(hWnd, NULL);
			break;
			
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
    }
	
    return lResult;
}
